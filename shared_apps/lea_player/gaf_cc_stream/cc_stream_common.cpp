/***************************************************************************
 *
 * Copyright 2015-2023 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#include <string.h>
#include "cmsis.h"
#include "cmsis_os.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_location.h"
#include "plat_types.h"
#include "app_utils.h"
#include "gaf_dbg.h"
#include "gaf_codec_lc3.h"
#ifdef LEA_LHDC
#include "gaf_codec_lhdc.h"
#endif
#include "cc_stream_common.h"
#include "gaf_cc_stream_process.h"
#include "gaf_media_common.h"
#include "gaf_cp_stream.h"
/************************private macro defination***************************/
#define CC_STREAM_THREAD_STACK_SIZE        (1024*4)
#define CC_STREAM_SIGNAL_DATA_RECEIVED     (1 << 0)
#define CC_CAPTURE_SIGNAL_STOP             (1 << 1)
#define CC_CAPTURE_SIGNAL_START            (1 << 2)
#define CC_PLAYBACK_SIGNAL_STOP            (1 << 3)
#define CC_PLAYBACK_SIGNAL_START           (1 << 4)

#define PLAYBACK_INIT_REQ_BUF_SIZE      \
    ((sizeof(CC_PLAYBACK_INIT_REQ_T) + sizeof(CORE_PLAYBACK_INIT_INFO_T))*PLAYBACK_INSTANCE_MAX)
#define PLAYBACK_DEINIT_REQ_BUF_SIZE    \
    (sizeof(CC_PLAYBACK_DEINIT_REQ_T)*PLAYBACK_INSTANCE_MAX)
#define CAPTURE_INIT_REQ_BUF_SIZE       \
    ((sizeof(CC_CAPTURE_INIT_REQ_T) + sizeof(CORE_CAPTURE_INIT_INFO_T))*CAPTURE_INSTANCE_MAX)
#define CAPTURE_DEINIT_REQ_BUF_SIZE     \
    (sizeof(CC_CAPTURE_DEINIT_REQ_T)*CAPTURE_INSTANCE_MAX)

#define CC_PLAYBACK_MAX_FRAME_CNT   (16)
#define CC_PLAYBACK_MAX_PCM_CNT     (4)
#define CC_CAPTURE_MAX_FRAME_CNT    (8)
#define CC_CAPTURE_MAX_PCM_CNT      (4)

#define CC_STREAM_FREQ_BOOST_TIMEOUT (1500)

#ifndef AOB_CODEC_CP
#define CC_STREAM_TRACE(...) TRACE(0, ##__VA_ARGS__)
#else
#define CC_STREAM_TRACE(...)
#endif
/************************private type defination****************************/
typedef struct
{
    CC_PLAYBACK_INSTANCE playbackInstance[PLAYBACK_INSTANCE_MAX];
    CC_CAPTURE_INSTANCE captureInstance[CAPTURE_INSTANCE_MAX];

    cfifo playback_init_req_fifo;
    cfifo capture_init_req_fifo;
    cfifo playback_deinit_req_fifo;
    cfifo capture_deinit_req_fifo;

    uint8_t playback_init_req_buf[PLAYBACK_INIT_REQ_BUF_SIZE];
    uint8_t capture_init_req_buf[CAPTURE_INIT_REQ_BUF_SIZE];
    uint8_t playback_deinit_req_buf[PLAYBACK_DEINIT_REQ_BUF_SIZE];
    uint8_t capture_deinit_req_buf[CAPTURE_DEINIT_REQ_BUF_SIZE];

    GAF_AUDIO_STREAM_ENV_T *pStreamEnv;
    bool isProcessing;
} CC_STREAM_ENV_T;

/************************private variable defination************************/
CC_STREAM_ENV_T ccStreamEnv;

static osThreadId cc_stream_processor_thread_id = NULL;
osTimerId cc_stream_freq_boost_timer_id = NULL;

/**********************private function declaration*************************/
static void cc_stream_boost_timeout_handler(void const *n);
static void cc_stream_processor_thread(const void *arg);

osThreadDef(cc_stream_processor_thread, osPriorityAboveNormal, 1,
            (CC_STREAM_THREAD_STACK_SIZE), "cc_stream_processor_thread");
osTimerDef(CC_STREAM_FREQ_BOOST_TIMER, cc_stream_boost_timeout_handler);

/****************************function defination****************************/

static void cc_stream_boost_timeout_handler(void const *n)
{
    LOG_I("%s", __func__);
    app_sysfreq_req(APP_SYSFREQ_USER_STREAM_BOOST, APP_SYSFREQ_32K);
}

void cc_stream_boost_freq(void)
{
    LOG_I("%s ", __func__);
    if (NULL == cc_stream_freq_boost_timer_id)
    {
        cc_stream_freq_boost_timer_id =
            osTimerCreate(osTimer(CC_STREAM_FREQ_BOOST_TIMER), osTimerOnce, NULL);
    }
    osTimerStop(cc_stream_freq_boost_timer_id);
    osTimerStart(cc_stream_freq_boost_timer_id, CC_STREAM_FREQ_BOOST_TIMEOUT);
    app_sysfreq_req(APP_SYSFREQ_USER_STREAM_BOOST, APP_SYSFREQ_208M);
}

