/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#include "cmsis_os.h"
#include <string.h>
#include "bluetooth_bt_api.h"
#include "app_ibrt_keyboard.h"
#include "hal_trace.h"
#include "bts_am_api.h"
#include "btapp.h"
#include "apps.h"
#include "app_bt.h"
#include "app_hfp.h"
#include "app_ibrt_debug.h"
#include "bts_module_if.h"
#include "bta_tws_ux_api.h"
#include "bta_tws_audio_api.h"
#include "app_ibrt_search_pair_ui.h"
#ifdef __AI_VOICE__
#include "ai_manager.h"
#endif
// #include "ea.rbud_ux_api.h"


#if defined(BT_HID_DEVICE)
#include "app_hid_device.h"
#endif
#ifdef BESUI_KEY_EN
#include "twsui_key.h"
#endif

#if defined(IBRT)

#ifdef TILE_DATAPATH
extern "C" void app_tile_key_handler(APP_KEY_STATUS *status, void *param);
#endif

extern void app_otaMode_enter(APP_KEY_STATUS *status, void *param);

#ifdef SUPPORT_SIRI
extern uint8_t voice_assistant_flag;
#endif

#ifdef APP_KEY_ENABLE
void app_ibrt_handle_longpress_v2(APP_KEY_STATUS *status)
{
#ifdef BT_HFP_SUPPORT
    // switch between sco streaming devices
    uint8_t playing_sco = bts_am_get_curr_playing_sco();
    uint8_t sco_count = bts_am_count_connected_sco();
    if (playing_sco != BT_DEVICE_INVALID_ID)
    {
        if((sco_count > 1 || bts_am_select_another_device_to_create_sco(playing_sco) != BT_DEVICE_INVALID_ID))
        {
            bta_tws_switch_streaming_sco();
        }
        return;
    }
#endif

    // switch between a2dp streaming devices
#ifdef BT_A2DP_SUPPORT
    uint8_t a2dp_count = bts_am_count_streaming_a2dp();
    if (a2dp_count > 1)
    {
        bta_tws_switch_streaming_a2dp();
        return;
    }
#endif
}

void app_ibrt_middleware_handle_click(void)
{
    EARBUDS_TRACE(1, "%s", __func__);
#if defined(__AI_VOICE__) || defined(BISTO_ENABLED)
    if (bta_tws_get_ui_role() == BT_IBRT_MASTER)
    {
        bta_tws_request_ui_role_switch(BT_IBRT_SLAVE);
    }
    else
    {
        bta_tws_request_ui_role_switch(BT_IBRT_MASTER);
    }
#else
    bt_key_handle_func_click();
#endif
}

#ifdef IBRT_SEARCH_UI
void app_ibrt_search_ui_handle_key_v2(bt_bdaddr_t *remote, APP_KEY_STATUS *status, void *param)
{
    EARBUDS_TRACE(0,"%s %d,%d",__func__, status->code, status->event);


    if (APP_KEY_CODE_GOOGLE != status->code)
    {
        switch(status->event)
        {
            case APP_KEY_EVENT_CLICK:
                app_ibrt_middleware_handle_click();
                break;

            case APP_KEY_EVENT_DOUBLECLICK:
                EARBUDS_TRACE(0,"double kill");
                if(BT_IBRT_UNKNOWN == bts_tws_if_get_nv_role())
                {
                    bta_tws_box_event_entry(BTA_TWS_OPEN);
                    app_start_tws_serching_direactly();
                }
                else
                {
                    bt_key_handle_func_doubleclick();
                }
                break;

            case APP_KEY_EVENT_LONGPRESS:
                break;

            case APP_KEY_EVENT_TRIPLECLICK:
            #ifdef TILE_DATAPATH
                app_tile_key_handler(status,NULL);
            #else
                app_otaMode_enter(NULL,NULL);
            #endif
                break;
            case HAL_KEY_EVENT_LONGLONGPRESS:
                EARBUDS_TRACE(0,"long long press");
                app_shutdown();
                break;

            case APP_KEY_EVENT_ULTRACLICK:
                EARBUDS_TRACE(0,"ultra kill");
                break;

            case APP_KEY_EVENT_RAMPAGECLICK:
                EARBUDS_TRACE(0,"rampage kill!you are crazy!");
                break;

            case APP_KEY_EVENT_UP:
                break;
        }
    }

#ifdef TILE_DATAPATH
    if(APP_KEY_CODE_TILE == status->code)
        app_tile_key_handler(status,NULL);
#endif
}
#else
#ifdef GFPS_ENABLED
extern "C" void app_enter_fastpairing_mode(void);
#endif
void app_ibrt_normal_ui_handle_key_v2(bt_bdaddr_t *remote, APP_KEY_STATUS *status, void *param)
{
    uint8_t conn_devices = 0;
    if (APP_KEY_CODE_GOOGLE != status->code)
    {
        switch(status->event)
        {
            case APP_KEY_EVENT_CLICK:
                EARBUDS_TRACE(0,"first blood.");
                app_ibrt_middleware_handle_click();
                break;

            case APP_KEY_EVENT_DOUBLECLICK:
                EARBUDS_TRACE(0,"double kill,enter freeman mode");
                //app_ibrt_if_init_open_box_state_for_evb();
                bta_tws_enable_freeman_mode(true);
#ifdef GFPS_ENABLED
                app_enter_fastpairing_mode();
#endif
                break;
            case APP_KEY_EVENT_LONGPRESS:
                conn_devices = app_bt_count_connected_device();
                EARBUDS_TRACE(0,"%s conn_devices %d", __func__, conn_devices);
                if (conn_devices > 0)
                {
                    app_ibrt_handle_longpress_v2(status);
                }
                else
                {
                    bta_tws_box_event_entry(BTA_TWS_OPEN);
                    bta_tws_enable_pairing_mode(true);
                }
                break;

            case APP_KEY_EVENT_TRIPLECLICK:
#ifdef TILE_DATAPATH
                app_tile_key_handler(status,NULL);
#else
                bta_tws_box_event_entry(BTA_TWS_OPEN);
                bta_tws_enable_pairing_mode(true);
#endif
                break;

            case HAL_KEY_EVENT_LONGLONGPRESS:
                EARBUDS_TRACE(0,"long long press");
                app_shutdown();
                break;

            case APP_KEY_EVENT_ULTRACLICK:
                EARBUDS_TRACE(0,"ultra kill");
                break;

            case APP_KEY_EVENT_RAMPAGECLICK:
                EARBUDS_TRACE(0,"rampage kill!you are crazy!");
                break;

            case APP_KEY_EVENT_UP:
                break;
        }
    }

#ifdef TILE_DATAPATH
    if(APP_KEY_CODE_TILE == status->code)
        app_tile_key_handler(status,NULL);
#endif
}
#endif

