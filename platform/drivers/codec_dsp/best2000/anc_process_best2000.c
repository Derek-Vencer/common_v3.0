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
<31:21> Reserved 11��b0
<20:18> Loop_sel_R 3��b001  //select ad for loop to R.
<17:15> Loop_sel_L 3��b000  //select ad for loop to L.
<14:12> adc_in_sel_ch4 3��b100
<11:9> adc_in_sel_ch3 3��b011
<8:6> adc_in_sel_ch2 3��b010
<5:3> adc_in_sel_ch1 3��b001
<2:0> adc_in_sel_ch0 3��b000  //select analong AD to digital AD(0->A.1->B,2->C,3->D,4->E)
*/
struct _anc_adc_sel_ana_ch
{
	uint32 adc_in_sel_ch0 : 3;
	uint32 adc_in_sel_ch1 : 3;
	uint32 adc_in_sel_ch2 : 3;

	uint32 adc_in_sel_ch3 : 3;
	uint32 adc_in_sel_ch4 : 3;
	uint32 Loop_sel_L : 3;
	uint32 Loop_sel_R : 3;

	uint32 Reserved : 11;
};


/*

REG32B_13 �� offset�� 0x00a8��BIT NAME DEFINITION DEFAULT
<31:17> reserved
<16:14> iir_count_ch3 3��b0
<13:11> iir_count_ch2 3��b0
<10:8> iir_count_ch1 3��b0
<7:5> iir_count_ch0 3��b0
<4> iir_ch3_bypass 1��b0
<3> iir_ch2_bypass 1��b0
<2> iir_ch1_bypass 1��b0
<1> iir_ch0_bypass 1��b0
<0> iir_enable 1��b0
*/

struct _anc_iir_control
{
	uint32 iir_enable : 1;

	uint32 iir_ch0_bypass : 1;
	uint32 iir_ch1_bypass : 1;
	uint32 iir_ch2_bypass : 1;
	uint32 iir_ch3_bypass : 1;

	uint32 iir_count_ch0 : 3;
	uint32 iir_count_ch1 : 3;
	uint32 iir_count_ch2 : 3;
	uint32 iir_count_ch3 : 3;

	uint32 reserved : 15;
};


/*
REG32B_09 �� offset�� 0x0098��BIT NAME DEFINITION DEFAULT
<31> Anc_ch1_mute 1: anc ch1 output 0 1��b0
<30> Anc_ch0_mute 1: anc ch0 output 0 1��b0
<29> Anc_fs1_to_fir_bypass 1: anc fs1 fir bypass 1��b0
<28> Anc_fs0_to_fir_bypass 1: anc fs0 fir bypass 1��b0
<27> Anc_pdu1_to_fir_bypass 1: anc pdu1 fir bypass 1��b0
<26> Anc_pdu0_to_fir_bypass 1: anc pdu0 fir bypass 1��b0
<25> Dsd_sample_rate 0: x64 1: x128 1��b0
<24> Dsd_data_inv Inv dsd input data 1��b0
<23> Dac_sdm_swap 1: dac L/R sdm out swap 1��b0
<22> Pdm_adc_sel_ch4 0 sel adc path, 1 sel pdm path 1��b0
<21> Pdm_adc_sel_ch3 0 sel adc path, 1 sel pdm path 1��b0
<20> Pdm_adc_sel_ch2 0 sel adc path, 1 sel pdm path 1��b0
<19> sidetone_mic_sel L or R 1��b0
<18:15> Sin1k_step Step 1 for 1k @48k sample rate 4��h1<14> dac_gain_sel 0: direct gain 1: gain updated when pass 0 1��b0
<13> adc_gain_sel 0: direct gain 1: gain updated when pass 0 1��b0
<12> adc_lr_swap 1��b0
<11> dac_lr_swap 1��b0
<10:7> s_adc_volume_ch1<3:0> mic gain,-12dB(0001) ~16dB(1111) 0000: mute 4��b0111
<6> Dsd_enable_R 1: use adc ch for dsd 1��b0
<5> Dsd_enable_L 1: use adc ch for dsd 1��b0
<4> Pdm_adc_sel_ch1 0 sel adc path, 1 sel pdm path 1��b0
<3> Pdm_adc_sel_ch0 0 sel adc path, 1 sel pdm path 1��b0
<2> Dsd_ch_sel 0:use ch0/ch1, 1:use ch2/ch3 1��b0
<1> Pdm_data_inv 0 pdm ori value, 1 pdm inv value 1��b0
<0> Pdm_enable Pdm interface enable 1��b0
*/

struct _anc_fir_bypass_anc_mute
{
	uint32 NotAncRegs : 26;

	uint32 Anc_pdu0_to_fir_bypass : 1;
	uint32 Anc_pdu1_to_fir_bypass : 1;
	uint32 Anc_fs0_to_fir_bypass : 1;
	uint32 Anc_fs1_to_fir_bypass : 1;

	uint32 Anc_ch0_mute : 1;
	uint32 Anc_ch1_mute : 1;
};


/*

REG32B_14 �� offset�� 0x00ac��BIT NAME DEFINITION DEFAULT
<31:24> Reserved 8��b0
<23:12> Mute_gain_coef_pdu_ch1 3.9�� 0x200 for *1 12��b0
<11:0> Mute_gain_coef_pdu_ch0 3.9�� 0x200 for *1 12��b0
*/


struct _anc_ff_gain
{
	int32 Mute_gain_coef_pdu_ch0 : 12;
	int32 Mute_gain_coef_pdu_ch1 : 12;

	uint32 Reserved : 8;
};



/*
REG32B_15 �� offset�� 0x00b0��BIT NAME DEFINITION DEFAULT
<31:24> Reserved 8��b0
<23:12> Mute_gain_coef_fs_ch1 3.9�� 0x200 for *1 12��b0
<11:0> Mute_gain_coef_fs_ch0 3.9�� 0x200 for *1 12��b0
*/

struct _anc_fb_gain
{
	int32 Mute_gain_coef_fs_ch0 : 12;
	int32 Mute_gain_coef_fs_ch1 : 12;

	uint32 Reserved : 8;
};


/*
REG32B_07 �� offset�� 0x0090�� for anc BIT NAME DEFINITION DEFAULT

<7> Music cancel rate select 0:use 384k, 1:use 768k 1��b0
<6> Anc rate select 0:use 384k, 1:use 768k 1��b0
<5> Mute_gain_pass0_ch1 0: gain updated when pass0 1��b0
<4> Mute_gain_pass0_ch0 0: gain updated when pass0 1��b0
<3> Dual_anc_ch1 0 for pdu anc, 1 for pdu+fs 1��b0 anc
<2> Dual_anc_ch0 0 for pdu anc, 1 for pdu+fs anc 1��b0
<1> Anc_enable_ch1 1��b0
<0> Anc_enable_ch0 1��b0

*/

struct _anc_control
{
	uint32 Anc_enable_ch0 : 1;
	uint32 Anc_enable_ch1 : 1;

	uint32 Dual_anc_ch0 : 1;
	uint32 Dual_anc_ch1 : 1;

	uint32 Mute_gain_pass0_ch0 : 1;
	uint32 Mute_gain_pass0_ch1 : 1;

	uint32 Anc_rate_select : 1;
	uint32 Music_cancel_rate_select : 1;

	uint32 Reserved : 24;

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

FIR Config��0x100- fir ���ƼĴ�����
λ	Name	Type	����
31:28	reserved
27	Iir_coef_swap	r/w	0: coef0,  1: coef1
26	Pdu_fs_swap	r/w	0: pdu&pdu swap   1: pdu&fs swap
25	Anc_coef_sel_pdu1_fs1	r/w	0: original  1: pdu1/fs1 swap
24	Anc_coef_sel_pdu0_fs0	r/w	0: original  1: pdu0/fs0 swap
23	Anc_coef_sel_fs0_fs1	r/w	0��original fs  1��ch0/ch1 fs swap
22	Anc_coef_sel_pdu0_pdu1	r/w	0��original pdu  1��ch0/ch1 pdu swap
21	dma_fir_tx_ctrl	r/w	����dma��fir���������
20	dma_fir_rx_ctrl	r/w	����dma��fir��д������
19	Fir3_256p0_512p1	r/w	0��fir6, fir7����256����ã�1���ϲ���512�����
18	Fir2_256p0_512p1	r/w	0��fir4, fir5����256����ã�1���ϲ���512�����
17	Fir1_256p0_512p1	r/w	0��fir2, fir3����256����ã�1���ϲ���512�����
16	fir0_256p0_512p1	r/w	0��fir0, fir1����256����ã�1���ϲ���512�����
15	fir enable  ch7	r/w
14	fir enable  ch6	r/w
13	fir enable  ch5	r/w
12	fir enable  ch4	r/w
11	fir enable  ch3	r/w
10	fir enable  ch2	r/w
9	fir enable  ch1	r/w
8	fir enable  ch0	r/w	��������������Ӳ��fir����
7	Stream enable  ch7	r/w
6	Stream enable  ch6	r/w
5	Stream enable  ch5	r/w
4	Stream enable  ch4	r/w
3	Stream enable  ch3	r/w
2	Stream enable  ch2	r/w
1	Stream enable  ch1	r/w
0	Stream enable  ch0	r/w	���� anc �� adc ������ģʽ��
*/



struct _fir_control
{
	uint32 Stream_enable_ch0 : 1;
	uint32 Stream_enable_ch1 : 1;
	uint32 Stream_enable_ch2 : 1;
	uint32 Stream_enable_ch3 : 1;
	uint32 Stream_enable_ch4 : 1;
	uint32 Stream_enable_ch5 : 1;
	uint32 Stream_enable_ch6 : 1;
	uint32 Stream_enable_ch7 : 1;

