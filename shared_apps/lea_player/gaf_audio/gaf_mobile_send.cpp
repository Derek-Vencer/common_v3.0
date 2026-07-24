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
#ifdef AOB_MOBILE_ENABLED
#include "cmsis.h"
#include "cmsis_os.h"
#include "cqueue.h"
#include "hal_trace.h"
#include "plat_types.h"
#include "app_audio.h"
#include "gaf_dbg.h"
#include "bt_drv_interface.h"
#include "gaf_media_common.h"
#include "gaf_source_data.h"
#include "hwtimer_list.h"
#include "bt_service_thread.h"
#include "audioflinger.h"
#include "gaf_media_sync.h"
#include "gaf_audio_path.h"
extern "C" {
#include "usb_audio_app.h"
}
#include "app_dongle.h"
#include "bta_dongle.h"
#include "hal_codec.h"
#include "bta_bt_api.h"

/************************private macro defination***************************/
#define HW_TIMER_PORCESS_PLAYBACK               (1 << 0)
#define HW_TIMER_PORCESS_CAPTURE                (1 << 1)
#define GAF_PCM_Q_CACHE_SIZE                    (10*1024)
#ifndef BT_AUDIO_CACHE_2_UNCACHE
#define BT_AUDIO_CACHE_2_UNCACHE(addr) \
    ((unsigned char *)((unsigned int)addr & ~(0x04000000)))
#endif

#ifdef _AOB_LOW_LATENCY_2_5_MODE_
#define MARGIN_TRIGGER_US   2500
#else
#define MARGIN_TRIGGER_US   5500
#endif
/************************private strcuture defination****************************/
typedef struct
{
    bool     tx_packet_start;
    bool     rx_packet_start;
    CQueue   tx_pcm_queue;
    CQueue   rx_pcm_queue;

    uint8_t stream_flg;
    uint8_t playback_count;
    uint8_t capture_count;

    uint8_t* playback_cache_buf;
    uint8_t* playback_dmaBufPtr;
    uint8_t* capture_cache_buf;
    uint8_t* capture_dmaBufPtr;

    HWTIMER_ID playback_timer_id;
    HWTIMER_ID capture_timer_id;

    GAF_AUDIO_STREAM_ENV_T* playbackStreamEnv;
    GAF_AUDIO_STREAM_ENV_T* captureStreamEnv;
} gaf_cource_send_env_t;

/************************private variable defination************************/
static gaf_cource_send_env_t gaf_cource_send_env = {0};

/**********************private function declaration*************************/

/****************************function defination****************************/
static uint32_t gaf_source_capture_get_trigger_offset(void *_pStreamEnv)
{
    GAF_AUDIO_STREAM_ENV_T *pStreamEnv = (GAF_AUDIO_STREAM_ENV_T*)_pStreamEnv;
    static uint32_t pre_capture_latest_iso_bt_time = 0;
    uint32_t current_bt_time = bt_syn_ble_bt_time_to_bts(btdrv_syn_get_curr_ticks(), 0);
    uint32_t latest_iso_bt_time = gaf_media_common_get_latest_tx_iso_evt_timestamp(pStreamEnv);
    GAF_AUDIO_STREAM_COMMON_INFO_T *captureInfo = &pStreamEnv->stream_info.captureInfo;

    if (latest_iso_bt_time <= current_bt_time)
    {
        latest_iso_bt_time += captureInfo->isoIntervalUs;
    }

    if (latest_iso_bt_time == pre_capture_latest_iso_bt_time)
    {
        latest_iso_bt_time += captureInfo->isoIntervalUs;
    }

    pre_capture_latest_iso_bt_time = latest_iso_bt_time;

    int32_t gapUs = (int32_t)(latest_iso_bt_time - current_bt_time);

    return gapUs;
}

