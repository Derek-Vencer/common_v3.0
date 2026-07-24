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
#include "app_earbuds.h"

#include "apps.h"
#include "cmsis_os2.h"
#include "hal_trace.h"

#include "hal_codec.h"
#include "hal_bootmode.h"

#include "app_battery.h"
#include "app_factory_bt.h"
#include "nvrecord_extension.h"
#include "app_ibrt_customif_cmd.h"

#include "bta_tws_ux_api.h"
#include "bta_bt_api.h"
#include "app_bta_cmd.h"

#include "app_ibrt_configure.h"

#include "earbud_ux_api.h"
#include "sep_codec_config.h"
#ifdef MEDIA_PLAYER_SUPPORT
#include "app_media_player.h"
#endif

#if defined(SNDP_VAD_ENABLE)
#include "mcu_sensor_hub_app_soundplus.h"
#endif

#ifdef __BIXBY
#include "app_bixby_thirdparty_if.h"
#endif

#ifdef GFPS_ENABLED
#include "gfps.h"
#endif

#ifdef __AI_VOICE__
#include "ai_spp.h"
#endif

#ifdef FINDMY_ENABLED
#include "findmy_internal.h"
#endif

#if defined (BLE_HOST_SUPPORT) || defined (__GATT_OVER_BR_EDR__)
#ifdef CFG_APP_DATAPATH_SERVER
#include "app_datapaths.h"
#endif
#endif

#ifdef CFG_APP_DATAPATH_CLIENT
#include "app_datapathc.h"
#endif

#ifdef BLE_HOST_SUPPORT
#include "app_ble_mgr.h"
#include "app_ble_ota.h"
#include "app_ble_swift.h"
#include "app_bis_selfscan.h"
#include "app_ble_battery.h"
#include "app_ble_tota.h"
#include "app_ble_dis.h"
#endif

#ifdef SWIFT_ENABLED
#include "app_ble_swift.h"
#endif

#define APP_APPEARANCE_VALUE_EARBUDS    (0x0941)

#define  IBRT_UI_CLOSE_BOX_EVENT_WAIT_RESPONSE_TIMEOUT              (600)//ms
#define  IBRT_UI_MOBILE_RECONNECT_WAIT_READY_TIMEOUT                (1000)//ms
#define  IBRT_UI_TWS_RECONNECT_WAIT_READY_TIMEOUT                   (1500)//ms
#define  IBRT_UI_RECONNECT_MOBILE_WAIT_RESPONSE_TIMEOUT             (5000)//5s
#define  IBRT_UI_RECONNECT_IBRT_WAIT_RESPONSE_TIMEOUT               (300)//ms
#define  IBRT_UI_NV_SLAVE_RECONNECT_TWS_WAIT_RESPONSE_TIMEOUT       (1000)//ms
#define  IBRT_UI_NV_MASTER_RECONNECT_TWS_WAIT_RESPONSE_TIMEOUT      (300)//ms
#define  IBRT_UI_DISABLE_BT_SCAN_TIMEOUT                            (150000)//5min

/// IBRT_UI_OPEN_RECONNECT_TWS_MAX_TIMES need max than IBRT_UI_DELAY_RECONN_MOBILE_MAX_TIMES
#define  IBRT_UI_DELAY_RECONN_MOBILE_MAX_TIMES              (3)

#define  IBRT_UI_OPEN_RECONNECT_MOBILE_MAX_TIMES            (3)
#ifdef PRODUCTION_LINE_PROJECT_ENABLED
#define  IBRT_UI_OPEN_RECONNECT_TWS_MAX_TIMES               (0xFFFF)
#define  IBRT_UI_RECONNECT_TWS_MAX_TIMES                    (0xFFFF)
#else
#define  IBRT_UI_OPEN_RECONNECT_TWS_MAX_TIMES               (6)
#define  IBRT_UI_RECONNECT_TWS_MAX_TIMES                    (20)
#endif

#define  IBRT_UI_RX_SEQ_ERROR_TIMEOUT                     (10000)
#define  IBRT_UI_RX_SEQ_ERROR_THRESHOLD                   (50)
#define  IBRT_UI_RX_SEQ_ERROR_RECOVER_TIMEOUT             (5000)

#define  IBRT_TWS_SWITCH_RSSI_THRESHOLD                   (30)

#ifndef TRACE
#define TRACE(attr, str, ...)   TR_INFO(attr, str, ##__VA_ARGS__)
#endif

static bt_pbap_search_recrod_t pbap_search_records[BT_DEVICE_NUM]= {0,};

