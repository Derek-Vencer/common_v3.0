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
#ifndef __AP_PORTING_H__
#define __AP_PORTING_H__
#include "bluetooth_bt_api.h"

#ifdef __cplusplus
extern "C" {
#endif
bool ap_is_profile_exchanged(const bt_bdaddr_t *addr);

uint8_t ap_get_dev_acl_state(const bt_bdaddr_t* addr);

bool ap_is_acl_disconnecting(const bt_bdaddr_t* addr);

void ap_ext_cmd_send_focus_info(uint8_t* param, uint16_t length, bool is_response);

bool ap_tws_is_connected(void);

bool ap_is_mobile_link_connected(const bt_bdaddr_t* addr);

bool ap_is_a2dp_profile_exchanged(const bt_bdaddr_t* addr);

bool ap_disconnect_a2dp_prfoile(uint8_t device_id);

bool ap_connect_a2dp_prfoile(uint8_t device_id);

void ap_ext_send_a2dp_playing_device(uint8_t device_id, bool is_rsp);

void ap_hold_active_call(uint8_t device_id);

bool ap_is_role_switch_on(void);

void ap_a2dp_stream_modify_tws_bandwith(bool enable);

void ap_a2dp_send_allow_init(bool(*send_allow)(void *remote));

void ap_start_switch_a2dp_action(uint32_t btclk, uint8_t error_code);

void ap_start_toggle_a2dp_cis_action(uint32_t btclk, uint8_t error_code);

void ap_start_switch_sco_action();

uint8_t ap_media_manager_get_current_sco();

uint8_t ap_media_manager_get_current_music();

bool ap_media_manager_is_device_media_active(uint8_t device_id);

bool ap_media_manager_is_media_playing();

void ap_a2dp_player_sysfreq_update_normalfreq();

void ap_sco_player_switch_trigger_init(bool (*func)(void));

void ap_sco_player_switch_trigger_deinit();

void ap_disconnect_acl(const bt_bdaddr_t* addr);

void ap_connect_acl(const bt_bdaddr_t* addr);
#ifdef __cplusplus
}
#endif
#endif