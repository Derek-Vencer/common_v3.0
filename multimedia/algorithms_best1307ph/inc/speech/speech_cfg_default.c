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
// TX: Transimt process
// RX: Receive process
// 16k: base 25M/208M(1300,1302) base 28M/104M(1400,1402)
| ----- | ------------------- | --------------------------------- | --------- | ------------- | ---------------------- |
| TX/RX | Algo                | description                       | MIPS(M)   | RAM(kB)       | Note                   |
|       |                     |                                   | 16k    8k | 16k        8k |                        |
| ----- | ------------------- | --------------------------------- | --------- | ------------- | ---------------------- |
|       | SPEECH_TX_DC_FILTER | Direct Current filter             | 1~2    \  | 0.05          |
|       | SPEECH_TX_AEC2FLOAT | Acoustic Echo Cancellation(new)   | 23/22  \  | 29.3          | nlp/af(blocks=1)       |
|       | SPEECH_TX_AEC2FLOAT | Acoustic Echo Cancellation(ns)    | 14/10  \  |               | banks=256/banks=128    |
|       | (ns_enabled)        |                                   | 8/6    \  |               | banks=64/banks=32      |
|       | SPEECH_TX_NS3       | 1 mic noise suppress(new)         | 26     \  | 33.3          |                        |
|       | SPEECH_TX_NS2FLOAT  | 1 mic noise suppress(new float)   | 12.5   \  |               | banks=64               |
| TX    | 
|       | SPEECH_TX_2MIC_NS4  | sensor mic noise suppress         | 31.5      |               |                        |
|       | SPEECH_TX_2MIC_NS7  | 2 mic noise suppr(Cohernt&RLS)    | 58.4      | 38.9          |                        |
|       | SPEECH_TX_2MIC_NS8  | 2 mic noise suppr(DSB)            | 3.5    \  |               |                        |
|       | SPEECH_TX_3MIC_NS   | 3 mic 2FF+FB NS(new)              | 80        | 33.1          | Wnr_enable = 0         |
|       |                     |                                   |           |               | include 3mic_preprocess|
|       | SPEECH_TX_3MIC_NS2  | 3 mic 2FF+FB NS (Cohernt&RLS)     | 62        | 58.7          |                        |
|       |                     |                                   |           |               | include 3mic_preprocess|
|       | SPEECH_TX_AGC       | Automatic Gain Control            | 3         | 0.4           |                        |
|       | SPEECH_TX_COMPEXP   | Compressor and expander           | 4         | 0.6           |                        |
|       | SPEECH_TX_EQ        | Default EQ                        | 0.5     \ | 1.1           | each section           |
| ----- | ------------------- | --------------------------------- | --------- | ------------- | ---------------------- |
| RX    | 
|       | SPEECH_RX_NS2FLOAT  | 1 mic noise suppress(new float)   | 12.5   \  | 17.9          | banks=64               |
|       | SPEECH_RX_AGC       | Automatic Gain Control            | 3         | 0.4           |                        |
|       | SPEECH_RX_EQ        | Default EQ                        | 0.5     \ | 1.1           | each section           |
| ----- | ------------------- | --------------------------------- | --------- | ------------- | ---------------------- |
**/
#include "plat_types.h"
#include "bt_sco_chain_cfg.h"

