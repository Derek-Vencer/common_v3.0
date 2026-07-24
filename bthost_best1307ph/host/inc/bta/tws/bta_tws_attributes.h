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

typedef enum
{
    // dont disc any mob
    BTA_TWS_PAIRING_DISC_NONE = 0,
    // disc all already connected mob when enter pairing
    BTA_TWS_PAIRING_DISC_ALL_MOB,
    // support multipoint & current already exist two link, but only disc one when enter pairing
    BTA_TWS_PAIRING_DISC_ONE_MOB,
} bta_tws_pairing_with_disc_num;

typedef enum
{
    // just disconnect sco and resume when pairing exit
    BTA_TWS_PAIRING_DISC_SCO = 0,
    // hungup call
    BTA_TWS_PAIRING_HF_HUNGUP,
} bta_tws_pairing_with_disc_hf_cfg;

typedef enum
{
    BTA_TWS_NV_MERGE_LOCAL_FIRST = 1,
    BTA_TWS_NV_MERGE_PEER_FIRST,
    BTA_TWS_NV_MERGE_MIX_LOCAL_FIRST,
    BTA_TWS_NV_MERGE_MIX_PEER_FIRST,
} bta_tws_nv_merge_scheme_t;

typedef enum
{
    BTA_TWS_NV_MERGE_SCOPE_MINIMAL, // Merge only NV records of devices involved in active or pending reconnections.
    BTA_TWS_NV_MERGE_SCOPE_FULL,    // Merge the full NV records from both earbuds.
} bta_tws_nv_merge_scope_t;

typedef enum
{
    BTA_TWS_AFH_ASSESS_POLICY_ALWAYS_ON,         // Keep afh assessment always enabled (ui master only).
    BTA_TWS_AFH_ASSESS_POLICY_ON_LINK,           // Enable when any link exists (ui master only).
    BTA_TWS_AFH_ASSESS_POLICY_ON_STREAMING,      // Enable only during streaming (ui master only).
} bta_tws_afh_assess_policy_t;

