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
#ifndef __REG_ANALOG_BEST1503P_H__
#define __REG_ANALOG_BEST1503P_H__

#include "plat_types.h"

enum ANA_REG_T {
    ANA_REG_00 = 0x00,
    ANA_REG_01,
    ANA_REG_02,
    ANA_REG_03,
    ANA_REG_04,
    ANA_REG_05,
    ANA_REG_06,
    ANA_REG_07,
    ANA_REG_08,
    ANA_REG_09,
    ANA_REG_0A,
    ANA_REG_0B,
    ANA_REG_0C,
    ANA_REG_0D,
    ANA_REG_0E,
    ANA_REG_0F,
    ANA_REG_10,
    ANA_REG_11,
    ANA_REG_12,
    ANA_REG_13,
    ANA_REG_14,
    ANA_REG_15,
    ANA_REG_16,
    ANA_REG_17,
    ANA_REG_18,
    ANA_REG_19,
    ANA_REG_1A,
    ANA_REG_1B,
    ANA_REG_1C,
    ANA_REG_1D,
    ANA_REG_1E,
    ANA_REG_1F,
    ANA_REG_20,
    ANA_REG_21,
    ANA_REG_22,
    ANA_REG_23,
    ANA_REG_24,
    ANA_REG_25,
    ANA_REG_26,
    ANA_REG_27,
    ANA_REG_28,
    ANA_REG_29,
    ANA_REG_2A,
    ANA_REG_2B,
    ANA_REG_2C,
    ANA_REG_2D,
    ANA_REG_2E,
    ANA_REG_2F,
    ANA_REG_30,
    ANA_REG_31,
    ANA_REG_32,
    ANA_REG_33,
    ANA_REG_34,
    ANA_REG_35,
    ANA_REG_36,
    ANA_REG_37,
    ANA_REG_38,
    ANA_REG_39,
    ANA_REG_3A,
    ANA_REG_3B,
    ANA_REG_3C,
    ANA_REG_3D,
    ANA_REG_3E,
    ANA_REG_3F,
    ANA_REG_40,
    ANA_REG_41,
    ANA_REG_42,
    ANA_REG_43,
    ANA_REG_44,
    ANA_REG_45,
    ANA_REG_46,
    ANA_REG_47,
    ANA_REG_48,
    ANA_REG_49,
    ANA_REG_4A,
    ANA_REG_4B,
    ANA_REG_4C,
    ANA_REG_4D,
    ANA_REG_4E,
    ANA_REG_4F,
    ANA_REG_50,

    ANA_REG_58 = 0x58,

    ANA_REG_5A = 0x5A,

    ANA_REG_60 = 0x60,
    ANA_REG_61,
    ANA_REG_62,
    ANA_REG_63,
    ANA_REG_64,
    ANA_REG_65,
    ANA_REG_66,
    ANA_REG_67,
    ANA_REG_68,
    ANA_REG_69,
    ANA_REG_6A,
    ANA_REG_6B,
    ANA_REG_6C,
    ANA_REG_6D,
    ANA_REG_6E,
    ANA_REG_6F,
    ANA_REG_70,
    ANA_REG_71,
    ANA_REG_72,
    ANA_REG_73,
    ANA_REG_74,
    ANA_REG_75,
    ANA_REG_76,
    ANA_REG_77,
    ANA_REG_78,
    ANA_REG_79,
    ANA_REG_7A,
    ANA_REG_7B,
    ANA_REG_7C,
    ANA_REG_7D,
    ANA_REG_7E,
    ANA_REG_7F,
    ANA_REG_80,
    ANA_REG_81,
    ANA_REG_82,
    ANA_REG_83,
    ANA_REG_84,
    ANA_REG_85,
    ANA_REG_86,
    ANA_REG_87,
    ANA_REG_88,
    ANA_REG_89,
    ANA_REG_8A,
    ANA_REG_8B,
    ANA_REG_8C,
    ANA_REG_8D,
    ANA_REG_8E,
    ANA_REG_8F,

    ANA_REG_101 = 0x101,
    ANA_REG_102,
    ANA_REG_103,
    ANA_REG_104,
    ANA_REG_105,
    ANA_REG_106,
    ANA_REG_107,
    ANA_REG_108,
    ANA_REG_109,
    ANA_REG_10A,
    ANA_REG_10B,
    ANA_REG_10C,
    ANA_REG_10D,
    ANA_REG_10E,
    ANA_REG_10F,
    ANA_REG_110,
    ANA_REG_111,
    ANA_REG_112,
    ANA_REG_113,
    ANA_REG_114,
    ANA_REG_115,
    ANA_REG_116,
    ANA_REG_117,
    ANA_REG_118,
    ANA_REG_119,
    ANA_REG_11A,
    ANA_REG_11B,
    ANA_REG_11C,
    ANA_REG_11D,
    ANA_REG_11E,
    ANA_REG_11F,
    ANA_REG_120,
    ANA_REG_121,
    ANA_REG_122,
    ANA_REG_123,
    ANA_REG_124,
    ANA_REG_125,
    ANA_REG_126,
    ANA_REG_127,
    ANA_REG_128,
    ANA_REG_129,
    ANA_REG_12A,
    ANA_REG_12B,
    ANA_REG_12C,
    ANA_REG_12D,
    ANA_REG_12E,
    ANA_REG_12F,
    ANA_REG_130,
    ANA_REG_131,
    ANA_REG_132,
    ANA_REG_133,
    ANA_REG_134,
    ANA_REG_135,
    ANA_REG_136,
    ANA_REG_137,
    ANA_REG_138,
    ANA_REG_139,
    ANA_REG_13A,
    ANA_REG_13B,
    ANA_REG_13C,
    ANA_REG_13D,
    ANA_REG_13E,
    ANA_REG_13F,
    ANA_REG_140,
    ANA_REG_141,
    ANA_REG_142,
    ANA_REG_143,
    ANA_REG_144,
    ANA_REG_145,
    ANA_REG_146,
    ANA_REG_147,
    ANA_REG_148,
    ANA_REG_149,
    ANA_REG_14A,
    ANA_REG_14B,
    ANA_REG_14C,
    ANA_REG_14D,
    ANA_REG_14E,
    ANA_REG_14F,
    ANA_REG_150,
    ANA_REG_151,
    ANA_REG_152,
    ANA_REG_153,
    ANA_REG_154,
    ANA_REG_155,
    ANA_REG_156,
    ANA_REG_157,
    ANA_REG_158,
    ANA_REG_159,
    ANA_REG_15A,
    ANA_REG_15B,
    ANA_REG_15C,
    ANA_REG_15D,
    ANA_REG_15E,
    ANA_REG_15F,
    ANA_REG_160,
    ANA_REG_161,
    ANA_REG_162,
    ANA_REG_163,
    ANA_REG_164,
    ANA_REG_165,
    ANA_REG_166,
    ANA_REG_167,
    ANA_REG_168,
    ANA_REG_169,
    ANA_REG_16A,
    ANA_REG_16B,
    ANA_REG_16C,
    ANA_REG_16D,
    ANA_REG_16E,
    ANA_REG_16F,
    ANA_REG_170,
    ANA_REG_171,
    ANA_REG_172,
    ANA_REG_173,
    ANA_REG_174,
    ANA_REG_175,
    ANA_REG_176,
    ANA_REG_177,
    ANA_REG_178,
    ANA_REG_179,
    ANA_REG_17A,
    ANA_REG_17B,
    ANA_REG_17C,
    ANA_REG_17D,
    ANA_REG_17E,
    ANA_REG_17F,
    ANA_REG_180,
    ANA_REG_181,
    ANA_REG_182,
    ANA_REG_183,
    ANA_REG_184,
    ANA_REG_185,
    ANA_REG_186,
    ANA_REG_187,
    ANA_REG_188,
    ANA_REG_189,
    ANA_REG_18A,
    ANA_REG_18B,
    ANA_REG_18C,
    ANA_REG_18D,
    ANA_REG_18E,
    ANA_REG_18F,
    ANA_REG_190,
    ANA_REG_191,
    ANA_REG_192,
    ANA_REG_193,
    ANA_REG_194,
    ANA_REG_195,
    ANA_REG_196,
    ANA_REG_197,
    ANA_REG_198,
    ANA_REG_199,
    ANA_REG_19A,
    ANA_REG_19B,
    ANA_REG_19C,
    ANA_REG_19D,
    ANA_REG_19E,
    ANA_REG_19F,
    ANA_REG_1A0,
    ANA_REG_1A1,
    ANA_REG_1A2,
    ANA_REG_1A3,
    ANA_REG_1A4,
    ANA_REG_1A5,
    ANA_REG_1A6,
    ANA_REG_1A7,
    ANA_REG_1A8,
    ANA_REG_1A9,
    ANA_REG_1AA,
    ANA_REG_1AB,
    ANA_REG_1AC,
    ANA_REG_1AD,
    ANA_REG_1AE,
    ANA_REG_1AF,
    ANA_REG_1B0,
    ANA_REG_1B1,
    ANA_REG_1B2,
    ANA_REG_1B3,
    ANA_REG_1B4,
    ANA_REG_1B5,
    ANA_REG_1B6,
    ANA_REG_1B7,
    ANA_REG_1B8,
    ANA_REG_1B9,
    ANA_REG_1BA,
    ANA_REG_1BB,
    ANA_REG_1BC,
    ANA_REG_1BD,
    ANA_REG_1BE,
    ANA_REG_1BF,
    ANA_REG_1C0,
    ANA_REG_1C1,
    ANA_REG_1C2,
    ANA_REG_1C3,
    ANA_REG_1C4,
    ANA_REG_1C5,
    ANA_REG_1C6,
    ANA_REG_1C7,
    ANA_REG_1C8,
    ANA_REG_1C9,
    ANA_REG_1CA,
    ANA_REG_1CB,
    ANA_REG_1CC,
    ANA_REG_1CD,
    ANA_REG_1CE,
    ANA_REG_1CF,
};

// REG_00
#define REVID_SHIFT                         0
#define REVID_MASK                          (0xF << REVID_SHIFT)
#define REVID(n)                            BITFIELD_VAL(REVID, n)
#define CHIPID_SHIFT                        4
#define CHIPID_MASK                         (0xFFF << CHIPID_SHIFT)
#define CHIPID(n)                           BITFIELD_VAL(CHIPID, n)

// REG_01
#define REG_CODEC_BIAS_IBSEL_LP_RSEL_SHIFT  3
#define REG_CODEC_BIAS_IBSEL_LP_RSEL_MASK   (0x3 << REG_CODEC_BIAS_IBSEL_LP_RSEL_SHIFT)
#define REG_CODEC_BIAS_IBSEL_LP_RSEL(n)     BITFIELD_VAL(REG_CODEC_BIAS_IBSEL_LP_RSEL, n)
#define REG_CODEC_BIAS_IBSEL_RSEL_SHIFT     5
#define REG_CODEC_BIAS_IBSEL_RSEL_MASK      (0x3 << REG_CODEC_BIAS_IBSEL_RSEL_SHIFT)
#define REG_CODEC_BIAS_IBSEL_RSEL(n)        BITFIELD_VAL(REG_CODEC_BIAS_IBSEL_RSEL, n)
#define REG_LPO_SEL                         (1 << 7)
#define CFG_VOICE2OSC                       (1 << 8)
#define REG_CODEC_EN_BIAS                   (1 << 13)
#define REG_CODEC_EN_BIAS_LP                (1 << 14)
#define REG_CODEC_EN_RX_EXT                 (1 << 15)

// REG_02
#define REG_CODEC_BIAS_IBSEL_SHIFT          0
#define REG_CODEC_BIAS_IBSEL_MASK           (0xF << REG_CODEC_BIAS_IBSEL_SHIFT)
#define REG_CODEC_BIAS_IBSEL(n)             BITFIELD_VAL(REG_CODEC_BIAS_IBSEL, n)
#define REG_CODEC_BIAS_IBSEL_TX_SHIFT       4
#define REG_CODEC_BIAS_IBSEL_TX_MASK        (0xF << REG_CODEC_BIAS_IBSEL_TX_SHIFT)
#define REG_CODEC_BIAS_IBSEL_TX(n)          BITFIELD_VAL(REG_CODEC_BIAS_IBSEL_TX, n)
#define REG_CODEC_BIAS_IBSEL_VOICE_SHIFT    8
#define REG_CODEC_BIAS_IBSEL_VOICE_MASK     (0xF << REG_CODEC_BIAS_IBSEL_VOICE_SHIFT)
#define REG_CODEC_BIAS_IBSEL_VOICE(n)       BITFIELD_VAL(REG_CODEC_BIAS_IBSEL_VOICE, n)
#define CFG_ADC_START                       (1 << 12)
#define CFG_REG_CLKMUX_DVDD_SEL             (1 << 13)

// REG_03
#define REG_CODEC_EN_VCM                    (1 << 0)
#define REG_CODEC_EN_VCM_BUFFER             (1 << 1)
#define REG_CODEC_LP_VCM                    (1 << 2)
#define REG_CODEC_VCM_LOW_VCM_SHIFT         4
#define REG_CODEC_VCM_LOW_VCM_MASK          (0xF << REG_CODEC_VCM_LOW_VCM_SHIFT)
#define REG_CODEC_VCM_LOW_VCM(n)            BITFIELD_VAL(REG_CODEC_VCM_LOW_VCM, n)
#define REG_CODEC_VCM_LOW_VCM_LP_SHIFT      8
#define REG_CODEC_VCM_LOW_VCM_LP_MASK       (0xF << REG_CODEC_VCM_LOW_VCM_LP_SHIFT)
#define REG_CODEC_VCM_LOW_VCM_LP(n)         BITFIELD_VAL(REG_CODEC_VCM_LOW_VCM_LP, n)
#define REG_CODEC_VCM_LOW_VCM_LPF_SHIFT     12
#define REG_CODEC_VCM_LOW_VCM_LPF_MASK      (0xF << REG_CODEC_VCM_LOW_VCM_LPF_SHIFT)
#define REG_CODEC_VCM_LOW_VCM_LPF(n)        BITFIELD_VAL(REG_CODEC_VCM_LOW_VCM_LPF, n)

// REG_04
#define REG_CODEC_VCM_EN_LPF                (1 << 0)
#define REG_CODEC_BIAS_LOWV                 (1 << 1)
#define REG_CODEC_BIAS_LOWV_LP_SHIFT        8
#define REG_CODEC_BIAS_LOWV_LP_MASK         (0x3 << REG_CODEC_BIAS_LOWV_LP_SHIFT)
#define REG_CODEC_BIAS_LOWV_LP(n)           BITFIELD_VAL(REG_CODEC_BIAS_LOWV_LP, n)
#define REG_CODEC_BUF_LOWPOWER              (1 << 3)
#define REG_CODEC_BUF_LOWPOWER2             (1 << 4)
#define REG_CODEC_BUF_LOWVCM_SHIFT          5
#define REG_CODEC_BUF_LOWVCM_MASK           (0x7 << REG_CODEC_BUF_LOWVCM_SHIFT)
#define REG_CODEC_BUF_LOWVCM(n)             BITFIELD_VAL(REG_CODEC_BUF_LOWVCM, n)
#define REG_CLKMUX_LDO_HP                   (1 << 10)

// REG_05
#define CFG_VOICE2ADCE                      (1 << 0)
#define ADC_SEL_VAD_DIG_SHIFT               1
#define ADC_SEL_VAD_DIG_MASK                (0x7 << ADC_SEL_VAD_DIG_SHIFT)
#define ADC_SEL_VAD_DIG(n)                  BITFIELD_VAL(ADC_SEL_VAD_DIG, n)
#define DIG_PU_CLKMUX_LDO0P9                (1 << 5)
#define REG_PU_OSC                          (1 << 6)
#define REG_CLKMUX_DVDD_SEL                 (1 << 7)
#define REG_CRYSTAL_SEL_LV                  (1 << 8)
#define REG_EXTPLL_SEL                      (1 << 9)
#define REG_CODEC_BUF_IBIT_SHIFT            10
#define REG_CODEC_BUF_IBIT_MASK             (0x7 << REG_CODEC_BUF_IBIT_SHIFT)
#define REG_CODEC_BUF_IBIT(n)               BITFIELD_VAL(REG_CODEC_BUF_IBIT, n)
#define REG_CLKMUX_LOWF_IN                  (1 << 13)
#define REG_CODEC_BUF_RSEL                  (1 << 14)

// REG_06
#define RESERVED_ANA_15_0_SHIFT             0
#define RESERVED_ANA_15_0_MASK              (0xFFFF << RESERVED_ANA_15_0_SHIFT)
#define RESERVED_ANA_15_0(n)                BITFIELD_VAL(RESERVED_ANA_15_0, n)

// REG_07
#define RESERVED_ANA_31_16_SHIFT            0
#define RESERVED_ANA_31_16_MASK             (0xFFFF << RESERVED_ANA_31_16_SHIFT)
#define RESERVED_ANA_31_16(n)               BITFIELD_VAL(RESERVED_ANA_31_16, n)

// REG_08
#define RESERVED_ANA_47_32_SHIFT            0
#define RESERVED_ANA_47_32_MASK             (0xFFFF << RESERVED_ANA_47_32_SHIFT)
#define RESERVED_ANA_47_32(n)               BITFIELD_VAL(RESERVED_ANA_47_32, n)

// REG_09
#define RESERVED_ANA_55_48_SHIFT            0
#define RESERVED_ANA_55_48_MASK             (0xFF << RESERVED_ANA_55_48_SHIFT)
#define RESERVED_ANA_55_48(n)               BITFIELD_VAL(RESERVED_ANA_55_48, n)
#define POWER_STATE_SHIFT                   8
#define POWER_STATE_MASK                    (0x7 << POWER_STATE_SHIFT)
#define POWER_STATE(n)                      BITFIELD_VAL(POWER_STATE, n)

// REG_0A
#define CHIP_ADDR_I2C_SHIFT                 0
#define CHIP_ADDR_I2C_MASK                  (0x7F << CHIP_ADDR_I2C_SHIFT)
#define CHIP_ADDR_I2C(n)                    BITFIELD_VAL(CHIP_ADDR_I2C, n)

// REG_0B
#define REG_POWER_TIMER1_SHIFT              0
#define REG_POWER_TIMER1_MASK               (0x3F << REG_POWER_TIMER1_SHIFT)
#define REG_POWER_TIMER1(n)                 BITFIELD_VAL(REG_POWER_TIMER1, n)
#define REG_POWER_TIMER2_SHIFT              6
#define REG_POWER_TIMER2_MASK               (0x3F << REG_POWER_TIMER2_SHIFT)
#define REG_POWER_TIMER2(n)                 BITFIELD_VAL(REG_POWER_TIMER2, n)
#define REG_POWERUP                         (1 << 12)
#define REG_PU_CAP_MEM                      (1 << 13)
#define REG_ISO_EN_ANA2CORE_DR              (1 << 14)
#define REG_ISO_EN_ANA2CORE                 (1 << 15)

// REG_0C
#define REG_POWER_TIMER3_SHIFT              0
#define REG_POWER_TIMER3_MASK               (0x3F << REG_POWER_TIMER3_SHIFT)
#define REG_POWER_TIMER3(n)                 BITFIELD_VAL(REG_POWER_TIMER3, n)
#define REG_POWER_TIMER4_SHIFT              6
#define REG_POWER_TIMER4_MASK               (0x3F << REG_POWER_TIMER4_SHIFT)
#define REG_POWER_TIMER4(n)                 BITFIELD_VAL(REG_POWER_TIMER4, n)
#define REG_ISO_EN_DR                       (1 << 12)
#define REG_PSW_EN_DR                       (1 << 13)
#define REG_ISO_EN                          (1 << 14)
#define REG_PSW_EN                          (1 << 15)

// REG_0D
#define REG_POWER_TIMER5_SHIFT              0
#define REG_POWER_TIMER5_MASK               (0x3F << REG_POWER_TIMER5_SHIFT)
#define REG_POWER_TIMER5(n)                 BITFIELD_VAL(REG_POWER_TIMER5, n)
#define REG_POWER_TIMER6_SHIFT              6
#define REG_POWER_TIMER6_MASK               (0x3F << REG_POWER_TIMER6_SHIFT)
#define REG_POWER_TIMER6(n)                 BITFIELD_VAL(REG_POWER_TIMER6, n)
#define REG_RESET_ASSERT_DR                 (1 << 12)
#define REG_RESET_ASSERT                    (1 << 13)
#define REG_ISO_EN_ANA_INF_DR               (1 << 14)
#define REG_ISO_EN_ANA_INF                  (1 << 15)

// REG_0E
#define REG_CLK_DAC_CODEC_DIG_GATE          (1 << 0)
#define REG_CLK_ADC_6M_CODEC_GATE           (1 << 1)
#define REG_RC_OSC_CLK_ADC_DIG_GATE         (1 << 2)
#define REG_CLK_DAC_CODEC_ANA_GATE          (1 << 3)
#define REG_CLK_OSC_PD_GATE                 (1 << 4)
#define REG_CLK_OSC_AON_GATE                (1 << 5)
#define REG_PU_ANA_MASK                     (1 << 6)
#define REG_CLK_SAR_OSC_EN                  (1 << 7)
#define REG_CLK_6M_CODEC_SAR_EN             (1 << 8)
#define REG_CLK_6M_SAR_CODEC_SEL            (1 << 9)
#define REG_SOFT_RESETN_SAR                 (1 << 10)
#define REG_CLK_SARADC_ANA_SEL              (1 << 11)
#define REG_CLK_SARADC_POL                  (1 << 12)
#define REG_CLK_SAR_CTRL_INV                (1 << 13)

// REG_0F
#define PU_RC_CAP_DR                        (1 << 0)
#define REG_PU_RC_CAP                       (1 << 1)

