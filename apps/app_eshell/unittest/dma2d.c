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

#if defined(UTILS_ESHELL_EN) && defined(IMGDMA_BASE)

#include "stdlib.h"
#include "string.h"
#include "cmsis_os.h"
#include "hal_timer.h"
#include "hal_sysfreq.h"
#include "hal_dma2d.h"
#include "display_test.h"

#include "eshell.h"
#include "ut_test.h"

#ifndef TEST_WIDTH
#define TEST_WIDTH      320
#endif
#ifndef TEST_HEIGHT
#define TEST_HEIGHT     320
#endif
#define TEST_MAX_SIZE       (TEST_WIDTH * TEST_HEIGHT * 4)

static int test_width = TEST_WIDTH;
static int test_height = TEST_HEIGHT;
static int test_format = DMA2D_RGBA8888;

static osSemaphoreId_t dma2d_sem = NULL;

static void dma2d_handle(int error)
{
    if (dma2d_sem)
        osSemaphoreRelease(dma2d_sem);
}

static void dma2d_wait_done(void)
{
    if (dma2d_sem) {
        osSemaphoreAcquire(dma2d_sem, 200);
    } else {
        while (hal_dma2d_busy()) {
            osDelay(1);
        }
    }
}

static int check_color(uint8_t *buf, uint32_t color)
{
    int err = 0;
    uint32_t *p = (uint32_t *)buf;
    for (int i = 0; i < test_width * test_height; i++) {
        if ((p[i] & 0xffffff) != (color & 0xffffff)) {
            eshell_putstring("addr %p = 0x%x, expect 0x%x\r\n", &p[i], p[i], color);
            err++;
        }
    }
    return err;
}

static bool is_same_color(uint32_t c1, uint32_t c2)
{
    int r1 = c1 & 0xff;
    int g1 = (c1 >> 8) & 0xff;
    int b1 = (c1 >> 16) & 0xff;
    int r2 = c2 & 0xff;
    int g2 = (c2 >> 8) & 0xff;
    int b2 = (c2 >> 16) & 0xff;
    if (abs(r1-r2) <= 2 && abs(g1-g2) <= 2 && abs(b1-b2) <= 2)
        return true;
    return false;
}

static void sw_blend_buffer(struct DMA2D_BUFFER_T *src, struct DMA2D_BUFFER_T *dst,
                            uint32_t color, uint8_t opa)
{
    disp_buffer_t b1 = {
        .width = src->width,
        .height = src->height,
        .format = FB_FMT_RGB32,
        .data = src->data,
    };
    disp_buffer_t b2 = {
        .width = dst->width,
        .height = dst->height,
        .format = FB_FMT_RGB32,
        .data = dst->data,
    };
    disp_sw_blend(&b1, NULL, &b2, NULL, FB_CONST_ALPHA, 0xff - opa);
}

static int compare_blend_buffer(uint8_t *buf, uint8_t *swbuf)
{
    int errs = 0;
    uint32_t *p1 = (uint32_t *)buf;
    uint32_t *p2 = (uint32_t *)swbuf;
    for (int i = 0; i < TEST_WIDTH*TEST_HEIGHT; i++) {
        if (!is_same_color(p1[i], p2[i])) {
            if (errs < 32) {
                eshell_putstring("ERROR: %6d, DMA2D got 0x%x, CPU got 0x%x\r\n", i, p1[i], p2[i]);
                osDelay(1);
            }
            errs++;
        }
    }
    return errs;
}

POSSIBLY_UNUSED
static void test_fillcolor(int times)
{
    eshell_putstring("dma2d fillcolor test start:\r\n");

    hal_dma2d_open(dma2d_handle);
    if (!dma2d_sem)
        dma2d_sem = osSemaphoreNew(1, 0, NULL);

    uint32_t color = 0xffffffff;
    struct DMA2D_BUFFER_T buf = {
        .width = test_width,
        .height = test_height,
        .format = test_format,
        .data = frame_test_nc_buf1,
    };

    for (int i = 0; i < times; i++) {
        color = test_colors_RGBA8888[i % NCOLORS];
        hal_dma2d_clear(&buf, NULL, color);
        dma2d_wait_done();
        if (check_color(buf.data, color)) {
            ASSERT(0, "DMA2D fillcolor test failed!");
        } else {
            eshell_putstring("DMA2d fillcolor test %d succeed\r\n", i+1);
        }
    }

    hal_dma2d_close();
}

