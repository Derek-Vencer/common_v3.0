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
#if !defined(BESUI_TWS_EN) && !defined(BESUI_STEREO_EN)
#define IIR_COUNTER_FF_L (6)
#define IIR_COUNTER_FF_R (6)
#define IIR_COUNTER_FB_L (5)
#define IIR_COUNTER_FB_R (5)

static const struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_50p7k_mode0 = {
    .anc_cfg_ff_l = {
        .total_gain = 512,
        .iir_bypass_flag = 0,
        .iir_counter = 7,
        .iir_coef[0].coef_b = {0x04026e74,0xf7fba684,0x0401eb10},
        .iir_coef[0].coef_a = {0x08000000,0xf0010638,0x07fef9d7},
        .iir_coef[1].coef_b = {0x07e2d478,0xf05a62f8,0x07c7d95f},
        .iir_coef[1].coef_a = {0x08000000,0xf05a62f8,0x07aaadd8},
        .iir_coef[2].coef_b = {0x080bbea5,0xf039817c,0x07bb5102},
        .iir_coef[2].coef_a = {0x08000000,0xf039817c,0x07c70fa7},
        .iir_coef[3].coef_b = {0x0800b399,0xf000edae,0x07fe5f0e},
        .iir_coef[3].coef_a = {0x08000000,0xf000ed8b,0x07ff1283},
        .iir_coef[4].coef_b = {0x02b628e9,0xfb6ec9e1,0x01f8beff},
        .iir_coef[4].coef_a = {0x08000000,0xf16e8668,0x06af2b62},
        .iir_coef[5].coef_b = {0x076b462b,0xf1d3daf4,0x06e1b5c1},
        .iir_coef[5].coef_a = {0x08000000,0xf1d3daf4,0x064cfbed},
        .iir_coef[6].coef_b = {0x08000000,0xf022991b,0x07ddf8db},
        .iir_coef[6].coef_a = {0x08000000,0xf022991b,0x07ddf8db},
        .iir_coef[7].coef_b = {0,0,0},
        .iir_coef[7].coef_a = {0,0,0},
        .iir_coef[8].coef_b = {0,0,0},
        .iir_coef[8].coef_a = {0,0,0},
        .iir_coef[9].coef_b = {0,0,0},
        .iir_coef[9].coef_a = {0,0,0},
        .iir_coef[10].coef_b = {0,0,0},
        .iir_coef[10].coef_a = {0,0,0},
        .iir_coef[11].coef_b = {0,0,0},
        .iir_coef[11].coef_a = {0,0,0},
        .iir_coef[12].coef_b = {0,0,0},
        .iir_coef[12].coef_a = {0,0,0},
        .limiter_flag = 0,
        .limiter_reserved = 0,
        .limiter_th = 0.0,
        .limiter_att_time = 0.0,
        .limiter_rls_time = 0.0,
        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_ff_r = {
        .total_gain = 0,
        .iir_bypass_flag = 0,
        .iir_counter = 1,
        .iir_coef[0].coef_b = {0x08000000,0xf022991b,0x07ddf8db},
        .iir_coef[0].coef_a = {0x08000000,0xf022991b,0x07ddf8db},
        .iir_coef[1].coef_b = {0,0,0},
        .iir_coef[1].coef_a = {0,0,0},
        .iir_coef[2].coef_b = {0,0,0},
        .iir_coef[2].coef_a = {0,0,0},
        .iir_coef[3].coef_b = {0,0,0},
        .iir_coef[3].coef_a = {0,0,0},
        .iir_coef[4].coef_b = {0,0,0},
        .iir_coef[4].coef_a = {0,0,0},
        .iir_coef[5].coef_b = {0,0,0},
        .iir_coef[5].coef_a = {0,0,0},
        .iir_coef[6].coef_b = {0,0,0},
        .iir_coef[6].coef_a = {0,0,0},
        .iir_coef[7].coef_b = {0,0,0},
        .iir_coef[7].coef_a = {0,0,0},
        .iir_coef[8].coef_b = {0,0,0},
        .iir_coef[8].coef_a = {0,0,0},
        .iir_coef[9].coef_b = {0,0,0},
        .iir_coef[9].coef_a = {0,0,0},
        .iir_coef[10].coef_b = {0,0,0},
        .iir_coef[10].coef_a = {0,0,0},
        .iir_coef[11].coef_b = {0,0,0},
        .iir_coef[11].coef_a = {0,0,0},
        .iir_coef[12].coef_b = {0,0,0},
        .iir_coef[12].coef_a = {0,0,0},
        .limiter_flag = 0,
        .limiter_reserved = 0,
        .limiter_th = 0.0,
        .limiter_att_time = 0.0,
        .limiter_rls_time = 0.0,
        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_fb_l = {
        .total_gain = 512,
        .iir_bypass_flag = 0,
        .iir_counter = 8,
        .iir_coef[0].coef_b = {0x03058000,0xfa108bf2,0x02ed5f4c},
        .iir_coef[0].coef_a = {0x08000000,0xf1174e47,0x06f1483b},
        .iir_coef[1].coef_b = {0x07bc4464,0xf0df97ac,0x0771c4ee},
        .iir_coef[1].coef_a = {0x08000000,0xf0df97ac,0x072e0952},
        .iir_coef[2].coef_b = {0x081077ab,0xf0028426,0x07ed05f6},
        .iir_coef[2].coef_a = {0x08000000,0xf0028426,0x07fd7da1},
        .iir_coef[3].coef_b = {0x07f53c7d,0xf02d001c,0x07df9b3a},
        .iir_coef[3].coef_a = {0x08000000,0xf02d001c,0x07d4d7b6},
        .iir_coef[4].coef_b = {0x0a10ecef,0xebf4518e,0x09fb120c},
        .iir_coef[4].coef_a = {0x08000000,0xf013cafc,0x07ec858d},
        .iir_coef[5].coef_b = {0x08001960,0xf000982e,0x07ff4e85},
        .iir_coef[5].coef_a = {0x08000000,0xf000982a,0x07ff67e1},
        .iir_coef[6].coef_b = {0x0b4a2cd2,0xe9884213,0x0b2e2e11},
        .iir_coef[6].coef_a = {0x08000000,0xf0182b09,0x07e871ed},
        .iir_coef[7].coef_b = {0x0382c7fc,0xfadb0f78,0x023bb097},
        .iir_coef[7].coef_a = {0x08000000,0xf29fefb2,0x05f99858},
        .iir_coef[8].coef_b = {0,0,0},
        .iir_coef[8].coef_a = {0,0,0},
        .iir_coef[9].coef_b = {0,0,0},
        .iir_coef[9].coef_a = {0,0,0},
        .iir_coef[10].coef_b = {0,0,0},
        .iir_coef[10].coef_a = {0,0,0},
        .iir_coef[11].coef_b = {0,0,0},
        .iir_coef[11].coef_a = {0,0,0},
        .iir_coef[12].coef_b = {0,0,0},
        .iir_coef[12].coef_a = {0,0,0},
        .limiter_flag = 0,
        .limiter_reserved = 0,
        .limiter_th = -12.0,
        .limiter_att_time = 10.0,
        .limiter_rls_time = 1000.0,
        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_fb_r = {
        .total_gain = 0,
        .iir_bypass_flag = 0,
        .iir_counter = 1,
        .iir_coef[0].coef_b = {0x08000000,0xf022991b,0x07ddf8db},
        .iir_coef[0].coef_a = {0x08000000,0xf022991b,0x07ddf8db},
        .iir_coef[1].coef_b = {0,0,0},
        .iir_coef[1].coef_a = {0,0,0},
        .iir_coef[2].coef_b = {0,0,0},
        .iir_coef[2].coef_a = {0,0,0},
        .iir_coef[3].coef_b = {0,0,0},
        .iir_coef[3].coef_a = {0,0,0},
        .iir_coef[4].coef_b = {0,0,0},
        .iir_coef[4].coef_a = {0,0,0},
        .iir_coef[5].coef_b = {0,0,0},
        .iir_coef[5].coef_a = {0,0,0},
        .iir_coef[6].coef_b = {0,0,0},
        .iir_coef[6].coef_a = {0,0,0},
        .iir_coef[7].coef_b = {0,0,0},
        .iir_coef[7].coef_a = {0,0,0},
        .iir_coef[8].coef_b = {0,0,0},
        .iir_coef[8].coef_a = {0,0,0},
        .iir_coef[9].coef_b = {0,0,0},
        .iir_coef[9].coef_a = {0,0,0},
        .iir_coef[10].coef_b = {0,0,0},
        .iir_coef[10].coef_a = {0,0,0},
        .iir_coef[11].coef_b = {0,0,0},
        .iir_coef[11].coef_a = {0,0,0},
        .iir_coef[12].coef_b = {0,0,0},
        .iir_coef[12].coef_a = {0,0,0},
        .limiter_flag = 0,
        .limiter_reserved = 0,
        .limiter_th = 0.0,
        .limiter_att_time = 0.0,
        .limiter_rls_time = 0.0,
        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_tt_l = {
        .total_gain = 0,
        .iir_bypass_flag = 0,
        .iir_counter = 1,
        .iir_coef[0].coef_b = {0x08000000,0xf022991b,0x07ddf8db},
        .iir_coef[0].coef_a = {0x08000000,0xf022991b,0x07ddf8db},
        .iir_coef[1].coef_b = {0,0,0},
        .iir_coef[1].coef_a = {0,0,0},
        .iir_coef[2].coef_b = {0,0,0},
        .iir_coef[2].coef_a = {0,0,0},
        .iir_coef[3].coef_b = {0,0,0},
        .iir_coef[3].coef_a = {0,0,0},
        .iir_coef[4].coef_b = {0,0,0},
        .iir_coef[4].coef_a = {0,0,0},
        .iir_coef[5].coef_b = {0,0,0},
        .iir_coef[5].coef_a = {0,0,0},
        .iir_coef[6].coef_b = {0,0,0},
        .iir_coef[6].coef_a = {0,0,0},
        .iir_coef[7].coef_b = {0,0,0},
        .iir_coef[7].coef_a = {0,0,0},
        .iir_coef[8].coef_b = {0,0,0},
        .iir_coef[8].coef_a = {0,0,0},
        .iir_coef[9].coef_b = {0,0,0},
        .iir_coef[9].coef_a = {0,0,0},
        .iir_coef[10].coef_b = {0,0,0},
        .iir_coef[10].coef_a = {0,0,0},
        .iir_coef[11].coef_b = {0,0,0},
        .iir_coef[11].coef_a = {0,0,0},
        .iir_coef[12].coef_b = {0,0,0},
        .iir_coef[12].coef_a = {0,0,0},
        .limiter_flag = 0,
        .limiter_reserved = 0,
        .limiter_th = 0.0,
        .limiter_att_time = 0.0,
        .limiter_rls_time = 0.0,
        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
   .anc_cfg_tt_r = {
        .total_gain = 0,
        .iir_bypass_flag = 0,
        .iir_counter = 1,
        .iir_coef[0].coef_b = {0x08000000,0xf022991b,0x07ddf8db},
        .iir_coef[0].coef_a = {0x08000000,0xf022991b,0x07ddf8db},
        .iir_coef[1].coef_b = {0,0,0},
        .iir_coef[1].coef_a = {0,0,0},
        .iir_coef[2].coef_b = {0,0,0},
        .iir_coef[2].coef_a = {0,0,0},
        .iir_coef[3].coef_b = {0,0,0},
        .iir_coef[3].coef_a = {0,0,0},
        .iir_coef[4].coef_b = {0,0,0},
        .iir_coef[4].coef_a = {0,0,0},
        .iir_coef[5].coef_b = {0,0,0},
        .iir_coef[5].coef_a = {0,0,0},
        .iir_coef[6].coef_b = {0,0,0},
        .iir_coef[6].coef_a = {0,0,0},
        .iir_coef[7].coef_b = {0,0,0},
        .iir_coef[7].coef_a = {0,0,0},
        .iir_coef[8].coef_b = {0,0,0},
        .iir_coef[8].coef_a = {0,0,0},
        .iir_coef[9].coef_b = {0,0,0},
        .iir_coef[9].coef_a = {0,0,0},
        .iir_coef[10].coef_b = {0,0,0},
        .iir_coef[10].coef_a = {0,0,0},
        .iir_coef[11].coef_b = {0,0,0},
        .iir_coef[11].coef_a = {0,0,0},
        .iir_coef[12].coef_b = {0,0,0},
        .iir_coef[12].coef_a = {0,0,0},
        .limiter_flag = 0,
        .limiter_reserved = 0,
        .limiter_th = 0.0,
        .limiter_att_time = 0.0,
        .limiter_rls_time = 0.0,
        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_mc_l = {
        .total_gain = 512,
        .iir_bypass_flag = 0,
        .iir_counter = 6,
        .iir_coef[0].coef_b = {0x1c5e3441,0xc747507d,0x1c5a7b62},
        .iir_coef[0].coef_a = {0x08000000,0xf00383f4,0x07fc7c6f},
        .iir_coef[1].coef_b = {0x0733feaa,0xf19e226b,0x072f5d69},
        .iir_coef[1].coef_a = {0x08000000,0xf19e226b,0x06635c13},
        .iir_coef[2].coef_b = {0x029bd9eb,0xfb272388,0x02483ad0},
        .iir_coef[2].coef_a = {0x08000000,0xf09fec15,0x076b4c2e},
        .iir_coef[3].coef_b = {0x0802ff95,0xf007c1bc,0x07f54be7},
        .iir_coef[3].coef_a = {0x08000000,0xf007c1bc,0x07f84b7c},
        .iir_coef[4].coef_b = {0x07284a91,0xf2fa883a,0x063b09b6},
        .iir_coef[4].coef_a = {0x08000000,0xf2fa883a,0x05635447},
        .iir_coef[5].coef_b = {0x07ffe385,0xf0005351,0x07ffc92d},
        .iir_coef[5].coef_a = {0x08000000,0xf0005351,0x07ffacb1},
        .iir_coef[6].coef_b = {0,0,0},
        .iir_coef[6].coef_a = {0,0,0},
        .iir_coef[7].coef_b = {0,0,0},
        .iir_coef[7].coef_a = {0,0,0},
        .iir_coef[8].coef_b = {0,0,0},
        .iir_coef[8].coef_a = {0,0,0},
        .iir_coef[9].coef_b = {0,0,0},
        .iir_coef[9].coef_a = {0,0,0},
        .iir_coef[10].coef_b = {0,0,0},
        .iir_coef[10].coef_a = {0,0,0},
        .iir_coef[11].coef_b = {0,0,0},
        .iir_coef[11].coef_a = {0,0,0},
        .iir_coef[12].coef_b = {0,0,0},
        .iir_coef[12].coef_a = {0,0,0},
        .limiter_flag = 0,
        .limiter_reserved = 0,
        .limiter_th = 0.0,
        .limiter_att_time = 0.0,
        .limiter_rls_time = 0.0,
        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_mc_r = {
        .total_gain = 0,
        .iir_bypass_flag = 0,
        .iir_counter = 1,
        .iir_coef[0].coef_b = {0x08000000,0xf022991b,0x07ddf8db},
        .iir_coef[0].coef_a = {0x08000000,0xf022991b,0x07ddf8db},
        .iir_coef[1].coef_b = {0,0,0},
        .iir_coef[1].coef_a = {0,0,0},
        .iir_coef[2].coef_b = {0,0,0},
        .iir_coef[2].coef_a = {0,0,0},
        .iir_coef[3].coef_b = {0,0,0},
        .iir_coef[3].coef_a = {0,0,0},
        .iir_coef[4].coef_b = {0,0,0},
        .iir_coef[4].coef_a = {0,0,0},
        .iir_coef[5].coef_b = {0,0,0},
        .iir_coef[5].coef_a = {0,0,0},
        .iir_coef[6].coef_b = {0,0,0},
        .iir_coef[6].coef_a = {0,0,0},
        .iir_coef[7].coef_b = {0,0,0},
        .iir_coef[7].coef_a = {0,0,0},
        .iir_coef[8].coef_b = {0,0,0},
        .iir_coef[8].coef_a = {0,0,0},
        .iir_coef[9].coef_b = {0,0,0},
        .iir_coef[9].coef_a = {0,0,0},
        .iir_coef[10].coef_b = {0,0,0},
        .iir_coef[10].coef_a = {0,0,0},
        .iir_coef[11].coef_b = {0,0,0},
        .iir_coef[11].coef_a = {0,0,0},
        .iir_coef[12].coef_b = {0,0,0},
        .iir_coef[12].coef_a = {0,0,0},
        .limiter_flag = 0,
        .limiter_reserved = 0,
        .limiter_th = 0.0,
        .limiter_att_time = 0.0,
        .limiter_rls_time = 0.0,
        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
};

static const struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_48k_mode0 = {
    .anc_cfg_ff_l = {
        .total_gain = 512*2,

        .iir_bypass_flag=0,
        .iir_counter=IIR_COUNTER_FF_L,

        .iir_coef[0].coef_b={0x8000000,0,0},
        .iir_coef[0].coef_a={0x8000000,0,0},

        .iir_coef[1].coef_b={0x8000000,0,0},
        .iir_coef[1].coef_a={0x8000000,0,0},

        .iir_coef[2].coef_b={0x8000000,0,0},
        .iir_coef[2].coef_a={0x8000000,0,0},

        .iir_coef[3].coef_b={0x8000000,0,0},
        .iir_coef[3].coef_a={0x8000000,0,0},

        .iir_coef[4].coef_b={0x8000000,0,0},
        .iir_coef[4].coef_a={0x8000000,0,0},

        .iir_coef[5].coef_b={0x8000000,0,0},
        .iir_coef[5].coef_a={0x8000000,0,0},

/*        .fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/
        .dac_gain_offset=0,
        .adc_gain_offset=(0)*4,
    },
    .anc_cfg_fb_l = {
        .total_gain = 512/2,

        .iir_bypass_flag=0,
        .iir_counter=IIR_COUNTER_FB_L,

        .iir_coef[0].coef_b={0x8000000,0,0},
        .iir_coef[0].coef_a={0x8000000,0,0},

        .iir_coef[1].coef_b={0x8000000,0,0},
        .iir_coef[1].coef_a={0x8000000,0,0},

        .iir_coef[2].coef_b={0x8000000,0,0},
        .iir_coef[2].coef_a={0x8000000,0,0},

        .iir_coef[3].coef_b={0x8000000,0,0},
        .iir_coef[3].coef_a={0x8000000,0,0},

        .iir_coef[4].coef_b={0x8000000,0,0},
        .iir_coef[4].coef_a={0x8000000,0,0},

        .iir_coef[5].coef_b={0x8000000,0,0},
        .iir_coef[5].coef_a={0x8000000,0,0},

/*        .fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/
        .dac_gain_offset=0,
        .adc_gain_offset=(0)*4,
    },
    .anc_cfg_mc_l = {
        .total_gain = 512*2,

        .iir_bypass_flag=0,
        .iir_counter=5,

        .iir_coef[0].coef_b={0x8000000,0,0},
        .iir_coef[0].coef_a={0x8000000,0,0},

        .iir_coef[1].coef_b={0x8000000,0,0},
        .iir_coef[1].coef_a={0x8000000,0,0},

        .iir_coef[2].coef_b={0x8000000,0,0},
        .iir_coef[2].coef_a={0x8000000,0,0},

        .iir_coef[3].coef_b={0x8000000,0,0},
        .iir_coef[3].coef_a={0x8000000,0,0},

        .iir_coef[4].coef_b={0x8000000,0,0},
        .iir_coef[4].coef_a={0x8000000,0,0},

        .iir_coef[5].coef_b={0x8000000,0,0},
        .iir_coef[5].coef_a={0x8000000,0,0},

        .dac_gain_offset=0,
        .adc_gain_offset=(0)*4,
    },
};

static const struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_44p1k_mode0 = {
    .anc_cfg_ff_l = {
        .total_gain =512*2,

        .iir_bypass_flag=0,
        .iir_counter=IIR_COUNTER_FF_L,

        .iir_coef[0].coef_b={0x8000000,0,0},
        .iir_coef[0].coef_a={0x8000000,0,0},

        .iir_coef[1].coef_b={0x8000000,0,0},
        .iir_coef[1].coef_a={0x8000000,0,0},

        .iir_coef[2].coef_b={0x8000000,0,0},
        .iir_coef[2].coef_a={0x8000000,0,0},

        .iir_coef[3].coef_b={0x8000000,0,0},
        .iir_coef[3].coef_a={0x8000000,0,0},

        .iir_coef[4].coef_b={0x8000000,0,0},
        .iir_coef[4].coef_a={0x8000000,0,0},

        .iir_coef[5].coef_b={0x8000000,0,0},
        .iir_coef[5].coef_a={0x8000000,0,0},

/*        .fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/
        .dac_gain_offset=0,
        .adc_gain_offset=(0)*4,
    },
    .anc_cfg_fb_l = {
        .total_gain = 512/2,

        .iir_bypass_flag=0,
        .iir_counter=IIR_COUNTER_FB_L,

        .iir_coef[0].coef_b={0x8000000,0,0},
        .iir_coef[0].coef_a={0x8000000,0,0},

        .iir_coef[1].coef_b={0x8000000,0,0},
        .iir_coef[1].coef_a={0x8000000,0,0},

        .iir_coef[2].coef_b={0x8000000,0,0},
        .iir_coef[2].coef_a={0x8000000,0,0},

        .iir_coef[3].coef_b={0x8000000,0,0},
        .iir_coef[3].coef_a={0x8000000,0,0},

        .iir_coef[4].coef_b={0x8000000,0,0},
        .iir_coef[4].coef_a={0x8000000,0,0},

        .iir_coef[5].coef_b={0x8000000,0,0},
        .iir_coef[5].coef_a={0x8000000,0,0},

/*        .fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/
        .dac_gain_offset=0,
        .adc_gain_offset=(0)*4,
    },
    .anc_cfg_mc_l = {
        .total_gain = 512,

        .iir_bypass_flag=0,
        .iir_counter=5,

        .iir_coef[0].coef_b={0x8000000,0,0},
        .iir_coef[0].coef_a={0x8000000,0,0},

        .iir_coef[1].coef_b={0x8000000,0,0},
        .iir_coef[1].coef_a={0x8000000,0,0},

        .iir_coef[2].coef_b={0x8000000,0,0},
        .iir_coef[2].coef_a={0x8000000,0,0},

        .iir_coef[3].coef_b={0x8000000,0,0},
        .iir_coef[3].coef_a={0x8000000,0,0},

        .iir_coef[4].coef_b={0x8000000,0,0},
        .iir_coef[4].coef_a={0x8000000,0,0},

        .iir_coef[5].coef_b={0x8000000,0,0},
        .iir_coef[5].coef_a={0x8000000,0,0},

        .dac_gain_offset=0,
        .adc_gain_offset=(0)*4,
    },
};
#endif //#if !defined(BESUI_TWS_EN) && !defined(BESUI_STEREO_EN)

const struct_anc_cfg * anc_coef_list_50p7k[ANC_COEF_LIST_NUM] = {
    &AncFirCoef_50p7k_mode0,
#if defined(BESUI_TWS_EN) || defined(BESUI_STEREO_EN)
    &AncFirCoef_50p7k_mode1,
#endif
};

const struct_anc_cfg * anc_coef_list_48k[ANC_COEF_LIST_NUM] = {
    &AncFirCoef_48k_mode0,
#if defined(BESUI_TWS_EN) || defined(BESUI_STEREO_EN)
    &AncFirCoef_48k_mode1,
#endif
};

const struct_anc_cfg * anc_coef_list_44p1k[ANC_COEF_LIST_NUM] = {
    &AncFirCoef_44p1k_mode0,
#if defined(BESUI_TWS_EN) || defined(BESUI_STEREO_EN)
    &AncFirCoef_44p1k_mode1,
#endif
};

static const struct_psap_cfg POSSIBLY_UNUSED PsapFirCoef_50p7k_mode0 = {
    .psap_cfg_l = {
        .psap_total_gain = 723,
        .psap_band_num = 9,
        .psap_band_gain={0, 476222470, 672682118, 534330399, 950188747, 1066129310, 1066129310, 424433723, 0},
        .psap_iir_coef[0].iir0.coef_b={131557370, -265754754, 134217728, 0},
        .psap_iir_coef[0].iir0.coef_a={134217728, -265754754, 131557370, 0},
        .psap_iir_coef[0].iir1.coef_b={-131557370, 397299677, -399959783, 134217728},
        .psap_iir_coef[0].iir1.coef_a={134217728, -399959783, 397299677, -131557370},
        .psap_iir_coef[1].iir0.coef_b={129986488, -264152445, 134217728, 0},
        .psap_iir_coef[1].iir0.coef_a={134217728, -264152445, 129986488, 0},
        .psap_iir_coef[1].iir1.coef_b={-129986488, 394107454, -398337669, 134217728},
        .psap_iir_coef[1].iir1.coef_a={134217728, -398337669, 394107454, -129986488},
        .psap_iir_coef[2].iir0.coef_b={125134130, -259108702, 134217728, 0},
        .psap_iir_coef[2].iir0.coef_a={134217728, -259108702, 125134130, 0},
        .psap_iir_coef[2].iir1.coef_b={-125134129, 384097929, -393171000, 134217728},
        .psap_iir_coef[2].iir1.coef_a={134217728, -393171000, 384097929, -125134129},
        .psap_iir_coef[3].iir0.coef_b={120220377, -253849171, 134217728, 0},
        .psap_iir_coef[3].iir0.coef_a={134217728, -253849171, 120220377, 0},
        .psap_iir_coef[3].iir1.coef_b={-120220366, 373726258, -387683554, 134217728},
        .psap_iir_coef[3].iir1.coef_a={134217728, -387683554, 373726258, -120220366},
        .psap_iir_coef[4].iir0.coef_b={109848832, -242200690, 134217728, 0},
        .psap_iir_coef[4].iir0.coef_a={134217728, -242200690, 109848832, 0},
        .psap_iir_coef[4].iir1.coef_b={-109848632, 351018377, -375156857, 134217728},
        .psap_iir_coef[4].iir1.coef_a={134217728, -375156857, 351018377, -109848632},
        .psap_iir_coef[5].iir0.coef_b={81171724, -205166742, 134217728, 0},
        .psap_iir_coef[5].iir0.coef_a={134217728, -205166742, 81171724, 0},
        .psap_iir_coef[5].iir1.coef_b={-81156770, 281801460, -331721088, 134217728},
        .psap_iir_coef[5].iir1.coef_a={134217728, -331721088, 281801460, -81156770},
        .psap_iir_coef[6].iir0.coef_b={66181540, -181899702, 134217728, 0},
        .psap_iir_coef[6].iir0.coef_a={134217728, -181899702, 66181540, 0},
        .psap_iir_coef[6].iir1.coef_b={-66113394, 241306967, -301500735, 134217728},
        .psap_iir_coef[6].iir1.coef_a={134217728, -301500735, 241306967, -66113394},
        .psap_iir_coef[7].iir0.coef_b={62863906, -176244865, 134217728, 0},
        .psap_iir_coef[7].iir0.coef_a={134217728, -176244865, 62863906, 0},
        .psap_iir_coef[7].iir1.coef_b={-62771443, 231892680, -293809571, 134217728},
        .psap_iir_coef[7].iir1.coef_a={134217728, -293809571, 231892680, -62771443},
        .psap_cpd_cfg[0]={27069, 0, 27069, 0, 10505, -29491, 32689, 79, 32766, 2, 29491, 3277, 0},
        .psap_cpd_cfg[1]={27069, 0, 27069, 0, 10505, -29491, 32689, 79, 32766, 2, 29491, 3277, 0},
        .psap_cpd_cfg[2]={27069, 0, 27069, 0, 10505, -29491, 32689, 79, 32766, 2, 29491, 3277, 0},
        .psap_cpd_cfg[3]={27069, 0, 27069, 0, 10505, -29491, 32689, 79, 32766, 2, 29491, 3277, 0},
        .psap_cpd_cfg[4]={27069, 0, 27069, 0, 10505, -29491, 32689, 79, 32766, 2, 29491, 3277, 0},
        .psap_cpd_cfg[5]={27069, 0, 27069, 0, 10505, -29491, 32689, 79, 32766, 2, 29491, 3277, 0},
        .psap_cpd_cfg[6]={27069, 0, 27069, 0, 10505, -29491, 32689, 79, 32766, 2, 29491, 3277, 0},
        .psap_cpd_cfg[7]={27069, 0, 27069, 0, 10505, -29491, 32689, 79, 32766, 2, 29491, 3277, 0},
        .psap_limiter_cfg={524287, 31991, 777, 32752, 16, 0},
        .psap_dac_gain_offset=0,
        .psap_adc_gain_offset=-12,
    }
};

const struct_psap_cfg * psap_coef_list_50p7k[PSAP_COEF_LIST_NUM] = {
    &PsapFirCoef_50p7k_mode0,
};

const struct_psap_cfg * psap_coef_list_48k[PSAP_COEF_LIST_NUM] = {
    &PsapFirCoef_50p7k_mode0,
};

const struct_psap_cfg * psap_coef_list_44p1k[PSAP_COEF_LIST_NUM] = {
    &PsapFirCoef_50p7k_mode0,
};
