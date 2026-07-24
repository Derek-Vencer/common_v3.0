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

typedef signed int                          fir_sample_24bits_t;
typedef signed short int                    fir_sample_16bits_t;

// Hardware fir filter: enabled when USB_AUDIO_APP=1
// using irq and pingpong machine
//FIR_USE_IRQ
//FIR_USE_PINGPONG

#define FIR_USE_IRQ 1
#define FIR_USE_PINGPONG 1

#define FIR_MEM_LEN                         512

enum FIR_CHNL_ID_T {
    FIR_CHNL_ID_0 = 0,
    FIR_CHNL_ID_1 ,
    FIR_CHNL_ID_2 ,
    FIR_CHNL_ID_3 ,
    FIR_CHNL_ID_4 ,
    FIR_CHNL_ID_5 ,
    FIR_CHNL_ID_6 ,
    FIR_CHNL_ID_7 ,
    FIR_CHNL_ID_QTY,
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
0	Stream enable  ch0	r/w	���� anc �� adc ������ģʽ��*/
struct _fir_config0
{
	uint32_t Stream_enable_ch0 : 1;
	uint32_t Stream_enable_ch1 : 1;
	uint32_t Stream_enable_ch2 : 1;
	uint32_t Stream_enable_ch3 : 1;
	uint32_t Stream_enable_ch4 : 1;
	uint32_t Stream_enable_ch5 : 1;
	uint32_t Stream_enable_ch6 : 1;
	uint32_t Stream_enable_ch7 : 1;

	uint32_t Fir_enable_ch0 : 1;
	uint32_t Fir_enable_ch1 : 1;
	uint32_t Fir_enable_ch2 : 1;
	uint32_t Fir_enable_ch3 : 1;
	uint32_t Fir_enable_ch4 : 1;
	uint32_t Fir_enable_ch5 : 1;
	uint32_t Fir_enable_ch6 : 1;
	uint32_t Fir_enable_ch7 : 1;

	uint32_t Fir0_256p0_512p1 : 1;
	uint32_t Fir1_256p0_512p1 : 1;
	uint32_t Fir2_256p0_512p1 : 1;
	uint32_t Fir3_256p0_512p1 : 1;

	uint32_t dma_fir_rx_ctrl : 1;
	uint32_t dma_fir_tx_ctrl : 1;

	uint32_t Anc_coef_sel_pdu0_pdu1 : 1;
	uint32_t Anc_coef_sel_fs0_fs1 : 1;
	uint32_t Anc_coef_sel_pdu0_fs0 : 1;
	uint32_t Anc_coef_sel_pdu1_fs1 : 1;

	uint32_t Pdu_fs_swap : 1;
	uint32_t Iir_coef_swap : 1;

	uint32_t Reserved : 4;
};

/*
FIR config1��0x144-  ��λ	Name	Type	����
23:21	Access_offset_ch7	r/w
20:18	Access_offset_ch6	r/w
17:15	Access_offset_ch5	r/w
14:12	Access_offset_ch4	r/w
11:9	Access_offset_ch3	r/w
8:6	Access_offset_ch2	r/w
5:3	Access_offset_ch1	r/w
2:0	Access_offset_ch0	r/w	*/
struct _fir_config1
{
	uint32_t Access_offset_ch0 : 3;
	uint32_t Access_offset_ch1 : 3;
	uint32_t Access_offset_ch2 : 3;
	uint32_t Access_offset_ch3 : 3;
	uint32_t Access_offset_ch4 : 3;
	uint32_t Access_offset_ch5 : 3;
	uint32_t Access_offset_ch6 : 3;
	uint32_t Access_offset_ch7 : 3;

