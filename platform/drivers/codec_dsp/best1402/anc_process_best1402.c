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
#include "cmsis.h"
#include "hal_dma.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_cmu.h"
#include "hal_codec.h"
#include "analog.h"
#include "anc_process.h"
#include "hwtimer_list.h"

/**********************************mocro**************************************/

typedef short int16;
typedef unsigned short uint16;
typedef int   int32;
typedef unsigned int   uint32;

/*
1	cc	codec_iir_a_enable	RW	0	0
1	cc	codec_iir_a_ch0_bypass	RW	0	1
1	cc	codec_iir_a_ch1_bypass	RW	0	2
4	cc	codec_iir_a_count_ch0	RW	0	3
4	cc	codec_iir_a_count_ch1	RW	0	7
1	cc	codec_iir_a_coef_swap	RW	0	11
1	cc	codec_iir_a_coef_swap_status	R	0	12
13	cc		x
*/
struct _anc_iir_control
{
	uint32 codec_iir_a_enable : 1;

	uint32 codec_iir_a_ch0_bypass : 1;
	uint32 codec_iir_a_ch1_bypass : 1;

	uint32 codec_iir_a_count_ch0 : 4;
	uint32 codec_iir_a_count_ch1 : 4;

       uint32 codec_iir_a_coef_swap : 1;
       uint32 codec_iir_a_coef_swap_status : 1;

	uint32 reserved : 19;
};

/*
1	190	codec_anc_enable_ch0	RW	0	0	anc enable
1	190	codec_anc_enable_ch1	RW	0	1
1	190	codec_anc_mute_ch0	RW	0	2	anc output to 0
1	190	codec_anc_mute_ch1	RW	0	3
1	190	codec_anc_rate_sel	RW	0	4	  0: 384k anc;  1: 768k anc;
12	190	codec_anc_mute_gain_ff_ch0	RW	0	5	Format 3.9, for FF
12	190	codec_anc_mute_gain_ff_ch1	RW	0	17
1	190	codec_anc_mute_gain_pass0_ff_ch0	RW	0	29	1: gain update no pass0
1	190	codec_anc_mute_gain_pass0_ff_ch1	RW	0	30
31	190		x
*/
struct _anc_control
{
	uint32 codec_anc_enable_ch0 : 1;
	uint32 codec_anc_enable_ch1 : 1;

	uint32 codec_anc_mute_ch0 : 1;
	uint32 codec_anc_mute_ch1 : 1;

	uint32 codec_anc_rate_sel : 1;

	int32 codec_anc_mute_gain_ff_ch0 : 12;
	int32 codec_anc_mute_gain_ff_ch1 : 12;

	int32 codec_anc_mute_gain_pass0_ff_ch0 : 1;
	int32 codec_anc_mute_gain_pass0_ff_ch1 : 1;

       uint32 Reserved : 1;
};



/*
1	200	codec_iir_a_gaincal_ext_ch0_bypass	RW	1	0	 =1, bypass extern gain ramp calc for iir ch0
1	200	codec_iir_a_gaincal_ext_ch1_bypass	RW	1	1	 =1, bypass extern gain ramp calc for iir ch1
1	200	codec_iir_a_gainuse_ext_ch0_bypass	RW	1	2	 =1, bypass, not use extern gain for iir ch0
1	200	codec_iir_a_gainuse_ext_ch1_bypass	RW	1	3	 =1, bypass, not use extern gain for iir ch1
1	200	codec_iir_a_lmt_ch0_bypass	RW	1	4	 =1, bypass limiter for iir ch0
1	200	codec_iir_a_lmt_ch1_bypass	RW	1	5	 =1, bypass limiter for iir ch1
1	200	codec_iir_a_gain_ext_update_ch0	RW	0	6	  posedge to update extern gain for iir ch0
1	200	codec_iir_a_gain_ext_update_ch1	RW	0	7	  posedge to update extern gain for iir ch0
1	200	codec_iir_a_gain_ext_sel_ch0	RW	0	8	 =1, extern gain updated when pass0; =0, updated right away
1	200	codec_iir_a_gain_ext_sel_ch1	RW	0	9	 =1, extern gain updated when pass0; =0, updated right away
3	200	codec_iir_a_lmt_th_ch0	RW	0	10	limiter threshold, 0: 0x800000,0db; 1: 0x400000, -6db; 2: -12db;�� 7: 0x10000, -42db
3	200	codec_iir_a_lmt_th_ch1	RW	0	13	limiter threshold, 0: 0x800000,0db; 1: 0x400000, -6db; 2: -12db;�� 7: 0x10000, -42db
1	200	codec_iir_a_lmt_th_update_ch0	RW	0	16	threshold update, high level to update
1	200	codec_iir_a_lmt_th_update_ch1	RW	0	17	threshold update, high level to update
1	200	codec_anc_mute_gain_update_ff_ch0	RW	0	18	ff anc gain update, high level to update
1	200	codec_anc_mute_gain_update_ff_ch1	RW	0	19	ff anc gain update, high level to update
7	200	codec_iir_a_lmt_delay	RW	0	20	limiter delay for attack time
3	200	codec_iir_a_lmt_rls_rate	RW	0	27	limiter release rate, 0: 0x80=128; 1: 0x100=256; 2: 0x200;�� 7: 0x4000;
30	200		x

32	204	codec_iir_a_gain_ext_ch0	RW	0	0	extern gain for iir_ch0, S5.27
32	204		x

32	208	codec_iir_a_gain_ext_ch1	RW	0	0	extern gain for iir_ch1, S5.27
32	208		x
*/
struct _anc_gain_and_limiter_config
{
	uint32 codec_iir_a_gaincal_ext_ch0_bypass : 1;
	uint32 codec_iir_a_gaincal_ext_ch1_bypass : 1;

	uint32 codec_iir_a_gainuse_ext_ch0_bypass : 1;
	uint32 codec_iir_a_gainuse_ext_ch1_bypass : 1;

	uint32 codec_iir_a_lmt_ch0_bypass : 1;
	uint32 codec_iir_a_lmt_ch1_bypass : 1;

	uint32 codec_iir_a_gain_ext_update_ch0 : 1;
	uint32 codec_iir_a_gain_ext_update_ch1 : 1;

	uint32 codec_iir_a_gain_ext_sel_ch0 : 1;
	uint32 codec_iir_a_gain_ext_sel_ch1 : 1;

	uint32 codec_iir_a_lmt_th_ch0 : 3;
	uint32 codec_iir_a_lmt_th_ch1 : 3;

	uint32 codec_iir_a_lmt_th_update_ch0 : 1;
	uint32 codec_iir_a_lmt_th_update_ch1 : 1;

	int32 codec_anc_mute_gain_update_ff_ch0 : 1;
	int32 codec_anc_mute_gain_update_ff_ch1 : 1;

	uint32 codec_iir_a_lmt_delay : 7;
	uint32 codec_iir_a_lmt_rls_rate : 3;

       uint32 Reserved : 2;
};

/*
iir coefficients(b/a Q27)
a1
a2
b1
b2
b0
*/
struct _anc_iir_coefs
{
 	int32 a1 ;
 	int32 a2 ;

 	int32 b1 ;
 	int32 b2 ;
	int32 b0 ;
};

#ifdef ANC_FB_CHECK
/*
1	194	codec_anc_check_enable_ch0	RW	0	0
2	194	codec_anc_check_acc_sample_rate_ch0	RW	0	1
12	194	codec_anc_check_acc_window_ch0	RW	0	3
10	194	codec_anc_check_trig_window_ch0	RW	0	15
1	194	codec_anc_check_keep_ch0	RW	0	25
26	194		x
*/
struct _anc_check_ch0_config
{
	uint32 codec_anc_check_enable_ch0 : 1;
	uint32 codec_anc_check_acc_sample_rate_ch0 : 2;
	uint32 codec_anc_check_acc_window_ch0 : 12;
   	uint32 codec_anc_check_trig_window_ch0 : 10;
       uint32 codec_anc_check_keep_sel_ch0: 1;
       uint32 Reserved : 6;
};

