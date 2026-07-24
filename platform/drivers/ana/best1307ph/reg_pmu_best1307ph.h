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
#ifndef __REG_PMU_BEST1307PH_H__
#define __REG_PMU_BEST1307PH_H__

#include "plat_types.h"

enum PMU_REG_T {
    PMU_REG_METAL_ID                = 0x00,
    PMU_REG_POWER_KEY_CFG           = 0x02,
    PMU_REG_CHARGER_CFG             = 0x05,
    PMU_REG_ANA_CFG                 = 0x07,
    PMU_REG_USB_CFG                 = 0x0A,
    PMU_REG_AVDD_EN                 = 0x0D,
    PMU_REG_SLEEP_CFG               = 0x17,
    PMU_REG_INT_MASK                = 0x1F,
    PMU_REG_INT_EN                  = 0x20,
    PMU_REG_RTC_DIV_1HZ             = 0x22,
    PMU_REG_GPADC_CLK_DIV           = 0x23,
    PMU_REG_MIC_BIAS_A              = 0x29,
    PMU_REG_MIC_BIAS_B              = 0x2A,
    PMU_REG_IPTAT_CORE_I_CFG        = 0x34,
    PMU_REG_DIV_HW_RESET_CFG        = 0x35,
    PMU_REG_WDT_RESET_TIMER         = 0x36,
    PMU_REG_WDT_CFG                 = 0x37,
    PMU_REG_PWMB_TOGGLE             = 0x3A,
    PMU_REG_PWMB_ST1                = 0x3B,
    PMU_REG_PWMB_EN                 = 0x3C,
    PMU_REG_PWMB_BR_EN              = 0x3D,
    PMU_REG_LED_CFG_IO2             = 0x3E,
    PMU_REG_USB_CFG_3F              = 0x3F,
    PMU_REG_USB_CFG2                = 0x40,
    PMU_REG_USB_CFG3                = 0x41,
    PMU_REG_WDT_INT_CFG             = 0x43,
    PMU_REG_MIC_BIAS_C              = 0x45,
    PMU_REG_UART1_CFG               = 0x46,
    PMU_REG_UART2_CFG               = 0x47,
    PMU_REG_LED_IO_IN               = 0x48,
    PMU_REG_WDT_IRQ_TIMER           = 0x4D,
    PMU_REG_POWER_OFF               = 0x4F,
    PMU_REG_INT_STATUS              = 0x50,
    PMU_REG_INT_MSKED_STATUS        = 0x51,
    PMU_REG_INT_CLR                 = 0x51,
    PMU_REG_RAW_STATUS              = 0x52,
    PMU_REG_GPADC_INT_MSKED         = 0x53,
    PMU_REG_GPADC_INT_CLR           = 0x53,
    PMU_REG_CHARGER_STATUS          = 0x5E,
    PMU_REG_NOT_RESET_61            = 0x61,
    PMU_REG_MIC_PULL_DOWN           = 0x67,
    PMU_REG_SAR_CFG_8C              = 0x8C,
    PMU_REG_SAR_ADC_OFFSET_CFG      = 0x8E,
    PMU_REG_SAR_CLK_CFG             = 0x90,
    PMU_REG_SAR_CFG_91              = 0x91,
    PMU_REG_SAR_CFG_92              = 0x92,
    PMU_REG_SAR_PU_CFG              = 0x93,
    PMU_REG_EFUSE_CTRL              = 0xB7,
    PMU_REG_EFUSE_DATA_HIGH         = 0xBD,
    PMU_REG_EFUSE_DATA_LOW          = 0xBE,
    PMU_REG_RC_SLEEP_CFG            = 0xD0,
    PMU_REG_PU_VSYS_DIV             = 0xDA,
    PMU_REG_SAR_VREF_CFG_E6         = 0xE6,
    PMU_REG_BOOT_SOURCE             = 0xF6,
    PMU_REG_RCOSC_6M_GOAL_CNT       = 0xF7,
    PMU_REG_RCOSC_6M_CFG            = 0xF8,
    PMU_REG_RCOSC_192M_CALIB        = 0xFA,
    PMU_REG_DBG_RCOSC_6M_CFG        = 0xFC,
    PMU_REG_DBG_RCOSC6M_REF_CNT     = 0xFD,
    PMU_REG_FE                      = 0xFE,
    PMU_REG_RTC_MATCH_MIN_SEC       = 0x10A,
    PMU_REG_RTC_MATCH_HOURS         = 0x10B,
    PMU_REG_RTC_MATCH_DATAS         = 0x10C,
    PMU_REG_RTC_MATCH_YEARS         = 0x10D,
    PMU_REG_RTC_RTC_CFG_10E         = 0x10E,
    PMU_REG_RTC_LOAD_MIN_SEC_EN     = 0x10F,
    PMU_REG_RTC_LOAD_HOURS          = 0x110,
    PMU_REG_RTC_LOAD_DATAS_EN       = 0x111,
    PMU_REG_RTC_LOAD_YEARS          = 0x112,
    PMU_REG_RTC_GET_MIN_SEC         = 0x113,
    PMU_REG_RTC_GET_HOURS           = 0x114,
    PMU_REG_RTC_GET_DATAS           = 0x115,
    PMU_REG_RTC_GET_YEARS           = 0x116,
    PMU_REG_LDO_ANA_CFG             = 0x126,
    PMU_REG_LDO_ANA_VOLT            = 0x128,
    PMU_REG_BUCK_VHPPA_RAMP_CFG     = 0x130,
    PMU_REG_BUCK_VCORE_EN           = 0x131,
    PMU_REG_BUCK_VCORE_CFG          = 0x132,
    PMU_REG_BUCK_VCORE_LP_CFG       = 0x133,
    PMU_REG_BUCK_VCORE_RC_CFG       = 0x134,
    PMU_REG_BUCK_VHPPA_EN           = 0x135,
    PMU_REG_BUCK_VHPPA_LP_CFG       = 0x136,
    PMU_REG_BUCK_BUCK_CFG_138       = 0x138,
    PMU_REG_BUCK_BUCK_CFG_139       = 0x139,
    PMU_REG_BUCK_BUCK_CFG_13A       = 0x13A,
    PMU_REG_EFUSE_CLOCK_CTRL        = 0x147,
    PMU_REG_EFUSE_MSK               = 0x159,
};

// REG_000
#define REVID_SHIFT                         0
#define REVID_MASK                          (0xF << REVID_SHIFT)
#define REVID(n)                            BITFIELD_VAL(REVID, n)
#define CHIPID_SHIFT                        4
#define CHIPID_MASK                         (0xFFF << CHIPID_SHIFT)
#define CHIPID(n)                           BITFIELD_VAL(CHIPID, n)

// REG_001
#define LPO_OFF_CNT_SHIFT                   12
#define LPO_OFF_CNT_MASK                    (0xF << LPO_OFF_CNT_SHIFT)
#define LPO_OFF_CNT(n)                      BITFIELD_VAL(LPO_OFF_CNT, n)
#define POWER_ON_DB_CNT_SHIFT               8
#define POWER_ON_DB_CNT_MASK                (0xF << POWER_ON_DB_CNT_SHIFT)
#define POWER_ON_DB_CNT(n)                  BITFIELD_VAL(POWER_ON_DB_CNT, n)
#define POWER_OFF_CNT_SHIFT                 4
#define POWER_OFF_CNT_MASK                  (0xF << POWER_OFF_CNT_SHIFT)
#define POWER_OFF_CNT(n)                    BITFIELD_VAL(POWER_OFF_CNT, n)
#define CLK_STABLE_CNT_SHIFT                0
#define CLK_STABLE_CNT_MASK                 (0xF << CLK_STABLE_CNT_SHIFT)
#define CLK_STABLE_CNT(n)                   BITFIELD_VAL(CLK_STABLE_CNT, n)

// REG_002
#define REG_PU_VBAT_DIV                     (1 << 15)
#define PU_LPO_DR                           (1 << 14)
#define PU_LPO_REG                          (1 << 13)
#define POWERKEY_WAKEUP_OSC_EN              (1 << 12)
#define RTC_POWER_ON_EN                     (1 << 11)
#define PU_ALL_REG                          (1 << 10)
#define RESERVED_ANA_16                     (1 << 9)

#define DEEPSLEEP_MODE_DIGI_DR              (1 << 7)
#define DEEPSLEEP_MODE_DIGI_REG             (1 << 6)
#define RESERVED_ANA_18_17_SHIFT            4
#define RESERVED_ANA_18_17_MASK             (0x3 << RESERVED_ANA_18_17_SHIFT)
#define RESERVED_ANA_18_17(n)               BITFIELD_VAL(RESERVED_ANA_18_17, n)
#define RESETN_ANA_DR                       (1 << 3)
#define RESETN_ANA_REG                      (1 << 2)
#define RESETN_DIG_DR                       (1 << 1)
#define RESETN_DIG_REG                      (1 << 0)

// REG_003
#define RESERVED_ANA_19                     (1 << 15)
#define PU_LP_BIAS_LDO_DSLEEP               (1 << 14)
#define PU_LP_BIAS_LDO_DR                   (1 << 13)
#define PU_LP_BIAS_LDO_REG                  (1 << 12)
#define PU_BIAS_LDO_DR                      (1 << 11)
#define PU_BIAS_LDO_REG                     (1 << 10)

#define REG_RFPA_3P3_RES_SHIFT              2
#define REG_RFPA_3P3_RES_MASK               (0xF << REG_RFPA_3P3_RES_SHIFT)
#define REG_RFPA_3P3_RES(n)                 BITFIELD_VAL(REG_RFPA_3P3_RES, n)
#define REG_RFPA_3P3                        (1 << 1)
#define REG_RFPA_ON_B                       (1 << 0)















// REG_005
#define REG_PU_LDO_VRTC_RF_DSLEEP           (1 << 15)
#define REG_PU_LDO_VRTC_RF_DR               (1 << 14)
#define REG_PU_LDO_VRTC_RF_REG              (1 << 13)
#define REG_CHARGE_OUT_INTR_MSK             (1 << 12)
#define REG_CHARGE_IN_INTR_MSK              (1 << 11)
#define REG_AC_ON_OUT_EN                    (1 << 10)
#define REG_AC_ON_IN_EN                     (1 << 9)
#define REG_PMU_CHARGE_INTR_EN              (1 << 8)
#define REG_AC_ON_DB_VALUE_SHIFT            0
#define REG_AC_ON_DB_VALUE_MASK             (0xFF << REG_AC_ON_DB_VALUE_SHIFT)
#define REG_AC_ON_DB_VALUE(n)               BITFIELD_VAL(REG_AC_ON_DB_VALUE, n)

// REG_006
#define REG_BYPASS_VBG_RF_BUFFER_DR         (1 << 15)
#define REG_BYPASS_VBG_RF_BUFFER_REG        (1 << 14)
#define PU_VBG_RF_BUFFER_DR                 (1 << 13)
#define PU_VBG_RF_BUFFER_REG                (1 << 12)
#define RESERVED_ANA_21_20_SHIFT            10
#define RESERVED_ANA_21_20_MASK             (0x3 << RESERVED_ANA_21_20_SHIFT)
#define RESERVED_ANA_21_20(n)               BITFIELD_VAL(RESERVED_ANA_21_20, n)
#define REG_LPO_KTRIM_SHIFT                 4
#define REG_LPO_KTRIM_MASK                  (0x3F << REG_LPO_KTRIM_SHIFT)
#define REG_LPO_KTRIM(n)                    BITFIELD_VAL(REG_LPO_KTRIM, n)
#define REG_LPO_ITRIM_SHIFT                 0
#define REG_LPO_ITRIM_MASK                  (0xF << REG_LPO_ITRIM_SHIFT)
#define REG_LPO_ITRIM(n)                    BITFIELD_VAL(REG_LPO_ITRIM, n)

// REG_007
#define REG_PU_ACIN_SENSE                   (1 << 6)
#define REG_TRIM_VBG_AUX_SHIFT              0
#define REG_TRIM_VBG_AUX_MASK               (0x3F << REG_TRIM_VBG_AUX_SHIFT)
#define REG_TRIM_VBG_AUX(n)                 BITFIELD_VAL(REG_TRIM_VBG_AUX, n)



























// REG_009
#define POWER_UP_MOD12_CNT_SHIFT            8
#define POWER_UP_MOD12_CNT_MASK             (0xFF << POWER_UP_MOD12_CNT_SHIFT)
#define POWER_UP_MOD12_CNT(n)               BITFIELD_VAL(POWER_UP_MOD12_CNT, n)
#define POWER_UP_MOD11_CNT_SHIFT            0
#define POWER_UP_MOD11_CNT_MASK             (0xFF << POWER_UP_MOD11_CNT_SHIFT)
#define POWER_UP_MOD11_CNT(n)               BITFIELD_VAL(POWER_UP_MOD11_CNT, n)













// REG_00B
#define POWER_DOWN_DB_CNT_SHIFT             4
#define POWER_DOWN_DB_CNT_MASK              (0xF << POWER_DOWN_DB_CNT_SHIFT)
#define POWER_DOWN_DB_CNT(n)                BITFIELD_VAL(POWER_DOWN_DB_CNT, n)






// REG_00C
#define REG_VCORE_SSTIME_MODE_SHIFT         14
#define REG_VCORE_SSTIME_MODE_MASK          (0x3 << REG_VCORE_SSTIME_MODE_SHIFT)
#define REG_VCORE_SSTIME_MODE(n)            BITFIELD_VAL(REG_VCORE_SSTIME_MODE, n)
#define REG_HW_WDT_COM_RST_TIME_SHIFT       0
#define REG_HW_WDT_COM_RST_TIME_MASK        (0xFFF << REG_HW_WDT_COM_RST_TIME_SHIFT)
#define REG_HW_WDT_COM_RST_TIME(n)          BITFIELD_VAL(REG_HW_WDT_COM_RST_TIME, n)


// REG_00D
#define REG_PU_AVDD25_ANA                   (1 << 1)
#define REG_LDO_VCORE_BYPASS                (1 << 0)

// REG_00E
#define POWER_UP_MOD10_CNT_SHIFT            8
#define POWER_UP_MOD10_CNT_MASK             (0xFF << POWER_UP_MOD10_CNT_SHIFT)
#define POWER_UP_MOD10_CNT(n)               BITFIELD_VAL(POWER_UP_MOD10_CNT, n)
#define POWER_UP_MOD9_CNT_SHIFT             0
#define POWER_UP_MOD9_CNT_MASK              (0xFF << POWER_UP_MOD9_CNT_SHIFT)
#define POWER_UP_MOD9_CNT(n)                BITFIELD_VAL(POWER_UP_MOD9_CNT, n)







// REG_00F
#define REG_LOW_VIO                         (1 << 15)

#define REG_UVLO_SEL_SHIFT                  12
#define REG_UVLO_SEL_MASK                   (0x3 << REG_UVLO_SEL_SHIFT)
#define REG_UVLO_SEL(n)                     BITFIELD_VAL(REG_UVLO_SEL, n)
#define POWER_UP_SOFT_CNT_SHIFT             8
#define POWER_UP_SOFT_CNT_MASK              (0xF << POWER_UP_SOFT_CNT_SHIFT)
#define POWER_UP_SOFT_CNT(n)                BITFIELD_VAL(POWER_UP_SOFT_CNT, n)



// REG_010
#define POWER_UP_MOD2_CNT_SHIFT             8
#define POWER_UP_MOD2_CNT_MASK              (0xFF << POWER_UP_MOD2_CNT_SHIFT)
#define POWER_UP_MOD2_CNT(n)                BITFIELD_VAL(POWER_UP_MOD2_CNT, n)
#define POWER_UP_MOD1_CNT_SHIFT             0
#define POWER_UP_MOD1_CNT_MASK              (0xFF << POWER_UP_MOD1_CNT_SHIFT)
#define POWER_UP_MOD1_CNT(n)                BITFIELD_VAL(POWER_UP_MOD1_CNT, n)

// REG_011
#define POWER_UP_MOD4_CNT_SHIFT             8
#define POWER_UP_MOD4_CNT_MASK              (0xFF << POWER_UP_MOD4_CNT_SHIFT)
#define POWER_UP_MOD4_CNT(n)                BITFIELD_VAL(POWER_UP_MOD4_CNT, n)
#define POWER_UP_MOD3_CNT_SHIFT             0
#define POWER_UP_MOD3_CNT_MASK              (0xFF << POWER_UP_MOD3_CNT_SHIFT)
#define POWER_UP_MOD3_CNT(n)                BITFIELD_VAL(POWER_UP_MOD3_CNT, n)

// REG_012
#define POWER_UP_MOD6_CNT_SHIFT             8
#define POWER_UP_MOD6_CNT_MASK              (0xFF << POWER_UP_MOD6_CNT_SHIFT)
#define POWER_UP_MOD6_CNT(n)                BITFIELD_VAL(POWER_UP_MOD6_CNT, n)
#define POWER_UP_MOD5_CNT_SHIFT             0
#define POWER_UP_MOD5_CNT_MASK              (0xFF << POWER_UP_MOD5_CNT_SHIFT)
#define POWER_UP_MOD5_CNT(n)                BITFIELD_VAL(POWER_UP_MOD5_CNT, n)

// REG_013
#define POWER_UP_MOD8_CNT_SHIFT             8
#define POWER_UP_MOD8_CNT_MASK              (0xFF << POWER_UP_MOD8_CNT_SHIFT)
#define POWER_UP_MOD8_CNT(n)                BITFIELD_VAL(POWER_UP_MOD8_CNT, n)
#define POWER_UP_MOD7_CNT_SHIFT             0
#define POWER_UP_MOD7_CNT_MASK              (0xFF << POWER_UP_MOD7_CNT_SHIFT)
#define POWER_UP_MOD7_CNT(n)                BITFIELD_VAL(POWER_UP_MOD7_CNT, n)

// REG_014
#define DFT_MODE_CORE                       (1 << 15)
#define POWER_UP_MOD13_CNT_SHIFT            0
#define POWER_UP_MOD13_CNT_MASK             (0xFF << POWER_UP_MOD13_CNT_SHIFT)
#define POWER_UP_MOD13_CNT(n)               BITFIELD_VAL(POWER_UP_MOD13_CNT, n)

// REG_015
#define CHAN_EN_REG_SHIFT                   0
#define CHAN_EN_REG_MASK                    (0xFFF << CHAN_EN_REG_SHIFT)
#define CHAN_EN_REG(n)                      BITFIELD_VAL(CHAN_EN_REG, n)

// REG_017
#define REG_SS_VCORE_EN                     (1 << 15)
#define RESERVED_ANA_23                     (1 << 14)
#define SLEEP_ALLOW                         (1 << 13)
#define REG_GPADC_RESETN_DR                 (1 << 11)
#define REG_GPADC_RESETN                    (1 << 10)
#define SAR_PWR_BIT_SHIFT                   8
#define SAR_PWR_BIT_MASK                    (0x3 << SAR_PWR_BIT_SHIFT)
#define SAR_PWR_BIT(n)                      BITFIELD_VAL(SAR_PWR_BIT, n)
#define SAR_OP_IBIT_SHIFT                   5
#define SAR_OP_IBIT_MASK                    (0x7 << SAR_OP_IBIT_SHIFT)
#define SAR_OP_IBIT(n)                      BITFIELD_VAL(SAR_OP_IBIT, n)
#define SAR_THERM_GAIN_SHIFT                3
#define SAR_THERM_GAIN_MASK                 (0x3 << SAR_THERM_GAIN_SHIFT)
#define SAR_THERM_GAIN(n)                   BITFIELD_VAL(SAR_THERM_GAIN, n)
#define SAR_VREF_BIT_SHIFT                  0
#define SAR_VREF_BIT_MASK                   (0x7 << SAR_VREF_BIT_SHIFT)
#define SAR_VREF_BIT(n)                     BITFIELD_VAL(SAR_VREF_BIT, n)

// REG_018
#define SAR_MODE_SEL                        (1 << 15)
#define KEY_DB_DSB                          (1 << 14)
#define KEY_INTERVAL_MODE                   (1 << 13)
#define GPADC_INTERVAL_MODE                 (1 << 12)
#define STATE_RESET                         (1 << 11)
#define SAR_ADC_MODE                        (1 << 10)
#define DELAY_BEFORE_SAMP_SHIFT             8
#define DELAY_BEFORE_SAMP_MASK              (0x3 << DELAY_BEFORE_SAMP_SHIFT)
#define DELAY_BEFORE_SAMP(n)                BITFIELD_VAL(DELAY_BEFORE_SAMP, n)
#define CONV_CLK_INV                        (1 << 7)
#define SAMP_CLK_INV                        (1 << 6)
#define TIME_SAMP_NEG                       (1 << 5)
#define TIME_SAMP_POS                       (1 << 4)
#define SAR_OUT_POLARITY                    (1 << 3)
#define TIMER_SAR_STABLE_SEL_SHIFT          0
#define TIMER_SAR_STABLE_SEL_MASK           (0x7 << TIMER_SAR_STABLE_SEL_SHIFT)
#define TIMER_SAR_STABLE_SEL(n)             BITFIELD_VAL(TIMER_SAR_STABLE_SEL, n)

