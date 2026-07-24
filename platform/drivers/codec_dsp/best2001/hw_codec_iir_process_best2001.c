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
** namer��ANC filter
** description��IIR and FIR filter
** version��V1.0
** author�� xuml
** modify��2017.7.11.
*******************************************************************************/

#include "mm_dbg.h"
#include <stdio.h>
#include <string.h>

#include "plat_addr_map.h"
#include "filter_debug_trace.h"
#include "hal_cmu.h"

#include "iir_process.h"
#include "hw_codec_iir_process.h"


/**********************************mocro**************************************/

typedef short int16;
typedef unsigned short uint16;
typedef int   int32;
typedef unsigned int   uint32;


/*
1	dc	codec_iir_d_enable	RW	0	0	dac eq iir enable
1	dc	codec_iir_d_ch0_bypass	RW	0	1	iir ch0 bypass
1	dc	codec_iir_d_ch1_bypass	RW	0	2	iir ch1 bypass
5	dc	codec_iir_d_count_ch0	RW	0	3	iir ch0 count
5	dc	codec_iir_d_count_ch1	RW	0	8	iir ch1 count
1	dc	codec_dac_L_iir_enable	RW	0	13	1: use iir ch0 for dac L eq
1	dc	codec_dac_R_iir_enable	RW	0	14	1: use iir ch1 for dac R eq
1	dc	codec_iir_d_coef_swap	RW	0	15	0: use iir coef memory0;  1: use iir coef memory1;
1	dc	codec_iir_d_coef_swap_status	R	0	16	read only;  coef memoryX used currently
17	dc		x

*/
struct _deq_iir_d_control
{
	uint32 codec_d_iir_enable : 1;
	uint32 codec_d_iir_ch0_bypass : 1;
	uint32 codec_d_iir_ch1_bypass : 1;
	uint32 codec_d_iir_count_ch0 : 5;
	uint32 codec_d_iir_count_ch1 : 5;
	uint32 codec_dac_L_iir_enable : 1;
	uint32 codec_dac_R_iir_enable : 1;
       uint32 codec_d_iir_coef_swap : 1;
       uint32 codec_d_iir_coef_swap_status : 1;

	uint32 reserved : 15;
};

/*
1	104	codec_iir_d_gaincal_ext_ch0_bypass	RW	1	0	gain ramp ch0 cal bypass, use for dac0_L
1	104	codec_iir_d_gaincal_ext_ch1_bypass	RW	1	1	gain ramp ch1 cal bypass, use for dac1_L
1	104	codec_iir_d_gaincal_ext_ch2_bypass	RW	1	2	gain ramp ch2 cal bypass, use for dac0_R
1	104	codec_iir_d_gaincal_ext_ch3_bypass	RW	1	3	gain ramp ch3 cal bypass, use for dac1_R
1	104	codec_iir_d_gainuse_ext_ch0_bypass	RW	1	4	gain ramp ch0 use bypass
1	104	codec_iir_d_gainuse_ext_ch1_bypass	RW	1	5
1	104	codec_iir_d_gainuse_ext_ch2_bypass	RW	1	6
1	104	codec_iir_d_gainuse_ext_ch3_bypass	RW	1	7
1	104	codec_iir_d_gain_ext_update_ch0	RW	0	8	  posedge to update extern gain for iir ch0
1	104	codec_iir_d_gain_ext_update_ch1	RW	0	9
1	104	codec_iir_d_gain_ext_update_ch2	RW	0	10
1	104	codec_iir_d_gain_ext_update_ch3	RW	0	11
1	104	codec_iir_d_gain_ext_sel_ch0	RW	0	12	 =1, extern gain updated when pass0; =0, updated right away
1	104	codec_iir_d_gain_ext_sel_ch1	RW	0	13
1	104	codec_iir_d_gain_ext_sel_ch2	RW	0	14
1	104	codec_iir_d_gain_ext_sel_ch3	RW	0	15
16	104		x
*/
struct _deq_iir_gain_config
{
	uint32 codec_iir_d_gaincal_ext_ch0_bypass : 1;
	uint32 codec_iir_d_gaincal_ext_ch1_bypass : 1;
	uint32 codec_iir_d_gaincal_ext_ch2_bypass : 1;
	uint32 codec_iir_d_gaincal_ext_ch3_bypass : 1;
	uint32 codec_iir_d_gainuse_ext_ch0_bypass : 1;
	uint32 codec_iir_d_gainuse_ext_ch1_bypass : 1;
	uint32 codec_iir_d_gainuse_ext_ch2_bypass : 1;
       uint32 codec_iir_d_gainuse_ext_ch3_bypass : 1;
       uint32 codec_iir_d_gain_ext_update_ch0 : 1;
       uint32 codec_iir_d_gain_ext_update_ch1 : 1;
       uint32 codec_iir_d_gain_ext_update_ch2 : 1;
       uint32 codec_iir_d_gain_ext_update_ch3 : 1;
       uint32 codec_iir_d_gain_ext_sel_ch0 : 1;
       uint32 codec_iir_d_gain_ext_sel_ch1 : 1;
       uint32 codec_iir_d_gain_ext_sel_ch2 : 1;
       uint32 codec_iir_d_gain_ext_sel_ch3 : 1;
	uint32 reserved : 16;
};


