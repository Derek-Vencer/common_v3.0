/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#if BLE_AUDIO_ENABLED
#include "bluetooth_bt_api.h"
#include "app_bt_func.h"
#include "app_utils.h"
#include "audio_dump.h"
#include "audioflinger.h"
#include "cqueue.h"
#include "hal_aud.h"
#include "hal_trace.h"

#include "bluetooth_ble_api.h"
#include "gaf_media_pid.h"

// Max allowed total tune ratio (5000ppm)
#define MAX_TOTAL_TUNE_RATIO        0.0005000

void gaf_media_pid_init(gaf_media_pid_t* pStreamPid)
{
    memset(pStreamPid, 0, sizeof(gaf_media_pid_t));
    pStreamPid->Kp = 0.000018;
    pStreamPid->Ki = 0.000005;
    pStreamPid->Kd = 0.000008;
}

void gaf_usb_media_pid_init(gaf_media_pid_t* pStreamPid)
{
    memset(pStreamPid, 0, sizeof(gaf_media_pid_t));
    // resample mode 3.6 3.6 -0.005
    pStreamPid->Kp = 1.6;
    pStreamPid->Ki = 1.2;
    pStreamPid->Kd = 0.001;
}

float gaf_offload_pid_compute(gaf_media_pid_t* pid, float setpoint, float measured, float dt) {
    float error = setpoint - measured;
    pid->integral += error * dt;
    float derivative = (error - pid->prev_error) / dt;

    float output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;
    pid->prev_error = error;
    // TRACE(0,"pid->Kp error %d pid->Ki pid->integral %d pid->Kd derivative %d", (int)(pid->Kp * error*1000), (int)(pid->Ki * pid->integral*1000), (int)(pid->Kd * derivative*1000));
    return output;
}

void gaf_usb_media_pid_adjust(uint8_t streamType, gaf_media_pid_t* pStreamPid, uint32_t ref_diff_id, uint32_t diff_id)
{
    float ratio_test = (0.000002f * gaf_offload_pid_compute(pStreamPid, ref_diff_id, diff_id, 0.005f));
    af_codec_tune((enum AUD_STREAM_T)streamType, ratio_test);
}

void gaf_media_pid_adjust(uint8_t streamType, gaf_media_pid_t* pStreamPid, int32_t diff_time)
{
    float result_P, result_I, result_D;

    pStreamPid->diff_time0 = diff_time;
    result_P = pStreamPid->Kp * (float)(pStreamPid->diff_time0 - pStreamPid->diff_time1);
    result_I = pStreamPid->Ki * (float)pStreamPid->diff_time0;
    result_D = pStreamPid->Kd * (float)(pStreamPid->diff_time0 - 2 * pStreamPid->diff_time1 + pStreamPid->diff_time2);
    pStreamPid->result += (result_P + result_I + result_D);
    if (pStreamPid->result > MAX_TOTAL_TUNE_RATIO)
    {
        pStreamPid->result = MAX_TOTAL_TUNE_RATIO;
    }
    else if (pStreamPid->result < -MAX_TOTAL_TUNE_RATIO)
    {
        pStreamPid->result = -MAX_TOTAL_TUNE_RATIO;
    }

    pStreamPid->diff_time2 = pStreamPid->diff_time1;
    pStreamPid->diff_time1 = pStreamPid->diff_time0;

    af_codec_tune((enum AUD_STREAM_T)streamType, pStreamPid->result);
}

void gaf_media_pid_update_threshold(gaf_media_pid_t* pStreamPid, int32_t threshold_us)
{
    pStreamPid->gap_threshold_us = threshold_us;
}
#endif

/// @} APP