	uint32 Fir_enable_ch0 : 1;
	uint32 Fir_enable_ch1 : 1;
	uint32 Fir_enable_ch2 : 1;
	uint32 Fir_enable_ch3 : 1;
	uint32 Fir_enable_ch4 : 1;
	uint32 Fir_enable_ch5 : 1;
	uint32 Fir_enable_ch6 : 1;
	uint32 Fir_enable_ch7 : 1;

	uint32 Fir0_256p0_512p1 : 1;
	uint32 Fir1_256p0_512p1 : 1;
	uint32 Fir2_256p0_512p1 : 1;
	uint32 Fir3_256p0_512p1 : 1;

	uint32 dma_fir_rx_ctrl : 1;
	uint32 dma_fir_tx_ctrl : 1;

	uint32 Anc_coef_sel_pdu0_pdu1 : 1;
	uint32 Anc_coef_sel_fs0_fs1 : 1;
	uint32 Anc_coef_sel_pdu0_fs0 : 1;
	uint32 Anc_coef_sel_pdu1_fs1 : 1;

	uint32 Pdu_fs_swap : 1;
	uint32 Iir_coef_swap : 1;

	uint32 Reserved : 4;

};



/*

FIR ch0 config0��0x104- FIR ch0 config0���ƼĴ�����
λ	Name	Type	����
29:21	fir_sample_num_ch0	r/w	��ǰfir��������������ٸ���ʵ�����óɼ�1��������������128���㣬���ó�127
20:12	fir_sample_start_ch0	r/w	In stream mode, used as initial phase In fir mode, used as fir base addr ��һ��fir��sample memory�����ĸ��㿪ʼ����
11:3	fir_order_ch0	r/w	Fir ϵ�����������511
2:1	fir_mode_ch0	r/w	00��general fir;  01:  10:  11�� not used
0	stream0_fir1_ch0	r/w	0��Stream������anc��adc������ 1��fir����

*/

struct _fir_settings
{
	uint32 stream0_fir1 : 1;
	uint32 fir_mode : 2;
	uint32 fir_order : 9;
	uint32 fir_sample_start : 9;
	uint32 fir_sample_num : 9;
	uint32 Reserved : 2;
};


/*
REG32B_08    ��offset��0x0094�� BIT	NAME	DEFINITION	DEFAULT
<31:29>	Pdm_mux_ch7	Ch7 mux from ch0~ch7	3'h7
<28:26>	Pdm_mux_ch6	Ch6 mux from ch0~ch7	3'h6
<25:23>	Pdm_mux_ch5	Ch5 mux from ch0~ch7	3'h5
<22:20>	Pdm_mux_ch4	Ch4 mux from ch0~ch7	3'h4
<19:17>	Pdm_mux_ch3	Ch3 mux from ch0~ch7	3'h3
<16:14>	Pdm_mux_ch2	Ch2 mux from ch0~ch7	3'h2
<13:11>	Pdm_mux_ch1	Ch1 mux from ch0~ch7	3'h1
<10:8>	Pdm_mux_ch0	Ch0 mux from ch0~ch7	3'h0
<7>	Pdu_fs_swap_ch0		1'b0
<6>	Feedback_music_cancel_hd_ch1		1'b0
<5>	Feedback_music_cancel_hd_ch0	1: Music processed by Hardware	1'b0
<4>	Feedback_music_cancel_en_ch1	1: ch1 music cancel	1'b0
<3>	Feedback_music_cancel_en_ch0	1: ch0 music cancel	1'b0
<2>	Feedback_pdu	1: use pdu1 for ch0 feedback (one chip for single ear)	1'b0
<1>	Feedback_ch1	Usd pdm5	1'b0
<0>	Feedback_ch0	Usd pdm4	1'b0


*/


struct _feedback_settings
{
    uint32 Feedback_ch0 : 1;
    uint32 Feedback_ch1 : 1;

    uint32 Feedback_pdu : 1;

    uint32 Feedback_music_cancel_en_ch0 : 1;
    uint32 Feedback_music_cancel_en_ch1 : 1;

    uint32 Reserved : 27;
};

#ifdef ANC_FB_CHECK
/*
REG32B_51     ��offset��0x01a8��
BIT	NAME	DEFINITION	DEFAULT

<26:17>	Fb_check_trig_window_ch0		10'h100
<16:5>	Fb_check_acc_window_ch0		12'h100
<4>	Fb_check_keep_sel_ch0	0: keep fb mic data 1: keep music cancel data 	1'b0
<3>	Fb_check_src_sel_ch0	0: use music cancel data 1: use fb mic data	1'b0
<2:1>	Fb_check_acc_sample_rate_ch0	00: 384k 01: 192k 10: 96k 11: 48k	2'b0
<0>	Fb_check_enable_ch0		1'b0
*/

struct _fb_check_ch0_config
{
	uint32 Fb_check_enable_ch0 : 1;

	uint32 Fb_check_acc_sample_rate_ch0 : 2;

	uint32 Fb_check_src_sel_ch0 : 1;

    uint32 Fb_check_keep_sel_ch0: 1;

	uint32 Fb_check_acc_window_ch0 : 12;

   	uint32 Fb_check_trig_window_ch0 : 10;

    uint32 Reserved : 5;
};

/*
REG32B_52     ��offset��0x01ac��
BIT	NAME	DEFINITION	DEFAULT

<26:17>	Fb_check_trig_window_ch1		10'h100
<16:5>	Fb_check_acc_window_ch1		12'h100
<4>	Fb_check_keep_sel_ch1	0: keep fb mic data 1: keep music cancel data 	1'b0
<3>	Fb_check_src_sel_ch1	0: use music cancel data 1: use fb mic data	1'b0
<2:1>	Fb_check_acc_sample_rate_ch1	00: 384k 01: 192k 10: 96k 11: 48k	2'b0
<0>	Fb_check_enable_ch1		1'b0
*/

struct _fb_check_ch1_config
{
	uint32 Fb_check_enable_ch1 : 1;

	uint32 Fb_check_acc_sample_rate_ch1 : 2;

	uint32 Fb_check_src_sel_ch1 : 1;

    uint32 Fb_check_keep_sel_ch1: 1;

	uint32 Fb_check_acc_window_ch1 : 12;

   	uint32 Fb_check_trig_window_ch1 : 10;

    uint32 Reserved : 5;
};

/*
ISR��0x10-�ж�״̬�Ĵ�����
λ	Name	Type	����
31:29	Rev
28	Feedback_error_trig_ch1
27	Feedback_error_trig_ch0		1: feedback error trigger; д1������ж�
26:25	reserved		Reserved for interrupt to mcu & bt
24	mc_underflow	r/w	1=mc fifo underflow; д1������жϣ�
23	mc_overflow	r/w	1=mc fifo overflow; д1������жϣ�
22	Dsd_tx_underflow	r/w	1=dsd txfifo underflow; д1������жϣ�
21	Dsd_tx_overflow	r/w	1=dsd txfifo overflow; д1������жϣ�
20	Dsd_rx_underflow	r/w	1=dsd rxfifo underflow; д1������жϣ�
19	Dsd_rx_overflow	r/w	1=dsd rxfifo overflow; д1������жϣ�
18	Tsf_trigger	r/w	1=tsf trigger; д1������жϣ�
17	tx_underflow	r/w	1=txfifo underflow; д1������жϣ�
16	tx_overflow	r/w	1=txfifo overflow; д1������жϣ�
15:8	rx_underflow	r/w	1=rxfifo underflow; д1������жϣ�
7:0	rx_overflow	r/w	1=rxfifo overflow; д1������жϣ�

*/
struct _codec_int_config
{
	uint32 Reserved_notused : 27;

	uint32 Feedback_error_trig_ch0 : 1;
	uint32 Feedback_error_trig_ch1 : 1;

