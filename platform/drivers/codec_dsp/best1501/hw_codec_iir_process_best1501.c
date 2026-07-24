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
** namer��EQ filter
** description��IIR filter
** version��V1.0
** author�� xuml
** modify��2020.10.5.
*******************************************************************************/

#include "mm_dbg.h"
#include <stdio.h>
#include <string.h>

#include "plat_addr_map.h"
#include "cmsis.h"
#include "hal_trace.h"
#include "hal_cmu.h"
#include "hal_codec.h"
#include "hal_timer.h"
#include "iir_process.h"
#include "hw_codec_iir_process.h"
#include "hwtimer_list.h"
#include "hal_chipid.h"


/**********************************mocro**************************************/

typedef short int16;
typedef unsigned short uint16;
typedef int   int32;
typedef unsigned int   uint32;


/*
1	258	codec_deq_iir_enable	RW	0	0	anc iir module enable
1	258	codec_deq_iir_iira_enable	RW	0	1
1	258	codec_deq_iir_iirb_enable	RW	0	2
1	258	codec_deq_iir_ch0_bypass	RW	0	3	1: ch0 bypass
1	258	codec_deq_iir_gaincal_ext_ch0_bypass	RW	1	4
1	258	codec_deq_iir_gainuse_ext_ch0_bypass	RW	1	5
6	258	codec_deq_iir_count_ch0	RW	0	6	ch0 iir number  0~8
1	258	codec_deq_iir_coef_swap	RW	0	12	0: use iir coef memory0;  1: use iir coef memory1;
1	258	codec_deq_iir_auto_stop	RW	0	13	read only;  coef memoryX used currently
1	258	codec_deq_iir_coef_swap_status_sync[1]	R	0	14	read only;  coef memoryX used currently
1	258	codec_deq_iir_iira_stop_status_sync[1]	R	0	15
1	258	codec_deq_iir_iirb_stop_status_sync[1]	R	0	16
17	258		x


1	258	codec_deq_iir_enable	RW	0	0	anc iir module enable
1	258	codec_deq_iir_iira_enable	RW	0	1
1	258	codec_deq_iir_iirb_enable	RW	0	2
1	258	codec_deq_iir_ch0_bypass	RW	0	3	1: ch0 bypass
1	258	codec_deq_iir_ch1_bypass	RW	0	4
1	258	codec_deq_iir_gaincal_ext_ch0_bypass	RW	1	5
1	258	codec_deq_iir_gaincal_ext_ch1_bypass	RW	1	6
1	258	codec_deq_iir_gainuse_ext_ch0_bypass	RW	1	7
1	258	codec_deq_iir_gainuse_ext_ch1_bypass	RW	1	8
6	258	codec_deq_iir_count_ch0	RW	0	9	ch0 iir number  0~8
6	258	codec_deq_iir_count_ch1	RW	0	15
1	258	codec_deq_iir_coef_swap	RW	0	21	0: use iir coef memory0;  1: use iir coef memory1;
1	258	codec_deq_iir_auto_stop	RW	0	22	read only;  coef memoryX used currently
1	258	codec_deq_iir_coef_swap_status_sync[1]	R	0	23	read only;  coef memoryX used currently
1	258	codec_deq_iir_iira_stop_status_sync[1]	R	0	24
1	258	codec_deq_iir_iirb_stop_status_sync[1]	R	0	25
26	258		x



*/
struct _deq_iir_d_control
{
	uint32 codec_deq_iir_enable : 1;

	uint32 codec_deq_iir_iira_enable : 1;
	uint32 codec_deq_iir_iirb_enable : 1;

	uint32 codec_deq_iir_ch0_bypass : 1;
	uint32 codec_deq_iir_ch1_bypass : 1;

       uint32 codec_deq_iir_gaincal_ext_ch0_bypass : 1;
       uint32 codec_deq_iir_gaincal_ext_ch1_bypass : 1;
       uint32 codec_deq_iir_gainuse_ext_ch0_bypass : 1;
       uint32 codec_deq_iir_gainuse_ext_ch1_bypass : 1;

       uint32 codec_deq_iir_count_ch0 : 6;
       uint32 codec_deq_iir_count_ch1 : 6;

       uint32 codec_deq_iir_coef_swap : 1;
       uint32 codec_deq_iir_auto_stop : 1;

       uint32 codec_deq_iir_coef_swap_status_sync : 1;
       uint32 codec_deq_iir_iira_stop_status_sync : 1;
       uint32 codec_deq_iir_iirb_stop_status_sync : 1;

	uint32 reserved : 6;
};

/*
1	25c	codec_iir0_gain_ext_update_ch0	RW	0	0
1	25c	codec_iir0_gain_ext_update_ch1	RW	0	1
1	25c	codec_iir0_gain_ext_sel_ch0	RW	0	2
1	25c	codec_iir0_gain_ext_sel_ch1	RW	0	3
1	25c	codec_iir1_gain_ext_update_ch0	RW	0	4
1	25c	codec_iir1_gain_ext_update_ch1	RW	0	5
1	25c	codec_iir1_gain_ext_sel_ch0	RW	0	6
1	25c	codec_iir1_gain_ext_sel_ch1	RW	0	7
1	25c	codec_iir2_gain_ext_update_ch0	RW	0	8
1	25c	codec_iir2_gain_ext_update_ch1	RW	0	9
1	25c	codec_iir2_gain_ext_sel_ch0	RW	0	10
1	25c	codec_iir2_gain_ext_sel_ch1	RW	0	11
1	25c	codec_deq_iir_gain_ext_update_ch0	RW	0	12
1	25c	codec_deq_iir_gain_ext_update_ch1	RW	0	13
1	25c	codec_deq_iir_gain_ext_sel_ch0	RW	0	14
1	25c	codec_deq_iir_gain_ext_sel_ch1	RW	0	15
1	25c	codec_iir0_lmt_th_update_ch0	RW	0	16
1	25c	codec_iir0_lmt_th_update_ch1	RW	0	17
1	25c	codec_iir1_lmt_th_update_ch0	RW	0	18
1	25c	codec_iir1_lmt_th_update_ch1	RW	0	19
1	25c	codec_iir2_lmt_th_update_ch0	RW	0	20
1	25c	codec_iir2_lmt_th_update_ch1	RW	0	21
22	25c		x
*/
struct _anc_iir_gain_update
{
    uint32 codec_iir0_gain_ext_update : 2;
    uint32 codec_iir0_gain_ext_sel_ch0 : 1;
    uint32 codec_iir0_gain_ext_sel_ch1 : 1;

