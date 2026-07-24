/***************************************************************************
 *
 * Copyright 2015-2021 BES.
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

#include "mm_dbg.h"
#include "plat_addr_map.h"
#include "cmsis_nvic.h"
#include "hal_timer.h"
#include "filter_debug_trace.h"
#include "string.h"
#include "hal_cmu.h"
#include "fir_process.h"
#include "hal_dma.h"
#include "math.h"
#include "hal_codec.h"

typedef signed int                          fir_sample_24bits_t;
typedef signed short int                    fir_sample_16bits_t;

// Hardware fir filter parallel flag
#define HW_FIR_PARALLEL_RUN

// #define HW_FIR_MUTI_CHANNEL

#ifdef HW_FIR_MUTI_CHANNEL
#define HW_FIR_MAX_CHANNEL (4)
#else
#define HW_FIR_MAX_CHANNEL (2)
#endif

#define FIR_MEM_LEN                         512

enum FIR_CHNL_ID_T {
    FIR_CHNL_ID_0 = 0,
    FIR_CHNL_ID_1 ,
    FIR_CHNL_ID_2 ,
    FIR_CHNL_ID_3 ,
    FIR_CHNL_ID_QTY,
};

/*
2    6c    rtsel_rom                 RW    2    0     rtsel_rom
2    6c    ptsel_rom                 RW    1    2     ptsel_rom
2    6c    trb_rom                   RW    1    4     rtb_rom
4    6c    en_clk_iir_anc            RW    0    6
1    6c    en_clk_iir_eq             RW    0    10
4    6c    en_clk_fir                RW    0    11
2    6c    en_clk_psap               RW    0    15
3    6c    sel_i2s_mclk              RW    0    17
1    6c    en_i2s_mclk               RW    0    20
2    6c    cfg_div_codec_eqiir       RW    0    21    divider for iir
1    6c    bypass_div_codec_eqiir    RW    0    23    bypass_div_codec_iir
1    6c    sel_oscx4_eqiir           RW    0    24    iir clock select.1:oscx4;0:pll
1    6c    sel_oscx2_eqiir           RW    1    25    iir clock select.1:osc/oscx2;0:oscx4/pll
1    6c    sel_osc_eqiir             RW    0    26    iir clock select.1:osc;0:oscx2
1    6c    sel_osc_psap              RW    0    27    iir clock select.1:osc;0:oscx2
28   6c    x
*/
struct _anc_iir_clock
{
    uint32_t rtsel_rom : 2;
    uint32_t ptsel_rom : 2;
    uint32_t trb_rom : 2;

    uint32_t en_clk_iir_anc : 4;
    uint32_t en_clk_iir_eq : 1;
    uint32_t en_clk_fir : 4;
    uint32_t en_clk_psap : 2;

    uint32_t sel_i2s_mclk : 3;
    uint32_t en_i2s_mclk : 1;
    uint32_t cfg_div_codec_eqiir : 2;
    uint32_t bypass_div_codec_eqiir : 1;

    uint32_t sel_oscx4_eqiir : 1;
    uint32_t sel_oscx2_eqiir : 1;

    uint32_t sel_osc_eqiir : 1;
    uint32_t sel_osc_psap : 1;

    uint32_t reserved : 4;
};

/*
1     100    fir_stream_enable_ch0    RW    0    0     1: used for anc fir or adc downsample fir
1     100    fir_stream_enable_ch1    RW    0    1
1     100    fir_stream_enable_ch2    RW    0    2
1     100    fir_stream_enable_ch3    RW    0    3
1     100    fir_enable_ch0           RW    0    4     1: used for memory to memory fir
1     100    fir_enable_ch1           RW    0    5
1     100    fir_enable_ch2           RW    0    6
1     100    fir_enable_ch3           RW    0    7
1     100    dma_ctrl_rx_fir          RW    0    8
1     100    dma_ctrl_tx_fir          RW    0    9
1     100    fir_upsample_ch0         RW    0    10    1: use for memory to memory fir upsample
1     100    fir_upsample_ch1         RW    0    11
1     100    fir_upsample_ch2         RW    0    12
1     100    fir_upsample_ch3         RW    0    13
1     100    mode_32bit_fir           RW    0    14    all coef/sample to memory; not used together with any 16 bit mode
1     100    fir_reserved_reg0        RW    0    15    not used
1     100    mode_16bit_fir_tx_ch0    RW    0    16    sample memory write as 16bit mode
1     100    mode_16bit_fir_rx_ch0    RW    0    17    coef memory read as 16bit mode
1     100    mode_16bit_fir_tx_ch1    RW    0    18
1     100    mode_16bit_fir_rx_ch1    RW    0    19
1     100    mode_16bit_fir_tx_ch2    RW    0    20
1     100    mode_16bit_fir_rx_ch2    RW    0    21
1     100    mode_16bit_fir_tx_ch3    RW    0    22
1     100    mode_16bit_fir_rx_ch3    RW    0    23
24    100    x
*/
struct _fir_config0
{
    uint32_t fir_stream_enable_ch0 : 1;
    uint32_t fir_stream_enable_ch1 : 1;
    uint32_t fir_stream_enable_ch2 : 1;
    uint32_t fir_stream_enable_ch3 : 1;

    uint32_t fir_enable_ch0 : 1;
    uint32_t fir_enable_ch1 : 1;
    uint32_t fir_enable_ch2 : 1;
    uint32_t fir_enable_ch3 : 1;

    uint32_t dma_ctrl_rx_fir : 1;
    uint32_t dma_ctrl_tx_fir : 1;

    uint32_t fir_upsample_ch0 : 1;
    uint32_t fir_upsample_ch1 : 1;
    uint32_t fir_upsample_ch2 : 1;
    uint32_t fir_upsample_ch3 : 1;

    uint32_t mode_32bit_fir : 1;
    uint32_t fir_reserved_reg0 : 1;

    uint32_t mode_16bit_fir_tx_ch0 : 1;
    uint32_t mode_16bit_fir_rx_ch0 : 1;
    uint32_t mode_16bit_fir_tx_ch1 : 1;
    uint32_t mode_16bit_fir_rx_ch1 : 1;
    uint32_t mode_16bit_fir_tx_ch2 : 1;
    uint32_t mode_16bit_fir_rx_ch2 : 1;
    uint32_t mode_16bit_fir_tx_ch3 : 1;
    uint32_t mode_16bit_fir_rx_ch3 : 1;

    uint32_t Reserved : 8;
};

/*
5     64    soft_rstn_adc_ana    RW    31     0     soft reset of clk_adc_ana domain
8     64    soft_rstn_adc        RW    255    5     soft reset of clk_adc domain
1     64    soft_rstn_dac        RW    1      13    soft reset of clk_dac domain
1     64    soft_rstn_rs_adc     RW    1      14    soft reset of clk_rsadc domain
2     64    soft_rstn_rs_dac     RW    3      15    soft reset of clk_rsdac domain
4     64    soft_rstn_iir_anc    RW    15     17    soft reset of clk_iir domain
1     64    soft_rstn_iir_eq     RW    1      21    soft reset of clk_iir1 domain
4     64    soft_rstn_fir        RW    15     22    soft reset of clk_fir domain
2     64    soft_rstn_psap       RW    3      26
28    64    x
*/
struct _anc_soft_rstn
{
    uint32_t soft_rstn_adc_ana : 5;
    uint32_t soft_rstn_adc : 8;
    uint32_t soft_rstn_dac : 1;

    uint32_t soft_rstn_rs_adc : 1;
    uint32_t soft_rstn_rs_dac : 2;
    uint32_t soft_rstn_iir_anc : 4;

    uint32_t soft_rstn_iir_eq : 1;
    uint32_t soft_rstn_fir : 4;
    uint32_t soft_rstn_psap : 2;

    uint32_t reserved : 4;
};

/*
3     104    fir_access_offset_ch0    RW    0    0
3     104    fir_access_offset_ch1    RW    0    3
3     104    fir_access_offset_ch2    RW    0    6
3     104    fir_access_offset_ch3    RW    0    9
12    104    x
*/
struct _fir_config1
{
    uint32_t fir_access_offset_ch0 : 3;
    uint32_t fir_access_offset_ch1 : 3;
    uint32_t fir_access_offset_ch2 : 3;
    uint32_t fir_access_offset_ch3 : 3;

    uint32_t Reserved : 20;

};

/*
1     108    stream0_fir1_ch0        RW    0    0    0: stream mode; 1: fir mode
2     108    fir_mode_ch0            RW    0    1    fir operation mode;  0: normal fir;   1: synthesis1;  2: synthesis2;
10    108    fir_order_ch0           RW    0    3
9     108    fir_sample_start_ch0    RW    0    13
9     108    fir_sample_num_ch0      RW    0    22
1     108    fir_do_remap_ch0        RW    0    31
32    108    x
*/
struct _fir_ch_config0
{
    uint32_t stream0_fir1 : 1;
    uint32_t fir_mode : 2;
    uint32_t fir_order : 10;
    uint32_t fir_sample_start : 9;
    uint32_t fir_sample_num : 9;
    uint32_t fir_do_remap : 1;
};

/*
9     10c    fir_result_base_addr_ch0    RW    0    0
6     10c    fir_slide_offset_ch0        RW    0    9
6     10c    fir_burst_length_ch0        RW    0    15
4     10c    fir_gain_sel_ch0            RW    0    21
7     10c    fir_loop_num_ch0            RW    0    25
32    10c    x
*/
struct _fir_ch_config1
{
    uint32_t fir_result_base_addr : 9;
    uint32_t fir_slide_offset : 6;
    uint32_t fir_burst_length : 6;
    uint32_t fir_gain_sel : 4;
    uint32_t fir_loop_num : 7;
};

struct _fir_ch_config{
    volatile struct _fir_ch_config0 *fir_ch_config0;
    volatile struct _fir_ch_config1 *fir_ch_config1;
    volatile int32_t *sample;
    volatile int32_t *coef;
};

struct FIR_SETTINGS_T {
    volatile struct _fir_config0 *fir_config0;
    volatile struct _fir_config1 *fir_config1;
    volatile struct _fir_ch_config fir_ch_config[4];
};

#define FIR_BASE                            ((uint32_t)CODEC_BASE)

static volatile const struct FIR_SETTINGS_T fir_setings = {
    .fir_config0   = (volatile struct _fir_config0 *)(FIR_BASE + 0x100),
    .fir_config1   = (volatile struct _fir_config1 *)(FIR_BASE + 0x104),
    .fir_ch_config[0] = {
        .fir_ch_config0    = (volatile struct _fir_ch_config0 *)(FIR_BASE + 0x108),
        .fir_ch_config1   = (volatile struct _fir_ch_config1 *)(FIR_BASE + 0x10c),
        .sample =  (volatile int32_t *)(FIR_BASE + 0x8000),
        .coef     =  (volatile int32_t *)(FIR_BASE + 0x9000),
    },
    .fir_ch_config[1] = {
        .fir_ch_config0    = (volatile struct _fir_ch_config0 *)(FIR_BASE + 0x110),
        .fir_ch_config1   = (volatile struct _fir_ch_config1 *)(FIR_BASE + 0x114),
        .sample =  (volatile int32_t *)(FIR_BASE + 0xa000),
        .coef     =  (volatile int32_t *)(FIR_BASE + 0xb000),
    },
    .fir_ch_config[2] = {
        .fir_ch_config0    = (volatile struct _fir_ch_config0 *)(FIR_BASE + 0x118),
        .fir_ch_config1   = (volatile struct _fir_ch_config1 *)(FIR_BASE + 0x11c),
        .sample =  (volatile int32_t *)(FIR_BASE + 0xc000),
        .coef     =  (volatile int32_t *)(FIR_BASE + 0xd000),
    },
    .fir_ch_config[3] = {
        .fir_ch_config0    = (volatile struct _fir_ch_config0 *)(FIR_BASE + 0x120),
        .fir_ch_config1   = (volatile struct _fir_ch_config1 *)(FIR_BASE + 0x124),
        .sample =  (volatile int32_t *)(FIR_BASE + 0xe000),
        .coef     =  (volatile int32_t *)(FIR_BASE + 0xf000),
    },
};

