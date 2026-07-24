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

#include "plat_types.h"
#include "speech_algo_cfg.h"

const SpeechAlgoConfig WEAK speech_cfg_default = {
#if defined(SPEECH_TX_AEC2FLOAT)
    .tx_aec2float_cfg = {
        .bypass = 0,
        .hpf_enabled = 0,
        .af_enabled = 1,
        .adprop_enabled = 0,
        .varistep_enabled = 0,
        .nlp_enabled = 1,
        .clip_enabled = 1,
        .stsupp_enabled = 0,
        .hfsupp_enabled = 0,
        .constrain_enabled = 0,
        .ns_enabled = 1,
        .cng_enabled = 0,
        .blocks = 1,
        .delay = 32,
        .gamma = 0.5,
        .echo_band_start = 300,
        .echo_band_end = 3600,
        .min_ovrd = 3,
        .target_supp = -50,
        .highfre_band_start = 5000,
        .highfre_supp = 8.f,
        .noise_supp = -15,
        .cng_type = 1,
        .cng_level = -60,
        .clip_threshold = -50.f,
        .banks = 64,
        .ref_thd = 1000,
    },
#endif

#if defined(SPEECH_TX_EQ)
    .tx_eq_cfg = {
        .bypass = 0,
        .gain   = 0.f,
        .num    = 1,
        .params = {
            {IIR_BIQUARD_HPF, {{60, 0, 0.707f}}},
        },
    },
#endif

#if defined(SPEECH_TX_COMPEXP)
    .tx_compexp_cfg = {
        .bypass =              0,
        .type =                0,
        .comp_threshold =      -18.f,
        .comp_ratio =          8.f,
        .expand_threshold =    -55.f,
        .expand_ratio =        0.555,
        .attack_time =         0.001f,
        .release_time =        0.002f,
        .makeup_gain =         6.f,
        .delay =               32,
        .tav =                 0.1
    },
#endif

#if defined(SPEECH_RX_NS2FLOAT)
    .rx_ns2float_cfg = {
        .bypass = 0,
        .denoise_dB = -16,
        .banks = 64,
#ifdef SPEECH_RX_COMPEXP
        .wdrc_enable = 0,
#else
        .wdrc_enable = 1,
#endif
        .expander_threshold = -100,
        .expander_ratio = 0.7f,
    },
#endif

#if defined(SPEECH_RX_EQ)
    .rx_eq_cfg = {
        .bypass = 0,
        .gain   = 0.f,
        .num    = 1,
        .params = {
            {IIR_BIQUARD_HPF, {{60, 0, 0.707f}}},
        },
    },
#endif

#if defined(SPEECH_RX_COMPEXP)
    .rx_compexp_cfg = {
        .bypass =              0,
        .type =                0,
        .comp_threshold =      -18.f,
        .comp_ratio =          8.f,
        .expand_threshold =    -55.f,
        .expand_ratio =        0.5,
        .attack_time =         0.001f,
        .release_time =        0.002f,
        .makeup_gain =         6.f,
        .delay =               32,
        .tav =                 0.1
    },
#endif
};