static void bt_link_state_changed_handler(const bt_bdaddr_t *addr, bta_tws_bt_link_event_t event, bt_ibrt_role_t role, uint8_t reason)
{
    uint8_t device_id = bta_get_device_id_by_addr(addr);
    TRACE(0, "custom_ui d(%x)bt_link %d role %d reason %d", device_id, event, role, reason);

    switch (event)
    {
        case BTA_TWS_BT_DISCONNECTED_EVENT:
#if defined(SNDP_VAD_ENABLE)
            if (bta_tws_get_ui_role() != BT_IBRT_SLAVE)
            {
                app_sensor_hub_sndp_mcu_request_vad_stop();
            }
#endif

#ifdef GFPS_ENABLED
            gfps_link_disconnect_handler(SET_BT_ID(device_id), addr, reason);
#endif
        break;
        case BTA_TWS_BT_CONNECTING_EVENT:
        break;
        case BTA_TWS_BT_CONNECTING_CANCELLED_EVENT:
        break;
        case BTA_TWS_BT_CONNECTING_FAILURE_EVENT:
        break;
        case BTA_TWS_BT_CONNECTED_EVENT:
        break;
        case BTA_TWS_BT_ENCRYPTED_EVENT:
        {
#if defined(SNDP_VAD_ENABLE)
            if (bta_tws_get_ui_role() != BT_IBRT_SLAVE)
            {
                app_sensor_hub_sndp_mcu_request_vad_start();
            }
#endif

#ifdef GFPS_ENABLED
            gfps_link_connect_handler(SET_BT_ID(device_id), addr);
#endif

#ifdef FINDMY_ENABLED
            findmy_bt_connected_handler(addr);
#endif
        }
        break;

        case BTA_TWS_IBRT_DISCONNECTED_EVENT:
        break;
        case BTA_TWS_IBRT_CONNECTED_EVENT:
        break;
        case BTA_TWS_IBRT_ROLE_CHANGED_EVENT:
        break;
    }
}

static void bt_profile_exchanged_handler(const bt_bdaddr_t *addr)
{
    uint8_t device_id = bta_get_device_id_by_addr(addr);
    TRACE(0, "custom_ui d(%x)profile_exchanged", device_id);

}

static void bt_access_mode_changed_handler(bt_access_mode_t mode)
{
    TRACE(0, "custom_ui bt_access_mode %d", mode);

}

static void bt_link_mode_changed_handler(const bt_bdaddr_t *addr, bt_link_mode_t mode, uint16_t interval, uint8_t status)
{
    uint8_t device_id = bta_get_device_id_by_addr(addr);
    TRACE(0, "custom_ui d(%x)bt_link_mode %d", device_id, mode);
}

static void remote_name_request_completed_handler(const bt_bdaddr_t *addr, const uint8_t *name, uint8_t len)
{
    uint8_t device_id = bta_get_device_id_by_addr(addr);
    TRACE(0, "custom_ui d(%x)name %.*s", device_id, len, name);
}

static void bt_subsys_state_changed_handler(bta_tws_subsys_state_t state)
{
    TRACE(0, "custom_ui bt_subsys %d", state);

    // bta_tws_config_pscan_interval can be called here
}

static void tws_link_state_changed_handler(bta_tws_sync_state_t state, uint8_t reason)
{
    TRACE(0, "custom_ui tws_link %d reason %d", state, reason);

    switch (state)
    {
        case BTA_TWS_DISCONNECTED:
#if defined(A2DP_AUDIO_STEREO_MIX_CTRL)
            a2dp_audio_stereo_set_mix(true);
#endif
        break;
        case BTA_TWS_SYNCING:
#if defined(A2DP_AUDIO_STEREO_MIX_CTRL)
            a2dp_audio_stereo_set_mix(false);
#endif
        break;
        case BTA_TWS_SYNCED:
        break;
    }

#if defined(SWIFT_ENABLED)
    app_ble_swift_adv_refresh();
#endif /* SWIFT_ENABLED */

#if defined (BIS_SELFSCAN_ENABLED)
    app_bis_selfscan_tws_state_handler(state == BTA_TWS_SYNCED);
#endif
}

static void pairing_mode_entered_handler()
{
#if !defined(BESUI_TWS_EN) && !defined(BESUI_STEREO_EN)
#ifdef GFPS_ENABLED
    gfps_enter_fastpairing_mode();
#endif
#endif

#ifdef SWIFT_ENABLED
    app_swift_enter_pairing_mode();
#endif
}

static void pairing_mode_exited_handler()
{
#ifdef SWIFT_ENABLED
    app_swift_exit_pairing_mode();
#endif
}

