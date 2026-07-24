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
1	dc	codec_iir_enable	RW	0	0	anc iir module enable
1	dc	codec_iir_ch0_bypass	RW	0	1	1: ch0 bypass
1	dc	codec_iir_ch1_bypass	RW	0	2
1	dc	codec_iir_ch2_bypass	RW	0	3
1	dc	codec_iir_ch3_bypass	RW	0	4
4	dc	codec_iir_count_ch0	RW	0	5	ch0 iir number  0~8
4	dc	codec_iir_count_ch1	RW	0	9
4	dc	codec_iir_count_ch2	RW	0	13
4	dc	codec_iir_count_ch3	RW	0	17
1	dc	codec_dac_L_iir_enable	RW	0	21	1: use iir ch0 for dac L eq
1	dc	codec_dac_R_iir_enable	RW	0	22	1: use iir ch1 for dac R eq
1	dc	codec_adc_ch0_iir_enable	RW	0	23	1: use iir ch2 for adc some channel eq;  adc ch0~ch4 use resample adc ch0 select
1	dc	codec_adc_ch1_iir_enable	RW	0	24	1: use iir ch3 for adc some channel eq;  adc ch0~ch4 use resample adc ch1 select
1	dc	codec_iir_coef_swap	RW	0	25	0: use iir coef memory0;  1: use iir coef memory1;
1	dc	codec_iir_coef_swap_status	R	0	26	read only;  coef memoryX used currently
27	dc		x
*/
struct _anc_iir_control
{
	uint32 codec_iir_enable : 1;

	uint32 codec_iir_ch0_bypass : 1;
	uint32 codec_iir_ch1_bypass : 1;
	uint32 codec_iir_ch2_bypass : 1;
	uint32 codec_iir_ch3_bypass : 1;

	uint32 codec_iir_count_ch0 : 4;
	uint32 codec_iir_count_ch1 : 4;
	uint32 codec_iir_count_ch2 : 4;
	uint32 codec_iir_count_ch3 : 4;

    uint32 codec_dac_L_iir_enable : 1;
    uint32 codec_dac_R_iir_enable : 1;
    uint32 codec_adc_ch0_iir_enable : 1;
    uint32 codec_adc_ch1_iir_enable : 1;
    uint32 codec_iir_coef_swap : 1;
    uint32 codec_iir_coef_swap_status : 1;

	uint32 reserved : 5;
};

/*
1	d0	codec_anc_enable_ch0	RW	0	0	anc enable
1	d0	codec_anc_enable_ch1	RW	0	1
1	d0	codec_dual_anc_ch0	RW	0	2	FF + FB
1	d0	codec_dual_anc_ch1	RW	0	3
1	d0	codec_anc_mute_ch0	RW	0	4	anc output to 0
1	d0	codec_anc_mute_ch1	RW	0	5
1	d0	codec_ff_ch0_fir_en	RW	0	6	fir enable
1	d0	codec_ff_ch1_fir_en	RW	0	7
1	d0	codec_fb_ch0_fir_en	RW	0	8
1	d0	codec_fb_ch1_fir_en	RW	0	9
1	d0	codec_anc_rate_sel	RW	0	10	  0: 384k anc;  1: 768k anc;
2	d0	codec_anc_ff_sr_sel	RW	0	11	  0: 384k;  1: 192k;  2: 96k;  3:48k;
3	d0	codec_anc_ff_in_phase_sel	RW	0	13	  8 phase
2	d0	codec_anc_fb_sr_sel	RW	0	16	  0: 384k;  1: 192k;  2: 96k;  3:48k;
3	d0	codec_anc_fb_in_phase_sel	RW	0	18	  8 phase
1	d0	codec_feedback_ch0	RW	0	21	feedback mode
1	d0	codec_feedback_ch1	RW	0	22
1	d0	codec_adc_fir_ds_en_ch2	RW	0	23	1: adc ch2 192k/384k to 48k downsample use fir
1	d0	codec_adc_fir_ds_sel_ch2	RW	0	24	1: adc ch2 384k to 48k;  0: adc ch2 192k to 48k
1	d0	codec_adc_fir_ds_en_ch3	RW	0	25
1	d0	codec_adc_fir_ds_sel_ch3	RW	0	26
27	d0		x
*/
struct _anc_control
{
	uint32 codec_anc_enable_ch0 : 1;
	uint32 codec_anc_enable_ch1 : 1;

	uint32 codec_dual_anc_ch0 : 1;
	uint32 codec_dual_anc_ch1 : 1;

	uint32 codec_anc_mute_ch0 : 1;
	uint32 codec_anc_mute_ch1 : 1;

	uint32 codec_ff_ch0_fir_en : 1;
	uint32 codec_ff_ch1_fir_en : 1;
	uint32 codec_fb_ch0_fir_en : 1;
	uint32 codec_fb_ch1_fir_en : 1;

	uint32 codec_anc_rate_sel : 1;

	uint32 codec_anc_ff_sr_sel : 2;
	uint32 codec_anc_ff_in_phase_sel : 3;
	uint32 codec_anc_fb_sr_sel : 2;
	uint32 codec_anc_fb_in_phase_sel : 3;

    uint32 codec_feedback_ch0 : 1;
	uint32 codec_feedback_ch1 : 1;

    uint32 Reserved : 9;
};

/*
12	d4	codec_anc_mute_gain_ff_ch0	RW	0	0	Format 3.9, for FF
12	d4	codec_anc_mute_gain_ff_ch1	RW	0	12
1	d4	codec_anc_mute_gain_pass0_ff_ch0	RW	0	24	1: gain update no pass0
1	d4	codec_anc_mute_gain_pass0_ff_ch1	RW	0	25
26	d4		x
*/
struct _anc_ff_gain
{
	int32 codec_anc_mute_gain_ff_ch0 : 12;
	int32 codec_anc_mute_gain_ff_ch1 : 12;

	int32 codec_anc_mute_gain_pass0_ff_ch0 : 1;
	int32 codec_anc_mute_gain_pass0_ff_ch1 : 1;

	uint32 Reserved : 6;
};

/*
12	d8	codec_anc_mute_gain_fb_ch0	RW	0	0	same as above
12	d8	codec_anc_mute_gain_fb_ch1	RW	0	12
1	d8	codec_anc_mute_gain_pass0_fb_ch0	RW	0	24
1	d8	codec_anc_mute_gain_pass0_fb_ch1	RW	0	25
26	d8		x
*/
struct _anc_fb_gain
{
	int32 codec_anc_mute_gain_fb_ch0 : 12;
	int32 codec_anc_mute_gain_fb_ch1 : 12;

	int32 codec_anc_mute_gain_pass0_fb_ch0 : 1;
	int32 codec_anc_mute_gain_pass0_fb_ch1 : 1;

	uint32 Reserved : 6;
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

/*
1	100	fir_stream_enable_ch0	RW	0	0	1: used for anc fir or adc downsample fir
1	100	fir_stream_enable_ch1	RW	0	1
1	100	fir_stream_enable_ch2	RW	0	2
1	100	fir_stream_enable_ch3	RW	0	3
1	100	fir_enable_ch0	RW	0	4	1: used for memory to memory fir
1	100	fir_enable_ch1	RW	0	5
1	100	fir_enable_ch2	RW	0	6
1	100	fir_enable_ch3	RW	0	7
1	100	dma_ctrl_rx_fir	RW	0	8
1	100	dma_ctrl_tx_fir	RW	0	9
1	100	fir_upsample_ch0	RW	0	10	1: use for memory to memory fir upsample
1	100	fir_upsample_ch1	RW	0	11
1	100	fir_upsample_ch2	RW	0	12
1	100	fir_upsample_ch3	RW	0	13
1	100	mode_32bit_fir	RW	0	14	 all coef/sample to memory; not used together with any 16 bit mode
1	100	fir_reserved_reg0	RW	0	15	not used
1	100	mode_16bit_fir_tx_ch0	RW	0	16	 sample memory write as 16bit mode
1	100	mode_16bit_fir_rx_ch0	RW	0	17	 coef memory read as 16bit mode
1	100	mode_16bit_fir_tx_ch1	RW	0	18
1	100	mode_16bit_fir_rx_ch1	RW	0	19
1	100	mode_16bit_fir_tx_ch2	RW	0	20
1	100	mode_16bit_fir_rx_ch2	RW	0	21
1	100	mode_16bit_fir_tx_ch3	RW	0	22
1	100	mode_16bit_fir_rx_ch3	RW	0	23
24	100		x
*/
struct _fir_control
{
	uint32 fir_stream_enable_ch0 : 1;
	uint32 fir_stream_enable_ch1 : 1;
	uint32 fir_stream_enable_ch2 : 1;
	uint32 fir_stream_enable_ch3 : 1;

	uint32 Reserved : 28;
};

/*
1	108	stream0_fir1_ch0	RW	0	0	0: stream mode; 1: fir mode
2	108	fir_mode_ch0	RW	0	1	fir operation mode;  0: normal fir;   1: synthesis1;  2: synthesis2;
10	108	fir_order_ch0	RW	0	3
9	108	fir_sample_start_ch0	RW	0	13
9	108	fir_sample_num_ch0	RW	0	22
1	108	fir_do_remap_ch0	RW	0	31
32	108		x
*/
struct _fir_ch_config0
{
	uint32 stream0_fir1 : 1;
	uint32 fir_mode : 2;
	uint32 fir_order : 10;
	uint32 fir_sample_start : 9;
	uint32 fir_sample_num : 9;
	uint32 Reserved : 1;
};

/*
9	10c	fir_result_base_addr_ch0	RW	0	0
6	10c	fir_slide_offset_ch0	RW	0	9
6	10c	fir_burst_length_ch0	RW	0	15
4	10c	fir_gain_sel_ch0	RW	0	21
7	10c	fir_loop_num_ch0	RW	0	25
32	10c		x
*/
struct _fir_ch_config1
{
	uint32_t fir_result_base_addr : 9;
	uint32_t fir_slide_offset : 6;
	uint32_t fir_burst_length : 6;
	uint32_t fir_gain_sel : 4;
	uint32_t fir_loop_num : 7;
};



/*
1	bc	codec_adc_mc_en_ch0	RW	0	0	adc ch0 music cancel enable
1	bc	codec_adc_mc_en_ch1	RW	0	1	adc ch1 music cancel enable
1	bc	codec_feedback_mc_en_ch0	RW	0	2	feedback music cancel enable
1	bc	codec_feedback_mc_en_ch1	RW	0	3
4	bc		x
*/
struct _feedback_mc_settings
{
    uint32 codec_adc_mc_en_ch0 : 1;
    uint32 codec_adc_mc_en_ch1 : 1;

    uint32 codec_feedback_mc_en_ch0 : 1;
    uint32 codec_feedback_mc_en_ch1 : 1;

    uint32 Reserved : 28;
};


#if defined(AUDIO_ANC_FB_MC)||defined(ANC_FB_CHECK)||defined(AUDIO_ANC_FB_MC_HW)

/*
1	130	codec_fb_check_enable_ch0	RW	0	0
2	130	codec_fb_check_acc_sample_rate_ch0	RW	0	1
1	130	codec_fb_check_src_sel_ch0	RW	0	3
1	130	codec_fb_check_keep_sel_ch0	RW	0	4
12	130	codec_fb_check_acc_window_ch0	RW	0	5
10	130	codec_fb_check_trig_window_ch0	RW	0	17
1	130	codec_fb_check_keep_ch0	RW	0	27
28	130		x
*/
struct _fb_check_ch0_config
{
	uint32 codec_fb_check_enable_ch0 : 1;

	uint32 codec_fb_check_acc_sample_rate_ch0 : 2;

	uint32 codec_fb_check_src_sel_ch0 : 1;

       uint32 codec_fb_check_keep_sel_ch0: 1;

	uint32 codec_fb_check_acc_window_ch0 : 12;

   	uint32 codec_fb_check_trig_window_ch0 : 10;

   	uint32 codec_fb_check_keep_ch0 : 1;	

    uint32 Reserved : 4;
};

/*
1	134	codec_fb_check_enable_ch1	RW	0	0
2	134	codec_fb_check_acc_sample_rate_ch1	RW	0	1
1	134	codec_fb_check_src_sel_ch1	RW	0	3
1	134	codec_fb_check_keep_sel_ch1	RW	0	4
12	134	codec_fb_check_acc_window_ch1	RW	0	5
10	134	codec_fb_check_trig_window_ch1	RW	0	17
1	134	codec_fb_check_keep_ch1	RW	0	27
28	134		x
*/

struct _fb_check_ch1_config
{
	uint32 codec_fb_check_enable_ch1 : 1;

	uint32 codec_fb_check_acc_sample_rate_ch1 : 2;

	uint32 codec_fb_check_src_sel_ch1 : 1;

    uint32 codec_fb_check_keep_sel_ch1: 1;

	uint32 codec_fb_check_acc_window_ch1 : 12;

   	uint32 codec_fb_check_trig_window_ch1 : 10;

   	uint32 codec_fb_check_keep_ch1 : 1;		

    uint32 Reserved : 4;
};
#endif

#ifdef ANC_FB_CHECK
/*
5	0c	codec_rx_overflow	RW	0	0	interrupt status which not been masked.
5	0c	codec_rx_underflow	RW	0	5
1	0c	codec_tx_overflow	RW	0	10
1	0c	codec_tx_underflow	RW	0	11
1	0c	dsd_rx_overflow	RW	0	12
1	0c	dsd_rx_underflow	RW	0	13
1	0c	dsd_tx_overflow	RW	0	14
1	0c	dsd_tx_underflow	RW	0	15
1	0c	mc_overflow	RW	0	16
1	0c	mc_underflow	RW	0	17
1	0c	iir_rx_overflow	RW	0	18
1	0c	iir_rx_underflow	RW	0	19
1	0c	iir_tx_overflow	RW	0	20
1	0c	iir_tx_underflow	RW	0	21
1	0c	event_trigger	RW	0	22
1	0c	fb_check_error_trig_ch0	RW	0	23
1	0c	fb_check_error_trig_ch1	RW	0	24
1	0c	vad_find	RW	0	25
1	0c	vad_not_find	RW	0	26
1	0c	bt_trigger	RW	0	27
1	0c	adc_max_overflow	RW	0	28
29	0c		x
*/
struct _codec_int_config
{
	uint32 Reserved_notused : 23;

	uint32 fb_check_error_trig_ch0 : 1;
	uint32 fb_check_error_trig_ch1 : 1;

    uint32 Reserved : 7;
};
/*
5	10	codec_rx_overflow_mask	RW	0	0	interrupt mask; 0 will mask interrupt
5	10	codec_rx_underflow_mask	RW	0	5
1	10	codec_tx_overflow_mask	RW	0	10
1	10	codec_tx_underflow_mask	RW	0	11
1	10	dsd_rx_overflow_mask	RW	0	12
1	10	dsd_rx_underflow_mask	RW	0	13
1	10	dsd_tx_overflow_mask	RW	0	14
1	10	dsd_tx_underflow_mask	RW	0	15
1	10	mc_overflow_mask	RW	0	16
1	10	mc_underflow_mask	RW	0	17
1	10	iir_rx_overflow_mask	RW	0	18
1	10	iir_rx_underflow_mask	RW	0	19
1	10	iir_tx_overflow_mask	RW	0	20
1	10	iir_tx_underflow_mask	RW	0	21
1	10	event_trigger_mask	RW	0	22
1	10	fb_check_error_trig_ch0_mask	RW	0	23
1	10	fb_check_error_trig_ch1_mask	RW	0	24
1	10	vad_find_mask	RW	0	25
1	10	vad_not_find_mask	RW	0	26
1	10	bt_trigger_mask	RW	0	27
1	10	adc_max_overflow_mask	RW	0	28
29	10		x

*/
struct _codec_mask_config
{
	uint32 Reserved_notused : 23;

	uint32 fb_check_error_trig_ch0_mask : 1;
	uint32 fb_check_error_trig_ch1_mask : 1;

    uint32 Reserved : 7;
};
#endif


#if defined(AUDIO_ANC_TT_HW) ||defined(AUDIO_ANC_FB_MC_HW)
/*
1	22c	codec_tt_enable	RW	0	0	��talk through enable
12	22c	codec_mute_gain_coef_tt	RW	0	1	��talk through gain
1	22c	codec_mute_gain_pass0_tt	RW	0	13	��talk through gain update pass0. 1: update always; 0: update pass0
1	22c	codec_mm_enable	RW	0	14	��music cancel enable
12	22c	codec_mute_gain_coef_mm	RW	0	15	��music cancel gain
1	22c	codec_mute_gain_pass0_mm	RW	0	27	��music cancel gain update pass0. 1: update always; 0: update pass0
28	22c	��	x	��	��	��
*/
struct _tt_mc_control
{
    uint32 codec_tt_enable : 1;
    uint32 codec_mute_gain_coef_tt : 12;
    uint32 codec_mute_gain_pass0_tt : 1;

    uint32 codec_mm_enable : 1;
    uint32 codec_mute_gain_coef_mm : 12;
    uint32 codec_mute_gain_pass0_mm : 1;

