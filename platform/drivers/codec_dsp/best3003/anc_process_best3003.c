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
1	d4	codec_anc_mute_gain_update_ff_ch0	RW	0	26	1: gain update
1	d4	codec_anc_mute_gain_update_ff_ch1	RW	0	27
28	d4		x
*/
struct _anc_ff_gain
{
	int32 codec_anc_mute_gain_ff_ch0 : 12;
	int32 codec_anc_mute_gain_ff_ch1 : 12;

	int32 codec_anc_mute_gain_pass0_ff_ch0 : 1;
	int32 codec_anc_mute_gain_pass0_ff_ch1 : 1;

	int32 codec_anc_mute_gain_update_ff_ch0 : 1;
	int32 codec_anc_mute_gain_update_ff_ch1 : 1;

	uint32 Reserved : 4;
};

/*
12	d8	codec_anc_mute_gain_fb_ch0	RW	0	0	same as above
12	d8	codec_anc_mute_gain_fb_ch1	RW	0	12
1	d8	codec_anc_mute_gain_pass0_fb_ch0	RW	0	24
1	d8	codec_anc_mute_gain_pass0_fb_ch1	RW	0	25
1	d8	codec_anc_mute_gain_update_fb_ch0	RW	0	26
1	d8	codec_anc_mute_gain_update_fb_ch1	RW	0	27
28	d8		x
*/
struct _anc_fb_gain
{
	int32 codec_anc_mute_gain_fb_ch0 : 12;
	int32 codec_anc_mute_gain_fb_ch1 : 12;

	int32 codec_anc_mute_gain_pass0_fb_ch0 : 1;
	int32 codec_anc_mute_gain_pass0_fb_ch1 : 1;

	int32 codec_anc_mute_gain_update_fb_ch0 : 1;
	int32 codec_anc_mute_gain_update_fb_ch1 : 1;

	uint32 Reserved : 4;
};

/*
1	2a0	codec_iir_gaincal_ext_ch0_bypass	RW	1	0	 =1, bypass extern gain ramp calc for iir ch0
1	2a0	codec_iir_gaincal_ext_ch1_bypass	RW	1	1	 =1, bypass extern gain ramp calc for iir ch1
1	2a0	codec_iir_gaincal_ext_ch2_bypass	RW	1	2	 =1, bypass extern gain ramp calc for iir ch2
1	2a0	codec_iir_gaincal_ext_ch3_bypass	RW	1	3	 =1, bypass extern gain ramp calc for iir ch3
1	2a0	codec_iir_gainuse_ext_ch0_bypass	RW	1	4	 =1, bypass, not use extern gain for iir ch0
1	2a0	codec_iir_gainuse_ext_ch1_bypass	RW	1	5	 =1, bypass, not use extern gain for iir ch1
1	2a0	codec_iir_gainuse_ext_ch2_bypass	RW	1	6	 =1, bypass, not use extern gain for iir ch2
1	2a0	codec_iir_gainuse_ext_ch3_bypass	RW	1	7	 =1, bypass, not use extern gain for iir ch3
1	2a0	codec_iir_gain_ext_update_ch0	RW	0	8	  posedge to update extern gain for iir ch0
1	2a0	codec_iir_gain_ext_update_ch1	RW	0	9	  posedge to update extern gain for iir ch1
1	2a0	codec_iir_gain_ext_update_ch2	RW	0	10	  posedge to update extern gain for iir ch2
1	2a0	codec_iir_gain_ext_update_ch3	RW	0	11	  posedge to update extern gain for iir ch3
1	2a0	codec_iir_gain_ext_sel_ch0	RW	0	12	 =1, extern gain updated when pass0; =0, updated right away
1	2a0	codec_iir_gain_ext_sel_ch1	RW	0	13	 =1, extern gain updated when pass0; =0, updated right away
1	2a0	codec_iir_gain_ext_sel_ch2	RW	0	14	 =1, extern gain updated when pass0; =0, updated right away
1	2a0	codec_iir_gain_ext_sel_ch3	RW	0	15	 =1, extern gain updated when pass0; =0, updated right away
16	2a0		x
*/

struct _anc_iir_gain_config
{
	uint32 codec_iir_gaincal_ext_ch0_bypass : 1;
	uint32 codec_iir_gaincal_ext_ch1_bypass : 1;
	uint32 codec_iir_gaincal_ext_ch2_bypass : 1;
	uint32 codec_iir_gaincal_ext_ch3_bypass : 1;

	uint32 codec_iir_gainuse_ext_ch0_bypass : 1;
	uint32 codec_iir_gainuse_ext_ch1_bypass : 1;
	uint32 codec_iir_gainuse_ext_ch2_bypass : 1;
	uint32 codec_iir_gainuse_ext_ch3_bypass : 1;

	uint32 codec_iir_gain_ext_update_ch0 : 1;
	uint32 codec_iir_gain_ext_update_ch1 : 1;
	uint32 codec_iir_gain_ext_update_ch2 : 1;
	uint32 codec_iir_gain_ext_update_ch3 : 1;

	uint32 codec_iir_gain_ext_sel_ch0 : 1;
	uint32 codec_iir_gain_ext_sel_ch1 : 1;
	uint32 codec_iir_gain_ext_sel_ch2 : 1;
	uint32 codec_iir_gain_ext_sel_ch3 : 1;

       uint32 Reserved : 16;
};


/*
16	340	codec_anc_calib_gain_coef_ff_ch0	RW	4096	0	Format 3.9, for FF
16	340	codec_anc_calib_gain_coef_ff_ch1	RW	4096	16
32	340		x
*/

struct _anc_ff_iir_calib_gain
{
	uint32 codec_anc_calib_gain_coef_ff_ch0 : 16;
	uint32 codec_anc_calib_gain_coef_ff_ch1 : 16;
};

/*
16	344	codec_anc_calib_gain_coef_fb_ch0	RW	4096	0
16	344	codec_anc_calib_gain_coef_fb_ch1	RW	4096	16
32	344		x
*/
struct _anc_fb_iir_calib_gain
{
	uint32 codec_anc_calib_gain_coef_fb_ch0 : 16;
	uint32 codec_anc_calib_gain_coef_fb_ch1 : 16;
};

/*
1	348	codec_anc_calib_gain_pass0_ff_ch0	RW	0	0	1: gain update no pass0
1	348	codec_anc_calib_gain_pass0_ff_ch1	RW	0	1
1	348	codec_anc_calib_gain_update_ff_ch0	RW	0	2	1: gain update
1	348	codec_anc_calib_gain_update_ff_ch1	RW	0	3
1	348	codec_anc_calib_gain_pass0_fb_ch0	RW	0	4	1: gain update no pass0
1	348	codec_anc_calib_gain_pass0_fb_ch1	RW	0	5
1	348	codec_anc_calib_gain_update_fb_ch0	RW	0	6	1: gain update
1	348	codec_anc_calib_gain_update_fb_ch1	RW	0	7
8	348		x
*/
struct _anc_iir_calib_gain_config
{
	uint32 codec_anc_calib_gain_pass0_ff_ch0 : 1;
	uint32 codec_anc_calib_gain_pass0_ff_ch1 : 1;

	uint32 codec_anc_calib_gain_update_ff_ch0 : 1;
	uint32 codec_anc_calib_gain_update_ff_ch1 : 1;

	uint32 codec_anc_calib_gain_pass0_fb_ch0 : 1;
	uint32 codec_anc_calib_gain_pass0_fb_ch1 : 1;

	uint32 codec_anc_calib_gain_update_fb_ch0 : 1;
	uint32 codec_anc_calib_gain_update_fb_ch1 : 1;

       uint32 Reserved : 24;
};

/*
iir coefficients(b/a Q27)
b2
b1
b0
a2
a1
*/
struct _anc_iir_coefs
{
	int32 b2 ;
	int32 b1 ;
	int32 b0 ;

    	int32 a2 ;
	int32 a1 ;
};


/*

3	104	fir_access_offset_ch0	RW	0	0
3	104	fir_access_offset_ch1	RW	0	3
3	104	fir_access_offset_ch2	RW	0	6
3	104	fir_access_offset_ch3	RW	0	9
1	104	pdu_fs_swap	RW	0	12
1	104	anc_coef_sel_pdu0_pdu1	RW	0	13
1	104	anc_coef_sel_fs0_fs1	RW	0	14
1	104	anc_coef_sel_pdu0_fs0	RW	0	15
1	104	anc_coef_sel_pdu1_fs1	RW	0	16
1	104	anc_coef_sel_pdu0_pdu1_new	R	0	17
1	104	anc_coef_sel_fs0_fs1_new	R	0	18
1	104	anc_coef_sel_pdu0_fs0_new	R	0	19
1	104	anc_coef_sel_pdu1_fs1_new	R	0	20
21	104		x

*/

struct _fir_config
{
	uint32 fir_access_offset_ch0 : 3;
	uint32 fir_access_offset_ch1 : 3;
	uint32 fir_access_offset_ch2 : 3;
	uint32 fir_access_offset_ch3 : 3;

	uint32 pdu_fs_swap : 1;

	uint32 anc_coef_sel_pdu0_pdu1 : 1;
	uint32 anc_coef_sel_fs0_fs1 : 1;
	uint32 anc_coef_sel_pdu0_fs0 : 1;
	uint32 anc_coef_sel_pdu1_fs1 : 1;

	uint32 anc_coef_sel_pdu0_pdu1_new : 1;
	uint32 anc_coef_sel_fs0_fs1_new : 1;
	uint32 anc_coef_sel_pdu0_fs0_new : 1;
	uint32 anc_coef_sel_pdu1_fs1_new : 1;

	uint32 Reserved : 11;
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
	uint32 fir_do_remap_ch0 : 1;
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

#ifdef ANC_FB_CHECK
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

    uint32 Reserved : 5;
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

    uint32 Reserved : 5;
};

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
1	2b8	codec_tm_iir_enable	RW	0	0	tt/mc iir module enable
1	2b8	codec_tm_iir_ch0_bypass	RW	0	1	1: ch0 bypass
1	2b8	codec_tm_iir_ch1_bypass	RW	0	2
1	2b8	codec_tm_iir_ch2_bypass	RW	0	3
1	2b8	codec_tm_iir_ch3_bypass	RW	0	4
4	2b8	codec_tm_iir_count_ch0	RW	0	5	ch0 iir number  0~8
4	2b8	codec_tm_iir_count_ch1	RW	0	9
4	2b8	codec_tm_iir_count_ch2	RW	0	13
4	2b8	codec_tm_iir_count_ch3	RW	0	17
1	2b8	codec_tm_iir_coef_swap	RW	0	21	0: use iir coef memory0;  1: use iir coef memory1;
1	2b8	tm_iir_coef_swap_status_sync[1]	R	0	22	read only;  coef memoryX used currently	��	��	��
*/
struct _tt_mc_control
{
    uint32 codec_tm_iir_enable : 1;

    uint32 codec_tm_iir_ch0_bypass : 1;
    uint32 codec_tm_iir_ch1_bypass : 1;
    uint32 codec_tm_iir_ch2_bypass : 1;
    uint32 codec_tm_iir_ch3_bypass : 1;

    uint32 codec_tm_iir_count_ch0 : 4;
    uint32 codec_tm_iir_count_ch1 : 4;
    uint32 codec_tm_iir_count_ch2 : 4;
    uint32 codec_tm_iir_count_ch3 : 4;

    uint32 codec_tm_iir_coef_swap : 1;
    uint32 tm_iir_coef_swap_status_sync : 1;

    uint32 Reserved : 9;
};

/*
1	2bc	codec_tm_iir_gaincal_ext_ch0_bypass	RW	1	0	same as anc iir in reg_2a0
1	2bc	codec_tm_iir_gaincal_ext_ch1_bypass	RW	1	1
1	2bc	codec_tm_iir_gaincal_ext_ch2_bypass	RW	1	2
1	2bc	codec_tm_iir_gaincal_ext_ch3_bypass	RW	1	3
1	2bc	codec_tm_iir_gainuse_ext_ch0_bypass	RW	1	4
1	2bc	codec_tm_iir_gainuse_ext_ch1_bypass	RW	1	5
1	2bc	codec_tm_iir_gainuse_ext_ch2_bypass	RW	1	6
1	2bc	codec_tm_iir_gainuse_ext_ch3_bypass	RW	1	7
1	2bc	codec_tm_iir_gain_ext_update_ch0	RW	0	8
1	2bc	codec_tm_iir_gain_ext_update_ch1	RW	0	9
1	2bc	codec_tm_iir_gain_ext_update_ch2	RW	0	10
1	2bc	codec_tm_iir_gain_ext_update_ch3	RW	0	11
1	2bc	codec_tm_iir_gain_ext_sel_ch0	RW	0	12
1	2bc	codec_tm_iir_gain_ext_sel_ch1	RW	0	13
1	2bc	codec_tm_iir_gain_ext_sel_ch2	RW	0	14
1	2bc	codec_tm_iir_gain_ext_sel_ch3	RW	0	15
16	2bc		x
*/

struct _tm_iir_gain_config
{
	uint32 codec_tm_iir_gaincal_ext_ch0_bypass : 1;
	uint32 codec_tm_iir_gaincal_ext_ch1_bypass : 1;
	uint32 codec_tm_iir_gaincal_ext_ch2_bypass : 1;
	uint32 codec_tm_iir_gaincal_ext_ch3_bypass : 1;

	uint32 codec_tm_iir_gainuse_ext_ch0_bypass : 1;
	uint32 codec_tm_iir_gainuse_ext_ch1_bypass : 1;
	uint32 codec_tm_iir_gainuse_ext_ch2_bypass : 1;
	uint32 codec_tm_iir_gainuse_ext_ch3_bypass : 1;

	uint32 codec_tm_iir_gain_ext_update_ch0 : 1;
	uint32 codec_tm_iir_gain_ext_update_ch1 : 1;
	uint32 codec_tm_iir_gain_ext_update_ch2 : 1;
	uint32 codec_tm_iir_gain_ext_update_ch3 : 1;

	uint32 codec_tm_iir_gain_ext_sel_ch0 : 1;
	uint32 codec_tm_iir_gain_ext_sel_ch1 : 1;
	uint32 codec_tm_iir_gain_ext_sel_ch2 : 1;
	uint32 codec_tm_iir_gain_ext_sel_ch3 : 1;

       uint32 Reserved : 16;
};

/*
1	2d0	codec_tt_enable_ch0	RW	0	0	talk through enable
3	2d0	codec_tt_adc_sel_ch0	RW	0	1	talk through adc sel: 0, ff_ch0; 1, ff_ch1; 2, fb_ch0; 3, fb_ch1; 4, adc_ch4
1	2d0	codec_mm_enable_ch0	RW	0	4	music cancel enable
1	2d0	codec_mm_fifo_en_ch0	RW	0	5	music cancel fifo enable
1	2d0	codec_mm_fifo_bypass_ch0	RW	0	6	music cancel fifo bypass
5	2d0	codec_mm_delay_ch0	RW	0	7	music cancel delay
1	2d0	codec_tt_enable_ch1	RW	0	12	talk through enable
3	2d0	codec_tt_adc_sel_ch1	RW	1	13	talk through adc sel: 0, ff_ch0; 1, ff_ch1; 2, fb_ch0; 3, fb_ch1; 4, adc_ch4
1	2d0	codec_mm_enable_ch1	RW	0	16	music cancel enable
1	2d0	codec_mm_fifo_en_ch1	RW	0	17	music cancel fifo enable
1	2d0	codec_mm_fifo_bypass_ch1	RW	0	18	music cancel fifo bypass
5	2d0	codec_mm_delay_ch1	RW	0	19	music cancel delay
24	2d0		x
*/
struct _tm_config
{
    uint32 codec_tt_enable_ch0 : 1;
    uint32 codec_tt_adc_sel_ch0 : 3;
    uint32 codec_mm_enable_ch0 : 1;
    uint32 codec_mm_fifo_en_ch0 : 1;
    uint32 codec_mm_fifo_bypass_ch0 : 1;
    uint32 codec_mm_delay_ch0 : 5;

    uint32 codec_tt_enable_ch1 : 1;
    uint32 codec_tt_adc_sel_ch1 : 3;
    uint32 codec_mm_enable_ch1 : 1;
    uint32 codec_mm_fifo_en_ch1 : 1;
    uint32 codec_mm_fifo_bypass_ch1 : 1;
    uint32 codec_mm_delay_ch1 : 5;

    uint32 Reserved : 8;
};


/*
12	2d4	codec_mute_gain_coef_tt_ch0	RW	0	0	talk through gain
1	2d4	codec_mute_gain_pass0_tt_ch0	RW	0	12	talk through gain pass 0 effect
1	2d4	codec_mute_gain_update_tt_ch0	RW	0	13	talk through gain update
12	2d4	codec_mute_gain_coef_tt_ch1	RW	0	14	talk through gain
1	2d4	codec_mute_gain_pass0_tt_ch1	RW	0	26	talk through gain pass 0 effect
1	2d4	codec_mute_gain_update_tt_ch1	RW	0	27	talk through gain update
28	2d4		x
*/

struct _anc_tt_gain
{
	int32 codec_mute_gain_coef_tt_ch0 : 12;
	int32 codec_mute_gain_pass0_tt_ch0 : 1;
	int32 codec_mute_gain_update_tt_ch0 : 1;

	int32 codec_mute_gain_coef_tt_ch1 : 12;
	int32 codec_mute_gain_pass0_tt_ch1 : 1;
	int32 codec_mute_gain_update_tt_ch1 : 1;

	uint32 Reserved : 4;
};

/*
12	2d8	codec_mute_gain_coef_mc_ch0	RW	0	0	music cancel gain
1	2d8	codec_mute_gain_pass0_mc_ch0	RW	0	12	music cancel gain pass 0
1	2d8	codec_mute_gain_update_mc_ch0	RW	0	13	music cancel gain update
12	2d8	codec_mute_gain_coef_mc_ch1	RW	0	14	music cancel gain
1	2d8	codec_mute_gain_pass0_mc_ch1	RW	0	26	music cancel gain pass 0
1	2d8	codec_mute_gain_update_mc_ch1	RW	0	27	music cancel gain update
28	2d8		x
*/
struct _anc_mc_gain
{
	int32 codec_mute_gain_coef_mc_ch0 : 12;
	int32 codec_mute_gain_pass0_mc_ch0 : 1;
	int32 codec_mute_gain_update_mc_ch0 : 1;

	int32 codec_mute_gain_coef_mc_ch1 : 12;
	int32 codec_mute_gain_pass0_mc_ch1 : 1;
	int32 codec_mute_gain_update_mc_ch1 : 1;

	uint32 Reserved : 4;
};
#endif


#define IIR_COUNTER (8)
#define FIR_LEN (400)


typedef struct _iir_parameter
{
    int32_t total_gain;

    uint16_t iir_bypass_flag;
    uint16_t iir_counter;

    anc_iir_coefs iir_coef[IIR_COUNTER];

} iir_parameter;


