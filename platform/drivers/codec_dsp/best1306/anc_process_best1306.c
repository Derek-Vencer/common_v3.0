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
** namer：ANC filter
** description：IIR and FIR filter
** version：V1.0
** author： xuml
** modify：2022.2.24.
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
#include "hal_sysfreq.h"
#include "tgt_hardware.h"
/**********************************mocro**************************************/

typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;

/*
5 64 soft_rstn_adc_ana RW 31 0 soft reset of clk_adc_ana domain
8 64 soft_rstn_adc RW 255 5 soft reset of clk_adc domain
1 64 soft_rstn_dac RW 1 13 soft reset of clk_dac domain
1 64 soft_rstn_rs_adc RW 1 14 soft reset of clk_rsadc domain
2 64 soft_rstn_rs_dac RW 3 15 soft reset of clk_rsdac domain
4 64 soft_rstn_iir_anc RW 15 17 soft reset of clk_iir domain
1 64 soft_rstn_iir_eq RW 1 21 soft reset of clk_iir1 domain
4 64 soft_rstn_fir RW 15 22 soft reset of clk_fir domain
2 64 soft_rstn_psap RW 3 26
28 64  x
*/
struct _anc_soft_rstn
{
    uint32 soft_rstn_adc_ana : 5;
    uint32 soft_rstn_adc : 8;
    uint32 soft_rstn_dac : 1;

    uint32 soft_rstn_rs_adc : 1;
    uint32 soft_rstn_rs_dac : 2;
    uint32 soft_rstn_iir_anc : 4;

    uint32 soft_rstn_iir_eq : 1;
    uint32 soft_rstn_fir : 4;
    uint32 soft_rstn_psap : 2;

    uint32 reserved : 4;
};

/*
2 6c rtsel_rom RW 2 0 rtsel_rom
2 6c ptsel_rom RW 1 2 ptsel_rom
2 6c trb_rom RW 1 4 rtb_rom
4 6c en_clk_iir_anc RW 0 6
1 6c en_clk_iir_eq RW 0 10
4 6c en_clk_fir RW 0 11
2 6c en_clk_psap RW 0 15
3 6c sel_i2s_mclk RW 0 17
1 6c en_i2s_mclk RW 0 20
2 6c cfg_div_codec_eqiir RW 0 21 divider for iir
1 6c bypass_div_codec_eqiir RW 0 23 bypass_div_codec_iir
1 6c sel_oscx4_eqiir RW 0 24 iir clock select.1:oscx4;0:pll
1 6c sel_oscx2_eqiir RW 1 25 iir clock select.1:osc/oscx2;0:oscx4/pll
1 6c sel_osc_eqiir RW 0 26 iir clock select.1:osc;0:oscx2
1 6c sel_osc_psap RW 0 27 iir clock select.1:osc;0:oscx2
28 6c  x

*/
struct _anc_iir_clock
{
    uint32 rtsel_rom : 2;
    uint32 ptsel_rom : 2;
    uint32 trb_rom : 2;

    uint32 en_clk_iir_anc : 4;
    uint32 en_clk_iir_eq : 1;
    uint32 en_clk_fir : 4;
    uint32 en_clk_psap : 2;

    uint32 sel_i2s_mclk : 3;
    uint32 en_i2s_mclk : 1;
    uint32 cfg_div_codec_eqiir : 2;
    uint32 bypass_div_codec_eqiir : 1;

    uint32 sel_oscx4_eqiir : 1;
    uint32 sel_oscx2_eqiir : 1;

    uint32 sel_osc_eqiir : 1;
    uint32 sel_osc_psap : 1;

    uint32 reserved : 4;
};

/*

1 248 codec_iir0_enable RW 0 0 anc iir module enable
1 248 codec_iir0_iira_enable RW 0 1
1 248 codec_iir0_iirb_enable RW 0 2
1 248 codec_iir0_ch0_bypass RW 0 3 1: ch0 bypass
1 248 codec_iir0_ch1_bypass RW 0 4
1 248 codec_iir0_gaincal_ext_ch0_bypass RW 1 5
1 248 codec_iir0_gaincal_ext_ch1_bypass RW 1 6
1 248 codec_iir0_gainuse_ext_ch0_bypass RW 1 7
1 248 codec_iir0_gainuse_ext_ch1_bypass RW 1 8
1 248 codec_iir0_lmt_ch0_bypass RW 1 9
1 248 codec_iir0_lmt_ch1_bypass RW 1 10
5 248 codec_iir0_count_ch0 RW 0 11 ch0 iir number  0~8
5 248 codec_iir0_count_ch1 RW 0 16
1 248 codec_iir0_coef_swap RW 0 21 0: use iir coef memory0;  1: use iir coef memory1;
1 248 codec_iir0_auto_stop RW 0 22 read only;  coef memoryX used currently
1 248 codec_iir0_coef_swap_status_sync[1] R 0 23 read only;  coef memoryX used currently
1 248 codec_iir0_iira_stop_status_sync[1] R 0 24
1 248 codec_iir0_iirb_stop_status_sync[1] R 0 25
26 248  x
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
1 25c codec_iir0_gain_ext_update_ch0 RW 0 0
1 25c codec_iir0_gain_ext_update_ch1 RW 0 1
1 25c codec_iir0_gain_ext_sel_ch0 RW 0 2
1 25c codec_iir0_gain_ext_sel_ch1 RW 0 3
1 25c codec_deq_iir_gain_ext_update_ch0 RW 0 4
1 25c codec_deq_iir_gain_ext_update_ch1 RW 0 5
1 25c codec_deq_iir_gain_ext_sel_ch0 RW 0 6
1 25c codec_deq_iir_gain_ext_sel_ch1 RW 0 7
1 25c codec_iir0_lmt_th_update_ch0 RW 0 8
1 25c codec_iir0_lmt_th_update_ch1 RW 0 9
28 25c  x

*/
struct _anc_iir_gain_update
{
    uint32 codec_iir0_gain_ext_update : 2;
    uint32 codec_iir0_gain_ext_sel_ch0 : 1;
    uint32 codec_iir0_gain_ext_sel_ch1 : 1;

    uint32 codec_deq_iir_gain_ext_update_ch0 : 1;
    uint32 codec_deq_iir_gain_ext_update_ch1 : 1;
    uint32 codec_deq_iir_gain_ext_sel_ch0 : 1;
    uint32 codec_deq_iir_gain_ext_sel_ch1 : 1;

    uint32 codec_iir0_lmt_th_update_ch0 : 1;
    uint32 codec_iir0_lmt_th_update_ch1 : 1;

    uint32 Reserved : 18;
};

/*
7 300 codec_iir0_lmt_delay_ch0 RW 0 0
7 300 codec_iir0_lmt_delay_ch1 RW 0 7

21 300  x

*/
struct _anc_iir_lmt_delay_ch0
{
    uint32 codec_iir0_lmt_delay : 7;
    uint32 Reserved : 25;
};

/*
7 300 codec_iir0_lmt_delay_ch0 RW 0 0
7 300 codec_iir0_lmt_delay_ch1 RW 0 7

21 300  x

*/
struct _anc_iir_lmt_delay_ch1
{
    uint32 Reserved_notused : 7;
    uint32 codec_iir0_lmt_delay : 7;
    uint32 Reserved : 18;
};

/*
1 d0 codec_anc_enable_ch0 RW 0 0 anc enable
1 d0 codec_anc_enable_ch1 RW 0 1
1 d0 codec_dual_anc_ch0     RW 0 2 FF + FB
1 d0 codec_dual_anc_ch1     RW 0 3
1 d0 codec_anc_mute_ch0     RW 0 4 anc output to 0
1 d0 codec_anc_mute_ch1     RW 0 5
1 d0 codec_anc_rate_sel     RW 0 6   0: 384k anc;  1: 768k anc;
1 d0 codec_feedback_ch0     RW 0 7 feedback mode
1 d0 codec_feedback_ch1     RW 0 8
31 d0  x

28 d0  x

*/
struct _anc_control
{
    uint32 codec_anc_enable_ch0 : 1;
    uint32 codec_anc_enable_ch1 : 1;
    uint32 codec_dual_anc_ch0 : 1;
    uint32 codec_dual_anc_ch1 : 1;
    uint32 codec_anc_mute_ch0 : 1;
    uint32 codec_anc_mute_ch1 : 1;
    uint32 codec_anc_rate_sel : 1;
    uint32 codec_feedback_ch0 : 1;
    uint32 codec_feedback_ch1 : 1;

    uint32 Reserved : 23;
};

/*
12 d4 codec_anc_mute_gain_ff_ch0 RW 0 0 Format 3.9, for FF
1 d4 codec_anc_mute_gain_pass0_ff_ch0 RW 0 12 1: gain update no pass0
1 d4 codec_anc_mute_gain_update_ff_ch0 RW 0 13
14 d4  x
*/
struct _anc_ff_gain
{
    int32 codec_anc_mute_gain_ff_ch0 : 12;
    int32 codec_anc_mute_gain_pass0_ff_ch0 : 1;
    int32 codec_anc_mute_gain_update_ff_ch0 : 1;

    int32 codec_anc_mute_gain_ff_ch1 : 12;
    int32 codec_anc_mute_gain_pass0_ff_ch1 : 1;
    int32 codec_anc_mute_gain_update_ff_ch1 : 1;

    uint32 Reserved : 4;
};

/*
12 d8 codec_anc_mute_gain_fb_ch0 RW 0 0 same as above
1 d8 codec_anc_mute_gain_pass0_fb_ch0 RW 0 12
1 d8 codec_anc_mute_gain_update_fb_ch0 RW 0 13
14 d8  x
*/
struct _anc_fb_gain
{
    int32 codec_anc_mute_gain_fb_ch0 : 12;
    int32 codec_anc_mute_gain_pass0_fb_ch0 : 1;
    int32 codec_anc_mute_gain_update_fb_ch0 : 1;

    uint32 Reserved : 18;
};

/*
1 2a0 codec_iir_gaincal_ext_ch0_bypass RW 1 0  =1, bypass extern gain ramp calc for iir ch0
1 2a0 codec_iir_gaincal_ext_ch1_bypass RW 1 1  =1, bypass extern gain ramp calc for iir ch1
1 2a0 codec_iir_gaincal_ext_ch2_bypass RW 1 2  =1, bypass extern gain ramp calc for iir ch2
1 2a0 codec_iir_gaincal_ext_ch3_bypass RW 1 3  =1, bypass extern gain ramp calc for iir ch3
1 2a0 codec_iir_gainuse_ext_ch0_bypass RW 1 4  =1, bypass, not use extern gain for iir ch0
1 2a0 codec_iir_gainuse_ext_ch1_bypass RW 1 5  =1, bypass, not use extern gain for iir ch1
1 2a0 codec_iir_gainuse_ext_ch2_bypass RW 1 6  =1, bypass, not use extern gain for iir ch2
1 2a0 codec_iir_gainuse_ext_ch3_bypass RW 1 7  =1, bypass, not use extern gain for iir ch3
1 2a0 codec_iir_gain_ext_update_ch0 RW 0 8   posedge to update extern gain for iir ch0
1 2a0 codec_iir_gain_ext_update_ch1 RW 0 9   posedge to update extern gain for iir ch1
1 2a0 codec_iir_gain_ext_update_ch2 RW 0 10   posedge to update extern gain for iir ch2
1 2a0 codec_iir_gain_ext_update_ch3 RW 0 11   posedge to update extern gain for iir ch3
1 2a0 codec_iir_gain_ext_sel_ch0 RW 0 12  =1, extern gain updated when pass0; =0, updated right away
1 2a0 codec_iir_gain_ext_sel_ch1 RW 0 13  =1, extern gain updated when pass0; =0, updated right away
1 2a0 codec_iir_gain_ext_sel_ch2 RW 0 14  =1, extern gain updated when pass0; =0, updated right away
1 2a0 codec_iir_gain_ext_sel_ch3 RW 0 15  =1, extern gain updated when pass0; =0, updated right away
16 2a0  x
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
16 238 codec_anc_calib_gain_coef_ff_ch0 RW 4096 0
16 238 codec_anc_calib_gain_coef_ff_ch1 RW 4096 0
*/

struct _anc_ff_iir_calib_gain
{
    uint32 codec_anc_calib_gain_coef_ff_ch0 : 16;
    uint32 codec_anc_calib_gain_coef_ff_ch1 : 16;
    // uint32 Reserved : 16;
};

/*
16 23c codec_anc_calib_gain_coef_fb_ch0 RW 4096 0
16 23c  x
*/

struct _anc_fb_iir_calib_gain
{
    uint32 codec_anc_calib_gain_coef_fb_ch0 : 16;
    uint32 Reserved : 16;
};

/*
1 33c codec_music_mix_off_ch0 RW 0 0
1 33c codec_pdu_off_ch0 RW 0 1
1 33c codec_pdu_mix_en_ch0 RW 0 2
1 33c codec_dehowl_en_ch0 RW 0 3
1 33c codec_music_mix_off_ch1 RW 0 4
1 33c codec_pdu_off_ch1 RW 0 5
1 33c codec_pdu_mix_en_ch1 RW 0 6
1 33c codec_dehowl_en_ch1 RW 0 7
1 33c codec_tt_out_off_ch0             RW 0 8
1 33c codec_tt_pdu_off_ch0        RW 1 9
1 33c codec_tt_out_off_ch1             RW 0 10
1 33c codec_tt_pdu_off_ch1             RW 1 11
1 33c codec_dehowl_keep_ch0 RW 0 12
3 33c codec_dehowl_keep_sel_ch0 RW 0 13
1 33c codec_dehowl_keep_ch1 RW 0 16
3 33c codec_dehowl_keep_sel_ch1 RW 0 17
1 33c codec_tws_anc_eq RW 0 20
21 33c  x
*/
struct _anc_path_set
{
    uint32 codec_music_mix_off_ch0 : 1;
    uint32 codec_pdu_off_ch0 : 1;
    uint32 codec_pdu_mix_en_ch0 : 1;
    uint32 codec_dehowl_en_ch0 : 1;

    uint32 codec_music_mix_off_ch1 : 1;
    uint32 codec_pdu_off_ch1 : 1;
    uint32 codec_pdu_mix_en_ch1 : 1;
    uint32 codec_dehowl_en_ch1 : 1;

    uint32 codec_tt_out_off_ch0 : 1;
    uint32 codec_tt_pdu_off_ch0 : 1;

    uint32 codec_tt_out_off_ch1 : 1;
    uint32 codec_tt_pdu_off_ch1 : 1;

    uint32 codec_dehowl_keep_ch0 : 1;
    uint32 codec_dehowl_keep_sel_ch0 : 3;

    uint32 codec_dehowl_keep_ch1 : 1;
    uint32 codec_dehowl_keep_sel_ch1 : 3;

    uint32 codec_tws_anc_eq : 1;

    uint32 Reserved : 11;
};

/*
1 244 codec_anc_calib_gain_pass0_ff_ch0 RW 0 0 Format 3.9, for FF
1 244 codec_anc_calib_gain_pass0_ff_ch1 RW 0 1
1 244 codec_anc_calib_gain_update_ff_ch0 RW 0 2 1: gain update no pass0
1 244 codec_anc_calib_gain_update_ff_ch1 RW 0 3
1 244 codec_anc_calib_gain_pass0_fb_ch0 RW 0 4
1 244 codec_anc_calib_gain_update_fb_ch0 RW 0 5
12 244  x
*/
struct _anc_iir_calib_gain_config
{
    uint32 codec_anc_calib_gain_pass0_ff_ch0 : 1;
    uint32 codec_anc_calib_gain_pass0_ff_ch1 : 1;
    uint32 codec_anc_calib_gain_update_ff_ch0 : 1;
    uint32 codec_anc_calib_gain_update_ff_ch1 : 1;
    uint32 codec_anc_calib_gain_pass0_fb_ch0 : 1;
    uint32 codec_anc_calib_gain_update_fb_ch0 : 1;

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
    int32 a1;
    int32 a2;

    int32 b1;
    int32 b2;
    int32 b0;
};

/*
1 dc codec_adc_mc_en_ch0 RW 0 0 adc ch0 music cancel enable
1 dc codec_adc_mc_en_ch1 RW 0 1 adc ch1 music cancel enable
1 dc codec_feedback_mc_en_ch0 RW 0 2 feedback music cancel enable
1 dc codec_feedback_mc_en_ch1 RW 0 3
1 dc codec_dac_L_iir_enable RW 0 4
1 dc codec_dac_R_iir_enable RW 0 5
1 dc codec_adc_ch0_iir_enable RW 0 6
1 dc codec_adc_ch1_iir_enable RW 0 7
4 dc codec_fb_check_udc_ch0 RW 0 8
4 dc codec_fb_check_udc_ch1 RW 0 12
22 dc
*/
struct _feedback_mc_settings
{
    uint32 codec_adc_mc_en_ch0 : 1;
    uint32 codec_adc_mc_en_ch1 : 1;
    uint32 codec_feedback_mc_en_ch0 : 1;
    uint32 codec_feedback_mc_en_ch1 : 1;
    uint32 codec_dac_L_iir_enable : 1;
    uint32 codec_dac_R_iir_enable : 1;
    uint32 codec_adc_ch0_iir_enable : 1;
    uint32 codec_adc_ch1_iir_enable : 1;
    uint32 codec_fb_check_udc_ch0 : 4;
    uint32 codec_fb_check_udc_ch1 : 4;

    uint32 Reserved1 : 16;
};

#if defined(AUDIO_ANC_FB_MC) || defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK) || defined(AUDIO_ANC_FB_MC_HW)
/*
1 130 codec_fb_check_enable_ch0 RW 0 0
2 130 codec_fb_check_acc_sample_rate_ch0 RW 0 1
2 130 codec_fb_check_src_sel_ch0 RW 0 3 00: feedback music cancel; 01: feedback; 10: feedfoward
1 130 codec_fb_check_keep_sel_ch0 RW 0 5
12 130 codec_fb_check_acc_window_ch0 RW 0 6
10 130 codec_fb_check_trig_window_ch0 RW 0 18
1 130 codec_fb_check_keep_ch0 RW 0 28
1 130 codec_fb_check_dcf_bypass_ch0 RW 0 29
30 130  x
*/
struct _fb_check_ch0_config
{
    uint32 codec_fb_check_enable_ch0 : 1;
    uint32 codec_fb_check_acc_sample_rate_ch0 : 2;
    uint32 codec_fb_check_src_sel_ch0 : 2;
    uint32 codec_fb_check_keep_sel_ch0 : 1;
    uint32 codec_fb_check_acc_window_ch0 : 12;
    uint32 codec_fb_check_trig_window_ch0 : 10;
    uint32 codec_fb_check_keep_ch0 : 1;
    uint32 codec_fb_check_dcf_bypass_ch0 : 1;
    uint32 Reserved : 2;
};

/*
1 134 codec_fb_check_enable_ch1 RW 0 0
2 134 codec_fb_check_acc_sample_rate_ch1 RW 0 1
2 134 codec_fb_check_src_sel_ch1 RW 0 3
1 134 codec_fb_check_keep_sel_ch1 RW 0 5
12 134 codec_fb_check_acc_window_ch1 RW 0 6
10 134 codec_fb_check_trig_window_ch1 RW 0 18
1 134 codec_fb_check_keep_ch1 RW 0 28
1 134 codec_fb_check_dcf_bypass_ch1 RW 0 29
30 134  x
*/

struct _fb_check_ch1_config
{
    uint32 codec_fb_check_enable_ch1 : 1;

    uint32 codec_fb_check_acc_sample_rate_ch1 : 2;

    uint32 codec_fb_check_src_sel_ch1 : 2;

    uint32 codec_fb_check_keep_sel_ch1 : 1;

    uint32 codec_fb_check_acc_window_ch1 : 12;

    uint32 codec_fb_check_trig_window_ch1 : 10;

    uint32 codec_fb_check_keep_ch1 : 1;

    uint32 codec_fb_check_dcf_bypass_ch1 : 1;

    uint32 Reserved : 2;
};
#endif

#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK)

/*
5 0c codec_rx_overflow                                     RW 0 0 interrupt status which not been masked.
5 0c codec_rx_underflow            RW 0 5
1 0c codec_tx_overflow RW 0 10
1 0c codec_tx_underflow RW 0 11
1 0c codec_tx_overflow_snd RW 0 12
1 0c codec_tx_underflow_snd RW 0 13
1 0c dsd_rx_overflow RW 0 14
1 0c dsd_rx_underflow RW 0 15
1 0c dsd_tx_overflow RW 0 16
1 0c dsd_tx_underflow RW 0 17
1 0c mc_overflow RW 0 18
1 0c mc_underflow RW 0 19
1 0c event_trigger RW 0 20
1 0c fb_check_error_trig_ch0 RW 0 21
1 0c fb_check_error_trig_ch1 RW 0 22
1 0c adc_max_overflow RW 0 27
1 0c time_trigger RW 0 28
29 0c  x

*/
struct _codec_int_config
{
    uint32 Reserved_notused : 21;

    uint32 fb_check_error_trig_ch0 : 1;
    uint32 fb_check_error_trig_ch1 : 1;

    uint32 Reserved : 9;
};
/*
5 10 codec_rx_overflow_mask RW 0 0 interrupt mask; 0 will mask interrupt
5 10 codec_rx_underflow_mask RW 0 5
1 10 codec_tx_overflow_mask RW 0 10
1 10 codec_tx_underflow_mask RW 0 11
1 10 codec_tx_overflow_snd_mask RW 0 12
1 10 codec_tx_underflow_snd_mask RW 0 13
1 10 dsd_rx_overflow_mask RW 0 14
1 10 dsd_rx_underflow_mask RW 0 15
1 10 dsd_tx_overflow_mask RW 0 16
1 10 dsd_tx_underflow_mask RW 0 17
1 10 mc_overflow_mask RW 0 18
1 10 mc_underflow_mask RW 0 19
1 10 event_trigger_mask RW 0 20
1 10 fb_check_error_trig_ch0_mask RW 0 21
1 10 fb_check_error_trig_ch1_mask RW 0 22
1 10 adc_max_overflow_mask RW 0 23
1 10 time_trigger_mask RW 0 24
29 10  x
*/
struct _codec_mask_config
{
    uint32 Reserved_notused : 21;

    uint32 fb_check_error_trig_ch0_mask : 1;
    uint32 fb_check_error_trig_ch1_mask : 1;

    uint32 Reserved : 9;
};
#endif