    uint32 codec_iir1_gain_ext_update : 2;
    uint32 codec_iir1_gain_ext_sel_ch0 : 1;
    uint32 codec_iir1_gain_ext_sel_ch1 : 1;

    uint32 codec_iir2_gain_ext_update: 2;
    uint32 codec_iir2_gain_ext_sel_ch0 : 1;
    uint32 codec_iir2_gain_ext_sel_ch1 : 1;

    uint32 codec_deq_iir_gain_ext_update_ch0 : 1;
    uint32 codec_deq_iir_gain_ext_update_ch1 : 1;
    uint32 codec_deq_iir_gain_ext_sel_ch0 : 1;
    uint32 codec_deq_iir_gain_ext_sel_ch1 : 1;

    uint32 codec_iir0_lmt_th_update_ch0 : 1;
    uint32 codec_iir0_lmt_th_update_ch1 : 1;
    uint32 codec_iir1_lmt_th_update_ch0 : 1;
    uint32 codec_iir1_lmt_th_update_ch1 : 1;
    uint32 codec_iir2_lmt_th_update_ch0 : 1;
    uint32 codec_iir2_lmt_th_update_ch1 : 1;

    uint32 Reserved : 10;
};


/*
1	bc	codec_adc_mc_en_ch0	RW	0	0	adc ch0 music cancel enable
1	bc	codec_feedback_mc_en_ch0	RW	0	1	feedback music cancel enable
1	bc	codec_dac_L_iir_enable	RW	0	2
1	bc	codec_adc_ch0_iir_enable	RW	0	3
1	bc	codec_adc_ch1_iir_enable	RW	0	4
1	bc	codec_adc_ch2_iir_enable	RW	0	5
1	bc	codec_adc_ch3_iir_enable	RW	0	6
4	bc	codec_fb_check_udc_ch0	RW	0	7
11	bc		x
*/
struct _codec_dac_iir_config
{
	uint32 reserved0 : 4;
       uint32 codec_dac_L_iir_enable : 1;
       uint32 codec_dac_R_iir_enable : 1;
	uint32 reserved1 : 26;
};


/*
1	a4	codec_pdm_enable	RW	0	0	dig mems enable
1	a4	codec_pdm_data_inv	RW	0	1	data inv;  1: 0 to 1, 1 to 0
2	a4	codec_pdm_rate_sel	RW	0	2
1	a4	codec_pdm_adc_sel_ch0	RW	0	4	1: dig adc ch0 path use mems
1	a4	codec_pdm_adc_sel_ch1	RW	0	5	1: dig adc ch1 path use mems
1	a4	codec_pdm_adc_sel_ch2	RW	0	6
1	a4	codec_pdm_adc_sel_ch3	RW	0	7
1	a4	codec_adc_dcf_bypass_ch0	RW	1	8
1	a4	codec_adc_dcf_bypass_ch1	RW	1	9
1	a4	codec_adc_dcf_bypass_ch2	RW	1	10
1	a4	codec_adc_dcf_bypass_ch3	RW	1	11
1	a4	codec_dac_sdm_close	RW	0	12	1: reset sdm module
1	a4	codec_ditherf_bypass	RW	1	13
2	a4	codec_dac_ditherf_gain	RW	0	14
1	a4	codec_dac_uh_en	RW	0	16
1	a4	codec_dac_lh_en	RW	0	17
1	a4	codec_dac_sdm_3rd_en_ch0	RW	0	18
2	a4	en_48kx64_mode	RW	0	19
21	a4		x
*/

struct _codec_dac_config
{
	uint32 reserved0 : 16;

	uint32 codec_dac_uh_en : 1;
	uint32 codec_dac_lh_en : 1;

	uint32 reserved1 : 14;
};



/*
5	0c	codec_rx_overflow 	RW	0	0	interrupt status which not been masked.
5	0c	codec_rx_underflow	RW	0	5
1	0c	codec_tx_overflow	RW	0	10
1	0c	codec_tx_underflow	RW	0	11
1	0c	codec_tx_overflow_snd	RW	0	12
1	0c	codec_tx_underflow_snd	RW	0	13
1	0c	dsd_rx_overflow	RW	0	14
1	0c	dsd_rx_underflow	RW	0	15
1	0c	dsd_tx_overflow	RW	0	16
1	0c	dsd_tx_underflow	RW	0	17
1	0c	mc_overflow	RW	0	18
1	0c	mc_underflow	RW	0	19
1	0c	event_trigger	RW	0	20
1	0c	fb_check_error_trig_ch0	RW	0	21
1	0c	vad_find	RW	0	22
1	0c	vad_not_find	RW	0	23
1	0c	bt_trigger	RW	0	24
1	0c	adc_max_overflow	RW	0	25
1	0c	time_trigger	RW	0	26
27	0c		x
*/

struct _trigger_status
{
	uint32 reserved0 : 24;

	uint32 bt_trigger : 1;

	uint32 reserved : 7;
};


