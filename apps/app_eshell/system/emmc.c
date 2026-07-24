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
#if defined(UTILS_ESHELL_EN) && defined(CHIP_HAS_EMMC) && defined(__SYS_AS_MAIN__)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "card.h"
#include "eshell.h"
#include "hal_gpio.h"
#include "hal_timer.h"
#include "hal_iomux.h"
#include "emmc_error.h"
#include "emmc_config.h"
#include "hal_location.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif

//#define EMMC_DEBUG_ENABLE

#define EMMC_TRACE_SLE  2
#if   (EMMC_TRACE_SLE == 1)
#define EMMC_TRACE(s, ...)       TRACE(1, s, ##__VA_ARGS__)
#elif (EMMC_TRACE_SLE == 2)
#define EMMC_TRACE(s, ...)       eshell_putstring(s"\r\n", ##__VA_ARGS__)
#else
#define EMMC_TRACE(s, ...)
#endif

#ifdef EMMC_DEBUG_ENABLE
#define EMMC_DEBUG  EMMC_TRACE
#else
#define EMMC_DEBUG(...)
#endif

#if defined(EMMC_HS_MODE) && defined(EMMC_USE_PLL)
#define DEF_BUS_MODE    HS200
#else
#define DEF_BUS_MODE    SDR52
#endif
#define DEF_BUS_WIDTH   MMC_8BIT_WIDTH /* MMC_4BIT_WIDTH, MMC_1BIT_WIDTH */

/* used to test different interfaces */
//#define EMMC_TEST_BLOCKING_MODE
#define EMMC_TEST_DMA_MODE
#define EMMC_CALLBACK_ENABLE

/* Used to test the reading and writing function after opening EMMC's internal cache */
//#define EMMC_CACHE_ENABLE

/* Used to test the descriptor scatter function of ADMA2 */
//#define EMMC_ADMA_DESC_MODE

#if defined(EMMC_ADMA_DESC_MODE) && defined(CORE_SLEEP_POWER_DOWN)
#error "The ADMA test does not support in the PD mode, because the memory is too small"
#endif

#if defined(EMMC_TEST_BLOCKING_MODE) && defined(EMMC_TEST_DMA_MODE)
#error "Blocking mode and dma mode cannot be used at the same time"
#elif !defined(EMMC_TEST_BLOCKING_MODE) && !defined(EMMC_TEST_DMA_MODE) && defined(EMMC_CALLBACK_ENABLE)
#error "EMMC_CALLBACK_ENABLE cannot be defined in this mode"
#endif

static struct mmc_host emmc_host_dev;
static struct mmc_config emmc_cfg;
static struct mmc_card_info card_info;

static void emmc_delay(uint32_t ms)
{
    if (ms) {
        osDelay(ms);
    }
}

static volatile uint8_t host_error_flag = 0;
static void emmc_host_error(enum EMMC_HOST_ERR error)
{
    host_error_flag = 1;
    EMMC_TRACE("%s: error = %d", __FUNCTION__, error);
}

#ifdef EMMC_CALLBACK_ENABLE
static volatile uint8_t dma_done_flag = 0;
static void emmc_host_dma_done(void)
{
    dma_done_flag = 1;
}
#endif

#if defined(CHIP_BEST2009) || defined(CHIP_BEST3601) || defined(CHIP_BEST1701)
#define DEV_3V3_PIN     HAL_IOMUX_PIN_P11_2
#else
#define DEV_RST_PIN     HAL_IOMUX_PIN_P3_6
#define DEV_3V3_PIN     HAL_IOMUX_PIN_PMU_P2_3
#define DEV_1V8_PIN     HAL_IOMUX_PIN_PMU_P2_1
#endif
#define PWR_ON_LEVEL    1

static void emmc_pwr_ctrl_init(void)
{
#if defined(DEV_RST_PIN) || defined(DEV_3V3_PIN) || defined(DEV_1V8_PIN)
    struct HAL_IOMUX_PIN_FUNCTION_MAP iomux[] = {
        {HAL_IOMUX_PIN_NUM, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
    };
#endif

#ifdef DEV_RST_PIN
    //emmc reset pin
    if ((enum HAL_IOMUX_PIN_T)(DEV_RST_PIN) < HAL_IOMUX_PIN_NUM) {
        iomux[0].pin = (enum HAL_IOMUX_PIN_T)(DEV_RST_PIN);
        hal_iomux_init(iomux, ARRAY_SIZE(iomux));
    }
    //hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)(DEV_RST_PIN), HAL_GPIO_DIR_OUT, 1);
#endif

#ifdef DEV_3V3_PIN
    //emmc 3.3v power contrl pin
    if ((enum HAL_IOMUX_PIN_T)(DEV_3V3_PIN) < HAL_IOMUX_PIN_NUM) {
        iomux[0].pin = (enum HAL_IOMUX_PIN_T)(DEV_3V3_PIN);
        hal_iomux_init(iomux, ARRAY_SIZE(iomux));
    }
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)(DEV_3V3_PIN), HAL_GPIO_DIR_OUT, 0);
#endif

