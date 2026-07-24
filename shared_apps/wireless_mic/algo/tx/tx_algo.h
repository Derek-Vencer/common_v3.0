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
#ifndef __TX_ALGO__
#define __TX_ALGO__

#ifdef __cplusplus
extern "C" {
#endif

#include "hdr.h"
#include "multipoint_drc.h"
#include "sw_gain.h"
#include "iir_process.h"


typedef bool (*TX_ALGO_EMMC_DUMP_CALLBACK)(uint8_t* buff, uint32_t len);

void tx_algo_emmc_dump_orig_24bit_register(TX_ALGO_EMMC_DUMP_CALLBACK cb);

void tx_algo_emmc_dump_orig_24bit_degister(void);

void tx_algo_emmc_dump_algo_24bit_register(TX_ALGO_EMMC_DUMP_CALLBACK cb);

void tx_algo_emmc_dump_algo_24bit_degister(void);

void tx_algo_emmc_dump_algo_32bit_register(TX_ALGO_EMMC_DUMP_CALLBACK cb);

void tx_algo_emmc_dump_algo_32bit_degister(void);

int tx_hdr_mode_get(void);

int tx_ga_mode_get(void);

int tx_eq_mode_get(void);

float tx_pre_gain_get(void);

float tx_post_gain_get(void);

void tx_hdr_mode_set(int mode);

void tx_ga_mode_set(int mode);

void tx_eq_mode_set(int mode);

void tx_pre_gain_set(float gain);

void tx_post_gain_set(float gain);

void tx_algo_mpdrc_onoff_set(bool onoff);

void tx_algo_onoff_set(bool onoff);

void tx_algo_init(enum AUD_SAMPRATE_T sample_rate, int32_t frame_size, enum AUD_BITS_T sample_bit, enum AUD_CHANNEL_NUM_T ch_num, bool heap_inited);

void tx_algo_deinit(void);

void tx_algo_process(uint8_t* buff, uint32_t* len);

#ifdef __cplusplus
}
#endif

#endif