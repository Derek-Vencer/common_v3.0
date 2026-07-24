/***************************************************************************
 *
 * Copyright 2015-2022 BES.
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
#include "hal_sec.h"
#include "hal_trace.h"

#include "mpc_sie200_drv.h"

/* Ranges controlled by this FLASH0_MPC */
// flash0_base, range_offset is 1<<25
static const struct mpc_sie200_memory_range_t MPC_FLASH_RANGE_S = {
    .base         = FLASH_BASE,
    .limit        = FLASH_BASE + FLASH_S_SIZE - 1,
    .range_offset = 0,
    .attr         = MPC_SIE200_SEC_ATTR_SECURE,
};

static const struct mpc_sie200_memory_range_t MPC_FLASH_RANGE_NS = {
    .base         = FLASH_BASE + FLASH_S_SIZE,
    .limit        = FLASH_BASE + FLASH_SIZE - 1,
    .range_offset = FLASH_S_SIZE,
    .attr         = MPC_SIE200_SEC_ATTR_NONSECURE,
};

#define FLASH_MPC_VALID_ADDR_BIT 26
#define FLASH_MPC_BLOCK_SIZE 0x10000
#define FLASH_MPC_ALIGNED_SIZE(addr) (addr&(~(FLASH_MPC_BLOCK_SIZE-1)))
#define FLASH_MPC_OFFSET(addr) FLASH_MPC_ALIGNED_SIZE(addr)&((1<<FLASH_MPC_VALID_ADDR_BIT)-1)
static const struct mpc_sie200_memory_range_t MPC_FLASH_RANGE_ICACHE = {
    .base         = FLASH_MPC_ALIGNED_SIZE(ICACHE_CTRL_BASE),
    .limit        = FLASH_MPC_ALIGNED_SIZE(ICACHE_CTRL_BASE) + FLASH_MPC_BLOCK_SIZE - 1,
    .range_offset = FLASH_MPC_OFFSET(ICACHE_CTRL_BASE),
    .attr         = MPC_SIE200_SEC_ATTR_NONSECURE,
};

#define MPC_FLASH_RANGE_LIST_LEN  3u
static const struct mpc_sie200_memory_range_t* MPC_FLASH_RANGE_LIST[MPC_FLASH_RANGE_LIST_LEN] = {
    &MPC_FLASH_RANGE_S,
    &MPC_FLASH_RANGE_NS,
    &MPC_FLASH_RANGE_ICACHE,
};

static const struct mpc_sie200_dev_cfg_t MPC_FLASH0_DEV_CFG = {
    .base = MPC_FLASH0_BASE,
};
static struct mpc_sie200_dev_data_t MPC_FLASH0_DEV_DATA = {
    .range_list = 0,
    .nbr_of_ranges = 0,
    .state = 0,
    .reserved = 0,
};

struct mpc_sie200_dev_t MPC_FLASH0_DEV = {
    &(MPC_FLASH0_DEV_CFG),
    &(MPC_FLASH0_DEV_DATA),
};

static int in_ns_ram(uint32_t start, uint32_t end)
{
    uint32_t se_start = RAM_BASE;
    uint32_t se_end = RAM_BASE + RAM_S_SIZE + RAM_NSC_SIZE;

    return (end <= se_start || se_end <= start);
}

int mpc_init(void)
{
    enum mpc_sie200_error_t ret;

    hal_sec_set_sram0_nosec(in_ns_ram(RAM0_BASE, RAM1_BASE));
    hal_sec_set_sram1_nosec(in_ns_ram(RAM1_BASE, RAM2_BASE));
    hal_sec_set_sram2_nosec(in_ns_ram(RAM2_BASE, RAM3_BASE));
    hal_sec_set_sram3_nosec(in_ns_ram(RAM3_BASE, RAM4_BASE));
    hal_sec_set_sram4_nosec(in_ns_ram(RAM4_BASE, RAM5_BASE));
    hal_sec_set_sram5_nosec(in_ns_ram(RAM5_BASE, RAM6_BASE));
    hal_sec_set_sram6_nosec(in_ns_ram(RAM6_BASE, RAM7_BASE));
    hal_sec_set_sram7_nosec(in_ns_ram(RAM7_BASE, RAM8_BASE));
    hal_sec_set_sram8_nosec(in_ns_ram(RAM8_BASE, RAM8_BASE + RAM8_SIZE));

    // init FLASH0
    ret = mpc_sie200_init(&MPC_FLASH0_DEV,
                          MPC_FLASH_RANGE_LIST,
                          MPC_FLASH_RANGE_LIST_LEN);
    if (ret != MPC_SIE200_ERR_NONE) {
        DRIVERS_TRACE(0, "%s:%d, sie200 init fail. ret:%d", __FUNCTION__, __LINE__, ret);
        return ret;
    }
    mpc_sie200_set_ctrl(&MPC_FLASH0_DEV, MPC_SIE200_CTRL_SEC_RESP);
    ret = mpc_sie200_config_region(&MPC_FLASH0_DEV,
        FLASH_BASE+FLASH_S_SIZE, FLASH_BASE+FLASH_SIZE-1, MPC_SIE200_SEC_ATTR_NONSECURE);
    if (ret != MPC_SIE200_ERR_NONE) {
        DRIVERS_TRACE(0, "%s:%d, sie200 config fail. ret:%d", __FUNCTION__, __LINE__, ret);
        return ret;
    }
    ret = mpc_sie200_config_region(&MPC_FLASH0_DEV,
        FLASH_MPC_ALIGNED_SIZE(ICACHE_CTRL_BASE), FLASH_MPC_ALIGNED_SIZE(ICACHE_CTRL_BASE) + FLASH_MPC_BLOCK_SIZE - 1,
        MPC_SIE200_SEC_ATTR_NONSECURE);
    if (ret != MPC_SIE200_ERR_NONE) {
        DRIVERS_TRACE(0, "%s:%d, sie200 config fail. ret:%d", __FUNCTION__, __LINE__, ret);
        return ret;
    }
    mpc_sie200_irq_disable(&MPC_FLASH0_DEV);
    mpc_sie200_lock_down(&MPC_FLASH0_DEV);
    hal_sec_cfg_nonsec_bypass(false);

    DRIVERS_TRACE(0, "MPC init successfully");

    return 0;
}