/*
1 2b8 codec_tm_iir_enable RW 0 0 tt/mc iir module enable
1 2b8 codec_tm_iir_ch0_bypass RW 0 1 1: ch0 bypass
1 2b8 codec_tm_iir_ch1_bypass RW 0 2
1 2b8 codec_tm_iir_ch2_bypass RW 0 3
1 2b8 codec_tm_iir_ch3_bypass RW 0 4
4 2b8 codec_tm_iir_count_ch0 RW 0 5 ch0 iir number  0~8
4 2b8 codec_tm_iir_count_ch1 RW 0 9
4 2b8 codec_tm_iir_count_ch2 RW 0 13
4 2b8 codec_tm_iir_count_ch3 RW 0 17
1 2b8 codec_tm_iir_coef_swap RW 0 21 0: use iir coef memory0;  1: use iir coef memory1;
1 2b8 tm_iir_coef_swap_status_sync[1] R 0 22 read only;  coef memoryX used currently 　 　 　
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
1 2bc codec_tm_iir_gaincal_ext_ch0_bypass RW 1 0 same as anc iir in reg_2a0
1 2bc codec_tm_iir_gaincal_ext_ch1_bypass RW 1 1
1 2bc codec_tm_iir_gaincal_ext_ch2_bypass RW 1 2
1 2bc codec_tm_iir_gaincal_ext_ch3_bypass RW 1 3
1 2bc codec_tm_iir_gainuse_ext_ch0_bypass RW 1 4
1 2bc codec_tm_iir_gainuse_ext_ch1_bypass RW 1 5
1 2bc codec_tm_iir_gainuse_ext_ch2_bypass RW 1 6
1 2bc codec_tm_iir_gainuse_ext_ch3_bypass RW 1 7
1 2bc codec_tm_iir_gain_ext_update_ch0 RW 0 8
1 2bc codec_tm_iir_gain_ext_update_ch1 RW 0 9
1 2bc codec_tm_iir_gain_ext_update_ch2 RW 0 10
1 2bc codec_tm_iir_gain_ext_update_ch3 RW 0 11
1 2bc codec_tm_iir_gain_ext_sel_ch0 RW 0 12
1 2bc codec_tm_iir_gain_ext_sel_ch1 RW 0 13
1 2bc codec_tm_iir_gain_ext_sel_ch2 RW 0 14
1 2bc codec_tm_iir_gain_ext_sel_ch3 RW 0 15
16 2bc  x
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
1  488 iir_ch1_sp_en RW 1 0 sel FB MC mode
1  488 codec_mm_enable_ch0_eco RW 1 1 enable MC
30 488  x
*/
struct _tm_config
{
    uint32 iir_ch1_sp_en : 1;
    uint32 codec_mm_enable_ch0_eco : 1;

    uint32 Reserved : 30;
};

#define CODEC_EN_CLK_IIR_IIR0 (1 << 0)

#if defined(ANC_FB_CHECK) && defined(ANC_FF_CHECK)
#error "ANC_FB_CHECK and ANC_FF_CHECK can not defined simultaneously "
#endif

#if (defined VOICE_ASSIST_ADA_IIR) && (!defined LITTLE_FF)
#define IIR_COUNTER (11)

#define IIR_FF_COUNTER (11)
#define IIR_FB_COUNTER (5)
#if defined(AUDIO_ANC_FB_MC_HW)
#define IIR_MC_COUNTER (5)
#else
#define IIR_MC_COUNTER (0)
#endif
#else
#define IIR_COUNTER (8)

#define IIR_FF_COUNTER (8)
#define IIR_FB_COUNTER (8)
#if defined(AUDIO_ANC_FB_MC_HW)
#define IIR_MC_COUNTER (8)
#else
#define IIR_MC_COUNTER (0)
#endif
#endif

// anc tools tws&stereo seleect (stereo: ch0 | ch1 , tws: ch0)
// #define ANC_AUD_OUTPUT_PATH_SPEAKER_DEV  (AUD_CHANNEL_MAP_CH1)
// #define ANC_AUD_OUTPUT_PATH_SPEAKER_DEV  (AUD_CHANNEL_MAP_CH0)
// #define ANC_AUD_OUTPUT_PATH_SPEAKER_DEV  (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)
#define ANC_AUD_OUTPUT_PATH_SPEAKER_DEV (CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV)

typedef struct _iir_parameter
{
    int32_t total_gain;

    uint16_t iir_bypass_flag;
    uint16_t iir_counter;

    anc_iir_coefs iir_coef[IIR_COUNTER];

} iir_parameter;

#define GAIN_Q (9)
#define ANC_GAIN_RAMP
//#define AUDIO_ANC_FIR_HW
#define CALIB_GAIN_Q (1 << 12)

#define FIXED_COEF_Q (1 << 27)

#define ANC_GAIN_RAMP_THRESHLD (40)

#define ANC_GAIN_RAMP_ZERO (10)

#define ANC_GAIN_RAMP_BURST_THRESHLD (ANC_GAIN_RAMP_THRESHLD * 10000)

#define ANC_FAST_CALIB_MODE

#ifdef ANC_GAIN_RAMP
#define FIXED_GAIN_RAMP_Q (1 << 27)

/*
Type='Low Passing';
Freq=240;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
static const anc_iir_coefs iir_coef_gain_ramp_delay1 =
{
        .coef_b = {512, 1025, 512},
        .coef_a = {134217728, -265823675, 131607997},
};

#define ANC_SET_GAIN_TIME_DELAY1 ((MS_TO_TICKS(70)))

/*
Type='Low Passing';
Freq=120;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
static const anc_iir_coefs iir_coef_gain_ramp_delay2 =
{
        .coef_b = {129, 257, 129},
        .coef_a = {134217728, -267123699, 132906486},
};

#define ANC_SET_GAIN_TIME_DELAY2 ((MS_TO_TICKS(150)))

/*
Type='Low Passing';
Freq=60;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
static const anc_iir_coefs iir_coef_gain_ramp_delay3 =
{
        .coef_b = {32, 65, 32},
        .coef_a = {134217728, -267778100, 133560501},
};

#define ANC_SET_GAIN_TIME_DELAY3 ((MS_TO_TICKS(300)))

/*
Type='Low Passing';
Freq=30;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs iir_coef_gain_ramp_delay4 =
{
        .coef_b = {8, 16, 8},
        .coef_a = {134217728, -268106408, 133888712},
};

#define ANC_SET_GAIN_TIME_DELAY4 ((MS_TO_TICKS(550)))
/*
Type='Low Passing';
Freq=1000;
Gain=0;
Q=0.004;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs iir_coef_gain_ramp_delay5 =
{
        .coef_b = {2950, 5900, 2950},
        .coef_a = {134217728, -88138077, -46067851},
};

#define ANC_SET_GAIN_TIME_DELAY5 ((MS_TO_TICKS(900)))

/*
Type='Low Passing';
Freq=1000;
Gain=0;
Q=0.003;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs iir_coef_gain_ramp_delay6 =
{
        .coef_b = {2410, 4821, 2410},
        .coef_a = {134217728, -72015762, -62192325},
};

#define ANC_SET_GAIN_TIME_DELAY6 ((MS_TO_TICKS(1200)))

/*
Type='Low Passing';
Freq=1000;
Gain=0;
Q=0.002;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs iir_coef_gain_ramp_delay7 =
{
        .coef_b = {1765, 3529, 1765},
        .coef_a = {134217728, -52726267, -81484402},
};

#define ANC_SET_GAIN_TIME_DELAY7 ((MS_TO_TICKS(1800)))

/*
Type='Low Passing';
Freq=1000;
Gain=0;
Q=0.00175;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs iir_coef_gain_ramp_delay8 =
{
        .coef_b = {1583, 3166, 1583},
        .coef_a = {134217728, -47296900, -86914496},
};

#define ANC_SET_GAIN_TIME_DELAY8 ((MS_TO_TICKS(2150)))

/*
Type='Low Passing';
Freq=1000;
Gain=0;
Q=0.00150;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs iir_coef_gain_ramp_delay9 =
{
        .coef_b = {1392, 2784, 1392},
        .coef_a = {134217728, -41587115, -92625045},
};

#define ANC_SET_GAIN_TIME_DELAY9 ((MS_TO_TICKS(2500)))

/*
Type='Low Passing';
Freq=1000;
Gain=0;
Q=0.00125;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs iir_coef_gain_ramp_delay10 =
{
        .coef_b = {1191, 2381, 1191},
        .coef_a = {134217728, -35574613, -98638352},
};

#define ANC_SET_GAIN_TIME_DELAY10 ((MS_TO_TICKS(2850)))

/*
Type='Low Passing';
Freq=1000;
Gain=0;
Q=0.001;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs iir_coef_gain_ramp_delay11 =
{
        .coef_b = {978, 1957, 978},
        .coef_a = {134217728, -29234663, -104979151},
};

#define ANC_SET_GAIN_TIME_DELAY11 ((MS_TO_TICKS(3200)))

#define ANC_GAIN_RAMP_DELAY (4) // 1.5s(4)

#if ANC_GAIN_RAMP_DELAY == 1

/*
Type='Low Passing';
Freq=240;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
static const anc_iir_coefs iir_coef_gain_ramp_ff_normal =
{
        .coef_b = {512, 1025, 512},
        .coef_a = {134217728, -265823675, 131607997},
};

static const anc_iir_coefs iir_coef_gain_ramp_fb_normal =
{
        .coef_b = {512, 1025, 512},
        .coef_a = {134217728, -265823675, 131607997},
};
#if defined(AUDIO_PSAP_DEHOWLING_HW) || defined(AUDIO_ANC_SPKCALIB_HW)

// 384k
static const anc_iir_coefs iir_coef_gain_ramp_spkcalib_normal =
{
        .coef_b = {512, 1025, 512},
        .coef_a = {134217728, -265823675, 131607997},
};

// 96k
static const anc_iir_coefs iir_coef_gain_ramp_dehowling_normal =
{
        .coef_b = {7966, 15932, 7966},
        .coef_a = {134217728, -258260876, 124075013},
};
#endif

#define ANC_SET_GAIN_TIME ((MS_TO_TICKS(70)))

#elif ANC_GAIN_RAMP_DELAY == 2
/*

Type='Low Passing';
Freq=120;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
static const anc_iir_coefs iir_coef_gain_ramp_ff_normal =
{
        .coef_b = {129, 257, 129},
        .coef_a = {134217728, -267123699, 132906486},
};

static const anc_iir_coefs iir_coef_gain_ramp_fb_normal =
{
        .coef_b = {129, 257, 129},
        .coef_a = {134217728, -267123699, 132906486},
};

#define ANC_SET_GAIN_TIME ((MS_TO_TICKS(150)))
#elif ANC_GAIN_RAMP_DELAY == 3

/*
Type='Low Passing';
Freq=60;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
static const anc_iir_coefs iir_coef_gain_ramp_ff_normal =
{
        .coef_b = {32, 65, 32},
        .coef_a = {134217728, -267778100, 133560501},
};

static const anc_iir_coefs iir_coef_gain_ramp_fb_normal =
{
        .coef_b = {32, 65, 32},
        .coef_a = {134217728, -267778100, 133560501},
};

#define ANC_SET_GAIN_TIME ((MS_TO_TICKS(300)))

#elif ANC_GAIN_RAMP_DELAY == 4

/*
Type='Low Passing';
Freq=30;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs iir_coef_gain_ramp_ff_normal =
{
        .coef_b = {8, 16, 8},
        .coef_a = {134217728, -268106408, 133888712},
};

static const anc_iir_coefs iir_coef_gain_ramp_fb_normal =
{
        .coef_b = {8, 16, 8},
        .coef_a = {134217728, -268106408, 133888712},
};

#define ANC_SET_GAIN_TIME ((MS_TO_TICKS(200)))

#elif ANC_GAIN_RAMP_DELAY == 5

/*
Type='Low Passing';
Freq=1000;
Gain=0;
Q=0.001;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/

static const anc_iir_coefs iir_coef_gain_ramp_ff_normal =
{
        .coef_b = {978, 1957, 978},
        .coef_a = {134217728, -29234663, -104979151},
};

static const anc_iir_coefs iir_coef_gain_ramp_fb_normal =
{
        .coef_b = {978, 1957, 978},
        .coef_a = {134217728, -29234663, -104979151},
};

#define ANC_SET_GAIN_TIME ((MS_TO_TICKS(3200)))

#endif /*ANC_GAIN_RAMP_DELAY*/

/*
Type='Low Passing';
Freq=240;
Gain=0;
Q=0.2;
[b,a]=biquad1(Freq,Gain,Q,Type,SampleRate);
*/
#ifdef ANC_FAST_CALIB_MODE
static const anc_iir_coefs iir_coef_gain_ramp_ff_calib =
{
        .coef_b = {512, 1025, 512},
        .coef_a = {134217728, -265823675, 131607997},
};

static const anc_iir_coefs iir_coef_gain_ramp_fb_calib =
{
        .coef_b = {512, 1025, 512},
        .coef_a = {134217728, -265823675, 131607997},
};

#define ANC_SET_GAIN_CALIB_TIME ((MS_TO_TICKS(70)))

#endif

#endif /*ANC_GAIN_RAMP*/

#if (!defined VOICE_ASSIST_ADA_IIR) ||  (defined LITTLE_FF)
#define ANC_LIMITER_IIR0_CH0
#define ANC_LIMITER_IIR0_CH1
#endif

#if defined(AUDIO_ANC_FB_MC_HW)
// #define ANC_LIMITER_MC
#endif

#define ANC_LIMITER_FF_ATTACK_DELAY (1)
#define ANC_LIMITER_FB_ATTACK_DELAY (1)
#define ANC_LIMITER_MC_ATTACK_DELAY (1)
// limiter threshold, 0dB: 0x7fffff
#define ANC_LIMITER_FF_THREHOLD (0x3fffff) //(0x32F52C)
#define ANC_LIMITER_FB_THREHOLD (0x3fffff) //(0x32F52C)
#define ANC_LIMITER_MC_THREHOLD (0x3fffff) //(0x7fffff)

#define ANC_LIMITER_RELEASE_FF_TIME (10) // ms
#define ANC_LIMITER_RELEASE_FB_TIME (10) // ms
#define ANC_LIMITER_RELEASE_MC_TIME (10) // ms

#define ANC_LIMITER_ATT_FF_TIME (13.42f) // ms
#define ANC_LIMITER_ATT_FB_TIME (13.42f) // ms
#define ANC_LIMITER_ATT_MC_TIME (13.42f) // ms

#if defined(ANC_LIMITER_IIR0_CH0)

#define ANC_LIMITER_ATT_FF_B1 (134217728 / (uint32)(ANC_LIMITER_ATT_FF_TIME * 100))
#define ANC_LIMITER_ATT_FF_A1 (134217728 - ANC_LIMITER_ATT_FF_B1)

#define ANC_LIMITER_RELEASE_FF_B1 (134217728 / (uint32)(ANC_LIMITER_RELEASE_FF_TIME * 100))
#define ANC_LIMITER_RELEASE_FF_A1 (134217728 - ANC_LIMITER_RELEASE_FF_B1)

static const anc_iir_coefs iir_coef_limiter_attack_ff =
{
        .coef_b = {0, ANC_LIMITER_ATT_FF_B1, 0},
        .coef_a = {134217728, -ANC_LIMITER_ATT_FF_A1, 0},
};
static const anc_iir_coefs iir_coef_limiter_release_ff =
{
        .coef_b = {0, ANC_LIMITER_RELEASE_FF_B1, 0},
        .coef_a = {134217728, -ANC_LIMITER_RELEASE_FF_A1, 0},
};
#endif

#if defined(ANC_LIMITER_IIR0_CH1)

#define ANC_LIMITER_ATT_FB_B1 (134217728 / (uint32)(ANC_LIMITER_ATT_FB_TIME * 100))
#define ANC_LIMITER_ATT_FB_A1 (134217728 - ANC_LIMITER_ATT_FB_B1)

#define ANC_LIMITER_RELEASE_FB_B1 (134217728 / (uint32)(ANC_LIMITER_RELEASE_FB_TIME * 100))
#define ANC_LIMITER_RELEASE_FB_A1 (134217728 - ANC_LIMITER_RELEASE_FB_B1)

static const anc_iir_coefs iir_coef_limiter_attack_fb =
{
        .coef_b = {0, ANC_LIMITER_ATT_FB_B1, 0},
        .coef_a = {134217728, -ANC_LIMITER_ATT_FB_A1, 0},
};
static const anc_iir_coefs iir_coef_limiter_release_fb =
{
        .coef_b = {0, ANC_LIMITER_RELEASE_FB_B1, 0},
        .coef_a = {134217728, -ANC_LIMITER_RELEASE_FB_A1, 0},
};
#endif

#if defined(ANC_LIMITER_MC)

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

#define ANC_BASE ((uint32)CODEC_BASE)

// clock
volatile static struct _anc_iir_clock *anc_iir_clock = (volatile struct _anc_iir_clock *)(ANC_BASE + 0x6c);

// calib gain
volatile static struct _anc_ff_iir_calib_gain *anc_ff_iir_calib_gain = (volatile struct _anc_ff_iir_calib_gain *)(ANC_BASE + 0x238);
volatile static struct _anc_fb_iir_calib_gain *anc_fb_iir_calib_gain = (volatile struct _anc_fb_iir_calib_gain *)(ANC_BASE + 0x23c);

volatile static struct _anc_iir_calib_gain_config *anc_iir_calib_gain_config = (volatile struct _anc_iir_calib_gain_config *)(ANC_BASE + 0x244);

volatile static struct _anc_control *anc_control = (volatile struct _anc_control *)(ANC_BASE + 0xd0);
volatile static struct _tm_config *tm_config = (volatile struct _tm_config *)(ANC_BASE + 0x488);

volatile static struct _anc_ff_gain *anc_ff_gain = (volatile struct _anc_ff_gain *)(ANC_BASE + 0xd4);
volatile static struct _anc_fb_gain *anc_fb_gain = (volatile struct _anc_fb_gain *)(ANC_BASE + 0xd8);

volatile static struct _anc_path_set *anc_path_set = (volatile struct _anc_path_set *)(ANC_BASE + 0x33c);

volatile static struct _anc_iir0_control *anc_iir0_control = (volatile struct _anc_iir0_control *)(ANC_BASE + 0x248);

volatile static struct _anc_iir_gain_update *anc_iir_gain_update = (volatile struct _anc_iir_gain_update *)(ANC_BASE + 0x25c);

volatile static int32 *codec_iir0_gaina_ext_ch0 = (volatile int32 *)(ANC_BASE + 0x0260);
volatile static int32 *codec_iir0_gaina_ext_ch1 = (volatile int32 *)(ANC_BASE + 0x0264);
volatile static int32 *codec_iir0_gainb_ext_ch0 = (volatile int32 *)(ANC_BASE + 0x0268);
volatile static int32 *codec_iir0_gainb_ext_ch1 = (volatile int32 *)(ANC_BASE + 0x026c);

volatile static int32 POSSIBLY_UNUSED *codec_iir0_gaina_ext_out_ch0_sync = (volatile int32 *)(ANC_BASE + 0x02a0);
volatile static int32 POSSIBLY_UNUSED *codec_iir0_gaina_ext_out_ch1_sync = (volatile int32 *)(ANC_BASE + 0x02a4);
volatile static int32 POSSIBLY_UNUSED *codec_iir0_gainb_ext_out_ch0_sync = (volatile int32 *)(ANC_BASE + 0x02a8);
volatile static int32 POSSIBLY_UNUSED *codec_iir0_gainb_ext_out_ch1_sync = (volatile int32 *)(ANC_BASE + 0x02ac);

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
#ifdef ANC_GAIN_RAMP
volatile static int32 *codec_iir0_gain_ext_th = (volatile int32 *)(ANC_BASE + 0x0308);
#endif

#if defined(ANC_LIMITER_IIR0_CH0)
volatile static int32 *codec_iir0_lmt_th_ch0 = (volatile int32 *)(ANC_BASE + 0x031c);
#endif

#if defined(ANC_LIMITER_IIR0_CH1)
volatile static int32 *codec_iir0_lmt_th_ch1 = (volatile int32 *)(ANC_BASE + 0x0320);
#endif
#endif

#if defined(ANC_LIMITER_IIR0_CH0)
volatile static struct _anc_iir_lmt_delay_ch0 *anc_iir_lmt_delay_ch0 = (volatile struct _anc_iir_lmt_delay_ch0 *)(ANC_BASE + 0x300);
#endif

#if defined(ANC_LIMITER_IIR0_CH1)
volatile static struct _anc_iir_lmt_delay_ch1 *anc_iir_lmt_delay_ch1 = (volatile struct _anc_iir_lmt_delay_ch1 *)(ANC_BASE + 0x300);
#endif

// IIR0
//  ff fb left coefs iir
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
volatile static struct _anc_iir_coefs *anc_ff_iir_coefs0_l = (volatile struct _anc_iir_coefs *)(ANC_BASE + 0x1000);
volatile static struct _anc_iir_coefs *anc_fb_iir_coefs0_l = (volatile struct _anc_iir_coefs *)(ANC_BASE + 0x1000 + IIR_FF_COUNTER * 20);
#ifdef ANC_GAIN_RAMP
volatile static struct _anc_iir_coefs *anc_gain_ff_iir_coefs0_l = (volatile struct _anc_iir_coefs *)(ANC_BASE + 0x1140);
volatile static struct _anc_iir_coefs *anc_gain_fb_iir_coefs0_l = (volatile struct _anc_iir_coefs *)(ANC_BASE + 0x1154);
#endif

#ifdef ANC_LIMITER_IIR0_CH0
volatile static struct _anc_iir_coefs *anc_limiter_ff_iir_coefs0_l = (volatile struct _anc_iir_coefs *)(ANC_BASE + 0x1168);
#endif
#ifdef ANC_LIMITER_IIR0_CH1
volatile static struct _anc_iir_coefs *anc_limiter_fb_iir_coefs0_l = (volatile struct _anc_iir_coefs *)(ANC_BASE + 0x1190);
#if defined(AUDIO_ANC_FB_MC_HW)
volatile static struct _anc_iir_coefs POSSIBLY_UNUSED *anc_limiter_mc_iir_coefs0_l = (volatile struct _anc_iir_coefs *)(ANC_BASE + 0x1190);
#endif
#endif