/*
1	108	codec_iir_d_lmt_ch0_bypass	RW	1	0	 =1, bypass limiter for iir ch0
1	108	codec_iir_d_lmt_ch1_bypass	RW	1	1	 =1, bypass limiter for iir ch1
3	108	codec_iir_d_lmt_th_ch0	RW	0		2	limiter threshold, 0: 0x800000,0db; 1: 0x400000, -6db; 2: -12db;�� 7: 0x10000, -42db
3	108	codec_iir_d_lmt_th_ch1	RW	0		5
1	108	codec_iir_d_lmt_th_update_ch0		RW	0	8	threshold update, high level to update
1	108	codec_iir_d_lmt_th_update_ch1		RW	0	9
7	108	codec_iir_d_lmt_delay	RW	0		10	limiter delay for attack time
3	108	codec_iir_d_lmt_rls_rate	RW		0	17	limiter release rate, 0: 0x80=128; 1: 0x100=256; 2: 0x200;�� 7: 0x4000;
20	108		x
*/
struct _deq_iir_limiter_config
{
	uint32 codec_iir_d_lmt_ch0_bypass : 1;
	uint32 codec_iir_d_lmt_ch1_bypass : 1;
	uint32 codec_iir_d_lmt_th_ch0 : 3;
	uint32 codec_iir_d_lmt_th_ch1 : 3;
	uint32 codec_iir_d_lmt_th_update_ch0 : 1;
	uint32 codec_iir_d_lmt_th_update_ch1 : 1;
	uint32 codec_iir_d_lmt_delay : 7;
       uint32 codec_iir_d_lmt_rls_rate : 3;

	uint32 reserved : 12;
};


/*
iir coefficients(b/a Q27)
a1
a2
b1
b2
b0
*/
struct _deq_iir_coefs
{
	int32 a1 ;
	int32 a2 ;

	int32 b1 ;
	int32 b2 ;
	int32 b0 ;
};

typedef struct _anc_rir_coefs {
	int32_t coef_b[3];
	int32_t coef_a[3];
} anc_iir_coefs;

#define FIXED_GAIN_RAMP_Q (1<<27)

//#define DEQ_LIMITER

static const anc_iir_coefs   iir_coef_gain_ramp_l=
{
    .coef_b={            8,           16,            8},
    .coef_a={134217728,   -268106408,    133888712},
};

static const anc_iir_coefs   iir_coef_gain_ramp_r=
{
    .coef_b={            8,           16,            8},
    .coef_a={134217728,   -268106408,    133888712},
};
#ifdef DEQ_LIMITER

#define DEQ_LIMITER_DELAY (60)

//#define DEQ_LIMITER_RATE (16384)

//#define DEQ_LIMITER_ATT_FLOAT ((1.0f/9.0f)^(1.0f/ANC_LIMITER_DELAY))

//#define DEQ_LIMITER_ATT_A1 (int32)(ANC_LIMITER_ATT_FLOAT*(2^27))
//#define DEQ_LIMITER_ATT_B1 (int32)((1.0f-ANC_LIMITER_ATT_FLOAT)*(2^27))

//#define DEQ_LIMITER_RELEASE_B1 (ANC_LIMITER_ATT_B1/ANC_LIMITER_RATE)
//#define DEQ_LIMITER_RELEASE_A1 (2^27-ANC_LIMITER_RELEASE_B1)

#if DEQ_LIMITER_DELAY==60

#define DEQ_LIMITER_ATT_A1 (129391528)
#define DEQ_LIMITER_ATT_B1 (4826200)

#define DEQ_LIMITER_RELEASE_B1 (295)
#define DEQ_LIMITER_RELEASE_A1 (134217433)

#elif DEQ_LIMITER_DELAY==45

#define DEQ_LIMITER_ATT_A1 (127821672)
#define DEQ_LIMITER_ATT_B1 (6396056)

#define DEQ_LIMITER_RELEASE_B1 (390)
#define DEQ_LIMITER_RELEASE_A1 (134217338)

#elif DEQ_LIMITER_DELAY==30

#define DEQ_LIMITER_ATT_A1 (124738868)
#define DEQ_LIMITER_ATT_B1 (9478860)

#define DEQ_LIMITER_RELEASE_B1 (579)
#define DEQ_LIMITER_RELEASE_A1 (134217149)

#elif DEQ_LIMITER_DELAY==15

#define DEQ_LIMITER_ATT_A1 (115929434)
#define DEQ_LIMITER_ATT_B1 (18288294)

#define DEQ_LIMITER_RELEASE_B1 (1116)
#define DEQ_LIMITER_RELEASE_A1 (134216612)

#elif DEQ_LIMITER_DELAY==8

#define DEQ_LIMITER_ATT_A1 (101983419)
#define DEQ_LIMITER_ATT_B1 (32234309)

#define DEQ_LIMITER_RELEASE_B1 (1967)
#define DEQ_LIMITER_RELEASE_A1 (134215761)

#else
//#define DEQ_LIMITER_ATT_A1 (0)
//#define DEQ_LIMITER_ATT_B1 (0)

//#define DEQ_LIMITER_RELEASE_B1 (0)
//#define DEQ_LIMITER_RELEASE_A1 (0)

#error "ERROR DEQ_LIMITER_DELAY"
#endif

static const anc_iir_coefs   iir_coef_limiter_attack_l=
{
    .coef_b={0,    DEQ_LIMITER_ATT_B1,     0},
    .coef_a={134217728,    -DEQ_LIMITER_ATT_A1,     0},
};
static const anc_iir_coefs   iir_coef_limiter_attack_r=
{
    .coef_b={0,    DEQ_LIMITER_ATT_B1,     0},
    .coef_a={134217728,    -DEQ_LIMITER_ATT_A1,     0},
};

static const anc_iir_coefs   iir_coef_limiter_release_l=
{
    .coef_b={0,    DEQ_LIMITER_RELEASE_B1,     0},
    .coef_a={134217728,    -DEQ_LIMITER_RELEASE_A1,     0},
};
static const anc_iir_coefs   iir_coef_limiter_release_r=
{
    .coef_b={0,    DEQ_LIMITER_RELEASE_B1,     0},
    .coef_a={134217728,    -DEQ_LIMITER_RELEASE_A1,     0},
};
#endif


/*******************************data struction***********************************/


#define DEQ_BASE                            ((uint32)CODEC_BASE)


//IIR registers
volatile static struct _deq_iir_d_control *deq_iir_d_control=(volatile struct _deq_iir_d_control *)(DEQ_BASE+0xdc);
volatile static struct _deq_iir_gain_config *deq_iir_gain_config=(volatile struct _deq_iir_gain_config *)(DEQ_BASE+0x104);

