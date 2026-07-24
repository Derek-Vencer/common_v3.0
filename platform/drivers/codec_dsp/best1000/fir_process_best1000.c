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
#include "math.h"

typedef signed int                          fir_sample_24bits_t;
typedef signed short int                    fir_sample_16bits_t;

// Hardware fir filter: enabled when USB_AUDIO_APP=1
// using irq and pingpong machine
//FIR_USE_IRQ
//FIR_USE_PINGPONG

#define FIR_MEM_LEN                         512

enum FIR_CHNL_ID_T {
    FIR_CHNL_ID_0 = 0,
    FIR_CHNL_ID_1 = 1,
    FIR_CHNL_ID_QTY
};

/**********************************mocro**************************************/
// ANC enable : 0x12000
//
// 位    Name    Type    功能
// 31 : 10  Rev R
// 9    Apb_anc_sel_pdu_ch1 r / w   1 : anc access memory, 0 : apb access memory
// 8    Apb_anc_sel_pdu_ch0 r / w   1 : anc access memory, 0 : apb access memory
// 7    Apb_anc_sel_fs_ch1  r / w   1 : anc access memory, 0 : apb access memory
// 6    Sel_gpadc_ch1   r / w   Same as Sel_gpadc_ch0
// 5    Sel_gpadc_ch0   r / w   0 : sel sigma - delta adc for anc, 1 : sel gpadc for anc
// 4    Anc_mode_ch1    r / w   Anc mode ch1, 0 for feedforward, 1 for feedback
// 3    Anc_mode_ch0    r / w   Anc mode ch0, 0 for feedforward, 1 for feedback
// 2    Anc_enable_ch1  r / w   使能ANC ch1  1 = enable。
// 1    Anc_enable_ch0  r / w   使能ANC ch0  1 = enable。
// 0    Apb_anc_sel_fs_ch0  r / w   1 : anc access memory, 0 : apb access memory
struct ANC_CONFIG_REG_T
{
    uint32_t APB_ANC_SEL_FS_CH0 : 1;
    uint32_t ANC_ENABLE_CH0 : 1;
    uint32_t ANC_ENABLE_CH1 : 1;
    uint32_t ANC_MODE_CH0 : 1;
    uint32_t ANC_MODE_CH1 : 1;
    uint32_t SEL_GPADC_CH0 : 1;
    uint32_t SEL_GPADC_CH1 : 1;
    uint32_t APB_ANC_SEL_FS_CH1 : 1;
    uint32_t APB_ANC_SEL_PDU_CH0 : 1;
    uint32_t APB_ANC_SEL_PDU_CH1 : 1;
    uint32_t GPADC_UNSIGNED : 1;
    uint32_t APB_ANC_SEL_FS_SAMPLE_CH0 : 1;
    uint32_t APB_ANC_SEL_FS_SAMPLE_CH1 : 1;
    uint32_t APB_ANC_SEL_PDU_SAMPLE_CH0 : 1;
    uint32_t APB_ANC_SEL_PDU_SAMPLE_CH1 : 1;
    uint32_t REV : 17;
};

// 位    Name    Type    功能
// 31:25    rev
// 24 : 16  fs_fir_order_Ch1    r / w   Fs fir order Ch1(配置成实际阶数 - 1)
// 15 : 9   rev
// 8 : 0    Fs_fir_order_Ch0    r / w   Fs fir order Ch0(配置成实际阶数 - 1)
// If 121 tap, need to config 120
struct FIR_ORDER_REG_T
{
    uint32_t ORDER_CH0 : 9;
    uint32_t REV0 : 7;
    uint32_t ORDER_CH1 : 9;
    uint32_t REV1 : 7;
};

