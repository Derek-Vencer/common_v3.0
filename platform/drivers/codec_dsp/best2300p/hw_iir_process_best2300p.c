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
#include CHIP_SPECIFIC_HDR(reg_codec)

#include "iir_process.h"
#include "hw_iir_process.h"

#include "hal_timer.h"
#include "filter_debug_trace.h"
#include "string.h"
#include "hal_cmu.h"
#include "hal_dma.h"

/**********************************mocro**************************************/

typedef short int16;
typedef unsigned short uint16;
typedef int   int32;
typedef unsigned int   uint32;

typedef signed int                          hw_iir_sample_24bits_t;
typedef signed short int                    hw_iir_sample_16bits_t;

/*
4	08	codec_rx_threshold	RW	4	0	codec rx fifo threshold; sample count >= threshold, dma request trigger
4	08	codec_tx_threshold	RW	4	4
4	08	dsd_rx_threshold	RW	4	8
3	08	dsd_tx_threshold	RW	2	12
4	08	mc_threshold	RW	4	15
6	08	iir_rx_threshold	RW	16	19
6	08	iir_tx_threshold	RW	16	25
31	08		x
*/
struct _hw_iir_settings
{
	uint32 codec_rx_threshold : 4;
	uint32 codec_tx_threshold : 4;

	uint32 dsd_rx_threshold : 4;
	uint32 dsd_tx_threshold : 3;

	uint32 mc_threshold : 4;

	uint32 iir_rx_threshold : 6;
	uint32 iir_tx_threshold : 6;

	uint32 reserved : 1;
};

/*
1	128	ahb_iir_enable	RW	0	0	ahb iir enable
1	128	do_sample_ini	RW	1	1	only initial part specified by use_sample_high_half
1	128	use_sample_high_half	RW	0	2	
1	128	ahb_iir_if_en	RW	0	3	
1	128	dma_ctrl_rx_iir	RW	0	4	
1	128	dma_ctrl_tx_iir	RW	0	5	
1	128	mode_32bit_iir	RW	0	6	
1	128	mode_16bit_iir	RW	0	7	
4	128	ahb_iir_max_cnt	RW	0	8	
2	128	iir_multi_cycle_mode	RW	0	12	
14	128		x
*/
struct _hw_iir_control
{
	uint32 ahb_iir_enable : 1;	
    
	uint32 do_sample_ini : 1;
    
	uint32 use_sample_high_half : 1;
    
	uint32 ahb_iir_if_en : 1;
    
	uint32 dma_ctrl_rx_iir : 1;	
	uint32 dma_ctrl_tx_iir : 1;

	uint32 mode_32bit_iir : 1;
	uint32 mode_16bit_iir : 1;
    
  	uint32 ahb_iir_max_cnt : 4;
    
  	uint32 iir_multi_cycle_mode : 2;
    
	uint32 reserved : 18;
};

/*
iir coefficients(b/a Q27) 
a1
a2
b1
b2
b0
*/
struct _hw_iir_coefs
{
	int32 a1 ;
	int32 a2 ;
	
	int32 b1 ;
	int32 b2 ;	
	int32 b0 ;
};

/*******************************data struction***********************************/
#define HW_IIR_BASE                            ((uint32)CODEC_BASE)

//Hardware IIR registers
volatile static struct _hw_iir_settings *hw_iir_settings=(volatile struct _hw_iir_settings *)(HW_IIR_BASE+0x8);
volatile static struct _hw_iir_control *hw_iir_control=(volatile struct _hw_iir_control *)(HW_IIR_BASE+0x128);

volatile static struct _hw_iir_coefs *hw_iir_coefs=(volatile struct _hw_iir_coefs *)(HW_IIR_BASE+0x3000);

volatile static int hw_iir_open_flag=0;
volatile static int hw_iir_open_set_flag=0;
volatile static int hw_iir_finished_ch_index=0;
static volatile int hw_iir_finished_flag=0;

volatile static int hw_iir_left_init=0;
volatile static int hw_iir_right_init=0;

#define IIR_COUNTER (8)
#define HW_IIR_DMA_MEM_LEN    (4094)

#define HW_IIR_PARALLEL_RUN

#ifdef HW_IIR_PARALLEL_RUN

typedef enum {
    PP_PING = 0,
    PP_PANG = 1
} HW_IIR_PP_T;

typedef enum {
    HW_IIR_DATA_IDLE,
    HW_IIR_DATA_BUSY,
    HW_IIR_DATA_DONE,
} HW_IIR_DATA_STATE_T;

typedef struct {
    HW_IIR_DATA_STATE_T state;
    uint16_t len;
    int32_t *buf;
} HW_IIR_DATA_T;

#define PP_PINGPANG(v) \
    (v == PP_PING ? PP_PANG: PP_PING)

#endif

typedef struct{
#ifdef HW_IIR_PARALLEL_RUN
    HW_IIR_PP_T    pp;
    uint32_t    single_buf_num;
    HW_IIR_DATA_T  data[2];
#endif    
    uint32_t sample_rate;
    uint32_t sample_bits;
    enum AUD_CHANNEL_NUM_T ch_num;
} HW_IIR_RUN_CFG_T;

#define HW_IIR_EQ_DMA_LINKLIST_NUM_MAX (8)

static struct HAL_DMA_DESC_T hw_iir_dma_in_desc[HW_IIR_EQ_DMA_LINKLIST_NUM_MAX];
static struct HAL_DMA_DESC_T hw_iir_dma_out_desc[HW_IIR_EQ_DMA_LINKLIST_NUM_MAX];

