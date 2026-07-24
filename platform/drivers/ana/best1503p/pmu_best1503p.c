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
#include "pmu.h"
#include CHIP_SPECIFIC_HDR(reg_pmu)
#include CHIP_SPECIFIC_HDR(hal_cmu_pri)
#include CHIP_SPECIFIC_HDR(rc)
#include "analog.h"
#include "cmsis.h"
#include "cmsis_nvic.h"
#include "hal_aud.h"
#include "hal_bootmode.h"
#include "hal_cache.h"
#include "hal_chipid.h"
#include "hal_cmu.h"
#include "hal_location.h"
#include "hal_sleep.h"
#include "hal_sysfreq.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "patch.h"
#include "tgt_hardware.h"
#ifdef PMU_NTC_MONITOR
#ifndef NTC_MONITOR_USE_OSTIMER
#include "hwtimer_list.h"
#endif
#endif
#include "system_utils.h"

#define PMU_EFUSE_INIT_IN_BOOT
#define VRTC_SWITCH_TO_VCORE

#if !defined(DIG_DCDC_MODE) && !defined(PMU_LDO_VCORE_CALIB)
#define PMU_LDO_VCORE_CALIB
#endif

#if defined(PSRAM_ENABLE) || defined(PMU_ALREADY_INIT)
#define PMU_OPEN_BSS_LOC                BOOT_BSS_LOC
#else
#define PMU_OPEN_BSS_LOC
#endif

#ifdef PMU_IRQ_UNIFIED
#define PMU_IRQ_HDLR_PARAM              uint16_t irq_status
#else
#define PMU_IRQ_HDLR_PARAM              void
#endif

#define ana_read(reg,val)               hal_analogif_reg_read(ANA_REG(reg),val)
#define ana_write(reg,val)              hal_analogif_reg_write(ANA_REG(reg),val)
#define rf_read(reg,val)                hal_analogif_reg_read(reg,val)
#define rf_write(reg,val)               hal_analogif_reg_write(reg,val)

// LDO soft start interval is about 1000 us
#define PMU_LDO_PU_STABLE_TIME_US       1800
#define PMU_LDO_PULLDOWN_STABLE_TIME_US 1000
#define PMU_LDO_VRTC_STABLE_TIME_US     1000
#define PMU_DCDC_PU_STABLE_TIME_US      100
#define PMU_VANA_STABLE_TIME_US         10
#define PMU_VCORE_STABLE_TIME_US        10
#define PMU_BIG_BG_STABLE_TIME_US       200
#define PMU_GPADC_VREF_STABLE_TIME_US   100

#define PMU_HW_RAMP_PER_CLK_TIME_US     40
#define PMU_BUCK_VHPPA_HW_RAMP_STEP     8
#define PMU_BUCK_VANA_HW_RAMP_STEP      8
#define PMU_BUCK_VCORE_HW_RAMP_STEP     16
#define PMU_VCORE_L_HW_RAMP_STEP        8

// External 32.768KHz stable time(ms)
#define PMU_EXT_32K_STABLE_TIME         300
#define PMU_EXT_32K_CLK_OUT_STABLE_TIME 1
#define PMU_CLK_SWITCH_STABLE_TIME      1
#define PMU_EXT_32K_CAPBIT_DEFAULT      0x141

#define RCX8_FREQ_MHZ                   384
#define RCX8_CALIB_MODE                 0
#define RCX8_CALIB_TIMEOUT_MS           10
#define RC2M_CALIB_TIMEOUT_MS           15

#define RCX8_PU_STABLE_TIME_US          10
#define RC2M_PU_STABLE_TIME_US          10

// 1.9mV
#define PMU_DCDC_DIG_1_1V               0x198
#define PMU_DCDC_DIG_1_05V              0x17E
#define PMU_DCDC_DIG_1_0V               0x163
#define PMU_DCDC_DIG_0_95V              0x149
#define PMU_DCDC_DIG_0_9V               0x12F
#define PMU_DCDC_DIG_0_88V              0x125
#define PMU_DCDC_DIG_0_85V              0x114
#define PMU_DCDC_DIG_0_84V              0x10F
#define PMU_DCDC_DIG_0_8V               0xFA
#define PMU_DCDC_DIG_0_78V              0xF0
#define PMU_DCDC_DIG_0_75V              0xE0
#define PMU_DCDC_DIG_0_74V              0xDA
#define PMU_DCDC_DIG_0_7V               0xC5
#define PMU_DCDC_DIG_0_65V              0xAB
#define PMU_DCDC_DIG_0_6V               0x91
#define PMU_DCDC_DIG_0_55V              0x76
#define PMU_DCDC_DIG_0_5V               0x5C
#define PMU_DCDC_DIG_50_MV              (PMU_DCDC_DIG_0_85V - PMU_DCDC_DIG_0_8V)
#define PMU_DCDC_DIG_DEFAULT            0x140

// 3.8mV
#define PMU_DCDC_ANA_1_4V               0xCF
#define PMU_DCDC_ANA_1_3V               0xB5
#define PMU_DCDC_ANA_1_2V               0x9A
#define PMU_DCDC_ANA_1_1V               0x80
#define PMU_DCDC_ANA_1_0V               0x66

// 3.8mV
#define PMU_DCDC_HPPA_1_9V              0x151
#define PMU_DCDC_HPPA_1_8V              0x138
#define PMU_DCDC_HPPA_1_75V             0x12A
#define PMU_DCDC_HPPA_1_7V              0x11D
#define PMU_DCDC_HPPA_1_65V             0x110
#define PMU_DCDC_HPPA_1_6V              0x103
#define PMU_DCDC_HPPA_1_5V              0xEA
#define PMU_DCDC_HPPA_1_4V              0xCF

#define PMU_VDIG_1_1V                   0x9
#define PMU_VDIG_1_05V                  0x8
#define PMU_VDIG_1_0V                   0x7
#define PMU_VDIG_0_95V                  0x6
#define PMU_VDIG_0_9V                   0x5
#define PMU_VDIG_0_85V                  0x4
#define PMU_VDIG_0_8V                   0x3
#define PMU_VDIG_0_75V                  0x2
#define PMU_VDIG_0_7V                   0x1
#define PMU_VDIG_0_65V                  0x0
#define PMU_VDIG_DEFAULT                0x5 /* 0.9V */

// 3mV
#define PMU_VDIG_L_1_05V                0xFA
#define PMU_VDIG_L_1_0V                 0xE9
#define PMU_VDIG_L_0_95V                0xD8
#define PMU_VDIG_L_0_9V                 0xC8
#define PMU_VDIG_L_0_85V                0xB7
#define PMU_VDIG_L_0_8V                 0xA6
#define PMU_VDIG_L_0_75V                0x96
#define PMU_VDIG_L_0_7V                 0x85
#define PMU_VDIG_L_0_65V                0x74
#define PMU_VDIG_L_0_6V                 0x64
#define PMU_VDIG_L_0_55V                0x53
#define PMU_VDIG_L_0_5V                 0x42
#define PMU_VDIG_L_0_45V                0x32
#define PMU_VDIG_L_DEFAULT              0x64

#define PMU_LDO_SENSOR_3_3V             0x28
#define PMU_LDO_SENSOR_3_2V             0x26
#define PMU_LDO_SENSOR_3_1V             0x24
#define PMU_LDO_SENSOR_3_0V             0x22
#define PMU_LDO_SENSOR_2_9V             0x20
#define PMU_LDO_SENSOR_2_8V             0x1E
#define PMU_LDO_SENSOR_2_7V             0x1C
#define PMU_LDO_SENSOR_2_6V             0x1B
#define PMU_LDO_SENSOR_2_5V             0x19
#define PMU_LDO_SENSOR_2_4V             0x17
#define PMU_LDO_SENSOR_2_3V             0x15
#define PMU_LDO_SENSOR_2_2V             0x13
#define PMU_LDO_SENSOR_2_1V             0x11
#define PMU_LDO_SENSOR_2_0V             0xF
#define PMU_LDO_SENSOR_1_9V             0xD
#define PMU_LDO_SENSOR_1_8V             0xB

#define PMU_LDO_VRTC_0_825V             0xF
#define PMU_LDO_VRTC_0_8V               0xE
#define PMU_LDO_VRTC_0_775V             0xD
#define PMU_LDO_VRTC_0_75V              0xC
#define PMU_LDO_VRTC_0_725V             0xB
#define PMU_LDO_VRTC_0_7V               0xA
#define PMU_LDO_VRTC_0_675V             0x9
#define PMU_LDO_VRTC_0_65V              0x8
#define PMU_LDO_VRTC_0_625V             0x7
#define PMU_LDO_VRTC_0_6V               0x6
#define PMU_LDO_VRTC_0_575V             0x5
#define PMU_LDO_VRTC_0_55V              0x4
#define PMU_LDO_VRTC_0_525V             0x3
#define PMU_LDO_VRTC_0_5V               0x2
#define PMU_LDO_VRTC_0_475V             0x1
#define PMU_LDO_VRTC_0_45V              0x0
#define PMU_LDO_VRTC_DEFAULT_ACT        0xE
#define PMU_LDO_VRTC_DEFAULT_LP         0x8

#define PMU_VMIC_2_8V                   0x33
#define PMU_VMIC_2_7V                   0x30
#define PMU_VMIC_2_6V                   0x2D
#define PMU_VMIC_1_6V                   0x13

#define PMU_VMIC_RES_3_3V               0xF
#define PMU_VMIC_RES_2_8V               0xA

#define PMU_CODEC_1_9V                  0x7
#define PMU_CODEC_1_8V                  0x6
#define PMU_CODEC_1_7V                  0x5
#define PMU_CODEC_1_6V                  0x4
#define PMU_CODEC_DEFAULT_V             0x6

#define PMU_VMEM_1_8V                   0xC
#define PMU_VMEM_1_7V                   0xA
#define PMU_VMEM_1_6V                   0x8
#define PMU_VMEM_DEFAULT_V              0xC

#define PMU_USB_3_3V                    0xC
#define PMU_USB_3_2V                    0xB
#define PMU_USB_3_1V                    0xA
#define PMU_USB_3_0V                    0x9
#define PMU_USB_2_9V                    0x8
#define PMU_USB_2_8V                    0x7
#define PMU_USB_2_7V                    0x6
#define PMU_USB_2_6V                    0x5
#define PMU_USB_2_5V                    0x4
#define PMU_USB_2_4V                    0x3

#define MAX_BUCK_VANA_BIT_VAL                   (REG_BUCK_VANA_VBIT_NORMAL_MASK >> REG_BUCK_VANA_VBIT_NORMAL_SHIFT)
#define MAX_BUCK_VCORE_BIT_VAL                  (REG_BUCK_VCORE_VBIT_NORMAL_MASK >> REG_BUCK_VCORE_VBIT_NORMAL_SHIFT)
#define MAX_BUCK_VHPPA_BIT_VAL                  (REG_BUCK_VHPPA_VBIT_NORMAL_MASK >> REG_BUCK_VHPPA_VBIT_NORMAL_SHIFT)
#define MAX_LDO_VCORE_L_BIT_VAL                 (REG_LDO_VCORE_L_VBIT_NORMAL_MASK >> REG_LDO_VCORE_L_VBIT_NORMAL_SHIFT)

#define INTR_MSKED_CHARGE_OUT                   INTR_MSKED_CHARGE(1 << 1)
#define INTR_MSKED_CHARGE_IN                    INTR_MSKED_CHARGE(1 << 0)

#define REG_DCDC1_RAMP_EN                       (1 << 1)
#define REG_DCDC2_RAMP_EN                       (1 << 2)
#define REG_DCDC3_RAMP_EN                       (1 << 3)
#define REG_VCORE_L_RAMP_EN                     (1 << 4)

#define CHIP_REV_ID_SHIFT                       0
#define CHIP_REV_ID_MASK                        (0xF << CHIP_REV_ID_SHIFT)
#define CHIP_REV_ID(n)                          BITFIELD_VAL(CHIP_REV_ID, n)

/* PMU_EFUSE */
// PMU_VMIN_SEARCH
#define PMU_VMIN_SET_REG_ADDR                   PMU_REG_RTC_LOAD_MIN_SEC_EN
#define PMU_VMIN_VCORE_SET_DONE                 (1 << 15)
#define PMU_VMIN_VCORE_SET_PLUS                 (1 << 14)
#define PMU_VMIN_VCORE_SET_OFFSET_SHIFT         0
#define PMU_VMIN_VCORE_SET_OFFSET_MASK          (0x1F << PMU_VMIN_VCORE_SET_OFFSET_SHIFT)
#define PMU_VMIN_VCORE_SET_OFFSET(n)            BITFIELD_VAL(PMU_VMIN_VCORE_SET_OFFSET, n)

// PMU_EFUSE_PAGE_8
#define PMU_EFUSE_DCDC1_VOLT_COMP_SHIFT         0
#define PMU_EFUSE_DCDC1_VOLT_COMP_MASK          (0x7F << PMU_EFUSE_DCDC1_VOLT_COMP_SHIFT)
#define PMU_EFUSE_DCDC1_VOLT_COMP(n)            BITFIELD_VAL(PMU_EFUSE_DCDC1_VOLT_COMP, n)
#define PMU_EFUSE_DCDC1_VOLT_COMP_SIGN          (1 << 7)
#define PMU_EFUSE_DCDC1_VOLT_LP_COMP_SHIFT      8
#define PMU_EFUSE_DCDC1_VOLT_LP_COMP_MASK       (0x7F << PMU_EFUSE_DCDC1_VOLT_LP_COMP_SHIFT)
#define PMU_EFUSE_DCDC1_VOLT_LP_COMP(n)         BITFIELD_VAL(PMU_EFUSE_DCDC1_VOLT_LP_COMP, n)
#define PMU_EFUSE_DCDC1_VOLT_LP_COMP_SIGN       (1 << 15)

// PMU_EFUSE_PAGE_9
#define PMU_EFUSE_DCDC2_VOLT_COMP_SHIFT         0
#define PMU_EFUSE_DCDC2_VOLT_COMP_MASK          (0x3F << PMU_EFUSE_DCDC2_VOLT_COMP_SHIFT)
#define PMU_EFUSE_DCDC2_VOLT_COMP(n)            BITFIELD_VAL(PMU_EFUSE_DCDC2_VOLT_COMP, n)
#define PMU_EFUSE_DCDC2_VOLT_COMP_SIGN          (1 << 6)
#define PMU_EFUSE_DCDC2_VOLT_LP_COMP_SHIFT      7
#define PMU_EFUSE_DCDC2_VOLT_LP_COMP_MASK       (0x3F << PMU_EFUSE_DCDC2_VOLT_LP_COMP_SHIFT)
#define PMU_EFUSE_DCDC2_VOLT_LP_COMP(n)         BITFIELD_VAL(PMU_EFUSE_DCDC2_VOLT_LP_COMP, n)
#define PMU_EFUSE_DCDC2_VOLT_LP_COMP_SIGN       (1 << 13)

// PMU_EFUSE_PAGE_A
#define PMU_EFUSE_DCDC3_VOLT_COMP_SHIFT         0
#define PMU_EFUSE_DCDC3_VOLT_COMP_MASK          (0x3F << PMU_EFUSE_DCDC3_VOLT_COMP_SHIFT)
#define PMU_EFUSE_DCDC3_VOLT_COMP(n)            BITFIELD_VAL(PMU_EFUSE_DCDC3_VOLT_COMP, n)
#define PMU_EFUSE_DCDC3_VOLT_COMP_SIGN          (1 << 6)
#define PMU_EFUSE_DCDC3_VOLT_LP_COMP_SHIFT      7
#define PMU_EFUSE_DCDC3_VOLT_LP_COMP_MASK       (0x3F << PMU_EFUSE_DCDC3_VOLT_LP_COMP_SHIFT)
#define PMU_EFUSE_DCDC3_VOLT_LP_COMP(n)         BITFIELD_VAL(PMU_EFUSE_DCDC3_VOLT_LP_COMP, n)
#define PMU_EFUSE_DCDC3_VOLT_LP_COMP_SIGN       (1 << 13)

// PMU_EFUSE_PAGE_D
#define PMU_EFUSE_LDO_CORE_L_VOLT_COMP_SHIFT    0
#define PMU_EFUSE_LDO_CORE_L_VOLT_COMP_MASK     (0x3F << PMU_EFUSE_LDO_CORE_L_VOLT_COMP_SHIFT)
#define PMU_EFUSE_LDO_CORE_L_VOLT_COMP(n)       BITFIELD_VAL(PMU_EFUSE_LDO_CORE_L_VOLT_COMP, n)
#define PMU_EFUSE_LDO_CORE_L_VOLT_COMP_SIGN     (1 << 6)
#define PMU_EFUSE_LDO_CORE_L_VOLT_LP_COMP_SHIFT 7
#define PMU_EFUSE_LDO_CORE_L_VOLT_LP_COMP_MASK  (0x3F << PMU_EFUSE_LDO_CORE_L_VOLT_LP_COMP_SHIFT)
#define PMU_EFUSE_LDO_CORE_L_VOLT_LP_COMP(n)    BITFIELD_VAL(PMU_EFUSE_LDO_CORE_L_VOLT_LP_COMP, n)
#define PMU_EFUSE_LDO_CORE_L_VOLT_LP_COMP_SIGN  (1 << 13)

// REG_CP_WAFER_CTX_XY
#define PMU_EFUSE_CP_WAFER_CTX_X_SHIFT          0
#define PMU_EFUSE_CP_WAFER_CTX_X_MASK           (0x7F << PMU_EFUSE_CP_WAFER_CTX_X_SHIFT)
#define PMU_EFUSE_CP_WAFER_CTX_X(n)             BITFIELD_VAL(PMU_EFUSE_CP_WAFER_CTX_X, n)
#define PMU_EFUSE_CP_WAFER_CTX_Y_SHIFT          7
#define PMU_EFUSE_CP_WAFER_CTX_Y_MASK           (0x7F << PMU_EFUSE_CP_WAFER_CTX_Y_SHIFT)
#define PMU_EFUSE_CP_WAFER_CTX_Y(n)             BITFIELD_VAL(PMU_EFUSE_CP_WAFER_CTX_Y, n)

// REG_CP_WAFER_CTX_ID
#define PMU_EFUSE_CP_WAFER_CTX_ID_SHIFT         0
#define PMU_EFUSE_CP_WAFER_CTX_ID_MASK          (0x1F << PMU_EFUSE_CP_WAFER_CTX_ID_SHIFT)
#define PMU_EFUSE_CP_WAFER_CTX_ID(n)            BITFIELD_VAL(PMU_EFUSE_CP_WAFER_CTX_ID, n)

// PMU_EFUSE_PAGE_14
#define PMU_EFUSE_SAR_ADC_VREF_COMP_SHIFT       0
#define PMU_EFUSE_SAR_ADC_VREF_COMP_MASK        (0x1F << PMU_EFUSE_SAR_ADC_VREF_COMP_SHIFT)
#define PMU_EFUSE_SAR_ADC_VREF_COMP(n)          BITFIELD_VAL(PMU_EFUSE_SAR_ADC_VREF_COMP, n)
#define PMU_EFUSE_SAR_ADC_VREF_COMP_SIGN        (1 << 5)

// PMU_EFUSE_PAGE_15
#define PMU_EFUSE_BUCK_VCORE_IS_GAIN_SHIFT      0
#define PMU_EFUSE_BUCK_VCORE_IS_GAIN_MASK       (0xF << PMU_EFUSE_BUCK_VCORE_IS_GAIN_SHIFT)
#define PMU_EFUSE_BUCK_VCORE_IS_GAIN(n)         BITFIELD_VAL(PMU_EFUSE_BUCK_VCORE_IS_GAIN, n)
#define PMU_EFUSE_BUCK_VANA_IS_GAIN_SHIFT       5
#define PMU_EFUSE_BUCK_VANA_IS_GAIN_MASK        (0xF << PMU_EFUSE_BUCK_VANA_IS_GAIN_SHIFT)
#define PMU_EFUSE_BUCK_VANA_IS_GAIN(n)          BITFIELD_VAL(PMU_EFUSE_BUCK_VANA_IS_GAIN, n)
#define PMU_EFUSE_BUCK_VHPPA_IS_GAIN_SHIFT      10
#define PMU_EFUSE_BUCK_VHPPA_IS_GAIN_MASK       (0xF << PMU_EFUSE_BUCK_VHPPA_IS_GAIN_SHIFT)
#define PMU_EFUSE_BUCK_VHPPA_IS_GAIN(n)         BITFIELD_VAL(PMU_EFUSE_BUCK_VHPPA_IS_GAIN, n)

// PMU_EFUSE_PAGE_17
#define PMU_EFUSE_DCDC1_VOLT_CAL_NEW            (1 << 8)

// PMU_EFUSE_PAGE_18
#define PMU_EFUSE_BUCK_VCORE_BURST_THRESHOLD_SHIFT  0
#define PMU_EFUSE_BUCK_VCORE_BURST_THRESHOLD_MASK   (0x1F << PMU_EFUSE_BUCK_VCORE_BURST_THRESHOLD_SHIFT)
#define PMU_EFUSE_BUCK_VCORE_BURST_THRESHOLD(n)     BITFIELD_VAL(PMU_EFUSE_BUCK_VCORE_BURST_THRESHOLD, n)
#define PMU_EFUSE_BUCK_VANA_BURST_THRESHOLD_SHIFT   5
#define PMU_EFUSE_BUCK_VANA_BURST_THRESHOLD_MASK    (0x1F << PMU_EFUSE_BUCK_VANA_BURST_THRESHOLD_SHIFT)
#define PMU_EFUSE_BUCK_VANA_BURST_THRESHOLD(n)      BITFIELD_VAL(PMU_EFUSE_BUCK_VANA_BURST_THRESHOLD, n)
#define PMU_EFUSE_BUCK_VHPPA_BURST_THRESHOLD_SHIFT  10
#define PMU_EFUSE_BUCK_VHPPA_BURST_THRESHOLD_MASK   (0x1F << PMU_EFUSE_BUCK_VHPPA_BURST_THRESHOLD_SHIFT)
#define PMU_EFUSE_BUCK_VHPPA_BURST_THRESHOLD(n)     BITFIELD_VAL(PMU_EFUSE_BUCK_VHPPA_BURST_THRESHOLD, n)
/* PMU_EFUSE END */

// RF_REG_F5
#define REG_BT_BBPLL_DIVN_CODEC_SHIFT           0
#define REG_BT_BBPLL_DIVN_CODEC_MASK            (0x1F << REG_BT_BBPLL_DIVN_CODEC_SHIFT)
#define REG_BT_BBPLL_DIVN_CODEC(n)              BITFIELD_VAL(REG_BT_BBPLL_DIVN_CODEC, n)
#define REG_RXCALI_REVERSE                      (1 << 5)
#define REG_SIG_LOSS_SHIFT                      6
#define REG_SIG_LOSS_MASK                       (0xF << REG_SIG_LOSS_SHIFT)
#define REG_SIG_LOSS(n)                         BITFIELD_VAL(REG_SIG_LOSS, n)
#define REG_BT_IPTAT_ISEL_SHIFT                 10
#define REG_BT_IPTAT_ISEL_MASK                  (0x3F << REG_BT_IPTAT_ISEL_SHIFT)
#define REG_BT_IPTAT_ISEL(n)                    BITFIELD_VAL(REG_BT_IPTAT_ISEL, n)

// REG_100C
#define REG_XTAL_LDO_VTUNE_OTHER_SHIFT          7
#define REG_XTAL_LDO_VTUNE_OTHER_MASK           (0x7 << REG_XTAL_LDO_VTUNE_OTHER_SHIFT)
#define REG_XTAL_LDO_VTUNE_OTHER(n)             BITFIELD_VAL(REG_XTAL_LDO_VTUNE_OTHER, n)

// REG_100E
#define REG_XTAL_MDLL_CP_SWRC_SHIFT             0
#define REG_XTAL_MDLL_CP_SWRC_MASK              (0x7 << REG_XTAL_MDLL_CP_SWRC_SHIFT)
#define REG_XTAL_MDLL_CP_SWRC(n)                BITFIELD_VAL(REG_XTAL_MDLL_CP_SWRC, n)
#define REG_XTAL_MDLL_CLK_ADC_SEL_I_4M_SHIFT    13
#define REG_XTAL_MDLL_CLK_ADC_SEL_I_4M_MASK     (0x3 << REG_XTAL_MDLL_CLK_ADC_SEL_I_4M_SHIFT)
#define REG_XTAL_MDLL_CLK_ADC_SEL_I_4M(n)       BITFIELD_VAL(REG_XTAL_MDLL_CLK_ADC_SEL_I_4M, n)

// REG_1016
#define REG_XTAL_LDO_VTUNE_CH22_SHIFT           0
#define REG_XTAL_LDO_VTUNE_CH22_MASK            (0x7 << REG_XTAL_LDO_VTUNE_CH22_SHIFT)
#define REG_XTAL_LDO_VTUNE_CH22(n)              BITFIELD_VAL(REG_XTAL_LDO_VTUNE_CH22, n)
#define REG_XTAL_LDO_VTUNE_CH46_SHIFT           3
#define REG_XTAL_LDO_VTUNE_CH46_MASK            (0x7 << REG_XTAL_LDO_VTUNE_CH46_SHIFT)
#define REG_XTAL_LDO_VTUNE_CH46(n)              BITFIELD_VAL(REG_XTAL_LDO_VTUNE_CH46, n)
#define REG_XTAL_LDO_VTUNE_CH70_SHIFT           6
#define REG_XTAL_LDO_VTUNE_CH70_MASK            (0x7 << REG_XTAL_LDO_VTUNE_CH70_SHIFT)
#define REG_XTAL_LDO_VTUNE_CH70(n)              BITFIELD_VAL(REG_XTAL_LDO_VTUNE_CH70, n)

// REG_102D
#define REG_LP_BBPLL_RSTN_DR                    (1 << 0)
#define REG_LP_BBPLL_RSTN                       (1 << 1)
#define REG_LP_BBPLL_CLK_FBC_EDGE               (1 << 2)
#define REG_LP_BBPLL_INT_DEC_SEL_SHIFT          3
#define REG_LP_BBPLL_INT_DEC_SEL_MASK           (0x7 << REG_LP_BBPLL_INT_DEC_SEL_SHIFT)
#define REG_LP_BBPLL_INT_DEC_SEL(n)             BITFIELD_VAL(REG_LP_BBPLL_INT_DEC_SEL, n)
#define REG_LP_BBPLL_DITHER_BYPASS              (1 << 6)
#define REG_LP_BBPLL_PRESCALER_DEL_SEL_SHIFT     7
#define REG_LP_BBPLL_PRESCALER_DEL_SEL_MASK     (0xF << REG_LP_BBPLL_PRESCALER_DEL_SEL_SHIFT)
#define REG_LP_BBPLL_PRESCALER_DEL_SEL(n)       BITFIELD_VAL(REG_LP_BBPLL_PRESCALER_DEL_SEL, n)
#define REG_LP_BBPLL_FREQ_EN_COMB               (1 << 11)
#define REG_LP_BBPLL_TRI_STEP_SEL_SHIFT         12
#define REG_LP_BBPLL_TRI_STEP_SEL_MASK          (0x3 << REG_LP_BBPLL_TRI_STEP_SEL_SHIFT)
#define REG_LP_BBPLL_TRI_STEP_SEL(n)            BITFIELD_VAL(REG_LP_BBPLL_TRI_STEP_SEL, n)
#define REG_LP_BBPLL_TRI_FREQ_SEL_SHIFT         14
#define REG_LP_BBPLL_TRI_FREQ_SEL_MASK          (0x3 << REG_LP_BBPLL_TRI_FREQ_SEL_SHIFT)
#define REG_LP_BBPLL_TRI_FREQ_SEL(n)            BITFIELD_VAL(REG_LP_BBPLL_TRI_FREQ_SEL, n)

// REG_102F
#define REG_LP_BBPLL_SDM_FREQWORD_15_0_SHIFT    0
#define REG_LP_BBPLL_SDM_FREQWORD_15_0_MASK     (0xFFFF << REG_LP_BBPLL_SDM_FREQWORD_15_0_SHIFT)
#define REG_LP_BBPLL_SDM_FREQWORD_15_0(n)       BITFIELD_VAL(REG_LP_BBPLL_SDM_FREQWORD_15_0, n)

// REG_1030
#define REG_LP_BBPLL_SDM_FREQWORD_31_16_SHIFT   0
#define REG_LP_BBPLL_SDM_FREQWORD_31_16_MASK    (0xFFFF << REG_LP_BBPLL_SDM_FREQWORD_31_16_SHIFT)
#define REG_LP_BBPLL_SDM_FREQWORD_31_16(n)      BITFIELD_VAL(REG_LP_BBPLL_SDM_FREQWORD_31_16, n)

// REG_1031
#define REG_LP_BBPLL_SDM_FREQWORD_32            (1 << 0)

// REG_1034
#define REG_LP_BBPLL_PFD_RST_DR                 1
#define REG_LP_BBPLL_PFD_RST                    2
#define REG_LP_BBPLL_PRECHARGE_DR               14
#define REG_LP_BBPLL_PRECHARGE                  15

// REG_1035
#define REG_LP_BBPLL_VCO_SWRC_SHIFT             3
#define REG_LP_BBPLL_VCO_SWRC_MASK              (0x3 << REG_LP_BBPLL_VCO_SWRC_SHIFT)
#define REG_LP_BBPLL_VCO_SWRC(n)                BITFIELD_VAL(REG_LP_BBPLL_VCO_SWRC, n)
#define REG_LP_BBPLL_VCO_SPD_SHIFT              5
#define REG_LP_BBPLL_VCO_SPD_MASK               (0x7 << REG_LP_BBPLL_VCO_SPD_SHIFT)
#define REG_LP_BBPLL_VCO_SPD(n)                 BITFIELD_VAL(REG_LP_BBPLL_VCO_SPD, n)
#define REG_LP_BBPLL_DIG_SWRC_SHIFT             10
#define REG_LP_BBPLL_DIG_SWRC_MASK              (0x7 << REG_LP_BBPLL_DIG_SWRC_SHIFT)
#define REG_LP_BBPLL_DIG_SWRC(n)                BITFIELD_VAL(REG_LP_BBPLL_DIG_SWRC, n)

// REG_1037
#define REG_LP_BBPLL_LK_RSTB                    (1 << 0)
#define REG_LP_BBPLL_CLKGEN_RST                 (1 << 1)
#define REG_LP_BBPLL_EN_CLK_USB                 (1 << 2)
#define REG_LP_BBPLL_EN_CLK_CODEC               (1 << 3)
#define REG_LP_BBPLL_DIVN_AD_SHIFT              4
#define REG_LP_BBPLL_DIVN_AD_MASK               (0x1F << REG_LP_BBPLL_DIVN_AD_SHIFT)
#define REG_LP_BBPLL_DIVN_AD(n)                 BITFIELD_VAL(REG_LP_BBPLL_DIVN_AD_SHIFT, n)
#define REG_LP_BBPLL_DIG_CLK_FREQ_SEL           (1 << 9)
#define REG_LP_BBPLL_EN_CLK_DIG                 (1 << 10)
#define REG_LP_BBPLL_EN_CLK_BBADC               (1 << 11)
#define REG_LP_BBPLL_CLKGEN_RST_DR              (1 << 12)
#define REG_LP_BBPLL_SDM_CLK_SEL                (1 << 14)

// REG_1043
#define REG_XTAL_MDLL_PFD_ERROR_DET_EN          (1 << 3)

// REG_1044
#define REG_XTAL_MDLL_LPF_PRECH_BIAS_SHIFT      0
#define REG_XTAL_MDLL_LPF_PRECH_BIAS_MASK       (0x7 << REG_XTAL_MDLL_LPF_PRECH_BIAS_SHIFT)
#define REG_XTAL_MDLL_LPF_PRECH_BIAS(n)         BITFIELD_VAL(REG_XTAL_MDLL_LPF_PRECH_BIAS, n)
#define REG_XTAL_MDLL_DLL_SWRC_SHIFT            7
#define REG_XTAL_MDLL_DLL_SWRC_MASK             (0x3 << REG_XTAL_MDLL_DLL_SWRC_SHIFT)
#define REG_XTAL_MDLL_DLL_SWRC(n)               BITFIELD_VAL(REG_XTAL_MDLL_DLL_SWRC, n)

// REG_1050
#define REG_XTAL_MAIN_ICORE_SEL_SHIFT           8
#define REG_XTAL_MAIN_ICORE_SEL_MASK            (0x1F << REG_XTAL_MAIN_ICORE_SEL_SHIFT)
#define REG_XTAL_MAIN_ICORE_SEL(n)              BITFIELD_VAL(REG_XTAL_MAIN_ICORE_SEL, n)