/*
2	54	dac_enable_sel	RW	0	0	  0: no trigger;  1: gpio trigger;  2: bt trigger;  this for dac dma interface enable;
2	54	adc_enable_sel	RW	0	2	  0: no trigger;  1: gpio trigger;  2: bt trigger;  this for adc dma interface enable;
2	54	dac_enable_sel_snd	RW	0	4
2	54	codec_dac_enable_sel	RW	0	6	  0: no trigger;  1: gpio trigger;  2: bt trigger;  this for codec dac enable;
2	54	codec_adc_enable_sel	RW	0	8	  0: no trigger;  1: gpio trigger;  2: bt trigger;  this for codec adc enable;
2	54	codec_adc_uh_enable_sel	RW	0	10
2	54	codec_adc_lh_enable_sel	RW	0	12
2	54	codec_dac_uh_enable_sel	RW	0	14	  0: no trigger;  1: gpio trigger;  2: bt trigger;  this for codec dac uh_enable;
2	54	codec_dac_lh_enable_sel	RW	0	16	  0: no trigger;  1: gpio trigger;  2: bt trigger;  this for codec dac lh_enable;
1	54	gpio_trigger_db_enable	RW	0	18	gpio trigger debounce enable;
1	54	stamp_clr_used	RW	0	19	Hardware clear trigger signal from bt
2	54	event_sel	RW	0	20	  0: select bt event for cnt keep;  1: select gpio event for cnt keep;
1	54	event_for_capture	RW	0	22	  1: enable 48k*64 counter;  each time of bt/gpio event will capture counter to cnt_keep
3	54	test_port_sel	RW	0	23
2	54	pll_osc_trigger_sel	RW	0	26	 0: no trigger;  1: gpio trigger;  2: bt trigger
1	54	fault_mute_dac_enable	RW	0	28
1	54	fault_mute_dac_enable_snd	RW	0	29
1	54	mode_hclk_access_reg	RW	0	30
1	54	trig_bt0_i2s1	RW	0	31
32	54		x
*/
struct _trigger_conig
{
	uint32 reserved0 : 16;

	uint32 codec_dac_lh_enable_sel : 2;

	uint32 reserved : 14;
};
/*
iir coefficients(b/a Q27)
a1
a2
b1
b2
b0
*/
struct _iir_coefs
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

#define DEQ_GAIN_RAMP_THRESHLD (40)

#define DEQ_GAIN_RAMP_ZERO (20)

#define DEQ_GAIN_RAMP_BURST_THRESHLD (DEQ_GAIN_RAMP_THRESHLD*10000)

#define IIR_DAC_COUNTER_L (34)

#define IIR_DAC_COUNTER_L_MAX (40)

/*******************************data struction***********************************/

#define ANC_BASE                            ((uint32)CODEC_BASE)

#define IIR_COEF_BASE  (ANC_BASE+0x5000)

//IIR registers
volatile static struct _deq_iir_d_control *deq_iir_d_control=(volatile struct _deq_iir_d_control *)(ANC_BASE+0x258);

volatile static struct _codec_dac_iir_config *codec_dac_iir_config=(volatile struct _codec_dac_iir_config *)(ANC_BASE+0xdc);
//volatile static struct _codec_dac_config *codec_dac_config=(volatile struct _codec_dac_config *)(ANC_BASE+0xa4);
//volatile static struct _trigger_status *trigger_status=(volatile struct _trigger_status *)(ANC_BASE+0xc);
//volatile static struct _trigger_conig *trigger_conig=(volatile struct _trigger_conig *)(ANC_BASE+0x54);


volatile static struct _iir_coefs *deq_dac_iir_coefs0_l=(volatile struct _iir_coefs *)(IIR_COEF_BASE);
//volatile static struct _iir_coefs *deq_dac_iir_coefs0_r=(volatile struct _iir_coefs *)(IIR_COEF_BASE+20*20);

volatile static struct _iir_coefs *deq_dac_gain_iir_coefs0_l=(volatile struct _iir_coefs *)(IIR_COEF_BASE+0x0320);
//volatile static struct _iir_coefs *deq_dac_gain_iir_coefs0_r=(volatile struct _iir_coefs *)(IIR_COEF_BASE+0x0334);

volatile static struct _iir_coefs *deq_dac_iir_coefs1_l=(volatile struct _iir_coefs *)(IIR_COEF_BASE+0x400);
//volatile static struct _iir_coefs *deq_dac_iir_coefs1_r=(volatile struct _iir_coefs *)(IIR_COEF_BASE+0x1400+20*20);

volatile static struct _iir_coefs *deq_dac_gain_iir_coefs1_l=(volatile struct _iir_coefs *)(IIR_COEF_BASE+0x720);
//volatile static struct _iir_coefs *deq_dac_gain_iir_coefs1_r=(volatile struct _iir_coefs *)(IIR_COEF_BASE+0x1734);


volatile static int32 *codec_deq_iir_gaina_ext_ch0=(volatile int32 *)(ANC_BASE+0x02e0);
//volatile static int32 *codec_deq_iir_gaina_ext_ch1=(volatile int32 *)(ANC_BASE+0x02e4);
volatile static int32 *codec_deq_iir_gainb_ext_ch0=(volatile int32 *)(ANC_BASE+0x02e8);
//volatile static int32 *codec_deq_iir_gainb_ext_ch1=(volatile int32 *)(ANC_BASE+0x02ec);


//volatile static int32 *codec_deq_iir_gaina_ext_out_ch0_sync=(volatile int32 *)(ANC_BASE+0x02f0);

//volatile static int32 *codec_deq_iir_gainb_ext_out_ch0_sync=(volatile int32 *)(ANC_BASE+0x02f8);


volatile static int32 *codec_deq_iir_gain_ext_th=(volatile int32 *)(ANC_BASE+0x0318);

volatile static struct _anc_iir_gain_update *anc_iir_gain_update=(volatile struct _anc_iir_gain_update *)(ANC_BASE+0x25c);

static int32 dac_eq_counter_l=IIR_DAC_COUNTER_L;

#define DEQ_GAIN_RAMP_DELAY   (1)

#if DEQ_GAIN_RAMP_DELAY==1