// REG_019
#define REG_KEY_IN_DR                       (1 << 14)
#define REG_KEY_IN_REG                      (1 << 13)
#define REG_GPADC_EN_DR                     (1 << 12)
#define REG_GPADC_EN_REG_SHIFT              0
#define REG_GPADC_EN_REG_MASK               (0xFFF << REG_GPADC_EN_REG_SHIFT)
#define REG_GPADC_EN_REG(n)                 BITFIELD_VAL(REG_GPADC_EN_REG, n)

// REG_01A
#define REG_PMU_VSEL1_SHIFT                 13
#define REG_PMU_VSEL1_MASK                  (0x7 << REG_PMU_VSEL1_SHIFT)
#define REG_PMU_VSEL1(n)                    BITFIELD_VAL(REG_PMU_VSEL1, n)
#define REG_POWER_SEL_CNT_SHIFT             8
#define REG_POWER_SEL_CNT_MASK              (0x1F << REG_POWER_SEL_CNT_SHIFT)
#define REG_POWER_SEL_CNT(n)                BITFIELD_VAL(REG_POWER_SEL_CNT, n)
#define REG_32K_GATE_EN                     (1 << 7)
#define REG_PWR_SEL                         (1 << 6)
#define CLK_BG_EN                           (1 << 5)
#define CLK_BG_DIV_VALUE_SHIFT              0
#define CLK_BG_DIV_VALUE_MASK               (0x1F << CLK_BG_DIV_VALUE_SHIFT)
#define CLK_BG_DIV_VALUE(n)                 BITFIELD_VAL(CLK_BG_DIV_VALUE, n)

// REG_01B
#define KEY_INTERVAL_SHIFT                  0
#define KEY_INTERVAL_MASK                   (0xFFFF << KEY_INTERVAL_SHIFT)
#define KEY_INTERVAL(n)                     BITFIELD_VAL(KEY_INTERVAL, n)

// REG_01C
#define GPADC_INTERVAL_SHIFT                0
#define GPADC_INTERVAL_MASK                 (0xFFFF << GPADC_INTERVAL_SHIFT)
#define GPADC_INTERVAL(n)                   BITFIELD_VAL(GPADC_INTERVAL, n)


// REG_01D
#define CHIP_ADDR_I2C_SHIFT                 0
#define CHIP_ADDR_I2C_MASK                  (0x7F << CHIP_ADDR_I2C_SHIFT)
#define CHIP_ADDR_I2C(n)                    BITFIELD_VAL(CHIP_ADDR_I2C, n)


// REG_01E
#define RESERVED_ANA_24                     (1 << 15)
#define REG_SAR_REF_MODE_SEL                (1 << 14)
#define REG_CLK_GPADC_EN                    (1 << 13)
#define REG_DR_PU_SAR                       (1 << 12)
#define REG_PU_SAR                          (1 << 11)
#define REG_DR_TSC_SAR_BIT                  (1 << 10)
#define REG_TSC_SAR_BIT_SHIFT               0
#define REG_TSC_SAR_BIT_MASK                (0x3FF << REG_TSC_SAR_BIT_SHIFT)
#define REG_TSC_SAR_BIT(n)                  BITFIELD_VAL(REG_TSC_SAR_BIT, n)

// REG_01F
#define USB_INSERT_INTR_MSK                 (1 << 15)
#define RTC_INT1_MSK                        (1 << 14)
#define RTC_INT0_MSK                        (1 << 13)
#define KEY_ERR1_INTR_MSK                   (1 << 12)
#define KEY_ERR0_INTR_MSK                   (1 << 11)
#define KEY_PRESS_INTR_MSK                  (1 << 10)
#define KEY_RELEASE_INTR_MSK                (1 << 9)
#define SAMPLE_DONE_INTR_MSK                (1 << 8)



// REG_020
#define RTC_INT_EN_1                        (1 << 14)
#define RTC_INT_EN_0                        (1 << 13)
#define KEY_ERR1_INTR_EN                    (1 << 12)
#define KEY_ERR0_INTR_EN                    (1 << 11)
#define KEY_PRESS_INTR_EN                   (1 << 10)
#define KEY_RELEASE_INTR_EN                 (1 << 9)
#define SAMPLE_DONE_INTR_EN                 (1 << 8)


// REG_021
#define RESERVED_ANA_26_25_SHIFT            14
#define RESERVED_ANA_26_25_MASK             (0x3 << RESERVED_ANA_26_25_SHIFT)
#define RESERVED_ANA_26_25(n)               BITFIELD_VAL(RESERVED_ANA_26_25, n)


// REG_022
#define CFG_DIV_RTC_1HZ_SHIFT               0
#define CFG_DIV_RTC_1HZ_MASK                (0xFFFF << CFG_DIV_RTC_1HZ_SHIFT)
#define CFG_DIV_RTC_1HZ(n)                  BITFIELD_VAL(CFG_DIV_RTC_1HZ, n)

// REG_023
#define REG_CLK_GPADC_DIV_VALUE_SHIFT       11
#define REG_CLK_GPADC_DIV_VALUE_MASK        (0x1F << REG_CLK_GPADC_DIV_VALUE_SHIFT)
#define REG_CLK_GPADC_DIV_VALUE(n)          BITFIELD_VAL(REG_CLK_GPADC_DIV_VALUE, n)

// REG_024
#define CHAN_DATA_INTR_EN_SHIFT             0
#define CHAN_DATA_INTR_EN_MASK              (0xFFF << CHAN_DATA_INTR_EN_SHIFT)
#define CHAN_DATA_INTR_EN(n)                BITFIELD_VAL(CHAN_DATA_INTR_EN, n)

// REG_025
#define CHAN_DATA_INTR_MSK_SHIFT            0
#define CHAN_DATA_INTR_MSK_MASK             (0xFFF << CHAN_DATA_INTR_MSK_SHIFT)
#define CHAN_DATA_INTR_MSK(n)               BITFIELD_VAL(CHAN_DATA_INTR_MSK, n)


// REG_029
#define REG_MIC_BIASA_CHANSEL_SHIFT         14
#define REG_MIC_BIASA_CHANSEL_MASK          (0x3 << REG_MIC_BIASA_CHANSEL_SHIFT)
#define REG_MIC_BIASA_CHANSEL(n)            BITFIELD_VAL(REG_MIC_BIASA_CHANSEL, n)
#define REG_MIC_BIASA_EN                    (1 << 13)
#define REG_MIC_BIASA_ENLPF                 (1 << 12)
#define RESERVED_ANA_22                     (1 << 11)
#define REG_MIC_BIASA_VSEL_SHIFT            5
#define REG_MIC_BIASA_VSEL_MASK             (0x3F << REG_MIC_BIASA_VSEL_SHIFT)
#define REG_MIC_BIASA_VSEL(n)               BITFIELD_VAL(REG_MIC_BIASA_VSEL, n)
#define REG_MIC_LDO_RES_SHIFT               1
#define REG_MIC_LDO_RES_MASK                (0xF << REG_MIC_LDO_RES_SHIFT)
#define REG_MIC_LDO_RES(n)                  BITFIELD_VAL(REG_MIC_LDO_RES, n)
#define REG_MIC_LDO_LOOPCTRL                (1 << 0)

// REG_02A
#define REG_MIC_BIASB_CHANSEL_SHIFT         14
#define REG_MIC_BIASB_CHANSEL_MASK          (0x3 << REG_MIC_BIASB_CHANSEL_SHIFT)
#define REG_MIC_BIASB_CHANSEL(n)            BITFIELD_VAL(REG_MIC_BIASB_CHANSEL, n)
#define REG_MIC_BIASB_EN                    (1 << 13)
#define REG_MIC_BIASB_ENLPF                 (1 << 12)

#define REG_MIC_BIASB_VSEL_SHIFT            5
#define REG_MIC_BIASB_VSEL_MASK             (0x3F << REG_MIC_BIASB_VSEL_SHIFT)
#define REG_MIC_BIASB_VSEL(n)               BITFIELD_VAL(REG_MIC_BIASB_VSEL, n)
#define REG_MIC_LDO_EN                      (1 << 4)
#define REG_MIC_LDO_PULLDOWN                (1 << 3)

// REG_02B
#define RESERVED_ANA_27                     (1 << 5)




// REG_02C
#define RESERVED_ANA_28                     (1 << 13)








// REG_02D
#define R_PWM2_TOGGLE_SHIFT                 0
#define R_PWM2_TOGGLE_MASK                  (0xFFFF << R_PWM2_TOGGLE_SHIFT)
#define R_PWM2_TOGGLE(n)                    BITFIELD_VAL(R_PWM2_TOGGLE, n)

// REG_02E
#define REG_PWM2_ST1_SHIFT                  0
#define REG_PWM2_ST1_MASK                   (0xFFFF << REG_PWM2_ST1_SHIFT)
#define REG_PWM2_ST1(n)                     BITFIELD_VAL(REG_PWM2_ST1, n)

// REG_02F
#define SUBCNT_DATA2_SHIFT                  8
#define SUBCNT_DATA2_MASK                   (0xFF << SUBCNT_DATA2_SHIFT)
#define SUBCNT_DATA2(n)                     BITFIELD_VAL(SUBCNT_DATA2, n)
#define TG_SUBCNT_D2_ST_SHIFT               1
#define TG_SUBCNT_D2_ST_MASK                (0x7F << TG_SUBCNT_D2_ST_SHIFT)
#define TG_SUBCNT_D2_ST(n)                  BITFIELD_VAL(TG_SUBCNT_D2_ST, n)
#define REG_LED0_OUT                        (1 << 0)

// REG_030
#define RESERVED_ANA_29                     (1 << 15)
#define RESETN_DB_NUMBER_SHIFT              11
#define RESETN_DB_NUMBER_MASK               (0xF << RESETN_DB_NUMBER_SHIFT)
#define RESETN_DB_NUMBER(n)                 BITFIELD_VAL(RESETN_DB_NUMBER, n)
#define RESETN_DB_EN                        (1 << 10)
#define REG_PWM_CLK_EN                      (1 << 9)
#define REG_CLK_PWM_DIV_SHIFT               4
#define REG_CLK_PWM_DIV_MASK                (0x1F << REG_CLK_PWM_DIV_SHIFT)
#define REG_CLK_PWM_DIV(n)                  BITFIELD_VAL(REG_CLK_PWM_DIV, n)
#define REG_PWM2_BR_EN                      (1 << 3)
#define PWM_SELECT_EN                       (1 << 2)
#define PWM_SELECT_INV                      (1 << 1)
#define LED_GPIO_SELECT                     (1 << 0)


// REG_034
#define RESERVED_ANA_30                     (1 << 15)
#define CLK_32K_COUNT_NUM_SHIFT             11
#define CLK_32K_COUNT_NUM_MASK              (0xF << CLK_32K_COUNT_NUM_SHIFT)
#define CLK_32K_COUNT_NUM(n)                BITFIELD_VAL(CLK_32K_COUNT_NUM, n)

#define DIG_IPTAT_CORE_EN                   (1 << 4)
#define CLK_32K_COUNT_EN                    (1 << 0)



// REG_035
#define REG_DIV_HW_RESET_SHIFT              0
#define REG_DIV_HW_RESET_MASK               (0xFFFF << REG_DIV_HW_RESET_SHIFT)
#define REG_DIV_HW_RESET(n)                 BITFIELD_VAL(REG_DIV_HW_RESET, n)

// REG_036
#define REG_WDT_TIMER_SHIFT                 0
#define REG_WDT_TIMER_MASK                  (0xFFFF << REG_WDT_TIMER_SHIFT)
#define REG_WDT_TIMER(n)                    BITFIELD_VAL(REG_WDT_TIMER, n)

// REG_037
#define RESERVED_ANA_33_31_SHIFT            13
#define RESERVED_ANA_33_31_MASK             (0x7 << RESERVED_ANA_33_31_SHIFT)
#define RESERVED_ANA_33_31(n)               BITFIELD_VAL(RESERVED_ANA_33_31, n)

#define CLK_32K_COUNT_EN_TRIGGER            (1 << 12)
#define CLK_32K_COUNT_CLOCK_EN              (1 << 11)
#define POWERON_DETECT_EN                   (1 << 10)
#define MERGE_INTR                          (1 << 9)
#define REG_WDT_EN                          (1 << 8)
#define REG_WDT_RESET_EN                    (1 << 7)
#define REG_HW_RESET_TIME_SHIFT             1
#define REG_HW_RESET_TIME_MASK              (0x3F << REG_HW_RESET_TIME_SHIFT)
#define REG_HW_RESET_TIME(n)                BITFIELD_VAL(REG_HW_RESET_TIME, n)
#define REG_HW_RESET_EN                     (1 << 0)

// REG_038
#define RESERVED_ANA_15_0_SHIFT             0
#define RESERVED_ANA_15_0_MASK              (0xFFFF << RESERVED_ANA_15_0_SHIFT)
#define RESERVED_ANA_15_0(n)                BITFIELD_VAL(RESERVED_ANA_15_0, n)

// REG_039
#define RESERVED_DIG_15_0_SHIFT             0
#define RESERVED_DIG_15_0_MASK              (0xFFFF << RESERVED_DIG_15_0_SHIFT)
#define RESERVED_DIG_15_0(n)                BITFIELD_VAL(RESERVED_DIG_15_0, n)

// REG_03A
#define R_PWMB_TOGGLE_SHIFT                 0
#define R_PWMB_TOGGLE_MASK                  (0xFFFF << R_PWMB_TOGGLE_SHIFT)
#define R_PWMB_TOGGLE(n)                    BITFIELD_VAL(R_PWMB_TOGGLE, n)

// REG_03B
#define REG_PWMB_ST1_SHIFT                  0
#define REG_PWMB_ST1_MASK                   (0xFFFF << REG_PWMB_ST1_SHIFT)
#define REG_PWMB_ST1(n)                     BITFIELD_VAL(REG_PWMB_ST1, n)

// REG_03C
#define SUBCNT_DATAB_SHIFT                  8
#define SUBCNT_DATAB_MASK                   (0xFF << SUBCNT_DATAB_SHIFT)
#define SUBCNT_DATAB(n)                     BITFIELD_VAL(SUBCNT_DATAB, n)
#define TG_SUBCNT_DB_ST_SHIFT               1
#define TG_SUBCNT_DB_ST_MASK                (0x7F << TG_SUBCNT_DB_ST_SHIFT)
#define TG_SUBCNT_DB_ST(n)                  BITFIELD_VAL(TG_SUBCNT_DB_ST, n)
#define REG_LEDB_OUT                        (1 << 0)

// REG_03D
#define RESERVED_PWMB_SHIFT                 10
#define RESERVED_PWMB_MASK                  (0x3F << RESERVED_PWMB_SHIFT)
#define RESERVED_PWMB(n)                    BITFIELD_VAL(RESERVED_PWMB, n)
#define REG_PWMB_CLK_EN                     (1 << 9)
#define REG_CLK_PWMB_DIV_SHIFT              4
#define REG_CLK_PWMB_DIV_MASK               (0x1F << REG_CLK_PWMB_DIV_SHIFT)
#define REG_CLK_PWMB_DIV(n)                 BITFIELD_VAL(REG_CLK_PWMB_DIV, n)
#define REG_PWMB_BR_EN                      (1 << 3)
#define PWMB_SELECT_EN                      (1 << 2)
#define PWMB_SELECT_INV                     (1 << 1)
#define LEDB_GPIO_SELECT                    (1 << 0)

// REG_03E
#define REG_LED_IO2_IBIT_SHIFT              14
#define REG_LED_IO2_IBIT_MASK               (0x3 << REG_LED_IO2_IBIT_SHIFT)
#define REG_LED_IO2_IBIT(n)                 BITFIELD_VAL(REG_LED_IO2_IBIT, n)
#define REG_LED_IO2_OENB_PRE                (1 << 13)
#define REG_LED_IO2_PDEN                    (1 << 12)
#define REG_LED_IO2_PU                      (1 << 11)
#define REG_LED_IO2_PUEN                    (1 << 10)
#define REG_LED_IO2_SEL_SHIFT               8
#define REG_LED_IO2_SEL_MASK                (0x3 << REG_LED_IO2_SEL_SHIFT)
#define REG_LED_IO2_SEL(n)                  BITFIELD_VAL(REG_LED_IO2_SEL, n)
#define REG_LED_IO2_RX_EN                   (1 << 7)
#define REG_LED_IO2_AIO_EN                  (1 << 6)
#define RESERVED_3E_SHIFT                   0
#define RESERVED_3E_MASK                    (0x3F << RESERVED_3E_SHIFT)
#define RESERVED_3E(n)                      BITFIELD_VAL(RESERVED_3E, n)

// REG_03F
#define AC_OUT_LDO_ON_EN                    (1 << 15)
#define REG_LED_GPIO_PWM_SEL                (1 << 14)
#define REG_LED_DBG_SEL_SHIFT               0
#define REG_LED_DBG_SEL_MASK                (0x7 << REG_LED_DBG_SEL_SHIFT)
#define REG_LED_DBG_SEL(n)                  BITFIELD_VAL(REG_LED_DBG_SEL, n)






// REG_040
#define RESERVED_40                         (1 << 15)
#define RTC_INTR_TMP_MERGED_MSK             (1 << 14)
#define GPADC_INTR_MERGED_MSK               (1 << 13)
#define CHARGE_INTR_MERGED_MSK              (1 << 12)












// REG_041
#define USB_INTR_MERGED_MSK                 (1 << 15)
#define POWER_ON_INTR_MERGED_MSK            (1 << 14)
#define PMU_GPIO_INTR_MSKED1_MERGED_MSK     (1 << 13)
#define PMU_GPIO_INTR_MSKED2_MERGED_MSK     (1 << 12)
#define WDT_INTR_MSKED_MERGED_MSK           (1 << 11)









// REG_042
#define RESERVED_42_SHIFT                   10
#define RESERVED_42_MASK                    (0x3F << RESERVED_42_SHIFT)
#define RESERVED_42(n)                      BITFIELD_VAL(RESERVED_42, n)










// REG_043
#define REG_LOW_LEVEL_INTR_SEL1             (1 << 15)
#define REG_WDT_INTR_EN                     (1 << 14)
#define REG_WDT_INTR_MSK                    (1 << 13)


// REG_044
#define CLK_32K_SEL_SHIFT                   6
#define CLK_32K_SEL_MASK                    (0x7 << CLK_32K_SEL_SHIFT)
#define CLK_32K_SEL(n)                      BITFIELD_VAL(CLK_32K_SEL, n)
#define RESETN_RF_DR                        (1 << 5)
#define RESETN_RF_REG                       (1 << 4)
#define RESETN_MOD1_CNT_SHIFT               0
#define RESETN_MOD1_CNT_MASK                (0xF << RESETN_MOD1_CNT_SHIFT)
#define RESETN_MOD1_CNT(n)                  BITFIELD_VAL(RESETN_MOD1_CNT, n)


// REG_045
#define REG_EDGE_INTR_SEL2                  (1 << 14)
#define REG_POS_INTR_SEL2                   (1 << 13)
#define REG_EDGE_INTR_SEL1                  (1 << 12)
#define REG_POS_INTR_SEL1                   (1 << 11)
#define REG_CLK_32K_GATE_SHIFT              0
#define REG_CLK_32K_GATE_MASK               (0x7 << REG_CLK_32K_GATE_SHIFT)
#define REG_CLK_32K_GATE(n)                 BITFIELD_VAL(REG_CLK_32K_GATE, n)





// REG_046
#define REG_GPIO_I_SEL                      (1 << 15)
#define PMU_DB_BYPASS1                      (1 << 14)
#define PMU_DB_TARGET1_SHIFT                6
#define PMU_DB_TARGET1_MASK                 (0xFF << PMU_DB_TARGET1_SHIFT)
#define PMU_DB_TARGET1(n)                   BITFIELD_VAL(PMU_DB_TARGET1, n)
#define REG_PMU_UART_DR1                    (1 << 5)
#define REG_PMU_UART_TX1                    (1 << 4)
#define REG_PMU_UART_OENB1                  (1 << 3)
#define REG_UART_LEDA_SEL                   (1 << 2)
#define REG_PMU_GPIO_INTR_MSK1              (1 << 1)
#define REG_PMU_GPIO_INTR_EN1               (1 << 0)