static struct HAL_DMA_CH_CFG_T hw_iir_dma_cfg_in, hw_iir_dma_cfg_out;
static struct HAL_DMA_2D_CFG_T hw_iir_2d_src, hw_iir_2d_dst;

static HW_IIR_RUN_CFG_T hw_iir_run_cfg;

static const HW_IIR_FILTERS_T *hw_iir_filtes_l;
static const HW_IIR_FILTERS_T *hw_iir_filtes_r;

static void memcpy_samples(void *dest, void *src, int num)
{
    hw_iir_sample_24bits_t *sample_src = (hw_iir_sample_24bits_t *)src;
    hw_iir_sample_24bits_t *sample_dest = (hw_iir_sample_24bits_t *)dest;

    if(hw_iir_run_cfg.sample_bits == 16)
    {
        for (int i = 0; i < num/2; i++)
        {
            sample_dest[i] = sample_src[i];
        }
    }
    else
    {        
        for (int i = 0; i < num; i++)
        {
            sample_dest[i] = sample_src[i];
        }
    }

}

static void memset_samples(uint8_t *dest, int value, int num)
{
    if(hw_iir_run_cfg.sample_bits == 16)
    {
        hw_iir_sample_16bits_t *sample_dest = (hw_iir_sample_16bits_t *)dest;
        for (int i = 0; i < num; i++)
        {
            sample_dest[i] = value;
        }
    }
    else
    {
        // 24 or 32 bits
        hw_iir_sample_24bits_t *sample_dest = (hw_iir_sample_24bits_t *)dest;
        for (int i = 0; i < num; i++)
        {
            sample_dest[i] = value;
        }        
    }
}

// hardware iir start
static int hw_iir_start(void)
{
   // LOG_I("%s\n", __func__);




    hw_iir_control->ahb_iir_if_en=1;

    hw_iir_control->dma_ctrl_rx_iir=1;
    hw_iir_control->dma_ctrl_tx_iir=1;

    if(hw_iir_run_cfg.ch_num==1)
    {
        hw_iir_control->ahb_iir_enable=1;
        hal_audma_sg_start(&hw_iir_dma_out_desc[0], &hw_iir_dma_cfg_out);
        hal_audma_sg_start(&hw_iir_dma_in_desc[0], &hw_iir_dma_cfg_in);
    }
    else if(hw_iir_run_cfg.ch_num==2)
    {
        hw_iir_control->ahb_iir_enable=1;
        hal_dma_sg_2d_start(&hw_iir_dma_out_desc[0], &hw_iir_dma_cfg_out, NULL, &hw_iir_2d_dst);
        hal_dma_sg_2d_start(&hw_iir_dma_in_desc[0], &hw_iir_dma_cfg_in, &hw_iir_2d_src, NULL);
    }

	return 0;
}

// hardware iir stop
static int hw_iir_stop(void)
{
   // LOG_I("%s\n", __func__);

    hw_iir_control->ahb_iir_enable=0;

    hal_dma_stop(hw_iir_dma_cfg_in.ch);
    hal_dma_stop(hw_iir_dma_cfg_out.ch);

    hw_iir_control->ahb_iir_if_en=0;

    hw_iir_control->dma_ctrl_rx_iir=0;
    hw_iir_control->dma_ctrl_tx_iir=0;

    return 0;
}

static void hw_iir_irq_handler(uint8_t chan, uint32_t remain_tsize, uint32_t error, struct HAL_DMA_DESC_T *lli);

#define READ_REG(b,a) *(volatile uint32_t *)(b+a)
#define WRITE_REG(v,b,a) *(volatile uint32_t *)(b+a) = v

