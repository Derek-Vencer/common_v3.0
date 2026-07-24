
/***************************************************************************
 *
 * Copyright 2024-2024 BES.
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
#ifndef __PRESSURE_DRIVER_H__
#define __PRESSURE_DRIVER_H__
#ifdef __cplusplus
extern "C" {
#endif

struct PRESSURE_CFG_T{
    uint8_t num_sel;      //reg_psensor_adc_num_sel[15:13]
    uint8_t done_time;    //reg_done_time_ch0[15:12]
    uint16_t det_period;  //reg_det_period[15:7]
    uint16_t wait_time;   //reg_wait_time_ch0[11:0]
};

struct pressure_sample_data {
    int32_t value;  //ch1
    int32_t value2; //ch2
};

void pressure_drv_start(void);
void pressure_read_curr_data(struct pressure_sample_data *sample, uint8_t num);

#ifdef __cplusplus
}
#endif
#endif // __PRESSURE_DRIVER_H__
