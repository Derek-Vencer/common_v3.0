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
#include "app_headset.h"
#include "app_headset_cmd.h"

#include "cmsis_os2.h"

#include "apps.h"
#include "hal_codec.h"
#include "hal_trace.h"
#include "hal_bootmode.h"

#include "bta_bt_api.h"
#include "earbud_ux_api.h"
#include "sep_codec_config.h"
#include "bta_normal_ux_api.h"

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
#include "app_ble_battery.h"
#include "nvrecord_ble.h"
#include "app_ble_tota.h"
#include "app_ble_dis.h"
#include "app_bis_selfscan.h"
#endif

#ifdef GFPS_ENABLED
#include "gfps.h"
#endif

#ifdef __AI_VOICE__
#include "ai_spp.h"
#endif

#ifdef SWIFT_ENABLED
#include "app_ble_swift.h"
#endif

#ifndef TRACE
#define TRACE(attr, str, ...)               TR_INFO(attr, str, ##__VA_ARGS__)
#endif

#define APP_APPEARANCE_VALUE_HEADSET    (0x0942)

static app_headset_attributes_t g_headset_attributes;
static osTimerId_t g_pairing_mode_timer;

#if defined(BIS_SELFSCAN_ENABLED)
static app_bis_selfscan_evt_cb_t g_bis_selfscan_callbacks;

static void bis_selfscan_scan_state_callback(bool started)
{
    TRACE(0, "BIS self-scan state changed: %s", started ? "started" : "stopped");
}

static void bis_selfscan_scan_result_callback(uint8_t src_lid, const struct app_bis_src_scan_result *p_result)
{
    const uint8_t expected_broadcast_id[3] = {0x01, 0x02, 0x03};

    TRACE(0, "BIS self-scan result received, src_lid: %d", src_lid);

    if (p_result != NULL && memcmp(p_result->broadcast_id, expected_broadcast_id, sizeof(expected_broadcast_id)) == 0)
    {
        app_bis_selfscan_enable_scan(false);
        app_bis_selfscan_select_sync_source(src_lid, NULL);
        TRACE(0, "Broadcast ID matches start sync source");
    }
    else
    {
        TRACE(0, "Broadcast ID does not match expected value, ignoring result");
    }
}

static void bis_selfscan_sync_state_callback(const ble_bdaddr_t *p_src_addr, uint8_t adv_sid, enum selfscan_sync_state sync_state, uint16_t error)
{
    TRACE(0, "BIS self-scan sync state: %d, error: %d", sync_state, error);
}

static void bis_selfscan_bcast_code_req_callback(uint8_t src_lid)
{
    TRACE(0, "BIS self-scan broadcast code request for src_lid: %d", src_lid);
}

static void bis_selfscan_stream_state_callback(bool started)
{
    TRACE(0, "BIS self-scan stream state: %s", started ? "started" : "stopped");
}

static void app_ble_init_bis_selfscan_callbacks(void)
{
    g_bis_selfscan_callbacks.cb_scan_state = bis_selfscan_scan_state_callback;
    g_bis_selfscan_callbacks.cb_scan_result = bis_selfscan_scan_result_callback;
    g_bis_selfscan_callbacks.cb_sync_state = bis_selfscan_sync_state_callback;
    g_bis_selfscan_callbacks.cb_bcast_code_req = bis_selfscan_bcast_code_req_callback;
    g_bis_selfscan_callbacks.cb_stream_state = bis_selfscan_stream_state_callback;
}
#endif

