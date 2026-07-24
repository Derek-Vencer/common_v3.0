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

/**********************************mocro**************************************/

typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;

/*
5	64	soft_rstn_adc_ana	RW	31	0	soft reset of clk_adc_ana domain
8	64	soft_rstn_adc	RW	255	5	soft reset of clk_adc domain
1	64	soft_rstn_dac	RW	1	13	soft reset of clk_dac domain
1	64	soft_rstn_rs_adc	RW	1	14	soft reset of clk_rsadc domain
2	64	soft_rstn_rs_dac	RW	3	15	soft reset of clk_rsdac domain
4	64	soft_rstn_iir_anc	RW	15	17	soft reset of clk_iir domain
1	64	soft_rstn_iir_eq	RW	1	21	soft reset of clk_iir1 domain
4	64	soft_rstn_fir	RW	15	22	soft reset of clk_fir domain
2	64	soft_rstn_psap	RW	3	26
28	64		x
*/
struct _anc_soft_rstn
{
    uint32 soft_rstn_adc_ana : 1;
    uint32 soft_rstn_adc : 3;
    uint32 soft_rstn_dac : 1;

    uint32 soft_rstn_rs_adc : 1;
    uint32 soft_rstn_rs_dac : 2;
    uint32 soft_rstn_iir_anc : 1;

    uint32 soft_rstn_iir_eq : 1;

    uint32 reserved : 22;
};

/*
2	6c	rtsel_rom	RW	2	0	rtsel_rom
2	6c	ptsel_rom	RW	1	2	ptsel_rom
2	6c	trb_rom	RW	1	4	rtb_rom
4	6c	en_clk_iir_anc	RW	0	6
1	6c	en_clk_iir_eq	RW	0	10
4	6c	en_clk_fir	RW	0	11
2	6c	en_clk_psap	RW	0	15
3	6c	sel_i2s_mclk	RW	0	17
1	6c	en_i2s_mclk	RW	0	20
2	6c	cfg_div_codec_eqiir	RW	0	21	divider for iir
1	6c	bypass_div_codec_eqiir	RW	0	23	bypass_div_codec_iir
1	6c	sel_oscx4_eqiir	RW	0	24	iir clock select.1:oscx4;0:pll
1	6c	sel_oscx2_eqiir	RW	1	25	iir clock select.1:osc/oscx2;0:oscx4/pll
1	6c	sel_osc_eqiir	RW	0	26	iir clock select.1:osc;0:oscx2
1	6c	sel_osc_psap	RW	0	27	iir clock select.1:osc;0:oscx2
28	6c		x

*/
struct _anc_iir_clock
{
    uint32 rtsel_rom : 2;
    uint32 ptsel_rom : 2;
    uint32 trb_rom : 2;

    uint32 en_clk_iir_anc : 1;
    uint32 en_clk_iir_eq : 1;

    uint32 sel_i2s_mclk : 3;
    uint32 en_i2s_mclk : 1;
    uint32 cfg_div_codec_eqiir : 2;
    uint32 bypass_div_codec_eqiir : 1;

    uint32 sel_oscx4_eqiir : 1;
    uint32 sel_oscx2_eqiir : 1;

    uint32 sel_osc_eqiir : 1;
    uint32 sel_osc_psap : 1;

    uint32 reserved : 13;
};

/*

1	248	codec_iir0_enable	RW	0	0	anc iir module enable
1	248	codec_iir0_iira_enable	RW	0	1
1	248	codec_iir0_iirb_enable	RW	0	2
1	248	codec_iir0_ch0_bypass	RW	0	3	1: ch0 bypass
1	248	codec_iir0_ch1_bypass	RW	0	4
1	248	codec_iir0_gaincal_ext_ch0_bypass	RW	1	5
1	248	codec_iir0_gaincal_ext_ch1_bypass	RW	1	6
1	248	codec_iir0_gainuse_ext_ch0_bypass	RW	1	7
1	248	codec_iir0_gainuse_ext_ch1_bypass	RW	1	8
1	248	codec_iir0_lmt_ch0_bypass	RW	1	9
1	248	codec_iir0_lmt_ch1_bypass	RW	1	10
5	248	codec_iir0_count_ch0	RW	0	11	ch0 iir number  0~8
5	248	codec_iir0_count_ch1	RW	0	16
1	248	codec_iir0_coef_swap	RW	0	21	0: use iir coef memory0;  1: use iir coef memory1;
1	248	codec_iir0_auto_stop	RW	0	22	read only;  coef memoryX used currently
1	248	codec_iir0_coef_swap_status_sync[1]	R	0	23	read only;  coef memoryX used currently
1	248	codec_iir0_iira_stop_status_sync[1]	R	0	24
1	248	codec_iir0_iirb_stop_status_sync[1]	R	0	25
26	248		x
*/
struct _anc_iir0_control
{
    uint32 codec_iir0_enable : 1;
    uint32 codec_iir0_iira_enable : 1;
    uint32 codec_iir0_iirb_enable : 1;
    uint32 codec_iir0_ch0_bypass : 1;

    uint32 codec_iir0_gaincal_ext_ch0_bypass : 1;
    uint32 codec_iir0_gainuse_ext_ch0_bypass : 1;
    uint32 codec_iir0_lmt_ch0_bypass : 1;

    uint32 codec_iir0_count_ch0 : 5;

    uint32 codec_iir0_coef_swap : 1;
    uint32 codec_iir0_auto_stop : 1;
    uint32 codec_iir0_coef_swap_status_sync : 1;
    uint32 codec_iir0_iira_stop_status_sync : 1;
    uint32 codec_iir0_iirb_stop_status_sync : 1;

    uint32 reserved : 15;
};

/*
1	25c	codec_iir0_gain_ext_update_ch0	RW	0	0
1	25c	codec_iir0_gain_ext_sel_ch0	RW	0	2
1	25c	codec_deq_iir_gain_ext_update_ch0	RW	0	16
1	25c	codec_deq_iir_gain_ext_sel_ch0	RW	0	18
1	25c	codec_iir0_lmt_th_update_ch0	RW	0	20

28	25c		x

*/
struct _anc_iir_gain_update
{
    uint32 codec_iir0_gain_ext_update : 1;
    uint32 codec_iir0_gain_ext_sel_ch0 : 1;
    uint32 codec_deq_iir_gain_ext_update_ch0 : 1;
    uint32 codec_deq_iir_gain_ext_sel_ch0 : 1;
    uint32 codec_iir0_lmt_th_update_ch0 : 1;

    uint32 Reserved : 16;
};

/*
7	300	codec_iir0_lmt_delay_ch0	RW	0	0
21	300		x

*/

struct _anc_iir_lmt_delay_ch0
{
    uint32 codec_iir0_lmt_delay : 7;

    uint32 Reserved : 25;
};

/*
1	d0	codec_codec_anc_enable_ch0  	RW	0	0	anc enable
1	d0	codec_codec_dual_anc_ch0    	RW	0	1
1	d0	codec_codec_anc_mute_ch0        RW	0	2
1	d0	codec_codec_anc_rate_sel        RW	0	3
1	d0	codec_codec_feedback_ch0        RW	0	4
31	d0		x

28	d0		x

*/
struct _anc_control
{
    uint32 codec_anc_enable_ch0 : 1;
    uint32 codec_dual_anc_ch0 : 1;
    uint32 codec_anc_mute_ch0 : 1;
    uint32 codec_anc_rate_sel : 1;
    uint32 codec_feedback_ch0 : 1;

    uint32 Reserved : 27;
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
16	238	codec_anc_calib_gain_coef_ff_ch0	RW	4096	0
16	238	codec_anc_calib_gain_coef_ff_ch1	RW	4096	0
*/

struct _anc_ff_iir_calib_gain
{
    uint32 codec_anc_calib_gain_coef_ff_ch0 : 16;
    uint32 Reserved : 16;
};

/*
1	33c	codec_music_mix_off_ch0	RW	0	0
1	33c	codec_pdu_off_ch0	RW	0	1
*/
struct _anc_path_set
{
    uint32 codec_music_mix_off_ch0 : 1;
    uint32 codec_pdu_off_ch0 : 1;

