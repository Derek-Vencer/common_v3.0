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
#include "bt_common_define.h"
#include "hal_codec.h"
#include "codec_int.h"
#include "app_utils.h"
#include "gaf_cp_stream.h"
#include "gaf_media_sync.h"
#include "gaf_media_stream.h"
#include "bt_drv_reg_op.h"
#include "gaf_stream_dbg.h"
#include "hwtimer_list.h"
#include "bt_drv_interface.h"
#include "analog.h"
#include "gaf_audio_path.h"
#include "gaf_mobile_media_stream.h"
#include "app_bt.h"
#include "hal_mcu2cp.h"
#include "cp_accel.h"
#include "gaf_offload_dsp_stream.h"
#include "bes_aob_api.h"
#include "bta_bt_api.h"
struct lea_offload_tuning_pid_t
{
    float prev_error;
    float integral;
    float Kp;
    float Ki;
    float Kd;
    float result;
};

struct lea_offload_tuning_info_t
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv_ptr;
    HWTIMER_ID gaf_offload_tuning_timer_id;
    uint32_t ref_diff_id;
    uint16_t iso_interval;
    lea_offload_tuning_pid_t offload_pid;
};

#define ULL_MODE_FRAME    5000
#pragma pack(1)
static gaf_offload_message gaf_offload_codec_cfg =
{
    .magic_num = 0x5e5a,
    .startedStreamTypes = 0,
    .codec_cfg = NULL,
    .pStreamEnv_ptr = NULL,
    .cap_vol_gain = 1.0f,
};
#pragma pack()

/*===============================================================================*/
// The follwing part is common api
/*===============================================================================*/
float gaf_offload_pid_compute(lea_offload_tuning_pid_t *pid, float setpoint, float measured, float dt) {
    float error = setpoint - measured;
    pid->integral += error * dt;
    float derivative = (error - pid->prev_error) / dt;

    float output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;
    pid->prev_error = error;
    LOG_D("pid->Kp error %d pid->Ki pid->integral %d pid->Kd derivative %d", (int)(pid->Kp * error*1000), (int)(pid->Ki * pid->integral*1000), (int)(pid->Kd * derivative*1000));
    return output;
}

void init_offload_pid(struct lea_offload_tuning_pid_t* offload_pid) 
{
#if defined(__AUDIO_RESAMPLE__)
    offload_pid->prev_error = 0;
    offload_pid->integral = 0;
    offload_pid->Kp = 3.6;
    offload_pid->Ki = 3.6;
    offload_pid->Kd = -0.005;
    offload_pid->result = 0;
#else
    offload_pid->prev_error = 0;
    offload_pid->integral = 0;
    offload_pid->Kp = 2.6;
    offload_pid->Ki = 1.2;
    offload_pid->Kd = 0.001;
    offload_pid->result = 0;
#endif
}

void gaf_offload_capture_vol_change(float gain)
{
    gaf_offload_codec_cfg.startedStreamTypes = GAF_OFFLOAD_CAPTURE_VOL_CHANGR;
    gaf_offload_codec_cfg.cap_vol_gain = gain;
    hal_mcu2cp_send_mcu(HAL_MCU2CP_ID_0, (const unsigned char *)&gaf_offload_codec_cfg, sizeof(gaf_offload_message));
}

void gaf_offload_playback_vol_change(uint8_t vol)
{
    gaf_offload_codec_cfg.startedStreamTypes = GAF_OFFLOAD_PLAYBACK_VOL_CHANGR;
    gaf_offload_codec_cfg.playback_vol_gain = vol;
    hal_mcu2cp_send_mcu(HAL_MCU2CP_ID_0, (const unsigned char *)&gaf_offload_codec_cfg, sizeof(gaf_offload_message));
}


void gaf_offload_close_cp(void)
{
    btdrv_set_iso_intr(0);
    gaf_offload_codec_cfg.startedStreamTypes = GAF_OFFLOAD_BUF_DEINITED;
    hal_mcu2cp_send_mcu(HAL_MCU2CP_ID_0, (const unsigned char *)&gaf_offload_codec_cfg, sizeof(gaf_offload_message));
    app_bt_set_access_mode(BTIF_BAM_CONNECTABLE_ONLY);
    app_sysfreq_req(APP_SYSFREQ_USER_AOB, APP_SYSFREQ_32K);
    bta_bt_write_sleep_enable(BTA_BT_SLEEP_GAF_OFFLOAD_STREAM, 1);
    osDelay(10);
    gaf_cp_stop_process();
}
/*===============================================================================*/
// The follwing part is headset part code
/*===============================================================================*/
struct lea_offload_tuning_info_t gaf_offload_playback_tunning_info = {
    .pStreamEnv_ptr = NULL,
    .gaf_offload_tuning_timer_id = NULL,
    .ref_diff_id = 0,
    .iso_interval = ULL_MODE_FRAME,
};

struct lea_offload_tuning_info_t gaf_offload_capture_tunning_info = {
    .pStreamEnv_ptr = NULL,
    .gaf_offload_tuning_timer_id = NULL,
    .ref_diff_id = 0,
    .iso_interval = ULL_MODE_FRAME,
};