	uint32_t Reserved : 8;

};


/*
FIR ch0 config0��0x104- FIR ch0 config0���ƼĴ�����
λ	Name	Type	����
29:21	fir_sample_num_ch0	r/w	��ǰfir��������������ٸ���ʵ�����óɼ�1��������������128���㣬���ó�127
20:12	fir_sample_start_ch0	r/w	In stream mode, used as initial phase In fir mode, used as fir base addr ��һ��fir��sample memory�����ĸ��㿪ʼ����
11:3	fir_order_ch0	r/w	Fir ϵ�����������511
2:1	fir_mode_ch0	r/w	00��general fir;  01:  10:  11�� not used
0	stream0_fir1_ch0	r/w	0��Stream������anc��adc������ 1��fir����*/
struct _fir_ch_config0
{
	uint32_t stream0_fir1 : 1;
	uint32_t fir_mode : 2;
	uint32_t fir_order : 9;
	uint32_t fir_sample_start : 9;
	uint32_t fir_sample_num : 9;
	uint32_t Reserved : 2;
};


/*
FIR ch0 config1��0x108-  FIR ch0 config1���ƼĴ�����
λ	Name	Type	����
31	do_remap_ch0	r/w	����fir mode 1/2
30:23	loop_num_ch0	r/w	��ǰfir��ɶ��ٴ�dma tx/rx
22:19	gain_sel_ch0	r/w	Fir gain��ÿ�����6db�������� 0~12
��..
4: -12db
5: -6db
6: 0db
7: 6db
8: 12db
��..

18:14	burst_length_ch0	r/w	���� fir mode 1/2
13:9	slide_offset_ch0	r/w	ÿ��fir������sampleƫ�Ƹ���������ͨfir������Ϊ1
8:0	result_base_addr_ch0	r/w	Fir������д��coef memory����ʼoffset */
struct _fir_ch_config1
{
	uint32_t result_base_addr : 9;
	uint32_t slide_offset : 5;
	uint32_t burst_length : 5;
	uint32_t gain_sel : 4;
	uint32_t loop_num : 8;
	uint32_t do_remap : 1;
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
    volatile struct _fir_ch_config fir_ch_config[8];
};

#define FIR_BASE                            ((uint32_t)CODEC_BASE)

static volatile const struct FIR_SETTINGS_T fir_setings = {
    .fir_config0   = (volatile struct _fir_config0 *)(FIR_BASE + 0x100),
    .fir_config1   = (volatile struct _fir_config1 *)(FIR_BASE + 0x144),
    .fir_ch_config[0] = {
        .fir_ch_config0    = (volatile struct _fir_ch_config0 *)(FIR_BASE + 0x104),
        .fir_ch_config1   = (volatile struct _fir_ch_config1 *)(FIR_BASE + 0x108),
        .sample =  (volatile int32_t *)(FIR_BASE + 0x8000),
        .coef     =  (volatile int32_t *)(FIR_BASE + 0x9000),
    },
    .fir_ch_config[1] = {
        .fir_ch_config0    = (volatile struct _fir_ch_config0 *)(FIR_BASE + 0x10c),
        .fir_ch_config1   = (volatile struct _fir_ch_config1 *)(FIR_BASE + 0x110),
        .sample =  (volatile int32_t *)(FIR_BASE + 0x8800),
        .coef     =  (volatile int32_t *)(FIR_BASE + 0x9800),
    },
    .fir_ch_config[2] = {
        .fir_ch_config0    = (volatile struct _fir_ch_config0 *)(FIR_BASE + 0x114),
        .fir_ch_config1   = (volatile struct _fir_ch_config1 *)(FIR_BASE + 0x118),
        .sample =  (volatile int32_t *)(FIR_BASE + 0xa000),
        .coef     =  (volatile int32_t *)(FIR_BASE + 0xb000),
    },
    .fir_ch_config[3] = {
        .fir_ch_config0    = (volatile struct _fir_ch_config0 *)(FIR_BASE + 0x11c),
        .fir_ch_config1   = (volatile struct _fir_ch_config1 *)(FIR_BASE + 0x120),
        .sample =  (volatile int32_t *)(FIR_BASE + 0xa800),
        .coef     =  (volatile int32_t *)(FIR_BASE + 0xb800),
    },
    .fir_ch_config[4] = {
        .fir_ch_config0    = (volatile struct _fir_ch_config0 *)(FIR_BASE + 0x124),
        .fir_ch_config1   = (volatile struct _fir_ch_config1 *)(FIR_BASE + 0x128),
        .sample =  (volatile int32_t *)(FIR_BASE + 0xc000),
        .coef     =  (volatile int32_t *)(FIR_BASE + 0xd000),
    },
    .fir_ch_config[5] = {
        .fir_ch_config0    = (volatile struct _fir_ch_config0 *)(FIR_BASE + 0x13c),
        .fir_ch_config1   = (volatile struct _fir_ch_config1 *)(FIR_BASE + 0x130),
        .sample =  (volatile int32_t *)(FIR_BASE + 0xc800),
        .coef     =  (volatile int32_t *)(FIR_BASE + 0xd800),
    },
    .fir_ch_config[6] = {
        .fir_ch_config0    = (volatile struct _fir_ch_config0 *)(FIR_BASE + 0x134),
        .fir_ch_config1   = (volatile struct _fir_ch_config1 *)(FIR_BASE + 0x138),
        .sample =  (volatile int32_t *)(FIR_BASE + 0xe000),
        .coef     =  (volatile int32_t *)(FIR_BASE + 0xf000),
    },
    .fir_ch_config[7] = {
        .fir_ch_config0    = (volatile struct _fir_ch_config0 *)(FIR_BASE + 0x13c),
        .fir_ch_config1   = (volatile struct _fir_ch_config1 *)(FIR_BASE + 0x140),
        .sample =  (volatile int32_t *)(FIR_BASE + 0xe800),
        .coef     =  (volatile int32_t *)(FIR_BASE + 0xf800),
    },
};


#ifdef FIR_USE_PINGPONG

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
    uint16_t len;
    int32_t *buf;
} FIR_DATA_T;

#define PP_PINGPANG(v) \
    (v == PP_PING ? PP_PANG: PP_PING)

#endif

