/***************************************************************************
 *
 * Copyright 2015-2024 BES.
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

#if defined(UTILS_ESHELL_EN) && defined(__SYS_AS_MAIN__)

#include "plat_addr_map.h"

#ifdef SDMMC0_BASE

#include "hal_sdio_host.h"
#include "hal_sysfreq.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include <string.h>
#include "pmu.h"
#include "stdlib.h"
#include "eshell.h"

#define SDIO_HOST_DELAY_MS(ms)         hal_sys_timer_delay(MS_TO_TICKS(ms))
#define SDIO_HOST_DELAY_US(us)         hal_sys_timer_delay_us(us)

#define SDIO_HOST_ID                   HAL_SDIO_HOST_ID_0
#define SDMMC0_PULL_UP_ENABLE

#define BUFF_LEN               32768
static uint8_t data_buff[BUFF_LEN];

static volatile uint8_t enable_read_device = 0;
static uint16_t real_read_len = 0;

POSSIBLY_UNUSED static volatile uint8_t dma_txrx_done = 0;

static void hal_sdio_host_card_detect(void)
{
    eshell_putstring("------sdio host detected a card\r\n");
}

static void hal_sdio_host_signal_voltage_switch(void)
{
    eshell_putstring("------sdio host signal voltage switching completed\r\n");
}

static void hal_sdio_host_error(enum HAL_SDIO_HOST_ERR error)
{
    eshell_putstring("******sdio host error:%d\r\n", error);
}

static void hal_sdio_host_func_int(enum HAL_SDIO_FUNC_NUM_T func_num)
{
    eshell_putstring("------host has received device func%d int\r\n", func_num);
    uint8_t int_data_lsb = 0;
    uint8_t int_data_hsb = 0;
#define SDIO_FUNC1_INT_VALUE1       0x0
#define SDIO_FUNC1_INT_VALUE2       0x1

    hal_sdio_io_rw_direct(SDIO_HOST_ID, HAL_SDIO_DEVICE_READ, func_num, SDIO_FUNC1_INT_VALUE1, 0, &int_data_lsb);
    hal_sdio_io_rw_direct(SDIO_HOST_ID, HAL_SDIO_DEVICE_READ, func_num, SDIO_FUNC1_INT_VALUE2, 0, &int_data_hsb);

    enable_read_device = 1;
    real_read_len = ((uint16_t)(int_data_hsb << 8)) + int_data_lsb;
}

static void hal_sdio_host_dma_tx_done(uint32_t remain_size, uint32_t error)
{
    eshell_putstring("------host tx done\r\n");

    dma_txrx_done = 1;
}

static void hal_sdio_host_dma_rx_done(uint32_t remain_size, uint32_t error)
{
    eshell_putstring("------host rx done\r\n");
    dma_txrx_done = 1;
}

static struct HAL_SDIO_HOST_CB_T sdio_host_test_dma_callback = {
    .hal_sdio_host_card_detect = hal_sdio_host_card_detect,
    .hal_sdio_host_signal_voltage_switch = hal_sdio_host_signal_voltage_switch,
    .hal_sdio_host_error = hal_sdio_host_error,
    .hal_sdio_host_func_int = hal_sdio_host_func_int,
    .hal_sdio_host_dma_tx_done = hal_sdio_host_dma_tx_done,
    .hal_sdio_host_dma_rx_done = hal_sdio_host_dma_rx_done,
    .hal_sdio_host_sem_init = NULL,
    .hal_sdio_host_sem_reset = NULL,
    .hal_sdio_host_sem_post = NULL,
    .hal_sdio_host_sem_wait = NULL,
};

static struct HAL_SDIO_HOST_CB_T sdio_host_test_callback = {
    .hal_sdio_host_card_detect = hal_sdio_host_card_detect,
    .hal_sdio_host_signal_voltage_switch = hal_sdio_host_signal_voltage_switch,
    .hal_sdio_host_error = hal_sdio_host_error,
    .hal_sdio_host_func_int = hal_sdio_host_func_int,
    .hal_sdio_host_dma_tx_done = NULL,
    .hal_sdio_host_dma_rx_done = NULL,
    .hal_sdio_host_sem_init = NULL,
    .hal_sdio_host_sem_reset = NULL,
    .hal_sdio_host_sem_post = NULL,
    .hal_sdio_host_sem_wait = NULL,
};

static void sdio_host_test(uint8_t dma_enable, uint32_t speed_hz)
{
    uint32_t i;
    uint8_t  response_r5;
    uint8_t  buf_cnt = 0;
    uint8_t  capability;
    uint8_t  bus_speed_mode;
    uint16_t device_rx_buf_len_max;
    uint16_t real_write_len = 0;
    uint16_t aligned_write_len = 0;
    uint16_t blocksize;
    uint16_t *write_p = (uint16_t *)data_buff;
    struct HAL_SDIO_HOST_CONFIG_T sdio_cfg;
    enum HAL_SDIO_ERR err;

    eshell_putstring("------sdio host test started\r\n");
    if (dma_enable) {
        eshell_putstring("------sdio host runs in dma transceiver non-blocking/callback mode\r\n");
    } else {
        eshell_putstring("------sdio host runs in non-dma transceiver blocking mode\r\n");
    }
    hal_sysfreq_req(HAL_SYSFREQ_USER_INIT, HAL_CMU_FREQ_208M);
    eshell_putstring("CPU frequency changed to: %uM\r\n", hal_sys_timer_calc_cpu_freq(5, 0) / 1000000);
    SDIO_HOST_DELAY_MS(5);//For palladium and chip simulation:wait device run first,2021-05-15

    memset(&sdio_cfg, 0, sizeof(sdio_cfg));
    sdio_cfg.yield = 0;
    if (dma_enable) {
        sdio_cfg.dma_en = 1;
    } else {
        sdio_cfg.dma_en = 0;
    }
    sdio_cfg.ddr_mode = 0;
    sdio_cfg.func_en = 1;
    sdio_cfg.send_abort = 1;
    sdio_cfg.func1_only = 1;
    sdio_cfg.async_int_en = 1;
    sdio_cfg.set_func_blks = 1;
    sdio_cfg.volt_switch = 1;
    sdio_cfg.blocking_init_en = 0;
    sdio_cfg.block_size = 256;
    sdio_cfg.bus_speed = speed_hz;
    sdio_cfg.bus_width = 4;
    if (dma_enable) {
        sdio_cfg.callback = &sdio_host_test_dma_callback;
    } else {
        sdio_cfg.callback = &sdio_host_test_callback;
    }
    err = hal_sdio_host_open(SDIO_HOST_ID, &sdio_cfg);
    if (err) {
        eshell_putstring("******hal_sdio_host_open error:%d\r\n", err);
        return;
    }

    hal_sdio_get_block_size(SDIO_HOST_ID, HAL_SDIO_FUNC_1, &blocksize);
    eshell_putstring("------sdio block size:%d\r\n", blocksize);
    hal_sdio_get_capability(SDIO_HOST_ID, &capability);
    eshell_putstring("------sdio device capability:0x%X\r\n", capability);
    hal_sdio_get_bus_speed(SDIO_HOST_ID, &bus_speed_mode);
    eshell_putstring("------sdio speed mode:%d\r\n", bus_speed_mode);
    eshell_putstring("------sdio bus speed:%d\r\n", hal_sdio_get_host_speed(SDIO_HOST_ID));

#ifdef ENABLE_WIFI_TXRX_TEST
    //---------------------------read test----------------------------------
    if (enable_read_device) {
        enable_read_device = 0;
        eshell_putstring("------sdio host read real len:%d\r\n", real_read_len);
        ASSERT(real_read_len && (real_read_len <= BUFF_LEN), "%s:%d,sdio host get device tx buf len:%d", __func__, __LINE__, real_read_len);
        memset(data_buff, 0xAA, sizeof(data_buff));
        if (dma_enable) {
            dma_txrx_done = 0;
        }

        hal_sdio_io_rw_extended(SDIO_HOST_ID, HAL_SDIO_DEVICE_READ, HAL_SDIO_FUNC_1, 0,
                                &response_r5, 1, (uint8_t *)data_buff, real_read_len);
        if (dma_enable) {
            while (!dma_txrx_done) {
                SDIO_HOST_DELAY_US(10);
            }
            dma_txrx_done = 0;
        }
        eshell_putstring("------sdio cmd53 read complete------,rsp=0x%X\r\n", response_r5);
    }
#else
    //---------------------------read test----------------------------------
wait_device_int:
    eshell_putstring("------The host actively sends an interrupt to the device\r\n");
    hal_sdio_host_gen_int_to_device(SDIO_HOST_ID);
    eshell_putstring("------wait device int\r\n");
    i = 0;
    while (!enable_read_device) {
        SDIO_HOST_DELAY_MS(1);

        i++;
        if (i >= 1000) {  //1s timeout
            goto wait_device_int;
        }
    }
    enable_read_device = 0;
    eshell_putstring("------sdio host read real len:%d\r\n", real_read_len);
    ASSERT(real_read_len && (real_read_len <= BUFF_LEN), "%s:%d,sdio host get device tx buf len:%d", __func__, __LINE__, real_read_len);
    memset(data_buff, 0xAA, sizeof(data_buff));
    if (dma_enable) {
        dma_txrx_done = 0;
    }
    hal_sdio_io_rw_extended(SDIO_HOST_ID, HAL_SDIO_DEVICE_READ, HAL_SDIO_FUNC_1, 0,
                            &response_r5, 1, (uint8_t *)data_buff, real_read_len);
    if (dma_enable) {
        while (!dma_txrx_done) {
            SDIO_HOST_DELAY_US(10);
        }
        dma_txrx_done = 0;
    }
    eshell_putstring("------sdio cmd53 read complete------,rsp=0x%X\r\n", response_r5);
#endif

    //----------------------------write test--------------------------------
    //write
    write_p = (uint16_t *)data_buff;
    for (i = 0; i < BUFF_LEN / 2; i++) {
        write_p[i] = i;
    }

#ifndef ENABLE_WIFI_TXRX_TEST
wait_buf_cnt:
#endif
    if (!buf_cnt) {
        hal_sdio_host_get_func1_rx_buf_cfg(SDIO_HOST_ID, &buf_cnt, &device_rx_buf_len_max);
        eshell_putstring("------sdio device rx buf cnt:%d,buf len max:%d\r\n", buf_cnt, device_rx_buf_len_max);
    }
    if (BUFF_LEN < device_rx_buf_len_max) {
        eshell_putstring("%s:%d\r\n", __func__, __LINE__);
        ASSERT(0, "******sdio host buf is too short %d ,len at least %d", BUFF_LEN, device_rx_buf_len_max);
    }

    if (buf_cnt) {
        real_write_len = 3072;
        aligned_write_len = (real_write_len + 3) & (~3);    //Round up to a multiple of 4 bytes
        eshell_putstring("------sdio host tx data size:%d\r\n", real_write_len);
        if (dma_enable) {
            dma_txrx_done = 0;
        }
        hal_sdio_io_rw_extended(SDIO_HOST_ID, HAL_SDIO_DEVICE_WRITE, HAL_SDIO_FUNC_1, real_write_len,
                                &response_r5, 1, (uint8_t *)data_buff, aligned_write_len);
        if (dma_enable) {
            while (!dma_txrx_done) {
                SDIO_HOST_DELAY_US(10);
            }
            dma_txrx_done = 0;
        }
        eshell_putstring("------sdio cmd53 write complete---,rsp=0x%X,buf_cnt=%d\r\n", response_r5, buf_cnt);
        buf_cnt--;
    } else {
        eshell_putstring("******sdio device rx buf cnt:%d\r\n", buf_cnt);
#ifndef ENABLE_WIFI_TXRX_TEST
        SDIO_HOST_DELAY_MS(100);
        goto wait_buf_cnt;
#endif
    }

    if (real_write_len >= device_rx_buf_len_max) {
        real_write_len = 0;
    }

}
static void unitest_sdio_usage(void)
{
    eshell_putstring("Usage: \r\n");
    eshell_putstring("sdio need device to transfer data,BES2005_FC_BGA_EVB_V3.1\r\n");
    eshell_putstring("utest_sdio speed(24/48) dma_enable(0/1).\r\n");
}
static void utest_sdio(int argc, char *argv[])
{
    if (argc != 3)
        goto usage;

    uint8_t dma_enable;
    uint32_t speed_hz;
    speed_hz = atoi(argv[1]);
    dma_enable = atoi(argv[2]);

    if (dma_enable != 0 && dma_enable != 1) {
        goto usage;
    }
    if (speed_hz != 24 && speed_hz != 48) {
        goto usage;
    } else {
        speed_hz *= 1000 * 1000;
    }
    sdio_host_test(dma_enable, speed_hz);


usage:
    unitest_sdio_usage();

    return;
}
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_UNIT_TEST, "utest_sdio", "usage: utest_sdio help", utest_sdio);
#endif
#endif
