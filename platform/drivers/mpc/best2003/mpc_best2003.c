/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#include "hal_chipid.h"

#include "mpc_sie200_drv.h"

/* Ranges controlled by this SRAM0_MPC */
static const struct mpc_sie200_memory_range_t MPC_SRAM0_RANGE_S = {
    .base         = RAM0_BASE,
    .limit        = RAM0_BASE + RAM_S_SIZE + RAM_NSC_SIZE - 1,
    .range_offset = 0,
    .attr         = MPC_SIE200_SEC_ATTR_SECURE,
};

static const struct mpc_sie200_memory_range_t MPC_SRAM0_RANGE_NS = {
    .base         = RAM0_BASE + RAM_S_SIZE,
    .limit        = RAM1_BASE - 1,
    .range_offset = RAM_S_SIZE,
    .attr         = MPC_SIE200_SEC_ATTR_NONSECURE,
};

#define MPC_SRAM0_RANGE_LIST_LEN  2u
static const struct mpc_sie200_memory_range_t* MPC_SRAM0_RANGE_LIST[MPC_SRAM0_RANGE_LIST_LEN] = {
    &MPC_SRAM0_RANGE_S,
    &MPC_SRAM0_RANGE_NS
};

static const struct mpc_sie200_dev_cfg_t MPC_SRAM0_DEV_CFG = {
    .base = MPC_SRAM0_BASE,
};
static struct mpc_sie200_dev_data_t MPC_SRAM0_DEV_DATA = {
    .range_list = 0,
    .nbr_of_ranges = 0,
    .state = 0,
    .reserved = 0,
};

struct mpc_sie200_dev_t MPC_SRAM0_DEV = {
    &(MPC_SRAM0_DEV_CFG),
    &(MPC_SRAM0_DEV_DATA),
};

/* Ranges controlled by this FLASH0_MPC */
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
#define FLASH_MPC_BLOCK_SIZE 0x40000
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

#if 0
#define MPC_FLASH1_RANGE_LIST_LEN  2u
static const struct mpc_sie200_memory_range_t* MPC_FLASH1_RANGE_LIST[MPC_FLASH_RANGE_LIST_LEN] = {
    &MPC_FLASH_RANGE_S,
    &MPC_FLASH_RANGE_NS,
};

static const struct mpc_sie200_dev_cfg_t MPC_FLASH1_DEV_CFG = {
    .base = MPC_FLASH1_BASE,
};
static struct mpc_sie200_dev_data_t MPC_FLASH1_DEV_DATA = {
    .range_list = 0,
    .nbr_of_ranges = 0,
    .state = 0,
    .reserved = 0,
};

struct mpc_sie200_dev_t MPC_FLASH1_DEV = {
    &(MPC_FLASH1_DEV_CFG),
    &(MPC_FLASH1_DEV_DATA),
};

/* Ranges controlled by this PSRAM0_MPC */
static const struct mpc_sie200_memory_range_t MPC_PSRAM_RANGE_PSRAM = {
    .base         = PSRAM_BASE,
    .limit        = PSRAM_BASE + PSRAM_SIZE - 1,
    .range_offset = 0,
    .attr         = MPC_SIE200_SEC_ATTR_NONSECURE,
};

static const struct mpc_sie200_memory_range_t MPC_PSRAM_RANGE_PSRAMUHS = {
    .base         = PSRAMUHS_BASE,
    .limit        = PSRAMUHS_BASE + PSRAMUHS_SIZE - 1,
    .range_offset = 0,
    .attr         = MPC_SIE200_SEC_ATTR_NONSECURE,
};
#define PSRAM_MPC_VALID_ADDR_BIT 28
#define PSRAM_MPC_BLOCK_SIZE 0x40000
#define PSRAM_MPC_ALIGNED_SIZE(addr) (addr&(~(PSRAM_MPC_BLOCK_SIZE-1)))
#define PSRAM_MPC_OFFSET(addr) PSRAM_MPC_ALIGNED_SIZE(addr)&((1<<PSRAM_MPC_VALID_ADDR_BIT)-1)
static const struct mpc_sie200_memory_range_t MPC_PSRAM_RANGE_DCACHE = {
    .base         = PSRAM_MPC_ALIGNED_SIZE(DCACHE_CTRL_BASE),
    .limit        = PSRAM_MPC_ALIGNED_SIZE(DCACHE_CTRL_BASE) + PSRAM_MPC_BLOCK_SIZE - 1,
    .range_offset = PSRAM_MPC_OFFSET(DCACHE_CTRL_BASE),
    .attr         = MPC_SIE200_SEC_ATTR_NONSECURE,
};

