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
#include "hal_codec.h"

#include "iir_process.h"
#include "hw_codec_iir_process.h"

/**********************************mocro**************************************/

typedef short int16;
typedef unsigned short uint16;
typedef int   int32;
typedef unsigned int   uint32;


/*
1	18c	codec_iir_d_enable	RW	0	0
1	18c	codec_iir_d_ch0_bypass	RW	0	1
1	18c	codec_iir_d_ch1_bypass	RW	0	2
5	18c	codec_iir_d_count_ch0	RW	0	3
5	18c	codec_iir_d_count_ch1	RW	0	8
1	18c	codec_iir_d_coef_swap	RW	0	13
1	18c	codec_iir_d_coef_swap_status	R	0	14
15	18c		x
*/
struct _anc_iir_d_control
{
	uint32 codec_d_iir_enable : 1;
	uint32 codec_d_iir_ch0_bypass : 1;
	uint32 codec_d_iir_ch1_bypass : 1;
	uint32 codec_d_iir_count_ch0 : 5;
	uint32 codec_d_iir_count_ch1 : 5;
       uint32 codec_d_iir_coef_swap : 1;
       uint32 codec_d_iir_coef_swap_status : 1;

	uint32 reserved : 17;
};
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
struct _anc_iir_a_control
{
	uint32 codec_a_iir_enable : 1;
	uint32 codec_a_iir_ch0_bypass : 1;
	uint32 codec_a_iir_ch1_bypass : 1;
	uint32 codec_a_iir_count_ch0 : 4;
	uint32 codec_a_iir_count_ch1 : 4;
       uint32 codec_a_iir_coef_swap : 1;
       uint32 codec_a_iir_coef_swap_status : 1;

	uint32 reserved : 19;
};


struct _anc_iir_control_dac
{
	uint32 reserved : 27;

       uint32 codec_dac_L_iir_enable : 1;
       uint32 codec_dac_R_iir_enable : 1;

	uint32 reserved1 : 3;
};

struct _anc_iir_control_adc0
{
	uint32 reserved : 30;

       uint32 codec_adc_ch0_iir_enable : 1;

	uint32 reserved1 : 2;
};

struct _anc_iir_control_adc1
{
	uint32 reserved : 30;

       uint32 codec_adc_ch1_iir_enable : 1;

	uint32 reserved1 : 2;
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
volatile static struct _anc_iir_d_control *anc_iir_d_control=(volatile struct _anc_iir_d_control *)(ANC_BASE+0x18c);
volatile static struct _anc_iir_a_control *anc_iir_a_control=(volatile struct _anc_iir_a_control *)(ANC_BASE+0xcc);

volatile static struct _anc_iir_control_dac *anc_iir_control_dac = (volatile struct _anc_iir_control_dac *)(ANC_BASE+0x88);
volatile static struct _anc_iir_control_adc0 *anc_iir_control_adc0 = (volatile struct _anc_iir_control_adc0 *)(ANC_BASE+0x84);
volatile static struct _anc_iir_control_adc1 *anc_iir_control_adc1 = (volatile struct _anc_iir_control_adc1 *)(ANC_BASE+0x180);

volatile static struct _anc_iir_coefs *anc_dac_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x2000);
volatile static struct _anc_iir_coefs *anc_dac_iir_coefs0_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x2190);
volatile static struct _anc_iir_coefs *anc_dac_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x2400);
volatile static struct _anc_iir_coefs *anc_dac_iir_coefs1_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x2590);

volatile static struct _anc_iir_coefs *anc_ff_iir_coefs0_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x3000);
volatile static struct _anc_iir_coefs *anc_ff_iir_coefs0_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x30dc);
volatile static struct _anc_iir_coefs *anc_ff_iir_coefs1_l=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x3200);
volatile static struct _anc_iir_coefs *anc_ff_iir_coefs1_r=(volatile struct _anc_iir_coefs *)(ANC_BASE+0x32dc);

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

static int eq_output_ch_map=AUD_CHANNEL_MAP_CH0|AUD_CHANNEL_MAP_CH1;