//clock
volatile static struct _anc_iir_clock *anc_iir_clock=(volatile struct _anc_iir_clock *)(FIR_BASE+0x6c);
volatile static struct _anc_soft_rstn *anc_soft_rstn=(volatile struct _anc_soft_rstn *)(FIR_BASE+0x64);


#ifdef HW_FIR_PARALLEL_RUN

typedef enum {
    PP_PING = 0,
    PP_PANG = 1
} FIR_PP_T;

typedef enum {
    FIR_DATA_IDLE,
    FIR_DATA_BUSY,
    FIR_DATA_DONE,
} FIR_DATA_STATE_T;

typedef struct {
    FIR_DATA_STATE_T state;
    uint16_t eq_num;
    int16_t *eq_buf;
} FIR_DATA_T;

#define PP_PINGPANG(v) \
    (v == PP_PING ? PP_PANG: PP_PING)

#endif

typedef struct{
#ifdef HW_FIR_PARALLEL_RUN
    FIR_PP_T    pp;
    uint16_t    single_run_eq_buf_len;
    FIR_DATA_T  data[2];
#endif

    int16_t offset_num;
    int16_t proc_num;

    uint16_t coef_len[HW_FIR_MAX_CHANNEL];
    uint32_t sample_rate;
    uint32_t sample_bits;
    enum AUD_CHANNEL_NUM_T ch_num;
} FIR_RUN_CFG_T;

#define FIREQ_DMA_LINKLIST_NUM_MAX (24)

static struct HAL_DMA_DESC_T fir_dma_in_desc[FIREQ_DMA_LINKLIST_NUM_MAX];
static struct HAL_DMA_DESC_T fir_dma_out_desc[FIREQ_DMA_LINKLIST_NUM_MAX];

static struct HAL_DMA_CH_CFG_T fir_dma_cfg_in, fir_dma_cfg_out;
static struct HAL_DMA_2D_CFG_T fir_in_2d_src, fir_in_2d_dst;
static struct HAL_DMA_2D_CFG_T fir_out_2d_src, fir_out_2d_dst;

volatile static int hw_fir_open_flag=0;
static FIR_RUN_CFG_T fir_run_cfg;
static uint16_t fir_sample_start[HW_FIR_MAX_CHANNEL];

volatile static int fir_finished_flag=0;

// Used by fir filter input, fir filter just can deal with 32bits
static void memcpy_samples(void *dest, void *src, int num)
{
    fir_sample_24bits_t *sample_src = (fir_sample_24bits_t *)src;
    fir_sample_24bits_t *sample_dest = (fir_sample_24bits_t *)dest;

    if(fir_run_cfg.sample_bits == 16)
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
    if(fir_run_cfg.sample_bits == 16)
    {
        fir_sample_16bits_t *sample_dest = (fir_sample_16bits_t *)dest;
        for (int i = 0; i < num; i++)
        {
            sample_dest[i] = value;
        }
    }
    else
    {
        // 24 or 32 bits
        fir_sample_24bits_t *sample_dest = (fir_sample_24bits_t *)dest;
        for (int i = 0; i < num; i++)
        {
            sample_dest[i] = value;
        }
    }
}

static void coefcpy_int16_to_int32(volatile int32_t *dest, const int32_t *src, int16_t num,float gain)
{
    int16_t i;

    for (i = 0; i < num; i++)
    {
        dest[i] = (int32_t)((float)(src[i])*gain);
    }

    return;
}

static void coefset_int16_to_int32(volatile int32_t *dest, int16_t value, int16_t num)
{
    int16_t i;
    for (i = 0; i < num; i++)
    {
        dest[i] = value;
    }
    return;
}

// hardware
inline static int fir_start(void)
{
    //LOG_I("%s\n", __func__);
    if(fir_run_cfg.ch_num==1)
    {
        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {
            hal_dma_sg_2d_start(&fir_dma_in_desc[0], &fir_dma_cfg_in, NULL, &fir_in_2d_dst);
            hal_dma_sg_2d_start(&fir_dma_out_desc[0], &fir_dma_cfg_out, &fir_out_2d_src, NULL);
        }
        else
        {
            hal_audma_sg_start(&fir_dma_in_desc[0], &fir_dma_cfg_in);
            hal_audma_sg_start(&fir_dma_out_desc[0], &fir_dma_cfg_out);
        }

        fir_setings.fir_config0->fir_enable_ch0= 1 ;
    }
    else if(fir_run_cfg.ch_num==2)
    {

        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {
            hal_dma_sg_2d_start(&fir_dma_in_desc[0], &fir_dma_cfg_in, &fir_in_2d_src, &fir_in_2d_dst);
            hal_dma_sg_2d_start(&fir_dma_out_desc[0], &fir_dma_cfg_out, &fir_out_2d_src, &fir_out_2d_dst);
        }
        else
        {
            hal_dma_sg_2d_start(&fir_dma_in_desc[0], &fir_dma_cfg_in, &fir_in_2d_src, NULL);
            hal_dma_sg_2d_start(&fir_dma_out_desc[0], &fir_dma_cfg_out, NULL, &fir_out_2d_dst);
        }

        fir_setings.fir_config0->fir_enable_ch0= 1 ;
        fir_setings.fir_config0->fir_enable_ch1= 1 ;
    }
#ifdef HW_FIR_MUTI_CHANNEL
    else if(fir_run_cfg.ch_num==3)
    {

        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {
            hal_dma_sg_2d_start(&fir_dma_in_desc[0], &fir_dma_cfg_in, &fir_in_2d_src, &fir_in_2d_dst);
            hal_dma_sg_2d_start(&fir_dma_out_desc[0], &fir_dma_cfg_out, &fir_out_2d_src, &fir_out_2d_dst);
        }
        else
        {
            hal_dma_sg_2d_start(&fir_dma_in_desc[0], &fir_dma_cfg_in, &fir_in_2d_src, NULL);
            hal_dma_sg_2d_start(&fir_dma_out_desc[0], &fir_dma_cfg_out, NULL, &fir_out_2d_dst);
        }

        fir_setings.fir_config0->fir_enable_ch0= 1 ;
        fir_setings.fir_config0->fir_enable_ch1= 1 ;
        fir_setings.fir_config0->fir_enable_ch2= 1 ;
    }
    else if(fir_run_cfg.ch_num==4)
    {

        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {
            hal_dma_sg_2d_start(&fir_dma_in_desc[0], &fir_dma_cfg_in, &fir_in_2d_src, &fir_in_2d_dst);
            hal_dma_sg_2d_start(&fir_dma_out_desc[0], &fir_dma_cfg_out, &fir_out_2d_src, &fir_out_2d_dst);
        }
        else
        {
            hal_dma_sg_2d_start(&fir_dma_in_desc[0], &fir_dma_cfg_in, &fir_in_2d_src, NULL);
            hal_dma_sg_2d_start(&fir_dma_out_desc[0], &fir_dma_cfg_out, NULL, &fir_out_2d_dst);
        }

        fir_setings.fir_config0->fir_enable_ch0= 1 ;
        fir_setings.fir_config0->fir_enable_ch1= 1 ;
        fir_setings.fir_config0->fir_enable_ch2= 1 ;
        fir_setings.fir_config0->fir_enable_ch3= 1 ;
    }
#endif

    return 0;
}

inline static int fir_stop(void)
{
    //LOG_I("%s\n", __func__);
    if(fir_run_cfg.ch_num==1)
    {
        fir_setings.fir_config0->fir_enable_ch0= 0 ;
    }
    else if(fir_run_cfg.ch_num==2)
    {
        fir_setings.fir_config0->fir_enable_ch0= 0 ;
        fir_setings.fir_config0->fir_enable_ch1= 0 ;
    }
#ifdef HW_FIR_MUTI_CHANNEL
    else if(fir_run_cfg.ch_num==3)
    {
        fir_setings.fir_config0->fir_enable_ch0= 0 ;
        fir_setings.fir_config0->fir_enable_ch1= 0 ;
        fir_setings.fir_config0->fir_enable_ch2= 0 ;
    }
    else if(fir_run_cfg.ch_num==4)
    {
        fir_setings.fir_config0->fir_enable_ch0= 0 ;
        fir_setings.fir_config0->fir_enable_ch1= 0 ;
        fir_setings.fir_config0->fir_enable_ch2= 0 ;
        fir_setings.fir_config0->fir_enable_ch3= 0 ;
    }
#endif
    hal_dma_stop(fir_dma_cfg_in.ch);
    hal_dma_stop(fir_dma_cfg_out.ch);

    return 0;
}

static void fir_irq_handler(uint8_t chan, uint32_t remain_tsize, uint32_t error, struct HAL_DMA_DESC_T *lli);

#define READ_REG(b,a) *(volatile uint32_t *)(b+a)
#define WRITE_REG(v,b,a) *(volatile uint32_t *)(b+a) = v

inline static int fir_fill_sample(int16_t *input, int16_t total_num, int16_t *proc_num)
{
    int link_num = 0, loop = 0, i = 0;
    int coef_len_max=0;

    //LTER_DEBUG_TRACE(3,"%s,len:%d,ch:%d.\n", __func__, total_num,fir_run_cfg.ch_num);

    *proc_num=0;

    if(fir_run_cfg.ch_num==1)
    {
        fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_sample_start=fir_sample_start[FIR_CHNL_ID_0];

        loop = (total_num)/(FIR_MEM_LEN-fir_run_cfg.coef_len[FIR_CHNL_ID_0]);

        ASSERT(loop <= FIREQ_DMA_LINKLIST_NUM_MAX, "[%s] loop:%u <= FIREQ_DMA_LINKLIST_NUM_MAX:%d", __func__, loop,FIREQ_DMA_LINKLIST_NUM_MAX);

        //config input DMA
        fir_dma_cfg_in.dst = 0; // useless
        fir_dma_cfg_in.dst_bsize = HAL_DMA_BSIZE_1;
        fir_dma_cfg_in.dst_periph = HAL_AUDMA_FIR_TX;
        fir_dma_cfg_in.handler = 0; //NULL
        fir_dma_cfg_in.src_bsize = HAL_DMA_BSIZE_1;
        fir_dma_cfg_in.src_periph = 0; // useless
        fir_dma_cfg_in.try_burst = 1;
        fir_dma_cfg_in.type = HAL_DMA_FLOW_M2P_DI_DMA;

        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {
            fir_dma_cfg_in.dst_width = HAL_DMA_WIDTH_HALFWORD;
            fir_dma_cfg_in.src_width = HAL_DMA_WIDTH_HALFWORD;
        }
        else
        {
            fir_dma_cfg_in.dst_width = HAL_DMA_WIDTH_WORD;
            fir_dma_cfg_in.src_width = HAL_DMA_WIDTH_WORD;
        }

        if(loop!=0)
        {
            fir_dma_cfg_in.src_tsize = FIR_MEM_LEN-fir_run_cfg.coef_len[FIR_CHNL_ID_0];
            link_num = 0;

            for (i = 0; i < loop; i++)
            {
                if(fir_run_cfg.sample_bits==AUD_BITS_16)
                {
                    fir_dma_cfg_in.src = (uint32_t)(input) + fir_dma_cfg_in.src_tsize*i*2;
                }
                else
                {
                    fir_dma_cfg_in.src = (uint32_t)(input) + fir_dma_cfg_in.src_tsize*i*4;
                }

                if(i==loop-1)
                {
                    hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, 0, 0); //last link
                }
                else
                {
                    hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, &fir_dma_in_desc[link_num + 1], 0);
                }
                fir_dma_in_desc[link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].sample  +fir_sample_start[FIR_CHNL_ID_0]);
                fir_sample_start[FIR_CHNL_ID_0]=(fir_dma_cfg_in.src_tsize+ fir_sample_start[FIR_CHNL_ID_0])%FIR_MEM_LEN;
/*
                if(fir_run_cfg.sample_bits==AUD_BITS_16)
                {
                    fir_dma_in_desc[link_num].ctrl=fir_dma_in_desc[link_num].ctrl|(1<<25);
                }
                else
                {
                    fir_dma_in_desc[link_num].ctrl=fir_dma_in_desc[link_num].ctrl&(~(1<<25));
                }
*/
                link_num++;

            }

        }
        else
        {
            fir_dma_cfg_in.src_tsize =total_num;
            link_num = 0;

            fir_dma_cfg_in.src = (uint32_t)(input);
            hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, 0, 0); //disable interupt
            fir_dma_in_desc[link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].sample   + fir_sample_start[FIR_CHNL_ID_0]);
            fir_sample_start[FIR_CHNL_ID_0]=(fir_dma_cfg_in.src_tsize+ fir_sample_start[FIR_CHNL_ID_0])%FIR_MEM_LEN;
