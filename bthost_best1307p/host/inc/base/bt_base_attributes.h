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
    bool a2dp_prompt_play_only_when_avrcp_play_received;
    uint32_t a2dp_prompt_delay_ms;
    bool a2dp_delay_prompt_play;
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
    bool delay_abandon_a2dp_focus_depends_hfp_status;
} bt_am_attributes_t;