/*
1	198	codec_anc_check_enable_ch1	RW	0	0
2	198	codec_anc_check_acc_sample_rate_ch1	RW	0	1
12	198	codec_anc_check_acc_window_ch1	RW	0	3
10	198	codec_anc_check_trig_window_ch1	RW	0	15
1	198	codec_anc_check_keep_ch1	RW	0	25
26	198		x
*/

struct _anc_check_ch1_config
{
	uint32 codec_anc_check_enable_ch1 : 1;
	uint32 codec_anc_check_acc_sample_rate_ch1 : 2;
	uint32 codec_anc_check_acc_window_ch1 : 12;
   	uint32 codec_anc_check_trig_window_ch1 : 10;
       uint32 codec_anc_check_keep_sel_ch1: 1;
       uint32 Reserved : 6;
};

/*
3	0c	codec_rx_overflow	RW	0	0
3	0c	codec_rx_underflow	RW	0	3
1	0c	codec_tx_overflow	RW	0	6
1	0c	codec_tx_underflow	RW	0	7
1	0c	anc_check_error_trig_ch0	RW	0	8
1	0c	anc_check_error_trig_ch1	RW	0	9
1	0c	event_trigger	RW	0	10
1	0c	adc_max_overflow	RW	0	11
1	0c	time_trigger	RW	0	12
13	0c		x
*/
struct _codec_int_config
{
	uint32 Reserved_notused : 8;

	uint32 anc_check_error_trig_ch0 : 1;
	uint32 anc_check_error_trig_ch1 : 1;

    uint32 Reserved : 22;
};
/*
3	10	codec_rx_overflow_mask	RW	0	0
3	10	codec_rx_underflow_mask	RW	0	3
1	10	codec_tx_overflow_mask	RW	0	6
1	10	codec_tx_underflow_mask	RW	0	7
1	10	anc_check_error_trig_ch0_mask	RW	0	8
1	10	anc_check_error_trig_ch1_mask	RW	0	9
1	10	event_trigger_mask	RW	0	10
1	10	adc_max_overflow_mask	RW	0	11
1	10	time_trigger_mask	RW	0	12
13	10		x
*/
struct _codec_mask_config
{
	uint32 Reserved_notused : 8;

	uint32 anc_check_error_trig_ch0_mask : 1;
	uint32 anc_check_error_trig_ch1_mask : 1;

    uint32 Reserved : 22;
};
#endif

#define IIR_COUNTER (8)

typedef struct _iir_parameter
{
    int32_t total_gain;

    uint16_t iir_bypass_flag;
    uint16_t iir_counter;

    anc_iir_coefs iir_coef[IIR_COUNTER];

} iir_parameter;


#define GAIN_Q (9)
#define ANC_GAIN_RAMP
//#define ANC_LIMITER

#ifdef ANC_GAIN_RAMP

#define FIXED_GAIN_RAMP_Q (1<<27)
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
#endif

#ifdef ANC_LIMITER

#define ANC_LIMITER_DELAY (30)

//#define ANC_LIMITER_RATE (16384)

//#define ANC_LIMITER_ATT_FLOAT ((1.0f/9.0f)^(1.0f/ANC_LIMITER_DELAY))

//#define ANC_LIMITER_ATT_A1 (int32)(ANC_LIMITER_ATT_FLOAT*(2^27))
//#define ANC_LIMITER_ATT_B1 (int32)((1.0f-ANC_LIMITER_ATT_FLOAT)*(2^27))

//#define ANC_LIMITER_RELEASE_B1 (ANC_LIMITER_ATT_B1/ANC_LIMITER_RATE)
//#define ANC_LIMITER_RELEASE_A1 (2^27-ANC_LIMITER_RELEASE_B1)

#if ANC_LIMITER_DELAY==60

#define ANC_LIMITER_ATT_A1 (129391528)
#define ANC_LIMITER_ATT_B1 (4826200)

#define ANC_LIMITER_RELEASE_B1 (295)
#define ANC_LIMITER_RELEASE_A1 (134217433)

#elif ANC_LIMITER_DELAY==45

#define ANC_LIMITER_ATT_A1 (127821672)
#define ANC_LIMITER_ATT_B1 (6396056)

#define ANC_LIMITER_RELEASE_B1 (390)
#define ANC_LIMITER_RELEASE_A1 (134217338)

#elif ANC_LIMITER_DELAY==30

#define ANC_LIMITER_ATT_A1 (124738868)
#define ANC_LIMITER_ATT_B1 (9478860)

#define ANC_LIMITER_RELEASE_B1 (579)
#define ANC_LIMITER_RELEASE_A1 (134217149)

#elif ANC_LIMITER_DELAY==15

#define ANC_LIMITER_ATT_A1 (115929434)
#define ANC_LIMITER_ATT_B1 (18288294)

#define ANC_LIMITER_RELEASE_B1 (1116)
#define ANC_LIMITER_RELEASE_A1 (134216612)

#elif ANC_LIMITER_DELAY==8

#define ANC_LIMITER_ATT_A1 (101983419)
#define ANC_LIMITER_ATT_B1 (32234309)

#define ANC_LIMITER_RELEASE_B1 (1967)
#define ANC_LIMITER_RELEASE_A1 (134215761)

#else
//#define ANC_LIMITER_ATT_A1 (0)
//#define ANC_LIMITER_ATT_B1 (0)

//#define ANC_LIMITER_RELEASE_B1 (0)
//#define ANC_LIMITER_RELEASE_A1 (0)

#error "ERROR ANC_LIMITER_DELAY"
#endif



static const anc_iir_coefs   iir_coef_limiter_attack_l=
{
    .coef_b={0,    ANC_LIMITER_ATT_B1,     0},
    .coef_a={134217728,    -ANC_LIMITER_ATT_A1,     0},
};
static const anc_iir_coefs   iir_coef_limiter_attack_r=
{
    .coef_b={0,    ANC_LIMITER_ATT_B1,     0},
    .coef_a={134217728,    -ANC_LIMITER_ATT_A1,     0},
};

static const anc_iir_coefs   iir_coef_limiter_release_l=
{
    .coef_b={0,    ANC_LIMITER_RELEASE_B1,     0},
    .coef_a={134217728,    -ANC_LIMITER_RELEASE_A1,     0},
};
static const anc_iir_coefs   iir_coef_limiter_release_r=
{
    .coef_b={0,    ANC_LIMITER_RELEASE_B1,     0},
    .coef_a={134217728,    -ANC_LIMITER_RELEASE_A1,     0},
};
#endif



/*******************************data struction***********************************/

#define ANC_BASE                            ((uint32)CODEC_BASE)

//ANC registers
volatile static struct _anc_control *anc_control=(volatile struct _anc_control *)(ANC_BASE+0x190);

//IIR registers
volatile static struct _anc_iir_control *anc_iir_control=(volatile struct _anc_iir_control *)(ANC_BASE+0xcc);
//gain and limiter

volatile static struct _anc_gain_and_limiter_config *anc_gain_and_limiter_config=(volatile struct _anc_gain_and_limiter_config *)(ANC_BASE+0x200);

volatile static struct _anc_iir_coefs *anc_ff_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x3000);
volatile static struct _anc_iir_coefs *anc_ff_iir_coefs0_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x30a0);
volatile static struct _anc_iir_coefs *anc_ff_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x3200);
volatile static struct _anc_iir_coefs *anc_ff_iir_coefs1_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x32a0);

#ifdef ANC_GAIN_RAMP
volatile static struct _anc_iir_coefs *anc_gain_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x3140);
volatile static struct _anc_iir_coefs *anc_gain_iir_coefs0_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x3154);
volatile static struct _anc_iir_coefs *anc_gain_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x3340);
volatile static struct _anc_iir_coefs *anc_gain_iir_coefs1_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x3354);

volatile static int32 *codec_iir_a_gain_ext_ch0=(volatile int32 *)(ANC_BASE+0x0204);
volatile static int32 *codec_iir_a_gain_ext_ch1=(volatile int32 *)(ANC_BASE+0x0208);
#endif

#ifdef ANC_LIMITER
volatile static struct _anc_iir_coefs *anc_limiter_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x3168);
volatile static struct _anc_iir_coefs *anc_limiter_iir_coefs0_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x3190);
volatile static struct _anc_iir_coefs *anc_limiter_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x3368);
volatile static struct _anc_iir_coefs *anc_limiter_iir_coefs1_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x3390);
#endif