static uint32_t gaf_source_playback_get_trigger_offset(void *_pStreamEnv)
{
    GAF_AUDIO_STREAM_ENV_T *pStreamEnv = (GAF_AUDIO_STREAM_ENV_T*)_pStreamEnv;
    static uint32_t pre_ply_latest_iso_bt_time = 0;

    uint32_t current_bt_time = bt_syn_ble_bt_time_to_bts(btdrv_syn_get_curr_ticks(), 0);
    uint32_t latest_iso_bt_time = gaf_media_common_get_latest_rx_iso_evt_timestamp(pStreamEnv);
    int32_t gapUs = (int32_t)(latest_iso_bt_time - current_bt_time);

    GAF_AUDIO_STREAM_COMMON_INFO_T *playbackInfo = &pStreamEnv->stream_info.playbackInfo;

    if (latest_iso_bt_time <= current_bt_time)
    {
        latest_iso_bt_time += playbackInfo->isoIntervalUs;
    }

    if (latest_iso_bt_time == pre_ply_latest_iso_bt_time)
    {
        latest_iso_bt_time += playbackInfo->isoIntervalUs;
    }

    pre_ply_latest_iso_bt_time = latest_iso_bt_time;

    return gapUs;
}

static uint32_t gaf_source_calculate_playback_trigger_time(GAF_AUDIO_STREAM_ENV_T* pStreamEnv)
{
    uint32_t anch_time = gaf_media_common_get_latest_rx_iso_evt_timestamp(pStreamEnv);
    uint32_t curr_bt_time = bt_syn_ble_bt_time_to_bts(btdrv_syn_get_curr_ticks(), 0);
    uint32_t trigger_time = anch_time;

    while ((int32_t)(trigger_time - curr_bt_time) < 1000)
    {
        trigger_time += pStreamEnv->stream_info.playbackInfo.isoIntervalUs;
    }

    LOG_I("usb playback iso anch %u cur time %u trigger time %u", anch_time, curr_bt_time, trigger_time);
    return trigger_time;
}

static void gaf_source_playback_hw_timer_handler(void *_pStreamEnv)
{
    gaf_cource_send_env.playback_count++;
    bt_svc_base_thread_send_signal();
}

void gaf_source_stream_data_tx_read(uint8_t* data, uint32_t len)
{
    if (!gaf_cource_send_env.captureStreamEnv)
    {
        return;
    }

    if (!gaf_cource_send_env.tx_packet_start)
    {
        gaf_cource_send_env.tx_packet_start = true;
        LOG_I("[%s][%d]: read start!", __FUNCTION__, __LINE__);
        return;
    }
    uint8_t ret = CQ_ERR;
    int left_len = LengthOfCQueue(&gaf_cource_send_env.tx_pcm_queue);

    uint32_t dmaIntervalMs = gaf_cource_send_env.captureStreamEnv->stream_info.captureInfo.dma_info.dmaChunkIntervalUs/1000;
#ifdef BLE_USB_AUDIO_SUPPORT
    if ((left_len > (int)(len/dmaIntervalMs)*2) && (left_len <= (int)len) && usb_get_stream1_first_pkt())
#else
    if ((left_len > (int)(len/dmaIntervalMs)*2) && (left_len <= (int)len))
#endif
    {
        memset(data, 0, len);
        if (left_len == (int)len)
        {
            ret = DeCQueue(&gaf_cource_send_env.tx_pcm_queue, &data[(len/dmaIntervalMs)*(dmaIntervalMs - 3)], (len/dmaIntervalMs)*3);
        }
        else
        {
            ret = DeCQueue(&gaf_cource_send_env.tx_pcm_queue, &data[(len/dmaIntervalMs)*(dmaIntervalMs - 2)], (len/dmaIntervalMs)*2);
        }
    }
    else
    {
        ret = DeCQueue(&gaf_cource_send_env.tx_pcm_queue, data, len);
    }
    LOG_D("data_tx_read queue have %d, data need %d ret %d", left_len, len, ret);

#ifdef BLE_USB_AUDIO_SUPPORT

    if (usb_get_stream1_first_pkt())
    {
        usb_set_stream1_first_pkt(false);
    }
#endif
}

