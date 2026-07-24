/***************************************************************************
 *
 * Copyright 2015-2022 BES.
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
** namer��PSAP filter
** description��iir, drc and limiter
** version��V1.0
** author�� xuml
** modify��2022.08.22.
*******************************************************************************/

#include "mm_dbg.h"
#include <stdio.h>
#include <string.h>

#include "plat_addr_map.h"
#include "cmsis.h"
#include "hal_dma.h"
#include "hal_codec.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_cmu.h"
#include "hal_codec.h"
#include "analog.h"
#include "psap_process.h"

#if defined(AUDIO_PSAP_DEHOWLING_HW)
#include "anc_process.h"
#endif

/**********************************mocro**************************************/

typedef short int16;
typedef unsigned short uint16;
typedef int   int32;
typedef unsigned int   uint32;

/*
1	400	psap_enable	RW	0	0
1	400	psap_enable_ch0	RW	0	1
1	400	psap_enable_ch1	RW	0	2
3	400	psap_adc0_sel_ch0	RW	0	3
3	400	psap_adc1_sel_ch0	RW	0	6
3	400	psap_adc0_sel_ch1	RW	0	9
3	400	psap_adc1_sel_ch1	RW	0	12
1	400	psap_rate_sel	RW	0	15
1	400	psap_adc_rate_sel	RW	0	16
1	400	adc_psap_mode	RW	0	17
2	400	down_sel_psap	RW	2	18
1	400	dac_psap_mode	RW	0	20
3	400	up_sel_psap	RW	4	21
2	400	psap_mix_mode	RW	0	24
1	400	psap_dehowl_enable	RW	0	26
1	400	psap_dehowl0_fir_en	RW	0	27
28	400		x
*/

struct _psap_config
{
	uint32 psap_enable : 1;
	uint32 psap_enable_ch0 : 1;
	uint32 psap_enable_ch1 : 1;

	uint32 psap_adc0_sel_ch0 : 3;
	uint32 psap_adc1_sel_ch0 : 3;
	uint32 psap_adc0_sel_ch1 : 3;
	uint32 psap_adc1_sel_ch1 : 3;

	uint32 psap_rate_sel : 1;
	uint32 psap_adc_rate_sel : 1;
	uint32 adc_psap_mode : 1;

	uint32 down_sel_psap : 2;
	uint32 dac_psap_mode : 1;
	uint32 up_sel_psap : 3;
	uint32 psap_mix_mode : 2;

	uint32 psap_dehowl_enable : 1;
	uint32 psap_dehowl0_fir_en : 1;

	uint32 reserved : 4;
};

/*
0x3c4
17	404	psap_gain_ext_update	RW	0	0
17	404		x
*/

struct _psap_gain_update
{
	uint32 psap_gain_ext_update_0 : 1;
	uint32 psap_gain_ext_update_1 : 1;
	uint32 psap_gain_ext_update_2 : 1;
	uint32 psap_gain_ext_update_3 : 1;
	uint32 psap_gain_ext_update_4 : 1;
	uint32 psap_gain_ext_update_5 : 1;
	uint32 psap_gain_ext_update_6 : 1;
	uint32 psap_gain_ext_update_7 : 1;
	uint32 psap_gain_ext_update_8 : 1;
	uint32 psap_gain_ext_update_9 : 1;
	uint32 psap_gain_ext_update_10 : 1;
	uint32 psap_gain_ext_update_11 : 1;
	uint32 psap_gain_ext_update_12 : 1;
	uint32 psap_gain_ext_update_13 : 1;
    	uint32 psap_gain_ext_update_14 : 1;
    	uint32 psap_gain_ext_update_15 : 1;
    	uint32 psap_gain_ext_update_16 : 1;
	uint32 reserved : 15;
};

/*
17	408	psap_gain_ext_sel	RW	0	0
4	408	psap_sbd_cnt	RW	0	17
21	408		x
*/

struct _psap_gain_passzero_sel
{
	uint32 psap_gain_ext_sel_0 : 1;
	uint32 psap_gain_ext_sel_1 : 1;
	uint32 psap_gain_ext_sel_2 : 1;
	uint32 psap_gain_ext_sel_3 : 1;
	uint32 psap_gain_ext_sel_4 : 1;
	uint32 psap_gain_ext_sel_5 : 1;
	uint32 psap_gain_ext_sel_6 : 1;
	uint32 psap_gain_ext_sel_7 : 1;
	uint32 psap_gain_ext_sel_8 : 1;
	uint32 psap_gain_ext_sel_9 : 1;
	uint32 psap_gain_ext_sel_10 : 1;
	uint32 psap_gain_ext_sel_11 : 1;
	uint32 psap_gain_ext_sel_12 : 1;
	uint32 psap_gain_ext_sel_13 : 1;
    	uint32 psap_gain_ext_sel_14 : 1;
    	uint32 psap_gain_ext_sel_15 : 1;
    	uint32 psap_gain_ext_sel_16 : 1;
    	uint32 psap_sbd_cnt : 4;
	uint32 reserved : 11;
};

/*
0x410
λ	Name	Type	����
15:0	psap_cpd_ct	r/w	compressor��ֵ
31:16	psap_cpd_cs		compressorб��
*/
struct _psap_cpd_c_cfg
{
	uint32 psap_cpd_ct :16;
	uint32 psap_cpd_cs : 16;
};

/*
0x414
λ	Name	Type	����
15:0	psap_cpd_wt	r/w	wt
31:16	psap_cpd_ws
*/
struct _psap_cpd_w_cfg
{
	uint32 psap_cpd_wt :16;
	uint32 psap_cpd_ws : 16;
};

/*
0x418
λ	Name	Type	����
15:0	psap_cpd_et	r/w	et
31:16	psap_cpd_es
*/
struct _psap_cpd_e_cfg
{
	uint32 psap_cpd_et :16;
	uint32 psap_cpd_es : 16;
};

/*
0x41c
λ	Name	Type	����
15:0	psap_cpd_coefa_at	r/w	Attack_time����gs���
31:16	psap_cpd_coefb_at		Attack_time����gc���
*/
struct _psap_cpd_coef_attack_cfg
{
	uint32 psap_cpd_coefa_at :16;
	uint32 psap_cpd_coefb_at : 16;
};

