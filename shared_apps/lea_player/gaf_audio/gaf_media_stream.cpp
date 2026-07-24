/**
 *
 * @copyright Copyright (c) 2015-2021 BES Technic.
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
 */

/*****************************header include********************************/
#if BLE_AUDIO_ENABLED
#include "bluetooth_bt_api.h"
#include "hal_dma.h"
#include "hal_trace.h"
#include "hal_codec.h"
#include "app_trace_rx.h"
#include "app_bt_func.h"
#include "app_utils.h"
#include "plat_types.h"
#include "cqueue.h"
#include "heap_api.h"
#include "audio_dump.h"
#include "audioflinger.h"
#include "bt_drv_reg_op.h"
#include "bt_drv_interface.h"
#include "app_audio.h"
#include "app_overlay.h"
#include "list.h"
#include "lc3_process.h"
#include "besbt_string.h"
#include "app_bt_stream.h"

#include "gaf_media_sync.h"
#include "gaf_media_common.h"
#include "gaf_codec_lc3.h"
#include "gaf_media_stream.h"
#include "gaf_bis_media_stream.h"
#include "gaf_stream_process.h"
#include "gaf_stream_resample_process.h"
#include "gaf_stream_dbg.h"
#include "app_audio_active_device_manager.h"
#include "app_bt_media_manager.h"
#include "gaf_audio_path.h"
#include "app_bap.h"
#ifdef LEA_LHDC
#include "gaf_codec_lhdc.h"
#endif

#if (BES_AHP)
#include "app_gaf_define.h"
#include "gaf_non_codec_stream.h"
#endif
#include "bta_bt_api.h"
#include "nvrecord_ble.h"
#include "app_anc.h"
#include "app_anc_assist.h"
#include "anc_assist_mic.h"
#include "anc_assist_resample.h"
#include "iir_resample.h"
#if defined (VOICE_ASSIST_FIR_LMS_ENABLED) || defined (VOICE_ASSIST_FF_FIR_LMS)
#include "app_voice_assist_fir_lms.h"
#endif

#include "bes_aob_api.h"
#include "app_bt_sync.h"

#include "app_mcpp.h"
#ifdef GAF_OFFLOAD_ENABLE
#include "cp_accel.h"
#include "codec_int.h"
#include "gaf_cp_stream.h"
#include "gaf_offload_dsp_stream.h"
#include "gaf_offload_stream_cfg.h"
#include "app_gaf_custom_api.h"
#endif
#ifdef CODEC_DAC_MULTI_VOLUME_TABLE
#include "hal_codec.h"
#endif

#ifdef SPEECH_BONE_SENSOR
#include "speech_bone_sensor.h"
#define SPEECH_BS_SINGLE_CHANNEL    (SPEECH_BS_CHANNEL_Z)
static bool g_capture_vpu_enabled = false;
#ifdef GAF_ENCODER_CROSS_CORE_USE_M55
static int32_t *g_capture_combo_vpu_pcm_buf = NULL;
#endif
#endif

#ifdef MIX_AUDIO_PROMPT_WITH_A2DP_MEDIA_ENABLED
#include "audio_prompt_sbc.h"
#endif

#ifdef GAF_DSP
#include "mcu_dsp_hifi4_app.h"
#endif


// #define GAF_STREAM_PLAYBACK_AUDIO_DUMP      (16)
#ifdef BLE_AUDIO_SPEECH_ALGORITHM_SUPPORT
#define STREAM_PROCESS_ENABLE
#endif

/*********************external function declaration*************************/

/************************private type defination****************************/
#ifndef BT_AUDIO_CACHE_2_UNCACHE
#define BT_AUDIO_CACHE_2_UNCACHE(addr) \
    ((unsigned char *)((unsigned int)addr & ~(0x04000000)))
#endif

#define APP_BAP_MAX_ASCS_NB_ASE_CFG               (APP_BAP_MAX_ASCS_NB_ASE_CHAR * BLE_AUDIO_CONNECTION_CNT)
#define APP_BAP_MAX_RECORD_CNT                    (APP_BAP_MAX_ASCS_NB_ASE_CFG + 1)

/**********************private function declaration*************************/

/************************private variable defination************************/
#ifdef AOB_UC_TEST
uint8_t buds_freq;
#endif
/****************************GAF CUSTOM*****************************/
static uint32_t gaf_stream_get_curr_dma_irq_happening_time(bool is_pre_get);
static const GAF_STREAM_COMMON_CUSTOM_DATA_HANDLER_FUNC_T *gaf_uc_srv_custom_data_callback = NULL;

static GAF_AUDIO_STREAM_ENV_T gaf_audio_stream_env[GAF_MAXIMUM_CONNECTION_COUNT];
static GAF_MEDIA_DWELLING_INFO_T gaf_cis_media_dwelling_info[GAF_MAXIMUM_CONNECTION_COUNT];
static uint32_t POSSIBLY_UNUSED g_capture_ch_num = 4;
static uint32_t POSSIBLY_UNUSED g_speech_algo_ch_num = 4;
static uint32_t POSSIBLY_UNUSED g_capture_frame_len = 16 * 10;
static uint32_t POSSIBLY_UNUSED g_capture_sample_rate = 16000;
static GAF_STREAM_PLAYBACK_STATUS_E gaf_stream_playback_status = GAF_STREAM_PLAYBACK_STATUS_IDLE;

static void gaf_audio_start_sink_trigger_by_cis_cig_delay(GAF_AUDIO_STREAM_ENV_T *pStreamEnv, const bes_ble_bap_ascs_ase_t *p_ase_info);

static uint32_t POSSIBLY_UNUSED g_playback_sample_rate = 16000;

#if defined(ANC_ASSIST_LE_AUDIO_SUPPORT)
#define RESAMPLE_HEAP_BUFF_SIZE         (1024 * 24)
static uint8_t *g_anc_assist_interval_buf = NULL;
static uint8_t *g_anc_assist_resample_buf = NULL;
static uint32_t g_anc_assist_resample_buf_used = 0;
#if defined (VOICE_ASSIST_FIR_LMS_ENABLED) || defined (VOICE_ASSIST_FF_FIR_LMS)
static IirResampleState *g_anc_assist_iir_resample_st = NULL;
#endif
#endif

#ifdef GAF_STREAM_PLAYBACK_AUDIO_DUMP
static uint32_t g_stream_pcm_dump_frame_len = 0;
static uint32_t g_stream_pcm_dump_channel_num = 0;
static uint32_t g_stream_pcm_dump_sample_bytes = 0;
#endif

osMutexDef(gaf_playback_status_mutex);

/*
 * GAF_AUDIO_STREAM_CONTEXT_TYPE_MIN           = 0
 * GAF_AUDIO_STREAM_CONTEXT_TYPE_MEDIA         = GAF_AUDIO_STREAM_CONTEXT_TYPE_MIN,
 * GAF_AUDIO_STREAM_CONTEXT_TYPE_CALL,
 * GAF_AUDIO_STREAM_CONTEXT_TYPE_AI,
 * GAF_AUDIO_STREAM_CONTEXT_TYPE_FLEXIBLE,
*/
const static GAF_MEDIA_STREAM_TYPE_OPERATION_RULE_T gaf_cis_stream_type_op_rule[GAF_AUDIO_CONTEXT_NUM_MAX] =
{
    { GAF_AUDIO_STREAM_TYPE_PLAYBACK, GAF_AUDIO_TRIGGER_BY_PLAYBACK_STREAM, 1, 0},
#ifdef AOB_MOBILE_ENABLED
    { GAF_AUDIO_STREAM_TYPE_PLAYBACK|GAF_AUDIO_STREAM_TYPE_CAPTURE, GAF_AUDIO_TRIGGER_BY_PLAYBACK_STREAM, 1, 1},
    { GAF_AUDIO_STREAM_TYPE_CAPTURE, GAF_AUDIO_TRIGGER_BY_CAPTURE_STREAM, 0, 2},
#endif
    { GAF_AUDIO_STREAM_TYPE_FLEXIBLE, GAF_AUDIO_TRIGGER_BY_ISOLATE_STREAM, 1, 1},
};

#ifdef MIX_AUDIO_PROMPT_WITH_A2DP_MEDIA_ENABLED
static uint8_t* promptTmpSourcePcmDataBuf = NULL;
static uint8_t* promptTmpTargetPcmDataBuf = NULL;
static uint8_t* promptPcmDataBuf = NULL;
static uint8_t* promptResamplerBuf = NULL;
#endif

#define GAF_MEDIA_STREAM_POST_START_FREQ_BOOST_TIMEOUT_VALUE  (1500)

#if defined (GAF_CODEC_CROSS_CORE) || defined (AOB_CODEC_CP)
#define GAF_STREAM_DELAY_GET_PCM_PADDING_US                   (GAF_AUDIO_CONTROLLER_2_HOST_LATENCY_US)
#else
/// According to mips and thread prio
#define GAF_STREAM_DELAY_GET_PCM_PADDING_US                   (GAF_AUDIO_LOACL_DECODER_LATENCY_US)
#endif

/****************************function defination****************************/
#ifdef AF_DELAY_HANDLE_DMA_IRQ_SUPPORT
static uint32_t gaf_audio_stream_delay_get_pcm_ticks_get(uint32_t dummy)
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T *)dummy;
    uint32_t curr_dma_ts_us = gaf_stream_get_curr_dma_irq_happening_time(true);
    uint32_t curr_cis_ts_us = gaf_media_sync_get_curr_time();
    uint32_t dma_interval_us = pStreamEnv->stream_info.playbackInfo.dma_info.dmaChunkIntervalUs;

    if (curr_dma_ts_us + dma_interval_us - curr_cis_ts_us <= GAF_STREAM_DELAY_GET_PCM_PADDING_US)
    {
        return 0;
    }

    return US_TO_TICKS((dma_interval_us - (curr_cis_ts_us - curr_dma_ts_us) - GAF_STREAM_DELAY_GET_PCM_PADDING_US));
}
#endif
static void gaf_audio_add_ase_into_playback_ase_list(uint8_t con_lid, uint8_t ase_lid)
{
    LOG_I("set con_lid %d playback ase lid %d", con_lid, ase_lid);
    uint8_t idx = 0;

    if (GAF_AUDIO_USED_ASE_PER_DIR_MAX_CNT ==
        gaf_stream_common_get_ase_idx_in_ase_lid_list(
            &gaf_cis_media_dwelling_info[con_lid].playback_ase_id[0], ase_lid))
    {
        idx = gaf_stream_common_get_valid_idx_in_ase_lid_list(
                &gaf_cis_media_dwelling_info[con_lid].playback_ase_id[0]);
        if (idx == GAF_AUDIO_USED_ASE_PER_DIR_MAX_CNT)
        {
            LOG_E("no more space for adding playback ase lid: %d into list!!!", ase_lid);
        }
        else
        {
            gaf_cis_media_dwelling_info[con_lid].playback_ase_id[idx] = ase_lid;
        }
    }
    else
    {
        LOG_E("ase_lid: %d already in playback ase list!!!", ase_lid);
    }
}

static void gaf_audio_remove_ase_from_playback_ase_list(uint8_t con_lid, uint8_t ase_lid)
{
    LOG_I("remove con_lid %d playback ase lid %d", con_lid, ase_lid);

    uint8_t idx = gaf_stream_common_get_ase_idx_in_ase_lid_list(
                &gaf_cis_media_dwelling_info[con_lid].playback_ase_id[0], ase_lid);
    if (idx == GAF_AUDIO_USED_ASE_PER_DIR_MAX_CNT)
    {
        LOG_E("ase lid: %d is not in the list!!!", ase_lid);
        return;
    }
    gaf_cis_media_dwelling_info[con_lid].playback_ase_id[idx] = GAF_INVALID_ASE_INDEX;
}

static void gaf_audio_add_ase_into_capture_ase_list(uint8_t con_lid, uint8_t ase_lid)
{
    LOG_I("set con_lid %d capture ase lid %d", con_lid, ase_lid);
    uint8_t idx = 0;

    if (GAF_AUDIO_USED_ASE_PER_DIR_MAX_CNT ==
        gaf_stream_common_get_ase_idx_in_ase_lid_list(
            &gaf_cis_media_dwelling_info[con_lid].capture_ase_id[0], ase_lid))
    {
        idx = gaf_stream_common_get_valid_idx_in_ase_lid_list(
                &gaf_cis_media_dwelling_info[con_lid].capture_ase_id[0]);
        if (idx == GAF_AUDIO_USED_ASE_PER_DIR_MAX_CNT)
        {
            LOG_E("no more space for adding capture ase lid: %d into list!!!", ase_lid);
        }
        else
        {
            gaf_cis_media_dwelling_info[con_lid].capture_ase_id[idx] = ase_lid;
        }
    }
    else
    {
        LOG_E("ase_lid: %d already in capture ase list!!!", ase_lid);
    }
}

static void gaf_audio_remove_ase_from_capture_ase_list(uint8_t con_lid, uint8_t ase_lid)
{
    LOG_I("remove con_lid %d capture ase lid %d", con_lid, ase_lid);

    uint8_t idx = gaf_stream_common_get_ase_idx_in_ase_lid_list(
                &gaf_cis_media_dwelling_info[con_lid].capture_ase_id[0], ase_lid);
    if (idx == GAF_AUDIO_USED_ASE_PER_DIR_MAX_CNT)
    {
        LOG_E("ase lid: %d is not in the list!!!", ase_lid);
        return;
    }
    gaf_cis_media_dwelling_info[con_lid].capture_ase_id[idx] = GAF_INVALID_ASE_INDEX;
}

static uint8_t* gaf_audio_get_playback_ase_index_list(uint8_t con_lid)
{
    LOG_D("get playback con_lid %d", con_lid);
    return &gaf_cis_media_dwelling_info[con_lid].playback_ase_id[0];
}

static uint8_t* gaf_audio_get_capture_ase_index_list(uint8_t con_lid)
{
    LOG_D("get capture con_lid %d", con_lid);
    return &gaf_cis_media_dwelling_info[con_lid].capture_ase_id[0];
}

static void gaf_audio_clear_playback_ase_index_list(uint8_t con_lid)
{
    LOG_D("clear playback ase list con_lid %d", con_lid);
    memset_s(&gaf_cis_media_dwelling_info[con_lid].playback_ase_id[0],
            GAF_AUDIO_USED_ASE_PER_DIR_MAX_CNT*sizeof(uint8_t), GAF_INVALID_ASE_INDEX,
            GAF_AUDIO_USED_ASE_PER_DIR_MAX_CNT*sizeof(uint8_t));
}

static void gaf_audio_clear_capture_ase_index_list(uint8_t con_lid)
{
    LOG_D("clear playback ase list con_lid %d", con_lid);
    memset_s(&gaf_cis_media_dwelling_info[con_lid].capture_ase_id[0],
            GAF_AUDIO_USED_ASE_PER_DIR_MAX_CNT*sizeof(uint8_t), GAF_INVALID_ASE_INDEX,
            GAF_AUDIO_USED_ASE_PER_DIR_MAX_CNT*sizeof(uint8_t));
}

static void gaf_audio_dwelling_info_list_init(void)
{
    memset_s((void *)&gaf_cis_media_dwelling_info, sizeof(gaf_cis_media_dwelling_info),
             0, sizeof(gaf_cis_media_dwelling_info));
}

static GAF_AUDIO_STREAM_ENV_T* gaf_audio_get_stream_from_channel_index(uint8_t channel)
{
    for (uint32_t con_lid = 0; con_lid < GAF_MAXIMUM_CONNECTION_COUNT; con_lid++)
    {
        for (uint8_t i = 0; i < GAF_AUDIO_ASE_TOTAL_COUNT; i++)
        {
            if ((gaf_audio_stream_env[con_lid].stream_info.playbackInfo.aseChInfo[i].iso_channel_hdl == channel) ||
                (gaf_audio_stream_env[con_lid].stream_info.captureInfo.aseChInfo[i].iso_channel_hdl == channel))
            {
                return &gaf_audio_stream_env[con_lid];
            }
        }
    }

    LOG_D("Receive a CIS packet from channel: %d before cooresponding stream is ready!", channel);
    return NULL;
}

#ifdef IS_BLE_AUDIO_DEBUG_INFO_COLLECTOR_ENABLED
GAF_AUDIO_STREAM_ENV_T* gaf_audio_get_stream_env_from_con_lid(uint8_t con_lid)
{
    for (uint32_t type = GAF_AUDIO_STREAM_CONTEXT_TYPE_MEDIA; type < GAF_AUDIO_CONTEXT_NUM_MAX; type++)
    {
        if (gaf_audio_stream_env[type].stream_info.con_lid == con_lid)
        {
            return &gaf_audio_stream_env[type];
        }
    }

    LOG_W("(d%d)%s fail invaild", con_lid, __func__);
    return NULL;
}

static gaf_stream_dma_irq_cb capture_dma_irq_info_record_handler = NULL;
void gaf_stream_capture_register_dma_irq_cb(gaf_stream_dma_irq_cb func)
{
    capture_dma_irq_info_record_handler = func;
}
#endif

POSSIBLY_UNUSED static void gaf_media_stop_all_iso_dp_rx(GAF_AUDIO_STREAM_ENV_T* pStreamEnv)
{
    uint8_t con_lid = 0;
    uint8_t ase_lid_list[GAF_AUDIO_ASE_TOTAL_COUNT] = {0};
    const bes_ble_bap_ascs_ase_t *p_bap_ase_info = NULL;

    for (con_lid = 0; con_lid < AOB_COMMON_MOBILE_CONNECTION_MAX; con_lid++)
    {
        if (bes_ble_bap_ascs_get_streaming_ase_lid_list(con_lid, ase_lid_list) != 0)
        {
            p_bap_ase_info = bes_ble_get_ascs_ase_info(ase_lid_list[0]);

            if (p_bap_ase_info == NULL)
            {
                continue;
            }

            bes_ble_bap_dp_rx_iso_stop(p_bap_ase_info->cis_hdl);
        }
    }
}

POSSIBLY_UNUSED static void gaf_media_stop_all_iso_dp_tx(GAF_AUDIO_STREAM_ENV_T* pStreamEnv)
{
    uint8_t con_lid = 0;
    uint8_t ase_lid_list[GAF_AUDIO_ASE_TOTAL_COUNT] = {0};
    const bes_ble_bap_ascs_ase_t *p_bap_ase_info = NULL;

    for (con_lid = 0; con_lid < AOB_COMMON_MOBILE_CONNECTION_MAX; con_lid++)
    {
        if (bes_ble_bap_ascs_get_streaming_ase_lid_list(con_lid, ase_lid_list) != 0)
        {
            p_bap_ase_info = bes_ble_get_ascs_ase_info(ase_lid_list[0]);

            if (p_bap_ase_info == NULL)
            {
                continue;
            }

            bes_ble_bap_dp_tx_iso_stop(p_bap_ase_info->cis_hdl);
        }
    }
}