void app_headset_set_without_reconnect_when_fetch_out_wear_up(bool value)
{
    g_headset_attributes.without_reconnect_when_fetch_out_wear_up = value;
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

static void fill_ux_attributes(bta_ux_attributes_t *attributes)
{
    attributes->enter_pairing_on_empty_record = true;
    attributes->enter_pairing_on_reconnect_mobile_failed = false;
    attributes->reconnect_mobile_max_times = 32;
    attributes->paring_with_disc_mob_num = BTA_PAIRING_DISC_ONE_MOB;
}

WEAK void app_headset_init_hook(bt_am_attributes_t *am_attributes, bta_ux_attributes_t *ux_attributes, app_headset_attributes_t *headset_attributes)
{

}

static void bt_acl_connection_state_handler(const bt_bdaddr_t *addr, bt_connection_state_t state, uint8_t error_code)
{
    POSSIBLY_UNUSED uint8_t device_id = bta_get_device_id_by_addr(addr);

    switch (state)
    {
        case BT_CONN_STATE_DISCONNECTED:
#if defined(SNDP_VAD_ENABLE)
            app_sensor_hub_sndp_mcu_request_vad_stop();
#endif

#ifdef GFPS_ENABLED
            gfps_link_disconnect_handler(SET_BT_ID(device_id), addr, error_code);
#endif
        break;
        case BT_CONN_STATE_CONNECTED:
        break;
    }
}

static void bt_encrypt_changed_handler(const bt_bdaddr_t *addr, bool encrypted, uint8_t error_code)
{
    POSSIBLY_UNUSED uint8_t device_id = bta_get_device_id_by_addr(addr);

    if (encrypted && !error_code)
    {
#if defined(SNDP_VAD_ENABLE)
            app_sensor_hub_sndp_mcu_request_vad_start();
#endif

#ifdef GFPS_ENABLED
            gfps_link_connect_handler(SET_BT_ID(device_id), addr);
#endif

#ifdef FINDMY_ENABLED
            findmy_bt_connected_handler(addr);
#endif
    }
}

static void disconnect_channels_when_basic_profiles_disconncted(const bt_bdaddr_t *addr)
{
    POSSIBLY_UNUSED uint8_t device_id = bta_get_device_id_by_addr(addr);

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
}

static void a2dp_connection_state_changed_handler(const bt_bdaddr_t *addr, bt_a2dp_conn_state_t state, uint8_t error_code)
{
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
    TRACE(0, "headset: a2dp audio state: %d", state);
    switch (state)
    {
        case BT_A2DP_AUDIO_STATE_CLOSED:
        break;
        case BT_A2DP_AUDIO_STATE_SUSPEND:
        break;
        case BT_A2DP_AUDIO_STATE_STARTED:
        break;
    }
}

static void hfp_connection_state_changed_handler(const bt_bdaddr_t *addr, bt_hfp_conn_state_t state, uint8_t error_code)
{
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
    switch (state)
    {
        case BT_HFP_AUDIO_STATE_DISCONNECTED:
        break;
        case BT_HFP_AUDIO_STATE_CONNECTED:
        break;
    }
}

static void bt_subsys_state_changed_handler(bta_subsys_state_t state)
{
    TRACE(0, "headset: bt_subsys %d", state);
}

static void pairing_mode_entered_handler()
{
#ifdef GFPS_ENABLED
    gfps_enter_fastpairing_mode();
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
        osTimerStart(g_pairing_mode_timer, g_headset_attributes.pairing_timeout_value);
    }
    else
    {
        pairing_mode_exited_handler();
        osTimerStop(g_pairing_mode_timer);
    }
}

static void pairing_mode_timeout(void *argument)
{
    bta_enable_pairing_mode(false);
}

static bool security_check_override_for_bt(const bt_bdaddr_t *addr)
{
    TRACE(0, "headset: security_check bt");

    // Return true here if you don't care about security check.
    return false;
}

static bool security_check_override_for_le(const ble_bdaddr_t *addr)
{
    TRACE(0, "headset: security_check le");

    return false;
}

#define PAGE_COUNT_WHEN_OPEN    2
#define PAGE_COUNT_WHEN_WEAR_UP 1

#define ADV_DURATION_WHEN_OPEN  20000
#define ADV_DURATION_WHEN_WEAR_UP 1000

void app_bta_init(void)
{
    g_headset_attributes.pairing_timeout_value = 120000;
    g_headset_attributes.open_reconnect_mobile_max_times = PAGE_COUNT_WHEN_OPEN;
    g_headset_attributes.without_reconnect_when_fetch_out_wear_up = false;

    bt_am_attributes_t am_attributes = {};
    fill_am_attributes(&am_attributes);
    bta_ux_attributes_t ux_attributes = {};
    fill_ux_attributes(&ux_attributes);
    app_headset_init_hook(&am_attributes, &ux_attributes, &g_headset_attributes);

    g_pairing_mode_timer = osTimerNew(pairing_mode_timeout, osTimerOnce, NULL, NULL);

    static bt_callbacks_t bt_callbacks =
    {
        .acl_connection_state_cb = bt_acl_connection_state_handler,
        .encrypt_changed_cb = bt_encrypt_changed_handler,
    };
    bta_bt_register_callbacks(BT_CB_USER_APP, &bt_callbacks);

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
    };
    bta_hf_register_callbacks(BT_HFP_HF_CB_USER_APP, &hfp_hf_callbacks);

    const bta_ui_state_changed_t ui_state_changed =
    {
        .bt_subsys_state_changed = bt_subsys_state_changed_handler,
        .pairing_mode_changed = pairing_mode_changed_handler,
    };
    bta_register_ui_state_changed_hook(BTA_UX_USER_APP, &ui_state_changed);

    const bta_security_check_override_callbacks_t security_check_override_callbacks =
    {
        .connection_request = security_check_override_for_bt,
        .bt_pairing = security_check_override_for_bt,
        .le_smp_pairing = security_check_override_for_le,
    };
    bta_register_security_check_override_callback(BTA_UX_USER_APP, &security_check_override_callbacks);

    bta_normal_init(&am_attributes, &ux_attributes);

    // Call this to disable preemption if not required.
    // bta_config_preempt(false);

    app_bta_earbuds_deprecated_init();

    app_headset_cmd_init();

