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
    BT_HFP_CONN_STATE_DISCONNECTED = 0,
    BT_HFP_CONN_STATE_CONNECTED,
} bt_hfp_conn_state_t;

typedef enum
{
    BT_HFP_AUDIO_STATE_DISCONNECTED = 0,
    BT_HFP_AUDIO_STATE_CONNECTED,
} bt_hfp_audio_state_t;

typedef enum
{
    BT_HFP_AUDIO_CODEC_NONE = 0,
    BT_HFP_AUDIO_CODEC_CVSD,
    BT_HFP_AUDIO_CODEC_MSBC,
    BT_HFP_AUDIO_CODEC_LC3,
} bt_hfp_audio_codec_t;

typedef enum
{
    BT_HFP_VOICE_RECOG_STATE_STOPPED = 0, // Voice recognition stopped state
    BT_HFP_VOICE_RECOG_STATE_STARTED      // Voice recognition started state
} bt_hfp_voice_recog_state_t;

typedef enum
{
    BT_HFP_SERVICE_UNAVAILABLE = 0, // Network unavailable state
    BT_HFP_SERVICE_AVAILABLE        // Network available state
} bt_hfp_service_avail_state_t;

typedef enum
{
    BT_HFP_ROAM_TYPE_HOME = 0, // Home network service type
    BT_HFP_ROAM_TYPE_ROAMING   // Roaming network service type
} bt_hfp_roam_state_t;

typedef enum
{
    BT_HFP_NO_CALLS = 0, // No calls in progress
    BT_HFP_CALL_PRESENT    // Calls in progress
} bt_hfp_call_state_t;

typedef enum
{
    BT_HFP_CALLSETUP_NONE = 0, // No call setup
    BT_HFP_CALLSETUP_INCOMING, // Incoming call setup
    BT_HFP_CALLSETUP_OUTGOING, // Outgoing call setup
    BT_HFP_CALLSETUP_ALERTING  // Call is alerting
} bt_hfp_callsetup_state_t;

typedef enum
{
    BT_HFP_CALLHELD_NONE = 0,        // No call held
    BT_HFP_CALLHELD_HOLD_AND_ACTIVE, // Hold and active call
    BT_HFP_CALLHELD_HOLD,            // Call held
} bt_hfp_callheld_state_t;

typedef enum
{
    BT_HFP_RSP_AND_HOLD_HELD = 0, // Hold the call
    BT_HFP_RSP_AND_HOLD_ACCEPT,   // Accept the call
    BT_HFP_RSP_AND_HOLD_REJECT,   // Reject the call
} bt_hfp_rsp_and_hold_state_t;

// Bluetooth Hands-Free Client Subscriber Service Types
typedef enum
{
    BT_HFP_SUB_SERVICE_UNKNOWN = 0, // Unknown service type
    BT_HFP_SUB_SERVICE_VOICE   = 4, // Voice service type
    BT_HFP_SUB_SERVICE_FAX     = 5  // Fax service type
} bt_hfp_sub_service_type_t;

typedef enum
{
    BT_HFP_CALL_DIRECTION_OUTGOING = 0, // Outgoing call direction
    BT_HFP_CALL_DIRECTION_INCOMING      // Incoming call direction
} bt_hfp_curr_call_direction_t;

// Bluetooth Hands-Free Call States
typedef enum
{
    BT_HFP_CALL_STATE_ACTIVE = 0,        // Call is active
    BT_HFP_CALL_STATE_HELD,              // Call is held
    BT_HFP_CALL_STATE_DIALING,           // Call is dialing
    BT_HFP_CALL_STATE_ALERTING,          // Call is alerting
    BT_HFP_CALL_STATE_INCOMING,          // Incoming call
    BT_HFP_CALL_STATE_WAITING,           // Call is waiting
    BT_HFP_CALL_STATE_HELD_BY_RESP_HOLD
} bt_hfp_curr_call_state_t;

// Bluetooth Hands-Free Call Multiparty Type
typedef enum
{
    BT_HFP_CALL_MPTY_SINGLE = 0, // Single party call
    BT_HFP_CALL_MPTY_MULTI       // Multi-party call
} bt_hfp_curr_call_mrty_t;

typedef enum
{
    BT_HFP_IN_BAND_RINGTONE_NOT_PROVIDED = 0, // In-band ringtone not provided
    BT_HFP_IN_BAND_RINGTONE_PROVIDED,         // In-band ringtone provided
} bt_hfp_in_band_ringtone_setting_t;

