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

typedef enum {
    BT_AVRCP_KEY_NULL               = 0,
    BT_AVRCP_KEY_VOLUME_UP          = 0x41,
    BT_AVRCP_KEY_VOLUME_DOWN        = 0x42,
    BT_AVRCP_KEY_MUTE               = 0x43,
    BT_AVRCP_KEY_PLAY               = 0x44,
    BT_AVRCP_KEY_STOP               = 0x45,
    BT_AVRCP_KEY_PAUSE              = 0x46,
    BT_AVRCP_KEY_RECORD             = 0x47,
    BT_AVRCP_KEY_REWIND             = 0x48,
    BT_AVRCP_KEY_FAST_FORWARD       = 0x49,
    BT_AVRCP_KEY_EJECT              = 0x4A,
    BT_AVRCP_KEY_FORWARD            = 0x4B,
    BT_AVRCP_KEY_BACKWARD           = 0x4C,
    BT_AVRCP_KEY_FAST_FORWARD_START = 0X4D,
    BT_AVRCP_KEY_FAST_FORWARD_STOP  = 0X4E,
    BT_AVRCP_KEY_REWIND_START       = 0X4F,
    BT_AVRCP_KEY_REWIND_STOP        = 0X50
} bt_avrcp_key_t;

typedef enum {
    BT_AVRCP_CONN_STATE_DISCONNECTED = 0,
    BT_AVRCP_CONN_STATE_CONNECTED,
} bt_avrcp_conn_state_t;

typedef enum {
    BT_AVRCP_CAPABILITY_COMPANY_ID         = 0x2,
    BT_AVRCP_CAPABILITY_EVENTS_SUPPORTED   = 0x3
} bt_avrcp_capability_t;

typedef enum {
    BT_AVRCP_MEDIA_ATTR_TITLE          = 0x01, // Media title attribute
    BT_AVRCP_MEDIA_ATTR_ARTIST         = 0x02, // Media artist attribute
    BT_AVRCP_MEDIA_ATTR_ALBUM          = 0x03, // Media album attribute
    BT_AVRCP_MEDIA_ATTR_TRACK          = 0x04, // Track number attribute
    BT_AVRCP_MEDIA_ATTR_NUM_TRACKS     = 0x05, // Total number of tracks attribute
    BT_AVRCP_MEDIA_ATTR_GENRE          = 0x06, // Media genre attribute
    BT_AVRCP_MEDIA_ATTR_PLAYING_TIME   = 0x07, // Playing time of the media attribute
    BT_AVRCP_MEDIA_ATTR_COVER_HANDLE   = 0x08,
} bt_avrcp_media_attr_id_t;

typedef enum {
    BT_AVRCP_PLAYSTATUS_STOPPED    = 0,
    BT_AVRCP_PLAYSTATUS_PLAYING,
    BT_AVRCP_PLAYSTATUS_PAUSED,
    BT_AVRCP_PLAYSTATUS_FWD_SEEK,
    BT_AVRCP_PLAYSTATUS_REV_SEEK,
    BT_AVRCP_PLAYSTATUS_ERROR      = 0xFF
} bt_avrcp_playback_status_t;

typedef enum {
    BT_AVRCP_EVENT_PLAY_STATUS_CHANGED         = 0x01,
    BT_AVRCP_EVENT_TRACK_CHANGED               = 0x02,
    BT_AVRCP_EVENT_TRACK_REACHED_END           = 0x03,
    BT_AVRCP_EVENT_TRACK_REACHED_START         = 0x04,
    BT_AVRCP_EVENT_PLAY_POS_CHANGED            = 0x05,
    BT_AVRCP_EVENT_BATT_STATUS_CHANGED         = 0x06,
    BT_AVRCP_EVENT_SYSTEM_STATUS_CHANGED       = 0x07,
    BT_AVRCP_EVENT_PLAYER_APP_SETTINGS_CHANGED = 0x08,
    BT_AVRCP_EVENT_NOW_PLAYING_CONTENT_CHANGED = 0x09,
    BT_AVRCP_EVENT_AVAL_PLAYER_CHANGED         = 0x0a,
    BT_AVRCP_EVENT_ADDR_PLAYER_CHANGED         = 0x0b,
    BT_AVRCP_EVENT_UIDS_CHANGED                = 0x0c,
    BT_AVRCP_EVENT_VOL_CHANGED                 = 0x0d,
    BT_AVRCP_EVENT_MAX,
} bt_avrcp_event_id_t;

typedef enum {
    BT_AVRCP_BATTERY_STATUS_NORMAL      = 0x00, // Battery operation is in normal state
    BT_AVRCP_BATTERY_STATUS_WARNING     = 0x01, // unable to operate soon. Is provided when the battery level is going down
    BT_AVRCP_BATTERY_STATUS_CRITICAL    = 0x02, // Cannot operate any more. Is provided when the battery level is going down
    BT_AVRCP_BATTERY_STATUS_EXTERNAL    = 0x03, // Plugged to external power supply
    BT_AVRCP_BATTERY_STATUS_FULLCHARGE  = 0x04, // when the device is completely charged from the external power supply
} bt_avrcp_battery_status_t;