static void gaf_offload_playback_tuning_handler(void *argument) 
{
    uint32_t btclk, lock; //hal slot -- 312.5us
    uint16_t btcnt; //hal  microsecond -- 0.5 us
    uint8_t cnt = 0;
    struct lea_offload_config_t* lea_offload_cfg =(struct lea_offload_config_t*)get_lea_offload_cfg_param();
    uint8_t adma_ch = lea_offload_cfg->offload_playback_ch;
    static uint32_t fist_iso_cd = 0;
    static uint8_t num = 0;
#ifdef CP_IN_SAME_EE
    uint32_t dma_base = AUDMA_BASE;
#else
    uint32_t dma_base = SYS_DMA2_BASE;
#endif
    lock = int_lock();
    bt_drv_reg_op_dma_tc_clkcnt_get_by_ch(&btclk, &btcnt, adma_ch&0xFF, dma_base);
    uint32_t dmaIrqHappeningTimeUs = bt_syn_ble_bt_time_to_bts(btclk, btcnt);
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = gaf_offload_playback_tunning_info.pStreamEnv_ptr;
    if(pStreamEnv)
    {
        uint32_t iso_event_timestamp = gaf_media_common_get_latest_rx_iso_evt_timestamp(pStreamEnv);
        uint32_t current_bt_time = gaf_media_sync_get_curr_time();
        if ((iso_event_timestamp - current_bt_time) > gaf_offload_playback_tunning_info.iso_interval)
        {
            iso_event_timestamp -= gaf_offload_playback_tunning_info.iso_interval;
        }
        while (GAF_AUDIO_CLK_32_BIT_DIFF(iso_event_timestamp ,dmaIrqHappeningTimeUs) > 0)
        {
            TRACE(0, "iso_event_timestamp less than dmaIrqHappeningTimeUs");
            iso_event_timestamp += gaf_offload_playback_tunning_info.iso_interval;
            if (cnt > 10)
            {
                ASSERT(0, "[%s] iso_time %d dmaTime %d", __func__, iso_event_timestamp, dmaIrqHappeningTimeUs);
            }
            cnt++;
        }
        uint32_t diff_id = iso_event_timestamp - dmaIrqHappeningTimeUs;
        if (diff_id > gaf_offload_playback_tunning_info.iso_interval)
        {
            diff_id -= gaf_offload_playback_tunning_info.iso_interval;
        }
        POSSIBLY_UNUSED uint32_t diff_ic = iso_event_timestamp - current_bt_time;
        uint32_t diff_cd = current_bt_time - dmaIrqHappeningTimeUs;
        uint32_t diff_diff = 0;
        if (fist_iso_cd)
        {
            diff_diff = diff_cd - fist_iso_cd;
        }
        else
        {
            fist_iso_cd = diff_cd;
        }
        hwtimer_start(gaf_offload_playback_tunning_info.gaf_offload_tuning_timer_id, US_TO_TICKS(gaf_offload_playback_tunning_info.iso_interval - diff_diff + 100000));
        int_unlock(lock);
        if (gaf_offload_playback_tunning_info.ref_diff_id == 0)
        {
            gaf_offload_playback_tunning_info.ref_diff_id = diff_id;
        }
        if (iso_event_timestamp < dmaIrqHappeningTimeUs)
        {
            diff_id = gaf_offload_playback_tunning_info.iso_interval + diff_id;
        }

        if (diff_id > gaf_offload_playback_tunning_info.iso_interval)
        {
            diff_id -= gaf_offload_playback_tunning_info.iso_interval;
        }

        float ratio_test = (0.000002f * gaf_offload_pid_compute(&gaf_offload_playback_tunning_info.offload_pid, gaf_offload_playback_tunning_info.ref_diff_id, diff_id, 0.005f));
#if defined(__AUDIO_RESAMPLE__)
        hal_codec_tune_resample_rate(AUD_STREAM_PLAYBACK, ratio_test);
#else
        analog_aud_pll_tune(ratio_test);
#endif
        LOG_I("%s:dma %u iso %u cur %u num %d", __func__, dmaIrqHappeningTimeUs, iso_event_timestamp, current_bt_time,num);
        LOG_I("diff_id %d diff_ic %d diff_diff %d ref_diff_id %d", diff_id, diff_ic, diff_diff, gaf_offload_playback_tunning_info.ref_diff_id);
        num++;
    }
    else
    {
        int_unlock(lock);
    }
}

