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
#ifndef __PMU_BEST1503P_H__
#define __PMU_BEST1503P_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_cmu.h"

#define CAP_REG(r)                          (((r) & 0xFFF) | 0x0000)
#define ANA_REG(r)                          (((r) & 0xFFF) | 0x1000)
#define RF_REG(r)                           (((r) & 0x3FF) | (0 << 10) | 0x2000)
#define RF1_REG(r)                          (((r) & 0x3FF) | (1 << 10) | 0x2000)
#define PMU_REG(r)                          (((r) & 0xFFF) | 0x3000)

#define ISPI_PMU_REG(reg)                   (((reg) & 0xFFF) | 0x3000)

#define MAX_VMIC_CH_NUM                     2

enum PMU_EFUSE_PAGE_T {
    PMU_EFUSE_PAGE_SECURITY     = 0x0,
    PMU_EFUSE_PAGE_BOOT         = 0x1,
    PMU_EFUSE_PAGE_FEATURE      = 0x2,
    PMU_EFUSE_PAGE_BATTER_LV    = 0x3,
    PMU_EFUSE_PAGE_BATTER_HV    = 0x4,
    PMU_EFUSE_PAGE_SW_CFG       = 0x5,
    PMU_EFUSE_PAGE_PROD_TEST    = 0x6,
    PMU_EFUSE_PAGE_7            = 0x7,
    PMU_EFUSE_PAGE_8            = 0x8,
    PMU_EFUSE_PAGE_9            = 0x9,
    PMU_EFUSE_PAGE_A            = 0xA,
    PMU_EFUSE_PAGE_B            = 0xB,
    PMU_EFUSE_PAGE_C            = 0xC,
    PMU_EFUSE_PAGE_D            = 0xD,
    PMU_EFUSE_PAGE_E            = 0xE,
    PMU_EFUSE_PAGE_F            = 0xF,
    PMU_EFUSE_PAGE_GPADC_LV     = 0x10,
    PMU_EFUSE_PAGE_11           = 0x11,
    PMU_EFUSE_PAGE_GPADC_HV     = 0x12,
    PMU_EFUSE_PAGE_13           = 0x13,
    PMU_EFUSE_PAGE_14           = 0x14,
    PMU_EFUSE_PAGE_15           = 0x15,
    PMU_EFUSE_PAGE_16           = 0x16,
    PMU_EFUSE_PAGE_17           = 0x17,
    PMU_EFUSE_PAGE_18           = 0x18,
    PMU_EFUSE_PAGE_1F           = 0x1F,

    PMU_EFUSE_PAGE_QTY
};

enum PMU_IRQ_TYPE_T {
    PMU_IRQ_TYPE_GPADC,
    PMU_IRQ_TYPE_RTC,
    PMU_IRQ_TYPE_CHARGER,
    PMU_IRQ_TYPE_GPIO,
    PMU_IRQ_TYPE_WDT,
    PMU_IRQ_TYPE_PWRKEY,

    PMU_IRQ_TYPE_QTY
};

enum PMU_PLL_DIV_TYPE_T {
    PMU_PLL_DIV_DIG,
    PMU_PLL_DIV_CODEC,
};

enum PMU_BIG_BANDGAP_USER_T {
    PMU_BIG_BANDGAP_USER_GPADC          = (1 << 0),
    PMU_BIG_BANDGAP_USER_SAR_VREF_OUTPUT = (1 << 1),
    PMU_BIG_BANDGAP_USER_PMU_REBOOT     = (1 << 2),
    PMU_BIG_BANDGAP_USER_VMIC1          = (1 << 3),
    PMU_BIG_BANDGAP_USER_VMIC2          = (1 << 4),
};

enum PMU_NTC_USER_T {
    PMU_NTC_USER_BT,
    PMU_NTC_USER_PMU,

    PMU_NTC_USER_QTY,
};