static void gaf_stream_receive_data(uint16_t conhdl, GAF_ISO_PKT_STATUS_E pkt_status)
{
    gaf_media_data_t *p_media_data = NULL;
    CC_PLAYBACK_DATA_T frame = {0};
    uint32_t pk_size;
    // map to gaf stream context
    uint8_t channel = BLE_ISOHDL_TO_ACTID(conhdl);
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = gaf_audio_get_stream_from_channel_index(channel);
    uint8_t ase_chan_lid = gaf_media_common_get_ase_chan_lid_from_iso_channel(
                                            pStreamEnv, BES_BLE_GAF_DIRECTION_SINK, channel);

    if (ase_chan_lid >= GAF_AUDIO_ASE_TOTAL_COUNT)
    {
        while ((p_media_data = (gaf_media_data_t *)bes_ble_bap_dp_itf_get_rx_data(conhdl, NULL)))
        {
            gaf_stream_data_free(p_media_data);
        }
        return;
    }

    CODEC_CORE_INFO_T *coreInfo = &pStreamEnv->stream_info.playbackInfo.aseChInfo[ase_chan_lid].codec_core_info;
#if defined (GAF_CODEC_CROSS_CORE) || defined (AOB_CODEC_CP)
    if (coreInfo->instance_status < INSTANCE_INITIALIZED)
    {
        while ((p_media_data = (gaf_media_data_t *)bes_ble_bap_dp_itf_get_rx_data(conhdl, NULL)))
        {
            gaf_stream_data_free(p_media_data);
        }
        LOG_E("instance_handle 0x%x status %d ", coreInfo->instance_handle, coreInfo->instance_status);
        return;
    }
#else
    coreInfo->instance_handle = 0;
#ifdef BLE_AUDIO_STEREO_CHAN_OVER_CIS_CNT
    coreInfo->instance_handle = ase_chan_lid;
#endif
#endif

    while ((p_media_data = (gaf_media_data_t *)bes_ble_bap_dp_itf_get_rx_data(conhdl, NULL)))
    {
        pk_size = pStreamEnv->stream_info.playbackInfo.codec_info.frame_size * \
                    pStreamEnv->stream_info.playbackInfo.codec_info.num_channels;
        if (p_media_data->data_len > pk_size)
        {
            LOG_E("receive len %d err", p_media_data->data_len);
            gaf_stream_data_free(p_media_data);
            continue;
        }

        frame.time_stamp = p_media_data->time_stamp;
        frame.seq_nb = p_media_data->pkt_seq_nb;
        frame.data_len = p_media_data->data_len;
        frame.data = p_media_data->sdu_data;
        frame.isPLC = ((GAF_ISO_PKT_STATUS_VALID != p_media_data->pkt_status) || (0 == p_media_data->data_len));
        if (pStreamEnv->stream_context.playback_stream_state >=  GAF_PLAYBACK_STREAM_START_TRIGGERING)
        {
            if (frame.isPLC) {
                LOG_I("PLC seq:0x%02x ts:%u curr:%u pk status:%d data len:%d",
                    p_media_data->pkt_seq_nb, p_media_data->time_stamp, gaf_media_sync_get_curr_time(), p_media_data->pkt_status, p_media_data->data_len);
            }
            /// gaf custom, may be a watch point to check some pattern in encoded packet
            if (gaf_uc_srv_custom_data_callback->encoded_packet_recv_cb)
            {
                gaf_uc_srv_custom_data_callback->encoded_packet_recv_cb(p_media_data);
            }

            gaf_stream_common_store_packet(pStreamEnv, &frame, coreInfo);
        }

        if (GAF_PLAYBACK_STREAM_INITIALIZED == pStreamEnv->stream_context.playback_stream_state)
        {
            gaf_audio_start_sink_trigger_by_cis_cig_delay(pStreamEnv, NULL);
        }

        gaf_stream_data_free(p_media_data);
    }
}

static int gaf_audio_flexible_playback_stream_start_handler(void* _pStreamEnv)
{
    LOG_I("%s start", __func__);

    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T *)_pStreamEnv;

    uint8_t rt_vol = bes_ble_arc_vol_get_real_time_volume(pStreamEnv->stream_info.con_lid);
    uint8_t vol = bes_ble_arc_convert_le_vol_to_local_vol(rt_vol);

    if (GAF_PLAYBACK_STREAM_INITIALIZING == pStreamEnv->stream_context.playback_stream_state)
    {
        GAF_AUDIO_STREAM_COMMON_INFO_T *pCommonInfo = &pStreamEnv->stream_info.playbackInfo;

        g_playback_sample_rate = get_gaf_stream_playback_sample_rate(pStreamEnv->stream_info.bap_contextType,
            pCommonInfo->dma_info.sample_rate);
        if (g_playback_sample_rate > pCommonInfo->dma_info.sample_rate) {
            uint32_t sample_rate_div = g_playback_sample_rate / pCommonInfo->dma_info.sample_rate;
            pCommonInfo->dma_info.dmaChunkSize *= sample_rate_div;
        } else {
            uint32_t sample_rate_div = pCommonInfo->dma_info.sample_rate / g_playback_sample_rate;
            pCommonInfo->dma_info.dmaChunkSize /= sample_rate_div;
        }

        pStreamEnv->func_list->stream_func_list.playback_init_stream_buf_func(pStreamEnv);

#ifdef STREAM_PROCESS_ENABLE_USE_CC_PLAYBACK
        pStreamEnv->stream_info.rx_algo_cfg.channel_num = pCommonInfo->dma_info.num_channels;
        pStreamEnv->stream_info.rx_algo_cfg.sample_rate = g_playback_sample_rate;
        pStreamEnv->stream_info.rx_algo_cfg.frame_len = (uint32_t)(pCommonInfo->codec_info.sample_rate * pCommonInfo->dma_info.frame_ms / 1000);
        pStreamEnv->stream_info.rx_algo_cfg.bits = pCommonInfo->dma_info.bits_depth;

        if (gaf_stream_process_context_is_media(pStreamEnv->stream_info.bap_contextType)) {
            pStreamEnv->stream_info.rx_algo_cfg.bypass = false;
        } else {
            pStreamEnv->stream_info.rx_algo_cfg.bypass = true;
        }
#else
        pStreamEnv->stream_info.rx_algo_cfg.bypass = true;
#endif

        if (pStreamEnv->func_list->decoder_func_list)
        {
            pStreamEnv->func_list->decoder_func_list->decoder_init_func(
                0, &(pStreamEnv->stream_info.playbackInfo.codec_info));
        }

        struct AF_STREAM_CONFIG_T stream_cfg;
        int af_ply_priority = pCommonInfo->presDelayUs < pCommonInfo->dma_info.dmaChunkIntervalUs ? osPriorityRealtime : osPriorityHigh;
        af_set_priority(AF_USER_AOB_PLAYBACK, af_ply_priority);

#ifdef CODEC_DAC_MULTI_VOLUME_TABLE
        if (gaf_stream_process_context_is_call(pStreamEnv->stream_info.bap_contextType)) {
            hal_codec_set_dac_volume_table(codec_dac_hfp_vol, ARRAY_SIZE(codec_dac_hfp_vol));
        } else {
            hal_codec_set_dac_volume_table(codec_dac_a2dp_vol, ARRAY_SIZE(codec_dac_a2dp_vol));
        }
#endif
        // playback stream
        memset_s((void *)&stream_cfg, sizeof(struct AF_STREAM_CONFIG_T),
                0, sizeof(struct AF_STREAM_CONFIG_T));
        stream_cfg.bits         = (enum AUD_BITS_T)(pStreamEnv->stream_info.playbackInfo.dma_info.bits_depth);
        stream_cfg.channel_num  = (enum AUD_CHANNEL_NUM_T)(pStreamEnv->stream_info.playbackInfo.dma_info.num_channels);

#ifdef PLAYBACK_USE_I2S
        stream_cfg.channel_num = AUD_CHANNEL_NUM_2;
        stream_cfg.device  = AUD_STREAM_USE_I2S0_MASTER;
        stream_cfg.io_path = AUD_IO_PATH_NULL;
#elif defined(PSAP_SW_USE_DAC1)
        stream_cfg.device  = AUD_STREAM_USE_INT_CODEC3;
        stream_cfg.io_path = AUD_OUTPUT_PATH_SPEAKER;
#else
        stream_cfg.device  = AUD_STREAM_USE_INT_CODEC;
        stream_cfg.io_path = AUD_OUTPUT_PATH_SPEAKER;
#endif
        stream_cfg.sample_rate  = (enum AUD_SAMPRATE_T)g_playback_sample_rate;
        if (stream_cfg.channel_num == AUD_CHANNEL_NUM_2)
        {
            stream_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)( AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1);
        }
        stream_cfg.vol          = vol;
        stream_cfg.data_size    = (uint32_t)(2 * pStreamEnv->stream_info.playbackInfo.dma_info.dmaChunkSize);
        stream_cfg.data_ptr = BT_AUDIO_CACHE_2_UNCACHE(pStreamEnv->stream_info.playbackInfo.dma_info.dmaBufPtr);
        stream_cfg.handler = pStreamEnv->func_list->stream_func_list.playback_dma_irq_handler_func;

        bes_ble_bap_dp_itf_data_come_callback_register((void *)gaf_stream_receive_data);

        af_stream_tune(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK, 0);
        af_stream_open(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK, &stream_cfg);

#ifdef STREAM_PROCESS_ENABLE
        gaf_stream_process_playback_open(pStreamEnv->stream_info.bap_contextType, &stream_cfg);
#endif
        gaf_stream_resample_playback_dn_init(&pStreamEnv->stream_info, &stream_cfg);

#if defined(SPA_AUDIO_ENABLE)
        uint32_t audio_chnl_sel = bts_bt_if_get_audio_chnl_sel();
        //spatial_audio_app_init_if(sample_rate, sample_bits, hw_ch_num, frame_size, (void*)app_audio_mempool_get_buff, bts_core_get_ui_role(), audio_chnl_sel);
        spatial_audio_app_init_if(stream_cfg.sample_rate,stream_cfg.bits,stream_cfg.channel_num,stream_cfg.data_size/stream_cfg.channel_num/(stream_cfg.bits <= AUD_BITS_16 ? 2 : 4)/2,(void*)app_audio_mempool_get_buff, bts_core_get_ui_role(), audio_chnl_sel);
#endif

#ifdef GAF_STREAM_PLAYBACK_AUDIO_DUMP
        if (stream_cfg.bits == 16) {
            g_stream_pcm_dump_sample_bytes = sizeof(int16_t);
        } else {
            g_stream_pcm_dump_sample_bytes = sizeof(int32_t);
        }
        g_stream_pcm_dump_channel_num = stream_cfg.channel_num;
        g_stream_pcm_dump_frame_len = stream_cfg.data_size / 2 / g_stream_pcm_dump_sample_bytes / g_stream_pcm_dump_channel_num;
#if GAF_STREAM_PLAYBACK_AUDIO_DUMP == 16
        audio_dump_init(g_stream_pcm_dump_frame_len, sizeof(int16_t), 1);
#else
        audio_dump_init(g_stream_pcm_dump_frame_len, g_stream_pcm_dump_sample_bytes, 1);
#endif
#endif

#ifdef BT_SVC_MODULE_TWS_ENABLED
        pStreamEnv->stream_context.playbackTriggerChannel = BLE_AUDIO_PLAYBACK_TRIGGER_CHANNEL;
#endif

        gaf_media_prepare_playback_trigger(pStreamEnv->stream_context.playbackTriggerChannel);

        gaf_media_pid_init(&(pStreamEnv->stream_context.playback_pid_env));

        gaf_stream_common_update_playback_stream_state(pStreamEnv, GAF_PLAYBACK_STREAM_INITIALIZED);

        bt_drv_reg_op_afh_assess_en(true);

#ifdef AF_DELAY_HANDLE_DMA_IRQ_SUPPORT
        if (pCommonInfo->presDelayUs < pCommonInfo->dma_info.dmaChunkIntervalUs)
        {
            af_register_delay_ticks_get_callback(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK,
                                                gaf_audio_stream_delay_get_pcm_ticks_get, (uint32_t)pStreamEnv);
            af_delay_handle_dma_irq_enable(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK, true);
            bta_bt_config_checker_logging(0);
        }
#endif

        af_stream_start(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);

#ifdef MIX_AUDIO_PROMPT_WITH_A2DP_MEDIA_ENABLED
        audio_prompt_stop_playing();
        audio_prompt_buffer_config(MIX_WITH_BLE_AUDIO_FLEXIBLE_STREAMING,
                                    pStreamEnv->stream_info.playbackInfo.codec_info.num_channels,
                                    stream_cfg.bits,
                                    promptTmpSourcePcmDataBuf,
                                    promptTmpTargetPcmDataBuf,
                                    promptPcmDataBuf,
                                    AUDIO_PROMPT_PCM_BUFFER_SIZE,
                                    promptResamplerBuf,
                                    AUDIO_PROMPT_BUF_SIZE_FOR_RESAMPLER);
#endif

#ifdef PLAYBACK_USE_I2S
        hal_cmu_audio_resample_disable();
#endif

        uint32_t playback_freq = APP_SYSFREQ_52M;
        if (pStreamEnv->func_list->decoder_func_list)
        {
           playback_freq = pStreamEnv->func_list->decoder_func_list->decoder_set_freq(
                &(pStreamEnv->stream_info.playbackInfo.codec_info), playback_freq, M33_CORE);
        }
        app_sysfreq_req(APP_SYSFREQ_USER_AOB_PLAYBACK, (enum APP_SYSFREQ_FREQ_T)playback_freq);

        LOG_D("%s end", __func__);

        return 0;
    }



    return -1;
}

#if defined(ANC_ASSIST_LE_AUDIO_SUPPORT)
static uint32_t gaf_audio_anc_resample_heap_free_size()
{
    return RESAMPLE_HEAP_BUFF_SIZE - g_anc_assist_resample_buf_used;
}

static void gaf_audio_anc_resample_heap_init()
{
    g_anc_assist_resample_buf_used = 0;
    memset_s(g_anc_assist_resample_buf, RESAMPLE_HEAP_BUFF_SIZE, 0, RESAMPLE_HEAP_BUFF_SIZE);
}

static int gaf_audio_anc_resample_get_heap_buff(uint8_t **buff, uint32_t size)
{
    uint32_t buff_size_free;

    buff_size_free = gaf_audio_anc_resample_heap_free_size();

    if (size % 4){
        size = size + (4 - size % 4);
    }
    ASSERT (size <= buff_size_free, "anc resample pool in shortage! To allocate size %d but free size %d.",
        size, buff_size_free);
    *buff = g_anc_assist_resample_buf + g_anc_assist_resample_buf_used;
    g_anc_assist_resample_buf_used += size;
    LOG_I("[%s] ptr=%p size=%u free=%u", __func__, *buff, size, buff_size_free);
    return buff_size_free;
}

void *gaf_anc_assist_resample_malloc(size_t size)
{
    uint8_t *pBuf = NULL;

    if (size % 4) {
        size = size + (4 - size % 4);
    }

    gaf_audio_anc_resample_get_heap_buff(&pBuf, size);

    return (void *)pBuf;
}

void *gaf_anc_assist_resample_calloc(size_t nitems, size_t len)
{
    uint8_t *pBuf = NULL;
    int size;

    size = nitems * len;

    if (size % 4) {
        size = size + (4 - size % 4);
    }

    gaf_audio_anc_resample_get_heap_buff(&pBuf, size);

    memset((uint8_t*)pBuf, 0x0, size);

    return (void *)pBuf;
}

void gaf_anc_assist_resample_free_ext_buf(void *mem_ptr)
{

}

static POSSIBLY_UNUSED custom_allocator gaf_resample_allocator = {
    .malloc = gaf_anc_assist_resample_malloc,
    .calloc = gaf_anc_assist_resample_calloc,
    .free = gaf_anc_assist_resample_free_ext_buf,
};
#endif

static void gaf_audio_flexible_common_buf_init(GAF_AUDIO_STREAM_ENV_T * pStreamEnv, GAF_STREAM_TYPE_E stream_type)
{
    uint32_t audioCacheHeapSize = 0;
    uint8_t* heapBufStartAddr = NULL;
    if ((pStreamEnv->stream_context.playback_stream_state <= GAF_PLAYBACK_STREAM_INITIALIZING) &&
        (pStreamEnv->stream_context.capture_stream_state <= GAF_CAPTURE_STREAM_INITIALIZING))
    {
        pStreamEnv->stream_info.playbackInfo.dma_info.storedDmaBufPtr = NULL;
        pStreamEnv->stream_info.captureInfo.dma_info.storedDmaBufPtr = NULL;
        pStreamEnv->stream_context.capture_frame_cache = NULL;
        pStreamEnv->stream_context.playback_frame_cache = NULL;
        pStreamEnv->stream_context.playback_pcm_cache = NULL;
#if !defined (GAF_CODEC_CROSS_CORE) && !defined (AOB_CODEC_CP)
        for (uint32_t instance = 0; instance < PLAYBACK_INSTANCE_MAX; instance++) {
            pStreamEnv->stream_context.playback_frame_buf[instance] = NULL;
            pStreamEnv->stream_context.playback_frame_fifo[instance] = NULL;
        }
#endif
#if defined(SPEECH_BONE_SENSOR) && defined(GAF_ENCODER_CROSS_CORE_USE_M55)
        g_capture_combo_vpu_pcm_buf = NULL;
#endif
#if defined(ANC_ASSIST_LE_AUDIO_SUPPORT)
        g_anc_assist_interval_buf = NULL;
        g_anc_assist_resample_buf = NULL;
#endif

#ifdef STREAM_PROCESS_ENABLE
        gaf_stream_process_init();
#endif
        gaf_codec_buf_entry_init();
        gaf_stream_resample_process_init();
        app_audio_mempool_init();

        if (GAF_STREAM_PLAYBACK == stream_type)
        {
            audioCacheHeapSize = pStreamEnv->stream_info.playbackInfo.maxCachedFrameCount*
                pStreamEnv->stream_info.playbackInfo.maxFrameSize;
        }
        else
        {
            audioCacheHeapSize = pStreamEnv->stream_info.captureInfo.maxCachedFrameCount*
                pStreamEnv->stream_info.captureInfo.maxFrameSize;
        }

        app_audio_mempool_get_buff(&heapBufStartAddr, audioCacheHeapSize);
        gaf_stream_heap_init(heapBufStartAddr, audioCacheHeapSize);

#ifdef MIX_AUDIO_PROMPT_WITH_A2DP_MEDIA_ENABLED
        app_audio_mempool_get_buff(&promptTmpSourcePcmDataBuf, AUDIO_PROMPT_SOURCE_PCM_BUFFER_SIZE);
        app_audio_mempool_get_buff(&promptTmpTargetPcmDataBuf, AUDIO_PROMPT_TARGET_PCM_BUFFER_SIZE);
        app_audio_mempool_get_buff(&promptPcmDataBuf, AUDIO_PROMPT_PCM_BUFFER_SIZE);
        app_audio_mempool_get_buff(&promptResamplerBuf, AUDIO_PROMPT_BUF_SIZE_FOR_RESAMPLER);
#endif

    }

    if (GAF_STREAM_PLAYBACK == stream_type)
    {
#ifdef STREAM_PROCESS_ENABLE
        gaf_stream_process_playback_buf_init(pStreamEnv->stream_info.bap_contextType);
#endif
        if (NULL == pStreamEnv->stream_info.playbackInfo.dma_info.storedDmaBufPtr)
        {
            app_audio_mempool_get_buff(&(pStreamEnv->stream_info.playbackInfo.dma_info.storedDmaBufPtr),
                            pStreamEnv->stream_info.playbackInfo.dma_info.dmaChunkSize*2);
        }
        pStreamEnv->stream_info.playbackInfo.dma_info.dmaBufPtr = pStreamEnv->stream_info.playbackInfo.dma_info.storedDmaBufPtr;
        gaf_stream_common_update_playback_stream_state(pStreamEnv, GAF_PLAYBACK_STREAM_BUF_INITIALIZED);
    }
    else
    {
        GAF_AUDIO_STREAM_COMMON_INFO_T *pCommonInfo = &pStreamEnv->stream_info.captureInfo;
#ifdef STREAM_PROCESS_ENABLE
        gaf_stream_process_capture_buf_init(pStreamEnv->stream_info.bap_contextType);
#endif

        if (gaf_stream_process_context_is_live(pStreamEnv->stream_info.bap_contextType)) {
            g_capture_ch_num = 1;
            g_speech_algo_ch_num = 1;
        } else {
#ifdef ANC_ASSIST_LE_AUDIO_SUPPORT
            g_capture_ch_num = app_anc_assist_get_mic_ch_num(AUD_INPUT_PATH_MAINMIC);
            pCommonInfo->dma_info.dmaChunkSize *= g_capture_ch_num;
#else
            g_capture_ch_num = hal_codec_get_input_path_chan_num(AUD_INPUT_PATH_MAINMIC);
            pCommonInfo->dma_info.dmaChunkSize *= g_capture_ch_num;
#endif
            g_speech_algo_ch_num = hal_codec_get_input_path_chan_num(AUD_INPUT_PATH_MAINMIC);

#if defined(SPEECH_BONE_SENSOR) && defined(GAF_ENCODER_CROSS_CORE_USE_M55)
            if ((g_capture_combo_vpu_pcm_buf == NULL) && g_capture_vpu_enabled) {
                g_capture_combo_vpu_pcm_buf = (int32_t *)app_audio_mempool_calloc(g_capture_frame_len * (g_speech_algo_ch_num + 1), sizeof(int32_t));
            }
#endif
        }
        LOG_I("[%s] speech algo ch_num: %d", __func__, g_speech_algo_ch_num);

#ifdef STREAM_PROCESS_ENABLE
        gaf_stream_process_set_capture_channel_num(g_speech_algo_ch_num);
#endif

#if defined(ANC_ASSIST_LE_AUDIO_SUPPORT)
        if (g_anc_assist_interval_buf == NULL) {
#if defined (VOICE_ASSIST_FIR_LMS_ENABLED) || defined (VOICE_ASSIST_FF_FIR_LMS)
            if (g_capture_sample_rate != AUD_SAMPRATE_32000) {
                app_audio_mempool_get_buff(&g_anc_assist_interval_buf, g_capture_frame_len * (32000 / g_capture_sample_rate) * g_capture_ch_num * sizeof(int32_t));
                app_audio_mempool_get_buff(&g_anc_assist_resample_buf, RESAMPLE_HEAP_BUFF_SIZE);
            }
#else
            app_audio_mempool_get_buff(&g_anc_assist_interval_buf, g_capture_frame_len * g_capture_ch_num * sizeof(int32_t));
            if (g_capture_sample_rate != AUD_SAMPRATE_16000) {
                app_audio_mempool_get_buff(&g_anc_assist_resample_buf, RESAMPLE_HEAP_BUFF_SIZE);
            }
#endif
        }
#endif

        if (NULL == pCommonInfo->dma_info.storedDmaBufPtr)
        {
            app_audio_mempool_get_buff(&(pCommonInfo->dma_info.storedDmaBufPtr), pCommonInfo->dma_info.dmaChunkSize*2);
        }
        pCommonInfo->dma_info.dmaBufPtr = pCommonInfo->dma_info.storedDmaBufPtr;
        gaf_stream_common_update_capture_stream_state(pStreamEnv, GAF_CAPTURE_STREAM_BUF_INITIALIZED);
    }
}