// REG_30
#define REG_SAR_INIT_CALI_BIT_SHIFT         0
#define REG_SAR_INIT_CALI_BIT_MASK          (0x1F << REG_SAR_INIT_CALI_BIT_SHIFT)
#define REG_SAR_INIT_CALI_BIT(n)            BITFIELD_VAL(REG_SAR_INIT_CALI_BIT, n)
#define REG_SAR_CALI                        (1 << 5)
#define REG_SAR_CALI_CNT_SHIFT              6
#define REG_SAR_CALI_CNT_MASK               (0xF << REG_SAR_CALI_CNT_SHIFT)
#define REG_SAR_CALI_CNT(n)                 BITFIELD_VAL(REG_SAR_CALI_CNT, n)
#define REG_SAR_CALI_LSB_SCRN               (1 << 10)
#define REG_SAR_WEIGHT_DR                   (1 << 11)
#define REG_CODEC_SAR_VREF_PULLDOWN         (1 << 12)
#define REG_CODEC_SAR_VREF_LOWGAIN_SHIFT    13
#define REG_CODEC_SAR_VREF_LOWGAIN_MASK     (0x3 << REG_CODEC_SAR_VREF_LOWGAIN_SHIFT)
#define REG_CODEC_SAR_VREF_LOWGAIN(n)       BITFIELD_VAL(REG_CODEC_SAR_VREF_LOWGAIN, n)

// REG_31
#define CLK_SARADC_CODEC_INV                (1 << 0)
#define REG_CLK_SAR_MUX                     (1 << 1)
#define REG_SAR_OFFSET_DR                   (1 << 2)
#define REG_SAR_OFFSET_P_SHIFT              3
#define REG_SAR_OFFSET_P_MASK               (0x1F << REG_SAR_OFFSET_P_SHIFT)
#define REG_SAR_OFFSET_P(n)                 BITFIELD_VAL(REG_SAR_OFFSET_P, n)
#define REG_SAR_OFFSET_N_SHIFT              8
#define REG_SAR_OFFSET_N_MASK               (0x1F << REG_SAR_OFFSET_N_SHIFT)
#define REG_SAR_OFFSET_N(n)                 BITFIELD_VAL(REG_SAR_OFFSET_N, n)
#define REG_CODEC_SAR_DELAY_CMP             (1 << 13)
#define REG_CODEC_SAR_HIGH_DVDD_EN          (1 << 14)
#define REG_CODEC_SAR_DVDD_IBIT             (1 << 15)

// REG_32
#define REG_CODEC_SAR_PGA_EN_IN             (1 << 0)
#define REG_SAR_DIFF_EN_IN_DR               (1 << 1)
#define REG_SAR_DIFF_EN_IN_REG              (1 << 2)
#define REG_CODEC_SAR_PGA_BYPASS            (1 << 3)
#define REG_CODEC_SAR_PGA_CC_SEL            (1 << 4)
#define REG_CODEC_SAR_PGA_OFFSET_CAL_EN     (1 << 5)
#define REG_CODEC_SAR_PGA_RFB_HALF          (1 << 6)
#define REG_CODEC_SAR_PGA_RIN_HALF          (1 << 7)
#define REG_CODEC_SAR_PRE_CHARGE            (1 << 8)
#define REG_CODEC_SAR_PU_CHOP               (1 << 9)
#define REG_CODEC_SAR_PU_IN                 (1 << 10)
#define REG_CODEC_SAR_RES_BIT_SHIFT         11
#define REG_CODEC_SAR_RES_BIT_MASK          (0x3 << REG_CODEC_SAR_RES_BIT_SHIFT)
#define REG_CODEC_SAR_RES_BIT(n)            BITFIELD_VAL(REG_CODEC_SAR_RES_BIT, n)
#define REG_CODEC_SAR_RESET_PGA             (1 << 13)
#define REG_CODEC_SAR_RFB_EN                (1 << 14)
#define REG_SAR_ADC_OFFSET_DR               (1 << 15)

// REG_33
#define REG_SAR_ADC_OFFSET_SHIFT            0
#define REG_SAR_ADC_OFFSET_MASK             (0xFFFF << REG_SAR_ADC_OFFSET_SHIFT)
#define REG_SAR_ADC_OFFSET(n)               BITFIELD_VAL(REG_SAR_ADC_OFFSET, n)

// REG_34
#define REG_CODEC_SAR_VCMBUF_I_SEL          (1 << 0)
#define REG_CODEC_SAR_PU_AVDD18             (1 << 1)
#define REG_CODEC_SAR_EN_VREF_LP            (1 << 2)
#define REG_CODEC_SAR_HIGH_PW_CMP_SHIFT     3
#define REG_CODEC_SAR_HIGH_PW_CMP_MASK      (0x3 << REG_CODEC_SAR_HIGH_PW_CMP_SHIFT)
#define REG_CODEC_SAR_HIGH_PW_CMP(n)        BITFIELD_VAL(REG_CODEC_SAR_HIGH_PW_CMP, n)
#define REG_CODEC_SAR_DELAY_BIT_SHIFT       5
#define REG_CODEC_SAR_DELAY_BIT_MASK        (0x7 << REG_CODEC_SAR_DELAY_BIT_SHIFT)
#define REG_CODEC_SAR_DELAY_BIT(n)          BITFIELD_VAL(REG_CODEC_SAR_DELAY_BIT, n)
#define REG_CODEC_SAR_CFB_SEL_SHIFT         8
#define REG_CODEC_SAR_CFB_SEL_MASK          (0xF << REG_CODEC_SAR_CFB_SEL_SHIFT)
#define REG_CODEC_SAR_CFB_SEL(n)            BITFIELD_VAL(REG_CODEC_SAR_CFB_SEL, n)
#define REG_CODEC_SAR_GAIN_SHIFT            12
#define REG_CODEC_SAR_GAIN_MASK             (0x7 << REG_CODEC_SAR_GAIN_SHIFT)
#define REG_CODEC_SAR_GAIN(n)               BITFIELD_VAL(REG_CODEC_SAR_GAIN, n)
#define REG_CODEC_SAR_EN_PGA                (1 << 15)

// REG_35
#define SAR_DOUT_SHIFT                      0
#define SAR_DOUT_MASK                       (0xFFFF << SAR_DOUT_SHIFT)
#define SAR_DOUT(n)                         BITFIELD_VAL(SAR_DOUT, n)

// REG_36
#define SAR_RAW_DATA_8_B_SHIFT              0
#define SAR_RAW_DATA_8_B_MASK               (0x1FF << SAR_RAW_DATA_8_B_SHIFT)
#define SAR_RAW_DATA_8_B(n)                 BITFIELD_VAL(SAR_RAW_DATA_8_B, n)

// REG_37
#define SAR_RAW_DATA_B_SHIFT                0
#define SAR_RAW_DATA_B_MASK                 (0x1FF << SAR_RAW_DATA_B_SHIFT)
#define SAR_RAW_DATA_B(n)                   BITFIELD_VAL(SAR_RAW_DATA_B, n)
#define CODEC_SARADC_CH_0                   (1 << 9)

// REG_38
#define CODEC_SARADC_CH_16_1_SHIFT          0
#define CODEC_SARADC_CH_16_1_MASK           (0xFFFF << CODEC_SARADC_CH_16_1_SHIFT)
#define CODEC_SARADC_CH_16_1(n)             BITFIELD_VAL(CODEC_SARADC_CH_16_1, n)

// REG_40
#define REG_SAR_OFFSET_CALI                 (1 << 0)
#define REG_SAR_OFFSET_CALI_CNT_SHIFT       1
#define REG_SAR_OFFSET_CALI_CNT_MASK        (0x7 << REG_SAR_OFFSET_CALI_CNT_SHIFT)
#define REG_SAR_OFFSET_CALI_CNT(n)          BITFIELD_VAL(REG_SAR_OFFSET_CALI_CNT, n)
#define REG_SAR_CLK_OUT_SEL                 (1 << 4)
#define REG_SAR_CLK_INT_DIV_SHIFT           5
#define REG_SAR_CLK_INT_DIV_MASK            (0x7F << REG_SAR_CLK_INT_DIV_SHIFT)
#define REG_SAR_CLK_INT_DIV(n)              BITFIELD_VAL(REG_SAR_CLK_INT_DIV, n)
#define SAR_VOUT_CALIB_INV                  (1 << 12)
#define SAR_CONV_DONE_INV                   (1 << 13)
#define REG_SAR_RESULT_SEL                  (1 << 14)
#define REG_SAR_ADC_ON                      (1 << 15)

// REG_41
#define REG_CODEC_SAR_CLK_TRIM_SHIFT        8
#define REG_CODEC_SAR_CLK_TRIM_MASK         (0x3 << REG_CODEC_SAR_CLK_TRIM_SHIFT)
#define REG_CODEC_SAR_CLK_TRIM(n)           BITFIELD_VAL(REG_CODEC_SAR_CLK_TRIM, n)
#define REG_CODEC_SAR_CLK_MODE              (1 << 10)
#define REG_CODEC_SAR_EN_PREAMP             (1 << 13)
#define REG_SAR_CLK_EN                      (1 << 14)

// REG_42
#define REG_SAR_PU_PIN_BIAS                 (1 << 0)
#define REG_SAR_PU_NIN_BIAS                 (1 << 1)
#define REG_CODEC_SAR_SE_MODE               (1 << 2)
#define REG_CODEC_SAR_VREG_IBIT_SHIFT       5
#define REG_CODEC_SAR_VREG_IBIT_MASK        (0x3 << REG_CODEC_SAR_VREG_IBIT_SHIFT)
#define REG_CODEC_SAR_VREG_IBIT(n)          BITFIELD_VAL(REG_CODEC_SAR_VREG_IBIT, n)
#define REG_CODEC_SAR_VREG_OUTCAP_MODE_SHIFT 7
#define REG_CODEC_SAR_VREG_OUTCAP_MODE_MASK (0x3 << REG_CODEC_SAR_VREG_OUTCAP_MODE_SHIFT)
#define REG_CODEC_SAR_VREG_OUTCAP_MODE(n)   BITFIELD_VAL(REG_CODEC_SAR_VREG_OUTCAP_MODE, n)
#define REG_CODEC_SAR_VREG_SEL_SHIFT        9
#define REG_CODEC_SAR_VREG_SEL_MASK         (0xF << REG_CODEC_SAR_VREG_SEL_SHIFT)
#define REG_CODEC_SAR_VREG_SEL(n)           BITFIELD_VAL(REG_CODEC_SAR_VREG_SEL, n)
#define REG_SAR_PU_PIN_BIAS_DR              (1 << 13)
#define REG_SAR_PU_NIN_BIAS_DR              (1 << 14)

// REG_43
#define REG_SAR_CLK_OUT_DIV_SHIFT           0
#define REG_SAR_CLK_OUT_DIV_MASK            (0x1FFF << REG_SAR_CLK_OUT_DIV_SHIFT)
#define REG_SAR_CLK_OUT_DIV(n)              BITFIELD_VAL(REG_SAR_CLK_OUT_DIV, n)
#define REG_SAR_CLK_SEL_SHIFT               13
#define REG_SAR_CLK_SEL_MASK                (0x3 << REG_SAR_CLK_SEL_SHIFT)
#define REG_SAR_CLK_SEL(n)                  BITFIELD_VAL(REG_SAR_CLK_SEL, n)

// REG_44
#define REG_SAR_PU_VREF_DR                  (1 << 0)
#define REG_SAR_PU_VREF_REG                 (1 << 1)
#define REG_SAR_PU_VREF_IN                  (1 << 2)
#define REG_SAR_PU_DR                       (1 << 3)
#define REG_SAR_PU_REG                      (1 << 4)
#define REG_SAR_PU_PRECHARGE_DR             (1 << 5)
#define REG_SAR_PU_PRECHARGE_REG            (1 << 6)
#define REG_SAR_PU_RST_DR                   (1 << 7)
#define REG_SAR_PU_RST_REG                  (1 << 8)
#define PU_OSC_SAR                          (1 << 9)
#define REG_DET_EN                          (1 << 10)
#define REG_CRYSTAL_SEL                     (1 << 11)
#define REG_SAR_EN_SAR_DOUT                 (1 << 12)

// REG_45
#define REG_SAR_PU_START_DLY_CNT_SHIFT      0
#define REG_SAR_PU_START_DLY_CNT_MASK       (0x7F << REG_SAR_PU_START_DLY_CNT_SHIFT)
#define REG_SAR_PU_START_DLY_CNT(n)         BITFIELD_VAL(REG_SAR_PU_START_DLY_CNT, n)
#define REG_SAR_PU_PRECHARGE_START_DLY_CNT_SHIFT 7
#define REG_SAR_PU_PRECHARGE_START_DLY_CNT_MASK (0x7F << REG_SAR_PU_PRECHARGE_START_DLY_CNT_SHIFT)
#define REG_SAR_PU_PRECHARGE_START_DLY_CNT(n) BITFIELD_VAL(REG_SAR_PU_PRECHARGE_START_DLY_CNT, n)

// REG_46
#define REG_SAR_PU_RST_START_DLY_CNT_SHIFT  0
#define REG_SAR_PU_RST_START_DLY_CNT_MASK   (0x7F << REG_SAR_PU_RST_START_DLY_CNT_SHIFT)
#define REG_SAR_PU_RST_START_DLY_CNT(n)     BITFIELD_VAL(REG_SAR_PU_RST_START_DLY_CNT, n)
#define REG_SAR_PU_RST_LAST_CNT_SHIFT       7
#define REG_SAR_PU_RST_LAST_CNT_MASK        (0x7F << REG_SAR_PU_RST_LAST_CNT_SHIFT)
#define REG_SAR_PU_RST_LAST_CNT(n)          BITFIELD_VAL(REG_SAR_PU_RST_LAST_CNT, n)

// REG_47
#define REG_SAR_PU_PRECHARGE_LAST_CNT_SHIFT 0
#define REG_SAR_PU_PRECHARGE_LAST_CNT_MASK  (0x1FF << REG_SAR_PU_PRECHARGE_LAST_CNT_SHIFT)
#define REG_SAR_PU_PRECHARGE_LAST_CNT(n)    BITFIELD_VAL(REG_SAR_PU_PRECHARGE_LAST_CNT, n)
#define REG_CODEC_SAR_IBIT_IN1_SHIFT        9
#define REG_CODEC_SAR_IBIT_IN1_MASK         (0x7 << REG_CODEC_SAR_IBIT_IN1_SHIFT)
#define REG_CODEC_SAR_IBIT_IN1(n)           BITFIELD_VAL(REG_CODEC_SAR_IBIT_IN1, n)
#define REG_CODEC_SAR_IBIT_IN2_SHIFT        12
#define REG_CODEC_SAR_IBIT_IN2_MASK         (0x7 << REG_CODEC_SAR_IBIT_IN2_SHIFT)
#define REG_CODEC_SAR_IBIT_IN2(n)           BITFIELD_VAL(REG_CODEC_SAR_IBIT_IN2, n)

// REG_48
#define SAR_BIT00_WEIGHT_SHIFT              0
#define SAR_BIT00_WEIGHT_MASK               (0x3FFF << SAR_BIT00_WEIGHT_SHIFT)
#define SAR_BIT00_WEIGHT(n)                 BITFIELD_VAL(SAR_BIT00_WEIGHT, n)

// REG_49
#define SAR_BIT01_WEIGHT_SHIFT              0
#define SAR_BIT01_WEIGHT_MASK               (0x3FFF << SAR_BIT01_WEIGHT_SHIFT)
#define SAR_BIT01_WEIGHT(n)                 BITFIELD_VAL(SAR_BIT01_WEIGHT, n)

// REG_4A
#define SAR_BIT02_WEIGHT_SHIFT              0
#define SAR_BIT02_WEIGHT_MASK               (0x3FFF << SAR_BIT02_WEIGHT_SHIFT)
#define SAR_BIT02_WEIGHT(n)                 BITFIELD_VAL(SAR_BIT02_WEIGHT, n)

// REG_4B
#define SAR_BIT03_WEIGHT_SHIFT              0
#define SAR_BIT03_WEIGHT_MASK               (0x3FFF << SAR_BIT03_WEIGHT_SHIFT)
#define SAR_BIT03_WEIGHT(n)                 BITFIELD_VAL(SAR_BIT03_WEIGHT, n)

// REG_4C
#define SAR_BIT04_WEIGHT_SHIFT              0
#define SAR_BIT04_WEIGHT_MASK               (0x3FFF << SAR_BIT04_WEIGHT_SHIFT)
#define SAR_BIT04_WEIGHT(n)                 BITFIELD_VAL(SAR_BIT04_WEIGHT, n)

// REG_4D
#define SAR_P_BIT05_WEIGHT_SHIFT            0
#define SAR_P_BIT05_WEIGHT_MASK             (0x3FFF << SAR_P_BIT05_WEIGHT_SHIFT)
#define SAR_P_BIT05_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT05_WEIGHT, n)

// REG_4E
#define SAR_P_BIT06_WEIGHT_SHIFT            0
#define SAR_P_BIT06_WEIGHT_MASK             (0x3FFF << SAR_P_BIT06_WEIGHT_SHIFT)
#define SAR_P_BIT06_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT06_WEIGHT, n)

// REG_4F
#define SAR_P_BIT07_WEIGHT_SHIFT            0
#define SAR_P_BIT07_WEIGHT_MASK             (0x3FFF << SAR_P_BIT07_WEIGHT_SHIFT)
#define SAR_P_BIT07_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT07_WEIGHT, n)

// REG_50
#define SAR_P_BIT08_WEIGHT_SHIFT            0
#define SAR_P_BIT08_WEIGHT_MASK             (0x3FFF << SAR_P_BIT08_WEIGHT_SHIFT)
#define SAR_P_BIT08_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT08_WEIGHT, n)

// REG_51
#define SAR_P_BIT09_WEIGHT_SHIFT            0
#define SAR_P_BIT09_WEIGHT_MASK             (0x3FFF << SAR_P_BIT09_WEIGHT_SHIFT)
#define SAR_P_BIT09_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT09_WEIGHT, n)

// REG_52
#define SAR_P_BIT10_WEIGHT_SHIFT            0
#define SAR_P_BIT10_WEIGHT_MASK             (0x3FFF << SAR_P_BIT10_WEIGHT_SHIFT)
#define SAR_P_BIT10_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT10_WEIGHT, n)

// REG_53
#define SAR_P_BIT11_WEIGHT_SHIFT            0
#define SAR_P_BIT11_WEIGHT_MASK             (0x3FFF << SAR_P_BIT11_WEIGHT_SHIFT)
#define SAR_P_BIT11_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT11_WEIGHT, n)

// REG_54
#define SAR_P_BIT12_WEIGHT_SHIFT            0
#define SAR_P_BIT12_WEIGHT_MASK             (0x3FFF << SAR_P_BIT12_WEIGHT_SHIFT)
#define SAR_P_BIT12_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT12_WEIGHT, n)

// REG_55
#define SAR_P_BIT13_WEIGHT_SHIFT            0
#define SAR_P_BIT13_WEIGHT_MASK             (0x3FFF << SAR_P_BIT13_WEIGHT_SHIFT)
#define SAR_P_BIT13_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT13_WEIGHT, n)

// REG_56
#define SAR_P_BIT14_WEIGHT_SHIFT            0
#define SAR_P_BIT14_WEIGHT_MASK             (0x3FFF << SAR_P_BIT14_WEIGHT_SHIFT)
#define SAR_P_BIT14_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT14_WEIGHT, n)

// REG_57
#define SAR_P_BIT15_WEIGHT_SHIFT            0
#define SAR_P_BIT15_WEIGHT_MASK             (0x3FFF << SAR_P_BIT15_WEIGHT_SHIFT)
#define SAR_P_BIT15_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT15_WEIGHT, n)

// REG_58
#define SAR_P_BIT16_WEIGHT_SHIFT            0
#define SAR_P_BIT16_WEIGHT_MASK             (0x3FFF << SAR_P_BIT16_WEIGHT_SHIFT)
#define SAR_P_BIT16_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT16_WEIGHT, n)

// REG_59
#define SAR_P_BIT17_WEIGHT_SHIFT            0
#define SAR_P_BIT17_WEIGHT_MASK             (0x3FFF << SAR_P_BIT17_WEIGHT_SHIFT)
#define SAR_P_BIT17_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT17_WEIGHT, n)

// REG_5A
#define SAR_N_BIT05_WEIGHT_SHIFT            0
#define SAR_N_BIT05_WEIGHT_MASK             (0x3FFF << SAR_N_BIT05_WEIGHT_SHIFT)
#define SAR_N_BIT05_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT05_WEIGHT, n)

// REG_5B
#define SAR_N_BIT06_WEIGHT_SHIFT            0
#define SAR_N_BIT06_WEIGHT_MASK             (0x3FFF << SAR_N_BIT06_WEIGHT_SHIFT)
#define SAR_N_BIT06_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT06_WEIGHT, n)

// REG_5C
#define SAR_N_BIT07_WEIGHT_SHIFT            0
#define SAR_N_BIT07_WEIGHT_MASK             (0x3FFF << SAR_N_BIT07_WEIGHT_SHIFT)
#define SAR_N_BIT07_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT07_WEIGHT, n)

// REG_5D
#define SAR_N_BIT08_WEIGHT_SHIFT            0
#define SAR_N_BIT08_WEIGHT_MASK             (0x3FFF << SAR_N_BIT08_WEIGHT_SHIFT)
#define SAR_N_BIT08_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT08_WEIGHT, n)

// REG_5E
#define SAR_N_BIT09_WEIGHT_SHIFT            0
#define SAR_N_BIT09_WEIGHT_MASK             (0x3FFF << SAR_N_BIT09_WEIGHT_SHIFT)
#define SAR_N_BIT09_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT09_WEIGHT, n)