#define IIR_DAC_COUNTER (20)
#define IIR_ADC_COUNTER (8)


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

    LOG_I("%s: iir_coef_using:%d,codec_d_iir_coef_swap:%d", __func__,iir_coef_using,anc_iir_d_control->codec_d_iir_coef_swap);

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
            LOG_I("%s: switching dac IIR", __func__);

            if(iir_coef_using==0)
            {
                for(i=0;i<IIR_DAC_COUNTER;i++)
                {
                    anc_dac_iir_coefs1_l[i].a1=-dac_filtes_l->iir_coef[i].coef_a[1];
                    anc_dac_iir_coefs1_l[i].a2=-dac_filtes_l->iir_coef[i].coef_a[2];
                    anc_dac_iir_coefs1_l[i].b0=dac_filtes_l->iir_coef[i].coef_b[0];
                    anc_dac_iir_coefs1_l[i].b1=dac_filtes_l->iir_coef[i].coef_b[1];
                    anc_dac_iir_coefs1_l[i].b2=dac_filtes_l->iir_coef[i].coef_b[2];

                    anc_dac_iir_coefs1_r[i].a1=-dac_filtes_r->iir_coef[i].coef_a[1];
                    anc_dac_iir_coefs1_r[i].a2=-dac_filtes_r->iir_coef[i].coef_a[2];
                    anc_dac_iir_coefs1_r[i].b0=dac_filtes_r->iir_coef[i].coef_b[0];
                    anc_dac_iir_coefs1_r[i].b1=dac_filtes_r->iir_coef[i].coef_b[1];
                    anc_dac_iir_coefs1_r[i].b2=dac_filtes_r->iir_coef[i].coef_b[2];

                    dac_filtes_l_old.iir_coef[i]=dac_filtes_l->iir_coef[i];
                    dac_filtes_r_old.iir_coef[i]=dac_filtes_r->iir_coef[i];
                }
                anc_iir_d_control->codec_d_iir_coef_swap=1;
                while(1)
                {
                    if(anc_iir_d_control->codec_d_iir_coef_swap_status==1) break;
                }

            }
            else
            {

                for(i=0;i<IIR_DAC_COUNTER;i++)
                {
                    anc_dac_iir_coefs0_l[i].a1=-dac_filtes_l->iir_coef[i].coef_a[1];
                    anc_dac_iir_coefs0_l[i].a2=-dac_filtes_l->iir_coef[i].coef_a[2];
                    anc_dac_iir_coefs0_l[i].b0=dac_filtes_l->iir_coef[i].coef_b[0];
                    anc_dac_iir_coefs0_l[i].b1=dac_filtes_l->iir_coef[i].coef_b[1];
                    anc_dac_iir_coefs0_l[i].b2=dac_filtes_l->iir_coef[i].coef_b[2];

                    anc_dac_iir_coefs0_r[i].a1=-dac_filtes_r->iir_coef[i].coef_a[1];
                    anc_dac_iir_coefs0_r[i].a2=-dac_filtes_r->iir_coef[i].coef_a[2];
                    anc_dac_iir_coefs0_r[i].b0=dac_filtes_r->iir_coef[i].coef_b[0];
                    anc_dac_iir_coefs0_r[i].b1=dac_filtes_r->iir_coef[i].coef_b[1];
                    anc_dac_iir_coefs0_r[i].b2=dac_filtes_r->iir_coef[i].coef_b[2];

                    dac_filtes_l_old.iir_coef[i]=dac_filtes_l->iir_coef[i];
                    dac_filtes_r_old.iir_coef[i]=dac_filtes_r->iir_coef[i];
                }
                anc_iir_d_control->codec_d_iir_coef_swap=0;
                while(1)
                {
                    if(anc_iir_d_control->codec_d_iir_coef_swap_status==0) break;
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
            anc_iir_d_control->codec_d_iir_enable=0;

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
                for(i=0;i<IIR_ADC_COUNTER;i++)
                {
                    anc_ff_iir_coefs1_l[i].a1=-adc_filtes_l->iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs1_l[i].a2=-adc_filtes_l->iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs1_l[i].b0=adc_filtes_l->iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs1_l[i].b1=adc_filtes_l->iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs1_l[i].b2=adc_filtes_l->iir_coef[i].coef_b[2];

                    anc_ff_iir_coefs1_r[i].a1=-adc_filtes_r->iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs1_r[i].a2=-adc_filtes_r->iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs1_r[i].b0=adc_filtes_r->iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs1_r[i].b1=adc_filtes_r->iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs1_r[i].b2=adc_filtes_r->iir_coef[i].coef_b[2];

                    adc_filtes_l_old.iir_coef[i]=adc_filtes_l->iir_coef[i];
                    adc_filtes_r_old.iir_coef[i]=adc_filtes_r->iir_coef[i];
                }
                anc_iir_a_control->codec_a_iir_coef_swap=1;
                while(1)
                {
                    if(anc_iir_a_control->codec_a_iir_coef_swap_status==1) break;
                }

            }
            else
            {
                for(i=0;i<IIR_ADC_COUNTER;i++)
                {
                    anc_ff_iir_coefs0_l[i].a1=-adc_filtes_l->iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs0_l[i].a2=-adc_filtes_l->iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs0_l[i].b0=adc_filtes_l->iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs0_l[i].b1=adc_filtes_l->iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs0_l[i].b2=adc_filtes_l->iir_coef[i].coef_b[2];

                    anc_ff_iir_coefs0_r[i].a1=-adc_filtes_r->iir_coef[i].coef_a[1];
                    anc_ff_iir_coefs0_r[i].a2=-adc_filtes_r->iir_coef[i].coef_a[2];
                    anc_ff_iir_coefs0_r[i].b0=adc_filtes_r->iir_coef[i].coef_b[0];
                    anc_ff_iir_coefs0_r[i].b1=adc_filtes_r->iir_coef[i].coef_b[1];
                    anc_ff_iir_coefs0_r[i].b2=adc_filtes_r->iir_coef[i].coef_b[2];

                    adc_filtes_l_old.iir_coef[i]=adc_filtes_l->iir_coef[i];
                    adc_filtes_r_old.iir_coef[i]=adc_filtes_r->iir_coef[i];


                }
                anc_iir_a_control->codec_a_iir_coef_swap=0;
                while(1)
                {
                    if(anc_iir_a_control->codec_a_iir_coef_swap_status==0) break;
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
            anc_iir_a_control->codec_a_iir_enable=0;

            adc_iir_reset_flag=1;
        }
    }


    //DAC IIR settings
    if(hw_iir_type==HW_CODEC_IIR_DAC&&dac_iir_reset_flag==1)
    {

        for(i=0;i<IIR_DAC_COUNTER;i++)
        {
            dac_filtes_l_old.iir_coef[i]=dac_filtes_l->iir_coef[i];
            dac_filtes_r_old.iir_coef[i]=dac_filtes_r->iir_coef[i];
         }

        LOG_I("%s: resetting dac", __func__);

        //setting the dac IIR filters;
        if(dac_filtes_l->iir_counter>IIR_DAC_COUNTER)
        {
            anc_iir_d_control->codec_d_iir_count_ch0=IIR_DAC_COUNTER;
        }
        else if(dac_filtes_l->iir_counter>0)
        {
            anc_iir_d_control->codec_d_iir_count_ch0=dac_filtes_l->iir_counter;
        }
        else
        {
            anc_iir_control_dac->codec_dac_L_iir_enable=0;
            LOG_I("Error dac_filtes_l IIR counter:%d", dac_filtes_l->iir_counter);
            err=HW_CODEC_IIR_COUNTER_ERR;
            return err;
        }

        if(iir_coef_using==0)
        {
            //dac left ch iir coefs settings
            for(i=0;i<IIR_DAC_COUNTER;i++)
            {
                anc_dac_iir_coefs0_l[i].a1=-dac_filtes_l->iir_coef[i].coef_a[1];
                anc_dac_iir_coefs0_l[i].a2=-dac_filtes_l->iir_coef[i].coef_a[2];
                anc_dac_iir_coefs0_l[i].b0=dac_filtes_l->iir_coef[i].coef_b[0];
                anc_dac_iir_coefs0_l[i].b1=dac_filtes_l->iir_coef[i].coef_b[1];
                anc_dac_iir_coefs0_l[i].b2=dac_filtes_l->iir_coef[i].coef_b[2];
            }
        }
        else
        {
            //dac left ch iir coefs settings
            for(i=0;i<IIR_DAC_COUNTER;i++)
            {
                anc_dac_iir_coefs1_l[i].a1=-dac_filtes_l->iir_coef[i].coef_a[1];
                anc_dac_iir_coefs1_l[i].a2=-dac_filtes_l->iir_coef[i].coef_a[2];
                anc_dac_iir_coefs1_l[i].b0=dac_filtes_l->iir_coef[i].coef_b[0];
                anc_dac_iir_coefs1_l[i].b1=dac_filtes_l->iir_coef[i].coef_b[1];
                anc_dac_iir_coefs1_l[i].b2=dac_filtes_l->iir_coef[i].coef_b[2];
            }
        }

        if(dac_filtes_r->iir_counter>IIR_DAC_COUNTER)
        {
            anc_iir_d_control->codec_d_iir_count_ch1=IIR_DAC_COUNTER;
        }
        else if(dac_filtes_r->iir_counter>0)
        {
            anc_iir_d_control->codec_d_iir_count_ch1=dac_filtes_r->iir_counter;
        }
        else
        {
             anc_iir_control_dac->codec_dac_R_iir_enable=0;
            LOG_I("Error dac_filtes_r IIR counter:%d", dac_filtes_r->iir_counter);
            err=HW_CODEC_IIR_COUNTER_ERR;
            return err;
        }
        if(iir_coef_using==0)
        {
            //dac left ch iir coefs settings
            for(i=0;i<IIR_DAC_COUNTER;i++)
            {
                anc_dac_iir_coefs0_r[i].a1=-dac_filtes_r->iir_coef[i].coef_a[1];
                anc_dac_iir_coefs0_r[i].a2=-dac_filtes_r->iir_coef[i].coef_a[2];
                anc_dac_iir_coefs0_r[i].b0=dac_filtes_r->iir_coef[i].coef_b[0];
                anc_dac_iir_coefs0_r[i].b1=dac_filtes_r->iir_coef[i].coef_b[1];
                anc_dac_iir_coefs0_r[i].b2=dac_filtes_r->iir_coef[i].coef_b[2];
            }
        }
        else
        {
            //dac right ch iir coefs settings
            for(i=0;i<IIR_DAC_COUNTER;i++)
            {
                anc_dac_iir_coefs1_r[i].a1=-dac_filtes_r->iir_coef[i].coef_a[1];
                anc_dac_iir_coefs1_r[i].a2=-dac_filtes_r->iir_coef[i].coef_a[2];
                anc_dac_iir_coefs1_r[i].b0=dac_filtes_r->iir_coef[i].coef_b[0];
                anc_dac_iir_coefs1_r[i].b1=dac_filtes_r->iir_coef[i].coef_b[1];
                anc_dac_iir_coefs1_r[i].b2=dac_filtes_r->iir_coef[i].coef_b[2];
            }
        }

        //enable all IIR filters
        anc_iir_d_control->codec_d_iir_enable=1;
        LOG_I("%s:anc_iir_d_control->codec_d_iir_enable %x", __func__, anc_iir_d_control->codec_d_iir_enable);

       //enable dac iir;
       if(dac_filtes_l->iir_bypass_flag==0)
        {
            anc_iir_control_dac->codec_dac_L_iir_enable=1;
        }
        else
        {
            anc_iir_control_dac->codec_dac_L_iir_enable=0;
        }

        if(dac_filtes_r->iir_bypass_flag==0)
        {
            anc_iir_control_dac->codec_dac_R_iir_enable=1;
        }
        else
        {
            anc_iir_control_dac->codec_dac_R_iir_enable=0;
        }

        if(!(eq_output_ch_map&AUD_CHANNEL_MAP_CH0))
        {
            anc_iir_d_control->codec_d_iir_ch0_bypass=1;
            anc_iir_control_dac->codec_dac_L_iir_enable=0;
        }

        if(!(eq_output_ch_map&AUD_CHANNEL_MAP_CH1))
        {
            anc_iir_d_control->codec_d_iir_ch1_bypass=1;
            anc_iir_control_dac->codec_dac_R_iir_enable=0;
        }
    }


    //adc iir settings
    if(hw_iir_type==HW_CODEC_IIR_ADC&&adc_iir_reset_flag==1)
    {
        LOG_I("%s: resetting adc", __func__);

        for(i=0;i<IIR_ADC_COUNTER;i++)
        {
            adc_filtes_l_old.iir_coef[i]=adc_filtes_l->iir_coef[i];
            adc_filtes_r_old.iir_coef[i]=adc_filtes_r->iir_coef[i];
         }

        //setting the adc IIR filters;
        if(adc_filtes_l->iir_counter>IIR_ADC_COUNTER)
        {
            anc_iir_a_control->codec_a_iir_count_ch0=IIR_ADC_COUNTER;
        }
        else if(adc_filtes_l->iir_counter>0)
        {
            anc_iir_a_control->codec_a_iir_count_ch0=adc_filtes_l->iir_counter;
        }
        else
        {
            anc_iir_control_adc0->codec_adc_ch0_iir_enable=0;
            LOG_I("Error adc_filtes_l IIR counter:%d", adc_filtes_l->iir_counter);
            err=HW_CODEC_IIR_COUNTER_ERR;
            return err;
        }
        if(iir_coef_using==0)
        {
            //adc left ch iir coefs settings
            for(i=0;i<IIR_ADC_COUNTER;i++)
            {
                anc_ff_iir_coefs0_l[i].a1=-adc_filtes_l->iir_coef[i].coef_a[1];
                anc_ff_iir_coefs0_l[i].a2=-adc_filtes_l->iir_coef[i].coef_a[2];
                anc_ff_iir_coefs0_l[i].b0=adc_filtes_l->iir_coef[i].coef_b[0];
                anc_ff_iir_coefs0_l[i].b1=adc_filtes_l->iir_coef[i].coef_b[1];
                anc_ff_iir_coefs0_l[i].b2=adc_filtes_l->iir_coef[i].coef_b[2];
            }
        }
        else
        {
            //adc left ch iir coefs settings
            for(i=0;i<IIR_ADC_COUNTER;i++)
            {
                anc_ff_iir_coefs1_l[i].a1=-adc_filtes_l->iir_coef[i].coef_a[1];
                anc_ff_iir_coefs1_l[i].a2=-adc_filtes_l->iir_coef[i].coef_a[2];
                anc_ff_iir_coefs1_l[i].b0=adc_filtes_l->iir_coef[i].coef_b[0];
                anc_ff_iir_coefs1_l[i].b1=adc_filtes_l->iir_coef[i].coef_b[1];
                anc_ff_iir_coefs1_l[i].b2=adc_filtes_l->iir_coef[i].coef_b[2];
            }
         }

        if(adc_filtes_r->iir_counter>IIR_ADC_COUNTER)
        {
            anc_iir_a_control->codec_a_iir_count_ch1=IIR_ADC_COUNTER;
        }
        else if(adc_filtes_r->iir_counter>0)
        {
            anc_iir_a_control->codec_a_iir_count_ch1=adc_filtes_r->iir_counter;
        }
        else
        {
             anc_iir_control_adc1->codec_adc_ch1_iir_enable=0;
            LOG_I("Error adc_filtes_r IIR counter:%d", adc_filtes_r->iir_counter);
            err=HW_CODEC_IIR_COUNTER_ERR;
            return err;
        }

        if(iir_coef_using==0)
        {
            //adc right ch iir coefs settings
            for(i=0;i<IIR_ADC_COUNTER;i++)
            {
                anc_ff_iir_coefs0_r[i].a1=-adc_filtes_r->iir_coef[i].coef_a[1];
                anc_ff_iir_coefs0_r[i].a2=-adc_filtes_r->iir_coef[i].coef_a[2];
                anc_ff_iir_coefs0_r[i].b0=adc_filtes_r->iir_coef[i].coef_b[0];
                anc_ff_iir_coefs0_r[i].b1=adc_filtes_r->iir_coef[i].coef_b[1];
                anc_ff_iir_coefs0_r[i].b2=adc_filtes_r->iir_coef[i].coef_b[2];
            }
        }
        else
        {
            //adc right ch iir coefs settings
            for(i=0;i<IIR_ADC_COUNTER;i++)
            {
                anc_ff_iir_coefs1_r[i].a1=-adc_filtes_r->iir_coef[i].coef_a[1];
                anc_ff_iir_coefs1_r[i].a2=-adc_filtes_r->iir_coef[i].coef_a[2];
                anc_ff_iir_coefs1_r[i].b0=adc_filtes_r->iir_coef[i].coef_b[0];
                anc_ff_iir_coefs1_r[i].b1=adc_filtes_r->iir_coef[i].coef_b[1];
                anc_ff_iir_coefs1_r[i].b2=adc_filtes_r->iir_coef[i].coef_b[2];
            }
         }

        //enable all IIR filters
        anc_iir_a_control->codec_a_iir_enable=1;

        //enable hardware adc iir.
       if(adc_filtes_l->iir_bypass_flag==0)
        {
             anc_iir_control_adc0->codec_adc_ch0_iir_enable=1;
        }
        else
        {
             anc_iir_control_adc0->codec_adc_ch0_iir_enable=0;
        }


        if(adc_filtes_r->iir_bypass_flag==0)
        {
             anc_iir_control_adc1->codec_adc_ch1_iir_enable=1;
        }
        else
        {
             anc_iir_control_adc1->codec_adc_ch1_iir_enable=0;
        }



    }

	return err;
}

