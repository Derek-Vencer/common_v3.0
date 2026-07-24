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
#include "cmsis.h"
#include "pressure_driver.h"
#include "hal_trace.h"
#include "cmsis_os.h"
#include CHIP_SPECIFIC_HDR(pressure_driver)

static struct PRESSURE_CFG_T pressure_cfg = {0};

/***************************************************************************
 * @brief read pressure data function
 *
 ***************************************************************************/
void pressure_read_curr_data(struct pressure_sample_data *sample, uint8_t num)
{
    pressure_sensor_adc_raw_get(sample, num);
}

/***************************************************************************
 * @brief pressure driver start function
 ***************************************************************************
 ***************************************************************************
***     wait_time+num_sel                                               ****
***     ____________                                                    ****
***     |          |                                                    ****
***     |          |                                                    ****
***     |          |                                                    ****
***_____|          |____________________________________________________****
***                |<--done_time-->|                                    ****
***     |<<<------------------------det_period---------------------->>>|****
****************************************************************************
****************************************************************************
 ***************************************************************************/
void pressure_drv_start(void)
{
    /*pressure config set*/
    pressure_cfg.num_sel = 0x4; //0x4 -> 16num , 16*1/100KHZ = 160us
    pressure_cfg.done_time = 0x8; //8*256 , 8/1MKHZ * 256 = 2048us
    pressure_cfg.det_period = 0xa; // 1/1KHZ * 10 = 10ms
    pressure_cfg.wait_time = 0xf5;  // 1/1MKHZ * 0xf5 = 245us

    pressure_sensor_init(&pressure_cfg);
}
