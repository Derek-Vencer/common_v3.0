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
/*******************************************************************************
** namer ANC filter
** description IIR and FIR filter
** version 1.0
** author xuml
** modify 2021.9.24.
*******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "plat_addr_map.h"
#include CHIP_SPECIFIC_HDR(reg_codec)
#include "cmsis.h"
#include "hal_dma.h"
#include "hal_codec.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_cmu.h"
#include "hal_codec.h"
#include "analog.h"
#include "anc_process.h"
#include "hwtimer_list.h"
#include "hal_sysfreq.h"
#include "hal_chipid.h"
#include "system_utils.h"

/**********************************mocro**************************************/
/*
iir coefficients(b/a Q27)
b2
b1
b0
a2
a1
*/
struct _anc_iir_coefs
{
    int32_t a1 ;
    int32_t a2 ;

    int32_t b1 ;
    int32_t b2 ;
    int32_t b0 ;
};

static struct CODEC_T * const codec = (struct CODEC_T *)CODEC_BASE;

#define CODEC_EN_CLK_IIR_IIR0 (1<<0)
#define CODEC_EN_CLK_IIR_IIR1 (1<<1)
#define CODEC_EN_CLK_IIR_IIR4 (1<<2)
#define CODEC_EN_CLK_IIR_IIR5 (1<<3)


#if defined(ANC_FB_CHECK) && defined(ANC_FF_CHECK)
#error "ANC_FB_CHECK and ANC_FF_CHECK can not defined simultaneously "
#endif
#if defined(ANC_FB_CHECK) && defined(ANC_TT_CHECK)
#error "ANC_FB_CHECK and ANC_TT_CHECK can not defined simultaneously "
#endif

#if defined(ANC_TT_CHECK) && defined(ANC_FF_CHECK)
#error "ANC_TT_CHECK and ANC_FF_CHECK can not defined simultaneously "
#endif


#define IIR_COUNTER (13)

#if defined(AUDIO_ANC_TT_HW)
#define IIR_FF_COUNTER (13)
#define IIR_TT_COUNTER (13)
#else
#define IIR_FF_COUNTER (13)
#define IIR_TT_COUNTER (0)
#endif

#if defined(AUDIO_ANC_FB_MC_HW)
#define IIR_FB_COUNTER (13)
#define IIR_MC_COUNTER (13)
#else
#define IIR_FB_COUNTER (13)
#define IIR_MC_COUNTER (0)
#endif

#define ANC_AUD_OUTPUT_PATH_SPEAKER_DEV  (AUD_CHANNEL_MAP_CH0)

#define FIR_LEN (472)

#define MAX_FIR_LEN (512)

typedef struct _iir_parameter
{
    int32_t total_gain;

    uint16_t iir_bypass_flag;
    uint16_t iir_counter;

    anc_iir_coefs iir_coef[IIR_COUNTER];

} iir_parameter;


#define GAIN_Q (9)

// #define AUDIO_ANC_FIR_HW

#define CALIB_GAIN_Q (1<<12)
#define FIXED_COEF_Q (1<<27)
#define ANC_GAIN_RAMP_THRESHLD (40)
#define ANC_GAIN_RAMP_GAIN_PRE (ANC_GAIN_RAMP_THRESHLD + 1)
#define ANC_GAIN_RAMP_ZERO (10)
#define ANC_GAIN_RAMP_BURST_THRESHLD (ANC_GAIN_RAMP_THRESHLD*10000)
#define ANC_FAST_CALIB_MODE

#define FIXED_GAIN_RAMP_Q (1<<27)

/*
Type='Low Passing';
Freq=240;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
static const anc_iir_coefs   iir_coef_gain_ramp_delay1 = {
    .coef_b={512,         1025,          512},
    .coef_a={134217728,   -265823675,    131607997},
};

#define ANC_SET_GAIN_TIME_DELAY1  ((MS_TO_TICKS(70)))


/*
Type='Low Passing';
Freq=120;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
static const anc_iir_coefs   iir_coef_gain_ramp_delay2 = {
    .coef_b={129,          257,          129},
    .coef_a={134217728,   -267123699,    132906486},
};

#define ANC_SET_GAIN_TIME_DELAY2  ((MS_TO_TICKS(150)))


/*
Type='Low Passing';
Freq=60;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
static const anc_iir_coefs   iir_coef_gain_ramp_delay3 = {
    .coef_b={32,           65,           32},
    .coef_a={134217728,   -267778100,    133560501},
};

#define ANC_SET_GAIN_TIME_DELAY3  ((MS_TO_TICKS(300)))


/*
Type='Low Passing';
Freq=30;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs   iir_coef_gain_ramp_delay4 = {
    .coef_b={            8,           16,            8},
    .coef_a={134217728,   -268106408,    133888712},
};

#define ANC_SET_GAIN_TIME_DELAY4  ((MS_TO_TICKS(550)))
/*
Type='Low Passing';
Freq=1000;
Gain=0;
Q=0.004;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs   iir_coef_gain_ramp_delay5 = {
    .coef_b={            2950,         5900,         2950},
    .coef_a={134217728,    -88138077,    -46067851},
};

#define ANC_SET_GAIN_TIME_DELAY5  ((MS_TO_TICKS(900)))


/*
Type='Low Passing';
Freq=1000;
Gain=0;
Q=0.003;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs   iir_coef_gain_ramp_delay6 = {
    .coef_b={            2410,         4821,         2410},
    .coef_a={134217728,    -72015762,    -62192325},
};

#define ANC_SET_GAIN_TIME_DELAY6  ((MS_TO_TICKS(1200)))

/*
Type='Low Passing';
Freq=1000;
Gain=0;
Q=0.002;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs   iir_coef_gain_ramp_delay7 = {
    .coef_b={            1765,         3529,         1765},
    .coef_a={134217728,    -52726267,    -81484402},
};

#define ANC_SET_GAIN_TIME_DELAY7  ((MS_TO_TICKS(1800)))


/*
Type='Low Passing';
Freq=1000;
Gain=0;
Q=0.00175;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs   iir_coef_gain_ramp_delay8 = {
    .coef_b={            1583,         3166,         1583},
    .coef_a={134217728,    -47296900,    -86914496},
};

#define ANC_SET_GAIN_TIME_DELAY8  ((MS_TO_TICKS(2150)))

/*
Type='Low Passing';
Freq=1000;
Gain=0;
Q=0.00150;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs   iir_coef_gain_ramp_delay9 = {
    .coef_b={            1392,         2784,             1392},
    .coef_a={134217728,    -41587115,    -92625045},
};

#define ANC_SET_GAIN_TIME_DELAY9  ((MS_TO_TICKS(2500)))


/*
Type='Low Passing';
Freq=1000;
Gain=0;
Q=0.00125;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs   iir_coef_gain_ramp_delay10 = {
    .coef_b={            1191,         2381,         1191},
    .coef_a={134217728,    -35574613,    -98638352},
};

#define ANC_SET_GAIN_TIME_DELAY10  ((MS_TO_TICKS(2850)))

/*
Type='Low Passing';
Freq=1000;
Gain=0;
Q=0.001;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs   iir_coef_gain_ramp_delay11 = {
    .coef_b={            978,         1957,          978},
    .coef_a={134217728,    -29234663,   -104979151},
};

#define ANC_SET_GAIN_TIME_DELAY11  ((MS_TO_TICKS(3200)))




#define ANC_GAIN_RAMP_DELAY   (4)  // 1.5s(4)


#if ANC_GAIN_RAMP_DELAY==1
/*
Type='Low Passing';
Freq=240;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
#if !defined(ANC_IIR_LINEAR_SMOOTH_MODE)
static const anc_iir_coefs   iir_coef_gain_ramp_ff_normal = {
    .coef_b={512,         1025,          512},
    .coef_a={134217728,   -265823675,    131607997},
};

static const anc_iir_coefs   iir_coef_gain_ramp_fb_normal = {
    .coef_b={512,         1025,          512},
    .coef_a={134217728,   -265823675,    131607997},
};
#if defined(AUDIO_ANC_TT_HW)
static const anc_iir_coefs   iir_coef_gain_ramp_tt_normal = {
    .coef_b={512,         1025,          512},
    .coef_a={134217728,   -265823675,    131607997},
};
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
static const anc_iir_coefs   iir_coef_gain_ramp_mc_normal = {
    .coef_b={512,         1025,          512},
    .coef_a={134217728,   -265823675,    131607997},
};
#endif

#define ANC_SET_GAIN_TIME  ((MS_TO_TICKS(70)))
#endif

#elif ANC_GAIN_RAMP_DELAY==2
/*
Type='Low Passing';
Freq=120;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
#if !defined(ANC_IIR_LINEAR_SMOOTH_MODE)
static const anc_iir_coefs   iir_coef_gain_ramp_ff_normal = {
    .coef_b={129,          257,          129},
    .coef_a={134217728,   -267123699,    132906486},
};

static const anc_iir_coefs   iir_coef_gain_ramp_fb_normal = {
    .coef_b={129,          257,          129},
    .coef_a={134217728,   -267123699,    132906486},
};
#if defined(AUDIO_ANC_TT_HW)
static const anc_iir_coefs   iir_coef_gain_ramp_tt_normal = {
    .coef_b={129,          257,          129},
    .coef_a={134217728,   -267123699,    132906486},
};
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
static const anc_iir_coefs   iir_coef_gain_ramp_mc_normal = {
    .coef_b={129,          257,          129},
    .coef_a={134217728,   -267123699,    132906486},
};
#endif

#define ANC_SET_GAIN_TIME  ((MS_TO_TICKS(150)))
#endif

#elif ANC_GAIN_RAMP_DELAY==3
/*
Type='Low Passing';
Freq=60;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
#if !defined(ANC_IIR_LINEAR_SMOOTH_MODE)
static const anc_iir_coefs   iir_coef_gain_ramp_ff_normal = {
    .coef_b={32,           65,           32},
    .coef_a={134217728,   -267778100,    133560501},
};

static const anc_iir_coefs   iir_coef_gain_ramp_fb_normal = {
    .coef_b={32,           65,           32},
    .coef_a={134217728,   -267778100,    133560501},
};
#if defined(AUDIO_ANC_TT_HW)
static const anc_iir_coefs   iir_coef_gain_ramp_tt_normal = {
    .coef_b={32,           65,           32},
    .coef_a={134217728,   -267778100,    133560501},
};
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
static const anc_iir_coefs   iir_coef_gain_ramp_mc_normal = {
    .coef_b={32,           65,           32},
    .coef_a={134217728,   -267778100,    133560501},
};
#endif

#define ANC_SET_GAIN_TIME  ((MS_TO_TICKS(300)))
#endif

#elif ANC_GAIN_RAMP_DELAY==4
/*
Type='Low Passing';
Freq=30;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
#if defined(ANC_IIR_LINEAR_SMOOTH_MODE)
#define ANC_SET_GAIN_TIME  ((MS_TO_TICKS(550)))
#else
static const anc_iir_coefs   iir_coef_gain_ramp_ff_normal = {
    .coef_b={            8,           16,            8},
    .coef_a={134217728,   -268106408,    133888712},
};

static const anc_iir_coefs   iir_coef_gain_ramp_fb_normal = {
    .coef_b={            8,           16,            8},
    .coef_a={134217728,   -268106408,    133888712},
};
#if defined(AUDIO_ANC_TT_HW)
static const anc_iir_coefs   iir_coef_gain_ramp_tt_normal = {
    .coef_b={            8,           16,            8},
    .coef_a={134217728,   -268106408,    133888712},
};
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
static const anc_iir_coefs   iir_coef_gain_ramp_mc_normal = {
    .coef_b={            8,           16,            8},
    .coef_a={134217728,   -268106408,    133888712},
};
#endif

#define ANC_SET_GAIN_TIME  ((MS_TO_TICKS(550)))
#endif

#elif ANC_GAIN_RAMP_DELAY==5
/*
Type='Low Passing';
Freq=1000;
Gain=0;
Q=0.001;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
#if !defined(ANC_IIR_LINEAR_SMOOTH_MODE)
static const anc_iir_coefs   iir_coef_gain_ramp_ff_normal = {
    .coef_b={            978,         1957,          978},
    .coef_a={134217728,    -29234663,   -104979151},
};

static const anc_iir_coefs   iir_coef_gain_ramp_fb_normal = {
    .coef_b={            978,         1957,          978},
    .coef_a={134217728,    -29234663,   -104979151},
};
#if defined(AUDIO_ANC_TT_HW)
static const anc_iir_coefs   iir_coef_gain_ramp_tt_normal = {
    .coef_b={            978,         1957,          978},
    .coef_a={134217728,    -29234663,   -104979151},
};
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
static const anc_iir_coefs   iir_coef_gain_ramp_mc_normal = {
    .coef_b={            978,         1957,          978},
    .coef_a={134217728,    -29234663,   -104979151},
};
#endif

#define ANC_SET_GAIN_TIME  ((MS_TO_TICKS(3200)))
#endif
#endif

/*
Type='Low Passing';
Freq=240;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
#ifdef ANC_FAST_CALIB_MODE
#if !defined(ANC_IIR_LINEAR_SMOOTH_MODE)
static const anc_iir_coefs   iir_coef_gain_ramp_ff_calib = {
    .coef_b={512,         1025,          512},
    .coef_a={134217728,   -265823675,    131607997},
};

static const anc_iir_coefs   iir_coef_gain_ramp_fb_calib = {
    .coef_b={512,         1025,          512},
    .coef_a={134217728,   -265823675,    131607997},
};
#if defined(AUDIO_ANC_TT_HW)
static const anc_iir_coefs   iir_coef_gain_ramp_tt_calib = {
    .coef_b={512,         1025,          512},
    .coef_a={134217728,   -265823675,    131607997},
};
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
static const anc_iir_coefs   iir_coef_gain_ramp_mc_calib = {
    .coef_b={512,         1025,          512},
    .coef_a={134217728,   -265823675,    131607997},
};
#endif

#endif
#define ANC_SET_GAIN_CALIB_TIME  ((MS_TO_TICKS(70)))
#endif


#define ANC_LIMITER_FF
#if defined(AUDIO_ANC_TT_HW)
#define ANC_LIMITER_TT
#endif
#define ANC_LIMITER_FB
#if defined(AUDIO_ANC_FB_MC_HW)
#define ANC_LIMITER_MC
#endif
//#define PSAP_DEHOWLING_LIMITER
//#define SPKCALIB_LIMITER
//#define PSAP_EQ_LIMITER

#define ANC_LIMITER_FF_ATTACK_DELAY (1)
#define ANC_LIMITER_TT_ATTACK_DELAY (1)
#define ANC_LIMITER_FB_ATTACK_DELAY (1)
#define ANC_LIMITER_MC_ATTACK_DELAY (1)

#define ANC_LIMITER_DEHOWLING_ATTACK_DELAY (1)
#define ANC_LIMITER_SPKCALIB_ATTACK_DELAY (1)

//limiter threshold, 0dB: 0x7fffff
#define ANC_LIMITER_FF_THREHOLD (0x3fffff) //(0x32F52C)
#define ANC_LIMITER_TT_THREHOLD (0x3fffff) //(0x7fffff)
#define ANC_LIMITER_FB_THREHOLD (0x3fffff) //(0x32F52C)
#define ANC_LIMITER_MC_THREHOLD (0x3fffff) //(0x7fffff)
#define ANC_LIMITER_DEHOWLING_THREHOLD (0x3fffff) //(0x7fffff)
#define ANC_LIMITER_SPKCALIB_THREHOLD (0x3fffff) //(0x7fffff)

#define ANC_LIMITER_RELEASE_FF_TIME (10) // ms
#define ANC_LIMITER_RELEASE_TT_TIME (10) // ms
#define ANC_LIMITER_RELEASE_FB_TIME (10) // ms
#define ANC_LIMITER_RELEASE_MC_TIME (10) // ms
#define PSAP_DEHOWLING_LIMITER_RELEASE_TIME (10) // ms
#define SPKCALIB_LIMITER_RELEASE (10) // ms

#define ANC_LIMITER_ATT_FF_TIME (13.42f) // ms
#define ANC_LIMITER_ATT_TT_TIME (13.42f) // ms
#define ANC_LIMITER_ATT_FB_TIME (1.34f) // ms
#define ANC_LIMITER_ATT_MC_TIME (13.42f) // ms
#define PSAP_DEHOWLING_LIMITER_ATT_TIME (13.42f) // ms
#define SPKCALIB_LIMITER_ATT_TIME (13.42f) // ms

#if defined(ANC_LIMITER_FF)
#define ANC_LIMITER_ATT_FF_B1 (134217728/(uint32_t)(ANC_LIMITER_ATT_FF_TIME*100))
#define ANC_LIMITER_ATT_FF_A1 (134217728 -ANC_LIMITER_ATT_FF_B1)

#define ANC_LIMITER_RELEASE_FF_B1 (134217728/(uint32_t)(ANC_LIMITER_RELEASE_FF_TIME*100))
#define ANC_LIMITER_RELEASE_FF_A1 (134217728-ANC_LIMITER_RELEASE_FF_B1)

static const anc_iir_coefs   iir_coef_limiter_attack_ff = {
    .coef_b={0,    ANC_LIMITER_ATT_FF_B1,     0},
    .coef_a={134217728,    -ANC_LIMITER_ATT_FF_A1,     0},
};
static const anc_iir_coefs   iir_coef_limiter_release_ff = {
    .coef_b={0,    ANC_LIMITER_RELEASE_FF_B1,     0},
    .coef_a={134217728,    -ANC_LIMITER_RELEASE_FF_A1,     0},
};
#endif

#if defined(ANC_LIMITER_TT) && defined(AUDIO_ANC_TT_HW)
#define ANC_LIMITER_ATT_TT_B1 (134217728/(uint32_t)(ANC_LIMITER_ATT_TT_TIME*100))
#define ANC_LIMITER_ATT_TT_A1 (134217728 -ANC_LIMITER_ATT_TT_B1)

#define ANC_LIMITER_RELEASE_TT_B1 (134217728/(uint32_t)(ANC_LIMITER_RELEASE_TT_TIME*100))
#define ANC_LIMITER_RELEASE_TT_A1 (134217728-ANC_LIMITER_RELEASE_TT_B1)

static const anc_iir_coefs   iir_coef_limiter_attack_tt = {
    .coef_b={0,    ANC_LIMITER_ATT_TT_B1,     0},
    .coef_a={134217728,    -ANC_LIMITER_ATT_TT_A1,     0},
};
static const anc_iir_coefs   iir_coef_limiter_release_tt = {
    .coef_b={0,    ANC_LIMITER_RELEASE_TT_B1,     0},
    .coef_a={134217728,    -ANC_LIMITER_RELEASE_TT_A1,     0},
};
#endif

#if defined(ANC_LIMITER_FB)
#define ANC_LIMITER_ATT_FB_B1 (134217728/(uint32_t)(ANC_LIMITER_ATT_FB_TIME*100))
#define ANC_LIMITER_ATT_FB_A1 (134217728 -ANC_LIMITER_ATT_FB_B1)

#define ANC_LIMITER_RELEASE_FB_B1 (134217728/(uint32_t)(ANC_LIMITER_RELEASE_FB_TIME*100))
#define ANC_LIMITER_RELEASE_FB_A1 (134217728-ANC_LIMITER_RELEASE_FB_B1)

static const anc_iir_coefs   iir_coef_limiter_attack_fb = {
    .coef_b={0,    ANC_LIMITER_ATT_FB_B1,     0},
    .coef_a={134217728,    -ANC_LIMITER_ATT_FB_A1,     0},
};
static const anc_iir_coefs   iir_coef_limiter_release_fb = {
    .coef_b={0,    ANC_LIMITER_RELEASE_FB_B1,     0},
    .coef_a={134217728,    -ANC_LIMITER_RELEASE_FB_A1,     0},
};
#endif

#if defined(ANC_LIMITER_MC) && defined(AUDIO_ANC_FB_MC_HW)
#define ANC_LIMITER_ATT_MC_B1 (134217728/(uint32_t)(ANC_LIMITER_ATT_MC_TIME*100))
#define ANC_LIMITER_ATT_MC_A1 (134217728 -ANC_LIMITER_ATT_MC_B1)

#define ANC_LIMITER_RELEASE_MC_B1 (134217728/(uint32_t)(ANC_LIMITER_RELEASE_MC_TIME*100))
#define ANC_LIMITER_RELEASE_MC_A1 (134217728-ANC_LIMITER_RELEASE_MC_B1)

static POSSIBLY_UNUSED const anc_iir_coefs   iir_coef_limiter_attack_mc = {
    .coef_b={0,    ANC_LIMITER_ATT_MC_B1,     0},
    .coef_a={134217728,    -ANC_LIMITER_ATT_MC_A1,     0},
};
static POSSIBLY_UNUSED const anc_iir_coefs   iir_coef_limiter_release_mc = {
    .coef_b={0,    ANC_LIMITER_RELEASE_MC_B1,     0},
    .coef_a={134217728,    -ANC_LIMITER_RELEASE_MC_A1,     0},
};
#endif

/*******************************data struction***********************************/

#define ANC_BASE                            ((uint32_t)CODEC_BASE)

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
//IIR 0 .(left ff, gain and limiter iir coef)
volatile static struct _anc_iir_coefs *anc_ff_iir_coefs0_l = (volatile struct _anc_iir_coefs *)(ANC_BASE+0x0800);
volatile static struct _anc_iir_coefs *anc_ff_iir_coefs1_l = (volatile struct _anc_iir_coefs *)(ANC_BASE+0x0a00);
volatile static struct _anc_iir_coefs *anc_gain_ff_iir_coefs0_l = (volatile struct _anc_iir_coefs *)(ANC_BASE+0x0800+IIR_COUNTER*20);
volatile static struct _anc_iir_coefs *anc_gain_ff_iir_coefs1_l = (volatile struct _anc_iir_coefs *)(ANC_BASE+0x0a00+IIR_COUNTER*20);
#ifdef ANC_LIMITER_FF
volatile static struct _anc_iir_coefs *anc_limiter_ff_iir_coefs0_l = (volatile struct _anc_iir_coefs *)(ANC_BASE+0x0800+IIR_COUNTER*20+1*20);
volatile static struct _anc_iir_coefs *anc_limiter_ff_iir_coefs1_l = (volatile struct _anc_iir_coefs *)(ANC_BASE+0x0a00+IIR_COUNTER*20+1*20);
#endif

#if defined(AUDIO_ANC_TT_HW)
//IIR 1 .(left tt, gain and limiter iir coef)
volatile static struct _anc_iir_coefs *anc_tt_iir_coefs0_l = (volatile struct _anc_iir_coefs *)(ANC_BASE+0x0c00);
volatile static struct _anc_iir_coefs *anc_tt_iir_coefs1_l = (volatile struct _anc_iir_coefs *)(ANC_BASE+0x0e00);
volatile static struct _anc_iir_coefs *anc_gain_tt_iir_coefs0_l = (volatile struct _anc_iir_coefs *)(ANC_BASE+0x0c00+IIR_COUNTER*20);
volatile static struct _anc_iir_coefs *anc_gain_tt_iir_coefs1_l = (volatile struct _anc_iir_coefs *)(ANC_BASE+0x0e00+IIR_COUNTER*20);
#if defined(ANC_LIMITER_TT)
volatile static struct _anc_iir_coefs *anc_limiter_tt_iir_coefs0_l = (volatile struct _anc_iir_coefs *)(ANC_BASE+0x0c00+IIR_COUNTER*20+1*20);
volatile static struct _anc_iir_coefs *anc_limiter_tt_iir_coefs1_l = (volatile struct _anc_iir_coefs *)(ANC_BASE+0x0e00+IIR_COUNTER*20+1*20);
#endif
#endif

//IIR 4 .(left fb, gain and limiter iir coef)
volatile static struct _anc_iir_coefs *anc_fb_iir_coefs0_l = (volatile struct _anc_iir_coefs *)(ANC_BASE+0x1800);
volatile static struct _anc_iir_coefs *anc_fb_iir_coefs1_l = (volatile struct _anc_iir_coefs *)(ANC_BASE+0x1a00);
volatile static struct _anc_iir_coefs *anc_gain_fb_iir_coefs0_l = (volatile struct _anc_iir_coefs *)(ANC_BASE+0x1800+IIR_COUNTER*20);
volatile static struct _anc_iir_coefs *anc_gain_fb_iir_coefs1_l = (volatile struct _anc_iir_coefs *)(ANC_BASE+0x1a00+IIR_COUNTER*20);
#ifdef ANC_LIMITER_FF
volatile static struct _anc_iir_coefs *anc_limiter_fb_iir_coefs0_l = (volatile struct _anc_iir_coefs *)(ANC_BASE+0x1800+IIR_COUNTER*20+1*20);
volatile static struct _anc_iir_coefs *anc_limiter_fb_iir_coefs1_l = (volatile struct _anc_iir_coefs *)(ANC_BASE+0x1a00+IIR_COUNTER*20+1*20);
#endif

#if defined(AUDIO_ANC_FB_MC_HW)
//IIR 5 .(left mc, gain and limiter iir coef)
volatile static struct _anc_iir_coefs *anc_mc_iir_coefs0_l = (volatile struct _anc_iir_coefs *)(ANC_BASE+0x1c00);
volatile static struct _anc_iir_coefs *anc_mc_iir_coefs1_l = (volatile struct _anc_iir_coefs *)(ANC_BASE+0x1e00);
volatile static struct _anc_iir_coefs *anc_gain_mc_iir_coefs0_l = (volatile struct _anc_iir_coefs *)(ANC_BASE+0x1c00+IIR_COUNTER*20);
volatile static struct _anc_iir_coefs *anc_gain_mc_iir_coefs1_l = (volatile struct _anc_iir_coefs *)(ANC_BASE+0x1e00+IIR_COUNTER*20);
#if defined(ANC_LIMITER_MC)
volatile static struct _anc_iir_coefs *anc_limiter_mc_iir_coefs0_l = (volatile struct _anc_iir_coefs *)(ANC_BASE+0x1c00+IIR_COUNTER*20+1*20);
volatile static struct _anc_iir_coefs *anc_limiter_mc_iir_coefs1_l = (volatile struct _anc_iir_coefs *)(ANC_BASE+0x1e00+IIR_COUNTER*20+1*20);
#endif
#endif
#endif

#if defined(AUDIO_ANC_FIR_HW)
volatile static POSSIBLY_UNUSED int32_t *anc_fir0_sample = (volatile int32_t *)(ANC_BASE + 0x8000);
volatile static POSSIBLY_UNUSED int32_t *anc_fir0_sample_swap = (volatile int32_t *)(ANC_BASE + 0xC000);
volatile static POSSIBLY_UNUSED int32_t *anc_fir2_sample = (volatile int32_t *)(ANC_BASE + 0xA000);
volatile static POSSIBLY_UNUSED int32_t *anc_fir2_sample_swap = (volatile int32_t *)(ANC_BASE + 0xE000);

volatile static POSSIBLY_UNUSED int32_t *anc_fir0_coefs = (volatile int32_t *)(ANC_BASE + 0x8800);
volatile static POSSIBLY_UNUSED int32_t *anc_fir0_coefs_swap = (volatile int32_t *)(ANC_BASE + 0xC800);
volatile static POSSIBLY_UNUSED int32_t *anc_fir2_coefs = (volatile int32_t *)(ANC_BASE + 0xA800);
volatile static POSSIBLY_UNUSED int32_t *anc_fir2_coefs_swap = (volatile int32_t *)(ANC_BASE + 0xE800);

volatile static POSSIBLY_UNUSED int32_t *anc_fir1_sample = (volatile int32_t *)(ANC_BASE + 0x9000);
volatile static POSSIBLY_UNUSED int32_t *anc_fir1_sample_swap = (volatile int32_t *)(ANC_BASE + 0xD000);
volatile static POSSIBLY_UNUSED int32_t *anc_fir3_sample = (volatile int32_t *)(ANC_BASE + 0xB000);
volatile static POSSIBLY_UNUSED int32_t *anc_fir3_sample_swap = (volatile int32_t *)(ANC_BASE + 0xF000);

volatile static POSSIBLY_UNUSED int32_t *anc_fir1_coefs = (volatile int32_t *)(ANC_BASE + 0x9800);
volatile static POSSIBLY_UNUSED int32_t *anc_fir1_coefs_swap = (volatile int32_t *)(ANC_BASE + 0xD800);
volatile static POSSIBLY_UNUSED int32_t *anc_fir3_coefs = (volatile int32_t *)(ANC_BASE + 0xB800);
volatile static POSSIBLY_UNUSED int32_t *anc_fir3_coefs_swap = (volatile int32_t *)(ANC_BASE + 0xF800);

volatile static uint32_t fir_sample_step = 12;

volatile static uint32_t fir_max_order = 0;
#endif

volatile static int ff_open_flag = 0;
volatile static int fb_open_flag = 0;

volatile static int tt_open_flag = 0;
volatile static int mc_open_flag = 0;

volatile static int fb_check_open_flag = 0;
volatile static int anc_output_ch_map = AUD_CHANNEL_MAP_CH0;

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
volatile static int max_ff_gain_l = 0;
volatile static int max_fb_gain_l = 0;
volatile static int max_tt_gain_l = 0;
volatile static int max_mc_gain_l = 0;

volatile static int ff_ramp_gain_l = 0;
volatile static int fb_ramp_gain_l = 0;
volatile static int tt_ramp_gain_l = 0;
volatile static int mc_ramp_gain_l = 0;

volatile static int ff_ramp_coef_l = 0;
volatile static int fb_ramp_coef_l = 0;
volatile static int tt_ramp_coef_l = 0;
volatile static int mc_ramp_coef_l = 0;
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK) ||defined(ANC_TT_CHECK)
static HWTIMER_ID anc_howling_check_dev_timer_l = NULL;
volatile static int ff_howling_gain_l = 0;
volatile static int fb_howling_gain_l = 0;
volatile static int tt_howling_gain_l = 0;

static uint32_t howling_cnt_l = 0;
static uint32_t howling_time_l = 0;
#endif
#endif

static HWTIMER_ID anc_ff_set_gain_timer = NULL;
#if defined(AUDIO_ANC_TT_HW)
static HWTIMER_ID anc_tt_set_gain_timer = NULL;
#endif
static HWTIMER_ID anc_fb_set_gain_timer = NULL;
#if defined(AUDIO_ANC_FB_MC_HW)
static HWTIMER_ID anc_mc_set_gain_timer = NULL;
#endif

static HWTIMER_ID anc_ff_switching_timer = NULL;
#if defined(AUDIO_ANC_TT_HW)
static HWTIMER_ID anc_tt_switching_timer = NULL;
#endif
static HWTIMER_ID anc_fb_switching_timer = NULL;
#if defined(AUDIO_ANC_FB_MC_HW)
static HWTIMER_ID anc_mc_switching_timer = NULL;
#endif

#define ANC_SET_GAIN_DELAY_LONG  ((MS_TO_TICKS(200)))

#define ANC_SET_GAIN_DELAY_SHORT  ((MS_TO_TICKS(2)))

#define CALIB_GAIN_INVALID     (0xFFFF)

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
static iir_parameter ff_filtes_l_old;
#if defined(ANC_LIMITER_FF)
static anc_iir_coefs ff_limiter_att_l_old;
static anc_iir_coefs ff_limiter_rls_l_old;
#endif
static iir_parameter fb_filtes_l_old;
#if defined(ANC_LIMITER_FB)
static anc_iir_coefs fb_limiter_att_l_old;
static anc_iir_coefs fb_limiter_rls_l_old;
#endif
#if defined(AUDIO_ANC_TT_HW)
static iir_parameter tt_filtes_l_old;
#if defined(ANC_LIMITER_TT)
static anc_iir_coefs tt_limiter_att_l_old;
static anc_iir_coefs tt_limiter_rls_l_old;
#endif
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
static iir_parameter mc_filtes_l_old;
#if defined(ANC_LIMITER_MC)
static anc_iir_coefs mc_limiter_att_l_old;
static anc_iir_coefs mc_limiter_rls_l_old;
#endif
#endif

volatile static int32_t iir0_coef_using;
#if defined(AUDIO_ANC_TT_HW)
volatile static int32_t iir1_coef_using;
#endif
volatile static int32_t iir4_coef_using;
volatile static int32_t iir5_coef_using;

