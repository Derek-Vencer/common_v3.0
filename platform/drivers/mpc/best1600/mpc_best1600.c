/***************************************************************************
 *
 * Copyright 2015-2021 BES.
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
#include "cmsis.h"
#include "plat_addr_map.h"
#include "hal_cmu.h"
#include "hal_sec.h"
#include "hal_sleep.h"
#include "hal_timer.h"
#include "hal_trace.h"

void mpc_sys_sram0_setup(void)
{
    uint32_t val;
    uint32_t addr;
    uint32_t block_size, block_num;

    hal_cmu_axi_clock_set_mode(HAL_CMU_AXI_CLK_SRAM0, true);

    *(volatile uint32_t *)(MPC_SRAM0_BASE + 0x0) &= ~( 1<<8 );
    val = *(volatile uint32_t *)(MPC_SRAM0_BASE + 0x14);
    block_size = val & 0xF;
    block_size = 1 << (5 + block_size); //0x20000
    val = *(volatile uint32_t *)(MPC_SRAM0_BASE + 0x10);
    block_num = 32 * (val + 1);
    //DRIVERS_TRACE(0, "%s, block_size:0x%x, block_num:0x%x", __func__, block_size, block_num);
    for (addr = SYS_RAM0_NS_BASE - SYS_RAM0_BASE;
        addr < SYS_RAM0_NS_BASE + SYS_RAM0_NS_SIZE - SYS_RAM0_BASE; addr += block_size) {
        //DRIVERS_TRACE(0, "addr:0x%x", addr);
        uint32_t bit_offset;
        uint32_t reg_idx;
        bit_offset = addr / block_size;
        reg_idx = bit_offset / 32;
        ASSERT(reg_idx <= block_num, "Error, addr:0x%x, block_size:0x%x, block_num:0x%x",
            addr, block_size, block_num);
        *(volatile uint32_t *)(MPC_SRAM0_BASE + 0x18) = reg_idx;
        bit_offset = bit_offset % 32;
        *(volatile uint32_t *)(MPC_SRAM0_BASE + 0x1c) |= (1UL << bit_offset);
    }
    *(volatile uint32_t *)(MPC_SRAM0_BASE + 0x0) |= (1 << 31); //lock down
    *(volatile uint32_t *)(SPY_SRAM0_BASE + 0x0) |= (1 << 2);

    hal_cmu_axi_clock_set_mode(HAL_CMU_AXI_CLK_SRAM0, false);
}

void mpc_sys_sram1_setup(void)
{
    uint32_t val;
    uint32_t addr;
    uint32_t block_size, block_num;

    hal_cmu_axi_clock_set_mode(HAL_CMU_AXI_CLK_SRAM1, true);

    *(volatile uint32_t *)(MPC_SRAM1_BASE + 0x0) &= ~( 1<<8 );
    val = *(volatile uint32_t *)(MPC_SRAM1_BASE + 0x14);
    block_size = val & 0xF;
    block_size = 1 << (5 + block_size); //0x20000
    val = *(volatile uint32_t *)(MPC_SRAM1_BASE + 0x10);
    block_num = 32 * (val + 1);
    //DRIVERS_TRACE(0, "%s, block_size:0x%x, block_num:0x%x", __func__, block_size, block_num);
    for (addr = SYS_RAM1_NS_BASE - SYS_RAM1_BASE;
        addr < SYS_RAM1_NS_BASE + SYS_RAM1_NS_SIZE - SYS_RAM1_BASE; addr += block_size) {
        //DRIVERS_TRACE(0, "addr:0x%x", addr);
        uint32_t bit_offset;
        uint32_t reg_idx;
        bit_offset = addr / block_size;
        reg_idx = bit_offset / 32;
        ASSERT(reg_idx <= block_num, "Error, addr:0x%x, block_size:0x%x, block_num:0x%x",
            addr, block_size, block_num);
        *(volatile uint32_t *)(MPC_SRAM1_BASE + 0x18) = reg_idx;
        bit_offset = bit_offset % 32;
        *(volatile uint32_t *)(MPC_SRAM1_BASE + 0x1c) |= (1UL << bit_offset);
    }
    *(volatile uint32_t *)(MPC_SRAM1_BASE + 0x0) |= (1 << 31); //lock down
    *(volatile uint32_t *)(SPY_SRAM1_BASE + 0x0) |= (1 << 2);

    hal_cmu_axi_clock_set_mode(HAL_CMU_AXI_CLK_SRAM1, false);
}

void mpc_sys_sram2_setup(void)
{
    uint32_t val;
    uint32_t addr;
    uint32_t block_size, block_num;

    hal_cmu_axi_clock_set_mode(HAL_CMU_AXI_CLK_SRAM2, true);

    *(volatile uint32_t *)(MPC_SRAM2_BASE + 0x0) &= ~( 1<<8 );
    val = *(volatile uint32_t *)(MPC_SRAM2_BASE + 0x14);
    block_size = val & 0xF;
    block_size = 1 << (5 + block_size); //0x20000
    val = *(volatile uint32_t *)(MPC_SRAM2_BASE + 0x10);
    block_num = 32 * (val + 1);
    //DRIVERS_TRACE(0, "%s, block_size:0x%x, block_num:0x%x", __func__, block_size, block_num);
    for (addr = SYS_RAM2_NS_BASE - SYS_RAM2_BASE;
        addr < SYS_RAM2_NS_BASE + SYS_RAM2_NS_SIZE - SYS_RAM2_BASE; addr += block_size) {
        //DRIVERS_TRACE(0, "addr:0x%x", addr);
        uint32_t bit_offset;
        uint32_t reg_idx;
        bit_offset = addr / block_size;
        reg_idx = bit_offset / 32;
        ASSERT(reg_idx <= block_num, "Error, addr:0x%x, block_size:0x%x, block_num:0x%x",
            addr, block_size, block_num);
        *(volatile uint32_t *)(MPC_SRAM2_BASE + 0x18) = reg_idx;
        bit_offset = bit_offset % 32;
        *(volatile uint32_t *)(MPC_SRAM2_BASE + 0x1c) |= (1UL << bit_offset);
    }
    *(volatile uint32_t *)(MPC_SRAM2_BASE + 0x0) |= (1 << 31); //lock down
    *(volatile uint32_t *)(SPY_SRAM2_BASE + 0x0) |= (1 << 2);

    hal_cmu_axi_clock_set_mode(HAL_CMU_AXI_CLK_SRAM2, false);
}

void mpc_sys_sram3_setup(void)
{
    uint32_t val;
    uint32_t addr;
    uint32_t block_size, block_num;

    hal_cmu_axi_clock_set_mode(HAL_CMU_AXI_CLK_SRAM3, true);

    *(volatile uint32_t *)(MPC_SRAM3_BASE + 0x0) &= ~( 1<<8 );
    val = *(volatile uint32_t *)(MPC_SRAM3_BASE + 0x14);
    block_size = val & 0xF;
    block_size = 1 << (5 + block_size); //0x20000
    val = *(volatile uint32_t *)(MPC_SRAM3_BASE + 0x10);
    block_num = 32 * (val + 1);
    //DRIVERS_TRACE(0, "%s, block_size:0x%x, block_num:0x%x", __func__, block_size, block_num);
    for (addr = SYS_RAM3_NS_BASE - SYS_RAM3_BASE;
        addr < SYS_RAM3_NS_BASE + SYS_RAM3_NS_SIZE - SYS_RAM3_BASE; addr += block_size) {
        //DRIVERS_TRACE(0, "addr:0x%x", addr);
        uint32_t bit_offset;
        uint32_t reg_idx;
        bit_offset = addr / block_size;
        reg_idx = bit_offset / 32;
        ASSERT(reg_idx <= block_num, "Error, addr:0x%x, block_size:0x%x, block_num:0x%x",
            addr, block_size, block_num);
        *(volatile uint32_t *)(MPC_SRAM3_BASE + 0x18) = reg_idx;
        bit_offset = bit_offset % 32;
        *(volatile uint32_t *)(MPC_SRAM3_BASE + 0x1c) |= (1UL << bit_offset);
    }
    *(volatile uint32_t *)(MPC_SRAM3_BASE + 0x0) |= (1 << 31); //lock down
    *(volatile uint32_t *)(SPY_SRAM3_BASE + 0x0) |= (1 << 2);

    hal_cmu_axi_clock_set_mode(HAL_CMU_AXI_CLK_SRAM3, false);
}

void mpc_sys_psram_setup(void)
{
    uint32_t val;
    uint32_t addr;
    uint32_t block_size, block_num;

    hal_cmu_axi_clock_set_mode(HAL_CMU_AXI_CLK_PSRAM, true);

    *(volatile uint32_t *)(MPC_PSRAM_BASE + 0x0) &= ~( 1<<8 );
    val = *(volatile uint32_t *)(MPC_PSRAM_BASE + 0x14);
    block_size = val & 0xF;
    block_size = 1 << (5 + block_size); //0x40000
    val = *(volatile uint32_t *)(MPC_PSRAM_BASE + 0x10);
    block_num = 32 * (val + 1);
    //DRIVERS_TRACE(0, "%s, block_size:0x%x, block_num:0x%x", __func__, block_size, block_num);
    for (addr = SYS_PSRAM_S_SIZE; addr < PSRAM_SIZE; addr += block_size) {
        //DRIVERS_TRACE(0, "addr:0x%x", addr);
        uint32_t bit_offset;
        uint32_t reg_idx;
        bit_offset = addr / block_size;
        reg_idx = bit_offset / 32;
        ASSERT(reg_idx <= block_num, "Error, addr:0x%x, block_size:0x%x, block_num:0x%x",
            addr, block_size, block_num);
        *(volatile uint32_t *)(MPC_PSRAM_BASE + 0x18) = reg_idx;
        bit_offset = bit_offset % 32;
        *(volatile uint32_t *)(MPC_PSRAM_BASE + 0x1c) |= (1UL << bit_offset);
    }
    *(volatile uint32_t *)(MPC_PSRAM_BASE + 0x0) |= (1 << 31); //lock down
    *(volatile uint32_t *)(SPY_PSRAM_BASE + 0x0) |= (1 << 2);

    hal_cmu_axi_clock_set_mode(HAL_CMU_AXI_CLK_PSRAM, false);
}

void mpc_sys_flash_setup(void)
{
    uint32_t val;
    uint32_t addr;
    uint32_t block_size, block_num;

    hal_cmu_axi_clock_set_mode(HAL_CMU_AXI_CLK_MAIN, true);

    *(volatile uint32_t *)(MPC_FLASH_BASE + 0x0) &= ~( 1<<8 );
    val = *(volatile uint32_t *)(MPC_FLASH_BASE + 0x14);
    block_size = val & 0xF;
    block_size = 1 << (5 + block_size); //0x40000
    val = *(volatile uint32_t *)(MPC_FLASH_BASE + 0x10);
    block_num = 32 * (val + 1);
    //DRIVERS_TRACE(0, "%s, block_size:0x%x, block_num:0x%x", __func__, block_size, block_num);
    for (addr = FLASH_S_SIZE; addr < FLASH_SIZE; addr += block_size) {
        //DRIVERS_TRACE(0, "addr:0x%x", addr);
        uint32_t bit_offset;
        uint32_t reg_idx;
        bit_offset = addr / block_size;
        reg_idx = bit_offset / 32;
        ASSERT(reg_idx <= block_num, "Error, addr:0x%x, block_size:0x%x, block_num:0x%x",
            addr, block_size, block_num);
        *(volatile uint32_t *)(MPC_FLASH_BASE + 0x18) = reg_idx;
        bit_offset = bit_offset % 32;
        *(volatile uint32_t *)(MPC_FLASH_BASE + 0x1c) |= (1UL << bit_offset);
    }
    *(volatile uint32_t *)(MPC_FLASH_BASE + 0x0) |= (1 << 31); //lock down
    *(volatile uint32_t *)(SPY_FLASH_BASE + 0x0) |= (1 << 2);

    hal_cmu_axi_clock_set_mode(HAL_CMU_AXI_CLK_MAIN, false);
}

void tz_TGU_setup (void)
{
    uint32_t val;
    uint32_t addr;
    uint32_t block_size, block_num;

    val = *(volatile uint32_t *)(ITGU_BASE + 4);
    block_size = val & 0xF;
    block_size = 1 << (5 + block_size); //0x10000
    block_num = (val >> 8)&0xF;
    //ASSERT(block_num == 4, "error block num:%d", block_num);
    block_num = 1 << block_num;
    //DRIVERS_TRACE(0, "ITGU block_size:0x%x block_num:0x%x", block_size, block_num);
    //setup ns ITCM region
    for (addr = RAMX_NS_BASE - M55_ITCM_START;
        addr < RAMX_NS_BASE + RAMX_NS_SIZE - M55_ITCM_START; addr += block_size) {
        //DRIVERS_TRACE(0, "addr:0x%x", addr);
        uint32_t bit_offset;
        uint32_t reg_offset;
        bit_offset = addr / block_size;
        ASSERT(bit_offset <= block_num, "Error, addr:0x%x, block_size:0x%x, block_num:0x%x",
            addr, block_size, block_num);
        reg_offset = 4 * (bit_offset / 32) + 0x10;
        bit_offset = bit_offset % 32;
        *(volatile uint32_t *)(ITGU_BASE + reg_offset) |= (1UL << bit_offset);
    }
    *(volatile uint32_t *)ITGU_BASE = 0x3;

    val = *(volatile uint32_t *)(DTGU_BASE + 4);
    block_size = val & 0xF;
    block_size = 1 << (5 + block_size); //0x10000
    block_num = (val >> 8) & 0xF;
    block_num = 1 << block_num;
    //DRIVERS_TRACE(0, "DTGU block_size:0x%x block_num:0x%x", block_size, block_num);
    //setup ns DTCM region
    for (addr = RAM_NS_BASE - M55_DTCM_START;
        addr < RAM_NS_BASE + RAM_NS_SIZE - M55_DTCM_START; addr += block_size) {
        //DRIVERS_TRACE(0, "addr:0x%x", addr);
        uint32_t bit_offset;
        uint32_t reg_offset;
        bit_offset = addr / block_size;
        ASSERT(bit_offset <= block_num, "Error, addr:0x%x, block_size:0x%x, block_num:0x%x",
            addr, block_size, block_num);
        reg_offset = 4 * (bit_offset / 32) + 0x10;
        bit_offset = bit_offset % 32;
        *(volatile uint32_t *)(DTGU_BASE + reg_offset) |= (1UL << bit_offset);
    }
    *(volatile uint32_t *)DTGU_BASE = 0x3;
}

void mpc_setup_internal(void)
{
    tz_TGU_setup();

    mpc_sys_sram0_setup();
    mpc_sys_sram1_setup();
    mpc_sys_sram2_setup();
    mpc_sys_sram3_setup();

    mpc_sys_psram_setup();

    mpc_sys_flash_setup();
}

#ifdef CORE_SLEEP_POWER_DOWN
static int mpc_pm_notif_handler(enum HAL_PM_STATE_T state)
{
    if (state == HAL_PM_STATE_POWER_DOWN_WAKEUP) {
        mpc_setup_internal();
    }

    return 0;
}
#endif

int mpc_init(void)
{
#ifdef CORE_SLEEP_POWER_DOWN
    hal_pm_notif_register(HAL_PM_USER_HAL, mpc_pm_notif_handler);
#endif

    mpc_setup_internal();

    DRIVERS_TRACE(0, "MPC init successfully");
    return 0;
}