POSSIBLY_UNUSED
static void test_blit(int times)
{
    eshell_putstring("dma2d blit test start:\r\n");

    hal_dma2d_open(dma2d_handle);
    if (!dma2d_sem)
        dma2d_sem = osSemaphoreNew(1, 0, NULL);
    uint32_t color;

    struct DMA2D_BUFFER_T src = {
        .width = test_width,
        .height = test_height,
        .format = test_format,
        .data = frame_test_nc_buf0,
    };
    struct DMA2D_BUFFER_T dst = {
        .width = test_width,
        .height = test_height,
        .format = test_format,
        .data = frame_test_nc_buf1,
    };

    for (int i = 0; i < times; i++) {
        color = test_colors_RGBA8888[(i / 10) % NCOLORS];
        hal_dma2d_clear(&src, NULL, color);
        dma2d_wait_done();

        color = test_colors_RGBA8888[(i+1 / 10) % NCOLORS];
        hal_dma2d_clear(&dst, NULL, color);
        dma2d_wait_done();

        hal_dma2d_blit(&src, NULL, &dst, NULL, 100, 100);
        dma2d_wait_done();
        if (memcmp(src.data, dst.data, TEST_MAX_SIZE)) {
            ASSERT(0, "DMA2D blit test failed!");
        } else {
            eshell_putstring("DMA2d blit test %d succeed\r\n", i+1);
        }
    }

    hal_dma2d_close();
}

POSSIBLY_UNUSED
static void test_blend(int times)
{
    eshell_putstring("dma2d blend test:\r\n");

    hal_dma2d_open(dma2d_handle);
    if (!dma2d_sem)
        dma2d_sem = osSemaphoreNew(1, 0, NULL);
    uint32_t color;

    struct DMA2D_BUFFER_T src = {
        .width = test_width,
        .height = test_height,
        .format = test_format,
        .data = frame_test_nc_buf0,
    };
    struct DMA2D_BUFFER_T dst1 = {
        .width = test_width,
        .height = test_height,
        .format = test_format,
        .data = frame_test_nc_buf1,
    };
    struct DMA2D_BUFFER_T dst2 = {
        .width = test_width,
        .height = test_height,
        .format = test_format,
        .data = frame_test_nc_buf1 + TEST_MAX_SIZE,
    };

    for (int i = 0; i < times; i++) {
        memset(src.data, 0, TEST_MAX_SIZE);
        memset(dst1.data, 0, TEST_MAX_SIZE);
        memset(dst2.data, 0, TEST_MAX_SIZE);

        uint8_t opa = (25 - i % 25) * 10;

        color = test_colors_RGBA8888[(i / 10) % NCOLORS];
        hal_dma2d_clear(&src, NULL, color);
        dma2d_wait_done();

        color = test_colors_RGBA8888[(i+1 / 10) % NCOLORS];
        hal_dma2d_clear(&dst1, NULL, color);
        dma2d_wait_done();
        hal_dma2d_clear(&dst2, NULL, color);
        dma2d_wait_done();

        hal_dma2d_blend(&src, NULL, &dst1, NULL, DMA2D_BLEND_CONST_ALPHA, opa, 100, 100);
        dma2d_wait_done();

        /* do sw blending, compare with DMA2d */
        sw_blend_buffer(&src, &dst2, color, opa);
        if (compare_blend_buffer(dst1.data, dst2.data)) {
            ASSERT(0, "DMA2D blend test failed!");
        } else {
            eshell_putstring("DMA2d blend test %d succeed\r\n", i+1);
        }
    }

    hal_dma2d_close();
}

static void unitest_dma2d_usage(void)
{
    eshell_putstring("Usage:\r\n");
    eshell_putstring("  utest_dma2d fillcolor [times]\r\n");
    eshell_putstring("  utest_dma2d blit [times]\r\n");
    eshell_putstring("  utest_dma2d blend [times]\r\n");
}

static void unitest_dma2d(int argc, char *argv[])
{
    uint32_t times = 10;

    if (argc < 2)
        goto usage;

    if (argc > 2)
        times = atoi(argv[2]);

    if (strncmp(argv[1], "fillcolor", 9) == 0) {
        test_fillcolor(times);

    } else if (strncmp(argv[1], "blit", 4) == 0) {
        test_blit(times);

    } else if (strncmp(argv[1], "blend", 5) == 0) {
        test_blend(times);

    } else {
        goto usage;
    }

    return;

usage:
    unitest_dma2d_usage();
}

ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_UNIT_TEST, "utest_dma2d", "usage: utest_dma2d help",
                   unitest_dma2d);
#endif
