/***************************************************************************
 *
 * Copyright 2015-2020 BES.
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
#ifndef __APP_BT_AUDIO_POLICY_H__
#define __APP_BT_AUDIO_POLICY_H__

#include "bluetooth.h"
#include "bes_me_api.h"
#include "bt_base_attributes.h"
#ifdef __cplusplus
extern "C" {
#endif

#define APP_BT_AUDIO_SCO_PACKET_HV3 0x02

#define APP_BT_AUDIO_SCO_PACKET_EV3 0x07

#define APP_BT_AUDIO_HFP_RECONN_MS (2000)

#define APP_BT_AUDIO_A2DP_RECONN_MS (500)

#define APP_BT_AUDIO_RINGTONE_MS (5000)

#define APP_BT_AUDIO_SWITCH_MS  (3200)    //how to caculate

#define APP_BT_AUDIO_SCO_REQ_DIFF_MS (1500)

#define APP_BT_AUDIO_ABANDON_FOCUS_MS (8000)

#define APP_BT_AUDIO_DELAY_A2DP_PLAY_MS (2000)

#define APP_BT_AUDIO_A2DP_STREAMING_CHECK_MS (3000)

#define APP_BT_AUDIO_AVRCP_PLAY_STATUS_WAIT_TIMER_MS (500)

#define APP_BT_AUDIO_AVRCP_STATUS_QUICK_SWITCH_FILTER_TIMER_MS (3000)

#define APP_BT_AUDIO_A2DP_WAIT_PHONE_AUTO_START_STREAM_MS (3000)

#define APP_BT_AUDIO_A2DP_WAIT_AVRCP_PAUSED_STREAM_SUSPEND_MS (10000)

#define APP_BT_AUDIO_FOCOUS_DATA_SIZE 72

#define APP_BT_AUDIO_STREAM_AVA_CHECK_MS   (4000)

enum SWITCH_A2DP_ERROR
{
    SWITCH_A2DP_NO_ERROR,
    SWITCH_A2DP_NOW,
    SWITCH_A2DP_RSP_NO_ERROR,
    SWITCH_A2DP_RSP_ERROR,
};

enum TONGGLE_A2DP_CIS_ERROR
{
    TOGGLE_A2DP_CIS_NO_ERROR,
    TOGGLE_A2DP_CIS_NOW,
    TOGGLE_A2DP_CIS_RSP_NO_ERROR,
    TOGGLE_A2DP_CIS_RSP_ERROR,
};

typedef enum
{
    BT_AUDIO_EVENT_MEDIA_SWITCH_CMP                      = 0x00,
    BT_AUDIO_EVENT_PROFILE_EXC_DONE                      = 0x01,
} app_bt_audio_switch_event_t;

typedef enum
{
    APP_BT_AUDIO_A2DP_RECHECK_CONTEXT_NULL,
    APP_BT_AUDIO_A2DP_WAIT_PHONE_AUTO_START_STREAM = 1,
    APP_BT_AUDIO_A2DP_WAIT_PAUSED_STREAM_SUSPEND,
} app_bt_audio_a2dp_recheck_enum_t;

typedef enum
{
    HF_FOCUS_MACHINE_CURRENT_IDLE                        = 0x00,
    HF_FOCUS_MACHINE_CURRENT_INCOMING                    = 0x01,
    HF_FOCUS_MACHINE_CURRENT_OUTGOING                    = 0x02,
    HF_FOCUS_MACHINE_CURRENT_CALLING                     = 0x03,
    HF_FOCUS_MACHINE_CURRENT_3WAY_INCOMING               = 0x04,
    HF_FOCUS_MACHINE_CURRENT_3WAY_HOLD_CALING            = 0x05,
    HF_FOCUS_MACHINE_CURRENT_IDLE_ANOTHER_IDLE           = 0x06,
    HF_FOCUS_MACHINE_CURRENT_INCOMMING_ANOTHER_INCOMMING = 0x07,
    HF_FOCUS_MACHINE_CURRENT_INCOMMING_ANOTHER_CALLING   = 0x08,
    HF_FOCUS_MACHINE_CURRENT_INCOMMING_ANOTHER_OUTGOING  = 0x09,
    HF_FOCUS_MACHINE_CURRENT_OUTGOING_ANOTHER_INCOMMING  = 0x0A,
    HF_FOCUS_MACHINE_CURRENT_OUTGOING_ANOTHER_OUTGOING   = 0x0B,
    HF_FOCUS_MACHINE_CURRENT_OUTGOING_ANOTHER_CALLING    = 0x0C,
    HF_FOCUS_MACHINE_CURRENT_CALLING_ANOTHER_INCOMING    = 0x0D,
    HF_FOCUS_MACHINE_CURRENT_CALLING_ANOTHER_OUTGOING    = 0x0E,
    HF_FOCUS_MACHINE_CURRENT_CALLING_ANOTHER_CALLING     = 0x0F,
    HF_FOCUS_MACHINE_CURRENT_CALLING_ANOTHER_HOLD        = 0x10,
    HF_FOCUS_MACHINE_CURRENT_HOLD_ANOTHER_ACTIVE         = 0x11,
    HF_FOCUS_MACHINE_CURRENT_ACTIVE_ANOTHER_HOLD         = 0x12,
} hf_call_focus_machine_t;

typedef enum
{
    APP_BT_AUDIO_A2DP_START  = 0,
    APP_BT_AUDIO_A2DP_STOP   = 1,
    APP_BT_AUDIO_SCO_START   = 2,
    APP_BT_AUDIO_SCO_STOP    = 3,
    APP_BT_AUDIO_MEDIA_START = 4,
    APP_BT_AUDIO_MEDIA_STOP  = 5,
}app_bt_audio_action_t;

typedef struct
{
    bool time_valid;
    uint32_t time_of_sco_preempted;
} sco_preempted_time_t;

typedef bool (*local_stream_status_changed)(const bt_bdaddr_t *addr, uint8_t state);

typedef bool (*app_bt_audio_ui_allow_resume_request)(bt_bdaddr_t *bdaddr);

typedef void(*app_bt_audio_adapter_player)(app_bt_audio_action_t action, uint8_t device_id, uint32_t aud_id);

typedef int(*app_bt_audio_event_callback)(bt_bdaddr_t *addr, enum app_bt_base_event_t event, uint32_t data);

void app_bt_audio_event_callback_init(int(*cb)(bt_bdaddr_t *addr, enum app_bt_base_event_t event, uint32_t data));

void app_bt_audio_adapter_player_init(app_bt_audio_adapter_player player);

void app_bt_audio_strategy_init(void);

void app_bt_audio_strategy_deinit(void);

uint32_t app_bt_audio_create_new_prio(void);

void app_bt_audio_init_focus_listener(void *device);

void app_bt_audio_handle_profile_exchanged_done(void);

bool app_bt_audio_all_profile_exchanged(void);

bool app_bt_audio_allow_send_profile_immediate(void *remote);

void app_bt_audio_set_bt_ble_active_link_completed(uint8_t status);

bool app_bt_audio_judge_is_pc_by_device_id(uint8 devid);

bool app_bt_audio_handle_peer_focus_info(uint8_t *buff, uint16_t length);

bool app_bt_audio_handle_peer_focus_info_rsp(uint8_t *buff, uint16_t length);

void app_bt_audio_register_local_stream_callback(local_stream_status_changed cb);

uint8_t app_bt_audio_get_streaming_device_num(void);

uint8_t app_bt_audio_select_connected_device(void);

uint8_t app_bt_audio_get_device_for_user_action(void);

void app_bt_audio_profile_timer_init(void);

void app_bt_audio_profile_timer_deinit(void);

uint8_t app_bt_audio_get_curr_audio_focus_type(void);

int app_bt_audio_event_handler(uint8_t device_id, enum app_bt_audio_event_t event, uint32_t data);

bool a2dp_bt_audio_is_in_switch(void);

#if defined(BT_A2DP_SUPPORT)
void app_bt_audio_stop_a2dp_playing(uint8_t device_id);

void app_bt_audio_toggle_a2dp_cis(void);

void app_bt_audio_toggle_a2dp_cis_handler(uint32_t btclk, uint8_t error_code);

void app_bt_audio_check_toggle_a2dp_cis(void);

void app_bt_audio_register_toggle_a2dp_cis_cmp_cb(void (*cb)(uint8_t device_id));

void app_bt_audio_switch_streaming_a2dp();

void app_bt_audio_switch_streaming_a2dp_handler(uint32_t btclk, uint8_t error_code);

void app_bt_audio_check_switch_streaming_a2dp(void);

void app_bt_audio_register_switch_streaming_a2dp_cmp_cb(void (*cb)(uint8_t device_id));

// TODO: get_curr_a2dp_device & get_curr_playing_a2dp maybe only need one
uint8_t app_bt_audio_get_curr_a2dp_device(void);

uint8_t app_bt_audio_select_bg_a2dp_to_resume(void);

uint8_t app_bt_audio_count_streaming_a2dp(void);

uint8_t app_bt_audio_get_curr_playing_a2dp(void);

void app_bt_audio_a2dp_resume_this_profile(uint8_t device_id);

void app_bt_audio_recheck_a2dp_streaming(void);

bool app_bt_audio_a2dp_disconnect_self_check(uint8_t device_id);

void app_bt_audio_receive_peer_a2dp_playing_device(bool is_response, uint8_t device_id);

uint8 app_bt_audio_a2dp_close_req_allow(uint8_t device_id);

void app_bt_audio_stop_check_a2dp_restreaming_timer(uint8_t device_id);

void app_bt_audio_start_check_a2dp_restreaming_timer(uint8_t device_id, uint32_t timeout);

void app_bt_audio_switch_to_non_prompt_disc_a2dp_play_mode(void);

void app_bt_audio_switch_to_multi_a2dp_quick_switch_play_mode(void);

uint8_t app_bt_audio_select_another_streaming_a2dp(uint8_t curr_device_id);

void app_bt_audio_play_a2dp_stream(uint8_t device_id);

void app_bt_audio_pause_a2dp_stream(uint8_t device_id);

uint8_t app_bt_audio_select_last_paused_a2dp_device(void);
#endif /* BT_A2DP_SUPPORT */