#ifdef DEQ_LIMITER
volatile static struct _deq_iir_limiter_config *deq_iir_limiter_config=(volatile struct _deq_iir_limiter_config *)(DEQ_BASE+0x108);
#endif

volatile static struct _deq_iir_coefs *deq_dac_iir_coefs0_l=(volatile struct _deq_iir_coefs *)(DEQ_BASE+0x2000);
volatile static struct _deq_iir_coefs *deq_dac_iir_coefs0_r=(volatile struct _deq_iir_coefs *)(DEQ_BASE+0x2190);

volatile static struct _deq_iir_coefs *deq_gain_iir_coefs0_l0=(volatile struct _deq_iir_coefs *)(DEQ_BASE+0x2320);
volatile static struct _deq_iir_coefs *deq_gain_iir_coefs0_l1=(volatile struct _deq_iir_coefs *)(DEQ_BASE+0x2334);
volatile static struct _deq_iir_coefs *deq_gain_iir_coefs0_r0=(volatile struct _deq_iir_coefs *)(DEQ_BASE+0x2348);
volatile static struct _deq_iir_coefs *deq_gain_iir_coefs0_r1=(volatile struct _deq_iir_coefs *)(DEQ_BASE+0x235c);

#ifdef DEQ_LIMITER
volatile static struct _deq_iir_coefs *deq_limiter_iir_coefs0_l=(volatile struct _deq_iir_coefs *)(DEQ_BASE+0x2370);
volatile static struct _deq_iir_coefs *deq_limiter_iir_coefs0_r=(volatile struct _deq_iir_coefs *)(DEQ_BASE+0x2398);
#endif

volatile static struct _deq_iir_coefs *deq_dac_iir_coefs1_l=(volatile struct _deq_iir_coefs *)(DEQ_BASE+0x2400);
volatile static struct _deq_iir_coefs *deq_dac_iir_coefs1_r=(volatile struct _deq_iir_coefs *)(DEQ_BASE+0x2590);

volatile static struct _deq_iir_coefs *deq_gain_iir_coefs1_l0=(volatile struct _deq_iir_coefs *)(DEQ_BASE+0x2720);
volatile static struct _deq_iir_coefs *deq_gain_iir_coefs1_l1=(volatile struct _deq_iir_coefs *)(DEQ_BASE+0x2734);
volatile static struct _deq_iir_coefs *deq_gain_iir_coefs1_r0=(volatile struct _deq_iir_coefs *)(DEQ_BASE+0x2748);
volatile static struct _deq_iir_coefs *deq_gain_iir_coefs1_r1=(volatile struct _deq_iir_coefs *)(DEQ_BASE+0x275c);

#ifdef DEQ_LIMITER
volatile static struct _deq_iir_coefs *deq_limiter_iir_coefs1_l=(volatile struct _deq_iir_coefs *)(DEQ_BASE+0x2770);
volatile static struct _deq_iir_coefs *deq_limiter_iir_coefs1_r=(volatile struct _deq_iir_coefs *)(DEQ_BASE+0x2798);
#endif

volatile static int32 *codec_iir_d_gain_ext_ch0=(volatile int32 *)(DEQ_BASE+0x010c);
volatile static int32 *codec_iir_d_gain_ext_ch1=(volatile int32 *)(DEQ_BASE+0x0110);
volatile static int32 *codec_iir_d_gain_ext_ch2=(volatile int32 *)(DEQ_BASE+0x0114);
volatile static int32 *codec_iir_d_gain_ext_ch3=(volatile int32 *)(DEQ_BASE+0x0118);

volatile static int dac_open_flag=0;

static HW_CODEC_IIR_FILTERS_T dac_filtes_l_old;
static HW_CODEC_IIR_FILTERS_T dac_filtes_r_old;

volatile static int8_t dac_iir_reset_flag;

volatile static int8_t iir_coef_using;

volatile static enum AUD_SAMPRATE_T sample_rate_dac;

#define IIR_DAC_COUNTER (20)

#define IIR_DAC_COUNTER_L (15)
#define IIR_DAC_COUNTER_R (15)