#ifdef DEV_1V8_PIN
    //emmc 1.8v power contrl pin
    if ((enum HAL_IOMUX_PIN_T)(DEV_1V8_PIN) < HAL_IOMUX_PIN_NUM) {
        iomux[0].pin = (enum HAL_IOMUX_PIN_T)(DEV_1V8_PIN);
        hal_iomux_init(iomux, ARRAY_SIZE(iomux));
    }
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)(DEV_1V8_PIN), HAL_GPIO_DIR_OUT, 0);
#endif
}

static void emmc_pwr_on_1v8(void)
{
#ifdef DEV_1V8_PIN
    //emmc 1.8v power contrl pin
    if (PWR_ON_LEVEL) {
        hal_gpio_pin_set((enum HAL_GPIO_PIN_T)(DEV_1V8_PIN));
    } else {
        hal_gpio_pin_clr((enum HAL_GPIO_PIN_T)(DEV_1V8_PIN));
    }
    EMMC_TRACE("%s: sys done", __FUNCTION__);
#endif
}

static void emmc_pwr_on_3v3(void)
{
#ifdef DEV_3V3_PIN
    //emmc 3.3v power contrl pin
    if (PWR_ON_LEVEL) {
        hal_gpio_pin_set((enum HAL_GPIO_PIN_T)(DEV_3V3_PIN));
    } else {
        hal_gpio_pin_clr((enum HAL_GPIO_PIN_T)(DEV_3V3_PIN));
    }
    EMMC_TRACE("%s: sys done", __FUNCTION__);
#endif
}

static void emmc_pwr_off_1v8(void)
{
#ifdef DEV_1V8_PIN
    //emmc 1.8v power contrl pin
    if (PWR_ON_LEVEL) {
        hal_gpio_pin_clr((enum HAL_GPIO_PIN_T)(DEV_1V8_PIN));
    } else {
        hal_gpio_pin_set((enum HAL_GPIO_PIN_T)(DEV_1V8_PIN));
    }
    EMMC_TRACE("%s: done", __FUNCTION__);
#endif
}

static void emmc_pwr_off_3v3(void)
{
#ifdef DEV_3V3_PIN
    //emmc 3.3v power contrl pin
    if (PWR_ON_LEVEL) {
        hal_gpio_pin_clr((enum HAL_GPIO_PIN_T)(DEV_3V3_PIN));
    } else {
        hal_gpio_pin_set((enum HAL_GPIO_PIN_T)(DEV_3V3_PIN));
    }
    EMMC_TRACE("%s: done", __FUNCTION__);
#endif
}

static void emmc_pwr_off_other(void)
{
    /*  If the RST pin is enabled, it needs to be pulled low at this time,
        otherwise the power consumption will increase
    */
#ifdef DEV_RST_PIN
    hal_gpio_pin_clr((enum HAL_GPIO_PIN_T)(DEV_RST_PIN));
    EMMC_TRACE("%s: done", __FUNCTION__);
#endif
}