/*
0x420
λ	Name	Type	����
15:0	psap_lmt_coefa_at	r/w	Attack_time����gs���
31:16	psap_lmt_coefb_at		Attack_time����gc���
*/
struct _psap_lmt_coef_attack_cfg
{
	uint32 psap_lmt_coefa_at :16;
	uint32 psap_lmt_coefb_at : 16;
};
/*
0x424
λ	Name	Type	����
18:0	psap_lmt_th	r/w
19	psap_lmt_th_update
*/
struct _psap_lmt_th_cfg
{
	uint32 psap_lmt_th :19;
	uint32 psap_lmt_th_update : 1;
	uint32 reserved : 12;
};

/*
0x428
λ	Name	Type	����
15:0	psap_cpd_tava	r/w	��rms���
31:16	psap_cpd_tavb		�������źž���ֵ���
*/
struct _psap_cpd_tav_cfg
{
	uint32 psap_cpd_tava :16;
	uint32 psap_cpd_tavb :16;
};


/*
0x42c
λ	Name	Type	����
15:0	psap_cpd_coefa_rt	r/w	Release_time����gs���
31:16	psap_cpd_coefb_rt		Release_time����gc���
*/
struct _psap_cpd_coef_release_cfg
{
	uint32 psap_cpd_coefa_rt :16;
	uint32 psap_cpd_coefb_rt : 16;
};

/*
0x430
λ	Name	Type	����
15:0	psap_lmt_coefa_rt	r/w	Release_time����gs���
31:16	psap_lmt_coefb_rt		Release_time����gc���
*/
struct _psap_lmt_coef_release_cfg
{
	uint32 psap_lmt_coefa_rt :16;
	uint32 psap_lmt_coefb_rt : 16;
};

/*
1	474	psap_iir_enable	RW	0	0
1	474	psap_iir_ext_bypass  	RW	0	1
1	474	psap_iir_ext_bypass_1	RW	0	2
1	474	psap_iir_ext_bypass_2	RW	0	3
1	474	psap_iir_ext_bypass_3	RW	0	4
1	474	psap_iir_ext_bypass_4	RW	0	5
1	474	psap_iir_gaincal_ext_bypass	RW	0	6
1	474	psap_iir_gainuse_ext_bypass	RW	0	7
2	474	psap_iir_output_bit	RW	0	8
1	474	psap_cpd_enable	RW	0	10
7	474	psap_cpd_delay	RW	0	11
1	474	psap_lmt_enable	RW	0	18
7	474	psap_lmt_delay	RW	0	19
26	474		x
*/
struct _psap_control
{
	uint32 psap_iir_enable : 1;
	uint32 psap_iir_ext_bypass : 1;
	uint32 psap_iir_ext_bypass_1 : 1;
	uint32 psap_iir_ext_bypass_2 : 1;
	uint32 psap_iir_ext_bypass_3 : 1;
	uint32 psap_iir_ext_bypass_4 : 1;
	uint32 psap_iir_gaincal_ext_bypass : 1;
	uint32 psap_iir_gainuse_ext_bypass : 1;
    	uint32 psap_iir_output_bit : 2;
	uint32 psap_cpd_enable :1;
	uint32 psap_cpd_delay : 7;
	uint32 psap_lmt_enable :1;
	uint32 psap_lmt_delay : 7;
	uint32 reserved : 6;
};


/*
iir coefficients(b/a Q27)
a1
a2
a3

b1
b2
b3
b0
*/
struct _psap_chip_iir_coefs
{
    int32 a1 ;
    int32 a2 ;
    int32 a3 ;

    int32 b1 ;
    int32 b2 ;
    int32 b3 ;
    int32 b0 ;
};

struct _psap_iir_coef
{
    struct _psap_chip_iir_coefs iir0;
    struct _psap_chip_iir_coefs iir1;
};


/*
iir coefficients(b/a Q27)
a1
a2

b1
b2
b3
*/
struct _gain_iir_coefs
{
    int32 a1 ;
    int32 a2 ;

    int32 b1 ;
    int32 b2 ;
    int32 b0 ;
};

struct _psap_cpd_all_config
{
     struct _psap_cpd_c_cfg psap_cpd_c_cfg;
     struct _psap_cpd_w_cfg psap_cpd_w_cfg;
     struct _psap_cpd_e_cfg psap_cpd_e_cfg;
     struct _psap_cpd_tav_cfg psap_cpd_tav_cfg;
     struct _psap_cpd_coef_attack_cfg psap_cpd_coef_attack_cfg;
     struct _psap_cpd_coef_release_cfg psap_cpd_coef_release_cfg;
};

#define PSAP_BASE                            ((uint32)CODEC_BASE)

#define PSAP_BAND_NUM 17
#define PSAP_IIR_NUM (PSAP_BAND_NUM-1)

#if ANC_PROD_TEST
#define PSAP_DEBUG_TRACE
#else
//#define PSAP_DEBUG_TRACE
#endif

volatile static struct _psap_config *psap_config=(volatile struct _psap_config *)(PSAP_BASE+0x400);
volatile static struct _psap_gain_update *psap_gain_update=(volatile struct _psap_gain_update *)(PSAP_BASE+0x404);
volatile static struct _psap_gain_passzero_sel *psap_gain_passzero_sel=(volatile struct _psap_gain_passzero_sel *)(PSAP_BASE+0x408);

volatile static struct _psap_cpd_c_cfg *psap_cpd_c_cfg=(volatile struct _psap_cpd_c_cfg *)(PSAP_BASE+0x450);
volatile static struct _psap_cpd_w_cfg *psap_cpd_w_cfg=(volatile struct _psap_cpd_w_cfg *)(PSAP_BASE+0x454);
volatile static struct _psap_cpd_e_cfg *psap_cpd_e_cfg=(volatile struct _psap_cpd_e_cfg *)(PSAP_BASE+0x458);
volatile static struct _psap_cpd_tav_cfg *psap_cpd_tav_cfg=(volatile struct _psap_cpd_tav_cfg *)(PSAP_BASE+0x468);
volatile static struct _psap_cpd_coef_attack_cfg *psap_cpd_coef_attack_cfg=(volatile struct _psap_cpd_coef_attack_cfg *)(PSAP_BASE+0x45c);
volatile static struct _psap_cpd_coef_release_cfg *psap_cpd_coef_release_cfg=(volatile struct _psap_cpd_coef_release_cfg *)(PSAP_BASE+0x46c);

volatile static struct _psap_cpd_all_config *psap_cpd_all_config=(volatile struct _psap_cpd_all_config *)(PSAP_BASE+0x4a0);

