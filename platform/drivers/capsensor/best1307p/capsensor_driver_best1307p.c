/***************************************************************************
 *
 * Copyright 2022-2023 BES.
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
#ifndef CHIP_SUBSYS_SENS
#include "capsensor_driver_best1307p.h"
#include "analog.h"
#include "pmu.h"
#include "touch_wear_core.h"
#include "cmsis.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "capsensor_spi_best1307p.h"
#include "tgt_hardware_capsensor.h"
#include "capsensor_driver.h"

#ifdef BESUI_CAPSENSOR_FACTORY_EN
#include "hal_trace.h"
#include "capsensor_factory_cal.h"
#endif
#ifdef BESUI_TOUCH_POWERON_EN
#include "besui_common.h"
#endif

capsensor_channel_number_get_func_type capsensor_channel_number_get_cb = NULL;
capsensor_conversion_number_get_func_type capsensor_conversion_number_get_cb = NULL;
capsensor_channel_map_get_func_type capsensor_channel_map_get_cb = NULL;
capsensor_samp_fs_get_func_type capsensor_samp_fs_get_cb = NULL;

int capsensor_channel_number_get_callback(capsensor_channel_number_get_func_type p)
{
    int8_t ret = 0;

    if (p == NULL) {
        ret = -1;
        return ret;
    }

    capsensor_channel_number_get_cb = p;
    return ret;
}

int capsensor_conversion_number_get_callback(capsensor_conversion_number_get_func_type p)
{
    int8_t ret = 0;

    if (p == NULL) {
        ret = -1;
        return ret;
    }

    capsensor_conversion_number_get_cb = p;
    return ret;
}

int capsensor_channel_map_get_callback(capsensor_channel_map_get_func_type p)
{
    int8_t ret = 0;

    if (p == NULL) {
        ret = -1;
        return ret;
    }

    capsensor_channel_map_get_cb = p;
    return ret;
}

int capsensor_samp_fs_get_callback(capsensor_samp_fs_get_func_type p)
{
    int8_t ret = 0;

    if (p == NULL) {
        ret = -1;
        return ret;
    }

    capsensor_samp_fs_get_cb = p;
    return ret;
}

void capsensor_driver_init(void)
{
    analog_capsensor_open();
}

/***************************************************************************
 * @brief capsensor baseline and sar init control function
 *
 ***************************************************************************/
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
    return (CAP_CH0 | CAP_CH1 | CAP_CH2 | CAP_CH3 | CAP_CH4);
#endif
}

void capsensor_gpio_init(void)
{
#ifdef CAP_CHNL_BONDING_WITH_GPIO
    uint8_t cap_ch_map = capsensor_ch_map_get();
    unsigned short mic_cfg = 0;

    pmu_get_sw_cfg_value(PMU_EFUSE_MIC_CFG, &mic_cfg);
    if(cap_ch_map & CAP_CH0) {
        hal_iomux_set_analog_io(HAL_IOMUX_PIN_P0_7);
    }

    if(cap_ch_map & CAP_CH1) {
        if (mic_cfg == 0) {
            hal_iomux_set_analog_io(HAL_IOMUX_PIN_P0_6);
        } else if(mic_cfg == 1) {
            hal_iomux_set_analog_io(HAL_IOMUX_PIN_P0_0);
        }
    }

    if(cap_ch_map & CAP_CH2) {
        if (mic_cfg == 0) {
            hal_iomux_set_analog_io(HAL_IOMUX_PIN_P1_5);
        } else if(mic_cfg == 1) {
            hal_iomux_set_analog_io(HAL_IOMUX_PIN_P0_1);
        }
    }

    if(cap_ch_map & CAP_CH3) {
        hal_iomux_set_analog_io(HAL_IOMUX_PIN_P1_4);
    }

    if(cap_ch_map & CAP_CH4) {
        hal_iomux_set_analog_io(HAL_IOMUX_PIN_P1_3);
    }

    if(cap_ch_map & CAP_CH5) {
        hal_iomux_set_analog_io(HAL_IOMUX_PIN_P0_2);
    }

    if(cap_ch_map & CAP_CH6) {
        hal_iomux_set_analog_io(HAL_IOMUX_PIN_P0_3);
    }
#endif
}

/***************************************************************************
 * @brief suspend capsensor functions.
 *
 ***************************************************************************/
void capsensor_suspend(void)
{
    analog_capsensor_clk_gate_on();
}

/***************************************************************************
 * @brief start capsensor functions.
 *
 ***************************************************************************/
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

#endif