static int emmc_host_read(struct mmc_host *host, uint32_t offset,
                          uint32_t size, uint8_t *buf)
{
    int ret = MMC_NO_ERR;
    POSSIBLY_UNUSED uint32_t timeb, time = 0xffffffff;

    EMMC_TRACE("EMMC Read: offset 0x%x, size 0x%x, to address %p",
               offset, size, buf);

    host_error_flag = 0;
#ifdef EMMC_CALLBACK_ENABLE
    dma_done_flag = 0;
#endif

    timeb = hal_fast_sys_timer_get();

#if defined(EMMC_TEST_BLOCKING_MODE)
    ret = read_card_blocking_mode(host, offset, (void *)buf, size);
#elif defined(EMMC_TEST_DMA_MODE)
    ret = read_card_dma_mode(host, offset, (void *)buf, size);
#ifdef EMMC_CALLBACK_ENABLE
    if (ret == MMC_NO_ERR) {
        if (host_error_flag) {
            EMMC_TRACE("EMMC Read: DMA xfer error.");
            ret = -1;
        } else {
            int timeout_ms = 2000;
            while (!dma_done_flag && timeout_ms-- > 0)
                host->cfg->ms_delay(1);
            if (timeout_ms <= 0)
                ret = -1;
        }
    }
#endif
#else
    ret = read_card(host, offset, (void *)buf, size);
#endif

    time = FAST_TICKS_TO_US(hal_fast_sys_timer_get() - timeb);

    if (ret != MMC_NO_ERR) {
        EMMC_TRACE("EMMC Read: failed, ret = %d", ret);
    } else {
        EMMC_DEBUG("EMMC Read: completed!");
    }

    EMMC_DEBUG("---------------EMMC Read------------------------");
    EMMC_DEBUG("Size      :%d bytes", size);
    EMMC_DEBUG("Time      :read[%dus]", time);
    time = (uint32_t)(95400 * size / 1000 / time);
    EMMC_DEBUG("Speed     :%u.%uMB/s", time / 100, time % 100);
    EMMC_DEBUG("------------------------------------------------");
    return ret;
}

static int emmc_host_write(struct mmc_host *host, uint32_t offset,
                           uint32_t size, const uint8_t *buf)
{
    int ret = MMC_NO_ERR;
    POSSIBLY_UNUSED uint32_t timeb, time = 0xffffffff;

    EMMC_TRACE("EMMC Write: offset 0x%x, size 0x%x, from address %p",
               offset, size, buf);

    host_error_flag = 0;
#ifdef EMMC_CALLBACK_ENABLE
    dma_done_flag = 0;
#endif

    timeb = hal_fast_sys_timer_get();

#if defined(EMMC_TEST_BLOCKING_MODE)
    ret = write_card_blocking_mode(host, offset, (void *)buf, size);
#elif defined(EMMC_TEST_DMA_MODE)
    ret = write_card_dma_mode(host, offset, (void *)buf, size);
#ifdef EMMC_CALLBACK_ENABLE
    if (ret == MMC_NO_ERR) {
        if (host_error_flag) {
            EMMC_TRACE("EMMC Write: DMA xfer error.");
            ret = -1;
        } else {
            int timeout_ms = 2000;
            while (!dma_done_flag && timeout_ms-- > 0)
                host->cfg->ms_delay(1);
            if (timeout_ms <= 0)
                ret = -1;
        }
    }
#endif
#else
    ret = write_card(host, offset, (void *)buf, size);
#endif

    time = FAST_TICKS_TO_US(hal_fast_sys_timer_get() - timeb);

    if (ret != MMC_NO_ERR) {
        EMMC_TRACE("EMMC Write: failed, ret = %d", ret);
    } else {
        EMMC_DEBUG("EMMC Write: completed!");
    }

    EMMC_DEBUG("---------------------EMMC Write--------------------");
    EMMC_DEBUG("Size      :%d bytes", size);
    EMMC_DEBUG("Time      :%dus", time);
    time = (uint32_t)(95400 * size / 1000 / time);
    EMMC_DEBUG("Speed     :%u.%uMB/s", time / 100, time % 100);
    EMMC_DEBUG("---------------------------------------------------");
    return ret;
}

static int emmc_host_erase(struct mmc_host *host, uint32_t offset,
                           uint32_t size)
{
    int ret = MMC_NO_ERR;
    POSSIBLY_UNUSED uint32_t timeb, time = 0xffffffff;

    EMMC_TRACE("EMMC Erase: offset 0x%x, size 0x%x", offset, size);

    timeb = hal_fast_sys_timer_get();

    ret = erase_card(host, offset, size);

    time = FAST_TICKS_TO_US(hal_fast_sys_timer_get() - timeb);
    if (ret != MMC_NO_ERR)
        EMMC_TRACE("EMMC Erase: failed, ret = %d", ret);
    else
        EMMC_DEBUG("EMMC Erase: completed!");

    EMMC_DEBUG("---------------------EMMC Erase--------------------");
    EMMC_DEBUG("Size     :%d bytes", size);
    EMMC_DEBUG("Time     :%dus", time);
    time = (uint32_t)(95400 * size / 1000 / time);
    EMMC_DEBUG("Speed    :%u.%uMB/s", time / 100, time % 100);
    EMMC_DEBUG("---------------------------------------------------");
    return ret;
}

