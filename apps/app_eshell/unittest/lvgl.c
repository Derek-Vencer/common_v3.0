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

#if defined(UTILS_ESHELL_EN) && defined(ENABLE_LVGL)
#include "stdlib.h"
#include "string.h"
#include "cmsis_os.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "lvgl.h"
#include "lv_hal_disp.h"
#include "lv_hal_indev.h"
#include "app_utils.h"
#include "ut_test.h"

enum {
    LV_DEMO_BENCHMARK = 0,
    LV_DEMO_STRESS,
    LV_DEMO_WIDGETS,
    LV_DEMO_MUSIC,
};

static uint8_t lv_demo_id = LV_DEMO_BENCHMARK;

static uint32_t prev_time = 0;
static uint32_t sys_ticks = 0;

static uint32_t hal_tick_get(void)
{
    uint32_t now = hal_sys_timer_get();
    sys_ticks += TICKS_TO_MS(now - prev_time);
    prev_time = now;
    return sys_ticks;
}

extern void lv_demo_benchmark(void);
extern void lv_demo_music(void);
extern void lv_demo_stress(void);
extern void lv_demo_widgets(void);

static void lvgl_thread(void)
{
    hal_sysfreq_req(HAL_SYSFREQ_USER_APP_1, HAL_CMU_FREQ_208M);
    TRACE(0, "CPU freq: %u", hal_sys_timer_calc_cpu_freq(5, 0));

    TRACE(0, "Welcome to LVGL\r\n");

    lv_init();

    lv_tick_set_cb(hal_tick_get);

    lv_hal_disp_init();
    lv_hal_indev_init();

    if (lv_demo_id == LV_DEMO_STRESS)
        lv_demo_stress();
    else if (lv_demo_id == LV_DEMO_MUSIC)
        lv_demo_music();
    else if (lv_demo_id == LV_DEMO_WIDGETS)
        lv_demo_widgets();
    else
        lv_demo_benchmark();

    while (1) {
        uint32_t time_till_next = lv_timer_handler();
        lv_delay_ms(time_till_next);
    }

    lv_hal_disp_exit();
    lv_hal_indev_exit();

    lv_deinit();
}

static void test_lvgl(void)
{
    osThreadAttr_t attr;

    memset(&attr, 0, sizeof(osThreadAttr_t));
    attr.name = "lvgl_thread";
    attr.attr_bits = osThreadDetached;
    attr.stack_size = 16384;
    attr.priority = osPriorityHigh;

    osThreadNew((osThreadFunc_t)lvgl_thread, NULL, &attr);
}

static void unitest_lvgl_usage(void)
{
    eshell_putstring("Usage:\r\n");
    eshell_putstring("  utest_lvgl benchmark/widgets/music\r\n");
}

static void unitest_lvgl(int argc, char *argv[])
{
    if (argc < 2) {
        unitest_lvgl_usage();
        return;
    }
    if (strncmp(argv[0], "utest_lvgl", 10) == 0) {
        if (argc > 1) {
            if (strncmp(argv[1], "stress", 6) == 0)
                lv_demo_id = LV_DEMO_STRESS;
            else if (strncmp(argv[1], "music", 5) == 0)
                lv_demo_id = LV_DEMO_MUSIC;
            else if (strncmp(argv[1], "widgets", 7) == 0)
                lv_demo_id = LV_DEMO_WIDGETS;
            else
                lv_demo_id = LV_DEMO_BENCHMARK;
        }
        test_lvgl();
    } else {
        goto usage;
    }

    return;

usage:
    unitest_lvgl_usage();
}

ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_UNIT_TEST, "utest_lvgl", "usage: utest_lvgl help",
                   unitest_lvgl);
#endif
