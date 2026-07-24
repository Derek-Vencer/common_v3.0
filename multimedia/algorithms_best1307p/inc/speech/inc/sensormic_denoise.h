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
#ifndef __SENSORMIC_DENOISE_H__
#define __SENSORMIC_DENOISE_H__

#include <stdint.h>
#include "speech_common.h"
#include "custom_allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t     bypass;
    int32_t     wnr_enable;
    int32_t     mode_flag;
    //af
    int32_t     outer_echo_af_enable;
    // float       outer_af_error_threshold;
    // float       fb_af_error_threshold;
    // int32_t     outer_af_filter_len;
    // int32_t     fb_af_filter_len;
    //nlp
    int32_t     echo_supp_enable;
    int32_t     ref_delay;
    int32_t     outer_echo_band_start;
    int32_t     outer_echo_band_end;
    int32_t     fb_echo_band_start;
    int32_t     fb_echo_band_end;

    int32_t     post_supp_enable;
    float       pf_denoise_db;
    //change with mode
    float       inner_denoise_db;
    // const float*      fb_filter;
    float       fb_energy_diff_min;
    float       fb_energy_diff_max;
    // crossover
    int32_t     blend_en;
    int32_t     wind_blend_band[2];

    int32_t     comp_num;
    int32_t     comp_freq[MAX_COMP_NUM];
    int32_t     comp_gaindB[MAX_COMP_NUM];
    
    // wdrc
    int32_t     wdrc_enable;
    // gain
    float       pre_gain;
    float       post_gain;
} SensorMicDenoiseConfig;

struct SensorMicDenoiseState_;

typedef struct SensorMicDenoiseState_ SensorMicDenoiseState;

SensorMicDenoiseState *sensormic_denoise_create(void *inner_modeldata, void *outer_modeldata, int32_t sample_rate, int32_t frame_size, const SensorMicDenoiseConfig *cfg, custom_allocator *allocator);

int32_t sensormic_denoise_destroy(SensorMicDenoiseState *st);

int32_t sensormic_denoise_set_config(SensorMicDenoiseState *st, const SensorMicDenoiseConfig *cfg);

int32_t sensormic_denoise_reset_config_with_mode(SensorMicDenoiseState *st, const SensorMicDenoiseConfig *cfg);

int32_t sensormic_denoise_process(SensorMicDenoiseState *st, short* pcm_buf, short *ref_buf, int32_t pcm_len, short* out_buf);

float sensormic_denoise_get_required_mips(SensorMicDenoiseState *st);

bool sensormic_denoise_get_wind_status(SensorMicDenoiseState *st);

void sensormic_denoise_set_af_state(SensorMicDenoiseState *st, void *af, FILTER_RESET_HANDLER handler);

#ifdef __cplusplus
}
#endif

#endif
