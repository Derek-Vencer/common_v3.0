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
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "analog.h"
#include "plat_addr_map.h"
#include CHIP_SPECIFIC_HDR(pressure_driver)
#include "hal_timer.h"
#include "audioflinger.h"
#include "hal_aud.h"
#include "codec_int.h"
#include "string.h"
#include "hal_trace.h"
#include "hal_codec.h"
#include "tgt_hardware.h"

static int pressure_adca_data_get(bool on)
{
    static bool isRun = false;

    if (isRun == on)
        return 0;

    if (on) {
        codec_pressure_open(AUD_SAMPRATE_8000);
    } else {
        codec_pressure_close();
    }
    isRun = on;

    return 0;
}

int pressure_sensor_irq_get_active(void)
{
    return 0;
}

void pressure_sensor_irq_clr(void)
{
}

void pressure_sensor_irq_enable(bool en)
{
}

void pressure_sensor_adc_raw_get(struct pressure_sample_data *sample, uint8_t num)
{
    int raw_data, raw_data2;
    af_lock_thread();
    pressure_adca_data_get(true);
    af_unlock_thread();
    osDelay(2);

    for(uint8_t i=0; i < num; i++) {
        raw_data = hal_codec_adc_dc_dout_ch3_data();
        raw_data2 = hal_codec_adc_dc_dout_ch4_data();

        raw_data = raw_data << 11;
        raw_data = raw_data >> 11;
        if(raw_data & (1<<20)) {
            sample[i].value = 0 - raw_data;
        } else {
            sample[i].value = raw_data;
        }

        raw_data2 = raw_data2 << 11;
        raw_data2 = raw_data2 >> 11;
        if(raw_data2 & (1<<20)) {
            sample[i].value2 = 0 - raw_data2;
        } else {
            sample[i].value2 = raw_data2;
        }
        // DRIVERS_TRACE(0, "pressure value:%x value2:%x ", sample[i].value, sample[i].value2);
    }

    af_lock_thread();
    pressure_adca_data_get(false);
    af_unlock_thread();
}

void pressure_sensor_adc_raw_previous_get(struct pressure_sample_data *sample, uint8_t num)
{
}

void pressure_sensor_init(struct PRESSURE_CFG_T * press_cfg)
{
    analog_aud_adc_force_precharge_enable(PRESS_MIC_CH, false);
}

void pressure_sensor_deinit(void)
{
    analog_aud_adc_force_precharge_disable(PRESS_MIC_CH);
}
