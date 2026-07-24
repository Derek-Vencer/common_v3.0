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
#ifndef __ANALOG_BEST1307P_H__
#define __ANALOG_BEST1307P_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ISPI_ANA_REG(r)                     (((r) & 0xFFF) | 0x1000)

#define MAX_ANA_MIC_CH_NUM                  3

void analog_aud_pll_set_dig_div(uint32_t div);

uint32_t analog_aud_get_max_dre_gain(void);

void analog_aud_codec_anc_boost(bool en);

int analog_debug_config_vad_mic(bool enable);

void analog_aud_vad_enable(enum AUD_VAD_TYPE_T type, bool en);

void analog_aud_vad_adc_enable(bool en);

void analog_aud_classd_pa_enable(bool en);

void analog_aud_adc_mic_sel(enum AUD_CHANNEL_MAP_T mic_map);

void analog_aud_dre_dc_sel(bool en);

uint16_t analog_aud_dc_diff_to_val(int32_t val, int32_t val_offset[]);

void analog_aud_dc_calib_set_dre_ana_dc(uint32_t offs, uint16_t dc_offs_l, uint16_t dc_offs_r);

void analog_aud_adc_dc_auto_calib_enable(enum AUD_CHANNEL_MAP_T ch_map, bool on);

void analog_aud_adc_dc_calib_set_step(enum AUD_CHANNEL_MAP_T ch_map, uint16_t step);

void analog_aud_adc_dc_calib_offset_update_enable(enum AUD_CHANNEL_MAP_T ch_map, bool on);

void analog_aud_adc_dc_calib_offset_update(enum AUD_CHANNEL_MAP_T ch_map, uint16_t value);

void analog_aud_codec_enable_capsensor_rc(bool en);

void analog_capsensor_open(void);

void analog_capsensor_reset_set(void);

void analog_capsensor_reset_clear(void);

void analog_capsensor_clk_gate_on(void);

void analog_capsensor_clk_gate_off(void);

uint16_t analog_capsensor_clk_is_ready(void);

#ifdef __cplusplus
}
#endif

#endif

