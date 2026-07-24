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
** namer：ANC filter
** description：IIR and FIR filter
** version：V1.0
** author： xuml
** modify：2017.7.11.
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
#include "anc_process.h"
#include "hwtimer_list.h"

/**********************************mocro**************************************/

typedef short int16;
typedef unsigned short uint16;
typedef int   int32;
typedef unsigned int   uint32;

/*
3	60	en_clk_adc_ana	RW	0	0	gate enable of clk_adc_anc
5	60	en_clk_adc	RW	0	3	gate enable of clk_adc
1	60	en_clk_dac	RW	0	8	gate enable of clk_dac
2	60	en_clk_iir	RW	0	9
2	60	en_clk_rs	RW	0	11
3	60	pol_adc_ana	RW	7	13	inv of clk_adc_ana
1	60	pol_dac_out	RW	1	16	inv of clk_dac_out
4	60	cfg_clk_out	RW	0	17	Debug clock out
21	60		x

*/
struct _anc_iir_clock
{
    uint32 en_clk_adc_ana : 3;
    uint32 en_clk_adc : 5;
    uint32 en_clk_dac : 1;

    uint32 en_clk_iir : 2;
    uint32 en_clk_rs : 2;

    uint32 pol_adc_ana : 3;
    uint32 pol_dac_out : 1;
    uint32 cfg_clk_out : 4;

    uint32 reserved : 11;
};


/*
1	244	codec_iir0_enable	RW	0	0	anc iir module enable
1	244	codec_iir0_iira_enable	RW	0	1
1	244	codec_iir0_iirb_enable	RW	0	2
1	244	codec_iir0_ch0_bypass	RW	0	3	1: ch0 bypass
1	244	codec_iir0_ch1_bypass	RW	0	4
1	244	codec_iir0_gaincal_ext_ch0_bypass	RW	1	5
1	244	codec_iir0_gaincal_ext_ch1_bypass	RW	1	6
1	244	codec_iir0_gainuse_ext_ch0_bypass	RW	1	7
1	244	codec_iir0_gainuse_ext_ch1_bypass	RW	1	8
1	244	codec_iir0_lmt_ch0_bypass	RW	1	9
1	244	codec_iir0_lmt_ch1_bypass	RW	1	10
5	244	codec_iir0_count_ch0	RW	0	11	ch0 iir number  0~8
5	244	codec_iir0_count_ch1	RW	0	16
1	244	codec_iir0_coef_swap	RW	0	21	0: use iir coef memory0;  1: use iir coef memory1;
1	244	codec_iir0_auto_stop	RW	0	22	read only;  coef memoryX used currently
1	244	codec_iir0_coef_swap_status_sync[1]	R	0	23	read only;  coef memoryX used currently
1	244	codec_iir0_iira_stop_status_sync[1]	R	0	24
1	244	codec_iir0_iirb_stop_status_sync[1]	R	0	25
26	244		x
*/
struct _anc_iir0_control
{
    uint32 codec_iir0_enable : 1;

    uint32 codec_iir0_iira_enable : 1;
    uint32 codec_iir0_iirb_enable : 1;

    uint32 codec_iir0_ch0_bypass : 1;
    uint32 codec_iir0_ch1_bypass : 1;

    uint32 codec_iir0_gaincal_ext_ch0_bypass : 1;
    uint32 codec_iir0_gaincal_ext_ch1_bypass : 1;

    uint32 codec_iir0_gainuse_ext_ch0_bypass : 1;
    uint32 codec_iir0_gainuse_ext_ch1_bypass : 1;

    uint32 codec_iir0_lmt_ch0_bypass : 1;
    uint32 codec_iir0_lmt_ch1_bypass : 1;

    uint32 codec_iir0_count_ch0 : 5;
    uint32 codec_iir0_count_ch1 : 5;

    uint32 codec_iir0_coef_swap : 1;
    uint32 codec_iir0_auto_stop : 1;

    uint32 codec_iir0_coef_swap_status_sync : 1;
    uint32 codec_iir0_iira_stop_status_sync : 1;
    uint32 codec_iir0_iirb_stop_status_sync : 1;

    uint32 reserved : 6;
};


/*
1	24c	codec_iir2_enable	RW	0	0	anc iir module enable
1	24c	codec_iir2_iira_enable	RW	0	1
1	24c	codec_iir2_iirb_enable	RW	0	2
1	24c	codec_iir2_ch0_bypass	RW	0	3	1: ch0 bypass
1	24c	codec_iir2_ch1_bypass	RW	0	4
1	24c	codec_iir2_gaincal_ext_ch0_bypass	RW	1	5
1	24c	codec_iir2_gaincal_ext_ch1_bypass	RW	1	6
1	24c	codec_iir2_gainuse_ext_ch0_bypass	RW	1	7
1	24c	codec_iir2_gainuse_ext_ch1_bypass	RW	1	8
1	24c	codec_iir2_lmt_ch0_bypass	RW	1	9
1	24c	codec_iir2_lmt_ch1_bypass	RW	1	10
1	24c	codec_iir2_ch01_mix	RW	0	11
5	24c	codec_iir2_count_ch0	RW	0	12	ch0 iir number  0~8
5	24c	codec_iir2_count_ch1	RW	0	17
1	24c	codec_iir2_coef_swap	RW	0	22	0: use iir coef memory0;  1: use iir coef memory1;
1	24c	codec_iir2_auto_stop	RW	0	23	read only;  coef memoryX used currently
1	24c	codec_iir2_coef_swap_status_sync[1]	R	0	24	read only;  coef memoryX used currently
1	24c	codec_iir2_iira_stop_status_sync[1]	R	0	25
1	24c	codec_iir2_iirb_stop_status_sync[1]	R	0	26
27	24c		x

*/
struct _anc_iir2_control
{
    uint32 codec_iir2_enable : 1;

    uint32 codec_iir2_iira_enable : 1;
    uint32 codec_iir2_iirb_enable : 1;

    uint32 codec_iir2_ch0_bypass : 1;
    uint32 codec_iir2_ch1_bypass : 1;

    uint32 codec_iir2_gaincal_ext_ch0_bypass : 1;
    uint32 codec_iir2_gaincal_ext_ch1_bypass : 1;

    uint32 codec_iir2_gainuse_ext_ch0_bypass : 1;
    uint32 codec_iir2_gainuse_ext_ch1_bypass : 1;

    uint32 codec_iir2_lmt_ch0_bypass : 1;
    uint32 codec_iir2_lmt_ch1_bypass : 1;

    uint32 codec_iir2_ch01_mix : 1;

    uint32 codec_iir2_count_ch0 : 5;
    uint32 codec_iir2_count_ch1 : 5;

    uint32 codec_iir2_coef_swap : 1;
    uint32 codec_iir2_auto_stop : 1;

    uint32 codec_iir2_coef_swap_status_sync : 1;
    uint32 codec_iir2_iira_stop_status_sync : 1;
    uint32 codec_iir2_iirb_stop_status_sync : 1;

    uint32 reserved : 5;
};


/*
1	254	codec_iir3_enable	RW	0	0	anc iir module enable
1	254	codec_iir3_iira_enable	RW	0	1
1	254	codec_iir3_iirb_enable	RW	0	2
1	254	codec_iir3_ch0_bypass	RW	0	3	1: ch0 bypass
1	254	codec_iir3_ch1_bypass	RW	0	4
1	254	codec_iir3_gaincal_ext_ch0_bypass	RW	1	5
1	254	codec_iir3_gaincal_ext_ch1_bypass	RW	1	6
1	254	codec_iir3_gainuse_ext_ch0_bypass	RW	1	7
1	254	codec_iir3_gainuse_ext_ch1_bypass	RW	1	8
1	254	codec_iir3_lmt_ch0_bypass	RW	1	9
1	254	codec_iir3_lmt_ch1_bypass	RW	1	10
1	254	codec_iir3_ch01_mix	RW	0	11
5	254	codec_iir3_count_ch0	RW	0	12	ch0 iir number  0~8
5	254	codec_iir3_count_ch1	RW	0	17
1	254	codec_iir3_coef_swap	RW	0	22	0: use iir coef memory0;  1: use iir coef memory1;
1	254	codec_iir3_auto_stop	RW	0	23	read only;  coef memoryX used currently
1	254	codec_iir3_coef_swap_status_sync[1]	R	0	24	read only;  coef memoryX used currently
1	254	codec_iir3_iira_stop_status_sync[1]	R	0	25
1	254	codec_iir3_iirb_stop_status_sync[1]	R	0	26
27	254		x
*/
struct _anc_iir3_control
{
    uint32 codec_iir3_enable : 1;

    uint32 codec_iir3_iira_enable : 1;
    uint32 codec_iir3_iirb_enable : 1;

    uint32 codec_iir3_ch0_bypass : 1;
    uint32 codec_iir3_ch1_bypass : 1;

    uint32 codec_iir3_gaincal_ext_ch0_bypass : 1;
    uint32 codec_iir3_gaincal_ext_ch1_bypass : 1;

    uint32 codec_iir3_gainuse_ext_ch0_bypass : 1;
    uint32 codec_iir3_gainuse_ext_ch1_bypass : 1;

    uint32 codec_iir3_ch01_mix : 1;

    uint32 codec_iir3_lmt_ch0_bypass : 1;
    uint32 codec_iir3_lmt_ch1_bypass : 1;

    uint32 codec_iir3_count_ch0 : 5;
    uint32 codec_iir3_count_ch1 : 5;

    uint32 codec_iir3_coef_swap : 1;
    uint32 codec_iir3_auto_stop : 1;

    uint32 codec_iir3_coef_swap_status_sync : 1;
    uint32 codec_iir3_iira_stop_status_sync : 1;
    uint32 codec_iir3_iirb_stop_status_sync : 1;

    uint32 reserved : 5;
};

/*
1	25c	codec_iir0_gain_ext_update_ch0	RW	0	0
1	25c	codec_iir0_gain_ext_update_ch1	RW	0	1
1	25c	codec_iir0_gain_ext_sel_ch0	RW	0	2
1	25c	codec_iir0_gain_ext_sel_ch1	RW	0	3
1	25c	codec_iir2_gain_ext_update_ch0	RW	0	4
1	25c	codec_iir2_gain_ext_update_ch1	RW	0	5
1	25c	codec_iir2_gain_ext_sel_ch0	RW	0	6
1	25c	codec_iir2_gain_ext_sel_ch1	RW	0	7
1	25c	codec_deq_iir_gain_ext_update_ch0	RW	0	8
1	25c	codec_deq_iir_gain_ext_sel_ch0	RW	0	9
1	25c	codec_iir0_lmt_th_update_ch0	RW	0	10
1	25c	codec_iir0_lmt_th_update_ch1	RW	0	11
1	25c	codec_iir2_lmt_th_update_ch0	RW	0	12
1	25c	codec_iir2_lmt_th_update_ch1	RW	0	13
14	25c		x
*/
struct _anc_iir_gain_update
{
    uint32 codec_iir0_gain_ext_update : 2;
    uint32 codec_iir0_gain_ext_sel_ch0 : 1;
    uint32 codec_iir0_gain_ext_sel_ch1 : 1;
    uint32 codec_iir2_gain_ext_update: 2;
    uint32 codec_iir2_gain_ext_sel_ch0 : 1;
    uint32 codec_iir2_gain_ext_sel_ch1 : 1;
    uint32 codec_deq_iir_gain_ext_update_ch0 : 1;
    uint32 codec_deq_iir_gain_ext_sel_ch0 : 1;
    uint32 codec_iir0_lmt_th_update_ch0 : 1;
    uint32 codec_iir0_lmt_th_update_ch1 : 1;
    uint32 codec_iir2_lmt_th_update_ch0 : 1;
    uint32 codec_iir2_lmt_th_update_ch1 : 1;

    uint32 Reserved : 18;
};

/*
7	300	codec_iir0_lmt_delay_ch0	RW	0	0
7	300	codec_iir2_lmt_delay_ch0	RW	0	7
14	300		x
*/

struct _anc_iir_lmt_delay
{
    uint32 codec_iir0_lmt_delay : 7;
    uint32 codec_iir2_lmt_delay : 7;

    uint32 Reserved : 18;
};

/*
1	d0	codec_anc_enable_ch0	RW	0	0	anc enable
1	d0	codec_dual_anc_ch0	RW	0	1	FF + FB
1	d0	codec_anc_mute_ch0	RW	0	2	anc output to 0
1	d0	codec_anc_rate_sel	RW	0	3	  0: 384k anc;  1: 768k anc;
2	d0	codec_anc_ff_sr_sel	RW	0	4	  0: 384k;  1: 192k;  2: 96k;  3:48k;
3	d0	codec_anc_ff_in_phase_sel	RW	0	6	  8 phase
2	d0	codec_anc_fb_sr_sel	RW	0	9	  0: 384k;  1: 192k;  2: 96k;  3:48k;
3	d0	codec_anc_fb_in_phase_sel	RW	0	11	  8 phase
1	d0	codec_feedback_ch0	RW	0	14	feedback mode
1	d0	codec_anc_music_ch_sel	RW	0	15
16	d0		x

*/
struct _anc_control
{
    uint32 codec_anc_enable_ch0 : 1;
    uint32 codec_dual_anc_ch0 : 1;
    uint32 codec_anc_mute_ch0 : 1;
    uint32 codec_anc_rate_sel : 1;
    uint32 codec_anc_ff_sr_sel : 2;
    uint32 codec_anc_ff_in_phase_sel : 3;
    uint32 codec_anc_fb_sr_sel : 2;
    uint32 codec_anc_fb_in_phase_sel : 3;

    uint32 codec_feedback_ch0 : 1;

    uint32 codec_anc_music_ch_sel : 1;


    uint32 Reserved : 16;
};




/*
12	d4	codec_anc_mute_gain_ff_ch0	RW	0	0	Format 3.9, for FF
1	d4	codec_anc_mute_gain_pass0_ff_ch0	RW	0	12	1: gain update no pass0
1	d4	codec_anc_mute_gain_update_ff_ch0	RW	0	13
14	d4		x
*/
struct _anc_ff_gain
{
	int32 codec_anc_mute_gain_ff_ch0 : 12;
	int32 codec_anc_mute_gain_pass0_ff_ch0 : 1;
	int32 codec_anc_mute_gain_update_ff_ch0 : 1;

	uint32 Reserved : 18;
};

/*
12	d8	codec_anc_mute_gain_fb_ch0	RW	0	0	same as above
1	d8	codec_anc_mute_gain_pass0_fb_ch0	RW	0	12
1	d8	codec_anc_mute_gain_update_fb_ch0	RW	0	13
14	d8		x
*/
struct _anc_fb_gain
{
	int32 codec_anc_mute_gain_fb_ch0 : 12;
	int32 codec_anc_mute_gain_pass0_fb_ch0 : 1;
	int32 codec_anc_mute_gain_update_fb_ch0 : 1;

	uint32 Reserved : 18;
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
16	238	codec_anc_calib_gain_coef_ff_ch0	RW	4096	0
16	238		x
*/

struct _anc_ff_iir_calib_gain
{
	uint32 codec_anc_calib_gain_coef_ff_ch0 : 16;
	uint32 Reserved : 16;
};

/*
16	23c	codec_anc_calib_gain_coef_fb_ch0	RW	4096	0
16	23c		x
*/

struct _anc_fb_iir_calib_gain
{
	uint32 codec_anc_calib_gain_coef_fb_ch0 : 16;
	uint32 Reserved : 16;
};

/*
16	378	codec_calib_gain_coef_tt_ch0 	RW	0	0
16	378		x
*/
struct _anc_tt_iir_calib_gain
{
	uint32 codec_anc_calib_gain_coef_tt_ch0 : 16;
	uint32 Reserved : 16;
};

/*
1	374	codec_music_mix_off_ch0	RW	0	0
1	374	codec_pdu_off_ch0	RW	0	1
1	374	codec_pdu_mix_en_ch0	RW	0	2
1	374	codec_tt_out_off_ch0            	RW	0	3
1	374	codec_tt_pdu_off_ch0       	RW	1	4
5	374		x

*/
struct _anc_path_set
{
    uint32 codec_music_mix_off_ch0 : 1;
    uint32 codec_pdu_off_ch0 : 1;
    uint32 codec_pdu_mix_en_ch0 : 1;

    uint32 codec_tt_out_off_ch0 : 1;
    uint32 codec_tt_pdu_off_ch0	 : 1;

    uint32 Reserved : 27;
};


/*
1	240	codec_anc_calib_gain_pass0_ff_ch0	RW	0	0	Format 3.9, for FF
1	240	codec_anc_calib_gain_update_ff_ch0	RW	0	1	1: gain update no pass0
1	240	codec_anc_calib_gain_pass0_fb_ch0	RW	0	2
1	240	codec_anc_calib_gain_update_fb_ch0	RW	0	3
1	240	codec_calib_gain_pass0_tt_ch0	RW	0	4
1	240	codec_calib_gain_update_tt_ch0	RW	0	5
6	240		x
*/
struct _anc_iir_calib_gain_config
{
    uint32 codec_anc_calib_gain_pass0_ff_ch0 : 1;
    uint32 codec_anc_calib_gain_update_ff_ch0 : 1;
    uint32 codec_anc_calib_gain_pass0_fb_ch0 : 1;
    uint32 codec_anc_calib_gain_update_fb_ch0 : 1;
    uint32 codec_anc_calib_gain_pass0_tt_ch0 : 1;
    uint32 codec_anc_calib_gain_update_tt_ch0 : 1;

    uint32 Reserved : 26;
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
	int32 a1 ;
	int32 a2 ;

	int32 b1 ;
	int32 b2 ;
	int32 b0 ;
};


/*

3	104	fir_access_offset_ch0	RW	0	0
3	104	fir_access_offset_ch2	RW	0	3
1	104	anc_coef_sel_pdu0_fs0	RW	0	6
1	104	anc_coef_sel_pdu0_fs0_new	R	0	7
8	104		x


*/

struct _fir_config
{
	uint32 fir_access_offset_ch0 : 3;
	uint32 fir_access_offset_ch2 : 3;
	uint32 anc_coef_sel_pdu0_fs0 : 1;
	uint32 anc_coef_sel_pdu0_fs0_new : 1;

	uint32 Reserved : 24;
};




/*
1	100	fir_stream_enable_ch0	RW	0	0	1: used for anc fir or adc downsample fir
1	100	fir_stream_enable_ch2	RW	0	1
1	100	fir_enable_ch0	RW	0	2	1: used for memory to memory fir
1	100	fir_enable_ch2	RW	0	3
1	100	dmactrl_rx_fir	RW	0	4
1	100	dmactrl_tx_fir	RW	0	5
1	100	fir_upsample_ch0	RW	0	6	1: use for memory to memory fir upsample
1	100	fir_upsample_ch2	RW	0	7
1	100	mode_32bit_fir	RW	0	8	 all coef/sample to memory; not used together with any 16 bit mode
1	100	fir_reserved_reg0	RW	0	9	not used
1	100	mode_16bit_fir_tx_ch0	RW	0	10	 sample memory write as 16bit mode
1	100	mode_16bit_fir_rx_ch0	RW	0	11	 coef memory read as 16bit mode
1	100	mode_16bit_fir_tx_ch2	RW	0	12
1	100	mode_16bit_fir_rx_ch2	RW	0	13
14	100		x


*/
struct _fir_control
{
	uint32 fir_stream_enable_ch0 : 1;
	uint32 fir_stream_enable_ch2 : 1;

	uint32 Reserved : 30;
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
1	bc	codec_feedback_mc_en_ch0	RW	0	1	feedback music cancel enable
1	bc	codec_dac_L_iir_enable	RW	0	2
1	bc	codec_adc_ch0_iir_enable	RW	0	3
1	bc	codec_adc_ch1_iir_enable	RW	0	4
1	bc	codec_adc_ch2_iir_enable	RW	0	5
1	bc	codec_adc_ch3_iir_enable	RW	0	6
4	bc	codec_fb_check_udc_ch0	RW	0	7
11	bc		x


*/
struct _feedback_mc_settings
{
    uint32 codec_adc_mc_en_ch0 : 1;
    uint32 codec_feedback_mc_en_ch0 : 1;
    uint32 codec_dac_L_iir_enable : 1;
    uint32 codec_adc_ch0_iir_enable : 1;
    uint32 codec_adc_ch1_iir_enable : 1;
    uint32 codec_adc_ch2_iir_enable : 1;
    uint32 codec_adc_ch3_iir_enable : 1;
    uint32 codec_fb_check_udc_ch0 : 4;

    uint32 Reserved1 : 21;
};

#if defined(AUDIO_ANC_FB_MC)||defined(ANC_FB_CHECK)||defined(ANC_FF_CHECK)||defined(AUDIO_ANC_FB_MC_HW)||defined(ANC_TT_CHECK)
/*
1	130	codec_fb_check_enable_ch0	RW	0	0
2	130	codec_fb_check_acc_sample_rate_ch0	RW	0	1
2	130	codec_fb_check_src_sel_ch0	RW	0	3	00: feedback music cancel; 01: feedback; 10: feedfoward
1	130	codec_fb_check_keep_sel_ch0	RW	0	5
12	130	codec_fb_check_acc_window_ch0	RW	0	6
10	130	codec_fb_check_trig_window_ch0	RW	0	18
1	130	codec_fb_check_keep_ch0	RW	0	28
1	130	codec_fb_check_dcf_bypass_ch0	RW	0	29
30	130		x


*/
struct _fb_check_ch0_config
{
	uint32 codec_fb_check_enable_ch0 : 1;
	uint32 codec_fb_check_acc_sample_rate_ch0 : 2;
	uint32 codec_fb_check_src_sel_ch0 : 2;
	uint32 codec_fb_check_keep_sel_ch0: 1;
	uint32 codec_fb_check_acc_window_ch0 : 12;
	uint32 codec_fb_check_trig_window_ch0 : 10;
	uint32 codec_fb_check_keep_ch0:1;
	uint32 codec_fb_check_dcf_bypass_ch0 : 1;
	uint32 Reserved : 2;
};



/*
1	134	codec_fb_check_enable_ch1	RW	0	0
2	134	codec_fb_check_acc_sample_rate_ch1	RW	0	1
2	134	codec_fb_check_src_sel_ch1	RW	0	3
1	134	codec_fb_check_keep_sel_ch1	RW	0	5
12	134	codec_fb_check_acc_window_ch1	RW	0	6
10	134	codec_fb_check_trig_window_ch1	RW	0	18
1	134	codec_fb_check_keep_ch1	RW	0	28
1	134	codec_fb_check_dcf_bypass_ch1	RW	0	29
30	134		x
*/

struct _fb_check_ch1_config
{
	uint32 codec_fb_check_enable_ch1 : 1;

	uint32 codec_fb_check_acc_sample_rate_ch1 : 2;

	uint32 codec_fb_check_src_sel_ch1 : 2;

	uint32 codec_fb_check_keep_sel_ch1: 1;

	uint32 codec_fb_check_acc_window_ch1 : 12;

	uint32 codec_fb_check_trig_window_ch1 : 10;

	uint32 codec_fb_check_keep_ch1 : 1;

	uint32 codec_fb_check_dcf_bypass_ch1: 1;

	uint32 Reserved : 2;
};
#endif

#if defined(ANC_FB_CHECK)||defined(ANC_FF_CHECK)||defined(ANC_TT_CHECK)

/*
5	0c	codec_rx_overflow 	RW	0	0	interrupt status which not been masked.
5	0c	codec_rx_underflow	RW	0	5
1	0c	codec_tx_overflow	RW	0	10
1	0c	codec_tx_underflow	RW	0	11
1	0c	codec_tx_overflow_snd	RW	0	12
1	0c	codec_tx_underflow_snd	RW	0	13
1	0c	mc_overflow	RW	0	14
1	0c	mc_underflow	RW	0	15
1	0c	event_trigger	RW	0	16
1	0c	fb_check_error_trig_ch0	RW	0	17
1	0c	bt_trigger	RW	0	18
1	0c	adc_max_overflow	RW	0	19
1	0c	time_trigger	RW	0	20
21	0c		x


*/
struct _codec_int_config
{
	uint32 Reserved_notused : 17;

	uint32 fb_check_error_trig_ch0 : 1;