static void gaf_audio_flexible_playback_buf_init(void* _pStreamEnv)
{
    LOG_D("%s start", __func__);
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T *)_pStreamEnv;

    gaf_audio_flexible_common_buf_init(pStreamEnv, GAF_STREAM_PLAYBACK);

#if !defined (GAF_CODEC_CROSS_CORE) && !defined (AOB_CODEC_CP)
    uint32_t frame_size = pStreamEnv->stream_info.playbackInfo.codec_info.frame_size *
        pStreamEnv->stream_info.playbackInfo.codec_info.num_channels +  sizeof(CC_PLAYBACK_DATA_T);
    uint32_t frame_buf_size = pStreamEnv->stream_info.playbackInfo.maxCachedFrameCount * frame_size;
    if (NULL == pStreamEnv->stream_context.playback_frame_cache)
    {
        app_audio_mempool_get_buff(
            &(pStreamEnv->stream_context.playback_frame_cache), frame_size);
    }
    if (NULL == pStreamEnv->stream_context.playback_frame_fifo[0])
    {
        app_audio_mempool_get_buff(
            (uint8_t**)&(pStreamEnv->stream_context.playback_frame_fifo[0]), sizeof(cfifo));
    }
    if (NULL == pStreamEnv->stream_context.playback_frame_buf[0])
    {
        app_audio_mempool_get_buff(
            &(pStreamEnv->stream_context.playback_frame_buf[0]), frame_buf_size);
    }
    cfifo_init(pStreamEnv->stream_context.playback_frame_fifo[0],
        pStreamEnv->stream_context.playback_frame_buf[0], frame_buf_size);
#ifdef BLE_AUDIO_STEREO_CHAN_OVER_CIS_CNT
    if (NULL == pStreamEnv->stream_context.playback_frame_fifo[1])
    {
        app_audio_mempool_get_buff(
            (uint8_t**)&(pStreamEnv->stream_context.playback_frame_fifo[1]), sizeof(cfifo));
    }
    if (NULL == pStreamEnv->stream_context.playback_frame_buf[1])
    {
        app_audio_mempool_get_buff(
            &(pStreamEnv->stream_context.playback_frame_buf[1]), frame_buf_size);
    }
    cfifo_init(pStreamEnv->stream_context.playback_frame_fifo[1],
        pStreamEnv->stream_context.playback_frame_buf[1], frame_buf_size);
#endif
#else
#ifdef BLE_AUDIO_STEREO_CHAN_OVER_CIS_CNT
    if (NULL == pStreamEnv->stream_context.playback_pcm_cache)
    {
        app_audio_mempool_get_buff(&(pStreamEnv->stream_context.playback_pcm_cache),
            pStreamEnv->stream_info.playbackInfo.dma_info.dmaChunkSize);
    }
#endif
#endif
    if (pStreamEnv->func_list->decoder_func_list)
    {
        pStreamEnv->func_list->decoder_func_list->decoder_init_buf_func(0,
            &(pStreamEnv->stream_info.playbackInfo.codec_info), gaf_stream_common_buf_alloc, gaf_stream_common_buf_free);
    }

    LOG_D("%s end", __func__);
}

static inline int gaf_playback_status_mutex_init(void *mutex)
{
    if (mutex == NULL){
        mutex = osMutexCreate((osMutex(gaf_playback_status_mutex)));
    }
    return 0;
}

static int gaf_stream_set_playback_status(uint8_t con_lid,
                                          GAF_STREAM_PLAYBACK_STATUS_E playback_status)
{
    void *mutex = gaf_audio_stream_env[con_lid].stream_info.playback_status_mutex;

    gaf_playback_status_mutex_lock(mutex);
    gaf_stream_playback_status = playback_status;
    gaf_playback_status_mutex_unlock(mutex);

    return 0;
}

GAF_STREAM_PLAYBACK_STATUS_E gaf_stream_get_playback_status(uint8_t con_lid)
{
    void *mutex = gaf_audio_stream_env[con_lid].stream_info.playback_status_mutex;
    GAF_STREAM_PLAYBACK_STATUS_E playback_status;

    gaf_playback_status_mutex_lock(mutex);
    playback_status = gaf_stream_playback_status;
    gaf_playback_status_mutex_unlock(mutex);

    return playback_status;
}

void gaf_stream_dump_dma_trigger_status()
{
    uint8_t context_id = 0;
    uint8_t len = 0;
    char state_str[60];
    uint8_t con_lid = 0;
    for (con_lid = 0; con_lid < AOB_COMMON_MOBILE_CONNECTION_MAX; con_lid++)
    {
        GAF_PLAYBACK_STREAM_STATE_E trigger_status = gaf_audio_stream_env[con_lid].stream_context.playback_stream_state;
        if(trigger_status == GAF_PLAYBACK_STREAM_IDLE)
            continue;
        context_id = gaf_audio_stream_env[con_lid].stream_info.bap_contextType;
        len += sprintf(state_str + len, "con_id %d context %d state %d ", con_lid, context_id, trigger_status);
    }

    if (len==0){
        LOG_I("gaf dma tri error");
    }
    else{
         LOG_I("gaf tri: %s", state_str);
    }
}


static int gaf_stream_playback_stop(uint8_t con_lid)
{
    LOG_D("%s",__func__);
    int cnt = 50;

    do {
        if (gaf_stream_get_playback_status(con_lid) == GAF_STREAM_PLAYBACK_STATUS_IDLE)
        {
            LOG_I("[STOP] PLAYBACK_STATUS_IDLE cnt:%d", cnt);
            break;
        }
        else
        {
            osDelay(1);
        }
    } while(--cnt > 0);

    return 0;
}

