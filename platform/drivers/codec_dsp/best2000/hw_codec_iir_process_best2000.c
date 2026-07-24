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
REG32B_05    ��offset��0x0088��
<31:29>	dither_gain<2:0>	sdm dither gain	3'b0
<28:25>	s_adc_volume_ch0<3:0>	mic gain,-12dB(0001)~16dB(1111) 0000: mute	4'b0111
<24>	s_dac1k	1: sine 1khz test	1'b0
<23>	s_loop	1: loop test	1'b0
<22>	tports_codec_test_en		1'b0
<21:17>	tports_sel<4:0>		5'b00000
<16:12>	s_adc_dvst_ctl<4:0>	side tone gain, 0dB(00000)~-60dB(11110), step is 2dB, 11111: disable	5'b11111
<11:10>	sdm_gain<1:0>	11: 0dB, 10: -0.56dB, 01: -1.16dB, 00: -2.5dB 	2'b10
<9:8>	dither_gain<4:3>	sdm dither gain 	2'b01
<7:2>	Reserved		1'b0
<1>	Adc_ch1_iir_enable		1'b0
<0>	Adc_ch0_iir_enable		1'b0
*/
struct _hw_adc_iir_control
{
    uint32 Adc_ch0_iir_enable : 1;
    uint32 Adc_ch1_iir_enable : 1;	

    uint32 Reserved : 6;	

    uint32 dither_gain_4_3 : 2;	
    uint32 sdm_gain : 2;	
    uint32 s_adc_dvst_ctl : 5;	
    uint32 tports_sel : 5;

    uint32 tports_codec_test_en : 1;	
    uint32 s_loop : 1;	
    uint32 s_dac1k : 1;	
    uint32 s_adc_volume_ch0 : 4;
    uint32 dither_gain_2_0 : 3;	
};


/*
REG32B_06    ��offset��0x008c��
BIT	NAME	DEFINITION	DEFAULT
<31>	Adc_hbf1_bypass_ch7		1'b0
<30>	Adc_hbf2_bypass_ch7		
<29>	Adc_hbf3_bypass_ch7		
<28>	Adc_hbf1_bypass_ch6		1'b0
<27>	Adc_hbf2_bypass_ch6		
<26>	Adc_hbf3_bypass_ch6		
<25>	Adc_hbf1_bypass_ch5		
<24>	Adc_hbf2_bypass_ch5		
<23>	Adc_hbf3_bypass_ch5		
<22>	Adc_hbf1_bypass_ch4		
<21>	Adc_hbf2_bypass_ch4		
<20>	Adc_hbf3_bypass_ch4		
<19>	Adc_hbf1_bypass_ch3		
<18>	Adc_hbf2_bypass_ch3		
<17>	Adc_hbf3_bypass_ch3		
<16>	Adc_hbf1_bypass_ch2		
<15>	Adc_hbf2_bypass_ch2		
<14>	Adc_hbf3_bypass_ch2		
<13>	Adc_hbf1_bypass_ch1		1'b0
<12>	Adc_hbf2_bypass_ch1		1'b0
<11>	Adc_hbf3_bypass_ch1		1'b0
<10>	Adc_hbf1_bypass_ch0		1'b0
<9>	Adc_hbf2_bypass_ch0		1'b0
<8>	Adc_hbf3_bypass_ch0	1: bypass	1'b0
<7>	reserved		
<6>	Dac_R_iir_enable		1'b0
<5>	Dac_L_iir_enable		1'b0
<4:3>	s_dac_osr_sel<1:0>	1x=512osr, 01=256osr, 00=128osr	2'b10
<2:0>	s_fs_sel	Used for ramp	3'b0
*/
struct _hw_dac_iir_control
{
    uint32 s_fs_sel : 3;
    uint32 s_dac_osr_sel : 2;	

    uint32 Dac_L_iir_enable : 1;	
    uint32 Dac_R_iir_enable : 1;	
    
    uint32 reserved : 1;	
    
    uint32 Adc_hbf3_bypass_ch0 : 1;
    uint32 Adc_hbf2_bypass_ch0 : 1;
    uint32 Adc_hbf1_bypass_ch0 : 1;

    uint32 Adc_hbf3_bypass_ch1 : 1;
    uint32 Adc_hbf2_bypass_ch1 : 1;
    uint32 Adc_hbf1_bypass_ch1 : 1;

    uint32 Adc_hbf3_bypass_ch2 : 1;
    uint32 Adc_hbf2_bypass_ch2 : 1;
    uint32 Adc_hbf1_bypass_ch2 : 1;

    uint32 Adc_hbf3_bypass_ch3 : 1;
    uint32 Adc_hbf2_bypass_ch3 : 1;
    uint32 Adc_hbf1_bypass_ch3 : 1;

    uint32 Adc_hbf3_bypass_ch4 : 1;
    uint32 Adc_hbf2_bypass_ch4 : 1;
    uint32 Adc_hbf1_bypass_ch4 : 1;

    uint32 Adc_hbf3_bypass_ch5 : 1;
    uint32 Adc_hbf2_bypass_ch5 : 1;
    uint32 Adc_hbf1_bypass_ch5 : 1;

    uint32 Adc_hbf3_bypass_ch6 : 1;
    uint32 Adc_hbf2_bypass_ch6 : 1;
    uint32 Adc_hbf1_bypass_ch6 : 1;

    uint32 Adc_hbf3_bypass_ch7 : 1;
    uint32 Adc_hbf2_bypass_ch7 : 1;
    uint32 Adc_hbf1_bypass_ch7 : 1;
        
};


