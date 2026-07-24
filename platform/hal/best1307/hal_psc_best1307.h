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
#ifndef __HAL_PSC_BEST1307_H__
#define __HAL_PSC_BEST1307_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "hal_gpio.h"

enum HAL_PSC_SHR_RAM_PD_CFG_T {
    HAL_PSC_SHR_RAM_FORCE_PU = 0,
    HAL_PSC_SHR_RAM_FORCE_PD,
    HAL_PSC_SHR_RAM_SLEEP_PU,
    HAL_PSC_SHR_RAM_SLEEP_PD,
};

void hal_psc_jtag_select_sens(void);

void hal_psc_gpio_irq_enable(enum HAL_GPIO_PIN_T pin);

void hal_psc_gpio_irq_disable(enum HAL_GPIO_PIN_T pin);

uint32_t hal_psc_gpio_irq_get_status(uint32_t *status, uint32_t cnt);

uint32_t hal_psc_get_power_loop_cycle_cnt(void);

void hal_psc_rf_inf_enable_auto_power_down(void);

void hal_psc_rf_inf_disable_auto_power_down(void);

bool hal_psc_pll_freq_is_high(void);

void hal_psc_pll_freq_sel_high(bool high);

void hal_psc_mcu_ram_auto_psw_enable(bool enable);

#ifdef __cplusplus
}
#endif

#endif

