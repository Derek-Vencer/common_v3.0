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
    #ifndef VOICE_ASSIST_ADA_IIR
    .anc_cfg_ff_l = {
		.total_gain = 350,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_L,

		.iir_coef[0].coef_b={42462788,    -84862242,     42399478},
		.iir_coef[0].coef_a={134217728,   -268358003,    134140286},

		.iir_coef[1].coef_b={135905569,   -267224817,    131334465},
		.iir_coef[1].coef_a={134217728,   -267224817,    133022306},

		.iir_coef[2].coef_b={132936489,   -263935268,    131067941},
		.iir_coef[2].coef_a={134217728,   -263935268,    129786702},

		.iir_coef[3].coef_b={131758190,   -257297054,    126191415},
		.iir_coef[3].coef_a={134217728,   -257297054,    123731878},

		.iir_coef[4].coef_b={0x8000000,0,0},
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

/*		.fir_bypass_flag=1,
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
        .total_gain = 350,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FB_L,

		.iir_coef[0].coef_b={  27461831,    -54408898,     27001841},
		.iir_coef[0].coef_a={134217728,   -216605724,     82606056},

		.iir_coef[1].coef_b={138294078,   -267600712,    129323227},
		.iir_coef[1].coef_a={134217728,   -267600712,    133399577},

		.iir_coef[2].coef_b={134500015,   -268177932,    133678688},
		.iir_coef[2].coef_a={134217728,   -268177932,    133960975},

		.iir_coef[3].coef_b={133629164,   -264794659,    131257050},
		.iir_coef[3].coef_a={134217728,   -264794659,    130668486},

		.iir_coef[4].coef_b={0x8000000,0,0},
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

/*		.fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/
		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    #else
    .anc_cfg_ff_l = {
        .total_gain = 512,
        .iir_bypass_flag = 0,

        //beijing
        .iir_counter = 6,
        .iir_coef[0].coef_b = {0x05ffe37e,0xf400afe2,0x05ff6cad},
        .iir_coef[0].coef_a = {0x08000000,0xf0005697,0x07ffa96e},
        .iir_coef[1].coef_b = {0x07fee72e,0xf00ab3ed,0x07f66e11},
        .iir_coef[1].coef_a = {0x08000000,0xf00ab3ed,0x07f5553f},
        .iir_coef[2].coef_b = {0x07ffce5a,0xf0022f6f,0x07fe030b},
        .iir_coef[2].coef_a = {0x08000000,0xf0022f6f,0x07fdd165},
        .iir_coef[3].coef_b = {0x08041ff0,0xf0207d53,0x07dc03bf},
        .iir_coef[3].coef_a = {0x08000000,0xf0207d53,0x07e023af},
        .iir_coef[4].coef_b = {0x029f7210,0xfb313e00,0x023e7e4c},
        .iir_coef[4].coef_a = {0x08000000,0xf0bb96d6,0x07539785},
        .iir_coef[5].coef_b = {0x075fafb0,0xf1c5615b,0x06f421c1},
        .iir_coef[5].coef_a = {0x08000000,0xf1c5615b,0x0653d170},

/*		.fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/
		.dac_gain_offset=0,
		.adc_gain_offset= -24, // (0)*4
    },
    .anc_cfg_fb_l = {
        .total_gain = 512,
        .iir_bypass_flag = 0,
        .iir_counter = 6,
        .iir_coef[0].coef_b = {0x110c37ba,0xde61c857,0x10acc11e},
        .iir_coef[0].coef_a = {0x08000000,0xf0fbbe6f,0x071034fb},
        .iir_coef[1].coef_b = {0x04855a96,0xf722ea8e,0x04596e08},
        .iir_coef[1].coef_a = {0x08000000,0xf03c4608,0x07c56d24},
        .iir_coef[2].coef_b = {0x0802921c,0xf0069ff2,0x07f6d2b5},
        .iir_coef[2].coef_a = {0x08000000,0xf0069ff2,0x07f964d0},
        .iir_coef[3].coef_b = {0x0800985a,0xf002e38c,0x07fc850b},
        .iir_coef[3].coef_a = {0x08000000,0xf002e38c,0x07fd1d65},
        .iir_coef[4].coef_b = {0x08017c7a,0xf0073dbe,0x07f74ef7},
        .iir_coef[4].coef_a = {0x08000000,0xf0073dbe,0x07f8cb71},
        .iir_coef[5].coef_b = {0x07a15426,0xf0eb2d9a,0x0778602f},
        .iir_coef[5].coef_a = {0x08000000,0xf0eb2d9a,0x0719b455},
        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_tt_l = {
		//mute TT iir config band 0.
		.total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=8,

		//NOTE: TT+FF bands num set 1+7.
		//TT: band 0
		.iir_coef[0].coef_b={0x08000000,0xf024cb39,0x07dbd999},
		.iir_coef[0].coef_a={0x08000000,0xf024cb39,0x07dbd999},

		//FF: band 1~7
		.iir_coef[1].coef_b={0x08000000,0xf024cb39,0x07dbd999},
		.iir_coef[1].coef_a={0x08000000,0xf024cb39,0x07dbd999},
		.iir_coef[2].coef_b={0x08000000,0xf024cb39,0x07dbd999},
		.iir_coef[2].coef_a={0x08000000,0xf024cb39,0x07dbd999},
		.iir_coef[3].coef_b={0x08000000,0xf024cb39,0x07dbd999},
		.iir_coef[3].coef_a={0x08000000,0xf024cb39,0x07dbd999},
		.iir_coef[4].coef_b={0x08000000,0xf024cb39,0x07dbd999},
		.iir_coef[4].coef_a={0x08000000,0xf024cb39,0x07dbd999},
		.iir_coef[5].coef_b={0x08000000,0xf024cb39,0x07dbd999},
		.iir_coef[5].coef_a={0x08000000,0xf024cb39,0x07dbd999},

		.iir_coef[6].coef_b={0x08000000,0xf024cb39,0x07dbd999},
		.iir_coef[6].coef_a={0x08000000,0xf024cb39,0x07dbd999},
		.iir_coef[7].coef_b={0x08000000,0xf024cb39,0x07dbd999},
		.iir_coef[7].coef_a={0x08000000,0xf024cb39,0x07dbd999},

		.dac_gain_offset=0,
		.adc_gain_offset= -24,
    },
    .anc_cfg_mc_l = {
        .total_gain = 512,
        .iir_bypass_flag = 0,
        .iir_counter = 5,
        .iir_coef[0].coef_b = {0x05bb982e,0xf48c36ff,0x05b830ef},
        .iir_coef[0].coef_a = {0x08000000,0xf00225e0,0x07fdda48},
        .iir_coef[1].coef_b = {0x0801f05e,0xf006a3ee,0x07f76e57},
        .iir_coef[1].coef_a = {0x08000000,0xf006a3ee,0x07f95eb5},
        .iir_coef[2].coef_b = {0x080317ef,0xf00c1b92,0x07f0dbfd},
        .iir_coef[2].coef_a = {0x08000000,0xf00c1b92,0x07f3f3ec},
        .iir_coef[3].coef_b = {0x0483a30b,0xf7187409,0x04644641},
        .iir_coef[3].coef_a = {0x08000000,0xf02a2fe5,0x07d62d70},
        .iir_coef[4].coef_b = {0x07fe8c8c,0xf0115ef1,0x07f03395},
        .iir_coef[4].coef_a = {0x08000000,0xf0115ef1,0x07eec020},
        .iir_coef[5].coef_b = {0,0,0},
        .iir_coef[5].coef_a = {0,0,0},
        .iir_coef[6].coef_b = {0,0,0},
        .iir_coef[6].coef_a = {0,0,0},
        .iir_coef[7].coef_b = {0,0,0},
        .iir_coef[7].coef_a = {0,0,0},
        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    #endif
};

#ifdef VOICE_ASSIST_ADA_IIR
static struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_50p7k_mode1 = {
    .anc_cfg_ff_l = {
        .total_gain = 512,
        .iir_bypass_flag = 0,

        //beijing
        .iir_counter = 6,
        .iir_coef[0].coef_b = {0x05ffe37e,0xf400afe2,0x05ff6cad},
        .iir_coef[0].coef_a = {0x08000000,0xf0005697,0x07ffa96e},
        .iir_coef[1].coef_b = {0x07fee72e,0xf00ab3ed,0x07f66e11},
        .iir_coef[1].coef_a = {0x08000000,0xf00ab3ed,0x07f5553f},
        .iir_coef[2].coef_b = {0x07ffce5a,0xf0022f6f,0x07fe030b},
        .iir_coef[2].coef_a = {0x08000000,0xf0022f6f,0x07fdd165},
        .iir_coef[3].coef_b = {0x08041ff0,0xf0207d53,0x07dc03bf},
        .iir_coef[3].coef_a = {0x08000000,0xf0207d53,0x07e023af},
        .iir_coef[4].coef_b = {0x029f7210,0xfb313e00,0x023e7e4c},
        .iir_coef[4].coef_a = {0x08000000,0xf0bb96d6,0x07539785},
        .iir_coef[5].coef_b = {0x075fafb0,0xf1c5615b,0x06f421c1},
        .iir_coef[5].coef_a = {0x08000000,0xf1c5615b,0x0653d170},

/*		.fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/
		.dac_gain_offset=0,
		.adc_gain_offset= -24, // (0)*4
    },
    .anc_cfg_fb_l = {
        .total_gain = 512,
        .iir_bypass_flag = 0,
        .iir_counter = 6,
        .iir_coef[0].coef_b = {0x110c37ba,0xde61c857,0x10acc11e},
        .iir_coef[0].coef_a = {0x08000000,0xf0fbbe6f,0x071034fb},
        .iir_coef[1].coef_b = {0x04855a96,0xf722ea8e,0x04596e08},
        .iir_coef[1].coef_a = {0x08000000,0xf03c4608,0x07c56d24},
        .iir_coef[2].coef_b = {0x0802921c,0xf0069ff2,0x07f6d2b5},
        .iir_coef[2].coef_a = {0x08000000,0xf0069ff2,0x07f964d0},
        .iir_coef[3].coef_b = {0x0800985a,0xf002e38c,0x07fc850b},
        .iir_coef[3].coef_a = {0x08000000,0xf002e38c,0x07fd1d65},
        .iir_coef[4].coef_b = {0x08017c7a,0xf0073dbe,0x07f74ef7},
        .iir_coef[4].coef_a = {0x08000000,0xf0073dbe,0x07f8cb71},
        .iir_coef[5].coef_b = {0x07a15426,0xf0eb2d9a,0x0778602f},
        .iir_coef[5].coef_a = {0x08000000,0xf0eb2d9a,0x0719b455},
        .iir_coef[6].coef_b = {0,0,0},
        .iir_coef[6].coef_a = {0,0,0},
        .iir_coef[7].coef_b = {0,0,0},
        .iir_coef[7].coef_a = {0,0,0},
        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_tt_l = {
		//mute TT iir config band 0.
		.total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=8,

		//NOTE: TT+FF bands num set 1+7.
		//TT: band 0
		.iir_coef[0].coef_b={0x08000000,0xf024cb39,0x07dbd999},
		.iir_coef[0].coef_a={0x08000000,0xf024cb39,0x07dbd999},

		//FF: band 1~7
		.iir_coef[1].coef_b={0x08000000,0xf024cb39,0x07dbd999},
		.iir_coef[1].coef_a={0x08000000,0xf024cb39,0x07dbd999},
		.iir_coef[2].coef_b={0x08000000,0xf024cb39,0x07dbd999},
		.iir_coef[2].coef_a={0x08000000,0xf024cb39,0x07dbd999},
		.iir_coef[3].coef_b={0x08000000,0xf024cb39,0x07dbd999},
		.iir_coef[3].coef_a={0x08000000,0xf024cb39,0x07dbd999},
		.iir_coef[4].coef_b={0x08000000,0xf024cb39,0x07dbd999},
		.iir_coef[4].coef_a={0x08000000,0xf024cb39,0x07dbd999},
		.iir_coef[5].coef_b={0x08000000,0xf024cb39,0x07dbd999},
		.iir_coef[5].coef_a={0x08000000,0xf024cb39,0x07dbd999},

		.iir_coef[6].coef_b={0x08000000,0xf024cb39,0x07dbd999},
		.iir_coef[6].coef_a={0x08000000,0xf024cb39,0x07dbd999},
		.iir_coef[7].coef_b={0x08000000,0xf024cb39,0x07dbd999},
		.iir_coef[7].coef_a={0x08000000,0xf024cb39,0x07dbd999},

		.dac_gain_offset=0,
		.adc_gain_offset= -24,
    },
    .anc_cfg_mc_l = {
        .total_gain = 512,
        .iir_bypass_flag = 0,
        .iir_counter = 5,
        .iir_coef[0].coef_b = {0x05bb982e,0xf48c36ff,0x05b830ef},
        .iir_coef[0].coef_a = {0x08000000,0xf00225e0,0x07fdda48},
        .iir_coef[1].coef_b = {0x0801f05e,0xf006a3ee,0x07f76e57},
        .iir_coef[1].coef_a = {0x08000000,0xf006a3ee,0x07f95eb5},
        .iir_coef[2].coef_b = {0x080317ef,0xf00c1b92,0x07f0dbfd},
        .iir_coef[2].coef_a = {0x08000000,0xf00c1b92,0x07f3f3ec},
        .iir_coef[3].coef_b = {0x0483a30b,0xf7187409,0x04644641},
        .iir_coef[3].coef_a = {0x08000000,0xf02a2fe5,0x07d62d70},
        .iir_coef[4].coef_b = {0x07fe8c8c,0xf0115ef1,0x07f03395},
        .iir_coef[4].coef_a = {0x08000000,0xf0115ef1,0x07eec020},
        .iir_coef[5].coef_b = {0,0,0},
        .iir_coef[5].coef_a = {0,0,0},
        .iir_coef[6].coef_b = {0,0,0},
        .iir_coef[6].coef_a = {0,0,0},
        .iir_coef[7].coef_b = {0,0,0},
        .iir_coef[7].coef_a = {0,0,0},
        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
};
#endif

static const struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_48k_mode0 = {
    .anc_cfg_ff_l = {
		.total_gain = 312,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_L,

		.iir_coef[0].coef_b={42463913,    -84860822,     42396935},
		.iir_coef[0].coef_a={134217728,   -268353516,    134135801},

		.iir_coef[1].coef_b={136002894,   -267154076,    131168209},
		.iir_coef[1].coef_a={134217728,   -267154076,    132953376},

		.iir_coef[2].coef_b={132863566,   -263674901,    130888668},
		.iir_coef[2].coef_a={134217728,   -263674901,    129534506},

		.iir_coef[3].coef_b={131621817,   -256639526,    125746382},
		.iir_coef[3].coef_a={134217728,   -256639526,    123150471},

		.iir_coef[4].coef_b={0x8000000,0,0},
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

/*		.fir_bypass_flag=1,
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
        .total_gain = 511,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FB_L,

		.iir_coef[0].coef_b={  27172676,    -53803459,     26691412},
		.iir_coef[0].coef_a={134217728,   -214195429,     80219070},

		.iir_coef[1].coef_b={138529480,   -267551490,    129040578},
		.iir_coef[1].coef_a={134217728,   -267551490,    133352330},

		.iir_coef[2].coef_b={134516353,   -268162980,    133647489},
		.iir_coef[2].coef_a={134217728,   -268162980,    133946114},

		.iir_coef[3].coef_b={133595549,   -264581113,    131087955},
		.iir_coef[3].coef_a={134217728,   -264581113,    130465777},

		.iir_coef[4].coef_b={0x8000000,0,0},
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

/*		.fir_bypass_flag=1,
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
        .total_gain = 1228,

		.iir_bypass_flag=0,
		.iir_counter=5,

		.iir_coef[0].coef_b={19855313,    -39617845,     19762640},
		.iir_coef[0].coef_a={16777216,    -33333946,     16557454},

		.iir_coef[1].coef_b={9751459,    -17329625,      7727703},
		.iir_coef[1].coef_a={16777216,    -17329625,       701946},

		.iir_coef[2].coef_b={18001809,    -32843215,     14866746},
		.iir_coef[2].coef_a={16777216,    -32843215,     16091339},

		.iir_coef[3].coef_b={12659487,    -24147313,     11526097},
		.iir_coef[3].coef_a={16777216,    -32207342,     15468397},

		.iir_coef[4].coef_b={16490453,    -32048020,     15620931},
		.iir_coef[4].coef_a={16777216,    -32048020,     15334169},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},


		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
};

static const struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_44p1k_mode0 = {
    .anc_cfg_ff_l = {
		.total_gain =312,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_L,

		.iir_coef[0].coef_b={42465729,    -84858529,     42392831},
		.iir_coef[0].coef_a={134217728,   -268346271,    134128558},

		.iir_coef[1].coef_b={136159949,   -267039705,    130899919},
		.iir_coef[1].coef_a={134217728,   -267039705,    132842140},

		.iir_coef[2].coef_b={132746107,   -263254540,    130599907},
		.iir_coef[2].coef_a={134217728,   -263254540,    129128286},

		.iir_coef[3].coef_b={131402980,   -255575175,    125032243},
		.iir_coef[3].coef_a={ 134217728,   -255575175,    122217496},

		.iir_coef[4].coef_b={0x8000000,0,0},
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

/*		.fir_bypass_flag=1,
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
        .total_gain = 511,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FB_L,

		.iir_coef[0].coef_b={26719020,    -52852829,     26204379},
		.iir_coef[0].coef_a={134217728,   -210410903,     76474119},

		.iir_coef[1].coef_b={138909433,   -267471808,    128584365},
		.iir_coef[1].coef_a={134217728,   -267471808,    133276071},

		.iir_coef[2].coef_b={134542733,   -268138827,    133597115},
		.iir_coef[2].coef_a={134217728,   -268138827,    133922120},

		.iir_coef[3].coef_b={133541379,   -264235686,    130815458},
		.iir_coef[3].coef_a={134217728,   -264235686,    130139109},

		.iir_coef[4].coef_b={0x8000000,0,0},
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

/*		.fir_bypass_flag=1,
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
        .total_gain = 1228,

		.iir_bypass_flag=0,
		.iir_counter=5,

		.iir_coef[0].coef_b={19847881,    -39594823,     19747071},
		.iir_coef[0].coef_a={16777216,    -33314517,     16538159},

		.iir_coef[1].coef_b={9442890,    -16603187,      7330251},
		.iir_coef[1].coef_a={16777216,    -16603187,        -4075},

		.iir_coef[2].coef_b={18107639,    -32779315,     14701642},
		.iir_coef[2].coef_a={16777216,    -32779315,     16032065},

		.iir_coef[3].coef_b={12666347,    -24058210,     11437046},
		.iir_coef[3].coef_a={16777216,    -32089673,     15357640},

		.iir_coef[4].coef_b={16466312,    -31915122,     15523589},
		.iir_coef[4].coef_a={16777216,    -31915122,     15212684},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},


		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
};
#endif //#if !defined(BESUI_TWS_EN) && !defined(BESUI_STEREO_EN)

const struct_anc_cfg * anc_coef_list_50p7k[ANC_COEF_LIST_NUM] = {
    &AncFirCoef_50p7k_mode0,
    #ifdef VOICE_ASSIST_ADA_IIR
    &AncFirCoef_50p7k_mode1,
    #endif
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

#if defined(PSAP_APP)
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
#endif
