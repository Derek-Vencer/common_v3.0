
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
#if defined(UTILS_ESHELL_EN)

#include "plat_addr_map.h"
#if defined(SENSOR_ENG0_BASE)

#include "cmsis.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "eshell.h"
#include "hal_i2c.h"
#include "hal_spi.h"
#include "cmsis_os.h"
#include "hal_trace.h"
#include "hal_iomux.h"
#include "hal_sensor_eng_v2.h"

#define  I2C_CLK_FREQ                        (1 * 100 * 1000)
#define  SPI_CLK_FREQ                        (6 * 1000 * 1000)
#define  SENS_ENG_TX_BUF_SIZE                10
#define  SENS_ENG_RX_BUF_SIZE                10
#define  SENSOR_TRIGGER_TIMER_MS             1000
#define  SENSOR0_TRIGGER_GPIO                HAL_GPIO_PIN_P0_0

static uint32_t sens_eng_w_data;
static uint32_t i2c_slave;
static uint8_t sens_eng_r_buf[HAL_SENSOR_ENGINE_ID_QTY][SENS_ENG_RX_BUF_SIZE];

static struct HAL_SENSOR_ENGINE_CFG_T sensor_cfg[HAL_SENSOR_ENGINE_MASTER_QTY][HAL_SENSOR_ENGINE_ID_QTY];

static void sensor_test_handler(enum HAL_SENSOR_ENGINE_MASTER_T master, enum HAL_SENSOR_ENGINE_ID_T id, enum HAL_SENSOR_ENGINE_DEVICE_T device, const uint8_t *buf, uint32_t len,
                                enum HAL_SENSOR_ENGINE_READ_WRITE_T read_write)
{
    if (read_write == HAL_SENSOR_ENGINE_WRITE) {
        eshell_putstring("%s: master=%d sensor enging id=%d device=%d ***write complete***", __FUNCTION__, master, id, device);
    } else {
        eshell_putstring("%s: master=%d sensor enging id=%d device=%d\r\n", __FUNCTION__, master, id, device);
        eshell_putstring("array pointer at %p  len=%u data=\r\n", buf, len);

        uint8_t *addr;
        addr = sensor_cfg[0][id].rx_buffer_addr + sensor_cfg[0][id].rx_buffer_size;

        if (buf + len > addr) {
            DUMP8("%02x ", buf, addr - buf);
            DUMP8("%02x ", sensor_cfg[0][id].rx_buffer_addr, len - (addr - buf));
        } else {
            DUMP8("%02x ", buf, len);
        }
    }
}