       uint32 Reserved : 14;
};
/*
5	10	codec_rx_overflow_msk	RW	0	0	interrupt mask; 0 will mask interrupt
5	10	codec_rx_underflow_msk	RW	0	5
1	10	codec_tx_overflow_msk	RW	0	10
1	10	codec_tx_underflow_msk	RW	0	11
1	10	codec_tx_overflow_snd_msk	RW	0	12
1	10	codec_tx_underflow_snd_msk	RW	0	13
1	10	mc_overflow_msk	RW	0	14
1	10	mc_underflow_msk	RW	0	15
1	10	event_trigger_msk	RW	0	16
1	10	fb_check_error_trig_ch0_msk	RW	0	17
1	10	bt_trigger_msk	RW	0	18
1	10	adc_max_overflow_msk	RW	0	19
1	10	time_trigger_msk	RW	0	20
21	10		x
*/
struct _codec_mask_config
{
	uint32 Reserved_notused : 17;

	uint32 fb_check_error_trig_ch0_mask : 1;

       uint32 Reserved : 14;
};
#endif


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
1	2b8	tm_iir_coef_swap_status_sync[1]	R	0	22	read only;  coef memoryX used currently	　	　	　
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
1	22c	codec_tt_enable_ch0	RW	0	0	tws+anc: talk through enable
3	22c	codec_tt_adc_sel_ch0	RW	0	1
1	22c	codec_mm_enable_ch0	RW	0	4	tws+anc: music cancel enalbe
1	22c	codec_mm_fifo_en_ch0	RW	0	5	tws+anc: music cancel fifo enable
1	22c	codec_mm_fifo_bypass_ch0	RW	0	6	music cancel fifo bypass
5	22c	codec_mm_delay_ch0	RW	0	7	music cancel delay
12	22c		x
*/
struct _tm_config
{
    uint32 codec_tt_enable_ch0 : 1;
    uint32 codec_tt_adc_sel_ch0 : 3;
    uint32 codec_mm_enable_ch0 : 1;
    uint32 codec_mm_fifo_en_ch0 : 1;
    uint32 codec_mm_fifo_bypass_ch0 : 1;
    uint32 codec_mm_delay_ch0 : 5;

    uint32 Reserved : 20;
};


/*
12	230	codec_mute_gain_coef_tt_ch0	RW	0	0	talk through gain
1	230	codec_mute_gain_pass0_tt_ch0	RW	0	12	talk through gain pass 0 effect
1	230	codec_mute_gain_update_tt_ch0	RW	0	13
14	230		x

*/

struct _anc_tt_gain
{
	int32 codec_anc_mute_gain_tt_ch0 : 12;
	int32 codec_anc_mute_gain_pass0_tt_ch0 : 1;
	int32 codec_anc_mute_gain_update_tt_ch0 : 1;

	uint32 Reserved : 18;
};

/*
12	234	codec_mute_gain_coef_mm_ch0	RW	0	0	music cancel gain
1	234	codec_mute_gain_pass0_mm_ch0	RW	0	12	music cancel gain pass 0
1	234	codec_mute_gain_update_mm_ch0	RW	0	13
14	234		x
*/
struct _anc_mc_gain
{
	int32 codec_anc_mute_gain_mc_ch0 : 12;
	int32 codec_anc_mute_gain_pass0_mc_ch0 : 1;
	int32 codec_anc_mute_gain_update_mc_ch0 : 1;

	uint32 Reserved : 18;
};


#if defined(ANC_FB_CHECK) && defined(ANC_FF_CHECK)
#error "ANC_FB_CHECK and ANC_FF_CHECK can not defined simultaneously "
#endif

#if defined(ANC_FB_CHECK) && defined(ANC_TT_CHECK)
#error "ANC_FB_CHECK and ANC_TT_CHECK can not defined simultaneously "
#endif

#if defined(ANC_TT_CHECK) && defined(ANC_FF_CHECK)
#error "ANC_TT_CHECK and ANC_FF_CHECK can not defined simultaneously "
#endif

#define IIR_COUNTER (8)

#if defined(AUDIO_ANC_TT_HW)
#define IIR_FF_COUNTER (7)
#define IIR_TT_COUNTER (6)
#else
#define IIR_FF_COUNTER (8)
#define IIR_TT_COUNTER (1)
#endif

#if defined(AUDIO_ANC_FB_MC_HW)
#define IIR_FB_COUNTER (7)
#define IIR_MC_COUNTER (6)
#else
#define IIR_FB_COUNTER (7)
#define IIR_MC_COUNTER (1)
#endif


#define ANC_AUD_OUTPUT_PATH_SPEAKER_DEV  (AUD_CHANNEL_MAP_CH0)
//#define ANC_AUD_OUTPUT_PATH_SPEAKER_DEV  (AUD_CHANNEL_MAP_CH0|AUD_CHANNEL_MAP_CH1)

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

#define CALIB_GAIN_Q (1<<12)

#define FIXED_COEF_Q (1<<27)

#define ANC_GAIN_RAMP_THRESHLD (40)

#define ANC_GAIN_RAMP_ZERO (10)

#define ANC_GAIN_RAMP_BURST_THRESHLD (ANC_GAIN_RAMP_THRESHLD*10000)

#define ANC_FAST_CALIB_MODE

#ifdef ANC_GAIN_RAMP
#define FIXED_GAIN_RAMP_Q (1<<27)


/*
Type='Low Passing';
Freq=240;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
static const anc_iir_coefs   iir_coef_gain_ramp_delay1=
{
    .coef_b={512,         1025,          512},
    .coef_a={134217728,   -265823675,    131607997},
};

#define ANC_SET_GAIN_TIME_DELAY1  ((MS_TO_TICKS(50)))


/*
Type='Low Passing';
Freq=120;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
static const anc_iir_coefs   iir_coef_gain_ramp_delay2=
{
    .coef_b={129,          257,          129},
    .coef_a={134217728,   -267123699,    132906486},
};

#define ANC_SET_GAIN_TIME_DELAY2  ((MS_TO_TICKS(100)))


/*
Type='Low Passing';
Freq=60;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
static const anc_iir_coefs   iir_coef_gain_ramp_delay3=
{
    .coef_b={32,           65,           32},
    .coef_a={134217728,   -267778100,    133560501},
};

#define ANC_SET_GAIN_TIME_DELAY3  ((MS_TO_TICKS(200)))


/*
Type='Low Passing';
Freq=30;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs   iir_coef_gain_ramp_delay4=
{
    .coef_b={            8,           16,            8},
    .coef_a={134217728,   -268106408,    133888712},
};

#define ANC_SET_GAIN_TIME_DELAY4  ((MS_TO_TICKS(450)))



/*
Type='Low Passing';
Freq=1000;
Gain=0;
Q=0.004;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs   iir_coef_gain_ramp_delay5=
{
    .coef_b={            2950,         5900,         2950},
    .coef_a={134217728,    -88138077,    -46067851},
};

#define ANC_SET_GAIN_TIME_DELAY5  ((MS_TO_TICKS(650)))


/*
Type='Low Passing';
Freq=1000;
Gain=0;
Q=0.003;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs   iir_coef_gain_ramp_delay6=
{
    .coef_b={            2410,         4821,         2410},
    .coef_a={134217728,    -72015762,    -62192325},
};

#define ANC_SET_GAIN_TIME_DELAY6  ((MS_TO_TICKS(866)))

/*
Type='Low Passing';
Freq=1000;
Gain=0;
Q=0.002;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs   iir_coef_gain_ramp_delay7=
{
    .coef_b={            1765,         3529,         1765},
    .coef_a={134217728,    -52726267,    -81484402},
};

#define ANC_SET_GAIN_TIME_DELAY7  ((MS_TO_TICKS(1300)))



/*
Type='Low Passing';
Freq=1000;
Gain=0;
Q=0.001;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs   iir_coef_gain_ramp_delay8=
{
    .coef_b={            978,         1957,          978},
    .coef_a={134217728,    -29234663,   -104979151},
};

#define ANC_SET_GAIN_TIME_DELAY8  ((MS_TO_TICKS(2600)))

#define ANC_GAIN_RAMP_DELAY   (4)  // 1.5s(4)


#if ANC_GAIN_RAMP_DELAY==1

/*
Type='Low Passing';
Freq=240;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
static const anc_iir_coefs   iir_coef_gain_ramp_ff_normal=
{
    .coef_b={512,         1025,          512},
    .coef_a={134217728,   -265823675,    131607997},
};

static const anc_iir_coefs   iir_coef_gain_ramp_fb_normal=
{
    .coef_b={512,         1025,          512},
    .coef_a={134217728,   -265823675,    131607997},
};
static const anc_iir_coefs   iir_coef_gain_ramp_tt_normal=
{
    .coef_b={512,         1025,          512},
    .coef_a={134217728,   -265823675,    131607997},
};
static const anc_iir_coefs   iir_coef_gain_ramp_mc_normal=
{
    .coef_b={512,         1025,          512},
    .coef_a={134217728,   -265823675,    131607997},
};

#define ANC_SET_GAIN_TIME  ((MS_TO_TICKS(50)))

#elif ANC_GAIN_RAMP_DELAY==2
/*

Type='Low Passing';
Freq=120;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
static const anc_iir_coefs   iir_coef_gain_ramp_ff_normal=
{
    .coef_b={129,          257,          129},
    .coef_a={134217728,   -267123699,    132906486},
};

static const anc_iir_coefs   iir_coef_gain_ramp_fb_normal=
{
    .coef_b={129,          257,          129},
    .coef_a={134217728,   -267123699,    132906486},
};
static const anc_iir_coefs   iir_coef_gain_ramp_tt_normal=
{
    .coef_b={129,          257,          129},
    .coef_a={134217728,   -267123699,    132906486},
};
static const anc_iir_coefs   iir_coef_gain_ramp_mc_normal=
{
    .coef_b={129,          257,          129},
    .coef_a={134217728,   -267123699,    132906486},
};

#define ANC_SET_GAIN_TIME  ((MS_TO_TICKS(100)))
#elif ANC_GAIN_RAMP_DELAY==3

/*
Type='Low Passing';
Freq=60;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
static const anc_iir_coefs   iir_coef_gain_ramp_ff_normal=
{
    .coef_b={32,           65,           32},
    .coef_a={134217728,   -267778100,    133560501},
};

static const anc_iir_coefs   iir_coef_gain_ramp_fb_normal=
{
    .coef_b={32,           65,           32},
    .coef_a={134217728,   -267778100,    133560501},
};
static const anc_iir_coefs   iir_coef_gain_ramp_tt_normal=
{
    .coef_b={32,           65,           32},
    .coef_a={134217728,   -267778100,    133560501},
};
static const anc_iir_coefs   iir_coef_gain_ramp_mc_normal=
{
    .coef_b={32,           65,           32},
    .coef_a={134217728,   -267778100,    133560501},
};

#define ANC_SET_GAIN_TIME  ((MS_TO_TICKS(150)))


#elif ANC_GAIN_RAMP_DELAY==4


/*
Type='Low Passing';
Freq=30;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs   iir_coef_gain_ramp_ff_normal=
{
    .coef_b={            8,           16,            8},
    .coef_a={134217728,   -268106408,    133888712},
};

static const anc_iir_coefs   iir_coef_gain_ramp_fb_normal=
{
    .coef_b={            8,           16,            8},
    .coef_a={134217728,   -268106408,    133888712},
};
static const anc_iir_coefs   iir_coef_gain_ramp_tt_normal=
{
    .coef_b={            8,           16,            8},
    .coef_a={134217728,   -268106408,    133888712},
};
static const anc_iir_coefs   iir_coef_gain_ramp_mc_normal=
{
    .coef_b={            8,           16,            8},
    .coef_a={134217728,   -268106408,    133888712},
};
#define ANC_SET_GAIN_TIME  ((MS_TO_TICKS(450)))

#elif ANC_GAIN_RAMP_DELAY==5


/*
Type='Low Passing';
Freq=1000;
Gain=0;
Q=0.001;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs   iir_coef_gain_ramp_ff_normal=
{
    .coef_b={            978,         1957,          978},
    .coef_a={134217728,    -29234663,   -104979151},
};

static const anc_iir_coefs   iir_coef_gain_ramp_fb_normal=
{
    .coef_b={            978,         1957,          978},
    .coef_a={134217728,    -29234663,   -104979151},
};
static const anc_iir_coefs   iir_coef_gain_ramp_tt_normal=
{
    .coef_b={            978,         1957,          978},
    .coef_a={134217728,    -29234663,   -104979151},
};
static const anc_iir_coefs   iir_coef_gain_ramp_mc_normal=
{
    .coef_b={            978,         1957,          978},
    .coef_a={134217728,    -29234663,   -104979151},
};
#define ANC_SET_GAIN_TIME  ((MS_TO_TICKS(2600)))

#endif

/*
Type='Low Passing';
Freq=240;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
#ifdef ANC_FAST_CALIB_MODE
static const anc_iir_coefs   iir_coef_gain_ramp_ff_calib=
{
    .coef_b={512,         1025,          512},
    .coef_a={134217728,   -265823675,    131607997},
};

static const anc_iir_coefs   iir_coef_gain_ramp_fb_calib=
{
    .coef_b={512,         1025,          512},
    .coef_a={134217728,   -265823675,    131607997},
};
static const anc_iir_coefs   iir_coef_gain_ramp_tt_calib=
{
    .coef_b={512,         1025,          512},
    .coef_a={134217728,   -265823675,    131607997},
};
static const anc_iir_coefs   iir_coef_gain_ramp_mc_calib=
{
    .coef_b={512,         1025,          512},
    .coef_a={134217728,   -265823675,    131607997},
};

#define ANC_SET_GAIN_CALIB_TIME  ((MS_TO_TICKS(50)))

#endif


#endif

#define ANC_LIMITER_FF
//#define ANC_LIMITER_TT
#define ANC_LIMITER_FB
//#define ANC_LIMITER_MC

#define ANC_LIMITER_FF_ATTACK_DELAY (1)
#define ANC_LIMITER_TT_ATTACK_DELAY (1)
#define ANC_LIMITER_FB_ATTACK_DELAY (1)
#define ANC_LIMITER_MC_ATTACK_DELAY (1)

//limiter threshold, 0dB: 0x7fffff
#define ANC_LIMITER_FF_THREHOLD (0x3FFFFF) //(0x7fffff)
#define ANC_LIMITER_TT_THREHOLD (0x3FFFFF) //(0x7fffff)
#define ANC_LIMITER_FB_THREHOLD (0x3FFFFF) //(0x7fffff)
#define ANC_LIMITER_MC_THREHOLD (0x3FFFFF) //(0x7fffff)

#define ANC_LIMITER_RELEASE_FF_TIME (10) // ms
#define ANC_LIMITER_RELEASE_TT_TIME (10) // ms
#define ANC_LIMITER_RELEASE_FB_TIME (10) // ms
#define ANC_LIMITER_RELEASE_MC_TIME (10) // ms

#define ANC_LIMITER_ATT_FF_TIME (13.42f) // ms
#define ANC_LIMITER_ATT_TT_TIME (13.42f) // ms
#define ANC_LIMITER_ATT_FB_TIME (1.34f)   // ms
#define ANC_LIMITER_ATT_MC_TIME (13.42f) // ms


#if defined(ANC_LIMITER_FF)

#define ANC_LIMITER_ATT_FF_B1 (134217728/(uint32)(ANC_LIMITER_ATT_FF_TIME*100))
#define ANC_LIMITER_ATT_FF_A1 (134217728 -ANC_LIMITER_ATT_FF_B1)

#define ANC_LIMITER_RELEASE_FF_B1 (134217728/(uint32)(ANC_LIMITER_RELEASE_FF_TIME*100))
#define ANC_LIMITER_RELEASE_FF_A1 (134217728-ANC_LIMITER_RELEASE_FF_B1)

static const anc_iir_coefs   iir_coef_limiter_attack_ff=
{
    .coef_b={0,    ANC_LIMITER_ATT_FF_B1,     0},
    .coef_a={134217728,    -ANC_LIMITER_ATT_FF_A1,     0},
};
static const anc_iir_coefs   iir_coef_limiter_release_ff=
{
    .coef_b={0,    ANC_LIMITER_RELEASE_FF_B1,     0},
    .coef_a={134217728,    -ANC_LIMITER_RELEASE_FF_A1,     0},
};
#endif

#if defined(ANC_LIMITER_TT) && defined(AUDIO_ANC_TT_HW)

#define ANC_LIMITER_ATT_TT_B1 (134217728/(uint32)(ANC_LIMITER_ATT_TT_TIME*100))
#define ANC_LIMITER_ATT_TT_A1 (134217728 -ANC_LIMITER_ATT_TT_B1)

#define ANC_LIMITER_RELEASE_TT_B1 (134217728/(uint32)(ANC_LIMITER_RELEASE_TT_TIME*100))
#define ANC_LIMITER_RELEASE_TT_A1 (134217728-ANC_LIMITER_RELEASE_TT_B1)

static const anc_iir_coefs   iir_coef_limiter_attack_tt=
{
    .coef_b={0,    ANC_LIMITER_ATT_TT_B1,     0},
    .coef_a={134217728,    -ANC_LIMITER_ATT_TT_A1,     0},
};
static const anc_iir_coefs   iir_coef_limiter_release_tt=
{
    .coef_b={0,    ANC_LIMITER_RELEASE_TT_B1,     0},
    .coef_a={134217728,    -ANC_LIMITER_RELEASE_TT_A1,     0},
};
#endif

#if defined(ANC_LIMITER_FB)

#define ANC_LIMITER_ATT_FB_B1 (134217728/(uint32)(ANC_LIMITER_ATT_FB_TIME*100))
#define ANC_LIMITER_ATT_FB_A1 (134217728 -ANC_LIMITER_ATT_FB_B1)

#define ANC_LIMITER_RELEASE_FB_B1 (134217728/(uint32)(ANC_LIMITER_RELEASE_FB_TIME*100))
#define ANC_LIMITER_RELEASE_FB_A1 (134217728-ANC_LIMITER_RELEASE_FB_B1)

static const anc_iir_coefs   iir_coef_limiter_attack_fb=
{
    .coef_b={0,    ANC_LIMITER_ATT_FB_B1,     0},
    .coef_a={134217728,    -ANC_LIMITER_ATT_FB_A1,     0},
};
static const anc_iir_coefs   iir_coef_limiter_release_fb=
{
    .coef_b={0,    ANC_LIMITER_RELEASE_FB_B1,     0},
    .coef_a={134217728,    -ANC_LIMITER_RELEASE_FB_A1,     0},
};
#endif

#if defined(AUDIO_ANC_FB_MC_HW) &&defined(ANC_LIMITER_MC)

#define ANC_LIMITER_ATT_MC_B1 (134217728/(uint32)(ANC_LIMITER_ATT_MC_TIME*100))
#define ANC_LIMITER_ATT_MC_A1 (134217728 -ANC_LIMITER_ATT_MC_B1)

#define ANC_LIMITER_RELEASE_MC_B1 (134217728/(uint32)(ANC_LIMITER_RELEASE_MC_TIME*100))
#define ANC_LIMITER_RELEASE_MC_A1 (134217728-ANC_LIMITER_RELEASE_MC_B1)

static const anc_iir_coefs   iir_coef_limiter_attack_mc=
{
    .coef_b={0,    ANC_LIMITER_ATT_MC_B1,     0},
    .coef_a={134217728,    -ANC_LIMITER_ATT_MC_A1,     0},
};
static const anc_iir_coefs   iir_coef_limiter_release_mc=
{
    .coef_b={0,    ANC_LIMITER_RELEASE_MC_B1,     0},
    .coef_a={134217728,    -ANC_LIMITER_RELEASE_MC_A1,     0},
};
#endif

/*******************************data struction***********************************/

#define ANC_BASE                            ((uint32)CODEC_BASE)

//clock
volatile static struct _anc_iir_clock *anc_iir_clock=(volatile struct _anc_iir_clock *)(ANC_BASE+0x60);

//calib gain
volatile static struct _anc_ff_iir_calib_gain *anc_ff_iir_calib_gain=(volatile struct _anc_ff_iir_calib_gain *)(ANC_BASE+0x238);
volatile static struct _anc_fb_iir_calib_gain *anc_fb_iir_calib_gain=(volatile struct _anc_fb_iir_calib_gain *)(ANC_BASE+0x23c);
#if defined(AUDIO_ANC_TT_HW)
volatile static struct _anc_tt_iir_calib_gain *anc_tt_iir_calib_gain=(volatile struct _anc_tt_iir_calib_gain *)(ANC_BASE+0x378);
#endif
volatile static struct _anc_iir_calib_gain_config *anc_iir_calib_gain_config=(volatile struct _anc_iir_calib_gain_config *)(ANC_BASE+0x240);

volatile static struct _anc_control *anc_control=(volatile struct _anc_control *)(ANC_BASE+0xd0);
volatile static struct _tm_config *tm_config=(volatile struct _tm_config *)(ANC_BASE+0x22c);

volatile static struct _anc_ff_gain *anc_ff_gain=(volatile struct _anc_ff_gain *)(ANC_BASE+0xd4);
volatile static struct _anc_fb_gain *anc_fb_gain=(volatile struct _anc_fb_gain *)(ANC_BASE+0xd8);
#if defined(AUDIO_ANC_TT_HW)
volatile static struct _anc_tt_gain *anc_tt_gain=(volatile struct _anc_tt_gain *)(ANC_BASE+0x230);
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
volatile static struct _anc_mc_gain *anc_mc_gain=(volatile struct _anc_mc_gain *)(ANC_BASE+0x234);
#endif

volatile static struct _anc_path_set *anc_path_set=(volatile struct _anc_path_set *)(ANC_BASE+0x374);

volatile static struct _anc_iir0_control *anc_iir0_control=(volatile struct _anc_iir0_control *)(ANC_BASE+0x244);
volatile static struct _anc_iir2_control *anc_iir2_control=(volatile struct _anc_iir2_control *)(ANC_BASE+0x24c);

volatile static struct _anc_iir_gain_update *anc_iir_gain_update=(volatile struct _anc_iir_gain_update *)(ANC_BASE+0x25c);

volatile static int32 *codec_iir0_gaina_ext_ch0=(volatile int32 *)(ANC_BASE+0x0260);
volatile static int32 *codec_iir0_gaina_ext_ch1=(volatile int32 *)(ANC_BASE+0x0264);
volatile static int32 *codec_iir0_gainb_ext_ch0=(volatile int32 *)(ANC_BASE+0x0268);
volatile static int32 *codec_iir0_gainb_ext_ch1=(volatile int32 *)(ANC_BASE+0x026c);

volatile static int32 *codec_iir2_gaina_ext_ch0=(volatile int32 *)(ANC_BASE+0x0280);
volatile static int32 *codec_iir2_gaina_ext_ch1=(volatile int32 *)(ANC_BASE+0x0284);
volatile static int32 *codec_iir2_gainb_ext_ch0=(volatile int32 *)(ANC_BASE+0x0288);
volatile static int32 *codec_iir2_gainb_ext_ch1=(volatile int32 *)(ANC_BASE+0x028c);

volatile static int32 *codec_iir0_gaina_ext_out_ch0_sync=(volatile int32 *)(ANC_BASE+0x02a0);
volatile static int32 *codec_iir0_gaina_ext_out_ch1_sync=(volatile int32 *)(ANC_BASE+0x02a4);
volatile static int32 *codec_iir0_gainb_ext_out_ch0_sync=(volatile int32 *)(ANC_BASE+0x02a8);
volatile static int32 *codec_iir0_gainb_ext_out_ch1_sync=(volatile int32 *)(ANC_BASE+0x02ac);

volatile static int32 *codec_iir2_gaina_ext_out_ch0_sync=(volatile int32 *)(ANC_BASE+0x02c0);
volatile static int32 *codec_iir2_gaina_ext_out_ch1_sync=(volatile int32 *)(ANC_BASE+0x02c4);
volatile static int32 *codec_iir2_gainb_ext_out_ch0_sync=(volatile int32 *)(ANC_BASE+0x02c8);
volatile static int32 *codec_iir2_gainb_ext_out_ch1_sync=(volatile int32 *)(ANC_BASE+0x02cc);

#ifdef ANC_GAIN_RAMP
volatile static int32 *codec_iir0_gain_ext_th=(volatile int32 *)(ANC_BASE+0x0340);
volatile static int32 *codec_iir2_gain_ext_th=(volatile int32 *)(ANC_BASE+0x0348);
#endif

#if defined(ANC_LIMITER_FF)
volatile static int32 *codec_iir0_lmt_th_ch0=(volatile int32 *)(ANC_BASE+0x0354);
#endif

#if defined(ANC_LIMITER_TT) && defined(AUDIO_ANC_TT_HW)
volatile static int32 *codec_iir0_lmt_th_ch1=(volatile int32 *)(ANC_BASE+0x0358);
#endif

#if defined(ANC_LIMITER_FB)
volatile static int32 *codec_iir2_lmt_th_ch0=(volatile int32 *)(ANC_BASE+0x0364);
#endif

#if defined(ANC_LIMITER_MC) && defined(AUDIO_ANC_FB_MC_HW)
volatile static int32 *codec_iir2_lmt_th_ch1=(volatile int32 *)(ANC_BASE+0x0368);
#endif

#if defined(ANC_LIMITER_FF)|| defined(ANC_LIMITER_FB)|| defined(ANC_LIMITER_TT)|| defined(ANC_LIMITER_MC)
volatile static struct _anc_iir_lmt_delay *anc_iir_lmt_delay=(volatile struct _anc_iir_lmt_delay *)(ANC_BASE+0x300);
#endif

//IIR 0
// ff tt left coef0 iir
volatile static struct _anc_iir_coefs *anc_ff_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x0800);
//volatile static struct _anc_iir_coefs *anc_tt_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x20a0);
volatile static struct _anc_iir_coefs *anc_tt_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x0800+IIR_FF_COUNTER*20);
//ff tt left ramp coef0 iir
#ifdef ANC_GAIN_RAMP
volatile static struct _anc_iir_coefs *anc_gain_ff_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x0940);
volatile static struct _anc_iir_coefs *anc_gain_tt_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x0954);
#endif
//ff tt left limiter coef0 iir
#ifdef ANC_LIMITER_FF
volatile static struct _anc_iir_coefs *anc_limiter_ff_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x0968);
#endif
#if defined(ANC_LIMITER_TT) && defined(AUDIO_ANC_TT_HW)
volatile static struct _anc_iir_coefs *anc_limiter_tt_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x0990);
#endif
//ff tt left coef1 iir
volatile static struct _anc_iir_coefs *anc_ff_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x0a00);
//volatile static struct _anc_iir_coefs *anc_tt_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x22a0);
volatile static struct _anc_iir_coefs *anc_tt_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x0a00+IIR_FF_COUNTER*20);

