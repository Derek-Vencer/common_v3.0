/***************************************************************************
 *
 * Copyright 2015-2025 BES.
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
#ifdef GAF_OFFLOAD_ENABLE
#include "hal_dma.h"
#include "hal_iomux.h"
#include "usb_audio_app.h"
#include "string.h"
#include "hal_trace.h"
#include "gaf_offload_dsp_stream.h"
#include "gaf_offload_lc3.h"
#include "lc3_process.h"
#include "bt_common_define.h"
#include "cp_accel.h"
#include "hal_codec.h"
#include "codec_int.h"
#include "app_utils.h"
#include "gaf_cp_stream.h"
#include "gaf_media_sync.h"
#include "heap_api.h"
#include "app_audio.h"
#include "gaf_cp_stream.h"
#include "gaf_audio_path.h"
#include "audio_process_vol.h"


#ifdef  LEA_OFFLOAD_96K
#define CODEC_IN_FRAME_SIZE (96 * 4 * 10)
#define CODEC_OUT_FRAME_SIZE (96 * 4 * 10)
#define CODEC_OUT_FRAME_TEST (96 * 4 * 10)
#else
#define CODEC_IN_FRAME_SIZE (48 * 4 * 10)
#define CODEC_OUT_FRAME_SIZE (48 * 4 * 10)
#define MAX_USB_JITTER_BUF (48 * 4 * 2 * 7)
#define CODEC_OUT_FRAME_TEST (48 * 4 * 10)
#define MAX_ENCODED_SIZE (240 * 2)
#endif

#ifdef WIRELESS_MIC_RX
#undef CODEC_IN_FRAME_SIZE
#undef CODEC_OUT_FRAME_SIZE
#define CODEC_IN_FRAME_SIZE  (48 * 4 * 2 * BLE_AUDIO_FRAME_US / 1000)
#define CODEC_OUT_FRAME_SIZE (48 * 4 * 2 * BLE_AUDIO_FRAME_US / 1000)

#ifdef AUD_OUTPUT_BY_USB
extern bool audio_data_usb_capture_stream_running(void);
extern void audio_data_usb_capture_stream_data_input(uint8_t* buff, uint32_t len);
static int32_t audio_data_usb_capture_stream_buffer[48 * 2 * BLE_AUDIO_FRAME_US / 1000] = {0};
#endif
#endif
static uint8_t codec_in_buf[2][CODEC_IN_FRAME_SIZE];
static uint8_t codec_out_buf[2][CODEC_OUT_FRAME_SIZE];
static struct HAL_DMA_DESC_T codec_in_dma_desc[2];
static struct HAL_DMA_DESC_T codec_out_dma_desc[2];

#define LC3_APP_HEAP_SIZE 0x20000
uint8_t* lc3_registered_heap = NULL;
static void* codec_out_buf_lli;
static uint8_t* rx_pcm_cache_buf = NULL;
static uint8_t* codec_out_buf_test = NULL;
static uint8_t* encode_cache_buf = NULL;
#define MAX_LEA_OCTETS_PER_FRAME  80
#define HCI_ISO_SDU_MAX_HEADER_ULL_LEN  8 // include Time_Stamp
#define OFFLOAD_FRAME_INTERVAL 5

#ifdef BLE_USB_AUDIO_IS_DONGLE_ROLE
extern void bt_svc_lea_uc_cli_stream_data_tx_read(uint8_t* data, uint32_t len);
extern void bt_svc_lea_uc_cli_stream_data_rx_write(uint8_t* data, uint32_t len);
extern void gaf_offload_stream_data_tx_read(uint8_t* data, uint32_t len);
extern void gaf_offload_stream_data_tx_write(uint8_t* data, uint32_t len);
static void mobile_codec_in_capture_handler(uint8_t chan, uint32_t remains, uint32_t error, struct HAL_DMA_DESC_T *lli);
#endif
static void codec_out_playback_handler(uint8_t chan, uint32_t remains, uint32_t error, struct HAL_DMA_DESC_T *lli);
#if defined(BLE_USB_AUDIO_IS_HEADSET_ROLE) && !defined(GAF_OFFLOAD_MIC)
static void codec_in_capture_handler(uint8_t chan, uint32_t remains, uint32_t error, struct HAL_DMA_DESC_T *lli);
#endif
#if defined(BLE_USB_AUDIO_IS_HEADSET_ROLE) && defined(GAF_OFFLOAD_MIC)
static void wireless_mic_codec_in_capture_handler(uint8_t chan, uint32_t remains, uint32_t error, struct HAL_DMA_DESC_T *lli);
#endif
void gaf_offload_decodec_init(CODEC_INFO_T* codec_cfg);
void gaf_offload_encodec_init(CODEC_INFO_T* codec_cfg);
static void gaf_offload_capture_vol_changes(const float gain);
static void get_lea_offload_release_dma_ch(uint8_t adma_ch);

struct lea_offload_config_t lea_offload_cfg = {
    .gaf_offload_enable = false,
    .lc3_ch_num = AUD_CHANNEL_NUM_2,
#ifdef BLE_USB_AUDIO_IS_DONGLE_ROLE
    .codec_in_handler = mobile_codec_in_capture_handler,
    .codec_out_handler = codec_out_playback_handler,
#endif
#if defined(BLE_USB_AUDIO_IS_HEADSET_ROLE) && !defined(GAF_OFFLOAD_MIC)
    .codec_in_handler = codec_in_capture_handler,
    .codec_out_handler = codec_out_playback_handler,
#endif
#if defined(BLE_USB_AUDIO_IS_HEADSET_ROLE) && defined(GAF_OFFLOAD_MIC)
    .codec_in_handler = wireless_mic_codec_in_capture_handler,
    .codec_out_handler = codec_out_playback_handler,
#endif
    .decoder_func_list = gaf_offload_lc3_decoder_funcs(),
    .encoder_func_list = gaf_offload_lc3_encoder_funcs(),
    .offload_capture_ch = 0xff,
    .offload_playback_ch = 0xff,
    .lc3_codec_complete = 0,
};

static void gaf_offload_buf_init(void)
{
    if (!lc3_registered_heap)
    {
#if defined(BLE_USB_AUDIO_IS_HEADSET_ROLE)||defined(GAF_OFFLOAD_MIC)
        app_audio_mempool_init();
        // lc3_registered_heap = (uint8_t*)gaf_cp_buffer_alloc(NULL, LC3_APP_HEAP_SIZE);
        app_audio_mempool_get_buff(&lc3_registered_heap, LC3_APP_HEAP_SIZE);
#else
    // in dongle role bt_svc_base_buf_syspool_init has inited
    //when cp ram is not enough, use app_audio_mempool_get_buff as workaround
        app_audio_mempool_get_buff(&lc3_registered_heap, LC3_APP_HEAP_SIZE);
#endif
        bes_heap_init_lc3(lc3_registered_heap, LC3_APP_HEAP_SIZE);
    }

    if(!rx_pcm_cache_buf)
    {
        app_audio_mempool_get_buff(&rx_pcm_cache_buf, CODEC_IN_FRAME_SIZE);
    }

    if (!codec_out_buf_test)
    {
        app_audio_mempool_get_buff(&codec_out_buf_test, MAX_USB_JITTER_BUF);
    }

    if (!encode_cache_buf)
    {
        app_audio_mempool_get_buff(&encode_cache_buf, MAX_ENCODED_SIZE);
    }
    lea_offload_cfg.gaf_offload_enable = true;

}


void gaf_offload_capture_codec_open(CODEC_INFO_T *codec_info)
{
    struct HAL_CODEC_CONFIG_T codec_cfg;

    TRACE(2,"%s", __func__);

    memset(&codec_cfg, 0, sizeof(codec_cfg));
    codec_cfg.bits = (enum AUD_BITS_T)codec_info->bits_depth;
    codec_cfg.sample_rate = (enum AUD_SAMPRATE_T)codec_info->sample_rate;
    codec_cfg.channel_num = (enum AUD_CHANNEL_NUM_T)codec_info->num_channels;
    if (codec_info->num_channels == AUD_CHANNEL_NUM_2)
    {
        codec_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1);
    }
    else
    {
        codec_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0);
    }
    codec_cfg.use_dma = true;
    codec_cfg.vol = 1;
    codec_cfg.io_path = AUD_INPUT_PATH_MAINMIC;
    codec_cfg.set_flag = HAL_CODEC_CONFIG_ALL;

    codec_int_open();
    codec_int_stream_open(AUD_STREAM_CAPTURE);
    codec_int_stream_setup(AUD_STREAM_CAPTURE, &codec_cfg);

    // hal_cmu_codec_dma_ack_select(HAL_CMU_CODEC_DMA_ACK_ALL, true);  //CODEC DMA USE SYS
}

void gaf_offload_playback_codec_open(CODEC_INFO_T *codec_info, uint8_t vol)
{
    struct HAL_CODEC_CONFIG_T codec_cfg;

    TRACE(2,"%s", __func__);
    memset(&codec_cfg, 0, sizeof(codec_cfg));
    codec_cfg.bits = (enum AUD_BITS_T)codec_info->bits_depth;
    codec_cfg.sample_rate = (enum AUD_SAMPRATE_T)codec_info->sample_rate;
    codec_cfg.channel_num = (enum AUD_CHANNEL_NUM_T)codec_info->num_channels;
    if (codec_info->num_channels == AUD_CHANNEL_NUM_2)
    {
        codec_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1);
    }
    else
    {
        codec_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0);
    }

    codec_cfg.use_dma = true;
    codec_cfg.vol = vol;
    codec_cfg.io_path = AUD_OUTPUT_PATH_SPEAKER;
    codec_cfg.set_flag = HAL_CODEC_CONFIG_ALL;

    codec_int_open();
    codec_int_stream_open(AUD_STREAM_PLAYBACK);
    codec_int_stream_setup(AUD_STREAM_PLAYBACK, &codec_cfg);

    // hal_cmu_codec_dma_ack_select(HAL_CMU_CODEC_DMA_ACK_ALL, true);  //CODEC DMA USE SYS
}


void gaf_offload_lea_offload_cfg(const void *data, unsigned int len)
{
    gaf_offload_message gaf_offload_codec_cfg= *(gaf_offload_message*)data;
    CODEC_INFO_T* codec_cfg = NULL;
    if (gaf_offload_codec_cfg.magic_num == 0x5e5a)
    {
        lea_offload_cfg.pStreamEnv_ptr = (GAF_AUDIO_STREAM_ENV_T*)gaf_offload_codec_cfg.pStreamEnv_ptr;
        TRACE(0, "%s : startedStreamTypes %d", __func__,gaf_offload_codec_cfg.startedStreamTypes);
        switch (gaf_offload_codec_cfg.startedStreamTypes)
        {
            case GAF_OFFLOAD_MOBILE_STREAM_PLAYBACK_START:
                gaf_offload_buf_init();
                for (uint8_t i = 0; i < BLE_CONNECTION_MAX; i++)
                {
                    lea_offload_cfg.iso_handler[i] = 0;
                }
                codec_cfg = (CODEC_INFO_T*)gaf_offload_codec_cfg.codec_cfg;
                lea_offload_cfg.decodec_info.sample_rate = (enum AUD_SAMPRATE_T)codec_cfg->sample_rate;
                lea_offload_cfg.decodec_info.num_channels = codec_cfg->num_channels;
                lea_offload_cfg.decodec_info.bits_depth = (enum AUD_BITS_T)codec_cfg->bits_depth;
                lea_offload_cfg.decodec_info.frame_ms = codec_cfg->frame_ms;
                lea_offload_cfg.decodec_info.frame_size = codec_cfg->frame_size;
#ifdef GAF_OFFLOAD_MIC
                codec_cfg->pcm_size = codec_cfg->pcm_size * 2;
#endif
                lea_offload_cfg.decodec_info.pcm_size = codec_cfg->pcm_size;
                gaf_offload_playback_codec_open(codec_cfg, 15);
                gaf_offload_decodec_init(codec_cfg);
                break;
            case GAF_OFFLOAD_MOBILE_STREAM_CAPTURE_START:
                gaf_offload_buf_init();
                codec_cfg = (CODEC_INFO_T*)gaf_offload_codec_cfg.codec_cfg;
                lea_offload_cfg.encodec_info.sample_rate = (enum AUD_SAMPRATE_T)codec_cfg->sample_rate;
                lea_offload_cfg.encodec_info.num_channels = codec_cfg->num_channels;
                lea_offload_cfg.encodec_info.bits_depth = (enum AUD_BITS_T)codec_cfg->bits_depth;
                lea_offload_cfg.encodec_info.frame_ms = codec_cfg->frame_ms;
                lea_offload_cfg.encodec_info.frame_size = codec_cfg->frame_size;
                lea_offload_cfg.encodec_info.pcm_size = codec_cfg->pcm_size;
                gaf_offload_capture_codec_open(codec_cfg);
                gaf_offload_encodec_init(codec_cfg);
                break;
            case GAF_OFFLOAD_STREAM_PLAYBACK_START:
                gaf_offload_buf_init();
                for (uint8_t i = 0; i < BLE_CONNECTION_MAX; i++)
                {
                    lea_offload_cfg.iso_handler[i] = 0;
                }
                codec_cfg = (CODEC_INFO_T*)gaf_offload_codec_cfg.codec_cfg;
                lea_offload_cfg.decodec_info.sample_rate = (enum AUD_SAMPRATE_T)codec_cfg->sample_rate;
                lea_offload_cfg.decodec_info.num_channels = codec_cfg->num_channels;
                lea_offload_cfg.decodec_info.bits_depth = (enum AUD_BITS_T)codec_cfg->bits_depth;
                lea_offload_cfg.decodec_info.frame_ms = codec_cfg->frame_ms;
                lea_offload_cfg.decodec_info.frame_size = codec_cfg->frame_size;
                lea_offload_cfg.decodec_info.pcm_size = codec_cfg->pcm_size;
                gaf_offload_playback_codec_open(codec_cfg, 15);
                gaf_offload_decodec_init(codec_cfg);
                break;
            case GAF_OFFLOAD_STREAM_CAPTURE_START:
                gaf_offload_buf_init();
                codec_cfg = (CODEC_INFO_T*)gaf_offload_codec_cfg.codec_cfg;
                lea_offload_cfg.encodec_info.sample_rate = (enum AUD_SAMPRATE_T)codec_cfg->sample_rate;
                lea_offload_cfg.encodec_info.num_channels = codec_cfg->num_channels;
                lea_offload_cfg.encodec_info.bits_depth = (enum AUD_BITS_T)codec_cfg->bits_depth;
                lea_offload_cfg.encodec_info.frame_ms = codec_cfg->frame_ms;
                lea_offload_cfg.encodec_info.frame_size = codec_cfg->frame_size;
                lea_offload_cfg.encodec_info.pcm_size = codec_cfg->pcm_size;
                gaf_offload_capture_codec_open(codec_cfg);
                gaf_offload_encodec_init(codec_cfg);
                audio_process_vol_init(codec_cfg->sample_rate, codec_cfg->bits_depth, codec_cfg->num_channels, 1.0);
                break;
            case GAF_OFFLOAD_MOBILE_STREAM_PLAYBACK_STOP:
                codec_int_stream_stop(AUD_STREAM_PLAYBACK);
                get_lea_offload_release_dma_ch(lea_offload_cfg.offload_playback_ch);
                break;
            case GAF_OFFLOAD_MOBILE_STREAM_CAPTURE_STOP:
                codec_int_stream_stop(AUD_STREAM_CAPTURE);
                get_lea_offload_release_dma_ch(lea_offload_cfg.offload_capture_ch);
                break;
            case GAF_OFFLOAD_STREAM_PLAYBACK_STOP:
                codec_int_stream_stop(AUD_STREAM_PLAYBACK);
                get_lea_offload_release_dma_ch(lea_offload_cfg.offload_playback_ch);
                break;
            case GAF_OFFLOAD_STREAM_CAPTURE_STOP:
                codec_int_stream_stop(AUD_STREAM_CAPTURE);
                get_lea_offload_release_dma_ch(lea_offload_cfg.offload_capture_ch);
                break;
            case GAF_OFFLOAD_BUF_DEINITED:
                lc3_registered_heap = NULL;
                codec_out_buf_lli = NULL;
                rx_pcm_cache_buf = NULL;
                codec_out_buf_test = NULL;
                encode_cache_buf = NULL;
                lea_offload_cfg.lc3_codec_complete = 0;
                lea_offload_cfg.gaf_offload_enable = false;
                for (uint8_t i = 0; i < BLE_CONNECTION_MAX; i++)
                {
                    lea_offload_cfg.iso_handler[i] = 0;
                }
                break;
            case GAF_OFFLOAD_PLAYBACK_VOL_CHANGR:
                    if (lea_offload_cfg.gaf_offload_enable)
                    {
                        codec_cfg = (CODEC_INFO_T*)&lea_offload_cfg.pStreamEnv_ptr->stream_info.playbackInfo.codec_info;
                        gaf_offload_playback_codec_open(codec_cfg, gaf_offload_codec_cfg.playback_vol_gain);
                    }
                break;
            case GAF_OFFLOAD_CAPTURE_VOL_CHANGR:
                    if (lea_offload_cfg.gaf_offload_enable)
                    {
                        gaf_offload_capture_vol_changes(gaf_offload_codec_cfg.cap_vol_gain);
                    }
                break;
            default:
                break;
        }
    }
}

static void get_lea_offload_release_dma_ch(uint8_t adma_ch)
{
    TRACE(0, "get_lea_offload_release_dma_ch %d", adma_ch);
    hal_audma_stop(adma_ch);
    hal_dma_tc_irq_disable(adma_ch);
    hal_audma_free_chan(adma_ch);
}

uint8_t* get_lea_offload_cfg_param(void)
{
    return (uint8_t* )&lea_offload_cfg;
}

static void * ull_audio_get_codec_out_buf_lli(void)
{
    return (void *)codec_out_buf_lli;
}

static uint8_t cis_ch = 0;
static uint8_t get_cis_ch()
{
    return cis_ch;
}

static void cis_ch_increase()
{
    cis_ch += 1;
}

static void cis_ch_reset()
{
    cis_ch = 0;
}

static void * get_codec_ptr(struct Codec_intr* ptr, struct HAL_DMA_DESC_T *lli)
{
    int idx = (lli == ptr->dma_desc) ? 0 : 1;
    return (void *)&codec_in_buf[idx][0];
}

static uint8_t iso_packet_buffer[2][13+MAX_LEA_OCTETS_PER_FRAME*2];
static uint16_t iso_sdu_sequence_num = 0;
#define BLE_ASE_HDL_TO_INDEX(aes_handle)  (aes_handle-0x100-2)

POSSIBLY_UNUSED static void hci_tx_btc_audio_test(uint16_t ase_handle,const uint8_t *buf, uint16_t size)
{
    uint8_t buf_idx = BLE_ASE_HDL_TO_INDEX(ase_handle);
    iso_sdu_sequence_num +=1 ;
    uint32_t iso_sdu_timestamp = 0;
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
#ifdef ULL_AUDIO_GPIO_TEST
    hal_iomux_tportclr(0);
#endif
}

static uint8_t check_iso_data(struct Iso_intr *iso_rx_ins)
{
    if (iso_rx_ins->dma_buf == NULL)
    {
        TRACE(0, "btc2dsp_rx dst is null");
        return 1;
    }

    if ((lea_offload_cfg.lc3_codec_complete & GAF_OFFLOAD_DECODER_INIT_COMPLETED) == 0)
    {
        TRACE(0, "lc3_codec_complete is false");
        return 2;
    }

    if (iso_rx_ins->payload_len == 0)
    {
        TRACE(0, "SDU length zero");
        iso_rx_ins->isPLC = true;
    }
    else if (iso_rx_ins->payload_len == 0x8000)
    {
        TRACE(0, "SDU length anomaly");
        iso_rx_ins->isPLC = true;
#ifdef ULL_AUDIO_GPIO_TEST
        hal_iomux_tportset(0);
        hal_iomux_tportclr(0);
        hal_iomux_tportclr(1);
#endif
    }

    if (get_cis_ch() >= BLE_CONNECTION_MAX)
    {
        TRACE(0, "cis_ch %d is anomaly", get_cis_ch());
        return 1;
    }
    return 0;
}

POSSIBLY_UNUSED static void gaf_fill_dma_buf_16bit_1v2mix(int16_t *dma_buf, int16_t *pcm_buf, uint8_t cis_ch_id)
{
    for (unsigned int i = 0, j = 0; i < CODEC_OUT_FRAME_TEST/(lea_offload_cfg.decodec_info.bits_depth/8)/lea_offload_cfg.lc3_ch_num; i++)
    {
        for (int k = 0; k < lea_offload_cfg.lc3_ch_num; k++)
        {
            dma_buf[j] = (dma_buf[j] / (cis_ch_id+1)) + (pcm_buf[i] / (cis_ch_id+1));
            j++;
        }
    }
}

POSSIBLY_UNUSED static void gaf_fill_dma_buf_32bit_1v2mix(int32_t *dma_buf, int32_t *pcm_buf, uint8_t cis_ch_id)
{
    int byte_width = lea_offload_cfg.decodec_info.bits_depth == 24 ? 4 : lea_offload_cfg.decodec_info.bits_depth >> 3;
#ifdef GAF_OFFLOAD_MIC
    for (unsigned int i = 0, j = 0; i < lea_offload_cfg.decodec_info.pcm_size/byte_width/2; i++)
#else
    for (unsigned int i = 0, j = 0; i < lea_offload_cfg.decodec_info.pcm_size/byte_width; i++)
#endif
    {
            dma_buf[j] = (dma_buf[j] / (cis_ch_id+1)) + (pcm_buf[i] / (cis_ch_id+1));
            j++;
            dma_buf[j] = (dma_buf[j] / (cis_ch_id+1)) + (pcm_buf[i] / (cis_ch_id+1));
            j++;
    }
}

POSSIBLY_UNUSED static void gaf_fill_dma_buf_16bit(int16_t *dma_buf, int16_t *pcm_buf, uint8_t cis_ch_id)
{
    for (unsigned int i = 0, j = 0; i < lea_offload_cfg.decodec_info.pcm_size/(lea_offload_cfg.decodec_info.bits_depth/8); i++)
    {
        dma_buf[j] = (dma_buf[j] / (cis_ch_id+1)) + (pcm_buf[i] / (cis_ch_id+1));
        j++;
    }
}

POSSIBLY_UNUSED static void gaf_fill_dma_buf_32bit(int32_t *dma_buf, int32_t *pcm_buf, uint8_t cis_ch_id)
{
    for (unsigned int i = 0, j = 0; i < lea_offload_cfg.decodec_info.pcm_size/(sizeof(uint32_t)); i++)
    {
        dma_buf[j] = (dma_buf[j] / (cis_ch_id+1)) + (pcm_buf[i] / (cis_ch_id+1));
        j++;
    }
}

POSSIBLY_UNUSED static void gaf_fill_dma_buf_16bit_1v2LR(int16_t *dma_buf, int16_t *pcm_buf, uint8_t cis_ch_id)
{
    for (unsigned int i = 0, j = cis_ch_id; i < lea_offload_cfg.decodec_info.pcm_size/(lea_offload_cfg.decodec_info.bits_depth/8)/lea_offload_cfg.lc3_ch_num; i++)
    {
        dma_buf[j] = pcm_buf[i];
        j++;
        j++;
    }
}
#ifdef BLE_USB_AUDIO_IS_DONGLE_ROLE
POSSIBLY_UNUSED static void gaf_mobile_fill_dma_buf_16bit_1v2LR(int16_t *dma_buf, int16_t *pcm_buf, uint8_t cis_ch_id)
{
    // the num 2 is USB_AUDIO_SEND_CHAN
    for (unsigned int i = 0, j = cis_ch_id; i < (lea_offload_cfg.decodec_info.pcm_size*2)/(lea_offload_cfg.decodec_info.bits_depth/8)/lea_offload_cfg.lc3_ch_num; i++)
    {
        dma_buf[j] = pcm_buf[i];
        j++;
        j++;
    }
}

POSSIBLY_UNUSED static void gaf_fill_dma_buf_32bit_1v2LR(int32_t *dma_buf, int32_t *pcm_buf, uint8_t cis_ch_id)
{
    if(lea_offload_cfg.decodec_info.num_channels == 2)
    {
    // the num 2 is USB_AUDIO_SEND_CHAN
        for (unsigned int i = 0, j = cis_ch_id; i < (lea_offload_cfg.decodec_info.pcm_size)/(lea_offload_cfg.decodec_info.bits_depth/8); i++)
        {
            dma_buf[j] = pcm_buf[i];
            j++;
            j++;
        }
    }
    else
    {
        for (unsigned int i = 0, j = cis_ch_id; i < lea_offload_cfg.decodec_info.pcm_size/(lea_offload_cfg.decodec_info.bits_depth/8); i++)
        {
            // the channel num is 1
            dma_buf[j] = pcm_buf[i];
            j++;
        }
    }
}

#endif

#if defined(GAF_OFFLOAD_MIC) && defined(BLE_USB_AUDIO_IS_DONGLE_ROLE)
static void wirelessmic_decode_iso_run(uint8_t cis_ch_id, struct Iso_intr iso_rx_ins)
{
    lea_offload_cfg.decoder_func_list->decoder_decode_frame_func(cis_ch_id, NULL, iso_rx_ins.payload_len, (void*)iso_rx_ins.enc_data, iso_rx_ins.pcm_buf, iso_rx_ins.isPLC);
    gaf_fill_dma_buf_32bit_1v2mix((int32_t *)iso_rx_ins.dma_buf, (int32_t *)iso_rx_ins.pcm_buf, cis_ch_id);
    LOG_D("enter channel %d", cis_ch_id);

#if defined(WIRELESS_MIC_RX) && defined(AUD_OUTPUT_BY_USB)
    int32_t* pcm_buff = (int32_t *)iso_rx_ins.pcm_buf;
    for (uint16_t i = 0; i < lea_offload_cfg.decodec_info.pcm_size / 4 / 2; i++) {
        audio_data_usb_capture_stream_buffer[2 * i + cis_ch_id] = (int32_t)pcm_buff[i];
    }
#endif

    cis_ch_increase();

    if (get_cis_ch() == iso_rx_ins.total_ch)
    {
#if defined(WIRELESS_MIC_RX) && defined(AUD_OUTPUT_BY_USB)
        if (audio_data_usb_capture_stream_running()) {
            audio_data_usb_capture_stream_data_input((uint8_t*)audio_data_usb_capture_stream_buffer, lea_offload_cfg.decodec_info.pcm_size);
            memset(audio_data_usb_capture_stream_buffer, 0x0, sizeof(audio_data_usb_capture_stream_buffer));
        }
        return;
#endif
        // the num 2 is USB_AUDIO_SEND_CHAN
        int *_pcm_buf = (int *)iso_rx_ins.dma_buf;
#ifndef USB_AUDIO_SEND_24BIT
        short *usb_send_buf = (short *)iso_rx_ins.pcm_buf;
        for (uint32_t i = 0, j = 0; i < lea_offload_cfg.decodec_info.pcm_size/sizeof(uint32_t); i++)
        {
            usb_send_buf[j] = _pcm_buf[i] >> 8;
            j++;
        }

        gaf_source_stream_data_rx_write((uint8_t* )usb_send_buf, (lea_offload_cfg.decodec_info.pcm_size/4) * 2);
#else
        uint8_t *usb_send_buf = (uint8_t *)iso_rx_ins.pcm_buf;
        for (size_t i = 0 ,j = 0; i < lea_offload_cfg.decodec_info.pcm_size/sizeof(uint32_t); i++) {
            uint32_t val = _pcm_buf[i];
            usb_send_buf[j * 3 + 0] = val & 0xFF;
            usb_send_buf[j * 3 + 1] = (val >> 8)  & 0xFF;
            usb_send_buf[j * 3 + 2] = (val >> 16) & 0xFF;
            j++;
            j++;
        }

        gaf_source_stream_data_rx_write((uint8_t* )usb_send_buf, (lea_offload_cfg.decodec_info.pcm_size/4) * 3 * 2);
#endif
    }
}
#endif
#ifdef BLE_USB_AUDIO_IS_HEADSET_ROLE
static void headset_decode_iso_run(uint8_t cis_ch_id, struct Iso_intr iso_rx_ins)
{
    lea_offload_cfg.decoder_func_list->decoder_decode_frame_func(cis_ch_id, NULL, iso_rx_ins.payload_len, (void*)iso_rx_ins.enc_data, iso_rx_ins.pcm_buf, iso_rx_ins.isPLC);

    LOG_D("enter channel %d", cis_ch_id);
    if (lea_offload_cfg.decodec_info.bits_depth == AUD_BITS_16)
    {
        gaf_fill_dma_buf_16bit((int16_t *)iso_rx_ins.dma_buf, (int16_t *)iso_rx_ins.pcm_buf, cis_ch_id);
    }
    else if ((lea_offload_cfg.decodec_info.bits_depth == AUD_BITS_32)||(lea_offload_cfg.decodec_info.bits_depth == AUD_BITS_24))
    {
        gaf_fill_dma_buf_32bit((int32_t *)iso_rx_ins.dma_buf, (int32_t *)iso_rx_ins.pcm_buf, cis_ch_id);
    }
#ifdef OFFLOAD_AUDIO_DUMP
    audio_dump_add_channel_data_from_multi_channels_32bit_to_16bit(cis_ch_id, (void *)iso_rx_ins.pcm_buf, CODEC_OUT_FRAME_TEST/sizeof(int32_t)/2, 1, 0, 8);
#endif
    cis_ch_increase();
}
#endif

#ifdef BLE_USB_AUDIO_IS_DONGLE_ROLE
static void dongle_decode_iso_run(uint8_t cis_ch_id, struct Iso_intr iso_rx_ins)
{
    lea_offload_cfg.decoder_func_list->decoder_decode_frame_func(cis_ch_id, NULL, iso_rx_ins.payload_len, (void*)iso_rx_ins.enc_data, iso_rx_ins.pcm_buf, iso_rx_ins.isPLC);

    LOG_D("enter channel %d", cis_ch_id);
    if (lea_offload_cfg.decodec_info.bits_depth == AUD_BITS_16)
    {
        gaf_mobile_fill_dma_buf_16bit_1v2LR((int16_t *)iso_rx_ins.dma_buf, (int16_t *)iso_rx_ins.pcm_buf, cis_ch_id);
    }
    else if ((lea_offload_cfg.decodec_info.bits_depth == AUD_BITS_32)||(lea_offload_cfg.decodec_info.bits_depth == AUD_BITS_24))
    {
        gaf_fill_dma_buf_32bit_1v2LR((int32_t *)iso_rx_ins.dma_buf, (int32_t *)iso_rx_ins.pcm_buf, cis_ch_id);
    }

#ifdef OFFLOAD_AUDIO_DUMP
    audio_dump_add_channel_data_from_multi_channels_32bit_to_16bit(cis_ch_id, (void *)iso_rx_ins.pcm_buf, CODEC_OUT_FRAME_TEST/sizeof(int32_t)/2, 1, 0, 8);
#endif
    cis_ch_increase();
    if (get_cis_ch() == iso_rx_ins.total_ch)
    {
#ifdef __BLE_AUDIO_24BIT__
        // the num 2 is USB_AUDIO_SEND_CHAN
        int *_pcm_buf = (int *)iso_rx_ins.dma_buf;
#ifndef USB_AUDIO_SEND_24BIT
        short *usb_send_buf = (short *)iso_rx_ins.pcm_buf;
        for (uint32_t i = 0, j = 0; i < lea_offload_cfg.decodec_info.pcm_size/sizeof(uint32_t); i++)
        {
            usb_send_buf[j] = _pcm_buf[i] >> 8;
            j++;
            j++;
        }

        gaf_source_stream_data_rx_write((uint8_t* )usb_send_buf, (lea_offload_cfg.decodec_info.pcm_size/4) * 2 * 2);
#else
        uint8_t *usb_send_buf = (uint8_t *)iso_rx_ins.pcm_buf;
        for (size_t i = 0 ,j = 0; i < lea_offload_cfg.decodec_info.pcm_size/sizeof(uint32_t); i++) {
            uint32_t val = _pcm_buf[i];
            usb_send_buf[j * 3 + 0] = val & 0xFF;
            usb_send_buf[j * 3 + 1] = (val >> 8)  & 0xFF;
            usb_send_buf[j * 3 + 2] = (val >> 16) & 0xFF;
            j++;
            j++;
        }

        gaf_source_stream_data_rx_write((uint8_t* )usb_send_buf, (lea_offload_cfg.decodec_info.pcm_size/4) * 3 * 2);
#endif
#else
        // in this place, lea_offload_cfg.decodec_info.pcm_size or lea_offload_cfg.decodec_info.pcm_size*2 is depended on cis_cnt_in_cig
        gaf_source_stream_data_rx_write((uint8_t* )iso_rx_ins.dma_buf, lea_offload_cfg.decodec_info.pcm_size);
#endif
    }
}
#endif

static uint16_t con_record = 0;
static void iso_rx_ins_cfg(struct Iso_intr* iso_rx_ins, const unsigned char *data)
{
    struct hci_iso_timestamp_test_header_t* iso_data_header = (struct hci_iso_timestamp_test_header_t*)data;

    iso_rx_ins->payload_len = iso_data_header->iso_sdu_length;
    iso_rx_ins->dma_buf = (void *)ull_audio_get_codec_out_buf_lli();
    iso_rx_ins->pcm_buf = (void *)rx_pcm_cache_buf;
    iso_rx_ins->isPLC = false;
    iso_rx_ins->enc_data = iso_data_header+1;
#ifdef GAF_OFFLOAD_MICCH_USE_ALLOCATION
    uint32_t audio_allocation_bf = 0;
    bool is_right_channel = false;
#endif
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = lea_offload_cfg.pStreamEnv_ptr;
    if (!pStreamEnv)
    {
        iso_rx_ins->gaf_offload_decode_iso_run = NULL;
        return;
    }


    for (uint8_t i = 0; i < BLE_CONNECTION_MAX; i++)
    {
        if (lea_offload_cfg.iso_handler[i] == iso_data_header->iso_conn_handle)
        {
            iso_rx_ins->cur_ch = i;
            break;
        }
        else if (!lea_offload_cfg.iso_handler[i])
        {
            lea_offload_cfg.iso_handler[i] = iso_data_header->iso_conn_handle;
            iso_rx_ins->cur_ch = i;
            break;
        }
        if (i == BLE_CONNECTION_MAX) 
        {
            ASSERT(0, "err iso handler %d", iso_data_header->iso_conn_handle);
            iso_rx_ins->cur_ch = 0xFF;
        }
    }

    for (uint8_t i = 0, j = 0; i < GAF_AUDIO_ASE_TOTAL_COUNT; i++)
    {
        if (pStreamEnv->stream_info.playbackInfo.aseChInfo[i].iso_channel_hdl == GAF_AUDIO_INVALID_ISO_CHANNEL)
        {
            continue;
        }
        j++;
        iso_rx_ins->total_ch = j;
#ifdef GAF_OFFLOAD_MICCH_USE_ALLOCATION
        if(pStreamEnv->stream_info.playbackInfo.aseChInfo[i].iso_channel_hdl == (iso_data_header->iso_conn_handle & 0xFF))
        {
            audio_allocation_bf = pStreamEnv->stream_info.playbackInfo.aseChInfo[i].allocation_bf;
            is_right_channel = ((audio_allocation_bf & (0x00000800 | 0x00000002)) != 0);
            if (is_right_channel)
            {
                iso_rx_ins->cur_ch = 1;
            }
            else
            {
                iso_rx_ins->cur_ch = 0;
            }
            LOG_I("iso_rx_ins->total_ch %d playback_Info %d %d", iso_rx_ins->total_ch, pStreamEnv->stream_info.playbackInfo.aseChInfo[i].allocation_bf, is_right_channel);
        }
#endif
    }

#ifdef BLE_USB_AUDIO_IS_HEADSET_ROLE
    iso_rx_ins->gaf_offload_decode_iso_run = headset_decode_iso_run;
#endif
#ifdef BLE_USB_AUDIO_IS_DONGLE_ROLE
    iso_rx_ins->gaf_offload_decode_iso_run = dongle_decode_iso_run;
#endif

#if defined(GAF_OFFLOAD_MIC) && defined(BLE_USB_AUDIO_IS_DONGLE_ROLE)
    iso_rx_ins->gaf_offload_decode_iso_run = wirelessmic_decode_iso_run;
#endif
    con_record = iso_data_header->iso_conn_handle;
    LOG_D("iso handle 0x%x length 0x%x ",iso_data_header->iso_conn_handle, iso_rx_ins->payload_len);
}

static void btc2dsp_rx(const unsigned char *data, unsigned int len)
{

#ifdef ULL_AUDIO_GPIO_TEST
    hal_iomux_tportset(1);
#endif

    struct Iso_intr iso_rx_ins;

    iso_rx_ins_cfg(&iso_rx_ins, data);
    if(check_iso_data(&iso_rx_ins))
    {
        cis_ch_increase();
        return;
    }

#ifdef ULL_AUDIO_GPIO_TEST
    uint8_t* test_flag = (uint8_t*)(iso_rx_ins.enc_data);
    if((test_flag[0]==0x12)&&(test_flag[1]==0x34)&&(test_flag[2]==0x56)&&(test_flag[3]==0x78))
    {
        hal_iomux_tportset(0);
        hal_iomux_tportclr(0);
        hal_iomux_tportclr(1);
        cis_ch_increase();
        return;
    }
#endif

    if (iso_rx_ins.gaf_offload_decode_iso_run)
    {
        iso_rx_ins.gaf_offload_decode_iso_run(iso_rx_ins.cur_ch, iso_rx_ins);
    }

#ifdef ULL_AUDIO_GPIO_TEST
    hal_iomux_tportclr(1);
#endif
    return;
}

POSSIBLY_UNUSED static void regroup_data(struct Codec_intr* ptr)
{
    int16_t* dst = (int16_t*)ptr->chace_buf;

    for (uint32_t i = 0, j = 0, m = 0; i < (ptr->pcm_size/sizeof(int16_t)/ 2); i++) {
        int in_idx = m++;
        dst[j++] = ptr->src_buf[in_idx];
        dst[j++] = ptr->src_buf[4 + in_idx];
        if (m % 4 == 0)
        {
            m = m + 4;
        }
    }

    memcpy(ptr->src_buf, dst, ptr->pcm_size);
}
#if defined(BLE_USB_AUDIO_IS_HEADSET_ROLE) && defined(GAF_OFFLOAD_MIC)
static void wireless_mic_codec_in_capture_handler(uint8_t chan, uint32_t remains, uint32_t error, struct HAL_DMA_DESC_T *lli)
{
    hal_iomux_tportset(0);
    struct Codec_intr codec_in_instant;
    codec_in_instant.pcm_size = lea_offload_cfg.encodec_info.pcm_size;
    codec_in_instant.dma_desc = &codec_in_dma_desc[0];
    codec_in_instant.chace_buf = (int16_t*)codec_out_buf_test;
    codec_in_instant.get_codec_ptr = get_codec_ptr;
    codec_in_instant.regroup_data = regroup_data;
    codec_in_instant.src_buf = (uint8_t *)codec_in_instant.get_codec_ptr(&codec_in_instant, lli);
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = lea_offload_cfg.pStreamEnv_ptr;
    uint32_t payload_len_per_channel = 0;
    uint16_t payload_size = (lea_offload_cfg.encodec_info.frame_size*lea_offload_cfg.encodec_info.num_channels);

    // codec_in_instant.regroup_data(&codec_in_instant);

    audio_process_vol_run((int32_t*)codec_in_instant.src_buf, codec_in_instant.pcm_size/sizeof(int32_t));

#ifdef OFFLOAD_AUDIO_DUMP
    codec_in_audio_dump(codec_in_instant);
#endif

#ifdef ULL_AUDIO_GPIO_TEST
    static uint32_t cnt=0;
    cnt++;
    if (cnt%1000==0)
    {
        hal_iomux_tportset(1);
        hal_iomux_tportclr(1);
        codec_in_instant.src_buf[0] = 0x12;
        codec_in_instant.src_buf[1] = 0x34;
        codec_in_instant.src_buf[2] = 0x56;
        codec_in_instant.src_buf[3] = 0x78;
    }
#endif

    if ((lea_offload_cfg.lc3_codec_complete & GAF_OFFLOAD_ENCODER_INIT_COMPLETED) == 0)
    {
        return;
    }
    lea_offload_cfg.encoder_func_list->encoder_encode_frame_func(0, NULL, lea_offload_cfg.encodec_info.pcm_size, codec_in_instant.src_buf, \
    (lea_offload_cfg.encodec_info.frame_size*lea_offload_cfg.encodec_info.num_channels), encode_cache_buf);

    for (uint8_t i = 0; i < GAF_AUDIO_ASE_TOTAL_COUNT; i++)
    {
        if (GAF_AUDIO_INVALID_ISO_CHANNEL == pStreamEnv->stream_info.captureInfo.aseChInfo[i].iso_channel_hdl)
        {
            continue;
        }
        // Config palyload len  for AUD_CHANNEL_NUM_2
        payload_len_per_channel = payload_size / pStreamEnv->stream_info.captureInfo.codec_info.num_channels;

        hci_tx_btc_audio_test(pStreamEnv->stream_info.captureInfo.aseChInfo[i].ase_handle,encode_cache_buf, payload_len_per_channel);
    }
#ifdef ULL_AUDIO_GPIO_TEST
    hal_iomux_tportclr(0);
#endif
}
#endif

static void gaf_offload_capture_vol_changes(const float gain)
{
    audio_process_vol_start_impl(gain, 5);
}

#if defined(BLE_USB_AUDIO_IS_HEADSET_ROLE) && !defined(GAF_OFFLOAD_MIC)
static void codec_in_capture_handler(uint8_t chan, uint32_t remains, uint32_t error, struct HAL_DMA_DESC_T *lli)
{
    hal_iomux_tportset(0);
    struct Codec_intr codec_in_instant;
    codec_in_instant.pcm_size = lea_offload_cfg.encodec_info.pcm_size;
    codec_in_instant.dma_desc = &codec_in_dma_desc[0];
    codec_in_instant.chace_buf = (int16_t*)codec_out_buf_test;
    codec_in_instant.get_codec_ptr = get_codec_ptr;
    codec_in_instant.regroup_data = regroup_data;
    codec_in_instant.src_buf = (uint8_t *)codec_in_instant.get_codec_ptr(&codec_in_instant, lli);
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = lea_offload_cfg.pStreamEnv_ptr;
    uint32_t payload_len_per_channel = 0;
    uint16_t payload_size = (lea_offload_cfg.encodec_info.frame_size*lea_offload_cfg.encodec_info.num_channels);

    // codec_in_instant.regroup_data(&codec_in_instant);

#ifdef OFFLOAD_AUDIO_DUMP
    codec_in_audio_dump(codec_in_instant);
#endif

#ifdef ULL_AUDIO_GPIO_TEST
    static uint32_t cnt=0;
    cnt++;
    if (cnt%1000==0)
    {
        hal_iomux_tportset(1);
        hal_iomux_tportclr(1);
        codec_in_instant.src_buf[0] = 0x12;
        codec_in_instant.src_buf[1] = 0x34;
        codec_in_instant.src_buf[2] = 0x56;
        codec_in_instant.src_buf[3] = 0x78;
    }
#endif

    if ((lea_offload_cfg.lc3_codec_complete & GAF_OFFLOAD_ENCODER_INIT_COMPLETED) == 0)
    {
        return;
    }
    lea_offload_cfg.encoder_func_list->encoder_encode_frame_func(0, NULL, lea_offload_cfg.encodec_info.pcm_size, codec_in_instant.src_buf, \
    (lea_offload_cfg.encodec_info.frame_size*lea_offload_cfg.encodec_info.num_channels), encode_cache_buf);

    for (uint8_t i = 0; i < GAF_AUDIO_ASE_TOTAL_COUNT; i++)
    {
        if (GAF_AUDIO_INVALID_ISO_CHANNEL == pStreamEnv->stream_info.captureInfo.aseChInfo[i].iso_channel_hdl)
        {
            continue;
        }
        // Config palyload len  for AUD_CHANNEL_NUM_2
        payload_len_per_channel = payload_size / pStreamEnv->stream_info.captureInfo.codec_info.num_channels;

        hci_tx_btc_audio_test(pStreamEnv->stream_info.captureInfo.aseChInfo[i].ase_handle,encode_cache_buf, payload_len_per_channel);
    }
#ifdef ULL_AUDIO_GPIO_TEST
    hal_iomux_tportclr(0);
#endif
}
#endif

static void codec_out_playback_handler(uint8_t chan, uint32_t remains, uint32_t error, struct HAL_DMA_DESC_T *lli)
{
#ifdef ULL_AUDIO_GPIO_TEST
    hal_iomux_tportset(0);
#endif
    int idx = (lli == &codec_out_dma_desc[0]) ? 0 : 1;
    codec_out_buf_lli = (void *)&codec_out_buf[idx][0];
    cis_ch_reset();
#ifdef OFFLOAD_AUDIO_DUMP
    audio_dump_run();
#endif
    memset(codec_out_buf_lli, 0, CODEC_OUT_FRAME_SIZE);
#ifdef ULL_AUDIO_GPIO_TEST
    hal_iomux_tportclr(0);
#endif
    LOG_D("codec_out_playback_handler");
}
#ifdef BLE_USB_AUDIO_IS_DONGLE_ROLE
#ifdef APP_BLE_USB_AUDIO_RESAMPLE_SYNC
POSSIBLY_UNUSED struct APP_RESAMPLE_T *gaf_offload_resample = NULL;
POSSIBLY_UNUSED static SRAM_BSS_LOC int32_t usb_24bit_dst[500];

FRAM_TEXT_LOC
POSSIBLY_UNUSED static int iter_cb(uint8_t *buf, uint32_t len)
{
    EnCQueue(&bts_lea_uc_cli_env->tx_pcm_queue, (uint8_t*)buf, len);
    return 0;
}
#define CALIB_FACTOR_DELAY_TEST (0.000100f)

FRAM_TEXT_LOC
void gaf_offload_stream_data_tx_read(uint8_t* data, uint32_t len)
{
    if (!bts_lea_uc_cli_env)
    {
        return;
    }

    if (!bts_lea_uc_cli_env->tx_packet_start)
    {
        bts_lea_uc_cli_env->tx_packet_start = true;
#ifdef __BLE_AUDIO_24BIT__
        gaf_offload_resample = app_capture_resample_32bit_any_open(AUD_CHANNEL_NUM_2, iter_cb, len/5, 1.0f);
#else
        gaf_offload_resample = app_capture_resample_any_open(AUD_CHANNEL_NUM_2, iter_cb, len/5, 1.0f);
#endif

        LOG_I("[%s][%d]: read start! len %d", __FUNCTION__, __LINE__, len);
        return;
    }

    uint32_t left_len = LengthOfCQueue(&bts_lea_uc_cli_env->tx_pcm_queue);
    if(gaf_offload_resample)
    {
        DeCQueue(&bts_lea_uc_cli_env->tx_pcm_queue, data, len);
        if (left_len > (len/5)*7)
        {
            app_resample_tune(gaf_offload_resample, -CALIB_FACTOR_DELAY_TEST);
        }
        if (left_len < (len/5)*6)
        {
            app_resample_tune(gaf_offload_resample, CALIB_FACTOR_DELAY_TEST);
        }
    }
    TRACE(0, "data_tx_read queue have %d, data need %d", left_len, len);
}

FRAM_TEXT_LOC
void gaf_offload_stream_data_tx_write(uint8_t* data, uint32_t len, uint8_t bits_byte)
{
    if (!bts_lea_uc_cli_env)
    {
        return;
    }

    if (!bts_lea_uc_cli_env->tx_packet_start)
    {
        return;
    }

    if(bts_lea_uc_cli_env->tx_packet_start)
    {
        hal_iomux_tportset(1);

        if (bits_byte >= 3)
        {
            int32_t *ptr = (int32_t *)data;
            for (uint32_t i = 0; i < len/4; i++) {
                ptr[i] = ptr[i] >> 8;
            }
        }
        app_capture_resample_run(gaf_offload_resample, data, len);
        hal_iomux_tportclr(1);
    }
}
#endif


static void gaf_mobile_offload_audio_process_pcm_data_send(const uint8_t *buf, uint16_t size)
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = lea_offload_cfg.pStreamEnv_ptr;
    uint8_t *payload = (uint8_t *)buf;

    uint32_t payload_len_per_channel = 0;
    uint32_t channel_shift = 0;
    uint32_t audio_allocation_bf = 0;
    uint8_t audio_allocation_cnt = 0;
    bool stereo_channel_support = false;
    bool is_right_channel = false;
#ifdef ULL_AUDIO_GPIO_TEST
    hal_iomux_tportclr(0);
#endif
    // Check all ase for streaming send
    for (uint8_t i = 0; i < GAF_AUDIO_ASE_TOTAL_COUNT; i++)
    {
        if (pStreamEnv->stream_info.captureInfo.aseChInfo[i].iso_channel_hdl == GAF_AUDIO_INVALID_ISO_CHANNEL)
        {
            continue;
        }
        channel_shift = 0;
#ifdef BLE_AUDIO_STEREO_CHAN_OVER_CIS_CNT
        stereo_channel_support = 1;
#else
        stereo_channel_support = 0;
#endif
        audio_allocation_bf = pStreamEnv->stream_info.captureInfo.aseChInfo[i].allocation_bf;
        is_right_channel = ((audio_allocation_bf & (0x00000800 | 0x00000002)) != 0);


        // Config palyload len  for AUD_CHANNEL_NUM_2
        payload_len_per_channel = size / AUD_CHANNEL_NUM_2;
        // Most of time, pStreamEnv->stream_info.captureInfo.num_channels == AUD_CHANNEL_NUM_2
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
            if (audio_allocation_cnt == AUD_CHANNEL_NUM_1 && is_right_channel)
            {
                // shift to right channel
                channel_shift += payload_len_per_channel;
            }
            else if (audio_allocation_cnt == AUD_CHANNEL_NUM_2)
            {
            }
            payload_len_per_channel = size;
        }
        LOG_I("[CAPTURE SEND] p_len:%d stereo supp: %d, allocation_bf: 0x%x, shift :%d",
              payload_len_per_channel, stereo_channel_support, audio_allocation_bf, channel_shift);
#ifdef ULL_AUDIO_GPIO_TEST
        hal_iomux_tportset(0);
#endif
        hci_tx_btc_audio_test(pStreamEnv->stream_info.captureInfo.aseChInfo[i].ase_handle, payload + channel_shift, payload_len_per_channel);
    }
}

static void gaf_mobile_offload_data_send(const uint8_t *buf, uint16_t size)
{
    gaf_mobile_offload_audio_process_pcm_data_send(buf, size);
}

POSSIBLY_UNUSED static void mobile_codec_in_capture_handler(uint8_t chan, uint32_t remains, uint32_t error, struct HAL_DMA_DESC_T *lli)
{
#ifdef ULL_AUDIO_GPIO_TEST
    hal_iomux_tportset(0);
#endif
#ifdef APP_BLE_USB_AUDIO_RESAMPLE_SYNC
    gaf_offload_stream_data_tx_read(codec_out_buf_test, lea_offload_cfg.encodec_info.pcm_size);
#else
    gaf_source_stream_data_tx_read(codec_out_buf_test, lea_offload_cfg.encodec_info.pcm_size);
#endif
    POSSIBLY_UNUSED struct Codec_intr codec_out_instant;

    codec_out_instant.dma_desc = &codec_in_dma_desc[0];
    codec_out_instant.get_codec_ptr = get_codec_ptr;
    codec_out_instant.src_buf = codec_out_buf_test;

    if ((lea_offload_cfg.lc3_codec_complete & GAF_OFFLOAD_ENCODER_INIT_COMPLETED) == 0)
    {
        return;
    }
    lea_offload_cfg.encoder_func_list->encoder_encode_frame_func(0, NULL, lea_offload_cfg.encodec_info.pcm_size, codec_out_instant.src_buf, \
    (lea_offload_cfg.encodec_info.frame_size*lea_offload_cfg.encodec_info.num_channels), encode_cache_buf);

    gaf_mobile_offload_data_send(encode_cache_buf, (lea_offload_cfg.encodec_info.frame_size*lea_offload_cfg.encodec_info.num_channels));
#ifdef ULL_AUDIO_GPIO_TEST
    hal_iomux_tportclr(0);
#endif
}
#endif

static void offload_enc_dma_cfg(CODEC_INFO_T* codec_cfg)
{
    uint32_t i;
    struct HAL_DMA_CH_CFG_T codec_dma_cfg;

    TRACE(2,"%s", __func__);
    memset(&codec_dma_cfg, 0, sizeof(codec_dma_cfg));
    codec_dma_cfg.dst_bsize = HAL_DMA_BSIZE_4;
    if (codec_cfg->bits_depth == 16)
    {
        codec_dma_cfg.dst_width = HAL_DMA_WIDTH_HALFWORD;
    }
    else if ((codec_cfg->bits_depth == 32)||(codec_cfg->bits_depth == 24))
    {
        codec_dma_cfg.dst_width = HAL_DMA_WIDTH_WORD;
    }
    codec_dma_cfg.handler = lea_offload_cfg.codec_in_handler;
    codec_dma_cfg.src = 0; // useless
    codec_dma_cfg.src_bsize = HAL_DMA_BSIZE_4;
    codec_dma_cfg.src_periph = HAL_AUDMA_CODEC_RX;
    codec_dma_cfg.src_tsize = codec_cfg->pcm_size / (codec_cfg->bits_depth/8);
    if ((codec_cfg->bits_depth == 24))
    {
        codec_dma_cfg.src_tsize = codec_cfg->pcm_size / 4;
    }
    if (codec_cfg->bits_depth == 16)
    {
        codec_dma_cfg.src_width = HAL_DMA_WIDTH_HALFWORD;
    }
    else if ((codec_cfg->bits_depth == 32)||(codec_cfg->bits_depth == 24))
    {
        codec_dma_cfg.src_width = HAL_DMA_WIDTH_WORD;
    }
    codec_dma_cfg.try_burst = 1;
    codec_dma_cfg.type = HAL_DMA_FLOW_P2M_DMA;
    codec_dma_cfg.ch = hal_audma_get_chan(codec_dma_cfg.src_periph, HAL_DMA_HIGH_PRIO);
    for (i = 0; i < 2; i++) {
        codec_dma_cfg.dst = (uint32_t)&codec_in_buf[i][0];
        hal_audma_init_desc(&codec_in_dma_desc[i], &codec_dma_cfg, &codec_in_dma_desc[(i + 1) % 2], 1);
    }
    hal_audma_sg_start(&codec_in_dma_desc[0], &codec_dma_cfg);
    hal_dma_tc_irq_enable(codec_dma_cfg.ch);
    lea_offload_cfg.offload_capture_ch = codec_dma_cfg.ch;
}

static void offload_dec_dma_cfg(CODEC_INFO_T* codec_cfg)
{
    uint32_t i;
    struct HAL_DMA_CH_CFG_T codec_dma_cfg;

    memset(&codec_dma_cfg, 0, sizeof(codec_dma_cfg));
    codec_dma_cfg.dst = 0; // useless
    codec_dma_cfg.dst_bsize = HAL_DMA_BSIZE_4;
    codec_dma_cfg.dst_periph = HAL_AUDMA_CODEC_TX;
    if (codec_cfg->bits_depth == 16)
    {
        codec_dma_cfg.dst_width = HAL_DMA_WIDTH_HALFWORD;
    }
    else if ((codec_cfg->bits_depth == 32)||(codec_cfg->bits_depth == 24))
    {
        codec_dma_cfg.dst_width = HAL_DMA_WIDTH_WORD;
    }
    codec_dma_cfg.handler = lea_offload_cfg.codec_out_handler;
    codec_dma_cfg.src_bsize = HAL_DMA_BSIZE_4;
    codec_dma_cfg.src_tsize = codec_cfg->pcm_size / (codec_cfg->bits_depth/8);
    if ((codec_cfg->bits_depth == 24))
    {
        codec_dma_cfg.src_tsize = codec_cfg->pcm_size / 4;
    }
    if (codec_cfg->bits_depth == 16)
    {
        codec_dma_cfg.src_width = HAL_DMA_WIDTH_HALFWORD;
    }
    else if ((codec_cfg->bits_depth == 32)||(codec_cfg->bits_depth == 24))
    {
        codec_dma_cfg.src_width = HAL_DMA_WIDTH_WORD;
    }
    codec_dma_cfg.try_burst = 1;
    codec_dma_cfg.type = HAL_DMA_FLOW_M2P_DMA;
    codec_dma_cfg.ch = hal_audma_get_chan(codec_dma_cfg.dst_periph, HAL_DMA_HIGH_PRIO);
    for (i = 0; i < 2; i++) {
        codec_dma_cfg.src = (uint32_t)&codec_out_buf[i][0];
        hal_audma_init_desc(&codec_out_dma_desc[i], &codec_dma_cfg, &codec_out_dma_desc[(i + 1) % 2], 1);
    }
    hal_audma_sg_start(&codec_out_dma_desc[0], &codec_dma_cfg);
    hal_dma_tc_irq_enable(codec_dma_cfg.ch);
    lea_offload_cfg.offload_playback_ch = codec_dma_cfg.ch;
}

static void config_lc3_dec_info_ins(uint8_t instance_handle, void *codec_info)
{
    lea_offload_cfg.decoder_func_list->decoder_init_buf_func(instance_handle, codec_info, NULL, NULL);
    lea_offload_cfg.decoder_func_list->decoder_init_func(instance_handle, codec_info);
}

static void config_lc3_enc_info_ins(uint8_t instance_handle, void *codec_info)
{
    lea_offload_cfg.encoder_func_list->encoder_init_buf_func(instance_handle, codec_info, NULL, NULL);
    lea_offload_cfg.encoder_func_list->encoder_init_func(instance_handle, codec_info);
}

void gaf_offload_decodec_init(CODEC_INFO_T* codec_cfg)
{
    offload_dec_dma_cfg(codec_cfg);

    for (uint8_t i = 0; i < BLE_CONNECTION_MAX; i++)
    {
        config_lc3_dec_info_ins(i, codec_cfg);
    }
    lea_offload_cfg.lc3_codec_complete |= GAF_OFFLOAD_DECODER_INIT_COMPLETED;
    TRACE(0, "decodec frame size %d pcm size %d", codec_cfg->frame_size, codec_cfg->pcm_size);
}

void gaf_offload_encodec_init(CODEC_INFO_T* codec_cfg)
{
    offload_enc_dma_cfg(codec_cfg);
// encode instance should be one by default
    for (uint8_t i = 0; i < 1; i++)
    {
        config_lc3_enc_info_ins(i, codec_cfg);
    }
    lea_offload_cfg.lc3_codec_complete |= GAF_OFFLOAD_ENCODER_INIT_COMPLETED;
    TRACE(0, "encodec frame size %d pcm size %d", codec_cfg->frame_size, codec_cfg->pcm_size);
}

extern "C" void gaf_offload_cp_init(void)
{
    cp_accel_register_iso_data_callback(btc2dsp_rx, gaf_offload_lea_offload_cfg);
#ifdef ULL_AUDIO_GPIO_TEST
    hal_iomux_tportopen();
#endif
}
#endif