    uint32 Reserved : 4;
};
#endif

#if defined(AUDIO_ANC_FB_MC_HW)
/*
1	230	codec_mm_fifo_en	RW	0	0	��music cancel fifo enable, for delay
1	230	codec_mm_fifo_bypass	RW	0	1	   music cancel fifo bypass. 1: bypass fifo, means no delay. 0: no bypass
1	230	codec_mm_delay_update	RW	0	2	��music cancel delay update.
5	230	codec_mm_delay	RW	0	3	��music cancel delay, in 384k samples. at most 31 sample, about 80us
*/

struct _mc_config
{
    uint32 codec_mm_fifo_en : 1;
    uint32 codec_mm_fifo_bypass : 1;
    uint32 codec_mm_delay_update : 1;
    uint32 codec_mm_delay : 5;

    uint32 Reserved : 24;
};

#endif

#define IIR_COUNTER (8)
#define FIR_LEN (500)


typedef struct _iir_parameter
{
    int32_t total_gain;

    uint16_t iir_bypass_flag;
    uint16_t iir_counter;

    anc_iir_coefs iir_coef[IIR_COUNTER];

} iir_parameter;


#define GAIN_Q (9)

/*******************************data struction***********************************/

#define ANC_BASE                            ((uint32)CODEC_BASE)

//ANC registers
volatile static struct _anc_control *anc_control=(volatile struct _anc_control *)(ANC_BASE+0xd0);
volatile static struct _anc_ff_gain *anc_ff_gain=(volatile struct _anc_ff_gain *)(ANC_BASE+0xd4);
volatile static struct _anc_fb_gain *anc_fb_gain=(volatile struct _anc_fb_gain *)(ANC_BASE+0xd8);

//IIR registers
volatile static struct _anc_iir_control *anc_iir_control=(volatile struct _anc_iir_control *)(ANC_BASE+0xdc);

volatile static struct _anc_iir_coefs *anc_ff_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x2000);
volatile static struct _anc_iir_coefs *anc_ff_iir_coefs0_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x20a0);
volatile static struct _anc_iir_coefs *anc_fb_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x2140);
volatile static struct _anc_iir_coefs *anc_fb_iir_coefs0_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x21e0);

volatile static struct _anc_iir_coefs *anc_ff_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x2400);
volatile static struct _anc_iir_coefs *anc_ff_iir_coefs1_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x24a0);
volatile static struct _anc_iir_coefs *anc_fb_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x2540);
volatile static struct _anc_iir_coefs *anc_fb_iir_coefs1_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x25e0);

//TT and MC registers
#if defined(AUDIO_ANC_TT_HW)|| defined(AUDIO_ANC_FB_MC_HW)
volatile static struct _tt_mc_control *tt_mc_control=(volatile struct _tt_mc_control *)(ANC_BASE+0x22c);
#endif

#if defined(AUDIO_ANC_FB_MC_HW)
volatile static struct _mc_config *mc_config=(volatile struct _mc_config *)(ANC_BASE+0x230);
#endif

#if (AUD_SECTION_STRUCT_VERSION == 1)
//FIR registers
volatile static struct _fir_control *fir_control=(volatile struct _fir_control *)(ANC_BASE+0x100);

volatile static int32 *anc_ff_fir_coefs_l=(volatile  int32 *)(ANC_BASE+0x9000);
volatile static int32 *anc_ff_fir_coefs_r=(volatile  int32 *)(ANC_BASE+0xb000);
volatile static int32 *anc_fb_fir_coefs_l=(volatile  int32 *)(ANC_BASE+0xd000);
volatile static int32 *anc_fb_fir_coefs_r=(volatile  int32 *)(ANC_BASE+0xf000);

volatile static struct _fir_ch_config0 *fir_ff_config0_l=(volatile struct _fir_ch_config0 *)(ANC_BASE+0x108);
volatile static struct _fir_ch_config0 *fir_ff_config0_r=(volatile struct _fir_ch_config0 *)(ANC_BASE+0x110);
volatile static struct _fir_ch_config0 *fir_fb_config0_l=(volatile struct _fir_ch_config0 *)(ANC_BASE+0x118);
volatile static struct _fir_ch_config0 *fir_fb_config0_r=(volatile struct _fir_ch_config0 *)(ANC_BASE+0x120);

volatile static struct _fir_ch_config1 *fir_ff_config1_l=(volatile struct _fir_ch_config1 *)(ANC_BASE+0x10c);
volatile static struct _fir_ch_config1 *fir_ff_config1_r=(volatile struct _fir_ch_config1 *)(ANC_BASE+0x114);
volatile static struct _fir_ch_config1 *fir_fb_config1_l=(volatile struct _fir_ch_config1 *)(ANC_BASE+0x11c);
volatile static struct _fir_ch_config1 *fir_fb_config1_r=(volatile struct _fir_ch_config1 *)(ANC_BASE+0x124);

#endif

#if defined(AUDIO_ANC_FB_MC)||defined(ANC_FB_CHECK)||defined(AUDIO_ANC_FB_MC_HW)
//feedback registers
volatile static struct _feedback_mc_settings *feedback_mc_settings=(volatile struct _feedback_mc_settings *)(ANC_BASE+0xbc);

volatile static struct _fb_check_ch0_config *fb_check_ch0_config=(volatile struct _fb_check_ch0_config *)(ANC_BASE+0x0130);
volatile static struct _fb_check_ch1_config *fb_check_ch1_config=(volatile struct _fb_check_ch1_config *)(ANC_BASE+0x0134);
#endif

#ifdef ANC_FB_CHECK
volatile static uint32 *codec_fb_check_threshold_ch0=(volatile uint32 *)(ANC_BASE+0x0138);
volatile static uint32 *codec_fb_check_threshold_ch1=(volatile uint32 *)(ANC_BASE+0x013c);

volatile static uint32 *codec_fb_check_data_avg_keep_ch0=(volatile uint32 *)(ANC_BASE+0x0140);
volatile static uint32 *codec_fb_check_data_avg_keep_ch1=(volatile uint32 *)(ANC_BASE+0x0144);

volatile static uint32 *codec_int_config_clr=(volatile uint32 *)(ANC_BASE+0x0c);
volatile static struct _codec_mask_config *codec_mask_config=(volatile struct _codec_mask_config *)(ANC_BASE+0x10);

#define CODEC_FB_CHECK_ERROR_TRIG_CH0            (1 << 23)
#define CODEC_FB_CHECK_ERROR_TRIG_CH1            (1 << 24)

#endif

volatile static int max_ff_gain_l=0;
volatile static int max_ff_gain_r=0;

volatile static int max_fb_gain_l=0;
volatile static int max_fb_gain_r=0;

#if defined(AUDIO_ANC_TT_HW)
volatile static int max_tt_gain_l=0;
#endif

#if defined(AUDIO_ANC_FB_MC_HW)
volatile static int max_mc_gain_l=0;
#endif

volatile static int ff_open_flag=0;
volatile static int fb_open_flag=0;

volatile static int anc_output_ch_map=AUD_CHANNEL_MAP_CH0|AUD_CHANNEL_MAP_CH1;

#ifdef ANC_FB_CHECK
static HWTIMER_ID fb_adc_check_dev_timer=NULL;
#endif

static iir_parameter ff_filtes_l_old;
static iir_parameter ff_filtes_r_old;
static iir_parameter fb_filtes_l_old;
static iir_parameter fb_filtes_r_old;

#if defined(AUDIO_ANC_TT_HW) ||defined(AUDIO_ANC_FB_MC_HW)
static iir_parameter tt_filtes_l_old;
static iir_parameter mc_filtes_l_old;
#endif

volatile static int8_t ff_iir_reset_flag;
volatile static int8_t fb_iir_reset_flag;

#if defined(AUDIO_ANC_TT_HW)
volatile static int8_t tt_iir_reset_flag;
#endif

#if defined(AUDIO_ANC_FB_MC_HW)
volatile static int8_t mc_iir_reset_flag;
#endif

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
    int i;
    const aud_item *ff_filtes_l;
    const aud_item *ff_filtes_r;
    const aud_item *fb_filtes_l;
    const aud_item *fb_filtes_r;
#if defined(AUDIO_ANC_TT_HW)
    const aud_item *tt_filtes_l;
#endif

#if defined(AUDIO_ANC_FB_MC_HW)
    const aud_item *mc_filtes_l;
#endif


    int total_iir_counter=0;

    if (cfg == NULL)
    {
        LOG_I("%s: cfg is null", __func__);
        return ANC_OTHER_ERR;
    }

    if ((anc_type==ANC_FEEDFORWARD && ff_open_flag==0) ||
            (anc_type==ANC_FEEDBACK && fb_open_flag==0)) {
        LOG_I("%s: anc_type=%d not opened", __func__, anc_type);
        return ANC_OTHER_ERR;
    }

    LOG_I("%s: iir_coef_using:%d,Iir_coef_swap:%d", __func__,iir_coef_using,anc_iir_control->codec_iir_coef_swap);

    ff_filtes_l = &(cfg->anc_cfg_ff_l);
    fb_filtes_l = &(cfg->anc_cfg_fb_l);

#if (AUD_SECTION_STRUCT_VERSION == 3)
    ff_filtes_r = &(cfg->anc_cfg_ff_l);
    fb_filtes_r = &(cfg->anc_cfg_fb_l);
#else
    ff_filtes_r = &(cfg->anc_cfg_ff_r);
    fb_filtes_r = &(cfg->anc_cfg_fb_r);
#endif

#if defined(AUDIO_ANC_TT_HW)
    tt_filtes_l = &(cfg->anc_cfg_tt_l);
#endif

#if defined(AUDIO_ANC_FB_MC_HW)
    mc_filtes_l = &(cfg->anc_cfg_mc_l);
#endif

//    LOG_I("ff_l:%d,ff_r:%d,fb_l:%d,fb_r:%d",ff_filtes_l->total_gain,ff_filtes_r->total_gain,fb_filtes_l->total_gain,fb_filtes_r->total_gain);

#ifdef ANC_PROD_TEST
       LOG_I("ff gain %d,fb counter %d",ff_filtes_l->total_gain, ff_filtes_l->iir_counter);
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

       LOG_I("fb gain %d,fb counter %d",fb_filtes_l->total_gain, fb_filtes_l->iir_counter);
	for(int j = 0; j <IIR_COUNTER; j++)
	{
		//LOG_I("iir coef ff r 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x",
		LOG_I("iir coef ff r %10d, %10d, %10d, %10d, %10d, %10d",  \
		fb_filtes_l->iir_coef[j].coef_b[0], \
		fb_filtes_l->iir_coef[j].coef_b[1], \
		fb_filtes_l->iir_coef[j].coef_b[2], \
		fb_filtes_l->iir_coef[j].coef_a[0], \
		fb_filtes_l->iir_coef[j].coef_a[1], \
		fb_filtes_l->iir_coef[j].coef_a[2]);
	}
#if defined(AUDIO_ANC_TT_HW)
       LOG_I("tt gain %d,tt counter %d",tt_filtes_l->total_gain, tt_filtes_l->iir_counter);
	for(int j = 0; j <IIR_COUNTER; j++)
	{
		//LOG_I("iir coef ff r 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x",
		LOG_I("iir coef ff r %10d, %10d, %10d, %10d, %10d, %10d",  \
		tt_filtes_l->iir_coef[j].coef_b[0], \
		tt_filtes_l->iir_coef[j].coef_b[1], \
		tt_filtes_l->iir_coef[j].coef_b[2], \
		tt_filtes_l->iir_coef[j].coef_a[0], \
		tt_filtes_l->iir_coef[j].coef_a[1], \
		tt_filtes_l->iir_coef[j].coef_a[2]);
	}
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
       LOG_I("mc gain %d,mc counter %d",mc_filtes_l->total_gain, mc_filtes_l->iir_counter);
	for(int j = 0; j <IIR_COUNTER; j++)
	{
		//LOG_I("iir coef ff r 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x",
		LOG_I("iir coef ff r %10d, %10d, %10d, %10d, %10d, %10d",  \
		mc_filtes_l->iir_coef[j].coef_b[0], \
		mc_filtes_l->iir_coef[j].coef_b[1], \
		mc_filtes_l->iir_coef[j].coef_b[2], \
		mc_filtes_l->iir_coef[j].coef_a[0], \
		mc_filtes_l->iir_coef[j].coef_a[1], \
		mc_filtes_l->iir_coef[j].coef_a[2]);
	}
