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
#include CHIP_SPECIFIC_HDR(charger)
#include CHIP_SPECIFIC_HDR(reg_charger)
#include "analog.h"
#include "cmsis.h"
#include "cmsis_nvic.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
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

#define PMU_EFUSE_INIT_IN_BOOT
#ifndef MTEST_ENABLED
#define PMU_INTF_CALIBED_VOLT
#endif
#define PMU_DCDC_PWM

#if !defined(DIG_DCDC_MODE) && !defined(PMU_LDO_VCORE_CALIB)
#define PMU_LDO_VCORE_CALIB
#endif

#ifdef PMU_ALREADY_INIT
#define PMU_OPEN_BSS_LOC                BOOT_BSS_LOC
#else
#define PMU_OPEN_BSS_LOC
#endif

#ifdef PMU_IRQ_UNIFIED
#define PMU_IRQ_HDLR_PARAM              uint16_t irq_status
#else
#define PMU_IRQ_HDLR_PARAM              void
#endif

#define cap_read(reg,val)               hal_analogif_reg_read(CAP_REG(reg),val)
#define cap_write(reg,val)              hal_analogif_reg_write(CAP_REG(reg),val)
#define ana_read(reg,val)               hal_analogif_reg_read(ANA_REG(reg),val)
#define ana_write(reg,val)              hal_analogif_reg_write(ANA_REG(reg),val)
#define rf_read(reg,val)                hal_analogif_reg_read(RF_REG(reg),val)
#define rf_write(reg,val)               hal_analogif_reg_write(RF_REG(reg),val)
#define chg_read(reg,val)               hal_analogif_reg_read(CHG_REG(reg),val)
#define chg_write(reg,val)              hal_analogif_reg_write(CHG_REG(reg),val)

// LDO soft start interval is about 1000 us
#define PMU_LDO_PU_STABLE_TIME_US       1800
#define PMU_LDO_PULLDOWN_STABLE_TIME_US 1000
#define PMU_LDO_VRTC_STABLE_TIME_US     1000
#define PMU_DCDC_PU_STABLE_TIME_US      100
#define PMU_DCDC_HW_RAMP_1_CLK_TIME_US  40
#define PMU_DCDC_HW_RAMP_1_CLK_STEP     4
#define PMU_DCDC_HW_RAMP_1_CLK_STEP_HALF (PMU_DCDC_HW_RAMP_1_CLK_STEP / 2)
#define PMU_VANA_STABLE_TIME_US         10
#define PMU_VCORE_STABLE_TIME_US        10

// External 32.768KHz stable time(ms)
#define PMU_EXT_32K_STABLE_TIME         300
#define PMU_EXT_32K_CLK_OUT_STABLE_TIME 1
#define PMU_CLK_SWITCH_STABLE_TIME      1
#define PMU_EXT_32K_CAPBIT_DEFAULT      0x141

// 3.75mV
#define PMU_DCDC_DIG_1_1V               0xD5
#define PMU_DCDC_DIG_1_05V              0xC7
#define PMU_DCDC_DIG_1_0V               0xBA
#define PMU_DCDC_DIG_0_95V              0xAD
#define PMU_DCDC_DIG_0_9V               0xA0
#define PMU_DCDC_DIG_0_86V              0x95
#define PMU_DCDC_DIG_0_85V              0x92
#define PMU_DCDC_DIG_0_84V              0x90
#define PMU_DCDC_DIG_0_8V               0x85
#define PMU_DCDC_DIG_0_78V              0x80
#define PMU_DCDC_DIG_0_75V              0x77
#define PMU_DCDC_DIG_0_74V              0x75
#define PMU_DCDC_DIG_0_7V               0x6A
#define PMU_DCDC_DIG_0_65V              0x5D
#define PMU_DCDC_DIG_0_6V               0x4F
#define PMU_DCDC_DIG_50_MV              (PMU_DCDC_DIG_0_85V - PMU_DCDC_DIG_0_8V)
#define PMU_DCDC_DIG_DEFAULT            0x85 /* 0.8V */
#define PMU_DCDC_DIG_20_MV              0x5
#define PMU_DCDC_DIG_40_MV              0xB
#define PMU_DCDC_DIG_60_MV              0x10

// 25mV
#define PMU_ANA_1_4V                    0x14
#define PMU_ANA_1_35V                   0x12
#define PMU_ANA_1_3V                    0x10
#define PMU_ANA_1_25V                   0xE
#define PMU_ANA_1_2V                    0xC
#define PMU_ANA_1_1V                    0x8
#define PMU_ANA_1_0V                    0x4
#define PMU_ANA_DEFAULT                 0x10 /* 1.3V */

// 7.5mV
#define PMU_DCDC_HPPA_1_9V              0xB3
#define PMU_DCDC_HPPA_1_8V              0xA0
#define PMU_DCDC_HPPA_1_75V             0x99
#define PMU_DCDC_HPPA_1_7V              0x92
#define PMU_DCDC_HPPA_1_65V             0x8C
#define PMU_DCDC_HPPA_1_6V              0x85
#define PMU_DCDC_HPPA_1_55V             0x7E
#define PMU_DCDC_HPPA_1_5V              0x78
#define PMU_DCDC_HPPA_1_45V             0x71
#define PMU_DCDC_HPPA_1_4V              0x6A
#define PMU_DCDC_HPPA_DEFAULT           0xA0 /* 1.8V */

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

#define PMU_VMIC_2_8V                   0x37
#define PMU_VMIC_1_6V                   0x16

#define PMU_VMIC_RES_3_3V               0xF
#define PMU_VMIC_RES_2_8V               0xA

#define MAX_BUCK_VANA_BIT_VAL                   (REG_BUCK_VANA_VBIT_NORMAL_MASK >> REG_BUCK_VANA_VBIT_NORMAL_SHIFT)
#define MAX_BUCK_VCORE_BIT_VAL                  (REG_BUCK_VCORE_VBIT_NORMAL_MASK >> REG_BUCK_VCORE_VBIT_NORMAL_SHIFT)
#define MAX_BUCK_VHPPA_BIT_VAL                  (REG_BUCK_VHPPA_VBIT_NORMAL_MASK >> REG_BUCK_VHPPA_VBIT_NORMAL_SHIFT)
#define MAX_LDO_VCORE_L_BIT_VAL                 (REG_LDO_VCORE_L_VBIT_NORMAL_MASK >> REG_LDO_VCORE_L_VBIT_NORMAL_SHIFT)

#define INTR_MSKED_CHARGE_OUT                   INTR_MSKED_CHARGE(1 << 1)
#define INTR_MSKED_CHARGE_IN                    INTR_MSKED_CHARGE(1 << 0)

#define REG_DCDC1_RAMP_EN                       (1 << 1)
#define REG_DCDC2_RAMP_EN                       (1 << 2)
#define REG_DCDC3_RAMP_EN                       (1 << 3)

#define LDO_VANA_RAMP_EN                        (1 << 0)

#define REG_DCDC_CALIB_MTEST                    PMU_REG_RTC_LOAD_HOURS

// PMU_VMIN_SEARCH
#define PMU_VMIN_SET_REG_ADDR                   PMU_REG_RTC_LOAD_MIN_SEC_EN
#define PMU_VMIN_VCORE_SET_DONE                 (1 << 15)
#define PMU_VMIN_VCORE_SET_PLUS                 (1 << 14)
#define PMU_VMIN_VCORE_SET_OFFSET_SHIFT         0
#define PMU_VMIN_VCORE_SET_OFFSET_MASK          (0x1F << PMU_VMIN_VCORE_SET_OFFSET_SHIFT)
#define PMU_VMIN_VCORE_SET_OFFSET(n)            BITFIELD_VAL(PMU_VMIN_VCORE_SET_OFFSET, n)

// PMU_EFUSE_PAGE_BATTER_LV
#define PMU_EFUSE_PAGE_BATTER_LV_OFFSET_SHIFT   0
#define PMU_EFUSE_PAGE_BATTER_LV_OFFSET_MASK    (0xFFF << PMU_EFUSE_PAGE_BATTER_LV_OFFSET_SHIFT)
#define PMU_EFUSE_PAGE_BATTER_LV_OFFSET(n)      BITFIELD_VAL(PMU_EFUSE_PAGE_BATTER_LV_OFFSET, n)
#define PMU_EFUSE_PAGE_BATTER_LV_BASE           26400

// PMU_EFUSE_PAGE_BATTER_HV
#define PMU_EFUSE_PAGE_BATTER_HV_OFFSET_SHIFT   0
#define PMU_EFUSE_PAGE_BATTER_HV_OFFSET_MASK    (0xFFF << PMU_EFUSE_PAGE_BATTER_HV_OFFSET_SHIFT)
#define PMU_EFUSE_PAGE_BATTER_HV_OFFSET(n)      BITFIELD_VAL(PMU_EFUSE_PAGE_BATTER_HV_OFFSET, n)
#define PMU_EFUSE_PAGE_BATTER_HV_BASE           30400
#define PMU_EFUSE_DCDC_IS_GAIN_CALIB            (1 << 15)

// PMU_EFUSE_PAGE_EXT_GPADC_LV
#define PMU_EFUSE_PAGE_EXT_GPADC_LV_OFFSET_SHIFT 0
#define PMU_EFUSE_PAGE_EXT_GPADC_LV_OFFSET_MASK  (0xFFF << PMU_EFUSE_PAGE_EXT_GPADC_LV_OFFSET_SHIFT)
#define PMU_EFUSE_PAGE_EXT_GPADC_LV_OFFSET(n)    BITFIELD_VAL(PMU_EFUSE_PAGE_EXT_GPADC_LV_OFFSET, n)
#define PMU_EFUSE_PAGE_EXT_GPADC_LV_BASE         19900
#define PMU_EFUSE_DCDC_CFG_TYPE_SHIFT            12
#define PMU_EFUSE_DCDC_CFG_TYPE_MASK             (0x3 << PMU_EFUSE_DCDC_CFG_TYPE_SHIFT)
#define PMU_EFUSE_DCDC_CFG_TYPE(n)               BITFIELD_VAL(PMU_EFUSE_DCDC_CFG_TYPE, n)

