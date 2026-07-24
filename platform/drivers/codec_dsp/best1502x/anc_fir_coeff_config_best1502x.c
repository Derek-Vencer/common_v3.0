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

#include "mm_dbg.h"
#include <stdio.h>
#include <string.h>

#include "plat_addr_map.h"
#include CHIP_SPECIFIC_HDR(reg_codec)
#include "cmsis.h"
#include "hal_dma.h"
#include "hal_codec.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_cmu.h"
#include "hal_codec.h"
#include "analog.h"
#include "hwtimer_list.h"
#include "hal_sysfreq.h"
#include "hal_chipid.h"
#include "anc_fir_coeff_config.h"

#if defined(AUDIO_PSAP_DEHOWLING_HW)
#define FIR_LEN (128)
#else
#define FIR_LEN (472)
#endif

#define FIR_COEFF_FIXED_Q                           ((1 << 23) - 1)

#define WAIT_FIR_CONFIG(reg, cond, time, cnt)       while ((reg & cond) == 0) {     \
                                                        hal_sys_timer_delay_us(10); \
                                                        cnt = cnt + 1;              \
                                                        if (cnt > time) {           \
                                                            ASSERT(0, "[%s] coeff set timeout!!", __func__); \
                                                        }                                               \
                                                    }

static int32_t g_fir_item[ANC_FIR_MAX_ORDER];

static uint32_t g_ch0_fir_len;
static uint32_t g_ch1_fir_len;
static uint32_t g_ch3_fir_len;
static bool g_fir_is_opened = false;

static struct CODEC_T * const codec = (struct CODEC_T *)CODEC_BASE;
struct _fir_control
{
	uint32_t fir_stream_enable_ch0 : 1;
	uint32_t fir_stream_enable_ch1 : 1;
	uint32_t fir_stream_enable_ch2 : 1;
	uint32_t fir_stream_enable_ch3 : 1;

	uint32_t Reserved : 28;
};
POSSIBLY_UNUSED volatile static struct _fir_control *fir_control=(volatile struct _fir_control *)(CODEC_BASE+0x100);

volatile static int32_t *anc_ff_fir_coefs_l = (volatile  int32_t *)(CODEC_BASE+0x9000);
volatile static int32_t *anc_ff_fir_coefs_r = (volatile  int32_t *)(CODEC_BASE+0xb000);
volatile static int32_t *anc_fb_fir_coefs_l = (volatile  int32_t *)(CODEC_BASE+0xd000);
volatile static int32_t *anc_fb_fir_coefs_r = (volatile  int32_t *)(CODEC_BASE+0xf000);
volatile static int32_t *anc_mc_fir_coefs_b = (volatile  int32_t *)(CODEC_BASE+0xb000);
volatile static int32_t *anc_mc_fir_coefs_a = (volatile  int32_t *)(CODEC_BASE+0xf000);

int32_t anc_fir_coeff_config_init()
{
    uint32_t val = GET_BITFIELD(codec->REG_06C, CODEC_EN_CLK_FIR);

    LOG_I( "[%s] ptr = %x", __func__, CODEC_BASE);

    if (val == 0) {
        LOG_I( "[%s] please open anc first!!!", __func__);
        return -1;
    }
    LOG_I( "[%s] ptr = %x", __func__, (uint32_t)(*(uint32_t *)fir_control));

    if (codec->REG_100 & CODEC_FIR_STREAM_ENABLE_CH0)
        g_ch0_fir_len = GET_BITFIELD(codec->REG_108, CODEC_FIR_ORDER_CH0);
    if (codec->REG_100 & CODEC_FIR_STREAM_ENABLE_CH1)
        g_ch1_fir_len = GET_BITFIELD(codec->REG_110, CODEC_FIR_ORDER_CH1);
    if (codec->REG_100 & CODEC_FIR_STREAM_ENABLE_CH3)
        g_ch3_fir_len = GET_BITFIELD(codec->REG_120, CODEC_FIR_ORDER_CH3);

    g_fir_is_opened = true;

    LOG_I( "[%s] ch0: %d ch1: %d ch3: %d", __func__, g_ch0_fir_len, g_ch1_fir_len, g_ch3_fir_len);

    return 0;
}

