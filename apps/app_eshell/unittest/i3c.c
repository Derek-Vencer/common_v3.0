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
#include "plat_addr_map.h"

#if defined(UTILS_ESHELL_EN) && defined(I3C0_BASE) && defined(I3C_TEST_ENABLE)

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "eshell.h"
#include "hal_dma.h"
#include "hal_i3c.h"
#include "hal_timer.h"

static void test_i3c_callback(uint32_t hci_interrupt_status, uint32_t mode_interrupt_status)
{
    if (hci_interrupt_status & (1 << 10)) {
        eshell_putstring("INTR_HC_INTRNL_ERR_STS trigger\r\n");
    }
    if (mode_interrupt_status & (1 << 5)) {
        eshell_putstring("INTR_TX_ABORT_STS trigger\r\n");
    }
    if (mode_interrupt_status & (1 << 9)) {
        eshell_putstring("INTR_TX_ERR_STS trigger\r\n");
    }
}

static void test_cmd_usage(void)
{
    eshell_putstring("ERROR_ARG\r\n");
    eshell_putstring("config format: utest_i3c config i3c_id data_mode scl_speed xfer_mode dev_cnt\r\n");
    eshell_putstring("write format: utest_i3c write i3c_id reg_addr reg_len write_val1 write_val2 write_val3...\r\n");
    eshell_putstring("read format : utest_i3c read  i3c_id reg_addr reg_len data_len\r\n");
}

static void utest_i3c_config(enum HAL_I3C_ID_T i3c_id, enum HAL_I3C_MODE_T data_mode, uint32_t speed, uint8_t use_dma, uint8_t dev_cnt)
{
    enum HAL_I3C_ERR err;
    struct HAL_I3C_CONFIG_T i3c_cfg;

    memset(&i3c_cfg, 0, sizeof(struct HAL_I3C_CONFIG_T));
    i3c_cfg.mode = data_mode;
    i3c_cfg.speed = speed;
    i3c_cfg.dev_cnt = dev_cnt;
    i3c_cfg.use_dma = use_dma;
    i3c_cfg.i2c_dev_present = 0;
    i3c_cfg.inc_broadcast_addr = 0;
    i3c_cfg.static_addr = 0;

    hal_i3c_close(i3c_id);

    err = hal_i3c_open(i3c_id, &i3c_cfg);
    if (err) {
        eshell_putstring("hal_i3c_open err:%d\r\n", err);
        return;
    }

    hal_i3c_irq_set_callback(i3c_id, test_i3c_callback);

    err = hal_i3c_bus_enumeration(i3c_id, HAL_I3C_ENUM_CMD_DAA);
    if (err < 0) {
        eshell_putstring("i3c bus enumeration err:%d\r\n", err);
    } else {
        eshell_putstring("i3c bus enumeration success, dev cnt:%d\r\n", err);
    }
}

static void utest_i3c_read(enum HAL_I3C_ID_T i3c_id, uint8_t reg_addr, uint8_t reg_len, uint8_t data_len)
{
    uint8_t ret;
    uint8_t *pst_read_buffer = (uint8_t *)malloc(data_len + reg_len);

    if (pst_read_buffer == NULL) {
        eshell_putstring("read buffer malloc failed\r\n");
        return;
    }

    pst_read_buffer[0] = (uint8_t)reg_addr;
    eshell_putstring("reg_addr = 0x%x, reg_len = 0x%x, data_len = 0x%x\r\n", pst_read_buffer[0], reg_len, data_len);

    ret = hal_i3c_recv(i3c_id, 0, pst_read_buffer, reg_len, data_len);
    if (ret != 0) {
        eshell_putstring("utest_i3c_read failed, ret = %d\r\n", ret);
    }

    for (int j = 0; j < data_len; j++) {
        eshell_putstring("pst_read_buffer[%d] = 0x%x\r\n", j, pst_read_buffer[j]);
    }
    free(pst_read_buffer);
}

static void utest_i3c_write(enum HAL_I3C_ID_T i3c_id, uint8_t reg_addr, uint8_t reg_len, uint8_t data_len, char *write_val)
{
    uint8_t ret;
    uint8_t write_buffer[reg_len + data_len];
    write_buffer[0] = (uint8_t)reg_addr;
    eshell_putstring("reg_addr = 0x%x\r\n", write_buffer[0]);

    uint32_t value;
    for (int i = 0; i < data_len; i++) {
        sscanf(write_val + i, "0x%x", &value);
        write_buffer[i + 1] = (uint8_t)value;
        eshell_putstring("write value[%d] = 0x%x\r\n", i, write_buffer[i + 1]);
    }

    ret = hal_i3c_send(i3c_id, 0, write_buffer, reg_len, data_len);
    if (ret != 0) {
        eshell_putstring("utest_i3c_write failed, ret = %d\r\n", ret);
    }
}

static void unittest_i3c(int argc, char *argv[])
{
    if (argc < 6) {
        test_cmd_usage();
        return;
    }

    enum HAL_I3C_ID_T i3c_id = atoi(argv[2]);
    if (i3c_id < 0 || i3c_id > HAL_I3C_ID_NUM) {
        eshell_putstring("i3c_id err, valid value: 0 ~ %d\r\n", (HAL_I3C_ID_NUM - 1));
        return;
    }

    if (strncmp(argv[1], "config", 6) == 0) {
        enum HAL_I3C_MODE_T data_mode = atoi(argv[3]);
        if (data_mode < 0 || data_mode > HAL_I3C_MODE_SDR4) {
            eshell_putstring("i3c_data_mode err, valid mode: SDR0-SDR4(0-4)\r\n");
        }

        uint32_t speed = atoi(argv[4]);
        if (speed < 0 || speed > 12000000) {
            eshell_putstring("i3c_speed err, only two valid value: 6MHz/12MHz\r\n");
            return;
        }

        uint8_t use_dma = atoi(argv[5]);
        if (use_dma < 0 || use_dma > 1) {
            eshell_putstring("i3c_use_dma err, valid value: 0/1 (0:PIO, 1:DMA)\r\n");
            return;
        }

        uint8_t dev_cnt = atoi(argv[6]);
        if (dev_cnt <= 0 || dev_cnt > 8) {
            eshell_putstring("dev_cnt err, valid value: 1-8\r\n");
            return;
        }

        utest_i3c_config(i3c_id, data_mode, speed, use_dma, dev_cnt);

    } else if (strncmp(argv[1], "read", 4) == 0) {
        uint32_t reg_addr;
        sscanf(argv[3], "%x", &reg_addr);
        uint16_t reg_len = atoi(argv[4]);
        uint16_t data_len = atoi(argv[5]);

        utest_i3c_read(i3c_id, (uint8_t)reg_addr, reg_len, data_len);

    } else if (strncmp(argv[1], "write", 5) == 0) {
        uint32_t reg_addr;
        sscanf(argv[3], "0x%x", &reg_addr);
        uint16_t reg_len = atoi(argv[4]);
        uint16_t data_len = argc - 5;
        eshell_putstring("write data_len = 0x%x\r\n", data_len);

        char *temp_write_buffer = argv[5];
        utest_i3c_write(i3c_id, reg_addr, reg_len, data_len, temp_write_buffer);

    } else {
        test_cmd_usage();
    }
}

ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_UNIT_TEST, "utest_i3c", "usage: utest_i3c help", unittest_i3c);
#endif