// PMU_EFUSE_PAGE_EXT_GPADC_HV
#define PMU_EFUSE_PAGE_EXT_GPADC_HV_OFFSET_SHIFT 0
#define PMU_EFUSE_PAGE_EXT_GPADC_HV_OFFSET_MASK  (0xFFF << PMU_EFUSE_PAGE_EXT_GPADC_HV_OFFSET_SHIFT)
#define PMU_EFUSE_PAGE_EXT_GPADC_HV_OFFSET(n)    BITFIELD_VAL(PMU_EFUSE_PAGE_EXT_GPADC_HV_OFFSET, n)
#define PMU_EFUSE_PAGE_EXT_GPADC_HV_BASE         36100
#define PMU_EFUSE_VMIN_MAP_SHIFT                 12
#define PMU_EFUSE_VMIN_MAP_MASK                  (0x3 << PMU_EFUSE_VMIN_MAP_SHIFT)
#define PMU_EFUSE_VMIN_MAP(n)                    BITFIELD_VAL(PMU_EFUSE_VMIN_MAP, n)
#define PMU_EFUSE_VMIN_HF_MAP_SHIFT              14
#define PMU_EFUSE_VMIN_HF_MAP_MASK               (0x3 << PMU_EFUSE_VMIN_HF_MAP_SHIFT)
#define PMU_EFUSE_VMIN_HF_MAP(n)                 BITFIELD_VAL(PMU_EFUSE_VMIN_HF_MAP, n)

#define PMU_EFUSE_GPADC_CALIB_DEFAULT            2000

// PMU_EFUSE_PAGE_VBG_VRTC_CALIB
#define PMU_EFUSE_VBG_PMU_INTF_CALIB_VBG        (1 << 10)
#define PMU_EFUSE_VBG_PMU_INTF_CALIB_VRTC       (1 << 11)
#define PMU_EFUSE_VBG_PMU_INTF_CALIB_VCORE      (1 << 12)
#define PMU_EFUSE_VBG_PMU_INTF_CALIB_VANA       (1 << 13)

#define PMU_EFUSE_VBG_PMU_INTF_CALIB_ENABLE     PMU_EFUSE_VBG_PMU_INTF_CALIB_VBG

// PMU_EFUSE_PAGE_DCDC_VOLT_CALIB
#define PMU_EFUSE_DCDC1_VOLT_COMP_SHIFT         0
#define PMU_EFUSE_DCDC1_VOLT_COMP_MASK          (0x1F << PMU_EFUSE_DCDC1_VOLT_COMP_SHIFT)
#define PMU_EFUSE_DCDC1_VOLT_COMP(n)            BITFIELD_VAL(PMU_EFUSE_DCDC1_VOLT_COMP, n)
#define PMU_EFUSE_DCDC1_VOLT_COMP_SIGN          (1 << 5)
#define PMU_EFUSE_LDO_ANA_VOLT_COMP_SHIFT       6
#define PMU_EFUSE_LDO_ANA_VOLT_COMP_MASK        (0xF << PMU_EFUSE_LDO_ANA_VOLT_COMP_SHIFT)
#define PMU_EFUSE_LDO_ANA_VOLT_COMP(n)          BITFIELD_VAL(PMU_EFUSE_LDO_ANA_VOLT_COMP, n)
#define PMU_EFUSE_LDO_ANA_VOLT_COMP_SIGN        (1 << 10)
#define PMU_EFUSE_LDO_ANA_VOLT_SHIFT            6
#define PMU_EFUSE_LDO_ANA_VOLT_MASK             (0x1F << PMU_EFUSE_LDO_ANA_VOLT_SHIFT)
#define PMU_EFUSE_LDO_ANA_VOLT(n)               BITFIELD_VAL(PMU_EFUSE_LDO_ANA_VOLT, n)
#define PMU_EFUSE_DCDC3_VOLT_COMP_SHIFT         11
#define PMU_EFUSE_DCDC3_VOLT_COMP_MASK          (0xF << PMU_EFUSE_DCDC3_VOLT_COMP_SHIFT)
#define PMU_EFUSE_DCDC3_VOLT_COMP(n)            BITFIELD_VAL(PMU_EFUSE_DCDC3_VOLT_COMP, n)
#define PMU_EFUSE_DCDC3_VOLT_COMP_SIGN          (1 << 15)

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

#define PMU_EFUSE_RAM_EMA_SIGN                  (1 << 15)

// ANALOG REG_5A
#define REG_ANALOG_SOFT_RESETN_CAPSENSOR        (1 << 14)
#define REG_ANALOG_SOFT_RESETN_CAPSENSOR_REG    (1 << 15)

// ANALOG REG_16D
#define REG_ANALOG_PU_OSC                       (1 << 6)

// ANALOG REG_1AD
#define REG_ANALOG_CLK_SEL_DSI                  (1 << 0)
#define REG_ANALOG_EN_CLK_DSI                   (1 << 1)

// RF_REG_50F
#define REG_BT_XTAL_PU                          (1 << 8)
#define REG_BT_XTAL_PU_DR                       (1 << 7)

// RF_REG_526
#define PU_OSC_DLY_DR                           (1 << 1)

// RF_REG_527
#define REG_BT_BG_PU                            (1 << 1)
#define REG_BT_BG_PU_DR                         (1 << 0)

// RF_REG_517
#define REG_XTAL_MDLL_OUTBUFFER_SWRC_I_CH22_SHIFT  0
#define REG_XTAL_MDLL_OUTBUFFER_SWRC_I_CH22_MASK   (0x7 << REG_XTAL_MDLL_OUTBUFFER_SWRC_I_CH22_SHIFT)
#define REG_XTAL_MDLL_OUTBUFFER_SWRC_I_CH22(n)     BITFIELD_VAL(REG_XTAL_MDLL_OUTBUFFER_SWRC_I_CH22, n)
#define REG_XTAL_MDLL_OUTBUFFER_SWRC_I_CH46_SHIFT  3
#define REG_XTAL_MDLL_OUTBUFFER_SWRC_I_CH46_MASK   (0x7 << REG_XTAL_MDLL_OUTBUFFER_SWRC_I_CH46_SHIFT)
#define REG_XTAL_MDLL_OUTBUFFER_SWRC_I_CH46(n)     BITFIELD_VAL(REG_XTAL_MDLL_OUTBUFFER_SWRC_I_CH46, n)
#define REG_XTAL_MDLL_OUTBUFFER_SWRC_I_CH70_SHIFT  6
#define REG_XTAL_MDLL_OUTBUFFER_SWRC_I_CH70_MASK   (0x7 << REG_XTAL_MDLL_OUTBUFFER_SWRC_I_CH70_SHIFT)
#define REG_XTAL_MDLL_OUTBUFFER_SWRC_I_CH70(n)     BITFIELD_VAL(REG_XTAL_MDLL_OUTBUFFER_SWRC_I_CH70, n)
#define REG_XTAL_MDLL_OUTBUFFER_SWRC_I_OTHER_SHIFT 9
#define REG_XTAL_MDLL_OUTBUFFER_SWRC_I_OTHER_MASK  (0x7 << REG_XTAL_MDLL_OUTBUFFER_SWRC_I_OTHER_SHIFT)
#define REG_XTAL_MDLL_OUTBUFFER_SWRC_I_OTHER(n)    BITFIELD_VAL(REG_XTAL_MDLL_OUTBUFFER_SWRC_I_OTHER, n)

// RF_REG_524
#define REG_XTAL_MDLL_CAL_WAIT_TIME_SHIFT       3
#define REG_XTAL_MDLL_CAL_WAIT_TIME_MASK        (0x3F << REG_XTAL_MDLL_CAL_WAIT_TIME_SHIFT)
#define REG_XTAL_MDLL_CAL_WAIT_TIME(n)          BITFIELD_VAL(REG_XTAL_MDLL_CAL_WAIT_TIME, n)

// RF_REG_50C
#define REG_XTAL_LDO_VTUNE_OTHER_SHIFT         7
#define REG_XTAL_LDO_VTUNE_OTHER_MASK          (0x7 << REG_XTAL_LDO_VTUNE_OTHER_SHIFT)
#define REG_XTAL_LDO_VTUNE_OTHER(n)            BITFIELD_VAL(REG_XTAL_LDO_VTUNE_OTHER, n)

// RF_REG_50E
#define REG_XTAL_MDLL_CP_SWRC_SHIFT            0
#define REG_XTAL_MDLL_CP_SWRC_MASK             (0x7 << REG_XTAL_MDLL_CP_SWRC_SHIFT)
#define REG_XTAL_MDLL_CP_SWRC(n)               BITFIELD_VAL(REG_XTAL_MDLL_CP_SWRC, n)

// RF_REG_518
#define REG_XTAL_LDO_VTUNE_CH22_SHIFT              0
#define REG_XTAL_LDO_VTUNE_CH22_MASK               (0x7 << REG_XTAL_LDO_VTUNE_CH22_SHIFT)
#define REG_XTAL_LDO_VTUNE_CH22(n)                 BITFIELD_VAL(REG_XTAL_LDO_VTUNE_CH22, n)
#define REG_XTAL_LDO_VTUNE_CH46_SHIFT              3
#define REG_XTAL_LDO_VTUNE_CH46_MASK               (0x7 << REG_XTAL_LDO_VTUNE_CH46_SHIFT)
#define REG_XTAL_LDO_VTUNE_CH46(n)                 BITFIELD_VAL(REG_XTAL_LDO_VTUNE_CH46, n)
#define REG_XTAL_LDO_VTUNE_CH70_SHIFT              6
#define REG_XTAL_LDO_VTUNE_CH70_MASK               (0x7 << REG_XTAL_LDO_VTUNE_CH70_SHIFT)
#define REG_XTAL_LDO_VTUNE_CH70(n)                 BITFIELD_VAL(REG_XTAL_LDO_VTUNE_CH70, n)

// RF_REG_543
#define REG_XTAL_MDLL_PFD_ERROR_DET_EN          (1 << 3)

// RF_REG_544
#define REG_XTAL_MDLL_LPF_PRECH_BIAS_SHIFT      7
#define REG_XTAL_MDLL_LPF_PRECH_BIAS_MASK       (0x3 << REG_XTAL_MDLL_LPF_PRECH_BIAS_SHIFT)
#define REG_XTAL_MDLL_LPF_PRECH_BIAS(n)         BITFIELD_VAL(REG_XTAL_MDLL_LPF_PRECH_BIAS, n)
#define REG_XTAL_MDLL_DLL_SWRC_SHIFT            0
#define REG_XTAL_MDLL_DLL_SWRC_MASK             (0x7 << REG_XTAL_MDLL_DLL_SWRC_SHIFT)
#define REG_XTAL_MDLL_DLL_SWRC(n)               BITFIELD_VAL(REG_XTAL_MDLL_DLL_SWRC, n)

// RF_REG_550
#define REG_XTAL_MAIN_ICORE_SEL_SHIFT           8
#define REG_XTAL_MAIN_ICORE_SEL_MASK            (0x1F << REG_XTAL_MAIN_ICORE_SEL_SHIFT)
#define REG_XTAL_MAIN_ICORE_SEL(n)              BITFIELD_VAL(REG_XTAL_MAIN_ICORE_SEL, n)