FRAM_TEXT_LOC
static CC_PLAYBACK_DATA_T* cc_playback_get_frame(uint8_t instance_handle)
{
    CC_PLAYBACK_INSTANCE *instance = &(ccStreamEnv.playbackInstance[instance_handle]);
    cfifo *frame_fifo = instance->playback_context.frame_fifo;
    CC_PLAYBACK_DATA_T *frame = (CC_PLAYBACK_DATA_T *)instance->playback_context.frame_cache;

    uint32_t header_len = sizeof(CC_PLAYBACK_DATA_T);
    POSSIBLY_UNUSED uint32_t frame_size = instance->playback_info.frameSize;
    uint16_t lastSeq = instance->playback_context.last_seq;
peek_again:
    if (cfifo_len(frame_fifo) >= header_len)
    {
        cfifo_peek_to_buf(frame_fifo, (uint8_t*)frame, header_len);
        if (cfifo_len(frame_fifo) < (header_len + frame->data_len))
        {
            CC_STREAM_TRACE("instance:%d seq:0x%02x len:%d-%d", instance_handle,
                frame->seq_nb, frame->data_len, cfifo_len(frame_fifo));
            return NULL;
        }
    }
    else
    {
        CC_STREAM_TRACE("instance:%d playback get frame err len:%d", instance_handle, cfifo_len(frame_fifo));
        return NULL;
    }

    if (INSTANCE_INITIALIZED == instance->status)
    {
        instance->playback_context.last_seq = lastSeq = frame->seq_nb - 1;
        instance->status = INSTANCE_STREAMING;
        CC_STREAM_TRACE("instance:%d first pkt seq:%d timeStamp:%u len:%d",
                instance_handle, frame->seq_nb, frame->time_stamp, frame->data_len);
    }

    frame->data = instance->playback_context.frame_cache + header_len;

    int16_t checkResult = frame->seq_nb - 1 - lastSeq;
    instance->playback_context.last_seq = frame->seq_nb;

    if (checkResult == 0)
    {
        cfifo_pop(frame_fifo, NULL, header_len);
        cfifo_pop(frame_fifo, frame->data, frame->data_len);
    }
    else if (checkResult > 0)
    {
        CC_STREAM_TRACE("[PLC] instance:%d seq: 0x%02x/ 0x%02x", instance_handle, frame->seq_nb, lastSeq);
        frame->isPLC = true;
    }
    else
    {
        CC_STREAM_TRACE("[POP] instance:%d seq: 0x%02x/ 0x%02x", instance_handle, frame->seq_nb, lastSeq);
        cfifo_pop(frame_fifo, NULL, (header_len + frame->data_len));
        goto peek_again;
    }

    return frame;
}

FRAM_TEXT_LOC
static void cc_process_decoding(uint8_t instance_handle)
{
    CC_PLAYBACK_INSTANCE *instance = &(ccStreamEnv.playbackInstance[instance_handle]);
    cfifo *pcm_fifo = instance->playback_context.pcm_fifo;
    uint32_t header_len = sizeof(CC_PLAYBACK_DATA_T);
    uint32_t pcm_size = instance->playback_info.pcmSize;
    CC_PLAYBACK_DATA_T *pcm_data = (CC_PLAYBACK_DATA_T*)instance->playback_context.pcm_cache;
    uint32_t ret = 0;

    CC_PLAYBACK_DATA_T *frame = cc_playback_get_frame(instance_handle);
    if (!frame) {
        return;
    }

    pcm_data->data = instance->playback_context.pcm_cache + header_len;
    pcm_data->data_len = pcm_size - header_len;
    pcm_data->isPLC = frame->isPLC;
    pcm_data->seq_nb = frame->seq_nb;
    pcm_data->time_stamp = frame->time_stamp;

    ret = instance->dec_func_list->decoder_decode_frame_func(
                instance_handle, &(instance->playback_info.decoder_info),
                frame->data_len, frame->data, pcm_data->data, pcm_data->isPLC);
    if (CODEC_ERR == ret) {
        CC_STREAM_TRACE("dec err!!");
        memset(pcm_data->data, 0, pcm_size - header_len);
    }

    if (instance->playback_algo_func_list) {
        instance->playback_algo_func_list->algo_run(pcm_data->data, pcm_data->data_len);
    }

    cfifo_put(pcm_fifo, instance->playback_context.pcm_cache, pcm_size);
}

#ifdef ISO_USE_INTERSYS2
extern int dsp2btc_send_data(const uint8_t *buf, uint16_t size);
#define MAX_LEA_OCTETS_PER_FRAME  1024
#define HCI_CONN_HANDLE_MASK 0x0fff
#define APP_BLE_AUDIO_ISO_PACKET_IDX  2
static uint8_t iso_packet_buffer[APP_BLE_AUDIO_ISO_PACKET_IDX][13+MAX_LEA_OCTETS_PER_FRAME*2];
static uint16_t iso_sdu_sequence_num = 0;
#define BLE_ASE_HDL_TO_INDEX(aes_handle)  (aes_handle-0x101)
#define HCI_ISO_SDU_MAX_HEADER_ULL_LEN  8 // include Time_Stamp

struct hci_iso_timestamp_ull_header_t {
    uint8_t iso_hci_type;
    uint16_t iso_conn_handle;       // conhdl 0~11 pb_flag 12~13 ts_flag 14 rfu 15
    uint16_t iso_data_load_length;  // length 0~13 rfu 14~15
    uint32_t iso_sdu_timestamp;
    uint16_t iso_sdu_sequence_num;
    uint16_t iso_sdu_length;        // sdu_length 0~11 rfu 12~13 ps_flag 14~15
} __attribute__ ((packed));

#define HOST_OS_LITTLE_ENDIAN 1

#if HOST_OS_LITTLE_ENDIAN
static inline uint16_t co_host_to_uint16_le(uint16_t n) { return n; }
static inline uint16_t co_uint16_le_to_host(uint16_t n) { return n; }
static inline uint32_t co_host_to_uint32_le(uint32_t n) { return n; }
static inline uint32_t co_uint32_le_to_host(uint32_t n) { return n; }
static inline uint32_t co_host_to_uint24_le(uint32_t n) { return n; }
static inline uint32_t co_uint24_le_to_host(uint32_t n) { return n; }
#else
static inline uint16_t co_host_to_uint16_le(uint16_t n) {
    return ((n & 0xff) << 8) | (n >> 8);
}
static inline uint32_t co_host_to_uint32_le(uint32_t n) {
    return ((n & 0xff) << 24) | ((n & 0xff00) << 8) | ((n & 0xff0000) >> 8) | ((n & 0xff000000) >> 24);
}
static inline uint32_t co_host_to_uint24_le(uint32_t n) {
    return ((n << 16) & 0xFF0000) | (n &0xFF00) | ((n >> 16) & 0xFF);
}
static inline uint16_t co_uint16_le_to_host(uint16_t n) {
    return co_host_to_uint16_le(n);
}
static inline uint32_t co_uint32_le_to_host(uint32_t n) {
    return co_host_to_uint32_le(n);
}
static inline uint32_t co_uint24_le_to_host(uint32_t n) {
    return co_host_to_uint24_le(n);
}
#endif 

