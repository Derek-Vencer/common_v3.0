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
#ifdef UTILS_ESHELL_EN
#if defined(__SYS_AS_MAIN__)

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "cmsis_os.h"
#include "math.h"
#include "hal_iomux.h"
#include "hal_i2c.h"
#include "hal_trace.h"
#include "eshell.h"

#define I2C_TEST_SIZE      8
#define TEST_I2C_SLAVE_ADDR     0x66

static uint8_t master_tx_buff[I2C_TEST_SIZE];
static uint8_t slave_rx_buff[I2C_TEST_SIZE];

static osSemaphoreId_t i2c_sem;

struct I2C_LOOPBACK_ARGS {
    enum HAL_I2C_ID_T xfer_id;
    uint32_t loopback_i2c_speed;
    uint32_t loopback_counts;
};

static void test_cmd_usage(void)
{
    eshell_putstring("Usage\r\n");
    eshell_putstring("utest_i2c read i2c_id mode(task:0/simple:1) dev_addr(hex) reg_addr(hex) reg_len rx_len\r\n");
    eshell_putstring("utest_i2c write i2c_id mode(task:0/simple:1) dev_addr(hex) reg_addr(hex) reg_len txdata0(hex) txdata1(hex) txdata2(hex) ... \r\n");
    eshell_putstring("utest_i2c tx tx_id i2c_speed\r\n");
    eshell_putstring("utest_i2c rx rx_id i2c_speed\r\n");
    eshell_putstring("utest_i2c loopback i2c_tx_id i2c_rx_id i2c_speed xfer_count\r\n");
}

static uint32_t test_i2c_open(enum HAL_I2C_ID_T id, enum HAL_I2C_API_MODE_T mode)
{
    struct HAL_I2C_CONFIG_T i2c_cfg;
    memset(&i2c_cfg, 0, sizeof(i2c_cfg));

    i2c_cfg.speed = 400000;
    i2c_cfg.as_master = 1;
    i2c_cfg.addr_as_slave = 0;
    i2c_cfg.rising_time_ns = 0;

    if (mode == HAL_I2C_API_MODE_SIMPLE) {
        i2c_cfg.mode = HAL_I2C_API_MODE_SIMPLE;
        i2c_cfg.use_dma = 0;
        i2c_cfg.use_sync = 0;
    } else if (mode == HAL_I2C_API_MODE_TASK) {
        i2c_cfg.mode = HAL_I2C_API_MODE_TASK;
        i2c_cfg.use_dma = 1;
        i2c_cfg.use_sync = 1;
    }

    return hal_i2c_open(id, &i2c_cfg);
}

static void test_i2c_read(enum HAL_I2C_ID_T id, enum HAL_I2C_API_MODE_T mode, uint8_t dev_addr, uint16_t reg_buf, uint16_t reg_len, uint16_t rx_len)
{
    uint32_t ret = 0;
    uint8_t *pdata = NULL;
    pdata = (uint8_t *)malloc(rx_len);
    if (!pdata) {
        return;
    }

    ret = test_i2c_open(id, mode);
    if (ret != 0) {
        free(pdata);
        eshell_putstring("i2c_open fail ret=0x%x\r\n", ret);
        return;
    }

    if (mode == HAL_I2C_API_MODE_SIMPLE) {
        ret = hal_i2c_simple_recv(id, dev_addr, (uint8_t *)&reg_buf, reg_len, pdata, rx_len);
    } else if (mode == HAL_I2C_API_MODE_TASK) {
        ret = hal_i2c_task_recv(id, dev_addr, (uint8_t *)&reg_buf, reg_len, pdata, rx_len, 0, NULL);
    }

    if (!ret) {
        for (int i = 0; i < rx_len; i++) {
            eshell_putstring("data[%d] = 0x%x\r\n", i, pdata[i]);
        }
    } else {
        eshell_putstring("i2c recv failed id:%d ret=%08x\r\n", id, ret);
    }

    free(pdata);
    hal_i2c_close(id);
}