typedef struct
{
    uint32_t rx_seq_error_timeout;
    uint32_t rx_seq_error_threshold;
    uint32_t rx_seq_recover_wait_timeout;
    uint32_t rssi_monitor_timeout;
    uint32_t tws_conn_failed_wait_time;
    uint32_t connect_no_03_timeout;
    uint32_t disconnect_no_05_timeout;
    uint32_t tws_cmd_send_timeout;
    uint32_t tws_cmd_send_counter_threshold;

    /// freeman mode config, default should be false
    bool freeman_enable;
    /// enable sdk lea adv strategy, default should be true
    bool sdk_lea_adv_enable;
    /// pairing mode timeout value config
    uint32_t pairing_timeout_value;
    /// SDK pairing enable, the default should be true
    bool sdk_pairing_enable;
    /// passive enter pairing when no mobile record, the default should be false
    bool enter_pairing_on_empty_record;
    /// passive enter pairing when reconnect failed, the default should be false
    bool enter_pairing_on_reconnect_mobile_failed;
    /// passive enter pairing when mobile disconnect, the default should be false
    bool enter_pairing_on_mobile_disconnect;
    /// exit pairing when peer close, the default should be true
    bool exit_pairing_on_peer_close;
    /// exit pairing when a2dp or hfp streaming, the default should be true
    bool exit_pairing_on_streaming;
    /// disconnect remote devices when entering pairing mode actively
    uint8_t paring_with_disc_mob_num;
    /// disconnect SCO or not when accepting phone connection in pairing mode
    uint8_t pairing_with_disc_hf_cfg;
    /// pause current music when entering pairing mode
    bool pairing_with_pause_music;
    /// not start ibrt in pairing mode, the default should be false
    bool pairing_without_start_ibrt;
    /// set nv master to tws master for lea connection
    bool pairing_with_set_nv_master_as_master;
    /// disconnect tws immediately when single bud closed in box
    bool disc_tws_imm_on_single_bud_closed;

    ///no need to reconnect bt_device when easbuds in busy mode
    bool disallow_reconnect_bt_device_in_streaming_state;
    ///no need to reconnect tws when easbuds in busy mode
    bool disallow_reconnect_tws_in_streaming_state;
    bool disallow_reconnect_bt_device_in_streaming_state_triggered_by_case_open;
    bool disallow_reconnect_bt_device_in_streaming_state_triggered_by_non_case_open;

    /// open(enable) without auto reconnect
    bool open_without_auto_reconnect;
    /// do tws switch when RSII value change, default should be true
    bool tws_switch_according_to_rssi_value;
    /// controller basband monitor
    bool lowlayer_monitor_enable;
    bool support_steal_connection;
    bool check_plugin_excute_closedbox_event;
    //if add ai feature
    bool ibrt_with_ai;

    bool tws_switch_tx_data_protect;

    bool without_reconnect_if_remote_initiate_disc;
    bool without_reconnect_when_fetch_out_wear_up;
    bool only_rs_when_sco_active;
    bool reject_same_cod_device_conn_req;
    /// do tws switch when rssi value change over threshold
    uint8_t rssi_threshold;
    /// do tws switch when RSII value change, timer threshold
    uint8_t role_switch_timer_threshold;
    uint8_t audio_sync_mismatch_resume_version;

    uint8_t  profile_concurrency_supported;

    /// close box delay disconnect tws timeout
    uint16_t close_box_delay_tws_disc_timeout;

    /// close box debounce time config
    uint16_t close_box_event_wait_response_timeout;
    /// reconnect event internal config wait timer when tws disconnect
    uint16_t reconnect_wait_ready_timeout;
    uint16_t reconnect_mobile_wait_ready_timeout;
    uint16_t reconnect_tws_wait_ready_timeout;

    /// wait time before launch reconnect event
    uint16_t reconnect_mobile_wait_response_timeout;
    uint16_t reconnect_ibrt_wait_response_timeout;
    uint16_t nv_master_reconnect_tws_wait_response_timeout;
    uint16_t nv_slave_reconnect_tws_wait_response_timeout;

    /// open box reconnect mobile times config
    uint16_t open_reconnect_mobile_max_times;
    /// open box reconnect tws times config
    uint16_t open_reconnect_tws_max_times;
    /// connection timeout reconnect mobile times config
    uint16_t reconnect_mobile_max_times;
    /// connection timeout reconnect tws times config
    uint16_t reconnect_tws_max_times;
    /// connection timeout reconnect ibrt times config
    uint16_t reconnect_ibrt_max_times;
    uint16_t mobile_page_timeout;

    /// tws connection supervision timeout
    uint16_t tws_connection_timeout;

    uint16_t radical_scan_interval_nv_slave;
    uint16_t radical_scan_interval_nv_master;

    uint16_t scan_interval_in_sco_tws_disconnected;
    uint16_t scan_window_in_sco_tws_disconnected;

    uint16_t scan_interval_in_sco_tws_connected;
    uint16_t scan_window_in_sco_tws_connected;

    uint16_t scan_interval_in_a2dp_tws_disconnected;
    uint16_t scan_window_in_a2dp_tws_disconnected;

    uint16_t scan_interval_in_a2dp_tws_connected;
    uint16_t scan_window_in_a2dp_tws_connected;

    bool support_steal_connection_in_sco;
    bool support_steal_connection_in_a2dp_steaming;
    bool steal_audio_inactive_device;
    bool allow_sniff_in_sco;
    bool always_interlaced_scan;
    uint8_t llmonitor_report_format;
    uint32_t llmonitor_report_count;

    bool is_changed_to_ui_master_on_tws_disconnected;
    /// if tws&mobile disc, reconnect tws first until tws connected then reconn mobile
    bool delay_reconn_mob_until_tws_connected;
    uint8_t delay_reconn_mob_max_times;
    bta_tws_nv_merge_scheme_t merge_nv_scheme;
    // handle lea multiple connect cfg
    bool  connected_max_device_num_allow_new_connect;
    bool lea_connected_allow_new_connect;
    bool dev_idle_allow_nonsupport_stay_connected;

    bta_tws_nv_merge_scope_t nv_merge_scope;
    bta_tws_afh_assess_policy_t afh_assess_policy;
} bta_tws_attributes_t;