#if defined(SPEECH_TX_2MIC_NS4) || defined(SPEECH_TX_3MIC_PREAF)
  const float normal_calib_filter[128] = {
 1.145965267e-02f,  1.206589128e-01f,  1.758397343e-01f, -6.246553318e-02f,  1.336895552e-01f,
 1.082028146e-01f, -5.459512938e-02f,  1.530997148e-01f,  6.958618273e-02f, -4.680734937e-02f,
 8.212355062e-02f,  1.942954718e-02f, -2.334746491e-02f,  9.267938146e-02f,  1.970704612e-04f,
 1.156303273e-02f,  3.437917938e-02f,  3.678213597e-03f,  1.031119956e-02f,  4.913992266e-02f,
-2.747005522e-02f,  2.618704237e-02f,  2.372637816e-02f, -5.482094980e-03f,  1.442099763e-02f,
 2.180467528e-02f,  2.753382289e-03f,  1.516452672e-02f,  1.955607962e-02f, -6.549615358e-03f,
 1.399030902e-02f,  1.825539703e-04f,  9.520292497e-03f,  9.909736339e-03f,  9.436065964e-03f,
 6.062503340e-03f,  1.264082782e-02f,  3.324618249e-03f, -1.093181724e-03f,  1.267979621e-02f,
 2.485674849e-03f,  6.029026656e-03f,  6.030512212e-03f,  1.459835433e-02f, -3.881189402e-03f,
 1.254612884e-02f,  2.627920788e-03f,  3.417514707e-03f, -2.840038056e-04f,  1.012994862e-02f,
-4.427714288e-03f,  9.603694635e-03f,  7.143005943e-03f, -1.499412190e-03f,  9.731242990e-03f,
 5.208779430e-03f, -5.901023086e-03f,  7.198260964e-03f,  6.346983259e-03f, -6.115784718e-03f,
 1.159047745e-02f,  1.923792422e-03f,  4.970981927e-03f,  1.761339054e-03f,  7.593780248e-03f,
 2.842567972e-03f,  2.222544091e-04f,  3.482302920e-03f,  2.219222618e-03f,  1.133205164e-04f,
 9.489042104e-03f,  4.403978036e-03f, -2.680440425e-03f,  1.799635192e-02f, -7.843130023e-03f,
 2.762654301e-03f,  1.507143096e-02f, -7.719551285e-03f,  1.910793371e-03f,  1.814965226e-02f,
-1.099608780e-02f,  8.119524790e-03f,  7.618184277e-03f, -3.464927407e-03f,  5.667733884e-03f,
 7.289220609e-03f, -5.586884489e-03f,  1.348337533e-02f, -3.802282591e-03f, -4.868802809e-03f,
 1.312825371e-02f,  4.052408971e-04f, -1.146365975e-03f,  9.408628493e-03f,  1.037166142e-03f,
-6.922702263e-03f,  7.984051281e-03f, -1.248125062e-03f,  5.149608642e-04f,  7.795772306e-03f,
 2.816351943e-04f,  2.845926857e-03f,  1.504910272e-03f,  1.327518415e-03f, -1.523880313e-04f,
 3.383468843e-03f, -1.005718518e-03f,  3.345546555e-04f,  3.860638364e-03f,  2.514202053e-03f,
-4.634151581e-03f,  5.427796009e-03f,  2.272234049e-04f, -1.976874325e-03f,  1.915148698e-03f,
 4.868705588e-03f, -4.696998162e-03f,  1.565883808e-03f,  2.890411826e-03f,  7.482699619e-04f,
 1.273118081e-03f,  5.649183182e-03f, -4.223805618e-03f,  1.640385698e-03f, -3.134747941e-03f,
 1.417944659e-03f,  2.644378971e-03f,  3.942004640e-04f,
};

  const float tt_calib_filter[128] = {
 5.264907041e-03f,  5.405581193e-02f,  2.960867215e-01f,  1.231566005e-01f, -5.886309438e-02f, 
-1.073677716e-01f, -7.326609173e-03f,  9.356123071e-02f,  9.039441555e-02f, -3.625940130e-02f, 
-8.747018066e-02f,  1.078155290e-02f, -9.818255775e-03f, -2.027068719e-02f,  2.909006394e-02f, 
 1.084935608e-02f, -3.379080225e-02f, -2.034241131e-02f,  3.617140487e-02f, -2.868178874e-03f, 
-9.697754924e-03f,  8.996119266e-03f,  4.369094139e-03f, -1.787822711e-02f,  3.961938162e-03f, 
 1.041807832e-02f, -6.806393468e-03f, -1.051036952e-02f,  4.650316857e-03f, -2.229146193e-03f, 
-7.324748716e-03f,  1.799763060e-03f,  2.004041394e-03f, -6.548666746e-03f, -1.172731555e-03f, 
 2.671778579e-03f, -8.353991507e-04f, -3.732380224e-03f,  1.676484350e-03f,  8.138684644e-04f, 
-2.415811225e-03f, -2.580231776e-04f,  3.831236622e-03f, -2.888465862e-03f,  5.606007414e-05f, 
 1.005042960e-03f,  2.651998269e-04f,  8.137239888e-05f,  3.989861155e-04f,  1.483649417e-03f, 
 2.665748591e-04f,  3.528147921e-04f,  1.506750595e-03f, -5.696667820e-06f,  2.347253889e-04f, 
 3.003349409e-04f,  1.494441311e-03f, -5.650967338e-04f,  5.161902121e-04f,  9.771846909e-04f, 
-4.611814409e-05f,  9.315715043e-04f,  7.063157593e-04f,  1.199085028e-03f, -6.075040750e-05f, 
 1.444777830e-03f,  9.837773479e-04f, -1.635278116e-04f,  4.020045110e-04f,  2.376727623e-03f, 
-5.187474952e-04f,  3.598402230e-04f,  1.144302830e-03f,  1.398545798e-03f, -1.323453503e-03f, 
 2.255561584e-03f,  1.065290526e-04f, -1.069769504e-04f,  5.078339239e-04f,  1.548547900e-03f, 
-5.896576204e-04f,  1.752413625e-03f, -6.465033174e-04f,  2.755726492e-03f, -1.085273691e-03f, 
 2.128265322e-03f,  8.457332228e-04f,  1.022531770e-03f,  1.166506172e-03f,  2.288244944e-03f, 
 2.991596048e-04f,  1.927451361e-03f, -4.204656766e-05f,  3.215331622e-03f, -1.611955408e-03f, 
 5.354421628e-03f, -2.187433302e-03f,  4.371884608e-03f, -1.092113320e-03f,  4.667069134e-03f, 
-2.888380838e-03f,  6.596303032e-03f, -3.717569688e-03f,  5.467706597e-03f, -2.970076174e-03f, 
 6.320069307e-03f, -3.447539073e-03f,  5.953479619e-03f, -3.556563619e-03f,  5.665726116e-03f, 
-5.138195045e-03f,  7.914915237e-03f, -7.135613279e-03f,  1.011567070e-02f, -8.525915602e-03f, 
 1.071933969e-02f, -7.300576148e-03f,  9.145539332e-03f, -5.232127703e-03f,  6.612405352e-03f, 
-3.550333778e-03f,  7.658410879e-03f, -7.359003755e-03f,  1.208892635e-02f, -1.432434108e-02f, 
 1.441610296e-02f, -8.876295654e-03f,  2.137929849e-02f, 
};
#endif

