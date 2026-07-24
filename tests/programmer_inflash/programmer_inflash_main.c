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
#include "cmsis.h"
#include "cmsis_nvic.h"
#include "hal_cache.h"
#include "hal_cmu.h"
#include "hal_iomux.h"
#include "hal_location.h"
#include "tool_msg.h"
#include "tgt_hardware.h"
#include "hal_bootmode.h"


typedef void (*FLASH_ENTRY)(void);

extern void programmer_start(void);

extern uint32_t __app_entry_address__[];

extern uint32_t programmer_get_magic_number(void);
extern bool programmer_need_run_bflash(void);

#ifdef SINGLE_WIRE_DOWNLOAD
static POSSIBLY_UNUSED int BOOT_TEXT_FLASH_LOC programmer_inflash_is_single_line_download_mode(void)
{
    uint32_t bootmode = hal_sw_bootmode_get();

    return (bootmode & HAL_SW_BOOTMODE_SINGLE_LINE_DOWNLOAD);
}
#else
static POSSIBLY_UNUSED int BOOT_TEXT_FLASH_LOC programmer_inflash_is_single_line_download_mode(void)
{
    return false;
}
#endif

int BOOT_TEXT_FLASH_LOC ProgrammerInflashEnterApp(void)
{
    struct boot_struct_t * boot_struct;

    boot_struct = (struct boot_struct_t *)__app_entry_address__[0];

    if (BOOT_MAGIC_NUMBER == boot_struct->hdr.magic)
    {
        FLASH_ENTRY entry;
        // Disable all IRQs
        NVIC_DisableAllIRQs();
        // Ensure in thumb state
        entry = (FLASH_ENTRY)(&boot_struct->hdr + 1);
        entry = (FLASH_ENTRY)((uint32_t)entry | 1);
        entry();
        //can't run to here
        return 0;
    }
    return -1;
}

int BOOT_TEXT_FLASH_LOC WEAK ProgrammerInflashCheckBootToApp(void)
{
    return 0;
}

void BOOT_TEXT_FLASH_LOC ProgrammerInflashBootInit(void)
{
#if !defined(OTA_BOOT_TO_DUAL_CHIP) && !defined(USER_SECURE_BOOT)
    if (ProgrammerInflashCheckBootToApp()) {
        ProgrammerInflashEnterApp();
    }
#endif
    // Enable icache
    hal_cache_enable(HAL_CACHE_ID_I_CACHE);
    // Enable dcache
    hal_cache_enable(HAL_CACHE_ID_D_CACHE);
    // Enable write buffer
    hal_cache_writebuffer_enable(HAL_CACHE_ID_D_CACHE);

    // Init boot sections
    boot_init_boot_sections();
}
void boot_loader_pre_init_hook(void) __attribute__((alias("ProgrammerInflashBootInit")));

int BOOT_TEXT_FLASH_LOC programmer_inflash_main(int argc, char *argv[])
{
    programmer_start();

#ifdef UNCONDITIONAL_ENTER_SINGLE_WIRE_DOWNLOAD
    if (ProgrammerInflashEnterApp() < 0) {
        //enter app failed,try reboot.
        hal_cmu_sys_reboot();
    }
#endif

    return 0;
}
int _start(int,  char **) __attribute__((weak,alias("programmer_inflash_main")));