static void hci_tx_btc_audio_test(uint16_t ase_handle,const uint8_t *buf, uint16_t size, uint32_t ref_iso_time)
{
    uint8_t buf_idx = BLE_ASE_HDL_TO_INDEX(ase_handle)%APP_BLE_AUDIO_ISO_PACKET_IDX;
    ASSERT(buf_idx < APP_BLE_AUDIO_ISO_PACKET_IDX,"buf_idx err %d", buf_idx);
    iso_sdu_sequence_num +=1 ;
    uint32_t iso_sdu_timestamp = ref_iso_time;
    uint16_t iso_conn_handle = (ase_handle & HCI_CONN_HANDLE_MASK) | 0x6000;
    uint16_t iso_data_load_length = HCI_ISO_SDU_MAX_HEADER_ULL_LEN + size;
    struct hci_iso_timestamp_ull_header_t *iso_packet = NULL;
    iso_packet = (struct hci_iso_timestamp_ull_header_t *)iso_packet_buffer[buf_idx];
    iso_packet->iso_hci_type = 0x05; //HCI_DATA_TYPE_ISO
    iso_packet->iso_conn_handle = co_host_to_uint16_le(iso_conn_handle);
    iso_packet->iso_data_load_length = co_host_to_uint16_le(iso_data_load_length);
    iso_packet->iso_sdu_timestamp = co_host_to_uint32_le(iso_sdu_timestamp);
    iso_packet->iso_sdu_sequence_num = co_host_to_uint16_le(iso_sdu_sequence_num);
    iso_packet->iso_sdu_length = co_host_to_uint16_le(size);

    memcpy(iso_packet + 1, buf, size);
    size += sizeof(hci_iso_timestamp_ull_header_t);
    dsp2btc_send_data((const uint8_t *)iso_packet, size);
}

static uint8_t gaf_offload_get_audio_location_l_r_cnt(uint32_t audio_location_bf)
{
    uint8_t location_l_r_cnt = 0;
    if (audio_location_bf & (0x00000400 | 0x00000001))
    {
        location_l_r_cnt++;
    }
    if (audio_location_bf & (0x00000800 | 0x00000002))
    {
        location_l_r_cnt++;
    }

    return location_l_r_cnt;
}

static void gaf_mobile_offload_audio_process_pcm_data_send(const uint8_t *buf, uint16_t size, uint32_t ref_iso_time)
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = ccStreamEnv.pStreamEnv;
    uint8_t *payload = (uint8_t *)buf;

    uint32_t payload_len_per_channel = 0;
    uint32_t channel_shift = 0;
    uint32_t audio_allocation_bf = 0;
    uint8_t audio_allocation_cnt = 0;
    bool stereo_channel_support = true;
    bool is_right_channel = false;
    // Check all ase for streaming send
    // DUMP8("%02X ", &pStreamEnv->stream_info.captureInfo, 16);
    for (uint8_t i = 0; i < GAF_AUDIO_ASE_TOTAL_COUNT; i++)
    {
        if (pStreamEnv->stream_info.captureInfo.aseChInfo[i].iso_channel_hdl == GAF_AUDIO_INVALID_ISO_CHANNEL)
        {
            continue;
        }
        channel_shift = 0;
        audio_allocation_bf = pStreamEnv->stream_info.captureInfo.aseChInfo[i].allocation_bf;
        is_right_channel = ((audio_allocation_bf & (0x00000800 | 0x00000002)) != 0);
        audio_allocation_cnt = gaf_offload_get_audio_location_l_r_cnt(audio_allocation_bf);

        // Config palyload len  for AUD_CHANNEL_NUM_2
        payload_len_per_channel = size / 2;
        if (!stereo_channel_support)
        {
            // Only one channel in this ASE, choose one pcm channel
            if (is_right_channel)
            {
                // shift to right channel
                channel_shift += payload_len_per_channel;
            }
        }
        else// if STEREO CHANNNEL
        {
            //Check is there use two ASE
            if (audio_allocation_cnt == 1 && is_right_channel)
            {
                // shift to right channel
                channel_shift += payload_len_per_channel;
            }
            else if (audio_allocation_cnt == 2)
            {
                payload_len_per_channel = size;
            }
        }
        LOG_D("[CAPTURE SEND] p_len:%d stereo supp: %d, allocation_bf: 0x%x, shift :%d",
              payload_len_per_channel, stereo_channel_support, audio_allocation_bf, channel_shift);
        hci_tx_btc_audio_test(pStreamEnv->stream_info.captureInfo.aseChInfo[i].ase_handle, payload + channel_shift, payload_len_per_channel, ref_iso_time);
    }
}
#endif

FRAM_TEXT_LOC
static void cc_process_encoding(uint8_t instance_handle)
{
    CC_CAPTURE_INSTANCE *instance = &(ccStreamEnv.captureInstance[instance_handle]);
    cfifo *pcm_fifo = instance->capture_context.pcm_fifo;
    CC_CAPTURE_DATA_T *frame = (CC_CAPTURE_DATA_T*)instance->capture_context.frame_cache;
    CC_CAPTURE_DATA_T *pcm_data = (CC_CAPTURE_DATA_T*)instance->capture_context.pcm_cache;

    uint32_t header_len = sizeof(CC_CAPTURE_DATA_T);
    uint32_t pcm_size = instance->capture_info.pcmSize;
    uint32_t ret = cfifo_pop(pcm_fifo, instance->capture_context.pcm_cache, pcm_size);
    if (FIFO_ERR == ret)
    {
        CC_STREAM_TRACE("capture get pcm err! len:%d", cfifo_len(pcm_fifo));
        return;
    }

    pcm_data->data = instance->capture_context.pcm_cache + header_len;

    uint32_t pcm_len = pcm_data->data_len;
    if (instance->capture_algo_func_list) {
        pcm_len = instance->capture_algo_func_list->algo_run(pcm_data->data, pcm_len);
    }

    uint8_t *pcm_data_after_algo = NULL;
    if (instance->capture_algo_func_list &&
        instance->capture_algo_func_list->get_resample_buf &&
        instance->capture_algo_func_list->get_resample_buf()) {
        pcm_data_after_algo = (uint8_t *)instance->capture_algo_func_list->get_resample_buf();
    } else {
        pcm_data_after_algo = pcm_data->data;
    }

    frame->time_stamp = pcm_data->time_stamp;
    frame->frame_size = pcm_data->frame_size;
    frame->data = instance->capture_context.frame_cache + header_len;

    ret = instance->enc_func_list->encoder_encode_frame_func(
            instance_handle, &(instance->capture_info.encoder_info),
            pcm_len, pcm_data_after_algo, pcm_data->frame_size, frame->data);
    if (ret == CODEC_ERR) {
        CC_STREAM_TRACE("enc err!!");
    }

#ifdef ISO_USE_INTERSYS2
    gaf_mobile_offload_audio_process_pcm_data_send(frame->data, frame->frame_size, frame->time_stamp);
#else
    cfifo *frame_fifo = instance->capture_context.frame_fifo;
    uint32_t normal_frame_size = instance->capture_info.frameSize;
    cfifo_put(frame_fifo, instance->capture_context.frame_cache, normal_frame_size);
#endif
}