// RF_REG_552
#define REG_XTAL_SWRC_ACBUF_MAIN_SHIFT          0
#define REG_XTAL_SWRC_ACBUF_MAIN_MASK           (0x1F << REG_XTAL_SWRC_ACBUF_MAIN_SHIFT)
#define REG_XTAL_SWRC_ACBUF_MAIN(n)              BITFIELD_VAL(REG_XTAL_SWRC_ACBUF_MAIN, n)
#define REG_XTAL_SWRC_ACBUF_SUB_SHIFT           5
#define REG_XTAL_SWRC_ACBUF_SUB_MASK            (0x1F << REG_XTAL_SWRC_ACBUF_SUB_SHIFT)
#define REG_XTAL_SWRC_ACBUF_SUB(n)              BITFIELD_VAL(REG_XTAL_SWRC_ACBUF_SUB, n)
#define REG_XTAL_ACBUF_MAIN_SEL_SHIFT           10
#define REG_XTAL_ACBUF_MAIN_SEL_MASK            (0x3 << REG_XTAL_ACBUF_MAIN_SEL_SHIFT)
#define REG_XTAL_ACBUF_MAIN_SEL(n)              BITFIELD_VAL(REG_XTAL_ACBUF_MAIN_SEL, n)

#define PMU_EFUSE_TEMPSENSOR_DEFAULT            32347

// EFUSE_REG_5
#define REG_EFUSE_SRAM_CFG_SHIFT                12
#define REG_EFUSE_SRAM_CFG_MASK                 (0x3 << REG_EFUSE_SRAM_CFG_SHIFT)
#define REG_EFUSE_SRAM_CFG(n)                   BITFIELD_VAL(REG_EFUSE_SRAM_CFG, n)
#define REG_EFUSE_MIC_CFG_SHIFT                 14
#define REG_EFUSE_MIC_CFG_MASK                  (0x1 << REG_EFUSE_MIC_CFG_SHIFT)
#define REG_EFUSE_MIC_CFG(n)                    BITFIELD_VAL(REG_EFUSE_MIC_CFG, n)
#define REG_EFUSE_DUAL_CPU_CFG_SHIFT            15
#define REG_EFUSE_DUAL_CPU_CFG_MASK             (0x1 << REG_EFUSE_DUAL_CPU_CFG_SHIFT)
#define REG_EFUSE_DUAL_CPU_CFG(n)               BITFIELD_VAL(REG_EFUSE_DUAL_CPU_CFG, n)

#define REG_CP_WAFER_CTX_XY                     PMU_EFUSE_PAGE_CP_WAFER_XY_CTX
#define REG_CP_WAFER_CTX_ID                     PMU_EFUSE_PAGE_CP_WAFER_ID

enum RF_ANA_CHG_REG_T {
    ANA_REG_5A          = 0x5A,
    ANA_REG_60          = 0x60,

    RF_REG_00           = 0x00,
    RF_REG_50C          = 0x50C,
    RF_REG_50E          = 0x50E,
    RF_REG_50F          = 0x50F,
    RF_REG_517          = 0x517,
    RF_REG_518          = 0x518,
    RF_REG_524          = 0x524,
    RF_REG_526          = 0x526,
    RF_REG_527          = 0x527,
    RF_REG_543          = 0x543,
    RF_REG_544          = 0x544,
    RF_REG_550          = 0x550,
    RF_REG_552          = 0x552,
};

enum PMU_VCORE_REQ_T {
    PMU_VCORE_FLASH_WRITE_ENABLED   = (1 << 0),
    PMU_VCORE_FLASH_FREQ_HIGH       = (1 << 1),
    PMU_VCORE_PSRAM_FREQ_HIGH       = (1 << 2),
    PMU_VCORE_PSRAM_FREQ_MEDIUM     = (1 << 3),
    PMU_VCORE_PSRAM_FREQ_MEDIUM_LOW = (1 << 4),
    PMU_VCORE_USB_HS_ENABLED        = (1 << 5),
    PMU_VCORE_SYS_FREQ_LOW          = (1 << 6),
    PMU_VCORE_SYS_FREQ_MEDIUM_LOW   = (1 << 7),
    PMU_VCORE_SYS_FREQ_MEDIUM       = (1 << 8),
    PMU_VCORE_SYS_FREQ_HIGH         = (1 << 9),
    PMU_VCORE_RS_FREQ_HIGH          = (1 << 10),
    PMU_VCORE_RS_ADC_FREQ_HIGH      = (1 << 11),
    PMU_VCORE_IIR_FREQ_HIGH         = (1 << 12),
    PMU_VCORE_IIR_EQ_FREQ_HIGH      = (1 << 13),
    PMU_VCORE_FIR_FREQ_HIGH         = (1 << 14),
    PMU_VCORE_PMU_RESET             = (1 << 15),
    PMU_VCORE_BT_RF                 = (1 << 16),
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

    PMU_MODULE_QTY,
};

struct PMU_MODULE_CFG_T {
    unsigned short manual_bit;
    unsigned short ldo_en;
    unsigned short lp_en_dr;
    unsigned short lp_en;
    unsigned short dsleep_mode;
    unsigned short dsleep_v;
    unsigned short dsleep_v_shift;
    unsigned short normal_v;
    unsigned short normal_v_shift;
    unsigned short rc_v;
    unsigned short rc_v_shift;
};

#define PMU_MOD_CFG_VAL(m)              { \
    REG_PU_LDO_V##m##_DR, REG_PU_LDO_V##m##_REG, \
    REG_LP_EN_LDO_V##m##_DR, REG_LP_EN_LDO_V##m##_REG, \
    REG_PU_LDO_V##m##_DSLEEP, \
    REG_LDO_V##m##_VBIT_DSLEEP_MASK, REG_LDO_V##m##_VBIT_DSLEEP_SHIFT,\
    REG_LDO_V##m##_VBIT_NORMAL_MASK, REG_LDO_V##m##_VBIT_NORMAL_SHIFT,\
    REG_LDO_V##m##_VBIT_RC_MASK, REG_LDO_V##m##_VBIT_RC_SHIFT }

static const struct PMU_MODULE_CFG_T pmu_module_cfg[] = {
    PMU_MOD_CFG_VAL(ANA),
};

#define OPT_TYPE                        const

static OPT_TYPE POSSIBLY_UNUSED uint16_t vcodec_mv = (uint16_t)(VCODEC_VOLT * 1000);
static OPT_TYPE POSSIBLY_UNUSED uint16_t vhppa_mv = (uint16_t)(VHPPA_VOLT * 1000);

static OPT_TYPE POSSIBLY_UNUSED uint8_t ana_act_ldo = PMU_ANA_1_35V;

static OPT_TYPE uint8_t dig_lp_dcdc = PMU_DCDC_DIG_0_7V;

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

static uint32_t BOOT_BSS_LOC pmu_metal_id;

static uint16_t wdt_irq_timer;
static uint16_t wdt_reset_timer;

static const bool dcdc_ramp_en =
#if defined(DCDC_VOLT_HW_RAMP_ONLY)
    true;
#else
    false;
#endif

#ifndef MTEST_ENABLED
const
#endif
#if defined(PMU_ALREADY_INIT)
BOOT_DATA_LOC
#endif
static bool intf_calib_en = true;

static uint16_t SRAM_BSS_DEF(dcdc_ramp_map);

static uint8_t SRAM_BSS_DEF(ldo_ramp_map);

static enum PMU_BOOT_CAUSE_T BOOT_BSS_LOC pmu_boot_reason = PMU_BOOT_CAUSE_NULL;

static bool BOOT_BSS_LOC pmu_boot_first_pwr_up = false;

static uint16_t BOOT_BSS_LOC vmin_offset = 0;
static uint16_t BOOT_BSS_LOC vmin_hf_offset = 0;

#ifdef RC_CLK_ENABLE
static enum PMU_CLK_SEL_RC_USER_T rc_clk_user_map = 0;
#endif

#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
static void pmu_voltage_resume(void);
#if !defined(ROM_BUILD) && defined(PMU_EFUSE_INIT_IN_BOOT)
static bool BOOT_BSS_LOC efuse_buf_valid;
static unsigned short pmu_efuse_page_arrays[PMU_EFUSE_PAGE_QTY];
#endif
#endif

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

    pmu_read(PMU_REG_POWER_KEY_CFG, &val);
    val |= PU_LPO_DR | PU_LPO_REG;
    pmu_write(PMU_REG_POWER_KEY_CFG, val);

    pmu_rtc_calendar_alarm_intr_enable();

#ifdef SIMU
    val = 32 - 2;
#else
    val = CONFIG_SYSTICK_HZ * 2 - 2;
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
static int8_t PMU_OPEN_BSS_LOC pmu_ldo_ana_comp = 0;

#ifdef PMU_ALREADY_INIT
BOOT_TEXT_FLASH_LOC
#endif
void pmu_get_ldo_dig_calib_value(void)
{
}
#endif

#ifdef PMU_DCDC_CALIB
static int16_t PMU_OPEN_BSS_LOC pmu_dcdc_dig_comp = 0;
static int8_t PMU_OPEN_BSS_LOC pmu_dcdc_hppa_comp = 0;

