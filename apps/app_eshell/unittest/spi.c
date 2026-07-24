/***************************************************************************
 *
 * Copyright 2015-2022 BES.
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

#if defined(UTILS_ESHELL_EN)

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "hal_trace.h"
#include "hal_gpio.h"
#include "hal_spi.h"
#include "hal_sysfreq.h"
#include "crc_c.h"
#include "cmsis_os.h"
#include "eshell.h"

/**
 * 2700BP SYS SPI0 P4_0|SPI0_CLK P4_1|SPI0_CS P4_2|SPI0_DO P4_3|SPI0_DI
 *    SENSOR  SPI0 P1_0|SENS_CLK P1_1|SENS_CS P1_2|SENS_DO P1_3|SENS_DI
 */

#if defined(CHIP_BEST1502X) || defined(CHIP_BEST1503)
#undef HAS_SPILCD
#else
#define HAS_SPILCD
#endif

#if defined(CHIP_SUBSYS_SENS) || defined(CHIP_SUBSYS_BTH)
#define HAL_SYS_FREQ HAL_CMU_FREQ_96M
#else
#define HAL_SYS_FREQ HAL_CMU_FREQ_208M
#endif

#if defined(CHIP_SUBSYS_SENS) || defined(CHIP_SUBSYS_BTH)
#define SOFT_CS_PIN HAL_IOMUX_PIN_P1_1
#else
#define SOFT_CS_PIN HAL_IOMUX_PIN_P9_1
#endif

static int total_count = 0, error_count = 0;

enum TEST_SPI_RATE_T {
    SPI_RATE_1M = 1000000,
    SPI_RATE_6M = 6000000,
    SPI_RATE_12M = 12000000,
    SPI_RATE_24M = 24000000,
    SPI_RATE_48M = 48000000
};

enum TEST_SPI_DELAY_T { SAMPLE_DELAY_0, SAMPLE_DELAY_1, SAMPLE_DELAY_2, SAMPLE_DELAY_3 };

static void spi_wait_send_recv_done(uint8_t spi_id, int error)
{
    if (error != 0)
        eshell_putstring("spi send or recv error: %d\r\n", error);
#ifdef HAS_SPILCD
    if (spi_id == 1)
        while(hal_spilcd_busy());
    else
#endif
        while (hal_spi_busy());
}


static volatile uint8_t irecv_enable = 1;
static void spi_dma_recv_callback(int error)
{
    static uint32_t icnt = 0;
    icnt++;
    eshell_putstring("%s,%d\r\n", __func__, icnt);
    if (error != 0)
        eshell_putstring("spi recv error:%d\r\n", error);
    irecv_enable = 1;
}

POSSIBLY_UNUSED static void spi_loop_test(uint8_t spi_id)
{
    uint8_t tx_buff[4], rx_buff[8];
    uint8_t pat;
    int ret, num = 0;

    pat = rand() & 0xff;
    memset(tx_buff, pat, 4);
    memset(rx_buff , 0 , 8);
    //eshell_putstring("test spi id:%d, %x\r\n", TEST_SPI_ID, pat);

#ifdef HAS_SPILCD
    if (spi_id == 1)
        ret = hal_spilcd_dma_recv(tx_buff, rx_buff, 4, spi_dma_recv_callback);
    else
#endif
        ret = hal_spi_dma_recv(tx_buff, rx_buff, 4, spi_dma_recv_callback);

    spi_wait_send_recv_done(spi_id, ret);
    for (; num < 4; ++num) {
        if (rx_buff[num] != tx_buff[num]) {
            eshell_putstring("\r\nSPI test error: index=%d, tx_buff=0x%x, rx_buff=0x%x\r\n", num,
                             tx_buff[num], rx_buff[num]);
        }
    }
}


POSSIBLY_UNUSED static void spi_slavedebug_test_result(void)
{
    eshell_putstring("SPI slave debug  DONE\r\n");
    eshell_putstring("SPI slave debug test result : total count=%d, error_count=%d\r\n",
                     total_count, error_count);
}

static int spi_test(uint8_t spi_id, enum TEST_SPI_RATE_T spi_rate,
                    enum TEST_SPI_DELAY_T samp_delay, uint8_t width_bis)
{
    eshell_putstring("------spi test started------\r\n");