static int gaf_audio_flexible_playback_stream_stop_handler(void* _pStreamEnv)
{
    LOG_D("%s start", __func__);
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T *)_pStreamEnv;

    if (GAF_PLAYBACK_STREAM_IDLE != pStreamEnv->stream_context.playback_stream_state)
    {
        bool POSSIBLY_UNUSED isRetrigger = pStreamEnv->stream_context.playback_retrigger_onprocess;
        app_sysfreq_req(APP_SYSFREQ_USER_AOB_PLAYBACK, APP_SYSFREQ_208M);
        bes_ble_bap_dp_itf_data_come_callback_deregister();
        gaf_media_stop_all_iso_dp_rx(pStreamEnv);
        gaf_stream_common_update_playback_stream_state(pStreamEnv, GAF_PLAYBACK_STREAM_IDLE);
        gaf_stream_playback_stop(pStreamEnv->stream_info.con_lid);

        uint8_t POSSIBLY_UNUSED adma_ch = HAL_DMA_CHAN_NONE;
        uint32_t dma_base;
        // sink
        af_stream_dma_tc_irq_disable(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
        adma_ch = af_stream_get_dma_chan(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
        dma_base = af_stream_get_dma_base_addr(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
        if(adma_ch != HAL_DMA_CHAN_NONE)
        {
            bt_drv_reg_op_disable_dma_tc(adma_ch&0xFF, dma_base);
        }

#ifdef AF_DELAY_HANDLE_DMA_IRQ_SUPPORT
        af_delay_handle_dma_irq_enable(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK, false);
#endif
        af_stream_stop(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
        af_stream_close(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
        af_codec_tune(AUD_STREAM_PLAYBACK, 0);

        gaf_stream_resample_playback_dn_deinit();
#ifdef STREAM_PROCESS_ENABLE
        gaf_stream_process_playback_close();
#endif

        if (pStreamEnv->func_list->decoder_func_list)
        {
            pStreamEnv->func_list->decoder_func_list->decoder_deinit_func(0);
        }
#if defined (GAF_CODEC_CROSS_CORE) || defined (AOB_CODEC_CP)
        gaf_playback_decoder_deinit_all_ase(pStreamEnv, isRetrigger);
#endif

        gaf_stream_common_clr_trigger(pStreamEnv->stream_context.playbackTriggerChannel);

        af_codec_set_bt_sync_source(AUD_STREAM_PLAYBACK, 0);

        af_set_priority(AF_USER_AOB_PLAYBACK, osPriorityAboveNormal);
        pStreamEnv->func_list->stream_func_list.playback_deinit_stream_buf_func(pStreamEnv);

#ifdef GAF_ENCODER_CROSS_CORE_USE_M55
        app_overlay_unloadall();
#else
        if ((GAF_CAPTURE_STREAM_IDLE == pStreamEnv->stream_context.capture_stream_state)
            && (APP_OVERLAY_ID_QTY != app_get_current_overlay())) {
            app_overlay_unloadall();
        }
#endif

#ifdef BLE_I2S_AUDIO_SUPPORT
        app_ble_audio_stream_stop_post_handler();
#endif
        app_sysfreq_req(APP_SYSFREQ_USER_AOB_PLAYBACK, APP_SYSFREQ_32K);
        LOG_D("%s end", __func__);
        return 0;
    }

    return -1;
}

static bool gaf_stream_is_any_downstream_iso_created(uint8_t con_lid)
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = &gaf_audio_stream_env[con_lid];

    for (uint8_t i = 0; i < GAF_AUDIO_ASE_TOTAL_COUNT; i++)
    {
        if (GAF_AUDIO_INVALID_ISO_CHANNEL != pStreamEnv->stream_info.playbackInfo.aseChInfo[i].iso_channel_hdl)
        {
            return true;
        }
    }

    return false;
}

static uint32_t gaf_stream_get_curr_dma_irq_happening_time(bool is_pre_get)
{
    static uint32_t dmaIrqHappeningTimeUs = 0;
    uint32_t btclk; //hal slot -- 312.5us
    uint16_t btcnt; //hal  microsecond -- 0.5 us
    uint8_t adma_ch = HAL_DMA_CHAN_NONE;
    uint32_t dma_base;

    if (is_pre_get == false)
    {
        return dmaIrqHappeningTimeUs;
    }

    adma_ch = af_stream_get_dma_chan(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
    dma_base = af_stream_get_dma_base_addr(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
    if (adma_ch != HAL_DMA_CHAN_NONE)
    {
        bt_drv_reg_op_dma_tc_clkcnt_get_by_ch(&btclk, &btcnt, adma_ch&0xFF, dma_base);
        dmaIrqHappeningTimeUs = bt_syn_ble_bt_time_to_bts(btclk, btcnt);
    }

    return dmaIrqHappeningTimeUs;
}

static uint32_t _gaf_stream_flexible_playback_dma_irq_handler(uint8_t con_lid, uint8_t* ptrBuf, uint32_t length)
{
    LOG_D("%s buffer len = %d", __func__,length);
    GAF_AUDIO_STREAM_ENV_T *pStreamEnv = &gaf_audio_stream_env[con_lid];

    if (!gaf_stream_is_any_downstream_iso_created(pStreamEnv->stream_info.con_lid))
    {
        LOG_E("%s %d invalid chan", __func__, pStreamEnv->stream_info.con_lid);
        return length;
    }

    int ret = CODEC_OK;
    POSSIBLY_UNUSED uint8_t instance_handle = 0;

    gaf_stream_set_playback_status(con_lid, GAF_STREAM_PLAYBACK_STATUS_BUSY);
#ifdef AF_DELAY_HANDLE_DMA_IRQ_SUPPORT
    uint32_t dmaIrqHappeningTimeUs = gaf_stream_get_curr_dma_irq_happening_time(false);
#else
    uint32_t dmaIrqHappeningTimeUs = gaf_stream_get_curr_dma_irq_happening_time(true);
#endif
    // it's possible the multiple DMA irq triggered message accumulates,
    // so the acuiqred dmaIrqHappeningTimeUs is the last dma irq, which has
    // been handled. For this case, ignore this dma irq message handling
    if (true)
    {
        if ((!pStreamEnv) ||
            (GAF_PLAYBACK_STREAM_START_TRIGGERING > pStreamEnv->stream_context.playback_stream_state) ||
            ((GAF_PLAYBACK_STREAM_STREAMING_TRIGGERED == pStreamEnv->stream_context.playback_stream_state) &&
            (dmaIrqHappeningTimeUs == pStreamEnv->stream_context.lastPlaybackDmaIrqTimeUs)))
        {
            memset_s(ptrBuf, length, 0, length);
            gaf_stream_set_playback_status(con_lid, GAF_STREAM_PLAYBACK_STATUS_IDLE);
            LOG_E("%s state err", __func__);
            return length;
        }

#if defined (BLE_AUDIO_STEREO_CHAN_OVER_CIS_CNT)
        gaf_stream_common_update_multi_channel_expect_seq_and_time(pStreamEnv, dmaIrqHappeningTimeUs);
#else
        gaf_stream_common_updated_expeceted_playback_seq_and_time(pStreamEnv, GAF_AUDIO_DFT_PLAYBACK_LIST_IDX, dmaIrqHappeningTimeUs);
#endif

        if (GAF_PLAYBACK_STREAM_STREAMING_TRIGGERED !=
            pStreamEnv->stream_context.playback_stream_state)
        {
            gaf_stream_common_update_playback_stream_state(pStreamEnv,  GAF_PLAYBACK_STREAM_STREAMING_TRIGGERED);
            gaf_stream_common_clr_trigger(pStreamEnv->stream_context.playbackTriggerChannel);
            LOG_I("%s Update playback 0 seq to [0x%x]", __func__, pStreamEnv->stream_context.lastestPlaybackSeqNum[GAF_AUDIO_DFT_PLAYBACK_LIST_IDX]);
#if defined (BLE_AUDIO_STEREO_CHAN_OVER_CIS_CNT)
            if (pStreamEnv->stream_context.lastestPlaybackSeqNum[1] != GAF_AUDIO_INVALID_SEQ_NUMBER)
            {
               LOG_I("%s Update playback 1 seq to [0x%x]", __func__, pStreamEnv->stream_context.lastestPlaybackSeqNum[1]);
            }
#endif
        }
    }

    gaf_stream_resample_remap_dn_ptrbuf_length(&ptrBuf, &length);

#if defined (GAF_CODEC_CROSS_CORE) || defined (AOB_CODEC_CP)
    bool isSuccessful = false;
#ifdef BLE_AUDIO_STEREO_CHAN_OVER_CIS_CNT
    isSuccessful = gaf_stream_common_fetch_multi_pcm(pStreamEnv, ptrBuf, length, dmaIrqHappeningTimeUs);
#else
    isSuccessful = gaf_stream_common_fetch_pcm(pStreamEnv, ptrBuf, length,
                instance_handle, dmaIrqHappeningTimeUs, GAF_INVALID_SINK_PLAY_DELAY, true);
#endif
    if (!isSuccessful)
    {
        ret = CODEC_ERR;
    }
#else
    CC_PLAYBACK_DATA_T *frame = (CC_PLAYBACK_DATA_T*)pStreamEnv->stream_context.playback_frame_cache;
    frame->data = pStreamEnv->stream_context.playback_frame_cache + sizeof(CC_PLAYBACK_DATA_T);
    uint32_t pk_size;

#ifdef BLE_AUDIO_STEREO_CHAN_OVER_CIS_CNT
    pk_size = pStreamEnv->stream_info.playbackInfo.codec_info.frame_size;
    gaf_stream_common_get_combined_packet(pStreamEnv, frame, dmaIrqHappeningTimeUs, pk_size);
#else
    pk_size = pStreamEnv->stream_info.playbackInfo.codec_info.frame_size * pStreamEnv->stream_info.playbackInfo.codec_info.num_channels;
    gaf_stream_common_get_packet(pStreamEnv, frame,
        instance_handle, dmaIrqHappeningTimeUs, pk_size, true);
#endif

    ret = pStreamEnv->func_list->decoder_func_list->decoder_decode_frame_func
            (instance_handle, &(pStreamEnv->stream_info.playbackInfo.codec_info),
            frame->data_len, frame->data,  ptrBuf, frame->isPLC);

    LOG_D("decoded seq 0x%02x expected play time %u local time %u dec ret %d",
            frame->seq_nb, frame->time_stamp, dmaIrqHappeningTimeUs, ret);

    /// gaf custom, may be a watch point to check some pattern in decoded raw data
    // if (gaf_uc_srv_custom_data_callback->decoded_raw_data_cb)
    // {
    //     gaf_uc_srv_custom_data_callback->decoded_raw_data_cb(frame, ptrBuf, length);
    // }
#endif

    if (CODEC_OK == ret)
    {
        length = gaf_stream_resample_playback_dn_run(ptrBuf, length);
        uint8_t *playback_downsampling_buf = gaf_stream_resample_get_dn_ptrbuf();
        if (playback_downsampling_buf){
            memcpy(playback_downsampling_buf, ptrBuf, length);
            ptrBuf = playback_downsampling_buf;
        }
#ifdef STREAM_PROCESS_ENABLE
#ifdef PLAYBACK_USE_I2S
    if (pStreamEnv->stream_info.playbackInfo.codec_info.num_channels  == 1)
    {
        length = gaf_stream_process_playback_run(ptrBuf, length/2);
        length *= 2;
    }
    else
    {
        length = gaf_stream_process_playback_run(ptrBuf, length);
    }
#else
        length = gaf_stream_process_playback_run(ptrBuf, length);
#endif
#endif
    }
    else
    {
        //LOG_E("[le][err]%dms,%p,%d->%p,%d",dmaIrqHappeningTimeUs/1000,decoder_frame_p->data,decoder_frame_p->data_len,ptrBuf,length);
        memset_s(ptrBuf, length, 0, length);
    }
    gaf_stream_set_playback_status(con_lid, GAF_STREAM_PLAYBACK_STATUS_IDLE);

#ifdef MIX_AUDIO_PROMPT_WITH_A2DP_MEDIA_ENABLED
    if (audio_prompt_is_playing_ongoing()){
#ifdef PLAYBACK_USE_I2S
    if (pStreamEnv->stream_info.playbackInfo.codec_info.num_channels  == 1)
    {
        audio_prompt_processing_handler(0, length/2, ptrBuf);
    }
    else
    {
        audio_prompt_processing_handler(0, length, ptrBuf);
    }
#elif
        audio_prompt_processing_handler(0, length, ptrBuf);
#endif
    }
#endif


#ifdef PLAYBACK_USE_I2S
    if (pStreamEnv->stream_info.playbackInfo.codec_info.num_channels == 1)
    {
        // 1 channel copy to n channel
        if (pStreamEnv->stream_info.playbackInfo.codec_info.bits_depth == AUD_BITS_16)
        {
            int16_t *buf_16 = (int16_t *)ptrBuf;
            for (int i = (int)(length/2/sizeof(int16_t) - 1); i >= 0; i--)
            {
                buf_16[i*2 + 0] = buf_16[i*2 + 1] = buf_16[i];
            }
        }
        else if (pStreamEnv->stream_info.playbackInfo.codec_info.bits_depth == AUD_BITS_24)
        {
            int32_t *buf_32 = (int32_t *)ptrBuf;

            uint32_t samples = length / 2 / sizeof(int32_t);

            for (int i = samples - 1; i >= 0; i--)
            {
                int32_t s = buf_32[i];
                buf_32[2*i]     = s;
                buf_32[2*i + 1] = s;
            }
        }
    }
#endif

    LOG_D("%s end", __func__);

    return 0;
}

static uint32_t gaf_stream_flexible_playback_idx_0_dma_irq_handler(uint8_t* ptrBuf, uint32_t length)
{
    return _gaf_stream_flexible_playback_dma_irq_handler(0, ptrBuf, length);
}

static uint32_t gaf_stream_flexible_playback_idx_1_dma_irq_handler(uint8_t* ptrBuf, uint32_t length)
{
    return _gaf_stream_flexible_playback_dma_irq_handler(1, ptrBuf, length);
}

static uint32_t gaf_stream_flexible_playback_idx_2_dma_irq_handler(uint8_t* ptrBuf, uint32_t length)
{
    return _gaf_stream_flexible_playback_dma_irq_handler(2, ptrBuf, length);
}

/// TODO: maybe need more

static void gaf_audio_flexible_playback_buf_deinit(void* _pStreamEnv)
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T *)_pStreamEnv;
    pStreamEnv->stream_info.playbackInfo.dma_info.dmaBufPtr = NULL;
    if (pStreamEnv->func_list->decoder_func_list)
    {
        pStreamEnv->func_list->decoder_func_list->decoder_deinit_buf_func(0);
    }

    LOG_I("[%s] syspool free size: %d/%d", __func__, syspool_free_size(), syspool_total_size());
}

static int gaf_audio_flexible_capture_stream_start_handler(void* _pStreamEnv)
{
    LOG_D("%s start", __func__);
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T *)_pStreamEnv;

    uint8_t rt_vol = bes_ble_arc_vol_get_real_time_volume(pStreamEnv->stream_info.con_lid);
    uint8_t vol = bes_ble_arc_convert_le_vol_to_local_vol(rt_vol);

    if (GAF_CAPTURE_STREAM_INITIALIZING == pStreamEnv->stream_context.capture_stream_state)
    {
#ifdef BLE_I2S_AUDIO_SUPPORT
        app_ble_audio_stream_start_pre_handler();
#endif
#ifdef __BIXBY
        app_bixby_on_suspend_handle();
#endif

        g_capture_sample_rate = get_gaf_stream_capture_sample_rate(pStreamEnv->stream_info.bap_contextType,
            pStreamEnv->stream_info.captureInfo.dma_info.sample_rate);
        g_capture_frame_len = (uint32_t)(g_capture_sample_rate * pStreamEnv->stream_info.captureInfo.dma_info.frame_ms / 1000);
        if (g_capture_sample_rate >= pStreamEnv->stream_info.captureInfo.dma_info.sample_rate) {
            uint32_t sample_rate_div = g_capture_sample_rate / pStreamEnv->stream_info.captureInfo.dma_info.sample_rate;
            pStreamEnv->stream_info.captureInfo.dma_info.dmaChunkSize *= sample_rate_div;
        } else {
            uint32_t sample_rate_div = pStreamEnv->stream_info.captureInfo.dma_info.sample_rate / g_capture_sample_rate;
            pStreamEnv->stream_info.captureInfo.dma_info.dmaChunkSize /= sample_rate_div;
        }

#ifdef ANC_ASSIST_ENABLED
#ifdef ANC_ASSIST_LE_AUDIO_SUPPORT
#if defined (VOICE_ASSIST_FIR_LMS_ENABLED) || defined (VOICE_ASSIST_FF_FIR_LMS)
        if (g_capture_sample_rate == 16000) {
            app_anc_assist_set_app_capture_info(g_capture_frame_len * 2, AUD_SAMPRATE_32000);
        } else if (g_capture_sample_rate == 32000) {
            app_anc_assist_set_app_capture_info(g_capture_frame_len, AUD_SAMPRATE_32000);
        }
#else
        app_anc_assist_set_app_capture_info(g_capture_frame_len / (g_capture_sample_rate / AUD_SAMPRATE_16000), AUD_SAMPRATE_16000);
#endif
#endif
        app_anc_assist_set_mode(ANC_ASSIST_MODE_LE_CALL);
#endif

#ifdef ANC_ASSIST_LE_AUDIO_SUPPORT
#if defined (VOICE_ASSIST_FIR_LMS_ENABLED) || defined (VOICE_ASSIST_FF_FIR_LMS)
        app_voice_assist_fir_lms_enable_fir(false);
#endif
#if defined(VOICE_ASSIST_FF_IIR_LMS)
        app_voice_assist_iir_lms_enable_iir(false);
#endif
#endif

#ifdef SPEECH_BONE_SENSOR
        if (gaf_stream_process_context_is_call(pStreamEnv->stream_info.bap_contextType) && (g_capture_sample_rate == AUD_SAMPRATE_16000)) {
            LOG_I("[%s] Enable VPU", __func__);
            g_capture_vpu_enabled = true;
        } else {
            LOG_I("[%s] Disable VPU", __func__);
            g_capture_vpu_enabled = false;
        }
#endif

        pStreamEnv->func_list->stream_func_list.capture_init_stream_buf_func(pStreamEnv);

        /**
         * Call after ANC_ASSIST switch mode, because ANC_ASSIST perhaps open VPU.
         * Make sure power on VPU in here.
         * If use VMIC for VPU and shared with MIC, you must move open VPU after af_stream_start(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
         */
#ifdef SPEECH_BONE_SENSOR
        if (g_capture_vpu_enabled) {
            speech_bone_sensor_open(16000, g_capture_frame_len);
// #ifdef VPU_STREAM_SYNC_TUNING
//         bt_sco_i2s_sync_tuning_reset();
// #endif

// #ifdef HW_I2S_TDM_TRIGGER
//         af_i2s_sync_config(AUD_STREAM_CAPTURE, AF_I2S_SYNC_TYPE_BT, true);
// #else
//         ASSERT(0, "Need to enable HW_I2S_TDM_TRIGGER");
// #endif
        }
#endif

#ifdef STREAM_PROCESS_ENABLE_USE_CC_CAPTURE
        pStreamEnv->stream_info.tx_algo_cfg.channel_num = g_speech_algo_ch_num;
#ifdef SPEECH_BONE_SENSOR
        if (g_capture_vpu_enabled) {
            pStreamEnv->stream_info.tx_algo_cfg.channel_num++;
        }
#endif

#ifdef APP_MCPP_CLI
        // MCPP framework parameter information
#ifndef MCPP_VERSION_STRING
#ifdef SPEECH_BONE_SENSOR
        pStreamEnv->stream_info.tx_algo_cfg.params[1] = app_mcpp_get_cap_channel_map(true, g_capture_vpu_enabled, false, false, false);
#else
        pStreamEnv->stream_info.tx_algo_cfg.params[1] = app_mcpp_get_cap_channel_map(true, false, false, false, false);
#endif
#endif
#endif

        pStreamEnv->stream_info.tx_algo_cfg.frame_len = g_capture_frame_len;
        // Please set real algo processing frame len in this place.
        // For the BES speech algorithm, you need to set it according to the following results.
        int32_t algo_frame_len = SPEECH_PROCESS_FRAME_MS * g_capture_sample_rate / 1000;
        pStreamEnv->stream_info.tx_algo_cfg.algo_frame_len = algo_frame_len;
        pStreamEnv->stream_info.tx_algo_cfg.sample_rate = g_capture_sample_rate;
        pStreamEnv->stream_info.tx_algo_cfg.bits = pStreamEnv->stream_info.captureInfo.dma_info.bits_depth;

        if (gaf_stream_process_context_is_call(pStreamEnv->stream_info.bap_contextType) &&
        ((g_capture_sample_rate == 16000) || (g_capture_sample_rate == 32000)) &&
        (pStreamEnv->stream_info.tx_algo_cfg.bits == 24)) {
            pStreamEnv->stream_info.tx_algo_cfg.bypass = false;
            app_overlay_subsys_select(APP_OVERLAY_M55, APP_OVERLAY_SUBSYS_SPEECH_ALGO);
        } else {
            pStreamEnv->stream_info.tx_algo_cfg.bypass = true;
        }
#endif

        if (pStreamEnv->func_list->encoder_func_list)
        {
            pStreamEnv->func_list->encoder_func_list->encoder_init_func(
                        0, &(pStreamEnv->stream_info.captureInfo.codec_info));
        }
#if defined (GAF_CODEC_CROSS_CORE) || defined (AOB_CODEC_CP)
        gaf_capture_encoder_init(pStreamEnv);
#endif

        struct AF_STREAM_CONFIG_T stream_cfg;

        af_set_priority(AF_USER_AOB_CAPTURE, osPriorityHigh);
        // capture stream
        memset_s((void *)&stream_cfg, sizeof(struct AF_STREAM_CONFIG_T),
               0, sizeof(struct AF_STREAM_CONFIG_T));
        stream_cfg.bits         = (enum AUD_BITS_T)(pStreamEnv->stream_info.captureInfo.dma_info.bits_depth);
        stream_cfg.sample_rate  = (enum AUD_SAMPRATE_T)g_capture_sample_rate;
        stream_cfg.channel_num  = (enum AUD_CHANNEL_NUM_T)g_capture_ch_num;
        stream_cfg.device       = AUD_STREAM_USE_INT_CODEC;
        stream_cfg.io_path      = AUD_INPUT_PATH_MAINMIC;

        if (gaf_stream_process_context_is_live(pStreamEnv->stream_info.bap_contextType)) {
            stream_cfg.channel_map  = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0);
        } else {
#ifdef ANC_ASSIST_LE_AUDIO_SUPPORT
            ASSERT(stream_cfg.bits == AUD_BITS_24, "[%s] bits(%d) != AUD_BITS_24", __func__, stream_cfg.bits);
            ASSERT(stream_cfg.sample_rate % AUD_SAMPRATE_16000 == 0, "[%s] sample_rate(%d) is invalid", __func__, stream_cfg.sample_rate);
            stream_cfg.channel_map  = (enum AUD_CHANNEL_MAP_T)app_anc_assist_get_mic_ch_map(AUD_INPUT_PATH_MAINMIC);

#if defined (VOICE_ASSIST_FIR_LMS_ENABLED) || defined (VOICE_ASSIST_FF_FIR_LMS)
            if (g_capture_sample_rate == 16000) {
                gaf_audio_anc_resample_heap_init();
                g_anc_assist_iir_resample_st = multi_iir_resample_init_with_custom_allocator(
                                g_capture_frame_len * g_capture_ch_num,
                                stream_cfg.bits,
                                g_capture_ch_num,
                                iir_resample_choose_mode(g_capture_sample_rate, 32000),
                                &gaf_resample_allocator);
            }
#else
            if (g_capture_sample_rate != AUD_SAMPRATE_16000) {
                gaf_audio_anc_resample_heap_init();
                anc_assist_resample_init(g_capture_sample_rate, g_capture_frame_len, &gaf_resample_allocator);
            }
#endif
#endif
        }

        g_capture_ch_num        = stream_cfg.channel_num;

        stream_cfg.vol          = vol;
        stream_cfg.data_size    = (uint32_t)(2 * pStreamEnv->stream_info.captureInfo.dma_info.dmaChunkSize);
        stream_cfg.data_ptr = BT_AUDIO_CACHE_2_UNCACHE(pStreamEnv->stream_info.captureInfo.dma_info.dmaBufPtr);
        stream_cfg.handler = pStreamEnv->func_list->stream_func_list.capture_dma_irq_handler_func;

#ifdef PSAP_SW_USE_DAC1
        af_codec_replace_echo_ref0_init(&stream_cfg, &g_capture_stream_channel_map, &g_capture_stream_channel_num);
#endif

        LOG_I("[%s] global frame_len: %d, ch_num: %d", __func__, g_capture_frame_len, g_capture_ch_num);

        af_stream_open(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE, &stream_cfg);

#ifdef STREAM_PROCESS_ENABLE
        gaf_stream_process_capture_open(pStreamEnv->stream_info.bap_contextType, &stream_cfg);
#endif
        gaf_stream_resample_capture_up_init(&pStreamEnv->stream_info, &stream_cfg);

        pStreamEnv->stream_context.isUsSinceLatestAnchorPointConfigured = false;
#ifdef BT_SVC_MODULE_TWS_ENABLED
        pStreamEnv->stream_context.captureTriggerChannel = app_bt_sync_get_available_trigger_channel(APP_BT_SYNC_OP_MAX, APP_BT_SYNC_POLICY_DEFAULT);
#endif

        gaf_media_prepare_capture_trigger(pStreamEnv->stream_context.captureTriggerChannel);

        gaf_media_pid_init(&(pStreamEnv->stream_context.capture_pid_env));
        gaf_media_pid_update_threshold(&(pStreamEnv->stream_context.capture_pid_env),
            pStreamEnv->stream_info.captureInfo.dma_info.dmaChunkIntervalUs/2);

        gaf_stream_common_update_capture_stream_state(pStreamEnv, GAF_CAPTURE_STREAM_INITIALIZED);
        gaf_stream_common_start_sync_capture(pStreamEnv);

        bt_drv_reg_op_afh_assess_en(true);

        af_stream_start(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);

#ifdef ANC_ASSIST_SUPPORT_FOUR_REF
        hal_codec_set_echo_rate(HAL_CODEC_ECHO_RATE_384K);
        hal_codec_set_echo_ch1_rate(HAL_CODEC_ECHO_RATE_384K);
        hal_codec_set_echo_path(HAL_CODEC_ECHO_PATH_DAC_2_6_UP_OUT);
        hal_codec_set_echo_ch(HAL_CODEC_ECHO_CHAN_DAC_DATA_OUT_L);
        hal_codec_set_echo_ch1(HAL_CODEC_ECHO_CHAN_DAC_DATA_DBG_OUT_R);
#endif

#ifdef SPEECH_BONE_SENSOR
        if (g_capture_vpu_enabled) {
            speech_bone_sensor_start();
        }
#endif

        uint32_t capture_freq = APP_SYSFREQ_78M;
        if (pStreamEnv->func_list->encoder_func_list)
        {
            capture_freq = pStreamEnv->func_list->encoder_func_list->encoder_set_freq(
                &(pStreamEnv->stream_info.captureInfo.codec_info), APP_SYSFREQ_78M, M33_CORE);
        }
        if (gaf_stream_process_context_is_live(pStreamEnv->stream_info.bap_contextType))
        {
#ifdef BINAURAL_RECORD_PROCESS
            capture_freq = APP_SYSFREQ_104M;
#else
            capture_freq = APP_SYSFREQ_78M;
#endif
        }
        app_sysfreq_req(APP_SYSFREQ_USER_AOB_CAPTURE, (enum APP_SYSFREQ_FREQ_T)capture_freq);
        LOG_I("flexible freq %d.", capture_freq);

        // FIXME: It's a workaround method
        if (gaf_stream_process_context_is_call(pStreamEnv->stream_info.bap_contextType)) {
            // Set sysfreq depend on algos
#ifdef GAF_CODEC_CROSS_CORE
            app_sysfreq_req(APP_SYSFREQ_USER_SPEECH_ALGO, APP_SYSFREQ_32K);
#else
            // app_sysfreq_req(APP_SYSFREQ_USER_SPEECH_ALGO, APP_SYSFREQ_104M);
            app_sysfreq_req(APP_SYSFREQ_USER_AOB_CAPTURE, APP_SYSFREQ_104M);
#endif
        }

        LOG_D("%s end", __func__);
        return 0;
    }

    return -1;
}

static void gaf_audio_flexible_capture_buf_init(void* _pStreamEnv)
{
    LOG_D("%s start", __func__);
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T *)_pStreamEnv;

    gaf_audio_flexible_common_buf_init(pStreamEnv, GAF_STREAM_CAPTURE);

    uint32_t frame_len =  pStreamEnv->stream_info.captureInfo.codec_info.frame_size *
            pStreamEnv->stream_info.captureInfo.codec_info.num_channels;
    if (NULL == pStreamEnv->stream_context.capture_frame_cache)
    {
        app_audio_mempool_get_buff(&(pStreamEnv->stream_context.capture_frame_cache), frame_len);
    }
    if (pStreamEnv->func_list->encoder_func_list)
    {
        pStreamEnv->func_list->encoder_func_list->encoder_init_buf_func(0,
            &(pStreamEnv->stream_info.captureInfo.codec_info), gaf_stream_common_buf_alloc, gaf_stream_common_buf_free);
    }
    LOG_D("%s end", __func__);
}