static int32_t g_calib_ff_gain_l = CALIB_GAIN_INVALID;
static int32_t g_calib_fb_gain_l = CALIB_GAIN_INVALID;
static int32_t g_calib_tt_gain_l = CALIB_GAIN_INVALID;
static int32_t g_calib_mc_gain_l = CALIB_GAIN_INVALID;
#endif

static int32_t trace_counter=0;

static struct_anc_cfg  anc_cfg_coef_new;
static struct_anc_cfg  anc_cfg_coef;

static int anc_switching_coef_delay_ff = 0;
static int anc_switching_gain_delay_ff = 0;

static int anc_switching_coef_delay_fb = 0;
static int anc_switching_gain_delay_fb = 0;

#if defined(AUDIO_ANC_TT_HW)
static int anc_switching_coef_delay_tt = 0;
static int anc_switching_gain_delay_tt = 0;
#endif

#if defined(AUDIO_ANC_FB_MC_HW)
static int anc_switching_coef_delay_mc = 0;
static int anc_switching_gain_delay_mc = 0;
#endif

volatile static bool anc_ff_using_flag = 0;
volatile static bool anc_tt_using_flag = 0;
volatile static bool anc_fb_using_flag = 0;
volatile static bool anc_mc_using_flag = 0;

volatile static bool anc_ff_reserve_flag = 0;
volatile static bool anc_tt_reserve_flag = 0;
volatile static bool anc_fb_reserve_flag = 0;
volatile static bool anc_mc_reserve_flag = 0;

volatile static bool using_tt_as_ff_en = false;

#define TRACE_COUNTER (10000)

static anc_bf_cfg_t g_anc_bf_cfg = {
    .enable = 0,

    .tt_delay_flag = 0,
    .tt_delay_num = 0,
    .tt_pdu_mix_flag = 0,
    .tt_pdu_off_flag = 0,

    .anc_ff_calib_gain = CALIB_GAIN_Q,
    .anc_tt_calib_gain = CALIB_GAIN_Q,
};
/*******************************iir ramp linear smooth mode*********************************/
// #define _ANC_RAMP_DEBUG

#if defined(_ANC_RAMP_DEBUG)
#define ANC_RAMP_LOG_D(str, ...)          TR_DEBUG(TR_MOD(AUD), "[ANC_RAMP]" str, ##__VA_ARGS__)
#else
#define ANC_RAMP_LOG_D(str, ...)
#endif

#define ANC_SMOOTH_RAMP_DELAY_MS                  (500)

#if defined(ANC_LINEAR_SMOOTH_CHECK)
#define ANC_LINEAR_SMOOTH_CHECK_TIMES             (3)

static int32_t anc_iir0_linear_smooth_fade_flag;
static int32_t anc_iir1_linear_smooth_fade_flag;
static int32_t anc_iir4_linear_smooth_fade_flag;
static int32_t anc_iir5_linear_smooth_fade_flag;
#endif
static uint32_t g_anc_linear_ramp_ms_curr = 300;

#if defined(ANC_IIR_LINEAR_SMOOTH_MODE)

#define CODEC_IIR_RAMP_STEP_MAX                   (1<<27)

#define IIR_REG_CONTINUE_ADDR_NUM                 (2)

#define ANC_SAMP_RATE_DISABLE_PLL                 (375000)
#define ANC_SAMP_RATE_ENABLE_PLL                  (384000)

static uint32_t ramp_check_status = 255;
static uint32_t g_anc_linear_ramp_ms_new = 300;
static uint32_t g_anc_linear_ramp_step_new = 1193; //CODEC_IIR_RAMP_STEP_MAX * (1 << CODEC_IIR_RAMP_INTVL_1_SAMP) / (sample_rate * ramp_ms / 1000);
static uint32_t g_anc_linear_ramp_step_curr = 1193;

enum ANC_IIR_ID_T {
    ANC_IIR_ID_0 = 0,
    ANC_IIR_ID_1,
    ANC_IIR_ID_4,
    ANC_IIR_ID_5,

    ANC_IIR_ID_NUM,
};

enum CODEC_IIR_RAMP_INTVL_T {
    CODEC_IIR_RAMP_INTVL_1_SAMP = 0, //default:each sample update gain
    CODEC_IIR_RAMP_INTVL_2_SAMP,     // 2 sample
    CODEC_IIR_RAMP_INTVL_4_SAMP,
    CODEC_IIR_RAMP_INTVL_8_SAMP,
    CODEC_IIR_RAMP_INTVL_16_SAMP,

    CODEC_IIR_RAMP_INTVL_QTY,
};

bool anc_ramp_get_status(uint32_t anc_fade_types)
{
    bool fade_ok = true;
    if(anc_fade_types & ANC_FEEDFORWARD) {
        fade_ok = fade_ok && ((GET_BITFIELD(codec->REG_2A0, CODEC_CODEC_IIR0_GAINA_EXT_OUT_SYNC)==GET_BITFIELD(codec->REG_260, CODEC_CODEC_IIR0_GAINA_EXT)) &&
                              (GET_BITFIELD(codec->REG_2A4, CODEC_CODEC_IIR0_GAINB_EXT_OUT_SYNC)==GET_BITFIELD(codec->REG_264, CODEC_CODEC_IIR0_GAINB_EXT)));

        ANC_RAMP_LOG_D("[%s][IIR0]:REG_2A0=%d,REG_260=%d, REG_2A4=%d, REG_264=%d", __func__,GET_BITFIELD(codec->REG_2A0, CODEC_CODEC_IIR0_GAINA_EXT_OUT_SYNC),
                                                                             GET_BITFIELD(codec->REG_260, CODEC_CODEC_IIR0_GAINA_EXT),
                                                                             GET_BITFIELD(codec->REG_2A4, CODEC_CODEC_IIR0_GAINB_EXT_OUT_SYNC),
                                                                             GET_BITFIELD(codec->REG_264, CODEC_CODEC_IIR0_GAINB_EXT));
    }

#if defined(AUDIO_ANC_TT_HW)
    if(anc_fade_types & ANC_TALKTHRU) {
        fade_ok = fade_ok && ((GET_BITFIELD(codec->REG_2A8,CODEC_CODEC_IIR1_GAINA_EXT_OUT_SYNC)==GET_BITFIELD(codec->REG_268, CODEC_CODEC_IIR1_GAINA_EXT)) &&
                              (GET_BITFIELD(codec->REG_2AC,CODEC_CODEC_IIR1_GAINB_EXT_OUT_SYNC)==GET_BITFIELD(codec->REG_26C, CODEC_CODEC_IIR1_GAINB_EXT)));
    ANC_RAMP_LOG_D("[%s][IIR1]:REG_2A8=%d,REG_268=%d, REG_2AC=%d, REG_26C=%d", __func__,GET_BITFIELD(codec->REG_2A8, CODEC_CODEC_IIR1_GAINA_EXT_OUT_SYNC),
                                                                             GET_BITFIELD(codec->REG_268, CODEC_CODEC_IIR1_GAINA_EXT),
                                                                             GET_BITFIELD(codec->REG_2AC, CODEC_CODEC_IIR1_GAINB_EXT_OUT_SYNC),
                                                                             GET_BITFIELD(codec->REG_26C, CODEC_CODEC_IIR1_GAINB_EXT));
    }
#endif

    if(anc_fade_types & ANC_FEEDBACK) {
        fade_ok = fade_ok && ((GET_BITFIELD(codec->REG_2C0,CODEC_CODEC_IIR4_GAINA_EXT_OUT_SYNC)==GET_BITFIELD(codec->REG_280, CODEC_CODEC_IIR4_GAINA_EXT)) &&
                              (GET_BITFIELD(codec->REG_2C4,CODEC_CODEC_IIR4_GAINB_EXT_OUT_SYNC)==GET_BITFIELD(codec->REG_284, CODEC_CODEC_IIR4_GAINB_EXT)));
        ANC_RAMP_LOG_D("[%s][IIR4]:REG_2C0=%d,REG_280=%d, REG_2C4=%d, REG_284=%d", __func__,GET_BITFIELD(codec->REG_2C0, CODEC_CODEC_IIR4_GAINA_EXT_OUT_SYNC),
                                                                             GET_BITFIELD(codec->REG_280, CODEC_CODEC_IIR4_GAINA_EXT),
                                                                             GET_BITFIELD(codec->REG_2C4, CODEC_CODEC_IIR4_GAINB_EXT_OUT_SYNC),
                                                                             GET_BITFIELD(codec->REG_284, CODEC_CODEC_IIR4_GAINB_EXT));
    }

#if defined(AUDIO_ANC_FB_MC_HW)
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if(anc_fade_types & ANC_MUSICCANCLE) {
        fade_ok = fade_ok && ((GET_BITFIELD(codec->REG_2C8,CODEC_CODEC_IIR5_GAINA_EXT_OUT_SYNC)==GET_BITFIELD(codec->REG_288, CODEC_CODEC_IIR5_GAINA_EXT)) &&
                              (GET_BITFIELD(codec->REG_2CC,CODEC_CODEC_IIR5_GAINB_EXT_OUT_SYNC)==GET_BITFIELD(codec->REG_28C, CODEC_CODEC_IIR5_GAINB_EXT)));
        ANC_RAMP_LOG_D("[%s][IIR5]:REG_2C8=%d,REG_288=%d, REG_2CC=%d, REG_28C=%d", __func__,GET_BITFIELD(codec->REG_2C8, CODEC_CODEC_IIR5_GAINA_EXT_OUT_SYNC),
                                                                             GET_BITFIELD(codec->REG_288, CODEC_CODEC_IIR5_GAINA_EXT),
                                                                             GET_BITFIELD(codec->REG_2CC, CODEC_CODEC_IIR5_GAINB_EXT_OUT_SYNC),
                                                                             GET_BITFIELD(codec->REG_28C, CODEC_CODEC_IIR5_GAINB_EXT));
    }
#endif
#endif

    return fade_ok;
}

static uint32_t anc_ramp_get_step(uint32_t sample_rate, uint32_t ramp_ms)
{
    return CODEC_IIR_RAMP_STEP_MAX * (1 << CODEC_IIR_RAMP_INTVL_1_SAMP) / (sample_rate * ramp_ms / 1000);
}

/*******************************iir0~iir5 config*********************************/
static void anc_ramp_set_timer(enum ANC_IIR_ID_T iir_id)
{
    DRIVERS_TRACE(1,"%s...",__func__);

    if (iir_id==ANC_IIR_ID_0) {
        anc_switching_coef_delay_ff = ((MS_TO_TICKS(g_anc_linear_ramp_ms_curr)));
    }

#if defined(AUDIO_ANC_TT_HW)
    if (iir_id==ANC_IIR_ID_1) {
        anc_switching_coef_delay_tt = ((MS_TO_TICKS(g_anc_linear_ramp_ms_curr)));
    }
#endif

    if (iir_id==ANC_IIR_ID_4) {
        anc_switching_coef_delay_fb = ((MS_TO_TICKS(g_anc_linear_ramp_ms_curr)));
    }

#if defined(AUDIO_ANC_FB_MC_HW)
    if (iir_id==ANC_IIR_ID_5) {
        anc_switching_coef_delay_mc = ((MS_TO_TICKS(g_anc_linear_ramp_ms_curr)));
    }
#endif

    return;
}

static void anc_ramp_update_step(enum ANC_IIR_ID_T iir_id)
{
    if (anc_ramp_get_status(ramp_check_status) && (g_anc_linear_ramp_step_curr != g_anc_linear_ramp_step_new)) {
        uint32_t lock;
        lock = int_lock();
        g_anc_linear_ramp_ms_curr = g_anc_linear_ramp_ms_new;
        g_anc_linear_ramp_step_curr = g_anc_linear_ramp_step_new;
        anc_ramp_set_timer(iir_id);
        int_unlock(lock);
        codec->REG_224 &= ~(CODEC_CODEC_IIR0_RAMP_EN << iir_id);
        codec->REG_220 = SET_BITFIELD(codec->REG_220, CODEC_CODEC_IIR_RAMP_STEP, g_anc_linear_ramp_step_curr);
        codec->REG_224 |= (CODEC_CODEC_IIR0_RAMP_EN << iir_id);
        DRIVERS_TRACE(2,"%s:ramp ms=%d, ramp step=%d, iir_id=%d ...",__func__,g_anc_linear_ramp_ms_curr, g_anc_linear_ramp_step_curr, iir_id);
    }
}

static void anc_iir_reg_reset(enum ANC_IIR_ID_T iir_id)
{
    DRIVERS_TRACE(1,"%s...",__func__);
    if(iir_id < IIR_REG_CONTINUE_ADDR_NUM) { //iir0 ~ iir1
        *(&codec->REG_248 + iir_id) &= ~CODEC_CODEC_IIR0_ENABLE;
        *(&codec->REG_248 + iir_id) &= ~CODEC_CODEC_IIR0_IIRA_ENABLE;
        *(&codec->REG_248 + iir_id) &= ~CODEC_CODEC_IIR0_IIRB_ENABLE;
        *(&codec->REG_248 + iir_id) &= ~CODEC_CODEC_IIR0_COEF_SWAP;
        *(&codec->REG_248 + iir_id) &= ~CODEC_CODEC_IIR0_AUTO_STOP;
    } else {  //iir4 ~ iir5
        *(&codec->REG_680 + iir_id - IIR_REG_CONTINUE_ADDR_NUM) &= ~CODEC_CODEC_IIR4_ENABLE;
        *(&codec->REG_680 + iir_id - IIR_REG_CONTINUE_ADDR_NUM) &= ~CODEC_CODEC_IIR4_IIRA_ENABLE;
        *(&codec->REG_680 + iir_id - IIR_REG_CONTINUE_ADDR_NUM) &= ~CODEC_CODEC_IIR4_IIRB_ENABLE;
        *(&codec->REG_680 + iir_id - IIR_REG_CONTINUE_ADDR_NUM) &= ~CODEC_CODEC_IIR4_COEF_SWAP;
        *(&codec->REG_680 + iir_id - IIR_REG_CONTINUE_ADDR_NUM) &= ~CODEC_CODEC_IIR4_AUTO_STOP;
    }

    codec->REG_224 &= ~(CODEC_CODEC_IIR0_RAMP_EN << iir_id);
}

static void anc_iir_reg_enable(enum ANC_IIR_ID_T iir_id)
{
    DRIVERS_TRACE(1,"%s...",__func__);

    codec->REG_224 |= (CODEC_CODEC_IIR0_RAMP_EN << iir_id);

    if(iir_id < IIR_REG_CONTINUE_ADDR_NUM) { //iir0 ~ iir2
        *(&codec->REG_248 + iir_id) |= CODEC_CODEC_IIR0_AUTO_STOP;
        *(&codec->REG_248 + iir_id) |= CODEC_CODEC_IIR0_IIRA_ENABLE;
        *(&codec->REG_248 + iir_id) |= CODEC_CODEC_IIR0_IIRB_ENABLE;
        *(&codec->REG_248 + iir_id) |= CODEC_CODEC_IIR0_ENABLE;
    } else {  //iir4 ~ iir5
        *(&codec->REG_680 + iir_id - IIR_REG_CONTINUE_ADDR_NUM) |= CODEC_CODEC_IIR4_AUTO_STOP;
        *(&codec->REG_680 + iir_id - IIR_REG_CONTINUE_ADDR_NUM) |= CODEC_CODEC_IIR4_IIRA_ENABLE;
        *(&codec->REG_680 + iir_id - IIR_REG_CONTINUE_ADDR_NUM) |= CODEC_CODEC_IIR4_IIRB_ENABLE;
        *(&codec->REG_680 + iir_id - IIR_REG_CONTINUE_ADDR_NUM) |= CODEC_CODEC_IIR4_ENABLE;
    }

}
#endif

int32_t anc_ramp_set_time(uint32_t ms)
{
#if defined(ANC_IIR_LINEAR_SMOOTH_MODE)

    uint32_t lock;
    lock = int_lock();
    g_anc_linear_ramp_ms_new = ms;
    g_anc_linear_ramp_step_new = anc_ramp_get_step(ANC_SAMP_RATE_DISABLE_PLL, ms);
    int_unlock(lock);
#if defined(ANC_RAMP_DEBUG)
    DRIVERS_TRACE(5,"%s:ramp ms cur=%d, ramp step cur=%d, ramp ms new=%d, ramp step new=%d...",__func__,g_anc_linear_ramp_ms_curr,
                                                                                                g_anc_linear_ramp_step_curr,
                                                                                                g_anc_linear_ramp_ms_new,
                                                                                                g_anc_linear_ramp_step_new);
#endif
#else
    g_anc_linear_ramp_ms_curr = ANC_SMOOTH_RAMP_DELAY_MS;
#endif

    return 0;
}

int32_t anc_ramp_get_time(void)
{
    // ms
    return g_anc_linear_ramp_ms_curr;
}

/*******************************FIR hardware filter*********************************/
static bool anc_fir_open_flag = 0;

#if defined(AUDIO_ANC_FIR_HW)
static bool ff_l_fir_open_flag = 0;
aud_fir_item POSSIBLY_UNUSED fir_ff_filtes_l_old;
#endif

int32_t anc_fir_set_status(bool flag)
{
    DRIVERS_TRACE(1,"%s: open=%d ...", __func__, flag);

    anc_fir_open_flag = flag;

    return 0;
}

#if defined(AUDIO_ANC_FIR_HW)
static void anc_fir_ctrl_reg_open(enum ANC_TYPE_T anc_type, int32_t ch_map)
{
    DRIVERS_TRACE(1, "%s: anc_type=0x%x, ch_map=0x%x ...", __func__, anc_type, ch_map);

    if (anc_type == ANC_FEEDFORWARD) {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if ((ch_map & AUD_CHANNEL_MAP_CH0) && ff_l_fir_open_flag) {
            // setting the feedforward FIR filters;
            codec->REG_108 &= ~CODEC_STREAM0_FIR1_CH0;
            codec->REG_100 |= CODEC_FIR_STREAM_ENABLE_CH0;
            codec->REG_104 &= ~CODEC_ANC_COEF_SEL_FIR_CH0;

            // feedforward left ch FIR sample clear
            for (int i = 0; i < 512; i++) {
                anc_fir0_sample[i] = 0;
            }

            // feedforward left ch FIR coefs settings
            for (int i = 0; i < MAX_FIR_LEN; i++) {
                anc_fir0_coefs[i] = 0;
            }
            anc_fir0_coefs[0] = 32767 * 256;

            codec->REG_10C = SET_BITFIELD(codec->REG_10C, CODEC_FIR_GAIN_SEL_CH0, 6);
            codec->REG_0D0 |= CODEC_CODEC_FF_CH0_FIR_EN;
        } else {
            codec->REG_0D0 &= ~CODEC_CODEC_FF_CH0_FIR_EN;
        }
    }
#endif
}

static int fir_filter_para_copy(aud_fir_item *fir_filtes_old, const aud_fir_item *fir_filtes_new)
{
    fir_filtes_old->fir_len = fir_filtes_new->fir_len;
    fir_filtes_old->fir_step = fir_filtes_new->fir_step;
    fir_filtes_old->fir_bypass_flag = fir_filtes_new->fir_bypass_flag;

    return 0;
}

static void anc_fir_comb_cfg(void)
{
    uint32_t fir0_len = 0;

    if (ff_l_fir_open_flag) {
        fir0_len = fir_ff_filtes_l_old.fir_len * fir_ff_filtes_l_old.fir_step;
    }

    DRIVERS_TRACE(0,"fir0_len(%d)", fir0_len);

    if (fir0_len > 2048) {
        // 4-ch FIR cascade
        codec->REG_12C = SET_BITFIELD(codec->REG_12C, CODEC_FIR_COMB_CNT_CH0, 2);

        // FIR0 as master
        codec->REG_12C |= CODEC_FIR_COMB_MST_EN_CH0;
        codec->REG_12C &= ~CODEC_FIR_COMB_MST_EN_CH1;
        codec->REG_12C &= ~CODEC_FIR_COMB_MST_EN_CH2;
        codec->REG_12C &= ~CODEC_FIR_COMB_MST_EN_CH3;

        // Enable all fir cascade
        codec->REG_77C |= CODEC_FIR_COMB_SLV_EN_CH0;
        codec->REG_77C |= CODEC_FIR_COMB_SLV_EN_CH1;
        codec->REG_77C |= CODEC_FIR_COMB_SLV_EN_CH2;
        codec->REG_77C |= CODEC_FIR_COMB_SLV_EN_CH3;
    }
}

void anc_fir_open(struct_anc_fir_cfg *cfg, enum ANC_TYPE_T anc_type)
{
    aud_fir_item POSSIBLY_UNUSED *ff_filtes_l = &(cfg->anc_fir_cfg_ff_l);

    uint32_t fir_len = 0;

    if (anc_type == ANC_FEEDFORWARD) {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            fir_filter_para_copy(&fir_ff_filtes_l_old, ff_filtes_l);
            if (ff_filtes_l->fir_len != 0) {
                codec->REG_108 = SET_BITFIELD(codec->REG_108, CODEC_FIR_ORDER_CH0, ff_filtes_l->fir_len);
                fir_len = ff_filtes_l->fir_len * ff_filtes_l->fir_step;
                codec->REG_100 |= CODEC_FIR_STREAM_ENABLE_CH0;
                if (fir_len > 1024) {
                    codec->REG_100 |= CODEC_FIR_STREAM_ENABLE_CH1;
                }
                if (fir_len > 2048) {
                    codec->REG_100 |= CODEC_FIR_STREAM_ENABLE_CH2;
                    codec->REG_100 |= CODEC_FIR_STREAM_ENABLE_CH3;
                }
                ff_l_fir_open_flag = 1;
            } else {
                ff_l_fir_open_flag = 0;
            }
        }
#endif
    }

    anc_fir_ctrl_reg_open(anc_type, anc_output_ch_map);

    if (!ff_l_fir_open_flag) {
        codec->REG_100 |= CODEC_FIR_STREAM_ENABLE_CH0;
        codec->REG_100 |= CODEC_FIR_STREAM_ENABLE_CH1;
        codec->REG_100 |= CODEC_FIR_STREAM_ENABLE_CH2;
        codec->REG_100 |= CODEC_FIR_STREAM_ENABLE_CH3;
    }

    anc_fir_comb_cfg();
}

int anc_set_fir_cfg(struct_anc_fir_cfg *cfg, enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err = ANC_NO_ERR;
    int i;
    aud_fir_item POSSIBLY_UNUSED *ff_filtes_l = &(cfg->anc_fir_cfg_ff_l);

    /******************************************************************************/
    if (anc_type & ANC_FEEDFORWARD && (ff_l_fir_open_flag == 0)) {
        DRIVERS_TRACE(1, "%s: anc_type = %d not opened !!!", __func__, anc_type);
        return ANC_NO_ERR;
    }

    if (anc_type == ANC_FEEDFORWARD) {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV == AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            // setting the feedforward FIR filters;
            if (ff_filtes_l->fir_bypass_flag == 0) {
                if (ff_filtes_l->fir_len > 0) {

                    codec->REG_1A4 |= CODEC_ANC_COEF_SWITCH_REQ_CH0;
                    while ((codec->REG_1A4 & CODEC_ANC_COEF_SWITCH_ACK_CH0_SYNC_1) == 0)
                        ;
                    if ((codec->REG_104 & CODEC_ANC_COEF_SEL_FIR_CH0_NEW_SYNC_1) == 0) {
                        // feedforward left ch FIR coefs settings
                        for (i = 0; i < ff_filtes_l->fir_len; i++) {
                            anc_fir0_coefs_swap[i] = (int32_t)ff_filtes_l->fir_coef[i];
                        }
                        for (; i < MAX_FIR_LEN; i++) {
                            anc_fir0_coefs_swap[i] = 0;
                        }
                        // DRIVERS_TRACE(1, "FIR switching 0!");
                    } else {
                        // feedforward left ch FIR coefs settings
                        for (i = 0; i < ff_filtes_l->fir_len; i++) {
                            anc_fir0_coefs[i] = (int32_t)ff_filtes_l->fir_coef[i];
                        }
                        for (; i < MAX_FIR_LEN; i++) {
                            anc_fir0_coefs[i] = 0;
                        }
                        // DRIVERS_TRACE(1, "FIR switching 1!");
                    }

                    //NOTE: pass0 update coefficients. If can't get signal pass0, need to disable it and make coef update instant.
                    codec->REG_104 |= CODEC_ANC_COEF_SWITCH_MODE_CH0;
                    codec->REG_1A4 &= ~CODEC_ANC_COEF_SWITCH_REQ_CH0;
                    if ((codec->REG_104 & CODEC_ANC_COEF_SEL_FIR_CH0_NEW_SYNC_1) == 0) {
                        codec->REG_104 |= CODEC_ANC_COEF_SEL_FIR_CH0;
                    } else {
                        codec->REG_104 &= ~CODEC_ANC_COEF_SEL_FIR_CH0;
                    }
                    codec->REG_0D0 |= CODEC_CODEC_FF_CH0_FIR_EN;
                    DRIVERS_TRACE(1, "FIR switching left!");
                }
            } else {
                DRIVERS_TRACE(1, "warning FIR BYPASS!");
                codec->REG_108 = SET_BITFIELD(codec->REG_108, CODEC_FIR_ORDER_CH0, 0);
                codec->REG_0D0 &= ~CODEC_CODEC_FF_CH0_FIR_EN;
            }
        }
#endif
    }
    return err;
}
#endif

static int iir_filter_para_copy(iir_parameter *filtes_old,const aud_item *filtes_new,uint16_t iir_counter)
{
    filtes_old->iir_bypass_flag = filtes_new->iir_bypass_flag;
    filtes_old->iir_counter = filtes_new->iir_counter;

    int i;

    if (filtes_new->iir_counter > iir_counter) {
        DRIVERS_TRACE(1,"%s: warning filtes_new->iir_counter:%d",__func__,filtes_new->iir_counter);
        filtes_old->iir_counter = iir_counter;
    }
    else if (filtes_new->iir_counter <= 0) {
        DRIVERS_TRACE(1,"%s: warning filtes_new->iir_counter:%d",__func__,filtes_new->iir_counter);
        filtes_old->iir_counter = 0;
        filtes_old->iir_bypass_flag = 1;
    }

    for (i = 0; i < filtes_old->iir_counter; i++) {
        filtes_old->iir_coef[i] = filtes_new->iir_coef[i];
    }

    for ( ; i < iir_counter; i++) {
        filtes_old->iir_coef[i].coef_a[0] = FIXED_COEF_Q;
        filtes_old->iir_coef[i].coef_a[1] = 0;
        filtes_old->iir_coef[i].coef_a[2] = 0;
        filtes_old->iir_coef[i].coef_b[0] = FIXED_COEF_Q;
        filtes_old->iir_coef[i].coef_b[1] = 0;
        filtes_old->iir_coef[i].coef_b[2] = 0;
    }

    filtes_old->total_gain = filtes_new->total_gain;
    filtes_old->iir_counter = iir_counter;

    return 0;
}


static int iir_filter_coef_copy(volatile struct _anc_iir_coefs *iir_coefs, iir_parameter * __restrict filtes_old)
{
    int i;

    for (i = 0; i< filtes_old->iir_counter; i++) {
        iir_coefs[i].a1 = -filtes_old->iir_coef[i].coef_a[1];
        iir_coefs[i].a2 = -filtes_old->iir_coef[i].coef_a[2];
        iir_coefs[i].b0 = filtes_old->iir_coef[i].coef_b[0];
        iir_coefs[i].b1 = filtes_old->iir_coef[i].coef_b[1];
        iir_coefs[i].b2 = filtes_old->iir_coef[i].coef_b[2];
    }

    if(filtes_old->iir_counter > 0) {
        iir_coefs[0].b0 = (int32_t)((filtes_old->iir_coef[0].coef_b[0]*(int64_t)filtes_old->total_gain)>>GAIN_Q);
        iir_coefs[0].b1 = (int32_t)((filtes_old->iir_coef[0].coef_b[1]*(int64_t)filtes_old->total_gain)>>GAIN_Q);
        iir_coefs[0].b2 = (int32_t)((filtes_old->iir_coef[0].coef_b[2]*(int64_t)filtes_old->total_gain)>>GAIN_Q);
    }

    return 0;
}

