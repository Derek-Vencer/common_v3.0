/***************************************************************************
 *
 * Copyright 2015-2024 BES.
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
 * @brief xxx.
 *
 ****************************************************************************/

/****************************** header include ********************************/
#include <stdint.h>
#include "cmsis.h"
#include "cmsis_os.h"
#include "hal_dma.h"
#include "app_trace_rx.h"
#include "bluetooth_bt_api.h"
#include "app_bt_func.h"
#include "app_utils.h"
#include "plat_types.h"
#include "audioflinger.h"
#include "bt_drv_reg_op.h"
#include "bt_drv_interface.h"
#include "app_audio.h"
#include "gaf_audio_path.h"
#include "gaf_mobile_media_stream.h"
#include "aob_media_api.h"
#include "aob_mgr_gaf_evt.h"
#include "gaf_dbg.h"
#include "gaf_media_sync.h"
#include "gaf_media_pid.h"
#include "lc3_process.h"
#include "gaf_codec_lc3.h"
#include "gaf_media_common.h"
#include "app_bt_sync.h"
#include "audio_dump.h"
#include "bt_service.h"
#include "gaf_source_data.h"
#include "app_dongle.h"
#include "bta_dongle.h"
#include "bta_bt_api.h"
/***************************** external declaration *****************************/

/***************************** macro defination *******************************/

/***************************** type defination ********************************/

/***************************** variable defination *****************************/

/***************************** function declaration ****************************/
#define USB_BLE_AUDIO_PLAYBACK_TRIGGER_CHANNEL  0
#define USB_BLE_AUDIO_CAPTURE_TRIGGER_CHANNEL   1

#define USB_REC_AUDIO_DUMP   0
#define CLAMP(x, low, high) ((x) > (high) ? (high) : ((x) < (low) ? (low) : (x)))
extern GAF_AUDIO_STREAM_ENV_T gaf_mobile_audio_stream_env;

static bool app_ble_is_usb_playback_on = false;
static bool app_ble_is_usb_capture_on = false;

void* gaf_mobile_audio_get_media_stream_env(void)
{
    return &gaf_mobile_audio_stream_env;
}

/**
 ****************************************************************************************
 * @brief Called when cis establishment
 *
 * @return void
 ****************************************************************************************
 */

void gap_mobile_start_usb_audio_transmission_dma(void)
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv =
        (GAF_AUDIO_STREAM_ENV_T*)gaf_mobile_audio_get_media_stream_env();

    if (pStreamEnv == NULL)
    {
        LOG_D("%s, pStreamEnv is NULL", __func__);
        return;
    }
    LOG_D("%s", __func__);

    bes_ble_bap_dp_itf_data_come_callback_register((void *)gaf_mobile_audio_receive_data);
}

static void gaf_usb_mobile_audio_process_pcm_data_send(void *pStreamEnv_,void *payload_,
    uint32_t payload_size, uint32_t ref_time)
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T* )pStreamEnv_;
    uint8_t *payload = (uint8_t *)payload_;

    uint32_t payload_len_per_channel = 0;
    uint32_t channel_shift = 0;
    uint32_t audio_allocation_bf = 0;
    uint8_t audio_allocation_cnt = 0;
    bool stereo_channel_support = false;
    bool is_right_channel = false;
    /// @see pCommonInfo->num_channels in SINK
    ASSERT(pStreamEnv->stream_info.captureInfo.codec_info.num_channels == AUD_CHANNEL_NUM_2, "need stereo channel here");
    // Check all ase for streaming send
    for (uint8_t i = 0; i < GAF_AUDIO_ASE_TOTAL_COUNT; i++)
    {
        if (pStreamEnv->stream_info.captureInfo.aseChInfo[i].iso_channel_hdl == GAF_AUDIO_INVALID_ISO_CHANNEL)
        {
            continue;
        }
        channel_shift = 0;
        stereo_channel_support = app_bap_capa_cli_is_peer_support_stereo_channel(
                    pStreamEnv->stream_info.con_lid, BES_BLE_GAF_DIRECTION_SINK);
        audio_allocation_bf = pStreamEnv->stream_info.captureInfo.aseChInfo[i].allocation_bf;
        is_right_channel = ((audio_allocation_bf & (BES_BLE_LOC_SIDE_RIGHT | BES_BLE_LOC_FRONT_RIGHT)) != 0);
        audio_allocation_cnt = app_bap_get_audio_location_l_r_cnt(audio_allocation_bf);

        // Config palyload len  for AUD_CHANNEL_NUM_2
        payload_len_per_channel = payload_size / AUD_CHANNEL_NUM_2;
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
                payload_len_per_channel = payload_size;
            }
        }
        LOG_D("[CAPTURE SEND] p_len:%d stereo supp: %d, allocation_bf: 0x%x, shift :%d",
              payload_len_per_channel, stereo_channel_support, audio_allocation_bf, channel_shift);

        bes_ble_bap_iso_dp_send_data(pStreamEnv->stream_info.captureInfo.aseChInfo[i].ase_handle,
                                pStreamEnv->stream_context.latestCaptureSeqNum,
                                payload + channel_shift, payload_len_per_channel,
                                ref_time);
    }
}

