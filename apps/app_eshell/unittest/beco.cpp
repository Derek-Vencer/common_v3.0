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
#if (defined(UTILS_ESHELL_EN) && defined(ENABLE_BECO))

#include "stdio.h"
#include "beco_types.h"
#include "beco.h"
#include "beco_nnfunctions.h"
#include "arm_nnfunctions.h"
#include "naive_utils.hpp"
#include "eshell.h"
#include "stdlib.h"
#include "string.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "dwt.h"

#define PIXSIZE 2 * sizeof(q15_t)
#define DUMP_RESULTS 0
#define ICH 8
#define IH  4
#define IW  60
#define OCH 64
#define OH  4
#define OW  60
#define PX  1
#define PY  1
#define SX  1
#define SY  1
#define KX  3
#define KY  3
#define DX  1
#define DY  1

static void beco_shift_block5_test()
{
    q15_t data_q15[20];
    beco_vec64_in_t *beco_in = (beco_vec64_in_t *)data_q15;
    for (int i = 0; i < 20; i++) { data_q15[i] = i - 10; }

    beco_write_reg(BECO_REG0, beco_in[0]);
    beco_write_reg(BECO_REG1, beco_in[1]);
    beco_write_reg(BECO_REG2, beco_in[2]);
    beco_write_reg(BECO_REG3, beco_in[3]);
    beco_write_reg(BECO_REG4, beco_in[4]);

    q15_t out_q15[20];
    beco_vec64_out_t *beco_out = (beco_vec64_out_t *)out_q15;

    beco_out[0] = beco_read_reg(BECO_REG0);
    beco_out[1] = beco_read_reg(BECO_REG1);
    beco_out[2] = beco_read_reg(BECO_REG2);
    beco_out[3] = beco_read_reg(BECO_REG3);
    beco_out[4] = beco_read_reg(BECO_REG4);

    eshell_putstring("===== befor shift =====\r\n");
    eshell_putstring("BECO_REG0 : %d %d %d %d\r\n", out_q15[0], out_q15[1], out_q15[2], out_q15[3]);
    eshell_putstring("BECO_REG1 : %d %d %d %d\r\n", out_q15[4], out_q15[5], out_q15[6], out_q15[7]);
    eshell_putstring("BECO_REG2 : %d %d %d %d\r\n", out_q15[8], out_q15[9], out_q15[10], out_q15[11]);
    eshell_putstring("BECO_REG3 : %d %d %d %d\r\n", out_q15[12], out_q15[13], out_q15[14], out_q15[15]);
    eshell_putstring("BECO_REG4 : %d %d %d %d\r\n", out_q15[16], out_q15[17], out_q15[18], out_q15[19]);

    beco_shift_block5(BECO_REG0, PIXSIZE);

    beco_out[0] = beco_read_reg(BECO_REG0);
    beco_out[1] = beco_read_reg(BECO_REG1);
    beco_out[2] = beco_read_reg(BECO_REG2);
    beco_out[3] = beco_read_reg(BECO_REG3);
    beco_out[4] = beco_read_reg(BECO_REG4);

    eshell_putstring("===== after shift =====\r\n");
    eshell_putstring("BECO_REG0 : %d %d %d %d\r\n", out_q15[0], out_q15[1], out_q15[2], out_q15[3]);
    eshell_putstring("BECO_REG1 : %d %d %d %d\r\n", out_q15[4], out_q15[5], out_q15[6], out_q15[7]);
    eshell_putstring("BECO_REG2 : %d %d %d %d\r\n", out_q15[8], out_q15[9], out_q15[10], out_q15[11]);
    eshell_putstring("BECO_REG3 : %d %d %d %d\r\n", out_q15[12], out_q15[13], out_q15[14], out_q15[15]);
    eshell_putstring("BECO_REG4 : %d %d %d %d\r\n", out_q15[16], out_q15[17], out_q15[18], out_q15[19]);
}