static int iir_coef_copy(volatile struct _anc_iir_coefs *iir_coefs, anc_iir_coefs * __restrict filtes_old)
{
#if 0
    DRIVERS_TRACE(1,"%s: iir_coefs:0x%x",__func__,(uint32_t)iir_coefs);
    DRIVERS_TRACE(1,"%s: -filtes_old->coef_a[1]:0x%x",__func__,-filtes_old->coef_a[1]);
    DRIVERS_TRACE(1,"%s: -filtes_old->coef_a[2]:0x%x",__func__, -filtes_old->coef_a[2]);
    DRIVERS_TRACE(1,"%s: -filtes_old->coef_a[0]:0x%x",__func__, filtes_old->coef_b[0]);
    DRIVERS_TRACE(1,"%s: -filtes_old->coef_b[1]:0x%x",__func__, filtes_old->coef_b[1]);
    DRIVERS_TRACE(1,"%s: -filtes_old->coef_b[2]:0x%x",__func__, filtes_old->coef_b[2]);
#endif
    iir_coefs->a1 = -filtes_old->coef_a[1];
    iir_coefs->a2 = -filtes_old->coef_a[2];
    iir_coefs->b0 = filtes_old->coef_b[0];
    iir_coefs->b1 = filtes_old->coef_b[1];
    iir_coefs->b2 = filtes_old->coef_b[2];

    return 0;
}

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
static int iir0_gaina_cfg_gain(void)
{
    int32_t iir0_gaina;
    uint32_t lock;

#if defined(ANC_FF_CHECK)
    iir0_gaina=(int32_t)((((float)ff_ramp_coef_l/512.0f)*((float)ff_ramp_gain_l/512.0f)*((float)ff_howling_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
#else
    iir0_gaina=(int32_t)((((float)ff_ramp_coef_l/512.0f)*((float)ff_ramp_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
#endif

    //DRIVERS_TRACE(1,"%s:iir0_gaina:%d",__func__,iir0_gaina);
#if defined(ANC_LINEAR_SMOOTH_CHECK)
    for (int i=0; i<ANC_LINEAR_SMOOTH_CHECK_TIMES; ++i) {
#endif
        lock = int_lock();
        codec->REG_25C &= ~CODEC_CODEC_IIR0_GAIN_EXT_UPDATE;
#if defined(ANC_IIR_LINEAR_SMOOTH_MODE)
        anc_ramp_update_step(ANC_IIR_ID_0);
#endif
        codec->REG_260 = SET_BITFIELD(codec->REG_260, CODEC_CODEC_IIR0_GAINA_EXT, iir0_gaina);
        codec->REG_264 = SET_BITFIELD(codec->REG_264, CODEC_CODEC_IIR0_GAINB_EXT, ANC_GAIN_RAMP_ZERO);
        hal_sys_timer_delay_us(1);
        // DRIVERS_TRACE(1,"%s:iir0_gaina:%d, REG_260=%d",__func__,iir0_gaina, GET_BITFIELD(codec->REG_260, CODEC_CODEC_IIR0_GAINA_EXT));
#if defined(ANC_LINEAR_SMOOTH_CHECK)
        int32_t iir0_gaina_sync = GET_BITFIELD(codec->REG_2A0, CODEC_CODEC_IIR0_GAINA_EXT_OUT_SYNC);
#endif
        codec->REG_25C |= CODEC_CODEC_IIR0_GAIN_EXT_UPDATE;
        int_unlock(lock);
#if defined(ANC_LINEAR_SMOOTH_CHECK)
        if (anc_iir0_linear_smooth_fade_flag == 1) {
            anc_iir0_linear_smooth_fade_flag = 0;
            break;
        } else {
            hal_sys_timer_delay_us(10);
            if (GET_BITFIELD(codec->REG_2A0, CODEC_CODEC_IIR0_GAINA_EXT_OUT_SYNC) != iir0_gaina_sync) {
                break;
            }
            DRIVERS_TRACE(1,"%s:iir0_gaina:%d, REG_260=%d, REG_2A0=%d, ANC FADE CHECK,  NEED SET AGAIN !!!",__func__,iir0_gaina,
                                                            GET_BITFIELD(codec->REG_260, CODEC_CODEC_IIR0_GAINA_EXT),
                                                            GET_BITFIELD(codec->REG_2A0, CODEC_CODEC_IIR0_GAINA_EXT_OUT_SYNC));
        }
    }
#endif

    return 0;
}

static int iir0_gainb_cfg_gain(void)
{
    int32_t iir0_gainb;
    uint32_t lock;

#if defined(ANC_FF_CHECK)
    iir0_gainb=(int32_t)((((float)ff_ramp_coef_l/512.0f)*((float)ff_ramp_gain_l/512.0f)*((float)ff_howling_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
#else
    iir0_gainb=(int32_t)((((float)ff_ramp_coef_l/512.0f)*((float)ff_ramp_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
#endif

    //DRIVERS_TRACE(1,"%s:iir0_gainb:%d",__func__,iir0_gainb);
#if defined(ANC_LINEAR_SMOOTH_CHECK)
    for (int i=0; i<ANC_LINEAR_SMOOTH_CHECK_TIMES; ++i) {
#endif
        lock = int_lock();
        codec->REG_25C &= ~CODEC_CODEC_IIR0_GAIN_EXT_UPDATE;
#if defined(ANC_IIR_LINEAR_SMOOTH_MODE)
        anc_ramp_update_step(ANC_IIR_ID_0);
#endif
        codec->REG_260 = SET_BITFIELD(codec->REG_260, CODEC_CODEC_IIR0_GAINA_EXT, ANC_GAIN_RAMP_ZERO);
        codec->REG_264 = SET_BITFIELD(codec->REG_264, CODEC_CODEC_IIR0_GAINB_EXT, iir0_gainb);
        hal_sys_timer_delay_us(1);
        // DRIVERS_TRACE(1,"%s:iir0_gainb:%d, REG_264=%d",__func__,iir0_gainb, GET_BITFIELD(codec->REG_264, CODEC_CODEC_IIR0_GAINB_EXT));
#if defined(ANC_LINEAR_SMOOTH_CHECK)
        int32_t iir0_gainb_sync = GET_BITFIELD(codec->REG_2A4, CODEC_CODEC_IIR0_GAINB_EXT_OUT_SYNC);
#endif
        codec->REG_25C |= CODEC_CODEC_IIR0_GAIN_EXT_UPDATE;
        int_unlock(lock);
#if defined(ANC_LINEAR_SMOOTH_CHECK)
        if (anc_iir0_linear_smooth_fade_flag == 1) {
            anc_iir0_linear_smooth_fade_flag = 0;
            break;
        } else {
            hal_sys_timer_delay_us(10);
            if (GET_BITFIELD(codec->REG_2A4, CODEC_CODEC_IIR0_GAINB_EXT_OUT_SYNC) != iir0_gainb_sync) {
                break;
            }
            DRIVERS_TRACE(1,"%s:iir0_gainb:%d, REG_264=%d, REG_2A4=%d, ANC FADE CHECK,  NEED SET AGAIN !!!",__func__,iir0_gainb,
                                                            GET_BITFIELD(codec->REG_264, CODEC_CODEC_IIR0_GAINB_EXT),
                                                            GET_BITFIELD(codec->REG_2A4, CODEC_CODEC_IIR0_GAINB_EXT_OUT_SYNC));
        }
    }
#endif

    return 0;
}
#if defined(AUDIO_ANC_TT_HW)
static int iir1_gaina_cfg_gain(void)
{
    int32_t iir1_gaina;
    uint32_t lock;

#if defined(ANC_TT_CHECK)
    iir1_gaina=(int32_t)((((float)tt_ramp_coef_l/512.0f)*((float)tt_ramp_gain_l/512.0f)*((float)tt_howling_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
#else
    iir1_gaina=(int32_t)((((float)tt_ramp_coef_l/512.0f)*((float)tt_ramp_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
#endif

    //DRIVERS_TRACE(1,"%s:iir1_gaina:%d",__func__,iir1_gaina);

#if defined(ANC_LINEAR_SMOOTH_CHECK)
    for (int i=0; i<ANC_LINEAR_SMOOTH_CHECK_TIMES; ++i) {
#endif
        lock = int_lock();
        codec->REG_25C &= ~CODEC_CODEC_IIR1_GAIN_EXT_UPDATE;
#if defined(ANC_IIR_LINEAR_SMOOTH_MODE)
        anc_ramp_update_step(ANC_IIR_ID_1);
#endif
        codec->REG_268 = SET_BITFIELD(codec->REG_268, CODEC_CODEC_IIR1_GAINA_EXT, iir1_gaina);
        codec->REG_26C = SET_BITFIELD(codec->REG_26C, CODEC_CODEC_IIR1_GAINB_EXT, ANC_GAIN_RAMP_ZERO);
        hal_sys_timer_delay_us(1);
#if defined(ANC_LINEAR_SMOOTH_CHECK)
        int32_t iir1_gaina_sync = GET_BITFIELD(codec->REG_2A8, CODEC_CODEC_IIR1_GAINA_EXT_OUT_SYNC);
#endif
        codec->REG_25C |= CODEC_CODEC_IIR1_GAIN_EXT_UPDATE;
        int_unlock(lock);
#if defined(ANC_LINEAR_SMOOTH_CHECK)
        if (anc_iir1_linear_smooth_fade_flag == 1) {
            anc_iir1_linear_smooth_fade_flag = 0;
            break;
        } else {
            hal_sys_timer_delay_us(10);
            if (GET_BITFIELD(codec->REG_2A8, CODEC_CODEC_IIR1_GAINA_EXT_OUT_SYNC) != iir1_gaina_sync) {
                break;
            }
            DRIVERS_TRACE(1,"%s:iir1_gaina:%d, REG_268=%d, REG_2A8=%d, ANC FADE CHECK,  NEED SET AGAIN !!!",__func__,iir1_gaina,
                                                            GET_BITFIELD(codec->REG_268, CODEC_CODEC_IIR1_GAINA_EXT),
                                                            GET_BITFIELD(codec->REG_2A8, CODEC_CODEC_IIR1_GAINA_EXT_OUT_SYNC));
        }
    }
#endif

    return 0;
}

static int iir1_gainb_cfg_gain(void)
{
    int32_t iir1_gainb;
    uint32_t lock;

#if defined(ANC_TT_CHECK)
    iir1_gainb=(int32_t)((((float)tt_ramp_coef_l/512.0f)*((float)tt_ramp_gain_l/512.0f)*((float)ff_howling_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
#else
    iir1_gainb=(int32_t)((((float)tt_ramp_coef_l/512.0f)*((float)tt_ramp_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
#endif

    //DRIVERS_TRACE(1,"%s:iir1_gainb:%d",__func__,iir1_gainb);

#if defined(ANC_LINEAR_SMOOTH_CHECK)
    for (int i=0; i<ANC_LINEAR_SMOOTH_CHECK_TIMES; ++i) {
#endif
        lock = int_lock();
        codec->REG_25C &= ~CODEC_CODEC_IIR1_GAIN_EXT_UPDATE;
#if defined(ANC_IIR_LINEAR_SMOOTH_MODE)
        anc_ramp_update_step(ANC_IIR_ID_1);
#endif
        codec->REG_268 = SET_BITFIELD(codec->REG_268, CODEC_CODEC_IIR1_GAINA_EXT, ANC_GAIN_RAMP_ZERO);
        codec->REG_26C = SET_BITFIELD(codec->REG_26C, CODEC_CODEC_IIR1_GAINB_EXT, iir1_gainb);
        hal_sys_timer_delay_us(1);
#if defined(ANC_LINEAR_SMOOTH_CHECK)
        int32_t iir1_gainb_sync = GET_BITFIELD(codec->REG_2AC, CODEC_CODEC_IIR1_GAINB_EXT_OUT_SYNC);
#endif
        codec->REG_25C |= CODEC_CODEC_IIR1_GAIN_EXT_UPDATE;
        int_unlock(lock);
#if defined(ANC_LINEAR_SMOOTH_CHECK)
        if (anc_iir1_linear_smooth_fade_flag == 1) {
            anc_iir1_linear_smooth_fade_flag = 0;
            break;
        } else {
            hal_sys_timer_delay_us(10);
            if (GET_BITFIELD(codec->REG_2AC, CODEC_CODEC_IIR1_GAINB_EXT_OUT_SYNC) != iir1_gainb_sync) {
                break;
            }
            DRIVERS_TRACE(1,"%s:iir1_gainb:%d, REG_26C=%d, REG_2AC=%d, ANC FADE CHECK,  NEED SET AGAIN !!!",__func__,iir1_gainb,
                                                            GET_BITFIELD(codec->REG_26C, CODEC_CODEC_IIR1_GAINB_EXT),
                                                            GET_BITFIELD(codec->REG_2AC, CODEC_CODEC_IIR1_GAINB_EXT_OUT_SYNC));
        }
    }
#endif

    return 0;
}
#endif
static int iir4_gaina_cfg_gain(void)
{
    int32_t iir4_gaina;
    uint32_t lock;

#if defined(ANC_FB_CHECK)
    iir4_gaina=(int32_t)((((float)fb_ramp_coef_l/512.0f)*((float)fb_ramp_gain_l/512.0f)*((float)fb_howling_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
#else
    iir4_gaina=(int32_t)((((float)fb_ramp_coef_l/512.0f)*((float)fb_ramp_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
#endif

    ANC_RAMP_LOG_D("%s:ramp ms cur=%d, ramp step cur=%d, ramp ms new=%d, ramp step new=%d...",__func__,
                                                                                       g_anc_linear_ramp_ms_curr,
                                                                                       g_anc_linear_ramp_step_curr,
                                                                                       g_anc_linear_ramp_ms_new,
                                                                                       g_anc_linear_ramp_step_new);
    ANC_RAMP_LOG_D("%s:iir4_gaina:%d",__func__,iir4_gaina);

#if defined(ANC_LINEAR_SMOOTH_CHECK)
    for (int i=0; i<ANC_LINEAR_SMOOTH_CHECK_TIMES; ++i) {
#endif
        lock = int_lock();
        codec->REG_25C &= ~CODEC_CODEC_IIR4_GAIN_EXT_UPDATE;
#if defined(ANC_IIR_LINEAR_SMOOTH_MODE)
        anc_ramp_update_step(ANC_IIR_ID_4);
#endif
        codec->REG_280 = SET_BITFIELD(codec->REG_280, CODEC_CODEC_IIR4_GAINA_EXT, iir4_gaina);
        codec->REG_284 = SET_BITFIELD(codec->REG_284, CODEC_CODEC_IIR4_GAINB_EXT, ANC_GAIN_RAMP_ZERO);
        hal_sys_timer_delay_us(1);
        // DRIVERS_TRACE(1,"%s:iir4_gaina:%d, REG_280=%d",__func__,iir4_gaina, GET_BITFIELD(codec->REG_280, CODEC_CODEC_IIR4_GAINA_EXT));
#if defined(ANC_LINEAR_SMOOTH_CHECK)
        int32_t iir4_gaina_sync = GET_BITFIELD(codec->REG_2C0, CODEC_CODEC_IIR4_GAINA_EXT_OUT_SYNC);
#endif
        codec->REG_25C |= CODEC_CODEC_IIR4_GAIN_EXT_UPDATE;
        int_unlock(lock);
#if defined(ANC_LINEAR_SMOOTH_CHECK)
        if (anc_iir4_linear_smooth_fade_flag == 1) {
            anc_iir4_linear_smooth_fade_flag = 0;
            break;
        } else {
            hal_sys_timer_delay_us(10);
            if (GET_BITFIELD(codec->REG_2C0, CODEC_CODEC_IIR4_GAINA_EXT_OUT_SYNC) != iir4_gaina_sync) {
                break;
            }
            DRIVERS_TRACE(1,"%s:iir4_gaina:%d, REG_280=%d, REG_2C0=%d, ANC FADE CHECK,  NEED SET AGAIN !!!",__func__,iir4_gaina,
                                                            GET_BITFIELD(codec->REG_280, CODEC_CODEC_IIR4_GAINA_EXT),
                                                            GET_BITFIELD(codec->REG_2C0, CODEC_CODEC_IIR4_GAINA_EXT_OUT_SYNC));
        }
    }
#endif
    return 0;
}

static int iir4_gainb_cfg_gain(void)
{
    int32_t iir4_gainb;
    uint32_t lock;

#if defined(ANC_FB_CHECK)
    iir4_gainb=(int32_t)((((float)fb_ramp_coef_l/512.0f)*((float)fb_ramp_gain_l/512.0f)*((float)fb_howling_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
#else
    iir4_gainb=(int32_t)((((float)fb_ramp_coef_l/512.0f)*((float)fb_ramp_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
#endif

    ANC_RAMP_LOG_D("%s:ramp ms cur=%d, ramp step cur=%d, ramp ms new=%d, ramp step new=%d...",__func__,
                                                                                       g_anc_linear_ramp_ms_curr,
                                                                                       g_anc_linear_ramp_step_curr,
                                                                                       g_anc_linear_ramp_ms_new,
                                                                                       g_anc_linear_ramp_step_new);
    ANC_RAMP_LOG_D("%s:iir4_gainb:%d",__func__,iir4_gainb);

#if defined(ANC_LINEAR_SMOOTH_CHECK)
    for (int i=0; i<ANC_LINEAR_SMOOTH_CHECK_TIMES; ++i) {
#endif
        lock = int_lock();
        codec->REG_25C &= ~CODEC_CODEC_IIR4_GAIN_EXT_UPDATE;
#if defined(ANC_IIR_LINEAR_SMOOTH_MODE)
        anc_ramp_update_step(ANC_IIR_ID_4);
#endif
        codec->REG_280 = SET_BITFIELD(codec->REG_280, CODEC_CODEC_IIR4_GAINA_EXT, ANC_GAIN_RAMP_ZERO);
        codec->REG_284 = SET_BITFIELD(codec->REG_284, CODEC_CODEC_IIR4_GAINB_EXT, iir4_gainb);
        hal_sys_timer_delay_us(1);
        // DRIVERS_TRACE(1,"%s:iir4_gainb:%d, REG_284=%d",__func__,iir4_gainb, GET_BITFIELD(codec->REG_284, CODEC_CODEC_IIR4_GAINB_EXT));
#if defined(ANC_LINEAR_SMOOTH_CHECK)
        int32_t iir4_gainb_sync = GET_BITFIELD(codec->REG_2C4, CODEC_CODEC_IIR4_GAINB_EXT_OUT_SYNC);
#endif
        codec->REG_25C |= CODEC_CODEC_IIR4_GAIN_EXT_UPDATE;
        int_unlock(lock);
#if defined(ANC_LINEAR_SMOOTH_CHECK)
        if (anc_iir4_linear_smooth_fade_flag == 1) {
            anc_iir4_linear_smooth_fade_flag = 0;
            break;
        } else {
            hal_sys_timer_delay_us(10);
            if (GET_BITFIELD(codec->REG_2C4, CODEC_CODEC_IIR4_GAINB_EXT_OUT_SYNC) != iir4_gainb_sync) {
                break;
            }
            DRIVERS_TRACE(1,"%s:iir4_gainb:%d, REG_284=%d, REG_2C4=%d, ANC FADE CHECK,  NEED SET AGAIN !!!",__func__,iir4_gainb,
                                                            GET_BITFIELD(codec->REG_284, CODEC_CODEC_IIR4_GAINB_EXT),
                                                            GET_BITFIELD(codec->REG_2C4, CODEC_CODEC_IIR4_GAINB_EXT_OUT_SYNC));
        }
    }
#endif

    return 0;
}

#if defined(AUDIO_ANC_FB_MC_HW)
static int iir5_gaina_cfg_gain(void)
{
    int32_t iir5_gaina;
    uint32_t lock;

    iir5_gaina=(int32_t)((((float)mc_ramp_coef_l/512.0f)*((float)mc_ramp_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);

    //DRIVERS_TRACE(1,"%s:iir5_gaina:%d",__func__,iir5_gaina);

#if defined(ANC_LINEAR_SMOOTH_CHECK)
    int32_t iir5_gaina_fade_check_cnt = 0;
    for (int i=0; i<ANC_LINEAR_SMOOTH_CHECK_TIMES; ++i) {
#endif
        lock = int_lock();
        codec->REG_25C &= ~CODEC_CODEC_IIR5_GAIN_EXT_UPDATE;
#if defined(ANC_IIR_LINEAR_SMOOTH_MODE)
        anc_ramp_update_step(ANC_IIR_ID_5);
#endif
        codec->REG_288 = SET_BITFIELD(codec->REG_288, CODEC_CODEC_IIR5_GAINA_EXT, iir5_gaina);
        codec->REG_28C = SET_BITFIELD(codec->REG_28C, CODEC_CODEC_IIR5_GAINB_EXT, ANC_GAIN_RAMP_ZERO);
        hal_sys_timer_delay_us(1);
#if defined(ANC_LINEAR_SMOOTH_CHECK)
        int32_t iir5_gaina_sync = GET_BITFIELD(codec->REG_2C8, CODEC_CODEC_IIR5_GAINA_EXT_OUT_SYNC);
#endif
        codec->REG_25C |= CODEC_CODEC_IIR5_GAIN_EXT_UPDATE;
        int_unlock(lock);
#if defined(ANC_LINEAR_SMOOTH_CHECK)
        if (anc_iir5_linear_smooth_fade_flag == 1) {
            hal_sys_timer_delay_us(10);
            if (GET_BITFIELD(codec->REG_2C8, CODEC_CODEC_IIR5_GAINA_EXT_OUT_SYNC) != iir5_gaina_sync) {
                anc_iir5_linear_smooth_fade_flag = 0;
                break;
            }
            ++iir5_gaina_fade_check_cnt;
            if (iir5_gaina_fade_check_cnt == ANC_LINEAR_SMOOTH_CHECK_TIMES) {
                anc_iir5_linear_smooth_fade_flag = 0;
                DRIVERS_TRACE(1,"%s: fade error !!!",__func__);
            }
            DRIVERS_TRACE(1,"%s:iir5_gaina:%d, REG_288=%d, REG_2C8=%d, ANC FADE CHECK,  NEED SET AGAIN !!!",__func__,iir5_gaina,
                                                            GET_BITFIELD(codec->REG_288, CODEC_CODEC_IIR5_GAINA_EXT),
                                                            GET_BITFIELD(codec->REG_2C8, CODEC_CODEC_IIR5_GAINA_EXT_OUT_SYNC));
        }
    }
#endif
    return 0;
}

static int iir5_gainb_cfg_gain(void)
{
    int32_t iir5_gainb;
    uint32_t lock;

    iir5_gainb=(int32_t)((((float)mc_ramp_coef_l/512.0f)*((float)mc_ramp_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);

    //DRIVERS_TRACE(1,"%s:iir5_gaina:%d",__func__,iir5_gaina);

#if defined(ANC_LINEAR_SMOOTH_CHECK)
    int32_t iir5_gainb_fade_check_cnt = 0;
    for (int i=0; i<ANC_LINEAR_SMOOTH_CHECK_TIMES; ++i) {
#endif
        lock = int_lock();
        codec->REG_25C &= ~CODEC_CODEC_IIR5_GAIN_EXT_UPDATE;
#if defined(ANC_IIR_LINEAR_SMOOTH_MODE)
        anc_ramp_update_step(ANC_IIR_ID_5);
#endif
        codec->REG_288 = SET_BITFIELD(codec->REG_288, CODEC_CODEC_IIR5_GAINA_EXT, ANC_GAIN_RAMP_ZERO);
        codec->REG_28C = SET_BITFIELD(codec->REG_28C, CODEC_CODEC_IIR5_GAINB_EXT, iir5_gainb);
        hal_sys_timer_delay_us(1);
#if defined(ANC_LINEAR_SMOOTH_CHECK)
        int32_t iir5_gainb_sync = GET_BITFIELD(codec->REG_2CC, CODEC_CODEC_IIR5_GAINB_EXT_OUT_SYNC);
#endif
        codec->REG_25C |= CODEC_CODEC_IIR5_GAIN_EXT_UPDATE;
        int_unlock(lock);
#if defined(ANC_LINEAR_SMOOTH_CHECK)
        if (anc_iir5_linear_smooth_fade_flag == 1) {
            hal_sys_timer_delay_us(10);
            if (GET_BITFIELD(codec->REG_2CC, CODEC_CODEC_IIR5_GAINB_EXT_OUT_SYNC) != iir5_gainb_sync) {
                anc_iir5_linear_smooth_fade_flag = 0;
                break;
            }
            ++iir5_gainb_fade_check_cnt;
            if (iir5_gainb_fade_check_cnt == ANC_LINEAR_SMOOTH_CHECK_TIMES) {
                anc_iir5_linear_smooth_fade_flag = 0;
                DRIVERS_TRACE(1,"%s: fade error !!!",__func__);
            }
            DRIVERS_TRACE(1,"%s:iir5_gainb:%d, REG_28C=%d, REG_2CC=%d, ANC FADE CHECK,  NEED SET AGAIN !!!",__func__,iir5_gainb,
                                                            GET_BITFIELD(codec->REG_28C, CODEC_CODEC_IIR5_GAINB_EXT),
                                                            GET_BITFIELD(codec->REG_2CC, CODEC_CODEC_IIR5_GAINB_EXT_OUT_SYNC));
        }
    }
#endif
    return 0;
}
#endif
#endif

#if ((AUD_SECTION_STRUCT_VERSION & 0x1) == 0)
#if defined(ANC_LIMITER_FF) || defined(ANC_LIMITER_FB) || defined(ANC_LIMITER_TT) || defined(ANC_LIMITER_MC)
static int anc_limiter_att_rls_coef_generate(anc_iir_coefs *limiter_coef, float lmiter_time)
{
    if((lmiter_time >= 0.1f ) && (lmiter_time <= 671088.64f )) {
        limiter_coef->coef_b[0] = 0;
        limiter_coef->coef_b[1] = 134217728/(uint32_t)(lmiter_time*100);
        limiter_coef->coef_b[2] = 0;

        limiter_coef->coef_a[0] = 134217728;
        limiter_coef->coef_a[1] = -(134217728 -limiter_coef->coef_b[1]);
        limiter_coef->coef_a[2] = 0;
    } else {
        DRIVERS_TRACE(1,"%s: [warning]Error lmiter_time:%d", __func__, (uint32_t)(lmiter_time*100));
    }

    return 0;
}
#endif
#endif

/**
 * @brief      Update coef and store gain,
 * fadein: enable: use anc_set_gain(), disable: anc_set_gain() to set gain to 0.
 *
 * @param[in]  cfg   coef and gain
 */
static int anc_set_cfg_internal(const struct_anc_cfg * cfg, enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err=ANC_NO_ERR;
    uint32_t lock;

    int32_t POSSIBLY_UNUSED threhold = 0;
    int16_t ff_offset_l = 0;
    int16_t ff_offset_r = 0;
    int16_t fb_offset_l = 0;
    int16_t fb_offset_r = 0;

#if defined(AUDIO_ANC_TT_HW)
    int16_t tt_offset_l = 0;
    int16_t tt_offset_r = 0;
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    const aud_item *ff_filtes_l;
    const aud_item *fb_filtes_l;
#if defined(AUDIO_ANC_TT_HW)
    const aud_item *tt_filtes_l;
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    const aud_item *mc_filtes_l;
#endif
#endif

    if (((anc_type&ANC_FEEDFORWARD) && (ff_open_flag == 0))
     || ((anc_type&ANC_FEEDBACK) && (fb_open_flag == 0))
#if defined(AUDIO_ANC_TT_HW)
     || ((anc_type&ANC_TALKTHRU) && (tt_open_flag == 0))
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
     || ((anc_type&ANC_MUSICCANCLE) && (mc_open_flag == 0))
#endif
     ) {
        DRIVERS_TRACE(1,"%s: anc_type = %d not opened", __func__, anc_type);
        return ANC_OTHER_ERR;
    }

    //DRIVERS_TRACE(1,"%s*******anc_type:%d, seting:a1:%d,a2:%d", __func__,anc_type,anc_gain_ff_iir_coefs0_l[0].a1,anc_gain_ff_iir_coefs0_l[0].a2);
    //DRIVERS_TRACE(1,"%s: iir0_coef_using:%d,iir1_coef_using:%d,anc_gain_delay:%d", __func__,iir0_coef_using,iir1_coef_using,anc_gain_delay);

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    ff_filtes_l = &(cfg->anc_cfg_ff_l);
    fb_filtes_l = &(cfg->anc_cfg_fb_l);

    ff_offset_l = ff_filtes_l->adc_gain_offset;
    fb_offset_l = fb_filtes_l->adc_gain_offset;

#if defined(AUDIO_ANC_TT_HW)
    tt_filtes_l = &(cfg->anc_cfg_tt_l);
    tt_offset_l = tt_filtes_l->adc_gain_offset;
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    mc_filtes_l = &(cfg->anc_cfg_mc_l);

#endif

#endif

    //DRIVERS_TRACE(1,"ff_l:%d,ff_r:%d,fb_l:%d,fb_r:%d",ff_filtes_l->total_gain,ff_filtes_r->total_gain,fb_filtes_l->total_gain,fb_filtes_r->total_gain);
#if ANC_PROD_TEST
    if (anc_type & ANC_FEEDFORWARD){
        int time_int = 0;
        int time_Fra = 0;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        DRIVERS_TRACE(1,"ff gain: %d, counter: %d iir_bypass_flag: %d ",ff_filtes_l->total_gain, ff_filtes_l->iir_counter,ff_filtes_l->iir_bypass_flag);
        DRIVERS_TRACE(1,"ff dac_gain_offset %d, adc_gain_offset %d",ff_filtes_l->dac_gain_offset, ff_filtes_l->adc_gain_offset);

#if ((AUD_SECTION_STRUCT_VERSION & 0x1) == 0)
        DRIVERS_TRACE(1,"limiter_flag:%d",ff_filtes_l->limiter_flag);

        time_int = (int)(ff_filtes_l->limiter_th);
        time_Fra = (int)(ff_filtes_l->limiter_th*1000)-((int)ff_filtes_l->limiter_th)*1000;

        DRIVERS_TRACE(1,"limiter_th(dB):%d.%d",time_int,time_Fra);

        time_int = (int)(ff_filtes_l->limiter_att_time);
        time_Fra = (int)(ff_filtes_l->limiter_att_time*1000)-((int)ff_filtes_l->limiter_att_time)*1000;

        DRIVERS_TRACE(1,"limiter_att_time(ms):%d.%d",time_int,time_Fra);

        time_int=(int)(ff_filtes_l->limiter_rls_time);
        time_Fra=(int)(ff_filtes_l->limiter_rls_time*1000)-((int)ff_filtes_l->limiter_rls_time)*1000;

        DRIVERS_TRACE(1,"limiter_rls_time(ms):%d.%d",time_int,time_Fra);
#endif

        for (int j = 0; j <IIR_FF_COUNTER; j++) {
            //DRIVERS_TRACE(1,"iir coef ff l %10d, %10d, %10d, %10d, %10d, %10d",
            DRIVERS_TRACE(1,"iir coef ff l 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x",\
                    ff_filtes_l->iir_coef[j].coef_b[0], \
                    ff_filtes_l->iir_coef[j].coef_b[1], \
                    ff_filtes_l->iir_coef[j].coef_b[2], \
                    ff_filtes_l->iir_coef[j].coef_a[0], \
                    ff_filtes_l->iir_coef[j].coef_a[1], \
                    ff_filtes_l->iir_coef[j].coef_a[2]);
        }
#endif
    }
    if (anc_type & ANC_FEEDBACK) {
        int time_int = 0;
        int time_Fra = 0;

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        DRIVERS_TRACE(1,"fb gain: %d, counter: %d iir_bypass_flag: %d ",fb_filtes_l->total_gain, fb_filtes_l->iir_counter,fb_filtes_l->iir_bypass_flag);
        DRIVERS_TRACE(1,"fb dac_gain_offset %d, adc_gain_offset %d",fb_filtes_l->dac_gain_offset, fb_filtes_l->adc_gain_offset);

#if ((AUD_SECTION_STRUCT_VERSION & 0x1) == 0)
        DRIVERS_TRACE(1,"limiter_flag:%d",fb_filtes_l->limiter_flag);

        time_int = (int)(fb_filtes_l->limiter_th);
        time_Fra = (int)(fb_filtes_l->limiter_th*1000)-((int)fb_filtes_l->limiter_th)*1000;

        DRIVERS_TRACE(1,"limiter_th(dB):%d.%d",time_int,time_Fra);

        time_int = (int)(fb_filtes_l->limiter_att_time);
        time_Fra = (int)(fb_filtes_l->limiter_att_time*1000)-((int)fb_filtes_l->limiter_att_time)*1000;

        DRIVERS_TRACE(1,"limiter_att_time(ms):%d.%d",time_int,time_Fra);

        time_int=(int)(fb_filtes_l->limiter_rls_time);
        time_Fra=(int)(fb_filtes_l->limiter_rls_time*1000)-((int)fb_filtes_l->limiter_rls_time)*1000;

        DRIVERS_TRACE(1,"limiter_rls_time(ms):%d.%d",time_int,time_Fra);
#endif

        for (int j = 0; j <IIR_FB_COUNTER; j++) {
            //DRIVERS_TRACE(1,"iir coef fb l %10d, %10d, %10d, %10d, %10d, %10d",
            DRIVERS_TRACE(1,"iir coef fb l 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x", \
                    fb_filtes_l->iir_coef[j].coef_b[0], \
                    fb_filtes_l->iir_coef[j].coef_b[1], \
                    fb_filtes_l->iir_coef[j].coef_b[2], \
                    fb_filtes_l->iir_coef[j].coef_a[0], \
                    fb_filtes_l->iir_coef[j].coef_a[1], \
                    fb_filtes_l->iir_coef[j].coef_a[2]);
        }
#endif
    }
#if defined(AUDIO_ANC_TT_HW)
    if (anc_type & ANC_TALKTHRU) {
        int time_int = 0;
        int time_Fra = 0;

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        DRIVERS_TRACE(1,"tt gain: %d, counter: %d iir_bypass_flag: %d ",tt_filtes_l->total_gain, tt_filtes_l->iir_counter,tt_filtes_l->iir_bypass_flag);
        DRIVERS_TRACE(1,"tt dac_gain_offset %d, adc_gain_offset %d",tt_filtes_l->dac_gain_offset, tt_filtes_l->adc_gain_offset);

#if ((AUD_SECTION_STRUCT_VERSION & 0x1) == 0)
        DRIVERS_TRACE(1,"limiter_flag:%d",tt_filtes_l->limiter_flag);

        time_int = (int)(tt_filtes_l->limiter_th);
        time_Fra = (int)(tt_filtes_l->limiter_th*1000)-((int)tt_filtes_l->limiter_th)*1000;

        DRIVERS_TRACE(1,"limiter_th(dB):%d.%d",time_int,time_Fra);

        time_int = (int)(tt_filtes_l->limiter_att_time);
        time_Fra = (int)(tt_filtes_l->limiter_att_time*1000)-((int)tt_filtes_l->limiter_att_time)*1000;

        DRIVERS_TRACE(1,"limiter_att_time(ms):%d.%d",time_int,time_Fra);

        time_int=(int)(tt_filtes_l->limiter_rls_time);
        time_Fra=(int)(tt_filtes_l->limiter_rls_time*1000)-((int)tt_filtes_l->limiter_rls_time)*1000;

        DRIVERS_TRACE(1,"limiter_rls_time(ms):%d.%d",time_int,time_Fra);
#endif
        for (int j = 0; j <IIR_TT_COUNTER; j++) {
//            DRIVERS_TRACE(1,"iir coef tt l %10d, %10d, %10d, %10d, %10d, %10d",
            DRIVERS_TRACE(1,"iir coef tt l 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x", \
                    tt_filtes_l->iir_coef[j].coef_b[0], \
                    tt_filtes_l->iir_coef[j].coef_b[1], \
                    tt_filtes_l->iir_coef[j].coef_b[2], \
                    tt_filtes_l->iir_coef[j].coef_a[0], \
                    tt_filtes_l->iir_coef[j].coef_a[1], \
                    tt_filtes_l->iir_coef[j].coef_a[2]);
        }
#endif
    }
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    if (anc_type & ANC_MUSICCANCLE) {
        int time_int = 0;
        int time_Fra = 0;

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        DRIVERS_TRACE(1,"mc gain: %d, counter: %d iir_bypass_flag: %d ",mc_filtes_l->total_gain, mc_filtes_l->iir_counter,mc_filtes_l->iir_bypass_flag);
        DRIVERS_TRACE(1,"mc dac_gain_offset %d, adc_gain_offset %d",mc_filtes_l->dac_gain_offset, mc_filtes_l->adc_gain_offset);

#if ((AUD_SECTION_STRUCT_VERSION & 0x1) == 0)
        DRIVERS_TRACE(1,"limiter_flag:%d",mc_filtes_l->limiter_flag);

        time_int = (int)(mc_filtes_l->limiter_th);
        time_Fra = (int)(mc_filtes_l->limiter_th*1000)-((int)mc_filtes_l->limiter_th)*1000;

        DRIVERS_TRACE(1,"limiter_th(dB):%d.%d",time_int,time_Fra);

        time_int = (int)(mc_filtes_l->limiter_att_time);
        time_Fra = (int)(mc_filtes_l->limiter_att_time*1000)-((int)mc_filtes_l->limiter_att_time)*1000;

        DRIVERS_TRACE(1,"limiter_att_time(ms):%d.%d",time_int,time_Fra);

        time_int=(int)(mc_filtes_l->limiter_rls_time);
        time_Fra=(int)(mc_filtes_l->limiter_rls_time*1000)-((int)mc_filtes_l->limiter_rls_time)*1000;

        DRIVERS_TRACE(1,"limiter_rls_time(ms):%d.%d",time_int,time_Fra);
#endif
        for(int j = 0; j <IIR_MC_COUNTER; j++) {
//            DRIVERS_TRACE(1,"iir coef mc l %10d, %10d, %10d, %10d, %10d, %10d",
            DRIVERS_TRACE(1,"iir coef mc l 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x",\
                    mc_filtes_l->iir_coef[j].coef_b[0], \
                    mc_filtes_l->iir_coef[j].coef_b[1], \
                    mc_filtes_l->iir_coef[j].coef_b[2], \
                    mc_filtes_l->iir_coef[j].coef_a[0], \
                    mc_filtes_l->iir_coef[j].coef_a[1], \
                    mc_filtes_l->iir_coef[j].coef_a[2]);
        }
#endif
    }
#endif
#endif


// updata AD analog gain.
    if (anc_type & ANC_FEEDFORWARD){
        analog_aud_apply_anc_adc_gain_offset(ANC_FEEDFORWARD, ff_offset_l, ff_offset_r);
    }

    if (anc_type & ANC_FEEDBACK){
        analog_aud_apply_anc_adc_gain_offset(ANC_FEEDBACK, fb_offset_l, fb_offset_r);
    }

#if defined(AUDIO_ANC_TT_HW)
    if (anc_type & ANC_TALKTHRU){
        analog_aud_apply_anc_adc_gain_offset(ANC_TALKTHRU, tt_offset_l, tt_offset_r);
    }
#endif


// updata limiter parameters.
#if ((AUD_SECTION_STRUCT_VERSION & 0x1) == 0)

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
#if defined(ANC_LIMITER_FF)
        if (ff_filtes_l->limiter_flag != 0) {

            if(ff_filtes_l->limiter_th>0) {
                threhold = 0x7fffff;
            } else {
                threhold = (int32_t)(db_to_float(ff_filtes_l->limiter_th) * 0x7fffff);
            }

            //limiter threshold, 0dB: 0x7fffff
            codec->REG_25C &= ~CODEC_CODEC_IIR0_LMT_TH_UPDATE;
            codec->REG_31C = SET_BITFIELD(codec->REG_31C, CODEC_CODEC_IIR0_LMT_TH, threhold);
            hal_sys_timer_delay_us(1);
            codec->REG_25C |= CODEC_CODEC_IIR0_LMT_TH_UPDATE;

            anc_limiter_att_rls_coef_generate(&ff_limiter_att_l_old,ff_filtes_l->limiter_att_time);
            anc_limiter_att_rls_coef_generate(&ff_limiter_rls_l_old,ff_filtes_l->limiter_rls_time);

            //Enable the FF limiter;
            codec->REG_248 &= ~CODEC_CODEC_IIR0_LMT_BYPASS;
        } else {
            //Disable the FF limiter;
            codec->REG_248 |= CODEC_CODEC_IIR0_LMT_BYPASS;
        }

#endif
#if defined(ANC_LIMITER_TT) && defined(AUDIO_ANC_TT_HW)
        if (tt_filtes_l->limiter_flag != 0) {
            if(tt_filtes_l->limiter_th>0) {
                threhold = 0x7fffff;
            } else {
                threhold = (int32_t)(db_to_float(tt_filtes_l->limiter_th) * 0x7fffff);
            }

            //limiter threshold, 0dB: 0x7fffff
            codec->REG_25C &= ~CODEC_CODEC_IIR1_LMT_TH_UPDATE;
            codec->REG_320 = SET_BITFIELD(codec->REG_320, CODEC_CODEC_IIR1_LMT_TH, threhold);
            hal_sys_timer_delay_us(1);
            codec->REG_25C |= CODEC_CODEC_IIR1_LMT_TH_UPDATE;

            anc_limiter_att_rls_coef_generate(&tt_limiter_att_l_old,tt_filtes_l->limiter_att_time);
            anc_limiter_att_rls_coef_generate(&tt_limiter_rls_l_old,tt_filtes_l->limiter_rls_time);

            //Enable the TT limiter;
            codec->REG_24C &= ~CODEC_CODEC_IIR1_LMT_BYPASS;
        } else {
            //Disable the TT limiter;
            codec->REG_24C |= CODEC_CODEC_IIR1_LMT_BYPASS;
        }
#endif
#if defined(ANC_LIMITER_FB)
        if (fb_filtes_l->limiter_flag != 0) {
            if(fb_filtes_l->limiter_th>0) {
                threhold = 0x7fffff;
            } else {
                threhold = (int32_t)(db_to_float(fb_filtes_l->limiter_th) * 0x7fffff);
            }

            //limiter threshold, 0dB: 0x7fffff
            codec->REG_25C &= ~CODEC_CODEC_IIR4_LMT_TH_UPDATE;
            codec->REG_32C = SET_BITFIELD(codec->REG_32C, CODEC_CODEC_IIR4_LMT_TH, threhold);
            hal_sys_timer_delay_us(1);
            codec->REG_25C |= CODEC_CODEC_IIR4_LMT_TH_UPDATE;

            anc_limiter_att_rls_coef_generate(&fb_limiter_att_l_old,fb_filtes_l->limiter_att_time);
            anc_limiter_att_rls_coef_generate(&fb_limiter_rls_l_old,fb_filtes_l->limiter_rls_time);

            //Enable the FB limiter;
            codec->REG_680 &= ~CODEC_CODEC_IIR4_LMT_BYPASS;
        } else {
            //Disable the FB limiter;
            codec->REG_680 |= CODEC_CODEC_IIR4_LMT_BYPASS;
        }

#endif
#if defined(ANC_LIMITER_MC) && defined(AUDIO_ANC_FB_MC_HW)
        if (mc_filtes_l->limiter_flag != 0) {
            if(mc_filtes_l->limiter_th>0) {
                threhold = 0x7fffff;
            } else {
                threhold = (int32_t)(db_to_float(mc_filtes_l->limiter_th) * 0x7fffff);
            }

            //limiter threshold, 0dB: 0x7fffff
            codec->REG_25C &= ~CODEC_CODEC_IIR5_LMT_TH_UPDATE;
            codec->REG_330 = SET_BITFIELD(codec->REG_330, CODEC_CODEC_IIR5_LMT_TH, threhold);
            hal_sys_timer_delay_us(1);
            codec->REG_25C |= CODEC_CODEC_IIR5_LMT_TH_UPDATE;

            anc_limiter_att_rls_coef_generate(&mc_limiter_att_l_old,mc_filtes_l->limiter_att_time);
            anc_limiter_att_rls_coef_generate(&mc_limiter_rls_l_old,mc_filtes_l->limiter_rls_time);

            //Enable the MC limiter;
            codec->REG_684 &= ~CODEC_CODEC_IIR5_LMT_BYPASS;
        } else {
            //Disable the MC limiter;
            codec->REG_684 |= CODEC_CODEC_IIR5_LMT_BYPASS;
        }

#endif
    }
#endif
#endif

    /***************************ANC_FEEDFORWARD**********************/
    if (anc_type & ANC_FEEDFORWARD) {
        DRIVERS_TRACE(1,"%s: ANC_FEEDFORWARD",__func__);
        // DRIVERS_TRACE(1,"iir0_iira_stop_status_sync:%d,iir0_iirb_stop_status_sync:%d",anc_iir0_control->codec_iir0_iira_stop_status_sync,anc_iir0_control->codec_iir0_iirb_stop_status_sync);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
        {
            iir_filter_para_copy(&ff_filtes_l_old, ff_filtes_l, IIR_FF_COUNTER);

            if (ff_filtes_l_old.iir_bypass_flag == 1){
                max_ff_gain_l = ff_filtes_l_old.total_gain;
            } else {
                max_ff_gain_l = (1<<GAIN_Q);
            }

            if (iir0_coef_using == 0) {
                trace_counter = 0;
                while (GET_BITFIELD(codec->REG_2A4,CODEC_CODEC_IIR0_GAINB_EXT_OUT_SYNC) > ANC_GAIN_RAMP_THRESHLD) {
                    if((trace_counter % TRACE_COUNTER) == 0){
                        DRIVERS_TRACE(1,"%s: CODEC_CODEC_IIR0_GAINB_EXT_OUT_SYNC:%d",__func__,
                                GET_BITFIELD(codec->REG_2A4,CODEC_CODEC_IIR0_GAINB_EXT_OUT_SYNC));
                    }
                    trace_counter++;
                };

                codec->REG_248 &= ~CODEC_CODEC_IIR0_IIRB_ENABLE;

                if (ff_filtes_l_old.iir_bypass_flag == 0) {
                    codec->REG_248 &= ~CODEC_CODEC_IIR0_BYPASS;
                } else {
                    codec->REG_248 |= CODEC_CODEC_IIR0_BYPASS;
                }

                codec->REG_248 = SET_BITFIELD(codec->REG_248, CODEC_CODEC_IIR0_COUNT, ff_filtes_l_old.iir_counter);

                iir_filter_coef_copy(anc_ff_iir_coefs1_l, &ff_filtes_l_old);
#if defined(ANC_LIMITER_FF)
                iir_coef_copy(&(anc_limiter_ff_iir_coefs1_l[0]),&ff_limiter_att_l_old);
                iir_coef_copy(&(anc_limiter_ff_iir_coefs1_l[1]),&ff_limiter_rls_l_old);
#endif
                codec->REG_264 = SET_BITFIELD(codec->REG_264, CODEC_CODEC_IIR0_GAINB_EXT, ANC_GAIN_RAMP_GAIN_PRE);
                codec->REG_248 |= CODEC_CODEC_IIR0_IIRB_ENABLE;

                iir0_coef_using = 1;
            } else {
                trace_counter = 0;
                while (GET_BITFIELD(codec->REG_2A0,CODEC_CODEC_IIR0_GAINA_EXT_OUT_SYNC) > ANC_GAIN_RAMP_THRESHLD) {
                    if ((trace_counter % TRACE_COUNTER) == 0){
                        DRIVERS_TRACE(1,"%s: CODEC_CODEC_IIR0_GAINA_EXT_OUT_SYNC:%d",__func__,
                                GET_BITFIELD(codec->REG_2A0,CODEC_CODEC_IIR0_GAINA_EXT_OUT_SYNC));
                    }
                    trace_counter++;
                };

                codec->REG_248 &= ~CODEC_CODEC_IIR0_IIRA_ENABLE;

                if (ff_filtes_l_old.iir_bypass_flag == 0) {
                    codec->REG_248 &= ~CODEC_CODEC_IIR0_BYPASS;
                } else {
                    codec->REG_248 |= CODEC_CODEC_IIR0_BYPASS;
                }

                codec->REG_248 = SET_BITFIELD(codec->REG_248, CODEC_CODEC_IIR0_COUNT, ff_filtes_l_old.iir_counter);

                iir_filter_coef_copy(anc_ff_iir_coefs0_l, &ff_filtes_l_old);
#if defined(ANC_LIMITER_FF)
                iir_coef_copy(&(anc_limiter_ff_iir_coefs0_l[0]),&ff_limiter_att_l_old);
                iir_coef_copy(&(anc_limiter_ff_iir_coefs0_l[1]),&ff_limiter_rls_l_old);
#endif
                codec->REG_260 = SET_BITFIELD(codec->REG_260, CODEC_CODEC_IIR0_GAINA_EXT, ANC_GAIN_RAMP_GAIN_PRE);
                codec->REG_248 |= CODEC_CODEC_IIR0_IIRA_ENABLE;

                iir0_coef_using = 0;
            }

            ff_ramp_coef_l = max_ff_gain_l;

            lock = int_lock();
            codec->REG_25C &= ~CODEC_CODEC_IIR0_GAIN_EXT_UPDATE;
            hal_sys_timer_delay_us(1);
            codec->REG_25C |= CODEC_CODEC_IIR0_GAIN_EXT_UPDATE;
            int_unlock(lock);

            max_ff_gain_l = (1<<GAIN_Q);
        }
#endif
        hwtimer_stop(anc_ff_set_gain_timer);
        hwtimer_start(anc_ff_set_gain_timer, anc_switching_gain_delay_ff);

        return err;
    }

    /***************************ANC_TALKTHRU**********************/
#if defined(AUDIO_ANC_TT_HW)
    //talk thr settings
    if (anc_type & ANC_TALKTHRU)
    {
        DRIVERS_TRACE(1,"%s: ANC_TALKTHRU",__func__);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            iir_filter_para_copy(&tt_filtes_l_old, tt_filtes_l, IIR_TT_COUNTER);

            if (tt_filtes_l_old.iir_bypass_flag == 1){
                max_tt_gain_l=tt_filtes_l_old.total_gain;
            } else {
                max_tt_gain_l=(1<<GAIN_Q);
            }

            if (iir1_coef_using == 0) {
                trace_counter=0;

                while (GET_BITFIELD(codec->REG_2AC,CODEC_CODEC_IIR1_GAINB_EXT_OUT_SYNC) > ANC_GAIN_RAMP_THRESHLD) {
                    if ((trace_counter % TRACE_COUNTER) == 0) {
                        DRIVERS_TRACE(1,"%s: CODEC_CODEC_IIR1_GAINB_EXT_OUT_SYNC:%d",__func__,
                                GET_BITFIELD(codec->REG_2AC, CODEC_CODEC_IIR1_GAINB_EXT_OUT_SYNC));
                    }
                    trace_counter++;
                };

                codec->REG_24C &= ~CODEC_CODEC_IIR1_IIRB_ENABLE;

                if (tt_filtes_l_old.iir_bypass_flag == 0) {
                    codec->REG_24C &= ~CODEC_CODEC_IIR1_BYPASS;
                } else {
                    codec->REG_24C |= CODEC_CODEC_IIR1_BYPASS;
                }

                codec->REG_24C = SET_BITFIELD(codec->REG_24C, CODEC_CODEC_IIR1_COUNT, tt_filtes_l_old.iir_counter);

                iir_filter_coef_copy(anc_tt_iir_coefs1_l, &tt_filtes_l_old);
#if defined(ANC_LIMITER_TT)
                iir_coef_copy(&(anc_limiter_tt_iir_coefs1_l[0]),&tt_limiter_att_l_old);
                iir_coef_copy(&(anc_limiter_tt_iir_coefs1_l[1]),&tt_limiter_rls_l_old);
#endif
                codec->REG_26C = SET_BITFIELD(codec->REG_26C, CODEC_CODEC_IIR1_GAINB_EXT, ANC_GAIN_RAMP_GAIN_PRE);
                codec->REG_24C |= CODEC_CODEC_IIR1_IIRB_ENABLE;
                iir1_coef_using = 1;
            } else {
                trace_counter = 0;
                while (GET_BITFIELD(codec->REG_2A8,CODEC_CODEC_IIR1_GAINA_EXT_OUT_SYNC) > ANC_GAIN_RAMP_THRESHLD) {
                    if ((trace_counter % TRACE_COUNTER) == 0) {
                        DRIVERS_TRACE(1,"%s: CODEC_CODEC_IIR1_GAINA_EXT_OUT_SYNC:%d",__func__,
                                GET_BITFIELD(codec->REG_2A8, CODEC_CODEC_IIR1_GAINA_EXT_OUT_SYNC));
                    }
                    trace_counter++;
                };

                codec->REG_24C &= ~CODEC_CODEC_IIR1_IIRA_ENABLE;

                if (tt_filtes_l_old.iir_bypass_flag == 0) {
                    codec->REG_24C &= ~CODEC_CODEC_IIR1_BYPASS;
                } else {
                    codec->REG_24C |= CODEC_CODEC_IIR1_BYPASS;
                }

                codec->REG_24C = SET_BITFIELD(codec->REG_24C, CODEC_CODEC_IIR1_COUNT, tt_filtes_l_old.iir_counter);

                iir_filter_coef_copy(anc_tt_iir_coefs0_l, &tt_filtes_l_old);
#if defined(ANC_LIMITER_TT)
                iir_coef_copy(&(anc_limiter_tt_iir_coefs0_l[0]),&tt_limiter_att_l_old);
                iir_coef_copy(&(anc_limiter_tt_iir_coefs0_l[1]),&tt_limiter_rls_l_old);
#endif
                codec->REG_268 = SET_BITFIELD(codec->REG_268, CODEC_CODEC_IIR1_GAINA_EXT, ANC_GAIN_RAMP_GAIN_PRE);
                codec->REG_24C |= CODEC_CODEC_IIR1_IIRA_ENABLE;
                iir1_coef_using = 0;
            }
            tt_ramp_coef_l = max_tt_gain_l;

            lock = int_lock();
            codec->REG_25C &= ~CODEC_CODEC_IIR1_GAIN_EXT_UPDATE;
            hal_sys_timer_delay_us(1);
            codec->REG_25C |= CODEC_CODEC_IIR1_GAIN_EXT_UPDATE;
            int_unlock(lock);

            max_tt_gain_l = (1<<GAIN_Q);
        }
#endif
        hwtimer_stop(anc_tt_set_gain_timer);
        hwtimer_start(anc_tt_set_gain_timer, anc_switching_gain_delay_tt);

        return err;
    }
#endif

    /***************************ANC_FEEDBACK**********************/
    if (anc_type & ANC_FEEDBACK)
    {
        DRIVERS_TRACE(1,"%s: ANC_FEEDBACK",__func__);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            iir_filter_para_copy(&fb_filtes_l_old, fb_filtes_l, IIR_FB_COUNTER);

            if (fb_filtes_l_old.iir_bypass_flag == 1) {
                max_fb_gain_l = fb_filtes_l_old.total_gain;
            } else {
                max_fb_gain_l = (1<<GAIN_Q);
            }

            if (iir4_coef_using == 0) {
                trace_counter = 0;
                while (GET_BITFIELD(codec->REG_2C4,CODEC_CODEC_IIR4_GAINB_EXT_OUT_SYNC) > ANC_GAIN_RAMP_THRESHLD) {
                    if ((trace_counter % TRACE_COUNTER) == 0) {
                        DRIVERS_TRACE(1,"%s: CODEC_CODEC_IIR4_GAINB_EXT_OUT_SYNC:%d",__func__,
                                GET_BITFIELD(codec->REG_2C4, CODEC_CODEC_IIR4_GAINB_EXT_OUT_SYNC));
                    }
                    trace_counter++;
                };

                codec->REG_680 &= ~CODEC_CODEC_IIR4_IIRB_ENABLE;

                if (fb_filtes_l_old.iir_bypass_flag == 0) {
                    codec->REG_680 &= ~CODEC_CODEC_IIR4_BYPASS;
                } else {
                    codec->REG_680 |= CODEC_CODEC_IIR4_BYPASS;
                }

                codec->REG_680 = SET_BITFIELD(codec->REG_680, CODEC_CODEC_IIR4_COUNT, fb_filtes_l_old.iir_counter);

                iir_filter_coef_copy(anc_fb_iir_coefs1_l, &fb_filtes_l_old);
#if defined(ANC_LIMITER_FB)
                iir_coef_copy(&(anc_limiter_fb_iir_coefs1_l[0]),&fb_limiter_att_l_old);
                iir_coef_copy(&(anc_limiter_fb_iir_coefs1_l[1]),&fb_limiter_rls_l_old);
#endif
                codec->REG_284 = SET_BITFIELD(codec->REG_284, CODEC_CODEC_IIR4_GAINB_EXT, ANC_GAIN_RAMP_GAIN_PRE);
                codec->REG_680 |= CODEC_CODEC_IIR4_IIRB_ENABLE;
                iir4_coef_using = 1;
            } else {
                trace_counter = 0;
                while (GET_BITFIELD(codec->REG_2C0,CODEC_CODEC_IIR4_GAINA_EXT_OUT_SYNC) > ANC_GAIN_RAMP_THRESHLD){
                    if ((trace_counter % TRACE_COUNTER)==0) {
                        DRIVERS_TRACE(1,"%s: CODEC_CODEC_IIR4_GAINA_EXT_OUT_SYNC:%d",__func__,
                                GET_BITFIELD(codec->REG_2C0, CODEC_CODEC_IIR4_GAINA_EXT_OUT_SYNC));
                    }
                    trace_counter++;
                };

                codec->REG_680 &= ~CODEC_CODEC_IIR4_IIRA_ENABLE;

                if (fb_filtes_l_old.iir_bypass_flag == 0) {
                    codec->REG_680 &= ~CODEC_CODEC_IIR4_BYPASS;
                } else {
                    codec->REG_680 |= CODEC_CODEC_IIR4_BYPASS;
                }

                codec->REG_680 = SET_BITFIELD(codec->REG_680, CODEC_CODEC_IIR4_COUNT, fb_filtes_l_old.iir_counter);

                iir_filter_coef_copy(anc_fb_iir_coefs0_l, &fb_filtes_l_old);
#if defined(ANC_LIMITER_FB)
                iir_coef_copy(&(anc_limiter_fb_iir_coefs0_l[0]),&fb_limiter_att_l_old);
                iir_coef_copy(&(anc_limiter_fb_iir_coefs0_l[1]),&fb_limiter_rls_l_old);
#endif
                codec->REG_280 = SET_BITFIELD(codec->REG_280, CODEC_CODEC_IIR4_GAINA_EXT, ANC_GAIN_RAMP_GAIN_PRE);
                codec->REG_680 |= CODEC_CODEC_IIR4_IIRA_ENABLE;
                iir4_coef_using = 0;
            }

            fb_ramp_coef_l = max_fb_gain_l;

            lock = int_lock();
            codec->REG_25C &= ~CODEC_CODEC_IIR4_GAIN_EXT_UPDATE;
            hal_sys_timer_delay_us(1);
            codec->REG_25C |= CODEC_CODEC_IIR4_GAIN_EXT_UPDATE;
            int_unlock(lock);

            max_fb_gain_l = (1<<GAIN_Q);
        }
#endif

        hwtimer_stop(anc_fb_set_gain_timer);
        hwtimer_start(anc_fb_set_gain_timer, anc_switching_gain_delay_fb);

        return err;
    }


    /***************************ANC_MUSICCANCLE**********************/
#if defined(AUDIO_ANC_FB_MC_HW)
    //music cancel ANC settings
    if (anc_type & ANC_MUSICCANCLE)
    {
        DRIVERS_TRACE(1,"%s: ANC_MUSICCANCLE",__func__);

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
        {
            iir_filter_para_copy(&mc_filtes_l_old, mc_filtes_l, IIR_MC_COUNTER);

            if (mc_filtes_l_old.iir_bypass_flag == 1) {
                max_mc_gain_l = mc_filtes_l_old.total_gain;
            } else {
                max_mc_gain_l = (1<<GAIN_Q);
            }

            if (iir5_coef_using == 0) {
                trace_counter = 0;
                while (GET_BITFIELD(codec->REG_2CC,CODEC_CODEC_IIR5_GAINB_EXT_OUT_SYNC) > ANC_GAIN_RAMP_THRESHLD) {
                    if ((trace_counter % TRACE_COUNTER) == 0) {
                        DRIVERS_TRACE(1,"%s: CODEC_CODEC_IIR5_GAINB_EXT_OUT_SYNC:%d",__func__,
                                GET_BITFIELD(codec->REG_2CC, CODEC_CODEC_IIR5_GAINB_EXT_OUT_SYNC));
                    }
                    trace_counter++;
                };

                codec->REG_684 &= ~CODEC_CODEC_IIR5_IIRB_ENABLE;

                if (mc_filtes_l_old.iir_bypass_flag == 0) {
                    codec->REG_684 &= ~CODEC_CODEC_IIR5_BYPASS;
                } else {
                    codec->REG_684 |= CODEC_CODEC_IIR5_BYPASS;
                }

                codec->REG_684 = SET_BITFIELD(codec->REG_684, CODEC_CODEC_IIR5_COUNT, mc_filtes_l_old.iir_counter);

                iir_filter_coef_copy(anc_mc_iir_coefs1_l, &mc_filtes_l_old);
#if defined(ANC_LIMITER_MC)
                iir_coef_copy(&(anc_limiter_mc_iir_coefs1_l[0]),&mc_limiter_att_l_old);
                iir_coef_copy(&(anc_limiter_mc_iir_coefs1_l[1]),&mc_limiter_rls_l_old);
#endif
                codec->REG_28C = SET_BITFIELD(codec->REG_28C, CODEC_CODEC_IIR5_GAINB_EXT, ANC_GAIN_RAMP_GAIN_PRE);
                codec->REG_684 |= CODEC_CODEC_IIR5_IIRB_ENABLE;
                iir5_coef_using = 1;
            } else {
                trace_counter = 0;
                while (GET_BITFIELD(codec->REG_2C8,CODEC_CODEC_IIR5_GAINA_EXT_OUT_SYNC) > ANC_GAIN_RAMP_THRESHLD) {
                    if (trace_counter % TRACE_COUNTER==0) {

                        DRIVERS_TRACE(1,"%s: CODEC_CODEC_IIR5_GAINA_EXT_OUT_SYNC:/%d",__func__,
                                GET_BITFIELD(codec->REG_2C8, CODEC_CODEC_IIR5_GAINA_EXT_OUT_SYNC));
                    }
                    trace_counter++;
                };

                codec->REG_684 &= ~CODEC_CODEC_IIR5_IIRA_ENABLE;

                if (mc_filtes_l_old.iir_bypass_flag == 0) {
                    codec->REG_684 &= ~CODEC_CODEC_IIR5_BYPASS;
                } else {
                    codec->REG_684 |= CODEC_CODEC_IIR5_BYPASS;
                }

                codec->REG_684 = SET_BITFIELD(codec->REG_684, CODEC_CODEC_IIR5_COUNT, mc_filtes_l_old.iir_counter);

                iir_filter_coef_copy(anc_mc_iir_coefs0_l, &mc_filtes_l_old);
#if defined(ANC_LIMITER_MC)
                iir_coef_copy(&(anc_limiter_mc_iir_coefs0_l[0]),&mc_limiter_att_l_old);
                iir_coef_copy(&(anc_limiter_mc_iir_coefs0_l[1]),&mc_limiter_rls_l_old);
#endif
                codec->REG_288 = SET_BITFIELD(codec->REG_288, CODEC_CODEC_IIR5_GAINA_EXT, ANC_GAIN_RAMP_GAIN_PRE);
                codec->REG_684 |= CODEC_CODEC_IIR5_IIRA_ENABLE;
                iir5_coef_using = 0;
            }
            mc_ramp_coef_l = max_mc_gain_l;

            lock = int_lock();
            codec->REG_25C &= ~CODEC_CODEC_IIR5_GAIN_EXT_UPDATE;
            hal_sys_timer_delay_us(1);
            codec->REG_25C |= CODEC_CODEC_IIR5_GAIN_EXT_UPDATE;
            int_unlock(lock);

            max_mc_gain_l = (1<<GAIN_Q);
        }
#endif

        hwtimer_stop(anc_mc_set_gain_timer);
        hwtimer_start(anc_mc_set_gain_timer, anc_switching_gain_delay_mc);

        return err;
    }
#endif

    return err;
}

int32_t anc_set_calib_gain(enum ANC_TYPE_T type, int32_t gain_l, int32_t gain_r)
{
    DRIVERS_TRACE(0, "[%s] type: %d, gain_l: %d, gain_r: %d", __func__, type, gain_l, gain_r);
    // +/-6 dB
    if ((gain_l < 512 / 2) || (gain_l > 512 * 2)) {
        gain_l = 512;
        DRIVERS_TRACE(0, "[%s] WARNING: Invalid gain_l: %d. Use default value", __func__, gain_l);
    }

    if ((gain_r < 512 / 2) || (gain_r > 512 * 2)) {
        gain_r = 512;
        DRIVERS_TRACE(0, "[%s] WARNING: Invalid gain_r: %d. Use default value", __func__, gain_r);
    }

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
        if (type == ANC_FEEDFORWARD) {
            g_calib_ff_gain_l = gain_l;
        } else if (type == ANC_FEEDBACK) {
            g_calib_fb_gain_l = gain_l;
            g_calib_mc_gain_l = (512 * 512) / gain_l;
        } else if (type == ANC_TALKTHRU) {
            g_calib_tt_gain_l = gain_l;
        } else {
            ASSERT(0, "[%s] Invalid anc type: %d", __func__, type);
        }
    }
#endif

    return 0;
}

int anc_set_cfg(const struct_anc_cfg * cfg, enum ANC_TYPE_T anc_type,ANC_GAIN_TIME anc_gain_delay)
{
    uint32_t lock;

    DRIVERS_TRACE(1,"%s:0x%x", __func__,anc_type);

    if (cfg == NULL){
        DRIVERS_TRACE(1,"%s: cfg is null", __func__);
        return ANC_OTHER_ERR;
    }

    if ((anc_type&ANC_FEEDFORWARD && ff_open_flag==0)
    || (anc_type&ANC_FEEDBACK && fb_open_flag==0)
#if defined(AUDIO_ANC_TT_HW)
   ||  (anc_type&ANC_TALKTHRU && tt_open_flag==0)
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    || (anc_type&ANC_MUSICCANCLE && mc_open_flag==0)
#endif
     ) {
        DRIVERS_TRACE(1,"%s: anc_type=%d not opened", __func__, anc_type);
        return ANC_OTHER_ERR;
    }

    lock = int_lock();

    bool ff_using_flag = anc_ff_using_flag;
#if defined(AUDIO_ANC_TT_HW)
    bool tt_using_flag = anc_tt_using_flag;
#endif
    bool fb_using_flag = anc_fb_using_flag;
#if defined(AUDIO_ANC_FB_MC_HW)
    bool mc_using_flag = anc_mc_using_flag;
#endif

    if (anc_type & ANC_FEEDFORWARD){
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0){
            anc_cfg_coef_new.anc_cfg_ff_l = cfg->anc_cfg_ff_l;
        }
        if (g_calib_ff_gain_l != CALIB_GAIN_INVALID){
            anc_cfg_coef_new.anc_cfg_ff_l.total_gain = g_calib_ff_gain_l;
        }
#endif

        if (anc_ff_using_flag == 0) {
            anc_ff_using_flag = 1;
            anc_ff_reserve_flag = 0;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
            if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
                anc_cfg_coef.anc_cfg_ff_l = anc_cfg_coef_new.anc_cfg_ff_l;
            }
#endif
        } else {
            anc_ff_reserve_flag = 1;
        }
    }

#if defined(AUDIO_ANC_TT_HW)
    if (anc_type & ANC_TALKTHRU) {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            anc_cfg_coef_new.anc_cfg_tt_l = cfg->anc_cfg_tt_l;
        }
        if (g_calib_tt_gain_l != CALIB_GAIN_INVALID) {
            anc_cfg_coef_new.anc_cfg_tt_l.total_gain = g_calib_tt_gain_l;
        }
#endif
        if (anc_tt_using_flag == 0) {
            anc_tt_using_flag = 1;
            anc_tt_reserve_flag = 0;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
            if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
                anc_cfg_coef.anc_cfg_tt_l = anc_cfg_coef_new.anc_cfg_tt_l;
            }
#endif
        } else {
            anc_tt_reserve_flag = 1;
        }
    }
#endif

    if (anc_type & ANC_FEEDBACK) {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            anc_cfg_coef_new.anc_cfg_fb_l = cfg->anc_cfg_fb_l;
        }
        if (g_calib_fb_gain_l != CALIB_GAIN_INVALID) {
            anc_cfg_coef_new.anc_cfg_fb_l.total_gain = g_calib_fb_gain_l;
        }
#endif
        if (anc_fb_using_flag ==0) {
            anc_fb_using_flag = 1;
            anc_fb_reserve_flag = 0;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
            if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
                anc_cfg_coef.anc_cfg_fb_l = anc_cfg_coef_new.anc_cfg_fb_l;
            }
#endif
        } else {
            anc_fb_reserve_flag = 1;
        }
    }

#if defined(AUDIO_ANC_FB_MC_HW)
    if (anc_type & ANC_MUSICCANCLE){
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0){
            anc_cfg_coef_new.anc_cfg_mc_l = cfg->anc_cfg_mc_l;
        }
        if (g_calib_mc_gain_l != CALIB_GAIN_INVALID) {
            anc_cfg_coef_new.anc_cfg_mc_l.total_gain = g_calib_mc_gain_l;
        }
#endif
        if(anc_mc_using_flag == 0) {
            anc_mc_using_flag = 1;
            anc_mc_reserve_flag = 0;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
            if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
                anc_cfg_coef.anc_cfg_mc_l = anc_cfg_coef_new.anc_cfg_mc_l;
            }
#endif
        } else {
            anc_mc_reserve_flag = 1;
        }
    }
#endif

    int_unlock(lock);

    if (anc_type & ANC_FEEDFORWARD) {
        if (ff_using_flag == 0) {
            DRIVERS_TRACE(1,"%s:ANC_FEEDFORWARD", __func__);

            anc_set_cfg_internal(&anc_cfg_coef, ANC_FEEDFORWARD);
#if !defined(ANC_IIR_LINEAR_SMOOTH_MODE)
            hwtimer_stop(anc_ff_switching_timer);
            hwtimer_start(anc_ff_switching_timer, anc_switching_coef_delay_ff);
#endif
        }
    }
#if defined(AUDIO_ANC_TT_HW)
    if (anc_type & ANC_TALKTHRU) {
        if (tt_using_flag == 0) {
            DRIVERS_TRACE(1,"%s:ANC_TALKTHRU", __func__);

            anc_set_cfg_internal(&anc_cfg_coef, ANC_TALKTHRU);
#if !defined(ANC_IIR_LINEAR_SMOOTH_MODE)
            hwtimer_stop(anc_tt_switching_timer);
            hwtimer_start(anc_tt_switching_timer, anc_switching_coef_delay_tt);
#endif
        }
    }
#endif
    if (anc_type & ANC_FEEDBACK) {
        if(fb_using_flag == 0) {
            DRIVERS_TRACE(1,"%s:ANC_FEEDBACK", __func__);

            anc_set_cfg_internal(&anc_cfg_coef, ANC_FEEDBACK);
#if !defined(ANC_IIR_LINEAR_SMOOTH_MODE)
            hwtimer_stop(anc_fb_switching_timer);
            hwtimer_start(anc_fb_switching_timer, anc_switching_coef_delay_fb);
#endif
        }
    }
#if defined(AUDIO_ANC_FB_MC_HW)
    if (anc_type & ANC_MUSICCANCLE){
        if (mc_using_flag == 0){
            DRIVERS_TRACE(1,"%s:ANC_MUSICCANCLE", __func__);

            anc_set_cfg_internal(&anc_cfg_coef, ANC_MUSICCANCLE);
#if !defined(ANC_IIR_LINEAR_SMOOTH_MODE)
            hwtimer_stop(anc_mc_switching_timer);
            hwtimer_start(anc_mc_switching_timer, anc_switching_coef_delay_mc);
#endif
        }
    }
#endif
    return 0;
}

int anc_set_spkcalib_cfg(const struct_spkcalib_cfg *cfg)
{
    return 0;
}

int anc_set_dehowling_cfg(const dehowling_config *cfg)
{
    return 0;
}

int anc_set_psap_eq_cfg(const struct_psap_eq_cfg *cfg)
{
    return 0;
}

#if defined(AUDIO_ANC_FIR_HW)
void anc_fir_step_sample_mode(uint16_t step)
{
    fir_sample_step = step;
}

static void anc_fir_ctrl_reg_init(void)
{
    uint32_t real_fir_clk;

    fir_max_order = (128/fir_sample_step) *(32/4) * 4 * fir_sample_step;
    real_fir_clk = (fir_max_order * 384000) / (4 * fir_sample_step);
    DRIVERS_TRACE(1, "%s: fir_max_order = %d, MAX_FIR_LEN = %d，real_fir_clk = %d ", __func__, fir_max_order, MAX_FIR_LEN, real_fir_clk);

    hal_codec_fir_enable(real_fir_clk);
    hal_codec_fir_select_own_clock();

    codec->REG_06C = SET_BITFIELD(codec->REG_06C, CODEC_EN_CLK_FIR, 0x1f);
    codec->REG_07C = SET_BITFIELD(codec->REG_07C, CODEC_SOFT_RSTN_FIR, 0x1f);

    // set sample step
    codec->REG_77C = SET_BITFIELD(codec->REG_77C, CODEC_FIR_SAMPLE_STEP_CH0, fir_sample_step);
    codec->REG_77C = SET_BITFIELD(codec->REG_77C, CODEC_FIR_SAMPLE_STEP_CH1, fir_sample_step);
    codec->REG_77C = SET_BITFIELD(codec->REG_77C, CODEC_FIR_SAMPLE_STEP_CH2, fir_sample_step);
    codec->REG_77C = SET_BITFIELD(codec->REG_77C, CODEC_FIR_SAMPLE_STEP_CH3, fir_sample_step);

    // disable FIR stream;
    codec->REG_100 &= ~CODEC_FIR_STREAM_ENABLE_CH0;
    codec->REG_100 &= ~CODEC_FIR_STREAM_ENABLE_CH1;
    codec->REG_100 &= ~CODEC_FIR_STREAM_ENABLE_CH2;
    codec->REG_100 &= ~CODEC_FIR_STREAM_ENABLE_CH3;

    codec->REG_104 |= CODEC_PDU_FS_SWAP;
}
#endif

static void anc_ctrl_reg_init(void)
{
    //disable  iir clock
    codec->REG_06C = SET_BITFIELD(codec->REG_06C, CODEC_EN_CLK_IIR_ANC, 0x0);

    //disable iir 1.
    codec->REG_33C &= ~CODEC_CODEC_TWS_ANC_EQ;

    if (g_anc_bf_cfg.enable && g_anc_bf_cfg.tt_pdu_mix_flag) {
        DRIVERS_TRACE(0, "anc bf enable, tt mix to ff channel ..");
        codec->REG_33C |= CODEC_CODEC_PDU_MIX_EN_CH0;
    } else {
        codec->REG_33C &= ~CODEC_CODEC_PDU_MIX_EN_CH0;
    }


#if defined(AUDIO_ANC_FIR_HW)
    if (anc_fir_open_flag) {
        anc_fir_ctrl_reg_init();
    }
#endif

    //disable ANC;
    codec->REG_0D0 &= ~(CODEC_CODEC_ANC_ENABLE_CH0);

    //using TT as FF ANC
    if (using_tt_as_ff_en) {
        codec->REG_33C &= ~(CODEC_CODEC_TT_OUT_OFF_CH0);
        codec->REG_33C |= (CODEC_CODEC_TT_PDU_OFF_CH0);
    } else {
        codec->REG_33C |= (CODEC_CODEC_TT_OUT_OFF_CH0);
        codec->REG_33C &= ~(CODEC_CODEC_TT_PDU_OFF_CH0);
    }

    if (g_anc_bf_cfg.enable && g_anc_bf_cfg.tt_pdu_off_flag) {
        DRIVERS_TRACE(0, "anc bf enable, tt pdu off ..");
        codec->REG_33C |= (CODEC_CODEC_TT_PDU_OFF_CH0);
    } else {
        codec->REG_33C &= ~(CODEC_CODEC_TT_PDU_OFF_CH0);
    }

    //set the FF calib gain;
    codec->REG_244 &= ~CODEC_CODEC_ANC_CALIB_GAIN_UPDATE_FF_CH0;
    codec->REG_238 = SET_BITFIELD(codec->REG_238, CODEC_CODEC_ANC_CALIB_GAIN_COEF_FF_CH0, g_anc_bf_cfg.anc_ff_calib_gain);
    codec->REG_244 |= CODEC_CODEC_ANC_CALIB_GAIN_UPDATE_FF_CH0;

    //diable FF pass zero;
    codec->REG_244 |= CODEC_CODEC_ANC_CALIB_GAIN_PASS0_FF_CH0;

    //set the FB calib gain;
    codec->REG_244 &= ~CODEC_CODEC_ANC_CALIB_GAIN_UPDATE_FB_CH0;
    codec->REG_23C = SET_BITFIELD(codec->REG_23C, CODEC_CODEC_ANC_CALIB_GAIN_COEF_FB_CH0, CALIB_GAIN_Q);
    codec->REG_244 |= CODEC_CODEC_ANC_CALIB_GAIN_UPDATE_FB_CH0;

    //diable FB pass zero;
    codec->REG_244 |= CODEC_CODEC_ANC_CALIB_GAIN_PASS0_FB_CH0;

#if defined(AUDIO_ANC_TT_HW)
    //set the FB calib gain;
    codec->REG_244 &= ~CODEC_CODEC_CALIB_GAIN_UPDATE_TT_CH0;
    codec->REG_240 = SET_BITFIELD(codec->REG_240, CODEC_CODEC_CALIB_GAIN_COEF_TT_CH0, g_anc_bf_cfg.anc_tt_calib_gain);
    codec->REG_244 |= CODEC_CODEC_CALIB_GAIN_UPDATE_TT_CH0;

    //diable FB pass zero;
    codec->REG_244 |= CODEC_CODEC_CALIB_GAIN_PASS0_TT_CH0;
#endif

    codec->REG_22C &= ~CODEC_CODEC_TT_ENABLE_CH0;

    codec->REG_22C &= ~CODEC_CODEC_MM_ENABLE_CH0;

    codec->REG_0D0 &= ~CODEC_CODEC_ANC_RATE_SEL;

    codec->REG_0D0 &= ~CODEC_CODEC_FEEDBACK_CH0;

    codec->REG_0D0 &= ~CODEC_CODEC_DUAL_ANC_CH0;

    //8:-120dB 83:-100dB
    codec->REG_308 = SET_BITFIELD(codec->REG_308, CODEC_CODEC_IIR0_GAIN_EXT_TH, ANC_GAIN_RAMP_THRESHLD);
    codec->REG_30C = SET_BITFIELD(codec->REG_30C, CODEC_CODEC_IIR1_GAIN_EXT_TH, ANC_GAIN_RAMP_THRESHLD);
    codec->REG_690 = SET_BITFIELD(codec->REG_690, CODEC_CODEC_IIR4_GAIN_EXT_TH, ANC_GAIN_RAMP_THRESHLD);
    codec->REG_694 = SET_BITFIELD(codec->REG_694, CODEC_CODEC_IIR5_GAIN_EXT_TH, ANC_GAIN_RAMP_THRESHLD);

#if defined(ANC_IIR_LINEAR_SMOOTH_MODE)
    for (uint32_t ch=0; ch<IIR_REG_CONTINUE_ADDR_NUM; ch++) {
        *(&codec->REG_248 + ch) &= ~CODEC_CODEC_IIR0_ENABLE;
        *(&codec->REG_248 + ch) &= ~CODEC_CODEC_IIR0_IIRA_ENABLE;
        *(&codec->REG_248 + ch) &= ~CODEC_CODEC_IIR0_IIRB_ENABLE;
        *(&codec->REG_248 + ch) |= CODEC_CODEC_IIR0_BYPASS;
        *(&codec->REG_248 + ch) |= CODEC_CODEC_IIR0_LMT_BYPASS;
        *(&codec->REG_248 + ch) = SET_BITFIELD(*(&codec->REG_248 + ch), CODEC_CODEC_IIR0_COUNT, 0);
        *(&codec->REG_248 + ch) &= ~CODEC_CODEC_IIR0_COEF_SWAP;
        *(&codec->REG_248 + ch) &= ~CODEC_CODEC_IIR0_AUTO_STOP;
    }

    for (uint32_t ch=0; ch<IIR_REG_CONTINUE_ADDR_NUM; ch++) {
        *(&codec->REG_680 + ch) &= ~CODEC_CODEC_IIR4_ENABLE;
        *(&codec->REG_680 + ch) &= ~CODEC_CODEC_IIR4_IIRA_ENABLE;
        *(&codec->REG_680 + ch) &= ~CODEC_CODEC_IIR4_IIRB_ENABLE;
        *(&codec->REG_680 + ch) |= CODEC_CODEC_IIR4_BYPASS;
        *(&codec->REG_680 + ch) |= CODEC_CODEC_IIR4_LMT_BYPASS;
        *(&codec->REG_680 + ch) = SET_BITFIELD(*(&codec->REG_680 + ch), CODEC_CODEC_IIR4_COUNT, 0);
        *(&codec->REG_680 + ch) &= ~CODEC_CODEC_IIR4_COEF_SWAP;
        *(&codec->REG_680 + ch) &= ~CODEC_CODEC_IIR4_AUTO_STOP;
    }

    for (uint32_t ch=0; ch<4; ch++) {
        // Disable linear smooth
        codec->REG_224 &= ~(CODEC_CODEC_IIR0_RAMP_EN<<ch);
    }
#else
    // TODO: Will be removed
    //iir0
    codec->REG_248 &= ~CODEC_CODEC_IIR0_ENABLE;
    codec->REG_248 &= ~CODEC_CODEC_IIR0_IIRA_ENABLE;
    codec->REG_248 &= ~CODEC_CODEC_IIR0_IIRB_ENABLE;
    codec->REG_248 |= CODEC_CODEC_IIR0_BYPASS;
    codec->REG_248 |= CODEC_CODEC_IIR0_LMT_BYPASS;
    codec->REG_248 = SET_BITFIELD(codec->REG_248, CODEC_CODEC_IIR0_COUNT, 0);
    codec->REG_248 &= ~CODEC_CODEC_IIR0_COEF_SWAP;
    codec->REG_248 &= ~CODEC_CODEC_IIR0_AUTO_STOP;

    //iir1
    codec->REG_24C &= ~CODEC_CODEC_IIR1_ENABLE;
    codec->REG_24C &= ~CODEC_CODEC_IIR1_IIRA_ENABLE;
    codec->REG_24C &= ~CODEC_CODEC_IIR1_IIRB_ENABLE;
    codec->REG_24C |= CODEC_CODEC_IIR1_BYPASS;
    codec->REG_24C |= CODEC_CODEC_IIR1_LMT_BYPASS;
    codec->REG_24C = SET_BITFIELD(codec->REG_24C, CODEC_CODEC_IIR1_COUNT, 0);
    codec->REG_24C &= ~CODEC_CODEC_IIR1_COEF_SWAP;
    codec->REG_24C &= ~CODEC_CODEC_IIR1_AUTO_STOP;

    //iir4
    codec->REG_680 &= ~CODEC_CODEC_IIR4_ENABLE;
    codec->REG_680 &= ~CODEC_CODEC_IIR4_IIRA_ENABLE;
    codec->REG_680 &= ~CODEC_CODEC_IIR4_IIRB_ENABLE;
    codec->REG_680 |= CODEC_CODEC_IIR4_BYPASS;
    codec->REG_680 |= CODEC_CODEC_IIR4_LMT_BYPASS;
    codec->REG_680 = SET_BITFIELD(codec->REG_680, CODEC_CODEC_IIR4_COUNT, 0);
    codec->REG_680 &= ~CODEC_CODEC_IIR4_COEF_SWAP;
    codec->REG_680 &= ~CODEC_CODEC_IIR4_AUTO_STOP;

    //iir5
    codec->REG_684 &= ~CODEC_CODEC_IIR5_ENABLE;
    codec->REG_684 &= ~CODEC_CODEC_IIR5_IIRA_ENABLE;
    codec->REG_684 &= ~CODEC_CODEC_IIR5_IIRB_ENABLE;
    codec->REG_684 |= CODEC_CODEC_IIR5_BYPASS;
    codec->REG_684 |= CODEC_CODEC_IIR5_LMT_BYPASS;
    codec->REG_684 = SET_BITFIELD(codec->REG_684, CODEC_CODEC_IIR5_COUNT, 0);
    codec->REG_684 &= ~CODEC_CODEC_IIR5_COEF_SWAP;
    codec->REG_684 &= ~CODEC_CODEC_IIR5_AUTO_STOP;
#endif

    //unmute ANC.
    codec->REG_0D0 &= ~CODEC_CODEC_ANC_MUTE_CH0;

    //seting FIR filters,Bypass FIr Filter.
    codec->REG_0D0 &= ~CODEC_CODEC_FF_CH0_FIR_EN;
    codec->REG_0D0 &= ~CODEC_CODEC_FB_CH0_FIR_EN;
    codec->REG_0D0 &= ~CODEC_CODEC_MM0_FIR_EN;

    //disable gain updated when pass0
    codec->REG_0D4 |= CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FF_CH0;
    codec->REG_0D8 |= CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FB_CH0;

    codec->REG_230 |= CODEC_CODEC_MUTE_GAIN_PASS0_TT_CH0;
    codec->REG_234 |= CODEC_CODEC_MUTE_GAIN_PASS0_MM_CH0;

#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK) || defined(ANC_TT_CHECK)
    codec->REG_15C &= ~CODEC_CODEC_FB_CHECK_DCF1_BYPASS_CH0;
    codec->REG_15C &= ~CODEC_CODEC_FB_CHECK_LPF1_BYPASS_CH0;
    codec->REG_15C = SET_BITFIELD(codec->REG_15C, CODEC_CODEC_FB_CHECK_DCF1_UDC_CH0, 4);
    codec->REG_15C = SET_BITFIELD(codec->REG_15C, CODEC_CODEC_FB_CHECK_LPF1_COEF_A1_CH0, 0x0c);
    codec->REG_15C = SET_BITFIELD(codec->REG_15C, CODEC_CODEC_FB_CHECK_LPF1_COEF_B0_CH0, 0x1a);
    codec->REG_15C = SET_BITFIELD(codec->REG_15C, CODEC_CODEC_FB_CHECK_LPF1_COEF_B1_CH0, 0x1a);

    codec->REG_160 &= ~CODEC_CODEC_FB_CHECK_DCF2_BYPASS_CH0;
    codec->REG_160 &= ~CODEC_CODEC_FB_CHECK_LPF2_BYPASS_CH0;
    codec->REG_160 = SET_BITFIELD(codec->REG_160, CODEC_CODEC_FB_CHECK_DCF2_UDC_CH0, 4);
    codec->REG_160 = SET_BITFIELD(codec->REG_160, CODEC_CODEC_FB_CHECK_LPF2_COEF_A1_CH0, 0x0c);
    codec->REG_160 = SET_BITFIELD(codec->REG_160, CODEC_CODEC_FB_CHECK_LPF2_COEF_B0_CH0, 0x1a);
    codec->REG_160 = SET_BITFIELD(codec->REG_160, CODEC_CODEC_FB_CHECK_LPF2_COEF_B1_CH0, 0x1a);

    //codec->REG_15C |= CODEC_CODEC_FB_CHECK_DCF1_BYPASS_CH0;
    //codec->REG_15C |= CODEC_CODEC_FB_CHECK_LPF1_BYPASS_CH0;
    //codec->REG_160 |= CODEC_CODEC_FB_CHECK_DCF2_BYPASS_CH0;
    //codec->REG_160 |= CODEC_CODEC_FB_CHECK_LPF2_BYPASS_CH0;
#endif

#ifdef ANC_FAST_CALIB_MODE
    if (analog_debug_get_anc_calib_mode()) {
        anc_switching_gain_delay_ff = ANC_SET_GAIN_DELAY_SHORT;
        anc_switching_coef_delay_ff = ANC_SET_GAIN_DELAY_SHORT+ANC_SET_GAIN_CALIB_TIME;
#if defined(AUDIO_ANC_TT_HW)
        anc_switching_gain_delay_tt = ANC_SET_GAIN_DELAY_SHORT;
        anc_switching_coef_delay_tt = ANC_SET_GAIN_DELAY_SHORT+ANC_SET_GAIN_CALIB_TIME;
#endif
        anc_switching_gain_delay_fb = ANC_SET_GAIN_DELAY_SHORT;
        anc_switching_coef_delay_fb = ANC_SET_GAIN_DELAY_SHORT+ANC_SET_GAIN_CALIB_TIME;
#if defined(AUDIO_ANC_FB_MC_HW)
        anc_switching_gain_delay_mc = ANC_SET_GAIN_DELAY_SHORT;
        anc_switching_coef_delay_mc = ANC_SET_GAIN_DELAY_SHORT+ANC_SET_GAIN_CALIB_TIME;
#endif
    } else
#endif
    {
        anc_switching_gain_delay_ff = ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay_ff = ANC_SET_GAIN_DELAY_LONG+ANC_SET_GAIN_TIME;
#if defined(AUDIO_ANC_TT_HW)
        anc_switching_gain_delay_tt = ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay_tt = ANC_SET_GAIN_DELAY_LONG+ANC_SET_GAIN_TIME;
#endif
        anc_switching_gain_delay_fb = ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay_fb = ANC_SET_GAIN_DELAY_LONG+ANC_SET_GAIN_TIME;
#if defined(AUDIO_ANC_FB_MC_HW)
        anc_switching_gain_delay_mc = ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay_mc = ANC_SET_GAIN_DELAY_LONG+ANC_SET_GAIN_TIME;
#endif
    }

    anc_ff_using_flag = 0;
    anc_tt_using_flag = 0;
    anc_fb_using_flag = 0;
    anc_mc_using_flag = 0;

    anc_ff_reserve_flag = 0;
    anc_tt_reserve_flag = 0;
    anc_fb_reserve_flag = 0;
    anc_mc_reserve_flag = 0;

    return;
}

void anc_disable_gain_updated_when_pass0(uint8_t on)
{

}

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
static void anc_iir0_init(void)
{
    uint32_t iir_clk_value;

    //enable iir0 clock
    iir_clk_value = GET_BITFIELD(codec->REG_06C, CODEC_EN_CLK_IIR_ANC);
    iir_clk_value |= CODEC_EN_CLK_IIR_IIR0;
    codec->REG_06C = SET_BITFIELD(codec->REG_06C, CODEC_EN_CLK_IIR_ANC, iir_clk_value);

#if defined(ANC_IIR_LINEAR_SMOOTH_MODE)
    anc_iir_reg_reset(ANC_IIR_ID_0);
    codec->REG_248 |= CODEC_CODEC_IIR0_GAINCAL_EXT_BYPASS;
    codec->REG_220 = SET_BITFIELD(codec->REG_220, CODEC_CODEC_IIR_RAMP_STEP, g_anc_linear_ramp_step_curr);
    codec->REG_220 = SET_BITFIELD(codec->REG_220, CODEC_CODEC_IIR_RAMP_INTERVAL, CODEC_IIR_RAMP_INTVL_1_SAMP);
#else

    anc_iir_coefs  iir_coef_gain_ramp_ff;
#ifdef ANC_FAST_CALIB_MODE
    if (analog_debug_get_anc_calib_mode()){
        iir_coef_gain_ramp_ff = iir_coef_gain_ramp_ff_calib;
    } else
#endif
    {
        iir_coef_gain_ramp_ff = iir_coef_gain_ramp_ff_normal;
    }

    codec->REG_248 &= ~CODEC_CODEC_IIR0_ENABLE;
    codec->REG_248 &= ~CODEC_CODEC_IIR0_IIRA_ENABLE;
    codec->REG_248 &= ~CODEC_CODEC_IIR0_IIRB_ENABLE;
    codec->REG_248 &= ~CODEC_CODEC_IIR0_COEF_SWAP;
    codec->REG_248 &= ~CODEC_CODEC_IIR0_AUTO_STOP;

    //exp smooth
    anc_gain_ff_iir_coefs0_l[0].a1 = -iir_coef_gain_ramp_ff.coef_a[1];
    anc_gain_ff_iir_coefs0_l[0].a2 = -iir_coef_gain_ramp_ff.coef_a[2];
    anc_gain_ff_iir_coefs0_l[0].b0 = iir_coef_gain_ramp_ff.coef_b[0];
    anc_gain_ff_iir_coefs0_l[0].b1 = iir_coef_gain_ramp_ff.coef_b[1];
    anc_gain_ff_iir_coefs0_l[0].b2 = iir_coef_gain_ramp_ff.coef_b[2];

    anc_gain_ff_iir_coefs1_l[0].a1 = -iir_coef_gain_ramp_ff.coef_a[1];
    anc_gain_ff_iir_coefs1_l[0].a2 = -iir_coef_gain_ramp_ff.coef_a[2];
    anc_gain_ff_iir_coefs1_l[0].b0 = iir_coef_gain_ramp_ff.coef_b[0];
    anc_gain_ff_iir_coefs1_l[0].b1 = iir_coef_gain_ramp_ff.coef_b[1];
    anc_gain_ff_iir_coefs1_l[0].b2 = iir_coef_gain_ramp_ff.coef_b[2];

    codec->REG_248 &= ~CODEC_CODEC_IIR0_GAINCAL_EXT_BYPASS;
#endif

    codec->REG_248 &= ~CODEC_CODEC_IIR0_GAINUSE_EXT_BYPASS;
    codec->REG_25C &= ~CODEC_CODEC_IIR0_GAIN_EXT_SEL;

#if defined(ANC_LIMITER_FF)
    anc_limiter_ff_iir_coefs0_l[0].a1 = -iir_coef_limiter_attack_ff.coef_a[1];
    anc_limiter_ff_iir_coefs0_l[0].a2 = -iir_coef_limiter_attack_ff.coef_a[2];
    anc_limiter_ff_iir_coefs0_l[0].b0 = iir_coef_limiter_attack_ff.coef_b[0];
    anc_limiter_ff_iir_coefs0_l[0].b1 = iir_coef_limiter_attack_ff.coef_b[1];
    anc_limiter_ff_iir_coefs0_l[0].b2 = iir_coef_limiter_attack_ff.coef_b[2];

    anc_limiter_ff_iir_coefs0_l[1].a1 = -iir_coef_limiter_release_ff.coef_a[1];
    anc_limiter_ff_iir_coefs0_l[1].a2 = -iir_coef_limiter_release_ff.coef_a[2];
    anc_limiter_ff_iir_coefs0_l[1].b0 = iir_coef_limiter_release_ff.coef_b[0];
    anc_limiter_ff_iir_coefs0_l[1].b1 = iir_coef_limiter_release_ff.coef_b[1];
    anc_limiter_ff_iir_coefs0_l[1].b2 = iir_coef_limiter_release_ff.coef_b[2];

    anc_limiter_ff_iir_coefs1_l[0].a1 = -iir_coef_limiter_attack_ff.coef_a[1];
    anc_limiter_ff_iir_coefs1_l[0].a2 = -iir_coef_limiter_attack_ff.coef_a[2];
    anc_limiter_ff_iir_coefs1_l[0].b0 = iir_coef_limiter_attack_ff.coef_b[0];
    anc_limiter_ff_iir_coefs1_l[0].b1 = iir_coef_limiter_attack_ff.coef_b[1];
    anc_limiter_ff_iir_coefs1_l[0].b2 = iir_coef_limiter_attack_ff.coef_b[2];

    anc_limiter_ff_iir_coefs1_l[1].a1 = -iir_coef_limiter_release_ff.coef_a[1];
    anc_limiter_ff_iir_coefs1_l[1].a2 = -iir_coef_limiter_release_ff.coef_a[2];
    anc_limiter_ff_iir_coefs1_l[1].b0 = iir_coef_limiter_release_ff.coef_b[0];
    anc_limiter_ff_iir_coefs1_l[1].b1 = iir_coef_limiter_release_ff.coef_b[1];
    anc_limiter_ff_iir_coefs1_l[1].b2 = iir_coef_limiter_release_ff.coef_b[2];

    ff_limiter_att_l_old=iir_coef_limiter_attack_ff;
    ff_limiter_rls_l_old=iir_coef_limiter_release_ff;

#endif

#if defined(ANC_LIMITER_FF)
    codec->REG_248 &= ~CODEC_CODEC_IIR0_LMT_BYPASS;

    //update the threshold
    codec->REG_25C &= ~CODEC_CODEC_IIR0_LMT_TH_UPDATE;
    codec->REG_31C = SET_BITFIELD(codec->REG_31C, CODEC_CODEC_IIR0_LMT_TH, ANC_LIMITER_FF_THREHOLD);
    hal_sys_timer_delay_us(1);
    codec->REG_25C |= CODEC_CODEC_IIR0_LMT_TH_UPDATE;

    codec->REG_300 = SET_BITFIELD(codec->REG_300, CODEC_CODEC_IIR0_LMT_DELAY, ANC_LIMITER_FF_ATTACK_DELAY);
#else
    codec->REG_248 |= CODEC_CODEC_IIR0_LMT_BYPASS;
#endif

    for (int i = 0; i < IIR_COUNTER; i++) {
        anc_ff_iir_coefs0_l[i].a1 = 0;
        anc_ff_iir_coefs0_l[i].a2 = 0;
        anc_ff_iir_coefs0_l[i].b0 = 0;
        anc_ff_iir_coefs0_l[i].b1 = 0;
        anc_ff_iir_coefs0_l[i].b2 = 0;

        anc_ff_iir_coefs1_l[i].a1 = 0;
        anc_ff_iir_coefs1_l[i].a2 = 0;
        anc_ff_iir_coefs1_l[i].b0 = 0;
        anc_ff_iir_coefs1_l[i].b1 = 0;
        anc_ff_iir_coefs1_l[i].b2 = 0;
    }

    codec->REG_248 &= ~CODEC_CODEC_IIR0_BYPASS;
    codec->REG_248 = SET_BITFIELD(codec->REG_248, CODEC_CODEC_IIR0_COUNT, IIR_FF_COUNTER);

#if defined(ANC_IIR_LINEAR_SMOOTH_MODE)
    anc_iir_reg_enable(ANC_IIR_ID_0);
#else
    codec->REG_248 |= CODEC_CODEC_IIR0_AUTO_STOP;
    codec->REG_248 |= CODEC_CODEC_IIR0_IIRA_ENABLE;
    codec->REG_248 |= CODEC_CODEC_IIR0_IIRB_ENABLE;
    codec->REG_248 |= CODEC_CODEC_IIR0_ENABLE;
#endif

    iir0_coef_using = 0;
#if defined(ANC_LINEAR_SMOOTH_CHECK)
    anc_iir0_linear_smooth_fade_flag = 0;
#endif
}

#if defined(AUDIO_ANC_TT_HW)
static void anc_iir1_init(void)
{
    uint32_t iir_clk_value;

    //enable iir0 clock
    iir_clk_value = GET_BITFIELD(codec->REG_06C, CODEC_EN_CLK_IIR_ANC);
    iir_clk_value |= CODEC_EN_CLK_IIR_IIR1;
    codec->REG_06C = SET_BITFIELD(codec->REG_06C, CODEC_EN_CLK_IIR_ANC, iir_clk_value);

#if defined(ANC_IIR_LINEAR_SMOOTH_MODE)
    anc_iir_reg_reset(ANC_IIR_ID_1);
    codec->REG_24C |= CODEC_CODEC_IIR1_GAINCAL_EXT_BYPASS;
    codec->REG_220 = SET_BITFIELD(codec->REG_220, CODEC_CODEC_IIR_RAMP_STEP, g_anc_linear_ramp_step_curr);
    codec->REG_220 = SET_BITFIELD(codec->REG_220, CODEC_CODEC_IIR_RAMP_INTERVAL, CODEC_IIR_RAMP_INTVL_1_SAMP);
#else
    codec->REG_24C &= ~CODEC_CODEC_IIR1_ENABLE;
    codec->REG_24C &= ~CODEC_CODEC_IIR1_IIRA_ENABLE;
    codec->REG_24C &= ~CODEC_CODEC_IIR1_IIRB_ENABLE;
    codec->REG_24C &= ~CODEC_CODEC_IIR1_COEF_SWAP;
    codec->REG_24C &= ~CODEC_CODEC_IIR1_AUTO_STOP;

    anc_iir_coefs  iir_coef_gain_ramp_tt;
#ifdef ANC_FAST_CALIB_MODE
    if (analog_debug_get_anc_calib_mode()) {
        iir_coef_gain_ramp_tt = iir_coef_gain_ramp_tt_calib;
    } else
#endif
    {
        iir_coef_gain_ramp_tt = iir_coef_gain_ramp_tt_normal;
    }

    anc_gain_tt_iir_coefs0_l[0].a1 = -iir_coef_gain_ramp_tt.coef_a[1];
    anc_gain_tt_iir_coefs0_l[0].a2 = -iir_coef_gain_ramp_tt.coef_a[2];
    anc_gain_tt_iir_coefs0_l[0].b0 = iir_coef_gain_ramp_tt.coef_b[0];
    anc_gain_tt_iir_coefs0_l[0].b1 = iir_coef_gain_ramp_tt.coef_b[1];
    anc_gain_tt_iir_coefs0_l[0].b2 = iir_coef_gain_ramp_tt.coef_b[2];

    anc_gain_tt_iir_coefs1_l[0].a1 = -iir_coef_gain_ramp_tt.coef_a[1];
    anc_gain_tt_iir_coefs1_l[0].a2 = -iir_coef_gain_ramp_tt.coef_a[2];
    anc_gain_tt_iir_coefs1_l[0].b0 = iir_coef_gain_ramp_tt.coef_b[0];
    anc_gain_tt_iir_coefs1_l[0].b1 = iir_coef_gain_ramp_tt.coef_b[1];
    anc_gain_tt_iir_coefs1_l[0].b2 = iir_coef_gain_ramp_tt.coef_b[2];

    codec->REG_24C &= ~CODEC_CODEC_IIR1_GAINCAL_EXT_BYPASS;
#endif
    codec->REG_24C &= ~CODEC_CODEC_IIR1_GAINUSE_EXT_BYPASS;
    codec->REG_25C &= ~CODEC_CODEC_IIR1_GAIN_EXT_SEL;

#if defined(ANC_LIMITER_TT) && defined(AUDIO_ANC_TT_HW)
    anc_limiter_tt_iir_coefs0_l[0].a1 = -iir_coef_limiter_attack_tt.coef_a[1];
    anc_limiter_tt_iir_coefs0_l[0].a2 = -iir_coef_limiter_attack_tt.coef_a[2];
    anc_limiter_tt_iir_coefs0_l[0].b0 = iir_coef_limiter_attack_tt.coef_b[0];
    anc_limiter_tt_iir_coefs0_l[0].b1 = iir_coef_limiter_attack_tt.coef_b[1];
    anc_limiter_tt_iir_coefs0_l[0].b2 = iir_coef_limiter_attack_tt.coef_b[2];

    anc_limiter_tt_iir_coefs0_l[1].a1 = -iir_coef_limiter_release_tt.coef_a[1];
    anc_limiter_tt_iir_coefs0_l[1].a2 = -iir_coef_limiter_release_tt.coef_a[2];
    anc_limiter_tt_iir_coefs0_l[1].b0 = iir_coef_limiter_release_tt.coef_b[0];
    anc_limiter_tt_iir_coefs0_l[1].b1 = iir_coef_limiter_release_tt.coef_b[1];
    anc_limiter_tt_iir_coefs0_l[1].b2 = iir_coef_limiter_release_tt.coef_b[2];

    anc_limiter_tt_iir_coefs1_l[0].a1 = -iir_coef_limiter_attack_tt.coef_a[1];
    anc_limiter_tt_iir_coefs1_l[0].a2 = -iir_coef_limiter_attack_tt.coef_a[2];
    anc_limiter_tt_iir_coefs1_l[0].b0 = iir_coef_limiter_attack_tt.coef_b[0];
    anc_limiter_tt_iir_coefs1_l[0].b1 = iir_coef_limiter_attack_tt.coef_b[1];
    anc_limiter_tt_iir_coefs1_l[0].b2 = iir_coef_limiter_attack_tt.coef_b[2];

    anc_limiter_tt_iir_coefs1_l[1].a1 = -iir_coef_limiter_release_tt.coef_a[1];
    anc_limiter_tt_iir_coefs1_l[1].a2 = -iir_coef_limiter_release_tt.coef_a[2];
    anc_limiter_tt_iir_coefs1_l[1].b0 = iir_coef_limiter_release_tt.coef_b[0];
    anc_limiter_tt_iir_coefs1_l[1].b1 = iir_coef_limiter_release_tt.coef_b[1];
    anc_limiter_tt_iir_coefs1_l[1].b2 = iir_coef_limiter_release_tt.coef_b[2];

    tt_limiter_att_l_old=iir_coef_limiter_attack_tt;
    tt_limiter_rls_l_old=iir_coef_limiter_release_tt;

//#ifdef ANC_FAST_CALIB_MODE
//    if (analog_debug_get_anc_calib_mode()) {
//        codec->REG_24C |= CODEC_CODEC_IIR1_LMT_BYPASS;
//    } else
//#endif
    {
        codec->REG_24C &= ~CODEC_CODEC_IIR1_LMT_BYPASS;
    }

    //update the threshold
    codec->REG_25C &= ~CODEC_CODEC_IIR1_LMT_TH_UPDATE;
    codec->REG_320 = SET_BITFIELD(codec->REG_320, CODEC_CODEC_IIR1_LMT_TH, ANC_LIMITER_TT_THREHOLD);
    hal_sys_timer_delay_us(1);
    codec->REG_25C |= CODEC_CODEC_IIR1_LMT_TH_UPDATE;

    codec->REG_300 = SET_BITFIELD(codec->REG_300, CODEC_CODEC_IIR1_LMT_DELAY, ANC_LIMITER_TT_ATTACK_DELAY);
#else
    codec->REG_24C |= CODEC_CODEC_IIR1_LMT_BYPASS;
#endif

    for (int i = 0; i < IIR_COUNTER; i++) {
        anc_tt_iir_coefs0_l[i].a1 = 0;
        anc_tt_iir_coefs0_l[i].a2 = 0;
        anc_tt_iir_coefs0_l[i].b0 = 0;
        anc_tt_iir_coefs0_l[i].b1 = 0;
        anc_tt_iir_coefs0_l[i].b2 = 0;

        anc_tt_iir_coefs1_l[i].a1 = 0;
        anc_tt_iir_coefs1_l[i].a2 = 0;
        anc_tt_iir_coefs1_l[i].b0 = 0;
        anc_tt_iir_coefs1_l[i].b1 = 0;
        anc_tt_iir_coefs1_l[i].b2 =  0;
    }

    codec->REG_24C &= ~CODEC_CODEC_IIR1_BYPASS;
    codec->REG_24C = SET_BITFIELD(codec->REG_24C, CODEC_CODEC_IIR1_COUNT, IIR_TT_COUNTER);

#if defined(ANC_IIR_LINEAR_SMOOTH_MODE)
    anc_iir_reg_enable(ANC_IIR_ID_1);
#else
    codec->REG_24C |= CODEC_CODEC_IIR1_AUTO_STOP;
    codec->REG_24C |= CODEC_CODEC_IIR1_IIRA_ENABLE;
    codec->REG_24C |= CODEC_CODEC_IIR1_IIRB_ENABLE;
    codec->REG_24C |= CODEC_CODEC_IIR1_ENABLE;
#endif

    iir1_coef_using = 0;
#if defined(ANC_LINEAR_SMOOTH_CHECK)
    anc_iir1_linear_smooth_fade_flag = 0;
#endif
}
#endif

static void anc_iir4_init(void)
{
    uint32_t iir_clk_value;

    //enable iir4 clock
    iir_clk_value = GET_BITFIELD(codec->REG_06C, CODEC_EN_CLK_IIR_ANC);
    iir_clk_value |= CODEC_EN_CLK_IIR_IIR4;
    codec->REG_06C = SET_BITFIELD(codec->REG_06C, CODEC_EN_CLK_IIR_ANC, iir_clk_value);

#if defined(ANC_IIR_LINEAR_SMOOTH_MODE)
    anc_iir_reg_reset(ANC_IIR_ID_4);
    codec->REG_680 |= CODEC_CODEC_IIR4_GAINCAL_EXT_BYPASS;
    codec->REG_220 = SET_BITFIELD(codec->REG_220, CODEC_CODEC_IIR_RAMP_STEP, g_anc_linear_ramp_step_curr);
    codec->REG_220 = SET_BITFIELD(codec->REG_220, CODEC_CODEC_IIR_RAMP_INTERVAL, CODEC_IIR_RAMP_INTVL_1_SAMP);
#else
    codec->REG_680 &= ~CODEC_CODEC_IIR4_ENABLE;
    codec->REG_680 &= ~CODEC_CODEC_IIR4_IIRA_ENABLE;
    codec->REG_680 &= ~CODEC_CODEC_IIR4_IIRB_ENABLE;
    codec->REG_680 &= ~CODEC_CODEC_IIR4_COEF_SWAP;
    codec->REG_680 &= ~CODEC_CODEC_IIR4_AUTO_STOP;

    anc_iir_coefs  iir_coef_gain_ramp_fb;
#ifdef ANC_FAST_CALIB_MODE
    if (analog_debug_get_anc_calib_mode()) {
        iir_coef_gain_ramp_fb = iir_coef_gain_ramp_fb_calib;
    } else
#endif
    {
        iir_coef_gain_ramp_fb = iir_coef_gain_ramp_fb_normal;
    }
    anc_gain_fb_iir_coefs0_l[0].a1 = -iir_coef_gain_ramp_fb.coef_a[1];
    anc_gain_fb_iir_coefs0_l[0].a2 = -iir_coef_gain_ramp_fb.coef_a[2];
    anc_gain_fb_iir_coefs0_l[0].b0 = iir_coef_gain_ramp_fb.coef_b[0];
    anc_gain_fb_iir_coefs0_l[0].b1 = iir_coef_gain_ramp_fb.coef_b[1];
    anc_gain_fb_iir_coefs0_l[0].b2 = iir_coef_gain_ramp_fb.coef_b[2];

    anc_gain_fb_iir_coefs1_l[0].a1 = -iir_coef_gain_ramp_fb.coef_a[1];
    anc_gain_fb_iir_coefs1_l[0].a2 = -iir_coef_gain_ramp_fb.coef_a[2];
    anc_gain_fb_iir_coefs1_l[0].b0 = iir_coef_gain_ramp_fb.coef_b[0];
    anc_gain_fb_iir_coefs1_l[0].b1 = iir_coef_gain_ramp_fb.coef_b[1];
    anc_gain_fb_iir_coefs1_l[0].b2 = iir_coef_gain_ramp_fb.coef_b[2];

    codec->REG_680 &= ~CODEC_CODEC_IIR4_GAINCAL_EXT_BYPASS;
#endif
    codec->REG_680 &= ~CODEC_CODEC_IIR4_GAINUSE_EXT_BYPASS;

    codec->REG_25C &= ~CODEC_CODEC_IIR4_GAIN_EXT_SEL;

#if defined(ANC_LIMITER_FB)
    anc_limiter_fb_iir_coefs0_l[0].a1 = -iir_coef_limiter_attack_fb.coef_a[1];
    anc_limiter_fb_iir_coefs0_l[0].a2 = -iir_coef_limiter_attack_fb.coef_a[2];
    anc_limiter_fb_iir_coefs0_l[0].b0 = iir_coef_limiter_attack_fb.coef_b[0];
    anc_limiter_fb_iir_coefs0_l[0].b1 = iir_coef_limiter_attack_fb.coef_b[1];
    anc_limiter_fb_iir_coefs0_l[0].b2 = iir_coef_limiter_attack_fb.coef_b[2];

    anc_limiter_fb_iir_coefs0_l[1].a1 = -iir_coef_limiter_release_fb.coef_a[1];
    anc_limiter_fb_iir_coefs0_l[1].a2 = -iir_coef_limiter_release_fb.coef_a[2];
    anc_limiter_fb_iir_coefs0_l[1].b0 = iir_coef_limiter_release_fb.coef_b[0];
    anc_limiter_fb_iir_coefs0_l[1].b1 = iir_coef_limiter_release_fb.coef_b[1];
    anc_limiter_fb_iir_coefs0_l[1].b2 = iir_coef_limiter_release_fb.coef_b[2];

    anc_limiter_fb_iir_coefs1_l[0].a1 = -iir_coef_limiter_attack_fb.coef_a[1];
    anc_limiter_fb_iir_coefs1_l[0].a2 = -iir_coef_limiter_attack_fb.coef_a[2];
    anc_limiter_fb_iir_coefs1_l[0].b0 = iir_coef_limiter_attack_fb.coef_b[0];
    anc_limiter_fb_iir_coefs1_l[0].b1 = iir_coef_limiter_attack_fb.coef_b[1];
    anc_limiter_fb_iir_coefs1_l[0].b2 = iir_coef_limiter_attack_fb.coef_b[2];

    anc_limiter_fb_iir_coefs1_l[1].a1 = -iir_coef_limiter_release_fb.coef_a[1];
    anc_limiter_fb_iir_coefs1_l[1].a2 = -iir_coef_limiter_release_fb.coef_a[2];
    anc_limiter_fb_iir_coefs1_l[1].b0 = iir_coef_limiter_release_fb.coef_b[0];
    anc_limiter_fb_iir_coefs1_l[1].b1 = iir_coef_limiter_release_fb.coef_b[1];
    anc_limiter_fb_iir_coefs1_l[1].b2 = iir_coef_limiter_release_fb.coef_b[2];

    fb_limiter_att_l_old=iir_coef_limiter_attack_fb;
    fb_limiter_rls_l_old=iir_coef_limiter_release_fb;

//#ifdef ANC_FAST_CALIB_MODE
//    if (analog_debug_get_anc_calib_mode()) {
//        codec->REG_680 |= CODEC_CODEC_IIR4_LMT_BYPASS;
//    } else
//#endif
    {
        codec->REG_680 &= ~CODEC_CODEC_IIR4_LMT_BYPASS;
    }

    //update the threshold, 0dB: 0x7fffff
    codec->REG_25C &= ~CODEC_CODEC_IIR4_LMT_TH_UPDATE;
    codec->REG_32C = SET_BITFIELD(codec->REG_32C, CODEC_CODEC_IIR4_LMT_TH, ANC_LIMITER_FB_THREHOLD);
    hal_sys_timer_delay_us(1);
    codec->REG_25C |= CODEC_CODEC_IIR4_LMT_TH_UPDATE;

    codec->REG_304 = SET_BITFIELD(codec->REG_304, CODEC_CODEC_IIR4_LMT_DELAY, ANC_LIMITER_FB_ATTACK_DELAY);
#else
    codec->REG_680 |= CODEC_CODEC_IIR4_LMT_BYPASS;
#endif

    for (int i = 0; i < IIR_COUNTER; i++) {
        anc_fb_iir_coefs0_l[i].a1 = 0;
        anc_fb_iir_coefs0_l[i].a2 = 0;
        anc_fb_iir_coefs0_l[i].b0 = 0;
        anc_fb_iir_coefs0_l[i].b1 = 0;
        anc_fb_iir_coefs0_l[i].b2 = 0;

        anc_fb_iir_coefs1_l[i].a1 = 0;
        anc_fb_iir_coefs1_l[i].a2 = 0;
        anc_fb_iir_coefs1_l[i].b0 = 0;
        anc_fb_iir_coefs1_l[i].b1 = 0;
        anc_fb_iir_coefs1_l[i].b2 = 0;
    }

    codec->REG_680 &= ~CODEC_CODEC_IIR4_BYPASS;
    codec->REG_680 = SET_BITFIELD(codec->REG_680, CODEC_CODEC_IIR4_COUNT, IIR_FB_COUNTER);

#if defined(ANC_IIR_LINEAR_SMOOTH_MODE)
    anc_iir_reg_enable(ANC_IIR_ID_4);
#else
    codec->REG_680 |= CODEC_CODEC_IIR4_AUTO_STOP;
    codec->REG_680 |= CODEC_CODEC_IIR4_IIRA_ENABLE;
    codec->REG_680 |= CODEC_CODEC_IIR4_IIRB_ENABLE;
    codec->REG_680 |= CODEC_CODEC_IIR4_ENABLE;
#endif

    iir4_coef_using = 0;
#if defined(ANC_LINEAR_SMOOTH_CHECK)
    anc_iir4_linear_smooth_fade_flag = 0;
#endif
}

#if defined(AUDIO_ANC_FB_MC_HW)
static POSSIBLY_UNUSED void anc_iir5_init(void)
{
    uint32_t iir_clk_value;

    //enable iir5 clock
    iir_clk_value = GET_BITFIELD(codec->REG_06C, CODEC_EN_CLK_IIR_ANC);
    iir_clk_value |= CODEC_EN_CLK_IIR_IIR5;
    codec->REG_06C = SET_BITFIELD(codec->REG_06C, CODEC_EN_CLK_IIR_ANC, iir_clk_value);

#if defined(ANC_IIR_LINEAR_SMOOTH_MODE)
    anc_iir_reg_reset(ANC_IIR_ID_5);
    codec->REG_684 |= CODEC_CODEC_IIR5_GAINCAL_EXT_BYPASS;
    codec->REG_220 = SET_BITFIELD(codec->REG_220, CODEC_CODEC_IIR_RAMP_STEP, g_anc_linear_ramp_step_curr);
    codec->REG_220 = SET_BITFIELD(codec->REG_220, CODEC_CODEC_IIR_RAMP_INTERVAL, CODEC_IIR_RAMP_INTVL_1_SAMP);
#else
    codec->REG_684 &= ~CODEC_CODEC_IIR5_ENABLE;
    codec->REG_684 &= ~CODEC_CODEC_IIR5_IIRA_ENABLE;
    codec->REG_684 &= ~CODEC_CODEC_IIR5_IIRB_ENABLE;
    codec->REG_684 &= ~CODEC_CODEC_IIR5_COEF_SWAP;
    codec->REG_684 &= ~CODEC_CODEC_IIR5_AUTO_STOP;

    anc_iir_coefs  iir_coef_gain_ramp_mc;
#ifdef ANC_FAST_CALIB_MODE
    if (analog_debug_get_anc_calib_mode()) {
        iir_coef_gain_ramp_mc = iir_coef_gain_ramp_mc_calib;
    } else
#endif
    {
        iir_coef_gain_ramp_mc = iir_coef_gain_ramp_mc_normal;
    }
    anc_gain_mc_iir_coefs0_l[0].a1 = -iir_coef_gain_ramp_mc.coef_a[1];
    anc_gain_mc_iir_coefs0_l[0].a2 = -iir_coef_gain_ramp_mc.coef_a[2];
    anc_gain_mc_iir_coefs0_l[0].b0 = iir_coef_gain_ramp_mc.coef_b[0];
    anc_gain_mc_iir_coefs0_l[0].b1 = iir_coef_gain_ramp_mc.coef_b[1];
    anc_gain_mc_iir_coefs0_l[0].b2 = iir_coef_gain_ramp_mc.coef_b[2];

    anc_gain_mc_iir_coefs1_l[0].a1 = -iir_coef_gain_ramp_mc.coef_a[1];
    anc_gain_mc_iir_coefs1_l[0].a2 = -iir_coef_gain_ramp_mc.coef_a[2];
    anc_gain_mc_iir_coefs1_l[0].b0 = iir_coef_gain_ramp_mc.coef_b[0];
    anc_gain_mc_iir_coefs1_l[0].b1 = iir_coef_gain_ramp_mc.coef_b[1];
    anc_gain_mc_iir_coefs1_l[0].b2 = iir_coef_gain_ramp_mc.coef_b[2];

    codec->REG_684 &= ~CODEC_CODEC_IIR5_GAINCAL_EXT_BYPASS;
#endif
    codec->REG_684 &= ~CODEC_CODEC_IIR5_GAINUSE_EXT_BYPASS;

    codec->REG_25C &= ~CODEC_CODEC_IIR5_GAIN_EXT_SEL;

#if defined(ANC_LIMITER_MC)
    anc_limiter_mc_iir_coefs0_l[0].a1 = -iir_coef_limiter_attack_mc.coef_a[1];
    anc_limiter_mc_iir_coefs0_l[0].a2 = -iir_coef_limiter_attack_mc.coef_a[2];
    anc_limiter_mc_iir_coefs0_l[0].b0 = iir_coef_limiter_attack_mc.coef_b[0];
    anc_limiter_mc_iir_coefs0_l[0].b1 = iir_coef_limiter_attack_mc.coef_b[1];
    anc_limiter_mc_iir_coefs0_l[0].b2 = iir_coef_limiter_attack_mc.coef_b[2];

    anc_limiter_mc_iir_coefs0_l[1].a1 = -iir_coef_limiter_release_mc.coef_a[1];
    anc_limiter_mc_iir_coefs0_l[1].a2 =-iir_coef_limiter_release_mc.coef_a[2];
    anc_limiter_mc_iir_coefs0_l[1].b0 = iir_coef_limiter_release_mc.coef_b[0];
    anc_limiter_mc_iir_coefs0_l[1].b1 = iir_coef_limiter_release_mc.coef_b[1];
    anc_limiter_mc_iir_coefs0_l[1].b2 = iir_coef_limiter_release_mc.coef_b[2];

    anc_limiter_mc_iir_coefs1_l[0].a1 = -iir_coef_limiter_attack_mc.coef_a[1];
    anc_limiter_mc_iir_coefs1_l[0].a2 = -iir_coef_limiter_attack_mc.coef_a[2];
    anc_limiter_mc_iir_coefs1_l[0].b0 = iir_coef_limiter_attack_mc.coef_b[0];
    anc_limiter_mc_iir_coefs1_l[0].b1 = iir_coef_limiter_attack_mc.coef_b[1];
    anc_limiter_mc_iir_coefs1_l[0].b2 = iir_coef_limiter_attack_mc.coef_b[2];

    anc_limiter_mc_iir_coefs1_l[1].a1 = -iir_coef_limiter_release_mc.coef_a[1];
    anc_limiter_mc_iir_coefs1_l[1].a2 = -iir_coef_limiter_release_mc.coef_a[2];
    anc_limiter_mc_iir_coefs1_l[1].b0 = iir_coef_limiter_release_mc.coef_b[0];
    anc_limiter_mc_iir_coefs1_l[1].b1 = iir_coef_limiter_release_mc.coef_b[1];
    anc_limiter_mc_iir_coefs1_l[1].b2 = iir_coef_limiter_release_mc.coef_b[2];

    mc_limiter_att_l_old=iir_coef_limiter_attack_mc;
    mc_limiter_rls_l_old=iir_coef_limiter_release_mc;


//#ifdef ANC_FAST_CALIB_MODE
//    if (analog_debug_get_anc_calib_mode()) {
//        codec->REG_684 |= CODEC_CODEC_IIR5_LMT_BYPASS;
//    } else
//#endif
    {
        codec->REG_684 &= ~CODEC_CODEC_IIR5_LMT_BYPASS;
    }

    //update the threshold, 0dB: 0x7fffff
    codec->REG_25C &= ~CODEC_CODEC_IIR5_LMT_TH_UPDATE;
    codec->REG_330 = SET_BITFIELD(codec->REG_330, CODEC_CODEC_IIR5_LMT_TH, ANC_LIMITER_MC_THREHOLD);
    hal_sys_timer_delay_us(1);
    codec->REG_25C |= CODEC_CODEC_IIR5_LMT_TH_UPDATE;

    codec->REG_304 = SET_BITFIELD(codec->REG_304, CODEC_CODEC_IIR5_LMT_DELAY, ANC_LIMITER_MC_ATTACK_DELAY);
#else
    codec->REG_684 |= CODEC_CODEC_IIR5_LMT_BYPASS;
#endif

    for (int i = 0;i < IIR_COUNTER; i++) {
        anc_mc_iir_coefs0_l[i].a1 = 0;
        anc_mc_iir_coefs0_l[i].a2 = 0;
        anc_mc_iir_coefs0_l[i].b0 = 0;
        anc_mc_iir_coefs0_l[i].b1 = 0;
        anc_mc_iir_coefs0_l[i].b2 = 0;

        anc_mc_iir_coefs1_l[i].a1 = 0;
        anc_mc_iir_coefs1_l[i].a2 = 0;
        anc_mc_iir_coefs1_l[i].b0 = 0;
        anc_mc_iir_coefs1_l[i].b1 = 0;
        anc_mc_iir_coefs1_l[i].b2 = 0;
    }

    codec->REG_684 &= ~CODEC_CODEC_IIR5_BYPASS;
    codec->REG_684 = SET_BITFIELD(codec->REG_684, CODEC_CODEC_IIR5_COUNT, IIR_MC_COUNTER);

#if defined(ANC_IIR_LINEAR_SMOOTH_MODE)
    anc_iir_reg_enable(ANC_IIR_ID_5);
#else
    codec->REG_684 |= CODEC_CODEC_IIR5_AUTO_STOP;
    codec->REG_684 |= CODEC_CODEC_IIR5_IIRA_ENABLE;
    codec->REG_684 |= CODEC_CODEC_IIR5_IIRB_ENABLE;
    codec->REG_684 |= CODEC_CODEC_IIR5_ENABLE;
#endif

    iir5_coef_using = 0;
#if defined(ANC_LINEAR_SMOOTH_CHECK)
    anc_iir5_linear_smooth_fade_flag = 0;
#endif
}
#endif
#endif

static void anc_ctrl_reg_open(enum ANC_TYPE_T anc_type)
{
    DRIVERS_TRACE(1,"%s", __func__);

    if (anc_type == ANC_FEEDFORWARD) {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            //set the FF gain;
            codec->REG_0D4 &= ~CODEC_CODEC_ANC_MUTE_GAIN_UPDATE_FF_CH0;
            codec->REG_0D4 = SET_BITFIELD(codec->REG_0D4, CODEC_CODEC_ANC_MUTE_GAIN_FF_CH0, 512);
            hal_sys_timer_delay_us(1);
            codec->REG_0D4 |= CODEC_CODEC_ANC_MUTE_GAIN_UPDATE_FF_CH0;

            ff_ramp_gain_l = 0;
            ff_ramp_coef_l = 0;

            for (int i = 0; i < IIR_COUNTER; i++) {
                ff_filtes_l_old.iir_coef[i].coef_a[0] = 0;
                ff_filtes_l_old.iir_coef[i].coef_a[1] = 0;
                ff_filtes_l_old.iir_coef[i].coef_a[2] = 0;
                ff_filtes_l_old.iir_coef[i].coef_b[0] = 0;
                ff_filtes_l_old.iir_coef[i].coef_b[1] = 0;
                ff_filtes_l_old.iir_coef[i].coef_b[2] = 0;
            }

            ff_filtes_l_old.total_gain = 512;
            ff_filtes_l_old.iir_counter = IIR_FF_COUNTER;
            ff_filtes_l_old.iir_bypass_flag = 0;

            anc_iir0_init();

#if defined(AUDIO_ANC_FIR_HW)
            if (anc_fir_open_flag) {
                anc_fir_ctrl_reg_open(ANC_FEEDFORWARD, AUD_CHANNEL_MAP_CH0);
            }
#endif
        }
#endif
    }

#if defined(AUDIO_ANC_TT_HW)
    if (anc_type == ANC_TALKTHRU) {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            //set the TT gain;
            codec->REG_230 &= ~CODEC_CODEC_MUTE_GAIN_UPDATE_TT_CH0;
            codec->REG_230 = SET_BITFIELD(codec->REG_230, CODEC_CODEC_MUTE_GAIN_COEF_TT_CH0, 512);
            hal_sys_timer_delay_us(1);
            codec->REG_230 |= CODEC_CODEC_MUTE_GAIN_UPDATE_TT_CH0;

            tt_ramp_gain_l = 0;
            tt_ramp_coef_l = 0;

            for (int i = 0; i < IIR_COUNTER; i++) {
                tt_filtes_l_old.iir_coef[i].coef_a[0] = 0;
                tt_filtes_l_old.iir_coef[i].coef_a[1] = 0;
                tt_filtes_l_old.iir_coef[i].coef_a[2] = 0;
                tt_filtes_l_old.iir_coef[i].coef_b[0] = 0;
                tt_filtes_l_old.iir_coef[i].coef_b[1] = 0;
                tt_filtes_l_old.iir_coef[i].coef_b[2] = 0;
            }

            tt_filtes_l_old.total_gain = 512;
            tt_filtes_l_old.iir_counter = IIR_TT_COUNTER;
            tt_filtes_l_old.iir_bypass_flag = 0;

            anc_iir1_init();

            codec->REG_22C |= CODEC_CODEC_TT_ENABLE_CH0;
        }
#endif
    }
#endif

    if (anc_type == ANC_FEEDBACK) {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            //set the FB gain;
            codec->REG_0D8 &= ~CODEC_CODEC_ANC_MUTE_GAIN_UPDATE_FB_CH0;
            codec->REG_0D8 = SET_BITFIELD(codec->REG_0D8, CODEC_CODEC_ANC_MUTE_GAIN_FB_CH0, 512);
            hal_sys_timer_delay_us(1);
            codec->REG_0D8 |= CODEC_CODEC_ANC_MUTE_GAIN_UPDATE_FB_CH0;

            fb_ramp_gain_l = 0;
            fb_ramp_coef_l = 0;

            for (int i = 0; i < IIR_COUNTER; i++) {
                fb_filtes_l_old.iir_coef[i].coef_a[0] = 0;
                fb_filtes_l_old.iir_coef[i].coef_a[1] = 0;
                fb_filtes_l_old.iir_coef[i].coef_a[2] = 0;
                fb_filtes_l_old.iir_coef[i].coef_b[0] = 0;
                fb_filtes_l_old.iir_coef[i].coef_b[1] = 0;
                fb_filtes_l_old.iir_coef[i].coef_b[2] = 0;
            }

            fb_filtes_l_old.total_gain = 512;
            fb_filtes_l_old.iir_counter = IIR_FB_COUNTER;
            fb_filtes_l_old.iir_bypass_flag = 0;

            anc_iir4_init();

#if defined(AUDIO_ANC_FB_MC) ||defined(AUDIO_ANC_FB_MC_HW)
            if (analog_debug_get_anc_calib_mode()) {
                codec->REG_130 |= CODEC_CODEC_FB_CHECK_ENABLE_CH0;
            }

            codec->REG_0DC |= CODEC_CODEC_FEEDBACK_MC_EN_CH0;
#endif
        }
#endif
    }

#if defined(AUDIO_ANC_FB_MC_HW)
    if (anc_type == ANC_MUSICCANCLE) {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            //set the MC gain;
            codec->REG_234 &= ~CODEC_CODEC_MUTE_GAIN_UPDATE_MM_CH0;
            codec->REG_234 = SET_BITFIELD(codec->REG_234, CODEC_CODEC_MUTE_GAIN_COEF_MM_CH0, 512);
            hal_sys_timer_delay_us(1);
            codec->REG_234 |= CODEC_CODEC_MUTE_GAIN_UPDATE_MM_CH0;

            mc_ramp_gain_l = 0;
            mc_ramp_coef_l = 0;

            for (int i = 0; i < IIR_COUNTER; i++)
            {
                mc_filtes_l_old.iir_coef[i].coef_a[0] = 0;
                mc_filtes_l_old.iir_coef[i].coef_a[1] = 0;
                mc_filtes_l_old.iir_coef[i].coef_a[2] = 0;
                mc_filtes_l_old.iir_coef[i].coef_b[0] = 0;
                mc_filtes_l_old.iir_coef[i].coef_b[1] = 0;
                mc_filtes_l_old.iir_coef[i].coef_b[2] = 0;
            }

            mc_filtes_l_old.total_gain = 512;
            mc_filtes_l_old.iir_counter = IIR_MC_COUNTER;
            mc_filtes_l_old.iir_bypass_flag = 0;

            anc_iir5_init();
#if defined(AUDIO_ANC_FB_MC_FIR)
            if (anc_fir_open_flag) {
                anc_fir_ctrl_reg_open(ANC_MUSICCANCLE, AUD_CHANNEL_MAP_CH0);
            }
#endif
            //set MC delay.
            codec->REG_22C |= CODEC_CODEC_MM_FIFO_BYPASS_CH0;
            codec->REG_22C |= CODEC_CODEC_MM_ENABLE_CH0;
        }
#endif
    }
#endif

    if (((ff_open_flag == 1) || (tt_open_flag == 1)) && (fb_open_flag == 1)){
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            codec->REG_0D0 |= CODEC_CODEC_FEEDBACK_CH0;
            codec->REG_0D0 |= CODEC_CODEC_DUAL_ANC_CH0;
        }
#endif

    } else if (fb_open_flag == 1) {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            codec->REG_0D0 |= CODEC_CODEC_FEEDBACK_CH0;
            codec->REG_0D0 &= ~CODEC_CODEC_DUAL_ANC_CH0;
        }
#endif
    } else if (fb_open_flag == 0) {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            codec->REG_0D0 &= ~CODEC_CODEC_FEEDBACK_CH0;
            codec->REG_0D0 &= ~CODEC_CODEC_DUAL_ANC_CH0;
        }
#endif
    }

    if ((ff_open_flag == 1) || (fb_open_flag == 1) || (tt_open_flag == 1)){
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
            if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
                codec->REG_0D0 |= CODEC_CODEC_ANC_ENABLE_CH0;
            }
#endif
    }

}

static void anc_ctrl_reg_close(enum ANC_TYPE_T anc_type)
{
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
        uint32_t iir_clk_value;

        if (anc_type == ANC_FEEDFORWARD) {
            //set the FF gain;
            codec->REG_0D4 &= ~CODEC_CODEC_ANC_MUTE_GAIN_UPDATE_FF_CH0;
            codec->REG_0D4 = SET_BITFIELD(codec->REG_0D4, CODEC_CODEC_ANC_MUTE_GAIN_FF_CH0, 0);
            hal_sys_timer_delay_us(1);
            codec->REG_0D4 |= CODEC_CODEC_ANC_MUTE_GAIN_UPDATE_FF_CH0;
        }
#if defined(AUDIO_ANC_TT_HW)
        if (anc_type == ANC_TALKTHRU) {
            //set the TT gain;
            codec->REG_230 &= ~CODEC_CODEC_MUTE_GAIN_UPDATE_TT_CH0;
            codec->REG_230 = SET_BITFIELD(codec->REG_230, CODEC_CODEC_MUTE_GAIN_COEF_TT_CH0, 0);
            hal_sys_timer_delay_us(1);
            codec->REG_230 |= CODEC_CODEC_MUTE_GAIN_UPDATE_TT_CH0;
        }
#endif
        if (anc_type == ANC_FEEDBACK) {
            //set the FB gain;
            codec->REG_0D8 &= ~CODEC_CODEC_ANC_MUTE_GAIN_UPDATE_FB_CH0;
            codec->REG_0D8 = SET_BITFIELD(codec->REG_0D8, CODEC_CODEC_ANC_MUTE_GAIN_FB_CH0, 0);
            hal_sys_timer_delay_us(1);
            codec->REG_0D8 |= CODEC_CODEC_ANC_MUTE_GAIN_UPDATE_FB_CH0;

#if defined(AUDIO_ANC_FB_MC) ||defined(AUDIO_ANC_FB_MC_HW)
            codec->REG_130 &= ~CODEC_CODEC_FB_CHECK_KEEP_CH0;
            codec->REG_0DC &= ~CODEC_CODEC_FEEDBACK_MC_EN_CH0;
#endif
        }
#if defined(AUDIO_ANC_FB_MC_HW)
        if (anc_type == ANC_MUSICCANCLE) {
            //set the TT gain;
            codec->REG_234 &= ~CODEC_CODEC_MUTE_GAIN_UPDATE_MM_CH0;
            codec->REG_234 = SET_BITFIELD(codec->REG_234, CODEC_CODEC_MUTE_GAIN_COEF_MM_CH0, 0);
            hal_sys_timer_delay_us(1);
            codec->REG_234 |= CODEC_CODEC_MUTE_GAIN_UPDATE_MM_CH0;
        }
#endif

        if (((ff_open_flag == 1) || (tt_open_flag == 1)) && (fb_open_flag == 1)){
            codec->REG_0D0 |= CODEC_CODEC_FEEDBACK_CH0;
            codec->REG_0D0 |= CODEC_CODEC_DUAL_ANC_CH0;
        } else if (fb_open_flag == 1) {
            codec->REG_0D0 |= CODEC_CODEC_FEEDBACK_CH0;
            codec->REG_0D0 &= ~CODEC_CODEC_DUAL_ANC_CH0;
        } else if (fb_open_flag == 0) {
            codec->REG_0D0 &= ~CODEC_CODEC_FEEDBACK_CH0;
            codec->REG_0D0 &= ~CODEC_CODEC_DUAL_ANC_CH0;
        }

        if ((ff_open_flag == 0) && (fb_open_flag == 0) && (tt_open_flag == 0)){
            codec->REG_0D0 &= ~CODEC_CODEC_ANC_ENABLE_CH0;
        }

        if (ff_open_flag == 0) {
            //disable iir0 clock
            iir_clk_value = GET_BITFIELD(codec->REG_06C, CODEC_EN_CLK_IIR_ANC);
            iir_clk_value &= ~CODEC_EN_CLK_IIR_IIR0;
            codec->REG_06C = SET_BITFIELD(codec->REG_06C, CODEC_EN_CLK_IIR_ANC, iir_clk_value);
        }
        if (tt_open_flag == 0) {
            //disable iir1 clock
            iir_clk_value = GET_BITFIELD(codec->REG_06C, CODEC_EN_CLK_IIR_ANC);
            iir_clk_value &= ~CODEC_EN_CLK_IIR_IIR1;
            codec->REG_06C = SET_BITFIELD(codec->REG_06C, CODEC_EN_CLK_IIR_ANC, iir_clk_value);
        }
        if (fb_open_flag == 0) {
            //disable iir4 clock
            iir_clk_value = GET_BITFIELD(codec->REG_06C, CODEC_EN_CLK_IIR_ANC);
            iir_clk_value &= ~CODEC_EN_CLK_IIR_IIR4;
            codec->REG_06C = SET_BITFIELD(codec->REG_06C, CODEC_EN_CLK_IIR_ANC, iir_clk_value);
        }
        if (mc_open_flag == 0) {
            //disable iir5 clock
            iir_clk_value = GET_BITFIELD(codec->REG_06C, CODEC_EN_CLK_IIR_ANC);
            iir_clk_value &= ~CODEC_EN_CLK_IIR_IIR5;
            codec->REG_06C = SET_BITFIELD(codec->REG_06C, CODEC_EN_CLK_IIR_ANC, iir_clk_value);
        }
    }
#endif

}

int anc_opened(enum ANC_TYPE_T anc_type)
{
    int32_t open_flag=0;

    if (anc_type & ANC_FEEDFORWARD){
        open_flag |= (ff_open_flag<<1);
    }
    if (anc_type & ANC_FEEDBACK){
        open_flag |= (fb_open_flag<<2);
    }
#if defined(AUDIO_ANC_TT_HW)
    if (anc_type & ANC_TALKTHRU){
        open_flag |= (tt_open_flag<<3);
    }
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    if (anc_type & ANC_MUSICCANCLE){
        open_flag |= (mc_open_flag<<4);
    }
#endif

    return open_flag;
}

#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK)|| defined(ANC_TT_CHECK)
static int32_t howling_check_delay = 5000; // recovery time ms.
static int32_t howling_timer = 400; // ms.
static int32_t howling_set_gain_level = 0; // ms.

#define HOWLING_GAIN_0dB (512)
#define HOWLING_GAIN_m3dB (362)
#define HOWLING_GAIN_m6dB (256)
#define HOWLING_GAIN_m9dB (181)
#define HOWLING_GAIN_m12dB (128)
static void anc_howling_check_irq_handler(uint32_t status)
{
    //DRIVERS_TRACE(1,"%s,", __func__);
    uint32_t howling_time=TICKS_TO_MS(hal_sys_timer_get());

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
        if (status & CODEC_FB_CHECK_ERROR_TRIG_CH0) {
            DRIVERS_TRACE(1,"howling_cnt_l:%d",howling_cnt_l);
            DRIVERS_TRACE(1,"*Threshold:%10d,Data Energy :%10d",
                GET_BITFIELD(codec->REG_138, CODEC_CODEC_FB_CHECK_THRESHOLD_CH0),
                GET_BITFIELD(codec->REG_140, CODEC_CODEC_FB_CHECK_DATA_AVG_KEEP_CH0));

            if (howling_cnt_l == 0) {
#if defined(ANC_FF_CHECK)
                ff_howling_gain_l = HOWLING_GAIN_m3dB;
#elif defined(ANC_TT_CHECK)
                tt_howling_gain_l = HOWLING_GAIN_m3dB;
#elif defined(ANC_FB_CHECK)
                fb_howling_gain_l = HOWLING_GAIN_m3dB;
#endif
                howling_cnt_l = 1;
                howling_time_l = howling_time;
            } else if ((howling_cnt_l == 1) && (howling_time > (howling_time_l + howling_timer))) {
#if defined(ANC_FF_CHECK)
                ff_howling_gain_l = HOWLING_GAIN_m6dB;
#elif defined(ANC_TT_CHECK)
                tt_howling_gain_l = HOWLING_GAIN_m6dB;
#elif defined(ANC_FB_CHECK)
                fb_howling_gain_l = HOWLING_GAIN_m6dB;
#endif
                howling_cnt_l = 2;
                howling_time_l = howling_time;
            } else if ((howling_cnt_l == 2) && (howling_time > (howling_time_l + howling_timer))) {
#if defined(ANC_FF_CHECK)
                ff_howling_gain_l = HOWLING_GAIN_m9dB;
#elif defined(ANC_TT_CHECK)
                tt_howling_gain_l = HOWLING_GAIN_m9dB;
#elif defined(ANC_FB_CHECK)
                fb_howling_gain_l = HOWLING_GAIN_m9dB;
#endif
                howling_cnt_l = 3;
                howling_time_l = howling_time;
            } else if ((howling_cnt_l == 3) && (howling_time > (howling_time_l + howling_timer))) {
#if defined(ANC_FF_CHECK)
                ff_howling_gain_l = 0;
#elif defined(ANC_TT_CHECK)
                tt_howling_gain_l = 0;
#elif defined(ANC_FB_CHECK)
                fb_howling_gain_l = 0;
#endif
                howling_cnt_l = 4;
            }
#if defined(ANC_FF_CHECK)
            DRIVERS_TRACE(1,"ff_howling_gain_l:%d", ff_howling_gain_l);
#elif defined(ANC_TT_CHECK)
            DRIVERS_TRACE(1,"tt_howling_gain_l:%d", tt_howling_gain_l);
#elif defined(ANC_FB_CHECK)
            DRIVERS_TRACE(1,"fb_howling_gain_l:%d", fb_howling_gain_l);
#endif

            codec->REG_130 &= ~CODEC_CODEC_FB_CHECK_ENABLE_CH0;

            hwtimer_stop(anc_howling_check_dev_timer_l);
            hwtimer_start(anc_howling_check_dev_timer_l, howling_check_delay);

#if defined(ANC_FF_CHECK)
            if (iir0_coef_using == 0) {
                iir0_gaina_cfg_gain();
            } else {
                iir0_gainb_cfg_gain();
            }
#elif defined(ANC_TT_CHECK)
            if (iir1_coef_using == 0) {
                iir1_gaina_cfg_gain();
            } else {
                iir1_gainb_cfg_gain();
            }
#elif defined(ANC_FB_CHECK)
            if (iir4_coef_using == 0) {
                iir4_gaina_cfg_gain();
            } else {
                iir4_gainb_cfg_gain();
            }
#endif
            codec->REG_130 |= CODEC_CODEC_FB_CHECK_ENABLE_CH0;
        }
    }
#endif
}

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
static void anc_left_howling_check_timer_handler(void *param)
{
    DRIVERS_TRACE(1,"%s", __func__);
#if defined(ANC_FF_CHECK)
    ff_howling_gain_l = HOWLING_GAIN_0dB;

    if (iir0_coef_using == 0) {
        iir0_gaina_cfg_gain();
    } else {
        iir0_gainb_cfg_gain();
    }
#elif defined(ANC_TT_CHECK)
    tt_howling_gain_l = HOWLING_GAIN_0dB;

    if (iir1_coef_using == 0) {
        iir1_gaina_cfg_gain();
    } else {
        iir1_gainb_cfg_gain();
    }
#elif defined(ANC_FB_CHECK)
    fb_howling_gain_l = HOWLING_GAIN_0dB;

    if (iir4_coef_using == 0) {
        iir4_gaina_cfg_gain();
    } else {
        iir4_gainb_cfg_gain();
    }
#endif
    howling_cnt_l = howling_set_gain_level;
    howling_time_l = 0;
}
#endif

#endif

int32_t anc_process_set_cfg(const anc_process_cfg * anc_cfg)
{
    const anc_howling_check_cfg * howling_check_cfg = &(anc_cfg->howling_check_cfg);
    DRIVERS_TRACE(1,"%s: recover_time=%d(ms), interval_time=%d, gain_level=%d", __func__,
                                            howling_check_cfg->howling_recover_time,
                                            howling_check_cfg->howling_interval_time,
                                            howling_check_cfg->howling_set_gain_level);

#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK)|| defined(ANC_TT_CHECK)
    howling_check_delay = ((MS_TO_TICKS(howling_check_cfg->howling_recover_time)));
    howling_timer = howling_check_cfg->howling_interval_time;
    howling_set_gain_level = howling_check_cfg->howling_set_gain_level;
#endif

    memcpy(&g_anc_bf_cfg, &(anc_cfg->bf_cfg), sizeof(anc_bf_cfg_t));
    DRIVERS_TRACE(4,"%s: bf_enable=%d, tt_delay_num=%d, tt_pdu_mix_flag=%d, tt_pdu_off_flag=%d", __func__,
                                                                                        g_anc_bf_cfg.enable,
                                                                                        g_anc_bf_cfg.tt_delay_num,
                                                                                        g_anc_bf_cfg.tt_pdu_mix_flag,
                                                                                        g_anc_bf_cfg.tt_pdu_off_flag);
    DRIVERS_TRACE(4,"%s: anc_ff_calib_gain=%d, anc_tt_calib_gain=%d, tt_delay_flag=%d", __func__,
                                                                                        g_anc_bf_cfg.anc_ff_calib_gain,
                                                                                        g_anc_bf_cfg.anc_tt_calib_gain,
                                                                                        g_anc_bf_cfg.tt_delay_flag);

    using_tt_as_ff_en = anc_cfg->using_tt_as_ff_en;
    return 0;
}

static void anc_ff_set_gain_timer_handler(void *param)
{
    DRIVERS_TRACE(1,"%s", __func__);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
#if defined(ANC_LINEAR_SMOOTH_CHECK)
        anc_iir0_linear_smooth_fade_flag = 1;
#endif
        if (iir0_coef_using == 0) {
            iir0_gaina_cfg_gain();
        } else {
            iir0_gainb_cfg_gain();
        }
    }
#endif
#if defined(ANC_IIR_LINEAR_SMOOTH_MODE)
    hwtimer_stop(anc_ff_switching_timer);
    hwtimer_start(anc_ff_switching_timer, anc_switching_coef_delay_ff);
#endif
}

#if defined(AUDIO_ANC_TT_HW)
static void anc_tt_set_gain_timer_handler(void *param)
{
    DRIVERS_TRACE(1,"%s", __func__);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
#if defined(ANC_LINEAR_SMOOTH_CHECK)
        anc_iir1_linear_smooth_fade_flag = 1;
#endif
        if(iir1_coef_using==0) {
            iir1_gaina_cfg_gain();
        } else {
            iir1_gainb_cfg_gain();
        }
    }
#endif
#if defined(ANC_IIR_LINEAR_SMOOTH_MODE)
    hwtimer_stop(anc_tt_switching_timer);
    hwtimer_start(anc_tt_switching_timer, anc_switching_coef_delay_tt);
#endif
    return;
}
#endif

static void anc_fb_set_gain_timer_handler(void *param)
{
    DRIVERS_TRACE(1,"%s", __func__);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
#if defined(ANC_LINEAR_SMOOTH_CHECK)
        anc_iir4_linear_smooth_fade_flag = 1;
#endif
        if (iir4_coef_using == 0) {
            iir4_gaina_cfg_gain();
        } else {
            iir4_gainb_cfg_gain();
        }
    }
#endif
#if defined(ANC_IIR_LINEAR_SMOOTH_MODE)
    hwtimer_stop(anc_fb_switching_timer);
    hwtimer_start(anc_fb_switching_timer, anc_switching_coef_delay_fb);
#endif
}


#if defined(AUDIO_ANC_FB_MC_HW)
static void anc_mc_set_gain_timer_handler(void *param)
{
    DRIVERS_TRACE(1,"%s", __func__);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
#if defined(ANC_LINEAR_SMOOTH_CHECK)
        anc_iir5_linear_smooth_fade_flag = 1;
#endif
        if (iir5_coef_using == 0) {
            iir5_gaina_cfg_gain();
        } else {
            iir5_gainb_cfg_gain();
        }
    }
#endif
#if defined(ANC_IIR_LINEAR_SMOOTH_MODE)
    hwtimer_stop(anc_mc_switching_timer);
    hwtimer_start(anc_mc_switching_timer, anc_switching_coef_delay_mc);
#endif
}
#endif
static void anc_ff_switching_timer_handler(void *param)
{
    DRIVERS_TRACE(1,"%s", __func__);

    uint32_t lock;
    lock = int_lock();

    bool ff_reserve_flag = anc_ff_reserve_flag;

    if (anc_ff_reserve_flag) {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            anc_cfg_coef.anc_cfg_ff_l = anc_cfg_coef_new.anc_cfg_ff_l;
        }
#endif
        anc_ff_reserve_flag = 0;
    } else {
        anc_ff_using_flag = 0;
    }
    int_unlock(lock);

    if (ff_reserve_flag) {
        anc_set_cfg_internal(&anc_cfg_coef, ANC_FEEDFORWARD);
#if !defined(ANC_IIR_LINEAR_SMOOTH_MODE)
        hwtimer_stop(anc_ff_switching_timer);
        hwtimer_start(anc_ff_switching_timer, anc_switching_coef_delay_ff);
#endif
    }
    return;
}

#if defined(AUDIO_ANC_TT_HW)
static void anc_tt_switching_timer_handler(void *param)
{
    DRIVERS_TRACE(1,"%s", __func__);

    uint32_t lock;
    lock = int_lock();

    bool tt_reserve_flag = anc_tt_reserve_flag;

    if (anc_tt_reserve_flag) {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            anc_cfg_coef.anc_cfg_tt_l = anc_cfg_coef_new.anc_cfg_tt_l;
        }
#endif
        anc_tt_reserve_flag = 0;
    } else {
        anc_tt_using_flag = 0;
    }
    int_unlock(lock);

    if (tt_reserve_flag) {
        anc_set_cfg_internal(&anc_cfg_coef, ANC_TALKTHRU);
#if !defined(ANC_IIR_LINEAR_SMOOTH_MODE)
        hwtimer_stop(anc_tt_switching_timer);
        hwtimer_start(anc_tt_switching_timer, anc_switching_coef_delay_tt);
#endif
    }
    return;
}
#endif

static void anc_fb_switching_timer_handler(void *param)
{
    DRIVERS_TRACE(1,"%s", __func__);
    uint32_t lock;
    lock = int_lock();

    bool fb_reserve_flag = anc_fb_reserve_flag;

    if (anc_fb_reserve_flag) {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            anc_cfg_coef.anc_cfg_fb_l = anc_cfg_coef_new.anc_cfg_fb_l;
        }
#endif
        anc_fb_reserve_flag = 0;
    } else {
        anc_fb_using_flag = 0;
    }
    int_unlock(lock);

    if (fb_reserve_flag) {
        anc_set_cfg_internal(&anc_cfg_coef, ANC_FEEDBACK);
#if !defined(ANC_IIR_LINEAR_SMOOTH_MODE)
        hwtimer_stop(anc_fb_switching_timer);
        hwtimer_start(anc_fb_switching_timer, anc_switching_coef_delay_fb);
#endif
    }
    return;
}

#if defined(AUDIO_ANC_FB_MC_HW)
static void anc_mc_switching_timer_handler(void *param)
{
    DRIVERS_TRACE(1,"%s", __func__);
    uint32_t lock;
    lock = int_lock();

    bool mc_reserve_flag = anc_mc_reserve_flag;

    if (anc_mc_reserve_flag) {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            anc_cfg_coef.anc_cfg_mc_l = anc_cfg_coef_new.anc_cfg_mc_l;
        }
#endif
        anc_mc_reserve_flag = 0;
    } else {
        anc_mc_using_flag = 0;
    }
    int_unlock(lock);

    if (mc_reserve_flag) {
        anc_set_cfg_internal(&anc_cfg_coef, ANC_MUSICCANCLE);
#if !defined(ANC_IIR_LINEAR_SMOOTH_MODE)
        hwtimer_stop(anc_mc_switching_timer);
        hwtimer_start(anc_mc_switching_timer, anc_switching_coef_delay_mc);
#endif
    }
    return;
}
#endif
#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK) || defined(ANC_TT_CHECK)
static void anc_ff_fb_tt_check_init(void)
{
    hal_codec_anc_fb_check_set_irq_handler(anc_howling_check_irq_handler);

    fb_check_open_flag = 1;

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)

    codec->REG_130 &= ~CODEC_CODEC_FB_CHECK_ENABLE_CH0;
    codec->REG_00C = CODEC_FB_CHECK_ERROR_TRIG_CH0;

    if (analog_debug_get_anc_calib_mode()) {
        codec->REG_010 &= ~CODEC_FB_CHECK_ERROR_TRIG_CH0_MSK;
    } else {
        codec->REG_010 |= CODEC_FB_CHECK_ERROR_TRIG_CH0_MSK;
    }
    codec->REG_0DC |= CODEC_CODEC_FEEDBACK_MC_EN_CH0;

    codec->REG_130 = SET_BITFIELD(codec->REG_130, CODEC_CODEC_FB_CHECK_ACC_SAMPLE_RATE_CH0, 3);
#if defined(ANC_FB_CHECK)
    codec->REG_130 = SET_BITFIELD(codec->REG_130, CODEC_CODEC_FB_CHECK_SRC_SEL_CH0, 0);
#elif defined(ANC_FF_CHECK)
    codec->REG_130 = SET_BITFIELD(codec->REG_130, CODEC_CODEC_FB_CHECK_SRC_SEL_CH0, 2);
#elif defined(ANC_TT_CHECK)
    codec->REG_130 = SET_BITFIELD(codec->REG_130, CODEC_CODEC_FB_CHECK_SRC_SEL_CH0, 2);
    codec->REG_33C |= CODEC_CODEC_PDU_MIX_EN_CH0;
#endif
    codec->REG_130 = SET_BITFIELD(codec->REG_130, CODEC_CODEC_FB_CHECK_ACC_WINDOW_CH0, 64);
    codec->REG_130 = SET_BITFIELD(codec->REG_130, CODEC_CODEC_FB_CHECK_TRIG_WINDOW_CH0, 64);

    codec->REG_138 = SET_BITFIELD(codec->REG_138, CODEC_CODEC_FB_CHECK_THRESHOLD_CH0, ANC_HOWLING_THRESHOLD_0dB);
    codec->REG_130 |= CODEC_CODEC_FB_CHECK_ENABLE_CH0;

    ff_howling_gain_l = 512;
    fb_howling_gain_l = 512;

    howling_cnt_l = howling_set_gain_level;
    howling_time_l = 0;
#endif
    return;
}
#endif
int anc_open(enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err = ANC_NO_ERR;

    DRIVERS_TRACE(1,"%s", __func__);

    if ((anc_type == ANC_FEEDFORWARD) && (ff_open_flag == 1))return err;
    if ((anc_type == ANC_FEEDBACK) && (fb_open_flag == 1))return err;
#if defined(AUDIO_ANC_TT_HW)
    if ((anc_type == ANC_TALKTHRU) && (tt_open_flag == 1))return err;
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    if ((anc_type == ANC_MUSICCANCLE) && (mc_open_flag == 1))return err;
#endif

    if (anc_ff_set_gain_timer == NULL) {
        anc_ff_set_gain_timer = hwtimer_alloc(anc_ff_set_gain_timer_handler, NULL);
        ASSERT(anc_ff_set_gain_timer, "Failed to alloc anc_ff_set_gain_timer");
    }
#if defined(AUDIO_ANC_TT_HW)
    if (anc_tt_set_gain_timer == NULL) {
        anc_tt_set_gain_timer = hwtimer_alloc(anc_tt_set_gain_timer_handler, NULL);
        ASSERT(anc_tt_set_gain_timer, "Failed to alloc anc_tt_set_gain_timer");
    }
#endif
    if (anc_fb_set_gain_timer == NULL) {
        anc_fb_set_gain_timer = hwtimer_alloc(anc_fb_set_gain_timer_handler, NULL);
        ASSERT(anc_fb_set_gain_timer, "Failed to alloc anc_fb_set_gain_timer");
    }
#if defined(AUDIO_ANC_FB_MC_HW)
    if (anc_mc_set_gain_timer == NULL) {
        anc_mc_set_gain_timer = hwtimer_alloc(anc_mc_set_gain_timer_handler, NULL);
        ASSERT(anc_mc_set_gain_timer, "Failed to alloc anc_mc_set_gain_timer");
    }
#endif

    if(anc_ff_switching_timer == NULL) {
        anc_ff_switching_timer = hwtimer_alloc(anc_ff_switching_timer_handler, NULL);
        ASSERT(anc_ff_switching_timer, "Failed to alloc anc_ff_switching_timer");
    }
#if defined(AUDIO_ANC_TT_HW)
    if (anc_tt_switching_timer == NULL) {
        anc_tt_switching_timer = hwtimer_alloc(anc_tt_switching_timer_handler, NULL);
        ASSERT(anc_tt_switching_timer, "Failed to alloc anc_tt_switching_timer");
    }
#endif
    if (anc_fb_switching_timer == NULL) {
        anc_fb_switching_timer = hwtimer_alloc(anc_fb_switching_timer_handler, NULL);
        ASSERT(anc_fb_switching_timer, "Failed to alloc anc_fb_switching_timer");
    }
#if defined(AUDIO_ANC_FB_MC_HW)
    if (anc_mc_switching_timer == NULL) {
        anc_mc_switching_timer = hwtimer_alloc(anc_mc_switching_timer_handler, NULL);
        ASSERT(anc_mc_switching_timer, "Failed to anc_mc_switching_timer");
    }
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK) || defined(ANC_TT_CHECK)
    if (anc_howling_check_dev_timer_l == NULL) {
        anc_howling_check_dev_timer_l = hwtimer_alloc(anc_left_howling_check_timer_handler, NULL);
        ASSERT(anc_howling_check_dev_timer_l, "Failed to alloc usbdev_timer");
    }
#endif
#endif

    if ((ff_open_flag == 0)
            && (fb_open_flag == 0)
#if defined(AUDIO_ANC_TT_HW)
            && (tt_open_flag == 0)
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
            && (mc_open_flag == 0)
#endif
      ) {
        if ((IIR_FF_COUNTER > 6)
            || (IIR_TT_COUNTER > 6)
            || (IIR_FB_COUNTER > 6)
            || (IIR_MC_COUNTER > 6)
        ) {
            if (hal_cmu_get_audio_resample_status()) {
                hal_codec_iir_enable(48000000);
            } else {
                hal_codec_iir_enable(24576000*2);
            }
        } else {
            if (hal_cmu_get_audio_resample_status()) {
                hal_codec_iir_enable(24000000);
            } else {
                hal_codec_iir_enable(24576000);
            }
        }

        anc_ctrl_reg_init();

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        iir0_coef_using = 0;
#ifdef AUDIO_ANC_TT_HW
        iir1_coef_using = 0;
#endif
        iir4_coef_using = 0;
        iir5_coef_using = 0;
#if defined(ANC_LINEAR_SMOOTH_CHECK)
        anc_iir0_linear_smooth_fade_flag = 0;
#ifdef AUDIO_ANC_TT_HW
        anc_iir1_linear_smooth_fade_flag = 0;
#endif
        anc_iir4_linear_smooth_fade_flag = 0;
        anc_iir5_linear_smooth_fade_flag = 0;
#endif
#endif
    }

#if defined(ANC_FF_CHECK)
    if (anc_type == ANC_FEEDFORWARD) {
        anc_ff_fb_tt_check_init();
    }
#elif defined(ANC_TT_CHECK)
    if (anc_type == ANC_TALKTHRU) {
        anc_ff_fb_tt_check_init();
    }
#elif defined(ANC_FB_CHECK)
    if (anc_type == ANC_FEEDBACK) {
        anc_ff_fb_tt_check_init();
    }
#endif

    if (anc_type == ANC_FEEDFORWARD) {
        ff_open_flag = 1;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            max_ff_gain_l = (1 << GAIN_Q);
        }
#endif
    }

#if defined(AUDIO_ANC_TT_HW)
    if (anc_type == ANC_TALKTHRU) {
        tt_open_flag = 1;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            max_tt_gain_l = (1 << GAIN_Q);
        }
#endif
    }
#endif

    if (anc_type == ANC_FEEDBACK) {
        fb_open_flag = 1;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            max_fb_gain_l = (1 << GAIN_Q);
        }
#endif
    }
#if defined(AUDIO_ANC_FB_MC_HW)
    if (anc_type == ANC_MUSICCANCLE) {
        mc_open_flag = 1;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            max_mc_gain_l = (1<<GAIN_Q);
        }
#endif
    }
#endif

    anc_ctrl_reg_open(anc_type);

    return err;
}

#if defined(AUDIO_ANC_FIR_HW)
void anc_fir_ctrl_reg_close(void)
{
    DRIVERS_TRACE(1, "%s ...", __func__);

    codec->REG_07C = SET_BITFIELD(codec->REG_07C, CODEC_SOFT_RSTN_FIR, 0x0);
    hal_codec_fir_disable();
    codec->REG_06C = SET_BITFIELD(codec->REG_06C, CODEC_EN_CLK_FIR, 0x0);

    ff_l_fir_open_flag = 0;
}
#endif

void anc_close(enum ANC_TYPE_T anc_type)
{
    if (anc_type == ANC_FEEDFORWARD) {
        DRIVERS_TRACE(1,"%s: ANC_FEEDFORWARD",__func__);

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            max_ff_gain_l = 0;
            ff_ramp_gain_l = 0;
#if defined(ANC_FF_CHECK)
            codec->REG_130 &= ~CODEC_CODEC_FB_CHECK_ENABLE_CH0;
            codec->REG_00C = CODEC_FB_CHECK_ERROR_TRIG_CH0;
            codec->REG_010 &= ~CODEC_FB_CHECK_ERROR_TRIG_CH0_MSK;
            ff_howling_gain_l = 0;
#endif
        }
#endif
        ff_open_flag = 0;
#if defined(ANC_FF_CHECK)
        fb_check_open_flag = 0;
#endif
    }
#if defined(AUDIO_ANC_TT_HW)
    if (anc_type == ANC_TALKTHRU) {
        DRIVERS_TRACE(1,"%s: ANC_TALKTHRU",__func__);

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            max_tt_gain_l = 0;
            tt_ramp_gain_l = 0;
#if defined(ANC_TT_CHECK)
            codec->REG_130 &= ~CODEC_CODEC_FB_CHECK_ENABLE_CH0;
            codec->REG_00C = CODEC_FB_CHECK_ERROR_TRIG_CH0;
            codec->REG_010 &= ~CODEC_FB_CHECK_ERROR_TRIG_CH0_MSK;
            tt_howling_gain_l = 0;
#endif
        }
#endif
        tt_open_flag = 0;
#if defined(ANC_TT_CHECK)
        fb_check_open_flag = 0;
#endif

    }
#endif

    if (anc_type == ANC_FEEDBACK) {
        DRIVERS_TRACE(1,"%s: ANC_FEEDBACK",__func__);

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            max_fb_gain_l = 0;
            fb_ramp_gain_l = 0;

#if defined(ANC_FB_CHECK)
            codec->REG_130 &= ~CODEC_CODEC_FB_CHECK_ENABLE_CH0;
            codec->REG_00C = CODEC_FB_CHECK_ERROR_TRIG_CH0;
            codec->REG_010 &= ~CODEC_FB_CHECK_ERROR_TRIG_CH0_MSK;
            fb_howling_gain_l = 0;
#endif
        }
#endif

#if defined(ANC_FB_CHECK)
        fb_check_open_flag = 0;
#endif
        fb_open_flag = 0;
    }

#if defined(AUDIO_ANC_FB_MC_HW)
    if (anc_type == ANC_MUSICCANCLE) {
        DRIVERS_TRACE(1,"%s: ANC_MUSICCANCLE",__func__);

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            max_mc_gain_l = 0;
            mc_ramp_gain_l = 0;
        }
#endif
        mc_open_flag = 0;
    }
#endif

    anc_ctrl_reg_close(anc_type);

    if ((ff_open_flag == 0)
            && (fb_open_flag==0)
#if defined(AUDIO_ANC_TT_HW)
            && (tt_open_flag==0)
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
            && (mc_open_flag==0)
#endif
      ) {
        hwtimer_stop(anc_ff_set_gain_timer);
        hwtimer_stop(anc_fb_set_gain_timer);
        hwtimer_stop(anc_ff_switching_timer);
        hwtimer_stop(anc_fb_switching_timer);
#if defined(AUDIO_ANC_FIR_HW)
        if (anc_fir_open_flag) {
            anc_fir_ctrl_reg_close();
            anc_fir_open_flag = 0;
        }
#endif

        hal_codec_iir_disable();
        codec->REG_06C = SET_BITFIELD(codec->REG_06C, CODEC_EN_CLK_IIR_ANC, 0x0);
    }

    return;
}