/*******************************data struction***********************************/

#define ANC_BASE                            ((uint32)CODEC_BASE)

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
volatile static struct _fir_control *fir_control=(volatile struct _fir_control *)(ANC_BASE+0x100);

//codec iir registers
volatile static struct _hw_adc_iir_control *hw_adc_iir_control=(volatile struct _hw_adc_iir_control *)(ANC_BASE+0x88);
volatile static struct _hw_dac_iir_control *hw_dac_iir_control=(volatile struct _hw_dac_iir_control *)(ANC_BASE+0x8c);

volatile static int dac_open_flag=0;
volatile static int adc_open_flag=0;

static HW_CODEC_IIR_FILTERS_T dac_filtes_l_old;
static HW_CODEC_IIR_FILTERS_T dac_filtes_r_old;
static HW_CODEC_IIR_FILTERS_T adc_filtes_l_old;
static HW_CODEC_IIR_FILTERS_T adc_filtes_r_old;

volatile static int8_t dac_iir_reset_flag;
volatile static int8_t adc_iir_reset_flag;
volatile static int8_t iir_coef_using;

volatile static enum AUD_SAMPRATE_T sample_rate_adc;
volatile static enum AUD_SAMPRATE_T sample_rate_dac;

#define IIR_COUNTER (6)