// REG_1052
#define REG_XTAL_SWRC_ACBUF_MAIN_SHIFT          0
#define REG_XTAL_SWRC_ACBUF_MAIN_MASK           (0x1F << REG_XTAL_SWRC_ACBUF_MAIN_SHIFT)
#define REG_XTAL_SWRC_ACBUF_MAIN(n)             BITFIELD_VAL(REG_XTAL_SWRC_ACBUF_MAIN, n)
#define REG_XTAL_SWRC_ACBUF_SUB_SHIFT           5
#define REG_XTAL_SWRC_ACBUF_SUB_MASK            (0x1F << REG_XTAL_SWRC_ACBUF_SUB_SHIFT)
#define REG_XTAL_SWRC_ACBUF_SUB(n)              BITFIELD_VAL(REG_XTAL_SWRC_ACBUF_SUB, n)
#define REG_XTAL_ACBUF_MAIN_SEL_SHIFT           10
#define REG_XTAL_ACBUF_MAIN_SEL_MASK            (0x3 << REG_XTAL_ACBUF_MAIN_SEL_SHIFT)
#define REG_XTAL_ACBUF_MAIN_SEL(n)              BITFIELD_VAL(REG_XTAL_ACBUF_MAIN_SEL, n)

// ANALOG REG_5A
#define SOFT_RESETN_CAPSENSOR                   (1 << 14)
#define SOFT_RESETN_CAPSENSOR_REG               (1 << 15)

// ANALOG REG_16D
#define REG_ANALOG_PU_OSC                       (1 << 6)

// ANALOG REG_1AD
#define REG_ANALOG_CLK_SEL_DSI                  (1 << 0)
#define REG_ANALOG_EN_CLK_DSI                   (1 << 1)

#define PMU_VCORE_RC_MODE_FIXED_VDIG            PMU_VDIG_0_7V
#define PMU_VCORE_RC_MODE_FIXED_DCDC_DIG        PMU_DCDC_DIG_0_7V
#define PMU_VCORE_RC_MODE_FIXED_VDIG_L          PMU_VDIG_L_0_65V

#define PMU_EFUSE_TEMPSENSOR_DEFAULT            24948

#define REG_CP_WAFER_CTX_XY                     PMU_EFUSE_PAGE_B
#define REG_CP_WAFER_CTX_ID                     PMU_EFUSE_PAGE_C

enum RF_ANA_CHG_REG_T {
    CHG_REG_METAL_ID            = 0x00,

    ANA_REG_5A                  = 0x5A,
    ANA_REG_60                  = 0x60,

    RF_REG_00                   = RF_REG(0x00),
    RF_REG_F5                   = RF_REG(0xF5),
    RF_REG_111                  = RF_REG(0x111),
    RF_REG_113                  = RF_REG(0x113),
    RF_REG_114                  = RF_REG(0x114),
    RF_REG_115                  = RF_REG(0x115),
    RF_REG_11F                  = RF_REG(0X11F),
    RF_REG_120                  = RF_REG(0x120),
    RF_REG_122                  = RF_REG(0x122),
    RF_REG_1F4                  = RF_REG(0x1F4),

    RF_REG_100C                 = RF1_REG(0x0C),
    RF_REG_100E                 = RF1_REG(0x0E),
    RF_REG_1016                 = RF1_REG(0x16),
    RF_REG_102D                 = RF1_REG(0x2D),
    RF_REG_102F                 = RF1_REG(0x2F),
    RF_REG_1030                 = RF1_REG(0x30),
    RF_REG_1031                 = RF1_REG(0x31),
    RF_REG_1034                 = RF1_REG(0x34),
    RF_REG_1035                 = RF1_REG(0x35),
    RF_REG_1037                 = RF1_REG(0x37),
    RF_REG_1043                 = RF1_REG(0x43),
    RF_REG_1044                 = RF1_REG(0x44),
    RF_REG_1050                 = RF1_REG(0x50),
    RF_REG_1052                 = RF1_REG(0x52),
};

enum PMU_VCORE_REQ_T {
    PMU_VCORE_FLASH_WRITE_ENABLED   = (1 << 0),
    PMU_VCORE_FLASH_FREQ_HIGH       = (1 << 1),
    PMU_VCORE_PSRAM_FREQ_HIGH       = (1 << 2),
    PMU_VCORE_PSRAM_FREQ_MEDIUM     = (1 << 3),
    PMU_VCORE_PSRAM_FREQ_MEDIUM_LOW = (1 << 4),
    PMU_VCORE_USB_HS_ENABLED        = (1 << 5),
    PMU_VCORE_SYS_FREQ_MEDIUM_LOW   = (1 << 6),
    PMU_VCORE_SYS_FREQ_MEDIUM       = (1 << 7),
    PMU_VCORE_SYS_FREQ_HIGH         = (1 << 8),
    PMU_VCORE_RS_FREQ_HIGH          = (1 << 9),
    PMU_VCORE_RS_ADC_FREQ_HIGH      = (1 << 10),
    PMU_VCORE_IIR_FREQ_HIGH         = (1 << 11),
    PMU_VCORE_IIR_EQ_FREQ_HIGH      = (1 << 12),
    PMU_VCORE_FIR_FREQ_HIGH         = (1 << 13),
    PMU_VCORE_PMU_RESET             = (1 << 14),
};

union BOOT_SETTINGS_T {
    struct {
        unsigned short usb_dld_dis      :1;
        unsigned short uart_dld_en      :1;
        unsigned short uart_trace_en    :1;
        unsigned short pll_dis          :1;
        unsigned short uart_baud_div2   :1;
        unsigned short sec_freq_div2    :1;
        unsigned short crystal_freq     :2;
        unsigned short timeout_div      :2;
        unsigned short uart_connected   :1;
        unsigned short read_quit_crm    :1;
        unsigned short chksum           :4;
    };
    unsigned short reg;
};

// Before adding a new module, it is necessary to check if pmu_module_set(get)_volt() is compatible.
enum PMU_MODULE_T {
    PMU_ANA,
    PMU_CORE,
    PMU_CODEC,
    PMU_CORE_L,
    PMU_USB,
    PMU_SENSOR,
    PMU_MEM,

    PMU_MODULE_QTY,
};

struct PMU_MODULE_CFG_T {
    unsigned short manual_bit;
    unsigned short ldo_en;
    unsigned short lp_en_dr;
    unsigned short lp_en;
    unsigned short dsleep_mode;
    unsigned short res_sel_dr;
    unsigned short dsleep_v;
    unsigned short dsleep_v_shift;
    unsigned short normal_v;
    unsigned short normal_v_shift;
    unsigned short rc_v;
    unsigned short rc_v_shift;
};

#define PMU_MOD_CFG_VAL(m)              { \
    REG_PU_LDO_V##m##_DR, REG_PU_LDO_V##m, \
    REG_LP_EN_LDO_V##m##_DR, REG_LP_EN_LDO_V##m, \
    REG_PU_LDO_V##m##_DSLEEP, REG_LDO_V##m##_RES_SEL_DR, \
    REG_LDO_V##m##_VBIT_DSLEEP_MASK, REG_LDO_V##m##_VBIT_DSLEEP_SHIFT,\
    REG_LDO_V##m##_VBIT_NORMAL_MASK, REG_LDO_V##m##_VBIT_NORMAL_SHIFT,\
    REG_LDO_V##m##_VBIT_RC_MASK, REG_LDO_V##m##_VBIT_RC_SHIFT }

static const struct PMU_MODULE_CFG_T pmu_module_cfg[] = {
    PMU_MOD_CFG_VAL(ANA),
    PMU_MOD_CFG_VAL(CORE),
    PMU_MOD_CFG_VAL(CODEC),
    PMU_MOD_CFG_VAL(CORE_L),
    PMU_MOD_CFG_VAL(USB),
    PMU_MOD_CFG_VAL(SENSOR),
    PMU_MOD_CFG_VAL(MEM),
};

#define OPT_TYPE                        const

static OPT_TYPE uint16_t ana_act_dcdc =
    PMU_DCDC_ANA_1_3V;
static OPT_TYPE uint16_t ana_lp_dcdc = PMU_DCDC_ANA_1_3V;

static OPT_TYPE POSSIBLY_UNUSED uint16_t vcodec_mv = (uint16_t)(VCODEC_VOLT * 1000);
static OPT_TYPE POSSIBLY_UNUSED uint16_t vhppa_mv = (uint16_t)(VHPPA_VOLT * 1000);
#ifdef PMU_VMEM_PWR_SUPPLY_IO
static OPT_TYPE POSSIBLY_UNUSED uint16_t vmem_mv = (uint16_t)(VMEM_VOLT * 1000);
#endif

static OPT_TYPE uint16_t dig_lp_ldo = PMU_VDIG_0_65V;
static OPT_TYPE uint16_t dig_lp_dcdc = PMU_DCDC_DIG_0_6V;
#ifdef SPLIT_CORE_M_CORE_L
static OPT_TYPE uint16_t dig_lp_logic = PMU_VDIG_L_0_55V;
#endif

// TODO: Use the value in BL as the default value
static enum PMU_POWER_MODE_T BOOT_DATA_LOC pmu_power_mode =
#ifdef PMU_ALREADY_INIT
    PMU_POWER_MODE_DIG_DCDC;
#else
    PMU_POWER_MODE_NONE;
#endif

// TODO: Use the value in BL as the default value
static enum PMU_VCORE_REQ_T BOOT_BSS_LOC pmu_vcore_req;

static PMU_CHARGER_IRQ_HANDLER_T charger_irq_handler;

#ifdef PMU_IRQ_UNIFIED
static bool gpio_irq_en[2];
static HAL_GPIO_PIN_IRQ_HANDLER gpio_irq_handler[2];

static PMU_WDT_IRQ_HANDLER_T wdt_irq_handler;

static PMU_IRQ_UNIFIED_HANDLER_T pmu_irq_hdlrs[PMU_IRQ_TYPE_QTY];
#endif

// Move all the data/bss invovled in pmu_open() to .sram_data/.sram_bss,
// so that pmu_open() can be called at the end of BootInit(),
// for data/bss is initialized after BootInit().
static uint8_t BOOT_DATA_LOC bbpll_codec_div = 32;

static uint32_t BOOT_BSS_LOC pmu_metal_id;

static uint16_t wdt_irq_timer;
static uint16_t wdt_reset_timer;

static const bool dcdc_ramp_en =
#if defined(DCDC_VOLT_HW_RAMP_ONLY)
    true;
#else
    false;
#endif

static uint16_t SRAM_BSS_DEF(dcdc_ramp_map);

#if defined(MCU_HIGH_PERFORMANCE_MODE)
static const uint16_t high_perf_freq_mhz =
#if defined(MTEST_ENABLED) && defined(MTEST_CLK_MHZ)
    MTEST_CLK_MHZ;
#else
    320;
#endif
PMU_OPEN_BSS_LOC
static bool high_perf_on;
#endif

static enum PMU_BOOT_CAUSE_T BOOT_BSS_LOC pmu_boot_reason;

static bool BOOT_BSS_LOC pmu_boot_first_pwr_up;

#ifdef PMU_NTC_MONITOR
static struct PMU_NTC_MONITOR_CTX_T ntc_monitor_ctx[PMU_NTC_USER_QTY];
static struct PMU_NTC_CTX_T ntc_ctx;
static bool ntc_irq_busy = false;
static void pmu_ntc_monitor_init(void);
#ifdef NTC_MONITOR_USE_OSTIMER
static void pmu_ntc_monitor_ostimer_handler(void const *param);
osTimerDef(ntc_monitor_ostimer, pmu_ntc_monitor_ostimer_handler);
static osTimerId ntc_monitor_ostimer_id = NULL;
#else
static HWTIMER_ID ntc_monitor_timer = NULL;
#endif
#endif

#ifdef PMU_FORCE_LP_MODE
static enum PMU_BIG_BANDGAP_USER_T big_bandgap_user_map;
#endif

#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
#ifndef HPPA_LDO_ON
static void pmu_hppa_dcdc_to_ldo(void);
#endif
static void pmu_module_ramp_volt(unsigned char module, unsigned short sleep_v, unsigned short normal_v);
#ifndef PMU_VMEM_PWR_SUPPLY_IO
void pmu_module_config(enum PMU_MODULE_T module,unsigned short is_manual,unsigned short ldo_on,unsigned short lp_mode,unsigned short dpmode);
#endif
#ifdef PMU_FORCE_LP_MODE
static void pmu_bandgap_cfg_reset(void);
#endif
#if !defined(ROM_BUILD) && defined(PMU_EFUSE_INIT_IN_BOOT)
static bool BOOT_BSS_LOC efuse_buf_valid;
static unsigned short pmu_efuse_page_arrays[PMU_EFUSE_PAGE_QTY];
#endif
#endif

#if defined(PMU_CLK_USE_EXT_CRYSTAL) || defined(RTC_CLK_USE_EXT_CRYSTAL)
#ifdef PMU_CLK_USE_EXT_CRYSTAL

enum XTAL_32K_CALIB_STATE_T {
    XTAL_32K_CALIB_STATE_NULL,
    XTAL_32K_CALIB_STATE_CALIB,
    XTAL_32K_CALIB_STATE_SET,
    XTAL_32K_CALIB_STATE_ROLLBACK,
};

static enum XTAL_32K_CALIB_STATE_T xtal_32k_calib_state = XTAL_32K_CALIB_STATE_NULL;
static uint32_t xtal_32k_calib_time;
static float xtal_32k_calib_prev_hz;
static uint16_t xtal_32k_calib_prev_capbit;
static bool xtal_32k_calib_prev_valid = false;
static const uint32_t xtal_32k_calib_intvl_ms = 200;
static const uint32_t xtal_32k_calib_stable_ms = 300;
#endif

static uint32_t BOOT_BSS_LOC pmu_ext_crystal_init_stime;
static uint8_t pmu_ext_crystal_clkout_usr_map;

static void pmu_external_crystal_init(void);
static void pmu_external_crystal_clkout_after_pwroff(void);
#endif

#if defined(_AUTO_TEST_)
static bool at_skip_shutdown = false;

void pmu_at_skip_shutdown(bool enable)
{
    at_skip_shutdown = enable;
}
#endif

#ifdef RTC_CALENDAR
#define RTC_REG_VAL2CALENDAR(TENS, UNITS)                     ((TENS) * 10 + (UNITS))
#define RTC_CALENDAR2REG_VAL_TENS(RTC_CALENDAR)               ((RTC_CALENDAR) / 10 % 10)
#define RTC_CALENDAR2REG_VAL_UNITS(RTC_CALENDAR)              ((RTC_CALENDAR) / 1 % 10)

#define RTC_CALENDAR_LOAD_TIME_STABLE_TIME_US                 (100)

struct RTC_REG_VAL_FORMAT_T {
    uint8_t year_tens;
    uint8_t year_units;
    uint8_t data_month_tens;
    uint8_t data_month_units;
    uint8_t data_day_tens;
    uint8_t data_day_units;
    uint8_t data_week;
    uint8_t hour_tens;
    uint8_t hour_units;
    uint8_t min_tens;
    uint8_t min_units;
    uint8_t sec_tens;
    uint8_t sec_units;
};

static PMU_RTC_CALENDAR_IRQ_HANDLER_T rtc_cal_irq_handler;

static void pmu_rtc_calendar_alarm_intr_enable(void)
{
    uint16_t val;
    uint32_t lock;

    lock = int_lock();
    pmu_read(PMU_REG_INT_EN, &val);
    val |= RTC_INT_EN_0;
    pmu_write(PMU_REG_INT_EN, val);
    int_unlock(lock);
}

static void pmu_rtc_calendar_init(void)
{
    uint16_t POSSIBLY_UNUSED val;

#ifdef RTC_CLK_USE_EXT_CRYSTAL
    pmu_read(PMU_REG_RTC_RTC_CFG_10E, &val);
    val |= REG_CLK_RTC_SEL_XO;
    pmu_write(PMU_REG_RTC_RTC_CFG_10E, val);
#elif !defined(PMU_CLK_USE_EXT_CRYSTAL)
    pmu_read(PMU_REG_POWER_KEY_CFG, &val);
    val |= PU_LPO_DR | PU_LPO_REG;
    pmu_write(PMU_REG_POWER_KEY_CFG, val);
#endif

    pmu_rtc_calendar_alarm_intr_enable();

#ifdef SIMU
    val = 32 - 2;
#elif defined(RTC_CLK_USE_EXT_CRYSTAL)
    val = 32768 - 2;
#else
    val = CONFIG_SYSTICK_HZ - 2;
#endif
    pmu_write(PMU_REG_RTC_DIV_1HZ, val);
}

static void _pmu_rtc_calendar_alarm_irq_handler(uint16_t irq_status)
{
    struct RTC_CALENDAR_FORMAT_T alarm_cal = {0, };

    if (irq_status & RTC_INT0_MSKED) {
        pmu_write(PMU_REG_INT_CLR, RTC_INT_0);
        if (rtc_cal_irq_handler) {
            pmu_rtc_calendar_get(&alarm_cal);
            rtc_cal_irq_handler(&alarm_cal);
        }
    }
}

int pmu_rtc_calendar_alarm_irq_handler_set(PMU_RTC_CALENDAR_IRQ_HANDLER_T handler)
{
    uint32_t lock;
    uint16_t val;

    lock = int_lock();
    rtc_cal_irq_handler = handler;

    pmu_read(PMU_REG_INT_MASK, &val);
    if (handler) {
        val |= RTC_INT0_MSK;
    } else {
        val &= ~RTC_INT0_MSK;
    }
    pmu_write(PMU_REG_INT_MASK, val);
    pmu_set_irq_unified_handler(PMU_IRQ_TYPE_RTC, handler ? _pmu_rtc_calendar_alarm_irq_handler : NULL);
    int_unlock(lock);

    return 0;
}

int pmu_rtc_calendar_pwron_enable(bool en)
{
    uint32_t lock;
    uint16_t val;

    lock = int_lock();
    pmu_read(PMU_REG_POWER_KEY_CFG, &val);
    if (en) {
        val |= RTC_POWER_ON_EN;
    } else {
        val &= ~RTC_POWER_ON_EN;
    }
    pmu_write(PMU_REG_POWER_KEY_CFG, val);
    int_unlock(lock);

    return 0;
}

int pmu_rtc_calendar_alarm_clear(void)
{
    uint16_t val;
    uint32_t lock;

    lock = int_lock();
    pmu_read(PMU_REG_INT_EN, &val);
    val &= ~RTC_INT_EN_0;
    pmu_write(PMU_REG_INT_EN, val);
    int_unlock(lock);

    pmu_write(PMU_REG_INT_CLR, RTC_INT_0);

    return 0;
}

int pmu_rtc_calendar_alarm_set(struct RTC_CALENDAR_FORMAT_T *alarm_set)
{
    uint32_t lock;
    uint16_t val;
    struct RTC_REG_VAL_FORMAT_T rtc_reg_fmt = {0, };

    lock = int_lock();

    val = rtc_calendar_is_invalid(alarm_set, true);
    if (val) {
        ASSERT(false, "%s: Invalid calendar! error=0x%04x", __func__, val);
    }

    rtc_reg_fmt.year_tens = RTC_CALENDAR2REG_VAL_TENS(alarm_set->year);
    rtc_reg_fmt.year_units = RTC_CALENDAR2REG_VAL_UNITS(alarm_set->year);

    rtc_reg_fmt.data_month_tens = RTC_CALENDAR2REG_VAL_TENS(alarm_set->month);
    rtc_reg_fmt.data_month_units = RTC_CALENDAR2REG_VAL_UNITS(alarm_set->month);

    rtc_reg_fmt.data_day_tens = RTC_CALENDAR2REG_VAL_TENS(alarm_set->day);
    rtc_reg_fmt.data_day_units = RTC_CALENDAR2REG_VAL_UNITS(alarm_set->day);

    rtc_reg_fmt.data_week = alarm_set->week;

    rtc_reg_fmt.hour_tens = RTC_CALENDAR2REG_VAL_TENS(alarm_set->hour);
    rtc_reg_fmt.hour_units = RTC_CALENDAR2REG_VAL_UNITS(alarm_set->hour);

    rtc_reg_fmt.min_tens = RTC_CALENDAR2REG_VAL_TENS(alarm_set->minute);
    rtc_reg_fmt.min_units = RTC_CALENDAR2REG_VAL_UNITS(alarm_set->minute);

    rtc_reg_fmt.sec_tens = RTC_CALENDAR2REG_VAL_TENS(alarm_set->second);
    rtc_reg_fmt.sec_units = RTC_CALENDAR2REG_VAL_UNITS(alarm_set->second);

    // year
    pmu_read(PMU_REG_RTC_MATCH_YEARS, &val);
    val = SET_BITFIELD(val, RTC_MATCH_YEARS_TENS, rtc_reg_fmt.year_tens);
    val = SET_BITFIELD(val, RTC_MATCH_YEARS_UNITS, rtc_reg_fmt.year_units);
    pmu_write(PMU_REG_RTC_MATCH_YEARS, val);

    // data
    pmu_read(PMU_REG_RTC_MATCH_DATAS, &val);
    if (rtc_reg_fmt.data_month_tens) {
        val |= RTC_MATCH_DATAS_MONTH_TENS;
    } else {
        val &= ~RTC_MATCH_DATAS_MONTH_TENS;
    }
    val = SET_BITFIELD(val, RTC_MATCH_DATAS_MONTH_UNITS, rtc_reg_fmt.data_month_units);
    val = SET_BITFIELD(val, RTC_MATCH_DATAS_DAY_TENS, rtc_reg_fmt.data_day_tens);
    val = SET_BITFIELD(val, RTC_MATCH_DATAS_DAY_UNITS, rtc_reg_fmt.data_day_units);
    val = SET_BITFIELD(val, RTC_MATCH_DATAS_WEEK, rtc_reg_fmt.data_week);
    pmu_write(PMU_REG_RTC_MATCH_DATAS, val);

    // hour
    pmu_read(PMU_REG_RTC_MATCH_HOURS, &val);
    val = SET_BITFIELD(val, RTC_MATCH_HOURS_TENS, rtc_reg_fmt.hour_tens);
    val = SET_BITFIELD(val, RTC_MATCH_HOURS_UNITS, rtc_reg_fmt.hour_units);
    pmu_write(PMU_REG_RTC_MATCH_HOURS, val);

    // minute_second
    pmu_read(PMU_REG_RTC_MATCH_MIN_SEC, &val);
    val = SET_BITFIELD(val, RTC_MATCH_TIME_MIN_TENS, rtc_reg_fmt.min_tens);
    val = SET_BITFIELD(val, RTC_MATCH_TIME_MIN_UNITS, rtc_reg_fmt.min_units);
    val = SET_BITFIELD(val, RTC_MATCH_TIME_SEC_TENS, rtc_reg_fmt.sec_tens);
    val = SET_BITFIELD(val, RTC_MATCH_TIME_SEC_UNITS, rtc_reg_fmt.sec_units);
    pmu_write(PMU_REG_RTC_MATCH_MIN_SEC, val);

    pmu_rtc_calendar_alarm_intr_enable();

    int_unlock(lock);

    return 0;
}

int pmu_rtc_calendar_alarm_get(struct RTC_CALENDAR_FORMAT_T *alarm_get)
{
    uint32_t lock;
    uint16_t val;
    struct RTC_REG_VAL_FORMAT_T rtc_reg_fmt = {0, };

    lock = int_lock();
    // year
    pmu_read(PMU_REG_RTC_MATCH_YEARS, &val);
    rtc_reg_fmt.year_tens = GET_BITFIELD(val, RTC_GET_YEARS_TENS);
    rtc_reg_fmt.year_units = GET_BITFIELD(val, RTC_GET_YEARS_UNITS);
    alarm_get->year = RTC_REG_VAL2CALENDAR(rtc_reg_fmt.year_tens, rtc_reg_fmt.year_units) + RTC_CALENDAR_YEAR_REF;

    // data
    pmu_read(PMU_REG_RTC_MATCH_DATAS, &val);
    if (val & RTC_GET_DATAS_MONTH_TENS) {
        rtc_reg_fmt.data_month_tens = 1;
    } else {
        rtc_reg_fmt.data_month_tens = 0;
    }
    rtc_reg_fmt.data_month_units = GET_BITFIELD(val, RTC_GET_DATAS_MONTH_UNITS);
    alarm_get->month = RTC_REG_VAL2CALENDAR(rtc_reg_fmt.data_month_tens, rtc_reg_fmt.data_month_units);

    rtc_reg_fmt.data_day_tens = GET_BITFIELD(val, RTC_GET_DATAS_DAY_TENS);
    rtc_reg_fmt.data_day_units = GET_BITFIELD(val, RTC_GET_DATAS_DAY_UNITS);
    alarm_get->day = RTC_REG_VAL2CALENDAR(rtc_reg_fmt.data_day_tens, rtc_reg_fmt.data_day_units);

    rtc_reg_fmt.data_week = GET_BITFIELD(val, RTC_GET_DATAS_WEEK);
    alarm_get->week = rtc_reg_fmt.data_week;

    // hour
    pmu_read(PMU_REG_RTC_MATCH_HOURS, &val);
    rtc_reg_fmt.hour_tens = GET_BITFIELD(val, RTC_GET_HOURS_TENS);
    rtc_reg_fmt.hour_units = GET_BITFIELD(val, RTC_GET_HOURS_UNITS);
    alarm_get->hour = RTC_REG_VAL2CALENDAR(rtc_reg_fmt.hour_tens, rtc_reg_fmt.hour_units);

    // minute_second
    pmu_read(PMU_REG_RTC_MATCH_MIN_SEC, &val);
    rtc_reg_fmt.min_tens = GET_BITFIELD(val, RTC_GET_TIME_MIN_TENS);
    rtc_reg_fmt.min_units = GET_BITFIELD(val, RTC_GET_TIME_MIN_UNITS);
    alarm_get->minute = RTC_REG_VAL2CALENDAR(rtc_reg_fmt.min_tens, rtc_reg_fmt.min_units);

    rtc_reg_fmt.sec_tens = GET_BITFIELD(val, RTC_GET_TIME_SEC_TENS);
    rtc_reg_fmt.sec_units = GET_BITFIELD(val, RTC_GET_TIME_SEC_UNITS);
    alarm_get->second = RTC_REG_VAL2CALENDAR(rtc_reg_fmt.sec_tens, rtc_reg_fmt.sec_units);

    int_unlock(lock);

    return 0;
}

int pmu_rtc_calendar_set(struct RTC_CALENDAR_FORMAT_T *calendar_set)
{
    uint32_t lock;
    uint16_t val;
    struct RTC_REG_VAL_FORMAT_T rtc_reg_fmt = {0, };

    lock = int_lock();

    val = rtc_calendar_is_invalid(calendar_set, true);
    if (val) {
        ASSERT(false, "%s: Invalid calendar! error=0x%04x", __func__, val);
    }

    rtc_reg_fmt.year_tens = RTC_CALENDAR2REG_VAL_TENS(calendar_set->year);
    rtc_reg_fmt.year_units = RTC_CALENDAR2REG_VAL_UNITS(calendar_set->year);

    if (RTC_CALENDAR2REG_VAL_TENS(calendar_set->month) != 0) {
        rtc_reg_fmt.data_month_tens = 1;
    } else {
        rtc_reg_fmt.data_month_tens = 0;
    }
    rtc_reg_fmt.data_month_units = RTC_CALENDAR2REG_VAL_UNITS(calendar_set->month);

    rtc_reg_fmt.data_day_tens = RTC_CALENDAR2REG_VAL_TENS(calendar_set->day);
    rtc_reg_fmt.data_day_units = RTC_CALENDAR2REG_VAL_UNITS(calendar_set->day);

    rtc_reg_fmt.data_week = calendar_set->week;

    rtc_reg_fmt.hour_tens = RTC_CALENDAR2REG_VAL_TENS(calendar_set->hour);
    rtc_reg_fmt.hour_units = RTC_CALENDAR2REG_VAL_UNITS(calendar_set->hour);

    rtc_reg_fmt.min_tens = RTC_CALENDAR2REG_VAL_TENS(calendar_set->minute);
    rtc_reg_fmt.min_units = RTC_CALENDAR2REG_VAL_UNITS(calendar_set->minute);

    rtc_reg_fmt.sec_tens = RTC_CALENDAR2REG_VAL_TENS(calendar_set->second);
    rtc_reg_fmt.sec_units = RTC_CALENDAR2REG_VAL_UNITS(calendar_set->second);

    // year
    pmu_read(PMU_REG_RTC_LOAD_YEARS, &val);
    val = SET_BITFIELD(val, RTC_LOAD_YEARS_TENS, rtc_reg_fmt.year_tens);
    val = SET_BITFIELD(val, RTC_LOAD_YEARS_UNITS, rtc_reg_fmt.year_units);
    pmu_write(PMU_REG_RTC_LOAD_YEARS, val);

    // data
    pmu_read(PMU_REG_RTC_LOAD_DATAS_EN, &val);
    if (rtc_reg_fmt.data_month_tens) {
        val |= RTC_LOAD_DATAS_MONTH_TENS;
    } else {
        val &= ~RTC_LOAD_DATAS_MONTH_TENS;
    }
    val = SET_BITFIELD(val, RTC_LOAD_DATAS_MONTH_UNITS, rtc_reg_fmt.data_month_units);
    val = SET_BITFIELD(val, RTC_LOAD_DATAS_DAY_TENS, rtc_reg_fmt.data_day_tens);
    val = SET_BITFIELD(val, RTC_LOAD_DATAS_DAY_UNITS, rtc_reg_fmt.data_day_units);
    val = SET_BITFIELD(val, RTC_LOAD_DATAS_WEEK, rtc_reg_fmt.data_week);
    pmu_write(PMU_REG_RTC_LOAD_DATAS_EN, val);

    // hour
    pmu_read(PMU_REG_RTC_LOAD_HOURS, &val);
    val = SET_BITFIELD(val, RTC_LOAD_HOURS_TENS, rtc_reg_fmt.hour_tens);
    val = SET_BITFIELD(val, RTC_LOAD_HOURS_UNITS, rtc_reg_fmt.hour_units);
    pmu_write(PMU_REG_RTC_LOAD_HOURS, val);

    // minute_second
    pmu_read(PMU_REG_RTC_LOAD_MIN_SEC_EN, &val);
    val = SET_BITFIELD(val, RTC_LOAD_TIME_MIN_TENS, rtc_reg_fmt.min_tens);
    val = SET_BITFIELD(val, RTC_LOAD_TIME_MIN_UNITS, rtc_reg_fmt.min_units);
    val = SET_BITFIELD(val, RTC_LOAD_TIME_SEC_TENS, rtc_reg_fmt.sec_tens);
    val = SET_BITFIELD(val, RTC_LOAD_TIME_SEC_UNITS, rtc_reg_fmt.sec_units);
    pmu_write(PMU_REG_RTC_LOAD_MIN_SEC_EN, val);

    pmu_read(PMU_REG_RTC_RTC_CFG_10E, &val);
    val |= RTC_TIME_LOAD;
    pmu_write(PMU_REG_RTC_RTC_CFG_10E, val);
    hal_sys_timer_delay_us(RTC_CALENDAR_LOAD_TIME_STABLE_TIME_US);

    int_unlock(lock);

    return 0;
}

int pmu_rtc_calendar_get(struct RTC_CALENDAR_FORMAT_T *calendar_get)
{
    uint32_t lock;
    uint16_t val;
    struct RTC_REG_VAL_FORMAT_T rtc_reg_fmt = {0, };

    lock = int_lock();
    // year
    pmu_read(PMU_REG_RTC_GET_YEARS, &val);
    rtc_reg_fmt.year_tens = GET_BITFIELD(val, RTC_GET_YEARS_TENS);
    rtc_reg_fmt.year_units = GET_BITFIELD(val, RTC_GET_YEARS_UNITS);
    calendar_get->year = RTC_REG_VAL2CALENDAR(rtc_reg_fmt.year_tens, rtc_reg_fmt.year_units) + RTC_CALENDAR_YEAR_REF;

    // data
    pmu_read(PMU_REG_RTC_GET_DATAS, &val);
    if (val & RTC_GET_DATAS_MONTH_TENS) {
        rtc_reg_fmt.data_month_tens = 1;
    } else {
        rtc_reg_fmt.data_month_tens = 0;
    }
    rtc_reg_fmt.data_month_units = GET_BITFIELD(val, RTC_GET_DATAS_MONTH_UNITS);
    calendar_get->month = RTC_REG_VAL2CALENDAR(rtc_reg_fmt.data_month_tens, rtc_reg_fmt.data_month_units);

    rtc_reg_fmt.data_day_tens = GET_BITFIELD(val, RTC_GET_DATAS_DAY_TENS);
    rtc_reg_fmt.data_day_units = GET_BITFIELD(val, RTC_GET_DATAS_DAY_UNITS);
    calendar_get->day = RTC_REG_VAL2CALENDAR(rtc_reg_fmt.data_day_tens, rtc_reg_fmt.data_day_units);

    rtc_reg_fmt.data_week = GET_BITFIELD(val, RTC_GET_DATAS_WEEK);
    calendar_get->week = rtc_reg_fmt.data_week;

    // hour
    pmu_read(PMU_REG_RTC_GET_HOURS, &val);
    rtc_reg_fmt.hour_tens = GET_BITFIELD(val, RTC_GET_HOURS_TENS);
    rtc_reg_fmt.hour_units = GET_BITFIELD(val, RTC_GET_HOURS_UNITS);
    calendar_get->hour = RTC_REG_VAL2CALENDAR(rtc_reg_fmt.hour_tens, rtc_reg_fmt.hour_units);

    // minute_second
    pmu_read(PMU_REG_RTC_GET_MIN_SEC, &val);
    rtc_reg_fmt.min_tens = GET_BITFIELD(val, RTC_GET_TIME_MIN_TENS);
    rtc_reg_fmt.min_units = GET_BITFIELD(val, RTC_GET_TIME_MIN_UNITS);
    calendar_get->minute = RTC_REG_VAL2CALENDAR(rtc_reg_fmt.min_tens, rtc_reg_fmt.min_units);

    rtc_reg_fmt.sec_tens = GET_BITFIELD(val, RTC_GET_TIME_SEC_TENS);
    rtc_reg_fmt.sec_units = GET_BITFIELD(val, RTC_GET_TIME_SEC_UNITS);
    calendar_get->second = RTC_REG_VAL2CALENDAR(rtc_reg_fmt.sec_tens, rtc_reg_fmt.sec_units);

    int_unlock(lock);

    return 0;
}
#endif