static void pairing_mode_changed_handler(bool enabled)
{
    if (enabled)
    {
        pairing_mode_entered_handler();
    }
    else
    {
        pairing_mode_exited_handler();
    }
}

static void peer_box_state_changed_handler(bta_tws_box_state_t box_state)
{
    TRACE(0, "custom_ui peer_box_state %d", box_state);
}

static void local_box_state_changed_handler(bta_tws_box_state_t box_state)
{
    TRACE(0, "custom_ui local_box_state %d", box_state);
}

static void tws_role_switch_state_changed_handler(bt_ui_role_t role)
{
    TRACE(0, "custom_ui tws_role_switch %d", role);
}

static void disconnect_channels_when_basic_profiles_disconncted(const bt_bdaddr_t *addr)
{
    POSSIBLY_UNUSED uint8_t device_id = bta_get_device_id_by_addr(addr);

#ifdef __IAG_BLE_INCLUDE__
    if (!bta_a2dp_is_connected(addr) &&
#ifdef BT_HFP_SUPPORT
        !bta_hf_is_connected(addr)
#endif
        )
    {
#ifdef __AI_VOICE__
        ai_spp_enumerate_disconnect_service((uint8_t *)addr, device_id);
#endif
#ifdef GFPS_ENABLED
        gfps_disconnect(SET_BT_ID(device_id));
#endif
    }
#endif // __IAG_BLE_INCLUDE__
}

static void a2dp_connection_state_changed_handler(const bt_bdaddr_t *addr, bt_a2dp_conn_state_t state, uint8_t error_code)
{
    uint8_t device_id = bta_get_device_id_by_addr(addr);
    TRACE(0, "custom_ui d(%x)a2dp_connection %d reason %d", device_id, state, error_code);

    switch (state)
    {
        case BT_A2DP_CONN_STATE_DISCONNECTED:
            disconnect_channels_when_basic_profiles_disconncted(addr);
        break;
        case BT_A2DP_CONN_STATE_CONNECTED:
        break;
    }
}

static void a2dp_audio_state_changed_handler(const bt_bdaddr_t *addr, bt_a2dp_audio_state_t state, uint8_t error_code)
{
    uint8_t device_id = bta_get_device_id_by_addr(addr);
    TRACE(0, "custom_ui d(%x)a2dp_audio %d reason %d", device_id, state, error_code);
}

static void hfp_connection_state_changed_handler(const bt_bdaddr_t *addr, bt_hfp_conn_state_t state, uint8_t error_code)
{
    uint8_t device_id = bta_get_device_id_by_addr(addr);
    TRACE(0, "custom_ui d(%x)hfp_connection %d reason %d", device_id, state, error_code);
    switch (state)
    {
        case BT_HFP_CONN_STATE_DISCONNECTED:
            disconnect_channels_when_basic_profiles_disconncted(addr);
        break;
        case BT_HFP_CONN_STATE_CONNECTED:
        break;
    }
}

static void hfp_audio_status_changed_handler(const bt_bdaddr_t *addr, bt_hfp_audio_state_t state, bt_hfp_audio_codec_t codec, uint8_t error_code)
{
    uint8_t device_id = bta_get_device_id_by_addr(addr);
    TRACE(0, "custom_ui d(%x)hfp_audio %d codec %d reason %d", device_id, state, codec, error_code);
}

static void hfp_query_current_calls_handler(const bt_bdaddr_t *addr, int index, bt_hfp_curr_call_direction_t dir,
                                                    bt_hfp_curr_call_state_t state, bt_hfp_curr_call_mrty_t mpty, const char *number)
{
    uint8_t device_id = bta_get_device_id_by_addr(addr);
    // // Allow phonebook list query by incoming call number if input number is not empty and PBAP is enabled
    if(number)
    {
        uint8_t tel_num_len = strlen(number) >= BT_PBAP_MAX_VCARD_CONTACT_TEL_SIZE ? BT_PBAP_MAX_VCARD_CONTACT_TEL_SIZE - 1 : strlen(number);
        memcpy(pbap_search_records[device_id].search_tel_num, number, tel_num_len);
        pbap_search_records[device_id].search_tel_num[tel_num_len] = '\0';
        pbap_search_records[device_id].search_tel_len = tel_num_len;
        pbap_search_records[device_id].list_done = false;

        bt_pbap_request_search_param_t pbap_search_param = {0,};
        pbap_search_param.child_path_name = (char*)BT_PBAP_TELECOM_FOLDER_NAME; // folder name: "telecom"
        pbap_search_param.child_path_name_len = strlen(BT_PBAP_TELECOM_FOLDER_NAME);
        pbap_search_param.search_tel_num = pbap_search_records[device_id].search_tel_num; // don't use local var, the action is asyn
        pbap_search_param.search_tel_len = tel_num_len;
        bta_pbap_client_list_search_vcard_by_telephone(addr, &pbap_search_param);
    }
}