static int hw_iir_fill_sample(int32_t input[], int16_t total_num,int ch_index)
{  
    int link_num = 0;
    int loop = 0;
    int i = 0;
    int remainder_num;
    HW_IIR_ERROR err;

  //  LOG_I("%s,ch_index:%d.hw_iir_left_init:%d,hw_iir_right_init:%d.\n", __func__,ch_index, hw_iir_left_init,hw_iir_right_init);

    if(hw_iir_left_init==1&&ch_index==0)
    {
        hw_iir_control->do_sample_ini=1;
        hw_iir_left_init=0;
    }
    else if(hw_iir_right_init==1&&ch_index==1)
    {
        hw_iir_control->do_sample_ini=1;
        hw_iir_right_init=0;
    }
    else
    {
        hw_iir_control->do_sample_ini=0;
    }

    hw_iir_control->use_sample_high_half=ch_index;

    if(ch_index==0)
    {
        //setting the dac IIR filters;
        if(hw_iir_filtes_l->iir_counter>IIR_COUNTER)
        {
            hw_iir_control->ahb_iir_max_cnt=IIR_COUNTER;
        }
        else if(hw_iir_filtes_l->iir_counter>0)
        {
            hw_iir_control->ahb_iir_max_cnt=hw_iir_filtes_l->iir_counter;
        }
        else
        {
            hw_iir_control->ahb_iir_enable=0;
            ASSERT(false,"Error hw_iir_filtes IIR counter:%d", hw_iir_filtes_l->iir_counter);
            err=HW_IIR_COUNTER_ERR;
            return err;
        }

        //iir coefs settings
        for(i=0;i<IIR_COUNTER;i++)
        {
            hw_iir_coefs[i].a1=-hw_iir_filtes_l->iir_coef[i].coef_a[1];
            hw_iir_coefs[i].a2=-hw_iir_filtes_l->iir_coef[i].coef_a[2];
            hw_iir_coefs[i].b0=hw_iir_filtes_l->iir_coef[i].coef_b[0];
            hw_iir_coefs[i].b1=hw_iir_filtes_l->iir_coef[i].coef_b[1];
            hw_iir_coefs[i].b2=hw_iir_filtes_l->iir_coef[i].coef_b[2];
        }
    }
    else if(ch_index==1)
    {
        //setting the dac IIR filters;
        if(hw_iir_filtes_r->iir_counter>IIR_COUNTER)
        {
            hw_iir_control->ahb_iir_max_cnt=IIR_COUNTER;		
        }
        else if(hw_iir_filtes_r->iir_counter>0)
        {
            hw_iir_control->ahb_iir_max_cnt=hw_iir_filtes_r->iir_counter;
        }
        else
        {
            hw_iir_control->ahb_iir_enable=0;
            ASSERT(false,"Error hw_iir_filtes IIR counter:%d", hw_iir_filtes_r->iir_counter);
            err=HW_IIR_COUNTER_ERR;
            return err;
        }

        //iir coefs settings
        for(i=0;i<IIR_COUNTER;i++)
        {
            hw_iir_coefs[i].a1=-hw_iir_filtes_r->iir_coef[i].coef_a[1];
            hw_iir_coefs[i].a2=-hw_iir_filtes_r->iir_coef[i].coef_a[2];
            hw_iir_coefs[i].b0=hw_iir_filtes_r->iir_coef[i].coef_b[0];
            hw_iir_coefs[i].b1=hw_iir_filtes_r->iir_coef[i].coef_b[1];
            hw_iir_coefs[i].b2=hw_iir_filtes_r->iir_coef[i].coef_b[2];
        }
    }
    else
    {
         ASSERT(false, "%s: Invalid ch_index :%d", __func__, ch_index);
    }
    
//    LOG_I("ch in:%d,ch out:%d",hw_iir_dma_cfg_in.ch,hw_iir_dma_cfg_out.ch);

    if(hw_iir_run_cfg.ch_num==1)
    {
        loop = (total_num)/(HW_IIR_DMA_MEM_LEN);

        //config input DMA
        hw_iir_dma_cfg_in.dst = 0; // useless
        hw_iir_dma_cfg_in.dst_bsize = HAL_DMA_BSIZE_8;
        hw_iir_dma_cfg_in.dst_periph = HAL_AUDMA_IIR_TX;
        hw_iir_dma_cfg_in.handler = 0; //NULL
        hw_iir_dma_cfg_in.src_bsize = HAL_DMA_BSIZE_8;
        hw_iir_dma_cfg_in.src_periph = 0; // useless
        hw_iir_dma_cfg_in.try_burst = 1;
        hw_iir_dma_cfg_in.type = HAL_DMA_FLOW_M2P_DMA;
        hw_iir_dma_cfg_in.src_tsize=0;

        if(hw_iir_run_cfg.sample_bits==AUD_BITS_16)
        {
            hw_iir_dma_cfg_in.dst_width = HAL_DMA_WIDTH_HALFWORD;
            hw_iir_dma_cfg_in.src_width = HAL_DMA_WIDTH_HALFWORD;
        }
        else
        {
            hw_iir_dma_cfg_in.dst_width = HAL_DMA_WIDTH_WORD;
            hw_iir_dma_cfg_in.src_width = HAL_DMA_WIDTH_WORD;
        }

        link_num = 0;

        remainder_num=total_num-loop *HW_IIR_DMA_MEM_LEN;

        //LOG_I("remainder_num:%d",remainder_num);

        if(remainder_num>0)
        {
            loop++;
        }
        
        ASSERT(loop<=HW_IIR_EQ_DMA_LINKLIST_NUM_MAX, "%s: loop must less than 16 :%d", __func__, loop);

        for (i = 0; i < loop; i++) 
        {          
            if(hw_iir_run_cfg.sample_bits==AUD_BITS_16)
            {
                hw_iir_dma_cfg_in.src = (uint32_t)(input) + hw_iir_dma_cfg_in.src_tsize*i*2;
            }
            else
            {
                hw_iir_dma_cfg_in.src = (uint32_t)(input) + hw_iir_dma_cfg_in.src_tsize*i*4;
            }
            
            if(i==loop-1)
            {
                if(remainder_num>0)
                {
                    hw_iir_dma_cfg_in.src_tsize = remainder_num;
                }
                else
                {
                    hw_iir_dma_cfg_in.src_tsize = HW_IIR_DMA_MEM_LEN;
                }
                hal_audma_init_desc(&hw_iir_dma_in_desc[link_num], &hw_iir_dma_cfg_in, 0, 0); //last link
            }
            else
            {
                hw_iir_dma_cfg_in.src_tsize = HW_IIR_DMA_MEM_LEN;
                hal_audma_init_desc(&hw_iir_dma_in_desc[link_num], &hw_iir_dma_cfg_in, &hw_iir_dma_in_desc[link_num + 1], 0);
            }

            if(hw_iir_run_cfg.sample_bits==AUD_BITS_16)
            {
                hw_iir_dma_in_desc[link_num].ctrl=hw_iir_dma_in_desc[link_num].ctrl|(1<<25);
            }
            else
            {
                hw_iir_dma_in_desc[link_num].ctrl=hw_iir_dma_in_desc[link_num].ctrl&(~(1<<25));
            }                
            link_num++;
        }

        if(hw_iir_run_cfg.sample_bits==AUD_BITS_16)
        {
            uint32_t val;
            val = READ_REG(0x40120000,0x200+hw_iir_dma_cfg_in.ch*0x20+0x18); //dst modify 
            val &= 0xffff0000; //bit15:0 dst modify value 
            val |= 2;
            WRITE_REG(val,0x40120000,0x200+hw_iir_dma_cfg_in.ch*0x20+0x18);
        }
        else
        {
            uint32_t val;
            val = READ_REG(0x40120000,0x200+hw_iir_dma_cfg_in.ch*0x20+0x18); //dst modify 
            val &= 0xffff0000; //bit15:0 dst modify value 
            val |= 0;
            WRITE_REG(val,0x40120000,0x200+hw_iir_dma_cfg_in.ch*0x20+0x18);
        }

        //config out DMA
        hw_iir_dma_cfg_out.dst_bsize = HAL_DMA_BSIZE_8;
        hw_iir_dma_cfg_out.dst_periph = 0; //useless
        hw_iir_dma_cfg_out.handler = hw_iir_irq_handler;
        hw_iir_dma_cfg_out.src = 0; // useless
        hw_iir_dma_cfg_out.src_bsize = HAL_DMA_BSIZE_8;
        hw_iir_dma_cfg_out.src_periph = HAL_AUDMA_IIR_RX;
        hw_iir_dma_cfg_out.try_burst = 1;
        hw_iir_dma_cfg_out.type = HAL_DMA_FLOW_P2M_DMA;
        hw_iir_dma_cfg_out.src_tsize=0;


        if(hw_iir_run_cfg.sample_bits==AUD_BITS_16)
        {
            hw_iir_dma_cfg_out.dst_width = HAL_DMA_WIDTH_HALFWORD;
            hw_iir_dma_cfg_out.src_width = HAL_DMA_WIDTH_HALFWORD;
        }
        else
        {
            hw_iir_dma_cfg_out.dst_width = HAL_DMA_WIDTH_WORD;
            hw_iir_dma_cfg_out.src_width = HAL_DMA_WIDTH_WORD;
        }

        link_num = 0;

        for (i = 0; i < loop; i++)
        {

            if(hw_iir_run_cfg.sample_bits==AUD_BITS_16)
            {
               hw_iir_dma_cfg_out.dst = (uint32_t)(input) + hw_iir_dma_cfg_out.src_tsize*i*2;
            }
            else
            {
               hw_iir_dma_cfg_out.dst = (uint32_t)(input) + hw_iir_dma_cfg_out.src_tsize*i*4;
            }
            
            if(i==loop-1)
            {
                if(remainder_num>0)
                {
                    hw_iir_dma_cfg_out.src_tsize = remainder_num;
                }
                else
                {
                    hw_iir_dma_cfg_out.src_tsize = HW_IIR_DMA_MEM_LEN;
                }
                
                hal_audma_init_desc(&hw_iir_dma_out_desc[link_num], &hw_iir_dma_cfg_out, 0, 1);
            }
            else
            {
                hw_iir_dma_cfg_out.src_tsize = HW_IIR_DMA_MEM_LEN;
                hal_audma_init_desc(&hw_iir_dma_out_desc[link_num], &hw_iir_dma_cfg_out, &hw_iir_dma_out_desc[link_num + 1], 0);
            }
            if(hw_iir_run_cfg.sample_bits==AUD_BITS_16)
            {
                hw_iir_dma_out_desc[link_num].ctrl=hw_iir_dma_out_desc[link_num].ctrl| (1<<24);
            }
            else
            {
                hw_iir_dma_out_desc[link_num].ctrl=hw_iir_dma_out_desc[link_num].ctrl&(~(1<<24));
            }
            link_num++;
        }			

        if(hw_iir_run_cfg.sample_bits==AUD_BITS_16)
        {
            uint32_t val;            
            val = READ_REG(0x40120000,0x200+hw_iir_dma_cfg_out.ch*0x20+0x14); //src modify 
            val &= 0xffff0000; //bit15:0 src modify value
            val |= 2;
            WRITE_REG(val,0x40120000,0x200+hw_iir_dma_cfg_out.ch*0x20+0x14);
        }
        else
        {
            uint32_t val;            
            val = READ_REG(0x40120000,0x200+hw_iir_dma_cfg_out.ch*0x20+0x14); //src modify 
            val &= 0xffff0000; //bit15:0 src modify value
            val |= 0;
            WRITE_REG(val,0x40120000,0x200+hw_iir_dma_cfg_out.ch*0x20+0x14);
        }
    }
    else if(hw_iir_run_cfg.ch_num==AUD_CHANNEL_NUM_2)
    {
        total_num=total_num/2;
        loop = (total_num)/(HW_IIR_DMA_MEM_LEN);

        //config input DMA
        hw_iir_dma_cfg_in.dst = 0; // useless
        hw_iir_dma_cfg_in.dst_bsize = HAL_DMA_BSIZE_8;
        hw_iir_dma_cfg_in.dst_periph = HAL_AUDMA_IIR_TX;
        hw_iir_dma_cfg_in.handler = 0; //NULL
        hw_iir_dma_cfg_in.src_bsize = HAL_DMA_BSIZE_1;
        hw_iir_dma_cfg_in.src_periph = 0; // useless
        hw_iir_dma_cfg_in.try_burst = 0;
        hw_iir_dma_cfg_in.type = HAL_DMA_FLOW_M2P_DMA;
        hw_iir_dma_cfg_in.src_tsize=0;

        if(hw_iir_run_cfg.sample_bits==AUD_BITS_16)
        {
            hw_iir_dma_cfg_in.dst_width = HAL_DMA_WIDTH_HALFWORD;
            hw_iir_dma_cfg_in.src_width = HAL_DMA_WIDTH_HALFWORD;
        }
        else
        {
            hw_iir_dma_cfg_in.dst_width = HAL_DMA_WIDTH_WORD;
            hw_iir_dma_cfg_in.src_width = HAL_DMA_WIDTH_WORD;
        }

        link_num = 0;

        remainder_num=total_num-loop *HW_IIR_DMA_MEM_LEN;

        if(remainder_num>0)
        {
            loop++;
        }

        ASSERT(loop<=HW_IIR_EQ_DMA_LINKLIST_NUM_MAX, "%s: loop must less than 16 :%d", __func__, loop);        

        for (i = 0; i < loop; i++) 
        {
            if(hw_iir_run_cfg.sample_bits==AUD_BITS_16)
            {
                hw_iir_dma_cfg_in.src = (uint32_t)(input) + (hw_iir_dma_cfg_in.src_tsize*i+ch_index)*2;
            }
            else
            {
                hw_iir_dma_cfg_in.src = (uint32_t)(input) + (hw_iir_dma_cfg_in.src_tsize*i+ch_index)*4;
            }

            if(i==loop-1)
            {
                if(remainder_num>0)
                {
                    hw_iir_dma_cfg_in.src_tsize = remainder_num;
                }
                else
                {
                    hw_iir_dma_cfg_in.src_tsize = HW_IIR_DMA_MEM_LEN;
                }
                hal_audma_init_desc(&hw_iir_dma_in_desc[link_num], &hw_iir_dma_cfg_in, 0, 0); //last link
            }
            else
            {
                hw_iir_dma_cfg_in.src_tsize = HW_IIR_DMA_MEM_LEN;
                hal_audma_init_desc(&hw_iir_dma_in_desc[link_num], &hw_iir_dma_cfg_in, &hw_iir_dma_in_desc[link_num + 1], 0);
            }

            if(hw_iir_run_cfg.sample_bits==AUD_BITS_16)
            {
                hw_iir_dma_in_desc[link_num].ctrl=hw_iir_dma_in_desc[link_num].ctrl|(1<<25);
            }
            else
            {
                hw_iir_dma_in_desc[link_num].ctrl=hw_iir_dma_in_desc[link_num].ctrl&(~(1<<25));
            }                
            link_num++;
        }

        if(hw_iir_run_cfg.sample_bits==AUD_BITS_16)
        {
            uint32_t val;
            val = READ_REG(0x40120000,0x200+hw_iir_dma_cfg_in.ch*0x20+0x18); //dst modify 
            val &= 0xffff0000; //bit15:0 dst modify value 
            val |= 2;
            WRITE_REG(val,0x40120000,0x200+hw_iir_dma_cfg_in.ch*0x20+0x18);
        }
        else
        {
            uint32_t val;
            val = READ_REG(0x40120000,0x200+hw_iir_dma_cfg_in.ch*0x20+0x18); //dst modify 
            val &= 0xffff0000; //bit15:0 dst modify value 
            val |= 0;
            WRITE_REG(val,0x40120000,0x200+hw_iir_dma_cfg_in.ch*0x20+0x18);
        }

        //config out DMA
        hw_iir_dma_cfg_out.dst_bsize = HAL_DMA_BSIZE_1;
        hw_iir_dma_cfg_out.dst_periph = 0; //useless
        hw_iir_dma_cfg_out.handler = hw_iir_irq_handler;
        hw_iir_dma_cfg_out.src = 0; // useless
        hw_iir_dma_cfg_out.src_bsize = HAL_DMA_BSIZE_8;
        hw_iir_dma_cfg_out.src_periph = HAL_AUDMA_IIR_RX;
        hw_iir_dma_cfg_out.try_burst = 0;
        hw_iir_dma_cfg_out.type = HAL_DMA_FLOW_P2M_DMA;
        hw_iir_dma_cfg_out.src_tsize=0;

        if(hw_iir_run_cfg.sample_bits==AUD_BITS_16)
        {
            hw_iir_dma_cfg_out.dst_width = HAL_DMA_WIDTH_HALFWORD;
            hw_iir_dma_cfg_out.src_width = HAL_DMA_WIDTH_HALFWORD;
        }
        else
        {
            hw_iir_dma_cfg_out.dst_width = HAL_DMA_WIDTH_WORD;
            hw_iir_dma_cfg_out.src_width = HAL_DMA_WIDTH_WORD;
        }

        link_num = 0;

        for (i = 0; i < loop; i++)
        {

            if(hw_iir_run_cfg.sample_bits==AUD_BITS_16)
            {
               hw_iir_dma_cfg_out.dst = (uint32_t)(input) + (hw_iir_dma_cfg_out.src_tsize*i+ch_index)*2;
            }
            else
            {
               hw_iir_dma_cfg_out.dst = (uint32_t)(input) + (hw_iir_dma_cfg_out.src_tsize*i+ch_index)*4;
            }

            if(i==loop-1)
            {
                if(remainder_num>0)
                {
                    hw_iir_dma_cfg_out.src_tsize = remainder_num;
                }
                else
                {
                    hw_iir_dma_cfg_out.src_tsize = HW_IIR_DMA_MEM_LEN;
                }
                
                hal_audma_init_desc(&hw_iir_dma_out_desc[link_num], &hw_iir_dma_cfg_out, 0, 1);
            }
            else
            {
                hw_iir_dma_cfg_out.src_tsize = HW_IIR_DMA_MEM_LEN;
                hal_audma_init_desc(&hw_iir_dma_out_desc[link_num], &hw_iir_dma_cfg_out, &hw_iir_dma_out_desc[link_num + 1], 0);
            }
            if(hw_iir_run_cfg.sample_bits==AUD_BITS_16)
            {
                hw_iir_dma_out_desc[link_num].ctrl=hw_iir_dma_out_desc[link_num].ctrl| (1<<24);
            }
            else
            {
                hw_iir_dma_out_desc[link_num].ctrl=hw_iir_dma_out_desc[link_num].ctrl&(~(1<<24));
            }
            link_num++;
        }			

        if(hw_iir_run_cfg.sample_bits==AUD_BITS_16)
        {
            uint32_t val;            
            val = READ_REG(0x40120000,0x200+hw_iir_dma_cfg_out.ch*0x20+0x14); //src modify 
            val &= 0xffff0000; //bit15:0 src modify value
            val |= 2;
            WRITE_REG(val,0x40120000,0x200+hw_iir_dma_cfg_out.ch*0x20+0x14);
        }
        else
        {
            uint32_t val;            
            val = READ_REG(0x40120000,0x200+hw_iir_dma_cfg_out.ch*0x20+0x14); //src modify 
            val &= 0xffff0000; //bit15:0 src modify value
            val |= 0;
            WRITE_REG(val,0x40120000,0x200+hw_iir_dma_cfg_out.ch*0x20+0x14);
        }
        memset(&hw_iir_2d_src, 0, sizeof(hw_iir_2d_src));
        memset(&hw_iir_2d_dst, 0, sizeof(hw_iir_2d_dst));

        hw_iir_2d_src.xmodify = 1;
        hw_iir_2d_src.xcount = HW_IIR_DMA_MEM_LEN;
        hw_iir_2d_src.ymodify = 0;
        hw_iir_2d_src.ycount = 1;

        hw_iir_2d_dst.xmodify = 1;
        hw_iir_2d_dst.xcount = HW_IIR_DMA_MEM_LEN;
        hw_iir_2d_dst.ymodify = 0;
        hw_iir_2d_dst.ycount = 1;
    }
    else
    {
        ASSERT(hw_iir_run_cfg.ch_num < 2, "Bad channel number!: %u", hw_iir_run_cfg.ch_num);
    }
    //LOG_I("%s:%d loop %d, ch1 %d, ch2 %d\n", __func__, __LINE__, loop, hw_iir_dma_cfg_in.ch, hw_iir_dma_cfg_out.ch);
    return 0;
}