void gaf_source_stream_data_tx_write(uint8_t* data, uint32_t len, uint8_t bits_byte)
{
    if (!gaf_cource_send_env.captureStreamEnv)
    {
        return;
    }

    if (!gaf_cource_send_env.tx_packet_start)
    {
        return;
    }

    if (bits_byte >= 3)
    {
        int32_t *ptr = (int32_t *)data;
        for (uint32_t i = 0; i < len/4; i++) {
            ptr[i] = ptr[i] >> 8;
        }
    }

    if (gaf_cource_send_env.captureStreamEnv)
    {
        gaf_cource_send_env.captureStreamEnv->stream_context.usb_sync.latest_usb_time = gaf_media_sync_get_curr_time();
    }

    if (CQ_OK != EnCQueue(&gaf_cource_send_env.tx_pcm_queue, data, len))
    {
        LOG_I("[%s][%d]: %d", __FUNCTION__, __LINE__, len);
    }
}

void gaf_source_stream_data_rx_read(uint8_t* data, uint32_t len)
{
    if (!gaf_cource_send_env.playbackStreamEnv)
    {
        memset(data, 0, len);
        return;
    }

    if (!gaf_cource_send_env.rx_packet_start)
    {
        memset(data, 0, len);
        return;
    }

    if (CQ_OK != DeCQueue(&gaf_cource_send_env.rx_pcm_queue, data, len))
    {
        memset(data, 0, len);
        LOG_E("[%s][%d]: %d, %d", __FUNCTION__, __LINE__,
              LengthOfCQueue(&gaf_cource_send_env.rx_pcm_queue), len);
    }
}

void gaf_source_stream_data_rx_write(uint8_t* data, uint32_t len)
{
    if (!gaf_cource_send_env.playbackStreamEnv)
    {
        return;
    }

    if (!gaf_cource_send_env.rx_packet_start)
    {
        gaf_cource_send_env.rx_packet_start = true;
        LOG_I("[%s][%d]: read start!", __FUNCTION__, __LINE__);
        return;
    }

    if (CQ_OK != EnCQueue(&gaf_cource_send_env.rx_pcm_queue, data, len))
    {
        LOG_I("[%s][%d]: %d", __FUNCTION__, __LINE__, len);
    }
}

static void gaf_source_send_data_thread(void)
{
    uint32_t trigger_offset;
    GAF_AUDIO_STREAM_COMMON_INFO_T* stream=NULL;

    if (gaf_cource_send_env.playback_count)
    {
        stream = &gaf_cource_send_env.playbackStreamEnv->stream_info.playbackInfo;
        if (gaf_cource_send_env.playback_count > 1)
        {
            LOG_W("send data miss: %d", gaf_cource_send_env.playback_count);
        }

        if (gaf_stream_is_playback_stream_iso_created(gaf_cource_send_env.playbackStreamEnv)) {
            gaf_mobile_usb_feed_data(stream->dma_info.dmaBufPtr, stream->dma_info.dmaChunkSize);
            gaf_source_stream_data_rx_write(stream->dma_info.dmaBufPtr, stream->dma_info.dmaChunkSize);
        }
        else
        {
            LOG_I("playback iso stream stopped");
        }

        gaf_cource_send_env.playback_count = 0;

        if (gaf_cource_send_env.playback_timer_id)
        {
            trigger_offset = gaf_source_playback_get_trigger_offset(gaf_cource_send_env.playbackStreamEnv);
            hwtimer_start(gaf_cource_send_env.playback_timer_id, US_TO_TICKS(trigger_offset));
        }
    }

    if (gaf_cource_send_env.capture_count)
    {
        stream = &gaf_cource_send_env.captureStreamEnv->stream_info.captureInfo;

        if (gaf_cource_send_env.capture_count > 1)
        {
            LOG_W("send data miss: %d", gaf_cource_send_env.playback_count);
        }

        if (gaf_stream_is_capture_stream_iso_created(gaf_cource_send_env.captureStreamEnv)) {
            gaf_source_stream_data_tx_read(stream->dma_info.dmaBufPtr, stream->dma_info.dmaChunkSize);
            gaf_mobile_usb_processing_received_data(stream->dma_info.dmaBufPtr, stream->dma_info.dmaChunkSize);
        }
        else {
            LOG_I("capture iso stream stopped");
        }

        gaf_cource_send_env.capture_count = 0;
        if (gaf_cource_send_env.capture_timer_id)
        {
            uint32_t lock = int_lock();
            trigger_offset = gaf_source_capture_get_trigger_offset(gaf_cource_send_env.captureStreamEnv);
            hwtimer_start(gaf_cource_send_env.capture_timer_id, US_TO_TICKS(trigger_offset));
            int_unlock(lock);
            LOG_D("trigger_offset %u", trigger_offset);
        }
    }
}