typedef struct{
#ifdef FIR_USE_PINGPONG
    FIR_PP_T    pp;
    uint16_t    single_run_buf_len;
    FIR_DATA_T  data[2];
#endif

#ifdef FIR_USE_IRQ
    int16_t offset;
    int16_t proclen;
#endif

    uint16_t coef_len;
    uint32_t sample_rate;
    uint32_t sample_bits;
    enum AUD_CHANNEL_NUM_T ch_num;
} FIR_RUN_CFG_T;

#define FIREQ_DMA_LINKLIST_NUM_MAX (24)

static struct HAL_DMA_DESC_T fir_dma_in_desc[FIREQ_DMA_LINKLIST_NUM_MAX];
static struct HAL_DMA_DESC_T fir_dma_out_desc[FIREQ_DMA_LINKLIST_NUM_MAX];

static struct HAL_DMA_CH_CFG_T fir_dma_cfg_in, fir_dma_cfg_out;
static struct HAL_DMA_2D_CFG_T fir_2d_src, fir_2d_dst;

static FIR_RUN_CFG_T fir_run_cfg;
volatile static int hw_fir_open_flag=0;
static uint16_t fir_sample_start_l,fir_sample_start_r;

// Used by fir filter input, fir filter just can deal with 32bits
static inline void memcpy_sample_to_int32(int32_t *dest, uint8_t *src, int num)
{
    //FIR hardware only use 20bit PCM as input.
    if(fir_run_cfg.sample_bits == 16)
    { //16bit
        fir_sample_16bits_t *sample_src = (fir_sample_16bits_t *)src;
        for (int i = 0; i < num; i++)
        {
            dest[i] = ((int32_t)sample_src[i])<<4;
        }
    }
    else if(fir_run_cfg.sample_bits == 24)
    {
        // 24 bits
        fir_sample_24bits_t *sample_src = (fir_sample_24bits_t *)src;
        for (int i = 0; i < num; i++)
        {
            dest[i] = (sample_src[i])>>4;
        }
    }
    else
    {
        // 32 bits
        fir_sample_24bits_t *sample_src = (fir_sample_24bits_t *)src;
        for (int i = 0; i < num; i++)
        {
            dest[i] = (sample_src[i])>>12;
        }
    }

}

// Used by fir filter output, dac just can deal with 16 or 18 bits
static inline void memcpy_int32_to_sample(uint8_t *dest, const int32_t *src, int num)
{
    if(fir_run_cfg.sample_bits == 16)
    {
        fir_sample_16bits_t *sample_dest = (fir_sample_16bits_t *)dest;
        for (int i = 0; i < num; i++)
        {
            sample_dest[i] = src[i]>>4;
        }
    }
    else if(fir_run_cfg.sample_bits == 24)
    {
        // 24 bits
        fir_sample_24bits_t *sample_dest = (fir_sample_24bits_t *)dest;
        for (int i = 0; i < num; i++)
        {
            sample_dest[i] = src[i]<<4;
        }
    }
    else
    {
        //32 bits
        fir_sample_24bits_t *sample_dest = (fir_sample_24bits_t *)dest;
        for (int i = 0; i < num; i++)
        {
            sample_dest[i] = src[i]<<12;
        }
    }
}

static inline void memset_sample(uint8_t *dest, int value, int num)
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
        hal_audma_sg_start(&fir_dma_in_desc[0], &fir_dma_cfg_in);
        hal_audma_sg_start(&fir_dma_out_desc[0], &fir_dma_cfg_out);

        fir_setings.fir_config0->Fir_enable_ch0= 1 ;
    }
    else if(fir_run_cfg.ch_num==2)
    {
        hal_dma_sg_2d_start(&fir_dma_in_desc[0], &fir_dma_cfg_in, &fir_2d_src, NULL);
        hal_dma_sg_2d_start(&fir_dma_out_desc[0], &fir_dma_cfg_out, NULL, &fir_2d_dst);

        fir_setings.fir_config0->Fir_enable_ch0= 1 ;
        fir_setings.fir_config0->Fir_enable_ch2= 1 ;
    }

	return 0;
}

inline static int fir_stop(void)
{
    //LOG_I("%s\n", __func__);
    if(fir_run_cfg.ch_num==1)
    {
        fir_setings.fir_config0->Fir_enable_ch0= 0 ;
    }
    else if(fir_run_cfg.ch_num==2)
    {
        fir_setings.fir_config0->Fir_enable_ch0= 0 ;
        fir_setings.fir_config0->Fir_enable_ch2= 0 ;
    }

    hal_dma_stop(fir_dma_cfg_in.ch);
    hal_dma_stop(fir_dma_cfg_out.ch);

    return 0;
}

static void fir_irq_handler(uint8_t chan, uint32_t remain_tsize, uint32_t error, struct HAL_DMA_DESC_T *lli);