static void gaf_offload_capture_tuning_handler(void *argument) 
{
    uint32_t btclk, lock; //hal slot -- 312.5us
    uint16_t btcnt; //hal  microsecond -- 0.5 us
    uint8_t cnt = 0;
    struct lea_offload_config_t* lea_offload_cfg =(struct lea_offload_config_t*)get_lea_offload_cfg_param();
    uint8_t adma_ch = lea_offload_cfg->offload_capture_ch;
    static uint32_t fist_iso_cd = 0;
    static uint8_t num = 0;
#ifdef CP_IN_SAME_EE
    uint32_t dma_base = AUDMA_BASE;
#else
    uint32_t dma_base = SYS_DMA2_BASE;
#endif
    lock = int_lock();
    bt_drv_reg_op_dma_tc_clkcnt_get_by_ch(&btclk, &btcnt, adma_ch&0xFF, dma_base);
    uint32_t dmaIrqHappeningTimeUs = bt_syn_ble_bt_time_to_bts(btclk, btcnt);
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = gaf_offload_capture_tunning_info.pStreamEnv_ptr;
    if(pStreamEnv)
    {
        uint32_t iso_event_timestamp = gaf_media_common_get_latest_tx_iso_evt_timestamp(pStreamEnv);
        uint32_t current_bt_time = gaf_media_sync_get_curr_time();
        if ((iso_event_timestamp - current_bt_time) > gaf_offload_capture_tunning_info.iso_interval)
        {
            iso_event_timestamp -= gaf_offload_capture_tunning_info.iso_interval;
        }
        while (GAF_AUDIO_CLK_32_BIT_DIFF(iso_event_timestamp ,dmaIrqHappeningTimeUs) > 0)
        {
            TRACE(0, "iso_event_timestamp less than dmaIrqHappeningTimeUs");
            iso_event_timestamp += gaf_offload_capture_tunning_info.iso_interval;
            if (cnt > 10)
            {
                ASSERT(0, "[%s] iso_time %d dmaTime %d", __func__, iso_event_timestamp, dmaIrqHappeningTimeUs);
            }
            cnt++;
        }
        uint32_t diff_id = iso_event_timestamp - dmaIrqHappeningTimeUs;
        if (diff_id > gaf_offload_capture_tunning_info.iso_interval)
        {
            diff_id -= gaf_offload_capture_tunning_info.iso_interval;
        }
        POSSIBLY_UNUSED uint32_t diff_ic = iso_event_timestamp - current_bt_time;
        uint32_t diff_cd = current_bt_time - dmaIrqHappeningTimeUs;
        uint32_t diff_diff = 0;
        if (fist_iso_cd)
        {
            diff_diff = diff_cd - fist_iso_cd;
        }
        else
        {
            fist_iso_cd = diff_cd;
        }
        hwtimer_start(gaf_offload_capture_tunning_info.gaf_offload_tuning_timer_id, US_TO_TICKS(gaf_offload_capture_tunning_info.iso_interval - diff_diff + 100000));
        int_unlock(lock);
        if (gaf_offload_capture_tunning_info.ref_diff_id == 0)
        {
            gaf_offload_capture_tunning_info.ref_diff_id = diff_id;
        }
        if (iso_event_timestamp < dmaIrqHappeningTimeUs)
        {
            diff_id = gaf_offload_capture_tunning_info.iso_interval + diff_id;
        }
        float ratio_test = (0.000002f * gaf_offload_pid_compute(&gaf_offload_capture_tunning_info.offload_pid, gaf_offload_capture_tunning_info.ref_diff_id, diff_id, 0.005f));
#if defined(__AUDIO_RESAMPLE__)
        hal_codec_tune_resample_rate(AUD_STREAM_CAPTURE, ratio_test);
#else
        analog_aud_pll_tune(ratio_test);
#endif
        LOG_I("%s:dma %u iso %u cur %u num %d", __func__, dmaIrqHappeningTimeUs, iso_event_timestamp, current_bt_time,num);
        LOG_I("diff_id %d diff_ic %d diff_diff %d ref_diff_id %d", diff_id, diff_ic, diff_diff, gaf_offload_capture_tunning_info.ref_diff_id);
        num++;
    }
    else
    {
        int_unlock(lock);
    }
}

void gaf_audio_flexible_playback_dma_dsp_start_handler(uint8_t con_lid)
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv =  _gaf_refresh_all_stream_info_by_con_lid(con_lid);
    if (pStreamEnv->stream_context.playback_stream_state != GAF_PLAYBACK_STREAM_INITIALIZING)
    {
        return;
    }
    uint32_t iso_interval = 5000;
    uint32_t current_bt_time = 0;
    uint32_t trigger_time = 0, cnt = 0;
    gaf_offload_playback_tunning_info.ref_diff_id = 0;
    init_offload_pid(&gaf_offload_playback_tunning_info.offload_pid);
    uint32_t iso_event_timestamp = gaf_media_common_get_latest_rx_iso_evt_timestamp(pStreamEnv);

    if (iso_event_timestamp)
    {
        // iso_evnt
        current_bt_time = gaf_media_sync_get_curr_time();
        while (GAF_AUDIO_CLK_32_BIT_DIFF(iso_event_timestamp, current_bt_time) > 0)
        {
            iso_event_timestamp += iso_interval;
            if (cnt > 10)
            {
                ASSERT(0, "[%s] iso_time %d current_bt_time %d", __func__, iso_event_timestamp, current_bt_time);
            }
            cnt++;
        }
        trigger_time = iso_event_timestamp + 220 + 370;
        //1270 or 370 reasonable
        gaf_stream_common_update_playback_stream_state(pStreamEnv, GAF_PLAYBACK_STREAM_START_TRIGGERING);
        gaf_stream_common_set_playback_trigger_time(pStreamEnv, trigger_time);
        gaf_stream_common_update_playback_stream_state(pStreamEnv, GAF_PLAYBACK_STREAM_STREAMING_TRIGGERED);
    }
    struct lea_offload_config_t* lea_offload_cfg =(struct lea_offload_config_t*)get_lea_offload_cfg_param();
    uint8_t adma_ch = lea_offload_cfg->offload_playback_ch;
#ifdef CP_IN_SAME_EE
    uint32_t dma_base = AUDMA_BASE;
#else
    uint32_t dma_base = SYS_DMA2_BASE;
#endif
    bt_drv_reg_op_enable_dma_tc(adma_ch&0xFF, dma_base);
    if (NULL == gaf_offload_playback_tunning_info.gaf_offload_tuning_timer_id)
    {
        gaf_offload_playback_tunning_info.gaf_offload_tuning_timer_id = 
            hwtimer_alloc(gaf_offload_playback_tuning_handler, NULL);
    }
    hwtimer_start(gaf_offload_playback_tunning_info.gaf_offload_tuning_timer_id, US_TO_TICKS(20000));
}