// REG_5F
#define SAR_N_BIT10_WEIGHT_SHIFT            0
#define SAR_N_BIT10_WEIGHT_MASK             (0x3FFF << SAR_N_BIT10_WEIGHT_SHIFT)
#define SAR_N_BIT10_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT10_WEIGHT, n)

// REG_60
#define SAR_N_BIT11_WEIGHT_SHIFT            0
#define SAR_N_BIT11_WEIGHT_MASK             (0x3FFF << SAR_N_BIT11_WEIGHT_SHIFT)
#define SAR_N_BIT11_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT11_WEIGHT, n)

// REG_61
#define SAR_N_BIT12_WEIGHT_SHIFT            0
#define SAR_N_BIT12_WEIGHT_MASK             (0x3FFF << SAR_N_BIT12_WEIGHT_SHIFT)
#define SAR_N_BIT12_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT12_WEIGHT, n)

// REG_62
#define SAR_N_BIT13_WEIGHT_SHIFT            0
#define SAR_N_BIT13_WEIGHT_MASK             (0x3FFF << SAR_N_BIT13_WEIGHT_SHIFT)
#define SAR_N_BIT13_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT13_WEIGHT, n)

// REG_63
#define SAR_N_BIT14_WEIGHT_SHIFT            0
#define SAR_N_BIT14_WEIGHT_MASK             (0x3FFF << SAR_N_BIT14_WEIGHT_SHIFT)
#define SAR_N_BIT14_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT14_WEIGHT, n)

// REG_64
#define SAR_N_BIT15_WEIGHT_SHIFT            0
#define SAR_N_BIT15_WEIGHT_MASK             (0x3FFF << SAR_N_BIT15_WEIGHT_SHIFT)
#define SAR_N_BIT15_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT15_WEIGHT, n)

// REG_65
#define SAR_N_BIT16_WEIGHT_SHIFT            0
#define SAR_N_BIT16_WEIGHT_MASK             (0x3FFF << SAR_N_BIT16_WEIGHT_SHIFT)
#define SAR_N_BIT16_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT16_WEIGHT, n)

// REG_66
#define SAR_N_BIT17_WEIGHT_SHIFT            0
#define SAR_N_BIT17_WEIGHT_MASK             (0x3FFF << SAR_N_BIT17_WEIGHT_SHIFT)
#define SAR_N_BIT17_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT17_WEIGHT, n)

// REG_67
#define REG_SAR_TRIG_ENABLE                 (1 << 0)
#define REG_SAR_TRIG_POL_RATE               (1 << 1)
#define REG_SAR_TRIG_DET_CLR                (1 << 2)
#define REG_SAR_TRIG_HPF_BYPASS             (1 << 3)
#define REG_SAR_TRIG_LPF_BYPASS             (1 << 4)
#define REG_SAR_TRIG_U_DC_SHIFT             5
#define REG_SAR_TRIG_U_DC_MASK              (0xF << REG_SAR_TRIG_U_DC_SHIFT)
#define REG_SAR_TRIG_U_DC(n)                BITFIELD_VAL(REG_SAR_TRIG_U_DC, n)

// REG_68
#define REG_SAR_TRIG_COEF_A1_SHIFT          0
#define REG_SAR_TRIG_COEF_A1_MASK           (0x7F << REG_SAR_TRIG_COEF_A1_SHIFT)
#define REG_SAR_TRIG_COEF_A1(n)             BITFIELD_VAL(REG_SAR_TRIG_COEF_A1, n)
#define REG_SAR_TRIG_COEF_B0_SHIFT          7
#define REG_SAR_TRIG_COEF_B0_MASK           (0x7F << REG_SAR_TRIG_COEF_B0_SHIFT)
#define REG_SAR_TRIG_COEF_B0(n)             BITFIELD_VAL(REG_SAR_TRIG_COEF_B0, n)

// REG_69
#define REG_SAR_TRIG_COEF_B1_SHIFT          0
#define REG_SAR_TRIG_COEF_B1_MASK           (0x7F << REG_SAR_TRIG_COEF_B1_SHIFT)
#define REG_SAR_TRIG_COEF_B1(n)             BITFIELD_VAL(REG_SAR_TRIG_COEF_B1, n)

// REG_6A
#define REG_SAR_TRIG_TH_SHIFT               0
#define REG_SAR_TRIG_TH_MASK                (0xFFFF << REG_SAR_TRIG_TH_SHIFT)
#define REG_SAR_TRIG_TH(n)                  BITFIELD_VAL(REG_SAR_TRIG_TH, n)

// REG_6B
#define CFG_SAR_DET_DELAY_SHIFT             0
#define CFG_SAR_DET_DELAY_MASK              (0xFFF << CFG_SAR_DET_DELAY_SHIFT)
#define CFG_SAR_DET_DELAY(n)                BITFIELD_VAL(CFG_SAR_DET_DELAY, n)
#define SAR_SWITCH2OSC_EN                   (1 << 12)
#define SEL_SARCLK_DIV2                     (1 << 13)
#define REG_SAR_TO_SIGN                     (1 << 14)
#define REG_SAR_CLOSE_ON_DET                (1 << 15)

// REG_6C
#define REG_SAR_DET_DLY_TIMER_SHIFT         0
#define REG_SAR_DET_DLY_TIMER_MASK          (0xFF << REG_SAR_DET_DLY_TIMER_SHIFT)
#define REG_SAR_DET_DLY_TIMER(n)            BITFIELD_VAL(REG_SAR_DET_DLY_TIMER, n)
#define REG_SAR_DET_DLY_EN                  (1 << 8)

// REG_6D
#define REG_SAR_ADC_OFFSET_IN_SHIFT         0
#define REG_SAR_ADC_OFFSET_IN_MASK          (0xFFFF << REG_SAR_ADC_OFFSET_IN_SHIFT)
#define REG_SAR_ADC_OFFSET_IN(n)            BITFIELD_VAL(REG_SAR_ADC_OFFSET_IN, n)

// REG_6E
#define REG_SAR_OFFSET_P_OUT_SHIFT          0
#define REG_SAR_OFFSET_P_OUT_MASK           (0x1F << REG_SAR_OFFSET_P_OUT_SHIFT)
#define REG_SAR_OFFSET_P_OUT(n)             BITFIELD_VAL(REG_SAR_OFFSET_P_OUT, n)
#define REG_SAR_OFFSET_N_OUT_SHIFT          5
#define REG_SAR_OFFSET_N_OUT_MASK           (0x1F << REG_SAR_OFFSET_N_OUT_SHIFT)
#define REG_SAR_OFFSET_N_OUT(n)             BITFIELD_VAL(REG_SAR_OFFSET_N_OUT, n)

// REG_6F
#define SAR_P_BIT05_WEIGHT_OUT_SHIFT        0
#define SAR_P_BIT05_WEIGHT_OUT_MASK         (0x3FFF << SAR_P_BIT05_WEIGHT_OUT_SHIFT)
#define SAR_P_BIT05_WEIGHT_OUT(n)           BITFIELD_VAL(SAR_P_BIT05_WEIGHT_OUT, n)

// REG_70
#define SAR_P_BIT06_WEIGHT_OUT_SHIFT        0
#define SAR_P_BIT06_WEIGHT_OUT_MASK         (0x3FFF << SAR_P_BIT06_WEIGHT_OUT_SHIFT)
#define SAR_P_BIT06_WEIGHT_OUT(n)           BITFIELD_VAL(SAR_P_BIT06_WEIGHT_OUT, n)

// REG_71
#define SAR_P_BIT07_WEIGHT_OUT_SHIFT        0
#define SAR_P_BIT07_WEIGHT_OUT_MASK         (0x3FFF << SAR_P_BIT07_WEIGHT_OUT_SHIFT)
#define SAR_P_BIT07_WEIGHT_OUT(n)           BITFIELD_VAL(SAR_P_BIT07_WEIGHT_OUT, n)

// REG_72
#define SAR_P_BIT08_WEIGHT_OUT_SHIFT        0
#define SAR_P_BIT08_WEIGHT_OUT_MASK         (0x3FFF << SAR_P_BIT08_WEIGHT_OUT_SHIFT)
#define SAR_P_BIT08_WEIGHT_OUT(n)           BITFIELD_VAL(SAR_P_BIT08_WEIGHT_OUT, n)

// REG_73
#define SAR_P_BIT09_WEIGHT_OUT_SHIFT        0
#define SAR_P_BIT09_WEIGHT_OUT_MASK         (0x3FFF << SAR_P_BIT09_WEIGHT_OUT_SHIFT)
#define SAR_P_BIT09_WEIGHT_OUT(n)           BITFIELD_VAL(SAR_P_BIT09_WEIGHT_OUT, n)

// REG_74
#define SAR_P_BIT10_WEIGHT_OUT_SHIFT        0
#define SAR_P_BIT10_WEIGHT_OUT_MASK         (0x3FFF << SAR_P_BIT10_WEIGHT_OUT_SHIFT)
#define SAR_P_BIT10_WEIGHT_OUT(n)           BITFIELD_VAL(SAR_P_BIT10_WEIGHT_OUT, n)

// REG_75
#define SAR_P_BIT11_WEIGHT_OUT_SHIFT        0
#define SAR_P_BIT11_WEIGHT_OUT_MASK         (0x3FFF << SAR_P_BIT11_WEIGHT_OUT_SHIFT)
#define SAR_P_BIT11_WEIGHT_OUT(n)           BITFIELD_VAL(SAR_P_BIT11_WEIGHT_OUT, n)

// REG_76
#define SAR_P_BIT12_WEIGHT_OUT_SHIFT        0
#define SAR_P_BIT12_WEIGHT_OUT_MASK         (0x3FFF << SAR_P_BIT12_WEIGHT_OUT_SHIFT)
#define SAR_P_BIT12_WEIGHT_OUT(n)           BITFIELD_VAL(SAR_P_BIT12_WEIGHT_OUT, n)

// REG_77
#define SAR_P_BIT13_WEIGHT_OUT_SHIFT        0
#define SAR_P_BIT13_WEIGHT_OUT_MASK         (0x3FFF << SAR_P_BIT13_WEIGHT_OUT_SHIFT)
#define SAR_P_BIT13_WEIGHT_OUT(n)           BITFIELD_VAL(SAR_P_BIT13_WEIGHT_OUT, n)

// REG_78
#define SAR_P_BIT14_WEIGHT_OUT_SHIFT        0
#define SAR_P_BIT14_WEIGHT_OUT_MASK         (0x3FFF << SAR_P_BIT14_WEIGHT_OUT_SHIFT)
#define SAR_P_BIT14_WEIGHT_OUT(n)           BITFIELD_VAL(SAR_P_BIT14_WEIGHT_OUT, n)

// REG_79
#define SAR_P_BIT15_WEIGHT_OUT_SHIFT        0
#define SAR_P_BIT15_WEIGHT_OUT_MASK         (0x3FFF << SAR_P_BIT15_WEIGHT_OUT_SHIFT)
#define SAR_P_BIT15_WEIGHT_OUT(n)           BITFIELD_VAL(SAR_P_BIT15_WEIGHT_OUT, n)

// REG_7A
#define SAR_P_BIT16_WEIGHT_OUT_SHIFT        0
#define SAR_P_BIT16_WEIGHT_OUT_MASK         (0x3FFF << SAR_P_BIT16_WEIGHT_OUT_SHIFT)
#define SAR_P_BIT16_WEIGHT_OUT(n)           BITFIELD_VAL(SAR_P_BIT16_WEIGHT_OUT, n)

// REG_7B
#define SAR_P_BIT17_WEIGHT_OUT_SHIFT        0
#define SAR_P_BIT17_WEIGHT_OUT_MASK         (0x3FFF << SAR_P_BIT17_WEIGHT_OUT_SHIFT)
#define SAR_P_BIT17_WEIGHT_OUT(n)           BITFIELD_VAL(SAR_P_BIT17_WEIGHT_OUT, n)

// REG_7C
#define SAR_N_BIT05_WEIGHT_OUT_SHIFT        0
#define SAR_N_BIT05_WEIGHT_OUT_MASK         (0x3FFF << SAR_N_BIT05_WEIGHT_OUT_SHIFT)
#define SAR_N_BIT05_WEIGHT_OUT(n)           BITFIELD_VAL(SAR_N_BIT05_WEIGHT_OUT, n)

// REG_7D
#define SAR_N_BIT06_WEIGHT_OUT_SHIFT        0
#define SAR_N_BIT06_WEIGHT_OUT_MASK         (0x3FFF << SAR_N_BIT06_WEIGHT_OUT_SHIFT)
#define SAR_N_BIT06_WEIGHT_OUT(n)           BITFIELD_VAL(SAR_N_BIT06_WEIGHT_OUT, n)

// REG_7E
#define SAR_N_BIT07_WEIGHT_OUT_SHIFT        0
#define SAR_N_BIT07_WEIGHT_OUT_MASK         (0x3FFF << SAR_N_BIT07_WEIGHT_OUT_SHIFT)
#define SAR_N_BIT07_WEIGHT_OUT(n)           BITFIELD_VAL(SAR_N_BIT07_WEIGHT_OUT, n)

// REG_7F
#define SAR_N_BIT08_WEIGHT_OUT_SHIFT        0
#define SAR_N_BIT08_WEIGHT_OUT_MASK         (0x3FFF << SAR_N_BIT08_WEIGHT_OUT_SHIFT)
#define SAR_N_BIT08_WEIGHT_OUT(n)           BITFIELD_VAL(SAR_N_BIT08_WEIGHT_OUT, n)

// REG_80
#define SAR_N_BIT09_WEIGHT_OUT_SHIFT        0
#define SAR_N_BIT09_WEIGHT_OUT_MASK         (0x3FFF << SAR_N_BIT09_WEIGHT_OUT_SHIFT)
#define SAR_N_BIT09_WEIGHT_OUT(n)           BITFIELD_VAL(SAR_N_BIT09_WEIGHT_OUT, n)

// REG_81
#define SAR_N_BIT10_WEIGHT_OUT_SHIFT        0
#define SAR_N_BIT10_WEIGHT_OUT_MASK         (0x3FFF << SAR_N_BIT10_WEIGHT_OUT_SHIFT)
#define SAR_N_BIT10_WEIGHT_OUT(n)           BITFIELD_VAL(SAR_N_BIT10_WEIGHT_OUT, n)

// REG_82
#define SAR_N_BIT11_WEIGHT_OUT_SHIFT        0
#define SAR_N_BIT11_WEIGHT_OUT_MASK         (0x3FFF << SAR_N_BIT11_WEIGHT_OUT_SHIFT)
#define SAR_N_BIT11_WEIGHT_OUT(n)           BITFIELD_VAL(SAR_N_BIT11_WEIGHT_OUT, n)

// REG_83
#define SAR_N_BIT12_WEIGHT_OUT_SHIFT        0
#define SAR_N_BIT12_WEIGHT_OUT_MASK         (0x3FFF << SAR_N_BIT12_WEIGHT_OUT_SHIFT)
#define SAR_N_BIT12_WEIGHT_OUT(n)           BITFIELD_VAL(SAR_N_BIT12_WEIGHT_OUT, n)

// REG_84
#define SAR_N_BIT13_WEIGHT_OUT_SHIFT        0
#define SAR_N_BIT13_WEIGHT_OUT_MASK         (0x3FFF << SAR_N_BIT13_WEIGHT_OUT_SHIFT)
#define SAR_N_BIT13_WEIGHT_OUT(n)           BITFIELD_VAL(SAR_N_BIT13_WEIGHT_OUT, n)

// REG_85
#define SAR_N_BIT14_WEIGHT_OUT_SHIFT        0
#define SAR_N_BIT14_WEIGHT_OUT_MASK         (0x3FFF << SAR_N_BIT14_WEIGHT_OUT_SHIFT)
#define SAR_N_BIT14_WEIGHT_OUT(n)           BITFIELD_VAL(SAR_N_BIT14_WEIGHT_OUT, n)

// REG_86
#define SAR_N_BIT15_WEIGHT_OUT_SHIFT        0
#define SAR_N_BIT15_WEIGHT_OUT_MASK         (0x3FFF << SAR_N_BIT15_WEIGHT_OUT_SHIFT)
#define SAR_N_BIT15_WEIGHT_OUT(n)           BITFIELD_VAL(SAR_N_BIT15_WEIGHT_OUT, n)

// REG_87
#define SAR_N_BIT16_WEIGHT_OUT_SHIFT        0
#define SAR_N_BIT16_WEIGHT_OUT_MASK         (0x3FFF << SAR_N_BIT16_WEIGHT_OUT_SHIFT)
#define SAR_N_BIT16_WEIGHT_OUT(n)           BITFIELD_VAL(SAR_N_BIT16_WEIGHT_OUT, n)

// REG_88
#define SAR_N_BIT17_WEIGHT_OUT_SHIFT        0
#define SAR_N_BIT17_WEIGHT_OUT_MASK         (0x3FFF << SAR_N_BIT17_WEIGHT_OUT_SHIFT)
#define SAR_N_BIT17_WEIGHT_OUT(n)           BITFIELD_VAL(SAR_N_BIT17_WEIGHT_OUT, n)

// REG_89
#define REG_ADCE_OPEN                       (1 << 0)
#define REG_SAR_LP_MODE_DR                  (1 << 1)
#define REG_SAR_LP_MODE                     (1 << 2)
#define REG_R_DET_ACTIVE                    (1 << 3)
#define REG_R_DET_SAR_PU_CH0                (1 << 4)
#define REG_R_DET_SAR_PU_CH1                (1 << 5)

// REG_90
#define REG_SARADC_IN_SIGNED                (1 << 0)
#define REG_SARADC_DOWN_SEL_SHIFT           1
#define REG_SARADC_DOWN_SEL_MASK            (0xF << REG_SARADC_DOWN_SEL_SHIFT)
#define REG_SARADC_DOWN_SEL(n)              BITFIELD_VAL(REG_SARADC_DOWN_SEL, n)
#define REG_SARADC_INN_IIR_CNT_SHIFT        7
#define REG_SARADC_INN_IIR_CNT_MASK         (0x3 << REG_SARADC_INN_IIR_CNT_SHIFT)
#define REG_SARADC_INN_IIR_CNT(n)           BITFIELD_VAL(REG_SARADC_INN_IIR_CNT, n)
#define REG_SARADC_DCF_BYPASS               (1 << 9)
#define REG_SARADC_EN                       (1 << 10)
#define REG_SARADC_DBG_EN                   (1 << 11)

// REG_91
#define REG_SARADC_COEF_UDC_15_0_SHIFT      0
#define REG_SARADC_COEF_UDC_15_0_MASK       (0xFFFF << REG_SARADC_COEF_UDC_15_0_SHIFT)
#define REG_SARADC_COEF_UDC_15_0(n)         BITFIELD_VAL(REG_SARADC_COEF_UDC_15_0, n)

// REG_92
#define REG_SARADC_COEF_UDC_19_16_SHIFT     0
#define REG_SARADC_COEF_UDC_19_16_MASK      (0xF << REG_SARADC_COEF_UDC_19_16_SHIFT)
#define REG_SARADC_COEF_UDC_19_16(n)        BITFIELD_VAL(REG_SARADC_COEF_UDC_19_16, n)

// REG_93
#define REG_SARADC_COEF0_A1_15_0_SHIFT      0
#define REG_SARADC_COEF0_A1_15_0_MASK       (0xFFFF << REG_SARADC_COEF0_A1_15_0_SHIFT)
#define REG_SARADC_COEF0_A1_15_0(n)         BITFIELD_VAL(REG_SARADC_COEF0_A1_15_0, n)

// REG_94
#define REG_SARADC_COEF0_A1_19_16_SHIFT     0
#define REG_SARADC_COEF0_A1_19_16_MASK      (0xF << REG_SARADC_COEF0_A1_19_16_SHIFT)
#define REG_SARADC_COEF0_A1_19_16(n)        BITFIELD_VAL(REG_SARADC_COEF0_A1_19_16, n)

// REG_95
#define REG_SARADC_COEF0_A2_15_0_SHIFT      0
#define REG_SARADC_COEF0_A2_15_0_MASK       (0xFFFF << REG_SARADC_COEF0_A2_15_0_SHIFT)
#define REG_SARADC_COEF0_A2_15_0(n)         BITFIELD_VAL(REG_SARADC_COEF0_A2_15_0, n)

// REG_96
#define REG_SARADC_COEF0_A2_19_16_SHIFT     0
#define REG_SARADC_COEF0_A2_19_16_MASK      (0xF << REG_SARADC_COEF0_A2_19_16_SHIFT)
#define REG_SARADC_COEF0_A2_19_16(n)        BITFIELD_VAL(REG_SARADC_COEF0_A2_19_16, n)

// REG_97
#define REG_SARADC_COEF0_B0_15_0_SHIFT      0
#define REG_SARADC_COEF0_B0_15_0_MASK       (0xFFFF << REG_SARADC_COEF0_B0_15_0_SHIFT)
#define REG_SARADC_COEF0_B0_15_0(n)         BITFIELD_VAL(REG_SARADC_COEF0_B0_15_0, n)

// REG_98
#define REG_SARADC_COEF0_B0_19_16_SHIFT     0
#define REG_SARADC_COEF0_B0_19_16_MASK      (0xF << REG_SARADC_COEF0_B0_19_16_SHIFT)
#define REG_SARADC_COEF0_B0_19_16(n)        BITFIELD_VAL(REG_SARADC_COEF0_B0_19_16, n)

// REG_99
#define REG_SARADC_COEF0_B1_15_0_SHIFT      0
#define REG_SARADC_COEF0_B1_15_0_MASK       (0xFFFF << REG_SARADC_COEF0_B1_15_0_SHIFT)
#define REG_SARADC_COEF0_B1_15_0(n)         BITFIELD_VAL(REG_SARADC_COEF0_B1_15_0, n)