static void beco_set_add_bias_test()
{
    uint32_t beco_cfg = BECO_CONF_AMODE_REP32    | BECO_CONF_BMODE_REP32  |
                        BECO_CONF_ATYPE_INT16    | BECO_CONF_BTYPE_INT16  |
                        BECO_CONF_RSHIFT(0)      | BECO_CONF_RD_16x16 |
                        BECO_CONF_PACK_INT32;
    beco_write_config(beco_cfg);

    int32_t bias[4] = {-100000, -200, 300, 400};
    int32_t out[4] = {0};

    beco_write_reg(BECO_REG0, (beco_vec64_in_t){.i32 = {bias[0], 0}});
    beco_write_reg(BECO_REG1, (beco_vec64_in_t){.i32 = {bias[1], 0}});
    beco_write_reg(BECO_REG2, (beco_vec64_in_t){.i32 = {bias[2], 0}});
    beco_write_reg(BECO_REG3, (beco_vec64_in_t){.i32 = {bias[3], 0}});

    beco_set_acc_bias(BECO_ACC0_0, BECO_REG0, BECO_REG7);
    beco_set_acc_bias(BECO_ACC0_4, BECO_REG1, BECO_REG7);
    beco_set_acc_bias(BECO_ACC0_8, BECO_REG2, BECO_REG7);
    beco_set_acc_bias(BECO_ACC0_12, BECO_REG3, BECO_REG7);

    out[0] = beco_read_acc(BECO_ACC0, 0).i32;
    out[1] = beco_read_acc(BECO_ACC0, 4).i32;
    out[2] = beco_read_acc(BECO_ACC0, 8).i32;
    out[3] = beco_read_acc(BECO_ACC0, 12).i32;

    eshell_putstring("===>beco_set_add_bias_test out:\r\n");
    eshell_putstring("%d %d %d %d\r\n", out[0], out[1], out[2], out[3]);
}

static void beco_mmacrr_test()
{
    uint32_t config =
        BECO_CONF_AMODE_REP32 | BECO_CONF_BMODE_REP32 |
        BECO_CONF_ATYPE_INT16 | BECO_CONF_BTYPE_INT16 |
        BECO_CONF_RSHIFT(0)   |
        BECO_CONF_PACK_INT16  | BECO_CONF_RD_16x16;
    beco_write_config(config);

    q15_t image[] = { -1, -2, 3, 4};
    q15_t ker[] = { 10, 20, -30, -40};
    q15_t out[16];

    beco_write_reg(BECO_REG0, (beco_vec64_in_t){.i16 = {image[0], image[1], image[2], image[3]}});
    beco_write_reg(BECO_REG1, (beco_vec64_in_t){.i16 = {ker[0], ker[1], ker[2], ker[3]}});
    beco_clear_acc(BECO_ACC0);
    beco_clear_acc(BECO_ACC1);
    beco_clear_acc(BECO_ACC2);
    beco_clear_acc(BECO_ACC3);
    beco_mmacrr(BECO_REG0, BECO_REG1);

    out[0] = beco_read_acc(BECO_ACC0, 0).i16[0];
    out[1] = beco_read_acc(BECO_ACC0, 4).i16[0];
    out[2] = beco_read_acc(BECO_ACC0, 8).i16[0];
    out[3] = beco_read_acc(BECO_ACC0, 12).i16[0];
    out[4] = beco_read_acc(BECO_ACC1, 0).i16[0];
    out[5] = beco_read_acc(BECO_ACC1, 4).i16[0];
    out[6] = beco_read_acc(BECO_ACC1, 8).i16[0];
    out[7] = beco_read_acc(BECO_ACC1, 12).i16[0];
    out[8] = beco_read_acc(BECO_ACC2, 0).i16[0];
    out[9] = beco_read_acc(BECO_ACC2, 4).i16[0];
    out[10] = beco_read_acc(BECO_ACC2, 8).i16[0];
    out[11] = beco_read_acc(BECO_ACC2, 12).i16[0];
    out[12] = beco_read_acc(BECO_ACC3, 0).i16[0];
    out[13] = beco_read_acc(BECO_ACC3, 4).i16[0];
    out[14] = beco_read_acc(BECO_ACC3, 8).i16[0];
    out[15] = beco_read_acc(BECO_ACC3, 12).i16[0];

    eshell_putstring("===>test_beco_mmacrr out:\r\n");
    eshell_putstring("acc0 = %d %d %d %d\r\n", out[0], out[1], out[2], out[3]);
    eshell_putstring("acc1 = %d %d %d %d\r\n", out[4], out[5], out[6], out[7]);
    eshell_putstring("acc2 = %d %d %d %d\r\n", out[8], out[9], out[10], out[11]);
    eshell_putstring("acc3 = %d %d %d %d\r\n", out[12], out[13], out[14], out[15]);
}