volatile static struct _anc_iir_coefs *anc_ff_iir_coefs1_l = (volatile struct _anc_iir_coefs *)(ANC_BASE + 0x1200);
volatile static struct _anc_iir_coefs *anc_fb_iir_coefs1_l = (volatile struct _anc_iir_coefs *)(ANC_BASE + 0x1200 + IIR_FF_COUNTER * 20);
#ifdef ANC_GAIN_RAMP
volatile static struct _anc_iir_coefs *anc_gain_ff_iir_coefs1_l = (volatile struct _anc_iir_coefs *)(ANC_BASE + 0x1340);
volatile static struct _anc_iir_coefs *anc_gain_fb_iir_coefs1_l = (volatile struct _anc_iir_coefs *)(ANC_BASE + 0x1354);
#endif

#ifdef ANC_LIMITER_IIR0_CH0
volatile static struct _anc_iir_coefs *anc_limiter_ff_iir_coefs1_l = (volatile struct _anc_iir_coefs *)(ANC_BASE + 0x1368);
#endif

#ifdef ANC_LIMITER_IIR0_CH1
volatile static struct _anc_iir_coefs *anc_limiter_fb_iir_coefs1_l = (volatile struct _anc_iir_coefs *)(ANC_BASE + 0x1390);
#if defined(AUDIO_ANC_FB_MC_HW)
volatile static struct _anc_iir_coefs POSSIBLY_UNUSED *anc_limiter_mc_iir_coefs1_l = (volatile struct _anc_iir_coefs *)(ANC_BASE + 0x1390);
#endif
#endif
#endif

// IIR0
//  ff right coefs iir
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
volatile static struct _anc_iir_coefs *anc_ff_iir_coefs0_r = (volatile struct _anc_iir_coefs *)(ANC_BASE + 0x1000 + IIR_FF_COUNTER * 20);
#ifdef ANC_GAIN_RAMP
volatile static struct _anc_iir_coefs *anc_gain_ff_iir_coefs0_r = (volatile struct _anc_iir_coefs *)(ANC_BASE + 0x1154);
#endif

#ifdef ANC_LIMITER_IIR0_CH1
volatile static struct _anc_iir_coefs *anc_limiter_ff_iir_coefs0_r = (volatile struct _anc_iir_coefs *)(ANC_BASE + 0x1190);
#endif

volatile static struct _anc_iir_coefs *anc_ff_iir_coefs1_r = (volatile struct _anc_iir_coefs *)(ANC_BASE + 0x1200 + IIR_FF_COUNTER * 20);
#ifdef ANC_GAIN_RAMP
volatile static struct _anc_iir_coefs *anc_gain_ff_iir_coefs1_r = (volatile struct _anc_iir_coefs *)(ANC_BASE + 0x1354);
#endif

#ifdef ANC_LIMITER_IIR0_CH1
volatile static struct _anc_iir_coefs *anc_limiter_ff_iir_coefs1_r = (volatile struct _anc_iir_coefs *)(ANC_BASE + 0x1390);
#endif
#endif

#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK) || defined(AUDIO_ANC_FB_MC_HW)
// feedback registers
volatile static struct _feedback_mc_settings *feedback_mc_settings = (volatile struct _feedback_mc_settings *)(ANC_BASE + 0xdc);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
volatile static struct _fb_check_ch0_config *fb_check_ch0_config = (volatile struct _fb_check_ch0_config *)(ANC_BASE + 0x0130);
#endif
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
volatile static struct _fb_check_ch1_config POSSIBLY_UNUSED *fb_check_ch1_config = (volatile struct _fb_check_ch1_config *)(ANC_BASE + 0x0134);
#endif
#endif

#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK)
volatile static struct _codec_int_config *codec_int_config = (volatile struct _codec_int_config *)(ANC_BASE + 0x0c);
volatile static struct _codec_mask_config *codec_mask_config = (volatile struct _codec_mask_config *)(ANC_BASE + 0x10);

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
volatile static uint32 *codec_fb_check_threshold_ch0 = (volatile uint32 *)(ANC_BASE + 0x0138);
volatile static uint32 *codec_fb_check_data_avg_keep_ch0 = (volatile uint32 *)(ANC_BASE + 0x0140);
#endif
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
volatile static uint32 *codec_fb_check_threshold_ch1 = (volatile uint32 *)(ANC_BASE + 0x013c);
volatile static uint32 *codec_fb_check_data_avg_keep_ch1 = (volatile uint32 *)(ANC_BASE + 0x0144);
#endif
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
volatile static int max_ff_gain_l = 0;
volatile static int max_fb_gain_l = 0;
volatile static int max_mc_gain_l=0;
#endif
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
volatile static int max_ff_gain_r = 0;
#endif

volatile static int ff_open_flag = 0;
volatile static int fb_open_flag = 0;
volatile static int mc_open_flag=0;

volatile static int fb_check_open_flag = 0;

//tws stereo select
volatile static int anc_output_ch_map = (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1);

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
#ifdef ANC_GAIN_RAMP
volatile static int ff_ramp_gain_l = 0;
volatile static int fb_ramp_gain_l = 0;
volatile static int mc_ramp_gain_l=0;
#endif

volatile static int ff_ramp_coef_l = 0;
volatile static int fb_ramp_coef_l = 0;
volatile static int mc_ramp_coef_l=0;
#endif
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
#ifdef ANC_GAIN_RAMP
volatile static int ff_ramp_gain_r = 0;
#endif

volatile static int ff_ramp_coef_r = 0;
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK)
static HWTIMER_ID anc_howling_check_dev_timer_l = NULL;
volatile static int ff_howling_gain_l = 0;
volatile static int fb_howling_gain_l = 0;

static uint32 howling_cnt_l = 0;
static uint32 howling_time_l = 0;
#endif
#endif
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK)
static HWTIMER_ID anc_howling_check_dev_timer_r=NULL;
volatile static int ff_howling_gain_r=0;

static uint32 howling_cnt_r=0;
static uint32 howling_time_r=0;
#endif
#endif

static HWTIMER_ID anc_iir0_set_gain_timer = NULL;

#define ANC_SET_GAIN_DELAY_LONG ((MS_TO_TICKS(200)))

#define ANC_SET_GAIN_DELAY_SHORT ((MS_TO_TICKS(2)))

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
static iir_parameter ff_filtes_l_old;
static iir_parameter fb_filtes_l_old;
#if defined(AUDIO_ANC_FB_MC_HW)
static iir_parameter mc_filtes_l_old;
#endif
#endif
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
static iir_parameter ff_filtes_r_old;
#endif

volatile static int8_t iir0_coef_using;

static struct_anc_cfg anc_cfg_coef_new;
static struct_anc_cfg anc_cfg_coef;

#define CALIB_GAIN_INVALID (0xFFFF)
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
static int32_t g_calib_ff_gain_l = CALIB_GAIN_INVALID;
static int32_t g_calib_fb_gain_l = CALIB_GAIN_INVALID;
#if defined(AUDIO_ANC_FB_MC_HW)
static int32_t g_calib_mc_gain_l = CALIB_GAIN_INVALID;
#endif
#endif
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
static int32_t g_calib_ff_gain_r = CALIB_GAIN_INVALID;
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
#if defined(ANC_LIMITER_IIR0_CH0)
static anc_iir_coefs ff_limiter_att_l_old;
static anc_iir_coefs ff_limiter_rls_l_old;
#endif
#if defined(ANC_LIMITER_IIR0_CH1)
static anc_iir_coefs fb_limiter_att_l_old;
static anc_iir_coefs fb_limiter_rls_l_old;
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
#if defined(ANC_LIMITER_MC)
static anc_iir_coefs mc_limiter_att_l_old;
static anc_iir_coefs mc_limiter_rls_l_old;
#endif
#endif
#endif
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
#if defined(ANC_LIMITER_IIR0_CH1)
static anc_iir_coefs ff_limiter_att_r_old;
static anc_iir_coefs ff_limiter_rls_r_old;
#endif
#endif

static int32 anc_cfg_iir0_type;

static int anc_switching_coef_delay_ff_fb = 0;
static int anc_switching_gain_delay_ff_fb = 0;

#define TRACE_COUNTER (10000)

static void anc_cfg_lock(void)
{
    return;
}

static void anc_cfg_unlock(void)
{
    return;
}

static int iir_filter_para_copy(iir_parameter *filtes_old, const aud_item *filtes_new, uint16_t iir_counter)
{
    filtes_old->iir_bypass_flag = filtes_new->iir_bypass_flag;
    filtes_old->iir_counter = filtes_new->iir_counter;

    int i;

    if (filtes_new->iir_counter > iir_counter)
    {
        LOG_I( "%s: warning filtes_new->iir_counter:%d", __func__, filtes_new->iir_counter);
        filtes_old->iir_counter = iir_counter;
    }
    else if (filtes_new->iir_counter <= 0)
    {
        LOG_I( "%s: warning filtes_new->iir_counter:%d", __func__, filtes_new->iir_counter);
        filtes_old->iir_counter = 0;
        filtes_old->iir_bypass_flag = 1;
    }

    for (i = 0; i < filtes_old->iir_counter; i++)
    {
        filtes_old->iir_coef[i] = filtes_new->iir_coef[i];
    }

    for (; i < iir_counter; i++)
    {
        filtes_old->iir_coef[i].coef_a[0] = FIXED_COEF_Q;
        filtes_old->iir_coef[i].coef_a[1] = 0;
        filtes_old->iir_coef[i].coef_a[2] = 0;
        filtes_old->iir_coef[i].coef_b[0] = FIXED_COEF_Q;
        filtes_old->iir_coef[i].coef_b[1] = 0;
        filtes_old->iir_coef[i].coef_b[2] = 0;
    }

    filtes_old->total_gain = filtes_new->total_gain;
    filtes_old->iir_counter = iir_counter;

    return 0;
}

static int iir_filter_coef_copy(volatile struct _anc_iir_coefs *iir_coefs, iir_parameter *__restrict filtes_old)
{
    int i;

    for (i = 0; i < filtes_old->iir_counter; i++)
    {
        iir_coefs[i].a1 = -filtes_old->iir_coef[i].coef_a[1];
        iir_coefs[i].a2 = -filtes_old->iir_coef[i].coef_a[2];
        iir_coefs[i].b0 = filtes_old->iir_coef[i].coef_b[0];
        iir_coefs[i].b1 = filtes_old->iir_coef[i].coef_b[1];
        iir_coefs[i].b2 = filtes_old->iir_coef[i].coef_b[2];
    }

    if (filtes_old->iir_counter > 0)
    {
        iir_coefs[0].b0 = (int32)((filtes_old->iir_coef[0].coef_b[0] * (int64_t)filtes_old->total_gain) >> GAIN_Q);
        iir_coefs[0].b1 = (int32)((filtes_old->iir_coef[0].coef_b[1] * (int64_t)filtes_old->total_gain) >> GAIN_Q);
        iir_coefs[0].b2 = (int32)((filtes_old->iir_coef[0].coef_b[2] * (int64_t)filtes_old->total_gain) >> GAIN_Q);
    }

    return 0;
}

#if defined(ANC_LIMITER_IIR0_CH0)||defined(ANC_LIMITER_IIR0_CH1)
static int iir_coef_copy(volatile struct _anc_iir_coefs *iir_coefs, anc_iir_coefs *__restrict filtes_old)
{
#if 0
    LOG_I("%s: iir_coefs:0x%x",__func__,(uint32_t)iir_coefs);
    LOG_I("%s: -filtes_old->coef_a[1]:0x%x",__func__,-filtes_old->coef_a[1]);
    LOG_I("%s: -filtes_old->coef_a[2]:0x%x",__func__, -filtes_old->coef_a[2]);
    LOG_I("%s: -filtes_old->coef_a[0]:0x%x",__func__, filtes_old->coef_b[0]);
    LOG_I("%s: -filtes_old->coef_b[1]:0x%x",__func__, filtes_old->coef_b[1]);
    LOG_I("%s: -filtes_old->coef_b[2]:0x%x",__func__, filtes_old->coef_b[2]);
#endif
    iir_coefs->a1 = -filtes_old->coef_a[1];
    iir_coefs->a2 = -filtes_old->coef_a[2];
    iir_coefs->b0 = filtes_old->coef_b[0];
    iir_coefs->b1 = filtes_old->coef_b[1];
    iir_coefs->b2 = filtes_old->coef_b[2];

    return 0;
}
#endif

static int iir0_ch0_gaina_cfg_gain(bool on)
{
    int32 iir0_gaina_ch0;
    uint32_t lock;

#if defined(ANC_FF_CHECK)
    /* ff_l use iir0_ch0*/
    iir0_gaina_ch0 = (int32)((((float)ff_ramp_coef_l / 512.0f) * ((float)ff_ramp_gain_l / 512.0f) * ((float)ff_howling_gain_l / 512.0f)) * FIXED_GAIN_RAMP_Q);
#else
    /* ff_l use iir0_ch0*/
    iir0_gaina_ch0 = (int32)((((float)ff_ramp_coef_l / 512.0f) * ((float)ff_ramp_gain_l / 512.0f)) * FIXED_GAIN_RAMP_Q);
#endif
    if (!on) {
        iir0_gaina_ch0 = 0;
    }
    // TRACE_IMM(1,"%s:iir0_gaina_ch0:%d",__func__,iir0_gaina_ch0);

    lock = int_lock();
    anc_iir_gain_update->codec_iir0_gain_ext_update = 0;
    *codec_iir0_gaina_ext_ch0 = iir0_gaina_ch0;
    hal_sys_timer_delay_us(1);
    anc_iir_gain_update->codec_iir0_gain_ext_update = 3;
    int_unlock(lock);

    return 0;
}

static int POSSIBLY_UNUSED iir0_ch0_gainb_cfg_gain(bool on)
{
    int32 iir0_gainb_ch0;
    uint32_t lock;

#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK)
    /* ff_l use iir0_ch0*/
    iir0_gainb_ch0 = (int32)((((float)ff_ramp_coef_l / 512.0f) * ((float)ff_ramp_gain_l / 512.0f) * ((float)ff_howling_gain_l / 512.0f)) * FIXED_GAIN_RAMP_Q);
#else
    /* ff_l use iir0_ch0*/
    iir0_gainb_ch0 = (int32)((((float)ff_ramp_coef_l / 512.0f) * ((float)ff_ramp_gain_l / 512.0f)) * FIXED_GAIN_RAMP_Q);
#endif
    if (!on) {
        iir0_gainb_ch0 = 0;
    }
    // TRACE_IMM(1,"%s:iir0_gainb_ch0:%d",__func__,iir0_gainb_ch0);

    lock = int_lock();
    anc_iir_gain_update->codec_iir0_gain_ext_update = 0;
    *codec_iir0_gainb_ext_ch0 = iir0_gainb_ch0;
    hal_sys_timer_delay_us(1);
    anc_iir_gain_update->codec_iir0_gain_ext_update = 3;
    int_unlock(lock);

    return 0;
}

static int iir0_ch1_gaina_cfg_gain(bool on)
{
    int32 iir0_gaina_ch1;
    uint32_t lock;

#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK)
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1) {
        /* stereo ff_r use iir0_ch1*/
        iir0_gaina_ch1 = (int32)((((float)ff_ramp_coef_r / 512.0f) * ((float)ff_ramp_gain_r / 512.0f) * ((float)ff_howling_gain_r / 512.0f)) * FIXED_GAIN_RAMP_Q);
    } else
#endif
    {
        /* tws fb_l use iir0_ch1*/
        iir0_gaina_ch1 = (int32)((((float)fb_ramp_coef_l / 512.0f) * ((float)fb_ramp_gain_l / 512.0f) * ((float)fb_howling_gain_l / 512.0f)) * FIXED_GAIN_RAMP_Q);
    }
#else
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1) {
        /* stereo ff_r use iir0_ch1*/
        iir0_gaina_ch1 = (int32)((((float)ff_ramp_coef_r / 512.0f) * ((float)ff_ramp_gain_r / 512.0f)) * FIXED_GAIN_RAMP_Q);
    } else
#endif
    {
        /* tws fb_l use iir0_ch1*/
        iir0_gaina_ch1 = (int32)((((float)fb_ramp_coef_l / 512.0f) * ((float)fb_ramp_gain_l / 512.0f)) * FIXED_GAIN_RAMP_Q);
    }
#endif
    if (!on) {
        iir0_gaina_ch1 = 0;
    }
    // TRACE_IMM(1,"%s:iir0_gaina_ch1:%d",__func__,iir0_gaina_ch1);

    lock = int_lock();
    anc_iir_gain_update->codec_iir0_gain_ext_update = 0;
    *codec_iir0_gaina_ext_ch1 = iir0_gaina_ch1;
    hal_sys_timer_delay_us(1);
    anc_iir_gain_update->codec_iir0_gain_ext_update = 3;
    int_unlock(lock);

    return 0;
}

static int POSSIBLY_UNUSED iir0_ch1_gainb_cfg_gain(bool on)
{
    int32 iir0_gainb_ch1;
    uint32_t lock;

#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK)
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1) {
        /* stereo ff_r use iir0_ch1*/
        iir0_gainb_ch1 = (int32)((((float)ff_ramp_coef_r / 512.0f) * ((float)ff_ramp_gain_r / 512.0f) * ((float)ff_howling_gain_r / 512.0f)) * FIXED_GAIN_RAMP_Q);
    } else
#endif
    {
        /* tws mc_l use iir0_ch1*/
        iir0_gainb_ch1 = (int32)((((float)mc_ramp_coef_l / 512.0f) * ((float)mc_ramp_gain_l / 512.0f) * ((float)fb_howling_gain_l / 512.0f)) * FIXED_GAIN_RAMP_Q);
    }
#else
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1) {
        /* stereo ff_r use iir0_ch1*/
        iir0_gainb_ch1 = (int32)((((float)ff_ramp_coef_r / 512.0f) * ((float)ff_ramp_gain_r / 512.0f)) * FIXED_GAIN_RAMP_Q);
    } else
#endif
    {
        /* tws mc_l use iir0_ch1*/
        iir0_gainb_ch1 = (int32)((((float)mc_ramp_coef_l / 512.0f) * ((float)mc_ramp_gain_l / 512.0f)) * FIXED_GAIN_RAMP_Q);
    }
#endif
    if (!on) {
        iir0_gainb_ch1 = 0;
    }
    // TRACE_IMM(1,"%s:iir0_gainb_ch1:%d",__func__,iir0_gainb_ch1);

    lock = int_lock();
    anc_iir_gain_update->codec_iir0_gain_ext_update = 0;
    *codec_iir0_gainb_ext_ch1 = iir0_gainb_ch1;
    hal_sys_timer_delay_us(1);
    anc_iir_gain_update->codec_iir0_gain_ext_update = 3;
    int_unlock(lock);

    return 0;
}

static int POSSIBLY_UNUSED iir0_gainb_creat_stop_status(void)
{
    return 0;
}
static int POSSIBLY_UNUSED iir0_gaina_creat_stop_status(void)
{

    return 0;
}

#if ((AUD_SECTION_STRUCT_VERSION & 0x1) == 0)
#if defined(ANC_LIMITER_IIR0_CH0) || defined(ANC_LIMITER_IIR0_CH1)
static int anc_limiter_att_rls_coef_generate(anc_iir_coefs *limiter_coef, float lmiter_time)
{
    if ((lmiter_time >= 0.1f) && (lmiter_time <= 671088.64f))
    {
        limiter_coef->coef_b[0] = 0;
        limiter_coef->coef_b[1] = 134217728 / (uint32_t)(lmiter_time * 100);
        limiter_coef->coef_b[2] = 0;

        limiter_coef->coef_a[0] = 134217728;
        limiter_coef->coef_a[1] = -(134217728 - limiter_coef->coef_b[1]);
        limiter_coef->coef_a[2] = 0;
    }
    else
    {
        LOG_I( "%s: [warning]Error lmiter_time:%d", __func__, (uint32_t)(lmiter_time * 100));
    }

    return 0;
}
#endif
#endif

/**
 * @brief      Update coef and store gain,
 *     fadein: enable: use anc_set_gain(), disable: anc_apply_max_gain() to set gain.
 *
 * @param[in]  cfg   coef and gain
 */
