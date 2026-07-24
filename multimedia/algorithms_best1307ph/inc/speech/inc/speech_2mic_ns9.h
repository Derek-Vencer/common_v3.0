/***************************************************************************
 *
 * Copyright 2015-2020 BES.
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


#ifndef __SPEECH_2MIC_NS9_H__
#define __SPEECH_2MIC_NS9_H__


#include <stdint.h>
#include "speech_common.h"
#include "custom_allocator.h"

typedef struct {
    int32_t         bypass;

    int32_t         echo_supp_enable;
    int32_t         ref_delay;
    float           gamma;
    int32_t         echo_band_start;
    int32_t         echo_band_end;
    int32_t         high_echo_band_start;
    int32_t         high_echo_band_end;
    float           min_ovrd;
    float           reset_ec_thd;
    int32_t         target_supp;
    
    int32_t         wdrc_enable;
    float           wdrc_CT;
    float           wdrc_CS;
    float           wdrc_ET;
    float           wdrc_ES;

    float           talk_pre_gain;
    float           pre_gain;
    float           post_gain;

    int32_t         post_supp_enable;
    int32_t         pf_denoise_db;

} Speech2MicNs9Config;

struct Speech2MicNs9State_;

typedef struct Speech2MicNs9State_ Speech2MicNs9State;

#ifdef __cplusplus
extern "C" {
#endif

Speech2MicNs9State *speech_2mic_ns9_create(void *modeldata, int32_t sample_rate, int32_t frame_size, Speech2MicNs9Config *cfg, custom_allocator *allocator);

int32_t speech_2mic_ns9_destroy(Speech2MicNs9State *st);

int32_t speech_2mic_ns9_process(Speech2MicNs9State *st, int16_t *pcm_buf, int16_t *ref_buf, int32_t pcm_len, int16_t *out_buf);

int32_t speech_2mic_ns9_set_config(Speech2MicNs9State *st, const Speech2MicNs9Config *cfg);

int32_t speech_2mic_ns9_get_delay(Speech2MicNs9State *st);

float speech_2mic_ns9_get_required_mips(Speech2MicNs9State *st);

void *speech_nn_ns_get_model_data_2mic_v0(void);

#ifdef __cplusplus
}
#endif

#endif
