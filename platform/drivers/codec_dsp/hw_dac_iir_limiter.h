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
#ifndef __HW_DAC_LIMITER_H__
#define __HW_DAC_LIMITER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "hal_aud.h"

/*
iir coefficients(b/a Q27)
b2
b1
b0
a2
a1
*/

typedef struct {
    uint16_t   enable;
    uint16_t   delay_ms;

    float      thd;       //threshold
    float      att_ms;    //attack time
    float      rls_ms;    //release time
} hw_dac_limiter_param_t;

typedef struct {
    hw_dac_limiter_param_t limiter_param_ch0;
    hw_dac_limiter_param_t limiter_param_ch1;
} hw_dac_limiter_cfg_t;

int32_t hw_dac_limiter_open(int32_t ch_map);
int32_t hw_dac_limiter_close(void);
int32_t hw_dac_limiter_set_cfg(const hw_dac_limiter_cfg_t* cfg);
int32_t hw_dac_limiter_att_rls_coef_copy(int8_t coefa_using_flag);
int32_t hw_dac_limiter_set_ch(int32_t ch_map);
int32_t hw_dac_limiter_enable(void);
int32_t hw_dac_limiter_disable(void);
int32_t hw_dac_limiter_set_threhold(int32_t threhold_db);

#ifdef __cplusplus
}
#endif

#endif