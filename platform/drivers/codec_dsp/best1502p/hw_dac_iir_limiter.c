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
/*******************************************************************************
** namer: audio limiter
** description: use for music.
*******************************************************************************/

#include "mm_dbg.h"
#include <stdio.h>
#include <string.h>

#include "plat_addr_map.h"
#include CHIP_SPECIFIC_HDR(reg_codec)
#include "cmsis.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hw_dac_iir_limiter.h"
#include "system_utils.h"

/**********************************mocro**************************************/
#define AUDIO_LIMITER_DELAY (1)
#define AUDIO_LIMITER_THREHOLD (0x3fffff)
#define AUDIO_LIMITER_THREHOLD_0dB (0x7fffff)
#define AUDIO_LIMITER_ATT_MS (13.42f)
#define AUDIO_LIMITER_RLS_MS (10)

#define AUDIO_LIMITER_ATT_B1 (134217728 / (uint32_t)(AUDIO_LIMITER_ATT_MS * 100))
#define AUDIO_LIMITER_ATT_A1 (134217728 - AUDIO_LIMITER_ATT_B1)

#define AUDIO_LIMITER_RLS_B1 (134217728 / (uint32_t)(AUDIO_LIMITER_RLS_MS * 100))
#define AUDIO_LIMITER_RLS_A1 (134217728 - AUDIO_LIMITER_RLS_B1)

typedef struct {
    int32_t a1 ;
    int32_t a2 ;

    int32_t b1 ;
    int32_t b2 ;
    int32_t b0 ;
} _hw_dac_limiter_coefs;

typedef struct {
    _hw_dac_limiter_coefs att_coef_ch0;
    _hw_dac_limiter_coefs rls_coef_ch0;
    _hw_dac_limiter_coefs att_coef_ch1;
    _hw_dac_limiter_coefs rls_coef_ch1;
} hw_dac_limiter_coef_t;

static const _hw_dac_limiter_coefs limiter_att_coef_trans = {
    .a1 = AUDIO_LIMITER_ATT_A1,
    .a2 = 0,

    .b1 = AUDIO_LIMITER_ATT_B1,
    .b2 = 0,
    .b0 = 0,
};

static const _hw_dac_limiter_coefs limiter_rls_coef_trans = {
    .a1 = AUDIO_LIMITER_RLS_A1,
    .a2 = 0,

    .b1 = AUDIO_LIMITER_RLS_B1,
    .b2 = 0,
    .b0 = 0,
};

/*******************************data struction***********************************/
#define LIMITER_BASE ((uint32_t)CODEC_BASE + 0x5000)
static struct CODEC_T *const codec = (struct CODEC_T *)CODEC_BASE;
volatile static _hw_dac_limiter_coefs *audio_limiter_coefsa_ch0 = (volatile _hw_dac_limiter_coefs *)(LIMITER_BASE + 0x348);
volatile static _hw_dac_limiter_coefs *audio_limiter_coefsb_ch0 = (volatile _hw_dac_limiter_coefs *)(LIMITER_BASE + 0x748);
volatile static _hw_dac_limiter_coefs *audio_limiter_coefsa_ch1 = (volatile _hw_dac_limiter_coefs *)(LIMITER_BASE + 0x370);
volatile static _hw_dac_limiter_coefs *audio_limiter_coefsb_ch1 = (volatile _hw_dac_limiter_coefs *)(LIMITER_BASE + 0x770);

volatile static uint32_t limiter_ch_map = (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1);

static hw_dac_limiter_coef_t limiter_coef;

static int32_t audio_limiter_att_rls_coef_generate(_hw_dac_limiter_coefs *coef, float limiter_ms)
{
    if ((limiter_ms > 0.1f) && (limiter_ms < 671088.64f))
    {
        coef->b0 = 0;
        coef->b1 = 134217728 / (uint32_t)(limiter_ms * 100);
        coef->b2 = 0;

        coef->a1 = -(134217728 - coef->b1);
        coef->a2 = 0;
    }
    else
    {
        LOG_I( "%s: [warning]Error lmiter_time(ms*100):%d", __func__, (uint32_t)(limiter_ms * 100));
    }

    return 0;
}

