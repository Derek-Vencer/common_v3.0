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

#if defined(UTILS_ESHELL_EN) && defined(__SYS_AS_MAIN__) && defined(TEST_WITH_GPU)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hal_uart.h"
#include "hal_trace.h"
#include "hal_sysfreq.h"
#include "hal_location.h"
#include "hal_timer.h"
#include "vg_lite.h"
#include "gpu_port.h"

#include "ut_test.h"

#define TEST_WIDTH      96
#define TEST_HEIGHT     96

#define TEST_BUF_SIZE   (TEST_WIDTH * TEST_HEIGHT)

#ifdef PSRAM_ENABLE
static volatile uint32_t gpu_vg_buf[TEST_BUF_SIZE] ALIGNED(64) PSRAM_NC_BSS_LOC;
#else
static volatile uint32_t gpu_vg_buf[TEST_BUF_SIZE] ALIGNED(64);
#endif

#ifdef PSRAM_ENABLE
static volatile uint32_t cpu_vg_buf[TEST_BUF_SIZE] ALIGNED(64) PSRAM_NC_BSS_LOC;
#else
static volatile uint32_t cpu_vg_buf[TEST_BUF_SIZE] ALIGNED(64);
#endif

static vg_lite_buffer_t vg_buf;

static char *error_type[] =
{
    "VG_LITE_SUCCESS",
    "VG_LITE_INVALID_ARGUMENT",
    "VG_LITE_OUT_OF_MEMORY",
    "VG_LITE_NO_CONTEXT",
    "VG_LITE_TIMEOUT",
    "VG_LITE_OUT_OF_RESOURCES",
    "VG_LITE_GENERIC_IO",
    "VG_LITE_NOT_SUPPORT",
};
#define IS_ERROR(status)         (status > 0)
#define CHECK_ERROR(Function) \
    error = Function; \
    if (IS_ERROR(error)) \
    { \
        UT_LOG("[%s: %d] failed, error type is %s\n", \
                __func__, __LINE__,error_type[error]); \
    }

static void gpu_test_init(void)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;

    CHECK_ERROR(vg_lite_init(TEST_WIDTH, TEST_HEIGHT));

    vg_buf.format = VG_LITE_RGBA8888;
    vg_buf.tiled = VG_LITE_LINEAR;
    vg_buf.image_mode = VG_LITE_NORMAL_IMAGE_MODE;
    vg_buf.width = (int32_t) TEST_WIDTH;
    vg_buf.height = (int32_t) TEST_HEIGHT;
    vg_buf.stride = (int32_t) (TEST_WIDTH*4);
    vg_buf.memory = (void *)gpu_vg_buf;
    vg_buf.address = (uint32_t)gpu_vg_buf;
    vg_buf.handle = NULL;
}

static void gpu_test_exit(void)
{
    vg_lite_close();
}

static void gpu_test_draw(uint32_t color1, uint32_t color2, uint32_t bgcolor)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_matrix_t matrix;

    int8_t rect_path_data[] = {
        2, -36, -36, //move to
        4, 36, -36,  //line to
        4, 36, 36,   //line to
        4, -36, 36,  //line to
        0,
    };
    vg_lite_path_t path_rect = {
        {-36, -36,          // left,top
         36, 36},           // right,bottom
        VG_LITE_HIGH,        // quality
        VG_LITE_S8,          // -128 to 127 coordinate range
        {0},                 // uploaded
        sizeof(rect_path_data),   // path length
        rect_path_data,           // path data
        1};

    int8_t blend_path_data[] = {
        2, -24, -24, //move to
        4, 24, -24,  //line to
        4, 24, 24,   //line to
        4, -24, 24,  //line to
        0,
    };
    vg_lite_path_t blend_rect = {
        {-24, -24,          // left,top
         24, 24},           // right,bottom
        VG_LITE_HIGH,        // quality
        VG_LITE_S8,          // -128 to 127 coordinate range
        {0},                 // uploaded
        sizeof(blend_path_data),   // path length
        blend_path_data,           // path data
        1};

    vg_lite_identity(&matrix);

    vg_lite_translate(TEST_WIDTH / 2.0f, TEST_HEIGHT / 2.0f, &matrix);

    vg_lite_clear(&vg_buf, NULL, bgcolor);

    CHECK_ERROR(vg_lite_draw(&vg_buf, &path_rect,
                VG_LITE_FILL_EVEN_ODD, &matrix,
                VG_LITE_BLEND_NONE, color1));

    CHECK_ERROR(vg_lite_finish());

    CHECK_ERROR(vg_lite_draw(&vg_buf, &blend_rect,
                VG_LITE_FILL_EVEN_ODD, &matrix,
                VG_LITE_BLEND_SRC_OVER, color2));

    CHECK_ERROR(vg_lite_finish());
}