int hw_codec_iir_set_cfg(HW_CODEC_IIR_CFG_T *cfg,enum AUD_SAMPRATE_T sample_rate,HW_CODEC_IIR_TYPE_T hw_iir_type)
{
    HW_CODEC_IIR_ERROR err=HW_CODEC_IIR_NO_ERR;
    int i;
    const HW_CODEC_IIR_FILTERS_T *dac_filtes_l=NULL;
    const HW_CODEC_IIR_FILTERS_T *dac_filtes_r=NULL;

    if (cfg == NULL)
    {
        LOG_I("%s: cfg is null", __func__);
        return HW_CODEC_IIR_OTHER_ERR;
    }

    if ((hw_iir_type==HW_CODEC_IIR_DAC && dac_open_flag==0)) {
        LOG_I("%s: hw_iir_type=%d not opened", __func__, hw_iir_type);
        return HW_CODEC_IIR_TYPE_ERR;
    }

    if(hw_iir_type==HW_CODEC_IIR_DAC)
    {
        sample_rate_dac=sample_rate;
    }

    LOG_I("%s: iir_coef_using:%d,codec_d_iir_coef_swap:%d", __func__,iir_coef_using,deq_iir_d_control->codec_d_iir_coef_swap);

    if(hw_iir_type==HW_CODEC_IIR_DAC)
    {
        dac_filtes_l = &(cfg->iir_filtes_l);
        dac_filtes_r = &(cfg->iir_filtes_r);
    }

    LOG_I("EQ counter %d", dac_filtes_l->iir_counter);
    for(int j = 0; j <IIR_DAC_COUNTER; j++)
    {
        //LOG_I("iir coef ff l 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x",
        LOG_I("iir coef eq l %10d, %10d, %10d, %10d, %10d, %10d",  \
        dac_filtes_l->iir_coef[j].coef_b[0], \
        dac_filtes_l->iir_coef[j].coef_b[1], \
        dac_filtes_l->iir_coef[j].coef_b[2], \
        dac_filtes_l->iir_coef[j].coef_a[0], \
        dac_filtes_l->iir_coef[j].coef_a[1], \
        dac_filtes_l->iir_coef[j].coef_a[2]);
    }


    //LOG_I("ff_l:%d,ff_r:%d,fb_l:%d,fb_r:%d",dac_filtes_l->total_gain,dac_filtes_r->total_gain,adc_filtes_l->total_gain,adc_filtes_r->total_gain);

    if(hw_iir_type==HW_CODEC_IIR_DAC)
    {
        if(dac_filtes_l_old.iir_counter==dac_filtes_l->iir_counter
        &&dac_filtes_r_old.iir_counter==dac_filtes_r->iir_counter
        &&dac_filtes_l_old.iir_bypass_flag==dac_filtes_l->iir_bypass_flag
        &&dac_filtes_r_old.iir_bypass_flag==dac_filtes_r->iir_bypass_flag
        &&dac_filtes_l->iir_bypass_flag==0
        &&dac_filtes_r->iir_bypass_flag==0)
        {
            LOG_I("%s: switching dac IIR", __func__);

            if(iir_coef_using==0)
            {
                for(i=0;i<IIR_DAC_COUNTER;i++)
                {
                    deq_dac_iir_coefs1_l[i].a1=-dac_filtes_l->iir_coef[i].coef_a[1];
                    deq_dac_iir_coefs1_l[i].a2=-dac_filtes_l->iir_coef[i].coef_a[2];
                    deq_dac_iir_coefs1_l[i].b0=dac_filtes_l->iir_coef[i].coef_b[0];
                    deq_dac_iir_coefs1_l[i].b1=dac_filtes_l->iir_coef[i].coef_b[1];
                    deq_dac_iir_coefs1_l[i].b2=dac_filtes_l->iir_coef[i].coef_b[2];

                    deq_dac_iir_coefs1_r[i].a1=-dac_filtes_r->iir_coef[i].coef_a[1];
                    deq_dac_iir_coefs1_r[i].a2=-dac_filtes_r->iir_coef[i].coef_a[2];
                    deq_dac_iir_coefs1_r[i].b0=dac_filtes_r->iir_coef[i].coef_b[0];
                    deq_dac_iir_coefs1_r[i].b1=dac_filtes_r->iir_coef[i].coef_b[1];
                    deq_dac_iir_coefs1_r[i].b2=dac_filtes_r->iir_coef[i].coef_b[2];

                    dac_filtes_l_old.iir_coef[i]=dac_filtes_l->iir_coef[i];
                    dac_filtes_r_old.iir_coef[i]=dac_filtes_r->iir_coef[i];
                }
                deq_iir_d_control->codec_d_iir_coef_swap=1;
                while(1)
                {
                    if(deq_iir_d_control->codec_d_iir_coef_swap_status==1) break;
                }

            }
            else
            {

                for(i=0;i<IIR_DAC_COUNTER;i++)
                {
                    deq_dac_iir_coefs0_l[i].a1=-dac_filtes_l->iir_coef[i].coef_a[1];
                    deq_dac_iir_coefs0_l[i].a2=-dac_filtes_l->iir_coef[i].coef_a[2];
                    deq_dac_iir_coefs0_l[i].b0=dac_filtes_l->iir_coef[i].coef_b[0];
                    deq_dac_iir_coefs0_l[i].b1=dac_filtes_l->iir_coef[i].coef_b[1];
                    deq_dac_iir_coefs0_l[i].b2=dac_filtes_l->iir_coef[i].coef_b[2];

                    deq_dac_iir_coefs0_r[i].a1=-dac_filtes_r->iir_coef[i].coef_a[1];
                    deq_dac_iir_coefs0_r[i].a2=-dac_filtes_r->iir_coef[i].coef_a[2];
                    deq_dac_iir_coefs0_r[i].b0=dac_filtes_r->iir_coef[i].coef_b[0];
                    deq_dac_iir_coefs0_r[i].b1=dac_filtes_r->iir_coef[i].coef_b[1];
                    deq_dac_iir_coefs0_r[i].b2=dac_filtes_r->iir_coef[i].coef_b[2];

                    dac_filtes_l_old.iir_coef[i]=dac_filtes_l->iir_coef[i];
                    dac_filtes_r_old.iir_coef[i]=dac_filtes_r->iir_coef[i];
                }
                deq_iir_d_control->codec_d_iir_coef_swap=0;
                while(1)
                {
                    if(deq_iir_d_control->codec_d_iir_coef_swap_status==0) break;
                }
            }
            dac_iir_reset_flag=0;
            iir_coef_using=1-iir_coef_using;
        }
        else
        {
            LOG_I("%s: no switching dac", __func__);

            dac_filtes_l_old.iir_counter=dac_filtes_l->iir_counter;
            dac_filtes_r_old.iir_counter=dac_filtes_r->iir_counter;

            dac_filtes_l_old.iir_bypass_flag=dac_filtes_l->iir_bypass_flag;
            dac_filtes_r_old.iir_bypass_flag=dac_filtes_r->iir_bypass_flag;


            //disable all IIR filter before updata the coefs;
            deq_iir_d_control->codec_d_iir_enable=0;

            dac_iir_reset_flag=1;
        }
    }

    //DAC IIR settings
    if(hw_iir_type==HW_CODEC_IIR_DAC&&dac_iir_reset_flag==1)
    {

        for(i=0;i<IIR_DAC_COUNTER_L;i++)
        {
            dac_filtes_l_old.iir_coef[i]=dac_filtes_l->iir_coef[i];
            dac_filtes_r_old.iir_coef[i]=dac_filtes_r->iir_coef[i];
         }

        LOG_I("%s: resetting dac", __func__);

        //setting the dac IIR filters;
        if(dac_filtes_l->iir_counter>IIR_DAC_COUNTER_L)
        {
            deq_iir_d_control->codec_d_iir_count_ch0=IIR_DAC_COUNTER_L;
        }
        else if(dac_filtes_l->iir_counter>0)
        {
            deq_iir_d_control->codec_d_iir_count_ch0=dac_filtes_l->iir_counter;
        }
        else
        {
            deq_iir_d_control->codec_dac_L_iir_enable=0;
            LOG_I("Error dac_filtes_l IIR counter:%d", dac_filtes_l->iir_counter);
            err=HW_CODEC_IIR_COUNTER_ERR;
            return err;
        }

        if(iir_coef_using==0)
        {
            //dac left ch iir coefs settings
            for(i=0;i<IIR_DAC_COUNTER_L;i++)
            {
                deq_dac_iir_coefs0_l[i].a1=-dac_filtes_l->iir_coef[i].coef_a[1];
                deq_dac_iir_coefs0_l[i].a2=-dac_filtes_l->iir_coef[i].coef_a[2];
                deq_dac_iir_coefs0_l[i].b0=dac_filtes_l->iir_coef[i].coef_b[0];
                deq_dac_iir_coefs0_l[i].b1=dac_filtes_l->iir_coef[i].coef_b[1];
                deq_dac_iir_coefs0_l[i].b2=dac_filtes_l->iir_coef[i].coef_b[2];
            }
        }
        else
        {
            //dac left ch iir coefs settings
            for(i=0;i<IIR_DAC_COUNTER_L;i++)
            {
                deq_dac_iir_coefs1_l[i].a1=-dac_filtes_l->iir_coef[i].coef_a[1];
                deq_dac_iir_coefs1_l[i].a2=-dac_filtes_l->iir_coef[i].coef_a[2];
                deq_dac_iir_coefs1_l[i].b0=dac_filtes_l->iir_coef[i].coef_b[0];
                deq_dac_iir_coefs1_l[i].b1=dac_filtes_l->iir_coef[i].coef_b[1];
                deq_dac_iir_coefs1_l[i].b2=dac_filtes_l->iir_coef[i].coef_b[2];
            }
        }

        if(dac_filtes_r->iir_counter>IIR_DAC_COUNTER_R)
        {
            deq_iir_d_control->codec_d_iir_count_ch1=IIR_DAC_COUNTER_R;
        }
        else if(dac_filtes_r->iir_counter>0)
        {
            deq_iir_d_control->codec_d_iir_count_ch1=dac_filtes_r->iir_counter;
        }
        else
        {
             deq_iir_d_control->codec_dac_R_iir_enable=0;
            LOG_I("Error dac_filtes_r IIR counter:%d", dac_filtes_r->iir_counter);
            err=HW_CODEC_IIR_COUNTER_ERR;
            return err;
        }
        if(iir_coef_using==0)
        {
            //dac left ch iir coefs settings
            for(i=0;i<IIR_DAC_COUNTER_R;i++)
            {
                deq_dac_iir_coefs0_r[i].a1=-dac_filtes_r->iir_coef[i].coef_a[1];
                deq_dac_iir_coefs0_r[i].a2=-dac_filtes_r->iir_coef[i].coef_a[2];
                deq_dac_iir_coefs0_r[i].b0=dac_filtes_r->iir_coef[i].coef_b[0];
                deq_dac_iir_coefs0_r[i].b1=dac_filtes_r->iir_coef[i].coef_b[1];
                deq_dac_iir_coefs0_r[i].b2=dac_filtes_r->iir_coef[i].coef_b[2];
            }
        }
        else
        {
            //dac right ch iir coefs settings
            for(i=0;i<IIR_DAC_COUNTER_R;i++)
            {
                deq_dac_iir_coefs1_r[i].a1=-dac_filtes_r->iir_coef[i].coef_a[1];
                deq_dac_iir_coefs1_r[i].a2=-dac_filtes_r->iir_coef[i].coef_a[2];
                deq_dac_iir_coefs1_r[i].b0=dac_filtes_r->iir_coef[i].coef_b[0];
                deq_dac_iir_coefs1_r[i].b1=dac_filtes_r->iir_coef[i].coef_b[1];
                deq_dac_iir_coefs1_r[i].b2=dac_filtes_r->iir_coef[i].coef_b[2];
            }
        }

        //enable all IIR filters
        deq_iir_d_control->codec_d_iir_enable=1;
        LOG_I("%s:deq_iir_d_control->codec_d_iir_enable %x", __func__, deq_iir_d_control->codec_d_iir_enable);

       //enable dac iir;
#if IIR_DAC_COUNTER_L==0
        deq_iir_d_control->codec_dac_L_iir_enable=0;
        deq_iir_d_control->codec_d_iir_ch0_bypass=1;
#else
       if(dac_filtes_l->iir_bypass_flag==0)
        {
            deq_iir_d_control->codec_dac_L_iir_enable=1;
            deq_iir_d_control->codec_d_iir_ch0_bypass=0;
        }
        else
        {
            deq_iir_d_control->codec_dac_L_iir_enable=0;
      	     deq_iir_d_control->codec_d_iir_ch0_bypass=1;
        }
#endif

#if IIR_DAC_COUNTER_R==0
        deq_iir_d_control->codec_dac_R_iir_enable=0;
        deq_iir_d_control->codec_d_iir_ch1_bypass=1;
#else
        if(dac_filtes_r->iir_bypass_flag==0)
        {
            deq_iir_d_control->codec_dac_R_iir_enable=1;
 	     deq_iir_d_control->codec_d_iir_ch1_bypass=0;
        }
        else
        {
            deq_iir_d_control->codec_dac_R_iir_enable=0;
      	     deq_iir_d_control->codec_d_iir_ch1_bypass=1;
        }
#endif
    }

	return err;
}