//Gain select:  0x1200c
//位 Name    Type    功能
//31:23 Rev
//22:20 Pre_pdu_gain_ch1    r/w 0 for bypass, 1 for left shift 1, …..
//18:16 Pre_pdu_gain_ch0    r/w 0 for bypass, 1 for left shift 1, …..
//15:12 Fs gain select ch1  r/w
//11:8  Pdu gain select ch1 r/w
//7:4   Fs gain select ch0  r/w
///3:0  Pdu gain select ch0 r/w 6 for 0db
struct GAIN_SELECT_REG_T
{
    uint32_t PDU_GAIN_SELECT_CH0 : 4;
    uint32_t FS_GAIN_SELECT_CH0 : 4;
    uint32_t PDU_GAIN_SELECT_CH1 : 4;
    uint32_t FS_GAIN_SELECT_CH1 : 4;
    uint32_t PRE_PDU_GAIN_CH0 : 3;
    uint32_t REV1 : 1;
    uint32_t PRE_PDU_GAIN_CH1 : 3;
    uint32_t REV : 9;
};

//Mute Gain coef :  0x12010
//位 Name    Type    功能
//31:16 Rev
//15:8  Mute gain coef ch1  r/w
//7:0   Mute gain coef ch0  r/w 1.7, 0x7f  for  0db gain
struct MUTE_GAIN_COEF_REG_T
{
    uint32_t MUTE_GAIN_COEF_CH0 : 8;
    uint32_t MUTE_GAIN_COEF_CH1 : 8;
    uint32_t REV : 16;
};



// 31:27    Rev
// 26 : 18  result_base_addr    r / w   Fir结果在coef memory里存储的起始地址
// 17 : 9   fir_sample_num  r / w   一次中断计算的样点数.  (配置成实际点数 - 1)
// 8 : 0    fir_sample_start    r / w   从sample memory的某个sample开始计算fir
struct FIR_CONFIG_REG_T
{
    uint32_t FIR_SAMPLE_START : 9;
    uint32_t FIR_SAMPLE_NUM : 9;
    uint32_t RESULT_BASE_ADDR : 9;
    uint32_t REV : 5;
};


// 16:8 Sample cur addr r   Sample address being calculated.
// 4    Fir_irq_mask    r / w   Irq mask, 0 for mask
// 3    Fir_irq_status  r / w   Read 1, fir finished, trigger interrupt;  write 1, clear interrupt
// 2    Fir_busy    R   Read 1, fir busy;
// 1    fir_start   w   Write 1，trigger fir start
// 0    fir_mode    r / w   0：used for ANC    1：used for configurable fir
struct FIR_CONTROL_REG_T
{
    uint32_t FIR_MODE : 1;
    uint32_t FIR_START : 1;
    uint32_t FIR_BUSY : 1;
    uint32_t FIR_IRQ_STATUS : 1;
    uint32_t FIR_IRQ_MASK : 1;
    uint32_t SAMPLE_CUR_ADDR : 9;
    uint32_t REV : 18;
};


struct FIR_CH_CTRL_T {
    volatile struct FIR_CONFIG_REG_T *cfg;
    volatile struct FIR_CONTROL_REG_T *ctrl;
    volatile int16_t *coef;
    volatile int16_t *sample;
};

struct FIR_CTRL_T {
    volatile struct ANC_CONFIG_REG_T *anc_cfg_reg;
    volatile struct FIR_ORDER_REG_T *fs_order;
    volatile struct FIR_ORDER_REG_T *pdu_order;
    volatile struct GAIN_SELECT_REG_T *gain_sel;
    volatile struct MUTE_GAIN_COEF_REG_T *mute_gain_coef;
    volatile struct FIR_CH_CTRL_T fs_ch[2];
    volatile struct FIR_CH_CTRL_T pdu_ch[2];
};

#define ANC_BASE                            (CODEC_BASE + 0x8000)

