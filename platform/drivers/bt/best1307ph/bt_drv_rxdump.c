/***************************************************************************
 *
 * Copyright 2015-2026 BES.
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
#if defined(RX_IQ_DUMP) || defined(TX_IQ_CAL)
#include <string.h>
#include "hal_dma.h"
#include "bt_drv.h"
#include "hal_btdump.h"
#include "bt_drv_1307ph_internal.h"
#include "bt_drv_internal.h"
#include "besbt_string.h"
#include "bt_1307ph_reg_map.h"
#include CHIP_SPECIFIC_HDR(bt_drv_modem_reg_map)

typedef struct FFTCalcuFromDMA_
{
    short *M0data;
    float *win;
    short fft_len;
} FFTCalcuFromDMA;

#define FFT_SIZE 16384//1024//4096//2048//4096

volatile int iqimb_dma_status = 0;

FFTCalcuFromDMA *IQMismatchPreprocessState_init(int fft_size)
{
    // BT_DRV_TRACE(0,"malloc ini");

    FFTCalcuFromDMA *st = (FFTCalcuFromDMA *)bt_drv_malloc(1 * sizeof(FFTCalcuFromDMA));

    if (st){
        st->M0data = (short*)bt_drv_malloc(fft_size*2 * sizeof(short));

        // BT_DRV_TRACE(1,"[%s]malloc ok",__func__);
    }

    return st;
}

int32_t IQMismatchPreprocessState_destroy(FFTCalcuFromDMA *st)
{
    if (!st) {
        return 0;
    }
    bt_drv_free(st->M0data);
    bt_drv_free(st);

    return 0;
}

static struct HAL_DMA_DESC_T iqimb_dma_desc[32];
static uint8_t g_dma_channel = HAL_DMA_CHAN_NONE;
static FFTCalcuFromDMA *st = NULL;

static void iqimb_dma_dout_handler(uint32_t remains, uint32_t error, struct HAL_DMA_DESC_T *lli)
{
    if(g_dma_channel != HAL_DMA_CHAN_NONE)
    {
        hal_audma_free_chan(g_dma_channel);
    }
    hal_btdump_disable();
    iqimb_dma_status = 0;

    return;
}

#define TRANSFER_SIZE    1024
int bt_iqimb_dma_enable ( short * dma_dst_data, uint16_t size)
{
    int sRet = 0;
    struct HAL_DMA_CH_CFG_T iqimb_dma_cfg;
    // BT_DRV_TRACE(0,"bt_iqimb_dma_enable :");
    int times = 0;

    times = size/TRANSFER_SIZE;
    iqimb_dma_status = 1;

    sRet = memset_s(&iqimb_dma_cfg, sizeof(iqimb_dma_cfg), 0, sizeof(iqimb_dma_cfg));
    if (sRet){
        TRACE(1, "%s line:%d sRet:%d", __func__, __LINE__, sRet);
    }

    iqimb_dma_cfg.ch = hal_audma_get_chan(HAL_AUDMA_BTDUMP,HAL_DMA_HIGH_PRIO);
    ASSERT((HAL_DMA_CHAN_NONE != iqimb_dma_cfg.ch), "hal_audma_get_chan failed.");
    g_dma_channel = iqimb_dma_cfg.ch;
    iqimb_dma_cfg.dst_bsize = HAL_DMA_BSIZE_16;
    iqimb_dma_cfg.dst_periph = 0; //useless
    iqimb_dma_cfg.dst_width = HAL_DMA_WIDTH_WORD;
    iqimb_dma_cfg.handler = (HAL_DMA_IRQ_HANDLER_T)iqimb_dma_dout_handler;
    iqimb_dma_cfg.src = 0; // useless
    iqimb_dma_cfg.src_bsize = HAL_DMA_BSIZE_4;

    iqimb_dma_cfg.src_periph = HAL_AUDMA_BTDUMP;
    iqimb_dma_cfg.src_tsize = TRANSFER_SIZE;//size;//1600; //1600*2/26=123us
    iqimb_dma_cfg.src_width = HAL_DMA_WIDTH_WORD;
    iqimb_dma_cfg.try_burst = 1;
    iqimb_dma_cfg.type = HAL_DMA_FLOW_P2M_DMA;

	for(int i = 0;i < times; i++)
	{
        memset(&iqimb_dma_desc[i], 0, sizeof(iqimb_dma_desc[i]));
        iqimb_dma_cfg.dst = (uint32_t)(dma_dst_data + TRANSFER_SIZE * 2 * i);
        if(i < (times - 1))
        {
            hal_audma_init_desc(&iqimb_dma_desc[i], &iqimb_dma_cfg, &iqimb_dma_desc[i+1], 0);
        }
        else
        {
            hal_audma_init_desc(&iqimb_dma_desc[i], &iqimb_dma_cfg, 0, 1);
        }
    }

    hal_audma_sg_start(&iqimb_dma_desc[0], &iqimb_dma_cfg);

    hal_btdump_enable();

    return 1;
}

void check_mem_data(void* data, int len)
{
    short* share_mem = (short*)data;
    BT_DRV_TRACE(3,"check_mem_data :share_mem= %p, 0x%x, 0x%x",share_mem,share_mem[0],share_mem[1]);

    int32_t i =0;
    int32_t remain = len;

    while (remain > 0) {
        for(i = 0; i < 32; i++) {
            if (remain > 16) {
                DUMP16("%04X ",share_mem,16);
                share_mem +=16;
                remain -= 16;
            } else {
                DUMP16("%04X ",share_mem,remain);
                remain = 0;
                return;
            }
        }
        hal_sys_timer_delay(MS_TO_TICKS(100));
    }
}

void bt_iq_fft_calculate_init(void)
{
    int fftsize = FFT_SIZE;

    st = IQMismatchPreprocessState_init(fftsize);
}

void bt_iq_fft_calculate_run(void)
{
    uint32_t time_start = hal_sys_timer_get();
    int fftsize;

    BTDIGITAL_REG_WR(BESMDM_INT_TX_IQCOMP_BASE,0x0);

    fftsize = FFT_SIZE;
    time_start = hal_sys_timer_get();

    bt_iqimb_dma_enable(st->M0data, fftsize);
    while(1) {
        if(iqimb_dma_status == 0)
            break;
    }

    check_mem_data((uint8_t *)st->M0data, fftsize*2);
    BT_DRV_TRACE(1,"FFT cal use time: %d ms", __TICKS_TO_MS(hal_sys_timer_get()-time_start));

    IQMismatchPreprocessState_destroy(st);
}

void bt_iq_fft_calculate_init_txcali(int dump_length)
{
    int fftsize = dump_length;

    st = IQMismatchPreprocessState_init(fftsize);
}

short *bt_iq_fft_calculate_run_txcali(int dump_length)
{
    int fftsize;
    BTDIGITAL_REG_WR(BESMDM_INT_TX_IQCOMP_BASE,0x0);

    fftsize = dump_length;

    if (!st) {
        ASSERT(0, "%s, [IQ-FFT] st is NULL", __func__);
        return NULL;
    }

    bt_iqimb_dma_enable(st->M0data, fftsize);
    while (1) {
        if (iqimb_dma_status == 0) {
            break;
        }
    }
    return(st->M0data);
}

void bt_iq_fft_calculate_print_txcali(int dump_length)
{
    if (!st) {
        ASSERT(0, "%s, [IQ-FFT] st is NULL", __func__);
        return;
    }

    BT_DRV_TRACE(1,"[%s]malloc post",__func__);
    check_mem_data((uint8_t *)st->M0data, dump_length*2);
}

void bt_iq_fft_calculate_clear_txcali(void)
{
    IQMismatchPreprocessState_destroy(st);
}

static uint32_t POSSIBLY_UNUSED dpd_dig_reg_back[][2] =
{
    // {BESMDM_DUMP_ANA_CFG_ADDR, 0},
    // {BESMDM_RX_IQ_SWAP_ADDR, 0},
};

void btdrv_rx_iq_cal(void)
{
    // uint16_t tbl_size;

    // tbl_size = sizeof(dpd_dig_reg_back) / sizeof(dpd_dig_reg_back[0]);
    // for (uint16_t i = 0; i < tbl_size; i++) {
    //     dpd_dig_reg_back[i][1] = BTDIGITAL_REG(dpd_dig_reg_back[i][0]);
    // }

    while(1) {
        bt_iq_fft_calculate_init();

        // BTDIGITAL_REG(BESMDM_DUMP_ANA_CFG_ADDR) = 0x00000585;
        // BTDIGITAL_REG_SET_FIELD(BESMDM_RX_IQ_SWAP_ADDR, 1, 0, 0);
        // BTDIGITAL_REG_WR(BT_BES_TESTMODE_ADDR, 0x000A0080);

        bt_iq_fft_calculate_run();
        btdrv_delay(5*1000);
    }

    BT_DRV_TRACE(0,"reg_reset:\n");

    // tbl_size = sizeof(dpd_dig_reg_back) / sizeof(dpd_dig_reg_back[0]);
    // for (uint16_t i = 0; i < tbl_size; i++){
    //     BTDIGITAL_REG_WR(dpd_dig_reg_back[i][0], dpd_dig_reg_back[i][1]);
    // }
}
#endif