#define GAIN_Q (9)
#define ANC_GAIN_RAMP
#define AUDIO_ANC_FIR_HW
#define CALIB_GAIN_Q (12)

#ifdef ANC_GAIN_RAMP

#define FIXED_GAIN_RAMP_Q (1<<27)
static const anc_iir_coefs   iir_coef_gain_ramp_ff=
{
    .coef_b={            8,           16,            8},
    .coef_a={134217728,   -268106408,    133888712},
};

static const anc_iir_coefs   iir_coef_gain_ramp_fb=
{
    .coef_b={            8,           16,            8},
    .coef_a={134217728,   -268106408,    133888712},
};
#if defined(AUDIO_ANC_TT_HW)
static const anc_iir_coefs   iir_coef_gain_ramp_tt=
{
    .coef_b={            8,           16,            8},
    .coef_a={134217728,   -268106408,    133888712},
};
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
static const anc_iir_coefs   iir_coef_gain_ramp_mc=
{
    .coef_b={            8,           16,            8},
    .coef_a={134217728,   -268106408,    133888712},
};
#endif
#endif


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

//calib gain
volatile static struct _anc_ff_iir_calib_gain *anc_ff_iir_calib_gain=(volatile struct _anc_ff_iir_calib_gain *)(ANC_BASE+0x340);
volatile static struct _anc_fb_iir_calib_gain *anc_fb_iir_calib_gain=(volatile struct _anc_fb_iir_calib_gain *)(ANC_BASE+0x344);
volatile static struct _anc_iir_calib_gain_config *anc_iir_calib_gain_config=(volatile struct _anc_iir_calib_gain_config *)(ANC_BASE+0x348);

#ifdef ANC_GAIN_RAMP
volatile static struct _anc_iir_gain_config *anc_iir_gain_config=(volatile struct _anc_iir_gain_config *)(ANC_BASE+0x2a0);

volatile static struct _anc_iir_coefs *anc_gain_ff_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x2280);
volatile static struct _anc_iir_coefs *anc_gain_ff_iir_coefs0_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x2294);
volatile static struct _anc_iir_coefs *anc_gain_fb_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x22a8);
volatile static struct _anc_iir_coefs *anc_gain_fb_iir_coefs0_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x22bc);

volatile static struct _anc_iir_coefs *anc_gain_ff_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x2680);
volatile static struct _anc_iir_coefs *anc_gain_ff_iir_coefs1_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x2694);
volatile static struct _anc_iir_coefs *anc_gain_fb_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x26a8);
volatile static struct _anc_iir_coefs *anc_gain_fb_iir_coefs1_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x26bc);

volatile static int32 *codec_iir_gain_ext_ff_l=(volatile int32 *)(ANC_BASE+0x02a4);
volatile static int32 *codec_iir_gain_ext_ff_r=(volatile int32 *)(ANC_BASE+0x02a8);
volatile static int32 *codec_iir_gain_ext_fb_l=(volatile int32 *)(ANC_BASE+0x02ac);
volatile static int32 *codec_iir_gain_ext_fb_r=(volatile int32 *)(ANC_BASE+0x02b0);
#endif


//FIR registers
volatile static struct _fir_control *fir_control=(volatile struct _fir_control *)(ANC_BASE+0x100);

//TT and MC registers
#if defined(AUDIO_ANC_TT_HW)|| defined(AUDIO_ANC_FB_MC_HW)
volatile static struct _tt_mc_control *tt_mc_control=(volatile struct _tt_mc_control *)(ANC_BASE+0x2b8);
volatile static struct _tm_config *tm_config=(volatile struct _tm_config *)(ANC_BASE+0x2d0);
#ifdef ANC_GAIN_RAMP
volatile static struct _tm_iir_gain_config *tm_iir_gain_config=(volatile struct _tm_iir_gain_config *)(ANC_BASE+0x2bc);
#endif
#endif


#if defined(AUDIO_ANC_TT_HW)
volatile static struct _anc_tt_gain *anc_tt_gain=(volatile struct _anc_tt_gain *)(ANC_BASE+0x2d4);

volatile static struct _anc_iir_coefs *anc_tt_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x3000);
volatile static struct _anc_iir_coefs *anc_tt_iir_coefs0_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x30a0);

volatile static struct _anc_iir_coefs *anc_tt_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x3400);
volatile static struct _anc_iir_coefs *anc_tt_iir_coefs1_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x34a0);

#ifdef ANC_GAIN_RAMP
volatile static struct _anc_iir_coefs *anc_gain_tt_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x3280);
volatile static struct _anc_iir_coefs *anc_gain_tt_iir_coefs0_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x3294);

volatile static struct _anc_iir_coefs *anc_gain_tt_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x3680);
volatile static struct _anc_iir_coefs *anc_gain_tt_iir_coefs1_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x3694);

volatile static int32 *codec_tm_iir_gain_ext_tt_l=(volatile int32 *)(ANC_BASE+0x02c0);
volatile static int32 *codec_tm_iir_gain_ext_tt_r=(volatile int32 *)(ANC_BASE+0x02c4);
#endif
#endif

#if defined(AUDIO_ANC_FB_MC_HW)
volatile static struct _anc_mc_gain *anc_mc_gain=(volatile struct _anc_mc_gain *)(ANC_BASE+0x2d8);

volatile static struct _anc_iir_coefs *anc_mc_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x3140);
volatile static struct _anc_iir_coefs *anc_mc_iir_coefs0_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x31e0);

volatile static struct _anc_iir_coefs *anc_mc_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x3540);
volatile static struct _anc_iir_coefs *anc_mc_iir_coefs1_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x35e0);

#ifdef ANC_GAIN_RAMP
volatile static struct _anc_iir_coefs *anc_gain_mc_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x32a8);
volatile static struct _anc_iir_coefs *anc_gain_mc_iir_coefs0_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x32bc);

volatile static struct _anc_iir_coefs *anc_gain_mc_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x36a8);
volatile static struct _anc_iir_coefs *anc_gain_mc_iir_coefs1_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x36bc);

volatile static int32 *codec_tm_iir_gain_ext_mc_l=(volatile int32 *)(ANC_BASE+0x02c8);
volatile static int32 *codec_tm_iir_gain_ext_mc_r=(volatile int32 *)(ANC_BASE+0x02cc);
#endif
#endif

#if defined(AUDIO_ANC_FIR_HW)
volatile static struct _fir_config *fir_config=(volatile struct _fir_config *)(ANC_BASE+0x104);

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
#endif

#ifdef ANC_FB_CHECK
volatile static struct _fb_check_ch0_config *fb_check_ch0_config=(volatile struct _fb_check_ch0_config *)(ANC_BASE+0x0130);
volatile static struct _fb_check_ch1_config *fb_check_ch1_config=(volatile struct _fb_check_ch1_config *)(ANC_BASE+0x0134);

volatile static uint32 *codec_fb_check_threshold_ch0=(volatile uint32 *)(ANC_BASE+0x0138);
volatile static uint32 *codec_fb_check_threshold_ch1=(volatile uint32 *)(ANC_BASE+0x013c);

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
volatile static int max_tt_gain_r=0;
#endif

#if defined(AUDIO_ANC_FB_MC_HW)
volatile static int max_mc_gain_l=0;
volatile static int max_mc_gain_r=0;
#endif

volatile static int ff_open_flag=0;
volatile static int fb_open_flag=0;

#if defined(AUDIO_ANC_TT_HW)
volatile static int tt_open_flag=0;
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
volatile static int mc_open_flag=0;
#endif

volatile static int anc_output_ch_map=AUD_CHANNEL_MAP_CH0|AUD_CHANNEL_MAP_CH1;

#ifdef ANC_GAIN_RAMP
volatile static int ff_ramp_gain_l=0;
volatile static int ff_ramp_gain_r=0;

volatile static int fb_ramp_gain_l=0;
volatile static int fb_ramp_gain_r=0;

#if defined(AUDIO_ANC_TT_HW)
volatile static int tt_ramp_gain_l=0;
volatile static int tt_ramp_gain_r=0;
#endif

#if defined(AUDIO_ANC_FB_MC_HW)
volatile static int mc_ramp_gain_l=0;
volatile static int mc_ramp_gain_r=0;
#endif

#endif

#ifdef ANC_FB_CHECK
static HWTIMER_ID fb_adc_check_dev_timer=NULL;
#endif

static iir_parameter ff_filtes_l_old;
static iir_parameter ff_filtes_r_old;
static iir_parameter fb_filtes_l_old;
static iir_parameter fb_filtes_r_old;

#if defined(AUDIO_ANC_TT_HW)
static iir_parameter tt_filtes_l_old;
static iir_parameter tt_filtes_r_old;
#endif

#if defined(AUDIO_ANC_FB_MC_HW)
static iir_parameter mc_filtes_l_old;
static iir_parameter mc_filtes_r_old;
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
volatile static int8_t tm_iir_coef_using;


/*******************************FIR hardware filter*********************************/
static void anc_cfg_lock(void)
{
	return;
}

static void anc_cfg_unlock(void)
{
	return;
}

#if defined(AUDIO_ANC_FIR_HW)
int anc_set_fir_cfg( struct_anc_fir_cfg * cfg, enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err=ANC_NO_ERR;
    int i;
    aud_fir_item *ff_filtes_l=&(cfg->anc_fir_cfg_ff_l);
    aud_fir_item *ff_filtes_r=&(cfg->anc_fir_cfg_ff_r);

    if(anc_type==ANC_FEEDFORWARD)
    {
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            //setting the feedforward FIR filters;
            if(ff_filtes_l->fir_bypass_flag==0)
            {
                if(ff_filtes_l->fir_len>0)
                {
                    if(fir_config->anc_coef_sel_pdu0_fs0_new==0)
                    {
                        //feedforward left ch FIR coefs settings
                        for(i=0;i<fir_ff_config0_l->fir_order;i++)
                        {
                            anc_fb_fir_coefs_l[i]=(int32)ff_filtes_l->fir_coef[i];
                        }
                        for(;i<FIR_LEN;i++)
                        {
                            anc_fb_fir_coefs_l[i]=0;
                        }
                        fir_config->anc_coef_sel_pdu0_fs0=1;
                        while(fir_config->anc_coef_sel_pdu0_fs0_new==0);
                    }
                    else
                    {
                        //feedforward left ch FIR coefs settings
                        for(i=0;i<fir_ff_config0_l->fir_order;i++)
                        {
                            anc_ff_fir_coefs_l[i]=(int32)ff_filtes_l->fir_coef[i];
                        }
                        for(;i<FIR_LEN;i++)
                        {
                            anc_ff_fir_coefs_l[i]=0;
                        }
                        fir_config->anc_coef_sel_pdu0_fs0=0;
                        while(fir_config->anc_coef_sel_pdu0_fs0_new!=0);
                    }
                    anc_control->codec_ff_ch0_fir_en=1;
                    LOG_I("FIR switching left!");
                }
                else
                {
                    anc_control->codec_ff_ch0_fir_en=0;
                    fir_ff_config0_l->fir_order=0;
                    LOG_I("Error ff_filtes_l FIR LEN:%d", ff_filtes_l->fir_len);
                    err=ANC_TYPE_ERR;
                }
            }
            else
            {
                LOG_I("warning FIR BYPASS!");
                fir_ff_config0_l->fir_order=0;
                anc_control->codec_ff_ch0_fir_en=0;
            }
        }

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            //setting the feedforward FIR filters;
            if(ff_filtes_r->fir_bypass_flag==0)
            {
                if(ff_filtes_r->fir_len>0)
                {
                    if(fir_config->anc_coef_sel_pdu1_fs1_new==0)
                    {
                        //feedforward left ch FIR coefs settings
                        for(i=0;i<fir_ff_config0_r->fir_order;i++)
                        {
                            anc_fb_fir_coefs_r[i]=(int32)ff_filtes_r->fir_coef[i];
                        }
                        for(;i<FIR_LEN;i++)
                        {
                            anc_fb_fir_coefs_r[i]=0;
                        }
                        fir_config->anc_coef_sel_pdu1_fs1=1;
                        while(fir_config->anc_coef_sel_pdu1_fs1_new==1);
                    }
                    else
                    {
                        //feedforward left ch FIR coefs settings
                        for(i=0;i<fir_ff_config0_r->fir_order;i++)
                        {
                            anc_ff_fir_coefs_r[i]=(int32)ff_filtes_r->fir_coef[i];
                        }
                        for(;i<FIR_LEN;i++)
                        {
                            anc_ff_fir_coefs_r[i]=0;
                        }
                        fir_config->anc_coef_sel_pdu1_fs1=0;
                        while(fir_config->anc_coef_sel_pdu1_fs1_new==0);
                    }
                    anc_control->codec_ff_ch1_fir_en=1;
                    LOG_I("FIR switching right!");
                }
                else
                {
                    anc_control->codec_ff_ch1_fir_en=0;
                    fir_ff_config0_r->fir_order=0;
                    LOG_I("Error ff_filtes_l FIR LEN:%d", ff_filtes_r->fir_len);
                    err=ANC_TYPE_ERR;
                }
            }
            else
            {
                LOG_I("warning FIR BYPASS!");
                fir_ff_config0_r->fir_order=0;
                anc_control->codec_ff_ch1_fir_en=0;
            }
        }
    }

    return err;
}
#endif
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
    const aud_item *fb_filtes_l;
    const aud_item *fb_filtes_r;

#if defined(AUDIO_ANC_TT_HW)
    const aud_item *tt_filtes_l;
    const aud_item *tt_filtes_r;
#endif

#if defined(AUDIO_ANC_FB_MC_HW)
    const aud_item *mc_filtes_l;
    const aud_item *mc_filtes_r;
#endif

//    LOG_I("%s*******anc_type:%d, seting:a1:%d,a2:%d", __func__,anc_type,anc_gain_ff_iir_coefs0_l[0].a1,anc_gain_ff_iir_coefs0_l[0].a2);

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
#if defined(AUDIO_ANC_TT_HW)|| defined(AUDIO_ANC_FB_MC_HW)
    LOG_I("%s: tm_iir_coef_using:%d,Iir_coef_swap:%d", __func__,tm_iir_coef_using,tt_mc_control->codec_tm_iir_coef_swap);
#endif

    ff_filtes_l = &(cfg->anc_cfg_ff_l);
    ff_filtes_r = &(cfg->anc_cfg_ff_r);
    fb_filtes_l = &(cfg->anc_cfg_fb_l);
    fb_filtes_r = &(cfg->anc_cfg_fb_r);

#if defined(AUDIO_ANC_TT_HW)
    tt_filtes_l = &(cfg->anc_cfg_tt_l);
    tt_filtes_r = &(cfg->anc_cfg_tt_r);
#endif

#if defined(AUDIO_ANC_FB_MC_HW)
    mc_filtes_l = &(cfg->anc_cfg_mc_l);
    mc_filtes_r = &(cfg->anc_cfg_mc_r);