static void gaf_usb_mobile_audio_process_pcm_data(GAF_AUDIO_STREAM_ENV_T *pStreamEnv, uint8_t *ptrBuf, uint32_t length)
{
    uint32_t dmaIrqHappeningTimeUs = 0;
    GAF_AUDIO_STREAM_CONTEXT_T *stream_context = &pStreamEnv->stream_context;
    GAF_AUDIO_STREAM_COMMON_INFO_T *captureInfo = &pStreamEnv->stream_info.captureInfo;
    POSSIBLY_UNUSED CODEC_CORE_INFO_T *coreInfo =
        &pStreamEnv->stream_info.captureInfo.aseChInfo[0].codec_core_info;
    uint8_t *output_buf = stream_context->capture_frame_cache;
    uint32_t frame_len = (uint32_t)(captureInfo->codec_info.frame_size *
                        captureInfo->codec_info.num_channels);

#if defined(GAF_CODEC_CROSS_CORE) || defined(AOB_CODEC_CP)
    if (coreInfo->instance_status < INSTANCE_INITIALIZED) {
        return;
    }
#endif

    int32_t curr_bt_time = (int32_t)bt_syn_ble_bt_time_to_bts(btdrv_syn_get_curr_ticks(), 0);
    int32_t latest_anch_time = (int32_t)gaf_media_common_get_latest_tx_iso_evt_timestamp(pStreamEnv);

    latest_anch_time -= (((int32_t)(latest_anch_time - curr_bt_time)) /
        (int32_t)captureInfo->isoIntervalUs) * (int32_t)captureInfo->isoIntervalUs;

    LOG_D("latest_anch_time %u curr_bt_time %u", latest_anch_time, curr_bt_time);

    if (latest_anch_time <= curr_bt_time)
    {
        latest_anch_time += captureInfo->isoIntervalUs;
    }

    if (latest_anch_time == stream_context->pre_cap_latest_anch_time)
    {
        latest_anch_time += captureInfo->isoIntervalUs;
    }

    dmaIrqHappeningTimeUs = latest_anch_time - captureInfo->isoIntervalUs;
    stream_context->pre_cap_latest_anch_time = latest_anch_time;

    // it's possible the multiple DMA irq triggered message accumulates,
    // so the acuiqred dmaIrqHappeningTimeUs is the last dma irq, which has
    // been handled. For this case, ignore this dma irq message handling
    if ((GAF_CAPTURE_STREAM_STREAMING_TRIGGERED ==stream_context->capture_stream_state) &&
        (dmaIrqHappeningTimeUs == stream_context->lastCaptureDmaIrqTimeUs))
    {
        LOG_W("accumulated irq messages happen!");
        return;
    }
    if (GAF_CAPTURE_STREAM_START_TRIGGERING == stream_context->capture_stream_state)
    {
        uint32_t expectedDmaIrqHappeningTimeUs =
            stream_context->lastCaptureDmaIrqTimeUs +
            (uint32_t)stream_context->captureAverageDmaChunkIntervalUs;

        int32_t gapUs = GAF_AUDIO_CLK_32_BIT_DIFF(dmaIrqHappeningTimeUs, expectedDmaIrqHappeningTimeUs);

        int32_t gapUs_abs = GAF_AUDIO_ABS(gapUs);

        if ((gapUs > 0) && (gapUs_abs > (int32_t)captureInfo->dma_info.dmaChunkIntervalUs/2)) {
            LOG_I("%s, gapUs = %d, dmaChunkIntervalUs = %d", __func__, gapUs, captureInfo->dma_info.dmaChunkIntervalUs);
            return;
        }
        gaf_stream_common_update_capture_stream_state(pStreamEnv, GAF_CAPTURE_STREAM_STREAMING_TRIGGERED);
        gaf_stream_common_clr_trigger(stream_context->playbackTriggerChannel);
        uint32_t latest_iso_bt_time = gaf_media_common_get_latest_tx_iso_evt_timestamp(pStreamEnv);
        stream_context->usSinceLatestAnchorPoint = GAF_AUDIO_CLK_32_BIT_DIFF(dmaIrqHappeningTimeUs, latest_iso_bt_time);
    }

    gaf_stream_common_playback_timestamp_checker(pStreamEnv, dmaIrqHappeningTimeUs);

    LOG_D("length %d encoded_len %d filled timestamp %u", length,
        captureInfo->codec_info.frame_size,dmaIrqHappeningTimeUs);

#if defined(GAF_CODEC_CROSS_CORE) || defined(AOB_CODEC_CP)
    if (stream_context->isUpStreamingStarted)
    {
        // bth fetch encoded data
        gaf_stream_common_fetch_frame(pStreamEnv, output_buf,
            frame_len, dmaIrqHappeningTimeUs, coreInfo->instance_handle);
        // bth send out encoded data
        gaf_usb_mobile_audio_process_pcm_data_send(pStreamEnv,
            output_buf, frame_len, dmaIrqHappeningTimeUs);
    }
    gaf_stream_common_store_pcm(pStreamEnv,
        dmaIrqHappeningTimeUs, ptrBuf, length, frame_len, coreInfo);
#else
    pStreamEnv->func_list->encoder_func_list->encoder_encode_frame_func(0,
        &(captureInfo->codec_info), length, ptrBuf, frame_len, output_buf);

    gaf_usb_mobile_audio_process_pcm_data_send(pStreamEnv, output_buf, frame_len, dmaIrqHappeningTimeUs);
#endif
}