//ff tt left ramp coef1 iir
#ifdef ANC_GAIN_RAMP
volatile static struct _anc_iir_coefs *anc_gain_ff_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x0b40);
volatile static struct _anc_iir_coefs *anc_gain_tt_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x0b54);
#endif
//ff tt left limiter coef1 iir
#ifdef ANC_LIMITER_FF
volatile static struct _anc_iir_coefs *anc_limiter_ff_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x0b68);
#endif
#if defined(ANC_LIMITER_TT) && defined(AUDIO_ANC_TT_HW)
volatile static struct _anc_iir_coefs *anc_limiter_tt_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x0b90);
#endif



//IIR 2
//fb mc left coef0 iir
volatile static struct _anc_iir_coefs *anc_fb_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x1000);
//volatile static struct _anc_iir_coefs *anc_mc_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x40a0);
volatile static struct _anc_iir_coefs *anc_mc_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x1000+IIR_FB_COUNTER*20);

//fb mc left ramp coef0 iir
#ifdef ANC_GAIN_RAMP
volatile static struct _anc_iir_coefs *anc_gain_fb_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x1140);
volatile static struct _anc_iir_coefs *anc_gain_mc_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x1154);
#endif
//fb mc left limiter coef0 iir
#ifdef ANC_LIMITER_FB
volatile static struct _anc_iir_coefs *anc_limiter_fb_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x1168);
#endif

#if defined(AUDIO_ANC_FB_MC_HW) &&defined(ANC_LIMITER_MC)
volatile static struct _anc_iir_coefs *anc_limiter_mc_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x1190);
#endif

//fb mc left coef1 iir
volatile static struct _anc_iir_coefs *anc_fb_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x1200);
//volatile static struct _anc_iir_coefs *anc_mc_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x42a0);
volatile static struct _anc_iir_coefs *anc_mc_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x1200+IIR_FB_COUNTER*20);
//fb mc left ramp coef1 iir
#ifdef ANC_GAIN_RAMP
volatile static struct _anc_iir_coefs *anc_gain_fb_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x1340);
volatile static struct _anc_iir_coefs *anc_gain_mc_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x1354);
#endif
//fb mc left limiter coef1 iir
#ifdef ANC_LIMITER_FB
volatile static struct _anc_iir_coefs *anc_limiter_fb_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x1368);
#endif

#if defined(AUDIO_ANC_FB_MC_HW) &&defined(ANC_LIMITER_MC)
volatile static struct _anc_iir_coefs *anc_limiter_mc_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x1390);
#endif


#if defined(AUDIO_ANC_FB_MC)||defined(ANC_FB_CHECK)||defined(ANC_FF_CHECK)||defined(AUDIO_ANC_FB_MC_HW)||defined(ANC_TT_CHECK)
//feedback registers
volatile static struct _feedback_mc_settings *feedback_mc_settings=(volatile struct _feedback_mc_settings *)(ANC_BASE+0xbc);
volatile static struct _fb_check_ch0_config *fb_check_ch0_config=(volatile struct _fb_check_ch0_config *)(ANC_BASE+0x0130);
#endif

#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK) || defined(ANC_TT_CHECK)

volatile static uint32 *codec_fb_check_threshold_ch0=(volatile uint32 *)(ANC_BASE+0x0138);

volatile static uint32 *codec_fb_check_data_avg_keep_ch0=(volatile uint32 *)(ANC_BASE+0x0140);

volatile static uint32 *codec_int_config_clr=(volatile uint32 *)(ANC_BASE+0x0c);
volatile static struct _codec_mask_config *codec_mask_config=(volatile struct _codec_mask_config *)(ANC_BASE+0x10);

#define CODEC_FB_CHECK_ERROR_TRIG_CH0            (1 << 25)
#endif

volatile static int max_ff_gain_l=0;

volatile static int max_fb_gain_l=0;

volatile static int max_tt_gain_l=0;

volatile static int max_mc_gain_l=0;

volatile static int ff_open_flag=0;
volatile static int fb_open_flag=0;

volatile static int tt_open_flag=0;
volatile static int mc_open_flag=0;

volatile static int fb_check_open_flag=0;

volatile static int anc_output_ch_map=AUD_CHANNEL_MAP_CH0;

#ifdef ANC_GAIN_RAMP
volatile static int ff_ramp_gain_l=0;
volatile static int fb_ramp_gain_l=0;
volatile static int tt_ramp_gain_l=0;
volatile static int mc_ramp_gain_l=0;
#endif

volatile static int ff_ramp_coef_l=0;

volatile static int fb_ramp_coef_l=0;

volatile static int tt_ramp_coef_l=0;

volatile static int mc_ramp_coef_l=0;

#if defined(ANC_FB_CHECK) ||defined(ANC_FF_CHECK) || defined(ANC_TT_CHECK)
static HWTIMER_ID anc_howling_check_dev_timer=NULL;
volatile static int ff_howling_gain_l=0;
volatile static int fb_howling_gain_l=0;
volatile static int tt_howling_gain_l=0;

static uint32 howling_cnt_l=0;
static uint32 howling_time_l=0;
#endif

static HWTIMER_ID anc_iir0_set_gain_timer=NULL;
static HWTIMER_ID anc_iir2_set_gain_timer=NULL;

static HWTIMER_ID anc_iir0_switching_timer=NULL;
static HWTIMER_ID anc_iir2_switching_timer=NULL;


#define ANC_SET_GAIN_DELAY_LONG  ((MS_TO_TICKS(200)))

#define ANC_SET_GAIN_DELAY_SHORT  ((MS_TO_TICKS(2)))


static iir_parameter ff_filtes_l_old;
static iir_parameter fb_filtes_l_old;

#if defined(AUDIO_ANC_TT_HW)
static iir_parameter tt_filtes_l_old;
#endif

#if defined(AUDIO_ANC_FB_MC_HW)
static iir_parameter mc_filtes_l_old;
#endif

volatile static int8_t iir0_coef_using;
volatile static int8_t iir2_coef_using;

static int32 trace_counter=0;

static struct_anc_cfg  anc_cfg_coef_new;
static struct_anc_cfg  anc_cfg_coef;

#define CALIB_GAIN_INVALID     (0xFFFF)
static int32_t g_calib_ff_gain = CALIB_GAIN_INVALID;
static int32_t g_calib_fb_gain = CALIB_GAIN_INVALID;
static int32_t g_calib_tt_gain = CALIB_GAIN_INVALID;
static int32_t g_calib_mc_gain = CALIB_GAIN_INVALID;

static int32 anc_cfg_iir0_type;
static int32 anc_cfg_iir2_type;

static int anc_switching_coef_delay_ff_tt=0;
static int anc_switching_gain_delay_ff_tt=0;

static int anc_switching_coef_delay_fb_mc=0;
static int anc_switching_gain_delay_fb_mc=0;

volatile static bool anc_iir0_using_flag=0;
volatile static bool anc_iir2_using_flag=0;

volatile static bool anc_iir0_reserve_flag=0;
volatile static bool anc_iir2_reserve_flag=0;


#define TRACE_COUNTER (10000)



/*******************************FIR hardware filter*********************************/

static void anc_cfg_lock(void)
{
	return;
}

static void anc_cfg_unlock(void)
{
	return;
}


