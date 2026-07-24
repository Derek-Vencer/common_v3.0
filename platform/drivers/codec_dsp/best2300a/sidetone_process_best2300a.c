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
/*******************************************************************************
** namer��sidetone filter
** description��IIR filter
** version��V1.0
** author�� xuml
** modify��2021.1.14.
*******************************************************************************/

#include "mm_dbg.h"
#include <stdio.h>
#include <string.h>

#include "plat_addr_map.h"
#include CHIP_SPECIFIC_HDR(reg_codec)
#include "cmsis.h"
#include "hal_codec.h"
#include "hal_trace.h"
#include "hal_cmu.h"
#include "hal_timer.h"
#include "sidetone_process.h"

/*
iir coefficients(b/a Q27)
b2
b1
b0
a2
a1
*/
struct _sidetone_iir_coefs
{
	int32_t a1 ;
	int32_t a2 ;

	int32_t b1 ;
	int32_t b2 ;
	int32_t b0 ;
};

#define SIDETONE_GAIN_RAMP_THRESHLD (40)
#define SIDETONE_LIMITER_THREHOLD (0x7ffff0) //(0x7ffff0) (0x0dfff0)
#define SIDETONE_LIMITER_ATTACK_DELAY (8)

#define SIDETONE_GAIN_RAMP_ZERO (10)
#define SIDETONE_GAIN_RAMP_ONE (0x7ffffff)

#define GAIN_Q (9)

#define IIR_COUNTER (8)


//#define SIDETONE_LIMITER

volatile static int sidetone_open_flag=0;



#if defined(SIDETONE_LIMITER)

#define SIDETONE_LIMITER_ATTACK_DELAY (8)

#define SIDETONE_LIMITER_RELEASE_B1 (295)
#define SIDETONE_LIMITER_RELEASE_A1 (134217433)

#if SIDETONE_LIMITER_ATTACK_DELAY==60
#define SIDETONE_LIMITER_ATT_A1 (129391528)
#define SIDETONE_LIMITER_ATT_B1 (4826200)
#elif SIDETONE_LIMITER_ATTACK_DELAY==45
#define SIDETONE_LIMITER_ATT_A1 (127821672)
#define SIDETONE_LIMITER_ATT_B1 (6396056)
#elif SIDETONE_LIMITER_ATTACK_DELAY==30
#define SIDETONE_LIMITER_ATT_A1 (124738868)
#define SIDETONE_LIMITER_ATT_B1 (9478860)
#elif SIDETONE_LIMITER_ATTACK_DELAY==15
#define SIDETONE_LIMITER_ATT_A1 (115929434)
#define SIDETONE_LIMITER_ATT_B1 (18288294)
#elif SIDETONE_LIMITER_ATTACK_DELAY==8
#define SIDETONE_LIMITER_ATT_A1 (101983419)
#define SIDETONE_LIMITER_ATT_B1 (32234309)
#else
#error "ERROR SIDETONE_LIMITER_ATTACK_DELAY"
#endif

static const hw_codec_iir_coefs   iir_coef_limiter_attack_sidetone=
{
    .coef_b={0,    SIDETONE_LIMITER_ATT_B1,     0},
    .coef_a={134217728,    -SIDETONE_LIMITER_ATT_A1,     0},
};
static const hw_codec_iir_coefs   iir_coef_limiter_release_sidetone=
{
    .coef_b={0,    SIDETONE_LIMITER_RELEASE_B1,     0},
    .coef_a={134217728,    -SIDETONE_LIMITER_RELEASE_A1,     0},
};
#endif


static struct CODEC_T * const codec = (struct CODEC_T *)CODEC_BASE;

volatile static struct _sidetone_iir_coefs *sidetone_gain_ch0_iir_coefs0_l=(volatile struct _sidetone_iir_coefs *)(CODEC_BASE+0x0940);
volatile static struct _sidetone_iir_coefs *sidetone_gain_ch0_iir_coefs1_l=(volatile struct _sidetone_iir_coefs *)(CODEC_BASE+0x0b40);