static bool gaf_mobile_usb_is_any_capture_stream_iso_created(GAF_AUDIO_STREAM_ENV_T *pStreamEnv)
{
    GAF_AUDIO_STREAM_COMMON_INFO_T* captureInfo = &(pStreamEnv->stream_info.captureInfo);

    for (uint8_t i = 0; i < GAF_AUDIO_ASE_TOTAL_COUNT; i++)
    {
        if (GAF_AUDIO_INVALID_ISO_CHANNEL != captureInfo->aseChInfo[i].iso_channel_hdl)
        {
            return true;
        }
    }

    return false;
}

void gaf_mobile_usb_processing_received_data(uint8_t* ptrBuf, uint32_t length)
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T*)gaf_mobile_audio_get_media_stream_env();

    if (pStreamEnv && (pStreamEnv->stream_context.capture_stream_state > GAF_CAPTURE_STREAM_INITIALIZED))
    {
        bool isAnyCaptureIsoCreated = gaf_mobile_usb_is_any_capture_stream_iso_created(pStreamEnv);
        if (isAnyCaptureIsoCreated)
        {
            gaf_usb_mobile_audio_process_pcm_data(pStreamEnv, ptrBuf, length);
        }
    }
}

static void gaf_usb_mobile_audio_process_decoded_data(GAF_AUDIO_STREAM_ENV_T *pStreamEnv, uint8_t *ptrBuf, uint32_t length)
{
    LOG_D("%s start, len = %d", __func__,length);
    uint32_t dmaIrqHappeningTimeUs = 0;
    GAF_AUDIO_STREAM_COMMON_INFO_T *playbackInfo = &pStreamEnv->stream_info.playbackInfo;

    int32_t curr_bt_time = (int32_t)bt_syn_ble_bt_time_to_bts(btdrv_syn_get_curr_ticks(), 0);
    int32_t latest_anch_time = (int32_t)gaf_media_common_get_latest_rx_iso_evt_timestamp(pStreamEnv);

    latest_anch_time -= (((int32_t)(latest_anch_time - curr_bt_time)) /
        (int32_t)playbackInfo->isoIntervalUs) * (int32_t)playbackInfo->isoIntervalUs;

    GAF_AUDIO_STREAM_CONTEXT_T *stream_context = &pStreamEnv->stream_context;

    if (latest_anch_time <= curr_bt_time)
    {
        latest_anch_time += playbackInfo->isoIntervalUs;
    }

    if (latest_anch_time == stream_context->pre_ply_latest_anch_time)
    {
        latest_anch_time += playbackInfo->isoIntervalUs;
    }

    dmaIrqHappeningTimeUs = latest_anch_time - playbackInfo->isoIntervalUs;
    stream_context->pre_ply_latest_anch_time = latest_anch_time;

    // it's possible the multiple DMA irq triggered message accumulates,
    // so the acuiqred dmaIrqHappeningTimeUs is the last dma irq, which has
    // been handled. For this case, ignore this dma irq message handling
   if ((pStreamEnv->stream_context.playback_stream_state < GAF_PLAYBACK_STREAM_START_TRIGGERING) ||
        ((GAF_PLAYBACK_STREAM_STREAMING_TRIGGERED ==
       pStreamEnv->stream_context.playback_stream_state) &&
       (dmaIrqHappeningTimeUs ==
       pStreamEnv->stream_context.lastPlaybackDmaIrqTimeUs)))
   {
        memset(ptrBuf, 0, length);
        return;
   }

    gaf_stream_common_updated_expeceted_playback_seq_and_time(pStreamEnv, GAF_AUDIO_DFT_PLAYBACK_LIST_IDX, dmaIrqHappeningTimeUs);

    if (GAF_PLAYBACK_STREAM_START_TRIGGERING ==
        pStreamEnv->stream_context.playback_stream_state)
    {
        gaf_stream_common_update_playback_stream_state(pStreamEnv,
            GAF_PLAYBACK_STREAM_STREAMING_TRIGGERED);
        gaf_stream_common_clr_trigger(pStreamEnv->stream_context.captureTriggerChannel);
        pStreamEnv->stream_context.lastestPlaybackSeqNum[GAF_AUDIO_DFT_PLAYBACK_LIST_IDX]--;
        LOG_I("Update playback seq to 0x%x", pStreamEnv->stream_context.lastestPlaybackSeqNum[GAF_AUDIO_DFT_PLAYBACK_LIST_IDX]);
    }

    int ret = 0;
    uint8_t instance_handle = 0;
    POSSIBLY_UNUSED uint32_t instance_status = 0;
    uint8_t connected_iso_num = 0;
    uint32_t sample_bytes = pStreamEnv->stream_info.playbackInfo.dma_info.bits_depth <= 16 ? 2 : 4;
    uint32_t sample_cnt = length / sample_bytes;
    uint8_t *pcm_cache = pStreamEnv->stream_context.playback_pcm_cache;

    for (uint8_t i = 0; i < GAF_AUDIO_ASE_TOTAL_COUNT; i++)
    {
        if (playbackInfo->aseChInfo[i].iso_channel_hdl == GAF_AUDIO_INVALID_ISO_CHANNEL)
        {
            continue;
        }

        instance_handle = playbackInfo->aseChInfo[i].codec_core_info.instance_handle;
        instance_status = playbackInfo->aseChInfo[i].codec_core_info.instance_status;
#if defined(GAF_CODEC_CROSS_CORE) || defined(AOB_CODEC_CP)
        if (instance_status < INSTANCE_INITIALIZED)
        {
            continue;
        }
        ret = !gaf_stream_common_fetch_pcm(pStreamEnv,
            pcm_cache, playbackInfo->codec_info.pcm_size, instance_handle,
            dmaIrqHappeningTimeUs, GAF_INVALID_SINK_PLAY_DELAY, 0 == connected_iso_num);
#else
        instance_handle = connected_iso_num;
        CC_PLAYBACK_DATA_T *out_frame = (CC_PLAYBACK_DATA_T*)pStreamEnv->stream_context.playback_frame_cache;
        out_frame->data = pStreamEnv->stream_context.playback_frame_cache + sizeof(CC_PLAYBACK_DATA_T);
        uint32_t pk_size = pStreamEnv->stream_info.playbackInfo.codec_info.frame_size * \
                    pStreamEnv->stream_info.playbackInfo.codec_info.num_channels;
        /* use one iso data to do pid */
        gaf_stream_common_get_packet(pStreamEnv, out_frame, instance_handle,
            dmaIrqHappeningTimeUs, pk_size, 0 == connected_iso_num);

        ret = pStreamEnv->func_list->decoder_func_list->decoder_decode_frame_func
                  (instance_handle, &(playbackInfo->codec_info), out_frame->data_len,
                  out_frame->data, pcm_cache, out_frame->isPLC);

        LOG_D("(%d) dec ret:%d frame_len:%d %d length %d", i, ret,
            out_frame->data_len, playbackInfo->aseChInfo[i].iso_channel_hdl, length);
#endif
        if (ret) {
            LOG_E("%s ret:%d", __func__, ret);
            memset(pcm_cache, 0, length);
        }

        connected_iso_num++;
        if (connected_iso_num >= PLAYBACK_INSTANCE_MAX)
        {
            break;
        }
        pcm_cache += playbackInfo->codec_info.pcm_size;
    }

    pcm_cache = pStreamEnv->stream_context.playback_pcm_cache;
#if USB_REC_AUDIO_DUMP
    audio_dump_clear_up();
    audio_dump_add_channel_data(0, pcm_cache, sample_cnt);
    audio_dump_run();
#endif

    /* Merge stream data from multiple devices iso data */
    if (4 == sample_bytes)
    {
        int32_t pcm_data = 0;
        int32_t *buf = (int32_t*)ptrBuf;
        int32_t *cache = (int32_t*)pcm_cache;
        for (uint32_t samples = 0; samples < sample_cnt; samples++)
        {
            pcm_data = 0;
            for (uint32_t index = 0; index < connected_iso_num; index++)
            {
                pcm_data += (cache + sample_cnt * index)[samples];
            }
            buf[samples] = (int32_t)(pcm_data / MAX(connected_iso_num, 1));
        }
    }
    else
    {
        int32_t pcm_data = 0;
        int16_t *buf = (int16_t*)ptrBuf;
        int16_t *cache = (int16_t*)pcm_cache;
        for (uint32_t samples = 0; samples < sample_cnt; samples++)
        {
            pcm_data = 0;
            for (uint32_t index = 0; index < connected_iso_num; index++)
            {
                pcm_data += (cache + sample_cnt * index)[samples];
            }
            buf[samples] = (int16_t)(pcm_data / MAX(connected_iso_num, 1));
        }
    }
}