    hal_sysfreq_req(HAL_SYSFREQ_USER_INIT, HAL_SYS_FREQ);

    eshell_putstring("%s used id: %s\r\n", __FUNCTION__,
#if (spi_id == 1)
                     "SPI_ID_SLCD"
#else
                     "SPI_ID_0"
#endif
    );

    int ret = 0;
    int times = 0;
    struct HAL_SPI_CFG_T spi_cfg;
    memset(&spi_cfg, 0, sizeof(spi_cfg));

    spi_cfg.clk_delay_half = true; /* clk phase */
    spi_cfg.clk_polarity = true;   /* clk polarity in steady state*/
    spi_cfg.slave = false;
    // use dma
    spi_cfg.dma_rx = true;
    spi_cfg.dma_tx = true;

    spi_cfg.rx_sep_line = false;
    spi_cfg.cs = 0;
    spi_cfg.rate = spi_rate;
    spi_cfg.rx_bits = width_bis; //32
    spi_cfg.tx_bits = width_bis;
    spi_cfg.rx_frame_bits = 0;
    spi_cfg.samp_delay = 0;

    eshell_putstring("hal_spi_open: %d\r\n", spi_id);
#ifdef HAS_SPILCD
    if(spi_id == 1)
    {
        /* LCDSPI */
        hal_iomux_set_spilcd();
        ret = hal_spilcd_open(&spi_cfg);
        TRACE(2,"%s hal_spilcd_open ret[%d]", __FUNCTION__, ret);
    }else
#endif
    {
        /* SPI 0*/
        hal_iomux_set_spi();
        ret = hal_spi_open(&spi_cfg);
        TRACE(2,"%s hal_spi_open ret[%d]", __FUNCTION__, ret);
    }

#ifdef USE_SOFT_CS
    hal_iomux_init((enum HAL_GPIO_PIN_T)SOFT_CS_PIN, 1);
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)SOFT_CS_PIN , HAL_GPIO_DIR_OUT , 1);
#endif
    for (; times < 512; times++) {
        spi_loop_test(spi_id);
    }
    return 0;
}


static void unitest_usage(void)
{
    eshell_putstring("Usage: \r\n");
    eshell_putstring("  utest_spi Test Case\r\n");
    eshell_putstring(
        "  utest_spi loop_test|dma_test|detect_test|8bit_test|32bit_test|slavedebug_test\r\n");
}
/**
 * LOOP_TEST USE 48M,short circuit DI DO
 * DMA_TEST to verify 24M
 * DETECT_TEST to connect slave
 * 8BIT_TEST config rx/tx 8bit to read chip id
 * 32BIT_TEST config rx/tx 32bit to read chip id
 */
static void unitest_spi(int argc, char *argv[])
{
    if (argc < 2)
        goto usage;
    // test case
    if (strncmp(argv[1], "loop_test", 9) == 0)
    {
        if (argc < 3)
        {
            eshell_putstring("usage int: %d\r\n", argc);
            return;
        }
        uint8_t spi_id = atoi(argv[2]);
        uint8_t spi_rate = atoi(argv[3]);
        uint8_t bits_width = atoi(argv[4]);
        switch(spi_rate){
            case 0:
                spi_test(spi_id, SPI_RATE_1M, SAMPLE_DELAY_0, bits_width);
                break;
            case 1:
                spi_test(spi_id, SPI_RATE_6M, SAMPLE_DELAY_0, bits_width);
                break;
            case 2:
                spi_test(spi_id, SPI_RATE_12M, SAMPLE_DELAY_0, bits_width);
                break;
            case 3:
                spi_test(spi_id, SPI_RATE_24M, SAMPLE_DELAY_1, bits_width);
                break;
            case 4:
                spi_test(spi_id, SPI_RATE_48M, SAMPLE_DELAY_1, bits_width);
                break;
            default:
                eshell_putstring("not define spi rate:0 ~ 1M, 1 ~ 6M, 2 ~ 12M, 3 ~ 24M, 4 ~ 48m", argc);
                break;
        }
    }else
        goto usage;
    return;

usage:
    unitest_usage();
}
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_UNIT_TEST, "utest_spi", "usage: utest_spi help", unitest_spi);

#endif
