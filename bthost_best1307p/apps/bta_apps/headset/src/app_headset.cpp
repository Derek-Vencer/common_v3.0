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

#ifdef BLE_HOST_SUPPORT
#include "app_ble_mgr.h"
#include "app_ble_ota.h"
#include "app_ble_swift.h"
#endif

#ifdef GFPS_ENABLED
#include "gfps.h"
#endif

#ifdef __AI_VOICE__
#include "ai_spp.h"
#endif

#ifndef TRACE
#define TRACE(attr, str, ...)               TR_INFO(attr, str, ##__VA_ARGS__)
#endif

#define APP_APPEARANCE_VALUE_HEADSET    (0x0942)

static app_headset_attributes_t g_headset_attributes;
static osTimerId_t g_pairing_mode_timer;
static bta_page_policy_t g_page_policy;

void app_headset_set_without_reconnect_when_fetch_out_wear_up(bool value)
{
    g_headset_attributes.without_reconnect_when_fetch_out_wear_up = value;
}
static void fill_am_attributes(bt_am_attributes_t *attributes)
{
    uint32_t default_vol = hal_codec_get_default_dac_volume_index();

#ifdef BT_A2DP_SUPPORT
    attributes->a2dp_default_abs_volume = unsigned_range_value_map(default_vol, TGT_VOLUME_LEVEL_MUTE, TGT_VOLUME_LEVEL_MAX, 0, MAX_A2DP_VOL);
#endif

#ifdef BT_HFP_SUPPORT
    attributes->hfp_default_volume = unsigned_range_value_map(default_vol, TGT_VOLUME_LEVEL_MUTE, TGT_VOLUME_LEVEL_MAX, 0, MAX_HFP_VOL);
#endif

    attributes->a2dp_force_use_the_codec = BT_A2DP_CODEC_TYPE_INVALID;

    attributes->a2dp_force_use_prev_codec = false;

    attributes->hid_capture_non_invade_mode = false;

    attributes->music_preempt_play_mode = false;

    attributes->call_preempt_play_mode = false;

    attributes->dont_auto_play_bg_a2dp = false;

    attributes->second_sco_handle_mode = IBRT_REJECT_SECOND_SCO;

    attributes->a2dp_prompt_play_only_when_avrcp_play_received = false;

    attributes->a2dp_delay_prompt_play = false;

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
}

static void fill_ux_attributes(bta_ux_attributes_t *attributes)
{
    attributes->enter_pairing_on_empty_record = true;
    attributes->enter_pairing_on_reconnect_mobile_failed = false;
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
    // TODO: pause music

#ifdef GFPS_ENABLED
        gfps_enter_fastpairing_mode();
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

    bt_drv_reg_op_afh_assess_en(true);

    app_headset_cmd_init();

#if defined (BLE_HOST_SUPPORT) || defined (__GATT_OVER_BR_EDR__)
#ifdef CFG_APP_DATAPATH_SERVER
    app_datapaths_init();
#endif /* CFG_APP_DATAPATH_SERVER */
#endif /* BLE_HOST_SUPPORT || __GATT_OVER_BR_EDR__*/

#ifdef BLE_HOST_SUPPORT
    app_ble_mgr_init(APP_APPEARANCE_VALUE_HEADSET);

#if (defined(BES_OTA) || defined(BES_OTA_BASIC)) && !defined(OTA_OVER_TOTA_ENABLED) && !defined(NUTTX_BLE_OTA)
    app_ble_ota_init();
#endif

#if defined(SWIFT_ENABLED)
    app_ble_swift_init();
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
            bta_ux_open(BT_DEVICE_NUM, g_headset_attributes.open_reconnect_mobile_max_times, 0, ADV_DURATION_WHEN_OPEN);
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