// REG_047
#define REG_LOW_LEVEL_INTR_SEL2             (1 << 15)
#define PMU_DB_BYPASS2                      (1 << 14)
#define PMU_DB_TARGET2_SHIFT                6
#define PMU_DB_TARGET2_MASK                 (0xFF << PMU_DB_TARGET2_SHIFT)
#define PMU_DB_TARGET2(n)                   BITFIELD_VAL(PMU_DB_TARGET2, n)
#define REG_PMU_UART_DR2                    (1 << 5)
#define REG_PMU_UART_TX2                    (1 << 4)
#define REG_PMU_UART_OENB2                  (1 << 3)
#define REG_UART_LEDB_SEL                   (1 << 2)
#define REG_PMU_GPIO_INTR_MSK2              (1 << 1)
#define REG_PMU_GPIO_INTR_EN2               (1 << 0)

// REG_048
#define PMU_GPIO_INTR_MSKED1                (1 << 15)
#define PMU_GPIO_INTR_MSKED2                (1 << 14)
#define LED_IO1_IN_DB                       (1 << 13)
#define LED_IO2_IN_DB                       (1 << 12)
#define WDT_INTR_MSKED                      (1 << 11)
#define POWER_DOWN_INTR_MSKED               (1 << 10)


#define BUCK_BURST_MODE_OUT                 (1 << 7)
#define BUCK_I_REVERSE_OUT                  (1 << 6)

#define REG_PMU_GPIO_INTR_CLR1              (1 << 15)
#define REG_PMU_GPIO_INTR_CLR2              (1 << 14)
#define REG_WDT_INTR_CLR                    (1 << 13)
#define REG_POWER_DOWN_INTR_CLR             (1 << 10)





// REG_04B
#define DATA_CHAN_MSB_SHIFT                 0
#define DATA_CHAN_MSB_MASK                  (0xFFF << DATA_CHAN_MSB_SHIFT)
#define DATA_CHAN_MSB(n)                    BITFIELD_VAL(DATA_CHAN_MSB, n)

// REG_04C
#define REG_VCORE_ON_DELAY_DR               (1 << 15)
#define REG_VCORE_ON_DELAY                  (1 << 14)
#define CHAN_DATA_VALID_INTR_SHIFT          0
#define CHAN_DATA_VALID_INTR_MASK           (0xFFF << CHAN_DATA_VALID_INTR_SHIFT)
#define CHAN_DATA_VALID_INTR(n)             BITFIELD_VAL(CHAN_DATA_VALID_INTR, n)










// REG_04D
#define REG_WDT_TIMER_INTR_SHIFT            0
#define REG_WDT_TIMER_INTR_MASK             (0xFFFF << REG_WDT_TIMER_INTR_SHIFT)
#define REG_WDT_TIMER_INTR(n)               BITFIELD_VAL(REG_WDT_TIMER_INTR, n)

// REG_04E
#define CLK_32K_COUNTER_26M_SHIFT           0
#define CLK_32K_COUNTER_26M_MASK            (0x7FFF << CLK_32K_COUNTER_26M_SHIFT)
#define CLK_32K_COUNTER_26M(n)              BITFIELD_VAL(CLK_32K_COUNTER_26M, n)
#define CLK_32K_COUNTER_26M_READY           (1 << 15)


// REG_04F
#define GPADC_START                         (1 << 5)
#define KEY_START                           (1 << 4)
#define AC_ON_EN                            (1 << 2)
#define HARDWARE_POWER_OFF_EN               (1 << 1)
#define SOFT_POWER_OFF                      (1 << 0)

// REG_050
#define USBINSERT_INTR2CPU                  (1 << 15)
#define RTC_INT_1                           (1 << 14)
#define RTC_INT_0                           (1 << 13)
#define KEY_ERR1_INTR                       (1 << 12)
#define KEY_ERR0_INTR                       (1 << 11)
#define KEY_PRESS_INTR                      (1 << 10)
#define KEY_RELEASE_INTR                    (1 << 9)
#define SAMPLE_PERIOD_DONE_INTR             (1 << 8)


// REG_051
#define USB_INSERT_INTR_MSKED               (1 << 15)
#define RTC_INT1_MSKED                      (1 << 14)
#define RTC_INT0_MSKED                      (1 << 13)
#define KEY_ERR1_INTR_MSKED                 (1 << 12)
#define KEY_ERR0_INTR_MSKED                 (1 << 11)
#define KEY_PRESS_INTR_MSKED                (1 << 10)
#define KEY_RELEASE_INTR_MSKED              (1 << 9)
#define SAMPLE_DONE_INTR_MSKED              (1 << 8)



#define RTC_INT_CLR_1                       (1 << 14)
#define RTC_INT_CLR_0                       (1 << 13)
#define KEY_ERR1_INTR_CLR                   (1 << 12)
#define KEY_ERR0_INTR_CLR                   (1 << 11)
#define KEY_PRESS_INTR_CLR                  (1 << 10)
#define KEY_RELEASE_INTR_CLR                (1 << 9)
#define SAMPLE_DONE_INTR_CLR                (1 << 8)


// REG_052
#define WDT_LOAD_INTR                       (1 << 9)
#define PMU_GPIO_INTR2                      (1 << 8)
#define PMU_GPIO_INTR1                      (1 << 7)
#define R_WDT_INTR                          (1 << 6)
#define USB_DTESTO_TX                       (1 << 5)
#define VCORE_ON_DELAY                      (1 << 4)

#define WDT_LOAD                            (1 << 2)
#define CORE_GPIO_OUT1                      (1 << 1)
#define POWER_ON                            (1 << 0)

// REG_053
#define CHAN_DATA_INTR_MSKED_SHIFT          0
#define CHAN_DATA_INTR_MSKED_MASK           (0xFFF << CHAN_DATA_INTR_MSKED_SHIFT)
#define CHAN_DATA_INTR_MSKED(n)             BITFIELD_VAL(CHAN_DATA_INTR_MSKED, n)
#define CHAN_DATA_INTR_CLR_SHIFT            0
#define CHAN_DATA_INTR_CLR_MASK             (0xFFF << CHAN_DATA_INTR_CLR_SHIFT)
#define CHAN_DATA_INTR_CLR(n)               BITFIELD_VAL(CHAN_DATA_INTR_CLR, n)



// REG_056
#define DATA_CHAN0_SHIFT                    0
#define DATA_CHAN0_MASK                     (0xFFFF << DATA_CHAN0_SHIFT)
#define DATA_CHAN0(n)                       BITFIELD_VAL(DATA_CHAN0, n)

// REG_057
#define DATA_CHAN1_SHIFT                    0
#define DATA_CHAN1_MASK                     (0xFFFF << DATA_CHAN1_SHIFT)
#define DATA_CHAN1(n)                       BITFIELD_VAL(DATA_CHAN1, n)

// REG_058
#define DATA_CHAN2_SHIFT                    0
#define DATA_CHAN2_MASK                     (0xFFFF << DATA_CHAN2_SHIFT)
#define DATA_CHAN2(n)                       BITFIELD_VAL(DATA_CHAN2, n)

// REG_059
#define DATA_CHAN3_SHIFT                    0
#define DATA_CHAN3_MASK                     (0xFFFF << DATA_CHAN3_SHIFT)
#define DATA_CHAN3(n)                       BITFIELD_VAL(DATA_CHAN3, n)

// REG_05A
#define DATA_CHAN4_SHIFT                    0
#define DATA_CHAN4_MASK                     (0xFFFF << DATA_CHAN4_SHIFT)
#define DATA_CHAN4(n)                       BITFIELD_VAL(DATA_CHAN4, n)

// REG_05B
#define DATA_CHAN5_SHIFT                    0
#define DATA_CHAN5_MASK                     (0xFFFF << DATA_CHAN5_SHIFT)
#define DATA_CHAN5(n)                       BITFIELD_VAL(DATA_CHAN5, n)

// REG_05C
#define DATA_CHAN6_SHIFT                    0
#define DATA_CHAN6_MASK                     (0xFFFF << DATA_CHAN6_SHIFT)
#define DATA_CHAN6(n)                       BITFIELD_VAL(DATA_CHAN6, n)

// REG_05D
#define DATA_CHAN7_SHIFT                    0
#define DATA_CHAN7_MASK                     (0xFFFF << DATA_CHAN7_SHIFT)
#define DATA_CHAN7(n)                       BITFIELD_VAL(DATA_CHAN7, n)

// REG_05E
#define POWER_ON_RELEASE                    (1 << 15)
#define REG_POWER_ON_RELEASE_CLR            (1 << 15)
#define POWER_ON_PRESS                      (1 << 14)
#define REG_POWER_ON_PRESS_CLR              (1 << 14)




#define DIG_PU_VRTC_RF                      (1 << 7)
#define DIG_PU_BIAS_LDO                     (1 << 6)
#define DIG_PU_LP_BIAS_LDO                  (1 << 5)
#define INTR_MSKED_CHARGE_SHIFT             3
#define INTR_MSKED_CHARGE_MASK              (0x3 << INTR_MSKED_CHARGE_SHIFT)
#define INTR_MSKED_CHARGE(n)                BITFIELD_VAL(INTR_MSKED_CHARGE, n)
#define AC_ON                               (1 << 2)
#define AC_ON_DET_OUT                       (1 << 1)
#define REG_CHARGE_OUT_INTR_CLR             (1 << 1)
#define AC_ON_DET_IN                        (1 << 0)
#define REG_CHARGE_IN_INTR_CLR              (1 << 0)

// REG_05F
#define RC_SLEEP_MODE                       (1 << 15)
#define DIG_PU_SAR                          (1 << 14)
#define DIG_BUCK_EN_SIMO                    (1 << 13)




#define DIG_LDO_VANA_EN                     (1 << 8)

#define DEEPSLEEP_MODE                      (1 << 6)
#define UVLO_LV                             (1 << 5)
#define DIG_PU_LPO                          (1 << 4)

// #define PMU_LDO_ON                          (1 << 0)

// REG_060
#define REG_WD_RESETN_INFOR_SHIFT           0
#define REG_WD_RESETN_INFOR_MASK            (0xFFFF << REG_WD_RESETN_INFOR_SHIFT)
#define REG_WD_RESETN_INFOR(n)              BITFIELD_VAL(REG_WD_RESETN_INFOR, n)

// REG_061
#define REG_COMMAND_RST_DIS                 (1 << 15)
#define REG_NOT_RESET_SHIFT                 1
#define REG_NOT_RESET_MASK                  (0x3FFF << REG_NOT_RESET_SHIFT)
#define REG_NOT_RESET(n)                    BITFIELD_VAL(REG_NOT_RESET, n)
#define REG_NOT_RESET_BIT0                  (1 << 0)

#define REG_NOT_RESET_CHIP_PWR_ON           REG_NOT_RESET_BIT0

// REG_062
#define RESETN_A7PLL_REG                    (1 << 11)
#define RESETN_A7PLL_DR                     (1 << 10)
#define RESETN_PSRAM1_REG                   (1 << 9)
#define RESETN_PSRAM1_DR                    (1 << 8)
#define RESETN_PSRAM0_REG                   (1 << 7)
#define RESETN_PSRAM0_DR                    (1 << 6)
#define RESETN_PSRAMPLL_REG                 (1 << 5)
#define RESETN_PSRAMPLL_DR                  (1 << 4)
#define RESETN_BT_REG                       (1 << 3)
#define RESETN_BT_DR                        (1 << 2)
#define RESETN_WIFI_REG                     (1 << 1)
#define RESETN_WIFI_DR                      (1 << 0)





// REG_066
#define REG_COMMAND_RST_BIT_WIDTH_TOLERANCE_SHIFT 8
#define REG_COMMAND_RST_BIT_WIDTH_TOLERANCE_MASK (0xFF << REG_COMMAND_RST_BIT_WIDTH_TOLERANCE_SHIFT)
#define REG_COMMAND_RST_BIT_WIDTH_TOLERANCE(n) BITFIELD_VAL(REG_COMMAND_RST_BIT_WIDTH_TOLERANCE, n)



// REG_067
#define REG_MIC_BIASB_PULLDOWN              (1 << 2)
#define REG_MIC_BIASA_PULLDOWN              (1 << 1)
#define REG_MIC_LP_ENABLE                   (1 << 0)


// REG_068
#define REG_2M_RAMP_GATE_MSK_SHIFT          7
#define REG_2M_RAMP_GATE_MSK_MASK           (0x3 << REG_2M_RAMP_GATE_MSK_SHIFT)
#define REG_2M_RAMP_GATE_MSK(n)             BITFIELD_VAL(REG_2M_RAMP_GATE_MSK, n)
#define REG_RAMP_MODE_SEL                   (1 << 6)
#define REG_RAMP_FASTER_SEL_RC              (1 << 5)
#define REG_2M_OPEN_CNT_SHIFT               0
#define REG_2M_OPEN_CNT_MASK                (0x1F << REG_2M_OPEN_CNT_SHIFT)
#define REG_2M_OPEN_CNT(n)                  BITFIELD_VAL(REG_2M_OPEN_CNT, n)















// REG_06A
#define REG_BIAS_SEL_LDO                    (1 << 7)
#define LP_MODE_RTC_DR                      (1 << 6)
#define LP_MODE_RTC_REG                     (1 << 5)



// REG_06C
#define REG_SAR_DVDD_VSEL_SHIFT             8
#define REG_SAR_DVDD_VSEL_MASK              (0x7 << REG_SAR_DVDD_VSEL_SHIFT)
#define REG_SAR_DVDD_VSEL(n)                BITFIELD_VAL(REG_SAR_DVDD_VSEL, n)
#define REG_SAR_IBSEL_10U_PIN_SHIFT         6
#define REG_SAR_IBSEL_10U_PIN_MASK          (0x3 << REG_SAR_IBSEL_10U_PIN_SHIFT)
#define REG_SAR_IBSEL_10U_PIN(n)            BITFIELD_VAL(REG_SAR_IBSEL_10U_PIN, n)
#define REG_SAR_IBSEL_1U_NIN_SHIFT          4
#define REG_SAR_IBSEL_1U_NIN_MASK           (0x3 << REG_SAR_IBSEL_1U_NIN_SHIFT)
#define REG_SAR_IBSEL_1U_NIN(n)             BITFIELD_VAL(REG_SAR_IBSEL_1U_NIN, n)
#define REG_SAR_IBSEL_0P5U_NIN_SHIFT        2
#define REG_SAR_IBSEL_0P5U_NIN_MASK         (0x3 << REG_SAR_IBSEL_0P5U_NIN_SHIFT)
#define REG_SAR_IBSEL_0P5U_NIN(n)           BITFIELD_VAL(REG_SAR_IBSEL_0P5U_NIN, n)
#define REG_SAR_IBSEL_0P5U_PIN_SHIFT        0
#define REG_SAR_IBSEL_0P5U_PIN_MASK         (0x3 << REG_SAR_IBSEL_0P5U_PIN_SHIFT)
#define REG_SAR_IBSEL_0P5U_PIN(n)           BITFIELD_VAL(REG_SAR_IBSEL_0P5U_PIN, n)



// REG_06D
#define REG_SAR_RESULT_SEL                  (1 << 15)
#define REG_SAR_ADC_ON                      (1 << 14)
#define SAR_BIT00_WEIGHT_SHIFT              0
#define SAR_BIT00_WEIGHT_MASK               (0x3FFF << SAR_BIT00_WEIGHT_SHIFT)
#define SAR_BIT00_WEIGHT(n)                 BITFIELD_VAL(SAR_BIT00_WEIGHT, n)

// REG_06E
#define SAR_BIT01_WEIGHT_SHIFT              0
#define SAR_BIT01_WEIGHT_MASK               (0x3FFF << SAR_BIT01_WEIGHT_SHIFT)
#define SAR_BIT01_WEIGHT(n)                 BITFIELD_VAL(SAR_BIT01_WEIGHT, n)

// REG_06F
#define SAR_BIT02_WEIGHT_SHIFT              0
#define SAR_BIT02_WEIGHT_MASK               (0x3FFF << SAR_BIT02_WEIGHT_SHIFT)
#define SAR_BIT02_WEIGHT(n)                 BITFIELD_VAL(SAR_BIT02_WEIGHT, n)

// REG_070
#define SAR_BIT03_WEIGHT_SHIFT              0
#define SAR_BIT03_WEIGHT_MASK               (0x3FFF << SAR_BIT03_WEIGHT_SHIFT)
#define SAR_BIT03_WEIGHT(n)                 BITFIELD_VAL(SAR_BIT03_WEIGHT, n)

// REG_071
#define SAR_BIT04_WEIGHT_SHIFT              0
#define SAR_BIT04_WEIGHT_MASK               (0x3FFF << SAR_BIT04_WEIGHT_SHIFT)
#define SAR_BIT04_WEIGHT(n)                 BITFIELD_VAL(SAR_BIT04_WEIGHT, n)

// REG_072
#define SAR_P_BIT05_WEIGHT_SHIFT            0
#define SAR_P_BIT05_WEIGHT_MASK             (0x3FFF << SAR_P_BIT05_WEIGHT_SHIFT)
#define SAR_P_BIT05_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT05_WEIGHT, n)

// REG_073
#define SAR_P_BIT06_WEIGHT_SHIFT            0
#define SAR_P_BIT06_WEIGHT_MASK             (0x3FFF << SAR_P_BIT06_WEIGHT_SHIFT)
#define SAR_P_BIT06_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT06_WEIGHT, n)

// REG_074
#define SAR_P_BIT07_WEIGHT_SHIFT            0
#define SAR_P_BIT07_WEIGHT_MASK             (0x3FFF << SAR_P_BIT07_WEIGHT_SHIFT)
#define SAR_P_BIT07_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT07_WEIGHT, n)

// REG_075
#define SAR_P_BIT08_WEIGHT_SHIFT            0
#define SAR_P_BIT08_WEIGHT_MASK             (0x3FFF << SAR_P_BIT08_WEIGHT_SHIFT)
#define SAR_P_BIT08_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT08_WEIGHT, n)

// REG_076
#define SAR_P_BIT09_WEIGHT_SHIFT            0
#define SAR_P_BIT09_WEIGHT_MASK             (0x3FFF << SAR_P_BIT09_WEIGHT_SHIFT)
#define SAR_P_BIT09_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT09_WEIGHT, n)

// REG_077
#define SAR_P_BIT10_WEIGHT_SHIFT            0
#define SAR_P_BIT10_WEIGHT_MASK             (0x3FFF << SAR_P_BIT10_WEIGHT_SHIFT)
#define SAR_P_BIT10_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT10_WEIGHT, n)

// REG_078
#define SAR_P_BIT11_WEIGHT_SHIFT            0
#define SAR_P_BIT11_WEIGHT_MASK             (0x3FFF << SAR_P_BIT11_WEIGHT_SHIFT)
#define SAR_P_BIT11_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT11_WEIGHT, n)

// REG_079
#define SAR_P_BIT12_WEIGHT_SHIFT            0
#define SAR_P_BIT12_WEIGHT_MASK             (0x3FFF << SAR_P_BIT12_WEIGHT_SHIFT)
#define SAR_P_BIT12_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT12_WEIGHT, n)

// REG_07A
#define SAR_P_BIT13_WEIGHT_SHIFT            0
#define SAR_P_BIT13_WEIGHT_MASK             (0x3FFF << SAR_P_BIT13_WEIGHT_SHIFT)
#define SAR_P_BIT13_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT13_WEIGHT, n)

// REG_07B
#define SAR_P_BIT14_WEIGHT_SHIFT            0
#define SAR_P_BIT14_WEIGHT_MASK             (0x3FFF << SAR_P_BIT14_WEIGHT_SHIFT)
#define SAR_P_BIT14_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT14_WEIGHT, n)

// REG_07C
#define SAR_P_BIT15_WEIGHT_SHIFT            0
#define SAR_P_BIT15_WEIGHT_MASK             (0x3FFF << SAR_P_BIT15_WEIGHT_SHIFT)
#define SAR_P_BIT15_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT15_WEIGHT, n)

// REG_07D
#define SAR_P_BIT16_WEIGHT_SHIFT            0
#define SAR_P_BIT16_WEIGHT_MASK             (0x3FFF << SAR_P_BIT16_WEIGHT_SHIFT)
#define SAR_P_BIT16_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT16_WEIGHT, n)