#ifdef PMU_ALREADY_INIT
BOOT_TEXT_FLASH_LOC
#endif
static POSSIBLY_UNUSED void pmu_get_dcdc_calib_value(void)
{
    uint16_t val;
    bool vana_intf_calib_en;

#ifdef MTEST_ENABLED
    pmu_get_efuse(PMU_EFUSE_PAGE_VBG_VRTC_CALIB, &val);

    intf_calib_en = !!(val & PMU_EFUSE_VBG_PMU_INTF_CALIB_ENABLE);
#endif

    pmu_get_efuse(PMU_EFUSE_PAGE_VBG_VRTC_CALIB, &val);

    vana_intf_calib_en = !!(val & PMU_EFUSE_VBG_PMU_INTF_CALIB_VANA);

    PMU_INFO_TRACE(3, "%s: intf_calib_en:%d vana_intf_calib_en:%d",
                        __func__, intf_calib_en, vana_intf_calib_en);

    if (intf_calib_en) {
        pmu_get_efuse(PMU_EFUSE_PAGE_DCDC_VOLT_CALIB, &val);
    } else {
        pmu_read(REG_DCDC_CALIB_MTEST, &val);
    }

    pmu_dcdc_dig_comp = GET_BITFIELD(val, PMU_EFUSE_DCDC1_VOLT_COMP);
    if (val & PMU_EFUSE_DCDC1_VOLT_COMP_SIGN) {
        pmu_dcdc_dig_comp = -pmu_dcdc_dig_comp;
    }

    if (vana_intf_calib_en) {
        pmu_ldo_ana_comp = GET_BITFIELD(val, PMU_EFUSE_LDO_ANA_VOLT) - PMU_ANA_DEFAULT;
    } else {
        pmu_ldo_ana_comp = GET_BITFIELD(val, PMU_EFUSE_LDO_ANA_VOLT_COMP);
        if (val & PMU_EFUSE_LDO_ANA_VOLT_COMP_SIGN) {
            pmu_ldo_ana_comp = -pmu_ldo_ana_comp;
        }
    }

    pmu_dcdc_hppa_comp = GET_BITFIELD(val, PMU_EFUSE_DCDC3_VOLT_COMP);
    if (val & PMU_EFUSE_DCDC3_VOLT_COMP_SIGN) {
        pmu_dcdc_hppa_comp = -pmu_dcdc_hppa_comp;
    }
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

    pmu_get_efuse(PMU_EFUSE_PAGE_RESERVED_7, &val);
    if (val & PMU_EFUSE_RAM_EMA_SIGN){
        hal_cmu_set_ram_timing();
    }

    pmu_boot_cause_init();

#ifdef __WATCHER_DOG_RESET__
    pmu_wdt_save_context();
#endif

#if !defined(FPGA) && !defined(PROGRAMMER)
    pmu_charger_save_context();
#endif

#ifndef PMU_ALREADY_INIT
    // Reset PMU (to recover from a possible insane state, e.g., ESD reset)
    pmu_write(PMU_REG_METAL_ID, 0xCAFE);
    pmu_write(PMU_REG_METAL_ID, 0x5FEE);
    hal_sys_timer_delay(US_TO_TICKS(500));

    // Enable auto load efuse
    pmu_write(PMU_REG_EFUSE_MSK, (REG_EFUSE_FLAG_TRIM_VBG_AUX | REG_EFUSE_FLAG_BUCK_VCORE_VBIT_NORMAL));
    hal_sys_timer_delay(US_TO_TICKS(100));

    // Disable hw ramp, use sw ramp in pmu_dig_init_volt() by default.
    pmu_read(PMU_REG_BUCK_VCORE_EN, &val);
    val &= ~REG_BUCK_VCORE_RAMP_EN;
    pmu_write(PMU_REG_BUCK_VCORE_EN, val);

    // VANA use sw ramp by default.
    pmu_read(PMU_REG_LDO_ANA_CFG, &val);
    val &= ~REG_LDO_VANA_RAMP_EN;
    pmu_write(PMU_REG_LDO_ANA_CFG, val);
#endif

    pmu_read(PMU_REG_CHARGER_CFG, &val);
    val |= REG_PMU_CHARGE_INTR_EN;
    pmu_write(PMU_REG_CHARGER_CFG, val);

#ifdef CHG_FORCE_LP_PWROFF
    // Resume chg bg cfg
    chg_read(CHG_REG_TEMP_BG_CFG, &val);
    val |= REG_BG_CORE_EN;
    chg_write(CHG_REG_TEMP_BG_CFG, val);

    val |= REG_BG_REF_GEN_EN;
    chg_write(CHG_REG_TEMP_BG_CFG, val);

    hal_sys_timer_delay(MS_TO_TICKS(1));

    val |= REG_BG_EN_OTP;
    chg_write(CHG_REG_TEMP_BG_CFG, val);
#endif

    chg_write(CHG_REG_METAL_ID, 0xCAFE);
    chg_write(CHG_REG_METAL_ID, 0x5FEE);
    hal_sys_timer_delay(US_TO_TICKS(500));

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

    pmu_read(PMU_REG_LPO_CFG, &val);
    val = SET_BITFIELD(val, REG_LPO_CONST_BIAS_SEL, 0x0);
    val = SET_BITFIELD(val, REG_LPO_BIAS_SEL, 0xF);
    pmu_write(PMU_REG_LPO_CFG, val);
    hal_sys_timer_delay(MS_TO_TICKS(1));

    pmu_rf_ana_init();
}

