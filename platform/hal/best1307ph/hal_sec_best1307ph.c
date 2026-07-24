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
#include "plat_addr_map.h"

#if defined(SEC_CTRL_BASE)

#include "hal_cmu.h"
#include "hal_sec.h"
#include CHIP_SPECIFIC_HDR(reg_sec)
#include CHIP_SPECIFIC_HDR(reg_aonsec)
#include "hal_trace.h"

//these sec regs can only be accessed in security state
static struct HAL_SEC_T *const sec = (struct HAL_SEC_T *)SEC_CTRL_BASE;

static struct HAL_SEC_T *const aon_sec = (struct HAL_SEC_T *)AON_SEC_CTRL_BASE;

static void hal_sec_set_m_gdma(enum HAL_SEC_TYPE_T sec_type)
{
    if (sec_type == HAL_SEC_TYPE_NS)
        sec->REG_00C |= (SEC_CFG_NONSEC_GDMA);
    else
        sec->REG_00C &= ~(SEC_CFG_NONSEC_GDMA);
}

static void hal_sec_set_m_adma(enum HAL_SEC_TYPE_T sec_type)
{
    if (sec_type == HAL_SEC_TYPE_NS)
        sec->REG_00C |= (SEC_CFG_NONSEC_ADMA);
    else
        sec->REG_00C &= ~(SEC_CFG_NONSEC_ADMA);
}

static void hal_sec_set_m_bt2mcu(enum HAL_SEC_TYPE_T sec_type)
{
    if (sec_type == HAL_SEC_TYPE_NS)
        sec->REG_00C |= SEC_CFG_NONSEC_BT2MCU;
    else
        sec->REG_00C &= ~SEC_CFG_NONSEC_BT2MCU;
}

static void hal_sec_set_m_bt_tp_dump(enum HAL_SEC_TYPE_T sec_type)
{
    if (sec_type == HAL_SEC_TYPE_NS)
        sec->REG_00C |= SEC_CFG_NONSEC_BT_TP_DUMP;
    else
        sec->REG_00C &= ~SEC_CFG_NONSEC_BT_TP_DUMP;
}

void hal_sec_cfg_nonsec_bypass(bool bypass)
{
    if (bypass)
        sec->REG_004 |= SEC_CFG_NONSEC_BYPASS_MPC_SPINOR0;
    else
        sec->REG_004 &= ~SEC_CFG_NONSEC_BYPASS_MPC_SPINOR0;
}

static void hal_sec_set_m_i2c_slv(enum HAL_SEC_TYPE_T sec_type)
{
    if (sec_type == HAL_SEC_TYPE_NS)
        aon_sec->REG_000 |= AON_SEC_CFG_NONSEC_I2C_SLV;
    else
        aon_sec->REG_000 &= ~AON_SEC_CFG_NONSEC_I2C_SLV;
}

void hal_sec_idau_enable(bool enable)
{
    if (enable) {
        sec->REG_01C |= SEC_IDAUEN;
        aon_sec->REG_00C|= AON_SEC_MCU_CORE0_IDAUEN;
    } else {
        sec->REG_01C &= ~SEC_IDAUEN;
        aon_sec->REG_00C &= ~AON_SEC_MCU_CORE0_IDAUEN;
    }
}

void hal_sec_init(void)
{
    hal_cmu_clock_enable(HAL_CMU_MOD_P_TZC);
    hal_cmu_clock_enable(HAL_CMU_AON_A_TZC);
    hal_cmu_reset_clear(HAL_CMU_MOD_P_TZC);
    hal_cmu_reset_clear(HAL_CMU_AON_A_TZC);

    //hal_sec_idau_enable(true);

    hal_sec_set_m_gdma(HAL_SEC_TYPE_NS);
    hal_sec_set_m_adma(HAL_SEC_TYPE_NS);
    hal_sec_set_m_bt2mcu(HAL_SEC_TYPE_NS);
    hal_sec_set_m_bt_tp_dump(HAL_SEC_TYPE_NS);
    hal_sec_set_m_i2c_slv(HAL_SEC_TYPE_NS);
}

#endif