// REG_07E
#define SAR_P_BIT17_WEIGHT_SHIFT            0
#define SAR_P_BIT17_WEIGHT_MASK             (0x3FFF << SAR_P_BIT17_WEIGHT_SHIFT)
#define SAR_P_BIT17_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT17_WEIGHT, n)

// REG_07F
#define SAR_N_BIT05_WEIGHT_SHIFT            0
#define SAR_N_BIT05_WEIGHT_MASK             (0x3FFF << SAR_N_BIT05_WEIGHT_SHIFT)
#define SAR_N_BIT05_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT05_WEIGHT, n)

// REG_080
#define SAR_N_BIT06_WEIGHT_SHIFT            0
#define SAR_N_BIT06_WEIGHT_MASK             (0x3FFF << SAR_N_BIT06_WEIGHT_SHIFT)
#define SAR_N_BIT06_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT06_WEIGHT, n)

// REG_081
#define SAR_N_BIT07_WEIGHT_SHIFT            0
#define SAR_N_BIT07_WEIGHT_MASK             (0x3FFF << SAR_N_BIT07_WEIGHT_SHIFT)
#define SAR_N_BIT07_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT07_WEIGHT, n)

// REG_082
#define SAR_N_BIT08_WEIGHT_SHIFT            0
#define SAR_N_BIT08_WEIGHT_MASK             (0x3FFF << SAR_N_BIT08_WEIGHT_SHIFT)
#define SAR_N_BIT08_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT08_WEIGHT, n)

// REG_083
#define SAR_N_BIT09_WEIGHT_SHIFT            0
#define SAR_N_BIT09_WEIGHT_MASK             (0x3FFF << SAR_N_BIT09_WEIGHT_SHIFT)
#define SAR_N_BIT09_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT09_WEIGHT, n)

// REG_084
#define SAR_N_BIT10_WEIGHT_SHIFT            0
#define SAR_N_BIT10_WEIGHT_MASK             (0x3FFF << SAR_N_BIT10_WEIGHT_SHIFT)
#define SAR_N_BIT10_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT10_WEIGHT, n)

// REG_085
#define SAR_N_BIT11_WEIGHT_SHIFT            0
#define SAR_N_BIT11_WEIGHT_MASK             (0x3FFF << SAR_N_BIT11_WEIGHT_SHIFT)
#define SAR_N_BIT11_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT11_WEIGHT, n)

// REG_086
#define SAR_N_BIT12_WEIGHT_SHIFT            0
#define SAR_N_BIT12_WEIGHT_MASK             (0x3FFF << SAR_N_BIT12_WEIGHT_SHIFT)
#define SAR_N_BIT12_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT12_WEIGHT, n)

// REG_087
#define SAR_N_BIT13_WEIGHT_SHIFT            0
#define SAR_N_BIT13_WEIGHT_MASK             (0x3FFF << SAR_N_BIT13_WEIGHT_SHIFT)
#define SAR_N_BIT13_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT13_WEIGHT, n)

// REG_088
#define SAR_N_BIT14_WEIGHT_SHIFT            0
#define SAR_N_BIT14_WEIGHT_MASK             (0x3FFF << SAR_N_BIT14_WEIGHT_SHIFT)
#define SAR_N_BIT14_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT14_WEIGHT, n)

// REG_089
#define SAR_N_BIT15_WEIGHT_SHIFT            0
#define SAR_N_BIT15_WEIGHT_MASK             (0x3FFF << SAR_N_BIT15_WEIGHT_SHIFT)
#define SAR_N_BIT15_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT15_WEIGHT, n)

// REG_08A
#define SAR_N_BIT16_WEIGHT_SHIFT            0
#define SAR_N_BIT16_WEIGHT_MASK             (0x3FFF << SAR_N_BIT16_WEIGHT_SHIFT)
#define SAR_N_BIT16_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT16_WEIGHT, n)

// REG_08B
#define SAR_N_BIT17_WEIGHT_SHIFT            0
#define SAR_N_BIT17_WEIGHT_MASK             (0x3FFF << SAR_N_BIT17_WEIGHT_SHIFT)
#define SAR_N_BIT17_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT17_WEIGHT, n)

// REG_08C
#define CLK_SARADC_CODEC_INV                (1 << 14)
#define REG_SAR_ADC_OFFSET_DR               (1 << 13)
#define REG_SAR_OFFSET_DR                   (1 << 12)
#define REG_SAR_WEIGHT_DR                   (1 << 11)
#define REG_SAR_CALI_LSB_SCRN               (1 << 10)
#define REG_SAR_CALI_CNT_SHIFT              6
#define REG_SAR_CALI_CNT_MASK               (0xF << REG_SAR_CALI_CNT_SHIFT)
#define REG_SAR_CALI_CNT(n)                 BITFIELD_VAL(REG_SAR_CALI_CNT, n)
#define REG_SAR_CALI                        (1 << 5)
#define REG_SAR_INIT_CALI_BIT_SHIFT         0
#define REG_SAR_INIT_CALI_BIT_MASK          (0x1F << REG_SAR_INIT_CALI_BIT_SHIFT)
#define REG_SAR_INIT_CALI_BIT(n)            BITFIELD_VAL(REG_SAR_INIT_CALI_BIT, n)

// REG_08D
#define REG_SAR_SAMPLE_IBIT_SHIFT           10
#define REG_SAR_SAMPLE_IBIT_MASK            (0x7 << REG_SAR_SAMPLE_IBIT_SHIFT)
#define REG_SAR_SAMPLE_IBIT(n)              BITFIELD_VAL(REG_SAR_SAMPLE_IBIT, n)
#define REG_SAR_OFFSET_N_SHIFT              5
#define REG_SAR_OFFSET_N_MASK               (0x1F << REG_SAR_OFFSET_N_SHIFT)
#define REG_SAR_OFFSET_N(n)                 BITFIELD_VAL(REG_SAR_OFFSET_N, n)
#define REG_SAR_OFFSET_P_SHIFT              0
#define REG_SAR_OFFSET_P_MASK               (0x1F << REG_SAR_OFFSET_P_SHIFT)
#define REG_SAR_OFFSET_P(n)                 BITFIELD_VAL(REG_SAR_OFFSET_P, n)

// REG_08E
#define REG_SAR_ADC_OFFSET_SHIFT            0
#define REG_SAR_ADC_OFFSET_MASK             (0xFFFF << REG_SAR_ADC_OFFSET_SHIFT)
#define REG_SAR_ADC_OFFSET(n)               BITFIELD_VAL(REG_SAR_ADC_OFFSET, n)

// REG_08F
#define REG_SAR_VCM_CORE_SEL_SHIFT          11
#define REG_SAR_VCM_CORE_SEL_MASK           (0x7 << REG_SAR_VCM_CORE_SEL_SHIFT)
#define REG_SAR_VCM_CORE_SEL(n)             BITFIELD_VAL(REG_SAR_VCM_CORE_SEL, n)
#define REG_SAR_DIFF_EN_IN_REG              (1 << 10)
#define REG_SAR_DIFF_EN_IN_DR               (1 << 9)
#define REG_SAR_SE2DIFF_EN                  (1 << 8)
#define REG_SAR_BUF_EN                      (1 << 7)
#define SAR_CONV_DONE_INV                   (1 << 6)
#define SAR_VOUT_CALIB_INV                  (1 << 5)
#define REG_SAR_CLK_OUT_SEL                 (1 << 4)
#define REG_SAR_OFFSET_CALI_CNT_SHIFT       1
#define REG_SAR_OFFSET_CALI_CNT_MASK        (0x7 << REG_SAR_OFFSET_CALI_CNT_SHIFT)
#define REG_SAR_OFFSET_CALI_CNT(n)          BITFIELD_VAL(REG_SAR_OFFSET_CALI_CNT, n)
#define REG_SAR_OFFSET_CALI                 (1 << 0)

// REG_090
#define REG_SAR_DELAY_CMP                   (1 << 15)
#define REG_SAR_INPUT_BUF_EN                (1 << 14)
#define REG_SAR_EN_PREAMP                   (1 << 13)
#define REG_SAR_DELAY_BIT_SHIFT             11
#define REG_SAR_DELAY_BIT_MASK              (0x3 << REG_SAR_DELAY_BIT_SHIFT)
#define REG_SAR_DELAY_BIT(n)                BITFIELD_VAL(REG_SAR_DELAY_BIT, n)
#define REG_SAR_CLK_MODE                    (1 << 10)
#define REG_SAR_CLK_TRIM_SHIFT              8
#define REG_SAR_CLK_TRIM_MASK               (0x3 << REG_SAR_CLK_TRIM_SHIFT)
#define REG_SAR_CLK_TRIM(n)                 BITFIELD_VAL(REG_SAR_CLK_TRIM, n)
#define REG_SAR_CH_SEL_IN_SHIFT             0
#define REG_SAR_CH_SEL_IN_MASK              (0xFF << REG_SAR_CH_SEL_IN_SHIFT)
#define REG_SAR_CH_SEL_IN(n)                BITFIELD_VAL(REG_SAR_CH_SEL_IN, n)

// REG_091
#define REG_SAR_DONE_SEL                    (1 << 13)
#define REG_SAR_DONE_DLY_SEL                (1 << 12)
#define REG_SAR_HIGH_PW_CMP                 (1 << 11)
#define REG_SAR_HIGH_DVDD_EN                (1 << 10)
#define REG_SAR_VREG_OUTCAP_MODE            (1 << 9)
#define REG_SAR_VREG_IBIT_SHIFT             7
#define REG_SAR_VREG_IBIT_MASK              (0x3 << REG_SAR_VREG_IBIT_SHIFT)
#define REG_SAR_VREG_IBIT(n)                BITFIELD_VAL(REG_SAR_VREG_IBIT, n)
#define REG_SAR_THERM_GAIN_IN_SHIFT         5
#define REG_SAR_THERM_GAIN_IN_MASK          (0x3 << REG_SAR_THERM_GAIN_IN_SHIFT)
#define REG_SAR_THERM_GAIN_IN(n)            BITFIELD_VAL(REG_SAR_THERM_GAIN_IN, n)
#define REG_SAR_SE_MODE                     (1 << 4)
#define REG_SAR_PU_NIN_BIAS                 (1 << 3)
#define REG_SAR_PU_NIN_BIAS_DR              (1 << 2)
#define REG_SAR_PU_PIN_BIAS                 (1 << 1)
#define REG_SAR_PU_PIN_BIAS_DR              (1 << 0)

// REG_092
#define REG_SAR_RESET                       (1 << 9)
#define REG_SAR_ADC_RESET                   (1 << 8)
#define REG_SAR_CLK_OUT_DIV_SHIFT           0
#define REG_SAR_CLK_OUT_DIV_MASK            (0xFF << REG_SAR_CLK_OUT_DIV_SHIFT)
#define REG_SAR_CLK_OUT_DIV(n)              BITFIELD_VAL(REG_SAR_CLK_OUT_DIV, n)

// REG_093
#define REG_SAR_PU_START_DLY_CNT_SHIFT      9
#define REG_SAR_PU_START_DLY_CNT_MASK       (0x7F << REG_SAR_PU_START_DLY_CNT_SHIFT)
#define REG_SAR_PU_START_DLY_CNT(n)         BITFIELD_VAL(REG_SAR_PU_START_DLY_CNT, n)
#define REG_SAR_PU_RST_REG                  (1 << 8)
#define REG_SAR_PU_RST_DR                   (1 << 7)
#define REG_SAR_PU_PRECHARGE_REG            (1 << 6)
#define REG_SAR_PU_PRECHARGE_DR             (1 << 5)
#define REG_SAR_PU_REG                      (1 << 4)
#define REG_SAR_PU_DR                       (1 << 3)
#define REG_SAR_PU_VREF_IN                  (1 << 2)
#define REG_SAR_PU_VREF_REG                 (1 << 1)
#define REG_SAR_PU_VREF_DR                  (1 << 0)

// REG_094
#define REG_SAR_PU_RST_START_DLY_CNT_SHIFT  7
#define REG_SAR_PU_RST_START_DLY_CNT_MASK   (0x7F << REG_SAR_PU_RST_START_DLY_CNT_SHIFT)
#define REG_SAR_PU_RST_START_DLY_CNT(n)     BITFIELD_VAL(REG_SAR_PU_RST_START_DLY_CNT, n)
#define REG_SAR_PU_PRECHARGE_START_DLY_CNT_SHIFT 0
#define REG_SAR_PU_PRECHARGE_START_DLY_CNT_MASK (0x7F << REG_SAR_PU_PRECHARGE_START_DLY_CNT_SHIFT)
#define REG_SAR_PU_PRECHARGE_START_DLY_CNT(n) BITFIELD_VAL(REG_SAR_PU_PRECHARGE_START_DLY_CNT, n)

// REG_095
#define REG_SAR_PU_PRECHARGE_LAST_CNT_SHIFT 7
#define REG_SAR_PU_PRECHARGE_LAST_CNT_MASK  (0x1FF << REG_SAR_PU_PRECHARGE_LAST_CNT_SHIFT)
#define REG_SAR_PU_PRECHARGE_LAST_CNT(n)    BITFIELD_VAL(REG_SAR_PU_PRECHARGE_LAST_CNT, n)
#define REG_SAR_PU_RST_LAST_CNT_SHIFT       0
#define REG_SAR_PU_RST_LAST_CNT_MASK        (0x7F << REG_SAR_PU_RST_LAST_CNT_SHIFT)
#define REG_SAR_PU_RST_LAST_CNT(n)          BITFIELD_VAL(REG_SAR_PU_RST_LAST_CNT, n)


// REG_096
#define SAR_P_BIT05_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT05_WEIGHT_IN_MASK          (0x3FFF << SAR_P_BIT05_WEIGHT_IN_SHIFT)
#define SAR_P_BIT05_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT05_WEIGHT_IN, n)

// REG_097
#define SAR_P_BIT06_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT06_WEIGHT_IN_MASK          (0x3FFF << SAR_P_BIT06_WEIGHT_IN_SHIFT)
#define SAR_P_BIT06_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT06_WEIGHT_IN, n)

// REG_098
#define SAR_P_BIT07_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT07_WEIGHT_IN_MASK          (0x3FFF << SAR_P_BIT07_WEIGHT_IN_SHIFT)
#define SAR_P_BIT07_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT07_WEIGHT_IN, n)

// REG_099
#define SAR_P_BIT08_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT08_WEIGHT_IN_MASK          (0x3FFF << SAR_P_BIT08_WEIGHT_IN_SHIFT)
#define SAR_P_BIT08_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT08_WEIGHT_IN, n)

// REG_09A
#define SAR_P_BIT09_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT09_WEIGHT_IN_MASK          (0x3FFF << SAR_P_BIT09_WEIGHT_IN_SHIFT)
#define SAR_P_BIT09_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT09_WEIGHT_IN, n)

// REG_09B
#define SAR_P_BIT10_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT10_WEIGHT_IN_MASK          (0x3FFF << SAR_P_BIT10_WEIGHT_IN_SHIFT)
#define SAR_P_BIT10_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT10_WEIGHT_IN, n)

// REG_09C
#define SAR_P_BIT11_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT11_WEIGHT_IN_MASK          (0x3FFF << SAR_P_BIT11_WEIGHT_IN_SHIFT)
#define SAR_P_BIT11_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT11_WEIGHT_IN, n)

// REG_09D
#define SAR_P_BIT12_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT12_WEIGHT_IN_MASK          (0x3FFF << SAR_P_BIT12_WEIGHT_IN_SHIFT)
#define SAR_P_BIT12_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT12_WEIGHT_IN, n)

// REG_09E
#define SAR_P_BIT13_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT13_WEIGHT_IN_MASK          (0x3FFF << SAR_P_BIT13_WEIGHT_IN_SHIFT)
#define SAR_P_BIT13_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT13_WEIGHT_IN, n)

// REG_09F
#define SAR_P_BIT14_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT14_WEIGHT_IN_MASK          (0x3FFF << SAR_P_BIT14_WEIGHT_IN_SHIFT)
#define SAR_P_BIT14_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT14_WEIGHT_IN, n)

// REG_0A0
#define SAR_P_BIT15_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT15_WEIGHT_IN_MASK          (0x3FFF << SAR_P_BIT15_WEIGHT_IN_SHIFT)
#define SAR_P_BIT15_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT15_WEIGHT_IN, n)

// REG_0A1
#define SAR_P_BIT16_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT16_WEIGHT_IN_MASK          (0x3FFF << SAR_P_BIT16_WEIGHT_IN_SHIFT)
#define SAR_P_BIT16_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT16_WEIGHT_IN, n)

// REG_0A2
#define SAR_P_BIT17_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT17_WEIGHT_IN_MASK          (0x3FFF << SAR_P_BIT17_WEIGHT_IN_SHIFT)
#define SAR_P_BIT17_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT17_WEIGHT_IN, n)

// REG_0A3
#define SAR_N_BIT05_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT05_WEIGHT_IN_MASK          (0x3FFF << SAR_N_BIT05_WEIGHT_IN_SHIFT)
#define SAR_N_BIT05_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT05_WEIGHT_IN, n)

// REG_0A4
#define SAR_N_BIT06_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT06_WEIGHT_IN_MASK          (0x3FFF << SAR_N_BIT06_WEIGHT_IN_SHIFT)
#define SAR_N_BIT06_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT06_WEIGHT_IN, n)

// REG_0A5
#define SAR_N_BIT07_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT07_WEIGHT_IN_MASK          (0x3FFF << SAR_N_BIT07_WEIGHT_IN_SHIFT)
#define SAR_N_BIT07_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT07_WEIGHT_IN, n)

// REG_0A6
#define SAR_N_BIT08_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT08_WEIGHT_IN_MASK          (0x3FFF << SAR_N_BIT08_WEIGHT_IN_SHIFT)
#define SAR_N_BIT08_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT08_WEIGHT_IN, n)

// REG_0A7
#define SAR_N_BIT09_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT09_WEIGHT_IN_MASK          (0x3FFF << SAR_N_BIT09_WEIGHT_IN_SHIFT)
#define SAR_N_BIT09_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT09_WEIGHT_IN, n)

// REG_0A8
#define SAR_N_BIT10_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT10_WEIGHT_IN_MASK          (0x3FFF << SAR_N_BIT10_WEIGHT_IN_SHIFT)
#define SAR_N_BIT10_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT10_WEIGHT_IN, n)

// REG_0A9
#define SAR_N_BIT11_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT11_WEIGHT_IN_MASK          (0x3FFF << SAR_N_BIT11_WEIGHT_IN_SHIFT)
#define SAR_N_BIT11_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT11_WEIGHT_IN, n)

// REG_0AA
#define SAR_N_BIT12_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT12_WEIGHT_IN_MASK          (0x3FFF << SAR_N_BIT12_WEIGHT_IN_SHIFT)
#define SAR_N_BIT12_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT12_WEIGHT_IN, n)

// REG_0AB
#define SAR_N_BIT13_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT13_WEIGHT_IN_MASK          (0x3FFF << SAR_N_BIT13_WEIGHT_IN_SHIFT)
#define SAR_N_BIT13_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT13_WEIGHT_IN, n)

// REG_0AC
#define SAR_N_BIT14_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT14_WEIGHT_IN_MASK          (0x3FFF << SAR_N_BIT14_WEIGHT_IN_SHIFT)
#define SAR_N_BIT14_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT14_WEIGHT_IN, n)

// REG_0AD
#define SAR_N_BIT15_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT15_WEIGHT_IN_MASK          (0x3FFF << SAR_N_BIT15_WEIGHT_IN_SHIFT)
#define SAR_N_BIT15_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT15_WEIGHT_IN, n)

// REG_0AE
#define SAR_N_BIT16_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT16_WEIGHT_IN_MASK          (0x3FFF << SAR_N_BIT16_WEIGHT_IN_SHIFT)
#define SAR_N_BIT16_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT16_WEIGHT_IN, n)

// REG_0AF
#define SAR_N_BIT17_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT17_WEIGHT_IN_MASK          (0x3FFF << SAR_N_BIT17_WEIGHT_IN_SHIFT)
#define SAR_N_BIT17_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT17_WEIGHT_IN, n)