void gaf_mobile_usb_feed_data(uint8_t* ptrBuf, uint32_t length)
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T*)gaf_mobile_audio_get_media_stream_env();
    if (pStreamEnv && (pStreamEnv->stream_context.playback_stream_state > GAF_PLAYBACK_STREAM_INITIALIZED)) {
        gaf_usb_mobile_audio_process_decoded_data(pStreamEnv, ptrBuf, length);
    }
}

void gaf_mobile_usb_dma_playback_stop(void)
{
    uint8_t POSSIBLY_UNUSED adma_ch = HAL_DMA_CHAN_NONE;
    uint32_t dma_base;
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T*)gaf_mobile_audio_get_media_stream_env();

    if (pStreamEnv) {
        af_stream_dma_tc_irq_disable(AUD_STREAM_ID_1, AUD_STREAM_PLAYBACK);
        adma_ch = af_stream_get_dma_chan(AUD_STREAM_ID_1, AUD_STREAM_PLAYBACK);
        dma_base = af_stream_get_dma_base_addr(AUD_STREAM_ID_1, AUD_STREAM_PLAYBACK);
        if (adma_ch != HAL_DMA_CHAN_NONE)
        {
            bt_drv_reg_op_disable_dma_tc(adma_ch&0xFF, dma_base);
        }

        btdrv_syn_clr_trigger(pStreamEnv->stream_context.playbackTriggerChannel);
    }
}