#ifdef HW_IIR_PARALLEL_RUN

static void hw_iir_irq_handler(uint8_t chan, uint32_t remain_tsize, uint32_t error, struct HAL_DMA_DESC_T *lli)
{
    HW_IIR_PP_T pp;
    int32_t *buf = NULL;
    int16_t len = 0;

    //LOG_I("[%s]:%d", __func__,hal_sys_timer_get());
    hw_iir_stop();  

    // init parameter
    pp = hw_iir_run_cfg.pp;
    buf = hw_iir_run_cfg.data[pp].buf;
    len = hw_iir_run_cfg.data[pp].len;

    if(hw_iir_finished_ch_index==1||hw_iir_run_cfg.ch_num==1) // OK!
    {
        hw_iir_run_cfg.data[pp].state = HW_IIR_DATA_DONE;
        hw_iir_finished_flag=1;
    }
    else // the other ch
    {
        hw_iir_finished_ch_index=1;        
        hw_iir_fill_sample(buf, len,1);
        hw_iir_start();
    }
    return;
}

int hw_iir_run(uint8_t *buf, uint32_t num)
{
    HW_IIR_PP_T pp_empty;
    HW_IIR_PP_T pp_full;
    HW_IIR_DATA_STATE_T state_full;

    if(hw_iir_open_set_flag==0)
    {
        LOG_I("[%s] Must set the correct config before running\n", __func__);
        return 1;
    }

    while(hw_iir_finished_flag==0)
    {
        LOG_I("[%s] waiting......:%d\n", __func__,hw_iir_finished_flag);
    }
    hw_iir_finished_flag=0;

    // LOG_I("%s,len:%d.\n", __func__, num);
    //LOG_I("[%s] len=%d pp=%d state=(%d,%d)", __func__, len, hw_iir_run_cfg.pp, hw_iir_run_cfg.data[0].state, hw_iir_run_cfg.data[1].state);
    ASSERT(num <= hw_iir_run_cfg.single_buf_num, "[%s] len(%u) > single_buf_num(%u)", __func__, num, hw_iir_run_cfg.single_buf_num);

    pp_full = hw_iir_run_cfg.pp;
    pp_empty = PP_PINGPANG(pp_full);
    // Make a copy of full state, because IRQ might change it
    state_full = hw_iir_run_cfg.data[pp_full].state;

    ASSERT(hw_iir_run_cfg.data[pp_empty].state == HW_IIR_DATA_IDLE, "Invalid pp_empty state: pp_empty=%d state_empty=%d state_full=%d)",
    pp_empty, hw_iir_run_cfg.data[pp_empty].state, state_full);
    
    // app --> eq       
    memcpy_samples(hw_iir_run_cfg.data[pp_empty].buf, buf, num);

    // eq --> af
    if (state_full == HW_IIR_DATA_DONE)
    {
        memcpy_samples(buf, hw_iir_run_cfg.data[pp_full].buf, num);
        hw_iir_run_cfg.data[pp_full].state = HW_IIR_DATA_IDLE;
    } 
    else
    {
        // HW_IIR_DATA_IDLE or HW_IIR_DATA_BUSY
        memset_samples(buf, 0, num);
        if (state_full == HW_IIR_DATA_BUSY) 
        {
            LOG_I("[%s] pp_full is no full!!!", __func__);
            // Skip this frame
            return 0;
        }
    } 

    // Update state
    hw_iir_finished_ch_index=0;
    hw_iir_run_cfg.data[pp_empty].len = num;
    hw_iir_run_cfg.data[pp_empty].state = HW_IIR_DATA_BUSY;
    hw_iir_run_cfg.pp = pp_empty;
    hw_iir_fill_sample(hw_iir_run_cfg.data[pp_empty].buf, hw_iir_run_cfg.data[pp_empty].len, 0);   
    hw_iir_start();
    

    return 0;
}