    uint32 Reserved : 3;
};
/*
IMR��0x14-�ж����μĴ�����
λ	Name	Type	����
31:29	Rev	r
28	Feedback_error_mask_ch1
27	Feedback_error_mask_ch0
26	Bt_int_mask	r/w
25	Mcu_int_mask	r/w
24	mc_underflow_mask	r/w	Mc fifo undererflow mask
23	mc_overflow_mask	r/w	Mc fifo overflow mask
22	Dsd_tx_underflow_mask	r/w	Dsd Txfifo undererflow mask
21	Dsd_tx_overflow_mask	r/w	Dsd Txfifo overflow mask
20	Dsd_rx_underflow_mask	r/w	Dsd Rxfifo undererflow mask
19	Dsd_rx_overflow_mask	r/w	Dsd Rxfifo overflow mask
18	Tsf_trigger_mask	r/w	Tsf trigger mask
17	tx_underflow_mask	r/w	Txfifo undererflow mask
16	tx_overflow_mask	r/w	Txfifo overflow mask
15:8	rx_underflow_mask	r/w	Rxfifo undererflow mask
7:0	rx_overflow_mask	r/w	Rxfifo overflow mask
*/
struct _codec_mask_config
{
	uint32 Reserved_notused : 25;

	uint32 Codec_to_Mcu_mask : 1;
	uint32 Reserved_notused_0 : 1;

	uint32 Feedback_error_mask_ch0 : 1;
	uint32 Feedback_error_mask_ch1 : 1;

    uint32 Reserved : 3;
};



#endif

#define IIR_COUNTER (6)
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
volatile static struct _anc_control *anc_control=(volatile struct _anc_control *)(ANC_BASE+0x90);
volatile static struct _anc_ff_gain *anc_ff_gain=(volatile struct _anc_ff_gain *)(ANC_BASE+0xAC);
volatile static struct _anc_fb_gain *anc_fb_gain=(volatile struct _anc_fb_gain *)(ANC_BASE+0xB0);
//volatile struct _anc_adc_sel_ana_ch *anc_adc_sel_ana_ch=(volatile struct _anc_adc_sel_ana_ch *)(ANC_BASE+0xB8);


//IIR registers
volatile static struct _anc_iir_control *anc_iir_control=(volatile struct _anc_iir_control *)(ANC_BASE+0xA8);

volatile static struct _anc_iir_coefs *anc_ff_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x4000);
volatile static struct _anc_iir_coefs *anc_ff_iir_coefs0_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x4078);
volatile static struct _anc_iir_coefs *anc_fb_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x40f0);
volatile static struct _anc_iir_coefs *anc_fb_iir_coefs0_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x4168);


volatile static struct _anc_iir_coefs *anc_ff_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x4200);
volatile static struct _anc_iir_coefs *anc_ff_iir_coefs1_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x4278);
volatile static struct _anc_iir_coefs *anc_fb_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x42f0);
volatile static struct _anc_iir_coefs *anc_fb_iir_coefs1_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x4368);


//FIR registers
volatile static struct _anc_fir_bypass_anc_mute *anc_fir_bypass_anc_mute=(volatile struct _anc_fir_bypass_anc_mute *)(ANC_BASE+0x98);
volatile static struct _fir_control *fir_control=(volatile struct _fir_control *)(ANC_BASE+0x100);

#if (AUD_SECTION_STRUCT_VERSION == 1)
volatile static int32 *anc_ff_fir_coefs_l=(volatile  int32 *)(ANC_BASE+0x9000);
volatile static int32 *anc_ff_fir_coefs_r=(volatile  int32 *)(ANC_BASE+0xb000);
volatile static int32 *anc_fb_fir_coefs_l=(volatile  int32 *)(ANC_BASE+0xd000);
volatile static int32 *anc_fb_fir_coefs_r=(volatile  int32 *)(ANC_BASE+0xf000);

volatile static struct _fir_settings *fir_ff_settings_l=(volatile struct _fir_settings *)(ANC_BASE+0x104);
volatile static struct _fir_settings *fir_ff_settings_r=(volatile struct _fir_settings *)(ANC_BASE+0x114);
volatile static struct _fir_settings *fir_fb_settings_l=(volatile struct _fir_settings *)(ANC_BASE+0x124);
volatile static struct _fir_settings *fir_fb_settings_r=(volatile struct _fir_settings *)(ANC_BASE+0x134);
#endif

//feedback registers
volatile static struct _feedback_settings *feedback_settings=(volatile struct _feedback_settings *)(ANC_BASE+0x94);



#ifdef ANC_FB_CHECK
volatile static struct _fb_check_ch0_config *fb_check_ch0_config=(volatile struct _fb_check_ch0_config *)(ANC_BASE+0x01a8);
volatile static struct _fb_check_ch1_config *fb_check_ch1_config=(volatile struct _fb_check_ch1_config *)(ANC_BASE+0x01ac);

volatile static uint32 *Fb_check_threshold_ch0=(volatile uint32 *)(ANC_BASE+0x01b0);
volatile static uint32 *Fb_check_threshold_ch1=(volatile uint32 *)(ANC_BASE+0x01b4);

volatile static uint32 *codec_int_config_clr=(volatile uint32 *)(ANC_BASE+0x10);
volatile static struct _codec_mask_config *codec_mask_config=(volatile struct _codec_mask_config *)(ANC_BASE+0x14);

#define CODEC_FB_CHECK_ERROR_TRIG_CH0            (1 << 27)
#define CODEC_FB_CHECK_ERROR_TRIG_CH1            (1 << 28)
#endif


/*
volatile struct _anc_iir_coefs *anc_ff_iir_coefs0_l1=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x4000);
volatile struct _anc_iir_coefs *anc_ff_iir_coefs0_l2=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x4014);
volatile struct _anc_iir_coefs *anc_ff_iir_coefs0_l3=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x4028);
volatile struct _anc_iir_coefs *anc_ff_iir_coefs0_l4=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x403c);
volatile struct _anc_iir_coefs *anc_ff_iir_coefs0_l5=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x4050);
volatile struct _anc_iir_coefs *anc_ff_iir_coefs0_l6=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x4064);

volatile struct _anc_iir_coefs *anc_ff_iir_coefs_r1=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x4078);
volatile struct _anc_iir_coefs *anc_ff_iir_coefs_r2=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x408c);
volatile struct _anc_iir_coefs *anc_ff_iir_coefs_r3=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x40a0);
volatile struct _anc_iir_coefs *anc_ff_iir_coefs_r4=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x40b4);
volatile struct _anc_iir_coefs *anc_ff_iir_coefs_r5=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x40c8);
volatile struct _anc_iir_coefs *anc_ff_iir_coefs_r6=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x40dc);


volatile struct _anc_iir_coefs *anc_fb_iir_coefs0_l1=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x40f0);
volatile struct _anc_iir_coefs *anc_fb_iir_coefs0_l2=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x4104);
volatile struct _anc_iir_coefs *anc_fb_iir_coefs0_l3=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x4118);
volatile struct _anc_iir_coefs *anc_fb_iir_coefs0_l4=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x412c);
volatile struct _anc_iir_coefs *anc_fb_iir_coefs0_l5=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x4140);
volatile struct _anc_iir_coefs *anc_fb_iir_coefs0_l6=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x4154);

volatile struct _anc_iir_coefs *anc_fb_iir_coefs_r1=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x4168);
volatile struct _anc_iir_coefs *anc_fb_iir_coefs_r2=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x417c);
volatile struct _anc_iir_coefs *anc_fb_iir_coefs_r3=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x4190);
volatile struct _anc_iir_coefs *anc_fb_iir_coefs_r4=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x41a4);
volatile struct _anc_iir_coefs *anc_fb_iir_coefs_r5=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x41b8);
volatile struct _anc_iir_coefs *anc_fb_iir_coefs_r6=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x41cc);

*/

volatile static int max_ff_gain_l=0;
volatile static int max_ff_gain_r=0;

volatile static int max_fb_gain_l=0;
volatile static int max_fb_gain_r=0;

volatile static int ff_open_flag=0;
volatile static int fb_open_flag=0;

#ifdef ANC_FB_CHECK
static HWTIMER_ID fb_adc_check_dev_timer=NULL;
#endif

static iir_parameter ff_filtes_l_old;
static iir_parameter ff_filtes_r_old;
static iir_parameter fb_filtes_l_old;
static iir_parameter fb_filtes_r_old;