void gaf_audio_flexible_capture_dma_dsp_start_handler(uint8_t con_lid)
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = gaf_offload_capture_tunning_info.pStreamEnv_ptr;

    uint32_t iso_interval = 5000, current_bt_time = 0, trigger_time = 0, cnt = 0;
    gaf_offload_capture_tunning_info.ref_diff_id = 0;
    init_offload_pid(&gaf_offload_capture_tunning_info.offload_pid);

    uint32_t iso_event_timestamp = gaf_media_common_get_latest_tx_iso_evt_timestamp(pStreamEnv);
    if (iso_event_timestamp)
    {
        // iso_evnt
        current_bt_time = gaf_media_sync_get_curr_time();
        while (GAF_AUDIO_CLK_32_BIT_DIFF(iso_event_timestamp, current_bt_time) > 0)
        {
            iso_event_timestamp += iso_interval;
            if (cnt > 10)
            {
                ASSERT(0, "[%s] iso_time %d current_bt_time %d", __func__, iso_event_timestamp, current_bt_time);
            }
            cnt++;
        }
#if defined(__AUDIO_RESAMPLE__)
        trigger_time = iso_event_timestamp + gaf_offload_capture_tunning_info.iso_interval + 220;
#else
        trigger_time = iso_event_timestamp + gaf_offload_capture_tunning_info.iso_interval + 1450;
#endif
        LOG_I("trigger_time %u iso %u cur %u ", trigger_time, iso_event_timestamp, current_bt_time);
        gaf_stream_common_update_capture_stream_state(pStreamEnv, GAF_CAPTURE_STREAM_START_TRIGGERING);
        gaf_stream_common_set_capture_trigger_time(pStreamEnv, trigger_time);
        gaf_stream_common_update_capture_stream_state(pStreamEnv, GAF_CAPTURE_STREAM_STREAMING_TRIGGERED);
    }
    struct lea_offload_config_t* lea_offload_cfg =(struct lea_offload_config_t*)get_lea_offload_cfg_param();
    uint8_t adma_ch = lea_offload_cfg->offload_capture_ch;
#ifdef CP_IN_SAME_EE
    uint32_t dma_base = AUDMA_BASE;
#else
    uint32_t dma_base = SYS_DMA2_BASE;
#endif
    bt_drv_reg_op_enable_dma_tc(adma_ch&0xFF, dma_base);

    if (NULL == gaf_offload_capture_tunning_info.gaf_offload_tuning_timer_id)
    {
        gaf_offload_capture_tunning_info.gaf_offload_tuning_timer_id = 
            hwtimer_alloc(gaf_offload_capture_tuning_handler, NULL);
    }
    hwtimer_start(gaf_offload_capture_tunning_info.gaf_offload_tuning_timer_id, US_TO_TICKS((trigger_time - current_bt_time + 8700)));
}

bool gaf_offload_audio_stream_start(GAF_MEDIA_DWELLING_INFO_T* pDewellingInfo, GAF_AUDIO_STREAM_ENV_T* pStreamEnv, uint8_t con_lid)
{
    app_sysfreq_req(APP_SYSFREQ_USER_AOB, APP_SYSFREQ_208M);
    app_bt_set_access_mode(BTIF_BAM_NOT_ACCESSIBLE);
    bta_bt_write_sleep_enable(BTA_BT_SLEEP_GAF_OFFLOAD_STREAM, 0);
    if ((pDewellingInfo->startedStreamTypes&GAF_AUDIO_STREAM_TYPE_PLAYBACK) &&
        (GAF_PLAYBACK_STREAM_IDLE == pStreamEnv->stream_context.playback_stream_state))
    {
        if (GAF_CAPTURE_STREAM_IDLE == pStreamEnv->stream_context.capture_stream_state)
        {
            btdrv_set_iso_intr(1);
            gaf_cp_buf_init();
            gaf_cp_start_process();
            osDelay(5);
        }
        gaf_offload_playback_tunning_info.pStreamEnv_ptr = pStreamEnv;
        gaf_offload_codec_cfg.codec_cfg = (void*)&pStreamEnv->stream_info.playbackInfo.codec_info;
        gaf_offload_codec_cfg.startedStreamTypes = GAF_OFFLOAD_STREAM_PLAYBACK_START;
        gaf_offload_codec_cfg.pStreamEnv_ptr = pStreamEnv;
        hal_mcu2cp_send_mcu(HAL_MCU2CP_ID_0, (const unsigned char *)&gaf_offload_codec_cfg, sizeof(gaf_offload_message));
        osDelay(20);
        LOG_I("both GAF_AUDIO_STREAM_TYPE_PLAYBACK %d", con_lid);
        pStreamEnv->stream_context.playbackTriggerChannel = 1;
        gaf_media_prepare_playback_trigger(pStreamEnv->stream_context.playbackTriggerChannel);
        codec_int_stream_start(AUD_STREAM_PLAYBACK);
        gaf_stream_common_update_playback_stream_state(pStreamEnv, GAF_PLAYBACK_STREAM_INITIALIZING);
    }

    if ((pDewellingInfo->startedStreamTypes&GAF_AUDIO_STREAM_TYPE_CAPTURE) &&
        (GAF_CAPTURE_STREAM_IDLE == pStreamEnv->stream_context.capture_stream_state))
    {
        // return false;
        if (GAF_PLAYBACK_STREAM_IDLE == pStreamEnv->stream_context.playback_stream_state)
        {
            btdrv_set_iso_intr(1);
            gaf_cp_buf_init();
            gaf_cp_start_process();
            osDelay(5);
        }
        gaf_offload_capture_tunning_info.pStreamEnv_ptr = pStreamEnv;
        gaf_offload_codec_cfg.codec_cfg = (void*)&pStreamEnv->stream_info.captureInfo.codec_info;
        gaf_offload_codec_cfg.startedStreamTypes = GAF_OFFLOAD_STREAM_CAPTURE_START;
        gaf_offload_codec_cfg.pStreamEnv_ptr = pStreamEnv;
        hal_mcu2cp_send_mcu(HAL_MCU2CP_ID_0, (const unsigned char *)&gaf_offload_codec_cfg, sizeof(gaf_offload_message));
        osDelay(20);
        LOG_I("both GAF_AUDIO_STREAM_TYPE_CAPTURE %d", con_lid);
        pStreamEnv->stream_context.captureTriggerChannel = 0;
        gaf_media_prepare_capture_trigger(pStreamEnv->stream_context.captureTriggerChannel);
        codec_int_stream_start(AUD_STREAM_CAPTURE);
        gaf_stream_common_update_capture_stream_state(pStreamEnv, GAF_CAPTURE_STREAM_INITIALIZING);
        gaf_audio_flexible_capture_dma_dsp_start_handler(con_lid);
    }
    return true;
}