/*
            if(fir_run_cfg.sample_bits==AUD_BITS_16)
            {
                fir_dma_in_desc[link_num].ctrl=fir_dma_in_desc[link_num].ctrl|(1<<25);
            }
            else
            {
                fir_dma_in_desc[link_num].ctrl=fir_dma_in_desc[link_num].ctrl&(~(1<<25));
            }
*/
        }

        fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_sample_num= fir_dma_cfg_in.src_tsize;

        //config fir filter out.
        if(loop!=0)
        {
            fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->fir_loop_num=loop;
            *proc_num = loop *(FIR_MEM_LEN-fir_run_cfg.coef_len[FIR_CHNL_ID_0]);
        }
        else
        {
            fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->fir_loop_num=1;
            *proc_num = total_num;
        }
/*
        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {
            uint32_t val;
            val = READ_REG(0x40120000,0x200+fir_dma_cfg_in.ch*0x20+0x18); //dst modify
            val &= 0xffff0000; //bit15:0 dst modify value
            val |= 2;
            WRITE_REG(val,0x40120000,0x200+fir_dma_cfg_in.ch*0x20+0x18);
        }
        else
        {
            uint32_t val;
            val = READ_REG(0x40120000,0x200+fir_dma_cfg_in.ch*0x20+0x18); //dst modify
            val &= 0xffff0000; //bit15:0 dst modify value
            val |= 0;
            WRITE_REG(val,0x40120000,0x200+fir_dma_cfg_in.ch*0x20+0x18);
        }
*/
        //config out DMA
        fir_dma_cfg_out.dst_bsize = HAL_DMA_BSIZE_1;
        fir_dma_cfg_out.dst_periph = 0; //useless
        fir_dma_cfg_out.handler = fir_irq_handler;
        fir_dma_cfg_out.src = 0; // useless
        fir_dma_cfg_out.src_bsize = HAL_DMA_BSIZE_1;
        fir_dma_cfg_out.src_periph = HAL_AUDMA_FIR_RX;
        fir_dma_cfg_out.try_burst = 1;
        fir_dma_cfg_out.type = HAL_DMA_FLOW_P2M_SI_DMA;



        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {
            fir_dma_cfg_out.dst_width = HAL_DMA_WIDTH_HALFWORD;
            fir_dma_cfg_out.src_width = HAL_DMA_WIDTH_HALFWORD;
        }
        else
        {
            fir_dma_cfg_out.dst_width = HAL_DMA_WIDTH_WORD;
            fir_dma_cfg_out.src_width = HAL_DMA_WIDTH_WORD;
        }


        if(loop!=0)
        {
            fir_dma_cfg_out.src_tsize = FIR_MEM_LEN-fir_run_cfg.coef_len[FIR_CHNL_ID_0];
            link_num = 0;

            for (i = 0; i < loop; i++)
            {

                if(fir_run_cfg.sample_bits==AUD_BITS_16)
                {
                   fir_dma_cfg_out.dst = (uint32_t)(input) + fir_dma_cfg_out.src_tsize*i*2;
                }
                else
                {
                   fir_dma_cfg_out.dst = (uint32_t)(input) + fir_dma_cfg_out.src_tsize*i*4;
                }

                if(i==loop-1)
                {
                    hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, 0, 1); //enable interupt
                }
                else
                {
                    hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, &fir_dma_out_desc[link_num + 1], 0);
                }
                fir_dma_out_desc[link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].coef +fir_run_cfg.coef_len[FIR_CHNL_ID_0]);
/*
                if(fir_run_cfg.sample_bits==AUD_BITS_16)
                {
                    fir_dma_out_desc[link_num].ctrl=fir_dma_out_desc[link_num].ctrl| (1<<24);
                }
                else
                {
                    fir_dma_out_desc[link_num].ctrl=fir_dma_out_desc[link_num].ctrl&(~(1<<24));
                }
*/
                link_num++;
            }
        }
        else
        {
            fir_dma_cfg_out.src_tsize =total_num;
            link_num = 0;

            fir_dma_cfg_out.dst = (uint32_t)(input);
            hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, 0, 1); //enable interupt
            fir_dma_out_desc[link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].coef+fir_run_cfg.coef_len[FIR_CHNL_ID_0]);
/*
            if(fir_run_cfg.sample_bits==AUD_BITS_16)
            {
                fir_dma_out_desc[link_num].ctrl=fir_dma_out_desc[link_num].ctrl| (1<<24);
            }
            else
            {
                fir_dma_out_desc[link_num].ctrl=fir_dma_out_desc[link_num].ctrl&(~(1<<24));
            }
*/
        }
/*
        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {
            uint32_t val;
            val = READ_REG(0x40120000,0x200+fir_dma_cfg_out.ch*0x20+0x14); //src modify
            val &= 0xffff0000; //bit15:0 src modify value
            val |= 2;
            WRITE_REG(val,0x40120000,0x200+fir_dma_cfg_out.ch*0x20+0x14);
        }
        else
        {
            uint32_t val;
            val = READ_REG(0x40120000,0x200+fir_dma_cfg_out.ch*0x20+0x14); //src modify
            val &= 0xffff0000; //bit15:0 src modify value
            val |= 0;
            WRITE_REG(val,0x40120000,0x200+fir_dma_cfg_out.ch*0x20+0x14);
        }
        */
        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {

            memset(&fir_in_2d_dst, 0, sizeof(fir_in_2d_dst));
            memset(&fir_out_2d_src, 0, sizeof(fir_out_2d_src));

            fir_in_2d_dst.xmodify = 1;
            fir_in_2d_dst.xcount = FIR_MEM_LEN;
            fir_in_2d_dst.ymodify = 0;
            fir_in_2d_dst.ycount = 1;

            fir_out_2d_src.xmodify = 1;
            fir_out_2d_src.xcount = FIR_MEM_LEN;
            fir_out_2d_src.ymodify = 0;
            fir_out_2d_src.ycount = 1;
        }


    }
    else if(fir_run_cfg.ch_num==2)
    {
        fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_sample_start=fir_sample_start[FIR_CHNL_ID_0];
        fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->fir_sample_start=fir_sample_start[FIR_CHNL_ID_1];

        coef_len_max=0;
        if(coef_len_max<fir_run_cfg.coef_len[FIR_CHNL_ID_0])
        {
            coef_len_max=fir_run_cfg.coef_len[FIR_CHNL_ID_0];
        }
        if(coef_len_max<fir_run_cfg.coef_len[FIR_CHNL_ID_1])
        {
            coef_len_max=fir_run_cfg.coef_len[FIR_CHNL_ID_1];
        }

        loop = (total_num)/(FIR_MEM_LEN-coef_len_max);

        ASSERT(loop <= FIREQ_DMA_LINKLIST_NUM_MAX, "[%s] loop:%u <= FIREQ_DMA_LINKLIST_NUM_MAX:%d", __func__, loop,FIREQ_DMA_LINKLIST_NUM_MAX);

        loop=loop/fir_run_cfg.ch_num;

        //config input DMA
        fir_dma_cfg_in.dst = 0; // useless
        fir_dma_cfg_in.dst_bsize = HAL_DMA_BSIZE_1;
        fir_dma_cfg_in.dst_periph = HAL_AUDMA_FIR_TX;
        fir_dma_cfg_in.handler = 0; //NULL
        fir_dma_cfg_in.src_bsize = HAL_DMA_BSIZE_1;
        fir_dma_cfg_in.src_periph = 0; // useless
        fir_dma_cfg_in.try_burst = 1;
        fir_dma_cfg_in.type = HAL_DMA_FLOW_M2P_DI_DMA;

        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {
            fir_dma_cfg_in.dst_width = HAL_DMA_WIDTH_HALFWORD;
            fir_dma_cfg_in.src_width = HAL_DMA_WIDTH_HALFWORD;
        }
        else
        {
            fir_dma_cfg_in.dst_width = HAL_DMA_WIDTH_WORD;
            fir_dma_cfg_in.src_width = HAL_DMA_WIDTH_WORD;
        }

        if(loop!=0)
        {
            fir_dma_cfg_in.src_tsize = FIR_MEM_LEN-coef_len_max;
            link_num = 0;

            for (i = 0; i < loop; i++)
            {

                if(fir_run_cfg.sample_bits==AUD_BITS_16)
                {
                    fir_dma_cfg_in.src = (uint32_t)(input) + fir_dma_cfg_in.src_tsize*fir_run_cfg.ch_num*i*2;
                }
                else
                {
                    fir_dma_cfg_in.src = (uint32_t)(input) + fir_dma_cfg_in.src_tsize*fir_run_cfg.ch_num*i*4;
                }

                hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, &fir_dma_in_desc[link_num + 1], 0);
                fir_dma_in_desc[link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].sample + fir_sample_start[FIR_CHNL_ID_0]);
                fir_sample_start[FIR_CHNL_ID_0]=(fir_dma_cfg_in.src_tsize+ fir_sample_start[FIR_CHNL_ID_0])%FIR_MEM_LEN;
/*
                if(fir_run_cfg.sample_bits==AUD_BITS_16)
                {
                    fir_dma_in_desc[link_num].ctrl=fir_dma_in_desc[link_num].ctrl|(1<<25);
                }
                else
                {
                    fir_dma_in_desc[link_num].ctrl=fir_dma_in_desc[link_num].ctrl&(~(1<<25));
                }
*/
                link_num++;


                if(fir_run_cfg.sample_bits==AUD_BITS_16)
                {
                    fir_dma_cfg_in.src = (uint32_t)(input) + (fir_dma_cfg_in.src_tsize*fir_run_cfg.ch_num*i+1)*2;
                }
                else
                {
                    fir_dma_cfg_in.src = (uint32_t)(input) + (fir_dma_cfg_in.src_tsize*fir_run_cfg.ch_num*i+1)*4;
                }

                if(i==loop-1)
                {
                    hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, 0, 0); //disable interupt
                }
                else
                {
                    hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, &fir_dma_in_desc[link_num + 1], 0);
                }
                fir_dma_in_desc[link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_1].sample  + fir_sample_start[FIR_CHNL_ID_1]);
                fir_sample_start[FIR_CHNL_ID_1]=(fir_dma_cfg_in.src_tsize+ fir_sample_start[FIR_CHNL_ID_1])%FIR_MEM_LEN;
/*
                if(fir_run_cfg.sample_bits==AUD_BITS_16)
                {
                    fir_dma_in_desc[link_num].ctrl=fir_dma_in_desc[link_num].ctrl|(1<<25);
                }
                else
                {
                    fir_dma_in_desc[link_num].ctrl=fir_dma_in_desc[link_num].ctrl&(~(1<<25));
                }
*/

                link_num++;

            }

        }
        else
        {
            fir_dma_cfg_in.src_tsize =total_num/fir_run_cfg.ch_num;
            link_num = 0;

            fir_dma_cfg_in.src = (uint32_t)(input);
            hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, &fir_dma_in_desc[link_num + 1], 0); //disable interupt
            fir_dma_in_desc[link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].sample   + fir_sample_start[FIR_CHNL_ID_0]);
            fir_sample_start[FIR_CHNL_ID_0]=(fir_dma_cfg_in.src_tsize+ fir_sample_start[FIR_CHNL_ID_0])%FIR_MEM_LEN;
/*
            if(fir_run_cfg.sample_bits==AUD_BITS_16)
            {
                fir_dma_in_desc[link_num].ctrl=fir_dma_in_desc[link_num].ctrl|(1<<25);
            }
            else
            {
                fir_dma_in_desc[link_num].ctrl=fir_dma_in_desc[link_num].ctrl&(~(1<<25));
            }
*/
            link_num++;
            if(fir_run_cfg.sample_bits==AUD_BITS_16)
            {
                fir_dma_cfg_in.src = (uint32_t)(input)+1*2;
            }
            else
            {
                fir_dma_cfg_in.src = (uint32_t)(input)+1*4;
            }
            hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, 0, 0); //disable interupt
            fir_dma_in_desc[link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_1].sample   + fir_sample_start[FIR_CHNL_ID_1]);
            fir_sample_start[FIR_CHNL_ID_1]=(fir_dma_cfg_in.src_tsize+ fir_sample_start[FIR_CHNL_ID_1])%FIR_MEM_LEN;