/*
Type='Low Passing';
Freq=240;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
static const anc_iir_coefs   iir_coef_gain_ramp_deq=
{
    .coef_b={30703,        61406,        30703},
    .coef_a={134217728,   -248768019,    114673103},
};
#define DEQ_SET_GAIN_TIME  ((MS_TO_TICKS(50)))

#elif DEQ_GAIN_RAMP_DELAY==2
/*

Type='Low Passing';
Freq=120;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
static const anc_iir_coefs   iir_coef_gain_ramp_deq=
{
    .coef_b={7966,        15932,         7966},
    .coef_a={134217728,   -258260876,    124075013},
};

#define DEQ_SET_GAIN_TIME  ((MS_TO_TICKS(100)))
#elif DEQ_GAIN_RAMP_DELAY==3

/*
Type='Low Passing';
Freq=60;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
static const anc_iir_coefs   iir_coef_gain_ramp_deq=
{
    .coef_b={2030,         4060,         2030},
    .coef_a={134217728,   -263258168,    129048560},
};
#define DEQ_SET_GAIN_TIME  ((MS_TO_TICKS(200)))
#elif DEQ_GAIN_RAMP_DELAY==4


/*
Type='Low Passing';
Freq=30;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs   iir_coef_gain_ramp_deq=
{
    .coef_b={            512,         1025,          512},
    .coef_a={134217728,   -265823675,    131607997},
};
#define DEQ_SET_GAIN_TIME  ((MS_TO_TICKS(400)))
#elif DEQ_GAIN_RAMP_DELAY==5
/*
Type='Low Passing';
Freq=15;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs   iir_coef_gain_ramp_deq=
{
    .coef_b={       129,         257,          129},
    .coef_a={134217728,   -267123699,    132906486},
};
#define DEQ_SET_GAIN_TIME  ((MS_TO_TICKS(800)))
#endif


#define TRACE_COUNTER (10000)

volatile static int dac_open_flag=0;

static HW_CODEC_IIR_FILTERS_T dac_filtes_l_old;

volatile static int8_t dac_iir_reset_flag;

volatile static int8_t iir_coef_using;

volatile static enum AUD_SAMPRATE_T sample_rate_dac;

static int32 trace_counter=0;

volatile static int32 deq_iir_gaina_ch0;

volatile static int32 deq_iir_gainb_ch0;

HW_CODEC_IIR_CFG_T dac_iir_cfg_new;
HW_CODEC_IIR_CFG_T dac_iir_cfg;

volatile static int max_deq_gain_l=0;

static HWTIMER_ID deq_switching_timer=NULL;
static bool deq_using_flag=0;
static bool deq_reserve_flag=0;
static int deq_switching_coef_delay;

volatile static int codec_dac_flag=0;

#define FIXED_COEF_Q (1<<27)
#define GAIN_Q (9)
/*
static int codec_dac_enable_flag(void)
{
    int rtn_value=0;

    LOG_I("codec_dac_config->codec_dac_lh_en:%d",codec_dac_config->codec_dac_lh_en);
    LOG_I("codec_dac_config->codec_dac_uh_en:%d",codec_dac_config->codec_dac_uh_en);
    LOG_I("trigger_conig->codec_dac_lh_enable_sel:%d",trigger_conig->codec_dac_lh_enable_sel);
    LOG_I("trigger_status->bt_trigger:%d",trigger_status->bt_trigger);
    LOG_I("codec_dac_flag:%d",codec_dac_flag);


    if(hal_cmu_clock_get_status(HAL_CMU_MOD_H_CODEC) == HAL_CMU_CLK_ENABLED
        &&codec_dac_config->codec_dac_lh_en
        &&codec_dac_config->codec_dac_uh_en
        &&codec_dac_flag==1)
    {
        if(trigger_conig->codec_dac_lh_enable_sel==0x0)
        {
            rtn_value=1;
        }
        else if(trigger_conig->codec_dac_lh_enable_sel==0x1)
        {
            // TO BE DONE.
        }
        else if(trigger_conig->codec_dac_lh_enable_sel==0x2)
        {
            if(trigger_status->bt_trigger==0x1)
            {
                rtn_value=1;
            }
        }
    }
    LOG_I("rtn_value:%d",rtn_value);

    return rtn_value;
}
*/
static int iir_eq_filter_para_copy(HW_CODEC_IIR_FILTERS_T *filtes_old,const HW_CODEC_IIR_FILTERS_T *filtes_new,int iir_dac_counter)
{
	filtes_old->iir_bypass_flag=filtes_new->iir_bypass_flag;
	filtes_old->iir_counter=filtes_new->iir_counter;

	int i;

	if(filtes_new->iir_counter>iir_dac_counter)
	{
		LOG_I("%s: warning filtes_new->iir_counter:%d",__func__,filtes_new->iir_counter);
		filtes_old->iir_counter=iir_dac_counter;
	}
	else if(filtes_new->iir_counter<=0)
	{
		LOG_I("%s: warning filtes_new->iir_counter:%d",__func__,filtes_new->iir_counter);
		filtes_old->iir_counter=0;
		filtes_old->iir_bypass_flag=1;
	}

	for(i=0;i<filtes_old->iir_counter;i++)
	{
		filtes_old->iir_coef[i]=filtes_new->iir_coef[i];
	}

	for(;i<iir_dac_counter;i++)
	{
		filtes_old->iir_coef[i].coef_a[0]=FIXED_COEF_Q;
		filtes_old->iir_coef[i].coef_a[1]=0;
		filtes_old->iir_coef[i].coef_a[2]=0;
		filtes_old->iir_coef[i].coef_b[0]=FIXED_COEF_Q;
		filtes_old->iir_coef[i].coef_b[1]=0;
		filtes_old->iir_coef[i].coef_b[2]=0;
	}

	filtes_old->iir_counter=iir_dac_counter;

	return 0;
}