volatile static struct _psap_lmt_coef_attack_cfg *psap_lmt_coef_attack_cfg=(volatile struct _psap_lmt_coef_attack_cfg *)(PSAP_BASE+0x460);
volatile static struct _psap_lmt_th_cfg *psap_lmt_th_cfg=(volatile struct _psap_lmt_th_cfg *)(PSAP_BASE+0x464);
volatile static struct _psap_lmt_coef_release_cfg *psap_lmt_coef_release_cfg=(volatile struct _psap_lmt_coef_release_cfg *)(PSAP_BASE+0x470);

volatile static struct _psap_control *psap_control=(volatile struct _psap_control *)(PSAP_BASE+0x474);

volatile static struct _psap_iir_coef *psap_iir_coef=(volatile struct _psap_iir_coef *)(PSAP_BASE+0x6000);

volatile static int32 *psap_gain_ext=(volatile int32 *)(PSAP_BASE+0x40c);

volatile static struct _gain_iir_coefs *gain_iir_coefs=(volatile struct _gain_iir_coefs *)(PSAP_BASE+0x6000+0x380);

volatile static int psap_open_flag;
volatile static int psap_band_num;
volatile static float psap_ramp_total_gain_l = 1.0f;
volatile static float psap_bands_same_gain_l[PSAP_BAND_NUM];
volatile static float psap_bands_gain_l[PSAP_BAND_NUM];
volatile static int psap_coef_gain_l[PSAP_BAND_NUM];

const aud_item_psap  *psap_cfg_coef_l;

// 4 * 70ms
static const anc_iir_coefs   iir_coef_gain_ramp_psap_normal=
{
    .coef_b={512,         1025,          512},
    .coef_a={134217728,   -265823675,    131607997},
};
static void psap_updata_gain(void);
static void psap_updata_coef(unsigned int dest,unsigned int src);

