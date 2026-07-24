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
#ifndef __PLAT_ADDR_MAP_BEST1307P_H__
#define __PLAT_ADDR_MAP_BEST1307P_H__

#ifdef __cplusplus
extern "C" {
#endif

#define PATCH_RAM_BASE                          0x20006800
#if defined(ROM_BUILD)
#define ROM_BASE                                0x26000000
#define ROMX_BASE                               0x04020000
#else
#define ROM_BASE                                0x22000000
#define ROMX_BASE                               0x00020000
#endif

#ifndef ROM_SIZE
#define ROM_SIZE                                0x00010000
#endif
#define ROM_EXT_SIZE                            0x00060000

#if defined(BTHOST_ROM_TEST)
#define BTH_ROM_BASE                            0x2c200000
#define BTH_ROMX_BASE                           0x0c200000
#else
#define BTH_ROM_BASE                            0x22100000
#define BTH_ROMX_BASE                           0x00100000
#endif

#ifndef BTHOST_ROM_SIZE
#if defined(BTHOST_ROM_TEST)
#define BTHOST_ROM_SIZE                         0x0009D800
#else
#define BTHOST_ROM_SIZE                         0x0006D800
#endif


/*BTHOST stack ram size for .data and .bss*/
#define BTHOST_RAM_BASE                         0x20000000

#ifndef BTHOST_RAM_SIZE
#define BTHOST_RAM_SIZE                         0x00008000
#endif

#endif

#ifndef BTH_ROM_SIZE
#define BTH_ROM_SIZE                            0x000A8000
#endif

#define BTH_RAM_BASE                            0x20000000

#ifndef BTH_RAM_SIZE
#define BTH_RAM_SIZE                            0x00006400
#endif

#define PATCH_ENTRY_NUM                         8
#define PATCH_CTRL_BASE                         0x0008F000
#define PATCH_DATA_BASE                         0x0008F100

#define PATCH1_ENTRY_NUM                        8
#define PATCH1_CTRL_BASE                        0x001CF000
#define PATCH1_DATA_BASE                        0x001CF100

#define RAM_SECURITY_FLAG                       0x04000000

#ifdef BTH_IN_ROM
#define RAM0_BASE                               0x20010000
#define RAMX0_BASE                              0x00210000
#else
#define RAM0_BASE                               0x20000000
#define RAMX0_BASE                              0x00200000
#endif
#define RAM1_BASE                               0x20040000
#define RAMX1_BASE                              0x00240000
#define RAM2_BASE                               0x20048000
#define RAMX2_BASE                              0x00248000
#define RAM3_BASE                               0x20058000
#define RAMX3_BASE                              0x00258000
#define RAM4_BASE                               0x20068000
#define RAMX4_BASE                              0x00268000
#define RAM5_BASE                               0x20070000
#define RAMX5_BASE                              0x00270000

#ifdef BTH_IN_ROM
#define RAM0_SIZE                               0x00030000
#else
#define RAM0_SIZE                               0x00040000
#endif
#define RAM1_SIZE                               0x00008000
#define RAM2_SIZE                               0x00010000
#define RAM3_SIZE                               0x00010000
#define RAM4_SIZE                               0x00008000
#define RAM5_SIZE                               0x00020000
#ifndef RAM_TOTAL_SIZE
#define RAM_TOTAL_SIZE                          (RAM5_BASE + RAM5_SIZE - RAM0_BASE)
#endif
#ifndef AP_MAILBOX_SIZE
#define AP_MAILBOX_SIZE                         0
#endif

#ifdef CP_IN_SAME_EE
#ifndef RAMCPX_SIZE
#define RAMCPX_SIZE                             (RAM4_SIZE)
#endif
#define RAMCPX_BASE                             (RAM_BASE + RAM_SIZE - RAM0_BASE + RAMX0_BASE)

#ifndef RAMCP_SIZE
#define RAMCP_SIZE                              (RAM5_SIZE)
#endif
#define RAMCP_BASE                              (RAMCPX_BASE + RAMCPX_SIZE - RAMX0_BASE + RAM0_BASE)

#define RAMCP_TOP                               (RAMCP_BASE + RAMCP_SIZE)
#elif defined(CP_AS_SUBSYS) || defined(CHIP_ROLE_CP)
#define RAMCP_SUBSYS_BASE                       RAM0_BASE
#ifndef RAMCP_SIZE
#if defined(NO_SUBSYS_RAM)
#define RAMCP_SUBSYS_SIZE                       (0)
#else
#define RAMCP_SUBSYS_SIZE                       (RAM2_BASE - RAM0_BASE)
#endif
#else
#define RAMCP_SUBSYS_SIZE                       RAMCP_SIZE
#endif

#define CP_SUBSYS_MAILBOX_SIZE                  0x20

#if RAMCP_SUBSYS_SIZE != 0
#define CP_SUBSYS_MAILBOX_BASE                  (RAMCP_SUBSYS_BASE + RAMCP_SUBSYS_SIZE - CP_SUBSYS_MAILBOX_SIZE)
#else
#define CP_SUBSYS_MAILBOX_BASE                  (RAM5_BASE + RAM5_SIZE - CP_SUBSYS_MAILBOX_SIZE)
#endif

#define RAMCP_TOP                               (RAMCP_SUBSYS_BASE + RAMCP_SUBSYS_SIZE)

#if defined(CHIP_ROLE_CP) && defined(NO_SUBSYS_RAM) && (PSRAM_REGION_SIZE > 0)
#define RAM_NC_SIZE                             (0x2000)
#define RAM_NC_BASE                             (PSRAM_NC_REGION_BASE + PSRAM_REGION_SIZE - RAM_NC_SIZE)
#endif

#else
#define RAMCP_TOP                               RAM0_BASE
#endif


#ifdef CORE_SLEEP_POWER_DOWN

#ifdef MEM_POOL_BASE
#if (MEM_POOL_BASE < RAMCP_TOP)
#error "Bad MEM_POOL_BASE with CORE_SLEEP_POWER_DOWN"
#endif
#else
#define MEM_POOL_BASE                           RAMCP_TOP
#endif

#ifdef MEM_POOL_SIZE
#if (MEM_POOL_BASE + MEM_POOL_SIZE > RAM_NORM_BASE)
#error "Bad MEM_POOL_SIZE with CORE_SLEEP_POWER_DOWN"
#endif
#else
#define MEM_POOL_SIZE                           (RAM4_BASE - MEM_POOL_BASE)
#endif

#ifdef RAM_NORM_BASE
#if (RAM_NORM_BASE < MEM_POOL_BASE)
#error "Bad RAM_BASE with CORE_SLEEP_POWER_DOWN"
#endif
#else
#define RAM_NORM_BASE                                (MEM_POOL_BASE + MEM_POOL_SIZE)
#endif
#define RAMX_NORM_BASE                               (RAM_NORM_BASE - RAM0_BASE + RAMX0_BASE)

#else /* !CORE_SLEEP_POWER_DOWN */

#ifdef CHIP_ROLE_CP
#define RAM_NORM_BASE                                RAMCP_SUBSYS_BASE
#elif defined(RAM_NORM_BASE)
#if (RAM_NORM_BASE < RAMCP_TOP)
#error "Bad RAM_BASE"
#endif
#else
#define RAM_NORM_BASE                                RAM0_BASE
#endif
#define RAMX_NORM_BASE                               (RAM_NORM_BASE - RAM0_BASE + RAMX0_BASE)

#endif /* !CORE_SLEEP_POWER_DOWN */

#ifdef CHIP_ROLE_CP
#define RAM_NORM_SIZE                                (CP_SUBSYS_MAILBOX_BASE - RAMCP_SUBSYS_BASE)
#elif defined(RAM_NORM_SIZE)
#if (RAM_NORM_BASE + RAM_NORM_SIZE) > (RAM5_BASE + RAM5_SIZE)
#error "Bad RAM_SIZE"
#endif
#else
#if defined(CP_IN_SAME_EE) || defined(CP_AS_SUBSYS)
#define RAM_NORM_SIZE                                (RAM_TOTAL_SIZE - RAMCP_SIZE - RAMCPX_SIZE)
#else
#define RAM_NORM_SIZE                                (RAM_TOTAL_SIZE)
#endif
#endif

#if defined(ARM_CMSE) || defined(ARM_CMNS)
/*MPC: SRAM block size: 0x8000, FLASH block size 0x40000*/
#ifdef LARGE_SE_RAM
#define RAM_S_SIZE                              0x00038000
#else
#define RAM_S_SIZE                              0x00018000
#endif
#define RAM_NSC_SIZE                            0x00008000
#ifndef FLASH_S_SIZE
#define FLASH_S_SIZE                            0x00040000
#endif

#undef RAM_BASE
#undef RAMX_BASE
#undef RAM_SIZE
#undef RAMCP_BASE
#undef RAMCPX_BASE
#undef RAMCP_SIZE
#undef RAMCPX_SIZE
#undef RAM_NORM_BASE
#undef RAMX_NORM_BASE
#undef RAM_NORM_SIZE

#define RAMCPX_BASE                             (RAMX0_BASE + RAM_S_SIZE + RAM_NSC_SIZE)
#define RAMCPX_SIZE                             0x20000
#define RAMCP_BASE                              (RAM0_BASE + RAM_S_SIZE + RAM_NSC_SIZE + RAMCPX_SIZE)
#define RAMCP_SIZE                              0x20000
#define RAM_NS_BASE                             (RAMCP_BASE + RAMCP_SIZE)
#define RAMX_NS_BASE                            (RAMCPX_BASE + RAMCPX_SIZE + RAMCP_SIZE)
#define RAM_NS_SIZE                             (RAM6_BASE + RAM6_SIZE - RAM_NORM_BASE)
#define RAM_S_BASE                              (RAM_NORM_BASE)
#define RAMX_S_BASE                             (RAMX_NORM_BASE)

#if defined(ARM_CMNS)
#define RAM_NORM_BASE                                RAM_NS_BASE
#define RAMX_NORM_BASE                               RAMX_NS_BASE
#define RAM_NORM_SIZE                                RAM_NS_SIZE
#else
#if ((RAM_S_SIZE + RAM_NSC_SIZE) & (0x8000-1))
#error "RAM_S_SIZE should be 0x8000 aligned"
#endif
#if (FLASH_S_SIZE & (0x40000-1))
#error "FLASH_S_SIZE should be 0x40000 aligned"
#endif

#define RAM_NORM_BASE                                RAM0_BASE
#define RAMX_NORM_BASE                               RAMX0_BASE
#define RAM_NORM_SIZE                                RAM_S_SIZE
#ifndef NS_APP_START_OFFSET
#define NS_APP_START_OFFSET                     (FLASH_S_SIZE)
#endif
#ifndef FLASH_REGION_SIZE
#define FLASH_REGION_SIZE                       FLASH_S_SIZE
#endif
#endif
#else
#define RAM_S_BASE                              RAM_NS_TO_S(RAM_NORM_BASE)
#define RAM_S_SIZE                              RAM_NORM_SIZE
#define RAMX_S_BASE                             RAM_NS_TO_S(RAMX_NORM_BASE)

#define RAM_NS_BASE                             RAM_S_TO_NS(RAM_NORM_BASE)
#define RAM_NS_SIZE                             RAM_NORM_SIZE
#define RAMX_NS_BASE                            RAM_S_TO_NS(RAMX_NORM_BASE)
#endif /* defined(ARM_CMSE) || defined(ARM_CMNS) */

#if defined(NO_TRUSTZONE) || defined(ARM_CMNS) || \
        !(defined(ARM_CMSE) || defined(ROM_BUILD) || defined(PROGRAMMER))
#define RAM_BASE                                RAM_NS_BASE
#if !defined(RAM_SIZE)
#define RAM_SIZE                                RAM_NS_SIZE
#endif
#define RAMX_BASE                               RAMX_NS_BASE
#else
#define RAM_BASE                                RAM_S_BASE
#if !defined(RAM_SIZE)
#define RAM_SIZE                                RAM_S_SIZE
#endif
#define RAMX_BASE                               RAMX_S_BASE
#endif


#define REAL_FLASH_BASE                         0x2C000000
#define REAL_FLASH_NC_BASE                      0x28000000
#define REAL_FLASHX_BASE                        0x0C000000
#define REAL_FLASHX_NC_BASE                     0x08000000

#define ICACHE_CTRL_BASE                        0x07FFA000
#define ICACHE_SIZE                             0x00004000

#define CMU_BASE                                0x40000000
#define MCU_WDT_BASE                            0x40001000
#define MCU_TIMER0_BASE                         0x40002000
#define MCU_TIMER1_BASE                         0x40003000
#define MCU_TIMER2_BASE                         0x40004000

#define I2C0_BASE                               0x40005000
#define I2C1_BASE                               0x40006000
#define SPI_BASE                                0x40007000
#define TRNG_BASE                               0x40008000
#define ISPI_BASE                               0x40009000
#define UART0_BASE                              0x4000B000
#define UART1_BASE                              0x4000C000
#define BTPCM_BASE                              0x4000E000
#define I2S0_BASE                               0x4000F000
#define SEC_CTRL_BASE                           0x40030000
#define MPC_FLASH0_BASE                         0x40032000
#define MPC_SRAM0_BASE                          0x40034000
#define MPC_SRAM1_BASE                          0x40035000
#define MPC_SRAM2_BASE                          0x40036000
#define PAGE_SPY_BASE                           0x40050000
#define I2C2_BASE                               0x40060000

#define AON_CMU_BASE                            0x40080000
#define AON_GPIO_BASE                           0x40081000
#define AON_WDT_BASE                            0x40082000
#define AON_PWM_BASE                            0x40083000
#define AON_TIMER0_BASE                         0x40084000
#define AON_PSC_BASE                            0x40085000
#define AON_IOMUX_BASE                          0x40086000
#define I2C_SLAVE_BASE                          0x40087000
#define AON_SEC_CTRL_BASE                       0x40088000
#define AON_TIMER1_BASE                         0x4008B000
#define AON_PWM1_BASE                           0x4008C000

#define AUDMA_BASE                              0x40120000
#define GPDMA_BASE                              0x40130000
#define REAL_FLASH_CTRL_BASE                    0x40140000
#define BTDUMP_BASE                             0x40150000
#define SEDMA_BASE                              0x401D0000
#define CPUDUMP_BASE                            0x401E0000

#define CODEC_BASE                              0x40300000

#ifndef BTC_ROM_RAM_BASE
#define BTC_ROM_RAM_BASE                        0xA0000000
#endif

#ifndef BTC_ROM_RAM_SIZE
#define BTC_ROM_RAM_SIZE                        0x00098000
#endif

/* need update
#define ROM_CRC                                 0x46C760C1
#define BTH_ROM_CRC                             0x270D5FBF
#define BTC_ROM_CRC                             0x207CF48C
*/

#define BT_RAM_BASE                             0xC0000000
#define BT_RAM_SIZE                             0x00010000
#define BT_EXCH_MEM_BASE                        0xD0200000
#define BT_EXCH_MEM_SIZE                        0x0000A000
#define BT_UART_BASE                            0xD0300000
#define BT_CMU_BASE                             0xD0330000
#define BTC_CPUDUMP_BASE                        0xD0700000

#define TIMER0_BASE                             AON_TIMER0_BASE
#define TIMER1_BASE                             MCU_TIMER1_BASE
#define TIMER2_BASE                             MCU_TIMER2_BASE

#define FLASH_BASE                              REAL_FLASH_BASE
#define FLASH_NC_BASE                           REAL_FLASH_NC_BASE
#define FLASHX_BASE                             REAL_FLASHX_BASE
#define FLASHX_NC_BASE                          REAL_FLASHX_NC_BASE

#define FLASH_CTRL_BASE                         REAL_FLASH_CTRL_BASE

#define IOMUX_BASE                              AON_IOMUX_BASE
#define GPIO_BASE                               AON_GPIO_BASE
#define PWM_BASE                                AON_PWM_BASE
#define PWM1_BASE                               AON_PWM1_BASE
#define WDT_BASE                                AON_WDT_BASE

/* For linker scripts */
#define VECTOR_SECTION_SIZE                     380
#define REBOOT_PARAM_SECTION_SIZE               64
#define ROM_BUILD_INFO_SECTION_SIZE             40
#define ROM_EXPORT_FN_SECTION_SIZE              128
#define BT_INTESYS_MEM_OFFSET                   0x00004000

/* For module features */
#define CODEC_FREQ_CRYSTAL                      CODEC_FREQ_24M
#define CODEC_FREQ_EXTRA_DIV                    2
#define CODEC_PLL_DIV                           16
#define CODEC_CMU_DIV                           8
#define CODEC_PLAYBACK_BIT_DEPTH                24
#define CODEC_HAS_FIR
#define GPADC_CTRL_VER                          3
#define GPADC_VALUE_BITS                        16
#define GPADC_HAS_VSYS_DIV
#define GPADC_HAS_EXT_SLOPE_CAL
#define GPADC_CUSTOM_CALIB_VAL
#define GPADC_SAR_INPUT_BUF_CTRL
#define GPIO_HAS_BOTH_EDGE_IRQ
#define PAGE_SPY_HAS_IRQ
#define PWRKEY_IRQ_IN_PMU
#ifndef AUD_SECTION_STRUCT_VERSION
#define AUD_SECTION_STRUCT_VERSION              4
#endif
#define PAGE_SPY_VER                            2
#define UARTCOM_USE_LED2

/* TODO: MCU has 64M-byte space but SENS only has 16M-byte space */
#define HAL_NORFLASH_ADDR_MASK                  0x00FFFFFF
#define RMT_IPC_API_ENABLE

#define DUMPPC_TOTAL_NUM                        64
#define DUMPPC_NUM                              100

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
#define NO_FLASH_S_ACCESS
#endif

/* #define FLASH_CACHE_DOUBLE_LINEFILL */
/* #define PSRAM_CACHE_DOUBLE_LINEFILL */

/* For boot struct version */
#ifndef SECURE_BOOT_VER
#define SECURE_BOOT_VER                         5
#endif

/* For ROM export functions */
#define NO_MEMMOVE
#define NO_EXPORT_QSORT
#define NO_EXPORT_BSEARCH
#define NO_EXPORT_VSSCANF

#ifdef __cplusplus
}
#endif

#endif