#endif

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
        if( ff_filtes_l_old.iir_counter==ff_filtes_l->iir_counter
            &&ff_filtes_l_old.iir_bypass_flag==ff_filtes_l->iir_bypass_flag
            &&ff_filtes_l->iir_bypass_flag==0
            &&ff_filtes_r_old.iir_counter==ff_filtes_r->iir_counter
            &&ff_filtes_r_old.iir_bypass_flag==ff_filtes_r->iir_bypass_flag
            &&ff_filtes_r->iir_bypass_flag==0
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
                anc_iir_control->codec_iir_coef_swap=0;
                while(1)
                {
                    if(anc_iir_control->codec_iir_coef_swap_status==0) break;
                }

            }
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
            &&fb_filtes_r_old.iir_counter==fb_filtes_r->iir_counter
            &&fb_filtes_r_old.iir_bypass_flag==fb_filtes_r->iir_bypass_flag
            &&fb_filtes_r->iir_bypass_flag==0
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
                anc_iir_control->codec_iir_coef_swap=0;
                while(1)
                {
                    if(anc_iir_control->codec_iir_coef_swap_status==0) break;
                }
            }
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
            &&tt_filtes_r_old.iir_counter==tt_filtes_r->iir_counter
            &&tt_filtes_r_old.iir_bypass_flag==tt_filtes_r->iir_bypass_flag
            &&tt_filtes_r->iir_bypass_flag==0
            &&anc_gain_delay==ANC_GAIN_NO_DELAY)
        {
            LOG_I("%s: switching TT", __func__);

            if(tm_iir_coef_using==0)
            {
                if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
                {
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_tt_iir_coefs1_l[i].a1=-tt_filtes_l->iir_coef[i].coef_a[1];
                        anc_tt_iir_coefs1_l[i].a2=-tt_filtes_l->iir_coef[i].coef_a[2];
                        anc_tt_iir_coefs1_l[i].b0=tt_filtes_l->iir_coef[i].coef_b[0];
                        anc_tt_iir_coefs1_l[i].b1=tt_filtes_l->iir_coef[i].coef_b[1];
                        anc_tt_iir_coefs1_l[i].b2=tt_filtes_l->iir_coef[i].coef_b[2];

#if defined(AUDIO_ANC_FB_MC_HW)
                        anc_mc_iir_coefs1_l[i].a1=-mc_filtes_l_old.iir_coef[i].coef_a[1];
                        anc_mc_iir_coefs1_l[i].a2=-mc_filtes_l_old.iir_coef[i].coef_a[2];
                        anc_mc_iir_coefs1_l[i].b0=mc_filtes_l_old.iir_coef[i].coef_b[0];
                        anc_mc_iir_coefs1_l[i].b1=mc_filtes_l_old.iir_coef[i].coef_b[1];
                        anc_mc_iir_coefs1_l[i].b2=mc_filtes_l_old.iir_coef[i].coef_b[2];
#endif

                        tt_filtes_l_old.iir_coef[i]=tt_filtes_l->iir_coef[i];
                    }
                    tt_filtes_l_old.total_gain=tt_filtes_l->total_gain;

                    anc_tt_iir_coefs1_l[0].b0=(int32)((tt_filtes_l->iir_coef[0].coef_b[0]*(int64_t)tt_filtes_l->total_gain)>>GAIN_Q);
                    anc_tt_iir_coefs1_l[0].b1=(int32)((tt_filtes_l->iir_coef[0].coef_b[1]*(int64_t)tt_filtes_l->total_gain)>>GAIN_Q);
                    anc_tt_iir_coefs1_l[0].b2=(int32)((tt_filtes_l->iir_coef[0].coef_b[2]*(int64_t)tt_filtes_l->total_gain)>>GAIN_Q);

#if defined(AUDIO_ANC_FB_MC_HW)
                    anc_mc_iir_coefs1_l[0].b0=(int32)((mc_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)mc_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_mc_iir_coefs1_l[0].b1=(int32)((mc_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)mc_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_mc_iir_coefs1_l[0].b2=(int32)((mc_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)mc_filtes_l_old.total_gain)>>GAIN_Q);
#endif
                }

                if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
                {
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_tt_iir_coefs1_r[i].a1=-tt_filtes_r->iir_coef[i].coef_a[1];
                        anc_tt_iir_coefs1_r[i].a2=-tt_filtes_r->iir_coef[i].coef_a[2];
                        anc_tt_iir_coefs1_r[i].b0=tt_filtes_r->iir_coef[i].coef_b[0];
                        anc_tt_iir_coefs1_r[i].b1=tt_filtes_r->iir_coef[i].coef_b[1];
                        anc_tt_iir_coefs1_r[i].b2=tt_filtes_r->iir_coef[i].coef_b[2];

#if defined(AUDIO_ANC_FB_MC_HW)
                        anc_mc_iir_coefs1_r[i].a1=-mc_filtes_r_old.iir_coef[i].coef_a[1];
                        anc_mc_iir_coefs1_r[i].a2=-mc_filtes_r_old.iir_coef[i].coef_a[2];
                        anc_mc_iir_coefs1_r[i].b0=mc_filtes_r_old.iir_coef[i].coef_b[0];
                        anc_mc_iir_coefs1_r[i].b1=mc_filtes_r_old.iir_coef[i].coef_b[1];
                        anc_mc_iir_coefs1_r[i].b2=mc_filtes_r_old.iir_coef[i].coef_b[2];
#endif
                        tt_filtes_r_old.iir_coef[i]=tt_filtes_r->iir_coef[i];
                    }

                    tt_filtes_r_old.total_gain=tt_filtes_r->total_gain;

                    anc_tt_iir_coefs1_r[0].b0=(int32)((tt_filtes_r->iir_coef[0].coef_b[0]*(int64_t)tt_filtes_r->total_gain)>>GAIN_Q);
                    anc_tt_iir_coefs1_r[0].b1=(int32)((tt_filtes_r->iir_coef[0].coef_b[1]*(int64_t)tt_filtes_r->total_gain)>>GAIN_Q);
                    anc_tt_iir_coefs1_r[0].b2=(int32)((tt_filtes_r->iir_coef[0].coef_b[2]*(int64_t)tt_filtes_r->total_gain)>>GAIN_Q);

#if defined(AUDIO_ANC_FB_MC_HW)
                    anc_mc_iir_coefs1_r[0].b0=(int32)((mc_filtes_r_old.iir_coef[0].coef_b[0]*(int64_t)mc_filtes_r_old.total_gain)>>GAIN_Q);
                    anc_mc_iir_coefs1_r[0].b1=(int32)((mc_filtes_r_old.iir_coef[0].coef_b[1]*(int64_t)mc_filtes_r_old.total_gain)>>GAIN_Q);
                    anc_mc_iir_coefs1_r[0].b2=(int32)((mc_filtes_r_old.iir_coef[0].coef_b[2]*(int64_t)mc_filtes_r_old.total_gain)>>GAIN_Q);
#endif
                }
                tt_mc_control->codec_tm_iir_coef_swap=1;
                hal_sys_timer_delay_us(10);
                /*while(1)
                {
                    if(tt_mc_control->tm_iir_coef_swap_status_sync==1) break;
                }*/
            }
            else
            {
                if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
                {
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_tt_iir_coefs0_l[i].a1=-tt_filtes_l->iir_coef[i].coef_a[1];
                        anc_tt_iir_coefs0_l[i].a2=-tt_filtes_l->iir_coef[i].coef_a[2];
                        anc_tt_iir_coefs0_l[i].b0=tt_filtes_l->iir_coef[i].coef_b[0];
                        anc_tt_iir_coefs0_l[i].b1=tt_filtes_l->iir_coef[i].coef_b[1];
                        anc_tt_iir_coefs0_l[i].b2=tt_filtes_l->iir_coef[i].coef_b[2];

#if defined(AUDIO_ANC_FB_MC_HW)
                        anc_mc_iir_coefs0_l[i].a1=-mc_filtes_l_old.iir_coef[i].coef_a[1];
                        anc_mc_iir_coefs0_l[i].a2=-mc_filtes_l_old.iir_coef[i].coef_a[2];
                        anc_mc_iir_coefs0_l[i].b0=mc_filtes_l_old.iir_coef[i].coef_b[0];
                        anc_mc_iir_coefs0_l[i].b1=mc_filtes_l_old.iir_coef[i].coef_b[1];
                        anc_mc_iir_coefs0_l[i].b2=mc_filtes_l_old.iir_coef[i].coef_b[2];
#endif
                        tt_filtes_l_old.iir_coef[i]=tt_filtes_l->iir_coef[i];
                    }
                    tt_filtes_l_old.total_gain=tt_filtes_l->total_gain;

                    anc_tt_iir_coefs0_l[0].b0=(int32)((tt_filtes_l->iir_coef[0].coef_b[0]*(int64_t)tt_filtes_l->total_gain)>>GAIN_Q);
                    anc_tt_iir_coefs0_l[0].b1=(int32)((tt_filtes_l->iir_coef[0].coef_b[1]*(int64_t)tt_filtes_l->total_gain)>>GAIN_Q);
                    anc_tt_iir_coefs0_l[0].b2=(int32)((tt_filtes_l->iir_coef[0].coef_b[2]*(int64_t)tt_filtes_l->total_gain)>>GAIN_Q);

#if defined(AUDIO_ANC_FB_MC_HW)
                    anc_mc_iir_coefs0_l[0].b0=(int32)((mc_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)mc_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_mc_iir_coefs0_l[0].b1=(int32)((mc_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)mc_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_mc_iir_coefs0_l[0].b2=(int32)((mc_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)mc_filtes_l_old.total_gain)>>GAIN_Q);
#endif
                }

                if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
                {
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_tt_iir_coefs0_r[i].a1=-tt_filtes_r->iir_coef[i].coef_a[1];
                        anc_tt_iir_coefs0_r[i].a2=-tt_filtes_r->iir_coef[i].coef_a[2];
                        anc_tt_iir_coefs0_r[i].b0=tt_filtes_r->iir_coef[i].coef_b[0];
                        anc_tt_iir_coefs0_r[i].b1=tt_filtes_r->iir_coef[i].coef_b[1];
                        anc_tt_iir_coefs0_r[i].b2=tt_filtes_r->iir_coef[i].coef_b[2];

#if defined(AUDIO_ANC_FB_MC_HW)
                        anc_mc_iir_coefs0_r[i].a1=-mc_filtes_r_old.iir_coef[i].coef_a[1];
                        anc_mc_iir_coefs0_r[i].a2=-mc_filtes_r_old.iir_coef[i].coef_a[2];
                        anc_mc_iir_coefs0_r[i].b0=mc_filtes_r_old.iir_coef[i].coef_b[0];
                        anc_mc_iir_coefs0_r[i].b1=mc_filtes_r_old.iir_coef[i].coef_b[1];
                        anc_mc_iir_coefs0_r[i].b2=mc_filtes_r_old.iir_coef[i].coef_b[2];
#endif
                        tt_filtes_r_old.iir_coef[i]=tt_filtes_r->iir_coef[i];
                    }

                    tt_filtes_r_old.total_gain=tt_filtes_r->total_gain;

                    anc_tt_iir_coefs0_r[0].b0=(int32)((tt_filtes_r->iir_coef[0].coef_b[0]*(int64_t)tt_filtes_r->total_gain)>>GAIN_Q);
                    anc_tt_iir_coefs0_r[0].b1=(int32)((tt_filtes_r->iir_coef[0].coef_b[1]*(int64_t)tt_filtes_r->total_gain)>>GAIN_Q);
                    anc_tt_iir_coefs0_r[0].b2=(int32)((tt_filtes_r->iir_coef[0].coef_b[2]*(int64_t)tt_filtes_r->total_gain)>>GAIN_Q);

#if defined(AUDIO_ANC_FB_MC_HW)
                    anc_mc_iir_coefs0_r[0].b0=(int32)((mc_filtes_r_old.iir_coef[0].coef_b[0]*(int64_t)mc_filtes_r_old.total_gain)>>GAIN_Q);
                    anc_mc_iir_coefs0_r[0].b1=(int32)((mc_filtes_r_old.iir_coef[0].coef_b[1]*(int64_t)mc_filtes_r_old.total_gain)>>GAIN_Q);
                    anc_mc_iir_coefs0_r[0].b2=(int32)((mc_filtes_r_old.iir_coef[0].coef_b[2]*(int64_t)mc_filtes_r_old.total_gain)>>GAIN_Q);
#endif
                }
                tt_mc_control->codec_tm_iir_coef_swap=0;
                hal_sys_timer_delay_us(10);
                /*while(1)
                {
                    if(tt_mc_control->tm_iir_coef_swap_status_sync==0) break;
                }*/
            }

            tt_iir_reset_flag=0;
            tm_iir_coef_using=1-tm_iir_coef_using;
        }
        else
        {
            LOG_I("%s: no switching TT", __func__);

            //disable all IIR filter before updata the coefs;
            tt_mc_control->codec_tm_iir_enable=0;
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
            &&mc_filtes_r_old.iir_counter==mc_filtes_r->iir_counter
            &&mc_filtes_r_old.iir_bypass_flag==mc_filtes_r->iir_bypass_flag
            &&mc_filtes_r->iir_bypass_flag==0
            &&anc_gain_delay==ANC_GAIN_NO_DELAY)
        {
            LOG_I("%s: switching MC", __func__);

            if(tm_iir_coef_using==0)
            {
                if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
                {
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_mc_iir_coefs1_l[i].a1=-mc_filtes_l->iir_coef[i].coef_a[1];
                        anc_mc_iir_coefs1_l[i].a2=-mc_filtes_l->iir_coef[i].coef_a[2];
                        anc_mc_iir_coefs1_l[i].b0=mc_filtes_l->iir_coef[i].coef_b[0];
                        anc_mc_iir_coefs1_l[i].b1=mc_filtes_l->iir_coef[i].coef_b[1];
                        anc_mc_iir_coefs1_l[i].b2=mc_filtes_l->iir_coef[i].coef_b[2];

#if defined(AUDIO_ANC_TT_HW)
                        anc_tt_iir_coefs1_l[i].a1=-tt_filtes_l_old.iir_coef[i].coef_a[1];
                        anc_tt_iir_coefs1_l[i].a2=-tt_filtes_l_old.iir_coef[i].coef_a[2];
                        anc_tt_iir_coefs1_l[i].b0=tt_filtes_l_old.iir_coef[i].coef_b[0];
                        anc_tt_iir_coefs1_l[i].b1=tt_filtes_l_old.iir_coef[i].coef_b[1];
                        anc_tt_iir_coefs1_l[i].b2=tt_filtes_l_old.iir_coef[i].coef_b[2];
#endif
                        mc_filtes_l_old.iir_coef[i]=fb_filtes_l->iir_coef[i];
                    }

                    mc_filtes_l_old.total_gain=fb_filtes_l->total_gain;

                    anc_mc_iir_coefs1_l[0].b0=(int32)((mc_filtes_l->iir_coef[0].coef_b[0]*(int64_t)mc_filtes_l->total_gain)>>GAIN_Q);
                    anc_mc_iir_coefs1_l[0].b1=(int32)((mc_filtes_l->iir_coef[0].coef_b[1]*(int64_t)mc_filtes_l->total_gain)>>GAIN_Q);
                    anc_mc_iir_coefs1_l[0].b2=(int32)((mc_filtes_l->iir_coef[0].coef_b[2]*(int64_t)mc_filtes_l->total_gain)>>GAIN_Q);

#if defined(AUDIO_ANC_TT_HW)
                    anc_tt_iir_coefs1_l[0].b0=(int32)((tt_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)tt_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_tt_iir_coefs1_l[0].b1=(int32)((tt_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)tt_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_tt_iir_coefs1_l[0].b2=(int32)((tt_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)tt_filtes_l_old.total_gain)>>GAIN_Q);
#endif
                }

                if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
                {
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_mc_iir_coefs1_r[i].a1=-mc_filtes_r->iir_coef[i].coef_a[1];
                        anc_mc_iir_coefs1_r[i].a2=-mc_filtes_r->iir_coef[i].coef_a[2];
                        anc_mc_iir_coefs1_r[i].b0=mc_filtes_r->iir_coef[i].coef_b[0];
                        anc_mc_iir_coefs1_r[i].b1=mc_filtes_r->iir_coef[i].coef_b[1];
                        anc_mc_iir_coefs1_r[i].b2=mc_filtes_r->iir_coef[i].coef_b[2];

#if defined(AUDIO_ANC_TT_HW)
                        anc_tt_iir_coefs1_r[i].a1=-tt_filtes_r_old.iir_coef[i].coef_a[1];
                        anc_tt_iir_coefs1_r[i].a2=-tt_filtes_r_old.iir_coef[i].coef_a[2];
                        anc_tt_iir_coefs1_r[i].b0=tt_filtes_r_old.iir_coef[i].coef_b[0];
                        anc_tt_iir_coefs1_r[i].b1=tt_filtes_r_old.iir_coef[i].coef_b[1];
                        anc_tt_iir_coefs1_r[i].b2=tt_filtes_r_old.iir_coef[i].coef_b[2];
#endif
                        mc_filtes_r_old.iir_coef[i]=mc_filtes_r->iir_coef[i];
                    }
                    mc_filtes_r_old.total_gain=mc_filtes_r->total_gain;

                    anc_mc_iir_coefs1_r[0].b0=(int32)((mc_filtes_r->iir_coef[0].coef_b[0]*(int64_t)mc_filtes_r->total_gain)>>GAIN_Q);
                    anc_mc_iir_coefs1_r[0].b1=(int32)((mc_filtes_r->iir_coef[0].coef_b[1]*(int64_t)mc_filtes_r->total_gain)>>GAIN_Q);
                    anc_mc_iir_coefs1_r[0].b2=(int32)((mc_filtes_r->iir_coef[0].coef_b[2]*(int64_t)mc_filtes_r->total_gain)>>GAIN_Q);

#if defined(AUDIO_ANC_TT_HW)
                    anc_tt_iir_coefs1_r[0].b0=(int32)((tt_filtes_r_old.iir_coef[0].coef_b[0]*(int64_t)tt_filtes_r_old.total_gain)>>GAIN_Q);
                    anc_tt_iir_coefs1_r[0].b1=(int32)((tt_filtes_r_old.iir_coef[0].coef_b[1]*(int64_t)tt_filtes_r_old.total_gain)>>GAIN_Q);
                    anc_tt_iir_coefs1_r[0].b2=(int32)((tt_filtes_r_old.iir_coef[0].coef_b[2]*(int64_t)tt_filtes_r_old.total_gain)>>GAIN_Q);
#endif
                }
                tt_mc_control->codec_tm_iir_coef_swap=1;
                hal_sys_timer_delay_us(10);
                /*
                while(1)
                {
                    if(tt_mc_control->tm_iir_coef_swap_status_sync==0) break;
                }
                */
            }
            else
            {

                if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
                {
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_mc_iir_coefs0_l[i].a1=-mc_filtes_l->iir_coef[i].coef_a[1];
                        anc_mc_iir_coefs0_l[i].a2=-mc_filtes_l->iir_coef[i].coef_a[2];
                        anc_mc_iir_coefs0_l[i].b0=mc_filtes_l->iir_coef[i].coef_b[0];
                        anc_mc_iir_coefs0_l[i].b1=mc_filtes_l->iir_coef[i].coef_b[1];
                        anc_mc_iir_coefs0_l[i].b2=mc_filtes_l->iir_coef[i].coef_b[2];

#if defined(AUDIO_ANC_TT_HW)
                        anc_tt_iir_coefs0_l[i].a1=-tt_filtes_l_old.iir_coef[i].coef_a[1];
                        anc_tt_iir_coefs0_l[i].a2=-tt_filtes_l_old.iir_coef[i].coef_a[2];
                        anc_tt_iir_coefs0_l[i].b0=tt_filtes_l_old.iir_coef[i].coef_b[0];
                        anc_tt_iir_coefs0_l[i].b1=tt_filtes_l_old.iir_coef[i].coef_b[1];
                        anc_tt_iir_coefs0_l[i].b2=tt_filtes_l_old.iir_coef[i].coef_b[2];
#endif

                        mc_filtes_l_old.iir_coef[i]=fb_filtes_l->iir_coef[i];
                    }

                    mc_filtes_l_old.total_gain=fb_filtes_l->total_gain;

                    anc_mc_iir_coefs0_l[0].b0=(int32)((mc_filtes_l->iir_coef[0].coef_b[0]*(int64_t)mc_filtes_l->total_gain)>>GAIN_Q);
                    anc_mc_iir_coefs0_l[0].b1=(int32)((mc_filtes_l->iir_coef[0].coef_b[1]*(int64_t)mc_filtes_l->total_gain)>>GAIN_Q);
                    anc_mc_iir_coefs0_l[0].b2=(int32)((mc_filtes_l->iir_coef[0].coef_b[2]*(int64_t)mc_filtes_l->total_gain)>>GAIN_Q);

#if defined(AUDIO_ANC_TT_HW)
                    anc_tt_iir_coefs0_l[0].b0=(int32)((tt_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)tt_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_tt_iir_coefs0_l[0].b1=(int32)((tt_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)tt_filtes_l_old.total_gain)>>GAIN_Q);
                    anc_tt_iir_coefs0_l[0].b2=(int32)((tt_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)tt_filtes_l_old.total_gain)>>GAIN_Q);
#endif
                }

                if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
                {
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_mc_iir_coefs0_r[i].a1=-mc_filtes_r->iir_coef[i].coef_a[1];
                        anc_mc_iir_coefs0_r[i].a2=-mc_filtes_r->iir_coef[i].coef_a[2];
                        anc_mc_iir_coefs0_r[i].b0=mc_filtes_r->iir_coef[i].coef_b[0];
                        anc_mc_iir_coefs0_r[i].b1=mc_filtes_r->iir_coef[i].coef_b[1];
                        anc_mc_iir_coefs0_r[i].b2=mc_filtes_r->iir_coef[i].coef_b[2];

#if defined(AUDIO_ANC_TT_HW)
                        anc_tt_iir_coefs0_r[i].a1=-tt_filtes_r_old.iir_coef[i].coef_a[1];
                        anc_tt_iir_coefs0_r[i].a2=-tt_filtes_r_old.iir_coef[i].coef_a[2];
                        anc_tt_iir_coefs0_r[i].b0=tt_filtes_r_old.iir_coef[i].coef_b[0];
                        anc_tt_iir_coefs0_r[i].b1=tt_filtes_r_old.iir_coef[i].coef_b[1];
                        anc_tt_iir_coefs0_r[i].b2=tt_filtes_r_old.iir_coef[i].coef_b[2];
#endif
                        mc_filtes_r_old.iir_coef[i]=mc_filtes_r->iir_coef[i];
                    }
                    mc_filtes_r_old.total_gain=mc_filtes_r->total_gain;

                    anc_mc_iir_coefs0_r[0].b0=(int32)((mc_filtes_r->iir_coef[0].coef_b[0]*(int64_t)mc_filtes_r->total_gain)>>GAIN_Q);
                    anc_mc_iir_coefs0_r[0].b1=(int32)((mc_filtes_r->iir_coef[0].coef_b[1]*(int64_t)mc_filtes_r->total_gain)>>GAIN_Q);
                    anc_mc_iir_coefs0_r[0].b2=(int32)((mc_filtes_r->iir_coef[0].coef_b[2]*(int64_t)mc_filtes_r->total_gain)>>GAIN_Q);