// REG_9A
#define REG_SARADC_COEF0_B1_19_16_SHIFT     0
#define REG_SARADC_COEF0_B1_19_16_MASK      (0xF << REG_SARADC_COEF0_B1_19_16_SHIFT)
#define REG_SARADC_COEF0_B1_19_16(n)        BITFIELD_VAL(REG_SARADC_COEF0_B1_19_16, n)

// REG_9B
#define REG_SARADC_COEF0_B2_15_0_SHIFT      0
#define REG_SARADC_COEF0_B2_15_0_MASK       (0xFFFF << REG_SARADC_COEF0_B2_15_0_SHIFT)
#define REG_SARADC_COEF0_B2_15_0(n)         BITFIELD_VAL(REG_SARADC_COEF0_B2_15_0, n)

// REG_9C
#define REG_SARADC_COEF0_B2_19_16_SHIFT     0
#define REG_SARADC_COEF0_B2_19_16_MASK      (0xF << REG_SARADC_COEF0_B2_19_16_SHIFT)
#define REG_SARADC_COEF0_B2_19_16(n)        BITFIELD_VAL(REG_SARADC_COEF0_B2_19_16, n)

// REG_9D
#define REG_SARADC_COEF1_A1_15_0_SHIFT      0
#define REG_SARADC_COEF1_A1_15_0_MASK       (0xFFFF << REG_SARADC_COEF1_A1_15_0_SHIFT)
#define REG_SARADC_COEF1_A1_15_0(n)         BITFIELD_VAL(REG_SARADC_COEF1_A1_15_0, n)

// REG_9E
#define REG_SARADC_COEF1_A1_19_16_SHIFT     0
#define REG_SARADC_COEF1_A1_19_16_MASK      (0xF << REG_SARADC_COEF1_A1_19_16_SHIFT)
#define REG_SARADC_COEF1_A1_19_16(n)        BITFIELD_VAL(REG_SARADC_COEF1_A1_19_16, n)

// REG_9F
#define REG_SARADC_COEF1_A2_15_0_SHIFT      0
#define REG_SARADC_COEF1_A2_15_0_MASK       (0xFFFF << REG_SARADC_COEF1_A2_15_0_SHIFT)
#define REG_SARADC_COEF1_A2_15_0(n)         BITFIELD_VAL(REG_SARADC_COEF1_A2_15_0, n)

// REG_A0
#define REG_SARADC_COEF1_A2_19_16_SHIFT     0
#define REG_SARADC_COEF1_A2_19_16_MASK      (0xF << REG_SARADC_COEF1_A2_19_16_SHIFT)
#define REG_SARADC_COEF1_A2_19_16(n)        BITFIELD_VAL(REG_SARADC_COEF1_A2_19_16, n)

// REG_A1
#define REG_SARADC_COEF1_B0_15_0_SHIFT      0
#define REG_SARADC_COEF1_B0_15_0_MASK       (0xFFFF << REG_SARADC_COEF1_B0_15_0_SHIFT)
#define REG_SARADC_COEF1_B0_15_0(n)         BITFIELD_VAL(REG_SARADC_COEF1_B0_15_0, n)

// REG_A2
#define REG_SARADC_COEF1_B0_19_16_SHIFT     0
#define REG_SARADC_COEF1_B0_19_16_MASK      (0xF << REG_SARADC_COEF1_B0_19_16_SHIFT)
#define REG_SARADC_COEF1_B0_19_16(n)        BITFIELD_VAL(REG_SARADC_COEF1_B0_19_16, n)

// REG_A3
#define REG_SARADC_COEF1_B1_15_0_SHIFT      0
#define REG_SARADC_COEF1_B1_15_0_MASK       (0xFFFF << REG_SARADC_COEF1_B1_15_0_SHIFT)
#define REG_SARADC_COEF1_B1_15_0(n)         BITFIELD_VAL(REG_SARADC_COEF1_B1_15_0, n)

// REG_A4
#define REG_SARADC_COEF1_B1_19_16_SHIFT     0
#define REG_SARADC_COEF1_B1_19_16_MASK      (0xF << REG_SARADC_COEF1_B1_19_16_SHIFT)
#define REG_SARADC_COEF1_B1_19_16(n)        BITFIELD_VAL(REG_SARADC_COEF1_B1_19_16, n)

// REG_A5
#define REG_SARADC_COEF1_B2_15_0_SHIFT      0
#define REG_SARADC_COEF1_B2_15_0_MASK       (0xFFFF << REG_SARADC_COEF1_B2_15_0_SHIFT)
#define REG_SARADC_COEF1_B2_15_0(n)         BITFIELD_VAL(REG_SARADC_COEF1_B2_15_0, n)

// REG_A6
#define REG_SARADC_COEF1_B2_19_16_SHIFT     0
#define REG_SARADC_COEF1_B2_19_16_MASK      (0xF << REG_SARADC_COEF1_B2_19_16_SHIFT)
#define REG_SARADC_COEF1_B2_19_16(n)        BITFIELD_VAL(REG_SARADC_COEF1_B2_19_16, n)

// REG_A7
#define REG_SARADC_GAIN_15_0_SHIFT          0
#define REG_SARADC_GAIN_15_0_MASK           (0xFFFF << REG_SARADC_GAIN_15_0_SHIFT)
#define REG_SARADC_GAIN_15_0(n)             BITFIELD_VAL(REG_SARADC_GAIN_15_0, n)

// REG_A8
#define REG_SARADC_GAIN_19_16_SHIFT         0
#define REG_SARADC_GAIN_19_16_MASK          (0xF << REG_SARADC_GAIN_19_16_SHIFT)
#define REG_SARADC_GAIN_19_16(n)            BITFIELD_VAL(REG_SARADC_GAIN_19_16, n)
#define REG_SARADC_GAIN_SEL                 (1 << 4)
#define REG_SARADC_GAIN_UPDATE              (1 << 5)
#define REG_CLK_SAR_INT_BYPASS              (1 << 6)

// REG_A9
#define REG_SARADC_DC_DIN_SHIFT             0
#define REG_SARADC_DC_DIN_MASK              (0xFFFF << REG_SARADC_DC_DIN_SHIFT)
#define REG_SARADC_DC_DIN(n)                BITFIELD_VAL(REG_SARADC_DC_DIN, n)

// REG_AA
#define REG_SARADC_DC_UPDATE                (1 << 0)

// REG_AE
#define SAR_TRIG_DOUT_ABS_SHIFT             0
#define SAR_TRIG_DOUT_ABS_MASK              (0xFFFF << SAR_TRIG_DOUT_ABS_SHIFT)
#define SAR_TRIG_DOUT_ABS(n)                BITFIELD_VAL(SAR_TRIG_DOUT_ABS, n)

// REG_101
#define REG_CODEC_ADCA_INPUT_CURRENT_SHIFT  0
#define REG_CODEC_ADCA_INPUT_CURRENT_MASK   (0x7 << REG_CODEC_ADCA_INPUT_CURRENT_SHIFT)
#define REG_CODEC_ADCA_INPUT_CURRENT(n)     BITFIELD_VAL(REG_CODEC_ADCA_INPUT_CURRENT, n)
#define REG_CODEC_RESET_ADCA                (1 << 3)
#define CFG_RESET_ADCA_DR                   (1 << 4)
#define REG_CODEC_IDETA_EN                  (1 << 5)
#define REG_CODEC_ADCA_CLK_SEL              (1 << 6)
#define REG_CODEC_ADCA_DITHER_BIT_EN        (1 << 7)
#define CFG_ADCA_DITHER_CLK_INV             (1 << 8)
#define REG_CODEC_ADCA_GAIN_UPDATE          (1 << 9)
#define REG_CODEC_ADCA_GAIN_BIT_SHIFT       10
#define REG_CODEC_ADCA_GAIN_BIT_MASK        (0x7 << REG_CODEC_ADCA_GAIN_BIT_SHIFT)
#define REG_CODEC_ADCA_GAIN_BIT(n)          BITFIELD_VAL(REG_CODEC_ADCA_GAIN_BIT, n)
#define REG_CODEC_ADCA_PRE_CHARGE           (1 << 13)
#define CFG_PRE_CHARGE_ADCA_DR              (1 << 14)
#define REG_CODEC_ADCA_EN_ZERO_DET          (1 << 15)

// REG_102
#define REG_CODEC_ADCA_RES_2P5K_DR          (1 << 0)
#define REG_CODEC_ADCA_RES_2P5K_UPDATE      (1 << 1)
#define REG_CODEC_ADCA_RES_2P5K             (1 << 2)
#define REG_CODEC_ADCA_CAP_BIT1_SHIFT       3
#define REG_CODEC_ADCA_CAP_BIT1_MASK        (0x7F << REG_CODEC_ADCA_CAP_BIT1_SHIFT)
#define REG_CODEC_ADCA_CAP_BIT1(n)          BITFIELD_VAL(REG_CODEC_ADCA_CAP_BIT1, n)
#define REG_CODEC_ADCA_CAP_BIT2_SHIFT       10
#define REG_CODEC_ADCA_CAP_BIT2_MASK        (0x1F << REG_CODEC_ADCA_CAP_BIT2_SHIFT)
#define REG_CODEC_ADCA_CAP_BIT2(n)          BITFIELD_VAL(REG_CODEC_ADCA_CAP_BIT2, n)
#define REG_RC_OSC_CLK_GATE_EN              (1 << 15)

// REG_103
#define REG_CODEC_ADCA_CAP_BIT3_SHIFT       0
#define REG_CODEC_ADCA_CAP_BIT3_MASK        (0x1F << REG_CODEC_ADCA_CAP_BIT3_SHIFT)
#define REG_CODEC_ADCA_CAP_BIT3(n)          BITFIELD_VAL(REG_CODEC_ADCA_CAP_BIT3, n)
#define REG_CODEC_ADCA_OP1_IBIT_SHIFT       5
#define REG_CODEC_ADCA_OP1_IBIT_MASK        (0x3 << REG_CODEC_ADCA_OP1_IBIT_SHIFT)
#define REG_CODEC_ADCA_OP1_IBIT(n)          BITFIELD_VAL(REG_CODEC_ADCA_OP1_IBIT, n)
#define REG_CODEC_ADCA_OP2_IBIT_SHIFT       7
#define REG_CODEC_ADCA_OP2_IBIT_MASK        (0x3 << REG_CODEC_ADCA_OP2_IBIT_SHIFT)
#define REG_CODEC_ADCA_OP2_IBIT(n)          BITFIELD_VAL(REG_CODEC_ADCA_OP2_IBIT, n)
#define REG_CODEC_ADCA_OP3_IBIT_SHIFT       9
#define REG_CODEC_ADCA_OP3_IBIT_MASK        (0x3 << REG_CODEC_ADCA_OP3_IBIT_SHIFT)
#define REG_CODEC_ADCA_OP3_IBIT(n)          BITFIELD_VAL(REG_CODEC_ADCA_OP3_IBIT, n)
#define REG_CODEC_ADCA_OP4_IBIT_SHIFT       11
#define REG_CODEC_ADCA_OP4_IBIT_MASK        (0x3 << REG_CODEC_ADCA_OP4_IBIT_SHIFT)
#define REG_CODEC_ADCA_OP4_IBIT(n)          BITFIELD_VAL(REG_CODEC_ADCA_OP4_IBIT, n)
#define REG_CODEC_ADCA_REG_VSEL_SHIFT       13
#define REG_CODEC_ADCA_REG_VSEL_MASK        (0x7 << REG_CODEC_ADCA_REG_VSEL_SHIFT)
#define REG_CODEC_ADCA_REG_VSEL(n)          BITFIELD_VAL(REG_CODEC_ADCA_REG_VSEL, n)

// REG_104
#define REG_CODEC_ADCA_IBSEL_OFFSET_SHIFT   0
#define REG_CODEC_ADCA_IBSEL_OFFSET_MASK    (0xF << REG_CODEC_ADCA_IBSEL_OFFSET_SHIFT)
#define REG_CODEC_ADCA_IBSEL_OFFSET(n)      BITFIELD_VAL(REG_CODEC_ADCA_IBSEL_OFFSET, n)
#define REG_CODEC_ADCA_IBSEL_OP1_SHIFT      4
#define REG_CODEC_ADCA_IBSEL_OP1_MASK       (0xF << REG_CODEC_ADCA_IBSEL_OP1_SHIFT)
#define REG_CODEC_ADCA_IBSEL_OP1(n)         BITFIELD_VAL(REG_CODEC_ADCA_IBSEL_OP1, n)
#define REG_CODEC_ADCA_IBSEL_OP2_SHIFT      8
#define REG_CODEC_ADCA_IBSEL_OP2_MASK       (0xF << REG_CODEC_ADCA_IBSEL_OP2_SHIFT)
#define REG_CODEC_ADCA_IBSEL_OP2(n)         BITFIELD_VAL(REG_CODEC_ADCA_IBSEL_OP2, n)
#define REG_CODEC_ADCA_IBSEL_OP3_SHIFT      12
#define REG_CODEC_ADCA_IBSEL_OP3_MASK       (0xF << REG_CODEC_ADCA_IBSEL_OP3_SHIFT)
#define REG_CODEC_ADCA_IBSEL_OP3(n)         BITFIELD_VAL(REG_CODEC_ADCA_IBSEL_OP3, n)

// REG_105
#define REG_CODEC_ADCA_IBSEL_OP4_SHIFT      0
#define REG_CODEC_ADCA_IBSEL_OP4_MASK       (0xF << REG_CODEC_ADCA_IBSEL_OP4_SHIFT)
#define REG_CODEC_ADCA_IBSEL_OP4(n)         BITFIELD_VAL(REG_CODEC_ADCA_IBSEL_OP4, n)
#define REG_CODEC_ADCA_IBSEL_REG_SHIFT      4
#define REG_CODEC_ADCA_IBSEL_REG_MASK       (0xF << REG_CODEC_ADCA_IBSEL_REG_SHIFT)
#define REG_CODEC_ADCA_IBSEL_REG(n)         BITFIELD_VAL(REG_CODEC_ADCA_IBSEL_REG, n)
#define REG_CODEC_ADCA_IBSEL_VCOMP_SHIFT    8
#define REG_CODEC_ADCA_IBSEL_VCOMP_MASK     (0xF << REG_CODEC_ADCA_IBSEL_VCOMP_SHIFT)
#define REG_CODEC_ADCA_IBSEL_VCOMP(n)       BITFIELD_VAL(REG_CODEC_ADCA_IBSEL_VCOMP, n)
#define REG_CODEC_ADCA_IBSEL_VREF_SHIFT     12
#define REG_CODEC_ADCA_IBSEL_VREF_MASK      (0xF << REG_CODEC_ADCA_IBSEL_VREF_SHIFT)
#define REG_CODEC_ADCA_IBSEL_VREF(n)        BITFIELD_VAL(REG_CODEC_ADCA_IBSEL_VREF, n)

// REG_106
#define REG_CODEC_ADCA_IBSEL_VREFBUF_SHIFT  0
#define REG_CODEC_ADCA_IBSEL_VREFBUF_MASK   (0xF << REG_CODEC_ADCA_IBSEL_VREFBUF_SHIFT)
#define REG_CODEC_ADCA_IBSEL_VREFBUF(n)     BITFIELD_VAL(REG_CODEC_ADCA_IBSEL_VREFBUF, n)
#define REG_CODEC_ADCA_IBSEL_IDAC2_SHIFT    4
#define REG_CODEC_ADCA_IBSEL_IDAC2_MASK     (0xF << REG_CODEC_ADCA_IBSEL_IDAC2_SHIFT)
#define REG_CODEC_ADCA_IBSEL_IDAC2(n)       BITFIELD_VAL(REG_CODEC_ADCA_IBSEL_IDAC2, n)
#define REG_CODEC_ADCA_OFFSET_CAL_S_SHIFT   8
#define REG_CODEC_ADCA_OFFSET_CAL_S_MASK    (0x1F << REG_CODEC_ADCA_OFFSET_CAL_S_SHIFT)
#define REG_CODEC_ADCA_OFFSET_CAL_S(n)      BITFIELD_VAL(REG_CODEC_ADCA_OFFSET_CAL_S, n)
#define REG_CODEC_ADCA_OFFSET_CURRENT_EN    (1 << 13)
#define REG_CODEC_ADCA_OFFSET_CURRENT_SEL   (1 << 14)
#define REG_CODEC_ADCA_OFFSET_SW_EN         (1 << 15)

// REG_107
#define REG_CODEC_ADCA_OFFSET_BIT_SHIFT     0
#define REG_CODEC_ADCA_OFFSET_BIT_MASK      (0x3FFF << REG_CODEC_ADCA_OFFSET_BIT_SHIFT)
#define REG_CODEC_ADCA_OFFSET_BIT(n)        BITFIELD_VAL(REG_CODEC_ADCA_OFFSET_BIT, n)
#define REG_CODEC_ADCA_DVDD_SEL             (1 << 14)
#define REG_CODEC_ADCA_PU_REG               (1 << 15)

// REG_108
#define REG_CODEC_ADCA_VREF_SEL_SHIFT       0
#define REG_CODEC_ADCA_VREF_SEL_MASK        (0xF << REG_CODEC_ADCA_VREF_SEL_SHIFT)
#define REG_CODEC_ADCA_VREF_SEL(n)          BITFIELD_VAL(REG_CODEC_ADCA_VREF_SEL, n)
#define REG_CODEC_ADCA_VREFBUF_BIT_SHIFT    4
#define REG_CODEC_ADCA_VREFBUF_BIT_MASK     (0xF << REG_CODEC_ADCA_VREFBUF_BIT_SHIFT)
#define REG_CODEC_ADCA_VREFBUF_BIT(n)       BITFIELD_VAL(REG_CODEC_ADCA_VREFBUF_BIT, n)
#define ADCA_TIMER_RSTN_DLY_SHIFT           8
#define ADCA_TIMER_RSTN_DLY_MASK            (0x3F << ADCA_TIMER_RSTN_DLY_SHIFT)
#define ADCA_TIMER_RSTN_DLY(n)              BITFIELD_VAL(ADCA_TIMER_RSTN_DLY, n)
#define REG_CODEC_ADCA_BYPASS_VREFBUFFER    (1 << 14)
#define REG_CODEC_ADCA_IBP25U_SEL           (1 << 15)

// REG_109
#define REG_CODEC_ADCA_CH_SEL_INT_SHIFT     0
#define REG_CODEC_ADCA_CH_SEL_INT_MASK      (0xF << REG_CODEC_ADCA_CH_SEL_INT_SHIFT)
#define REG_CODEC_ADCA_CH_SEL_INT(n)        BITFIELD_VAL(REG_CODEC_ADCA_CH_SEL_INT, n)
#define ADCA_PRE_CHARGE_TIMER_DLY_SHIFT     4
#define ADCA_PRE_CHARGE_TIMER_DLY_MASK      (0xFF << ADCA_PRE_CHARGE_TIMER_DLY_SHIFT)
#define ADCA_PRE_CHARGE_TIMER_DLY(n)        BITFIELD_VAL(ADCA_PRE_CHARGE_TIMER_DLY, n)
#define REG_CODEC_ADCA_HP_MODE              (1 << 12)
#define REG_CODEC_ADCA_OP1_HP_MODE          (1 << 15)

// REG_10A
#define REG_CODEC_ADCA_CAL_MODE             (1 << 0)
#define REG_CODEC_ADCA_EN_DITHER_DAC1       (1 << 1)
#define REG_CODEC_ADCA_EN_DITHER_DAC2       (1 << 2)
#define REG_CODEC_ADCA_EN_N_DET             (1 << 3)
#define REG_CODEC_ADCA_EN_P_DET             (1 << 4)
#define REG_CODEC_ADCA_RDAC_DITHER_CAL_SHIFT 5
#define REG_CODEC_ADCA_RDAC_DITHER_CAL_MASK (0xFF << REG_CODEC_ADCA_RDAC_DITHER_CAL_SHIFT)
#define REG_CODEC_ADCA_RDAC_DITHER_CAL(n)   BITFIELD_VAL(REG_CODEC_ADCA_RDAC_DITHER_CAL, n)

// REG_10B
#define REG_CODEC_ADCA_NRES_VREF_SEL1_SHIFT 0
#define REG_CODEC_ADCA_NRES_VREF_SEL1_MASK  (0xF << REG_CODEC_ADCA_NRES_VREF_SEL1_SHIFT)
#define REG_CODEC_ADCA_NRES_VREF_SEL1(n)    BITFIELD_VAL(REG_CODEC_ADCA_NRES_VREF_SEL1, n)
#define REG_CODEC_ADCA_NRES_VREF_SEL2_SHIFT 4
#define REG_CODEC_ADCA_NRES_VREF_SEL2_MASK  (0xF << REG_CODEC_ADCA_NRES_VREF_SEL2_SHIFT)
#define REG_CODEC_ADCA_NRES_VREF_SEL2(n)    BITFIELD_VAL(REG_CODEC_ADCA_NRES_VREF_SEL2, n)
#define REG_CODEC_ADCA_PRES_VREF_SEL1_SHIFT 8
#define REG_CODEC_ADCA_PRES_VREF_SEL1_MASK  (0xF << REG_CODEC_ADCA_PRES_VREF_SEL1_SHIFT)
#define REG_CODEC_ADCA_PRES_VREF_SEL1(n)    BITFIELD_VAL(REG_CODEC_ADCA_PRES_VREF_SEL1, n)
#define REG_CODEC_ADCA_PRES_VREF_SEL2_SHIFT 12
#define REG_CODEC_ADCA_PRES_VREF_SEL2_MASK  (0xF << REG_CODEC_ADCA_PRES_VREF_SEL2_SHIFT)
#define REG_CODEC_ADCA_PRES_VREF_SEL2(n)    BITFIELD_VAL(REG_CODEC_ADCA_PRES_VREF_SEL2, n)