static int anc_set_cfg_internal(const struct_anc_cfg *cfg, enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err = ANC_NO_ERR;
    uint32_t lock;
    uint32_t set_gain_timer_flag = 0;
    // int32 gaina_ch0, gaina_ch1,gainb_ch0,gainb_ch1;
#if defined(ANC_LIMITER_IIR0_CH0)||defined(ANC_LIMITER_IIR0_CH1)
    POSSIBLY_UNUSED int32_t threhold = 0;
#endif
    int16_t ff_offset_l = 0;
    int16_t ff_offset_r = 0;
    int16_t fb_offset_l = 0;
    int16_t fb_offset_r = 0;

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    const aud_item *ff_filtes_l;
    const aud_item *fb_filtes_l;
#if defined(AUDIO_ANC_FB_MC_HW)
    const aud_item *mc_filtes_l;
#endif
#endif
#if ((AUD_SECTION_STRUCT_VERSION & 0x1) == 0)
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
    const aud_item *ff_filtes_r;
#endif
#endif

    if ((anc_type & ANC_FEEDFORWARD && ff_open_flag == 0)
    || (anc_type & ANC_FEEDBACK && fb_open_flag == 0)
#if defined(AUDIO_ANC_FB_MC_HW)
    || (anc_type & ANC_MUSICCANCLE && mc_open_flag==0)
#endif
    )
    {
        LOG_I( "%s: anc_type=%d not opened", __func__, anc_type);
        return ANC_OTHER_ERR;
    }

    // anc_gain_delay=ANC_GAIN_NO_DELAY;

    // LOG_I("%s*******anc_type:%d, seting:a1:%d,a2:%d", __func__,anc_type,anc_gain_ff_iir_coefs0_l[0].a1,anc_gain_ff_iir_coefs0_l[0].a2);

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    ff_filtes_l = &(cfg->anc_cfg_ff_l);
    fb_filtes_l = &(cfg->anc_cfg_fb_l);

    ff_offset_l = ff_filtes_l->adc_gain_offset;
    fb_offset_l = fb_filtes_l->adc_gain_offset;
#if defined(AUDIO_ANC_FB_MC_HW)
    mc_filtes_l = &(cfg->anc_cfg_mc_l);
#endif
#endif
#if ((AUD_SECTION_STRUCT_VERSION & 0x1) == 0)
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
    ff_filtes_r = &(cfg->anc_cfg_ff_r);

    ff_offset_r = ff_filtes_r->adc_gain_offset;
#endif
#endif
    // LOG_I("ff_l:%d,ff_r:%d,fb_l:%d,fb_r:%d",ff_filtes_l->total_gain,ff_filtes_r->total_gain,fb_filtes_l->total_gain,fb_filtes_r->total_gain);
#ifdef ANC_PROD_TEST
    if(anc_type&ANC_FEEDFORWARD)
    {
        int time_int = 0;
        int time_Fra = 0;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        LOG_I("ff gain: %d, counter: %d iir_bypass_flag: %d ",ff_filtes_l->total_gain, ff_filtes_l->iir_counter,ff_filtes_l->iir_bypass_flag);
        LOG_I("ff dac_gain_offset %d, adc_gain_offset %d",ff_filtes_l->dac_gain_offset, ff_filtes_l->adc_gain_offset);

#if ((AUD_SECTION_STRUCT_VERSION & 0x1) == 0)
        LOG_I("limiter_flag:%d",ff_filtes_l->limiter_flag);

        time_int = (int)(ff_filtes_l->limiter_th);
        time_Fra = (int)(ff_filtes_l->limiter_th*1000)-((int)ff_filtes_l->limiter_th)*1000;

        LOG_I("limiter_th(dB):%d.%d",time_int,time_Fra);

        time_int = (int)(ff_filtes_l->limiter_att_time);
        time_Fra = (int)(ff_filtes_l->limiter_att_time*1000)-((int)ff_filtes_l->limiter_att_time)*1000;

        LOG_I("limiter_att_time(ms):%d.%d",time_int,time_Fra);

        time_int=(int)(ff_filtes_l->limiter_rls_time);
        time_Fra=(int)(ff_filtes_l->limiter_rls_time*1000)-((int)ff_filtes_l->limiter_rls_time)*1000;

        LOG_I("limiter_rls_time(ms):%d.%d",time_int,time_Fra);
#endif

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
#endif
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
        LOG_I("ff gain: %d, counter: %d iir_bypass_flag: %d ",ff_filtes_r->total_gain, ff_filtes_r->iir_counter,ff_filtes_r->iir_bypass_flag);
        LOG_I("ff dac_gain_offset %d, adc_gain_offset %d",ff_filtes_r->dac_gain_offset, ff_filtes_r->adc_gain_offset);

#if ((AUD_SECTION_STRUCT_VERSION & 0x1) == 0)
        LOG_I("limiter_flag:%d",ff_filtes_r->limiter_flag);

        time_int = (int)(ff_filtes_r->limiter_th);
        time_Fra = (int)(ff_filtes_r->limiter_th*1000)-((int)ff_filtes_r->limiter_th)*1000;

        LOG_I("limiter_th(dB):%d.%d",time_int,time_Fra);

        time_int=(int)(ff_filtes_r->limiter_att_time);
        time_Fra=(int)(ff_filtes_r->limiter_att_time*1000) - ((int)ff_filtes_r->limiter_att_time)*1000;

        LOG_I("limiter_att_time(ms):%d.%d",time_int,time_Fra);

        time_int = (int)(ff_filtes_r->limiter_rls_time);
        time_Fra = (int)(ff_filtes_r->limiter_rls_time*1000)-((int)ff_filtes_r->limiter_rls_time)*1000;

        LOG_I("limiter_rls_time(ms):%d.%d",time_int,time_Fra);
#endif

        for(int j = 0; j <IIR_FF_COUNTER; j++)
        {
            //LOG_I("iir coef ff r %10d, %10d, %10d, %10d, %10d, %10d",
            LOG_I("iir coef ff r 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x",\
                    ff_filtes_r->iir_coef[j].coef_b[0], \
                    ff_filtes_r->iir_coef[j].coef_b[1], \
                    ff_filtes_r->iir_coef[j].coef_b[2], \
                    ff_filtes_r->iir_coef[j].coef_a[0], \
                    ff_filtes_r->iir_coef[j].coef_a[1], \
                    ff_filtes_r->iir_coef[j].coef_a[2]);
        }
#endif
    }
    if(anc_type&ANC_FEEDBACK)
    {
        int time_int = 0;
        int time_Fra = 0;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        LOG_I("fb gain: %d, counter: %d iir_bypass_flag: %d ",fb_filtes_l->total_gain, fb_filtes_l->iir_counter,fb_filtes_l->iir_bypass_flag);
        LOG_I("fb dac_gain_offset %d, adc_gain_offset %d",fb_filtes_l->dac_gain_offset, fb_filtes_l->adc_gain_offset);

#if ((AUD_SECTION_STRUCT_VERSION & 0x1) == 0)
        LOG_I("limiter_flag:%d",fb_filtes_l->limiter_flag);

        time_int = (int)(fb_filtes_l->limiter_th);
        time_Fra = (int)(fb_filtes_l->limiter_th*1000)-((int)fb_filtes_l->limiter_th)*1000;

        LOG_I("limiter_th(dB):%d.%d",time_int,time_Fra);

        time_int = (int)(fb_filtes_l->limiter_att_time);
        time_Fra = (int)(fb_filtes_l->limiter_att_time*1000)-((int)fb_filtes_l->limiter_att_time)*1000;

        LOG_I("limiter_att_time(ms):%d.%d",time_int,time_Fra);

        time_int=(int)(fb_filtes_l->limiter_rls_time);
        time_Fra=(int)(fb_filtes_l->limiter_rls_time*1000)-((int)fb_filtes_l->limiter_rls_time)*1000;

        LOG_I("limiter_rls_time(ms):%d.%d",time_int,time_Fra);
#endif

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
#endif
    }

#if defined(AUDIO_ANC_FB_MC_HW)
    if(anc_type&ANC_MUSICCANCLE)
    {
        int time_int = 0;
        int time_Fra = 0;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        LOG_I("mc gain: %d, counter: %d iir_bypass_flag: %d ",mc_filtes_l->total_gain, mc_filtes_l->iir_counter,mc_filtes_l->iir_bypass_flag);

#if ((AUD_SECTION_STRUCT_VERSION & 0x1) == 0)
        LOG_I("limiter_flag:%d",mc_filtes_l->limiter_flag);

        time_int = (int)(mc_filtes_l->limiter_th);
        time_Fra = (int)(mc_filtes_l->limiter_th*1000)-((int)mc_filtes_l->limiter_th)*1000;

        LOG_I("limiter_th(dB):%d.%d",time_int,time_Fra);

        time_int = (int)(mc_filtes_l->limiter_att_time);
        time_Fra = (int)(mc_filtes_l->limiter_att_time*1000)-((int)mc_filtes_l->limiter_att_time)*1000;

        LOG_I("limiter_att_time(ms):%d.%d",time_int,time_Fra);

        time_int=(int)(mc_filtes_l->limiter_rls_time);
        time_Fra=(int)(mc_filtes_l->limiter_rls_time*1000)-((int)mc_filtes_l->limiter_rls_time)*1000;

        LOG_I("limiter_rls_time(ms):%d.%d",time_int,time_Fra);
#endif

        for(int j = 0; j <IIR_MC_COUNTER; j++)
        {
            LOG_I("iir coef mc l 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x",\
                    mc_filtes_l->iir_coef[j].coef_b[0], \
                    mc_filtes_l->iir_coef[j].coef_b[1], \
                    mc_filtes_l->iir_coef[j].coef_b[2], \
                    mc_filtes_l->iir_coef[j].coef_a[0], \
                    mc_filtes_l->iir_coef[j].coef_a[1], \
                    mc_filtes_l->iir_coef[j].coef_a[2]);
        }
#endif
    }
#endif
#endif /*ANC_PROD_TEST*/

    if (anc_type & ANC_FEEDFORWARD)
    {
        LOG_I( "%s: ff_offset_l=%d ff_offset_r=%d...", __func__, ff_offset_l, ff_offset_r);
        analog_aud_apply_anc_adc_gain_offset(ANC_FEEDFORWARD, ff_offset_l, ff_offset_r);
    }

    if (anc_type & ANC_FEEDBACK)
    {
        LOG_I( "%s: fb_offset_l=%d fb_offset_r=%d...", __func__, fb_offset_l, fb_offset_r);
        analog_aud_apply_anc_adc_gain_offset(ANC_FEEDBACK, fb_offset_l, fb_offset_r);
    }

// // updata limiter parameters.
#if ((AUD_SECTION_STRUCT_VERSION & 0x1) == 0)

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
    {
#if defined(ANC_LIMITER_IIR0_CH0)
        if (ff_filtes_l->limiter_flag != 0)
        {

            if (ff_filtes_l->limiter_th > 0)
            {
                threhold = 0x7fffff;
            }
            else
            {
                threhold = (int32_t)(db_to_float(ff_filtes_l->limiter_th) * 0x7fffff);
            }

            // limiter threshold, 0dB: 0x7fffff
            anc_iir_gain_update->codec_iir0_lmt_th_update_ch0 = 0;
            *codec_iir0_lmt_th_ch0 = threhold;
            hal_sys_timer_delay_us(1);
            anc_iir_gain_update->codec_iir0_lmt_th_update_ch0 = 1;

            anc_limiter_att_rls_coef_generate(&ff_limiter_att_l_old, ff_filtes_l->limiter_att_time);
            anc_limiter_att_rls_coef_generate(&ff_limiter_rls_l_old, ff_filtes_l->limiter_rls_time);

            // Enable the FF limiter;
            anc_iir0_control->codec_iir0_lmt_ch0_bypass = 0;
        }
        else
        {
            // Disable the FF limiter;
            anc_iir0_control->codec_iir0_lmt_ch0_bypass = 1;
        }

#endif

#if defined(ANC_LIMITER_IIR0_CH1)
        if (fb_filtes_l->limiter_flag != 0)
        {
            if (fb_filtes_l->limiter_th > 0)
            {
                threhold = 0x7fffff;
            }
            else
            {
                threhold = (int32_t)(db_to_float(fb_filtes_l->limiter_th) * 0x7fffff);
            }

            // limiter threshold, 0dB: 0x7fffff
            anc_iir_gain_update->codec_iir0_lmt_th_update_ch1 = 0;
            *codec_iir0_lmt_th_ch1 = threhold;
            hal_sys_timer_delay_us(1);
            anc_iir_gain_update->codec_iir0_lmt_th_update_ch1 = 1;

            anc_limiter_att_rls_coef_generate(&fb_limiter_att_l_old, fb_filtes_l->limiter_att_time);
            anc_limiter_att_rls_coef_generate(&fb_limiter_rls_l_old, fb_filtes_l->limiter_rls_time);

            // Enable the FB limiter;
            anc_iir0_control->codec_iir0_lmt_ch1_bypass = 0;
        }
        else
        {
            // Disable the FB limiter;
            anc_iir0_control->codec_iir0_lmt_ch1_bypass = 1;
        }
#endif

#if defined(ANC_LIMITER_MC) && defined(AUDIO_ANC_FB_MC_HW)
        if (mc_filtes_l->limiter_flag != 0) {
            if(mc_filtes_l->limiter_th>0) {
                threhold = 0x7fffff;
            } else {
                threhold = (int32_t)(db_to_float(mc_filtes_l->limiter_th) * 0x7fffff);
            }

            //limiter threshold, 0dB: 0x7fffff
            anc_iir_gain_update->codec_iir0_lmt_th_update_ch1 = 0;
            *codec_iir0_lmt_th_ch1 = threhold;
            hal_sys_timer_delay_us(1);
            anc_iir_gain_update->codec_iir0_lmt_th_update_ch1 = 1;

            anc_limiter_att_rls_coef_generate(&mc_limiter_att_l_old,mc_filtes_l->limiter_att_time);
            anc_limiter_att_rls_coef_generate(&mc_limiter_rls_l_old,mc_filtes_l->limiter_rls_time);

            //Enable the MC limiter;
            anc_iir0_control->codec_iir0_lmt_ch1_bypass = 0;
        } else {
            //Disable the MC limiter;
            anc_iir0_control->codec_iir0_lmt_ch1_bypass = 1;
        }
#endif
    }
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1)
    {
#if defined(ANC_LIMITER_IIR0_CH1)
        if (ff_filtes_r->limiter_flag != 0)
        {
            if (ff_filtes_r->limiter_th > 0)
            {
                threhold = 0x7fffff;
            }
            else
            {
                threhold = (int32_t)(db_to_float(ff_filtes_r->limiter_th) * 0x7fffff);
            }

            // limiter threshold, 0dB: 0x7fffff
            anc_iir_gain_update->codec_iir0_lmt_th_update_ch1 = 0;
            *codec_iir0_lmt_th_ch1 = threhold;
            hal_sys_timer_delay_us(1);
            anc_iir_gain_update->codec_iir0_lmt_th_update_ch1 = 1;

            anc_limiter_att_rls_coef_generate(&ff_limiter_att_r_old, ff_filtes_r->limiter_att_time);
            anc_limiter_att_rls_coef_generate(&ff_limiter_rls_r_old, ff_filtes_r->limiter_rls_time);

            // Enable the FF limiter;
            anc_iir0_control->codec_iir0_lmt_ch1_bypass = 0;
        }
        else
        {
            // Disable the FF limiter;
            anc_iir0_control->codec_iir0_lmt_ch1_bypass = 1;
        }
#endif
    }
#endif


#endif

    /***************************ANC_FEEDFORWARD**********************/
    if (anc_type & ANC_FEEDFORWARD)
    {
        LOG_I( "%s: ANC_FEEDFORWARD", __func__);
        // LOG_I("iir0_iira_stop_status_sync:%d,iir0_iirb_stop_status_sync:%d",anc_iir0_control->codec_iir0_iira_stop_status_sync,anc_iir0_control->codec_iir0_iirb_stop_status_sync);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
        {
            LOG_I( "%s: ANC_FEEDFORWARD ch_L...", __func__);
            iir_filter_para_copy(&ff_filtes_l_old, ff_filtes_l, IIR_FF_COUNTER);
            if (ff_filtes_l_old.iir_bypass_flag == 1)
            {
                max_ff_gain_l = ff_filtes_l_old.total_gain;
            }
            else
            {
                max_ff_gain_l = (1 << GAIN_Q);
                LOG_I( "%s: ANC_FEEDFORWARD ch_L... 2", __func__);
            }

            anc_iir0_control->codec_iir0_ch0_bypass = ff_filtes_l_old.iir_bypass_flag;
            anc_iir0_control->codec_iir0_count_ch0 = ff_filtes_l_old.iir_counter;

            iir_filter_coef_copy(anc_ff_iir_coefs0_l, &ff_filtes_l_old);
#if defined(ANC_LIMITER_IIR0_CH0)
            iir_coef_copy(&(anc_limiter_ff_iir_coefs0_l[0]), &ff_limiter_att_l_old);
            iir_coef_copy(&(anc_limiter_ff_iir_coefs0_l[1]), &ff_limiter_rls_l_old);
#endif

            anc_iir0_control->codec_iir0_iira_enable = 1;

            ff_ramp_coef_l = max_ff_gain_l;

            lock = int_lock();
            anc_iir_gain_update->codec_iir0_gain_ext_update = 0;
            hal_sys_timer_delay_us(1);
            anc_iir_gain_update->codec_iir0_gain_ext_update = 3;
            int_unlock(lock);

            max_ff_gain_l = (1 << GAIN_Q);
        }
#endif
#if ((AUD_SECTION_STRUCT_VERSION & 0x1) == 0)
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1)
        {
            LOG_I( "%s: ANC_FF ch_R...", __func__);
            iir_filter_para_copy(&ff_filtes_r_old, ff_filtes_r, IIR_FF_COUNTER);
            if (ff_filtes_r_old.iir_bypass_flag == 1)
            {
                max_ff_gain_r = ff_filtes_r_old.total_gain;
            }
            else
            {
                max_ff_gain_r = (1 << GAIN_Q);
            }

            anc_iir0_control->codec_iir0_ch1_bypass = ff_filtes_r_old.iir_bypass_flag;
            anc_iir0_control->codec_iir0_count_ch1 = ff_filtes_r_old.iir_counter;

            if(anc_iir0_control->codec_iir0_ch0_bypass==1)
            {
                iir_filter_coef_copy(anc_ff_iir_coefs0_l, &ff_filtes_r_old);
            }
            else
            {
                iir_filter_coef_copy(anc_ff_iir_coefs0_r, &ff_filtes_r_old);
            }

#if defined(ANC_LIMITER_IIR0_CH1)
            iir_coef_copy(&(anc_limiter_ff_iir_coefs0_r[0]), &ff_limiter_att_r_old);
            iir_coef_copy(&(anc_limiter_ff_iir_coefs0_r[1]), &ff_limiter_rls_r_old);
#endif

            anc_iir0_control->codec_iir0_iira_enable = 1;

            ff_ramp_coef_r = max_ff_gain_r;

            lock = int_lock();
            anc_iir_gain_update->codec_iir0_gain_ext_update = 0;
            hal_sys_timer_delay_us(1);
            anc_iir_gain_update->codec_iir0_gain_ext_update = 3;
            int_unlock(lock);

            max_ff_gain_r = (1 << GAIN_Q);
        }
#endif
#endif
        set_gain_timer_flag |= ANC_FEEDFORWARD;
    }

    /***************************ANC_FEEDBACK**********************/
    if (anc_type & ANC_FEEDBACK)
    {
        LOG_I( "%s: ANC_FEEDBACK ...", __func__);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
        {
            LOG_I( "%s: ANC_FEEDBACK ch_L...", __func__);
            iir_filter_para_copy(&fb_filtes_l_old, fb_filtes_l, IIR_FB_COUNTER);

            if (fb_filtes_l_old.iir_bypass_flag == 1)
            {
                max_fb_gain_l = fb_filtes_l_old.total_gain;
            }
            else
            {
                max_fb_gain_l = (1 << GAIN_Q);
                LOG_I( "%s: ANC_FEEDBACK ch_L... 2", __func__);
            }

            anc_iir0_control->codec_iir0_ch1_bypass = fb_filtes_l_old.iir_bypass_flag;
            anc_iir0_control->codec_iir0_count_ch1 = fb_filtes_l_old.iir_counter;

            if(anc_iir0_control->codec_iir0_ch0_bypass==1)
            {
                iir_filter_coef_copy(anc_ff_iir_coefs0_l, &fb_filtes_l_old);
            }
            else
            {
                iir_filter_coef_copy(anc_fb_iir_coefs0_l, &fb_filtes_l_old);
            }

#if defined(ANC_LIMITER_IIR0_CH1)
            iir_coef_copy(&(anc_limiter_fb_iir_coefs0_l[0]), &fb_limiter_att_l_old);
            iir_coef_copy(&(anc_limiter_fb_iir_coefs0_l[1]), &fb_limiter_rls_l_old);
#endif

            anc_iir0_control->codec_iir0_iira_enable = 1;
            fb_ramp_coef_l = max_fb_gain_l;

            lock = int_lock();
            anc_iir_gain_update->codec_iir0_gain_ext_update = 0;
            hal_sys_timer_delay_us(1);
            anc_iir_gain_update->codec_iir0_gain_ext_update = 3;
            int_unlock(lock);

            max_fb_gain_l = (1 << GAIN_Q);
        }
#endif
        set_gain_timer_flag |= ANC_FEEDBACK;
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
            LOG_I("%s: ANC_MUSICCANCLE ch-L...",__func__);
            iir_filter_para_copy(&mc_filtes_l_old,mc_filtes_l,IIR_MC_COUNTER);

            max_mc_gain_l=(1<<GAIN_Q);

            if(anc_iir0_control->codec_iir0_ch0_bypass==1)
            {
                iir_filter_coef_copy(anc_ff_iir_coefs1_l, &mc_filtes_l_old);
            }
            else
            {
                iir_filter_coef_copy(anc_fb_iir_coefs1_l, &mc_filtes_l_old);
            }

#if defined(ANC_LIMITER_MC)
            iir_coef_copy(&(anc_limiter_mc_iir_coefs1_l[0]), &mc_limiter_att_l_old);
            iir_coef_copy(&(anc_limiter_mc_iir_coefs1_l[1]), &mc_limiter_rls_l_old);
#endif

            anc_iir0_control->codec_iir0_iirb_enable=1;

            mc_ramp_coef_l = max_mc_gain_l;

            lock = int_lock();
            anc_iir_gain_update->codec_iir0_gain_ext_update=0;
            hal_sys_timer_delay_us(1);
            anc_iir_gain_update->codec_iir0_gain_ext_update=3;
            int_unlock(lock);

            max_mc_gain_l=(1<<GAIN_Q);
        }
#endif

        set_gain_timer_flag |= ANC_MUSICCANCLE;
    }
#endif

    if (set_gain_timer_flag) {
        hwtimer_stop(anc_iir0_set_gain_timer);
        hwtimer_start(anc_iir0_set_gain_timer, anc_switching_gain_delay_ff_fb);
    }

    return err;
}

int32_t anc_set_calib_gain(enum ANC_TYPE_T type, int32_t gain_l, int32_t gain_r)
{
    LOG_I( "[%s] type: %d, gain_l: %d, gain_r: %d", __func__, type, gain_l, gain_r);
    // +/-6 dB
    if ((gain_l < 512 / 2) || (gain_l > 512 * 2)) {
        gain_l = 512;
        LOG_I( "[%s] WARNING: Invalid gain_l: %d. Use default value", __func__, gain_l);
    }

    if ((gain_r < 512 / 2) || (gain_r > 512 * 2)) {
        gain_r = 512;
        LOG_I( "[%s] WARNING: Invalid gain_r: %d. Use default value", __func__, gain_r);
    }

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
    {
        if (type == ANC_FEEDFORWARD)
        {
            g_calib_ff_gain_l = gain_l;
        }
        else if (type == ANC_FEEDBACK)
        {
            g_calib_fb_gain_l = gain_l;
        }
        else
        {
            ASSERT(0, "[%s] Invalid anc type: %d", __func__, type);
        }
    }
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1)
    {
        if (type == ANC_FEEDFORWARD)
        {
            g_calib_ff_gain_r = gain_r;
        }
        else
        {
            ASSERT(0, "[%s] Invalid anc type: %d", __func__, type);
        }
    }
#endif

    return 0;
}