// AT response code - OK/Error
typedef enum {
    BT_HFP_AT_RESPONSE_OK = 0,
    BT_HFP_AT_RESPONSE_ERROR,
    BT_HFP_AT_RESPONSE_ERROR_NO_CARRIER,
    BT_HFP_AT_RESPONSE_ERROR_BUSY,
    BT_HFP_AT_RESPONSE_ERROR_NO_ANSWER,
    BT_HFP_AT_RESPONSE_ERROR_DELAYED,
    BT_HFP_AT_RESPONSE_ERROR_BLACKLISTED,
    BT_HFP_AT_RESPONSE_ERROR_CME,
} bt_hfp_at_response_t;

/**
 ****************************************************************************************
 * @brief       Callback for Hands-Free connection state changes.
 * @param[in]   address: Pointer to the Bluetooth address of the remote device.
 * @param[in]   state: Connection state of the HFP client.
 * @return      void
 ****************************************************************************************
 */
typedef void (*bt_hfp_hf_connection_state_cb)(const bt_bdaddr_t *address, bt_hfp_conn_state_t state, uint8_t error_code);

/**
 ****************************************************************************************
 * @brief       Callback for audio connection state changes
 * @param[in]   address: Pointer to the Bluetooth address of the remote device.
 * @param[in]   state: Audio connection(eSCO/SCO) state.
 * @return      void
 ****************************************************************************************
 */
typedef void (*bt_hfp_hf_audio_status_cb)(const bt_bdaddr_t *address, bt_hfp_audio_state_t state, bt_hfp_audio_codec_t codec, uint8_t error_code);

/**
 ****************************************************************************************
 * @brief       Callback for voice recognition command state changes.
 * @param[in]   address: Pointer to the Bluetooth address of the remote device.
 * @param[in]   state: Voice recognition state.
 * @return      void
 ****************************************************************************************
 */
typedef void (*bt_hfp_hf_voice_recognition_status_cb)(const bt_bdaddr_t *address, bt_hfp_voice_recog_state_t state);

/**
 ****************************************************************************************
 * @brief       Callback for network service state changes.
 * @param[in]   address: Pointer to the Bluetooth address of the remote device.
 * @param[in]   state: Network service state.
 * @return      void
 ****************************************************************************************
 */
typedef void (*bt_hfp_hf_service_availability_cb)(const bt_bdaddr_t *address, bt_hfp_service_avail_state_t state);

/**
 ****************************************************************************************
 * @brief       Callback for signal strength indication.
 * @param[in]   address: Pointer to the Bluetooth address of the remote device.
 * @param[in]   state: Signal strength level.
 * @return      void
 ****************************************************************************************
 */
typedef void (*bt_hfp_hf_signal_strength_cb)(const bt_bdaddr_t *address, uint8_t signal_strength);

/**
 ****************************************************************************************
 * @brief       Callback for network roaming status changes.
 * @param[in]   address: Pointer to the Bluetooth address of the remote device.
 * @param[in]   state: Roaming status.
 * @return      void
 ****************************************************************************************
 */
typedef void (*bt_hfp_hf_roaming_status_cb)(const bt_bdaddr_t *address, bt_hfp_roam_state_t type);

/**
 ****************************************************************************************
 * @brief       Callback for battery level indication.
 * @param[in]   address: Pointer to the Bluetooth address of the remote device.
 * @param[in]   battery_level: Battery level reported by the remote device.
 *               - Valid range: 0 to 100 (percentage).
 * @return      void
 ****************************************************************************************
 */
typedef void (*bt_hfp_hf_battery_level_cb)(const bt_bdaddr_t *address, uint8_t battery_level);

/**
 ****************************************************************************************
 * @brief       Callback for current network operator name.
 * @param[in]   address: Pointer to the Bluetooth address of the remote device.
 * @param[in]   name: Name of the current operator.
 * @return      void
 ****************************************************************************************
 */
typedef void (*bt_hfp_hf_current_operator_cb)(const bt_bdaddr_t *address, const char *name);

/**
 ****************************************************************************************
 * @brief       Callback for call indicator changes.
 * @param[in]   address: Pointer to the Bluetooth address of the remote device.
 * @param[in]   call: Current call indicator state.
 * @return      void
 ****************************************************************************************
 */
typedef void (*bt_hfp_hf_call_status_cb)(const bt_bdaddr_t *address, bt_hfp_call_state_t call);

/**
 ****************************************************************************************
 * @brief       Callback for call setup indicator changes.
 * @param[in]   address: Pointer to the Bluetooth address of the remote device.
 * @param[in]   callsetup: Current call setup state.
 * @return      void
 ****************************************************************************************
 */
typedef void (*bt_hfp_hf_callsetup_status_cb)(const bt_bdaddr_t *address, bt_hfp_callsetup_state_t callsetup);

/**
 ****************************************************************************************
 * @brief       Callback for call held indicator changes.
 * @param[in]   address: Pointer to the Bluetooth address of the remote device.
 * @param[in]   callheld: Current call held state.
 * @return      void
 ****************************************************************************************
 */