static int gaf_audio_flexible_capture_stream_stop_handler(void* _pStreamEnv)
{
    LOG_D("%s start", __func__);
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T *)_pStreamEnv;

    if (GAF_CAPTURE_STREAM_IDLE != pStreamEnv->stream_context.capture_stream_state)
    {
        bool POSSIBLY_UNUSED isRetrigger = pStreamEnv->stream_context.capture_retrigger_onprocess;
        app_sysfreq_req(APP_SYSFREQ_USER_AOB_CAPTURE, APP_SYSFREQ_208M);
        gaf_stream_common_update_capture_stream_state(pStreamEnv, GAF_CAPTURE_STREAM_IDLE);

        uint8_t POSSIBLY_UNUSED adma_ch = HAL_DMA_CHAN_NONE;
        uint32_t dma_base;
        // source
        af_stream_dma_tc_irq_disable(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
        adma_ch = af_stream_get_dma_chan(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
        dma_base = af_stream_get_dma_base_addr(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
        if(adma_ch != HAL_DMA_CHAN_NONE)
        {
            bt_drv_reg_op_disable_dma_tc(adma_ch&0xFF, dma_base);
        }

        af_stream_stop(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
        af_stream_close(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);

#ifdef SPEECH_BONE_SENSOR
        if (g_capture_vpu_enabled) {
            speech_bone_sensor_stop();
            speech_bone_sensor_close();
// #ifdef VPU_STREAM_SYNC_TUNING
//         bt_sco_i2s_sync_tuning_reset();
// #endif
        }
        g_capture_vpu_enabled = false;
#endif

        gaf_stream_resample_capture_up_deinit();
#ifdef STREAM_PROCESS_ENABLE
        gaf_stream_process_capture_close();
#endif

        gaf_stream_common_clr_trigger(pStreamEnv->stream_context.captureTriggerChannel);

        af_codec_set_bt_sync_source(AUD_STREAM_CAPTURE, 0);

        af_set_priority(AF_USER_AOB_CAPTURE, osPriorityAboveNormal);

#if defined (GAF_CODEC_CROSS_CORE) || defined (AOB_CODEC_CP)
        gaf_capture_encoder_deinit(pStreamEnv, isRetrigger);
#endif
        if (pStreamEnv->func_list->encoder_func_list)
        {
            pStreamEnv->func_list->encoder_func_list->encoder_deinit_func(0);
        }

        pStreamEnv->func_list->stream_func_list.capture_deinit_stream_buf_func(pStreamEnv);

#ifdef GAF_ENCODER_CROSS_CORE_USE_M55
        app_overlay_subsys_unloadall(APP_OVERLAY_M55);
#else
        app_overlay_unloadall();
#endif

#ifdef ANC_ASSIST_LE_AUDIO_SUPPORT
#if defined (VOICE_ASSIST_FIR_LMS_ENABLED) || defined (VOICE_ASSIST_FF_FIR_LMS)
        if (g_anc_assist_iir_resample_st != NULL) {
            iir_resample_destroy(g_anc_assist_iir_resample_st);
            g_anc_assist_iir_resample_st = NULL;
        }
#else
        if (g_capture_sample_rate != AUD_SAMPRATE_16000) {
            anc_assist_resample_deinit(g_capture_sample_rate);
        }
#endif
#endif
#ifdef ANC_ASSIST_ENABLED
        app_anc_assist_set_mode(ANC_ASSIST_MODE_STANDALONE);
#endif

#ifdef ANC_ASSIST_LE_AUDIO_SUPPORT
#if defined (VOICE_ASSIST_FIR_LMS_ENABLED) || defined (VOICE_ASSIST_FF_FIR_LMS)
        app_voice_assist_fir_lms_enable_fir(true);
#endif
#if defined(VOICE_ASSIST_FF_IIR_LMS)
        app_voice_assist_iir_lms_enable_iir(true);
#endif
#endif

#ifdef BLE_I2S_AUDIO_SUPPORT
        app_ble_audio_stream_stop_post_handler();
#endif

        app_sysfreq_req(APP_SYSFREQ_USER_AOB_CAPTURE, APP_SYSFREQ_32K);
        LOG_D("%s end", __func__);
        return 0;
    }

    return -1;
}

static void gaf_stream_capture_dma_irq_handler_send(void* pStreamEnv_, void *payload,
    uint32_t payload_size, uint32_t ref_time)
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T*) pStreamEnv_;

    uint8_t ase_count = GAF_AUDIO_ASE_TOTAL_COUNT;
    uint8_t channel_shift = 0;
    uint8_t audio_allocation_cnt = 0;
    uint16_t payload_len_per_channel = 0;

    for (uint8_t i = 0; i < ase_count; i++)
    {
        if (GAF_AUDIO_INVALID_ISO_CHANNEL == pStreamEnv->stream_info.captureInfo.aseChInfo[i].iso_channel_hdl)
        {
            continue;
        }

        channel_shift = 0;
        audio_allocation_cnt = bes_ble_audio_get_location_fs_l_r_cnt(pStreamEnv->stream_info.captureInfo.aseChInfo[i].allocation_bf);
        // Config palyload len  for AUD_CHANNEL_NUM_2
        payload_len_per_channel = payload_size / pStreamEnv->stream_info.captureInfo.codec_info.num_channels;
        // Check is there use two ASE
        if (audio_allocation_cnt == AUD_CHANNEL_NUM_1)
        {
            if ((pStreamEnv->stream_info.captureInfo.aseChInfo[i].allocation_bf &
                (BES_BLE_LOC_SIDE_RIGHT | BES_BLE_LOC_FRONT_RIGHT)) &&
                pStreamEnv->stream_info.captureInfo.codec_info.num_channels == AUD_CHANNEL_NUM_2)
            {
                // shift to right channel
                channel_shift += payload_len_per_channel;
            }
        }
        else// if (audio_allocation_cnt == AUD_CHANNEL_NUM_2)
        {
            payload_len_per_channel = payload_size;
            channel_shift = 0;
        }

        /// gaf custom, may be a watch point to put some pattern in encoded packet to be send
        if (gaf_uc_srv_custom_data_callback->encoded_packet_send_cb)
        {
            gaf_uc_srv_custom_data_callback->encoded_packet_send_cb((uint8_t *)payload + channel_shift, payload_len_per_channel);
        }

        LOG_D("channel_shift: %d, payload_len_per_channel: %d", channel_shift, payload_len_per_channel);
        bes_ble_bap_iso_dp_send_data(pStreamEnv->stream_info.captureInfo.aseChInfo[i].ase_handle,
                                pStreamEnv->stream_context.latestCaptureSeqNum,
                                (uint8_t *)payload + channel_shift, payload_len_per_channel,
                                ref_time);
    }
}

static uint8_t *gaf_stream_captured_data_pre_processing_handler(GAF_AUDIO_STREAM_ENV_T* pStreamEnv, uint8_t* ptrBuf, uint32_t* pLength)
{
    uint32_t length = *pLength;
    int32_t POSSIBLY_UNUSED *pcm_buf = (int32_t *)ptrBuf;
    uint32_t POSSIBLY_UNUSED pcm_len = length / sizeof(int32_t);
    uint32_t POSSIBLY_UNUSED frame_len = pcm_len / g_capture_ch_num;

#ifdef ANC_ASSIST_LE_AUDIO_SUPPORT
    if (!gaf_stream_process_context_is_live(pStreamEnv->stream_info.bap_contextType)) {
#if defined (VOICE_ASSIST_FIR_LMS_ENABLED) || defined (VOICE_ASSIST_FF_FIR_LMS)
        uint32_t anc_assist_len = length * (AUD_SAMPRATE_32000 / g_capture_sample_rate);
        uint32_t data_len = length / sizeof(float);

        if (app_anc_assist_is_runing()) {
            if (g_anc_assist_iir_resample_st != NULL) {
                float *temp_anc = (float *)g_anc_assist_interval_buf;
                iir_resample_process(g_anc_assist_iir_resample_st, ptrBuf, temp_anc, data_len);
                app_anc_assist_process(temp_anc, anc_assist_len);
            } else {
                app_anc_assist_process(ptrBuf, anc_assist_len);
            }
        }
#else
        uint32_t anc_assist_len = length / (g_capture_sample_rate / AUD_SAMPRATE_16000);
        uint32_t frame_len = length / g_capture_ch_num / sizeof(float);
        float *temp_anc = (float *)g_anc_assist_interval_buf;

        if(app_anc_assist_is_runing()) {
            for (uint8_t ch = 0; ch < g_capture_ch_num; ch++) { //BES intentional code. g_capture_ch_num will never be 0.
                for (uint32_t i = 0; i < frame_len; i++) {
                    temp_anc[ch * frame_len + i] = (float)pcm_buf[i * g_capture_ch_num + ch];
                }
            }

            anc_assist_resample_process(temp_anc, frame_len, frame_len);

            if (g_capture_sample_rate != AUD_SAMPRATE_16000) {
                for (uint8_t ch = 0; ch < g_capture_ch_num; ch++) { //BES intentional code. g_capture_ch_num will never be 0.
                    for (uint32_t i = 0; i < (frame_len / (g_capture_sample_rate / AUD_SAMPRATE_16000)); i++) {
                        temp_anc[ch * (frame_len / (g_capture_sample_rate / AUD_SAMPRATE_16000)) + i] = temp_anc[ch * frame_len + i];
                    }
                }
            }
            //BES intentional code. g_capture_ch_num will never be 0.
            app_anc_assist_process_interval(temp_anc, anc_assist_len);
        }
#endif
        app_anc_assist_parser_app_mic_buf(ptrBuf, &length);
    }
#endif

#ifdef STREAM_PROCESS_ENABLE
    length = gaf_stream_process_capture_run(ptrBuf, length);
#else
    for (uint32_t i = 0; i < frame_len; i++) {
        pcm_buf[i] = pcm_buf[i * g_speech_algo_ch_num];
    }
    length /= g_speech_algo_ch_num;
#endif

#ifndef STREAM_PROCESS_ENABLE_USE_CC_CAPTURE
    if (pStreamEnv->stream_info.captureInfo.codec_info.num_channels > 1) {
        uint32_t codec_chs = pStreamEnv->stream_info.captureInfo.codec_info.num_channels;
        for (uint32_t ch = 0; ch < codec_chs; ch++) {
            for (uint32_t i = 0; i < frame_len; i++) {
                pcm_buf[i * codec_chs + ch] = pcm_buf[i];
            }
            length *= codec_chs;
        }
    }
#endif

    length = gaf_stream_resample_capture_up_run(&ptrBuf, length);

#if defined(SPEECH_BONE_SENSOR) && defined(GAF_ENCODER_CROSS_CORE_USE_M55)
    if (g_capture_vpu_enabled) {
        speech_bone_sensor_get_data(g_capture_combo_vpu_pcm_buf, frame_len, SPEECH_BS_SINGLE_CHANNEL, 24);
        for (int32_t i=frame_len-1; i>=0; i--) {
            g_capture_combo_vpu_pcm_buf[i * (g_speech_algo_ch_num + 1) + g_speech_algo_ch_num] = g_capture_combo_vpu_pcm_buf[i];
        }

        for (uint32_t ch=0; ch<g_speech_algo_ch_num; ch++) {
            for (uint32_t i=0; i<frame_len; i++) {
                g_capture_combo_vpu_pcm_buf[i * (g_speech_algo_ch_num + 1) + ch] = pcm_buf[i * g_speech_algo_ch_num + ch];
            }
        }

        ptrBuf = (uint8_t *)g_capture_combo_vpu_pcm_buf;
        length = (length / g_speech_algo_ch_num) * (g_speech_algo_ch_num + 1);
    }
#endif

    *pLength = length;

    return ptrBuf;
}

static uint32_t _gaf_stream_flexible_capture_dma_irq_handler(uint8_t con_lid, uint8_t* ptrBuf, uint32_t length)
{
    LOG_D("%s start,length = %d", __func__,length);
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = NULL;

    pStreamEnv = &gaf_audio_stream_env[con_lid];
    uint8_t muted = bes_ble_arc_get_mic_state(pStreamEnv->stream_info.con_lid);
    if (muted) {
        memset((uint8_t*)ptrBuf, 0x0, length);
    }

    if (!gaf_stream_is_capture_stream_iso_created(pStreamEnv))
    {
        return length;
    }

    uint32_t dmaIrqHappeningTimeUs = 0;
    uint8_t adma_ch = HAL_DMA_CHAN_NONE;
    uint32_t dma_base;
    uint32_t btclk; //hal slot -- 312.5us
    uint16_t btcnt; //hal microsecond -- 0.5 us
    uint8_t *output_buf = pStreamEnv->stream_context.capture_frame_cache;
    POSSIBLY_UNUSED CODEC_CORE_INFO_T *coreInfo = &pStreamEnv->stream_info.captureInfo.aseChInfo[0].codec_core_info;
    uint32_t frame_len = (uint32_t)(pStreamEnv->stream_info.captureInfo.codec_info.frame_size *
                pStreamEnv->stream_info.captureInfo.codec_info.num_channels);

    adma_ch = af_stream_get_dma_chan(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
    dma_base = af_stream_get_dma_base_addr(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
    if (adma_ch != HAL_DMA_CHAN_NONE)
    {
        bt_drv_reg_op_dma_tc_clkcnt_get_by_ch(&btclk, &btcnt,adma_ch&0xFF, dma_base);
        dmaIrqHappeningTimeUs = bt_syn_ble_bt_time_to_bts(btclk, btcnt);
    }

    if ((GAF_CAPTURE_STREAM_START_TRIGGERING > pStreamEnv->stream_context.capture_stream_state)) {
        memset_s(ptrBuf, length, 0x00, length);
        return length;
    } else if (GAF_CAPTURE_STREAM_START_TRIGGERING == pStreamEnv->stream_context.capture_stream_state) {
        gaf_stream_common_update_capture_stream_state(pStreamEnv, GAF_CAPTURE_STREAM_STREAMING_TRIGGERED);
        gaf_stream_common_clr_trigger(pStreamEnv->stream_context.captureTriggerChannel);
        uint32_t latest_iso_bt_time = gaf_media_common_get_latest_tx_iso_evt_timestamp(pStreamEnv);
        if (!pStreamEnv->stream_context.isUsSinceLatestAnchorPointConfigured)
        {
            pStreamEnv->stream_context.isUsSinceLatestAnchorPointConfigured = true;
            pStreamEnv->stream_context.usSinceLatestAnchorPoint = GAF_AUDIO_CLK_32_BIT_DIFF(dmaIrqHappeningTimeUs, latest_iso_bt_time);
            LOG_I("initial usSinceAnch %d", pStreamEnv->stream_context.usSinceLatestAnchorPoint);
            LOG_I("anch time %d dma irq time %d", latest_iso_bt_time, dmaIrqHappeningTimeUs);
            gaf_stream_common_sync_us_since_latest_anchor_point(pStreamEnv);
        }
    }

    // it's possible the multiple DMA irq triggered message accumulates,
    // so the acuiqred dmaIrqHappeningTimeUs is the last dma irq, which has
    // been handled. For this case, ignore this dma irq message handling
    if ((GAF_CAPTURE_STREAM_STREAMING_TRIGGERED ==
        pStreamEnv->stream_context.capture_stream_state) &&
        (dmaIrqHappeningTimeUs ==
        pStreamEnv->stream_context.lastCaptureDmaIrqTimeUs))
    {
        LOG_W("accumulated irq messages happen!");
        return length;
    }

    gaf_stream_common_capture_timestamp_checker(pStreamEnv, dmaIrqHappeningTimeUs);

    LOG_D("%s length %d encoded_len %d filled timestamp %u", __func__, length,
        pStreamEnv->stream_info.captureInfo.codec_info.frame_size,
        dmaIrqHappeningTimeUs);

    /// gaf custom, may be a watch point to put some pattern in pcm raw data to be encode
    if (gaf_uc_srv_custom_data_callback->raw_pcm_data_cb)
    {
        gaf_uc_srv_custom_data_callback->raw_pcm_data_cb(ptrBuf, length);
    }

#ifdef IS_BLE_AUDIO_DEBUG_INFO_COLLECTOR_ENABLED
    if (NULL != capture_dma_irq_info_record_handler)
    {
        capture_dma_irq_info_record_handler(pStreamEnv->stream_info.con_lid, dmaIrqHappeningTimeUs,
                                            pStreamEnv->stream_context.latestCaptureSeqNum);
    }
#endif

#if defined (GAF_CODEC_CROSS_CORE) || defined (AOB_CODEC_CP)
    if (pStreamEnv->stream_context.isUpStreamingStarted)
    {
        // bth fetch encoded data
        gaf_stream_common_fetch_frame(pStreamEnv, output_buf,
            frame_len, dmaIrqHappeningTimeUs, coreInfo->instance_handle);
        LOG_D("%s frame_len %d ", __func__, frame_len);
        gaf_stream_capture_dma_irq_handler_send(pStreamEnv, output_buf, frame_len, dmaIrqHappeningTimeUs);
    }

    ptrBuf = gaf_stream_captured_data_pre_processing_handler(pStreamEnv, ptrBuf, &length);
    gaf_stream_common_store_pcm(pStreamEnv, dmaIrqHappeningTimeUs, ptrBuf, length, frame_len, coreInfo);
#else
    ptrBuf = gaf_stream_captured_data_pre_processing_handler(pStreamEnv, ptrBuf, &length);

    pStreamEnv->func_list->encoder_func_list->encoder_encode_frame_func(0,
        &(pStreamEnv->stream_info.captureInfo.codec_info), length, ptrBuf, frame_len, output_buf);

    gaf_stream_capture_dma_irq_handler_send(pStreamEnv, output_buf, frame_len, dmaIrqHappeningTimeUs);
#endif
    LOG_D("%s end", __func__);
    return length;
}

static uint32_t gaf_stream_flexible_capture_idx_0_dma_irq_handler(uint8_t* ptrBuf, uint32_t length)
{
    return _gaf_stream_flexible_capture_dma_irq_handler(0, ptrBuf, length);
}

static uint32_t gaf_stream_flexible_capture_idx_1_dma_irq_handler(uint8_t* ptrBuf, uint32_t length)
{
    return _gaf_stream_flexible_capture_dma_irq_handler(1, ptrBuf, length);
}

static uint32_t gaf_stream_flexible_capture_idx_2_dma_irq_handler(uint8_t* ptrBuf, uint32_t length)
{
    return _gaf_stream_flexible_capture_dma_irq_handler(2, ptrBuf, length);
}

static void gaf_audio_flexible_capture_buf_deinit(void* _pStreamEnv)
{
    LOG_D("%s start", __func__);
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T *)_pStreamEnv;

    pStreamEnv->stream_info.captureInfo.dma_info.dmaBufPtr = NULL;

    if (pStreamEnv->func_list->encoder_func_list)
    {
        pStreamEnv->func_list->encoder_func_list->encoder_deinit_buf_func(0);
    }
    LOG_I("[%s] syspool free size: %d/%d", __func__, syspool_free_size(), syspool_total_size());
    LOG_D("%s end", __func__);
}

static GAF_AUDIO_FUNC_LIST_T gaf_audio_flexible_stream_func_list[GAF_MAXIMUM_CONNECTION_COUNT] =
{
    {
        {
            .playback_dma_irq_handler_func = gaf_stream_flexible_playback_idx_0_dma_irq_handler,
            .capture_dma_irq_handler_func = gaf_stream_flexible_capture_idx_0_dma_irq_handler,

            .playback_start_stream_func = gaf_audio_flexible_playback_stream_start_handler,
            .playback_init_stream_buf_func = gaf_audio_flexible_playback_buf_init,
            .playback_stop_stream_func = gaf_audio_flexible_playback_stream_stop_handler,
            .playback_deinit_stream_buf_func = gaf_audio_flexible_playback_buf_deinit,

            .capture_start_stream_func = gaf_audio_flexible_capture_stream_start_handler,
            .capture_init_stream_buf_func = gaf_audio_flexible_capture_buf_init,
            .capture_stop_stream_func = gaf_audio_flexible_capture_stream_stop_handler,
            .capture_deinit_stream_buf_func = gaf_audio_flexible_capture_buf_deinit,
        }
    },

    {
        {
            .playback_dma_irq_handler_func = gaf_stream_flexible_playback_idx_1_dma_irq_handler,
            .capture_dma_irq_handler_func = gaf_stream_flexible_capture_idx_1_dma_irq_handler,

            .playback_start_stream_func = gaf_audio_flexible_playback_stream_start_handler,
            .playback_init_stream_buf_func = gaf_audio_flexible_playback_buf_init,
            .playback_stop_stream_func = gaf_audio_flexible_playback_stream_stop_handler,
            .playback_deinit_stream_buf_func = gaf_audio_flexible_playback_buf_deinit,

            .capture_start_stream_func = gaf_audio_flexible_capture_stream_start_handler,
            .capture_init_stream_buf_func = gaf_audio_flexible_capture_buf_init,
            .capture_stop_stream_func = gaf_audio_flexible_capture_stream_stop_handler,
            .capture_deinit_stream_buf_func = gaf_audio_flexible_capture_buf_deinit,
        }
    },

    {
        {
            .playback_dma_irq_handler_func = gaf_stream_flexible_playback_idx_2_dma_irq_handler,
            .capture_dma_irq_handler_func = gaf_stream_flexible_capture_idx_2_dma_irq_handler,

            .playback_start_stream_func = gaf_audio_flexible_playback_stream_start_handler,
            .playback_init_stream_buf_func = gaf_audio_flexible_playback_buf_init,
            .playback_stop_stream_func = gaf_audio_flexible_playback_stream_stop_handler,
            .playback_deinit_stream_buf_func = gaf_audio_flexible_playback_buf_deinit,

            .capture_start_stream_func = gaf_audio_flexible_capture_stream_start_handler,
            .capture_init_stream_buf_func = gaf_audio_flexible_capture_buf_init,
            .capture_stop_stream_func = gaf_audio_flexible_capture_stream_stop_handler,
            .capture_deinit_stream_buf_func = gaf_audio_flexible_capture_buf_deinit,
        }
    },
};

static GAF_AUDIO_STREAM_ENV_T* gaf_audio_get_stream_env_from_ase(uint8_t ase_lid)
{
    const bes_ble_bap_ascs_ase_t *p_bap_ase_info = bes_ble_get_ascs_ase_info(ase_lid);

    return &gaf_audio_stream_env[p_bap_ase_info->con_lid];
}

static void gaf_audio_start_sink_trigger_by_cis_cig_delay(GAF_AUDIO_STREAM_ENV_T *pStreamEnv, const bes_ble_bap_ascs_ase_t *p_ase_info)
{
    uint32_t trigger_bt_time = 0;
    uint32_t current_bt_time = gaf_media_sync_get_curr_time();
    uint8_t *playback_ase_lid_list = gaf_audio_get_playback_ase_index_list(pStreamEnv->stream_info.con_lid);

    if (p_ase_info == NULL)
    {
        p_ase_info = bes_ble_get_ascs_ase_info(playback_ase_lid_list[0]);
    }

    if (p_ase_info == NULL)
    {
        LOG_E("%s (%d) no valid ase", __func__, pStreamEnv->stream_info.con_lid);
        return;
    }

    // uint32_t lock = int_lock();

    if (GAF_PLAYBACK_STREAM_INITIALIZED != pStreamEnv->stream_context.playback_stream_state)
    {
        // int_unlock(lock);
        return;
    }

    LOG_I("[%d] ase_lid: %d cig_sync_delay: %u, curr_us: %u", p_ase_info->con_lid,
                                            p_ase_info->ase_lid, p_ase_info->cig_sync_delay, current_bt_time);

    if (p_ase_info->cig_sync_delay)
    {
        trigger_bt_time = gaf_media_common_get_latest_rx_iso_evt_timestamp(pStreamEnv) +
                                p_ase_info->cig_sync_delay + pStreamEnv->stream_info.playbackInfo.presDelayUs;
        gaf_stream_common_set_playback_trigger_time(pStreamEnv, trigger_bt_time);
    }

    // int_unlock(lock);
}

void gaf_audio_update_stream_iso_hdl(uint8_t ase_lid)
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = gaf_audio_get_stream_env_from_ase(ase_lid);
    if (!pStreamEnv)
    {
        LOG_E("[UPDATE ISO HDL] ase lid %d with no stream info now, please check!!!", ase_lid);
        return;
    }

    const bes_ble_bap_ascs_ase_t *p_bap_ase_info = bes_ble_get_ascs_ase_info(ase_lid);
    uint8_t aseChnInfoIdx = p_bap_ase_info->ase_lid % GAF_AUDIO_ASE_TOTAL_COUNT;
    GAF_AUDIO_STREAM_COMMON_INFO_T* pCommonInfo;
    if (BES_BLE_GAF_DIRECTION_SINK == p_bap_ase_info->direction)
    {
        pCommonInfo = &(pStreamEnv->stream_info.playbackInfo);
    }
    else
    {
        pCommonInfo = &(pStreamEnv->stream_info.captureInfo);
    }

    pCommonInfo->aseChInfo[aseChnInfoIdx].iso_channel_hdl = BLE_ISOHDL_TO_ACTID(p_bap_ase_info->cis_hdl);
    pCommonInfo->aseChInfo[aseChnInfoIdx].ase_handle = p_bap_ase_info->cis_hdl;
    LOG_I("[UPDATE ISO HDL] ase lid %d cis hdl %d con_lid %d channel %d",
        ase_lid, pCommonInfo->aseChInfo[aseChnInfoIdx].ase_handle,
        p_bap_ase_info->con_lid, pCommonInfo->aseChInfo[aseChnInfoIdx].iso_channel_hdl);

    /// CIS connected and check for capture trigger set
    if (BES_BLE_GAF_DIRECTION_SRC == p_bap_ase_info->direction &&
        pStreamEnv->stream_context.capture_stream_state == GAF_CAPTURE_STREAM_INITIALIZED)
    {
        gaf_stream_common_start_sync_capture(pStreamEnv);
    }
    /// CIS connected and check for playback trigger set
    else if (BES_BLE_GAF_DIRECTION_SINK == p_bap_ase_info->direction &&
        pStreamEnv->stream_context.playback_stream_state == GAF_PLAYBACK_STREAM_INITIALIZED)
    {
        gaf_audio_start_sink_trigger_by_cis_cig_delay(pStreamEnv, p_bap_ase_info);
    }
}

POSSIBLY_UNUSED static GAF_AUDIO_STREAM_ENV_T* gaf_audio_refresh_stream_info_from_ase(uint8_t ase_lid)
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = gaf_audio_get_stream_env_from_ase(ase_lid);
    if (!pStreamEnv)
    {
        LOG_W("ase is not ready yet for refresh stream info from");
        return NULL;
    }
    const bes_ble_bap_ascs_ase_t *p_bap_ase_info = bes_ble_get_ascs_ase_info(ase_lid);
    GAF_AUDIO_STREAM_COMMON_INFO_T* pCommonInfo;
    uint8_t audio_alloc_l_r_cnt = 0;
    uint8_t aseChnInfoIdx = p_bap_ase_info->ase_lid % GAF_AUDIO_ASE_TOTAL_COUNT;
    uint8_t loc_supp_audio_location = 0;
    uint32_t sample_bytes = 0;

    pStreamEnv->stream_info.bap_contextType = p_bap_ase_info->p_metadata->param.context_bf;
    LOG_I("[%s] TYPE bap: %d, app: %d", __func__, pStreamEnv->stream_info.bap_contextType, pStreamEnv->stream_info.contextType);

    if (BES_BLE_GAF_DIRECTION_SINK == p_bap_ase_info->direction)
    {
        loc_supp_audio_location = bes_ble_bap_capa_get_location_bf(BES_BLE_GAF_DIRECTION_SINK);
        pCommonInfo = &(pStreamEnv->stream_info.playbackInfo);
    }
    else
    {
        loc_supp_audio_location = bes_ble_bap_capa_get_location_bf(BES_BLE_GAF_DIRECTION_SRC);
        pCommonInfo = &(pStreamEnv->stream_info.captureInfo);
    }
    /// Check for this stream's playback or capture info is refreshed
    if (pCommonInfo->aseChInfo[aseChnInfoIdx].allocation_bf != 0 &&
                pCommonInfo->codec_info.frame_ms != 0)
    {
        LOG_I("already been refreshed %s stream info, allocation 0x%x via ase lid: %d",
              (p_bap_ase_info->direction == BES_BLE_GAF_DIRECTION_SINK ? "playback" : "capture"),
              pCommonInfo->aseChInfo[aseChnInfoIdx].allocation_bf,
              ase_lid);
        return pStreamEnv;
    }
    /// update stream common info
    pCommonInfo->codec_info.num_channels = bes_ble_audio_get_location_fs_l_r_cnt(loc_supp_audio_location);
    pCommonInfo->cigSyncDelayUs = p_bap_ase_info->cig_sync_delay;
    pCommonInfo->isoIntervalUs = p_bap_ase_info->iso_interval_us;
    pCommonInfo->bnM2S = p_bap_ase_info->bn_m2s;
    pCommonInfo->bnS2M = p_bap_ase_info->bn_s2m;
    pCommonInfo->codec_info.bits_depth = GAF_AUDIO_STREAM_BIT_NUM;
#if defined (AF_DELAY_HANDLE_DMA_IRQ_SUPPORT) && defined (AOB_LOW_LATENCY_MODE)
    gaf_stream_common_set_custom_presdelay_us(GAF_STREAM_DELAY_GET_PCM_PADDING_US + GAF_AUDIO_DECODER_TIME);
    #endif
    pCommonInfo->presDelayUs = gaf_stream_common_get_custom_presdelay_us();
    if (pCommonInfo->presDelayUs == 0)
    {
        pCommonInfo->presDelayUs = p_bap_ase_info->qos_cfg.pres_delay_us > GAF_AUDIO_CONTROLLER_2_HOST_LATENCY_US ?
        p_bap_ase_info->qos_cfg.pres_delay_us : GAF_AUDIO_CONTROLLER_2_HOST_LATENCY_US;
    }
    pCommonInfo->aseChInfo[aseChnInfoIdx].iso_channel_hdl = BLE_ISOHDL_TO_ACTID(p_bap_ase_info->cis_hdl);
    pCommonInfo->aseChInfo[aseChnInfoIdx].ase_handle = p_bap_ase_info->cis_hdl;
    pCommonInfo->aseChInfo[aseChnInfoIdx].allocation_bf = p_bap_ase_info->p_cfg->param.location_bf;

    audio_alloc_l_r_cnt = bes_ble_audio_get_location_fs_l_r_cnt(p_bap_ase_info->p_cfg->param.location_bf);
    /// multi channel allocation or no specified allocation check
    if ((audio_alloc_l_r_cnt >= AUD_CHANNEL_NUM_2 && p_bap_ase_info->qos_cfg.max_sdu_size ==
         p_bap_ase_info->p_cfg->param.frame_octet * p_bap_ase_info->p_cfg->param.frames_sdu) ||
        /// absence of audio allocation, value == 0
        (audio_alloc_l_r_cnt == 0) ||
        /// Sanity check
        (audio_alloc_l_r_cnt > pCommonInfo->codec_info.num_channels))
    {
        if (BES_BLE_AUDIO_TWS_SLAVE == bes_ble_audio_get_tws_nv_role())
        {
            pCommonInfo->aseChInfo[aseChnInfoIdx].allocation_bf = BES_BLE_LOC_FRONT_LEFT;
        }
        else
        {
            pCommonInfo->aseChInfo[aseChnInfoIdx].allocation_bf = BES_BLE_LOC_FRONT_RIGHT;
        }

        LOG_W("use mono channel with allocation: 0x%x",
              pCommonInfo->aseChInfo[aseChnInfoIdx].allocation_bf);
    }

    LOG_I("ase_id:%d direction:%d codec_id:%d real presDelayus:%u cigDelay:%u", ase_lid,
        p_bap_ase_info->direction, p_bap_ase_info->codec_id.codec_id[0], pCommonInfo->presDelayUs,
        pCommonInfo->cigSyncDelayUs);

    switch (p_bap_ase_info->codec_id.codec_id[0])
    {
        /// todo audio progress for row pcm: BES_BLE_GAF_CODEC_TYPE_PCM
        case BES_BLE_GAF_CODEC_TYPE_LC3:
        {
            AOB_BAP_CFG_T* p_lc3_cfg = (AOB_BAP_CFG_T *)p_bap_ase_info->p_cfg;
            pCommonInfo->codec_info.frame_ms =
                gaf_stream_common_frame_duration_parse(p_lc3_cfg->param.frame_dur);
            pCommonInfo->codec_info.sample_rate =
                gaf_stream_common_sample_freq_parse(p_lc3_cfg->param.sampling_freq);
            pCommonInfo->codec_info.frame_size = p_lc3_cfg->param.frame_octet;
            pCommonInfo->maxCachedFrameCount = GAF_AUDIO_MEDIA_DATA_PACKET_NUM_LIMITER;
            pCommonInfo->maxFrameSize = gaf_audio_lc3_encoder_get_max_frame_size();
            pStreamEnv->stream_info.codec_type = LC3;
#if !defined(GAF_CODEC_CROSS_CORE) && !defined(AOB_CODEC_CP)
            gaf_audio_lc3_update_decoder_func_list(&(pStreamEnv->func_list->decoder_func_list));
            gaf_audio_lc3_update_encoder_func_list(&(pStreamEnv->func_list->encoder_func_list));
#endif
            break;
        }
        case BES_BLE_GAF_CODEC_TYPE_VENDOR:
        {
            AOB_BAP_CFG_T* p_vend_cfg = (AOB_BAP_CFG_T *)p_bap_ase_info->p_cfg;
            pCommonInfo->codec_info.frame_ms =
                gaf_stream_common_frame_duration_parse(p_vend_cfg->param.frame_dur);
            pCommonInfo->codec_info.sample_rate =
                gaf_stream_common_sample_freq_parse(p_vend_cfg->param.sampling_freq);
            pCommonInfo->maxCachedFrameCount = GAF_AUDIO_MEDIA_DATA_PACKET_NUM_LIMITER;
            pCommonInfo->codec_info.frame_size = p_vend_cfg->param.frame_octet;
#ifdef LC3PLUS_SUPPORT
            if (app_bap_codec_is_lc3plus(p_bap_ase_info->codec_id.codec_id)) {
                pCommonInfo->maxFrameSize = gaf_audio_lc3_encoder_get_max_frame_size();
                pStreamEnv->stream_info.codec_type = LC3PLUS;
#if !defined(GAF_CODEC_CROSS_CORE) && !defined(AOB_CODEC_CP)
                gaf_audio_lc3_update_decoder_func_list(&(pStreamEnv->func_list->decoder_func_list));
                gaf_audio_lc3_update_encoder_func_list(&(pStreamEnv->func_list->encoder_func_list));
#endif
            }
#endif // LC3PLUS_SUPPORT
#ifdef LEA_LHDC
            if (app_bap_codec_is_lhdc(p_bap_ase_info->codec_id.codec_id)) {
                pCommonInfo->maxFrameSize = gaf_audio_lhdc_encoder_get_max_frame_size();
                pCommonInfo->codec_info.bits_depth = AUD_BITS_16;
                pStreamEnv->stream_info.codec_type = LHDC;
#if !defined(GAF_CODEC_CROSS_CORE) && !defined(AOB_CODEC_CP)
                gaf_audio_lhdc_update_decoder_func_list(&(pStreamEnv->func_list->decoder_func_list));
                gaf_audio_lhdc_update_encoder_func_list(&(pStreamEnv->func_list->encoder_func_list));
#endif
            }
#endif // LEA_LHDC
            /// 2.5ms * GAF_AUDIO_MEDIA_DATA_PACKET_NUM_LIMITER <= BAP Presdelay
            if (p_vend_cfg->param.frame_dur == BES_BLE_GAF_BAP_FRAME_DURATION_2_5MS)
            {
                pCommonInfo->maxCachedFrameCount = pCommonInfo->maxCachedFrameCount*2;
                pCommonInfo->maxFrameSize = pCommonInfo->maxFrameSize/2;
            }
            break;
        }
#ifdef HID_ULL_ENABLE
        case BES_BLE_GAF_CODEC_TYPE_ULL:
        {
            AOB_BAP_CFG_T* p_ull_cfg = p_bap_ase_info->p_cfg;
            pCommonInfo->codec_info.frame_ms =
                gaf_stream_common_frame_duration_parse(p_ull_cfg->param.frame_dur);
            pCommonInfo->codec_info.sample_rate =
                gaf_stream_common_sample_freq_parse(p_ull_cfg->param.sampling_freq);
            pCommonInfo->codec_info.frame_size = p_ull_cfg->param.frame_octet;
            pCommonInfo->maxCachedFrameCount = GAF_AUDIO_MEDIA_DATA_PACKET_NUM_LIMITER;
            pCommonInfo->maxFrameSize = gaf_audio_lc3_encoder_get_max_frame_size();
            pStreamEnv->stream_info.codec_type = ULL;
#if !defined(GAF_CODEC_CROSS_CORE) && !defined(AOB_CODEC_CP)
            gaf_audio_lc3_update_decoder_func_list(&(pStreamEnv->func_list->decoder_func_list));
            gaf_audio_lc3_update_encoder_func_list(&(pStreamEnv->func_list->encoder_func_list));
#endif
            break;
        }
#endif
        default:
            ASSERT(false, "unknown codec type!");
            return NULL;
    }

    sample_bytes = (pCommonInfo->codec_info.bits_depth / 8);
#ifdef __BLE_AUDIO_24BIT__
    sample_bytes = sizeof(int32_t);
#endif

#if (defined(GAF_CODEC_CROSS_CORE) || defined(AOB_CODEC_CP)) && defined(BLE_AUDIO_STEREO_CHAN_OVER_CIS_CNT)
    pCommonInfo->codec_info.num_channels = audio_alloc_l_r_cnt;
#endif

    pCommonInfo->codec_info.pcm_size =
        (uint32_t)(((pCommonInfo->codec_info.frame_ms * 1000)
        * pCommonInfo->codec_info.sample_rate * sample_bytes *
        pCommonInfo->codec_info.num_channels) / (1000 * 1000));

    pCommonInfo->dma_info.num_channels = pCommonInfo->codec_info.num_channels;
#ifdef BLE_AUDIO_STEREO_CHAN_OVER_CIS_CNT
    if (BES_BLE_GAF_DIRECTION_SINK == p_bap_ase_info->direction) {
        pCommonInfo->dma_info.num_channels = 2;
    }
#endif

#ifdef PLAYBACK_USE_I2S
    pCommonInfo->dma_info.num_channels = 2;
#endif

    pCommonInfo->dma_info.bits_depth = pCommonInfo->codec_info.bits_depth;
    pCommonInfo->dma_info.frame_ms = pCommonInfo->codec_info.frame_ms;
    pCommonInfo->dma_info.sample_rate = pCommonInfo->codec_info.sample_rate;
    pCommonInfo->dma_info.dmaChunkIntervalUs = (uint32_t)(pCommonInfo->dma_info.frame_ms * 1000);
    pCommonInfo->dma_info.dmaChunkSize = (uint32_t)(((uint64_t)pCommonInfo->codec_info.sample_rate *
        sample_bytes * pCommonInfo->dma_info.dmaChunkIntervalUs *
        pCommonInfo->dma_info.num_channels) / (1000 * 1000));

    LOG_I("sample_rate = %d bytes = %d chnls = %d",pCommonInfo->codec_info.sample_rate, sample_bytes, pCommonInfo->dma_info.num_channels);
    LOG_I("isoIntervalUs:%d qos presDelayus:%u cigDelay:%u, bn m2s:%u s2m:%u appCtxType:%d",
         p_bap_ase_info->iso_interval_us, p_bap_ase_info->qos_cfg.pres_delay_us,
        p_bap_ase_info->cig_sync_delay, pCommonInfo->bnM2S, pCommonInfo->bnS2M, pStreamEnv->stream_info.contextType);
    LOG_I("ase lid %d cis hdl %d con_lid %d channel %d", ase_lid, pCommonInfo->aseChInfo[aseChnInfoIdx].ase_handle,
        p_bap_ase_info->con_lid, pCommonInfo->aseChInfo[aseChnInfoIdx].iso_channel_hdl);
    LOG_I("frame len %d us, sample rate %d dma chunk time %d us dma chunk size %d",
        (uint32_t)(pCommonInfo->codec_info.frame_ms*1000), pCommonInfo->dma_info.sample_rate,
        pCommonInfo->dma_info.dmaChunkIntervalUs, pCommonInfo->dma_info.dmaChunkSize);
    LOG_I("num_channel %d codec %d", pCommonInfo->dma_info.num_channels, pCommonInfo->codec_info.num_channels);
    LOG_I("allocation: 0x%x frame_size %d", pCommonInfo->aseChInfo[aseChnInfoIdx].allocation_bf, pCommonInfo->codec_info.frame_size);
    LOG_I("context: %s",
        gaf_stream_common_print_context(p_bap_ase_info->p_metadata->param.context_bf));
    LOG_I("codec: %s", gaf_stream_common_print_code_type(pStreamEnv->stream_info.codec_type));

    return pStreamEnv;
}