static void test_i2c_write(enum HAL_I2C_ID_T id, enum HAL_I2C_API_MODE_T mode, uint8_t dev_addr, uint16_t reg_addr, uint8_t *pdata, uint16_t reg_len, uint16_t tx_len)
{
    uint32_t ret = 0;
    uint8_t *i2c_x_tx_buf = NULL;
    i2c_x_tx_buf = (uint8_t *)malloc(reg_len + tx_len);
    if (!i2c_x_tx_buf) {
        return;
    }

    ret = test_i2c_open(id, mode);
    if (ret != 0) {
        free(i2c_x_tx_buf);
        eshell_putstring("i2c_open fail ret=0x%x\r\n", ret);
        return;
    }

    if (reg_len) {
        memcpy(&i2c_x_tx_buf[0], &reg_addr, reg_len);
    }
    if (tx_len) {
        memcpy(&i2c_x_tx_buf[reg_len], pdata, tx_len);
    }

    if (mode == HAL_I2C_API_MODE_SIMPLE) {
        ret = hal_i2c_simple_send(id, dev_addr, i2c_x_tx_buf, reg_len + tx_len);
    } else if (mode == HAL_I2C_API_MODE_TASK) {
        ret = hal_i2c_send(id, dev_addr, i2c_x_tx_buf, reg_len, tx_len, 0, NULL);
    }

    if (ret != 0) {
        eshell_putstring("i2c simple send failed id:%d ret=%08x\r\n", id, ret);
    }

    free(i2c_x_tx_buf);
    hal_i2c_close(id);
}

static void test_i2c_master_tx_loop(enum HAL_I2C_ID_T id, uint32_t speed)
{
    eshell_putstring("%s\r\n", __func__);
    int ret;
    struct HAL_I2C_CONFIG_T i2c_cfg;
    memset(&i2c_cfg, 0, sizeof(i2c_cfg));
    i2c_cfg.as_master = 1; //1 - master, 0 - slave
    i2c_cfg.mode = HAL_I2C_API_MODE_SIMPLE;
    i2c_cfg.speed = speed;
    ret = hal_i2c_open(id, &i2c_cfg);
    if (ret != 0) {
        eshell_putstring("master open i2c fail");
        return;
    }

    for (uint32_t i = 0; i < sizeof(master_tx_buff); i++) {
        master_tx_buff[i] = rand() & 0xff;
    }

    while (1) {
        ret = hal_i2c_simple_send(id, TEST_I2C_SLAVE_ADDR, master_tx_buff, sizeof(master_tx_buff));
        eshell_putstring("i2c master write data: %d\r\n", ret);
        osDelay(50);
    }
}

static void test_i2c_slave_rx_loop(enum HAL_I2C_ID_T id, uint32_t speed)
{
    eshell_putstring("%s\r\n", __func__);
    int ret;
    struct HAL_I2C_CONFIG_T i2c_cfg;
    memset(&i2c_cfg, 0, sizeof(i2c_cfg));

    i2c_cfg.mode = HAL_I2C_API_MODE_SIMPLE;

    i2c_cfg.use_dma = 1;
    i2c_cfg.use_sync = 0;
    i2c_cfg.speed = speed;
    i2c_cfg.as_master = 0;
    i2c_cfg.addr_as_slave = TEST_I2C_SLAVE_ADDR;
    i2c_cfg.rising_time_ns = 0;

    ret = hal_i2c_open(id, &i2c_cfg);
    if (ret != 0) {
        eshell_putstring("slave open i2c fail");
        return;
    }
    while (1) {
        ret = hal_i2c_slv_simple_recv(id, slave_rx_buff, sizeof(slave_rx_buff), 0);
        eshell_putstring("i2c slave receive data: %d\r\n", ret);
        //osDelay(1000);
    }
}

static void i2c_loopback_rx_thread(void *i2c_rx_args)
{
    eshell_putstring("%s\r\n", __func__);
    int ret;
    struct I2C_LOOPBACK_ARGS rx_args = *(struct I2C_LOOPBACK_ARGS *)i2c_rx_args;
    struct HAL_I2C_CONFIG_T slave_cfg;

    memset(&slave_cfg, 0, sizeof(slave_cfg));
    slave_cfg.mode = HAL_I2C_API_MODE_SIMPLE;
    slave_cfg.use_dma = 1;
    slave_cfg.use_sync = 0;
    slave_cfg.speed = rx_args.loopback_i2c_speed;
    slave_cfg.as_master = 0;
    slave_cfg.addr_as_slave = TEST_I2C_SLAVE_ADDR;
    slave_cfg.rising_time_ns = 0;

    ret = hal_i2c_open(rx_args.xfer_id, &slave_cfg);
    if (ret) {
        eshell_putstring("slave open i2c fail\r\n");
        return;
    }

    for (uint32_t i = 0; i < rx_args.loopback_counts; i++) {
        eshell_putstring("Rx start\r\n");
        ret = hal_i2c_slv_simple_recv(rx_args.xfer_id, slave_rx_buff, sizeof(slave_rx_buff), 1);
        if (ret) {
            eshell_putstring("i2c_loopback_rx_thread failed:%d\r\n", ret);
            osThreadExit();
            return;
        }
        ret = memcmp(slave_rx_buff, master_tx_buff, I2C_TEST_SIZE);
        if (ret) {
            eshell_putstring("loopback xfer mismatch:%d\r\n", ret);
            return;
        }
        eshell_putstring("loopback xfer success\r\n");
        osSemaphoreRelease(i2c_sem);
    }
    hal_i2c_close(rx_args.xfer_id);
    osThreadExit();
}