static void pbap_connection_state_changed_handler(const bt_bdaddr_t *addr, bt_pbap_conn_state_t state, bt_pbap_report_param_t* param)
{
    uint8_t device_id = bta_get_device_id_by_addr(addr);
    switch (state)
    {
        case BT_PBAP_CONN_STATE_DISCONNECTED:
            TRACE(0, "custom_ui d(%x)pbap disconnect reason %d", device_id, param->error_code);
            break;
        case BT_PBAP_CONN_STATE_OBEX_CONNECTED:
            TRACE(0, "custom_ui d(%x)pbap obex connect", device_id);
            break;
        case BT_PBAP_CONN_STATE_CONNECTED:
            TRACE(0, "custom_ui d(%x)pbap connect", device_id);
            // Clear PBAP search record structure to default values (avoid residual data from previous connection)
            memset(&pbap_search_records[device_id], 0, sizeof(bt_pbap_search_recrod_t));
            break;
        default:
            TRACE(0, "custom_ui d(%x)pbap unknown conn event", device_id);
            break;
    }
}

static void pbap_search_result_handler(const bt_bdaddr_t* addr, bt_pbap_search_state_t state, bt_pbap_report_param_t* param)
{
    uint8_t device_id = bta_get_device_id_by_addr(addr);
    switch (state)
    {
        case BT_PBAP_PHONEBOOK_SIZE_RSP:
            TRACE(0, "custom_ui d(%x)pbap search pb size: %d", device_id, param->phonebook_size);
            break;
        case BT_PBAP_LIST_STATE_ITEM_RSP:
            TRACE(0, "custom_ui d(%x)pbap get list item", device_id);
            // telephone number
            if(pbap_search_records[device_id].search_tel_len)
            {
                TRACE(0, "custom_ui d(%x)pbap_list_item tel(%d) %s", device_id, pbap_search_records[device_id].search_tel_len, pbap_search_records[device_id].search_tel_num);
            }
            // telephone name
            if(param->list_rsp_item->contact_name_len)
            {
                TRACE(0, "custom_ui d(%x)pbap_list_item name(%d) %s", device_id, param->list_rsp_item->contact_name_len, param->list_rsp_item->vcard_contact_name);
            }
            break;
        case BT_PBAP_LIST_STATE_DONE:
            TRACE(0, "custom_ui d(%x)pbap list done", device_id);
            pbap_search_records[device_id].list_done = true;
            break;
        case BT_PBAP_PULL_STATE_ENTRY_RSP:
            TRACE(0, "custom_ui d(%x)pbap get entry item", device_id);
            // telephone number
            if(param->pull_rsp_item->contact_tel_len)
            {
                TRACE(0, "custom_ui d(%x)pbap_pull_item tel(%d) %s", device_id, param->pull_rsp_item->contact_tel_len, param->pull_rsp_item->contact_tel);
            }
            // telephone name
            if(param->pull_rsp_item->contact_name_len)
            {
                TRACE(0, "custom_ui d(%x)pbap_pull_item name(%d) %s", device_id, param->pull_rsp_item->contact_name_len, param->pull_rsp_item->contact_name);
            }
            break;
        case BT_PBAP_PULL_STATE_DONE:
            TRACE(0, "custom_ui d(%x)pbap pull done", device_id);
            pbap_search_records[device_id].pull_done = true;
            break;
        case BT_PBAP_PULL_STATE_FINISHED:
            TRACE(0, "custom_ui d(%x)pbap pull finished", device_id);
            pbap_search_records[device_id].pull_finished = true;
            break;
        default:
            TRACE(0, "custom_ui d(%x)pbap unknown search event", device_id);
            pbap_search_records[device_id].pull_finished = true;
            break;
    }
}

static bool process_factory_test_cmd(uint8_t test_type)
{
    bool need_block = false;

    switch (test_type)
    {
        case NV_READ_LE_TEST_RESULT:
        case NV_READ_BT_RX_TEST_RESULT:
            break;
        default:
            if (bta_tws_get_ui_role() == BT_IBRT_SLAVE)
            {
                hal_sw_bootmode_set(HAL_SW_BOOTMODE_CUSTOM_OP1_AFTER_REBOOT);
                app_reset();
                need_block = true;
            }
            break;
    }
    return need_block;
}

WEAK void app_earbuds_init_hook(bt_am_attributes_t *am_attributes, bta_tws_attributes_t *tws_attributes)
{

}