    uint32 Reserved : 30;
};

/*
1	244	codec_anc_calib_gain_pass0_ff_ch0	RW	0	0	Format 3.9, for FF
1	244	codec_anc_calib_gain_update_ff_ch0	RW	0	2	1: gain update no pass0
12	244		x
*/
struct _anc_iir_calib_gain_config
{
    uint32 codec_anc_calib_gain_pass0_ff_ch0 : 1;
    uint32 codec_anc_calib_gain_update_ff_ch0 : 1;

    uint32 Reserved : 30;
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
1	dc	codec_dac_L_iir_enable	RW	0	4
1	dc	codec_adc_ch0_iir_enable	RW	0	6
22	dc
*/
struct _feedback_mc_settings
{
    uint32 codec_dac_L_iir_enable : 1;
    uint32 codec_adc_ch0_iir_enable : 1;

    uint32 Reserved1 : 21;
};

#if defined(ANC_FF_CHECK)
/*
1	130	codec_fb_check_keep_ch0	RW	0
1	130	codec_fb_check_keep_sel_ch0	RW	0
30	130		x
*/
struct _ff_check_ch0_config
{
    uint32 codec_ff_check_keep_ch0 : 1;
    uint32 codec_ff_check_keep_sel_ch0 : 1;
    uint32 Reserved : 30;
};
#endif

#if defined(ANC_FF_CHECK)

/*
7	0c	codec_rx_overflow                                    	RW	0	0	interrupt status which not been masked.
7	0c	codec_rx_underflow           	RW	0	7
1	0c	codec_tx_overflow	RW	0	14
1	0c	codec_tx_underflow	RW	0	15
1	0c	codec_tx_overflow_snd	RW	0	16
1	0c	codec_tx_underflow_snd	RW	0	17
1	0c	dsd_rx_overflow	RW	0	18
1	0c	dsd_rx_underflow	RW	0	19
1	0c	dsd_tx_overflow	RW	0	20
1	0c	dsd_tx_underflow	RW	0	21
1	0c	mc_overflow	RW	0	22
1	0c	mc_underflow	RW	0	23
1	0c	event_trigger	RW	0	24
1	0c	fb_check_error_trig_ch0	RW	0	25
1	0c	fb_check_error_trig_ch1	RW	0	26
1	0c	adc_max_overflow	RW	0	27
1	0c	time_trigger	RW	0	28
29	0c		x

*/
struct _codec_int_config
{
    uint32 Reserved_notused : 8;

    uint32 fb_check_error_trig_ch0 : 1;

    uint32 Reserved : 23;
};
/*
7	10	codec_rx_overflow_mask	RW	0	0	interrupt mask; 0 will mask interrupt
7	10	codec_rx_underflow_mask	RW	0	7
1	10	codec_tx_overflow_mask	RW	0	14
1	10	codec_tx_underflow_mask	RW	0	15
1	10	codec_tx_overflow_snd_mask	RW	0	16
1	10	codec_tx_underflow_snd_mask	RW	0	17
1	10	dsd_rx_overflow_mask	RW	0	18
1	10	dsd_rx_underflow_mask	RW	0	19
1	10	dsd_tx_overflow_mask	RW	0	20
1	10	dsd_tx_underflow_mask	RW	0	21
1	10	mc_overflow_mask	RW	0	22
1	10	mc_underflow_mask	RW	0	23
1	10	event_trigger_mask	RW	0	24
1	10	fb_check_error_trig_ch0_mask	RW	0	25
1	10	fb_check_error_trig_ch1_mask	RW	0	26
1	10	adc_max_overflow_mask	RW	0	27
1	10	time_trigger_mask	RW	0	28
29	10		x
*/
struct _codec_mask_config
{
    uint32 Reserved_notused : 8;

    uint32 fb_check_error_trig_ch0_mask : 1;

