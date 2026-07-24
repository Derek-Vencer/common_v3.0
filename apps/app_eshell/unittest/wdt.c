/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#include "hal_sleep.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_wdt.h"
#include "stdlib.h"
#include "string.h"
#include "pmu.h"

#if defined(UTILS_ESHELL_EN) && defined(__SYS_AS_MAIN__)

uint32_t wdt_type = 0;

static void digital_wdt_irq_handle(enum HAL_WDT_ID_T id, enum HAL_WDT_EVENT_T event)
{
    eshell_putstring("wait wdt timeout...");
    eshell_putstring("%s id:%d event:%d", __func__, id, event);
}

static void pmu_wdt_irq_handler(void)
{
    eshell_putstring("wait wdt timeout...");
}

static int wdt_kick(void)
{
    static uint8_t cnt = 0;
    if (wdt_type == 0) {
        pmu_wdt_feed();
    } else if (wdt_type == 1) {
        hal_wdt_ping(HAL_WDT_ID_0);
    }
    eshell_putstring("feeded dog. times: %d!!!\r\n", ++cnt);
    return 0;
}

static int wdt_open(int seconds)
{
    if (wdt_type == 0) {
        pmu_wdt_config((seconds * 1000), (seconds * 1000));
        pmu_wdt_set_irq_handler(pmu_wdt_irq_handler);
        pmu_wdt_start();
    } else if (wdt_type == 1) {
        hal_wdt_set_irq_callback(HAL_WDT_ID_0, digital_wdt_irq_handle);
        hal_wdt_set_timeout(HAL_WDT_ID_0, seconds);
        hal_wdt_start(HAL_WDT_ID_0);
    }
    return 0;
}

static void test_wdt(uint32_t wdt_delaytime, uint32_t wdt_timeout, uint32_t wdt_cnt)
{
    wdt_open(wdt_timeout);
    while (wdt_cnt) {
        wdt_cnt--;
        eshell_putstring("dealy %dms, and then feeding dog.\r\n", wdt_delaytime);
        hal_sys_timer_delay(MS_TO_TICKS(wdt_delaytime));
        wdt_kick();
    }
    eshell_putstring("wait wdt timeout...%dms\r\n", wdt_timeout * 1000);
    while (1) {
        hal_sys_timer_delay(MS_TO_TICKS(wdt_delaytime));
        eshell_putstring("delay %dms...\r\n", wdt_delaytime);
    }
}

static void unitest_wdt(int argc, char *argv[])
{
    uint32_t wdt_delaytime;
    uint32_t wdt_timeout;
    uint32_t wdt_cnt;
    if (argc != 5) {
        eshell_putstring("ERROR_ARG\r\n");
        return;
    }
    wdt_type = atoi(argv[1]);
    wdt_delaytime = atoi(argv[2]);
    wdt_timeout = atoi(argv[3]);
    wdt_cnt = atoi(argv[4]);

    if ((wdt_type < 0) || (wdt_type > 1)) {
        eshell_putstring("wdt_type err, 0:pmu_wdt  1:digital_wdt");
        return;
    }

    test_wdt(wdt_delaytime, wdt_timeout, wdt_cnt);
}
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_UNIT_TEST, "utest_wdt", "utest_wdt pmu/dig_wdt(0/1) delaytime(ms) timeout(s) cnt",
                   unitest_wdt);
#endif