int anc_set_cfg(const struct_anc_cfg *cfg, enum ANC_TYPE_T anc_type, ANC_GAIN_TIME anc_gain_delay)
{
    uint32_t lock;

    LOG_I( "%s:0x%x", __func__, anc_type);

    if (cfg == NULL)
    {
        LOG_I( "%s: cfg is null", __func__);
        return ANC_OTHER_ERR;
    }

    if ((anc_type & ANC_FEEDFORWARD && ff_open_flag == 0) || (anc_type & ANC_FEEDBACK && fb_open_flag == 0)
#if defined(AUDIO_ANC_FB_MC_HW)
    || (anc_type&ANC_MUSICCANCLE && mc_open_flag==0)
#endif
     )
    {
        LOG_I( "%s: anc_type=%d not opened", __func__, anc_type);
        return ANC_OTHER_ERR;
    }

    lock = int_lock();

    int iir0_type;

    if (anc_type & ANC_FEEDFORWARD)
    {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        anc_cfg_iir0_type = anc_cfg_iir0_type | ANC_FEEDFORWARD;
        anc_cfg_coef_new.anc_cfg_ff_l = cfg->anc_cfg_ff_l;
        if (g_calib_ff_gain_l != CALIB_GAIN_INVALID)
        {
            anc_cfg_coef_new.anc_cfg_ff_l.total_gain = g_calib_ff_gain_l;
        }
#endif
#if ((AUD_SECTION_STRUCT_VERSION & 0x1) == 0)
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
        anc_cfg_coef_new.anc_cfg_ff_r = cfg->anc_cfg_ff_r;
        if (g_calib_ff_gain_r != CALIB_GAIN_INVALID)
        {
            anc_cfg_coef_new.anc_cfg_ff_r.total_gain = g_calib_ff_gain_r;
        }
#endif
#endif
    }

    if (anc_type & ANC_FEEDBACK)
    {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        anc_cfg_iir0_type = anc_cfg_iir0_type | ANC_FEEDBACK;
        anc_cfg_coef_new.anc_cfg_fb_l = cfg->anc_cfg_fb_l;
        if (g_calib_fb_gain_l != CALIB_GAIN_INVALID)
        {
            anc_cfg_coef_new.anc_cfg_fb_l.total_gain = g_calib_fb_gain_l;
        }
#endif
    }

#if defined(AUDIO_ANC_FB_MC_HW)
    if (anc_type&ANC_MUSICCANCLE) {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        anc_cfg_iir0_type = anc_cfg_iir0_type | ANC_MUSICCANCLE;
        anc_cfg_coef_new.anc_cfg_mc_l = cfg->anc_cfg_mc_l;
        if (g_calib_mc_gain_l != CALIB_GAIN_INVALID) {
            anc_cfg_coef_new.anc_cfg_mc_l.total_gain = g_calib_mc_gain_l;
        }
#endif
    }
#endif

    iir0_type = anc_cfg_iir0_type;
    if (anc_type & ANC_FEEDFORWARD)
    {
        anc_cfg_iir0_type &= ~ANC_FEEDFORWARD;
        /* ff_l use iir0_ch0*/
        anc_cfg_coef.anc_cfg_ff_l = anc_cfg_coef_new.anc_cfg_ff_l;
#if ((AUD_SECTION_STRUCT_VERSION & 0x1) == 0)
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1) {
            /* stereo ff_r use iir0_ch1*/
            anc_cfg_coef.anc_cfg_ff_r = anc_cfg_coef_new.anc_cfg_ff_r;
        }
#endif
#endif
    }

    if (anc_type & ANC_FEEDBACK)
    {
        anc_cfg_iir0_type &= ~ANC_FEEDBACK;
        /* tws fb_l use iir0_ch1*/
        anc_cfg_coef.anc_cfg_fb_l = anc_cfg_coef_new.anc_cfg_fb_l;
    }

    if (anc_type & ANC_MUSICCANCLE)
    {
        anc_cfg_iir0_type &= ~ANC_MUSICCANCLE;
        /* tws mc_l use iir0_ch1*/
        anc_cfg_coef.anc_cfg_mc_l = anc_cfg_coef_new.anc_cfg_mc_l;
    }

    int_unlock(lock);

    if (anc_type & ANC_FEEDFORWARD || anc_type & ANC_FEEDBACK || anc_type & ANC_MUSICCANCLE)
    {
        LOG_I( "%s:iir0_type:0x%x", __func__, iir0_type);

        anc_set_cfg_internal(&anc_cfg_coef, iir0_type);
    }

    return 0;
}

int anc_set_dehowling_cfg(const dehowling_config *cfg)
{

    return 0;
}

int anc_set_spkcalib_cfg(const struct_spkcalib_cfg *cfg)
{

    return 0;
}
static void anc_ctrl_reg_init(void)
{
    LOG_I( "%s:...", __func__);
    // disable iir clock
    anc_iir_clock->en_clk_iir_anc = 0x0;

    // disable iir 1.
    anc_path_set->codec_tws_anc_eq = 0;

    // disable ANC;
    anc_control->codec_anc_enable_ch0 = 0;
    anc_control->codec_anc_enable_ch1 = 0;

    // using TT as FF ANC
    //  unused tt
    anc_path_set->codec_tt_out_off_ch0 = 1;
    anc_path_set->codec_tt_pdu_off_ch0 = 0;
    anc_path_set->codec_tt_out_off_ch1 = 1;
    anc_path_set->codec_tt_pdu_off_ch1 = 0;
    // anc_path_set->codec_pdu_off_ch0=1;
    // anc_path_set->codec_dehowl_en_ch0=1;

    anc_path_set->codec_pdu_mix_en_ch0 = 0;
    anc_path_set->codec_pdu_mix_en_ch1 = 0;

    // set the FF calib gain;
    anc_iir_calib_gain_config->codec_anc_calib_gain_update_ff_ch0 = 0;
    anc_ff_iir_calib_gain->codec_anc_calib_gain_coef_ff_ch0 = CALIB_GAIN_Q;
    anc_iir_calib_gain_config->codec_anc_calib_gain_update_ff_ch0 = 1;

    anc_iir_calib_gain_config->codec_anc_calib_gain_update_ff_ch1 = 0;
    anc_ff_iir_calib_gain->codec_anc_calib_gain_coef_ff_ch1 = CALIB_GAIN_Q;
    anc_iir_calib_gain_config->codec_anc_calib_gain_update_ff_ch1 = 1;

    // set the FF calib gain;
    anc_iir_calib_gain_config->codec_anc_calib_gain_pass0_ff_ch0 = 1;
    anc_iir_calib_gain_config->codec_anc_calib_gain_pass0_ff_ch1 = 1;

    // set the FB calib gain;
    anc_iir_calib_gain_config->codec_anc_calib_gain_update_fb_ch0 = 0;
    anc_fb_iir_calib_gain->codec_anc_calib_gain_coef_fb_ch0 = CALIB_GAIN_Q;
    anc_iir_calib_gain_config->codec_anc_calib_gain_update_fb_ch0 = 1;

    // set the FB calib gain;
    anc_iir_calib_gain_config->codec_anc_calib_gain_pass0_fb_ch0 = 1;

    tm_config->codec_mm_enable_ch0_eco=0;

    anc_control->codec_anc_rate_sel = 0;

    anc_control->codec_feedback_ch0 = 0;
    anc_control->codec_dual_anc_ch0 = 0;
    anc_control->codec_feedback_ch1 = 0;
    anc_control->codec_dual_anc_ch1 = 0;

    tm_config->iir_ch1_sp_en=1;
    // 8:-120dB 83:-100dB
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    *codec_iir0_gain_ext_th = ANC_GAIN_RAMP_THRESHLD;
#endif

    // iir0
    anc_iir0_control->codec_iir0_enable = 0;
    anc_iir0_control->codec_iir0_iira_enable = 0;
    anc_iir0_control->codec_iir0_iirb_enable = 0;
    anc_iir0_control->codec_iir0_ch0_bypass = 1;
    anc_iir0_control->codec_iir0_ch1_bypass = 1;
    anc_iir0_control->codec_iir0_lmt_ch0_bypass = 1;
    anc_iir0_control->codec_iir0_lmt_ch1_bypass = 1;
    anc_iir0_control->codec_iir0_count_ch0 = 0;
    anc_iir0_control->codec_iir0_count_ch1 = 0;
    anc_iir0_control->codec_iir0_coef_swap = 0;
    anc_iir0_control->codec_iir0_auto_stop = 0;

    // unmute ANC.
    anc_control->codec_anc_mute_ch0 = 0;
    anc_control->codec_anc_mute_ch1 = 0;

    // disable gain updated when pass0
    anc_ff_gain->codec_anc_mute_gain_pass0_ff_ch0 = 1;
    anc_ff_gain->codec_anc_mute_gain_pass0_ff_ch1 = 1;

    anc_fb_gain->codec_anc_mute_gain_pass0_fb_ch0 = 1;

#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK)
    feedback_mc_settings->codec_fb_check_udc_ch0 = 6;
    feedback_mc_settings->codec_fb_check_udc_ch1 = 6;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    fb_check_ch0_config->codec_fb_check_dcf_bypass_ch0 = 0;
#endif
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
    fb_check_ch1_config->codec_fb_check_dcf_bypass_ch1 = 0;
#endif
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    ff_ramp_coef_l = 0;
    fb_ramp_coef_l = 0;
    mc_ramp_coef_l = 0;
#endif
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
    ff_ramp_coef_r = 0;
#endif

#ifdef ANC_FAST_CALIB_MODE
    if (analog_debug_get_anc_calib_mode())
    {
        anc_switching_gain_delay_ff_fb = ANC_SET_GAIN_DELAY_SHORT;
        anc_switching_coef_delay_ff_fb = ANC_SET_GAIN_DELAY_SHORT + ANC_SET_GAIN_CALIB_TIME;
    }
    else
#endif
    {
        anc_switching_gain_delay_ff_fb = ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay_ff_fb = ANC_SET_GAIN_DELAY_LONG + ANC_SET_GAIN_TIME;
    }
    anc_cfg_iir0_type = 0;

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
	#if (defined VOICE_ASSIST_ADA_IIR) && (!defined LITTLE_FF)
	for (int i = 0; i < IIR_FF_COUNTER; i++)
	#else
    for (int i = 0; i < IIR_COUNTER; i++)
	#endif
    {
        ff_filtes_l_old.iir_coef[i].coef_a[0] = 0;
        ff_filtes_l_old.iir_coef[i].coef_a[1] = 0;
        ff_filtes_l_old.iir_coef[i].coef_a[2] = 0;
        ff_filtes_l_old.iir_coef[i].coef_b[0] = 0;
        ff_filtes_l_old.iir_coef[i].coef_b[1] = 0;
        ff_filtes_l_old.iir_coef[i].coef_b[2] = 0;
    }

    ff_filtes_l_old.total_gain = 512;
    ff_filtes_l_old.iir_counter = IIR_FF_COUNTER;
    ff_filtes_l_old.iir_bypass_flag = 0;

	#if (defined VOICE_ASSIST_ADA_IIR) && (!defined LITTLE_FF)
	for (int i = 0; i < IIR_FB_COUNTER; i++)
	#else
    for (int i = 0; i < IIR_COUNTER; i++)
	#endif
    {
        fb_filtes_l_old.iir_coef[i].coef_a[0] = 0;
        fb_filtes_l_old.iir_coef[i].coef_a[1] = 0;
        fb_filtes_l_old.iir_coef[i].coef_a[2] = 0;
        fb_filtes_l_old.iir_coef[i].coef_b[0] = 0;
        fb_filtes_l_old.iir_coef[i].coef_b[1] = 0;
        fb_filtes_l_old.iir_coef[i].coef_b[2] = 0;
    }

    fb_filtes_l_old.total_gain = 512;
    fb_filtes_l_old.iir_counter = IIR_FB_COUNTER;
    fb_filtes_l_old.iir_bypass_flag = 0;

#if defined(AUDIO_ANC_FB_MC_HW)
	#if (defined VOICE_ASSIST_ADA_IIR) && (!defined LITTLE_FF)
	for(int i = 0; i < IIR_MC_COUNTER; i++)
	#else
    for(int i = 0; i < IIR_COUNTER; i++)
	#endif
    {
        mc_filtes_l_old.iir_coef[i].coef_a[0] = 0;
        mc_filtes_l_old.iir_coef[i].coef_a[1] = 0;
        mc_filtes_l_old.iir_coef[i].coef_a[2] = 0;
        mc_filtes_l_old.iir_coef[i].coef_b[0] = 0;
        mc_filtes_l_old.iir_coef[i].coef_b[1] = 0;
        mc_filtes_l_old.iir_coef[i].coef_b[2] = 0;
    }
#endif
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
    for(int i=0;i<IIR_COUNTER;i++)
    {
        ff_filtes_r_old.iir_coef[i].coef_a[0]=0;
        ff_filtes_r_old.iir_coef[i].coef_a[1]=0;
        ff_filtes_r_old.iir_coef[i].coef_a[2]=0;
        ff_filtes_r_old.iir_coef[i].coef_b[0]=0;
        ff_filtes_r_old.iir_coef[i].coef_b[1]=0;
        ff_filtes_r_old.iir_coef[i].coef_b[2]=0;
    }

    ff_filtes_r_old.total_gain=512;
    ff_filtes_r_old.iir_counter=IIR_FF_COUNTER;
    ff_filtes_r_old.iir_bypass_flag=0;
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

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
static void anc_iir0_init(void)
{
    anc_iir_coefs iir_coef_gain_ramp_ff;
    anc_iir_coefs iir_coef_gain_ramp_fb;

    // enable iir0 clock
    anc_iir_clock->en_clk_iir_anc |= CODEC_EN_CLK_IIR_IIR0;

#ifdef ANC_FAST_CALIB_MODE
    if (analog_debug_get_anc_calib_mode())
    {
        iir_coef_gain_ramp_ff = iir_coef_gain_ramp_ff_calib;
        iir_coef_gain_ramp_fb = iir_coef_gain_ramp_fb_calib;
    }
    else
#endif
    {
        iir_coef_gain_ramp_ff = iir_coef_gain_ramp_ff_normal;
        iir_coef_gain_ramp_fb = iir_coef_gain_ramp_fb_normal;
    }

    anc_iir0_control->codec_iir0_enable = 0;

    anc_iir0_control->codec_iir0_iira_enable = 0;
    anc_iir0_control->codec_iir0_iirb_enable = 0;

    anc_iir0_control->codec_iir0_coef_swap = 0;
    anc_iir0_control->codec_iir0_auto_stop = 0;

#ifdef ANC_GAIN_RAMP
    /* ff_l use iir0_ch0*/
    anc_gain_ff_iir_coefs0_l[0].a1 = -iir_coef_gain_ramp_ff.coef_a[1];
    anc_gain_ff_iir_coefs0_l[0].a2 = -iir_coef_gain_ramp_ff.coef_a[2];
    anc_gain_ff_iir_coefs0_l[0].b0 = iir_coef_gain_ramp_ff.coef_b[0];
    anc_gain_ff_iir_coefs0_l[0].b1 = iir_coef_gain_ramp_ff.coef_b[1];
    anc_gain_ff_iir_coefs0_l[0].b2 = iir_coef_gain_ramp_ff.coef_b[2];

    anc_gain_ff_iir_coefs1_l[0].a1 = -iir_coef_gain_ramp_ff.coef_a[1];
    anc_gain_ff_iir_coefs1_l[0].a2 = -iir_coef_gain_ramp_ff.coef_a[2];
    anc_gain_ff_iir_coefs1_l[0].b0 = iir_coef_gain_ramp_ff.coef_b[0];
    anc_gain_ff_iir_coefs1_l[0].b1 = iir_coef_gain_ramp_ff.coef_b[1];
    anc_gain_ff_iir_coefs1_l[0].b2 = iir_coef_gain_ramp_ff.coef_b[2];

    anc_iir0_control->codec_iir0_gaincal_ext_ch0_bypass = 0;
    anc_iir0_control->codec_iir0_gainuse_ext_ch0_bypass = 0;
    anc_iir_gain_update->codec_iir0_gain_ext_sel_ch0 = 0;

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1) {
        /* stereo ff_r use iir0_ch1*/
        anc_gain_ff_iir_coefs0_r[0].a1 = -iir_coef_gain_ramp_ff.coef_a[1];
        anc_gain_ff_iir_coefs0_r[0].a2 = -iir_coef_gain_ramp_ff.coef_a[2];
        anc_gain_ff_iir_coefs0_r[0].b0 = iir_coef_gain_ramp_ff.coef_b[0];
        anc_gain_ff_iir_coefs0_r[0].b1 = iir_coef_gain_ramp_ff.coef_b[1];
        anc_gain_ff_iir_coefs0_r[0].b2 = iir_coef_gain_ramp_ff.coef_b[2];

        anc_gain_ff_iir_coefs1_r[0].a1 = -iir_coef_gain_ramp_ff.coef_a[1];
        anc_gain_ff_iir_coefs1_r[0].a2 = -iir_coef_gain_ramp_ff.coef_a[2];
        anc_gain_ff_iir_coefs1_r[0].b0 = iir_coef_gain_ramp_ff.coef_b[0];
        anc_gain_ff_iir_coefs1_r[0].b1 = iir_coef_gain_ramp_ff.coef_b[1];
        anc_gain_ff_iir_coefs1_r[0].b2 = iir_coef_gain_ramp_ff.coef_b[2];
    } else
#endif
    {
        /* tws fb_l use iir0_ch1*/
        anc_gain_fb_iir_coefs0_l[0].a1 = -iir_coef_gain_ramp_fb.coef_a[1];
        anc_gain_fb_iir_coefs0_l[0].a2 = -iir_coef_gain_ramp_fb.coef_a[2];
        anc_gain_fb_iir_coefs0_l[0].b0 = iir_coef_gain_ramp_fb.coef_b[0];
        anc_gain_fb_iir_coefs0_l[0].b1 = iir_coef_gain_ramp_fb.coef_b[1];
        anc_gain_fb_iir_coefs0_l[0].b2 = iir_coef_gain_ramp_fb.coef_b[2];

        anc_gain_fb_iir_coefs1_l[0].a1 = -iir_coef_gain_ramp_fb.coef_a[1];
        anc_gain_fb_iir_coefs1_l[0].a2 = -iir_coef_gain_ramp_fb.coef_a[2];
        anc_gain_fb_iir_coefs1_l[0].b0 = iir_coef_gain_ramp_fb.coef_b[0];
        anc_gain_fb_iir_coefs1_l[0].b1 = iir_coef_gain_ramp_fb.coef_b[1];
        anc_gain_fb_iir_coefs1_l[0].b2 = iir_coef_gain_ramp_fb.coef_b[2];
    }

    anc_iir0_control->codec_iir0_gaincal_ext_ch1_bypass = 0;
    anc_iir0_control->codec_iir0_gainuse_ext_ch1_bypass = 0;
    anc_iir_gain_update->codec_iir0_gain_ext_sel_ch1 = 0;
#endif

#if defined(ANC_LIMITER_IIR0_CH0)
    /* ff_l use iir0_ch0*/
    anc_limiter_ff_iir_coefs0_l[0].a1 = -iir_coef_limiter_attack_ff.coef_a[1];
    anc_limiter_ff_iir_coefs0_l[0].a2 = -iir_coef_limiter_attack_ff.coef_a[2];
    anc_limiter_ff_iir_coefs0_l[0].b0 = iir_coef_limiter_attack_ff.coef_b[0];
    anc_limiter_ff_iir_coefs0_l[0].b1 = iir_coef_limiter_attack_ff.coef_b[1];
    anc_limiter_ff_iir_coefs0_l[0].b2 = iir_coef_limiter_attack_ff.coef_b[2];

    anc_limiter_ff_iir_coefs0_l[1].a1 = -iir_coef_limiter_release_ff.coef_a[1];
    anc_limiter_ff_iir_coefs0_l[1].a2 = -iir_coef_limiter_release_ff.coef_a[2];
    anc_limiter_ff_iir_coefs0_l[1].b0 = iir_coef_limiter_release_ff.coef_b[0];
    anc_limiter_ff_iir_coefs0_l[1].b1 = iir_coef_limiter_release_ff.coef_b[1];
    anc_limiter_ff_iir_coefs0_l[1].b2 = iir_coef_limiter_release_ff.coef_b[2];

    anc_limiter_ff_iir_coefs1_l[0].a1 = -iir_coef_limiter_attack_ff.coef_a[1];
    anc_limiter_ff_iir_coefs1_l[0].a2 = -iir_coef_limiter_attack_ff.coef_a[2];
    anc_limiter_ff_iir_coefs1_l[0].b0 = iir_coef_limiter_attack_ff.coef_b[0];
    anc_limiter_ff_iir_coefs1_l[0].b1 = iir_coef_limiter_attack_ff.coef_b[1];
    anc_limiter_ff_iir_coefs1_l[0].b2 = iir_coef_limiter_attack_ff.coef_b[2];

    anc_limiter_ff_iir_coefs1_l[1].a1 = -iir_coef_limiter_release_ff.coef_a[1];
    anc_limiter_ff_iir_coefs1_l[1].a2 = -iir_coef_limiter_release_ff.coef_a[2];
    anc_limiter_ff_iir_coefs1_l[1].b0 = iir_coef_limiter_release_ff.coef_b[0];
    anc_limiter_ff_iir_coefs1_l[1].b1 = iir_coef_limiter_release_ff.coef_b[1];
    anc_limiter_ff_iir_coefs1_l[1].b2 = iir_coef_limiter_release_ff.coef_b[2];

    ff_limiter_att_l_old = iir_coef_limiter_attack_ff;
    ff_limiter_rls_l_old = iir_coef_limiter_release_ff;
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
        anc_iir0_control->codec_iir0_lmt_ch0_bypass = 0;
    }

    /* ff_l use iir0_ch0*/
    // update the threshold
    anc_iir_gain_update->codec_iir0_lmt_th_update_ch0 = 0;
    // limiter threshold, 0: 0x7fffff
    *codec_iir0_lmt_th_ch0 = ANC_LIMITER_FF_THREHOLD;
    hal_sys_timer_delay_us(1);
    anc_iir_gain_update->codec_iir0_lmt_th_update_ch0 = 1;

    anc_iir_lmt_delay_ch0->codec_iir0_lmt_delay = ANC_LIMITER_FF_ATTACK_DELAY;
#else
    anc_iir0_control->codec_iir0_lmt_ch0_bypass = 1;
#endif