bool gaf_audio_is_capture_stream_on(void)
{
    GAF_AUDIO_STREAM_ENV_T *pStreamEnv = NULL;
    for (uint8_t con_lid = 0; con_lid < GAF_MAXIMUM_CONNECTION_COUNT; con_lid++)
    {
        pStreamEnv = &gaf_audio_stream_env[con_lid];

        if (pStreamEnv->stream_context.capture_stream_state != GAF_CAPTURE_STREAM_IDLE) {
            return true;
        }
    }

    return false;
}

#ifdef MIX_AUDIO_PROMPT_WITH_A2DP_MEDIA_ENABLED
uint32_t gaf_get_ble_audio_playback_sample_rate(void)
{
    return gaf_audio_stream_env[0]->stream_info.playbackInfo.dma_info.sample_rate;
}
#endif

bool gaf_audio_is_playback_stream_on(void)
{
    GAF_AUDIO_STREAM_ENV_T *pStreamEnv = NULL;
    GAF_MEDIA_DWELLING_INFO_T* pDewellingInfo = NULL;
    for (uint8_t con_lid = 0; con_lid < GAF_MAXIMUM_CONNECTION_COUNT; con_lid++)
    {
        pStreamEnv = &gaf_audio_stream_env[con_lid];

        if (pStreamEnv->stream_context.capture_stream_state != GAF_CAPTURE_STREAM_IDLE ||
            pStreamEnv->stream_context.playback_stream_state != GAF_PLAYBACK_STREAM_IDLE)
        {
            pDewellingInfo = &gaf_cis_media_dwelling_info[con_lid];

            if (GAF_AUDIO_STREAM_TYPE_PLAYBACK & pDewellingInfo->startedStreamTypes ||
                GAF_AUDIO_STREAM_TYPE_FLEXIBLE & pDewellingInfo->startedStreamTypes)
            {
                return true;
            }
        }
    }

    return false;
}