static void fill_am_attributes(bt_am_attributes_t *attributes)
{
    uint32_t default_vol = hal_codec_get_default_dac_volume_index();

#ifdef BT_A2DP_SUPPORT
    attributes->a2dp_default_abs_volume = bta_unsigned_range_value_map(default_vol, TGT_VOLUME_LEVEL_MUTE, TGT_VOLUME_LEVEL_MAX, 0, MAX_A2DP_VOL);
#endif

#ifdef BT_HFP_SUPPORT
    attributes->hfp_default_volume = bta_unsigned_range_value_map(default_vol, TGT_VOLUME_LEVEL_MUTE, TGT_VOLUME_LEVEL_MAX, 0, MAX_HFP_VOL);
#endif

    attributes->a2dp_force_use_the_codec = BT_A2DP_CODEC_TYPE_INVALID;

    attributes->a2dp_force_use_prev_codec = false;

    attributes->hid_capture_non_invade_mode = false;

    attributes->music_preempt_play_mode = false;

    attributes->call_preempt_play_mode = false;

    attributes->dont_auto_play_bg_a2dp = false;

    attributes->second_sco_handle_mode = IBRT_REJECT_SECOND_SCO;

    attributes->a2dp_prompt_delay_ms = 0;

    attributes->bg_a2dp_action = MUTE_BG_A2DP;

    attributes->keep_only_one_stream_close_connected_a2dp = false;

    attributes->pause_a2dp_when_call_exist = false;

    attributes->reconn_sco_if_fast_disc_after_call_active_for_iphone_auto_mode = false;

    attributes->reject_sco_req_within_a_certain_time_after_it_be_preempted = true;

    attributes->host_reject_unexcept_sco_packet = true;

    attributes->second_sco_bg_action = IBRT_ACTION_ROUTE_SCO_TO_PHONE;

    attributes->allow_duck_ringtone = true;

    attributes->virtual_call_handle = VIRTUAL_HANDLE_NORMAL_WAY;

    attributes->create_sco_for_call_active_device = true;

    attributes->pc_second_sco_discon_acl = true;

    attributes->second_sco_handle_mode = IBRT_ACCEPT_SECOND_SCO;

    attributes->dont_resume_music_when_preempted_by_another_music = false;

    attributes->auto_resume_stream_when_focus_empty = false;

    attributes->sco_reject_connect_frist_and_disconnet_later = true;

#if defined(SASS_ENABLED)
    attributes->call_preempt_play_mode = false;

    attributes->second_sco_handle_mode = IBRT_REJECT_SECOND_SCO;
#else
    attributes->call_preempt_play_mode = true;

    attributes->second_sco_bg_action = IBRT_ACTION_ROUTE_SCO_TO_PHONE;

    attributes->second_sco_handle_mode = IBRT_HOST_DECIDE_SECONED_SCO;
#endif
    attributes->prompt_tone_cant_preempt_music = false;

    attributes->resume_pc_sco_by_dis_then_conn_hfp = false;

    attributes->bt_update_active_device_when_incoming_call = false;

    attributes->a2dp_start_select_set_prefer_rate = false;
}