/*
            if(fir_run_cfg.sample_bits==AUD_BITS_16)
            {
                fir_dma_in_desc[link_num].ctrl=fir_dma_in_desc[link_num].ctrl|(1<<25);
            }
            else
            {
                fir_dma_in_desc[link_num].ctrl=fir_dma_in_desc[link_num].ctrl&(~(1<<25));
            }
*/
        }

        fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_sample_num= fir_dma_cfg_in.src_tsize;
        fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->fir_sample_num= fir_dma_cfg_in.src_tsize;

        //config fir filter out.
        if(loop!=0)
        {
            fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->fir_loop_num=loop;
            fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config1->fir_loop_num=loop;

            *proc_num = loop *(FIR_MEM_LEN-coef_len_max)*fir_run_cfg.ch_num;
        }
        else
        {
            fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->fir_loop_num=1;
            fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config1->fir_loop_num=1;

            *proc_num = total_num;
        }
/*
        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {
            uint32_t val;
            val = READ_REG(0x40120000,0x200+fir_dma_cfg_in.ch*0x20+0x18); //dst modify
            val &= 0xffff0000; //bit15:0 dst modify value
            val |= 2;
            WRITE_REG(val,0x40120000,0x200+fir_dma_cfg_in.ch*0x20+0x18);
        }
        else
        {
            uint32_t val;
            val = READ_REG(0x40120000,0x200+fir_dma_cfg_in.ch*0x20+0x18); //dst modify
            val &= 0xffff0000; //bit15:0 dst modify value
            val |= 0;
            WRITE_REG(val,0x40120000,0x200+fir_dma_cfg_in.ch*0x20+0x18);
        }
*/

        //config out DMA
        fir_dma_cfg_out.dst_bsize = HAL_DMA_BSIZE_1;
        fir_dma_cfg_out.dst_periph = 0; //useless
        fir_dma_cfg_out.handler = fir_irq_handler;
        fir_dma_cfg_out.src = 0; // useless
        fir_dma_cfg_out.src_bsize = HAL_DMA_BSIZE_1;
        fir_dma_cfg_out.src_periph = HAL_AUDMA_FIR_RX;
        fir_dma_cfg_out.try_burst = 1;
        fir_dma_cfg_out.type = HAL_DMA_FLOW_P2M_SI_DMA;


        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {
            fir_dma_cfg_out.dst_width = HAL_DMA_WIDTH_HALFWORD;
            fir_dma_cfg_out.src_width = HAL_DMA_WIDTH_HALFWORD;
        }
        else
        {
            fir_dma_cfg_out.dst_width = HAL_DMA_WIDTH_WORD;
            fir_dma_cfg_out.src_width = HAL_DMA_WIDTH_WORD;
        }


        if(loop!=0)
        {
            fir_dma_cfg_out.src_tsize = FIR_MEM_LEN-coef_len_max;
            link_num = 0;

            for (i = 0; i < loop; i++)
            {
                if(fir_run_cfg.sample_bits==AUD_BITS_16)
                {
                    fir_dma_cfg_out.dst = (uint32_t)(input) + fir_dma_cfg_out.src_tsize*fir_run_cfg.ch_num*i*2;
                }
                else
                {
                    fir_dma_cfg_out.dst = (uint32_t)(input) + fir_dma_cfg_out.src_tsize*fir_run_cfg.ch_num*i*4;
                }

                hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, &fir_dma_out_desc[link_num + 1], 0);
                fir_dma_out_desc[link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].coef +fir_run_cfg.coef_len[FIR_CHNL_ID_0]);
/*
                if(fir_run_cfg.sample_bits==AUD_BITS_16)
                {
                    fir_dma_out_desc[link_num].ctrl=fir_dma_out_desc[link_num].ctrl| (1<<24);
                }
                else
                {
                    fir_dma_out_desc[link_num].ctrl=fir_dma_out_desc[link_num].ctrl&(~(1<<24));
                }
*/
                link_num++;
                if(fir_run_cfg.sample_bits==AUD_BITS_16)
                {
                    fir_dma_cfg_out.dst = (uint32_t)(input) + (fir_dma_cfg_out.src_tsize*fir_run_cfg.ch_num*i+1)*2;
                }
                else
                {
                    fir_dma_cfg_out.dst = (uint32_t)(input) + (fir_dma_cfg_out.src_tsize*fir_run_cfg.ch_num*i+1)*4;
                }

                if(i==loop-1)
                {
                    hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, 0, 1); //enable interupt
                }
                else
                {
                    hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, &fir_dma_out_desc[link_num + 1], 0);
                }
                fir_dma_out_desc[link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_1].coef +fir_run_cfg.coef_len[FIR_CHNL_ID_1]);
/*
                if(fir_run_cfg.sample_bits==AUD_BITS_16)
                {
                    fir_dma_out_desc[link_num].ctrl=fir_dma_out_desc[link_num].ctrl| (1<<24);
                }
                else
                {
                    fir_dma_out_desc[link_num].ctrl=fir_dma_out_desc[link_num].ctrl&(~(1<<24));
                }
*/
                link_num++;
            }

        }
        else
        {
            fir_dma_cfg_out.src_tsize =total_num/fir_run_cfg.ch_num;
            link_num = 0;

            fir_dma_cfg_out.dst = (uint32_t)(input);
            hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, &fir_dma_out_desc[link_num + 1], 0); //disable interupt
            fir_dma_out_desc[link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].coef+fir_run_cfg.coef_len[FIR_CHNL_ID_0]);
/*
            if(fir_run_cfg.sample_bits==AUD_BITS_16)
            {
                fir_dma_out_desc[link_num].ctrl=fir_dma_out_desc[link_num].ctrl| (1<<24);
            }
            else
            {
                fir_dma_out_desc[link_num].ctrl=fir_dma_out_desc[link_num].ctrl&(~(1<<24));
            }
*/
            link_num++;

            if(fir_run_cfg.sample_bits==AUD_BITS_16)
            {
                fir_dma_cfg_out.dst = (uint32_t)(input) + 1*2;
            }
            else
            {
                fir_dma_cfg_out.dst = (uint32_t)(input) + 1*4;
            }

            hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, 0, 1); //enable interupt
            fir_dma_out_desc[link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_1].coef+fir_run_cfg.coef_len[FIR_CHNL_ID_1]);
/*
            if(fir_run_cfg.sample_bits==AUD_BITS_16)
            {
                fir_dma_out_desc[link_num].ctrl=fir_dma_out_desc[link_num].ctrl| (1<<24);
            }
            else
            {
                fir_dma_out_desc[link_num].ctrl=fir_dma_out_desc[link_num].ctrl&(~(1<<24));
            }
*/
        }
/*
        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {
            uint32_t val;
            val = READ_REG(0x40120000,0x200+fir_dma_cfg_out.ch*0x20+0x14); //src modify
            val &= 0xffff0000; //bit15:0 src modify value
            val |= 2;
            WRITE_REG(val,0x40120000,0x200+fir_dma_cfg_out.ch*0x20+0x14);
        }
        else
        {
            uint32_t val;
            val = READ_REG(0x40120000,0x200+fir_dma_cfg_out.ch*0x20+0x14); //src modify
            val &= 0xffff0000; //bit15:0 src modify value
            val |= 0;
            WRITE_REG(val,0x40120000,0x200+fir_dma_cfg_out.ch*0x20+0x14);
        }
*/
        memset(&fir_in_2d_src, 0, sizeof(fir_in_2d_src));
        memset(&fir_out_2d_dst, 0, sizeof(fir_out_2d_dst));
        fir_in_2d_src.xmodify = fir_run_cfg.ch_num-1;
        fir_in_2d_src.xcount = FIR_MEM_LEN;
        fir_in_2d_src.ymodify = 0;
        fir_in_2d_src.ycount = 1;

        fir_out_2d_dst.xmodify = fir_run_cfg.ch_num-1;
        fir_out_2d_dst.xcount = FIR_MEM_LEN;
        fir_out_2d_dst.ymodify = 0;
        fir_out_2d_dst.ycount = 1;

        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {
            memset(&fir_in_2d_dst, 0, sizeof(fir_in_2d_dst));
            memset(&fir_out_2d_src, 0, sizeof(fir_out_2d_src));

            fir_in_2d_dst.xmodify = 1;
            fir_in_2d_dst.xcount = FIR_MEM_LEN;
            fir_in_2d_dst.ymodify = 0;
            fir_in_2d_dst.ycount = 1;

            fir_out_2d_src.xmodify = 1;
            fir_out_2d_src.xcount = FIR_MEM_LEN;
            fir_out_2d_src.ymodify = 0;
            fir_out_2d_src.ycount = 1;
        }
    }