// REG_0B0
#define REG_SAR_OFFSET_N_IN_SHIFT           5
#define REG_SAR_OFFSET_N_IN_MASK            (0x1F << REG_SAR_OFFSET_N_IN_SHIFT)
#define REG_SAR_OFFSET_N_IN(n)              BITFIELD_VAL(REG_SAR_OFFSET_N_IN, n)
#define REG_SAR_OFFSET_P_IN_SHIFT           0
#define REG_SAR_OFFSET_P_IN_MASK            (0x1F << REG_SAR_OFFSET_P_IN_SHIFT)
#define REG_SAR_OFFSET_P_IN(n)              BITFIELD_VAL(REG_SAR_OFFSET_P_IN, n)


// REG_0B1
#define REG_SAR_ADC_OFFSET_IN_SHIFT         0
#define REG_SAR_ADC_OFFSET_IN_MASK          (0xFFFF << REG_SAR_ADC_OFFSET_IN_SHIFT)
#define REG_SAR_ADC_OFFSET_IN(n)            BITFIELD_VAL(REG_SAR_ADC_OFFSET_IN, n)

// REG_0B2
#define REG_EFUSE_REDUNDANCY_DATA_OUT_SHIFT 2
#define REG_EFUSE_REDUNDANCY_DATA_OUT_MASK  (0xF << REG_EFUSE_REDUNDANCY_DATA_OUT_SHIFT)
#define REG_EFUSE_REDUNDANCY_DATA_OUT(n)    BITFIELD_VAL(REG_EFUSE_REDUNDANCY_DATA_OUT, n)
#define REG_EFUSE_REDUNDANCY_DATA_OUT_DR    (1 << 1)
#define REG_EFUSE_DATA_OUT_DR               (1 << 0)

// REG_0B3
#define REG_EFUSE_DATA_OUT_HI_SHIFT         0
#define REG_EFUSE_DATA_OUT_HI_MASK          (0xFFFF << REG_EFUSE_DATA_OUT_HI_SHIFT)
#define REG_EFUSE_DATA_OUT_HI(n)            BITFIELD_VAL(REG_EFUSE_DATA_OUT_HI, n)

// REG_0B4
#define REG_EFUSE_DATA_OUT_LO_SHIFT         0
#define REG_EFUSE_DATA_OUT_LO_MASK          (0xFFFF << REG_EFUSE_DATA_OUT_LO_SHIFT)
#define REG_EFUSE_DATA_OUT_LO(n)            BITFIELD_VAL(REG_EFUSE_DATA_OUT_LO, n)

// REG_0B5
#define EFUSE_REDUNDANCY_DATA_OUT_SHIFT     0
#define EFUSE_REDUNDANCY_DATA_OUT_MASK      (0xF << EFUSE_REDUNDANCY_DATA_OUT_SHIFT)
#define EFUSE_REDUNDANCY_DATA_OUT(n)        BITFIELD_VAL(EFUSE_REDUNDANCY_DATA_OUT, n)

// REG_0B6
#define EFUSE_CHIP_SEL_ENB_DR               (1 << 15)
#define EFUSE_CHIP_SEL_ENB_REG              (1 << 14)
#define EFUSE_STROBE_DR                     (1 << 13)
#define EFUSE_STROBE_REG                    (1 << 12)
#define EFUSE_LOAD_DR                       (1 << 11)
#define EFUSE_LOAD_REG                      (1 << 10)
#define EFUSE_PGM_ENB_DR                    (1 << 9)
#define EFUSE_PGM_ENB_REG                   (1 << 8)
#define EFUSE_PGM_SEL_DR                    (1 << 7)
#define EFUSE_PGM_SEL_REG                   (1 << 6)
#define EFUSE_READ_MODE_DR                  (1 << 5)
#define EFUSE_READ_MODE_REG                 (1 << 4)
#define EFUSE_PWR_DN_ENB_DR                 (1 << 3)
#define EFUSE_PWR_DN_ENB_REG                (1 << 2)
#define EFUSE_REDUNDANCY_EN_DR              (1 << 1)
#define EFUSE_REDUNDANCY_EN_REG             (1 << 0)


// REG_0B7
#define REG_EFUSE_ADDRESS_SHIFT             6
#define REG_EFUSE_ADDRESS_MASK              (0x1FF << REG_EFUSE_ADDRESS_SHIFT)
#define REG_EFUSE_ADDRESS(n)                BITFIELD_VAL(REG_EFUSE_ADDRESS, n)
#define REG_EFUSE_STROBE_TRIGGER            (1 << 5)
#define REG_EFUSE_TURN_ON                   (1 << 4)
#define REG_EFUSE_CLK_EN                    (1 << 3)
#define REG_EFUSE_READ_MODE                 (1 << 2)
#define REG_EFUSE_PGM_MODE                  (1 << 1)
#define REG_EFUSE_PGM_READ_SEL              (1 << 0)

// REG_0B8
#define REG_EFUSE_TIME_CSB_ADDR_VALUE_SHIFT 10
#define REG_EFUSE_TIME_CSB_ADDR_VALUE_MASK  (0xF << REG_EFUSE_TIME_CSB_ADDR_VALUE_SHIFT)
#define REG_EFUSE_TIME_CSB_ADDR_VALUE(n)    BITFIELD_VAL(REG_EFUSE_TIME_CSB_ADDR_VALUE, n)
#define REG_EFUSE_TIME_PS_CSB_VALUE_SHIFT   6
#define REG_EFUSE_TIME_PS_CSB_VALUE_MASK    (0xF << REG_EFUSE_TIME_PS_CSB_VALUE_SHIFT)
#define REG_EFUSE_TIME_PS_CSB_VALUE(n)      BITFIELD_VAL(REG_EFUSE_TIME_PS_CSB_VALUE, n)
#define REG_EFUSE_TIME_PD_PS_VALUE_SHIFT    0
#define REG_EFUSE_TIME_PD_PS_VALUE_MASK     (0x3F << REG_EFUSE_TIME_PD_PS_VALUE_SHIFT)
#define REG_EFUSE_TIME_PD_PS_VALUE(n)       BITFIELD_VAL(REG_EFUSE_TIME_PD_PS_VALUE, n)


// REG_0B9
#define REG_EFUSE_TIME_PGM_STROBING_VALUE_SHIFT 4
#define REG_EFUSE_TIME_PGM_STROBING_VALUE_MASK (0x1FF << REG_EFUSE_TIME_PGM_STROBING_VALUE_SHIFT)
#define REG_EFUSE_TIME_PGM_STROBING_VALUE(n) BITFIELD_VAL(REG_EFUSE_TIME_PGM_STROBING_VALUE, n)
#define REG_EFUSE_TIME_ADDR_STROBE_VALUE_SHIFT 0
#define REG_EFUSE_TIME_ADDR_STROBE_VALUE_MASK (0xF << REG_EFUSE_TIME_ADDR_STROBE_VALUE_SHIFT)
#define REG_EFUSE_TIME_ADDR_STROBE_VALUE(n) BITFIELD_VAL(REG_EFUSE_TIME_ADDR_STROBE_VALUE, n)

// REG_0BA
#define REG_EFUSE_TIME_READ_STROBING_VALUE_SHIFT 0
#define REG_EFUSE_TIME_READ_STROBING_VALUE_MASK (0x1FF << REG_EFUSE_TIME_READ_STROBING_VALUE_SHIFT)
#define REG_EFUSE_TIME_READ_STROBING_VALUE(n) BITFIELD_VAL(REG_EFUSE_TIME_READ_STROBING_VALUE, n)

// REG_0BB
#define REG_EFUSE_TIME_PS_PD_VALUE_SHIFT    10
#define REG_EFUSE_TIME_PS_PD_VALUE_MASK     (0x3F << REG_EFUSE_TIME_PS_PD_VALUE_SHIFT)
#define REG_EFUSE_TIME_PS_PD_VALUE(n)       BITFIELD_VAL(REG_EFUSE_TIME_PS_PD_VALUE, n)
#define REG_EFUSE_TIME_CSB_PS_VALUE_SHIFT   6
#define REG_EFUSE_TIME_CSB_PS_VALUE_MASK    (0xF << REG_EFUSE_TIME_CSB_PS_VALUE_SHIFT)
#define REG_EFUSE_TIME_CSB_PS_VALUE(n)      BITFIELD_VAL(REG_EFUSE_TIME_CSB_PS_VALUE, n)
#define REG_EFUSE_TIME_STROBE_CSB_VALUE_SHIFT 0
#define REG_EFUSE_TIME_STROBE_CSB_VALUE_MASK (0x3F << REG_EFUSE_TIME_STROBE_CSB_VALUE_SHIFT)
#define REG_EFUSE_TIME_STROBE_CSB_VALUE(n)  BITFIELD_VAL(REG_EFUSE_TIME_STROBE_CSB_VALUE, n)

// REG_0BC
#define EFUSE_REDUNDANCY_INFO_ROW_SEL_DR    (1 << 7)
#define EFUSE_REDUNDANCY_INFO_ROW_SEL_REG   (1 << 6)
#define REG_EFUSE_TIME_PD_OFF_VALUE_SHIFT   0
#define REG_EFUSE_TIME_PD_OFF_VALUE_MASK    (0x3F << REG_EFUSE_TIME_PD_OFF_VALUE_SHIFT)
#define REG_EFUSE_TIME_PD_OFF_VALUE(n)      BITFIELD_VAL(REG_EFUSE_TIME_PD_OFF_VALUE, n)

// REG_0BD
#define EFUSE_DATA_OUT_HI_SHIFT             0
#define EFUSE_DATA_OUT_HI_MASK              (0xFFFF << EFUSE_DATA_OUT_HI_SHIFT)
#define EFUSE_DATA_OUT_HI(n)                BITFIELD_VAL(EFUSE_DATA_OUT_HI, n)

// REG_0BE
#define EFUSE_DATA_OUT_LO_SHIFT             0
#define EFUSE_DATA_OUT_LO_MASK              (0xFFFF << EFUSE_DATA_OUT_LO_SHIFT)
#define EFUSE_DATA_OUT_LO(n)                BITFIELD_VAL(EFUSE_DATA_OUT_LO, n)


// REG_0CB
#define AC_PRESENT_DB_CNT_SHIFT             11
#define AC_PRESENT_DB_CNT_MASK              (0x1F << AC_PRESENT_DB_CNT_SHIFT)
#define AC_PRESENT_DB_CNT(n)                BITFIELD_VAL(AC_PRESENT_DB_CNT, n)
#define AC_PRESENT_INTR_EN                  (1 << 10)
#define AC_PRESENT_N_INTR_MSK               (1 << 9)
#define AC_PRESENT_P_INTR_MSK               (1 << 8)
#define AC_PRESENT_N_INTR_MSKED             (1 << 5)
#define AC_PRESENT_P_INTR_MSKED             (1 << 4)
#define AC_PRESENT_N_INTR                   (1 << 3)
#define AC_PRESENT_P_INTR                   (1 << 2)
#define AC_PRESENT_STATUS                   (1 << 1)
#define AC_PRESENT                          (1 << 0)

#define AC_PRESENT_N_INTR_CLR               (1 << 3)
#define AC_PRESENT_P_INTR_CLR               (1 << 2)






// REG_0D0
#define REG_RC_SLEEP_ALLOW                  (1 << 7)
#define REG_RC_MERGE_MODE                   (1 << 6)
#define REG_RC_SLEEP_MODE_DR                (1 << 5)
#define REG_RC_SLEEP                        (1 << 4)

// REG_0D1
#define REG_VBAT_SUB3V_ISEL_SHIFT           6
#define REG_VBAT_SUB3V_ISEL_MASK            (0x3 << REG_VBAT_SUB3V_ISEL_SHIFT)
#define REG_VBAT_SUB3V_ISEL(n)              BITFIELD_VAL(REG_VBAT_SUB3V_ISEL, n)






// REG_0D2
#define REG_BG_ISEL_SHIFT                   2
#define REG_BG_ISEL_MASK                    (0x3 << REG_BG_ISEL_SHIFT)
#define REG_BG_ISEL(n)                      BITFIELD_VAL(REG_BG_ISEL, n)
#define REG_VRTC_OVERRIDE                   (1 << 1)
#define REG_RTC_LDO_TRAN_ENHANCE            (1 << 0)

// REG_0D3
#define DATA_CHAN8_SHIFT                    0
#define DATA_CHAN8_MASK                     (0xFFFF << DATA_CHAN8_SHIFT)
#define DATA_CHAN8(n)                       BITFIELD_VAL(DATA_CHAN8, n)

// REG_0D4
#define DATA_CHAN9_SHIFT                    0
#define DATA_CHAN9_MASK                     (0xFFFF << DATA_CHAN9_SHIFT)
#define DATA_CHAN9(n)                       BITFIELD_VAL(DATA_CHAN9, n)

// REG_0D5
#define DATA_CHAN10_SHIFT                   0
#define DATA_CHAN10_MASK                    (0xFFFF << DATA_CHAN10_SHIFT)
#define DATA_CHAN10(n)                      BITFIELD_VAL(DATA_CHAN10, n)

// REG_0D6
#define DATA_CHAN11_SHIFT                   0
#define DATA_CHAN11_MASK                    (0xFFFF << DATA_CHAN11_SHIFT)
#define DATA_CHAN11(n)                      BITFIELD_VAL(DATA_CHAN11, n)


// REG_0D7
#define DBG_DONE_SYNC_CNT_SHIFT             4
#define DBG_DONE_SYNC_CNT_MASK              (0x3F << DBG_DONE_SYNC_CNT_SHIFT)
#define DBG_DONE_SYNC_CNT(n)                BITFIELD_VAL(DBG_DONE_SYNC_CNT, n)
#define REG_SAR_SAMPLE_PRE_CYCLE_SHIFT      0
#define REG_SAR_SAMPLE_PRE_CYCLE_MASK       (0xF << REG_SAR_SAMPLE_PRE_CYCLE_SHIFT)
#define REG_SAR_SAMPLE_PRE_CYCLE(n)         BITFIELD_VAL(REG_SAR_SAMPLE_PRE_CYCLE, n)

// REG_0D8
#define REG_SMP_POS_EDGE_SEL                (1 << 8)
#define REG_SAR_SAMPLE_CYCLE_SHIFT          2
#define REG_SAR_SAMPLE_CYCLE_MASK           (0x3F << REG_SAR_SAMPLE_CYCLE_SHIFT)
#define REG_SAR_SAMPLE_CYCLE(n)             BITFIELD_VAL(REG_SAR_SAMPLE_CYCLE, n)
#define REG_SAR_CNT_END_MODE                (1 << 1)
#define REG_DIGI_SMP_MODE                   (1 << 0)

// REG_0DA
#define REG_CLK_32K_EXT1_EN                 (1 << 12)
#define REG_CLK_32K_EXT2_EN                 (1 << 11)
#define REG_CLK_32K_EXT3_EN                 (1 << 10)
#define REG_PU_VSYS_DIV                     (1 << 9)
#define REG_XO_32K_CAPBIT_SHIFT             0
#define REG_XO_32K_CAPBIT_MASK              (0x1FF << REG_XO_32K_CAPBIT_SHIFT)
#define REG_XO_32K_CAPBIT(n)                BITFIELD_VAL(REG_XO_32K_CAPBIT, n)

// REG_0DB
#define DBG_SAR_DOUT_0_15_0_SHIFT           0
#define DBG_SAR_DOUT_0_15_0_MASK            (0xFFFF << DBG_SAR_DOUT_0_15_0_SHIFT)
#define DBG_SAR_DOUT_0_15_0(n)              BITFIELD_VAL(DBG_SAR_DOUT_0_15_0, n)

// REG_0DC
#define DBG_SAR_DOUT_1_15_0_SHIFT           0
#define DBG_SAR_DOUT_1_15_0_MASK            (0xFFFF << DBG_SAR_DOUT_1_15_0_SHIFT)
#define DBG_SAR_DOUT_1_15_0(n)              BITFIELD_VAL(DBG_SAR_DOUT_1_15_0, n)













// REG_0DE
#define DBG_SAR_DOUT_2_15_0_SHIFT           0
#define DBG_SAR_DOUT_2_15_0_MASK            (0xFFFF << DBG_SAR_DOUT_2_15_0_SHIFT)
#define DBG_SAR_DOUT_2_15_0(n)              BITFIELD_VAL(DBG_SAR_DOUT_2_15_0, n)










// REG_0DF
#define REG_GUARD_MODE_SEL                  (1 << 15)
#define POWER_DOWN_INTR_MERGED_MSK          (1 << 14)
#define POWER_DOWN_INTR_MSK                 (1 << 13)
#define HARD_POWER_OFF_INTR_EN              (1 << 12)

#define REG_GUARD_RELEASE_SHIFT             4
#define REG_GUARD_RELEASE_MASK              (0xF << REG_GUARD_RELEASE_SHIFT)
#define REG_GUARD_RELEASE(n)                BITFIELD_VAL(REG_GUARD_RELEASE, n)
#define REG_GUARD_HIGH_SHIFT                0
#define REG_GUARD_HIGH_MASK                 (0xF << REG_GUARD_HIGH_SHIFT)
#define REG_GUARD_HIGH(n)                   BITFIELD_VAL(REG_GUARD_HIGH, n)

// REG_0E0
#define REG_OSC_NPULSE_EXT_EN               (1 << 14)
#define REG_OSC_GUARD_TIME_SHIFT            8
#define REG_OSC_GUARD_TIME_MASK             (0x3F << REG_OSC_GUARD_TIME_SHIFT)
#define REG_OSC_GUARD_TIME(n)               BITFIELD_VAL(REG_OSC_GUARD_TIME, n)
#define REG_POWER_DOWN_DB_VALUE_SHIFT       0
#define REG_POWER_DOWN_DB_VALUE_MASK        (0xFF << REG_POWER_DOWN_DB_VALUE_SHIFT)
#define REG_POWER_DOWN_DB_VALUE(n)          BITFIELD_VAL(REG_POWER_DOWN_DB_VALUE, n)

// REG_0E1
#define REG_BIASB_LPFSEL_DLY_SHIFT          5
#define REG_BIASB_LPFSEL_DLY_MASK           (0x1F << REG_BIASB_LPFSEL_DLY_SHIFT)
#define REG_BIASB_LPFSEL_DLY(n)             BITFIELD_VAL(REG_BIASB_LPFSEL_DLY, n)
#define REG_BIASA_LPFSEL_DLY_SHIFT          0
#define REG_BIASA_LPFSEL_DLY_MASK           (0x1F << REG_BIASA_LPFSEL_DLY_SHIFT)
#define REG_BIASA_LPFSEL_DLY(n)             BITFIELD_VAL(REG_BIASA_LPFSEL_DLY, n)

// REG_0E2
#define REG_RC_ASENSOR_EN                   (1 << 14)
#define REG_MIC_BIASB_EN_DR                 (1 << 13)
#define REG_MIC_LDO_EN_DR                   (1 << 12)
#define REG_MIC_BIASB_LPFSEL_SHIFT          9
#define REG_MIC_BIASB_LPFSEL_MASK           (0x7 << REG_MIC_BIASB_LPFSEL_SHIFT)
#define REG_MIC_BIASB_LPFSEL(n)             BITFIELD_VAL(REG_MIC_BIASB_LPFSEL, n)
#define REG_MIC_BIASB_LPFSEL_CHANGE_SHIFT   6
#define REG_MIC_BIASB_LPFSEL_CHANGE_MASK    (0x7 << REG_MIC_BIASB_LPFSEL_CHANGE_SHIFT)
#define REG_MIC_BIASB_LPFSEL_CHANGE(n)      BITFIELD_VAL(REG_MIC_BIASB_LPFSEL_CHANGE, n)
#define REG_MIC_BIASA_LPFSEL_SHIFT          3
#define REG_MIC_BIASA_LPFSEL_MASK           (0x7 << REG_MIC_BIASA_LPFSEL_SHIFT)
#define REG_MIC_BIASA_LPFSEL(n)             BITFIELD_VAL(REG_MIC_BIASA_LPFSEL, n)
#define REG_MIC_BIASA_LPFSEL_CHANGE_SHIFT   0
#define REG_MIC_BIASA_LPFSEL_CHANGE_MASK    (0x7 << REG_MIC_BIASA_LPFSEL_CHANGE_SHIFT)
#define REG_MIC_BIASA_LPFSEL_CHANGE(n)      BITFIELD_VAL(REG_MIC_BIASA_LPFSEL_CHANGE, n)







// REG_0E4
#define DBG_SAR_DOUT_3_16                   (1 << 3)
#define DBG_SAR_DOUT_2_16                   (1 << 2)
#define DBG_SAR_DOUT_1_16                   (1 << 1)
#define DBG_SAR_DOUT_0_16                   (1 << 0)

