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

#pragma once

#include "bt_base_types.h"

typedef uint8_t second_sco_decision_t;
#define IBRT_REJECT_SECOND_SCO                   0x00
#define IBRT_ACCEPT_SECOND_SCO_DISC_FIRST_SCO    0x01
#define IBRT_ACCEPT_SECOND_SCO_KEEP_FIRST_SCO    0x02
#define IBRT_HOST_DECIDE_SECONED_SCO             0x03
#define IBRT_ACCEPT_SECOND_SCO                   0x04

typedef uint8_t bg_music_action_t;
#define MUTE_BG_A2DP       0x00
#define PAUSE_BG_A2DP      0x01
#define CLOSE_BG_A2DP      0x02

typedef uint8_t second_sco_bg_action_t;
#define IBRT_ACTION_ROUTE_SCO_TO_PHONE 0x00
#define IBRT_ACTION_HOLD_ACTIVE_SCO    0x01

typedef uint8_t virtual_call_handle_t;
#define VIRTUAL_HANDLE_NORMAL_WAY      0x00
#define VIRTUAL_HANDLE_NON_PROMPT      0x01
#define VIRTUAL_HANDLE_NON_PROMPTED    0x02

typedef struct
{
    uint8_t a2dp_default_abs_volume;
    uint8_t a2dp_force_use_the_codec;
    uint8_t a2dp_force_non_type;
    bool a2dp_force_use_prev_codec;
    bool hid_capture_non_invade_mode;
    bool music_preempt_play_mode; //new music disruppt old music automatically
    bool call_preempt_play_mode; //new call disruppt old call automatically
    bool dont_auto_play_bg_a2dp;
    second_sco_decision_t second_sco_handle_mode;
    uint32_t a2dp_prompt_delay_ms;
    bg_music_action_t bg_a2dp_action;
    bool keep_only_one_stream_close_connected_a2dp;
    bool pause_a2dp_when_call_exist;
    bool reconn_sco_if_fast_disc_after_call_active_for_iphone_auto_mode;
    uint8_t hfp_default_volume;
    bool host_reject_unexcept_sco_packet;
    second_sco_bg_action_t second_sco_bg_action;
    bool allow_duck_ringtone;
    virtual_call_handle_t virtual_call_handle;
    bool pc_second_sco_discon_acl;
    bool create_sco_for_call_active_device;
    bool prompt_tone_cant_preempt_music;
    bool dont_resume_music_when_preempted_by_another_music;
    bool resume_pc_sco_by_dis_then_conn_hfp;
    bool only_use_avrcp_to_preempt_a2dp;
    bool reject_sco_req_within_a_certain_time_after_it_be_preempted;
    bool auto_resume_stream_when_focus_empty;
    bool sco_reject_connect_frist_and_disconnet_later;
    bool delay_abandon_a2dp_focus_depends_hfp_status;
    bool bt_update_active_device_when_incoming_call;
    bool a2dp_start_select_set_prefer_rate;   //Only enable when 3M packet is enabled
} bt_am_attributes_t;