void gaf_offload_stop_playback_stream(GAF_AUDIO_STREAM_ENV_T* pStreamEnv)
{
    if ((GAF_PLAYBACK_STREAM_IDLE != pStreamEnv->stream_context.playback_stream_state) && (pStreamEnv->stream_info.bap_contextType == BES_BLE_GAF_CONTEXT_TYPE_GAME_BIT))
    {
        LOG_I("%s",__func__);
        hwtimer_stop(gaf_offload_playback_tunning_info.gaf_offload_tuning_timer_id);
        gaf_offload_codec_cfg.startedStreamTypes = GAF_OFFLOAD_STREAM_PLAYBACK_STOP;
        hal_mcu2cp_send_mcu(HAL_MCU2CP_ID_0, (const unsigned char *)&gaf_offload_codec_cfg, sizeof(gaf_offload_message));
    }
}

void gaf_offload_stop_capture_stream(GAF_AUDIO_STREAM_ENV_T* pStreamEnv)
{
    if ((GAF_CAPTURE_STREAM_IDLE != pStreamEnv->stream_context.capture_stream_state) && (pStreamEnv->stream_info.bap_contextType == BES_BLE_GAF_CONTEXT_TYPE_GAME_BIT))
    {
        LOG_I("%s",__func__);
        hwtimer_stop(gaf_offload_capture_tunning_info.gaf_offload_tuning_timer_id);
        gaf_offload_codec_cfg.startedStreamTypes = GAF_OFFLOAD_STREAM_CAPTURE_STOP;
        hal_mcu2cp_send_mcu(HAL_MCU2CP_ID_0, (const unsigned char *)&gaf_offload_codec_cfg, sizeof(gaf_offload_message));
    }
}

/*===============================================================================*/
// The follwing part is dongle part code
/*===============================================================================*/

struct lea_offload_tuning_info_t mobile_playback_tunning_info = {
    .pStreamEnv_ptr = NULL,
    .gaf_offload_tuning_timer_id = NULL,
    .ref_diff_id = 0,
    .iso_interval = ULL_MODE_FRAME,
};

struct lea_offload_tuning_info_t mobile_capture_tunning_info = {
    .pStreamEnv_ptr = NULL,
    .gaf_offload_tuning_timer_id = NULL,
    .ref_diff_id = 0,
    .iso_interval = ULL_MODE_FRAME,
};
#ifdef BLE_USB_AUDIO_IS_DONGLE_ROLE
void gaf_mobile_offload_close_cp(void)
{
    uint8_t enabled_playback_ase_cnt = gaf_mobile_audio_get_enabled_playback_ase_count();
    uint8_t enabled_capture_ase_cnt = gaf_mobile_audio_get_enabled_capture_ase_count();
    if ((0 == enabled_playback_ase_cnt) && (0 == enabled_capture_ase_cnt))
    {
        btdrv_set_iso_intr(0);
        gaf_offload_codec_cfg.startedStreamTypes = GAF_OFFLOAD_BUF_DEINITED;
        hal_mcu2cp_send_mcu(HAL_MCU2CP_ID_0, (const unsigned char *)&gaf_offload_codec_cfg, sizeof(gaf_offload_message));
        app_bt_set_access_mode(BTIF_BAM_CONNECTABLE_ONLY);
        app_sysfreq_req(APP_SYSFREQ_USER_AOB, APP_SYSFREQ_32K);
        bta_bt_write_sleep_enable(BTA_BT_SLEEP_GAF_OFFLOAD_STREAM, 1);
        osDelay(10);
        gaf_cp_stop_process();
    }
}
#endif

POSSIBLY_UNUSED static void gaf_mobile_offload_playback_tuning_handler(void *argument) 
{
    uint32_t btclk, lock; //hal slot -- 312.5us
    uint16_t btcnt; //hal  microsecond -- 0.5 us
    struct lea_offload_config_t* lea_offload_cfg =(struct lea_offload_config_t*)get_lea_offload_cfg_param();
    uint8_t adma_ch = lea_offload_cfg->offload_playback_ch;
    static uint32_t fist_iso_cd = 0;
    static uint8_t num = 0;
    POSSIBLY_UNUSED float ratio_test = 0.00000f;
#ifdef CP_IN_SAME_EE
    uint32_t dma_base = AUDMA_BASE;
#else
    uint32_t dma_base = SYS_DMA2_BASE;
#endif
    lock = int_lock();
    bt_drv_reg_op_dma_tc_clkcnt_get_by_ch(&btclk, &btcnt, adma_ch&0xFF, dma_base);
    uint32_t dmaIrqHappeningTimeUs = bt_syn_ble_bt_time_to_bts(btclk, btcnt);

    uint32_t iso_event_timestamp = gaf_media_common_get_latest_rx_iso_evt_timestamp(mobile_playback_tunning_info.pStreamEnv_ptr);
    uint32_t current_bt_time = gaf_media_sync_get_curr_time();

    if ((iso_event_timestamp - current_bt_time) > mobile_playback_tunning_info.iso_interval)
    {
        TRACE(0, "iso_event_timestamp more than current_bt_time too much");
        iso_event_timestamp -= mobile_playback_tunning_info.iso_interval;
    }
    

    for (int cnt = 0; cnt < 10; cnt++)
    {
        if (iso_event_timestamp >= dmaIrqHappeningTimeUs)
        {
            break;
        }
        TRACE(0, "iso_event_timestamp less than dmaIrqHappeningTimeUs");
        iso_event_timestamp += mobile_playback_tunning_info.iso_interval;
    }
    uint32_t diff_id = iso_event_timestamp - dmaIrqHappeningTimeUs;
    if (diff_id > mobile_playback_tunning_info.iso_interval)
    {
        TRACE(0, "diff_id value anomaly");
        diff_id -= mobile_playback_tunning_info.iso_interval;
    }
    POSSIBLY_UNUSED uint32_t diff_ic = iso_event_timestamp - current_bt_time;
    uint32_t diff_cd = current_bt_time - dmaIrqHappeningTimeUs;
    uint32_t diff_diff = 0;
    if (mobile_playback_tunning_info.ref_diff_id == 0)
    {
        mobile_playback_tunning_info.ref_diff_id = diff_id;
    }
    if (fist_iso_cd)
    {
        diff_diff = diff_cd - fist_iso_cd;
    }
    else
    {
        fist_iso_cd = diff_cd;
    }
    LOG_I("dma %u iso %u cur %u diff_diff %d", dmaIrqHappeningTimeUs, iso_event_timestamp, current_bt_time,diff_diff);
    hwtimer_start(mobile_playback_tunning_info.gaf_offload_tuning_timer_id, US_TO_TICKS(mobile_playback_tunning_info.iso_interval - diff_diff + 2000000));
    int_unlock(lock);

    // mobile side do not need tuning.
    if (diff_id < mobile_playback_tunning_info.ref_diff_id - 1)
    {
        // TRACE(0, "%s: tunning up",__func__);
        ratio_test = 0.00000002f;
    }
    else if (diff_id > mobile_playback_tunning_info.ref_diff_id + 1)
    {
        // TRACE(0, "%s: tunning down",__func__);
        ratio_test = -0.00000002f;
    }
#if defined(__AUDIO_RESAMPLE__)
    hal_codec_tune_resample_rate(AUD_STREAM_PLAYBACK, ratio_test);
#else
    analog_aud_pll_tune(ratio_test);
#endif
    app_sysfreq_req(APP_SYSFREQ_USER_AOB, APP_SYSFREQ_208M);
    LOG_I("%s:dma %u iso %u cur %u num %d", __func__, dmaIrqHappeningTimeUs, iso_event_timestamp, current_bt_time,num);
    LOG_I("diff_id %d diff_ic %d diff_diff %d ref_diff_id %d", diff_id, diff_ic, diff_diff, mobile_playback_tunning_info.ref_diff_id);
    num++;
}