#if defined(AUDIO_ANC_TT_HW)
                    anc_tt_iir_coefs0_r[0].b0=(int32)((tt_filtes_r_old.iir_coef[0].coef_b[0]*(int64_t)tt_filtes_r_old.total_gain)>>GAIN_Q);
                    anc_tt_iir_coefs0_r[0].b1=(int32)((tt_filtes_r_old.iir_coef[0].coef_b[1]*(int64_t)tt_filtes_r_old.total_gain)>>GAIN_Q);
                    anc_tt_iir_coefs0_r[0].b2=(int32)((tt_filtes_r_old.iir_coef[0].coef_b[2]*(int64_t)tt_filtes_r_old.total_gain)>>GAIN_Q);
#endif
                }
                tt_mc_control->codec_tm_iir_coef_swap=0;
                hal_sys_timer_delay_us(10);
                /*
                while(1)
                {
                    if(tt_mc_control->tm_iir_coef_swap_status_sync==0) break;
                }*/
            }

            mc_iir_reset_flag=0;
            tm_iir_coef_using=1-tm_iir_coef_using;
        }
        else
        {
            LOG_I("%s: no switching MC", __func__);

            //disable all IIR filter before updata the coefs;
            tt_mc_control->codec_tm_iir_enable=0;

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
        lock = int_lock();
#ifdef ANC_GAIN_RAMP
        anc_iir_gain_config->codec_iir_gain_ext_update_ch0=0;
         *codec_iir_gain_ext_ff_l=0;
        hal_sys_timer_delay_us(1);
        anc_iir_gain_config->codec_iir_gain_ext_update_ch0=1;

#else
        anc_ff_gain->codec_anc_mute_gain_update_ff_ch0=0;
        anc_ff_gain->codec_anc_mute_gain_ff_ch0=0;
        hal_sys_timer_delay_us(1);
        anc_ff_gain->codec_anc_mute_gain_update_ff_ch0=1;
#endif
        int_unlock(lock);


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
                    lock = int_lock();
#ifdef ANC_GAIN_RAMP
                    anc_iir_gain_config->codec_iir_gain_ext_update_ch0=0;
                    *codec_iir_gain_ext_ff_l=(int32)(((float)max_ff_gain_l/512.0f)*FIXED_GAIN_RAMP_Q);
                    hal_sys_timer_delay_us(1);
                    anc_iir_gain_config->codec_iir_gain_ext_update_ch0=1;
#else
                    anc_ff_gain->codec_anc_mute_gain_update_ff_ch0=0;
                    anc_ff_gain->codec_anc_mute_gain_ff_ch0=max_ff_gain_l;
                    hal_sys_timer_delay_us(1);
                    anc_ff_gain->codec_anc_mute_gain_update_ff_ch0=1;
#endif
                    int_unlock(lock);

                }

            }
            else
            {
                anc_iir_control->codec_iir_ch0_bypass=1;
                lock = int_lock();

#ifdef ANC_GAIN_RAMP
                anc_iir_gain_config->codec_iir_gain_ext_update_ch0=0;
                *codec_iir_gain_ext_ff_l=(int32)(((float)ff_filtes_l->total_gain/512.0f)*FIXED_GAIN_RAMP_Q);
                hal_sys_timer_delay_us(1);
                anc_iir_gain_config->codec_iir_gain_ext_update_ch0=1;
#else
                anc_ff_gain->codec_anc_mute_gain_update_ff_ch0=0;
                anc_ff_gain->codec_anc_mute_gain_ff_ch0=ff_filtes_l->total_gain;
                hal_sys_timer_delay_us(1);
                anc_ff_gain->codec_anc_mute_gain_update_ff_ch0=1;
#endif
                int_unlock(lock);

                max_ff_gain_l=ff_filtes_l->total_gain;

            }
        }
        else
        {
                anc_iir_control->codec_iir_ch0_bypass=1;
                lock = int_lock();
#ifdef ANC_GAIN_RAMP
                anc_iir_gain_config->codec_iir_gain_ext_update_ch0=0;
                 *codec_iir_gain_ext_ff_l=0;
                hal_sys_timer_delay_us(1);
                anc_iir_gain_config->codec_iir_gain_ext_update_ch0=1;
#else
                anc_ff_gain->codec_anc_mute_gain_update_ff_ch0=0;
                anc_ff_gain->codec_anc_mute_gain_ff_ch0=0;
                hal_sys_timer_delay_us(1);
                anc_ff_gain->codec_anc_mute_gain_update_ff_ch0=1;
#endif
                int_unlock(lock);

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
            lock = int_lock();
#ifdef ANC_GAIN_RAMP
            anc_iir_gain_config->codec_iir_gain_ext_update_ch1=0;
            *codec_iir_gain_ext_ff_r=0;
            hal_sys_timer_delay_us(1);
            anc_iir_gain_config->codec_iir_gain_ext_update_ch1=1;
#else
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch1=0;
            anc_ff_gain->codec_anc_mute_gain_ff_ch1=0;
            hal_sys_timer_delay_us(1);
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch1=1;
#endif
            int_unlock(lock);


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

                    lock = int_lock();
#ifdef ANC_GAIN_RAMP
                    anc_iir_gain_config->codec_iir_gain_ext_update_ch1=0;
                    *codec_iir_gain_ext_ff_r=(int32)(((float)max_ff_gain_r/512.0f)*FIXED_GAIN_RAMP_Q);
                    hal_sys_timer_delay_us(1);
                    anc_iir_gain_config->codec_iir_gain_ext_update_ch1=1;
#else
                    anc_ff_gain->codec_anc_mute_gain_update_ff_ch1=0;
                    anc_ff_gain->codec_anc_mute_gain_ff_ch1=max_ff_gain_r;
                    hal_sys_timer_delay_us(1);
                    anc_ff_gain->codec_anc_mute_gain_update_ff_ch1=1;
#endif
                    int_unlock(lock);

                }
            }
            else
            {
                anc_iir_control->codec_iir_ch1_bypass=1;
                lock = int_lock();
#ifdef ANC_GAIN_RAMP
                anc_iir_gain_config->codec_iir_gain_ext_update_ch1=0;
                *codec_iir_gain_ext_ff_r=(int32)(((float)ff_filtes_r->total_gain/512.0f)*FIXED_GAIN_RAMP_Q);
                hal_sys_timer_delay_us(1);
                anc_iir_gain_config->codec_iir_gain_ext_update_ch1=1;
#else
                anc_ff_gain->codec_anc_mute_gain_update_ff_ch1=0;
                anc_ff_gain->codec_anc_mute_gain_ff_ch1=ff_filtes_r->total_gain;
                hal_sys_timer_delay_us(1);
                anc_ff_gain->codec_anc_mute_gain_update_ff_ch1=1;
#endif
                int_unlock(lock);

                max_ff_gain_r=ff_filtes_r->total_gain;
            }
        }
        else
        {
            anc_iir_control->codec_iir_ch1_bypass=1;
            lock = int_lock();
#ifdef ANC_GAIN_RAMP
            anc_iir_gain_config->codec_iir_gain_ext_update_ch1=0;
            *codec_iir_gain_ext_ff_r=0;
            hal_sys_timer_delay_us(1);
            anc_iir_gain_config->codec_iir_gain_ext_update_ch1=1;
#else
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch1=0;
            anc_ff_gain->codec_anc_mute_gain_ff_ch1=0;
            hal_sys_timer_delay_us(1);
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch1=1;
#endif
            int_unlock(lock);

            anc_iir_control->codec_iir_count_ch1=0;
        }

        total_iir_counter=total_iir_counter+anc_iir_control->codec_iir_count_ch0+anc_iir_control->codec_iir_count_ch1;
        total_iir_counter=total_iir_counter+anc_iir_control->codec_iir_count_ch2+anc_iir_control->codec_iir_count_ch3;

        LOG_I("%s:total_iir_counter:%d", __func__,total_iir_counter);

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
            lock = int_lock();
#ifdef ANC_GAIN_RAMP
            anc_iir_gain_config->codec_iir_gain_ext_update_ch2=0;
            *codec_iir_gain_ext_fb_l=0;
            hal_sys_timer_delay_us(1);
            anc_iir_gain_config->codec_iir_gain_ext_update_ch2=1;
#else
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch0=0;
            anc_fb_gain->codec_anc_mute_gain_fb_ch0=0;
            hal_sys_timer_delay_us(1);
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch0=1;
#endif
            int_unlock(lock);


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
                lock = int_lock();

#ifdef ANC_GAIN_RAMP
                    anc_iir_gain_config->codec_iir_gain_ext_update_ch2=0;
                    *codec_iir_gain_ext_fb_l=(int32)(((float)max_fb_gain_l/512.0f)*FIXED_GAIN_RAMP_Q);
                    hal_sys_timer_delay_us(1);
                    anc_iir_gain_config->codec_iir_gain_ext_update_ch2=1;
#else
                    anc_fb_gain->codec_anc_mute_gain_update_fb_ch0=0;
                    anc_fb_gain->codec_anc_mute_gain_fb_ch0=max_fb_gain_l;
                    hal_sys_timer_delay_us(1);
                    anc_fb_gain->codec_anc_mute_gain_update_fb_ch0=1;
#endif
                int_unlock(lock);

                }
            }
            else
            {
                anc_iir_control->codec_iir_ch2_bypass=1;

                lock = int_lock();
#ifdef ANC_GAIN_RAMP
                anc_iir_gain_config->codec_iir_gain_ext_update_ch2=0;
                *codec_iir_gain_ext_fb_l=(int32)(((float)fb_filtes_l->total_gain/512.0f)*FIXED_GAIN_RAMP_Q);
                hal_sys_timer_delay_us(1);
                anc_iir_gain_config->codec_iir_gain_ext_update_ch2=1;
#else
                anc_fb_gain->codec_anc_mute_gain_update_fb_ch0=0;
                anc_fb_gain->codec_anc_mute_gain_fb_ch0=fb_filtes_l->total_gain;
                hal_sys_timer_delay_us(1);
                anc_fb_gain->codec_anc_mute_gain_update_fb_ch0=1;
#endif
                int_unlock(lock);

                max_fb_gain_l=fb_filtes_l->total_gain;

            }
        }
        else
        {
                anc_iir_control->codec_iir_ch2_bypass=1;

                lock = int_lock();
#ifdef ANC_GAIN_RAMP
                anc_iir_gain_config->codec_iir_gain_ext_update_ch2=0;
                *codec_iir_gain_ext_fb_l=0;
                hal_sys_timer_delay_us(1);
                anc_iir_gain_config->codec_iir_gain_ext_update_ch2=1;
#else
                anc_fb_gain->codec_anc_mute_gain_update_fb_ch0=0;
                anc_fb_gain->codec_anc_mute_gain_fb_ch0=0;
                hal_sys_timer_delay_us(1);
                anc_fb_gain->codec_anc_mute_gain_update_fb_ch0=1;
#endif
                int_unlock(lock);

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
            lock = int_lock();

#ifdef ANC_GAIN_RAMP
            anc_iir_gain_config->codec_iir_gain_ext_update_ch3=0;
            *codec_iir_gain_ext_fb_r=0;
            hal_sys_timer_delay_us(1);
            anc_iir_gain_config->codec_iir_gain_ext_update_ch3=1;
#else
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch1=0;
            anc_fb_gain->codec_anc_mute_gain_fb_ch1=0;
            hal_sys_timer_delay_us(1);
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch1=1;
#endif
            int_unlock(lock);


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
                    lock = int_lock();
#ifdef ANC_GAIN_RAMP
                    anc_iir_gain_config->codec_iir_gain_ext_update_ch3=0;
                    *codec_iir_gain_ext_fb_r=(int32)(((float)max_fb_gain_r/512.0f)*FIXED_GAIN_RAMP_Q);
                    hal_sys_timer_delay_us(1);
                    anc_iir_gain_config->codec_iir_gain_ext_update_ch3=1;
#else
                    anc_fb_gain->codec_anc_mute_gain_update_fb_ch1=0;
                    anc_fb_gain->codec_anc_mute_gain_fb_ch1=max_fb_gain_r;
                    hal_sys_timer_delay_us(1);
                    anc_fb_gain->codec_anc_mute_gain_update_fb_ch1=1;
#endif
                    int_unlock(lock);

                }
            }
            else
            {
                anc_iir_control->codec_iir_ch3_bypass=1;
                lock = int_lock();
#ifdef ANC_GAIN_RAMP
                anc_iir_gain_config->codec_iir_gain_ext_update_ch3=0;
                *codec_iir_gain_ext_fb_r=(int32)(((float)fb_filtes_r->total_gain/512.0f)*FIXED_GAIN_RAMP_Q);
                hal_sys_timer_delay_us(1);
                anc_iir_gain_config->codec_iir_gain_ext_update_ch3=1;
#else
                anc_fb_gain->codec_anc_mute_gain_update_fb_ch1=0;
                anc_fb_gain->codec_anc_mute_gain_fb_ch1=fb_filtes_r->total_gain;
				hal_sys_timer_delay_us(1);
                anc_fb_gain->codec_anc_mute_gain_update_fb_ch1=1;
#endif
                int_unlock(lock);

                max_fb_gain_r=fb_filtes_r->total_gain;
            }
        }
        else
        {
            anc_iir_control->codec_iir_ch3_bypass=1;
            lock = int_lock();
#ifdef ANC_GAIN_RAMP
            anc_iir_gain_config->codec_iir_gain_ext_update_ch3=0;
            *codec_iir_gain_ext_fb_r=0;
            hal_sys_timer_delay_us(1);
            anc_iir_gain_config->codec_iir_gain_ext_update_ch3=1;
#else
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch1=0;
            anc_fb_gain->codec_anc_mute_gain_fb_ch1=0;
            hal_sys_timer_delay_us(1);
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch1=1;
#endif
            int_unlock(lock);

            anc_iir_control->codec_iir_count_ch3=0;
        }

        total_iir_counter=total_iir_counter+anc_iir_control->codec_iir_count_ch0+anc_iir_control->codec_iir_count_ch1;
        total_iir_counter=total_iir_counter+anc_iir_control->codec_iir_count_ch2+anc_iir_control->codec_iir_count_ch3;

        LOG_I("%s:total_iir_counter:%d", __func__,total_iir_counter);

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

            //Set the TT gain;
            lock = int_lock();
#ifdef ANC_GAIN_RAMP
            tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch0=0;
            *codec_tm_iir_gain_ext_tt_l=0;
            hal_sys_timer_delay_us(1);
            tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch0=1;
#else
            anc_tt_gain->codec_mute_gain_update_tt_ch0=0;
            anc_tt_gain->codec_mute_gain_coef_tt_ch0=0;
            hal_sys_timer_delay_us(1);
            anc_tt_gain->codec_mute_gain_update_tt_ch0=1;
#endif
            int_unlock(lock);

            max_tt_gain_l=-(1<<GAIN_Q);

            if(tt_filtes_l->iir_bypass_flag==0)
            {
                tt_mc_control->codec_tm_iir_ch0_bypass=0;

                if(tt_filtes_l->iir_counter>IIR_COUNTER)
                {
                    tt_mc_control->codec_tm_iir_count_ch0=IIR_COUNTER;
                }
                else if(tt_filtes_l->iir_counter>0)
                {
                    tt_mc_control->codec_tm_iir_count_ch0=tt_filtes_l->iir_counter;
                }
                else
                {
                    tt_mc_control->codec_tm_iir_ch0_bypass=1;
                    LOG_I("Error ff_filtes_r IIR counter:%d", tt_filtes_l->iir_counter);
                    err=ANC_TYPE_ERR;
                }
                if(tm_iir_coef_using==0)
                {
                    //talk thru left ch iir coefs settings
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_tt_iir_coefs0_l[i].a1=-tt_filtes_l->iir_coef[i].coef_a[1];
                        anc_tt_iir_coefs0_l[i].a2=-tt_filtes_l->iir_coef[i].coef_a[2];
                        anc_tt_iir_coefs0_l[i].b0=tt_filtes_l->iir_coef[i].coef_b[0];
                        anc_tt_iir_coefs0_l[i].b1=tt_filtes_l->iir_coef[i].coef_b[1];
                        anc_tt_iir_coefs0_l[i].b2=tt_filtes_l->iir_coef[i].coef_b[2];
                    }
                    anc_tt_iir_coefs0_l[0].b0=(int32)((tt_filtes_l->iir_coef[0].coef_b[0]*(int64_t)tt_filtes_l->total_gain)>>GAIN_Q);
                    anc_tt_iir_coefs0_l[0].b1=(int32)((tt_filtes_l->iir_coef[0].coef_b[1]*(int64_t)tt_filtes_l->total_gain)>>GAIN_Q);
                    anc_tt_iir_coefs0_l[0].b2=(int32)((tt_filtes_l->iir_coef[0].coef_b[2]*(int64_t)tt_filtes_l->total_gain)>>GAIN_Q);
                }
                else
                {
                    //talk thru left ch iir coefs settings
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_tt_iir_coefs1_l[i].a1=-tt_filtes_l->iir_coef[i].coef_a[1];
                        anc_tt_iir_coefs1_l[i].a2=-tt_filtes_l->iir_coef[i].coef_a[2];
                        anc_tt_iir_coefs1_l[i].b0=tt_filtes_l->iir_coef[i].coef_b[0];
                        anc_tt_iir_coefs1_l[i].b1=tt_filtes_l->iir_coef[i].coef_b[1];
                        anc_tt_iir_coefs1_l[i].b2=tt_filtes_l->iir_coef[i].coef_b[2];
                    }
                    anc_tt_iir_coefs1_l[0].b0=(int32)((tt_filtes_l->iir_coef[0].coef_b[0]*(int64_t)tt_filtes_l->total_gain)>>GAIN_Q);
                    anc_tt_iir_coefs1_l[0].b1=(int32)((tt_filtes_l->iir_coef[0].coef_b[1]*(int64_t)tt_filtes_l->total_gain)>>GAIN_Q);
                    anc_tt_iir_coefs1_l[0].b2=(int32)((tt_filtes_l->iir_coef[0].coef_b[2]*(int64_t)tt_filtes_l->total_gain)>>GAIN_Q);
                }
                if(err!=ANC_NO_ERR)
                {
                    max_tt_gain_l=0;
                }
                if(anc_gain_delay==ANC_GAIN_NO_DELAY)
                {
                    //Set the TT gain;
                    lock = int_lock();
#ifdef ANC_GAIN_RAMP
                    tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch0=0;
                    *codec_tm_iir_gain_ext_tt_l=(int32)(((float)max_tt_gain_l/512.0f)*FIXED_GAIN_RAMP_Q);
                    hal_sys_timer_delay_us(1);
                    tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch0=1;
#else
                    anc_tt_gain->codec_mute_gain_update_tt_ch0=0;
                    anc_tt_gain->codec_mute_gain_coef_tt_ch0=max_tt_gain_l;
                    hal_sys_timer_delay_us(1);
                    anc_tt_gain->codec_mute_gain_update_tt_ch0=1;
#endif
                    int_unlock(lock);

                }
            }
            else
            {
                tt_mc_control->codec_tm_iir_ch0_bypass=1;

                lock = int_lock();
#ifdef ANC_GAIN_RAMP
                tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch0=0;
                *codec_tm_iir_gain_ext_tt_l=(int32)(((float)tt_filtes_l->total_gain/512.0f)*FIXED_GAIN_RAMP_Q);
                hal_sys_timer_delay_us(1);
                tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch0=1;
#else
                anc_tt_gain->codec_mute_gain_update_tt_ch0=0;
                anc_tt_gain->codec_mute_gain_coef_tt_ch0=tt_filtes_l->total_gain;
                hal_sys_timer_delay_us(1);
                anc_tt_gain->codec_mute_gain_update_tt_ch0=1;
#endif
                int_unlock(lock);

                max_tt_gain_l=tt_filtes_l->total_gain;
            }
        }
        else
        {
            tt_mc_control->codec_tm_iir_ch0_bypass=1;

            lock = int_lock();
#ifdef ANC_GAIN_RAMP
            tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch0=0;
            *codec_tm_iir_gain_ext_tt_l=0;
            hal_sys_timer_delay_us(1);
            tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch0=1;
#else
            anc_tt_gain->codec_mute_gain_update_tt_ch0=0;
            anc_tt_gain->codec_mute_gain_coef_tt_ch0=0;
            hal_sys_timer_delay_us(1);
            anc_tt_gain->codec_mute_gain_update_tt_ch0=1;
#endif
            int_unlock(lock);

            tt_mc_control->codec_tm_iir_count_ch0=0;
        }

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            for(i=0;i<IIR_COUNTER;i++)
            {
                tt_filtes_r_old.iir_coef[i]=tt_filtes_r->iir_coef[i];
             }

            tt_filtes_r_old.total_gain=tt_filtes_r->total_gain;
            tt_filtes_r_old.iir_counter=tt_filtes_r->iir_counter;
            tt_filtes_r_old.iir_bypass_flag=tt_filtes_r->iir_bypass_flag;

            //Set the TT gain;
            lock = int_lock();
