
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
#ifndef __PRESSURE_DRIVER_BEST1502P_H__
#define __PRESSURE_DRIVER_BEST1502P_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "pressure_driver.h"

void pressure_sensor_init(struct PRESSURE_CFG_T * press_cfg);
void pressure_sensor_irq_enable(bool en);
void pressure_sensor_irq_clr(void);
int pressure_sensor_irq_get_active(void);
void pressure_sensor_adc_raw_get(struct pressure_sample_data *sample, uint8_t num);

#ifdef __cplusplus
}
#endif
#endif // __PRESSURE_DRIVER_BEST1502P_H__