void BOOT_TEXT_FLASH_LOC pmu_rf_ana_init(void)
{
#if defined(PMU_FULL_INIT) || (!defined(FPGA) && !defined(PROGRAMMER) && !defined(PMU_ALREADY_INIT))
    uint16_t val, vtmp, ldo_vtune;

    // Reset RF
    rf_write(RF_REG_00, 0xCAFE);
    rf_write(RF_REG_00, 0x5FEE);
    hal_sys_timer_delay(US_TO_TICKS(500));

    // Reset ANA
    ana_write(ANA_REG_60, 0xCAFE);
    ana_write(ANA_REG_60, 0x5FEE);
    hal_sys_timer_delay(US_TO_TICKS(500));

    ana_read(ANA_REG_5A, &val);
    val &= ~(REG_ANALOG_SOFT_RESETN_CAPSENSOR | REG_ANALOG_SOFT_RESETN_CAPSENSOR_REG);
    ana_write(ANA_REG_5A, val);
    hal_sys_timer_delay(US_TO_TICKS(500));

#ifdef ANALOG_XTAL_ALWAYS_ON
    rf_read(RF_REG_527, &val);
    val |= REG_BT_BG_PU_DR | REG_BT_BG_PU;
    rf_write(RF_REG_527, val);

    rf_read(RF_REG_50F, &val);
    val |= REG_BT_XTAL_PU_DR | REG_BT_XTAL_PU;
    rf_write(RF_REG_50F, val);
#endif

    // Mdll drv bit 001 -> 010
    vtmp = 0x2;
    rf_read(RF_REG_517, &val);
    val = SET_BITFIELD(val, REG_XTAL_MDLL_OUTBUFFER_SWRC_I_CH22, vtmp);
    val = SET_BITFIELD(val, REG_XTAL_MDLL_OUTBUFFER_SWRC_I_CH46, vtmp);
    val = SET_BITFIELD(val, REG_XTAL_MDLL_OUTBUFFER_SWRC_I_CH70, vtmp);
    val = SET_BITFIELD(val, REG_XTAL_MDLL_OUTBUFFER_SWRC_I_OTHER, vtmp);
    rf_write(RF_REG_517, val);

    rf_read(RF_REG_544, &val);
    val = SET_BITFIELD(val, REG_XTAL_MDLL_DLL_SWRC, vtmp);
    val = SET_BITFIELD(val, REG_XTAL_MDLL_LPF_PRECH_BIAS, 0x0);
    rf_write(RF_REG_544, val);

    rf_read(RF_REG_543, &val);
    val |= REG_XTAL_MDLL_PFD_ERROR_DET_EN;
    rf_write(RF_REG_543, val);

    // Add 1us delay for mdll calib flag ready
    rf_read(RF_REG_524, &val);
    val = SET_BITFIELD(val, REG_XTAL_MDLL_CAL_WAIT_TIME, 0x10);
    rf_write(RF_REG_524, val);

    ldo_vtune = 0x2;
    rf_read(RF_REG_50C, &val);
    val = SET_BITFIELD(val, REG_XTAL_LDO_VTUNE_OTHER, ldo_vtune);
    rf_write(RF_REG_50C, val);

    rf_read(RF_REG_518, &val);
    val = SET_BITFIELD(val, REG_XTAL_LDO_VTUNE_CH22, ldo_vtune);
    val = SET_BITFIELD(val, REG_XTAL_LDO_VTUNE_CH46, ldo_vtune);
    val = SET_BITFIELD(val, REG_XTAL_LDO_VTUNE_CH70, ldo_vtune);
    rf_write(RF_REG_518, val);

    rf_read(RF_REG_50E, &val);
    val = SET_BITFIELD(val, REG_XTAL_MDLL_CP_SWRC, 0x2);
    rf_write(RF_REG_50E, val);

    rf_read(RF_REG_552, &val);
    val = SET_BITFIELD(val, REG_XTAL_SWRC_ACBUF_MAIN, 0x8);
    val = SET_BITFIELD(val, REG_XTAL_SWRC_ACBUF_SUB, 0x4);
    val = SET_BITFIELD(val, REG_XTAL_ACBUF_MAIN_SEL, 0x3);
    rf_write(RF_REG_552, val);

    rf_read(RF_REG_550, &val);
    val = SET_BITFIELD(val, REG_XTAL_MAIN_ICORE_SEL, 0x3);
    rf_write(RF_REG_550, val);
#endif
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

int pmu_get_sw_cfg_value(enum PMU_EFUSE_SW_CFG_T sw_cfg, unsigned short *cfg_val)
{
    int ret;

    unsigned short val = 0;

    ret = pmu_get_efuse(PMU_EFUSE_PAGE_SW_CFG, &val);
    if (ret) {
        // Error
        return ret;
    }

    // Efuse defaults to 0xF if not programmed
    if ((val & 0xF000) == 0) {
        val |= 0xF000;
    }

    if (sw_cfg == PMU_EFUSE_SRAM_CFG) {
        *cfg_val = GET_BITFIELD(val, REG_EFUSE_SRAM_CFG);
    } else if (sw_cfg == PMU_EFUSE_MIC_CFG) {
        *cfg_val = GET_BITFIELD(val, REG_EFUSE_MIC_CFG);
    } else if (sw_cfg == PMU_EFUSE_DUAL_CPU_CFG) {
        *cfg_val = GET_BITFIELD(val, REG_EFUSE_DUAL_CPU_CFG);
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
    unsigned short tmp[2];

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

    // Write Address
#ifdef PMU_EFUSE_NO_REDUNDANCY
    val |= REG_EFUSE_ADDRESS(page / 2);
#else
    val |= REG_EFUSE_ADDRESS(page); //redundancy
#endif
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        goto _exit;
    }

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

    // Read Efuse High 16 bits
    ret = pmu_read(PMU_REG_EFUSE_DATA_LOW, &tmp[0]);
    if (ret) {
        goto _exit;
    }

    // Read Efuse Low 16 bits
    ret = pmu_read(PMU_REG_EFUSE_DATA_HIGH, &tmp[1]);
    if (ret) {
        goto _exit;
    }
#ifdef PMU_EFUSE_NO_REDUNDANCY
    *efuse = tmp[page % 2];
#else
    *efuse = (tmp[0] | tmp[1]); //redundancy
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
    unsigned short chip_package_val = 0;

    for (i = 0; i < PMU_EFUSE_PAGE_QTY; i++) {
        pmu_get_efuse_phy(i, &pmu_efuse_page_arrays[i]);
    }

    chip_package_val = (pmu_efuse_page_arrays[PMU_EFUSE_PAGE_SW_CFG] >> REG_EFUSE_SRAM_CFG_SHIFT) & 0xF;

    switch (chip_package_val) {
        case 0x1:
            PMU_INFO_TRACE(0, "CHIP_PACKAGE_VER=1  BES2710IUC2-4X1");
            break;
        case 0xA:
            PMU_INFO_TRACE(0, "CHIP_PACKAGE_VER=2  BES2710IUC2-4X2");
            break;
        case 0xB:
            PMU_INFO_TRACE(0, "CHIP_PACKAGE_VER=3  BES2710IUC2-4X");
            break;
        case 0xF:
            PMU_INFO_TRACE(0, "CHIP_PACKAGE_VER=4  BES2710IUC3-4X");
            break;
        default:
            PMU_INFO_TRACE(0, "CHIP_PACKAGE_VER=0");
            break;
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

    PMU_INFO_TRACE_IMM(1, "Start pmu %s", shutdown ? "shutdown" : "reboot");

#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
    hal_cmu_sys_set_freq(HAL_CMU_FREQ_26M);
    pmu_voltage_resume();
#endif

    // Reset PMU
    pmu_write(PMU_REG_METAL_ID, 0xCAFE);
    pmu_write(PMU_REG_METAL_ID, 0x5FEE);
    hal_sys_timer_delay(US_TO_TICKS(500));

    // Enable auto load efuse
    pmu_write(PMU_REG_EFUSE_MSK, (REG_EFUSE_FLAG_TRIM_VBG_AUX | REG_EFUSE_FLAG_BUCK_VCORE_VBIT_NORMAL));
    hal_sys_timer_delay(US_TO_TICKS(100));

    pmu_read(PMU_REG_CHARGER_CFG, &val);
    val |= REG_PMU_CHARGE_INTR_EN;
    pmu_write(PMU_REG_CHARGER_CFG, val);

    // Reset CHARGER
    chg_write(CHG_REG_METAL_ID, 0xCAFE);
    chg_write(CHG_REG_METAL_ID, 0x5FEE);
    hal_sys_timer_delay(US_TO_TICKS(500));

#ifdef RTC_CALENDAR
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
        if (
#if defined(CHG_OUT_PWRON) || defined(RTC_CALENDAR)
            // ac_on will en lpo
            true ||
#endif
            false) {
            pmu_wdt_stop();
        } else {
            pmu_wdt_config(3*1000, 3*1000);
            pmu_wdt_start();
        }
#endif

        pmu_read(PMU_REG_USB_CFG_3F, &val);
#ifdef CHG_OUT_PWRON
        val |= AC_OUT_LDO_ON_EN;
#else
        val &= ~AC_OUT_LDO_ON_EN;
#endif
        pmu_write(PMU_REG_USB_CFG_3F, val);

#ifdef ACIN_PATTERN_DISABLE
        charger_charge_pattern_disable();
#endif

#ifdef CHG_FORCE_LP_PWROFF
        chg_read(CHG_REG_TEMP_BG_CFG, &val);
        val &= ~REG_BG_EN_OTP;
        chg_write(CHG_REG_TEMP_BG_CFG, val);

        val &= ~REG_BG_REF_GEN_EN;
        chg_write(CHG_REG_TEMP_BG_CFG, val);

        val &= ~REG_BG_CORE_EN;
        chg_write(CHG_REG_TEMP_BG_CFG, val);

        hal_sys_timer_delay(MS_TO_TICKS(1));
#endif

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

        pmu_read(PMU_REG_POWER_OFF, &val);
        if (val & SOFT_POWER_OFF) {
            PMU_INFO_TRACE_IMM(0, "\nError: SOFT_POWER_OFF=1!\n");
        }
        pmu_wdt_reboot();
        while (1) {}
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
    return PMU_REG_LDO_ANA_CFG;
}

static inline uint16_t pmu_get_module_volt_addr(enum PMU_MODULE_T module)
{
    return PMU_REG_LDO_ANA_VOLT;
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

    val |= module_cfg_p->lp_en_dr | module_cfg_p->lp_en;

    if (dpmode) {
        val |= module_cfg_p->dsleep_mode;
    } else {
        val &= ~module_cfg_p->dsleep_mode;
    }
    pmu_write(module_address, val);
}

void pmu_module_set_volt(unsigned char module, unsigned short sleep_v, unsigned short normal_v)
{
    unsigned short val;
    unsigned short module_volt_address;
    const struct PMU_MODULE_CFG_T *module_cfg_p = &pmu_module_cfg[module];

#ifdef PMU_LDO_VCORE_CALIB
    normal_v = pmu_reg_val_add(normal_v, pmu_ldo_ana_comp, MAX_LDO_VANA_BIT_VAL);
    sleep_v = pmu_reg_val_add(sleep_v, pmu_ldo_ana_comp, MAX_LDO_VANA_BIT_VAL);
#endif

    module_volt_address = pmu_get_module_volt_addr(module);

    pmu_read(module_volt_address, &val);
    val &= ~module_cfg_p->normal_v;
    val |= (normal_v << module_cfg_p->normal_v_shift) & module_cfg_p->normal_v;
    val &= ~module_cfg_p->dsleep_v;
    val |= (sleep_v << module_cfg_p->dsleep_v_shift) & module_cfg_p->dsleep_v;
    pmu_write(module_volt_address, val);

    if (normal_v > sleep_v) {
        ldo_ramp_map |= LDO_VANA_RAMP_EN;
    } else {
        ldo_ramp_map &= ~LDO_VANA_RAMP_EN;
    }
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

    if (sleep_vp) {
        *sleep_vp = (val & module_cfg_p->dsleep_v) >> module_cfg_p->dsleep_v_shift;
    }

#ifdef PMU_LDO_VCORE_CALIB
    if (normal_vp) {
        *normal_vp = pmu_reg_val_add(*normal_vp, -pmu_ldo_ana_comp, MAX_LDO_VANA_BIT_VAL);
    }
    if (sleep_vp) {
        *sleep_vp = pmu_reg_val_add(*sleep_vp, -pmu_ldo_ana_comp, MAX_LDO_VANA_BIT_VAL);
    }
#endif

    return 0;
}

static void POSSIBLY_UNUSED pmu_module_ramp_volt(unsigned char module, unsigned short sleep_v, unsigned short normal_v)
{
    uint16_t old_normal_v;
    uint16_t old_sleep_v;

    pmu_module_get_volt(module, &old_sleep_v, &old_normal_v);

    if (old_normal_v < normal_v) {
        while (old_normal_v++ < normal_v) {
            pmu_module_set_volt(module, sleep_v, old_normal_v);
        }
        hal_sys_timer_delay_us(PMU_VANA_STABLE_TIME_US);
    } else if (old_normal_v != normal_v || old_sleep_v != sleep_v) {
        pmu_module_set_volt(module, sleep_v, normal_v);
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
#ifndef PMU_INTF_CALIBED_VOLT
    if (!intf_calib_en) {
        if (normal_vp) {
            *normal_vp = pmu_reg_val_add(*normal_vp, -pmu_dcdc_dig_comp, MAX_BUCK_VCORE_BIT_VAL);
        }
    }
#endif
    if (dsleep_vp) {
        *dsleep_vp = pmu_reg_val_add(*dsleep_vp, -pmu_dcdc_dig_comp, MAX_BUCK_VCORE_BIT_VAL);
    }
#endif
}

static void pmu_dcdc_dig_set_volt(unsigned short normal_v,unsigned short dsleep_v)
{
    unsigned short val;

#ifdef PMU_DCDC_CALIB
#ifndef PMU_INTF_CALIBED_VOLT
    if (!intf_calib_en) {
        normal_v = pmu_reg_val_add(normal_v, pmu_dcdc_dig_comp, MAX_BUCK_VCORE_BIT_VAL);
    }
#endif
    dsleep_v = pmu_reg_val_add(dsleep_v, pmu_dcdc_dig_comp, MAX_BUCK_VCORE_BIT_VAL);
#endif

    pmu_read(PMU_REG_BUCK_VCORE_CFG, &val);
    val = SET_BITFIELD(val, REG_BUCK_VCORE_VBIT_NORMAL, normal_v);
    pmu_write(PMU_REG_BUCK_VCORE_CFG, val);

    pmu_read(PMU_REG_BUCK_VCORE_LP_CFG, &val);
    val = SET_BITFIELD(val, REG_BUCK_VCORE_VBIT_DSLEEP, dsleep_v);
    pmu_write(PMU_REG_BUCK_VCORE_LP_CFG, val);

    if (!dcdc_ramp_en) {
#ifdef PMU_DCDC_CALIB
        if (intf_calib_en) {
            // Get real vbit
            normal_v = pmu_reg_val_add(normal_v, pmu_dcdc_dig_comp, MAX_BUCK_VCORE_BIT_VAL);
        }
#endif
        if (normal_v > dsleep_v) {
            dcdc_ramp_map |= REG_DCDC1_RAMP_EN;
        } else {
            dcdc_ramp_map &= ~REG_DCDC1_RAMP_EN;
        }
    }
}

#ifdef MTEST_ENABLED
bool pmu_dig_set_volt_value(uint16_t dcdc_volt_mv, uint16_t logic_volt_mv, uint16_t ldo_volt_mv)
{
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
    };

    uint16_t dcdc_volt;
    uint32_t i, find = 0;
    bool success = false;

    for (i = 0; i < ARRAY_SIZE(dcdc_volt_list); i++) {
        if (dcdc_volt_mv == dcdc_volt_list[i][0]) {
            dcdc_volt = dcdc_volt_list[i][1];
            find |= 0x1;
            break;
        }
    }
    if (find == 0x1) {
        pmu_dcdc_dig_set_volt(dcdc_volt, dig_lp_dcdc);
        success = true;
    }
    return success;
}
#endif /* MTEST_ENABLED */

static void BOOT_TEXT_SRAM_LOC pmu_dig_get_target_volt(uint16_t *dcdc)
{
    uint16_t dcdc_volt;

    if (0) {
    } else if (pmu_vcore_req & PMU_VCORE_PMU_RESET) {
        dcdc_volt = PMU_DCDC_DIG_DEFAULT;
    } else if (pmu_vcore_req & PMU_VCORE_SYS_FREQ_MEDIUM) {
        dcdc_volt = PMU_DCDC_DIG_0_9V + vmin_hf_offset;
    } else if (pmu_vcore_req & PMU_VCORE_SYS_FREQ_MEDIUM_LOW) {
        dcdc_volt = PMU_DCDC_DIG_0_86V + vmin_hf_offset;
    } else if (pmu_vcore_req & PMU_VCORE_SYS_FREQ_LOW) {
        dcdc_volt = PMU_DCDC_DIG_0_8V + vmin_offset;
    } else {
        // Common cases
        dcdc_volt = PMU_DCDC_DIG_0_78V + vmin_offset;
    }

#ifdef PMU_VCORE_RISE_MV
    dcdc_volt += (PMU_DCDC_DIG_0_8V - PMU_DCDC_DIG_0_75V) * (PMU_VCORE_RISE_MV / 25) / 2;
#endif

#if (defined(PROGRAMMER) || defined(__BES_OTA_MODE__)) && !defined(PMU_FULL_INIT)
    // Try to keep the same vcore voltage as ROM (hardware default)
    if (dcdc_volt < PMU_DCDC_DIG_DEFAULT) {
        dcdc_volt = PMU_DCDC_DIG_DEFAULT;
    }
#endif

    if (dcdc) {
        *dcdc = dcdc_volt;
    }
}

static void pmu_dig_set_volt(enum PMU_POWER_MODE_T mode)
{
    uint32_t lock;
    uint32_t delay_us;
    uint16_t dcdc_volt, old_act_dcdc, old_lp_dcdc;
    bool volt_inc = false;

    lock = int_lock();

    pmu_dig_get_target_volt(&dcdc_volt);
    pmu_dcdc_dig_get_volt(&old_act_dcdc, &old_lp_dcdc);

    if (old_act_dcdc < dcdc_volt) {
        volt_inc = true;
        if (dcdc_ramp_en) {
            pmu_dcdc_dig_set_volt(dcdc_volt, dig_lp_dcdc);
        } else {
            while (old_act_dcdc++ < dcdc_volt) {
                pmu_dcdc_dig_set_volt(old_act_dcdc, dig_lp_dcdc);
            }
        }
    } else {
        pmu_dcdc_dig_set_volt(dcdc_volt, dig_lp_dcdc);
    }

    if (volt_inc) {
        if (dcdc_ramp_en) {
            delay_us = (dcdc_volt - old_act_dcdc) / PMU_DCDC_HW_RAMP_1_CLK_STEP * PMU_DCDC_HW_RAMP_1_CLK_TIME_US + 100;
            hal_sys_timer_delay_us(delay_us);
        } else {
            hal_sys_timer_delay_us(PMU_VCORE_STABLE_TIME_US);
        }
    }

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

    pmu_read(PMU_REG_BUCK_VHPPA_EN, &val);
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
        *dsleep_vp = pmu_reg_val_add(*dsleep_vp, -pmu_dcdc_hppa_comp, MAX_BUCK_VHPPA_BIT_VAL);
    }
#endif
}

static void pmu_dcdc_hppa_set_volt(unsigned short normal_v,unsigned short dsleep_v)
{
    unsigned short val;

#ifdef PMU_DCDC_CALIB
    normal_v = pmu_reg_val_add(normal_v, pmu_dcdc_hppa_comp, MAX_BUCK_VHPPA_BIT_VAL);
    dsleep_v = pmu_reg_val_add(dsleep_v, pmu_dcdc_hppa_comp, MAX_BUCK_VHPPA_BIT_VAL);
#endif

    pmu_read(PMU_REG_BUCK_VHPPA_EN, &val);
    val = SET_BITFIELD(val, REG_BUCK_VHPPA_VBIT_NORMAL, normal_v);
    pmu_write(PMU_REG_BUCK_VHPPA_EN, val);

    pmu_read(PMU_REG_BUCK_VHPPA_LP_CFG, &val);
    val = SET_BITFIELD(val, REG_BUCK_VHPPA_VBIT_DSLEEP, dsleep_v);
    pmu_write(PMU_REG_BUCK_VHPPA_LP_CFG, val);

    if (!dcdc_ramp_en) {
        if (normal_v > dsleep_v) {
            dcdc_ramp_map |= REG_DCDC3_RAMP_EN;
        } else {
            dcdc_ramp_map &= ~REG_DCDC3_RAMP_EN;
        }
    }
}

static void pmu_hppa_set_volt(uint16_t volt)
{
    uint16_t new_act_dcdc;
    uint16_t old_act_dcdc;
    uint16_t old_lp_dcdc;

    new_act_dcdc = volt;

    pmu_dcdc_hppa_get_volt(&old_act_dcdc, &old_lp_dcdc);
    if (old_act_dcdc < new_act_dcdc) {
        if (dcdc_ramp_en) {
            pmu_dcdc_hppa_set_volt(new_act_dcdc, new_act_dcdc);
        } else {
            while (old_act_dcdc++ < new_act_dcdc) {
                pmu_dcdc_hppa_set_volt(old_act_dcdc, new_act_dcdc);
            }
        }
        if (dcdc_ramp_en) {
            uint32_t delay_us;

            delay_us = (new_act_dcdc - old_act_dcdc) / PMU_DCDC_HW_RAMP_1_CLK_STEP_HALF * PMU_DCDC_HW_RAMP_1_CLK_TIME_US + 100;
            hal_sys_timer_delay_us(delay_us);
        } else {
            hal_sys_timer_delay_us(PMU_VANA_STABLE_TIME_US);
        }
    } else if (old_act_dcdc != new_act_dcdc || old_lp_dcdc != new_act_dcdc) {
        pmu_dcdc_hppa_set_volt(new_act_dcdc, new_act_dcdc);
    }
}

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
    uint16_t tgt_volt;
    uint16_t old_volt;
    uint16_t val;

    pmu_dig_get_target_volt(&tgt_volt);

    pmu_read(PMU_REG_BUCK_VCORE_CFG, &val);
    old_volt = GET_BITFIELD(val, REG_BUCK_VCORE_VBIT_NORMAL);

    if (old_volt < tgt_volt) {
        while (old_volt++ < tgt_volt) {
            val = SET_BITFIELD(val, REG_BUCK_VCORE_VBIT_NORMAL, old_volt);
            pmu_write(PMU_REG_BUCK_VCORE_CFG, val);
        }
        hal_sys_timer_delay_us(PMU_VCORE_STABLE_TIME_US);
    }
}