#ifdef ANC_FB_CHECK
volatile static struct _anc_check_ch0_config *anc_check_ch0_config=(volatile struct _anc_check_ch0_config *)(ANC_BASE+0x0194);
volatile static struct _anc_check_ch1_config *anc_check_ch1_config=(volatile struct _anc_check_ch1_config *)(ANC_BASE+0x0198);

volatile static uint32 *codec_anc_check_threshold_ch0=(volatile uint32 *)(ANC_BASE+0x019c);
volatile static uint32 *codec_anc_check_threshold_ch1=(volatile uint32 *)(ANC_BASE+0x01a0);

volatile static uint32 *codec_int_config_clr=(volatile uint32 *)(ANC_BASE+0x0c);
volatile static struct _codec_mask_config *codec_mask_config=(volatile struct _codec_mask_config *)(ANC_BASE+0x10);

#define CODEC_FB_CHECK_ERROR_TRIG_CH0            (1 << 23)
#define CODEC_FB_CHECK_ERROR_TRIG_CH1            (1 << 24)

#endif

volatile static int max_ff_gain_l=0;
volatile static int max_ff_gain_r=0;

#ifdef ANC_GAIN_RAMP
volatile static int ff_ramp_gain_l=0;
volatile static int ff_ramp_gain_r=0;
#endif

volatile static int ff_open_flag=0;

#ifdef ANC_FB_CHECK
static HWTIMER_ID anc_adc_check_dev_timer=NULL;
#endif

static iir_parameter ff_filtes_l_old;
static iir_parameter ff_filtes_r_old;

volatile static int8_t ff_iir_reset_flag;
volatile static int8_t iir_coef_using;

/*******************************FIR hardware filter*********************************/
static void anc_cfg_lock(void)
{
	return;
}

static void anc_cfg_unlock(void)
{
	return;
}

/**
 * @brief      Update coef and store gain,
 * 				fadein: enable: use anc_set_gain(), disable: anc_apply_max_gain() to set gain.
 *
 * @param[in]  cfg   coef and gain
 */