// REG_10C
#define REG_CODEC_ADCA_DITHER_DR            (1 << 0)
#define REG_CODEC_ADCA_DITHER_MGEN_SHIFT    1
#define REG_CODEC_ADCA_DITHER_MGEN_MASK     (0x7 << REG_CODEC_ADCA_DITHER_MGEN_SHIFT)
#define REG_CODEC_ADCA_DITHER_MGEN(n)       BITFIELD_VAL(REG_CODEC_ADCA_DITHER_MGEN, n)
#define REG_CODEC_EN_ADCA                   (1 << 4)
#define REG_CODEC_ADCA_CHANNEL_SEL_SHIFT    5
#define REG_CODEC_ADCA_CHANNEL_SEL_MASK     (0xF << REG_CODEC_ADCA_CHANNEL_SEL_SHIFT)
#define REG_CODEC_ADCA_CHANNEL_SEL(n)       BITFIELD_VAL(REG_CODEC_ADCA_CHANNEL_SEL, n)

// REG_111
#define REG_CODEC_ADCB_INPUT_CURRENT_SHIFT  0
#define REG_CODEC_ADCB_INPUT_CURRENT_MASK   (0x7 << REG_CODEC_ADCB_INPUT_CURRENT_SHIFT)
#define REG_CODEC_ADCB_INPUT_CURRENT(n)     BITFIELD_VAL(REG_CODEC_ADCB_INPUT_CURRENT, n)
#define REG_CODEC_RESET_ADCB                (1 << 3)
#define CFG_RESET_ADCB_DR                   (1 << 4)
#define REG_CODEC_IDETB_EN                  (1 << 5)
#define REG_CODEC_ADCB_CLK_SEL              (1 << 6)
#define REG_CODEC_ADCB_DITHER_BIT_EN        (1 << 7)
#define CFG_ADCB_DITHER_CLK_INV             (1 << 8)
#define REG_CODEC_ADCB_GAIN_UPDATE          (1 << 9)
#define REG_CODEC_ADCB_GAIN_BIT_SHIFT       10
#define REG_CODEC_ADCB_GAIN_BIT_MASK        (0x7 << REG_CODEC_ADCB_GAIN_BIT_SHIFT)
#define REG_CODEC_ADCB_GAIN_BIT(n)          BITFIELD_VAL(REG_CODEC_ADCB_GAIN_BIT, n)
#define REG_CODEC_ADCB_PRE_CHARGE           (1 << 13)
#define CFG_PRE_CHARGE_ADCB_DR              (1 << 14)
#define REG_CODEC_ADCB_EN_ZERO_DET          (1 << 15)

// REG_112
#define REG_CODEC_ADCB_RES_2P5K_DR          (1 << 0)
#define REG_CODEC_ADCB_RES_2P5K_UPDATE      (1 << 1)
#define REG_CODEC_ADCB_RES_2P5K             (1 << 2)
#define REG_CODEC_ADCB_CAP_BIT1_SHIFT       3
#define REG_CODEC_ADCB_CAP_BIT1_MASK        (0x7F << REG_CODEC_ADCB_CAP_BIT1_SHIFT)
#define REG_CODEC_ADCB_CAP_BIT1(n)          BITFIELD_VAL(REG_CODEC_ADCB_CAP_BIT1, n)
#define REG_CODEC_ADCB_CAP_BIT2_SHIFT       10
#define REG_CODEC_ADCB_CAP_BIT2_MASK        (0x1F << REG_CODEC_ADCB_CAP_BIT2_SHIFT)
#define REG_CODEC_ADCB_CAP_BIT2(n)          BITFIELD_VAL(REG_CODEC_ADCB_CAP_BIT2, n)

// REG_113
#define REG_CODEC_ADCB_CAP_BIT3_SHIFT       0
#define REG_CODEC_ADCB_CAP_BIT3_MASK        (0x1F << REG_CODEC_ADCB_CAP_BIT3_SHIFT)
#define REG_CODEC_ADCB_CAP_BIT3(n)          BITFIELD_VAL(REG_CODEC_ADCB_CAP_BIT3, n)
#define REG_CODEC_ADCB_OP1_IBIT_SHIFT       5
#define REG_CODEC_ADCB_OP1_IBIT_MASK        (0x3 << REG_CODEC_ADCB_OP1_IBIT_SHIFT)
#define REG_CODEC_ADCB_OP1_IBIT(n)          BITFIELD_VAL(REG_CODEC_ADCB_OP1_IBIT, n)
#define REG_CODEC_ADCB_OP2_IBIT_SHIFT       7
#define REG_CODEC_ADCB_OP2_IBIT_MASK        (0x3 << REG_CODEC_ADCB_OP2_IBIT_SHIFT)
#define REG_CODEC_ADCB_OP2_IBIT(n)          BITFIELD_VAL(REG_CODEC_ADCB_OP2_IBIT, n)
#define REG_CODEC_ADCB_OP3_IBIT_SHIFT       9
#define REG_CODEC_ADCB_OP3_IBIT_MASK        (0x3 << REG_CODEC_ADCB_OP3_IBIT_SHIFT)
#define REG_CODEC_ADCB_OP3_IBIT(n)          BITFIELD_VAL(REG_CODEC_ADCB_OP3_IBIT, n)
#define REG_CODEC_ADCB_OP4_IBIT_SHIFT       11
#define REG_CODEC_ADCB_OP4_IBIT_MASK        (0x3 << REG_CODEC_ADCB_OP4_IBIT_SHIFT)
#define REG_CODEC_ADCB_OP4_IBIT(n)          BITFIELD_VAL(REG_CODEC_ADCB_OP4_IBIT, n)
#define REG_CODEC_ADCB_REG_VSEL_SHIFT       13
#define REG_CODEC_ADCB_REG_VSEL_MASK        (0x7 << REG_CODEC_ADCB_REG_VSEL_SHIFT)
#define REG_CODEC_ADCB_REG_VSEL(n)          BITFIELD_VAL(REG_CODEC_ADCB_REG_VSEL, n)

// REG_114
#define REG_CODEC_ADCB_IBSEL_OFFSET_SHIFT   0
#define REG_CODEC_ADCB_IBSEL_OFFSET_MASK    (0xF << REG_CODEC_ADCB_IBSEL_OFFSET_SHIFT)
#define REG_CODEC_ADCB_IBSEL_OFFSET(n)      BITFIELD_VAL(REG_CODEC_ADCB_IBSEL_OFFSET, n)
#define REG_CODEC_ADCB_IBSEL_OP1_SHIFT      4
#define REG_CODEC_ADCB_IBSEL_OP1_MASK       (0xF << REG_CODEC_ADCB_IBSEL_OP1_SHIFT)
#define REG_CODEC_ADCB_IBSEL_OP1(n)         BITFIELD_VAL(REG_CODEC_ADCB_IBSEL_OP1, n)
#define REG_CODEC_ADCB_IBSEL_OP2_SHIFT      8
#define REG_CODEC_ADCB_IBSEL_OP2_MASK       (0xF << REG_CODEC_ADCB_IBSEL_OP2_SHIFT)
#define REG_CODEC_ADCB_IBSEL_OP2(n)         BITFIELD_VAL(REG_CODEC_ADCB_IBSEL_OP2, n)
#define REG_CODEC_ADCB_IBSEL_OP3_SHIFT      12
#define REG_CODEC_ADCB_IBSEL_OP3_MASK       (0xF << REG_CODEC_ADCB_IBSEL_OP3_SHIFT)
#define REG_CODEC_ADCB_IBSEL_OP3(n)         BITFIELD_VAL(REG_CODEC_ADCB_IBSEL_OP3, n)

// REG_115
#define REG_CODEC_ADCB_IBSEL_OP4_SHIFT      0
#define REG_CODEC_ADCB_IBSEL_OP4_MASK       (0xF << REG_CODEC_ADCB_IBSEL_OP4_SHIFT)
#define REG_CODEC_ADCB_IBSEL_OP4(n)         BITFIELD_VAL(REG_CODEC_ADCB_IBSEL_OP4, n)
#define REG_CODEC_ADCB_IBSEL_REG_SHIFT      4
#define REG_CODEC_ADCB_IBSEL_REG_MASK       (0xF << REG_CODEC_ADCB_IBSEL_REG_SHIFT)
#define REG_CODEC_ADCB_IBSEL_REG(n)         BITFIELD_VAL(REG_CODEC_ADCB_IBSEL_REG, n)
#define REG_CODEC_ADCB_IBSEL_VCOMP_SHIFT    8
#define REG_CODEC_ADCB_IBSEL_VCOMP_MASK     (0xF << REG_CODEC_ADCB_IBSEL_VCOMP_SHIFT)
#define REG_CODEC_ADCB_IBSEL_VCOMP(n)       BITFIELD_VAL(REG_CODEC_ADCB_IBSEL_VCOMP, n)
#define REG_CODEC_ADCB_IBSEL_VREF_SHIFT     12
#define REG_CODEC_ADCB_IBSEL_VREF_MASK      (0xF << REG_CODEC_ADCB_IBSEL_VREF_SHIFT)
#define REG_CODEC_ADCB_IBSEL_VREF(n)        BITFIELD_VAL(REG_CODEC_ADCB_IBSEL_VREF, n)

// REG_116
#define REG_CODEC_ADCB_IBSEL_VREFBUF_SHIFT  0
#define REG_CODEC_ADCB_IBSEL_VREFBUF_MASK   (0xF << REG_CODEC_ADCB_IBSEL_VREFBUF_SHIFT)
#define REG_CODEC_ADCB_IBSEL_VREFBUF(n)     BITFIELD_VAL(REG_CODEC_ADCB_IBSEL_VREFBUF, n)
#define REG_CODEC_ADCB_IBSEL_IDAC2_SHIFT    4
#define REG_CODEC_ADCB_IBSEL_IDAC2_MASK     (0xF << REG_CODEC_ADCB_IBSEL_IDAC2_SHIFT)
#define REG_CODEC_ADCB_IBSEL_IDAC2(n)       BITFIELD_VAL(REG_CODEC_ADCB_IBSEL_IDAC2, n)
#define REG_CODEC_ADCB_OFFSET_CAL_S_SHIFT   8
#define REG_CODEC_ADCB_OFFSET_CAL_S_MASK    (0x1F << REG_CODEC_ADCB_OFFSET_CAL_S_SHIFT)
#define REG_CODEC_ADCB_OFFSET_CAL_S(n)      BITFIELD_VAL(REG_CODEC_ADCB_OFFSET_CAL_S, n)
#define REG_CODEC_ADCB_OFFSET_CURRENT_EN    (1 << 13)
#define REG_CODEC_ADCB_OFFSET_CURRENT_SEL   (1 << 14)
#define REG_CODEC_ADCB_OFFSET_SW_EN         (1 << 15)

// REG_117
#define REG_CODEC_ADCB_OFFSET_BIT_SHIFT     0
#define REG_CODEC_ADCB_OFFSET_BIT_MASK      (0x3FFF << REG_CODEC_ADCB_OFFSET_BIT_SHIFT)
#define REG_CODEC_ADCB_OFFSET_BIT(n)        BITFIELD_VAL(REG_CODEC_ADCB_OFFSET_BIT, n)
#define REG_CODEC_ADCB_DVDD_SEL             (1 << 14)
#define REG_CODEC_ADCB_PU_REG               (1 << 15)

// REG_118
#define REG_CODEC_ADCB_VREF_SEL_SHIFT       0
#define REG_CODEC_ADCB_VREF_SEL_MASK        (0xF << REG_CODEC_ADCB_VREF_SEL_SHIFT)
#define REG_CODEC_ADCB_VREF_SEL(n)          BITFIELD_VAL(REG_CODEC_ADCB_VREF_SEL, n)
#define REG_CODEC_ADCB_VREFBUF_BIT_SHIFT    4
#define REG_CODEC_ADCB_VREFBUF_BIT_MASK     (0xF << REG_CODEC_ADCB_VREFBUF_BIT_SHIFT)
#define REG_CODEC_ADCB_VREFBUF_BIT(n)       BITFIELD_VAL(REG_CODEC_ADCB_VREFBUF_BIT, n)
#define ADCB_TIMER_RSTN_DLY_SHIFT           8
#define ADCB_TIMER_RSTN_DLY_MASK            (0x3F << ADCB_TIMER_RSTN_DLY_SHIFT)
#define ADCB_TIMER_RSTN_DLY(n)              BITFIELD_VAL(ADCB_TIMER_RSTN_DLY, n)
#define REG_CODEC_ADCB_BYPASS_VREFBUFFER    (1 << 14)
#define REG_CODEC_ADCB_IBP25U_SEL           (1 << 15)

// REG_119
#define REG_CODEC_ADCB_CH_SEL_SHIFT         0
#define REG_CODEC_ADCB_CH_SEL_MASK          (0xF << REG_CODEC_ADCB_CH_SEL_SHIFT)
#define REG_CODEC_ADCB_CH_SEL(n)            BITFIELD_VAL(REG_CODEC_ADCB_CH_SEL, n)
#define ADCB_PRE_CHARGE_TIMER_DLY_SHIFT     4
#define ADCB_PRE_CHARGE_TIMER_DLY_MASK      (0xFF << ADCB_PRE_CHARGE_TIMER_DLY_SHIFT)
#define ADCB_PRE_CHARGE_TIMER_DLY(n)        BITFIELD_VAL(ADCB_PRE_CHARGE_TIMER_DLY, n)
#define REG_CODEC_ADCB_HP_MODE              (1 << 12)
#define REG_CODEC_ADCB_OP1_HP_MODE          (1 << 15)

// REG_11A
#define REG_CODEC_ADCB_CAL_MODE             (1 << 0)
#define REG_CODEC_ADCB_EN_DITHER_DAC1       (1 << 1)
#define REG_CODEC_ADCB_EN_DITHER_DAC2       (1 << 2)
#define REG_CODEC_ADCB_EN_N_DET             (1 << 3)
#define REG_CODEC_ADCB_EN_P_DET             (1 << 4)
#define REG_CODEC_ADCB_RDAC_DITHER_CAL_SHIFT 5
#define REG_CODEC_ADCB_RDAC_DITHER_CAL_MASK (0xFF << REG_CODEC_ADCB_RDAC_DITHER_CAL_SHIFT)
#define REG_CODEC_ADCB_RDAC_DITHER_CAL(n)   BITFIELD_VAL(REG_CODEC_ADCB_RDAC_DITHER_CAL, n)

// REG_11B
#define REG_CODEC_ADCB_NRES_VREF_SEL1_SHIFT 0
#define REG_CODEC_ADCB_NRES_VREF_SEL1_MASK  (0xF << REG_CODEC_ADCB_NRES_VREF_SEL1_SHIFT)
#define REG_CODEC_ADCB_NRES_VREF_SEL1(n)    BITFIELD_VAL(REG_CODEC_ADCB_NRES_VREF_SEL1, n)
#define REG_CODEC_ADCB_NRES_VREF_SEL2_SHIFT 4
#define REG_CODEC_ADCB_NRES_VREF_SEL2_MASK  (0xF << REG_CODEC_ADCB_NRES_VREF_SEL2_SHIFT)
#define REG_CODEC_ADCB_NRES_VREF_SEL2(n)    BITFIELD_VAL(REG_CODEC_ADCB_NRES_VREF_SEL2, n)
#define REG_CODEC_ADCB_PRES_VREF_SEL1_SHIFT 8
#define REG_CODEC_ADCB_PRES_VREF_SEL1_MASK  (0xF << REG_CODEC_ADCB_PRES_VREF_SEL1_SHIFT)
#define REG_CODEC_ADCB_PRES_VREF_SEL1(n)    BITFIELD_VAL(REG_CODEC_ADCB_PRES_VREF_SEL1, n)
#define REG_CODEC_ADCB_PRES_VREF_SEL2_SHIFT 12
#define REG_CODEC_ADCB_PRES_VREF_SEL2_MASK  (0xF << REG_CODEC_ADCB_PRES_VREF_SEL2_SHIFT)
#define REG_CODEC_ADCB_PRES_VREF_SEL2(n)    BITFIELD_VAL(REG_CODEC_ADCB_PRES_VREF_SEL2, n)

// REG_11C
#define REG_CODEC_ADCB_DITHER_DR            (1 << 0)
#define REG_CODEC_ADCB_DITHER_MGEN_SHIFT    1
#define REG_CODEC_ADCB_DITHER_MGEN_MASK     (0x7 << REG_CODEC_ADCB_DITHER_MGEN_SHIFT)
#define REG_CODEC_ADCB_DITHER_MGEN(n)       BITFIELD_VAL(REG_CODEC_ADCB_DITHER_MGEN, n)
#define REG_CODEC_EN_ADCB                   (1 << 4)
#define REG_CODEC_ADCB_CHANNEL_SEL_SHIFT    5
#define REG_CODEC_ADCB_CHANNEL_SEL_MASK     (0xF << REG_CODEC_ADCB_CHANNEL_SEL_SHIFT)
#define REG_CODEC_ADCB_CHANNEL_SEL(n)       BITFIELD_VAL(REG_CODEC_ADCB_CHANNEL_SEL, n)

// REG_121
#define REG_CODEC_ADCC_INPUT_CURRENT_SHIFT  0
#define REG_CODEC_ADCC_INPUT_CURRENT_MASK   (0x7 << REG_CODEC_ADCC_INPUT_CURRENT_SHIFT)
#define REG_CODEC_ADCC_INPUT_CURRENT(n)     BITFIELD_VAL(REG_CODEC_ADCC_INPUT_CURRENT, n)
#define REG_CODEC_RESET_ADCC                (1 << 3)
#define CFG_RESET_ADCC_DR                   (1 << 4)
#define REG_CODEC_IDETC_EN                  (1 << 5)
#define REG_CODEC_ADCC_CLK_SEL              (1 << 6)
#define REG_CODEC_ADCC_DITHER_BIT_EN        (1 << 7)
#define CFG_ADCC_DITHER_CLK_INV             (1 << 8)
#define REG_CODEC_ADCC_GAIN_UPDATE          (1 << 9)
#define REG_CODEC_ADCC_GAIN_BIT_SHIFT       10
#define REG_CODEC_ADCC_GAIN_BIT_MASK        (0x7 << REG_CODEC_ADCC_GAIN_BIT_SHIFT)
#define REG_CODEC_ADCC_GAIN_BIT(n)          BITFIELD_VAL(REG_CODEC_ADCC_GAIN_BIT, n)
#define REG_CODEC_ADCC_PRE_CHARGE           (1 << 13)
#define CFG_PRE_CHARGE_ADCC_DR              (1 << 14)
#define REG_CODEC_ADCC_EN_ZERO_DET          (1 << 15)

// REG_122
#define REG_CODEC_ADCC_RES_2P5K_DR          (1 << 0)
#define REG_CODEC_ADCC_RES_2P5K_UPDATE      (1 << 1)
#define REG_CODEC_ADCC_RES_2P5K             (1 << 2)
#define REG_CODEC_ADCC_CAP_BIT1_SHIFT       3
#define REG_CODEC_ADCC_CAP_BIT1_MASK        (0x7F << REG_CODEC_ADCC_CAP_BIT1_SHIFT)
#define REG_CODEC_ADCC_CAP_BIT1(n)          BITFIELD_VAL(REG_CODEC_ADCC_CAP_BIT1, n)
#define REG_CODEC_ADCC_CAP_BIT2_SHIFT       10
#define REG_CODEC_ADCC_CAP_BIT2_MASK        (0x1F << REG_CODEC_ADCC_CAP_BIT2_SHIFT)
#define REG_CODEC_ADCC_CAP_BIT2(n)          BITFIELD_VAL(REG_CODEC_ADCC_CAP_BIT2, n)

// REG_123
#define REG_CODEC_ADCC_CAP_BIT3_SHIFT       0
#define REG_CODEC_ADCC_CAP_BIT3_MASK        (0x1F << REG_CODEC_ADCC_CAP_BIT3_SHIFT)
#define REG_CODEC_ADCC_CAP_BIT3(n)          BITFIELD_VAL(REG_CODEC_ADCC_CAP_BIT3, n)
#define REG_CODEC_ADCC_OP1_IBIT_SHIFT       5
#define REG_CODEC_ADCC_OP1_IBIT_MASK        (0x3 << REG_CODEC_ADCC_OP1_IBIT_SHIFT)
#define REG_CODEC_ADCC_OP1_IBIT(n)          BITFIELD_VAL(REG_CODEC_ADCC_OP1_IBIT, n)
#define REG_CODEC_ADCC_OP2_IBIT_SHIFT       7
#define REG_CODEC_ADCC_OP2_IBIT_MASK        (0x3 << REG_CODEC_ADCC_OP2_IBIT_SHIFT)
#define REG_CODEC_ADCC_OP2_IBIT(n)          BITFIELD_VAL(REG_CODEC_ADCC_OP2_IBIT, n)
#define REG_CODEC_ADCC_OP3_IBIT_SHIFT       9
#define REG_CODEC_ADCC_OP3_IBIT_MASK        (0x3 << REG_CODEC_ADCC_OP3_IBIT_SHIFT)
#define REG_CODEC_ADCC_OP3_IBIT(n)          BITFIELD_VAL(REG_CODEC_ADCC_OP3_IBIT, n)
#define REG_CODEC_ADCC_OP4_IBIT_SHIFT       11
#define REG_CODEC_ADCC_OP4_IBIT_MASK        (0x3 << REG_CODEC_ADCC_OP4_IBIT_SHIFT)
#define REG_CODEC_ADCC_OP4_IBIT(n)          BITFIELD_VAL(REG_CODEC_ADCC_OP4_IBIT, n)
#define REG_CODEC_ADCC_REG_VSEL_SHIFT       13
#define REG_CODEC_ADCC_REG_VSEL_MASK        (0x7 << REG_CODEC_ADCC_REG_VSEL_SHIFT)
#define REG_CODEC_ADCC_REG_VSEL(n)          BITFIELD_VAL(REG_CODEC_ADCC_REG_VSEL, n)