int anc_enable( void)
{
    DRIVERS_TRACE(1,"%s", __func__);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
        codec->REG_0D0 |= CODEC_CODEC_ANC_ENABLE_CH0;

#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK) || defined(ANC_TT_CHECK)
        codec->REG_130 |= CODEC_CODEC_FB_CHECK_ENABLE_CH0;
#endif
#if defined(AUDIO_ANC_TT_HW)
        codec->REG_22C |= CODEC_CODEC_TT_ENABLE_CH0;
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
        codec->REG_22C |= CODEC_CODEC_MM_ENABLE_CH0;
#endif
    }
#endif

    return ANC_NO_ERR;
}

int anc_disable(void)
{
    DRIVERS_TRACE(1,"%s", __func__);

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0) {
#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK) || defined(ANC_TT_CHECK)
        codec->REG_0D0 &= ~CODEC_CODEC_ANC_ENABLE_CH0;
        hwtimer_stop(anc_howling_check_dev_timer_l);
#if defined(ANC_TT_CHECK)
        codec->REG_33C &= ~CODEC_CODEC_PDU_MIX_EN_CH0;
#endif
#endif
    }
#endif

    return ANC_NO_ERR;
}


int anc_set_ff_and_tt_gain(int32_t gain_ff_l, int32_t gain_ff_r,int32_t gain_tt_l, int32_t gain_tt_r)
{
    ANC_ERROR err=ANC_NO_ERR;

    //if(gain_ch_l==511||gain_ch_l==0||gain_ch_l==250)
    {
        DRIVERS_TRACE(1,"gain_ff_l:%d,gain_tt_l:%d", gain_ff_l, gain_tt_l);
    }

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
        ff_ramp_gain_l = gain_ff_l;
        if (iir0_coef_using == 0) {
            iir0_gaina_cfg_gain();
        } else {
            iir0_gainb_cfg_gain();
        }
#if defined(AUDIO_ANC_TT_HW)
        tt_ramp_gain_l = gain_tt_l;
        if (iir1_coef_using == 0) {
            iir1_gaina_cfg_gain();
        } else {
            iir1_gainb_cfg_gain();
        }
#endif
    }
