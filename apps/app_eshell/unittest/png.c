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

#include "plat_addr_map.h"

#if defined(UTILS_ESHELL_EN) && defined(PNG_BASE)

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "cmsis_os.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_sysfreq.h"
#include "hal_location.h"
#include "hal_iomux.h"
#include "hal_png.h"

#include "display_test.h"

#include "ut_test.h"

POSSIBLY_UNUSED
static PSRAM_DATA_LOC uint8_t ALIGNED(64) rgb888_png[] = {
#include "pngdec/png_500x500_24bpp.txt"
};

static int test_width = 464;
static int test_height = 454;
static int test_format = VG_LITE_RGBA8888;

static int test_with_display = 0;

static void test_start(void *lcdc_buf)
{
    if (!test_with_display)
        return;

    lcdc_test_init();
    lcdc_test_layer_init(0, lcdc_buf);
    lcdc_test_start();

    test_width = lcdc_test_get_width();
    test_height = lcdc_test_get_height();
    test_format = lcdc_test_get_bpp() == 16 ? VG_LITE_BGR565: VG_LITE_RGBA8888;

    gpu_test_open(test_width, test_height, test_format, lcdc_buf);
}

static void test_stop(void)
{
    if (!test_with_display)
        return;

    gpu_test_close();

    lcdc_test_stop();
    lcdc_test_exit();
}

static void test_display_update(void)
{
    if (test_with_display) {
        lcdc_test_frame_xfer();
        lcdc_test_frame_wait_done();
    }
}

static void test_draw_png(uint8_t *png_data, uint32_t png_size, int count)
{
    uint32_t frames = 0;
    uint32_t start;
    struct HAL_PNG_CONFIG_T cfg;
    uint8_t *pix_buf = frame_test_buf1;
    uint8_t *lcdc_buf = frame_test_nc_buf0;

    hal_png_open();
    test_start(lcdc_buf);

    while (frames < count) {
        UT_LOG("----------------------------------------------------------------------");
        UT_LOG("#### Frame %u start.", frames);

        UT_LOG("#### Frame %u png decode start.", frames);
        start = hal_fast_sys_timer_get();

        memset(&cfg, 0, sizeof(cfg));
        cfg.png_data = png_data;
        cfg.png_size = png_size;
        cfg.pix_buf = pix_buf;
        cfg.pix_fmt = HAL_PNG_PIX_RGBA8888;

        if (hal_png_start(&cfg) != HAL_PNG_OK) {
            UT_LOG("PNG decode start failed. %d", cfg.result);
            break;
        }

        while (hal_png_busy()) {
            hal_sys_timer_delay(US_TO_TICKS(10));
            if (FAST_TICKS_TO_MS(hal_fast_sys_timer_get() - start) > 200)
                break;
        }

        if (hal_png_busy() || cfg.result != HAL_PNG_OK) {
            UT_LOG("PNG decode failed! %d", cfg.result);
            break;
        }

        UT_LOG("Image Info:");
        UT_LOG("  Width:          %u", cfg.img_width);
        UT_LOG("  Height:         %u", cfg.img_height);
        UT_LOG("  Color depth:    0x%x", cfg.img_color_depth);

        UT_LOG("#### Frame %u png decode done, %uus.", frames,
                    FAST_TICKS_TO_US(hal_fast_sys_timer_get() - start));

        if (test_with_display) {
            UT_LOG("#### Frame %u gpu drawing start.", frames+1);
            start = hal_fast_sys_timer_get();

            gpu_test_fill_color(0);
            gpu_test_blit(pix_buf, cfg.img_width, cfg.img_height, VG_LITE_RGBA8888,
                      lcdc_buf, test_width, test_height, test_format, 1.0f, frames % 360);

            UT_LOG("#### Frame %u gpu drawing done, %uus.", frames,
                        FAST_TICKS_TO_US(hal_fast_sys_timer_get() - start));

            start = hal_fast_sys_timer_get();
            test_display_update();
            UT_LOG("#### Frame %u transfer done, %uus", frames,
                        FAST_TICKS_TO_US(hal_fast_sys_timer_get() - start));
        }
        frames++;
    }

    test_stop();
    hal_png_close();
}


static void unitest_png_usage(void)
{
    eshell_putstring("Usage:\r\n");
    eshell_putstring("  utest_png decode format times [display]\r\n");
    eshell_putstring("    format:  rgb888\r\n");
    eshell_putstring("    times:   > 1, test times\r\n");
    eshell_putstring("    display: 1 or 0, display the image on LCD panel.\r\n");
}

static void unitest_png(int argc, char *argv[])
{
    int times = 10;

    if (argc < 4)
        goto usage;

    times = atoi(argv[3]);
    if (argc > 4)
        test_with_display = atoi(argv[4]);

    if (strncmp(argv[2], "rgb888", 6) == 0) {
        test_draw_png(rgb888_png, sizeof(rgb888_png), times);
    } else {
        goto usage;
    }

    return;

usage:
    unitest_png_usage();
}

ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_UNIT_TEST, "utest_png", "usage: utest_png help",
                   unitest_png);
#endif
