/****************************************************************************
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

#ifndef __BTS_HFP_API_H__
#define __BTS_HFP_API_H__

#include "bts_bt_types.h"
#include "bt_hfp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 ****************************************************************************************
 *    _   _ _____ ____       _   _ _____
 *   | | | |  ___|  _ \     | | | |  ___|
 *   | |_| | |_  | |_) |    | |_| | |_
 *   |  _  |  _| |  __/     |  _  |  _|
 *   |_| |_|_|   |_|        |_| |_|_|
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief       Register the hf event callbacks.
 * @param[in]   user: Registered user.
 * @param[in]   callbacks: Pointer to callbacks for handling hfp hf events.
 * @return      none
 ****************************************************************************************
 */
void bts_hfp_hf_register_callbacks(bt_hfp_hf_callback_user_t user, bt_hfp_hf_callbacks_t *callbacks);

/**
 ****************************************************************************************
 * @brief       Ddinit the hfp profile.
 ****************************************************************************************
 */
void bts_hfp_deinit(void);

/**
 ****************************************************************************************
 * @brief       Register the hf event callbacks.
 * @param[in]   user: Registered user.
 * @return      none
 ****************************************************************************************
 */
void bts_hfp_hf_deregister_callbacks(bt_hfp_hf_callback_user_t user);

/**
 ****************************************************************************************
 * @brief       Connect to an audio gateway device.
 * @param[in]   address: Pointer to the Bluetooth address of the AG device.
 *              - Must not be NULL.
 * @return      status of the operation
 *              - BTH_BT_STATUS_SUCCESS: Connection initiated successfully.
 *              - BTH_BT_STATUS_FAIL: Failed to initiate connection.
 ****************************************************************************************
 */