int32_t anc_fir_coeff_config_deinit()
{
    g_fir_is_opened = false;

    g_ch0_fir_len = 0;
    g_ch1_fir_len = 0;
    g_ch3_fir_len = 0;

    return 0;
}

int32_t anc_fir_get_len(ANC_FIR_CHANNEL_T ch)
{
    switch (ch)
    {
    case ANC_FIR_CHANNEL_FF_L:
        return g_ch0_fir_len;
        break;

    case ANC_FIR_CHANNEL_FF_R:
        return g_ch1_fir_len;
        break;

    case ANC_FIR_CHANNEL_MC_L:
        return g_ch3_fir_len;
        break;

    default:
        LOG_I( "[%s] The path is not supported!!!", __func__);
        break;
    }

    return 0;
}

static int32_t anc_fir_set_coeff_ch(const float *coeff, volatile int32_t *coefs_a,
                volatile int32_t *coefs_b, uint32_t sel, uint32_t sel_new, uint32_t ch_en)
{
    int index = 0, wait_cnt = 0;

    if ((codec->REG_104 & sel_new) == 0) {
        //feedforward left ch FIR coefs settings
        for (index = 0; index < g_ch0_fir_len; index++) {
            coefs_a[index] = (int32_t)(coeff[index] * FIR_COEFF_FIXED_Q);
        }
        for (; index < FIR_LEN; index++) {
            coefs_a[index] = 0;
        }
        codec->REG_104 |= sel;
        WAIT_FIR_CONFIG(codec->REG_104, sel_new, 200, wait_cnt);
    } else {
        //feedforward left ch FIR coefs settings
        for (index = 0; index < g_ch0_fir_len; index++) {
            coefs_b[index] = (int32_t)(coeff[index] * FIR_COEFF_FIXED_Q);
        }
        for (; index < FIR_LEN; index++) {
            coefs_b[index] = 0;
        }
        codec->REG_104 &= ~sel;
        WAIT_FIR_CONFIG(codec->REG_104, ~sel_new, 200, wait_cnt);
    }
    codec->REG_0D0 |= ch_en;

    return 0;
}

static int32_t anc_fir_set_coeff_ch_q23(const int32_t *coeff, volatile int32_t *coefs_a,
                volatile int32_t *coefs_b, uint32_t sel, uint32_t sel_new, uint32_t ch_en)
{
    int index = 0, wait_cnt = 0;

    if ((codec->REG_104 & sel_new) == 0) {
        //feedforward left ch FIR coefs settings
        for (index = 0; index < g_ch0_fir_len; index++) {
            coefs_a[index] = (int32_t)coeff[index];
        }
        for (; index < FIR_LEN; index++) {
            coefs_a[index] = 0;
        }
        codec->REG_104 |= sel;
        WAIT_FIR_CONFIG(codec->REG_104, sel_new, 200, wait_cnt);
    } else {
        //feedforward left ch FIR coefs settings
        for (index = 0; index < g_ch0_fir_len; index++) {
            coefs_b[index] = (int32_t)coeff[index];
        }
        for (; index < FIR_LEN; index++) {
            coefs_b[index] = 0;
        }
        codec->REG_104 &= ~sel;
        WAIT_FIR_CONFIG(codec->REG_104, ~sel_new, 200, wait_cnt);
    }
    codec->REG_0D0 |= ch_en;

    return 0;
}

