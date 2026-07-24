/***************************************************************************
*
* Copyright 2015-2026 BES.
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
#include "capsensor_driver_best1307ph.h"
#include "analog.h"
#include "pmu.h"
#include "capsensor_spi_best1307ph.h"
#include "tgt_hardware_capsensor.h"
#include "capsensor_driver.h"
#ifdef BESUI_TOUCH_POWERON_EN
#include "besui_common.h"
#endif

void capsensor_driver_init(void)
{
    analog_capsensor_open();
}

void capsensor_sar_baseline_init(void)
{
}

uint8_t capsensor_ch_num_get(void)
{
    return CAP_CHNUM;
}

uint8_t capsensor_conversion_num_get(void)
{
    return CAP_REPNUM;
}

uint16_t capsensor_samp_fs_get(void)
{
    return CAP_SAMP_FS;
}

uint8_t capsensor_ch_map_get(void)
{
#ifdef BESUI_TWS_EN
    return BESUI_CAP_CH;
#else
    return (CAP_CH0 | CAP_CH1 | CAP_CH2 | CAP_CH3 | CAP_CH4 | CAP_CH5 | CAP_CH6);
#endif
}

void capsensor_gpio_init(void)
{
#ifdef CAP_CHNL_BONDING_WITH_GPIO
    uint8_t cap_ch_map = capsensor_ch_map_get();

    if (cap_ch_map & CAP_CH0) {
        hal_iomux_set_analog_io(HAL_IOMUX_PIN_P0_7);
    }

    if (cap_ch_map & CAP_CH1) {
        hal_iomux_set_analog_io(HAL_IOMUX_PIN_P0_6);
    }

    if (cap_ch_map & CAP_CH2) {
        hal_iomux_set_analog_io(HAL_IOMUX_PIN_P1_5);
    }

    if (cap_ch_map & CAP_CH3) {
        hal_iomux_set_analog_io(HAL_IOMUX_PIN_P1_4);
    }

    if (cap_ch_map & CAP_CH4) {
        hal_iomux_set_analog_io(HAL_IOMUX_PIN_P1_3);
    }

    if (cap_ch_map & CAP_CH5) {
        hal_iomux_set_analog_io(HAL_IOMUX_PIN_P0_2);
    }

    if (cap_ch_map & CAP_CH6) {
        hal_iomux_set_analog_io(HAL_IOMUX_PIN_P0_3);
    }
#endif
}

void capsensor_suspend(void)
{
    analog_capsensor_clk_gate_on();
}

void capsensor_resume(void)
{
    analog_capsensor_clk_gate_off();
}

uint16_t capsensor_clk_is_ready(void)
{
    return analog_capsensor_clk_is_ready();
}

void capsensor_close(void)
{
    analog_capsensor_reset_set();
}

void capsensor_open(void)
{
    analog_capsensor_reset_clear();
    analog_capsensor_open();
    capsensor_drv_start();
}

void capsensor_sens2mcu_irq_set(void)
{
}

void capsensor_driver_baseline_dr(uint32_t* baseline_value_p)
{
    capsensor_baseline_dr(baseline_value_p);
}

void capsensor_driver_baseline_reg_read(uint32_t* baseline_value_p)
{
    capsensor_baseline_reg_read(baseline_value_p);
}