#ifdef PMU_LDO_VCORE_CALIB
static int8_t PMU_OPEN_BSS_LOC pmu_ldo_dig_l_comp;
static int8_t PMU_OPEN_BSS_LOC pmu_ldo_lp_dig_l_comp;

#ifdef PMU_ALREADY_INIT
BOOT_TEXT_FLASH_LOC
#endif
void pmu_get_ldo_dig_calib_value(void)
{
    uint16_t val;

    pmu_get_efuse(PMU_EFUSE_PAGE_D, &val);
    pmu_ldo_dig_l_comp = GET_BITFIELD(val, PMU_EFUSE_LDO_CORE_L_VOLT_COMP);
    if (val & PMU_EFUSE_LDO_CORE_L_VOLT_COMP_SIGN) {
        pmu_ldo_dig_l_comp = -pmu_ldo_dig_l_comp;
    }
    pmu_ldo_lp_dig_l_comp = GET_BITFIELD(val, PMU_EFUSE_LDO_CORE_L_VOLT_LP_COMP);
    if (val & PMU_EFUSE_LDO_CORE_L_VOLT_LP_COMP_SIGN) {
        pmu_ldo_lp_dig_l_comp = -pmu_ldo_lp_dig_l_comp;
    }

#ifdef FORCE_BIG_BANDGAP
    pmu_ldo_lp_dig_l_comp  = pmu_ldo_dig_l_comp;
#elif defined(PMU_FORCE_LP_MODE)
    pmu_ldo_dig_l_comp  = pmu_ldo_lp_dig_l_comp;
#endif
}
#endif

#ifdef PMU_DCDC_CALIB
static int16_t PMU_OPEN_BSS_LOC pmu_dcdc_dig_comp;
static int8_t PMU_OPEN_BSS_LOC pmu_dcdc_ana_comp;
static int8_t PMU_OPEN_BSS_LOC pmu_dcdc_hppa_comp;

static int16_t PMU_OPEN_BSS_LOC pmu_dig_lp_comp;
static int8_t PMU_OPEN_BSS_LOC pmu_ana_lp_comp;
static int8_t PMU_OPEN_BSS_LOC pmu_hppa_lp_comp;

#ifdef PMU_ALREADY_INIT
BOOT_TEXT_FLASH_LOC
#endif
static POSSIBLY_UNUSED void pmu_get_dcdc_calib_value(void)
{
    uint16_t val;
    bool new_calib = false;

    pmu_get_efuse(PMU_EFUSE_PAGE_17, &val);
    new_calib = !!(val & PMU_EFUSE_DCDC1_VOLT_CAL_NEW);
    if (new_calib) {
        pmu_dig_lp_comp = GET_BITFIELD(val, PMU_EFUSE_DCDC1_VOLT_COMP);
        if (val & PMU_EFUSE_DCDC1_VOLT_COMP_SIGN) {
            pmu_dig_lp_comp = -pmu_dig_lp_comp;
        }
        pmu_dcdc_dig_comp = pmu_dig_lp_comp;
    } else {
        pmu_get_efuse(PMU_EFUSE_PAGE_8, &val);
        pmu_dcdc_dig_comp = GET_BITFIELD(val, PMU_EFUSE_DCDC1_VOLT_COMP);
        if (val & PMU_EFUSE_DCDC1_VOLT_COMP_SIGN) {
            pmu_dcdc_dig_comp = -pmu_dcdc_dig_comp;
        }
        pmu_dig_lp_comp = GET_BITFIELD(val, PMU_EFUSE_DCDC1_VOLT_LP_COMP);
        if (val & PMU_EFUSE_DCDC1_VOLT_LP_COMP_SIGN) {
            pmu_dig_lp_comp = -pmu_dig_lp_comp;
        }
    }

    pmu_get_efuse(PMU_EFUSE_PAGE_9, &val);
    pmu_dcdc_ana_comp = GET_BITFIELD(val, PMU_EFUSE_DCDC2_VOLT_COMP);
    if (val & PMU_EFUSE_DCDC2_VOLT_COMP_SIGN) {
        pmu_dcdc_ana_comp = -pmu_dcdc_ana_comp;
    }
    pmu_ana_lp_comp = GET_BITFIELD(val, PMU_EFUSE_DCDC2_VOLT_LP_COMP);
    if (val & PMU_EFUSE_DCDC2_VOLT_LP_COMP_SIGN) {
        pmu_ana_lp_comp = -pmu_ana_lp_comp;
    }

    pmu_get_efuse(PMU_EFUSE_PAGE_A, &val);
    pmu_dcdc_hppa_comp = GET_BITFIELD(val, PMU_EFUSE_DCDC3_VOLT_COMP);
    if (val & PMU_EFUSE_DCDC3_VOLT_COMP_SIGN) {
        pmu_dcdc_hppa_comp = -pmu_dcdc_hppa_comp;
    }
    pmu_hppa_lp_comp = GET_BITFIELD(val, PMU_EFUSE_DCDC3_VOLT_LP_COMP);
    if (val & PMU_EFUSE_DCDC3_VOLT_LP_COMP_SIGN) {
        pmu_hppa_lp_comp = -pmu_hppa_lp_comp;
    }

#ifdef FORCE_BIG_BANDGAP
    pmu_dig_lp_comp  = pmu_dcdc_dig_comp;
    pmu_ana_lp_comp  = pmu_dcdc_ana_comp;
    pmu_hppa_lp_comp = pmu_dcdc_hppa_comp;
#elif defined(PMU_FORCE_LP_MODE)
    pmu_dcdc_dig_comp  = pmu_dig_lp_comp;
    pmu_dcdc_ana_comp  = pmu_ana_lp_comp;
    pmu_dcdc_hppa_comp = pmu_hppa_lp_comp;
#endif
}

static POSSIBLY_UNUSED unsigned short pmu_reg_val_add(unsigned short val, int delta, unsigned short max)
{
    int result = val + delta;

    if (result > max) {
        result = max;
    } else if (result < 0) {
        result = 0;
    }

    return (unsigned short)result;
}
#endif

uint32_t BOOT_TEXT_FLASH_LOC read_hw_metal_id(void)
{
    uint16_t val;
    uint32_t metal_id;

    pmu_read(PMU_REG_METAL_ID, &val);
    pmu_metal_id = GET_BITFIELD(val, REVID);

    metal_id = hal_cmu_get_aon_revision_id();
    metal_id = GET_BITFIELD(metal_id, CHIP_REV_ID);
    if (metal_id == 0x4) {
        metal_id = HAL_CHIP_METAL_ID_1;
    } else if (metal_id == 0x8) {
        metal_id = HAL_CHIP_METAL_ID_2;
    } else if (metal_id == 0xC) {
        metal_id = HAL_CHIP_METAL_ID_3;
    } else if (metal_id == 0xD) {
        metal_id = HAL_CHIP_METAL_ID_4;
    } else if (metal_id == 0xA) {
        metal_id = HAL_CHIP_METAL_ID_6;
    }

    return metal_id;
}

static void BOOT_TEXT_FLASH_LOC pmu_boot_cause_init(void)
{
    uint16_t val = 0;
    uint16_t val_reason = 0;
    union HAL_HW_BOOTMODE_T hw_bm;

    hw_bm = hal_hw_bootmode_get();

    if (hw_bm.watchdog) {
        pmu_boot_reason = PMU_BOOT_CAUSE_DIG_WDT;
    } else if (hw_bm.global) {
        pmu_boot_reason = PMU_BOOT_CAUSE_DIG_REBOOT;
    } else {
        pmu_read(PMU_REG_BOOT_SOURCE, &val);
        val_reason = GET_BITFIELD(val, ABORT_SOURCE);
        if (val_reason) {
            if (val_reason == 1) {
                pmu_boot_reason = PMU_BOOT_CAUSE_PMU_WDT;
            } else if (val_reason == 2) {
                pmu_boot_reason = PMU_BOOT_CAUSE_LONG_PRESS;
            } else if (val_reason == 3) {
                pmu_boot_reason = PMU_BOOT_CAUSE_VCHG_COMMAND;
            } else if (val_reason == 4) {
                pmu_boot_reason = PMU_BOOT_CAUSE_RESET_KEY;
            }
        } else {
            val_reason = GET_BITFIELD(val, PMU_LDO_ON_SOURCE);
            if (val_reason == 1) {
                pmu_boot_reason = PMU_BOOT_CAUSE_POWER_KEY;
            } else if (val_reason == 2) {
                pmu_boot_reason = PMU_BOOT_CAUSE_RTC;
            } else if (val_reason == 3) {
                pmu_boot_reason = PMU_BOOT_CAUSE_AC_IN;
            } else if (val_reason == 4) {
                pmu_boot_reason = PMU_BOOT_CAUSE_AC_OUT;
            }
        }
#if !(defined(OTA_BOOT_IMAGE) || defined(ARM_CMSE))
        if (pmu_boot_reason != PMU_BOOT_CAUSE_NULL) {
            val = ABORT_SOURCE_CLR | PMU_LDO_ON_SOURCE_CLR;
            pmu_write(PMU_REG_BOOT_SOURCE, val);
            // One lpo clock
            hal_sys_timer_delay_us(30);
            pmu_write(PMU_REG_BOOT_SOURCE, val);
        }
#endif
    }

    pmu_read(PMU_REG_NOT_RESET_61, &val);
    if ((val & REG_NOT_RESET_CHIP_PWR_ON) == 0) {
        pmu_boot_first_pwr_up = true;
    }
#if !(defined(OTA_BOOT_IMAGE) || defined(ARM_CMSE))
    val |= REG_NOT_RESET_CHIP_PWR_ON;
    pmu_write(PMU_REG_NOT_RESET_61, val);
#endif
}

enum PMU_BOOT_CAUSE_T pmu_boot_cause_get(void)
{
    return pmu_boot_reason;
}

bool pmu_boot_first_power_up(void)
{
    return pmu_boot_first_pwr_up;
}

void BOOT_TEXT_FLASH_LOC pmu_boot_init(void)
{
    uint16_t val;

#if defined(PROGRAMMER) && !defined(PROGRAMMER_INFLASH)
    if (usb_reopen) {
        // TODO: Init pmu/rf/ana settings that have no impact on USB (and sys freq)
        return;
    }
#endif

    pmu_boot_cause_init();

#ifdef __WATCHER_DOG_RESET__
    pmu_wdt_save_context();
#endif

#if !defined(FPGA) && !defined(PROGRAMMER)
    pmu_charger_save_context();
#endif

#ifdef PMU_CLK_USE_EXT_CRYSTAL
    // PMU clock sel LPO
    pmu_read(PMU_REG_CLK_SEL_CFG, &val);
    val = SET_BITFIELD(val, CLK_32K_SEL, 0x0);
    pmu_write(PMU_REG_CLK_SEL_CFG, val);
    hal_sys_timer_delay(MS_TO_TICKS(PMU_CLK_SWITCH_STABLE_TIME));
#endif

#ifndef PMU_ALREADY_INIT
    // Reset PMU (to recover from a possible insane state, e.g., ESD reset)
    pmu_write(PMU_REG_METAL_ID, 0xCAFE);
    pmu_write(PMU_REG_METAL_ID, 0x5FEE);
    hal_sys_timer_delay(US_TO_TICKS(500));
#endif

    pmu_read(PMU_REG_CHARGER_CFG, &val);
    val |= REG_CHARGE_INTR_EN;
    pmu_write(PMU_REG_CHARGER_CFG, val);

#if defined(PMU_CLK_USE_EXT_CRYSTAL) || defined(RTC_CLK_USE_EXT_CRYSTAL)
    pmu_external_crystal_init();
#else
    pmu_read(PMU_REG_XO_32K_CFG, &val);
    val &= ~(REG_XO_32K_PU_LDO | REG_XO_32K_PU_LDO_DR);
    pmu_write(PMU_REG_XO_32K_CFG, val);
#endif

#ifdef __WATCHER_DOG_RESET__
    pmu_wdt_restore_context();
#else
    pmu_wdt_stop();
#endif

#if defined(PMU_DCDC_CALIB) && defined(PMU_ALREADY_INIT)
    pmu_get_dcdc_calib_value();
#endif

#if defined(PMU_LDO_VCORE_CALIB) && defined(PMU_ALREADY_INIT)
    pmu_get_ldo_dig_calib_value();
#endif

    pmu_rf_ana_init();
}

void pmu_pll_freq_reg_set(uint16_t low, uint16_t high, uint16_t high2)
{
    int ret;
    uint16_t val;

    val = low;
    ret = rf_write(RF_REG_102F, val);
    if (ret) {
        return;
    }

    val = high;
    ret = rf_write(RF_REG_1030, val);
    if (ret) {
        return;
    }

    rf_read(RF_REG_1031, &val);
    if (REG_LP_BBPLL_SDM_FREQWORD_32 & high2) {
        val |= REG_LP_BBPLL_SDM_FREQWORD_32;
    } else {
        val &= ~REG_LP_BBPLL_SDM_FREQWORD_32;
    }
    ret = rf_write(RF_REG_1031, val);
    if (ret) {
        return;
    }

    ret = rf_read(RF_REG_102D, &val);
    if (ret) {
        return;
    }
    val |= REG_LP_BBPLL_FREQ_EN_COMB;
    ret = rf_write(RF_REG_102D, val);
    if (ret) {
        return;
    }

    // Delay at least for 7us
    hal_sys_timer_delay(US_TO_TICKS(100));
}

void BOOT_TEXT_FLASH_LOC bbpll_freq_pll_config(uint32_t freq)
{
    uint64_t PLL_cfg_val;
    uint16_t v[3];
    uint32_t crystal = hal_cmu_get_crystal_freq();

    PLL_cfg_val = ((uint64_t)(1 << 28) * freq + crystal / 2) / crystal;
    v[0] = PLL_cfg_val & 0xFFFF;
    v[1] = (PLL_cfg_val >> 16) & 0xFFFF;
    v[2] = (PLL_cfg_val >> 32) & 0xFFFF;

    pmu_pll_freq_reg_set(v[0], v[1], v[2]);
}

void bbpll_codec_clock_enable(bool en)
{
    uint16_t val;

    rf_read(RF_REG_1037, &val);
    if (en) {
        val |= REG_LP_BBPLL_EN_CLK_CODEC;
    } else {
        val &= ~(REG_LP_BBPLL_EN_CLK_CODEC);
    }
    rf_write(RF_REG_1037, val);
}

void BOOT_TEXT_FLASH_LOC bbpll_pll_update(uint64_t pll_cfg_val, bool calib)
{
    uint16_t v[3];
    uint16_t val;
    //bool sys_is_bbpll = false;
    //bool flash_is_bbpll = false;
    //uint32_t lock = 0;

    v[0] = pll_cfg_val & 0xFFFF;
    v[1] = (pll_cfg_val >> 16) & 0xFFFF;
    v[2] = (pll_cfg_val >> 32) & 0xFFFF;

    val = v[0];
    rf_write(RF_REG_102F, val);

    val = v[1];
    rf_write(RF_REG_1030, val);

    rf_read(RF_REG_1031, &val);
    if (REG_LP_BBPLL_SDM_FREQWORD_32 & v[2]) {
        val |= REG_LP_BBPLL_SDM_FREQWORD_32;
    } else {
        val &= ~REG_LP_BBPLL_SDM_FREQWORD_32;
    }
    rf_write(RF_REG_1031, val);

    rf_read(RF_REG_102D, &val);
    val &= ~REG_LP_BBPLL_FREQ_EN_COMB;
    rf_write(RF_REG_102D, val);
}

void bbpll_set_codec_div(uint32_t div)
{
    uint16_t val;

    rf_read(RF_REG_1037, &val);
    val = SET_BITFIELD(val, REG_LP_BBPLL_DIVN_AD, div);
    rf_write(RF_REG_1037, val);
}

void bbpll_set_spd(uint32_t spd)
{
    uint16_t val;

    rf_read(RF_REG_1035, &val);
    val = SET_BITFIELD(val, REG_LP_BBPLL_VCO_SPD, spd);
    rf_write(RF_REG_1035, val);
}

void bbpll_auto_calib_spd(uint32_t codec_freq, uint32_t div)
{
    const uint32_t sys_freq_div = 4;
    const uint32_t f_med = codec_freq * div / sys_freq_div
#ifndef AUD_PLL_DOUBLE
        / 2
#endif
        ;
    const uint32_t calc_ms = 50;
    const float deviation = (float)f_med / hal_cmu_get_crystal_freq() * (1.0f / CONFIG_FAST_SYSTICK_HZ) / (calc_ms / 1000.0f);
    const uint32_t f_min = f_med - (uint32_t)(f_med * deviation * 1.6f);
    const uint32_t f_max = f_med + (uint32_t)(f_med * deviation * 1.2f);
    uint32_t f;
    uint32_t k = 0;
    uint32_t valid_cnt = 0;
    uint32_t valid_start = 0;
    uint32_t valid_end = 0;
    uint32_t cur_cnt = 0;
    uint32_t sel;
    uint32_t lock;

    lock = int_lock();

    const enum HAL_CMU_FREQ_T org_freq = hal_sysfreq_get_hw_freq();
    // Switch to 52M to avoid using PLL
    hal_cmu_sys_set_freq(HAL_CMU_FREQ_52M);
    // Enable PLL
#ifdef ULTRA_LOW_POWER
    hal_cmu_low_freq_mode_disable(org_freq, HAL_CMU_FREQ_208M);
#endif
    // Inc vcore voltage
    pmu_sys_freq_config(HAL_CMU_FREQ_208M);

    analog_aud_freq_pll_config(codec_freq, div);
    DRIVERS_TRACE(0,"codec_freq = %d, div = %d", codec_freq, div);
    DRIVERS_TRACE(0,"f_med = %d, deviation = %f", f_med, (double)deviation);
    hal_cmu_force_sys_pll_div(sys_freq_div);

    for (k = 0; k < 8; k++) {
        // Switch to 52M to avoid using PLL
        hal_cmu_sys_set_freq(HAL_CMU_FREQ_52M);

        bbpll_set_spd(k);

        // Switch to PLL freq
        hal_cmu_sys_set_freq(HAL_CMU_FREQ_208M);

        f = hal_sys_timer_calc_cpu_freq(calc_ms, 1);
        if (f_min < f && f < f_max) {
            cur_cnt++;
        } else {
            if (cur_cnt) {
                if (cur_cnt > valid_cnt) {
                    valid_cnt = cur_cnt;
                    valid_end = k - 1;
                }
                cur_cnt = 0;
            }
        }
        DRIVERS_TRACE(0, "[%c%2u] f=%u min=%u max=%u", cur_cnt ? '+' : '-', k, f, f_min, f_max);
    }

    if (cur_cnt) {
        if (cur_cnt > valid_cnt) {
            valid_cnt = cur_cnt;
            valid_end = k - 1;
        }
    }
    if (valid_cnt) {
        valid_start = valid_end + 1 - valid_cnt;
        sel = (valid_start + valid_end) / 2;
        DRIVERS_TRACE(0, "Valid spd=[%2u, %2u] cnt=%2u sel=%2u/0x%02X", valid_start, valid_end, valid_cnt, sel, sel);
    } else {
        sel = 0x0;
        DRIVERS_TRACE(0, "BAD: no valid spd");
    }
    bbpll_set_spd(sel);
    // Restore the sys freq
    hal_cmu_force_sys_pll_div(0);
    hal_cmu_sys_set_freq(org_freq);
    // Restore PLL status
#ifdef ULTRA_LOW_POWER
    hal_cmu_low_freq_mode_enable(HAL_CMU_FREQ_208M, org_freq);
#endif
    // Restore vcore voltage
    pmu_sys_freq_config(org_freq);

    int_unlock(lock);
}

#define CODEC_FREQ                  (48000 * 512)
#define PLL_DIV                     (16)
#define CRYSTAL                     (24000000)
void BOOT_TEXT_FLASH_LOC pmu_rf_ana_init(void)
{
    uint64_t PLL_cfg_val;
    uint16_t POSSIBLY_UNUSED val;

#if defined(PMU_FULL_INIT) || (!defined(FPGA) && !defined(PROGRAMMER) && !defined(PMU_ALREADY_INIT))
    // Reset RF
    rf_write(RF_REG_00, 0xCAFE);
    rf_write(RF_REG_00, 0x5FEE);
    hal_sys_timer_delay(US_TO_TICKS(500));

    // Reset ANA
    ana_write(ANA_REG_60, 0xCAFE);
    ana_write(ANA_REG_60, 0x5FEE);
    hal_sys_timer_delay(US_TO_TICKS(500));

    // Reset CAP
    ana_read(ANA_REG_5A, &val);
    val &= ~(SOFT_RESETN_CAPSENSOR | SOFT_RESETN_CAPSENSOR_REG);
    ana_write(ANA_REG_5A, val);
    hal_sys_timer_delay(US_TO_TICKS(500));
#endif

    //Reduce the current of xtal
    uint16_t vtune_val = 0x2;

    rf_read(RF_REG_100C, &val);
    val = SET_BITFIELD(val, REG_XTAL_LDO_VTUNE_OTHER, vtune_val);
    rf_write(RF_REG_100C,val);

    rf_read(RF_REG_1016, &val);
    val = SET_BITFIELD(val, REG_XTAL_LDO_VTUNE_CH22, vtune_val);
    val = SET_BITFIELD(val, REG_XTAL_LDO_VTUNE_CH46, vtune_val);
    val = SET_BITFIELD(val, REG_XTAL_LDO_VTUNE_CH70, vtune_val);
    rf_write(RF_REG_1016,val);

    rf_read(RF_REG_1043, &val);
    val |= REG_XTAL_MDLL_PFD_ERROR_DET_EN;
    rf_write(RF_REG_1043, val);

    rf_read(RF_REG_1044, &val);
    val = SET_BITFIELD(val, REG_XTAL_MDLL_LPF_PRECH_BIAS, 0x2);
    val = SET_BITFIELD(val, REG_XTAL_MDLL_DLL_SWRC, 0x0);
    rf_write(RF_REG_1044,val);

    rf_read(RF_REG_100E, &val);
    val = SET_BITFIELD(val, REG_XTAL_MDLL_CP_SWRC, 0x2);
    val = SET_BITFIELD(val, REG_XTAL_MDLL_CLK_ADC_SEL_I_4M, 0x1);
    rf_write(RF_REG_100E,val);

    rf_read(RF_REG_1052, &val);
    val = SET_BITFIELD(val, REG_XTAL_SWRC_ACBUF_SUB, 0x4);
    val = SET_BITFIELD(val, REG_XTAL_SWRC_ACBUF_MAIN, 0x8);
    val = SET_BITFIELD(val, REG_XTAL_ACBUF_MAIN_SEL, 0x3);
    rf_write(RF_REG_1052,val);

    rf_read(RF_REG_1050, &val);
    val = SET_BITFIELD(val, REG_XTAL_MAIN_ICORE_SEL, 0x4);
    rf_write(RF_REG_1050,val);

    // init pll 393MHz
    // CODEC_FREQ is likely 24.576M (48K series) or 22.5792M (44.1K series)
    PLL_cfg_val = ((uint64_t)(1 << 25) * CODEC_FREQ * PLL_DIV + CRYSTAL / 2) / CRYSTAL;
    bbpll_pll_update(PLL_cfg_val, true);

    rf_read(RF_REG_1037, &val);
#if defined(RC_CLK_ENABLE) || (defined(PSRAM_ENABLE) && (PSRAM_SPEED >= 200))
    val &= ~REG_LP_BBPLL_DIG_CLK_FREQ_SEL;
#else
    val |= REG_LP_BBPLL_DIG_CLK_FREQ_SEL;
#endif
    rf_write(RF_REG_1037, val);
}

void BOOT_TEXT_SRAM_LOC pmu_pll_div_reset_set(enum HAL_CMU_PLL_T pll)
{
    uint32_t lock;
    uint16_t val;

    lock = int_lock();
    if (pll == HAL_CMU_PLL_AUD) {
        rf_read(RF_REG_F5, &val);
        val |= REG_BT_BBPLL_DIVN_CODEC_MASK;
        rf_write(RF_REG_F5, val);
    }
    int_unlock(lock);
}

void BOOT_TEXT_SRAM_LOC pmu_pll_div_reset_clear(enum HAL_CMU_PLL_T pll)
{
    uint32_t lock;
    uint16_t val;

    lock = int_lock();
    if (pll == HAL_CMU_PLL_AUD) {
        rf_read(RF_REG_F5, &val);
        val = SET_BITFIELD(val, REG_BT_BBPLL_DIVN_CODEC, bbpll_codec_div);
        rf_write(RF_REG_F5, val);
    }
    int_unlock(lock);
}

void pmu_pll_div_set(enum HAL_CMU_PLL_T pll, enum PMU_PLL_DIV_TYPE_T type, uint32_t div)
{
    uint32_t lock;
    uint16_t val;

    if (type != PMU_PLL_DIV_CODEC) {
        return;
    }

    lock = int_lock();
    if (pll == HAL_CMU_PLL_AUD) {
        if (div != bbpll_codec_div) {
            bbpll_codec_div = div;
            rf_read(RF_REG_F5, &val);
            val |= REG_BT_BBPLL_DIVN_CODEC_MASK;
            rf_write(RF_REG_F5, val);
            if (div != (REG_BT_BBPLL_DIVN_CODEC_MASK >> REG_BT_BBPLL_DIVN_CODEC_SHIFT)) {
                val = SET_BITFIELD(val, REG_BT_BBPLL_DIVN_CODEC, bbpll_codec_div);
                rf_write(RF_REG_F5, val);
            }
        }
    }
    int_unlock(lock);
}

int pmu_get_security_value(union SECURITY_VALUE_T *val)
{
    int ret;

    val->reg = 0;

    ret = pmu_get_efuse(PMU_EFUSE_PAGE_SECURITY, &val->reg);
    if (ret) {
        // Error
        return ret;
    }

    return 0;
}

enum HAL_FLASH_ID_T pmu_get_boot_flash_ctrl_id(void)
{
#ifdef FLASH1_CTRL_BASE
    union SECURITY_VALUE_T security;

    pmu_get_security_value(&security);

    return (security.root.flash_id) ? HAL_FLASH_ID_1 : HAL_FLASH_ID_0;
#else
    return HAL_FLASH_ID_0;
#endif
}

static int BOOT_TEXT_SRAM_LOC pmu_get_efuse_phy(enum PMU_EFUSE_PAGE_T page, unsigned short *efuse)
{
    int ret;
    unsigned short val;
    unsigned short tmp[4];
    unsigned short rval;

#ifdef EFUSE_READ_WORKAROUND
    *efuse = 0;
    return 0;
#endif

    // Sel fast clk
    pmu_read(PMU_REG_EFUSE_CLOCK_CTRL, &val);
    if ((val & REG_EFUSE_CLK_OSC_SEL) == 0) {
        val = REG_EFUSE_CLK_OSC_SEL;
        ret = pmu_write(PMU_REG_EFUSE_CLOCK_CTRL, val);
        if (ret) {
            goto _exit;
        }
        hal_sys_timer_delay_us(100);
    }

    // Enable CLK_EN
    val = REG_EFUSE_CLK_EN;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        goto _exit;
    }

    // Enable TURN_ON
    val |= REG_EFUSE_TURN_ON;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        goto _exit;
    }
    hal_sys_timer_delay_us(30);

    for(uint32_t i = 0; i < 4; i++) {
        // Write Address
#ifdef PMU_EFUSE_NO_REDUNDANCY
        val = SET_BITFIELD(val, REG_EFUSE_ADDRESS, ((page / 2) * 4 + i));
#else
        val = SET_BITFIELD(val, REG_EFUSE_ADDRESS, (page * 4 + i)); //redundancy
#endif
        ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
        if (ret) {
            goto _exit;
        }
        hal_sys_timer_delay_us(2);

        // Set Strobe Trigger = 1
        val |= REG_EFUSE_STROBE_TRIGGER;
        ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
        if (ret) {
            goto _exit;
        }

        // set Strobe Trigger = 0
        val &= ~REG_EFUSE_STROBE_TRIGGER;
        ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
        if (ret) {
            goto _exit;
        }
        hal_sys_timer_delay_us(2);

        // Read Efuse
        ret = pmu_read(PMU_REG_EFUSE_DATA, &rval);
        if (ret) {
            goto _exit;
        }
        tmp[i] = rval;
    }

#ifdef PMU_EFUSE_NO_REDUNDANCY
    *efuse = (page % 2) ? (tmp[2] | (tmp[3] << 8)) : (tmp[0] | (tmp[1] << 8));
#else
    *efuse = (tmp[0] | (tmp[1] << 8)) | (tmp[2] | (tmp[3] << 8)); //redundancy
#endif

    _exit:
    // Disable TURN_ON
    val &= ~(REG_EFUSE_TURN_ON | REG_EFUSE_ADDRESS_MASK);
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);

    // Disable CLK_EN
    val &= ~REG_EFUSE_CLK_EN;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);

    return ret;
}

#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
#if !defined(ROM_BUILD) && defined(PMU_EFUSE_INIT_IN_BOOT)
static void BOOT_TEXT_SRAM_LOC pmu_efuse_init(void)
{
    uint32_t i;

    for (i = 0; i < PMU_EFUSE_PAGE_QTY; i++) {
        pmu_get_efuse_phy(i, &pmu_efuse_page_arrays[i]);
    }

    efuse_buf_valid = true;
}
#endif
#endif

int BOOT_TEXT_SRAM_LOC pmu_get_efuse(enum PMU_EFUSE_PAGE_T page, unsigned short *efuse)
{
#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
#if !defined(ROM_BUILD) && defined(PMU_EFUSE_INIT_IN_BOOT)
    if (efuse_buf_valid) {
        *efuse = pmu_efuse_page_arrays[page];
        return 0;
    }
#endif
#endif
    return pmu_get_efuse_phy(page, efuse);
}