int anc_set_cfg(const struct_anc_cfg * cfg,enum ANC_TYPE_T anc_type,ANC_GAIN_TIME anc_gain_delay)
{
    ANC_ERROR err=ANC_NO_ERR;
    uint32_t lock;
    int i;
    const aud_item *ff_filtes_l;
    const aud_item *ff_filtes_r;

    int total_iir_counter=0;

    if (cfg == NULL)
    {
        LOG_I("%s: cfg is null", __func__);
        return ANC_OTHER_ERR;
    }

    if (anc_type==ANC_FEEDFORWARD && ff_open_flag==0) {
        LOG_I("%s: anc_type=%d not opened", __func__, anc_type);
        return ANC_OTHER_ERR;
    }

    LOG_I("%s: iir_coef_using:%d,Iir_coef_swap:%d", __func__,iir_coef_using,anc_iir_control->codec_iir_a_coef_swap);

    ff_filtes_l = &(cfg->anc_cfg_ff_l);
    ff_filtes_r = &(cfg->anc_cfg_ff_r);

//    LOG_I("ff_l:%d,ff_r:%d,fb_l:%d,fb_r:%d",ff_filtes_l->total_gain,ff_filtes_r->total_gain,fb_filtes_l->total_gain,fb_filtes_r->total_gain);

/*
	LOG_I("FEEDFORWARD,L:gain %d,R:gain %d",ff_filtes_l->total_gain, ff_filtes_r->total_gain);
    LOG_I("FEEDFORWARD,L:iir_counter %d,R:iir_counter %d",ff_filtes_l->iir_counter, ff_filtes_r->iir_counter);
	for(int j = 0; j <IIR_COUNTER; j++)
	{
		//LOG_I("iir coef ff l 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x",
		LOG_I("iir coef ff l %10d, %10d, %10d, %10d, %10d, %10d",  \
		ff_filtes_l->iir_coef[j].coef_b[0], \
		ff_filtes_l->iir_coef[j].coef_b[1], \
		ff_filtes_l->iir_coef[j].coef_b[2], \
		ff_filtes_l->iir_coef[j].coef_a[0], \
		ff_filtes_l->iir_coef[j].coef_a[1], \
		ff_filtes_l->iir_coef[j].coef_a[2]);
	}
	for(int j = 0; j <IIR_COUNTER; j++)
	{
		//LOG_I("iir coef ff r 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x",
		LOG_I("iir coef ff r %10d, %10d, %10d, %10d, %10d, %10d",  \
		ff_filtes_r->iir_coef[j].coef_b[0], \
		ff_filtes_r->iir_coef[j].coef_b[1], \
		ff_filtes_r->iir_coef[j].coef_b[2], \
		ff_filtes_r->iir_coef[j].coef_a[0], \
		ff_filtes_r->iir_coef[j].coef_a[1], \
		ff_filtes_r->iir_coef[j].coef_a[2]);
	}

	*/

    if(anc_type==ANC_FEEDFORWARD)
    {
        if(ff_filtes_l_old.iir_counter==ff_filtes_l->iir_counter
            &&ff_filtes_l_old.iir_bypass_flag==ff_filtes_l->iir_bypass_flag
            &&ff_filtes_l->iir_bypass_flag==0

            &&ff_filtes_r_old.iir_counter==ff_filtes_r->iir_counter
            &&ff_filtes_r_old.iir_bypass_flag==ff_filtes_r->iir_bypass_flag
            &&ff_filtes_r->iir_bypass_flag==0

            &&anc_gain_delay==ANC_GAIN_NO_DELAY)
        {
            LOG_I("%s: switching ff", __func__);

            if(iir_coef_using==0)
            {
                for(i=0;i<IIR_COUNTER;i++)
                {
                    anc_ff_iir_coefs1_l[i].a1=-ff_filtes_l->iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs1_l[i].a2=-ff_filtes_l->iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs1_l[i].b0=ff_filtes_l->iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs1_l[i].b1=ff_filtes_l->iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs1_l[i].b2=ff_filtes_l->iir_coef[i].coef_b[2];

                    anc_ff_iir_coefs1_r[i].a1=-ff_filtes_r->iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs1_r[i].a2=-ff_filtes_r->iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs1_r[i].b0=ff_filtes_r->iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs1_r[i].b1=ff_filtes_r->iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs1_r[i].b2=ff_filtes_r->iir_coef[i].coef_b[2];

                    ff_filtes_l_old.iir_coef[i]=ff_filtes_l->iir_coef[i];
                    ff_filtes_r_old.iir_coef[i]=ff_filtes_r->iir_coef[i];
                }

                ff_filtes_l_old.total_gain=ff_filtes_l->total_gain;
                ff_filtes_r_old.total_gain=ff_filtes_r->total_gain;

                anc_ff_iir_coefs1_l[0].b0=(int32)((ff_filtes_l->iir_coef[0].coef_b[0]*(int64_t)ff_filtes_l->total_gain)>>GAIN_Q);
                anc_ff_iir_coefs1_l[0].b1=(int32)((ff_filtes_l->iir_coef[0].coef_b[1]*(int64_t)ff_filtes_l->total_gain)>>GAIN_Q);
                anc_ff_iir_coefs1_l[0].b2=(int32)((ff_filtes_l->iir_coef[0].coef_b[2]*(int64_t)ff_filtes_l->total_gain)>>GAIN_Q);

                anc_ff_iir_coefs1_r[0].b0=(int32)((ff_filtes_r->iir_coef[0].coef_b[0]*(int64_t)ff_filtes_r->total_gain)>>GAIN_Q);
                anc_ff_iir_coefs1_r[0].b1=(int32)((ff_filtes_r->iir_coef[0].coef_b[1]*(int64_t)ff_filtes_r->total_gain)>>GAIN_Q);
                anc_ff_iir_coefs1_r[0].b2=(int32)((ff_filtes_r->iir_coef[0].coef_b[2]*(int64_t)ff_filtes_r->total_gain)>>GAIN_Q);

                anc_iir_control->codec_iir_a_coef_swap=1;
                while(1)
                {
                    if(anc_iir_control->codec_iir_a_coef_swap_status==1) break;
                }

            }
            else
            {

                for(i=0;i<IIR_COUNTER;i++)
                {
                    anc_ff_iir_coefs0_l[i].a1=-ff_filtes_l->iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs0_l[i].a2=-ff_filtes_l->iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs0_l[i].b0=ff_filtes_l->iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs0_l[i].b1=ff_filtes_l->iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs0_l[i].b2=ff_filtes_l->iir_coef[i].coef_b[2];

                    anc_ff_iir_coefs0_r[i].a1=-ff_filtes_r->iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs0_r[i].a2=-ff_filtes_r->iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs0_r[i].b0=ff_filtes_r->iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs0_r[i].b1=ff_filtes_r->iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs0_r[i].b2=ff_filtes_r->iir_coef[i].coef_b[2];

                    ff_filtes_l_old.iir_coef[i]=ff_filtes_l->iir_coef[i];
                    ff_filtes_r_old.iir_coef[i]=ff_filtes_r->iir_coef[i];

                }

                ff_filtes_l_old.total_gain=ff_filtes_l->total_gain;
                ff_filtes_r_old.total_gain=ff_filtes_r->total_gain;

                anc_ff_iir_coefs0_l[0].b0=(int32)((ff_filtes_l->iir_coef[0].coef_b[0]*(int64_t)ff_filtes_l->total_gain)>>GAIN_Q);
                anc_ff_iir_coefs0_l[0].b1=(int32)((ff_filtes_l->iir_coef[0].coef_b[1]*(int64_t)ff_filtes_l->total_gain)>>GAIN_Q);
                anc_ff_iir_coefs0_l[0].b2=(int32)((ff_filtes_l->iir_coef[0].coef_b[2]*(int64_t)ff_filtes_l->total_gain)>>GAIN_Q);

                anc_ff_iir_coefs0_r[0].b0=(int32)((ff_filtes_r->iir_coef[0].coef_b[0]*(int64_t)ff_filtes_r->total_gain)>>GAIN_Q);
                anc_ff_iir_coefs0_r[0].b1=(int32)((ff_filtes_r->iir_coef[0].coef_b[1]*(int64_t)ff_filtes_r->total_gain)>>GAIN_Q);
                anc_ff_iir_coefs0_r[0].b2=(int32)((ff_filtes_r->iir_coef[0].coef_b[2]*(int64_t)ff_filtes_r->total_gain)>>GAIN_Q);

                anc_iir_control->codec_iir_a_coef_swap=0;
                while(1)
                {
                    if(anc_iir_control->codec_iir_a_coef_swap_status==0) break;
                }

            }

            ff_iir_reset_flag=0;
            iir_coef_using=1-iir_coef_using;
        }
        else
        {
            LOG_I("%s: no switching ff", __func__);

            //disable all IIR filter before updata the coefs;
            anc_iir_control->codec_iir_a_enable=0;

            ff_iir_reset_flag=1;
        }
    }


    //feedforward ANC settings
    if(anc_type==ANC_FEEDFORWARD&&ff_iir_reset_flag==1)
    {
        for(i=0;i<IIR_COUNTER;i++)
        {
            ff_filtes_l_old.iir_coef[i]=ff_filtes_l->iir_coef[i];
            ff_filtes_r_old.iir_coef[i]=ff_filtes_r->iir_coef[i];
         }

        ff_filtes_l_old.total_gain=ff_filtes_l->total_gain;
        ff_filtes_r_old.total_gain=ff_filtes_r->total_gain;

        ff_filtes_l_old.iir_counter=ff_filtes_l->iir_counter;
        ff_filtes_r_old.iir_counter=ff_filtes_r->iir_counter;

        ff_filtes_l_old.iir_bypass_flag=ff_filtes_l->iir_bypass_flag;
        ff_filtes_r_old.iir_bypass_flag=ff_filtes_r->iir_bypass_flag;

        LOG_I("%s: resetting ff", __func__);

        max_ff_gain_l=(1<<GAIN_Q);
        max_ff_gain_r=(1<<GAIN_Q);

        //Set the FF gain;

#ifdef ANC_GAIN_RAMP
        lock = int_lock();
        anc_gain_and_limiter_config->codec_iir_a_gain_ext_update_ch0=0;
        anc_gain_and_limiter_config->codec_iir_a_gain_ext_update_ch1=0;
         *codec_iir_a_gain_ext_ch0=0;
         *codec_iir_a_gain_ext_ch1=0;
         hal_sys_timer_delay_us(1);
        anc_gain_and_limiter_config->codec_iir_a_gain_ext_update_ch0=1;
        anc_gain_and_limiter_config->codec_iir_a_gain_ext_update_ch1=1;
        int_unlock(lock);
#else
        lock = int_lock();
        anc_gain_and_limiter_config->codec_anc_mute_gain_update_ff_ch0=0;
        anc_gain_and_limiter_config->codec_anc_mute_gain_update_ff_ch1=0;
        anc_control->codec_anc_mute_gain_ff_ch0=0;
        anc_control->codec_anc_mute_gain_ff_ch1=0;
        hal_sys_timer_delay_us(1);
        anc_gain_and_limiter_config->codec_anc_mute_gain_update_ff_ch0=1;
        anc_gain_and_limiter_config->codec_anc_mute_gain_update_ff_ch1=1;
        int_unlock(lock);
#endif

        //setting the feedforward IIR filters;
        if(ff_filtes_l->iir_bypass_flag==0)
        {
            anc_iir_control->codec_iir_a_ch0_bypass=0;

            if(ff_filtes_l->iir_counter>IIR_COUNTER)
            {
                anc_iir_control->codec_iir_a_count_ch0=IIR_COUNTER;
            }
            else if(ff_filtes_l->iir_counter>0)
            {
                anc_iir_control->codec_iir_a_count_ch0=ff_filtes_l->iir_counter;
            }
            else
            {
                anc_iir_control->codec_iir_a_ch0_bypass=1;
                LOG_I("Error ff_filtes_l IIR counter:%d", ff_filtes_l->iir_counter);
                err=ANC_TYPE_ERR;
            }

            if(iir_coef_using==0)
            {
                //feedforward left ch iir coefs settings
                for(i=0;i<IIR_COUNTER;i++)
                {
                    anc_ff_iir_coefs0_l[i].a1=-ff_filtes_l->iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs0_l[i].a2=-ff_filtes_l->iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs0_l[i].b0=ff_filtes_l->iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs0_l[i].b1=ff_filtes_l->iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs0_l[i].b2=ff_filtes_l->iir_coef[i].coef_b[2];
                }
                anc_ff_iir_coefs0_l[0].b0=(int32)((ff_filtes_l->iir_coef[0].coef_b[0]*(int64_t)ff_filtes_l->total_gain)>>GAIN_Q);
                anc_ff_iir_coefs0_l[0].b1=(int32)((ff_filtes_l->iir_coef[0].coef_b[1]*(int64_t)ff_filtes_l->total_gain)>>GAIN_Q);
                anc_ff_iir_coefs0_l[0].b2=(int32)((ff_filtes_l->iir_coef[0].coef_b[2]*(int64_t)ff_filtes_l->total_gain)>>GAIN_Q);
            }
            else
            {
                //feedforward left ch iir coefs settings
                for(i=0;i<IIR_COUNTER;i++)
                {
                    anc_ff_iir_coefs1_l[i].a1=-ff_filtes_l->iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs1_l[i].a2=-ff_filtes_l->iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs1_l[i].b0=ff_filtes_l->iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs1_l[i].b1=ff_filtes_l->iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs1_l[i].b2=ff_filtes_l->iir_coef[i].coef_b[2];
                }
                anc_ff_iir_coefs1_l[0].b0=(int32)((ff_filtes_l->iir_coef[0].coef_b[0]*(int64_t)ff_filtes_l->total_gain)>>GAIN_Q);
                anc_ff_iir_coefs1_l[0].b1=(int32)((ff_filtes_l->iir_coef[0].coef_b[1]*(int64_t)ff_filtes_l->total_gain)>>GAIN_Q);
                anc_ff_iir_coefs1_l[0].b2=(int32)((ff_filtes_l->iir_coef[0].coef_b[2]*(int64_t)ff_filtes_l->total_gain)>>GAIN_Q);
            }

            if(err!=ANC_NO_ERR)
            {
                max_ff_gain_l=0;
            }

            if(anc_gain_delay==ANC_GAIN_NO_DELAY)
            {
                //Set the FF gain;
#ifdef ANC_GAIN_RAMP
                lock = int_lock();
                anc_gain_and_limiter_config->codec_iir_a_gain_ext_update_ch0=0;
                 *codec_iir_a_gain_ext_ch0=(int32)(((float)max_ff_gain_l/512.0f)*FIXED_GAIN_RAMP_Q);
                 hal_sys_timer_delay_us(1);
                anc_gain_and_limiter_config->codec_iir_a_gain_ext_update_ch0=1;
                int_unlock(lock);
#else
                lock = int_lock();
                anc_gain_and_limiter_config->codec_anc_mute_gain_update_ff_ch0=0;
                anc_control->codec_anc_mute_gain_ff_ch0=max_ff_gain_l;
                hal_sys_timer_delay_us(1);
                anc_gain_and_limiter_config->codec_anc_mute_gain_update_ff_ch0=1;
                int_unlock(lock);
#endif
            }

        }
        else
        {
            anc_iir_control->codec_iir_a_ch0_bypass=1;
#ifdef ANC_GAIN_RAMP
            lock = int_lock();
            anc_gain_and_limiter_config->codec_iir_a_gain_ext_update_ch0=0;
            *codec_iir_a_gain_ext_ch0=(int32)(((float)ff_filtes_l->total_gain/512.0f)*FIXED_GAIN_RAMP_Q);
            hal_sys_timer_delay_us(1);
            anc_gain_and_limiter_config->codec_iir_a_gain_ext_update_ch0=1;
            int_unlock(lock);
#else
            lock = int_lock();
            anc_gain_and_limiter_config->codec_anc_mute_gain_update_ff_ch0=0;
            anc_control->codec_anc_mute_gain_ff_ch0=ff_filtes_l->total_gain;
            hal_sys_timer_delay_us(1);
            anc_gain_and_limiter_config->codec_anc_mute_gain_update_ff_ch0=1;
            int_unlock(lock);
#endif
            max_ff_gain_l=ff_filtes_l->total_gain;

        }

        if(ff_filtes_r->iir_bypass_flag==0)
        {
            anc_iir_control->codec_iir_a_ch1_bypass=0;

            if(ff_filtes_r->iir_counter>IIR_COUNTER)
            {
                anc_iir_control->codec_iir_a_count_ch1=IIR_COUNTER;
            }
            else if(ff_filtes_r->iir_counter>0)
            {
                anc_iir_control->codec_iir_a_count_ch1=ff_filtes_r->iir_counter;
            }
            else
            {
                anc_iir_control->codec_iir_a_ch1_bypass=1;
                LOG_I("Error ff_filtes_r IIR counter:%d", ff_filtes_r->iir_counter);
                err=ANC_TYPE_ERR;
            }
            if(iir_coef_using==0)
            {
                //feedforward right ch iir coefs settings
                for(i=0;i<IIR_COUNTER;i++)
                {
                    anc_ff_iir_coefs0_r[i].a1=-ff_filtes_r->iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs0_r[i].a2=-ff_filtes_r->iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs0_r[i].b0=ff_filtes_r->iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs0_r[i].b1=ff_filtes_r->iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs0_r[i].b2=ff_filtes_r->iir_coef[i].coef_b[2];
                }
                anc_ff_iir_coefs0_r[0].b0=(int32)((ff_filtes_r->iir_coef[0].coef_b[0]*(int64_t)ff_filtes_r->total_gain)>>GAIN_Q);
                anc_ff_iir_coefs0_r[0].b1=(int32)((ff_filtes_r->iir_coef[0].coef_b[1]*(int64_t)ff_filtes_r->total_gain)>>GAIN_Q);
                anc_ff_iir_coefs0_r[0].b2=(int32)((ff_filtes_r->iir_coef[0].coef_b[2]*(int64_t)ff_filtes_r->total_gain)>>GAIN_Q);
            }
            else
            {
                //feedforward right ch iir coefs settings
                for(i=0;i<IIR_COUNTER;i++)
                {
                    anc_ff_iir_coefs1_r[i].a1=-ff_filtes_r->iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs1_r[i].a2=-ff_filtes_r->iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs1_r[i].b0=ff_filtes_r->iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs1_r[i].b1=ff_filtes_r->iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs1_r[i].b2=ff_filtes_r->iir_coef[i].coef_b[2];
                }
                anc_ff_iir_coefs1_r[0].b0=(int32)((ff_filtes_r->iir_coef[0].coef_b[0]*(int64_t)ff_filtes_r->total_gain)>>GAIN_Q);
                anc_ff_iir_coefs1_r[0].b1=(int32)((ff_filtes_r->iir_coef[0].coef_b[1]*(int64_t)ff_filtes_r->total_gain)>>GAIN_Q);
                anc_ff_iir_coefs1_r[0].b2=(int32)((ff_filtes_r->iir_coef[0].coef_b[2]*(int64_t)ff_filtes_r->total_gain)>>GAIN_Q);
            }
            if(err!=ANC_NO_ERR)
            {
                max_ff_gain_r=0;
            }
            if(anc_gain_delay==ANC_GAIN_NO_DELAY)
            {
                //Set the FF gain;
#ifdef ANC_GAIN_RAMP
                lock = int_lock();
                anc_gain_and_limiter_config->codec_iir_a_gain_ext_update_ch1=0;
                 *codec_iir_a_gain_ext_ch1=(int32)(((float)max_ff_gain_l/512.0f)*FIXED_GAIN_RAMP_Q);
                hal_sys_timer_delay_us(1);
                anc_gain_and_limiter_config->codec_iir_a_gain_ext_update_ch1=1;
                int_unlock(lock);
#else
                lock = int_lock();
                anc_gain_and_limiter_config->codec_anc_mute_gain_update_ff_ch1=0;
                anc_control->codec_anc_mute_gain_ff_ch1=max_ff_gain_r;
                hal_sys_timer_delay_us(1);
                anc_gain_and_limiter_config->codec_anc_mute_gain_update_ff_ch1=1;
                int_unlock(lock);
#endif
            }
        }
        else
        {
            anc_iir_control->codec_iir_a_ch1_bypass=1;
#ifdef ANC_GAIN_RAMP
            lock = int_lock();
            anc_gain_and_limiter_config->codec_iir_a_gain_ext_update_ch0=0;
            *codec_iir_a_gain_ext_ch0=(int32)(((float)ff_filtes_l->total_gain/512.0f)*FIXED_GAIN_RAMP_Q);
            hal_sys_timer_delay_us(1);
            anc_gain_and_limiter_config->codec_iir_a_gain_ext_update_ch0=1;
            int_unlock(lock);
#else
            lock = int_lock();
            anc_gain_and_limiter_config->codec_anc_mute_gain_update_ff_ch1=0;
            anc_control->codec_anc_mute_gain_ff_ch1=ff_filtes_r->total_gain;
            hal_sys_timer_delay_us(1);
            anc_gain_and_limiter_config->codec_anc_mute_gain_update_ff_ch1=1;
            int_unlock(lock);
#endif
            max_ff_gain_r=ff_filtes_r->total_gain;
        }

        total_iir_counter=total_iir_counter+anc_iir_control->codec_iir_a_count_ch0+anc_iir_control->codec_iir_a_count_ch1;

        LOG_I("%s:total_iir_counter:%d", __func__,total_iir_counter);

        //enable all IIR filters
        anc_iir_control->codec_iir_a_enable=1;
    }



    anc_control->codec_anc_enable_ch0=1;
    anc_control->codec_anc_enable_ch1=1;

    if(anc_type==ANC_FEEDFORWARD)
    {
        analog_aud_apply_anc_adc_gain_offset(anc_type, ff_filtes_l->adc_gain_offset, ff_filtes_r->adc_gain_offset);
    }

	return err;
}