static void fill_tws_attributes(bta_tws_attributes_t *attributes)
{
    //freeman mode config, default should be false
#ifdef FREEMAN_ENABLED_STERO
    attributes->freeman_enable                           = true;
#else
    attributes->freeman_enable                           = false;
#endif

    //enable sdk lea adv strategy, default should be true
    attributes->sdk_lea_adv_enable                       = true;

    //pairing mode timeout value config
    attributes->pairing_timeout_value                    = IBRT_UI_DISABLE_BT_SCAN_TIMEOUT;

    //SDK pairing enable, the default should be true
    attributes->sdk_pairing_enable                       = true;

    //passive enter pairing when no mobile record, the default should be false
#if defined(BESUI_TWS_EN) || defined(BESUI_STEREO_EN)
    attributes->enter_pairing_on_empty_record            = true;
#else
    attributes->enter_pairing_on_empty_record            = false;
#endif

    //passive enter pairing when reconnect failed, the default should be false
#ifdef FREEMAN_ENABLED_STERO
    attributes->enter_pairing_on_reconnect_mobile_failed = true;
#else
#ifdef BESUI_TWS_EN
    attributes->enter_pairing_on_reconnect_mobile_failed = true;
#else
    attributes->enter_pairing_on_reconnect_mobile_failed = false;
#endif
#endif

    //passive enter pairing when mobile disconnect, the default should be false
#if defined(BESUI_TWS_EN) || defined(BESUI_STEREO_EN)
    attributes->enter_pairing_on_mobile_disconnect       = true;
#else
    attributes->enter_pairing_on_mobile_disconnect       = false;
#endif

    //exit pairing when peer close, the default should be true
#if defined(BESUI_TWS_EN) || defined(BESUI_STEREO_EN)
    attributes->exit_pairing_on_peer_close               = false;
#else
    attributes->exit_pairing_on_peer_close               = true;
#endif

    //exit pairing when a2dp or hfp streaming, the default should be true
    attributes->exit_pairing_on_streaming                = true;

    //disconnect remote devices when entering pairing mode actively
    attributes->paring_with_disc_mob_num                 = BTA_TWS_PAIRING_DISC_ONE_MOB;

    //disconnect SCO or not when accepting phone connection in pairing mode
    attributes->pairing_with_disc_hf_cfg                 = BTA_TWS_PAIRING_HF_HUNGUP;

    //pause current music when entering pairing mode
    attributes->pairing_with_pause_music                 = true;

    //not start ibrt in pairing mode, the default should be false
    attributes->pairing_without_start_ibrt               = false;

    //set nv master to tws master for lea connection
    attributes->pairing_with_set_nv_master_as_master     = true;

    //disconnect tws immediately when single bud closed in box
    attributes->disc_tws_imm_on_single_bud_closed        = false;

    //do tws switch when RSII value change, default should be true
    attributes->tws_switch_according_to_rssi_value       = false;

    //do tws switch when RSII value change, timer threshold
    attributes->role_switch_timer_threshold                    = IBRT_UI_ROLE_SWITCH_TIME_THRESHOLD;

    //do tws switch when rssi value change over threshold
    attributes->rssi_threshold                                 = IBRT_UI_ROLE_SWITCH_THRESHOLD_WITH_RSSI;

    //close box delay disconnect tws timeout
    attributes->close_box_delay_tws_disc_timeout               = IBRT_UI_CLOSE_BOX_DEFAULT_DELAY_DISCONNECT_TWS_TIME;

    //close box debounce time config
    attributes->close_box_event_wait_response_timeout          = IBRT_UI_CLOSE_BOX_EVENT_WAIT_RESPONSE_TIMEOUT;

    //wait time before launch reconnect event
    attributes->reconnect_mobile_wait_response_timeout         = IBRT_UI_RECONNECT_MOBILE_WAIT_RESPONSE_TIMEOUT;

    //reconnect event internal config wait timer when tws disconnect
    attributes->reconnect_wait_ready_timeout                   = IBRT_UI_MOBILE_RECONNECT_WAIT_READY_TIMEOUT;//inquiry scan enable timeout when enter paring

    attributes->tws_conn_failed_wait_time                      = TWS_CONN_FAILED_WAIT_TIME;

    attributes->reconnect_mobile_wait_ready_timeout            = IBRT_UI_MOBILE_RECONNECT_WAIT_READY_TIMEOUT;
    attributes->reconnect_tws_wait_ready_timeout               = IBRT_UI_TWS_RECONNECT_WAIT_READY_TIMEOUT;
    attributes->reconnect_ibrt_wait_response_timeout           = IBRT_UI_RECONNECT_IBRT_WAIT_RESPONSE_TIMEOUT;
    attributes->nv_master_reconnect_tws_wait_response_timeout  = IBRT_UI_NV_MASTER_RECONNECT_TWS_WAIT_RESPONSE_TIMEOUT;
    attributes->nv_slave_reconnect_tws_wait_response_timeout   = IBRT_UI_NV_SLAVE_RECONNECT_TWS_WAIT_RESPONSE_TIMEOUT;

    attributes->check_plugin_excute_closedbox_event            = true;
    attributes->ibrt_with_ai                                   = false;

    //if tws&mobile disc, reconnect tws first until tws connected then reconn mobile
    attributes->delay_reconn_mob_until_tws_connected           = false;

    attributes->delay_reconn_mob_max_times                     = IBRT_UI_DELAY_RECONN_MOBILE_MAX_TIMES;
    attributes->merge_nv_scheme                                = BTA_TWS_NV_MERGE_LOCAL_FIRST;

    //open box reconnect mobile times config
    attributes->open_reconnect_mobile_max_times                = IBRT_UI_OPEN_RECONNECT_MOBILE_MAX_TIMES;

    //open box reconnect tws times config
    attributes->open_reconnect_tws_max_times                   = IBRT_UI_OPEN_RECONNECT_TWS_MAX_TIMES;

    //connection timeout reconnect mobile times config
    attributes->reconnect_mobile_max_times                     = IBRT_UI_RECONNECT_MOBILE_MAX_TIMES;

    //connection timeout reconnect tws times config
    attributes->reconnect_tws_max_times                        = IBRT_UI_RECONNECT_TWS_MAX_TIMES;

    //connection timeout reconnect ibrt times config
    attributes->reconnect_ibrt_max_times                       = IBRT_UI_RECONNECT_IBRT_MAX_TIMES;

    //controller basband monitor
    attributes->lowlayer_monitor_enable                        = true;
    attributes->llmonitor_report_format                        = REP_FORMAT_PACKET;
    attributes->llmonitor_report_count                         = 1000;
  //  attributes->llmonitor_report_format                      = REP_FORMAT_TIME;
  //  attributes->llmonitor_report_count                       = 1600;////625us uint

    attributes->mobile_page_timeout                            = IBRT_MOBILE_PAGE_TIMEOUT;
    //tws connection supervision timeout
    attributes->tws_connection_timeout                         = IBRT_UI_TWS_CONNECTION_TIMEOUT;

    attributes->rx_seq_error_timeout                           = IBRT_UI_RX_SEQ_ERROR_TIMEOUT;
    attributes->rx_seq_error_threshold                         = IBRT_UI_RX_SEQ_ERROR_THRESHOLD;
    attributes->rx_seq_recover_wait_timeout                    = IBRT_UI_RX_SEQ_ERROR_RECOVER_TIMEOUT;

    attributes->rssi_monitor_timeout                           = IBRT_UI_RSSI_MONITOR_TIMEOUT;

    attributes->connect_no_03_timeout                          = CONNECT_NO_03_TIMEOUT;
    attributes->disconnect_no_05_timeout                       = DISCONNECT_NO_05_TIMEOUT;

    attributes->tws_switch_tx_data_protect                     = true;

    attributes->tws_cmd_send_timeout                           = IBRT_UI_TWS_CMD_SEND_TIMEOUT;
    attributes->tws_cmd_send_counter_threshold                 = IBRT_UI_TWS_COUNTER_THRESHOLD;

    attributes->profile_concurrency_supported                  = true;

    attributes->audio_sync_mismatch_resume_version             = 2;

#ifdef APP_MULTIPOINT_ONOFF_EN
    attributes->support_steal_connection                       = false;
#else
    attributes->support_steal_connection                       = true;
#endif
    attributes->support_steal_connection_in_sco                = false;
    attributes->support_steal_connection_in_a2dp_steaming      = true;
    attributes->steal_audio_inactive_device                    = false;

    attributes->allow_sniff_in_sco                             = false;

    attributes->always_interlaced_scan                         = true;

    attributes->disallow_reconnect_bt_device_in_streaming_state = false;

    attributes->disallow_reconnect_tws_in_streaming_state      = false;

    attributes->open_without_auto_reconnect                    = false;

    attributes->without_reconnect_if_remote_initiate_disc      = false;
#if defined(BESUI_TWS_EN) || defined(BESUI_STEREO_EN)
    attributes->without_reconnect_when_fetch_out_wear_up       = true;
#else
    attributes->without_reconnect_when_fetch_out_wear_up       = false;
#endif

    attributes->reject_same_cod_device_conn_req                = false;

#ifdef IBRT_UI_MASTER_ON_TWS_DISCONNECTED
    attributes->is_changed_to_ui_master_on_tws_disconnected    = true;
#else
    attributes->is_changed_to_ui_master_on_tws_disconnected    = false;
#endif

    attributes->connected_max_device_num_allow_new_connect     = true;

    attributes->lea_connected_allow_new_connect                = true;

    attributes->dev_idle_allow_nonsupport_stay_connected       = true;

    attributes->nv_merge_scope                                 = BTA_TWS_NV_MERGE_SCOPE_MINIMAL;

    attributes->afh_assess_policy                              = BTA_TWS_AFH_ASSESS_POLICY_ON_STREAMING;
}