void gaf_source_playback_start(void *_pStreamEnv)
{
    uint32_t trig_tick;
    uint32_t current_bt_time;

    if (NULL == _pStreamEnv)
    {
         LOG_E("%s, pStreamEnv is NULL", __func__);
         return;
    }

    if ((!gaf_cource_send_env.capture_timer_id) &&
        (!gaf_cource_send_env.playback_timer_id))
    {
        bt_svc_base_thread_add_task(gaf_source_send_data_thread);
        bt_svc_base_thread_set_priority(osPriorityHigh);
    }

    if (NULL == gaf_cource_send_env.playback_timer_id)
    {
        gaf_cource_send_env.playback_timer_id =
            hwtimer_alloc(gaf_source_playback_hw_timer_handler, _pStreamEnv);
    }
    gaf_cource_send_env.playbackStreamEnv = (GAF_AUDIO_STREAM_ENV_T*)_pStreamEnv;

    gaf_cource_send_env.rx_packet_start = false;

    if (!gaf_cource_send_env.playback_cache_buf)
    {
        app_audio_mempool_get_buff(&gaf_cource_send_env.playback_cache_buf, GAF_PCM_Q_CACHE_SIZE);
    }
    if (!gaf_cource_send_env.playback_dmaBufPtr)
    {
        app_audio_mempool_get_buff(&gaf_cource_send_env.playback_dmaBufPtr,
            gaf_cource_send_env.playbackStreamEnv->stream_info.playbackInfo.dma_info.dmaChunkSize);
    }
    gaf_cource_send_env.playbackStreamEnv->stream_info.playbackInfo.dma_info.dmaBufPtr = gaf_cource_send_env.playback_dmaBufPtr;

    InitCQueue(&gaf_cource_send_env.rx_pcm_queue, GAF_PCM_Q_CACHE_SIZE, gaf_cource_send_env.playback_cache_buf);

    trig_tick = gaf_source_calculate_playback_trigger_time((GAF_AUDIO_STREAM_ENV_T*)_pStreamEnv);
    current_bt_time = bt_syn_ble_bt_time_to_bts(btdrv_syn_get_curr_ticks(), 0);
    LOG_I("hwtimer playback trigger_tick:%u curr_time:%u", trig_tick, current_bt_time);
    hwtimer_update_then_start(gaf_cource_send_env.playback_timer_id,
        gaf_source_playback_hw_timer_handler, _pStreamEnv, US_TO_TICKS(trig_tick - current_bt_time));
}

