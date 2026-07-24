/***************************************************************************
 *
 * Copyright 2015-2025 BES.
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
#include "analog.h"
#include "cmsis.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "hal_cmu.h"
#include "hal_location.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "pmu.h"
#include CHIP_SPECIFIC_HDR(hal_cmu_pri)
#include CHIP_SPECIFIC_HDR(reg_pmu)

#define rc_read(reg,val)            hal_analogif_reg_read(PMU_REG(reg),val)
#define rc_write(reg,val)           hal_analogif_reg_write(PMU_REG(reg),val)

#define RCX8_FREQ_MHZ               384
#define RCX8_FIRST_CALIB_MODE       0
#define RCX8_CALIB_MODE             0
#define RCX8_CALIB_TIMEOUT_MS       2

#define RC2M_CALIB_TIMEOUT_MS       20

#define RCX8_PU_STABLE_TIME_US      10
#define RC2M_PU_STABLE_TIME_US      10

#define RC_AGPIO_CLKOUT_PIN         HAL_IOMUX_PIN_P1_6

enum RC_ANA_CLOCK_OUT_ID_T {
    RC_ANA_CLOCK_OUT_RCX8,
    RC_ANA_CLOCK_OUT_RC2M,
};

POSSIBLY_UNUSED
static void rc_set_analog_gpio_clkout(enum RC_ANA_CLOCK_OUT_ID_T id)
{
    uint16_t val;

    if (id == RC_ANA_CLOCK_OUT_RCX8) {
        rc_read(PMU_REG_RCOSC_192M_CFG, &val);
        val |= REG_RCOSC_192M_EN_AGPIO_TEST_SEL | REG_RCOSC_192M_EN_CLK_DLATCH | REG_RCOSC_192M_EN_AGPIO_TEST;
        rc_write(PMU_REG_RCOSC_192M_CFG, val);
    } else {
        rc_read(PMU_REG_RCOSC_192M_CFG, &val);
        val &= ~REG_RCOSC_192M_EN_AGPIO_TEST_SEL;
        rc_write(PMU_REG_RCOSC_192M_CFG, val);

        rc_read(PMU_REG_RCOSC_2M_CFG, &val);
        val |= REG_RCOSC_192M_EN_CLK_DLATCH | REG_RCOSC_2M_EN_AGPIO_TEST;
        rc_write(PMU_REG_RCOSC_2M_CFG, val);

        rc_read(PMU_REG_DBG_RCOSC6M_REF_CNT, &val);
        val |= REG_RCOSC_2M_BYPASS_COUNTER;
        rc_write(PMU_REG_DBG_RCOSC6M_REF_CNT, val);
    }

    hal_iomux_set_analog_io(RC_AGPIO_CLKOUT_PIN);
}

#ifdef RC_FAST_WAKEUP
static int rc_fast_wakeup_calib(void)
{
    uint16_t val, val_cfg;
    int ret = 0;

    rc_read(PMU_REG_RCOSC_2M_CFG, &val_cfg);
    val_cfg |= REG_RCOSC_2M_PU_LDO | REG_RCOSC_2M_PU_LDO_DR | REG_RCOSC_2M_EN | REG_RCOSC_2M_EN_DR;
    val_cfg |= REG_RCOSC_2M_CALIB_GATE;
    rc_write(PMU_REG_RCOSC_2M_CFG, val_cfg);

    hal_cmu_pu_osc_enable(HAL_CMU_PU_OSC_USER_RC2M_CALIB);
#ifndef SIMU
    hal_sys_timer_delay_us(HAL_CMU_OSC_STABLE_TIME);
#endif

    rc_read(PMU_REG_RCOSC2M_CALIB, &val);
    val &= ~REG_RCOSC2M_SFT_RSTN;
    rc_write(PMU_REG_RCOSC2M_CALIB, val);
#ifndef SIMU
    hal_sys_timer_delay(1);
#endif
    val |= REG_RCOSC2M_SFT_RSTN;
    rc_write(PMU_REG_RCOSC2M_CALIB, val);

    val &= ~REG_RCOSC2M_CALIB_EN;
    rc_write(PMU_REG_RCOSC2M_CALIB, val);
#ifndef SIMU
    hal_sys_timer_delay(1);
#endif
    val |= REG_RCOSC2M_CALIB_EN;
    rc_write(PMU_REG_RCOSC2M_CALIB, val);

#ifndef SIMU
    uint32_t start = hal_sys_timer_get();
    do {
        rc_read(PMU_REG_DBG_RCOSC2M, &val);
        if (hal_sys_timer_get() - start > MS_TO_TICKS(RC2M_CALIB_TIMEOUT_MS)) {
            ret = 1;
            break;
        }
    } while ((val & DBG_RCOSC2M_TUN_DONE) == 0);
#endif

    // Resume
    val_cfg &= ~(REG_RCOSC_2M_PU_LDO | REG_RCOSC_2M_PU_LDO_DR | REG_RCOSC_2M_EN | REG_RCOSC_2M_EN_DR);
    val_cfg &= ~REG_RCOSC_2M_CALIB_GATE;
    rc_write(PMU_REG_RCOSC_2M_CFG, val_cfg);

    hal_cmu_pu_osc_disable(HAL_CMU_PU_OSC_USER_RC2M_CALIB);

    return ret;
}

static void rc_fast_wakeup_enable(void)
{
    uint16_t val;

    rc_read(PMU_REG_RCOSC_2M_CFG, &val);
    val = SET_BITFIELD(val, REG_RCOSC_2M_PWR_BIT, 0x3);
    rc_write(PMU_REG_RCOSC_2M_CFG, val);

    rc_read(PMU_REG_RCOSC2M_GOAL_CNT_MARK, &val);
    val = SET_BITFIELD(val, REG_RCOSC2M_GOAL_CNT_MARK_15_0, 0x2FFF);
    rc_write(PMU_REG_RCOSC2M_GOAL_CNT_MARK, val);

    rc_read(PMU_REG_RCOSC2M_CALIB, &val);
    val = SET_BITFIELD(val, REG_RCOSC2M_REF_DIV_NUM, 0x1FF);
    rc_write(PMU_REG_RCOSC2M_CALIB, val);

#ifndef SIMU
    int ret;
    ret = rc_fast_wakeup_calib();
    if (ret) {
        DRIVERS_TRACE(0, "%s: rc_fast_wakeup_calib() fail!!! ret:%d", __func__, ret);
    }
#endif

    rc_read(PMU_REG_RCOSC_2M_CFG, &val);
    val |= REG_RCOSC_2M_EN_CLK_DIG;
    rc_write(PMU_REG_RCOSC_2M_CFG, val);
}
#endif

static int _rc_clock_calib(int first_cal)
{
    uint16_t val, val_cfg, val_gate, val_intr;
    uint16_t ftrim_cal, ftrim_fine_cal;
    int ret = 0;

    rc_read(PMU_REG_RCOSC_192M_CFG, &val_cfg);
    val_cfg |= REG_RCOSC_192M_EN_CLK_DLATCH | REG_RCOSC_192M_EN_CLK_ADC_DIG;
    val_cfg |= REG_RCOSC_192M_EN_DR | REG_RCOSC_192M_EN | REG_RCOSC_192M_PU_LDO_DR | REG_RCOSC_192M_PU_LDO;
    rc_write(PMU_REG_RCOSC_192M_CFG, val_cfg);

#ifdef RC_CAL_USE_EXT_32K
    hal_sys_timer_delay_us(RCX8_PU_STABLE_TIME_US);
#else
    hal_cmu_pu_osc_enable(HAL_CMU_PU_OSC_USER_RCX8_CALIB);
#ifndef SIMU
    hal_sys_timer_delay_us(HAL_CMU_OSC_STABLE_TIME);
#endif
#endif

    if (!first_cal && RCX8_CALIB_MODE != 0) {
        rc_read(PMU_REG_RC_192M_CALIB_STAT, &val);
        ftrim_cal = GET_BITFIELD(val, RC_192M_CALIB_FTRIM);
        ftrim_fine_cal = GET_BITFIELD(val, RC_192M_CALIB_FTRIM_FINE);
        rc_read(PMU_REG_RC_192M_CALIB_INTR, &val);
        val = SET_BITFIELD(val, REG_RC_192M_CALIB_FTRIM, ftrim_cal);
        val = SET_BITFIELD(val, REG_RC_192M_CALIB_FTRIM_FINE, ftrim_fine_cal);
        rc_write(PMU_REG_RC_192M_CALIB_INTR, val);

        rc_read(PMU_REG_RC_192M_CALIB_CNT, &val);
        val = SET_BITFIELD(val, REG_RC_192M_CALIB_MODE, RCX8_CALIB_MODE);
        rc_write(PMU_REG_RC_192M_CALIB_CNT, val);
    }

    rc_read(PMU_REG_RCOSC_192M_CALIB, &val_gate);
    val_gate |= REG_RCOSC_192M_CALIB_GATE;
    rc_write(PMU_REG_RCOSC_192M_CALIB, val_gate);

    rc_read(PMU_REG_RC_192M_CALIB_INTR, &val_intr);
    val_intr |= REG_RC_192M_CALIB_DONE_INTR_EN;
    rc_write(PMU_REG_RC_192M_CALIB_INTR, val_intr);

    rc_read(PMU_REG_RC_192M_CALIB_STOP_CLR, &val);
    val |= PU_RC_192M_32K_CALIB_STOP_CLR;
    rc_write(PMU_REG_RC_192M_CALIB_STOP_CLR, val);

    if (first_cal) {
        rc_read(PMU_REG_RC_192M_CALIB_CNT, &val);
        val &= ~REG_RC_192M_CALIB_EN;
        rc_write(PMU_REG_RC_192M_CALIB_CNT, val);
#ifndef SIMU
        hal_sys_timer_delay(1);
#endif
        val |= REG_RC_192M_CALIB_EN;
        rc_write(PMU_REG_RC_192M_CALIB_CNT, val);
    } else {
        rc_read(PMU_REG_RC_192M_CFG_16B, &val);
        val = (val & ~REG_RC_32K_CALIB_192M) | RC_32K_CALIB_DR_192M;
        rc_write(PMU_REG_RC_192M_CFG_16B, val);
#ifndef SIMU
        hal_sys_timer_delay(1);
#endif
        val |= REG_RC_32K_CALIB_192M | RC_32K_CALIB_DR_192M;
        rc_write(PMU_REG_RC_192M_CFG_16B, val);
    }

#ifndef SIMU
    uint32_t start = hal_sys_timer_get();
    do {
        rc_read(PMU_REG_RC_192M_CALIB_STAT, &val);
        if (hal_sys_timer_get() - start > MS_TO_TICKS(RCX8_CALIB_TIMEOUT_MS)) {
            ret = 1;
            break;
        }
    } while ((val & RC_192M_CALIB_DONE_RAW_INTR) == 0);
#endif

    // Resume
    val_intr = (val_intr & ~REG_RC_192M_CALIB_DONE_INTR_EN) | REG_RC_192M_CALIB_DONE_INTR_CLR;
    rc_write(PMU_REG_RC_192M_CALIB_INTR, val_intr);

    val_cfg &= ~(REG_RCOSC_192M_EN_DR | REG_RCOSC_192M_EN | REG_RCOSC_192M_PU_LDO_DR | REG_RCOSC_192M_PU_LDO);
    val_cfg &= ~(REG_RCOSC_192M_EN_CLK_DLATCH | REG_RCOSC_192M_EN_CLK_ADC_DIG);
    rc_write(PMU_REG_RCOSC_192M_CFG, val_cfg);

    val_gate &= ~REG_RCOSC_192M_CALIB_GATE;
    rc_write(PMU_REG_RCOSC_192M_CALIB, val_gate);

#ifndef RC_CAL_USE_EXT_32K
    hal_cmu_pu_osc_disable(HAL_CMU_PU_OSC_USER_RCX8_CALIB);
#endif

    return ret;
}

int rc_clock_calib(void)
{
    int ret;

    ret = _rc_clock_calib(false);

    return ret;
}

void rc_clock_enable(void)
{
    enum HAL_CMU_CLK_SEL_OSC_USER_T user;
    uint16_t val;

    rc_read(PMU_REG_RC_192M_REF_CNT, &val);
    val = SET_BITFIELD(val, REG_RC_192M_CALIB_REF_CNT, 0xF);
    rc_write(PMU_REG_RC_192M_REF_CNT, val);

    rc_write(PMU_REG_RC_192M_GOAL_CNT, 0x5F);

    rc_read(PMU_REG_RC_192M_CALIB_CNT, &val);
    val = SET_BITFIELD(val, REG_RC_192M_CALIB_MODE, RCX8_FIRST_CALIB_MODE);
    rc_write(PMU_REG_RC_192M_CALIB_CNT, val);

#ifndef SIMU
    int ret;
    ret = _rc_clock_calib(true);
    if (ret) {
        DRIVERS_TRACE(0, "%s: fail! ret=%d", __func__, ret);
    }
#endif

    rc_read(PMU_REG_RCOSC_192M_CALIB, &val);
    val = SET_BITFIELD(val, REG_RCOSC_192M_DIG_SEL, 0x3);
    rc_write(PMU_REG_RCOSC_192M_CALIB, val);

    rc_read(PMU_REG_RCOSC_192M_CFG, &val);
    val |= REG_RCOSC_192M_EN_CLK_DIG;
    rc_write(PMU_REG_RCOSC_192M_CFG, val);

#ifdef RC_FAST_WAKEUP
    rc_fast_wakeup_enable();
    pmu_fast_wakeup_enable();
#endif

#ifdef RC_CAL_USE_EXT_32K
#ifndef PMU_CLK_USE_EXT_CRYSTAL
#error "PMU_CLK_USE_EXT_CRYSTAL must be defined if RC_CAL_USE_EXT_32K=1"
#endif
    rc_read(PMU_REG_RC_192M_CFG_16B, &val);
    val |= REG_RC_CAL_REF_32K_SEL;
    rc_write(PMU_REG_RC_192M_CFG_16B, val);

    rc_read(PMU_REG_RC_192M_CALIB_32K_CFG, &val);
    val |= REG_CLK_32K_CALIB_RC_EN;
    rc_write(PMU_REG_RC_192M_CALIB_32K_CFG, val);

    rc_read(PMU_REG_RC_192M_REF_CNT, &val);
    val = SET_BITFIELD(val, REG_RC_192M_CALIB_REF_CNT, 0x1);
    rc_write(PMU_REG_RC_192M_REF_CNT, val);

    rc_write(PMU_REG_RC_192M_GOAL_CNT, 0x16D);
#endif

    user = 0;
#ifdef SYS_USE_RC_CLK
    user |= HAL_CMU_CLK_SEL_OSC_USER_SYS;
#endif
    if (user == 0) {
        user = HAL_CMU_CLK_SEL_OSC_USER_SYS;
    }
    hal_cmu_select_rc_clock(user);
}

void rc_clock_disable(void)
{
}
