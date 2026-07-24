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
#if defined(SPEECH_TX_1MIC_NS)
    .tx_1mic_preaf_cfg = {
        .bypass             = 0,
        .ref_delay          = 0,
        .error_threshold    = 1.5e-7,
        .filter_len         = 32,
        .ref_pre_scale      =1.0,
    },
#endif
#if defined(SPEECH_TX_1MIC_NS)
    .tx_1mic_ns_cfg = {
        .bypass             = 0,
        .nn_gain_db         = 15,
        .pf_enabled         = true,
        .pf_denoise_db      = -8,
        .echo_supp_enabled  = true,
        .af_enabled         = true,
        .hfsupp_enabled     = false,
        .nlp_enabled        = true,
        .ref_delay          = 0,
        .gamma              = 0.8,
        .echo_band_start    = 1000,
        .echo_band_end      = 5000,
        .min_ovrd           = 2,
        .target_supp        = -20,
        .highfre_band_start = 4000,
        .highfre_supp       = 8.f,
        .ref_thd            = 1000.f,
        .reset_ec_thd       = 1.03,
        .wdrc_enabled        = 1,
        .wdrc_CT            = -5,
        .wdrc_CS            = 0.6667f,
        .wdrc_WT            = -52,
        .wdrc_WS            = 0,
        .wdrc_ET            = -75,
        .wdrc_ES            = -0.95f,
        .wdrc_G             = 0.f,
        .post_gain          = 0,
        .pcen_enable        = 0,
        .pcen_thd           = 0.25,
        .gru_enable         = 0,
        .gru_energy_db_outer= -40,
        .gru_energy_db_inner= -40,
        .gru_frame_cnt      = 300,
        .gru_snr            = 10,
    },
#endif
#if defined(SPEECH_TX_2MIC_PREAF)
    .tx_2mic_preaf_cfg = {
        .bypass                  = 0,
        .ref_delay               = 0,
        .talk_error_threshold    = 1.5e-7,
        .ff_error_threshold      = 1.5e-8,
        .talk_filter_len         = 32,
        .ff_filter_len           = 32,
        .ref_pre_scale           = 1.0,
    },
#endif
#if defined(SPEECH_TX_2MIC_NS8)
    .tx_2mic_ns8_cfg = {
        .bypass             = 0,
        .dist               = 0.028f,
        .min_frequency      = 150,
        .max_frequency      = 3000,
        .ows_no_bf_enable   = 0,//when tws,enable = 0; when ows earclip，enable = 1, echo_af_enable = 1,open SPEECH_TX_2MIX_PREAF;
        .energy_threshold   = 1.5f,
        .corr_threshold     = 0.9f,
        .fb_ratio           = 1.0f,
        .crossover_threshold= 1.0f,
        .Pb_energy_db       = -55,
        .update_snr         = -15,
        .wind_mic_switch_thr = 1.5,

        .wind_supp_enable   = 1,
        .echo_af_enable     = 0,
        .echo_supp_enable   = 1,
        .ref_delay          = 0,
        .gamma              = 0.8f,
        .echo_band_start    = 100,
        .echo_band_end      = 1800,
        .min_ovrd           = 3,

        .wdrc_enable        = 1,
        .wdrc_CT             = -10.f,
        .wdrc_CS             = 2.f,        //same as speech_tx_compexp comp_ratio
        .wdrc_ET             = -100.f,
        .wdrc_ES             = 0.5556f,    //same as speech_tx_compexp expand_ratio

        .talk_pre_gain      = -2,    //Calibrate 2-mic frequency response(reduce the talk mic), usually gain <=0
        .pre_gain           = 0,     //Amplify all microphones with pre gain
        .post_gain          = 14,
        .bf_ns_nowind_gain  = 20,
        .bf_ns_wind_gain    = 10,

        .post_supp_enable   = 1,
        .denoise_dB         = -5,
        .dnn_denoise_dB     = -35,
        .reset_ec_thd       = 1.0,
        .target_supp        = -20,
        .vad_min_frequency  = 160,
        .vad_max_frequency  = 8000,
        .corr_threshold1    = 0.6,
        .wind_gamma         = 0.5,
        .wind_band_num      = 2,
        .wind_band_freq     = {250, 1000},
    },
#endif
#if defined(SPEECH_TX_AEC2FLOAT)
    .tx_aec2float_cfg = {
        .bypass         = 0,
        .hpf_enabled    = false,
        .af_enabled     = false,
        .adprop_enabled    = false,
        .varistep_enabled    = false,
        .nlp_enabled    = true,
        .clip_enabled   = false,
        .stsupp_enabled = false,
        .hfsupp_enabled = false,
        .constrain_enabled = false,
        .ns_enabled     = true,
        .cng_enabled    = false,
        .blocks         = 1,
        .delay          = 70,
        .error_threshold = 1.5e-5f,
        .gamma          = 0.9,
        .echo_band_start = 300,
        .echo_band_end  = 1800,
        .min_ovrd       = 2,
        .target_supp    = -40,
        .highfre_band_start = 4000,
        .highfre_supp   = 8.f,
        .noise_supp     = -15,
        .cng_type       = 1,
        .cng_level      = -60,
        .clip_threshold = -20.f,
        .banks          = 64,
        .filter_len     = 32,
        .ref_thd        = 1000,
        .reset_ec_thd   = 1.03,
    },
#endif
#if defined(SPEECH_TX_NS4)
    .tx_ns4_cfg = {
        .bypass     = 0,
        .denoise_dB = -18,
    },
#endif
#if defined(SPEECH_TX_EQ)
    .tx_eq_cfg = {
        .bypass     = 0,
        .gain       = 0.f,
        .num        = 1,
        .params = {
            {IIR_BIQUARD_HPF, {{60, 0, 0.707f}}},
        },
    },
#endif
#if defined(SPEECH_TX_COMPEXP)
    .tx_compexp_cfg = {
        .bypass             = 0,
        .type               = 0,
        .comp_threshold     = -15.f,
        .comp_ratio         = 2.f,
        .expand_threshold   = -45.f,
        .expand_ratio       = 0.555f,
        .attack_time        = 0.001f,
        .release_time       = 0.006f,
        .makeup_gain        = 6,
        .delay              = 32,
        .tav                = 0.2f,
    },
#endif
#if defined(SPEECH_RX_NS2FLOAT)
    .rx_ns2float_cfg = {
        .bypass     = 0,
        .denoise_dB = -15,
        .banks      = 64,
        .wdrc_enable = 0,
        .expander_threshold = -70,
        .expander_ratio = 0.7,
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
        .bypass             = 0,
        .type               = 0,
        .comp_threshold     = -15.f,
        .comp_ratio         = 2.f,
        .expand_threshold   = -45.f,
        .expand_ratio       = 0.555f,
        .attack_time        = 0.001f,
        .release_time       = 0.006f,
        .makeup_gain        = 6,
        .delay              = 32,
        .tav                = 0.2f,
    },
#endif
};