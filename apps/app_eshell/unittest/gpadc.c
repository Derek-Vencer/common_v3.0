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
#include "stdlib.h"
#include "string.h"
#include "cmsis.h"
#include "cmsis_os.h"
#include "eshell.h"
#include "hal_cmu.h"
#include "hal_gpadc.h"
#include "hal_gpio.h"
#include "pmu.h"
#include CHIP_SPECIFIC_HDR(pmu)

static void ntc_irq_handler(uint16_t irq_val, HAL_GPADC_MV_T volt)
{
    uint32_t temperature = pmu_volt2temperature(volt);
    eshell_putstring("%s volt=%dmv Current temperatur:%d", __func__, volt, temperature);
}

static void gpadc_irq_handler(uint16_t irq_val, HAL_GPADC_MV_T volt)
{
    eshell_putstring("%s raw_val=%d volt=%dmv", __func__, irq_val, volt);
}

static void test_gpadc_start(uint32_t enable, enum HAL_GPADC_CHAN_T channel, enum HAL_GPADC_ATP_T atp)
{
    int ret = 0;
    if (enable) {
        if(channel == HAL_GPADC_CHAN_0) {
            ret = hal_gpadc_open(channel, atp, ntc_irq_handler);
        } else {
            ret = hal_gpadc_open(channel, atp, gpadc_irq_handler);
        }
        if (ret != 0) {
            eshell_putstring("gpadc ch%d open fail, ret=%d", channel, ret);
        } else {
            eshell_putstring("gpadc ch%d open sucess", channel);
        }
    } else {
        ret = hal_gpadc_close(channel);
        if (ret != 0) {
            eshell_putstring("gpadc ch%d close fail, ret=%d", channel, ret);
        } else {
            eshell_putstring("gpadc ch%d close sucess", channel);
        }
    }
}

static void utest_gpadc(int argc, char* argv[])
{
    if (argc < 3) {
        eshell_putstring("cmd err\r\n");
        eshell_putstring("cmd format: utest_gpadc open/close channel [atp]\r\n");
        return;
    }

    static uint32_t enable = 0;
    static enum HAL_GPADC_CHAN_T channel = HAL_GPADC_CHAN_0;
    static enum HAL_GPADC_ATP_T atp = HAL_GPADC_ATP_NULL;

    if (strncmp(argv[1], "open", 4) == 0) {
        enable = 1;
        atp = HAL_GPADC_ATP_ONESHOT;
    } else if (strncmp(argv[1], "close", 5) == 0) {
        enable = 0;
        atp = HAL_GPADC_ATP_NULL;
    } else {
        eshell_putstring(" gpadc cmd err, valid cmd: utest_gpadc open/close channel [atp]\r\n");
        return;
    }

    channel = (enum HAL_GPADC_CHAN_T)atoi(argv[2]);
    if ((channel < HAL_GPADC_CHAN_0) || (channel >= HAL_GPADC_CHAN_QTY)) {
        eshell_putstring("channel err, valid value: %d ~ %d\r\n", HAL_GPADC_CHAN_0, (HAL_GPADC_CHAN_QTY - 1));
        return;
    }

    if (enable && argc >= 4) {
        atp = (enum HAL_GPADC_ATP_T)atoi(argv[3]);
    }

    test_gpadc_start(enable, channel, atp);
}

ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_UNIT_TEST, "utest_gpadc", "usage: utest_gpadc help", utest_gpadc);
#endif