#ifdef HW_FIR_MUTI_CHANNEL
    else if(fir_run_cfg.ch_num==3)
    {
        fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_sample_start=fir_sample_start[FIR_CHNL_ID_0];
        fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->fir_sample_start=fir_sample_start[FIR_CHNL_ID_1];
        fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config0->fir_sample_start=fir_sample_start[FIR_CHNL_ID_2];

        coef_len_max=0;
        if(coef_len_max<fir_run_cfg.coef_len[FIR_CHNL_ID_0])
        {
            coef_len_max=fir_run_cfg.coef_len[FIR_CHNL_ID_0];
        }
        if(coef_len_max<fir_run_cfg.coef_len[FIR_CHNL_ID_1])
        {
            coef_len_max=fir_run_cfg.coef_len[FIR_CHNL_ID_1];
        }
        if(coef_len_max<fir_run_cfg.coef_len[FIR_CHNL_ID_2])
        {
            coef_len_max=fir_run_cfg.coef_len[FIR_CHNL_ID_2];
        }

        loop = (total_num)/(FIR_MEM_LEN-coef_len_max);

        ASSERT(loop <= FIREQ_DMA_LINKLIST_NUM_MAX, "[%s] loop:%u <= FIREQ_DMA_LINKLIST_NUM_MAX:%d", __func__, loop,FIREQ_DMA_LINKLIST_NUM_MAX);

        loop=loop/fir_run_cfg.ch_num;

        //config input DMA
        fir_dma_cfg_in.dst = 0; // useless
        fir_dma_cfg_in.dst_bsize = HAL_DMA_BSIZE_1;
        fir_dma_cfg_in.dst_periph = HAL_AUDMA_FIR_TX;
        fir_dma_cfg_in.handler = 0; //NULL
        fir_dma_cfg_in.src_bsize = HAL_DMA_BSIZE_1;
        fir_dma_cfg_in.src_periph = 0; // useless
        fir_dma_cfg_in.try_burst = 1;
        fir_dma_cfg_in.type = HAL_DMA_FLOW_M2P_DI_DMA;

        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {
            fir_dma_cfg_in.dst_width = HAL_DMA_WIDTH_HALFWORD;
            fir_dma_cfg_in.src_width = HAL_DMA_WIDTH_HALFWORD;
        }
        else
        {
            fir_dma_cfg_in.dst_width = HAL_DMA_WIDTH_WORD;
            fir_dma_cfg_in.src_width = HAL_DMA_WIDTH_WORD;
        }

        if(loop!=0)
        {
            fir_dma_cfg_in.src_tsize = FIR_MEM_LEN-coef_len_max;
            link_num = 0;

            for (i = 0; i < loop; i++)
            {
                //ch 0
                if(fir_run_cfg.sample_bits==AUD_BITS_16)
                {
                    fir_dma_cfg_in.src = (uint32_t)(input) + fir_dma_cfg_in.src_tsize*fir_run_cfg.ch_num*i*2;
                }
                else
                {
                    fir_dma_cfg_in.src = (uint32_t)(input) + fir_dma_cfg_in.src_tsize*fir_run_cfg.ch_num*i*4;
                }

                hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, &fir_dma_in_desc[link_num + 1], 0);
                fir_dma_in_desc[link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].sample + fir_sample_start[FIR_CHNL_ID_0]);
                fir_sample_start[FIR_CHNL_ID_0]=(fir_dma_cfg_in.src_tsize+ fir_sample_start[FIR_CHNL_ID_0])%FIR_MEM_LEN;

                link_num++;

                //ch 1
                if(fir_run_cfg.sample_bits==AUD_BITS_16)
                {
                    fir_dma_cfg_in.src = (uint32_t)(input) + (fir_dma_cfg_in.src_tsize*fir_run_cfg.ch_num*i+1)*2;
                }
                else
                {
                    fir_dma_cfg_in.src = (uint32_t)(input) + (fir_dma_cfg_in.src_tsize*fir_run_cfg.ch_num*i+1)*4;
                }


                hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, &fir_dma_in_desc[link_num + 1], 0);
                fir_dma_in_desc[link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_1].sample  + fir_sample_start[FIR_CHNL_ID_1]);
                fir_sample_start[FIR_CHNL_ID_1]=(fir_dma_cfg_in.src_tsize+ fir_sample_start[FIR_CHNL_ID_1])%FIR_MEM_LEN;

                link_num++;

                //ch 2
                if(fir_run_cfg.sample_bits==AUD_BITS_16)
                {
                    fir_dma_cfg_in.src = (uint32_t)(input) + (fir_dma_cfg_in.src_tsize*fir_run_cfg.ch_num*i+2)*2;
                }
                else
                {
                    fir_dma_cfg_in.src = (uint32_t)(input) + (fir_dma_cfg_in.src_tsize*fir_run_cfg.ch_num*i+2)*4;
                }

                if(i==loop-1)
                {
                    hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, 0, 0); //disable interupt
                }
                else
                {
                    hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, &fir_dma_in_desc[link_num + 1], 0);
                }
                fir_dma_in_desc[link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_2].sample  + fir_sample_start[FIR_CHNL_ID_2]);
                fir_sample_start[FIR_CHNL_ID_2]=(fir_dma_cfg_in.src_tsize+ fir_sample_start[FIR_CHNL_ID_2])%FIR_MEM_LEN;

                link_num++;
            }

        }
        else
        {
            fir_dma_cfg_in.src_tsize =total_num/fir_run_cfg.ch_num;
            link_num = 0;

            fir_dma_cfg_in.src = (uint32_t)(input);
            hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, &fir_dma_in_desc[link_num + 1], 0); //disable interupt
            fir_dma_in_desc[link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].sample   + fir_sample_start[FIR_CHNL_ID_0]);
            fir_sample_start[FIR_CHNL_ID_0]=(fir_dma_cfg_in.src_tsize+ fir_sample_start[FIR_CHNL_ID_0])%FIR_MEM_LEN;

            link_num++;
            if(fir_run_cfg.sample_bits==AUD_BITS_16)
            {
                fir_dma_cfg_in.src = (uint32_t)(input)+1*2;
            }
            else
            {
                fir_dma_cfg_in.src = (uint32_t)(input)+1*4;
            }
            hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, &fir_dma_in_desc[link_num + 1], 0); //disable interupt
            fir_dma_in_desc[link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_1].sample   + fir_sample_start[FIR_CHNL_ID_1]);
            fir_sample_start[FIR_CHNL_ID_1]=(fir_dma_cfg_in.src_tsize+ fir_sample_start[FIR_CHNL_ID_1])%FIR_MEM_LEN;

            link_num++;
            if(fir_run_cfg.sample_bits==AUD_BITS_16)
            {
                fir_dma_cfg_in.src = (uint32_t)(input)+2*2;
            }
            else
            {
                fir_dma_cfg_in.src = (uint32_t)(input)+2*4;
            }
            hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, 0, 0); //disable interupt
            fir_dma_in_desc[link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_2].sample   + fir_sample_start[FIR_CHNL_ID_2]);
            fir_sample_start[FIR_CHNL_ID_2]=(fir_dma_cfg_in.src_tsize+ fir_sample_start[FIR_CHNL_ID_2])%FIR_MEM_LEN;
        }

        fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_sample_num= fir_dma_cfg_in.src_tsize;
        fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->fir_sample_num= fir_dma_cfg_in.src_tsize;
        fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config0->fir_sample_num= fir_dma_cfg_in.src_tsize;

        //config fir filter out.
        if(loop!=0)
        {
            fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->fir_loop_num=loop;
            fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config1->fir_loop_num=loop;
            fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config1->fir_loop_num=loop;

            *proc_num = loop *(FIR_MEM_LEN-coef_len_max)*fir_run_cfg.ch_num;
        }
        else
        {
            fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->fir_loop_num=1;
            fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config1->fir_loop_num=1;
            fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config1->fir_loop_num=1;

            *proc_num = total_num;
        }

        //config out DMA
        fir_dma_cfg_out.dst_bsize = HAL_DMA_BSIZE_1;
        fir_dma_cfg_out.dst_periph = 0; //useless
        fir_dma_cfg_out.handler = fir_irq_handler;
        fir_dma_cfg_out.src = 0; // useless
        fir_dma_cfg_out.src_bsize = HAL_DMA_BSIZE_1;
        fir_dma_cfg_out.src_periph = HAL_AUDMA_FIR_RX;
        fir_dma_cfg_out.try_burst = 1;
        fir_dma_cfg_out.type = HAL_DMA_FLOW_P2M_SI_DMA;


        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {
            fir_dma_cfg_out.dst_width = HAL_DMA_WIDTH_HALFWORD;
            fir_dma_cfg_out.src_width = HAL_DMA_WIDTH_HALFWORD;
        }
        else
        {
            fir_dma_cfg_out.dst_width = HAL_DMA_WIDTH_WORD;
            fir_dma_cfg_out.src_width = HAL_DMA_WIDTH_WORD;
        }

        if(loop!=0)
        {
            fir_dma_cfg_out.src_tsize = FIR_MEM_LEN-coef_len_max;
            link_num = 0;

            for (i = 0; i < loop; i++)
            {
                if(fir_run_cfg.sample_bits==AUD_BITS_16)
                {
                    fir_dma_cfg_out.dst = (uint32_t)(input) + fir_dma_cfg_out.src_tsize*fir_run_cfg.ch_num*i*2;
                }
                else
                {
                    fir_dma_cfg_out.dst = (uint32_t)(input) + fir_dma_cfg_out.src_tsize*fir_run_cfg.ch_num*i*4;
                }

                hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, &fir_dma_out_desc[link_num + 1], 0);
                fir_dma_out_desc[link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].coef +fir_run_cfg.coef_len[FIR_CHNL_ID_0]);

                link_num++;
                if(fir_run_cfg.sample_bits==AUD_BITS_16)
                {
                    fir_dma_cfg_out.dst = (uint32_t)(input) + (fir_dma_cfg_out.src_tsize*fir_run_cfg.ch_num*i+1)*2;
                }
                else
                {
                    fir_dma_cfg_out.dst = (uint32_t)(input) + (fir_dma_cfg_out.src_tsize*fir_run_cfg.ch_num*i+1)*4;
                }

                hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, &fir_dma_out_desc[link_num + 1], 0);

                fir_dma_out_desc[link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_1].coef +fir_run_cfg.coef_len[FIR_CHNL_ID_1]);

                link_num++;

                if(fir_run_cfg.sample_bits==AUD_BITS_16)
                {
                    fir_dma_cfg_out.dst = (uint32_t)(input) + (fir_dma_cfg_out.src_tsize*fir_run_cfg.ch_num*i+2)*2;
                }
                else
                {
                    fir_dma_cfg_out.dst = (uint32_t)(input) + (fir_dma_cfg_out.src_tsize*fir_run_cfg.ch_num*i+2)*4;
                }

                if(i==loop-1)
                {
                    hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, 0, 1); //enable interupt
                }
                else
                {
                    hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, &fir_dma_out_desc[link_num + 1], 0);
                }
                fir_dma_out_desc[link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_2].coef +fir_run_cfg.coef_len[FIR_CHNL_ID_2]);

                link_num++;

            }

        }
        else
        {
            fir_dma_cfg_out.src_tsize =total_num/fir_run_cfg.ch_num;
            link_num = 0;

            fir_dma_cfg_out.dst = (uint32_t)(input);
            hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, &fir_dma_out_desc[link_num + 1], 0); //disable interupt
            fir_dma_out_desc[link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].coef+fir_run_cfg.coef_len[FIR_CHNL_ID_0]);

            link_num++;

            if(fir_run_cfg.sample_bits==AUD_BITS_16)
            {
                fir_dma_cfg_out.dst = (uint32_t)(input) + 1*2;
            }
            else
            {
                fir_dma_cfg_out.dst = (uint32_t)(input) + 1*4;
            }

            hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, &fir_dma_out_desc[link_num + 1], 0); //enable interupt
            fir_dma_out_desc[link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_1].coef+fir_run_cfg.coef_len[FIR_CHNL_ID_1]);

            link_num++;

            if(fir_run_cfg.sample_bits==AUD_BITS_16)
            {
                fir_dma_cfg_out.dst = (uint32_t)(input) + 2*2;
            }
            else
            {
                fir_dma_cfg_out.dst = (uint32_t)(input) + 2*4;
            }

            hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, 0, 1); //enable interupt
            fir_dma_out_desc[link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_2].coef+fir_run_cfg.coef_len[FIR_CHNL_ID_2]);

        }

        memset(&fir_in_2d_src, 0, sizeof(fir_in_2d_src));
        memset(&fir_out_2d_dst, 0, sizeof(fir_out_2d_dst));
        fir_in_2d_src.xmodify = fir_run_cfg.ch_num-1;
        fir_in_2d_src.xcount = FIR_MEM_LEN;
        fir_in_2d_src.ymodify = 0;
        fir_in_2d_src.ycount = 1;

        fir_out_2d_dst.xmodify = fir_run_cfg.ch_num-1;
        fir_out_2d_dst.xcount = FIR_MEM_LEN;
        fir_out_2d_dst.ymodify = 0;
        fir_out_2d_dst.ycount = 1;

        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {
            memset(&fir_in_2d_dst, 0, sizeof(fir_in_2d_dst));
            memset(&fir_out_2d_src, 0, sizeof(fir_out_2d_src));

            fir_in_2d_dst.xmodify = 1;
            fir_in_2d_dst.xcount = FIR_MEM_LEN;
            fir_in_2d_dst.ymodify = 0;
            fir_in_2d_dst.ycount = 1;

            fir_out_2d_src.xmodify = 1;
            fir_out_2d_src.xcount = FIR_MEM_LEN;
            fir_out_2d_src.ymodify = 0;
            fir_out_2d_src.ycount = 1;
        }
    }
    else if(fir_run_cfg.ch_num==4)
    {
        fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_sample_start=fir_sample_start[FIR_CHNL_ID_0];
        fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->fir_sample_start=fir_sample_start[FIR_CHNL_ID_1];
        fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config0->fir_sample_start=fir_sample_start[FIR_CHNL_ID_2];
        fir_setings.fir_ch_config[FIR_CHNL_ID_3].fir_ch_config0->fir_sample_start=fir_sample_start[FIR_CHNL_ID_3];

        coef_len_max=0;
        if(coef_len_max<fir_run_cfg.coef_len[FIR_CHNL_ID_0])
        {
            coef_len_max=fir_run_cfg.coef_len[FIR_CHNL_ID_0];
        }
        if(coef_len_max<fir_run_cfg.coef_len[FIR_CHNL_ID_1])
        {
            coef_len_max=fir_run_cfg.coef_len[FIR_CHNL_ID_1];
        }
        if(coef_len_max<fir_run_cfg.coef_len[FIR_CHNL_ID_2])
        {
            coef_len_max=fir_run_cfg.coef_len[FIR_CHNL_ID_2];
        }
        if(coef_len_max<fir_run_cfg.coef_len[FIR_CHNL_ID_3])
        {
            coef_len_max=fir_run_cfg.coef_len[FIR_CHNL_ID_3];
        }

        loop = (total_num)/(FIR_MEM_LEN-coef_len_max);

        ASSERT(loop <= FIREQ_DMA_LINKLIST_NUM_MAX, "[%s] loop:%u <= FIREQ_DMA_LINKLIST_NUM_MAX:%d", __func__, loop,FIREQ_DMA_LINKLIST_NUM_MAX);

        loop=loop/fir_run_cfg.ch_num;

        //config input DMA
        fir_dma_cfg_in.dst = 0; // useless
        fir_dma_cfg_in.dst_bsize = HAL_DMA_BSIZE_1;
        fir_dma_cfg_in.dst_periph = HAL_AUDMA_FIR_TX;
        fir_dma_cfg_in.handler = 0; //NULL
        fir_dma_cfg_in.src_bsize = HAL_DMA_BSIZE_1;
        fir_dma_cfg_in.src_periph = 0; // useless
        fir_dma_cfg_in.try_burst = 1;
        fir_dma_cfg_in.type = HAL_DMA_FLOW_M2P_DI_DMA;

        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {
            fir_dma_cfg_in.dst_width = HAL_DMA_WIDTH_HALFWORD;
            fir_dma_cfg_in.src_width = HAL_DMA_WIDTH_HALFWORD;
        }
        else
        {
            fir_dma_cfg_in.dst_width = HAL_DMA_WIDTH_WORD;
            fir_dma_cfg_in.src_width = HAL_DMA_WIDTH_WORD;
        }

        if(loop!=0)
        {
            fir_dma_cfg_in.src_tsize = FIR_MEM_LEN-coef_len_max;
            link_num = 0;

            for (i = 0; i < loop; i++)
            {
                //ch 0
                if(fir_run_cfg.sample_bits==AUD_BITS_16)
                {
                    fir_dma_cfg_in.src = (uint32_t)(input) + fir_dma_cfg_in.src_tsize*fir_run_cfg.ch_num*i*2;
                }
                else
                {
                    fir_dma_cfg_in.src = (uint32_t)(input) + fir_dma_cfg_in.src_tsize*fir_run_cfg.ch_num*i*4;
                }

                hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, &fir_dma_in_desc[link_num + 1], 0);
                fir_dma_in_desc[link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].sample + fir_sample_start[FIR_CHNL_ID_0]);
                fir_sample_start[FIR_CHNL_ID_0]=(fir_dma_cfg_in.src_tsize+ fir_sample_start[FIR_CHNL_ID_0])%FIR_MEM_LEN;

                link_num++;

                //ch 1
                if(fir_run_cfg.sample_bits==AUD_BITS_16)
                {
                    fir_dma_cfg_in.src = (uint32_t)(input) + (fir_dma_cfg_in.src_tsize*fir_run_cfg.ch_num*i+1)*2;
                }
                else
                {
                    fir_dma_cfg_in.src = (uint32_t)(input) + (fir_dma_cfg_in.src_tsize*fir_run_cfg.ch_num*i+1)*4;
                }

                hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, &fir_dma_in_desc[link_num + 1], 0);
                fir_dma_in_desc[link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_1].sample  + fir_sample_start[FIR_CHNL_ID_1]);
                fir_sample_start[FIR_CHNL_ID_1]=(fir_dma_cfg_in.src_tsize+ fir_sample_start[FIR_CHNL_ID_1])%FIR_MEM_LEN;

                link_num++;

                //ch 2
                if(fir_run_cfg.sample_bits==AUD_BITS_16)
                {
                    fir_dma_cfg_in.src = (uint32_t)(input) + (fir_dma_cfg_in.src_tsize*fir_run_cfg.ch_num*i+2)*2;
                }
                else
                {
                    fir_dma_cfg_in.src = (uint32_t)(input) + (fir_dma_cfg_in.src_tsize*fir_run_cfg.ch_num*i+2)*4;
                }

                hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, &fir_dma_in_desc[link_num + 1], 0);

                fir_dma_in_desc[link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_2].sample  + fir_sample_start[FIR_CHNL_ID_2]);
                fir_sample_start[FIR_CHNL_ID_2]=(fir_dma_cfg_in.src_tsize+ fir_sample_start[FIR_CHNL_ID_2])%FIR_MEM_LEN;

                link_num++;

                //ch 3
                if(fir_run_cfg.sample_bits==AUD_BITS_16)
                {
                    fir_dma_cfg_in.src = (uint32_t)(input) + (fir_dma_cfg_in.src_tsize*fir_run_cfg.ch_num*i+3)*2;
                }
                else
                {
                    fir_dma_cfg_in.src = (uint32_t)(input) + (fir_dma_cfg_in.src_tsize*fir_run_cfg.ch_num*i+3)*4;
                }

                if(i==loop-1)
                {
                    hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, 0, 0); //disable interupt
                }
                else
                {
                    hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, &fir_dma_in_desc[link_num + 1], 0);
                }
                fir_dma_in_desc[link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_3].sample  + fir_sample_start[FIR_CHNL_ID_3]);
                fir_sample_start[FIR_CHNL_ID_3]=(fir_dma_cfg_in.src_tsize+ fir_sample_start[FIR_CHNL_ID_3])%FIR_MEM_LEN;

                link_num++;
            }

        }
        else
        {
            fir_dma_cfg_in.src_tsize =total_num/fir_run_cfg.ch_num;
            link_num = 0;

            fir_dma_cfg_in.src = (uint32_t)(input);
            hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, &fir_dma_in_desc[link_num + 1], 0); //disable interupt
            fir_dma_in_desc[link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].sample   + fir_sample_start[FIR_CHNL_ID_0]);
            fir_sample_start[FIR_CHNL_ID_0]=(fir_dma_cfg_in.src_tsize+ fir_sample_start[FIR_CHNL_ID_0])%FIR_MEM_LEN;

            link_num++;
            if(fir_run_cfg.sample_bits==AUD_BITS_16)
            {
                fir_dma_cfg_in.src = (uint32_t)(input)+1*2;
            }
            else
            {
                fir_dma_cfg_in.src = (uint32_t)(input)+1*4;
            }
            hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, &fir_dma_in_desc[link_num + 1], 0); //disable interupt
            fir_dma_in_desc[link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_1].sample   + fir_sample_start[FIR_CHNL_ID_1]);
            fir_sample_start[FIR_CHNL_ID_1]=(fir_dma_cfg_in.src_tsize+ fir_sample_start[FIR_CHNL_ID_1])%FIR_MEM_LEN;

            link_num++;
            if(fir_run_cfg.sample_bits==AUD_BITS_16)
            {
                fir_dma_cfg_in.src = (uint32_t)(input)+2*2;
            }
            else
            {
                fir_dma_cfg_in.src = (uint32_t)(input)+2*4;
            }
            hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, &fir_dma_in_desc[link_num + 1], 0); //disable interupt
            fir_dma_in_desc[link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_2].sample   + fir_sample_start[FIR_CHNL_ID_2]);
            fir_sample_start[FIR_CHNL_ID_2]=(fir_dma_cfg_in.src_tsize+ fir_sample_start[FIR_CHNL_ID_2])%FIR_MEM_LEN;

            link_num++;
            if(fir_run_cfg.sample_bits==AUD_BITS_16)
            {
                fir_dma_cfg_in.src = (uint32_t)(input)+3*2;
            }
            else
            {
                fir_dma_cfg_in.src = (uint32_t)(input)+3*4;
            }
            hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, 0, 0); //disable interupt
            fir_dma_in_desc[link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_3].sample   + fir_sample_start[FIR_CHNL_ID_3]);
            fir_sample_start[FIR_CHNL_ID_3]=(fir_dma_cfg_in.src_tsize+ fir_sample_start[FIR_CHNL_ID_3])%FIR_MEM_LEN;
        }

        fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_sample_num= fir_dma_cfg_in.src_tsize;
        fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->fir_sample_num= fir_dma_cfg_in.src_tsize;
        fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config0->fir_sample_num= fir_dma_cfg_in.src_tsize;
        fir_setings.fir_ch_config[FIR_CHNL_ID_3].fir_ch_config0->fir_sample_num= fir_dma_cfg_in.src_tsize;

        //config fir filter out.
        if(loop!=0)
        {
            fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->fir_loop_num=loop;
            fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config1->fir_loop_num=loop;
            fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config1->fir_loop_num=loop;
            fir_setings.fir_ch_config[FIR_CHNL_ID_3].fir_ch_config1->fir_loop_num=loop;

            *proc_num = loop *(FIR_MEM_LEN-coef_len_max)*fir_run_cfg.ch_num;
        }
        else
        {
            fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->fir_loop_num=1;
            fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config1->fir_loop_num=1;
            fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config1->fir_loop_num=1;
            fir_setings.fir_ch_config[FIR_CHNL_ID_3].fir_ch_config1->fir_loop_num=1;

            *proc_num = total_num;
        }

        //config out DMA
        fir_dma_cfg_out.dst_bsize = HAL_DMA_BSIZE_1;
        fir_dma_cfg_out.dst_periph = 0; //useless
        fir_dma_cfg_out.handler = fir_irq_handler;
        fir_dma_cfg_out.src = 0; // useless
        fir_dma_cfg_out.src_bsize = HAL_DMA_BSIZE_1;
        fir_dma_cfg_out.src_periph = HAL_AUDMA_FIR_RX;
        fir_dma_cfg_out.try_burst = 1;
        fir_dma_cfg_out.type = HAL_DMA_FLOW_P2M_SI_DMA;


        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {
            fir_dma_cfg_out.dst_width = HAL_DMA_WIDTH_HALFWORD;
            fir_dma_cfg_out.src_width = HAL_DMA_WIDTH_HALFWORD;
        }
        else
        {
            fir_dma_cfg_out.dst_width = HAL_DMA_WIDTH_WORD;
            fir_dma_cfg_out.src_width = HAL_DMA_WIDTH_WORD;
        }

        if(loop!=0)
        {
            fir_dma_cfg_out.src_tsize = FIR_MEM_LEN-coef_len_max;
            link_num = 0;

            for (i = 0; i < loop; i++)
            {
                if(fir_run_cfg.sample_bits==AUD_BITS_16)
                {
                    fir_dma_cfg_out.dst = (uint32_t)(input) + fir_dma_cfg_out.src_tsize*fir_run_cfg.ch_num*i*2;
                }
                else
                {
                    fir_dma_cfg_out.dst = (uint32_t)(input) + fir_dma_cfg_out.src_tsize*fir_run_cfg.ch_num*i*4;
                }

                hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, &fir_dma_out_desc[link_num + 1], 0);
                fir_dma_out_desc[link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].coef +fir_run_cfg.coef_len[FIR_CHNL_ID_0]);

                link_num++;
                if(fir_run_cfg.sample_bits==AUD_BITS_16)
                {
                    fir_dma_cfg_out.dst = (uint32_t)(input) + (fir_dma_cfg_out.src_tsize*fir_run_cfg.ch_num*i+1)*2;
                }
                else
                {
                    fir_dma_cfg_out.dst = (uint32_t)(input) + (fir_dma_cfg_out.src_tsize*fir_run_cfg.ch_num*i+1)*4;
                }

                hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, &fir_dma_out_desc[link_num + 1], 0);

                fir_dma_out_desc[link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_1].coef +fir_run_cfg.coef_len[FIR_CHNL_ID_1]);

                link_num++;

                if(fir_run_cfg.sample_bits==AUD_BITS_16)
                {
                    fir_dma_cfg_out.dst = (uint32_t)(input) + (fir_dma_cfg_out.src_tsize*fir_run_cfg.ch_num*i+2)*2;
                }
                else
                {
                    fir_dma_cfg_out.dst = (uint32_t)(input) + (fir_dma_cfg_out.src_tsize*fir_run_cfg.ch_num*i+2)*4;
                }

                hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, &fir_dma_out_desc[link_num + 1], 0);

                fir_dma_out_desc[link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_2].coef +fir_run_cfg.coef_len[FIR_CHNL_ID_2]);

                link_num++;

                if(fir_run_cfg.sample_bits==AUD_BITS_16)
                {
                    fir_dma_cfg_out.dst = (uint32_t)(input) + (fir_dma_cfg_out.src_tsize*fir_run_cfg.ch_num*i+3)*2;
                }
                else
                {
                    fir_dma_cfg_out.dst = (uint32_t)(input) + (fir_dma_cfg_out.src_tsize*fir_run_cfg.ch_num*i+3)*4;
                }

                if(i==loop-1)
                {
                    hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, 0, 1); //enable interupt
                }
                else
                {
                    hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, &fir_dma_out_desc[link_num + 1], 0);
                }
                fir_dma_out_desc[link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_3].coef +fir_run_cfg.coef_len[FIR_CHNL_ID_3]);

                link_num++;
            }

        }
        else
        {
            fir_dma_cfg_out.src_tsize =total_num/fir_run_cfg.ch_num;
            link_num = 0;

            fir_dma_cfg_out.dst = (uint32_t)(input);
            hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, &fir_dma_out_desc[link_num + 1], 0); //disable interupt
            fir_dma_out_desc[link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].coef+fir_run_cfg.coef_len[FIR_CHNL_ID_0]);

            link_num++;

            if(fir_run_cfg.sample_bits==AUD_BITS_16)
            {
                fir_dma_cfg_out.dst = (uint32_t)(input) + 1*2;
            }
            else
            {
                fir_dma_cfg_out.dst = (uint32_t)(input) + 1*4;
            }

            hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, &fir_dma_out_desc[link_num + 1], 0); //disable interupt
            fir_dma_out_desc[link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_1].coef+fir_run_cfg.coef_len[FIR_CHNL_ID_1]);

            link_num++;

            if(fir_run_cfg.sample_bits==AUD_BITS_16)
            {
                fir_dma_cfg_out.dst = (uint32_t)(input) + 2*2;
            }
            else
            {
                fir_dma_cfg_out.dst = (uint32_t)(input) + 2*4;
            }

            hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, &fir_dma_out_desc[link_num + 1], 0); //disable interupt
            fir_dma_out_desc[link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_2].coef+fir_run_cfg.coef_len[FIR_CHNL_ID_2]);

            link_num++;

            if(fir_run_cfg.sample_bits==AUD_BITS_16)
            {
                fir_dma_cfg_out.dst = (uint32_t)(input) + 3*2;
            }
            else
            {
                fir_dma_cfg_out.dst = (uint32_t)(input) + 3*4;
            }

            hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, 0, 1); //enable interupt
            fir_dma_out_desc[link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_3].coef+fir_run_cfg.coef_len[FIR_CHNL_ID_3]);
        }

        memset(&fir_in_2d_src, 0, sizeof(fir_in_2d_src));
        memset(&fir_out_2d_dst, 0, sizeof(fir_out_2d_dst));
        fir_in_2d_src.xmodify = fir_run_cfg.ch_num-1;
        fir_in_2d_src.xcount = FIR_MEM_LEN;
        fir_in_2d_src.ymodify = 0;
        fir_in_2d_src.ycount = 1;

        fir_out_2d_dst.xmodify = fir_run_cfg.ch_num-1;
        fir_out_2d_dst.xcount = FIR_MEM_LEN;
        fir_out_2d_dst.ymodify = 0;
        fir_out_2d_dst.ycount = 1;

        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {
            memset(&fir_in_2d_dst, 0, sizeof(fir_in_2d_dst));
            memset(&fir_out_2d_src, 0, sizeof(fir_out_2d_src));

            fir_in_2d_dst.xmodify = 1;
            fir_in_2d_dst.xcount = FIR_MEM_LEN;
            fir_in_2d_dst.ymodify = 0;
            fir_in_2d_dst.ycount = 1;

            fir_out_2d_src.xmodify = 1;
            fir_out_2d_src.xcount = FIR_MEM_LEN;
            fir_out_2d_src.ymodify = 0;
            fir_out_2d_src.ycount = 1;
        }
    }