enum app_bt_base_event_t {
    APP_BT_BASE_EVENT_AVRCP_PLAY_STATUS_CHANGED                  = 0x1,
    APP_BT_BASE_EVENT_AVRCP_PLAY_STATUS_NOTIFY_RSP               = 0x2,
    APP_BT_BASE_EVENT_AVRCP_PLAY_STATUS                          = 0x3,
    APP_BT_BASE_EVENT_AVRCP_PLAY_STATUS_MOCK                     = 0x4,
    APP_BT_BASE_EVENT_AVRCP_IS_REALLY_PAUSED                     = 0x5,
    APP_BT_BASE_EVENT_AVRCP_MEDIA_PLAYING                        = 0x6,
    APP_BT_BASE_EVENT_AVRCP_MEDIA_PAUSED                         = 0x7,
    APP_BT_BASE_EVENT_A2DP_STREAM_OPEN                           = 0x8,
    APP_BT_BASE_EVENT_A2DP_STREAM_CLOSE                          = 0x9,
    APP_BT_BASE_EVENT_A2DP_STREAM_START                          = 0xA,
    APP_BT_BASE_EVENT_A2DP_STREAM_MOCK_START                     = 0xB,
    APP_BT_BASE_EVENT_A2DP_STREAM_SUSPEND                        = 0xC,
    APP_BT_BASE_EVENT_A2DP_STREAM_SWITCH                         = 0xD,
    APP_BT_BASE_EVENT_A2DP_STREAM_RESELECT                       = 0xE,
    APP_BT_BASE_EVENT_AVRCP_CONNECTED                            = 0xF,
    APP_BT_BASE_EVENT_HFP_SERVICE_CONNECTED                      = 0x10,
    APP_BT_BASE_EVENT_HFP_SERVICE_MOCK_CONNECTED                 = 0x11,
    APP_BT_BASE_EVENT_HFP_SERVICE_DISCONNECTED                   = 0x12,
    APP_BT_BASE_EVENT_HFP_SCO_CONNECT_REQ                        = 0x13,
    APP_BT_BASE_EVENT_HFP_SCO_CONNECTED                          = 0x14,
    APP_BT_BASE_EVENT_HFP_SCO_DISCONNECTED                       = 0x15,
    APP_BT_BASE_EVENT_HFP_CCWA_IND                               = 0x16,
    APP_BT_BASE_EVENT_HFP_RING_IND                               = 0x17,
    APP_BT_BASE_EVENT_HFP_CLCC_IND                               = 0x18,
    APP_BT_BASE_EVENT_HFP_CALL_IND                               = 0x19,
    APP_BT_BASE_EVENT_HFP_CALLSETUP_IND                          = 0x1A,
    APP_BT_BASE_EVENT_HFP_CALLHELD_IND                           = 0x1B,
    APP_BT_BASE_EVENT_PROFILE_EXCHANGED                          = 0x1C,
    APP_BT_BASE_EVENT_IBRT_STARTED                               = 0x1D,
    APP_BT_BASE_EVENT_REPLAY_WHEN_PAUSE_COME_AFTER_SCO_DISC      = 0x1E,
    APP_BT_BASE_EVENT_REPLAY_WHEN_SUSPEND_COME_AFTER_SCO_DISC    = 0x1F,
    APP_BT_BASE_EVENT_ROUTE_CALL_TO_LE                           = 0x20,
    APP_BT_BASE_EVENT_ACL_CONNECTED                              = 0x21,
    APP_BT_BASE_EVENT_ACL_DISC                                   = 0x22,
    APP_BT_BASE_EVENT_SNOOP_DISC                                 = 0x23,
    APP_BT_BASE_EVENT_AUTHENTICATED                              = 0x24,
    APP_BT_BASE_EVENT_HFP_CALL_MOCK                              = 0x25,
    APP_BT_BASE_EVENT_HFP_CALLSETUP_MOCK                         = 0x26,
    APP_BT_BASE_EVENT_HFP_CALLHOLD_MOCK                          = 0x27,
    APP_BT_BASE_EVENT_HUNGUP_INCOMING_CALL                       = 0x28,
    APP_BT_BASE_EVENT_HUNGUP_ACTIVE_CALL                         = 0x29,
    APP_BT_BASE_HOLD_SWICTH_BACKGROUND                           = 0x2A,
    APP_BT_BASE_BG_TO_FG_COMPLETE                                = 0x2B,
    APP_BT_BASE_EVENT_AVRCP_PLAY_STATUS_CHANGED_MOCK             = 0x2C,
    APP_BT_BASE_EVENT_A2DP_SYNC_STREAMING_COMPLETE               = 0x2D,
    APP_BT_BASE_EVENT_A2DP_MUTE_FRAME                            = 0x2E,
    APP_BT_BASE_EVENT_HFP_SCO_CON_FAIL_IND                       = 0x2F,
    APP_BT_BASE_EVENT_MUTE_STREAM_DETECT_NOTIFY                  = 0x30,
    APP_BT_BASE_EVENT_SET_BT_BLE_ACTIVE_LINK_COMPLETE            = 0x31,
};