#define MPC_PSRAM_RANGE_LIST_LEN  3u
static const struct mpc_sie200_memory_range_t* MPC_PSRAM_RANGE_LIST[MPC_PSRAM_RANGE_LIST_LEN] = {
    &MPC_PSRAM_RANGE_PSRAM,
    &MPC_PSRAM_RANGE_PSRAMUHS,
    &MPC_PSRAM_RANGE_DCACHE,
};

static const struct mpc_sie200_dev_cfg_t MPC_PSRAM0_DEV_CFG = {
    .base = MPC_PSRAM0_BASE,
};
static struct mpc_sie200_dev_data_t MPC_PSRAM0_DEV_DATA = {
    .range_list = 0,
    .nbr_of_ranges = 0,
    .state = 0,
    .reserved = 0,
};

struct mpc_sie200_dev_t MPC_PSRAM0_DEV = {
    &(MPC_PSRAM0_DEV_CFG),
    &(MPC_PSRAM0_DEV_DATA),
};

#define MPC_PSRAM1_RANGE_LIST_LEN  2u
static const struct mpc_sie200_memory_range_t* MPC_PSRAM1_RANGE_LIST[MPC_PSRAM1_RANGE_LIST_LEN] = {
    &MPC_PSRAM_RANGE_PSRAM,
    &MPC_PSRAM_RANGE_PSRAMUHS,
};

static const struct mpc_sie200_dev_cfg_t MPC_PSRAM1_DEV_CFG = {
    .base = MPC_PSRAM1_BASE,
};
static struct mpc_sie200_dev_data_t MPC_PSRAM1_DEV_DATA = {
    .range_list = 0,
    .nbr_of_ranges = 0,
    .state = 0,
    .reserved = 0,
};

struct mpc_sie200_dev_t MPC_PSRAM1_DEV = {
    &(MPC_PSRAM1_DEV_CFG),
    &(MPC_PSRAM1_DEV_DATA),
};
#endif

#if 0
/* SRAM0_MPC Driver wrapper functions */
static int32_t SRAM0_MPC_Initialize(void)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_init(&MPC_SRAM0_DEV,
                          MPC_SRAM0_RANGE_LIST,
                          MPC_SRAM0_RANGE_LIST_LEN);

    return ret;
}

static int32_t SRAM0_MPC_Uninitialize(void)
{
    /* Nothing to be done */
    return 0;
}

static int32_t SRAM0_MPC_GetBlockSize(uint32_t *blk_size)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_get_block_size(&MPC_SRAM0_DEV, blk_size);

    return ret;
}

static int32_t SRAM0_MPC_GetCtrlConfig(uint32_t *ctrl_val)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_get_ctrl(&MPC_SRAM0_DEV, ctrl_val);

    return ret;
}

static int32_t SRAM0_MPC_SetCtrlConfig(uint32_t ctrl)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_set_ctrl(&MPC_SRAM0_DEV, ctrl);

    return ret;
}

static int32_t SRAM0_MPC_GetRegionConfig(uintptr_t base,
                                          uintptr_t limit,
                                          ARM_MPC_SEC_ATTR *attr)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_get_region_config(&MPC_SRAM0_DEV, base, limit,
                                       (enum mpc_sie200_sec_attr_t*)attr);

    return ret;
}