void gaf_mobile_usb_dma_capture_stop(void)
{
    uint8_t POSSIBLY_UNUSED adma_ch = HAL_DMA_CHAN_NONE;
    uint32_t dma_base;
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv =
        (GAF_AUDIO_STREAM_ENV_T*)gaf_mobile_audio_get_media_stream_env();

    af_stream_dma_tc_irq_disable(AUD_STREAM_ID_1, AUD_STREAM_CAPTURE);
    adma_ch = af_stream_get_dma_chan(AUD_STREAM_ID_1, AUD_STREAM_CAPTURE);
    dma_base = af_stream_get_dma_base_addr(AUD_STREAM_ID_1, AUD_STREAM_CAPTURE);
    if (adma_ch != HAL_DMA_CHAN_NONE)
    {
        bt_drv_reg_op_disable_dma_tc(adma_ch&0xFF, dma_base);
    }

    btdrv_syn_clr_trigger(pStreamEnv->stream_context.captureTriggerChannel);
}

static void gaf_mobile_offload_capture_tuning_handler(void *pStreamEnv_) 
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T* )pStreamEnv_;
    GAF_AUDIO_STREAM_COMMON_INFO_T* pCommonInfo = &(pStreamEnv->stream_info.captureInfo);
    uint32_t dmaIrqHappeningTimeUs = 0;
    uint8_t adma_ch = HAL_DMA_CHAN_NONE;
    uint32_t dma_base;
    uint32_t btclk; //hal slot -- 312.5us
    uint16_t btcnt; //hal microsecond -- 0.5 us
    uint32_t lock = int_lock();
    adma_ch = af_stream_get_dma_chan(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
    dma_base = af_stream_get_dma_base_addr(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
    if (adma_ch != HAL_DMA_CHAN_NONE)
    {
        bt_drv_reg_op_dma_tc_clkcnt_get_by_ch(&btclk, &btcnt,adma_ch&0xFF, dma_base);
        dmaIrqHappeningTimeUs = bt_syn_ble_bt_time_to_bts(btclk, btcnt);
    }
    uint32_t iso_event_timestamp = gaf_media_common_get_latest_tx_iso_evt_timestamp(pStreamEnv);
    int32_t diff_du = dmaIrqHappeningTimeUs - pStreamEnv->stream_context.usb_sync.latest_usb_time;
    int32_t du_diff_diff = 0;

    for (int cnt = 0; cnt < 10; cnt++)
    {
        if (iso_event_timestamp >= dmaIrqHappeningTimeUs)
        {
            break;
        }
        // TRACE(0, "iso_event_timestamp less than dmaIrqHappeningTimeUs");
        iso_event_timestamp += (uint32_t)(pCommonInfo->dma_info.frame_ms * 1000);
    }
    uint32_t diff_id = iso_event_timestamp - dmaIrqHappeningTimeUs;
    if (diff_id > (uint32_t)(pCommonInfo->dma_info.frame_ms * 1000))
    {
        TRACE(0, "diff_id value anomaly");
        diff_id -= (uint32_t)(pCommonInfo->dma_info.frame_ms * 1000);
    }
    if (pStreamEnv->stream_context.usb_sync.ref_diff_id == 0)
    {
        pStreamEnv->stream_context.usb_sync.ref_diff_id = diff_id;
    }

    if (pStreamEnv->stream_context.usb_sync.ref_diff_ud)
    {
        du_diff_diff = diff_du - pStreamEnv->stream_context.usb_sync.ref_diff_ud;
        // tunning isp anchor pinter will 08 disconnect in 1607
        if (pStreamEnv->stream_context.usb_sync.last_du_diff_diff < du_diff_diff && du_diff_diff > 0 )
        {
            bt_drv_reg_op_turning_cig_anchor(-du_diff_diff/2);
        }
        else if (pStreamEnv->stream_context.usb_sync.last_du_diff_diff > du_diff_diff && du_diff_diff < 0 )
        {
            bt_drv_reg_op_turning_cig_anchor(-du_diff_diff/2);
        }
        else
        {
            bt_drv_reg_op_turning_cig_anchor(du_diff_diff/2);
        }
    }

    if (!pStreamEnv->stream_context.usb_sync.ref_diff_ud && pStreamEnv->stream_context.usb_sync.dma_cnt > 100 && diff_du < (int32_t)(pCommonInfo->dma_info.frame_ms * 1000))
    {
        //the diff between dma ts and usb ts should be 100 ~ 900 us
        diff_du = CLAMP(diff_du, 100, 900);
        pStreamEnv->stream_context.usb_sync.ref_diff_ud = diff_du;
    }
    int_unlock(lock);

    if (pStreamEnv->stream_context.usb_sync.ref_diff_ud > 6000 || pStreamEnv->stream_context.usb_sync.ref_diff_ud < -6000)
    {
        TRACE(0, "warning dmaIrqHappeningTimeUs %u latest_usb_time %u", dmaIrqHappeningTimeUs, pStreamEnv->stream_context.usb_sync.latest_usb_time);
    }
    gaf_usb_media_pid_adjust(AUD_STREAM_CAPTURE, &pStreamEnv->stream_context.capture_pid_env, pStreamEnv->stream_context.usb_sync.ref_diff_id, diff_id);
    LOG_I("diff_id %d du_diff_diff %d ref_diff_id %d ref_diff_ud %d", diff_id, du_diff_diff, pStreamEnv->stream_context.usb_sync.ref_diff_id, pStreamEnv->stream_context.usb_sync.ref_diff_ud);
    pStreamEnv->stream_context.usb_sync.dma_cnt++;
    pStreamEnv->stream_context.usb_sync.last_du_diff_diff = du_diff_diff;
}


uint32_t gaf_mobile_usb_audio_capture_dma_irq_handler(uint8_t* ptrBuf, uint32_t length)
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv =
        (GAF_AUDIO_STREAM_ENV_T*)gaf_mobile_audio_get_media_stream_env();

    GAF_AUDIO_STREAM_COMMON_INFO_T *captureInfo = &pStreamEnv->stream_info.captureInfo;
    POSSIBLY_UNUSED CODEC_CORE_INFO_T *coreInfo =
        &pStreamEnv->stream_info.captureInfo.aseChInfo[0].codec_core_info;
    uint32_t frame_len = (uint32_t)(captureInfo->codec_info.frame_size *
                        captureInfo->codec_info.num_channels);

#if defined (GAF_CODEC_CROSS_CORE) || defined (AOB_CODEC_CP)
    if (coreInfo->instance_status < INSTANCE_INITIALIZED) {
        LOG_E("%s instance_status %d", __func__, coreInfo->instance_status);
        return length;
    }
#endif

    uint8_t input_type = 0, output_type = 0;
    app_dongle_in_out_type_get(&input_type, &output_type);

    if (input_type == BTA_DONGLE_DATA_PATH_USB) {
        gaf_source_stream_data_tx_read(ptrBuf, length);
        gaf_mobile_offload_capture_tuning_handler(pStreamEnv);
    }
    else
    {
        int16_t *pcm = (int16_t *)ptrBuf;
        uint32_t sample_cnt = length / (pStreamEnv->stream_info.captureInfo.dma_info.bits_depth <= 16 ? 2 : 4); // 16bit = 2 bytes
        uint32_t frame_cnt = sample_cnt / captureInfo->codec_info.num_channels; // 2 channel
        for (uint32_t i = 0; i < frame_cnt; i++)
        {
            int16_t l = pcm[2 * i];
            int16_t r = pcm[2 * i + 1];

            int32_t mix = ((int32_t)l + (int32_t)r) >> 1;

            pcm[2 * i]     = (int16_t)mix;
            pcm[2 * i + 1] = (int16_t)mix;
        }

    }

#if defined (GAF_CODEC_CROSS_CORE) || defined (AOB_CODEC_CP)
    gaf_stream_common_store_pcm(pStreamEnv,
        gaf_media_sync_get_curr_time(), ptrBuf, length, frame_len, coreInfo);
#else
    uint8_t *output_buf = pStreamEnv->stream_context.capture_frame_cache;
    pStreamEnv->func_list->encoder_func_list->encoder_encode_frame_func(0,
        &(captureInfo->codec_info), length, ptrBuf, frame_len, output_buf);

    gaf_usb_mobile_audio_process_pcm_data_send(pStreamEnv, output_buf, frame_len, 0);

#endif
    return length;
}