FRAM_TEXT_LOC
static bool cc_process_again(void)
{
    uint8_t instance_handle = 0;
    CC_PLAYBACK_INSTANCE *playback_instance = NULL;
    CC_CAPTURE_INSTANCE *capture_instance = NULL;
    for (instance_handle = 0; instance_handle < PLAYBACK_INSTANCE_MAX; instance_handle++)
    {
        playback_instance = &(ccStreamEnv.playbackInstance[instance_handle]);
        if (INSTANCE_IDLE == playback_instance->status) {
            continue;
        }
        if (cfifo_len(playback_instance->playback_context.frame_fifo) >= \
            sizeof(CC_PLAYBACK_DATA_T) &&
            cfifo_free_space(playback_instance->playback_context.pcm_fifo) >= \
            playback_instance->playback_info.pcmSize)
        {
            return true;
        }
    }
    for (instance_handle = 0; instance_handle < CAPTURE_INSTANCE_MAX; instance_handle++)
    {
        capture_instance = &(ccStreamEnv.captureInstance[instance_handle]);
        if (INSTANCE_IDLE == capture_instance->status) {
            continue;
        }
        if (cfifo_len(capture_instance->capture_context.pcm_fifo) >= \
            capture_instance->capture_info.pcmSize &&
            cfifo_free_space(capture_instance->capture_context.frame_fifo) >= \
            capture_instance->capture_info.frameSize)
        {
            return true;
        }
    }
    return false;
}

FRAM_TEXT_LOC
void cc_stream_process(void)
{
    uint8_t instance_handle = 0;
    ccStreamEnv.isProcessing = true;
    do
    {
        for (instance_handle = 0; instance_handle < PLAYBACK_INSTANCE_MAX; instance_handle++)
        {
            CC_PLAYBACK_INSTANCE *instance = &(ccStreamEnv.playbackInstance[instance_handle]);
            if (INSTANCE_IDLE == instance->status) {
                continue;
            }
            if (cfifo_len(instance->playback_context.frame_fifo) >=  \
                sizeof(CC_PLAYBACK_DATA_T) &&
                cfifo_free_space(instance->playback_context.pcm_fifo) >= \
                instance->playback_info.pcmSize)
            {
                cc_process_decoding(instance_handle);
            }
        }
        for (instance_handle = 0; instance_handle < CAPTURE_INSTANCE_MAX; instance_handle++)
        {
            CC_CAPTURE_INSTANCE *instance = &(ccStreamEnv.captureInstance[instance_handle]);
            if (INSTANCE_IDLE == instance->status) {
                continue;
            }
            if (cfifo_len(instance->capture_context.pcm_fifo) >= \
                instance->capture_info.pcmSize &&
                cfifo_free_space(instance->capture_context.frame_fifo) >= \
                instance->capture_info.frameSize)
            {
                cc_process_encoding(instance_handle);
            }
        }
    } while (cc_process_again());
    ccStreamEnv.isProcessing = false;
}

static void cc_playback_buf_init(uint8_t instance_handle)
{
    CC_PLAYBACK_INFO_T *playbackInfo =
        &(ccStreamEnv.playbackInstance[instance_handle].playback_info);
    CC_PLAYBACK_CONTEXT_T *playbackContext =
        &(ccStreamEnv.playbackInstance[instance_handle].playback_context);
    CC_HEAP_ALLOC buf_alloc = ccStreamEnv.playbackInstance[instance_handle].core_playback_func_list->buf_alloc;

    uint32_t frame_buf_size = playbackInfo->frameSize * CC_PLAYBACK_MAX_FRAME_CNT;
    uint32_t pcm_buf_size = playbackInfo->pcmSize * CC_PLAYBACK_MAX_PCM_CNT;

    if (!ccStreamEnv.playbackInstance[instance_handle].buffer_initialized)
    {
        playbackContext->frame_fifo = ADDR_CPU_TO_DEV((cfifo*)buf_alloc(NULL, sizeof(cfifo)));
        playbackContext->pcm_fifo = ADDR_CPU_TO_DEV((cfifo*)buf_alloc(NULL, sizeof(cfifo)));

        playbackContext->frame_buf = ADDR_CPU_TO_DEV((uint8_t*)buf_alloc(NULL, frame_buf_size));
        playbackContext->pcm_buf = ADDR_CPU_TO_DEV((uint8_t*)buf_alloc(NULL, pcm_buf_size));

        playbackContext->frame_cache = ADDR_CPU_TO_DEV((uint8_t*)buf_alloc(NULL, playbackInfo->frameSize));
        playbackContext->pcm_cache = ADDR_CPU_TO_DEV((uint8_t*)buf_alloc(NULL, playbackInfo->pcmSize));

        ccStreamEnv.playbackInstance[instance_handle].buffer_initialized = true;
    }

    if (ccStreamEnv.playbackInstance[instance_handle].playback_algo_func_list) {
        ccStreamEnv.playbackInstance[instance_handle].playback_algo_func_list->algo_buf_init((void*)buf_alloc);
    }

    cfifo_init(playbackContext->frame_fifo, playbackContext->frame_buf, frame_buf_size);
    cfifo_init(playbackContext->pcm_fifo, playbackContext->pcm_buf, pcm_buf_size);
}

