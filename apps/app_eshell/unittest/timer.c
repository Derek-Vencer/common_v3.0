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

#include "cmsis.h"
#include "cmsis_os.h"
#include "eshell.h"
#include "hal_timer.h"
#include "string.h"
#include <stdlib.h>
#include "hal_timer_fast_irq.h"
#ifdef TIMER2_BASE
#include "hal_timer_user_irq.h"
#endif
#include "app_utils.h"
#include "hal_sysfreq.h"

#ifdef TIMER2_BASE
#define USER_TIMER_ENABLE   1
#endif

static uint32_t fast_start_time;
static uint32_t periodic_fast_timer_cnt = 0;
#ifdef USER_TIMER_ENABLE
static uint32_t user_start_time;
static uint32_t periodic_user_timer_cnt = 0;
#endif

#define FAST_TIMER_SYSFREQ_USER       HAL_SYSFREQ_USER_APP_1
#ifdef USER_TIMER_ENABLE
#define USER_TIMER_SYSFREQ_USER       HAL_SYSFREQ_USER_APP_2
#endif

static void oneshot_fast_timer_handler(void)
{
    uint32_t elapsed = hal_fast_sys_timer_get() - fast_start_time;
    eshell_putstring("oneshot_timer_handler timer_id:1, elapased:%d\r\n", elapsed);
    hal_fast_timer_stop();
    hal_sysfreq_req(FAST_TIMER_SYSFREQ_USER, HAL_CMU_FREQ_32K);
}

#ifdef USER_TIMER_ENABLE
static void oneshot_user_timer_handler(void)
{
    uint32_t elapsed = hal_fast_sys_timer_get() - user_start_time;
    eshell_putstring("oneshot_timer_handler timer_id:2, elapased:%d\r\n", elapsed);
    hal_user_timer_stop();
    hal_sysfreq_req(USER_TIMER_SYSFREQ_USER, HAL_CMU_FREQ_32K);
}
#endif

static void periodic_fast_timer_handler(void)
{
    uint32_t elapsed = hal_fast_sys_timer_get() - fast_start_time;
    periodic_fast_timer_cnt++;
    eshell_putstring("periodic_timer_handler timer_id:1, elapased:%d, periodic_timer_cnt:%d", elapsed, periodic_fast_timer_cnt);
}

#ifdef USER_TIMER_ENABLE
static void periodic_user_timer_handler(void)
{
    uint32_t elapsed = hal_fast_sys_timer_get() - user_start_time;
    periodic_user_timer_cnt++;
    eshell_putstring("periodic_timer_handler timer_id:2 elapased:%d, periodic_timer_cnt:%d", elapsed, periodic_user_timer_cnt);
}
#endif

static void timer_test_start(uint32_t timer_id, enum HAL_TIMER_TYPE_T intr_type, uint32_t periodic_ms)
{
    if (timer_id == 1) {
        hal_sysfreq_req(FAST_TIMER_SYSFREQ_USER, HAL_CMU_FREQ_52M);
        if (intr_type == HAL_TIMER_TYPE_ONESHOT) {
            hal_fast_timer_setup(intr_type, oneshot_fast_timer_handler);
        } else if (intr_type == HAL_TIMER_TYPE_PERIODIC) {
            hal_fast_timer_setup(intr_type, periodic_fast_timer_handler);
        }
        fast_start_time = hal_fast_sys_timer_get();
        hal_fast_timer_start(MS_TO_FAST_TICKS(periodic_ms));
#ifdef USER_TIMER_ENABLE
    } else if (timer_id == 2) {
        hal_sysfreq_req(USER_TIMER_SYSFREQ_USER, HAL_CMU_FREQ_52M);
        if (intr_type == HAL_TIMER_TYPE_ONESHOT) {
            hal_user_timer_setup(intr_type, oneshot_user_timer_handler);
        } else if (intr_type == HAL_TIMER_TYPE_PERIODIC) {
            hal_user_timer_setup(intr_type, periodic_user_timer_handler);
        }
        user_start_time = hal_fast_sys_timer_get();
        hal_user_timer_start(MS_TO_FAST_TICKS(periodic_ms));
#endif
    }
    eshell_putstring("timer_test_start timer_id:%d", timer_id);
}

static void timer_test_stop(uint32_t timer_id)
{
    if (timer_id == 1) {
        hal_fast_timer_stop();
        hal_sysfreq_req(FAST_TIMER_SYSFREQ_USER, HAL_CMU_FREQ_32K);
#ifdef USER_TIMER_ENABLE
    } else if (timer_id == 2) {
        hal_user_timer_stop();
        hal_sysfreq_req(USER_TIMER_SYSFREQ_USER, HAL_CMU_FREQ_32K);
#endif
    }
    eshell_putstring("timer_test_stop timer_id:%d", timer_id);
}

static void unitest_timer(int argc, char *argv[])
{
    if (argc < 3) {
        goto exit;
    }

    uint32_t timer_id = atoi(argv[2]);
#ifdef USER_TIMER_ENABLE
    if ((timer_id < 1) || (timer_id > 2)) {
        eshell_putstring("timer_id err, id_range: 1-fast_timer 2-user_timer\r\n");
#else
    if ((timer_id != 1) ) {
        eshell_putstring("timer_id err, id_range: 1-fast_timer\r\n");
#endif
        goto exit;
    }

    if (strncmp(argv[1], "start", 5) == 0) {
        enum HAL_TIMER_TYPE_T intr_type = (enum HAL_TIMER_TYPE_T)atoi(argv[3]);
        uint32_t periodic_ms = atoi(argv[4]);
        if ((intr_type < 1) || (intr_type > 2)) {
            eshell_putstring("intr_type err, type_range: 1-oneshot 2-periodic\r\n");
            return;
        }

        timer_test_start(timer_id, intr_type, periodic_ms);
        return;
    } else if (strncmp(argv[1], "stop", 4) == 0) {
        timer_test_stop(timer_id);
        return;
    }

exit:
   eshell_putstring("Usage: \r\n");
   eshell_putstring("  utest_timer start timer_id intr_type(0/1/2) periodic_ms\r\n");
   eshell_putstring("  utest_timer stop timer_id\r\n");
#ifdef USER_TIMER_ENABLE
   eshell_putstring("  timer_id: 1-fast_timer 2-user_timer\r\n");
#else
    eshell_putstring("  timer_id: 1-fast_timer\r\n");
#endif
    eshell_putstring("  intr_type: 1-oneshot 2-periodic\r\n");
}

ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_UNIT_TEST, "utest_timer", "usage: utest_timer timer_id intr_type(1/2) periodic_ms", unitest_timer);
#endif