#ifdef ANC_GAIN_RAMP
            tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch1=0;
            *codec_tm_iir_gain_ext_tt_r=0;
            hal_sys_timer_delay_us(1);
            tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch1=1;
#else
            anc_tt_gain->codec_mute_gain_update_tt_ch1=0;
            anc_tt_gain->codec_mute_gain_coef_tt_ch1=0;
            hal_sys_timer_delay_us(1);
            anc_tt_gain->codec_mute_gain_update_tt_ch1=1;
#endif
            int_unlock(lock);

            max_tt_gain_r=-(1<<GAIN_Q);

            if(tt_filtes_r->iir_bypass_flag==0)
            {
                tt_mc_control->codec_tm_iir_ch1_bypass=0;

                if(tt_filtes_r->iir_counter>IIR_COUNTER)
                {
                    tt_mc_control->codec_tm_iir_count_ch1=IIR_COUNTER;
                }
                else if(tt_filtes_r->iir_counter>0)
                {
                    tt_mc_control->codec_tm_iir_count_ch1=tt_filtes_r->iir_counter;
                }
                else
                {
                    tt_mc_control->codec_tm_iir_ch1_bypass=1;
                    LOG_I("Error ff_filtes_r IIR counter:%d", tt_filtes_r->iir_counter);
                    err=ANC_TYPE_ERR;
                }
                if(tm_iir_coef_using==0)
                {
                    //talk thru left ch iir coefs settings
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_tt_iir_coefs0_r[i].a1=-tt_filtes_r->iir_coef[i].coef_a[1];
                        anc_tt_iir_coefs0_r[i].a2=-tt_filtes_r->iir_coef[i].coef_a[2];
                        anc_tt_iir_coefs0_r[i].b0=tt_filtes_r->iir_coef[i].coef_b[0];
                        anc_tt_iir_coefs0_r[i].b1=tt_filtes_r->iir_coef[i].coef_b[1];
                        anc_tt_iir_coefs0_r[i].b2=tt_filtes_r->iir_coef[i].coef_b[2];
                    }
                    anc_tt_iir_coefs0_r[0].b0=(int32)((tt_filtes_r->iir_coef[0].coef_b[0]*(int64_t)tt_filtes_r->total_gain)>>GAIN_Q);
                    anc_tt_iir_coefs0_r[0].b1=(int32)((tt_filtes_r->iir_coef[0].coef_b[1]*(int64_t)tt_filtes_r->total_gain)>>GAIN_Q);
                    anc_tt_iir_coefs0_r[0].b2=(int32)((tt_filtes_r->iir_coef[0].coef_b[2]*(int64_t)tt_filtes_r->total_gain)>>GAIN_Q);
                }
                else
                {
                    //talk thru left ch iir coefs settings
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_tt_iir_coefs1_r[i].a1=-tt_filtes_r->iir_coef[i].coef_a[1];
                        anc_tt_iir_coefs1_r[i].a2=-tt_filtes_r->iir_coef[i].coef_a[2];
                        anc_tt_iir_coefs1_r[i].b0=tt_filtes_r->iir_coef[i].coef_b[0];
                        anc_tt_iir_coefs1_r[i].b1=tt_filtes_r->iir_coef[i].coef_b[1];
                        anc_tt_iir_coefs1_r[i].b2=tt_filtes_r->iir_coef[i].coef_b[2];
                    }
                    anc_tt_iir_coefs1_r[0].b0=(int32)((tt_filtes_r->iir_coef[0].coef_b[0]*(int64_t)tt_filtes_r->total_gain)>>GAIN_Q);
                    anc_tt_iir_coefs1_r[0].b1=(int32)((tt_filtes_r->iir_coef[0].coef_b[1]*(int64_t)tt_filtes_r->total_gain)>>GAIN_Q);
                    anc_tt_iir_coefs1_r[0].b2=(int32)((tt_filtes_r->iir_coef[0].coef_b[2]*(int64_t)tt_filtes_r->total_gain)>>GAIN_Q);
                }
                if(err!=ANC_NO_ERR)
                {
                    max_tt_gain_l=0;
                }
                if(anc_gain_delay==ANC_GAIN_NO_DELAY)
                {
                    //Set the TT gain;

                    lock = int_lock();
#ifdef ANC_GAIN_RAMP
                    tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch1=0;
                    *codec_tm_iir_gain_ext_tt_r=(int32)(((float)max_tt_gain_r/512.0f)*FIXED_GAIN_RAMP_Q);
                    hal_sys_timer_delay_us(1);
                    tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch1=1;
#else
                    anc_tt_gain->codec_mute_gain_update_tt_ch1=0;
                    anc_tt_gain->codec_mute_gain_coef_tt_ch1=max_tt_gain_r;
                    hal_sys_timer_delay_us(1);
                    anc_tt_gain->codec_mute_gain_update_tt_ch1=1;
#endif
                    int_unlock(lock);

                }
            }
            else
            {
                tt_mc_control->codec_tm_iir_ch1_bypass=1;

                lock = int_lock();
#ifdef ANC_GAIN_RAMP
                tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch1=0;
                *codec_tm_iir_gain_ext_tt_r=(int32)(((float)tt_filtes_r->total_gain/512.0f)*FIXED_GAIN_RAMP_Q);
                hal_sys_timer_delay_us(1);
                tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch1=1;
#else
                anc_tt_gain->codec_mute_gain_update_tt_ch1=0;
                anc_tt_gain->codec_mute_gain_coef_tt_ch1=tt_filtes_r->total_gain;
                hal_sys_timer_delay_us(1);
                anc_tt_gain->codec_mute_gain_update_tt_ch1=1;
#endif
                int_unlock(lock);

                max_tt_gain_r=tt_filtes_r->total_gain;
            }
        }
        else
        {
            tt_mc_control->codec_tm_iir_ch1_bypass=1;

            lock = int_lock();
#ifdef ANC_GAIN_RAMP
            tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch1=0;
            *codec_tm_iir_gain_ext_tt_r=0;
            hal_sys_timer_delay_us(1);
            tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch1=1;
#else
            anc_tt_gain->codec_mute_gain_update_tt_ch1=0;
            anc_tt_gain->codec_mute_gain_coef_tt_ch1=0;
            hal_sys_timer_delay_us(1);
            anc_tt_gain->codec_mute_gain_update_tt_ch1=1;
#endif
            int_unlock(lock);

            tt_mc_control->codec_tm_iir_count_ch1=0;
        }

        total_iir_counter=total_iir_counter+tt_mc_control->codec_tm_iir_count_ch0+tt_mc_control->codec_tm_iir_count_ch1;

        LOG_I("%s:total_iir_counter:%d", __func__,total_iir_counter);
        //enable all IIR filters
        tt_mc_control->codec_tm_iir_enable=1;

        //enable TT.
        tm_config->codec_tt_enable_ch0=1;
        tm_config->codec_tt_enable_ch1=1;
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
            lock = int_lock();
#ifdef ANC_GAIN_RAMP
            tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch2=0;
            *codec_tm_iir_gain_ext_mc_l=0;
            hal_sys_timer_delay_us(1);
            tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch2=1;
#else
            anc_mc_gain->codec_mute_gain_update_mc_ch0=0;
            anc_mc_gain->codec_mute_gain_coef_mc_ch0=0;
            hal_sys_timer_delay_us(1);
            anc_mc_gain->codec_mute_gain_update_mc_ch0=1;
#endif
            int_unlock(lock);


            if(mc_filtes_l->iir_bypass_flag==0)
            {
                tt_mc_control->codec_tm_iir_ch2_bypass=0;

                if(mc_filtes_l->iir_counter>IIR_COUNTER)
                {
                    tt_mc_control->codec_tm_iir_count_ch2=IIR_COUNTER;
                }
                else if(mc_filtes_l->iir_counter>0)
                {
                    tt_mc_control->codec_tm_iir_count_ch2=mc_filtes_l->iir_counter;
                }
                else
                {
                    tt_mc_control->codec_tm_iir_ch2_bypass=1;
                    LOG_I("Error fb_filtes_r IIR counter:%d", mc_filtes_l->iir_counter);
                    err=ANC_TYPE_ERR;
                }

                if(tm_iir_coef_using==0)
                {
                    //music cancel ch iir coefs settings
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_mc_iir_coefs0_l[i].a1=-mc_filtes_l->iir_coef[i].coef_a[1];
                        anc_mc_iir_coefs0_l[i].a2=-mc_filtes_l->iir_coef[i].coef_a[2];
                        anc_mc_iir_coefs0_l[i].b0=mc_filtes_l->iir_coef[i].coef_b[0];
                        anc_mc_iir_coefs0_l[i].b1=mc_filtes_l->iir_coef[i].coef_b[1];
                        anc_mc_iir_coefs0_l[i].b2=mc_filtes_l->iir_coef[i].coef_b[2];
                    }
                    anc_mc_iir_coefs0_l[0].b0=(int32)((mc_filtes_l->iir_coef[0].coef_b[0]*(int64_t)mc_filtes_l->total_gain)>>GAIN_Q);
                    anc_mc_iir_coefs0_l[0].b1=(int32)((mc_filtes_l->iir_coef[0].coef_b[1]*(int64_t)mc_filtes_l->total_gain)>>GAIN_Q);
                    anc_mc_iir_coefs0_l[0].b2=(int32)((mc_filtes_l->iir_coef[0].coef_b[2]*(int64_t)mc_filtes_l->total_gain)>>GAIN_Q);
                }
                else
                {
                    //music cancel ch iir coefs settings
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_mc_iir_coefs1_l[i].a1=-mc_filtes_l->iir_coef[i].coef_a[1];
                        anc_mc_iir_coefs1_l[i].a2=-mc_filtes_l->iir_coef[i].coef_a[2];
                        anc_mc_iir_coefs1_l[i].b0=mc_filtes_l->iir_coef[i].coef_b[0];
                        anc_mc_iir_coefs1_l[i].b1=mc_filtes_l->iir_coef[i].coef_b[1];
                        anc_mc_iir_coefs1_l[i].b2=mc_filtes_l->iir_coef[i].coef_b[2];
                    }
                    anc_mc_iir_coefs1_l[0].b0=(int32)((mc_filtes_l->iir_coef[0].coef_b[0]*(int64_t)mc_filtes_l->total_gain)>>GAIN_Q);
                    anc_mc_iir_coefs1_l[0].b1=(int32)((mc_filtes_l->iir_coef[0].coef_b[1]*(int64_t)mc_filtes_l->total_gain)>>GAIN_Q);
                    anc_mc_iir_coefs1_l[0].b2=(int32)((mc_filtes_l->iir_coef[0].coef_b[2]*(int64_t)mc_filtes_l->total_gain)>>GAIN_Q);
                 }
                if(err!=ANC_NO_ERR)
                {
                    max_mc_gain_l=0;
                }

                if(anc_gain_delay==ANC_GAIN_NO_DELAY)
                {
                    //Set the MC gain;
                lock = int_lock();

#ifdef ANC_GAIN_RAMP
                tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch2=0;
                *codec_tm_iir_gain_ext_mc_l=(int32)(((float)max_mc_gain_l/512.0f)*FIXED_GAIN_RAMP_Q);
                hal_sys_timer_delay_us(1);
                tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch2=1;
#else
                anc_mc_gain->codec_mute_gain_update_mc_ch0=0;
                anc_mc_gain->codec_mute_gain_coef_mc_ch0=max_mc_gain_l;
                hal_sys_timer_delay_us(1);
                anc_mc_gain->codec_mute_gain_update_mc_ch0=1;
#endif
                int_unlock(lock);


                }
            }
            else
            {
                tt_mc_control->codec_tm_iir_ch2_bypass=1;
                lock = int_lock();

#ifdef ANC_GAIN_RAMP
                tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch2=0;
                *codec_tm_iir_gain_ext_mc_l=(int32)(((float)mc_filtes_l->total_gain/512.0f)*FIXED_GAIN_RAMP_Q);
                hal_sys_timer_delay_us(1);
                tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch2=1;
#else
                anc_mc_gain->codec_mute_gain_update_mc_ch0=0;
                anc_mc_gain->codec_mute_gain_coef_mc_ch0=mc_filtes_l->total_gain;
                hal_sys_timer_delay_us(1);
                anc_mc_gain->codec_mute_gain_update_mc_ch0=1;
#endif
                int_unlock(lock);

                max_mc_gain_l=mc_filtes_l->total_gain;
            }
        }
        else
        {
            tt_mc_control->codec_tm_iir_ch2_bypass=1;

            lock = int_lock();
#ifdef ANC_GAIN_RAMP
            tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch2=0;
            *codec_tm_iir_gain_ext_mc_l=0;
            hal_sys_timer_delay_us(1);
            tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch2=1;
#else
            anc_mc_gain->codec_mute_gain_update_mc_ch0=0;
            anc_mc_gain->codec_mute_gain_coef_mc_ch0=0;
            hal_sys_timer_delay_us(1);
            anc_mc_gain->codec_mute_gain_update_mc_ch0=1;
#endif
            int_unlock(lock);

            tt_mc_control->codec_tm_iir_count_ch2=0;
        }

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            for(i=0;i<IIR_COUNTER;i++)
            {
                mc_filtes_r_old.iir_coef[i]=mc_filtes_r->iir_coef[i];
             }

            mc_filtes_r_old.total_gain=mc_filtes_r->total_gain;
            mc_filtes_r_old.iir_counter=mc_filtes_r->iir_counter;
            mc_filtes_r_old.iir_bypass_flag=mc_filtes_r->iir_bypass_flag;

            max_mc_gain_r=(1<<GAIN_Q);

            //Set the mc gain;
            lock = int_lock();
#ifdef ANC_GAIN_RAMP
            tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch3=0;
            *codec_tm_iir_gain_ext_mc_r=0;
            hal_sys_timer_delay_us(1);
            tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch3=1;
#else
            anc_mc_gain->codec_mute_gain_update_mc_ch1=0;
            anc_mc_gain->codec_mute_gain_coef_mc_ch1=0;
            hal_sys_timer_delay_us(1);
            anc_mc_gain->codec_mute_gain_update_mc_ch1=1;