static int sensor_eng_cfg(enum HAL_SENSOR_ENGINE_MASTER_T master, enum HAL_SENSOR_ENGINE_TRIGGER_T trigger_type, enum HAL_SENSOR_ENGINE_DEVICE_T device, uint8_t send_data_addr, uint8_t i2c_slave_addr)
{
    int ret, i;

    if (device == 7) {   //spi device
        eshell_putstring("-----sensor engine spi test-----\r\n");
        struct HAL_SPI_CFG_T cfg_spi;

        memset(&cfg_spi, 0, sizeof(cfg_spi));
        cfg_spi.clk_delay_half = true;
        cfg_spi.skip_en = true;
        cfg_spi.slave = false;
        cfg_spi.dma_rx = true;
        cfg_spi.dma_tx = true;
        cfg_spi.rx_sep_line = false;
        cfg_spi.cs = 0;
        cfg_spi.rate = SPI_CLK_FREQ;
        cfg_spi.rx_bits = 8;
        cfg_spi.tx_bits = 8;
        cfg_spi.rx_frame_bits = 0;
        hal_iomux_set_spi();
        ret = hal_spi_open(&cfg_spi);
        if (ret) {
            eshell_putstring("%d,%s: spi open failed:%d", __LINE__, __FUNCTION__, ret);
            return ret;
        }
        i = 1;
        memset(&sensor_cfg[master][i], 0, sizeof(sensor_cfg[master][i]));
        sensor_cfg[master][i].id = i;
        sensor_cfg[master][i].device = HAL_SENSOR_ENGINE_DEVICE_SPI;
        sensor_cfg[master][i].trigger_type = trigger_type;
        if (trigger_type == HAL_SENSOR_ENGINE_TRIGGER_TIMER) {
            sensor_cfg[master][i].timer_period_ms = SENSOR_TRIGGER_TIMER_MS;
            sensor_cfg[master][i].handler = sensor_test_handler;
        } else {
            sensor_cfg[master][i].trigger_gpio = SENSOR0_TRIGGER_GPIO;
            sensor_cfg[master][i].handler = sensor_test_handler;
        }
        sensor_cfg[master][i].interrupt_count = 1;
        sensor_cfg[master][i].spi_device_cs = 0;
        sensor_cfg[master][i].spi_device_rxds = 1;
        sensor_cfg[master][i].spi_id = 1;
        sensor_cfg[master][i].read_write = HAL_SENSOR_ENGINE_READ;
        sensor_cfg[master][i].tx_data_width = HAL_SENSOR_ENGINE_DATA_HALF_WORD;
        sensor_cfg[master][i].rx_data_width = HAL_SENSOR_ENGINE_DATA_BYTE;
        sensor_cfg[master][i].tx_buffer_addr = &send_data_addr;
        sensor_cfg[master][i].rx_buffer_addr = sens_eng_r_buf[i];
        sensor_cfg[master][i].tx_buffer_size = 1;
        sensor_cfg[master][i].rx_buffer_size = SENS_ENG_RX_BUF_SIZE;
        sensor_cfg[master][i].tx_transfer_size = 1;
        sensor_cfg[master][i].rx_transfer_size = 1;

        hal_sensor_engine_cfg(master, &sensor_cfg[master][1]);
    } else {    //i2c device
        eshell_putstring("-----sensor engine i2c test-----\r\n");

        struct HAL_I2C_CONFIG_T cfg_i2c;
        enum HAL_I2C_ID_T i2c_id;

        for (i = 0; i < HAL_I2C_ID_NUM; i++) {
            memset(&cfg_i2c, 0, sizeof(cfg_i2c));
            i2c_id = (enum HAL_I2C_ID_T)i;
            cfg_i2c.mode = HAL_I2C_API_MODE_SENSOR_ENGINE;
            cfg_i2c.use_dma = 1;
            cfg_i2c.as_master = 1;
            cfg_i2c.speed = I2C_CLK_FREQ;
            ret = hal_i2c_open(i2c_id, &cfg_i2c);
            if (ret) {
                eshell_putstring("%d,%s: i2c open failed:%d", __LINE__, __FUNCTION__, ret);
                return ret;
            }
        }

        i = 0;
        memset(&sensor_cfg[master][i], 0, sizeof(sensor_cfg[master][i]));
        sensor_cfg[master][i].id = i;
        sensor_cfg[master][i].device = device;
        sensor_cfg[master][i].trigger_type = trigger_type;
        if (trigger_type == HAL_SENSOR_ENGINE_TRIGGER_TIMER) {
            sensor_cfg[master][i].timer_period_ms = SENSOR_TRIGGER_TIMER_MS;
            sensor_cfg[master][i].handler = sensor_test_handler;
        } else {
            sensor_cfg[master][i].trigger_gpio = SENSOR0_TRIGGER_GPIO;
            sensor_cfg[master][i].handler = sensor_test_handler;
        }
        sensor_cfg[master][i].interrupt_count = 1;
        sensor_cfg[master][i].i2c_device_address = i2c_slave_addr;
        sensor_cfg[master][i].read_write = HAL_SENSOR_ENGINE_READ;
        sensor_cfg[master][i].tx_data_width = HAL_SENSOR_ENGINE_DATA_HALF_WORD;
        sensor_cfg[master][i].rx_data_width = HAL_SENSOR_ENGINE_DATA_BYTE;
        sensor_cfg[master][i].tx_buffer_addr = &send_data_addr;
        sensor_cfg[master][i].rx_buffer_addr = sens_eng_r_buf[i];
        sensor_cfg[master][i].tx_buffer_size = 1;
        sensor_cfg[master][i].rx_buffer_size = SENS_ENG_RX_BUF_SIZE;
        sensor_cfg[master][i].tx_transfer_size = 1;
        sensor_cfg[master][i].rx_transfer_size = 1;

        hal_sensor_engine_cfg(master, &sensor_cfg[master][0]);

    }
    hal_sensor_engine_open(master);
    eshell_putstring("----sensor engine cfg and opening has been completed----\r\n");
    eshell_putstring("---Please wait for sensor engine's completion interrupt---\r\n");

    return 0;
}


static void unitest_sensor_engine(int argc, char *argv[])
{
    if (argc != 6) {
        eshell_putstring("ERROR_ARG\r\n");
        eshell_putstring("   utest_sens_eng sensor_id triger(timer/gpio) device_id  tx_data i2c_slave_addr\r\n");
        eshell_putstring("[0~7]master id [0/1] gpio/time trriger [0~7] i2c/spi device\r\n");
        return;
    }

    enum HAL_SENSOR_ENGINE_ID_T sensor_id = atoi(argv[1]);
    if ((sensor_id < 0) || (sensor_id >= HAL_SENSOR_ENGINE_ID_QTY)) {
        eshell_putstring("sensor_id err, valid value:0~%d", (HAL_SENSOR_ENGINE_ID_QTY - 1));
        return;
    }

    enum HAL_SENSOR_ENGINE_TRIGGER_T triger_type = atoi(argv[2]);
    if ((triger_type < 0) || (triger_type >= HAL_SENSOR_ENGINE_TRIGGER_QTY)) {
        eshell_putstring("triger_type err, valid value:0~%d", (HAL_SENSOR_ENGINE_TRIGGER_QTY - 1));
        return;
    }

    enum HAL_SENSOR_ENGINE_DEVICE_T devide_id = atoi(argv[3]);
    if ((devide_id < 0) || (devide_id > HAL_SENSOR_ENGINE_DEVICE_I2C4)) {
        if (devide_id != HAL_SENSOR_ENGINE_DEVICE_SPI) {
            eshell_putstring("device id err, valid value:0~%d or %d\r\n", HAL_SENSOR_ENGINE_DEVICE_I2C4, HAL_SENSOR_ENGINE_DEVICE_SPI);
            eshell_putstring("0~4 represent i2c0~i2c4  7 represent spi");
            return;
        }
    }

    sscanf(argv[4], "%x", &sens_eng_w_data);
    sscanf(argv[5], "%x", &i2c_slave);

    sensor_eng_cfg(sensor_id, triger_type, devide_id, sens_eng_w_data, i2c_slave);
}

ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_UNIT_TEST, "utest_sens_eng", "utest_sens_eng sens_eng_id trigger_type device_id",
                   unitest_sensor_engine);
#endif //defined(SENSOR_ENG0_BASE)
#endif //defined(UTILS_ESHELL_EN)