static void i2c_loopback_tx_thread(void *i2c_tx_args)
{
    eshell_putstring("%s\r\n", __func__);
    int ret;
    struct I2C_LOOPBACK_ARGS tx_args = *(struct I2C_LOOPBACK_ARGS *)i2c_tx_args;

    struct HAL_I2C_CONFIG_T master_cfg;
    memset(&master_cfg, 0, sizeof(master_cfg));
    master_cfg.as_master = 1; //1 - master, 0 - slave
    master_cfg.mode = HAL_I2C_API_MODE_SIMPLE;
    master_cfg.speed = tx_args.loopback_i2c_speed;

    ret = hal_i2c_open(tx_args.xfer_id, &master_cfg);
    if (ret) {
        eshell_putstring("master open i2c failed\r\n");
        return;
    }

    for (uint32_t i = 0; i < tx_args.loopback_counts; i++) {
        osSemaphoreAcquire(i2c_sem, osWaitForever);
        eshell_putstring("Tx start\r\n");
        for (uint32_t j = 0; j < sizeof(master_tx_buff); j++) {
            master_tx_buff[j] = rand() & 0xff;
        }
        ret = hal_i2c_simple_send(tx_args.xfer_id, TEST_I2C_SLAVE_ADDR, master_tx_buff, sizeof(master_tx_buff));
        if (ret) {
            eshell_putstring("i2c_loopback_tx_thread failed:%d\r\n", ret);
            osThreadExit();
            return;
        }
    }
    hal_i2c_close(tx_args.xfer_id);
    osThreadExit();
}

static void test_i2c_loopback(enum HAL_I2C_ID_T tx_id, enum HAL_I2C_ID_T rx_id, uint32_t loopback_speed, uint32_t counts)
{
    osThreadAttr_t rx_thread_attr = {
        .name = "loopback_slave",
        .attr_bits = osThreadDetached,
        .stack_size = 1024,
        .priority = osPriorityAboveNormal,
    };

    osThreadAttr_t tx_thread_attr = {
        .name = "loopback_master",
        .attr_bits = osThreadDetached,
        .stack_size = 1024,
        .priority = osPriorityAboveNormal,
    };

    struct I2C_LOOPBACK_ARGS rx_args = {
        .xfer_id = rx_id,
        .loopback_i2c_speed = loopback_speed,
        .loopback_counts = counts,
    };

    struct I2C_LOOPBACK_ARGS tx_args = {
        .xfer_id = tx_id,
        .loopback_i2c_speed = loopback_speed,
        .loopback_counts = counts,
    };

    i2c_sem = osSemaphoreNew(1U, 1U, NULL);
    if (i2c_sem == NULL) {
        eshell_putstring("i2c_sem creat failure\r\n");
        return;
    }

    osThreadNew(i2c_loopback_rx_thread, &rx_args, &rx_thread_attr);
    osThreadNew(i2c_loopback_tx_thread, &tx_args, &tx_thread_attr);
}

