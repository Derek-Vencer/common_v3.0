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
 ****************************************************************************/


#ifndef __SPEECH_1MIC_NS_H__
#define __SPEECH_1MIC_NS_H__


#include <stdint.h>
#include <stdbool.h>
#include "custom_allocator.h"
#include "speech_common.h"

typedef struct {
    int32_t         bypass;
    int32_t         nn_gain_db;
    bool            pf_enabled;
    int32_t         pf_denoise_db;
    bool            echo_supp_enabled;
    bool            af_enabled;
    bool            hfsupp_enabled;
    bool            nlp_enabled;
    int32_t         ref_delay;
    float           gamma;
    int32_t         echo_band_start;
    int32_t         echo_band_end;
    float           min_ovrd;
    float           target_supp;
    int32_t         highfre_band_start;
    float           highfre_supp;
    float           ref_thd;
    float           reset_ec_thd;
    int32_t         wdrc_enabled;
    float           wdrc_CT;
    float           wdrc_CS;
    float           wdrc_WT;
    float           wdrc_WS;
    float           wdrc_ET;
    float           wdrc_ES;
    float           wdrc_G;
    float           post_gain;
    bool            pcen_enable;
    float           pcen_thd;
    bool            gru_enable;
    float           gru_energy_db_outer;
    float           gru_energy_db_inner;
    int32_t         gru_frame_cnt;
    float           gru_snr;
} Speech1MicNSConfig;

struct Speech1MicNSState_;

typedef struct Speech1MicNSState_ Speech1MicNSState;

#ifdef __cplusplus
extern "C" {
#endif

Speech1MicNSState *speech_1mic_ns_create(void *modeldata, Speech1MicNSConfig *cfg, custom_allocator *allocator);

int32_t speech_1mic_ns_destroy(Speech1MicNSState *st);

int32_t speech_1mic_ns_process(Speech1MicNSState *st, int16_t *pcm_buf, int16_t *ref_buf, int32_t pcm_len, int16_t *out_buf);

void *speech_nn_ns_get_model_data_93l(void);

void *speech_nn_ns_get_model_data_94l(void);

void *speech_nn_ns_get_model_data_95l(void);

void *speech_nn_ns_get_model_data_96l(void);

void *speech_nn_ns_get_model_data_95s(void);

void *speech_nn_ns_get_model_data_91s(void);

void *speech_nn_ns_get_model_data_100l(void);

void *speech_nn_ns_get_model_data_101l(void);

void *speech_nn_ns_get_model_data_102l(void);

void *speech_nn_ns_get_model_data_112l(void);

void speech_1mic_ns_set_af_state(Speech1MicNSState *st, void *af, FILTER_RESET_HANDLER handler);

#ifdef __cplusplus
}
#endif

#endif