static void hw_iir_ctrl_reg_init(void)
{
    //disable adc iir;
    anc_iir_control_adc0->codec_adc_ch0_iir_enable=0;
    anc_iir_control_adc1->codec_adc_ch1_iir_enable=0;

   //disable dac iir;
    anc_iir_control_dac->codec_dac_L_iir_enable=0;
    anc_iir_control_dac->codec_dac_R_iir_enable=0;


}


static void hw_iir_ctrl_reg_open( HW_CODEC_IIR_TYPE_T hw_iir_type)
{
    LOG_I("%s", __func__);

    if(hw_iir_type==HW_CODEC_IIR_DAC)
    {
        //init swap to use coef0
        anc_iir_d_control->codec_d_iir_coef_swap=0;

        //disable all IIR filters
        anc_iir_d_control->codec_d_iir_enable=0;

         //use all of IIR filters.The max number is 8.
        anc_iir_d_control->codec_d_iir_count_ch0=1;
        anc_iir_d_control->codec_d_iir_count_ch1=1;
    }

    if(hw_iir_type==HW_CODEC_IIR_ADC)
    {
            //init swap to use coef0
        anc_iir_a_control->codec_a_iir_coef_swap=0;

        //disable all IIR filters
        anc_iir_a_control->codec_a_iir_enable=0;

        //use all of IIR filters.The max number is 8.
        anc_iir_a_control->codec_a_iir_count_ch0=1;
        anc_iir_a_control->codec_a_iir_count_ch1=1;
    }
}