#endif

    return err;
}

static float g_ff_adaptive_gain = 1;
int anc_set_gain2_float(float gain)
{
    if(gain > 1) {
        DRIVERS_TRACE(2,"[%s] warning input value = %d/100 > 1",__func__,(int)(gain*100));
        g_ff_adaptive_gain = 1;

    } else if(gain < 0) {
        DRIVERS_TRACE(2,"[%s] warning input value = %d/100 < 1",__func__,(int)(gain*100));
        g_ff_adaptive_gain = 0;
    } else {
        g_ff_adaptive_gain = gain;
    }
    return 0;
}

float anc_get_gain2_float(void)
{
    return g_ff_adaptive_gain;
}


int anc_set_gain(int32_t gain_ch_l, int32_t gain_ch_r,enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err = ANC_NO_ERR;

    if ((gain_ch_l == 511) || (gain_ch_l == 0) || (gain_ch_l == 250)) {
        DRIVERS_TRACE(1,"anc_set_gain anc_type:%d, gain_ch_l:%d,gain_ch_r:%d", anc_type, gain_ch_l, gain_ch_r);
    }

    if (((anc_type==ANC_FEEDFORWARD) && (ff_open_flag==0)) || ((anc_type==ANC_FEEDBACK) && (fb_open_flag==0))
     || ((anc_type==ANC_TALKTHRU) && (tt_open_flag==0)) || ((anc_type==ANC_MUSICCANCLE) && (mc_open_flag==0))) {
        DRIVERS_TRACE(1,"%s: anc_type=%d not opened", __func__, anc_type);
        return ANC_OTHER_ERR;
    }

    //DRIVERS_TRACE(1,"iir0_iira_stop_status_sync:%d,iir0_iirb_stop_status_sync:%d",anc_iir0_control->codec_iir0_iira_stop_status_sync,anc_iir0_control->codec_iir0_iirb_stop_status_sync);
    //DRIVERS_TRACE(1,"%s: iir0_coef_using:%d,iir1_coef_using:%d", __func__,iir0_coef_using,iir1_coef_using);

    if (anc_type & ANC_FEEDFORWARD) {
        gain_ch_l = (uint32_t)(gain_ch_l * g_ff_adaptive_gain);
        gain_ch_r = (uint32_t)(gain_ch_r * g_ff_adaptive_gain);
        // DRIVERS_TRACE(0,"!!!!!!! type =% d internal gain = %d coef = %d",anc_type,gain_ch_l,(int)(100*g_ff_adaptive_gain));
    }

    if (anc_type & ANC_FEEDFORWARD) {
        //Set the FF gain;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            ff_ramp_gain_l = gain_ch_l;
            if(iir0_coef_using == 0) {
                iir0_gaina_cfg_gain();
            } else {
                iir0_gainb_cfg_gain();
            }
        }
#endif
    }