#endif
    else
    {
        ASSERT(fir_run_cfg.ch_num < 2, "Bad channel number!: %u", fir_run_cfg.ch_num);
    }
    //LOG_I("%s:%d loop %d, ch1 %d, ch2 %d\n", __func__, __LINE__, loop, fir_dma_cfg_in.ch, fir_dma_cfg_out.ch);
    return 0;
}

#ifdef HW_FIR_PARALLEL_RUN

static void fir_irq_handler(uint8_t chan, uint32_t remain_tsize, uint32_t error, struct HAL_DMA_DESC_T *lli)
{
    FIR_PP_T pp;
    int16_t *buf = NULL;
    int16_t proc_num = 0, offset_num = 0,  eq_num = 0;

  //  LOG_I("%s.error:%d.\n", __func__,error);
    fir_stop();

    // init parameter
    pp = fir_run_cfg.pp;
    buf = fir_run_cfg.data[pp].eq_buf;
    eq_num = fir_run_cfg.data[pp].eq_num;
    proc_num = fir_run_cfg.proc_num;
    offset_num = fir_run_cfg.offset_num;

    // store output data, set new data

    offset_num+=proc_num;

    ASSERT((uint32_t)offset_num <= (uint32_t)eq_num, "Bad fir offset_num: %u, eq_num=%u", offset_num, eq_num);

    if(offset_num == eq_num) // OK!!!
    {
        fir_run_cfg.offset_num = 0;
        fir_run_cfg.proc_num = 0;
        fir_run_cfg.data[pp].state = FIR_DATA_DONE;

        fir_finished_flag=1;
    }
    else // Continue...
    {
        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {
            fir_fill_sample(buf+offset_num, eq_num-offset_num, &proc_num);
        }
        else
        {
            fir_fill_sample(buf+offset_num*2, eq_num-offset_num, &proc_num);
        }

        fir_run_cfg.proc_num = proc_num;
        fir_run_cfg.offset_num = offset_num;

        fir_start();
    }

}