#endif
            int_unlock(lock);

            if(mc_filtes_r->iir_bypass_flag==0)
            {
                tt_mc_control->codec_tm_iir_ch3_bypass=0;

                if(mc_filtes_r->iir_counter>IIR_COUNTER)
                {
                    tt_mc_control->codec_tm_iir_count_ch3=IIR_COUNTER;
                }
                else if(mc_filtes_r->iir_counter>0)
                {
                    tt_mc_control->codec_tm_iir_count_ch3=mc_filtes_r->iir_counter;
                }
                else
                {
                    tt_mc_control->codec_tm_iir_ch3_bypass=1;
                    LOG_I("Error fb_filtes_r IIR counter:%d", mc_filtes_r->iir_counter);
                    err=ANC_TYPE_ERR;
                }

                if(tm_iir_coef_using==0)
                {
                    //music cancel ch iir coefs settings
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_mc_iir_coefs0_r[i].a1=-mc_filtes_r->iir_coef[i].coef_a[1];
                        anc_mc_iir_coefs0_r[i].a2=-mc_filtes_r->iir_coef[i].coef_a[2];
                        anc_mc_iir_coefs0_r[i].b0=mc_filtes_r->iir_coef[i].coef_b[0];
                        anc_mc_iir_coefs0_r[i].b1=mc_filtes_r->iir_coef[i].coef_b[1];
                        anc_mc_iir_coefs0_r[i].b2=mc_filtes_r->iir_coef[i].coef_b[2];
                    }
                    anc_mc_iir_coefs0_r[0].b0=(int32)((mc_filtes_r->iir_coef[0].coef_b[0]*(int64_t)mc_filtes_r->total_gain)>>GAIN_Q);
                    anc_mc_iir_coefs0_r[0].b1=(int32)((mc_filtes_r->iir_coef[0].coef_b[1]*(int64_t)mc_filtes_r->total_gain)>>GAIN_Q);
                    anc_mc_iir_coefs0_r[0].b2=(int32)((mc_filtes_r->iir_coef[0].coef_b[2]*(int64_t)mc_filtes_r->total_gain)>>GAIN_Q);
                }
                else
                {
                    //music cancel ch iir coefs settings
                    for(i=0;i<IIR_COUNTER;i++)
                    {
                        anc_mc_iir_coefs1_r[i].a1=-mc_filtes_r->iir_coef[i].coef_a[1];
                        anc_mc_iir_coefs1_r[i].a2=-mc_filtes_r->iir_coef[i].coef_a[2];
                        anc_mc_iir_coefs1_r[i].b0=mc_filtes_r->iir_coef[i].coef_b[0];
                        anc_mc_iir_coefs1_r[i].b1=mc_filtes_r->iir_coef[i].coef_b[1];
                        anc_mc_iir_coefs1_r[i].b2=mc_filtes_r->iir_coef[i].coef_b[2];
                    }
                    anc_mc_iir_coefs1_r[0].b0=(int32)((mc_filtes_r->iir_coef[0].coef_b[0]*(int64_t)mc_filtes_r->total_gain)>>GAIN_Q);
                    anc_mc_iir_coefs1_r[0].b1=(int32)((mc_filtes_r->iir_coef[0].coef_b[1]*(int64_t)mc_filtes_r->total_gain)>>GAIN_Q);
                    anc_mc_iir_coefs1_r[0].b2=(int32)((mc_filtes_r->iir_coef[0].coef_b[2]*(int64_t)mc_filtes_r->total_gain)>>GAIN_Q);
                 }
                if(err!=ANC_NO_ERR)
                {
                    max_mc_gain_r=0;
                }

                if(anc_gain_delay==ANC_GAIN_NO_DELAY)
                {
                    //Set the MC gain;
                lock = int_lock();
#ifdef ANC_GAIN_RAMP
                tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch3=0;
                *codec_tm_iir_gain_ext_mc_r=(int32)(((float)max_mc_gain_r/512.0f)*FIXED_GAIN_RAMP_Q);
                hal_sys_timer_delay_us(1);
                tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch3=1;
#else
                anc_mc_gain->codec_mute_gain_update_mc_ch1=0;
                anc_mc_gain->codec_mute_gain_coef_mc_ch1=max_mc_gain_r;
                hal_sys_timer_delay_us(1);
                anc_mc_gain->codec_mute_gain_update_mc_ch1=1;
#endif
                int_unlock(lock);


                }
            }
            else
            {
                tt_mc_control->codec_tm_iir_ch3_bypass=1;
                lock = int_lock();
#ifdef ANC_GAIN_RAMP
                tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch3=0;
                *codec_tm_iir_gain_ext_mc_r=(int32)(((float)mc_filtes_r->total_gain/512.0f)*FIXED_GAIN_RAMP_Q);
                hal_sys_timer_delay_us(1);
                tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch3=1;
#else
                anc_mc_gain->codec_mute_gain_update_mc_ch1=0;
                anc_mc_gain->codec_mute_gain_coef_mc_ch1=mc_filtes_r->total_gain;
                hal_sys_timer_delay_us(1);
                anc_mc_gain->codec_mute_gain_update_mc_ch1=1;
#endif
                int_unlock(lock);

                max_mc_gain_r=mc_filtes_r->total_gain;
            }
        }
        else
        {
            tt_mc_control->codec_tm_iir_ch3_bypass=1;
            lock = int_lock();
#ifdef ANC_GAIN_RAMP
            tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch3=0;
            *codec_tm_iir_gain_ext_mc_r=0;
            hal_sys_timer_delay_us(1);
            tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch3=1;
#else
            anc_mc_gain->codec_mute_gain_update_mc_ch1=0;
            anc_mc_gain->codec_mute_gain_coef_mc_ch1=0;
            hal_sys_timer_delay_us(1);
            anc_mc_gain->codec_mute_gain_update_mc_ch1=1;
#endif
            int_unlock(lock);

            tt_mc_control->codec_tm_iir_count_ch3=0;
        }

        total_iir_counter=total_iir_counter+tt_mc_control->codec_tm_iir_count_ch2+tt_mc_control->codec_tm_iir_count_ch3;

        LOG_I("%s:total_iir_counter:%d", __func__,total_iir_counter);

        //enable MC.
        tm_config->codec_mm_enable_ch0=1;
        tm_config->codec_mm_enable_ch1=1;

        //enable all IIR filters
        tt_mc_control->codec_tm_iir_enable=1;

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

#ifdef ANC_GAIN_RAMP
static void anc_ctrl_hack_bug(void)
{
    anc_gain_ff_iir_coefs0_l[0].a1=0;
    anc_gain_ff_iir_coefs0_l[0].a2=0;
    anc_gain_ff_iir_coefs0_l[0].b0=0;
    anc_gain_ff_iir_coefs0_l[0].b1=0;
    anc_gain_ff_iir_coefs0_l[0].b2=0;

    anc_gain_ff_iir_coefs1_l[0].a1=0;
    anc_gain_ff_iir_coefs1_l[0].a2=0;
    anc_gain_ff_iir_coefs1_l[0].b0=0;
    anc_gain_ff_iir_coefs1_l[0].b1=0;
    anc_gain_ff_iir_coefs1_l[0].b2=0;

    anc_gain_ff_iir_coefs0_r[0].a1=0;
    anc_gain_ff_iir_coefs0_r[0].a2=0;
    anc_gain_ff_iir_coefs0_r[0].b0=0;
    anc_gain_ff_iir_coefs0_r[0].b1=0;
    anc_gain_ff_iir_coefs0_r[0].b2=0;

    anc_gain_ff_iir_coefs1_r[0].a1=0;
    anc_gain_ff_iir_coefs1_r[0].a2=0;
    anc_gain_ff_iir_coefs1_r[0].b0=0;
    anc_gain_ff_iir_coefs1_r[0].b1=0;
    anc_gain_ff_iir_coefs1_r[0].b2=0;

    anc_gain_fb_iir_coefs0_l[0].a1=0;
    anc_gain_fb_iir_coefs0_l[0].a2=0;
    anc_gain_fb_iir_coefs0_l[0].b0=0;
    anc_gain_fb_iir_coefs0_l[0].b1=0;
    anc_gain_fb_iir_coefs0_l[0].b2=0;

    anc_gain_fb_iir_coefs1_l[0].a1=0;
    anc_gain_fb_iir_coefs1_l[0].a2=0;
    anc_gain_fb_iir_coefs1_l[0].b0=0;
    anc_gain_fb_iir_coefs1_l[0].b1=0;
    anc_gain_fb_iir_coefs1_l[0].b2=0;

    anc_gain_fb_iir_coefs0_r[0].a1=0;
    anc_gain_fb_iir_coefs0_r[0].a2=0;
    anc_gain_fb_iir_coefs0_r[0].b0=0;
    anc_gain_fb_iir_coefs0_r[0].b1=0;
    anc_gain_fb_iir_coefs0_r[0].b2=0;

    anc_gain_fb_iir_coefs1_r[0].a1=0;
    anc_gain_fb_iir_coefs1_r[0].a2=0;
    anc_gain_fb_iir_coefs1_r[0].b0=0;
    anc_gain_fb_iir_coefs1_r[0].b1=0;
    anc_gain_fb_iir_coefs1_r[0].b2=0;

    anc_iir_control->codec_iir_enable=1;

    hal_sys_timer_delay_us(100);

    anc_iir_control->codec_iir_enable=0;

    return;
}

#endif
static void anc_ctrl_reg_init(void)
{

#ifdef ANC_GAIN_RAMP
    anc_ctrl_hack_bug();
#endif

    //disable ANC;
    anc_control->codec_anc_enable_ch0=0;
    anc_control->codec_anc_enable_ch1=0;

    //set the FF calib gain;
    anc_iir_calib_gain_config->codec_anc_calib_gain_update_ff_ch0=0;
    anc_ff_iir_calib_gain->codec_anc_calib_gain_coef_ff_ch0=1<<CALIB_GAIN_Q;
    anc_iir_calib_gain_config->codec_anc_calib_gain_update_ff_ch0=1;

    //set the FF calib gain;
    anc_iir_calib_gain_config->codec_anc_calib_gain_update_ff_ch1=0;
    anc_ff_iir_calib_gain->codec_anc_calib_gain_coef_ff_ch1=1<<CALIB_GAIN_Q;
    anc_iir_calib_gain_config->codec_anc_calib_gain_update_ff_ch1=1;

    //set the FB calib gain;
    anc_iir_calib_gain_config->codec_anc_calib_gain_update_fb_ch0=0;
    anc_fb_iir_calib_gain->codec_anc_calib_gain_coef_fb_ch0=1<<CALIB_GAIN_Q;
    anc_iir_calib_gain_config->codec_anc_calib_gain_update_fb_ch0=1;

    //set the FB calib gain;
    anc_iir_calib_gain_config->codec_anc_calib_gain_update_fb_ch1=0;
    anc_fb_iir_calib_gain->codec_anc_calib_gain_coef_fb_ch1=1<<CALIB_GAIN_Q;
    anc_iir_calib_gain_config->codec_anc_calib_gain_update_fb_ch1=1;

#if defined(AUDIO_ANC_TT_HW)
    tm_config->codec_tt_enable_ch0=1;
    tm_config->codec_tt_enable_ch1=1;
   // tm_config->codec_tt_adc_sel_ch0=0;
  //  tm_config->codec_tt_adc_sel_ch1=1;
#endif

#if defined(AUDIO_ANC_FB_MC_HW)
    tm_config->codec_mm_enable_ch0=1;
    tm_config->codec_mm_enable_ch1=1;
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

#if defined(AUDIO_ANC_FIR_HW)
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

#if defined(AUDIO_ANC_TT_HW)
    anc_tt_gain->codec_mute_gain_pass0_tt_ch0=1;
    anc_tt_gain->codec_mute_gain_pass0_tt_ch1=1;
#endif

#if defined(AUDIO_ANC_FB_MC_HW)
    anc_mc_gain->codec_mute_gain_pass0_mc_ch0=1;
    anc_mc_gain->codec_mute_gain_pass0_mc_ch1=1;
#endif

#if defined(AUDIO_ANC_FIR_HW)
    hal_cmu_codec_fir_enable(104000000);
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
    uint32_t lock;

    LOG_I("%s", __func__);

    if(anc_type==ANC_FEEDFORWARD)
    {
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            //clear iir counter
            anc_iir_control->codec_iir_count_ch0=0;

#if defined(AUDIO_ANC_FIR_HW)
        //setting the feedforward FIR filters;
            fir_ff_config0_l->stream0_fir1=0;

            fir_control->fir_stream_enable_ch0=1;

            fir_config->pdu_fs_swap=1;

            fir_config->anc_coef_sel_pdu0_fs0=0;
            fir_config->anc_coef_sel_pdu1_fs1=0;

            fir_ff_config0_l->fir_order=FIR_LEN;

            //feedforward left ch FIR coefs settings
            for(int i=0;i<fir_ff_config0_l->fir_order;i++)
            {
                anc_ff_fir_coefs_l[i]=0;
            }
            anc_ff_fir_coefs_l[0]=32767*256;

            fir_ff_config1_l->fir_gain_sel=1;

            anc_control->codec_ff_ch0_fir_en=1;
#endif

            //set the FF gain;
#ifdef ANC_GAIN_RAMP
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch0=0;
            anc_ff_gain->codec_anc_mute_gain_ff_ch0=512;
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch0=1;

            anc_gain_ff_iir_coefs0_l[0].a1=-iir_coef_gain_ramp_ff.coef_a[1];
            anc_gain_ff_iir_coefs0_l[0].a2=-iir_coef_gain_ramp_ff.coef_a[2];
            anc_gain_ff_iir_coefs0_l[0].b0=iir_coef_gain_ramp_ff.coef_b[0];
            anc_gain_ff_iir_coefs0_l[0].b1=iir_coef_gain_ramp_ff.coef_b[1];
            anc_gain_ff_iir_coefs0_l[0].b2=iir_coef_gain_ramp_ff.coef_b[2];

            anc_gain_ff_iir_coefs1_l[0].a1=-iir_coef_gain_ramp_ff.coef_a[1];
            anc_gain_ff_iir_coefs1_l[0].a2=-iir_coef_gain_ramp_ff.coef_a[2];
            anc_gain_ff_iir_coefs1_l[0].b0=iir_coef_gain_ramp_ff.coef_b[0];
            anc_gain_ff_iir_coefs1_l[0].b1=iir_coef_gain_ramp_ff.coef_b[1];
            anc_gain_ff_iir_coefs1_l[0].b2=iir_coef_gain_ramp_ff.coef_b[2];

            anc_iir_gain_config->codec_iir_gaincal_ext_ch0_bypass=0;
            anc_iir_gain_config->codec_iir_gainuse_ext_ch0_bypass=0;
            anc_iir_gain_config->codec_iir_gain_ext_sel_ch0=0;

            lock = int_lock();
            anc_iir_gain_config->codec_iir_gain_ext_update_ch0=0;
            *codec_iir_gain_ext_ff_l=0;
            hal_sys_timer_delay_us(1);
            anc_iir_gain_config->codec_iir_gain_ext_update_ch0=1;
            int_unlock(lock);


            ff_ramp_gain_l=0;
#else
            lock = int_lock();
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch0=0;
            anc_ff_gain->codec_anc_mute_gain_ff_ch0=0;
            hal_sys_timer_delay_us(1);
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch0=1;
            int_unlock(lock);

#endif

            ff_filtes_l_old.total_gain=0;
            ff_filtes_l_old.iir_counter=0;
            ff_filtes_l_old.iir_bypass_flag=1;
        }

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            //clear iir counter
            anc_iir_control->codec_iir_count_ch1=0;

#if defined(AUDIO_ANC_FIR_HW)
            fir_ff_config0_r->stream0_fir1=0;

            fir_control->fir_stream_enable_ch1=1;

            fir_config->pdu_fs_swap=1;

            fir_config->anc_coef_sel_pdu0_fs0=0;
            fir_config->anc_coef_sel_pdu1_fs1=0;

            fir_ff_config0_r->fir_order=FIR_LEN;

            //feedforward left ch FIR coefs settings
            for(int i=0;i<fir_ff_config0_r->fir_order;i++)
            {
                anc_ff_fir_coefs_r[i]=0;
            }

            anc_ff_fir_coefs_r[0]=32767*256;

            fir_ff_config1_r->fir_gain_sel=1;

            anc_control->codec_ff_ch1_fir_en=1;
#endif

            //set the FF gain;
#ifdef ANC_GAIN_RAMP
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch1=0;
            anc_ff_gain->codec_anc_mute_gain_ff_ch1=512;
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch1=1;

            anc_gain_ff_iir_coefs0_r[0].a1=-iir_coef_gain_ramp_ff.coef_a[1];
            anc_gain_ff_iir_coefs0_r[0].a2=-iir_coef_gain_ramp_ff.coef_a[2];
            anc_gain_ff_iir_coefs0_r[0].b0=iir_coef_gain_ramp_ff.coef_b[0];
            anc_gain_ff_iir_coefs0_r[0].b1=iir_coef_gain_ramp_ff.coef_b[1];
            anc_gain_ff_iir_coefs0_r[0].b2=iir_coef_gain_ramp_ff.coef_b[2];

            anc_gain_ff_iir_coefs1_r[0].a1=-iir_coef_gain_ramp_ff.coef_a[1];
            anc_gain_ff_iir_coefs1_r[0].a2=-iir_coef_gain_ramp_ff.coef_a[2];
            anc_gain_ff_iir_coefs1_r[0].b0=iir_coef_gain_ramp_ff.coef_b[0];
            anc_gain_ff_iir_coefs1_r[0].b1=iir_coef_gain_ramp_ff.coef_b[1];
            anc_gain_ff_iir_coefs1_r[0].b2=iir_coef_gain_ramp_ff.coef_b[2];

            anc_iir_gain_config->codec_iir_gaincal_ext_ch1_bypass=0;
            anc_iir_gain_config->codec_iir_gainuse_ext_ch1_bypass=0;
            anc_iir_gain_config->codec_iir_gain_ext_sel_ch1=0;

            lock = int_lock();
            anc_iir_gain_config->codec_iir_gain_ext_update_ch1=0;
            *codec_iir_gain_ext_ff_l=0;
            hal_sys_timer_delay_us(1);
            anc_iir_gain_config->codec_iir_gain_ext_update_ch1=1;
            int_unlock(lock);

            ff_ramp_gain_r=0;
#else
            lock = int_lock();
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch1=0;
            anc_ff_gain->codec_anc_mute_gain_ff_ch1=0;
            hal_sys_timer_delay_us(1);
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch1=1;
            int_unlock(lock);
#endif

            ff_filtes_r_old.total_gain=0;
            ff_filtes_r_old.iir_counter=0;
            ff_filtes_r_old.iir_bypass_flag=1;
        }
        ff_iir_reset_flag=0;


    }

#if defined(AUDIO_ANC_TT_HW)
    if(anc_type==ANC_TALKTHRU)
    {
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            //clear iir counter
            tt_mc_control->codec_tm_iir_count_ch0=0;

            //set the FF gain;
#ifdef ANC_GAIN_RAMP
            anc_tt_gain->codec_mute_gain_update_tt_ch0=0;
            anc_tt_gain->codec_mute_gain_coef_tt_ch0=512;
            anc_tt_gain->codec_mute_gain_update_tt_ch0=1;

            anc_gain_tt_iir_coefs0_l[0].a1=-iir_coef_gain_ramp_tt.coef_a[1];
            anc_gain_tt_iir_coefs0_l[0].a2=-iir_coef_gain_ramp_tt.coef_a[2];
            anc_gain_tt_iir_coefs0_l[0].b0=iir_coef_gain_ramp_tt.coef_b[0];
            anc_gain_tt_iir_coefs0_l[0].b1=iir_coef_gain_ramp_tt.coef_b[1];
            anc_gain_tt_iir_coefs0_l[0].b2=iir_coef_gain_ramp_tt.coef_b[2];

            anc_gain_tt_iir_coefs1_l[0].a1=-iir_coef_gain_ramp_tt.coef_a[1];
            anc_gain_tt_iir_coefs1_l[0].a2=-iir_coef_gain_ramp_tt.coef_a[2];
            anc_gain_tt_iir_coefs1_l[0].b0=iir_coef_gain_ramp_tt.coef_b[0];
            anc_gain_tt_iir_coefs1_l[0].b1=iir_coef_gain_ramp_tt.coef_b[1];
            anc_gain_tt_iir_coefs1_l[0].b2=iir_coef_gain_ramp_tt.coef_b[2];

            tm_iir_gain_config->codec_tm_iir_gaincal_ext_ch0_bypass=0;
            tm_iir_gain_config->codec_tm_iir_gainuse_ext_ch0_bypass=0;
            tm_iir_gain_config->codec_tm_iir_gain_ext_sel_ch0=0;

            lock = int_lock();
            tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch0=0;
            *codec_tm_iir_gain_ext_tt_l=0;
            hal_sys_timer_delay_us(1);
            tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch0=1;
            int_unlock(lock);

            tt_ramp_gain_l=0;
#else
            lock = int_lock();
            anc_tt_gain->codec_mute_gain_update_tt_ch0=0;
            anc_tt_gain->codec_mute_gain_coef_tt_ch0=0;
            hal_sys_timer_delay_us(1);
            anc_tt_gain->codec_mute_gain_update_tt_ch0=1;
            int_unlock(lock);
#endif
            tt_filtes_l_old.total_gain=0;
            tt_filtes_l_old.iir_counter=0;
            tt_filtes_l_old.iir_bypass_flag=1;
        }

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            tt_mc_control->codec_tm_iir_count_ch1=0;

            //set the FF gain;