static void gaf_source_capture_dev_set(struct AF_STREAM_CONFIG_T *stream_cfg)
{
    uint8_t input_type = 0, output_type = 0;
    app_dongle_in_out_type_get(&input_type, &output_type);

    switch (input_type) {
    case BTA_DONGLE_DATA_PATH_LINEIN:
        stream_cfg->channel_num  = (enum AUD_CHANNEL_NUM_T)hal_codec_get_input_path_chan_num(AUD_INPUT_PATH_LINEIN);
        stream_cfg->io_path = AUD_INPUT_PATH_LINEIN;
        stream_cfg->device  = AUD_STREAM_USE_INT_CODEC;
        break;
    case BTA_DONGLE_DATA_PATH_I2S:
        stream_cfg->channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0|AUD_CHANNEL_MAP_CH1);
        stream_cfg->io_path = AUD_IO_PATH_NULL;
        stream_cfg->device  = AUD_STREAM_USE_I2S0_MASTER;
        break;
    case BTA_DONGLE_DATA_PATH_AMIC:
    case BTA_DONGLE_DATA_PATH_DMIC:
        stream_cfg->channel_num  = (enum AUD_CHANNEL_NUM_T)hal_codec_get_input_path_chan_num(AUD_INPUT_PATH_MAINMIC);
        stream_cfg->io_path = AUD_INPUT_PATH_MAINMIC;
        stream_cfg->device  = AUD_STREAM_USE_INT_CODEC;
        break;
    case BTA_DONGLE_DATA_PATH_USB:
        stream_cfg->channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0|AUD_CHANNEL_MAP_CH1);
        stream_cfg->io_path = AUD_INPUT_PATH_MAINMIC;
        stream_cfg->device  = AUD_STREAM_USE_INT_CODEC;
        break;
    case BTA_DONGLE_DATA_PATH_SPDIF:
        /* code */
        break;
    default:
        stream_cfg->channel_num  = (enum AUD_CHANNEL_NUM_T)hal_codec_get_input_path_chan_num(AUD_INPUT_PATH_MAINMIC);
        stream_cfg->io_path = AUD_INPUT_PATH_MAINMIC;
        stream_cfg->device  = AUD_STREAM_USE_INT_CODEC;
        break;
    }
}