static void pmu_voltage_resume(void)
{
    pmu_hppa_set_volt(PMU_DCDC_HPPA_DEFAULT);

    pmu_module_ramp_volt(PMU_ANA, PMU_ANA_DEFAULT, PMU_ANA_DEFAULT);

    pmu_vcore_req |= PMU_VCORE_PMU_RESET;
    pmu_dig_set_volt(PMU_POWER_MODE_NONE);
    hal_sys_timer_delay(MS_TO_TICKS(1));
}

#ifdef PMU_REG_CFG_DUMP
static void pmu_reg_val_print(void)
{
    uint16_t val;

    PMU_INFO_TRACE(1, "%s start", __func__);
    for (uint16_t reg = 0x00; reg <= 0xFD; reg++) {
        pmu_read(reg, &val);
        PMU_INFO_TRACE_IMM(2, "reg=val 0x%04X=0x%04X", reg, val);
    }

    for (uint16_t reg = 0x10A; reg <= 0x158; reg++) {
        pmu_read(reg, &val);
        PMU_INFO_TRACE_IMM(2, "reg=val 0x%04X=0x%04X", reg, val);
    }

    for (uint16_t reg = PMU_EFUSE_PAGE_SECURITY; reg < PMU_EFUSE_PAGE_QTY; reg++) {
        pmu_get_efuse(reg, &val);
        PMU_INFO_TRACE_IMM(2, "efuse=val 0x%04X=0x%04X", reg, val);
    }
    PMU_INFO_TRACE(1, "%s done", __func__);
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

    PMU_INFO_TRACE(2, "%s reg_addr=0x%x", __func__, PMU_VMIN_SET_REG_ADDR);

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

    PMU_INFO_TRACE(3, "%s vmin_offset%d pmu_dcdc_dig_comp=%d",
                    __func__, vmin_offset, pmu_dcdc_dig_comp);
}
#endif

// Target voltage 1.8V
static void pmu_set_sar_adc_vref(void)
{
    // TODO:
}
#endif

static void pmu_buck_volt_hw_ramp_enable(uint8_t map)
{
    uint16_t val;

    if (dcdc_ramp_map & REG_DCDC1_RAMP_EN) {
        pmu_read(PMU_REG_BUCK_VCORE_EN, &val);
        val |= REG_BUCK_VCORE_RAMP_EN;
        pmu_write(PMU_REG_BUCK_VCORE_EN, val);
    }

    if (dcdc_ramp_map & REG_DCDC3_RAMP_EN) {
        pmu_read(PMU_REG_BUCK_VHPPA_RAMP_CFG, &val);
        val |= REG_BUCK_VHPPA_RAMP_EN;
        pmu_write(PMU_REG_BUCK_VHPPA_RAMP_CFG, val);
    }
}