void gaf_usb_sync_clk_init(GAF_AUDIO_STREAM_USB_SYNC_T* usb_sync)
{
    memset(usb_sync, 0, sizeof(GAF_AUDIO_STREAM_USB_SYNC_T));
}

int gaf_mobile_usb_audio_media_stream_start_handler(void* _pStreamEnv)
{
    LOG_I("%s", __func__);

    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T *)_pStreamEnv;

    app_ble_is_usb_playback_on = true;
    if (GAF_CAPTURE_STREAM_INITIALIZING == pStreamEnv->stream_context.capture_stream_state)
    {
        pStreamEnv->func_list->stream_func_list.capture_init_stream_buf_func(pStreamEnv);
        if (pStreamEnv->func_list->encoder_func_list)
        {
            pStreamEnv->func_list->encoder_func_list->encoder_init_func(
                        0, &(pStreamEnv->stream_info.captureInfo.codec_info));
        }
#if defined(GAF_CODEC_CROSS_CORE) || defined(AOB_CODEC_CP)
        gaf_capture_encoder_init(pStreamEnv);
#endif
        pStreamEnv->stream_context.captureTriggerChannel = USB_BLE_AUDIO_PLAYBACK_TRIGGER_CHANNEL;
        LOG_D("%s captureTriggerChannel %d", __func__,pStreamEnv->stream_context.captureTriggerChannel);

        gaf_usb_media_pid_init(&(pStreamEnv->stream_context.capture_pid_env));
        gaf_usb_sync_clk_init(&(pStreamEnv->stream_context.usb_sync));
        // capture pcm data from usb audio
        gaf_stream_common_update_capture_stream_state(pStreamEnv, GAF_CAPTURE_STREAM_INITIALIZED);
        gaf_source_capture_start(pStreamEnv);
        gaf_stream_common_update_capture_stream_state(pStreamEnv, GAF_CAPTURE_STREAM_START_TRIGGERING);

        app_sysfreq_req(APP_SYSFREQ_USER_AOB_CAPTURE, APP_SYSFREQ_208M);
        LOG_D("%s end", __func__);

        return 0;
    }

    return -1;
}

