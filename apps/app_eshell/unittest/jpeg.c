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

#if defined(UTILS_ESHELL_EN) && defined(JPEG_BASE)

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
#include "jpeg_drv.h"

#include "display_test.h"

#include "ut_test.h"

#define TEST_PIC_WIDTH 128
#define TEST_PIC_HEIGHT 128

POSSIBLY_UNUSED
static uint8_t yuv422_jpeg[] = {
#include "jpegdec/jpeg_yuv422_128x128.txt"
};
POSSIBLY_UNUSED
static uint8_t yuv444_jpeg[] = {
#include "jpegdec/jpeg_yuv444_128x128.txt"
};
POSSIBLY_UNUSED
static uint8_t yuv420_jpeg[] = {
#include "jpegdec/jpeg_yuv420_128x128.txt"
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

static void test_draw_jpeg(uint8_t *jpeg_data, uint32_t jpeg_size, int count)
{
    uint32_t frames = 0;
    uint32_t start;
    struct jpeg_handle_t handle;
    uint8_t *ecs_buf = frame_test_buf1;
    uint8_t *pix_buf = ((uint8_t *)frame_test_buf1 + TEST_PIC_WIDTH * TEST_PIC_HEIGHT);
    uint8_t *lcdc_buf = frame_test_nc_buf0;

    jpeg_init();
    test_start(lcdc_buf);

    memset(&handle, 0, sizeof(handle));

    handle.ecs_buf = ecs_buf;
    handle.ecs_bufsz = TEST_PIC_WIDTH * TEST_PIC_HEIGHT;
    handle.pix_data = pix_buf;
    handle.pix_size = TEST_PIC_WIDTH * TEST_PIC_HEIGHT * 4;
    handle.pix_fmt = PIX_YUV422_YUYV;

    while (frames < count) {
        UT_LOG("----------------------------------------------------------------------");
        UT_LOG("#### Frame %u start.", frames);

        UT_LOG("#### Frame %u jpeg decode start.", frames);
        start = hal_fast_sys_timer_get();

        handle.jpeg_size = jpeg_size;
        handle.jpeg_data = jpeg_data;

        if (jpeg_decode(&handle)) {
            UT_LOG("Failed to decode JPEG file.");
            break;
        }

        UT_LOG("#### Frame %u jpeg decode done, %uus.", frames,
                    FAST_TICKS_TO_US(hal_fast_sys_timer_get() - start));

        if (test_with_display) {
            UT_LOG("#### Frame %u gpu drawing start.", frames+1);
            start = hal_fast_sys_timer_get();

            gpu_test_fill_color(0);
            gpu_test_blit(handle.pix_data, handle.width, handle.height, VG_LITE_YUYV,
                      lcdc_buf, test_width, test_height, test_format, 2.0f, frames % 360);

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
    jpeg_exit();
}


static void unitest_jpeg_usage(void)
{
    eshell_putstring("Usage:\r\n");
    eshell_putstring("  utest_jpeg decode format times [display]\r\n");
    eshell_putstring("    format:  yuv422, yuv420, yuv444\r\n");
    eshell_putstring("    times:   > 1, test times\r\n");
    eshell_putstring("    display: 1 or 0, display the image on LCD panel.\r\n");
}

static void unitest_jpeg(int argc, char *argv[])
{
    int times = 10;

    if (argc < 4)
        goto usage;

    times = atoi(argv[3]);
    if (argc > 4)
        test_with_display = atoi(argv[4]);

    if (strncmp(argv[2], "yuv422", 6) == 0) {
        test_draw_jpeg(yuv422_jpeg, sizeof(yuv422_jpeg), times);
    } else if (strncmp(argv[2], "yuv420", 6) == 0) {
        test_draw_jpeg(yuv420_jpeg, sizeof(yuv422_jpeg), times);
    } else if (strncmp(argv[2], "yuv444", 6) == 0) {
        test_draw_jpeg(yuv444_jpeg, sizeof(yuv422_jpeg), times);
    } else {
        goto usage;
    }

    return;

usage:
    unitest_jpeg_usage();
}

ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_UNIT_TEST, "utest_jpeg", "usage: utest_jpeg help",
                   unitest_jpeg);
#endif