volatile static int8_t ff_iir_reset_flag;
volatile static int8_t fb_iir_reset_flag;
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

    LOG_I("%s: iir_coef_using:%d,Iir_coef_swap:%d", __func__,iir_coef_using,fir_control->Iir_coef_swap);

    ff_filtes_l = &(cfg->anc_cfg_ff_l);
    ff_filtes_r = &(cfg->anc_cfg_ff_r);
    fb_filtes_l = &(cfg->anc_cfg_fb_l);
    fb_filtes_r = &(cfg->anc_cfg_fb_r);

    //LOG_I("ff_l:%d,ff_r:%d,fb_l:%d,fb_r:%d",ff_filtes_l->total_gain,ff_filtes_r->total_gain,fb_filtes_l->total_gain,fb_filtes_r->total_gain);


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


                for(i=0;i<IIR_COUNTER;i++)
                {
                    anc_fb_iir_coefs1_l[i].a1=-fb_filtes_l_old.iir_coef[i].coef_a[1];
                    anc_fb_iir_coefs1_l[i].a2=-fb_filtes_l_old.iir_coef[i].coef_a[2];
                    anc_fb_iir_coefs1_l[i].b0=fb_filtes_l_old.iir_coef[i].coef_b[0];
                    anc_fb_iir_coefs1_l[i].b1=fb_filtes_l_old.iir_coef[i].coef_b[1];
                    anc_fb_iir_coefs1_l[i].b2=fb_filtes_l_old.iir_coef[i].coef_b[2];

                    anc_fb_iir_coefs1_r[i].a1=-fb_filtes_r_old.iir_coef[i].coef_a[1];
                    anc_fb_iir_coefs1_r[i].a2=-fb_filtes_r_old.iir_coef[i].coef_a[2];
                    anc_fb_iir_coefs1_r[i].b0=fb_filtes_r_old.iir_coef[i].coef_b[0];
                    anc_fb_iir_coefs1_r[i].b1=fb_filtes_r_old.iir_coef[i].coef_b[1];
                    anc_fb_iir_coefs1_r[i].b2=fb_filtes_r_old.iir_coef[i].coef_b[2];
                }

                anc_fb_iir_coefs1_l[0].b0=(int32)((fb_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)fb_filtes_l_old.total_gain)>>GAIN_Q);
                anc_fb_iir_coefs1_l[0].b1=(int32)((fb_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)fb_filtes_l_old.total_gain)>>GAIN_Q);
                anc_fb_iir_coefs1_l[0].b2=(int32)((fb_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)fb_filtes_l_old.total_gain)>>GAIN_Q);

                anc_fb_iir_coefs1_r[0].b0=(int32)((fb_filtes_r_old.iir_coef[0].coef_b[0]*(int64_t)fb_filtes_r_old.total_gain)>>GAIN_Q);
                anc_fb_iir_coefs1_r[0].b1=(int32)((fb_filtes_r_old.iir_coef[0].coef_b[1]*(int64_t)fb_filtes_r_old.total_gain)>>GAIN_Q);
                anc_fb_iir_coefs1_r[0].b2=(int32)((fb_filtes_r_old.iir_coef[0].coef_b[2]*(int64_t)fb_filtes_r_old.total_gain)>>GAIN_Q);

                fir_control->Iir_coef_swap=1;
                while(1)
                {
                    if(fir_control->Iir_coef_swap==1) break;
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

                for(i=0;i<IIR_COUNTER;i++)
                {
                    anc_fb_iir_coefs0_l[i].a1=-fb_filtes_l_old.iir_coef[i].coef_a[1];
                    anc_fb_iir_coefs0_l[i].a2=-fb_filtes_l_old.iir_coef[i].coef_a[2];
                    anc_fb_iir_coefs0_l[i].b0=fb_filtes_l_old.iir_coef[i].coef_b[0];
                    anc_fb_iir_coefs0_l[i].b1=fb_filtes_l_old.iir_coef[i].coef_b[1];
                    anc_fb_iir_coefs0_l[i].b2=fb_filtes_l_old.iir_coef[i].coef_b[2];

                    anc_fb_iir_coefs0_r[i].a1=-fb_filtes_r_old.iir_coef[i].coef_a[1];
                    anc_fb_iir_coefs0_r[i].a2=-fb_filtes_r_old.iir_coef[i].coef_a[2];
                    anc_fb_iir_coefs0_r[i].b0=fb_filtes_r_old.iir_coef[i].coef_b[0];
                    anc_fb_iir_coefs0_r[i].b1=fb_filtes_r_old.iir_coef[i].coef_b[1];
                    anc_fb_iir_coefs0_r[i].b2=fb_filtes_r_old.iir_coef[i].coef_b[2];
                }

                anc_fb_iir_coefs0_l[0].b0=(int32)((fb_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)fb_filtes_l_old.total_gain)>>GAIN_Q);
                anc_fb_iir_coefs0_l[0].b1=(int32)((fb_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)fb_filtes_l_old.total_gain)>>GAIN_Q);
                anc_fb_iir_coefs0_l[0].b2=(int32)((fb_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)fb_filtes_l_old.total_gain)>>GAIN_Q);

                anc_fb_iir_coefs0_r[0].b0=(int32)((fb_filtes_r_old.iir_coef[0].coef_b[0]*(int64_t)fb_filtes_r_old.total_gain)>>GAIN_Q);
                anc_fb_iir_coefs0_r[0].b1=(int32)((fb_filtes_r_old.iir_coef[0].coef_b[1]*(int64_t)fb_filtes_r_old.total_gain)>>GAIN_Q);
                anc_fb_iir_coefs0_r[0].b2=(int32)((fb_filtes_r_old.iir_coef[0].coef_b[2]*(int64_t)fb_filtes_r_old.total_gain)>>GAIN_Q);

                fir_control->Iir_coef_swap=0;
                while(1)
                {
                    if(fir_control->Iir_coef_swap==0) break;
                }

            }
        	//anc_ff_gain->Mute_gain_coef_pdu_ch0=ff_filtes_l->total_gain;
        	//anc_ff_gain->Mute_gain_coef_pdu_ch1=ff_filtes_r->total_gain;

           // anc_ff_gain->Mute_gain_coef_pdu_ch0=max_ff_gain_l;
          //  anc_ff_gain->Mute_gain_coef_pdu_ch1=max_ff_gain_r;

            ff_iir_reset_flag=0;
            iir_coef_using=1-iir_coef_using;
        }
        else
        {
            LOG_I("%s: no switching ff", __func__);

            //disable all IIR filter before updata the coefs;
            anc_iir_control->iir_enable=0;

            ff_iir_reset_flag=1;
        }
    }


    if(anc_type==ANC_FEEDBACK)
    {
        if(fb_filtes_l_old.iir_counter==fb_filtes_l->iir_counter
            &&fb_filtes_l_old.iir_bypass_flag==fb_filtes_l->iir_bypass_flag
            &&ff_filtes_l->iir_bypass_flag==0

            &&fb_filtes_r_old.iir_counter==fb_filtes_r->iir_counter
            &&fb_filtes_r_old.iir_bypass_flag==fb_filtes_r->iir_bypass_flag
            &&fb_filtes_r->iir_bypass_flag==0

            &&anc_gain_delay==ANC_GAIN_NO_DELAY)
        {
            LOG_I("%s: switching fb", __func__);

            if(iir_coef_using==0)
            {
                for(i=0;i<IIR_COUNTER;i++)
                {
                    anc_fb_iir_coefs1_l[i].a1=-fb_filtes_l->iir_coef[i].coef_a[1];
                    anc_fb_iir_coefs1_l[i].a2=-fb_filtes_l->iir_coef[i].coef_a[2];
                    anc_fb_iir_coefs1_l[i].b0=fb_filtes_l->iir_coef[i].coef_b[0];
                    anc_fb_iir_coefs1_l[i].b1=fb_filtes_l->iir_coef[i].coef_b[1];
                    anc_fb_iir_coefs1_l[i].b2=fb_filtes_l->iir_coef[i].coef_b[2];

                    anc_fb_iir_coefs1_r[i].a1=-fb_filtes_r->iir_coef[i].coef_a[1];
                    anc_fb_iir_coefs1_r[i].a2=-fb_filtes_r->iir_coef[i].coef_a[2];
                    anc_fb_iir_coefs1_r[i].b0=fb_filtes_r->iir_coef[i].coef_b[0];
                    anc_fb_iir_coefs1_r[i].b1=fb_filtes_r->iir_coef[i].coef_b[1];
                    anc_fb_iir_coefs1_r[i].b2=fb_filtes_r->iir_coef[i].coef_b[2];

                    fb_filtes_l_old.iir_coef[i]=fb_filtes_l->iir_coef[i];
                    fb_filtes_r_old.iir_coef[i]=fb_filtes_r->iir_coef[i];
                }

                fb_filtes_l_old.total_gain=fb_filtes_l->total_gain;
                fb_filtes_r_old.total_gain=fb_filtes_r->total_gain;

                anc_fb_iir_coefs1_l[0].b0=(int32)((fb_filtes_l->iir_coef[0].coef_b[0]*(int64_t)fb_filtes_l->total_gain)>>GAIN_Q);
                anc_fb_iir_coefs1_l[0].b1=(int32)((fb_filtes_l->iir_coef[0].coef_b[1]*(int64_t)fb_filtes_l->total_gain)>>GAIN_Q);
                anc_fb_iir_coefs1_l[0].b2=(int32)((fb_filtes_l->iir_coef[0].coef_b[2]*(int64_t)fb_filtes_l->total_gain)>>GAIN_Q);

                anc_fb_iir_coefs1_r[0].b0=(int32)((fb_filtes_r->iir_coef[0].coef_b[0]*(int64_t)fb_filtes_r->total_gain)>>GAIN_Q);
                anc_fb_iir_coefs1_r[0].b1=(int32)((fb_filtes_r->iir_coef[0].coef_b[1]*(int64_t)fb_filtes_r->total_gain)>>GAIN_Q);
                anc_fb_iir_coefs1_r[0].b2=(int32)((fb_filtes_r->iir_coef[0].coef_b[2]*(int64_t)fb_filtes_r->total_gain)>>GAIN_Q);


                for(i=0;i<IIR_COUNTER;i++)
                {
                    anc_ff_iir_coefs1_l[i].a1=-ff_filtes_l_old.iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs1_l[i].a2=-ff_filtes_l_old.iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs1_l[i].b0=ff_filtes_l_old.iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs1_l[i].b1=ff_filtes_l_old.iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs1_l[i].b2=ff_filtes_l_old.iir_coef[i].coef_b[2];

                    anc_ff_iir_coefs1_r[i].a1=-ff_filtes_r_old.iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs1_r[i].a2=-ff_filtes_r_old.iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs1_r[i].b0=ff_filtes_r_old.iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs1_r[i].b1=ff_filtes_r_old.iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs1_r[i].b2=ff_filtes_r_old.iir_coef[i].coef_b[2];
                }

                anc_ff_iir_coefs1_l[0].b0=(int32)((ff_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)ff_filtes_l_old.total_gain)>>GAIN_Q);
                anc_ff_iir_coefs1_l[0].b1=(int32)((ff_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)ff_filtes_l_old.total_gain)>>GAIN_Q);
                anc_ff_iir_coefs1_l[0].b2=(int32)((ff_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)ff_filtes_l_old.total_gain)>>GAIN_Q);

                anc_ff_iir_coefs1_r[0].b0=(int32)((ff_filtes_r_old.iir_coef[0].coef_b[0]*(int64_t)ff_filtes_r_old.total_gain)>>GAIN_Q);
                anc_ff_iir_coefs1_r[0].b1=(int32)((ff_filtes_r_old.iir_coef[0].coef_b[1]*(int64_t)ff_filtes_r_old.total_gain)>>GAIN_Q);
                anc_ff_iir_coefs1_r[0].b2=(int32)((ff_filtes_r_old.iir_coef[0].coef_b[2]*(int64_t)ff_filtes_r_old.total_gain)>>GAIN_Q);

                fir_control->Iir_coef_swap=1;
                while(1)
                {
                    if(fir_control->Iir_coef_swap==1) break;
                }
            }
            else
            {
                for(i=0;i<IIR_COUNTER;i++)
                {
                    anc_fb_iir_coefs0_l[i].a1=-fb_filtes_l->iir_coef[i].coef_a[1];
                    anc_fb_iir_coefs0_l[i].a2=-fb_filtes_l->iir_coef[i].coef_a[2];
                    anc_fb_iir_coefs0_l[i].b0=fb_filtes_l->iir_coef[i].coef_b[0];
                    anc_fb_iir_coefs0_l[i].b1=fb_filtes_l->iir_coef[i].coef_b[1];
                    anc_fb_iir_coefs0_l[i].b2=fb_filtes_l->iir_coef[i].coef_b[2];

                    anc_fb_iir_coefs0_r[i].a1=-fb_filtes_r->iir_coef[i].coef_a[1];
                    anc_fb_iir_coefs0_r[i].a2=-fb_filtes_r->iir_coef[i].coef_a[2];
                    anc_fb_iir_coefs0_r[i].b0=fb_filtes_r->iir_coef[i].coef_b[0];
                    anc_fb_iir_coefs0_r[i].b1=fb_filtes_r->iir_coef[i].coef_b[1];
                    anc_fb_iir_coefs0_r[i].b2=fb_filtes_r->iir_coef[i].coef_b[2];

                    fb_filtes_l_old.iir_coef[i]=fb_filtes_l->iir_coef[i];
                    fb_filtes_r_old.iir_coef[i]=fb_filtes_r->iir_coef[i];
                }

                fb_filtes_l_old.total_gain=fb_filtes_l->total_gain;
                fb_filtes_r_old.total_gain=fb_filtes_r->total_gain;

                anc_fb_iir_coefs0_l[0].b0=(int32)((fb_filtes_l->iir_coef[0].coef_b[0]*(int64_t)fb_filtes_l->total_gain)>>GAIN_Q);
                anc_fb_iir_coefs0_l[0].b1=(int32)((fb_filtes_l->iir_coef[0].coef_b[1]*(int64_t)fb_filtes_l->total_gain)>>GAIN_Q);
                anc_fb_iir_coefs0_l[0].b2=(int32)((fb_filtes_l->iir_coef[0].coef_b[2]*(int64_t)fb_filtes_l->total_gain)>>GAIN_Q);

                anc_fb_iir_coefs0_r[0].b0=(int32)((fb_filtes_r->iir_coef[0].coef_b[0]*(int64_t)fb_filtes_r->total_gain)>>GAIN_Q);
                anc_fb_iir_coefs0_r[0].b1=(int32)((fb_filtes_r->iir_coef[0].coef_b[1]*(int64_t)fb_filtes_r->total_gain)>>GAIN_Q);
                anc_fb_iir_coefs0_r[0].b2=(int32)((fb_filtes_r->iir_coef[0].coef_b[2]*(int64_t)fb_filtes_r->total_gain)>>GAIN_Q);


                for(i=0;i<IIR_COUNTER;i++)
                {
                    anc_ff_iir_coefs0_l[i].a1=-ff_filtes_l_old.iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs0_l[i].a2=-ff_filtes_l_old.iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs0_l[i].b0=ff_filtes_l_old.iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs0_l[i].b1=ff_filtes_l_old.iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs0_l[i].b2=ff_filtes_l_old.iir_coef[i].coef_b[2];

                    anc_ff_iir_coefs0_r[i].a1=-ff_filtes_r_old.iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs0_r[i].a2=-ff_filtes_r_old.iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs0_r[i].b0=ff_filtes_r_old.iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs0_r[i].b1=ff_filtes_r_old.iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs0_r[i].b2=ff_filtes_r_old.iir_coef[i].coef_b[2];
                }

                anc_ff_iir_coefs0_l[0].b0=(int32)((ff_filtes_l_old.iir_coef[0].coef_b[0]*(int64_t)ff_filtes_l_old.total_gain)>>GAIN_Q);
                anc_ff_iir_coefs0_l[0].b1=(int32)((ff_filtes_l_old.iir_coef[0].coef_b[1]*(int64_t)ff_filtes_l_old.total_gain)>>GAIN_Q);
                anc_ff_iir_coefs0_l[0].b2=(int32)((ff_filtes_l_old.iir_coef[0].coef_b[2]*(int64_t)ff_filtes_l_old.total_gain)>>GAIN_Q);

                anc_ff_iir_coefs0_r[0].b0=(int32)((ff_filtes_r_old.iir_coef[0].coef_b[0]*(int64_t)ff_filtes_r_old.total_gain)>>GAIN_Q);
                anc_ff_iir_coefs0_r[0].b1=(int32)((ff_filtes_r_old.iir_coef[0].coef_b[1]*(int64_t)ff_filtes_r_old.total_gain)>>GAIN_Q);
                anc_ff_iir_coefs0_r[0].b2=(int32)((ff_filtes_r_old.iir_coef[0].coef_b[2]*(int64_t)ff_filtes_r_old.total_gain)>>GAIN_Q);

                fir_control->Iir_coef_swap=0;
                while(1)
                {
                    if(fir_control->Iir_coef_swap==0) break;
                }
            }

         //   anc_fb_gain->Mute_gain_coef_fs_ch0=fb_filtes_l->total_gain;
        //    anc_fb_gain->Mute_gain_coef_fs_ch1=fb_filtes_r->total_gain;

        //    anc_fb_gain->Mute_gain_coef_fs_ch0=max_fb_gain_l;
        //    anc_fb_gain->Mute_gain_coef_fs_ch1=max_fb_gain_r;

            fb_iir_reset_flag=0;
            iir_coef_using=1-iir_coef_using;
        }
        else
        {
            LOG_I("%s: no switching fb", __func__);

            //disable all IIR filter before updata the coefs;
            anc_iir_control->iir_enable=0;

            fb_iir_reset_flag=1;
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
        anc_ff_gain->Mute_gain_coef_pdu_ch0=0;
        anc_ff_gain->Mute_gain_coef_pdu_ch1=0;

        //setting the feedforward IIR filters;
        if(ff_filtes_l->iir_bypass_flag==0)
        {
            anc_iir_control->iir_ch0_bypass=0;

            if(ff_filtes_l->iir_counter>IIR_COUNTER)
            {
                anc_iir_control->iir_count_ch0=IIR_COUNTER;
            }
            else if(ff_filtes_l->iir_counter>0)
            {
                anc_iir_control->iir_count_ch0=ff_filtes_l->iir_counter;
            }
            else
            {
                anc_iir_control->iir_ch0_bypass=1;
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
                anc_ff_gain->Mute_gain_coef_pdu_ch0=max_ff_gain_l;
            }

        }
        else
        {
            anc_iir_control->iir_ch0_bypass=1;
            anc_ff_gain->Mute_gain_coef_pdu_ch0=ff_filtes_l->total_gain;
            max_ff_gain_l=ff_filtes_l->total_gain;

        }

        if(ff_filtes_r->iir_bypass_flag==0)
        {
            anc_iir_control->iir_ch1_bypass=0;

            if(ff_filtes_r->iir_counter>IIR_COUNTER)
            {
                anc_iir_control->iir_count_ch1=IIR_COUNTER;
            }
            else if(ff_filtes_r->iir_counter>0)
            {
                anc_iir_control->iir_count_ch1=ff_filtes_r->iir_counter;
            }
            else
            {
                anc_iir_control->iir_ch1_bypass=1;
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
                anc_ff_gain->Mute_gain_coef_pdu_ch1=max_ff_gain_r;
            }
        }
        else
        {
            anc_iir_control->iir_ch1_bypass=1;
            anc_ff_gain->Mute_gain_coef_pdu_ch1=ff_filtes_r->total_gain;
            max_ff_gain_r=ff_filtes_r->total_gain;
        }

#if (AUD_SECTION_STRUCT_VERSION == 1)
        //setting the feedback FIR filters;
        fir_control->Fir0_256p0_512p1=1;
        fir_control->Fir1_256p0_512p1=1;

        if(ff_filtes_l->fir_bypass_flag==0&&ff_filtes_l->fir_len==FIR_LEN)
        {
            fir_ff_settings_l->stream0_fir1=0;

            anc_fir_bypass_anc_mute->Anc_pdu0_to_fir_bypass=0;
            fir_control->Stream_enable_ch0=1;


            if(ff_filtes_l->fir_len>FIR_LEN)
            {
                fir_ff_settings_l->fir_order=FIR_LEN;
            }
            else if(ff_filtes_l->fir_len>0)
            {
                fir_ff_settings_l->fir_order=ff_filtes_l->fir_len;
            }
            else
            {
                anc_fir_bypass_anc_mute->Anc_pdu0_to_fir_bypass=1;
                fir_ff_settings_l->fir_order=0;
                LOG_I("Error ff_filtes_l FIR LEN:%d", ff_filtes_l->fir_len);
                err=ANC_TYPE_ERR;
            }

            //feedforward left ch FIR coefs settings
            for(i=0;i<fir_ff_settings_l->fir_order;i++)
            {
                anc_ff_fir_coefs_l[i]=(int32)ff_filtes_l->fir_coef[i]*16;
            }

        }
        else
        {
            anc_fir_bypass_anc_mute->Anc_pdu0_to_fir_bypass=1;
            fir_control->Stream_enable_ch0=0;
        }

        if(ff_filtes_r->fir_bypass_flag==0&&ff_filtes_r->fir_len==FIR_LEN)
        {
            fir_ff_settings_r->stream0_fir1=0;

            anc_fir_bypass_anc_mute->Anc_pdu1_to_fir_bypass=0;
            fir_control->Stream_enable_ch2=1;

            if(ff_filtes_r->fir_len>FIR_LEN)
            {
                fir_ff_settings_r->fir_order=FIR_LEN;
            }
            else if(ff_filtes_r->fir_len>0)
            {
                fir_ff_settings_r->fir_order=ff_filtes_r->fir_len;
            }
            else
            {
                anc_fir_bypass_anc_mute->Anc_pdu1_to_fir_bypass=1;
                fir_ff_settings_r->fir_order=0;
                LOG_I("Error ff_filtes_l FIR LEN:%d", ff_filtes_r->fir_len);
                err=ANC_TYPE_ERR;
            }

            //feedforward left ch FIR coefs settings
            for(i=0;i<fir_ff_settings_r->fir_order;i++)
            {
                anc_ff_fir_coefs_r[i]=(int32)ff_filtes_r->fir_coef[i]*16;
            }

        }
        else
        {
            anc_fir_bypass_anc_mute->Anc_pdu1_to_fir_bypass=1;
            fir_control->Stream_enable_ch2=0;
        }
#endif


        total_iir_counter=0;
        if(ff_open_flag==1)
        {
               total_iir_counter=total_iir_counter+anc_iir_control->iir_count_ch0+anc_iir_control->iir_count_ch1;
        }
        if(fb_open_flag==1)
        {
               total_iir_counter=total_iir_counter+anc_iir_control->iir_count_ch2+anc_iir_control->iir_count_ch3;
        }

        hal_cmu_codec_iir_disable();

        if(total_iir_counter<=8)
        {
            hal_cmu_codec_iir_enable(39000000);
        }
        else if(total_iir_counter<=12)
        {
            hal_cmu_codec_iir_enable(52000000);
        }
        else
        {
            hal_cmu_codec_iir_enable(65000000);

#ifdef __AUDIO_RESAMPLE__
        ASSERT(total_iir_counter<=16, "Total number of IIR must <=16  in audio resample mode!And now:%d",total_iir_counter);
#else
        ASSERT(total_iir_counter<=22, "Total number of IIR must <=22  in pll mode!And now:%d",total_iir_counter);
#endif
        }

        //enable all IIR filters
        anc_iir_control->iir_enable=1;
    }



    //feedback ANC settings
    if(anc_type==ANC_FEEDBACK&&fb_iir_reset_flag==1)
    {
        LOG_I("%s: resetting fb", __func__);

        for(i=0;i<IIR_COUNTER;i++)
        {
            fb_filtes_l_old.iir_coef[i]=fb_filtes_l->iir_coef[i];
            fb_filtes_r_old.iir_coef[i]=fb_filtes_r->iir_coef[i];
         }

        fb_filtes_l_old.total_gain=fb_filtes_l->total_gain;
        fb_filtes_r_old.total_gain=fb_filtes_r->total_gain;

        fb_filtes_l_old.iir_counter=fb_filtes_l->iir_counter;
        fb_filtes_r_old.iir_counter=fb_filtes_r->iir_counter;

        fb_filtes_l_old.iir_bypass_flag=fb_filtes_l->iir_bypass_flag;
        fb_filtes_r_old.iir_bypass_flag=fb_filtes_r->iir_bypass_flag;

        max_fb_gain_l=(1<<GAIN_Q);
        max_fb_gain_r=(1<<GAIN_Q);

        //Set the FB gain;
        anc_fb_gain->Mute_gain_coef_fs_ch0=0;
        anc_fb_gain->Mute_gain_coef_fs_ch1=0;

        //setting the feedback IIR filters;
        if(fb_filtes_l->iir_bypass_flag==0)
        {
            anc_iir_control->iir_ch2_bypass=0;

            if(fb_filtes_l->iir_counter>IIR_COUNTER)
            {
                anc_iir_control->iir_count_ch2=IIR_COUNTER;
            }
            else if(fb_filtes_l->iir_counter>0)
            {
                anc_iir_control->iir_count_ch2=fb_filtes_l->iir_counter;
            }
            else
            {
                anc_iir_control->iir_ch2_bypass=1;
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
                anc_fb_gain->Mute_gain_coef_fs_ch0=max_fb_gain_l;
            }
        }
        else
        {
            anc_iir_control->iir_ch2_bypass=1;
            anc_fb_gain->Mute_gain_coef_fs_ch0=fb_filtes_l->total_gain;
            max_fb_gain_l=fb_filtes_l->total_gain;

        }


        if(fb_filtes_r->iir_bypass_flag==0)
        {
            anc_iir_control->iir_ch3_bypass=0;

            if(fb_filtes_r->iir_counter>IIR_COUNTER)
            {
                anc_iir_control->iir_count_ch3=IIR_COUNTER;
            }
            else if(fb_filtes_r->iir_counter>0)
            {
                anc_iir_control->iir_count_ch3=fb_filtes_r->iir_counter;
            }
            else
            {
                anc_iir_control->iir_ch3_bypass=1;
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
                anc_fb_gain->Mute_gain_coef_fs_ch1=max_fb_gain_r;
            }

        }
        else
        {
            anc_iir_control->iir_ch3_bypass=1;
            anc_fb_gain->Mute_gain_coef_fs_ch1=fb_filtes_r->total_gain;
            max_fb_gain_r=fb_filtes_r->total_gain;
        }


#if (AUD_SECTION_STRUCT_VERSION == 1)
        //setting the feedback FIR filters;

        fir_control->Fir2_256p0_512p1=1;
        fir_control->Fir3_256p0_512p1=1;

        if(fb_filtes_l->fir_bypass_flag==0&&fb_filtes_l->fir_len==FIR_LEN)
        {
            fir_fb_settings_l->stream0_fir1=0;

            anc_fir_bypass_anc_mute->Anc_fs0_to_fir_bypass=0;
            fir_control->Stream_enable_ch4=1;

            if(fb_filtes_l->fir_len>FIR_LEN)
            {
                fir_fb_settings_l->fir_order=FIR_LEN;
            }
            else if(fb_filtes_l->fir_len>0)
            {
                fir_fb_settings_l->fir_order=fb_filtes_l->fir_len;
            }
            else
            {
                anc_fir_bypass_anc_mute->Anc_fs0_to_fir_bypass=1;
                fir_fb_settings_l->fir_order=0;
                LOG_I("Error ff_filtes_l FIR LEN:%d", fb_filtes_l->fir_len);
                err=ANC_TYPE_ERR;
            }

            //feedforward left ch FIR coefs settings
            for(i=0;i<fir_fb_settings_l->fir_order;i++)
            {
                anc_fb_fir_coefs_l[i]=(int32)fb_filtes_l->fir_coef[i]*16;
            }

        }
        else
        {
            anc_fir_bypass_anc_mute->Anc_fs0_to_fir_bypass=1;
            fir_control->Stream_enable_ch4=0;
        }


        if(fb_filtes_r->fir_bypass_flag==0&&fb_filtes_r->fir_len==FIR_LEN)
        {
            fir_fb_settings_r->stream0_fir1=0;

            anc_fir_bypass_anc_mute->Anc_fs1_to_fir_bypass=0;
            fir_control->Stream_enable_ch6=1;

            if(fb_filtes_r->fir_len>FIR_LEN)
            {
                fir_fb_settings_r->fir_order=FIR_LEN;
            }
            else if(fb_filtes_r->fir_len>0)
            {
                fir_fb_settings_r->fir_order=fb_filtes_r->fir_len;
            }
            else
            {
                anc_fir_bypass_anc_mute->Anc_fs1_to_fir_bypass=1;
                fir_fb_settings_r->fir_order=0;
                LOG_I("Error ff_filtes_l FIR LEN:%d", fb_filtes_r->fir_len);
                err=ANC_TYPE_ERR;
            }
            //feedforward left ch FIR coefs settings
            for(i=0;i<fir_fb_settings_r->fir_order;i++)
            {
                anc_fb_fir_coefs_r[i]=(int32)fb_filtes_r->fir_coef[i]*16;
            }
        }
        else
        {
            anc_fir_bypass_anc_mute->Anc_fs1_to_fir_bypass=1;
            fir_control->Stream_enable_ch6=0;
        }
#endif

        total_iir_counter=0;
        if(ff_open_flag==1)
        {
               total_iir_counter=total_iir_counter+anc_iir_control->iir_count_ch0+anc_iir_control->iir_count_ch1;
        }
        if(fb_open_flag==1)
        {
               total_iir_counter=total_iir_counter+anc_iir_control->iir_count_ch2+anc_iir_control->iir_count_ch3;
        }

        hal_cmu_codec_iir_disable();

        if(total_iir_counter<=8)
        {
            hal_cmu_codec_iir_enable(39000000);
        }
        else if(total_iir_counter<=12)
        {
            hal_cmu_codec_iir_enable(52000000);
        }
        else
        {
            hal_cmu_codec_iir_enable(65000000);

#ifdef __AUDIO_RESAMPLE__
        ASSERT(total_iir_counter<=16, "Total number of IIR must <=16  in audio resample mode!And now:%d",total_iir_counter);
#else
        ASSERT(total_iir_counter<=22, "Total number of IIR must <=22  in pll mode!And now:%d",total_iir_counter);
#endif
        }

        //enable all IIR filters
        anc_iir_control->iir_enable=1;

    }

    anc_control->Anc_enable_ch0=1;
    anc_control->Anc_enable_ch1=1;

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
    anc_control->Anc_enable_ch0=0;
    anc_control->Anc_enable_ch1=0;

    //init swap to use coef0
    fir_control->Iir_coef_swap=0;

    //select FF analog ad for digital ad.
    //	anc_adc_sel_ana_ch->adc_in_sel_ch0=0;
    //	anc_adc_sel_ana_ch->adc_in_sel_ch1=1;

    //select FB analog ad for digital ad.
    //	anc_adc_sel_ana_ch->adc_in_sel_ch2=2;
    //	anc_adc_sel_ana_ch->adc_in_sel_ch3=3;

    //talk mic
    //	anc_adc_sel_ana_ch->adc_in_sel_ch4=4;


    feedback_settings->Feedback_ch0=0;
    feedback_settings->Feedback_ch1=0;

    anc_control->Dual_anc_ch0=0;
    anc_control->Dual_anc_ch1=0;

    //disable all IIR filters
    anc_iir_control->iir_enable=0;

    //use FF IIR filters
    anc_iir_control->iir_ch0_bypass=1;
    anc_iir_control->iir_ch1_bypass=1;

    //Bypass FB IIR filters
    anc_iir_control->iir_ch2_bypass=1;
    anc_iir_control->iir_ch3_bypass=1;

    //use all of IIR filters.The max number is 6.
    anc_iir_control->iir_count_ch0=0;
    anc_iir_control->iir_count_ch1=0;
    anc_iir_control->iir_count_ch2=0;
    anc_iir_control->iir_count_ch3=0;

    //unmute ANC.
    anc_fir_bypass_anc_mute->Anc_ch0_mute=0;
    anc_fir_bypass_anc_mute->Anc_ch1_mute=0;

    //seting FIR filters,Bypass FIr Filter.
    anc_fir_bypass_anc_mute->Anc_pdu0_to_fir_bypass=1;
    anc_fir_bypass_anc_mute->Anc_pdu1_to_fir_bypass=1;
    anc_fir_bypass_anc_mute->Anc_fs0_to_fir_bypass=1;
    anc_fir_bypass_anc_mute->Anc_fs1_to_fir_bypass=1;

    //disable FIR stream;
    fir_control->Stream_enable_ch0=0;
    fir_control->Stream_enable_ch1=0;
    fir_control->Stream_enable_ch2=0;
    fir_control->Stream_enable_ch3=0;
    fir_control->Stream_enable_ch4=0;
    fir_control->Stream_enable_ch5=0;
    fir_control->Stream_enable_ch6=0;
    fir_control->Stream_enable_ch7=0;

    //disable gain updated when pass0
    anc_control->Mute_gain_pass0_ch0=1;
    anc_control->Mute_gain_pass0_ch1=1;


}
void anc_disable_gain_updated_when_pass0(uint8_t on)
{
/*
    anc_control->Mute_gain_pass0_ch0=on;
    anc_control->Mute_gain_pass0_ch1=on;
    */
}