static uint8_t cc_playback_alloc_instance(uint32_t frameSize, uint32_t pcmSize)
{
    CC_PLAYBACK_INSTANCE *instance = NULL;
    for (uint8_t instance_handle = 0; instance_handle < PLAYBACK_INSTANCE_MAX; instance_handle++)
    {
        instance = &(ccStreamEnv.playbackInstance[instance_handle]);
        if (instance->status >= INSTANCE_INITIALIZED) {
            continue;
        }
        if (!instance->buffer_initialized ||
            (instance->buffer_initialized && instance->playback_info.pcmSize >= pcmSize &&
            instance->playback_info.frameSize >= frameSize))
        {
            return instance_handle;
        }
    }
    ASSERT(false, "%s frameSize:%d pcmSize:%d", __func__, frameSize, pcmSize);
}

void cc_playback_init(CORE_PLAYBACK_INIT_INFO_T *initInfo)
{
    CC_PLAYBACK_INIT_REQ_T *initReq = (CC_PLAYBACK_INIT_REQ_T*)(initInfo->initReq);

    uint32_t pcmSize = initReq->decoder_info.pcm_size + sizeof(CC_PLAYBACK_DATA_T);
    uint32_t frameSize = sizeof(CC_PLAYBACK_DATA_T) + \
        initReq->decoder_info.num_channels * initReq->decoder_info.frame_size;
    uint8_t instance_handle = cc_playback_alloc_instance(frameSize, pcmSize);
    CC_PLAYBACK_INSTANCE *instance = &(ccStreamEnv.playbackInstance[instance_handle]);
    CC_PLAYBACK_INFO_T *playbackInfo = &(instance->playback_info);

    instance->core = (CORE_TYPE_E)initInfo->core_type;
    instance->core_playback_func_list = initInfo->playback_func_list;
    playbackInfo->decoder_info = initReq->decoder_info;
    playbackInfo->pcmSize = pcmSize;
    playbackInfo->frameSize = frameSize;

    LOG_I("sample_rate:%d num_channels:%d frame_size:%d bits_depth:%d frame_ms:%d",
    playbackInfo->decoder_info.sample_rate, playbackInfo->decoder_info.num_channels,
    playbackInfo->decoder_info.frame_size, playbackInfo->decoder_info.bits_depth,
    (int)playbackInfo->decoder_info.frame_ms);
    LOG_I("pcmSize:%d", initReq->decoder_info.pcm_size);

    instance->core_playback_func_list->playback_init(instance_handle);

    if (initReq->algoProcess) {
        gaf_cc_playback_algo_func_register(&(instance->playback_algo_func_list));
    }

    cc_playback_buf_init(instance_handle);

    if (LC3 == initReq->codec_type || LC3PLUS == initReq->codec_type)
    {
        gaf_audio_lc3_update_decoder_func_list(&(instance->dec_func_list));
    }
    else if (LHDC == initReq->codec_type)
    {
#ifdef LEA_LHDC
        gaf_audio_lhdc_update_decoder_func_list(&(instance->dec_func_list));
#endif
    }
    else
    {
        ASSERT(false, "%s codec_type:%dcodec type err", __func__, initReq->codec_type);
    }

    uint32_t dec_freq = instance->dec_func_list->decoder_set_freq(
                    &(playbackInfo->decoder_info), APP_SYSFREQ_104M, initInfo->core_type);

    app_sysfreq_req(APP_SYSFREQ_USER_AOB_PLAYBACK, (enum APP_SYSFREQ_FREQ_T)dec_freq);

    instance->dec_func_list->decoder_init_buf_func(instance_handle, &(playbackInfo->decoder_info),
                (instance->core_playback_func_list->codec_buf_alloc),
                (instance->core_playback_func_list->codec_buf_free));
    instance->dec_func_list->decoder_init_func(instance_handle, &(playbackInfo->decoder_info));

    if (instance->playback_algo_func_list) {
        instance->playback_algo_func_list->algo_open(
                &initReq->rx_algo_cfg, &playbackInfo->decoder_info);
    }

    CC_PLAYBACK_INIT_RSP_T rsp;
    rsp.isBis = initReq->isBis;
    rsp.ptr = initReq->ptr;
    rsp.ptr2 = initReq->ptr2;
    rsp.frame_fifo = instance->playback_context.frame_fifo;
    rsp.pcm_fifo = instance->playback_context.pcm_fifo;
    rsp.isDecoding = ADDR_CPU_TO_DEV(&ccStreamEnv.isProcessing);
    rsp.instance_handle = instance_handle;

    instance->core_playback_func_list->playback_init_rsp(&rsp, sizeof(rsp));
    instance->status = INSTANCE_INITIALIZED;
}

static void cc_capture_buf_init(uint8_t instance_handle)
{
    CC_CAPTURE_INSTANCE *instance = &ccStreamEnv.captureInstance[instance_handle];
    CC_CAPTURE_CONTEXT_T *captureContext = &(instance->capture_context);
    CC_HEAP_ALLOC buf_alloc = instance->core_capture_func_list->buf_alloc;

    uint32_t frame_size = instance->capture_info.frameSize;
    uint32_t frame_buf_size = frame_size * CC_CAPTURE_MAX_FRAME_CNT;
    uint32_t pcm_size = instance->capture_info.pcmSize;
    uint32_t pcm_buf_size = pcm_size * CC_CAPTURE_MAX_PCM_CNT;

    if (!instance->buffer_initialized)
    {
        captureContext->pcm_fifo = ADDR_CPU_TO_DEV((cfifo*)buf_alloc(NULL, sizeof(cfifo)));
        captureContext->frame_fifo = ADDR_CPU_TO_DEV((cfifo*)buf_alloc(NULL, sizeof(cfifo)));

        captureContext->pcm_buf = ADDR_CPU_TO_DEV((uint8_t*)buf_alloc(NULL, pcm_buf_size));
        captureContext->frame_buf = ADDR_CPU_TO_DEV((uint8_t*)buf_alloc(NULL, frame_buf_size));

        captureContext->pcm_cache = ADDR_CPU_TO_DEV((uint8_t*)buf_alloc(NULL, pcm_size));
        captureContext->frame_cache = ADDR_CPU_TO_DEV((uint8_t*)buf_alloc(NULL, frame_size));

        instance->buffer_initialized = true;
    }

    instance->enc_func_list->encoder_init_buf_func(instance_handle, &(instance->capture_info.encoder_info),
                        (instance->core_capture_func_list->codec_buf_alloc),
                        (instance->core_capture_func_list->codec_buf_free));

    if (instance->capture_algo_func_list) {
        instance->capture_algo_func_list->algo_buf_init((void*)buf_alloc);
    }

    cfifo_init(captureContext->frame_fifo, captureContext->frame_buf, frame_buf_size);
    cfifo_init(captureContext->pcm_fifo, captureContext->pcm_buf, pcm_buf_size);
}