typedef void (*bt_hfp_hf_callheld_status_cb)(const bt_bdaddr_t *address, bt_hfp_callheld_state_t callheld);

/**
 ****************************************************************************************
 * @brief       Callback for response and hold state changes.
 * @param[in]   address: Pointer to the Bluetooth address of the remote device.
 * @param[in]   rsp_and_hold: Current response and hold state.
 * @return      void
 ****************************************************************************************
 */
typedef void (*bt_hfp_hf_response_and_hold_cb)(const bt_bdaddr_t *address, bt_hfp_rsp_and_hold_state_t rsp_and_hold);

/**
 ****************************************************************************************
 * @brief       Callback for Calling Line c (CLI) notification.
 * @param[in]   address: Pointer to the Bluetooth address of the remote device.
 * @param[in]   number: Phone number of the incoming caller.
 * @return      void
 ****************************************************************************************
 */
typedef void (*bt_hfp_hf_calling_line_identification_cb)(const bt_bdaddr_t *address, const char *number);

/**
 ****************************************************************************************
 * @brief       Callback for call waiting notification.
 * @param[in]   address: Pointer to the Bluetooth address of the remote device.
 * @param[in]   number: Phone number of the waiting call.
 * @return      void
 ****************************************************************************************
 */
typedef void (*bt_hfp_hf_call_waiting_cb)(const bt_bdaddr_t *address, const char *number);

/**
 ****************************************************************************************
 * @brief       Callback for audio speaker volume change.
 * @param[in]   address: Pointer to the Bluetooth address of the remote device.
 * @param[in]   volume: Updated volume level.
 * @return      void
 ****************************************************************************************
 */
typedef void (*bt_hfp_hf_spk_volume_change_cb)(const bt_bdaddr_t *address, uint8_t volume);

/**
 ****************************************************************************************
 * @brief       Callback for audio microphone volume change.
 * @param[in]   address: Pointer to the Bluetooth address of the remote device.
 * @param[in]   volume: Updated volume level.
 * @return      void
 ****************************************************************************************
 */
typedef void (*bt_hfp_hf_mic_volume_change_cb)(const bt_bdaddr_t *address, uint8_t volume);

/**
 ****************************************************************************************
 * @brief       Callback for subscriber information.
 * @param[in]   address: Pointer to the Bluetooth address of the remote device.
 * @param[in]   name: Subscriber's name or number.
 * @param[in]   type: Type of subscriber service.
 * @return      void
 ****************************************************************************************
 */
typedef void (*bt_hfp_hf_subscriber_number_info_cb)(const bt_bdaddr_t *address, const char *number, bt_hfp_sub_service_type_t type);


/**
 ****************************************************************************************
 * @brief       Callback for listing current calls.
 *              - This callback is triggered to provide the details of current calls. It may be
 *                called multiple times to list all ongoing calls. If the phone number is
 *                unknown, `NULL` is passed as the number.
 * @param[in]   address: Pointer to the Bluetooth address of the remote device.
 * @param[in]   index: Index of the call.
 * @param[in]   dir: Direction of the call.
 * @param[in]   state: Current state of the call.
 * @param[in]   mpty: Multi-party call type.
 * @param[in]   number: Phone number of the call.
 * @return      void
 ****************************************************************************************
 */
typedef void (*bt_hfp_hf_query_current_calls_cb)(const bt_bdaddr_t *address, int index,
                                                bt_hfp_curr_call_direction_t dir,
                                                bt_hfp_curr_call_state_t state,
                                                bt_hfp_curr_call_mrty_t mpty,
                                                const char *number);

/**
 ****************************************************************************************
 * @brief       Callback for in-band ring tone settings.
 * @param[in]   address: Pointer to the Bluetooth address of the remote device.
 * @param[in]   state: Current in-band ring tone state.
 * @return      void
 ****************************************************************************************
 */
typedef void (*bt_hfp_hf_in_band_ringtone_setting_cb)(const bt_bdaddr_t *address,
                                                bt_hfp_in_band_ringtone_setting_t state);

/**
 ****************************************************************************************
 * @brief       Callback for sending ring indication to the application.
 * @param[in]   address: Pointer to the Bluetooth address of the remote device.
 * @return      void
 ****************************************************************************************
 */