static void anc_ctrl_reg_open(enum ANC_TYPE_T anc_type)
{
    LOG_I("%s", __func__);

    if(anc_type==ANC_FEEDFORWARD)
    {
        //clear iir counter
        anc_iir_control->iir_count_ch0=0;
        anc_iir_control->iir_count_ch1=0;

        //set the FF gain;
        anc_ff_gain->Mute_gain_coef_pdu_ch0=0;
        anc_ff_gain->Mute_gain_coef_pdu_ch1=0;

        ff_filtes_l_old.total_gain=0;
        ff_filtes_l_old.iir_counter=0;
        ff_filtes_l_old.iir_bypass_flag=1;

        ff_filtes_r_old.total_gain=0;
        ff_filtes_r_old.iir_counter=0;
        ff_filtes_r_old.iir_bypass_flag=1;

        ff_iir_reset_flag=0;

    }

    if(anc_type==ANC_FEEDBACK)
    {
        //clear iir counter
        anc_iir_control->iir_count_ch2=0;
        anc_iir_control->iir_count_ch3=0;

        //set the FB gain;
        anc_fb_gain->Mute_gain_coef_fs_ch0=0;
        anc_fb_gain->Mute_gain_coef_fs_ch1=0;

#ifdef AUDIO_ANC_FB_MC
        feedback_settings->Feedback_music_cancel_en_ch0=1;
        feedback_settings->Feedback_music_cancel_en_ch1=1;
#endif

        fb_filtes_l_old.total_gain=0;
        fb_filtes_l_old.iir_counter=0;
        fb_filtes_l_old.iir_bypass_flag=1;

        fb_filtes_r_old.total_gain=0;
        fb_filtes_r_old.iir_counter=0;
        fb_filtes_r_old.iir_bypass_flag=1;

        fb_iir_reset_flag=0;
    }

    if(ff_open_flag==1&&fb_open_flag==1)
    {

        feedback_settings->Feedback_ch0=1;
        feedback_settings->Feedback_ch1=1;

        anc_control->Dual_anc_ch0=1;
        anc_control->Dual_anc_ch1=1;
    }
    else if(ff_open_flag==0&&fb_open_flag==1)
    {
        feedback_settings->Feedback_ch0=1;
        feedback_settings->Feedback_ch1=1;

        anc_control->Dual_anc_ch0=0;
        anc_control->Dual_anc_ch1=0;

    }
    else if(ff_open_flag==1&&fb_open_flag==0)
    {
        feedback_settings->Feedback_ch0=0;
        feedback_settings->Feedback_ch1=0;

        anc_control->Dual_anc_ch0=0;
        anc_control->Dual_anc_ch1=0;

    }

}