struct ibrt_keyboard_notify_v2_t
{
    bt_bdaddr_t remote;
    APP_KEY_STATUS key_status;
};

int app_ibrt_keyboard_notify_v2(bt_bdaddr_t *remote, APP_KEY_STATUS *status, void *param)
{
    struct ibrt_keyboard_notify_v2_t data;
    if (remote && bts_ibrt_if_is_ibrt_link_connected(remote)) {
        data.remote = *remote;
    }
    else {
        data.remote = {0};
    }

    data.key_status = *status;
    tws_ctrl_send_cmd(APP_TWS_CMD_KEYBOARD_REQUEST, (uint8_t *)&data, sizeof(struct ibrt_keyboard_notify_v2_t));

    return 0;
}
#endif


void app_ibrt_keyboard_request_handler_v2(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
#ifdef APP_KEY_ENABLE

    struct ibrt_keyboard_notify_v2_t *req = (struct ibrt_keyboard_notify_v2_t *)p_buff;

#ifdef BESUI_KEY_EN
    app_key_gui_post_msg(true, req->key_status.code, req->key_status.event);
#else
    if (bts_bt_if_is_dev_link_connected(&req->remote))
    {
#ifdef IBRT_SEARCH_UI
        app_ibrt_search_ui_handle_key_v2(&req->remote, &req->key_status, NULL);
#else
        app_ibrt_normal_ui_handle_key_v2(&req->remote, &req->key_status, NULL);
#endif
    }
#endif
#endif
}

#ifdef TOTA_v2
#define IBRT_TOTA_DAT_MAX_SIZE 200
uint8_t g_tota_data_action[IBRT_TOTA_DAT_MAX_SIZE];
bool app_spp_tota_send_data(uint8_t* ptrData, uint16_t length);
#endif

#if defined(BT_HID_DEVICE)
struct bt_hid_sensor_data_t {
    struct ibrt_if_action_header header;
    struct hid_sensor_report_t report;
};
#endif