// REG_0E5
#define DBG_SAR_DOUT_3_15_0_SHIFT           0
#define DBG_SAR_DOUT_3_15_0_MASK            (0xFFFF << DBG_SAR_DOUT_3_15_0_SHIFT)
#define DBG_SAR_DOUT_3_15_0(n)              BITFIELD_VAL(DBG_SAR_DOUT_3_15_0, n)






// REG_0E6
#define REG_SAR_VREG_SEL_SHIFT              8
#define REG_SAR_VREG_SEL_MASK               (0x7F << REG_SAR_VREG_SEL_SHIFT)
#define REG_SAR_VREG_SEL(n)                 BITFIELD_VAL(REG_SAR_VREG_SEL, n)
#define REG_SAR_EN_VREF_CALIB               (1 << 6)
#define REG_SAR_PD_VREF_ADC_SW              (1 << 5)


// REG_0E7
#define REG_SAR_OFFSET_BIT_VREF_SHIFT       10
#define REG_SAR_OFFSET_BIT_VREF_MASK        (0x3F << REG_SAR_OFFSET_BIT_VREF_SHIFT)
#define REG_SAR_OFFSET_BIT_VREF(n)          BITFIELD_VAL(REG_SAR_OFFSET_BIT_VREF, n)

// REG_0ED
#define REG_RCOSC2M_GOAL_CNT_MARK_15_0_SHIFT 0
#define REG_RCOSC2M_GOAL_CNT_MARK_15_0_MASK (0xFFFF << REG_RCOSC2M_GOAL_CNT_MARK_15_0_SHIFT)
#define REG_RCOSC2M_GOAL_CNT_MARK_15_0(n)   BITFIELD_VAL(REG_RCOSC2M_GOAL_CNT_MARK_15_0, n)

// REG_0EE
#define REG_RCOSC2M_GOAL_CNT_MARK_16        (1 << 15)
#define REG_RCOSC2M_SFT_RSTN                (1 << 14)
#define REG_RCOSC2M_REF_DIV_NUM_SHIFT       4
#define REG_RCOSC2M_REF_DIV_NUM_MASK        (0x3FF << REG_RCOSC2M_REF_DIV_NUM_SHIFT)
#define REG_RCOSC2M_REF_DIV_NUM(n)          BITFIELD_VAL(REG_RCOSC2M_REF_DIV_NUM, n)
#define REG_RCOSC2M_CALIB_EN                (1 << 3)
#define REG_RCOSC2M_TUN_CODE_DR             (1 << 2)
#define REG_RCOSC2M_TUN_CODE_FINE_DR        (1 << 1)
#define REG_RCOSC2M_POLAR_SEL               (1 << 0)

// REG_0EF
#define REG_RCOSC2M_TUN_CODE_SHIFT          5
#define REG_RCOSC2M_TUN_CODE_MASK           (0x3F << REG_RCOSC2M_TUN_CODE_SHIFT)
#define REG_RCOSC2M_TUN_CODE(n)             BITFIELD_VAL(REG_RCOSC2M_TUN_CODE, n)
#define REG_RCOSC2M_TUN_CODE_FINE_SHIFT     0
#define REG_RCOSC2M_TUN_CODE_FINE_MASK      (0x1F << REG_RCOSC2M_TUN_CODE_FINE_SHIFT)
#define REG_RCOSC2M_TUN_CODE_FINE(n)        BITFIELD_VAL(REG_RCOSC2M_TUN_CODE_FINE, n)

// REG_0F0
#define DBG_RCOSC2M_GOAL_CNT_16             (1 << 6)
#define REG_REF_DIV_SHIFT                   0
#define REG_REF_DIV_MASK                    (0x3F << REG_REF_DIV_SHIFT)
#define REG_REF_DIV(n)                      BITFIELD_VAL(REG_REF_DIV, n)

// REG_0F1
#define DBG_RCOSC2M_GOAL_CNT_15_0_SHIFT     0
#define DBG_RCOSC2M_GOAL_CNT_15_0_MASK      (0xFFFF << DBG_RCOSC2M_GOAL_CNT_15_0_SHIFT)
#define DBG_RCOSC2M_GOAL_CNT_15_0(n)        BITFIELD_VAL(DBG_RCOSC2M_GOAL_CNT_15_0, n)

// REG_0F2
#define DBG_RCOSC2M_STATE_SHIFT             12
#define DBG_RCOSC2M_STATE_MASK              (0x3 << DBG_RCOSC2M_STATE_SHIFT)
#define DBG_RCOSC2M_STATE(n)                BITFIELD_VAL(DBG_RCOSC2M_STATE, n)
#define DBG_RCOSC2M_TUN_DONE                (1 << 11)
#define DBG_RCOSC2M_FRIM_SHIFT              5
#define DBG_RCOSC2M_FRIM_MASK               (0x3F << DBG_RCOSC2M_FRIM_SHIFT)
#define DBG_RCOSC2M_FRIM(n)                 BITFIELD_VAL(DBG_RCOSC2M_FRIM, n)
#define DBG_RCOSC2M_FRIM_FINE_SHIFT         0
#define DBG_RCOSC2M_FRIM_FINE_MASK          (0x1F << DBG_RCOSC2M_FRIM_FINE_SHIFT)
#define DBG_RCOSC2M_FRIM_FINE(n)            BITFIELD_VAL(DBG_RCOSC2M_FRIM_FINE, n)

// REG_0F3
#define REG_RCOSC_2M_VOUT_SEL_SHIFT         13
#define REG_RCOSC_2M_VOUT_SEL_MASK          (0x7 << REG_RCOSC_2M_VOUT_SEL_SHIFT)
#define REG_RCOSC_2M_VOUT_SEL(n)            BITFIELD_VAL(REG_RCOSC_2M_VOUT_SEL, n)

#define DBG_RCOSC2M_REF_CNT_SHIFT           0
#define DBG_RCOSC2M_REF_CNT_MASK            (0x3FF << DBG_RCOSC2M_REF_CNT_SHIFT)
#define DBG_RCOSC2M_REF_CNT(n)              BITFIELD_VAL(DBG_RCOSC2M_REF_CNT, n)

// REG_0F4
#define REG_RCOSC_2M_TO_INF_CUTOFF          (1 << 15)
#define REG_RCOSC_2M_LP_LDO_SHIFT           13
#define REG_RCOSC_2M_LP_LDO_MASK            (0x3 << REG_RCOSC_2M_LP_LDO_SHIFT)
#define REG_RCOSC_2M_LP_LDO(n)              BITFIELD_VAL(REG_RCOSC_2M_LP_LDO, n)
#define REG_RCOSC_2M_CALIB_GATE             (1 << 12)
#define REG_RCOSC_2M_EN_CLK_DIG             (1 << 11)
#define REG_RCOSC_2M_EN_DR                  (1 << 10)
#define REG_RCOSC_2M_EN                     (1 << 9)
#define REG_RCOSC_2M_PU_LDO_DR              (1 << 8)
#define REG_RCOSC_2M_PU_LDO                 (1 << 7)
#define REG_RCOSC_2M_EN_AGPIO_TEST          (1 << 6)
#define REG_RCOSC_2M_EN_CLK_PMU_DIG         (1 << 5)
#define REG_RCOSC_2M_EN_CLK_DLATCH          (1 << 4)
#define REG_RCOSC_2M_OSC_FR_SHIFT           2
#define REG_RCOSC_2M_OSC_FR_MASK            (0x3 << REG_RCOSC_2M_OSC_FR_SHIFT)
#define REG_RCOSC_2M_OSC_FR(n)              BITFIELD_VAL(REG_RCOSC_2M_OSC_FR, n)
#define REG_RCOSC_2M_PWR_BIT_SHIFT          0
#define REG_RCOSC_2M_PWR_BIT_MASK           (0x3 << REG_RCOSC_2M_PWR_BIT_SHIFT)
#define REG_RCOSC_2M_PWR_BIT(n)             BITFIELD_VAL(REG_RCOSC_2M_PWR_BIT, n)




// REG_0F5
#define REG_RAMP_DIV_BYPASS                 (1 << 14)
#define REG_RAMP_BIAS_DIV_BYPASS            (1 << 13)
#define REG_CLK_RAMP_DIV_SHIFT              8
#define REG_CLK_RAMP_DIV_MASK               (0x1F << REG_CLK_RAMP_DIV_SHIFT)
#define REG_CLK_RAMP_DIV(n)                 BITFIELD_VAL(REG_CLK_RAMP_DIV, n)
#define REG_RAMP_BIAS_FAST_MSK              (1 << 7)
#define REG_CLK_RAMP_RC_GATE_EN             (1 << 6)
#define REG_RAMP_FAST_EN                    (1 << 5)
#define REG_RAMP_FAST_EN_DR                 (1 << 4)
#define REG_RAMP_CLK_TIME_SHIFT             0
#define REG_RAMP_CLK_TIME_MASK              (0xF << REG_RAMP_CLK_TIME_SHIFT)
#define REG_RAMP_CLK_TIME(n)                BITFIELD_VAL(REG_RAMP_CLK_TIME, n)

// REG_0F6
#define REG_CLK_RAMP_BIAS_DIV_SHIFT         8
#define REG_CLK_RAMP_BIAS_DIV_MASK          (0x1F << REG_CLK_RAMP_BIAS_DIV_SHIFT)
#define REG_CLK_RAMP_BIAS_DIV(n)            BITFIELD_VAL(REG_CLK_RAMP_BIAS_DIV, n)
#define ABORT_SOURCE_CLR                    (1 << 7)
#define PMU_LDO_ON_SOURCE_CLR               (1 << 6)
#define ABORT_SOURCE_SHIFT                  3
#define ABORT_SOURCE_MASK                   (0x7 << ABORT_SOURCE_SHIFT)
#define ABORT_SOURCE(n)                     BITFIELD_VAL(ABORT_SOURCE, n)
#define PMU_LDO_ON_SOURCE_SHIFT             0
#define PMU_LDO_ON_SOURCE_MASK              (0x7 << PMU_LDO_ON_SOURCE_SHIFT)
#define PMU_LDO_ON_SOURCE(n)                BITFIELD_VAL(PMU_LDO_ON_SOURCE, n)

// REG_0FD
#define REG_PU_RC_CAP_MSK                   (1 << 10)

// REG_0FE
#define REG_LPO_CONST_BIAS_SEL_SHIFT        12
#define REG_LPO_CONST_BIAS_SEL_MASK         (0x7 << REG_LPO_CONST_BIAS_SEL_SHIFT)
#define REG_LPO_CONST_BIAS_SEL(n)           BITFIELD_VAL(REG_LPO_CONST_BIAS_SEL, n)
#define REG_LPO_CTRIM_SHIFT                 5
#define REG_LPO_CTRIM_MASK                  (0x7F << REG_LPO_CTRIM_SHIFT)
#define REG_LPO_CTRIM(n)                    BITFIELD_VAL(REG_LPO_CTRIM, n)
#define REG_LPO_BIAS_SEL_SHIFT              1
#define REG_LPO_BIAS_SEL_MASK               (0xF << REG_LPO_BIAS_SEL_SHIFT)
#define REG_LPO_BIAS_SEL(n)                 BITFIELD_VAL(REG_LPO_BIAS_SEL, n)
#define REG_LPO_FILTER_EN                   (1 << 0)



























// REG_100
#define REVID_P1_SHIFT                      0
#define REVID_P1_MASK                       (0xF << REVID_P1_SHIFT)
#define REVID_P1(n)                         BITFIELD_VAL(REVID_P1, n)
#define CHIPID_P1_SHIFT                     4
#define CHIPID_P1_MASK                      (0xFFF << CHIPID_P1_SHIFT)
#define CHIPID_P1(n)                        BITFIELD_VAL(CHIPID_P1, n)



// REG_10A
#define RTC_MATCH_VALUE_TIME_15_0_SHIFT     0
#define RTC_MATCH_VALUE_TIME_15_0_MASK      (0xFFFF << RTC_MATCH_VALUE_TIME_15_0_SHIFT)
#define RTC_MATCH_VALUE_TIME_15_0(n)        BITFIELD_VAL(RTC_MATCH_VALUE_TIME_15_0, n)

// REG_10B
#define RTC_MATCH_VALUE_TIME_31_16_SHIFT    0
#define RTC_MATCH_VALUE_TIME_31_16_MASK     (0xFFFF << RTC_MATCH_VALUE_TIME_31_16_SHIFT)
#define RTC_MATCH_VALUE_TIME_31_16(n)       BITFIELD_VAL(RTC_MATCH_VALUE_TIME_31_16, n)

// REG_10C
#define RTC_MATCH_VALUE_DATE_15_0_SHIFT     0
#define RTC_MATCH_VALUE_DATE_15_0_MASK      (0xFFFF << RTC_MATCH_VALUE_DATE_15_0_SHIFT)
#define RTC_MATCH_VALUE_DATE_15_0(n)        BITFIELD_VAL(RTC_MATCH_VALUE_DATE_15_0, n)

// REG_10D
#define RTC_MATCH_VALUE_DATE_31_16_SHIFT    0
#define RTC_MATCH_VALUE_DATE_31_16_MASK     (0xFFFF << RTC_MATCH_VALUE_DATE_31_16_SHIFT)
#define RTC_MATCH_VALUE_DATE_31_16(n)       BITFIELD_VAL(RTC_MATCH_VALUE_DATE_31_16, n)

// REG_10E
#define REG_CLK_RTC_SEL_XO                  (1 << 1)
#define RTC_TIME_LOAD                       (1 << 0)

// REG_10F
#define RTC_TIME_LOAD_VALUE_15_0_SHIFT      0
#define RTC_TIME_LOAD_VALUE_15_0_MASK       (0xFFFF << RTC_TIME_LOAD_VALUE_15_0_SHIFT)
#define RTC_TIME_LOAD_VALUE_15_0(n)         BITFIELD_VAL(RTC_TIME_LOAD_VALUE_15_0, n)

// REG_110
#define RTC_TIME_LOAD_VALUE_31_16_SHIFT     0
#define RTC_TIME_LOAD_VALUE_31_16_MASK      (0xFFFF << RTC_TIME_LOAD_VALUE_31_16_SHIFT)
#define RTC_TIME_LOAD_VALUE_31_16(n)        BITFIELD_VAL(RTC_TIME_LOAD_VALUE_31_16, n)

// REG_111
#define RTC_DATE_LOAD_VALUE_15_0_SHIFT      0
#define RTC_DATE_LOAD_VALUE_15_0_MASK       (0xFFFF << RTC_DATE_LOAD_VALUE_15_0_SHIFT)
#define RTC_DATE_LOAD_VALUE_15_0(n)         BITFIELD_VAL(RTC_DATE_LOAD_VALUE_15_0, n)


// REG_112
#define RTC_DATE_LOAD_VALUE_31_16_SHIFT     0
#define RTC_DATE_LOAD_VALUE_31_16_MASK      (0xFFFF << RTC_DATE_LOAD_VALUE_31_16_SHIFT)
#define RTC_DATE_LOAD_VALUE_31_16(n)        BITFIELD_VAL(RTC_DATE_LOAD_VALUE_31_16, n)

// REG_113
#define RTC_TIME_VALUE_15_0_SHIFT           0
#define RTC_TIME_VALUE_15_0_MASK            (0xFFFF << RTC_TIME_VALUE_15_0_SHIFT)
#define RTC_TIME_VALUE_15_0(n)              BITFIELD_VAL(RTC_TIME_VALUE_15_0, n)

// REG_114
#define RTC_TIME_VALUE_31_16_SHIFT          0
#define RTC_TIME_VALUE_31_16_MASK           (0xFFFF << RTC_TIME_VALUE_31_16_SHIFT)
#define RTC_TIME_VALUE_31_16(n)             BITFIELD_VAL(RTC_TIME_VALUE_31_16, n)

// REG_115
#define RTC_DATE_VALUE_15_0_SHIFT           0
#define RTC_DATE_VALUE_15_0_MASK            (0xFFFF << RTC_DATE_VALUE_15_0_SHIFT)
#define RTC_DATE_VALUE_15_0(n)              BITFIELD_VAL(RTC_DATE_VALUE_15_0, n)

// REG_116
#define RTC_DATE_VALUE_31_16_SHIFT          0
#define RTC_DATE_VALUE_31_16_MASK           (0xFFFF << RTC_DATE_VALUE_31_16_SHIFT)
#define RTC_DATE_VALUE_31_16(n)             BITFIELD_VAL(RTC_DATE_VALUE_31_16, n)


// REG_126
#define REG_LDO_VANA_BYPASS                 (1 << 15)
#define REG_LDO_VANA_OCP_EN                 (1 << 14)
#define REG_TRAN_ENHANCE_LDO_VANA_DR        (1 << 13)
#define REG_TRAN_ENHANCE_LDO_VANA_REG       (1 << 12)
#define REG_LDO_VANA_PULLDOWN               (1 << 11)
#define REG_LDO_VANA_RAMP_STEP_SHIFT        7
#define REG_LDO_VANA_RAMP_STEP_MASK         (0xF << REG_LDO_VANA_RAMP_STEP_SHIFT)
#define REG_LDO_VANA_RAMP_STEP(n)           BITFIELD_VAL(REG_LDO_VANA_RAMP_STEP, n)
#define REG_LDO_VANA_RAMP_EN                (1 << 6)
#define REG_PU_LDO_VANA_DSLEEP              (1 << 5)
#define REG_PU_LDO_VANA_DR                  (1 << 4)
#define REG_PU_LDO_VANA                     (1 << 3)
#define REG_LP_EN_LDO_VANA_DSLEEP           (1 << 2)
#define REG_LP_EN_LDO_VANA_DR               (1 << 1)
#define REG_LP_EN_LDO_VANA                  (1 << 0)

#define REG_PU_LDO_VANA_REG                 REG_PU_LDO_VANA
#define REG_LP_EN_LDO_VANA_REG              REG_LP_EN_LDO_VANA


// REG_128
#define REG_LDO_VANA_VBIT_NORMAL_SHIFT      10
#define REG_LDO_VANA_VBIT_NORMAL_MASK       (0x1F << REG_LDO_VANA_VBIT_NORMAL_SHIFT)
#define REG_LDO_VANA_VBIT_NORMAL(n)         BITFIELD_VAL(REG_LDO_VANA_VBIT_NORMAL, n)
#define REG_LDO_VANA_VBIT_DSLEEP_SHIFT      5
#define REG_LDO_VANA_VBIT_DSLEEP_MASK       (0x1F << REG_LDO_VANA_VBIT_DSLEEP_SHIFT)
#define REG_LDO_VANA_VBIT_DSLEEP(n)         BITFIELD_VAL(REG_LDO_VANA_VBIT_DSLEEP, n)
#define REG_LDO_VANA_VBIT_RC_SHIFT          0
#define REG_LDO_VANA_VBIT_RC_MASK           (0x1F << REG_LDO_VANA_VBIT_RC_SHIFT)
#define REG_LDO_VANA_VBIT_RC(n)             BITFIELD_VAL(REG_LDO_VANA_VBIT_RC, n)

#define MAX_LDO_VANA_BIT_VAL                (REG_LDO_VANA_VBIT_NORMAL_MASK >> REG_LDO_VANA_VBIT_NORMAL_SHIFT)

// REG_129
#define REG_LDO_VRTC_RAMP_STEP_SHIFT        1
#define REG_LDO_VRTC_RAMP_STEP_MASK         (0x7 << REG_LDO_VRTC_RAMP_STEP_SHIFT)
#define REG_LDO_VRTC_RAMP_STEP(n)           BITFIELD_VAL(REG_LDO_VRTC_RAMP_STEP, n)
#define REG_LDO_VRTC_RAMP_EN                (1 << 0)

// REG_12A
#define REG_LDO_VRTC_VBIT_PWR_DOWN_SHIFT    12
#define REG_LDO_VRTC_VBIT_PWR_DOWN_MASK     (0xF << REG_LDO_VRTC_VBIT_PWR_DOWN_SHIFT)
#define REG_LDO_VRTC_VBIT_PWR_DOWN(n)       BITFIELD_VAL(REG_LDO_VRTC_VBIT_PWR_DOWN, n)
#define REG_LDO_VRTC_VBIT_NORMAL_SHIFT      8
#define REG_LDO_VRTC_VBIT_NORMAL_MASK       (0xF << REG_LDO_VRTC_VBIT_NORMAL_SHIFT)
#define REG_LDO_VRTC_VBIT_NORMAL(n)         BITFIELD_VAL(REG_LDO_VRTC_VBIT_NORMAL, n)
#define REG_LDO_VRTC_VBIT_DSLEEP_SHIFT      4
#define REG_LDO_VRTC_VBIT_DSLEEP_MASK       (0xF << REG_LDO_VRTC_VBIT_DSLEEP_SHIFT)
#define REG_LDO_VRTC_VBIT_DSLEEP(n)         BITFIELD_VAL(REG_LDO_VRTC_VBIT_DSLEEP, n)
#define REG_LDO_VRTC_VBIT_RC_SHIFT          0
#define REG_LDO_VRTC_VBIT_RC_MASK           (0xF << REG_LDO_VRTC_VBIT_RC_SHIFT)
#define REG_LDO_VRTC_VBIT_RC(n)             BITFIELD_VAL(REG_LDO_VRTC_VBIT_RC, n)