static void hw_iir_ctrl_reg_close(HW_CODEC_IIR_TYPE_T hw_iir_type)
{

    if(hw_iir_type==HW_CODEC_IIR_DAC)
    {
       //disable dac iir;
        //anc_iir_control_dac->codec_dac_L_iir_enable=0;
        anc_iir_control_dac->codec_dac_L_iir_enable=0;
        anc_iir_control_dac->codec_dac_R_iir_enable=0;
    }


    if(hw_iir_type==HW_CODEC_IIR_ADC)
    {
        //disable adc iir;
        anc_iir_control_adc0->codec_adc_ch0_iir_enable=0;
        anc_iir_control_adc1->codec_adc_ch1_iir_enable=0;

    }


}


int hw_codec_iir_open(enum AUD_SAMPRATE_T sample_rate,  HW_CODEC_IIR_TYPE_T hw_iir_type,int32_t ch_map)
{
    HW_CODEC_IIR_ERROR err=HW_CODEC_IIR_NO_ERR;

    eq_output_ch_map=ch_map;


    LOG_I("%s", __func__);

    if(dac_open_flag==0&&adc_open_flag==0)
    {
        hal_codec_iir_eq_enable(26000000);
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
        hal_codec_iir_eq_disable();
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
        anc_iir_d_control->codec_d_iir_enable = 0;

        //setting the dac IIR filters;
        if(dac_filtes_l->iir_counter>IIR_DAC_COUNTER)
        {
            anc_iir_d_control->codec_d_iir_count_ch0=IIR_DAC_COUNTER;
        }
        else if(dac_filtes_l->iir_counter>0)
        {
            anc_iir_d_control->codec_d_iir_count_ch0=dac_filtes_l->iir_counter;
        }
        else
        {
            //anc_iir_control_dac->codec_dac_L_iir_enable=0;
            anc_iir_control_dac->codec_dac_L_iir_enable=0;
            LOG_I("Error dac_filtes_l IIR counter:%d", dac_filtes_l->iir_counter);
            err=HW_CODEC_IIR_COUNTER_ERR;
            return err;
        }

        LOG_I("set iir coef!");
        for(int i = 0;i < adc_filtes_r->iir_counter; i++)
        {
            anc_dac_iir_coefs0_l[i].a1 = -((int32_t)((dac_filtes_l->iir_coef[i].coef_a[1]/dac_filtes_l->iir_coef[i].coef_a[0])*(int32_t)(1<<27)));
            anc_dac_iir_coefs0_l[i].a2 = -((int32_t)((dac_filtes_l->iir_coef[i].coef_a[2]/dac_filtes_l->iir_coef[i].coef_a[0])*(int32_t)(1<<27)));
            anc_dac_iir_coefs0_l[i].b0 = (int32_t)((dac_filtes_l->iir_coef[i].coef_b[0]/dac_filtes_l->iir_coef[i].coef_a[0])*(int32_t)(1<<27));
            anc_dac_iir_coefs0_l[i].b1 = (int32_t)((dac_filtes_l->iir_coef[i].coef_b[1]/dac_filtes_l->iir_coef[i].coef_a[0])*(int32_t)(1<<27));
            anc_dac_iir_coefs0_l[i].b2 = (int32_t)((dac_filtes_l->iir_coef[i].coef_b[2]/dac_filtes_l->iir_coef[i].coef_a[0])*(int32_t)(1<<27));

            anc_dac_iir_coefs0_r[i].a1 = -((int32_t)((dac_filtes_r->iir_coef[i].coef_a[1]/dac_filtes_r->iir_coef[i].coef_a[0])*(int32_t)(1<<27)));
            anc_dac_iir_coefs0_r[i].a2 = -((int32_t)((dac_filtes_r->iir_coef[i].coef_a[2]/dac_filtes_r->iir_coef[i].coef_a[0])*(int32_t)(1<<27)));
            anc_dac_iir_coefs0_r[i].b0 = (int32_t)((dac_filtes_r->iir_coef[i].coef_b[0]/dac_filtes_r->iir_coef[i].coef_a[0])*(int32_t)(1<<27));
            anc_dac_iir_coefs0_r[i].b1 = (int32_t)((dac_filtes_r->iir_coef[i].coef_b[1]/dac_filtes_r->iir_coef[i].coef_a[0])*(int32_t)(1<<27));
            anc_dac_iir_coefs0_r[i].b2 = (int32_t)((dac_filtes_r->iir_coef[i].coef_b[2]/dac_filtes_r->iir_coef[i].coef_a[0])*(int32_t)(1<<27));
        }

        //enable all IIR filters
        anc_iir_d_control->codec_d_iir_enable=1;

       //enable dac iir;
       if(dac_filtes_l->iir_bypass_flag==0)
        {
            //anc_iir_control_dac->codec_dac_L_iir_enable=1;
            anc_iir_control_dac->codec_dac_L_iir_enable=1;
        }
        else
        {
            //anc_iir_control_dac->codec_dac_L_iir_enable=0;
            anc_iir_control_dac->codec_dac_L_iir_enable=0;
        }

        if(dac_filtes_r->iir_bypass_flag==0)
        {
            anc_iir_control_dac->codec_dac_R_iir_enable=1;
        }
        else
        {
            anc_iir_control_dac->codec_dac_R_iir_enable=0;
        }
    }

    //adc iir settings
    if(hw_iir_type==HW_CODEC_IIR_ADC)
    {

        //disable all IIR filters
        anc_iir_a_control->codec_a_iir_enable = 0;

        //setting the adc IIR filters;
        if(adc_filtes_l->iir_counter>IIR_ADC_COUNTER)
        {
            anc_iir_a_control->codec_a_iir_count_ch0=IIR_ADC_COUNTER;
        }
        else if(adc_filtes_l->iir_counter>0)
        {
            anc_iir_a_control->codec_a_iir_count_ch0=adc_filtes_l->iir_counter;
        }
        else
        {
            anc_iir_control_adc0->codec_adc_ch0_iir_enable=0;
            LOG_I("Error adc_filtes_l IIR counter:%d", adc_filtes_l->iir_counter);
            err=HW_CODEC_IIR_COUNTER_ERR;
            return err;
        }

        if(adc_filtes_r->iir_counter>IIR_ADC_COUNTER)
        {
            anc_iir_a_control->codec_a_iir_count_ch1=IIR_ADC_COUNTER;
        }
        else if(adc_filtes_r->iir_counter>0)
        {
            anc_iir_a_control->codec_a_iir_count_ch1=adc_filtes_r->iir_counter;
        }
        else
        {
            anc_iir_control_adc1->codec_adc_ch1_iir_enable=0;
            LOG_I("Error adc_filtes_r IIR counter:%d", adc_filtes_r->iir_counter);
            err=HW_CODEC_IIR_COUNTER_ERR;
            return err;
        }


        LOG_I("set iir coef!");
        for(int i = 0;i < adc_filtes_r->iir_counter; i++)
        {
            anc_ff_iir_coefs0_l[i].a1 = -((int32_t)((adc_filtes_l->iir_coef[i].coef_a[1]/adc_filtes_l->iir_coef[i].coef_a[0])*(int32_t)(1<<27)));
            anc_ff_iir_coefs0_l[i].a2 = -((int32_t)((adc_filtes_l->iir_coef[i].coef_a[2]/adc_filtes_l->iir_coef[i].coef_a[0])*(int32_t)(1<<27)));
            anc_ff_iir_coefs0_l[i].b0 = (int32_t)((adc_filtes_l->iir_coef[i].coef_b[0]/adc_filtes_l->iir_coef[i].coef_a[0])*(int32_t)(1<<27));
            anc_ff_iir_coefs0_l[i].b1 = (int32_t)((adc_filtes_l->iir_coef[i].coef_b[1]/adc_filtes_l->iir_coef[i].coef_a[0])*(int32_t)(1<<27));
            anc_ff_iir_coefs0_l[i].b2 = (int32_t)((adc_filtes_l->iir_coef[i].coef_b[2]/adc_filtes_l->iir_coef[i].coef_a[0])*(int32_t)(1<<27));

            anc_ff_iir_coefs0_r[i].a1 = -((int32_t)((adc_filtes_r->iir_coef[i].coef_a[1]/adc_filtes_r->iir_coef[i].coef_a[0])*(int32_t)(1<<27)));
            anc_ff_iir_coefs0_r[i].a2 = -((int32_t)((adc_filtes_r->iir_coef[i].coef_a[2]/adc_filtes_r->iir_coef[i].coef_a[0])*(int32_t)(1<<27)));
            anc_ff_iir_coefs0_r[i].b0 = (int32_t)((adc_filtes_r->iir_coef[i].coef_b[0]/adc_filtes_r->iir_coef[i].coef_a[0])*(int32_t)(1<<27));
            anc_ff_iir_coefs0_r[i].b1 = (int32_t)((adc_filtes_r->iir_coef[i].coef_b[1]/adc_filtes_r->iir_coef[i].coef_a[0])*(int32_t)(1<<27));
            anc_ff_iir_coefs0_r[i].b2 = (int32_t)((adc_filtes_r->iir_coef[i].coef_b[2]/adc_filtes_r->iir_coef[i].coef_a[0])*(int32_t)(1<<27));
        }

        //enable all IIR filters
        anc_iir_a_control->codec_a_iir_enable=1;

        //enable hardware adc iir.
       if(adc_filtes_l->iir_bypass_flag==0)
        {
             anc_iir_control_adc0->codec_adc_ch0_iir_enable=1;
        }
        else
        {
             anc_iir_control_adc0->codec_adc_ch0_iir_enable=0;
        }

        if(adc_filtes_r->iir_bypass_flag==0)
        {
             anc_iir_control_adc1->codec_adc_ch1_iir_enable=1;
        }
        else
        {
             anc_iir_control_adc1->codec_adc_ch1_iir_enable=0;
        }
    }
    return err;
}