int hw_codec_iir_set_cfg(HW_CODEC_IIR_CFG_T *cfg,enum AUD_SAMPRATE_T sample_rate,HW_CODEC_IIR_TYPE_T hw_iir_type)
{
    HW_CODEC_IIR_ERROR err=HW_CODEC_IIR_NO_ERR;
    int i;
    const HW_CODEC_IIR_FILTERS_T *dac_filtes_l=NULL;
    const HW_CODEC_IIR_FILTERS_T *dac_filtes_r=NULL;
    const HW_CODEC_IIR_FILTERS_T *adc_filtes_l=NULL;
    const HW_CODEC_IIR_FILTERS_T *adc_filtes_r=NULL;

    if (cfg == NULL) 
    {
        LOG_I("%s: cfg is null", __func__);
        return HW_CODEC_IIR_OTHER_ERR;
    }

    if ((hw_iir_type==HW_CODEC_IIR_DAC && dac_open_flag==0) ||
            (hw_iir_type==HW_CODEC_IIR_ADC && adc_open_flag==0)) {
        LOG_I("%s: hw_iir_type=%d not opened", __func__, hw_iir_type);
        return HW_CODEC_IIR_TYPE_ERR;
    }

    if(hw_iir_type==HW_CODEC_IIR_DAC)
    {
        sample_rate_dac=sample_rate; 
    }

    if(hw_iir_type==HW_CODEC_IIR_ADC)
    {
        sample_rate_adc=sample_rate; 
    }

    if(dac_open_flag==1&&adc_open_flag==1&&sample_rate_dac!=sample_rate_adc)
    {
        ASSERT(false, "Dac samplerate must be same as adc samplerate in using hareware dac and adc IIR! ");
    }

    LOG_I("%s: iir_coef_using:%d,Iir_coef_swap:%d", __func__,iir_coef_using,fir_control->Iir_coef_swap);

    if(hw_iir_type==HW_CODEC_IIR_DAC)
    {
        dac_filtes_l = &(cfg->iir_filtes_l);
        dac_filtes_r = &(cfg->iir_filtes_r);
    }
    
    if(hw_iir_type==HW_CODEC_IIR_ADC)
    {
        adc_filtes_l = &(cfg->iir_filtes_l);
        adc_filtes_r = &(cfg->iir_filtes_r);
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
            LOG_I("%s: switching dac", __func__);

            if(iir_coef_using==0)
            {
                for(i=0;i<IIR_COUNTER;i++)
                {
                    anc_ff_iir_coefs1_l[i].a1=-dac_filtes_l->iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs1_l[i].a2=-dac_filtes_l->iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs1_l[i].b0=dac_filtes_l->iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs1_l[i].b1=dac_filtes_l->iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs1_l[i].b2=dac_filtes_l->iir_coef[i].coef_b[2];

                    anc_ff_iir_coefs1_r[i].a1=-dac_filtes_r->iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs1_r[i].a2=-dac_filtes_r->iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs1_r[i].b0=dac_filtes_r->iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs1_r[i].b1=dac_filtes_r->iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs1_r[i].b2=dac_filtes_r->iir_coef[i].coef_b[2];
                    
                    dac_filtes_l_old.iir_coef[i]=dac_filtes_l->iir_coef[i];
                    dac_filtes_r_old.iir_coef[i]=dac_filtes_r->iir_coef[i];
                }
                
                for(i=0;i<IIR_COUNTER;i++)
                {
                    anc_fb_iir_coefs1_l[i].a1=-adc_filtes_l_old.iir_coef[i].coef_a[1];
                    anc_fb_iir_coefs1_l[i].a2=-adc_filtes_l_old.iir_coef[i].coef_a[2];
                    anc_fb_iir_coefs1_l[i].b0=adc_filtes_l_old.iir_coef[i].coef_b[0];
                    anc_fb_iir_coefs1_l[i].b1=adc_filtes_l_old.iir_coef[i].coef_b[1];
                    anc_fb_iir_coefs1_l[i].b2=adc_filtes_l_old.iir_coef[i].coef_b[2];
                    
                    anc_fb_iir_coefs1_r[i].a1=-adc_filtes_r_old.iir_coef[i].coef_a[1];
                    anc_fb_iir_coefs1_r[i].a2=-adc_filtes_r_old.iir_coef[i].coef_a[2];
                    anc_fb_iir_coefs1_r[i].b0=adc_filtes_r_old.iir_coef[i].coef_b[0];
                    anc_fb_iir_coefs1_r[i].b1=adc_filtes_r_old.iir_coef[i].coef_b[1];
                    anc_fb_iir_coefs1_r[i].b2=adc_filtes_r_old.iir_coef[i].coef_b[2];
                }

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
                    anc_ff_iir_coefs0_l[i].a1=-dac_filtes_l->iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs0_l[i].a2=-dac_filtes_l->iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs0_l[i].b0=dac_filtes_l->iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs0_l[i].b1=dac_filtes_l->iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs0_l[i].b2=dac_filtes_l->iir_coef[i].coef_b[2];

                    anc_ff_iir_coefs0_r[i].a1=-dac_filtes_r->iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs0_r[i].a2=-dac_filtes_r->iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs0_r[i].b0=dac_filtes_r->iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs0_r[i].b1=dac_filtes_r->iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs0_r[i].b2=dac_filtes_r->iir_coef[i].coef_b[2];
                    
                    dac_filtes_l_old.iir_coef[i]=dac_filtes_l->iir_coef[i];
                    dac_filtes_r_old.iir_coef[i]=dac_filtes_r->iir_coef[i];

                    
                }
                
                for(i=0;i<IIR_COUNTER;i++)
                {
                    anc_fb_iir_coefs0_l[i].a1=-adc_filtes_l_old.iir_coef[i].coef_a[1];
                    anc_fb_iir_coefs0_l[i].a2=-adc_filtes_l_old.iir_coef[i].coef_a[2];
                    anc_fb_iir_coefs0_l[i].b0=adc_filtes_l_old.iir_coef[i].coef_b[0];
                    anc_fb_iir_coefs0_l[i].b1=adc_filtes_l_old.iir_coef[i].coef_b[1];
                    anc_fb_iir_coefs0_l[i].b2=adc_filtes_l_old.iir_coef[i].coef_b[2];
                    
                    anc_fb_iir_coefs0_r[i].a1=-adc_filtes_r_old.iir_coef[i].coef_a[1];
                    anc_fb_iir_coefs0_r[i].a2=-adc_filtes_r_old.iir_coef[i].coef_a[2];
                    anc_fb_iir_coefs0_r[i].b0=adc_filtes_r_old.iir_coef[i].coef_b[0];
                    anc_fb_iir_coefs0_r[i].b1=adc_filtes_r_old.iir_coef[i].coef_b[1];
                    anc_fb_iir_coefs0_r[i].b2=adc_filtes_r_old.iir_coef[i].coef_b[2];
                }

                fir_control->Iir_coef_swap=0;
                while(1)
                {
                    if(fir_control->Iir_coef_swap==0) break;
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
            anc_iir_control->iir_enable=0;
            
            dac_iir_reset_flag=1;
        }
    }


    if(hw_iir_type==HW_CODEC_IIR_ADC)
    {
        if(adc_filtes_l_old.iir_counter==adc_filtes_l->iir_counter
        &&adc_filtes_r_old.iir_counter==adc_filtes_r->iir_counter
        &&adc_filtes_l_old.iir_bypass_flag==adc_filtes_l->iir_bypass_flag
        &&adc_filtes_r_old.iir_bypass_flag==adc_filtes_r->iir_bypass_flag
        &&adc_filtes_l->iir_bypass_flag==0
        &&adc_filtes_r->iir_bypass_flag==0)
        {
            LOG_I("%s: switching adc", __func__);
        
            if(iir_coef_using==0)
            {
                for(i=0;i<IIR_COUNTER;i++)
                {
                    anc_fb_iir_coefs1_l[i].a1=-adc_filtes_l->iir_coef[i].coef_a[1];
                    anc_fb_iir_coefs1_l[i].a2=-adc_filtes_l->iir_coef[i].coef_a[2];
                    anc_fb_iir_coefs1_l[i].b0=adc_filtes_l->iir_coef[i].coef_b[0];
                    anc_fb_iir_coefs1_l[i].b1=adc_filtes_l->iir_coef[i].coef_b[1];
                    anc_fb_iir_coefs1_l[i].b2=adc_filtes_l->iir_coef[i].coef_b[2];

                    anc_fb_iir_coefs1_r[i].a1=-adc_filtes_r->iir_coef[i].coef_a[1];
                    anc_fb_iir_coefs1_r[i].a2=-adc_filtes_r->iir_coef[i].coef_a[2];
                    anc_fb_iir_coefs1_r[i].b0=adc_filtes_r->iir_coef[i].coef_b[0];
                    anc_fb_iir_coefs1_r[i].b1=adc_filtes_r->iir_coef[i].coef_b[1];
                    anc_fb_iir_coefs1_r[i].b2=adc_filtes_r->iir_coef[i].coef_b[2];

                    adc_filtes_l_old.iir_coef[i]=adc_filtes_l->iir_coef[i];
                    adc_filtes_r_old.iir_coef[i]=adc_filtes_r->iir_coef[i];                                 
                }
                
                for(i=0;i<IIR_COUNTER;i++)
                {
                    anc_ff_iir_coefs1_l[i].a1=-dac_filtes_l_old.iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs1_l[i].a2=-dac_filtes_l_old.iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs1_l[i].b0=dac_filtes_l_old.iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs1_l[i].b1=dac_filtes_l_old.iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs1_l[i].b2=dac_filtes_l_old.iir_coef[i].coef_b[2];
                    
                    anc_ff_iir_coefs1_r[i].a1=-dac_filtes_r_old.iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs1_r[i].a2=-dac_filtes_r_old.iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs1_r[i].b0=dac_filtes_r_old.iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs1_r[i].b1=dac_filtes_r_old.iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs1_r[i].b2=dac_filtes_r_old.iir_coef[i].coef_b[2];
                }

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
                    anc_fb_iir_coefs0_l[i].a1=-adc_filtes_l->iir_coef[i].coef_a[1];
                    anc_fb_iir_coefs0_l[i].a2=-adc_filtes_l->iir_coef[i].coef_a[2];
                    anc_fb_iir_coefs0_l[i].b0=adc_filtes_l->iir_coef[i].coef_b[0];
                    anc_fb_iir_coefs0_l[i].b1=adc_filtes_l->iir_coef[i].coef_b[1];
                    anc_fb_iir_coefs0_l[i].b2=adc_filtes_l->iir_coef[i].coef_b[2];

                    anc_fb_iir_coefs0_r[i].a1=-adc_filtes_r->iir_coef[i].coef_a[1];
                    anc_fb_iir_coefs0_r[i].a2=-adc_filtes_r->iir_coef[i].coef_a[2];
                    anc_fb_iir_coefs0_r[i].b0=adc_filtes_r->iir_coef[i].coef_b[0];
                    anc_fb_iir_coefs0_r[i].b1=adc_filtes_r->iir_coef[i].coef_b[1];
                    anc_fb_iir_coefs0_r[i].b2=adc_filtes_r->iir_coef[i].coef_b[2];

                    adc_filtes_l_old.iir_coef[i]=adc_filtes_l->iir_coef[i];
                    adc_filtes_r_old.iir_coef[i]=adc_filtes_r->iir_coef[i];                                 

                    
                }
                
                for(i=0;i<IIR_COUNTER;i++)
                {
                    anc_ff_iir_coefs0_l[i].a1=-dac_filtes_l_old.iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs0_l[i].a2=-dac_filtes_l_old.iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs0_l[i].b0=dac_filtes_l_old.iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs0_l[i].b1=dac_filtes_l_old.iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs0_l[i].b2=dac_filtes_l_old.iir_coef[i].coef_b[2];
                    
                    anc_ff_iir_coefs0_r[i].a1=-dac_filtes_r_old.iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs0_r[i].a2=-dac_filtes_r_old.iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs0_r[i].b0=dac_filtes_r_old.iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs0_r[i].b1=dac_filtes_r_old.iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs0_r[i].b2=dac_filtes_r_old.iir_coef[i].coef_b[2];
                }

                fir_control->Iir_coef_swap=0;
                while(1)
                {
                    if(fir_control->Iir_coef_swap==0) break;
                } 
               
            }
            adc_iir_reset_flag=0;
            iir_coef_using=1-iir_coef_using;
        }
        else
        {
            LOG_I("%s: no switching adc", __func__);
        
            adc_filtes_l_old.iir_counter=adc_filtes_l->iir_counter;
            adc_filtes_r_old.iir_counter=adc_filtes_r->iir_counter;
            
            adc_filtes_l_old.iir_bypass_flag=adc_filtes_l->iir_bypass_flag;
            adc_filtes_r_old.iir_bypass_flag=adc_filtes_r->iir_bypass_flag;
            
            //disable all IIR filter before updata the coefs;
            anc_iir_control->iir_enable=0;

            adc_iir_reset_flag=1;
        }
    }


    //DAC IIR settings
    if(hw_iir_type==HW_CODEC_IIR_DAC&&dac_iir_reset_flag==1)
    {

        for(i=0;i<IIR_COUNTER;i++)
        {
            dac_filtes_l_old.iir_coef[i]=dac_filtes_l->iir_coef[i];
            dac_filtes_r_old.iir_coef[i]=dac_filtes_r->iir_coef[i];
         }

        LOG_I("%s: resetting dac", __func__);
    
        //setting the dac IIR filters;
        if(dac_filtes_l->iir_counter>IIR_COUNTER)
        {
            anc_iir_control->iir_count_ch0=IIR_COUNTER;		
        }
        else if(dac_filtes_l->iir_counter>0)
        {
            anc_iir_control->iir_count_ch0=dac_filtes_l->iir_counter;
        }
        else
        {
            hw_dac_iir_control->Dac_L_iir_enable=0;
            LOG_I("Error dac_filtes_l IIR counter:%d", dac_filtes_l->iir_counter);
            err=HW_CODEC_IIR_COUNTER_ERR;
            return err;
        }

        if(iir_coef_using==0)
        {
            //dac left ch iir coefs settings
            for(i=0;i<IIR_COUNTER;i++)
            {
                anc_ff_iir_coefs0_l[i].a1=-dac_filtes_l->iir_coef[i].coef_a[1];
                anc_ff_iir_coefs0_l[i].a2=-dac_filtes_l->iir_coef[i].coef_a[2];
                anc_ff_iir_coefs0_l[i].b0=dac_filtes_l->iir_coef[i].coef_b[0];
                anc_ff_iir_coefs0_l[i].b1=dac_filtes_l->iir_coef[i].coef_b[1];
                anc_ff_iir_coefs0_l[i].b2=dac_filtes_l->iir_coef[i].coef_b[2];
            }
        }
        else
        {
            //dac left ch iir coefs settings
            for(i=0;i<IIR_COUNTER;i++)
            {
                anc_ff_iir_coefs1_l[i].a1=-dac_filtes_l->iir_coef[i].coef_a[1];
                anc_ff_iir_coefs1_l[i].a2=-dac_filtes_l->iir_coef[i].coef_a[2];
                anc_ff_iir_coefs1_l[i].b0=dac_filtes_l->iir_coef[i].coef_b[0];
                anc_ff_iir_coefs1_l[i].b1=dac_filtes_l->iir_coef[i].coef_b[1];
                anc_ff_iir_coefs1_l[i].b2=dac_filtes_l->iir_coef[i].coef_b[2];
            }
        }

        if(dac_filtes_r->iir_counter>IIR_COUNTER)
        {
            anc_iir_control->iir_count_ch1=IIR_COUNTER;		
        }
        else if(dac_filtes_r->iir_counter>0)
        {
            anc_iir_control->iir_count_ch1=dac_filtes_r->iir_counter;
        }
        else
        {
             hw_dac_iir_control->Dac_R_iir_enable=0;
            LOG_I("Error dac_filtes_r IIR counter:%d", dac_filtes_r->iir_counter);
            err=HW_CODEC_IIR_COUNTER_ERR;
            return err;
        }
        if(iir_coef_using==0)
        {
            //dac left ch iir coefs settings
            for(i=0;i<IIR_COUNTER;i++)
            {
                anc_ff_iir_coefs0_r[i].a1=-dac_filtes_r->iir_coef[i].coef_a[1];
                anc_ff_iir_coefs0_r[i].a2=-dac_filtes_r->iir_coef[i].coef_a[2];
                anc_ff_iir_coefs0_r[i].b0=dac_filtes_r->iir_coef[i].coef_b[0];
                anc_ff_iir_coefs0_r[i].b1=dac_filtes_r->iir_coef[i].coef_b[1];
                anc_ff_iir_coefs0_r[i].b2=dac_filtes_r->iir_coef[i].coef_b[2];
            }
        }
        else
        {
            //dac right ch iir coefs settings
            for(i=0;i<IIR_COUNTER;i++)
            {
                anc_ff_iir_coefs1_r[i].a1=-dac_filtes_r->iir_coef[i].coef_a[1];
                anc_ff_iir_coefs1_r[i].a2=-dac_filtes_r->iir_coef[i].coef_a[2];
                anc_ff_iir_coefs1_r[i].b0=dac_filtes_r->iir_coef[i].coef_b[0];
                anc_ff_iir_coefs1_r[i].b1=dac_filtes_r->iir_coef[i].coef_b[1];
                anc_ff_iir_coefs1_r[i].b2=dac_filtes_r->iir_coef[i].coef_b[2];
            }
        }
     
        //enable all IIR filters
        anc_iir_control->iir_enable=1;

       //enable dac iir;
       if(dac_filtes_l->iir_bypass_flag==0)
        {
            hw_dac_iir_control->Dac_L_iir_enable=1;
        }
        else
        {
            hw_dac_iir_control->Dac_L_iir_enable=0;
        }
        
        if(dac_filtes_r->iir_bypass_flag==0)
        {
            hw_dac_iir_control->Dac_R_iir_enable=1;
        }
        else
        {
            hw_dac_iir_control->Dac_R_iir_enable=0;
        }
    }


    //adc iir settings
    if(hw_iir_type==HW_CODEC_IIR_ADC&&adc_iir_reset_flag==1)
    {
        LOG_I("%s: resetting adc", __func__);

        for(i=0;i<IIR_COUNTER;i++)
        {
            adc_filtes_l_old.iir_coef[i]=adc_filtes_l->iir_coef[i];
            adc_filtes_r_old.iir_coef[i]=adc_filtes_r->iir_coef[i];
         }
        
        //setting the adc IIR filters;
        if(adc_filtes_l->iir_counter>IIR_COUNTER)
        {
            anc_iir_control->iir_count_ch2=IIR_COUNTER;		
        }
        else if(adc_filtes_l->iir_counter>0)
        {
            anc_iir_control->iir_count_ch2=adc_filtes_l->iir_counter;
        }
        else
        {
            hw_adc_iir_control->Adc_ch0_iir_enable=0;
            LOG_I("Error adc_filtes_l IIR counter:%d", adc_filtes_l->iir_counter);
            err=HW_CODEC_IIR_COUNTER_ERR;
            return err;
        }
        if(iir_coef_using==0)
        {
            //adc left ch iir coefs settings
            for(i=0;i<IIR_COUNTER;i++)
            {
                anc_fb_iir_coefs0_l[i].a1=-adc_filtes_l->iir_coef[i].coef_a[1];
                anc_fb_iir_coefs0_l[i].a2=-adc_filtes_l->iir_coef[i].coef_a[2];
                anc_fb_iir_coefs0_l[i].b0=adc_filtes_l->iir_coef[i].coef_b[0];
                anc_fb_iir_coefs0_l[i].b1=adc_filtes_l->iir_coef[i].coef_b[1];
                anc_fb_iir_coefs0_l[i].b2=adc_filtes_l->iir_coef[i].coef_b[2];
            }
        }
        else
        {
            //adc left ch iir coefs settings
            for(i=0;i<IIR_COUNTER;i++)
            {
                anc_fb_iir_coefs1_l[i].a1=-adc_filtes_l->iir_coef[i].coef_a[1];
                anc_fb_iir_coefs1_l[i].a2=-adc_filtes_l->iir_coef[i].coef_a[2];
                anc_fb_iir_coefs1_l[i].b0=adc_filtes_l->iir_coef[i].coef_b[0];
                anc_fb_iir_coefs1_l[i].b1=adc_filtes_l->iir_coef[i].coef_b[1];
                anc_fb_iir_coefs1_l[i].b2=adc_filtes_l->iir_coef[i].coef_b[2];
            }
         }

        if(adc_filtes_r->iir_counter>IIR_COUNTER)
        {
            anc_iir_control->iir_count_ch3=IIR_COUNTER;		
        }
        else if(adc_filtes_r->iir_counter>0)
        {
            anc_iir_control->iir_count_ch3=adc_filtes_r->iir_counter;
        }
        else
        {
             hw_adc_iir_control->Adc_ch1_iir_enable=0;
            LOG_I("Error adc_filtes_r IIR counter:%d", adc_filtes_r->iir_counter);
            err=HW_CODEC_IIR_COUNTER_ERR;
            return err;
        }
        
        if(iir_coef_using==0)
        {            
            //adc right ch iir coefs settings
            for(i=0;i<IIR_COUNTER;i++)
            {
                anc_fb_iir_coefs0_r[i].a1=-adc_filtes_r->iir_coef[i].coef_a[1];
                anc_fb_iir_coefs0_r[i].a2=-adc_filtes_r->iir_coef[i].coef_a[2];
                anc_fb_iir_coefs0_r[i].b0=adc_filtes_r->iir_coef[i].coef_b[0];
                anc_fb_iir_coefs0_r[i].b1=adc_filtes_r->iir_coef[i].coef_b[1];
                anc_fb_iir_coefs0_r[i].b2=adc_filtes_r->iir_coef[i].coef_b[2];
            }
        }
        else
        {
            //adc right ch iir coefs settings
            for(i=0;i<IIR_COUNTER;i++)
            {
                anc_fb_iir_coefs1_r[i].a1=-adc_filtes_r->iir_coef[i].coef_a[1];
                anc_fb_iir_coefs1_r[i].a2=-adc_filtes_r->iir_coef[i].coef_a[2];
                anc_fb_iir_coefs1_r[i].b0=adc_filtes_r->iir_coef[i].coef_b[0];
                anc_fb_iir_coefs1_r[i].b1=adc_filtes_r->iir_coef[i].coef_b[1];
                anc_fb_iir_coefs1_r[i].b2=adc_filtes_r->iir_coef[i].coef_b[2];
            }
         }

        //enable all IIR filters
        anc_iir_control->iir_enable=1;

        //enable hardware adc iir.
       if(adc_filtes_l->iir_bypass_flag==0)
        {
             hw_adc_iir_control->Adc_ch0_iir_enable=1;
        }
        else
        {
             hw_adc_iir_control->Adc_ch0_iir_enable=0;
        }
        
        if(adc_filtes_r->iir_bypass_flag==0)
        {
             hw_adc_iir_control->Adc_ch1_iir_enable=1;
        }
        else
        {
             hw_adc_iir_control->Adc_ch1_iir_enable=0;
        }


    }

	return err;
}