static void anc_ctrl_reg_init(void)
{
    //disable ANC;
    anc_control->codec_anc_enable_ch0=0;
    anc_control->codec_anc_enable_ch1=0;

    //init swap to use coef0
    anc_iir_control->codec_iir_a_coef_swap=0;

    anc_control->codec_anc_rate_sel=0;

    //disable all IIR filters
    anc_iir_control->codec_iir_a_enable=0;

    //use FF IIR filters
    anc_iir_control->codec_iir_a_ch0_bypass=1;
    anc_iir_control->codec_iir_a_ch1_bypass=1;

    //use all of IIR filters.The max number is 8.
    anc_iir_control->codec_iir_a_count_ch0=0;
    anc_iir_control->codec_iir_a_count_ch1=0;

    //unmute ANC.
    anc_control->codec_anc_mute_ch0=0;
    anc_control->codec_anc_mute_ch1=0;

    //disable gain updated when pass0
    anc_control->codec_anc_mute_gain_pass0_ff_ch0=1;
    anc_control->codec_anc_mute_gain_pass0_ff_ch1=1;
}


static void anc_ctrl_reg_open(enum ANC_TYPE_T anc_type)
{
    uint32_t lock;

    LOG_I("%s", __func__);

    if(anc_type==ANC_FEEDFORWARD)
    {
        //clear iir counter
        anc_iir_control->codec_iir_a_count_ch0=0;
        anc_iir_control->codec_iir_a_count_ch1=0;

        //set the FF gain;
        anc_gain_and_limiter_config->codec_anc_mute_gain_update_ff_ch0=0;
        anc_gain_and_limiter_config->codec_anc_mute_gain_update_ff_ch1=0;

#ifdef ANC_GAIN_RAMP
        anc_control->codec_anc_mute_gain_ff_ch0=512;
        anc_control->codec_anc_mute_gain_ff_ch1=512;
#else
        anc_control->codec_anc_mute_gain_ff_ch0=0;
        anc_control->codec_anc_mute_gain_ff_ch1=0;
#endif

        anc_gain_and_limiter_config->codec_anc_mute_gain_update_ff_ch0=1;
        anc_gain_and_limiter_config->codec_anc_mute_gain_update_ff_ch1=1;

        ff_filtes_l_old.total_gain=0;
        ff_filtes_l_old.iir_counter=0;
        ff_filtes_l_old.iir_bypass_flag=1;

        ff_filtes_r_old.total_gain=0;
        ff_filtes_r_old.iir_counter=0;
        ff_filtes_r_old.iir_bypass_flag=1;

        ff_iir_reset_flag=0;

#ifdef ANC_GAIN_RAMP
        anc_gain_iir_coefs0_l[0].a1=-iir_coef_gain_ramp_l.coef_a[1];
        anc_gain_iir_coefs0_l[0].a2=-iir_coef_gain_ramp_l.coef_a[2];
        anc_gain_iir_coefs0_l[0].b0=iir_coef_gain_ramp_l.coef_b[0];
        anc_gain_iir_coefs0_l[0].b1=iir_coef_gain_ramp_l.coef_b[1];
        anc_gain_iir_coefs0_l[0].b2=iir_coef_gain_ramp_l.coef_b[2];

        anc_gain_iir_coefs0_r[0].a1=-iir_coef_gain_ramp_r.coef_a[1];
        anc_gain_iir_coefs0_r[0].a2=-iir_coef_gain_ramp_r.coef_a[2];
        anc_gain_iir_coefs0_r[0].b0=iir_coef_gain_ramp_r.coef_b[0];
        anc_gain_iir_coefs0_r[0].b1=iir_coef_gain_ramp_r.coef_b[1];
        anc_gain_iir_coefs0_r[0].b2=iir_coef_gain_ramp_r.coef_b[2];

        anc_gain_iir_coefs1_l[0].a1=-iir_coef_gain_ramp_l.coef_a[1];
        anc_gain_iir_coefs1_l[0].a2=-iir_coef_gain_ramp_l.coef_a[2];
        anc_gain_iir_coefs1_l[0].b0=iir_coef_gain_ramp_l.coef_b[0];
        anc_gain_iir_coefs1_l[0].b1=iir_coef_gain_ramp_l.coef_b[1];
        anc_gain_iir_coefs1_l[0].b2=iir_coef_gain_ramp_l.coef_b[2];

        anc_gain_iir_coefs1_r[0].a1=-iir_coef_gain_ramp_r.coef_a[1];
        anc_gain_iir_coefs1_r[0].a2=-iir_coef_gain_ramp_r.coef_a[2];
        anc_gain_iir_coefs1_r[0].b0=iir_coef_gain_ramp_r.coef_b[0];
        anc_gain_iir_coefs1_r[0].b1=iir_coef_gain_ramp_r.coef_b[1];
        anc_gain_iir_coefs1_r[0].b2=iir_coef_gain_ramp_r.coef_b[2];

        anc_gain_and_limiter_config->codec_iir_a_gaincal_ext_ch0_bypass=0;
        anc_gain_and_limiter_config->codec_iir_a_gaincal_ext_ch1_bypass=0;

        anc_gain_and_limiter_config->codec_iir_a_gainuse_ext_ch0_bypass=0;
        anc_gain_and_limiter_config->codec_iir_a_gainuse_ext_ch1_bypass=0;

        anc_gain_and_limiter_config->codec_iir_a_gain_ext_sel_ch0=0;
        anc_gain_and_limiter_config->codec_iir_a_gain_ext_sel_ch1=0;

        lock = int_lock();
        anc_gain_and_limiter_config->codec_iir_a_gain_ext_update_ch0=0;
        anc_gain_and_limiter_config->codec_iir_a_gain_ext_update_ch1=0;

         *codec_iir_a_gain_ext_ch0=0;
         *codec_iir_a_gain_ext_ch1=0;

        hal_sys_timer_delay_us(1);

        anc_gain_and_limiter_config->codec_iir_a_gain_ext_update_ch0=1;
        anc_gain_and_limiter_config->codec_iir_a_gain_ext_update_ch1=1;
        int_unlock(lock);

        ff_ramp_gain_l=0;
        ff_ramp_gain_r=0;
#endif

#ifdef ANC_LIMITER
        anc_limiter_iir_coefs0_l[0].a1=-iir_coef_limiter_attack_l.coef_a[1];
        anc_limiter_iir_coefs0_l[0].a2=-iir_coef_limiter_attack_l.coef_a[2];
        anc_limiter_iir_coefs0_l[0].b0=iir_coef_limiter_attack_l.coef_b[0];
        anc_limiter_iir_coefs0_l[0].b1=iir_coef_limiter_attack_l.coef_b[1];
        anc_limiter_iir_coefs0_l[0].b2=iir_coef_limiter_attack_l.coef_b[2];

        anc_limiter_iir_coefs0_l[1].a1=-iir_coef_limiter_release_l.coef_a[1];
        anc_limiter_iir_coefs0_l[1].a2=-iir_coef_limiter_release_l.coef_a[2];
        anc_limiter_iir_coefs0_l[1].b0=iir_coef_limiter_release_l.coef_b[0];
        anc_limiter_iir_coefs0_l[1].b1=iir_coef_limiter_release_l.coef_b[1];
        anc_limiter_iir_coefs0_l[1].b2=iir_coef_limiter_release_l.coef_b[2];

        anc_limiter_iir_coefs0_r[0].a1=-iir_coef_limiter_attack_r.coef_a[1];
        anc_limiter_iir_coefs0_r[0].a2=-iir_coef_limiter_attack_r.coef_a[2];
        anc_limiter_iir_coefs0_r[0].b0=iir_coef_limiter_attack_r.coef_b[0];
        anc_limiter_iir_coefs0_r[0].b1=iir_coef_limiter_attack_r.coef_b[1];
        anc_limiter_iir_coefs0_r[0].b2=iir_coef_limiter_attack_r.coef_b[2];

        anc_limiter_iir_coefs0_r[1].a1=-iir_coef_limiter_release_r.coef_a[1];
        anc_limiter_iir_coefs0_r[1].a2=-iir_coef_limiter_release_r.coef_a[2];
        anc_limiter_iir_coefs0_r[1].b0=iir_coef_limiter_release_r.coef_b[0];
        anc_limiter_iir_coefs0_r[1].b1=iir_coef_limiter_release_r.coef_b[1];
        anc_limiter_iir_coefs0_r[1].b2=iir_coef_limiter_release_r.coef_b[2];

        anc_limiter_iir_coefs1_l[0].a1=-iir_coef_limiter_attack_l.coef_a[1];
        anc_limiter_iir_coefs1_l[0].a2=-iir_coef_limiter_attack_l.coef_a[2];
        anc_limiter_iir_coefs1_l[0].b0=iir_coef_limiter_attack_l.coef_b[0];
        anc_limiter_iir_coefs1_l[0].b1=iir_coef_limiter_attack_l.coef_b[1];
        anc_limiter_iir_coefs1_l[0].b2=iir_coef_limiter_attack_l.coef_b[2];

        anc_limiter_iir_coefs1_l[1].a1=-iir_coef_limiter_release_l.coef_a[1];
        anc_limiter_iir_coefs1_l[1].a2=-iir_coef_limiter_release_l.coef_a[2];
        anc_limiter_iir_coefs1_l[1].b0=iir_coef_limiter_release_l.coef_b[0];
        anc_limiter_iir_coefs1_l[1].b1=iir_coef_limiter_release_l.coef_b[1];
        anc_limiter_iir_coefs1_l[1].b2=iir_coef_limiter_release_l.coef_b[2];

        anc_limiter_iir_coefs1_r[0].a1=-iir_coef_limiter_attack_r.coef_a[1];
        anc_limiter_iir_coefs1_r[0].a2=-iir_coef_limiter_attack_r.coef_a[2];
        anc_limiter_iir_coefs1_r[0].b0=iir_coef_limiter_attack_r.coef_b[0];
        anc_limiter_iir_coefs1_r[0].b1=iir_coef_limiter_attack_r.coef_b[1];
        anc_limiter_iir_coefs1_r[0].b2=iir_coef_limiter_attack_r.coef_b[2];

        anc_limiter_iir_coefs1_r[1].a1=-iir_coef_limiter_release_r.coef_a[1];
        anc_limiter_iir_coefs1_r[1].a2=-iir_coef_limiter_release_r.coef_a[2];
        anc_limiter_iir_coefs1_r[1].b0=iir_coef_limiter_release_r.coef_b[0];
        anc_limiter_iir_coefs1_r[1].b1=iir_coef_limiter_release_r.coef_b[1];
        anc_limiter_iir_coefs1_r[1].b2=iir_coef_limiter_release_r.coef_b[2];

        anc_gain_and_limiter_config->codec_iir_a_lmt_th_update_ch0=0;
        anc_gain_and_limiter_config->codec_iir_a_lmt_th_update_ch1=0;

        //limiter threshold, 0: 0x800000,0db; 1: 0x400000, -6db; 2: -12db;�� 7: 0x10000, -42db
        anc_gain_and_limiter_config->codec_iir_a_lmt_th_ch0=0;
        anc_gain_and_limiter_config->codec_iir_a_lmt_th_ch1=0;

        anc_gain_and_limiter_config->codec_iir_a_lmt_th_update_ch0=1;
        anc_gain_and_limiter_config->codec_iir_a_lmt_th_update_ch1=1;

        anc_gain_and_limiter_config->codec_iir_a_lmt_ch0_bypass=0;
        anc_gain_and_limiter_config->codec_iir_a_lmt_ch1_bypass=0;

        anc_gain_and_limiter_config->codec_iir_a_lmt_delay=ANC_LIMITER_DELAY;

        //limiter release rate, 0:0x80=128; 1:0x100=256; 2:0x200;......7:0x4000
        anc_gain_and_limiter_config->codec_iir_a_lmt_rls_rate=7;

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


static void anc_ctrl_reg_close(enum ANC_TYPE_T anc_type)
{

    if(anc_type==ANC_FEEDFORWARD)
    {
        //clear iir counter
        anc_iir_control->codec_iir_a_count_ch0=0;
        anc_iir_control->codec_iir_a_count_ch1=0;

#ifdef ANC_GAIN_RAMP
        ff_ramp_gain_l=0;
        ff_ramp_gain_r=0;
#else
        anc_gain_and_limiter_config->codec_anc_mute_gain_update_ff_ch0=0;
        anc_gain_and_limiter_config->codec_anc_mute_gain_update_ff_ch1=0;

        //set the FF gain;
        anc_control->codec_anc_mute_gain_ff_ch0=0;
        anc_control->codec_anc_mute_gain_ff_ch1=0;

        anc_gain_and_limiter_config->codec_anc_mute_gain_update_ff_ch0=1;
        anc_gain_and_limiter_config->codec_anc_mute_gain_update_ff_ch1=1;
#endif
    }
}

int anc_opened(enum ANC_TYPE_T anc_type)
{
    if (anc_type == ANC_FEEDFORWARD) {
        return ff_open_flag;
    }
    return 0;
}

#ifdef ANC_FB_CHECK
#define ANC_CHECK_DELAY ((MS_TO_TICKS(5000)))
static void anc_fb_check_irq_handler(uint32_t status)
{
    //LOG_I("%s,", __func__);
    int32_t ff_gain_l, ff_gain_r;
    struct _codec_int_config *codec_int_config = (struct _codec_int_config *)&status;

    anc_get_gain(&ff_gain_l, &ff_gain_r, ANC_FEEDFORWARD);

    if(codec_int_config->anc_check_error_trig_ch0)
    {
        anc_check_ch0_config->codec_anc_check_enable_ch0=0;

        hwtimer_stop(anc_adc_check_dev_timer);
        hwtimer_start(anc_adc_check_dev_timer, ANC_CHECK_DELAY);

        if(ff_gain_l==max_ff_gain_l)
        {
            anc_set_gain(max_ff_gain_l/2, ff_gain_r,ANC_FEEDFORWARD);
        }

        LOG_I("ADC_left");

        anc_check_ch0_config->codec_anc_check_enable_ch0=1;
    }

    if(codec_int_config->anc_check_error_trig_ch1)
    {
        anc_check_ch1_config->codec_anc_check_enable_ch1=0;

        hwtimer_stop(anc_adc_check_dev_timer);
        hwtimer_start(anc_adc_check_dev_timer, ANC_CHECK_DELAY);

        if(ff_gain_r==max_ff_gain_r)
        {
            anc_set_gain(ff_gain_l, max_ff_gain_r/2,ANC_FEEDFORWARD);
        }

        LOG_I("ADC_right");

        anc_check_ch1_config->codec_anc_check_enable_ch1=1;
    }
}

static void anc_check_timer_handler(void *param)
{
    LOG_I("%s,", __func__);
    anc_set_gain(max_ff_gain_l, max_ff_gain_r,ANC_FEEDFORWARD);
}
#endif

int anc_open(enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err=ANC_NO_ERR;

    LOG_I("%s", __func__);

    if(anc_type==ANC_FEEDFORWARD&&ff_open_flag==1)return err;

    anc_cfg_lock();

#ifdef ANC_FB_CHECK
    if(anc_adc_check_dev_timer==NULL)
    {
        anc_adc_check_dev_timer = hwtimer_alloc(anc_check_timer_handler, NULL);
        ASSERT(anc_adc_check_dev_timer, "Failed to alloc usbdev_timer");
    }
#endif

    if(ff_open_flag==0)
    {
        hal_codec_iir_enable(78000000);
        anc_ctrl_reg_init();

        iir_coef_using=0;
    }


    if(anc_type==ANC_FEEDFORWARD)
    {
        max_ff_gain_l=0;
        max_ff_gain_r=0;

        ff_open_flag=1;

#ifdef ANC_FB_CHECK
        hal_codec_anc_fb_check_set_irq_handler(anc_fb_check_irq_handler);

        anc_check_ch0_config->codec_anc_check_enable_ch0=0;
        anc_check_ch1_config->codec_anc_check_enable_ch1=0;

        *codec_int_config_clr=(CODEC_FB_CHECK_ERROR_TRIG_CH0|CODEC_FB_CHECK_ERROR_TRIG_CH1);

        codec_mask_config->anc_check_error_trig_ch0_mask=1;
        codec_mask_config->anc_check_error_trig_ch1_mask=1;

        anc_check_ch0_config->codec_anc_check_acc_sample_rate_ch0=3;
        anc_check_ch0_config->codec_anc_check_acc_window_ch0=64;
        anc_check_ch0_config->codec_anc_check_trig_window_ch0=64;

        anc_check_ch1_config->codec_anc_check_acc_sample_rate_ch1=3;
        anc_check_ch1_config->codec_anc_check_acc_window_ch1=64;
        anc_check_ch1_config->codec_anc_check_trig_window_ch1=64;

        *codec_anc_check_threshold_ch0=0x4000000;
        *codec_anc_check_threshold_ch1=0x4000000;

        anc_check_ch0_config->codec_anc_check_enable_ch0=1;
        anc_check_ch1_config->codec_anc_check_enable_ch1=1;
#endif

    }

    anc_ctrl_reg_open(anc_type);

    anc_cfg_unlock();

    return	err;
}

void anc_close(enum ANC_TYPE_T anc_type)
{
    LOG_I("%s", __func__);

    if(anc_type==ANC_FEEDFORWARD)
    {
        max_ff_gain_l=0;
        max_ff_gain_r=0;

        ff_open_flag=0;

#ifdef ANC_FB_CHECK
        anc_check_ch0_config->codec_anc_check_enable_ch0=0;
        anc_check_ch1_config->codec_anc_check_enable_ch1=0;
        *codec_int_config_clr=(CODEC_FB_CHECK_ERROR_TRIG_CH0|CODEC_FB_CHECK_ERROR_TRIG_CH1);

        codec_mask_config->anc_check_error_trig_ch0_mask=0;
        codec_mask_config->anc_check_error_trig_ch1_mask=0;
#endif
    }

    anc_ctrl_reg_close(anc_type);

    if(ff_open_flag==0)
    {
        hal_codec_iir_disable();
    }

    return;
}

int anc_enable( void)
{
    LOG_I("%s", __func__);

    anc_cfg_lock();

    anc_control->codec_anc_enable_ch0=1;
    anc_control->codec_anc_enable_ch1=1;

#ifdef ANC_FB_CHECK
    anc_check_ch0_config->codec_anc_check_enable_ch0=1;
    anc_check_ch1_config->codec_anc_check_enable_ch1=1;
#endif

    anc_cfg_unlock();

    return ANC_NO_ERR;
}

int anc_disable(void)
{
    LOG_I("%s", __func__);

    anc_cfg_lock();

#ifdef ANC_FB_CHECK
    anc_check_ch0_config->codec_anc_check_enable_ch0=0;
    anc_check_ch1_config->codec_anc_check_enable_ch1=0;
    hwtimer_stop(anc_adc_check_dev_timer);
#endif

#ifndef ANC_GAIN_RAMP
    anc_control->codec_anc_enable_ch0=0;
    anc_control->codec_anc_enable_ch1=0;
#endif

    anc_cfg_unlock();

    return ANC_NO_ERR;
}

int anc_set_gain(int32_t gain_ch_l, int32_t gain_ch_r,enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err=ANC_NO_ERR;
    uint32_t lock;

    anc_cfg_lock();

#ifdef ANC_GAIN_RAMP
    lock = int_lock();

    anc_gain_and_limiter_config->codec_iir_a_gain_ext_update_ch0=0;
    anc_gain_and_limiter_config->codec_iir_a_gain_ext_update_ch1=0;

    if(anc_type==ANC_FEEDFORWARD)
    {
        *codec_iir_a_gain_ext_ch0=(int32)(((float)gain_ch_l/512.0f)*FIXED_GAIN_RAMP_Q);
        *codec_iir_a_gain_ext_ch1=(int32)(((float)gain_ch_r/512.0f)*FIXED_GAIN_RAMP_Q);

        ff_ramp_gain_l=gain_ch_l;
        ff_ramp_gain_r=gain_ch_r;
    }
    else
    {
        *codec_iir_a_gain_ext_ch0=0;
        *codec_iir_a_gain_ext_ch1=0;

        ff_ramp_gain_l=0;
        ff_ramp_gain_r=0;

        err=ANC_TYPE_ERR;
        LOG_I("Error ANC type:%d", anc_type);
    }
    hal_sys_timer_delay_us(1);

    anc_gain_and_limiter_config->codec_iir_a_gain_ext_update_ch0=1;
    anc_gain_and_limiter_config->codec_iir_a_gain_ext_update_ch1=1;

    int_unlock(lock);

#else
    lock = int_lock();

    anc_gain_and_limiter_config->codec_anc_mute_gain_update_ff_ch0=0;
    anc_gain_and_limiter_config->codec_anc_mute_gain_update_ff_ch1=0;

    if(anc_type==ANC_FEEDFORWARD)
    {
        //Set the FF gain;
        anc_control->codec_anc_mute_gain_ff_ch0=gain_ch_l;
        anc_control->codec_anc_mute_gain_ff_ch1=gain_ch_r;
    }
    else
    {
        anc_control->codec_anc_mute_gain_ff_ch0=0;
        anc_control->codec_anc_mute_gain_ff_ch1=0;

        err=ANC_TYPE_ERR;
        LOG_I("Error ANC type:%d", anc_type);
    }

    hal_sys_timer_delay_us(1);

    anc_gain_and_limiter_config->codec_anc_mute_gain_update_ff_ch0=1;
    anc_gain_and_limiter_config->codec_anc_mute_gain_update_ff_ch1=1;

    int_unlock(lock);
#endif

    anc_cfg_unlock();

	return err;
}

int anc_set_gain_f32(float gain_l, float gain_r, enum ANC_TYPE_T type)
{
    return anc_set_gain((int32_t)(gain_l * 512), (int32_t)(gain_r * 512), type);
}

int anc_get_gain(int32_t *gain_ch_l, int32_t *gain_ch_r,enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err=ANC_NO_ERR;

    anc_cfg_lock();
#ifdef ANC_GAIN_RAMP
    if(anc_type==ANC_FEEDFORWARD)
    {
        //Set the FF gain;
        *gain_ch_l=ff_ramp_gain_l;
        *gain_ch_r=ff_ramp_gain_r;
    }
    else
    {
        //if err ANC type,retun 0;
        *gain_ch_l=0;
        *gain_ch_r=0;

        err=ANC_TYPE_ERR;
        LOG_I("Error ANC type:%d", anc_type);
    }
#else
    if(anc_type==ANC_FEEDFORWARD)
    {
        //Get the FF gain;
        *gain_ch_l=anc_control->codec_anc_mute_gain_ff_ch0;
        *gain_ch_r=anc_control->codec_anc_mute_gain_ff_ch1;
    }
    else
    {
        //if err ANC type,retun 0;
        *gain_ch_l=0;
        *gain_ch_r=0;
        err=ANC_TYPE_ERR;
        LOG_I("Error ANC type:%d", anc_type);
    }
    anc_cfg_unlock();
#endif
    //  LOG_I("anc_get_gain gain_ch_l:%d,gain_ch_r:%d",*gain_ch_l,*gain_ch_r);
	return err;
}

int anc_get_cfg_gain(int32_t *gain_ch_l, int32_t *gain_ch_r,enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err=ANC_NO_ERR;

    anc_cfg_lock();
    if(anc_type==ANC_FEEDFORWARD)
    {
        //Get the max FF gain;
        *gain_ch_l = max_ff_gain_l;
        *gain_ch_r = max_ff_gain_r;
    }
    else
    {
        //if err ANC type,retun 0;
        *gain_ch_l=0;
        *gain_ch_r=0;
        err=ANC_TYPE_ERR;
        LOG_I("Error ANC type:%d", anc_type);
    }
    anc_cfg_unlock();

    //	LOG_I("anc_get_cfg_gain gain_ch_l:%d,gain_ch_r:%d",*gain_ch_l,*gain_ch_r);
    return err;
}

void  anc_set_ch_map( int32_t ch_map )
{
    return;
}

int anc_set_switching_delay(ANC_SWITCHING_DELAY  anc_switching_delay,enum ANC_TYPE_T anc_type)
{
    return 0;
}

int anc_howling_set(ANC_HOWLING_WINDOW window, ANC_HOWLING_THRESHOLD threshold)
{
    return 0;
}
int anc_adc_data_select(ANC_ADC_DATA data_select)
{
    return 0;
}