void app_ibrt_if_start_user_action_v2(uint8_t device_id, uint8_t action, uint32_t param, uint32_t param2)
{
    struct BT_DEVICE_T *curr_device = app_bt_get_device(device_id);
    uint16_t action_length = 0;
    uint8_t *action_data = NULL;
    struct ibrt_if_action_header action_header;
#if defined(BT_HID_DEVICE)
    struct bt_hid_sensor_data_t sensor_data;
#endif

    if (curr_device == NULL)
    {
        return;
    }

    action_header.action = action;
    action_header.remote = curr_device->remote;
    action_header.param = param;
    action_header.param2 = param2;

    action_data = (uint8_t *)&action_header;
    action_length = sizeof(action_header);

#ifdef TOTA_v2
    if (action == IBRT_ACTION_SEND_TOTA_DATA)
    {
        memcpy(g_tota_data_action, (uint8_t *)&action_header, action_length);
        if (action_length + param2 > IBRT_TOTA_DAT_MAX_SIZE)
        {
            EARBUDS_TRACE(0,"send tota data length too long %d", param2);
            return;
        }
        memcpy(g_tota_data_action+action_length, (void*)(uintptr_t)param, param2);
        action_data = g_tota_data_action;
        action_length += param2;
    }
#endif

#if defined(BT_HID_DEVICE)
    if (action == IBRT_ACTION_HID_SENSOR_REPORT)
    {
        sensor_data.header = action_header;
        sensor_data.report = *((struct hid_sensor_report_t *)(uintptr_t)param);
        action_data = (uint8_t *)&sensor_data;
        action_length = sizeof(sensor_data);
    }
#endif

    if (bts_tws_if_is_besaud_connnected() && bts_ibrt_if_is_ibrt_link_connected(&curr_device->remote))
    {
        bts_tws_if_send_user_action(action_data, action_length);
    }
    else
    {
        app_ibrt_ui_perform_user_action_v2(action_data, action_length);
    }
}

void app_ibrt_keyboard_sync_volume_info_v2(uint8_t device_id)
{
    struct BT_DEVICE_T *curr_device = app_bt_get_device(device_id);

    if(curr_device && curr_device->acl_is_connected && bts_tws_if_is_tws_link_connected())
    {
        TWS_VOLUME_SYNC_INFO_T_V2 volume_info;

        volume_info.remote = curr_device->remote;

        volume_info.a2dp_local_volume = a2dp_volume_local_get(device_id);

        volume_info.hfp_local_volume = hfp_volume_local_get(device_id);

        EARBUDS_TRACE(0,"(d%x) %s: a2dp %d hfp %d", device_id, __func__, volume_info.a2dp_local_volume, volume_info.hfp_local_volume);

        tws_ctrl_send_cmd(APP_TWS_CMD_SYNC_VOLUME_INFO, (uint8_t*)&volume_info, sizeof(volume_info));
    }
}