static void pmu_buck_volt_hw_ramp_disable(uint8_t map)
{
    uint16_t val;

    if (dcdc_ramp_map & REG_DCDC1_RAMP_EN) {
        pmu_read(PMU_REG_BUCK_VCORE_EN, &val);
        val &= ~REG_BUCK_VCORE_RAMP_EN;
        pmu_write(PMU_REG_BUCK_VCORE_EN, val);
    }

    if (dcdc_ramp_map & REG_DCDC3_RAMP_EN) {
        pmu_read(PMU_REG_BUCK_VHPPA_RAMP_CFG, &val);
        val &= ~REG_BUCK_VHPPA_RAMP_EN;
        pmu_write(PMU_REG_BUCK_VHPPA_RAMP_CFG, val);
    }
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

POSSIBLY_UNUSED static BOOT_BSS_LOC bool pmu_opened = false;

int BOOT_TEXT_FLASH_LOC pmu_open(void)
{
#ifdef NO_ISPI
    return 0;
#endif

#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
    uint16_t val, val_efuse, val_cal;

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
    PMU_INFO_TRACE(3, "%s vcodec_mv:%d vhppa_mv:%d", __func__, vcodec_mv, vhppa_mv);

#if !defined(ROM_BUILD) && defined(PMU_EFUSE_INIT_IN_BOOT)
    pmu_efuse_init();
#endif

    pmu_get_efuse(PMU_EFUSE_PAGE_EXT_GPADC_HV, &val);
    val_cal = GET_BITFIELD(val, PMU_EFUSE_VMIN_MAP);
    if (val_cal == 1) {
        vmin_offset = PMU_DCDC_DIG_20_MV;
    } else if (val_cal == 2) {
        vmin_offset = PMU_DCDC_DIG_40_MV;
    } else if (val_cal == 3) {
        vmin_offset = PMU_DCDC_DIG_60_MV;
    }
    val_cal = GET_BITFIELD(val, PMU_EFUSE_VMIN_HF_MAP);
    if (val_cal == 1) {
        vmin_hf_offset = PMU_DCDC_DIG_20_MV;
    } else if (val_cal == 2) {
        vmin_hf_offset = PMU_DCDC_DIG_40_MV;
    } else if (val_cal == 3) {
        vmin_hf_offset = PMU_DCDC_DIG_60_MV;
    }
    PMU_INFO_TRACE(0, "%s: vmin_offset=0x%x vmin_hf_offset=0x%x efuse=0x%x", __func__, vmin_offset, vmin_hf_offset, val);

#ifdef PMU_WAFER_PRINT
    pmu_get_efuse(REG_CP_WAFER_CTX_ID, &val);
    PMU_INFO_TRACE(1, "wafer_ctx_print: wafer_ID=%d", GET_BITFIELD(val, PMU_EFUSE_CP_WAFER_CTX_ID));

    pmu_get_efuse(REG_CP_WAFER_CTX_XY, &val);
    PMU_INFO_TRACE(2, "wafer_ctx_print: wafer_X=%d, wafer_Y=%d",
        GET_BITFIELD(val, PMU_EFUSE_CP_WAFER_CTX_X), GET_BITFIELD(val, PMU_EFUSE_CP_WAFER_CTX_Y));
#endif

    PMU_INFO_TRACE(3, "%s boot_cause=0x%04X first_pwron:%d", __func__, pmu_boot_reason, pmu_boot_first_pwr_up);

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

#ifdef PMU_IRQ_UNIFIED
    pmu_read(PMU_REG_WDT_CFG, &val);
    val |= POWERON_DETECT_EN | MERGE_INTR;
    pmu_write(PMU_REG_WDT_CFG, val);
#endif

    // Disable sar vref output
    pmu_sar_adc_vref_sw_pu(false);
    pmu_set_sar_adc_vref();

    // Set sar adc config
    // reg_sar_adc_offset[15:0]
    pmu_write(PMU_REG_SAR_ADC_OFFSET_CFG, 0x7C01);

    pmu_read(PMU_REG_SAR_CFG_8C, &val);
    val |= REG_SAR_WEIGHT_DR | REG_SAR_ADC_OFFSET_DR;
    pmu_write(PMU_REG_SAR_CFG_8C, val);

    pmu_read(PMU_REG_SAR_CFG_91, &val);
    val |= REG_SAR_DONE_DLY_SEL;
    val = SET_BITFIELD(val, REG_SAR_THERM_GAIN_IN, 0x2);
    pmu_write(PMU_REG_SAR_CFG_91, val);

    pmu_sar_input_buf_enable(false);

    // gpadc_dig 2m
    pmu_read(PMU_REG_GPADC_CLK_CH_EN_CFG, &val);
    val = SET_BITFIELD(val, REG_CLK_GPADC_DIV_VALUE, 0xA);
    pmu_write(PMU_REG_GPADC_CLK_CH_EN_CFG, val);

    // gpadc_analog 100k
    pmu_read(PMU_REG_SAR_CFG_92, &val);
    val = SET_BITFIELD(val, REG_SAR_CLK_OUT_DIV, 0x12);
    pmu_write(PMU_REG_SAR_CFG_92, val);

    pmu_read(PMU_REG_BUCK_VCORE_EN, &val);
#ifdef DCDC_VOLT_HW_RAMP_ONLY
    val |= REG_BUCK_VCORE_RAMP_EN;
#else
    val &= ~REG_BUCK_VCORE_RAMP_EN;
#endif
    val = SET_BITFIELD(val, REG_BUCK_VCORE_RAMP_STEP, PMU_DCDC_HW_RAMP_1_CLK_STEP);
    pmu_write(PMU_REG_BUCK_VCORE_EN, val);

    pmu_read(PMU_REG_BUCK_VHPPA_RAMP_CFG, &val);
#ifdef DCDC_VOLT_HW_RAMP_ONLY
    val |= REG_BUCK_VHPPA_RAMP_EN;
#else
    val &= ~REG_BUCK_VHPPA_RAMP_EN;
#endif
    val = SET_BITFIELD(val, REG_BUCK_VHPPA_RAMP_STEP, PMU_DCDC_HW_RAMP_1_CLK_STEP_HALF);
    pmu_write(PMU_REG_BUCK_VHPPA_RAMP_CFG, val);

#if defined(PMU_DCDC_CALIB) && !defined(PMU_ALREADY_INIT)
    pmu_get_dcdc_calib_value();
    PMU_INFO_TRACE(2, "dcdc_cal: normal=lp vcore=%d vcodec=%d", pmu_dcdc_dig_comp, pmu_dcdc_hppa_comp);
#endif

#if defined(PMU_LDO_VCORE_CALIB) && !defined(PMU_ALREADY_INIT)
    pmu_get_ldo_dig_calib_value();
    PMU_INFO_TRACE(1, "ldo_cal: normal=lp vana=%d", pmu_ldo_ana_comp);
#endif

#ifdef PMU_VMIN_SEARCH
    pmu_vmin_search_enable();
#endif

#ifndef NO_SLEEP
    pmu_sleep_en(1);  //enable sleep
#endif

    pmu_get_efuse(PMU_EFUSE_PAGE_EXT_GPADC_LV, &val_efuse);
    val_efuse = GET_BITFIELD(val_efuse, PMU_EFUSE_DCDC_CFG_TYPE);
    PMU_INFO_TRACE(0, "dcdc_cal_type=0x%x", val_efuse);
    if (val_efuse == 1 || val_efuse == 0) {
        pmu_read(PMU_REG_BUCK_BUCK_CFG_13A, &val);
        val = SET_BITFIELD(val, REG_BUCK_INTERNAL_FREQUENCY, 0x0);
        val &= ~REG_BUCK_SLOPE_DOUBLE;
        pmu_write(PMU_REG_BUCK_BUCK_CFG_13A, val);
    } else if (val_efuse == 2) {
        pmu_read(PMU_REG_BUCK_BUCK_CFG_13A, &val);
        val = SET_BITFIELD(val, REG_BUCK_INTERNAL_FREQUENCY, 0x1);
        val |= REG_BUCK_SLOPE_DOUBLE;
        pmu_write(PMU_REG_BUCK_BUCK_CFG_13A, val);
    }

    pmu_read(PMU_REG_BUCK_VCORE_CFG, &val);
    val = SET_BITFIELD(val, REG_BUCK_IS_GAIN_NORMAL, 0x7);
    pmu_write(PMU_REG_BUCK_VCORE_CFG, val);

    pmu_read(PMU_REG_BUCK_VCORE_RC_CFG, &val);
    val = SET_BITFIELD(val, REG_BUCK_IS_GAIN_RC, 0x7);
    pmu_write(PMU_REG_BUCK_VCORE_RC_CFG, val);

    pmu_read(PMU_REG_BUCK_VCORE_LP_CFG, &val);
    val = SET_BITFIELD(val, REG_BUCK_IS_GAIN_DSLEEP, 0x7);
    pmu_write(PMU_REG_BUCK_VCORE_LP_CFG, val);

    pmu_read(PMU_REG_BUCK_BUCK_CFG_138, &val);
    val = SET_BITFIELD(val, REG_BUCK_CC_CAP_BIT, 0xA);
    pmu_write(PMU_REG_BUCK_BUCK_CFG_138, val);

    pmu_read(PMU_REG_BUCK_BUCK_CFG_139, &val);
    val = SET_BITFIELD(val, REG_BUCK_BURST_THRESHOLD, 0x38);
    pmu_write(PMU_REG_BUCK_BUCK_CFG_139, val);

#ifndef PMU_DCDC_PWM
    pmu_read(PMU_REG_BUCK_VHPPA_EN, &val);
    val |= REG_BUCK_BURST_MODE_SEL_NORMAL;
    pmu_write(PMU_REG_BUCK_VHPPA_EN, val);
#endif

#ifndef PROGRAMMER
    // Disable usbphy power, and vusb if possible
    pmu_usb_config(PMU_USB_CONFIG_TYPE_NONE);
#endif

    pmu_codec_mic_bias_set_volt(AUD_VMIC_MAP_VMIC1 | AUD_VMIC_MAP_VMIC2, 2200);

    pmu_read(PMU_REG_MIC_PULL_DOWN, &val);
    val |= REG_MIC_LP_ENABLE;
    pmu_write(PMU_REG_MIC_PULL_DOWN, val);

    val = pmu_dcdc_hppa_mv_to_val(vhppa_mv);
    pmu_hppa_set_volt(val);

    pmu_module_ramp_volt(PMU_ANA, ana_act_ldo, ana_act_ldo);

    pmu_dig_set_volt(PMU_POWER_MODE_NONE);

#if defined(MCU_HIGH_PERFORMANCE_MODE)
#error "Not support MCU_HIGH_PERFORMANCE_MODE!"
#endif

    charger_charge_open();

#ifdef ACIN_PATTERN_DISABLE
    pmu_charger_pattern_enable(false);
#else
    pmu_charger_pattern_enable(true);
#endif

#ifdef SYS_RC_ENABLE
    pmu_rc_clk_enable(PMU_CLK_SEL_RC_USER_SYS);
#endif

#ifdef PMU_NTC_MONITOR
    pmu_ntc_monitor_init();
#endif

    // Set MIC_LDO_EN always on
    // MIC_LDO_EN switch will affect bandgap
    pmu_read(PMU_REG_MIC_BIAS_B, &val);
    val |= REG_MIC_LDO_EN;
    pmu_write(PMU_REG_MIC_BIAS_B, val);
    hal_sys_timer_delay_us(PMU_LDO_PU_STABLE_TIME_US);

    pmu_read(PMU_REG_LED_CFG_IO2, &val);
    // Sel vmem
    val = SET_BITFIELD(val, REG_LED_IO2_SEL, 0x1);
    // 36ohm
    val = SET_BITFIELD(val, REG_LED_IO2_IBIT, 0x0);
    pmu_write(PMU_REG_LED_CFG_IO2, val);

    // If pmu shutdown abnormally
    // keeping the LPO always on can ensure the effectiveness of the PMU WDT and pattern reset.
    pmu_read(PMU_REG_POWER_KEY_CFG, &val);
    val |= PU_LPO_DR | PU_LPO_REG;
    pmu_write(PMU_REG_POWER_KEY_CFG, val);

#endif // PMU_INIT || (!FPGA && !PROGRAMMER)

#ifdef RTC_CALENDAR
    pmu_rtc_calendar_init();
#endif

#ifdef PMU_REG_CFG_DUMP
    pmu_reg_val_print();
#endif

    return 0;
}

void pmu_big_bandgap_enable(enum PMU_BIG_BANDGAP_USER_T user, int enable)
{
}

void pmu_sleep(void)
{
    uint16_t val;

    if (!dcdc_ramp_en) {
        // Enable DCDC ramp
        pmu_buck_volt_hw_ramp_enable(dcdc_ramp_map);
    }

    if (ldo_ramp_map & LDO_VANA_RAMP_EN) {
        pmu_read(PMU_REG_LDO_ANA_CFG, &val);
        val |= REG_LDO_VANA_RAMP_EN;
        pmu_write(PMU_REG_LDO_ANA_CFG, val);
    }
}

void pmu_wakeup(void)
{
    uint16_t val;

    if (!dcdc_ramp_en) {
        pmu_buck_volt_hw_ramp_disable(dcdc_ramp_map);
    }

    if (ldo_ramp_map & LDO_VANA_RAMP_EN) {
        pmu_read(PMU_REG_LDO_ANA_CFG, &val);
        val &= ~REG_LDO_VANA_RAMP_EN;
        pmu_write(PMU_REG_LDO_ANA_CFG, val);
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
        volt = PMU_VMIC_1_6V + (mv - 1600 + step / 2) / step;
        if (volt > PMU_VMIC_2_8V) {
            volt = PMU_VMIC_2_8V;
        }
    }

    if (vcodec_mv > 1700) {
        volt -= 2;
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
        pmu_read(PMU_REG_MIC_BIAS_A, &val);
        val |= REG_MIC_LDO_LOOPCTRL;
        pmu_write(PMU_REG_MIC_BIAS_A, val);

        reg_mic_ldo_en = true;
    } else if (need_ldo_on_cnt == 0) {
        pmu_read(PMU_REG_MIC_BIAS_A, &val);
        val &= ~REG_MIC_LDO_LOOPCTRL;
        pmu_write(PMU_REG_MIC_BIAS_A, val);
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

    if (!pmu_opened) {
        // PMU not init yet
        pmu_dig_init_volt();
        return;
    }

    pmu_dig_set_volt(PMU_POWER_MODE_NONE);
#endif
}

void BOOT_TEXT_FLASH_LOC pmu_psram_freq_config(uint32_t freq)
{
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

    lock = int_lock();
    old_req = pmu_vcore_req;
    pmu_vcore_req &= ~(PMU_VCORE_SYS_FREQ_HIGH | PMU_VCORE_SYS_FREQ_MEDIUM | PMU_VCORE_SYS_FREQ_MEDIUM_LOW | PMU_VCORE_SYS_FREQ_LOW);
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
    if (freq > HAL_CMU_FREQ_144M) {
        // 192m
        pmu_vcore_req |= PMU_VCORE_SYS_FREQ_MEDIUM;
    } else if (freq > HAL_CMU_FREQ_104M) {
        // 144m
        pmu_vcore_req |= PMU_VCORE_SYS_FREQ_MEDIUM_LOW;
    } else if (freq > HAL_CMU_FREQ_52M) {
        // 96m
        pmu_vcore_req |= PMU_VCORE_SYS_FREQ_LOW;
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

    if (!pmu_opened) {
        // PMU not init yet
        pmu_dig_init_volt();
        return;
    }

    pmu_dig_set_volt(PMU_POWER_MODE_NONE);
#endif
}

void pmu_bt_rf_temperature_config(int enable)
{
    uint32_t lock;

    lock = int_lock();
    if (enable) {
        pmu_vcore_req |= PMU_VCORE_BT_RF;
    } else {
        pmu_vcore_req &= ~PMU_VCORE_BT_RF;
    }
    int_unlock(lock);

    pmu_dig_set_volt(PMU_POWER_MODE_NONE);
}

void pmu_high_performance_mode_enable(bool enable)
{
}

void pmu_usb_config(enum PMU_USB_CONFIG_TYPE_T type)
{
}

int pmu_usb_config_pin_status_check(enum PMU_USB_PIN_CHK_STATUS_T status, PMU_USB_PIN_CHK_CALLBACK callback, int enable)
{
    return 0;
}

void pmu_usb_enable_pin_status_check(void)
{
}

void pmu_usb_disable_pin_status_check(void)
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

    PMU_INFO_TRACE(2, "wdt_load: %u -> %u", !!(val0 & WDT_LOAD), !!(val1 & WDT_LOAD));

    if ((val0 & WDT_LOAD) == (val1 & WDT_LOAD)) {
        PMU_INFO_TRACE(0, "BAD: wdt load not changed");
        return 1;
    } else {
        PMU_INFO_TRACE(0, "GOOD: wdt load changed");
        return 0;
    }
}

void pmu_bt_reconn(bool en)
{
    uint16_t pmu_val = 0;

    pmu_read(0x38, &pmu_val);
    //PMU_INFO_TRACE(1,"origin: PMU_38=0x%x", pmu_val);
    pmu_val &= 0xfff0;      //clear [3:0]

    if(en){
        pmu_val |= 0xf;
    }
    pmu_write(0x38, pmu_val);

    //pmu_read(0x38, &pmu_val);
    //PMU_INFO_TRACE(1,"final: PMU_38=0x%x", pmu_val);
}

void pmu_bt_2v5pa(bool en)
{
}

void pmu_vio_3v3(bool en)
{
}

void pmu_sar_adc_vref_enable(void)
{
    uint16_t val;

    // force enable
    pmu_read(PMU_REG_AVDD_EN, &val);
    val |= REG_PU_AVDD25_ANA;
    pmu_write(PMU_REG_AVDD_EN, val);

    pmu_read(PMU_REG_SAR_PU_CFG, &val);
    val |= REG_SAR_PU_VREF_REG | REG_SAR_PU_VREF_DR | REG_SAR_PU_REG | REG_SAR_PU_DR;
    pmu_write(PMU_REG_SAR_PU_CFG, val);

    // sar vref output
    pmu_sar_adc_vref_sw_pu(true);
}

void pmu_sar_adc_vref_disable(void)
{
    uint16_t val;

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
        pmu_read(PMU_REG_IPTAT_CORE_I_CFG, &val);
        val |= DIG_IPTAT_CORE_EN;
        pmu_write(PMU_REG_IPTAT_CORE_I_CFG, val);
    } else {
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

static int pmu_vchgr_div_ctrl(int enable)
{
    uint32_t lock;
    uint16_t val;

    lock = int_lock();
    pmu_read(PMU_REG_ANA_CFG, &val);
    if (enable) {
        val |= REG_PU_ACIN_SENSE;
    } else {
        val &= ~REG_PU_ACIN_SENSE;
    }
    pmu_write(PMU_REG_ANA_CFG, val);
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
    } else if (channel == HAL_GPADC_CHAN_5) {
        ret = pmu_vchgr_div_ctrl(enable);
    }

    return ret;
}

int pmu_volt2temperature(const uint16_t volt)
{
    const int Kvtherm_T = 348;
    static uint16_t temp_ref_raw2volt = 0;
    int temp_centigrade = 0;

    if (!temp_ref_raw2volt) {
        bool calibed = true;

        pmu_get_efuse(PMU_EFUSE_PAGE_TEMPERATURE, &temp_ref_raw2volt);
        if (!temp_ref_raw2volt) {
            temp_ref_raw2volt = PMU_EFUSE_TEMPSENSOR_DEFAULT;
            calibed = false;
        }
        temp_ref_raw2volt = hal_gpadc_adc2volt(temp_ref_raw2volt, HAL_GPADC_CHAN_0);
        PMU_INFO_TRACE(2, "%s calibed=%d temp_ref_raw2volt=%d", __func__, calibed, temp_ref_raw2volt);
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
    if (!ntc_irq_busy && ntc_ctx.monitor_interval_ms) {
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
    pmu_write(PMU_REG_DIV_HW_RESET_CFG, CONFIG_SYSTICK_HZ / sample_clk_freq_hz * 2 - 2);
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

#ifdef RC_CLK_ENABLE
static void pmu_rc_clock_dr_cfg(bool en)
{
}

void pmu_rc_clk_enable(enum PMU_CLK_SEL_RC_USER_T user)
{
}

void pmu_rc_clk_disable(enum PMU_CLK_SEL_RC_USER_T user)
{
}

int pmu_rc_clk_calib(void)
{
    return 0;
}
#endif

#ifdef GPADC_CUSTOM_CALIB_VAL
void pmu_get_vbat_calib_value(unsigned short *val_lv, unsigned short *val_hv)
{
    pmu_get_efuse(PMU_EFUSE_PAGE_BATTER_LV, val_lv);
    pmu_get_efuse(PMU_EFUSE_PAGE_BATTER_HV, val_hv);
    *val_lv = GET_BITFIELD(*val_lv, PMU_EFUSE_PAGE_BATTER_LV_OFFSET);
    *val_hv = GET_BITFIELD(*val_hv, PMU_EFUSE_PAGE_BATTER_HV_OFFSET);

    PMU_INFO_TRACE(3, "%s, efuse_lv/hv:%d/%d", __func__, *val_lv, *val_hv);

    if (*val_lv == 0 || *val_hv == 0) {
        *val_lv = PMU_EFUSE_GPADC_CALIB_DEFAULT;
        *val_hv = PMU_EFUSE_GPADC_CALIB_DEFAULT;
    }

    *val_lv += PMU_EFUSE_PAGE_BATTER_LV_BASE;
    *val_hv += PMU_EFUSE_PAGE_BATTER_HV_BASE;
}

#ifdef GPADC_HAS_EXT_SLOPE_CAL
void pmu_get_ext_gpadc_calib_value(unsigned short *val_lv, unsigned short *val_hv)
{
    pmu_get_efuse(PMU_EFUSE_PAGE_EXT_GPADC_LV, val_lv);
    pmu_get_efuse(PMU_EFUSE_PAGE_EXT_GPADC_HV, val_hv);
    *val_lv = GET_BITFIELD(*val_lv, PMU_EFUSE_PAGE_EXT_GPADC_LV_OFFSET);
    *val_hv = GET_BITFIELD(*val_hv, PMU_EFUSE_PAGE_EXT_GPADC_HV_OFFSET);

    PMU_INFO_TRACE(3, "%s, efuse_lv/hv:%d/%d", __func__, *val_lv, *val_hv);

    if (*val_lv == 0 || *val_hv == 0) {
        *val_lv = PMU_EFUSE_GPADC_CALIB_DEFAULT;
        *val_hv = PMU_EFUSE_GPADC_CALIB_DEFAULT;
    }

    *val_lv += PMU_EFUSE_PAGE_EXT_GPADC_LV_BASE;
    *val_hv += PMU_EFUSE_PAGE_EXT_GPADC_HV_BASE;
}
#endif
#endif

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

int pmu_get_acin_volt(uint16_t *volt)
{
    uint32_t sum_val = 0;
    uint16_t v;
    int valid_cnt, nRet;
    const enum HAL_GPADC_CHAN_T ch = HAL_GPADC_CHAN_5;
    const uint8_t div = 5;
    const uint8_t db_cnt = 3;

    valid_cnt = 0;

    hal_gpadc_open(ch, HAL_GPADC_ATP_125US, NULL);

    for (int i = 0; i < db_cnt; i++) {
        if (hal_gpadc_get_volt_blocked(ch, &v)) {
            sum_val += v;
            valid_cnt++;
        }
    }

    hal_gpadc_close(ch);

    if (valid_cnt) {
        *volt = (sum_val / valid_cnt) * div;
        nRet = 0;
    } else {
        nRet = -1;
    }

    return nRet;
}