static int iir_eq_filter_coef_copy(volatile struct _iir_coefs *anc_iir_coefs,HW_CODEC_IIR_FILTERS_T *filtes_old)
{
	int i;
    // LOG_I("%s:anc_iir_coefs:0x%x",__func__,(int)anc_iir_coefs);
	for(i=0;i<filtes_old->iir_counter;i++)
	{


		anc_iir_coefs[i].a1=-filtes_old->iir_coef[i].coef_a[1];
		anc_iir_coefs[i].a2=-filtes_old->iir_coef[i].coef_a[2];
		anc_iir_coefs[i].b0=filtes_old->iir_coef[i].coef_b[0];
		anc_iir_coefs[i].b1=filtes_old->iir_coef[i].coef_b[1];
		anc_iir_coefs[i].b2=filtes_old->iir_coef[i].coef_b[2];
        // LOG_I("%s:anc_iir_coefs[i].a1:0x%x",__func__,anc_iir_coefs[i].a1);
        // LOG_I("%s:filtes_old->iir_coef[i].coef_a[1]:0x%x\n",__func__,-filtes_old->iir_coef[i].coef_a[1]);

	}
	return 0;
}
static int deq_gaina_cfg_gain(void)
{
    uint32_t lock;

	LOG_I("%s:deq_iir_gaina_ch0:%d",__func__,deq_iir_gaina_ch0);

#if 0
	if(deq_iir_gaina_ch0==0)deq_iir_gaina_ch0=DEQ_GAIN_RAMP_ZERO;

	anc_iir_gain_update->codec_deq_iir_gain_ext_update_ch0=0;

	*codec_deq_iir_gaina_ext_ch0=deq_iir_gaina_ch0;

	*codec_deq_iir_gainb_ext_ch0=DEQ_GAIN_RAMP_BURST_THRESHLD;

	anc_iir_gain_update->codec_deq_iir_gain_ext_update_ch0=1;

	while((*codec_deq_iir_gainb_ext_out_ch0_sync<=DEQ_GAIN_RAMP_THRESHLD)
	      &&codec_dac_enable_flag())
	{
		LOG_I("%s: *codec_deq_iir_gainb_ext_out_ch0_sync:%d",__func__,
		            *codec_deq_iir_gainb_ext_out_ch0_sync);

		anc_iir_gain_update->codec_deq_iir_gain_ext_update_ch0=0;

		hal_sys_timer_delay_us(1);

		anc_iir_gain_update->codec_deq_iir_gain_ext_update_ch0=1;
	};

	anc_iir_gain_update->codec_deq_iir_gain_ext_update_ch0=0;

	*codec_deq_iir_gainb_ext_ch0=DEQ_GAIN_RAMP_ZERO;

	anc_iir_gain_update->codec_deq_iir_gain_ext_update_ch0=1;
#else
    lock = int_lock();

	anc_iir_gain_update->codec_deq_iir_gain_ext_update_ch0=0;
	//anc_iir_gain_update->codec_deq_iir_gain_ext_update_ch1=0;

	*codec_deq_iir_gaina_ext_ch0=deq_iir_gaina_ch0;
       *codec_deq_iir_gainb_ext_ch0=DEQ_GAIN_RAMP_ZERO;

       //*codec_deq_iir_gaina_ext_ch1=deq_iir_gaina_ch0;
	//*codec_deq_iir_gainb_ext_ch1=DEQ_GAIN_RAMP_ZERO;

	hal_sys_timer_delay_us(2);

	anc_iir_gain_update->codec_deq_iir_gain_ext_update_ch0=1;
	//anc_iir_gain_update->codec_deq_iir_gain_ext_update_ch1=1;
    int_unlock(lock);

#endif

	return 0;
}

static int deq_gainb_cfg_gain(void)
{
    uint32_t lock;

	LOG_I("%s:deq_iir_gainb_ch0:%d",__func__,deq_iir_gainb_ch0);

#if 0
	if(deq_iir_gainb_ch0==0)deq_iir_gainb_ch0=DEQ_GAIN_RAMP_ZERO;

	anc_iir_gain_update->codec_deq_iir_gain_ext_update_ch0=0;
	*codec_deq_iir_gaina_ext_ch0=DEQ_GAIN_RAMP_BURST_THRESHLD;
	*codec_deq_iir_gainb_ext_ch0=deq_iir_gainb_ch0;
	anc_iir_gain_update->codec_deq_iir_gain_ext_update_ch0=1;

	while((*codec_deq_iir_gaina_ext_out_ch0_sync<=DEQ_GAIN_RAMP_THRESHLD)
	      &&codec_dac_enable_flag())
	{
		LOG_I("%s: *codec_deq_iir_gaina_ext_out_ch0_sync:%d",__func__,
		            *codec_deq_iir_gaina_ext_out_ch0_sync);
		anc_iir_gain_update->codec_deq_iir_gain_ext_update_ch0=0;
		hal_sys_timer_delay_us(1);
		anc_iir_gain_update->codec_deq_iir_gain_ext_update_ch0=1;
	};

	anc_iir_gain_update->codec_deq_iir_gain_ext_update_ch0=0;
	*codec_deq_iir_gaina_ext_ch0=DEQ_GAIN_RAMP_ZERO;
	anc_iir_gain_update->codec_deq_iir_gain_ext_update_ch0=1;
#else
    lock = int_lock();

	anc_iir_gain_update->codec_deq_iir_gain_ext_update_ch0=0;
	//anc_iir_gain_update->codec_deq_iir_gain_ext_update_ch1=0;
	*codec_deq_iir_gaina_ext_ch0=DEQ_GAIN_RAMP_ZERO;
	*codec_deq_iir_gainb_ext_ch0=deq_iir_gainb_ch0;
	//*codec_deq_iir_gaina_ext_ch1=DEQ_GAIN_RAMP_ZERO;
	//*codec_deq_iir_gainb_ext_ch1=deq_iir_gainb_ch0;

	hal_sys_timer_delay_us(2);

	anc_iir_gain_update->codec_deq_iir_gain_ext_update_ch0=1;
	//anc_iir_gain_update->codec_deq_iir_gain_ext_update_ch1=1;
    int_unlock(lock);

#endif

	return 0;
}