int fir_run(uint8_t *buf, uint32_t total_num)
{
    FIR_PP_T pp_empty;
    FIR_PP_T pp_full;
    FIR_DATA_STATE_T state_full;

  //  LOG_I("%s,len:%d hw_fir_open_flag:%d.\n", __func__, total_num,hw_fir_open_flag);

    if(hw_fir_open_flag==1)
    {
        //LOG_I("[%s] len=%d pp=%d state=(%d,%d)", __func__, len, fir_run_cfg.pp, fir_run_cfg.data[0].state, fir_run_cfg.data[1].state);
        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {
            ASSERT(total_num <= fir_run_cfg.single_run_eq_buf_len, "[%s] len(%u) > single_run_eq_buf_len(%u)", __func__, total_num, fir_run_cfg.single_run_eq_buf_len);
        }
        else
        {
            ASSERT(total_num <= fir_run_cfg.single_run_eq_buf_len/2, "[%s] len(%u) > single_run_eq_buf_len(%u)", __func__, total_num, fir_run_cfg.single_run_eq_buf_len);
        }

        while(fir_finished_flag==0)
        {
            LOG_I("[%s] waiting......:%d\n", __func__,fir_finished_flag);
        }
        fir_finished_flag=0;

        pp_full = fir_run_cfg.pp;
        pp_empty = PP_PINGPANG(pp_full);
        // Make a copy of full state, because IRQ might change it
        state_full = fir_run_cfg.data[pp_full].state;

        ASSERT(fir_run_cfg.data[pp_empty].state == FIR_DATA_IDLE, "Invalid pp_empty state: pp_empty=%d state_empty=%d state_full=%d)",
        pp_empty, fir_run_cfg.data[pp_empty].state, state_full);

        // app --> eq
        memcpy_samples(fir_run_cfg.data[pp_empty].eq_buf, buf, total_num);

        // eq --> af
        if (state_full == FIR_DATA_DONE)
        {
            memcpy_samples(buf, fir_run_cfg.data[pp_full].eq_buf, total_num);
            fir_run_cfg.data[pp_full].state = FIR_DATA_IDLE;
        }
        else
        {
            // FIR_DATA_IDLE or FIR_DATA_BUSY
            memset_samples(buf, 0, total_num);
            if (state_full == FIR_DATA_BUSY)
            {
                LOG_I("[%s] pp_full is no full!!!", __func__);
                // Skip this frame
                return 0;
            }
        }

        // Update state
        fir_run_cfg.data[pp_empty].eq_num = total_num;
        fir_run_cfg.data[pp_empty].state = FIR_DATA_BUSY;
        fir_run_cfg.pp = pp_empty;
        fir_fill_sample(fir_run_cfg.data[pp_empty].eq_buf, fir_run_cfg.data[pp_empty].eq_num, &fir_run_cfg.proc_num);
        fir_start();

        return 0;
    }
    else
    {
        LOG_I("[%s]HW FIR MUSET BE OPENED FIRSTLY!\n", __func__);
        return 1;
    }

}

#else

static void fir_irq_handler(uint8_t chan, uint32_t remain_tsize, uint32_t error, struct HAL_DMA_DESC_T *lli)
{
   // LOG_I("[%s]", __func__);
    fir_finished_flag=1;
}

int fir_run(uint8_t *buf, uint32_t total_num)
{
    int16_t proc_num = 0, offset_num = 0;
    int16_t *fir_buf = (int16_t *)buf;
    if(hw_fir_open_flag==1)
    {
        do {
            if(fir_run_cfg.sample_bits==AUD_BITS_16)
            {
                fir_fill_sample(fir_buf+offset_num, total_num-offset_num, &proc_num);
            }
            else
            {
                fir_fill_sample(fir_buf+offset_num*2, total_num-offset_num, &proc_num);
            }
            fir_finished_flag=0;
            fir_start();
            while(fir_finished_flag==0);
            fir_stop();
            offset_num=offset_num+proc_num;
        } while(offset_num < total_num);

        return 0;
    }
    else
    {
        LOG_I("[%s]HW FIR MUSET BE OPENED FIRSTLY!\n", __func__);
        return 1;
    }
}
#endif

int fir_set_cfg(const FIR_CFG_T *cfg)
{
    float gain = cfg->gain;
    int32_t coef_len = 0;
    const int32_t *coef_ptr = NULL;

    LOG_I("[%s]", __func__);

    if(hw_fir_open_flag==1)
    {

        if(gain>0.0f)
        {
            gain=0.0f;
        }
        gain = (float)powf(10,gain/20);
        coef_len = cfg->len;
        coef_ptr = cfg->coef;

        fir_run_cfg.coef_len[FIR_CHNL_ID_0] = coef_len;
        fir_run_cfg.coef_len[FIR_CHNL_ID_1] = coef_len;
#ifdef HW_FIR_MUTI_CHANNEL
        fir_run_cfg.coef_len[FIR_CHNL_ID_2] = coef_len;
        fir_run_cfg.coef_len[FIR_CHNL_ID_3] = coef_len;
#endif

        ASSERT(coef_ptr != NULL, "[%s] coef == NULL", __func__);
        ASSERT(coef_len < FIR_MEM_LEN, "[%s] coef_len(%d) > FIR_MEM_LEN", __func__, coef_len);

        fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_order=coef_len;
        fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_sample_start=fir_sample_start[FIR_CHNL_ID_0]=coef_len;
        fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_sample_num=0;

        fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->fir_result_base_addr=coef_len;
        fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->fir_gain_sel=6;

        fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->fir_order=coef_len;
        fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->fir_sample_start=fir_sample_start[FIR_CHNL_ID_1]=coef_len;
        fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->fir_sample_num=0;

        fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config1->fir_result_base_addr=coef_len;
        fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config1->fir_gain_sel=6;

        coefcpy_int16_to_int32(fir_setings.fir_ch_config[FIR_CHNL_ID_0].coef, coef_ptr, coef_len, gain);
        coefcpy_int16_to_int32(fir_setings.fir_ch_config[FIR_CHNL_ID_1].coef, coef_ptr, coef_len, gain);

        coefset_int16_to_int32(fir_setings.fir_ch_config[FIR_CHNL_ID_0].sample, 0x0, FIR_MEM_LEN);
        coefset_int16_to_int32(fir_setings.fir_ch_config[FIR_CHNL_ID_1].sample, 0x0, FIR_MEM_LEN);

#ifdef HW_FIR_MUTI_CHANNEL
        fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config0->fir_order=coef_len;
        fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config0->fir_sample_start=fir_sample_start[FIR_CHNL_ID_2]=coef_len;
        fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config0->fir_sample_num=0;

        fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config1->fir_result_base_addr=coef_len;
        fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config1->fir_gain_sel=6;

        fir_setings.fir_ch_config[FIR_CHNL_ID_3].fir_ch_config0->fir_order=coef_len;
        fir_setings.fir_ch_config[FIR_CHNL_ID_3].fir_ch_config0->fir_sample_start=fir_sample_start[FIR_CHNL_ID_3]=coef_len;
        fir_setings.fir_ch_config[FIR_CHNL_ID_3].fir_ch_config0->fir_sample_num=0;

        fir_setings.fir_ch_config[FIR_CHNL_ID_3].fir_ch_config1->fir_result_base_addr=coef_len;
        fir_setings.fir_ch_config[FIR_CHNL_ID_3].fir_ch_config1->fir_gain_sel=6;

        coefcpy_int16_to_int32(fir_setings.fir_ch_config[FIR_CHNL_ID_2].coef, coef_ptr, coef_len, gain);
        coefcpy_int16_to_int32(fir_setings.fir_ch_config[FIR_CHNL_ID_3].coef, coef_ptr, coef_len, gain);

        coefset_int16_to_int32(fir_setings.fir_ch_config[FIR_CHNL_ID_2].sample, 0x0, FIR_MEM_LEN);
        coefset_int16_to_int32(fir_setings.fir_ch_config[FIR_CHNL_ID_3].sample, 0x0, FIR_MEM_LEN);
#endif
        return 0;
    }
    else
    {
        LOG_I("[%s]HW FIR MUSET BE OPENED FIRSTLY!\n", __func__);
        return 1;
    }
}