// REG_124
#define REG_CODEC_ADCC_IBSEL_OFFSET_SHIFT   0
#define REG_CODEC_ADCC_IBSEL_OFFSET_MASK    (0xF << REG_CODEC_ADCC_IBSEL_OFFSET_SHIFT)
#define REG_CODEC_ADCC_IBSEL_OFFSET(n)      BITFIELD_VAL(REG_CODEC_ADCC_IBSEL_OFFSET, n)
#define REG_CODEC_ADCC_IBSEL_OP1_SHIFT      4
#define REG_CODEC_ADCC_IBSEL_OP1_MASK       (0xF << REG_CODEC_ADCC_IBSEL_OP1_SHIFT)
#define REG_CODEC_ADCC_IBSEL_OP1(n)         BITFIELD_VAL(REG_CODEC_ADCC_IBSEL_OP1, n)
#define REG_CODEC_ADCC_IBSEL_OP2_SHIFT      8
#define REG_CODEC_ADCC_IBSEL_OP2_MASK       (0xF << REG_CODEC_ADCC_IBSEL_OP2_SHIFT)
#define REG_CODEC_ADCC_IBSEL_OP2(n)         BITFIELD_VAL(REG_CODEC_ADCC_IBSEL_OP2, n)
#define REG_CODEC_ADCC_IBSEL_OP3_SHIFT      12
#define REG_CODEC_ADCC_IBSEL_OP3_MASK       (0xF << REG_CODEC_ADCC_IBSEL_OP3_SHIFT)
#define REG_CODEC_ADCC_IBSEL_OP3(n)         BITFIELD_VAL(REG_CODEC_ADCC_IBSEL_OP3, n)

// REG_125
#define REG_CODEC_ADCC_IBSEL_OP4_SHIFT      0
#define REG_CODEC_ADCC_IBSEL_OP4_MASK       (0xF << REG_CODEC_ADCC_IBSEL_OP4_SHIFT)
#define REG_CODEC_ADCC_IBSEL_OP4(n)         BITFIELD_VAL(REG_CODEC_ADCC_IBSEL_OP4, n)
#define REG_CODEC_ADCC_IBSEL_REG_SHIFT      4
#define REG_CODEC_ADCC_IBSEL_REG_MASK       (0xF << REG_CODEC_ADCC_IBSEL_REG_SHIFT)
#define REG_CODEC_ADCC_IBSEL_REG(n)         BITFIELD_VAL(REG_CODEC_ADCC_IBSEL_REG, n)
#define REG_CODEC_ADCC_IBSEL_VCOMP_SHIFT    8
#define REG_CODEC_ADCC_IBSEL_VCOMP_MASK     (0xF << REG_CODEC_ADCC_IBSEL_VCOMP_SHIFT)
#define REG_CODEC_ADCC_IBSEL_VCOMP(n)       BITFIELD_VAL(REG_CODEC_ADCC_IBSEL_VCOMP, n)
#define REG_CODEC_ADCC_IBSEL_VREF_SHIFT     12
#define REG_CODEC_ADCC_IBSEL_VREF_MASK      (0xF << REG_CODEC_ADCC_IBSEL_VREF_SHIFT)
#define REG_CODEC_ADCC_IBSEL_VREF(n)        BITFIELD_VAL(REG_CODEC_ADCC_IBSEL_VREF, n)

// REG_126
#define REG_CODEC_ADCC_IBSEL_VREFBUF_SHIFT  0
#define REG_CODEC_ADCC_IBSEL_VREFBUF_MASK   (0xF << REG_CODEC_ADCC_IBSEL_VREFBUF_SHIFT)
#define REG_CODEC_ADCC_IBSEL_VREFBUF(n)     BITFIELD_VAL(REG_CODEC_ADCC_IBSEL_VREFBUF, n)
#define REG_CODEC_ADCC_IBSEL_IDAC2_SHIFT    4
#define REG_CODEC_ADCC_IBSEL_IDAC2_MASK     (0xF << REG_CODEC_ADCC_IBSEL_IDAC2_SHIFT)
#define REG_CODEC_ADCC_IBSEL_IDAC2(n)       BITFIELD_VAL(REG_CODEC_ADCC_IBSEL_IDAC2, n)
#define REG_CODEC_ADCC_OFFSET_CAL_S_SHIFT   8
#define REG_CODEC_ADCC_OFFSET_CAL_S_MASK    (0x1F << REG_CODEC_ADCC_OFFSET_CAL_S_SHIFT)
#define REG_CODEC_ADCC_OFFSET_CAL_S(n)      BITFIELD_VAL(REG_CODEC_ADCC_OFFSET_CAL_S, n)
#define REG_CODEC_ADCC_OFFSET_CURRENT_EN    (1 << 13)
#define REG_CODEC_ADCC_OFFSET_CURRENT_SEL   (1 << 14)
#define REG_CODEC_ADCC_OFFSET_SW_EN         (1 << 15)

// REG_127
#define REG_CODEC_ADCC_OFFSET_BIT_SHIFT     0
#define REG_CODEC_ADCC_OFFSET_BIT_MASK      (0x3FFF << REG_CODEC_ADCC_OFFSET_BIT_SHIFT)
#define REG_CODEC_ADCC_OFFSET_BIT(n)        BITFIELD_VAL(REG_CODEC_ADCC_OFFSET_BIT, n)
#define REG_CODEC_ADCC_DVDD_SEL             (1 << 14)
#define REG_CODEC_ADCC_PU_REG               (1 << 15)

// REG_128
#define REG_CODEC_ADCC_VREF_SEL_SHIFT       0
#define REG_CODEC_ADCC_VREF_SEL_MASK        (0xF << REG_CODEC_ADCC_VREF_SEL_SHIFT)
#define REG_CODEC_ADCC_VREF_SEL(n)          BITFIELD_VAL(REG_CODEC_ADCC_VREF_SEL, n)
#define REG_CODEC_ADCC_VREFBUF_BIT_SHIFT    4
#define REG_CODEC_ADCC_VREFBUF_BIT_MASK     (0xF << REG_CODEC_ADCC_VREFBUF_BIT_SHIFT)
#define REG_CODEC_ADCC_VREFBUF_BIT(n)       BITFIELD_VAL(REG_CODEC_ADCC_VREFBUF_BIT, n)
#define ADCC_TIMER_RSTN_DLY_SHIFT           8
#define ADCC_TIMER_RSTN_DLY_MASK            (0x3F << ADCC_TIMER_RSTN_DLY_SHIFT)
#define ADCC_TIMER_RSTN_DLY(n)              BITFIELD_VAL(ADCC_TIMER_RSTN_DLY, n)
#define REG_CODEC_ADCC_BYPASS_VREFBUFFER    (1 << 14)
#define REG_CODEC_ADCC_IBP25U_SEL           (1 << 15)

// REG_129
#define REG_CODEC_ADCC_CH_SEL_SHIFT         0
#define REG_CODEC_ADCC_CH_SEL_MASK          (0xF << REG_CODEC_ADCC_CH_SEL_SHIFT)
#define REG_CODEC_ADCC_CH_SEL(n)            BITFIELD_VAL(REG_CODEC_ADCC_CH_SEL, n)
#define ADCC_PRE_CHARGE_TIMER_DLY_SHIFT     4
#define ADCC_PRE_CHARGE_TIMER_DLY_MASK      (0xFF << ADCC_PRE_CHARGE_TIMER_DLY_SHIFT)
#define ADCC_PRE_CHARGE_TIMER_DLY(n)        BITFIELD_VAL(ADCC_PRE_CHARGE_TIMER_DLY, n)
#define REG_CODEC_ADCC_HP_MODE              (1 << 12)
#define REG_CODEC_ADCC_OP1_HP_MODE          (1 << 15)

// REG_12A
#define REG_CODEC_ADCC_CAL_MODE             (1 << 0)
#define REG_CODEC_ADCC_EN_DITHER_DAC1       (1 << 1)
#define REG_CODEC_ADCC_EN_DITHER_DAC2       (1 << 2)
#define REG_CODEC_ADCC_EN_N_DET             (1 << 3)
#define REG_CODEC_ADCC_EN_P_DET             (1 << 4)
#define REG_CODEC_ADCC_RDAC_DITHER_CAL_SHIFT 5
#define REG_CODEC_ADCC_RDAC_DITHER_CAL_MASK (0xFF << REG_CODEC_ADCC_RDAC_DITHER_CAL_SHIFT)
#define REG_CODEC_ADCC_RDAC_DITHER_CAL(n)   BITFIELD_VAL(REG_CODEC_ADCC_RDAC_DITHER_CAL, n)

// REG_12B
#define REG_CODEC_ADCC_NRES_VREF_SEL1_SHIFT 0
#define REG_CODEC_ADCC_NRES_VREF_SEL1_MASK  (0xF << REG_CODEC_ADCC_NRES_VREF_SEL1_SHIFT)
#define REG_CODEC_ADCC_NRES_VREF_SEL1(n)    BITFIELD_VAL(REG_CODEC_ADCC_NRES_VREF_SEL1, n)
#define REG_CODEC_ADCC_NRES_VREF_SEL2_SHIFT 4
#define REG_CODEC_ADCC_NRES_VREF_SEL2_MASK  (0xF << REG_CODEC_ADCC_NRES_VREF_SEL2_SHIFT)
#define REG_CODEC_ADCC_NRES_VREF_SEL2(n)    BITFIELD_VAL(REG_CODEC_ADCC_NRES_VREF_SEL2, n)
#define REG_CODEC_ADCC_PRES_VREF_SEL1_SHIFT 8
#define REG_CODEC_ADCC_PRES_VREF_SEL1_MASK  (0xF << REG_CODEC_ADCC_PRES_VREF_SEL1_SHIFT)
#define REG_CODEC_ADCC_PRES_VREF_SEL1(n)    BITFIELD_VAL(REG_CODEC_ADCC_PRES_VREF_SEL1, n)
#define REG_CODEC_ADCC_PRES_VREF_SEL2_SHIFT 12
#define REG_CODEC_ADCC_PRES_VREF_SEL2_MASK  (0xF << REG_CODEC_ADCC_PRES_VREF_SEL2_SHIFT)
#define REG_CODEC_ADCC_PRES_VREF_SEL2(n)    BITFIELD_VAL(REG_CODEC_ADCC_PRES_VREF_SEL2, n)

// REG_12C
#define REG_CODEC_ADCC_DITHER_DR            (1 << 0)
#define REG_CODEC_ADCC_DITHER_MGEN_SHIFT    1
#define REG_CODEC_ADCC_DITHER_MGEN_MASK     (0x7 << REG_CODEC_ADCC_DITHER_MGEN_SHIFT)
#define REG_CODEC_ADCC_DITHER_MGEN(n)       BITFIELD_VAL(REG_CODEC_ADCC_DITHER_MGEN, n)
#define REG_CODEC_EN_ADCC                   (1 << 4)
#define REG_CODEC_ADCC_CHANNEL_SEL_SHIFT    5
#define REG_CODEC_ADCC_CHANNEL_SEL_MASK     (0xF << REG_CODEC_ADCC_CHANNEL_SEL_SHIFT)
#define REG_CODEC_ADCC_CHANNEL_SEL(n)       BITFIELD_VAL(REG_CODEC_ADCC_CHANNEL_SEL, n)

// REG_130
#define REG_CODEC_TX_PEAK_NL_EN             (1 << 0)
#define REG_CODEC_TX_PEAK_PL_EN             (1 << 1)
#define REG_CODEC_TX_PEAK_DET_BIT           (1 << 2)
#define CFG_TX_PEAK_OFF_ADC_EN              (1 << 4)
#define CFG_TX_PEAK_OFF_DAC_EN              (1 << 5)
#define CFG_PEAK_DET_DR                     (1 << 6)
#define CFG_TX_PEAK_OFF_ADC                 (1 << 7)
#define CFG_TX_PEAK_OFF_DAC                 (1 << 8)
#define CFG_PEAK_DET_DB_DELAY_SHIFT         9
#define CFG_PEAK_DET_DB_DELAY_MASK          (0x7 << CFG_PEAK_DET_DB_DELAY_SHIFT)
#define CFG_PEAK_DET_DB_DELAY(n)            BITFIELD_VAL(CFG_PEAK_DET_DB_DELAY, n)
#define TX_PEAK_DET_STATUS                  (1 << 12)
#define TX_PEAK_DET_NL_STATUS               (1 << 13)
#define TX_PEAK_DET_PL_STATUS               (1 << 14)
#define REG_TX_PEAK_INTR_MASK               (1 << 15)

// REG_131
#define REG_CODEC_TX_EN_LDAC                (1 << 0)
#define REG_EN_CLKADC                       (1 << 2)
#define REG_EN_CLKCLASSD                    (1 << 3)
#define REG_EN_CLKDAC                       (1 << 4)
#define REG_PU_LPO48M                       (1 << 5)
#define REG_PU_PLL24M                       (1 << 6)

// REG_13E
#define RESERVED_DIG_15_0_SHIFT             0
#define RESERVED_DIG_15_0_MASK              (0xFFFF << RESERVED_DIG_15_0_SHIFT)
#define RESERVED_DIG_15_0(n)                BITFIELD_VAL(RESERVED_DIG_15_0, n)

// REG_13F
#define RESERVED_DIG_31_16_SHIFT            0
#define RESERVED_DIG_31_16_MASK             (0xFFFF << RESERVED_DIG_31_16_SHIFT)
#define RESERVED_DIG_31_16(n)               BITFIELD_VAL(RESERVED_DIG_31_16, n)

// REG_140
#define REG_CODEC_ADC_CAP_BIT               (1 << 0)
#define REG_CODEC_ADC_DITHER0P5_EN          (1 << 1)
#define REG_CODEC_ADC_DITHER0P25_EN         (1 << 2)
#define REG_CODEC_ADC_DITHER1_EN            (1 << 3)
#define REG_CODEC_ADC_DITHER2_EN            (1 << 4)
#define REG_CODEC_ADC_DITHER_PHASE_SEL      (1 << 5)
#define REG_CODEC_ADC_OP1_R_SEL             (1 << 6)
#define REG_CODEC_ADC_OP2_R_SEL             (1 << 7)
#define REG_CODEC_ADC_OP3_R_SEL             (1 << 8)
#define REG_CODEC_ADC_OP4_R_SEL             (1 << 9)
#define REG_CODEC_ADC_RES_SEL_SHIFT         10
#define REG_CODEC_ADC_RES_SEL_MASK          (0x7 << REG_CODEC_ADC_RES_SEL_SHIFT)
#define REG_CODEC_ADC_RES_SEL(n)            BITFIELD_VAL(REG_CODEC_ADC_RES_SEL, n)

// REG_148
#define REG_CODEC_TXC_CASP_12STAGE_L_SHIFT  0
#define REG_CODEC_TXC_CASP_12STAGE_L_MASK   (0x7 << REG_CODEC_TXC_CASP_12STAGE_L_SHIFT)
#define REG_CODEC_TXC_CASP_12STAGE_L(n)     BITFIELD_VAL(REG_CODEC_TXC_CASP_12STAGE_L, n)

// REG_14A
#define REG_SEL_CAPSENSOR_OSC               (1 << 0)
#define REG_CFG_CAPSENSOR_DIV_SHIFT         1
#define REG_CFG_CAPSENSOR_DIV_MASK          (0x7FF << REG_CFG_CAPSENSOR_DIV_SHIFT)
#define REG_CFG_CAPSENSOR_DIV(n)            BITFIELD_VAL(REG_CFG_CAPSENSOR_DIV, n)
#define REG_CLK_CAPSENSOR_GATE_EN           (1 << 12)
#define REG_CLK_32K_CAPSENSOR_GATE_EN       (1 << 13)
#define SOFT_RESETN_CAPSENSOR               (1 << 14)
#define SOFT_RESETN_CAPSENSOR_REG           (1 << 15)

// REG_14B
#define POL_CLK_OSC_ALN                     (1 << 0)
#define REG_CLK_ADC_6M_CODEC_GATE           (1 << 1)
#define POL_CLK_DAC_OUT_ANA                 (1 << 2)
#define REG_CLK_DAC_OUT_DIG_CLASSD_EN       (1 << 3)
#define REG_CLK_CODEC_CLASSD_DAC_OUT_ANA_EN (1 << 4)
#define REG_CLK_DAC_OUT_ANA_CLASSD_EN       (1 << 5)
#define REG_CLK_DAC_OUT_ANA_SEL             (1 << 6)
#define REG_CLK_ADC_6M_CODEC_INV            (1 << 7)
#define REG_CLK_DAC_DIG_INV                 (1 << 8)
#define REG_CLK_DAC_ANA_INV                 (1 << 9)
#define REG_CLK_DAC_OUT_DIG_CLASSD_INV      (1 << 10)
#define REG_CLK_CAPSENSOR_INV               (1 << 11)

// REG_14C
#define REG_CODEC_DIV_EN                    (1 << 0)
#define REG_CODEC_DIV_MODE                  (1 << 1)

// REG_14D
#define REG_CODEC_TX_PWR_SW                 (1 << 0)
#define REG_IDETLEAR_EN                     (1 << 9)
#define REG_TX_REGULATOR_BIT_SHIFT          12
#define REG_TX_REGULATOR_BIT_MASK           (0xF << REG_TX_REGULATOR_BIT_SHIFT)
#define REG_TX_REGULATOR_BIT(n)             BITFIELD_VAL(REG_TX_REGULATOR_BIT, n)

// REG_14E
#define REG_CODEC_TX_EAR_DR_EN              (1 << 0)
#define REG_CODEC_TX_EAR_DRE_GAIN_L_UPDATE  (1 << 1)
#define REG_CODEC_TX_EAR_DRE_GAIN_L_SHIFT   3
#define REG_CODEC_TX_EAR_DRE_GAIN_L_MASK    (0x1F << REG_CODEC_TX_EAR_DRE_GAIN_L_SHIFT)
#define REG_CODEC_TX_EAR_DRE_GAIN_L(n)      BITFIELD_VAL(REG_CODEC_TX_EAR_DRE_GAIN_L, n)
#define REG_CODEC_TX_EAR_ENBIAS             (1 << 13)
#define REG_CODEC_TX_EAR_LPBIAS             (1 << 14)
#define REG_CODEC_TX_EAR_OCEN               (1 << 15)

// REG_14F
#define REG_CODEC_TEST_SEL_SHIFT            0
#define REG_CODEC_TEST_SEL_MASK             (0xF << REG_CODEC_TEST_SEL_SHIFT)
#define REG_CODEC_TEST_SEL(n)               BITFIELD_VAL(REG_CODEC_TEST_SEL, n)
#define REG_CODEC_TX_REGULATOR_BIT_L_SHIFT  4
#define REG_CODEC_TX_REGULATOR_BIT_L_MASK   (0xF << REG_CODEC_TX_REGULATOR_BIT_L_SHIFT)
#define REG_CODEC_TX_REGULATOR_BIT_L(n)     BITFIELD_VAL(REG_CODEC_TX_REGULATOR_BIT_L, n)
#define REG_CODEC_TX_DAC_SWR_SHIFT          8
#define REG_CODEC_TX_DAC_SWR_MASK           (0x3 << REG_CODEC_TX_DAC_SWR_SHIFT)
#define REG_CODEC_TX_DAC_SWR(n)             BITFIELD_VAL(REG_CODEC_TX_DAC_SWR, n)
#define REG_CODEC_TX_DAC_VREF_L_SHIFT       10
#define REG_CODEC_TX_DAC_VREF_L_MASK        (0xF << REG_CODEC_TX_DAC_VREF_L_SHIFT)
#define REG_CODEC_TX_DAC_VREF_L(n)          BITFIELD_VAL(REG_CODEC_TX_DAC_VREF_L, n)
#define REG_CODEC_TX_DRV_05_L               (1 << 14)

// REG_150
#define REG_CODEC_TX_EAR_COMP1_L_SHIFT      0
#define REG_CODEC_TX_EAR_COMP1_L_MASK       (0xFF << REG_CODEC_TX_EAR_COMP1_L_SHIFT)
#define REG_CODEC_TX_EAR_COMP1_L(n)         BITFIELD_VAL(REG_CODEC_TX_EAR_COMP1_L, n)

// REG_151
#define REG_CODEC_TX_EAR_LOWGAINL_SHIFT     0
#define REG_CODEC_TX_EAR_LOWGAINL_MASK      (0x3F << REG_CODEC_TX_EAR_LOWGAINL_SHIFT)
#define REG_CODEC_TX_EAR_LOWGAINL(n)        BITFIELD_VAL(REG_CODEC_TX_EAR_LOWGAINL, n)
#define REG_CODEC_TX_EAR_IBSEL_L_SHIFT      12
#define REG_CODEC_TX_EAR_IBSEL_L_MASK       (0x3 << REG_CODEC_TX_EAR_IBSEL_L_SHIFT)
#define REG_CODEC_TX_EAR_IBSEL_L(n)         BITFIELD_VAL(REG_CODEC_TX_EAR_IBSEL_L, n)

// REG_152
#define REG_CODEC_TX_EAR_OFF_BITL_SHIFT     0
#define REG_CODEC_TX_EAR_OFF_BITL_MASK      (0x3FFF << REG_CODEC_TX_EAR_OFF_BITL_SHIFT)
#define REG_CODEC_TX_EAR_OFF_BITL(n)        BITFIELD_VAL(REG_CODEC_TX_EAR_OFF_BITL, n)
#define REG_CODEC_TX_EAR_OFFEN_L            (1 << 14)
#define REG_CODEC_TX_EN_DACLDO              (1 << 15)