static void hw_iir_ctrl_reg_init(void)
{

   //disable dac iir;
    deq_iir_d_control->codec_dac_L_iir_enable=0;
    deq_iir_d_control->codec_dac_R_iir_enable=0;
}

static void hw_iir_ctrl_reg_open( HW_CODEC_IIR_TYPE_T hw_iir_type)
{
    uint32_t lock;

    LOG_I("%s", __func__);

    if(hw_iir_type==HW_CODEC_IIR_DAC)
    {
        //init swap to use coef0
        deq_iir_d_control->codec_d_iir_coef_swap=0;

        //disable all IIR filters
        deq_iir_d_control->codec_d_iir_enable=0;

	 deq_iir_d_control->codec_d_iir_ch0_bypass=0;
 	 deq_iir_d_control->codec_d_iir_ch1_bypass=0;

         //use all of IIR filters.The max number is 8.
        deq_iir_d_control->codec_d_iir_count_ch0=1;
        deq_iir_d_control->codec_d_iir_count_ch1=1;

	//first iir gain cfg
        deq_gain_iir_coefs0_l0[0].a1=-iir_coef_gain_ramp_l.coef_a[1];
        deq_gain_iir_coefs0_l0[0].a2=-iir_coef_gain_ramp_l.coef_a[2];
        deq_gain_iir_coefs0_l0[0].b0=iir_coef_gain_ramp_l.coef_b[0];
        deq_gain_iir_coefs0_l0[0].b1=iir_coef_gain_ramp_l.coef_b[1];
        deq_gain_iir_coefs0_l0[0].b2=iir_coef_gain_ramp_l.coef_b[2];

        deq_gain_iir_coefs0_r0[0].a1=-iir_coef_gain_ramp_r.coef_a[1];
        deq_gain_iir_coefs0_r0[0].a2=-iir_coef_gain_ramp_r.coef_a[2];
        deq_gain_iir_coefs0_r0[0].b0=iir_coef_gain_ramp_r.coef_b[0];
        deq_gain_iir_coefs0_r0[0].b1=iir_coef_gain_ramp_r.coef_b[1];
        deq_gain_iir_coefs0_r0[0].b2=iir_coef_gain_ramp_r.coef_b[2];

        deq_gain_iir_coefs1_l0[0].a1=-iir_coef_gain_ramp_l.coef_a[1];
        deq_gain_iir_coefs1_l0[0].a2=-iir_coef_gain_ramp_l.coef_a[2];
        deq_gain_iir_coefs1_l0[0].b0=iir_coef_gain_ramp_l.coef_b[0];
        deq_gain_iir_coefs1_l0[0].b1=iir_coef_gain_ramp_l.coef_b[1];
        deq_gain_iir_coefs1_l0[0].b2=iir_coef_gain_ramp_l.coef_b[2];

        deq_gain_iir_coefs1_r0[0].a1=-iir_coef_gain_ramp_r.coef_a[1];
        deq_gain_iir_coefs1_r0[0].a2=-iir_coef_gain_ramp_r.coef_a[2];
        deq_gain_iir_coefs1_r0[0].b0=iir_coef_gain_ramp_r.coef_b[0];
        deq_gain_iir_coefs1_r0[0].b1=iir_coef_gain_ramp_r.coef_b[1];
        deq_gain_iir_coefs1_r0[0].b2=iir_coef_gain_ramp_r.coef_b[2];

        deq_iir_gain_config->codec_iir_d_gaincal_ext_ch0_bypass=0;
        deq_iir_gain_config->codec_iir_d_gaincal_ext_ch1_bypass=0;

        deq_iir_gain_config->codec_iir_d_gainuse_ext_ch0_bypass=0;
        deq_iir_gain_config->codec_iir_d_gainuse_ext_ch1_bypass=0;

        deq_iir_gain_config->codec_iir_d_gain_ext_sel_ch0=0;
        deq_iir_gain_config->codec_iir_d_gain_ext_sel_ch1=0;

        lock = int_lock();
        deq_iir_gain_config->codec_iir_d_gain_ext_update_ch0=0;
        deq_iir_gain_config->codec_iir_d_gain_ext_update_ch1=0;
        *codec_iir_d_gain_ext_ch0=FIXED_GAIN_RAMP_Q;
        *codec_iir_d_gain_ext_ch1=FIXED_GAIN_RAMP_Q;
        hal_sys_timer_delay_us(2);
        deq_iir_gain_config->codec_iir_d_gain_ext_update_ch2=1;
        deq_iir_gain_config->codec_iir_d_gain_ext_update_ch3=1;
        int_unlock(lock);

        //second iir gain cfg
         deq_gain_iir_coefs0_l1[0].a1=-iir_coef_gain_ramp_l.coef_a[1];
         deq_gain_iir_coefs0_l1[0].a2=-iir_coef_gain_ramp_l.coef_a[2];
         deq_gain_iir_coefs0_l1[0].b0=iir_coef_gain_ramp_l.coef_b[0];
         deq_gain_iir_coefs0_l1[0].b1=iir_coef_gain_ramp_l.coef_b[1];
         deq_gain_iir_coefs0_l1[0].b2=iir_coef_gain_ramp_l.coef_b[2];

         deq_gain_iir_coefs0_r1[0].a1=-iir_coef_gain_ramp_r.coef_a[1];
         deq_gain_iir_coefs0_r1[0].a2=-iir_coef_gain_ramp_r.coef_a[2];
         deq_gain_iir_coefs0_r1[0].b0=iir_coef_gain_ramp_r.coef_b[0];
         deq_gain_iir_coefs0_r1[0].b1=iir_coef_gain_ramp_r.coef_b[1];
         deq_gain_iir_coefs0_r1[0].b2=iir_coef_gain_ramp_r.coef_b[2];

         deq_gain_iir_coefs1_l1[0].a1=-iir_coef_gain_ramp_l.coef_a[1];
         deq_gain_iir_coefs1_l1[0].a2=-iir_coef_gain_ramp_l.coef_a[2];
         deq_gain_iir_coefs1_l1[0].b0=iir_coef_gain_ramp_l.coef_b[0];
         deq_gain_iir_coefs1_l1[0].b1=iir_coef_gain_ramp_l.coef_b[1];
         deq_gain_iir_coefs1_l1[0].b2=iir_coef_gain_ramp_l.coef_b[2];

         deq_gain_iir_coefs1_r1[0].a1=-iir_coef_gain_ramp_r.coef_a[1];
         deq_gain_iir_coefs1_r1[0].a2=-iir_coef_gain_ramp_r.coef_a[2];
         deq_gain_iir_coefs1_r1[0].b0=iir_coef_gain_ramp_r.coef_b[0];
         deq_gain_iir_coefs1_r1[0].b1=iir_coef_gain_ramp_r.coef_b[1];
         deq_gain_iir_coefs1_r1[0].b2=iir_coef_gain_ramp_r.coef_b[2];

        deq_iir_gain_config->codec_iir_d_gaincal_ext_ch2_bypass=0;
        deq_iir_gain_config->codec_iir_d_gaincal_ext_ch3_bypass=0;

        deq_iir_gain_config->codec_iir_d_gainuse_ext_ch2_bypass=0;
        deq_iir_gain_config->codec_iir_d_gainuse_ext_ch3_bypass=0;

        deq_iir_gain_config->codec_iir_d_gain_ext_sel_ch2=0;
        deq_iir_gain_config->codec_iir_d_gain_ext_sel_ch3=0;

        lock = int_lock();
        deq_iir_gain_config->codec_iir_d_gain_ext_update_ch2=0;
        deq_iir_gain_config->codec_iir_d_gain_ext_update_ch3=0;
        *codec_iir_d_gain_ext_ch2=FIXED_GAIN_RAMP_Q;
        *codec_iir_d_gain_ext_ch3=FIXED_GAIN_RAMP_Q;
        hal_sys_timer_delay_us(2);
        deq_iir_gain_config->codec_iir_d_gain_ext_update_ch0=1;
        deq_iir_gain_config->codec_iir_d_gain_ext_update_ch1=1;
        int_unlock(lock);


#ifdef DEQ_LIMITER
        deq_limiter_iir_coefs0_l[0].a1=-iir_coef_limiter_attack_l.coef_a[1];
        deq_limiter_iir_coefs0_l[0].a2=-iir_coef_limiter_attack_l.coef_a[2];
        deq_limiter_iir_coefs0_l[0].b0=iir_coef_limiter_attack_l.coef_b[0];
        deq_limiter_iir_coefs0_l[0].b1=iir_coef_limiter_attack_l.coef_b[1];
        deq_limiter_iir_coefs0_l[0].b2=iir_coef_limiter_attack_l.coef_b[2];

        deq_limiter_iir_coefs0_l[1].a1=-iir_coef_limiter_release_l.coef_a[1];
        deq_limiter_iir_coefs0_l[1].a2=-iir_coef_limiter_release_l.coef_a[2];
        deq_limiter_iir_coefs0_l[1].b0=iir_coef_limiter_release_l.coef_b[0];
        deq_limiter_iir_coefs0_l[1].b1=iir_coef_limiter_release_l.coef_b[1];
        deq_limiter_iir_coefs0_l[1].b2=iir_coef_limiter_release_l.coef_b[2];

        deq_limiter_iir_coefs0_r[0].a1=-iir_coef_limiter_attack_r.coef_a[1];
        deq_limiter_iir_coefs0_r[0].a2=-iir_coef_limiter_attack_r.coef_a[2];
        deq_limiter_iir_coefs0_r[0].b0=iir_coef_limiter_attack_r.coef_b[0];
        deq_limiter_iir_coefs0_r[0].b1=iir_coef_limiter_attack_r.coef_b[1];
        deq_limiter_iir_coefs0_r[0].b2=iir_coef_limiter_attack_r.coef_b[2];

        deq_limiter_iir_coefs0_r[1].a1=-iir_coef_limiter_release_r.coef_a[1];
        deq_limiter_iir_coefs0_r[1].a2=-iir_coef_limiter_release_r.coef_a[2];
        deq_limiter_iir_coefs0_r[1].b0=iir_coef_limiter_release_r.coef_b[0];
        deq_limiter_iir_coefs0_r[1].b1=iir_coef_limiter_release_r.coef_b[1];
        deq_limiter_iir_coefs0_r[1].b2=iir_coef_limiter_release_r.coef_b[2];

        deq_limiter_iir_coefs1_l[0].a1=-iir_coef_limiter_attack_l.coef_a[1];
        deq_limiter_iir_coefs1_l[0].a2=-iir_coef_limiter_attack_l.coef_a[2];
        deq_limiter_iir_coefs1_l[0].b0=iir_coef_limiter_attack_l.coef_b[0];
        deq_limiter_iir_coefs1_l[0].b1=iir_coef_limiter_attack_l.coef_b[1];
        deq_limiter_iir_coefs1_l[0].b2=iir_coef_limiter_attack_l.coef_b[2];

        deq_limiter_iir_coefs1_l[1].a1=-iir_coef_limiter_release_l.coef_a[1];
        deq_limiter_iir_coefs1_l[1].a2=-iir_coef_limiter_release_l.coef_a[2];
        deq_limiter_iir_coefs1_l[1].b0=iir_coef_limiter_release_l.coef_b[0];
        deq_limiter_iir_coefs1_l[1].b1=iir_coef_limiter_release_l.coef_b[1];
        deq_limiter_iir_coefs1_l[1].b2=iir_coef_limiter_release_l.coef_b[2];

        deq_limiter_iir_coefs1_r[0].a1=-iir_coef_limiter_attack_r.coef_a[1];
        deq_limiter_iir_coefs1_r[0].a2=-iir_coef_limiter_attack_r.coef_a[2];
        deq_limiter_iir_coefs1_r[0].b0=iir_coef_limiter_attack_r.coef_b[0];
        deq_limiter_iir_coefs1_r[0].b1=iir_coef_limiter_attack_r.coef_b[1];
        deq_limiter_iir_coefs1_r[0].b2=iir_coef_limiter_attack_r.coef_b[2];

        deq_limiter_iir_coefs1_r[1].a1=-iir_coef_limiter_release_r.coef_a[1];
        deq_limiter_iir_coefs1_r[1].a2=-iir_coef_limiter_release_r.coef_a[2];
        deq_limiter_iir_coefs1_r[1].b0=iir_coef_limiter_release_r.coef_b[0];
        deq_limiter_iir_coefs1_r[1].b1=iir_coef_limiter_release_r.coef_b[1];
        deq_limiter_iir_coefs1_r[1].b2=iir_coef_limiter_release_r.coef_b[2];


        deq_iir_limiter_config->codec_iir_d_lmt_th_update_ch0=0;
        deq_iir_limiter_config->codec_iir_d_lmt_th_update_ch1=0;

        //limiter threshold, 0: 0x800000,0db; 1: 0x400000, -6db; 2: -12db;�� 7: 0x10000, -42db
        deq_iir_limiter_config->codec_iir_d_lmt_th_ch0=0;
        deq_iir_limiter_config->codec_iir_d_lmt_th_ch1=0;

        deq_iir_limiter_config->codec_iir_d_lmt_th_update_ch0=1;
        deq_iir_limiter_config->codec_iir_d_lmt_th_update_ch1=1;

        deq_iir_limiter_config->codec_iir_d_lmt_ch0_bypass=0;
        deq_iir_limiter_config->codec_iir_d_lmt_ch1_bypass=0;

        deq_iir_limiter_config->codec_iir_d_lmt_delay=DEQ_LIMITER_DELAY;

        //limiter release rate, 0:0x80=128; 1:0x100=256; 2:0x200;......7:0x4000
        deq_iir_limiter_config->codec_iir_d_lmt_rls_rate=7;

/*
        switch(ANC_LIMITER_RATE)
        {
            case 16384:
                anc_gain_and_limiter_config->codec_iir_a_lmt_rls_rate=7;
                break;
            case 8192:
                anc_gain_and_limiter_config->codec_iir_a_lmt_rls_rate=6;
                break;
            case 4096:
                anc_gain_and_limiter_config->codec_iir_a_lmt_rls_rate=5;
                break;
            case 2048:
                anc_gain_and_limiter_config->codec_iir_a_lmt_rls_rate=4;
                break;
            case 1024:
                anc_gain_and_limiter_config->codec_iir_a_lmt_rls_rate=3;
                break;
            case 512:
                anc_gain_and_limiter_config->codec_iir_a_lmt_rls_rate=2;
                break;
            case 256:
                anc_gain_and_limiter_config->codec_iir_a_lmt_rls_rate=1;
                break;
            case 128:
                anc_gain_and_limiter_config->codec_iir_a_lmt_rls_rate=0;
                break;
            default:
                ASSERT(false, "ERROR ANC_LIMITER_RATE");
                anc_gain_and_limiter_config->codec_iir_a_lmt_rls_rate=0;
                break;
        }
    */
#endif

    }
}