union SECURITY_VALUE_T {
    struct {
        unsigned short security_en      :1;
        unsigned short mode             :1;
        unsigned short sig_type         :1;
        unsigned short reserved         :1;
        unsigned short key_id           :2;
        unsigned short vendor_id        :5;
        unsigned short flash_id         :1;
        unsigned short chksum           :4;
    } root;
    struct {
        unsigned short security_en      :1;
        unsigned short mode             :1;
        unsigned short sig_type         :1;
        unsigned short skip_romkey      :1;
        unsigned short key_id           :2;
        unsigned short reg_base         :2;
        unsigned short reg_size         :2;
        unsigned short reg_offset       :1;
        unsigned short flash_id         :1;
        unsigned short chksum           :4;
    } otp;
    unsigned short reg;
};
#define SECURITY_VALUE_T                    SECURITY_VALUE_T

enum PMU_CLK_SEL_RC_USER_T {
    PMU_CLK_SEL_RC_USER_SYS      = (1 << 0),
    PMU_CLK_SEL_RC_USER_CAP      = (1 << 1),
};

struct PMU_NTC_MONITOR_CTX_T {
    uint32_t interval_ms;
    bool enable;
};

struct PMU_NTC_CTX_T {
    int temperature;
    uint32_t temperature_update_ms;
    uint32_t monitor_interval_ms;
};

enum HAL_FLASH_ID_T pmu_get_boot_flash_ctrl_id(void);

uint8_t pmu_gpio_setup_irq(enum HAL_GPIO_PIN_T pin, const struct HAL_GPIO_IRQ_CFG_T *cfg);

void pmu_codec_mic_bias_enable(uint32_t map, int enable);

void pmu_codec_mic_bias_lowpower_mode(uint32_t map, int enable);

int pmu_codec_volt_ramp_up(void);

int pmu_codec_volt_ramp_down(void);

void pmu_pll_div_reset_set(enum HAL_CMU_PLL_T pll);

void pmu_pll_div_reset_clear(enum HAL_CMU_PLL_T pll);

void pmu_pll_div_set(enum HAL_CMU_PLL_T pll, enum PMU_PLL_DIV_TYPE_T type, uint32_t div);

void pmu_pll_freq_reg_set(uint16_t low, uint16_t high, uint16_t high2);

void pmu_led_uart_enable(enum HAL_IOMUX_PIN_T pin);

void pmu_led_uart_disable(enum HAL_IOMUX_PIN_T pin);

void pmu_bt_reconn(bool en);

void pmu_bt_2v5pa(bool en);

bool pmu_dig_set_volt_value(uint16_t dcdc_volt_mv, uint16_t logic_volt_mv, uint16_t ldo_volt_mv);

void bbpll_pll_update(uint64_t pll_cfg_val, bool calib);

void bbpll_set_codec_div(uint32_t div);

void bbpll_codec_clock_enable(bool en);

void bbpll_auto_calib_spd(uint32_t codec_freq, uint32_t div);

void pmu_vio_3v3(bool en);

void pmu_rc_clk_test(void);

int pmu_rc_clk_calib(void);

void pmu_rc_clk_enable(enum PMU_CLK_SEL_RC_USER_T user);

void pmu_rc_clk_disable(enum PMU_CLK_SEL_RC_USER_T user);

typedef void (*PMU_NTC_IRQ_HANDLER_T)(uint16_t raw_val, int temperature);

int pmu_wdt_load_test(void);

int pmu_sar_input_buf_enable(int enable);

void pmu_ldo_vsensor_off_pull_down(void);

void pmu_ldo_vsensor_on_lp_dr(bool dr_lp);

void pmu_fast_wakeup_enable(void);

void pmu_charger_pattern_enable(bool enable);

void pmu_rf_ana_init(void);

int pmu_wdt_reboot(void);

int pmu_ntc_ctx_get(struct PMU_NTC_CTX_T *ctx);

int pmu_ntc_monitor_register(enum PMU_NTC_USER_T user, struct PMU_NTC_MONITOR_CTX_T *ctx);

void pmu_big_bandgap_enable(enum PMU_BIG_BANDGAP_USER_T user, int enable);
#ifdef __cplusplus
}
#endif

#endif
