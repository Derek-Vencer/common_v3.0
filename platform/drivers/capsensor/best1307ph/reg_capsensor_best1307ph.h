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
#ifndef __REG_CAPSENSOR_BEST1307PH_H__
#define __REG_CAPSENSOR_BEST1307PH_H__

#ifdef __cplusplus
extern "C" {
#endif

enum CAPSENSOR_T {
    CAP_REG_00 = 0x0,
    CAP_REG_01,
    CAP_REG_02,
    CAP_REG_03,
    CAP_REG_04,
    CAP_REG_05,
    CAP_REG_06,
    CAP_REG_07,
    CAP_REG_08,
    CAP_REG_09,
    CAP_REG_0A,
    CAP_REG_0B,
    CAP_REG_0C,
    CAP_REG_0D,
    CAP_REG_0E,
    CAP_REG_0F,

    CAP_REG_10,
    CAP_REG_11,
    CAP_REG_12,
    CAP_REG_13,
    CAP_REG_14,
    CAP_REG_15,
    CAP_REG_16,
    CAP_REG_17,
    CAP_REG_18,
    CAP_REG_19,
    CAP_REG_1A,
    CAP_REG_1B,
    CAP_REG_1C,
    CAP_REG_1D,
    CAP_REG_1E,
    CAP_REG_1F,

    CAP_REG_20,
    CAP_REG_21,
    CAP_REG_22,
    CAP_REG_23,
    CAP_REG_24,
    CAP_REG_25,
    CAP_REG_26,
    CAP_REG_27,
    CAP_REG_28,
    CAP_REG_29,
    CAP_REG_2A,
    CAP_REG_2B,
    CAP_REG_2C,
    CAP_REG_2D,
    CAP_REG_2E,
    CAP_REG_2F,

    CAP_REG_30,
    CAP_REG_31,
    CAP_REG_32,
    CAP_REG_33,
    CAP_REG_34,
    CAP_REG_35,
    CAP_REG_36,
    CAP_REG_37,
    CAP_REG_38,
    CAP_REG_39,
    CAP_REG_3A,
    CAP_REG_3B,
    CAP_REG_3C,
    CAP_REG_3D,
    CAP_REG_3E,
    CAP_REG_3F,

    CAP_REG_40,
    CAP_REG_41,
    CAP_REG_42,
    CAP_REG_43,
    CAP_REG_44,
    CAP_REG_45,
    CAP_REG_46,
    CAP_REG_47,
    CAP_REG_48,
    CAP_REG_49,
    CAP_REG_4A,
    CAP_REG_4B,
    CAP_REG_4C,
    CAP_REG_4D,
    CAP_REG_4E,
    CAP_REG_4F,

    CAP_REG_50,
    CAP_REG_51,
    CAP_REG_52,
    CAP_REG_53,
    CAP_REG_54,
    CAP_REG_55,
    CAP_REG_56,
    CAP_REG_57,
    CAP_REG_58,
    CAP_REG_59,
    CAP_REG_5A,
    CAP_REG_5B,
    CAP_REG_5C,
    CAP_REG_5D,
    CAP_REG_5E,
    CAP_REG_5F,

    CAP_REG_60,
    CAP_REG_61,
    CAP_REG_62,
    CAP_REG_63,
    CAP_REG_64,
    CAP_REG_65,
    CAP_REG_66,
    CAP_REG_67,
    CAP_REG_68,
    CAP_REG_69,
    CAP_REG_6A,
    CAP_REG_6B,
    CAP_REG_6C,
    CAP_REG_6D,
    CAP_REG_6E,
    CAP_REG_6F,

    CAP_REG_70,
    CAP_REG_71,
    CAP_REG_72,
    CAP_REG_73,
    CAP_REG_74,
    CAP_REG_75,
    CAP_REG_76,
    CAP_REG_77,
    CAP_REG_78,
    CAP_REG_79,
    CAP_REG_7A,
    CAP_REG_7B,
    CAP_REG_7C,
    CAP_REG_7D,
    CAP_REG_7E,
    CAP_REG_7F,

    CAP_REG_80,
    CAP_REG_81,
    CAP_REG_82,
    CAP_REG_83,
    CAP_REG_84,
    CAP_REG_85,
    CAP_REG_86,
    CAP_REG_87,
    CAP_REG_88,
    CAP_REG_89,
    CAP_REG_8A,
    CAP_REG_8B,
    CAP_REG_8C,
    CAP_REG_8D,
    CAP_REG_8E,
    CAP_REG_8F,

    CAP_REG_90,
    CAP_REG_91,
    CAP_REG_92,
    CAP_REG_93,
    CAP_REG_94,
    CAP_REG_95,
    CAP_REG_96,
    CAP_REG_97,
    CAP_REG_98,
    CAP_REG_99,
    CAP_REG_9A,
    CAP_REG_9B,
    CAP_REG_9C,
    CAP_REG_9D,
    CAP_REG_9E,
    CAP_REG_9F,

    CAP_REG_A0,
    CAP_REG_A1,
    CAP_REG_A2,
    CAP_REG_A3,
};

// REG_0x01
#define FSM_EN                              (1 << 0)
#define CDC_SDM_MODE_DR                     (1 << 1)
#define CDC_SDM_MODE_REG                    (1 << 2)
#define CDC_SAR_P_MODE_DR                   (1 << 4)
#define CDC_SAR_P_MODE_REG                  (1 << 5)
#define CDC_CAL_P_MODE_DR                   (1 << 8)
#define CDC_CAL_P_MODE_REG                  (1 << 9)
#define CDC_PU_VREF0P7                      (1 << 12)
#define CDC_PU_LDO                          (1 << 13)
#define CDC_PU_CDC_DR                       (1 << 14)
#define CDC_PU_CDC_REG                      (1 << 15)

// REG_0x02
#define CDC_PU_P_CHAN_DR                    (1 << 2)
#define CDC_PU_P_CHAN_REG                   (1 << 3)
#define ACTIVE_CH_NUM_SHIFT                 4
#define ACTIVE_CH_NUM_MASK                  (0x7 << ACTIVE_CH_NUM_SHIFT)
#define ACTIVE_CH_NUM(n)                    BITFIELD_VAL(ACTIVE_CH_NUM, n)
#define OPERATION_MODE_SHIFT                7
#define OPERATION_MODE_MASK                 (0x3 << OPERATION_MODE_SHIFT)
#define OPERATION_MODE(n)                   BITFIELD_VAL(OPERATION_MODE, n)
#define POWER_MODE_REG                      (1 << 9)
#define CDC_DLATCH_IBSEL_SHIFT              10
#define CDC_DLATCH_IBSEL_MASK               (0xF << CDC_DLATCH_IBSEL_SHIFT)
#define CDC_DLATCH_IBSEL(n)                 BITFIELD_VAL(CDC_DLATCH_IBSEL, n)
#define CDC_RSTB                            (1 << 14)

// REG_0x03
#define CDC_SAR_P_BIT_IN_DR                 (1 << 0)
#define PDAC_MODE_DR                        (1 << 5)
#define PDAC_MODE_REG                       (1 << 6)
#define CDC_LDO_ISEL_SHIFT                  9
#define CDC_LDO_ISEL_MASK                   (0xF << CDC_LDO_ISEL_SHIFT)
#define CDC_LDO_ISEL(n)                     BITFIELD_VAL(CDC_LDO_ISEL, n)

// REG_0x04
#define CH_IDX_1_SAMPLE_POINT_SHIFT         0
#define CH_IDX_1_SAMPLE_POINT_MASK          (0xFF << CH_IDX_1_SAMPLE_POINT_SHIFT)
#define CH_IDX_1_SAMPLE_POINT(n)            BITFIELD_VAL(CH_IDX_1_SAMPLE_POINT, n)
#define CH_IDX_0_SAMPLE_POINT_SHIFT         8
#define CH_IDX_0_SAMPLE_POINT_MASK          (0xFF << CH_IDX_0_SAMPLE_POINT_SHIFT)
#define CH_IDX_0_SAMPLE_POINT(n)            BITFIELD_VAL(CH_IDX_0_SAMPLE_POINT, n)

// REG_0x05
#define CH_IDX_3_SAMPLE_POINT_SHIFT         0
#define CH_IDX_3_SAMPLE_POINT_MASK          (0xFF << CH_IDX_3_SAMPLE_POINT_SHIFT)
#define CH_IDX_3_SAMPLE_POINT(n)            BITFIELD_VAL(CH_IDX_3_SAMPLE_POINT, n)
#define CH_IDX_2_SAMPLE_POINT_SHIFT         8
#define CH_IDX_2_SAMPLE_POINT_MASK          (0xFF << CH_IDX_2_SAMPLE_POINT_SHIFT)
#define CH_IDX_2_SAMPLE_POINT(n)            BITFIELD_VAL(CH_IDX_2_SAMPLE_POINT, n)

// REG_0x06
#define CH_IDX_5_SAMPLE_POINT_SHIFT         0
#define CH_IDX_5_SAMPLE_POINT_MASK          (0xFF << CH_IDX_5_SAMPLE_POINT_SHIFT)
#define CH_IDX_5_SAMPLE_POINT(n)            BITFIELD_VAL(CH_IDX_5_SAMPLE_POINT, n)
#define CH_IDX_4_SAMPLE_POINT_SHIFT         8
#define CH_IDX_4_SAMPLE_POINT_MASK          (0xFF << CH_IDX_4_SAMPLE_POINT_SHIFT)
#define CH_IDX_4_SAMPLE_POINT(n)            BITFIELD_VAL(CH_IDX_4_SAMPLE_POINT, n)

// REG_0x07
#define CH_IDX_7_SAMPLE_POINT_SHIFT         0
#define CH_IDX_7_SAMPLE_POINT_MASK          (0xFF << CH_IDX_7_SAMPLE_POINT_SHIFT)
#define CH_IDX_7_SAMPLE_POINT(n)            BITFIELD_VAL(CH_IDX_7_SAMPLE_POINT, n)
#define CH_IDX_6_SAMPLE_POINT_SHIFT         8
#define CH_IDX_6_SAMPLE_POINT_MASK          (0xFF << CH_IDX_6_SAMPLE_POINT_SHIFT)
#define CH_IDX_6_SAMPLE_POINT(n)            BITFIELD_VAL(CH_IDX_6_SAMPLE_POINT, n)

// REG_0x08
#define CDC_VCMBUF_DRV_SHIFT                0
#define CDC_VCMBUF_DRV_MASK                 (0xF << CDC_VCMBUF_DRV_SHIFT)
#define CDC_VCMBUF_DRV(n)                   BITFIELD_VAL(CDC_VCMBUF_DRV, n)

// REG_0x0C
#define CDC_BASELINE_P_OUT_CH0_REG_SHIFT    0
#define CDC_BASELINE_P_OUT_CH0_REG_MASK     (0x3FF << CDC_BASELINE_P_OUT_CH0_REG_SHIFT)
#define CDC_BASELINE_P_OUT_CH0_REG(n)       BITFIELD_VAL(CDC_BASELINE_P_OUT_CH0_REG, n)
#define CDC_BASELINE_P_BIT_IN_CH0_DR        (1 << 10)

// REG_0x0D
#define CDC_BASELINE_P_OUT_CH1_REG_SHIFT    0
#define CDC_BASELINE_P_OUT_CH1_REG_MASK     (0x3FF << CDC_BASELINE_P_OUT_CH1_REG_SHIFT)
#define CDC_BASELINE_P_OUT_CH1_REG(n)       BITFIELD_VAL(CDC_BASELINE_P_OUT_CH1_REG, n)
#define CDC_BASELINE_P_BIT_IN_CH1_DR        (1 << 10)

// REG_0x0E
#define CDC_BASELINE_P_OUT_CH2_REG_SHIFT    0
#define CDC_BASELINE_P_OUT_CH2_REG_MASK     (0x3FF << CDC_BASELINE_P_OUT_CH2_REG_SHIFT)
#define CDC_BASELINE_P_OUT_CH2_REG(n)       BITFIELD_VAL(CDC_BASELINE_P_OUT_CH2_REG, n)
#define CDC_BASELINE_P_BIT_IN_CH2_DR        (1 << 10)

// REG_0x0F
#define CDC_BASELINE_P_OUT_CH3_REG_SHIFT    0
#define CDC_BASELINE_P_OUT_CH3_REG_MASK     (0x3FF << CDC_BASELINE_P_OUT_CH3_REG_SHIFT)
#define CDC_BASELINE_P_OUT_CH3_REG(n)       BITFIELD_VAL(CDC_BASELINE_P_OUT_CH3_REG, n)
#define CDC_BASELINE_P_BIT_IN_CH3_DR        (1 << 10)

// REG_0x10
#define CDC_BASELINE_P_OUT_CH4_REG_SHIFT    0
#define CDC_BASELINE_P_OUT_CH4_REG_MASK     (0x3FF << CDC_BASELINE_P_OUT_CH4_REG_SHIFT)
#define CDC_BASELINE_P_OUT_CH4_REG(n)       BITFIELD_VAL(CDC_BASELINE_P_OUT_CH4_REG, n)
#define CDC_BASELINE_P_BIT_IN_CH4_DR        (1 << 10)

// REG_0x11
#define CDC_BASELINE_P_OUT_CH5_REG_SHIFT    0
#define CDC_BASELINE_P_OUT_CH5_REG_MASK     (0x3FF << CDC_BASELINE_P_OUT_CH5_REG_SHIFT)
#define CDC_BASELINE_P_OUT_CH5_REG(n)       BITFIELD_VAL(CDC_BASELINE_P_OUT_CH5_REG, n)
#define CDC_BASELINE_P_BIT_IN_CH5_DR        (1 << 10)

// REG_0x12
#define CDC_BASELINE_P_OUT_CH6_REG_SHIFT    0
#define CDC_BASELINE_P_OUT_CH6_REG_MASK     (0x3FF << CDC_BASELINE_P_OUT_CH6_REG_SHIFT)
#define CDC_BASELINE_P_OUT_CH6_REG(n)       BITFIELD_VAL(CDC_BASELINE_P_OUT_CH6_REG, n)
#define CDC_BASELINE_P_BIT_IN_CH6_DR        (1 << 10)

// REG_0x13
#define CDC_BASELINE_P_OUT_CH7_REG_SHIFT    0
#define CDC_BASELINE_P_OUT_CH7_REG_MASK     (0x3FF << CDC_BASELINE_P_OUT_CH7_REG_SHIFT)
#define CDC_BASELINE_P_OUT_CH7_REG(n)       BITFIELD_VAL(CDC_BASELINE_P_OUT_CH7_REG, n)
#define CDC_BASELINE_P_BIT_IN_CH7_DR        (1 << 10)

// REG_0x14
#define CDC_BASELINE_P_OUT_CH0_R_SHIFT      0
#define CDC_BASELINE_P_OUT_CH0_R_MASK       (0x3FF << CDC_BASELINE_P_OUT_CH0_R_SHIFT)
#define CDC_BASELINE_P_OUT_CH0_R(n)         BITFIELD_VAL(CDC_BASELINE_P_OUT_CH0_R, n)

// REG_0x15
#define CDC_BASELINE_P_OUT_CH1_R_SHIFT      0
#define CDC_BASELINE_P_OUT_CH1_R_MASK       (0x3FF << CDC_BASELINE_P_OUT_CH1_R_SHIFT)
#define CDC_BASELINE_P_OUT_CH1_R(n)         BITFIELD_VAL(CDC_BASELINE_P_OUT_CH1_R, n)

// REG_0x16
#define CDC_BASELINE_P_OUT_CH2_R_SHIFT      0
#define CDC_BASELINE_P_OUT_CH2_R_MASK       (0x3FF << CDC_BASELINE_P_OUT_CH2_R_SHIFT)
#define CDC_BASELINE_P_OUT_CH2_R(n)         BITFIELD_VAL(CDC_BASELINE_P_OUT_CH2_R, n)

// REG_0x17
#define CDC_BASELINE_P_OUT_CH3_R_SHIFT      0
#define CDC_BASELINE_P_OUT_CH3_R_MASK       (0x3FF << CDC_BASELINE_P_OUT_CH3_R_SHIFT)
#define CDC_BASELINE_P_OUT_CH3_R(n)         BITFIELD_VAL(CDC_BASELINE_P_OUT_CH3_R, n)

// REG_0x18
#define CDC_BASELINE_P_OUT_CH4_R_SHIFT      0
#define CDC_BASELINE_P_OUT_CH4_R_MASK       (0x3FF << CDC_BASELINE_P_OUT_CH4_R_SHIFT)
#define CDC_BASELINE_P_OUT_CH4_R(n)         BITFIELD_VAL(CDC_BASELINE_P_OUT_CH4_R, n)

// REG_0x19
#define CDC_BASELINE_P_OUT_CH5_R_SHIFT      0
#define CDC_BASELINE_P_OUT_CH5_R_MASK       (0x3FF << CDC_BASELINE_P_OUT_CH5_R_SHIFT)
#define CDC_BASELINE_P_OUT_CH5_R(n)         BITFIELD_VAL(CDC_BASELINE_P_OUT_CH5_R, n)

// REG_0x1A
#define CDC_BASELINE_P_OUT_CH6_R_SHIFT      0
#define CDC_BASELINE_P_OUT_CH6_R_MASK       (0x3FF << CDC_BASELINE_P_OUT_CH6_R_SHIFT)
#define CDC_BASELINE_P_OUT_CH6_R(n)         BITFIELD_VAL(CDC_BASELINE_P_OUT_CH6_R, n)

// REG_0x1B
#define CDC_BASELINE_P_OUT_CH7_R_SHIFT      0
#define CDC_BASELINE_P_OUT_CH7_R_MASK       (0x3FF << CDC_BASELINE_P_OUT_CH7_R_SHIFT)
#define CDC_BASELINE_P_OUT_CH7_R(n)         BITFIELD_VAL(CDC_BASELINE_P_OUT_CH7_R, n)

// REG_0x24
#define CDC_SAR_P_OUT_CH0_REG_SHIFT         0
#define CDC_SAR_P_OUT_CH0_REG_MASK          (0x3FF << CDC_SAR_P_OUT_CH0_REG_SHIFT)
#define CDC_SAR_P_OUT_CH0_REG(n)            BITFIELD_VAL(CDC_SAR_P_OUT_CH0_REG, n)
#define CDC_MUTUAL_MODE_CH0_REG             (1 << 10)
#define CDC_COMP_1K_EN_CH0_REG              (1 << 11)
#define CDC_VTX_CHAN_CH0_SEL_REG_SHIFT      13
#define CDC_VTX_CHAN_CH0_SEL_REG_MASK       (0x7 << CDC_VTX_CHAN_CH0_SEL_REG_SHIFT)
#define CDC_VTX_CHAN_CH0_SEL_REG(n)         BITFIELD_VAL(CDC_VTX_CHAN_CH0_SEL_REG, n)

// REG_0x25
#define CDC_SAR_P_OUT_CH1_REG_SHIFT         0
#define CDC_SAR_P_OUT_CH1_REG_MASK          (0x3FF << CDC_SAR_P_OUT_CH1_REG_SHIFT)
#define CDC_SAR_P_OUT_CH1_REG(n)            BITFIELD_VAL(CDC_SAR_P_OUT_CH1_REG, n)
#define CDC_MUTUAL_MODE_CH1_REG             (1 << 10)
#define CDC_COMP_1K_EN_CH1_REG              (1 << 11)
#define CDC_VTX_CHAN_CH1_SEL_REG_SHIFT      13
#define CDC_VTX_CHAN_CH1_SEL_REG_MASK       (0x7 << CDC_VTX_CHAN_CH1_SEL_REG_SHIFT)
#define CDC_VTX_CHAN_CH1_SEL_REG(n)         BITFIELD_VAL(CDC_VTX_CHAN_CH1_SEL_REG, n)

// REG_0x26
#define CDC_SAR_P_OUT_CH2_REG_SHIFT         0
#define CDC_SAR_P_OUT_CH2_REG_MASK          (0x3FF << CDC_SAR_P_OUT_CH2_REG_SHIFT)
#define CDC_SAR_P_OUT_CH2_REG(n)            BITFIELD_VAL(CDC_SAR_P_OUT_CH2_REG, n)
#define CDC_MUTUAL_MODE_CH2_REG             (1 << 10)
#define CDC_COMP_1K_EN_CH2_REG              (1 << 11)
#define CDC_VTX_CHAN_CH2_SEL_REG_SHIFT      13
#define CDC_VTX_CHAN_CH2_SEL_REG_MASK       (0x7 << CDC_VTX_CHAN_CH2_SEL_REG_SHIFT)
#define CDC_VTX_CHAN_CH2_SEL_REG(n)         BITFIELD_VAL(CDC_VTX_CHAN_CH2_SEL_REG, n)

// REG_0x27
#define CDC_SAR_P_OUT_CH3_REG_SHIFT         0
#define CDC_SAR_P_OUT_CH3_REG_MASK          (0x3FF << CDC_SAR_P_OUT_CH3_REG_SHIFT)
#define CDC_SAR_P_OUT_CH3_REG(n)            BITFIELD_VAL(CDC_SAR_P_OUT_CH3_REG, n)
#define CDC_MUTUAL_MODE_CH3_REG             (1 << 10)
#define CDC_COMP_1K_EN_CH3_REG              (1 << 11)
#define CDC_VTX_CHAN_CH3_SEL_REG_SHIFT      13
#define CDC_VTX_CHAN_CH3_SEL_REG_MASK       (0x7 << CDC_VTX_CHAN_CH3_SEL_REG_SHIFT)
#define CDC_VTX_CHAN_CH3_SEL_REG(n)         BITFIELD_VAL(CDC_VTX_CHAN_CH3_SEL_REG, n)

// REG_0x28
#define CDC_SAR_P_OUT_CH4_REG_SHIFT         0
#define CDC_SAR_P_OUT_CH4_REG_MASK          (0x3FF << CDC_SAR_P_OUT_CH4_REG_SHIFT)
#define CDC_SAR_P_OUT_CH4_REG(n)            BITFIELD_VAL(CDC_SAR_P_OUT_CH4_REG, n)
#define CDC_MUTUAL_MODE_CH4_REG             (1 << 10)
#define CDC_COMP_1K_EN_CH4_REG              (1 << 11)
#define CDC_VTX_CHAN_CH4_SEL_REG_SHIFT      13
#define CDC_VTX_CHAN_CH4_SEL_REG_MASK       (0x7 << CDC_VTX_CHAN_CH4_SEL_REG_SHIFT)
#define CDC_VTX_CHAN_CH4_SEL_REG(n)         BITFIELD_VAL(CDC_VTX_CHAN_CH4_SEL_REG, n)

// REG_0x29
#define CDC_SAR_P_OUT_CH5_REG_SHIFT         0
#define CDC_SAR_P_OUT_CH5_REG_MASK          (0x3FF << CDC_SAR_P_OUT_CH5_REG_SHIFT)
#define CDC_SAR_P_OUT_CH5_REG(n)            BITFIELD_VAL(CDC_SAR_P_OUT_CH5_REG, n)
#define CDC_MUTUAL_MODE_CH5_REG             (1 << 10)
#define CDC_COMP_1K_EN_CH5_REG              (1 << 11)
#define CDC_VTX_CHAN_CH5_SEL_REG_SHIFT      13
#define CDC_VTX_CHAN_CH5_SEL_REG_MASK       (0x7 << CDC_VTX_CHAN_CH5_SEL_REG_SHIFT)
#define CDC_VTX_CHAN_CH5_SEL_REG(n)         BITFIELD_VAL(CDC_VTX_CHAN_CH5_SEL_REG, n)

// REG_0x2A
#define CDC_SAR_P_OUT_CH6_REG_SHIFT         0
#define CDC_SAR_P_OUT_CH6_REG_MASK          (0x3FF << CDC_SAR_P_OUT_CH6_REG_SHIFT)
#define CDC_SAR_P_OUT_CH6_REG(n)            BITFIELD_VAL(CDC_SAR_P_OUT_CH6_REG, n)
#define CDC_MUTUAL_MODE_CH6_REG             (1 << 10)
#define CDC_COMP_1K_EN_CH6_REG              (1 << 11)
#define CDC_VTX_CHAN_CH6_SEL_REG_SHIFT      13
#define CDC_VTX_CHAN_CH6_SEL_REG_MASK       (0x7 << CDC_VTX_CHAN_CH6_SEL_REG_SHIFT)
#define CDC_VTX_CHAN_CH6_SEL_REG(n)         BITFIELD_VAL(CDC_VTX_CHAN_CH6_SEL_REG, n)

// REG_0x2B
#define CDC_SAR_P_OUT_CH7_REG_SHIFT         0
#define CDC_SAR_P_OUT_CH7_REG_MASK          (0x3FF << CDC_SAR_P_OUT_CH7_REG_SHIFT)
#define CDC_SAR_P_OUT_CH7_REG(n)            BITFIELD_VAL(CDC_SAR_P_OUT_CH7_REG, n)
#define CDC_MUTUAL_MODE_CH7_REG             (1 << 10)
#define CDC_COMP_1K_EN_CH7_REG              (1 << 11)
#define CDC_VTX_CHAN_CH7_SEL_REG_SHIFT      13
#define CDC_VTX_CHAN_CH7_SEL_REG_MASK       (0x7 << CDC_VTX_CHAN_CH7_SEL_REG_SHIFT)
#define CDC_VTX_CHAN_CH7_SEL_REG(n)         BITFIELD_VAL(CDC_VTX_CHAN_CH7_SEL_REG, n)

// REG_0x2C
#define CDC_SAR_P_OUT_CH0_R_SHIFT           0
#define CDC_SAR_P_OUT_CH0_R_MASK            (0x3FF << CDC_SAR_P_OUT_CH0_R_SHIFT)
#define CDC_SAR_P_OUT_CH0_R(n)              BITFIELD_VAL(CDC_SAR_P_OUT_CH0_R, n)

// REG_0x2D
#define CDC_SAR_P_OUT_CH1_R_SHIFT           0
#define CDC_SAR_P_OUT_CH1_R_MASK            (0x3FF << CDC_SAR_P_OUT_CH1_R_SHIFT)
#define CDC_SAR_P_OUT_CH1_R(n)              BITFIELD_VAL(CDC_SAR_P_OUT_CH1_R, n)

// REG_0x2E
#define CDC_SAR_P_OUT_CH2_R_SHIFT           0
#define CDC_SAR_P_OUT_CH2_R_MASK            (0x3FF << CDC_SAR_P_OUT_CH2_R_SHIFT)
#define CDC_SAR_P_OUT_CH2_R(n)              BITFIELD_VAL(CDC_SAR_P_OUT_CH2_R, n)

// REG_0x2F
#define CDC_SAR_P_OUT_CH3_R_SHIFT           0
#define CDC_SAR_P_OUT_CH3_R_MASK            (0x3FF << CDC_SAR_P_OUT_CH3_R_SHIFT)
#define CDC_SAR_P_OUT_CH3_R(n)              BITFIELD_VAL(CDC_SAR_P_OUT_CH3_R, n)

// REG_0x30
#define CDC_SAR_P_OUT_CH4_R_SHIFT           0
#define CDC_SAR_P_OUT_CH4_R_MASK            (0x3FF << CDC_SAR_P_OUT_CH4_R_SHIFT)
#define CDC_SAR_P_OUT_CH4_R(n)              BITFIELD_VAL(CDC_SAR_P_OUT_CH4_R, n)

// REG_0x31
#define CDC_SAR_P_OUT_CH5_R_SHIFT           0
#define CDC_SAR_P_OUT_CH5_R_MASK            (0x3FF << CDC_SAR_P_OUT_CH5_R_SHIFT)
#define CDC_SAR_P_OUT_CH5_R(n)              BITFIELD_VAL(CDC_SAR_P_OUT_CH5_R, n)

// REG_0x32
#define CDC_SAR_P_OUT_CH6_R_SHIFT           0
#define CDC_SAR_P_OUT_CH6_R_MASK            (0x3FF << CDC_SAR_P_OUT_CH6_R_SHIFT)
#define CDC_SAR_P_OUT_CH6_R(n)              BITFIELD_VAL(CDC_SAR_P_OUT_CH6_R, n)

// REG_0x33
#define CDC_SAR_P_OUT_CH7_R_SHIFT           0
#define CDC_SAR_P_OUT_CH7_R_MASK            (0x3FF << CDC_SAR_P_OUT_CH7_R_SHIFT)
#define CDC_SAR_P_OUT_CH7_R(n)              BITFIELD_VAL(CDC_SAR_P_OUT_CH7_R, n)

// REG_0x3C
#define LP_SLEEP_TIME_SHIFT                 0
#define LP_SLEEP_TIME_MASK                  (0x3FFF << LP_SLEEP_TIME_SHIFT)
#define LP_SLEEP_TIME(n)                    BITFIELD_VAL(LP_SLEEP_TIME, n)

// REG_0x3D
#define NEXT_STATE_SHIFT                    0
#define NEXT_STATE_MASK                     (0x7 << NEXT_STATE_SHIFT)
#define NEXT_STATE(n)                       BITFIELD_VAL(NEXT_STATE, n)
#define STATE_SHIFT                         3
#define STATE_MASK                          (0x7 << STATE_SHIFT)
#define STATE(n)                            BITFIELD_VAL(STATE, n)

// REG_0x3E
#define FIFO_DATA_NUM_TH_SHIFT              0
#define FIFO_DATA_NUM_TH_MASK               (0xFF << FIFO_DATA_NUM_TH_SHIFT)
#define FIFO_DATA_NUM_TH(n)                 BITFIELD_VAL(FIFO_DATA_NUM_TH, n)
#define CHIP_ADDR_I2C_SHIFT                 8
#define CHIP_ADDR_I2C_MASK                  (0x7F << CHIP_ADDR_I2C_SHIFT)
#define CHIP_ADDR_I2C(n)                    BITFIELD_VAL(CHIP_ADDR_I2C, n)
#define CDC_PU_CHOP                         (1 << 15)

// REG_0x43
#define FIFO_OUT_HI_SHIFT                   0
#define FIFO_OUT_HI_MASK                    (0x3FFF << FIFO_OUT_HI_SHIFT)
#define FIFO_OUT_HI(n)                      BITFIELD_VAL(FIFO_OUT_HI, n)

// REG_0x44
#define FIFO_OUT_LO_SHIFT                   0
#define FIFO_OUT_LO_MASK                    (0xFFFF << FIFO_OUT_LO_SHIFT)
#define FIFO_OUT_LO(n)                      BITFIELD_VAL(FIFO_OUT_LO, n)

// REG_0x45
#define CFG_ADC_OUT_PHASE_SHIFT             0
#define CFG_ADC_OUT_PHASE_MASK              (0x7 << CFG_ADC_OUT_PHASE_SHIFT)
#define CFG_ADC_OUT_PHASE(n)                BITFIELD_VAL(CFG_ADC_OUT_PHASE, n)
#define CFG_ADC_OUT_DECIM_SHIFT             3
#define CFG_ADC_OUT_DECIM_MASK              (0x7 << CFG_ADC_OUT_DECIM_SHIFT)
#define CFG_ADC_OUT_DECIM(n)                BITFIELD_VAL(CFG_ADC_OUT_DECIM, n)
#define CFG_CLK_ADC_INV                     (1 << 6)

// REG_0x46
#define HBF2_COEF_01_SHIFT                  0
#define HBF2_COEF_01_MASK                   (0xFFFF << HBF2_COEF_01_SHIFT)
#define HBF2_COEF_01(n)                     BITFIELD_VAL(HBF2_COEF_01, n)

// REG_0x47
#define HBF2_COEF_02_SHIFT                  0
#define HBF2_COEF_02_MASK                   (0xFFFF << HBF2_COEF_02_SHIFT)
#define HBF2_COEF_02(n)                     BITFIELD_VAL(HBF2_COEF_02, n)

// REG_0x48
#define HBF2_COEF_03_SHIFT                  0
#define HBF2_COEF_03_MASK                   (0xFFFF << HBF2_COEF_03_SHIFT)
#define HBF2_COEF_03(n)                     BITFIELD_VAL(HBF2_COEF_03, n)

// REG_0x49
#define HBF2_COEF_04_SHIFT                  0
#define HBF2_COEF_04_MASK                   (0xFFFF << HBF2_COEF_04_SHIFT)
#define HBF2_COEF_04(n)                     BITFIELD_VAL(HBF2_COEF_04, n)

// REG_0x4A
#define HBF2_COEF_05_SHIFT                  0
#define HBF2_COEF_05_MASK                   (0xFFFF << HBF2_COEF_05_SHIFT)
#define HBF2_COEF_05(n)                     BITFIELD_VAL(HBF2_COEF_05, n)

// REG_0x4B
#define HBF2_COEF_06_SHIFT                  0
#define HBF2_COEF_06_MASK                   (0xFFFF << HBF2_COEF_06_SHIFT)
#define HBF2_COEF_06(n)                     BITFIELD_VAL(HBF2_COEF_06, n)

// REG_0x4C
#define HBF2_COEF_07_SHIFT                  0
#define HBF2_COEF_07_MASK                   (0xFFFF << HBF2_COEF_07_SHIFT)
#define HBF2_COEF_07(n)                     BITFIELD_VAL(HBF2_COEF_07, n)

// REG_0x4D
#define HBF2_COEF_08_SHIFT                  0
#define HBF2_COEF_08_MASK                   (0xFFFF << HBF2_COEF_08_SHIFT)
#define HBF2_COEF_08(n)                     BITFIELD_VAL(HBF2_COEF_08, n)

// REG_0x4E
#define HBF2_COEF_09_SHIFT                  0
#define HBF2_COEF_09_MASK                   (0xFFFF << HBF2_COEF_09_SHIFT)
#define HBF2_COEF_09(n)                     BITFIELD_VAL(HBF2_COEF_09, n)

// REG_0x4F
#define RSVED_CDC_SHIFT                     0
#define RSVED_CDC_MASK                      (0xFFFF << RSVED_CDC_SHIFT)
#define RSVED_CDC(n)                        BITFIELD_VAL(RSVED_CDC, n)

// REG_0x52
#define FIFO_IN_HI_SHIFT                    0
#define FIFO_IN_HI_MASK                     (0x3FFF << FIFO_IN_HI_SHIFT)
#define FIFO_IN_HI(n)                       BITFIELD_VAL(FIFO_IN_HI, n)

// REG_0x53
#define FIFO_IN_LO_SHIFT                    0
#define FIFO_IN_LO_MASK                     (0xFFFF << FIFO_IN_LO_SHIFT)
#define FIFO_IN_LO(n)                       BITFIELD_VAL(FIFO_IN_LO, n)

// REG_0x54
#define FIFO_WR_CLK_DR                      (1 << 0)
#define FIFO_RD_CLK_DR                      (1 << 1)
#define FIFO_CLK_DR                         (1 << 3)
#define FIFO_RD_DIRECTION_REG               (1 << 4)
#define FIFO_RD_START_REG                   (1 << 5)

// REG_0x55
#define CH_CONFIG_IDX_0_SHIFT               0
#define CH_CONFIG_IDX_0_MASK                (0x7F << CH_CONFIG_IDX_0_SHIFT)
#define CH_CONFIG_IDX_0(n)                  BITFIELD_VAL(CH_CONFIG_IDX_0, n)
#define CH_CONFIG_IDX_1_SHIFT               8
#define CH_CONFIG_IDX_1_MASK                (0x7F << CH_CONFIG_IDX_1_SHIFT)
#define CH_CONFIG_IDX_1(n)                  BITFIELD_VAL(CH_CONFIG_IDX_1, n)

// REG_0x56
#define CH_CONFIG_IDX_2_SHIFT               0
#define CH_CONFIG_IDX_2_MASK                (0x7F << CH_CONFIG_IDX_2_SHIFT)
#define CH_CONFIG_IDX_2(n)                  BITFIELD_VAL(CH_CONFIG_IDX_2, n)
#define CH_CONFIG_IDX_3_SHIFT               8
#define CH_CONFIG_IDX_3_MASK                (0x7F << CH_CONFIG_IDX_3_SHIFT)
#define CH_CONFIG_IDX_3(n)                  BITFIELD_VAL(CH_CONFIG_IDX_3, n)

// REG_0x57
#define CH_CONFIG_IDX_4_SHIFT               0
#define CH_CONFIG_IDX_4_MASK                (0x7F << CH_CONFIG_IDX_4_SHIFT)
#define CH_CONFIG_IDX_4(n)                  BITFIELD_VAL(CH_CONFIG_IDX_4, n)
#define CH_CONFIG_IDX_5_SHIFT               8
#define CH_CONFIG_IDX_5_MASK                (0x7F << CH_CONFIG_IDX_5_SHIFT)
#define CH_CONFIG_IDX_5(n)                  BITFIELD_VAL(CH_CONFIG_IDX_5, n)

// REG_0x58
#define CH_CONFIG_IDX_6_SHIFT               0
#define CH_CONFIG_IDX_6_MASK                (0x7F << CH_CONFIG_IDX_6_SHIFT)
#define CH_CONFIG_IDX_6(n)                  BITFIELD_VAL(CH_CONFIG_IDX_6, n)
#define CH_CONFIG_IDX_7_SHIFT               8
#define CH_CONFIG_IDX_7_MASK                (0x7F << CH_CONFIG_IDX_7_SHIFT)
#define CH_CONFIG_IDX_7(n)                  BITFIELD_VAL(CH_CONFIG_IDX_7, n)

// REG_0x5A
#define CDC_SDM_OP_CO1_SHIFT                4
#define CDC_SDM_OP_CO1_MASK                 (0x7 << CDC_SDM_OP_CO1_SHIFT)
#define CDC_SDM_OP_CO1(n)                   BITFIELD_VAL(CDC_SDM_OP_CO1, n)

// REG_0x5B
#define CH_COUNT_REG_SHIFT                  0
#define CH_COUNT_REG_MASK                   (0x7 << CH_COUNT_REG_SHIFT)
#define CH_COUNT_REG(n)                     BITFIELD_VAL(CH_COUNT_REG, n)
#define CH_COUNT_DR                         (1 << 3)

// REG_0x5F
#define ADC_HBF1_BYPASS_CH0                 (1 << 0)
#define ADC_HBF1_BYPASS_CH1                 (1 << 1)
#define ADC_HBF1_BYPASS_CH2                 (1 << 2)
#define ADC_HBF1_BYPASS_CH3                 (1 << 3)
#define ADC_HBF1_BYPASS_CH4                 (1 << 4)
#define ADC_HBF1_BYPASS_CH5                 (1 << 5)
#define ADC_HBF1_BYPASS_CH6                 (1 << 6)
#define ADC_HBF1_BYPASS_CH7                 (1 << 7)
#define ADC_HBF2_BYPASS_CH0                 (1 << 8)
#define ADC_HBF2_BYPASS_CH1                 (1 << 9)
#define ADC_HBF2_BYPASS_CH2                 (1 << 10)
#define ADC_HBF2_BYPASS_CH3                 (1 << 11)
#define ADC_HBF2_BYPASS_CH4                 (1 << 12)
#define ADC_HBF2_BYPASS_CH5                 (1 << 13)
#define ADC_HBF2_BYPASS_CH6                 (1 << 14)
#define ADC_HBF2_BYPASS_CH7                 (1 << 15)

// REG_0x60
#define DOWN_SEL_CH0_SHIFT                  0
#define DOWN_SEL_CH0_MASK                   (0x7 << DOWN_SEL_CH0_SHIFT)
#define DOWN_SEL_CH0(n)                     BITFIELD_VAL(DOWN_SEL_CH0, n)
#define DOWN_SEL_CH1_SHIFT                  3
#define DOWN_SEL_CH1_MASK                   (0x7 << DOWN_SEL_CH1_SHIFT)
#define DOWN_SEL_CH1(n)                     BITFIELD_VAL(DOWN_SEL_CH1, n)
#define DOWN_SEL_CH2_SHIFT                  6
#define DOWN_SEL_CH2_MASK                   (0x7 << DOWN_SEL_CH2_SHIFT)
#define DOWN_SEL_CH2(n)                     BITFIELD_VAL(DOWN_SEL_CH2, n)
#define DOWN_SEL_CH3_SHIFT                  9
#define DOWN_SEL_CH3_MASK                   (0x7 << DOWN_SEL_CH3_SHIFT)
#define DOWN_SEL_CH3(n)                     BITFIELD_VAL(DOWN_SEL_CH3, n)
#define DOWN_SEL_CH4_SHIFT                  12
#define DOWN_SEL_CH4_MASK                   (0x7 << DOWN_SEL_CH4_SHIFT)
#define DOWN_SEL_CH4(n)                     BITFIELD_VAL(DOWN_SEL_CH4, n)
#define FILTER_CONFIG_DR                    (1 << 15)

// REG_0x61
#define DOWN_SEL_CH5_SHIFT                  0
#define DOWN_SEL_CH5_MASK                   (0x7 << DOWN_SEL_CH5_SHIFT)
#define DOWN_SEL_CH5(n)                     BITFIELD_VAL(DOWN_SEL_CH5, n)
#define DOWN_SEL_CH6_SHIFT                  3
#define DOWN_SEL_CH6_MASK                   (0x7 << DOWN_SEL_CH6_SHIFT)
#define DOWN_SEL_CH6(n)                     BITFIELD_VAL(DOWN_SEL_CH6, n)
#define DOWN_SEL_CH7_SHIFT                  6
#define DOWN_SEL_CH7_MASK                   (0x7 << DOWN_SEL_CH7_SHIFT)
#define DOWN_SEL_CH7(n)                     BITFIELD_VAL(DOWN_SEL_CH7, n)

// REG_0x62
#define FIFO_REG_WR_EN                      (1 << 0)

// REG_0x63
#define FIFO_RD_EN                          (1 << 0)

// REG_0x66
#define CDC_SDM_OP_CIN1_CH0_REG_SHIFT       0
#define CDC_SDM_OP_CIN1_CH0_REG_MASK        (0x7 << CDC_SDM_OP_CIN1_CH0_REG_SHIFT)
#define CDC_SDM_OP_CIN1_CH0_REG(n)          BITFIELD_VAL(CDC_SDM_OP_CIN1_CH0_REG, n)
#define CDC_SDM_OP_CIN1_CH1_REG_SHIFT       3
#define CDC_SDM_OP_CIN1_CH1_REG_MASK        (0x7 << CDC_SDM_OP_CIN1_CH1_REG_SHIFT)
#define CDC_SDM_OP_CIN1_CH1_REG(n)          BITFIELD_VAL(CDC_SDM_OP_CIN1_CH1_REG, n)
#define CDC_SDM_OP_CIN1_CH2_REG_SHIFT       6
#define CDC_SDM_OP_CIN1_CH2_REG_MASK        (0x7 << CDC_SDM_OP_CIN1_CH2_REG_SHIFT)
#define CDC_SDM_OP_CIN1_CH2_REG(n)          BITFIELD_VAL(CDC_SDM_OP_CIN1_CH2_REG, n)
#define CDC_SDM_OP_CIN1_CH3_REG_SHIFT       9
#define CDC_SDM_OP_CIN1_CH3_REG_MASK        (0x7 << CDC_SDM_OP_CIN1_CH3_REG_SHIFT)
#define CDC_SDM_OP_CIN1_CH3_REG(n)          BITFIELD_VAL(CDC_SDM_OP_CIN1_CH3_REG, n)

// REG_0x67
#define CDC_SDM_OP_CIN1_CH4_REG_SHIFT       0
#define CDC_SDM_OP_CIN1_CH4_REG_MASK        (0x7 << CDC_SDM_OP_CIN1_CH4_REG_SHIFT)
#define CDC_SDM_OP_CIN1_CH4_REG(n)          BITFIELD_VAL(CDC_SDM_OP_CIN1_CH4_REG, n)
#define CDC_SDM_OP_CIN1_CH5_REG_SHIFT       3
#define CDC_SDM_OP_CIN1_CH5_REG_MASK        (0x7 << CDC_SDM_OP_CIN1_CH5_REG_SHIFT)
#define CDC_SDM_OP_CIN1_CH5_REG(n)          BITFIELD_VAL(CDC_SDM_OP_CIN1_CH5_REG, n)
#define CDC_SDM_OP_CIN1_CH6_REG_SHIFT       6
#define CDC_SDM_OP_CIN1_CH6_REG_MASK        (0x7 << CDC_SDM_OP_CIN1_CH6_REG_SHIFT)
#define CDC_SDM_OP_CIN1_CH6_REG(n)          BITFIELD_VAL(CDC_SDM_OP_CIN1_CH6_REG, n)
#define CDC_SDM_OP_CIN1_CH7_REG_SHIFT       9
#define CDC_SDM_OP_CIN1_CH7_REG_MASK        (0x7 << CDC_SDM_OP_CIN1_CH7_REG_SHIFT)
#define CDC_SDM_OP_CIN1_CH7_REG(n)          BITFIELD_VAL(CDC_SDM_OP_CIN1_CH7_REG, n)

// REG_0x69
#define FILTER_OUT_CMP_REG_HI_SHIFT         0
#define FILTER_OUT_CMP_REG_HI_MASK          (0xFFFF << FILTER_OUT_CMP_REG_HI_SHIFT)
#define FILTER_OUT_CMP_REG_HI(n)            BITFIELD_VAL(FILTER_OUT_CMP_REG_HI, n)

// REG_0x6A
#define FILTER_OUT_CMP_REG_LOW_SHIFT        0
#define FILTER_OUT_CMP_REG_LOW_MASK         (0xFFFF << FILTER_OUT_CMP_REG_LOW_SHIFT)
#define FILTER_OUT_CMP_REG_LOW(n)           BITFIELD_VAL(FILTER_OUT_CMP_REG_LOW, n)

// REG_0x6B
#define MODE1_SAR_INCREASE_VALUE_SHIFT      0
#define MODE1_SAR_INCREASE_VALUE_MASK       (0xF << MODE1_SAR_INCREASE_VALUE_SHIFT)
#define MODE1_SAR_INCREASE_VALUE(n)         BITFIELD_VAL(MODE1_SAR_INCREASE_VALUE, n)
#define MODE1_SAR_DECREASE_VALUE_SHIFT      4
#define MODE1_SAR_DECREASE_VALUE_MASK       (0xF << MODE1_SAR_DECREASE_VALUE_SHIFT)
#define MODE1_SAR_DECREASE_VALUE(n)         BITFIELD_VAL(MODE1_SAR_DECREASE_VALUE, n)

// REG_0x6C
#define MODE1_FILTER_OUT_TH_HI_SHIFT        0
#define MODE1_FILTER_OUT_TH_HI_MASK         (0xFFFF << MODE1_FILTER_OUT_TH_HI_SHIFT)
#define MODE1_FILTER_OUT_TH_HI(n)           BITFIELD_VAL(MODE1_FILTER_OUT_TH_HI, n)

// REG_0x6D
#define MODE1_FILTER_OUT_TH_LOW_SHIFT       0
#define MODE1_FILTER_OUT_TH_LOW_MASK        (0xFFFF << MODE1_FILTER_OUT_TH_LOW_SHIFT)
#define MODE1_FILTER_OUT_TH_LOW(n)          BITFIELD_VAL(MODE1_FILTER_OUT_TH_LOW, n)

// REG_0x6E
#define MODE1_OVERFLOW_SAR_TH_HI_SHIFT      0
#define MODE1_OVERFLOW_SAR_TH_HI_MASK       (0xFF << MODE1_OVERFLOW_SAR_TH_HI_SHIFT)
#define MODE1_OVERFLOW_SAR_TH_HI(n)         BITFIELD_VAL(MODE1_OVERFLOW_SAR_TH_HI, n)
#define MODE1_OVERFLOW_SAR_TH_LOW_SHIFT     8
#define MODE1_OVERFLOW_SAR_TH_LOW_MASK      (0xFF << MODE1_OVERFLOW_SAR_TH_LOW_SHIFT)
#define MODE1_OVERFLOW_SAR_TH_LOW(n)        BITFIELD_VAL(MODE1_OVERFLOW_SAR_TH_LOW, n)

// REG_0x7F
#define CH_SAMPLE_START_SHIFT               0
#define CH_SAMPLE_START_MASK                (0xFF << CH_SAMPLE_START_SHIFT)
#define CH_SAMPLE_START(n)                  BITFIELD_VAL(CH_SAMPLE_START, n)
#define CH_SAMPLE_CNT_SEL_SHIFT             8
#define CH_SAMPLE_CNT_SEL_MASK              (0x7 << CH_SAMPLE_CNT_SEL_SHIFT)
#define CH_SAMPLE_CNT_SEL(n)                BITFIELD_VAL(CH_SAMPLE_CNT_SEL, n)
#define CH_SAMPLE_MODE                      (1 << 11)

// REG_0x80
#define FIFO_TAG_EN                         (1 << 0)
#define CDC_CAL_BASELINE_DONE_ALL_DR        (1 << 1)
#define CDC_CAL_BASELINE_DONE_ALL_REG       (1 << 2)
#define CDC_SAR_DONE_ALL_DR                 (1 << 3)
#define CDC_SAR_DONE_ALL_REG                (1 << 4)
#define ADDR_DR                             (1 << 5)
#define ADDR_REG_SHIFT                      6
#define ADDR_REG_MASK                       (0x7F << ADDR_REG_SHIFT)
#define ADDR_REG(n)                         BITFIELD_VAL(ADDR_REG, n)
#define FIFO_DATA_SDM_ONLY                  (1 << 13)
#define FILTER_SYNC_DATA_EN                 (1 << 14)

// REG_0x81
#define CH_COUNT_SHIFT                      0
#define CH_COUNT_MASK                       (0x7 << CH_COUNT_SHIFT)
#define CH_COUNT(n)                         BITFIELD_VAL(CH_COUNT, n)
#define ALL_FIFO_DATA_VLD_FLAG              (1 << 3)
#define ADDR_SHIFT                          4
#define ADDR_MASK                           (0x7F << ADDR_SHIFT)
#define ADDR(n)                             BITFIELD_VAL(ADDR, n)

// REG_0x82
#define WR_ADDR_PTR_SHIFT                   0
#define WR_ADDR_PTR_MASK                    (0xFF << WR_ADDR_PTR_SHIFT)
#define WR_ADDR_PTR(n)                      BITFIELD_VAL(WR_ADDR_PTR, n)
#define RD_ADDR_PTR_SHIFT                   8
#define RD_ADDR_PTR_MASK                    (0xFF << RD_ADDR_PTR_SHIFT)
#define RD_ADDR_PTR(n)                      BITFIELD_VAL(RD_ADDR_PTR, n)

// REG_0x84
#define CAP_SENSOR_CLK_POL_SEL              (1 << 0)
#define FSM_CLK_DR                          (1 << 2)
#define SEL_FSM_CLK_REG                     (1 << 3)
#define PU_OSC_CAP_SENSOR_DR                (1 << 4)
#define PU_OSC_CAP_SENSOR_REG               (1 << 5)
#define FILTER_OUT_OVERFLOW_DR              (1 << 6)
#define FILTER_OUT_OVERFLOW_REG             (1 << 7)

// REG_0x85
#define CLK_SETTLE_TIME_SHIFT               0
#define CLK_SETTLE_TIME_MASK                (0x3FFF << CLK_SETTLE_TIME_SHIFT)
#define CLK_SETTLE_TIME(n)                  BITFIELD_VAL(CLK_SETTLE_TIME, n)

// REG_0x87
#define FP_MODE_RD_INT_RAW_EN_REG           (1 << 4)
#define PRESS_INT_EN_REG                    (1 << 5)

// REG_0x88
#define PRESS_INT                           (1 << 8)
#define PRESS_INT_RAW                       (1 << 9)
#define PRESS_INT_MASK                      (1 << 10)
#define PRESS_INT_CLR_REG                   (1 << 11)
#define FP_MODE_RD_INT                      (1 << 12)
#define FP_MODE_RD_INT_RAW                  (1 << 13)
#define FP_MODE_RD_INT_MASK                 (1 << 14)
#define FP_MODE_RD_INT_CLR_REG              (1 << 15)

// REG_0x89
#define WR_ADDR_PTR_REG_SHIFT               0
#define WR_ADDR_PTR_REG_MASK                (0xFF << WR_ADDR_PTR_REG_SHIFT)
#define WR_ADDR_PTR_REG(n)                  BITFIELD_VAL(WR_ADDR_PTR_REG, n)
#define WR_PTR_DR                           (1 << 8)

// REG_0x8A
#define RD_ADDR_PTR_REG_SHIFT               0
#define RD_ADDR_PTR_REG_MASK                (0xFF << RD_ADDR_PTR_REG_SHIFT)
#define RD_ADDR_PTR_REG(n)                  BITFIELD_VAL(RD_ADDR_PTR_REG, n)
#define RD_PTR_DR                           (1 << 8)

// REG_0x8B
#define CDC_CK_EDGE_SEL                     (1 << 0)
#define CDC_PD_IDLE_CHAN_SHIFT              1
#define CDC_PD_IDLE_CHAN_MASK               (0xFF << CDC_PD_IDLE_CHAN_SHIFT)
#define CDC_PD_IDLE_CHAN(n)                 BITFIELD_VAL(CDC_PD_IDLE_CHAN, n)

// REG_0x8C
#define SAR_OVERFLOW_DR                     (1 << 1)
#define SAR_OVERFLOW_REG                    (1 << 2)
#define FIFO_SAR_DATA_SEL                   (1 << 3)
#define CH_SCAN_CNT_MATCHED_REG_SHIFT       4
#define CH_SCAN_CNT_MATCHED_REG_MASK        (0xF << CH_SCAN_CNT_MATCHED_REG_SHIFT)
#define CH_SCAN_CNT_MATCHED_REG(n)          BITFIELD_VAL(CH_SCAN_CNT_MATCHED_REG, n)
#define FILTER_OUT_SYNC_EN_DR               (1 << 8)
#define FILTER_OUT_SYNC_EN_REG              (1 << 9)
#define CDC_LDO_VSEL_SHIFT                  10
#define CDC_LDO_VSEL_MASK                   (0xF << CDC_LDO_VSEL_SHIFT)
#define CDC_LDO_VSEL(n)                     BITFIELD_VAL(CDC_LDO_VSEL, n)
#define CDC_CK_DELAY_SHIFT                  14
#define CDC_CK_DELAY_MASK                   (0x3 << CDC_CK_DELAY_SHIFT)
#define CDC_CK_DELAY(n)                     BITFIELD_VAL(CDC_CK_DELAY, n)

// REG_0x8D
#define EMA_SHIFT                           0
#define EMA_MASK                            (0x7 << EMA_SHIFT)
#define EMA(n)                              BITFIELD_VAL(EMA, n)
#define EMAW_SHIFT                          3
#define EMAW_MASK                           (0x3 << EMAW_SHIFT)
#define EMAW(n)                             BITFIELD_VAL(EMAW, n)
#define EMAS                                (1 << 5)
#define WABL                                (1 << 6)
#define WABLM_SHIFT                         7
#define WABLM_MASK                          (0x3 << WABLM_SHIFT)
#define WABLM(n)                            BITFIELD_VAL(WABLM, n)
#define RAWL                                (1 << 9)
#define RAWLM_SHIFT                         10
#define RAWLM_MASK                          (0x3 << RAWLM_SHIFT)
#define RAWLM(n)                            BITFIELD_VAL(RAWLM, n)

// REG_0x8F
#define PGEN_MODE0                          (1 << 0)
#define RET1N_MODE0                         (1 << 1)
#define RET2N_MODE0                         (1 << 2)
#define PGEN_MODE1                          (1 << 3)
#define RET1N_MODE1                         (1 << 4)
#define RET2N_MODE1                         (1 << 5)
#define MEM_PG_PIN_CTRL_DR                  (1 << 6)

// REG_0x90
#define RES_REG_FOR_CAP_DIG_SHIFT           0
#define RES_REG_FOR_CAP_DIG_MASK            (0xFFFF << RES_REG_FOR_CAP_DIG_SHIFT)
#define RES_REG_FOR_CAP_DIG(n)              BITFIELD_VAL(RES_REG_FOR_CAP_DIG, n)

// REG_0x91
#define RESETN_FILTER_REG_DR                (1 << 0)
#define RESETN_FIFO_REG                     (1 << 1)
#define RESETN_FSM_CTRL_REG                 (1 << 2)
#define RESETN_CH_CTRL_REG                  (1 << 3)
#define RESETN_FILTER_OUT_REG               (1 << 4)

// REG_0x92
#define C_OFFSET                            (1 << 0)
#define CDC_MASK_SDM_MODE                   (1 << 1)
#define PRESS_DET_MODE_SHIFT                13
#define PRESS_DET_MODE_MASK                 (0x3 << PRESS_DET_MODE_SHIFT)
#define PRESS_DET_MODE(n)                   BITFIELD_VAL(PRESS_DET_MODE, n)

// REG_0x93
#define THRESHOLD_VALUE_CH0_SHIFT           0
#define THRESHOLD_VALUE_CH0_MASK            (0xFFFF << THRESHOLD_VALUE_CH0_SHIFT)
#define THRESHOLD_VALUE_CH0(n)              BITFIELD_VAL(THRESHOLD_VALUE_CH0, n)

// REG_0x94
#define THRESHOLD_VALUE_CH1_SHIFT           0
#define THRESHOLD_VALUE_CH1_MASK            (0xFFFF << THRESHOLD_VALUE_CH1_SHIFT)
#define THRESHOLD_VALUE_CH1(n)              BITFIELD_VAL(THRESHOLD_VALUE_CH1, n)

// REG_0x95
#define THRESHOLD_VALUE_CH2_SHIFT           0
#define THRESHOLD_VALUE_CH2_MASK            (0xFFFF << THRESHOLD_VALUE_CH2_SHIFT)
#define THRESHOLD_VALUE_CH2(n)              BITFIELD_VAL(THRESHOLD_VALUE_CH2, n)

// REG_0x96
#define THRESHOLD_VALUE_CH3_SHIFT           0
#define THRESHOLD_VALUE_CH3_MASK            (0xFFFF << THRESHOLD_VALUE_CH3_SHIFT)
#define THRESHOLD_VALUE_CH3(n)              BITFIELD_VAL(THRESHOLD_VALUE_CH3, n)

// REG_0x97
#define THRESHOLD_VALUE_CH4_SHIFT           0
#define THRESHOLD_VALUE_CH4_MASK            (0xFFFF << THRESHOLD_VALUE_CH4_SHIFT)
#define THRESHOLD_VALUE_CH4(n)              BITFIELD_VAL(THRESHOLD_VALUE_CH4, n)

// REG_0x98
#define THRESHOLD_VALUE_CH5_SHIFT           0
#define THRESHOLD_VALUE_CH5_MASK            (0xFFFF << THRESHOLD_VALUE_CH5_SHIFT)
#define THRESHOLD_VALUE_CH5(n)              BITFIELD_VAL(THRESHOLD_VALUE_CH5, n)

// REG_0x99
#define THRESHOLD_VALUE_CH6_SHIFT           0
#define THRESHOLD_VALUE_CH6_MASK            (0xFFFF << THRESHOLD_VALUE_CH6_SHIFT)
#define THRESHOLD_VALUE_CH6(n)              BITFIELD_VAL(THRESHOLD_VALUE_CH6, n)

// REG_0x9A
#define THRESHOLD_VALUE_CH7_SHIFT           0
#define THRESHOLD_VALUE_CH7_MASK            (0xFFFF << THRESHOLD_VALUE_CH7_SHIFT)
#define THRESHOLD_VALUE_CH7(n)              BITFIELD_VAL(THRESHOLD_VALUE_CH7, n)

// REG_0x9B
#define FILTER_OUT_DEBUG_CH0_SHIFT          0
#define FILTER_OUT_DEBUG_CH0_MASK           (0xFFFF << FILTER_OUT_DEBUG_CH0_SHIFT)
#define FILTER_OUT_DEBUG_CH0(n)             BITFIELD_VAL(FILTER_OUT_DEBUG_CH0, n)

// REG_0x9C
#define FILTER_OUT_DEBUG_CH1_SHIFT          0
#define FILTER_OUT_DEBUG_CH1_MASK           (0xFFFF << FILTER_OUT_DEBUG_CH1_SHIFT)
#define FILTER_OUT_DEBUG_CH1(n)             BITFIELD_VAL(FILTER_OUT_DEBUG_CH1, n)

// REG_0x9D
#define FILTER_OUT_DEBUG_CH2_SHIFT          0
#define FILTER_OUT_DEBUG_CH2_MASK           (0xFFFF << FILTER_OUT_DEBUG_CH2_SHIFT)
#define FILTER_OUT_DEBUG_CH2(n)             BITFIELD_VAL(FILTER_OUT_DEBUG_CH2, n)

// REG_0x9F
#define FILTER_OUT_DEBUG_CH3_SHIFT          0
#define FILTER_OUT_DEBUG_CH3_MASK           (0xFFFF << FILTER_OUT_DEBUG_CH3_SHIFT)
#define FILTER_OUT_DEBUG_CH3(n)             BITFIELD_VAL(FILTER_OUT_DEBUG_CH3, n)

// REG_0xA0
#define FILTER_OUT_DEBUG_CH4_SHIFT          0
#define FILTER_OUT_DEBUG_CH4_MASK           (0xFFFF << FILTER_OUT_DEBUG_CH4_SHIFT)
#define FILTER_OUT_DEBUG_CH4(n)             BITFIELD_VAL(FILTER_OUT_DEBUG_CH4, n)

// REG_0xA1
#define FILTER_OUT_DEBUG_CH5_SHIFT          0
#define FILTER_OUT_DEBUG_CH5_MASK           (0xFFFF << FILTER_OUT_DEBUG_CH5_SHIFT)
#define FILTER_OUT_DEBUG_CH5(n)             BITFIELD_VAL(FILTER_OUT_DEBUG_CH5, n)

// REG_0xA2
#define FILTER_OUT_DEBUG_CH6_SHIFT          0
#define FILTER_OUT_DEBUG_CH6_MASK           (0xFFFF << FILTER_OUT_DEBUG_CH6_SHIFT)
#define FILTER_OUT_DEBUG_CH6(n)             BITFIELD_VAL(FILTER_OUT_DEBUG_CH6, n)

// REG_0xA3
#define FILTER_OUT_DEBUG_CH7_SHIFT          0
#define FILTER_OUT_DEBUG_CH7_MASK           (0xFFFF << FILTER_OUT_DEBUG_CH7_SHIFT)
#define FILTER_OUT_DEBUG_CH7(n)             BITFIELD_VAL(FILTER_OUT_DEBUG_CH7, n)

#ifdef __cplusplus
}
#endif

#endif // __REG_CAPSENSOR_BEST1307PH_H__