// REG_12B
#define REG_LP_BIAS_SEL_LDO_REG_SHIFT       8
#define REG_LP_BIAS_SEL_LDO_REG_MASK        (0x1F << REG_LP_BIAS_SEL_LDO_REG_SHIFT)
#define REG_LP_BIAS_SEL_LDO_REG(n)          BITFIELD_VAL(REG_LP_BIAS_SEL_LDO_REG, n)
#define REG_LDO_LP_BIAS_RAMP_STEP_SHIFT     2
#define REG_LDO_LP_BIAS_RAMP_STEP_MASK      (0x7 << REG_LDO_LP_BIAS_RAMP_STEP_SHIFT)
#define REG_LDO_LP_BIAS_RAMP_STEP(n)        BITFIELD_VAL(REG_LDO_LP_BIAS_RAMP_STEP, n)
#define REG_LDO_LP_BIAS_RAMP_EN             (1 << 1)
#define REG_LP_BIAS_SEL_LDO_DR              (1 << 0)


// REG_12C
#define REG_LDO_LP_BIAS_VBIT_NORMAL_SHIFT   10
#define REG_LDO_LP_BIAS_VBIT_NORMAL_MASK    (0x1F << REG_LDO_LP_BIAS_VBIT_NORMAL_SHIFT)
#define REG_LDO_LP_BIAS_VBIT_NORMAL(n)      BITFIELD_VAL(REG_LDO_LP_BIAS_VBIT_NORMAL, n)
#define REG_LDO_LP_BIAS_VBIT_DSLEEP_SHIFT   5
#define REG_LDO_LP_BIAS_VBIT_DSLEEP_MASK    (0x1F << REG_LDO_LP_BIAS_VBIT_DSLEEP_SHIFT)
#define REG_LDO_LP_BIAS_VBIT_DSLEEP(n)      BITFIELD_VAL(REG_LDO_LP_BIAS_VBIT_DSLEEP, n)
#define REG_LDO_LP_BIAS_VBIT_RC_SHIFT       0
#define REG_LDO_LP_BIAS_VBIT_RC_MASK        (0x1F << REG_LDO_LP_BIAS_VBIT_RC_SHIFT)
#define REG_LDO_LP_BIAS_VBIT_RC(n)          BITFIELD_VAL(REG_LDO_LP_BIAS_VBIT_RC, n)

// REG_12D
#define REG_BUCK_OFFSET_CALIB_EN_DR         (1 << 8)
#define REG_BUCK_OFFSET_DR                  (1 << 7)
#define REG_BUCK_OFFSET_SHIFT               2
#define REG_BUCK_OFFSET_MASK                (0x1F << REG_BUCK_OFFSET_SHIFT)
#define REG_BUCK_OFFSET(n)                  BITFIELD_VAL(REG_BUCK_OFFSET, n)
#define REG_BUCK_OFFSET_CALIB_EN            (1 << 1)
#define REG_BUCK_OFFSET_POL_SEL             (1 << 0)

// REG_130
#define REG_BUCK_VHPPA_RAMP_STEP_SHIFT      4
#define REG_BUCK_VHPPA_RAMP_STEP_MASK       (0x1F << REG_BUCK_VHPPA_RAMP_STEP_SHIFT)
#define REG_BUCK_VHPPA_RAMP_STEP(n)         BITFIELD_VAL(REG_BUCK_VHPPA_RAMP_STEP, n)
#define REG_BUCK_VHPPA_RAMP_EN              (1 << 3)
#define REG_BUCK_LX2_PULLDOWN               (1 << 2)
#define REG_BUCK_VCORE_PULLDOWN             (1 << 1)
#define REG_BUCK_VHPPA_PULLDOWN             (1 << 0)

// REG_131
#define REG_BUCK_POWER_UP_SOFT_CNT_SHIFT    9
#define REG_BUCK_POWER_UP_SOFT_CNT_MASK     (0xF << REG_BUCK_POWER_UP_SOFT_CNT_SHIFT)
#define REG_BUCK_POWER_UP_SOFT_CNT(n)       BITFIELD_VAL(REG_BUCK_POWER_UP_SOFT_CNT, n)
#define REG_BUCK_VCORE_RAMP_STEP_SHIFT      4
#define REG_BUCK_VCORE_RAMP_STEP_MASK       (0x1F << REG_BUCK_VCORE_RAMP_STEP_SHIFT)
#define REG_BUCK_VCORE_RAMP_STEP(n)         BITFIELD_VAL(REG_BUCK_VCORE_RAMP_STEP, n)
#define REG_BUCK_VCORE_RAMP_EN              (1 << 3)
#define REG_BUCK_DSLEEP_MSK                 (1 << 2)
#define REG_PU_BUCK_DR                      (1 << 1)
#define REG_PU_BUCK                         (1 << 0)

// REG_132
#define REG_BUCK_VREF_SEL_NORMAL_SHIFT      12
#define REG_BUCK_VREF_SEL_NORMAL_MASK       (0xF << REG_BUCK_VREF_SEL_NORMAL_SHIFT)
#define REG_BUCK_VREF_SEL_NORMAL(n)         BITFIELD_VAL(REG_BUCK_VREF_SEL_NORMAL, n)
#define REG_BUCK_IS_GAIN_NORMAL_SHIFT       8
#define REG_BUCK_IS_GAIN_NORMAL_MASK        (0x7 << REG_BUCK_IS_GAIN_NORMAL_SHIFT)
#define REG_BUCK_IS_GAIN_NORMAL(n)          BITFIELD_VAL(REG_BUCK_IS_GAIN_NORMAL, n)
#define REG_BUCK_VCORE_VBIT_NORMAL_SHIFT    0
#define REG_BUCK_VCORE_VBIT_NORMAL_MASK     (0xFF << REG_BUCK_VCORE_VBIT_NORMAL_SHIFT)
#define REG_BUCK_VCORE_VBIT_NORMAL(n)       BITFIELD_VAL(REG_BUCK_VCORE_VBIT_NORMAL, n)

// REG_133
#define REG_BUCK_VREF_SEL_DSLEEP_SHIFT      12
#define REG_BUCK_VREF_SEL_DSLEEP_MASK       (0xF << REG_BUCK_VREF_SEL_DSLEEP_SHIFT)
#define REG_BUCK_VREF_SEL_DSLEEP(n)         BITFIELD_VAL(REG_BUCK_VREF_SEL_DSLEEP, n)
#define REG_BUCK_IS_GAIN_DSLEEP_SHIFT       8
#define REG_BUCK_IS_GAIN_DSLEEP_MASK        (0x7 << REG_BUCK_IS_GAIN_DSLEEP_SHIFT)
#define REG_BUCK_IS_GAIN_DSLEEP(n)          BITFIELD_VAL(REG_BUCK_IS_GAIN_DSLEEP, n)
#define REG_BUCK_VCORE_VBIT_DSLEEP_SHIFT    0
#define REG_BUCK_VCORE_VBIT_DSLEEP_MASK     (0xFF << REG_BUCK_VCORE_VBIT_DSLEEP_SHIFT)
#define REG_BUCK_VCORE_VBIT_DSLEEP(n)       BITFIELD_VAL(REG_BUCK_VCORE_VBIT_DSLEEP, n)

// REG_134
#define REG_BUCK_VREF_SEL_RC_SHIFT          12
#define REG_BUCK_VREF_SEL_RC_MASK           (0xF << REG_BUCK_VREF_SEL_RC_SHIFT)
#define REG_BUCK_VREF_SEL_RC(n)             BITFIELD_VAL(REG_BUCK_VREF_SEL_RC, n)
#define REG_BUCK_IS_GAIN_RC_SHIFT           8
#define REG_BUCK_IS_GAIN_RC_MASK            (0x7 << REG_BUCK_IS_GAIN_RC_SHIFT)
#define REG_BUCK_IS_GAIN_RC(n)              BITFIELD_VAL(REG_BUCK_IS_GAIN_RC, n)
#define REG_BUCK_VCORE_VBIT_RC_SHIFT        0
#define REG_BUCK_VCORE_VBIT_RC_MASK         (0xFF << REG_BUCK_VCORE_VBIT_RC_SHIFT)
#define REG_BUCK_VCORE_VBIT_RC(n)           BITFIELD_VAL(REG_BUCK_VCORE_VBIT_RC, n)

// REG_135
#define REG_BUCK_VHPPA_VBIT_NORMAL_SHIFT    8
#define REG_BUCK_VHPPA_VBIT_NORMAL_MASK     (0xFF << REG_BUCK_VHPPA_VBIT_NORMAL_SHIFT)
#define REG_BUCK_VHPPA_VBIT_NORMAL(n)       BITFIELD_VAL(REG_BUCK_VHPPA_VBIT_NORMAL, n)
#define REG_BUCK_IX2_ERR_NORMAL             (1 << 2)
#define REG_BUCK_BURST_MODE_SEL_NORMAL      (1 << 1)
#define REG_BUCK_IX4_ERR_NORMAL             (1 << 0)

// REG_136
#define REG_BUCK_VHPPA_VBIT_DSLEEP_SHIFT    8
#define REG_BUCK_VHPPA_VBIT_DSLEEP_MASK     (0xFF << REG_BUCK_VHPPA_VBIT_DSLEEP_SHIFT)
#define REG_BUCK_VHPPA_VBIT_DSLEEP(n)       BITFIELD_VAL(REG_BUCK_VHPPA_VBIT_DSLEEP, n)
#define REG_BUCK_IX2_ERR_DSLEEP             (1 << 2)
#define REG_BUCK_BURST_MODE_SEL_DSLEEP      (1 << 1)
#define REG_BUCK_IX4_ERR_DSLEEP             (1 << 0)

// REG_137
#define REG_BUCK_VHPPA_VBIT_RC_SHIFT        8
#define REG_BUCK_VHPPA_VBIT_RC_MASK         (0xFF << REG_BUCK_VHPPA_VBIT_RC_SHIFT)
#define REG_BUCK_VHPPA_VBIT_RC(n)           BITFIELD_VAL(REG_BUCK_VHPPA_VBIT_RC, n)
#define REG_BUCK_IX2_ERR_RC                 (1 << 2)
#define REG_BUCK_BURST_MODE_SEL_RC          (1 << 1)
#define REG_BUCK_IX4_ERR_RC                 (1 << 0)

// REG_138
#define REG_BUCK_EDGE_CON_RX_SHIFT          12
#define REG_BUCK_EDGE_CON_RX_MASK           (0xF << REG_BUCK_EDGE_CON_RX_SHIFT)
#define REG_BUCK_EDGE_CON_RX(n)             BITFIELD_VAL(REG_BUCK_EDGE_CON_RX, n)
#define REG_BUCK_CAL_DELTAV_SHIFT           4
#define REG_BUCK_CAL_DELTAV_MASK            (0x3F << REG_BUCK_CAL_DELTAV_SHIFT)
#define REG_BUCK_CAL_DELTAV(n)              BITFIELD_VAL(REG_BUCK_CAL_DELTAV, n)
#define REG_BUCK_CC_CAP_BIT_SHIFT           0
#define REG_BUCK_CC_CAP_BIT_MASK            (0xF << REG_BUCK_CC_CAP_BIT_SHIFT)
#define REG_BUCK_CC_CAP_BIT(n)              BITFIELD_VAL(REG_BUCK_CC_CAP_BIT, n)

// REG_139
#define REG_BUCK_SYNC_DISABLE               (1 << 14)
#define REG_BUCK_CC_SLOPE_BIT               (1 << 13)
#define REG_BUCK_I_DELTAV_X2                (1 << 12)
#define REG_BUCK_CC_SENSE_BIT_SHIFT         10
#define REG_BUCK_CC_SENSE_BIT_MASK          (0x3 << REG_BUCK_CC_SENSE_BIT_SHIFT)
#define REG_BUCK_CC_SENSE_BIT(n)            BITFIELD_VAL(REG_BUCK_CC_SENSE_BIT, n)
#define REG_BUCK_BURST_THRESHOLD_SHIFT      4
#define REG_BUCK_BURST_THRESHOLD_MASK       (0x3F << REG_BUCK_BURST_THRESHOLD_SHIFT)
#define REG_BUCK_BURST_THRESHOLD(n)         BITFIELD_VAL(REG_BUCK_BURST_THRESHOLD, n)
#define REG_BUCK_DT_BIT                     (1 << 3)
#define REG_BUCK_I2V_BIT1_SHIFT             0
#define REG_BUCK_I2V_BIT1_MASK              (0x7 << REG_BUCK_I2V_BIT1_SHIFT)
#define REG_BUCK_I2V_BIT1(n)                BITFIELD_VAL(REG_BUCK_I2V_BIT1, n)

// REG_13A
#define REG_BUCK_EN_ZCD_CAL                 (1 << 13)
#define REG_BUCK_EDGE_CON_SHIFT             9
#define REG_BUCK_EDGE_CON_MASK              (0xF << REG_BUCK_EDGE_CON_SHIFT)
#define REG_BUCK_EDGE_CON(n)                BITFIELD_VAL(REG_BUCK_EDGE_CON, n)
#define REG_BUCK_SLOPE_DOUBLE               (1 << 8)
#define REG_BUCK_I2V_BIT2_SHIFT             5
#define REG_BUCK_I2V_BIT2_MASK              (0x7 << REG_BUCK_I2V_BIT2_SHIFT)
#define REG_BUCK_I2V_BIT2(n)                BITFIELD_VAL(REG_BUCK_I2V_BIT2, n)
#define REG_BUCK_V2I_BIT2_SHIFT             2
#define REG_BUCK_V2I_BIT2_MASK              (0x7 << REG_BUCK_V2I_BIT2_SHIFT)
#define REG_BUCK_V2I_BIT2(n)                BITFIELD_VAL(REG_BUCK_V2I_BIT2, n)
#define REG_BUCK_INTERNAL_FREQUENCY_SHIFT   0
#define REG_BUCK_INTERNAL_FREQUENCY_MASK    (0x3 << REG_BUCK_INTERNAL_FREQUENCY_SHIFT)
#define REG_BUCK_INTERNAL_FREQUENCY(n)      BITFIELD_VAL(REG_BUCK_INTERNAL_FREQUENCY, n)

// REG_13B
#define REG_BUCK_LOW_VOLTAGE                (1 << 15)
#define REG_BUCK_LP_VCOMP                   (1 << 14)
#define REG_BUCK_LP_VCOMP2_SHIFT            12
#define REG_BUCK_LP_VCOMP2_MASK             (0x3 << REG_BUCK_LP_VCOMP2_SHIFT)
#define REG_BUCK_LP_VCOMP2(n)               BITFIELD_VAL(REG_BUCK_LP_VCOMP2, n)
#define REG_BUCK_V2I_BIT1_SHIFT             9
#define REG_BUCK_V2I_BIT1_MASK              (0x7 << REG_BUCK_V2I_BIT1_SHIFT)
#define REG_BUCK_V2I_BIT1(n)                BITFIELD_VAL(REG_BUCK_V2I_BIT1, n)
#define REG_BUCK_ANTI_RES_DISABLE           (1 << 8)
#define REG_BUCK_OFFSET_CURRENT_EN          (1 << 7)
#define REG_BUCK_CC_CAP_BIT2_SHIFT          5
#define REG_BUCK_CC_CAP_BIT2_MASK           (0x3 << REG_BUCK_CC_CAP_BIT2_SHIFT)
#define REG_BUCK_CC_CAP_BIT2(n)             BITFIELD_VAL(REG_BUCK_CC_CAP_BIT2, n)


// REG_13C
#define REG_BUCK_ZCD_CAP_BIT_SHIFT          12
#define REG_BUCK_ZCD_CAP_BIT_MASK           (0xF << REG_BUCK_ZCD_CAP_BIT_SHIFT)
#define REG_BUCK_ZCD_CAP_BIT(n)             BITFIELD_VAL(REG_BUCK_ZCD_CAP_BIT, n)
#define REG_BUCK_RECOVER                    (1 << 11)
#define REG_BUCK_BIAS_HALF_P                (1 << 10)
#define REG_BUCK_REV_BACK                   (1 << 9)
#define REG_BUCK_EN_SIMO                    (1 << 8)
#define REG_BUCK_RESERVED_SHIFT             0
#define REG_BUCK_RESERVED_MASK              (0xFF << REG_BUCK_RESERVED_SHIFT)
#define REG_BUCK_RESERVED(n)                BITFIELD_VAL(REG_BUCK_RESERVED, n)

// REG_13D
#define REG_BUCK_BURST_REF_LOW              (1 << 15)
#define REG_BUCK_COUNTER_SEL                (1 << 14)
#define REG_BUCK_SUM_CAPX2                  (1 << 13)
#define REG_BUCK_IX2_IRCOMP                 (1 << 12)
#define REG_BUCK_NSW_OFF_DELAY              (1 << 11)
#define REG_BUCK_CLAMPH_SEL                 (1 << 10)
#define REG_BUCK_IS_GAIN_ALON               (1 << 9)
#define REG_BUCK_ERR_SLEEP_EN               (1 << 8)
#define REG_BUCK_CHARGE_HALF                (1 << 7)
#define REG_BUCK_CLAMPL_SEL_SHIFT           5
#define REG_BUCK_CLAMPL_SEL_MASK            (0x3 << REG_BUCK_CLAMPL_SEL_SHIFT)
#define REG_BUCK_CLAMPL_SEL(n)              BITFIELD_VAL(REG_BUCK_CLAMPL_SEL, n)
#define REG_BUCK_RESET_SLEEP_COUNTER        (1 << 4)
#define REG_BUCK_IS_GAIN_RES                (1 << 3)

// REG_13E
#define REG_BUCK_OSC_CAP_SHIFT              8
#define REG_BUCK_OSC_CAP_MASK               (0x7 << REG_BUCK_OSC_CAP_SHIFT)
#define REG_BUCK_OSC_CAP(n)                 BITFIELD_VAL(REG_BUCK_OSC_CAP, n)
#define REG_BUCK_DCDC_AVDD25RES             (1 << 7)
#define REG_BUCK_CC_FINE_BIT_SHIFT          4
#define REG_BUCK_CC_FINE_BIT_MASK           (0x7 << REG_BUCK_CC_FINE_BIT_SHIFT)
#define REG_BUCK_CC_FINE_BIT(n)             BITFIELD_VAL(REG_BUCK_CC_FINE_BIT, n)
#define REG_BUCK_CC_CORSE_BIT_SHIFT         2
#define REG_BUCK_CC_CORSE_BIT_MASK          (0x3 << REG_BUCK_CC_CORSE_BIT_SHIFT)
#define REG_BUCK_CC_CORSE_BIT(n)            BITFIELD_VAL(REG_BUCK_CC_CORSE_BIT, n)
#define REG_BUCK_TEST_POWERMOS              (1 << 1)
#define REG_BUCK_CC_REDUCTION               (1 << 0)


// REG_146
#define REG_EFUSE_POWER_ON_READ_ADDRESS_SHIFT 6
#define REG_EFUSE_POWER_ON_READ_ADDRESS_MASK (0x1F << REG_EFUSE_POWER_ON_READ_ADDRESS_SHIFT)
#define REG_EFUSE_POWER_ON_READ_ADDRESS(n)  BITFIELD_VAL(REG_EFUSE_POWER_ON_READ_ADDRESS, n)
#define REG_EFUSE_POWER_ON_READ_INC_SHIFT   2
#define REG_EFUSE_POWER_ON_READ_INC_MASK    (0xF << REG_EFUSE_POWER_ON_READ_INC_SHIFT)
#define REG_EFUSE_POWER_ON_READ_INC(n)      BITFIELD_VAL(REG_EFUSE_POWER_ON_READ_INC, n)
#define REG_EFUSE_PWR_ON_RD_STR_DR          (1 << 1)
#define REG_EFUSE_PWR_ON_RD_STR             (1 << 0)