int gaf_mobile_usb_audio_media_stream_stop_handler(void* _pStreamEnv)
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T *)_pStreamEnv;
    bool POSSIBLY_UNUSED isRetrigger = pStreamEnv->stream_context.capture_retrigger_onprocess;
    gaf_stream_common_update_capture_stream_state(pStreamEnv, GAF_CAPTURE_STREAM_IDLE);

#if defined (GAF_CODEC_CROSS_CORE) || defined (AOB_CODEC_CP)
    gaf_capture_encoder_deinit(pStreamEnv, isRetrigger);
#endif
    gaf_source_stop(AUD_STREAM_PLAYBACK);
    if (pStreamEnv->func_list->encoder_func_list)
    {
        pStreamEnv->func_list->encoder_func_list->encoder_deinit_func(0);
    }
    pStreamEnv->func_list->stream_func_list.capture_deinit_stream_buf_func(pStreamEnv);

    app_ble_is_usb_playback_on = false;
    app_sysfreq_req(APP_SYSFREQ_USER_AOB_CAPTURE, APP_SYSFREQ_32K);
    return 0;
}

int gaf_mobile_usb_audio_capture_start_handler(void* _pStreamEnv)
{
    LOG_I("%s", __func__);
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T *)_pStreamEnv;

    app_ble_is_usb_capture_on = true;
    pStreamEnv->func_list->stream_func_list.playback_init_stream_buf_func(pStreamEnv);
    if (pStreamEnv->func_list->decoder_func_list)
    {
        for (uint32_t instance_handle = 0; instance_handle < PLAYBACK_INSTANCE_MAX; instance_handle++)
        {
            pStreamEnv->func_list->decoder_func_list->decoder_init_func(
                instance_handle, &(pStreamEnv->stream_info.playbackInfo.codec_info));
        }
    }
    pStreamEnv->stream_context.playbackTriggerChannel = USB_BLE_AUDIO_CAPTURE_TRIGGER_CHANNEL;

    gaf_media_pid_init(&(pStreamEnv->stream_context.capture_pid_env));
    gaf_stream_common_update_playback_stream_state(pStreamEnv, GAF_PLAYBACK_STREAM_INITIALIZED);

    gaf_source_playback_start(pStreamEnv);

    app_sysfreq_req(APP_SYSFREQ_USER_AOB_PLAYBACK, APP_SYSFREQ_208M);

    bes_ble_bap_dp_itf_data_come_callback_register((void *)gaf_mobile_audio_receive_data);

#if defined(USB_REC_AUDIO_DUMP)
    audio_dump_init(480, sizeof(short), 2);
#endif

    return 0;
}

int gaf_mobile_usb_audio_capture_stop_handler(void* _pStreamEnv)
{
    LOG_I("%s", __func__);
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T *)_pStreamEnv;
    gaf_stream_common_update_playback_stream_state(pStreamEnv, GAF_PLAYBACK_STREAM_IDLE);
    gaf_source_stop(AUD_STREAM_CAPTURE);

    if (pStreamEnv->func_list->decoder_func_list)
    {
        pStreamEnv->func_list->decoder_func_list->decoder_deinit_func(0);
    }
    pStreamEnv->func_list->stream_func_list.playback_deinit_stream_buf_func(pStreamEnv);
    bes_ble_bap_dp_itf_data_come_callback_register(NULL);

    app_ble_is_usb_capture_on = false;
    app_sysfreq_req(APP_SYSFREQ_USER_AOB_PLAYBACK, APP_SYSFREQ_32K);
    return 0;
}