int psap_set_cfg_coef(const struct_psap_cfg * cfg)
{
    int i,j;
    int BandNum;
    int DestIndex;
    PSAP_ERROR err=PSAP_NO_ERR;

    psap_cfg_coef_l=&(cfg->psap_cfg_l);

    LOG_I("%s", __func__);

    LOG_I("psap_total_gain:%d",psap_cfg_coef_l->psap_total_gain);
    LOG_I("psap_band_num:%d",psap_cfg_coef_l->psap_band_num);


    if(psap_open_flag==0)
    {
        LOG_I("%s: PSAP not opened", __func__);
        return PSAP_OTHER_ERR;
    }

    if(psap_cfg_coef_l==NULL)
    {
        LOG_I("%s: cfg is null", __func__);
        return PSAP_OTHER_ERR;
    }

    BandNum=psap_cfg_coef_l->psap_band_num;

    if(BandNum!=17&&BandNum!=9&&BandNum!=5&&BandNum!=3&&BandNum!=2)
    {
        LOG_I("%s: Error band number!:%d", __func__,BandNum);
        return PSAP_NUM_ERR;
    }

#ifdef PSAP_DEBUG_TRACE
    for( j = 0; j <BandNum-1; j++)
    {
        LOG_I("PSAP_IIR_NUM_%d:",j);

        LOG_I("iir0.coef_b: 0x%08x, 0x%08x, 0x%08x, 0x%08x",\
        psap_cfg_coef_l->psap_iir_coef[j].iir0.coef_b[0], \
        psap_cfg_coef_l->psap_iir_coef[j].iir0.coef_b[1], \
        psap_cfg_coef_l->psap_iir_coef[j].iir0.coef_b[2], \
        psap_cfg_coef_l->psap_iir_coef[j].iir0.coef_b[3]);

        LOG_I("iir0.coef_a: 0x%08x, 0x%08x, 0x%08x, 0x%08x",\
        psap_cfg_coef_l->psap_iir_coef[j].iir0.coef_a[0], \
        psap_cfg_coef_l->psap_iir_coef[j].iir0.coef_a[1], \
        psap_cfg_coef_l->psap_iir_coef[j].iir0.coef_a[2], \
        psap_cfg_coef_l->psap_iir_coef[j].iir0.coef_a[3]);

        LOG_I("iir1.coef_b: 0x%08x, 0x%08x, 0x%08x, 0x%08x", \
        psap_cfg_coef_l->psap_iir_coef[j].iir1.coef_b[0], \
        psap_cfg_coef_l->psap_iir_coef[j].iir1.coef_b[1], \
        psap_cfg_coef_l->psap_iir_coef[j].iir1.coef_b[2], \
        psap_cfg_coef_l->psap_iir_coef[j].iir1.coef_b[3]);

        LOG_I("iir1.coef_a: 0x%08x, 0x%08x, 0x%08x, 0x%08x", \
        psap_cfg_coef_l->psap_iir_coef[j].iir1.coef_a[0], \
        psap_cfg_coef_l->psap_iir_coef[j].iir1.coef_a[1], \
        psap_cfg_coef_l->psap_iir_coef[j].iir1.coef_a[2], \
        psap_cfg_coef_l->psap_iir_coef[j].iir1.coef_a[3]);
    }

    for( j = 0; j <BandNum; j++)
    {
        LOG_I("PSAP_BAND_NUM_%02d_GAIN: 0x%08x", j,psap_cfg_coef_l->psap_band_gain[j]);
    }

    for( j = 0; j <BandNum-1; j++)
    {
        LOG_I("band:%04d",j);

        LOG_I("psap_cpd_ct:0x%04x",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_ct);
        LOG_I("psap_cpd_cs:0x%04x",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_cs);

        LOG_I("psap_cpd_wt:0x%04x",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_wt);
        LOG_I("psap_cpd_ws:0x%04x",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_ws);

        LOG_I("psap_cpd_et:0x%04x",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_et);
        LOG_I("psap_cpd_es:0x%04x",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_es);


        LOG_I("psap_cpd_coefa_at:0x%04x",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_coefa_at);
        LOG_I("psap_cpd_coefb_at:0x%04x",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_coefb_at);

        LOG_I("psap_cpd_tava:0x%04x",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_tava);
        LOG_I("psap_cpd_tavb:0x%04x",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_tavb);

        LOG_I("psap_cpd_coefa_rt:0x%04x",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_coefa_rt);
        LOG_I("psap_cpd_coefb_rt:0x%04x",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_coefb_rt);

        LOG_I("psap_cpd_delay:%d",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_delay);
    }

    LOG_I("psap_adc_gain_offset:%d",psap_cfg_coef_l->psap_adc_gain_offset);

    LOG_I("psap_limiter_coefa_at:%d",psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_coefa_at);
    LOG_I("psap_limiter_coefb_at:%d",psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_coefb_at);
    LOG_I("psap_limiter_th:%d",psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_th);
    LOG_I("psap_limiter_coefa_rt:%d",psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_coefa_rt);
    LOG_I("psap_limiter_coefb_rt:%d",psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_coefb_rt);
    LOG_I("psap_limiter_delay:%d",psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_delay);
#endif


    //init the gain.
    psap_band_num=PSAP_BAND_NUM;

    for(i=0;i<PSAP_BAND_NUM;i++)
    {
        psap_coef_gain_l[i]=0;
    }
    psap_updata_gain();

    psap_band_num=BandNum;

    psap_config->psap_enable=0;
    psap_config->psap_enable_ch0=0;
    //psap_config->psap_adc0_sel_ch0=0;
    //psap_config->psap_adc1_sel_ch0=1;
    psap_control->psap_cpd_enable=0;
    psap_control->psap_iir_enable=0;
    psap_control->psap_lmt_enable=0;


    if(BandNum<=4)
    {
        hal_codec_psap_enable(24000000);
    }
    else
    {
        hal_codec_psap_enable(48000000);
    }

#if defined(PSAP_APP)
    analog_aud_apply_anc_adc_gain_offset(ANC_TALKTHRU, psap_cfg_coef_l->psap_adc_gain_offset, 0);
#endif

    for(i=0;i<PSAP_BAND_NUM;i++)
    {
        psap_coef_gain_l[i]=(int)(psap_cfg_coef_l->psap_band_gain[i]*(psap_cfg_coef_l->psap_total_gain/512.0f));
    }

    psap_updata_gain();

    //cfg stage 0 filter coef.
    DestIndex=0;

    psap_control->psap_iir_ext_bypass_1=0;
    psap_control->psap_iir_ext_bypass_2=0;
    psap_control->psap_iir_ext_bypass_3=0;
    psap_control->psap_iir_ext_bypass_4=0;

    if(BandNum==17||BandNum==9||BandNum==5||BandNum==3||BandNum==2)
    {
        int SrcIndex0=(BandNum-1)-1;
        psap_updata_coef(DestIndex,SrcIndex0);
        DestIndex++;
    }

    //cfg stage 1 filter coef.
    if(BandNum==17||BandNum==9||BandNum==5||BandNum==3)
    {
        int SrcIndex0=(BandNum-1)/2-1;
        psap_updata_coef(DestIndex,SrcIndex0);
        DestIndex++;
        psap_control->psap_iir_ext_bypass_1=0;
    }
    else
    {
        psap_control->psap_iir_ext_bypass_1=1;
    }

    //cfg stage 2 filter coef.
    if(BandNum==17||BandNum==9||BandNum==5)
    {
        int SrcIndex0=(BandNum-1)/2-(BandNum-1)/4-1;
        int SrcIndex1=(BandNum-1)/2+(BandNum-1)/4-1;

        psap_updata_coef(DestIndex,SrcIndex0);
        DestIndex++;
        psap_updata_coef(DestIndex,SrcIndex1);
        DestIndex++;
        psap_control->psap_iir_ext_bypass_2=0;
    }
    else
    {
        psap_control->psap_iir_ext_bypass_2=1;
    }

    //cfg stage 3 filter coef.
    if(BandNum==17||BandNum==9)
    {
        int SrcIndex0=(BandNum-1)/2-(BandNum-1)/4-(BandNum-1)/8-1;
        int SrcIndex1=(BandNum-1)/2-(BandNum-1)/4+(BandNum-1)/8-1;
        int SrcIndex2=(BandNum-1)/2+(BandNum-1)/4-(BandNum-1)/8-1;
        int SrcIndex3=(BandNum-1)/2+(BandNum-1)/4+(BandNum-1)/8-1;

        psap_updata_coef(DestIndex,SrcIndex0);
        DestIndex++;
        psap_updata_coef(DestIndex,SrcIndex1);
        DestIndex++;
        psap_updata_coef(DestIndex,SrcIndex2);
        DestIndex++;
        psap_updata_coef(DestIndex,SrcIndex3);
        DestIndex++;

        psap_control->psap_iir_ext_bypass_3=0;
    }
    else
    {
        psap_control->psap_iir_ext_bypass_3=1;
    }

    //cfg stage 4 filter coef.
    if(BandNum==17)
    {
        int SrcIndex0=(BandNum-1)/2-(BandNum-1)/4-(BandNum-1)/8-(BandNum-1)/16-1;
        int SrcIndex1=(BandNum-1)/2-(BandNum-1)/4-(BandNum-1)/8+(BandNum-1)/16-1;
        int SrcIndex2=(BandNum-1)/2-(BandNum-1)/4+(BandNum-1)/8-(BandNum-1)/16-1;
        int SrcIndex3=(BandNum-1)/2-(BandNum-1)/4+(BandNum-1)/8+(BandNum-1)/16-1;

        int SrcIndex4=(BandNum-1)/2+(BandNum-1)/4-(BandNum-1)/8-(BandNum-1)/16-1;
        int SrcIndex5=(BandNum-1)/2+(BandNum-1)/4-(BandNum-1)/8+(BandNum-1)/16-1;
        int SrcIndex6=(BandNum-1)/2+(BandNum-1)/4+(BandNum-1)/8-(BandNum-1)/16-1;
        int SrcIndex7=(BandNum-1)/2+(BandNum-1)/4+(BandNum-1)/8+(BandNum-1)/16-1;

        psap_updata_coef(DestIndex,SrcIndex0);
        DestIndex++;
        psap_updata_coef(DestIndex,SrcIndex1);
        DestIndex++;
        psap_updata_coef(DestIndex,SrcIndex2);
        DestIndex++;
        psap_updata_coef(DestIndex,SrcIndex3);
        DestIndex++;
        psap_updata_coef(DestIndex,SrcIndex4);
        DestIndex++;
        psap_updata_coef(DestIndex,SrcIndex5);
        DestIndex++;
        psap_updata_coef(DestIndex,SrcIndex6);
        DestIndex++;
        psap_updata_coef(DestIndex,SrcIndex7);
        DestIndex++;

        psap_control->psap_iir_ext_bypass_4=0;
    }
    else
    {
        psap_control->psap_iir_ext_bypass_4=1;
    }
    //cfg cpd parameters.
    for(i=0,j=0;i<PSAP_BAND_NUM-1;i=i+16/(psap_band_num-1),j++)
    {
        if(i==0)
        {
            psap_cpd_c_cfg->psap_cpd_ct=psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_ct;
            psap_cpd_c_cfg->psap_cpd_cs=psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_cs;

            psap_cpd_w_cfg->psap_cpd_wt=psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_wt;
            psap_cpd_w_cfg->psap_cpd_ws=psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_ws;

            psap_cpd_e_cfg->psap_cpd_et=psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_et;
            psap_cpd_e_cfg->psap_cpd_es=psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_es;

            psap_cpd_coef_attack_cfg->psap_cpd_coefa_at=psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_coefa_at;
            psap_cpd_coef_attack_cfg->psap_cpd_coefb_at=psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_coefb_at;

            psap_cpd_tav_cfg->psap_cpd_tava=psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_tava;
            psap_cpd_tav_cfg->psap_cpd_tavb=psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_tavb;

            psap_cpd_coef_release_cfg->psap_cpd_coefa_rt=psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_coefa_rt;
            psap_cpd_coef_release_cfg->psap_cpd_coefb_rt=psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_coefb_rt;
        }
        else
        {
            psap_cpd_all_config[i-1].psap_cpd_c_cfg.psap_cpd_ct=psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_ct;
            psap_cpd_all_config[i-1].psap_cpd_c_cfg.psap_cpd_cs=psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_cs;

            psap_cpd_all_config[i-1].psap_cpd_w_cfg.psap_cpd_wt=psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_wt;
            psap_cpd_all_config[i-1].psap_cpd_w_cfg.psap_cpd_ws=psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_ws;

            psap_cpd_all_config[i-1].psap_cpd_e_cfg.psap_cpd_et=psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_et;
            psap_cpd_all_config[i-1].psap_cpd_e_cfg.psap_cpd_es=psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_es;

            psap_cpd_all_config[i-1].psap_cpd_coef_attack_cfg.psap_cpd_coefa_at=psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_coefa_at;
            psap_cpd_all_config[i-1].psap_cpd_coef_attack_cfg.psap_cpd_coefb_at=psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_coefb_at;

            psap_cpd_all_config[i-1].psap_cpd_tav_cfg.psap_cpd_tava=psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_tava;
            psap_cpd_all_config[i-1].psap_cpd_tav_cfg.psap_cpd_tavb=psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_tavb;

            psap_cpd_all_config[i-1].psap_cpd_coef_release_cfg.psap_cpd_coefa_rt=psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_coefa_rt;
            psap_cpd_all_config[i-1].psap_cpd_coef_release_cfg.psap_cpd_coefb_rt=psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_coefb_rt;
        }
    }


    if(psap_cfg_coef_l->psap_cpd_cfg[0].psap_cpd_delay>=0&&psap_cfg_coef_l->psap_cpd_cfg[0].psap_cpd_delay<128)
    {
        psap_control->psap_cpd_delay=psap_cfg_coef_l->psap_cpd_cfg[0].psap_cpd_delay;
    }
    else
    {
        psap_control->psap_cpd_delay=96;
    }

    //cfg limiter parameters
    psap_lmt_coef_attack_cfg->psap_lmt_coefa_at=psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_coefa_at;
    psap_lmt_coef_attack_cfg->psap_lmt_coefb_at=psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_coefb_at;

    psap_lmt_th_cfg->psap_lmt_th_update=0;
    psap_lmt_th_cfg->psap_lmt_th=psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_th;
    psap_lmt_th_cfg->psap_lmt_th_update=1;

    psap_lmt_coef_release_cfg->psap_lmt_coefa_rt=psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_coefa_rt;
    psap_lmt_coef_release_cfg->psap_lmt_coefb_rt=psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_coefb_rt;

    if(psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_delay>=0&&psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_delay<128)
    {
         psap_control->psap_lmt_delay=psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_delay;
    }
    else
    {
         psap_control->psap_lmt_delay=16;
    }

#if defined(AUDIO_PSAP_DEHOWLING_HW)
    anc_set_dehowling_cfg(&(psap_cfg_coef_l->psap_dehowling_cfg));
#endif

    if(psap_cfg_coef_l->psap_type==PSAP_MODE_ONLY_MIC
        ||psap_cfg_coef_l->psap_type==PSAP_MODE_MIC_MUSIC
        ||psap_cfg_coef_l->psap_type==PSAP_MODE_ONLY_MUSIC)
    {
        psap_set_mode(AUD_CHANNEL_MAP_CH0, psap_cfg_coef_l->psap_type);
    }
    else
    {
        LOG_I("%s Error psap tyep:%d", __func__,psap_cfg_coef_l->psap_type);

        psap_set_mode(AUD_CHANNEL_MAP_CH0, PSAP_MODE_ONLY_MIC);
    }

    psap_control->psap_cpd_enable=1;
    psap_control->psap_iir_enable=1;
    psap_control->psap_lmt_enable=1;
    psap_config->psap_enable_ch0=1;
    psap_config->psap_enable=1;
    return err;
}