typedef enum {
    BT_AVRCP_POWER_ON  = 0x00,
    BT_AVRCP_POWER_OFF = 0x01,
    BT_AVRCP_UNPLUGGED = 0x02
} bt_avrcp_system_status_t;

typedef struct {
    bt_avrcp_capability_t capability_id;

    union {
        // params for COMPANY_ID capability
        struct {
            uint8_t num;
            uint32_t id[12];
        } company_id;
        // params for EVENTS_SUPPORTED capability
        uint16_t supported_event_mask;
    } params;
} bt_avrcp_capabilities_params_t;

typedef struct {
    bt_avrcp_media_attr_id_t attr_id;
    uint16_t char_set_id;
    uint16_t attr_value_len;
    const char *attr_value;
} bt_avrcp_element_attr_t;

typedef struct {
    bt_avrcp_event_id_t event;

    union {
        // params for PLAY_STATUS_CHANGED event
        bt_avrcp_playback_status_t playback_status;
        // params for TRACK_CHANGED event
        uint64_t track_id;
        // no params for TRACK_REACHED_END event
        // no params for TRACK_REACHED_START event
        // params for PLAY_POS_CHANGED event
        uint32_t position;
        // params for BATT_STATUS_CHANGED event
        bt_avrcp_battery_status_t battery_status;
        // params for SYSTEM_STATUS_CHANGED event
        bt_avrcp_system_status_t system_status;
        // params for PLAYER_APP_SETTINGS_CHANGED event
        struct {

        } player_app_setting;
        // no params for NOW_PLAYING_CONTENT_CHANGED event
        // no params for AVAL_PLAYER_CHANGED event
        // params for ADDR_PLAYER_CHANGED event
        struct {
            uint16_t player_id;
            uint16_t uid_counter;
        } addr_player;
        // params for UIDS_CHANGED event
        uint16_t uid_counter;
        // params for VOL_CHANGED event
        uint8_t volume;
    } params;
} bt_avrcp_notification_params_t;

typedef struct {
    uint8_t data_len;
    uint8_t *data;
} bt_avrcp_browsing_rsp_t;

typedef struct {
    uint8_t cmd_trans_id;
    uint8_t data_len;
    uint8_t *data;
} bt_avrcp_browsing_cmd_t;

typedef struct {
    uint8_t error_code;
    bool is_final_packet;
    uint16_t body_data_length;
    uint8_t *body_data;
} bt_avrcp_obex_info_t;

/**
 ****************************************************************************************
 * @brief       Callback for avrcp profile connection state changed.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   state: connection state.
 ****************************************************************************************
 */
typedef void (*bt_avrcp_connection_state_cb)(const bt_bdaddr_t *address, bt_avrcp_conn_state_t state, uint8_t error_code);

/**
 ****************************************************************************************
 * @brief       Callback for CT received get capabilities response from TG.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   params: capabilities params, different params by capability_id.
 ****************************************************************************************
 */
typedef void (*bt_avrcp_recv_get_capabilities_rsp_cb)(const bt_bdaddr_t *address, const bt_avrcp_capabilities_params_t *params);

/**
 ****************************************************************************************
 * @brief       Callback for CT received get element attributes(media info) response from TG
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   num_attrs: number of attributes.
 * @param[in]   attr_params: array of attributes value.
 ****************************************************************************************
 */
typedef void (*bt_avrcp_recv_get_element_attributes_rsp_cb)(const bt_bdaddr_t *address, uint8_t num_attrs, const bt_avrcp_element_attr_t *attr_params);

/**
 ****************************************************************************************
 * @brief       Callback for CT received get play status response from TG
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   song_length: length of the currently playing music
 * @param[in]   song_position: position of the currently playing music
 * @param[in]   status: play status of current playing music
 ****************************************************************************************
 */
typedef void (*bt_avrcp_recv_play_status_rsp_cb)(const bt_bdaddr_t *address, uint32_t song_length, uint32_t song_position, bt_avrcp_playback_status_t status);

/**
 ****************************************************************************************
 * @brief       Callback for CT received set absolute volumee command from TG.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   volume: indicates the current status of playback.
 ****************************************************************************************
 */
typedef void (*bt_avrcp_recv_set_absolute_volume_cmd_cb)(const bt_bdaddr_t *address, uint8_t volume);

/**
 ****************************************************************************************
 * @brief       Callback for CT received register notification response from TG.
 * @param[in]   changed: interim or changed response.
 * @param[in]   params: different params for each register notification event.
 ****************************************************************************************
 */
typedef void (*bt_avrcp_recv_register_notification_rsp_cb)(const bt_bdaddr_t *address, bool changed, const bt_avrcp_notification_params_t *params);