void app_bta_init(void)
{
    bt_am_attributes_t am_attributes = {0};
    fill_am_attributes(&am_attributes);

    bta_tws_attributes_t tws_attributes = {0};
    fill_tws_attributes(&tws_attributes);

    app_earbuds_init_hook(&am_attributes, &tws_attributes);

    const bta_tws_bt_link_state_changed_t link_state_changed =
    {
        .bt_link_state_changed = bt_link_state_changed_handler,
        .bt_profile_exchanged = bt_profile_exchanged_handler,
    };
    bta_tws_register_bt_link_state_changed_hook(BTA_TWS_UX_USER_APP, &link_state_changed);

    const bta_tws_bt_callbacks_t bt_callbacks =
    {
        .bt_access_mode_changed = bt_access_mode_changed_handler,
        .bt_link_mode_changed = bt_link_mode_changed_handler,
        .remote_name_request_completed = remote_name_request_completed_handler,
    };
    bta_tws_register_bt_callbacks(BTA_TWS_UX_USER_APP, &bt_callbacks);

    static bt_a2dp_sink_callbacks_t a2dp_sink_callbacks =
    {
        .connection_state_cb = a2dp_connection_state_changed_handler,
        .audio_state_cb = a2dp_audio_state_changed_handler,
    };
    bta_a2dp_register_callbacks(BT_A2DP_SINK_CB_USER_APP, &a2dp_sink_callbacks);

    static bt_hfp_hf_callbacks_t hfp_hf_callbacks =
    {
        .connection_state_cb = hfp_connection_state_changed_handler,
        .audio_status_cb = hfp_audio_status_changed_handler,
        .current_calls_cb = hfp_query_current_calls_handler,
    };
    bta_hf_register_callbacks(BT_HFP_HF_CB_USER_APP, &hfp_hf_callbacks);

    static bt_pbap_callbacks_t pbap_callbacks =
    {
        .connection_state_cb    = pbap_connection_state_changed_handler,
        .search_result_cb       = pbap_search_result_handler,
    };
    bta_pbap_register_callbacks(BT_PBAP_CB_USER_APP, &pbap_callbacks);

    const bta_tws_ui_state_changed_t ui_state_changed =
    {
        .bt_subsys_state_changed = bt_subsys_state_changed_handler,
        .tws_sync_state_changed = tws_link_state_changed_handler,
        .pairing_mode_changed = pairing_mode_changed_handler,
        .peer_box_state_changed = peer_box_state_changed_handler,
        .local_box_state_changed = local_box_state_changed_handler,
        .ui_role_switch_state_changed = tws_role_switch_state_changed_handler,
    };
    bta_tws_register_ui_state_changed_hook(BTA_TWS_UX_USER_APP, &ui_state_changed);

    bta_tws_init(&am_attributes, &tws_attributes);

    app_bta_earbuds_deprecated_init();

    app_ibrt_customif_register_cmd_table();

    app_factory_register_test_ind_callback(process_factory_test_cmd);

#if defined (BLE_HOST_SUPPORT) || defined (__GATT_OVER_BR_EDR__)
#ifdef CFG_APP_DATAPATH_SERVER
    app_datapaths_init();
#endif /* CFG_APP_DATAPATH_SERVER */
#endif /* BLE_HOST_SUPPORT || __GATT_OVER_BR_EDR__*/

#ifdef CFG_APP_DATAPATH_CLIENT
    app_datapathc_init();
#endif /* CFG_APP_DATAPATH_CLIENT */

#ifdef BLE_HOST_SUPPORT
    app_ble_mgr_init(APP_APPEARANCE_VALUE_EARBUDS);

#if (defined(BES_OTA) || defined(BES_OTA_BASIC)) && !defined(OTA_OVER_TOTA_ENABLED) && !defined(NUTTX_BLE_OTA)
    app_ble_ota_init();
#endif

#if defined(SWIFT_ENABLED)
    app_ble_swift_init();
#endif

#if defined (BIS_SELFSCAN_ENABLED)
    app_bis_selfscan_init(NULL, NULL);
#endif

#if defined (BLE_BATT_ENABLE)
    app_ble_batt_init();
#endif

#if defined (BLE_TOTA_ENABLED)
    app_ble_tota_init();
#endif

#if defined (BLE_DIP_ENABLE)
    app_ble_dis_init();
#endif

#endif /* BLE_HOST_SUPPORT */

    bta_sep_codec_config_init();

    app_bta_cmd_init();
}

bool app_bta_bootmode_handler(void)
{
    uint32_t boot_mode = hal_sw_bootmode_get();
    if (boot_mode & HAL_SW_BOOTMODE_CUSTOM_OP1_AFTER_REBOOT)
    {
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_CUSTOM_OP1_AFTER_REBOOT);
        bta_tws_enable_freeman_mode(true);
        return false;
    }
    else if (boot_mode & HAL_SW_BOOTMODE_CUSTOM_OP2_AFTER_REBOOT)
    {
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_CUSTOM_OP2_AFTER_REBOOT);
#ifdef IBRT_UI
        bta_tws_box_event_entry(BTA_TWS_OPEN);
#endif
        return false;
    }
    else if (boot_mode & HAL_SW_BOOTMODE_TEST_NORMAL_MODE)
    {
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_TEST_NORMAL_MODE);
        bta_tws_enable_freeman_mode(true);
        return false;
    }
    else
    {
        return true;
    }
}
