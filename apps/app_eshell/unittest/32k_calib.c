
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
#if defined(UTILS_ESHELL_EN) && (PMU_CLK_USE_EXT_CRYSTAL) && (CALIB_SLOW_TIMER) && (TIMER_USE_FPU)
/* compilation condition:PMU_CLK_USE_EXT_CRYSTAL=1 CALIB_SLOW_TIMER=1 TIMER_USE_FPU=1 */
#include "eshell.h"
#include "cmsis_os.h"
#include "pmu.h"
#include "hal_timer.h"
#include "hal_trace.h"

static void unitest_32k_calib(int argc, char *argv[])
{
    uint32_t p_wait_ms;
    uint32_t cnt = 0;
    double calib = 0;
    do {
        pmu_external_crystal_calib(&p_wait_ms);
        osDelay(p_wait_ms + 100);
        cnt++;
        calib = hal_sys_timer_systick_hz_float();
        eshell_putstring("pmu_external_crystal_calib cnt=%d calib = %u.%02u ", cnt, (unsigned)calib, ((unsigned)(calib * 100) - ((unsigned)calib * 100)));
    } while (ABS(calib - 16384) > ((double)0.1));
    eshell_putstring("32K calib OK");

    pmu_external_crystal_enable();
}

ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_UNIT_TEST, "utest_32k_calib", "usage: utest_32k_calib help", unitest_32k_calib);

#endif