gaf_stream_context_state_t gaf_audio_update_stream_state_info_from_ase(
                            GAF_AUDIO_UPDATE_STREAM_INFO_PURPOSE_E purpose, uint8_t ase_lid)
{
    const bes_ble_bap_ascs_ase_t *p_bap_ase_info = bes_ble_get_ascs_ase_info(ase_lid);

    LOG_I("update stream ase %d purpose %d", ase_lid, purpose);

    GAF_MEDIA_DWELLING_INFO_T* pDewellingInfo =
        &gaf_cis_media_dwelling_info[p_bap_ase_info->con_lid];

    if (BES_BLE_GAF_DIRECTION_SINK == p_bap_ase_info->direction)
    {
        if (GAF_AUDIO_UPDATE_STREAM_INFO_TO_START == purpose)
        {
            gaf_audio_add_ase_into_playback_ase_list(p_bap_ase_info->con_lid, ase_lid);
            pDewellingInfo->startedStreamTypes |= GAF_AUDIO_STREAM_TYPE_PLAYBACK;
        }
        else
        {
            gaf_audio_remove_ase_from_playback_ase_list(p_bap_ase_info->con_lid, p_bap_ase_info->ase_lid);
            pDewellingInfo->startedStreamTypes &= (~GAF_AUDIO_STREAM_TYPE_PLAYBACK);
        }
    }
    else
    {
        if (GAF_AUDIO_UPDATE_STREAM_INFO_TO_START == purpose)
        {
            gaf_audio_add_ase_into_capture_ase_list(p_bap_ase_info->con_lid, ase_lid);
            pDewellingInfo->startedStreamTypes |= GAF_AUDIO_STREAM_TYPE_CAPTURE;
        }
        else
        {
            gaf_audio_remove_ase_from_capture_ase_list(p_bap_ase_info->con_lid, p_bap_ase_info->ase_lid);
            pDewellingInfo->startedStreamTypes &= (~GAF_AUDIO_STREAM_TYPE_CAPTURE);
        }
    }

    LOG_I("direction %d started stream types updated to %d", p_bap_ase_info->direction,
        pDewellingInfo->startedStreamTypes);

    if (GAF_AUDIO_UPDATE_STREAM_INFO_TO_START == purpose)
    {
        // any direction of stream can start the context
        return APP_GAF_CONTEXT_STREAM_STARTED;
    }
    else
    {
        if (0 == pDewellingInfo->startedStreamTypes)
        {
            return APP_GAF_CONTEXT_ALL_STREAMS_STOPPED;
        }
        else if (0 == (pDewellingInfo->startedStreamTypes & GAF_AUDIO_STREAM_TYPE_CAPTURE))
        {
            return APP_GAF_CONTEXT_CAPTURE_STREAMS_STOPPED;
        }
        else
        {
            return APP_GAF_CONTEXT_PLAYBACK_STREAMS_STOPPED;
        }
    }
}

uint8_t gaf_audio_get_stream_started_type(uint8_t con_lid)
{
    return gaf_cis_media_dwelling_info[con_lid].startedStreamTypes;
}

gaf_stream_context_state_t gaf_audio_stream_update_and_start_handler(uint8_t ase_lid)
{
    LOG_I("%s start", __func__);
    gaf_stream_context_state_t updatedContextStreamState =
        gaf_audio_update_stream_state_info_from_ase(GAF_AUDIO_UPDATE_STREAM_INFO_TO_START, ase_lid);

    gaf_media_stream_boost_freq(GAF_MEDIA_STREAM_POST_START_FREQ_BOOST_TIMEOUT_VALUE);

    return updatedContextStreamState;
}

gaf_stream_context_state_t gaf_audio_stream_update_and_stop_handler(uint8_t ase_lid)
{
    gaf_stream_context_state_t updatedContextStreamState =
        gaf_audio_update_stream_state_info_from_ase(GAF_AUDIO_UPDATE_STREAM_INFO_TO_STOP, ase_lid);

    return updatedContextStreamState;
}

GAF_AUDIO_STREAM_ENV_T* _gaf_refresh_all_stream_info_by_con_lid(uint8_t con_lid)
{
    uint8_t *playback_ase_lid_list = gaf_audio_get_playback_ase_index_list(con_lid);
    uint8_t *capture_ase_lid_list = gaf_audio_get_capture_ase_index_list(con_lid);
    uint8_t idx = 0;
    GAF_AUDIO_STREAM_ENV_T* pPlaybackStreamEnv = NULL;
    GAF_AUDIO_STREAM_ENV_T* pCaptureStreamEnv = NULL;
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = NULL;

    for (idx = 0; idx < GAF_AUDIO_USED_ASE_PER_DIR_MAX_CNT; idx++)
    {
        if (GAF_INVALID_ASE_INDEX != capture_ase_lid_list[idx])
        {
            pStreamEnv = gaf_audio_refresh_stream_info_from_ase(capture_ase_lid_list[idx]);
            pCaptureStreamEnv = (pStreamEnv == NULL) ?
                                        pCaptureStreamEnv : pStreamEnv;
            // No break for update all un refresh stream info
        }
    }

    for (idx = 0; idx < GAF_AUDIO_USED_ASE_PER_DIR_MAX_CNT; idx++)
    {
        if (GAF_INVALID_ASE_INDEX != playback_ase_lid_list[idx])
        {
            pStreamEnv = gaf_audio_refresh_stream_info_from_ase(playback_ase_lid_list[idx]);
            pPlaybackStreamEnv = (pStreamEnv == NULL) ?
                                        pPlaybackStreamEnv : pStreamEnv;
            // No break for update all un refresh stream info
        }
    }

    if (pPlaybackStreamEnv && pCaptureStreamEnv)
    {
        ASSERT(pPlaybackStreamEnv == pCaptureStreamEnv, "Same context should use the same env!");
    }

    if (pPlaybackStreamEnv)
    {
        pPlaybackStreamEnv->stream_info.con_lid = con_lid;
        return pPlaybackStreamEnv;
    }
    else if (pCaptureStreamEnv)
    {
        pCaptureStreamEnv->stream_info.con_lid = con_lid;
        return pCaptureStreamEnv;
    }
    else
    {
        return NULL;
    }
}

static void _gaf_audio_stream_start(uint8_t con_lid, uint8_t ase_lid)
{
    LOG_I("%s start,con_lid = 0x%x,ase_lid = 0x%x", __func__,con_lid, ase_lid);
#ifdef GAF_DSP
    app_dsp_hifi4_init(APP_DSP_HIFI4_USER_GAF_DSP);
#endif
    GAF_MEDIA_DWELLING_INFO_T* pDewellingInfo = &gaf_cis_media_dwelling_info[con_lid];
    uint8_t StreamTypes = pDewellingInfo->startedStreamTypes;
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = _gaf_refresh_all_stream_info_by_con_lid(con_lid);

    if (pStreamEnv)
    {
        if ((GAF_CAPTURE_STREAM_IDLE == pStreamEnv->stream_context.capture_stream_state) &&
            (GAF_PLAYBACK_STREAM_IDLE == pStreamEnv->stream_context.playback_stream_state))
        {
            // bt_adapter_write_sleep_enable(0);
            gaf_stream_register_running_stream_ref(con_lid, pStreamEnv);
            if (gaf_stream_process_context_is_call(pStreamEnv->stream_info.bap_contextType)) {
                app_overlay_select(APP_OVERLAY_SPEECH_ALGO);
            }
        }

        if ((StreamTypes & GAF_AUDIO_STREAM_TYPE_PLAYBACK) &&
            (GAF_PLAYBACK_STREAM_IDLE == pStreamEnv->stream_context.playback_stream_state))
        {
            uint8_t playback_ase_lid = GAF_INVALID_ASE_INDEX;
            uint8_t *playback_ase_lid_list = gaf_audio_get_playback_ase_index_list(con_lid);
            for (uint8_t ase_lid = 0; ase_lid < GAF_AUDIO_USED_ASE_PER_DIR_MAX_CNT; ase_lid++)
            {
                if (GAF_INVALID_ASE_INDEX != playback_ase_lid_list[ase_lid])
                {
                    playback_ase_lid = playback_ase_lid_list[ase_lid];
                    break;
                }
            }
            if (GAF_INVALID_ASE_INDEX == playback_ase_lid) {
                LOG_I("%s invalid playback ase lid", __func__);
                return;
            }

            gaf_stream_common_update_playback_stream_state(pStreamEnv, GAF_PLAYBACK_STREAM_INITIALIZING);
            pStreamEnv->func_list->stream_func_list.playback_start_stream_func(pStreamEnv);
#if defined (GAF_CODEC_CROSS_CORE) || defined (AOB_CODEC_CP)
            gaf_playback_decoder_init(pStreamEnv, playback_ase_lid);
#endif
            if (!pStreamEnv->stream_context.capture_retrigger_onprocess) {
                return;
            }
        }

        if ((StreamTypes & GAF_AUDIO_STREAM_TYPE_CAPTURE) &&
            (GAF_CAPTURE_STREAM_IDLE == pStreamEnv->stream_context.capture_stream_state))
        {
            uint8_t capture_ase_lid = GAF_INVALID_ASE_INDEX;
            uint8_t *capture_ase_lid_list = gaf_audio_get_capture_ase_index_list(con_lid);
            for (uint8_t ase_lid = 0; ase_lid < GAF_AUDIO_USED_ASE_PER_DIR_MAX_CNT; ase_lid++)
            {
                if (GAF_INVALID_ASE_INDEX != capture_ase_lid_list[ase_lid])
                {
                    capture_ase_lid = capture_ase_lid_list[ase_lid];
                    break;
                }
            }
            if (GAF_INVALID_ASE_INDEX == capture_ase_lid) {
                LOG_I("%s invalid capture ase lid", __func__);
                return;
            }

            gaf_stream_common_update_capture_stream_state(pStreamEnv, GAF_CAPTURE_STREAM_INITIALIZING);
            pStreamEnv->func_list->stream_func_list.capture_start_stream_func(pStreamEnv);
        }
    }

    LOG_D("%s end", __func__);
}

void gaf_audio_stream_start(uint8_t con_lid, uint8_t ase_lid)
{
#ifdef GAF_OFFLOAD_ENABLE
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = _gaf_refresh_all_stream_info_by_con_lid(con_lid);
    GAF_MEDIA_DWELLING_INFO_T* pDewellingInfo = &gaf_cis_media_dwelling_info[con_lid];
    if ((pStreamEnv->stream_info.bap_contextType == BES_BLE_GAF_CONTEXT_TYPE_GAME_BIT))
    {
        if (pStreamEnv && (gaf_offload_audio_stream_start(pDewellingInfo, pStreamEnv, con_lid)))
        {
            return;
        }
    }
#endif
    _gaf_audio_stream_start(con_lid, ase_lid);
}

static void gaf_audio_stream_stop_generic(bool isCheckServiceState, uint8_t con_lid)
{
    LOG_I("%s %d", __func__, isCheckServiceState);

    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = &gaf_audio_stream_env[con_lid];
    bool isRetrigger = pStreamEnv->stream_context.playback_retrigger_onprocess;

#ifdef MIX_AUDIO_PROMPT_WITH_A2DP_MEDIA_ENABLED
    uint8_t pendingStopOp;
    if (isCheckServiceState)
    {
        pendingStopOp = PENDING_TO_STOP_BLE_AUDIO_SINGLE_STREAMING;
    }
    else
    {
        pendingStopOp = PENDING_TO_STOP_BLE_AUDIO_STREAMING;
    }

    bool ret = audio_prompt_check_on_stopping_ble_audio_stream(pendingStopOp,
                    con_lid, con_lid, (void*)gaf_audio_stream_stop_generic);
    if (!ret)
    {
        TRACE(0,"Pending stop BLE_AUDIO_STREAM");
        return;
    }
#endif

    /// Check stream state
    if (pStreamEnv &&
        (GAF_CAPTURE_STREAM_IDLE != pStreamEnv->stream_context.capture_stream_state ||
         GAF_PLAYBACK_STREAM_IDLE != pStreamEnv->stream_context.playback_stream_state))
    {
        GAF_MEDIA_DWELLING_INFO_T* pDewellingInfo = &gaf_cis_media_dwelling_info[con_lid];

        if (isCheckServiceState)
        {
            if ((0 == (pDewellingInfo->startedStreamTypes&GAF_AUDIO_STREAM_TYPE_PLAYBACK)) &&
                (GAF_PLAYBACK_STREAM_IDLE != pStreamEnv->stream_context.playback_stream_state))
            {
#ifdef GAF_OFFLOAD_ENABLE
                gaf_offload_stop_playback_stream(pStreamEnv);
#endif
                pStreamEnv->func_list->stream_func_list.playback_stop_stream_func(pStreamEnv);
                for (uint8_t i = 0; i < GAF_AUDIO_ASE_TOTAL_COUNT; i++)
                {
                    pStreamEnv->stream_info.playbackInfo.aseChInfo[i].iso_channel_hdl = GAF_AUDIO_INVALID_ISO_CHANNEL;
                    pStreamEnv->stream_info.playbackInfo.aseChInfo[i].allocation_bf = 0;
                    pStreamEnv->stream_context.lastestPlaybackSeqNum[i] = GAF_AUDIO_INVALID_SEQ_NUMBER;
                }
            }

            if ((0 == (pDewellingInfo->startedStreamTypes&GAF_AUDIO_STREAM_TYPE_CAPTURE)) &&
                (GAF_CAPTURE_STREAM_IDLE != pStreamEnv->stream_context.capture_stream_state))
            {
#ifdef GAF_OFFLOAD_ENABLE
                gaf_offload_stop_capture_stream(pStreamEnv);
#endif
                pStreamEnv->func_list->stream_func_list.capture_stop_stream_func(pStreamEnv);
                for (uint8_t i = 0; i < GAF_AUDIO_ASE_TOTAL_COUNT; i++)
                {
                    pStreamEnv->stream_info.captureInfo.aseChInfo[i].iso_channel_hdl = GAF_AUDIO_INVALID_ISO_CHANNEL;
                    pStreamEnv->stream_info.captureInfo.aseChInfo[i].allocation_bf = 0;
                }
            }
        }
        else
        {
#ifdef GAF_OFFLOAD_ENABLE
            gaf_offload_stop_playback_stream(pStreamEnv);
#endif
            pStreamEnv->func_list->stream_func_list.playback_stop_stream_func(pStreamEnv);
            for (uint8_t i = 0; i < GAF_AUDIO_ASE_TOTAL_COUNT; i++)
            {
                pStreamEnv->stream_info.playbackInfo.aseChInfo[i].iso_channel_hdl = GAF_AUDIO_INVALID_ISO_CHANNEL;
                pStreamEnv->stream_info.playbackInfo.aseChInfo[i].allocation_bf = 0;
                pStreamEnv->stream_context.lastestPlaybackSeqNum[i] = GAF_AUDIO_INVALID_SEQ_NUMBER;
            }
#ifdef GAF_OFFLOAD_ENABLE
            gaf_offload_stop_capture_stream(pStreamEnv);
            gaf_offload_close_cp();
#endif
            pStreamEnv->func_list->stream_func_list.capture_stop_stream_func(pStreamEnv);
            for (uint8_t i = 0; i < GAF_AUDIO_ASE_TOTAL_COUNT; i++)
            {
                pStreamEnv->stream_info.captureInfo.aseChInfo[i].iso_channel_hdl = GAF_AUDIO_INVALID_ISO_CHANNEL;
                pStreamEnv->stream_info.captureInfo.aseChInfo[i].allocation_bf = 0;
            }
        }

        /// Should be transfered to IDLE by previous procedure, avoid multi call
        if ((GAF_CAPTURE_STREAM_IDLE == pStreamEnv->stream_context.capture_stream_state) &&
            (GAF_PLAYBACK_STREAM_IDLE == pStreamEnv->stream_context.playback_stream_state))
        {
            gaf_stream_register_running_stream_ref(con_lid, NULL);
            bt_drv_reg_op_afh_assess_en(false);
            if (!isRetrigger) {
                // bt_adapter_write_sleep_enable(1);
            }
        }
    }
    else
    {
        LOG_W("%s no stream to stop, may be already stopped", __func__);
    }

#ifdef GAF_DSP
    app_dsp_hifi4_deinit(APP_DSP_HIFI4_USER_GAF_DSP);
#endif

}