#endif
#endif

    if(anc_type==ANC_FEEDFORWARD)
    {
        if(ff_filtes_l_old.iir_counter==ff_filtes_l->iir_counter
            &&ff_filtes_l_old.iir_bypass_flag==ff_filtes_l->iir_bypass_flag
            &&ff_filtes_l->iir_bypass_flag==0
#if !defined(AUDIO_ANC_TT_HW)&&!defined(AUDIO_ANC_FB_MC_HW)
            &&ff_filtes_r_old.iir_counter==ff_filtes_r->iir_counter
            &&ff_filtes_r_old.iir_bypass_flag==ff_filtes_r->iir_bypass_flag
            &&ff_filtes_r->iir_bypass_flag==0
#endif
            &&anc_gain_delay==ANC_GAIN_NO_DELAY)
        {
            LOG_I("%s: switching FF", __func__);

            if(iir_coef_using==0)
            {
                if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
                {
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_ff_iir_coefs1_l[i].a1=-ff_filtes_l->iir_coef[i].coef_a[1];
                        anc_ff_iir_coefs1_l[i].a2=-ff_filtes_l->iir_coef[i].coef_a[2];
                        anc_ff_iir_coefs1_l[i].b0=ff_filtes_l->iir_coef[i].coef_b[0];
                        anc_ff_iir_coefs1_l[i].b1=ff_filtes_l->iir_coef[i].coef_b[1];
                        anc_ff_iir_coefs1_l[i].b2=ff_filtes_l->iir_coef[i].coef_b[2];

                        anc_fb_iir_coefs1_l[i].a1=-fb_filtes_l_old.iir_coef[i].coef_a[1];
                        anc_fb_iir_coefs1_l[i].a2=-fb_filtes_l_old.iir_coef[i].coef_a[2];
                        anc_fb_iir_coefs1_l[i].b0=fb_filtes_l_old.iir_coef[i].coef_b[0];
                        anc_fb_iir_coefs1_l[i].b1=fb_filtes_l_old.iir_coef[i].coef_b[1];
                        anc_fb_iir_coefs1_l[i].b2=fb_filtes_l_old.iir_coef[i].coef_b[2];

                        ff_filtes_l_old.iir_coef[i]=ff_filtes_l->iir_coef[i];

                    }
                    ff_filtes_l_old.total_gain=ff_filtes_l->total_gain;

                    anc_ff_iir_coefs1_l[0].b0=(int32)((ff_filtes_l->iir_coef[0].coef_b[0]*(int64_t)ff_filtes_l->total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs1_l[0].b1=(int32)((ff_filtes_l->iir_coef[0].coef_b[1]*(int64_t)ff_filtes_l->total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs1_l[0].b2=(int32)((ff_filtes_l->iir_coef[0].coef_b[2]*(int64_t)ff_filtes_l->total_gain)>>GAIN_Q);

                    anc_fb_iir_coefs1_l[0].b0=(int32)((fb_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)fb_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs1_l[0].b1=(int32)((fb_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)fb_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs1_l[0].b2=(int32)((fb_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)fb_filtes_l_old.total_gain)>>GAIN_Q);

                }

#if defined(AUDIO_ANC_TT_HW)||defined(AUDIO_ANC_FB_MC_HW)
                for(i=0;i<IIR_COUNTER;i++)
                {
                    anc_ff_iir_coefs1_r[i].a1=-tt_filtes_l_old.iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs1_r[i].a2=-tt_filtes_l_old.iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs1_r[i].b0=tt_filtes_l_old.iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs1_r[i].b1=tt_filtes_l_old.iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs1_r[i].b2=tt_filtes_l_old.iir_coef[i].coef_b[2];

                    anc_fb_iir_coefs1_r[i].a1=-mc_filtes_l_old.iir_coef[i].coef_a[1];
                    anc_fb_iir_coefs1_r[i].a2=-mc_filtes_l_old.iir_coef[i].coef_a[2];
                    anc_fb_iir_coefs1_r[i].b0=mc_filtes_l_old.iir_coef[i].coef_b[0];
                    anc_fb_iir_coefs1_r[i].b1=mc_filtes_l_old.iir_coef[i].coef_b[1];
                    anc_fb_iir_coefs1_r[i].b2=mc_filtes_l_old.iir_coef[i].coef_b[2];

                }
                anc_ff_iir_coefs1_r[0].b0=(int32)((tt_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)tt_filtes_l_old.total_gain)>>GAIN_Q);
                anc_ff_iir_coefs1_r[0].b1=(int32)((tt_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)tt_filtes_l_old.total_gain)>>GAIN_Q);
                anc_ff_iir_coefs1_r[0].b2=(int32)((tt_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)tt_filtes_l_old.total_gain)>>GAIN_Q);

                anc_fb_iir_coefs1_r[0].b0=(int32)((mc_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)mc_filtes_l_old.total_gain)>>GAIN_Q);
                anc_fb_iir_coefs1_r[0].b1=(int32)((mc_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)mc_filtes_l_old.total_gain)>>GAIN_Q);
                anc_fb_iir_coefs1_r[0].b2=(int32)((mc_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)mc_filtes_l_old.total_gain)>>GAIN_Q);
#else
                if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
                {
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_ff_iir_coefs1_r[i].a1=-ff_filtes_r->iir_coef[i].coef_a[1];
                        anc_ff_iir_coefs1_r[i].a2=-ff_filtes_r->iir_coef[i].coef_a[2];
                        anc_ff_iir_coefs1_r[i].b0=ff_filtes_r->iir_coef[i].coef_b[0];
                        anc_ff_iir_coefs1_r[i].b1=ff_filtes_r->iir_coef[i].coef_b[1];
                        anc_ff_iir_coefs1_r[i].b2=ff_filtes_r->iir_coef[i].coef_b[2];

                        anc_fb_iir_coefs1_r[i].a1=-fb_filtes_r_old.iir_coef[i].coef_a[1];
                        anc_fb_iir_coefs1_r[i].a2=-fb_filtes_r_old.iir_coef[i].coef_a[2];
                        anc_fb_iir_coefs1_r[i].b0=fb_filtes_r_old.iir_coef[i].coef_b[0];
                        anc_fb_iir_coefs1_r[i].b1=fb_filtes_r_old.iir_coef[i].coef_b[1];
                        anc_fb_iir_coefs1_r[i].b2=fb_filtes_r_old.iir_coef[i].coef_b[2];


                        ff_filtes_r_old.iir_coef[i]=ff_filtes_r->iir_coef[i];
                    }

                    ff_filtes_r_old.total_gain=ff_filtes_r->total_gain;

                    anc_ff_iir_coefs1_r[0].b0=(int32)((ff_filtes_r->iir_coef[0].coef_b[0]*(int64_t)ff_filtes_r->total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs1_r[0].b1=(int32)((ff_filtes_r->iir_coef[0].coef_b[1]*(int64_t)ff_filtes_r->total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs1_r[0].b2=(int32)((ff_filtes_r->iir_coef[0].coef_b[2]*(int64_t)ff_filtes_r->total_gain)>>GAIN_Q);

                    anc_fb_iir_coefs1_r[0].b0=(int32)((fb_filtes_r_old.iir_coef[0].coef_b[0]*(int64_t)fb_filtes_r_old.total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs1_r[0].b1=(int32)((fb_filtes_r_old.iir_coef[0].coef_b[1]*(int64_t)fb_filtes_r_old.total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs1_r[0].b2=(int32)((fb_filtes_r_old.iir_coef[0].coef_b[2]*(int64_t)fb_filtes_r_old.total_gain)>>GAIN_Q);

                }
#endif

                anc_iir_control->codec_iir_coef_swap=1;
                while(1)
                {
                    if(anc_iir_control->codec_iir_coef_swap_status==1) break;
                }

            }
            else
            {
                if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
                {
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_ff_iir_coefs0_l[i].a1=-ff_filtes_l->iir_coef[i].coef_a[1];
                        anc_ff_iir_coefs0_l[i].a2=-ff_filtes_l->iir_coef[i].coef_a[2];
                        anc_ff_iir_coefs0_l[i].b0=ff_filtes_l->iir_coef[i].coef_b[0];
                        anc_ff_iir_coefs0_l[i].b1=ff_filtes_l->iir_coef[i].coef_b[1];
                        anc_ff_iir_coefs0_l[i].b2=ff_filtes_l->iir_coef[i].coef_b[2];

                        anc_fb_iir_coefs0_l[i].a1=-fb_filtes_l_old.iir_coef[i].coef_a[1];
                        anc_fb_iir_coefs0_l[i].a2=-fb_filtes_l_old.iir_coef[i].coef_a[2];
                        anc_fb_iir_coefs0_l[i].b0=fb_filtes_l_old.iir_coef[i].coef_b[0];
                        anc_fb_iir_coefs0_l[i].b1=fb_filtes_l_old.iir_coef[i].coef_b[1];
                        anc_fb_iir_coefs0_l[i].b2=fb_filtes_l_old.iir_coef[i].coef_b[2];

                        ff_filtes_l_old.iir_coef[i]=ff_filtes_l->iir_coef[i];
                    }

                    ff_filtes_l_old.total_gain=ff_filtes_l->total_gain;

                    anc_ff_iir_coefs0_l[0].b0=(int32)((ff_filtes_l->iir_coef[0].coef_b[0]*(int64_t)ff_filtes_l->total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs0_l[0].b1=(int32)((ff_filtes_l->iir_coef[0].coef_b[1]*(int64_t)ff_filtes_l->total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs0_l[0].b2=(int32)((ff_filtes_l->iir_coef[0].coef_b[2]*(int64_t)ff_filtes_l->total_gain)>>GAIN_Q);

                    anc_fb_iir_coefs0_l[0].b0=(int32)((fb_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)fb_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs0_l[0].b1=(int32)((fb_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)fb_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs0_l[0].b2=(int32)((fb_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)fb_filtes_l_old.total_gain)>>GAIN_Q);

                }
#if defined(AUDIO_ANC_TT_HW)||defined(AUDIO_ANC_FB_MC_HW)
                for(i=0;i<IIR_COUNTER;i++)
                {
                    anc_ff_iir_coefs0_r[i].a1=-tt_filtes_l_old.iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs0_r[i].a2=-tt_filtes_l_old.iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs0_r[i].b0=tt_filtes_l_old.iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs0_r[i].b1=tt_filtes_l_old.iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs0_r[i].b2=tt_filtes_l_old.iir_coef[i].coef_b[2];

                    anc_fb_iir_coefs0_r[i].a1=-mc_filtes_l_old.iir_coef[i].coef_a[1];
                    anc_fb_iir_coefs0_r[i].a2=-mc_filtes_l_old.iir_coef[i].coef_a[2];
                    anc_fb_iir_coefs0_r[i].b0=mc_filtes_l_old.iir_coef[i].coef_b[0];
                    anc_fb_iir_coefs0_r[i].b1=mc_filtes_l_old.iir_coef[i].coef_b[1];
                    anc_fb_iir_coefs0_r[i].b2=mc_filtes_l_old.iir_coef[i].coef_b[2];
                }
                anc_ff_iir_coefs0_r[0].b0=(int32)((tt_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)tt_filtes_l_old.total_gain)>>GAIN_Q);
                anc_ff_iir_coefs0_r[0].b1=(int32)((tt_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)tt_filtes_l_old.total_gain)>>GAIN_Q);
                anc_ff_iir_coefs0_r[0].b2=(int32)((tt_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)tt_filtes_l_old.total_gain)>>GAIN_Q);

                anc_fb_iir_coefs0_r[0].b0=(int32)((mc_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)mc_filtes_l_old.total_gain)>>GAIN_Q);
                anc_fb_iir_coefs0_r[0].b1=(int32)((mc_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)mc_filtes_l_old.total_gain)>>GAIN_Q);
                anc_fb_iir_coefs0_r[0].b2=(int32)((mc_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)mc_filtes_l_old.total_gain)>>GAIN_Q);
#else
                if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
                {
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_ff_iir_coefs0_r[i].a1=-ff_filtes_r->iir_coef[i].coef_a[1];
                        anc_ff_iir_coefs0_r[i].a2=-ff_filtes_r->iir_coef[i].coef_a[2];
                        anc_ff_iir_coefs0_r[i].b0=ff_filtes_r->iir_coef[i].coef_b[0];
                        anc_ff_iir_coefs0_r[i].b1=ff_filtes_r->iir_coef[i].coef_b[1];
                        anc_ff_iir_coefs0_r[i].b2=ff_filtes_r->iir_coef[i].coef_b[2];

                        anc_fb_iir_coefs0_r[i].a1=-fb_filtes_r_old.iir_coef[i].coef_a[1];
                        anc_fb_iir_coefs0_r[i].a2=-fb_filtes_r_old.iir_coef[i].coef_a[2];
                        anc_fb_iir_coefs0_r[i].b0=fb_filtes_r_old.iir_coef[i].coef_b[0];
                        anc_fb_iir_coefs0_r[i].b1=fb_filtes_r_old.iir_coef[i].coef_b[1];
                        anc_fb_iir_coefs0_r[i].b2=fb_filtes_r_old.iir_coef[i].coef_b[2];

                        ff_filtes_r_old.iir_coef[i]=ff_filtes_r->iir_coef[i];
                    }
                    ff_filtes_r_old.total_gain=ff_filtes_r->total_gain;

                    anc_ff_iir_coefs0_r[0].b0=(int32)((ff_filtes_r->iir_coef[0].coef_b[0]*(int64_t)ff_filtes_r->total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs0_r[0].b1=(int32)((ff_filtes_r->iir_coef[0].coef_b[1]*(int64_t)ff_filtes_r->total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs0_r[0].b2=(int32)((ff_filtes_r->iir_coef[0].coef_b[2]*(int64_t)ff_filtes_r->total_gain)>>GAIN_Q);

                    anc_fb_iir_coefs0_r[0].b0=(int32)((fb_filtes_r_old.iir_coef[0].coef_b[0]*(int64_t)fb_filtes_r_old.total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs0_r[0].b1=(int32)((fb_filtes_r_old.iir_coef[0].coef_b[1]*(int64_t)fb_filtes_r_old.total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs0_r[0].b2=(int32)((fb_filtes_r_old.iir_coef[0].coef_b[2]*(int64_t)fb_filtes_r_old.total_gain)>>GAIN_Q);
                }
#endif
                anc_iir_control->codec_iir_coef_swap=0;
                while(1)
                {
                    if(anc_iir_control->codec_iir_coef_swap_status==0) break;
                }

            }
        	//anc_ff_gain->codec_anc_mute_gain_ff_ch0=ff_filtes_l->total_gain;
        	//anc_ff_gain->codec_anc_mute_gain_ff_ch1=ff_filtes_r->total_gain;

           // anc_ff_gain->codec_anc_mute_gain_ff_ch0=(1<<GAIN_Q);
          //  anc_ff_gain->codec_anc_mute_gain_ff_ch1=(1<<GAIN_Q);

            ff_iir_reset_flag=0;
            iir_coef_using=1-iir_coef_using;
        }
        else
        {
            LOG_I("%s: no switching FF", __func__);

            //disable all IIR filter before updata the coefs;
            anc_iir_control->codec_iir_enable=0;

            ff_iir_reset_flag=1;
        }
    }


    if(anc_type==ANC_FEEDBACK)
    {
        if(fb_filtes_l_old.iir_counter==fb_filtes_l->iir_counter
            &&fb_filtes_l_old.iir_bypass_flag==fb_filtes_l->iir_bypass_flag
            &&fb_filtes_l->iir_bypass_flag==0
#if !defined(AUDIO_ANC_TT_HW) && !defined(AUDIO_ANC_FB_MC_HW)
            &&fb_filtes_r_old.iir_counter==fb_filtes_r->iir_counter
            &&fb_filtes_r_old.iir_bypass_flag==fb_filtes_r->iir_bypass_flag
            &&fb_filtes_r->iir_bypass_flag==0
#endif
            &&anc_gain_delay==ANC_GAIN_NO_DELAY)
        {
            LOG_I("%s: switching FB", __func__);

            if(iir_coef_using==0)
            {
                if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
                {
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_fb_iir_coefs1_l[i].a1=-fb_filtes_l->iir_coef[i].coef_a[1];
                        anc_fb_iir_coefs1_l[i].a2=-fb_filtes_l->iir_coef[i].coef_a[2];
                        anc_fb_iir_coefs1_l[i].b0=fb_filtes_l->iir_coef[i].coef_b[0];
                        anc_fb_iir_coefs1_l[i].b1=fb_filtes_l->iir_coef[i].coef_b[1];
                        anc_fb_iir_coefs1_l[i].b2=fb_filtes_l->iir_coef[i].coef_b[2];

                        anc_ff_iir_coefs1_l[i].a1=-ff_filtes_l_old.iir_coef[i].coef_a[1];
                        anc_ff_iir_coefs1_l[i].a2=-ff_filtes_l_old.iir_coef[i].coef_a[2];
                        anc_ff_iir_coefs1_l[i].b0=ff_filtes_l_old.iir_coef[i].coef_b[0];
                        anc_ff_iir_coefs1_l[i].b1=ff_filtes_l_old.iir_coef[i].coef_b[1];
                        anc_ff_iir_coefs1_l[i].b2=ff_filtes_l_old.iir_coef[i].coef_b[2];

                        fb_filtes_l_old.iir_coef[i]=fb_filtes_l->iir_coef[i];
                    }

                    fb_filtes_l_old.total_gain=fb_filtes_l->total_gain;

                    anc_fb_iir_coefs1_l[0].b0=(int32)((fb_filtes_l->iir_coef[0].coef_b[0]*(int64_t)fb_filtes_l->total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs1_l[0].b1=(int32)((fb_filtes_l->iir_coef[0].coef_b[1]*(int64_t)fb_filtes_l->total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs1_l[0].b2=(int32)((fb_filtes_l->iir_coef[0].coef_b[2]*(int64_t)fb_filtes_l->total_gain)>>GAIN_Q);

                    anc_ff_iir_coefs1_l[0].b0=(int32)((ff_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)ff_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs1_l[0].b1=(int32)((ff_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)ff_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs1_l[0].b2=(int32)((ff_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)ff_filtes_l_old.total_gain)>>GAIN_Q);

                }

#if defined(AUDIO_ANC_TT_HW)||defined(AUDIO_ANC_FB_MC_HW)
                for(i=0;i<IIR_COUNTER;i++)
                {
                    anc_ff_iir_coefs1_r[i].a1=-tt_filtes_l_old.iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs1_r[i].a2=-tt_filtes_l_old.iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs1_r[i].b0=tt_filtes_l_old.iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs1_r[i].b1=tt_filtes_l_old.iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs1_r[i].b2=tt_filtes_l_old.iir_coef[i].coef_b[2];

                    anc_fb_iir_coefs1_r[i].a1=-mc_filtes_l_old.iir_coef[i].coef_a[1];
                    anc_fb_iir_coefs1_r[i].a2=-mc_filtes_l_old.iir_coef[i].coef_a[2];
                    anc_fb_iir_coefs1_r[i].b0=mc_filtes_l_old.iir_coef[i].coef_b[0];
                    anc_fb_iir_coefs1_r[i].b1=mc_filtes_l_old.iir_coef[i].coef_b[1];
                    anc_fb_iir_coefs1_r[i].b2=mc_filtes_l_old.iir_coef[i].coef_b[2];

                }
                anc_ff_iir_coefs1_r[0].b0=(int32)((tt_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)tt_filtes_l_old.total_gain)>>GAIN_Q);
                anc_ff_iir_coefs1_r[0].b1=(int32)((tt_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)tt_filtes_l_old.total_gain)>>GAIN_Q);
                anc_ff_iir_coefs1_r[0].b2=(int32)((tt_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)tt_filtes_l_old.total_gain)>>GAIN_Q);

                anc_fb_iir_coefs1_r[0].b0=(int32)((mc_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)mc_filtes_l_old.total_gain)>>GAIN_Q);
                anc_fb_iir_coefs1_r[0].b1=(int32)((mc_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)mc_filtes_l_old.total_gain)>>GAIN_Q);
                anc_fb_iir_coefs1_r[0].b2=(int32)((mc_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)mc_filtes_l_old.total_gain)>>GAIN_Q);
#else
                if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
                {
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_fb_iir_coefs1_r[i].a1=-fb_filtes_r->iir_coef[i].coef_a[1];
                        anc_fb_iir_coefs1_r[i].a2=-fb_filtes_r->iir_coef[i].coef_a[2];
                        anc_fb_iir_coefs1_r[i].b0=fb_filtes_r->iir_coef[i].coef_b[0];
                        anc_fb_iir_coefs1_r[i].b1=fb_filtes_r->iir_coef[i].coef_b[1];
                        anc_fb_iir_coefs1_r[i].b2=fb_filtes_r->iir_coef[i].coef_b[2];

                        anc_ff_iir_coefs1_r[i].a1=-ff_filtes_r_old.iir_coef[i].coef_a[1];
                        anc_ff_iir_coefs1_r[i].a2=-ff_filtes_r_old.iir_coef[i].coef_a[2];
                        anc_ff_iir_coefs1_r[i].b0=ff_filtes_r_old.iir_coef[i].coef_b[0];
                        anc_ff_iir_coefs1_r[i].b1=ff_filtes_r_old.iir_coef[i].coef_b[1];
                        anc_ff_iir_coefs1_r[i].b2=ff_filtes_r_old.iir_coef[i].coef_b[2];

                        fb_filtes_r_old.iir_coef[i]=fb_filtes_r->iir_coef[i];
                    }
                    fb_filtes_r_old.total_gain=fb_filtes_r->total_gain;

                    anc_fb_iir_coefs1_r[0].b0=(int32)((fb_filtes_r->iir_coef[0].coef_b[0]*(int64_t)fb_filtes_r->total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs1_r[0].b1=(int32)((fb_filtes_r->iir_coef[0].coef_b[1]*(int64_t)fb_filtes_r->total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs1_r[0].b2=(int32)((fb_filtes_r->iir_coef[0].coef_b[2]*(int64_t)fb_filtes_r->total_gain)>>GAIN_Q);

                    anc_ff_iir_coefs1_r[0].b0=(int32)((ff_filtes_r_old.iir_coef[0].coef_b[0]*(int64_t)ff_filtes_r_old.total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs1_r[0].b1=(int32)((ff_filtes_r_old.iir_coef[0].coef_b[1]*(int64_t)ff_filtes_r_old.total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs1_r[0].b2=(int32)((ff_filtes_r_old.iir_coef[0].coef_b[2]*(int64_t)ff_filtes_r_old.total_gain)>>GAIN_Q);
                }
#endif
                anc_iir_control->codec_iir_coef_swap=1;
                while(1)
                {
                    if(anc_iir_control->codec_iir_coef_swap_status==1) break;
                }
            }
            else
            {

                if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
                {
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_fb_iir_coefs0_l[i].a1=-fb_filtes_l->iir_coef[i].coef_a[1];
                        anc_fb_iir_coefs0_l[i].a2=-fb_filtes_l->iir_coef[i].coef_a[2];
                        anc_fb_iir_coefs0_l[i].b0=fb_filtes_l->iir_coef[i].coef_b[0];
                        anc_fb_iir_coefs0_l[i].b1=fb_filtes_l->iir_coef[i].coef_b[1];
                        anc_fb_iir_coefs0_l[i].b2=fb_filtes_l->iir_coef[i].coef_b[2];

                        anc_ff_iir_coefs0_l[i].a1=-ff_filtes_l_old.iir_coef[i].coef_a[1];
                        anc_ff_iir_coefs0_l[i].a2=-ff_filtes_l_old.iir_coef[i].coef_a[2];
                        anc_ff_iir_coefs0_l[i].b0=ff_filtes_l_old.iir_coef[i].coef_b[0];
                        anc_ff_iir_coefs0_l[i].b1=ff_filtes_l_old.iir_coef[i].coef_b[1];
                        anc_ff_iir_coefs0_l[i].b2=ff_filtes_l_old.iir_coef[i].coef_b[2];

                        fb_filtes_l_old.iir_coef[i]=fb_filtes_l->iir_coef[i];
                    }

                    fb_filtes_l_old.total_gain=fb_filtes_l->total_gain;

                    anc_fb_iir_coefs0_l[0].b0=(int32)((fb_filtes_l->iir_coef[0].coef_b[0]*(int64_t)fb_filtes_l->total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs0_l[0].b1=(int32)((fb_filtes_l->iir_coef[0].coef_b[1]*(int64_t)fb_filtes_l->total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs0_l[0].b2=(int32)((fb_filtes_l->iir_coef[0].coef_b[2]*(int64_t)fb_filtes_l->total_gain)>>GAIN_Q);

                    anc_ff_iir_coefs0_l[0].b0=(int32)((ff_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)ff_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs0_l[0].b1=(int32)((ff_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)ff_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs0_l[0].b2=(int32)((ff_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)ff_filtes_l_old.total_gain)>>GAIN_Q);
                }
#if defined(AUDIO_ANC_TT_HW)||defined(AUDIO_ANC_FB_MC_HW)
                for(i=0;i<IIR_COUNTER;i++)
                {
                    anc_ff_iir_coefs0_r[i].a1=-tt_filtes_l_old.iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs0_r[i].a2=-tt_filtes_l_old.iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs0_r[i].b0=tt_filtes_l_old.iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs0_r[i].b1=tt_filtes_l_old.iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs0_r[i].b2=tt_filtes_l_old.iir_coef[i].coef_b[2];

                    anc_fb_iir_coefs0_r[i].a1=-mc_filtes_l_old.iir_coef[i].coef_a[1];
                    anc_fb_iir_coefs0_r[i].a2=-mc_filtes_l_old.iir_coef[i].coef_a[2];
                    anc_fb_iir_coefs0_r[i].b0=mc_filtes_l_old.iir_coef[i].coef_b[0];
                    anc_fb_iir_coefs0_r[i].b1=mc_filtes_l_old.iir_coef[i].coef_b[1];
                    anc_fb_iir_coefs0_r[i].b2=mc_filtes_l_old.iir_coef[i].coef_b[2];
                }
                anc_ff_iir_coefs0_r[0].b0=(int32)((tt_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)tt_filtes_l_old.total_gain)>>GAIN_Q);
                anc_ff_iir_coefs0_r[0].b1=(int32)((tt_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)tt_filtes_l_old.total_gain)>>GAIN_Q);
                anc_ff_iir_coefs0_r[0].b2=(int32)((tt_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)tt_filtes_l_old.total_gain)>>GAIN_Q);

                anc_fb_iir_coefs0_r[0].b0=(int32)((mc_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)mc_filtes_l_old.total_gain)>>GAIN_Q);
                anc_fb_iir_coefs0_r[0].b1=(int32)((mc_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)mc_filtes_l_old.total_gain)>>GAIN_Q);
                anc_fb_iir_coefs0_r[0].b2=(int32)((mc_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)mc_filtes_l_old.total_gain)>>GAIN_Q);
#else
                if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
                {
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_fb_iir_coefs0_r[i].a1=-fb_filtes_r->iir_coef[i].coef_a[1];
                        anc_fb_iir_coefs0_r[i].a2=-fb_filtes_r->iir_coef[i].coef_a[2];
                        anc_fb_iir_coefs0_r[i].b0=fb_filtes_r->iir_coef[i].coef_b[0];
                        anc_fb_iir_coefs0_r[i].b1=fb_filtes_r->iir_coef[i].coef_b[1];
                        anc_fb_iir_coefs0_r[i].b2=fb_filtes_r->iir_coef[i].coef_b[2];

                        anc_ff_iir_coefs0_r[i].a1=-ff_filtes_r_old.iir_coef[i].coef_a[1];
                        anc_ff_iir_coefs0_r[i].a2=-ff_filtes_r_old.iir_coef[i].coef_a[2];
                        anc_ff_iir_coefs0_r[i].b0=ff_filtes_r_old.iir_coef[i].coef_b[0];
                        anc_ff_iir_coefs0_r[i].b1=ff_filtes_r_old.iir_coef[i].coef_b[1];
                        anc_ff_iir_coefs0_r[i].b2=ff_filtes_r_old.iir_coef[i].coef_b[2];

                        fb_filtes_r_old.iir_coef[i]=fb_filtes_r->iir_coef[i];
                    }
                    fb_filtes_r_old.total_gain=fb_filtes_r->total_gain;

                    anc_fb_iir_coefs0_r[0].b0=(int32)((fb_filtes_r->iir_coef[0].coef_b[0]*(int64_t)fb_filtes_r->total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs0_r[0].b1=(int32)((fb_filtes_r->iir_coef[0].coef_b[1]*(int64_t)fb_filtes_r->total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs0_r[0].b2=(int32)((fb_filtes_r->iir_coef[0].coef_b[2]*(int64_t)fb_filtes_r->total_gain)>>GAIN_Q);

                    anc_ff_iir_coefs0_r[0].b0=(int32)((ff_filtes_r_old.iir_coef[0].coef_b[0]*(int64_t)ff_filtes_r_old.total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs0_r[0].b1=(int32)((ff_filtes_r_old.iir_coef[0].coef_b[1]*(int64_t)ff_filtes_r_old.total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs0_r[0].b2=(int32)((ff_filtes_r_old.iir_coef[0].coef_b[2]*(int64_t)ff_filtes_r_old.total_gain)>>GAIN_Q);
                }
#endif
                anc_iir_control->codec_iir_coef_swap=0;
                while(1)
                {
                    if(anc_iir_control->codec_iir_coef_swap_status==0) break;
                }
            }

         //   anc_fb_gain->codec_anc_mute_gain_fb_ch0=fb_filtes_l->total_gain;
        //    anc_fb_gain->codec_anc_mute_gain_fb_ch1=fb_filtes_r->total_gain;

           // anc_fb_gain->codec_anc_mute_gain_fb_ch0=(1<<GAIN_Q);
         //   anc_fb_gain->codec_anc_mute_gain_fb_ch1=(1<<GAIN_Q);

            fb_iir_reset_flag=0;
            iir_coef_using=1-iir_coef_using;
        }
        else
        {
            LOG_I("%s: no switching FB", __func__);

            //disable all IIR filter before updata the coefs;
            anc_iir_control->codec_iir_enable=0;

            fb_iir_reset_flag=1;
        }
    }

#if defined(AUDIO_ANC_TT_HW)
    if(anc_type==ANC_TALKTHRU)
    {
        if(tt_filtes_l_old.iir_counter==tt_filtes_l->iir_counter
            &&tt_filtes_l_old.iir_bypass_flag==tt_filtes_l->iir_bypass_flag
            &&tt_filtes_l->iir_bypass_flag==0
            &&anc_gain_delay==ANC_GAIN_NO_DELAY)
        {
            LOG_I("%s: switching TT", __func__);

            if(iir_coef_using==0)
            {
                if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
                {
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_ff_iir_coefs1_r[i].a1=-tt_filtes_l->iir_coef[i].coef_a[1];
                        anc_ff_iir_coefs1_r[i].a2=-tt_filtes_l->iir_coef[i].coef_a[2];
                        anc_ff_iir_coefs1_r[i].b0=tt_filtes_l->iir_coef[i].coef_b[0];
                        anc_ff_iir_coefs1_r[i].b1=tt_filtes_l->iir_coef[i].coef_b[1];
                        anc_ff_iir_coefs1_r[i].b2=tt_filtes_l->iir_coef[i].coef_b[2];

                        anc_fb_iir_coefs1_r[i].a1=-mc_filtes_l_old.iir_coef[i].coef_a[1];
                        anc_fb_iir_coefs1_r[i].a2=-mc_filtes_l_old.iir_coef[i].coef_a[2];
                        anc_fb_iir_coefs1_r[i].b0=mc_filtes_l_old.iir_coef[i].coef_b[0];
                        anc_fb_iir_coefs1_r[i].b1=mc_filtes_l_old.iir_coef[i].coef_b[1];
                        anc_fb_iir_coefs1_r[i].b2=mc_filtes_l_old.iir_coef[i].coef_b[2];

                        anc_ff_iir_coefs1_l[i].a1=-ff_filtes_l_old.iir_coef[i].coef_a[1];
                        anc_ff_iir_coefs1_l[i].a2=-ff_filtes_l_old.iir_coef[i].coef_a[2];
                        anc_ff_iir_coefs1_l[i].b0=ff_filtes_l_old.iir_coef[i].coef_b[0];
                        anc_ff_iir_coefs1_l[i].b1=ff_filtes_l_old.iir_coef[i].coef_b[1];
                        anc_ff_iir_coefs1_l[i].b2=ff_filtes_l_old.iir_coef[i].coef_b[2];

                        anc_fb_iir_coefs1_l[i].a1=-fb_filtes_l_old.iir_coef[i].coef_a[1];
                        anc_fb_iir_coefs1_l[i].a2=-fb_filtes_l_old.iir_coef[i].coef_a[2];
                        anc_fb_iir_coefs1_l[i].b0=fb_filtes_l_old.iir_coef[i].coef_b[0];
                        anc_fb_iir_coefs1_l[i].b1=fb_filtes_l_old.iir_coef[i].coef_b[1];
                        anc_fb_iir_coefs1_l[i].b2=fb_filtes_l_old.iir_coef[i].coef_b[2];

                        tt_filtes_l_old.iir_coef[i]=tt_filtes_l->iir_coef[i];
                    }

                    tt_filtes_l_old.total_gain=tt_filtes_l->total_gain;

                    anc_ff_iir_coefs1_r[0].b0=(int32)((tt_filtes_l->iir_coef[0].coef_b[0]*(int64_t)tt_filtes_l->total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs1_r[0].b1=(int32)((tt_filtes_l->iir_coef[0].coef_b[1]*(int64_t)tt_filtes_l->total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs1_r[0].b2=(int32)((tt_filtes_l->iir_coef[0].coef_b[2]*(int64_t)tt_filtes_l->total_gain)>>GAIN_Q);

                    anc_fb_iir_coefs1_r[0].b0=(int32)((mc_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)mc_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs1_r[0].b1=(int32)((mc_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)mc_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs1_r[0].b2=(int32)((mc_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)mc_filtes_l_old.total_gain)>>GAIN_Q);

                    anc_ff_iir_coefs1_l[0].b0=(int32)((ff_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)ff_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs1_l[0].b1=(int32)((ff_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)ff_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs1_l[0].b2=(int32)((ff_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)ff_filtes_l_old.total_gain)>>GAIN_Q);

                    anc_fb_iir_coefs1_l[0].b0=(int32)((fb_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)fb_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs1_l[0].b1=(int32)((fb_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)fb_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs1_l[0].b2=(int32)((fb_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)fb_filtes_l_old.total_gain)>>GAIN_Q);


                }

                anc_iir_control->codec_iir_coef_swap=1;
                while(1)
                {
                    if(anc_iir_control->codec_iir_coef_swap_status==1) break;
                }

            }
            else
            {
                if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
                {
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_ff_iir_coefs0_r[i].a1=-tt_filtes_l->iir_coef[i].coef_a[1];
                        anc_ff_iir_coefs0_r[i].a2=-tt_filtes_l->iir_coef[i].coef_a[2];
                        anc_ff_iir_coefs0_r[i].b0=tt_filtes_l->iir_coef[i].coef_b[0];
                        anc_ff_iir_coefs0_r[i].b1=tt_filtes_l->iir_coef[i].coef_b[1];
                        anc_ff_iir_coefs0_r[i].b2=tt_filtes_l->iir_coef[i].coef_b[2];

                        anc_fb_iir_coefs0_r[i].a1=-mc_filtes_l_old.iir_coef[i].coef_a[1];
                        anc_fb_iir_coefs0_r[i].a2=-mc_filtes_l_old.iir_coef[i].coef_a[2];
                        anc_fb_iir_coefs0_r[i].b0=mc_filtes_l_old.iir_coef[i].coef_b[0];
                        anc_fb_iir_coefs0_r[i].b1=mc_filtes_l_old.iir_coef[i].coef_b[1];
                        anc_fb_iir_coefs0_r[i].b2=mc_filtes_l_old.iir_coef[i].coef_b[2];

                        anc_ff_iir_coefs0_l[i].a1=-ff_filtes_l_old.iir_coef[i].coef_a[1];
                        anc_ff_iir_coefs0_l[i].a2=-ff_filtes_l_old.iir_coef[i].coef_a[2];
                        anc_ff_iir_coefs0_l[i].b0=ff_filtes_l_old.iir_coef[i].coef_b[0];
                        anc_ff_iir_coefs0_l[i].b1=ff_filtes_l_old.iir_coef[i].coef_b[1];
                        anc_ff_iir_coefs0_l[i].b2=ff_filtes_l_old.iir_coef[i].coef_b[2];

                        anc_fb_iir_coefs0_l[i].a1=-fb_filtes_l_old.iir_coef[i].coef_a[1];
                        anc_fb_iir_coefs0_l[i].a2=-fb_filtes_l_old.iir_coef[i].coef_a[2];
                        anc_fb_iir_coefs0_l[i].b0=fb_filtes_l_old.iir_coef[i].coef_b[0];
                        anc_fb_iir_coefs0_l[i].b1=fb_filtes_l_old.iir_coef[i].coef_b[1];
                        anc_fb_iir_coefs0_l[i].b2=fb_filtes_l_old.iir_coef[i].coef_b[2];

                        tt_filtes_l_old.iir_coef[i]=tt_filtes_l->iir_coef[i];
                    }
                    tt_filtes_l_old.total_gain=tt_filtes_l->total_gain;

                    anc_ff_iir_coefs0_r[0].b0=(int32)((tt_filtes_l->iir_coef[0].coef_b[0]*(int64_t)tt_filtes_l->total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs0_r[0].b1=(int32)((tt_filtes_l->iir_coef[0].coef_b[1]*(int64_t)tt_filtes_l->total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs0_r[0].b2=(int32)((tt_filtes_l->iir_coef[0].coef_b[2]*(int64_t)tt_filtes_l->total_gain)>>GAIN_Q);

                    anc_fb_iir_coefs0_r[0].b0=(int32)((mc_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)mc_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs0_r[0].b1=(int32)((mc_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)mc_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs0_r[0].b2=(int32)((mc_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)mc_filtes_l_old.total_gain)>>GAIN_Q);

                    anc_ff_iir_coefs0_l[0].b0=(int32)((ff_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)ff_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs0_l[0].b1=(int32)((ff_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)ff_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs0_l[0].b2=(int32)((ff_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)ff_filtes_l_old.total_gain)>>GAIN_Q);

                    anc_fb_iir_coefs0_l[0].b0=(int32)((fb_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)fb_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs0_l[0].b1=(int32)((fb_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)fb_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs0_l[0].b2=(int32)((fb_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)fb_filtes_l_old.total_gain)>>GAIN_Q);
                }

                anc_iir_control->codec_iir_coef_swap=0;
                while(1)
                {
                    if(anc_iir_control->codec_iir_coef_swap_status==0) break;
                }

            }
        	//anc_ff_gain->codec_anc_mute_gain_ff_ch0=ff_filtes_l->total_gain;
        	//anc_ff_gain->codec_anc_mute_gain_ff_ch1=ff_filtes_r->total_gain;

           // anc_ff_gain->codec_anc_mute_gain_ff_ch0=(1<<GAIN_Q);
          //  anc_ff_gain->codec_anc_mute_gain_ff_ch1=(1<<GAIN_Q);

            tt_iir_reset_flag=0;
            iir_coef_using=1-iir_coef_using;
        }
        else
        {
            LOG_I("%s: no switching TT", __func__);

            //disable all IIR filter before updata the coefs;
            anc_iir_control->codec_iir_enable=0;

            tt_iir_reset_flag=1;
        }
    }
 #endif

#if defined(AUDIO_ANC_FB_MC_HW)
    if(anc_type==ANC_MUSICCANCLE)
    {
        if(mc_filtes_l_old.iir_counter==mc_filtes_l->iir_counter
            &&mc_filtes_l_old.iir_bypass_flag==mc_filtes_l->iir_bypass_flag
            &&mc_filtes_l->iir_bypass_flag==0

            &&anc_gain_delay==ANC_GAIN_NO_DELAY)
        {
            LOG_I("%s: switching MC", __func__);

            if(iir_coef_using==0)
            {
                if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
                {
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_fb_iir_coefs1_r[i].a1=-mc_filtes_l->iir_coef[i].coef_a[1];
                        anc_fb_iir_coefs1_r[i].a2=-mc_filtes_l->iir_coef[i].coef_a[2];
                        anc_fb_iir_coefs1_r[i].b0=mc_filtes_l->iir_coef[i].coef_b[0];
                        anc_fb_iir_coefs1_r[i].b1=mc_filtes_l->iir_coef[i].coef_b[1];
                        anc_fb_iir_coefs1_r[i].b2=mc_filtes_l->iir_coef[i].coef_b[2];

                        anc_ff_iir_coefs1_r[i].a1=-tt_filtes_l_old.iir_coef[i].coef_a[1];
                        anc_ff_iir_coefs1_r[i].a2=-tt_filtes_l_old.iir_coef[i].coef_a[2];
                        anc_ff_iir_coefs1_r[i].b0=tt_filtes_l_old.iir_coef[i].coef_b[0];
                        anc_ff_iir_coefs1_r[i].b1=tt_filtes_l_old.iir_coef[i].coef_b[1];
                        anc_ff_iir_coefs1_r[i].b2=tt_filtes_l_old.iir_coef[i].coef_b[2];

                        anc_ff_iir_coefs1_l[i].a1=-ff_filtes_l_old.iir_coef[i].coef_a[1];
                        anc_ff_iir_coefs1_l[i].a2=-ff_filtes_l_old.iir_coef[i].coef_a[2];
                        anc_ff_iir_coefs1_l[i].b0=ff_filtes_l_old.iir_coef[i].coef_b[0];
                        anc_ff_iir_coefs1_l[i].b1=ff_filtes_l_old.iir_coef[i].coef_b[1];
                        anc_ff_iir_coefs1_l[i].b2=ff_filtes_l_old.iir_coef[i].coef_b[2];

                        anc_fb_iir_coefs1_l[i].a1=-fb_filtes_l_old.iir_coef[i].coef_a[1];
                        anc_fb_iir_coefs1_l[i].a2=-fb_filtes_l_old.iir_coef[i].coef_a[2];
                        anc_fb_iir_coefs1_l[i].b0=fb_filtes_l_old.iir_coef[i].coef_b[0];
                        anc_fb_iir_coefs1_l[i].b1=fb_filtes_l_old.iir_coef[i].coef_b[1];
                        anc_fb_iir_coefs1_l[i].b2=fb_filtes_l_old.iir_coef[i].coef_b[2];


                        mc_filtes_l_old.iir_coef[i]=mc_filtes_l->iir_coef[i];
                    }
                    mc_filtes_l_old.total_gain=mc_filtes_l->total_gain;

                    anc_fb_iir_coefs1_r[0].b0=(int32)((mc_filtes_l->iir_coef[0].coef_b[0]*(int64_t)mc_filtes_l->total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs1_r[0].b1=(int32)((mc_filtes_l->iir_coef[0].coef_b[1]*(int64_t)mc_filtes_l->total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs1_r[0].b2=(int32)((mc_filtes_l->iir_coef[0].coef_b[2]*(int64_t)mc_filtes_l->total_gain)>>GAIN_Q);

                    anc_ff_iir_coefs1_r[0].b0=(int32)((tt_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)tt_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs1_r[0].b1=(int32)((tt_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)tt_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs1_r[0].b2=(int32)((tt_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)tt_filtes_l_old.total_gain)>>GAIN_Q);

                    anc_ff_iir_coefs1_l[0].b0=(int32)((ff_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)ff_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs1_l[0].b1=(int32)((ff_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)ff_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs1_l[0].b2=(int32)((ff_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)ff_filtes_l_old.total_gain)>>GAIN_Q);

                    anc_fb_iir_coefs1_l[0].b0=(int32)((fb_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)fb_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs1_l[0].b1=(int32)((fb_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)fb_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs1_l[0].b2=(int32)((fb_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)fb_filtes_l_old.total_gain)>>GAIN_Q);
                }

                anc_iir_control->codec_iir_coef_swap=1;
                while(1)
                {
                    if(anc_iir_control->codec_iir_coef_swap_status==1) break;
                }
            }
            else
            {
                if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
                {
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_fb_iir_coefs0_r[i].a1=-mc_filtes_l->iir_coef[i].coef_a[1];
                        anc_fb_iir_coefs0_r[i].a2=-mc_filtes_l->iir_coef[i].coef_a[2];
                        anc_fb_iir_coefs0_r[i].b0=mc_filtes_l->iir_coef[i].coef_b[0];
                        anc_fb_iir_coefs0_r[i].b1=mc_filtes_l->iir_coef[i].coef_b[1];
                        anc_fb_iir_coefs0_r[i].b2=mc_filtes_l->iir_coef[i].coef_b[2];

                        anc_ff_iir_coefs0_r[i].a1=-tt_filtes_l_old.iir_coef[i].coef_a[1];
                        anc_ff_iir_coefs0_r[i].a2=-tt_filtes_l_old.iir_coef[i].coef_a[2];
                        anc_ff_iir_coefs0_r[i].b0=tt_filtes_l_old.iir_coef[i].coef_b[0];
                        anc_ff_iir_coefs0_r[i].b1=tt_filtes_l_old.iir_coef[i].coef_b[1];
                        anc_ff_iir_coefs0_r[i].b2=tt_filtes_l_old.iir_coef[i].coef_b[2];

                        anc_ff_iir_coefs0_l[i].a1=-ff_filtes_l_old.iir_coef[i].coef_a[1];
                        anc_ff_iir_coefs0_l[i].a2=-ff_filtes_l_old.iir_coef[i].coef_a[2];
                        anc_ff_iir_coefs0_l[i].b0=ff_filtes_l_old.iir_coef[i].coef_b[0];
                        anc_ff_iir_coefs0_l[i].b1=ff_filtes_l_old.iir_coef[i].coef_b[1];
                        anc_ff_iir_coefs0_l[i].b2=ff_filtes_l_old.iir_coef[i].coef_b[2];

                        anc_fb_iir_coefs0_l[i].a1=-fb_filtes_l_old.iir_coef[i].coef_a[1];
                        anc_fb_iir_coefs0_l[i].a2=-fb_filtes_l_old.iir_coef[i].coef_a[2];
                        anc_fb_iir_coefs0_l[i].b0=fb_filtes_l_old.iir_coef[i].coef_b[0];
                        anc_fb_iir_coefs0_l[i].b1=fb_filtes_l_old.iir_coef[i].coef_b[1];
                        anc_fb_iir_coefs0_l[i].b2=fb_filtes_l_old.iir_coef[i].coef_b[2];

                        mc_filtes_l_old.iir_coef[i]=mc_filtes_l->iir_coef[i];
                    }
                    mc_filtes_l_old.total_gain=mc_filtes_l->total_gain;

                    anc_fb_iir_coefs0_r[0].b0=(int32)((mc_filtes_l->iir_coef[0].coef_b[0]*(int64_t)mc_filtes_l->total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs0_r[0].b1=(int32)((mc_filtes_l->iir_coef[0].coef_b[1]*(int64_t)mc_filtes_l->total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs0_r[0].b2=(int32)((mc_filtes_l->iir_coef[0].coef_b[2]*(int64_t)mc_filtes_l->total_gain)>>GAIN_Q);

                    anc_ff_iir_coefs0_r[0].b0=(int32)((tt_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)tt_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs0_r[0].b1=(int32)((tt_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)tt_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs0_r[0].b2=(int32)((tt_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)tt_filtes_l_old.total_gain)>>GAIN_Q);

                    anc_ff_iir_coefs0_l[0].b0=(int32)((ff_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)ff_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs0_l[0].b1=(int32)((ff_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)ff_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs0_l[0].b2=(int32)((ff_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)ff_filtes_l_old.total_gain)>>GAIN_Q);

                    anc_fb_iir_coefs0_l[0].b0=(int32)((fb_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)fb_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs0_l[0].b1=(int32)((fb_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)fb_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs0_l[0].b2=(int32)((fb_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)fb_filtes_l_old.total_gain)>>GAIN_Q);

                }

                anc_iir_control->codec_iir_coef_swap=0;
                while(1)
                {
                    if(anc_iir_control->codec_iir_coef_swap_status==0) break;
                }
            }

         //   anc_fb_gain->codec_anc_mute_gain_fb_ch0=fb_filtes_l->total_gain;
        //    anc_fb_gain->codec_anc_mute_gain_fb_ch1=fb_filtes_r->total_gain;

           // anc_fb_gain->codec_anc_mute_gain_fb_ch0=(1<<GAIN_Q);
         //   anc_fb_gain->codec_anc_mute_gain_fb_ch1=(1<<GAIN_Q);

            mc_iir_reset_flag=0;
            iir_coef_using=1-iir_coef_using;
        }
        else
        {
            LOG_I("%s: no switching MC", __func__);

            //disable all IIR filter before updata the coefs;
            anc_iir_control->codec_iir_enable=0;

            mc_iir_reset_flag=1;
        }
    }
#endif

    //feedforward ANC settings
    if(anc_type==ANC_FEEDFORWARD&&ff_iir_reset_flag==1)
    {
         LOG_I("%s: resetting FF", __func__);

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            for(i=0;i<IIR_COUNTER;i++)
            {
                ff_filtes_l_old.iir_coef[i]=ff_filtes_l->iir_coef[i];
             }

            ff_filtes_l_old.total_gain=ff_filtes_l->total_gain;
            ff_filtes_l_old.iir_counter=ff_filtes_l->iir_counter;
            ff_filtes_l_old.iir_bypass_flag=ff_filtes_l->iir_bypass_flag;

            max_ff_gain_l=(1<<GAIN_Q);

            //Set the FF gain;
            anc_ff_gain->codec_anc_mute_gain_ff_ch0=0;

            //setting the feedforward IIR filters;
            if(ff_filtes_l->iir_bypass_flag==0)
            {
                anc_iir_control->codec_iir_ch0_bypass=0;

                if(ff_filtes_l->iir_counter>IIR_COUNTER)
                {
                    anc_iir_control->codec_iir_count_ch0=IIR_COUNTER;
                }
                else if(ff_filtes_l->iir_counter>0)
                {
                    anc_iir_control->codec_iir_count_ch0=ff_filtes_l->iir_counter;
                }
                else
                {
                    anc_iir_control->codec_iir_ch0_bypass=1;
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
                    anc_ff_gain->codec_anc_mute_gain_ff_ch0=max_ff_gain_l;
                }

            }
            else
            {
                anc_iir_control->codec_iir_ch0_bypass=1;
                anc_ff_gain->codec_anc_mute_gain_ff_ch0=ff_filtes_l->total_gain;
                max_ff_gain_l=ff_filtes_l->total_gain;

            }
        }
        else
        {
                anc_iir_control->codec_iir_ch0_bypass=1;
                anc_ff_gain->codec_anc_mute_gain_ff_ch0=0;
                anc_iir_control->codec_iir_count_ch0=0;
        }

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            for(i=0;i<IIR_COUNTER;i++)
            {
                ff_filtes_r_old.iir_coef[i]=ff_filtes_r->iir_coef[i];
             }

            ff_filtes_r_old.total_gain=ff_filtes_r->total_gain;
            ff_filtes_r_old.iir_counter=ff_filtes_r->iir_counter;
            ff_filtes_r_old.iir_bypass_flag=ff_filtes_r->iir_bypass_flag;

            max_ff_gain_r=(1<<GAIN_Q);

            //Set the FF gain;
            anc_ff_gain->codec_anc_mute_gain_ff_ch1=0;

            if(ff_filtes_r->iir_bypass_flag==0)
            {
                anc_iir_control->codec_iir_ch1_bypass=0;

                if(ff_filtes_r->iir_counter>IIR_COUNTER)
                {
                    anc_iir_control->codec_iir_count_ch1=IIR_COUNTER;
                }
                else if(ff_filtes_r->iir_counter>0)
                {
                    anc_iir_control->codec_iir_count_ch1=ff_filtes_r->iir_counter;
                }
                else
                {
                    anc_iir_control->codec_iir_ch1_bypass=1;
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
                    anc_ff_gain->codec_anc_mute_gain_ff_ch1=max_ff_gain_r;
                }
            }
            else
            {
                anc_iir_control->codec_iir_ch1_bypass=1;
                anc_ff_gain->codec_anc_mute_gain_ff_ch1=ff_filtes_r->total_gain;
                max_ff_gain_r=ff_filtes_r->total_gain;
            }
        }
        else
        {
#if !defined(AUDIO_ANC_TT_HW)
            anc_iir_control->codec_iir_ch1_bypass=1;
            anc_ff_gain->codec_anc_mute_gain_ff_ch1=0;
            anc_iir_control->codec_iir_count_ch1=0;
#endif
        }
#if (AUD_SECTION_STRUCT_VERSION == 1)
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            //setting the feedforward FIR filters;
            if(ff_filtes_l->fir_bypass_flag==0&&ff_filtes_l->fir_len==FIR_LEN)
            {
                fir_ff_config0_l->stream0_fir1=0;

                anc_control->codec_ff_ch0_fir_en=1;
                fir_control->fir_stream_enable_ch0=1;

                if(ff_filtes_l->fir_len>FIR_LEN)
                {
                    fir_ff_config0_l->fir_order=FIR_LEN;
                }
                else if(ff_filtes_l->fir_len>0)
                {
                    fir_ff_config0_l->fir_order=ff_filtes_l->fir_len;
                }
                else
                {
                    anc_control->codec_ff_ch0_fir_en=0;
                    fir_ff_config0_l->fir_order=0;
                    LOG_I("Error ff_filtes_l FIR LEN:%d", ff_filtes_l->fir_len);
                    err=ANC_TYPE_ERR;
                }

                //feedforward left ch FIR coefs settings
                for(i=0;i<fir_ff_config0_l->fir_order;i++)
                {
                    anc_ff_fir_coefs_l[i]=(int32)ff_filtes_l->fir_coef[i]*256;
                }

                fir_ff_config1_l->fir_gain_sel=6;

            }
            else
            {
                anc_control->codec_ff_ch0_fir_en=0;
                fir_control->fir_stream_enable_ch0=0;
            }
        }
        else
        {
                anc_control->codec_ff_ch0_fir_en=0;
                fir_control->fir_stream_enable_ch0=0;
        }

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            if(ff_filtes_r->fir_bypass_flag==0&&ff_filtes_r->fir_len==FIR_LEN)
            {
                fir_ff_config0_r->stream0_fir1=0;

                anc_control->codec_ff_ch1_fir_en=1;
                fir_control->fir_stream_enable_ch1=1;

                if(ff_filtes_r->fir_len>FIR_LEN)
                {
                    fir_ff_config0_r->fir_order=FIR_LEN;
                }
                else if(ff_filtes_r->fir_len>0)
                {
                    fir_ff_config0_r->fir_order=ff_filtes_r->fir_len;
                }
                else
                {
                    anc_control->codec_ff_ch1_fir_en=0;
                    fir_ff_config0_r->fir_order=0;
                    LOG_I("Error ff_filtes_l FIR LEN:%d", ff_filtes_r->fir_len);
                    err=ANC_TYPE_ERR;
                }

                //feedforward left ch FIR coefs settings
                for(i=0;i<fir_ff_config0_r->fir_order;i++)
                {
                    anc_ff_fir_coefs_r[i]=(int32)ff_filtes_r->fir_coef[i]*256;
                }

                fir_ff_config1_r->fir_gain_sel=6;

            }
            else
            {
                anc_control->codec_ff_ch1_fir_en=0;
                fir_control->fir_stream_enable_ch1=0;
            }
        }
        else
        {
            anc_control->codec_ff_ch1_fir_en=0;
            fir_control->fir_stream_enable_ch1=0;
        }
#endif

        total_iir_counter=total_iir_counter+anc_iir_control->codec_iir_count_ch0+anc_iir_control->codec_iir_count_ch1;
        total_iir_counter=total_iir_counter+anc_iir_control->codec_iir_count_ch2+anc_iir_control->codec_iir_count_ch3;

        LOG_I("%s:total_iir_counter(FF):%d", __func__,total_iir_counter);

        //enable all IIR filters
        anc_iir_control->codec_iir_enable=1;
    }

    //feedback ANC settings
    if(anc_type==ANC_FEEDBACK&&fb_iir_reset_flag==1)
    {
        LOG_I("%s: resetting FB", __func__);

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            for(i=0;i<IIR_COUNTER;i++)
            {
                fb_filtes_l_old.iir_coef[i]=fb_filtes_l->iir_coef[i];
             }

            fb_filtes_l_old.total_gain=fb_filtes_l->total_gain;
            fb_filtes_l_old.iir_counter=fb_filtes_l->iir_counter;
            fb_filtes_l_old.iir_bypass_flag=fb_filtes_l->iir_bypass_flag;

            max_fb_gain_l=(1<<GAIN_Q);

            //Set the FB gain;
            anc_fb_gain->codec_anc_mute_gain_fb_ch0=0;

            //setting the feedback IIR filters;
            if(fb_filtes_l->iir_bypass_flag==0)
            {
                anc_iir_control->codec_iir_ch2_bypass=0;

                if(fb_filtes_l->iir_counter>IIR_COUNTER)
                {
                    anc_iir_control->codec_iir_count_ch2=IIR_COUNTER;
                }
                else if(fb_filtes_l->iir_counter>0)
                {
                    anc_iir_control->codec_iir_count_ch2=fb_filtes_l->iir_counter;
                }
                else
                {
                    anc_iir_control->codec_iir_ch2_bypass=1;
                    LOG_I("Error fb_filtes_l IIR counter:%d", fb_filtes_l->iir_counter);
                    err=ANC_TYPE_ERR;
                }
                if(iir_coef_using==0)
                {
                    //feedback left ch iir coefs settings
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_fb_iir_coefs0_l[i].a1=-fb_filtes_l->iir_coef[i].coef_a[1];
                        anc_fb_iir_coefs0_l[i].a2=-fb_filtes_l->iir_coef[i].coef_a[2];
                        anc_fb_iir_coefs0_l[i].b0=fb_filtes_l->iir_coef[i].coef_b[0];
                        anc_fb_iir_coefs0_l[i].b1=fb_filtes_l->iir_coef[i].coef_b[1];
                        anc_fb_iir_coefs0_l[i].b2=fb_filtes_l->iir_coef[i].coef_b[2];
                    }
                    anc_fb_iir_coefs0_l[0].b0=(int32)((fb_filtes_l->iir_coef[0].coef_b[0]*(int64_t)fb_filtes_l->total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs0_l[0].b1=(int32)((fb_filtes_l->iir_coef[0].coef_b[1]*(int64_t)fb_filtes_l->total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs0_l[0].b2=(int32)((fb_filtes_l->iir_coef[0].coef_b[2]*(int64_t)fb_filtes_l->total_gain)>>GAIN_Q);
                }
                else
                {
                    //feedback left ch iir coefs settings
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_fb_iir_coefs1_l[i].a1=-fb_filtes_l->iir_coef[i].coef_a[1];
                        anc_fb_iir_coefs1_l[i].a2=-fb_filtes_l->iir_coef[i].coef_a[2];
                        anc_fb_iir_coefs1_l[i].b0=fb_filtes_l->iir_coef[i].coef_b[0];
                        anc_fb_iir_coefs1_l[i].b1=fb_filtes_l->iir_coef[i].coef_b[1];
                        anc_fb_iir_coefs1_l[i].b2=fb_filtes_l->iir_coef[i].coef_b[2];
                    }
                    anc_fb_iir_coefs1_l[0].b0=(int32)((fb_filtes_l->iir_coef[0].coef_b[0]*(int64_t)fb_filtes_l->total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs1_l[0].b1=(int32)((fb_filtes_l->iir_coef[0].coef_b[1]*(int64_t)fb_filtes_l->total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs1_l[0].b2=(int32)((fb_filtes_l->iir_coef[0].coef_b[2]*(int64_t)fb_filtes_l->total_gain)>>GAIN_Q);
                 }
                if(err!=ANC_NO_ERR)
                {
                    max_fb_gain_l=0;
                }

                if(anc_gain_delay==ANC_GAIN_NO_DELAY)
                {
                    //Set the FB gain;
                    anc_fb_gain->codec_anc_mute_gain_fb_ch0=max_fb_gain_l;
                }
            }
            else
            {
                anc_iir_control->codec_iir_ch2_bypass=1;
                anc_fb_gain->codec_anc_mute_gain_fb_ch0=fb_filtes_l->total_gain;
                max_fb_gain_l=fb_filtes_l->total_gain;

            }
        }
        else
        {
                anc_iir_control->codec_iir_ch2_bypass=1;
                anc_fb_gain->codec_anc_mute_gain_fb_ch0=0;
                anc_iir_control->codec_iir_count_ch2=0;
        }

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            for(i=0;i<IIR_COUNTER;i++)
            {
                fb_filtes_r_old.iir_coef[i]=fb_filtes_r->iir_coef[i];
             }

            fb_filtes_r_old.total_gain=fb_filtes_r->total_gain;
            fb_filtes_r_old.iir_counter=fb_filtes_r->iir_counter;
            fb_filtes_r_old.iir_bypass_flag=fb_filtes_r->iir_bypass_flag;

            max_fb_gain_r=(1<<GAIN_Q);

            //Set the FB gain;
            anc_fb_gain->codec_anc_mute_gain_fb_ch1=0;

            if(fb_filtes_r->iir_bypass_flag==0)
            {
                anc_iir_control->codec_iir_ch3_bypass=0;

                if(fb_filtes_r->iir_counter>IIR_COUNTER)
                {
                    anc_iir_control->codec_iir_count_ch3=IIR_COUNTER;
                }
                else if(fb_filtes_r->iir_counter>0)
                {
                    anc_iir_control->codec_iir_count_ch3=fb_filtes_r->iir_counter;
                }
                else
                {
                    anc_iir_control->codec_iir_ch3_bypass=1;
                    LOG_I("Error fb_filtes_r IIR counter:%d", fb_filtes_r->iir_counter);
                    err=ANC_TYPE_ERR;
                }

                if(iir_coef_using==0)
                {
                    //feedback right ch iir coefs settings
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_fb_iir_coefs0_r[i].a1=-fb_filtes_r->iir_coef[i].coef_a[1];
                        anc_fb_iir_coefs0_r[i].a2=-fb_filtes_r->iir_coef[i].coef_a[2];
                        anc_fb_iir_coefs0_r[i].b0=fb_filtes_r->iir_coef[i].coef_b[0];
                        anc_fb_iir_coefs0_r[i].b1=fb_filtes_r->iir_coef[i].coef_b[1];
                        anc_fb_iir_coefs0_r[i].b2=fb_filtes_r->iir_coef[i].coef_b[2];
                    }
                    anc_fb_iir_coefs0_r[0].b0=(int32)((fb_filtes_r->iir_coef[0].coef_b[0]*(int64_t)fb_filtes_r->total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs0_r[0].b1=(int32)((fb_filtes_r->iir_coef[0].coef_b[1]*(int64_t)fb_filtes_r->total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs0_r[0].b2=(int32)((fb_filtes_r->iir_coef[0].coef_b[2]*(int64_t)fb_filtes_r->total_gain)>>GAIN_Q);
                }
                else
                {
                    //feedback right ch iir coefs settings
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_fb_iir_coefs1_r[i].a1=-fb_filtes_r->iir_coef[i].coef_a[1];
                        anc_fb_iir_coefs1_r[i].a2=-fb_filtes_r->iir_coef[i].coef_a[2];
                        anc_fb_iir_coefs1_r[i].b0=fb_filtes_r->iir_coef[i].coef_b[0];
                        anc_fb_iir_coefs1_r[i].b1=fb_filtes_r->iir_coef[i].coef_b[1];
                        anc_fb_iir_coefs1_r[i].b2=fb_filtes_r->iir_coef[i].coef_b[2];
                    }
                    anc_fb_iir_coefs1_r[0].b0=(int32)((fb_filtes_r->iir_coef[0].coef_b[0]*(int64_t)fb_filtes_r->total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs1_r[0].b1=(int32)((fb_filtes_r->iir_coef[0].coef_b[1]*(int64_t)fb_filtes_r->total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs1_r[0].b2=(int32)((fb_filtes_r->iir_coef[0].coef_b[2]*(int64_t)fb_filtes_r->total_gain)>>GAIN_Q);
                 }
                if(err!=ANC_NO_ERR)
                {
                    max_fb_gain_r=0;
                }

                if(anc_gain_delay==ANC_GAIN_NO_DELAY)
                {
                    //Set the FB gain;
                    anc_fb_gain->codec_anc_mute_gain_fb_ch1=max_fb_gain_r;
                }
            }
            else
            {
                anc_iir_control->codec_iir_ch3_bypass=1;
                anc_fb_gain->codec_anc_mute_gain_fb_ch1=fb_filtes_r->total_gain;
                max_fb_gain_r=fb_filtes_r->total_gain;
            }
        }
        else
        {
#if !defined(AUDIO_ANC_FB_MC_HW)
            anc_iir_control->codec_iir_ch3_bypass=1;
            anc_fb_gain->codec_anc_mute_gain_fb_ch1=0;
            anc_iir_control->codec_iir_count_ch3=0;
#endif
        }

#if (AUD_SECTION_STRUCT_VERSION == 1)

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            //setting the feedback FIR filters;
            if(fb_filtes_l->fir_bypass_flag==0&&fb_filtes_l->fir_len==FIR_LEN)
            {
                fir_fb_config0_l->stream0_fir1=0;

                anc_control->codec_fb_ch0_fir_en=1;
                fir_control->fir_stream_enable_ch2=1;

                if(fb_filtes_l->fir_len>FIR_LEN)
                {
                    fir_fb_config0_l->fir_order=FIR_LEN;
                }
                else if(fb_filtes_l->fir_len>0)
                {
                    fir_fb_config0_l->fir_order=fb_filtes_l->fir_len;
                }
                else
                {
                    anc_control->codec_fb_ch0_fir_en=0;
                    fir_fb_config0_l->fir_order=0;
                    LOG_I("Error ff_filtes_l FIR LEN:%d", fb_filtes_l->fir_len);
                    err=ANC_TYPE_ERR;
                }

                //feedforward left ch FIR coefs settings
                for(i=0;i<fir_fb_config0_l->fir_order;i++)
                {
                    anc_fb_fir_coefs_l[i]=(int32)fb_filtes_l->fir_coef[i]*256;
                }

                fir_fb_config1_l->fir_gain_sel=6;

            }
            else
            {
                anc_control->codec_fb_ch0_fir_en=0;
                fir_control->fir_stream_enable_ch2=0;
            }
        }
        else
        {
            anc_control->codec_fb_ch0_fir_en=0;
            fir_control->fir_stream_enable_ch2=0;
        }

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            if(fb_filtes_r->fir_bypass_flag==0&&fb_filtes_r->fir_len==FIR_LEN)
            {
                fir_fb_config0_r->stream0_fir1=0;

                anc_control->codec_fb_ch1_fir_en=1;
                fir_control->fir_stream_enable_ch3=1;

                if(fb_filtes_r->fir_len>FIR_LEN)
                {
                    fir_fb_config0_r->fir_order=FIR_LEN;
                }
                else if(fb_filtes_r->fir_len>0)
                {
                    fir_fb_config0_r->fir_order=fb_filtes_r->fir_len;
                }
                else
                {
                    anc_control->codec_fb_ch1_fir_en=0;
                    fir_fb_config0_r->fir_order=0;
                    LOG_I("Error ff_filtes_l FIR LEN:%d", fb_filtes_r->fir_len);
                    err=ANC_TYPE_ERR;
                }
                //feedforward left ch FIR coefs settings
                for(i=0;i<fir_fb_config0_r->fir_order;i++)
                {
                    anc_fb_fir_coefs_r[i]=(int32)fb_filtes_r->fir_coef[i]*256;
                }

                fir_fb_config1_r->fir_gain_sel=6;
            }
            else
            {
                anc_control->codec_fb_ch1_fir_en=0;
                fir_control->fir_stream_enable_ch3=0;
            }
        }
        else
        {
            anc_control->codec_fb_ch1_fir_en=0;
            fir_control->fir_stream_enable_ch3=0;
        }
#endif

        total_iir_counter=total_iir_counter+anc_iir_control->codec_iir_count_ch0+anc_iir_control->codec_iir_count_ch1;
        total_iir_counter=total_iir_counter+anc_iir_control->codec_iir_count_ch2+anc_iir_control->codec_iir_count_ch3;

        LOG_I("%s:total_iir_counter(FB):%d", __func__,total_iir_counter);

        //enable all IIR filters
        anc_iir_control->codec_iir_enable=1;

    }
#if defined(AUDIO_ANC_TT_HW)
    //talk thr settings
    if(anc_type==ANC_TALKTHRU&&tt_iir_reset_flag==1)
    {
         LOG_I("%s: resetting TT", __func__);

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            for(i=0;i<IIR_COUNTER;i++)
            {
                tt_filtes_l_old.iir_coef[i]=tt_filtes_l->iir_coef[i];
             }

            tt_filtes_l_old.total_gain=tt_filtes_l->total_gain;
            tt_filtes_l_old.iir_counter=tt_filtes_l->iir_counter;
            tt_filtes_l_old.iir_bypass_flag=tt_filtes_l->iir_bypass_flag;


            max_tt_gain_l=(1<<GAIN_Q);

            //Set the TT gain;
            tt_mc_control->codec_mute_gain_coef_tt=0;

            if(tt_filtes_l->iir_bypass_flag==0)
            {
                anc_iir_control->codec_iir_ch1_bypass=0;

                if(tt_filtes_l->iir_counter>IIR_COUNTER)
                {
                    anc_iir_control->codec_iir_count_ch1=IIR_COUNTER;
                }
                else if(tt_filtes_l->iir_counter>0)
                {
                    anc_iir_control->codec_iir_count_ch1=tt_filtes_l->iir_counter;
                }
                else
                {
                    anc_iir_control->codec_iir_ch1_bypass=1;
                    LOG_I("Error ff_filtes_r IIR counter:%d", tt_filtes_l->iir_counter);
                    err=ANC_TYPE_ERR;
                }
                if(iir_coef_using==0)
                {
                    //talk thru left ch iir coefs settings
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_ff_iir_coefs0_r[i].a1=-tt_filtes_l->iir_coef[i].coef_a[1];
                        anc_ff_iir_coefs0_r[i].a2=-tt_filtes_l->iir_coef[i].coef_a[2];
                        anc_ff_iir_coefs0_r[i].b0=tt_filtes_l->iir_coef[i].coef_b[0];
                        anc_ff_iir_coefs0_r[i].b1=tt_filtes_l->iir_coef[i].coef_b[1];
                        anc_ff_iir_coefs0_r[i].b2=tt_filtes_l->iir_coef[i].coef_b[2];
                    }
                    anc_ff_iir_coefs0_r[0].b0=(int32)((tt_filtes_l->iir_coef[0].coef_b[0]*(int64_t)tt_filtes_l->total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs0_r[0].b1=(int32)((tt_filtes_l->iir_coef[0].coef_b[1]*(int64_t)tt_filtes_l->total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs0_r[0].b2=(int32)((tt_filtes_l->iir_coef[0].coef_b[2]*(int64_t)tt_filtes_l->total_gain)>>GAIN_Q);
                }
                else
                {
                    //talk thru left ch iir coefs settings
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_ff_iir_coefs1_r[i].a1=-tt_filtes_l->iir_coef[i].coef_a[1];
                        anc_ff_iir_coefs1_r[i].a2=-tt_filtes_l->iir_coef[i].coef_a[2];
                        anc_ff_iir_coefs1_r[i].b0=tt_filtes_l->iir_coef[i].coef_b[0];
                        anc_ff_iir_coefs1_r[i].b1=tt_filtes_l->iir_coef[i].coef_b[1];
                        anc_ff_iir_coefs1_r[i].b2=tt_filtes_l->iir_coef[i].coef_b[2];
                    }
                    anc_ff_iir_coefs1_r[0].b0=(int32)((tt_filtes_l->iir_coef[0].coef_b[0]*(int64_t)tt_filtes_l->total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs1_r[0].b1=(int32)((tt_filtes_l->iir_coef[0].coef_b[1]*(int64_t)tt_filtes_l->total_gain)>>GAIN_Q);
                    anc_ff_iir_coefs1_r[0].b2=(int32)((tt_filtes_l->iir_coef[0].coef_b[2]*(int64_t)tt_filtes_l->total_gain)>>GAIN_Q);
                }
                if(err!=ANC_NO_ERR)
                {
                    max_tt_gain_l=0;
                }
                if(anc_gain_delay==ANC_GAIN_NO_DELAY)
                {
                    //Set the TT gain;
                    tt_mc_control->codec_mute_gain_coef_tt=max_tt_gain_l;
                }
            }
            else
            {
                anc_iir_control->codec_iir_ch1_bypass=1;
                tt_mc_control->codec_mute_gain_coef_tt=tt_filtes_l->total_gain;
                max_tt_gain_l=tt_filtes_l->total_gain;
            }
        }
        else
        {
            anc_iir_control->codec_iir_ch1_bypass=1;
            tt_mc_control->codec_mute_gain_coef_tt=0;
            anc_iir_control->codec_iir_count_ch1=0;
        }

        total_iir_counter=total_iir_counter+anc_iir_control->codec_iir_count_ch0+anc_iir_control->codec_iir_count_ch1;
        total_iir_counter=total_iir_counter+anc_iir_control->codec_iir_count_ch2+anc_iir_control->codec_iir_count_ch3;

        LOG_I("%s:total_iir_counter(TT):%d", __func__,total_iir_counter);
        //enable all IIR filters
        anc_iir_control->codec_iir_enable=1;

        //enable TT.
        tt_mc_control->codec_tt_enable=1;
    }
#endif

#if defined(AUDIO_ANC_FB_MC_HW)
    //music cancel ANC settings
    if(anc_type==ANC_MUSICCANCLE&&mc_iir_reset_flag==1)
    {
        LOG_I("%s: resetting MC", __func__);

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            for(i=0;i<IIR_COUNTER;i++)
            {
                mc_filtes_l_old.iir_coef[i]=mc_filtes_l->iir_coef[i];
             }

            mc_filtes_l_old.total_gain=mc_filtes_l->total_gain;
            mc_filtes_l_old.iir_counter=mc_filtes_l->iir_counter;
            mc_filtes_l_old.iir_bypass_flag=mc_filtes_l->iir_bypass_flag;

            max_mc_gain_l=(1<<GAIN_Q);

            //Set the mc gain;
            tt_mc_control->codec_mute_gain_coef_mm=0;

            if(mc_filtes_l->iir_bypass_flag==0)
            {
                anc_iir_control->codec_iir_ch3_bypass=0;

                if(mc_filtes_l->iir_counter>IIR_COUNTER)
                {
                    anc_iir_control->codec_iir_count_ch3=IIR_COUNTER;
                }
                else if(mc_filtes_l->iir_counter>0)
                {
                    anc_iir_control->codec_iir_count_ch3=mc_filtes_l->iir_counter;
                }
                else
                {
                    anc_iir_control->codec_iir_ch3_bypass=1;
                    LOG_I("Error fb_filtes_r IIR counter:%d", mc_filtes_l->iir_counter);
                    err=ANC_TYPE_ERR;
                }

                if(iir_coef_using==0)
                {
                    //music cancel ch iir coefs settings
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_fb_iir_coefs0_r[i].a1=-mc_filtes_l->iir_coef[i].coef_a[1];
                        anc_fb_iir_coefs0_r[i].a2=-mc_filtes_l->iir_coef[i].coef_a[2];
                        anc_fb_iir_coefs0_r[i].b0=mc_filtes_l->iir_coef[i].coef_b[0];
                        anc_fb_iir_coefs0_r[i].b1=mc_filtes_l->iir_coef[i].coef_b[1];
                        anc_fb_iir_coefs0_r[i].b2=mc_filtes_l->iir_coef[i].coef_b[2];
                    }
                    anc_fb_iir_coefs0_r[0].b0=(int32)((mc_filtes_l->iir_coef[0].coef_b[0]*(int64_t)mc_filtes_l->total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs0_r[0].b1=(int32)((mc_filtes_l->iir_coef[0].coef_b[1]*(int64_t)mc_filtes_l->total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs0_r[0].b2=(int32)((mc_filtes_l->iir_coef[0].coef_b[2]*(int64_t)mc_filtes_l->total_gain)>>GAIN_Q);
                }
                else
                {
                    //music cancel ch iir coefs settings
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_fb_iir_coefs1_r[i].a1=-mc_filtes_l->iir_coef[i].coef_a[1];
                        anc_fb_iir_coefs1_r[i].a2=-mc_filtes_l->iir_coef[i].coef_a[2];
                        anc_fb_iir_coefs1_r[i].b0=mc_filtes_l->iir_coef[i].coef_b[0];
                        anc_fb_iir_coefs1_r[i].b1=mc_filtes_l->iir_coef[i].coef_b[1];
                        anc_fb_iir_coefs1_r[i].b2=mc_filtes_l->iir_coef[i].coef_b[2];
                    }
                    anc_fb_iir_coefs1_r[0].b0=(int32)((mc_filtes_l->iir_coef[0].coef_b[0]*(int64_t)mc_filtes_l->total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs1_r[0].b1=(int32)((mc_filtes_l->iir_coef[0].coef_b[1]*(int64_t)mc_filtes_l->total_gain)>>GAIN_Q);
                    anc_fb_iir_coefs1_r[0].b2=(int32)((mc_filtes_l->iir_coef[0].coef_b[2]*(int64_t)mc_filtes_l->total_gain)>>GAIN_Q);
                 }
                if(err!=ANC_NO_ERR)
                {
                    max_mc_gain_l=0;
                }

                if(anc_gain_delay==ANC_GAIN_NO_DELAY)
                {
                    //Set the MC gain;
                    tt_mc_control->codec_mute_gain_coef_mm=max_mc_gain_l;
                }
            }
            else
            {
                anc_iir_control->codec_iir_ch3_bypass=1;
                tt_mc_control->codec_mute_gain_coef_mm=mc_filtes_l->total_gain;
                max_mc_gain_l=mc_filtes_l->total_gain;
            }
        }
        else
        {
            anc_iir_control->codec_iir_ch3_bypass=1;
            tt_mc_control->codec_mute_gain_coef_mm=0;
            anc_iir_control->codec_iir_count_ch3=0;
        }

        total_iir_counter=total_iir_counter+anc_iir_control->codec_iir_count_ch0+anc_iir_control->codec_iir_count_ch1;
        total_iir_counter=total_iir_counter+anc_iir_control->codec_iir_count_ch2+anc_iir_control->codec_iir_count_ch3;

        LOG_I("%s:total_iir_counter(MC):%d", __func__,total_iir_counter);

        //enable all IIR filters
        anc_iir_control->codec_iir_enable=1;

        //enable MC.
        tt_mc_control->codec_mm_enable=1;
    }
#endif
    anc_control->codec_anc_enable_ch0=1;
    anc_control->codec_anc_enable_ch1=1;

    if(anc_type==ANC_FEEDFORWARD)
    {
        analog_aud_apply_anc_adc_gain_offset(anc_type, ff_filtes_l->adc_gain_offset, ff_filtes_r->adc_gain_offset);
    }
    else if(anc_type==ANC_FEEDBACK)
    {
        analog_aud_apply_anc_adc_gain_offset(anc_type, fb_filtes_l->adc_gain_offset, fb_filtes_r->adc_gain_offset);
    }

	return err;
}

static void anc_ctrl_reg_init(void)
{
    //disable ANC;
    anc_control->codec_anc_enable_ch0=0;
    anc_control->codec_anc_enable_ch1=0;

#if defined(AUDIO_ANC_TT_HW)
    tt_mc_control->codec_tt_enable=1;
#endif

#if defined(AUDIO_ANC_FB_MC_HW)
    tt_mc_control->codec_mm_enable=1;
#endif

    //init swap to use coef0
    anc_iir_control->codec_iir_coef_swap=0;

    anc_control->codec_anc_rate_sel=0;

    anc_control->codec_feedback_ch0=0;
    anc_control->codec_feedback_ch1=0;

    anc_control->codec_dual_anc_ch0=0;
    anc_control->codec_dual_anc_ch1=0;

    //disable all IIR filters
    anc_iir_control->codec_iir_enable=0;

    //use FF IIR filters
    anc_iir_control->codec_iir_ch0_bypass=1;
    anc_iir_control->codec_iir_ch1_bypass=1;

    //Bypass FB IIR filters
    anc_iir_control->codec_iir_ch2_bypass=1;
    anc_iir_control->codec_iir_ch3_bypass=1;

    //use all of IIR filters.The max number is 8.
    anc_iir_control->codec_iir_count_ch0=0;
    anc_iir_control->codec_iir_count_ch1=0;
    anc_iir_control->codec_iir_count_ch2=0;
    anc_iir_control->codec_iir_count_ch3=0;

    //unmute ANC.
    anc_control->codec_anc_mute_ch0=0;
    anc_control->codec_anc_mute_ch1=0;

    //seting FIR filters,Bypass FIr Filter.
    anc_control->codec_ff_ch0_fir_en=0;
    anc_control->codec_ff_ch1_fir_en=0;
    anc_control->codec_fb_ch0_fir_en=0;
    anc_control->codec_fb_ch1_fir_en=0;

#if (AUD_SECTION_STRUCT_VERSION == 1)
    //disable FIR stream;
    fir_control->fir_stream_enable_ch0=0;
    fir_control->fir_stream_enable_ch1=0;
    fir_control->fir_stream_enable_ch2=0;
    fir_control->fir_stream_enable_ch3=0;
#endif

    //disable gain updated when pass0
    anc_ff_gain->codec_anc_mute_gain_pass0_ff_ch0=1;
    anc_ff_gain->codec_anc_mute_gain_pass0_ff_ch1=1;

    anc_fb_gain->codec_anc_mute_gain_pass0_fb_ch0=1;
    anc_fb_gain->codec_anc_mute_gain_pass0_fb_ch1=1;

#if defined(AUDIO_ANC_TT_HW) ||defined(AUDIO_ANC_FB_MC_HW)
    tt_mc_control->codec_mute_gain_pass0_tt=1;
    tt_mc_control->codec_mute_gain_pass0_mm=1;
#endif

}

void anc_disable_gain_updated_when_pass0(uint8_t on)
{
 /*
    anc_ff_gain->codec_anc_mute_gain_pass0_ff_ch0=on;
    anc_ff_gain->codec_anc_mute_gain_pass0_ff_ch1=on;

    anc_fb_gain->codec_anc_mute_gain_pass0_fb_ch0=on;
    anc_fb_gain->codec_anc_mute_gain_pass0_fb_ch1=on;
    */
}
static void anc_ctrl_reg_open(enum ANC_TYPE_T anc_type)
{
    LOG_I("%s", __func__);

    if(anc_type==ANC_FEEDFORWARD)
    {
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            //clear iir counter
            anc_iir_control->codec_iir_count_ch0=0;

            //set the FF gain;
            anc_ff_gain->codec_anc_mute_gain_ff_ch0=0;

            ff_filtes_l_old.total_gain=0;
            ff_filtes_l_old.iir_counter=0;
            ff_filtes_l_old.iir_bypass_flag=1;
        }

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            //clear iir counter
            anc_iir_control->codec_iir_count_ch1=0;

            //set the FF gain;
            anc_ff_gain->codec_anc_mute_gain_ff_ch1=0;

            ff_filtes_r_old.total_gain=0;
            ff_filtes_r_old.iir_counter=0;
            ff_filtes_r_old.iir_bypass_flag=1;
        }
        ff_iir_reset_flag=0;
#if defined(AUDIO_ANC_TT_HW)
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            //clear iir counter
            anc_iir_control->codec_iir_count_ch1=0;

            //set the TT gain;
            tt_mc_control->codec_mute_gain_coef_tt=0;

            tt_filtes_l_old.total_gain=0;
            tt_filtes_l_old.iir_counter=0;
            tt_filtes_l_old.iir_bypass_flag=1;
        }
        tt_iir_reset_flag=0;
#endif
    }

    if(anc_type==ANC_FEEDBACK)
    {
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            //clear iir counter
            anc_iir_control->codec_iir_count_ch2=0;

            //set the FB gain;
            anc_fb_gain->codec_anc_mute_gain_fb_ch0=0;

#if defined(AUDIO_ANC_FB_MC) ||defined(AUDIO_ANC_FB_MC_HW)
            feedback_mc_settings->codec_feedback_mc_en_ch0=1;
#endif
            fb_filtes_l_old.total_gain=0;
            fb_filtes_l_old.iir_counter=0;
            fb_filtes_l_old.iir_bypass_flag=1;
        }

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            //clear iir counter
            anc_iir_control->codec_iir_count_ch3=0;

            //set the FB gain;
            anc_fb_gain->codec_anc_mute_gain_fb_ch1=0;

#ifdef AUDIO_ANC_FB_MC
        //    feedback_mc_settings->codec_adc_mc_en_ch1=1;
            feedback_mc_settings->codec_feedback_mc_en_ch1=1;
#endif
            fb_filtes_r_old.total_gain=0;
            fb_filtes_r_old.iir_counter=0;
            fb_filtes_r_old.iir_bypass_flag=1;
        }
        fb_iir_reset_flag=0;
#if defined(AUDIO_ANC_FB_MC_HW)
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            //clear iir counter
            anc_iir_control->codec_iir_count_ch3=0;

            //set the MC gain;
            tt_mc_control->codec_mute_gain_coef_mm=0;

            mc_filtes_l_old.total_gain=0;
            mc_filtes_l_old.iir_counter=0;
            mc_filtes_l_old.iir_bypass_flag=1;

            //set MC delay.
            mc_config->codec_mm_fifo_bypass=1;
            /*
            mc_config->codec_mm_delay=4;

            mc_config->codec_mm_delay_update=0;
            mc_config->codec_mm_delay_update=1;

            mc_config->codec_mm_fifo_en=1;
            */
        }
        mc_iir_reset_flag=0;
#endif
    }

    if(ff_open_flag==1&&fb_open_flag==1)
    {
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            anc_control->codec_feedback_ch0=1;
            anc_control->codec_dual_anc_ch0=1;
        }
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            anc_control->codec_feedback_ch1=1;
            anc_control->codec_dual_anc_ch1=1;
        }

    }
    else if(ff_open_flag==0&&fb_open_flag==1)
    {
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            anc_control->codec_feedback_ch0=1;
            anc_control->codec_dual_anc_ch0=0;
        }
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            anc_control->codec_feedback_ch1=1;
            anc_control->codec_dual_anc_ch1=0;
        }
    }
    else if(ff_open_flag==1&&fb_open_flag==0)
    {
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            anc_control->codec_feedback_ch0=0;
            anc_control->codec_dual_anc_ch0=0;
        }
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            anc_control->codec_feedback_ch1=0;
            anc_control->codec_dual_anc_ch1=0;
        }
    }
}


static void anc_ctrl_reg_close(enum ANC_TYPE_T anc_type)
{
    if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
    {
        if(anc_type==ANC_FEEDFORWARD)
        {
            //clear iir counter
            anc_iir_control->codec_iir_count_ch0=0;

            //set the FF gain;
            anc_ff_gain->codec_anc_mute_gain_ff_ch0=0;

#if defined(AUDIO_ANC_TT_HW)
            //clear iir counter
            anc_iir_control->codec_iir_count_ch0=0;

            //set the TT gain;
            tt_mc_control->codec_mute_gain_coef_tt=0;
#endif

        }

        if(anc_type==ANC_FEEDBACK)
        {
            //clear iir counter
            anc_iir_control->codec_iir_count_ch2=0;

            //set the FB gain;
            anc_fb_gain->codec_anc_mute_gain_fb_ch0=0;

#if defined(AUDIO_ANC_FB_MC) ||defined(AUDIO_ANC_FB_MC_HW)
              feedback_mc_settings->codec_feedback_mc_en_ch0=0;
#endif
        }

        if(ff_open_flag==1&&fb_open_flag==1)
        {
            anc_control->codec_feedback_ch0=1;
            anc_control->codec_dual_anc_ch0=1;
        }
        else if(ff_open_flag==0&&fb_open_flag==1)
        {
            anc_control->codec_feedback_ch0=1;
            anc_control->codec_dual_anc_ch0=0;
        }
        else if(ff_open_flag==1&&fb_open_flag==0)
        {
            anc_control->codec_feedback_ch0=0;
            anc_control->codec_dual_anc_ch0=0;
        }
#if defined(AUDIO_ANC_FB_MC_HW)
        //clear iir counter
        anc_iir_control->codec_iir_count_ch0=0;

        //set the MC gain;
        tt_mc_control->codec_mute_gain_coef_mm=0;
#endif
    }

    if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
    {
        if(anc_type==ANC_FEEDFORWARD)
        {
            //clear iir counter
            anc_iir_control->codec_iir_count_ch1=0;

            //set the FF gain;
            anc_ff_gain->codec_anc_mute_gain_ff_ch1=0;
        }

        if(anc_type==ANC_FEEDBACK)
        {
            //clear iir counter
            anc_iir_control->codec_iir_count_ch3=0;

            //set the FB gain;
            anc_fb_gain->codec_anc_mute_gain_fb_ch1=0;

#ifdef AUDIO_ANC_FB_MC
      //      feedback_mc_settings->codec_adc_mc_en_ch1=0;

            feedback_mc_settings->codec_feedback_mc_en_ch1=0;
#endif
        }

        if(ff_open_flag==1&&fb_open_flag==1)
        {
            anc_control->codec_feedback_ch1=1;
            anc_control->codec_dual_anc_ch1=1;
        }
        else if(ff_open_flag==0&&fb_open_flag==1)
        {
            anc_control->codec_feedback_ch1=1;
            anc_control->codec_dual_anc_ch1=0;

        }
        else if(ff_open_flag==1&&fb_open_flag==0)
        {
            anc_control->codec_feedback_ch1=0;
            anc_control->codec_dual_anc_ch1=0;
        }
    }
}

int anc_opened(enum ANC_TYPE_T anc_type)
{
    if (anc_type == ANC_FEEDFORWARD) {
        return ff_open_flag;
    }
    if (anc_type == ANC_FEEDBACK) {
        return fb_open_flag;
    }
#if defined(AUDIO_ANC_FB_MC_HW)
    if (anc_type == ANC_MUSICCANCLE) {
        return fb_open_flag;
    }
#endif
    return 0;
}

#ifdef ANC_FB_CHECK
#define FB_CHECK_DELAY ((MS_TO_TICKS(5000)))
static void anc_fb_check_irq_handler(uint32_t status)
{
    //LOG_I("%s,", __func__);
    int32_t fb_gain_l, fb_gain_r;
    struct _codec_int_config *codec_int_config = (struct _codec_int_config *)&status;

    anc_get_gain(&fb_gain_l, &fb_gain_r, ANC_FEEDBACK);

    if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
    {
        if(codec_int_config->fb_check_error_trig_ch0)
        {
            LOG_I("*Threshold:%10d,Data Energy :%10d",*codec_fb_check_threshold_ch0,*codec_fb_check_data_avg_keep_ch0);

            fb_check_ch0_config->codec_fb_check_enable_ch0=0;

            hwtimer_stop(fb_adc_check_dev_timer);
            hwtimer_start(fb_adc_check_dev_timer, FB_CHECK_DELAY);

            if(fb_gain_l==max_fb_gain_l)
            {
                anc_set_gain(max_fb_gain_l/2, fb_gain_r,ANC_FEEDBACK);
            }

            LOG_I("ADC_left");

            fb_check_ch0_config->codec_fb_check_enable_ch0=1;
        }
    }
    if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
    {
        if(codec_int_config->fb_check_error_trig_ch1)
        {
            LOG_I("*Threshold:%10d,Data Energy :%10d",*codec_fb_check_threshold_ch1,*codec_fb_check_data_avg_keep_ch1);

            fb_check_ch1_config->codec_fb_check_enable_ch1=0;

            hwtimer_stop(fb_adc_check_dev_timer);
            hwtimer_start(fb_adc_check_dev_timer, FB_CHECK_DELAY);

            if(fb_gain_r==max_fb_gain_r)
            {
                anc_set_gain(fb_gain_l, max_fb_gain_r/2,ANC_FEEDBACK);
            }

            LOG_I("ADC_right");

            fb_check_ch1_config->codec_fb_check_enable_ch1=1;
        }
    }
}

static void anc_fb_check_timer_handler(void *param)
{
    LOG_I("%s,", __func__);
    anc_set_gain(max_fb_gain_l, max_fb_gain_r,ANC_FEEDBACK);
}
#endif

int anc_open(enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err=ANC_NO_ERR;

    LOG_I("%s", __func__);

    if(anc_type==ANC_FEEDFORWARD&&ff_open_flag==1)return err;
    if(anc_type==ANC_FEEDBACK&&fb_open_flag==1)return err;


    anc_cfg_lock();

#ifdef ANC_FB_CHECK
    if(fb_adc_check_dev_timer==NULL)
    {
        fb_adc_check_dev_timer = hwtimer_alloc(anc_fb_check_timer_handler, NULL);
        ASSERT(fb_adc_check_dev_timer, "Failed to alloc usbdev_timer");
    }
#endif

    if(ff_open_flag==0&&fb_open_flag==0)
    {
        hal_cmu_codec_iir_enable(98000000);
        anc_ctrl_reg_init();

        iir_coef_using=0;
    }

    if(anc_type==ANC_FEEDFORWARD)
    {
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            max_ff_gain_l=0;
        }

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            max_ff_gain_r=0;
        }

        ff_open_flag=1;
#if defined(AUDIO_ANC_TT_HW)
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            max_tt_gain_l=0;
        }
#endif
    }

    if(anc_type==ANC_FEEDBACK)
    {

#ifdef ANC_FB_CHECK
        hal_codec_anc_fb_check_set_irq_handler(anc_fb_check_irq_handler);
#endif
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            max_fb_gain_l=0;
#ifdef ANC_FB_CHECK
            fb_check_ch0_config->codec_fb_check_enable_ch0=0;
            *codec_int_config_clr=CODEC_FB_CHECK_ERROR_TRIG_CH0;
            codec_mask_config->fb_check_error_trig_ch0_mask=1;
            feedback_mc_settings->codec_feedback_mc_en_ch0=1;

            fb_check_ch0_config->codec_fb_check_acc_sample_rate_ch0=3;
            fb_check_ch0_config->codec_fb_check_src_sel_ch0=0;
            fb_check_ch0_config->codec_fb_check_acc_window_ch0=64;
            fb_check_ch0_config->codec_fb_check_trig_window_ch0=64;

            *codec_fb_check_threshold_ch0=0x4000000;

            fb_check_ch0_config->codec_fb_check_enable_ch0=1;

#endif
        }

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            max_fb_gain_r=0;
#ifdef ANC_FB_CHECK
        fb_check_ch1_config->codec_fb_check_enable_ch1=0;

        *codec_int_config_clr=CODEC_FB_CHECK_ERROR_TRIG_CH1;
        codec_mask_config->fb_check_error_trig_ch1_mask=1;
        feedback_mc_settings->codec_feedback_mc_en_ch1=1;

        fb_check_ch1_config->codec_fb_check_acc_sample_rate_ch1=3;
        fb_check_ch1_config->codec_fb_check_src_sel_ch1=0;
        fb_check_ch1_config->codec_fb_check_acc_window_ch1=64;
        fb_check_ch1_config->codec_fb_check_trig_window_ch1=64;

        *codec_fb_check_threshold_ch1=0x4000000;

        fb_check_ch1_config->codec_fb_check_enable_ch1=1;
#endif
        }
        fb_open_flag=1;
#if defined(AUDIO_ANC_FB_MC_HW)
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            max_mc_gain_l=0;
        }
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
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            max_ff_gain_l=0;
        }

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            max_ff_gain_r=0;
        }

        ff_open_flag=0;

#if defined(AUDIO_ANC_TT_HW)
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            max_tt_gain_l=0;
        }
#endif
    }

    if(anc_type==ANC_FEEDBACK)
    {
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            max_fb_gain_l=0;

#ifdef ANC_FB_CHECK
            fb_check_ch0_config->codec_fb_check_enable_ch0=0;
            *codec_int_config_clr=CODEC_FB_CHECK_ERROR_TRIG_CH0;
            codec_mask_config->fb_check_error_trig_ch0_mask=0;
#endif
        }

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            max_fb_gain_r=0;

#ifdef ANC_FB_CHECK
            fb_check_ch1_config->codec_fb_check_enable_ch1=0;
            *codec_int_config_clr=CODEC_FB_CHECK_ERROR_TRIG_CH1;
            codec_mask_config->fb_check_error_trig_ch1_mask=0;
#endif
        }
         fb_open_flag=0;
#if defined(AUDIO_ANC_FB_MC_HW)
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            max_mc_gain_l=0;
        }
#endif
    }

    anc_ctrl_reg_close(anc_type);

    if(ff_open_flag==0&&fb_open_flag==0)
    {
        hal_cmu_codec_iir_disable();
    }

    return;
}

int anc_enable( void)
{
    LOG_I("%s", __func__);

    anc_cfg_lock();

    if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
    {
        anc_control->codec_anc_enable_ch0=1;
#ifdef ANC_FB_CHECK
        fb_check_ch0_config->codec_fb_check_enable_ch0=1;
#endif
    }

     if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
    {
        anc_control->codec_anc_enable_ch1=1;
#ifdef ANC_FB_CHECK
        fb_check_ch1_config->codec_fb_check_enable_ch1=1;
#endif
    }

#if defined(AUDIO_ANC_TT_HW)
    tt_mc_control->codec_tt_enable=1;
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    tt_mc_control->codec_mm_enable=1;
#endif

    anc_cfg_unlock();

    return ANC_NO_ERR;
}

int anc_disable(void)
{
    LOG_I("%s", __func__);

    anc_cfg_lock();
    if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
    {
#ifdef ANC_FB_CHECK
        fb_check_ch0_config->codec_fb_check_enable_ch0=0;
        hwtimer_stop(fb_adc_check_dev_timer);
#endif
        anc_control->codec_anc_enable_ch0=0;
    }

     if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
    {
#ifdef ANC_FB_CHECK
        fb_check_ch1_config->codec_fb_check_enable_ch1=0;
        hwtimer_stop(fb_adc_check_dev_timer);
#endif
        anc_control->codec_anc_enable_ch1=0;
    }

#if defined(AUDIO_ANC_TT_HW)
    tt_mc_control->codec_tt_enable=0;
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    tt_mc_control->codec_mm_enable=0;
#endif

    anc_cfg_unlock();

    return ANC_NO_ERR;
}

int anc_set_gain(int32_t gain_ch_l, int32_t gain_ch_r,enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err=ANC_NO_ERR;

    // LOG_I("anc_set_gain gain_ch_l:%d,gain_ch_r:%d",gain_ch_l,gain_ch_r);
    anc_cfg_lock();

    if(anc_type==ANC_FEEDFORWARD)
    {
        //Set the FF gain;
      //  if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            anc_ff_gain->codec_anc_mute_gain_ff_ch0=gain_ch_l;
        }

     //    if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            anc_ff_gain->codec_anc_mute_gain_ff_ch1=gain_ch_r;
        }
    }
    else if(anc_type==ANC_FEEDBACK)
    {
        //Set the FB gain;
      //  if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            anc_fb_gain->codec_anc_mute_gain_fb_ch0=gain_ch_l;
        }

       //  if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            anc_fb_gain->codec_anc_mute_gain_fb_ch1=gain_ch_r;
        }
    }
#if defined(AUDIO_ANC_TT_HW)
    else if(anc_type==ANC_TALKTHRU)
    {
        //Set the TT gain;
        //if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            tt_mc_control->codec_mute_gain_coef_tt=gain_ch_l;
        }
    }
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    else if(anc_type==ANC_MUSICCANCLE)
    {
        //Set the MC gain;
        tt_mc_control->codec_mute_gain_coef_mm=gain_ch_l;
     }
#endif
    else
    {
        //if err ANC type,retun 0;
       // if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            anc_fb_gain->codec_anc_mute_gain_fb_ch0=0;
        }

         //if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            anc_fb_gain->codec_anc_mute_gain_fb_ch1=0;
        }
        err=ANC_TYPE_ERR;
        LOG_I("Error ANC type:%d", anc_type);
    }

    anc_cfg_unlock();

	return err;
}

int anc_set_gain_f32(float gain_l, float gain_r, enum ANC_TYPE_T type)
{
    if (type == ANC_FEEDFORWARD) {
        anc_set_gain((int32_t)(max_ff_gain_l * gain_l), (int32_t)(max_ff_gain_r * gain_r), type);
    } else if (type == ANC_FEEDBACK) {
        anc_set_gain((int32_t)(max_fb_gain_l * gain_l), (int32_t)(max_fb_gain_r * gain_r), type);
#if defined(AUDIO_ANC_TT_HW)
    } else if (type == ANC_TALKTHRU) {
        anc_set_gain((int32_t)(max_tt_gain_l * gain_l), 0, type);
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    } else if (type == ANC_MUSICCANCLE) {
        anc_set_gain((int32_t)(max_mc_gain_l * gain_l), 0, type);
#endif
    }

    return 0;
}

int anc_get_gain(int32_t *gain_ch_l, int32_t *gain_ch_r,enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err=ANC_NO_ERR;

    anc_cfg_lock();

    if(anc_type==ANC_FEEDFORWARD)
    {
        //Get the FF gain;
        //if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            *gain_ch_l=anc_ff_gain->codec_anc_mute_gain_ff_ch0;
        }

         //if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            *gain_ch_r=anc_ff_gain->codec_anc_mute_gain_ff_ch1;
        }
    }
    else if(anc_type==ANC_FEEDBACK)
    {
        //Get the FB gain;
        //if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            *gain_ch_l=anc_fb_gain->codec_anc_mute_gain_fb_ch0;
        }
         //if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            *gain_ch_r=anc_fb_gain->codec_anc_mute_gain_fb_ch1;
        }
    }
#if defined(AUDIO_ANC_TT_HW)
    else if(anc_type==ANC_TALKTHRU)
    {
        //Set the TT gain;
        //if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            *gain_ch_l=tt_mc_control->codec_mute_gain_coef_tt;
        }
    }
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    else if(anc_type==ANC_MUSICCANCLE)
    {
        //Set the MC gain;
         *gain_ch_l=tt_mc_control->codec_mute_gain_coef_mm;
     }
#endif
    else
    {
         //if err ANC type,retun 0;
        //if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            *gain_ch_l=0;
        }

         //if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            *gain_ch_r=0;
        }
        err=ANC_TYPE_ERR;
        LOG_I("Error ANC type:%d", anc_type);
    }
    anc_cfg_unlock();

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
        //if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            *gain_ch_l = max_ff_gain_l;
        }

         //if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            *gain_ch_r = max_ff_gain_r;
        }
    }
    else if(anc_type==ANC_FEEDBACK)
    {
        //Get the max FB gain;
        //if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            *gain_ch_l = max_fb_gain_l;
        }

        //if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            *gain_ch_r = max_fb_gain_r;
        }
    }
#if defined(AUDIO_ANC_TT_HW)
    else if(anc_type==ANC_TALKTHRU)
    {
        //Get the TT gain;
        //if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            *gain_ch_l=max_tt_gain_l;
        }
    }
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    else if(anc_type==ANC_MUSICCANCLE)
    {
        //Get the TT gain;
         *gain_ch_l=max_mc_gain_l;
    }
#endif
    else
    {
        //if err ANC type,retun 0;
        //if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
             *gain_ch_l=0;
        }

        //if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            *gain_ch_r=0;
        }
        err=ANC_TYPE_ERR;
        LOG_I("Error ANC type:%d", anc_type);
    }
    anc_cfg_unlock();

    //	LOG_I("anc_get_cfg_gain gain_ch_l:%d,gain_ch_r:%d",*gain_ch_l,*gain_ch_r);
    return err;
}

void  anc_set_ch_map( int32_t ch_map )
{
    anc_output_ch_map=ch_map;
    return;
}

void  anc_howling_check_enable(int32_t flag )
{
#ifdef ANC_FB_CHECK
    if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
    {
        if(flag==0)
        {
            fb_check_ch0_config->codec_fb_check_enable_ch0=0;
        }
        else
        {
            fb_check_ch0_config->codec_fb_check_enable_ch0=1;
        }
     }

    if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
    {
        if(flag==0)
        {
            fb_check_ch1_config->codec_fb_check_enable_ch1=0;
        }
        else
        {
            fb_check_ch1_config->codec_fb_check_enable_ch1=1;
        }
     }
#endif
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
#if defined(AUDIO_ANC_FB_MC) ||defined(AUDIO_ANC_FB_MC_HW)
    LOG_I("%s:data_select:%d", __func__,data_select);

    switch(data_select)
    {
        case ANC_ADC_ONLY_ADC:
        fb_check_ch0_config->codec_fb_check_keep_ch0=0;
        feedback_mc_settings->codec_feedback_mc_en_ch0=1;
        fb_check_ch1_config->codec_fb_check_keep_ch1=0;
        feedback_mc_settings->codec_feedback_mc_en_ch1=1;
        break;

        case ANC_ADC_ONLY_MC:
        fb_check_ch0_config->codec_fb_check_keep_ch0=1;
        feedback_mc_settings->codec_feedback_mc_en_ch0=0;
        fb_check_ch1_config->codec_fb_check_keep_ch1=1;
        feedback_mc_settings->codec_feedback_mc_en_ch1=0;		
        break;

        case ANC_ADC_ADC_ADD_MC:
        fb_check_ch0_config->codec_fb_check_keep_ch0=1;
        feedback_mc_settings->codec_feedback_mc_en_ch0=1;
        fb_check_ch1_config->codec_fb_check_keep_ch1=1;
        feedback_mc_settings->codec_feedback_mc_en_ch1=1;		
        break;

        default:
        fb_check_ch0_config->codec_fb_check_keep_ch0=0;
        feedback_mc_settings->codec_feedback_mc_en_ch0=1;
        fb_check_ch1_config->codec_fb_check_keep_ch1=0;
        feedback_mc_settings->codec_feedback_mc_en_ch1=1;		
        break;
     }
#endif
    return 0;
}