#if defined(ANC_LIMITER_IIR0_CH1)
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1) {
        /* stereo ff_r use iir0_ch1*/
        anc_limiter_ff_iir_coefs0_r[0].a1 = -iir_coef_limiter_attack_ff.coef_a[1];
        anc_limiter_ff_iir_coefs0_r[0].a2 = -iir_coef_limiter_attack_ff.coef_a[2];
        anc_limiter_ff_iir_coefs0_r[0].b0 = iir_coef_limiter_attack_ff.coef_b[0];
        anc_limiter_ff_iir_coefs0_r[0].b1 = iir_coef_limiter_attack_ff.coef_b[1];
        anc_limiter_ff_iir_coefs0_r[0].b2 = iir_coef_limiter_attack_ff.coef_b[2];

        anc_limiter_ff_iir_coefs0_r[1].a1 = -iir_coef_limiter_release_ff.coef_a[1];
        anc_limiter_ff_iir_coefs0_r[1].a2 = -iir_coef_limiter_release_ff.coef_a[2];
        anc_limiter_ff_iir_coefs0_r[1].b0 = iir_coef_limiter_release_ff.coef_b[0];
        anc_limiter_ff_iir_coefs0_r[1].b1 = iir_coef_limiter_release_ff.coef_b[1];
        anc_limiter_ff_iir_coefs0_r[1].b2 = iir_coef_limiter_release_ff.coef_b[2];

        anc_limiter_ff_iir_coefs1_r[0].a1 = -iir_coef_limiter_attack_ff.coef_a[1];
        anc_limiter_ff_iir_coefs1_r[0].a2 = -iir_coef_limiter_attack_ff.coef_a[2];
        anc_limiter_ff_iir_coefs1_r[0].b0 = iir_coef_limiter_attack_ff.coef_b[0];
        anc_limiter_ff_iir_coefs1_r[0].b1 = iir_coef_limiter_attack_ff.coef_b[1];
        anc_limiter_ff_iir_coefs1_r[0].b2 = iir_coef_limiter_attack_ff.coef_b[2];

        anc_limiter_ff_iir_coefs1_r[1].a1 = -iir_coef_limiter_release_ff.coef_a[1];
        anc_limiter_ff_iir_coefs1_r[1].a2 = -iir_coef_limiter_release_ff.coef_a[2];
        anc_limiter_ff_iir_coefs1_r[1].b0 = iir_coef_limiter_release_ff.coef_b[0];
        anc_limiter_ff_iir_coefs1_r[1].b1 = iir_coef_limiter_release_ff.coef_b[1];
        anc_limiter_ff_iir_coefs1_r[1].b2 = iir_coef_limiter_release_ff.coef_b[2];

        ff_limiter_att_r_old = iir_coef_limiter_attack_ff;
        ff_limiter_rls_r_old = iir_coef_limiter_release_ff;

    } else
#endif
    {
        /* tws fb_l use iir0_ch1*/
        anc_limiter_fb_iir_coefs0_l[0].a1 = -iir_coef_limiter_attack_fb.coef_a[1];
        anc_limiter_fb_iir_coefs0_l[0].a2 = -iir_coef_limiter_attack_fb.coef_a[2];
        anc_limiter_fb_iir_coefs0_l[0].b0 = iir_coef_limiter_attack_fb.coef_b[0];
        anc_limiter_fb_iir_coefs0_l[0].b1 = iir_coef_limiter_attack_fb.coef_b[1];
        anc_limiter_fb_iir_coefs0_l[0].b2 = iir_coef_limiter_attack_fb.coef_b[2];

        anc_limiter_fb_iir_coefs0_l[1].a1 = -iir_coef_limiter_release_fb.coef_a[1];
        anc_limiter_fb_iir_coefs0_l[1].a2 = -iir_coef_limiter_release_fb.coef_a[2];
        anc_limiter_fb_iir_coefs0_l[1].b0 = iir_coef_limiter_release_fb.coef_b[0];
        anc_limiter_fb_iir_coefs0_l[1].b1 = iir_coef_limiter_release_fb.coef_b[1];
        anc_limiter_fb_iir_coefs0_l[1].b2 = iir_coef_limiter_release_fb.coef_b[2];

        anc_limiter_fb_iir_coefs1_l[0].a1 = -iir_coef_limiter_attack_fb.coef_a[1];
        anc_limiter_fb_iir_coefs1_l[0].a2 = -iir_coef_limiter_attack_fb.coef_a[2];
        anc_limiter_fb_iir_coefs1_l[0].b0 = iir_coef_limiter_attack_fb.coef_b[0];
        anc_limiter_fb_iir_coefs1_l[0].b1 = iir_coef_limiter_attack_fb.coef_b[1];
        anc_limiter_fb_iir_coefs1_l[0].b2 = iir_coef_limiter_attack_fb.coef_b[2];

        anc_limiter_fb_iir_coefs1_l[1].a1 = -iir_coef_limiter_release_fb.coef_a[1];
        anc_limiter_fb_iir_coefs1_l[1].a2 = -iir_coef_limiter_release_fb.coef_a[2];
        anc_limiter_fb_iir_coefs1_l[1].b0 = iir_coef_limiter_release_fb.coef_b[0];
        anc_limiter_fb_iir_coefs1_l[1].b1 = iir_coef_limiter_release_fb.coef_b[1];
        anc_limiter_fb_iir_coefs1_l[1].b2 = iir_coef_limiter_release_fb.coef_b[2];

#if defined(ANC_LIMITER_MC)
#if defined(AUDIO_ANC_FB_MC_HW)
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

        mc_limiter_att_l_old=iir_coef_limiter_attack_mc;
        mc_limiter_rls_l_old=iir_coef_limiter_release_mc;
#endif
#endif
        fb_limiter_att_l_old = iir_coef_limiter_attack_fb;
        fb_limiter_rls_l_old = iir_coef_limiter_release_fb;
    }
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
        anc_iir0_control->codec_iir0_lmt_ch1_bypass = 0;
    }

    // update the threshold
    anc_iir_gain_update->codec_iir0_lmt_th_update_ch1 = 0;
    // limiter threshold, 0: 0x7fffff
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1) {
        /* stereo ff_r use iir0_ch1*/
        *codec_iir0_lmt_th_ch1 = ANC_LIMITER_FF_THREHOLD;
    } else
#endif
    {
        /* tws fb_l use iir0_ch1*/
        *codec_iir0_lmt_th_ch1 = ANC_LIMITER_FB_THREHOLD;
    }
    hal_sys_timer_delay_us(1);
    anc_iir_gain_update->codec_iir0_lmt_th_update_ch1 = 1;

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1) {
        /* stereo ff_r use iir0_ch1*/
        anc_iir_lmt_delay_ch1->codec_iir0_lmt_delay = ANC_LIMITER_FF_ATTACK_DELAY;
    } else
#endif
    {
        /* tws fb_l use iir0_ch1*/
        anc_iir_lmt_delay_ch1->codec_iir0_lmt_delay = ANC_LIMITER_FB_ATTACK_DELAY;
    }

#else
    anc_iir0_control->codec_iir0_lmt_ch1_bypass = 1;
#endif
	#if (defined VOICE_ASSIST_ADA_IIR) && (!defined LITTLE_FF)
	for (int i = 0; i < IIR_FF_COUNTER; i++)
	#else
    for (int i = 0; i < IIR_COUNTER; i++)
	#endif
    {
        /* ff_l use iir0_ch0*/
        anc_ff_iir_coefs0_l[i].a1 = 0;
        anc_ff_iir_coefs0_l[i].a2 = 0;
        anc_ff_iir_coefs0_l[i].b0 = 0;
        anc_ff_iir_coefs0_l[i].b1 = 0;
        anc_ff_iir_coefs0_l[i].b2 = 0;

        anc_ff_iir_coefs1_l[i].a1 = 0;
        anc_ff_iir_coefs1_l[i].a2 = 0;
        anc_ff_iir_coefs1_l[i].b0 = 0;
        anc_ff_iir_coefs1_l[i].b1 = 0;
        anc_ff_iir_coefs1_l[i].b2 = 0;
	#if (defined VOICE_ASSIST_ADA_IIR) && (!defined LITTLE_FF)
	}
    for (int i = 0; i < IIR_FB_COUNTER; i++) {
	#endif
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1) {
            /* stereo ff_r use iir0_ch1*/
            anc_ff_iir_coefs0_r[i].a1 = 0;
            anc_ff_iir_coefs0_r[i].a2 = 0;
            anc_ff_iir_coefs0_r[i].b0 = 0;
            anc_ff_iir_coefs0_r[i].b1 = 0;
            anc_ff_iir_coefs0_r[i].b2 = 0;

            anc_ff_iir_coefs1_r[i].a1 = 0;
            anc_ff_iir_coefs1_r[i].a2 = 0;
            anc_ff_iir_coefs1_r[i].b0 = 0;
            anc_ff_iir_coefs1_r[i].b1 = 0;
            anc_ff_iir_coefs1_r[i].b2 = 0;
        } else
#endif
        {
            /* tws fb_l use iir0_ch1*/
            anc_fb_iir_coefs0_l[i].a1 = 0;
            anc_fb_iir_coefs0_l[i].a2 = 0;
            anc_fb_iir_coefs0_l[i].b0 = 0;
            anc_fb_iir_coefs0_l[i].b1 = 0;
            anc_fb_iir_coefs0_l[i].b2 = 0;

            anc_fb_iir_coefs1_l[i].a1 = 0;
            anc_fb_iir_coefs1_l[i].a2 = 0;
            anc_fb_iir_coefs1_l[i].b0 = 0;
            anc_fb_iir_coefs1_l[i].b1 = 0;
            anc_fb_iir_coefs1_l[i].b2 = 0;
        }
    }

    /* ff_l use iir0_ch0*/
    anc_iir0_control->codec_iir0_ch0_bypass = 0;
    anc_iir0_control->codec_iir0_count_ch0 = IIR_FF_COUNTER;

    anc_iir0_control->codec_iir0_ch1_bypass = 0;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1) {
        /* stereo ff_r use iir0_ch1*/
        anc_iir0_control->codec_iir0_count_ch1 = IIR_FF_COUNTER;
    } else
#endif
    {
        /* tws fb_l mc_l use iir0_ch1*/
        anc_iir0_control->codec_iir0_count_ch1 = IIR_FB_COUNTER;
    }

    anc_iir0_control->codec_iir0_auto_stop = 1;
    anc_iir0_control->codec_iir0_iira_enable = 1;
    anc_iir0_control->codec_iir0_iirb_enable = 1;
    anc_iir0_control->codec_iir0_enable = 1;
    iir0_coef_using = 0;
}
#endif

static void anc_ctrl_reg_open(enum ANC_TYPE_T anc_type)
{
    LOG_I( "%s", __func__);

    if (anc_type == ANC_FEEDFORWARD)
    {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
        {
            // set the FF gain;
#ifdef ANC_GAIN_RAMP
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch0 = 0;
            anc_ff_gain->codec_anc_mute_gain_ff_ch0 = 512;
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch0 = 1;
            ff_ramp_gain_l = 0;
            ff_ramp_coef_l = 0;
#else
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch0 = 0;
            anc_ff_gain->codec_anc_mute_gain_ff_ch0 = 0;
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch0 = 1;
#endif
			#if (defined VOICE_ASSIST_ADA_IIR) && (!defined LITTLE_FF)
			for (int i = 0; i < IIR_FF_COUNTER; i++)
			#else
            for (int i = 0; i < IIR_COUNTER; i++)
			#endif
            {
                ff_filtes_l_old.iir_coef[i].coef_a[0] = 0;
                ff_filtes_l_old.iir_coef[i].coef_a[1] = 0;
                ff_filtes_l_old.iir_coef[i].coef_a[2] = 0;
                ff_filtes_l_old.iir_coef[i].coef_b[0] = 0;
                ff_filtes_l_old.iir_coef[i].coef_b[1] = 0;
                ff_filtes_l_old.iir_coef[i].coef_b[2] = 0;
            }

            ff_filtes_l_old.total_gain = 512;
            ff_filtes_l_old.iir_counter = IIR_FF_COUNTER;
            ff_filtes_l_old.iir_bypass_flag = 0;

            if (fb_open_flag == 0 && mc_open_flag == 0)
            {
                anc_iir0_init();
            }
        }
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1)
        {
            // set the FF gain;
#ifdef ANC_GAIN_RAMP
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch1 = 0;
            anc_ff_gain->codec_anc_mute_gain_ff_ch1 = 512;
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch1 = 1;
            ff_ramp_gain_r = 0;
            ff_ramp_coef_r = 0;
#else
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch1 = 0;
            anc_ff_gain->codec_anc_mute_gain_ff_ch1 = 0;
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch1 = 1;
#endif

            for (int i = 0; i < IIR_COUNTER; i++)
            {
                ff_filtes_r_old.iir_coef[i].coef_a[0] = 0;
                ff_filtes_r_old.iir_coef[i].coef_a[1] = 0;
                ff_filtes_r_old.iir_coef[i].coef_a[2] = 0;
                ff_filtes_r_old.iir_coef[i].coef_b[0] = 0;
                ff_filtes_r_old.iir_coef[i].coef_b[1] = 0;
                ff_filtes_r_old.iir_coef[i].coef_b[2] = 0;
            }

            ff_filtes_r_old.total_gain = 512;
            ff_filtes_r_old.iir_counter = IIR_FF_COUNTER;
            ff_filtes_r_old.iir_bypass_flag = 0;

            anc_iir0_init();
        }
#endif
    }

    if (anc_type == ANC_FEEDBACK)
    {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
        {
            // set the FB gain;
#ifdef ANC_GAIN_RAMP
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch0 = 0;
            anc_fb_gain->codec_anc_mute_gain_fb_ch0 = 512;
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch0 = 1;
            fb_ramp_gain_l = 0;
            fb_ramp_coef_l = 0;
#else
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch0 = 0;
            anc_fb_gain->codec_anc_mute_gain_fb_ch0 = 0;
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch0 = 1;
#endif
			#if (defined VOICE_ASSIST_ADA_IIR) && (!defined LITTLE_FF)
			for (int i = 0; i < IIR_FB_COUNTER; i++)
			#else
            for (int i = 0; i < IIR_COUNTER; i++)
			#endif
            {
                fb_filtes_l_old.iir_coef[i].coef_a[0] = 0;
                fb_filtes_l_old.iir_coef[i].coef_a[1] = 0;
                fb_filtes_l_old.iir_coef[i].coef_a[2] = 0;
                fb_filtes_l_old.iir_coef[i].coef_b[0] = 0;
                fb_filtes_l_old.iir_coef[i].coef_b[1] = 0;
                fb_filtes_l_old.iir_coef[i].coef_b[2] = 0;
            }

            fb_filtes_l_old.total_gain = 512;
            fb_filtes_l_old.iir_counter = IIR_FB_COUNTER;
            fb_filtes_l_old.iir_bypass_flag = 0;

            if (ff_open_flag == 0 && mc_open_flag == 0)
            {
                anc_iir0_init();
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
#endif
    }

#if defined(AUDIO_ANC_FB_MC_HW)
    if(anc_type==ANC_MUSICCANCLE)
    {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if(anc_output_ch_map&AUD_CHANNEL_MAP_CH0)
        {
            //set the MC gain;
#ifdef ANC_GAIN_RAMP
            mc_ramp_gain_l=0;
            mc_ramp_coef_l=0;
#endif
			#if (defined VOICE_ASSIST_ADA_IIR) && (!defined LITTLE_FF)
			for(int i=0;i<IIR_MC_COUNTER;i++)
			#else
            for(int i=0;i<IIR_COUNTER;i++)
			#endif
            {
                mc_filtes_l_old.iir_coef[i].coef_a[0]=0;
                mc_filtes_l_old.iir_coef[i].coef_a[1]=0;
                mc_filtes_l_old.iir_coef[i].coef_a[2]=0;
                mc_filtes_l_old.iir_coef[i].coef_b[0]=0;
                mc_filtes_l_old.iir_coef[i].coef_b[1]=0;
                mc_filtes_l_old.iir_coef[i].coef_b[2]=0;
            }

            if(ff_open_flag == 0 && fb_open_flag == 0)
            {
                anc_iir0_init();
            }

            tm_config->codec_mm_enable_ch0_eco=1;
        }
#endif
    }
#endif

    if (ff_open_flag == 1 && fb_open_flag == 1)
    {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
        {
            anc_control->codec_feedback_ch0 = 1;
            anc_control->codec_dual_anc_ch0 = 1;
            anc_control->codec_anc_enable_ch0 = 1;
        }
        if (mc_open_flag == 1)
        {
            tm_config->iir_ch1_sp_en=1;
        }
#endif
    }
    else if (ff_open_flag == 0 && fb_open_flag == 1)
    {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
        {
            anc_control->codec_feedback_ch0 = 1;
            anc_control->codec_dual_anc_ch0 = 0;
            anc_control->codec_anc_enable_ch0 = 1;
        }
        if (mc_open_flag == 1)
        {
            tm_config->iir_ch1_sp_en=1;
            anc_control->codec_dual_anc_ch0 = 1;
        }
#endif
    }
    else if (ff_open_flag == 1 && fb_open_flag == 0)
    {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
        {
            anc_control->codec_feedback_ch0 = 0;
            anc_control->codec_dual_anc_ch0 = 0;
            anc_control->codec_anc_enable_ch0 = 1;
        }
#endif
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1)
        {
            anc_control->codec_feedback_ch1 = 0;
            anc_control->codec_dual_anc_ch1 = 0;
            anc_control->codec_anc_enable_ch1 = 1;
        }
#endif
    }
}

static void anc_ctrl_reg_close(enum ANC_TYPE_T anc_type)
{
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
    {
        if (anc_type == ANC_FEEDFORWARD)
        {
            // set the FF gain;
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch0 = 0;
            anc_ff_gain->codec_anc_mute_gain_ff_ch0 = 0;
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch0 = 1;
        }

        if (anc_type == ANC_FEEDBACK)
        {
            // set the FB gain;
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch0 = 0;
            anc_fb_gain->codec_anc_mute_gain_fb_ch0 = 0;
            anc_fb_gain->codec_anc_mute_gain_update_fb_ch0 = 1;

#if defined(AUDIO_ANC_FB_MC) ||defined(AUDIO_ANC_FB_MC_HW)
            fb_check_ch0_config->codec_fb_check_keep_ch0=0;
            feedback_mc_settings->codec_feedback_mc_en_ch0=0;
#endif
        }

        if (ff_open_flag == 1 && fb_open_flag == 1)
        {
            anc_control->codec_feedback_ch0 = 1;
            anc_control->codec_dual_anc_ch0 = 1;
        }
        else if (ff_open_flag == 0 && fb_open_flag == 1)
        {
            anc_control->codec_feedback_ch0 = 1;
            anc_control->codec_dual_anc_ch0 = 0;
        }
        else if (ff_open_flag == 1 && fb_open_flag == 0)
        {
            anc_control->codec_feedback_ch0 = 0;
            anc_control->codec_dual_anc_ch0 = 0;
        }
    }
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1)
    {
        if (anc_type == ANC_FEEDFORWARD)
        {
            // set the FF gain;
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch1 = 0;
            anc_ff_gain->codec_anc_mute_gain_ff_ch1 = 0;
            anc_ff_gain->codec_anc_mute_gain_update_ff_ch1 = 1;
        }

        if (ff_open_flag == 1)
        {
            anc_control->codec_feedback_ch1 = 0;
            anc_control->codec_dual_anc_ch1 = 0;
        }
    }
#endif

    if (ff_open_flag == 0 && fb_open_flag == 0 && mc_open_flag ==0)
    {
        // disable iir0 clock
        anc_iir_clock->en_clk_iir_anc &= ~CODEC_EN_CLK_IIR_IIR0;
    }
}

int anc_opened(enum ANC_TYPE_T anc_type)
{
    int32_t open_flag = 0;

    if (anc_type & ANC_FEEDFORWARD)
    {
        open_flag = open_flag | (ff_open_flag << 1);
    }
    if (anc_type & ANC_FEEDBACK)
    {
        open_flag = open_flag | (fb_open_flag << 2);
    }
#if defined(AUDIO_ANC_FB_MC_HW)
    if (anc_type & ANC_MUSICCANCLE)
    {
        open_flag = open_flag | (mc_open_flag << 4);
    }
#endif
    return open_flag;
}

#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK)
static int32_t howling_check_delay = 5000; // recovery time ms.
static int32_t howling_timer = 400; // ms.
static int32_t howling_set_gain_level = 0; // ms.

#define HOWLING_GAIN_0dB (512)
#define HOWLING_GAIN_m3dB (362)
#define HOWLING_GAIN_m6dB (256)
#define HOWLING_GAIN_m9dB (181)
#define HOWLING_GAIN_m12dB (128)

static void anc_howling_check_irq_handler(uint32_t status)
{
    // LOG_I("%s,", __func__);
    struct _codec_int_config *codec_int_config = (struct _codec_int_config *)&status;
    uint32 howling_time = TICKS_TO_MS(hal_sys_timer_get());

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
    {
        if (codec_int_config->fb_check_error_trig_ch0)
        {
            LOG_I( "howling_cnt_l:%d", howling_cnt_l);

            LOG_I( "*Threshold:%10d,Data Energy :%10d", *codec_fb_check_threshold_ch0, *codec_fb_check_data_avg_keep_ch0);

            if (howling_cnt_l == 0)
            {
#if defined(ANC_FF_CHECK)
                ff_howling_gain_l = HOWLING_GAIN_m3dB;
#elif defined(ANC_FB_CHECK)
                fb_howling_gain_l = HOWLING_GAIN_m3dB;
#endif
                howling_cnt_l = 1;
                howling_time_l = howling_time;
            }
            else if (howling_cnt_l == 1 && (howling_time > howling_time_l + howling_timer))
            {
#if defined(ANC_FF_CHECK)
                ff_howling_gain_l = HOWLING_GAIN_m6dB;
#elif defined(ANC_FB_CHECK)
                fb_howling_gain_l = HOWLING_GAIN_m6dB;
#endif
                howling_cnt_l = 2;
                howling_time_l = howling_time;
            }
            else if (howling_cnt_l == 2 && (howling_time > howling_time_l + howling_timer))
            {
#if defined(ANC_FF_CHECK)
                ff_howling_gain_l = HOWLING_GAIN_m9dB;
#elif defined(ANC_FB_CHECK)
                fb_howling_gain_l = HOWLING_GAIN_m9dB;
#endif
                howling_cnt_l = 3;
                howling_time_l = howling_time;
            }
            else if (howling_cnt_l == 3 && (howling_time > howling_time_l + howling_timer))
            {
#if defined(ANC_FF_CHECK)
                ff_howling_gain_l = 0;
#elif defined(ANC_FB_CHECK)
                fb_howling_gain_l = 0;
#endif
                howling_cnt_l = 4;
            }

#if defined(ANC_FF_CHECK)
            LOG_I( "ff_howling_gain_l:%d", ff_howling_gain_l);
#elif defined(ANC_FB_CHECK)
            LOG_I( "fb_howling_gain_l:%d", fb_howling_gain_l);
#endif

            fb_check_ch0_config->codec_fb_check_enable_ch0 = 0;

            hwtimer_stop(anc_howling_check_dev_timer_l);
            hwtimer_start(anc_howling_check_dev_timer_l, howling_check_delay);
#if defined(ANC_FF_CHECK)
            iir0_ch0_gaina_cfg_gain(true);
#elif defined(ANC_FB_CHECK)
            iir0_ch1_gaina_cfg_gain(true);
#if defined(AUDIO_ANC_FB_MC_HW)
            iir0_ch1_gainb_cfg_gain(true); //FB & MC use same ch, need mute gain at same time.
#endif
#endif
            fb_check_ch0_config->codec_fb_check_enable_ch0 = 1;
        }
    }
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1)
    {
        if (codec_int_config->fb_check_error_trig_ch1)
        {
            LOG_I( "howling_cnt_r:%d", howling_cnt_r);

            LOG_I( "*Threshold:%10d,Data Energy :%10d", *codec_fb_check_threshold_ch1, *codec_fb_check_data_avg_keep_ch1);

            if (howling_cnt_r == 0)
            {
#if defined(ANC_FF_CHECK)
                ff_howling_gain_r = HOWLING_GAIN_m3dB;
#endif
                howling_cnt_r = 1;
                howling_time_r = howling_time;
            }
            else if (howling_cnt_r == 1 && (howling_time > howling_time_r + howling_timer))
            {
#if defined(ANC_FF_CHECK)
                ff_howling_gain_r = HOWLING_GAIN_m6dB;
#endif
                howling_cnt_r = 2;
                howling_time_r = howling_time;
            }
            else if (howling_cnt_r == 2 && (howling_time > howling_time_r + howling_timer))
            {
#if defined(ANC_FF_CHECK)
                ff_howling_gain_r = HOWLING_GAIN_m9dB;
#endif
                howling_cnt_r = 3;
                howling_time_r = howling_time;
            }
            else if (howling_cnt_r == 3 && (howling_time > howling_time_r + howling_timer))
            {
#if defined(ANC_FF_CHECK)
                ff_howling_gain_r = 0;
#endif
                howling_cnt_r = 4;
            }

#if defined(ANC_FF_CHECK)
            LOG_I( "ff_howling_gain_r:%d", ff_howling_gain_r);
#endif

            fb_check_ch1_config->codec_fb_check_enable_ch1 = 0;

            hwtimer_stop(anc_howling_check_dev_timer_r);
            hwtimer_start(anc_howling_check_dev_timer_r, howling_check_delay);
#if defined(ANC_FF_CHECK)
            iir0_ch1_gaina_cfg_gain(true);
#endif
            fb_check_ch1_config->codec_fb_check_enable_ch1 = 1;
        }
    }