inline static int fir_fill_sample(int32_t input[], int16_t len, int16_t *proclen)
{
    int link_num = 0, loop = 0, i = 0;

    //  LOG_I("%s,len:%d,ch:%d.\n", __func__, len,fir_run_cfg.ch_num);

    *proclen=0;

    if(fir_run_cfg.ch_num==1)
    {
        fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_sample_start=fir_sample_start_l;

        loop = (len)/(FIR_MEM_LEN-fir_run_cfg.coef_len);

        ASSERT(loop <= FIREQ_DMA_LINKLIST_NUM_MAX, "[%s] loop:%u <= FIREQ_DMA_LINKLIST_NUM_MAX:%d", __func__, loop,FIREQ_DMA_LINKLIST_NUM_MAX);

        //config input DMA
        fir_dma_cfg_in.dst = 0; // useless
        fir_dma_cfg_in.dst_bsize = HAL_DMA_BSIZE_16;
        fir_dma_cfg_in.dst_periph = HAL_AUDMA_FIR_TX;
        fir_dma_cfg_in.dst_width = HAL_DMA_WIDTH_WORD;
        fir_dma_cfg_in.handler = 0; //NULL
        fir_dma_cfg_in.src_bsize = HAL_DMA_BSIZE_16;
        fir_dma_cfg_in.src_periph = 0; // useless
        fir_dma_cfg_in.src_width = HAL_DMA_WIDTH_WORD;
        fir_dma_cfg_in.try_burst = 1;
        fir_dma_cfg_in.type = HAL_DMA_FLOW_M2P_DI_DMA;

        if(loop!=0)
        {
            fir_dma_cfg_in.src_tsize = FIR_MEM_LEN-fir_run_cfg.coef_len;
            link_num = 0;

            for (i = 0; i < loop; i++)
            {
                fir_dma_cfg_in.src = (uint32_t)(input + fir_dma_cfg_in.src_tsize*i);
                if(i==loop-1)
                {
                    hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, 0, 0); //last link
                }
                else
                {
                    hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, &fir_dma_in_desc[link_num + 1], 0);
                }
                fir_dma_in_desc[link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].sample  +fir_sample_start_l);
                fir_sample_start_l=(fir_dma_cfg_in.src_tsize+ fir_sample_start_l)%FIR_MEM_LEN;
                link_num++;

            }

        }
        else
        {
            fir_dma_cfg_in.src_tsize =len;
            link_num = 0;

            fir_dma_cfg_in.src = (uint32_t)(input);
            hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, 0, 0); //disable interupt
            fir_dma_in_desc[link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].sample   + fir_sample_start_l);
            fir_sample_start_l=(fir_dma_cfg_in.src_tsize+ fir_sample_start_l)%FIR_MEM_LEN;
        }

        fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_sample_num= fir_dma_cfg_in.src_tsize-1;

        //config fir filter out.
        if(loop!=0)
        {
            fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->loop_num=loop;
            *proclen = loop *(FIR_MEM_LEN-fir_run_cfg.coef_len);
        }
        else
        {
            fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->loop_num=1;
            *proclen = len;
        }


        //config out DMA
        fir_dma_cfg_out.dst_bsize = HAL_DMA_BSIZE_16;
        fir_dma_cfg_out.dst_periph = 0; //useless
        fir_dma_cfg_out.dst_width = HAL_DMA_WIDTH_WORD;
        fir_dma_cfg_out.handler = fir_irq_handler;
        fir_dma_cfg_out.src = 0; // useless
        fir_dma_cfg_out.src_bsize = HAL_DMA_BSIZE_16;
        fir_dma_cfg_out.src_periph = HAL_AUDMA_FIR_RX;
        fir_dma_cfg_out.src_width = HAL_DMA_WIDTH_WORD;
        fir_dma_cfg_out.try_burst = 1;
        fir_dma_cfg_out.type = HAL_DMA_FLOW_P2M_SI_DMA;

        if(loop!=0)
        {
            fir_dma_cfg_out.src_tsize = FIR_MEM_LEN-fir_run_cfg.coef_len;
            link_num = 0;

            for (i = 0; i < loop; i++)
            {
                fir_dma_cfg_out.dst = (uint32_t)(input + fir_dma_cfg_out.src_tsize*i);
                if(i==loop-1)
                {
                    hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, 0, 1); //enable interupt
                }
                else
                {
                    hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, &fir_dma_out_desc[link_num + 1], 0);
                }
                fir_dma_out_desc[link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].coef +fir_run_cfg.coef_len);
                link_num++;
            }
        }
        else
        {
            fir_dma_cfg_out.src_tsize =len;
            link_num = 0;

            fir_dma_cfg_out.dst = (uint32_t)(input);
            hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, 0, 1); //enable interupt
            fir_dma_out_desc[link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].coef+fir_run_cfg.coef_len);
        }
    }
    else if(fir_run_cfg.ch_num==2)
    {
        fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_sample_start=fir_sample_start_l;
        fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config0->fir_sample_start=fir_sample_start_r;

        loop = (len)/(FIR_MEM_LEN-fir_run_cfg.coef_len);

        ASSERT(loop <= FIREQ_DMA_LINKLIST_NUM_MAX, "[%s] loop:%u <= FIREQ_DMA_LINKLIST_NUM_MAX:%d", __func__, loop,FIREQ_DMA_LINKLIST_NUM_MAX);

        loop=loop/fir_run_cfg.ch_num;

        //config input DMA
        fir_dma_cfg_in.dst = 0; // useless
        fir_dma_cfg_in.dst_bsize = HAL_DMA_BSIZE_1;
        fir_dma_cfg_in.dst_periph = HAL_AUDMA_FIR_TX;
        fir_dma_cfg_in.dst_width = HAL_DMA_WIDTH_WORD;
        fir_dma_cfg_in.handler = 0; //NULL
        fir_dma_cfg_in.src_bsize = HAL_DMA_BSIZE_1;
        fir_dma_cfg_in.src_periph = 0; // useless
        fir_dma_cfg_in.src_width = HAL_DMA_WIDTH_WORD;
        fir_dma_cfg_in.try_burst = 1;
        fir_dma_cfg_in.type = HAL_DMA_FLOW_M2P_DI_DMA;

        if(loop!=0)
        {
            fir_dma_cfg_in.src_tsize = FIR_MEM_LEN-fir_run_cfg.coef_len;
            link_num = 0;

            for (i = 0; i < loop; i++)
            {
                fir_dma_cfg_in.src = (uint32_t)(input + fir_dma_cfg_in.src_tsize*fir_run_cfg.ch_num*i);
                hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, &fir_dma_in_desc[link_num + 1], 0);
                fir_dma_in_desc[link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].sample + fir_sample_start_l);
                fir_sample_start_l=(fir_dma_cfg_in.src_tsize+ fir_sample_start_l)%FIR_MEM_LEN;
                link_num++;

                fir_dma_cfg_in.src = (uint32_t)(input + fir_dma_cfg_in.src_tsize*fir_run_cfg.ch_num*i+1);
                if(i==loop-1)
                {
                    hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, 0, 0); //disable interupt
                }
                else
                {
                    hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, &fir_dma_in_desc[link_num + 1], 0);
                }
                fir_dma_in_desc[link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_2].sample  + fir_sample_start_r);
                fir_sample_start_r=(fir_dma_cfg_in.src_tsize+ fir_sample_start_r)%FIR_MEM_LEN;
                link_num++;

            }

        }
        else
        {
            fir_dma_cfg_in.src_tsize =len/fir_run_cfg.ch_num;
            link_num = 0;

            fir_dma_cfg_in.src = (uint32_t)(input);
            hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, &fir_dma_in_desc[link_num + 1], 0); //disable interupt
            fir_dma_in_desc[link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].sample   + fir_sample_start_l);
            fir_sample_start_l=(fir_dma_cfg_in.src_tsize+ fir_sample_start_l)%FIR_MEM_LEN;

            link_num++;

            fir_dma_cfg_in.src = (uint32_t)(input+1);
            hal_audma_init_desc(&fir_dma_in_desc[link_num], &fir_dma_cfg_in, 0, 0); //disable interupt
            fir_dma_in_desc[link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_2].sample   + fir_sample_start_r);
            fir_sample_start_r=(fir_dma_cfg_in.src_tsize+ fir_sample_start_r)%FIR_MEM_LEN;

        }

        fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_sample_num= fir_dma_cfg_in.src_tsize-1;
        fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config0->fir_sample_num= fir_dma_cfg_in.src_tsize-1;

        //config fir filter out.
        if(loop!=0)
        {
            fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->loop_num=loop;
            fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config1->loop_num=loop;

            *proclen = loop *(FIR_MEM_LEN-fir_run_cfg.coef_len)*fir_run_cfg.ch_num;
        }
        else
        {
            fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->loop_num=1;
            fir_setings.fir_ch_config[FIR_CHNL_ID_2].fir_ch_config1->loop_num=1;

            *proclen = len;
        }

        //config out DMA
        fir_dma_cfg_out.dst_bsize = HAL_DMA_BSIZE_1;
        fir_dma_cfg_out.dst_periph = 0; //useless
        fir_dma_cfg_out.dst_width = HAL_DMA_WIDTH_WORD;
        fir_dma_cfg_out.handler = fir_irq_handler;
        fir_dma_cfg_out.src = 0; // useless
        fir_dma_cfg_out.src_bsize = HAL_DMA_BSIZE_1;
        fir_dma_cfg_out.src_periph = HAL_AUDMA_FIR_RX;
        fir_dma_cfg_out.src_width = HAL_DMA_WIDTH_WORD;
        fir_dma_cfg_out.try_burst = 1;
        fir_dma_cfg_out.type = HAL_DMA_FLOW_P2M_SI_DMA;

        if(loop!=0)
        {
            fir_dma_cfg_out.src_tsize = FIR_MEM_LEN-fir_run_cfg.coef_len;
            link_num = 0;

            for (i = 0; i < loop; i++)
            {
                fir_dma_cfg_out.dst = (uint32_t)(input + fir_dma_cfg_out.src_tsize*fir_run_cfg.ch_num*i);
                hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, &fir_dma_out_desc[link_num + 1], 0);
                fir_dma_out_desc[link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].coef +fir_run_cfg.coef_len);

                link_num++;

                fir_dma_cfg_out.dst = (uint32_t)(input + fir_dma_cfg_out.src_tsize*fir_run_cfg.ch_num*i+1);
                if(i==loop-1)
                {
                    hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, 0, 1); //enable interupt
                }
                else
                {
                    hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, &fir_dma_out_desc[link_num + 1], 0);
                }
                fir_dma_out_desc[link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_2].coef +fir_run_cfg.coef_len);
                link_num++;
            }

        }
        else
        {
            fir_dma_cfg_out.src_tsize =len/fir_run_cfg.ch_num;
            link_num = 0;

            fir_dma_cfg_out.dst = (uint32_t)(input);
            hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, &fir_dma_out_desc[link_num + 1], 0); //disable interupt
            fir_dma_out_desc[link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].coef+fir_run_cfg.coef_len);

            link_num++;

            fir_dma_cfg_out.dst = (uint32_t)(input+1);
            hal_audma_init_desc(&fir_dma_out_desc[link_num], &fir_dma_cfg_out, 0, 1); //enable interupt
            fir_dma_out_desc[link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_2].coef+fir_run_cfg.coef_len);

        }

        memset(&fir_2d_src, 0, sizeof(fir_2d_src));
        memset(&fir_2d_dst, 0, sizeof(fir_2d_dst));

        fir_2d_src.xmodify = 1;
        fir_2d_src.xcount = fir_dma_cfg_out.src_tsize;
        fir_2d_src.ymodify = 0;
        fir_2d_src.ycount = 1;

        fir_2d_dst.xmodify = 1;
        fir_2d_dst.xcount = fir_dma_cfg_out.src_tsize;
        fir_2d_dst.ymodify = 0;
        fir_2d_dst.ycount = 1;
    }
    else
    {
        ASSERT(fir_run_cfg.ch_num < 2, "Bad channel number!: %u", fir_run_cfg.ch_num);
    }

    //LOG_I("%s:%d loop %d, ch1 %d, ch2 %d\n", __func__, __LINE__, loop, fir_dma_cfg_in.ch, fir_dma_cfg_out.ch);

    return 0;
}