static void beco_mmacgr_test()
{
    uint32_t config = BECO_CONF_AMODE_REP16    | BECO_CONF_BMODE_REP64  |
                      BECO_CONF_ATYPE_INT8     | BECO_CONF_BTYPE_INT16  |
                      BECO_CONF_RSHIFT(0)      | BECO_CONF_RD_8x16 |
                      BECO_CONF_PACK_INT16;
    beco_write_config(config);

    q15_t image[] = { -1, -2, 3, 4};
    q7_t ker[] = { -100 };
    q15_t out[4];
    beco_clear_acc(BECO_ACC0);
    beco_write_reg(BECO_REG0, (beco_vec64_in_t){.i16 = {image[0], image[1], image[2], image[3]}});
    beco_mmacgr(BECO_ACC0, ((beco_vec32_in_t){.i8 = {ker[0]}}), BECO_REG0);

    out[0] = beco_read_acc(BECO_ACC0, 0).i16[0];
    out[1] = beco_read_acc(BECO_ACC0, 4).i16[0];
    out[2] = beco_read_acc(BECO_ACC0, 8).i16[0];
    out[3] = beco_read_acc(BECO_ACC0, 12).i16[0];

    eshell_putstring("===>test_beco_mmacgr out:\r\n");
    eshell_putstring("%d %d %d %d\r\n", out[0], out[1], out[2], out[3]);
}

static void beco_api_test(void)
{
    BECO_INIT();
    eshell_putstring("===== beco_api_test START =====\r\n");
    beco_shift_block5_test();
    beco_set_add_bias_test();
    beco_mmacrr_test();
    beco_mmacgr_test();
    eshell_putstring("===== beco_api_test END =====\r\n");
    BECO_EXIT(0);
}

static void add_input_offset_2_bias(int32_t *bias, int8_t *weight, int input_offset,
                                    int input_ch, int output_ch, int kernel_h, int kernel_w)
{
    for (int i = 0; i < output_ch; i++) {
        int sum = 0;
        for (int j = 0; j < input_ch; j++) {
            for (int k = 0; k < kernel_h; k++) {
                for (int l = 0; l < kernel_w; l++) {
                    sum += weight[j*kernel_h*kernel_w*output_ch + k*kernel_w*output_ch + l*output_ch + i] * input_offset;
                }
            }
        }
        bias[i] += sum;
    }
}