static void psap_updata_coef(unsigned int dest,unsigned int src)
{
#ifdef PSAP_DEBUG_TRACE
    LOG_I("%s dst_coef_num:%02d,src_coef_num:%02d", __func__,dest,src);
#endif

    if(dest>=PSAP_IIR_NUM||src>=PSAP_IIR_NUM)
    {
            LOG_I("%s:Error band mum!dest:%d,source:%d", __func__,dest,src);
            return;
    }


    psap_iir_coef[dest].iir0.a1=-psap_cfg_coef_l->psap_iir_coef[src].iir0.coef_a[1];
    psap_iir_coef[dest].iir0.a2=-psap_cfg_coef_l->psap_iir_coef[src].iir0.coef_a[2];
    psap_iir_coef[dest].iir0.a3=-psap_cfg_coef_l->psap_iir_coef[src].iir0.coef_a[3];
    psap_iir_coef[dest].iir0.b1=psap_cfg_coef_l->psap_iir_coef[src].iir0.coef_b[1];
    psap_iir_coef[dest].iir0.b2=psap_cfg_coef_l->psap_iir_coef[src].iir0.coef_b[2];
    psap_iir_coef[dest].iir0.b3=psap_cfg_coef_l->psap_iir_coef[src].iir0.coef_b[3];
    psap_iir_coef[dest].iir0.b0=psap_cfg_coef_l->psap_iir_coef[src].iir0.coef_b[0];

    psap_iir_coef[dest].iir1.a1=-psap_cfg_coef_l->psap_iir_coef[src].iir1.coef_a[1];
    psap_iir_coef[dest].iir1.a2=-psap_cfg_coef_l->psap_iir_coef[src].iir1.coef_a[2];
    psap_iir_coef[dest].iir1.a3=-psap_cfg_coef_l->psap_iir_coef[src].iir1.coef_a[3];
    psap_iir_coef[dest].iir1.b1=psap_cfg_coef_l->psap_iir_coef[src].iir1.coef_b[1];
    psap_iir_coef[dest].iir1.b2=psap_cfg_coef_l->psap_iir_coef[src].iir1.coef_b[2];
    psap_iir_coef[dest].iir1.b3=psap_cfg_coef_l->psap_iir_coef[src].iir1.coef_b[3];
    psap_iir_coef[dest].iir1.b0=psap_cfg_coef_l->psap_iir_coef[src].iir1.coef_b[0];



    return;
}