int32_t anc_fir_set_coeff_q23(ANC_FIR_CHANNEL_T ch, const int32_t *coeff, uint32_t num)
{
    if (!g_fir_is_opened) {
        LOG_I( "[%s] please init fir", __func__);
        return -1;
    }

    if (ch == ANC_FIR_CHANNEL_FF_L) {
        ASSERT(g_ch0_fir_len > 0, "[%s] ch0 fir len %d should greater than 0", __func__, g_ch0_fir_len);
        anc_fir_set_coeff_ch_q23(coeff, anc_fb_fir_coefs_l, anc_ff_fir_coefs_l, CODEC_ANC_COEF_SEL_PDU0_FS0,
            CODEC_ANC_COEF_SEL_PDU0_FS0_NEW, CODEC_CODEC_FF_CH0_FIR_EN);

        LOG_I( "FF FIR switching left!");
    } else if (ch == ANC_FIR_CHANNEL_FF_R) {
        ASSERT(g_ch1_fir_len > 0, "[%s] ch1 fir len %d should greater than 0", __func__, g_ch1_fir_len);
        anc_fir_set_coeff_ch_q23(coeff, anc_fb_fir_coefs_r, anc_ff_fir_coefs_r, CODEC_ANC_COEF_SEL_PDU1_FS1,
            CODEC_ANC_COEF_SEL_PDU1_FS1_NEW, CODEC_CODEC_FF_CH1_FIR_EN);

        LOG_I( "FF FIR switching right!");
    } else if (ch == ANC_FIR_CHANNEL_MC_L) {
        ASSERT(g_ch3_fir_len > 0, "[%s] ch1 fir len %d should greater than 0", __func__, g_ch3_fir_len);
        anc_fir_set_coeff_ch_q23(coeff, anc_mc_fir_coefs_b, anc_mc_fir_coefs_a, CODEC_ANC_COEF_SEL_PDU1_FS1,
            CODEC_ANC_COEF_SEL_PDU1_FS1_NEW, CODEC_CODEC_MM0_FIR_EN);

        LOG_I( "MC FIR switching left!");
    }

    return 0;
}

int32_t anc_fir_set_coeff(ANC_FIR_CHANNEL_T ch, const float *coeff, uint32_t num)
{
    if (!g_fir_is_opened) {
        LOG_I( "[%s] please init fir", __func__);
        return -1;
    }

    if (ch == ANC_FIR_CHANNEL_FF_L) {
        ASSERT(g_ch0_fir_len > 0, "[%s] ch0 fir len %d should greater than 0", __func__, g_ch0_fir_len);
        anc_fir_set_coeff_ch(coeff, anc_fb_fir_coefs_l, anc_ff_fir_coefs_l, CODEC_ANC_COEF_SEL_PDU0_FS0,
            CODEC_ANC_COEF_SEL_PDU0_FS0_NEW, CODEC_CODEC_FF_CH0_FIR_EN);

        LOG_I( "FF FIR switching left!");
    } else if (ch == ANC_FIR_CHANNEL_FF_R) {
        ASSERT(g_ch1_fir_len > 0, "[%s] ch1 fir len %d should greater than 0", __func__, g_ch1_fir_len);
        anc_fir_set_coeff_ch(coeff, anc_fb_fir_coefs_r, anc_ff_fir_coefs_r, CODEC_ANC_COEF_SEL_PDU1_FS1,
            CODEC_ANC_COEF_SEL_PDU1_FS1_NEW, CODEC_CODEC_FF_CH1_FIR_EN);

        LOG_I( "FF FIR switching right!");
    } else if (ch == ANC_FIR_CHANNEL_MC_L) {
        ASSERT(g_ch3_fir_len > 0, "[%s] ch1 fir len %d should greater than 0", __func__, g_ch3_fir_len);
        anc_fir_set_coeff_ch(coeff, anc_mc_fir_coefs_b, anc_mc_fir_coefs_a, CODEC_ANC_COEF_SEL_PDU1_FS1,
            CODEC_ANC_COEF_SEL_PDU1_FS1_NEW, CODEC_CODEC_MM0_FIR_EN);

        LOG_I( "MC FIR switching left!");
    }

    return 0;
}

int32_t anc_fir_set_gain_test(uint32_t gain, ANC_FIR_CHANNEL_T ch)
{
    LOG_I( "[%s], gain:%d", __func__, gain);

    float ratio = gain / 10.0;
    g_fir_item[0] = (uint32_t)(ratio * FIR_COEFF_FIXED_Q);

    anc_fir_set_coeff_q23(ch, g_fir_item, FIR_LEN);

    return 0;
}