#else

static void hw_iir_irq_handler(uint8_t chan, uint32_t remain_tsize, uint32_t error, struct HAL_DMA_DESC_T *lli)
{
   // LOG_I("[%s]", __func__);
    hw_iir_finished_flag=1;
}

int hw_iir_run(uint8_t *buf, uint32_t num)
{
    uint32_t hw_iir_num = 0;
    int32_t *hw_iir_buf = NULL;

    LOG_I("%s,num:%d.\n", __func__, num);

    if(hw_iir_open_set_flag==0)
    {
        LOG_I("[%s] Must set the correct config before running\n", __func__);
        return 1;
    }

    hw_iir_num = num;
    hw_iir_buf = (int32_t *)buf;

    hw_iir_fill_sample(hw_iir_buf, hw_iir_num,0);
    hw_iir_finished_flag=0;
    hw_iir_start();
    while(hw_iir_finished_flag==0);
    hw_iir_stop();
    
    if(hw_iir_run_cfg.ch_num==AUD_CHANNEL_NUM_2)
    {
        hw_iir_fill_sample(hw_iir_buf, hw_iir_num,1);
        hw_iir_finished_flag=0;
        hw_iir_start();
        while(hw_iir_finished_flag==0);
        hw_iir_stop();
    }
    return 0;
}
#endif