static void hw_iir_ctrl_reg_init(void)
{
    //disable adc iir;
    hw_adc_iir_control->Adc_ch0_iir_enable=0;
    hw_adc_iir_control->Adc_ch1_iir_enable=0;

   //disable dac iir;
    hw_dac_iir_control->Dac_L_iir_enable=0;
    hw_dac_iir_control->Dac_R_iir_enable=0;

    //init swap to use coef0
    fir_control->Iir_coef_swap=0;

    //disable all IIR filters
    anc_iir_control->iir_enable=0;

    //use all of IIR filters.The max number is 6.
    anc_iir_control->iir_count_ch0=1;
    anc_iir_control->iir_count_ch1=1;	
    anc_iir_control->iir_count_ch2=1;
    anc_iir_control->iir_count_ch3=1;
			
}


static void hw_iir_ctrl_reg_open( HW_CODEC_IIR_TYPE_T hw_iir_type)
{
    LOG_I("%s", __func__);

    if(hw_iir_type==HW_CODEC_IIR_DAC)
    {
    }

    if(hw_iir_type==HW_CODEC_IIR_ADC)
    {
    }
}


static void hw_iir_ctrl_reg_close(HW_CODEC_IIR_TYPE_T hw_iir_type)
{

    if(hw_iir_type==HW_CODEC_IIR_DAC)
    {
       //disable dac iir;
        hw_dac_iir_control->Dac_L_iir_enable=0;
        hw_dac_iir_control->Dac_R_iir_enable=0;        
    }


    if(hw_iir_type==HW_CODEC_IIR_ADC)
    {
        //disable adc iir;
        hw_adc_iir_control->Adc_ch0_iir_enable=0;
        hw_adc_iir_control->Adc_ch1_iir_enable=0;
        
    }


}