static void psap_updata_gain(void)
{
    int i,j;
#ifdef PSAP_DEBUG_TRACE
    LOG_I("%s", __func__);
#endif
    if(psap_band_num!=17&&psap_band_num!=9&&psap_band_num!=5&&psap_band_num!=3&&psap_band_num!=2)
    {
        LOG_I("%s: Error band number!:%d", __func__,psap_band_num);
        return;
    }

    //setting the gain.
    psap_gain_update->psap_gain_ext_update_0=0;
    psap_gain_update->psap_gain_ext_update_1=0;
    psap_gain_update->psap_gain_ext_update_2=0;
    psap_gain_update->psap_gain_ext_update_3=0;
    psap_gain_update->psap_gain_ext_update_4=0;
    psap_gain_update->psap_gain_ext_update_5=0;
    psap_gain_update->psap_gain_ext_update_6=0;
    psap_gain_update->psap_gain_ext_update_7=0;
    psap_gain_update->psap_gain_ext_update_8=0;
    psap_gain_update->psap_gain_ext_update_9=0;
    psap_gain_update->psap_gain_ext_update_10=0;
    psap_gain_update->psap_gain_ext_update_11=0;
    psap_gain_update->psap_gain_ext_update_12=0;
    psap_gain_update->psap_gain_ext_update_13=0;
    psap_gain_update->psap_gain_ext_update_14=0;
    psap_gain_update->psap_gain_ext_update_15=0;
    psap_gain_update->psap_gain_ext_update_16=0;

    for(i=0,j=0;i<PSAP_BAND_NUM-1;i=i+16/(psap_band_num-1),j++)
    {
        psap_gain_ext[i]=(int32_t)(psap_coef_gain_l[j] * psap_bands_same_gain_l[j] * psap_bands_gain_l[j] * psap_ramp_total_gain_l);
#ifdef PSAP_DEBUG_TRACE
        LOG_I("band_num:%02d, chip_num:%02d, coef_gain_l:0x%08x, bands_gain_l:%d, total_gain_l:%d", j, i,
            psap_coef_gain_l[j],
            (int32_t)(psap_bands_same_gain_l[j] * 1000),
            (int32_t)(psap_ramp_total_gain_l * 1000));
#endif
    }
    psap_gain_ext[i]=(int32_t)(psap_coef_gain_l[j] * psap_bands_same_gain_l[j] * psap_bands_gain_l[j] * psap_ramp_total_gain_l / 4);
#ifdef PSAP_DEBUG_TRACE
    LOG_I("band_num:%02d, chip_num:%02d, coef_gain_l:0x%08x, bands_gain_l:%d, total_gain_l:%d", j, i,
        psap_coef_gain_l[j],
        (int32_t)(psap_bands_same_gain_l[j] * 1000),
        (int32_t)(psap_ramp_total_gain_l * 1000));
#endif

    psap_gain_update->psap_gain_ext_update_0=1;
    psap_gain_update->psap_gain_ext_update_1=1;
    psap_gain_update->psap_gain_ext_update_2=1;
    psap_gain_update->psap_gain_ext_update_3=1;
    psap_gain_update->psap_gain_ext_update_4=1;
    psap_gain_update->psap_gain_ext_update_5=1;
    psap_gain_update->psap_gain_ext_update_6=1;
    psap_gain_update->psap_gain_ext_update_7=1;
    psap_gain_update->psap_gain_ext_update_8=1;
    psap_gain_update->psap_gain_ext_update_9=1;
    psap_gain_update->psap_gain_ext_update_10=1;
    psap_gain_update->psap_gain_ext_update_11=1;
    psap_gain_update->psap_gain_ext_update_12=1;
    psap_gain_update->psap_gain_ext_update_13=1;
    psap_gain_update->psap_gain_ext_update_14=1;
    psap_gain_update->psap_gain_ext_update_15=1;
    psap_gain_update->psap_gain_ext_update_16=1;
}