static int hw_codec_iir_set_dac_cfg_internal(HW_CODEC_IIR_CFG_T *cfg)
{
    HW_CODEC_IIR_ERROR err=HW_CODEC_IIR_NO_ERR;

    const HW_CODEC_IIR_FILTERS_T *dac_filtes_l=NULL;

    LOG_I("%s:",__func__);

    if (dac_open_flag==0)
    {
        LOG_I("%s: dac iir not opened", __func__);
        return HW_CODEC_IIR_TYPE_ERR;
    }

    dac_filtes_l = &(cfg->iir_filtes_l);

#if 0
    LOG_I("eq counter %d", dac_filtes_l->iir_counter);

    for(int j = 0; j <dac_filtes_l->iir_counter ; j++)
    {
       LOG_I("iir coef dac eq l 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x",\
        dac_filtes_l->iir_coef[j].coef_b[0], \
        dac_filtes_l->iir_coef[j].coef_b[1], \
        dac_filtes_l->iir_coef[j].coef_b[2], \
        dac_filtes_l->iir_coef[j].coef_a[0], \
        dac_filtes_l->iir_coef[j].coef_a[1], \
        dac_filtes_l->iir_coef[j].coef_a[2]);
    }
#endif

    iir_eq_filter_para_copy(&dac_filtes_l_old,dac_filtes_l,dac_eq_counter_l);

    max_deq_gain_l=(1<<GAIN_Q);

    if(iir_coef_using==0)
    {
    	 trace_counter=0;

  /*   	while((*codec_deq_iir_gainb_ext_out_ch0_sync>*codec_deq_iir_gain_ext_th)
    	        &&codec_dac_enable_flag())
    	{
    		if(trace_counter%TRACE_COUNTER==0)
    		{

    		        LOG_I("%s: codec_deq_iir_gainb_ext_out_ch0_sync:%d",__func__,
    		                    *codec_deq_iir_gainb_ext_out_ch0_sync);
    		}
    		trace_counter++;
    	};*/
    	deq_iir_d_control->codec_deq_iir_iirb_enable=0;

    	deq_iir_d_control->codec_deq_iir_ch0_bypass=dac_filtes_l_old.iir_bypass_flag;
    	deq_iir_d_control->codec_deq_iir_count_ch0=dac_filtes_l_old.iir_counter;

    	iir_eq_filter_coef_copy(deq_dac_iir_coefs1_l,&dac_filtes_l_old);
    	//iir_eq_filter_coef_copy(deq_dac_iir_coefs1_r,&dac_filtes_l_old);

    	deq_iir_d_control->codec_deq_iir_iirb_enable=1;
 /*   if(!codec_dac_enable_flag())
    	{
    		deq_iir_d_control->codec_deq_iir_iira_enable=0;
    	}
    	*/

    	deq_iir_gaina_ch0=0;
    	deq_iir_gainb_ch0=(int32)(((float)max_deq_gain_l/512.0f)*FIXED_GAIN_RAMP_Q);

    	deq_gainb_cfg_gain();

       iir_coef_using=1;
      }
      else
      {
        	 trace_counter=0;

/*        	while((*codec_deq_iir_gaina_ext_out_ch0_sync>*codec_deq_iir_gain_ext_th)
        	        &&codec_dac_enable_flag())
        	{
        		if(trace_counter%TRACE_COUNTER==0)
        		{

        		        LOG_I("%s: codec_deq_iir_gaina_ext_out_ch0_sync:%d",__func__,
        		                    *codec_deq_iir_gaina_ext_out_ch0_sync);
        		}
        		trace_counter++;
        	};
*/
        	deq_iir_d_control->codec_deq_iir_iira_enable=0;

        	deq_iir_d_control->codec_deq_iir_ch0_bypass=dac_filtes_l_old.iir_bypass_flag;
        	deq_iir_d_control->codec_deq_iir_count_ch0=dac_filtes_l_old.iir_counter;

        	iir_eq_filter_coef_copy(deq_dac_iir_coefs0_l,&dac_filtes_l_old);
        //	iir_eq_filter_coef_copy(deq_dac_iir_coefs0_r,&dac_filtes_l_old);
        	deq_iir_d_control->codec_deq_iir_iira_enable=1;
 /*   if(!codec_dac_enable_flag())
    	{
    		deq_iir_d_control->codec_deq_iir_iirb_enable=0;
    	}
    	*/

    	deq_iir_gaina_ch0=(int32)(((float)max_deq_gain_l/512.0f)*FIXED_GAIN_RAMP_Q);
    	deq_iir_gainb_ch0=0;

    	deq_gaina_cfg_gain();

       iir_coef_using=0;
      }
     max_deq_gain_l=(1<<GAIN_Q);

    return err;
}
static void deq_switching_timer_handler(void *param)
{
    LOG_I("%s", __func__);

    uint32_t lock;
    lock = int_lock();

    bool deq_reserve_old_flag=deq_reserve_flag;

    if(deq_reserve_flag)
    {
        deq_using_flag=1;
        deq_reserve_flag=0;
        dac_iir_cfg=dac_iir_cfg_new;
    }
    else
    {
        deq_using_flag=0;
    }
    int_unlock(lock);

    if(deq_reserve_old_flag)
    {
        hw_codec_iir_set_dac_cfg_internal(&dac_iir_cfg);
        hwtimer_stop(deq_switching_timer);
        hwtimer_start(deq_switching_timer, deq_switching_coef_delay);
    }
    return;
}