void gaf_mobile_audio_flexible_playback_dma_dsp_start_handler()
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = mobile_playback_tunning_info.pStreamEnv_ptr;
    uint32_t iso_interval = mobile_playback_tunning_info.iso_interval;
    uint32_t iso_event_timestamp = gaf_media_common_get_latest_rx_iso_evt_timestamp(pStreamEnv);
    mobile_playback_tunning_info.ref_diff_id = 0;
    struct lea_offload_config_t* lea_offload_cfg =(struct lea_offload_config_t*)get_lea_offload_cfg_param();
    uint8_t adma_ch = lea_offload_cfg->offload_playback_ch;
#ifdef CP_IN_SAME_EE
    uint32_t dma_base = AUDMA_BASE;
#else
    uint32_t dma_base = SYS_DMA2_BASE;
#endif
    bt_drv_reg_op_disable_dma_tc(adma_ch&0xFF, dma_base);

    if (!iso_event_timestamp)
    {
        iso_event_timestamp = gaf_media_common_get_latest_tx_iso_evt_timestamp(pStreamEnv);
    }
    if (iso_event_timestamp)
    {
        // iso_evnt
        uint32_t current_bt_time = gaf_media_sync_get_curr_time();
        if (iso_event_timestamp < current_bt_time)
        {
            iso_event_timestamp += iso_interval;
        }
#if defined(__AUDIO_RESAMPLE__)
        uint32_t trigger_time = iso_event_timestamp + iso_interval + 310;
#else
        uint32_t trigger_time = iso_event_timestamp + iso_interval + 210;
#endif
        //iso_event_timestamp + iso_interval - 690
        gaf_stream_common_update_playback_stream_state(pStreamEnv, GAF_PLAYBACK_STREAM_START_TRIGGERING);
        gaf_stream_common_set_playback_trigger_time(pStreamEnv, trigger_time);
        gaf_stream_common_update_playback_stream_state(pStreamEnv, GAF_PLAYBACK_STREAM_STREAMING_TRIGGERED);
        // codec_int_stream_start(AUD_STREAM_PLAYBACK);
        LOG_I("trigger_time %u iso %u cur %u ", trigger_time, iso_event_timestamp, current_bt_time);
        bt_drv_reg_op_enable_dma_tc(adma_ch&0xFF, dma_base);
        if (NULL == mobile_playback_tunning_info.gaf_offload_tuning_timer_id)
        {
            mobile_playback_tunning_info.gaf_offload_tuning_timer_id = 
                hwtimer_alloc(gaf_mobile_offload_playback_tuning_handler, NULL);
        }
        hwtimer_start(mobile_playback_tunning_info.gaf_offload_tuning_timer_id, MS_TO_TICKS(15));
    }
}