static void psap_ctrl_reg_init(void)
{
    int i;
#ifdef PSAP_DEBUG_TRACE
    LOG_I("%s", __func__);
#endif

    psap_config->psap_enable=0;
  //  psap_config->psap_adc_sel=0;
  //  psap_config->psap_rate_sel=0;
  //  psap_config->psap_adc_rate_sel=0;

//    psap_config->adc_psap_mode=0;
//    psap_config->down_sel_psap=0;
//    psap_config->dac_psap_mode=0;
//    psap_config->up_sel_psap=0;
//    psap_config->psap_mix_mode=0;


    psap_band_num=PSAP_BAND_NUM;

    psap_ramp_total_gain_l = 0.0f;
    for(i=0;i<PSAP_BAND_NUM;i++)
    {
        psap_coef_gain_l[i] = 0;

        // If don't want to reset gain, can comment following gain.
        psap_bands_same_gain_l[i] = 1.0f;
        psap_bands_gain_l[i] = 1.0f;
    }

    psap_updata_gain();

    psap_gain_passzero_sel->psap_sbd_cnt=15;

    psap_cpd_c_cfg->psap_cpd_ct=24703;
    psap_cpd_c_cfg->psap_cpd_cs=0x6666;

    psap_cpd_w_cfg->psap_cpd_wt=21153;
    psap_cpd_w_cfg->psap_cpd_ws=0x4000;

    psap_cpd_e_cfg->psap_cpd_et=12871;
    psap_cpd_e_cfg->psap_cpd_es=0x2000;

    psap_cpd_coef_attack_cfg->psap_cpd_coefa_at=32428;
    psap_cpd_coef_attack_cfg->psap_cpd_coefb_at=32768-32428;

    psap_lmt_coef_attack_cfg->psap_lmt_coefa_at=28563;
    psap_lmt_coef_attack_cfg->psap_lmt_coefb_at=32768-28563;

    psap_lmt_th_cfg->psap_lmt_th_update=0;
    psap_lmt_th_cfg->psap_lmt_th=0x7ffff;
    psap_lmt_th_cfg->psap_lmt_th_update=1;

    psap_cpd_tav_cfg->psap_cpd_tava=0x7333;
    psap_cpd_tav_cfg->psap_cpd_tavb=0xccc;

    psap_cpd_coef_release_cfg->psap_cpd_coefa_rt=32734;
    psap_cpd_coef_release_cfg->psap_cpd_coefb_rt=32768-32734;

    psap_lmt_coef_release_cfg->psap_lmt_coefa_rt=32765;
    psap_lmt_coef_release_cfg->psap_lmt_coefb_rt=32768-32765;

    for(i=1;i<PSAP_BAND_NUM-1;i++)
    {
        psap_cpd_all_config[i-1].psap_cpd_c_cfg.psap_cpd_ct=24703;
        psap_cpd_all_config[i-1].psap_cpd_c_cfg.psap_cpd_cs=0x6666;

        psap_cpd_all_config[i-1].psap_cpd_w_cfg.psap_cpd_wt=21153;
        psap_cpd_all_config[i-1].psap_cpd_w_cfg.psap_cpd_ws=0x4000;

        psap_cpd_all_config[i-1].psap_cpd_e_cfg.psap_cpd_et=12871;
        psap_cpd_all_config[i-1].psap_cpd_e_cfg.psap_cpd_es=0x2000;

        psap_cpd_all_config[i-1].psap_cpd_coef_attack_cfg.psap_cpd_coefa_at=32428;
        psap_cpd_all_config[i-1].psap_cpd_coef_attack_cfg.psap_cpd_coefb_at=32768-32428;

        psap_cpd_all_config[i-1].psap_cpd_tav_cfg.psap_cpd_tava=0x7333;
        psap_cpd_all_config[i-1].psap_cpd_tav_cfg.psap_cpd_tavb=0xccc;

        psap_cpd_all_config[i-1].psap_cpd_coef_release_cfg.psap_cpd_coefa_rt=32734;
        psap_cpd_all_config[i-1].psap_cpd_coef_release_cfg.psap_cpd_coefb_rt=32768-32734;
    }

    psap_control->psap_iir_ext_bypass=0;
    psap_control->psap_iir_ext_bypass_1=0;
    psap_control->psap_iir_ext_bypass_2=0;
    psap_control->psap_iir_ext_bypass_3=0;
    psap_control->psap_iir_ext_bypass_4=0;
    psap_control->psap_iir_gaincal_ext_bypass=0;
    psap_control->psap_iir_gainuse_ext_bypass=0;
    psap_control->psap_iir_output_bit=0;
    psap_control->psap_cpd_delay=96;
    psap_control->psap_cpd_enable=0;
    psap_control->psap_lmt_delay=16;
    psap_control->psap_lmt_enable=0;

    for(i=0;i<PSAP_IIR_NUM;i++)
    {
        psap_iir_coef[i].iir0.a1=0;
        psap_iir_coef[i].iir0.a2=0;
        psap_iir_coef[i].iir0.a3=0;
        psap_iir_coef[i].iir0.b1=0;
        psap_iir_coef[i].iir0.b2=0;
        psap_iir_coef[i].iir0.b3=0;
        psap_iir_coef[i].iir0.b0=0;

        psap_iir_coef[i].iir1.a1=0;
        psap_iir_coef[i].iir1.a2=0;
        psap_iir_coef[i].iir1.a3=0;
        psap_iir_coef[i].iir1.b1=0;
        psap_iir_coef[i].iir1.b2=0;
        psap_iir_coef[i].iir1.b3=0;
        psap_iir_coef[i].iir1.b0=0;
    }

    for(i=0;i<PSAP_BAND_NUM;i++)
    {
        gain_iir_coefs[i].a1=-iir_coef_gain_ramp_psap_normal.coef_a[1];
        gain_iir_coefs[i].a2=-iir_coef_gain_ramp_psap_normal.coef_a[2];
        gain_iir_coefs[i].b1=iir_coef_gain_ramp_psap_normal.coef_b[1];
        gain_iir_coefs[i].b2=iir_coef_gain_ramp_psap_normal.coef_b[2];
        gain_iir_coefs[i].b0=iir_coef_gain_ramp_psap_normal.coef_b[0];
    }

    psap_control->psap_iir_enable=0;
    psap_config->psap_enable=0;

    return;
}

int psap_opened(void)
{
    return psap_open_flag;
}

void psap_set_total_gain_f32(float gain_l, float gain_r)
{
    LOG_I( "[%s] gain_l(x1000): %d", __func__, (int32_t)(gain_l*1000));

    psap_ramp_total_gain_l = gain_l;

    psap_updata_gain();

    return;
}

void psap_set_bands_same_gain_f32(float gain_l, float gain_r, uint32_t index_start, uint32_t index_end)
{
    ASSERT(index_start <= index_end, "[%s] index_start(%d) > index_end(%d)", __func__, index_start, index_end);
    ASSERT(index_end < PSAP_BAND_NUM, "[%s] index_end(%d) >= PSAP_BAND_NUM(%d)", __func__, index_end, PSAP_BAND_NUM);

    LOG_I( "[%s] gain_l(x1000): %d, index: %d ~ %d", __func__, (int32_t)(gain_l * 1000), index_start, index_end);

    for(int i=0; i<index_start; i++) {
        psap_bands_same_gain_l[i] = 1.0f;
    }

    for(int i=index_start; i<=index_end; i++) {
        psap_bands_same_gain_l[i] = gain_l;
    }

    for(int i=index_end+1; i<PSAP_BAND_NUM; i++) {
        psap_bands_same_gain_l[i] = 1.0f;
    }

    psap_updata_gain();

    return;
}

void psap_set_bands_gain_f32(float *gain_l, float *gain_r)
{
    if (gain_l != NULL) {
        LOG_I( "PSAP: Set left bands gain");
        for(int i=0; i<PSAP_BAND_NUM; i++) {
            psap_bands_gain_l[i] = *(gain_l + i);
        }
    } else {
        LOG_I( "PSAP: Reset left bands gain");
        for(int i=0; i<PSAP_BAND_NUM; i++) {
            psap_bands_gain_l[i] = 1.0f;
        }
    }

    if (gain_r != NULL) {
        LOG_I( "[%s] WARNING: Don't support right channel!", __func__);
    }

    psap_updata_gain();
}