// REG_153
#define REG_CODEC_TX_EAR_DIS_SHIFT          14
#define REG_CODEC_TX_EAR_DIS_MASK           (0x3 << REG_CODEC_TX_EAR_DIS_SHIFT)
#define REG_CODEC_TX_EAR_DIS(n)             BITFIELD_VAL(REG_CODEC_TX_EAR_DIS, n)

// REG_154
#define REG_CODEC_TX_EAR_SOFTSTART_SHIFT    0
#define REG_CODEC_TX_EAR_SOFTSTART_MASK     (0x3F << REG_CODEC_TX_EAR_SOFTSTART_SHIFT)
#define REG_CODEC_TX_EAR_SOFTSTART(n)       BITFIELD_VAL(REG_CODEC_TX_EAR_SOFTSTART, n)
#define REG_CODEC_TX_EAR_OUTPUTSEL_L_SHIFT  6
#define REG_CODEC_TX_EAR_OUTPUTSEL_L_MASK   (0x1F << REG_CODEC_TX_EAR_OUTPUTSEL_L_SHIFT)
#define REG_CODEC_TX_EAR_OUTPUTSEL_L(n)     BITFIELD_VAL(REG_CODEC_TX_EAR_OUTPUTSEL_L, n)
#define REG_CODEC_TX_EAR_DR_ST_SHIFT        11
#define REG_CODEC_TX_EAR_DR_ST_MASK         (0x3 << REG_CODEC_TX_EAR_DR_ST_SHIFT)
#define REG_CODEC_TX_EAR_DR_ST(n)           BITFIELD_VAL(REG_CODEC_TX_EAR_DR_ST, n)
#define REG_OCP_SEL_L_SHIFT                 13
#define REG_OCP_SEL_L_MASK                  (0x3 << REG_OCP_SEL_L_SHIFT)
#define REG_OCP_SEL_L(n)                    BITFIELD_VAL(REG_OCP_SEL_L, n)

// REG_155
#define REG_CODEC_TX_EN_EARPA_L             (1 << 0)
#define REG_CODEC_TX_EN_LCLK                (1 << 2)
#define REG_CODEC_TX_ENB_RC_L_SHIFT         3
#define REG_CODEC_TX_ENB_RC_L_MASK          (0x3 << REG_CODEC_TX_ENB_RC_L_SHIFT)
#define REG_CODEC_TX_ENB_RC_L(n)            BITFIELD_VAL(REG_CODEC_TX_ENB_RC_L, n)
#define REG_OCP_DET_EN_L                    (1 << 5)
#define REG_CODEC_TX_EN_S1PA                (1 << 6)
#define REG_CODEC_TX_EN_S2PA                (1 << 7)
#define REG_CODEC_TX_EN_S3PA                (1 << 8)
#define REG_CODEC_TX_EN_S4PA                (1 << 9)
#define REG_CODEC_TX_EN_S5PA                (1 << 10)
#define REG_PU_TX_REGULATOR                 (1 << 11)
#define REG_BYPASS_TX_REGULATOR             (1 << 12)
#define REG_CODEC_DAC_CLK_EDGE_SEL_L        (1 << 13)
#define REG_CODEC_TX_EAR_GAIN               (1 << 14)

// REG_156
#define REG_CODEC_TX_EN_LPPA                (1 << 0)
#define REG_CODEC_TX_SW_MODE_L              (1 << 1)
#define DRE_GAIN_SEL_L                      (1 << 2)
#define REG_CODEC_TX_REG_LOWGAIN            (1 << 3)
#define CFG_TX_TREE_EN                      (1 << 4)
#define CFG_TX_CH0_MUTE                     (1 << 5)
#define DIG_PU_CODEC_TX_REGULATOR_L         (1 << 6)
#define CFG_CODEC_DIN_L_RST                 (1 << 8)
#define REG_CODEC_TX_EAR_VCM_L_SHIFT        9
#define REG_CODEC_TX_EAR_VCM_L_MASK         (0x7 << REG_CODEC_TX_EAR_VCM_L_SHIFT)
#define REG_CODEC_TX_EAR_VCM_L(n)           BITFIELD_VAL(REG_CODEC_TX_EAR_VCM_L, n)
#define CFG_ADC_CODEC_CLK_INV               (1 << 12)

// REG_157
#define CODEC_DIN_L_HI_RSTVAL_SHIFT         0
#define CODEC_DIN_L_HI_RSTVAL_MASK          (0xFFFF << CODEC_DIN_L_HI_RSTVAL_SHIFT)
#define CODEC_DIN_L_HI_RSTVAL(n)            BITFIELD_VAL(CODEC_DIN_L_HI_RSTVAL, n)

// REG_158
#define CODEC_DIN_L_LO_RSTVAL_SHIFT         0
#define CODEC_DIN_L_LO_RSTVAL_MASK          (0xFFFF << CODEC_DIN_L_LO_RSTVAL_SHIFT)
#define CODEC_DIN_L_LO_RSTVAL(n)            BITFIELD_VAL(CODEC_DIN_L_LO_RSTVAL, n)

// REG_15B
#define REG_CODEC_TX_CASN_L_SHIFT           0
#define REG_CODEC_TX_CASN_L_MASK            (0x3 << REG_CODEC_TX_CASN_L_SHIFT)
#define REG_CODEC_TX_CASN_L(n)              BITFIELD_VAL(REG_CODEC_TX_CASN_L, n)
#define REG_CODEC_TX_CASP_L_SHIFT           4
#define REG_CODEC_TX_CASP_L_MASK            (0x3 << REG_CODEC_TX_CASP_L_SHIFT)
#define REG_CODEC_TX_CASP_L(n)              BITFIELD_VAL(REG_CODEC_TX_CASP_L, n)
#define REG_CODEC_TX_IB_SEL_ST2_L_SHIFT     8
#define REG_CODEC_TX_IB_SEL_ST2_L_MASK      (0x7 << REG_CODEC_TX_IB_SEL_ST2_L_SHIFT)
#define REG_CODEC_TX_IB_SEL_ST2_L(n)        BITFIELD_VAL(REG_CODEC_TX_IB_SEL_ST2_L, n)
#define REG_CODEC_TX_OC_PATH_L              (1 << 11)
#define REG_CODEC_TX_OFF_RANGE_X2_L         (1 << 12)
#define REG_PU_LPO48MADC_INT                (1 << 15)

// REG_15C
#define REG_CODEC_TX_VREFBUF_CAS_L_SHIFT    0
#define REG_CODEC_TX_VREFBUF_CAS_L_MASK     (0x3 << REG_CODEC_TX_VREFBUF_CAS_L_SHIFT)
#define REG_CODEC_TX_VREFBUF_CAS_L(n)       BITFIELD_VAL(REG_CODEC_TX_VREFBUF_CAS_L, n)
#define REG_CODEC_TX_VREFBUF_LOWGAIN_L_SHIFT 2
#define REG_CODEC_TX_VREFBUF_LOWGAIN_L_MASK (0x3 << REG_CODEC_TX_VREFBUF_LOWGAIN_L_SHIFT)
#define REG_CODEC_TX_VREFBUF_LOWGAIN_L(n)   BITFIELD_VAL(REG_CODEC_TX_VREFBUF_LOWGAIN_L, n)
#define REG_CODEC_TX_VREFBUF_ISEL_SHIFT     4
#define REG_CODEC_TX_VREFBUF_ISEL_MASK      (0x7 << REG_CODEC_TX_VREFBUF_ISEL_SHIFT)
#define REG_CODEC_TX_VREFBUF_ISEL(n)        BITFIELD_VAL(REG_CODEC_TX_VREFBUF_ISEL, n)
#define REG_CODEC_TX_VREFBUF_ISEL_L_SHIFT   8
#define REG_CODEC_TX_VREFBUF_ISEL_L_MASK    (0x7 << REG_CODEC_TX_VREFBUF_ISEL_L_SHIFT)
#define REG_CODEC_TX_VREFBUF_ISEL_L(n)      BITFIELD_VAL(REG_CODEC_TX_VREFBUF_ISEL_L, n)
#define REG_CODEC_TX_VREF_RZ_L              (1 << 11)
#define REG_CODEC_TX_VREFBUF_CSEL_L_SHIFT   12
#define REG_CODEC_TX_VREFBUF_CSEL_L_MASK    (0xF << REG_CODEC_TX_VREFBUF_CSEL_L_SHIFT)
#define REG_CODEC_TX_VREFBUF_CSEL_L(n)      BITFIELD_VAL(REG_CODEC_TX_VREFBUF_CSEL_L, n)

// REG_15D
#define REG_CODEC_TX_OUTPUT_L_LP_DR         (1 << 0)
#define REG_CODEC_TX_OUTPUT_L_LP            (1 << 1)
#define REG_ZERO_DETECT_CHANGE              (1 << 2)
#define REG_CODEC_TX_HP_DAC_LDO             (1 << 3)
#define REG_CODEC_TX_VCMO_SEL_L             (1 << 4)
#define REG_PU_ZERO_DET_L                   (1 << 5)
#define REG_ZERO_DETECT_POWER_DOWN_DIRECT   (1 << 7)
#define REG_ZERO_DETECT_POWER_DOWN          (1 << 8)
#define REG_CODEC_TX_OUTPUT_LP_POL          (1 << 12)
#define REG_CODEC_TX_EAR_COMP_CM1           (1 << 13)

// REG_15E
#define REG_CODEC_TX_EAR_GAIN_CHANGE        (1 << 0)
#define REG_CODEC_TX_EAR_DRE_GAIN_L_1_SHIFT 1
#define REG_CODEC_TX_EAR_DRE_GAIN_L_1_MASK  (0x1F << REG_CODEC_TX_EAR_DRE_GAIN_L_1_SHIFT)
#define REG_CODEC_TX_EAR_DRE_GAIN_L_1(n)    BITFIELD_VAL(REG_CODEC_TX_EAR_DRE_GAIN_L_1, n)
#define REG_CODEC_TX_EAR_DRE_GAIN_L_2_SHIFT 6
#define REG_CODEC_TX_EAR_DRE_GAIN_L_2_MASK  (0x1F << REG_CODEC_TX_EAR_DRE_GAIN_L_2_SHIFT)
#define REG_CODEC_TX_EAR_DRE_GAIN_L_2(n)    BITFIELD_VAL(REG_CODEC_TX_EAR_DRE_GAIN_L_2, n)
#define ANADRE_GAIN_SEL                     (1 << 11)
#define ANADRE_DC_SEL                       (1 << 12)
#define DIG_CODEC_TX_EN_LDAC_ANA            (1 << 13)
#define OCP_DET_L_STATUS                    (1 << 14)
#define REG_OCP_DET_L_CLR                   (1 << 15)

// REG_15F
#define REG_CODEC_TX_EAR_OFF_BITL_1_SHIFT   0
#define REG_CODEC_TX_EAR_OFF_BITL_1_MASK    (0x3FFF << REG_CODEC_TX_EAR_OFF_BITL_1_SHIFT)
#define REG_CODEC_TX_EAR_OFF_BITL_1(n)      BITFIELD_VAL(REG_CODEC_TX_EAR_OFF_BITL_1, n)

// REG_160
#define REG_CODEC_TX_EAR_OFF_BITL_2_SHIFT   0
#define REG_CODEC_TX_EAR_OFF_BITL_2_MASK    (0x3FFF << REG_CODEC_TX_EAR_OFF_BITL_2_SHIFT)
#define REG_CODEC_TX_EAR_OFF_BITL_2(n)      BITFIELD_VAL(REG_CODEC_TX_EAR_OFF_BITL_2, n)

// REG_16C
#define RESERVED_DIG_15_0_SHIFT             0
#define RESERVED_DIG_15_0_MASK              (0xFFFF << RESERVED_DIG_15_0_SHIFT)
#define RESERVED_DIG_15_0(n)                BITFIELD_VAL(RESERVED_DIG_15_0, n)

// REG_16D
#define RESERVED_DIG_31_16_SHIFT            0
#define RESERVED_DIG_31_16_MASK             (0xFFFF << RESERVED_DIG_31_16_SHIFT)
#define RESERVED_DIG_31_16(n)               BITFIELD_VAL(RESERVED_DIG_31_16, n)

// REG_1A0
#define LDO0P3_VCLASSD_OC_STAT              (1 << 0)
#define LDO0P6_VCLASSD_OC_STAT              (1 << 1)
#define LDO1P2_VCLASSD_OC_STAT              (1 << 2)

// REG_1A1
#define REG_LDO0P3_VCLASSD_BYPASS           (1 << 0)
#define REG_LDO0P3_VCLASSD_CAP_BIT_SHIFT    1
#define REG_LDO0P3_VCLASSD_CAP_BIT_MASK     (0x7 << REG_LDO0P3_VCLASSD_CAP_BIT_SHIFT)
#define REG_LDO0P3_VCLASSD_CAP_BIT(n)       BITFIELD_VAL(REG_LDO0P3_VCLASSD_CAP_BIT, n)
#define REG_LDO0P3_VCLASSD_EN_ILOAD         (1 << 4)
#define REG_LDO0P3_VCLASSD_EN_LEAKAGE_TRACK (1 << 5)
#define REG_LDO0P3_VCLASSD_ENABLE           (1 << 6)
#define REG_LDO0P3_VCLASSD_ILIMIT_SEL       (1 << 7)
#define REG_LDO0P3_VCLASSD_ILOAD_BIT_SHIFT  8
#define REG_LDO0P3_VCLASSD_ILOAD_BIT_MASK   (0x7 << REG_LDO0P3_VCLASSD_ILOAD_BIT_SHIFT)
#define REG_LDO0P3_VCLASSD_ILOAD_BIT(n)     BITFIELD_VAL(REG_LDO0P3_VCLASSD_ILOAD_BIT, n)
#define REG_LDO0P3_VCLASSD_LP_ENABLE        (1 << 11)
#define REG_LDO0P3_VCLASSD_OCP_EN           (1 << 12)
#define REG_LDO0P3_VCLASSD_PULL_DOWN        (1 << 13)
#define REG_LDO0P3_VCLASSD_RES_PULL_DOWN    (1 << 14)

// REG_1A2
#define REG_LDO0P3_VCLASSD_RES_SEL_SHIFT    0
#define REG_LDO0P3_VCLASSD_RES_SEL_MASK     (0xFF << REG_LDO0P3_VCLASSD_RES_SEL_SHIFT)
#define REG_LDO0P3_VCLASSD_RES_SEL(n)       BITFIELD_VAL(REG_LDO0P3_VCLASSD_RES_SEL, n)
#define REG_LDO0P3_VCLASSD_SOFT_START       (1 << 8)

// REG_1A3
#define REG_LDO0P6_VCLASSD_BYPASS           (1 << 0)
#define REG_LDO0P6_VCLASSD_CAP_BIT_SHIFT    1
#define REG_LDO0P6_VCLASSD_CAP_BIT_MASK     (0x7 << REG_LDO0P6_VCLASSD_CAP_BIT_SHIFT)
#define REG_LDO0P6_VCLASSD_CAP_BIT(n)       BITFIELD_VAL(REG_LDO0P6_VCLASSD_CAP_BIT, n)
#define REG_LDO0P6_VCLASSD_EN_ILOAD         (1 << 4)
#define REG_LDO0P6_VCLASSD_EN_LEAKAGE_TRACK (1 << 5)
#define REG_LDO0P6_VCLASSD_ENABLE           (1 << 6)
#define REG_LDO0P6_VCLASSD_ILIMIT_SEL       (1 << 7)
#define REG_LDO0P6_VCLASSD_ILOAD_BIT_SHIFT  8
#define REG_LDO0P6_VCLASSD_ILOAD_BIT_MASK   (0x7 << REG_LDO0P6_VCLASSD_ILOAD_BIT_SHIFT)
#define REG_LDO0P6_VCLASSD_ILOAD_BIT(n)     BITFIELD_VAL(REG_LDO0P6_VCLASSD_ILOAD_BIT, n)
#define REG_LDO0P6_VCLASSD_LP_ENABLE        (1 << 11)
#define REG_LDO0P6_VCLASSD_OCP_EN           (1 << 12)
#define REG_LDO0P6_VCLASSD_PULL_DOWN        (1 << 13)
#define REG_LDO0P6_VCLASSD_RES_PULL_DOWN    (1 << 14)

// REG_1A4
#define REG_LDO0P6_VCLASSD_RES_SEL_SHIFT    0
#define REG_LDO0P6_VCLASSD_RES_SEL_MASK     (0xFF << REG_LDO0P6_VCLASSD_RES_SEL_SHIFT)
#define REG_LDO0P6_VCLASSD_RES_SEL(n)       BITFIELD_VAL(REG_LDO0P6_VCLASSD_RES_SEL, n)
#define REG_LDO0P6_VCLASSD_SOFT_START       (1 << 8)

// REG_1A5
#define REG_LDO1P2_VCLASSD_BYPASS           (1 << 0)
#define REG_LDO1P2_VCLASSD_CAP_BIT_SHIFT    1
#define REG_LDO1P2_VCLASSD_CAP_BIT_MASK     (0x7 << REG_LDO1P2_VCLASSD_CAP_BIT_SHIFT)
#define REG_LDO1P2_VCLASSD_CAP_BIT(n)       BITFIELD_VAL(REG_LDO1P2_VCLASSD_CAP_BIT, n)
#define REG_LDO1P2_VCLASSD_EN_ILOAD         (1 << 4)
#define REG_LDO1P2_VCLASSD_EN_LEAKAGE_TRACK (1 << 5)
#define REG_LDO1P2_VCLASSD_ENABLE           (1 << 6)
#define REG_LDO1P2_VCLASSD_ILIMIT_SEL       (1 << 7)
#define REG_LDO1P2_VCLASSD_ILOAD_BIT_SHIFT  8
#define REG_LDO1P2_VCLASSD_ILOAD_BIT_MASK   (0x7 << REG_LDO1P2_VCLASSD_ILOAD_BIT_SHIFT)
#define REG_LDO1P2_VCLASSD_ILOAD_BIT(n)     BITFIELD_VAL(REG_LDO1P2_VCLASSD_ILOAD_BIT, n)
#define REG_LDO1P2_VCLASSD_LP_ENABLE        (1 << 11)
#define REG_LDO1P2_VCLASSD_OCP_EN           (1 << 12)
#define REG_LDO1P2_VCLASSD_PULL_DOWN        (1 << 13)
#define REG_LDO1P2_VCLASSD_RES_PULL_DOWN    (1 << 14)

// REG_1A6
#define REG_LDO1P2_VCLASSD_RES_SEL_SHIFT    0
#define REG_LDO1P2_VCLASSD_RES_SEL_MASK     (0xFF << REG_LDO1P2_VCLASSD_RES_SEL_SHIFT)
#define REG_LDO1P2_VCLASSD_RES_SEL(n)       BITFIELD_VAL(REG_LDO1P2_VCLASSD_RES_SEL, n)
#define REG_LDO1P2_VCLASSD_SOFT_START       (1 << 8)

// REG_1B0
#define REG_CLASSD_C_SEL_BUFFERN_L          (1 << 0)
#define REG_CLASSD_C_SEL_BUFFERP_L          (1 << 1)
#define REG_CLASSD_CLK_3M_POLARITY_SEL_L    (1 << 2)
#define REG_CLASSD_CLK_24M_POLARITY_SEL_L   (1 << 3)
#define REG_CLASSD_DAC_SHORT_RES_L          (1 << 4)
#define REG_CLASSD_DRIVER_PWR_SEL_L         (1 << 5)
#define REG_CLASSD_EN_OCP_L                 (1 << 6)
#define REG_CLASSD_FB_RES_CAP_L_SHIFT       7
#define REG_CLASSD_FB_RES_CAP_L_MASK        (0x7 << REG_CLASSD_FB_RES_CAP_L_SHIFT)
#define REG_CLASSD_FB_RES_CAP_L(n)          BITFIELD_VAL(REG_CLASSD_FB_RES_CAP_L, n)
#define REG_CLASSD_FB_RES_CAP_X2_L          (1 << 10)
#define REG_CLASSD_FSP_MODE_L               (1 << 11)
#define REG_CLASSD_GAIN_MODE_L              (1 << 12)
#define REG_CLASSD_IBIT_BUFFER_N_L_SHIFT    13
#define REG_CLASSD_IBIT_BUFFER_N_L_MASK     (0x3 << REG_CLASSD_IBIT_BUFFER_N_L_SHIFT)
#define REG_CLASSD_IBIT_BUFFER_N_L(n)       BITFIELD_VAL(REG_CLASSD_IBIT_BUFFER_N_L, n)

// REG_1B1
#define REG_CLASSD_CAP1_IN_L_SHIFT          0
#define REG_CLASSD_CAP1_IN_L_MASK           (0xF << REG_CLASSD_CAP1_IN_L_SHIFT)
#define REG_CLASSD_CAP1_IN_L(n)             BITFIELD_VAL(REG_CLASSD_CAP1_IN_L, n)
#define REG_CLASSD_CAP2_IN_L_SHIFT          4
#define REG_CLASSD_CAP2_IN_L_MASK           (0xF << REG_CLASSD_CAP2_IN_L_SHIFT)
#define REG_CLASSD_CAP2_IN_L(n)             BITFIELD_VAL(REG_CLASSD_CAP2_IN_L, n)
#define REG_CLASSD_CAP3_IN_L_SHIFT          8
#define REG_CLASSD_CAP3_IN_L_MASK           (0xF << REG_CLASSD_CAP3_IN_L_SHIFT)
#define REG_CLASSD_CAP3_IN_L(n)             BITFIELD_VAL(REG_CLASSD_CAP3_IN_L, n)
#define REG_CLASSD_CAP4_IN_L_SHIFT          12
#define REG_CLASSD_CAP4_IN_L_MASK           (0xF << REG_CLASSD_CAP4_IN_L_SHIFT)
#define REG_CLASSD_CAP4_IN_L(n)             BITFIELD_VAL(REG_CLASSD_CAP4_IN_L, n)