POSSIBLY_UNUSED static void gaf_mobile_offload_capture_tuning_handler(void *argument) 
{
    uint32_t btclk, lock; //hal slot -- 312.5us
    uint16_t btcnt; //hal  microsecond -- 0.5 us
    struct lea_offload_config_t* lea_offload_cfg =(struct lea_offload_config_t*)get_lea_offload_cfg_param();
    uint8_t adma_ch = lea_offload_cfg->offload_capture_ch;
    static uint32_t fist_iso_cd = 0;
    static uint8_t num = 0;
    POSSIBLY_UNUSED float ratio_test = 0.00000f;
#ifdef CP_IN_SAME_EE
    uint32_t dma_base = AUDMA_BASE;
#else
    uint32_t dma_base = SYS_DMA2_BASE;
#endif
    lock = int_lock();
    bt_drv_reg_op_dma_tc_clkcnt_get_by_ch(&btclk, &btcnt, adma_ch&0xFF, dma_base);
    uint32_t dmaIrqHappeningTimeUs = bt_syn_ble_bt_time_to_bts(btclk, btcnt);

    uint32_t iso_event_timestamp = gaf_media_common_get_latest_tx_iso_evt_timestamp(mobile_capture_tunning_info.pStreamEnv_ptr);
    uint32_t current_bt_time = gaf_media_sync_get_curr_time();

    if ((iso_event_timestamp - current_bt_time) > mobile_capture_tunning_info.iso_interval)
    {
        TRACE(0, "iso_event_timestamp more than current_bt_time too much");
        iso_event_timestamp -= mobile_capture_tunning_info.iso_interval;
    }
    

    for (int cnt = 0; cnt < 10; cnt++)
    {
        if (iso_event_timestamp >= dmaIrqHappeningTimeUs)
        {
            break;
        }
        TRACE(0, "iso_event_timestamp less than dmaIrqHappeningTimeUs");
        iso_event_timestamp += mobile_capture_tunning_info.iso_interval;
    }
    uint32_t diff_id = iso_event_timestamp - dmaIrqHappeningTimeUs;
    if (diff_id > mobile_capture_tunning_info.iso_interval)
    {
        TRACE(0, "diff_id value anomaly");
        diff_id -= mobile_capture_tunning_info.iso_interval;
    }
    POSSIBLY_UNUSED uint32_t diff_ic = iso_event_timestamp - current_bt_time;
    uint32_t diff_cd = current_bt_time - dmaIrqHappeningTimeUs;
    uint32_t diff_diff = 0;
    if (mobile_capture_tunning_info.ref_diff_id == 0)
    {
        mobile_capture_tunning_info.ref_diff_id = diff_id;
    }
    if (fist_iso_cd)
    {
        diff_diff = diff_cd - fist_iso_cd;
    }
    else
    {
        fist_iso_cd = diff_cd;
    }
    LOG_I("dma %u iso %u cur %u diff_diff %d", dmaIrqHappeningTimeUs, iso_event_timestamp, current_bt_time,diff_diff);
    hwtimer_start(mobile_capture_tunning_info.gaf_offload_tuning_timer_id, US_TO_TICKS(mobile_capture_tunning_info.iso_interval - diff_diff + 2000000));
    int_unlock(lock);

    // mobile side do not need tuning.
    if (diff_id < mobile_capture_tunning_info.ref_diff_id - 1)
    {
        // TRACE(0, "%s: tunning up",__func__);
        ratio_test = 0.00000002f;
    }
    else if (diff_id > mobile_capture_tunning_info.ref_diff_id + 1)
    {
        // TRACE(0, "%s: tunning down",__func__);
        ratio_test = -0.00000002f;
    }
#if defined(__AUDIO_RESAMPLE__)
    hal_codec_tune_resample_rate(AUD_STREAM_CAPTURE, ratio_test);
#else
    analog_aud_pll_tune(ratio_test);
#endif

    LOG_I("%s:dma %u iso %u cur %u num %d", __func__, dmaIrqHappeningTimeUs, iso_event_timestamp, current_bt_time,num);
    LOG_I("diff_id %d diff_ic %d diff_diff %d ref_diff_id %d", diff_id, diff_ic, diff_diff, mobile_capture_tunning_info.ref_diff_id);
    num++;
}

void gaf_mobile_audio_flexible_capture_dma_dsp_start_handler()
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = mobile_capture_tunning_info.pStreamEnv_ptr;
    uint32_t iso_interval = mobile_capture_tunning_info.iso_interval;
    uint32_t iso_event_timestamp = gaf_media_common_get_latest_tx_iso_evt_timestamp(pStreamEnv);
    struct lea_offload_config_t* lea_offload_cfg =(struct lea_offload_config_t*)get_lea_offload_cfg_param();
    uint8_t adma_ch = lea_offload_cfg->offload_capture_ch;
#ifdef CP_IN_SAME_EE
    uint32_t dma_base = AUDMA_BASE;
#else
    uint32_t dma_base = SYS_DMA2_BASE;
#endif
    mobile_capture_tunning_info.ref_diff_id = 0;
    bt_drv_reg_op_disable_dma_tc(adma_ch&0xFF, dma_base);

    if (iso_event_timestamp)
    {
        // iso_evnt
        uint32_t current_bt_time = gaf_media_sync_get_curr_time();
        if (iso_event_timestamp < current_bt_time)
        {
            iso_event_timestamp += iso_interval;
        }
#if defined(__AUDIO_RESAMPLE__)
        uint32_t trigger_time = iso_event_timestamp + iso_interval + 310;
#else
        uint32_t trigger_time = iso_event_timestamp + iso_interval + 210;
#endif
        //iso_event_timestamp + iso_interval - 690
        gaf_stream_common_update_capture_stream_state(pStreamEnv, GAF_CAPTURE_STREAM_START_TRIGGERING);
        gaf_stream_common_set_capture_trigger_time(pStreamEnv, trigger_time);
        gaf_stream_common_update_capture_stream_state(pStreamEnv, GAF_CAPTURE_STREAM_STREAMING_TRIGGERED);
        LOG_I("trigger_times %u iso %u cur %u ", trigger_time, iso_event_timestamp, current_bt_time);
    }
    bt_drv_reg_op_enable_dma_tc(adma_ch&0xFF, dma_base);
    if (NULL == mobile_capture_tunning_info.gaf_offload_tuning_timer_id)
    {
        mobile_capture_tunning_info.gaf_offload_tuning_timer_id = 
            hwtimer_alloc(gaf_mobile_offload_capture_tuning_handler, NULL);
    }
    hwtimer_start(mobile_capture_tunning_info.gaf_offload_tuning_timer_id, MS_TO_TICKS(15));
}