int hw_codec_iir_set_cfg(HW_CODEC_IIR_CFG_T *cfg,enum AUD_SAMPRATE_T sample_rate,HW_CODEC_IIR_TYPE_T hw_iir_type)
{
    HW_CODEC_IIR_ERROR err=HW_CODEC_IIR_NO_ERR;
    uint32_t lock;

    LOG_I("%s", __func__);

    if (cfg == NULL)
    {
        LOG_I("%s: cfg is null", __func__);
        return HW_CODEC_IIR_OTHER_ERR;
    }

    if (hw_iir_type!=HW_CODEC_IIR_DAC )
    {
        LOG_I("%s: can't support hw_iir_type=%d.", __func__, hw_iir_type);
        return HW_CODEC_IIR_TYPE_ERR;
    }

    if (hw_iir_type==HW_CODEC_IIR_DAC && dac_open_flag==0)
    {
        LOG_I("%s: hw_iir_type=%d not opened", __func__, hw_iir_type);
        return HW_CODEC_IIR_TYPE_ERR;
    }

    lock = int_lock();
    bool deq_using_old_flag=deq_using_flag;

    dac_iir_cfg_new=*cfg;

    if(deq_using_flag==0)
    {
        deq_using_flag=1;
        deq_reserve_flag=0;
        dac_iir_cfg=dac_iir_cfg_new;
    }
    else
    {
        deq_reserve_flag=1;
    }
    int_unlock(lock);

    if(deq_using_old_flag==0)
    {
        hw_codec_iir_set_dac_cfg_internal(&dac_iir_cfg);
        hwtimer_stop(deq_switching_timer);
        hwtimer_start(deq_switching_timer, deq_switching_coef_delay);
    }
    return err;
}


static void hw_iir_ctrl_reg_init(void)
{
   //disable dac iir;
   deq_iir_d_control->codec_deq_iir_enable=0;
   deq_iir_d_control->codec_deq_iir_iira_enable=0;
   deq_iir_d_control->codec_deq_iir_iirb_enable=0;

   *codec_deq_iir_gain_ext_th=DEQ_GAIN_RAMP_THRESHLD;

    codec_dac_iir_config->codec_dac_L_iir_enable=0;
}


static void hw_iir_ctrl_reg_open( HW_CODEC_IIR_TYPE_T hw_iir_type)
{
    LOG_I("%s", __func__);

    if(hw_iir_type==HW_CODEC_IIR_DAC)
    {
	deq_iir_d_control->codec_deq_iir_enable=0;

	deq_iir_d_control->codec_deq_iir_iira_enable=0;
	deq_iir_d_control->codec_deq_iir_iirb_enable=0;

	deq_iir_d_control->codec_deq_iir_coef_swap=0;
	deq_iir_d_control->codec_deq_iir_auto_stop=0;

	deq_dac_gain_iir_coefs0_l[0].a1=-iir_coef_gain_ramp_deq.coef_a[1];
	deq_dac_gain_iir_coefs0_l[0].a2=-iir_coef_gain_ramp_deq.coef_a[2];
	deq_dac_gain_iir_coefs0_l[0].b0=iir_coef_gain_ramp_deq.coef_b[0];
	deq_dac_gain_iir_coefs0_l[0].b1=iir_coef_gain_ramp_deq.coef_b[1];
	deq_dac_gain_iir_coefs0_l[0].b2=iir_coef_gain_ramp_deq.coef_b[2];

	deq_dac_gain_iir_coefs1_l[0].a1=-iir_coef_gain_ramp_deq.coef_a[1];
	deq_dac_gain_iir_coefs1_l[0].a2=-iir_coef_gain_ramp_deq.coef_a[2];
	deq_dac_gain_iir_coefs1_l[0].b0=iir_coef_gain_ramp_deq.coef_b[0];
	deq_dac_gain_iir_coefs1_l[0].b1=iir_coef_gain_ramp_deq.coef_b[1];
	deq_dac_gain_iir_coefs1_l[0].b2=iir_coef_gain_ramp_deq.coef_b[2];

/*	deq_dac_gain_iir_coefs0_r[0].a1=-iir_coef_gain_ramp_deq.coef_a[1];
	deq_dac_gain_iir_coefs0_r[0].a2=-iir_coef_gain_ramp_deq.coef_a[2];
	deq_dac_gain_iir_coefs0_r[0].b0=iir_coef_gain_ramp_deq.coef_b[0];
	deq_dac_gain_iir_coefs0_r[0].b1=iir_coef_gain_ramp_deq.coef_b[1];
	deq_dac_gain_iir_coefs0_r[0].b2=iir_coef_gain_ramp_deq.coef_b[2];

	deq_dac_gain_iir_coefs1_r[0].a1=-iir_coef_gain_ramp_deq.coef_a[1];
	deq_dac_gain_iir_coefs1_r[0].a2=-iir_coef_gain_ramp_deq.coef_a[2];
	deq_dac_gain_iir_coefs1_r[0].b0=iir_coef_gain_ramp_deq.coef_b[0];
	deq_dac_gain_iir_coefs1_r[0].b1=iir_coef_gain_ramp_deq.coef_b[1];
	deq_dac_gain_iir_coefs1_r[0].b2=iir_coef_gain_ramp_deq.coef_b[2];
*/

	deq_iir_d_control->codec_deq_iir_gaincal_ext_ch0_bypass=0;
	deq_iir_d_control->codec_deq_iir_gainuse_ext_ch0_bypass=0;
	anc_iir_gain_update->codec_deq_iir_gain_ext_sel_ch0=0;

	for(int i=0;i<dac_eq_counter_l;i++)
	{
		deq_dac_iir_coefs0_l[i].a1=0;
		deq_dac_iir_coefs0_l[i].a2=0;
		deq_dac_iir_coefs0_l[i].b0=0;
		deq_dac_iir_coefs0_l[i].b1=0;
		deq_dac_iir_coefs0_l[i].b2=0;

		deq_dac_iir_coefs1_l[i].a1=0;
		deq_dac_iir_coefs1_l[i].a2=0;
		deq_dac_iir_coefs1_l[i].b0=0;
		deq_dac_iir_coefs1_l[i].b1=0;
		deq_dac_iir_coefs1_l[i].b2=0;

/*		deq_dac_iir_coefs0_r[i].a1=0;
		deq_dac_iir_coefs0_r[i].a2=0;
		deq_dac_iir_coefs0_r[i].b0=0;
		deq_dac_iir_coefs0_r[i].b1=0;
		deq_dac_iir_coefs0_r[i].b2=0;

		deq_dac_iir_coefs1_r[i].a1=0;
		deq_dac_iir_coefs1_r[i].a2=0;
		deq_dac_iir_coefs1_r[i].b0=0;
		deq_dac_iir_coefs1_r[i].b1=0;
		deq_dac_iir_coefs1_r[i].b2=0;
*/
	}

	deq_iir_d_control->codec_deq_iir_ch0_bypass=0;
	codec_dac_iir_config->codec_dac_L_iir_enable=1;

	deq_iir_d_control->codec_deq_iir_count_ch0=dac_eq_counter_l;

    	deq_iir_d_control->codec_deq_iir_ch1_bypass=1;
    	deq_iir_d_control->codec_deq_iir_count_ch1=0;

	deq_iir_d_control->codec_deq_iir_gaincal_ext_ch1_bypass=1;
	deq_iir_d_control->codec_deq_iir_gainuse_ext_ch1_bypass=1;
	anc_iir_gain_update->codec_deq_iir_gain_ext_sel_ch1=0;

	deq_iir_d_control->codec_deq_iir_auto_stop=1;
	deq_iir_d_control->codec_deq_iir_iira_enable=1;
	deq_iir_d_control->codec_deq_iir_iirb_enable=1;
	deq_iir_d_control->codec_deq_iir_enable=1;

	iir_coef_using=0;
	deq_iir_gaina_ch0=DEQ_GAIN_RAMP_ZERO;
	deq_iir_gainb_ch0=DEQ_GAIN_RAMP_ZERO;
	max_deq_gain_l=0;

       deq_using_flag=0;
       deq_reserve_flag=0;
       codec_dac_flag=0;
       deq_switching_coef_delay=DEQ_SET_GAIN_TIME;

       deq_gaina_cfg_gain();
    }
}