static void pmu_sys_ctrl(bool shutdown)
{
    uint16_t val;
    uint32_t lock = int_lock();

    PMU_INFO_TRACE_IMM(0, "Start pmu %s", shutdown ? "shutdown" : "reboot");

#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
#ifdef RC_CLK_ENABLE
    hal_cmu_pu_osc_enable(HAL_CMU_PU_OSC_USER_PMU_SYS_CTRL);
    hal_sys_timer_delay_us(HAL_CMU_OSC_STABLE_TIME);
#endif

#if defined(MCU_HIGH_PERFORMANCE_MODE)
    // Default vcore might not be high enough to support high performance mode
    pmu_high_performance_mode_enable(false);
#endif
    hal_cmu_sys_set_freq(HAL_CMU_FREQ_26M);
    pmu_vcore_req |= PMU_VCORE_PMU_RESET;

#ifdef SPLIT_CORE_M_CORE_L
    pmu_read(PMU_REG_VCORE_BYPASS_CFG, &val);
    val |= REG_LDO_VCORE_L_BYPASS_DR | REG_LDO_VCORE_L_BYPASS;
    pmu_write(PMU_REG_VCORE_BYPASS_CFG, val);
    hal_sys_timer_delay(MS_TO_TICKS(1));
#endif

#ifdef PMU_FORCE_LP_MODE
    pmu_bandgap_cfg_reset();
#endif

    pmu_module_ramp_volt(PMU_MEM, PMU_VMEM_DEFAULT_V, PMU_VMEM_DEFAULT_V);
#ifndef PMU_VMEM_PWR_SUPPLY_IO
    pmu_module_config(PMU_MEM,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
    hal_sys_timer_delay_us(PMU_LDO_PU_STABLE_TIME_US);
#endif

#ifndef HPPA_LDO_ON
    pmu_hppa_dcdc_to_ldo();
#endif
    pmu_mode_change(PMU_POWER_MODE_LDO);
    hal_sys_timer_delay(MS_TO_TICKS(1));
#ifdef VRTC_SWITCH_TO_VCORE
    pmu_read(PMU_REG_LDO_VRTC_VOLT, &val);
    val = SET_BITFIELD(val, REG_LDO_VRTC_VBIT_NORMAL, PMU_LDO_VRTC_DEFAULT_ACT);
    val = SET_BITFIELD(val, REG_LDO_VRTC_VBIT_RC, PMU_LDO_VRTC_DEFAULT_ACT);
    val = SET_BITFIELD(val, REG_LDO_VRTC_VBIT_DSLEEP, PMU_LDO_VRTC_DEFAULT_LP);
    pmu_write(PMU_REG_LDO_VRTC_VOLT, val);

    pmu_read(PMU_REG_LDO_VRTC_CFG, &val);
    val |= REG_LDO_VRTC_VBIT_LOAD;
    pmu_write(PMU_REG_LDO_VRTC_CFG, val);
    hal_sys_timer_delay_us(PMU_LDO_VRTC_STABLE_TIME_US);

    pmu_read(PMU_REG_LPO_VRTC_CFG_D2, &val);
    val &= ~REG_VRTC_OVERRIDE;
    pmu_write(PMU_REG_LPO_VRTC_CFG_D2, val);
    hal_sys_timer_delay_us(PMU_LDO_VRTC_STABLE_TIME_US);
#endif
#endif

#ifdef PMU_CLK_USE_EXT_CRYSTAL
    uint16_t val_32k_sel;

    // PMU clock sel LPO
    pmu_read(PMU_REG_CLK_SEL_CFG, &val_32k_sel);
    val_32k_sel = SET_BITFIELD(val_32k_sel, CLK_32K_SEL, 0x0);
    pmu_write(PMU_REG_CLK_SEL_CFG, val_32k_sel);
    hal_sys_timer_delay(MS_TO_TICKS(PMU_CLK_SWITCH_STABLE_TIME));
#endif

    // Reset PMU
    pmu_write(PMU_REG_METAL_ID, 0xCAFE);
    pmu_write(PMU_REG_METAL_ID, 0x5FEE);
    hal_sys_timer_delay(US_TO_TICKS(500));

#ifdef PMU_CLK_USE_EXT_CRYSTAL
    // PMU clock sel EXT32K
    val_32k_sel = SET_BITFIELD(val_32k_sel, CLK_32K_SEL, 0x2);
    pmu_write(PMU_REG_CLK_SEL_CFG, val_32k_sel);
    hal_sys_timer_delay(MS_TO_TICKS(PMU_CLK_SWITCH_STABLE_TIME));
#endif

    pmu_read(PMU_REG_CHARGER_CFG, &val);
    val |= REG_CHARGE_INTR_EN;
    pmu_write(PMU_REG_CHARGER_CFG, val);

#ifdef RTC_CALENDAR
#ifndef PMU_CLK_USE_EXT_CRYSTAL
    if (shutdown) {
        uint32_t old_systick = CONFIG_SYSTICK_HZ;

        pmu_read(PMU_REG_LP_RTC_CFG, &val);
        val |= LP_MODE_RTC_REG | LP_MODE_RTC_DR;
        pmu_write(PMU_REG_LP_RTC_CFG, val);

        hal_sys_timer_delay_us(50);

        hal_sys_timer_calib();

        val &= ~(LP_MODE_RTC_REG | LP_MODE_RTC_DR);
        pmu_write(PMU_REG_LP_RTC_CFG, val);

        hal_sys_timer_delay_us(50);

        PMU_INFO_TRACE_IMM(0, "%s: Calib lp lpo freq done! freq(act/lp)=%u/%u",
                __func__, old_systick, CONFIG_SYSTICK_HZ);
    }
#endif
    pmu_rtc_calendar_init();
#endif

    if (shutdown) {
#if defined(_AUTO_TEST_)
        if (at_skip_shutdown) {
            hal_cmu_sys_reboot();
            return;
        }
#endif

#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
        pmu_wdt_config(3*1000, 3*1000);
        pmu_wdt_start();
#endif

#if defined(PMU_CLK_USE_EXT_CRYSTAL) || defined(RTC_CLK_USE_EXT_CRYSTAL)
        if (pmu_ext_crystal_clkout_usr_map) {
            pmu_external_crystal_clkout_after_pwroff();
        }
#endif

        pmu_read(PMU_REG_USB_CFG_3F, &val);
#ifdef CHG_OUT_PWRON
        val |= AC_OUT_LDO_ON_EN;
#else
        val &= ~AC_OUT_LDO_ON_EN;
#endif
        pmu_write(PMU_REG_USB_CFG_3F, val);

        // Power off
        pmu_read(PMU_REG_POWER_OFF,&val);
        val |= SOFT_POWER_OFF;
        for (int i = 0; i < 100; i++) {
            pmu_write(PMU_REG_POWER_OFF,val);
            hal_sys_timer_delay(MS_TO_TICKS(5));
        }

        hal_sys_timer_delay(MS_TO_TICKS(50));

        //can't reach here
        PMU_INFO_TRACE_IMM(0, "\nError: pmu shutdown failed!\n");
        hal_sys_timer_delay(MS_TO_TICKS(5));
    } else {
#if defined(PMU_FULL_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
        // CAUTION:
        // 1) Never reset RF because system or flash might be using X2/X4, which are off by default
        // 2) Never reset RF/ANA because system or flash might be using PLL, and the reset might cause clock glitch
        // TODO:
        // Restore BBPLL settings in RF
#endif
    }

    hal_cmu_sys_reboot();

    hal_sys_timer_delay(MS_TO_TICKS(5));
    hal_cmu_sys_reboot();
    for (int i = 0; i < 10; i++) {
        hal_sys_timer_delay(MS_TO_TICKS(1));
    }

    // Use pmu wdt reset instead of global reset, bootmode will be cleared
    pmu_wdt_reboot();
    while (1) {}

    int_unlock(lock);
}

void pmu_shutdown(void)
{
    pmu_sys_ctrl(true);
}

void pmu_reboot(void)
{
    pmu_sys_ctrl(false);
}

static inline uint16_t pmu_get_module_addr(enum PMU_MODULE_T module)
{
    if (module == PMU_ANA) {
        return PMU_REG_LDO_ANA_CFG;
    } else if (module == PMU_CORE) {
        return PMU_REG_LDO_CORE_CFG;
    } else if (module == PMU_CORE_L) {
        return PMU_REG_LDO_CORE_L_CFG;
    } else if (module == PMU_CODEC) {
        return PMU_REG_LDO_CODEC_CFG;
    } else if (module == PMU_USB) {
        return PMU_REG_LDO_USB_CFG;
    } else if (module == PMU_SENSOR) {
        return PMU_REG_LDO_SENSOR_CFG;
    } else {
        return PMU_REG_LDO_MEM_CFG;
    }
}

static inline uint16_t pmu_get_module_volt_addr(enum PMU_MODULE_T module)
{
    if (module == PMU_ANA) {
        return PMU_REG_LDO_ANA_VOLT;
    } else if (module == PMU_CORE) {
        return PMU_REG_LDO_CORE_VOLT;
    } else if (module == PMU_CORE_L) {
        return PMU_REG_LDO_CORE_L_VOLT;
    } else if (module == PMU_CODEC) {
        return PMU_REG_LDO_CODEC_VOLT;
    } else if (module == PMU_USB) {
        return PMU_REG_LDO_USB_VOLT;
    } else if (module == PMU_SENSOR) {
        return PMU_REG_LDO_SENSOR_VOLT;
    } else {
        return PMU_REG_LDO_MEM_VOLT;
    }
}

void pmu_module_config(enum PMU_MODULE_T module,unsigned short is_manual,unsigned short ldo_on,unsigned short lp_mode,unsigned short dpmode)
{
    unsigned short val;
    unsigned short module_address;
    const struct PMU_MODULE_CFG_T *module_cfg_p = &pmu_module_cfg[module];

    module_address = pmu_get_module_addr(module);

    pmu_read(module_address, &val);
    if (is_manual) {
        val |= module_cfg_p->manual_bit;
    } else {
        val &= ~module_cfg_p->manual_bit;
    }
    if (ldo_on) {
        val |= module_cfg_p->ldo_en;
    } else {
        val &= ~module_cfg_p->ldo_en;
    }

    if (module != PMU_CORE_L) {
        // Set res_sel_dr by default
        val |= module_cfg_p->res_sel_dr;
    }

#ifdef PMU_FORCE_LP_MODE
    if (pmu_power_mode != PMU_POWER_MODE_NONE) {
        val |= module_cfg_p->lp_en_dr;
        if (ldo_on) {
            val |= module_cfg_p->lp_en;
        } else {
            val &= ~module_cfg_p->lp_en;
        }
    }
#else
    if (lp_mode) {
        val &= ~module_cfg_p->lp_en_dr;
    } else {
        val = (val & ~module_cfg_p->lp_en) | module_cfg_p->lp_en_dr;
    }
#endif

    // Avoid pulldown reg is enabled by pmu_ldo_vsensor_off_pull_down() and not call pmu_ldo_vsensor_on_lp_dr() to resume pulldown reg.
    if (module == PMU_SENSOR && ldo_on) {
        val &= ~REG_LDO_VSENSOR_PULLDOWN;
    }

    if (dpmode) {
        val |= module_cfg_p->dsleep_mode;
    } else {
        val &= ~module_cfg_p->dsleep_mode;
    }
    pmu_write(module_address, val);
}

#ifdef PMU_FORCE_LP_MODE
void pmu_module_force_lp_config(void)
{
    uint16_t val;
    uint16_t module_address;
    const struct PMU_MODULE_CFG_T *module_cfg_p;
    enum PMU_MODULE_T module;
    bool ldo_on;

    for (module = 0; module < PMU_MODULE_QTY; module++) {
        module_cfg_p = &pmu_module_cfg[module];
        module_address = pmu_get_module_addr(module);

        pmu_read(module_address, &val);
        ldo_on = !!(val & module_cfg_p->ldo_en);
        val |= module_cfg_p->lp_en_dr;
        if (ldo_on) {
            val |= module_cfg_p->lp_en;
        } else {
            val &= ~module_cfg_p->lp_en;
        }
        pmu_write(module_address, val);
    }
}
#endif

void pmu_module_set_volt(unsigned char module, unsigned short sleep_v, unsigned short normal_v)
{
    unsigned short val;
    unsigned short module_volt_address;
    unsigned short rc_v = normal_v;
    const struct PMU_MODULE_CFG_T *module_cfg_p = &pmu_module_cfg[module];

#ifdef PMU_LDO_VCORE_CALIB
    if (module == PMU_CORE_L) {
        normal_v = pmu_reg_val_add(normal_v, pmu_ldo_dig_l_comp, MAX_LDO_VCORE_L_BIT_VAL);
        sleep_v = pmu_reg_val_add(sleep_v, pmu_ldo_lp_dig_l_comp, MAX_LDO_VCORE_L_BIT_VAL);
        rc_v = pmu_reg_val_add(rc_v, pmu_ldo_lp_dig_l_comp, MAX_LDO_VCORE_L_BIT_VAL);
    }
#endif

    module_volt_address = pmu_get_module_volt_addr(module);

    pmu_read(module_volt_address, &val);
    val &= ~module_cfg_p->normal_v;
    val |= (normal_v << module_cfg_p->normal_v_shift) & module_cfg_p->normal_v;
    if (module <= PMU_CODEC) {
        val &= ~module_cfg_p->dsleep_v;
        val |= (sleep_v << module_cfg_p->dsleep_v_shift) & module_cfg_p->dsleep_v;
#ifdef PMU_VCORE_RC_MODE_FIXED
        if (module != PMU_CORE)
#endif
        {
            val &= ~module_cfg_p->rc_v;
            val |= (rc_v << module_cfg_p->rc_v_shift) & module_cfg_p->rc_v;
        }
    }
    pmu_write(module_volt_address, val);
    if (module > PMU_CODEC) {
        module_volt_address += 1;

        pmu_read(module_volt_address, &val);
        val &= ~module_cfg_p->dsleep_v;
        val |= (sleep_v << module_cfg_p->dsleep_v_shift) & module_cfg_p->dsleep_v;
#ifdef PMU_VCORE_RC_MODE_FIXED
        if (module != PMU_CORE_L)
#endif
        {
            val &= ~module_cfg_p->rc_v;
            val |= (rc_v << module_cfg_p->rc_v_shift) & module_cfg_p->rc_v;
        }
        pmu_write(module_volt_address, val);
    }

#ifdef SPLIT_CORE_M_CORE_L
    if (module == PMU_CORE_L) {
        if (!dcdc_ramp_en) {
            if (normal_v > sleep_v || rc_v > sleep_v) {
                dcdc_ramp_map |= REG_VCORE_L_RAMP_EN;
            } else {
                dcdc_ramp_map &= ~REG_VCORE_L_RAMP_EN;
            }
        } else {
            pmu_read(PMU_REG_LDO_CORE_L_CFG, &val);
            val |= REG_LDO_VCORE_L_VBIT_LOAD;
            pmu_write(PMU_REG_LDO_CORE_L_CFG, val);
        }
    }
#endif
}

int pmu_module_get_volt(unsigned char module, unsigned short *sleep_vp, unsigned short *normal_vp)
{
    unsigned short val;
    unsigned short module_volt_address;
    const struct PMU_MODULE_CFG_T *module_cfg_p = &pmu_module_cfg[module];

    module_volt_address = pmu_get_module_volt_addr(module);

    pmu_read(module_volt_address, &val);
    if (normal_vp) {
        *normal_vp = (val & module_cfg_p->normal_v) >> module_cfg_p->normal_v_shift;
    }

    if (module <= PMU_CODEC) {
        if (sleep_vp) {
            *sleep_vp = (val & module_cfg_p->dsleep_v) >> module_cfg_p->dsleep_v_shift;
        }
    } else {
        module_volt_address += 1;
        pmu_read(module_volt_address, &val);
        if (sleep_vp) {
            *sleep_vp = (val & module_cfg_p->dsleep_v) >> module_cfg_p->dsleep_v_shift;
        }
    }

#ifdef PMU_LDO_VCORE_CALIB
    if (module == PMU_CORE_L) {
        if (normal_vp) {
            *normal_vp = pmu_reg_val_add(*normal_vp, -pmu_ldo_dig_l_comp, MAX_LDO_VCORE_L_BIT_VAL);
        }
        if (sleep_vp) {
            *sleep_vp = pmu_reg_val_add(*sleep_vp, -pmu_ldo_lp_dig_l_comp, MAX_LDO_VCORE_L_BIT_VAL);
        }
    }
#endif

    return 0;
}

static void pmu_module_ramp_volt(unsigned char module, unsigned short sleep_v, unsigned short normal_v)
{
    uint16_t old_normal_v;
    uint16_t old_sleep_v;

    pmu_module_get_volt(module, &old_sleep_v, &old_normal_v);

    if (old_normal_v < normal_v) {
        while (old_normal_v < normal_v) {
            if (old_normal_v + 2 < normal_v) {
                old_normal_v += 2;
            } else {
                old_normal_v = normal_v;
            }
            pmu_module_set_volt(module, sleep_v, old_normal_v);
        }
#ifndef DIG_DCDC_MODE
    } else if (old_normal_v > normal_v && module == PMU_CORE) {
        while (old_normal_v > normal_v) {
            if (old_normal_v > normal_v + 2) {
                old_normal_v -= 2;
            } else {
                old_normal_v = normal_v;
            }
            pmu_module_set_volt(module, sleep_v, old_normal_v);
            hal_sys_timer_delay_us(100);
        }
#endif
    } else if (old_normal_v != normal_v || old_sleep_v != sleep_v) {
        pmu_module_set_volt(module, sleep_v, normal_v);
    }
}

static void pmu_dcdc_ana_get_volt(unsigned short *normal_vp, unsigned short *dsleep_vp)
{
    unsigned short val;

    pmu_read(PMU_REG_BUCK_VANA_CFG, &val);
    if (normal_vp) {
        *normal_vp = GET_BITFIELD(val, REG_BUCK_VANA_VBIT_NORMAL);
    }

    pmu_read(PMU_REG_BUCK_VANA_LP_CFG, &val);
    if (dsleep_vp) {
        *dsleep_vp = GET_BITFIELD(val, REG_BUCK_VANA_VBIT_DSLEEP);
    }

#ifdef PMU_DCDC_CALIB
    if (normal_vp) {
        *normal_vp = pmu_reg_val_add(*normal_vp, -pmu_dcdc_ana_comp, MAX_BUCK_VANA_BIT_VAL);
    }
    if (dsleep_vp) {
        *dsleep_vp = pmu_reg_val_add(*dsleep_vp, -pmu_ana_lp_comp, MAX_BUCK_VANA_BIT_VAL);
    }
#endif
}

static void pmu_dcdc_ana_set_volt(unsigned short normal_v,unsigned short dsleep_v)
{
    unsigned short val;
    unsigned short rc_v = normal_v;

#ifdef PMU_DCDC_CALIB
    normal_v = pmu_reg_val_add(normal_v, pmu_dcdc_ana_comp, MAX_BUCK_VANA_BIT_VAL);
    dsleep_v = pmu_reg_val_add(dsleep_v, pmu_ana_lp_comp, MAX_BUCK_VANA_BIT_VAL);
    rc_v = pmu_reg_val_add(rc_v, pmu_ana_lp_comp, MAX_BUCK_VANA_BIT_VAL);
#endif

    pmu_read(PMU_REG_BUCK_VANA_CFG, &val);
    val = SET_BITFIELD(val, REG_BUCK_VANA_VBIT_NORMAL, normal_v);
    pmu_write(PMU_REG_BUCK_VANA_CFG, val);

    pmu_read(PMU_REG_BUCK_VANA_LP_CFG, &val);
    val = SET_BITFIELD(val, REG_BUCK_VANA_VBIT_DSLEEP, dsleep_v);
    pmu_write(PMU_REG_BUCK_VANA_LP_CFG, val);

    pmu_read(PMU_REG_BUCK_VANA_RC_CFG, &val);
    val = SET_BITFIELD(val, REG_BUCK_VANA_VBIT_RC, rc_v);
    pmu_write(PMU_REG_BUCK_VANA_RC_CFG, val);

    if (!dcdc_ramp_en) {
        if (normal_v > dsleep_v || rc_v > dsleep_v) {
            dcdc_ramp_map |= REG_DCDC2_RAMP_EN;
        } else {
            dcdc_ramp_map &= ~REG_DCDC2_RAMP_EN;
        }
    }
}

static void pmu_ana_set_volt(enum PMU_POWER_MODE_T mode)
{
    uint16_t old_act_dcdc;
    uint16_t old_lp_dcdc;
    uint16_t new_act_dcdc;

    new_act_dcdc = ana_act_dcdc;

    if (mode == PMU_POWER_MODE_ANA_DCDC || mode == PMU_POWER_MODE_DIG_DCDC) {
        pmu_dcdc_ana_get_volt(&old_act_dcdc, &old_lp_dcdc);
        if (old_act_dcdc < new_act_dcdc) {
            while (old_act_dcdc++ < new_act_dcdc) {
                pmu_dcdc_ana_set_volt(old_act_dcdc, ana_lp_dcdc);
            }
            hal_sys_timer_delay_us(PMU_VANA_STABLE_TIME_US);
        } else if (old_act_dcdc != new_act_dcdc || old_lp_dcdc != ana_lp_dcdc) {
            pmu_dcdc_ana_set_volt(new_act_dcdc, ana_lp_dcdc);
        }
    } else {
        pmu_dcdc_ana_set_volt(new_act_dcdc, ana_lp_dcdc);
    }
}

static void pmu_dcdc_dig_get_volt(unsigned short *normal_vp, unsigned short *dsleep_vp)
{
    unsigned short val;

    pmu_read(PMU_REG_BUCK_VCORE_CFG, &val);
    if (normal_vp) {
        *normal_vp = GET_BITFIELD(val, REG_BUCK_VCORE_VBIT_NORMAL);
    }

    pmu_read(PMU_REG_BUCK_VCORE_LP_CFG, &val);
    if (dsleep_vp) {
        *dsleep_vp = GET_BITFIELD(val, REG_BUCK_VCORE_VBIT_DSLEEP);
    }

#ifdef PMU_DCDC_CALIB
    if (normal_vp) {
        *normal_vp = pmu_reg_val_add(*normal_vp, -pmu_dcdc_dig_comp, MAX_BUCK_VCORE_BIT_VAL);
    }
    if (dsleep_vp) {
        *dsleep_vp = pmu_reg_val_add(*dsleep_vp, -pmu_dig_lp_comp, MAX_BUCK_VCORE_BIT_VAL);
    }
#endif
}

static void pmu_dcdc_dig_set_volt(unsigned short normal_v,unsigned short dsleep_v)
{
    unsigned short val;
    unsigned short rc_v =
#ifdef PMU_VCORE_RC_MODE_FIXED
        PMU_VCORE_RC_MODE_FIXED_DCDC_DIG;
#else
        normal_v;
#endif

#ifdef PMU_DCDC_CALIB
    normal_v = pmu_reg_val_add(normal_v, pmu_dcdc_dig_comp, MAX_BUCK_VCORE_BIT_VAL);
    dsleep_v = pmu_reg_val_add(dsleep_v, pmu_dig_lp_comp, MAX_BUCK_VCORE_BIT_VAL);
    rc_v = pmu_reg_val_add(rc_v, pmu_dig_lp_comp, MAX_BUCK_VCORE_BIT_VAL);
#endif

    pmu_read(PMU_REG_BUCK_VCORE_CFG, &val);
    val = SET_BITFIELD(val, REG_BUCK_VCORE_VBIT_NORMAL, normal_v);
    pmu_write(PMU_REG_BUCK_VCORE_CFG, val);

    pmu_read(PMU_REG_BUCK_VCORE_LP_CFG, &val);
    val = SET_BITFIELD(val, REG_BUCK_VCORE_VBIT_DSLEEP, dsleep_v);
    pmu_write(PMU_REG_BUCK_VCORE_LP_CFG, val);

#ifndef PMU_VCORE_RC_MODE_FIXED
    pmu_read(PMU_REG_BUCK_VCORE_RC_CFG, &val);
    val = SET_BITFIELD(val, REG_BUCK_VCORE_VBIT_RC, rc_v);
    pmu_write(PMU_REG_BUCK_VCORE_RC_CFG, val);
#endif

    pmu_read(PMU_REG_BUCK_VCORE_EN, &val);
    val |= REG_BUCK_VCORE_VBIT_LOAD;
    pmu_write(PMU_REG_BUCK_VCORE_EN, val);

    if (!dcdc_ramp_en) {
        if (normal_v > dsleep_v || rc_v > dsleep_v) {
            dcdc_ramp_map |= REG_DCDC1_RAMP_EN;
        } else {
            dcdc_ramp_map &= ~REG_DCDC1_RAMP_EN;
        }
    }
}

#ifdef MTEST_ENABLED
bool pmu_dig_set_volt_value(uint16_t dcdc_volt_mv, uint16_t logic_volt_mv, uint16_t ldo_volt_mv)
{
    uint16_t dcdc_volt, old_dcdc_volt;
    uint16_t POSSIBLY_UNUSED logic_volt;
    uint32_t i, find = 0;
    bool success = false;

    const static uint16_t dcdc_volt_list[][2] = {
        {1100, PMU_DCDC_DIG_1_1V },
        {1050, PMU_DCDC_DIG_1_05V},
        {1000, PMU_DCDC_DIG_1_0V },
        {950,  PMU_DCDC_DIG_0_95V},
        {900,  PMU_DCDC_DIG_0_9V },
        {850,  PMU_DCDC_DIG_0_85V},
        {840,  PMU_DCDC_DIG_0_84V},
        {800,  PMU_DCDC_DIG_0_8V },
        {750,  PMU_DCDC_DIG_0_75V},
        {740,  PMU_DCDC_DIG_0_74V},
        {700,  PMU_DCDC_DIG_0_7V },
        {650,  PMU_DCDC_DIG_0_65V},
        {600,  PMU_DCDC_DIG_0_6V },
        {550,  PMU_DCDC_DIG_0_55V},
        {500,  PMU_DCDC_DIG_0_5V },
    };

    const static uint16_t logic_volt_list[][2] = {
        {1050, PMU_VDIG_L_1_05V},
        {1000, PMU_VDIG_L_1_0V },
        {950,  PMU_VDIG_L_0_95V},
        {900,  PMU_VDIG_L_0_9V },
        {850,  PMU_VDIG_L_0_85V},
        {800,  PMU_VDIG_L_0_8V },
        {750,  PMU_VDIG_L_0_75V},
        {700,  PMU_VDIG_L_0_7V },
        {650,  PMU_VDIG_L_0_65V},
        {600,  PMU_VDIG_L_0_6V },
        {550,  PMU_VDIG_L_0_55V},
        {500,  PMU_VDIG_L_0_5V },
        {450,  PMU_VDIG_L_0_45V},
    };

    for (i = 0; i < ARRAY_SIZE(dcdc_volt_list); i++) {
        if (dcdc_volt_mv == dcdc_volt_list[i][0]) {
            dcdc_volt = dcdc_volt_list[i][1];
            find |= 0x1;
            break;
        }
    }
    for (i = 0; i < ARRAY_SIZE(logic_volt_list); i++) {
        if (logic_volt_mv == logic_volt_list[i][0]) {
            logic_volt = logic_volt_list[i][1];
            find |= 0x2;
            break;
        }
    }
    if (find == 0x3) {
        pmu_dcdc_dig_get_volt(&old_dcdc_volt, NULL);
        if (old_dcdc_volt < dcdc_volt) {
            while (old_dcdc_volt++ < dcdc_volt) {
                pmu_dcdc_dig_set_volt(old_dcdc_volt, dig_lp_dcdc);
            }
#ifdef SPLIT_CORE_M_CORE_L
            pmu_module_set_volt(PMU_CORE_L, dig_lp_logic, logic_volt);
#endif
        } else if (old_dcdc_volt != dcdc_volt) {
#ifdef SPLIT_CORE_M_CORE_L
            pmu_module_set_volt(PMU_CORE_L, dig_lp_logic, logic_volt);
#endif
            pmu_dcdc_dig_set_volt(dcdc_volt, dig_lp_dcdc);
        }
        success = true;
    }
    return success;
}
#endif /* MTEST_ENABLED */

static void BOOT_TEXT_SRAM_LOC pmu_dig_get_target_volt(uint16_t *ldo, uint16_t *dcdc, uint16_t *logic)
{
    uint16_t ldo_volt;
    uint16_t dcdc_volt;
    uint16_t logic_volt;

    if (0) {
    } else if (pmu_vcore_req & PMU_VCORE_PMU_RESET) {
        ldo_volt = PMU_VDIG_DEFAULT;
        dcdc_volt = PMU_DCDC_DIG_0_85V - PMU_DCDC_DIG_50_MV;
        logic_volt = PMU_VDIG_L_DEFAULT;
#if defined(MCU_HIGH_PERFORMANCE_MODE)
    } else if (pmu_vcore_req & (PMU_VCORE_SYS_FREQ_HIGH)) {
        if (high_perf_freq_mhz <= 260) {
            ldo_volt = PMU_VDIG_1_05V;
            dcdc_volt = PMU_DCDC_DIG_1_05V;
            logic_volt = PMU_VDIG_L_1_0V;
        } else {
            ldo_volt = PMU_VDIG_1_1V;
            dcdc_volt = PMU_DCDC_DIG_1_1V;
            logic_volt = PMU_VDIG_L_1_05V;
        }
#endif
    } else if (pmu_vcore_req & (PMU_VCORE_PSRAM_FREQ_HIGH | PMU_VCORE_SYS_FREQ_MEDIUM)) {
        ldo_volt = PMU_VDIG_0_9V;
        dcdc_volt = PMU_DCDC_DIG_0_88V;
        logic_volt = PMU_VDIG_L_0_85V;
    } else {
        // Common cases
        ldo_volt = PMU_VDIG_0_8V;
        dcdc_volt = PMU_DCDC_DIG_0_78V;
        logic_volt = PMU_VDIG_L_0_7V;
    }

#ifdef PMU_VCORE_RISE_MV
    ldo_volt += (PMU_VDIG_0_8V - PMU_VDIG_0_75V) * (PMU_VCORE_RISE_MV / 25) / 2;
    dcdc_volt += (PMU_DCDC_DIG_0_8V - PMU_DCDC_DIG_0_75V) * (PMU_VCORE_RISE_MV / 25) / 2;
    logic_volt += (PMU_VDIG_L_0_8V - PMU_VDIG_L_0_75V) * (PMU_VCORE_RISE_MV / 25) / 2;
#endif

#if (defined(PROGRAMMER) || defined(__BES_OTA_MODE__)) && !defined(PMU_FULL_INIT)
    // Try to keep the same vcore voltage as ROM (hardware default)
    if (ldo_volt < PMU_VDIG_DEFAULT) {
        ldo_volt = PMU_VDIG_DEFAULT;
    }
    if (dcdc_volt < PMU_DCDC_DIG_DEFAULT) {
        dcdc_volt = PMU_DCDC_DIG_DEFAULT;
    }
    if (logic_volt < PMU_VDIG_L_DEFAULT) {
        logic_volt = PMU_VDIG_L_DEFAULT;
    }
#endif

#ifdef DIG_DCDC_MODE
    if (ldo_volt < PMU_VDIG_DEFAULT) {
        ldo_volt = PMU_VDIG_DEFAULT;
    }
#endif

    if (ldo) {
        *ldo = ldo_volt;
    }
    if (dcdc) {
        *dcdc = dcdc_volt;
    }
    if (logic) {
        *logic = logic_volt;
    }
}

static void pmu_dig_set_volt(enum PMU_POWER_MODE_T mode)
{
    uint32_t lock;
    uint32_t delay_us;
    uint32_t delta_raw;
    uint16_t dcdc_volt, old_act_dcdc, old_lp_dcdc;
    uint16_t ldo_volt, old_act_ldo, old_lp_ldo;
    uint16_t logic_volt;
    bool volt_inc = false;
    uint8_t ramp_step = 12;
#ifdef SPLIT_CORE_M_CORE_L
    uint32_t delta_raw_logic = 0;
    uint16_t old_act_logic, old_lp_logic;
    uint8_t ramp_step_logic = 8;
    bool cfg_logic_done = false;
#endif

    lock = int_lock();

    if (mode == PMU_POWER_MODE_NONE) {
        mode = pmu_power_mode;
    }

    pmu_dig_get_target_volt(&ldo_volt, &dcdc_volt, &logic_volt);

    pmu_module_get_volt(PMU_CORE, &old_lp_ldo, &old_act_ldo);
    pmu_dcdc_dig_get_volt(&old_act_dcdc, &old_lp_dcdc);
#ifdef SPLIT_CORE_M_CORE_L
    pmu_module_get_volt(PMU_CORE_L, &old_lp_logic, &old_act_logic);
#endif

    if (mode == PMU_POWER_MODE_DIG_DCDC) {
        if (old_act_dcdc < dcdc_volt) {
            volt_inc = true;
            if (dcdc_ramp_en) {
                pmu_dcdc_dig_set_volt(dcdc_volt, dig_lp_dcdc);
            } else {
                while (old_act_dcdc < dcdc_volt) {
                    if (old_act_dcdc + ramp_step < dcdc_volt) {
                        old_act_dcdc += ramp_step;
                    } else {
                        old_act_dcdc = dcdc_volt;
                    }
                    pmu_dcdc_dig_set_volt(old_act_dcdc, dig_lp_dcdc);
                }
            }
        } else if (old_act_dcdc != dcdc_volt || old_lp_dcdc != dig_lp_dcdc) {
#ifdef SPLIT_CORE_M_CORE_L
            if (old_act_logic != logic_volt) {
                pmu_module_set_volt(PMU_CORE_L, dig_lp_logic, logic_volt);
                cfg_logic_done = true;
            }
#endif
            pmu_dcdc_dig_set_volt(dcdc_volt, dig_lp_dcdc);
        }
        // Update the voltage of the other mode
        pmu_module_set_volt(PMU_CORE, dig_lp_ldo, ldo_volt);
    } else {
        if (old_act_ldo < ldo_volt) {
            volt_inc = true;
        }
        pmu_module_ramp_volt(PMU_CORE, dig_lp_ldo, ldo_volt);
        // Update the voltage of the other mode
        pmu_dcdc_dig_set_volt(dcdc_volt, dig_lp_dcdc);
    }

#ifdef SPLIT_CORE_M_CORE_L
    if (old_act_logic < logic_volt) {
        volt_inc = true;
        if (dcdc_ramp_en) {
            pmu_module_set_volt(PMU_CORE_L, dig_lp_logic, logic_volt);
        } else {
            while (old_act_logic < logic_volt) {
                if (old_act_logic + ramp_step_logic < logic_volt) {
                    old_act_logic += ramp_step_logic;
                } else {
                    old_act_logic = logic_volt;
                }
                pmu_module_set_volt(PMU_CORE_L, dig_lp_logic, logic_volt);
            }
        }
    } else if ((old_act_logic != logic_volt || old_lp_logic != dig_lp_logic) && !cfg_logic_done) {
        pmu_module_set_volt(PMU_CORE_L, dig_lp_logic, logic_volt);
    }
#endif

    if (volt_inc) {
        if (dcdc_ramp_en) {
            if (mode != PMU_POWER_MODE_DIG_DCDC) {
                delay_us = PMU_VCORE_STABLE_TIME_US;
            } else {
                delta_raw = (dcdc_volt - old_act_dcdc) / PMU_BUCK_VCORE_HW_RAMP_STEP;
#ifdef SPLIT_CORE_M_CORE_L
                delta_raw_logic = (logic_volt - old_act_logic) / PMU_VCORE_L_HW_RAMP_STEP;
                if (delta_raw < delta_raw_logic) {
                    delta_raw = delta_raw_logic;
                }
#endif
                // Add 1 clk for rounding, 3 clks for 32k sync
                delta_raw += 4;
                delay_us = delta_raw * PMU_HW_RAMP_PER_CLK_TIME_US + PMU_VCORE_STABLE_TIME_US;
            }
        } else {
            delay_us = PMU_VCORE_STABLE_TIME_US;
        }
        hal_sys_timer_delay_us(delay_us);
    }

    int_unlock(lock);
}

static void pmu_ana_reserved_en(bool en)
{
    uint16_t val;

    pmu_read(PMU_REG_CA, &val);
    if (en) {
        val |= REG_PD_LDO_ANA;
    } else {
        val &= ~REG_PD_LDO_ANA;
    }
    pmu_write(PMU_REG_CA, val);
}

static void pmu_ldo_mode_en(void)
{
    unsigned short val;

    // The load under ana_ldo should be turned on after ana_ldo enabled.
    pmu_ana_reserved_en(true);

    // Enable vana ldo
    pmu_module_config(PMU_ANA,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
    // Enable vcore ldo
    pmu_module_config(PMU_CORE,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
    hal_sys_timer_delay_us(PMU_LDO_PU_STABLE_TIME_US);

    // Disable vana dcdc
    pmu_read(PMU_REG_BUCK_VANA_EN, &val);
    val = (val & ~REG_PU_BUCK_VANA) | REG_PU_BUCK_VANA_DR;
    pmu_write(PMU_REG_BUCK_VANA_EN, val);

    // Disable vcore dcdc
    pmu_read(PMU_REG_BUCK_VCORE_EN, &val);
    val = (val & ~REG_PU_BUCK_VCORE) | REG_PU_BUCK_VCORE_DR;
    pmu_write(PMU_REG_BUCK_VCORE_EN, val);
}

static void pmu_dcdc_ana_mode_en(void)
{
    unsigned short val;

    if (pmu_power_mode == PMU_POWER_MODE_DIG_DCDC) {
        // Enable vcore ldo
        pmu_module_config(PMU_CORE,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
        hal_sys_timer_delay_us(PMU_LDO_PU_STABLE_TIME_US);

        // Disable vcore dcdc
        pmu_read(PMU_REG_BUCK_VCORE_EN, &val);
        val = (val & ~REG_PU_BUCK_VCORE) | REG_PU_BUCK_VCORE_DR;
        pmu_write(PMU_REG_BUCK_VCORE_EN, val);
    } else {
        // Enable vana dcdc
        pmu_read(PMU_REG_BUCK_VANA_EN, &val);
        val |= REG_PU_BUCK_VANA_DR | REG_PU_BUCK_VANA;
        pmu_write(PMU_REG_BUCK_VANA_EN, val);
        hal_sys_timer_delay_us(PMU_DCDC_PU_STABLE_TIME_US);

        // Disable vana ldo
        pmu_module_config(PMU_ANA,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);

        // The load under ana_ldo should be turned off after ana_ldo disabled.
        pmu_ana_reserved_en(false);
    }
}

static void pmu_dcdc_dual_mode_en(void)
{
    unsigned short val;

    // Enable vana dcdc
    pmu_read(PMU_REG_BUCK_VANA_EN, &val);
    val |= REG_PU_BUCK_VANA_DR | REG_PU_BUCK_VANA;
    pmu_write(PMU_REG_BUCK_VANA_EN, val);
    hal_sys_timer_delay_us(PMU_DCDC_PU_STABLE_TIME_US);

    // Enable vcore dcdc
    pmu_read(PMU_REG_BUCK_VCORE_EN, &val);
    val |= REG_PU_BUCK_VCORE_DR | REG_PU_BUCK_VCORE;
    pmu_write(PMU_REG_BUCK_VCORE_EN, val);
    hal_sys_timer_delay_us(PMU_DCDC_PU_STABLE_TIME_US);

    // Disable vana ldo
    pmu_module_config(PMU_ANA,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
    // Disable vcore ldo
    pmu_module_config(PMU_CORE,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);

    // The load under ana_ldo should be turned off after ana_ldo disabled.
    pmu_ana_reserved_en(false);
}

void pmu_mode_change(enum PMU_POWER_MODE_T mode)
{
    uint32_t lock;

#ifndef PMU_ALREADY_INIT
    if (pmu_power_mode == mode || mode == PMU_POWER_MODE_NONE) {
        return;
    }
#endif

    lock = int_lock();

    pmu_ana_set_volt(pmu_power_mode);
    pmu_dig_set_volt(pmu_power_mode);

    if (mode == PMU_POWER_MODE_ANA_DCDC) {
        pmu_dcdc_ana_mode_en();
    } else if (mode == PMU_POWER_MODE_DIG_DCDC) {
        pmu_dcdc_dual_mode_en();
    } else if (mode == PMU_POWER_MODE_LDO) {
        pmu_ldo_mode_en();
    }

    pmu_power_mode = mode;

    int_unlock(lock);
}

void pmu_sleep_en(unsigned char sleep_en)
{
    unsigned short val;

    pmu_read(PMU_REG_SLEEP_CFG, &val);
    if(sleep_en) {
        val |= SLEEP_ALLOW;
    } else {
        val &= ~SLEEP_ALLOW;
    }
    pmu_write(PMU_REG_SLEEP_CFG, val);

    pmu_read(PMU_REG_RC_SLEEP_CFG, &val);
    if (sleep_en) {
        val |= REG_RC_SLEEP_ALLOW;
    } else {
        val &= ~REG_RC_SLEEP_ALLOW;
    }
    pmu_write(PMU_REG_RC_SLEEP_CFG, val);
}

#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
#ifndef HPPA_LDO_ON
static uint32_t pmu_dcdc_hppa_mv_to_val(uint16_t mv)
{
    uint32_t val;

    if (mv == 1600) {
        val = PMU_DCDC_HPPA_1_6V;
    } else if (mv == 1650) {
        val = PMU_DCDC_HPPA_1_65V;
    } else if (mv == 1700) {
        val = PMU_DCDC_HPPA_1_7V;
    } else if (mv == 1750) {
        val = PMU_DCDC_HPPA_1_75V;
    } else if (mv == 1900) {
        val = PMU_DCDC_HPPA_1_9V;
    } else {
        val = PMU_DCDC_HPPA_1_8V;
    }

    return val;
}

POSSIBLY_UNUSED
static void pmu_dcdc_hppa_get_volt(unsigned short *normal_vp, unsigned short *dsleep_vp)
{
    unsigned short val;

    pmu_read(PMU_REG_BUCK_VHPPA_CFG, &val);
    if (normal_vp) {
        *normal_vp = GET_BITFIELD(val, REG_BUCK_VHPPA_VBIT_NORMAL);
    }

    pmu_read(PMU_REG_BUCK_VHPPA_LP_CFG, &val);
    if (dsleep_vp) {
        *dsleep_vp = GET_BITFIELD(val, REG_BUCK_VHPPA_VBIT_DSLEEP);
    }

#ifdef PMU_DCDC_CALIB
    if (normal_vp) {
        *normal_vp = pmu_reg_val_add(*normal_vp, -pmu_dcdc_hppa_comp, MAX_BUCK_VHPPA_BIT_VAL);
    }
    if (dsleep_vp) {
        *dsleep_vp = pmu_reg_val_add(*dsleep_vp, -pmu_hppa_lp_comp, MAX_BUCK_VHPPA_BIT_VAL);
    }
#endif
}

static void pmu_dcdc_hppa_set_volt(unsigned short normal_v,unsigned short dsleep_v)
{
    unsigned short val;
    unsigned short rc_v = normal_v;

#ifdef PMU_DCDC_CALIB
    normal_v = pmu_reg_val_add(normal_v, pmu_dcdc_hppa_comp, MAX_BUCK_VHPPA_BIT_VAL);
    dsleep_v = pmu_reg_val_add(dsleep_v, pmu_hppa_lp_comp, MAX_BUCK_VHPPA_BIT_VAL);
    rc_v = pmu_reg_val_add(rc_v, pmu_hppa_lp_comp, MAX_BUCK_VHPPA_BIT_VAL);
#endif

    pmu_read(PMU_REG_BUCK_VHPPA_CFG, &val);
    val = SET_BITFIELD(val, REG_BUCK_VHPPA_VBIT_NORMAL, normal_v);
    pmu_write(PMU_REG_BUCK_VHPPA_CFG, val);

    pmu_read(PMU_REG_BUCK_VHPPA_LP_CFG, &val);
    val = SET_BITFIELD(val, REG_BUCK_VHPPA_VBIT_DSLEEP, dsleep_v);
    pmu_write(PMU_REG_BUCK_VHPPA_LP_CFG, val);

    pmu_read(PMU_REG_BUCK_VHPPA_RC_CFG, &val);
    val = SET_BITFIELD(val, REG_BUCK_VHPPA_VBIT_RC, rc_v);
    pmu_write(PMU_REG_BUCK_VHPPA_RC_CFG, val);

    if (!dcdc_ramp_en) {
        if (normal_v > dsleep_v || rc_v > dsleep_v) {
            dcdc_ramp_map |= REG_DCDC3_RAMP_EN;
        } else {
            dcdc_ramp_map &= ~REG_DCDC3_RAMP_EN;
        }
    }
}

static void pmu_dcdc_hppa_en(int enable)
{
    uint16_t val;

    pmu_read(PMU_REG_BUCK_VHPPA_EN, &val);
    val |= REG_PU_BUCK_VHPPA_DR;
    if (enable) {
        val |= REG_PU_BUCK_VHPPA;
    } else {
        val &= ~REG_PU_BUCK_VHPPA;
    }
    pmu_write(PMU_REG_BUCK_VHPPA_EN, val);
}

static void pmu_hppa_dcdc_to_ldo(void)
{
    pmu_module_set_volt(PMU_CODEC, PMU_CODEC_DEFAULT_V, PMU_CODEC_DEFAULT_V);
    pmu_module_config(PMU_CODEC,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_OFF,PMU_DSLEEP_MODE_OFF);
    hal_sys_timer_delay_us(PMU_LDO_PU_STABLE_TIME_US);
    pmu_dcdc_hppa_en(false);
}

#ifdef PMU_VMEM_PWR_SUPPLY_IO
static uint32_t pmu_ldo_vmem_mv_to_val(uint16_t mv)
{
    uint32_t val;

    if (mv == 1600) {
        val = PMU_VMEM_1_6V;
    } else if (mv == 1700) {
        val = PMU_VMEM_1_7V;
    } else {
        val = PMU_VMEM_1_8V;
    }

    return val;
}
#endif
#endif

int pmu_codec_volt_ramp_up(void)
{
    return 0;
}

int pmu_codec_volt_ramp_down(void)
{
    return 0;
}

static void BOOT_TEXT_FLASH_LOC pmu_dig_init_volt(void)
{
    uint16_t ldo_volt;
    uint16_t val;

    pmu_dig_get_target_volt(&ldo_volt, NULL, NULL);

    pmu_read(PMU_REG_LDO_CORE_VOLT, &val);
    if (GET_BITFIELD(val, REG_LDO_VCORE_VBIT_NORMAL) < ldo_volt) {
        val = SET_BITFIELD(val, REG_LDO_VCORE_VBIT_NORMAL, ldo_volt);
        pmu_write(PMU_REG_LDO_CORE_VOLT, val);
    }
}

#ifdef PMU_REG_CFG_DUMP
static void pmu_reg_val_print(void)
{
    uint16_t val;

    DRIVERS_TRACE(0, "%s start", __func__);
    for (uint16_t reg = 0x00; reg <= 0x199; reg++) {
        pmu_read(reg, &val);
        TRACE_IMM(0, "reg=val 0x%04X=0x%04X", reg, val);
    }

    for (uint16_t reg = PMU_EFUSE_PAGE_SECURITY; reg <= PMU_EFUSE_PAGE_1F; reg++) {
        pmu_get_efuse(reg, &val);
        TRACE_IMM(0, "efuse=val 0x%04X=0x%04X", reg, val);
    }
    DRIVERS_TRACE(0, "%s done", __func__);
}
#endif

#ifdef PMU_VCORE_RC_MODE_FIXED
// Don't need ramp because MCU don't use RC clock.
static void pmu_vcore_rc_mode_init(void)
{
    uint16_t rc_v;
    uint16_t val;

    // VCORE-M-DCDC-RC
    rc_v = PMU_VCORE_RC_MODE_FIXED_DCDC_DIG;

#ifdef PMU_DCDC_CALIB
    rc_v = pmu_reg_val_add(rc_v, pmu_dig_lp_comp, MAX_BUCK_VCORE_BIT_VAL);
#endif

    pmu_read(PMU_REG_BUCK_VCORE_RC_CFG, &val);
    val = SET_BITFIELD(val, REG_BUCK_VCORE_VBIT_RC, rc_v);
    pmu_write(PMU_REG_BUCK_VCORE_RC_CFG, val);

    // VCORE-M-LDO-RC
    pmu_read(PMU_REG_LDO_CORE_VOLT, &val);
    val = SET_BITFIELD(val, REG_LDO_VCORE_VBIT_RC, PMU_VCORE_RC_MODE_FIXED_VDIG);
    pmu_write(PMU_REG_LDO_CORE_VOLT, val);

#ifdef SPLIT_CORE_M_CORE_L
    // VCORE-L-RC
    rc_v = PMU_VCORE_RC_MODE_FIXED_VDIG_L;

#ifdef PMU_LDO_VCORE_CALIB
    rc_v = pmu_reg_val_add(rc_v, pmu_ldo_lp_dig_l_comp, MAX_LDO_VCORE_L_BIT_VAL);
#endif

    pmu_read(PMU_REG_LDO_CORE_L_LP_RC_VOLT, &val);
    val = SET_BITFIELD(val, REG_LDO_VCORE_L_VBIT_RC, rc_v);
    pmu_write(PMU_REG_LDO_CORE_L_LP_RC_VOLT, val);
#endif
}
#endif

#ifdef PMU_VMIN_SEARCH
#ifdef RTC_CALENDAR
#error "PMU_VMIN_SEARCH will use RTC reg, please check it."
#endif

static void pmu_vmin_search_enable(void)
{
    uint16_t val;
    int8_t vmin_offset;

    DRIVERS_TRACE(0, "%s reg_addr=0x%x", __func__, PMU_VMIN_SET_REG_ADDR);

    pmu_write(PMU_VMIN_SET_REG_ADDR, 0);

    while (1) {
        pmu_read(PMU_VMIN_SET_REG_ADDR, &val);
        if (val & PMU_VMIN_VCORE_SET_DONE) {
            break;
        }
    }

    if (val & PMU_VMIN_VCORE_SET_PLUS) {
        vmin_offset = GET_BITFIELD(val, PMU_VMIN_VCORE_SET_OFFSET);
    } else {
        vmin_offset = -GET_BITFIELD(val, PMU_VMIN_VCORE_SET_OFFSET);
    }

    pmu_dcdc_dig_comp += vmin_offset;

    DRIVERS_TRACE(0, "%s vmin_offset%d pmu_dcdc_dig_comp=%d", __func__, vmin_offset, pmu_dcdc_dig_comp);
}
#endif

// Target voltage 1.8V
static void pmu_set_sar_adc_vref(void)
{
    int8_t vref_comp;
    uint16_t val_efuse;
    uint16_t val_sar_vreg_sel = 0x17;
    uint16_t val;

    pmu_get_efuse(PMU_EFUSE_PAGE_14, &val_efuse);
    vref_comp = GET_BITFIELD(val_efuse, PMU_EFUSE_SAR_ADC_VREF_COMP);
    if (val_efuse & PMU_EFUSE_SAR_ADC_VREF_COMP_SIGN) {
        vref_comp = -vref_comp;
    }

    pmu_read(PMU_REG_SAR_VREF_CFG_E6, &val);
    val_sar_vreg_sel += vref_comp;
    val = SET_BITFIELD(val, REG_SAR_VREG_SEL, val_sar_vreg_sel);
    pmu_write(PMU_REG_SAR_VREF_CFG_E6, val);

    DRIVERS_TRACE(0, "%s Calibration done! vref_comp=%d", __func__, vref_comp);
}

#ifdef PMU_FORCE_LP_MODE
static void pmu_bandgap_cfg_reset(void)
{
    uint16_t val;

    pmu_read(PMU_REG_BIAS_CFG, &val);
    val |= PU_BIAS_LDO_DR | BG_CONSTANT_GM_BIAS_DR | BG_CORE_EN_DR | BG_VTOI_EN_DR | BG_NOTCH_EN_DR | BG_VBG_SEL_DR |
        PU_BIAS_LDO_REG | BG_CONSTANT_GM_BIAS_REG | BG_CORE_EN_REG | BG_VTOI_EN_REG | BG_NOTCH_EN_REG;
    pmu_write(PMU_REG_BIAS_CFG, val);
    hal_sys_timer_delay_us(PMU_BIG_BG_STABLE_TIME_US);

    val |= BG_VBG_SEL_REG;
    pmu_write(PMU_REG_BIAS_CFG, val);
}
#endif
#endif

static void pmu_buck_volt_hw_ramp_enable(uint8_t map)
{
    uint16_t val;

    if (dcdc_ramp_map & REG_DCDC1_RAMP_EN) {
        pmu_read(PMU_REG_BUCK_VCORE_EN, &val);
        val |= REG_BUCK_VCORE_RAMP_EN;
        pmu_write(PMU_REG_BUCK_VCORE_EN, val);
    }

    if (dcdc_ramp_map & REG_DCDC2_RAMP_EN) {
        pmu_read(PMU_REG_BUCK_VANA_EN, &val);
        val |= REG_BUCK_VANA_RAMP_EN;
        pmu_write(PMU_REG_BUCK_VANA_EN, val);
    }

    if (dcdc_ramp_map & REG_DCDC3_RAMP_EN) {
        pmu_read(PMU_REG_BUCK_VHPPA_EN, &val);
        val |= REG_BUCK_VHPPA_RAMP_EN;
        pmu_write(PMU_REG_BUCK_VHPPA_EN, val);
    }

#ifdef SPLIT_CORE_M_CORE_L
    if (dcdc_ramp_map & REG_VCORE_L_RAMP_EN) {
        pmu_read(PMU_REG_LDO_CORE_L_CFG, &val);
        val |= REG_LDO_VCORE_L_RAMP_EN;
        pmu_write(PMU_REG_LDO_CORE_L_CFG, val);
    }
#endif
}

static void pmu_buck_volt_hw_ramp_disable(uint8_t map)
{
    uint16_t val;

    if (dcdc_ramp_map & REG_DCDC1_RAMP_EN) {
        pmu_read(PMU_REG_BUCK_VCORE_EN, &val);
        val &= ~REG_BUCK_VCORE_RAMP_EN;
        pmu_write(PMU_REG_BUCK_VCORE_EN, val);
    }

    if (dcdc_ramp_map & REG_DCDC2_RAMP_EN) {
        pmu_read(PMU_REG_BUCK_VANA_EN, &val);
        val &= ~REG_BUCK_VANA_RAMP_EN;
        pmu_write(PMU_REG_BUCK_VANA_EN, val);
    }

    if (dcdc_ramp_map & REG_DCDC3_RAMP_EN) {
        pmu_read(PMU_REG_BUCK_VHPPA_EN, &val);
        val &= ~REG_BUCK_VHPPA_RAMP_EN;
        pmu_write(PMU_REG_BUCK_VHPPA_EN, val);
    }

#ifdef SPLIT_CORE_M_CORE_L
    if (dcdc_ramp_map & REG_VCORE_L_RAMP_EN) {
        pmu_read(PMU_REG_LDO_CORE_L_CFG, &val);
        val &= ~REG_LDO_VCORE_L_RAMP_EN;
        pmu_write(PMU_REG_LDO_CORE_L_CFG, val);
    }
#endif
}

static void pmu_sar_adc_vref_sw_pu(bool pu)
{
    uint16_t val;

    pmu_read(PMU_REG_SAR_VREF_CFG_E6, &val);
    if (pu) {
        val &= ~REG_SAR_PD_VREF_ADC_SW;
    } else {
        val |= REG_SAR_PD_VREF_ADC_SW;
    }
    pmu_write(PMU_REG_SAR_VREF_CFG_E6, val);
}

#if defined(ARM_CMSE)
STATIC_ASSERT(sizeof(((struct HAL_CMU_NS_BOOT_PARAM_T *)0)->pmu_vcore_req) == sizeof(pmu_vcore_req),
    "Bad pmu_vcore_req type");

void pmu_set_ns_vcore_req(struct HAL_CMU_NS_BOOT_PARAM_T *param)
{
    param->pmu_vcore_req = pmu_vcore_req;
}

#elif defined(ARM_CMNS)
BOOT_TEXT_FLASH_LOC
void pmu_get_ns_vcore_req(const struct HAL_CMU_NS_BOOT_PARAM_T *param)
{
    pmu_vcore_req = param->pmu_vcore_req;
}

#endif

POSSIBLY_UNUSED static BOOT_BSS_LOC bool pmu_opened;

int BOOT_TEXT_FLASH_LOC pmu_open(void)
{
#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))

    uint16_t val;
    uint16_t POSSIBLY_UNUSED val_efuse, val_cal;
    enum PMU_POWER_MODE_T mode;

    if (pmu_opened) {
        return 0;
    }

    pmu_opened = true;

    ASSERT(vcodec_mv == 1400 || vcodec_mv == 1450 || vcodec_mv == 1500 || vcodec_mv == 1550 || vcodec_mv == 1600 || vcodec_mv == 1650 ||
        vcodec_mv == 1700 || vcodec_mv == 1750 || vcodec_mv == 1800 || vcodec_mv == 1900,
        "Invalid vcodec cfg: vcodec_mv=%u", vcodec_mv);
    ASSERT(vhppa_mv == 1400 || vhppa_mv == 1450 || vhppa_mv == 1500 || vhppa_mv == 1550 || vhppa_mv == 1600 || vhppa_mv == 1650 ||
        vhppa_mv == 1700 || vhppa_mv == 1750 || vhppa_mv == 1800 || vhppa_mv == 1900,
        "Invalid vhppa cfg: vhppa_mv=%u", vhppa_mv);
    DRIVERS_TRACE(0, "%s vcodec_mv:%d vhppa_mv:%d", __func__, vcodec_mv, vhppa_mv);

#ifdef PMU_VMEM_PWR_SUPPLY_IO
    ASSERT(vmem_mv == 1600 || vmem_mv == 1700 || vmem_mv == 1800, "Invalid vmem cfg: vhppa_mv=%u", vmem_mv);
    DRIVERS_TRACE(0, "%s vmem_mv:%d", __func__, vmem_mv);
#endif

#if !defined(ROM_BUILD) && defined(PMU_EFUSE_INIT_IN_BOOT)
    pmu_efuse_init();
#endif

#ifdef PMU_WAFER_PRINT
    pmu_get_efuse(REG_CP_WAFER_CTX_ID, &val);
    DRIVERS_TRACE(0, "wafer_ctx_print: wafer_ID=%d", GET_BITFIELD(val, PMU_EFUSE_CP_WAFER_CTX_ID));

    pmu_get_efuse(REG_CP_WAFER_CTX_XY, &val);
    DRIVERS_TRACE(0, "wafer_ctx_print: wafer_X=%d, wafer_Y=%d",
        GET_BITFIELD(val, PMU_EFUSE_CP_WAFER_CTX_X), GET_BITFIELD(val, PMU_EFUSE_CP_WAFER_CTX_Y));
#endif

    DRIVERS_TRACE(0, "%s boot_cause=0x%04X first_pwron:%d", __func__, pmu_boot_reason, pmu_boot_first_pwr_up);

    // Disable and clear all PMU irqs by default
    pmu_write(PMU_REG_INT_MASK, 0);
    pmu_write(PMU_REG_INT_EN, 0);
    // PMU irqs cannot be cleared by PMU soft reset
    pmu_read(PMU_REG_CHARGER_STATUS, &val);
    pmu_write(PMU_REG_CHARGER_STATUS, val);
    pmu_read(PMU_REG_INT_STATUS, &val);
    pmu_write(PMU_REG_INT_CLR, val);

    // Allow PMU to sleep when power key is pressed
    pmu_read(PMU_REG_POWER_KEY_CFG, &val);
    val &= ~POWERKEY_WAKEUP_OSC_EN;
    pmu_write(PMU_REG_POWER_KEY_CFG, val);

#ifndef PMU_FORCE_LP_MODE
    pmu_read(PMU_REG_BIAS_CFG, &val);
#ifdef FORCE_BIG_BANDGAP
    val |= PU_BIAS_LDO_DR | BG_VBG_SEL_DR | BG_CONSTANT_GM_BIAS_DR | BG_CORE_EN_DR | BG_VTOI_EN_DR;
#else
    // Allow low power bandgap
    val &= ~BG_VBG_SEL_DR;
#endif
    pmu_write(PMU_REG_BIAS_CFG, val);
#endif

    // Disable sar vref output
    pmu_sar_adc_vref_sw_pu(false);
    pmu_set_sar_adc_vref();
    pmu_sar_input_buf_enable(false);
    pmu_read(PMU_REG_SAR_CFG, &val);
    val |= REG_SAR_DONE_DLY_SEL;
    val = SET_BITFIELD(val, REG_SAR_VREG_IBIT, 0x0);
    pmu_write(PMU_REG_SAR_CFG, val);

#ifdef PMU_IRQ_UNIFIED
    pmu_read(PMU_REG_WDT_CFG, &val);
    val |= POWERON_DETECT_EN | MERGE_INTR;
    pmu_write(PMU_REG_WDT_CFG, val);
#endif

#if defined(PMU_DCDC_CALIB) && !defined(PMU_ALREADY_INIT)
    pmu_get_dcdc_calib_value();
    DRIVERS_TRACE(0, "normal/lp vcore-m=%d/%d vana=%d/%d vcodec=%d/%d",
            pmu_dcdc_dig_comp, pmu_dig_lp_comp, pmu_dcdc_ana_comp, pmu_ana_lp_comp, pmu_dcdc_hppa_comp, pmu_hppa_lp_comp);
#endif

#if defined(PMU_LDO_VCORE_CALIB) && !defined(PMU_ALREADY_INIT)
    pmu_get_ldo_dig_calib_value();
    DRIVERS_TRACE(0, "normal/lp vcore-l=%d/%d", pmu_ldo_dig_l_comp, pmu_ldo_lp_dig_l_comp);
#endif

#ifdef PMU_VMIN_SEARCH
    pmu_vmin_search_enable();
#endif

#ifndef NO_SLEEP
    pmu_sleep_en(1);  //enable sleep
#endif

    // Cfg lp bias
    pmu_read(PMU_REG_LDO_LP_BIAS_SEL_VBIT, &val);
    val = SET_BITFIELD(val, REG_LDO_LP_BIAS_VBIT_NORMAL, 0xF);
    val = SET_BITFIELD(val, REG_LDO_LP_BIAS_VBIT_RC, 0xF);
    val = SET_BITFIELD(val, REG_LDO_LP_BIAS_VBIT_DSLEEP, 0x0);
    pmu_write(PMU_REG_LDO_LP_BIAS_SEL_VBIT, val);

    // Default lp_bias_normal is 0xF, so no need ramp.
    // vbit_load used ramp_step bit3
    pmu_read(PMU_REG_LDO_LP_BIAS_RAMP_CFG, &val);
    val |= REG_LDO_LP_BIAS_VBIT_LOAD;
    pmu_write(PMU_REG_LDO_LP_BIAS_RAMP_CFG, val);
    val = SET_BITFIELD(val, REG_LDO_LP_BIAS_RAMP_STEP, 0x1);
    pmu_write(PMU_REG_LDO_LP_BIAS_RAMP_CFG, val);

    // Cfg hw ramp
    // Disable fast ramp by default.
    pmu_read(PMU_REG_FAST_HW_RAMP_CFG, &val);
    val = (val & ~REG_RAMP_FAST_EN) | REG_RAMP_FAST_EN_DR;
    pmu_write(PMU_REG_FAST_HW_RAMP_CFG, val);

    pmu_read(PMU_REG_BUCK_VCORE_EN, &val);
    if (dcdc_ramp_en) {
        val |= REG_BUCK_VCORE_RAMP_EN;
    } else {
        val &= ~REG_BUCK_VCORE_RAMP_EN;
    }
    val = SET_BITFIELD(val, REG_BUCK_VCORE_RAMP_STEP, PMU_BUCK_VCORE_HW_RAMP_STEP);
    pmu_write(PMU_REG_BUCK_VCORE_EN, val);

    pmu_read(PMU_REG_BUCK_VANA_EN, &val);
    if (dcdc_ramp_en) {
        val |= REG_BUCK_VANA_RAMP_EN;
    } else {
        val &= ~REG_BUCK_VANA_RAMP_EN;
    }
    val |= REG_BUCK_VANA_RES_SEL_DR;
    val = SET_BITFIELD(val, REG_BUCK_VANA_RAMP_STEP, PMU_BUCK_VANA_HW_RAMP_STEP);
    pmu_write(PMU_REG_BUCK_VANA_EN, val);

    pmu_read(PMU_REG_BUCK_VHPPA_EN, &val);
    if (dcdc_ramp_en) {
        val |= REG_BUCK_VHPPA_RAMP_EN;
    } else {
        val &= ~REG_BUCK_VHPPA_RAMP_EN;
    }
    val |= REG_BUCK_VHPPA_RES_SEL_DR;
    val = SET_BITFIELD(val, REG_BUCK_VHPPA_RAMP_STEP, PMU_BUCK_VHPPA_HW_RAMP_STEP);
    pmu_write(PMU_REG_BUCK_VHPPA_EN, val);

#ifdef SPLIT_CORE_M_CORE_L
    pmu_read(PMU_REG_LDO_CORE_L_CFG, &val);
    if (dcdc_ramp_en) {
        val |= REG_LDO_VCORE_L_RAMP_EN;
    } else {
        val &= ~REG_LDO_VCORE_L_RAMP_EN;
    }
    val = SET_BITFIELD(val, REG_LDO_VCORE_L_RAMP_STEP, PMU_VCORE_L_HW_RAMP_STEP);
    pmu_write(PMU_REG_LDO_CORE_L_CFG, val);
#endif

    // Cfg dcdc
#ifndef HPPA_LDO_ON
    pmu_read(PMU_REG_BUCK_VHPPA_CFG_E8, &val);
    val &= ~REG_BUCK_VHPPA_PWR_OK_SEL;
    pmu_write(PMU_REG_BUCK_VHPPA_CFG_E8, val);

    pmu_get_efuse(PMU_EFUSE_PAGE_15, &val_efuse);
    val_cal = GET_BITFIELD(val_efuse, PMU_EFUSE_BUCK_VHPPA_IS_GAIN);
    val_cal = (val_cal > 0) ? val_cal : 0x6;

    pmu_read(PMU_REG_BUCK_VHPPA_CFG_144, &val);
    val = SET_BITFIELD(val, REG_BUCK_VHPPA_IS_GAIN_NORMAL, val_cal);
    pmu_write(PMU_REG_BUCK_VHPPA_CFG_144, val);

    pmu_read(PMU_REG_BUCK_VHPPA_CFG_145, &val);
    val = SET_BITFIELD(val, REG_BUCK_VHPPA_IS_GAIN_DSLEEP, val_cal);
    pmu_write(PMU_REG_BUCK_VHPPA_CFG_145, val);

    pmu_read(PMU_REG_BUCK_VHPPA_CFG_146, &val);
    val = SET_BITFIELD(val, REG_BUCK_VHPPA_IS_GAIN_RC, val_cal);
    pmu_write(PMU_REG_BUCK_VHPPA_CFG_146, val);

    pmu_get_efuse(PMU_EFUSE_PAGE_18, &val_efuse);
    val_cal = GET_BITFIELD(val_efuse, PMU_EFUSE_BUCK_VHPPA_BURST_THRESHOLD);
    if (val_cal) {
        pmu_read(PMU_REG_BUCK_HPPA_FREQ_CFG, &val);
        val = SET_BITFIELD(val, REG_BUCK_VHPPA_BURST_THRESHOLD, val_cal);
        pmu_write(PMU_REG_BUCK_HPPA_FREQ_CFG, val);
    }

    pmu_read(PMU_REG_BUCK_HPPA_FREQ_CFG, &val);
    val |= REG_BUCK_VHPPA_SYNC_DISABLE | REG_BUCK_VHPPA_OPT_REG;
    pmu_write(PMU_REG_BUCK_HPPA_FREQ_CFG, val);

    pmu_read(PMU_REG_BUCK_HPPA_CFG_C0, &val);
    val |= REG_BUCK_VHPPA_ICOMP_DC_HIGH;
    pmu_write(PMU_REG_BUCK_HPPA_CFG_C0, val);

    pmu_read(PMU_REG_BUCK_VHPPA_RC_CFG, &val);
    val &= ~(REG_BUCK_VHPPA_COUNTER_SEL_ULP_RC | REG_BUCK_VHPPA_HYSTERESIS_BIT_RC);
    val |= REG_BUCK_VHPPA_IX2_ERR_RC;
    pmu_write(PMU_REG_BUCK_VHPPA_RC_CFG, val);

    pmu_read(PMU_REG_BUCK_VHPPA_CFG_146, &val);
    val &= ~REG_BUCK_VHPPA_ULP_MODE_RC;
    pmu_write(PMU_REG_BUCK_VHPPA_CFG_146, val);
#endif

#ifndef LDO_MODE
    pmu_read(PMU_REG_BUCK_VANA_CFG_E7, &val);
    val &= ~REG_BUCK_VANA_PWR_OK_SEL;
    pmu_write(PMU_REG_BUCK_VANA_CFG_E7, val);

    pmu_get_efuse(PMU_EFUSE_PAGE_15, &val_efuse);
    val_cal = GET_BITFIELD(val_efuse, PMU_EFUSE_BUCK_VANA_IS_GAIN);
    val_cal = (val_cal > 0) ? val_cal : 0x6;

    pmu_read(PMU_REG_BUCK_VANA_CFG_139, &val);
    val = SET_BITFIELD(val, REG_BUCK_VANA_IS_GAIN_NORMAL, val_cal);
    pmu_write(PMU_REG_BUCK_VANA_CFG_139, val);

    pmu_read(PMU_REG_BUCK_VANA_CFG_13A, &val);
    val = SET_BITFIELD(val, REG_BUCK_VANA_IS_GAIN_DSLEEP, val_cal);
    pmu_write(PMU_REG_BUCK_VANA_CFG_13A, val);

    pmu_read(PMU_REG_BUCK_VANA_CFG_140, &val);
    val = SET_BITFIELD(val, REG_BUCK_VANA_IS_GAIN_RC, val_cal);
    pmu_write(PMU_REG_BUCK_VANA_CFG_140, val);

    val_cal = GET_BITFIELD(val_efuse, PMU_EFUSE_BUCK_VCORE_IS_GAIN);
    val_cal = (val_cal > 0) ? val_cal : 0x6;

    pmu_read(PMU_REG_BUCK_VCORE_CFG_141, &val);
    val = SET_BITFIELD(val, REG_BUCK_VCORE_IS_GAIN_NORMAL, val_cal);
    pmu_write(PMU_REG_BUCK_VCORE_CFG_141, val);

    pmu_read(PMU_REG_BUCK_VCORE_CFG_142, &val);
    val = SET_BITFIELD(val, REG_BUCK_VCORE_IS_GAIN_DSLEEP, val_cal);
    pmu_write(PMU_REG_BUCK_VCORE_CFG_142, val);

    pmu_read(PMU_REG_BUCK_VCORE_CFG_143, &val);
    val = SET_BITFIELD(val, REG_BUCK_VCORE_IS_GAIN_RC, val_cal);
    pmu_write(PMU_REG_BUCK_VCORE_CFG_143, val);

    pmu_get_efuse(PMU_EFUSE_PAGE_18, &val_efuse);
    val_cal = GET_BITFIELD(val_efuse, PMU_EFUSE_BUCK_VANA_BURST_THRESHOLD);
    if (val_cal) {
        pmu_read(PMU_REG_BUCK_ANA_CFG_C4, &val);
        val = SET_BITFIELD(val, REG_BUCK_VANA_BURST_THRESHOLD, val_cal);
        pmu_write(PMU_REG_BUCK_ANA_CFG_C4, val);
    }

    val_cal = GET_BITFIELD(val_efuse, PMU_EFUSE_BUCK_VCORE_BURST_THRESHOLD);
    if (val_cal) {
        pmu_read(PMU_REG_BUCK_CORE_CFG_C7, &val);
        val = SET_BITFIELD(val, REG_BUCK_VCORE_BURST_THRESHOLD, val_cal);
        pmu_write(PMU_REG_BUCK_CORE_CFG_C7, val);
    }

    pmu_read(PMU_REG_BUCK_ANA_FREQ_CFG, &val);
    val |= REG_BUCK_VANA_SYNC_DISABLE | REG_BUCK_VANA_OPT_REG;
    pmu_write(PMU_REG_BUCK_ANA_FREQ_CFG, val);

    pmu_read(PMU_REG_BUCK_ANA_CFG_C4, &val);
    val |= REG_BUCK_VANA_ICOMP_DC_HIGH;
    pmu_write(PMU_REG_BUCK_ANA_CFG_C4, val);

    pmu_read(PMU_REG_BUCK_VANA_RC_CFG, &val);
    val &= ~(REG_BUCK_VANA_COUNTER_SEL_ULP_RC | REG_BUCK_VANA_HYSTERESIS_BIT_RC);
    val |= REG_BUCK_VANA_IX2_ERR_RC;
    pmu_write(PMU_REG_BUCK_VANA_RC_CFG, val);

    pmu_read(PMU_REG_BUCK_VANA_CFG_140, &val);
    val &= ~REG_BUCK_VANA_ULP_MODE_RC;
    pmu_write(PMU_REG_BUCK_VANA_CFG_140, val);

    pmu_read(PMU_REG_BUCK_VCORE_CFG_E9, &val);
    val &= ~REG_BUCK_VCORE_PWR_OK_SEL;
    pmu_write(PMU_REG_BUCK_VCORE_CFG_E9, val);

    pmu_read(PMU_REG_DCDC_DIG_SYNC, &val);
    val |= REG_BUCK_VCORE_SYNC_DISABLE | REG_BUCK_VCORE_ICOMP_DC_HIGH;
    pmu_write(PMU_REG_DCDC_DIG_SYNC, val);

    pmu_read(PMU_REG_BUCK_CORE_CFG_63, &val);
    val |= REG_BUCK_VCORE_OPT_REG;
    pmu_write(PMU_REG_BUCK_CORE_CFG_63, val);

    pmu_read(PMU_REG_BUCK_VCORE_RC_CFG, &val);
    val &= ~(REG_BUCK_VCORE_COUNTER_SEL_ULP_RC | REG_BUCK_VCORE_HYSTERESIS_BIT_RC);
    val |= REG_BUCK_VCORE_IX2_ERR_RC;
    pmu_write(PMU_REG_BUCK_VCORE_RC_CFG, val);

    pmu_read(PMU_REG_BUCK_VCORE_CFG_143, &val);
    val &= ~REG_BUCK_VCORE_ULP_MODE_RC;
    pmu_write(PMU_REG_BUCK_VCORE_CFG_143, val);
#endif

#ifndef HPPA_LDO_ON
    // enable vcodec dcdc
    val = pmu_dcdc_hppa_mv_to_val(vhppa_mv);
    pmu_dcdc_hppa_set_volt(val, val);
    pmu_dcdc_hppa_en(true);
    hal_sys_timer_delay_us(PMU_DCDC_PU_STABLE_TIME_US);
    //disable vcodec ldo
    pmu_module_config(PMU_CODEC,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
#endif

#ifdef PMU_VMEM_PWR_SUPPLY_IO
    val = pmu_ldo_vmem_mv_to_val(vmem_mv);
    pmu_module_ramp_volt(PMU_MEM, val, val);
    pmu_module_config(PMU_MEM,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
#else
    pmu_module_config(PMU_MEM,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
#endif

#ifdef VRTC_SWITCH_TO_VCORE
    uint16_t val_vrtc_cfg;

    pmu_read(PMU_REG_LDO_VRTC_VOLT, &val);
    val = SET_BITFIELD(val, REG_LDO_VRTC_VBIT_NORMAL, PMU_LDO_VRTC_0_75V);
    val = SET_BITFIELD(val, REG_LDO_VRTC_VBIT_RC, PMU_LDO_VRTC_0_75V);
    val = SET_BITFIELD(val, REG_LDO_VRTC_VBIT_DSLEEP, PMU_LDO_VRTC_0_55V);
    pmu_write(PMU_REG_LDO_VRTC_VOLT, val);

    pmu_read(PMU_REG_LDO_VRTC_CFG, &val_vrtc_cfg);
    val_vrtc_cfg |= REG_LDO_VRTC_VBIT_LOAD;
    pmu_write(PMU_REG_LDO_VRTC_CFG, val_vrtc_cfg);

    pmu_read(PMU_REG_LPO_VRTC_CFG_D2, &val);
    val |= REG_VRTC_OVERRIDE;
    pmu_write(PMU_REG_LPO_VRTC_CFG_D2, val);
    hal_sys_timer_delay_us(PMU_LDO_VRTC_STABLE_TIME_US);

    pmu_read(PMU_REG_LDO_VRTC_VOLT, &val);
    val = SET_BITFIELD(val, REG_LDO_VRTC_VBIT_NORMAL, PMU_LDO_VRTC_0_65V);
    val = SET_BITFIELD(val, REG_LDO_VRTC_VBIT_RC, PMU_LDO_VRTC_0_65V);
    val = SET_BITFIELD(val, REG_LDO_VRTC_VBIT_DSLEEP, PMU_LDO_VRTC_0_55V);
    pmu_write(PMU_REG_LDO_VRTC_VOLT, val);
    hal_sys_timer_delay_us(PMU_LDO_VRTC_STABLE_TIME_US);

    pmu_write(PMU_REG_LDO_VRTC_CFG, val_vrtc_cfg);
#endif

#ifdef DIG_DCDC_MODE
    mode = PMU_POWER_MODE_DIG_DCDC;
#elif defined(ANA_DCDC_MODE)
    mode = PMU_POWER_MODE_ANA_DCDC;
#else // LDO_MODE
    mode = PMU_POWER_MODE_LDO;
#endif

    pmu_mode_change(mode);

#ifdef SPLIT_CORE_M_CORE_L
    pmu_module_config(PMU_CORE_L,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
    hal_sys_timer_delay_us(PMU_LDO_PU_STABLE_TIME_US);

    pmu_read(PMU_REG_VCORE_BYPASS_CFG, &val);
    val = (val & ~REG_LDO_VCORE_L_BYPASS) | REG_LDO_VCORE_L_BYPASS_DR;
    pmu_write(PMU_REG_VCORE_BYPASS_CFG, val);
    hal_sys_timer_delay(MS_TO_TICKS(5));
#else
    pmu_module_config(PMU_CORE_L,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
#endif

#ifdef VUSB_ON
    pmu_module_config(PMU_USB,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
#else
    pmu_module_config(PMU_USB,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
#endif

#ifdef PMU_VSENSOR_ON
    pmu_module_config(PMU_SENSOR,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
#else
    pmu_module_config(PMU_SENSOR,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
#endif

    pmu_codec_mic_bias_set_volt(AUD_VMIC_MAP_VMIC1 | AUD_VMIC_MAP_VMIC2, 2200);

#ifdef PMU_VCORE_RC_MODE_FIXED
    pmu_vcore_rc_mode_init();
#endif

#ifdef PMU_FORCE_LP_MODE
    // Force LP mode
    pmu_module_force_lp_config();
    pmu_codec_mic_bias_lowpower_mode(AUD_VMIC_MAP_VMIC1 | AUD_VMIC_MAP_VMIC2, true);
    hal_sys_timer_delay(MS_TO_TICKS(1));

    // Switch to little bandgap
    pmu_read(PMU_REG_BIAS_CFG, &val);
    val = (val & ~BG_VBG_SEL_REG) | BG_VBG_SEL_DR;
    pmu_write(PMU_REG_BIAS_CFG, val);
    hal_sys_timer_delay(MS_TO_TICKS(1));

    if (big_bandgap_user_map == 0) {
        // Disable big bandgap
        val = (val & ~(PU_BIAS_LDO_REG | BG_CONSTANT_GM_BIAS_REG | BG_CORE_EN_REG | BG_VTOI_EN_REG | BG_NOTCH_EN_REG)) |
            PU_BIAS_LDO_DR | BG_CONSTANT_GM_BIAS_DR | BG_CORE_EN_DR | BG_VTOI_EN_DR | BG_NOTCH_EN_DR;
        pmu_write(PMU_REG_BIAS_CFG, val);
    }
#endif

#ifdef PMU_VIO_3V3_ENABLE
    pmu_vio_3v3(true);
#endif

#if defined(MCU_HIGH_PERFORMANCE_MODE)
#error "Not support MCU_HIGH_PERFORMANCE_MODE!"
    pmu_high_performance_mode_enable(true);
#endif

#ifdef ACIN_PATTERN_DISABLE
    pmu_charger_pattern_enable(false);
#else
    pmu_charger_pattern_enable(true);
#endif

#ifdef PMU_CLK_USE_EXT_CRYSTAL
    pmu_external_crystal_enable();
#endif

#ifdef RTC_CALENDAR
    pmu_rtc_calendar_init();
#endif

#ifdef PMU_NTC_MONITOR
    pmu_ntc_monitor_init();
#endif

#ifdef RC_CLK_ENABLE
    rc_clock_enable();
#endif

#ifdef CAPSENSOR_ENABLE
    // TODO:
    // pmu_rc_clk_enable(PMU_CLK_SEL_RC_USER_CAP);
#endif

#ifdef PMU_REG_CFG_DUMP
    pmu_reg_val_print();
#endif

#endif // PMU_INIT || (!FPGA && !PROGRAMMER)

    return 0;
}

void pmu_big_bandgap_enable(enum PMU_BIG_BANDGAP_USER_T user, int enable)
{
#ifdef PMU_FORCE_LP_MODE
    uint16_t val;
    bool update = false;

    if (enable) {
        if (big_bandgap_user_map == 0) {
            update = true;
        }
        big_bandgap_user_map |= user;
    } else {
        if (big_bandgap_user_map & user) {
            big_bandgap_user_map &= ~user;
            if (big_bandgap_user_map == 0) {
                update = true;
            }
        }
    }

    if (!update) {
        return;
    }

    pmu_read(PMU_REG_BIAS_CFG, &val);
    val |= BG_CONSTANT_GM_BIAS_DR | BG_CORE_EN_DR | BG_VTOI_EN_DR;
    if (enable) {
        val |= BG_CONSTANT_GM_BIAS_REG | BG_CORE_EN_REG | BG_VTOI_EN_REG;
    } else {
        val &= ~(BG_CONSTANT_GM_BIAS_REG | BG_CORE_EN_REG | BG_VTOI_EN_REG);
    }
    pmu_write(PMU_REG_BIAS_CFG, val);
    if (enable) {
        hal_sys_timer_delay_us(PMU_BIG_BG_STABLE_TIME_US);
    }
#endif
}

void pmu_sleep(void)
{
    if (!dcdc_ramp_en) {
        pmu_buck_volt_hw_ramp_enable(dcdc_ramp_map);
    }
}

void pmu_wakeup(void)
{
    if (!dcdc_ramp_en) {
        pmu_buck_volt_hw_ramp_disable(dcdc_ramp_map);
    }
}

void pmu_codec_mic_bias_set_volt(uint32_t map, uint32_t mv)
{
    uint16_t val;
    int i;
    enum PMU_REG_T bias_reg;
    uint8_t volt = 0;
    uint8_t res = 0;
    uint8_t step = 0;

    step = 40;

    if (mv <= 1600) {
        volt = PMU_VMIC_1_6V;
    } else {
        volt = PMU_VMIC_1_6V + (mv - 1600) / step;
        if (volt > PMU_VMIC_2_8V) {
            volt = PMU_VMIC_2_8V;
        }
    }

    if (vcodec_mv < 1800) {
        volt += 2;
    }

    res = PMU_VMIC_RES_2_8V;

    pmu_read(PMU_REG_MIC_BIAS_A, &val);
    val = (val & ~(REG_MIC_LDO_RES_MASK)) | REG_MIC_LDO_RES(res);
    pmu_write(PMU_REG_MIC_BIAS_A, val);

    for (i = 0; i < MAX_VMIC_CH_NUM; i++) {
        if ((map & (AUD_VMIC_MAP_VMIC1 << i)) == 0) {
            continue;
        }
        bias_reg = PMU_REG_MIC_BIAS_A + i;
        pmu_read(bias_reg, &val);
        val = (val & ~(REG_MIC_BIASA_VSEL_MASK)) | REG_MIC_BIASA_VSEL(volt);
        pmu_write(bias_reg, val);
    }
}

void pmu_codec_mic_bias_enable(uint32_t map, int enable)
{
    int i;
    enum PMU_REG_T bias_reg;
    uint16_t val;
    uint16_t val_temp = 0;
    static bool reg_mic_ldo_en = false;
    static int need_ldo_on_cnt = 0;

    for (i = 0; i < MAX_VMIC_CH_NUM; i++) {
        if ((map & (AUD_VMIC_MAP_VMIC1 << i)) == 0) {
            continue;
        }
        bias_reg = PMU_REG_MIC_BIAS_A + i;
        pmu_read(bias_reg, &val);
        if (enable){
            val |= REG_MIC_BIASA_EN;
            need_ldo_on_cnt++;
        } else {
            val &= ~REG_MIC_BIASA_EN;
            need_ldo_on_cnt--;
        }
        pmu_write(bias_reg, val);
    }

    if (!reg_mic_ldo_en && need_ldo_on_cnt) {
        pmu_read(PMU_REG_MIC_BIAS_B, &val);
        val |= REG_MIC_LDO_EN;
        pmu_write(PMU_REG_MIC_BIAS_B, val);
        reg_mic_ldo_en = true;
    } else if (need_ldo_on_cnt == 0) {
        pmu_read(PMU_REG_MIC_BIAS_B, &val);
        val &= ~REG_MIC_LDO_EN;
        pmu_write(PMU_REG_MIC_BIAS_B, val);
        reg_mic_ldo_en = false;
    }

    if (!enable) {
        pmu_read(PMU_REG_MIC_PULL_DOWN, &val);
        val_temp = val;
        if (map & AUD_VMIC_MAP_VMIC1) {
            val_temp |= REG_MIC_BIASA_PULLDOWN;
        }
        if (map & AUD_VMIC_MAP_VMIC2) {
            val_temp |= REG_MIC_BIASB_PULLDOWN;
        }
        pmu_write(PMU_REG_MIC_PULL_DOWN, val_temp);
        hal_sys_timer_delay_us(PMU_LDO_PULLDOWN_STABLE_TIME_US);
        // need clear pull_down
        pmu_write(PMU_REG_MIC_PULL_DOWN, val);
    }
}

void pmu_codec_mic_bias_lowpower_mode(uint32_t map, int enable)
{
    uint16_t val;

    if (map & (AUD_VMIC_MAP_VMIC1 | AUD_VMIC_MAP_VMIC2)) {
        pmu_read(PMU_REG_MIC_PULL_DOWN, &val);
        if (enable) {
            val |= REG_MIC_LP_ENABLE;
        } else {
            val &= ~REG_MIC_LP_ENABLE;
        }
        pmu_write(PMU_REG_MIC_PULL_DOWN, val);
    }
}

SRAM_TEXT_LOC void pmu_flash_write_config(void)
{
#ifdef FLASH_WRITE_AT_HIGH_VCORE
    uint32_t lock;

    if (pmu_vcore_req & PMU_VCORE_FLASH_WRITE_ENABLED) {
        return;
    }

    lock = int_lock();
    pmu_vcore_req |= PMU_VCORE_FLASH_WRITE_ENABLED;
    int_unlock(lock);

    pmu_dig_set_volt(PMU_POWER_MODE_NONE);
#endif
}

SRAM_TEXT_LOC void pmu_flash_read_config(void)
{
#ifdef FLASH_WRITE_AT_HIGH_VCORE
    uint32_t lock;

    if ((pmu_vcore_req & PMU_VCORE_FLASH_WRITE_ENABLED) == 0) {
        return;
    }

    lock = int_lock();
    pmu_vcore_req &= ~PMU_VCORE_FLASH_WRITE_ENABLED;
    int_unlock(lock);

    pmu_dig_set_volt(PMU_POWER_MODE_NONE);
#endif
}

void BOOT_TEXT_FLASH_LOC pmu_flash_freq_config(uint32_t freq)
{
#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
    uint32_t lock;

    lock = int_lock();
    if (freq > 52000000) {
        // The real max freq is 120M
        pmu_vcore_req |= PMU_VCORE_FLASH_FREQ_HIGH;
    } else {
        pmu_vcore_req &= ~PMU_VCORE_FLASH_FREQ_HIGH;
    }
    int_unlock(lock);

    if (pmu_power_mode == PMU_POWER_MODE_NONE) {
        // PMU not init yet
        pmu_dig_init_volt();
        return;
    }

    pmu_dig_set_volt(PMU_POWER_MODE_NONE);
#endif
}

void BOOT_TEXT_FLASH_LOC pmu_psram_freq_config(uint32_t freq)
{
#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
    uint32_t lock;

    lock = int_lock();
    pmu_vcore_req &= ~(PMU_VCORE_PSRAM_FREQ_HIGH | PMU_VCORE_PSRAM_FREQ_MEDIUM | PMU_VCORE_PSRAM_FREQ_MEDIUM_LOW);
    if (freq >= 200000000) {
        pmu_vcore_req |= PMU_VCORE_PSRAM_FREQ_HIGH;
    } else if (freq >= 160000000) {
        pmu_vcore_req |= PMU_VCORE_PSRAM_FREQ_MEDIUM;
    } else if (freq >= 100000000) {
        pmu_vcore_req |= PMU_VCORE_PSRAM_FREQ_MEDIUM_LOW;
    }
    int_unlock(lock);

    if (pmu_power_mode == PMU_POWER_MODE_NONE) {
        // PMU not init yet
        pmu_dig_init_volt();
    } else {
        pmu_dig_set_volt(PMU_POWER_MODE_NONE);
    }
#endif
}

void pmu_anc_config(int enable)
{
}

void pmu_fir_high_speed_config(int enable)
{
}

void pmu_fir_freq_config(uint32_t freq)
{
    uint32_t lock;

    lock = int_lock();
    if (freq >= 96000000) {
        pmu_vcore_req |= PMU_VCORE_FIR_FREQ_HIGH;
    } else {
        pmu_vcore_req &= ~PMU_VCORE_FIR_FREQ_HIGH;
    }
    int_unlock(lock);

    pmu_dig_set_volt(PMU_POWER_MODE_NONE);
}

void pmu_iir_freq_config(uint32_t freq)
{
}

void pmu_iir_eq_freq_config(uint32_t freq)
{
    uint32_t lock;

    lock = int_lock();
    if (freq >= 96000000) {
        pmu_vcore_req |= PMU_VCORE_IIR_EQ_FREQ_HIGH;
    } else {
        pmu_vcore_req &= ~PMU_VCORE_IIR_EQ_FREQ_HIGH;
    }
    int_unlock(lock);

    pmu_dig_set_volt(PMU_POWER_MODE_NONE);
}

void pmu_rs_freq_config(uint32_t freq)
{
    uint32_t lock;

    lock = int_lock();
    if (freq >= 60000000) {
        pmu_vcore_req |= PMU_VCORE_RS_FREQ_HIGH;
    } else {
        pmu_vcore_req &= ~PMU_VCORE_RS_FREQ_HIGH;
    }
    int_unlock(lock);

    pmu_dig_set_volt(PMU_POWER_MODE_NONE);
}

void pmu_rs_adc_freq_config(uint32_t freq)
{
    uint32_t lock;

    lock = int_lock();
    if (freq >= 96000000) {
        pmu_vcore_req |= PMU_VCORE_RS_ADC_FREQ_HIGH;
    } else {
        pmu_vcore_req &= ~PMU_VCORE_RS_ADC_FREQ_HIGH;
    }
    int_unlock(lock);

    pmu_dig_set_volt(PMU_POWER_MODE_NONE);
}


void BOOT_TEXT_SRAM_LOC pmu_sys_freq_config(enum HAL_CMU_FREQ_T freq)
{
#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
    uint32_t lock;
    enum PMU_VCORE_REQ_T old_req;
    bool update = false;

#ifdef ARM_CMSE
    ASSERT(!hal_cmu_ns_running(), "pmu_sys_freq_config: NS is running");
#endif

    lock = int_lock();
    old_req = pmu_vcore_req;
    pmu_vcore_req &= ~(PMU_VCORE_SYS_FREQ_HIGH | PMU_VCORE_SYS_FREQ_MEDIUM);
#if defined(MCU_HIGH_PERFORMANCE_MODE)
    if (freq > HAL_CMU_FREQ_104M) {
        if (high_perf_on) {
            // The real freq is 350M
            pmu_vcore_req |= PMU_VCORE_SYS_FREQ_HIGH;
        } else {
            pmu_vcore_req |= PMU_VCORE_SYS_FREQ_MEDIUM;
        }
    } else {
#ifndef OSC_26M_X4_AUD2BB
        if (freq == HAL_CMU_FREQ_104M) {
            // The real freq is 200M
            pmu_vcore_req |= PMU_VCORE_SYS_FREQ_MEDIUM;
        }
#endif
    }
#else
    if (freq > HAL_CMU_FREQ_104M) {
        pmu_vcore_req |= PMU_VCORE_SYS_FREQ_MEDIUM;
    }
#endif
    if (old_req != pmu_vcore_req) {
        update = true;
    }
    int_unlock(lock);

    if (!update) {
        // Nothing changes
        return;
    }

    if (pmu_power_mode == PMU_POWER_MODE_NONE) {
        // PMU not init yet
        pmu_dig_init_volt();
        return;
    }

    pmu_dig_set_volt(PMU_POWER_MODE_NONE);
#endif
}

void pmu_high_performance_mode_enable(bool enable)
{
#if defined(MCU_HIGH_PERFORMANCE_MODE)
    uint16_t val;

    if (high_perf_on == enable) {
        return;
    }
    high_perf_on = enable;

    if (!enable) {
        if (high_perf_freq_mhz > 300) {
            // Switch to 52M to avoid using PLL
            hal_cmu_sys_set_freq(HAL_CMU_FREQ_52M);
            // Restore the default div
            rf_read(RF_REG_1037, &val);
            val |= (1 << 9); // div=2 (bit9=1)
            rf_write(RF_REG_1037, val);
            // Restore the sys freq
            hal_cmu_sys_set_freq(hal_sysfreq_get_hw_freq());
        }
        // Restore the default PLL freq (384M)
        bbpll_freq_pll_config(192 * 1000000 * 2);
    }

    pmu_sys_freq_config(hal_sysfreq_get_hw_freq());

    if (enable) {
        uint32_t pll_freq;

        // Change freq first, and then change divider.
        // Otherwise there will be an instant very high freq sent to digital domain.

        if (high_perf_freq_mhz <= 300) {
            pll_freq = high_perf_freq_mhz * 1000000 * 2;
        } else {
            pll_freq = high_perf_freq_mhz * 1000000;
        }
        bbpll_freq_pll_config(pll_freq);

        if (high_perf_freq_mhz > 300) {
            // Switch to 52M to avoid using PLL
            hal_cmu_sys_set_freq(HAL_CMU_FREQ_52M);
            rf_read(RF_REG_1037, &val);
            val &= ~(1 << 9); // div=1 (bit9=0)
            rf_write(RF_REG_1037, val);
            // Restore the sys freq
            hal_cmu_sys_set_freq(hal_sysfreq_get_hw_freq());
        }
    }
#endif
}

void pmu_usb_config(enum PMU_USB_CONFIG_TYPE_T type)
{

}

#if !defined(FPGA) && !defined(PROGRAMMER)
struct PMU_CHG_CTX_T {
    uint16_t pmu_chg_status;
};

struct PMU_CHG_CTX_T BOOT_BSS_LOC pmu_chg_ctx;

void BOOT_TEXT_SRAM_LOC pmu_charger_save_context(void)
{
    pmu_read(PMU_REG_CHARGER_STATUS, &pmu_chg_ctx.pmu_chg_status);
}

enum PMU_POWER_ON_CAUSE_T pmu_charger_poweron_status(void)
{
    enum PMU_POWER_ON_CAUSE_T pmu_power_on_cause = PMU_POWER_ON_CAUSE_NONE;

    if (pmu_chg_ctx.pmu_chg_status & AC_ON_DET_OUT){
        pmu_power_on_cause = PMU_POWER_ON_CAUSE_CHARGER_ACOFF;
    }else if (pmu_chg_ctx.pmu_chg_status & AC_ON){
        pmu_power_on_cause = PMU_POWER_ON_CAUSE_CHARGER_ACON;
    }
    return pmu_power_on_cause;
}
#endif

void pmu_charger_init(void)
{
    unsigned short readval_cfg;
    uint32_t lock;

    lock = int_lock();
    pmu_read(PMU_REG_CHARGER_CFG, &readval_cfg);
    readval_cfg &= ~(REG_CHARGE_OUT_INTR_MSK | REG_CHARGE_IN_INTR_MSK |
        REG_AC_ON_OUT_EN | REG_AC_ON_IN_EN);
    pmu_write(PMU_REG_CHARGER_CFG ,readval_cfg);
    int_unlock(lock);

    hal_sys_timer_delay(MS_TO_TICKS(1));

    lock = int_lock();
    pmu_read(PMU_REG_CHARGER_CFG, &readval_cfg);
    readval_cfg |= REG_AC_ON_OUT_EN | REG_AC_ON_IN_EN;
    readval_cfg = SET_BITFIELD(readval_cfg, REG_AC_ON_DB_VALUE, 8);
    pmu_write(PMU_REG_CHARGER_CFG ,readval_cfg);
    int_unlock(lock);
}

static void pmu_charger_irq_handler(PMU_IRQ_HDLR_PARAM)
{
    enum PMU_CHARGER_STATUS_T status = PMU_CHARGER_UNKNOWN;
    unsigned short readval;

#ifdef PMU_IRQ_UNIFIED
    readval = irq_status;
#else
    uint32_t lock;

    lock = int_lock();
    pmu_read(PMU_REG_CHARGER_STATUS, &readval);
    pmu_write(PMU_REG_CHARGER_STATUS, readval);
    int_unlock(lock);
#endif
    PMU_DEBUG_TRACE(3,"%s REG_%02X=0x%04X", __func__, PMU_REG_CHARGER_STATUS, readval);

    if ((readval & (INTR_MSKED_CHARGE_IN | INTR_MSKED_CHARGE_OUT)) == 0){
        PMU_DEBUG_TRACE(1,"%s SKIP", __func__);
        return;
    } else if ((readval & (INTR_MSKED_CHARGE_IN | INTR_MSKED_CHARGE_OUT)) ==
            (INTR_MSKED_CHARGE_IN | INTR_MSKED_CHARGE_OUT)) {
        PMU_DEBUG_TRACE(1,"%s DITHERING", __func__);
        hal_sys_timer_delay(2);
    } else {
        PMU_DEBUG_TRACE(1,"%s NORMAL", __func__);
    }

    status = pmu_charger_get_status();

    if (charger_irq_handler) {
        charger_irq_handler(status);
    }
}

void pmu_charger_set_irq_handler(PMU_CHARGER_IRQ_HANDLER_T handler)
{
    uint32_t lock;
    uint16_t val;

    charger_irq_handler = handler;

    lock = int_lock();
    pmu_read(PMU_REG_CHARGER_CFG, &val);
    if (handler) {
        val |= REG_CHARGE_IN_INTR_MSK | REG_CHARGE_OUT_INTR_MSK;
    } else {
        val &= ~(REG_CHARGE_IN_INTR_MSK | REG_CHARGE_OUT_INTR_MSK);
    }
    pmu_write(PMU_REG_CHARGER_CFG, val);

#ifdef PMU_IRQ_UNIFIED
   pmu_set_irq_unified_handler(PMU_IRQ_TYPE_CHARGER, handler ? pmu_charger_irq_handler : NULL);
#else
    if (handler) {
        NVIC_SetVector(CHARGER_IRQn, (uint32_t)pmu_charger_irq_handler);
        NVIC_SetPriority(CHARGER_IRQn, IRQ_PRIORITY_NORMAL);
        NVIC_ClearPendingIRQ(CHARGER_IRQn);
        NVIC_EnableIRQ(CHARGER_IRQn);
    } else {
        NVIC_DisableIRQ(CHARGER_IRQn);
    }
#endif
    int_unlock(lock);
}

void pmu_charger_plugin_config(void)
{
}

void pmu_charger_plugout_config(void)
{
}

enum PMU_CHARGER_STATUS_T pmu_charger_get_status(void)
{
    unsigned short readval;
    enum PMU_CHARGER_STATUS_T status;

    pmu_read(PMU_REG_CHARGER_STATUS, &readval);
    if (readval & AC_ON)
        status = PMU_CHARGER_PLUGIN;
    else
        status = PMU_CHARGER_PLUGOUT;

    return status;
}

#ifdef PMU_IRQ_UNIFIED
static void pmu_general_irq_handler(void)
{
    uint32_t lock;
    uint16_t val;
    bool pwrkey, charger, gpadc, rtc, gpio, wdt;

    pwrkey = false;
    charger = false;
    gpadc = false;
    rtc = false;
    gpio = false;
    wdt = false;

    lock = int_lock();
    pmu_read(PMU_REG_CHARGER_STATUS, &val);
    if (val & (POWER_ON_RELEASE | POWER_ON_PRESS)) {
        pwrkey = true;
    }
    if (val & (INTR_MSKED_CHARGE_IN | INTR_MSKED_CHARGE_OUT)) {
        charger = true;
    }
    if (pwrkey || charger) {
        pmu_write(PMU_REG_CHARGER_STATUS, val);
    }
    int_unlock(lock);

    if (pwrkey) {
        if (pmu_irq_hdlrs[PMU_IRQ_TYPE_PWRKEY]) {
            pmu_irq_hdlrs[PMU_IRQ_TYPE_PWRKEY](val);
        }
    }
    if (charger) {
        if (pmu_irq_hdlrs[PMU_IRQ_TYPE_CHARGER]) {
            pmu_irq_hdlrs[PMU_IRQ_TYPE_CHARGER](val);
        }
    }

    lock = int_lock();
    pmu_read(PMU_REG_INT_MSKED_STATUS, &val);
    if (val & (KEY_ERR1_INTR_MSKED | KEY_ERR0_INTR_MSKED | KEY_PRESS_INTR_MSKED | KEY_RELEASE_INTR_MSKED |
            SAMPLE_DONE_INTR_MSKED | CHAN_DATA_INTR_MSKED_MASK)) {
        gpadc = true;
    }
    if (val & (RTC_INT1_MSKED | RTC_INT0_MSKED)) {
        rtc = true;
    }
    if (gpadc || rtc) {
        pmu_write(PMU_REG_INT_CLR, val);
    }
    int_unlock(lock);

    if (gpadc) {
        if (pmu_irq_hdlrs[PMU_IRQ_TYPE_GPADC]) {
            pmu_irq_hdlrs[PMU_IRQ_TYPE_GPADC](val);
        }
    }
    if (rtc) {
        if (pmu_irq_hdlrs[PMU_IRQ_TYPE_RTC]) {
            pmu_irq_hdlrs[PMU_IRQ_TYPE_RTC](val);
        }
    }

    lock = int_lock();
    pmu_read(PMU_REG_LED_IO_IN, &val);
    if (val & (PMU_GPIO_INTR_MSKED1 | PMU_GPIO_INTR_MSKED2)) {
        gpio = true;
    }
    if (val & WDT_INTR_MSKED) {
        wdt = true;
    }
    if (gpio) {
        pmu_write(PMU_REG_LED_IO_IN, val);
    }
    if (wdt) {
        // No need to clear
    }
    int_unlock(lock);

    if (gpio) {
        if (pmu_irq_hdlrs[PMU_IRQ_TYPE_GPIO]) {
            pmu_irq_hdlrs[PMU_IRQ_TYPE_GPIO](val);
        }
    }
    if (wdt) {
        if (pmu_irq_hdlrs[PMU_IRQ_TYPE_WDT]) {
            pmu_irq_hdlrs[PMU_IRQ_TYPE_WDT](val);
        }
    }
}

int pmu_set_irq_unified_handler(enum PMU_IRQ_TYPE_T type, PMU_IRQ_UNIFIED_HANDLER_T hdlr)
{
    bool update;
    uint32_t lock;
    int i;

    if (type >= PMU_IRQ_TYPE_QTY) {
        return 1;
    }

    enum PMU_REG_T reg;
    uint16_t val;
    uint16_t mask;

    if (type == PMU_IRQ_TYPE_GPADC) {
        reg = PMU_REG_USB_CFG2;
        mask = GPADC_INTR_MERGED_MSK;
    } else if (type == PMU_IRQ_TYPE_RTC) {
        reg = PMU_REG_USB_CFG2;
        mask = RTC_INTR_TMP_MERGED_MSK;
    } else if (type == PMU_IRQ_TYPE_CHARGER) {
        reg = PMU_REG_USB_CFG2;
        mask = CHARGE_INTR_MERGED_MSK;
    } else if (type == PMU_IRQ_TYPE_GPIO) {
        reg = PMU_REG_USB_CFG3;
        mask = PMU_GPIO_INTR_MSKED1_MERGED_MSK | PMU_GPIO_INTR_MSKED2_MERGED_MSK;
    } else if (type == PMU_IRQ_TYPE_WDT) {
        reg = PMU_REG_USB_CFG3;
        mask = WDT_INTR_MSKED_MERGED_MSK;
    } else if (type == PMU_IRQ_TYPE_PWRKEY) {
        reg = PMU_REG_USB_CFG3;
        mask = POWER_ON_INTR_MERGED_MSK;
    } else {
        return 2;
    }

    update = false;

    lock = int_lock();

    for (i = 0; i < PMU_IRQ_TYPE_QTY; i++) {
        if (pmu_irq_hdlrs[i]) {
            break;
        }
    }

    pmu_irq_hdlrs[type] = hdlr;

    pmu_read(reg, &val);
    if (hdlr) {
        val |= mask;
    } else {
        val &= ~mask;
    }
    pmu_write(reg, val);

    if (hdlr) {
        update = (i >= PMU_IRQ_TYPE_QTY);
    } else {
        if (i == type) {
            for (; i < PMU_IRQ_TYPE_QTY; i++) {
                if (pmu_irq_hdlrs[i]) {
                    break;
                }
            }
            update = (i >= PMU_IRQ_TYPE_QTY);
        }
    }

    if (update) {
        if (hdlr) {
            NVIC_SetVector(RTC_IRQn, (uint32_t)pmu_general_irq_handler);
            NVIC_SetPriority(RTC_IRQn, IRQ_PRIORITY_NORMAL);
            NVIC_ClearPendingIRQ(RTC_IRQn);
            NVIC_EnableIRQ(RTC_IRQn);
        } else {
            NVIC_DisableIRQ(RTC_IRQn);
        }
    }

    int_unlock(lock);

    return 0;
}
#endif

enum HAL_PWRKEY_IRQ_T pmu_pwrkey_irq_value_to_state(uint16_t irq_status)
{
    enum HAL_PWRKEY_IRQ_T state = HAL_PWRKEY_IRQ_NONE;

    if (irq_status & POWER_ON_PRESS) {
        state |= HAL_PWRKEY_IRQ_FALLING_EDGE;
    }

    if (irq_status & POWER_ON_RELEASE) {
        state |= HAL_PWRKEY_IRQ_RISING_EDGE;
    }

    return state;
}

int pmu_debug_config_ana(uint16_t volt)
{
    return 0;
}

int pmu_debug_config_codec(uint16_t volt)
{
    return 0;
}

int pmu_debug_config_vcrystal(bool on)
{
    return 0;
}

int pmu_debug_config_audio_output(bool diff)
{
    return 0;
}

void pmu_debug_reliability_test(int stage)
{
    uint16_t volt;

    if (stage == 0) {
        volt = PMU_DCDC_ANA_1_2V;
    } else {
        volt = PMU_DCDC_ANA_1_3V;
    }
    pmu_dcdc_ana_set_volt(volt, ana_lp_dcdc);
}

void pmu_led_set_hiz(enum HAL_GPIO_PIN_T pin)
{
    uint16_t val;
    uint32_t lock;

    if (pin == HAL_GPIO_PIN_LED1 || pin == HAL_GPIO_PIN_LED2) {
        lock = int_lock();
        if (pin == HAL_GPIO_PIN_LED1) {
            pmu_read(PMU_REG_LED_CFG_IO1, &val);
            val = (val | REG_LED_IO1_PU | REG_LED_IO1_OENB_PRE) & ~(REG_LED_IO1_PUEN | REG_LED_IO1_PDEN | REG_LED_IO1_RX_EN);
            pmu_write(PMU_REG_LED_CFG_IO1, val);
        } else {
            pmu_read(PMU_REG_LED_CFG_IO2, &val);
            val = (val | REG_LED_IO2_PU | REG_LED_IO2_OENB_PRE) & ~(REG_LED_IO2_PUEN | REG_LED_IO2_PDEN | REG_LED_IO2_RX_EN);
            pmu_write(PMU_REG_LED_CFG_IO2, val);
        }
        int_unlock(lock);
    }
}

void pmu_led_set_direction(enum HAL_GPIO_PIN_T pin, enum HAL_GPIO_DIR_T dir)
{
    uint16_t val;
    uint32_t lock;

    if (pin == HAL_GPIO_PIN_LED1 || pin == HAL_GPIO_PIN_LED2) {
        lock = int_lock();
        if (pin == HAL_GPIO_PIN_LED1) {
            pmu_read(PMU_REG_LED_CFG_IO1, &val);
            val |= REG_LED_IO1_PU;
            if (dir == HAL_GPIO_DIR_IN) {
                val |= REG_LED_IO1_OENB_PRE | REG_LED_IO1_RX_EN;
            } else {
                val &= ~(REG_LED_IO1_OENB_PRE | REG_LED_IO1_RX_EN);
            }
            pmu_write(PMU_REG_LED_CFG_IO1, val);
        } else {
            pmu_read(PMU_REG_LED_CFG_IO2, &val);
            val |= REG_LED_IO2_PU;
            if (dir == HAL_GPIO_DIR_IN) {
                val |= REG_LED_IO2_OENB_PRE | REG_LED_IO2_RX_EN;
            } else {
                val &= ~(REG_LED_IO2_OENB_PRE | REG_LED_IO2_RX_EN);
            }
            pmu_write(PMU_REG_LED_CFG_IO2, val);
        }
        int_unlock(lock);
    }
}

enum HAL_GPIO_DIR_T pmu_led_get_direction(enum HAL_GPIO_PIN_T pin)
{
    uint16_t val;

    if (pin == HAL_GPIO_PIN_LED1 || pin == HAL_GPIO_PIN_LED2) {
        if (pin == HAL_GPIO_PIN_LED1) {
            pmu_read(PMU_REG_LED_CFG_IO1, &val);
            return (val & REG_LED_IO1_OENB_PRE) ? HAL_GPIO_DIR_IN : HAL_GPIO_DIR_OUT;
        } else {
            pmu_read(PMU_REG_LED_CFG_IO2, &val);
            return (val & REG_LED_IO2_OENB_PRE) ? HAL_GPIO_DIR_IN : HAL_GPIO_DIR_OUT;
        }
    } else {
        return HAL_GPIO_DIR_IN;
    }
}

void pmu_led_set_voltage_domains(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_PIN_VOLTAGE_DOMAINS_T volt)
{
    enum PMU_LED_VOLT_T {
        PMU_LED_VOLT_VBAT,
        PMU_LED_VOLT_VMEM,
        PMU_LED_VOLT_VIO,
    };
    enum PMU_LED_VOLT_T sel;
    uint16_t val;
    uint32_t lock;

    if (pin == HAL_IOMUX_PIN_LED1 || pin == HAL_IOMUX_PIN_LED2) {
        if (volt == HAL_IOMUX_PIN_VOLTAGE_VIO) {
            sel = PMU_LED_VOLT_VIO;
        } else if (volt == HAL_IOMUX_PIN_VOLTAGE_MEM) {
            sel = PMU_LED_VOLT_VMEM;
        } else {
            sel = PMU_LED_VOLT_VBAT;
        }

        lock = int_lock();
        if (pin == HAL_IOMUX_PIN_LED1) {
            pmu_read(PMU_REG_LED_CFG_IO1, &val);
            val = SET_BITFIELD(val, REG_LED_IO1_SEL, sel);
            pmu_write(PMU_REG_LED_CFG_IO1, val);
        } else {
            pmu_read(PMU_REG_LED_CFG_IO2, &val);
            val = SET_BITFIELD(val, REG_LED_IO2_SEL, sel);
            pmu_write(PMU_REG_LED_CFG_IO2, val);
        }
        int_unlock(lock);
    }
}

void pmu_led_set_pull_select(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_PIN_PULL_SELECT_T pull_sel)
{
    uint16_t val;
    uint32_t lock;

    if (pin == HAL_IOMUX_PIN_LED1 || pin == HAL_IOMUX_PIN_LED2) {
        lock = int_lock();
        if (pin == HAL_IOMUX_PIN_LED1) {
            pmu_read(PMU_REG_LED_CFG_IO1, &val);
            val &= ~(REG_LED_IO1_PDEN | REG_LED_IO1_PUEN);
            if (pull_sel == HAL_IOMUX_PIN_PULLUP_ENABLE) {
                val |= REG_LED_IO1_PUEN;
            } else if (pull_sel == HAL_IOMUX_PIN_PULLDOWN_ENABLE) {
                val |= REG_LED_IO1_PDEN;
            }
            pmu_write(PMU_REG_LED_CFG_IO1, val);
        } else {
            pmu_read(PMU_REG_LED_CFG_IO2, &val);
            val &= ~(REG_LED_IO2_PDEN | REG_LED_IO2_PUEN);
            if (pull_sel == HAL_IOMUX_PIN_PULLUP_ENABLE) {
                val |= REG_LED_IO2_PUEN;
            } else if (pull_sel == HAL_IOMUX_PIN_PULLDOWN_ENABLE) {
                val |= REG_LED_IO2_PDEN;
            }
            pmu_write(PMU_REG_LED_CFG_IO2, val);
        }
        int_unlock(lock);
    }
}

void pmu_led_set_value(enum HAL_GPIO_PIN_T pin, int data)
{
    uint32_t lock;
    uint16_t val;

    if (pin == HAL_GPIO_PIN_LED1 || pin == HAL_GPIO_PIN_LED2) {
        lock = int_lock();
        if (pin == HAL_GPIO_PIN_LED1) {
#ifdef PMU_LED_VIA_PWM
            pmu_write(PMU_REG_PWM2_TOGGLE, 0xFFFF);
            pmu_read(PMU_REG_PWM2_BR_EN, &val);
            val &= ~REG_PWM2_BR_EN;
            val |= PWM_SELECT_EN;
            if (data) {
                val &= ~PWM_SELECT_INV;
            } else {
                val |= PWM_SELECT_INV;
            }
            pmu_write(PMU_REG_PWM2_BR_EN, val);
#else
            pmu_read(PMU_REG_PWM2_EN, &val);
            if (data) {
                val |= REG_LED0_OUT;
            } else {
                val &= ~REG_LED0_OUT;
            }
            pmu_write(PMU_REG_PWM2_EN, val);
#endif
        }else {
#ifdef PMU_LED_VIA_PWM
            pmu_write(PMU_REG_PWMB_TOGGLE, 0xFFFF);
            pmu_read(PMU_REG_PWMB_BR_EN, &val);
            val &= ~REG_PWMB_BR_EN;
            val |= PWMB_SELECT_EN;
            if (data) {
                val &= ~PWMB_SELECT_INV;
            } else {
                val |= PWMB_SELECT_INV;
            }
            pmu_write(PMU_REG_PWMB_BR_EN, val);
#else
            pmu_read(PMU_REG_PWMB_EN, &val);
            if (data) {
                val |= REG_LEDB_OUT;
            } else {
                val &= ~REG_LEDB_OUT;
            }
            pmu_write(PMU_REG_PWMB_EN, val);
#endif
        }
        int_unlock(lock);
    }
}

int pmu_led_get_value(enum HAL_GPIO_PIN_T pin)
{
    uint16_t val;
    int data = 0;

    if (pin == HAL_GPIO_PIN_LED1 || pin == HAL_GPIO_PIN_LED2) {
        pmu_read(PMU_REG_LED_IO_IN, &val);
        if (pin == HAL_GPIO_PIN_LED1) {
            data = LED_IO1_IN_DB;
        } else {
            data = LED_IO2_IN_DB;
        }
        data &= val;
    }

    return !!data;
}

void pmu_led_uart_enable(enum HAL_IOMUX_PIN_T pin)
{
    uint16_t val;
    if (pin == HAL_IOMUX_PIN_LED1) {
        pmu_read(PMU_REG_UART1_CFG, &val);
        val &= ~(REG_UART_LEDA_SEL | REG_PMU_UART_DR1);
        val |=  REG_GPIO_I_SEL;
        pmu_write(PMU_REG_UART1_CFG, val);

        pmu_read(PMU_REG_UART2_CFG, &val);
        val |= REG_PMU_UART_DR2;
        pmu_write(PMU_REG_UART2_CFG, val);

        pmu_read(PMU_REG_LED_CFG_IO1, &val);
        val |= REG_LED_IO1_PUEN;
        pmu_write(PMU_REG_LED_CFG_IO1, val);
    } else {
        pmu_read(PMU_REG_UART1_CFG, &val);
        val &= ~ REG_GPIO_I_SEL;
        val |= REG_PMU_UART_DR1;
        pmu_write(PMU_REG_UART1_CFG, val);

        pmu_read(PMU_REG_UART2_CFG, &val);
        val &= ~(REG_UART_LEDB_SEL | REG_PMU_UART_DR2);
        pmu_write(PMU_REG_UART2_CFG, val);

        pmu_read(PMU_REG_LED_CFG_IO2, &val);
        val |= REG_LED_IO2_PUEN;
        pmu_write(PMU_REG_LED_CFG_IO2, val);
    }
}

void pmu_led_uart_disable(enum HAL_IOMUX_PIN_T pin)
{
    uint16_t val;
    if (pin == HAL_IOMUX_PIN_LED1) {
        pmu_read(PMU_REG_UART1_CFG, &val);
        val |= (REG_UART_LEDA_SEL | REG_PMU_UART_DR1);
        val &=  ~REG_GPIO_I_SEL;
        pmu_write(PMU_REG_UART1_CFG, val);

        pmu_read(PMU_REG_UART2_CFG, &val);
        val &= ~REG_PMU_UART_DR2;
        pmu_write(PMU_REG_UART2_CFG, val);

        pmu_read(PMU_REG_LED_CFG_IO1, &val);
        val &= ~REG_LED_IO1_PUEN;
        pmu_write(PMU_REG_LED_CFG_IO1, val);
    } else {
        pmu_read(PMU_REG_UART1_CFG, &val);
        val |= REG_GPIO_I_SEL;
        val &= ~REG_PMU_UART_DR1;
        pmu_write(PMU_REG_UART1_CFG, val);

        pmu_read(PMU_REG_UART2_CFG, &val);
        val |= (REG_UART_LEDB_SEL | REG_PMU_UART_DR2);
        pmu_write(PMU_REG_UART2_CFG, val);

        pmu_read(PMU_REG_LED_CFG_IO2, &val);
        val &= ~REG_LED_IO2_PUEN;
        pmu_write(PMU_REG_LED_CFG_IO2, val);
    }
}

void pmu_led_breathing_enable(enum HAL_IOMUX_PIN_T pin, const struct PMU_LED_BR_CFG_T *cfg)
{
    uint32_t st1;
    uint32_t st2;
    uint32_t subcnt_data;
    uint8_t tg;
    uint16_t val;
    uint32_t lock;
    if (pin == HAL_IOMUX_PIN_LED1 || pin == HAL_IOMUX_PIN_LED2) {
        st1 = MS_TO_TICKS(cfg->off_time_ms);
        if (st1 > 0xFFFF) {
            st1 = 0xFFFF;
        }
        st2 = MS_TO_TICKS(cfg->on_time_ms);
        if (st2 > 0xFFFF) {
            st2 = 0xFFFF;
        }
        subcnt_data = MS_TO_TICKS(cfg->fade_time_ms);
        subcnt_data = integer_sqrt_nearest(subcnt_data);
        if (subcnt_data > (SUBCNT_DATA2_MASK >> SUBCNT_DATA2_SHIFT)) {
            subcnt_data = (SUBCNT_DATA2_MASK >> SUBCNT_DATA2_SHIFT);
        }
        // TODO: Keep compatible with digital PWM module (can be removed after 2500)
        if (subcnt_data > 0xFE) {
            subcnt_data = 0xFE;
        }
        tg = 1;

        pmu_led_set_direction((enum HAL_GPIO_PIN_T)pin, HAL_GPIO_DIR_OUT);
        pmu_led_set_pull_select(pin, HAL_IOMUX_PIN_NOPULL);
        pmu_led_set_voltage_domains(pin, HAL_IOMUX_PIN_VOLTAGE_VBAT);

        lock = int_lock();
        if (pin == HAL_IOMUX_PIN_LED1) {
            pmu_write(PMU_REG_PWM2_TOGGLE, st2);
            pmu_write(PMU_REG_PWM2_ST1, st1);
            val = SUBCNT_DATA2(subcnt_data) | TG_SUBCNT_D2_ST(tg);
            pmu_write(PMU_REG_PWM2_EN, val);
            pmu_read(PMU_REG_PWM2_BR_EN, &val);
            val = (val & ~REG_CLK_PWM_DIV_MASK) | REG_CLK_PWM_DIV(0) |
                REG_PWM_CLK_EN | REG_PWM2_BR_EN | PWM_SELECT_EN;
            pmu_write(PMU_REG_PWM2_BR_EN, val);
        } else {
            pmu_write(PMU_REG_PWMB_TOGGLE, st2);
            pmu_write(PMU_REG_PWMB_ST1, st1);
            val = SUBCNT_DATAB(subcnt_data) | TG_SUBCNT_DB_ST(tg);
            pmu_write(PMU_REG_PWMB_EN, val);
            pmu_read(PMU_REG_PWMB_BR_EN, &val);
            val = (val & ~REG_CLK_PWMB_DIV_MASK) | REG_CLK_PWMB_DIV(0) |
                REG_PWMB_CLK_EN | REG_PWMB_BR_EN | PWMB_SELECT_EN;
            pmu_write(PMU_REG_PWMB_BR_EN, val);
        }
        int_unlock(lock);
    }
}

void pmu_led_breathing_disable(enum HAL_IOMUX_PIN_T pin)
{
    uint16_t val;
    uint32_t lock;
    if (pin == HAL_IOMUX_PIN_LED1 || pin == HAL_IOMUX_PIN_LED2) {
        lock = int_lock();
        if (pin == HAL_IOMUX_PIN_LED1) {
            pmu_read(PMU_REG_PWM2_BR_EN, &val);
            val &= ~(REG_PWM_CLK_EN | REG_PWM2_BR_EN | PWM_SELECT_EN);
            pmu_write(PMU_REG_PWM2_BR_EN, val);
        } else {
            pmu_read(PMU_REG_PWMB_BR_EN, &val);
            val &= ~(REG_PWMB_CLK_EN | REG_PWMB_BR_EN | PWMB_SELECT_EN);
            pmu_write(PMU_REG_PWMB_BR_EN, val);
        }
        int_unlock(lock);
        pmu_led_set_direction((enum HAL_GPIO_PIN_T)pin, HAL_GPIO_DIR_IN);
        pmu_led_set_pull_select(pin, HAL_IOMUX_PIN_PULLUP_ENABLE);
    }
}

#ifdef PMU_IRQ_UNIFIED
static void pmu_gpio_irq_handler(uint16_t irq_status)
{
    if (irq_status & PMU_GPIO_INTR_MSKED1) {
        if (gpio_irq_handler[0]) {
            gpio_irq_handler[0](HAL_GPIO_PIN_LED1);
        }
    }
    if (irq_status & PMU_GPIO_INTR_MSKED2) {
        if (gpio_irq_handler[1]) {
            gpio_irq_handler[1](HAL_GPIO_PIN_LED2);
        }
    }
}

uint8_t pmu_gpio_setup_irq(enum HAL_GPIO_PIN_T pin, const struct HAL_GPIO_IRQ_CFG_T *cfg)
{
    uint32_t lock;
    uint16_t val;
    bool old_en;

    if (pin != HAL_GPIO_PIN_LED1 && pin != HAL_GPIO_PIN_LED2) {
        return 1;
    }

    lock = int_lock();

    old_en = (gpio_irq_en[0] || gpio_irq_en[1]);

    if (pin == HAL_GPIO_PIN_LED1) {
        gpio_irq_en[0] = cfg->irq_enable;
        gpio_irq_handler[0] = cfg->irq_handler;

        if (cfg->irq_enable) {
        }

        pmu_read(PMU_REG_UART1_CFG, &val);
        if (cfg->irq_enable) {
            val |= REG_PMU_GPIO_INTR_MSK1 | REG_PMU_GPIO_INTR_EN1;
            if (cfg->irq_debounce) {
                val &= ~PMU_DB_BYPASS1;
            } else {
                val |= PMU_DB_BYPASS1;
            }
        } else {
            val &= ~(REG_PMU_GPIO_INTR_MSK1 | REG_PMU_GPIO_INTR_EN1);
        }
        pmu_write(PMU_REG_UART1_CFG, val);
    } else {
        gpio_irq_en[1] = cfg->irq_enable;
        gpio_irq_handler[1] = cfg->irq_handler;

        pmu_read(PMU_REG_UART2_CFG, &val);
        if (cfg->irq_enable) {
            val |= REG_PMU_GPIO_INTR_MSK2 | REG_PMU_GPIO_INTR_EN2;
            if (cfg->irq_debounce) {
                val &= ~PMU_DB_BYPASS2;
            } else {
                val |= PMU_DB_BYPASS2;
            }
        } else {
            val &= ~(REG_PMU_GPIO_INTR_MSK1 | REG_PMU_GPIO_INTR_EN1);
        }
        pmu_write(PMU_REG_UART2_CFG, val);
    }

    if (cfg->irq_enable) {
        uint16_t type;
        uint16_t pol;

        type = (pin == HAL_GPIO_PIN_LED1) ? REG_EDGE_INTR_SEL1 : REG_EDGE_INTR_SEL2;
        pol = (pin == HAL_GPIO_PIN_LED1) ? REG_POS_INTR_SEL1 : REG_POS_INTR_SEL2;
        pmu_read(PMU_REG_MIC_BIAS_C, &val);
        if (cfg->irq_type == HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE) {
            val |= type;
            if (cfg->irq_polarity == HAL_GPIO_IRQ_POLARITY_LOW_FALLING) {
                val &= ~pol;
            } else {
                val |= pol;
            }
        } else {
            val &= ~type;
        }
        pmu_write(PMU_REG_MIC_BIAS_C, val);

        if (cfg->irq_type != HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE) {
            if (pin == HAL_GPIO_PIN_LED1) {
                pmu_read(PMU_REG_WDT_INT_CFG, &val);
                if (cfg->irq_polarity == HAL_GPIO_IRQ_POLARITY_LOW_FALLING) {
                    val |= REG_LOW_LEVEL_INTR_SEL1;
                } else {
                    val &= ~REG_LOW_LEVEL_INTR_SEL1;
                }
                pmu_write(PMU_REG_WDT_INT_CFG, val);
            } else {
                pmu_read(PMU_REG_UART2_CFG, &val);
                if (cfg->irq_polarity == HAL_GPIO_IRQ_POLARITY_LOW_FALLING) {
                    val |= REG_LOW_LEVEL_INTR_SEL2;
                } else {
                    val &= ~REG_LOW_LEVEL_INTR_SEL2;
                }
                pmu_write(PMU_REG_UART2_CFG, val);
            }
        }
    }

    if (old_en != cfg->irq_enable) {
        pmu_set_irq_unified_handler(PMU_IRQ_TYPE_GPIO, cfg->irq_enable ? pmu_gpio_irq_handler : NULL);
    }

    int_unlock(lock);

    return 0;
}

static void pmu_wdt_irq_handler(uint16_t irq_status)
{
    if (wdt_irq_handler) {
        wdt_irq_handler();
    }
}

void pmu_wdt_set_irq_handler(PMU_WDT_IRQ_HANDLER_T handler)
{
    uint32_t lock;
    uint16_t val;

    lock = int_lock();

    val = REG_WDT_INTR_CLR;
    pmu_write(PMU_REG_LED_IO_IN, val);

    wdt_irq_handler = handler;

    pmu_read(PMU_REG_WDT_INT_CFG, &val);
    if (handler) {
        val |= REG_WDT_INTR_MSK;
    } else {
        val &= ~REG_WDT_INTR_MSK;
    }
    pmu_write(PMU_REG_WDT_INT_CFG, val);

    pmu_set_irq_unified_handler(PMU_IRQ_TYPE_WDT, handler ? pmu_wdt_irq_handler : NULL);

    int_unlock(lock);
}
#else
uint8_t pmu_gpio_setup_irq(enum HAL_GPIO_PIN_T pin, const struct HAL_GPIO_IRQ_CFG_T *cfg)
{
    ASSERT(false, "PMU_IRQ_UNIFIED must defined to use PMU GPIO IRQ");
    return 1;
}

void pmu_wdt_set_irq_handler(PMU_WDT_IRQ_HANDLER_T handler)
{
}
#endif

#ifdef __WATCHER_DOG_RESET__
struct PMU_WDT_CTX_T {
    bool enabled;
    uint16_t wdt_irq_timer;
    uint16_t wdt_reset_timer;
    uint16_t wdt_cfg;
};

static struct PMU_WDT_CTX_T BOOT_BSS_LOC wdt_ctx;

void BOOT_TEXT_SRAM_LOC pmu_wdt_save_context(void)
{
    uint16_t wdt_cfg = 0, wdt_timer = 0;
    pmu_read(PMU_REG_WDT_CFG, &wdt_cfg);
    if (wdt_cfg & (REG_WDT_RESET_EN | REG_WDT_EN)){
        wdt_ctx.enabled = true;
        wdt_ctx.wdt_cfg = wdt_cfg;
        pmu_read(PMU_REG_WDT_IRQ_TIMER, &wdt_timer);
        wdt_ctx.wdt_irq_timer = wdt_timer;
        pmu_read(PMU_REG_WDT_RESET_TIMER, &wdt_timer);
        wdt_ctx.wdt_reset_timer = wdt_timer;
    }
}

void BOOT_TEXT_SRAM_LOC pmu_wdt_restore_context(void)
{
    if (wdt_ctx.enabled) {
        pmu_write(PMU_REG_WDT_IRQ_TIMER, wdt_ctx.wdt_irq_timer);
        pmu_write(PMU_REG_WDT_RESET_TIMER, wdt_ctx.wdt_reset_timer);
        pmu_write(PMU_REG_WDT_CFG, wdt_ctx.wdt_cfg);
    }
}
#endif

int pmu_wdt_config(uint32_t irq_ms, uint32_t reset_ms)
{
    if (irq_ms > 0xFFFF) {
        return 1;
    }
    if (reset_ms > 0xFFFF) {
        return 1;
    }
    wdt_irq_timer = irq_ms;
    wdt_reset_timer = reset_ms;

    pmu_write(PMU_REG_WDT_IRQ_TIMER, wdt_irq_timer);
    pmu_write(PMU_REG_WDT_RESET_TIMER, wdt_reset_timer);

    return 0;
}

void pmu_wdt_start(void)
{
    uint16_t val;

    if (wdt_irq_timer == 0 && wdt_reset_timer == 0) {
        return;
    }

    pmu_read(PMU_REG_WDT_CFG, &val);
    val |= (REG_WDT_RESET_EN | REG_WDT_EN);
    pmu_write(PMU_REG_WDT_CFG, val);

    pmu_read(PMU_REG_WDT_INT_CFG, &val);
    val |= REG_WDT_INTR_EN;
    pmu_write(PMU_REG_WDT_INT_CFG, val);
}

#ifndef __WATCHER_DOG_RESET__
BOOT_TEXT_SRAM_LOC
#endif
void pmu_wdt_stop(void)
{
    uint16_t val;

    pmu_read(PMU_REG_WDT_INT_CFG, &val);
    val &= ~REG_WDT_INTR_EN;
    pmu_write(PMU_REG_WDT_INT_CFG, val);

    pmu_read(PMU_REG_WDT_CFG, &val);
    val &= ~(REG_WDT_RESET_EN | REG_WDT_EN);
    pmu_write(PMU_REG_WDT_CFG, val);

    val = REG_WDT_INTR_CLR;
    pmu_write(PMU_REG_LED_IO_IN, val);
}

void pmu_wdt_feed(void)
{
    if (wdt_irq_timer == 0 && wdt_reset_timer == 0) {
        return;
    }

    pmu_write(PMU_REG_WDT_IRQ_TIMER, wdt_irq_timer);
    pmu_write(PMU_REG_WDT_RESET_TIMER, wdt_reset_timer);
}

int pmu_wdt_reboot(void)
{
    uint32_t lock;
    uint16_t val;
    uint16_t val_wdt_cfg;
    const uint16_t wdt_time_ms = 4;

    lock = int_lock();

    // Disable wdt by defalut
    pmu_read(PMU_REG_WDT_CFG, &val_wdt_cfg);
    val_wdt_cfg &= ~(REG_WDT_RESET_EN | REG_WDT_EN);
    pmu_write(PMU_REG_WDT_CFG, val_wdt_cfg);

    // Clear intr by default
    val = REG_WDT_INTR_CLR;
    pmu_write(PMU_REG_LED_IO_IN, val);

    pmu_write(PMU_REG_WDT_IRQ_TIMER, wdt_time_ms / 2);
    pmu_write(PMU_REG_WDT_RESET_TIMER, wdt_time_ms / 2);

    pmu_read(PMU_REG_WDT_INT_CFG, &val);
    val |= REG_WDT_INTR_EN | REG_WDT_INTR_MSK;
    pmu_write(PMU_REG_WDT_INT_CFG, val);

    val_wdt_cfg |= (REG_WDT_RESET_EN | REG_WDT_EN);
    pmu_write(PMU_REG_WDT_CFG, val_wdt_cfg);
    hal_sys_timer_delay(MS_TO_TICKS(wdt_time_ms << 1));

    PMU_INFO_TRACE_IMM(0, "\nError: pmu wdt reboot failed!\n");

    int_unlock(lock);

    return 0;
}

int pmu_wdt_load_test(void)
{
    uint16_t val0, val1;

    pmu_read(PMU_REG_RAW_STATUS, &val0);

    pmu_write(PMU_REG_WDT_RESET_TIMER, 3000);

    pmu_read(PMU_REG_RAW_STATUS, &val1);

    DRIVERS_TRACE(0, "wdt_load: %u -> %u", !!(val0 & WDT_LOAD), !!(val1 & WDT_LOAD));

    if ((val0 & WDT_LOAD) == (val1 & WDT_LOAD)) {
        DRIVERS_TRACE(0, "BAD: wdt load not changed");
        return 1;
    } else {
        DRIVERS_TRACE(0, "GOOD: wdt load changed");
        return 0;
    }
}

void pmu_bt_reconn(bool en)
{
    uint16_t pmu_val = 0;

    pmu_read(0x38, &pmu_val);
    //DRIVERS_TRACE(1,"origin: PMU_38=0x%x", pmu_val);
    pmu_val &= 0xfff0;      //clear [3:0]

    if(en){
        pmu_val |= 0xf;
    }
    pmu_write(0x38, pmu_val);

    //pmu_read(0x38, &pmu_val);
    //DRIVERS_TRACE(1,"final: PMU_38=0x%x", pmu_val);
}

void pmu_bt_2v5pa(bool en)
{
}

void pmu_vio_3v3(bool en)
{
    uint32_t lock;

    lock = int_lock();
    if (en) {
        pmu_module_config(PMU_SENSOR, PMU_MANUAL_MODE, PMU_LDO_ON, PMU_LP_MODE_ON, PMU_DSLEEP_MODE_ON);
        pmu_module_ramp_volt(PMU_SENSOR, PMU_LDO_SENSOR_3_3V, PMU_LDO_SENSOR_3_3V);
    } else {
        pmu_module_config(PMU_SENSOR, PMU_MANUAL_MODE, PMU_LDO_OFF, PMU_LP_MODE_ON, PMU_DSLEEP_MODE_OFF);
    }
    int_unlock(lock);
}

void pmu_sar_adc_vref_enable(void)
{
    uint16_t val;

#ifdef PMU_FORCE_LP_MODE
    pmu_big_bandgap_enable(PMU_BIG_BANDGAP_USER_SAR_VREF_OUTPUT, true);
#endif

    // force enable
    pmu_read(PMU_REG_AVDD_EN, &val);
    val |= REG_PU_AVDD25_ANA;
    pmu_write(PMU_REG_AVDD_EN, val);

    pmu_read(PMU_REG_SAR_PU_CFG, &val);
    val |= REG_SAR_PU_VREF_REG | REG_SAR_PU_VREF_DR | REG_SAR_PU_REG | REG_SAR_PU_DR;
    pmu_write(PMU_REG_SAR_PU_CFG, val);

    // sar vref output
    pmu_sar_adc_vref_sw_pu(true);
    hal_sys_timer_delay_us(PMU_GPADC_VREF_STABLE_TIME_US);
}

void pmu_sar_adc_vref_disable(void)
{
    uint16_t val;

#ifdef PMU_FORCE_LP_MODE
    pmu_big_bandgap_enable(PMU_BIG_BANDGAP_USER_SAR_VREF_OUTPUT, false);
#endif

    pmu_sar_adc_vref_sw_pu(false);

    pmu_read(PMU_REG_SAR_PU_CFG, &val);
    val &= ~(REG_SAR_PU_VREF_REG | REG_SAR_PU_VREF_DR | REG_SAR_PU_REG | REG_SAR_PU_DR);
    pmu_write(PMU_REG_SAR_PU_CFG, val);

    pmu_read(PMU_REG_AVDD_EN, &val);
    val &= ~REG_PU_AVDD25_ANA;
    pmu_write(PMU_REG_AVDD_EN, val);
}

int pmu_sar_input_buf_enable(int enable)
{
    uint16_t val;

    pmu_read(PMU_REG_SAR_PU_CFG, &val);
    if (enable) {
        val |= REG_SAR_PU_VREF_IN;
    } else {
        val &= ~REG_SAR_PU_VREF_IN;
    }
    pmu_write(PMU_REG_SAR_PU_CFG, val);

    pmu_read(PMU_REG_SAR_CLK_CFG, &val);
    if (enable) {
        val |= REG_SAR_INPUT_BUF_EN;
    } else {
        val &= ~REG_SAR_INPUT_BUF_EN;
    }
    pmu_write(PMU_REG_SAR_CLK_CFG, val);

    return 0;
}

static int pmu_ntc_capture_enable(int enable)
{
    uint16_t val;

    if (enable) {
        pmu_read(PMU_REG_AVDD_EN, &val);
        val |= REG_PU_AVDD25_ANA;
        pmu_write(PMU_REG_AVDD_EN, val);

        pmu_read(PMU_REG_SAR_CLK_CFG, &val);
        val |= REG_SAR_INPUT_BUF_EN;
        pmu_write(PMU_REG_SAR_CLK_CFG, val);

        pmu_read(PMU_REG_SAR_PU_CFG, &val);
        val |= REG_SAR_PU_VREF_IN;
        pmu_write(PMU_REG_SAR_PU_CFG, val);

        pmu_read(PMU_REG_IPTAT_CORE_I_CFG, &val);
        val |= DIG_IPTAT_CORE_EN;
        pmu_write(PMU_REG_IPTAT_CORE_I_CFG, val);
    } else {
        pmu_read(PMU_REG_AVDD_EN, &val);
        val &= ~REG_PU_AVDD25_ANA;
        pmu_write(PMU_REG_AVDD_EN, val);

        pmu_read(PMU_REG_SAR_CLK_CFG, &val);
        val &= ~REG_SAR_INPUT_BUF_EN;
        pmu_write(PMU_REG_SAR_CLK_CFG, val);

        pmu_read(PMU_REG_SAR_PU_CFG, &val);
        val &= ~REG_SAR_PU_VREF_IN;
        pmu_write(PMU_REG_SAR_PU_CFG, val);

        pmu_read(PMU_REG_IPTAT_CORE_I_CFG, &val);
        val &= ~DIG_IPTAT_CORE_EN;
        pmu_write(PMU_REG_IPTAT_CORE_I_CFG, val);
    }

    return 0;
}

static int pmu_vsys_div_ctrl(int enable)
{
    uint32_t lock;
    uint16_t val;

    lock = int_lock();
    pmu_read(PMU_REG_PU_VSYS_DIV, &val);
    if (enable) {
        val |= REG_PU_VSYS_DIV;
    } else {
        val &= ~REG_PU_VSYS_DIV;
    }
    pmu_write(PMU_REG_PU_VSYS_DIV, val);
    int_unlock(lock);

    return 0;
}

static int pmu_vbat_div_ctrl(int enable)
{
    uint32_t lock;
    uint16_t val;

    lock = int_lock();
    pmu_read(PMU_REG_POWER_KEY_CFG, &val);
    if (enable) {
        val |= REG_PU_VBAT_DIV;
    } else {
        val &= ~REG_PU_VBAT_DIV;
    }
    pmu_write(PMU_REG_POWER_KEY_CFG, val);
    int_unlock(lock);

    return 0;
}

int pmu_gpadc_div_ctrl(enum HAL_GPADC_CHAN_T channel, int enable)
{
    int ret = 0;

    if (channel == HAL_GPADC_CHAN_0) {
        ret = pmu_ntc_capture_enable(enable);
    } else if (channel == HAL_GPADC_CHAN_BATTERY) {
        ret = pmu_vsys_div_ctrl(enable);
    } else if (channel == HAL_GPADC_CHAN_6) {
        ret = pmu_vbat_div_ctrl(enable);
    }

    return ret;
}

int pmu_volt2temperature(const uint16_t volt)
{
    const int Kvtherm_T = 385;
    static uint16_t temp_ref_raw2volt = 0;
    int temp_centigrade = 0;

    if (!temp_ref_raw2volt) {
        bool calibed = true;

        pmu_get_efuse(PMU_EFUSE_PAGE_13, &temp_ref_raw2volt);
        if (!temp_ref_raw2volt) {
            temp_ref_raw2volt = PMU_EFUSE_TEMPSENSOR_DEFAULT;
            calibed = false;
        }
        temp_ref_raw2volt = hal_gpadc_adc2volt(temp_ref_raw2volt, HAL_GPADC_CHAN_0);
        DRIVERS_TRACE(0, "%s calibed=%d temp_ref_raw2volt=%d", __func__, calibed, temp_ref_raw2volt);
    }

    temp_centigrade = (int)(((volt - temp_ref_raw2volt) * 100 + Kvtherm_T / 2) / Kvtherm_T + 25);

    return temp_centigrade;
}

#ifdef PMU_NTC_MONITOR
static void pmu_ntc_irq_handler(uint16_t raw, uint16_t volt)
{
    ntc_ctx.temperature = pmu_volt2temperature(volt);
    ntc_ctx.temperature_update_ms = TICKS_TO_MS(hal_sys_timer_get());

    ntc_irq_busy = false;
}

static int pmu_ntc_capture_start(void)
{
    int nRet = 0;
    uint32_t lock = 0;

    lock = int_lock();
    if (ntc_irq_busy) {
        nRet = -1;
        goto exit;
    }
    ntc_irq_busy = true;
    hal_gpadc_open(HAL_GPADC_CHAN_0, HAL_GPADC_ATP_ONESHOT, pmu_ntc_irq_handler);
exit:
    int_unlock(lock);

    return nRet;
}

#ifdef NTC_MONITOR_USE_OSTIMER
static void pmu_ntc_monitor_ostimer_handler(void const *param)
{
    pmu_ntc_capture_start();
    osTimerStart(ntc_monitor_ostimer_id, ntc_ctx.monitor_interval_ms);
}
#else
static void pmu_ntc_monitor_hwtimer_handler(void *param)
{
    pmu_ntc_capture_start();
    hwtimer_start(ntc_monitor_timer, MS_TO_TICKS(ntc_ctx.monitor_interval_ms));
}
#endif

static void pmu_setup_ntc_monitor_timer(void)
{
#ifdef NTC_MONITOR_USE_OSTIMER
    if (ntc_monitor_ostimer_id == NULL) {
        ntc_monitor_ostimer_id = osTimerCreate(osTimer(ntc_monitor_ostimer), osTimerPeriodic, NULL);
        ASSERT(ntc_monitor_ostimer_id, "%s fail!", __func__);
    }
#else
    if (ntc_monitor_timer == NULL) {
        ntc_monitor_timer = hwtimer_alloc(pmu_ntc_monitor_hwtimer_handler, 0);
        ASSERT(ntc_monitor_timer, "%s fail!", __func__);
    }
#endif
}

static void pmu_stop_ntc_monitor_timer(void)
{
#ifdef NTC_MONITOR_USE_OSTIMER
    osTimerStop(ntc_monitor_ostimer_id);
#else
    hwtimer_stop(ntc_monitor_timer);
#endif
}

static void pmu_start_ntc_monitor_timer(void)
{
#ifdef NTC_MONITOR_USE_OSTIMER
    osTimerStart(ntc_monitor_ostimer_id, ntc_ctx.monitor_interval_ms);
#else
    hwtimer_start(ntc_monitor_timer, MS_TO_TICKS(ntc_ctx.monitor_interval_ms));
#endif
}

static void pmu_ntc_monitor_init(void)
{
    uint32_t sum_val = 0;
    uint16_t volt;
    int i, valid_cnt, ret;

    valid_cnt = 0;

    hal_gpadc_open(HAL_GPADC_CHAN_0, HAL_GPADC_ATP_125US, NULL);

    for (i = 0; i < 5; i++) {
        ret = hal_gpadc_get_volt_blocked(HAL_GPADC_CHAN_0, &volt);
        if (ret) {
            sum_val += volt;
            valid_cnt++;
        }
    }

    hal_gpadc_close(HAL_GPADC_CHAN_0);

    ASSERT(valid_cnt, "%s: fail! Invalid gpadc read!", __func__);

    volt = sum_val / valid_cnt;

    ntc_ctx.temperature = pmu_volt2temperature(volt);
    ntc_ctx.temperature_update_ms = TICKS_TO_MS(hal_sys_timer_get());
    ntc_ctx.monitor_interval_ms = 0;

    pmu_setup_ntc_monitor_timer();
}

int pmu_ntc_monitor_register(enum PMU_NTC_USER_T user, struct PMU_NTC_MONITOR_CTX_T *ctx)
{
    uint32_t lock;
    uint32_t min_interval_ms = UINT32_MAX;

    if (user >= PMU_NTC_USER_QTY) {
        return -1;
    }

    if (ctx->enable && ctx->interval_ms < 5) {
        return -2;
    }

    lock = int_lock();

    ntc_monitor_ctx[user].interval_ms = ctx->interval_ms;
    ntc_monitor_ctx[user].enable = ctx->enable;

    for (int i = 0; i < PMU_NTC_USER_QTY; i++) {
        if (ntc_monitor_ctx[i].enable && ntc_monitor_ctx[i].interval_ms > 0) {
            if (ntc_monitor_ctx[i].interval_ms < min_interval_ms) {
                min_interval_ms = ntc_monitor_ctx[i].interval_ms;
            }
        }
    }

    if (min_interval_ms == UINT32_MAX) {
        ntc_ctx.monitor_interval_ms = 0;
        hal_gpadc_close(HAL_GPADC_CHAN_0);
        pmu_stop_ntc_monitor_timer();
        ntc_irq_busy = false;
    } else {
        ntc_ctx.monitor_interval_ms = min_interval_ms;
    }

    int_unlock(lock);

    // Update ntc_ctx once if ntc not in work
    if (!ntc_irq_busy && ntc_ctx.monitor_interval_ms > 0) {
        pmu_start_ntc_monitor_timer();
    }

    return 0;
}

int pmu_ntc_monitor_deregister(enum PMU_NTC_USER_T user)
{
    struct PMU_NTC_MONITOR_CTX_T ctx = {0, false};

    return pmu_ntc_monitor_register(user, &ctx);
}

int pmu_ntc_ctx_get(struct PMU_NTC_CTX_T *ctx)
{
    int ret = 0;

    ctx->temperature = ntc_ctx.temperature;
    ctx->temperature_update_ms = ntc_ctx.temperature_update_ms;
    ctx->monitor_interval_ms = ntc_ctx.monitor_interval_ms;

    // ntc_monitor_timer has been disabled
    // maybe ntc_ctx is invalid
    if (ntc_ctx.monitor_interval_ms == 0) {
        ret = -1;
    }

    return ret;
}
#endif // PMU_NTC_MONITOR

void pmu_power_key_hw_reset_enable(uint8_t seconds)
{
    uint16_t val;
    uint32_t lock;
    const int sample_clk_freq_hz = 4;

    seconds *= sample_clk_freq_hz;
    if (seconds > 60) {
        seconds = 60;
    }

#ifdef SIMU
    pmu_write(PMU_REG_DIV_HW_RESET_CFG, 32 / sample_clk_freq_hz - 2);
#else
    pmu_write(PMU_REG_DIV_HW_RESET_CFG, CONFIG_SYSTICK_HZ / sample_clk_freq_hz - 2);
#endif

    lock = int_lock();
    pmu_read(PMU_REG_WDT_CFG, &val);
    val = SET_BITFIELD(val, REG_HW_RESET_TIME, seconds) | REG_HW_RESET_EN;
    pmu_write(PMU_REG_WDT_CFG, val);
    int_unlock(lock);
}

void pmu_power_key_hw_reset_disable(void)
{
    uint16_t val;

    pmu_read(PMU_REG_WDT_CFG, &val);
    val &= ~REG_HW_RESET_EN;
    pmu_write(PMU_REG_WDT_CFG, val);
}

#if defined(PMU_CLK_USE_EXT_CRYSTAL) || defined(RTC_CLK_USE_EXT_CRYSTAL)
static void BOOT_TEXT_FLASH_LOC pmu_external_crystal_init(void)
{
    uint16_t val;

    pmu_read(PMU_REG_XO_32K_CFG, &val);
    if (val & REG_XO_32K_PU_LDO) {
        return;
    }

    pmu_external_crystal_capbit_set(PMU_EXT_32K_CAPBIT_DEFAULT);

    // power on external crystal.
    pmu_read(PMU_REG_XO_32K_CFG, &val);
    val = SET_BITFIELD(val, REG_XO_32K_LDO_PRECHARGE_VALUE, 0x1);
    val = SET_BITFIELD(val, REG_XO_32K_ISEL_VALUE, 0x10);
    pmu_write(PMU_REG_XO_32K_CFG, val);

    pmu_read(PMU_REG_XO_32K_ISEL_RAMP, &val);
    val = SET_BITFIELD(val, REG_XO_32K_ISEL_RAMP_START_DLY, 0x100);
    pmu_write(PMU_REG_XO_32K_ISEL_RAMP, val);

    pmu_read(PMU_REG_XO_32K_CFG, &val);
    val |= REG_XO_32K_PU_LDO | REG_XO_32K_PU_LDO_DR | REG_XO_32K_ISEL_DR;
    pmu_write(PMU_REG_XO_32K_CFG, val);

    pmu_ext_crystal_init_stime = hal_sys_timer_get();
}

static void pmu_external_crystal_clkout_after_pwroff(void)
{
    if (pmu_ext_crystal_clkout_usr_map & PMU_EXT_CRYSTAL_CLKOUT_PIN_0) {
        pmu_external_crystal_clkout_enable(PMU_EXT_CRYSTAL_CLKOUT_PIN_0);
    }
    if (pmu_ext_crystal_clkout_usr_map & PMU_EXT_CRYSTAL_CLKOUT_PIN_1) {
        pmu_external_crystal_clkout_enable(PMU_EXT_CRYSTAL_CLKOUT_PIN_1);
    }
    if (pmu_ext_crystal_clkout_usr_map & PMU_EXT_CRYSTAL_CLKOUT_PIN_2) {
        pmu_external_crystal_clkout_enable(PMU_EXT_CRYSTAL_CLKOUT_PIN_2);
    }
}

void pmu_external_crystal_enable(void)
{
    // External crystal can not be cleared by reg reset and will resume xo32k config after hw reset.
    while (hal_sys_timer_get() - pmu_ext_crystal_init_stime < MS_TO_TICKS(PMU_EXT_32K_STABLE_TIME));

#ifdef PMU_CLK_USE_EXT_CRYSTAL
    uint16_t val;

    // PMU clock slecet to external crystal.
    pmu_read(PMU_REG_CLK_SEL_CFG, &val);
    val = SET_BITFIELD(val, CLK_32K_SEL, 0x2);
    pmu_write(PMU_REG_CLK_SEL_CFG, val);
    hal_sys_timer_delay(MS_TO_TICKS(PMU_CLK_SWITCH_STABLE_TIME));
#endif
}

void pmu_external_crystal_clkout_enable(enum PMU_EXT_CRYSTAL_CLKOUT_PIN_T pin)
{
    uint16_t val;

    if (pin == PMU_EXT_CRYSTAL_CLKOUT_PIN_0) {
        pmu_read(PMU_REG_PU_VSYS_DIV, &val);
        val |= REG_CLK_32K_EXT1_EN;
        pmu_write(PMU_REG_PU_VSYS_DIV, val);
    } else if (pin == PMU_EXT_CRYSTAL_CLKOUT_PIN_1) {
        pmu_read(PMU_REG_PU_VSYS_DIV, &val);
        val |= REG_CLK_32K_EXT2_EN;
        pmu_write(PMU_REG_PU_VSYS_DIV, val);
    } else {
        pmu_read(PMU_REG_PU_VSYS_DIV, &val);
        val |= REG_CLK_32K_EXT3_EN;
        pmu_write(PMU_REG_PU_VSYS_DIV, val);
    }
    pmu_ext_crystal_clkout_usr_map |= pin;
    hal_sys_timer_delay(MS_TO_TICKS(PMU_EXT_32K_CLK_OUT_STABLE_TIME));
}

void pmu_external_crystal_clkout_disable(enum PMU_EXT_CRYSTAL_CLKOUT_PIN_T pin)
{
    uint16_t val;

    if (pin == PMU_EXT_CRYSTAL_CLKOUT_PIN_0) {
        pmu_read(PMU_REG_PU_VSYS_DIV, &val);
        val &= ~REG_CLK_32K_EXT1_EN;
        pmu_write(PMU_REG_PU_VSYS_DIV, val);
    } else if (pin == PMU_EXT_CRYSTAL_CLKOUT_PIN_1) {
        pmu_read(PMU_REG_PU_VSYS_DIV, &val);
        val &= ~REG_CLK_32K_EXT2_EN;
        pmu_write(PMU_REG_PU_VSYS_DIV, val);
    } else {
        pmu_read(PMU_REG_PU_VSYS_DIV, &val);
        val &= ~REG_CLK_32K_EXT3_EN;
        pmu_write(PMU_REG_PU_VSYS_DIV, val);
    }
    pmu_ext_crystal_clkout_usr_map &= ~pin;
}

void pmu_external_crystal_capbit_set(uint32_t capbit)
{
    uint16_t val;

    pmu_read(PMU_REG_PU_VSYS_DIV, &val);
    val = SET_BITFIELD(val, REG_XO_32K_CAPBIT, capbit);
    pmu_write(PMU_REG_PU_VSYS_DIV, val);
}

void pmu_external_crystal_capbit_get(uint32_t *p_capbit)
{
    uint16_t val;

    pmu_read(PMU_REG_PU_VSYS_DIV, &val);
    *p_capbit = GET_BITFIELD(val, REG_XO_32K_CAPBIT);
}

#ifdef PMU_CLK_USE_EXT_CRYSTAL
int pmu_external_crystal_calib(uint32_t *p_wait_ms)
{
    uint32_t time;
    int ret;
    float hz;
    uint32_t capbit;

    DRIVERS_TRACE(0, "%s: cur_state=%d", __func__, xtal_32k_calib_state);

    *p_wait_ms = 0;
    ret = -100;

    if (xtal_32k_calib_state == XTAL_32K_CALIB_STATE_NULL) {
_null_start:
        xtal_32k_calib_prev_valid = false;
_calib_start:
        hal_sys_timer_calib_start();
        hal_bus_wake_lock(HAL_BUS_WAKE_LOCK_USER_PMU);
        xtal_32k_calib_state = XTAL_32K_CALIB_STATE_CALIB;
        xtal_32k_calib_time = hal_sys_timer_get();
        *p_wait_ms = xtal_32k_calib_intvl_ms;
        ret = 1;
        goto _exit;
    } else if (xtal_32k_calib_state == XTAL_32K_CALIB_STATE_CALIB) {
        time = hal_sys_timer_get();
        time = TICKS_TO_MS(time - xtal_32k_calib_time);
        if (time < xtal_32k_calib_intvl_ms) {
            *p_wait_ms = xtal_32k_calib_intvl_ms - time;
            ret = 2;
            goto _exit;
        }
        ret = hal_sys_timer_calib_end();
        hal_bus_wake_unlock(HAL_BUS_WAKE_LOCK_USER_PMU);
        if (ret) {
            DRIVERS_TRACE(0, "%s: calib_end failed with %d", __func__, ret);
            xtal_32k_calib_state = XTAL_32K_CALIB_STATE_NULL;
            ret = -1;
            goto _exit;
        }
        hz = hal_sys_timer_systick_hz_float();
        pmu_external_crystal_capbit_get(&capbit);
        if (xtal_32k_calib_prev_valid) {
            DRIVERS_TRACE(0, "%s: hz=%u.%02u capbit=0x%X prev=%u.%02u prev_capbit=0x%X",
                __func__,
                (unsigned)hz, ((unsigned)(hz * 100) - ((unsigned)hz * 100)),
                capbit,
                (unsigned)xtal_32k_calib_prev_hz, ((unsigned)(xtal_32k_calib_prev_hz * 100) - ((unsigned)xtal_32k_calib_prev_hz * 100)),
                xtal_32k_calib_prev_capbit);
        } else {
            DRIVERS_TRACE(0, "%s: hz=%u.%02u", __func__, (unsigned)hz, ((unsigned)(hz * 100) - ((unsigned)hz * 100)));
        }
        if (ABS(hz - (CONFIG_SYSTICK_HZ_NOMINAL)) < 0.1) {
            // Calib result OK
            DRIVERS_TRACE(0, "%s: Calib result OK", __func__);
            xtal_32k_calib_state = XTAL_32K_CALIB_STATE_NULL;
            ret = 0;
            goto _exit;
        } else if (xtal_32k_calib_prev_valid) {
            // If no better result
            if (hz < (CONFIG_SYSTICK_HZ_NOMINAL) && (CONFIG_SYSTICK_HZ_NOMINAL) < xtal_32k_calib_prev_hz) {
                if (CONFIG_SYSTICK_HZ_NOMINAL - hz <= xtal_32k_calib_prev_hz - (CONFIG_SYSTICK_HZ_NOMINAL)) {
                    DRIVERS_TRACE(0, "%s: Calib no better result: Select cur", __func__);
                    xtal_32k_calib_state = XTAL_32K_CALIB_STATE_NULL;
                    ret = 0;
                    goto _exit;
                } else {
                    DRIVERS_TRACE(0, "%s: Calib no better result: Select prev", __func__);
                    pmu_external_crystal_capbit_set(xtal_32k_calib_prev_capbit);
                    xtal_32k_calib_state = XTAL_32K_CALIB_STATE_ROLLBACK;
                    xtal_32k_calib_time = hal_sys_timer_get();
                    ret = 0;
                    goto _exit;
                }
            } else if (xtal_32k_calib_prev_hz < (CONFIG_SYSTICK_HZ_NOMINAL) && (CONFIG_SYSTICK_HZ_NOMINAL) < hz) {
                if (hz - (CONFIG_SYSTICK_HZ_NOMINAL) <= (CONFIG_SYSTICK_HZ_NOMINAL) - xtal_32k_calib_prev_hz) {
                    DRIVERS_TRACE(0, "%s: Calib no better result: Select cur", __func__);
                    xtal_32k_calib_state = XTAL_32K_CALIB_STATE_NULL;
                    ret = 0;
                    goto _exit;
                } else {
                    DRIVERS_TRACE(0, "%s: Calib no better result: Select prev", __func__);
                    pmu_external_crystal_capbit_set(xtal_32k_calib_prev_capbit);
                    xtal_32k_calib_state = XTAL_32K_CALIB_STATE_ROLLBACK;
                    xtal_32k_calib_time = hal_sys_timer_get();
                    ret = 0;
                    goto _exit;
                }

            }
        }
        // Save as previous values
        xtal_32k_calib_prev_hz = hz;
        xtal_32k_calib_prev_capbit = capbit;
        xtal_32k_calib_prev_valid = true;
        if (hz > (CONFIG_SYSTICK_HZ_NOMINAL)) {
            if (capbit >= (REG_XO_32K_CAPBIT_MASK >> REG_XO_32K_CAPBIT_SHIFT)) {
                DRIVERS_TRACE(0, "%s: capbit too large: 0x%X", __func__, capbit);
                xtal_32k_calib_state = XTAL_32K_CALIB_STATE_NULL;
                ret = -2;
                goto _exit;
            }
            capbit += 0x10;
            if (capbit > (REG_XO_32K_CAPBIT_MASK >> REG_XO_32K_CAPBIT_SHIFT)) {
                capbit = (REG_XO_32K_CAPBIT_MASK >> REG_XO_32K_CAPBIT_SHIFT);
            }
        } else {
            if (capbit <= 1) {
                DRIVERS_TRACE(0, "%s: capbit too small: 0x%X", __func__, capbit);
                xtal_32k_calib_state = XTAL_32K_CALIB_STATE_NULL;
                ret = -3;
                goto _exit;
            }
            if (capbit > 0x10) {
                capbit -= 0x10;
            } else {
                capbit = 1;
            }
        }
        DRIVERS_TRACE(0, "%s: Set capbit=0x%X", __func__, capbit);
        pmu_external_crystal_capbit_set(capbit);
        xtal_32k_calib_state = XTAL_32K_CALIB_STATE_SET;
        xtal_32k_calib_time = hal_sys_timer_get();
        *p_wait_ms = xtal_32k_calib_stable_ms;
        ret = 3;
        goto _exit;
    } else if (xtal_32k_calib_state == XTAL_32K_CALIB_STATE_SET) {
        time = hal_sys_timer_get();
        time = TICKS_TO_MS(time - xtal_32k_calib_time);
        if (time < xtal_32k_calib_stable_ms) {
            *p_wait_ms = xtal_32k_calib_stable_ms - time;
            ret = 4;
            goto _exit;
        }
        goto _calib_start;
    } else if (xtal_32k_calib_state == XTAL_32K_CALIB_STATE_ROLLBACK) {
        time = hal_sys_timer_get();
        time = TICKS_TO_MS(time - xtal_32k_calib_time);
        if (time < xtal_32k_calib_stable_ms) {
            *p_wait_ms = xtal_32k_calib_stable_ms - time;
            ret = 5;
            goto _exit;
        }
        goto _null_start;
    } else {
        ASSERT(false, "%s: Bad xtal_32k_calib_state=%d", __func__, xtal_32k_calib_state);
    }

_exit:
    DRIVERS_TRACE(0, "%s: ret=%d next_state=%d", __func__, ret, xtal_32k_calib_state);
    return ret;
}
#endif
#endif

void pmu_capsensor_close(void)
{
}

void pmu_capsensor_open(uint8_t clk_cap_div)
{
}

void pmu_ldo_vsensor_off_pull_down(void)
{
    uint32_t lock;
    uint16_t val;

    lock = int_lock();
    pmu_module_config(PMU_SENSOR,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);

    pmu_read(PMU_REG_LDO_SENSOR_CFG, &val);
    val |= REG_LDO_VSENSOR_PULLDOWN;
    pmu_write(PMU_REG_LDO_SENSOR_CFG, val);
    int_unlock(lock);
}

void pmu_ldo_vsensor_on_lp_dr(bool dr_lp)
{
    uint32_t lock;
    uint16_t val;

    lock = int_lock();
    pmu_read(PMU_REG_SENSOR_CFG, &val);
    val &= ~REG_PULL_DOWN_VSENSOR;
    pmu_write(PMU_REG_SENSOR_CFG, val);

    pmu_module_config(PMU_SENSOR,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);

#ifndef PMU_FORCE_LP_MODE
    if (dr_lp) {
        pmu_read(PMU_REG_LDO_SENSOR_CFG, &val);
        val |= REG_LP_EN_LDO_VSENSOR | REG_LP_EN_LDO_VSENSOR_DR;
        pmu_write(PMU_REG_LDO_SENSOR_CFG, val);
    }
#endif
    int_unlock(lock);
}

void pmu_charger_pattern_enable(bool enable)
{
    uint16_t val;

    pmu_read(PMU_REG_NOT_RESET_61, &val);
    if (enable) {
        val &= ~REG_COMMAND_RST_DIS;
    } else {
        val |= REG_COMMAND_RST_DIS;
    }
    pmu_write(PMU_REG_NOT_RESET_61, val);
}

#ifdef RC_FAST_WAKEUP
void pmu_fast_wakeup_enable(void)
{
    uint16_t val;

    pmu_read(PMU_REG_RCOSC_2M_CFG, &val);
    val |= REG_RCOSC_2M_EN_CLK_PMU_DIG;
    pmu_write(PMU_REG_RCOSC_2M_CFG, val);

    pmu_read(PMU_REG_RC_RAMP_MODE_CFG, &val);
    val |= REG_RAMP_MODE_SEL;
    // bit0 pu_osc bit1 pu_rc
    val = SET_BITFIELD(val, REG_2M_RAMP_GATE_MSK, 0x2);
    pmu_write(PMU_REG_RC_RAMP_MODE_CFG, val);
}
#endif