#if defined(BT_HFP_SUPPORT)
bool app_bt_audio_current_device_is_hfp_idle_state(uint8_t device_id);

void app_bt_audio_stop_sco_playing(uint8_t device_id);

void app_bt_audio_switch_streaming_sco(void);

void app_bt_audio_switch_streaming_sco_handler(void);

uint8_t app_bt_audio_count_connected_hfp(void);

uint8_t app_bt_audio_count_connected_sco(void);

uint8_t app_bt_audio_get_curr_playing_sco(void);

void app_bt_audio_new_sco_is_rejected_by_controller(uint8_t device_id);

void app_bt_audio_peer_sco_codec_received(uint8_t device_id);

hf_call_focus_machine_t app_bt_audio_get_current_hfp_machine();

bool app_bt_audio_device_is_hfp_idle_state(uint8_t device_id);

uint8_t app_bt_audio_get_hfp_device_for_user_action(void);

uint8_t app_bt_audio_select_call_active_hfp(void);

uint8_t app_bt_audio_get_curr_hfp_device(void);

uint8_t app_bt_audio_get_curr_sco_device(void);

uint32_t app_bt_audio_trigger_switch_mute_streaming_sco(uint32_t btclk);

uint8_t app_bt_audio_select_another_call_active_hfp(uint8_t curr_device_id);

uint8_t app_bt_audio_select_another_call_setup_hfp(uint8_t curr_device_id);

uint8_t app_bt_audio_select_another_device_to_create_sco(uint8_t curr_device_id);

uint8_t app_bt_audio_get_another_hfp_device_for_user_action(uint8_t curr_device_id);

void app_bt_audio_delay_abandon_focus(uint8_t device_id);

void app_bt_audio_delay_abandon_focus_clear();

void app_bt_audio_restart_sco_player(uint8_t device_id);
#endif /* BT_HFP_SUPPORT */

#ifdef __cplusplus
}
#endif

#endif /* __APP_BT_AUDIO_POLICY_H__ */