#ifdef SIDETONE_LIMITER
volatile static struct _sidetone_iir_coefs *sidetone_limiter_ch0_iir_coefs0_l=(volatile struct _sidetone_iir_coefs *)(CODEC_BASE+0x0968);
volatile static struct _sidetone_iir_coefs *sidetone_limiter_ch0_iir_coefs1_l=(volatile struct _sidetone_iir_coefs *)(CODEC_BASE+0x0b68);
#endif

volatile static struct _sidetone_iir_coefs *sidetone_iir_coefs0_l=(volatile struct _sidetone_iir_coefs *)(CODEC_BASE+0x0800);
volatile static struct _sidetone_iir_coefs *sidetone_iir_coefs1_l=(volatile struct _sidetone_iir_coefs *)(CODEC_BASE+0x0a00);

/*
Type='Low Passing';
Freq=30;
Gain=0;
Q=0.1;
SampleRate=16000
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
static const hw_codec_iir_coefs   iir_coef_gain_ramp_sidetone=
{
    .coef_b={4398,         8796,         4398},
    .coef_a={134217728,   -253485637,    119285501},
};

int sidetone_set_cfg(HW_CODEC_IIR_CFG_T *cfg)
{
    LOG_I("%s", __func__);

    const HW_CODEC_IIR_FILTERS_T *sidetone_filtes_l = NULL;

    int counter = 0;
    int i;

    if (cfg == NULL)
    {
        LOG_I("%s: cfg is null", __func__);
        return -1;
    }

    if (sidetone_open_flag==0)
    {
        LOG_I("%s: sidetone is not opened", __func__);
        return -1;
    }

    sidetone_filtes_l = &(cfg->iir_filtes_l);

    counter = sidetone_filtes_l->iir_counter;

#if 1
    LOG_I("sidetone counter %d", sidetone_filtes_l->iir_counter);

    for(int j = 0; j <sidetone_filtes_l->iir_counter ; j++)
    {
       LOG_I("iir coef dac eq l 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x",\
        sidetone_filtes_l->iir_coef[j].coef_b[0], \
        sidetone_filtes_l->iir_coef[j].coef_b[1], \
        sidetone_filtes_l->iir_coef[j].coef_b[2], \
        sidetone_filtes_l->iir_coef[j].coef_a[0], \
        sidetone_filtes_l->iir_coef[j].coef_a[1], \
        sidetone_filtes_l->iir_coef[j].coef_a[2]);
    }
#endif


    //disable IIR0
    codec->REG_244 &= ~CODEC_CODEC_IIR0_ENABLE;
    codec->REG_244 &= ~CODEC_CODEC_IIR0_IIRA_ENABLE;
   // codec->REG_244 &= ~CODEC_CODEC_IIR0_IIRB_ENABLE;

    codec->REG_244 &= ~CODEC_CODEC_IIR0_CH0_BYPASS;

    if(counter>IIR_COUNTER)
    {
        counter=IIR_COUNTER;
    }
    else if(counter<=0)
    {
        LOG_I("%s: warning counter:%d",__func__,counter);
        counter=0;
        codec->REG_244 |= CODEC_CODEC_IIR0_CH0_BYPASS;
    }

    for(i=0; i<counter; i++)
    {
        sidetone_iir_coefs0_l[i].a1 = -sidetone_filtes_l->iir_coef[i].coef_a[1];
        sidetone_iir_coefs0_l[i].a2 = -sidetone_filtes_l->iir_coef[i].coef_a[2];
        sidetone_iir_coefs0_l[i].b0 = sidetone_filtes_l->iir_coef[i].coef_b[0];
        sidetone_iir_coefs0_l[i].b1 = sidetone_filtes_l->iir_coef[i].coef_b[1];
        sidetone_iir_coefs0_l[i].b2 = sidetone_filtes_l->iir_coef[i].coef_b[2];

//        sidetone_iir_coefs1_l[i].a1=-sidetone_filtes_l->iir_coef[i].coef_a[1];
//        sidetone_iir_coefs1_l[i].a2=-sidetone_filtes_l->iir_coef[i].coef_a[2];
//        sidetone_iir_coefs1_l[i].b0=sidetone_filtes_l->iir_coef[i].coef_b[0];
//        sidetone_iir_coefs1_l[i].b1=sidetone_filtes_l->iir_coef[i].coef_b[1];
//        sidetone_iir_coefs1_l[i].b2=sidetone_filtes_l->iir_coef[i].coef_b[2];
    }

    codec->REG_244 = SET_BITFIELD(codec->REG_244, CODEC_CODEC_IIR0_COUNT_CH0, counter);

    //enable IIR0
    codec->REG_244 |= CODEC_CODEC_IIR0_IIRA_ENABLE;
    // codec->REG_244 |= CODEC_CODEC_IIR0_IIRB_ENABLE;
    codec->REG_244 |= CODEC_CODEC_IIR0_ENABLE;


    return 0;
}


int sidetone_set_gain_f32(float gain_l, float gain_r)
{
    uint32_t lock;

    if (sidetone_open_flag==0)
    {
        LOG_I("%s: sidetone is not opened", __func__);
        return -1;
    }

    if(gain_l>8.0f||gain_l<0.0f)
    {
        LOG_I("%s gain_l must >=0 and <=8.0f", __func__);
        return -1;
    }

    lock = int_lock();

    codec->REG_25C &= ~(CODEC_CODEC_IIR0_GAIN_EXT_UPDATE_CH0|CODEC_CODEC_IIR0_GAIN_EXT_UPDATE_CH1);

    codec->REG_260=(uint32_t)((float)SIDETONE_GAIN_RAMP_ONE*gain_l);
    codec->REG_264=SIDETONE_GAIN_RAMP_ZERO;

    codec->REG_268=SIDETONE_GAIN_RAMP_ZERO;
    codec->REG_26C=SIDETONE_GAIN_RAMP_ZERO;

    codec->REG_25C |= (CODEC_CODEC_IIR0_GAIN_EXT_UPDATE_CH0|CODEC_CODEC_IIR0_GAIN_EXT_UPDATE_CH1);

    int_unlock(lock);

    return 0;
}


void sidetone_open(void)
{
    LOG_I("%s", __func__);

    if(sidetone_open_flag==1)
    {
        LOG_I("%s:sidetone is openned", __func__);
        return;
    }

    hal_cmu_codec_iir_enable(10000000);

    //open iir clock.
    codec->REG_060 = SET_BITFIELD(codec->REG_060, CODEC_EN_CLK_IIR, 0x1);

    //disable ANC.
    codec->REG_0D0 &= ~CODEC_CODEC_ANC_ENABLE_CH0;

    //iir ch0 -> sidetone
    codec->REG_0DC = SET_BITFIELD(codec->REG_0DC, CODEC_CODEC_ADC_IIR_CH0_SEL, 0x4);

    //init IIR0
    codec->REG_244 &= ~CODEC_CODEC_IIR0_ENABLE;
    codec->REG_244 &= ~CODEC_CODEC_IIR0_IIRA_ENABLE;
    codec->REG_244 &= ~CODEC_CODEC_IIR0_IIRB_ENABLE;
    codec->REG_244 &= ~CODEC_CODEC_IIR0_COEF_SWAP;

    //enable sidetone iir.
    codec->REG_080 |= CODEC_CODEC_SIDE_TONE_IIR_ENABLE;

    //enable adc iir.
    codec->REG_0BC |= CODEC_CODEC_ADC_CH0_IIR_ENABLE;

    //init threhold.
    codec->REG_340 = SIDETONE_GAIN_RAMP_THRESHLD;

    //init gain ramp coef.
    sidetone_gain_ch0_iir_coefs0_l[0].a1 = -iir_coef_gain_ramp_sidetone.coef_a[1];
    sidetone_gain_ch0_iir_coefs0_l[0].a2 = -iir_coef_gain_ramp_sidetone.coef_a[2];
    sidetone_gain_ch0_iir_coefs0_l[0].b0 = iir_coef_gain_ramp_sidetone.coef_b[0];
    sidetone_gain_ch0_iir_coefs0_l[0].b1 = iir_coef_gain_ramp_sidetone.coef_b[1];
    sidetone_gain_ch0_iir_coefs0_l[0].b2 = iir_coef_gain_ramp_sidetone.coef_b[2];

    sidetone_gain_ch0_iir_coefs1_l[0].a1 = -iir_coef_gain_ramp_sidetone.coef_a[1];
    sidetone_gain_ch0_iir_coefs1_l[0].a2 = -iir_coef_gain_ramp_sidetone.coef_a[2];
    sidetone_gain_ch0_iir_coefs1_l[0].b0 = iir_coef_gain_ramp_sidetone.coef_b[0];
    sidetone_gain_ch0_iir_coefs1_l[0].b1 = iir_coef_gain_ramp_sidetone.coef_b[1];
    sidetone_gain_ch0_iir_coefs1_l[0].b2 = iir_coef_gain_ramp_sidetone.coef_b[2];

    //init ch0
    codec->REG_244 &= ~CODEC_CODEC_IIR0_GAINCAL_EXT_CH0_BYPASS;
    codec->REG_244 &= ~CODEC_CODEC_IIR0_GAINUSE_EXT_CH0_BYPASS;
    codec->REG_25C &= ~CODEC_CODEC_IIR0_GAIN_EXT_SEL_CH0;

    //init ch1
    codec->REG_244 |= CODEC_CODEC_IIR0_GAINCAL_EXT_CH1_BYPASS;
    codec->REG_244 |= CODEC_CODEC_IIR0_GAINUSE_EXT_CH1_BYPASS;
    codec->REG_25C &= ~CODEC_CODEC_IIR0_GAIN_EXT_SEL_CH1;


#if defined(SIDETONE_LIMITER)
    sidetone_limiter_ch0_iir_coefs0_l[0].a1 = -iir_coef_limiter_attack_sidetone.coef_a[1];
    sidetone_limiter_ch0_iir_coefs0_l[0].a2 = -iir_coef_limiter_attack_sidetone.coef_a[2];
    sidetone_limiter_ch0_iir_coefs0_l[0].b0 = iir_coef_limiter_attack_sidetone.coef_b[0];
    sidetone_limiter_ch0_iir_coefs0_l[0].b1 = iir_coef_limiter_attack_sidetone.coef_b[1];
    sidetone_limiter_ch0_iir_coefs0_l[0].b2 = iir_coef_limiter_attack_sidetone.coef_b[2];

    sidetone_limiter_ch0_iir_coefs0_l[1].a1 = -iir_coef_limiter_release_sidetone.coef_a[1];
    sidetone_limiter_ch0_iir_coefs0_l[1].a2 = -iir_coef_limiter_release_sidetone.coef_a[2];
    sidetone_limiter_ch0_iir_coefs0_l[1].b0 = iir_coef_limiter_release_sidetone.coef_b[0];
    sidetone_limiter_ch0_iir_coefs0_l[1].b1 = iir_coef_limiter_release_sidetone.coef_b[1];
    sidetone_limiter_ch0_iir_coefs0_l[1].b2 = iir_coef_limiter_release_sidetone.coef_b[2];

    sidetone_limiter_ch0_iir_coefs1_l[0].a1 = -iir_coef_limiter_release_sidetone.coef_a[1];
    sidetone_limiter_ch0_iir_coefs1_l[0].a2 = -iir_coef_limiter_release_sidetone.coef_a[2];
    sidetone_limiter_ch0_iir_coefs1_l[0].b0 = iir_coef_limiter_release_sidetone.coef_b[0];
    sidetone_limiter_ch0_iir_coefs1_l[0].b1 = iir_coef_limiter_release_sidetone.coef_b[1];
    sidetone_limiter_ch0_iir_coefs1_l[0].b2 = iir_coef_limiter_release_sidetone.coef_b[2];

    sidetone_limiter_ch0_iir_coefs1_l[1].a1 = -iir_coef_limiter_release_sidetone.coef_a[1];
    sidetone_limiter_ch0_iir_coefs1_l[1].a2 = -iir_coef_limiter_release_sidetone.coef_a[2];
    sidetone_limiter_ch0_iir_coefs1_l[1].b0 = iir_coef_limiter_release_sidetone.coef_b[0];
    sidetone_limiter_ch0_iir_coefs1_l[1].b1 = iir_coef_limiter_release_sidetone.coef_b[1];
    sidetone_limiter_ch0_iir_coefs1_l[1].b2 = iir_coef_limiter_release_sidetone.coef_b[2];

    codec->REG_244 &= ~CODEC_CODEC_IIR0_LMT_CH0_BYPASS;

    codec->REG_25C &= ~CODEC_CODEC_IIR0_LMT_TH_UPDATE_CH0;
    codec->REG_354 = SIDETONE_LIMITER_THREHOLD;
    codec->REG_25C |= CODEC_CODEC_IIR0_LMT_TH_UPDATE_CH0;

    codec->REG_300 = SET_BITFIELD(codec->REG_300, CODEC_CODEC_IIR0_LMT_DELAY_CH0, SIDETONE_LIMITER_ATTACK_DELAY);
#else
    codec->REG_244 |= CODEC_CODEC_IIR0_LMT_CH0_BYPASS;
#endif

    for(int i=0;i<IIR_COUNTER;i++)
    {
        sidetone_iir_coefs0_l[i].a1=0;
        sidetone_iir_coefs0_l[i].a2=0;
        sidetone_iir_coefs0_l[i].b0=0;
        sidetone_iir_coefs0_l[i].b1=0;
        sidetone_iir_coefs0_l[i].b2=0;

        sidetone_iir_coefs1_l[i].a1=0;
        sidetone_iir_coefs1_l[i].a2=0;
        sidetone_iir_coefs1_l[i].b0=0;
        sidetone_iir_coefs1_l[i].b1=0;
        sidetone_iir_coefs1_l[i].b2=0;
    }

    codec->REG_244 &= ~CODEC_CODEC_IIR0_CH0_BYPASS;
    codec->REG_244 = SET_BITFIELD(codec->REG_244, CODEC_CODEC_IIR0_COUNT_CH0, IIR_COUNTER);

    codec->REG_244 |= CODEC_CODEC_IIR0_CH1_BYPASS;
    codec->REG_244 = SET_BITFIELD(codec->REG_244, CODEC_CODEC_IIR0_COUNT_CH1, IIR_COUNTER);

    codec->REG_244 |= CODEC_CODEC_IIR0_AUTO_STOP;


    sidetone_open_flag = 1;

    return;
}

void sidetone_close(void)
{
    if(sidetone_open_flag==0)
    {
        LOG_I("%s:sidetone is closed", __func__);
        return;
    }

    codec->REG_244 &= ~CODEC_CODEC_IIR0_ENABLE;
    codec->REG_244 &= ~CODEC_CODEC_IIR0_IIRA_ENABLE;
    codec->REG_244 &= ~CODEC_CODEC_IIR0_IIRB_ENABLE;

    hal_sys_timer_delay_us(1000);

    //disable sidetone iir.
    codec->REG_080 &= ~CODEC_CODEC_SIDE_TONE_IIR_ENABLE;
    //iir ch0 -> sidetone
    codec->REG_0DC = SET_BITFIELD(codec->REG_0DC, CODEC_CODEC_ADC_IIR_CH0_SEL, 0x0);

    //disable adc iir.
    codec->REG_0BC &= ~CODEC_CODEC_ADC_CH0_IIR_ENABLE;

    codec->REG_060 = SET_BITFIELD(codec->REG_060, CODEC_EN_CLK_IIR, 0x0);

    hal_cmu_codec_iir_disable();

    sidetone_open_flag=0;
    return;
}