#endif
}

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
static void anc_howling_check_timer_handler(void *param)
{
    LOG_I( "%s", __func__);
#if defined(ANC_FF_CHECK)
    ff_howling_gain_l = HOWLING_GAIN_0dB;
    iir0_ch0_gaina_cfg_gain(true);
#elif defined(ANC_FB_CHECK)
    fb_howling_gain_l = HOWLING_GAIN_0dB;
    iir0_ch1_gaina_cfg_gain(true);
#if defined(AUDIO_ANC_FB_MC_HW)
    iir0_ch1_gainb_cfg_gain(true);
#endif
#endif
    howling_cnt_l = howling_set_gain_level;
    howling_time_l = 0;
}
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
#if defined(ANC_FF_CHECK)
POSSIBLY_UNUSED static void anc_right_howling_check_timer_handler(void *param)

{
    LOG_I("%s", __func__);

    ff_howling_gain_r=HOWLING_GAIN_0dB;
    iir0_ch1_gaina_cfg_gain(true);

    howling_cnt_r = howling_set_gain_level;
    howling_time_r = 0;
}
#endif
#endif
#endif

int32_t anc_process_set_cfg(const anc_process_cfg * anc_cfg)
{
    POSSIBLY_UNUSED const anc_howling_check_cfg  *howling_check_cfg = &(anc_cfg->howling_check_cfg);
    LOG_I("%s: recover_time=%d(ms), interval_time=%d, gain_level=%d", __func__,
                                            howling_check_cfg->howling_recover_time,
                                            howling_check_cfg->howling_interval_time,
                                            howling_check_cfg->howling_set_gain_level);

#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK)|| defined(ANC_TT_CHECK)
    howling_check_delay = ((MS_TO_TICKS(howling_check_cfg->howling_recover_time)));
    howling_timer = howling_check_cfg->howling_interval_time;
    howling_set_gain_level = howling_check_cfg->howling_set_gain_level;
#endif

    return 0;
}

static void anc_iir0_set_gain_timer_handler(void *param)
{
    LOG_I("%s ", __func__);
    iir0_ch0_gaina_cfg_gain(true);
    iir0_ch1_gaina_cfg_gain(true);
#if defined(AUDIO_ANC_FB_MC_HW)
    iir0_ch1_gainb_cfg_gain(true);
#endif
}

#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK)
static void anc_ff_fb_check_init(void)
{
    hal_codec_anc_fb_check_set_irq_handler(anc_howling_check_irq_handler);
    fb_check_open_flag = 1;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    fb_check_ch0_config->codec_fb_check_enable_ch0 = 0;
    codec_int_config->fb_check_error_trig_ch0 = 1;
    if (analog_debug_get_anc_calib_mode())
    {
        codec_mask_config->fb_check_error_trig_ch0_mask = 0;
    }
    else
    {
        codec_mask_config->fb_check_error_trig_ch0_mask = 1;
    }
    feedback_mc_settings->codec_feedback_mc_en_ch0 = 1;

    fb_check_ch0_config->codec_fb_check_acc_sample_rate_ch0 = 3;
#if defined(ANC_FF_CHECK)
    fb_check_ch0_config->codec_fb_check_src_sel_ch0 = 2;
#elif defined(ANC_FB_CHECK)
    fb_check_ch0_config->codec_fb_check_src_sel_ch0 = 0;
#endif
    fb_check_ch0_config->codec_fb_check_acc_window_ch0 = 64;
    fb_check_ch0_config->codec_fb_check_trig_window_ch0 = 64;

    *codec_fb_check_threshold_ch0 = ANC_HOWLING_THRESHOLD_0dB;

    fb_check_ch0_config->codec_fb_check_enable_ch0 = 1;

    ff_howling_gain_l = 512;
    fb_howling_gain_l = 512;

    howling_cnt_l = howling_set_gain_level;
    howling_time_l = 0;
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
        fb_check_ch1_config->codec_fb_check_enable_ch1=0;
        // *codec_int_config_clr=CODEC_FB_CHECK_ERROR_TRIG_CH1;
        codec_int_config->fb_check_error_trig_ch1 = 1;
        if (analog_debug_get_anc_calib_mode()) {
            codec_mask_config->fb_check_error_trig_ch1_mask=0;
        } else {
            codec_mask_config->fb_check_error_trig_ch1_mask=1;
        }
        feedback_mc_settings->codec_feedback_mc_en_ch1=1;
        fb_check_ch1_config->codec_fb_check_acc_sample_rate_ch1=3;
#if defined(ANC_FF_CHECK)
        fb_check_ch1_config->codec_fb_check_src_sel_ch1=2;
#endif
        fb_check_ch1_config->codec_fb_check_acc_window_ch1=64;
        fb_check_ch1_config->codec_fb_check_trig_window_ch1=64;

        *codec_fb_check_threshold_ch1=ANC_HOWLING_THRESHOLD_0dB;

        fb_check_ch1_config->codec_fb_check_enable_ch1=1;

        ff_howling_gain_r=512;

        howling_cnt_r = howling_set_gain_level;
        howling_time_r = 0;
#endif
    return;
}
#endif

int anc_open(enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err = ANC_NO_ERR;

    LOG_I( "%s", __func__);

    if (anc_type == ANC_FEEDFORWARD && ff_open_flag == 1)
        return err;
    if (anc_type == ANC_FEEDBACK && fb_open_flag == 1)
        return err;
#if defined(AUDIO_ANC_FB_MC_HW)
    if(anc_type == ANC_MUSICCANCLE && mc_open_flag == 1)
        return err;
#endif

    anc_cfg_lock();

    if (anc_iir0_set_gain_timer == NULL)
    {
        anc_iir0_set_gain_timer = hwtimer_alloc(anc_iir0_set_gain_timer_handler, NULL);
        ASSERT(anc_iir0_set_gain_timer, "Failed to alloc anc_iir0_set_gain_timer");
    }

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK)
    if (anc_howling_check_dev_timer_l == NULL)
    {
        anc_howling_check_dev_timer_l = hwtimer_alloc(anc_howling_check_timer_handler, NULL);
        ASSERT(anc_howling_check_dev_timer_l, "Failed to alloc usbdev_timer");
    }
#endif
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
#if defined(ANC_FF_CHECK)
    if (anc_howling_check_dev_timer_r == NULL)
    {
        anc_howling_check_dev_timer_r = hwtimer_alloc(anc_right_howling_check_timer_handler, NULL);
        ASSERT(anc_howling_check_dev_timer_r, "Failed to alloc usbdev_timer");
    }
#endif
#endif

    if (ff_open_flag == 0 && fb_open_flag == 0
#if defined(AUDIO_ANC_FB_MC_HW)
        &&mc_open_flag==0
#endif
    )
    {
        if ((IIR_FF_COUNTER + IIR_FB_COUNTER) > 13)
        {
            hal_codec_iir_enable(60000000);
        }
        else if ((IIR_FF_COUNTER + IIR_FB_COUNTER) > 11)
        {
            if (hal_cmu_get_audio_resample_status())
            {
                hal_codec_iir_enable(48000000);
            }
            else
            {
                hal_codec_iir_enable(24576000 * 2);
            }
        }
        else
        {
#if defined(ANC_LIMITER_IIR0_CH0) || defined(ANC_LIMITER_IIR0_CH1)
            if (hal_cmu_get_audio_resample_status())
            {
                hal_codec_iir_enable(48000000);
            }
            else
            {
                hal_codec_iir_enable(24576000 * 2);
            }
#else
            hal_codec_iir_enable(39000000);
#endif
        }

        anc_ctrl_reg_init();

        iir0_coef_using = 0;
    }

#if defined(ANC_FF_CHECK)
    if (anc_type == ANC_FEEDFORWARD)
    {
        anc_ff_fb_check_init();
    }
#elif defined(ANC_FB_CHECK)
    if (anc_type == ANC_FEEDBACK)
    {
        anc_ff_fb_check_init();
    }
#endif

    if (anc_type == ANC_FEEDFORWARD)
    {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
        {
            max_ff_gain_l = (1 << GAIN_Q);
        }
#endif
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1)
        {
            max_ff_gain_r = (1 << GAIN_Q);
        }
#endif
        ff_open_flag = 1;
    }

    if (anc_type == ANC_FEEDBACK)
    {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
        {
            max_fb_gain_l = (1 << GAIN_Q);
        }
#endif
        fb_open_flag = 1;
    }
#if defined(AUDIO_ANC_FB_MC_HW)
    if(anc_type == ANC_MUSICCANCLE)
    {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if(anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
        {
            max_mc_gain_l = (1<<GAIN_Q);
        }
#endif
        mc_open_flag = 1;
    }
#endif

    anc_ctrl_reg_open(anc_type);

    anc_cfg_unlock();

    return err;
}

void anc_close(enum ANC_TYPE_T anc_type)
{
    if (anc_type == ANC_FEEDFORWARD)
    {
        LOG_I( "%s: ANC_FEEDFORWARD", __func__);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
        {
            max_ff_gain_l = 0;
#ifdef ANC_GAIN_RAMP
            ff_ramp_gain_l = 0;
#endif
#if defined(ANC_FF_CHECK)
            fb_check_ch0_config->codec_fb_check_enable_ch0 = 0;
            codec_int_config->fb_check_error_trig_ch0 = 1;
            codec_mask_config->fb_check_error_trig_ch0_mask = 0;
            ff_howling_gain_l = 0;
#endif
        }
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1) {
            max_ff_gain_r=0;
#ifdef ANC_GAIN_RAMP
            ff_ramp_gain_r=0;
#endif
#if defined(ANC_FF_CHECK)
            fb_check_ch1_config->codec_fb_check_enable_ch1=0;
            codec_int_config->fb_check_error_trig_ch1 = 1;
            codec_mask_config->fb_check_error_trig_ch1_mask=0;
            ff_howling_gain_r=0;
#endif
        }
#endif

#if defined(ANC_FF_CHECK)
        fb_check_open_flag = 0;
#endif
        ff_open_flag = 0;
    }

    if (anc_type == ANC_FEEDBACK)
    {
        LOG_I( "%s: ANC_FEEDBACK", __func__);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
        {
            max_fb_gain_l = 0;
#ifdef ANC_GAIN_RAMP
            fb_ramp_gain_l = 0;
#endif
#if defined(ANC_FB_CHECK)
            fb_check_ch0_config->codec_fb_check_enable_ch0 = 0;
            codec_int_config->fb_check_error_trig_ch0 = 1;
            codec_mask_config->fb_check_error_trig_ch0_mask = 0;
            fb_howling_gain_l = 0;
#endif
        }
#endif

#if defined(ANC_FB_CHECK)
        fb_check_open_flag = 0;
#endif
        fb_open_flag = 0;
    }
#if defined(AUDIO_ANC_FB_MC_HW)
    if(anc_type==ANC_MUSICCANCLE)
    {
        LOG_I("%s: ANC_MUSICCANCLE",__func__);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if(anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
        {
            max_mc_gain_l = 0;
#ifdef ANC_GAIN_RAMP
            mc_ramp_gain_l = 0;
#endif
        }
#endif
        mc_open_flag=0;
    }
#endif


    anc_ctrl_reg_close(anc_type);

    if (ff_open_flag == 0 && fb_open_flag == 0
#if defined(AUDIO_ANC_FB_MC_HW)
        &&mc_open_flag==0
#endif
    )
    {
        hwtimer_stop(anc_iir0_set_gain_timer);
        hal_codec_iir_disable();
        anc_iir_clock->en_clk_iir_anc = 0x0;
    }

    return;
}

int anc_enable(void)
{
    LOG_I( "%s", __func__);

    anc_cfg_lock();
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
    {
        anc_control->codec_anc_enable_ch0 = 1;
#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK)
        fb_check_ch0_config->codec_fb_check_enable_ch0 = 1;
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
        tm_config->codec_mm_enable_ch0_eco = 1;
#endif
    }
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1)
    {
        anc_control->codec_anc_enable_ch1 = 1;
#if defined(ANC_FF_CHECK)
        fb_check_ch1_config->codec_fb_check_enable_ch1 = 1;
#endif
    }
#endif
    anc_cfg_unlock();

    return ANC_NO_ERR;
}

int anc_disable(void)
{
    LOG_I( "%s", __func__);

    anc_cfg_lock();
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
    {
#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK)
        fb_check_ch0_config->codec_fb_check_enable_ch0 = 0;
        hwtimer_stop(anc_howling_check_dev_timer_l);
#endif
#ifndef ANC_GAIN_RAMP
        anc_control->codec_anc_enable_ch0 = 0;
#if defined(AUDIO_ANC_FB_MC_HW)
        tm_config->codec_mm_enable_ch0_eco = 0;
#endif
#endif
    }
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1)
    {
#if defined(ANC_FF_CHECK)
        fb_check_ch1_config->codec_fb_check_enable_ch1 = 0;
        hwtimer_stop(anc_howling_check_dev_timer_r);
#endif
#ifndef ANC_GAIN_RAMP
        anc_control->codec_anc_enable_ch1 = 0;
#endif
    }
#endif
    anc_cfg_unlock();

    return ANC_NO_ERR;
}

int anc_set_ff_and_fb_gain(int32_t gain_ff_l, int32_t gain_ff_r, int32_t gain_fb_l, int32_t gain_fb_r)
{
    ANC_ERROR err = ANC_NO_ERR;

    // if(gain_ch_l==511||gain_ch_l==0||gain_ch_l==250)
    {
        LOG_I( "gain_ff_l:%d,gain_fb_l:%d", gain_ff_l, gain_fb_l);
        LOG_I( "gain_ff_r:%d,gain_fb_r:%d", gain_ff_r, gain_fb_r);
    }

    anc_cfg_lock();
    /* ff_l use iir0_ch0*/
    ff_ramp_gain_l = gain_ff_l;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1) {
        /* stereo ff_r use iir0_ch1*/
        ff_ramp_gain_r = gain_ff_r;
    } else
#endif
    {
        /* tws fb_l use iir0_ch1*/
        fb_ramp_gain_l = gain_fb_l;
    }

    iir0_ch0_gaina_cfg_gain(true);
    iir0_ch1_gaina_cfg_gain(true);
    anc_cfg_unlock();

    return err;
}

static float g_ff_adaptive_gain = 1;
int anc_set_gain2_float(float gain)
{
    if (gain > 1)
    {
        LOG_I( "[%s] warning input value = %d/100 > 1", __func__, (int)(gain * 100));
        g_ff_adaptive_gain = 1;
    }
    else if (gain < 0)
    {
        LOG_I( "[%s] warning input value = %d/100 < 1", __func__, (int)(gain * 100));
        g_ff_adaptive_gain = 0;
    }
    else
    {
        g_ff_adaptive_gain = gain;
    }
    return 0;
}

float anc_get_gain2_float(void)
{
    return g_ff_adaptive_gain;
}

int anc_set_gain(int32_t gain_ch_l, int32_t gain_ch_r, enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err = ANC_NO_ERR;

    if (gain_ch_l == 511 || gain_ch_l == 0 || gain_ch_l == 250)
    {
        LOG_I( "anc_set_gain anc_type:%d, gain_ch_l:%d,gain_ch_r:%d", anc_type, gain_ch_l, gain_ch_r);
    }

    if ((anc_type == ANC_FEEDFORWARD && ff_open_flag == 0) ||
        (anc_type == ANC_FEEDBACK && fb_open_flag == 0))
    {
        LOG_I( "%s: anc_type=%d not opened", __func__, anc_type);
        return ANC_OTHER_ERR;
    }

    anc_cfg_lock();

    if (anc_type & ANC_FEEDFORWARD)
    {
        gain_ch_l = (uint32_t)(gain_ch_l * g_ff_adaptive_gain);
        gain_ch_r = (uint32_t)(gain_ch_r * g_ff_adaptive_gain);
        // LOG_I("!!!!!!! type =% d internal gain = %d coef = %d",anc_type,gain_ch_l,(int)(100*g_ff_adaptive_gain));
    }
#ifdef ANC_GAIN_RAMP

    if (anc_type & ANC_FEEDFORWARD)
    {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        ff_ramp_gain_l = gain_ch_l;
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
        {
            // gain_ch_l=512;
            // gain_ch_r=512;
            iir0_ch0_gaina_cfg_gain(true);
        }
#endif
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
        ff_ramp_gain_r = gain_ch_r;
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1)
        {
            // gain_ch_l=512;
            // gain_ch_r=512;

            iir0_ch1_gaina_cfg_gain(true);
        }
#endif
    }
    if (anc_type & ANC_FEEDBACK)
    {
        // Set the  FB gain;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        fb_ramp_gain_l = gain_ch_l;
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
        {
            // gain_ch_l=512;
            // gain_ch_r=512;
            iir0_ch1_gaina_cfg_gain(true);
        }
#endif
    }
#if defined(AUDIO_ANC_FB_MC_HW)
    if(anc_type & ANC_MUSICCANCLE)
    {
        //Set the MC gain;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        mc_ramp_gain_l = gain_ch_l;
        if(anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
        {
            iir0_ch1_gainb_cfg_gain(true);
        }
#endif
    }
#endif

#else /*ANC_GAIN_RAMP*/
    if (anc_type & ANC_FEEDFORWARD)
    {
        // Set the FF gain;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        anc_ff_gain->codec_anc_mute_gain_update_ff_ch0 = 0;

        anc_ff_gain->codec_anc_mute_gain_ff_ch0 = gain_ch_l;

        anc_ff_gain->codec_anc_mute_gain_update_ff_ch0 = 1;
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
        anc_ff_gain->codec_anc_mute_gain_update_ff_ch1 = 0;

        anc_ff_gain->codec_anc_mute_gain_ff_ch1 = gain_ch_r;

        anc_ff_gain->codec_anc_mute_gain_update_ff_ch1 = 1;
#endif
    }

    if (anc_type & ANC_FEEDBACK)
    {
        // Set the FB gain;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        anc_fb_gain->codec_anc_mute_gain_update_fb_ch0 = 0;

        anc_fb_gain->codec_anc_mute_gain_fb_ch0 = gain_ch_l;

        anc_fb_gain->codec_anc_mute_gain_update_fb_ch0 = 1;
#endif
    }

#if defined(AUDIO_ANC_FB_MC_HW)
    if(anc_type&ANC_MUSICCANCLE)
    {
        //Set the MC gain;
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

int anc_get_gain(int32_t *gain_ch_l, int32_t *gain_ch_r, enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err = ANC_NO_ERR;

    anc_cfg_lock();
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
#ifdef ANC_GAIN_RAMP
    if (anc_type & ANC_FEEDFORWARD)
    {
        // Get the FF gain;
        *gain_ch_l = ff_ramp_gain_l;
        *gain_ch_r = ff_ramp_gain_l;
    }

    if (anc_type & ANC_FEEDBACK)
    {
        // Get the FB gain;
        *gain_ch_l = fb_ramp_gain_l;
        *gain_ch_r = fb_ramp_gain_l;
    }
#if defined(AUDIO_ANC_FB_MC_HW)
    if(anc_type&ANC_MUSICCANCLE)
    {
        // Get the MC gain;
         *gain_ch_l=mc_ramp_gain_l;
         *gain_ch_r=mc_ramp_gain_l;
     }
#endif
#else

    if (anc_type & ANC_FEEDFORWARD)
    {
        // Get the FF gain;
        *gain_ch_l = anc_ff_gain->codec_anc_mute_gain_ff_ch0;
        *gain_ch_r = anc_ff_gain->codec_anc_mute_gain_ff_ch0;
    }
    if (anc_type & ANC_FEEDBACK)
    {
        // Get the FB gain;
        *gain_ch_l = anc_fb_gain->codec_anc_mute_gain_fb_ch0;
        *gain_ch_r = anc_fb_gain->codec_anc_mute_gain_fb_ch0;
    }
#if defined(AUDIO_ANC_FB_MC_HW)
     if(anc_type&ANC_MUSICCANCLE)
    {
        // Get the MC gain;
     }
#endif
#endif
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
#ifdef ANC_GAIN_RAMP
    if (anc_type & ANC_FEEDFORWARD)
    {
        // Get the FF gain;
        *gain_ch_r = ff_ramp_gain_r;
    }
#else

    if (anc_type & ANC_FEEDFORWARD)
    {
        // Get the FF gain;
        *gain_ch_r = anc_ff_gain->codec_anc_mute_gain_ff_ch1;
    }
#endif
#endif
    anc_cfg_unlock();

    //  LOG_I("anc_get_gain gain_ch_l:%d,gain_ch_r:%d",*gain_ch_l,*gain_ch_r);
    return err;
}

int anc_get_cfg_gain(int32_t *gain_ch_l, int32_t *gain_ch_r, enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err = ANC_NO_ERR;

    anc_cfg_lock();
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_type & ANC_FEEDFORWARD)
    {
        // Get the max FF gain;
        *gain_ch_l = max_ff_gain_l;
        *gain_ch_r = max_ff_gain_l;
    }

    if (anc_type & ANC_FEEDBACK)
    {
        // Get the max FB gain;
        *gain_ch_l = max_fb_gain_l;
        *gain_ch_r = max_fb_gain_l;
    }

#if defined(AUDIO_ANC_FB_MC_HW)
    if(anc_type&ANC_MUSICCANCLE)
    {
        //Get the MC gain;
        *gain_ch_l=max_mc_gain_l;
        *gain_ch_r=max_mc_gain_l;
    }
#endif
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
    if (anc_type & ANC_FEEDFORWARD)
    {
        // Get the max FF gain;
        *gain_ch_r = max_ff_gain_r;
    }
#endif
    anc_cfg_unlock();

    // LOG_I("anc_get_cfg_gain gain_ch_l:%d,gain_ch_r:%d",*gain_ch_l,*gain_ch_r);
    return err;
}

void anc_set_ch_map(int32_t ch_map)
{
    LOG_I( "%s ch_map:%d", __func__, ch_map);
    anc_output_ch_map = ch_map;
    return;
}

void anc_howling_check_enable(int32_t flag)
{
    uint32_t lock;

    LOG_I( "%s flag:%d", __func__, flag);

    lock = int_lock();
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
    {
#if defined(ANC_FF_CHECK) || defined(ANC_FB_CHECK)
        if (fb_check_open_flag == 1)
        {
            if (flag == 0)
            {
                fb_check_ch0_config->codec_fb_check_enable_ch0 = 0;
                codec_mask_config->fb_check_error_trig_ch0_mask = 0;
                codec_int_config->fb_check_error_trig_ch0 = 1;
            }
            else
            {
                codec_int_config->fb_check_error_trig_ch0 = 1;
                codec_mask_config->fb_check_error_trig_ch0_mask = 1;
                fb_check_ch0_config->codec_fb_check_enable_ch0 = 1;
            }
        }
#endif
    }
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1)
    {
#if defined(ANC_FF_CHECK)
        if (fb_check_open_flag == 1)
        {
            if (flag == 0)
            {
                fb_check_ch1_config->codec_fb_check_enable_ch1 = 0;
                codec_mask_config->fb_check_error_trig_ch1_mask = 0;
                codec_int_config->fb_check_error_trig_ch1 = 1;
            }
            else
            {
                codec_int_config->fb_check_error_trig_ch1 = 1;
                codec_mask_config->fb_check_error_trig_ch1_mask = 1;
                fb_check_ch1_config->codec_fb_check_enable_ch1 = 1;
            }
        }
#endif
    }
#endif
    int_unlock(lock);

    return;
}