// REG_1B2
#define REG_CLASSD_IBIT_BUFFER_P_L_SHIFT    0
#define REG_CLASSD_IBIT_BUFFER_P_L_MASK     (0x3 << REG_CLASSD_IBIT_BUFFER_P_L_SHIFT)
#define REG_CLASSD_IBIT_BUFFER_P_L(n)       BITFIELD_VAL(REG_CLASSD_IBIT_BUFFER_P_L, n)
#define REG_CLASSD_GAIN_N2P5DB_L            (1 << 2)
#define REG_CLASSD_IBSEL_BIAS_BUFFER_L_SHIFT 4
#define REG_CLASSD_IBSEL_BIAS_BUFFER_L_MASK (0xF << REG_CLASSD_IBSEL_BIAS_BUFFER_L_SHIFT)
#define REG_CLASSD_IBSEL_BIAS_BUFFER_L(n)   BITFIELD_VAL(REG_CLASSD_IBSEL_BIAS_BUFFER_L, n)
#define REG_CLASSD_IBSEL_OCP_L_SHIFT        8
#define REG_CLASSD_IBSEL_OCP_L_MASK         (0xF << REG_CLASSD_IBSEL_OCP_L_SHIFT)
#define REG_CLASSD_IBSEL_OCP_L(n)           BITFIELD_VAL(REG_CLASSD_IBSEL_OCP_L, n)
#define REG_CLASSD_IBSEL_OFF_L_SHIFT        12
#define REG_CLASSD_IBSEL_OFF_L_MASK         (0xF << REG_CLASSD_IBSEL_OFF_L_SHIFT)
#define REG_CLASSD_IBSEL_OFF_L(n)           BITFIELD_VAL(REG_CLASSD_IBSEL_OFF_L, n)

// REG_1B3
#define REG_CLASSD_IBSEL_OP1_L_SHIFT        0
#define REG_CLASSD_IBSEL_OP1_L_MASK         (0xF << REG_CLASSD_IBSEL_OP1_L_SHIFT)
#define REG_CLASSD_IBSEL_OP1_L(n)           BITFIELD_VAL(REG_CLASSD_IBSEL_OP1_L, n)
#define REG_CLASSD_IBSEL_OP2_L_SHIFT        4
#define REG_CLASSD_IBSEL_OP2_L_MASK         (0xF << REG_CLASSD_IBSEL_OP2_L_SHIFT)
#define REG_CLASSD_IBSEL_OP2_L(n)           BITFIELD_VAL(REG_CLASSD_IBSEL_OP2_L, n)
#define REG_CLASSD_IBSEL_OP3_L_SHIFT        8
#define REG_CLASSD_IBSEL_OP3_L_MASK         (0xF << REG_CLASSD_IBSEL_OP3_L_SHIFT)
#define REG_CLASSD_IBSEL_OP3_L(n)           BITFIELD_VAL(REG_CLASSD_IBSEL_OP3_L, n)
#define REG_CLASSD_IBSEL_OP4_L_SHIFT        12
#define REG_CLASSD_IBSEL_OP4_L_MASK         (0xF << REG_CLASSD_IBSEL_OP4_L_SHIFT)
#define REG_CLASSD_IBSEL_OP4_L(n)           BITFIELD_VAL(REG_CLASSD_IBSEL_OP4_L, n)

// REG_1B4
#define REG_CLASSD_IBSEL_OP5_L_SHIFT        0
#define REG_CLASSD_IBSEL_OP5_L_MASK         (0xF << REG_CLASSD_IBSEL_OP5_L_SHIFT)
#define REG_CLASSD_IBSEL_OP5_L(n)           BITFIELD_VAL(REG_CLASSD_IBSEL_OP5_L, n)
#define REG_CLASSD_IBSEL_OP6_L_SHIFT        4
#define REG_CLASSD_IBSEL_OP6_L_MASK         (0xF << REG_CLASSD_IBSEL_OP6_L_SHIFT)
#define REG_CLASSD_IBSEL_OP6_L(n)           BITFIELD_VAL(REG_CLASSD_IBSEL_OP6_L, n)
#define REG_CLASSD_IBSEL_REG_L_SHIFT        8
#define REG_CLASSD_IBSEL_REG_L_MASK         (0xF << REG_CLASSD_IBSEL_REG_L_SHIFT)
#define REG_CLASSD_IBSEL_REG_L(n)           BITFIELD_VAL(REG_CLASSD_IBSEL_REG_L, n)
#define REG_CLASSD_IBSEL_VCM_DAC_L_SHIFT    12
#define REG_CLASSD_IBSEL_VCM_DAC_L_MASK     (0xF << REG_CLASSD_IBSEL_VCM_DAC_L_SHIFT)
#define REG_CLASSD_IBSEL_VCM_DAC_L(n)       BITFIELD_VAL(REG_CLASSD_IBSEL_VCM_DAC_L, n)

// REG_1B5
#define REG_CLASSD_IBSEL_VCM_OUT_L_SHIFT    0
#define REG_CLASSD_IBSEL_VCM_OUT_L_MASK     (0xF << REG_CLASSD_IBSEL_VCM_OUT_L_SHIFT)
#define REG_CLASSD_IBSEL_VCM_OUT_L(n)       BITFIELD_VAL(REG_CLASSD_IBSEL_VCM_OUT_L, n)
#define REG_CLASSD_IBSEL_VCOMP_L_SHIFT      4
#define REG_CLASSD_IBSEL_VCOMP_L_MASK       (0xF << REG_CLASSD_IBSEL_VCOMP_L_SHIFT)
#define REG_CLASSD_IBSEL_VCOMP_L(n)         BITFIELD_VAL(REG_CLASSD_IBSEL_VCOMP_L, n)
#define REG_CLASSD_IBSEL_VREF_L_SHIFT       8
#define REG_CLASSD_IBSEL_VREF_L_MASK        (0xF << REG_CLASSD_IBSEL_VREF_L_SHIFT)
#define REG_CLASSD_IBSEL_VREF_L(n)          BITFIELD_VAL(REG_CLASSD_IBSEL_VREF_L, n)
#define REG_CLASSD_IBSEL_VTOI_L_SHIFT       12
#define REG_CLASSD_IBSEL_VTOI_L_MASK        (0xF << REG_CLASSD_IBSEL_VTOI_L_SHIFT)
#define REG_CLASSD_IBSEL_VTOI_L(n)          BITFIELD_VAL(REG_CLASSD_IBSEL_VTOI_L, n)

// REG_1B6
#define REG_CLASSD_OFFSET_CAL_N2N_EN_L      (1 << 0)
#define REG_CLASSD_OFFSET_CAL_N2P_EN_L      (1 << 1)
#define REG_CLASSD_OFFSET_CAL_P2N_EN_L      (1 << 2)
#define REG_CLASSD_OFFSET_CAL_P2P_EN_L      (1 << 3)
#define REG_CLASSD_OC_TH_L_SHIFT            4
#define REG_CLASSD_OC_TH_L_MASK             (0x3 << REG_CLASSD_OC_TH_L_SHIFT)
#define REG_CLASSD_OC_TH_L(n)               BITFIELD_VAL(REG_CLASSD_OC_TH_L, n)
#define REG_CLASSD_OCP_MODE_IN_L_SHIFT      6
#define REG_CLASSD_OCP_MODE_IN_L_MASK       (0x3 << REG_CLASSD_OCP_MODE_IN_L_SHIFT)
#define REG_CLASSD_OCP_MODE_IN_L(n)         BITFIELD_VAL(REG_CLASSD_OCP_MODE_IN_L, n)

// REG_1B7
#define REG_CLASSD_OFFSET_L_SHIFT           0
#define REG_CLASSD_OFFSET_L_MASK            (0x3FFF << REG_CLASSD_OFFSET_L_SHIFT)
#define REG_CLASSD_OFFSET_L(n)              BITFIELD_VAL(REG_CLASSD_OFFSET_L, n)

// REG_1B8
#define REG_CLASSD_MODE2_L                  (1 << 0)
#define REG_CLASSD_MODE3_L                  (1 << 1)
#define REG_CLASSD_OP1_C_SEL_IN_L           (1 << 2)
#define REG_CLASSD_OP1_IBIT_IN_L_SHIFT      4
#define REG_CLASSD_OP1_IBIT_IN_L_MASK       (0x7 << REG_CLASSD_OP1_IBIT_IN_L_SHIFT)
#define REG_CLASSD_OP1_IBIT_IN_L(n)         BITFIELD_VAL(REG_CLASSD_OP1_IBIT_IN_L, n)
#define REG_CLASSD_OP1_VCM_SEL_L            (1 << 7)
#define REG_CLASSD_OP2_C_SEL_IN_L           (1 << 8)
#define REG_CLASSD_OP1_R_SEL_L              (1 << 9)
#define REG_CLASSD_OP2_IBIT_IN_L_SHIFT      10
#define REG_CLASSD_OP2_IBIT_IN_L_MASK       (0x3 << REG_CLASSD_OP2_IBIT_IN_L_SHIFT)
#define REG_CLASSD_OP2_IBIT_IN_L(n)         BITFIELD_VAL(REG_CLASSD_OP2_IBIT_IN_L, n)
#define REG_CLASSD_OP3_C_SEL_IN_L           (1 << 12)
#define REG_CLASSD_OP3_IBIT_IN_L_SHIFT      13
#define REG_CLASSD_OP3_IBIT_IN_L_MASK       (0x3 << REG_CLASSD_OP3_IBIT_IN_L_SHIFT)
#define REG_CLASSD_OP3_IBIT_IN_L(n)         BITFIELD_VAL(REG_CLASSD_OP3_IBIT_IN_L, n)
#define REG_CLASSD_OP4_C_SEL_IN_L           (1 << 15)

// REG_1B9
#define REG_CLASSD_OP4_IBIT_IN_L_SHIFT      0
#define REG_CLASSD_OP4_IBIT_IN_L_MASK       (0x3 << REG_CLASSD_OP4_IBIT_IN_L_SHIFT)
#define REG_CLASSD_OP4_IBIT_IN_L(n)         BITFIELD_VAL(REG_CLASSD_OP4_IBIT_IN_L, n)
#define REG_CLASSD_OP6_C_SEL_IN_L           (1 << 2)
#define REG_CLASSD_OP6_IBIT_IN_L_SHIFT      3
#define REG_CLASSD_OP6_IBIT_IN_L_MASK       (0x3 << REG_CLASSD_OP6_IBIT_IN_L_SHIFT)
#define REG_CLASSD_OP6_IBIT_IN_L(n)         BITFIELD_VAL(REG_CLASSD_OP6_IBIT_IN_L, n)
#define REG_CLASSD_REG_LOWGAIN              (1 << 5)
#define REG_CLASSD_OVERFLOW_PROTECTION_EN_L (1 << 6)
#define REG_CLASSD_OVERFLOW_MODE_L          (1 << 7)
#define REG_CLASSD_PART_RESET_L             (1 << 8)
#define REG_CLASSD_PATTERN_MODE_2_L         (1 << 9)
#define REG_CLASSD_PU_REGULATOR             (1 << 10)
#define REG_CLASSD_REGULATOR_BIT_SHIFT      11
#define REG_CLASSD_REGULATOR_BIT_MASK       (0xF << REG_CLASSD_REGULATOR_BIT_SHIFT)
#define REG_CLASSD_REGULATOR_BIT(n)         BITFIELD_VAL(REG_CLASSD_REGULATOR_BIT, n)
#define REG_CLASSD_RSTN_OCP_L               (1 << 15)

// REG_1BA
#define REG_CLASSD_S1_IN_L                  (1 << 0)
#define REG_CLASSD_S2_IN_L                  (1 << 1)
#define REG_CLASSD_S3_IN_L                  (1 << 2)
#define REG_CLASSD_S4_IN_L                  (1 << 3)
#define REG_CLASSD_VCM_DAC_IBIT_L_SHIFT     4
#define REG_CLASSD_VCM_DAC_IBIT_L_MASK      (0x3 << REG_CLASSD_VCM_DAC_IBIT_L_SHIFT)
#define REG_CLASSD_VCM_DAC_IBIT_L(n)        BITFIELD_VAL(REG_CLASSD_VCM_DAC_IBIT_L, n)
#define REG_CLASSD_VCM_DAC_R_SEL_L          (1 << 6)
#define REG_CLASSD_VCM_OUT_IBIT_L_SHIFT     7
#define REG_CLASSD_VCM_OUT_IBIT_L_MASK      (0x3 << REG_CLASSD_VCM_OUT_IBIT_L_SHIFT)
#define REG_CLASSD_VCM_OUT_IBIT_L(n)        BITFIELD_VAL(REG_CLASSD_VCM_OUT_IBIT_L, n)
#define REG_CLASSD_VCM_OUT_R_SEL_L          (1 << 9)
#define REG_CLASSD_VCM_OUT_RES_BIT_L        (1 << 10)
#define REG_CLASSD_THREE_QUARTER_MODE_L     (1 << 11)
#define REG_CLASSD_THREE_QUARTER_PATTERN_BYPASS_L (1 << 12)
#define REG_CLASSD_VTOI_I_DAC2_L_SHIFT      13
#define REG_CLASSD_VTOI_I_DAC2_L_MASK       (0x7 << REG_CLASSD_VTOI_I_DAC2_L_SHIFT)
#define REG_CLASSD_VTOI_I_DAC2_L(n)         BITFIELD_VAL(REG_CLASSD_VTOI_I_DAC2_L, n)

// REG_1BB
#define REG_CLASSD_VTOI_IDAC_SEL_L_SHIFT    0
#define REG_CLASSD_VTOI_IDAC_SEL_L_MASK     (0xF << REG_CLASSD_VTOI_IDAC_SEL_L_SHIFT)
#define REG_CLASSD_VTOI_IDAC_SEL_L(n)       BITFIELD_VAL(REG_CLASSD_VTOI_IDAC_SEL_L, n)
#define REG_CLASSD_VTOI_VREF_SEL_L          (1 << 4)
#define REG_CLASSD_ADC_VREF_SEL_0P3_L_SHIFT 5
#define REG_CLASSD_ADC_VREF_SEL_0P3_L_MASK  (0x3F << REG_CLASSD_ADC_VREF_SEL_0P3_L_SHIFT)
#define REG_CLASSD_ADC_VREF_SEL_0P3_L(n)    BITFIELD_VAL(REG_CLASSD_ADC_VREF_SEL_0P3_L, n)

// REG_1BC
#define REG_CLASSD_ADC_VREF_SEL_0P6_L_SHIFT 0
#define REG_CLASSD_ADC_VREF_SEL_0P6_L_MASK  (0x3F << REG_CLASSD_ADC_VREF_SEL_0P6_L_SHIFT)
#define REG_CLASSD_ADC_VREF_SEL_0P6_L(n)    BITFIELD_VAL(REG_CLASSD_ADC_VREF_SEL_0P6_L, n)
#define REG_CLASSD_ADC_VREF_SEL_1P2_L_SHIFT 6
#define REG_CLASSD_ADC_VREF_SEL_1P2_L_MASK  (0x3F << REG_CLASSD_ADC_VREF_SEL_1P2_L_SHIFT)
#define REG_CLASSD_ADC_VREF_SEL_1P2_L(n)    BITFIELD_VAL(REG_CLASSD_ADC_VREF_SEL_1P2_L, n)

// REG_1BD
#define REG_CLASSD_ADC_VREF_SEL_1P8_L_SHIFT 0
#define REG_CLASSD_ADC_VREF_SEL_1P8_L_MASK  (0x3F << REG_CLASSD_ADC_VREF_SEL_1P8_L_SHIFT)
#define REG_CLASSD_ADC_VREF_SEL_1P8_L(n)    BITFIELD_VAL(REG_CLASSD_ADC_VREF_SEL_1P8_L, n)

// REG_1BE
#define REG_CLASSD_AUTO_PART_RESET_L        (1 << 6)
#define REG_CLASSD_VTOI_VCASCODE_SEL_L_SHIFT 7
#define REG_CLASSD_VTOI_VCASCODE_SEL_L_MASK (0x1F << REG_CLASSD_VTOI_VCASCODE_SEL_L_SHIFT)
#define REG_CLASSD_VTOI_VCASCODE_SEL_L(n)   BITFIELD_VAL(REG_CLASSD_VTOI_VCASCODE_SEL_L, n)

// REG_1BF
#define REG_POWER_DOWEN_DIRECT              (1 << 0)
#define DIG_CLASSD_RSTN_L_DR                (1 << 1)
#define REG_CLASSD_RSTN_L                   (1 << 2)
#define DIG_CLASSD_EN_CLASSD_L_DR           (1 << 3)
#define REG_CLASSD_EN_CLASSD_L              (1 << 4)
#define DIG_CLASSD_EN_VTOI_L_DR             (1 << 5)
#define REG_CLASSD_EN_VTOI_L                (1 << 6)

// REG_1C0
#define DEM_H7_MODE_CH0                     (1 << 0)
#define DEM_H7_IND_START_CH0_SHIFT          1
#define DEM_H7_IND_START_CH0_MASK           (0x1F << DEM_H7_IND_START_CH0_SHIFT)
#define DEM_H7_IND_START_CH0(n)             BITFIELD_VAL(DEM_H7_IND_START_CH0, n)
#define DEM_L5_MODE_CH0                     (1 << 6)
#define DEM_L5_IND_START_CH0_SHIFT          7
#define DEM_L5_IND_START_CH0_MASK           (0x7 << DEM_L5_IND_START_CH0_SHIFT)
#define DEM_L5_IND_START_CH0(n)             BITFIELD_VAL(DEM_L5_IND_START_CH0, n)
#define CFG_DEM_H7_P_CH0_RST                (1 << 10)
#define CFG_DEM_H7_N_CH0_RST                (1 << 11)
#define CFG_DEM_L5_P_CH0_RST                (1 << 12)
#define CFG_DEM_L5_N_CH0_RST                (1 << 13)
#define CFG_TX_CH0_MUTE_CLASSD              (1 << 14)

// REG_1C1
#define DEM_H7_P_CH0_RSTVAL_15_0_SHIFT      0
#define DEM_H7_P_CH0_RSTVAL_15_0_MASK       (0xFFFF << DEM_H7_P_CH0_RSTVAL_15_0_SHIFT)
#define DEM_H7_P_CH0_RSTVAL_15_0(n)         BITFIELD_VAL(DEM_H7_P_CH0_RSTVAL_15_0, n)

// REG_1C2
#define DEM_H7_P_CH0_RSTVAL_31_16_SHIFT     0
#define DEM_H7_P_CH0_RSTVAL_31_16_MASK      (0xFFFF << DEM_H7_P_CH0_RSTVAL_31_16_SHIFT)
#define DEM_H7_P_CH0_RSTVAL_31_16(n)        BITFIELD_VAL(DEM_H7_P_CH0_RSTVAL_31_16, n)

// REG_1C3
#define DEM_H7_N_CH0_RSTVAL_15_0_SHIFT      0
#define DEM_H7_N_CH0_RSTVAL_15_0_MASK       (0xFFFF << DEM_H7_N_CH0_RSTVAL_15_0_SHIFT)
#define DEM_H7_N_CH0_RSTVAL_15_0(n)         BITFIELD_VAL(DEM_H7_N_CH0_RSTVAL_15_0, n)

// REG_1C4
#define DEM_H7_N_CH0_RSTVAL_31_16_SHIFT     0
#define DEM_H7_N_CH0_RSTVAL_31_16_MASK      (0xFFFF << DEM_H7_N_CH0_RSTVAL_31_16_SHIFT)
#define DEM_H7_N_CH0_RSTVAL_31_16(n)        BITFIELD_VAL(DEM_H7_N_CH0_RSTVAL_31_16, n)

// REG_1C5
#define DEM_L5_P_CH0_RSTVAL_SHIFT           0
#define DEM_L5_P_CH0_RSTVAL_MASK            (0xFF << DEM_L5_P_CH0_RSTVAL_SHIFT)
#define DEM_L5_P_CH0_RSTVAL(n)              BITFIELD_VAL(DEM_L5_P_CH0_RSTVAL, n)
#define DEM_L5_N_CH0_RSTVAL_SHIFT           8
#define DEM_L5_N_CH0_RSTVAL_MASK            (0xFF << DEM_L5_N_CH0_RSTVAL_SHIFT)
#define DEM_L5_N_CH0_RSTVAL(n)              BITFIELD_VAL(DEM_L5_N_CH0_RSTVAL, n)

// REG_1C6
#define REG_CODEC_TX_EN_LDAC_ANA_CLASSD     (1 << 0)
#define DIG_CLASSD_EIG_MODE_L               (1 << 1)
#define DIG_CLASSD_EIG_PATTERN_BYPASS_L     (1 << 2)
#define DIG_CLASSD_EN_OFF_L                 (1 << 3)
#define DIG_CLASSD_GAIN_N18DB_L             (1 << 4)
#define DIG_CLASSD_PATTERN_MODE_L           (1 << 5)
#define DIG_CLASSD_ADC_VREF_SEL_2_L_SHIFT   6
#define DIG_CLASSD_ADC_VREF_SEL_2_L_MASK    (0x7 << DIG_CLASSD_ADC_VREF_SEL_2_L_SHIFT)
#define DIG_CLASSD_ADC_VREF_SEL_2_L(n)      BITFIELD_VAL(DIG_CLASSD_ADC_VREF_SEL_2_L, n)

#endif