const SpeechConfig WEAK speech_cfg_default = {
#if defined(SPEECH_TX_DC_FILTER)
    .tx_dc_filter = {
        .bypass                 = 0,
        .gain                   = 0,
    },
#endif

/****************************************************************************************************
 * Describtion:
 *     Acoustic Echo Cancellation
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     hpf_enabled(0/1): high pass filter enable or disable. Used to remove DC for Near and Ref signals.
 *     af_enabled(0/1): adaptive filter enable or disable. If the echo signal is very large, enable this
 *     adprop_enabled(0/1): update the filter coefficient according to the blocks enable or disable. If the number of blocks is bigger than 1, enable this.
 *     varistep_enabled(0/1): variable step size enable or disable. When dealing with the double-talk situation, enable this.
 *     nlp_enabled(0/1): non-linear process enable or disable. Enable this by default.
 *     clip_enabled(0/1): residual echo suppression enable or disable.
 *     stsupp_enabled(0/1): single-talk suppression enable or disable.
 *     hfsupp_enabled(0/1): high_frequency echo suppression enable or disable.
 *     constrain_enabled(0/1): constrain background noise enable or disable.
 *     ns_enabled(0/1): noise supression enable or disable. Enable this by default.
 *     cng_enabled(0/1):  comfort noise generator enable or disable.
 *     blocks(1~8): the length of adaptive filter = blocks * frame length
 *     delay(>0): delay samples from mic to speak, unit(sample).
 *     gamma(0~1): forgetting factor for psd estimation.
 *     echo_band_start(0~8000): start band for echo detection, unit(Hz)
 *     echo_band_end(echo_band_start~8000): end band for echo detection, unit(Hz)
 *     min_ovrd(1~6): supression factor, min_ovrd becomes larger and echo suppression becomes larger.
 *     target_supp(<0): target echo suppression, unit(dB)
 *     highfre_band_start(0~8000): start band for high_frequency echo suppression, unit(Hz)
 *     highfre_supp(>=0):high_frequency echo suppression, unit(dB) non-negative number!!!
 *     noise_supp(-30~0): noise suppression, unit(dB)
 *     cng_type(0/1): noise type(0: White noise; 1: Pink noise)
 *     cng_level(<0): noise gain, unit(dB)
 *     clip_threshold: compression threshold
 *     banks:Parameters for NS, bigger then better but cost more.
 * Resource consumption:
 *     RAM:     42K
 *     FLASH:   None
 *     MIPS:    30M(one block);62M(six blocks)    fs = 16kHz;
 * Note:
 *     This is the recommended AEC
****************************************************************************************************/
#if defined(SPEECH_TX_AEC2FLOAT)
    .tx_aec2float = {
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
        .ns_enabled     = false,
        .cng_enabled    = false,
        .blocks         = 1,
#if defined(SPEECH_TX_AEC_CODEC_REF)
        .delay          = 70,
#else
        .delay          = 139,
#endif
        .error_threshold = 1.5e-5f,
        .gamma          = 0.9,
        .echo_band_start = 300,
        .echo_band_end  = 1800,
        .high_echo_band_start = 0,
        .high_echo_band_end = 0,
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

#if defined(SPEECH_TX_1MIC_PREAF)
/****************************************************************************************************
 * Describtion:
 *     1 MIC Noise Suppression
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     talk_error_threshold(float): error_threshold for talk mic. (level:1e-5 ~ 1e-9)                      (for echo_af_enable)
 *     ref_delay(-240-240): delay for ref signal (no use so far, TBD)
 * Note:
 *     None
****************************************************************************************************/
    .tx_1mic_preaf = {
        .bypass             = 0,
        .ref_delay          = 0,
        .error_threshold    = 1.5e-7,
        .filter_len         = 32,
        .ref_pre_scale      =1.0,
    },
#endif

#if defined(SPEECH_TX_1MIC_NS)
/****************************************************************************************************
 * Describtion:
 *     2 MICs Noise Suppression
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz;
 * Note:
 *     None
****************************************************************************************************/
    .tx_1mic_ns = {
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
        .high_echo_band_start = 0,
        .high_echo_band_end = 0,
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

#if defined(SPEECH_TX_NS9)
/****************************************************************************************************
 * Describtion:
 *     Noise Suppression
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 * Resource consumption:

 * Note:
****************************************************************************************************/
    .tx_ns9 = {
        .bypass     = 0,
        .denoise_dB = -5,
        .echo_supp_enable = 0,
        .ref_delay = 148,
        .gamma = 0.9,
        .echo_band_start = 300,
        .echo_band_end = 7000,
        .min_ovrd = 2,
        .target_supp = -50,
        .ga_thr = 0.7,
        .en_thr = 100,
    },
#endif

#if defined(SPEECH_TX_2MIC_NS4_PREAF)
/****************************************************************************************************
 * Describtion:
 *     3 MICs pre_process
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     talk_error_threshold(float): error_threshold for talk mic. (level:1e-5 ~ 1e-9)                      (for echo_af_enable)
 *     ff_error_threshold(float): error_threshold for ff mic.  (level:1e-5 ~ 1e-9)                         (for echo_af_enable)
 *     ref_delay(-240-240): delay for ref signal (no use so far, TBD)
 * Note:
 *     None
****************************************************************************************************/
    .tx_2mic_ns4_preaf = {
        {
        .bypass                   = 0,
        .ref_delay                = 0,
        .outer_echo_af_enable     = 0,
        .ff_error_threshold    = 1.5e-8,
        .fb_error_threshold    = 1.5e-6,
        .ff_filter_len         = 32,
        .fb_filter_len         = 64,
        .ff_fb_calib_enable       = 1,
        .fb_filter       = normal_calib_filter,
        .ref_pre_scale           =1.0,
        },//normal
            {
            .bypass                   = 0,
            .ref_delay                = 0,
            .outer_echo_af_enable     = 0,
            .ff_error_threshold    = 1.5e-8,
            .fb_error_threshold    = 1.5e-6,
            .ff_filter_len         = 32,
            .fb_filter_len         = 64,
            .ff_fb_calib_enable       = 1,
            .fb_filter                = NULL,
            .ref_pre_scale           =1.0,
            },//ff
                {
                .bypass                   = 0,
                .ref_delay                = 0,
                .outer_echo_af_enable     = 0,
                .ff_error_threshold    = 1.5e-8,
                .fb_error_threshold    = 1.5e-6,
                .ff_filter_len         = 32,
                .fb_filter_len         = 64,
                .ff_fb_calib_enable       = 1,
                .fb_filter        = tt_calib_filter,
                .ref_pre_scale           =1.0,
                },//tt
    },
#endif

#if defined(SPEECH_TX_2MIC_NS4)
/****************************************************************************************************
 * Describtion:
 *     2 MICs Noise Suppression
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz;
 * Note:
 *     None
****************************************************************************************************/
    .tx_2mic_ns4 = {
        {
        .bypass =          0,
        .wnr_enable =      1,
        .mode_flag =       0,           //normal

        .outer_echo_af_enable =     0,
        .echo_supp_enable = 1,
        .ref_delay =        0,
        .outer_echo_band_start =    300,
        .outer_echo_band_end =      5000,
        .fb_echo_band_start =       100,
        .fb_echo_band_end =         2000,

        .post_supp_enable=  1,
        .pf_denoise_db =    -10,         //ns7_pf for outer
        .inner_denoise_db = -5,       //ns3 for inner

        .fb_energy_diff_min = 0.02,
        .fb_energy_diff_max = 0.04,

        .blend_en =         1,
        .wind_blend_band =         {2000, 2800},  

        .comp_num =         5,
        .comp_freq =        {400, 750, 1500, 2800},
        .comp_gaindB =      {-5, 0, 10, 18},   //for wind

        .wdrc_enable =      1,
        .pre_gain =         24,//24
        .post_gain =        5,//5
        },//normal
            {
            .bypass =          0,
            .wnr_enable =      1,
            .mode_flag =       1,           //normal

            .outer_echo_af_enable =     0,
            .echo_supp_enable = 1,
            .ref_delay =        0,
            .outer_echo_band_start =    300,
            .outer_echo_band_end =      5000,
            .fb_echo_band_start =       100,
            .fb_echo_band_end =         2000,

            .post_supp_enable=  1,
            .pf_denoise_db =    -15,         //ns7_pf for outer
            .inner_denoise_db = -10,       //ns3 for inner

            .fb_energy_diff_min = 0.04,
            .fb_energy_diff_max = 0.08,

            .blend_en =         1,
            .wind_blend_band =         {2000, 4000}, 

            .comp_num =         4,
            .comp_freq =        {500, 1000, 1500, 2000},
            .comp_gaindB =      {-5, 0, 0, 6},   //for wind

            .wdrc_enable =      1,
            .pre_gain =         10,
            .post_gain =        8,
            },//anc
                {
                .bypass =          0,
                .wnr_enable =      1,
                .mode_flag =       2,           //normal

                .outer_echo_af_enable =     0,
                .echo_supp_enable = 1,
                .ref_delay =        0,
                .outer_echo_band_start =    300,
                .outer_echo_band_end =      5000,
                .fb_echo_band_start =       100,
                .fb_echo_band_end =         2000,

                .post_supp_enable=  1,
                .pf_denoise_db =    -15,         //ns7_pf for outer
                .inner_denoise_db = -10,       //ns3 for inner

                .fb_energy_diff_min = 0.06,
                .fb_energy_diff_max = 0.01,

                .blend_en =         1,
                .wind_blend_band =         {2000, 4000},    

                .comp_num =         4,
                .comp_freq =        {500, 1000, 1500, 2000},
                .comp_gaindB =      { -5, 0, 6, 12},   //for wind

                .wdrc_enable =      1,
                .pre_gain =         10,
                .post_gain =        8,
                },//ff
    },
#endif

#if defined(SPEECH_TX_2MIC_PREAF)
/****************************************************************************************************
 * Describtion:
 *     2 MICs Noise Suppression
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     talk_error_threshold(float): error_threshold for talk mic. (level:1e-5 ~ 1e-9)                      (for echo_af_enable)
 *     ff_error_threshold(float): error_threshold for ff mic.  (level:1e-5 ~ 1e-9)                         (for echo_af_enable)
 *     ref_delay(-240-240): delay for ref signal (no use so far, TBD)
 * Note:
 *     None
****************************************************************************************************/
    .tx_2mic_preaf = {
        .bypass                  = 0,
        .ref_delay               = 0,
        .talk_error_threshold    = 1.5e-7,
        .ff_error_threshold      = 1.5e-7,
        .talk_filter_len         = 32,
        .ff_filter_len           = 32,
        .ref_pre_scale           =1,
    },
#endif

#if defined(SPEECH_TX_2MIC_NS8)
/****************************************************************************************************
 * Describtion:
 *     2 MICs Noise Suppression
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     dist: The distance between two mics.
 *     min_frequency(0-fs/2): denoise tunning para.
 *     max_frequency(0-fs/2): denoise tunning para.
 *     energy_threshold(0.1-32768): denoise tunning para.
 *     corr_threshold(0.1-0.999): denoise tunning para.
 *     fb_ratio(0.1-0.999): denoise tunning para.
 *     wind_supp_enable(0/1): bypass wind enhancment enable or disable.
 *     echo_af_enable(0/1): adaptive filter enable or disable. If the echo signal is very large, enable this.
 *     echo_supp_enable(0/1): enable echo suppression function
 *     ref_delay(-240-240): delay for ref signal (no use so far, TBD)
 **    gamma(0~1): forgetting factor for psd estimation.
 *     echo_band_start(0~8000): start band for echo detection, unit(Hz)
 *     echo_band_end(echo_band_start~8000): end band for echo detection, unit(Hz)
 *     min_ovrd(1~6): supression factor, min_ovrd becomes larger and echo suppression becomes larger.
 *     post_supp_enable(0/1): enable post denoise function
 *     wdrc_enable(0/1): enable WDRC function.
 *     pre_gain(float): normalized gain(before processing).
 *     post_gain(float): normalized gain(after processing).
 *     denoise_dB(0--20): max denoise dB for post denoise
 *     dnn_denoise_dB(0--20): max denoise dB for post denoise
 * Resource consumption:
 *     base ram: 40k, base mips: 31M, base flash:98k
 *     IF set echo_supp_enable = 1. Need more RAM += 8K & MIPS += 7M
 *     IF set echo_af_enable = 1. Need more RAM += 61K & MIPS += 63M
 *     IF set post_supp_enable = 1. Need more RAM += 60K & MIPS += 52M
 * Note:
 *     None
****************************************************************************************************/
    .tx_2mic_ns8 = {
        .bypass             = 0,
        .dist               = 0.021f,
        .min_frequency      = 160.f,
        .max_frequency      = 8000.f,
        .energy_threshold   = 1.2f,
        .corr_threshold     = 0.9f,
        .fb_ratio           = 1.0f,
        .crossover_threshold= 1.0f,
        .Pb_energy_db       = -55,
        .update_snr         = -15,
        .update_count       = 32,
        .wind_mic_switch_thr = 1.5,

        .wind_supp_enable   = 1,
        .echo_af_enable     = 1,
        .echo_supp_enable   = 1,
        .ref_delay          = 0,
        .gamma              = 0.8f,
        .echo_band_start    = 300,
        .echo_band_end      = 4000,
        .min_ovrd           = 2,

        .reset_ec_thd       = 1.0,

        .wdrc_enable        = 1,
        .wdrc_CT            = -10.f,
        .wdrc_CS            = 2.f,        //same as speech_tx_compexp comp_ratio
        .wdrc_ET            = -100.f,
        .wdrc_ES            = 0.5556f,    //same as speech_tx_compexp expand_ratio

        .talk_pre_gain      = -2,    //Calibrate 2-mic frequency response(reduce the talk mic), usually gain <=0
        .pre_gain           = 0,     //Amplify all microphones with pre gain
        .post_gain          = 18,
        .bf_ns_nowind_gain  = 20,
        .bf_ns_wind_gain    = 10,

        .post_supp_enable   = 1,
        .denoise_dB         = -5,
        .dnn_denoise_dB     = -35,
        .target_supp        = -20,
        .vad_min_frequency  = 160,
        .vad_max_frequency  = 8000,
        .corr_threshold1    = 0.6,
        .wind_gamma         = 0.5,
        .wind_band_num      = 2,
        .wind_band_freq     = {250, 1000},
        .high_echo_band_start =    0,
        .high_echo_band_end =      0,
        .ff_to_talk_spec_eq_enable     = 1,
        .eq = {
            .bypass     = 0,
            .gain       = 0.f,
            .num        = 1,
            .params = {
                {IIR_BIQUARD_HPF, {{100, 0, 0.707f}}},
            },
        },
    },
#endif

#if defined(SPEECH_TX_2MIC_NS9)
/****************************************************************************************************
 * Describtion:
 *     2 MICs Noise Suppression
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     wdrc_enable(0/1): enable WDRC function.
 *     pre_gain(float): normalized gain(before processing).
 *     post_gain(float): normalized gain(after processing).
 *     post_supp_enable(0/1): enable post denoise function.
 *     pf_denoise_db: denoise dB for PF process.
 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz;
 * Note: None
****************************************************************************************************/
    .tx_2mic_ns9 = {
        .bypass             = 0,
        .echo_supp_enable   = 1,
        .ref_delay          = 0,
        .gamma              = 0.8f,
        .echo_band_start    = 300,
        .echo_band_end      = 4000,
        .high_echo_band_start =    0,
        .high_echo_band_end =      0,
        .min_ovrd           = 2,
        .reset_ec_thd       = 1.0,

        .wdrc_enable        = 0,
        .wdrc_CT            = -10.f,
        .wdrc_CS            = 2.f,        //same as speech_tx_compexp comp_ratio
        .wdrc_ET            = -100.f,
        .wdrc_ES            = 0.5556f,    //same as speech_tx_compexp expand_ratio

        .talk_pre_gain      = -2,    //Calibrate 2-mic frequency response(reduce the talk mic), usually gain <=0
        .pre_gain           = 0,     //Amplify all microphones with pre gain
        .post_gain          = 14,

        .post_supp_enable   = 1,
        .pf_denoise_db      = -5,
    },
#endif

#if defined(SPEECH_TX_3MIC_PREAF)
/****************************************************************************************************
 * Describtion:
 *     3 MICs pre_process
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     talk_error_threshold(float): error_threshold for talk mic. (level:1e-5 ~ 1e-9)                      (for echo_af_enable)
 *     ff_error_threshold(float): error_threshold for ff mic.  (level:1e-5 ~ 1e-9)                         (for echo_af_enable)
 *     ref_delay(-240-240): delay for ref signal (no use so far, TBD)
 * Note:
 *     None
****************************************************************************************************/
    .tx_3mic_preaf = {
        {
        .bypass                   = 0,
        .ref_delay                = 0,
        .outer_echo_af_enable     = 0,
        .talk_af_error_threshold  = 1.5e-8,
        .ff_af_error_threshold    = 1.5e-8,
        .fb_af_error_threshold    = 1.5e-6,
        .outer_af_filter_len      = 32,
        .fb_af_filter_len         = 64,
        .ff_fb_calib_enable       = 1,
        .fb_filter                = normal_calib_filter,
        .ref_pre_scale            = 6.0,
        },//normal
            {
            .bypass                   = 0,
            .ref_delay                = 0,
            .outer_echo_af_enable     = 0,
            .talk_af_error_threshold  = 1.5e-8,
            .ff_af_error_threshold    = 1.5e-8,
            .fb_af_error_threshold    = 1.5e-6,
            .outer_af_filter_len      = 32,
            .fb_af_filter_len         = 64,
            .ff_fb_calib_enable       = 1,
            .fb_filter                = NULL,
            .ref_pre_scale            = 6.0,
            },//ff
                {
                .bypass                   = 0,
                .ref_delay                = 0,
                .outer_echo_af_enable     = 0,
                .talk_af_error_threshold  = 1.5e-8,
                .ff_af_error_threshold    = 1.5e-8,
                .fb_af_error_threshold    = 1.5e-6,
                .outer_af_filter_len      = 32,
                .fb_af_filter_len         = 64,
                .ff_fb_calib_enable       = 1,
                .fb_filter                = tt_calib_filter,
                .ref_pre_scale            = 6.0,
                },//tt
                    {
                    .bypass                   = 0,
                    .ref_delay                = 0,
                    .outer_echo_af_enable     = 1,
                    .talk_af_error_threshold  = 1.5e-8,
                    .ff_af_error_threshold    = 1.5e-8,
                    .fb_af_error_threshold    = 1.5e-6,
                    .outer_af_filter_len      = 32,
                    .fb_af_filter_len         = 64,
                    .ff_fb_calib_enable       = 0,
                    .fb_filter                = NULL,
                    .ref_pre_scale            = 6.0,
                    },//for ows vpu
    },
#endif

#if defined(SPEECH_TX_3MIC_NS)
    /****************************************************************************************************
     * Describtion:
     *     3 MICs Noise Suppression
     * Parameters:
     *     bypass(0/1): bypass enable or disable.
     *     freq_smooth_enable(1): Must enable
     *     wnr_enable(0/1): wind noise reduction enable or disable. This is also useful for improving
     *         noise suppression, but it also has some damage to speech signal.
     * Resource consumption:
     *     RAM:     None
     *     FLASE:   None
     *     MIPS:    fs = 16kHz;
     * Note:
     *     None
    ****************************************************************************************************/
    .tx_3mic_ns = {
        {
        .bypass = 0,
        .wnr_enable = 1,
        .wind_band_num = 3,
        .wind_band_freq = {500, 2000, 4000},
        .wind_mic_switch_thr = 1.5,

        .dist = 0.031F,
        .mode_flag = 0,           //normal
        .talk_pre_gain = -2,
        .ref_pre_scale = 6.0,
        .bf_bin_start = 150,
        .bf_bin_end = 3000,
        .vad_bin_start = 200,
        .vad_bin_end = 1300,
        .energy_ratio = 0.4F,
        .coef1_thd = 0.8F,
        .energy1_thd = 1.4F,
        .coef2_thd = 0.85F,
        .energy2_thd = 1.6F,
        .vad_speech_prob_thd = 0.10F,
        .Pb_energy_db = -60,
        .update_snr = -15,
        .update_fb_snr = -25,

        .outer_echo_af_enable = 0,
        .echo_supp_enable = 1,
        .ref_delay = 0,
        .outer_echo_band_start =    150,
        .outer_echo_band_end =      3000,
        .high_echo_band_start =    6000,
        .high_echo_band_end =      8000,
        .fb_echo_band_start = 100,
        .fb_echo_band_end = 2800,

        .post_supp_enable = 1,
        .nn_gain_enable   = 1,
        .pf_denoise_db = -10,         //ns9_pf for outer
        .complete_3mic_enable = 1,          //use fb
        .inner_denoise_db = -5,       //ns9 for inner

        .interference_voice_enable = 0,
        .Pb_noise_energy_db = -40,//when noise, do not suppress interference_voice
        .fb_energy_diff_min = 0.06,
        .fb_energy_diff_max = 0.08,

        .blend_en = 1,
        .blend_mix_start = 0,
        .blend_mix_end = 2800,
        .noise_blend_band = {1500, 2800},
        .blend_noise_power_thd = {0.4, 0.2},
        .blend_snr_thd = {15.0, 0.0},

        .comp_num = 4,
        .comp_freq = {500, 1700, 2200, 2800},
        .comp_gaindB = {-12, -6, 0, 5},   //for wind

        .wdrc_enable = 1,
        .bf_ns_nowind_gain  = 6,
        .pre_gain = 10,//10
        .post_gain = 12,//8
        .comp_speech_prob_thd = 0.95,
        .ff_to_talk_spec_eq_enable     = 1,
        .eq = {
            .bypass     = 0,
            .gain       = 0.f,
            .num        = 3,
            .params = {
                {IIR_BIQUARD_HPF, {{100, 0, 0.707f}}},
                { IIR_BIQUARD_HIGHSHELF,{ { 2800, -5, 1.f } } },
                { IIR_BIQUARD_PEAKINGEQ,{ { 3200, -11, 1.f } } },
            },
        },
        },//normal
            {
            .bypass = 0,
            .wnr_enable = 1,
            .wind_band_num = 3,
            .wind_band_freq = {500, 2000, 4000},
            .wind_mic_switch_thr = 1.5,

            .dist = 0.031F,
            .mode_flag = 1,           //normal
            .talk_pre_gain = -2,
            .ref_pre_scale = 6.0,
            .bf_bin_start = 150,
            .bf_bin_end = 3000,
            .vad_bin_start = 200,
            .vad_bin_end = 1300,
            .energy_ratio = 0.4F,
            .coef1_thd = 0.8F,
            .energy1_thd = 1.4F,
            .coef2_thd = 0.85F,
            .energy2_thd = 1.6F,
            .vad_speech_prob_thd = 0.08F,
            .Pb_energy_db = -60,
            .update_snr = -15,
            .update_fb_snr = -25,

            .outer_echo_af_enable = 0,
            .echo_supp_enable = 1,
            .ref_delay = 0,
            .outer_echo_band_start =    150,
            .outer_echo_band_end =      3000,
            .high_echo_band_start =    6000,
            .high_echo_band_end =      8000,
            .fb_echo_band_start = 100,
            .fb_echo_band_end = 2800,

            .post_supp_enable = 1,
            .nn_gain_enable   = 1,
            .pf_denoise_db = -10,         //ns7_pf for outer
            .complete_3mic_enable = 1,          //use fb
            .inner_denoise_db = -5,       //ns3 for inner

            .interference_voice_enable = 0,
            .Pb_noise_energy_db = -40,//when noise, do not suppress interference_voice
            .fb_energy_diff_min = 0.04,
            .fb_energy_diff_max = 0.06,

            .blend_en = 1,
            .blend_mix_start = 0,
            .blend_mix_end = 2800,
            .noise_blend_band = {1500, 2800},
            .blend_noise_power_thd = {0.4, 0.2},
            .blend_snr_thd = {15.0, 0.0},

            .comp_num = 3,
            .comp_freq = {1800, 2200, 2800},
            .comp_gaindB = {0, 5, 8},   //for wind

            .wdrc_enable = 1,
            .bf_ns_nowind_gain  = 6,
            .pre_gain = 10,
            .post_gain = 12,
            .comp_speech_prob_thd = 0.95,
            .ff_to_talk_spec_eq_enable     = 1,
            .eq = {
                .bypass     = 0,
                .gain       = 0.f,
                .num = 3,
                .params = {
                    { IIR_BIQUARD_HPF,{ { 100, 0, 0.707f } } },
                    { IIR_BIQUARD_HIGHSHELF,{ { 2800, -5, 1.f } } },
                    { IIR_BIQUARD_PEAKINGEQ,{ { 3200, -11, 1.f } } },
                },
            },
            },//anc
                {
                .bypass = 0,
                .wnr_enable = 1,
                .wind_band_num = 3,
                .wind_band_freq = {500, 2000, 4000},
                .wind_mic_switch_thr = 1.5,

                .dist = 0.031F,
                .mode_flag = 2,           //normal
                .talk_pre_gain = -2,
                .ref_pre_scale = 6.0,
                .bf_bin_start = 150,
                .bf_bin_end = 3000,
                .vad_bin_start = 200,
                .vad_bin_end = 1300,
                .energy_ratio = 0.4F,
                .coef1_thd = 0.8F,
                .energy1_thd = 1.4F,
                .coef2_thd = 0.85F,
                .energy2_thd = 1.6F,
                .vad_speech_prob_thd = 0.12F,
                .Pb_energy_db = -60,
                .update_snr = -15,
                .update_fb_snr = -25,

                .outer_echo_af_enable = 0,
                .echo_supp_enable = 1,
                .ref_delay = 0,
                .outer_echo_band_start =    150,
                .outer_echo_band_end =      3000,
                .high_echo_band_start =    6000,
                .high_echo_band_end =      8000,
                .fb_echo_band_start = 100,
                .fb_echo_band_end = 2800,

                .post_supp_enable = 1,
                .nn_gain_enable   = 1,
                .pf_denoise_db = -10,         //ns7_pf for outer
                .complete_3mic_enable = 1,          //use fb
                .inner_denoise_db = -5,       //ns3 for inner

                .interference_voice_enable = 0,
                .Pb_noise_energy_db = -40,//when noise, do not suppress interference_voice
                .fb_energy_diff_min = 0.08,
                .fb_energy_diff_max = 0.1,

                .blend_en = 1,
                .blend_mix_start = 0,
                .blend_mix_end = 2800,
                .noise_blend_band = {1500, 2800},
                .blend_noise_power_thd = {0.4, 0.2},
                .blend_snr_thd = {15.0, 0.0},

                .comp_num = 5,
                .comp_freq = {250, 500, 1300, 1800, 2800},
                .comp_gaindB = {-5, 0, -5, 0, 6},   //for wind

                .wdrc_enable = 1,
                .bf_ns_nowind_gain  = 6,
                .pre_gain = 10,
                .post_gain = 12,
                .comp_speech_prob_thd = 0.95,
                .ff_to_talk_spec_eq_enable     = 1,
                .eq = {
                    .bypass     = 0,
                    .gain       = 0.f,
                    .num = 3,
                    .params = {
                        { IIR_BIQUARD_HPF,{ { 100, 0, 0.707f } } },
                        { IIR_BIQUARD_HIGHSHELF,{ { 2800, -5, 1.f } } },
                        { IIR_BIQUARD_PEAKINGEQ,{ { 3200, -11, 1.f } } },
                    },
                },
                },//tt
    },
#endif

#if defined(SPEECH_TX_3MIC_NS2)
/****************************************************************************************************
 * Describtion:
 *     3 MICs Noise Suppression2
 * Parameters:

 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz;
 * Note:
 *     None
****************************************************************************************************/
    .tx_3mic_ns2 = {
        .bypass      = 0,
        .wnr_enable  = 1,
        .wind_band_num      = 2,
        .wind_band_freq     = {250, 1000},
        .wind_mic_switch_thr = 1.5,

        .vpu_bin_start =   150,
        .vpu_bin_end =     1000,
        .vad_speech_prob_thd =      0.06F,
        .update_snr         = -20,

        .outer_echo_af_enable =     1,
        .echo_supp_enable = 1,
        .ref_delay =        0,
        .outer_echo_band_start =    1000,
        .outer_echo_band_end =      6000,
        .vpu_echo_band_start =       100,
        .vpu_echo_band_end =         1000,

        .post_supp_enable =  1,
        .pf_denoise_db =    -15,         //ns7_pf for outer

        .inner_denoise_db     = -40,       //ns3 for inner
        .vpu_energy_diff_min  = 0.02,
        .vpu_energy_diff_max  = 0.04,

        .blend_en =         1,
        .blend_mix_start =  650,
        .blend_mix_end =    650,
        .noise_blend_band =     1000,
        .blend_noise_power_thd =    0.5,
        .blend_snr_thd =    15,

        .comp_num =         4,
        .comp_freq =        {150, 450, 550,650},
        .comp_gaindB =      {-3, -8, 0, 8},   //for wind

        .wdrc_enable =      1,
        .pre_gain =         10,//6
        .ref_pre_scale =     1,
        .post_gain =        12,//10
        .comp_speech_prob_thd =     0.95,
        },
#endif

/****************************************************************************************************
 * Describtion:
 *     Compressor and expander
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     ...
 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz;
 * Note:
 *     None
****************************************************************************************************/
#if defined(SPEECH_TX_COMPEXP)
    .tx_compexp = {
        .bypass             = 0,
        .type               = 0,
        .comp_threshold     = -20.f,
        .comp_ratio         = 2.f,
        .expand_threshold   = -65.f,
        .expand_ratio       = 0.555f,
        .attack_time        = 0.008f,
        .release_time       = 0.06f,
        .makeup_gain        = 8,
        .delay              = 128,
        .tav                = 0.1f,
    },
#endif

/****************************************************************************************************
 * Describtion:
 *     Equalizer, implementation with 2 order iir filters
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     gain(float): normalized gain. It is usually less than or equal to 0
 *     num(0~6): the number of EQs
 *     params: {type, frequency, gain, q}. It supports a lot of types, please refer to iirfilt.h file
 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz, 0.5M/section;
 * Note:
 *     None
****************************************************************************************************/
#if defined(SPEECH_TX_EQ)
    .tx_eq = {
        .bypass     = 0,
        .gain       = 0.f,
        .num        = 1,
        .params = {
            {IIR_BIQUARD_HPF, {{100, 0, 0.707f}}},
        },
    },
#endif

#if defined(SPEECH_RX_NS2FLOAT)
/****************************************************************************************************
 * Describtion:
 *     Acoustic Echo Cancellation
 * Parameters:
 *     Refer to SPEECH_TX_NS2FLOAT
 * Note:
 *     None
****************************************************************************************************/
    .rx_ns2float = {
        .bypass     = 0,
        .denoise_dB = -15,
        .banks      = 64,
        .wdrc_enable = 1,
        .expander_threshold = -70,
        .expander_ratio = 0.7,
    },
#endif


#if defined(SPEECH_RX_COMPEXP)
/****************************************************************************************************
 * Describtion:
 *     Compressor and expander
 * Parameters:
 *     Refer to SPEECH_TX_COMPEXP
 * Note:
 *     None
****************************************************************************************************/
    .rx_compexp = {
        .bypass = 0,
        .num = 2,
        .xover_freq = {5000},
        .order = 4,
        .params = {
            {
                .bypass             = 0,
                .type               = 0,
                .comp_threshold     = -10.f,
                .comp_ratio         = 2.f,
                .expand_threshold   = -60.f,
                .expand_ratio       = 0.5556f,
                .attack_time        = 0.001f,
                .release_time       = 0.006f,
                .makeup_gain        = 0,
                .delay              = 128,
                .tav                = 0.2f,
            },
            {
                .bypass             = 0,
                .type               = 0,
                .comp_threshold     = -10.f,
                .comp_ratio         = 2.f,
                .expand_threshold   = -60.f,
                .expand_ratio       = 0.5556f,
                .attack_time        = 0.001f,
                .release_time       = 0.006f,
                .makeup_gain        = 0,
                .delay              = 128,
                .tav                = 0.2f,
            }
        }
    },
#endif


#if defined(SPEECH_RX_EQ)
/****************************************************************************************************
 * Describtion:
 *     Equalizer, implementation with 2 order iir filters
 * Parameters:
 *     Refer to SPEECH_TX_EQ
 * Note:
 *     None
****************************************************************************************************/
    .rx_eq = {
        .bypass = 0,
        .gain   = 0.f,
        .num    = 1,
        .params = {
            {IIR_BIQUARD_HPF, {{60, 0, 0.707f}}},
        },
    },
#endif


};