#ifdef ANC_GAIN_RAMP
            anc_tt_gain->codec_mute_gain_update_tt_ch1=0;
            anc_tt_gain->codec_mute_gain_coef_tt_ch1=512;
            anc_tt_gain->codec_mute_gain_update_tt_ch1=1;

            anc_gain_tt_iir_coefs0_r[0].a1=-iir_coef_gain_ramp_tt.coef_a[1];
            anc_gain_tt_iir_coefs0_r[0].a2=-iir_coef_gain_ramp_tt.coef_a[2];
            anc_gain_tt_iir_coefs0_r[0].b0=iir_coef_gain_ramp_tt.coef_b[0];
            anc_gain_tt_iir_coefs0_r[0].b1=iir_coef_gain_ramp_tt.coef_b[1];
            anc_gain_tt_iir_coefs0_r[0].b2=iir_coef_gain_ramp_tt.coef_b[2];

            anc_gain_tt_iir_coefs1_r[0].a1=-iir_coef_gain_ramp_tt.coef_a[1];
            anc_gain_tt_iir_coefs1_r[0].a2=-iir_coef_gain_ramp_tt.coef_a[2];
            anc_gain_tt_iir_coefs1_r[0].b0=iir_coef_gain_ramp_tt.coef_b[0];
            anc_gain_tt_iir_coefs1_r[0].b1=iir_coef_gain_ramp_tt.coef_b[1];
            anc_gain_tt_iir_coefs1_r[0].b2=iir_coef_gain_ramp_tt.coef_b[2];

            tm_iir_gain_config->codec_tm_iir_gaincal_ext_ch1_bypass=0;
            tm_iir_gain_config->codec_tm_iir_gainuse_ext_ch1_bypass=0;
            tm_iir_gain_config->codec_tm_iir_gain_ext_sel_ch1=0;

            lock = int_lock();
            tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch1=0;
            *codec_tm_iir_gain_ext_tt_r=0;
            hal_sys_timer_delay_us(1);
            tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch1=1;
            int_unlock(lock);

            tt_ramp_gain_r=0;
#else
            lock = int_lock();
            anc_tt_gain->codec_mute_gain_update_tt_ch1=0;
            anc_tt_gain->codec_mute_gain_coef_tt_ch1=0;
            hal_sys_timer_delay_us(1);
            anc_tt_gain->codec_mute_gain_update_tt_ch1=1;
            int_unlock(lock);

#endif
            tt_filtes_r_old.total_gain=0;
            tt_filtes_r_old.iir_counter=0;
            tt_filtes_r_old.iir_bypass_flag=1;
        }

        tt_iir_reset_flag=0;
    }
#endif

    if(anc_type==ANC_FEEDBACK)
    {
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            //clear iir counter
            anc_iir_control->codec_iir_count_ch2=0;


#if defined(AUDIO_ANC_FIR_HW)
        //setting the feedforward FIR filters;
            fir_fb_config0_l->stream0_fir1=0;

            fir_control->fir_stream_enable_ch2=1;

            fir_config->pdu_fs_swap=1;

            fir_config->anc_coef_sel_pdu0_fs0=0;
            fir_config->anc_coef_sel_pdu1_fs1=0;

            fir_fb_config0_l->fir_order=FIR_LEN;

            //feedforward left ch FIR coefs settings
            for(int i=0;i<fir_fb_config0_l->fir_order;i++)
            {
                anc_fb_fir_coefs_l[i]=0;
            }
            anc_fb_fir_coefs_l[0]=32767*256;

            fir_fb_config1_l->fir_gain_sel=1;

            anc_control->codec_fb_ch0_fir_en=1;
#endif


            //set the FB gain;
#ifdef ANC_GAIN_RAMP
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch0=0;
            anc_fb_gain->codec_anc_mute_gain_fb_ch0=512;
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch0=1;

            anc_gain_fb_iir_coefs0_l[0].a1=-iir_coef_gain_ramp_fb.coef_a[1];
            anc_gain_fb_iir_coefs0_l[0].a2=-iir_coef_gain_ramp_fb.coef_a[2];
            anc_gain_fb_iir_coefs0_l[0].b0=iir_coef_gain_ramp_fb.coef_b[0];
            anc_gain_fb_iir_coefs0_l[0].b1=iir_coef_gain_ramp_fb.coef_b[1];
            anc_gain_fb_iir_coefs0_l[0].b2=iir_coef_gain_ramp_fb.coef_b[2];

            anc_gain_fb_iir_coefs1_l[0].a1=-iir_coef_gain_ramp_fb.coef_a[1];
            anc_gain_fb_iir_coefs1_l[0].a2=-iir_coef_gain_ramp_fb.coef_a[2];
            anc_gain_fb_iir_coefs1_l[0].b0=iir_coef_gain_ramp_fb.coef_b[0];
            anc_gain_fb_iir_coefs1_l[0].b1=iir_coef_gain_ramp_fb.coef_b[1];
            anc_gain_fb_iir_coefs1_l[0].b2=iir_coef_gain_ramp_fb.coef_b[2];

            anc_iir_gain_config->codec_iir_gaincal_ext_ch2_bypass=0;
            anc_iir_gain_config->codec_iir_gainuse_ext_ch2_bypass=0;
            anc_iir_gain_config->codec_iir_gain_ext_sel_ch2=0;

            lock = int_lock();
            anc_iir_gain_config->codec_iir_gain_ext_update_ch2=0;
            *codec_iir_gain_ext_fb_l=0;
            hal_sys_timer_delay_us(1);
            anc_iir_gain_config->codec_iir_gain_ext_update_ch2=1;
            int_unlock(lock);

            fb_ramp_gain_l=0;
#else
            lock = int_lock();
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch0=0;
            anc_fb_gain->codec_anc_mute_gain_fb_ch0=0;
            hal_sys_timer_delay_us(1);
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch0=1;
            int_unlock(lock);
#endif
#if defined(AUDIO_ANC_FB_MC) ||defined(AUDIO_ANC_FB_MC_HW)
            feedback_mc_settings->codec_adc_mc_en_ch0=0;
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

#if defined(AUDIO_ANC_FIR_HW)
            fir_fb_config0_r->stream0_fir1=0;

            fir_control->fir_stream_enable_ch3=1;

            fir_config->pdu_fs_swap=1;

            fir_config->anc_coef_sel_pdu0_fs0=0;
            fir_config->anc_coef_sel_pdu1_fs1=0;

            fir_fb_config0_r->fir_order=FIR_LEN;

            //feedforward left ch FIR coefs settings
            for(int i=0;i<fir_fb_config0_r->fir_order;i++)
            {
                anc_fb_fir_coefs_r[i]=0;
            }

            anc_fb_fir_coefs_r[0]=32767*256;

            fir_fb_config1_r->fir_gain_sel=1;

            anc_control->codec_fb_ch1_fir_en=1;
#endif

            //set the FB gain;
#ifdef ANC_GAIN_RAMP
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch1=0;
            anc_fb_gain->codec_anc_mute_gain_fb_ch1=512;
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch1=1;
            anc_gain_fb_iir_coefs0_r[0].a1=-iir_coef_gain_ramp_fb.coef_a[1];
            anc_gain_fb_iir_coefs0_r[0].a2=-iir_coef_gain_ramp_fb.coef_a[2];
            anc_gain_fb_iir_coefs0_r[0].b0=iir_coef_gain_ramp_fb.coef_b[0];
            anc_gain_fb_iir_coefs0_r[0].b1=iir_coef_gain_ramp_fb.coef_b[1];
            anc_gain_fb_iir_coefs0_r[0].b2=iir_coef_gain_ramp_fb.coef_b[2];

            anc_gain_fb_iir_coefs1_r[0].a1=-iir_coef_gain_ramp_fb.coef_a[1];
            anc_gain_fb_iir_coefs1_r[0].a2=-iir_coef_gain_ramp_fb.coef_a[2];
            anc_gain_fb_iir_coefs1_r[0].b0=iir_coef_gain_ramp_fb.coef_b[0];
            anc_gain_fb_iir_coefs1_r[0].b1=iir_coef_gain_ramp_fb.coef_b[1];
            anc_gain_fb_iir_coefs1_r[0].b2=iir_coef_gain_ramp_fb.coef_b[2];

            anc_iir_gain_config->codec_iir_gaincal_ext_ch3_bypass=0;
            anc_iir_gain_config->codec_iir_gainuse_ext_ch3_bypass=0;
            anc_iir_gain_config->codec_iir_gain_ext_sel_ch3=0;

            lock = int_lock();
            anc_iir_gain_config->codec_iir_gain_ext_update_ch3=0;
            *codec_iir_gain_ext_fb_r=0;
            hal_sys_timer_delay_us(1);
            anc_iir_gain_config->codec_iir_gain_ext_update_ch3=1;
            int_unlock(lock);

            fb_ramp_gain_r=0;

#else
            lock = int_lock();
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch1=0;
            anc_fb_gain->codec_anc_mute_gain_fb_ch1=0;
            hal_sys_timer_delay_us(1);
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch1=1;
            int_unlock(lock);
#endif

#if defined(AUDIO_ANC_FB_MC) ||defined(AUDIO_ANC_FB_MC_HW)
            feedback_mc_settings->codec_adc_mc_en_ch1=0;
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
            tt_mc_control->codec_tm_iir_count_ch2=0;

            //set the MC gain;
#ifdef ANC_GAIN_RAMP
            anc_mc_gain->codec_mute_gain_update_mc_ch0=0;
            anc_mc_gain->codec_mute_gain_coef_mc_ch0=512;
            anc_mc_gain->codec_mute_gain_update_mc_ch0=1;

            anc_gain_mc_iir_coefs0_l[0].a1=-iir_coef_gain_ramp_mc.coef_a[1];
            anc_gain_mc_iir_coefs0_l[0].a2=-iir_coef_gain_ramp_mc.coef_a[2];
            anc_gain_mc_iir_coefs0_l[0].b0=iir_coef_gain_ramp_mc.coef_b[0];
            anc_gain_mc_iir_coefs0_l[0].b1=iir_coef_gain_ramp_mc.coef_b[1];
            anc_gain_mc_iir_coefs0_l[0].b2=iir_coef_gain_ramp_mc.coef_b[2];

            anc_gain_mc_iir_coefs1_l[0].a1=-iir_coef_gain_ramp_mc.coef_a[1];
            anc_gain_mc_iir_coefs1_l[0].a2=-iir_coef_gain_ramp_mc.coef_a[2];
            anc_gain_mc_iir_coefs1_l[0].b0=iir_coef_gain_ramp_mc.coef_b[0];
            anc_gain_mc_iir_coefs1_l[0].b1=iir_coef_gain_ramp_mc.coef_b[1];
            anc_gain_mc_iir_coefs1_l[0].b2=iir_coef_gain_ramp_mc.coef_b[2];

            tm_iir_gain_config->codec_tm_iir_gaincal_ext_ch2_bypass=0;
            tm_iir_gain_config->codec_tm_iir_gainuse_ext_ch2_bypass=0;
            tm_iir_gain_config->codec_tm_iir_gain_ext_sel_ch2=0;

            lock = int_lock();
            tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch2=0;
            *codec_tm_iir_gain_ext_mc_l=0;
            hal_sys_timer_delay_us(1);
            tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch2=1;
            int_unlock(lock);

            mc_ramp_gain_l=0;
#else
            lock = int_lock();
            anc_mc_gain->codec_mute_gain_update_mc_ch0=0;
            anc_mc_gain->codec_mute_gain_coef_mc_ch0=0;
            hal_sys_timer_delay_us(1);
            anc_mc_gain->codec_mute_gain_update_mc_ch0=1;
            int_unlock(lock);

#endif

            mc_filtes_l_old.total_gain=0;
            mc_filtes_l_old.iir_counter=0;
            mc_filtes_l_old.iir_bypass_flag=1;


            //set MC delay.
            tm_config->codec_mm_fifo_bypass_ch0=1;
            /*
            tm_config->codec_mm_delay_ch0=4;
          //  tm_config->codec_mm_delay_update=0;
          //  tm_config->codec_mm_delay_update=1;
            tm_config->codec_mm_fifo_en_ch0=1;
            */
        }
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            //clear iir counter
            tt_mc_control->codec_tm_iir_count_ch3=0;

            //set the MC gain;
#ifdef ANC_GAIN_RAMP
            anc_mc_gain->codec_mute_gain_update_mc_ch1=0;
            anc_mc_gain->codec_mute_gain_coef_mc_ch1=512;
            anc_mc_gain->codec_mute_gain_update_mc_ch1=1;

            anc_gain_mc_iir_coefs0_r[0].a1=-iir_coef_gain_ramp_mc.coef_a[1];
            anc_gain_mc_iir_coefs0_r[0].a2=-iir_coef_gain_ramp_mc.coef_a[2];
            anc_gain_mc_iir_coefs0_r[0].b0=iir_coef_gain_ramp_mc.coef_b[0];
            anc_gain_mc_iir_coefs0_r[0].b1=iir_coef_gain_ramp_mc.coef_b[1];
            anc_gain_mc_iir_coefs0_r[0].b2=iir_coef_gain_ramp_mc.coef_b[2];

            anc_gain_mc_iir_coefs1_r[0].a1=-iir_coef_gain_ramp_mc.coef_a[1];
            anc_gain_mc_iir_coefs1_r[0].a2=-iir_coef_gain_ramp_mc.coef_a[2];
            anc_gain_mc_iir_coefs1_r[0].b0=iir_coef_gain_ramp_mc.coef_b[0];
            anc_gain_mc_iir_coefs1_r[0].b1=iir_coef_gain_ramp_mc.coef_b[1];
            anc_gain_mc_iir_coefs1_r[0].b2=iir_coef_gain_ramp_mc.coef_b[2];

            tm_iir_gain_config->codec_tm_iir_gaincal_ext_ch3_bypass=0;
            tm_iir_gain_config->codec_tm_iir_gainuse_ext_ch3_bypass=0;
            tm_iir_gain_config->codec_tm_iir_gain_ext_sel_ch3=0;

            lock = int_lock();
            tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch3=0;
            *codec_tm_iir_gain_ext_mc_r=0;
            hal_sys_timer_delay_us(1);
            tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch3=1;
            int_unlock(lock);

            mc_ramp_gain_r=0;
#else
            lock = int_lock();
            anc_mc_gain->codec_mute_gain_update_mc_ch1=0;
            anc_mc_gain->codec_mute_gain_coef_mc_ch1=0;
            hal_sys_timer_delay_us(1);
            anc_mc_gain->codec_mute_gain_update_mc_ch1=1;
            int_unlock(lock);
#endif

            mc_filtes_r_old.total_gain=0;
            mc_filtes_r_old.iir_counter=0;
            mc_filtes_r_old.iir_bypass_flag=1;

            //set MC delay.
            tm_config->codec_mm_fifo_bypass_ch1=1;
            /*
            tm_config->codec_mm_delay_ch0=4;
          //  tm_config->codec_mm_delay_update=0;
          //  tm_config->codec_mm_delay_update=1;
            tm_config->codec_mm_fifo_en_ch0=1;
            */
        }

        mc_iir_reset_flag=0;
#endif
    }

    if(((ff_open_flag==1) ||(ff_open_flag==1) ) && (fb_open_flag==1))
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
    else if(fb_open_flag==1)
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
    else if(fb_open_flag==0)
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
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch0=0;
            anc_ff_gain->codec_anc_mute_gain_ff_ch0=0;
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch0=1;
        }
#if defined(AUDIO_ANC_TT_HW)
        if(anc_type==ANC_TALKTHRU)
        {
            //clear iir counter
            tt_mc_control->codec_tm_iir_count_ch0=0;

            //set the TT gain;
            anc_tt_gain->codec_mute_gain_update_tt_ch0=0;
            anc_tt_gain->codec_mute_gain_coef_tt_ch0=0;
            anc_tt_gain->codec_mute_gain_update_tt_ch0=1;
        }
#endif
        if(anc_type==ANC_FEEDBACK)
        {
            //clear iir counter
            anc_iir_control->codec_iir_count_ch2=0;

            //set the FB gain;
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch0=0;
            anc_fb_gain->codec_anc_mute_gain_fb_ch0=0;
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch0=1;

#if defined(AUDIO_ANC_FB_MC) ||defined(AUDIO_ANC_FB_MC_HW)
              feedback_mc_settings->codec_feedback_mc_en_ch0=0;
#endif
        }
#if defined(AUDIO_ANC_FB_MC_HW)
        if(anc_type==ANC_MUSICCANCLE)
        {
            //clear iir counter
            tt_mc_control->codec_tm_iir_count_ch2=0;

            //set the TT gain;
            anc_mc_gain->codec_mute_gain_update_mc_ch0=0;
            anc_mc_gain->codec_mute_gain_coef_mc_ch0=0;
            anc_mc_gain->codec_mute_gain_update_mc_ch0=1;
        }