static int32_t audio_limiter_coef_copy(volatile _hw_dac_limiter_coefs *iir_coefs, _hw_dac_limiter_coefs *__restrict filtes_old)
{
#if 0
    LOG_I("%s: iir_coefs:0x%x",__func__,(uint32_t)iir_coefs);
    LOG_I("%s: -filtes_old->a1:0x%x",__func__, -filtes_old->a1);
    LOG_I("%s: -filtes_old->a2:0x%x",__func__, -filtes_old->a2);
    LOG_I("%s: -filtes_old->b0:0x%x",__func__, filtes_old->b0);
    LOG_I("%s: -filtes_old->b1:0x%x",__func__, filtes_old->b1);
    LOG_I("%s: -filtes_old->b2:0x%x",__func__, filtes_old->b2);
#endif
    iir_coefs->a1 = -filtes_old->a1;
    iir_coefs->a2 = -filtes_old->a2;
    iir_coefs->b0 = filtes_old->b0;
    iir_coefs->b1 = filtes_old->b1;
    iir_coefs->b2 = filtes_old->b2;

    return 0;
}

int32_t hw_dac_limiter_set_cfg(const hw_dac_limiter_cfg_t *cfg)
{
    int32_t threhold = 0;
    const hw_dac_limiter_param_t *limiter_param_ch0 = NULL;
    const hw_dac_limiter_param_t *limiter_param_ch1 = NULL;

    limiter_param_ch0 = &(cfg->limiter_param_ch0);
    limiter_param_ch1 = &(cfg->limiter_param_ch1);

    LOG_I( "enable = %d, delay=%d, thd=%d , att_ms=%d, rls_ms=%d", cfg->limiter_param_ch0.enable, 
                                                                     cfg->limiter_param_ch0.delay_ms,
                                                                     (int32_t)(cfg->limiter_param_ch0.thd),
                                                                     (int32_t)(cfg->limiter_param_ch0.att_ms),
                                                                     (int32_t)(cfg->limiter_param_ch0.rls_ms));
    if (limiter_ch_map & AUD_CHANNEL_MAP_CH0)
    {
        if (limiter_param_ch0->enable != 0)
        {

            if (limiter_param_ch0->thd > 0)
            {
                threhold = AUDIO_LIMITER_THREHOLD_0dB;
            }
            else
            {
                threhold = (int32_t)(db_to_float(limiter_param_ch0->thd) * AUDIO_LIMITER_THREHOLD_0dB);
            }

            codec->REG_25C &= ~CODEC_CODEC_DEQ_IIR_LMT_TH_UPDATE_CH0;
            codec->REG_340 = SET_BITFIELD(codec->REG_340, CODEC_CODEC_DEQ_IIR_LMT_TH_CH0, threhold);
            hal_sys_timer_delay_us(1);
            codec->REG_25C |= CODEC_CODEC_DEQ_IIR_LMT_TH_UPDATE_CH0;

            audio_limiter_att_rls_coef_generate(&limiter_coef.att_coef_ch0, limiter_param_ch0->att_ms);
            audio_limiter_att_rls_coef_generate(&limiter_coef.rls_coef_ch0, limiter_param_ch0->rls_ms);

            codec->REG_340 = SET_BITFIELD(codec->REG_340, CODEC_CODEC_DEQ_IIR_LMT_DELAY_CH0, limiter_param_ch0->delay_ms);

            codec->REG_258 &= ~CODEC_CODEC_DEQ_IIR_LMT_CH0_BYPASS;
        }
        else
        {
            codec->REG_258 |= CODEC_CODEC_DEQ_IIR_LMT_CH0_BYPASS;
        }
    }

    if (limiter_ch_map & AUD_CHANNEL_MAP_CH1)
    {
        if (limiter_param_ch1->enable != 0)
        {
            if (limiter_param_ch1->thd > 0)
            {
                threhold = AUDIO_LIMITER_THREHOLD_0dB;
            }
            else
            {
                threhold = (int32_t)(db_to_float(limiter_param_ch1->thd) * AUDIO_LIMITER_THREHOLD_0dB);
            }

            codec->REG_25C &= ~CODEC_CODEC_DEQ_IIR_LMT_TH_UPDATE_CH1;
            codec->REG_344 = SET_BITFIELD(codec->REG_344, CODEC_CODEC_DEQ_IIR_LMT_TH_CH1, threhold);
            hal_sys_timer_delay_us(1);
            codec->REG_25C |= CODEC_CODEC_DEQ_IIR_LMT_TH_UPDATE_CH1;

            audio_limiter_att_rls_coef_generate(&limiter_coef.att_coef_ch1, limiter_param_ch1->att_ms);
            audio_limiter_att_rls_coef_generate(&limiter_coef.rls_coef_ch1, limiter_param_ch1->rls_ms);

            codec->REG_344 = SET_BITFIELD(codec->REG_344, CODEC_CODEC_DEQ_IIR_LMT_DELAY_CH1, limiter_param_ch1->delay_ms);

            codec->REG_258 &= ~CODEC_CODEC_DEQ_IIR_LMT_CH1_BYPASS;
        }
        else
        {
            codec->REG_258 |= CODEC_CODEC_DEQ_IIR_LMT_CH1_BYPASS;
        }
    }

    return 0;
}