void gaf_audio_stream_stop(uint8_t con_lid)
{
    gaf_audio_stream_stop_generic(false, con_lid);
}

void gaf_audio_stream_refresh_and_stop(uint8_t conlid, uint16_t stream_type, uint8_t stream_lid)
{
    gaf_audio_stream_stop_generic(true, conlid);
}

static void gaf_audio_steam_retrigger_handler(void* stream_env, uint32_t streamType)
{
    GAF_AUDIO_STREAM_ENV_T *pStreamEnv = (GAF_AUDIO_STREAM_ENV_T *)stream_env;
    GAF_MEDIA_DWELLING_INFO_T gaf_cis_media_dwelling_info_rec;
    // Record this
    memcpy_s(&gaf_cis_media_dwelling_info_rec, sizeof(GAF_MEDIA_DWELLING_INFO_T),
                &gaf_cis_media_dwelling_info[pStreamEnv->stream_info.con_lid],
                sizeof(GAF_MEDIA_DWELLING_INFO_T));
    // Clear this
    gaf_audio_dwelling_info_list_init();
    // Memset to invalid ase lid
    for (uint8_t con_lid = 0; con_lid < GAF_MAXIMUM_CONNECTION_COUNT; con_lid++)
    {
        gaf_audio_clear_playback_ase_index_list(con_lid);
        gaf_audio_clear_capture_ase_index_list(con_lid);
    }
    // Stop this must in this thread
    gaf_cis_media_dwelling_info[pStreamEnv->stream_info.con_lid].startedStreamTypes &= ~streamType;
    gaf_audio_stream_stop_generic(true, pStreamEnv->stream_info.con_lid);

    // Recovery this
    memcpy_s(&gaf_cis_media_dwelling_info[pStreamEnv->stream_info.con_lid],
                sizeof(GAF_MEDIA_DWELLING_INFO_T),
                &gaf_cis_media_dwelling_info_rec, sizeof(GAF_MEDIA_DWELLING_INFO_T));
    // Start this in other thread is ok
    if (GAF_AUDIO_STREAM_TYPE_PLAYBACK == streamType)
    {
        uint8_t *playback_ase_lid_list = gaf_audio_get_playback_ase_index_list(pStreamEnv->stream_info.con_lid);
        for (uint8_t ase_lid = 0; ase_lid < GAF_AUDIO_USED_ASE_PER_DIR_MAX_CNT; ase_lid++)
        {
            if (GAF_INVALID_ASE_INDEX != playback_ase_lid_list[ase_lid])
            {
                if (GAF_INVALID_ASE_INDEX != playback_ase_lid_list[ase_lid])
                {
                    gaf_audio_stream_start(pStreamEnv->stream_info.con_lid, ase_lid);
#if !defined (BLE_AUDIO_STEREO_CHAN_OVER_CIS_CNT)
                    break;
#endif
                }
            }
        }
    }
    else
    {
        uint8_t *capture_ase_lid_list = gaf_audio_get_capture_ase_index_list(pStreamEnv->stream_info.con_lid);
        for (uint8_t ase_lid = 0; ase_lid < GAF_AUDIO_USED_ASE_PER_DIR_MAX_CNT; ase_lid++)
        {
            if (GAF_INVALID_ASE_INDEX != capture_ase_lid_list[ase_lid])
            {
                if (GAF_INVALID_ASE_INDEX != capture_ase_lid_list[ase_lid])
                {
                    gaf_audio_stream_start(pStreamEnv->stream_info.con_lid, ase_lid);
                    break;
                }
            }
        }
    }
}

void gaf_audio_stream_register_func_list(GAF_AUDIO_STREAM_ENV_T* pStreamEnv, GAF_AUDIO_FUNC_LIST_T* funcList)
{
    pStreamEnv->func_list = funcList;
}

static void gaf_audio_stream_started_ind(const bes_ble_bap_ascs_ase_t *p_ase_info)
{
    LOG_I("%s start", __func__);

#if (BES_AHP)
    // Fetch intv and fmt ltv
    app_gaf_bap_ht_cfg_intv_ltv_t *p_ht_intv =
            (app_gaf_bap_ht_cfg_intv_ltv_t *)bes_ble_audio_get_ltv_value_by_type(&p_ase_info->p_cfg->add_cfg,
                                                                           APP_GAF_BAP_CFG_TYPE_HT_FRAME_INTV);
    app_gaf_bap_ht_cfg_fmt_ltv_t *p_ht_fmt =
            (app_gaf_bap_ht_cfg_fmt_ltv_t *)bes_ble_audio_get_ltv_value_by_type(&p_ase_info->p_cfg->add_cfg,
                                                                          APP_GAF_BAP_CFG_TYPE_HT_FRAME_FMT);

    if (BES_BLE_GAF_DIRECTION_SRC == p_ase_info->direction && p_ht_intv != NULL && p_ht_fmt != NULL)
    {
        LOG_I("HT src ase_lid %d to start gaf, intv %x, supp_fmt %x, rel_flags %x", p_ase_info->ase_lid,
                                                             p_ht_intv->intv, p_ht_fmt->data_format,
                                                             p_ht_fmt->related_flags);

        gaf_simulated_sensor_data_update_upstream_start(p_ase_info, p_ht_intv->intv,
                                                        ((p_ht_fmt->data_format << 8) | p_ht_fmt->related_flags));
        return;
    }
#endif

    gaf_audio_stream_update_and_start_handler(p_ase_info->ase_lid);
}

static void gaf_audio_stream_stopped_ind(const bes_ble_bap_ascs_ase_t *p_ase_info)
{
    LOG_I("%s start", __func__);

#if (BES_AHP)
    // Fetch intv and fmt ltv
    app_gaf_bap_ht_cfg_intv_ltv_t *p_ht_intv =
        (app_gaf_bap_ht_cfg_intv_ltv_t *)bes_ble_audio_get_ltv_value_by_type(&p_ase_info->p_cfg->add_cfg,
                                                                             APP_GAF_BAP_CFG_TYPE_HT_FRAME_INTV);
    app_gaf_bap_ht_cfg_fmt_ltv_t *p_ht_fmt =
        (app_gaf_bap_ht_cfg_fmt_ltv_t *)bes_ble_audio_get_ltv_value_by_type(&p_ase_info->p_cfg->add_cfg,
                                                                            APP_GAF_BAP_CFG_TYPE_HT_FRAME_FMT);

    if (BES_BLE_GAF_DIRECTION_SRC == p_ase_info->direction && p_ht_intv != NULL && p_ht_fmt != NULL)
    {
        LOG_I("%s HT src ase_lid %d to start gaf", __func__, p_ase_info->ase_lid);
        gaf_non_codec_upstream_stop(p_ase_info->ase_lid);
        return;
    }
#endif
#ifdef HID_ULL_ENABLE
    if (BES_BLE_GAF_DIRECTION_SRC == p_ase_info->direction
            && APP_GAF_CODEC_TYPE_ULL == p_ase_info->codec_id.codec_id[0]
            && 0x00 == p_ase_info->codec_id.codec_id[3]
            && 0x00 == p_ase_info->codec_id.codec_id[4])
    {
        gaf_ull_hid_upstream_stop(p_ase_info->con_lid);
        return;
    }
#endif

    gaf_stream_context_state_t updatedContextStreamState = gaf_audio_stream_update_and_stop_handler(p_ase_info->ase_lid);

    app_ble_audio_event_t evt = BLE_AUDIO_MAX_IND;

    switch (updatedContextStreamState)
    {
        case APP_GAF_CONTEXT_ALL_STREAMS_STOPPED:
        {
            if (p_ase_info->init_context_bf & BES_BLE_GAF_CONTEXT_TYPE_CONVERSATIONAL_BIT)
            {
                evt = BLE_AUDIO_CALL_ALL_STREAMS_STOP_IND;
            }
            else if (p_ase_info->init_context_bf & BES_BLE_GAF_CONTEXT_TYPE_MEDIA_BIT)
            {
                evt = BLE_AUDIO_MUSIC_STREAM_STOP_IND;
            }
            else
            {
                evt = BLE_AUDIO_FLEXIBLE_ALL_STREAMS_STOP_IND;
            }
        }
        break;
        case APP_GAF_CONTEXT_CAPTURE_STREAMS_STOPPED:
        case APP_GAF_CONTEXT_PLAYBACK_STREAMS_STOPPED:
        {
            if (p_ase_info->init_context_bf & BES_BLE_GAF_CONTEXT_TYPE_CONVERSATIONAL_BIT)
            {
                if (APP_GAF_CONTEXT_CAPTURE_STREAMS_STOPPED == updatedContextStreamState)
                {
                    evt = BLE_AUDIO_CALL_CAPTURE_STREAM_STOP_IND;
                }
                else
                {
                    evt = BLE_AUDIO_CALL_PLAYBACK_STREAM_STOP_IND;
                }
            }
            else if (p_ase_info->init_context_bf & BES_BLE_GAF_CONTEXT_TYPE_MEDIA_BIT)
            {
                LOG_I("%s music do not need single stream stop!!!", __func__);
                return;
            }
            else
            {
                if (APP_GAF_CONTEXT_CAPTURE_STREAMS_STOPPED == updatedContextStreamState)
                {
                    evt = BLE_AUDIO_FLEXIBLE_CAPTURE_STREAM_STOP_IND;
                }
                else
                {
                    evt = BLE_AUDIO_FLEXIBLE_PLAYBACK_STREAM_STOP_IND;
                }
            }
        }
        break;
        default:
            return;
    }

    bes_ble_audio_sink_streaming_handle_event(p_ase_info->con_lid, p_ase_info->ase_lid, BES_BLE_GAF_DIRECTION_MAX, evt);
}

static void gaf_audio_stream_state_changed(uint8_t ase_lid, AOB_MGR_STREAM_STATE_E prev_state, AOB_MGR_STREAM_STATE_E curr_state)
{
    if (prev_state == curr_state)
    {
        return;
    }

    const bes_ble_bap_ascs_ase_t *p_ase_info = bes_ble_get_ascs_ase_info(ase_lid);

    if (p_ase_info == NULL)
    {
        return;
    }

    /// Send stream start ind to prepare gaf stream info before truly streaming
    if (((BES_BLE_GAF_DIRECTION_SINK == p_ase_info->direction) &&
            (curr_state == AOB_MGR_STREAM_STATE_ENABLING)) ||
            ((BES_BLE_GAF_DIRECTION_SRC == p_ase_info->direction) &&
             (curr_state == AOB_MGR_STREAM_STATE_STREAMING)))
    {
        gaf_audio_stream_started_ind(p_ase_info);
    }
    else if ((AOB_MGR_STREAM_STATE_STREAMING == prev_state ||
              AOB_MGR_STREAM_STATE_ENABLING == prev_state) &&
             /// Streaming or enabling go to any state legally
             ((AOB_MGR_STREAM_STATE_QOS_CONFIGURED == curr_state) ||
              (AOB_MGR_STREAM_STATE_RELEASING == curr_state) ||
              (AOB_MGR_STREAM_STATE_IDLE == curr_state) ||
              (AOB_MGR_STREAM_STATE_CODEC_CONFIGURED == curr_state) ||
              (AOB_MGR_STREAM_STATE_DISABLING == curr_state)))
    {
        gaf_audio_stream_stopped_ind(p_ase_info);
    }
}

static void gaf_audio_stream_vol_changed(uint8_t con_lid, uint8_t le_volume, bool mute)
{
    uint8_t localVol = mute ? TGT_VOLUME_LEVEL_MUTE : bes_ble_arc_convert_le_vol_to_local_vol(le_volume);
    GAF_AUDIO_STREAM_ENV_T *pStreamEnv = NULL;

    if (con_lid < GAF_MAXIMUM_CONNECTION_COUNT)
    {
        pStreamEnv = &(gaf_audio_stream_env[con_lid]);
    }

    if (pStreamEnv != NULL &&
            pStreamEnv->stream_context.playback_stream_state >  GAF_PLAYBACK_STREAM_IDLE)
    {
        /// TODO:BT thread lock call and audio thread mutex
        app_bt_stream_volumeset(localVol);
    }
    else if (gaf_bis_audio_get_stream_state() > GAF_PLAYBACK_STREAM_IDLE)
    {
        gaf_bis_audio_stream_set_stream_volume(localVol);
    }
}

void gaf_audio_le_audio_event_handler(const AOB_EVENT_PACKET *evt_pkt)
{
    const AOB_EVENT_PACKET *p = (AOB_EVENT_PACKET *)evt_pkt;
    const AOB_EVENT_HEADER_T *head = (AOB_EVENT_HEADER_T *)evt_pkt;

    switch (head->type)
    {
        case (AOB_EVENT_CIS_ESTABLISHED_IND):
        {
            const AOB_UC_SRV_CIS_INFO_T *cis_estb = p->aob_cis_established_ind.ascs_cis_established;
            /// Update iso handle for gaf media
            if (AOB_COMMON_INVALID_CON_ID != cis_estb->ase_lid_sink)
            {
                gaf_audio_update_stream_iso_hdl(cis_estb->ase_lid_sink);
            }

            if (AOB_COMMON_INVALID_CON_ID != cis_estb->ase_lid_src)
            {
                gaf_audio_update_stream_iso_hdl(cis_estb->ase_lid_src);
            }
        }
        break;
        case (AOB_EVENT_STREAM_STATUS_CHANGED):
        {
            gaf_audio_stream_state_changed(p->aob_stream_state.ase_lid, p->aob_stream_state.prev_state,
                                           p->aob_stream_state.curr_state);
        }
        break;
        case (AOB_EVENT_VOL_CHANGED):
        {
            gaf_audio_stream_vol_changed(p->aob_vol_changed.con_lid, p->aob_vol_changed.volume,
                                         p->aob_vol_changed.mute);
        }
        break;
        default:
            break;
    }
}

void gaf_audio_stream_init(void)
{
    /// GAF CUSTOM CALLBACK PTR
    gaf_uc_srv_custom_data_callback = gaf_stream_common_get_custom_data_handler(GAF_STREAM_USER_CASE_UC_SRV);
    /// Sanity check
    ASSERT(gaf_uc_srv_custom_data_callback, "Invalid custom data callback, user case %d", GAF_STREAM_USER_CASE_UC_SRV);

    gaf_audio_dwelling_info_list_init();

    for (uint8_t con_lid = 0; con_lid < GAF_MAXIMUM_CONNECTION_COUNT; con_lid++)
    {
        GAF_AUDIO_STREAM_ENV_T *pStreamEnv = &gaf_audio_stream_env[con_lid];
        memset_s((uint8_t *)pStreamEnv,sizeof(GAF_AUDIO_STREAM_ENV_T),  0, sizeof(GAF_AUDIO_STREAM_ENV_T));
        pStreamEnv->stream_context.playback_stream_state = GAF_PLAYBACK_STREAM_IDLE;
        pStreamEnv->stream_context.capture_stream_state = GAF_CAPTURE_STREAM_IDLE;
        pStreamEnv->stream_info.playbackInfo.presDelayUs = 0;
        pStreamEnv->stream_info.captureInfo.presDelayUs = 0;
        pStreamEnv->stream_context.playback_trigger_supervisor_timer_id = NULL;
        pStreamEnv->stream_context.capture_trigger_supervisor_timer_id = NULL;
        pStreamEnv->stream_context.playback_retrigger_onprocess = false;
        pStreamEnv->stream_context.capture_retrigger_onprocess = false;
        pStreamEnv->stream_info.is_mobile = false;
        pStreamEnv->stream_info.is_bis = false;
        pStreamEnv->stream_info.contextType = GAF_AUDIO_STREAM_CONTEXT_TYPE_FLEXIBLE;
        gaf_stream_common_register_func_list(pStreamEnv, &gaf_audio_flexible_stream_func_list[con_lid]);
        gaf_playback_status_mutex_init(pStreamEnv->stream_info.playback_status_mutex);
        gaf_audio_clear_playback_ase_index_list(con_lid);
        gaf_audio_clear_capture_ase_index_list(con_lid);

        for (uint8_t i = 0; i < GAF_AUDIO_ASE_TOTAL_COUNT; i++)
        {
            pStreamEnv->stream_info.playbackInfo.aseChInfo[i].iso_channel_hdl = GAF_AUDIO_INVALID_ISO_CHANNEL;
            pStreamEnv->stream_info.captureInfo.aseChInfo[i].iso_channel_hdl = GAF_AUDIO_INVALID_ISO_CHANNEL;
            pStreamEnv->stream_info.playbackInfo.aseChInfo[i].allocation_bf = 0;
            pStreamEnv->stream_info.captureInfo.aseChInfo[i].allocation_bf = 0;
            pStreamEnv->stream_context.lastestPlaybackSeqNum[i] = GAF_AUDIO_INVALID_SEQ_NUMBER;
            pStreamEnv->stream_info.playbackInfo.aseChInfo[i].codec_core_info.instance_status = INSTANCE_IDLE;
            pStreamEnv->stream_info.captureInfo.aseChInfo[i].codec_core_info.instance_status = INSTANCE_IDLE;
        }
    }

    gaf_stream_common_register_cc_func_list();
    gaf_stream_register_retrigger_callback(gaf_audio_steam_retrigger_handler);

    bes_ble_aob_register_global_handler(gaf_audio_le_audio_event_handler);
}

void gaf_audio_playback_closeall(void)
{
    GAF_AUDIO_STREAM_ENV_T *pStreamEnv = NULL;
    GAF_MEDIA_DWELLING_INFO_T* pDewellingInfo = NULL;
    for (uint8_t con_lid = 0; con_lid < GAF_MAXIMUM_CONNECTION_COUNT; con_lid++)
    {
        pStreamEnv = &gaf_audio_stream_env[con_lid];

        if (pStreamEnv->stream_context.capture_stream_state != GAF_CAPTURE_STREAM_IDLE ||
            pStreamEnv->stream_context.playback_stream_state != GAF_PLAYBACK_STREAM_IDLE)
        {
            pDewellingInfo = &gaf_cis_media_dwelling_info[con_lid];

            if (GAF_AUDIO_STREAM_TYPE_PLAYBACK & pDewellingInfo->startedStreamTypes ||
                GAF_AUDIO_STREAM_TYPE_FLEXIBLE & pDewellingInfo->startedStreamTypes)
            {
                gaf_audio_stream_stop(con_lid);
            }
        }
    }
}
#endif