    uint32 Reserved : 23;
};
#endif

#define CODEC_EN_CLK_IIR_IIR0 (1 << 0)

#define IIR_COUNTER (13)

#define IIR_FF_COUNTER (8)

#define ANC_AUD_OUTPUT_PATH_SPEAKER_DEV (AUD_CHANNEL_MAP_CH0)

typedef struct _iir_parameter
{
    int32_t total_gain;

    uint16_t iir_bypass_flag;
    uint16_t iir_counter;

    anc_iir_coefs iir_coef[IIR_COUNTER];

} iir_parameter;

#define GAIN_Q (9)
#define ANC_GAIN_RAMP
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

#define ANC_SET_GAIN_TIME ((MS_TO_TICKS(550)))

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
#define ANC_SET_GAIN_TIME ((MS_TO_TICKS(3200)))

#endif

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

#define ANC_SET_GAIN_CALIB_TIME ((MS_TO_TICKS(70)))

#endif
#endif

#define ANC_LIMITER_FF
#define ANC_LIMITER_FF_ATTACK_DELAY (1)

// limiter threshold, 0dB: 0x7fffff
#define ANC_LIMITER_FF_THREHOLD (0x3fffff)        //(0x32F52C)
#define ANC_LIMITER_RELEASE_FF_TIME (10)        // ms
#define ANC_LIMITER_ATT_FF_TIME (13.42f)        // ms

#if defined(ANC_LIMITER_FF)
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

/*******************************data struction***********************************/

#define ANC_BASE ((uint32)CODEC_BASE)

// clock
volatile static struct _anc_iir_clock *anc_iir_clock = (volatile struct _anc_iir_clock *)(ANC_BASE + 0x6c);

// calib gain
volatile static struct _anc_ff_iir_calib_gain *anc_ff_iir_calib_gain = (volatile struct _anc_ff_iir_calib_gain *)(ANC_BASE + 0x238);
volatile static struct _anc_iir_calib_gain_config *anc_iir_calib_gain_config = (volatile struct _anc_iir_calib_gain_config *)(ANC_BASE + 0x244);

volatile static struct _anc_control *anc_control = (volatile struct _anc_control *)(ANC_BASE + 0xd0);
// volatile static struct _tm_config *tm_config = (volatile struct _tm_config *)(ANC_BASE + 0x22c);

volatile static struct _anc_ff_gain *anc_ff_gain = (volatile struct _anc_ff_gain *)(ANC_BASE + 0xd4);

// volatile static struct _anc_path_set *anc_path_set = (volatile struct _anc_path_set *)(ANC_BASE + 0x33c);

// left channel
volatile static struct _anc_iir0_control *anc_iir0_control = (volatile struct _anc_iir0_control *)(ANC_BASE + 0x248);

volatile static struct _anc_iir_gain_update *anc_iir_gain_update = (volatile struct _anc_iir_gain_update *)(ANC_BASE + 0x25c);

volatile static int32 *codec_iir0_gaina_ext_ch0 = (volatile int32 *)(ANC_BASE + 0x0260);
volatile static int32 *codec_iir0_gainb_ext_ch0 = (volatile int32 *)(ANC_BASE + 0x0268);

volatile static int32 *codec_iir0_gaina_ext_out_ch0_sync = (volatile int32 *)(ANC_BASE + 0x02a0);
volatile static int32 *codec_iir0_gainb_ext_out_ch0_sync = (volatile int32 *)(ANC_BASE + 0x02a8);

#ifdef ANC_GAIN_RAMP
volatile static int32 *codec_iir0_gain_ext_th = (volatile int32 *)(ANC_BASE + 0x0308);
#endif

#if defined(ANC_LIMITER_FF)
volatile static int32 *codec_iir0_lmt_th_ch0 = (volatile int32 *)(ANC_BASE + 0x031c);
volatile static struct _anc_iir_lmt_delay_ch0 *anc_iir_lmt_delay_ch0 = (volatile struct _anc_iir_lmt_delay_ch0 *)(ANC_BASE + 0x300);
#endif

// IIR 0 .(left ff, gain and limiter iir coef)
volatile static struct _anc_iir_coefs *anc_ff_iir_coefs0_l = (volatile struct _anc_iir_coefs *)(ANC_BASE + 0x0800);
volatile static struct _anc_iir_coefs *anc_ff_iir_coefs1_l = (volatile struct _anc_iir_coefs *)(ANC_BASE + 0x0a00);
#ifdef ANC_GAIN_RAMP
volatile static struct _anc_iir_coefs *anc_gain_ff_iir_coefs0_l = (volatile struct _anc_iir_coefs *)(ANC_BASE + 0x0800 + IIR_COUNTER * 20);
volatile static struct _anc_iir_coefs *anc_gain_ff_iir_coefs1_l = (volatile struct _anc_iir_coefs *)(ANC_BASE + 0x0a00 + IIR_COUNTER * 20);
#endif

#ifdef ANC_LIMITER_FF
volatile static struct _anc_iir_coefs *anc_limiter_ff_iir_coefs0_l = (volatile struct _anc_iir_coefs *)(ANC_BASE + 0x0800 + IIR_COUNTER * 20 + 1 * 20);
volatile static struct _anc_iir_coefs *anc_limiter_ff_iir_coefs1_l = (volatile struct _anc_iir_coefs *)(ANC_BASE + 0x0a00 + IIR_COUNTER * 20 + 1 * 20);
#endif

#if defined(ANC_FF_CHECK)
// feedback registers
volatile static struct _feedback_mc_settings *feedback_mc_settings = (volatile struct _feedback_mc_settings *)(ANC_BASE + 0xdc);
volatile static struct _ff_check_ch0_config *ff_check_ch0_config = (volatile struct _ff_check_ch0_config *)(ANC_BASE + 0x0130);

volatile static struct _codec_int_config *codec_int_config = (volatile struct _codec_int_config *)(ANC_BASE + 0x0c);
volatile static struct _codec_mask_config *codec_mask_config = (volatile struct _codec_mask_config *)(ANC_BASE + 0x10);

volatile static uint32 *codec_fb_check_threshold_ch0 = (volatile uint32 *)(ANC_BASE + 0x0138);
volatile static uint32 *codec_fb_check_data_avg_keep_ch0 = (volatile uint32 *)(ANC_BASE + 0x0140);
#endif

volatile static int max_ff_gain_l = 0;

volatile static int ff_open_flag = 0;

volatile static int fb_check_open_flag = 0;
volatile static int anc_output_ch_map = AUD_CHANNEL_MAP_CH0;

#ifdef ANC_GAIN_RAMP
volatile static int ff_ramp_gain_l = 0;
#endif
volatile static int ff_ramp_coef_l = 0;

#if defined(ANC_FF_CHECK)
static HWTIMER_ID anc_howling_check_dev_timer = NULL;
// static HWTIMER_ID anc_howling_check_dev_timer_l=NULL; //TODO: use it
volatile static int ff_howling_gain_l = 0;

static uint32 howling_cnt_l = 0;
static uint32 howling_time_l = 0;
#endif

static HWTIMER_ID anc_iir0_set_gain_timer = NULL;
static HWTIMER_ID anc_iir0_switching_timer = NULL;

#define ANC_SET_GAIN_DELAY_LONG ((MS_TO_TICKS(200)))

#define ANC_SET_GAIN_DELAY_SHORT ((MS_TO_TICKS(2)))

static iir_parameter ff_filtes_l_old;
volatile static int8_t iir0_coef_using;

static int32 trace_counter = 0;

static struct_anc_cfg anc_cfg_coef_new;
static struct_anc_cfg anc_cfg_coef;

#define CALIB_GAIN_INVALID (0xFFFF)
static int32_t g_calib_ff_gain = CALIB_GAIN_INVALID;

#if defined(ANC_LIMITER_FF)
static anc_iir_coefs ff_limiter_att_l_old;
static anc_iir_coefs ff_limiter_rls_l_old;
#endif

static int32 anc_cfg_iir0_type;

static int anc_switching_coef_delay_ff_tt = 0;
static int anc_switching_gain_delay_ff_tt = 0;

volatile static bool anc_iir0_using_flag = 0;
volatile static bool anc_iir0_reserve_flag = 0;

#define TRACE_COUNTER (10000)

/*******************************FIR hardware filter*********************************/

static bool anc_fir_open_flag = 0;

static void anc_cfg_lock(void)
{
    return;
}

static void anc_cfg_unlock(void)
{
    return;
}

int32_t anc_fir_set_status(bool flag)
{
    DRIVERS_TRACE(1, "%s: open=%d ...", __func__, flag);

    anc_fir_open_flag = flag;

    return 0;
}

static int iir_filter_para_copy(iir_parameter *filtes_old, const aud_item *filtes_new, uint16_t iir_counter)
{
    filtes_old->iir_bypass_flag = filtes_new->iir_bypass_flag;
    filtes_old->iir_counter = filtes_new->iir_counter;

    int i;

    if (filtes_new->iir_counter > iir_counter)
    {
        DRIVERS_TRACE(1, "%s: warning filtes_new->iir_counter:%d", __func__, filtes_new->iir_counter);
        filtes_old->iir_counter = iir_counter;
    }
    else if (filtes_new->iir_counter <= 0)
    {
        DRIVERS_TRACE(1, "%s: warning filtes_new->iir_counter:%d", __func__, filtes_new->iir_counter);
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
#ifdef ANC_IIR_WORKAROUND
        // Workaround for ANC Noise in IIR0 BankA Register Writing (Addresses 0x81C and 0x83C)
        if ((i == 1 || i == 3) && (iir_coefs == anc_ff_iir_coefs0_l)) {
            continue;
        }
#endif
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

static int iir_coef_copy(volatile struct _anc_iir_coefs *iir_coefs, anc_iir_coefs *__restrict filtes_old)
{
#if 0
    DRIVERS_TRACE(1,"%s: iir_coefs:0x%x",__func__,(uint32_t)iir_coefs);
    DRIVERS_TRACE(1,"%s: -filtes_old->coef_a[1]:0x%x",__func__,-filtes_old->coef_a[1]);
    DRIVERS_TRACE(1,"%s: -filtes_old->coef_a[2]:0x%x",__func__, -filtes_old->coef_a[2]);
    DRIVERS_TRACE(1,"%s: -filtes_old->coef_a[0]:0x%x",__func__, filtes_old->coef_b[0]);
    DRIVERS_TRACE(1,"%s: -filtes_old->coef_b[1]:0x%x",__func__, filtes_old->coef_b[1]);
    DRIVERS_TRACE(1,"%s: -filtes_old->coef_b[2]:0x%x",__func__, filtes_old->coef_b[2]);
#endif
    iir_coefs->a1 = -filtes_old->coef_a[1];
    iir_coefs->a2 = -filtes_old->coef_a[2];
    iir_coefs->b0 = filtes_old->coef_b[0];
    iir_coefs->b1 = filtes_old->coef_b[1];
    iir_coefs->b2 = filtes_old->coef_b[2];

    return 0;
}

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
static int iir0_gaina_cfg_gain(void)
{
    int32 iir0_gaina_ch0;
    uint32_t lock;

#if defined(ANC_FF_CHECK)
    iir0_gaina_ch0 = (int32)((((float)ff_ramp_coef_l / 512.0f) * ((float)ff_ramp_gain_l / 512.0f) * ((float)ff_howling_gain_l / 512.0f)) * FIXED_GAIN_RAMP_Q);
#else
    iir0_gaina_ch0 = (int32)((((float)ff_ramp_coef_l / 512.0f) * ((float)ff_ramp_gain_l / 512.0f)) * FIXED_GAIN_RAMP_Q);
#endif

    //  DRIVERS_TRACE(1,"%s:iir0_gaina_ch0:%d,iir0_gaina_ch1:%d",__func__,iir0_gaina_ch0,iir0_gaina_ch1);

    lock = int_lock();
    anc_iir_gain_update->codec_iir0_gain_ext_update = 0;
    *codec_iir0_gaina_ext_ch0 = iir0_gaina_ch0;
    *codec_iir0_gainb_ext_ch0 = ANC_GAIN_RAMP_ZERO;
    hal_sys_timer_delay_us(1);
    anc_iir_gain_update->codec_iir0_gain_ext_update = 1;
    int_unlock(lock);

    return 0;
}

static int iir0_gainb_cfg_gain(void)
{
    int32 iir0_gainb_ch0;
    uint32_t lock;

#if defined(ANC_FF_CHECK)
    iir0_gainb_ch0 = (int32)((((float)ff_ramp_coef_l / 512.0f) * ((float)ff_ramp_gain_l / 512.0f) * ((float)ff_howling_gain_l / 512.0f)) * FIXED_GAIN_RAMP_Q);
#else
    iir0_gainb_ch0 = (int32)((((float)ff_ramp_coef_l / 512.0f) * ((float)ff_ramp_gain_l / 512.0f)) * FIXED_GAIN_RAMP_Q);
#endif

    //  DRIVERS_TRACE(1,"%s:iir0_gainb_ch0:%d,iir0_gainb_ch1:%d",__func__,iir0_gainb_ch0,iir0_gainb_ch1);

    lock = int_lock();
    anc_iir_gain_update->codec_iir0_gain_ext_update = 0;
    *codec_iir0_gaina_ext_ch0 = ANC_GAIN_RAMP_ZERO;
    *codec_iir0_gainb_ext_ch0 = iir0_gainb_ch0;
    hal_sys_timer_delay_us(1);
    anc_iir_gain_update->codec_iir0_gain_ext_update = 1;
    int_unlock(lock);

    return 0;
}

static int iir0_gainb_creat_stop_status(void)
{
    DRIVERS_TRACE(1, "%s", __func__);

    iir0_gaina_cfg_gain();
    trace_counter = 0;

    while (anc_iir0_control->codec_iir0_iirb_stop_status_sync == 0)
    {
        if (trace_counter % TRACE_COUNTER == 0)
        {
            DRIVERS_TRACE(1, "2 codec_iir0_gainb_ext_out_ch0_sync:%d", *codec_iir0_gainb_ext_out_ch0_sync);
        }
        trace_counter++;
    };

    return 0;
}
static int iir0_gaina_creat_stop_status(void)
{
    DRIVERS_TRACE(1, "%s", __func__);

    iir0_gainb_cfg_gain();

    trace_counter = 0;

    while (anc_iir0_control->codec_iir0_iira_stop_status_sync == 0)
    {
        if (trace_counter % TRACE_COUNTER == 0)
        {
            DRIVERS_TRACE(1, "2 codec_iir0_gaina_ext_out_ch0_sync:%d", *codec_iir0_gaina_ext_out_ch0_sync);
        }
        trace_counter++;
    };

    return 0;
}
#endif

static int POSSIBLY_UNUSED anc_limiter_att_rls_coef_generate(anc_iir_coefs *limiter_coef, float lmiter_time)
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
        DRIVERS_TRACE(1, "%s: [warning]Error lmiter_time:%d", __func__, (uint32_t)(lmiter_time * 100));
    }

    return 0;
}

/**
 * @brief      Update coef and store gain,
 * 				fadein: enable: use anc_set_gain(), disable: anc_apply_max_gain() to set gain.
 *
 * @param[in]  cfg   coef and gain
 */
static int anc_set_cfg_internal(const struct_anc_cfg *cfg, enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err = ANC_NO_ERR;
    uint32_t lock;
    // int32 gaina_ch0, gaina_ch1,gainb_ch0,gainb_ch1;
    int32_t POSSIBLY_UNUSED threhold = 0;
    int16_t ff_offset_l = 0;
    int16_t ff_offset_r = 0;
    const aud_item *ff_filtes_l;

    if (anc_type & ANC_FEEDFORWARD && ff_open_flag == 0)
    {
        DRIVERS_TRACE(1, "%s: anc_type=%d not opened", __func__, anc_type);
        return ANC_OTHER_ERR;
    }

    // anc_gain_delay=ANC_GAIN_NO_DELAY;
    ff_filtes_l = &(cfg->anc_cfg_ff_l);
    ff_offset_l = ff_filtes_l->adc_gain_offset;

#if ANC_PROD_TEST
    if (anc_type & ANC_FEEDFORWARD)
    {
        int POSSIBLY_UNUSED time_int = 0;
        int POSSIBLY_UNUSED time_Fra = 0;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        DRIVERS_TRACE(1, "ff gain: %d, counter: %d iir_bypass_flag: %d ", ff_filtes_l->total_gain, ff_filtes_l->iir_counter, ff_filtes_l->iir_bypass_flag);
        DRIVERS_TRACE(1, "ff dac_gain_offset %d, adc_gain_offset %d", ff_filtes_l->dac_gain_offset, ff_filtes_l->adc_gain_offset);

#if ((AUD_SECTION_STRUCT_VERSION & 0x1) == 0)
        DRIVERS_TRACE(1, "limiter_flag:%d", ff_filtes_l->limiter_flag);

        time_int = (int)(ff_filtes_l->limiter_th);
        time_Fra = (int)(ff_filtes_l->limiter_th * 1000) - ((int)ff_filtes_l->limiter_th) * 1000;

        DRIVERS_TRACE(1, "limiter_th(dB):%d.%d", time_int, time_Fra);

        time_int = (int)(ff_filtes_l->limiter_att_time);
        time_Fra = (int)(ff_filtes_l->limiter_att_time * 1000) - ((int)ff_filtes_l->limiter_att_time) * 1000;

        DRIVERS_TRACE(1, "limiter_att_time(ms):%d.%d", time_int, time_Fra);

        time_int = (int)(ff_filtes_l->limiter_rls_time);
        time_Fra = (int)(ff_filtes_l->limiter_rls_time * 1000) - ((int)ff_filtes_l->limiter_rls_time) * 1000;

        DRIVERS_TRACE(1, "limiter_rls_time(ms):%d.%d", time_int, time_Fra);
#endif

        for (int j = 0; j < IIR_FF_COUNTER; j++)
        {
            // DRIVERS_TRACE(1,"iir coef ff l %10d, %10d, %10d, %10d, %10d, %10d",
            DRIVERS_TRACE(1, "iir coef ff l 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x",
                  ff_filtes_l->iir_coef[j].coef_b[0],
                  ff_filtes_l->iir_coef[j].coef_b[1],
                  ff_filtes_l->iir_coef[j].coef_b[2],
                  ff_filtes_l->iir_coef[j].coef_a[0],
                  ff_filtes_l->iir_coef[j].coef_a[1],
                  ff_filtes_l->iir_coef[j].coef_a[2]);
        }
#endif
    }
#endif

    if (anc_type & ANC_FEEDFORWARD)
    {
        DRIVERS_TRACE(1, "%s: ff_offset_l=%d...", __func__, ff_offset_l);
        analog_aud_apply_anc_adc_gain_offset(ANC_FEEDFORWARD, ff_offset_l, ff_offset_r);
    }

// // updata limiter parameters.
#if ((AUD_SECTION_STRUCT_VERSION & 0x1) == 0)

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
    {
#if defined(ANC_LIMITER_FF)
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
    }
#endif

#endif

    /***************************ANC_FEEDFORWARD**********************/
    if (anc_type & ANC_FEEDFORWARD)
    {
        DRIVERS_TRACE(1, "%s: ANC_FEEDFORWARD", __func__);
        // DRIVERS_TRACE(1,"iir0_iira_stop_status_sync:%d,iir0_iirb_stop_status_sync:%d",anc_iir0_control->codec_iir0_iira_stop_status_sync,anc_iir0_control->codec_iir0_iirb_stop_status_sync);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
        {
            DRIVERS_TRACE(1, "%s: ANC_FF ch_L...", __func__);
            iir_filter_para_copy(&ff_filtes_l_old, ff_filtes_l, IIR_FF_COUNTER);
#if defined(AUDIO_ANC_TT_HW)
            if (anc_type & ANC_TALKTHRU)
            {
                DRIVERS_TRACE(1, "%s: ANC_TALKTHRU ch_l with ff...", __func__);

                iir_filter_para_copy(&tt_filtes_l_old, tt_filtes_l, IIR_TT_COUNTER);

                if (tt_filtes_l_old.iir_bypass_flag == 1)
                {
                    max_tt_gain_l = tt_filtes_l_old.total_gain;
                }
                else
                {
                    max_tt_gain_l = (1 << GAIN_Q);
                }
            }
#endif
            if (ff_filtes_l_old.iir_bypass_flag == 1)
            {
                max_ff_gain_l = ff_filtes_l_old.total_gain;
            }
            else
            {
                max_ff_gain_l = (1 << GAIN_Q);
            }

            if (iir0_coef_using == 0)
            {
                trace_counter = 0;

                while (*codec_iir0_gainb_ext_out_ch0_sync > *codec_iir0_gain_ext_th)
                {
                    if (trace_counter % TRACE_COUNTER == 0)
                    {
                        DRIVERS_TRACE(1, "%s: codec_iir0_gainb_ext_out_ch0_sync:%d", __func__, *codec_iir0_gainb_ext_out_ch0_sync);
                    }
                    trace_counter++;
                };

                if (anc_iir0_control->codec_iir0_iirb_stop_status_sync == 0)
                {
                    iir0_gainb_creat_stop_status();
                }

                anc_iir0_control->codec_iir0_ch0_bypass = ff_filtes_l_old.iir_bypass_flag;
                anc_iir0_control->codec_iir0_count_ch0 = ff_filtes_l_old.iir_counter;

                iir_filter_coef_copy(anc_ff_iir_coefs1_l, &ff_filtes_l_old);
#if defined(ANC_LIMITER_FF)
                iir_coef_copy(&(anc_limiter_ff_iir_coefs1_l[0]), &ff_limiter_att_l_old);
                iir_coef_copy(&(anc_limiter_ff_iir_coefs1_l[1]), &ff_limiter_rls_l_old);
#endif
                anc_iir0_control->codec_iir0_iirb_enable = 1;
                iir0_coef_using = 1;
            }
            else
            {
                trace_counter = 0;
                while (*codec_iir0_gaina_ext_out_ch0_sync > *codec_iir0_gain_ext_th)
                {
                    if (trace_counter % TRACE_COUNTER == 0)
                    {
                        DRIVERS_TRACE(1, "%s: codec_iir0_gaina_ext_out_ch0_sync:%d", __func__, *codec_iir0_gaina_ext_out_ch0_sync);
                    }
                    trace_counter++;
                };

                if (anc_iir0_control->codec_iir0_iira_stop_status_sync == 0)
                {
                    iir0_gaina_creat_stop_status();
                }

                anc_iir0_control->codec_iir0_ch0_bypass = ff_filtes_l_old.iir_bypass_flag;
                anc_iir0_control->codec_iir0_count_ch0 = ff_filtes_l_old.iir_counter;

                iir_filter_coef_copy(anc_ff_iir_coefs0_l, &ff_filtes_l_old);
#if defined(ANC_LIMITER_FF)
                iir_coef_copy(&(anc_limiter_ff_iir_coefs0_l[0]), &ff_limiter_att_l_old);
                iir_coef_copy(&(anc_limiter_ff_iir_coefs0_l[1]), &ff_limiter_rls_l_old);
#endif
                anc_iir0_control->codec_iir0_iira_enable = 1;
                iir0_coef_using = 0;
            }

            ff_ramp_coef_l = max_ff_gain_l;

            lock = int_lock();
            anc_iir_gain_update->codec_iir0_gain_ext_update = 0;
            hal_sys_timer_delay_us(1);
            anc_iir_gain_update->codec_iir0_gain_ext_update = 1;
            int_unlock(lock);

            max_ff_gain_l = (1 << GAIN_Q);
        }
#endif

        hwtimer_stop(anc_iir0_set_gain_timer);
        hwtimer_start(anc_iir0_set_gain_timer, anc_switching_gain_delay_ff_tt);
        return err;
    }

    return err;
}

int32_t anc_set_calib_gain(enum ANC_TYPE_T type, int32_t gain_l, int32_t gain_r)
{
    DRIVERS_TRACE(0, "[%s] type: %d, gain_l: %d, gain_r: %d", __func__, type, gain_l, gain_r);
    // +/-6 dB
    if ((gain_l < 512 / 2) || (gain_l > 512 * 2))
    {
        gain_l = 512;
        DRIVERS_TRACE(0, "[%s] WARNING: Invalid gain_l: %d. Use default value", __func__, gain_l);
    }

    if ((gain_r < 512 / 2) || (gain_r > 512 * 2))
    {
        gain_r = 512;
        DRIVERS_TRACE(0, "[%s] WARNING: Invalid gain_r: %d. Use default value", __func__, gain_r);
    }

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
    {
        if (type == ANC_FEEDFORWARD)
        {
            g_calib_ff_gain = gain_l;
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

    DRIVERS_TRACE(1, "%s:0x%x", __func__, anc_type);

    if (cfg == NULL)
    {
        DRIVERS_TRACE(1, "%s: cfg is null", __func__);
        return ANC_OTHER_ERR;
    }

    if (anc_type & ANC_FEEDFORWARD && ff_open_flag == 0)
    {
        DRIVERS_TRACE(1, "%s: anc_type=%d not opened", __func__, anc_type);
        return ANC_OTHER_ERR;
    }

    lock = int_lock();

    bool iir0_using_flag = anc_iir0_using_flag;
    int iir0_type;
    if (anc_type & ANC_FEEDFORWARD)
    {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        anc_cfg_iir0_type = anc_cfg_iir0_type | ANC_FEEDFORWARD;
        anc_cfg_coef_new.anc_cfg_ff_l = cfg->anc_cfg_ff_l;
        if (g_calib_ff_gain != CALIB_GAIN_INVALID)
        {
            anc_cfg_coef_new.anc_cfg_ff_l.total_gain = g_calib_ff_gain;
        }
#endif
    }

    iir0_type = anc_cfg_iir0_type;

    if (anc_type & ANC_FEEDFORWARD)
    {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_iir0_using_flag == 0)
        {
            anc_iir0_using_flag = 1;
            anc_cfg_iir0_type = 0;
            anc_iir0_reserve_flag = 0;
            anc_cfg_coef.anc_cfg_ff_l = anc_cfg_coef_new.anc_cfg_ff_l;
            anc_cfg_coef.anc_cfg_tt_l = anc_cfg_coef_new.anc_cfg_tt_l;
        }
        else
        {
            anc_iir0_reserve_flag = 1;
        }
#endif
    }
    int_unlock(lock);

    if (anc_type & ANC_FEEDFORWARD)
    {
        if (iir0_using_flag == 0)
        {
            DRIVERS_TRACE(1, "%s: iir0_type: 0x%x", __func__, iir0_type);

            anc_set_cfg_internal(&anc_cfg_coef, iir0_type);
            hwtimer_stop(anc_iir0_switching_timer);
            hwtimer_start(anc_iir0_switching_timer, anc_switching_coef_delay_ff_tt);
        }
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
    DRIVERS_TRACE(1, "%s:...", __func__);
    // disable  iir clock
    anc_iir_clock->en_clk_iir_anc = 0x0;

    // disable ANC;
    anc_control->codec_anc_enable_ch0 = 0;

    // set the FF calib gain;
    anc_iir_calib_gain_config->codec_anc_calib_gain_update_ff_ch0 = 0;
    anc_ff_iir_calib_gain->codec_anc_calib_gain_coef_ff_ch0 = CALIB_GAIN_Q;
    anc_iir_calib_gain_config->codec_anc_calib_gain_update_ff_ch0 = 1;

    // set the FF calib gain;
    anc_iir_calib_gain_config->codec_anc_calib_gain_pass0_ff_ch0 = 1;

    anc_control->codec_anc_rate_sel = 0;

    anc_control->codec_feedback_ch0 = 0;
    anc_control->codec_dual_anc_ch0 = 0;

    // 8:-120dB 83:-100dB
    *codec_iir0_gain_ext_th = ANC_GAIN_RAMP_THRESHLD;

    // iir0
    anc_iir0_control->codec_iir0_enable = 0;
    anc_iir0_control->codec_iir0_iira_enable = 0;
    anc_iir0_control->codec_iir0_iirb_enable = 0;
    anc_iir0_control->codec_iir0_ch0_bypass = 1;
    anc_iir0_control->codec_iir0_lmt_ch0_bypass = 1;
    anc_iir0_control->codec_iir0_count_ch0 = 0;
    anc_iir0_control->codec_iir0_coef_swap = 0;
    anc_iir0_control->codec_iir0_auto_stop = 0;

    // unmute ANC.
    anc_control->codec_anc_mute_ch0 = 0;

    // disable gain updated when pass0
    anc_ff_gain->codec_anc_mute_gain_pass0_ff_ch0 = 1;

    ff_ramp_coef_l = 0;

#ifdef ANC_FAST_CALIB_MODE
    if (analog_debug_get_anc_calib_mode())
    {
        anc_switching_gain_delay_ff_tt = ANC_SET_GAIN_DELAY_SHORT;
        anc_switching_coef_delay_ff_tt = ANC_SET_GAIN_DELAY_SHORT + ANC_SET_GAIN_CALIB_TIME;
    }
    else
#endif
    {
        anc_switching_gain_delay_ff_tt = ANC_SET_GAIN_DELAY_LONG;
        anc_switching_coef_delay_ff_tt = ANC_SET_GAIN_DELAY_LONG + ANC_SET_GAIN_TIME;
    }
    anc_cfg_iir0_type = 0;
    anc_iir0_using_flag = 0;

    for (int i = 0; i < IIR_COUNTER; i++)
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

    return;
}

void anc_disable_gain_updated_when_pass0(uint8_t on)
{
    /*
       anc_ff_gain->codec_anc_mute_gain_pass0_ff_ch0=on;

       anc_fb_gain->codec_anc_mute_gain_pass0_fb_ch0=on;
       */
}

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
static void anc_iir0_init(void)
{
    anc_iir_coefs iir_coef_gain_ramp_ff;
#if defined(AUDIO_ANC_TT_HW)
    anc_iir_coefs iir_coef_gain_ramp_tt;
#endif
    // enable iir0 clock
    anc_iir_clock->en_clk_iir_anc |= CODEC_EN_CLK_IIR_IIR0;

#ifdef ANC_FAST_CALIB_MODE
    if (analog_debug_get_anc_calib_mode())
    {
        iir_coef_gain_ramp_ff = iir_coef_gain_ramp_ff_calib;
    }
    else
#endif
    {
        iir_coef_gain_ramp_ff = iir_coef_gain_ramp_ff_normal;
    }

    anc_iir0_control->codec_iir0_enable = 0;

    anc_iir0_control->codec_iir0_iira_enable = 0;
    anc_iir0_control->codec_iir0_iirb_enable = 0;

    anc_iir0_control->codec_iir0_coef_swap = 0;
    anc_iir0_control->codec_iir0_auto_stop = 0;

#ifdef ANC_GAIN_RAMP
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
#endif

#if defined(ANC_LIMITER_FF)
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

    for (int i = 0; i < IIR_COUNTER; i++)
    {
#ifdef ANC_IIR_WORKAROUND
        // Workaround for ANC Noise in IIR0 BankA Register Writing (Addresses 0x81C and 0x83C)
        if (i == 1 || i == 3) {
            continue;
        }
#endif
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
    }

    anc_iir0_control->codec_iir0_ch0_bypass = 0;
    anc_iir0_control->codec_iir0_count_ch0 = IIR_FF_COUNTER;

    anc_iir0_control->codec_iir0_auto_stop = 1;
    anc_iir0_control->codec_iir0_iira_enable = 1;
    anc_iir0_control->codec_iir0_iirb_enable = 1;
    anc_iir0_control->codec_iir0_enable = 1;

    iir0_coef_using = 0;
}
#endif

static void anc_ctrl_reg_open(enum ANC_TYPE_T anc_type)
{
    DRIVERS_TRACE(1, "%s", __func__);

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

            for (int i = 0; i < IIR_COUNTER; i++)
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

            anc_iir0_init();
        }
#endif
    }

    if (ff_open_flag == 1)
    {
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
        {
            anc_control->codec_anc_enable_ch0 = 1;
        }
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

        if (ff_open_flag == 0)
        {
            // disable iir0 clock
            anc_iir_clock->en_clk_iir_anc &= ~CODEC_EN_CLK_IIR_IIR0;
        }
    }
#endif
}

int anc_opened(enum ANC_TYPE_T anc_type)
{
    int32_t open_flag = 0;

    if (anc_type & ANC_FEEDFORWARD)
    {
        open_flag = open_flag | (ff_open_flag << 1);
    }
    return open_flag;
}

#if defined(ANC_FF_CHECK)
static int32_t howling_check_delay = 5000; // recovery time ms.
static int32_t howling_timer = 400;        // ms.
static int32_t howling_set_gain_level = 0; // ms.

#define HOWLING_GAIN_0dB (512)
#define HOWLING_GAIN_m3dB (362)
#define HOWLING_GAIN_m6dB (256)
#define HOWLING_GAIN_m9dB (181)
#define HOWLING_GAIN_m12dB (128)

static void anc_howling_check_irq_handler(uint32_t status)
{
    // DRIVERS_TRACE(1,"%s,", __func__);
    struct _codec_int_config *codec_int_config = (struct _codec_int_config *)&status;
    uint32 howling_time = TICKS_TO_MS(hal_sys_timer_get());

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
    {
        if (codec_int_config->fb_check_error_trig_ch0)
        {
            DRIVERS_TRACE(1, "howling_cnt_l:%d", howling_cnt_l);

            DRIVERS_TRACE(1, "*Threshold:%10d,Data Energy :%10d", *codec_fb_check_threshold_ch0, *codec_fb_check_data_avg_keep_ch0);

            if (howling_cnt_l == 0)
            {
                ff_howling_gain_l = HOWLING_GAIN_m3dB;

                howling_cnt_l = 1;
                howling_time_l = howling_time;
            }
            else if (howling_cnt_l == 1 && (howling_time > howling_time_l + howling_timer))
            {
                ff_howling_gain_l = HOWLING_GAIN_m6dB;

                howling_cnt_l = 2;
                howling_time_l = howling_time;
            }
            else if (howling_cnt_l == 2 && (howling_time > howling_time_l + howling_timer))
            {
                ff_howling_gain_l = HOWLING_GAIN_m9dB;

                howling_cnt_l = 3;
                howling_time_l = howling_time;
            }
            else if (howling_cnt_l == 3 && (howling_time > howling_time_l + howling_timer))
            {
                ff_howling_gain_l = 0;

                howling_cnt_l = 4;
            }

            DRIVERS_TRACE(1, "ff_howling_gain_l:%d", ff_howling_gain_l);

            ff_check_ch0_config->codec_fb_check_enable_ch0 = 0;

            hwtimer_stop(anc_howling_check_dev_timer);
            hwtimer_start(anc_howling_check_dev_timer, howling_check_delay);
#if defined(ANC_FF_CHECK)
            if (iir0_coef_using == 0)
            {
                iir0_gaina_cfg_gain();
            }
            else
            {
                iir0_gainb_cfg_gain();
            }
#endif
            ff_check_ch0_config->codec_fb_check_enable_ch0 = 1;
        }
    }
#endif
}

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
static void anc_howling_check_timer_handler(void *param)
{
    DRIVERS_TRACE(1, "%s", __func__);
#if defined(ANC_FF_CHECK)
    ff_howling_gain_l = HOWLING_GAIN_0dB;

    if (iir0_coef_using == 0)
    {
        iir0_gaina_cfg_gain();
    }
    else
    {
        iir0_gainb_cfg_gain();
    }
#endif
    howling_cnt_l = howling_set_gain_level;
    howling_time_l = 0;
}
#endif
#endif

int32_t anc_process_set_cfg(const anc_process_cfg *anc_cfg)
{
    const anc_howling_check_cfg *howling_check_cfg = &(anc_cfg->howling_check_cfg);
    DRIVERS_TRACE(1, "%s: recover_time=%d(ms), interval_time=%d, gain_level=%d", __func__,
          howling_check_cfg->howling_recover_time,
          howling_check_cfg->howling_interval_time,
          howling_check_cfg->howling_set_gain_level);

#if defined(ANC_FF_CHECK)
    howling_check_delay = ((MS_TO_TICKS(howling_check_cfg->howling_recover_time)));
    howling_timer = howling_check_cfg->howling_interval_time;
    howling_set_gain_level = howling_check_cfg->howling_set_gain_level;
#endif

    return 0;
}

static void anc_iir0_set_gain_timer_handler(void *param)
{
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    DRIVERS_TRACE(1, "%s: CH_L...", __func__);
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
    {
        if (iir0_coef_using == 0)
        {
            iir0_gaina_cfg_gain();
        }
        else
        {
            iir0_gainb_cfg_gain();
        }
    }
#endif
}

static void anc_iir0_switching_timer_handler(void *param)
{
    DRIVERS_TRACE(1, "%s", __func__);

    uint32_t lock;
    lock = int_lock();

    int iir0_type = anc_cfg_iir0_type;
    bool iir0_reserve_flag = anc_iir0_reserve_flag;

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_iir0_reserve_flag)
    {
        anc_cfg_coef.anc_cfg_ff_l = anc_cfg_coef_new.anc_cfg_ff_l;
        anc_cfg_coef.anc_cfg_tt_l = anc_cfg_coef_new.anc_cfg_tt_l;
        anc_iir0_reserve_flag = 0;
        anc_cfg_iir0_type = 0;
    }
    else
    {
        anc_iir0_using_flag = 0;
    }
#endif
    int_unlock(lock);