// REG_147
#define REG_EFUSE_CLK_OSC_SEL               (1 << 14)
#define REG_EFUSE_AUTO_READ                 (1 << 13)
#define REG_BASE_ADDR_SHIFT                 8
#define REG_BASE_ADDR_MASK                  (0x1F << REG_BASE_ADDR_SHIFT)
#define REG_BASE_ADDR(n)                    BITFIELD_VAL(REG_BASE_ADDR, n)
#define REG_ADDR_INC_SHIFT                  4
#define REG_ADDR_INC_MASK                   (0xF << REG_ADDR_INC_SHIFT)
#define REG_ADDR_INC(n)                     BITFIELD_VAL(REG_ADDR_INC, n)
#define REG_SQ_TIME_SHIFT                   0
#define REG_SQ_TIME_MASK                    (0xF << REG_SQ_TIME_SHIFT)
#define REG_SQ_TIME(n)                      BITFIELD_VAL(REG_SQ_TIME, n)

// REG_148
#define AUTO_ADDR0_DATA_SHIFT               0
#define AUTO_ADDR0_DATA_MASK                (0xFFFF << AUTO_ADDR0_DATA_SHIFT)
#define AUTO_ADDR0_DATA(n)                  BITFIELD_VAL(AUTO_ADDR0_DATA, n)

// REG_149
#define AUTO_ADDR1_DATA_SHIFT               0
#define AUTO_ADDR1_DATA_MASK                (0xFFFF << AUTO_ADDR1_DATA_SHIFT)
#define AUTO_ADDR1_DATA(n)                  BITFIELD_VAL(AUTO_ADDR1_DATA, n)

// REG_14A
#define AUTO_ADDR2_DATA_SHIFT               0
#define AUTO_ADDR2_DATA_MASK                (0xFFFF << AUTO_ADDR2_DATA_SHIFT)
#define AUTO_ADDR2_DATA(n)                  BITFIELD_VAL(AUTO_ADDR2_DATA, n)

// REG_14B
#define AUTO_ADDR3_DATA_SHIFT               0
#define AUTO_ADDR3_DATA_MASK                (0xFFFF << AUTO_ADDR3_DATA_SHIFT)
#define AUTO_ADDR3_DATA(n)                  BITFIELD_VAL(AUTO_ADDR3_DATA, n)

// REG_14C
#define AUTO_ADDR4_DATA_SHIFT               0
#define AUTO_ADDR4_DATA_MASK                (0xFFFF << AUTO_ADDR4_DATA_SHIFT)
#define AUTO_ADDR4_DATA(n)                  BITFIELD_VAL(AUTO_ADDR4_DATA, n)

// REG_14D
#define AUTO_ADDR5_DATA_SHIFT               0
#define AUTO_ADDR5_DATA_MASK                (0xFFFF << AUTO_ADDR5_DATA_SHIFT)
#define AUTO_ADDR5_DATA(n)                  BITFIELD_VAL(AUTO_ADDR5_DATA, n)

// REG_14E
#define AUTO_ADDR6_DATA_SHIFT               0
#define AUTO_ADDR6_DATA_MASK                (0xFFFF << AUTO_ADDR6_DATA_SHIFT)
#define AUTO_ADDR6_DATA(n)                  BITFIELD_VAL(AUTO_ADDR6_DATA, n)

// REG_14F
#define AUTO_ADDR7_DATA_SHIFT               0
#define AUTO_ADDR7_DATA_MASK                (0xFFFF << AUTO_ADDR7_DATA_SHIFT)
#define AUTO_ADDR7_DATA(n)                  BITFIELD_VAL(AUTO_ADDR7_DATA, n)

// REG_150
#define AUTO_ADDR8_DATA_SHIFT               0
#define AUTO_ADDR8_DATA_MASK                (0xFFFF << AUTO_ADDR8_DATA_SHIFT)
#define AUTO_ADDR8_DATA(n)                  BITFIELD_VAL(AUTO_ADDR8_DATA, n)

// REG_151
#define AUTO_ADDR9_DATA_SHIFT               0
#define AUTO_ADDR9_DATA_MASK                (0xFFFF << AUTO_ADDR9_DATA_SHIFT)
#define AUTO_ADDR9_DATA(n)                  BITFIELD_VAL(AUTO_ADDR9_DATA, n)

// REG_152
#define AUTO_ADDRA_DATA_SHIFT               0
#define AUTO_ADDRA_DATA_MASK                (0xFFFF << AUTO_ADDRA_DATA_SHIFT)
#define AUTO_ADDRA_DATA(n)                  BITFIELD_VAL(AUTO_ADDRA_DATA, n)

// REG_153
#define AUTO_ADDRB_DATA_SHIFT               0
#define AUTO_ADDRB_DATA_MASK                (0xFFFF << AUTO_ADDRB_DATA_SHIFT)
#define AUTO_ADDRB_DATA(n)                  BITFIELD_VAL(AUTO_ADDRB_DATA, n)

// REG_154
#define AUTO_ADDRC_DATA_SHIFT               0
#define AUTO_ADDRC_DATA_MASK                (0xFFFF << AUTO_ADDRC_DATA_SHIFT)
#define AUTO_ADDRC_DATA(n)                  BITFIELD_VAL(AUTO_ADDRC_DATA, n)

// REG_155
#define AUTO_ADDRD_DATA_SHIFT               0
#define AUTO_ADDRD_DATA_MASK                (0xFFFF << AUTO_ADDRD_DATA_SHIFT)
#define AUTO_ADDRD_DATA(n)                  BITFIELD_VAL(AUTO_ADDRD_DATA, n)

// REG_156
#define AUTO_ADDRE_DATA_SHIFT               0
#define AUTO_ADDRE_DATA_MASK                (0xFFFF << AUTO_ADDRE_DATA_SHIFT)
#define AUTO_ADDRE_DATA(n)                  BITFIELD_VAL(AUTO_ADDRE_DATA, n)

// REG_157
#define AUTO_ADDRF_DATA_SHIFT               0
#define AUTO_ADDRF_DATA_MASK                (0xFFFF << AUTO_ADDRF_DATA_SHIFT)
#define AUTO_ADDRF_DATA(n)                  BITFIELD_VAL(AUTO_ADDRF_DATA, n)

// REG_158
#define EFUSE_FSM_SHIFT                     0
#define EFUSE_FSM_MASK                      (0xF << EFUSE_FSM_SHIFT)
#define EFUSE_FSM(n)                        BITFIELD_VAL(EFUSE_FSM, n)


// REG_159
#define REG_EFUSE_FLAG_MSK_SHIFT            0
#define REG_EFUSE_FLAG_MSK_MASK             (0xF << REG_EFUSE_FLAG_MSK_SHIFT)
#define REG_EFUSE_FLAG_MSK(n)               BITFIELD_VAL(REG_EFUSE_FLAG_MSK, n)

#define REG_EFUSE_FLAG_TRIM_VBG_AUX             (1 << 0)
#define REG_EFUSE_FLAG_LDO_VRTC_VBIT_NORMAL     (1 << 1)
#define REG_EFUSE_FLAG_BUCK_VCORE_VBIT_NORMAL   (1 << 2)
#define REG_EFUSE_FLAG_LDO_VANA_VBIT_NORMAL     (1 << 3)

/* RTC_CALENDAR */
// REG_10A
#define RTC_MATCH_TIME_MIN_TENS_SHIFT       12
#define RTC_MATCH_TIME_MIN_TENS_MASK        (0x7 << RTC_MATCH_TIME_MIN_TENS_SHIFT)
#define RTC_MATCH_TIME_MIN_TENS(n)          BITFIELD_VAL(RTC_MATCH_TIME_MIN_TENS, n)
#define RTC_MATCH_TIME_MIN_UNITS_SHIFT      8
#define RTC_MATCH_TIME_MIN_UNITS_MASK       (0xF << RTC_MATCH_TIME_MIN_UNITS_SHIFT)
#define RTC_MATCH_TIME_MIN_UNITS(n)         BITFIELD_VAL(RTC_MATCH_TIME_MIN_UNITS, n)
#define RTC_MATCH_TIME_SEC_TENS_SHIFT       4
#define RTC_MATCH_TIME_SEC_TENS_MASK        (0x7 << RTC_MATCH_TIME_SEC_TENS_SHIFT)
#define RTC_MATCH_TIME_SEC_TENS(n)          BITFIELD_VAL(RTC_MATCH_TIME_SEC_TENS, n)
#define RTC_MATCH_TIME_SEC_UNITS_SHIFT      0
#define RTC_MATCH_TIME_SEC_UNITS_MASK       (0xF << RTC_MATCH_TIME_SEC_UNITS_SHIFT)
#define RTC_MATCH_TIME_SEC_UNITS(n)         BITFIELD_VAL(RTC_MATCH_TIME_SEC_UNITS, n)

// REG_10B
#define RTC_MATCH_HOURS_TENS_SHIFT          4
#define RTC_MATCH_HOURS_TENS_MASK           (0x3 << RTC_MATCH_HOURS_TENS_SHIFT)
#define RTC_MATCH_HOURS_TENS(n)             BITFIELD_VAL(RTC_MATCH_HOURS_TENS, n)
#define RTC_MATCH_HOURS_UNITS_SHIFT         0
#define RTC_MATCH_HOURS_UNITS_MASK          (0xF << RTC_MATCH_HOURS_UNITS_SHIFT)
#define RTC_MATCH_HOURS_UNITS(n)            BITFIELD_VAL(RTC_MATCH_HOURS_UNITS, n)

// REG_10C
#define RTC_MATCH_DATAS_WEEK_SHIFT          13
#define RTC_MATCH_DATAS_WEEK_MASK           (0x7 << RTC_MATCH_DATAS_WEEK_SHIFT)
#define RTC_MATCH_DATAS_WEEK(n)             BITFIELD_VAL(RTC_MATCH_DATAS_WEEK, n)
#define RTC_MATCH_DATAS_MONTH_TENS          (1 << 12)
#define RTC_MATCH_DATAS_MONTH_UNITS_SHIFT   8
#define RTC_MATCH_DATAS_MONTH_UNITS_MASK    (0xF << RTC_MATCH_DATAS_MONTH_UNITS_SHIFT)
#define RTC_MATCH_DATAS_MONTH_UNITS(n)      BITFIELD_VAL(RTC_MATCH_DATAS_MONTH_UNITS, n)
#define RTC_MATCH_DATAS_DAY_TENS_SHIFT      4
#define RTC_MATCH_DATAS_DAY_TENS_MASK       (0x3 << RTC_MATCH_DATAS_DAY_TENS_SHIFT)
#define RTC_MATCH_DATAS_DAY_TENS(n)         BITFIELD_VAL(RTC_MATCH_DATAS_DAY_TENS, n)
#define RTC_MATCH_DATAS_DAY_UNITS_SHIFT     0
#define RTC_MATCH_DATAS_DAY_UNITS_MASK      (0xF << RTC_MATCH_DATAS_DAY_UNITS_SHIFT)
#define RTC_MATCH_DATAS_DAY_UNITS(n)        BITFIELD_VAL(RTC_MATCH_DATAS_DAY_UNITS, n)

// REG_10D
#define RTC_MATCH_YEARS_TENS_SHIFT          4
#define RTC_MATCH_YEARS_TENS_MASK           (0xF << RTC_MATCH_YEARS_TENS_SHIFT)
#define RTC_MATCH_YEARS_TENS(n)             BITFIELD_VAL(RTC_MATCH_YEARS_TENS, n)
#define RTC_MATCH_YEARS_UNITS_SHIFT         0
#define RTC_MATCH_YEARS_UNITS_MASK          (0xF << RTC_MATCH_YEARS_UNITS_SHIFT)
#define RTC_MATCH_YEARS_UNITS(n)            BITFIELD_VAL(RTC_MATCH_YEARS_UNITS, n)

// REG_10F
#define RTC_LOAD_TIME_MIN_TENS_SHIFT        12
#define RTC_LOAD_TIME_MIN_TENS_MASK         (0x7 << RTC_LOAD_TIME_MIN_TENS_SHIFT)
#define RTC_LOAD_TIME_MIN_TENS(n)           BITFIELD_VAL(RTC_LOAD_TIME_MIN_TENS, n)
#define RTC_LOAD_TIME_MIN_UNITS_SHIFT       8
#define RTC_LOAD_TIME_MIN_UNITS_MASK        (0xF << RTC_LOAD_TIME_MIN_UNITS_SHIFT)
#define RTC_LOAD_TIME_MIN_UNITS(n)          BITFIELD_VAL(RTC_LOAD_TIME_MIN_UNITS, n)
#define RTC_LOAD_TIME_SEC_TENS_SHIFT        4
#define RTC_LOAD_TIME_SEC_TENS_MASK         (0x7 << RTC_LOAD_TIME_SEC_TENS_SHIFT)
#define RTC_LOAD_TIME_SEC_TENS(n)           BITFIELD_VAL(RTC_LOAD_TIME_SEC_TENS, n)
#define RTC_LOAD_TIME_SEC_UNITS_SHIFT       0
#define RTC_LOAD_TIME_SEC_UNITS_MASK        (0xF << RTC_LOAD_TIME_SEC_UNITS_SHIFT)
#define RTC_LOAD_TIME_SEC_UNITS(n)          BITFIELD_VAL(RTC_LOAD_TIME_SEC_UNITS, n)

// REG_110
#define RTC_LOAD_HOURS_TENS_SHIFT           4
#define RTC_LOAD_HOURS_TENS_MASK            (0x3 << RTC_LOAD_HOURS_TENS_SHIFT)
#define RTC_LOAD_HOURS_TENS(n)              BITFIELD_VAL(RTC_LOAD_HOURS_TENS, n)
#define RTC_LOAD_HOURS_UNITS_SHIFT          0
#define RTC_LOAD_HOURS_UNITS_MASK           (0xF << RTC_LOAD_HOURS_UNITS_SHIFT)
#define RTC_LOAD_HOURS_UNITS(n)             BITFIELD_VAL(RTC_LOAD_HOURS_UNITS, n)

// REG_111
#define RTC_LOAD_DATAS_WEEK_SHIFT           13
#define RTC_LOAD_DATAS_WEEK_MASK            (0x7 << RTC_LOAD_DATAS_WEEK_SHIFT)
#define RTC_LOAD_DATAS_WEEK(n)              BITFIELD_VAL(RTC_LOAD_DATAS_WEEK, n)
#define RTC_LOAD_DATAS_MONTH_TENS           (1 << 12)
#define RTC_LOAD_DATAS_MONTH_UNITS_SHIFT    8
#define RTC_LOAD_DATAS_MONTH_UNITS_MASK     (0xF << RTC_LOAD_DATAS_MONTH_UNITS_SHIFT)
#define RTC_LOAD_DATAS_MONTH_UNITS(n)       BITFIELD_VAL(RTC_LOAD_DATAS_MONTH_UNITS, n)
#define RTC_LOAD_DATAS_DAY_TENS_SHIFT       4
#define RTC_LOAD_DATAS_DAY_TENS_MASK        (0x3 << RTC_LOAD_DATAS_DAY_TENS_SHIFT)
#define RTC_LOAD_DATAS_DAY_TENS(n)          BITFIELD_VAL(RTC_LOAD_DATAS_DAY_TENS, n)
#define RTC_LOAD_DATAS_DAY_UNITS_SHIFT      0
#define RTC_LOAD_DATAS_DAY_UNITS_MASK       (0xF << RTC_LOAD_DATAS_DAY_UNITS_SHIFT)
#define RTC_LOAD_DATAS_DAY_UNITS(n)         BITFIELD_VAL(RTC_LOAD_DATAS_DAY_UNITS, n)

// REG_112
#define RTC_LOAD_YEARS_TENS_SHIFT           4
#define RTC_LOAD_YEARS_TENS_MASK            (0xF << RTC_LOAD_YEARS_TENS_SHIFT)
#define RTC_LOAD_YEARS_TENS(n)              BITFIELD_VAL(RTC_LOAD_YEARS_TENS, n)
#define RTC_LOAD_YEARS_UNITS_SHIFT          0
#define RTC_LOAD_YEARS_UNITS_MASK           (0xF << RTC_LOAD_YEARS_UNITS_SHIFT)
#define RTC_LOAD_YEARS_UNITS(n)             BITFIELD_VAL(RTC_LOAD_YEARS_UNITS, n)

// REG_113
#define RTC_GET_TIME_MIN_TENS_SHIFT         12
#define RTC_GET_TIME_MIN_TENS_MASK          (0x7 << RTC_GET_TIME_MIN_TENS_SHIFT)
#define RTC_GET_TIME_MIN_TENS(n)            BITFIELD_VAL(RTC_GET_TIME_MIN_TENS, n)
#define RTC_GET_TIME_MIN_UNITS_SHIFT        8
#define RTC_GET_TIME_MIN_UNITS_MASK         (0xF << RTC_GET_TIME_MIN_UNITS_SHIFT)
#define RTC_GET_TIME_MIN_UNITS(n)           BITFIELD_VAL(RTC_GET_TIME_MIN_UNITS, n)
#define RTC_GET_TIME_SEC_TENS_SHIFT         4
#define RTC_GET_TIME_SEC_TENS_MASK          (0x7 << RTC_GET_TIME_SEC_TENS_SHIFT)
#define RTC_GET_TIME_SEC_TENS(n)            BITFIELD_VAL(RTC_GET_TIME_SEC_TENS, n)
#define RTC_GET_TIME_SEC_UNITS_SHIFT        0
#define RTC_GET_TIME_SEC_UNITS_MASK         (0xF << RTC_GET_TIME_SEC_UNITS_SHIFT)
#define RTC_GET_TIME_SEC_UNITS(n)           BITFIELD_VAL(RTC_GET_TIME_SEC_UNITS, n)

// REG_114
#define RTC_GET_HOURS_TENS_SHIFT            4
#define RTC_GET_HOURS_TENS_MASK             (0x3 << RTC_GET_HOURS_TENS_SHIFT)
#define RTC_GET_HOURS_TENS(n)               BITFIELD_VAL(RTC_GET_HOURS_TENS, n)
#define RTC_GET_HOURS_UNITS_SHIFT           0
#define RTC_GET_HOURS_UNITS_MASK            (0xF << RTC_GET_HOURS_UNITS_SHIFT)
#define RTC_GET_HOURS_UNITS(n)              BITFIELD_VAL(RTC_GET_HOURS_UNITS, n)

// REG_115
#define RTC_GET_DATAS_WEEK_SHIFT            13
#define RTC_GET_DATAS_WEEK_MASK             (0x7 << RTC_GET_DATAS_WEEK_SHIFT)
#define RTC_GET_DATAS_WEEK(n)               BITFIELD_VAL(RTC_GET_DATAS_WEEK, n)
#define RTC_GET_DATAS_MONTH_TENS            (1 << 12)
#define RTC_GET_DATAS_MONTH_UNITS_SHIFT     8
#define RTC_GET_DATAS_MONTH_UNITS_MASK      (0xF << RTC_GET_DATAS_MONTH_UNITS_SHIFT)
#define RTC_GET_DATAS_MONTH_UNITS(n)        BITFIELD_VAL(RTC_GET_DATAS_MONTH_UNITS, n)
#define RTC_GET_DATAS_DAY_TENS_SHIFT        4
#define RTC_GET_DATAS_DAY_TENS_MASK         (0x3 << RTC_GET_DATAS_DAY_TENS_SHIFT)
#define RTC_GET_DATAS_DAY_TENS(n)           BITFIELD_VAL(RTC_GET_DATAS_DAY_TENS, n)
#define RTC_GET_DATAS_DAY_UNITS_SHIFT       0
#define RTC_GET_DATAS_DAY_UNITS_MASK        (0xF << RTC_GET_DATAS_DAY_UNITS_SHIFT)
#define RTC_GET_DATAS_DAY_UNITS(n)          BITFIELD_VAL(RTC_GET_DATAS_DAY_UNITS, n)

// REG_116
#define RTC_GET_YEARS_TENS_SHIFT            4
#define RTC_GET_YEARS_TENS_MASK             (0xF << RTC_GET_YEARS_TENS_SHIFT)
#define RTC_GET_YEARS_TENS(n)               BITFIELD_VAL(RTC_GET_YEARS_TENS, n)
#define RTC_GET_YEARS_UNITS_SHIFT           0
#define RTC_GET_YEARS_UNITS_MASK            (0xF << RTC_GET_YEARS_UNITS_SHIFT)
#define RTC_GET_YEARS_UNITS(n)              BITFIELD_VAL(RTC_GET_YEARS_UNITS, n)
/* RTC_CALENDAR END */

#endif