static uint8_t cc_capture_alloc_instance(uint32_t frameSize, uint32_t pcmSize)
{
    CC_CAPTURE_INSTANCE *instance = NULL;
    for (uint8_t instance_handle = 0; instance_handle < CAPTURE_INSTANCE_MAX; instance_handle++)
    {
        instance = &(ccStreamEnv.captureInstance[instance_handle]);
        if (instance->status >= INSTANCE_INITIALIZED){
            continue;
        }
        if (!instance->buffer_initialized ||
            (instance->buffer_initialized && instance->capture_info.pcmSize >= pcmSize &&
            instance->capture_info.frameSize >= frameSize))
        {
            return instance_handle;
        }
    }
    ASSERT(false, "%s frameSize:%d pcmSize:%d", __func__, frameSize, pcmSize);
}

void cc_capture_init(CORE_CAPTURE_INIT_INFO_T *initInfo)
{
    CC_CAPTURE_INIT_REQ_T *initReq = (CC_CAPTURE_INIT_REQ_T*)initInfo->initReq;

    uint32_t pcmSize = initReq->pcm_size + sizeof(CC_CAPTURE_DATA_T);
    uint32_t frameSize = sizeof(CC_CAPTURE_DATA_T) + \
        initReq->encoder_info.num_channels * initReq->encoder_info.frame_size;

    uint8_t instance_handle = cc_capture_alloc_instance(frameSize, pcmSize);
    CC_CAPTURE_INSTANCE *instance = &(ccStreamEnv.captureInstance[instance_handle]);
    CC_CAPTURE_INFO_T *captureInfo = &(instance->capture_info);

    instance->core = (CORE_TYPE_E)initInfo->core_type;
    instance->core_capture_func_list = initInfo->capture_func_list;
    captureInfo->encoder_info = initReq->encoder_info;
    captureInfo->pcmSize = pcmSize;
    captureInfo->frameSize = frameSize;

    LOG_I("sample_rate:%d num_channels:%d frame_size:%d bits_depth:%d",
    captureInfo->encoder_info.sample_rate, captureInfo->encoder_info.num_channels,
    captureInfo->encoder_info.frame_size, captureInfo->encoder_info.bits_depth);
    LOG_I("frame_ms:%d pcmSize:%d", (int)captureInfo->encoder_info.frame_ms, initReq->pcm_size);

    instance->core_capture_func_list->capture_init(instance_handle);

    if (initReq->algoProcess) {
        gaf_cc_capture_algo_func_register(&(instance->capture_algo_func_list));
    }

    if (LC3 == initReq->codec_type || LC3PLUS == initReq->codec_type)
    {
        gaf_audio_lc3_update_encoder_func_list(&(instance->enc_func_list));
    }
    else if (LHDC == initReq->codec_type)
    {
#ifdef LEA_LHDC
        gaf_audio_lhdc_update_encoder_func_list(&(instance->enc_func_list));
#endif
    }
    else
    {
        ASSERT(false, "%s codec type err", __func__);
    }

    cc_capture_buf_init(instance_handle);

    uint32_t enc_freq = instance->enc_func_list->encoder_set_freq(
                    &(captureInfo->encoder_info), APP_SYSFREQ_52M, initInfo->core_type);
    app_sysfreq_req(APP_SYSFREQ_USER_AOB_CAPTURE, (enum APP_SYSFREQ_FREQ_T)enc_freq);

    instance->enc_func_list->encoder_init_func(instance_handle, &(captureInfo->encoder_info));

    if (instance->capture_algo_func_list) {
        instance->capture_algo_func_list->algo_open(
                &initReq->tx_algo_cfg, &captureInfo->encoder_info);
    }

    CC_CAPTURE_INIT_RSP_T rsp;
    rsp.pcm_fifo = instance->capture_context.pcm_fifo;
    rsp.frame_fifo = instance->capture_context.frame_fifo;
    rsp.isEncoding = ADDR_CPU_TO_DEV(&ccStreamEnv.isProcessing);
    rsp.instance_handle = instance_handle;
    rsp.isBis = initReq->isBis;
    rsp.ptr = initReq->ptr;
    rsp.ptr2 = initReq->ptr2;

    instance->core_capture_func_list->capture_init_rsp(&rsp, sizeof(rsp));
    instance->status = INSTANCE_INITIALIZED;
}