static volatile const struct FIR_CTRL_T fir_ctrl = {
    .anc_cfg_reg   = (volatile struct ANC_CONFIG_REG_T *)(ANC_BASE + 0x0000),
    .fs_order  = (volatile struct FIR_ORDER_REG_T *)(ANC_BASE + 0x0004),
    .pdu_order = (volatile struct FIR_ORDER_REG_T *)(ANC_BASE + 0x0008),
    .gain_sel  = (volatile struct GAIN_SELECT_REG_T *)(ANC_BASE + 0x000c),
    .mute_gain_coef = (volatile struct MUTE_GAIN_COEF_REG_T *)(ANC_BASE + 0x0010),
    .fs_ch[0] = {
        .cfg    = (volatile struct FIR_CONFIG_REG_T *)(ANC_BASE + 0x0020),
        .ctrl   = (volatile struct FIR_CONTROL_REG_T *)(ANC_BASE + 0x0024),
        .coef   = (volatile int16_t *)(ANC_BASE + 0x2000),
        .sample =  (volatile int16_t *)(ANC_BASE + 0x4000),
    },
    .fs_ch[1] = {
        .cfg  = (volatile struct FIR_CONFIG_REG_T *)(ANC_BASE + 0x0028),
        .ctrl = (volatile struct FIR_CONTROL_REG_T *)(ANC_BASE + 0x002c),
        .coef   = (volatile int16_t *)(ANC_BASE + 0x2800),
        .sample =  (volatile int16_t *)(ANC_BASE + 0x4800),
    },
    .pdu_ch[0] = {
        .cfg  = (volatile struct FIR_CONFIG_REG_T *)(ANC_BASE + 0x0030),
        .ctrl = (volatile struct FIR_CONTROL_REG_T *)(ANC_BASE + 0x0034),
        .coef   = (volatile int16_t *)(ANC_BASE + 0x3000),
        .sample =  (volatile int16_t *)(ANC_BASE + 0x5000),
    },
    .pdu_ch[1] = {
        .cfg  = (volatile struct FIR_CONFIG_REG_T *)(ANC_BASE + 0x0038),
        .ctrl = (volatile struct FIR_CONTROL_REG_T *)(ANC_BASE + 0x003c),
        .coef   = (volatile int16_t *)(ANC_BASE + 0x3800),
        .sample =  (volatile int16_t *)(ANC_BASE + 0x5800),
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
    int16_t *buf;
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
    bool    irq_ch[FIR_CHNL_ID_QTY];
    int16_t offset;
    int16_t proclen;
    int16_t next;
#endif

    uint16_t coef_len;
    uint32_t sample_rate;
    uint32_t sample_bits;
    enum AUD_CHANNEL_NUM_T ch_num;
} FIR_RUN_CFG_T;

static FIR_RUN_CFG_T fir_run_cfg;

// Used by fir filter input, fir filter just can deal with 16 bits
static inline void memcpy_sample_to_int16(int16_t *dest, uint8_t *src, int num)
{
    if(fir_run_cfg.sample_bits == 16)
    {
        fir_sample_16bits_t *sample_src = (fir_sample_16bits_t *)src;
        for (int i = 0; i < num; i++)
        {
            dest[i] = sample_src[i];
        }
    }
    else
    {
        // 24 or 32 bits
        fir_sample_24bits_t *sample_src = (fir_sample_24bits_t *)src;
        for (int i = 0; i < num; i++)
        {
            dest[i] = sample_src[i] >> 8;    // fir: 16bit
        }
    }
}

// Used by fir filter output, dac just can deal with 16 or 18 bits
static inline void memcpy_int16_to_sample(uint8_t *dest, const int16_t *src, int num)
{
    if(fir_run_cfg.sample_bits == 16)
    {
        fir_sample_16bits_t *sample_dest = (fir_sample_16bits_t *)dest;
        for (int i = 0; i < num; i++)
        {
            sample_dest[i] = src[i];
        }
    }
    else
    {
        // 24 or 32 bits
        fir_sample_24bits_t *sample_dest = (fir_sample_24bits_t *)dest;
        for (int i = 0; i < num; i++)
        {
            sample_dest[i] = src[i] << 8;
        }
    }
}

static inline void memset_sample(uint8_t *dest, int value, int num)
{
    if(fir_run_cfg.sample_bits == 16)
    {
        //fir_sample_16bits_t *sample_dest = (fir_sample_16bits_t *)dest;
        for (int i = 0; i < num; i++)
        {
            dest[i] = value;
        }
    }
    else
    {
        // 24 or 32 bits
        //fir_sample_24bits_t *sample_dest = (fir_sample_24bits_t *)dest;
        for (int i = 0; i < num; i++)
        {
            dest[i] = value;
        }
    }
}

static void memcpy_int32_to_int16(int16_t *dest, const int32_t *src, int16_t num, float gain)
{
    int16_t i;
    for (i = 0; i < num; i++){
        dest[i] = (int16_t)((float)(src[i])*gain);
    }
    return;
}

static void memset_int16_to_int16(int16_t *dest, int16_t value, int16_t num)
{
    int16_t i;
    for (i = 0; i < num; i++){
        dest[i] = value;
    }
    return;
}

// hardware
inline static int fir_start(void)
{
    fir_ctrl.anc_cfg_reg->APB_ANC_SEL_FS_CH0 = 1;
    fir_ctrl.anc_cfg_reg->APB_ANC_SEL_FS_CH1 = 1;
    fir_ctrl.anc_cfg_reg->APB_ANC_SEL_FS_SAMPLE_CH0 = 1;
    fir_ctrl.anc_cfg_reg->APB_ANC_SEL_FS_SAMPLE_CH1 = 1;

    fir_ctrl.fs_ch[FIR_CHNL_ID_0].ctrl->FIR_START = 1;
    fir_ctrl.fs_ch[FIR_CHNL_ID_1].ctrl->FIR_START = 1;
    return 0;
}

inline static int fir_stop(void)
{
    fir_ctrl.fs_ch[FIR_CHNL_ID_0].ctrl->FIR_START = 0;
    fir_ctrl.fs_ch[FIR_CHNL_ID_1].ctrl->FIR_START = 0;

    fir_ctrl.anc_cfg_reg->APB_ANC_SEL_FS_SAMPLE_CH0 = 0;
    fir_ctrl.anc_cfg_reg->APB_ANC_SEL_FS_SAMPLE_CH1 = 0;
    fir_ctrl.anc_cfg_reg->APB_ANC_SEL_FS_CH0 = 0;
    fir_ctrl.anc_cfg_reg->APB_ANC_SEL_FS_CH1 = 0;

    return 0;
}

inline static int fir_set_sample_addr(int16_t addr)
{
    fir_ctrl.fs_ch[FIR_CHNL_ID_0].cfg->FIR_SAMPLE_START = addr;
    fir_ctrl.fs_ch[FIR_CHNL_ID_1].cfg->FIR_SAMPLE_START = addr;

    return 0;
}

inline static int fir_get_result(int16_t output[], int16_t len)
{
    int16_t i,frameLen;
    int16_t *src_r, *src_l;

    frameLen = len>>1;

    src_r = (int16_t *)fir_ctrl.fs_ch[FIR_CHNL_ID_0].coef + fir_ctrl.fs_ch[FIR_CHNL_ID_0].cfg->RESULT_BASE_ADDR;
    src_l = (int16_t *)fir_ctrl.fs_ch[FIR_CHNL_ID_1].coef + fir_ctrl.fs_ch[FIR_CHNL_ID_1].cfg->RESULT_BASE_ADDR;

    // Output LRLRLR...
    for (i = 0; i < frameLen; i++)
    {
        *output++ = src_r[i];
        *output++ = src_l[i];
    }

    return 0;
}

inline static int fir_fill_sample(const int16_t input[], int16_t len, int16_t *proclen, int16_t *next)
{
    int16_t mono_len;
    int16_t fir_sample_start;
    int16_t *dest_r, *dest_l;
    int16_t i;
    int16_t fir_sample_end;

    // TODO: Check input

    mono_len = len>>1;
    // FIXME: use value to replace EQ_FIR_COEF_LEN
    if (mono_len>(FIR_MEM_LEN-fir_run_cfg.coef_len))
    {
        mono_len = FIR_MEM_LEN-fir_run_cfg.coef_len;
    }

    fir_sample_start = fir_ctrl.fs_ch[FIR_CHNL_ID_0].cfg->FIR_SAMPLE_START;
    ASSERT((uint32_t)fir_sample_start < FIR_MEM_LEN, "Bad FIR_SAMPLE_START: %u", fir_sample_start);

    dest_r = (int16_t *)fir_ctrl.fs_ch[FIR_CHNL_ID_0].sample + fir_sample_start;
    dest_l = (int16_t *)fir_ctrl.fs_ch[FIR_CHNL_ID_1].sample + fir_sample_start;

    fir_sample_end = FIR_MEM_LEN - fir_sample_start;
    if (fir_sample_end >= mono_len)
    {
        for (i = 0; i < mono_len; i++){
            dest_r[i] = *input++;
            dest_l[i] = *input++;
        }
        fir_sample_start = fir_sample_start + mono_len;
        if (fir_sample_start == FIR_MEM_LEN) {
            fir_sample_start = 0;
        }
    }
    else
    {
        for (i = 0; i < fir_sample_end; i++){
            dest_r[i] = *input++;
            dest_l[i] = *input++;
        }
        dest_r = (int16_t *)fir_ctrl.fs_ch[FIR_CHNL_ID_0].sample;
        dest_l = (int16_t *)fir_ctrl.fs_ch[FIR_CHNL_ID_1].sample;
        fir_sample_end = mono_len - fir_sample_end;
        for (i = 0; i < fir_sample_end; i++){
            dest_r[i] = *input++;
            dest_l[i] = *input++;
        }
        fir_sample_start = fir_sample_end;
    }

    fir_ctrl.fs_ch[FIR_CHNL_ID_0].cfg->FIR_SAMPLE_NUM = mono_len - 1;
    fir_ctrl.fs_ch[FIR_CHNL_ID_1].cfg->FIR_SAMPLE_NUM = mono_len - 1;

    *next = fir_sample_start;
    *proclen = mono_len<<1;

    return 0;
}

#ifdef FIR_USE_IRQ
void fir_irq_handler(void)
{
    for(int i=0;i<FIR_CHNL_ID_QTY;i++)
    {
        if (fir_ctrl.fs_ch[i].ctrl->FIR_IRQ_STATUS == 1)
        {
            fir_run_cfg.irq_ch[i] = true;
            fir_ctrl.fs_ch[i].ctrl->FIR_IRQ_STATUS = 1;
        }
    }

    if(fir_run_cfg.irq_ch[0] && fir_run_cfg.irq_ch[1])
    {
        fir_stop();
        fir_run_cfg.irq_ch[0] = false;
        fir_run_cfg.irq_ch[1] = false;

        FIR_PP_T pp;
        int16_t *buf = NULL;
        int16_t proclen = 0, offset = 0, next = 0, len = 0;

        // init parameter
        pp = fir_run_cfg.pp;
        buf = fir_run_cfg.data[pp].buf;
        len = fir_run_cfg.data[pp].len;
        proclen = fir_run_cfg.proclen;
        offset = fir_run_cfg.offset;
        next = fir_run_cfg.next;

        // store output data, set new data
        fir_get_result(buf+offset, proclen);
        fir_set_sample_addr(next);
        offset += proclen;

        ASSERT((uint32_t)len >= (uint32_t)offset, "Bad fir offset: %u len=%u", offset, len);
        if(len == offset) // OK!!!
        {
            fir_run_cfg.next = 0;
            fir_run_cfg.offset = 0;
            fir_run_cfg.proclen = 0;
            fir_run_cfg.data[pp].state = FIR_DATA_DONE;
        }
        else // Continue...
        {
            fir_fill_sample(buf+offset, len-offset, &proclen, &next);

            fir_run_cfg.proclen = proclen;
            fir_run_cfg.offset = offset;
            fir_run_cfg.next = next;

            fir_start();
        }
    }
}

int fir_run(uint8_t *buf, uint32_t len)
{
    FIR_PP_T pp_empty;
    FIR_PP_T pp_full;
    FIR_DATA_STATE_T state_full;

    //LOG_I("[%s] len=%d pp=%d state=(%d,%d)", __func__, len, fir_run_cfg.pp, fir_run_cfg.data[0].state, fir_run_cfg.data[1].state);
    ASSERT(len <= fir_run_cfg.single_run_buf_len, "[%s] len(%u) > single_run_buf_len(%u)", __func__, len, fir_run_cfg.single_run_buf_len);

    pp_full = fir_run_cfg.pp;
    pp_empty = PP_PINGPANG(pp_full);
    // Make a copy of full state, because IRQ might change it
    state_full = fir_run_cfg.data[pp_full].state;

    ASSERT(fir_run_cfg.data[pp_empty].state == FIR_DATA_IDLE, "Invalid pp_empty state: pp_empty=%d state_empty=%d state_full=%d)",
        pp_empty, fir_run_cfg.data[pp_empty].state, state_full);

    // app --> eq
    memcpy_sample_to_int16(fir_run_cfg.data[pp_empty].buf, buf, len);

    // eq --> af
    if (state_full == FIR_DATA_DONE) {
        memcpy_int16_to_sample(buf, fir_run_cfg.data[pp_full].buf, len);
        fir_run_cfg.data[pp_full].state = FIR_DATA_IDLE;
    } else {
        // FIR_DATA_IDLE or FIR_DATA_BUSY
        memset_sample(buf, 0, len);
        if (state_full == FIR_DATA_BUSY) {
            LOG_I("[%s] pp_full is no full!!!", __func__);
            // Skip this frame
            return 0;
        }
    }

    // Update state
    fir_run_cfg.data[pp_empty].len = len;
    fir_run_cfg.data[pp_empty].state = FIR_DATA_BUSY;
    fir_run_cfg.pp = pp_empty;
    fir_fill_sample(fir_run_cfg.data[pp_empty].buf, fir_run_cfg.data[pp_empty].len, &fir_run_cfg.proclen, &fir_run_cfg.next);
    fir_start();

    return 0;
}

#else

inline static int fir_wait_finished(void)
{
    do{
        // Can add delay or switch thread
    }while(fir_ctrl.fs_ch[FIR_CHNL_ID_0].ctrl->FIR_BUSY || fir_ctrl.fs_ch[FIR_CHNL_ID_1].ctrl->FIR_BUSY);

    return 0;
}

int fir_run(uint8_t *buf, uint32_t len)
{
    int16_t proclen = 0, offset = 0, next = 0;
    uint32_t fir_len = 0;
    int16_t *fir_buf = NULL;

    fir_len = len;
    fir_buf = (int16_t *)buf;

    do {
        fir_fill_sample(fir_buf+offset, fir_len-offset, &proclen, &next);
        fir_start();
        fir_wait_finished();
        fir_stop();
        fir_get_result(fir_buf+offset, proclen);
        offset+=proclen;
        fir_set_sample_addr(next);
    } while(offset < fir_len);

    return 0;
}
#endif

int fir_set_cfg(const FIR_CFG_T *cfg)
{
    float gain = 0.0f;
    int32_t coef_len = 0;
    const int32_t *coef_ptr = NULL;

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

    fir_ctrl.gain_sel->FS_GAIN_SELECT_CH0 = 6;
    fir_ctrl.gain_sel->FS_GAIN_SELECT_CH1 = 6;

    fir_ctrl.fs_order->ORDER_CH0 = coef_len-1;
    fir_ctrl.fs_order->ORDER_CH1 = coef_len-1;

    fir_ctrl.fs_ch[0].cfg->FIR_SAMPLE_START = coef_len;
    fir_ctrl.fs_ch[1].cfg->FIR_SAMPLE_START = coef_len;

    fir_ctrl.fs_ch[0].cfg->RESULT_BASE_ADDR = coef_len;
    fir_ctrl.fs_ch[1].cfg->RESULT_BASE_ADDR = coef_len;

    fir_ctrl.anc_cfg_reg->APB_ANC_SEL_FS_CH0 = 0;
    memcpy_int32_to_int16((int16_t *)fir_ctrl.fs_ch[0].coef, coef_ptr, coef_len, gain);
    fir_ctrl.anc_cfg_reg->APB_ANC_SEL_FS_CH0 = 1;

    fir_ctrl.anc_cfg_reg->APB_ANC_SEL_FS_CH1 = 0;
    memcpy_int32_to_int16((int16_t *)fir_ctrl.fs_ch[1].coef, coef_ptr, coef_len, gain);
    fir_ctrl.anc_cfg_reg->APB_ANC_SEL_FS_CH1 = 1;

    fir_ctrl.anc_cfg_reg->APB_ANC_SEL_FS_SAMPLE_CH0 = 0;
    memset_int16_to_int16((int16_t *)fir_ctrl.fs_ch[0].sample, 0x0, FIR_MEM_LEN);
    fir_ctrl.anc_cfg_reg->APB_ANC_SEL_FS_SAMPLE_CH0 = 0;

    fir_ctrl.anc_cfg_reg->APB_ANC_SEL_FS_SAMPLE_CH1 = 0;
    memset_int16_to_int16((int16_t *)fir_ctrl.fs_ch[1].sample, 0x0, FIR_MEM_LEN);
    fir_ctrl.anc_cfg_reg->APB_ANC_SEL_FS_SAMPLE_CH1 = 0;

    return 0;
}

int fir_needed_size(enum AUD_BITS_T sample_bits, int32_t frame_size)
{
    return frame_size * sizeof(int16_t) * 2;
}

int fir_open(enum AUD_SAMPRATE_T sample_rate, enum AUD_BITS_T sample_bits,enum AUD_CHANNEL_NUM_T ch_num, void *eq_buf, uint32_t len)
{
    // Check parameter
    fir_run_cfg.sample_rate = sample_rate;
    fir_run_cfg.sample_bits = sample_bits;

    // Parameter initialize
#ifdef FIR_USE_PINGPONG
    fir_run_cfg.pp = PP_PING;
    fir_run_cfg.data[0].state = FIR_DATA_IDLE;
    fir_run_cfg.data[1].state = FIR_DATA_IDLE;

    ASSERT(eq_buf && ((uint32_t)eq_buf & 0x3) == 0, "%s: Invalid eq_buf=%p", __func__, eq_buf);
    ASSERT(len && (len & 0x3) == 0, "%s: Invalid eq_buf len=%u", __func__, len);
    fir_run_cfg.single_run_buf_len = len / sizeof(int16_t) / 2;
    fir_run_cfg.data[0].buf = eq_buf;
    fir_run_cfg.data[1].buf = fir_run_cfg.data[0].buf + fir_run_cfg.single_run_buf_len;
#endif

#ifdef FIR_USE_IRQ
    fir_run_cfg.irq_ch[0] = false;
    fir_run_cfg.irq_ch[1] = false;
    // NOTE: offset MUST be initialized here as fir_run() will not do that
    fir_run_cfg.offset = 0;
    fir_run_cfg.proclen = 0;
    fir_run_cfg.next = 0;
#endif

#ifdef CHIP_BEST1000
    hal_cmu_anc_enable(HAL_CMU_ANC_CLK_USER_EQ);
#endif

    fir_ctrl.fs_ch[0].ctrl->FIR_MODE = 1;
    fir_ctrl.fs_ch[1].ctrl->FIR_MODE = 1;

#ifdef FIR_USE_IRQ
    fir_ctrl.fs_ch[0].ctrl->FIR_IRQ_STATUS = 1;
    fir_ctrl.fs_ch[1].ctrl->FIR_IRQ_STATUS = 1;
    fir_ctrl.fs_ch[0].ctrl->FIR_IRQ_MASK = 1;
    fir_ctrl.fs_ch[1].ctrl->FIR_IRQ_MASK = 1;
    NVIC_SetVector(CODEC_IRQn, (uint32_t)fir_irq_handler);
    NVIC_SetPriority(CODEC_IRQn, IRQ_PRIORITY_NORMAL);
    NVIC_ClearPendingIRQ(CODEC_IRQn);
    NVIC_EnableIRQ(CODEC_IRQn);
#endif

    return 0;
}

int fir_close(void)
{
#ifdef CHIP_BEST1000
    if (hal_cmu_anc_get_status(HAL_CMU_ANC_CLK_USER_EQ) == 0) {
        return 0;
    }
#endif

    fir_stop();

#ifdef FIR_USE_IRQ
    NVIC_DisableIRQ(CODEC_IRQn);
    fir_ctrl.fs_ch[0].ctrl->FIR_IRQ_MASK = 0;
    fir_ctrl.fs_ch[1].ctrl->FIR_IRQ_MASK = 0;
    fir_ctrl.fs_ch[0].ctrl->FIR_IRQ_STATUS = 1;
    fir_ctrl.fs_ch[1].ctrl->FIR_IRQ_STATUS = 1;
#endif

#ifdef CHIP_BEST1000
    hal_cmu_anc_disable(HAL_CMU_ANC_CLK_USER_EQ);
#endif

    return 0;
}