#ifdef FIR_USE_IRQ

static void fir_irq_handler(uint8_t chan, uint32_t remain_tsize, uint32_t error, struct HAL_DMA_DESC_T *lli)
{
    FIR_PP_T pp;
    int32_t *buf = NULL;
    int16_t proclen = 0, offset = 0,  len = 0;

  //  LOG_I("%s\n", __func__);
    fir_stop();

    // init parameter
    pp = fir_run_cfg.pp;
    buf = fir_run_cfg.data[pp].buf;
    len = fir_run_cfg.data[pp].len;
    proclen = fir_run_cfg.proclen;
    offset = fir_run_cfg.offset;

    // store output data, set new data
    offset += proclen;

    ASSERT((uint32_t)len >= (uint32_t)offset, "Bad fir offset: %u len=%u", offset, len);

    if(len == offset) // OK!!!
    {
        fir_run_cfg.offset = 0;
        fir_run_cfg.proclen = 0;
        fir_run_cfg.data[pp].state = FIR_DATA_DONE;
    }
    else // Continue...
    {
        fir_fill_sample(buf+offset, len-offset, &proclen);

        fir_run_cfg.proclen = proclen;
        fir_run_cfg.offset = offset;

        fir_start();
    }

}

int fir_run(uint8_t *buf, uint32_t len)
{
    FIR_PP_T pp_empty;
    FIR_PP_T pp_full;
    FIR_DATA_STATE_T state_full;

    if(hw_fir_open_flag==1)
    {

        //LOG_I("%s,len:%d.\n", __func__, len);
        //LOG_I("[%s] len=%d pp=%d state=(%d,%d)", __func__, len, fir_run_cfg.pp, fir_run_cfg.data[0].state, fir_run_cfg.data[1].state);
        ASSERT(len <= fir_run_cfg.single_run_buf_len, "[%s] len(%u) > single_run_buf_len(%u)", __func__, len, fir_run_cfg.single_run_buf_len);

        pp_full = fir_run_cfg.pp;
        pp_empty = PP_PINGPANG(pp_full);
        // Make a copy of full state, because IRQ might change it
        state_full = fir_run_cfg.data[pp_full].state;

        ASSERT(fir_run_cfg.data[pp_empty].state == FIR_DATA_IDLE, "Invalid pp_empty state: pp_empty=%d state_empty=%d state_full=%d)",
        pp_empty, fir_run_cfg.data[pp_empty].state, state_full);

        // app --> eq
        memcpy_sample_to_int32(fir_run_cfg.data[pp_empty].buf, buf, len);

        // eq --> af
        if (state_full == FIR_DATA_DONE)
        {
            memcpy_int32_to_sample(buf, fir_run_cfg.data[pp_full].buf, len);
            fir_run_cfg.data[pp_full].state = FIR_DATA_IDLE;
        }
        else
        {
            // FIR_DATA_IDLE or FIR_DATA_BUSY
            memset_sample(buf, 0, len);
            if (state_full == FIR_DATA_BUSY)
            {
                LOG_I("[%s] pp_full is no full!!!", __func__);
                // Skip this frame
                return 0;
            }
        }

        // Update state
        fir_run_cfg.data[pp_empty].len = len;
        fir_run_cfg.data[pp_empty].state = FIR_DATA_BUSY;
        fir_run_cfg.pp = pp_empty;
        fir_fill_sample(fir_run_cfg.data[pp_empty].buf, fir_run_cfg.data[pp_empty].len, &fir_run_cfg.proclen);
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

inline static int fir_wait_finished(void)
{
  //  do{
        // Can add delay or switch thread
  //  }while(fir_ctrl.fs_ch[FIR_CHNL_ID_0].ctrl->FIR_BUSY || fir_ctrl.fs_ch[FIR_CHNL_ID_2].ctrl->FIR_BUSY);

    return 0;
}

int fir_run(uint8_t *buf, uint32_t len)
{
/*
    int16_t proclen = 0, offset = 0, next = 0;
    uint32_t fir_len = 0;
    int32_t *fir_buf = NULL;

    fir_len = len;
    fir_buf = (int32_t *)buf;

    do {
        fir_fill_sample(fir_buf+offset, fir_len-offset, &proclen, &next);
        fir_start();
        fir_wait_finished();
        fir_stop();
        fir_get_result(fir_buf+offset, proclen);
        offset+=proclen;
        fir_set_sample_addr(next);
    } while(offset < fir_len);
*/
    return 0;
}
#endif

int fir_set_cfg(const FIR_CFG_T *cfg)
{
    float gain = cfg->gain;
    int32_t coef_len = 0;
    const int32_t *coef_ptr = NULL;

    LOG_I("%s\n", __func__);

    if(hw_fir_open_flag==1)
    {
        if(gain>0.0f)
        {
            gain=0.0f;
        }
        gain = (float)powf(10,gain/20);
        coef_len = cfg->len;
        coef_ptr = cfg->coef;

        fir_run_cfg.coef_len = coef_len;

        ASSERT(coef_ptr != NULL, "[%s] coef == NULL", __func__);
        ASSERT(coef_len < FIR_MEM_LEN, "[%s] coef_len(%d) > FIR_MEM_LEN", __func__, coef_len);

        fir_setings.fir_ch_config[0].fir_ch_config0->fir_order=coef_len-1;
        fir_setings.fir_ch_config[0].fir_ch_config0->fir_sample_start=fir_sample_start_l=coef_len;
        fir_setings.fir_ch_config[0].fir_ch_config0->fir_sample_num=0;

        fir_setings.fir_ch_config[0].fir_ch_config1->result_base_addr=coef_len;
        fir_setings.fir_ch_config[0].fir_ch_config1->gain_sel=6;

        fir_setings.fir_ch_config[2].fir_ch_config0->fir_order=coef_len-1;
        fir_setings.fir_ch_config[2].fir_ch_config0->fir_sample_start=fir_sample_start_r=coef_len;
        fir_setings.fir_ch_config[2].fir_ch_config0->fir_sample_num=0;

        fir_setings.fir_ch_config[2].fir_ch_config1->result_base_addr=coef_len;
        fir_setings.fir_ch_config[2].fir_ch_config1->gain_sel=6;

        coefcpy_int16_to_int32(fir_setings.fir_ch_config[0].coef, coef_ptr, coef_len,gain);
        coefcpy_int16_to_int32(fir_setings.fir_ch_config[2].coef, coef_ptr, coef_len,gain);
        coefset_int16_to_int32(fir_setings.fir_ch_config[0].sample, 0x0, FIR_MEM_LEN);
        coefset_int16_to_int32(fir_setings.fir_ch_config[2].sample, 0x0, FIR_MEM_LEN);

        return 0;
    }
    else
    {
        LOG_I("[%s]HW FIR MUSET BE OPENED FIRSTLY!\n", __func__);
        return 1;
    }

    return 0;
}

int fir_needed_size(enum AUD_BITS_T sample_bits, int32_t frame_size)
{
    return frame_size * sizeof(int32_t) * 2;
}

int fir_open(enum AUD_SAMPRATE_T sample_rate, enum AUD_BITS_T sample_bits,enum AUD_CHANNEL_NUM_T ch_num, void *eq_buf, uint32_t len)
{
    LOG_I("%s\n", __func__);
    // Check parameter
    fir_run_cfg.sample_rate = sample_rate;
    fir_run_cfg.sample_bits = sample_bits;
    fir_run_cfg.ch_num = ch_num;

    // Parameter initialize
#ifdef FIR_USE_PINGPONG
    fir_run_cfg.pp = PP_PING;
    fir_run_cfg.data[0].state = FIR_DATA_IDLE;
    fir_run_cfg.data[1].state = FIR_DATA_IDLE;

    ASSERT(eq_buf && ((uint32_t)eq_buf & 0x3) == 0, "%s: Invalid eq_buf=%p", __func__, eq_buf);
    ASSERT(len && (len & 0x3) == 0, "%s: Invalid eq_buf len=%u", __func__, len);
    fir_run_cfg.single_run_buf_len = len / sizeof(int32_t) / 2;
    fir_run_cfg.data[0].buf = eq_buf;
    fir_run_cfg.data[1].buf = fir_run_cfg.data[0].buf + fir_run_cfg.single_run_buf_len;
#endif

#ifdef FIR_USE_IRQ
    // NOTE: offset MUST be initialized here as fir_run() will not do that
    fir_run_cfg.offset = 0;
    fir_run_cfg.proclen = 0;
#endif


    fir_setings.fir_config0->Stream_enable_ch0=0;
    fir_setings.fir_config0->Stream_enable_ch2= 0 ;

    fir_setings.fir_config0->Fir_enable_ch0= 0 ;
    fir_setings.fir_config0->Fir_enable_ch2= 0 ;

    fir_setings.fir_config0->Fir0_256p0_512p1= 1 ;
    fir_setings.fir_config0->Fir1_256p0_512p1= 1 ;

    fir_setings.fir_config0->dma_fir_rx_ctrl= 1 ;
    fir_setings.fir_config0->dma_fir_tx_ctrl= 1 ;

    fir_setings.fir_ch_config[0].fir_ch_config0->stream0_fir1=1;
    fir_setings.fir_ch_config[0].fir_ch_config0->fir_mode=0;
    fir_setings.fir_ch_config[0].fir_ch_config0->fir_order=0;
    fir_setings.fir_ch_config[0].fir_ch_config0->fir_sample_start=0;
    fir_setings.fir_ch_config[0].fir_ch_config0->fir_sample_num=0;

    fir_setings.fir_ch_config[0].fir_ch_config1->result_base_addr=0;
    fir_setings.fir_ch_config[0].fir_ch_config1->slide_offset=1;
    // fir_setings.fir_ch_config[0].fir_ch_config1->burst_length=0;
    fir_setings.fir_ch_config[0].fir_ch_config1->gain_sel=0;
    fir_setings.fir_ch_config[0].fir_ch_config1->loop_num=0;
    // fir_setings.fir_ch_config[0].fir_ch_config1->do_remap=0;


    fir_setings.fir_ch_config[2].fir_ch_config0->stream0_fir1=1;
    fir_setings.fir_ch_config[2].fir_ch_config0->fir_mode=0;
    fir_setings.fir_ch_config[2].fir_ch_config0->fir_order=0;
    fir_setings.fir_ch_config[2].fir_ch_config0->fir_sample_start=0;
    fir_setings.fir_ch_config[2].fir_ch_config0->fir_sample_num=0;

    fir_setings.fir_ch_config[2].fir_ch_config1->result_base_addr=0;
    fir_setings.fir_ch_config[2].fir_ch_config1->slide_offset=1;
    //  fir_setings.fir_ch_config[2].fir_ch_config1->burst_length=0;
    fir_setings.fir_ch_config[2].fir_ch_config1->gain_sel=0;
    fir_setings.fir_ch_config[2].fir_ch_config1->loop_num=0;
    //  fir_setings.fir_ch_config[2].fir_ch_config1->do_remap=0;

    fir_sample_start_l=0;
    fir_sample_start_r=0;

    memset(&fir_dma_cfg_in, 0, sizeof(fir_dma_cfg_in));
    fir_dma_cfg_in.ch = hal_audma_get_chan(HAL_AUDMA_FIR_TX, HAL_DMA_HIGH_PRIO);

    memset(&fir_dma_cfg_out, 0, sizeof(fir_dma_cfg_out));
    fir_dma_cfg_out.ch = hal_audma_get_chan(HAL_AUDMA_FIR_RX, HAL_DMA_HIGH_PRIO);

    hw_fir_open_flag=1;

    return 0;
}

int fir_close(void)
{
    LOG_I("%s\n", __func__);
    fir_stop();

    if(hw_fir_open_flag==1)
    {
        fir_stop();

        hal_dma_free_chan(fir_dma_cfg_in.ch);
        hal_dma_free_chan(fir_dma_cfg_out.ch);

        fir_dma_cfg_in.ch=HAL_DMA_CHAN_NONE;
        fir_dma_cfg_out.ch=HAL_DMA_CHAN_NONE;

        hw_fir_open_flag=0;

    }
    return 0;
}