/**
 ****************************************************************************************
 * @brief       Callback for TG received passthrough command from CT.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   key: passthrough key code.
 * @param[in]   is_press: indicate key is press state or release state.
 ****************************************************************************************
 */
typedef void (*bt_avrcp_recv_passthrough_cmd_cb)(const bt_bdaddr_t *address, bt_avrcp_key_t key, bool is_press);

/**
 ****************************************************************************************
 * @brief       Callback for CT received passthrough command Response from TG
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 ****************************************************************************************
 */
typedef void (*bt_avrcp_recv_passthrough_rsp_cb)(const bt_bdaddr_t *address, bt_avrcp_key_t key, bool is_press);

typedef void (*bt_avrcp_brosing_connection_state_cb)(const bt_bdaddr_t *address, bt_avrcp_conn_state_t state, uint8_t error_code);

typedef void (*bt_avrcp_recv_browsing_cmd_cb)(const bt_bdaddr_t *address, const bt_avrcp_browsing_cmd_t *cmd);

typedef void (*bt_avrcp_recv_browsing_rsp_cb)(const bt_bdaddr_t *address, const bt_avrcp_browsing_rsp_t *rsp);

typedef void (*bt_avrcp_obex_connection_state_cb)(const bt_bdaddr_t *address, bt_avrcp_conn_state_t state, uint8_t error_code);

typedef void (*bt_avrcp_obex_get_image_done_cb)(const bt_bdaddr_t *address, const bt_avrcp_obex_info_t *info);

typedef void (*bt_avrcp_obex_get_thm_done_cb)(const bt_bdaddr_t *address, const bt_avrcp_obex_info_t *info);

typedef void (*bt_avrcp_obex_get_image_properties_cb)(const bt_bdaddr_t *address, const bt_avrcp_obex_info_t *info);

typedef struct {
    bt_avrcp_connection_state_cb                   connection_state_cb;
    bt_avrcp_recv_get_capabilities_rsp_cb          recv_capabilities_cb;
    bt_avrcp_recv_get_element_attributes_rsp_cb    recv_element_attrs_cb;
    bt_avrcp_recv_play_status_rsp_cb               recv_play_status_cb;
    bt_avrcp_recv_set_absolute_volume_cmd_cb       set_absolute_volume_cb;
    bt_avrcp_recv_register_notification_rsp_cb     notification_rsp_cb;
    bt_avrcp_recv_passthrough_cmd_cb               recv_passthrough_cmd_cb;
    bt_avrcp_recv_passthrough_rsp_cb               recv_passthrough_rsp_cb;
    bt_avrcp_brosing_connection_state_cb           brosing_connection_state_cb;
    bt_avrcp_recv_browsing_cmd_cb                  recv_brosing_cmd_cb;
    bt_avrcp_recv_browsing_rsp_cb                  recv_brosing_rsp_cb;
    bt_avrcp_obex_connection_state_cb              obex_connection_state_cb;
    bt_avrcp_obex_get_image_done_cb                obex_get_image_done_cb;
    bt_avrcp_obex_get_thm_done_cb                  obex_get_thm_done_cb;
    bt_avrcp_obex_get_image_properties_cb          obex_get_image_properties_cb;
} bt_avrcp_callbacks_t;

typedef enum {
    BT_AVRCP_CB_TYPE_CONNECTION,
    BT_AVRCP_CB_TYPE_RECV_GET_CAPABILITIES_RSP,
    BT_AVRCP_CB_TYPE_RECV_GET_ELEM_ATTR_RSP,
    BT_AVRCP_CB_TYPE_RECV_PLAY_STATUS_RSP,
    BT_AVRCP_CB_TYPE_RECV_SET_ABS_VOL_CMD,
    BT_AVRCP_CB_TYPE_RECV_REGISTER_NTF_RSP,
    BT_AVRCP_CB_TYPE_RECV_PASSTHROUGH_CMD,
    BT_AVRCP_CB_TYPE_RECV_PASSTHROUGH_RSP,
    BT_AVRCP_CB_TYPE_RECV_BROWSING_CMD,
    BT_AVRCP_CB_TYPE_RECV_BROWSING_RSP,
    BT_AVRCP_CB_TYPE_OBEX_GET_IMAGE_DONE,
    BT_AVRCP_CB_TYPE_OBEX_GET_THM_DONE,
    BT_AVRCP_CB_TYPE_OBEX_GET_IMAGE_PROPERTIES,
} bt_avrcp_cb_type_t;

typedef enum {
    BT_AVRCP_CB_USER_BTA   = 0,
    BT_AVRCP_CB_USER_APP,
    BT_AVRCP_CB_USER_DEPRECATED,
    BT_AVRCP_CB_USER_GFPS,
    BT_AVRCP_CB_USER_MODULE_1,
    BT_AVRCP_CB_USER_CUSTOMER,
    BT_AVRCP_CB_USER_MAX,
} bt_avrcp_callback_user_t;