int hw_codec_iir_open(enum AUD_SAMPRATE_T sample_rate,  HW_CODEC_IIR_TYPE_T hw_iir_type,int32_t ch_map)
{
    HW_CODEC_IIR_ERROR err=HW_CODEC_IIR_NO_ERR;

    LOG_I("%s", __func__);

    if(dac_open_flag==0&&adc_open_flag==0)
    {
#ifdef HW_IIR_HIGH_SPEED
        hal_cmu_codec_iir_enable(65000000);
#else
        hal_cmu_codec_iir_enable(26000000);
#endif
        hw_iir_ctrl_reg_init();

        iir_coef_using=0;
        sample_rate_adc=AUD_SAMPRATE_NULL;
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

    if(hw_iir_type==HW_CODEC_IIR_ADC)
    {
        adc_iir_reset_flag=0;
        adc_open_flag=1;
        sample_rate_adc=sample_rate;    

        //clear iir_counter;
        adc_filtes_l_old.iir_counter=0;
        adc_filtes_r_old.iir_counter=0;
        adc_filtes_l_old.iir_bypass_flag=1;
        adc_filtes_r_old.iir_bypass_flag=1;
    }

    if(dac_open_flag==1&&adc_open_flag==1&&sample_rate_dac!=sample_rate_adc)
    {
        ASSERT(false, "Dac samplerate must be same as adc samplerate in using hareware dac and adc IIR! ");
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

    if(hw_iir_type==HW_CODEC_IIR_ADC)
    {
        adc_open_flag=0;
        sample_rate_adc=AUD_SAMPRATE_NULL;
    }

    hw_iir_ctrl_reg_close(hw_iir_type);

    if(dac_open_flag==0&&adc_open_flag==0)
    {
        hal_cmu_codec_iir_disable();
    }

    return err;
}


int hw_codec_iir_set_coefs(HW_CODEC_IIR_CFG_F *cfg, HW_CODEC_IIR_TYPE_T hw_iir_type)
{
    HW_CODEC_IIR_ERROR err=HW_CODEC_IIR_NO_ERR;
    const HW_CODEC_IIR_FILTERS_F *dac_filtes_l=NULL;
    const HW_CODEC_IIR_FILTERS_F *dac_filtes_r=NULL;
    const HW_CODEC_IIR_FILTERS_F *adc_filtes_l=NULL;
    const HW_CODEC_IIR_FILTERS_F *adc_filtes_r=NULL;

    LOG_I("%s,%d", __func__,hw_iir_type);   

    if(hw_iir_type==HW_CODEC_IIR_DAC)
    {
        dac_filtes_l = &(cfg->iir_filtes_l);
        dac_filtes_r = &(cfg->iir_filtes_r);
    }
    
    if(hw_iir_type==HW_CODEC_IIR_ADC)
    {
        adc_filtes_l = &(cfg->iir_filtes_l);
        adc_filtes_r = &(cfg->iir_filtes_r);
    }

    if(hw_iir_type==HW_CODEC_IIR_DAC&&dac_iir_reset_flag==1)
    {
        //disable all IIR filters
        anc_iir_control->iir_enable = 0;

        //setting the dac IIR filters;
        if(dac_filtes_l->iir_counter>IIR_COUNTER)
        {
            anc_iir_control->iir_count_ch0=IIR_COUNTER;		
        }
        else if(dac_filtes_l->iir_counter>0)
        {
            anc_iir_control->iir_count_ch0=dac_filtes_l->iir_counter;
        }
        else
        {
            hw_dac_iir_control->Dac_L_iir_enable=0;
            LOG_I("Error dac_filtes_l IIR counter:%d", dac_filtes_l->iir_counter);
            err=HW_CODEC_IIR_COUNTER_ERR;
            return err;
        }

        LOG_I("set iir coef!");
        for(int i = 0;i < adc_filtes_r->iir_counter; i++)
        {
            anc_ff_iir_coefs0_l[i].a1 = -((int32_t)((dac_filtes_l->iir_coef[i].coef_a[1]/dac_filtes_l->iir_coef[i].coef_a[0])*(int32_t)(1<<27)));
            anc_ff_iir_coefs0_l[i].a2 = -((int32_t)((dac_filtes_l->iir_coef[i].coef_a[2]/dac_filtes_l->iir_coef[i].coef_a[0])*(int32_t)(1<<27)));
            anc_ff_iir_coefs0_l[i].b0 = (int32_t)((dac_filtes_l->iir_coef[i].coef_b[0]/dac_filtes_l->iir_coef[i].coef_a[0])*(int32_t)(1<<27));
            anc_ff_iir_coefs0_l[i].b1 = (int32_t)((dac_filtes_l->iir_coef[i].coef_b[1]/dac_filtes_l->iir_coef[i].coef_a[0])*(int32_t)(1<<27));
            anc_ff_iir_coefs0_l[i].b2 = (int32_t)((dac_filtes_l->iir_coef[i].coef_b[2]/dac_filtes_l->iir_coef[i].coef_a[0])*(int32_t)(1<<27));
    
            anc_ff_iir_coefs0_r[i].a1 = -((int32_t)((dac_filtes_r->iir_coef[i].coef_a[1]/dac_filtes_r->iir_coef[i].coef_a[0])*(int32_t)(1<<27)));
            anc_ff_iir_coefs0_r[i].a2 = -((int32_t)((dac_filtes_r->iir_coef[i].coef_a[2]/dac_filtes_r->iir_coef[i].coef_a[0])*(int32_t)(1<<27)));
            anc_ff_iir_coefs0_r[i].b0 = (int32_t)((dac_filtes_r->iir_coef[i].coef_b[0]/dac_filtes_r->iir_coef[i].coef_a[0])*(int32_t)(1<<27));
            anc_ff_iir_coefs0_r[i].b1 = (int32_t)((dac_filtes_r->iir_coef[i].coef_b[1]/dac_filtes_r->iir_coef[i].coef_a[0])*(int32_t)(1<<27));
            anc_ff_iir_coefs0_r[i].b2 = (int32_t)((dac_filtes_r->iir_coef[i].coef_b[2]/dac_filtes_r->iir_coef[i].coef_a[0])*(int32_t)(1<<27));
        }

        //enable all IIR filters
        anc_iir_control->iir_enable=1;

       //enable dac iir;
       if(dac_filtes_l->iir_bypass_flag==0)
        {
            hw_dac_iir_control->Dac_L_iir_enable=1;
        }
        else
        {
            hw_dac_iir_control->Dac_L_iir_enable=0;
        }
        
        if(dac_filtes_r->iir_bypass_flag==0)
        {
            hw_dac_iir_control->Dac_R_iir_enable=1;
        }
        else
        {
            hw_dac_iir_control->Dac_R_iir_enable=0;
        }
    }

    //adc iir settings
    if(hw_iir_type==HW_CODEC_IIR_ADC)
    {

        //disable all IIR filters
        anc_iir_control->iir_enable = 0;
      
        //setting the adc IIR filters;
        if(adc_filtes_l->iir_counter>IIR_COUNTER)
        {
            anc_iir_control->iir_count_ch2=IIR_COUNTER;		
        }
        else if(adc_filtes_l->iir_counter>0)
        {
            anc_iir_control->iir_count_ch2=adc_filtes_l->iir_counter;
        }
        else
        {
            hw_adc_iir_control->Adc_ch0_iir_enable=0;
            LOG_I("Error adc_filtes_l IIR counter:%d", adc_filtes_l->iir_counter);
            err=HW_CODEC_IIR_COUNTER_ERR;
            return err;
        }

        if(adc_filtes_r->iir_counter>IIR_COUNTER)
        {
            anc_iir_control->iir_count_ch3=IIR_COUNTER;		
        }
        else if(adc_filtes_r->iir_counter>0)
        {
            anc_iir_control->iir_count_ch3=adc_filtes_r->iir_counter;
        }
        else
        {
             hw_adc_iir_control->Adc_ch1_iir_enable=0;
            LOG_I("Error adc_filtes_r IIR counter:%d", adc_filtes_r->iir_counter);
            err=HW_CODEC_IIR_COUNTER_ERR;
            return err;
        }


        LOG_I("set iir coef!");
        for(int i = 0;i < adc_filtes_r->iir_counter; i++)
        {
            anc_fb_iir_coefs0_l[i].a1 = -((int32_t)((adc_filtes_l->iir_coef[i].coef_a[1]/adc_filtes_l->iir_coef[i].coef_a[0])*(int32_t)(1<<27)));
            anc_fb_iir_coefs0_l[i].a2 = -((int32_t)((adc_filtes_l->iir_coef[i].coef_a[2]/adc_filtes_l->iir_coef[i].coef_a[0])*(int32_t)(1<<27)));
            anc_fb_iir_coefs0_l[i].b0 = (int32_t)((adc_filtes_l->iir_coef[i].coef_b[0]/adc_filtes_l->iir_coef[i].coef_a[0])*(int32_t)(1<<27));
            anc_fb_iir_coefs0_l[i].b1 = (int32_t)((adc_filtes_l->iir_coef[i].coef_b[1]/adc_filtes_l->iir_coef[i].coef_a[0])*(int32_t)(1<<27));
            anc_fb_iir_coefs0_l[i].b2 = (int32_t)((adc_filtes_l->iir_coef[i].coef_b[2]/adc_filtes_l->iir_coef[i].coef_a[0])*(int32_t)(1<<27));
    
            anc_fb_iir_coefs0_r[i].a1 = -((int32_t)((adc_filtes_r->iir_coef[i].coef_a[1]/adc_filtes_r->iir_coef[i].coef_a[0])*(int32_t)(1<<27)));
            anc_fb_iir_coefs0_r[i].a2 = -((int32_t)((adc_filtes_r->iir_coef[i].coef_a[2]/adc_filtes_r->iir_coef[i].coef_a[0])*(int32_t)(1<<27)));
            anc_fb_iir_coefs0_r[i].b0 = (int32_t)((adc_filtes_r->iir_coef[i].coef_b[0]/adc_filtes_r->iir_coef[i].coef_a[0])*(int32_t)(1<<27));
            anc_fb_iir_coefs0_r[i].b1 = (int32_t)((adc_filtes_r->iir_coef[i].coef_b[1]/adc_filtes_r->iir_coef[i].coef_a[0])*(int32_t)(1<<27));
            anc_fb_iir_coefs0_r[i].b2 = (int32_t)((adc_filtes_r->iir_coef[i].coef_b[2]/adc_filtes_r->iir_coef[i].coef_a[0])*(int32_t)(1<<27));
        }

        //enable all IIR filters
        anc_iir_control->iir_enable=1;

        //enable hardware adc iir.
       if(adc_filtes_l->iir_bypass_flag==0)
        {
             hw_adc_iir_control->Adc_ch0_iir_enable=1;
        }
        else
        {
             hw_adc_iir_control->Adc_ch0_iir_enable=0;
        }
        
        if(adc_filtes_r->iir_bypass_flag==0)
        {
             hw_adc_iir_control->Adc_ch1_iir_enable=1;
        }
        else
        {
             hw_adc_iir_control->Adc_ch1_iir_enable=0;
        }
    }
    return err;
}