bts_status_t bts_hfp_hf_connect(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Disconnect from an audio gateway device.
 * @param[in]   address: Pointer to the Bluetooth address of the AG device.
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_hfp_hf_disconnect(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Determine whether the hf is connected.
 * @param[in]   address: Pointer to the Bluetooth address of the AG device.
 * @return      status of the operation
 ****************************************************************************************
 */
bool bts_hfp_hf_is_connected(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Determine whether the current connection is initiated actively.
 * @param[in]   address: Pointer to the Bluetooth address of the AG device.
 * @return      status of the operation
 ****************************************************************************************
 */
bool bts_hfp_hf_is_initiator(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Establish an audio connection(eSCO/SCO) with an audio gateway.
 * @param[in]   address: Pointer to the Bluetooth address of the AG device.
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_hfp_hf_connect_audio(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Terminate the audio connection with an audio gateway.
 * @param[in]   address: Pointer to the Bluetooth address of the AG device.
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_hfp_hf_disconnect_audio(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Determine whether SCO/eSCO is connected.
 * @param[in]   address: Pointer to the Bluetooth address of the AG device.
 * @return      status of the operation
 ****************************************************************************************
 */
bool bts_hfp_hf_is_audio_connected(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Start voice recognition on a remote device.
 * @param[in]   address: Pointer to the Bluetooth address of the AG device.
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_hfp_hf_start_voice_recognition(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Stop voice recognition on a remote device.
 * @param[in]   address: Pointer to the Bluetooth address of the AG device.
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_hfp_hf_stop_voice_recognition(const bt_bdaddr_t *address);

/**
 ***************************************************************************************
 * @brief       Stop voice recognition on a remote device.
 * @param[in]   address: Pointer to the Bluetooth address of the AG device.
 * @return      status of the operation
 ****************************************************************************************
 */
bt_hfp_voice_recog_state_t bts_hfp_hf_get_voice_recognition_state(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Control the speaker volume on a remote audio gateway device.
 * @param[in]   address: Pointer to the Bluetooth address of the AG device.
 * @param[in]   volume: Volume level to set.
 * @return      status of the operation
 ****************************************************************************************
 */
// bts_status_t bts_hfp_hf_spk_volume_control(const bt_bdaddr_t *address, uint8_t volume);

/**
 ****************************************************************************************
 * @brief       Control the speaker volume on a remote audio gateway device.
 * @param[in]   address: Pointer to the Bluetooth address of the AG device.
 * @param[in]   volume: Volume level to set.
 * @return      status of the operation
 ****************************************************************************************
 */
// bts_status_t bts_hfp_hf_mic_volume_control(const bt_bdaddr_t *address, uint8_t volume);

/**
 ****************************************************************************************
 * @brief       Place a call to the specified number.
 * @param[in]   address: Pointer to the Bluetooth address of the AG device.
 * @param[in]   number: The phone number to dial.
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_hfp_hf_dial(const bt_bdaddr_t *address, const char *number);

/**
 * @brief       Place a call using the specified memory location.
 * @param[in]   address: Pointer to the Bluetooth address of the AG device.
 * @param[in]   location: Memory location of the stored number.
 * @return      status of the operation
 */
bts_status_t bts_hfp_hf_dial_memory(const bt_bdaddr_t *address, int location);

/**
 ****************************************************************************************
 * @brief       Place a call using the last number
 * @param[in]   address: Pointer to the Bluetooth address of the AG device.
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_hfp_hf_redial(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Answer call
 * @param[in]   address: Pointer to the Bluetooth address of the AG device.
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_hfp_hf_answer_call(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Hung up call
 * @param[in]   address: Pointer to the Bluetooth address of the AG device.
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_hfp_hf_hungup_call(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Perform a specified call-related action.
 * @param[in]   address: Pointer to the Bluetooth address of the AG device.
 * @param[in]   action: Call action to perform.
 * @param[in]   idx: Call index for the action (if applicable).
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_hfp_hf_handle_call_action(const bt_bdaddr_t *address, bts_hfp_call_action_t action, int idx);

/**
 ****************************************************************************************
 * @brief       Query the list of current calls.
 * @param[in]   address: Pointer to the Bluetooth address of the AG device.
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_hfp_hf_query_current_calls(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Query the name of the currently selected operator.
 * @param[in]   address: Pointer to the Bluetooth address of the AG device.
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_hfp_hf_query_current_operator_name(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Retrieve subscriber information.
 * @param[in]   address: Pointer to the Bluetooth address of the AG device.
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_hfp_hf_retrieve_subscriber_info(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Send a DTMF (Dual Tone Multi-Frequency) code.
 * @param[in]   address: Pointer to the Bluetooth address of the AG device.
 * @param[in]   code: DTMF code to send (0-9, *, #, A-D).
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_hfp_hf_send_dtmf(const bt_bdaddr_t *address, char code);

/**
 ****************************************************************************************
 * @brief       Request the phone number associated with the last voice tag recorded.
 * @param[in]   address: Pointer to the Bluetooth address of the AG device.
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_hfp_hf_request_last_voice_tag_number(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Disable the echo canceling and noise reduction functions resident in AG.
 * @param[in]   address: Pointer to the Bluetooth address of the AG device.
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_hfp_hf_turning_off_ec_and_nr(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Indicate the current battery level to AG.
 * @param[in]   address: Pointer to the Bluetooth address of the AG device.
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_hfp_hf_send_battery_level(const bt_bdaddr_t *address, uint8_t level);

/**
 ****************************************************************************************
 * @brief       Send an AT command to the remote device.
 * @param[in]   address: Pointer to the Bluetooth address of the AG device.
 * @param[in]   cmd: AT command.
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_hfp_hf_send_at_cmd(const bt_bdaddr_t *address, const char *cmd);

/**
 ****************************************************************************************
 * @brief       Report current hfp volume through +VGS.
 * @note        This function only report streaming device volume.
 * @return      status of the operation
 ****************************************************************************************
 */
void bts_hfp_hf_report_speaker_gain();

void bts_hfp_hf_set_local_volume(const bt_bdaddr_t *address, uint8_t local_volume);

void bts_hfp_hf_set_btvolume(const bt_bdaddr_t *address, uint8_t bt_volume);

uint8_t bts_hfp_hf_get_local_volume(const bt_bdaddr_t *address);

uint8_t bts_hfp_hf_get_bt_volume(const bt_bdaddr_t *address);

uint8_t bts_hfp_hf_convert_bt_volume_to_local_volume(uint8_t bt_volume);

uint8_t bts_hfp_hf_convert_local_volume_to_bt_volume(uint8_t local_volume);

bt_hfp_call_state_t bts_hfp_hf_get_ciev_call_state(const bt_bdaddr_t *address);

bt_hfp_callsetup_state_t bts_hfp_hf_get_ciev_callsetup_state(const bt_bdaddr_t *address);

bt_hfp_callheld_state_t bts_hfp_hf_get_ciev_callheld_state(const bt_bdaddr_t *address);

uint8_t bts_hfp_hf_get_reject_dev(void);

void bts_hfp_hf_set_reject_dev(uint8_t device_id);

/**
 ****************************************************************************************
 *    _   _ _____ ____          _    ____
 *   | | | |  ___|  _ \        / \  / ___|
 *   | |_| | |_  | |_) |      / _ \| |  _
 *   |  _  |  _| |  __/      / ___ \ |_| |
 *   |_| |_|_|   |_|        /_/   \_\____|
 *
 ****************************************************************************************
 */

typedef struct {
    bt_hfp_service_avail_state_t service;
    bt_hfp_roam_state_t roam;
    uint8_t signal;
    uint8_t battery_level;
} bts_hfp_ag_device_status_t;

typedef struct {
    uint8_t num_active;
    uint8_t num_held;
    bt_hfp_curr_call_state_t call_state;
    bts_hfp_curr_call_number_type_t number_type;
    const char *number;
    const char *caller_name;
} bts_hfp_ag_phone_status_t;

typedef struct {
    uint8_t index;
    bt_hfp_curr_call_direction_t dir;
    bt_hfp_curr_call_state_t state;
    bts_hfp_curr_call_mode_t mode;
    bt_hfp_curr_call_mrty_t mpty;
    bts_hfp_curr_call_number_type_t number_type;
    const char *number;
} bts_hfp_ag_clcc_status_t;

typedef struct {
    bt_hfp_service_avail_state_t service;
    bt_hfp_roam_state_t roam;
    uint8_t signal;
    uint8_t battery_level;
    uint8_t num_active;
    uint8_t num_held;
    bt_hfp_curr_call_state_t call_state;
} bts_hfp_ag_cind_status_t;

/**
 ****************************************************************************************
 * @brief       Callback for connection state change.
 * @param[in]   state one of the values from bts_hfp_ag_connection_state_t
 * @param[in]   address remote device address
 ****************************************************************************************
 */
typedef void (*bts_hfp_ag_connection_state_cb)(const bt_bdaddr_t *address, bt_hfp_conn_state_t state, uint8_t error_code);

/**
 ****************************************************************************************
 * @brief       Callback for audio connection state change.
 * @param[in]   state one of the values from BTS_HFP_audio_state_t
 * @param[in]   address remote device address
 ****************************************************************************************
 */
typedef void (*bts_hfp_ag_audio_status_cb)(const bt_bdaddr_t *address, bt_hfp_audio_state_t state, bt_hfp_audio_codec_t codec, uint8_t error_code);

/**
 ****************************************************************************************
 * @brief       Callback for VR connection state change.
 * @param[in]   state one of the values from bts_hfp_ag_vr_state_t
 * @param[in]   address
 ****************************************************************************************
 */
typedef void (*bts_hfp_ag_voice_recognition_cb)(const bt_bdaddr_t *address, bt_hfp_voice_recog_state_t state);

/**
 ****************************************************************************************
 * @brief       Callback for answer incoming call (ATA)
 * @param[in]   address remote device address
 ****************************************************************************************
 */
typedef void (*bts_hfp_ag_answer_call_cb)(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Callback for disconnect call (AT+CHUP)
 * @param[in]   address remote device address
 ****************************************************************************************
 */
typedef void (*bts_hfp_ag_hangup_call_cb)(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Callback for audio speaker volume change.
 * @param[in]   address: Pointer to the Bluetooth address of the remote device.
 * @param[in]   volume: Updated volume level.
 ****************************************************************************************
 */
typedef void (*bts_hfp_ag_spk_volume_change_cb)(const bt_bdaddr_t *address, uint8_t volume);

/**
 ****************************************************************************************
 * @brief       Callback for audio microphone volume change.
 * @param[in]   address: Pointer to the Bluetooth address of the remote device.
 * @param[in]   volume: Updated volume level.
 ****************************************************************************************
 */
typedef void (*bts_hfp_ag_mic_volume_change_cb)(const bt_bdaddr_t *address, uint8_t volume);

/**
 ****************************************************************************************
 * @brief       Callback for dialing an outgoing call
 * @param[in]   number: intended phone number, if number is NULL, redial
 * @param[in]   address: remote device address
 ****************************************************************************************
 */
typedef void (*bts_hfp_ag_dial_cb)(const bt_bdaddr_t *address, const char* number);

/**
 ****************************************************************************************
 * @brief       Callback for dialing memory call
 * @param[in]   number: intended phone number, if number is NULL, redial
 * @param[in]   address: remote device address
 ****************************************************************************************
 */
typedef void (*bts_hfp_ag_dial_memory_cb)(const bt_bdaddr_t *address, uint32_t location);

/**
 ****************************************************************************************
 * @brief       Callback for redial an outgoing call
 * @param[in]   address: remote device address
 ****************************************************************************************
 */
typedef void (*bts_hfp_ag_redial_cb)(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Callback for enabling/disabling noise reduction/echo cancellation
 * @param[in]   address: remote device address
 * @param[in]   enable: true to enable, false to disable
 ****************************************************************************************
 */
typedef void (*bts_hfp_ag_noise_reduction_cb)(const bt_bdaddr_t *address, bool enable);

/**
 ****************************************************************************************
 * @brief       Callback for AT+BCS and event from BAC
 * @param[in]   wbs: WBS enable, WBS disable
 * @param[in]   address: remote device address
 ****************************************************************************************
 */
typedef void (*bts_hfp_ag_wbs_cb)(const bt_bdaddr_t *address, bool enable, bt_hfp_audio_codec_t codec);

/**
 ****************************************************************************************
 * @brief       Callback for sending DTMF tones
 * @param[in]   tone: contains the dtmf character to be sent
 * @param[in]   address: remote device address
 ****************************************************************************************
 */
typedef void (*bts_hfp_ag_dtmf_req_cb)(const bt_bdaddr_t *address, char tone);

/**
 ****************************************************************************************
 * @brief       Callback for CNUM (subscriber number)
 * @param[in]   address remote device address
 ****************************************************************************************
 */
typedef void (*bts_hfp_ag_at_cnum_req_cb)(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Callback for indicators (CIND)
 * @param[in]   address remote device address
 ****************************************************************************************
 */
typedef void (*bts_hfp_ag_at_cind_req_cb)(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Callback for operator selection (COPS)
 * @param[in]   address remote device address
 ****************************************************************************************
 */
typedef void (*bts_hfp_ag_at_cops_req_cb)(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Callback for call list (AT+CLCC)
 * @param[in]   address remote device address
 ****************************************************************************************
 */
typedef void (*bts_hfp_ag_at_clcc_req_cb)(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Callback for BIND. Pass the remote HF Indicators supported.
 * @param[in]   at_string unparsed AT command string
 * @param[in]   address remote device address
 ****************************************************************************************
 */
typedef void (*bts_hfp_ag_at_bind_req_cb)(const bt_bdaddr_t *address, const char* at_string);

/**
 ****************************************************************************************
 * @brief       Callback for BIEV. Pass the change in the Remote HF indicator values
 * @param[in]   ind_id HF indicator id
 * @param[in]   ind_value HF indicator value
 * @param[in]   address remote device address
 ****************************************************************************************
 */
typedef void (*bts_hfp_ag_at_biev_req_cb)(const bt_bdaddr_t *address, bts_hfp_hf_ind_type_t ind_id, int ind_value);

/**
 ****************************************************************************************
 * @brief       Callback for BIA. Pass the change in AG indicator activation.
 *              - Call, Call Setup and Call Held indicators are mandatory and cannot
 *                be disabled. Thus, they are not included here.
 * @param[in]   service whether HF should receive network service state update
 * @param[in]   roam whether HF should receive roaming state update
 * @param[in]   signal whether HF should receive signal strength update
 * @param[in]   battery whether HF should receive AG battery level update
 * @param[in]   address remote HF device address
 ****************************************************************************************
 */
typedef void (*bts_hfp_ag_at_bia_req_cb)(const bt_bdaddr_t *address, bool service, bool roam, bool signal, bool battery);

typedef struct
{
    bts_hfp_ag_connection_state_cb connection_state_cb;
    bts_hfp_ag_audio_status_cb audio_status_cb;
    bts_hfp_ag_voice_recognition_cb voice_recog_status_cb;
    bts_hfp_ag_answer_call_cb answer_call_cb;
    bts_hfp_ag_hangup_call_cb hangup_call_cb;
    bts_hfp_ag_spk_volume_change_cb spk_volume_cb;
    bts_hfp_ag_mic_volume_change_cb mic_volume_cb;
    bts_hfp_ag_dial_cb dial_cb;
    bts_hfp_ag_dial_memory_cb dial_memory_cb;
    bts_hfp_ag_redial_cb redial_cb;
    bts_hfp_ag_noise_reduction_cb noise_reduction_cb;
    bts_hfp_ag_wbs_cb wbs_cb;
    bts_hfp_ag_dtmf_req_cb dtmf_cmd_cb;
    bts_hfp_ag_at_cnum_req_cb at_cnum_cb;
    bts_hfp_ag_at_cind_req_cb at_cind_cb;
    bts_hfp_ag_at_cops_req_cb at_cops_cb;
    bts_hfp_ag_at_clcc_req_cb at_clcc_cb;
    bts_hfp_ag_at_bind_req_cb at_bind_cb;
    bts_hfp_ag_at_biev_req_cb at_biev_cb;
    bts_hfp_ag_at_bia_req_cb at_bia_cb;
} bts_hfp_ag_callbacks_t;

typedef enum {
    BTS_HFP_AG_CB_USER_APP      = 0,
    BTS_HFP_AG_CB_USER_DRIVER,
    BTS_HFP_AG_CB_USER_MAX,
} BTS_HFP_AG_CALLBACK_USER_E;

/**
 ****************************************************************************************
 * @brief       Register the hfp_hf_callbacks.
 * @param[in]   callbacks: Pointer to callbacks for handling hfp hf events.
 * @return      none
 ****************************************************************************************
 */
void bts_hfp_ag_register_callbacks(BTS_HFP_AG_CALLBACK_USER_E user, bts_hfp_ag_callbacks_t *callbacks);

void bts_hfp_ag_deregister_callbacks(BTS_HFP_AG_CALLBACK_USER_E user);

/**
 ****************************************************************************************
 * @brief       Connect to hf
 * @param[in]   address: remote device address.
 * @return      Result of operation.
 ****************************************************************************************
 */
bts_status_t bts_hfp_ag_connect(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Disconnect to hf
 * @param[in]   address: remote device address.
 * @return      Result of operation.
 ****************************************************************************************
 */
bts_status_t bts_hfp_ag_disconnect(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Create an audio connection
 * @param[in]   address: remote device address.
 * @return      Result of operation.
 ****************************************************************************************
 */
bts_status_t bts_hfp_ag_connect_audio(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Close the audio connection
 * @param[in]   address: remote device address.
 * @return      Result of operation.
 ****************************************************************************************
 */
bts_status_t bts_hfp_ag_disconnect_audio(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Checks whether the device support echo cancellation and/or noise reduction
 * @param[in]   address: remote device address.
 * @return      Result of operation.
 ****************************************************************************************
 */
bool bts_hfp_ag_is_ec_and_nr_supported(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Checks whether the device supports voice recognition
 * @param[in]   address: remote device address.
 * @return      Result of operation.
 ****************************************************************************************
 */
bool bts_hfp_ag_is_voice_recognition_supported(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Start voice recognition
 * @param[in]   address: remote device address.
 * @return      Result of operation.
 ****************************************************************************************
 */
bts_status_t bts_hfp_ag_start_voice_recognition(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Stop voice recognition
 * @param[in]   address: remote device address.
 * @return      Result of operation.
 ****************************************************************************************
 */
bts_status_t bts_hfp_ag_stop_voice_recognition(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Change HFP speaker volume on remote headset
 * @param[in]   address: remote device address.
 * @param[in]   volume: volume level on scale from 0 to 15, p69, HFP 1.7.1 spec
 * @return      Result of operation.
 ****************************************************************************************
 */
bts_status_t bts_hfp_ag_set_speaker_volume(const bt_bdaddr_t *address, uint8_t volume);

/**
 ****************************************************************************************
 * @brief       Change HFP microphone volume on remote headset
 * @param[in]   address: remote device address.
 * @param[in]   volume: volume level on scale from 0 to 15, p69, HFP 1.7.1 spec
 * @return      Result of operation.
 ****************************************************************************************
 */
bts_status_t bts_hfp_ag_set_microphone_volume(const bt_bdaddr_t *address, uint8_t volume);

/**
 ****************************************************************************************
 * @brief       Enable/Disable hfp ag in band ringtone
 * @param[in]   address: remote device address.
 * @param[in]   enable: whether or not
 * @return      Result of operation.
 ****************************************************************************************
 */
bts_status_t bts_hfp_ag_set_in_band_ringtone_enable(const bt_bdaddr_t *address, bool enable);

/**
 ****************************************************************************************
 * @brief       Combined device status change notification
 * @param[in]   status: device status structure
 * @return      Result of operation.
 ****************************************************************************************
 */
bts_status_t bts_hfp_ag_device_status_change(const bt_bdaddr_t *address, const bts_hfp_ag_device_status_t *status);

/**
 ****************************************************************************************
 * @brief       Notify of a call state change
 * @param[in]   status: phone status structure
 * @return      Result of operation.
 ****************************************************************************************
 */
bts_status_t bts_hfp_ag_phone_status_change(const bt_bdaddr_t *address, const bts_hfp_ag_phone_status_t *status);

/**
 ****************************************************************************************
 * @brief       Response for COPS (Query Operator Selection) command
 * @param[in]   cops: Operator Name, max length 16 char, p32 HFP 1.7.1 spec
 * @return      Result of operation.
 ****************************************************************************************
 */
bts_status_t bts_hfp_ag_cops_response(const bt_bdaddr_t *address, const char *cops);

/**
 ****************************************************************************************
 * @brief       Response for CIND (Stanford Indicator Update) command
 * @param[in]   status: cind status structure
 * @return      Result of operation.
 ****************************************************************************************
 */
bts_status_t bts_hfp_ag_cind_response(const bt_bdaddr_t *address, const bts_hfp_ag_cind_status_t *status);

/**
 ****************************************************************************************
 * @brief       Response for CLCC (Current List of Calls) command.
 * @param[in]   status: clcc status structure
 * @return      Result of operation.
 ****************************************************************************************
 */
bts_status_t bts_hfp_ag_clcc_response(const bt_bdaddr_t *address, const bts_hfp_ag_clcc_status_t *status);

/**
 ****************************************************************************************
 * @brief       ok/error response to AT commands
 * @param[in]   address: remote device address.
 * @param[in]   response_code: OK or ERROR
 * @param[in]   error_code: actual error code depend on use case
 * @return      Result of operation.
 ****************************************************************************************
 */
bts_status_t bts_hfp_ag_send_at_response_code(const bt_bdaddr_t *address, bt_hfp_at_response_t error_code, int cme_error);

/**
 ****************************************************************************************
 * @brief       Send AT cmd result
 * @param[in]   address: remote device address.
 * @param[in]   at_result: AT cmd
 * @return      Result of operation.
 ****************************************************************************************
 */
bts_status_t bts_hfp_ag_send_at_result(const bt_bdaddr_t *address, const char *at_result);

/**
 ****************************************************************************************
 * @brief       Let upper layer handle AT cmd, stack can't auto response
 * @param[in]   address: remote device address.
 * @param[in]   at_result: AT cmd
 * @return      Result of operation.
 ****************************************************************************************
 */
bts_status_t bts_hfp_ag_set_curr_at_upper_handle(const bt_bdaddr_t *address);


#ifdef __cplusplus
}
#endif

#endif /* __BTS_HFP_API_H__ */