void cc_playback_deinit(void *_deinitReq)
{
    CC_PLAYBACK_DEINIT_REQ_T *deinitReq = (CC_PLAYBACK_DEINIT_REQ_T*)_deinitReq;
    CC_PLAYBACK_DEINIT_RSP_T deinitRsp;
    deinitRsp.isBis = deinitReq->isBis;
    LOG_I("%s instance_handle:%d %d", __func__, deinitReq->instance_handle, deinitReq->isRetrigger);

    CC_PLAYBACK_INSTANCE *instance = &(ccStreamEnv.playbackInstance[deinitReq->instance_handle]);
    if (INSTANCE_IDLE != instance->status)
    {
        if (instance->playback_algo_func_list) {
            instance->playback_algo_func_list->algo_close();
        }
        if (instance->dec_func_list) {
            instance->dec_func_list->decoder_deinit_func(deinitReq->instance_handle);
            instance->dec_func_list->decoder_deinit_buf_func(deinitReq->instance_handle);
        }
        instance->status = INSTANCE_IDLE;
    }

    deinitRsp.closeDecoderCore =
        instance->core_playback_func_list->playback_deinit(deinitReq->instance_handle);
    deinitRsp.ptr = deinitReq->ptr;
    deinitRsp.instance_handle = deinitReq->instance_handle;
    if (instance->core_playback_func_list->playback_deinit_rsp)
    {
        instance->core_playback_func_list->playback_deinit_rsp((uint8_t*)&deinitRsp, sizeof(deinitRsp));
    }
}

void cc_capture_deinit(void *_deinitReq)
{
    CC_CAPTURE_DEINIT_REQ_T *deinitReq = (CC_CAPTURE_DEINIT_REQ_T*)_deinitReq;
    CC_CAPTURE_DEINIT_RSP_T deinitRsp;
    deinitRsp.isBis = deinitReq->isBis;
    LOG_I("%s instance_handle:%d %d", __func__, deinitReq->instance_handle, deinitReq->isRetrigger);

    CC_CAPTURE_INSTANCE *instance = &(ccStreamEnv.captureInstance[deinitReq->instance_handle]);
    if (INSTANCE_IDLE != instance->status)
    {
        if (instance->capture_algo_func_list) {
            instance->capture_algo_func_list->algo_close();
        }
        if (instance->enc_func_list) {
            instance->enc_func_list->encoder_deinit_func(deinitReq->instance_handle);
            instance->enc_func_list->encoder_deinit_buf_func(deinitReq->instance_handle);
        }
        instance->status = INSTANCE_IDLE;
    }

    deinitRsp.closeEncoderCore =
        instance->core_capture_func_list->capture_deinit(deinitReq->instance_handle);
    deinitRsp.ptr = deinitReq->ptr;
    deinitRsp.instance_handle = deinitReq->instance_handle;
    if (instance->core_capture_func_list->capture_deinit_rsp)
    {
        instance->core_capture_func_list->capture_deinit_rsp((uint8_t*)&deinitRsp, sizeof(deinitRsp));
    }
}

void cc_playback_buf_deinit(CORE_TYPE_E core_type)
{
    for (uint32_t index = 0; index < PLAYBACK_INSTANCE_MAX; index++) {
        if (core_type == ccStreamEnv.playbackInstance[index].core) {
            ccStreamEnv.playbackInstance[index].buffer_initialized = false;
            if (ccStreamEnv.playbackInstance[index].playback_algo_func_list) {
                ccStreamEnv.playbackInstance[index].playback_algo_func_list->algo_buf_deinit();
            }
        }
    }
    LOG_I("%s", __func__);
}

void cc_capture_buf_deinit(CORE_TYPE_E core_type)
{
    for (uint32_t index = 0; index < CAPTURE_INSTANCE_MAX; index++) {
        if (core_type == ccStreamEnv.captureInstance[index].core) {
            ccStreamEnv.captureInstance[index].buffer_initialized = false;
            if (ccStreamEnv.captureInstance[index].capture_algo_func_list) {
                ccStreamEnv.captureInstance[index].capture_algo_func_list->algo_buf_deinit();
            }
        }
    }
    LOG_I("%s", __func__);
}

static void cc_playback_init_signal_handler(void)
{
    cc_stream_boost_freq();

    CC_PLAYBACK_INIT_REQ_T initReq;
    CORE_PLAYBACK_INIT_INFO_T initInfo;
    cfifo *fifo = &(ccStreamEnv.playback_init_req_fifo);
    while (cfifo_len(fifo) >= sizeof(initReq) + sizeof(initInfo))
    {
        cfifo_pop(fifo, (uint8_t*)&initReq, sizeof(initReq));
        cfifo_pop(fifo, (uint8_t*)&initInfo, sizeof(initInfo));
        initInfo.initReq = (uint8_t*)&initReq;
        cc_playback_init(&initInfo);
    }
}

static void cc_playback_deinit_signal_handler(void)
{
    CC_PLAYBACK_DEINIT_REQ_T deinitReq;
    cfifo *fifo = &(ccStreamEnv.playback_deinit_req_fifo);
    while (cfifo_len(fifo) >= sizeof(deinitReq))
    {
        cfifo_pop(fifo, (uint8_t*)&deinitReq, sizeof(deinitReq));
        cc_playback_deinit(&deinitReq);
    }
}

static void cc_capture_init_signal_handler(void)
{
    cc_stream_boost_freq();

    CC_CAPTURE_INIT_REQ_T initReq;
    CORE_CAPTURE_INIT_INFO_T initInfo;
    cfifo *fifo = &(ccStreamEnv.capture_init_req_fifo);
    while (cfifo_len(fifo) >= sizeof(initReq) + sizeof(initInfo))
    {
        cfifo_pop(fifo, (uint8_t*)&initReq, sizeof(initReq));
        cfifo_pop(fifo, (uint8_t*)&initInfo, sizeof(initInfo));
        initInfo.initReq = (uint8_t*)&initReq;
        cc_capture_init(&initInfo);
    }
}

static void cc_capture_deinit_signal_handler(void)
{
    CC_CAPTURE_DEINIT_REQ_T deinitReq;
    cfifo *fifo = &(ccStreamEnv.capture_deinit_req_fifo);
    while (cfifo_len(fifo) >= sizeof(deinitReq))
    {
        cfifo_pop(fifo, (uint8_t*)&deinitReq, sizeof(deinitReq));
        cc_capture_deinit(&deinitReq);
    }
}