#endif
		if(((ff_open_flag==1) ||(ff_open_flag==1) ) && (fb_open_flag==1))
        {
            anc_control->codec_feedback_ch0=1;
            anc_control->codec_dual_anc_ch0=1;
        }
        else if(fb_open_flag==1)
        {
            anc_control->codec_feedback_ch0=1;
            anc_control->codec_dual_anc_ch0=0;
        }
        else if(fb_open_flag==0)
        {
            anc_control->codec_feedback_ch0=0;
            anc_control->codec_dual_anc_ch0=0;
        }
    }

    if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
    {
        if(anc_type==ANC_FEEDFORWARD)
        {
            //clear iir counter
            anc_iir_control->codec_iir_count_ch1=0;

            //set the FF gain;
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch1=0;
            anc_ff_gain->codec_anc_mute_gain_ff_ch1=0;
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch1=1;
        }
#if defined(AUDIO_ANC_TT_HW)
        if(anc_type==ANC_FEEDFORWARD)
        {
            //clear iir counter
            tt_mc_control->codec_tm_iir_count_ch1=0;

            //set the tt gain;
            anc_tt_gain->codec_mute_gain_update_tt_ch1=0;
            anc_tt_gain->codec_mute_gain_coef_tt_ch1=0;
            anc_tt_gain->codec_mute_gain_update_tt_ch1=1;
        }
#endif

        if(anc_type==ANC_FEEDBACK)
        {
            //clear iir counter
            anc_iir_control->codec_iir_count_ch3=0;

            //set the FB gain;
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch1=0;
            anc_fb_gain->codec_anc_mute_gain_fb_ch1=0;
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch1=1;

#if defined(AUDIO_ANC_FB_MC) ||defined(AUDIO_ANC_FB_MC_HW)
      //      feedback_mc_settings->codec_adc_mc_en_ch1=0;
            feedback_mc_settings->codec_feedback_mc_en_ch1=0;
#endif
        }
#if defined(AUDIO_ANC_FB_MC_HW)
        if(anc_type==ANC_MUSICCANCLE)
        {
            //clear iir counter
            tt_mc_control->codec_tm_iir_count_ch3=0;

            //set the mc gain;
            anc_mc_gain->codec_mute_gain_update_mc_ch1=0;
            anc_mc_gain->codec_mute_gain_coef_mc_ch1=0;
            anc_mc_gain->codec_mute_gain_update_mc_ch1=1;
        }
#endif

		if(((ff_open_flag==1) ||(ff_open_flag==1) ) && (fb_open_flag==1))
        {
            anc_control->codec_feedback_ch1=1;
            anc_control->codec_dual_anc_ch1=1;
        }
        else if(fb_open_flag==1)
        {
            anc_control->codec_feedback_ch1=1;
            anc_control->codec_dual_anc_ch1=0;

        }
        else if(fb_open_flag==0)
        {
            anc_control->codec_feedback_ch1=0;
            anc_control->codec_dual_anc_ch1=0;
        }
    }
}

int anc_opened(enum ANC_TYPE_T anc_type)
{
    if (anc_type == ANC_FEEDFORWARD)
    {
        return ff_open_flag;
    }
    if (anc_type == ANC_FEEDBACK)
    {
        return fb_open_flag;
    }
#if defined(AUDIO_ANC_TT_HW)
    if (anc_type == ANC_TALKTHRU)
    {
        return tt_open_flag;
    }
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    if (anc_type == ANC_MUSICCANCLE)
    {
        return mc_open_flag;
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

    if(ff_open_flag==0&&fb_open_flag==0
#if defined(AUDIO_ANC_TT_HW)
        &&tt_open_flag==0
#endif
        )
    {
        hal_cmu_codec_iir_enable(98000000);
        anc_ctrl_reg_init();

        iir_coef_using=0;
        tm_iir_coef_using=0;
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

    }
#if defined(AUDIO_ANC_TT_HW)

    if(anc_type==ANC_TALKTHRU)
    {
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            max_tt_gain_l=0;
        }
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            max_tt_gain_r=0;
        }
        tt_open_flag=1;
    }
#endif


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
    }
#if defined(AUDIO_ANC_FB_MC_HW)
    if(anc_type==ANC_MUSICCANCLE)
    {
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            max_mc_gain_l=0;
        }
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            max_mc_gain_r=0;
        }
        mc_open_flag=1;
    }
#endif
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

#ifdef ANC_GAIN_RAMP
        ff_ramp_gain_l=0;
        ff_ramp_gain_r=0;
#else
        //set the FF gain;
        anc_ff_gain->codec_anc_mute_gain_update_ff_ch0=0;
        anc_ff_gain->codec_anc_mute_gain_update_ff_ch1=0;

        anc_ff_gain->codec_anc_mute_gain_ff_ch0=0;
        anc_ff_gain->codec_anc_mute_gain_ff_ch1=0;

        anc_ff_gain->codec_anc_mute_gain_update_ff_ch0=1;
        anc_ff_gain->codec_anc_mute_gain_update_ff_ch1=1;
#endif


#if defined(AUDIO_ANC_TT_HW)
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            max_tt_gain_l=0;
        }

        ff_open_flag=0;
#endif
    }
#if defined(AUDIO_ANC_TT_HW)
    if(anc_type==ANC_TALKTHRU)
    {
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            max_tt_gain_l=0;
        }

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            max_tt_gain_r=0;
        }

        tt_open_flag=0;

#ifdef ANC_GAIN_RAMP
        tt_ramp_gain_l=0;
        tt_ramp_gain_r=0;
#else
        //set the FF gain;
        anc_tt_gain->codec_mute_gain_update_tt_ch0=0;
        anc_tt_gain->codec_mute_gain_update_tt_ch1=0;

        anc_tt_gain->codec_mute_gain_coef_tt_ch0=0;
        anc_tt_gain->codec_mute_gain_coef_tt_ch1=0;

        anc_tt_gain->codec_mute_gain_update_tt_ch0=1;
        anc_tt_gain->codec_mute_gain_update_tt_ch1=1;
#endif
    }
#endif

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

#ifdef ANC_GAIN_RAMP
        fb_ramp_gain_l=0;
        fb_ramp_gain_r=0;
#else
        //set the FF gain;
        anc_fb_gain->codec_anc_mute_gain_update_fb_ch0=0;
        anc_fb_gain->codec_anc_mute_gain_update_fb_ch1=0;

        anc_fb_gain->codec_anc_mute_gain_fb_ch0=0;
        anc_fb_gain->codec_anc_mute_gain_fb_ch1=0;

        anc_fb_gain->codec_anc_mute_gain_update_fb_ch0=1;
        anc_fb_gain->codec_anc_mute_gain_update_fb_ch1=1;
#endif
    }

#if defined(AUDIO_ANC_FB_MC_HW)
    if(anc_type==ANC_MUSICCANCLE)
    {
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            max_mc_gain_l=0;
        }

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
        {
            max_mc_gain_r=0;
        }
        mc_open_flag=0;

#ifdef ANC_GAIN_RAMP
        mc_ramp_gain_l=0;
        mc_ramp_gain_r=0;
#else
        //set the MC gain;
        anc_mc_gain->codec_mute_gain_update_mc_ch0=0;
        anc_mc_gain->codec_mute_gain_update_mc_ch1=0;

        anc_mc_gain->codec_mute_gain_coef_mc_ch0=0;
        anc_mc_gain->codec_mute_gain_coef_mc_ch0=0;

        anc_mc_gain->codec_mute_gain_update_mc_ch0=1;
        anc_mc_gain->codec_mute_gain_update_mc_ch1=1;
#endif
    }
#endif
    anc_ctrl_reg_close(anc_type);

    if(ff_open_flag==0&&fb_open_flag==0
#if defined(AUDIO_ANC_TT_HW)
        &&tt_open_flag==0
#endif
        )
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
#if defined(AUDIO_ANC_TT_HW)
        tm_config->codec_tt_enable_ch0=1;
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
        tm_config->codec_mm_enable_ch0=1;
#endif
    }

     if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
    {
        anc_control->codec_anc_enable_ch1=1;
#ifdef ANC_FB_CHECK
        fb_check_ch1_config->codec_fb_check_enable_ch1=1;
#endif
#if defined(AUDIO_ANC_TT_HW)
        tm_config->codec_tt_enable_ch1=1;
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
        tm_config->codec_mm_enable_ch1=1;
#endif
    }
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
#ifndef ANC_GAIN_RAMP
        anc_control->codec_anc_enable_ch1=0;
#if defined(AUDIO_ANC_TT_HW)
        tm_config->codec_tt_enable_ch0=0;
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
        tm_config->codec_mm_enable_ch0=0;
#endif
#endif
    }

     if(anc_output_ch_map&AUD_CHANNEL_MAP_CH1)
    {
#ifdef ANC_FB_CHECK
        fb_check_ch1_config->codec_fb_check_enable_ch1=0;
        hwtimer_stop(fb_adc_check_dev_timer);
#endif
#ifndef ANC_GAIN_RAMP
        anc_control->codec_anc_enable_ch1=0;
#if defined(AUDIO_ANC_TT_HW)
        tm_config->codec_tt_enable_ch1=0;
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
        tm_config->codec_mm_enable_ch1=0;
#endif
#endif
    }

    anc_cfg_unlock();

    return ANC_NO_ERR;
}

int anc_set_gain(int32_t gain_ch_l, int32_t gain_ch_r,enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err=ANC_NO_ERR;
    uint32_t lock;

   /* if(gain_ch_l==511||gain_ch_r==511||gain_ch_l==0||gain_ch_r==0)
    {
        LOG_I("anc_set_gain gain_ch_l:%d,gain_ch_r:%d",gain_ch_l,gain_ch_r);
    }*/
    anc_cfg_lock();
#ifdef ANC_GAIN_RAMP

    if(anc_type==ANC_FEEDFORWARD)
    {
        //Set the FF gain;
        lock = int_lock();

        anc_iir_gain_config->codec_iir_gain_ext_update_ch0=0;
        anc_iir_gain_config->codec_iir_gain_ext_update_ch1=0;

        *codec_iir_gain_ext_ff_l=(int32)(((float)gain_ch_l/512.0f)*FIXED_GAIN_RAMP_Q);
        *codec_iir_gain_ext_ff_r=(int32)(((float)gain_ch_r/512.0f)*FIXED_GAIN_RAMP_Q);

         ff_ramp_gain_l=gain_ch_l;
         ff_ramp_gain_r=gain_ch_r;

         hal_sys_timer_delay_us(1);

        anc_iir_gain_config->codec_iir_gain_ext_update_ch0=1;
        anc_iir_gain_config->codec_iir_gain_ext_update_ch1=1;

        int_unlock(lock);
    }
    else if(anc_type==ANC_FEEDBACK)
    {
        //Set the FB gain;
        lock = int_lock();

        anc_iir_gain_config->codec_iir_gain_ext_update_ch2=0;
        anc_iir_gain_config->codec_iir_gain_ext_update_ch3=0;

        *codec_iir_gain_ext_fb_l=(int32)(((float)gain_ch_l/512.0f)*FIXED_GAIN_RAMP_Q);
        *codec_iir_gain_ext_fb_r=(int32)(((float)gain_ch_r/512.0f)*FIXED_GAIN_RAMP_Q);

        fb_ramp_gain_l=gain_ch_l;
        fb_ramp_gain_r=gain_ch_r;

        hal_sys_timer_delay_us(1);

        anc_iir_gain_config->codec_iir_gain_ext_update_ch2=1;
        anc_iir_gain_config->codec_iir_gain_ext_update_ch3=1;

        int_unlock(lock);
     }
#if defined(AUDIO_ANC_TT_HW)
    else if(anc_type==ANC_TALKTHRU)
    {
        //Set the TT gain;
        lock = int_lock();

        tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch0=0;
        tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch1=0;

        *codec_tm_iir_gain_ext_tt_l=(int32)(((float)gain_ch_l/512.0f)*FIXED_GAIN_RAMP_Q);
        *codec_tm_iir_gain_ext_tt_r=(int32)(((float)gain_ch_r/512.0f)*FIXED_GAIN_RAMP_Q);

        tt_ramp_gain_l=gain_ch_l;
        tt_ramp_gain_r=gain_ch_r;

        hal_sys_timer_delay_us(1);

        tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch0=1;
        tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch1=1;

        int_unlock(lock);

    }
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    else if(anc_type==ANC_MUSICCANCLE)
    {
        //Set the TT gain;
        lock = int_lock();

        tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch2=0;
        tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch3=0;

        *codec_tm_iir_gain_ext_mc_l=(int32)(((float)gain_ch_l/512.0f)*FIXED_GAIN_RAMP_Q);
        *codec_tm_iir_gain_ext_mc_r=(int32)(((float)gain_ch_r/512.0f)*FIXED_GAIN_RAMP_Q);

        mc_ramp_gain_l=gain_ch_l;
        mc_ramp_gain_r=gain_ch_r;

        hal_sys_timer_delay_us(1);

        tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch2=1;
        tm_iir_gain_config->codec_tm_iir_gain_ext_update_ch3=1;

        int_unlock(lock);
    }
#endif
    else
    {
        err=ANC_TYPE_ERR;
        LOG_I("Error ANC type:%d", anc_type);
    }
#else
    if(anc_type==ANC_FEEDFORWARD)
    {
        //Set the FF gain;
        lock = int_lock();
        anc_ff_gain->codec_anc_mute_gain_update_ff_ch0=0;
        anc_ff_gain->codec_anc_mute_gain_update_ff_ch1=0;

        anc_ff_gain->codec_anc_mute_gain_ff_ch0=gain_ch_l;
        anc_ff_gain->codec_anc_mute_gain_ff_ch1=gain_ch_r;

        hal_sys_timer_delay_us(1);

        anc_ff_gain->codec_anc_mute_gain_update_ff_ch0=1;
        anc_ff_gain->codec_anc_mute_gain_update_ff_ch1=1;
        int_unlock(lock);

    }
    else if(anc_type==ANC_FEEDBACK)
    {
        //Set the FB gain;
        lock = int_lock();
        anc_fb_gain->codec_anc_mute_gain_update_fb_ch0=0;
        anc_fb_gain->codec_anc_mute_gain_update_fb_ch1=0;

        anc_fb_gain->codec_anc_mute_gain_fb_ch0=gain_ch_l;
        anc_fb_gain->codec_anc_mute_gain_fb_ch1=gain_ch_r;

        hal_sys_timer_delay_us(1);

        anc_fb_gain->codec_anc_mute_gain_update_fb_ch0=1;
        anc_fb_gain->codec_anc_mute_gain_update_fb_ch1=1;

        int_unlock(lock);
    }
#if defined(AUDIO_ANC_TT_HW)
    else if(anc_type==ANC_TALKTHRU)
    {
        //Set the TT gain;
        lock = int_lock();
        anc_tt_gain->codec_mute_gain_update_tt_ch0=0;
        anc_tt_gain->codec_mute_gain_update_tt_ch1=0;

        anc_tt_gain->codec_mute_gain_coef_tt_ch0=gain_ch_l;
        anc_tt_gain->codec_mute_gain_coef_tt_ch1=gain_ch_r;

        hal_sys_timer_delay_us(1);

        anc_tt_gain->codec_mute_gain_update_tt_ch0=1;
        anc_tt_gain->codec_mute_gain_update_tt_ch1=1;

        int_unlock(lock);
    }
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    else if(anc_type==ANC_MUSICCANCLE)
    {
        //Set the TT gain;
        lock = int_lock();
        anc_mc_gain->codec_mute_gain_update_mc_ch0=0;
        anc_mc_gain->codec_mute_gain_update_mc_ch0=0;

        anc_mc_gain->codec_mute_gain_coef_mc_ch0=gain_ch_l;
        anc_mc_gain->codec_mute_gain_coef_mc_ch1=gain_ch_r;

        hal_sys_timer_delay_us(1);

        anc_mc_gain->codec_mute_gain_update_mc_ch0=1;
        anc_mc_gain->codec_mute_gain_update_mc_ch0=1;

        int_unlock(lock);
    }
#endif
    else
    {
        //if err ANC type,retun 0;
        err=ANC_TYPE_ERR;
        LOG_I("Error ANC type:%d", anc_type);
    }
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
        //Get the FF gain;
         *gain_ch_l=ff_ramp_gain_l;
         *gain_ch_r=ff_ramp_gain_r;
     }
    else if(anc_type==ANC_FEEDBACK)
    {
        //Get the FB gain;
         *gain_ch_l=fb_ramp_gain_l;
         *gain_ch_r=fb_ramp_gain_r;
     }
#if defined(AUDIO_ANC_TT_HW)
    else if(anc_type==ANC_TALKTHRU)
    {
        //Set the TT gain;
         *gain_ch_l=tt_ramp_gain_l;
         *gain_ch_r=tt_ramp_gain_r;
     }
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    else if(anc_type==ANC_MUSICCANCLE)
    {
        //Set the MC gain;
         *gain_ch_l=mc_ramp_gain_l;
         *gain_ch_r=mc_ramp_gain_r;
     }
#endif
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
         *gain_ch_l=anc_ff_gain->codec_anc_mute_gain_ff_ch0;
         *gain_ch_r=anc_ff_gain->codec_anc_mute_gain_ff_ch1;
     }
    else if(anc_type==ANC_FEEDBACK)
    {
        //Get the FB gain;
         *gain_ch_l=anc_fb_gain->codec_anc_mute_gain_fb_ch0;
         *gain_ch_r=anc_fb_gain->codec_anc_mute_gain_fb_ch1;
     }
#if defined(AUDIO_ANC_TT_HW)
    else if(anc_type==ANC_TALKTHRU)
    {
        //Set the TT gain;
         *gain_ch_l=anc_tt_gain->codec_mute_gain_coef_tt_ch0;
         *gain_ch_r=anc_tt_gain->codec_mute_gain_coef_tt_ch1;
     }
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    else if(anc_type==ANC_MUSICCANCLE)
    {
        //Set the MC gain;
         *gain_ch_l=anc_mc_gain->codec_mute_gain_coef_mc_ch0;
         *gain_ch_r=anc_mc_gain->codec_mute_gain_coef_mc_ch1;
     }
#endif
    else
    {
         //if err ANC type,retun 0;
         *gain_ch_l=0;
         *gain_ch_r=0;
         err=ANC_TYPE_ERR;
        LOG_I("Error ANC type:%d", anc_type);
    }
#endif
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
         *gain_ch_l = max_ff_gain_l;
         *gain_ch_r = max_ff_gain_r;
    }
    else if(anc_type==ANC_FEEDBACK)
    {
        //Get the max FB gain;
         *gain_ch_l = max_fb_gain_l;
         *gain_ch_r = max_fb_gain_r;
    }
#if defined(AUDIO_ANC_TT_HW)
    else if(anc_type==ANC_TALKTHRU)
    {
        //Get the TT gain;
         *gain_ch_l=max_tt_gain_l;
         *gain_ch_r=max_tt_gain_r;
    }
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    else if(anc_type==ANC_MUSICCANCLE)
    {
        //Get the MC gain;
         *gain_ch_l=max_mc_gain_l;
         *gain_ch_r=max_mc_gain_r;
    }
#endif
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
    anc_output_ch_map=ch_map;
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
        break;

        case ANC_ADC_ONLY_MC:
        fb_check_ch0_config->codec_fb_check_keep_ch0=1;
        feedback_mc_settings->codec_feedback_mc_en_ch0=0;
        break;

        case ANC_ADC_ADC_ADD_MC:
        fb_check_ch0_config->codec_fb_check_keep_ch0=1;
        feedback_mc_settings->codec_feedback_mc_en_ch0=1;
        break;

        default:
        fb_check_ch0_config->codec_fb_check_keep_ch0=0;
        feedback_mc_settings->codec_feedback_mc_en_ch0=1;
        break;
     }
#endif
    return 0;
}