#if defined (BLE_HOST_SUPPORT) || defined (__GATT_OVER_BR_EDR__)
#ifdef CFG_APP_DATAPATH_SERVER
    app_datapaths_init();
#endif /* CFG_APP_DATAPATH_SERVER */

#ifdef CFG_APP_DATAPATH_CLIENT
    app_datapathc_init();
#endif

#endif /* BLE_HOST_SUPPORT || __GATT_OVER_BR_EDR__*/

#ifdef BLE_HOST_SUPPORT
    app_ble_mgr_init(APP_APPEARANCE_VALUE_HEADSET);

#if (defined(BES_OTA) || defined(BES_OTA_BASIC)) && !defined(OTA_OVER_TOTA_ENABLED) && !defined(NUTTX_BLE_OTA)
    app_ble_ota_init();
#endif

#if defined(SWIFT_ENABLED)
    app_ble_swift_init();
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

#if defined (BIS_SELFSCAN_ENABLED)
    app_ble_init_bis_selfscan_callbacks();
    app_bis_selfscan_init(NULL, &g_bis_selfscan_callbacks);
#endif

#endif /* BLE_HOST_SUPPORT */

    bta_sep_codec_config_init();
}

bool app_bta_bootmode_handler(void)
{
    uint32_t boot_mode = hal_sw_bootmode_get();
    if (boot_mode & HAL_SW_BOOTMODE_CUSTOM_OP1_AFTER_REBOOT)
    {
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_CUSTOM_OP1_AFTER_REBOOT);
        return false;
    }
    else if (boot_mode & HAL_SW_BOOTMODE_CUSTOM_OP2_AFTER_REBOOT)
    {
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_CUSTOM_OP2_AFTER_REBOOT);
        return false;
    }
    else if (boot_mode & HAL_SW_BOOTMODE_TEST_NORMAL_MODE)
    {
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_TEST_NORMAL_MODE);
        return false;
    }
    else
    {
        return true;
    }
}

void app_bta_box_event_entry(app_headset_box_event_t event)
{
    switch (event)
    {
        case APP_HEADSET_OPEN:
            bta_ux_open(bta_get_device_num_max(), g_headset_attributes.open_reconnect_mobile_max_times, 0, ADV_DURATION_WHEN_OPEN);
        break;
        case APP_HEADSET_UNDOCK:
        case APP_HEADSET_WEAR_UP:
            if (!g_headset_attributes.without_reconnect_when_fetch_out_wear_up)
            {
                bta_connect_all_bt_devices(PAGE_COUNT_WHEN_WEAR_UP, 0);
                bta_connect_all_lea_device(ADV_DURATION_WHEN_WEAR_UP);
            }
        break;
        case APP_HEADSET_DOCK:
        case APP_HEADSET_WEAR_DOWN:
        break;
        case APP_HEADSET_CLOSE:
            bta_ux_close();
        break;
    }
}

#if BLE_AUDIO_ENABLED
static void app_bta_bis_stream_state_cb(bool state, uint8_t role)
{
    TRACE(0, "LEA BIS stream state changed: state=%d, role=%d", state, role);
}

static void app_bta_bis_param_set_cb(bta_bis_cfg_param_t *param)
{
    param->broadcast_id[0] = 0x01;
    param->broadcast_id[1] = 0x02;
    param->broadcast_id[2] = 0x03;


    param->encrypt_enable = 0;
    memset(param->encrypt_key, 0, 16);

    param->adv_mfrs_data_len = 0;
    param->adv_mfrs_data = NULL;

    param->sync_policy = 0;
    param->company_id = 0x02B0;
    param->bcast_name = "LEA_BIS_Source";

    TRACE(0, "LEA BIS parameters configured successfully");
}

void app_bta_bis_src_set_audio_type(bta_input_audio_type audio_type)
{
    bta_lea_bis_src_set_audio_type(audio_type);
}

void app_bta_bis_source_open()
{
    bta_bis_cfg_param_t cfg_param = {0};

    cfg_param.bt_svc_bis_stream_state_cb = app_bta_bis_stream_state_cb;
    cfg_param.bt_svc_bis_param_set_cb = app_bta_bis_param_set_cb;

    bta_lea_bis_src_open(&cfg_param);
}

void app_bta_bis_source_close()
{
    bta_lea_bis_src_close();
}
#endif