static void cpu_test_draw(uint32_t color1, uint32_t color2, uint32_t bgcolor)
{
    for (int i = 0; i < TEST_BUF_SIZE; ++i)
        cpu_vg_buf[i] = bgcolor;

    uint32_t y_offset, x_offset, ret;
    uint32_t *dst;
    uint32_t *t;
    uint8_t *pixel_ptr;
    uint32_t Sr, Sg, Sb, Sa;
    uint32_t Dr, Dg, Db, Da;

    // draw rect_path_data
    y_offset = (TEST_HEIGHT - 36 * 2) / 2;
    x_offset = (TEST_WIDTH - 36 * 2) / 2;
    dst = (uint32_t *)cpu_vg_buf + TEST_HEIGHT * y_offset;
    for (int r = 0; r < 36 * 2; ++r) {
        t = dst + x_offset;
        for (int c = 0; c < 36 * 2; ++c, ++t) {
            *t = color1;
        }
        dst += TEST_WIDTH;
    }

    // draw blend_path_data
    y_offset += 36 - 24;
    x_offset += 36 - 24;
    dst = (uint32_t *)cpu_vg_buf + TEST_HEIGHT * y_offset;
    for (int r = 0; r < 24 * 2; ++r) {
        t = dst + x_offset;
        for (int c = 0; c < 24 * 2; ++c, ++t) {
            Sr = color2 & 0xff;
            Sg = (color2 >> 8) & 0xff;
            Sb = (color2 >> 16) & 0xff;
            Sa = (color2 >> 24) & 0xff;

            Dr = *t & 0xff;
            Dg = (*t >> 8) & 0xff;
            Db = (*t >> 16) & 0xff;
            Da = (*t >> 24) & 0xff;

            // SRC_OVER: S + D * (1 - Sa)
            pixel_ptr = (uint8_t *)t;
            pixel_ptr[0] = (ret = (Sr + ((Dr * (255 - Sa)) / 255))) > 255 ? 255 : ret;
            pixel_ptr[1] = (ret = (Sg + ((Dg * (255 - Sa)) / 255))) > 255 ? 255 : ret;
            pixel_ptr[2] = (ret = (Sb + ((Db * (255 - Sa)) / 255))) > 255 ? 255 : ret;
            pixel_ptr[3] = (ret = (Sa + ((Da * (255 - Sa)) / 255))) > 255 ? 255 : ret;
        }
        dst += TEST_WIDTH;
    }
}

int gpu_blend_verify_test(int test_times)
{
    int ret = 0;

    gpu_test_init();

    for (int t = 1; t <= test_times; t++) {
        UT_LOG("GPU blend test %d ... start", t);

        gpu_test_draw(0x801aff3b, 0x57e8f1ff, 0xffffffff);
        cpu_test_draw(0x801aff3b, 0x57e8f1ff, 0xffffffff);

        /* compare buffer data */
        for (uint32_t i = 0; i < TEST_BUF_SIZE; i++) {
            if (gpu_vg_buf[i] != cpu_vg_buf[i]) {
                UT_ERR("ERROR %5d: expect 0x%x, got 0x%x", i,
                        cpu_vg_buf[i],
                        gpu_vg_buf[i]);
                ret = -1;
                goto exit;
            }
        }

        UT_LOG("GPU blend test %d ... OKAY", t);
    };

exit:
    gpu_test_exit();
    return ret;
}

static void unitest_blend_usage(void)
{
    eshell_putstring("Usage:\r\n");
    eshell_putstring("  utest_blend [times]\r\n");
}

static void unitest_blend(int argc, char *argv[])
{
    int times = 10;

    if (argc > 1) {
        if (strncmp(argv[1], "help", 4) == 0) {
            unitest_blend_usage();
            return;
        }

        times = atoi(argv[1]);
    }

    gpu_blend_verify_test(times);
}
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_UNIT_TEST, "utest_blend", "usage: utest_blend help",
                   unitest_blend);

#endif