int32_t hw_dac_limiter_att_rls_coef_copy(int8_t coefa_using_flag)
{
    if (coefa_using_flag == 0)
    {
        if (limiter_ch_map & AUD_CHANNEL_MAP_CH0)
        {
            audio_limiter_coef_copy(&(audio_limiter_coefsb_ch0[0]), &limiter_coef.att_coef_ch0);
            audio_limiter_coef_copy(&(audio_limiter_coefsb_ch0[1]), &limiter_coef.rls_coef_ch0);
        }
        if (limiter_ch_map & AUD_CHANNEL_MAP_CH1)
        {
            audio_limiter_coef_copy(&(audio_limiter_coefsb_ch1[0]), &limiter_coef.att_coef_ch1);
            audio_limiter_coef_copy(&(audio_limiter_coefsb_ch1[1]), &limiter_coef.rls_coef_ch1);
        }
    }
    else
    {
        if (limiter_ch_map & AUD_CHANNEL_MAP_CH0)
        {
            audio_limiter_coef_copy(&(audio_limiter_coefsa_ch0[0]), &limiter_coef.att_coef_ch0);
            audio_limiter_coef_copy(&(audio_limiter_coefsa_ch0[1]), &limiter_coef.rls_coef_ch0);
        }
        if (limiter_ch_map & AUD_CHANNEL_MAP_CH1)
        {
            audio_limiter_coef_copy(&(audio_limiter_coefsa_ch1[0]), &limiter_coef.att_coef_ch1);
            audio_limiter_coef_copy(&(audio_limiter_coefsa_ch1[1]), &limiter_coef.rls_coef_ch1);
        }
    }

    return 0;
}

int32_t hw_dac_limiter_init()
{
    LOG_I( "[%s]", __func__);
    codec->REG_258 |= CODEC_CODEC_DEQ_IIR_LMT_CH0_BYPASS;
    codec->REG_258 |= CODEC_CODEC_DEQ_IIR_LMT_CH1_BYPASS;

    if (limiter_ch_map & AUD_CHANNEL_MAP_CH0)
    {
        audio_limiter_coefsa_ch0[0] = limiter_att_coef_trans;
        audio_limiter_coefsa_ch0[1] = limiter_rls_coef_trans;

        audio_limiter_coefsb_ch0[0] = limiter_att_coef_trans;
        audio_limiter_coefsb_ch0[1] = limiter_rls_coef_trans;

        limiter_coef.att_coef_ch0 = limiter_att_coef_trans;
        limiter_coef.rls_coef_ch0 = limiter_rls_coef_trans;

        codec->REG_258 &= ~CODEC_CODEC_DEQ_IIR_LMT_CH0_BYPASS;

        codec->REG_25C &= ~CODEC_CODEC_DEQ_IIR_LMT_TH_UPDATE_CH0;
        codec->REG_340 = SET_BITFIELD(codec->REG_340, CODEC_CODEC_DEQ_IIR_LMT_TH_CH0, AUDIO_LIMITER_THREHOLD);
        hal_sys_timer_delay_us(1);
        codec->REG_25C |= CODEC_CODEC_DEQ_IIR_LMT_TH_UPDATE_CH0;

        codec->REG_340 = SET_BITFIELD(codec->REG_340, CODEC_CODEC_DEQ_IIR_LMT_DELAY_CH0, AUDIO_LIMITER_DELAY);
    }

    if (limiter_ch_map & AUD_CHANNEL_MAP_CH1)
    {
        audio_limiter_coefsa_ch1[0] = limiter_att_coef_trans;
        audio_limiter_coefsa_ch1[1] = limiter_rls_coef_trans;

        audio_limiter_coefsb_ch1[0] = limiter_att_coef_trans;
        audio_limiter_coefsb_ch1[1] = limiter_rls_coef_trans;

        limiter_coef.att_coef_ch1 = limiter_att_coef_trans;
        limiter_coef.rls_coef_ch1 = limiter_rls_coef_trans;

        codec->REG_258 &= ~CODEC_CODEC_DEQ_IIR_LMT_CH1_BYPASS;

        codec->REG_25C &= ~CODEC_CODEC_DEQ_IIR_LMT_TH_UPDATE_CH1;
        codec->REG_344 = SET_BITFIELD(codec->REG_344, CODEC_CODEC_DEQ_IIR_LMT_TH_CH1, AUDIO_LIMITER_THREHOLD);
        hal_sys_timer_delay_us(1);
        codec->REG_25C |= CODEC_CODEC_DEQ_IIR_LMT_TH_UPDATE_CH1;

        codec->REG_344 = SET_BITFIELD(codec->REG_344, CODEC_CODEC_DEQ_IIR_LMT_DELAY_CH1, AUDIO_LIMITER_DELAY);
    }

    return 0;
}

