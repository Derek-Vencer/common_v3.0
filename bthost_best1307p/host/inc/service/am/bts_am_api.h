/****************************************************************************
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

#ifndef __BTS_AM_API_H__
#define __BTS_AM_API_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 ****************************************************************************************
 *       _             _    __  __                 _          _ 
 *      / \  _   _  __| |  |  \/  | __ _ _ __     / \   _ __ (_)
 *     / _ \| | | |/ _` |  | |\/| |/ _` | '__|   / _ \ | '_ \| |
 *    / ___ \ |_| | (_| |  | |  | | (_| | |     / ___ \| |_) | |
 *   /_/   \_\__,_|\__,_|  |_|  |_|\__, |_|    /_/   \_\ .__/|_|
 *                                 |___/               |_|
 ****************************************************************************************
 */

uint8_t bts_am_get_streaming_device_num(void);

uint8_t bts_am_get_curr_audio_focus_type(void);

uint8_t bts_am_get_device_for_user_action(void);

uint8_t bts_am_get_curr_playing_a2dp(void);

uint8_t bts_am_get_curr_playing_sco(void);

uint8_t bts_am_get_curr_a2dp_device(void);

uint8_t bts_am_get_curr_hfp_device(void);

uint8_t bts_am_get_curr_sco_device(void);

uint8_t bts_am_count_streaming_a2dp(void);

uint8_t bts_am_count_connected_hfp(void);

uint8_t bts_am_count_connected_sco(void);

void bts_am_switch_streaming_sco(void);

void bts_am_switch_streaming_sco_handler(void);

uint8_t bts_am_select_another_device_to_create_sco(uint8_t curr_device_id);

uint8_t bts_am_get_hfp_device_for_user_action(void);

uint8_t bts_am_get_another_hfp_device_for_user_action(uint8_t curr_device_id);

void bts_am_switch_streaming_a2dp(void);

void bts_am_switch_streaming_a2dp_handler(uint32_t btclk, uint8_t error_code);

void bts_am_check_switch_streaming_a2dp(void);

void bts_am_register_switch_streaming_a2dp_cmp_cb(void (*cb)(uint8_t device_id));

void bts_am_toggle_a2dp_cis();

void bts_am_toggle_a2dp_cis_handler(uint32_t btclk, uint8_t error_code);

void bts_am_check_toggle_a2dp_cis(void);

void bts_am_register_toggle_a2dp_cis_cmp_cb(void (*cb)(uint8_t device_id));

uint32_t bts_am_trigger_switch_mute_streaming_sco(uint32_t btclk);

void bts_am_play_a2dp_stream(uint8_t device_id);

void bts_am_pause_a2dp_stream(uint8_t device_id);

void bts_am_resume_a2dp_music_player(uint8_t device_id);

uint8_t bts_am_lea_get_active_conlid(void);

uint8_t bts_am_lea_get_curr_playing_conlid(void);

uint8_t bts_am_lea_get_music_streaming_conlid(void);

#ifdef __cplusplus
}
#endif

#endif /* __BTS_AM_API_H__ */