void gaf_source_capture_start(void *_pStreamEnv)
{
    if (NULL == _pStreamEnv)
    {
         LOG_E("%s please check param", __func__);
         return;
    }

    if ((!gaf_cource_send_env.capture_timer_id) &&
        (!gaf_cource_send_env.playback_timer_id))
    {
        bt_svc_base_thread_set_priority(osPriorityHigh);
    }

    gaf_cource_send_env.captureStreamEnv = (GAF_AUDIO_STREAM_ENV_T*)_pStreamEnv;

    gaf_cource_send_env.tx_packet_start = false;

    if (!gaf_cource_send_env.capture_cache_buf)
    {
        app_audio_mempool_get_buff(&gaf_cource_send_env.capture_cache_buf, GAF_PCM_Q_CACHE_SIZE);
    }
    if (!gaf_cource_send_env.capture_dmaBufPtr)
    {
        app_audio_mempool_get_buff(&gaf_cource_send_env.capture_dmaBufPtr,
            gaf_cource_send_env.captureStreamEnv->stream_info.captureInfo.dma_info.dmaChunkSize);
    }
    gaf_cource_send_env.captureStreamEnv->stream_info.captureInfo.dma_info.dmaBufPtr = gaf_cource_send_env.capture_dmaBufPtr;

    InitCQueue(&gaf_cource_send_env.tx_pcm_queue, GAF_PCM_Q_CACHE_SIZE, gaf_cource_send_env.capture_cache_buf);

    struct AF_STREAM_CONFIG_T stream_cfg;
    af_set_priority(AF_USER_AOB_CAPTURE, osPriorityRealtime);
    // capture stream
    memset((void *)&stream_cfg, 0, sizeof(struct AF_STREAM_CONFIG_T));
    stream_cfg.bits         = (enum AUD_BITS_T)(gaf_cource_send_env.captureStreamEnv->stream_info.captureInfo.dma_info.bits_depth);
    stream_cfg.channel_num  = (enum AUD_CHANNEL_NUM_T)(gaf_cource_send_env.captureStreamEnv->stream_info.captureInfo.dma_info.num_channels);
    stream_cfg.sample_rate  = (enum AUD_SAMPRATE_T)gaf_cource_send_env.captureStreamEnv->stream_info.captureInfo.dma_info.sample_rate;
    gaf_source_capture_dev_set(&stream_cfg);

    // TODO: get vol from VCC via ase_lid
    stream_cfg.vol          = TGT_VOLUME_LEVEL_7;

    stream_cfg.data_size    = (uint32_t)(2 * gaf_cource_send_env.captureStreamEnv->stream_info.captureInfo.dma_info.dmaChunkSize);
    stream_cfg.data_ptr = BT_AUDIO_CACHE_2_UNCACHE(gaf_cource_send_env.captureStreamEnv->stream_info.captureInfo.dma_info.dmaBufPtr);
    stream_cfg.handler = gaf_cource_send_env.captureStreamEnv->func_list->stream_func_list.capture_dma_irq_handler_func;
    af_stream_open(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE, &stream_cfg);
    TRACE(3, "[%s] Capture: sample rate: %d, data size: %d, channel num: %d",
                                                    __func__,
                                                    stream_cfg.sample_rate,
                                                    stream_cfg.data_size,
                                                    stream_cfg.channel_num);

    gaf_cource_send_env.captureStreamEnv->stream_context.captureTriggerChannel = 0;
    gaf_media_prepare_capture_trigger(gaf_cource_send_env.captureStreamEnv->stream_context.captureTriggerChannel);

    af_stream_start(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
    bta_bt_config_checker_logging(0);

    uint32_t iso_event_timestamp = gaf_media_common_get_latest_tx_iso_evt_timestamp(gaf_cource_send_env.captureStreamEnv);
    uint32_t dmaIntervalUs = gaf_cource_send_env.captureStreamEnv->stream_info.captureInfo.dma_info.dmaChunkIntervalUs;
    uint32_t trigger_time, cnt = 0;
    if (iso_event_timestamp)
    {
        uint32_t current_bt_time = gaf_media_sync_get_curr_time();
        while (GAF_AUDIO_CLK_32_BIT_DIFF(iso_event_timestamp, current_bt_time) > 0)
        {
            iso_event_timestamp += dmaIntervalUs;
            if (cnt > 10)
            {
                ASSERT(0, "[%s] iso_time %d current_bt_time %d", __func__, iso_event_timestamp, current_bt_time);
            }
            cnt++;
        }

        trigger_time = iso_event_timestamp + dmaIntervalUs + MARGIN_TRIGGER_US;
        LOG_I("trigger_time %u iso %u cur %u ", trigger_time, iso_event_timestamp, current_bt_time);
        gaf_stream_common_update_capture_stream_state(gaf_cource_send_env.captureStreamEnv, GAF_CAPTURE_STREAM_START_TRIGGERING);
        gaf_stream_common_set_capture_trigger_time(gaf_cource_send_env.captureStreamEnv, trigger_time);
    }
}

void gaf_source_stop(uint32_t stream)
{
    if (AUD_STREAM_PLAYBACK == stream)
    {
        af_stream_stop(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
        af_stream_close(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
        gaf_stream_common_clr_trigger(gaf_cource_send_env.captureStreamEnv->stream_context.captureTriggerChannel);
        af_set_priority(AF_USER_AOB_CAPTURE, osPriorityAboveNormal);
        gaf_cource_send_env.captureStreamEnv = NULL;
        gaf_cource_send_env.rx_packet_start = false;
    }
    else if (AUD_STREAM_CAPTURE == stream)
    {
        if (gaf_cource_send_env.playback_timer_id)
        {
            hwtimer_stop(gaf_cource_send_env.playback_timer_id);
            hwtimer_free(gaf_cource_send_env.playback_timer_id);
            gaf_cource_send_env.playback_timer_id = NULL;
        }
        gaf_cource_send_env.playbackStreamEnv = NULL;
        gaf_cource_send_env.tx_packet_start = false;
    }

    if ((!gaf_cource_send_env.capture_timer_id) &&
        (!gaf_cource_send_env.playback_timer_id))
    {
        gaf_cource_send_env.playback_cache_buf = NULL;
        gaf_cource_send_env.playback_dmaBufPtr = NULL;
        gaf_cource_send_env.capture_cache_buf  = NULL;
        gaf_cource_send_env.capture_dmaBufPtr  = NULL;
        bt_svc_base_thread_delete_task(gaf_source_send_data_thread);
        bt_svc_base_thread_set_priority(osPriorityNormal);
    }
}


#endif