void app_ibrt_ui_perform_user_action_v2(uint8_t *p_buff, uint16_t length)
{
    struct ibrt_if_action_header *action_header = (struct ibrt_if_action_header *)p_buff;
    uint8_t action = action_header->action;
    uint8_t device_id = app_bt_get_device_id_byaddr(&action_header->remote);
    uint32_t param = action_header->param;
    struct BT_DEVICE_T *curr_device = NULL;

    if (device_id == BT_DEVICE_INVALID_ID)
    {
        EARBUDS_TRACE(0,"%s invalid device id", __func__);
        return;
    }

    curr_device = app_bt_get_device(device_id);

    switch (action)
    {
        case IBRT_ACTION_PLAY:
            app_bt_a2dp_send_key_request(device_id, AVRCP_KEY_PLAY);
            break;
        case IBRT_ACTION_PAUSE:
            app_bt_a2dp_send_key_request(device_id, AVRCP_KEY_PAUSE);
            break;
        case IBRT_ACTION_FORWARD:
            app_bt_a2dp_send_key_request(device_id, AVRCP_KEY_FORWARD);
            break;
        case IBRT_ACTION_BACKWARD:
            app_bt_a2dp_send_key_request(device_id, AVRCP_KEY_BACKWARD);
            break;
        case IBRT_ACTION_AVRCP_VOLUP:
            app_bt_a2dp_send_key_request(device_id, AVRCP_KEY_VOLUME_UP);
            break;
        case IBRT_ACTION_AVRCP_VOLDN:
            app_bt_a2dp_send_key_request(device_id, AVRCP_KEY_VOLUME_DOWN);
            break;
        case IBRT_ACTION_AVRCP_ABS_VOL:
            app_bt_a2dp_send_set_abs_volume(device_id, param & 0x7f);
            break;
        case IBRT_ACTION_HFSCO_CREATE:
            if (curr_device->hf_audio_state == BT_HFP_AUDIO_CON)
            {
                EARBUDS_TRACE(0,"%s cannot create audio link\n", __func__);
            }
            else
            {
                btif_hf_create_audio_link(curr_device->hf_channel);
            }
            break;
        case IBRT_ACTION_HFSCO_DISC:
            if (curr_device->hf_audio_state == BT_HFP_AUDIO_CON)
            {
                btif_hf_disc_audio_link(curr_device->hf_channel);
            }
            else
            {
                EARBUDS_TRACE(0,"%s cannot disc audio link\n", __func__);
            }
            break;
        case IBRT_ACTION_REDIAL:
            if (curr_device->hf_conn_flag)
            {
                btif_hf_redial_call(curr_device->hf_channel);
            }
            else
            {
                EARBUDS_TRACE(0,"%s cannot redial call\n", __func__);
            }
            break;
        case IBRT_ACTION_ANSWER:
            if (curr_device->hf_conn_flag)
            {
                btif_hf_answer_call(curr_device->hf_channel);
            }
            else
            {
                EARBUDS_TRACE(0,"%s cannot answer call\n", __func__);
            }
            break;
        case IBRT_ACTION_HANGUP:
            if (curr_device->hf_conn_flag)
            {
                btif_hf_hang_up_call(curr_device->hf_channel);
            }
            else
            {
                EARBUDS_TRACE(0,"%s cannot hangup call\n", __func__);
            }
            break;
        case IBRT_ACTION_LOCAL_VOLUP:
            app_bt_local_volume_up(app_ibrt_keyboard_sync_volume_info_v2, param);
            break;
        case IBRT_ACTION_LOCAL_VOLDN:
            app_bt_local_volume_down(app_ibrt_keyboard_sync_volume_info_v2, param);
            break;
        case IBRT_ACTION_SWITCH_A2DP:
            bts_am_switch_streaming_a2dp_handler(action_header->param, action_header->param2);
            break;
        case IBRT_ACTION_SWITCH_SCO:
            bts_am_switch_streaming_sco_handler();
            break;
#if defined(BT_HID_DEVICE)
        case IBRT_ACTION_HID_SEND_CAPTURE:
            app_hid_device_capture_process(&curr_device->remote);
            break;
        case IBRT_ACTION_HID_SENSOR_REPORT:
            app_hid_device_send_sensor_report(&curr_device->remote, &((struct bt_hid_sensor_data_t *)p_buff)->report);
            break;
#endif
#ifdef TOTA_v2
        case IBRT_ACTION_SEND_TOTA_DATA:
            app_spp_tota_send_data((uint8_t*)p_buff+sizeof(struct ibrt_if_action_header), length-sizeof(struct ibrt_if_action_header));
            break;
#endif
        case IBRT_ACTION_HOLD_ACTIVE_CALL:
            if (curr_device->hf_conn_flag)
            {
                btif_hf_call_hold(curr_device->hf_channel, BTIF_HF_HOLD_HOLD_ACTIVE_CALLS, 0);
            }
            break;
        case IBRT_ACTION_3WAY_HUNGUP_INCOMING:
        case IBRT_ACTION_RELEASE_HOLD_CALL:
            if (curr_device->hf_conn_flag)
            {
                btif_hf_call_hold(curr_device->hf_channel, BTIF_HF_HOLD_RELEASE_HELD_CALLS, 0);
            }
            break;
        case IBRT_ACTION_HOLD_SWITCH:
            bta_tws_hold_background_switch();
            break;
        case IBRT_ACTION_RELEASE_ACTIVE_ACCEPT_ANOTHER:
            if (curr_device->hf_conn_flag)
            {
                btif_hf_call_hold(curr_device->hf_channel, BTIF_HF_HOLD_RELEASE_ACTIVE_CALLS, 0);
            }
            break;
#ifdef SUPPORT_SIRI
       case IBRT_ACTION_OPEN_VOICE_ASSISTANT:
            if (curr_device->hf_conn_flag){
                voice_assistant_flag = 1;
                btif_hf_enable_voice_recognition(curr_device->hf_channel, true);
            }

            break;
       case IBRT_ACTION_CLOSE_VOICE_ASSISTANT:
            if (curr_device->hf_conn_flag){
                voice_assistant_flag = 0;
                btif_hf_enable_voice_recognition(curr_device->hf_channel, false);
            }
            break;
#endif
        case IBRT_ACTION_AVRCP_FAST_FORWARD_START:
            app_bt_a2dp_send_key_request(device_id, AVRCP_KEY_FAST_FORWARD_START);
            break;
         case IBRT_ACTION_AVRCP_FAST_FORWARD_STOP:
            app_bt_a2dp_send_key_request(device_id, AVRCP_KEY_FAST_FORWARD_STOP);
            break;
         case IBRT_ACTION_AVRCP_REWIND_START:
            app_bt_a2dp_send_key_request(device_id, AVRCP_KEY_REWIND_START);
            break;
         case IBRT_ACTION_AVRCP_REWIND_STOP:
            app_bt_a2dp_send_key_request(device_id, AVRCP_KEY_REWIND_STOP);
            break;
         case IBRT_ACTION_TOGGLE_A2DP_CIS:
            bts_am_toggle_a2dp_cis_handler(action_header->param, action_header->param2);
           break;
       default:
           EARBUDS_TRACE(0,"%s unknown user action %d\n", __func__, action);
           break;
    }
}

#endif