/*********************************************************/

static bool emmc_inited = false;

bool emmc_is_inited(void)
{
    return emmc_inited;
}

uint32_t emmc_block_count(void)
{
    if (emmc_inited)
        return card_info.blocknbr;
    return 0;
}

uint32_t emmc_block_size(void)
{
    if (emmc_inited)
        return card_info.blocksize;
    return 0;
}

int emmc_write(uint32_t offset, uint32_t size, uint32_t addr)
{
    struct mmc_host *host = &emmc_host_dev;

    if (!emmc_inited) {
        eshell_putstring("EMMC is not inited.\r\n");
        return -1;
    }

    return emmc_host_write(host, offset, size, (const uint8_t *)addr);
}

int emmc_read(uint32_t offset, uint32_t size, uint32_t addr)
{
    struct mmc_host *host = &emmc_host_dev;

    if (!emmc_inited) {
        eshell_putstring("EMMC is not inited.\r\n");
        return -1;
    }

    return emmc_host_read(host, offset, size, (uint8_t *)addr);
}

int emmc_erase(uint32_t offset, uint32_t size)
{
    struct mmc_host *host = &emmc_host_dev;

    if (!emmc_inited) {
        eshell_putstring("EMMC is not inited.\r\n");
        return -1;
    }
    return emmc_host_erase(host, offset, size);
}

void emmc_info(void)
{
    struct mmc_config *cfg = &emmc_cfg;

    if (!emmc_inited) {
        eshell_putstring("EMMC is not inited.\r\n");
        return;
    }

    dump_card_cfg(cfg);
}

int emmc_init(uint32_t emmc_mode, uint32_t emmc_width)
{
    POSSIBLY_UNUSED int emmc_khz = 0;
    int ret = MMC_NO_ERR;
    struct mmc_host *host = &emmc_host_dev;
    struct mmc_config *cfg = &emmc_cfg;

    if (emmc_inited) {
        EMMC_TRACE("EMMC is inited!");
        return 0;
    }

    memset((void *)cfg, 0, sizeof(struct mmc_config));

    cfg->id       = MMC_HOST_ID_0;
    cfg->type     = DEV_TYPE_EMMC;
#ifdef EMMC_ADMA_DESC_MODE
    cfg->dma_mode = MM4_32_BIT_ADMA_2;
#else
    cfg->dma_mode = MM4_SDMA;
#endif
    cfg->auto_cmd_type = MM4_AUTO_CMD12;
    cfg->desc_sz   = ADMA2_DESC_MAX_LEN;
    cfg->desc_nr   = 1;
#ifdef EMMC_ADMA_DESC_MODE
    cfg->desc_type = ADMA2_DESC_SCATTER;
#else
    cfg->desc_type = ADMA2_DESC_FIXED_SZ;
#endif
    /* data transfer */
    cfg->blksz     = CARD_BLOCK_LENGTH;
    /* callback */
    cfg->ms_delay  = emmc_delay;
    cfg->emmc_host_error   = emmc_host_error;
    cfg->emmc_host_dma_done = emmc_host_dma_done;
    cfg->timing = emmc_mode;
    cfg->width = emmc_width;
    /* device power control */
    cfg->dev_pwr_on_time = 10;
    cfg->dev_reset = NULL;
    cfg->dev_pwr_on_1v8 = emmc_pwr_on_1v8;
    cfg->dev_pwr_on_3v3 = emmc_pwr_on_3v3;
    cfg->dev_pwr_off_1v8 = emmc_pwr_off_1v8;
    cfg->dev_pwr_off_3v3 = emmc_pwr_off_3v3;
    cfg->dev_pwr_off_other = emmc_pwr_off_other;
    cfg->dev_pwr_ctrl_init = emmc_pwr_ctrl_init;

    switch (cfg->timing) {
        case DS:
            cfg->khz = F_DS;
            break;
        case SDR52:
            cfg->khz = F_SDR52;
            break;
        case DDR52:
            cfg->khz = F_DDR52;
            break;
        case HS200:
            cfg->khz = F_HS200;
            break;
        case HS400:
        case EHS400:
            cfg->khz = F_HS400;
            break;
        default:
            return -1;
    }

    dump_card_cfg(cfg);
    ret = open_card(host, cfg);

    memset(&card_info, 0, sizeof(card_info));
    ret = get_card_info(host, &card_info);
    if (ret != MMC_NO_ERR) {
        EMMC_TRACE("EMMC Init: faild to get card info, ret = %d, error_code = %d",
                   ret, host->error_code);
        return ret;
    }

    EMMC_TRACE("  ");
    EMMC_TRACE("EMMC Init: completed!");
    emmc_inited = true;
    return ret;
}