static void unitest_i2c(int argc, char *argv[])
{
    if (argc < 4) {
        test_cmd_usage();
        return;
    }

    if (strncmp(argv[1], "tx", 2) == 0) {
        enum HAL_I2C_ID_T i2c_id = atoi(argv[2]);
        uint32_t i2c_speed = atoi(argv[3]);
        if ((i2c_id < 0) || (i2c_id >= HAL_I2C_ID_NUM)) {
            eshell_putstring("i2c_id err, valid value: 0--%d\r\n", (HAL_I2C_ID_NUM - 1));
            return;
        }
        test_i2c_master_tx_loop(i2c_id, i2c_speed);
    }else if (strncmp(argv[1], "rx", 2) == 0) {
        enum HAL_I2C_ID_T i2c_id = atoi(argv[2]);
        uint32_t i2c_speed = atoi(argv[3]);
        if ((i2c_id < 0) || (i2c_id >= HAL_I2C_ID_NUM)) {
            eshell_putstring("i2c_id err, valid value: 0--%d\r\n", (HAL_I2C_ID_NUM - 1));
            return;
        }
        test_i2c_slave_rx_loop(i2c_id, i2c_speed);
    }else if (strncmp(argv[1], "loopback", 8) == 0) {
        if (argc < 6){
            test_cmd_usage();
            return;
        }

        enum HAL_I2C_ID_T tx_id;
        enum HAL_I2C_ID_T rx_id;
        uint32_t loopback_i2c_speed;
        uint32_t loopback_counts;

        tx_id = atoi(argv[2]);
        if ((tx_id < 0) || (tx_id >= HAL_I2C_ID_NUM)) {
            eshell_putstring("tx_args.i2c_id err, valid value: 0--%d\r\n", (HAL_I2C_ID_NUM - 1));
            eshell_putstring("utest_i2c loopback i2c_tx_id i2c_rx_id i2c_speed xfer_count");
            return;
        }

        rx_id = atoi(argv[3]);
        if ((rx_id < 0) || (rx_id >= HAL_I2C_ID_NUM)) {
            eshell_putstring("rx_args.i2c_id err, valid value: 0--%d\r\n", (HAL_I2C_ID_NUM - 1));
            eshell_putstring("utest_i2c loopback i2c_tx_id i2c_rx_id i2c_speed xfer_count");
            return;
        }

        loopback_i2c_speed = atoi(argv[4]);
        loopback_counts = atoi(argv[5]);

        test_i2c_loopback(tx_id, rx_id, loopback_i2c_speed, loopback_counts);
        return;
    }

    if (argc < 7) {
        test_cmd_usage();
        return;
    }

    /**** i2c_id valid value: 0--(HAL_I2C_ID_NUM - 1) *****/
    enum HAL_I2C_ID_T i2c_id = atoi(argv[2]);
    if ((i2c_id < 0) || (i2c_id >= HAL_I2C_ID_NUM)) {
        eshell_putstring("i2c_id err, valid value: 0--%d\r\n", (HAL_I2C_ID_NUM - 1));
        return;
    }

    /**** i2c_mode valid value: 0 or 1, 0:task_mode 1:simple_mode ****/
    enum HAL_I2C_API_MODE_T i2c_mode;
    i2c_mode = atoi(argv[3]);
    if ((i2c_mode < 0) || (i2c_mode > 1)) {
        eshell_putstring("i2c_mode err, valid value: 0 or 1, 0:task_mode 1:simple_mode\r\n");
        return;
    }

    /**** dev_addr, reg_addr, reg_len*****/
    unsigned int dev_addr, reg_addr;
    sscanf(argv[4], "%x", &dev_addr);
    sscanf(argv[5], "%x", &reg_addr);
    if (reg_addr > 0xFF) {
        reg_addr = ((reg_addr >> 8) & 0x00FF) | ((reg_addr << 8) & 0xFF00);
    }
    uint16_t reg_len = atoi(argv[6]);
    eshell_putstring("i2c_mode=%d dev_addr=0x%x reg_addr=0x%x reg_len=%d \r\n", i2c_mode, dev_addr, reg_addr, reg_len);

    /**** i2c_cmd valid value: read or write ****/
    if (strncmp(argv[1], "read", 4) == 0) { // read
        uint16_t rx_len = atoi(argv[7]);
        test_i2c_read(i2c_id, i2c_mode, dev_addr, reg_addr, reg_len, rx_len);
    } else if (strncmp(argv[1], "write", 5) == 0) { // write
        uint16_t tx_len = argc - 7;
        uint8_t *pdata = NULL;
        pdata = (uint8_t *)malloc(tx_len);
        if (!pdata) {
            eshell_putstring("memory malloc fail\r\n");
            return;
        }
        unsigned int tx_data;
        for (int i = 0; i < tx_len; i++) {
            sscanf(argv[i + 7], "0x%x", &tx_data);
            pdata[i] = (uint8_t)tx_data;
            eshell_putstring("data[%d]=0x%x\r\n", i, pdata[i]);
        }
        test_i2c_write(i2c_id, i2c_mode, dev_addr, reg_addr, pdata, reg_len, tx_len);
        free(pdata);
    } else {
        test_cmd_usage();
    }

}

ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_UNIT_TEST, "utest_i2c", "usage: utest_i2c help", unitest_i2c);

#endif // __SYS_AS_MAIN__
#endif // UTILS_ESHELL_EN