void cc_playback_init_req_handler(CORE_PLAYBACK_INIT_INFO_T *initInfo)
{
    cfifo *fifo = &(ccStreamEnv.playback_init_req_fifo);
    uint32_t len = sizeof(CC_PLAYBACK_INIT_REQ_T) + sizeof(CORE_PLAYBACK_INIT_INFO_T);
    ASSERT(cfifo_free_space(fifo) >= len,
        "playback init fifo free space:%d len:%d", cfifo_free_space(fifo), len);
    cfifo_put(fifo, initInfo->initReq, sizeof(CC_PLAYBACK_INIT_REQ_T));
    cfifo_put(fifo, (uint8_t*)initInfo, sizeof(CORE_PLAYBACK_INIT_INFO_T));
    if (!ccStreamEnv.pStreamEnv)
    {
        CC_CAPTURE_INIT_REQ_T* initReq = (CC_CAPTURE_INIT_REQ_T*)initInfo->initReq;
        ccStreamEnv.pStreamEnv = (GAF_AUDIO_STREAM_ENV_T *)initReq->ptr2;
    }
    osSignalSet(cc_stream_processor_thread_id, CC_PLAYBACK_SIGNAL_START);
}

void cc_playback_deinit_req_handler(uint8_t *deinitReq)
{
    cfifo *fifo = &(ccStreamEnv.playback_deinit_req_fifo);
    ASSERT(cfifo_free_space(fifo) >= sizeof(CC_PLAYBACK_DEINIT_REQ_T),
        "playback deinit fifo free space:%d len:%d", cfifo_free_space(fifo), sizeof(CC_PLAYBACK_DEINIT_REQ_T));
    cfifo_put(fifo, deinitReq, sizeof(CC_PLAYBACK_DEINIT_REQ_T));
    osSignalSet(cc_stream_processor_thread_id, CC_PLAYBACK_SIGNAL_STOP);
}

void cc_capture_init_req_handler(CORE_CAPTURE_INIT_INFO_T *initInfo)
{
    cfifo *fifo = &(ccStreamEnv.capture_init_req_fifo);
    uint32_t len = sizeof(CC_CAPTURE_INIT_REQ_T) + sizeof(CORE_CAPTURE_INIT_INFO_T);
    ASSERT(cfifo_free_space(fifo) >= len,
        "capture init fifo free space:%d len:%d", cfifo_free_space(fifo), len);
    cfifo_put(fifo, initInfo->initReq, sizeof(CC_CAPTURE_INIT_REQ_T));
    cfifo_put(fifo, (uint8_t*)initInfo, sizeof(CORE_CAPTURE_INIT_INFO_T));
    if (!ccStreamEnv.pStreamEnv)
    {
        CC_CAPTURE_INIT_REQ_T* initReq = (CC_CAPTURE_INIT_REQ_T*)initInfo->initReq;
        ccStreamEnv.pStreamEnv = (GAF_AUDIO_STREAM_ENV_T *)initReq->ptr2;
    }
    osSignalSet(cc_stream_processor_thread_id, CC_CAPTURE_SIGNAL_START);
}

void cc_capture_deinit_req_handler(uint8_t *deinitReq)
{
    cfifo *fifo = &(ccStreamEnv.capture_deinit_req_fifo);
    ASSERT(cfifo_free_space(fifo) >= sizeof(CC_CAPTURE_DEINIT_REQ_T),
        "capture deinit fifo free space:%d len:%d", cfifo_free_space(fifo), sizeof(CC_CAPTURE_DEINIT_REQ_T));
    cfifo_put(fifo, deinitReq, sizeof(CC_CAPTURE_DEINIT_REQ_T));
    osSignalSet(cc_stream_processor_thread_id, CC_CAPTURE_SIGNAL_STOP);
}

void cc_stream_data_notify_handler(void)
{
    osSignalSet(cc_stream_processor_thread_id, CC_STREAM_SIGNAL_DATA_RECEIVED);
}

static void cc_stream_processor_thread(const void *arg)
{
    while(1)
    {
        osEvent evt;
        evt = osSignalWait(0x0, osWaitForever);

        if (osEventSignal == evt.status)
        {
            if (evt.value.signals & CC_PLAYBACK_SIGNAL_START)
            {
                cc_playback_init_signal_handler();
            }
            if (evt.value.signals & CC_PLAYBACK_SIGNAL_STOP)
            {
                cc_playback_deinit_signal_handler();
            }

            if (evt.value.signals & CC_CAPTURE_SIGNAL_START)
            {
                cc_capture_init_signal_handler();
            }
            if (evt.value.signals & CC_CAPTURE_SIGNAL_STOP)
            {
                cc_capture_deinit_signal_handler();
            }

            if (evt.value.signals & CC_STREAM_SIGNAL_DATA_RECEIVED)
            {
                cc_stream_process();
            }
        }
    }
}

void cc_stream_init(uint32_t core_type)
{
    uint32_t index = 0;
    ccStreamEnv.isProcessing = false;
    ccStreamEnv.pStreamEnv = NULL;
    for (index = 0; index < PLAYBACK_INSTANCE_MAX; index++)
    {
        ccStreamEnv.playbackInstance[index].buffer_initialized = false;
        ccStreamEnv.playbackInstance[index].status = INSTANCE_IDLE;
    }
    for (index = 0; index < CAPTURE_INSTANCE_MAX; index++)
    {
        ccStreamEnv.captureInstance[index].buffer_initialized = false;
        ccStreamEnv.captureInstance[index].status = INSTANCE_IDLE;
    }
    if (NULL == cc_stream_processor_thread_id && core_type != CP_CORE)
    {
        cc_stream_processor_thread_id =
            osThreadCreate(osThread(cc_stream_processor_thread), NULL);
    }
    if (core_type != CP_CORE)
    {
        cfifo_init(&(ccStreamEnv.playback_init_req_fifo), ccStreamEnv.playback_init_req_buf, PLAYBACK_INIT_REQ_BUF_SIZE);
        cfifo_init(&(ccStreamEnv.capture_init_req_fifo), ccStreamEnv.capture_init_req_buf, CAPTURE_INIT_REQ_BUF_SIZE);
        cfifo_init(&(ccStreamEnv.playback_deinit_req_fifo), ccStreamEnv.playback_deinit_req_buf, PLAYBACK_DEINIT_REQ_BUF_SIZE);
        cfifo_init(&(ccStreamEnv.capture_deinit_req_fifo), ccStreamEnv.capture_deinit_req_buf, CAPTURE_DEINIT_REQ_BUF_SIZE);
    }
}