int emmc_init_default(void)
{
    return emmc_init(DEF_BUS_MODE, DEF_BUS_WIDTH);
}

static void emmc_cmd_usage(void)
{
    eshell_putstring("Usage:\r\n");
    eshell_putstring("  emmc init [bus_mode bus_width]\r\n");
    eshell_putstring("      bus_mode: SDR52|DDR52|HS200|HS400, SDR52 is default\r\n");
    eshell_putstring("      bus_width: 8|4|1, 8 bits is default\r\n");
    eshell_putstring("  emmc info\r\n");
    eshell_putstring("  emmc write offset size addr\r\n");
    eshell_putstring("      offset: write start offset, HEX\r\n");
    eshell_putstring("      size:   write size, HEX\r\n");
    eshell_putstring("      addr:   source address in memory, HEX\r\n");
    eshell_putstring("  emmc read offset size addr\r\n");
    eshell_putstring("      offset: read start offset, HEX\r\n");
    eshell_putstring("      size:   read size, HEX\r\n");
    eshell_putstring("      addr:   destination address in memory, HEX\r\n");
    eshell_putstring("  emmc erase offset size\r\n");
    eshell_putstring("      offset: erase start offset, HEX\r\n");
    eshell_putstring("      size:   erase size, HEX\r\n");
}

static void emmc_cmd(int argc, char *argv[])
{
    uint32_t offset = 0;
    uint32_t size = 0;
    uint32_t addr = 0;

    if (argc < 2)
        goto usage;

    if (strncmp(argv[1], "init", 4) == 0) {
        uint32_t mode = DEF_BUS_MODE;
        uint32_t width = DEF_BUS_WIDTH;
        if (argc > 2) {
            if (strncmp(argv[2], "SDR52", 5) == 0
                || strncmp(argv[2], "sdr52", 5) == 0)
                mode = SDR52;
            else if (strncmp(argv[2], "DDR52", 5) == 0
                     || strncmp(argv[2], "ddr52", 5) == 0)
                mode = DDR52;
            else if (strncmp(argv[2], "HS200", 5) == 0
                     || strncmp(argv[2], "HS200", 5) == 0)
                mode = HS200;
            else if (strncmp(argv[2], "HS400", 5) == 0
                     || strncmp(argv[2], "HS400", 5) == 0)
                mode = HS400;
            else
                goto usage;
        }
        if (argc > 3) {
            if (strncmp(argv[3], "8", 1) == 0)
                width = MMC_8BIT_WIDTH;
            else if (strncmp(argv[3], "4", 1) == 0)
                width = MMC_4BIT_WIDTH;
            else if (strncmp(argv[3], "1", 1) == 0)
                width = MMC_1BIT_WIDTH;
            else
                goto usage;
        }

        emmc_init(mode, width);

    } else if (strncmp(argv[1], "info", 4) == 0) {
        emmc_info();

    } else if (strncmp(argv[1], "write", 5) == 0) {
        if (argc < 5)
            goto usage;

        offset = strtoul(argv[2], NULL, 16);
        size = strtoul(argv[3], NULL, 16);
        addr = strtoul(argv[4], NULL, 16);

        emmc_write(offset, size, addr);

    } else if (strncmp(argv[1], "read", 4) == 0) {
        if (argc < 5)
            goto usage;

        offset = strtoul(argv[2], NULL, 16);
        size = strtoul(argv[3], NULL, 16);
        addr = strtoul(argv[4], NULL, 16);

        emmc_read(offset, size, addr);

    } else if (strncmp(argv[1], "erase", 5) == 0) {
        if (argc < 4)
            goto usage;

        sscanf(argv[2], "%x", &offset);
        sscanf(argv[3], "%x", &size);

        emmc_erase(offset, size);

    } else {
        goto usage;
    }

    return;
usage:
    emmc_cmd_usage();
}
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_SYSTEM, "emmc", "emmc help", emmc_cmd);

#endif //UTILS_ESHELL_EN
