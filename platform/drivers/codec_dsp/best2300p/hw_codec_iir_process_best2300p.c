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


/*******************************data struction***********************************/

#define ANC_BASE                            ((uint32)CODEC_BASE)

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

#define IIR_COUNTER (8)

int hal_codec_iir_dump(HW_CODEC_IIR_CFG_T *cfg)
{
    uint16_t iir_counter_l = cfg->iir_filtes_l.iir_counter;
    uint16_t iir_counter_r = cfg->iir_filtes_r.iir_counter;

    LOG_I("####################[%s] Start####################", __func__);

    if (iir_counter_l != iir_counter_r)
    {
        LOG_I("[%s] iir_counter_l(%d) != iir_counter_r(%d)", __func__, iir_counter_l, iir_counter_r);
        
        return 0;
    }

    uint16_t iir_counter = iir_counter_l;

    LOG_I("[L] iir_bypass_flag = %d", cfg->iir_filtes_l.iir_bypass_flag);
    LOG_I("[R] iir_bypass_flag = %d", cfg->iir_filtes_r.iir_bypass_flag);
    LOG_I("[L] iir_counter = %d", cfg->iir_filtes_l.iir_counter);
    LOG_I("[R] iir_counter = %d", cfg->iir_filtes_r.iir_counter);

    for (uint16_t i=0; i<iir_counter; i++)
    {
        LOG_I("[L] iir_coef = %d, %d, %d, %d, %d, %d", cfg->iir_filtes_l.iir_coef[i].coef_b[0],
                                                    cfg->iir_filtes_l.iir_coef[i].coef_b[1],
                                                    cfg->iir_filtes_l.iir_coef[i].coef_b[2],
                                                    cfg->iir_filtes_l.iir_coef[i].coef_a[0],
                                                    cfg->iir_filtes_l.iir_coef[i].coef_a[1],
                                                    cfg->iir_filtes_l.iir_coef[i].coef_a[2]);

        LOG_I("[R] iir_coef = %d, %d, %d, %d, %d, %d", cfg->iir_filtes_r.iir_coef[i].coef_b[0],
                                                    cfg->iir_filtes_r.iir_coef[i].coef_b[1],
                                                    cfg->iir_filtes_r.iir_coef[i].coef_b[2],
                                                    cfg->iir_filtes_r.iir_coef[i].coef_a[0],
                                                    cfg->iir_filtes_r.iir_coef[i].coef_a[1],
                                                    cfg->iir_filtes_r.iir_coef[i].coef_a[2]);
    }

    LOG_I("####################[%s] End####################", __func__);

    return 0;
}

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

    LOG_I("%s: iir_coef_using:%d,codec_iir_coef_swap:%d", __func__,iir_coef_using,anc_iir_control->codec_iir_coef_swap);

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

                anc_iir_control->codec_iir_coef_swap=1;
                while(1)
                {
                    if(anc_iir_control->codec_iir_coef_swap_status==1) break;
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

                anc_iir_control->codec_iir_coef_swap=0;
                while(1)
                {
                    if(anc_iir_control->codec_iir_coef_swap_status==0) break;
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
            anc_iir_control->codec_iir_enable=0;
            
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

                anc_iir_control->codec_iir_coef_swap=1;
                while(1)
                {
                    if(anc_iir_control->codec_iir_coef_swap_status==1) break;
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

                anc_iir_control->codec_iir_coef_swap=0;
                while(1)
                {
                    if(anc_iir_control->codec_iir_coef_swap_status==0) break;
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
            anc_iir_control->codec_iir_enable=0;

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
            anc_iir_control->codec_iir_count_ch0=IIR_COUNTER;		
        }
        else if(dac_filtes_l->iir_counter>=0)
        {
            anc_iir_control->codec_iir_count_ch0=dac_filtes_l->iir_counter;
        }
        else
        {
            anc_iir_control->codec_dac_L_iir_enable=0;
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
            anc_iir_control->codec_iir_count_ch1=IIR_COUNTER;		
        }
        else if(dac_filtes_r->iir_counter>=0)
        {
            anc_iir_control->codec_iir_count_ch1=dac_filtes_r->iir_counter;
        }
        else
        {
             anc_iir_control->codec_dac_R_iir_enable=0;
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
        anc_iir_control->codec_iir_enable=1;

        //enable dac iir;
        if((dac_filtes_l->iir_bypass_flag==0) && dac_filtes_l->iir_counter)
        {
            anc_iir_control->codec_dac_L_iir_enable=1;
        }
        else
        {
            anc_iir_control->codec_dac_L_iir_enable=0;
        }
        
        if((dac_filtes_r->iir_bypass_flag==0) && dac_filtes_r->iir_counter)
        {
            anc_iir_control->codec_dac_R_iir_enable=1;
        }
        else
        {
            anc_iir_control->codec_dac_R_iir_enable=0;
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
            anc_iir_control->codec_iir_count_ch2=IIR_COUNTER;		
        }
        else if(adc_filtes_l->iir_counter>0)
        {
            anc_iir_control->codec_iir_count_ch2=adc_filtes_l->iir_counter;
        }
        else
        {
            anc_iir_control->codec_adc_ch0_iir_enable=0;
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
            anc_iir_control->codec_iir_count_ch3=IIR_COUNTER;		
        }
        else if(adc_filtes_r->iir_counter>0)
        {
            anc_iir_control->codec_iir_count_ch3=adc_filtes_r->iir_counter;
        }
        else
        {
             anc_iir_control->codec_adc_ch1_iir_enable=0;
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
        anc_iir_control->codec_iir_enable=1;

        //enable hardware adc iir.
       if(adc_filtes_l->iir_bypass_flag==0)
        {
             anc_iir_control->codec_adc_ch0_iir_enable=1;
        }
        else
        {
             anc_iir_control->codec_adc_ch0_iir_enable=0;
        }
        
        if(adc_filtes_r->iir_bypass_flag==0)
        {
             anc_iir_control->codec_adc_ch1_iir_enable=1;
        }
        else
        {
             anc_iir_control->codec_adc_ch1_iir_enable=0;
        }


    }

	return err;
}

static void hw_iir_ctrl_reg_init(void)
{
    //disable adc iir;
    anc_iir_control->codec_adc_ch0_iir_enable=0;
    anc_iir_control->codec_adc_ch1_iir_enable=0;

   //disable dac iir;
    anc_iir_control->codec_dac_L_iir_enable=0;
    anc_iir_control->codec_dac_R_iir_enable=0;

    //init swap to use coef0
    anc_iir_control->codec_iir_coef_swap=0;

    //disable all IIR filters
    anc_iir_control->codec_iir_enable=0;

    //use all of IIR filters.The max number is 8.
    anc_iir_control->codec_iir_count_ch0=1;
    anc_iir_control->codec_iir_count_ch1=1;	
    anc_iir_control->codec_iir_count_ch2=1;
    anc_iir_control->codec_iir_count_ch3=1;
			
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
        anc_iir_control->codec_dac_L_iir_enable=0;
        anc_iir_control->codec_dac_R_iir_enable=0;        
    }


    if(hw_iir_type==HW_CODEC_IIR_ADC)
    {
        //disable adc iir;
        anc_iir_control->codec_adc_ch0_iir_enable=0;
        anc_iir_control->codec_adc_ch1_iir_enable=0;
        
    }


}


int hw_codec_iir_open(enum AUD_SAMPRATE_T sample_rate,  HW_CODEC_IIR_TYPE_T hw_iir_type, int32_t ch_map)
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
        anc_iir_control->codec_iir_enable = 0;

        //setting the dac IIR filters;
        if(dac_filtes_l->iir_counter>IIR_COUNTER)
        {
            anc_iir_control->codec_iir_count_ch0=IIR_COUNTER;		
        }
        else if(dac_filtes_l->iir_counter>0)
        {
            anc_iir_control->codec_iir_count_ch0=dac_filtes_l->iir_counter;
        }
        else
        {
            anc_iir_control->codec_dac_L_iir_enable=0;
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
        anc_iir_control->codec_iir_enable=1;

       //enable dac iir;
       if(dac_filtes_l->iir_bypass_flag==0)
        {
            anc_iir_control->codec_dac_L_iir_enable=1;
        }
        else
        {
            anc_iir_control->codec_dac_L_iir_enable=0;
        }
        
        if(dac_filtes_r->iir_bypass_flag==0)
        {
            anc_iir_control->codec_dac_R_iir_enable=1;
        }
        else
        {
            anc_iir_control->codec_dac_R_iir_enable=0;
        }
    }

    //adc iir settings
    if(hw_iir_type==HW_CODEC_IIR_ADC)
    {

        //disable all IIR filters
        anc_iir_control->codec_iir_enable = 0;
      
        //setting the adc IIR filters;
        if(adc_filtes_l->iir_counter>IIR_COUNTER)
        {
            anc_iir_control->codec_iir_count_ch2=IIR_COUNTER;		
        }
        else if(adc_filtes_l->iir_counter>0)
        {
            anc_iir_control->codec_iir_count_ch2=adc_filtes_l->iir_counter;
        }
        else
        {
            anc_iir_control->codec_adc_ch0_iir_enable=0;
            LOG_I("Error adc_filtes_l IIR counter:%d", adc_filtes_l->iir_counter);
            err=HW_CODEC_IIR_COUNTER_ERR;
            return err;
        }

        if(adc_filtes_r->iir_counter>IIR_COUNTER)
        {
            anc_iir_control->codec_iir_count_ch3=IIR_COUNTER;		
        }
        else if(adc_filtes_r->iir_counter>0)
        {
            anc_iir_control->codec_iir_count_ch3=adc_filtes_r->iir_counter;
        }
        else
        {
             anc_iir_control->codec_adc_ch1_iir_enable=0;
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
        anc_iir_control->codec_iir_enable=1;

        //enable hardware adc iir.
       if(adc_filtes_l->iir_bypass_flag==0)
        {
             anc_iir_control->codec_adc_ch0_iir_enable=1;
        }
        else
        {
             anc_iir_control->codec_adc_ch0_iir_enable=0;
        }
        
        if(adc_filtes_r->iir_bypass_flag==0)
        {
             anc_iir_control->codec_adc_ch1_iir_enable=1;
        }
        else
        {
             anc_iir_control->codec_adc_ch1_iir_enable=0;
        }
    }
    return err;
}