int32_t hw_dac_limiter_set_ch(int32_t ch_map)
{
    LOG_I( "%s, ch = 0x%x", __func__, ch_map);

    if (ch_map & AUD_CHANNEL_MAP_CH0)
    {
        limiter_ch_map |= AUD_CHANNEL_MAP_CH0;
    }
    else
    {
        limiter_ch_map &= ~AUD_CHANNEL_MAP_CH0;
    }

    if (ch_map & AUD_CHANNEL_MAP_CH1)
    {
        limiter_ch_map |= AUD_CHANNEL_MAP_CH1;
    }
    else
    {
        limiter_ch_map &= ~AUD_CHANNEL_MAP_CH1;
    }

    return 0;
}

int32_t hw_dac_limiter_enable(void)
{
    LOG_I( "%s ...", __func__);

    if (limiter_ch_map & AUD_CHANNEL_MAP_CH0)
    {
        codec->REG_258 &= ~CODEC_CODEC_DEQ_IIR_LMT_CH0_BYPASS;
    }

    if (limiter_ch_map & AUD_CHANNEL_MAP_CH1)
    {
        codec->REG_258 &= ~CODEC_CODEC_DEQ_IIR_LMT_CH1_BYPASS;
    }

    return 0;
}

int32_t hw_dac_limiter_disable(void)
{
    LOG_I( "%s ...", __func__);

    if (limiter_ch_map & AUD_CHANNEL_MAP_CH0)
    {
        codec->REG_258 |= CODEC_CODEC_DEQ_IIR_LMT_CH0_BYPASS;
    }

    if (limiter_ch_map & AUD_CHANNEL_MAP_CH1)
    {
        codec->REG_258 |= CODEC_CODEC_DEQ_IIR_LMT_CH1_BYPASS;
    }

    hal_sys_timer_delay_us(1);

    return 0;
}

int32_t hw_dac_limiter_open(int32_t ch_map)
{
    LOG_I( "%s, ch_map = 0x%0x ...", __func__, ch_map);

    hw_dac_limiter_set_ch(ch_map);
    hw_dac_limiter_init();

    return 0;
}

int32_t hw_dac_limiter_close(void)
{
    LOG_I( "%s ...", __func__);

    hw_dac_limiter_disable();

    return 0;
}

int32_t hw_dac_limiter_set_threhold(int32_t threhold_db)
{
    int32_t threhold;
    if (threhold_db > 0)
    {
        threhold = AUDIO_LIMITER_THREHOLD_0dB;
    }
    else
    {
        threhold = (int32_t)(db_to_float(threhold_db) * AUDIO_LIMITER_THREHOLD_0dB);
    }

    LOG_I( "%s threhold_db:%d, threhold:0x%x", __func__, threhold_db, threhold);

    if (limiter_ch_map & AUD_CHANNEL_MAP_CH0)
    {
        codec->REG_25C &= ~CODEC_CODEC_DEQ_IIR_LMT_TH_UPDATE_CH0;
        codec->REG_340 = SET_BITFIELD(codec->REG_340, CODEC_CODEC_DEQ_IIR_LMT_TH_CH0, threhold);
        hal_sys_timer_delay_us(1);
        codec->REG_25C |= CODEC_CODEC_DEQ_IIR_LMT_TH_UPDATE_CH0;
    }

    if (limiter_ch_map & AUD_CHANNEL_MAP_CH1)
    {
        codec->REG_25C &= ~CODEC_CODEC_DEQ_IIR_LMT_TH_UPDATE_CH1;
        codec->REG_344 = SET_BITFIELD(codec->REG_344, CODEC_CODEC_DEQ_IIR_LMT_TH_CH1, threhold);
        hal_sys_timer_delay_us(1);
        codec->REG_25C |= CODEC_CODEC_DEQ_IIR_LMT_TH_UPDATE_CH1;
    }

    return 0;
}