static void hw_iir_ctrl_reg_close(HW_CODEC_IIR_TYPE_T hw_iir_type)
{

    if(hw_iir_type==HW_CODEC_IIR_DAC)
    {
       //disable dac iir;
        //deq_iir_d_control->codec_dac_L_iir_enable=0;
        deq_iir_d_control->codec_dac_L_iir_enable=0;
        deq_iir_d_control->codec_dac_R_iir_enable=0;
    }

}

int hw_codec_iir_open(enum AUD_SAMPRATE_T sample_rate,  HW_CODEC_IIR_TYPE_T hw_iir_type,int32_t ch_map)
{
    HW_CODEC_IIR_ERROR err=HW_CODEC_IIR_NO_ERR;

    LOG_I("%s", __func__);

    if(dac_open_flag==0)
    {
        hal_cmu_codec_iir_enable(104000000);
        hw_iir_ctrl_reg_init();

        iir_coef_using=0;
        sample_rate_dac=AUD_SAMPRATE_NULL;
    }

    if(hw_iir_type==HW_CODEC_IIR_DAC)
    {
        dac_iir_reset_flag=0;
        dac_open_flag=1;
        sample_rate_dac=sample_rate;

        //clear iir_counter;
        dac_filtes_l_old.iir_counter=0;
        dac_filtes_r_old.iir_counter=0;
        dac_filtes_l_old.iir_bypass_flag=1;
        dac_filtes_r_old.iir_bypass_flag=1;
    }

    hw_iir_ctrl_reg_open(hw_iir_type);

    return err;
}


int hw_codec_iir_close( HW_CODEC_IIR_TYPE_T hw_iir_type)
{
    HW_CODEC_IIR_ERROR err=HW_CODEC_IIR_NO_ERR;

    LOG_I("%s,%d", __func__,hw_iir_type);

    if(hw_iir_type==HW_CODEC_IIR_DAC)
    {
        dac_open_flag=0;
        sample_rate_dac=AUD_SAMPRATE_NULL;
    }

    hw_iir_ctrl_reg_close(hw_iir_type);

    if(dac_open_flag==0)
    {
        hal_cmu_codec_iir_disable();
    }

    return err;
}


int hw_codec_iir_set_coefs(HW_CODEC_IIR_CFG_F *cfg, HW_CODEC_IIR_TYPE_T hw_iir_type)
{
    HW_CODEC_IIR_ERROR err=HW_CODEC_IIR_OTHER_ERR;

    HW_CODEC_IIR_CFG_T *hw_iir_cfg = hw_codec_iir_convert_cfg(cfg);

    err = hw_codec_iir_set_cfg(hw_iir_cfg, AUD_SAMPRATE_NULL, hw_iir_type);

    return err;
}