#if defined(AUDIO_ANC_TT_HW)
    if (anc_type & ANC_TALKTHRU) {
        //Set the TT gain;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            tt_ramp_gain_l = gain_ch_l;
            if (iir1_coef_using == 0) {
                iir1_gaina_cfg_gain();
            } else {
                iir1_gainb_cfg_gain();
            }
        }
#endif
    }
#endif

    if (anc_type & ANC_FEEDBACK) {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        //Set the FB gain;
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            fb_ramp_gain_l = gain_ch_l;
            if (iir4_coef_using == 0) {
                iir4_gaina_cfg_gain();
            } else {
                iir4_gainb_cfg_gain();
            }
        }
#endif
    }

#if defined(AUDIO_ANC_FB_MC_HW)
    if (anc_type & ANC_MUSICCANCLE) {
        //Set the MC gain;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            mc_ramp_gain_l = gain_ch_l;
            if (iir5_coef_using == 0) {
                iir5_gaina_cfg_gain();
            } else {
                iir5_gainb_cfg_gain();
            }
        }
#endif
    }
#endif

    return err;
}

int anc_set_gain_f32(float gain_l, float gain_r, enum ANC_TYPE_T type)
{
    return anc_set_gain((int32_t)(gain_l * 512), (int32_t)(gain_r * 512), type);
}