bool gaf_mobile_offload_audio_stream_start(GAF_MEDIA_DWELLING_INFO_T* pDewellingInfo, GAF_AUDIO_STREAM_ENV_T* pStreamEnv, uint8_t con_lid)
{
    app_sysfreq_req(APP_SYSFREQ_USER_AOB, APP_SYSFREQ_208M);
    app_bt_set_access_mode(BTIF_BAM_NOT_ACCESSIBLE);
    bta_bt_write_sleep_enable(BTA_BT_SLEEP_GAF_OFFLOAD_STREAM, 0);
    if ((pDewellingInfo->startedStreamTypes&GAF_AUDIO_STREAM_TYPE_PLAYBACK) &&
        (GAF_PLAYBACK_STREAM_IDLE == pStreamEnv->stream_context.playback_stream_state))
    {
// open cp core
#ifdef GAF_OFFLOAD_MIC
        pStreamEnv->stream_info.playbackInfo.codec_info.num_channels = 2;
#endif
        if (GAF_CAPTURE_STREAM_IDLE == pStreamEnv->stream_context.capture_stream_state)
        {
            btdrv_set_iso_intr(1);
            gaf_cp_buf_init();
            gaf_cp_start_process();
            osDelay(5);
        }
        mobile_playback_tunning_info.pStreamEnv_ptr = pStreamEnv;
        gaf_offload_codec_cfg.codec_cfg = (void*)&pStreamEnv->stream_info.playbackInfo.codec_info;
        gaf_offload_codec_cfg.startedStreamTypes = GAF_OFFLOAD_MOBILE_STREAM_PLAYBACK_START;
        gaf_offload_codec_cfg.pStreamEnv_ptr = pStreamEnv;
        hal_mcu2cp_send_mcu(HAL_MCU2CP_ID_0, (const unsigned char *)&gaf_offload_codec_cfg, sizeof(gaf_offload_message));
        osDelay(20);
        LOG_I("both GAF_AUDIO_STREAM_TYPE_PLAYBACK %d", con_lid);
// set trigger channel to make sure codec dam timer fixed
        gaf_mobile_capture_stream_enter_started_state();
        mobile_playback_tunning_info.pStreamEnv_ptr->stream_context.playbackTriggerChannel = 1;
        gaf_media_prepare_playback_trigger(mobile_playback_tunning_info.pStreamEnv_ptr->stream_context.playbackTriggerChannel);
// open codec
        codec_int_stream_start(AUD_STREAM_PLAYBACK);
        gaf_stream_common_update_playback_stream_state(pStreamEnv, GAF_PLAYBACK_STREAM_INITIALIZING);
        gaf_mobile_audio_flexible_playback_dma_dsp_start_handler();
    }

    if ((pDewellingInfo->startedStreamTypes&GAF_AUDIO_STREAM_TYPE_CAPTURE) &&
        (GAF_CAPTURE_STREAM_IDLE == pStreamEnv->stream_context.capture_stream_state))
    {
        if (GAF_PLAYBACK_STREAM_IDLE == pStreamEnv->stream_context.playback_stream_state)
        {
            btdrv_set_iso_intr(1);
            gaf_cp_buf_init();
            gaf_cp_start_process();
            osDelay(5);
        }
        mobile_capture_tunning_info.pStreamEnv_ptr = pStreamEnv;
        gaf_offload_codec_cfg.codec_cfg = (void*)&pStreamEnv->stream_info.captureInfo.codec_info;
        gaf_offload_codec_cfg.startedStreamTypes = GAF_OFFLOAD_MOBILE_STREAM_CAPTURE_START;
        gaf_offload_codec_cfg.pStreamEnv_ptr = pStreamEnv;
        hal_mcu2cp_send_mcu(HAL_MCU2CP_ID_0, (const unsigned char *)&gaf_offload_codec_cfg, sizeof(gaf_offload_message));
        osDelay(20);
        LOG_I("both GAF_AUDIO_STREAM_TYPE_CAPTURE %d", con_lid);
        gaf_mobile_playback_stream_enter_started_state();
        mobile_capture_tunning_info.pStreamEnv_ptr ->stream_context.captureTriggerChannel = 0;
        gaf_media_prepare_capture_trigger(mobile_capture_tunning_info.pStreamEnv_ptr ->stream_context.captureTriggerChannel);
        codec_int_stream_start(AUD_STREAM_CAPTURE);
        gaf_stream_common_update_capture_stream_state(pStreamEnv, GAF_CAPTURE_STREAM_INITIALIZING);
        gaf_mobile_audio_flexible_capture_dma_dsp_start_handler();
    }
    return true;
}

void gaf_mobile_offload_stop_playback_stream(void)
{
    LOG_I("%s",__func__);
    codec_int_stream_stop(AUD_STREAM_PLAYBACK);
    hwtimer_stop(mobile_playback_tunning_info.gaf_offload_tuning_timer_id);
    gaf_offload_codec_cfg.startedStreamTypes = GAF_OFFLOAD_MOBILE_STREAM_PLAYBACK_STOP;
    hal_mcu2cp_send_mcu(HAL_MCU2CP_ID_0, (const unsigned char *)&gaf_offload_codec_cfg, sizeof(gaf_offload_message));
}

void gaf_mobile_offload_stop_capture_stream(void)
{
    LOG_I("%s",__func__);
    codec_int_stream_stop(AUD_STREAM_CAPTURE);
    hwtimer_stop(mobile_capture_tunning_info.gaf_offload_tuning_timer_id);
    gaf_offload_codec_cfg.startedStreamTypes = GAF_OFFLOAD_MOBILE_STREAM_CAPTURE_STOP;
    hal_mcu2cp_send_mcu(HAL_MCU2CP_ID_0, (const unsigned char *)&gaf_offload_codec_cfg, sizeof(gaf_offload_message));
}
#endif