typedef void (*bt_hfp_hf_ring_indication_cb)(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Callback for requested number from AG.
 *              - This callback is triggered to provide the phone number associated with the
 *                last voice tag recorded on the remote audio gateway.
 * @param[in]   address: Pointer to the Bluetooth address of the remote device.
 *              - Must not be NULL.
 * @param[in]   number: Phone number retrieved from the remote device.
 *              - NULL if unavailable.
 * @return      void
 ****************************************************************************************
 */
typedef void (*bt_hfp_hf_last_voice_tag_number_cb)(const bt_bdaddr_t *address, const char *number);

/**
 ****************************************************************************************
 * @brief       Callback for unknow event
 * @param[in]   address: Pointer to the Bluetooth address of the remote device.
 * @param[in]   unknow_event: unknow/vendor enent
 * @return      void
 ****************************************************************************************
 */
typedef void (*bt_hfp_hf_custom_at_cmd_cb)(const bt_bdaddr_t *address, const char *data);

typedef void (*bt_hfp_hf_at_cmd_result_cb)(const bt_bdaddr_t *address, bt_hfp_at_response_t result);

typedef struct
{
    bt_hfp_hf_connection_state_cb connection_state_cb;
    bt_hfp_hf_audio_status_cb audio_status_cb;
    bt_hfp_hf_voice_recognition_status_cb voice_recog_status_cb;
    bt_hfp_hf_service_availability_cb service_availability_cb;
    bt_hfp_hf_signal_strength_cb signal_strength_cb;
    bt_hfp_hf_roaming_status_cb roaming_status_cb;
    bt_hfp_hf_battery_level_cb battery_level_cb;
    bt_hfp_hf_current_operator_cb current_operator_cb;
    bt_hfp_hf_call_status_cb call_status_cb;
    bt_hfp_hf_callsetup_status_cb callsetup_status_cb;
    bt_hfp_hf_callheld_status_cb callheld_status_cb;
    bt_hfp_hf_response_and_hold_cb rsp_and_hold_cb;
    bt_hfp_hf_calling_line_identification_cb calling_line_id_cb;
    bt_hfp_hf_call_waiting_cb call_waiting_cb;
    bt_hfp_hf_query_current_calls_cb current_calls_cb;
    bt_hfp_hf_spk_volume_change_cb spk_volume_change_cb;
    bt_hfp_hf_mic_volume_change_cb mic_volume_change_cb;
    bt_hfp_hf_subscriber_number_info_cb subscriber_info_cb;
    bt_hfp_hf_in_band_ringtone_setting_cb in_band_ringtone_setting_cb;
    bt_hfp_hf_ring_indication_cb ring_indication_cb;
    bt_hfp_hf_last_voice_tag_number_cb last_voice_tag_number_cb;
    bt_hfp_hf_at_cmd_result_cb at_cmd_result_cb;
    bt_hfp_hf_custom_at_cmd_cb custom_at_cmd_cb;
} bt_hfp_hf_callbacks_t;

typedef enum {
    BT_HFP_HF_CB_TYPE_CONNECTION_STATE,
    BT_HFP_HF_CB_TYPE_AUDIO_STATUS,
    BT_HFP_HF_CB_TYPE_VOICE_RECOG_STATUS,
    BT_HFP_HF_CB_TYPE_SERVICE_AVAILABILITY,
    BT_HFP_HF_CB_TYPE_SIGNAL_STRENGTH,
    BT_HFP_HF_CB_TYPE_ROAMING_STATUS,
    BT_HFP_HF_CB_TYPE_BATTERY_LEVEL,
    BT_HFP_HF_CB_TYPE_CURRENT_OPERATOR,
    BT_HFP_HF_CB_TYPE_CURRENT_STATUS,
    BT_HFP_HF_CB_TYPE_CALL_STATUS,
    BT_HFP_HF_CB_TYPE_CALLSETUP_STATUS,
    BT_HFP_HF_CB_TYPE_RSP_AND_HOLD_CB,
    BT_HFP_HF_CB_TYPE_CALLING_LINE_ID_CB,
    BT_HFP_HF_CB_TYPE_CALL_WAITING_CB,
    BT_HFP_HF_CB_TYPE_CURRENT_CALLS_CB,
    BT_HFP_HF_CB_TYPE_SPK_VOLUME_CHANGE,
    BT_HFP_HF_CB_TYPE_MIC_VOLUME_CHANGE,
    BT_HFP_HF_CB_TYPE_SUBSCRIBER_INFO,
    BT_HFP_HF_CB_TYPE_IN_BAND_RINGTONE_SETTING,
    BT_HFP_HF_CB_TYPE_RING_INDICATION,
    BT_HFP_HF_CB_TYPE_LAST_VOICE_TAG_NUMBER,
    BT_HFP_HF_CB_TYPE_AT_CMD_RESULT,
    BT_HFP_HF_CB_TYPE_CUSTOM_AT_CMD,
} bt_hfp_hf_cb_type_t;

typedef enum {
    BT_HFP_HF_CB_USER_BTA = 0,
    BT_HFP_HF_CB_USER_APP,
    BT_HFP_HF_CB_USER_DEPRECATED,
    BT_HFP_HF_CB_USER_GFPS,
    BT_HFP_HF_CB_USER_MODULE_1,
    BT_HFP_HF_CB_USER_CUSTOMER,
    BT_HFP_HF_CB_USER_MAX,
} bt_hfp_hf_callback_user_t;