static void anc_ctrl_reg_close(enum ANC_TYPE_T anc_type)
{

    if(anc_type==ANC_FEEDFORWARD)
    {
        //clear iir counter
        anc_iir_control->iir_count_ch0=0;
        anc_iir_control->iir_count_ch1=0;

        //set the FF gain;
        anc_ff_gain->Mute_gain_coef_pdu_ch0=0;
        anc_ff_gain->Mute_gain_coef_pdu_ch1=0;
    }

    if(anc_type==ANC_FEEDBACK)
    {
        //clear iir counter
        anc_iir_control->iir_count_ch2=0;
        anc_iir_control->iir_count_ch3=0;

        //set the FB gain;
        anc_fb_gain->Mute_gain_coef_fs_ch0=0;
        anc_fb_gain->Mute_gain_coef_fs_ch1=0;

#ifdef AUDIO_ANC_FB_MC
        feedback_settings->Feedback_music_cancel_en_ch0=0;
        feedback_settings->Feedback_music_cancel_en_ch1=0;
#endif
    }

    if(ff_open_flag==1&&fb_open_flag==1)
    {
        feedback_settings->Feedback_ch0=1;
        feedback_settings->Feedback_ch1=1;

        anc_control->Dual_anc_ch0=1;
        anc_control->Dual_anc_ch1=1;
    }
    else if(ff_open_flag==0&&fb_open_flag==1)
    {
        feedback_settings->Feedback_ch0=1;
        feedback_settings->Feedback_ch1=1;

        anc_control->Dual_anc_ch0=0;
        anc_control->Dual_anc_ch1=0;

    }
    else if(ff_open_flag==1&&fb_open_flag==0)
    {
        feedback_settings->Feedback_ch0=0;
        feedback_settings->Feedback_ch1=0;

        anc_control->Dual_anc_ch0=0;
        anc_control->Dual_anc_ch1=0;
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

    if(codec_int_config->Feedback_error_trig_ch0)
    {
        fb_check_ch0_config->Fb_check_enable_ch0=0;

        hwtimer_stop(fb_adc_check_dev_timer);
        hwtimer_start(fb_adc_check_dev_timer, FB_CHECK_DELAY);

        if(fb_gain_l==max_fb_gain_l)
        {
            anc_set_gain(max_fb_gain_l/2, fb_gain_r,ANC_FEEDBACK);
        }

        LOG_I("ADC_left");

        fb_check_ch0_config->Fb_check_enable_ch0=1;
    }

    if(codec_int_config->Feedback_error_trig_ch1)
    {
        fb_check_ch1_config->Fb_check_enable_ch1=0;

        hwtimer_stop(fb_adc_check_dev_timer);
        hwtimer_start(fb_adc_check_dev_timer, FB_CHECK_DELAY);

        if(fb_gain_r==max_fb_gain_r)
        {
            anc_set_gain(fb_gain_l, max_fb_gain_r/2,ANC_FEEDBACK);
        }

        LOG_I("ADC_right");

        fb_check_ch1_config->Fb_check_enable_ch1=1;
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
        hal_cmu_codec_iir_enable(65000000);
        anc_ctrl_reg_init();

        iir_coef_using=0;
    }


    if(anc_type==ANC_FEEDFORWARD)
    {
        max_ff_gain_l=0;
        max_ff_gain_r=0;

        ff_open_flag=1;
    }

    if(anc_type==ANC_FEEDBACK)
    {
        max_fb_gain_l=0;
        max_fb_gain_r=0;

        fb_open_flag=1;

#ifdef ANC_FB_CHECK
        hal_codec_anc_fb_check_set_irq_handler(anc_fb_check_irq_handler);

        fb_check_ch0_config->Fb_check_enable_ch0=0;
        fb_check_ch1_config->Fb_check_enable_ch1=0;
        *codec_int_config_clr=CODEC_FB_CHECK_ERROR_TRIG_CH0;
        *codec_int_config_clr=CODEC_FB_CHECK_ERROR_TRIG_CH1;

        codec_mask_config->Feedback_error_mask_ch0=1;
        codec_mask_config->Feedback_error_mask_ch1=1;

        codec_mask_config->Codec_to_Mcu_mask=1;

        fb_check_ch0_config->Fb_check_acc_sample_rate_ch0=3;
        fb_check_ch0_config->Fb_check_src_sel_ch0=0;
        fb_check_ch0_config->Fb_check_acc_window_ch0=64;
        fb_check_ch0_config->Fb_check_trig_window_ch0=64;

        fb_check_ch1_config->Fb_check_acc_sample_rate_ch1=3;
        fb_check_ch1_config->Fb_check_src_sel_ch1=0;
        fb_check_ch1_config->Fb_check_acc_window_ch1=64;
        fb_check_ch1_config->Fb_check_trig_window_ch1=64;

        *Fb_check_threshold_ch0=0x400000;
        *Fb_check_threshold_ch1=0x400000;

        fb_check_ch0_config->Fb_check_enable_ch0=1;
        fb_check_ch1_config->Fb_check_enable_ch1=1;
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
    }

    if(anc_type==ANC_FEEDBACK)
    {
        max_fb_gain_l=0;
        max_fb_gain_r=0;

        fb_open_flag=0;

#ifdef ANC_FB_CHECK
        fb_check_ch0_config->Fb_check_enable_ch0=0;
        fb_check_ch1_config->Fb_check_enable_ch1=0;
        *codec_int_config_clr=CODEC_FB_CHECK_ERROR_TRIG_CH0;
        *codec_int_config_clr=CODEC_FB_CHECK_ERROR_TRIG_CH1;

        codec_mask_config->Feedback_error_mask_ch0=0;
        codec_mask_config->Feedback_error_mask_ch1=0;

        codec_mask_config->Codec_to_Mcu_mask=0;

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

    anc_control->Anc_enable_ch0=1;
    anc_control->Anc_enable_ch1=1;

#ifdef ANC_FB_CHECK
    fb_check_ch0_config->Fb_check_enable_ch0=1;
    fb_check_ch1_config->Fb_check_enable_ch1=1;
#endif

    anc_cfg_unlock();

    return ANC_NO_ERR;
}

int anc_disable(void)
{
    LOG_I("%s", __func__);

    anc_cfg_lock();

#ifdef ANC_FB_CHECK
    fb_check_ch0_config->Fb_check_enable_ch0=0;
    fb_check_ch1_config->Fb_check_enable_ch1=0;
    hwtimer_stop(fb_adc_check_dev_timer);
#endif

    anc_control->Anc_enable_ch0=0;
    anc_control->Anc_enable_ch1=0;

    anc_cfg_unlock();

    return ANC_NO_ERR;
}

int anc_set_gain(int32_t gain_ch_l, int32_t gain_ch_r,enum ANC_TYPE_T anc_type)
{
    ANC_ERROR err=ANC_NO_ERR;

    // LOG_I("anc_set_gain gain_ch_l:%d,gain_ch_r:%d",gain_ch_l,gain_ch_r);
    anc_cfg_lock();

    ASSERT(gain_ch_l<=(1<<GAIN_Q)&&gain_ch_l>=0, "Error left ch gain!");
    ASSERT(gain_ch_r<=(1<<GAIN_Q)&&gain_ch_r>=0, "Error right ch gain!");

    if(anc_type==ANC_FEEDFORWARD)
    {
        //Set the FF gain;
        anc_ff_gain->Mute_gain_coef_pdu_ch0=gain_ch_l;
        anc_ff_gain->Mute_gain_coef_pdu_ch1=gain_ch_r;
    }
    else if(anc_type==ANC_FEEDBACK)
    {
        //Set the FB gain;
        anc_fb_gain->Mute_gain_coef_fs_ch0=gain_ch_l;
        anc_fb_gain->Mute_gain_coef_fs_ch1=gain_ch_r;
    }
    else
    {
        //if err ANC type,retun 0;
        anc_fb_gain->Mute_gain_coef_fs_ch0=0;
        anc_fb_gain->Mute_gain_coef_fs_ch1=0;
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
        *gain_ch_l=anc_ff_gain->Mute_gain_coef_pdu_ch0;
        *gain_ch_r=anc_ff_gain->Mute_gain_coef_pdu_ch1;
    }
    else if(anc_type==ANC_FEEDBACK)
    {
        //Get the FB gain;
        *gain_ch_l=anc_fb_gain->Mute_gain_coef_fs_ch0;
        *gain_ch_r=anc_fb_gain->Mute_gain_coef_fs_ch1;
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
int anc_adc_data_select(ANC_ADC_DATA data_select)
{
    return 0;
}