int hw_iir_set_cfg(HW_IIR_CFG_T *cfg)
{
    HW_IIR_ERROR err=HW_IIR_NO_ERR;

    LOG_I("[%s]", __func__);

    if (cfg == NULL) 
    {
        LOG_I("%s: cfg is null!", __func__);
        return HW_IIR_OTHER_ERR;
    }

    if (hw_iir_open_flag==0) {
        LOG_I("%s: hardware iir not opened!", __func__);
        return HW_IIR_TYPE_ERR;
    }

    hw_iir_filtes_l=&(cfg->iir_filtes_l);
    hw_iir_filtes_r=&(cfg->iir_filtes_r);

     //disable IIR
    hw_iir_control->ahb_iir_enable=0;

    hw_iir_left_init=1;
    hw_iir_right_init=1;

    if(hw_iir_filtes_l->iir_counter>0&&hw_iir_filtes_r->iir_counter>0)
    {
        hw_iir_open_set_flag=1;
    }
    else
    {
        LOG_I("%s: the nub of filter must be >0!", __func__);
        hw_iir_open_set_flag=0;
    }
    

	return err;
}

int hw_iir_set_cfg_ch(const HW_IIR_FILTERS_T *iir_filters,enum AUD_CHANNEL_NUM_T ch)
{
    HW_IIR_ERROR err=HW_IIR_NO_ERR;

    LOG_I("[%s]", __func__);

    if (iir_filters == NULL)
    {
        LOG_I("%s: cfg is null!", __func__);
        return HW_IIR_OTHER_ERR;
    }

    if (hw_iir_open_flag==0) {
        LOG_I("%s: hardware iir not opened!", __func__);
        return HW_IIR_TYPE_ERR;
    }

    if (ch == AUD_CHANNEL_NUM_1) {
        hw_iir_filtes_l=iir_filters;
        hw_iir_left_init=1;
    } else if (ch == AUD_CHANNEL_NUM_2) {
        hw_iir_filtes_r=iir_filters;
        hw_iir_right_init=1;
    } else {
        LOG_I("%s: invalid channel!", __func__);
        return HW_IIR_OTHER_ERR;
    }

     //disable IIR
    hw_iir_control->ahb_iir_enable=0;

    if((ch == AUD_CHANNEL_NUM_1 && hw_iir_filtes_l->iir_counter>0)||
       (ch == AUD_CHANNEL_NUM_2 && hw_iir_filtes_r->iir_counter>0))
    {
        hw_iir_open_set_flag=1;
    }
    else
    {
        LOG_I("%s: the nub of filter must be >0!", __func__);
        hw_iir_open_set_flag=0;
    }

	return err;
}