int fir_set_cfg_ch(const FIR_CFG_T *cfg,enum AUD_CHANNEL_NUM_T ch)
{
    float gain = cfg->gain;
    int32_t coef_len = 0;
    const int32_t *coef_ptr = NULL;

    LOG_I("[%s]", __func__);

    if(hw_fir_open_flag==1)
    {
        if(gain>0.0f)
        {
            gain=0.0f;
        }
        gain = (float)powf(10,gain/20);
        coef_len = cfg->len;
        coef_ptr = cfg->coef;

        coef_len=(coef_len+3)&0xfffffffc;

        ASSERT((ch < 5)&&(ch >0), "[%s] 0<ch(%d) < 5", __func__, ch);
        ASSERT(coef_ptr != NULL, "[%s] coef == NULL", __func__);
        ASSERT(coef_len < FIR_MEM_LEN, "[%s] coef_len(%d) > FIR_MEM_LEN", __func__, coef_len);

        ch=ch-1;

        fir_run_cfg.coef_len[ch] = coef_len;

        fir_setings.fir_ch_config[ch].fir_ch_config0->fir_order=coef_len;
        fir_setings.fir_ch_config[ch].fir_ch_config0->fir_sample_start=fir_sample_start[ch]=coef_len;
        fir_setings.fir_ch_config[ch].fir_ch_config0->fir_sample_num=0;

        fir_setings.fir_ch_config[ch].fir_ch_config1->fir_result_base_addr=coef_len;
        fir_setings.fir_ch_config[ch].fir_ch_config1->fir_gain_sel=6;

        coefcpy_int16_to_int32(fir_setings.fir_ch_config[ch].coef, coef_ptr, coef_len, gain);
        coefset_int16_to_int32(fir_setings.fir_ch_config[ch].sample, 0x0, FIR_MEM_LEN);

        return 0;
    }
    else
    {
        LOG_I("[%s]HW FIR MUSET BE OPENED FIRSTLY!\n", __func__);
        return 1;
    }
}

int fir_needed_size(enum AUD_BITS_T sample_bits, int32_t frame_size)
{
    int bytes_per_sample = (sample_bits <= AUD_BITS_16) ? 2 : 4;
    return frame_size * bytes_per_sample * 2;
}

int fir_open(enum AUD_SAMPRATE_T sample_rate, enum AUD_BITS_T sample_bits,enum AUD_CHANNEL_NUM_T ch_num, void *eq_buf, uint32_t len)
{
    LOG_I("[%s] sample_rate:%d,sample_bits:%d,ch_num:%d.", __func__,sample_rate,sample_bits,ch_num);

    // Check parameter
    fir_run_cfg.sample_rate = sample_rate;
    fir_run_cfg.sample_bits = sample_bits;
    fir_run_cfg.ch_num = ch_num;

    anc_iir_clock->en_clk_fir=0xf;
    hal_codec_fir_select_sys_clock();
    hal_codec_fir_enable(48000000);
    anc_soft_rstn->soft_rstn_fir=0xf;

    // Parameter initialize
#ifdef HW_FIR_PARALLEL_RUN
    fir_run_cfg.pp = PP_PING;
    fir_run_cfg.data[0].state = FIR_DATA_IDLE;
    fir_run_cfg.data[1].state = FIR_DATA_IDLE;

    ASSERT(eq_buf && ((uint32_t)eq_buf & 0x3) == 0, "%s: Invalid eq_buf=%p", __func__, eq_buf);
    ASSERT(len && (len & 0x3) == 0, "%s: Invalid eq_buf len=%u", __func__, len);
    fir_run_cfg.single_run_eq_buf_len = len / sizeof(int16_t) / 2;
    fir_run_cfg.data[0].eq_buf = eq_buf;
    fir_run_cfg.data[1].eq_buf = fir_run_cfg.data[0].eq_buf + fir_run_cfg.single_run_eq_buf_len;
#endif

    // NOTE: offset_num MUST be initialized here as fir_run() will not do that
    fir_run_cfg.offset_num = 0;
    fir_run_cfg.proc_num = 0;

    fir_setings.fir_config0->fir_stream_enable_ch0=0;
    fir_setings.fir_config0->fir_stream_enable_ch1=0;

    fir_setings.fir_config0->fir_enable_ch0=0;
    fir_setings.fir_config0->fir_enable_ch1=0;

#ifdef HW_FIR_MUTI_CHANNEL
    fir_setings.fir_config0->fir_stream_enable_ch2=0;
    fir_setings.fir_config0->fir_stream_enable_ch3=0;

    fir_setings.fir_config0->fir_enable_ch2=0;
    fir_setings.fir_config0->fir_enable_ch3=0;
#endif

    fir_setings.fir_config0->dma_ctrl_rx_fir=1;
    fir_setings.fir_config0->dma_ctrl_tx_fir=1;

    if(sample_bits==AUD_BITS_16)
    {
        fir_setings.fir_config0->mode_32bit_fir=0;

        fir_setings.fir_config0->mode_16bit_fir_tx_ch0=1;
        fir_setings.fir_config0->mode_16bit_fir_rx_ch0=1;
        fir_setings.fir_config0->mode_16bit_fir_tx_ch1=1;
        fir_setings.fir_config0->mode_16bit_fir_rx_ch1=1;
#ifdef HW_FIR_MUTI_CHANNEL
        fir_setings.fir_config0->mode_16bit_fir_tx_ch2=1;
        fir_setings.fir_config0->mode_16bit_fir_rx_ch2=1;
        fir_setings.fir_config0->mode_16bit_fir_tx_ch3=1;
        fir_setings.fir_config0->mode_16bit_fir_rx_ch3=1;
#endif
    }
    else if(sample_bits==AUD_BITS_24)
    {
        fir_setings.fir_config0->mode_32bit_fir=0;

        fir_setings.fir_config0->mode_16bit_fir_tx_ch0=0;
        fir_setings.fir_config0->mode_16bit_fir_rx_ch0=0;
        fir_setings.fir_config0->mode_16bit_fir_tx_ch1=0;
        fir_setings.fir_config0->mode_16bit_fir_rx_ch1=0;
#ifdef HW_FIR_MUTI_CHANNEL
        fir_setings.fir_config0->mode_16bit_fir_tx_ch2=0;
        fir_setings.fir_config0->mode_16bit_fir_rx_ch2=0;
        fir_setings.fir_config0->mode_16bit_fir_tx_ch3=0;
        fir_setings.fir_config0->mode_16bit_fir_rx_ch3=0;
#endif
    }
    else if(sample_bits==AUD_BITS_32)
    {
        fir_setings.fir_config0->mode_32bit_fir=1;

        fir_setings.fir_config0->mode_16bit_fir_tx_ch0=0;
        fir_setings.fir_config0->mode_16bit_fir_rx_ch0=0;
        fir_setings.fir_config0->mode_16bit_fir_tx_ch1=0;
        fir_setings.fir_config0->mode_16bit_fir_rx_ch1=0;
#ifdef HW_FIR_MUTI_CHANNEL
        fir_setings.fir_config0->mode_16bit_fir_tx_ch2=0;
        fir_setings.fir_config0->mode_16bit_fir_rx_ch2=0;
        fir_setings.fir_config0->mode_16bit_fir_tx_ch3=0;
        fir_setings.fir_config0->mode_16bit_fir_rx_ch3=0;
#endif
    }
    else
    {
         ASSERT(false, "%s: Invalid sample bits:%d", __func__, sample_bits);
    }


    fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->stream0_fir1=1;
    fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_mode=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_order=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_sample_start=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_sample_num=0;

    fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->fir_result_base_addr=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->fir_slide_offset=1;
    // fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->fir_burst_length=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->fir_gain_sel=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->fir_loop_num=0;
    // fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_do_remap=0;

    fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->stream0_fir1=1;
    fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->fir_mode=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->fir_order=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->fir_sample_start=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->fir_sample_num=0;

    fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config1->fir_result_base_addr=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config1->fir_slide_offset=1;
    //  fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config1->fir_burst_length=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config1->fir_gain_sel=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config1->fir_loop_num=0;
    //  fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->fir_do_remap=0;

    fir_sample_start[FIR_CHNL_ID_0]=0;
    fir_sample_start[FIR_CHNL_ID_1]=0;

#ifdef HW_FIR_MUTI_CHANNEL
    fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config0->stream0_fir1=1;
    fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config0->fir_mode=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config0->fir_order=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config0->fir_sample_start=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config0->fir_sample_num=0;

    fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config1->fir_result_base_addr=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config1->fir_slide_offset=1;
    //  fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config1->fir_burst_length=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config1->fir_gain_sel=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config1->fir_loop_num=0;
    //  fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config0->fir_do_remap=0;

    fir_setings.fir_ch_config[FIR_CHNL_ID_3].fir_ch_config0->stream0_fir1=1;
    fir_setings.fir_ch_config[FIR_CHNL_ID_3].fir_ch_config0->fir_mode=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_3].fir_ch_config0->fir_order=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_3].fir_ch_config0->fir_sample_start=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_3].fir_ch_config0->fir_sample_num=0;

    fir_setings.fir_ch_config[FIR_CHNL_ID_3].fir_ch_config1->fir_result_base_addr=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_3].fir_ch_config1->fir_slide_offset=1;
    //  fir_setings.fir_ch_config[FIR_CHNL_ID_3].fir_ch_config1->fir_burst_length=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_3].fir_ch_config1->fir_gain_sel=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_3].fir_ch_config1->fir_loop_num=0;
    //  fir_setings.fir_ch_config[FIR_CHNL_ID_3].fir_ch_config0->fir_do_remap=0;

    fir_sample_start[FIR_CHNL_ID_2]=0;
    fir_sample_start[FIR_CHNL_ID_3]=0;
#endif

    memset(&fir_dma_cfg_in, 0, sizeof(fir_dma_cfg_in));
    fir_dma_cfg_in.ch = hal_audma_get_chan(HAL_AUDMA_FIR_TX, HAL_DMA_HIGH_PRIO);

    memset(&fir_dma_cfg_out, 0, sizeof(fir_dma_cfg_out));
    fir_dma_cfg_out.ch = hal_audma_get_chan(HAL_AUDMA_FIR_RX, HAL_DMA_HIGH_PRIO);

    hw_fir_open_flag=1;

    fir_finished_flag=1;

    //LOG_I("[%s]hw_fir_open_flag:%d.\n", __func__,hw_fir_open_flag);

    return 0;
}

int fir_close(void)
{
    LOG_I("%s\n", __func__);

    if(hw_fir_open_flag==1)
    {
        fir_stop();

        hal_dma_free_chan(fir_dma_cfg_in.ch);
        hal_dma_free_chan(fir_dma_cfg_out.ch);

        fir_dma_cfg_in.ch=HAL_DMA_CHAN_NONE;
        fir_dma_cfg_out.ch=HAL_DMA_CHAN_NONE;

        hw_fir_open_flag=0;

        hal_codec_fir_disable();
        anc_iir_clock->en_clk_fir=0x0;
        anc_soft_rstn->soft_rstn_fir=0x0;
    }

    return 0;
}