static int iir_filter_para_copy(iir_parameter *filtes_old,const aud_item *filtes_new,uint16_t iir_counter)
{
	filtes_old->iir_bypass_flag=filtes_new->iir_bypass_flag;
	filtes_old->iir_counter=filtes_new->iir_counter;

	int i;

	if(filtes_new->iir_counter>iir_counter)
	{
		LOG_I("%s: warning filtes_new->iir_counter:%d",__func__,filtes_new->iir_counter);
		filtes_old->iir_counter=iir_counter;
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

	for(;i<iir_counter;i++)
	{
		filtes_old->iir_coef[i].coef_a[0]=FIXED_COEF_Q;
		filtes_old->iir_coef[i].coef_a[1]=0;
		filtes_old->iir_coef[i].coef_a[2]=0;
		filtes_old->iir_coef[i].coef_b[0]=FIXED_COEF_Q;
		filtes_old->iir_coef[i].coef_b[1]=0;
		filtes_old->iir_coef[i].coef_b[2]=0;
	}

	filtes_old->total_gain=filtes_new->total_gain;
	filtes_old->iir_counter=iir_counter;

	return 0;
}


static int iir_filter_coef_copy(volatile struct _anc_iir_coefs *iir_coefs,iir_parameter * __restrict filtes_old)
{
    int i;

    for(i=0;i<filtes_old->iir_counter;i++)
    {
        iir_coefs[i].a1=-filtes_old->iir_coef[i].coef_a[1];
        iir_coefs[i].a2=-filtes_old->iir_coef[i].coef_a[2];
        iir_coefs[i].b0=filtes_old->iir_coef[i].coef_b[0];
        iir_coefs[i].b1=filtes_old->iir_coef[i].coef_b[1];
        iir_coefs[i].b2=filtes_old->iir_coef[i].coef_b[2];
    }

    if(filtes_old->iir_counter>0)
    {
        iir_coefs[0].b0=(int32)((filtes_old->iir_coef[0].coef_b[0]*(int64_t)filtes_old->total_gain)>>GAIN_Q);
        iir_coefs[0].b1=(int32)((filtes_old->iir_coef[0].coef_b[1]*(int64_t)filtes_old->total_gain)>>GAIN_Q);
        iir_coefs[0].b2=(int32)((filtes_old->iir_coef[0].coef_b[2]*(int64_t)filtes_old->total_gain)>>GAIN_Q);
    }
    return 0;
}

static int iir0_gaina_cfg_gain(void)
{
    int32 iir0_gaina_ch0;
    int32 iir0_gaina_ch1;
    uint32_t lock;


#if defined(ANC_TT_CHECK) || defined(ANC_FF_CHECK)
    iir0_gaina_ch0=(int32)((((float)ff_ramp_coef_l/512.0f)*((float)ff_ramp_gain_l/512.0f)*((float)ff_howling_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
    iir0_gaina_ch1=(int32)((((float)tt_ramp_coef_l/512.0f)*((float)tt_ramp_gain_l/512.0f)*((float)tt_howling_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
#else
    iir0_gaina_ch0=(int32)((((float)ff_ramp_coef_l/512.0f)*((float)ff_ramp_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
    iir0_gaina_ch1=(int32)((((float)tt_ramp_coef_l/512.0f)*((float)tt_ramp_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
#endif

  //  LOG_I("%s:iir0_gaina_ch0:%d,iir0_gaina_ch1:%d",__func__,iir0_gaina_ch0,iir0_gaina_ch1);
#if 0

    if(iir0_gaina_ch0<=ANC_GAIN_RAMP_THRESHLD+1)iir0_gaina_ch0=ANC_GAIN_RAMP_THRESHLD*2;
    if(iir0_gaina_ch1<=ANC_GAIN_RAMP_THRESHLD+1)iir0_gaina_ch1=ANC_GAIN_RAMP_THRESHLD*2;

    anc_iir_gain_update->codec_iir0_gain_ext_update=0;

    *codec_iir0_gaina_ext_ch0=iir0_gaina_ch0;
    *codec_iir0_gaina_ext_ch1=iir0_gaina_ch1;

    *codec_iir0_gainb_ext_ch0=ANC_GAIN_RAMP_BURST_THRESHLD;
    *codec_iir0_gainb_ext_ch1=ANC_GAIN_RAMP_BURST_THRESHLD;

    anc_iir_gain_update->codec_iir0_gain_ext_update=3;

    while(*codec_iir0_gainb_ext_out_ch0_sync<=ANC_GAIN_RAMP_THRESHLD
            ||*codec_iir0_gainb_ext_out_ch1_sync<=ANC_GAIN_RAMP_THRESHLD)
    {
        LOG_I("%s: *codec_iir0_gainb_ext_out_ch0_sync:%d,*codec_iir0_gainb_ext_out_ch1_sync:%d",__func__,
                *codec_iir0_gainb_ext_out_ch0_sync,
                *codec_iir0_gainb_ext_out_ch1_sync);
        anc_iir_gain_update->codec_iir0_gain_ext_update=0;
        hal_sys_timer_delay_us(1);
        anc_iir_gain_update->codec_iir0_gain_ext_update=3;
    };

    anc_iir_gain_update->codec_iir0_gain_ext_update=0;

    *codec_iir0_gainb_ext_ch0=ANC_GAIN_RAMP_ZERO;
    *codec_iir0_gainb_ext_ch1=ANC_GAIN_RAMP_ZERO;

    anc_iir_gain_update->codec_iir0_gain_ext_update=3;
#else
    lock = int_lock();

    anc_iir_gain_update->codec_iir0_gain_ext_update=0;

    *codec_iir0_gaina_ext_ch0=iir0_gaina_ch0;
    *codec_iir0_gaina_ext_ch1=iir0_gaina_ch1;

    *codec_iir0_gainb_ext_ch0=ANC_GAIN_RAMP_ZERO;
    *codec_iir0_gainb_ext_ch1=ANC_GAIN_RAMP_ZERO;

    hal_sys_timer_delay_us(1);

    anc_iir_gain_update->codec_iir0_gain_ext_update=3;

    int_unlock(lock);
#endif

    return 0;
}

static int iir0_gainb_cfg_gain(void)
{
    int32 iir0_gainb_ch0;
    int32 iir0_gainb_ch1;
    uint32_t lock;

#if defined(ANC_TT_CHECK) ||defined(ANC_FF_CHECK)
    iir0_gainb_ch0=(int32)((((float)ff_ramp_coef_l/512.0f)*((float)ff_ramp_gain_l/512.0f)*((float)ff_howling_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
    iir0_gainb_ch1=(int32)((((float)tt_ramp_coef_l/512.0f)*((float)tt_ramp_gain_l/512.0f)*((float)tt_howling_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
#else
    iir0_gainb_ch0=(int32)((((float)ff_ramp_coef_l/512.0f)*((float)ff_ramp_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
    iir0_gainb_ch1=(int32)((((float)tt_ramp_coef_l/512.0f)*((float)tt_ramp_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
#endif

  //  LOG_I("%s:iir0_gainb_ch0:%d,iir0_gainb_ch1:%d",__func__,iir0_gainb_ch0,iir0_gainb_ch1);
#if 0
    if(iir0_gainb_ch0<=ANC_GAIN_RAMP_THRESHLD+1)iir0_gainb_ch0=ANC_GAIN_RAMP_THRESHLD*2;
    if(iir0_gainb_ch1<=ANC_GAIN_RAMP_THRESHLD+1)iir0_gainb_ch1=ANC_GAIN_RAMP_THRESHLD*2;

    anc_iir_gain_update->codec_iir0_gain_ext_update=0;

    *codec_iir0_gaina_ext_ch0=ANC_GAIN_RAMP_BURST_THRESHLD;
    *codec_iir0_gaina_ext_ch1=ANC_GAIN_RAMP_BURST_THRESHLD;

    *codec_iir0_gainb_ext_ch0=iir0_gainb_ch0;
    *codec_iir0_gainb_ext_ch1=iir0_gainb_ch1;

    anc_iir_gain_update->codec_iir0_gain_ext_update=3;

    hal_sys_timer_delay_us(30);

    while(*codec_iir0_gaina_ext_out_ch0_sync<=ANC_GAIN_RAMP_THRESHLD
            ||*codec_iir0_gaina_ext_out_ch1_sync<=ANC_GAIN_RAMP_THRESHLD)
    {
        LOG_I("%s: *codec_iir0_gaina_ext_out_ch0_sync:%d,*codec_iir0_gaina_ext_out_ch1_sync:%d",__func__,
                *codec_iir0_gaina_ext_out_ch0_sync,
                *codec_iir0_gaina_ext_out_ch1_sync);
        anc_iir_gain_update->codec_iir0_gain_ext_update=0;
        hal_sys_timer_delay_us(1);
        anc_iir_gain_update->codec_iir0_gain_ext_update=3;
    };

    anc_iir_gain_update->codec_iir0_gain_ext_update=0;

    *codec_iir0_gaina_ext_ch0=ANC_GAIN_RAMP_ZERO;
    *codec_iir0_gaina_ext_ch1=ANC_GAIN_RAMP_ZERO;

    anc_iir_gain_update->codec_iir0_gain_ext_update=3;
#else
    lock = int_lock();

    anc_iir_gain_update->codec_iir0_gain_ext_update=0;

    *codec_iir0_gaina_ext_ch0=ANC_GAIN_RAMP_ZERO;
    *codec_iir0_gaina_ext_ch1=ANC_GAIN_RAMP_ZERO;

    *codec_iir0_gainb_ext_ch0=iir0_gainb_ch0;
    *codec_iir0_gainb_ext_ch1=iir0_gainb_ch1;

    hal_sys_timer_delay_us(1);

    anc_iir_gain_update->codec_iir0_gain_ext_update=3;

    int_unlock(lock);
#endif
    return 0;
}

static int iir2_gaina_cfg_gain(void)
{
    int32 iir2_gaina_ch0;
    int32 iir2_gaina_ch1;
    uint32_t lock;

#if defined(ANC_FB_CHECK)
    iir2_gaina_ch0=(int32)((((float)fb_ramp_coef_l/512.0f)*((float)fb_ramp_gain_l/512.0f)*((float)fb_howling_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
    iir2_gaina_ch1=(int32)((((float)mc_ramp_coef_l/512.0f)*((float)mc_ramp_gain_l/512.0f)*((float)fb_howling_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
#else
    iir2_gaina_ch0=(int32)((((float)fb_ramp_coef_l/512.0f)*((float)fb_ramp_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
    iir2_gaina_ch1=(int32)((((float)mc_ramp_coef_l/512.0f)*((float)mc_ramp_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
#endif

//    LOG_I("%s:iir2_gaina_ch0:%d,iir2_gaina_ch1:%d",__func__,iir2_gaina_ch0,iir2_gaina_ch1);
#if 0
    if(iir2_gaina_ch0<=ANC_GAIN_RAMP_THRESHLD+1)iir2_gaina_ch0=ANC_GAIN_RAMP_THRESHLD*2;
    if(iir2_gaina_ch1<=ANC_GAIN_RAMP_THRESHLD+1)iir2_gaina_ch1=ANC_GAIN_RAMP_THRESHLD*2;

    anc_iir_gain_update->codec_iir2_gain_ext_update=0;

    *codec_iir2_gaina_ext_ch0=iir2_gaina_ch0;
    *codec_iir2_gaina_ext_ch1=iir2_gaina_ch1;

    *codec_iir2_gainb_ext_ch0=ANC_GAIN_RAMP_BURST_THRESHLD;
    *codec_iir2_gainb_ext_ch1=ANC_GAIN_RAMP_BURST_THRESHLD;

    anc_iir_gain_update->codec_iir2_gain_ext_update=3;

    while(*codec_iir2_gainb_ext_out_ch0_sync<=ANC_GAIN_RAMP_THRESHLD
            ||*codec_iir2_gainb_ext_out_ch1_sync<=ANC_GAIN_RAMP_THRESHLD)
    {
        LOG_I("%s: codec_iir2_gainb_ext_out_ch0/1_sync:%d/%d",__func__,
                *codec_iir2_gainb_ext_out_ch0_sync,
                *codec_iir2_gainb_ext_out_ch1_sync);
        anc_iir_gain_update->codec_iir2_gain_ext_update=0;
        hal_sys_timer_delay_us(1);
        anc_iir_gain_update->codec_iir2_gain_ext_update=3;

    };

    anc_iir_gain_update->codec_iir2_gain_ext_update=0;

    *codec_iir2_gainb_ext_ch0=ANC_GAIN_RAMP_ZERO;
    *codec_iir2_gainb_ext_ch1=ANC_GAIN_RAMP_ZERO;

    anc_iir_gain_update->codec_iir2_gain_ext_update=3;
#else
    lock = int_lock();
    anc_iir_gain_update->codec_iir2_gain_ext_update=0;
    *codec_iir2_gaina_ext_ch0=iir2_gaina_ch0;
    *codec_iir2_gaina_ext_ch1=iir2_gaina_ch1;
    *codec_iir2_gainb_ext_ch0=ANC_GAIN_RAMP_ZERO;
    *codec_iir2_gainb_ext_ch1=ANC_GAIN_RAMP_ZERO;
    hal_sys_timer_delay_us(1);
    anc_iir_gain_update->codec_iir2_gain_ext_update=3;
    int_unlock(lock);

#endif
    return 0;
}

static int iir2_gainb_cfg_gain(void)
{
    int32 iir2_gainb_ch0;
    int32 iir2_gainb_ch1;
    uint32_t lock;

#if defined(ANC_FB_CHECK)
    iir2_gainb_ch0=(int32)((((float)fb_ramp_coef_l/512.0f)*((float)fb_ramp_gain_l/512.0f)*((float)fb_howling_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
    iir2_gainb_ch1=(int32)((((float)mc_ramp_coef_l/512.0f)*((float)mc_ramp_gain_l/512.0f)*((float)fb_howling_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
#else
    iir2_gainb_ch0=(int32)((((float)fb_ramp_coef_l/512.0f)*((float)fb_ramp_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
    iir2_gainb_ch1=(int32)((((float)mc_ramp_coef_l/512.0f)*((float)mc_ramp_gain_l/512.0f))*FIXED_GAIN_RAMP_Q);
#endif

   // LOG_I("%s:iir2_gainb_ch0:%d,iir2_gainb_ch1:%d",__func__,iir2_gainb_ch0,iir2_gainb_ch1);
#if 0

    if(iir2_gainb_ch0<=ANC_GAIN_RAMP_THRESHLD+1)iir2_gainb_ch0=ANC_GAIN_RAMP_THRESHLD*2;
    if(iir2_gainb_ch1<=ANC_GAIN_RAMP_THRESHLD+1)iir2_gainb_ch1=ANC_GAIN_RAMP_THRESHLD*2;

    anc_iir_gain_update->codec_iir2_gain_ext_update=0;

    *codec_iir2_gaina_ext_ch0=ANC_GAIN_RAMP_BURST_THRESHLD;
    *codec_iir2_gaina_ext_ch1=ANC_GAIN_RAMP_BURST_THRESHLD;

    *codec_iir2_gainb_ext_ch0=iir2_gainb_ch0;
    *codec_iir2_gainb_ext_ch1=iir2_gainb_ch1;

    anc_iir_gain_update->codec_iir2_gain_ext_update=3;

    while(*codec_iir2_gaina_ext_out_ch0_sync<=ANC_GAIN_RAMP_THRESHLD
            ||*codec_iir2_gaina_ext_out_ch1_sync<=ANC_GAIN_RAMP_THRESHLD)
    {
        LOG_I("%s: *codec_iir2_gaina_ext_out_ch0/1_sync:%d/%d",__func__,
                *codec_iir2_gaina_ext_out_ch0_sync,
                *codec_iir2_gaina_ext_out_ch1_sync);
        anc_iir_gain_update->codec_iir2_gain_ext_update=0;
        hal_sys_timer_delay_us(1);
        anc_iir_gain_update->codec_iir2_gain_ext_update=3;
    };

    anc_iir_gain_update->codec_iir2_gain_ext_update=0;

    *codec_iir2_gaina_ext_ch0=ANC_GAIN_RAMP_ZERO;
    *codec_iir2_gaina_ext_ch1=ANC_GAIN_RAMP_ZERO;

    anc_iir_gain_update->codec_iir2_gain_ext_update=3;
#else
    lock = int_lock();
    anc_iir_gain_update->codec_iir2_gain_ext_update=0;
    *codec_iir2_gaina_ext_ch0=ANC_GAIN_RAMP_ZERO;
    *codec_iir2_gaina_ext_ch1=ANC_GAIN_RAMP_ZERO;
    *codec_iir2_gainb_ext_ch0=iir2_gainb_ch0;
    *codec_iir2_gainb_ext_ch1=iir2_gainb_ch1;
    hal_sys_timer_delay_us(1);
    anc_iir_gain_update->codec_iir2_gain_ext_update=3;
    int_unlock(lock);
#endif

    return 0;
}

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)

static int iir0_gainb_creat_stop_status(void)
{
    LOG_I("%s",__func__);

    iir0_gaina_cfg_gain();
    trace_counter=0;

    while(anc_iir0_control->codec_iir0_iirb_stop_status_sync==0)
    {
        if(trace_counter%TRACE_COUNTER==0)
        {
            LOG_I("2 codec_iir0_gainb_ext_out_ch0_sync:%d,codec_iir0_gainb_ext_out_ch1_sync:%d",
                    *codec_iir0_gainb_ext_out_ch0_sync,
                    *codec_iir0_gainb_ext_out_ch1_sync);
        }
        trace_counter++;
    };

    return 0;
}
static int iir0_gaina_creat_stop_status(void)
{
    LOG_I("%s",__func__);

    iir0_gainb_cfg_gain();

    trace_counter=0;

    while(anc_iir0_control->codec_iir0_iira_stop_status_sync==0)
    {
        if(trace_counter%TRACE_COUNTER==0)
        {
            LOG_I("2 codec_iir0_gaina_ext_out_ch0_sync:%d,codec_iir0_gaina_ext_out_ch1_sync:%d",
                    *codec_iir0_gaina_ext_out_ch0_sync,
                    *codec_iir0_gaina_ext_out_ch1_sync);
        }
        trace_counter++;
    };

    return 0;
}

static int iir2_gainb_creat_stop_status(void)
{
	LOG_I("%s",__func__);

	iir2_gaina_cfg_gain();

	trace_counter=0;

	while(anc_iir2_control->codec_iir2_iirb_stop_status_sync==0)
	{
	  	if(trace_counter%TRACE_COUNTER==0)
	  	{
			LOG_I("2 codec_iir2_gainb_ext_out_ch0/1_sync:%d/%d",
			*codec_iir2_gainb_ext_out_ch0_sync,
			*codec_iir2_gainb_ext_out_ch1_sync);
	  	}
		trace_counter++;
	};

	return 0;
}
static int iir2_gaina_creat_stop_status(void)
{
    LOG_I("%s",__func__);
    iir2_gainb_cfg_gain();

    trace_counter=0;

    while(anc_iir2_control->codec_iir2_iira_stop_status_sync==0)
    {
        if(trace_counter%TRACE_COUNTER==0)
        {
            LOG_I("2 *codec_iir2_gaina_ext_out_ch0/1_sync:%d/%d",
                    *codec_iir2_gaina_ext_out_ch0_sync,
                    *codec_iir2_gaina_ext_out_ch1_sync);
        }
        trace_counter++;
    };

    return 0;
}
#endif



/**
 * @brief      Update coef and store gain,
 * 				fadein: enable: use anc_set_gain(), disable: anc_apply_max_gain() to set gain.
 *
 * @param[in]  cfg   coef and gain
 */
static int anc_set_cfg_internal(const struct_anc_cfg * cfg, enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err=ANC_NO_ERR;
    uint32_t lock;
    //int32 gaina_ch0, gaina_ch1,gainb_ch0,gainb_ch1;

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    const aud_item *ff_filtes_l;
    const aud_item *fb_filtes_l;

#if defined(AUDIO_ANC_TT_HW)
    const aud_item *tt_filtes_l;
#endif

#if defined(AUDIO_ANC_FB_MC_HW)
    const aud_item *mc_filtes_l;
#endif
#endif

    if ((anc_type&ANC_FEEDFORWARD && ff_open_flag==0)
    || (anc_type&ANC_FEEDBACK && fb_open_flag==0)
#if defined(AUDIO_ANC_TT_HW)
   ||  (anc_type&ANC_TALKTHRU && tt_open_flag==0)
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    || (anc_type&ANC_MUSICCANCLE && mc_open_flag==0)
#endif
     )
    {
        LOG_I("%s: anc_type=%d not opened", __func__, anc_type);
        return ANC_OTHER_ERR;
    }

    //anc_gain_delay=ANC_GAIN_NO_DELAY;

    //LOG_I("%s*******anc_type:%d, seting:a1:%d,a2:%d", __func__,anc_type,anc_gain_ff_iir_coefs0_l[0].a1,anc_gain_ff_iir_coefs0_l[0].a2);


    //LOG_I("%s: iir0_coef_using:%d,iir1_coef_using:%d,iir2_coef_using:%d,iir3_coef_using:%d,anc_gain_delay:%d", __func__,iir0_coef_using,iir1_coef_using,iir2_coef_using,iir3_coef_using,anc_gain_delay);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    ff_filtes_l = &(cfg->anc_cfg_ff_l);
    fb_filtes_l = &(cfg->anc_cfg_fb_l);
#if defined(AUDIO_ANC_TT_HW)
    tt_filtes_l = &(cfg->anc_cfg_tt_l);
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    mc_filtes_l = &(cfg->anc_cfg_mc_l);
#endif
#endif


    //LOG_I("ff_l:%d,ff_r:%d,fb_l:%d,fb_r:%d",ff_filtes_l->total_gain,ff_filtes_r->total_gain,fb_filtes_l->total_gain,fb_filtes_r->total_gain);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
#if ANC_PROD_TEST
    if(anc_type&ANC_FEEDFORWARD)
    {
        LOG_I("ff gain: %d, counter: %d iir_bypass_flag: %d ",ff_filtes_l->total_gain, ff_filtes_l->iir_counter,ff_filtes_l->iir_bypass_flag);
        LOG_I("ff dac_gain_offset %d, adc_gain_offset %d",ff_filtes_l->dac_gain_offset, ff_filtes_l->adc_gain_offset);

        for(int j = 0; j <IIR_FF_COUNTER; j++)
        {
            //LOG_I("iir coef ff l %10d, %10d, %10d, %10d, %10d, %10d",
            LOG_I("iir coef ff l 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x",\
                    ff_filtes_l->iir_coef[j].coef_b[0], \
                    ff_filtes_l->iir_coef[j].coef_b[1], \
                    ff_filtes_l->iir_coef[j].coef_b[2], \
                    ff_filtes_l->iir_coef[j].coef_a[0], \
                    ff_filtes_l->iir_coef[j].coef_a[1], \
                    ff_filtes_l->iir_coef[j].coef_a[2]);
        }
    }
    if(anc_type&ANC_FEEDBACK)
    {
        LOG_I("fb gain: %d, counter: %d iir_bypass_flag: %d ",fb_filtes_l->total_gain, fb_filtes_l->iir_counter,fb_filtes_l->iir_bypass_flag);
        LOG_I("fb dac_gain_offset %d, adc_gain_offset %d",fb_filtes_l->dac_gain_offset, fb_filtes_l->adc_gain_offset);

        for(int j = 0; j <IIR_FB_COUNTER; j++)
        {
            //LOG_I("iir coef fb l %10d, %10d, %10d, %10d, %10d, %10d",
            LOG_I("iir coef fb l 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x", \
            	      fb_filtes_l->iir_coef[j].coef_b[0], \
                    fb_filtes_l->iir_coef[j].coef_b[1], \
                    fb_filtes_l->iir_coef[j].coef_b[2], \
                    fb_filtes_l->iir_coef[j].coef_a[0], \
                    fb_filtes_l->iir_coef[j].coef_a[1], \
                    fb_filtes_l->iir_coef[j].coef_a[2]);
        }
    }
#if defined(AUDIO_ANC_TT_HW)
    if(anc_type&ANC_TALKTHRU)
    {
        LOG_I("tt gain: %d, counter: %d iir_bypass_flag: %d ",tt_filtes_l->total_gain, tt_filtes_l->iir_counter,tt_filtes_l->iir_bypass_flag);
        LOG_I("tt dac_gain_offset %d, adc_gain_offset %d",tt_filtes_l->dac_gain_offset, tt_filtes_l->adc_gain_offset);
        for(int j = 0; j <IIR_TT_COUNTER; j++)
        {
//            LOG_I("iir coef tt l %10d, %10d, %10d, %10d, %10d, %10d",
            LOG_I("iir coef tt l 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x", \
                    tt_filtes_l->iir_coef[j].coef_b[0], \
                    tt_filtes_l->iir_coef[j].coef_b[1], \
                    tt_filtes_l->iir_coef[j].coef_b[2], \
                    tt_filtes_l->iir_coef[j].coef_a[0], \
                    tt_filtes_l->iir_coef[j].coef_a[1], \
                    tt_filtes_l->iir_coef[j].coef_a[2]);
        }
    }
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    if(anc_type&ANC_MUSICCANCLE)
    {
        LOG_I("mc gain: %d, counter: %d iir_bypass_flag: %d ",mc_filtes_l->total_gain, mc_filtes_l->iir_counter,mc_filtes_l->iir_bypass_flag);
        for(int j = 0; j <IIR_MC_COUNTER; j++)
        {
//            LOG_I("iir coef mc l %10d, %10d, %10d, %10d, %10d, %10d",
            LOG_I("iir coef mc l 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x",\
                    mc_filtes_l->iir_coef[j].coef_b[0], \
                    mc_filtes_l->iir_coef[j].coef_b[1], \
                    mc_filtes_l->iir_coef[j].coef_b[2], \
                    mc_filtes_l->iir_coef[j].coef_a[0], \
                    mc_filtes_l->iir_coef[j].coef_a[1], \
                    mc_filtes_l->iir_coef[j].coef_a[2]);
        }
    }
#endif
#endif
#endif

    if(anc_type&ANC_FEEDFORWARD)
    {
        analog_aud_apply_anc_adc_gain_offset(ANC_FEEDFORWARD, ff_filtes_l->adc_gain_offset, 0);
    }

    if(anc_type&ANC_FEEDBACK)
    {
        analog_aud_apply_anc_adc_gain_offset(ANC_FEEDBACK, fb_filtes_l->adc_gain_offset, 0);
    }

#if defined(AUDIO_ANC_TT_HW)
    if(anc_type&ANC_TALKTHRU)
    {
        analog_aud_apply_anc_adc_gain_offset(ANC_TALKTHRU, tt_filtes_l->adc_gain_offset, 0);
    }
#endif


    /***************************ANC_FEEDFORWARD**********************/
    if(anc_type&ANC_FEEDFORWARD)
    {
        LOG_I("%s: ANC_FEEDFORWARD",__func__);
        // LOG_I("iir0_iira_stop_status_sync:%d,iir0_iirb_stop_status_sync:%d",anc_iir0_control->codec_iir0_iira_stop_status_sync,anc_iir0_control->codec_iir0_iirb_stop_status_sync);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            iir_filter_para_copy(&ff_filtes_l_old,ff_filtes_l,IIR_FF_COUNTER);
#if defined(AUDIO_ANC_TT_HW)
            if(anc_type&ANC_TALKTHRU)
            {
                LOG_I("%s: ANC_TALKTHRU",__func__);

                iir_filter_para_copy(&tt_filtes_l_old,tt_filtes_l,IIR_TT_COUNTER);

                if(tt_filtes_l_old.iir_bypass_flag==1)
                {
                    max_tt_gain_l=tt_filtes_l_old.total_gain;
                }
                else
                {
                    max_tt_gain_l=(1<<GAIN_Q);
                }
            }
#endif
            if(ff_filtes_l_old.iir_bypass_flag==1)
            {
                max_ff_gain_l=ff_filtes_l_old.total_gain;
            }
            else
            {
                max_ff_gain_l=(1<<GAIN_Q);
            }

            if(iir0_coef_using==0)
            {
                trace_counter=0;

                while(*codec_iir0_gainb_ext_out_ch0_sync>*codec_iir0_gain_ext_th
                        ||*codec_iir0_gainb_ext_out_ch1_sync>*codec_iir0_gain_ext_th)
                {
                    if(trace_counter%TRACE_COUNTER==0)
                    {

                        LOG_I("%s: codec_iir0_gainb_ext_out_ch0_sync:%d,codec_iir0_gainb_ext_out_ch1_sync:%d",__func__,
                                *codec_iir0_gainb_ext_out_ch0_sync,
                                *codec_iir0_gainb_ext_out_ch1_sync);
                    }
                    trace_counter++;
                };

                if(anc_iir0_control->codec_iir0_iirb_stop_status_sync==0)
                {
                    iir0_gainb_creat_stop_status();
                }

                anc_iir0_control->codec_iir0_ch0_bypass=ff_filtes_l_old.iir_bypass_flag;
                anc_iir0_control->codec_iir0_count_ch0=ff_filtes_l_old.iir_counter;

                iir_filter_coef_copy(anc_ff_iir_coefs1_l,&ff_filtes_l_old);
#if defined(AUDIO_ANC_TT_HW)
            if(anc_type&ANC_TALKTHRU)
            {
                anc_iir0_control->codec_iir0_ch1_bypass=tt_filtes_l_old.iir_bypass_flag;
                anc_iir0_control->codec_iir0_count_ch1=tt_filtes_l_old.iir_counter;
             }
               if(anc_iir0_control->codec_iir0_ch0_bypass==1)
               {
                    iir_filter_coef_copy(anc_ff_iir_coefs1_l,&tt_filtes_l_old);
               }
               else
               {
                    iir_filter_coef_copy(anc_tt_iir_coefs1_l,&tt_filtes_l_old);
               }
#endif
                anc_iir0_control->codec_iir0_iirb_enable=1;
                iir0_coef_using=1;
            }
            else
            {
                trace_counter=0;
                while(*codec_iir0_gaina_ext_out_ch0_sync>*codec_iir0_gain_ext_th
                        ||*codec_iir0_gaina_ext_out_ch1_sync>*codec_iir0_gain_ext_th)
                {
                    if(trace_counter%TRACE_COUNTER==0)
                    {

                        LOG_I("%s: codec_iir0_gaina_ext_out_ch0_sync:%d,codec_iir0_gaina_ext_out_ch1_sync:%d",__func__,
                                *codec_iir0_gaina_ext_out_ch0_sync,
                                *codec_iir0_gaina_ext_out_ch1_sync);
                    }
                    trace_counter++;
                };

                if(anc_iir0_control->codec_iir0_iira_stop_status_sync==0)
                {
                    iir0_gaina_creat_stop_status();
                }

                anc_iir0_control->codec_iir0_ch0_bypass=ff_filtes_l_old.iir_bypass_flag;
                anc_iir0_control->codec_iir0_count_ch0=ff_filtes_l_old.iir_counter;

                iir_filter_coef_copy(anc_ff_iir_coefs0_l,&ff_filtes_l_old);
#if defined(AUDIO_ANC_TT_HW)
            if(anc_type&ANC_TALKTHRU)
            {
                anc_iir0_control->codec_iir0_ch1_bypass=tt_filtes_l_old.iir_bypass_flag;
                anc_iir0_control->codec_iir0_count_ch1=tt_filtes_l_old.iir_counter;
             }
               if(anc_iir0_control->codec_iir0_ch0_bypass==1)
               {
                    iir_filter_coef_copy(anc_ff_iir_coefs0_l,&tt_filtes_l_old);
               }
               else
               {
                    iir_filter_coef_copy(anc_tt_iir_coefs0_l,&tt_filtes_l_old);
               }
#endif

                anc_iir0_control->codec_iir0_iira_enable=1;
                iir0_coef_using=0;
            }

	    ff_ramp_coef_l=max_ff_gain_l;
	    tt_ramp_coef_l=max_tt_gain_l;

            lock = int_lock();
            anc_iir_gain_update->codec_iir0_gain_ext_update=0;
            hal_sys_timer_delay_us(1);
            anc_iir_gain_update->codec_iir0_gain_ext_update=3;
            int_unlock(lock);

            hwtimer_stop(anc_iir0_set_gain_timer);
            hwtimer_start(anc_iir0_set_gain_timer, anc_switching_gain_delay_ff_tt);

            max_ff_gain_l=(1<<GAIN_Q);
#if defined(AUDIO_ANC_TT_HW)
	     max_tt_gain_l=(1<<GAIN_Q);
#endif
        }
#endif
        return err;
    }

    /***************************ANC_TALKTHRU**********************/
#if defined(AUDIO_ANC_TT_HW)
    //talk thr settings
    if(anc_type&ANC_TALKTHRU)
    {
        LOG_I("%s: ANC_TALKTHRU",__func__);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            iir_filter_para_copy(&tt_filtes_l_old,tt_filtes_l,IIR_TT_COUNTER);

            if(tt_filtes_l_old.iir_bypass_flag==1)
            {
                max_tt_gain_l=tt_filtes_l_old.total_gain;
            }
            else
            {
                max_tt_gain_l=(1<<GAIN_Q);
            }

            if(iir0_coef_using==0)
            {
                trace_counter=0;

                while(*codec_iir0_gainb_ext_out_ch0_sync>*codec_iir0_gain_ext_th
                        ||*codec_iir0_gainb_ext_out_ch1_sync>*codec_iir0_gain_ext_th)
                {
                    if(trace_counter%TRACE_COUNTER==0)
                    {

                        LOG_I("%s: codec_iir0_gainb_ext_out_ch0_sync:%d,codec_iir0_gainb_ext_out_ch1_sync:%d",__func__,
                                *codec_iir0_gainb_ext_out_ch0_sync,
                                *codec_iir0_gainb_ext_out_ch1_sync);
                    }
                    trace_counter++;
                };

                if(anc_iir0_control->codec_iir0_iirb_stop_status_sync==0)
                {
                    iir0_gainb_creat_stop_status();
                }

                anc_iir0_control->codec_iir0_ch1_bypass=tt_filtes_l_old.iir_bypass_flag;
                anc_iir0_control->codec_iir0_count_ch1=tt_filtes_l_old.iir_counter;

               if(anc_iir0_control->codec_iir0_ch0_bypass==1)
               {
                    iir_filter_coef_copy(anc_ff_iir_coefs1_l,&tt_filtes_l_old);
               }
               else
               {
                    iir_filter_coef_copy(anc_tt_iir_coefs1_l,&tt_filtes_l_old);
                    iir_filter_coef_copy(anc_ff_iir_coefs1_l,&ff_filtes_l_old);
               }

                anc_iir0_control->codec_iir0_iirb_enable=1;
                iir0_coef_using=1;
            }
            else
            {
                trace_counter=0;
                while(*codec_iir0_gaina_ext_out_ch0_sync>*codec_iir0_gain_ext_th
                        ||*codec_iir0_gaina_ext_out_ch1_sync>*codec_iir0_gain_ext_th)
                {
                    if(trace_counter%TRACE_COUNTER==0)
                    {

                        LOG_I("%s: codec_iir0_gaina_ext_out_ch0_sync:%d,codec_iir0_gaina_ext_out_ch1_sync:%d",__func__,
                                *codec_iir0_gaina_ext_out_ch0_sync,
                                *codec_iir0_gaina_ext_out_ch1_sync);
                    }
                    trace_counter++;
                };

                if(anc_iir0_control->codec_iir0_iira_stop_status_sync==0)
                {
                    iir0_gaina_creat_stop_status();
                }

                anc_iir0_control->codec_iir0_ch1_bypass=tt_filtes_l_old.iir_bypass_flag;
                anc_iir0_control->codec_iir0_count_ch1=tt_filtes_l_old.iir_counter;


               if(anc_iir0_control->codec_iir0_ch0_bypass==1)
               {
                    iir_filter_coef_copy(anc_ff_iir_coefs0_l,&tt_filtes_l_old);
               }
               else
               {
                    iir_filter_coef_copy(anc_tt_iir_coefs0_l,&tt_filtes_l_old);
                    iir_filter_coef_copy(anc_ff_iir_coefs0_l,&ff_filtes_l_old);
               }

                anc_iir0_control->codec_iir0_iira_enable=1;
                iir0_coef_using=0;
            }
	    ff_ramp_coef_l=max_ff_gain_l;
	    tt_ramp_coef_l=max_tt_gain_l;

            lock = int_lock();
            anc_iir_gain_update->codec_iir0_gain_ext_update=0;
            hal_sys_timer_delay_us(1);
            anc_iir_gain_update->codec_iir0_gain_ext_update=3;
            int_unlock(lock);

            hwtimer_stop(anc_iir0_set_gain_timer);
            hwtimer_start(anc_iir0_set_gain_timer, anc_switching_gain_delay_ff_tt);
            max_tt_gain_l=(1<<GAIN_Q);
        }
#endif
        return err;
    }
#endif


    /***************************ANC_FEEDBACK**********************/
    if(anc_type&ANC_FEEDBACK)
    {
        LOG_I("%s: ANC_FEEDBACK",__func__);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            iir_filter_para_copy(&fb_filtes_l_old,fb_filtes_l,IIR_FB_COUNTER);
#if defined(AUDIO_ANC_FB_MC_HW)
            if(anc_type&ANC_MUSICCANCLE)
            {
                LOG_I("%s: ANC_MUSICCANCLE",__func__);

                iir_filter_para_copy(&mc_filtes_l_old,mc_filtes_l,IIR_MC_COUNTER);
                if(mc_filtes_l_old.iir_bypass_flag==1)
                {
                    max_mc_gain_l=mc_filtes_l_old.total_gain;
                }
                else
                {
                    max_mc_gain_l=(1<<GAIN_Q);
                }
            }
#endif
            if(fb_filtes_l_old.iir_bypass_flag==1)
            {
                max_fb_gain_l=fb_filtes_l_old.total_gain;
            }
            else
            {
                max_fb_gain_l=(1<<GAIN_Q);
            }

            if(iir2_coef_using==0)
            {
                trace_counter=0;
                while(*codec_iir2_gainb_ext_out_ch0_sync>*codec_iir2_gain_ext_th
                        ||*codec_iir2_gainb_ext_out_ch1_sync>*codec_iir2_gain_ext_th)
                {
                    if(trace_counter%TRACE_COUNTER==0)
                    {

                        LOG_I("%s: codec_iir2_gainb_ext_out_ch0/1_sync:%d/%d",__func__,
                                *codec_iir2_gainb_ext_out_ch0_sync,
                                *codec_iir2_gainb_ext_out_ch1_sync);
                    }
                    trace_counter++;
                };

                if(anc_iir2_control->codec_iir2_iirb_stop_status_sync==0)
                {
                    iir2_gainb_creat_stop_status();
                }

                anc_iir2_control->codec_iir2_ch0_bypass=fb_filtes_l_old.iir_bypass_flag;
                anc_iir2_control->codec_iir2_count_ch0=fb_filtes_l_old.iir_counter;

                iir_filter_coef_copy(anc_fb_iir_coefs1_l,&fb_filtes_l_old);
#if defined(AUDIO_ANC_FB_MC_HW)
            if(anc_type&ANC_MUSICCANCLE)
            {
                anc_iir2_control->codec_iir2_ch1_bypass=mc_filtes_l_old.iir_bypass_flag;
                anc_iir2_control->codec_iir2_count_ch1=mc_filtes_l_old.iir_counter;
             }

                 if(anc_iir2_control->codec_iir2_ch0_bypass==1)
                 {
                     iir_filter_coef_copy(anc_fb_iir_coefs1_l,&mc_filtes_l_old);
                 }
                 else
                 {
                     iir_filter_coef_copy(anc_mc_iir_coefs1_l,&mc_filtes_l_old);
                 }
#endif

                anc_iir2_control->codec_iir2_iirb_enable=1;
                iir2_coef_using=1;
            }
            else
            {
                trace_counter=0;
                while(*codec_iir2_gaina_ext_out_ch0_sync>*codec_iir2_gain_ext_th
                        ||*codec_iir2_gaina_ext_out_ch1_sync>*codec_iir2_gain_ext_th)
                {
                    if(trace_counter%TRACE_COUNTER==0)
                    {
                        LOG_I("%s: *codec_iir2_gaina_ext_out_ch0/1_sync:%d/%d",__func__,
                                *codec_iir2_gaina_ext_out_ch0_sync,
                                *codec_iir2_gaina_ext_out_ch1_sync);
                    }
                    trace_counter++;
                };

                if(anc_iir2_control->codec_iir2_iira_stop_status_sync==0)
                {
                    iir2_gaina_creat_stop_status();
                }
                anc_iir2_control->codec_iir2_ch0_bypass=fb_filtes_l_old.iir_bypass_flag;
                anc_iir2_control->codec_iir2_count_ch0=fb_filtes_l_old.iir_counter;

                iir_filter_coef_copy(anc_fb_iir_coefs0_l,&fb_filtes_l_old);
#if defined(AUDIO_ANC_FB_MC_HW)
            if(anc_type&ANC_MUSICCANCLE)
            {
                anc_iir2_control->codec_iir2_ch1_bypass=mc_filtes_l_old.iir_bypass_flag;
                anc_iir2_control->codec_iir2_count_ch1=mc_filtes_l_old.iir_counter;
             }
                 if(anc_iir2_control->codec_iir2_ch0_bypass==1)
                 {
                     iir_filter_coef_copy(anc_fb_iir_coefs0_l,&mc_filtes_l_old);
                 }
                 else
                 {
                     iir_filter_coef_copy(anc_mc_iir_coefs0_l,&mc_filtes_l_old);
                 }
#endif
                anc_iir2_control->codec_iir2_iira_enable=1;
                iir2_coef_using=0;
            }

	    fb_ramp_coef_l=max_fb_gain_l;
	    mc_ramp_coef_l=max_mc_gain_l;

            lock = int_lock();
            anc_iir_gain_update->codec_iir2_gain_ext_update=0;
            hal_sys_timer_delay_us(1);
            anc_iir_gain_update->codec_iir2_gain_ext_update=3;
            int_unlock(lock);


            hwtimer_stop(anc_iir2_set_gain_timer);
            hwtimer_start(anc_iir2_set_gain_timer, anc_switching_gain_delay_fb_mc);
            max_fb_gain_l=(1<<GAIN_Q);
#if defined(AUDIO_ANC_FB_MC_HW)
	     max_mc_gain_l=(1<<GAIN_Q);
#endif
        }
#endif
        return err;
    }


    /***************************ANC_MUSICCANCLE**********************/
#if defined(AUDIO_ANC_FB_MC_HW)
    //music cancel ANC settings
    if(anc_type&ANC_MUSICCANCLE)
    {
        LOG_I("%s: ANC_MUSICCANCLE",__func__);

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            iir_filter_para_copy(&mc_filtes_l_old,mc_filtes_l,IIR_MC_COUNTER);

            if(mc_filtes_l_old.iir_bypass_flag==1)
            {
                max_mc_gain_l=mc_filtes_l_old.total_gain;
            }
            else
            {
                max_mc_gain_l=(1<<GAIN_Q);
            }

            if(iir2_coef_using==0)
            {
                trace_counter=0;
                while(*codec_iir2_gainb_ext_out_ch0_sync>*codec_iir2_gain_ext_th
                        ||*codec_iir2_gainb_ext_out_ch1_sync>*codec_iir2_gain_ext_th)
                {
                    if(trace_counter%TRACE_COUNTER==0)
                    {
                        LOG_I("%s: 1 codec_iir2_gainb_ext_out_ch0/1_sync:%d/%d",__func__,
                                *codec_iir2_gainb_ext_out_ch0_sync,
                                *codec_iir2_gainb_ext_out_ch1_sync);
                    }
                    trace_counter++;
                };

                if(anc_iir2_control->codec_iir2_iirb_stop_status_sync==0)
                {
                    iir2_gainb_creat_stop_status();
                }

                anc_iir2_control->codec_iir2_ch1_bypass=mc_filtes_l_old.iir_bypass_flag;
                anc_iir2_control->codec_iir2_count_ch1=mc_filtes_l_old.iir_counter;

                if(anc_iir2_control->codec_iir2_ch0_bypass==1)
                {
                    iir_filter_coef_copy(anc_fb_iir_coefs1_l,&mc_filtes_l_old);
                }
                else
                {
                   iir_filter_coef_copy(anc_mc_iir_coefs1_l,&mc_filtes_l_old);
                   iir_filter_coef_copy(anc_fb_iir_coefs1_l,&fb_filtes_l_old);
                }

                anc_iir2_control->codec_iir2_iirb_enable=1;
                iir2_coef_using=1;
            }
            else
            {
                trace_counter=0;
                while(*codec_iir2_gaina_ext_out_ch0_sync>*codec_iir2_gain_ext_th
                        ||*codec_iir2_gaina_ext_out_ch1_sync>*codec_iir2_gain_ext_th)
                {
                    if(trace_counter%TRACE_COUNTER==0)
                    {

                        LOG_I("%s: 2 *codec_iir2_gaina_ext_out_ch0/1_sync:%d/%d",__func__,
                                *codec_iir2_gaina_ext_out_ch0_sync,
                                *codec_iir2_gaina_ext_out_ch1_sync);
                    }
                    trace_counter++;
                };

                if(anc_iir2_control->codec_iir2_iira_stop_status_sync==0)
                {
                    iir2_gaina_creat_stop_status();
                }

                anc_iir2_control->codec_iir2_ch1_bypass=mc_filtes_l_old.iir_bypass_flag;
                anc_iir2_control->codec_iir2_count_ch1=mc_filtes_l_old.iir_counter;

                if(anc_iir2_control->codec_iir2_ch0_bypass==1)
                {
                    iir_filter_coef_copy(anc_fb_iir_coefs0_l,&mc_filtes_l_old);
                }
                else
                {
                    iir_filter_coef_copy(anc_mc_iir_coefs0_l,&mc_filtes_l_old);
                    iir_filter_coef_copy(anc_fb_iir_coefs0_l,&fb_filtes_l_old);
                }

                anc_iir2_control->codec_iir2_iira_enable=1;
                iir2_coef_using=0;
            }
	    fb_ramp_coef_l=max_fb_gain_l;
	    mc_ramp_coef_l=max_mc_gain_l;

            lock = int_lock();
            anc_iir_gain_update->codec_iir2_gain_ext_update=0;
            hal_sys_timer_delay_us(1);
            anc_iir_gain_update->codec_iir2_gain_ext_update=3;
            int_unlock(lock);

            hwtimer_stop(anc_iir2_set_gain_timer);
            hwtimer_start(anc_iir2_set_gain_timer, anc_switching_gain_delay_fb_mc);
            max_mc_gain_l=(1<<GAIN_Q);
        }
#endif
        return err;
    }

#endif

    return err;
}

int32_t anc_set_calib_gain(enum ANC_TYPE_T type, int32_t gain)
{
    LOG_I( "[%s] type: %d, gain: %d", __func__, type, gain);
    // +/-6 dB
    if ((gain < 512 / 2) || (gain > 512 * 2)) {
        gain = 512;
        LOG_I( "[%s] WARNING: Invalid gain: %d. Use default value", __func__, gain);
    }

    if (type == ANC_FEEDFORWARD) {
        g_calib_ff_gain = gain;
    } else if (type == ANC_FEEDBACK) {
        g_calib_fb_gain = gain;
        g_calib_mc_gain = 512;//(512 * 512) / gain;
    } else if (type == ANC_TALKTHRU) {
        g_calib_tt_gain = gain;
    } else {
        ASSERT(0, "[%s] Invalid anc type: %d", __func__, type);
    }

    return 0;
}

int anc_set_cfg(const struct_anc_cfg * cfg, enum ANC_TYPE_T anc_type,ANC_GAIN_TIME anc_gain_delay)
{
    uint32_t lock;

    LOG_I("%s:0x%x", __func__,anc_type);

    if (cfg == NULL)
    {
        LOG_I("%s: cfg is null", __func__);
        return ANC_OTHER_ERR;
    }

    if ((anc_type&ANC_FEEDFORWARD && ff_open_flag==0)
    || (anc_type&ANC_FEEDBACK && fb_open_flag==0)
#if defined(AUDIO_ANC_TT_HW)
   ||  (anc_type&ANC_TALKTHRU && tt_open_flag==0)
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    || (anc_type&ANC_MUSICCANCLE && mc_open_flag==0)
#endif
     )
    {
        LOG_I("%s: anc_type=%d not opened", __func__, anc_type);
        return ANC_OTHER_ERR;
    }

    lock = int_lock();

    bool iir0_using_flag=anc_iir0_using_flag;
    bool iir2_using_flag=anc_iir2_using_flag;

    int iir0_type;
    int iir2_type;

    if(anc_type&ANC_FEEDFORWARD)
    {
        anc_cfg_iir0_type=anc_cfg_iir0_type|ANC_FEEDFORWARD;
        anc_cfg_coef_new.anc_cfg_ff_l=cfg->anc_cfg_ff_l;
        if (g_calib_ff_gain != CALIB_GAIN_INVALID) {
            anc_cfg_coef_new.anc_cfg_ff_l.total_gain = g_calib_ff_gain;
        }
    }

#if defined(AUDIO_ANC_TT_HW)
    if(anc_type&ANC_TALKTHRU)
    {
        anc_cfg_iir0_type=anc_cfg_iir0_type|ANC_TALKTHRU;
        anc_cfg_coef_new.anc_cfg_tt_l=cfg->anc_cfg_tt_l;
        if (g_calib_tt_gain != CALIB_GAIN_INVALID) {
            anc_cfg_coef_new.anc_cfg_tt_l.total_gain = g_calib_tt_gain;
        }
    }
#endif

    if(anc_type&ANC_FEEDBACK)
    {
        anc_cfg_iir2_type=anc_cfg_iir2_type|ANC_FEEDBACK;
        anc_cfg_coef_new.anc_cfg_fb_l=cfg->anc_cfg_fb_l;
        if (g_calib_fb_gain != CALIB_GAIN_INVALID) {
            anc_cfg_coef_new.anc_cfg_fb_l.total_gain = g_calib_fb_gain;
        }
    }

#if defined(AUDIO_ANC_FB_MC_HW)
    if(anc_type&ANC_MUSICCANCLE)
    {
        anc_cfg_iir2_type=anc_cfg_iir2_type|ANC_MUSICCANCLE;
        anc_cfg_coef_new.anc_cfg_mc_l=cfg->anc_cfg_mc_l;
        if (g_calib_mc_gain != CALIB_GAIN_INVALID) {
            anc_cfg_coef_new.anc_cfg_mc_l.total_gain = g_calib_mc_gain;
        }
    }
#endif

    iir0_type=anc_cfg_iir0_type;
    iir2_type=anc_cfg_iir2_type;

    if(anc_type&ANC_FEEDFORWARD||anc_type&ANC_TALKTHRU)
    {
        if(anc_iir0_using_flag==0)
        {
            anc_iir0_using_flag=1;
            anc_cfg_iir0_type=0;
            anc_iir0_reserve_flag=0;
            anc_cfg_coef.anc_cfg_ff_l=anc_cfg_coef_new.anc_cfg_ff_l;
            anc_cfg_coef.anc_cfg_tt_l=anc_cfg_coef_new.anc_cfg_tt_l;
        }
        else
        {
            anc_iir0_reserve_flag=1;
        }
    }

    if(anc_type&ANC_FEEDBACK||anc_type&ANC_MUSICCANCLE)
    {
        if(anc_iir2_using_flag==0)
        {
            anc_iir2_using_flag=1;
            anc_cfg_iir2_type=0;
            anc_iir2_reserve_flag=0;
            anc_cfg_coef.anc_cfg_fb_l=anc_cfg_coef_new.anc_cfg_fb_l;
            anc_cfg_coef.anc_cfg_mc_l=anc_cfg_coef_new.anc_cfg_mc_l;
        }
        else
        {
            anc_iir2_reserve_flag=1;
        }
    }
    int_unlock(lock);

    if(anc_type&ANC_FEEDFORWARD||anc_type&ANC_TALKTHRU)
    {
        if(iir0_using_flag==0)
        {

                LOG_I("%s:iir0_type:0x%x", __func__,iir0_type);

                anc_set_cfg_internal(&anc_cfg_coef,iir0_type);
                hwtimer_stop(anc_iir0_switching_timer);
                hwtimer_start(anc_iir0_switching_timer, anc_switching_coef_delay_ff_tt);
        }
    }
    if(anc_type&ANC_FEEDBACK||anc_type&ANC_MUSICCANCLE)
    {
        if(iir2_using_flag==0)
        {
                LOG_I("%s:iir0_type:0x%x", __func__,iir2_type);

                anc_set_cfg_internal(&anc_cfg_coef,iir2_type);
                hwtimer_stop(anc_iir2_switching_timer);
                hwtimer_start(anc_iir2_switching_timer, anc_switching_coef_delay_fb_mc);
        }
    }

    return 0;
}


static void anc_ctrl_reg_init(void)
{
    //enable  iir clock
    anc_iir_clock->en_clk_iir=0x3;

    //disable ANC;
    anc_control->codec_anc_enable_ch0=0;

    //using TT as FF ANC
    anc_path_set->codec_tt_out_off_ch0=1;
    anc_path_set->codec_tt_pdu_off_ch0=0;

   // anc_path_set->codec_pdu_off_ch0=1;
   // anc_path_set->codec_dehowl_en_ch0=1;

	anc_path_set->codec_pdu_mix_en_ch0=0;

    //set the FF calib gain;
    anc_iir_calib_gain_config->codec_anc_calib_gain_update_ff_ch0=0;
    anc_ff_iir_calib_gain->codec_anc_calib_gain_coef_ff_ch0=CALIB_GAIN_Q;
    anc_iir_calib_gain_config->codec_anc_calib_gain_update_ff_ch0=1;

    //set the FF calib gain;
    anc_iir_calib_gain_config->codec_anc_calib_gain_pass0_ff_ch0=1;

    //set the FB calib gain;
    anc_iir_calib_gain_config->codec_anc_calib_gain_update_fb_ch0=0;
    anc_fb_iir_calib_gain->codec_anc_calib_gain_coef_fb_ch0=CALIB_GAIN_Q;
    anc_iir_calib_gain_config->codec_anc_calib_gain_update_fb_ch0=1;

    //set the FB calib gain;
    anc_iir_calib_gain_config->codec_anc_calib_gain_pass0_fb_ch0=1;

#if defined(AUDIO_ANC_TT_HW)
    //set the TT calib gain;
    anc_iir_calib_gain_config->codec_anc_calib_gain_update_tt_ch0=0;
    anc_tt_iir_calib_gain->codec_anc_calib_gain_coef_tt_ch0=CALIB_GAIN_Q;
    anc_iir_calib_gain_config->codec_anc_calib_gain_update_tt_ch0=1;

    //set the TT calib gain;
    anc_iir_calib_gain_config->codec_anc_calib_gain_pass0_tt_ch0=1;

#endif

    tm_config->codec_tt_enable_ch0=0;
    //tm_config->codec_tt_adc_sel_ch0=0;

    tm_config->codec_mm_enable_ch0=0;

    anc_control->codec_anc_rate_sel=0;

    anc_control->codec_feedback_ch0=0;

    anc_control->codec_dual_anc_ch0=0;

    //8:-120dB 83:-100dB

    *codec_iir0_gain_ext_th=ANC_GAIN_RAMP_THRESHLD;
    *codec_iir2_gain_ext_th=ANC_GAIN_RAMP_THRESHLD;

    //iir0
    anc_iir0_control->codec_iir0_enable=0;
    anc_iir0_control->codec_iir0_iira_enable=0;
    anc_iir0_control->codec_iir0_iirb_enable=0;
    anc_iir0_control->codec_iir0_ch0_bypass=1;
    anc_iir0_control->codec_iir0_ch1_bypass=1;
    anc_iir0_control->codec_iir0_lmt_ch0_bypass=1;
    anc_iir0_control->codec_iir0_lmt_ch1_bypass=1;
    anc_iir0_control->codec_iir0_count_ch0=0;
    anc_iir0_control->codec_iir0_count_ch1=0;
    anc_iir0_control->codec_iir0_coef_swap=0;
    anc_iir0_control->codec_iir0_auto_stop=0;


    //iir2
    anc_iir2_control->codec_iir2_enable=0;
    anc_iir2_control->codec_iir2_iira_enable=0;
    anc_iir2_control->codec_iir2_iirb_enable=0;
    anc_iir2_control->codec_iir2_ch0_bypass=1;
    anc_iir2_control->codec_iir2_ch1_bypass=1;
    anc_iir2_control->codec_iir2_lmt_ch0_bypass=1;
    anc_iir2_control->codec_iir2_lmt_ch1_bypass=1;
    anc_iir2_control->codec_iir2_count_ch0=0;
    anc_iir2_control->codec_iir2_count_ch1=0;
    anc_iir2_control->codec_iir2_coef_swap=0;
    anc_iir2_control->codec_iir2_auto_stop=0;


    //unmute ANC.
    anc_control->codec_anc_mute_ch0=0;

    //disable gain updated when pass0
    anc_ff_gain->codec_anc_mute_gain_pass0_ff_ch0=1;

    anc_fb_gain->codec_anc_mute_gain_pass0_fb_ch0=1;

#if defined(AUDIO_ANC_TT_HW)
    anc_tt_gain->codec_anc_mute_gain_pass0_tt_ch0=1;
#endif

#if defined(AUDIO_ANC_FB_MC_HW)
    anc_mc_gain->codec_anc_mute_gain_pass0_mc_ch0=1;
#endif

#if defined(ANC_FB_CHECK) ||defined(ANC_FF_CHECK) ||defined(ANC_TT_CHECK)
    feedback_mc_settings->codec_fb_check_udc_ch0=6;
    fb_check_ch0_config->codec_fb_check_dcf_bypass_ch0=0;
#endif

    ff_ramp_coef_l=0;

    fb_ramp_coef_l=0;

    tt_ramp_coef_l=0;

    mc_ramp_coef_l=0;

#ifdef ANC_FAST_CALIB_MODE
    if (analog_debug_get_anc_calib_mode())
    {
        anc_switching_gain_delay_ff_tt=ANC_SET_GAIN_DELAY_SHORT;
        anc_switching_coef_delay_ff_tt=ANC_SET_GAIN_DELAY_SHORT+ANC_SET_GAIN_CALIB_TIME;

        anc_switching_gain_delay_fb_mc=ANC_SET_GAIN_DELAY_SHORT;
        anc_switching_coef_delay_fb_mc=ANC_SET_GAIN_DELAY_SHORT+ANC_SET_GAIN_CALIB_TIME;
    }
    else
#endif
    {
        anc_switching_gain_delay_ff_tt=ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay_ff_tt=ANC_SET_GAIN_DELAY_LONG+ANC_SET_GAIN_TIME;

        anc_switching_gain_delay_fb_mc=ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay_fb_mc=ANC_SET_GAIN_DELAY_LONG+ANC_SET_GAIN_TIME;
    }
    anc_cfg_iir0_type=0;
    anc_cfg_iir2_type=0;
    anc_iir0_using_flag=0;
    anc_iir2_using_flag=0;


    for(int i=0;i<IIR_COUNTER;i++)
    {
        ff_filtes_l_old.iir_coef[i].coef_a[0]=0;
        ff_filtes_l_old.iir_coef[i].coef_a[1]=0;
        ff_filtes_l_old.iir_coef[i].coef_a[2]=0;
        ff_filtes_l_old.iir_coef[i].coef_b[0]=0;
        ff_filtes_l_old.iir_coef[i].coef_b[1]=0;
        ff_filtes_l_old.iir_coef[i].coef_b[2]=0;
    }

    ff_filtes_l_old.total_gain=512;
    ff_filtes_l_old.iir_counter=IIR_FF_COUNTER;
    ff_filtes_l_old.iir_bypass_flag=0;

#if defined(AUDIO_ANC_TT_HW)
    for(int i=0;i<IIR_COUNTER;i++)
    {
        tt_filtes_l_old.iir_coef[i].coef_a[0]=0;
        tt_filtes_l_old.iir_coef[i].coef_a[1]=0;
        tt_filtes_l_old.iir_coef[i].coef_a[2]=0;
        tt_filtes_l_old.iir_coef[i].coef_b[0]=0;
        tt_filtes_l_old.iir_coef[i].coef_b[1]=0;
        tt_filtes_l_old.iir_coef[i].coef_b[2]=0;
    }

    tt_filtes_l_old.total_gain=512;
    tt_filtes_l_old.iir_counter=IIR_TT_COUNTER;
    tt_filtes_l_old.iir_bypass_flag=0;
#endif

    for(int i=0;i<IIR_COUNTER;i++)
    {
        fb_filtes_l_old.iir_coef[i].coef_a[0]=0;
        fb_filtes_l_old.iir_coef[i].coef_a[1]=0;
        fb_filtes_l_old.iir_coef[i].coef_a[2]=0;
        fb_filtes_l_old.iir_coef[i].coef_b[0]=0;
        fb_filtes_l_old.iir_coef[i].coef_b[1]=0;
        fb_filtes_l_old.iir_coef[i].coef_b[2]=0;
    }

    fb_filtes_l_old.total_gain=512;
    fb_filtes_l_old.iir_counter=IIR_FB_COUNTER;
    fb_filtes_l_old.iir_bypass_flag=0;

#if defined(AUDIO_ANC_FB_MC_HW)
    for(int i=0;i<IIR_COUNTER;i++)
    {
        mc_filtes_l_old.iir_coef[i].coef_a[0]=0;
        mc_filtes_l_old.iir_coef[i].coef_a[1]=0;
        mc_filtes_l_old.iir_coef[i].coef_a[2]=0;
        mc_filtes_l_old.iir_coef[i].coef_b[0]=0;
        mc_filtes_l_old.iir_coef[i].coef_b[1]=0;
        mc_filtes_l_old.iir_coef[i].coef_b[2]=0;
    }

    mc_filtes_l_old.total_gain=512;
    mc_filtes_l_old.iir_counter=IIR_MC_COUNTER;
    mc_filtes_l_old.iir_bypass_flag=0;
#endif

    return;
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
static void anc_iir0_init(void)
{
    anc_iir_coefs  iir_coef_gain_ramp_ff;
    anc_iir_coefs  iir_coef_gain_ramp_tt;

#ifdef ANC_FAST_CALIB_MODE
    if (analog_debug_get_anc_calib_mode())
    {
        iir_coef_gain_ramp_ff=iir_coef_gain_ramp_ff_calib;
        iir_coef_gain_ramp_tt=iir_coef_gain_ramp_tt_calib;
    }
    else
#endif
    {
        iir_coef_gain_ramp_ff=iir_coef_gain_ramp_ff_normal;
        iir_coef_gain_ramp_tt=iir_coef_gain_ramp_tt_normal;
    }

    anc_iir0_control->codec_iir0_enable=0;

    anc_iir0_control->codec_iir0_iira_enable=0;
    anc_iir0_control->codec_iir0_iirb_enable=0;

    anc_iir0_control->codec_iir0_coef_swap=0;
    anc_iir0_control->codec_iir0_auto_stop=0;

#ifdef ANC_GAIN_RAMP

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

    anc_iir0_control->codec_iir0_gaincal_ext_ch0_bypass=0;
    anc_iir0_control->codec_iir0_gainuse_ext_ch0_bypass=0;
    anc_iir_gain_update->codec_iir0_gain_ext_sel_ch0=0;

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

    anc_iir0_control->codec_iir0_gaincal_ext_ch1_bypass=0;
    anc_iir0_control->codec_iir0_gainuse_ext_ch1_bypass=0;
    anc_iir_gain_update->codec_iir0_gain_ext_sel_ch1=0;
#endif


#if defined(ANC_LIMITER_FF)
    anc_limiter_ff_iir_coefs0_l[0].a1=-iir_coef_limiter_attack_ff.coef_a[1];
    anc_limiter_ff_iir_coefs0_l[0].a2=-iir_coef_limiter_attack_ff.coef_a[2];
    anc_limiter_ff_iir_coefs0_l[0].b0=iir_coef_limiter_attack_ff.coef_b[0];
    anc_limiter_ff_iir_coefs0_l[0].b1=iir_coef_limiter_attack_ff.coef_b[1];
    anc_limiter_ff_iir_coefs0_l[0].b2=iir_coef_limiter_attack_ff.coef_b[2];

    anc_limiter_ff_iir_coefs0_l[1].a1=-iir_coef_limiter_release_ff.coef_a[1];
    anc_limiter_ff_iir_coefs0_l[1].a2=-iir_coef_limiter_release_ff.coef_a[2];
    anc_limiter_ff_iir_coefs0_l[1].b0=iir_coef_limiter_release_ff.coef_b[0];
    anc_limiter_ff_iir_coefs0_l[1].b1=iir_coef_limiter_release_ff.coef_b[1];
    anc_limiter_ff_iir_coefs0_l[1].b2=iir_coef_limiter_release_ff.coef_b[2];

    anc_limiter_ff_iir_coefs1_l[0].a1=-iir_coef_limiter_attack_ff.coef_a[1];
    anc_limiter_ff_iir_coefs1_l[0].a2=-iir_coef_limiter_attack_ff.coef_a[2];
    anc_limiter_ff_iir_coefs1_l[0].b0=iir_coef_limiter_attack_ff.coef_b[0];
    anc_limiter_ff_iir_coefs1_l[0].b1=iir_coef_limiter_attack_ff.coef_b[1];
    anc_limiter_ff_iir_coefs1_l[0].b2=iir_coef_limiter_attack_ff.coef_b[2];

    anc_limiter_ff_iir_coefs1_l[1].a1=-iir_coef_limiter_release_ff.coef_a[1];
    anc_limiter_ff_iir_coefs1_l[1].a2=-iir_coef_limiter_release_ff.coef_a[2];
    anc_limiter_ff_iir_coefs1_l[1].b0=iir_coef_limiter_release_ff.coef_b[0];
    anc_limiter_ff_iir_coefs1_l[1].b1=iir_coef_limiter_release_ff.coef_b[1];
    anc_limiter_ff_iir_coefs1_l[1].b2=iir_coef_limiter_release_ff.coef_b[2];

/*
#ifdef ANC_FAST_CALIB_MODE
    if (analog_debug_get_anc_calib_mode())
    {
        anc_iir0_control->codec_iir0_lmt_ch0_bypass=1;
    }
    else
#endif
*/
    {
        anc_iir0_control->codec_iir0_lmt_ch0_bypass=0;
    }

    //update the threshold
    anc_iir_gain_update->codec_iir0_lmt_th_update_ch0=0;
    //limiter threshold, 0: 0x7fffff
    *codec_iir0_lmt_th_ch0=ANC_LIMITER_FF_THREHOLD;
    hal_sys_timer_delay_us(1);
    anc_iir_gain_update->codec_iir0_lmt_th_update_ch0=1;

    anc_iir_lmt_delay->codec_iir0_lmt_delay=ANC_LIMITER_FF_ATTACK_DELAY;
#else
    anc_iir0_control->codec_iir0_lmt_ch0_bypass=1;
#endif

#if defined(ANC_LIMITER_TT) && defined(AUDIO_ANC_TT_HW)
    anc_limiter_tt_iir_coefs0_l[0].a1=-iir_coef_limiter_attack_tt.coef_a[1];
    anc_limiter_tt_iir_coefs0_l[0].a2=-iir_coef_limiter_attack_tt.coef_a[2];
    anc_limiter_tt_iir_coefs0_l[0].b0=iir_coef_limiter_attack_tt.coef_b[0];
    anc_limiter_tt_iir_coefs0_l[0].b1=iir_coef_limiter_attack_tt.coef_b[1];
    anc_limiter_tt_iir_coefs0_l[0].b2=iir_coef_limiter_attack_tt.coef_b[2];

    anc_limiter_tt_iir_coefs0_l[1].a1=-iir_coef_limiter_release_tt.coef_a[1];
    anc_limiter_tt_iir_coefs0_l[1].a2=-iir_coef_limiter_release_tt.coef_a[2];
    anc_limiter_tt_iir_coefs0_l[1].b0=iir_coef_limiter_release_tt.coef_b[0];
    anc_limiter_tt_iir_coefs0_l[1].b1=iir_coef_limiter_release_tt.coef_b[1];
    anc_limiter_tt_iir_coefs0_l[1].b2=iir_coef_limiter_release_tt.coef_b[2];

    anc_limiter_tt_iir_coefs1_l[0].a1=-iir_coef_limiter_attack_tt.coef_a[1];
    anc_limiter_tt_iir_coefs1_l[0].a2=-iir_coef_limiter_attack_tt.coef_a[2];
    anc_limiter_tt_iir_coefs1_l[0].b0=iir_coef_limiter_attack_tt.coef_b[0];
    anc_limiter_tt_iir_coefs1_l[0].b1=iir_coef_limiter_attack_tt.coef_b[1];
    anc_limiter_tt_iir_coefs1_l[0].b2=iir_coef_limiter_attack_tt.coef_b[2];

    anc_limiter_tt_iir_coefs1_l[1].a1=-iir_coef_limiter_release_tt.coef_a[1];
    anc_limiter_tt_iir_coefs1_l[1].a2=-iir_coef_limiter_release_tt.coef_a[2];
    anc_limiter_tt_iir_coefs1_l[1].b0=iir_coef_limiter_release_tt.coef_b[0];
    anc_limiter_tt_iir_coefs1_l[1].b1=iir_coef_limiter_release_tt.coef_b[1];
    anc_limiter_tt_iir_coefs1_l[1].b2=iir_coef_limiter_release_tt.coef_b[2];
/*
#ifdef ANC_FAST_CALIB_MODE
    if (analog_debug_get_anc_calib_mode())
    {
        anc_iir0_control->codec_iir0_lmt_ch1_bypass=1;
    }
    else
#endif
*/
    {
        anc_iir0_control->codec_iir0_lmt_ch1_bypass=0;
    }

    //update the threshold
    anc_iir_gain_update->codec_iir0_lmt_th_update_ch1=0;
    //limiter threshold, 0: 0x7fffff
    *codec_iir0_lmt_th_ch1=ANC_LIMITER_TT_THREHOLD;
    hal_sys_timer_delay_us(1);
    anc_iir_gain_update->codec_iir0_lmt_th_update_ch1=1;

    anc_iir_lmt_delay->codec_iir0_lmt_delay=ANC_LIMITER_TT_ATTACK_DELAY;
#else
    anc_iir0_control->codec_iir0_lmt_ch1_bypass=1;
#endif

    for(int i=0;i<IIR_COUNTER;i++)
    {
        anc_ff_iir_coefs0_l[i].a1=0;
        anc_ff_iir_coefs0_l[i].a2=0;
        anc_ff_iir_coefs0_l[i].b0=0;
        anc_ff_iir_coefs0_l[i].b1=0;
        anc_ff_iir_coefs0_l[i].b2=0;

        anc_ff_iir_coefs1_l[i].a1=0;
        anc_ff_iir_coefs1_l[i].a2=0;
        anc_ff_iir_coefs1_l[i].b0=0;
        anc_ff_iir_coefs1_l[i].b1=0;
        anc_ff_iir_coefs1_l[i].b2=0;

        anc_tt_iir_coefs0_l[i].a1=0;
        anc_tt_iir_coefs0_l[i].a2=0;
        anc_tt_iir_coefs0_l[i].b0=0;
        anc_tt_iir_coefs0_l[i].b1=0;
        anc_tt_iir_coefs0_l[i].b2=0;

        anc_tt_iir_coefs1_l[i].a1=0;
        anc_tt_iir_coefs1_l[i].a2=0;
        anc_tt_iir_coefs1_l[i].b0=0;
        anc_tt_iir_coefs1_l[i].b1=0;
        anc_tt_iir_coefs1_l[i].b2=0;
    }
    anc_iir0_control->codec_iir0_ch0_bypass=0;
    anc_iir0_control->codec_iir0_count_ch0=IIR_FF_COUNTER;

#if defined(AUDIO_ANC_TT_HW)
    anc_iir0_control->codec_iir0_ch1_bypass=0;
#else
    anc_iir0_control->codec_iir0_ch1_bypass=1;
#endif
    anc_iir0_control->codec_iir0_count_ch1=IIR_TT_COUNTER;

    anc_iir0_control->codec_iir0_auto_stop=1;
    anc_iir0_control->codec_iir0_iira_enable=1;
    anc_iir0_control->codec_iir0_iirb_enable=1;
    anc_iir0_control->codec_iir0_enable=1;


    iir0_coef_using=0;
}


static void anc_iir2_init(void)
{
    anc_iir_coefs  iir_coef_gain_ramp_fb;
    anc_iir_coefs  iir_coef_gain_ramp_mc;

#ifdef ANC_FAST_CALIB_MODE
    if (analog_debug_get_anc_calib_mode())
    {
        iir_coef_gain_ramp_fb=iir_coef_gain_ramp_fb_calib;
        iir_coef_gain_ramp_mc=iir_coef_gain_ramp_mc_calib;
    }
    else
#endif
    {
        iir_coef_gain_ramp_fb=iir_coef_gain_ramp_fb_normal;
        iir_coef_gain_ramp_mc=iir_coef_gain_ramp_mc_normal;
    }

    anc_iir2_control->codec_iir2_enable=0;

    anc_iir2_control->codec_iir2_iira_enable=0;
    anc_iir2_control->codec_iir2_iirb_enable=0;

    anc_iir2_control->codec_iir2_coef_swap=0;
    anc_iir2_control->codec_iir2_auto_stop=0;

#ifdef ANC_GAIN_RAMP
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

    anc_iir2_control->codec_iir2_gaincal_ext_ch0_bypass=0;
    anc_iir2_control->codec_iir2_gainuse_ext_ch0_bypass=0;
    anc_iir_gain_update->codec_iir2_gain_ext_sel_ch0=0;

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

    anc_iir2_control->codec_iir2_gaincal_ext_ch1_bypass=0;
    anc_iir2_control->codec_iir2_gainuse_ext_ch1_bypass=0;
    anc_iir_gain_update->codec_iir2_gain_ext_sel_ch1=0;
#endif

#if defined(ANC_LIMITER_FB)
    anc_limiter_fb_iir_coefs0_l[0].a1=-iir_coef_limiter_attack_fb.coef_a[1];
    anc_limiter_fb_iir_coefs0_l[0].a2=-iir_coef_limiter_attack_fb.coef_a[2];
    anc_limiter_fb_iir_coefs0_l[0].b0=iir_coef_limiter_attack_fb.coef_b[0];
    anc_limiter_fb_iir_coefs0_l[0].b1=iir_coef_limiter_attack_fb.coef_b[1];
    anc_limiter_fb_iir_coefs0_l[0].b2=iir_coef_limiter_attack_fb.coef_b[2];

    anc_limiter_fb_iir_coefs0_l[1].a1=-iir_coef_limiter_release_fb.coef_a[1];
    anc_limiter_fb_iir_coefs0_l[1].a2=-iir_coef_limiter_release_fb.coef_a[2];
    anc_limiter_fb_iir_coefs0_l[1].b0=iir_coef_limiter_release_fb.coef_b[0];
    anc_limiter_fb_iir_coefs0_l[1].b1=iir_coef_limiter_release_fb.coef_b[1];
    anc_limiter_fb_iir_coefs0_l[1].b2=iir_coef_limiter_release_fb.coef_b[2];

    anc_limiter_fb_iir_coefs1_l[0].a1=-iir_coef_limiter_attack_fb.coef_a[1];
    anc_limiter_fb_iir_coefs1_l[0].a2=-iir_coef_limiter_attack_fb.coef_a[2];
    anc_limiter_fb_iir_coefs1_l[0].b0=iir_coef_limiter_attack_fb.coef_b[0];
    anc_limiter_fb_iir_coefs1_l[0].b1=iir_coef_limiter_attack_fb.coef_b[1];
    anc_limiter_fb_iir_coefs1_l[0].b2=iir_coef_limiter_attack_fb.coef_b[2];

    anc_limiter_fb_iir_coefs1_l[1].a1=-iir_coef_limiter_release_fb.coef_a[1];
    anc_limiter_fb_iir_coefs1_l[1].a2=-iir_coef_limiter_release_fb.coef_a[2];
    anc_limiter_fb_iir_coefs1_l[1].b0=iir_coef_limiter_release_fb.coef_b[0];
    anc_limiter_fb_iir_coefs1_l[1].b1=iir_coef_limiter_release_fb.coef_b[1];
    anc_limiter_fb_iir_coefs1_l[1].b2=iir_coef_limiter_release_fb.coef_b[2];
/*
#ifdef ANC_FAST_CALIB_MODE
    if (analog_debug_get_anc_calib_mode())
    {
        anc_iir2_control->codec_iir2_lmt_ch0_bypass=1;
    }
    else
#endif
*/
    {
        anc_iir2_control->codec_iir2_lmt_ch0_bypass=0;
    }

    //update the threshold
    anc_iir_gain_update->codec_iir2_lmt_th_update_ch0=0;
    //limiter threshold, 0: 0x7fffff
    *codec_iir2_lmt_th_ch0=ANC_LIMITER_FB_THREHOLD;
    hal_sys_timer_delay_us(1);
    anc_iir_gain_update->codec_iir2_lmt_th_update_ch0=1;

    anc_iir_lmt_delay->codec_iir2_lmt_delay=ANC_LIMITER_FB_ATTACK_DELAY;
#else
    anc_iir2_control->codec_iir2_lmt_ch0_bypass=1;
#endif

#if defined(AUDIO_ANC_FB_MC_HW) &&defined(ANC_LIMITER_MC)
    anc_limiter_mc_iir_coefs0_l[0].a1=-iir_coef_limiter_attack_mc.coef_a[1];
    anc_limiter_mc_iir_coefs0_l[0].a2=-iir_coef_limiter_attack_mc.coef_a[2];
    anc_limiter_mc_iir_coefs0_l[0].b0=iir_coef_limiter_attack_mc.coef_b[0];
    anc_limiter_mc_iir_coefs0_l[0].b1=iir_coef_limiter_attack_mc.coef_b[1];
    anc_limiter_mc_iir_coefs0_l[0].b2=iir_coef_limiter_attack_mc.coef_b[2];

    anc_limiter_mc_iir_coefs0_l[1].a1=-iir_coef_limiter_release_mc.coef_a[1];
    anc_limiter_mc_iir_coefs0_l[1].a2=-iir_coef_limiter_release_mc.coef_a[2];
    anc_limiter_mc_iir_coefs0_l[1].b0=iir_coef_limiter_release_mc.coef_b[0];
    anc_limiter_mc_iir_coefs0_l[1].b1=iir_coef_limiter_release_mc.coef_b[1];
    anc_limiter_mc_iir_coefs0_l[1].b2=iir_coef_limiter_release_mc.coef_b[2];

    anc_limiter_mc_iir_coefs1_l[0].a1=-iir_coef_limiter_attack_mc.coef_a[1];
    anc_limiter_mc_iir_coefs1_l[0].a2=-iir_coef_limiter_attack_mc.coef_a[2];
    anc_limiter_mc_iir_coefs1_l[0].b0=iir_coef_limiter_attack_mc.coef_b[0];
    anc_limiter_mc_iir_coefs1_l[0].b1=iir_coef_limiter_attack_mc.coef_b[1];
    anc_limiter_mc_iir_coefs1_l[0].b2=iir_coef_limiter_attack_mc.coef_b[2];

    anc_limiter_mc_iir_coefs1_l[1].a1=-iir_coef_limiter_release_mc.coef_a[1];
    anc_limiter_mc_iir_coefs1_l[1].a2=-iir_coef_limiter_release_mc.coef_a[2];
    anc_limiter_mc_iir_coefs1_l[1].b0=iir_coef_limiter_release_mc.coef_b[0];
    anc_limiter_mc_iir_coefs1_l[1].b1=iir_coef_limiter_release_mc.coef_b[1];
    anc_limiter_mc_iir_coefs1_l[1].b2=iir_coef_limiter_release_mc.coef_b[2];
/*
#ifdef ANC_FAST_CALIB_MODE
    if (analog_debug_get_anc_calib_mode())
    {
        anc_iir2_control->codec_iir2_lmt_ch1_bypass=1;
    }
    else
#endif
*/
    {
        anc_iir2_control->codec_iir2_lmt_ch1_bypass=0;
    }

    //update the threshold
    anc_iir_gain_update->codec_iir2_lmt_th_update_ch1=0;
    //limiter threshold, 0: 0x7fffff
    *codec_iir2_lmt_th_ch1=ANC_LIMITER_MC_THREHOLD;
    hal_sys_timer_delay_us(1);
    anc_iir_gain_update->codec_iir2_lmt_th_update_ch1=1;

    anc_iir_lmt_delay->codec_iir2_lmt_delay=ANC_LIMITER_MC_ATTACK_DELAY;
#else
    anc_iir2_control->codec_iir2_lmt_ch1_bypass=1;
#endif

    for(int i=0;i<IIR_COUNTER;i++)
    {
        anc_fb_iir_coefs0_l[i].a1=0;
        anc_fb_iir_coefs0_l[i].a2=0;
        anc_fb_iir_coefs0_l[i].b0=0;
        anc_fb_iir_coefs0_l[i].b1=0;
        anc_fb_iir_coefs0_l[i].b2=0;

        anc_fb_iir_coefs1_l[i].a1=0;
        anc_fb_iir_coefs1_l[i].a2=0;
        anc_fb_iir_coefs1_l[i].b0=0;
        anc_fb_iir_coefs1_l[i].b1=0;
        anc_fb_iir_coefs1_l[i].b2=0;

        anc_mc_iir_coefs0_l[i].a1=0;
        anc_mc_iir_coefs0_l[i].a2=0;
        anc_mc_iir_coefs0_l[i].b0=0;
        anc_mc_iir_coefs0_l[i].b1=0;
        anc_mc_iir_coefs0_l[i].b2=0;

        anc_mc_iir_coefs1_l[i].a1=0;
        anc_mc_iir_coefs1_l[i].a2=0;
        anc_mc_iir_coefs1_l[i].b0=0;
        anc_mc_iir_coefs1_l[i].b1=0;
        anc_mc_iir_coefs1_l[i].b2=0;
    }

    anc_iir2_control->codec_iir2_ch0_bypass=0;
    anc_iir2_control->codec_iir2_count_ch0=IIR_FB_COUNTER;

#if defined(AUDIO_ANC_FB_MC_HW)
    anc_iir2_control->codec_iir2_ch1_bypass=0;
#else
    anc_iir2_control->codec_iir2_ch1_bypass=1;
#endif
    anc_iir2_control->codec_iir2_count_ch1=IIR_MC_COUNTER;

    anc_iir2_control->codec_iir2_auto_stop=1;
    anc_iir2_control->codec_iir2_iirb_enable=1;
    anc_iir2_control->codec_iir2_iira_enable=1;
    anc_iir2_control->codec_iir2_enable=1;

    iir2_coef_using=0;
}

static void anc_ctrl_reg_open(enum ANC_TYPE_T anc_type)
{
    LOG_I("%s", __func__);

    if(anc_type==ANC_FEEDFORWARD)
    {
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            //set the FF gain;
#ifdef ANC_GAIN_RAMP
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch0=0;
            anc_ff_gain->codec_anc_mute_gain_ff_ch0=512;
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch0=1;
            ff_ramp_gain_l=0;
	     ff_ramp_coef_l=0;
#else
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch0=0;
            anc_ff_gain->codec_anc_mute_gain_ff_ch0=0;
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch0=1;
#endif

            for(int i=0;i<IIR_COUNTER;i++)
            {
                ff_filtes_l_old.iir_coef[i].coef_a[0]=0;
                ff_filtes_l_old.iir_coef[i].coef_a[1]=0;
                ff_filtes_l_old.iir_coef[i].coef_a[2]=0;
                ff_filtes_l_old.iir_coef[i].coef_b[0]=0;
                ff_filtes_l_old.iir_coef[i].coef_b[1]=0;
                ff_filtes_l_old.iir_coef[i].coef_b[2]=0;
            }

            ff_filtes_l_old.total_gain=512;
            ff_filtes_l_old.iir_counter=IIR_COUNTER;
            ff_filtes_l_old.iir_bypass_flag=0;


            if(tt_open_flag==0)
            {
                anc_iir0_init();
            }

        }
    }

#if defined(AUDIO_ANC_TT_HW)
    if(anc_type==ANC_TALKTHRU)
    {
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            //set the TT gain;
#ifdef ANC_GAIN_RAMP
            anc_tt_gain->codec_anc_mute_gain_update_tt_ch0=0;
            anc_tt_gain->codec_anc_mute_gain_tt_ch0=512;
            anc_tt_gain->codec_anc_mute_gain_update_tt_ch0=1;
            tt_ramp_gain_l=0;
            tt_ramp_coef_l=0;
#else
            anc_tt_gain->codec_anc_mute_gain_update_tt_ch0=0;
            anc_tt_gain->codec_anc_mute_gain_tt_ch0=0;
            anc_tt_gain->codec_anc_mute_gain_update_tt_ch0=1;
#endif

            for(int i=0;i<IIR_COUNTER;i++)
            {
                tt_filtes_l_old.iir_coef[i].coef_a[0]=0;
                tt_filtes_l_old.iir_coef[i].coef_a[1]=0;
                tt_filtes_l_old.iir_coef[i].coef_a[2]=0;
                tt_filtes_l_old.iir_coef[i].coef_b[0]=0;
                tt_filtes_l_old.iir_coef[i].coef_b[1]=0;
                tt_filtes_l_old.iir_coef[i].coef_b[2]=0;
            }

            tt_filtes_l_old.total_gain=512;
            tt_filtes_l_old.iir_counter=IIR_COUNTER;
            tt_filtes_l_old.iir_bypass_flag=0;

            if(ff_open_flag==0)
            {
                anc_iir0_init();
            }
            tm_config->codec_tt_enable_ch0=1;
        }
    }
#endif

    if(anc_type==ANC_FEEDBACK)
    {
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            //set the FB gain;
#ifdef ANC_GAIN_RAMP
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch0=0;
            anc_fb_gain->codec_anc_mute_gain_fb_ch0=512;
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch0=1;
            fb_ramp_gain_l=0;
            fb_ramp_coef_l=0;
#else
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch0=0;
            anc_fb_gain->codec_anc_mute_gain_fb_ch0=0;
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch0=1;
#endif

            for(int i=0;i<IIR_COUNTER;i++)
            {
                fb_filtes_l_old.iir_coef[i].coef_a[0]=0;
                fb_filtes_l_old.iir_coef[i].coef_a[1]=0;
                fb_filtes_l_old.iir_coef[i].coef_a[2]=0;
                fb_filtes_l_old.iir_coef[i].coef_b[0]=0;
                fb_filtes_l_old.iir_coef[i].coef_b[1]=0;
                fb_filtes_l_old.iir_coef[i].coef_b[2]=0;
            }

            fb_filtes_l_old.total_gain=512;
            fb_filtes_l_old.iir_counter=IIR_COUNTER;
            fb_filtes_l_old.iir_bypass_flag=0;

            if(mc_open_flag==0)
            {
                anc_iir2_init();
            }
#if defined(AUDIO_ANC_FB_MC) ||defined(AUDIO_ANC_FB_MC_HW)
            if (analog_debug_get_anc_calib_mode())
            {
                 fb_check_ch0_config->codec_fb_check_keep_ch0=1;
            }
            else
            {
//                 fb_check_ch0_config->codec_fb_check_keep_ch0=0;
            }

            feedback_mc_settings->codec_feedback_mc_en_ch0=1;
#endif

        }
    }

#if defined(AUDIO_ANC_FB_MC_HW)
    if(anc_type==ANC_MUSICCANCLE)
    {
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            //set the MC gain;
#ifdef ANC_GAIN_RAMP
            anc_mc_gain->codec_anc_mute_gain_update_mc_ch0=0;
            anc_mc_gain->codec_anc_mute_gain_mc_ch0=512;
            anc_mc_gain->codec_anc_mute_gain_update_mc_ch0=1;

            mc_ramp_gain_l=0;
            mc_ramp_coef_l=0;
#else
            anc_mc_gain->codec_anc_mute_gain_update_mc_ch0=0;
            anc_mc_gain->codec_anc_mute_gain_mc_ch0=0;
            anc_mc_gain->codec_anc_mute_gain_update_mc_ch0=1;
#endif

            for(int i=0;i<IIR_COUNTER;i++)
            {
                mc_filtes_l_old.iir_coef[i].coef_a[0]=0;
                mc_filtes_l_old.iir_coef[i].coef_a[1]=0;
                mc_filtes_l_old.iir_coef[i].coef_a[2]=0;
                mc_filtes_l_old.iir_coef[i].coef_b[0]=0;
                mc_filtes_l_old.iir_coef[i].coef_b[1]=0;
                mc_filtes_l_old.iir_coef[i].coef_b[2]=0;
            }

            mc_filtes_l_old.total_gain=512;
            mc_filtes_l_old.iir_counter=IIR_COUNTER;
            mc_filtes_l_old.iir_bypass_flag=0;

            if(fb_open_flag==0)
            {
                anc_iir2_init();
            }


            //set MC delay.
            tm_config->codec_mm_fifo_bypass_ch0=1;
            /*
               tm_config->codec_mm_delay_ch0=4;
            //  tm_config->codec_mm_delay_update=0;
            //  tm_config->codec_mm_delay_update=1;
            tm_config->codec_mm_fifo_en_ch0=1;
            */
            tm_config->codec_mm_enable_ch0=1;
        }
    }
#endif
    if((ff_open_flag==1 || tt_open_flag==1)&&fb_open_flag==1)
    {
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            anc_control->codec_feedback_ch0=1;
            anc_control->codec_dual_anc_ch0=1;
        }
    }
    else if(fb_open_flag==1)
    {
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            anc_control->codec_feedback_ch0=1;
            anc_control->codec_dual_anc_ch0=0;
        }
    }
    else if(fb_open_flag==0)
    {
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            anc_control->codec_feedback_ch0=0;
            anc_control->codec_dual_anc_ch0=0;
        }
    }

    if((ff_open_flag==1) || (fb_open_flag==1) || (tt_open_flag==1))
    {
        anc_control->codec_anc_enable_ch0=1;
    }

}


static void anc_ctrl_reg_close(enum ANC_TYPE_T anc_type)
{
    if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
    {
        if(anc_type==ANC_FEEDFORWARD)
        {
            //set the FF gain;
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch0=0;
            anc_ff_gain->codec_anc_mute_gain_ff_ch0=0;
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch0=1;
        }
#if defined(AUDIO_ANC_TT_HW)
        if(anc_type==ANC_TALKTHRU)
        {
            //set the TT gain;
            anc_tt_gain->codec_anc_mute_gain_update_tt_ch0=0;
            anc_tt_gain->codec_anc_mute_gain_tt_ch0=0;
            anc_tt_gain->codec_anc_mute_gain_update_tt_ch0=1;
        }
#endif
        if(anc_type==ANC_FEEDBACK)
        {
            //set the FB gain;
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch0=0;
            anc_fb_gain->codec_anc_mute_gain_fb_ch0=0;
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch0=1;

#if defined(AUDIO_ANC_FB_MC) ||defined(AUDIO_ANC_FB_MC_HW)
	     fb_check_ch0_config->codec_fb_check_keep_ch0=0;
            feedback_mc_settings->codec_feedback_mc_en_ch0=0;
#endif
        }
#if defined(AUDIO_ANC_FB_MC_HW)
        if(anc_type==ANC_MUSICCANCLE)
        {
            //set the TT gain;
            anc_mc_gain->codec_anc_mute_gain_update_mc_ch0=0;
            anc_mc_gain->codec_anc_mute_gain_mc_ch0=0;
            anc_mc_gain->codec_anc_mute_gain_update_mc_ch0=1;
        }
#endif

		if( (ff_open_flag==1 || tt_open_flag==1) && (fb_open_flag==1))
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

		if((ff_open_flag==0) && (fb_open_flag==0) && (tt_open_flag==0))
		{
			anc_control->codec_anc_enable_ch0=0;
		}
    }
}

int anc_opened(enum ANC_TYPE_T anc_type)
{
    int32_t open_flag=0;

    if (anc_type & ANC_FEEDFORWARD)
    {
        open_flag=open_flag|(ff_open_flag<<1);
    }
    if (anc_type & ANC_FEEDBACK)
    {
        open_flag=open_flag|(fb_open_flag<<2);
    }
#if defined(AUDIO_ANC_TT_HW)
    if (anc_type & ANC_TALKTHRU)
    {
        open_flag=open_flag|(tt_open_flag<<3);
    }
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    if (anc_type & ANC_MUSICCANCLE)
    {
        open_flag=open_flag|(mc_open_flag<<4);
    }
#endif
    return open_flag;
}

#if defined(ANC_FB_CHECK) ||defined(ANC_FF_CHECK)||defined(ANC_TT_CHECK)
#define HOWLING_CHECK_DELAY ((MS_TO_TICKS(5000)))
#define HOWLING_TIMER (400)

#define HOWLING_GAIN_0dB (512)
#define HOWLING_GAIN_m3dB (362)
#define HOWLING_GAIN_m6dB (256)
#define HOWLING_GAIN_m9dB (181)
#define HOWLING_GAIN_m12dB (128)

static void anc_howling_check_irq_handler(uint32_t status)
{
    //LOG_I("%s,", __func__);
    struct _codec_int_config *codec_int_config = (struct _codec_int_config *)&status;
    uint32 howling_time=TICKS_TO_MS(hal_sys_timer_get());

    if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
    {
        if(codec_int_config->fb_check_error_trig_ch0)
        {
            LOG_I("howling_cnt_l:%d",howling_cnt_l);

            LOG_I("*Threshold:%10d,Data Energy :%10d",*codec_fb_check_threshold_ch0,*codec_fb_check_data_avg_keep_ch0);

            if(howling_cnt_l==0)
            {
#if defined(ANC_FF_CHECK)
                ff_howling_gain_l=HOWLING_GAIN_m3dB;
#elif defined(ANC_FB_CHECK)
                fb_howling_gain_l=HOWLING_GAIN_m3dB;
#elif defined(ANC_TT_CHECK)
				tt_howling_gain_l=HOWLING_GAIN_m3dB;
#endif
                howling_cnt_l=1;
                howling_time_l=howling_time;
            }
            else if (howling_cnt_l==1&&(howling_time>howling_time_l+HOWLING_TIMER))
            {
#if defined(ANC_FF_CHECK)
                ff_howling_gain_l=HOWLING_GAIN_m6dB;
#elif defined(ANC_FB_CHECK)
				fb_howling_gain_l=HOWLING_GAIN_m6dB;
#elif defined(ANC_TT_CHECK)
                tt_howling_gain_l=HOWLING_GAIN_m6dB;
#endif
                howling_cnt_l=2;
                howling_time_l=howling_time;
            }
            else if (howling_cnt_l==2&&(howling_time>howling_time_l+HOWLING_TIMER))
            {
#if defined(ANC_FF_CHECK)
                ff_howling_gain_l=HOWLING_GAIN_m9dB;
#elif defined(ANC_FB_CHECK)
                fb_howling_gain_l=HOWLING_GAIN_m9dB;
#elif defined(ANC_TT_CHECK)
                tt_howling_gain_l=HOWLING_GAIN_m9dB;
#endif
                howling_cnt_l=3;
                howling_time_l=howling_time;
            }
            else if(howling_cnt_l==3&&(howling_time>howling_time_l+HOWLING_TIMER))
            {
#if defined(ANC_FF_CHECK)
                ff_howling_gain_l=0;
#elif defined(ANC_FB_CHECK)
                fb_howling_gain_l=0;
#elif defined(ANC_TT_CHECK)
                tt_howling_gain_l=0;
#endif
                howling_cnt_l=4;
            }

#if defined(ANC_FF_CHECK)
            LOG_I("ff_howling_gain_l:%d",ff_howling_gain_l);
#elif defined(ANC_FB_CHECK)
            LOG_I("fb_howling_gain_l:%d",fb_howling_gain_l);
#elif defined(ANC_TT_CHECK)
            LOG_I("tt_howling_gain_l:%d",tt_howling_gain_l);
#endif

            fb_check_ch0_config->codec_fb_check_enable_ch0=0;
            hwtimer_stop(anc_howling_check_dev_timer);
            hwtimer_start(anc_howling_check_dev_timer, HOWLING_CHECK_DELAY);
#if defined(ANC_FF_CHECK)||defined(ANC_TT_CHECK)
            if(iir0_coef_using==0)
            {
                iir0_gaina_cfg_gain();
            }
            else
            {
                iir0_gainb_cfg_gain();
            }
#elif defined(ANC_FB_CHECK)
            if(iir2_coef_using==0)
            {
                iir2_gaina_cfg_gain();
            }
            else
            {
                iir2_gainb_cfg_gain();
            }
#endif
            fb_check_ch0_config->codec_fb_check_enable_ch0=1;
        }
    }
}

static void anc_howling_check_timer_handler(void *param)
{
    LOG_I("%s", __func__);
#if defined(ANC_FF_CHECK)
    ff_howling_gain_l=HOWLING_GAIN_0dB;

    if(iir0_coef_using==0)
    {
        iir0_gaina_cfg_gain();
    }
    else
    {
        iir0_gainb_cfg_gain();
    }
#elif defined(ANC_TT_CHECK)
	tt_howling_gain_l=HOWLING_GAIN_0dB;

    if(iir0_coef_using==0)
    {
        iir0_gaina_cfg_gain();
    }
    else
    {
        iir0_gainb_cfg_gain();
    }
#elif defined(ANC_FB_CHECK)
    fb_howling_gain_l=HOWLING_GAIN_0dB;

    if(iir2_coef_using==0)
    {
        iir2_gaina_cfg_gain();
    }
    else
    {
        iir2_gainb_cfg_gain();
    }
#endif
    howling_cnt_l=0;
}
#endif

static void anc_iir0_set_gain_timer_handler(void *param)
{
    LOG_I("%s", __func__);
    if(iir0_coef_using==0)
    {
        iir0_gaina_cfg_gain();
    }
    else
    {
        iir0_gainb_cfg_gain();
    }
}

static void anc_iir2_set_gain_timer_handler(void *param)
{
    LOG_I("%s", __func__);
    if(iir2_coef_using==0)
    {
        iir2_gaina_cfg_gain();
    }
    else
    {
        iir2_gainb_cfg_gain();
    }
}

static void anc_iir0_switching_timer_handler(void *param)
{
    LOG_I("%s", __func__);

    uint32_t lock;
    lock = int_lock();

    bool iir0_reserve_flag=anc_iir0_reserve_flag;
    int iir0_type=anc_cfg_iir0_type;

    if(anc_iir0_reserve_flag)
    {
        anc_cfg_coef.anc_cfg_ff_l=anc_cfg_coef_new.anc_cfg_ff_l;
        anc_cfg_coef.anc_cfg_tt_l=anc_cfg_coef_new.anc_cfg_tt_l;
        anc_iir0_reserve_flag=0;
        anc_cfg_iir0_type=0;
    }
    else
    {
        anc_iir0_using_flag=0;
    }
    int_unlock(lock);

    if(iir0_reserve_flag)
    {
        anc_set_cfg_internal(&anc_cfg_coef,iir0_type);

        hwtimer_stop(anc_iir0_switching_timer);
        hwtimer_start(anc_iir0_switching_timer, anc_switching_coef_delay_ff_tt);
    }
    return;
}

static void anc_iir2_switching_timer_handler(void *param)
{
    LOG_I("%s", __func__);
    uint32_t lock;
    lock = int_lock();

    bool iir2_reserve_flag=anc_iir2_reserve_flag;
    int iir2_type=anc_cfg_iir2_type;

    if(anc_iir2_reserve_flag)
    {
        anc_cfg_coef.anc_cfg_fb_l=anc_cfg_coef_new.anc_cfg_fb_l;
        anc_cfg_coef.anc_cfg_mc_l=anc_cfg_coef_new.anc_cfg_mc_l;
        anc_iir2_reserve_flag=0;
        anc_cfg_iir2_type=0;
    }
    else
    {
        anc_iir2_using_flag=0;
    }
    int_unlock(lock);

    if(iir2_reserve_flag)
    {
        anc_set_cfg_internal(&anc_cfg_coef,iir2_type);

        hwtimer_stop(anc_iir2_switching_timer);
        hwtimer_start(anc_iir2_switching_timer, anc_switching_coef_delay_fb_mc);
    }
    return;
}


int anc_open(enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err=ANC_NO_ERR;

    LOG_I("%s", __func__);

    if(anc_type==ANC_FEEDFORWARD&&ff_open_flag==1)return err;
    if(anc_type==ANC_FEEDBACK&&fb_open_flag==1)return err;
#if defined(AUDIO_ANC_TT_HW)
    if(anc_type==ANC_TALKTHRU&&tt_open_flag==1)return err;
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    if(anc_type==ANC_MUSICCANCLE&&mc_open_flag==1)return err;
#endif

    anc_cfg_lock();
    if(anc_iir0_set_gain_timer==NULL)
    {
        anc_iir0_set_gain_timer = hwtimer_alloc(anc_iir0_set_gain_timer_handler, NULL);
        ASSERT(anc_iir0_set_gain_timer, "Failed to alloc anc_iir0_set_gain_timer");
    }
    if(anc_iir2_set_gain_timer==NULL)
    {
        anc_iir2_set_gain_timer = hwtimer_alloc(anc_iir2_set_gain_timer_handler, NULL);
        ASSERT(anc_iir2_set_gain_timer, "Failed to alloc anc_iir2_set_gain_timer");
    }

    if(anc_iir0_switching_timer==NULL)
    {
        anc_iir0_switching_timer = hwtimer_alloc(anc_iir0_switching_timer_handler, NULL);
        ASSERT(anc_iir0_switching_timer, "Failed to alloc anc_iir0_switching_timer");
    }

    if(anc_iir2_switching_timer==NULL)
    {
        anc_iir2_switching_timer = hwtimer_alloc(anc_iir2_switching_timer_handler, NULL);
        ASSERT(anc_iir2_switching_timer, "Failed to alloc anc_iir2_switching_timer");
    }

#if defined(ANC_FB_CHECK) ||defined(ANC_FF_CHECK)||defined(ANC_TT_CHECK)
    if(anc_howling_check_dev_timer==NULL)
    {
        anc_howling_check_dev_timer = hwtimer_alloc(anc_howling_check_timer_handler, NULL);
        ASSERT(anc_howling_check_dev_timer, "Failed to alloc usbdev_timer");
    }
#endif

    if(ff_open_flag==0&&fb_open_flag==0
#if defined(AUDIO_ANC_TT_HW)
            &&tt_open_flag==0
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
            &&mc_open_flag==0
#endif
      )
    {
        if((IIR_FF_COUNTER+IIR_TT_COUNTER)>13||(IIR_FB_COUNTER+IIR_MC_COUNTER)>13)
        {
            hal_cmu_codec_iir_enable(60000000);
        }
        else if((IIR_FF_COUNTER+IIR_TT_COUNTER)>11||(IIR_FB_COUNTER+IIR_MC_COUNTER)>11)
        {
            hal_cmu_codec_iir_enable(24576000*2);
        }
        else
        {
#if defined(ANC_LIMITER_FB) || \
     defined(ANC_LIMITER_MC) || \
    (defined(AUDIO_ANC_TT_HW)&&(defined(ANC_LIMITER_FF)|| defined(ANC_LIMITER_TT)))
            hal_cmu_codec_iir_enable(24576000*2);
#else
            hal_cmu_codec_iir_enable(39000000);
#endif
        }

        anc_ctrl_reg_init();

        iir0_coef_using=0;
        iir2_coef_using=0;
    }
#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK) || defined(ANC_TT_CHECK)

    if(anc_type==ANC_FEEDFORWARD||anc_type==ANC_FEEDBACK||anc_type==ANC_TALKTHRU)
    {
        hal_codec_anc_fb_check_set_irq_handler(anc_howling_check_irq_handler);

        fb_check_ch0_config->codec_fb_check_enable_ch0=0;
        *codec_int_config_clr=CODEC_FB_CHECK_ERROR_TRIG_CH0;
        if (analog_debug_get_anc_calib_mode())
        {
            codec_mask_config->fb_check_error_trig_ch0_mask=0;
        }
        else
        {
            codec_mask_config->fb_check_error_trig_ch0_mask=1;
        }
        feedback_mc_settings->codec_feedback_mc_en_ch0=1;

        fb_check_ch0_config->codec_fb_check_acc_sample_rate_ch0=3;
#if defined(ANC_FB_CHECK)
        fb_check_ch0_config->codec_fb_check_src_sel_ch0=0;
#elif defined(ANC_FF_CHECK)
        fb_check_ch0_config->codec_fb_check_src_sel_ch0=2;
#elif defined(ANC_TT_CHECK)
        fb_check_ch0_config->codec_fb_check_src_sel_ch0=2;
		anc_path_set->codec_pdu_mix_en_ch0=1;
#endif
        fb_check_ch0_config->codec_fb_check_acc_window_ch0=64;
        fb_check_ch0_config->codec_fb_check_trig_window_ch0=64;

        *codec_fb_check_threshold_ch0=ANC_HOWLING_THRESHOLD_0dB;

        fb_check_ch0_config->codec_fb_check_enable_ch0=1;
        fb_check_open_flag=1;

        ff_howling_gain_l=512;
        fb_howling_gain_l=512;
        tt_howling_gain_l=512;

        howling_cnt_l=0;
    }
#endif

    if(anc_type==ANC_FEEDFORWARD)
    {
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            max_ff_gain_l=(1<<GAIN_Q);
        }
        ff_open_flag=1;
    }

#if defined(AUDIO_ANC_TT_HW)
    if(anc_type==ANC_TALKTHRU)
    {
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            max_tt_gain_l=(1<<GAIN_Q);
        }
        tt_open_flag=1;
    }
#endif

    if(anc_type==ANC_FEEDBACK)
    {
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            max_fb_gain_l=(1<<GAIN_Q);
        }
        fb_open_flag=1;
    }
#if defined(AUDIO_ANC_FB_MC_HW)
    if(anc_type==ANC_MUSICCANCLE)
    {
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            max_mc_gain_l=(1<<GAIN_Q);
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
    if(anc_type==ANC_FEEDFORWARD)
    {
        LOG_I("%s: ANC_FEEDFORWARD",__func__);

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            max_ff_gain_l=0;
#if defined(ANC_FF_CHECK)
            fb_check_open_flag=0;
            fb_check_ch0_config->codec_fb_check_enable_ch0=0;
            *codec_int_config_clr=CODEC_FB_CHECK_ERROR_TRIG_CH0;
            codec_mask_config->fb_check_error_trig_ch0_mask=0;
            ff_howling_gain_l=0;
#endif
        }

        ff_open_flag=0;

#ifdef ANC_GAIN_RAMP
        ff_ramp_gain_l=0;
#endif
    }
#if defined(AUDIO_ANC_TT_HW)
    if(anc_type==ANC_TALKTHRU)
    {
        LOG_I("%s: ANC_TALKTHRU",__func__);

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            max_tt_gain_l=0;
#if defined(ANC_TT_CHECK)
            fb_check_open_flag=0;
            fb_check_ch0_config->codec_fb_check_enable_ch0=0;
            *codec_int_config_clr=CODEC_FB_CHECK_ERROR_TRIG_CH0;
            codec_mask_config->fb_check_error_trig_ch0_mask=0;
            tt_howling_gain_l=0;
			anc_path_set->codec_pdu_mix_en_ch0=0;
#endif
        }

        tt_open_flag=0;

        tt_ramp_gain_l=0;
    }
#endif

    if(anc_type==ANC_FEEDBACK)
    {
        LOG_I("%s: ANC_FEEDBACK",__func__);

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            max_fb_gain_l=0;

#if defined(ANC_FB_CHECK)
            fb_check_open_flag=0;
            fb_check_ch0_config->codec_fb_check_enable_ch0=0;
            *codec_int_config_clr=CODEC_FB_CHECK_ERROR_TRIG_CH0;
            codec_mask_config->fb_check_error_trig_ch0_mask=0;
            fb_howling_gain_l=0;
#endif
        }

        fb_open_flag=0;

#ifdef ANC_GAIN_RAMP
        fb_ramp_gain_l=0;
#endif
    }

#if defined(AUDIO_ANC_FB_MC_HW)
    if(anc_type==ANC_MUSICCANCLE)
    {
        LOG_I("%s: ANC_MUSICCANCLE",__func__);

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            max_mc_gain_l=0;
        }

        mc_open_flag=0;

#ifdef ANC_GAIN_RAMP
        mc_ramp_gain_l=0;
#endif
    }
#endif

    anc_ctrl_reg_close(anc_type);

    if(ff_open_flag==0&&fb_open_flag==0
#if defined(AUDIO_ANC_TT_HW)
            &&tt_open_flag==0
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
            &&mc_open_flag==0
#endif
      )
    {
        hwtimer_stop(anc_iir0_set_gain_timer);
        hwtimer_stop(anc_iir2_set_gain_timer);
        hwtimer_stop(anc_iir0_switching_timer);
        hwtimer_stop(anc_iir2_switching_timer);
        hal_cmu_codec_iir_disable();
        anc_iir_clock->en_clk_iir=0x0;
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
#if defined(ANC_FB_CHECK) ||defined(ANC_FF_CHECK) ||defined(ANC_TT_CHECK)
        fb_check_ch0_config->codec_fb_check_enable_ch0=1;
#endif
#if defined(AUDIO_ANC_TT_HW)
        tm_config->codec_tt_enable_ch0=1;
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
        tm_config->codec_mm_enable_ch0=1;
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
#if defined(ANC_FB_CHECK) ||defined(ANC_FF_CHECK) ||defined(ANC_TT_CHECK)
        fb_check_ch0_config->codec_fb_check_enable_ch0=0;
        hwtimer_stop(anc_howling_check_dev_timer);
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

    anc_cfg_unlock();

    return ANC_NO_ERR;
}


int anc_set_ff_and_tt_gain(int32_t gain_ff_l, int32_t gain_ff_r,int32_t gain_tt_l, int32_t gain_tt_r)
{
    ANC_ERROR err=ANC_NO_ERR;

    //if(gain_ch_l==511||gain_ch_l==0||gain_ch_l==250)
    {
        LOG_I("gain_ff_l:%d,gain_tt_l:%d",gain_ff_l,gain_tt_l);
    }

    anc_cfg_lock();

    ff_ramp_gain_l=gain_ff_l;
#if defined(AUDIO_ANC_TT_HW)
    tt_ramp_gain_l=gain_tt_l;
#endif

    if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
    {
        if(iir0_coef_using==0)
        {
            iir0_gaina_cfg_gain();
        }
        else
        {
            iir0_gainb_cfg_gain();
        }
    }

    anc_cfg_unlock();

    return err;
}

static float g_ff_adaptive_gain = 1.0;
int anc_set_gain2_float(float gain)
{
    if(gain > 1) {
        LOG_I("[%s] warning input value = %d/100 > 1",__func__,(int)(gain*100));
        g_ff_adaptive_gain = 1;

    } else if(gain < 0) {
        LOG_I("[%s] warning input value = %d/100 < 1",__func__,(int)(gain*100));
        g_ff_adaptive_gain = 0;
    } else {
        g_ff_adaptive_gain = gain;
    }
    return 0;
}

float anc_get_gain2_float(void)
{
    return g_ff_adaptive_gain;
}

int anc_set_gain(int32_t gain_ch_l, int32_t gain_ch_r,enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err=ANC_NO_ERR;

    if(gain_ch_l==511||gain_ch_l==0||gain_ch_l==250)
    {
        LOG_I("anc_set_gain anc_type:%d, gain_ch_l:%d,gain_ch_r:%d",anc_type,gain_ch_l,gain_ch_r);
    }

    if ((anc_type==ANC_FEEDFORWARD && ff_open_flag==0) ||
            (anc_type==ANC_FEEDBACK && fb_open_flag==0)) {
        LOG_I("%s: anc_type=%d not opened", __func__, anc_type);
        return ANC_OTHER_ERR;
    }

    anc_cfg_lock();

    // LOG_I("iir0_iira_stop_status_sync:%d,iir0_iirb_stop_status_sync:%d",anc_iir0_control->codec_iir0_iira_stop_status_sync,anc_iir0_control->codec_iir0_iirb_stop_status_sync);
    //LOG_I("%s: iir0_coef_using:%d,iir1_coef_using:%d,iir2_coef_using:%d,iir3_coef_using:%d", __func__,iir0_coef_using,iir1_coef_using,iir2_coef_using,iir3_coef_using);

    if(anc_type&ANC_FEEDFORWARD){
        gain_ch_l = (uint32_t)(gain_ch_l * g_ff_adaptive_gain);
        gain_ch_r = (uint32_t)(gain_ch_r * g_ff_adaptive_gain);
        // LOG_I("!!!!!!! internal gain = %d",gain_ch_l);
    }
#ifdef ANC_GAIN_RAMP

    if(anc_type&ANC_FEEDFORWARD)
    {
        ff_ramp_gain_l=gain_ch_l;

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            //gain_ch_l=512;
            //gain_ch_r=512;
            if(iir0_coef_using==0)
            {
                iir0_gaina_cfg_gain();
            }
            else
            {
                iir0_gainb_cfg_gain();
            }
        }
    }
    if(anc_type&ANC_FEEDBACK)
    {
        //Set the  FB gain;
        fb_ramp_gain_l=gain_ch_l;
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            //gain_ch_l=512;
            //gain_ch_r=512;
            if(iir2_coef_using==0)
            {
                iir2_gaina_cfg_gain();
            }
            else
            {
                iir2_gainb_cfg_gain();
            }
        }

    }

#if defined(AUDIO_ANC_TT_HW)
    if(anc_type&ANC_TALKTHRU)
    {
        //Set the TT gain;
        tt_ramp_gain_l=gain_ch_l;

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            if(iir0_coef_using==0)
            {
                iir0_gaina_cfg_gain();
            }
            else
            {
                iir0_gainb_cfg_gain();
            }
        }
    }
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    if(anc_type&ANC_MUSICCANCLE)
    {
        //Set the MC gain;
        mc_ramp_gain_l=gain_ch_l;

        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            if(iir2_coef_using==0)
            {
                iir2_gaina_cfg_gain();
            }
            else
            {
                iir2_gainb_cfg_gain();
            }
        }
    }
#endif

#else
    if(anc_type&ANC_FEEDFORWARD)
    {
        //Set the FF gain;
        anc_ff_gain->codec_anc_mute_gain_update_ff_ch0=0;

        anc_ff_gain->codec_anc_mute_gain_ff_ch0=gain_ch_l;

        anc_ff_gain->codec_anc_mute_gain_update_ff_ch0=1;
    }

    if(anc_type&ANC_FEEDBACK)
    {
        //Set the FB gain;
        anc_fb_gain->codec_anc_mute_gain_update_fb_ch0=0;

        anc_fb_gain->codec_anc_mute_gain_fb_ch0=gain_ch_l;

        anc_fb_gain->codec_anc_mute_gain_update_fb_ch0=1;
    }
#if defined(AUDIO_ANC_TT_HW)
    if(anc_type&ANC_TALKTHRU)
    {
        //Set the TT gain;
        anc_tt_gain->codec_anc_mute_gain_update_tt_ch0=0;

        anc_tt_gain->codec_anc_mute_gain_coef_tt_ch0=gain_ch_l;

        anc_tt_gain->codec_anc_mute_gain_update_tt_ch0=1;
    }
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    else if(anc_type&ANC_MUSICCANCLE)
    {
        //Set the TT gain;
        anc_mc_gain->codec_anc_mute_gain_update_mc_ch0=0;

        anc_mc_gain->codec_anc_mute_gain_coef_mc_ch0=gain_ch_l;

        anc_mc_gain->codec_anc_mute_gain_update_mc_ch0=1;
    }
#endif
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
    if(anc_type&ANC_FEEDFORWARD)
    {
        //Get the FF gain;
         *gain_ch_l=ff_ramp_gain_l;
         *gain_ch_r=ff_ramp_gain_l;
     }

    if(anc_type&ANC_FEEDBACK)
    {
        //Get the FB gain;
         *gain_ch_l=fb_ramp_gain_l;
         *gain_ch_r=fb_ramp_gain_l;
     }
#if defined(AUDIO_ANC_TT_HW)
    if(anc_type&ANC_TALKTHRU)
    {
        //Set the TT gain;
         *gain_ch_l=tt_ramp_gain_l;
         *gain_ch_r=tt_ramp_gain_l;
     }
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    if(anc_type&ANC_MUSICCANCLE)
    {
        //Set the MC gain;
         *gain_ch_l=mc_ramp_gain_l;
         *gain_ch_r=mc_ramp_gain_l;
     }
#endif
#else

    if(anc_type&ANC_FEEDFORWARD)
    {
        //Get the FF gain;
         *gain_ch_l=anc_ff_gain->codec_anc_mute_gain_ff_ch0;
         *gain_ch_r=anc_ff_gain->codec_anc_mute_gain_ff_ch0;
     }
    if(anc_type&ANC_FEEDBACK)
    {
        //Get the FB gain;
         *gain_ch_l=anc_fb_gain->codec_anc_mute_gain_fb_ch0;
         *gain_ch_r=anc_fb_gain->codec_anc_mute_gain_fb_ch0;
     }
#if defined(AUDIO_ANC_TT_HW)
    if(anc_type&ANC_TALKTHRU)
    {
        //Set the TT gain;
         *gain_ch_l=anc_tt_gain->codec_anc_mute_gain_coef_tt_ch0;
         *gain_ch_r=anc_tt_gain->codec_anc_mute_gain_coef_tt_ch0;
     }
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
     if(anc_type&ANC_MUSICCANCLE)
    {
        //Set the MC gain;
         *gain_ch_l=anc_mc_gain->codec_anc_mute_gain_coef_mc_ch0;
         *gain_ch_r=anc_mc_gain->codec_anc_mute_gain_coef_mc_ch0;
     }
#endif
#endif
    anc_cfg_unlock();

    //  LOG_I("anc_get_gain gain_ch_l:%d,gain_ch_r:%d",*gain_ch_l,*gain_ch_r);
	return err;
}

int anc_get_cfg_gain(int32_t *gain_ch_l, int32_t *gain_ch_r,enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err=ANC_NO_ERR;

    anc_cfg_lock();
    if(anc_type&ANC_FEEDFORWARD)
    {
        //Get the max FF gain;
        *gain_ch_l = max_ff_gain_l;
        *gain_ch_r = max_ff_gain_l;
    }

    if(anc_type&ANC_FEEDBACK)
    {
        //Get the max FB gain;
        *gain_ch_l = max_fb_gain_l;
        *gain_ch_r = max_fb_gain_l;
    }

#if defined(AUDIO_ANC_TT_HW)
    if(anc_type&ANC_TALKTHRU)
    {
        //Get the TT gain;
        *gain_ch_l=max_tt_gain_l;
        *gain_ch_r=max_tt_gain_l;
    }
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
    if(anc_type&ANC_MUSICCANCLE)
    {
        //Get the MC gain;
        *gain_ch_l=max_mc_gain_l;
        *gain_ch_r=max_mc_gain_l;
    }
#endif
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
    uint32_t lock;

    LOG_I("%s flag:%d", __func__,flag);

    lock = int_lock();
    if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
    {
#if defined(ANC_FF_CHECK) ||defined(ANC_FB_CHECK) ||defined(ANC_TT_CHECK)
        if(fb_check_open_flag==1)
        {
            if(flag==0)
            {
                fb_check_ch0_config->codec_fb_check_enable_ch0=0;
                codec_mask_config->fb_check_error_trig_ch0_mask=0;
               *codec_int_config_clr=CODEC_FB_CHECK_ERROR_TRIG_CH0;
#if defined(ANC_TT_CHECK)
				anc_path_set->codec_pdu_mix_en_ch0=0;
#endif
            }
            else
            {
                *codec_int_config_clr=CODEC_FB_CHECK_ERROR_TRIG_CH0;
                codec_mask_config->fb_check_error_trig_ch0_mask=1;
                fb_check_ch0_config->codec_fb_check_enable_ch0=1;
#if defined(ANC_TT_CHECK)
				anc_path_set->codec_pdu_mix_en_ch0=1;
#endif
            }
        }
#endif
    }
    int_unlock(lock);

    return;
}

void anc_howling_gain_reset(void)
{
    LOG_I("%s", __func__);
#if defined(ANC_FF_CHECK)
    ff_howling_gain_l=512;

    if(iir0_coef_using==0)
    {
        iir0_gaina_cfg_gain();
    }
    else
    {
        iir0_gainb_cfg_gain();
    }

#elif defined(ANC_TT_CHECK)
	tt_howling_gain_l=512;

	if(iir0_coef_using==0)
	{
		iir0_gaina_cfg_gain();
	}
	else
	{
		iir0_gainb_cfg_gain();
	}

#elif defined(ANC_FB_CHECK)
    fb_howling_gain_l=512;

    if(iir2_coef_using==0)
    {
        iir2_gaina_cfg_gain();
    }
    else
    {
        iir2_gainb_cfg_gain();
    }
#endif
}


int anc_set_switching_delay(ANC_SWITCHING_DELAY  anc_switching_delay,enum ANC_TYPE_T anc_type)
{
    anc_iir_coefs  iir_coef_gain_ramp;
    int anc_switching_coef_delay;
    int anc_switching_gain_delay;


    LOG_I("%s:anc_switching_delay:%d,anc_type:%d", __func__,anc_switching_delay,anc_type);

    uint32_t lock;
    lock = int_lock();

   switch(anc_switching_delay)
   {
        case ANC_SWITCHING_DELAY_50ms:
        iir_coef_gain_ramp=iir_coef_gain_ramp_delay1;
        anc_switching_gain_delay=ANC_SET_GAIN_DELAY_LONG/8;
        anc_switching_coef_delay=ANC_SET_GAIN_TIME_DELAY1+ANC_SET_GAIN_DELAY_LONG/8;
        break;

        case ANC_SWITCHING_DELAY_100ms:
        iir_coef_gain_ramp=iir_coef_gain_ramp_delay2;
        anc_switching_gain_delay=ANC_SET_GAIN_DELAY_LONG/4;
        anc_switching_coef_delay=ANC_SET_GAIN_TIME_DELAY2+ANC_SET_GAIN_DELAY_LONG/4;
        break;

        case ANC_SWITCHING_DELAY_200ms:
        iir_coef_gain_ramp=iir_coef_gain_ramp_delay3;
        anc_switching_gain_delay=ANC_SET_GAIN_DELAY_LONG/4;
        anc_switching_coef_delay=ANC_SET_GAIN_TIME_DELAY3+ANC_SET_GAIN_DELAY_LONG/4;
        break;

        case ANC_SWITCHING_DELAY_450ms:
        iir_coef_gain_ramp=iir_coef_gain_ramp_delay4;
        anc_switching_gain_delay=ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay=ANC_SET_GAIN_TIME_DELAY4+ANC_SET_GAIN_DELAY_LONG;
        break;

        case ANC_SWITCHING_DELAY_650ms:
        iir_coef_gain_ramp=iir_coef_gain_ramp_delay5;
        anc_switching_gain_delay=ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay=ANC_SET_GAIN_TIME_DELAY5+ANC_SET_GAIN_DELAY_LONG;
        break;

        case ANC_SWITCHING_DELAY_850ms:
        iir_coef_gain_ramp=iir_coef_gain_ramp_delay6;
        anc_switching_gain_delay=ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay=ANC_SET_GAIN_TIME_DELAY6+ANC_SET_GAIN_DELAY_LONG;
        break;

        case ANC_SWITCHING_DELAY_1300ms:
        iir_coef_gain_ramp=iir_coef_gain_ramp_delay7;
        anc_switching_gain_delay=ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay=ANC_SET_GAIN_TIME_DELAY7+ANC_SET_GAIN_DELAY_LONG;
        break;

        case ANC_SWITCHING_DELAY_2600ms:
        iir_coef_gain_ramp=iir_coef_gain_ramp_delay8;
        anc_switching_gain_delay=ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay=ANC_SET_GAIN_TIME_DELAY8+ANC_SET_GAIN_DELAY_LONG;
        break;

        default:
        iir_coef_gain_ramp=iir_coef_gain_ramp_delay4;
        anc_switching_gain_delay=ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay=ANC_SET_GAIN_TIME_DELAY4+ANC_SET_GAIN_DELAY_LONG;
        break;
   }

    if(anc_type&ANC_FEEDFORWARD || anc_type&ANC_TALKTHRU )
    {
        anc_iir0_control->codec_iir0_enable=0;

        anc_gain_ff_iir_coefs0_l[0].a1=-iir_coef_gain_ramp.coef_a[1];
        anc_gain_ff_iir_coefs0_l[0].a2=-iir_coef_gain_ramp.coef_a[2];
        anc_gain_ff_iir_coefs0_l[0].b0=iir_coef_gain_ramp.coef_b[0];
        anc_gain_ff_iir_coefs0_l[0].b1=iir_coef_gain_ramp.coef_b[1];
        anc_gain_ff_iir_coefs0_l[0].b2=iir_coef_gain_ramp.coef_b[2];

        anc_gain_ff_iir_coefs1_l[0].a1=-iir_coef_gain_ramp.coef_a[1];
        anc_gain_ff_iir_coefs1_l[0].a2=-iir_coef_gain_ramp.coef_a[2];
        anc_gain_ff_iir_coefs1_l[0].b0=iir_coef_gain_ramp.coef_b[0];
        anc_gain_ff_iir_coefs1_l[0].b1=iir_coef_gain_ramp.coef_b[1];
        anc_gain_ff_iir_coefs1_l[0].b2=iir_coef_gain_ramp.coef_b[2];

#if defined(AUDIO_ANC_TT_HW)
        anc_gain_tt_iir_coefs0_l[0].a1=-iir_coef_gain_ramp.coef_a[1];
        anc_gain_tt_iir_coefs0_l[0].a2=-iir_coef_gain_ramp.coef_a[2];
        anc_gain_tt_iir_coefs0_l[0].b0=iir_coef_gain_ramp.coef_b[0];
        anc_gain_tt_iir_coefs0_l[0].b1=iir_coef_gain_ramp.coef_b[1];
        anc_gain_tt_iir_coefs0_l[0].b2=iir_coef_gain_ramp.coef_b[2];

        anc_gain_tt_iir_coefs1_l[0].a1=-iir_coef_gain_ramp.coef_a[1];
        anc_gain_tt_iir_coefs1_l[0].a2=-iir_coef_gain_ramp.coef_a[2];
        anc_gain_tt_iir_coefs1_l[0].b0=iir_coef_gain_ramp.coef_b[0];
        anc_gain_tt_iir_coefs1_l[0].b1=iir_coef_gain_ramp.coef_b[1];
        anc_gain_tt_iir_coefs1_l[0].b2=iir_coef_gain_ramp.coef_b[2];
#endif

        anc_switching_gain_delay_ff_tt=anc_switching_gain_delay;
        anc_switching_coef_delay_ff_tt=anc_switching_coef_delay;

        anc_iir0_control->codec_iir0_enable=1;

    }

    if(anc_type&ANC_FEEDBACK ||anc_type&ANC_MUSICCANCLE)
    {
        anc_iir2_control->codec_iir2_enable=0;

        anc_gain_fb_iir_coefs0_l[0].a1=-iir_coef_gain_ramp.coef_a[1];
        anc_gain_fb_iir_coefs0_l[0].a2=-iir_coef_gain_ramp.coef_a[2];
        anc_gain_fb_iir_coefs0_l[0].b0=iir_coef_gain_ramp.coef_b[0];
        anc_gain_fb_iir_coefs0_l[0].b1=iir_coef_gain_ramp.coef_b[1];
        anc_gain_fb_iir_coefs0_l[0].b2=iir_coef_gain_ramp.coef_b[2];

        anc_gain_fb_iir_coefs1_l[0].a1=-iir_coef_gain_ramp.coef_a[1];
        anc_gain_fb_iir_coefs1_l[0].a2=-iir_coef_gain_ramp.coef_a[2];
        anc_gain_fb_iir_coefs1_l[0].b0=iir_coef_gain_ramp.coef_b[0];
        anc_gain_fb_iir_coefs1_l[0].b1=iir_coef_gain_ramp.coef_b[1];
        anc_gain_fb_iir_coefs1_l[0].b2=iir_coef_gain_ramp.coef_b[2];

#if defined(AUDIO_ANC_FB_MC_HW)
        anc_gain_mc_iir_coefs0_l[0].a1=-iir_coef_gain_ramp.coef_a[1];
        anc_gain_mc_iir_coefs0_l[0].a2=-iir_coef_gain_ramp.coef_a[2];
        anc_gain_mc_iir_coefs0_l[0].b0=iir_coef_gain_ramp.coef_b[0];
        anc_gain_mc_iir_coefs0_l[0].b1=iir_coef_gain_ramp.coef_b[1];
        anc_gain_mc_iir_coefs0_l[0].b2=iir_coef_gain_ramp.coef_b[2];

        anc_gain_mc_iir_coefs1_l[0].a1=-iir_coef_gain_ramp.coef_a[1];
        anc_gain_mc_iir_coefs1_l[0].a2=-iir_coef_gain_ramp.coef_a[2];
        anc_gain_mc_iir_coefs1_l[0].b0=iir_coef_gain_ramp.coef_b[0];
        anc_gain_mc_iir_coefs1_l[0].b1=iir_coef_gain_ramp.coef_b[1];
        anc_gain_mc_iir_coefs1_l[0].b2=iir_coef_gain_ramp.coef_b[2];
#endif

        anc_switching_gain_delay_fb_mc=anc_switching_gain_delay;
        anc_switching_coef_delay_fb_mc=anc_switching_coef_delay;

        anc_iir2_control->codec_iir2_enable=1;
    }
    int_unlock(lock);

    return 0;
}

int anc_howling_set(ANC_HOWLING_WINDOW window, ANC_HOWLING_THRESHOLD threshold)
{
#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK)|| defined(ANC_TT_CHECK)
    float threshold_multiple=0;
    uint32 acc_window=0;
    uint32 trig_window=0;
    uint32 check_threshold=0;

    uint32 lock;
    lock = int_lock();
    if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
    {
        if(fb_check_open_flag)
        {
            fb_check_ch0_config->codec_fb_check_enable_ch0=0;

            switch(window)
            {
                case ANC_HOWLING_WINDOW_32:
                threshold_multiple=0.25f;
                acc_window=window;
                trig_window=window;
                break;

                case ANC_HOWLING_WINDOW_64:
                threshold_multiple=1.0f;
                acc_window=window;
                trig_window=window;

                break;

                case ANC_HOWLING_WINDOW_128:
                threshold_multiple=4.0f;
                acc_window=window;
                trig_window=window;
                break;

                case ANC_HOWLING_WINDOW_256:
                threshold_multiple=16.0f;
                acc_window=window;
                trig_window=window;
                break;

                case ANC_HOWLING_WINDOW_512:
                threshold_multiple=64.0f;
                acc_window=window;
                trig_window=window;

                break;

                default:
                acc_window=ANC_HOWLING_WINDOW_64;
                trig_window=ANC_HOWLING_WINDOW_64;
                threshold_multiple=1.0f;
                break;
             }

            switch(threshold)
            {
                case ANC_HOWLING_THRESHOLD_m6dB:
                case ANC_HOWLING_THRESHOLD_m4dB:
                case ANC_HOWLING_THRESHOLD_m2dB:
                case ANC_HOWLING_THRESHOLD_0dB:
                case ANC_HOWLING_THRESHOLD_2dB:
                case ANC_HOWLING_THRESHOLD_4dB:
                case ANC_HOWLING_THRESHOLD_6dB:
                check_threshold=(uint32)(threshold*threshold_multiple);
                break;

                default:
                check_threshold=(uint32)(ANC_HOWLING_THRESHOLD_0dB*threshold_multiple);
                break;
             }
            LOG_I("%s:acc_window:%d,trig_window:%d", __func__,acc_window,trig_window);
            LOG_I("%s:check_threshold:0x%x", __func__,check_threshold);

            fb_check_ch0_config->codec_fb_check_acc_window_ch0=acc_window;
            fb_check_ch0_config->codec_fb_check_trig_window_ch0=trig_window;

           *codec_fb_check_threshold_ch0=check_threshold;

           fb_check_ch0_config->codec_fb_check_enable_ch0=1;
        }
    }
    int_unlock(lock);
#endif
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

int anc_limiter_enable(enum ANC_TYPE_T anc_type)
{
    LOG_I("%s anc_type:%d", __func__,anc_type);
#if defined(ANC_LIMITER_FF)
    if(anc_type&ANC_FEEDFORWARD)
    {
        //Enable the FF limiter;
        anc_iir0_control->codec_iir0_lmt_ch0_bypass=0;
    }
#endif
#if defined(ANC_LIMITER_FB)
    if(anc_type&ANC_FEEDBACK)
    {
        //Enable the FB limiter;
        anc_iir2_control->codec_iir2_lmt_ch0_bypass=0;
    }
#endif
#if defined(AUDIO_ANC_TT_HW)
#if defined(ANC_LIMITER_TT)
    if(anc_type&ANC_TALKTHRU)
    {
        //Enable the TT limiter;
        anc_iir0_control->codec_iir0_lmt_ch1_bypass=0;
    }
#endif
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
#if defined(ANC_LIMITER_MC)
    if(anc_type&ANC_MUSICCANCLE)
    {
        //Enable the MC limiter;
        anc_iir2_control->codec_iir2_lmt_ch1_bypass=0;
    }
#endif
#endif
    return 0;
}

int anc_limiter_disable(enum ANC_TYPE_T anc_type)
{
    LOG_I("%s anc_type:%d", __func__,anc_type);
#if defined(ANC_LIMITER_FF)
    if(anc_type&ANC_FEEDFORWARD)
    {
        //Disable the FF limiter;
        anc_iir0_control->codec_iir0_lmt_ch0_bypass=1;
    }
#endif
#if defined(ANC_LIMITER_FB)
    if(anc_type&ANC_FEEDBACK)
    {
        //Disable the FB limiter;
        anc_iir2_control->codec_iir2_lmt_ch0_bypass=1;
    }
#endif
#if defined(AUDIO_ANC_TT_HW)
#if defined(ANC_LIMITER_TT)
    if(anc_type&ANC_TALKTHRU)
    {
        //Disable the TT limiter;
        anc_iir0_control->codec_iir0_lmt_ch1_bypass=1;
    }
#endif
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
#if defined(ANC_LIMITER_MC)
    if(anc_type&ANC_MUSICCANCLE)
    {
        //Disable the MC limiter;
        anc_iir2_control->codec_iir2_lmt_ch1_bypass=1;
    }
#endif
#endif
    return 0;
}

int anc_limiter_threhold_set(enum ANC_TYPE_T anc_type,int32_t threhold_db)
{
    int32_t threhold;
    if(threhold_db>0)
    {
        threhold=0x7fffff;
    }
    else
    {
        threhold=(int32_t)(db_to_float(threhold_db)*0x7fffff);
    }
    LOG_I("%s threhold_db:%d,threhold:0x%x", __func__,threhold_db,threhold);
#if defined(ANC_LIMITER_FF)
    if(anc_type&ANC_FEEDFORWARD)
    {
        anc_iir_gain_update->codec_iir0_lmt_th_update_ch0=0;
        //limiter threshold, 0dB: 0x7fffff
        *codec_iir0_lmt_th_ch0=threhold;
        hal_sys_timer_delay_us(1);
        anc_iir_gain_update->codec_iir0_lmt_th_update_ch0=1;
    }
#endif
#if defined(ANC_LIMITER_FB)
    if(anc_type&ANC_FEEDBACK)
    {
        anc_iir_gain_update->codec_iir2_lmt_th_update_ch0=0;
        //limiter threshold, 0dB: 0x7fffff
        *codec_iir2_lmt_th_ch0=threhold;
        hal_sys_timer_delay_us(1);
        anc_iir_gain_update->codec_iir2_lmt_th_update_ch0=1;
    }
#endif
#if defined(AUDIO_ANC_TT_HW)
#if defined(ANC_LIMITER_TT)
    if(anc_type&ANC_TALKTHRU)
    {
        anc_iir_gain_update->codec_iir0_lmt_th_update_ch1=0;
        //limiter threshold, 0dB: 0x7fffff
        *codec_iir0_lmt_th_ch1=threhold;
        hal_sys_timer_delay_us(1);
        anc_iir_gain_update->codec_iir0_lmt_th_update_ch1=1;
    }
#endif
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
#if defined(ANC_LIMITER_MC)
    if(anc_type&ANC_MUSICCANCLE)
    {
        anc_iir_gain_update->codec_iir2_lmt_th_update_ch1=0;
        //limiter threshold, 0dB: 0x7fffff
        *codec_iir2_lmt_th_ch1=threhold;
        hal_sys_timer_delay_us(1);
        anc_iir_gain_update->codec_iir2_lmt_th_update_ch1=1;
    }
#endif
#endif
    return 0;
}