    if (iir0_reserve_flag)
    {
        anc_set_cfg_internal(&anc_cfg_coef, iir0_type);

        hwtimer_stop(anc_iir0_switching_timer);
        hwtimer_start(anc_iir0_switching_timer, anc_switching_coef_delay_ff_tt);
    }
    return;
}

#if defined(ANC_FF_CHECK)
static void anc_ff_fb_tt_check_init(void)
{
    hal_codec_anc_fb_check_set_irq_handler(anc_howling_check_irq_handler);
    fb_check_open_flag = 1;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    ff_check_ch0_config->codec_fb_check_enable_ch0 = 0;
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

    ff_check_ch0_config->codec_fb_check_acc_sample_rate_ch0 = 3;
#if defined(ANC_FF_CHECK)
    ff_check_ch0_config->codec_fb_check_src_sel_ch0 = 2;
#endif
    ff_check_ch0_config->codec_fb_check_acc_window_ch0 = 64;
    ff_check_ch0_config->codec_fb_check_trig_window_ch0 = 64;

    *codec_fb_check_threshold_ch0 = ANC_HOWLING_THRESHOLD_0dB;

    ff_check_ch0_config->codec_fb_check_enable_ch0 = 1;

    ff_howling_gain_l = 512;
    fb_howling_gain_l = 512;
    tt_howling_gain_l = 512;

    howling_cnt_l = howling_set_gain_level;
    howling_time_l = 0;
#endif

    return;
}
#endif

int anc_open(enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err = ANC_NO_ERR;

    DRIVERS_TRACE(1, "%s", __func__);

    if (anc_type == ANC_FEEDFORWARD && ff_open_flag == 1)
        return err;

    anc_cfg_lock();

    if (anc_iir0_set_gain_timer == NULL)
    {
        anc_iir0_set_gain_timer = hwtimer_alloc(anc_iir0_set_gain_timer_handler, NULL);
        ASSERT(anc_iir0_set_gain_timer, "Failed to alloc anc_iir0_set_gain_timer");
    }

    if (anc_iir0_switching_timer == NULL)
    {
        anc_iir0_switching_timer = hwtimer_alloc(anc_iir0_switching_timer_handler, NULL);
        ASSERT(anc_iir0_switching_timer, "Failed to alloc anc_iir0_switching_timer");
    }

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
#if defined(ANC_FF_CHECK)
    if (anc_howling_check_dev_timer == NULL)
    {
        anc_howling_check_dev_timer = hwtimer_alloc(anc_howling_check_timer_handler, NULL);
        ASSERT(anc_howling_check_dev_timer, "Failed to alloc usbdev_timer");
    }
#endif
#endif

    // return 0;
    if (ff_open_flag == 0)
    {
        if (IIR_FF_COUNTER > 6) {
            if (hal_cmu_get_audio_resample_status()) {
                hal_codec_iir_enable(48000000);
            } else {
                hal_codec_iir_enable(24576000*2);
            }
        } else {
            if (hal_cmu_get_audio_resample_status()) {
                hal_codec_iir_enable(24000000);
            } else {
                hal_codec_iir_enable(24576000);
            }
        }

        anc_ctrl_reg_init();
        iir0_coef_using = 0;
    }

#if defined(ANC_FF_CHECK)
    if (anc_type == ANC_FEEDFORWARD)
    {
        anc_ff_fb_tt_check_init();
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
        ff_open_flag = 1;
    }

    anc_ctrl_reg_open(anc_type);

    anc_cfg_unlock();

    return err;
}

#ifdef ANC_IIR_WORKAROUND
void anc_iir_coefs_init(void)
{
    anc_open(ANC_FEEDFORWARD);
    for (int i = 0; i < IIR_COUNTER; i++)
    {
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
        // Workaround for ANC Noise in IIR0 BankA Register Writing (Addresses 0x81C and 0x83C)
        if (i == 1 || i == 3) {
            anc_ff_iir_coefs0_l[i].b0 = 0x08000000;
            anc_ff_iir_coefs1_l[i].b0 = 0x08000000;
        }
    }
    anc_close(ANC_FEEDFORWARD);
}
#endif

void anc_close(enum ANC_TYPE_T anc_type)
{
    if (anc_type == ANC_FEEDFORWARD)
    {
        DRIVERS_TRACE(1, "%s: ANC_FEEDFORWARD", __func__);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
        {
            max_ff_gain_l = 0;
#ifdef ANC_GAIN_RAMP
            ff_ramp_gain_l = 0;
#endif
#if defined(ANC_FF_CHECK)
            ff_check_ch0_config->codec_fb_check_enable_ch0 = 0;
            codec_int_config->fb_check_error_trig_ch0 = 1;
            codec_mask_config->fb_check_error_trig_ch0_mask = 0;
            ff_howling_gain_l = 0;
#endif
        }
#endif

#if defined(ANC_FF_CHECK)
        fb_check_open_flag = 0;
#endif
        ff_open_flag = 0;
    }

    anc_ctrl_reg_close(anc_type);

    if (ff_open_flag == 0)
    {
        hwtimer_stop(anc_iir0_set_gain_timer);
        hwtimer_stop(anc_iir0_switching_timer);

        hal_codec_iir_disable();
        anc_iir_clock->en_clk_iir_anc = 0x0;
    }

    return;
}

int anc_enable(void)
{
    DRIVERS_TRACE(1, "%s", __func__);

    anc_cfg_lock();
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
    {
        anc_control->codec_anc_enable_ch0 = 1;
    }
#endif

    anc_cfg_unlock();

    return ANC_NO_ERR;
}

int anc_disable(void)
{
    DRIVERS_TRACE(1, "%s", __func__);

    anc_cfg_lock();
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
    {
#ifndef ANC_GAIN_RAMP
        anc_control->codec_anc_enable_ch0 = 0;
#endif
    }
#endif

    anc_cfg_unlock();

    return ANC_NO_ERR;
}

int anc_set_ff_and_tt_gain(int32_t gain_ff_l, int32_t gain_ff_r, int32_t gain_tt_l, int32_t gain_tt_r)
{
    ANC_ERROR err = ANC_NO_ERR;

    // if(gain_ch_l==511||gain_ch_l==0||gain_ch_l==250)
    {
        DRIVERS_TRACE(1, "gain_ff_l:%d,gain_tt_l:%d", gain_ff_l, gain_tt_l);
        DRIVERS_TRACE(1, "gain_ff_r:%d,gain_tt_r:%d", gain_ff_r, gain_tt_r);
    }

    anc_cfg_lock();
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    ff_ramp_gain_l = gain_ff_l;
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
    {
        if (iir0_coef_using == 0)
        {
            iir0_gaina_cfg_gain();
        }
        else
        {
            iir0_gainb_cfg_gain();
        }
    }
#endif
    anc_cfg_unlock();

    return err;
}

static float g_ff_adaptive_gain = 1;
int anc_set_gain2_float(float gain)
{
    if (gain > 1)
    {
        DRIVERS_TRACE(2, "[%s] warning input value = %d/100 > 1", __func__, (int)(gain * 100));
        g_ff_adaptive_gain = 1;
    }
    else if (gain < 0)
    {
        DRIVERS_TRACE(2, "[%s] warning input value = %d/100 < 1", __func__, (int)(gain * 100));
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
        DRIVERS_TRACE(1, "anc_set_gain anc_type:%d, gain_ch_l:%d,gain_ch_r:%d", anc_type, gain_ch_l, gain_ch_r);
    }

    if (anc_type == ANC_FEEDFORWARD && ff_open_flag == 0)
    {
        DRIVERS_TRACE(1, "%s: anc_type=%d not opened", __func__, anc_type);
        return ANC_OTHER_ERR;
    }

    anc_cfg_lock();

    // DRIVERS_TRACE(1,"iir0_iira_stop_status_sync:%d,iir0_iirb_stop_status_sync:%d",anc_iir0_control->codec_iir0_iira_stop_status_sync,anc_iir0_control->codec_iir0_iirb_stop_status_sync);
    // DRIVERS_TRACE(1,"%s: iir0_coef_using:%d,iir1_coef_using:%d,iir2_coef_using:%d,iir3_coef_using:%d", __func__,iir0_coef_using,iir1_coef_using,iir2_coef_using,iir3_coef_using);

    if (anc_type & ANC_FEEDFORWARD)
    {
        gain_ch_l = (uint32_t)(gain_ch_l * g_ff_adaptive_gain);
        gain_ch_r = (uint32_t)(gain_ch_r * g_ff_adaptive_gain);
        // DRIVERS_TRACE(0,"!!!!!!! type =% d internal gain = %d coef = %d",anc_type,gain_ch_l,(int)(100*g_ff_adaptive_gain));
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
            if (iir0_coef_using == 0)
            {
                iir0_gaina_cfg_gain();
            }
            else
            {
                iir0_gainb_cfg_gain();
            }
        }
#endif
    }
#else
    if (anc_type & ANC_FEEDFORWARD)
    {
        // Set the FF gain;
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        anc_ff_gain->codec_anc_mute_gain_update_ff_ch0 = 0;

        anc_ff_gain->codec_anc_mute_gain_ff_ch0 = gain_ch_l;

        anc_ff_gain->codec_anc_mute_gain_update_ff_ch0 = 1;
#endif
    }
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
#else