int anc_get_gain(int32_t *gain_ch_l, int32_t *gain_ch_r,enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err = ANC_NO_ERR;
    *gain_ch_l = 0;
    *gain_ch_r = 0;

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
        if (anc_type & ANC_FEEDFORWARD) {
            //Get the FF gain;
            *gain_ch_l = ff_ramp_gain_l;
            *gain_ch_r = ff_ramp_gain_l;
        }
        if (anc_type & ANC_FEEDBACK) {
            //Get the FB gain;
            *gain_ch_l = fb_ramp_gain_l;
            *gain_ch_r = fb_ramp_gain_l;
        }
#if defined(AUDIO_ANC_TT_HW)
        if (anc_type & ANC_TALKTHRU) {
            //Set the TT gain;
            *gain_ch_l = tt_ramp_gain_l;
            *gain_ch_r = tt_ramp_gain_l;
        }
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
        if (anc_type & ANC_MUSICCANCLE) {
            //Set the MC gain;
            *gain_ch_l = mc_ramp_gain_l;
            *gain_ch_r = mc_ramp_gain_l;
        }
#endif
    }
#endif

    //  DRIVERS_TRACE(1,"anc_get_gain gain_ch_l:%d,gain_ch_r:%d",*gain_ch_l,*gain_ch_r);
    return err;
}