static int32_t SRAM0_MPC_ConfigRegion(uintptr_t base,
                                       uintptr_t limit,
                                       ARM_MPC_SEC_ATTR attr)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_config_region(&MPC_SRAM0_DEV, base, limit,
                                   (enum mpc_sie200_sec_attr_t)attr);

    return ret;
}

static int32_t SRAM0_MPC_EnableInterrupt(void)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_irq_enable(&MPC_SRAM0_DEV);

    return ret;
}

static void SRAM0_MPC_DisableInterrupt(void)
{
    mpc_sie200_irq_disable(&MPC_SRAM0_DEV);
}

static void SRAM0_MPC_ClearInterrupt(void)
{
    mpc_sie200_clear_irq(&MPC_SRAM0_DEV);
}

static uint32_t SRAM0_MPC_InterruptState(void)
{
    return mpc_sie200_irq_state(&MPC_SRAM0_DEV);
}

static int32_t SRAM0_MPC_LockDown(void)
{
    return mpc_sie200_lock_down(&MPC_SRAM0_DEV);
}
#endif

int mpc_init(void)
{
    enum mpc_sie200_error_t ret;

    // init SRAM0
    ret = mpc_sie200_init(&MPC_SRAM0_DEV,
                          MPC_SRAM0_RANGE_LIST,
                          MPC_SRAM0_RANGE_LIST_LEN);
    if (ret != MPC_SIE200_ERR_NONE) {
        DRIVERS_TRACE(0, "%s:%d, sie200 init fail. ret:%d", __FUNCTION__, __LINE__, ret);
        return ret;
    }
    mpc_sie200_set_ctrl(&MPC_SRAM0_DEV, 0x10);

    if((RAM0_BASE + RAM_S_SIZE) < RAM1_BASE){
        ret = mpc_sie200_config_region(&MPC_SRAM0_DEV,
            RAM0_BASE+RAM_S_SIZE+RAM_NSC_SIZE, RAM1_BASE-1, MPC_SIE200_SEC_ATTR_NONSECURE);
        if (ret != MPC_SIE200_ERR_NONE) {
            DRIVERS_TRACE(0, "%s:%d, sie200 config fail. ret:%d", __FUNCTION__, __LINE__, ret);
            return ret;
        }
    }
    mpc_sie200_irq_disable(&MPC_SRAM0_DEV);
    mpc_sie200_lock_down(&MPC_SRAM0_DEV);
    hal_sec_cfg_nonsec_bypass(SPY_SRAM0_BASE, false);

    // init FLASH0
    ret = mpc_sie200_init(&MPC_FLASH0_DEV,
                          MPC_FLASH_RANGE_LIST,
                          MPC_FLASH_RANGE_LIST_LEN);
    if (ret != MPC_SIE200_ERR_NONE) {
        DRIVERS_TRACE(0, "%s:%d, sie200 init fail. ret:%d", __FUNCTION__, __LINE__, ret);
        return ret;
    }
    mpc_sie200_set_ctrl(&MPC_FLASH0_DEV, 0x10);
    ret = mpc_sie200_config_region(&MPC_FLASH0_DEV,
        FLASH_BASE + FLASH_S_SIZE,
        FLASH_BASE+FLASH_SIZE-1, MPC_SIE200_SEC_ATTR_NONSECURE);
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
    hal_sec_cfg_nonsec_bypass(SPY_FLASH0_BASE, false);

#if 0
    // init FLASH1
    ret = mpc_sie200_init(&MPC_FLASH1_DEV,
                          MPC_FLASH1_RANGE_LIST,
                          MPC_FLASH1_RANGE_LIST_LEN);
    if (ret != MPC_SIE200_ERR_NONE) {
        DRIVERS_TRACE(0, "%s:%d, sie200 init fail. ret:%d", __FUNCTION__, __LINE__, ret);
        return ret;
    }
    mpc_sie200_set_ctrl(&MPC_FLASH1_DEV, 0x10);
    ret = mpc_sie200_config_region(&MPC_FLASH1_DEV,
        FLASH_BASE + FLASH_S_SIZE,
        FLASH_BASE+FLASH_SIZE-1, MPC_SIE200_SEC_ATTR_NONSECURE);
    if (ret != MPC_SIE200_ERR_NONE) {
        DRIVERS_TRACE(0, "%s:%d, sie200 config fail. ret:%d", __FUNCTION__, __LINE__, ret);
        return ret;
    }
    mpc_sie200_irq_disable(&MPC_FLASH1_DEV);
    mpc_sie200_lock_down(&MPC_FLASH1_DEV);
    hal_sec_cfg_nonsec_bypass(SPY_FLASH1_BASE, false);

    // init PSRAM0
    ret = mpc_sie200_init(&MPC_PSRAM0_DEV,
                          MPC_PSRAM_RANGE_LIST,
                          MPC_PSRAM_RANGE_LIST_LEN);
    if (ret != MPC_SIE200_ERR_NONE) {
        DRIVERS_TRACE(0, "%s:%d, sie200 init fail. ret:%d", __FUNCTION__, __LINE__, ret);
        return ret;
    }
    mpc_sie200_set_ctrl(&MPC_PSRAM0_DEV, 0x10);
    ret = mpc_sie200_config_region(&MPC_PSRAM0_DEV,
        PSRAM_BASE, PSRAM_BASE+PSRAM_SIZE-1, MPC_SIE200_SEC_ATTR_NONSECURE);
    if (ret != MPC_SIE200_ERR_NONE) {
        DRIVERS_TRACE(0, "%s:%d, sie200 config fail. ret:%d", __FUNCTION__, __LINE__, ret);
        return ret;
    }
    ret = mpc_sie200_config_region(&MPC_PSRAM0_DEV,
        PSRAM_MPC_ALIGNED_SIZE(DCACHE_CTRL_BASE), PSRAM_MPC_ALIGNED_SIZE(DCACHE_CTRL_BASE) + PSRAM_MPC_BLOCK_SIZE - 1,
        MPC_SIE200_SEC_ATTR_NONSECURE);
    if (ret != MPC_SIE200_ERR_NONE) {
        DRIVERS_TRACE(0, "%s:%d, sie200 config fail. ret:%d", __FUNCTION__, __LINE__, ret);
        return ret;
    }

    mpc_sie200_irq_disable(&MPC_PSRAM0_DEV);
    mpc_sie200_lock_down(&MPC_PSRAM0_DEV);
    hal_sec_cfg_nonsec_bypass(SPY_PSRAM0_BASE, false);

    // init PSRAM1
    ret = mpc_sie200_init(&MPC_PSRAM1_DEV,
                          MPC_PSRAM1_RANGE_LIST,
                          MPC_PSRAM1_RANGE_LIST_LEN);
    if (ret != MPC_SIE200_ERR_NONE) {
        DRIVERS_TRACE(0, "%s:%d, sie200 init fail. ret:%d", __FUNCTION__, __LINE__, ret);
        return ret;
    }
    mpc_sie200_set_ctrl(&MPC_PSRAM1_DEV, 0x10);
    ret = mpc_sie200_config_region(&MPC_PSRAM1_DEV,
        PSRAM_BASE, PSRAM_BASE+PSRAM_SIZE-1, MPC_SIE200_SEC_ATTR_NONSECURE);
    if (ret != MPC_SIE200_ERR_NONE) {
        DRIVERS_TRACE(0, "%s:%d, sie200 config fail. ret:%d", __FUNCTION__, __LINE__, ret);
        return ret;
    }
    mpc_sie200_irq_disable(&MPC_PSRAM1_DEV);
    mpc_sie200_lock_down(&MPC_PSRAM1_DEV);
    hal_sec_cfg_nonsec_bypass(SPY_PSRAM1_BASE, false);
#endif

    DRIVERS_TRACE(0, "MPC init successfully");
    return 0;
}