    if (anc_type & ANC_FEEDFORWARD)
    {
        // Get the FF gain;
        *gain_ch_l = anc_ff_gain->codec_anc_mute_gain_ff_ch0;
        *gain_ch_r = anc_ff_gain->codec_anc_mute_gain_ff_ch0;
    }
#endif
#endif

    anc_cfg_unlock();

    //  DRIVERS_TRACE(1,"anc_get_gain gain_ch_l:%d,gain_ch_r:%d",*gain_ch_l,*gain_ch_r);
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
#endif

    anc_cfg_unlock();

    //	DRIVERS_TRACE(1,"anc_get_cfg_gain gain_ch_l:%d,gain_ch_r:%d",*gain_ch_l,*gain_ch_r);
    return err;
}

void anc_set_ch_map(int32_t ch_map)
{
    DRIVERS_TRACE(1, "%s ch_map:%d", __func__, ch_map);
    anc_output_ch_map = ch_map;
    return;
}

void anc_howling_check_enable(int32_t flag)
{
    uint32_t lock;

    DRIVERS_TRACE(1, "%s flag:%d", __func__, flag);

    lock = int_lock();
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
    {
#if defined(ANC_FF_CHECK)
        if (fb_check_open_flag == 1)
        {
            if (flag == 0)
            {
                ff_check_ch0_config->codec_fb_check_enable_ch0 = 0;
                codec_mask_config->fb_check_error_trig_ch0_mask = 0;
                codec_int_config->fb_check_error_trig_ch0 = 1;
            }
            else
            {
                codec_int_config->fb_check_error_trig_ch0 = 1;
                codec_mask_config->fb_check_error_trig_ch0_mask = 1;
                ff_check_ch0_config->codec_fb_check_enable_ch0 = 1;
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
    DRIVERS_TRACE(1, "%s", __func__);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
    if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
    {
#if defined(ANC_FF_CHECK)
        ff_howling_gain_l = 512;
        if (iir0_coef_using == 0)
        {
            iir0_gaina_cfg_gain();
        }
        else
        {
            iir0_gainb_cfg_gain();
        }
#endif
    }
#endif
}

int anc_set_switching_delay(ANC_SWITCHING_DELAY anc_switching_delay, enum ANC_TYPE_T anc_type)
{
    anc_iir_coefs iir_coef_gain_ramp;
    int anc_switching_coef_delay;
    int anc_switching_gain_delay;

    DRIVERS_TRACE(1, "%s:anc_switching_delay:%d,anc_type:%d", __func__, anc_switching_delay, anc_type);

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

    if (anc_type & ANC_FEEDFORWARD)
    {
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
        anc_iir0_control->codec_iir0_enable = 0;
        if (anc_output_ch_map & AUD_CHANNEL_MAP_CH0)
        {
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
        }
        anc_iir0_control->codec_iir0_enable = 1;
#endif

        anc_switching_gain_delay_ff_tt = anc_switching_gain_delay;
        anc_switching_coef_delay_ff_tt = anc_switching_coef_delay;
    }

    int_unlock(lock);

    return 0;
}

int anc_howling_set(ANC_HOWLING_WINDOW window, ANC_HOWLING_THRESHOLD threshold)
{
#if defined(ANC_FF_CHECK)
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
            ff_check_ch0_config->codec_fb_check_enable_ch0 = 0;

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
            DRIVERS_TRACE(1, "%s:acc_window:%d,trig_window:%d", __func__, acc_window, trig_window);
            DRIVERS_TRACE(1, "%s:check_threshold:0x%x", __func__, check_threshold);

            ff_check_ch0_config->codec_fb_check_acc_window_ch0 = acc_window;
            ff_check_ch0_config->codec_fb_check_trig_window_ch0 = trig_window;

            *codec_fb_check_threshold_ch0 = check_threshold;

            ff_check_ch0_config->codec_fb_check_enable_ch0 = 1;
        }
    }
#endif

    int_unlock(lock);
#endif
    return 0;
}

int anc_adc_data_select(ANC_ADC_DATA data_select)
{
    return 0;
}

int anc_limiter_enable(enum ANC_TYPE_T anc_type)
{
    DRIVERS_TRACE(1, "%s anc_type:%d", __func__, anc_type);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
#if defined(ANC_LIMITER_FF)
    if (anc_type & ANC_FEEDFORWARD)
    {
        // Enable the FF limiter;
        anc_iir0_control->codec_iir0_lmt_ch0_bypass = 0;
    }
#endif
#endif

    return 0;
}

int anc_limiter_disable(enum ANC_TYPE_T anc_type)
{
    DRIVERS_TRACE(1, "%s anc_type:%d", __func__, anc_type);
#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
#if defined(ANC_LIMITER_FF)
    if (anc_type & ANC_FEEDFORWARD)
    {
        // Disable the FF limiter;
        anc_iir0_control->codec_iir0_lmt_ch0_bypass = 1;
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
    DRIVERS_TRACE(1, "%s threhold_db:%d,threhold:0x%x", __func__, threhold_db, threhold);

#if (ANC_AUD_OUTPUT_PATH_SPEAKER_DEV & AUD_CHANNEL_MAP_CH0)
#if defined(ANC_LIMITER_FF)
    if (anc_type & ANC_FEEDFORWARD)
    {
        anc_iir_gain_update->codec_iir0_lmt_th_update_ch0 = 0;
        // limiter threshold, 0dB: 0x7fffff
        *codec_iir0_lmt_th_ch0 = threhold;
        hal_sys_timer_delay_us(1);
        anc_iir_gain_update->codec_iir0_lmt_th_update_ch0 = 1;
    }
#endif
#endif
    return 0;
}