void anc_howling_gain_reset(void)
{
    LOG_I( "%s", __func__);
#if defined(ANC_FF_CHECK)
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
        ff_howling_gain_l = 512;
        iir0_ch0_gaina_cfg_gain(true);
    }
#endif
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1) {
        ff_howling_gain_r = 512;
        iir0_ch1_gaina_cfg_gain(true);
    }
#endif
#elif defined(ANC_FB_CHECK)
        fb_howling_gain_l = 512;
        iir0_ch1_gaina_cfg_gain(true);
#if defined(AUDIO_ANC_FB_MC_HW)
        iir0_ch1_gainb_cfg_gain(true);
#endif
#endif
}

int anc_set_switching_delay(ANC_SWITCHING_DELAY anc_switching_delay, enum ANC_TYPE_T anc_type)
{
    anc_iir_coefs iir_coef_gain_ramp;
    int anc_switching_coef_delay;
    int anc_switching_gain_delay;

    LOG_I( "%s:anc_switching_delay:%d,anc_type:%d", __func__, anc_switching_delay, anc_type);

    uint32_t lock;
    lock = int_lock();

    switch (anc_switching_delay)
    {
    case ANC_SWITCHING_DELAY_50ms:
        iir_coef_gain_ramp = iir_coef_gain_ramp_delay1;
        anc_switching_gain_delay = ANC_SET_GAIN_DELAY_LONG / 8;
        anc_switching_coef_delay = ANC_SET_GAIN_TIME_DELAY1 + ANC_SET_GAIN_DELAY_LONG / 8;
        break;

    case ANC_SWITCHING_DELAY_100ms:
        iir_coef_gain_ramp = iir_coef_gain_ramp_delay2;
        anc_switching_gain_delay = ANC_SET_GAIN_DELAY_LONG / 4;
        anc_switching_coef_delay = ANC_SET_GAIN_TIME_DELAY2 + ANC_SET_GAIN_DELAY_LONG / 4;
        break;

    case ANC_SWITCHING_DELAY_200ms:
        iir_coef_gain_ramp = iir_coef_gain_ramp_delay3;
        anc_switching_gain_delay = ANC_SET_GAIN_DELAY_LONG / 4;
        anc_switching_coef_delay = ANC_SET_GAIN_TIME_DELAY3 + ANC_SET_GAIN_DELAY_LONG / 4;
        break;

    case ANC_SWITCHING_DELAY_450ms:
        iir_coef_gain_ramp = iir_coef_gain_ramp_delay4;
        anc_switching_gain_delay = ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay = ANC_SET_GAIN_TIME_DELAY4 + ANC_SET_GAIN_DELAY_LONG;
        break;

    case ANC_SWITCHING_DELAY_650ms:
        iir_coef_gain_ramp = iir_coef_gain_ramp_delay5;
        anc_switching_gain_delay = ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay = ANC_SET_GAIN_TIME_DELAY5 + ANC_SET_GAIN_DELAY_LONG;
        break;

    case ANC_SWITCHING_DELAY_850ms:
        iir_coef_gain_ramp = iir_coef_gain_ramp_delay6;
        anc_switching_gain_delay = ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay = ANC_SET_GAIN_TIME_DELAY6 + ANC_SET_GAIN_DELAY_LONG;
        break;

    case ANC_SWITCHING_DELAY_1300ms:
        iir_coef_gain_ramp = iir_coef_gain_ramp_delay7;
        anc_switching_gain_delay = ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay = ANC_SET_GAIN_TIME_DELAY7 + ANC_SET_GAIN_DELAY_LONG;
        break;

    case ANC_SWITCHING_DELAY_1600ms:
        iir_coef_gain_ramp = iir_coef_gain_ramp_delay8;
        anc_switching_gain_delay = ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay = ANC_SET_GAIN_TIME_DELAY8 + ANC_SET_GAIN_DELAY_LONG;
        break;

    case ANC_SWITCHING_DELAY_1900ms:
        iir_coef_gain_ramp = iir_coef_gain_ramp_delay9;
        anc_switching_gain_delay = ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay = ANC_SET_GAIN_TIME_DELAY9 + ANC_SET_GAIN_DELAY_LONG;
        break;

    case ANC_SWITCHING_DELAY_2200ms:
        iir_coef_gain_ramp = iir_coef_gain_ramp_delay10;
        anc_switching_gain_delay = ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay = ANC_SET_GAIN_TIME_DELAY10 + ANC_SET_GAIN_DELAY_LONG;
        break;

    case ANC_SWITCHING_DELAY_2600ms:
        iir_coef_gain_ramp = iir_coef_gain_ramp_delay11;
        anc_switching_gain_delay = ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay = ANC_SET_GAIN_TIME_DELAY11 + ANC_SET_GAIN_DELAY_LONG;
        break;

    default:
        iir_coef_gain_ramp = iir_coef_gain_ramp_delay4;
        anc_switching_gain_delay = ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay = ANC_SET_GAIN_TIME_DELAY4 + ANC_SET_GAIN_DELAY_LONG;
        break;
    }

    if (anc_type & ANC_FEEDFORWARD || anc_type & ANC_FEEDBACK || anc_type & ANC_MUSICCANCLE)
    {
        anc_iir0_control->codec_iir0_enable = 0;

        anc_gain_ff_iir_coefs0_l[0].a1 = -iir_coef_gain_ramp.coef_a[1];
        anc_gain_ff_iir_coefs0_l[0].a2 = -iir_coef_gain_ramp.coef_a[2];
        anc_gain_ff_iir_coefs0_l[0].b0 = iir_coef_gain_ramp.coef_b[0];
        anc_gain_ff_iir_coefs0_l[0].b1 = iir_coef_gain_ramp.coef_b[1];
        anc_gain_ff_iir_coefs0_l[0].b2 = iir_coef_gain_ramp.coef_b[2];

        anc_gain_ff_iir_coefs1_l[0].a1 = -iir_coef_gain_ramp.coef_a[1];
        anc_gain_ff_iir_coefs1_l[0].a2 = -iir_coef_gain_ramp.coef_a[2];
        anc_gain_ff_iir_coefs1_l[0].b0 = iir_coef_gain_ramp.coef_b[0];
        anc_gain_ff_iir_coefs1_l[0].b1 = iir_coef_gain_ramp.coef_b[1];
        anc_gain_ff_iir_coefs1_l[0].b2 = iir_coef_gain_ramp.coef_b[2];
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1)
        {
            anc_gain_ff_iir_coefs0_r[0].a1 = -iir_coef_gain_ramp.coef_a[1];
            anc_gain_ff_iir_coefs0_r[0].a2 = -iir_coef_gain_ramp.coef_a[2];
            anc_gain_ff_iir_coefs0_r[0].b0 = iir_coef_gain_ramp.coef_b[0];
            anc_gain_ff_iir_coefs0_r[0].b1 = iir_coef_gain_ramp.coef_b[1];
            anc_gain_ff_iir_coefs0_r[0].b2 = iir_coef_gain_ramp.coef_b[2];

            anc_gain_ff_iir_coefs1_r[0].a1 = -iir_coef_gain_ramp.coef_a[1];
            anc_gain_ff_iir_coefs1_r[0].a2 = -iir_coef_gain_ramp.coef_a[2];
            anc_gain_ff_iir_coefs1_r[0].b0 = iir_coef_gain_ramp.coef_b[0];
            anc_gain_ff_iir_coefs1_r[0].b1 = iir_coef_gain_ramp.coef_b[1];
            anc_gain_ff_iir_coefs1_r[0].b2 = iir_coef_gain_ramp.coef_b[2];
        }
        else
#endif
        {
            anc_gain_fb_iir_coefs0_l[0].a1 = -iir_coef_gain_ramp.coef_a[1];
            anc_gain_fb_iir_coefs0_l[0].a2 = -iir_coef_gain_ramp.coef_a[2];
            anc_gain_fb_iir_coefs0_l[0].b0 = iir_coef_gain_ramp.coef_b[0];
            anc_gain_fb_iir_coefs0_l[0].b1 = iir_coef_gain_ramp.coef_b[1];
            anc_gain_fb_iir_coefs0_l[0].b2 = iir_coef_gain_ramp.coef_b[2];

            anc_gain_fb_iir_coefs1_l[0].a1 = -iir_coef_gain_ramp.coef_a[1];
            anc_gain_fb_iir_coefs1_l[0].a2 = -iir_coef_gain_ramp.coef_a[2];
            anc_gain_fb_iir_coefs1_l[0].b0 = iir_coef_gain_ramp.coef_b[0];
            anc_gain_fb_iir_coefs1_l[0].b1 = iir_coef_gain_ramp.coef_b[1];
            anc_gain_fb_iir_coefs1_l[0].b2 = iir_coef_gain_ramp.coef_b[2];
        }
        anc_iir0_control->codec_iir0_enable = 1;

        anc_switching_gain_delay_ff_fb = anc_switching_gain_delay;
        anc_switching_coef_delay_ff_fb = anc_switching_coef_delay;
    }
    int_unlock(lock);

    return 0;
}

int anc_howling_set(ANC_HOWLING_WINDOW window, ANC_HOWLING_THRESHOLD threshold)
{
#if defined(ANC_FB_CHECK) || defined(ANC_FF_CHECK)
    float threshold_multiple = 0;
    uint32 acc_window = 0;
    uint32 trig_window = 0;
    uint32 check_threshold = 0;

    uint32 lock;
    lock = int_lock();
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
    {
        if (fb_check_open_flag)
        {
            fb_check_ch0_config->codec_fb_check_enable_ch0 = 0;

            switch (window)
            {
            case ANC_HOWLING_WINDOW_32:
                threshold_multiple = 0.25f;
                acc_window = window;
                trig_window = window;
                break;

            case ANC_HOWLING_WINDOW_64:
                threshold_multiple = 1.0f;
                acc_window = window;
                trig_window = window;

                break;

            case ANC_HOWLING_WINDOW_128:
                threshold_multiple = 4.0f;
                acc_window = window;
                trig_window = window;
                break;

            case ANC_HOWLING_WINDOW_256:
                threshold_multiple = 16.0f;
                acc_window = window;
                trig_window = window;
                break;

            case ANC_HOWLING_WINDOW_512:
                threshold_multiple = 64.0f;
                acc_window = window;
                trig_window = window;

                break;

            default:
                acc_window = ANC_HOWLING_WINDOW_64;
                trig_window = ANC_HOWLING_WINDOW_64;
                threshold_multiple = 1.0f;
                break;
            }

            switch (threshold)
            {
                case ANC_HOWLING_THRESHOLD_m12dB:
                case ANC_HOWLING_THRESHOLD_m10dB:
                case ANC_HOWLING_THRESHOLD_m8dB:
                case ANC_HOWLING_THRESHOLD_m6dB:
                case ANC_HOWLING_THRESHOLD_m4dB:
                case ANC_HOWLING_THRESHOLD_m2dB:
                case ANC_HOWLING_THRESHOLD_0dB:
                case ANC_HOWLING_THRESHOLD_2dB:
                case ANC_HOWLING_THRESHOLD_4dB:
                case ANC_HOWLING_THRESHOLD_6dB:
                case ANC_HOWLING_THRESHOLD_8dB:
                case ANC_HOWLING_THRESHOLD_10dB:
                case ANC_HOWLING_THRESHOLD_12dB:
                    check_threshold = (uint32)(threshold * threshold_multiple);
                    break;

                default:
                    check_threshold = (uint32)(ANC_HOWLING_THRESHOLD_0dB * threshold_multiple);
                    break;
            }
            LOG_I( "%s:acc_window:%d,trig_window:%d", __func__, acc_window, trig_window);
            LOG_I( "%s:check_threshold:0x%x", __func__, check_threshold);

            fb_check_ch0_config->codec_fb_check_acc_window_ch0 = acc_window;
            fb_check_ch0_config->codec_fb_check_trig_window_ch0 = trig_window;

            *codec_fb_check_threshold_ch0 = check_threshold;

            fb_check_ch0_config->codec_fb_check_enable_ch0 = 1;
        }
    }
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH1)
    {
        if (fb_check_open_flag)
        {
            fb_check_ch1_config->codec_fb_check_enable_ch1 = 0;

            switch (window)
            {
            case ANC_HOWLING_WINDOW_32:
                threshold_multiple = 0.25f;
                acc_window = window;
                trig_window = window;
                break;

            case ANC_HOWLING_WINDOW_64:
                threshold_multiple = 1.0f;
                acc_window = window;
                trig_window = window;

                break;

            case ANC_HOWLING_WINDOW_128:
                threshold_multiple = 4.0f;
                acc_window = window;
                trig_window = window;
                break;

            case ANC_HOWLING_WINDOW_256:
                threshold_multiple = 16.0f;
                acc_window = window;
                trig_window = window;
                break;

            case ANC_HOWLING_WINDOW_512:
                threshold_multiple = 64.0f;
                acc_window = window;
                trig_window = window;

                break;

            default:
                acc_window = ANC_HOWLING_WINDOW_64;
                trig_window = ANC_HOWLING_WINDOW_64;
                threshold_multiple = 1.0f;
                break;
            }

            switch (threshold)
            {
            case ANC_HOWLING_THRESHOLD_m6dB:
            case ANC_HOWLING_THRESHOLD_m4dB:
            case ANC_HOWLING_THRESHOLD_m2dB:
            case ANC_HOWLING_THRESHOLD_0dB:
            case ANC_HOWLING_THRESHOLD_2dB:
            case ANC_HOWLING_THRESHOLD_4dB:
            case ANC_HOWLING_THRESHOLD_6dB:
                check_threshold = (uint32)(threshold * threshold_multiple);
                break;

            default:
                check_threshold = (uint32)(ANC_HOWLING_THRESHOLD_0dB * threshold_multiple);
                break;
            }
            LOG_I( "%s:acc_window:%d,trig_window:%d", __func__, acc_window, trig_window);
            LOG_I( "%s:check_threshold:0x%x", __func__, check_threshold);

            fb_check_ch1_config->codec_fb_check_acc_window_ch1 = acc_window;
            fb_check_ch1_config->codec_fb_check_trig_window_ch1 = trig_window;

            *codec_fb_check_threshold_ch1 = check_threshold;

            fb_check_ch1_config->codec_fb_check_enable_ch1 = 1;
        }
    }
#endif
    int_unlock(lock);
#endif
    return 0;
}

int anc_adc_data_select(ANC_ADC_DATA data_select)
{
#if defined(AUDIO_ANC_FB_MC) || defined(AUDIO_ANC_FB_MC_HW)
    LOG_I("%s:data_select:%d", __func__,data_select);

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0) {
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
    }
#endif
#endif
    return 0;
}

int anc_limiter_enable(enum ANC_TYPE_T anc_type)
{
    LOG_I( "%s anc_type:%d", __func__, anc_type);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
#if defined(ANC_LIMITER_IIR0_CH0)
    if (anc_type & ANC_FEEDFORWARD)
    {
        // Enable the FF limiter;
        anc_iir0_control->codec_iir0_lmt_ch0_bypass = 0;
    }
#endif
#if defined(ANC_LIMITER_IIR0_CH1)
    if (anc_type & ANC_FEEDBACK)
    {
        // Enable the FB limiter;
        anc_iir0_control->codec_iir0_lmt_ch1_bypass = 0;
    }
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
#if defined(ANC_LIMITER_MC)
    if(anc_type&ANC_MUSICCANCLE)
    {
        //Enable the MC limiter;
        anc_iir0_control->codec_iir0_lmt_ch1_bypass=0;
    }
#endif
#endif
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
#if defined(ANC_LIMITER_IIR0_CH1)
    if (anc_type & ANC_FEEDFORWARD)
    {
        // Enable the FF limiter;
        anc_iir0_control->codec_iir0_lmt_ch1_bypass = 0;
    }
#endif
#endif
    return 0;
}

int anc_limiter_disable(enum ANC_TYPE_T anc_type)
{
    LOG_I( "%s anc_type:%d", __func__, anc_type);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
#if defined(ANC_LIMITER_IIR0_CH0)
    if (anc_type & ANC_FEEDFORWARD)
    {
        // Disable the FF limiter;
        anc_iir0_control->codec_iir0_lmt_ch0_bypass = 1;
    }
#endif
#if defined(ANC_LIMITER_IIR0_CH1)
    if (anc_type & ANC_FEEDBACK)
    {
        // Disable the FB limiter;
        anc_iir0_control->codec_iir0_lmt_ch1_bypass = 1;
    }
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
#if defined(ANC_LIMITER_MC)
    if(anc_type&ANC_MUSICCANCLE)
    {
        //Disable the MC limiter;
        anc_iir0_control->codec_iir0_lmt_ch1_bypass=1;
    }
#endif
#endif
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
#if defined(ANC_LIMITER_IIR0_CH1)
    if (anc_type & ANC_FEEDFORWARD)
    {
        // Disable the FF limiter;
        anc_iir0_control->codec_iir0_lmt_ch1_bypass = 1;
    }
#endif
#endif
    return 0;
}

int anc_limiter_threhold_set(enum ANC_TYPE_T anc_type, int32_t threhold_db)
{
    int32_t threhold;
    if (threhold_db > 0)
    {
        threhold = 0x7fffff;
    }
    else
    {
        threhold = (int32_t)(db_to_float(threhold_db) * 0x7fffff);
    }
    LOG_I( "%s threhold_db:%d,threhold:0x%x", __func__, threhold_db, threhold);

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
#if defined(ANC_LIMITER_IIR0_CH0)
    if (anc_type & ANC_FEEDFORWARD)
    {
        anc_iir_gain_update->codec_iir0_lmt_th_update_ch0 = 0;
        // limiter threshold, 0dB: 0x7fffff
        *codec_iir0_lmt_th_ch0 = threhold;
        hal_sys_timer_delay_us(1);
        anc_iir_gain_update->codec_iir0_lmt_th_update_ch0 = 1;
    }
#endif
#if defined(ANC_LIMITER_IIR0_CH1)
    if (anc_type & ANC_FEEDBACK)
    {
        anc_iir_gain_update->codec_iir0_lmt_th_update_ch1 = 0;
        // limiter threshold, 0dB: 0x7fffff
        *codec_iir0_lmt_th_ch1 = threhold;
        hal_sys_timer_delay_us(1);
        anc_iir_gain_update->codec_iir0_lmt_th_update_ch1 = 1;
    }
#endif
#if defined(AUDIO_ANC_FB_MC_HW)
#if defined(ANC_LIMITER_MC)
    if(anc_type&ANC_MUSICCANCLE)
    {
        anc_iir_gain_update->codec_iir0_lmt_th_update_ch1=0;
        //limiter threshold, 0dB: 0x7fffff
        *codec_iir0_lmt_th_ch1=threhold;
        hal_sys_timer_delay_us(1);
        anc_iir_gain_update->codec_iir0_lmt_th_update_ch1=1;
    }
#endif
#endif
#endif

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH1)
#if defined(ANC_LIMITER_IIR0_CH1)
    if (anc_type & ANC_FEEDFORWARD)
    {
        anc_iir_gain_update->codec_iir0_lmt_th_update_ch1 = 0;
        // limiter threshold, 0dB: 0x7fffff
        *codec_iir0_lmt_th_ch1 = threhold;
        hal_sys_timer_delay_us(1);
        anc_iir_gain_update->codec_iir0_lmt_th_update_ch1 = 1;
    }
#endif
#endif
    return 0;
}