static void compare_convolve_s8(void)
{
    BECO_INIT();

    int8_t Im_in[ICH*IH*IW];
    int8_t wt[ICH*KX*KY*OCH];
    int32_t bias_beco[OCH];
    int32_t bias_cmsis[OCH];
    int32_t multiplier[OCH];
    int32_t shift[OCH];

    // Create test pattern
    for (int i = 0; i < ICH*IH*IW; i++) { Im_in[i] = i % 100 - 50; }
    for (int i = 0; i < ICH*KX*KY*OCH; i++) { wt[i] = i % 100 - 50; }
    for (int i = 0; i < OCH; i++) {
        multiplier[i] = NN_Q31_MAX;
        shift[i] = -8;
        bias_beco[i] = 500 * (i % 50 - 25);
        bias_cmsis[i] = 500 * (i % 50 - 25);
    }

    // Init Conv params
    cmsis_nn_context ctx;
    cmsis_nn_conv_params conv_params;
    cmsis_nn_per_channel_quant_params quant_params;
    cmsis_nn_dims input_dims;
    cmsis_nn_dims filter_dims;
    cmsis_nn_dims bias_dims;
    cmsis_nn_dims output_dims;

    input_dims.n = 1;
    input_dims.w = IW;
    input_dims.h = IH;
    input_dims.c = ICH;
    filter_dims.w = KX;
    filter_dims.h = KY;
    filter_dims.c = ICH;
    output_dims.w = OW;
    output_dims.h = OH;
    output_dims.c = OCH;

    conv_params.padding.w = PX;
    conv_params.padding.h = PY;
    conv_params.stride.w = SX;
    conv_params.stride.h = SY;
    conv_params.dilation.w = DX;
    conv_params.dilation.h = DY;

    conv_params.input_offset = 10;
    conv_params.output_offset = 20;
    conv_params.activation.min = NN_Q7_MIN;
    conv_params.activation.max = NN_Q7_MAX;
    quant_params.multiplier = multiplier;
    quant_params.shift = shift;

    if (conv_params.input_offset != 0) {
        add_input_offset_2_bias(bias_beco, wt, conv_params.input_offset, ICH, OCH, KY, KX);
    }

    uint32_t s_time, e_time, n_cycle, lock;
    int8_t out_beco[OCH*OH*OW];
    /*-------------------------- beco_nn --------------------------*/
    int32_t buf_size = beco_convolve_s8_get_buffer_size(
        &input_dims, &output_dims, &conv_params, &filter_dims);
    ctx.buf = malloc(buf_size);
    dwt_enable();

    TRACE_FLUSH();
    hal_sys_timer_delay(16);
    s_time = hal_fast_sys_timer_get();
    lock = int_lock();
    dwt_reset_cycle_cnt();

    beco_convolve_s8(&ctx, &conv_params, &quant_params, &input_dims, Im_in, &filter_dims, wt,
                     &bias_dims, bias_beco, &output_dims, out_beco);

    n_cycle = dwt_read_cycle_cnt();
    int_unlock(lock);
    e_time = hal_fast_sys_timer_get();
    TRACE_FLUSH();
    hal_sys_timer_delay(16);

    eshell_putstring("[%s] cost %dus cycle: %d at sysfreq %d\r\n", "beco_convolve_s8",
                     FAST_TICKS_TO_US(e_time - s_time), n_cycle, hal_sys_timer_calc_cpu_freq(5, 0));

    free(ctx.buf);

    /*-------------------------- cmsis --------------------------*/
    int8_t out_cmsis[OCH*OH*OW];
    CMSIS_Input_Transform(Im_in, wt, ICH, IW, IH, OCH, KX, KY);

    buf_size = arm_convolve_s8_get_buffer_size(&input_dims, &filter_dims);
    ctx.buf = malloc(buf_size);
    ctx.size = 0;
    dwt_enable();

    TRACE_FLUSH();
    hal_sys_timer_delay(16);
    s_time = hal_fast_sys_timer_get();
    lock = int_lock();
    dwt_reset_cycle_cnt();

    arm_convolve_s8(&ctx, &conv_params, &quant_params, &input_dims, Im_in, &filter_dims, wt,
                    &bias_dims, bias_cmsis, &output_dims, out_cmsis);

    n_cycle = dwt_read_cycle_cnt();
    int_unlock(lock);
    e_time = hal_fast_sys_timer_get();
    TRACE_FLUSH();
    hal_sys_timer_delay(16);

    eshell_putstring("[%s] cost %dus cycle: %d at sysfreq %d\r\n", "arm_convolve_s8",
                     FAST_TICKS_TO_US(e_time - s_time), n_cycle, hal_sys_timer_calc_cpu_freq(5, 0));
    free(ctx.buf);
    CMSIS_Output_Transform(out_cmsis, OCH, OW, OH);

#if DUMP_RESULTS
    eshell_putstring("beco results:\r\n");
    for (int i = 0; i < OCH; i++) {
        DUMP8("%d ", &out_beco[i * OH * OW], OH * OW);
    }

    eshell_putstring("cmsis results:\r\n");
    for (int i = 0; i < OCH; i++) {
        DUMP8("%d ", &out_cmsis[i * OH * OW], OH * OW);
    }
#endif

    // Compare reference to beco
    bool match = true;
    for (int i = 0; i < OCH*OH*OW; i++) {
        if (abs(out_beco[i] - out_cmsis[i]) > 1) {
            match = false;
            break;
        }
    }
    if (match) {
        eshell_putstring("Beco result MATCH Reference\r\n");
    }
    else {
        eshell_putstring("Beco result doesn't match Reference\r\n");
    }

    BECO_EXIT(0);
}

static void unitest_beco(int argc, char *argv[])
{
    if (argc != 2) {
        eshell_putstring("ERROR_ARG\r\n");
        eshell_putstring("usage: utest_beco api_test/convolve_test\r\n");
        return;
    }

    if (strncmp(argv[1], "api_test", 8) == 0) {
        beco_api_test();

    } else if (strncmp(argv[1], "convolve_test", 13) == 0) {
        compare_convolve_s8();

    } else {
        eshell_putstring("usage: utest_beco api_test/convolve_test\r\n");
    }
}

ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_UNIT_TEST, "utest_beco", "usage: utest_beco help",
                   unitest_beco);
#endif