int hw_iir_needed_size(enum AUD_BITS_T sample_bits, int32_t frame_size)
{
    int bytes_per_sample = (sample_bits <= AUD_BITS_16) ? 2 : 4;
    return frame_size * bytes_per_sample * 2;
}

int hw_iir_open(enum AUD_SAMPRATE_T sample_rate, enum AUD_BITS_T sample_bits,enum AUD_CHANNEL_NUM_T ch_num, void *eq_buf, uint32_t len)
{
    HW_IIR_ERROR err=HW_IIR_NO_ERR;

    LOG_I("[%s] sample_rate:%d,sample_bits:%d,ch_num:%d.", __func__,sample_rate,sample_bits,ch_num);

    hw_iir_run_cfg.sample_rate=sample_rate;
    hw_iir_run_cfg.sample_bits=sample_bits;
    hw_iir_run_cfg.ch_num=ch_num;

    // Parameter initialize
#ifdef HW_IIR_PARALLEL_RUN
    hw_iir_run_cfg.pp = PP_PING;
    hw_iir_run_cfg.data[0].state = HW_IIR_DATA_IDLE;
    hw_iir_run_cfg.data[1].state = HW_IIR_DATA_IDLE;

    ASSERT(eq_buf && ((uint32_t)eq_buf & 0x3) == 0, "%s: Invalid eq_buf=%p", __func__, eq_buf);
    ASSERT(len && (len & 0x3) == 0, "%s: Invalid eq_buf len=%u", __func__, len);

    if(hw_iir_run_cfg.sample_bits==AUD_BITS_16)
    {
        hw_iir_run_cfg.single_buf_num = (len /2)/2;
    }
    else
    {
        hw_iir_run_cfg.single_buf_num = (len /2)/ 4;
    }

    hw_iir_run_cfg.data[0].buf = eq_buf;
    hw_iir_run_cfg.data[1].buf = (int32_t *)((uint32_t)hw_iir_run_cfg.data[0].buf + len/2);
#endif

    //setting threshold of FIFO.
    hw_iir_settings->iir_rx_threshold=8;
    hw_iir_settings->iir_tx_threshold=16;

{
    uint32_t val;
    val = READ_REG(0x40300004,0);
    val |= CODEC_IIR_RX_FIFO_FLUSH|CODEC_IIR_TX_FIFO_FLUSH;
    WRITE_REG(val,0x40300004,0);

    val = READ_REG(0x40300004,0);
    val &= ~(CODEC_IIR_RX_FIFO_FLUSH|CODEC_IIR_TX_FIFO_FLUSH);
    WRITE_REG(val,0x40300004,0);
}

    //disable IIR
    hw_iir_control->ahb_iir_enable=0;
    
    hw_iir_control->do_sample_ini=0;

    hw_iir_control->use_sample_high_half=0;
    
    hw_iir_control->ahb_iir_if_en=1;
    
    hw_iir_control->dma_ctrl_rx_iir=1;
    hw_iir_control->dma_ctrl_tx_iir=1;

    if(hw_iir_run_cfg.sample_bits==AUD_BITS_16)
    {
        hw_iir_control->mode_32bit_iir=0;
        hw_iir_control->mode_16bit_iir=1;
    }
    else if(hw_iir_run_cfg.sample_bits==AUD_BITS_24)
    {
        hw_iir_control->mode_32bit_iir=0;
        hw_iir_control->mode_16bit_iir=0;
    }
    else if(hw_iir_run_cfg.sample_bits==AUD_BITS_32)
    {
        hw_iir_control->mode_32bit_iir=1;
        hw_iir_control->mode_16bit_iir=0;
    }
    else
    {
         ASSERT(false, "%s: Invalid sample bits:%d", __func__, sample_bits);
    }

    hw_iir_control->ahb_iir_max_cnt=0;
    
    hw_iir_control->iir_multi_cycle_mode=1;

    hw_iir_left_init=1;
    hw_iir_right_init=1;
    
    memset(&hw_iir_dma_cfg_in, 0, sizeof(hw_iir_dma_cfg_in));
    hw_iir_dma_cfg_in.ch = hal_audma_get_chan(HAL_AUDMA_IIR_TX, HAL_DMA_LOW_PRIO);

    memset(&hw_iir_dma_cfg_out, 0, sizeof(hw_iir_dma_cfg_out));
    hw_iir_dma_cfg_out.ch = hal_audma_get_chan(HAL_AUDMA_IIR_RX, HAL_DMA_HIGH_PRIO);

    hw_iir_open_flag=1;
    hw_iir_open_set_flag=0;
    hw_iir_finished_flag=1;

    return err;
}

int hw_iir_close(void)
{
    HW_IIR_ERROR err=HW_IIR_NO_ERR;
    
    LOG_I("%s", __func__);   
    if(hw_iir_open_flag==1)
    {   
        hw_iir_stop();

        hal_dma_free_chan(hw_iir_dma_cfg_in.ch);    
        hal_dma_free_chan(hw_iir_dma_cfg_out.ch);
        
        hw_iir_dma_cfg_in.ch=HAL_DMA_CHAN_NONE;
        hw_iir_dma_cfg_out.ch=HAL_DMA_CHAN_NONE;

        hw_iir_open_flag=0;

        //disable IIR
        hw_iir_control->ahb_iir_enable=0;
        
        hw_iir_control->do_sample_ini=0;

        hw_iir_control->use_sample_high_half=0;
        
        hw_iir_control->ahb_iir_if_en=0;
        
        hw_iir_control->dma_ctrl_rx_iir=0;
        hw_iir_control->dma_ctrl_tx_iir=0;

        hw_iir_control->mode_32bit_iir=0;
        hw_iir_control->mode_16bit_iir=0;
                
        hw_iir_control->ahb_iir_max_cnt=0;

        hw_iir_control->iir_multi_cycle_mode=0;
    }
    return err;
}