int anc_get_cfg_gain(int32_t *gain_ch_l, int32_t *gain_ch_r,enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err = ANC_NO_ERR;
    *gain_ch_l = 512;
    *gain_ch_r = 512;

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
        if (anc_type & ANC_FEEDFORWARD) {
            //Get the max FF gain;
            *gain_ch_l = max_ff_gain_l;
            *gain_ch_r = max_ff_gain_l;
        }
        if (anc_type & ANC_FEEDBACK) {
            //Get the max FB gain;
            *gain_ch_l = max_fb_gain_l;
            *gain_ch_r = max_fb_gain_l;
        }
#if defined(AUDIO_ANC_TT_HW)
        if (anc_type & ANC_TALKTHRU) {
            //Get the TT gain;
            *gain_ch_l = max_tt_gain_l;
            *gain_ch_r = max_tt_gain_l;
        }
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
        if (anc_type & ANC_MUSICCANCLE) {
            //Get the MC gain;
            *gain_ch_l = max_mc_gain_l;
            *gain_ch_r = max_mc_gain_l;
        }
#endif
    }
#endif

    //DRIVERS_TRACE(1,"anc_get_cfg_gain gain_ch_l:%d,gain_ch_r:%d",*gain_ch_l,*gain_ch_r);
    return err;
}

void  anc_set_ch_map( int32_t ch_map )
{
    anc_output_ch_map = ch_map;

    return;
}

void  anc_howling_check_enable(int32_t flag )
{
    uint32_t lock;

    DRIVERS_TRACE(1,"%s flag:%d", __func__,flag);

    lock = int_lock();
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
#if defined(ANC_FF_CHECK) || defined(ANC_FB_CHECK) || defined(ANC_TT_CHECK)
        if (fb_check_open_flag == 1) {
            if (flag == 0) {
                codec->REG_130 &= ~CODEC_CODEC_FB_CHECK_ENABLE_CH0;
                codec->REG_010 &= ~CODEC_FB_CHECK_ERROR_TRIG_CH0_MSK;
                codec->REG_00C = CODEC_FB_CHECK_ERROR_TRIG_CH0;
#if defined(ANC_TT_CHECK)
                codec->REG_33C &= ~CODEC_CODEC_PDU_MIX_EN_CH0;
#endif
            } else {
                codec->REG_00C = CODEC_FB_CHECK_ERROR_TRIG_CH0;
                codec->REG_010 |= CODEC_FB_CHECK_ERROR_TRIG_CH0_MSK;
                codec->REG_130 |= CODEC_CODEC_FB_CHECK_ENABLE_CH0;
#if defined(ANC_TT_CHECK)
                codec->REG_33C |= CODEC_CODEC_PDU_MIX_EN_CH0;
#endif
            }
        }
#endif
    }
#endif

    int_unlock(lock);

    return;
}

void anc_howling_gain_reset(void)
{
    DRIVERS_TRACE(1,"%s", __func__);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
#if defined(ANC_FF_CHECK)
        ff_howling_gain_l = HOWLING_GAIN_0dB;
        if (iir0_coef_using == 0) {
            iir0_gaina_cfg_gain();
        } else {
            iir0_gainb_cfg_gain();
        }
#elif defined(ANC_TT_CHECK)
        tt_howling_gain_l = HOWLING_GAIN_0dB;
        if (iir1_coef_using == 0) {
            iir1_gaina_cfg_gain();
        } else {
            iir1_gainb_cfg_gain();
        }
#elif defined(ANC_FB_CHECK)
        fb_howling_gain_l = HOWLING_GAIN_0dB;
        if (iir4_coef_using == 0) {
            iir4_gaina_cfg_gain();
        } else {
            iir4_gainb_cfg_gain();
        }
#endif
    }
#endif
}


int anc_set_switching_delay(ANC_SWITCHING_DELAY  anc_switching_delay,enum ANC_TYPE_T anc_type)
{
    anc_iir_coefs  iir_coef_gain_ramp;
    int anc_switching_coef_delay;
    int anc_switching_gain_delay;


    DRIVERS_TRACE(1,"%s:anc_switching_delay:%d,anc_type:%d", __func__,anc_switching_delay,anc_type);

    uint32_t lock;
    lock = int_lock();

   switch(anc_switching_delay) {
        case ANC_SWITCHING_DELAY_50ms:
        iir_coef_gain_ramp = iir_coef_gain_ramp_delay1;
        anc_switching_gain_delay = ANC_SET_GAIN_DELAY_LONG/8;
        anc_switching_coef_delay = ANC_SET_GAIN_TIME_DELAY1 + ANC_SET_GAIN_DELAY_LONG/8;
        break;

        case ANC_SWITCHING_DELAY_100ms:
        iir_coef_gain_ramp = iir_coef_gain_ramp_delay2;
        anc_switching_gain_delay = ANC_SET_GAIN_DELAY_LONG/4;
        anc_switching_coef_delay = ANC_SET_GAIN_TIME_DELAY2 + ANC_SET_GAIN_DELAY_LONG/4;
        break;

        case ANC_SWITCHING_DELAY_200ms:
        iir_coef_gain_ramp = iir_coef_gain_ramp_delay3;
        anc_switching_gain_delay = ANC_SET_GAIN_DELAY_LONG/4;
        anc_switching_coef_delay = ANC_SET_GAIN_TIME_DELAY3 + ANC_SET_GAIN_DELAY_LONG/4;
        break;

        case ANC_SWITCHING_DELAY_450ms:
        iir_coef_gain_ramp = iir_coef_gain_ramp_delay4;
        anc_switching_gain_delay = ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay = ANC_SET_GAIN_TIME_DELAY4 + ANC_SET_GAIN_DELAY_LONG;
        break;

        case ANC_SWITCHING_DELAY_650ms:
        iir_coef_gain_ramp = iir_coef_gain_ramp_delay5;
        anc_switching_gain_delay = ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay = ANC_SET_GAIN_TIME_DELAY5 + ANC_SET_GAIN_DELAY_LONG;
        break;

        case ANC_SWITCHING_DELAY_850ms:
        iir_coef_gain_ramp = iir_coef_gain_ramp_delay6;
        anc_switching_gain_delay = ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay = ANC_SET_GAIN_TIME_DELAY6 + ANC_SET_GAIN_DELAY_LONG;
        break;

        case ANC_SWITCHING_DELAY_1300ms:
        iir_coef_gain_ramp = iir_coef_gain_ramp_delay7;
        anc_switching_gain_delay = ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay = ANC_SET_GAIN_TIME_DELAY7 + ANC_SET_GAIN_DELAY_LONG;
        break;

        case ANC_SWITCHING_DELAY_1600ms:
        iir_coef_gain_ramp = iir_coef_gain_ramp_delay8;
        anc_switching_gain_delay = ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay = ANC_SET_GAIN_TIME_DELAY8 + ANC_SET_GAIN_DELAY_LONG;
        break;

        case ANC_SWITCHING_DELAY_1900ms:
        iir_coef_gain_ramp = iir_coef_gain_ramp_delay9;
        anc_switching_gain_delay = ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay = ANC_SET_GAIN_TIME_DELAY9 + ANC_SET_GAIN_DELAY_LONG;
        break;

        case ANC_SWITCHING_DELAY_2200ms:
        iir_coef_gain_ramp = iir_coef_gain_ramp_delay10;
        anc_switching_gain_delay = ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay = ANC_SET_GAIN_TIME_DELAY10 + ANC_SET_GAIN_DELAY_LONG;
        break;

        case ANC_SWITCHING_DELAY_2600ms:
        iir_coef_gain_ramp = iir_coef_gain_ramp_delay11;
        anc_switching_gain_delay = ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay = ANC_SET_GAIN_TIME_DELAY11 + ANC_SET_GAIN_DELAY_LONG;
        break;

        default:
        iir_coef_gain_ramp = iir_coef_gain_ramp_delay4;
        anc_switching_gain_delay = ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay = ANC_SET_GAIN_TIME_DELAY4 + ANC_SET_GAIN_DELAY_LONG;
        break;
   }

    if (anc_type & ANC_FEEDFORWARD) {
        anc_switching_gain_delay_ff = anc_switching_gain_delay;
        anc_switching_coef_delay_ff = anc_switching_coef_delay;

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        codec->REG_248 &= ~CODEC_CODEC_IIR0_ENABLE;
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            anc_gain_ff_iir_coefs0_l[0].a1 = -iir_coef_gain_ramp.coef_a[1];
            anc_gain_ff_iir_coefs0_l[0].a2 = -iir_coef_gain_ramp.coef_a[2];
            anc_gain_ff_iir_coefs0_l[0].b0 = iir_coef_gain_ramp.coef_b[0];
            anc_gain_ff_iir_coefs0_l[0].b1 = iir_coef_gain_ramp.coef_b[1];
            anc_gain_ff_iir_coefs0_l[0].b2 = iir_coef_gain_ramp.coef_b[2];

            anc_gain_ff_iir_coefs1_l[0].a1 = -iir_coef_gain_ramp.coef_a[1];
            anc_gain_ff_iir_coefs1_l[0].a2 = -iir_coef_gain_ramp.coef_a[2];
            anc_gain_ff_iir_coefs1_l[0].b0 = iir_coef_gain_ramp.coef_b[0];
            anc_gain_ff_iir_coefs1_l[0].b1 = iir_coef_gain_ramp.coef_b[1];
            anc_gain_ff_iir_coefs1_l[0].b2 = iir_coef_gain_ramp.coef_b[2];
        }
        codec->REG_248 |= CODEC_CODEC_IIR0_ENABLE;
#endif
    }
#if defined(AUDIO_ANC_TT_HW)
    if (anc_type & ANC_TALKTHRU ) {
        anc_switching_gain_delay_tt = anc_switching_gain_delay;
        anc_switching_coef_delay_tt = anc_switching_coef_delay;

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        codec->REG_24C &= ~CODEC_CODEC_IIR1_ENABLE;
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            anc_gain_tt_iir_coefs0_l[0].a1 = -iir_coef_gain_ramp.coef_a[1];
            anc_gain_tt_iir_coefs0_l[0].a2 = -iir_coef_gain_ramp.coef_a[2];
            anc_gain_tt_iir_coefs0_l[0].b0 = iir_coef_gain_ramp.coef_b[0];
            anc_gain_tt_iir_coefs0_l[0].b1 = iir_coef_gain_ramp.coef_b[1];
            anc_gain_tt_iir_coefs0_l[0].b2 = iir_coef_gain_ramp.coef_b[2];

            anc_gain_tt_iir_coefs1_l[0].a1 = -iir_coef_gain_ramp.coef_a[1];
            anc_gain_tt_iir_coefs1_l[0].a2 = -iir_coef_gain_ramp.coef_a[2];
            anc_gain_tt_iir_coefs1_l[0].b0 = iir_coef_gain_ramp.coef_b[0];
            anc_gain_tt_iir_coefs1_l[0].b1 = iir_coef_gain_ramp.coef_b[1];
            anc_gain_tt_iir_coefs1_l[0].b2 = iir_coef_gain_ramp.coef_b[2];
        }
        codec->REG_24C |= CODEC_CODEC_IIR1_ENABLE;
#endif

    }
#endif

    if (anc_type & ANC_FEEDBACK) {
        anc_switching_gain_delay_fb = anc_switching_gain_delay;
        anc_switching_coef_delay_fb = anc_switching_coef_delay;

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        codec->REG_680 &= ~CODEC_CODEC_IIR4_ENABLE;
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            anc_gain_fb_iir_coefs0_l[0].a1 = -iir_coef_gain_ramp.coef_a[1];
            anc_gain_fb_iir_coefs0_l[0].a2 = -iir_coef_gain_ramp.coef_a[2];
            anc_gain_fb_iir_coefs0_l[0].b0 = iir_coef_gain_ramp.coef_b[0];
            anc_gain_fb_iir_coefs0_l[0].b1 = iir_coef_gain_ramp.coef_b[1];
            anc_gain_fb_iir_coefs0_l[0].b2 = iir_coef_gain_ramp.coef_b[2];

            anc_gain_fb_iir_coefs1_l[0].a1 = -iir_coef_gain_ramp.coef_a[1];
            anc_gain_fb_iir_coefs1_l[0].a2 = -iir_coef_gain_ramp.coef_a[2];
            anc_gain_fb_iir_coefs1_l[0].b0 = iir_coef_gain_ramp.coef_b[0];
            anc_gain_fb_iir_coefs1_l[0].b1 = iir_coef_gain_ramp.coef_b[1];
            anc_gain_fb_iir_coefs1_l[0].b2 = iir_coef_gain_ramp.coef_b[2];
        }
        codec->REG_680 |= CODEC_CODEC_IIR4_ENABLE;
#endif
    }

#if defined(AUDIO_ANC_FB_MC_HW)
    if (anc_type & ANC_MUSICCANCLE) {
        anc_switching_gain_delay_mc = anc_switching_gain_delay;
        anc_switching_coef_delay_mc = anc_switching_coef_delay;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        codec->REG_684 &= ~CODEC_CODEC_IIR5_ENABLE;
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
            anc_gain_mc_iir_coefs0_l[0].a1 = -iir_coef_gain_ramp.coef_a[1];
            anc_gain_mc_iir_coefs0_l[0].a2 = -iir_coef_gain_ramp.coef_a[2];
            anc_gain_mc_iir_coefs0_l[0].b0 = iir_coef_gain_ramp.coef_b[0];
            anc_gain_mc_iir_coefs0_l[0].b1 = iir_coef_gain_ramp.coef_b[1];
            anc_gain_mc_iir_coefs0_l[0].b2 = iir_coef_gain_ramp.coef_b[2];

            anc_gain_mc_iir_coefs1_l[0].a1 = -iir_coef_gain_ramp.coef_a[1];
            anc_gain_mc_iir_coefs1_l[0].a2 = -iir_coef_gain_ramp.coef_a[2];
            anc_gain_mc_iir_coefs1_l[0].b0 = iir_coef_gain_ramp.coef_b[0];
            anc_gain_mc_iir_coefs1_l[0].b1 = iir_coef_gain_ramp.coef_b[1];
            anc_gain_mc_iir_coefs1_l[0].b2 = iir_coef_gain_ramp.coef_b[2];
        }
        codec->REG_684 |= CODEC_CODEC_IIR5_ENABLE;
#endif
    }
#endif
    int_unlock(lock);

    return 0;
}

int anc_howling_set(ANC_HOWLING_WINDOW window, ANC_HOWLING_THRESHOLD threshold)
{
#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK) || defined(ANC_TT_CHECK)
    float threshold_multiple = 0;
    uint32_t acc_window = 0;
    uint32_t trig_window = 0;
    uint32_t check_threshold = 0;

    uint32_t lock;
    lock = int_lock();
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
        if (fb_check_open_flag) {
            codec->REG_130 &= ~CODEC_CODEC_FB_CHECK_ENABLE_CH0;

            switch(window) {
                case ANC_HOWLING_WINDOW_32:
                threshold_multiple = 0.25f;
                acc_window = window;
                trig_window = window;
                break;

                case ANC_HOWLING_WINDOW_64:
                threshold_multiple = 1.0f;
                acc_window = window;
                trig_window = window;
                break;

                case ANC_HOWLING_WINDOW_128:
                threshold_multiple = 4.0f;
                acc_window = window;
                trig_window = window;
                break;

                case ANC_HOWLING_WINDOW_256:
                threshold_multiple = 16.0f;
                acc_window = window;
                trig_window = window;
                break;

                case ANC_HOWLING_WINDOW_512:
                threshold_multiple = 64.0f;
                acc_window = window;
                trig_window = window;
                break;

                default:
                acc_window = ANC_HOWLING_WINDOW_64;
                trig_window = ANC_HOWLING_WINDOW_64;
                threshold_multiple = 1.0f;
                break;
             }

            switch(threshold) {
                case ANC_HOWLING_THRESHOLD_m12dB:
                case ANC_HOWLING_THRESHOLD_m10dB:
                case ANC_HOWLING_THRESHOLD_m8dB:
                case ANC_HOWLING_THRESHOLD_m6dB:
                case ANC_HOWLING_THRESHOLD_m4dB:
                case ANC_HOWLING_THRESHOLD_m2dB:
                case ANC_HOWLING_THRESHOLD_0dB:
                case ANC_HOWLING_THRESHOLD_2dB:
                case ANC_HOWLING_THRESHOLD_4dB:
                case ANC_HOWLING_THRESHOLD_6dB:
                case ANC_HOWLING_THRESHOLD_8dB:
                case ANC_HOWLING_THRESHOLD_10dB:
                case ANC_HOWLING_THRESHOLD_12dB:
                check_threshold = (uint32_t)(threshold*threshold_multiple);
                break;

                default:
                check_threshold = (uint32_t)(ANC_HOWLING_THRESHOLD_0dB*threshold_multiple);
                break;
             }
            DRIVERS_TRACE(1,"%s:acc_window:%d,trig_window:%d", __func__, acc_window,trig_window);
            DRIVERS_TRACE(1,"%s:check_threshold:0x%x", __func__, check_threshold);

            codec->REG_130 = SET_BITFIELD(codec->REG_130, CODEC_CODEC_FB_CHECK_ACC_WINDOW_CH0, acc_window);
            codec->REG_130 = SET_BITFIELD(codec->REG_130, CODEC_CODEC_FB_CHECK_TRIG_WINDOW_CH0, trig_window);
            codec->REG_138 = SET_BITFIELD(codec->REG_138, CODEC_CODEC_FB_CHECK_THRESHOLD_CH0, check_threshold);
            codec->REG_130 |= CODEC_CODEC_FB_CHECK_ENABLE_CH0;
        }
    }
#endif

    int_unlock(lock);
#endif
    return 0;
}

int anc_adc_data_select(ANC_ADC_DATA data_select)
{
#if defined(AUDIO_ANC_FB_MC) ||defined(AUDIO_ANC_FB_MC_HW)
    DRIVERS_TRACE(1,"%s:data_select:%d", __func__,data_select);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
        switch(data_select) {
            case ANC_ADC_ONLY_ADC:
            codec->REG_130 &= ~CODEC_CODEC_FB_CHECK_KEEP_CH0;
            codec->REG_0DC |= CODEC_CODEC_FEEDBACK_MC_EN_CH0;
            break;

            case ANC_ADC_ONLY_MC:
            codec->REG_130 |= CODEC_CODEC_FB_CHECK_KEEP_CH0;
            codec->REG_0DC &= ~CODEC_CODEC_FEEDBACK_MC_EN_CH0;
            break;

            case ANC_ADC_ADC_ADD_MC:
            codec->REG_130 |= CODEC_CODEC_FB_CHECK_KEEP_CH0;
            codec->REG_0DC |= CODEC_CODEC_FEEDBACK_MC_EN_CH0;
            break;

            default:
            codec->REG_130 &= ~CODEC_CODEC_FB_CHECK_KEEP_CH0;
            codec->REG_0DC |= CODEC_CODEC_FEEDBACK_MC_EN_CH0;
            break;
        }
    }
#endif
#endif
    return 0;
}

int anc_limiter_enable(enum ANC_TYPE_T anc_type)
{
    DRIVERS_TRACE(1,"%s anc_type:%d", __func__,anc_type);

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
#if defined(ANC_LIMITER_FF)
        if (anc_type & ANC_FEEDFORWARD) {
            //Enable the FF limiter;
            codec->REG_248 &= ~CODEC_CODEC_IIR0_LMT_BYPASS;
        }
#endif
#if defined(ANC_LIMITER_TT) && defined(AUDIO_ANC_TT_HW)
        if (anc_type & ANC_TALKTHRU) {
            //Enable the TT limiter;
            codec->REG_24C &= ~CODEC_CODEC_IIR1_LMT_BYPASS;
        }
#endif
#if defined(ANC_LIMITER_FB)
        if (anc_type & ANC_FEEDBACK) {
            //Enable the FB limiter;
            codec->REG_680 &= ~CODEC_CODEC_IIR4_LMT_BYPASS;
        }
#endif
#if defined(ANC_LIMITER_MC) && defined(AUDIO_ANC_FB_MC_HW)
        if (anc_type & ANC_MUSICCANCLE) {
            //Enable the MC limiter;
            codec->REG_684 &= ~CODEC_CODEC_IIR5_LMT_BYPASS;
        }
#endif
    }
#endif

    return 0;
}

int anc_limiter_disable(enum ANC_TYPE_T anc_type)
{
    DRIVERS_TRACE(1,"%s anc_type:%d", __func__,anc_type);

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
#if defined(ANC_LIMITER_FF)
        if (anc_type & ANC_FEEDFORWARD) {
            //Disable the FF limiter;
            codec->REG_248 |= CODEC_CODEC_IIR0_LMT_BYPASS;
        }
#endif
#if defined(ANC_LIMITER_TT) && defined(AUDIO_ANC_TT_HW)
        if (anc_type & ANC_TALKTHRU) {
            //Disable the TT limiter;
            codec->REG_24C |= CODEC_CODEC_IIR1_LMT_BYPASS;
        }
#endif
#if defined(ANC_LIMITER_FB)
        if (anc_type & ANC_FEEDBACK) {
            //Disable the FB limiter;
            codec->REG_680 |= CODEC_CODEC_IIR4_LMT_BYPASS;
        }
#endif
#if defined(ANC_LIMITER_MC) && defined(AUDIO_ANC_FB_MC_HW)
        if (anc_type & ANC_MUSICCANCLE) {
            //Disable the MC limiter;
            codec->REG_684 |= CODEC_CODEC_IIR5_LMT_BYPASS;
        }
#endif
    }
#endif

    return 0;
}

int anc_limiter_threhold_set(enum ANC_TYPE_T anc_type,int32_t threhold_db)
{
    int32_t threhold;
    if(threhold_db>0) {
        threhold = 0x7fffff;
    } else {
        threhold = (int32_t)(db_to_float(threhold_db) * 0x7fffff);
    }

    DRIVERS_TRACE(1,"%s threhold_db:%d,threhold:0x%x", __func__, threhold_db, threhold);

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
#if defined(ANC_LIMITER_FF)
        if (anc_type & ANC_FEEDFORWARD) {
            //limiter threshold, 0dB: 0x7fffff
            codec->REG_25C &= ~CODEC_CODEC_IIR0_LMT_TH_UPDATE;
            codec->REG_31C = SET_BITFIELD(codec->REG_31C, CODEC_CODEC_IIR0_LMT_TH, threhold);
            hal_sys_timer_delay_us(1);
            codec->REG_25C |= CODEC_CODEC_IIR0_LMT_TH_UPDATE;
        }
#endif

#if defined(ANC_LIMITER_TT) && defined(AUDIO_ANC_TT_HW)
        if (anc_type & ANC_TALKTHRU) {
            //limiter threshold, 0dB: 0x7fffff
            codec->REG_25C &= ~CODEC_CODEC_IIR1_LMT_TH_UPDATE;
            codec->REG_320 = SET_BITFIELD(codec->REG_320, CODEC_CODEC_IIR1_LMT_TH, threhold);
            hal_sys_timer_delay_us(1);
            codec->REG_25C |= CODEC_CODEC_IIR1_LMT_TH_UPDATE;
        }
#endif

#if defined(ANC_LIMITER_FB)
        if (anc_type & ANC_FEEDBACK) {
            //limiter threshold, 0dB: 0x7fffff
            codec->REG_25C &= ~CODEC_CODEC_IIR4_LMT_TH_UPDATE;
            codec->REG_32C = SET_BITFIELD(codec->REG_32C, CODEC_CODEC_IIR4_LMT_TH, threhold);
            hal_sys_timer_delay_us(1);
            codec->REG_25C |= CODEC_CODEC_IIR4_LMT_TH_UPDATE;
        }
#endif
#if defined(ANC_LIMITER_MC) && defined(AUDIO_ANC_FB_MC_HW)
        if (anc_type & ANC_MUSICCANCLE) {
            //limiter threshold, 0dB: 0x7fffff
            codec->REG_25C &= ~CODEC_CODEC_IIR5_LMT_TH_UPDATE;
            codec->REG_330 = SET_BITFIELD(codec->REG_330, CODEC_CODEC_IIR5_LMT_TH, threhold);
            hal_sys_timer_delay_us(1);
            codec->REG_25C |= CODEC_CODEC_IIR5_LMT_TH_UPDATE;
        }
#endif
    }
#endif

    return 0;
}