static void hw_iir_ctrl_reg_close(HW_CODEC_IIR_TYPE_T hw_iir_type)
{
    if(hw_iir_type==HW_CODEC_IIR_DAC)
    {
       //disable dac iir;
	deq_iir_d_control->codec_deq_iir_iira_enable=0;
	deq_iir_d_control->codec_deq_iir_iirb_enable=0;
	deq_iir_d_control->codec_deq_iir_enable=0;

	codec_dac_iir_config->codec_dac_L_iir_enable=0;
    }
}

int hw_codec_iir_open(enum AUD_SAMPRATE_T sample_rate,  HW_CODEC_IIR_TYPE_T hw_iir_type,int32_t ch_map)
{
    HW_CODEC_IIR_ERROR err=HW_CODEC_IIR_NO_ERR;

    LOG_I("%s:sample_rate:%d", __func__,sample_rate);

    uint32_t metal_id;

    metal_id = hal_get_chip_metal_id();

    if (metal_id >= HAL_CHIP_METAL_ID_4)
    {
        dac_eq_counter_l=IIR_DAC_COUNTER_L_MAX;
    }

    LOG_I("%s:dac_eq_counter:%d", __func__,dac_eq_counter_l);

    if(dac_open_flag==0)
    {
        if(sample_rate<=AUD_SAMPRATE_50781)
        {
            if(dac_eq_counter_l>34)
            {
                hal_codec_iir_eq_enable(24000000);
            }
            else
            {
                hal_codec_iir_eq_enable(24000000/2);
            }
        }
        else if(sample_rate<=AUD_SAMPRATE_101562)
        {
            if(dac_eq_counter_l>34)
            {
                hal_codec_iir_eq_enable(24000000*2);
            }
            else
            {
                hal_codec_iir_eq_enable(24000000);
            }        }
        else if(sample_rate<=AUD_SAMPRATE_203125)
        {
            if(dac_eq_counter_l>34)
            {
                hal_codec_iir_eq_enable(24000000*4);
            }
            else
            {
                hal_codec_iir_eq_enable(24000000*2);
            }        }
        else
        {
            if(dac_eq_counter_l>34)
            {
                hal_codec_iir_eq_enable(24000000*8);
            }
            else
            {
                hal_codec_iir_eq_enable(24000000*4);
            }
        }

        hw_iir_ctrl_reg_init();

        iir_coef_using=0;
        sample_rate_dac=AUD_SAMPRATE_NULL;
    }

    if(deq_switching_timer==NULL)
    {
        deq_switching_timer = hwtimer_alloc(deq_switching_timer_handler, NULL);
        ASSERT(deq_switching_timer, "Failed to alloc deq_switching_timer");
    }

    if(hw_iir_type==HW_CODEC_IIR_DAC)
    {
        dac_iir_reset_flag=0;
        dac_open_flag=1;
        sample_rate_dac=sample_rate;

        //clear iir_counter;
        dac_filtes_l_old.iir_counter=0;
        dac_filtes_l_old.iir_bypass_flag=1;
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
        hwtimer_stop(deq_switching_timer);
        hal_codec_iir_eq_disable();
    }

    return err;
}


int hw_codec_iir_set_coefs(HW_CODEC_IIR_CFG_F *cfg, HW_CODEC_IIR_TYPE_T hw_iir_type)
{
    HW_CODEC_IIR_ERROR err=HW_CODEC_IIR_NO_ERR;

    HW_CODEC_IIR_CFG_T *hw_iir_cfg = hw_codec_iir_convert_cfg(cfg);

    err = hw_codec_iir_set_cfg(hw_iir_cfg, AUD_SAMPRATE_NULL, hw_iir_type);

    return err;
}

void hw_codec_iir_enable(HW_CODEC_IIR_TYPE_T hw_iir_type, int flag)
{
    if(hw_iir_type==HW_CODEC_IIR_DAC)
    {
       //disable dac iir;
	codec_dac_iir_config->codec_dac_L_iir_enable=flag;
    }
}
/*
void hw_codec_iir_run(void)
{
    codec_dac_flag=1;
}*/
