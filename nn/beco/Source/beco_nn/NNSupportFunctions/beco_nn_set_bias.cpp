/***************************************************************************
 *
 * Copyright 2015-2025 BES.
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

#include "beco_nn/beco_nnsupportfunctions.h"
#include "beco_bias.hpp"

void beco_fully_connected_set_bias(const beco_vec64_in_t *bias)
{
    beco_bias_t load;
    const int32_t *cur_bias = (const int32_t *)bias;
    beco_calc_acc_preload_16x8(cur_bias[0], cur_bias[1], 0, 0, &load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC0_0, load);
    beco_calc_acc_preload_16x8(cur_bias[2], cur_bias[3], 0, 0, &load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC0_4, load);
    beco_calc_acc_preload_16x8(cur_bias[4], cur_bias[5], 0, 0, &load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC0_8, load);
    beco_calc_acc_preload_16x8(cur_bias[6], cur_bias[7], 0, 0, &load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC0_12, load);
    cur_bias += 8;

    beco_calc_acc_preload_16x8(cur_bias[0], cur_bias[1], 0, 0, &load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC1_0, load);
    beco_calc_acc_preload_16x8(cur_bias[2], cur_bias[3], 0, 0, &load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC1_4, load);
    beco_calc_acc_preload_16x8(cur_bias[4], cur_bias[5], 0, 0, &load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC1_8, load);
    beco_calc_acc_preload_16x8(cur_bias[6], cur_bias[7], 0, 0, &load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC1_12, load);
    cur_bias += 8;

    beco_calc_acc_preload_16x8(cur_bias[0], cur_bias[1], 0, 0, &load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC2_0, load);
    beco_calc_acc_preload_16x8(cur_bias[2], cur_bias[3], 0, 0, &load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC2_4, load);
    beco_calc_acc_preload_16x8(cur_bias[4], cur_bias[5], 0, 0, &load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC2_8, load);
    beco_calc_acc_preload_16x8(cur_bias[6], cur_bias[7], 0, 0, &load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC2_12, load);
    cur_bias += 8;

    beco_calc_acc_preload_16x8(cur_bias[0], cur_bias[1], 0, 0, &load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC3_0, load);
    beco_calc_acc_preload_16x8(cur_bias[2], cur_bias[3], 0, 0, &load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC3_4, load);
    beco_calc_acc_preload_16x8(cur_bias[4], cur_bias[5], 0, 0, &load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC3_8, load);
    beco_calc_acc_preload_16x8(cur_bias[6], cur_bias[7], 0, 0, &load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC3_12, load);
}

void beco_convolve_q7_set_bias(const beco_vec64_in_t *beco_bias)
{
    beco_write_reg(BECO_REG0, *beco_bias++);
    beco_write_reg(BECO_REG1, *beco_bias++);
    beco_write_reg(BECO_REG2, *beco_bias++);
    beco_write_reg(BECO_REG3, *beco_bias++);

    beco_set_acc_bias(BECO_ACC0_0, BECO_REG0, BECO_REG0);
    beco_set_acc_bias(BECO_ACC0_4, BECO_REG0, BECO_REG0);
    beco_set_acc_bias(BECO_ACC0_8, BECO_REG1, BECO_REG1);
    beco_set_acc_bias(BECO_ACC0_12, BECO_REG1, BECO_REG1);

    beco_set_acc_bias(BECO_ACC1_0, BECO_REG2, BECO_REG2);
    beco_set_acc_bias(BECO_ACC1_4, BECO_REG2, BECO_REG2);
    beco_set_acc_bias(BECO_ACC1_8, BECO_REG3, BECO_REG3);
    beco_set_acc_bias(BECO_ACC1_12, BECO_REG3, BECO_REG3);

    beco_set_acc_bias(BECO_ACC2_0, BECO_REG0, BECO_REG0);
    beco_set_acc_bias(BECO_ACC2_4, BECO_REG0, BECO_REG0);
    beco_set_acc_bias(BECO_ACC2_8, BECO_REG1, BECO_REG1);
    beco_set_acc_bias(BECO_ACC2_12, BECO_REG1, BECO_REG1);

    beco_set_acc_bias(BECO_ACC3_0, BECO_REG2, BECO_REG2);
    beco_set_acc_bias(BECO_ACC3_4, BECO_REG2, BECO_REG2);
    beco_set_acc_bias(BECO_ACC3_8, BECO_REG3, BECO_REG3);
    beco_set_acc_bias(BECO_ACC3_12, BECO_REG3, BECO_REG3);
}

void beco_convolve_q15_set_bias(const beco_vec64_in_t *beco_bias)
{
    beco_bias_t load;
    const int32_t *cur_bias = (const int32_t *)beco_bias;
    beco_calc_acc_preload_16x8(cur_bias[0], cur_bias[1], 0, 0, &load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC0_0, load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC0_4, load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC2_0, load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC2_4, load);
    beco_calc_acc_preload_16x8(cur_bias[2], cur_bias[3], 0, 0, &load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC0_8, load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC0_12, load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC2_8, load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC2_12, load);
    beco_calc_acc_preload_16x8(cur_bias[4], cur_bias[5], 0, 0, &load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC1_0, load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC1_4, load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC3_0, load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC3_4, load);
    beco_calc_acc_preload_16x8(cur_bias[6], cur_bias[7], 0, 0, &load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC1_8, load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC1_12, load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC3_8, load);
    BECO_LOAD_BIAS_ACC_MU(BECO_ACC3_12, load);
}
