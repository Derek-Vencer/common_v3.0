/***************************************************************************
 *
 * Copyright 2015-2023 BES.
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
#include "cmsis.h"
#include "cmsis_os.h"
#include "eshell.h"
#include "stdlib.h"
#include "string.h"
#include "hal_timer.h"
#include "hal_pwm.h"
#include "hal_cmu.h"
#include "hal_iomux.h"
#include "app_utils.h"
#include "hal_sysfreq.h"

#ifdef UTILS_ESHELL_EN
static uint8_t sys_freq_map = 0;
static void test_pwm_iomux(enum HAL_PWM_ID_T pwm_id)
{
    switch (pwm_id) {
        case HAL_PWM_ID_0:
            hal_iomux_set_pwm0();
            break;
        case HAL_PWM_ID_1:
            hal_iomux_set_pwm1();
            break;
        case HAL_PWM_ID_2:
            hal_iomux_set_pwm2();
            break;
        case HAL_PWM_ID_3:
            hal_iomux_set_pwm3();
            break;
#ifdef PWM1_BASE
        case HAL_PWM1_ID_0:
            hal_iomux_set_pwm4();
            break;
        case HAL_PWM1_ID_1:
            hal_iomux_set_pwm5();
            break;
        case HAL_PWM1_ID_2:
            hal_iomux_set_pwm6();
            break;
        case HAL_PWM1_ID_3:
            hal_iomux_set_pwm7();
            break;
#endif
        default:
            break;
    }
}

static void test_pwm_enable(enum HAL_PWM_ID_T pwm_id, uint32_t pwm_freq, uint8_t pwm_ratio)
{
    app_sysfreq_req(APP_SYSFREQ_USER_ESHELL, APP_SYSFREQ_52M);
    sys_freq_map |= 1 << pwm_id;
    test_pwm_iomux(pwm_id);

    struct HAL_PWM_CFG_T pwm_cfg = {
        .freq = pwm_freq,
        .ratio = pwm_ratio,
        .inv = false,
        .sleep_on = false,
    };

    hal_pwm_enable(pwm_id, &pwm_cfg);
    eshell_putstring("pwm_test start, pwm_id:%d freq:%d, ratio:%d\r\n", pwm_id, pwm_freq, pwm_ratio);
}

static void test_pwm_disable(enum HAL_PWM_ID_T pwm_id)
{
    hal_pwm_disable(pwm_id);
    sys_freq_map &= ~(1 << pwm_id);
    if (sys_freq_map == 0) {
        app_sysfreq_req(APP_SYSFREQ_USER_ESHELL, APP_SYSFREQ_32K);
    }
    eshell_putstring("pwm_id:%d disable\r\n", pwm_id);
}

static void unitest_pwm(int argc, char *argv[])
{
    if (argc < 3) {
        goto exit;
    }

    enum HAL_PWM_ID_T pwm_id = (enum HAL_PWM_ID_T)atoi(argv[2]);
    if ((pwm_id < 0) || (pwm_id >= HAL_PWM_ID_QTY)) {
        eshell_putstring("pwm_id err, id range:0--%d\r\n", (HAL_PWM_ID_QTY - 1));
        goto exit;
    }

    if (strncmp(argv[1], "enable", 6) == 0) {
        uint32_t pwm_freq = atoi(argv[3]);
        uint8_t pwm_ratio = atoi(argv[4]);

        if ((pwm_ratio < 0) || (pwm_ratio > 100)) {
            eshell_putstring("pwm_ratio err, ratio_range:0--100\r\n");
            return;
        }

        test_pwm_enable(pwm_id, pwm_freq, pwm_ratio);
        return;
    } else if (strncmp(argv[1], "disable", 7) == 0) {
        test_pwm_disable(pwm_id);
        return;
    }

exit:
    eshell_putstring("ERROR_ARG\r\n");
    eshell_putstring("    utest_pwm enable pwm_id freq ratio\r\n");
    eshell_putstring("    utest_pwm disable pwm_id\r\n");
}

ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_UNIT_TEST, "utest_pwm", "utest_pwm pwm_id freq ratio",
                   unitest_pwm);
#endif

