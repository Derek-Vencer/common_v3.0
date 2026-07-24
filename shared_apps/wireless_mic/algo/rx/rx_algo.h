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
#ifndef __RX_ALGO__
#define __RX_ALGO__

// #ifdef __cplusplus
// extern "C" {
// #endif

#ifndef MAX_INPUT_CH_NUM
#define MAX_INPUT_CH_NUM    (2)
#endif

#include "sw_gain.h"
#include "audio_channel_merge.h"
#include "multipoint_drc.h"
#include "audio_datapath_manager.h"

extern const MultiPointDrcConfig rx_algo_mpdrc_config[AUDIO_OUTPUT_PATH_NUM];
extern const AudioChannelMergeConfig channel_merge_config_mono;
extern const AudioChannelMergeConfig channel_merge_config_stereo;

void rx_algo_init(uint8_t datapath, int32_t sample_rate, int32_t frame_size, int32_t ch_num);

void rx_algo_deinit(uint8_t datapath);

void rx_algo_process(uint8_t datapath, uint8_t* dest, uint8_t* src, uint32_t src_bytes);

uint8_t rx_algo_merge_channel_mode_get(void);

void rx_algo_merge_channel_mode_set(uint8_t datapath, uint8_t mode);

bool rx_algo_mdrc_switch_get(uint8_t datapath);

void rx_algo_mdrc_switch_set(uint8_t datapath, bool onoff);

float rx_algo_sw_gain_get(uint8_t datapath);

void rx_algo_sw_gain_set(uint8_t datapath, float gain);

// #ifdef __cplusplus
// }
// #endif

#endif