void psap_enable(int ch_map)
{
    psap_set_total_gain_f32(1.0, 1.0);

    return;
}

void psap_disable(int ch_map)
{
    int i;

    LOG_I("%s", __func__);

    //setting the gain to mute.
    psap_gain_update->psap_gain_ext_update_0=0;
    psap_gain_update->psap_gain_ext_update_1=0;
    psap_gain_update->psap_gain_ext_update_2=0;
    psap_gain_update->psap_gain_ext_update_3=0;
    psap_gain_update->psap_gain_ext_update_4=0;
    psap_gain_update->psap_gain_ext_update_5=0;
    psap_gain_update->psap_gain_ext_update_6=0;
    psap_gain_update->psap_gain_ext_update_7=0;
    psap_gain_update->psap_gain_ext_update_8=0;
    psap_gain_update->psap_gain_ext_update_9=0;
    psap_gain_update->psap_gain_ext_update_10=0;
    psap_gain_update->psap_gain_ext_update_11=0;
    psap_gain_update->psap_gain_ext_update_12=0;
    psap_gain_update->psap_gain_ext_update_13=0;
    psap_gain_update->psap_gain_ext_update_14=0;
    psap_gain_update->psap_gain_ext_update_15=0;
    psap_gain_update->psap_gain_ext_update_16=0;

    for(i=0;i<PSAP_BAND_NUM;i++)
    {
        psap_gain_ext[i]=0;
    }

    psap_gain_update->psap_gain_ext_update_0=1;
    psap_gain_update->psap_gain_ext_update_1=1;
    psap_gain_update->psap_gain_ext_update_2=1;
    psap_gain_update->psap_gain_ext_update_3=1;
    psap_gain_update->psap_gain_ext_update_4=1;
    psap_gain_update->psap_gain_ext_update_5=1;
    psap_gain_update->psap_gain_ext_update_6=1;
    psap_gain_update->psap_gain_ext_update_7=1;
    psap_gain_update->psap_gain_ext_update_8=1;
    psap_gain_update->psap_gain_ext_update_9=1;
    psap_gain_update->psap_gain_ext_update_10=1;
    psap_gain_update->psap_gain_ext_update_11=1;
    psap_gain_update->psap_gain_ext_update_12=1;
    psap_gain_update->psap_gain_ext_update_13=1;
    psap_gain_update->psap_gain_ext_update_14=1;
    psap_gain_update->psap_gain_ext_update_15=1;
    psap_gain_update->psap_gain_ext_update_16=1;

    return;
}
int psap_open(int ch_map)
{
    PSAP_ERROR err=PSAP_NO_ERR;

    LOG_I("%s", __func__);

    if(psap_open_flag==1)
    {
        LOG_I("%s psap have been opened! ", __func__);
        return err;
    }

    hal_codec_psap_enable(48000000);

    psap_ctrl_reg_init();

#if defined(AUDIO_PSAP_DEHOWLING_HW)
    anc_open(ANC_DEHOWLING);
#endif
    psap_open_flag=1;

    return err;
}

void psap_close(void)
{
    LOG_I("%s", __func__);

    psap_set_mode(AUD_CHANNEL_MAP_CH0,PSAP_MODE_ONLY_MIC);

    hal_codec_psap_disable();
#if defined(AUDIO_PSAP_DEHOWLING_HW)
    anc_close(ANC_DEHOWLING);
#endif
    psap_open_flag=0;

    return;
}

int psap_get_gain(int32_t *gain_ch_l, int32_t *gain_ch_r, uint32_t band_index)
{
    PSAP_ERROR err=PSAP_NO_ERR;

    LOG_I("%s", __func__);

    if(psap_open_flag==0)
    {
        LOG_I("%s psap have been not opened! ", __func__);
        *gain_ch_l=0;
        *gain_ch_r=0;
        return PSAP_OTHER_ERR;
    }

    if(band_index>psap_band_num)
    {
        LOG_I("%s Error band_index!:%d ", __func__,band_index);
        *gain_ch_l=0;
        *gain_ch_r=0;
        return PSAP_OTHER_ERR;
    }

    *gain_ch_l=psap_coef_gain_l[band_index];
    *gain_ch_r=psap_coef_gain_l[band_index];

    LOG_I("gain_ch_l:%d,gain_ch_r:%d,band_index:%d",*gain_ch_l,*gain_ch_r,band_index);

    return err;
}
int psap_set_gain(int32_t gain_ch_l, int32_t gain_ch_r, uint32_t band_index)
{
    PSAP_ERROR err=PSAP_NO_ERR;

    LOG_I("%s", __func__);

    LOG_I("gain_ch_l:0x%08x,gain_ch_r:0x%08x,band_index:%d", gain_ch_l,gain_ch_r,band_index);

    if(psap_open_flag==0)
    {
        LOG_I("%s psap have been not opened! ", __func__);
        return PSAP_OTHER_ERR;
    }

    if(band_index>psap_band_num)
    {
        LOG_I("%s Error band_index!:%d ", __func__,band_index);
        return PSAP_OTHER_ERR;
    }

    psap_coef_gain_l[band_index]=gain_ch_l;

    psap_updata_gain();

    return err;
}

int psap_set_mode(int ch_map, PSAP_MODE psap_mode)
{
    PSAP_ERROR err=PSAP_NO_ERR;
    struct HAL_CODEC_PSAP_CFG_T psap_cfg;
    LOG_I("%s", __func__);

    if(psap_open_flag==0)
    {
        LOG_I("%s psap have been not opened! ", __func__);
        return PSAP_OTHER_ERR;
    }
    psap_cfg.din0_samp_delay=0;
    psap_cfg.din0_gain=1.0f;
    psap_cfg.din1_sign=0;

    switch(psap_mode)
    {
        case PSAP_MODE_ONLY_MIC:
        psap_cfg.mode=HAL_CODEC_PSAP_MODE_ADC;
        break;
        case PSAP_MODE_MIC_MUSIC:
        psap_cfg.mode=HAL_CODEC_PSAP_MODE_ADC_DAC;
        break;
        case PSAP_MODE_ONLY_MUSIC:
        psap_cfg.mode=HAL_CODEC_PSAP_MODE_DAC;
        break;
        default:
        psap_cfg.mode=HAL_CODEC_PSAP_MODE_ADC;
        break;
    }

    hal_codec_psap_setup(AUD_CHANNEL_MAP_CH0,&psap_cfg);

    return err;
}




