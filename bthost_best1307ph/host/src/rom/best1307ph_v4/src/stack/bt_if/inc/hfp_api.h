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
#ifndef __HFP_API_H__
#define __HFP_API_H__
#include "bluetooth.h"
#include "hci_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AG_AT_CMD_UPPER_HANDLE AT_CMD_UPPER_HANDLE

typedef uint16_t hf_gateway_version;

/* Unable to determine the Hands Free Profile version that is supported */
#define BTIF_HF_GW_VERSION_UNKNOWN 0x0000

/* Supports Version 0.96 of the Hands Free Profile */
#define BTIF_HF_GW_VERSION_0_96    0x0100

/* Supports Version 1.0 of the Hands Free Profile */
#define BTIF_HF_GW_VERSION_1_0     0x0101

/* Supports Version 1.5 of the Hands Free Profile */
#define BTIF_HF_GW_VERSION_1_5     0x0105

#define BTIF_HF_GW_VERSION_1_6     0x0106

#define BTIF_HF_GW_VERSION_1_7     0x0107

#define MAX_HFP_VOL    (15)
#define MIN_HFP_VOL    (0)
#define HFP_DEFAULT_VOLUME (7)

typedef enum {
    BT_HFP_CHAN_STATE_CLOSED = 0,
    BT_HFP_CHAN_STATE_CONN_PENDING = 1,
    BT_HFP_CHAN_STATE_CONN_INCOMING = 2,
    BT_HFP_CHAN_STATE_NEGOTIATE = 3,
    BT_HFP_CHAN_STATE_OPEN = 4,
    BT_HFP_CHAN_STATE_DISC = 5,
} bt_hfp_chan_state_t;

typedef enum {
    BT_HFP_AUDIO_DISCON = 0,
    BT_HFP_AUDIO_CON = 1,
} bt_audio_state_t;

typedef uint8_t bt_hfp_call_active_t;
typedef uint8_t bt_hfp_call_setup_t;
typedef uint8_t bt_hfp_call_held_t;

#define BT_HFP_CALL_NONE    0
#define BT_HFP_CALL_ACTIVE  1

#define BT_HFP_CALL_SETUP_NONE  0
#define BT_HFP_CALL_SETUP_IN    1
#define BT_HFP_CALL_SETUP_OUT   2
#define BT_HFP_CALL_SETUP_ALERT 3

#define BT_HFP_CALL_HELD_NONE       0
#define BT_HFP_CALL_HELD_ACTIVE     1
#define BT_HFP_CALL_HELD_NO_ACTIVE  2


#define HFP_KEY_ANSWER_CALL             8
#define HFP_KEY_HANGUP_CALL             9
#define HFP_KEY_REDIAL_LAST_CALL        10
#define HFP_KEY_CHANGE_TO_PHONE         11
#define HFP_KEY_ADD_TO_EARPHONE         12
#define HFP_KEY_MUTE                    13
#define HFP_KEY_CLEAR_MUTE              14
//3way calls oper
#define HFP_KEY_THREEWAY_HOLD_AND_ANSWER              15
#define HFP_KEY_THREEWAY_HANGUP_AND_ANSWER            16
#define HFP_KEY_THREEWAY_HOLD_REL_INCOMING            17
#define HFP_KEY_THREEWAY_HOLD_ADD_HELD_CALL           18

#define HFP_KEY_DUAL_HF_HANGUP_ANOTHER                19
#define HFP_KEY_DUAL_HF_HANGUP_CURR_ANSWER_ANOTHER    20
#define HFP_KEY_DUAL_HF_HOLD_CURR_ANSWER_ANOTHER      21
#define HFP_KEY_DUAL_HF_CHANGETOPHONE_ANSWER_ANOTHER  22
#define HFP_KEY_DUAL_HF_CHANGETOPHONE_ANOTHER_ADDTOEARPHONE 23
#define HFP_KEY_DUAL_HF_HANGUP_ANOTHER_ADDTOEARPHONE 24
#define HFP_KEY_DUAL_HF_CHANGETOPHONE_ANSWER_CURR 25

typedef enum {
    BT_HF_FEAT_ECNR         = 0x0001,
    BT_HF_FEAT_3WAY         = 0x0002,
    BT_HF_FEAT_CLI          = 0x0004,
    BT_HF_FEAT_VR           = 0x0008,
    BT_HF_FEAT_RVC          = 0x0010,
    BT_HF_FEAT_ECS          = 0x0020,
    BT_HF_FEAT_ECC          = 0x0040,
    BT_HF_FEAT_CODEC        = 0x0080,
    BT_HF_FEAT_HF_IND       = 0x0100,
    BT_HF_FEAT_ESCO_S4_T2   = 0x0200,
} bt_hf_feature_t;

typedef enum {
    BT_AG_FEAT_3WAY        = 0x0001,
    BT_AG_FEAT_ECNR        = 0x0002,
    BT_AG_FEAT_VR          = 0x0004,
    BT_AG_FEAT_INBAND      = 0x0008,
    BT_AG_FEAT_VTAG        = 0x0010,
    BT_AG_FEAT_REJ_CALL    = 0x0020,
    BT_AG_FEAT_ECS         = 0x0040,
    BT_AG_FEAT_ECC         = 0x0080,
    BT_AG_FEAT_EXT_ERR     = 0x0100,
    BT_AG_FEAT_CODEC       = 0x0200,
    BT_AG_FEAT_HF_IND      = 0x0400,
    BT_AG_FEAT_ESCO_S4_T2  = 0x0800,
} bt_ag_feature_t;

typedef enum {
    BT_HF_VR_STATE_STOPPED = 0,
    BT_HF_VR_STATE_STARTED,
} bt_hf_vr_state_t;

typedef enum {
    BT_HF_VOLUME_TYPE_SPK = 0,
    BT_HF_VOLUME_TYPE_MIC
} bt_hf_volume_type_t;

typedef enum {
    BT_HF_NETWORK_STATE_NOT_AVAILABLE = 0,
    BT_HF_NETWORK_STATE_AVAILABLE,
} bt_hf_network_state_t;

typedef enum {
    BT_HF_SERVICE_TYPE_HOME = 0,
    BT_HF_SERVICE_TYPE_ROAMING,
} bt_hf_service_type_t;

typedef enum {
    BT_HF_CALL_NO_CALLS_IN_PROGRESS = 0,
    BT_HF_CALL_CALLS_IN_PROGRESS,
} bt_hf_call_ind_t;

typedef enum {
    BT_HF_CALLSETUP_NONE = 0,
    BT_HF_CALLSETUP_INCOMING,
    BT_HF_CALLSETUP_OUTGOING,
    BT_HF_CALLSETUP_ALERTING,
} bt_hf_callsetup_ind_t;

typedef enum {
    BT_HF_CALLHELD_NONE = 0,
    BT_HF_CALLHELD_HOLD_AND_ACTIVE,
    BT_HF_CALLHELD_HOLD,
} bt_hf_callheld_ind_t;

typedef enum {
    BT_HF_CALL_STATE_ACTIVE = 0,
    BT_HF_CALL_STATE_HELD,
    BT_HF_CALL_STATE_DIALING,
    BT_HF_CALL_STATE_ALERTING,
    BT_HF_CALL_STATE_INCOMING,
    BT_HF_CALL_STATE_WAITING,
    BT_HF_CALL_STATE_HELD_BY_RESP_HOLD,
    BT_HF_CALL_STATE_DISCONNECTED,
} bt_hf_call_state_t;

typedef enum {
    BT_HF_CALL_DIRECTION_OUTGOING = 0,
    BT_HF_CALL_DIRECTION_INCOMING,
} bt_hf_call_direction_t;

typedef enum {
    BT_HF_CALL_MPTY_TYPE_SINGLE = 0,
    BT_HF_CALL_MPTY_TYPE_MULTI,
} bt_hf_call_mpty_type_t;

typedef enum {
    BT_HF_CALL_ACTION_CHLD_0 = 0,
    BT_HF_CALL_ACTION_CHLD_1,
    BT_HF_CALL_ACTION_CHLD_2,
    BT_HF_CALL_ACTION_CHLD_3,
    BT_HF_CALL_ACTION_CHLD_4,
    BT_HF_CALL_ACTION_CHLD_1x,
    BT_HF_CALL_ACTION_CHLD_2x,
    BT_HF_CALL_ACTION_ATA,
    BT_HF_CALL_ACTION_CHUP,
    BT_HF_CALL_ACTION_REDIAL,
    BT_HF_CALL_ACTION_BTRH_0,
    BT_HF_CALL_ACTION_BTRH_1,
    BT_HF_CALL_ACTION_BTRH_2,
} bt_hf_call_action_t;

typedef enum {
    BT_HF_RESP_AND_HOLD_HELD = 0,
    BT_HF_RESP_AND_HOLD_ACCEPT,
    BT_HF_RESP_AND_HOLD_REJECT,
} bt_hf_resp_and_hold_t;

typedef enum {
    BT_HF_CALL_MODE_VOICE = 0,
    BT_HF_CALL_MODE_DATA,
    BT_HF_CALL_MODE_FAX,
} bt_hf_call_mode_t;

typedef enum {
    BT_HF_CALL_SERVICE_UNKNOWN = 0,
    BT_HF_CALL_SERVICE_VOICE = 4,
    BT_HF_CALL_SERVICE_FAX = 5,
} bt_hf_cnum_service_t;

typedef enum {
    BT_HF_IN_BAND_RINGTONE_NOT_PROVIDED = 0,
    BT_HF_IN_BAND_RINGTONE_PROVIDED,
} bt_hf_in_band_ring_state_t;

typedef enum {
    BT_HF_AUDIO_CODEC_NONE = 0,
    BT_HF_AUDIO_CODEC_CVSD = 1,
    BT_HF_AUDIO_CODEC_MSBC = 2,
    BT_HF_AUDIO_CODEC_XXXX = 16,
} bt_hf_audio_codec_t;

typedef enum {
    BT_HF_CMD_COMPLETE_OK = 0,
    BT_HF_CMD_COMPLETE_ERROR,
    BT_HF_CMD_COMPLETE_ERROR_NO_CARRIER,
    BT_HF_CMD_COMPLETE_ERROR_BUSY,
    BT_HF_CMD_COMPLETE_ERROR_NO_ANSWER,
    BT_HF_CMD_COMPLETE_ERROR_DELAYED,
    BT_HF_CMD_COMPLETE_ERROR_BLACKLISTED,
    BT_HF_CMD_COMPLETE_ERROR_CME,
} bt_hf_cmd_complete_t;

typedef enum {
    BT_HF_AT_RESPONSE_OK = 0,
    BT_HF_AT_RESPONSE_ERROR,
    BT_HF_AT_RESPONSE_ERROR_NO_CARRIER,
    BT_HF_AT_RESPONSE_ERROR_BUSY,
    BT_HF_AT_RESPONSE_ERROR_NO_ANSWER,
    BT_HF_AT_RESPONSE_ERROR_DELAYED,
    BT_HF_AT_RESPONSE_ERROR_BLACKLISTED,
    BT_HF_AT_RESPONSE_ERROR_CME,
} bt_hf_at_response_t;

typedef enum {
    BT_HF_CALL_NUMBER_TYPE_UNKNOWN = 0x81,
    BT_HF_CALL_NUMBER_TYPE_INTERNATIONAL = 0x91,
} bt_hf_call_number_type_t;

typedef enum {
    BT_HF_IND_ENHANCED_DRIVER_SAFETY = 1,
    BT_HF_IND_BATTERY_LEVEL_STATUS = 2,
} bt_hf_ind_type_t;

typedef enum
{
    HFCALL_MACHINE_CURRENT_IDLE = 0,                                //0
    HFCALL_MACHINE_CURRENT_INCOMMING,                               //1
    HFCALL_MACHINE_CURRENT_OUTGOING,                                //2
    HFCALL_MACHINE_CURRENT_CALLING,                                 //3
    HFCALL_MACHINE_CURRENT_3WAY_INCOMMING,                          //4
    HFCALL_MACHINE_CURRENT_3WAY_HOLD_CALLING,                       //5
    HFCALL_MACHINE_CURRENT_IDLE_ANOTHER_IDLE,                       //6
    HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_IDLE,                  //7
    HFCALL_MACHINE_CURRENT_OUTGOING_ANOTHER_IDLE,                   //8
    HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_IDLE,                    //9
    HFCALL_MACHINE_CURRENT_3WAY_INCOMMING_ANOTHER_IDLE,             //10
    HFCALL_MACHINE_CURRENT_3WAY_HOLD_CALLING_ANOTHER_IDLE,          //11
    HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_CALLING,               //13
    HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_INCOMMING,             //14
    HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_INCOMMING,               //15
    HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_CHANGETOPHONE,           //16
    HFCALL_MACHINE_CURRENT_OUTGOING_ANOTHER_INCOMMING,              //17
    HFCALL_MACHINE_CURRENT_OUTGOING_ANOTHER_CALLING,
    HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_OUTGOING,              //18
    HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_HOLD,                    //19
    HFCALL_MECHINE_CURRENT_3WAY_HOLD_ANOTHER_IDLE,                  //20
    HFCALL_MACHINE_NUM
} HFCALL_MACHINE_ENUM;

typedef struct {
    uint8_t error_code;
    uint8_t call_status;
    uint8_t callsetup_status;
    uint8_t callhold_status;
    uint32_t peer_feat;
} bt_hf_opened_param_t;

typedef struct {
    uint8_t error_code;
} bt_hf_closed_param_t;

typedef struct {
    uint8_t error_code;
    bt_hf_audio_codec_t codec;
    uint16_t sco_handle;
} bt_hf_audio_opened_param_t;

typedef struct {
    uint8_t error_code;
} bt_hf_audio_closed_param_t;

typedef struct {
    bt_hf_network_state_t state;
} bt_hf_network_state_param_t;

typedef struct {
    bt_hf_service_type_t type;
} bt_hf_network_roaming_param_t;

typedef struct {
    uint8_t signal_strength;
} bt_hf_network_signal_param_t;

typedef struct {
    uint8_t battery_level;
} bt_hf_battery_level_param_t;

typedef struct {
    bt_hf_call_ind_t call;
} bt_hf_call_ind_param_t;

typedef struct {
    bt_hf_callsetup_ind_t callsetup;
} bt_hf_callsetup_ind_param_t;

typedef struct {
    bt_hf_callheld_ind_t callheld;
} bt_hf_callheld_ind_param_t;

typedef struct {
    const char *number;
    int number_len;
} bt_hf_clip_ind_param_t;

typedef struct {
    const char *number;
    int number_len;
} bt_hf_call_waiting_ind_param_t;

typedef struct {
    bt_hf_resp_and_hold_t resp_and_hold;
} bt_hf_resp_and_hold_param_t;

typedef struct {
    int index;
    bt_hf_call_direction_t dir;
    bt_hf_call_state_t state;
    bt_hf_call_mode_t mode;
    bt_hf_call_mpty_type_t mpty;
    const char *number;
} bt_hf_current_calls_param_t;

typedef struct {
    bt_hf_volume_type_t type;
    int volume;
} bt_hf_volume_change_param_t;

typedef struct {
    bt_hf_volume_type_t type;
    int volume;
} bt_hf_volume_control_param_t;

typedef struct {
    bt_hf_cmd_complete_t type;
    int cme_error;
} bt_hf_at_cmd_complete_param_t;

typedef struct {
    const char *name;
} bt_hf_current_operator_param_t;

typedef struct {
    const char *number;
    bt_hf_cnum_service_t service;
} bt_hf_subscriber_info_param_t;

typedef struct {
    bt_hf_vr_state_t state;
} bt_hf_vr_state_change_param_t;

typedef struct {
    bt_hf_in_band_ring_state_t state;
} bt_hf_in_band_ring_state_param_t;

typedef struct {
    const char *number;
} bt_hf_last_voice_tag_number_param_t;

typedef struct {
    const char *time_str;
    // * format: "yy/MM/dd,hh:mm:ss±zz", total len: 20
} bt_hf_cclk_time_param_t;

typedef struct {
    const char *remote_unknown_event;
} bt_hf_receive_unknown_param_t;

typedef union {
    bt_hf_opened_param_t *opened;
    bt_hf_closed_param_t *closed;
    bt_hf_audio_opened_param_t *audio_opened;
    bt_hf_audio_closed_param_t *audio_closed;
    bt_hf_network_state_param_t *network_state;
    bt_hf_network_roaming_param_t *network_roaming;
    bt_hf_network_signal_param_t *network_signal;
    bt_hf_battery_level_param_t *battery_level;
    bt_hf_call_ind_param_t *call_ind;
    bt_hf_callsetup_ind_param_t *callsetup_ind;
    bt_hf_callheld_ind_param_t *callheld_ind;
    bt_hf_clip_ind_param_t *clip_ind;
    bt_hf_call_waiting_ind_param_t *call_waiting;
    bt_hf_resp_and_hold_param_t *rsp_and_hold;
    bt_hf_current_calls_param_t *current_calls;
    bt_hf_volume_change_param_t *volume_change;
    bt_hf_at_cmd_complete_param_t *cmd_complete;
    bt_hf_current_operator_param_t *current_operator;
    bt_hf_subscriber_info_param_t *subscriber_info;
    bt_hf_vr_state_change_param_t *vr_state;
    bt_hf_in_band_ring_state_param_t *in_band_ring_state;
    bt_hf_last_voice_tag_number_param_t *last_voice_tag_number;
    bt_hf_cclk_time_param_t *cclk_time;
    bt_hf_receive_unknown_param_t *receive_unknown;
} bt_hf_callback_param_t;

typedef enum {
    BT_HFP_SCO_CODEC_NONE = 0,
    BT_HFP_SCO_CODEC_CVSD = 1,
    BT_HFP_SCO_CODEC_MSBC = 2,
    BT_HFP_SCO_CODEC_LC3_SWB = 3,     // For LC3_SWB Codec
    BT_HFP_SCO_CODEC_XXXX = 16,    // Cunstomer Codec
} hfp_sco_codec_t;

enum HFP_ROLE
{
    HFP_ROLE_AG,
    HFP_ROLE_HF,
    HFP_ROLE_UNKNOWN,
};

typedef enum {
    BTIF_HF_HOLD_RELEASE_HELD_CALLS = 0,
    BTIF_HF_HOLD_RELEASE_ACTIVE_CALLS = 1,
    BTIF_HF_HOLD_HOLD_ACTIVE_CALLS = 2,
    BTIF_HF_HOLD_ADD_HELD_CALL = 3,
    BTIF_HF_HOLD_CALL_TRANSFER = 4,
} btif_hf_hold_call_t;

typedef struct{
    uint8_t device_id;
    btif_hf_hold_call_t action;
} app_hfp_hold_call_t;

typedef enum app_hfp_hf_event {
    APP_HFP_HF_EVENT_SERVICE_CONNECTED = 0,
    APP_HFP_HF_EVENT_SERVICE_DISCONNECTED,
    APP_HFP_HF_EVENT_RING_IND,
    APP_HFP_HF_EVENT_CALLER_ID_IND,
    APP_HFP_HF_EVENT_AUDIO_CONNECTED,
    APP_HFP_HF_EVENT_AUDIO_DISCONNECTED,
} app_hfp_hf_event_t;

typedef struct { /* empty */ } btif_hf_channel_t; /* used to hold the pointer to struct _hshf_channel */

typedef struct app_hfp_hf_event_param_t {
    union {
        struct {
            int device_id;
            bt_bdaddr_t *addr;
            btif_hf_channel_t *channel;
        } service_connected;
        struct {
            int device_id;
            btif_hf_channel_t *channel;
        } service_disconnected;
        struct {
            int device_id;
            const char *number;
            btif_hf_channel_t *channel;
        } ring_ind;
        struct {
            int device_id;
            int number_len;
            const char *number;
            btif_hf_channel_t *channel;
        } caller_id_ind;
        struct {
            int device_id;
            btif_hf_channel_t *channel;
        } audio_connected;
        struct {
            int device_id;
            btif_hf_channel_t *channel;
        } audio_disconnected;
    } p;
} app_hfp_hf_event_param_t;

typedef int (*app_hfp_hf_callback_t)(app_hfp_hf_event_t event, app_hfp_hf_event_param_t *param);

typedef struct bt_hfp_state_t {
    bt_bdaddr_t remote;
    uint8_t hfp_is_connected;
    uint8_t sco_is_connected;
    uint8_t hfp_nego_codec;
    uint8_t call_is_active;
    bt_hfp_chan_state_t hfp_chan_state;
    bt_hfp_call_setup_t callsetup_state;
    bt_hfp_call_held_t callheld_state;
    uint8_t service_status;
    uint8_t signal_status;
    uint8_t roam_status;
    uint8_t battery_status;
    uint8_t spk_volume;
    uint8_t mic_volume;
    uint8_t in_band_ring_enable;
    uint8_t voice_rec_state;
} bt_hfp_state_t;

typedef enum {
    BT_HFP_ANSWER_CALL = 1,
    BT_HFP_HANGUP_CALL,
    BT_HFP_REDIAL_CALL,
} BT_HFP_CALL_ACTION_T;

typedef void (*hfp_vol_sync_done_cb)(void);

// Maintain consistency with MAX_NUMBER_LEN
#define MAX_CARD_NUMBER_LEN               (33)
struct hfp_sim_card_number {
    char number[MAX_CARD_NUMBER_LEN];
};


typedef enum {
    BTIF_HF_EVENT_SERVICE_CONNECT_REQ = 1,
    BTIF_HF_EVENT_SERVICE_CONNECTED = 2,
    BTIF_HF_EVENT_SERVICE_DISCONNECTED = 3,
    BTIF_HF_EVENT_AUDIO_CONNECTED = 4,
    BTIF_HF_EVENT_AUDIO_DISCONNECTED = 5,
    BTIF_HF_EVENT_AUDIO_DATA = 6,
    BTIF_HF_EVENT_AUDIO_DATA_SENT = 7,
    BTIF_HF_EVENT_GATEWAY_FEATURES = 8,
    BTIF_HF_EVENT_GW_HOLD_FEATURES = 9,
    BTIF_HF_EVENT_CALL_STATE = 10,
    BTIF_HF_EVENT_CALLER_ID = 11,
    BTIF_HF_EVENT_CALL_LISTING_ENABLED = 12,
    BTIF_HF_EVENT_RESPONSE_HOLD_APPL = 13,
    BTIF_HF_EVENT_CALL_IND = 14,
    BTIF_HF_EVENT_CALLSETUP_IND = 15,
    BTIF_HF_EVENT_CALLHELD_IND = 16,
    BTIF_HF_EVENT_RING_IND = 17,
    BTIF_HF_EVENT_WAIT_NOTIFY = 18,
    BTIF_HF_EVENT_CALLER_ID_NOTIFY = 19,
    BTIF_HF_EVENT_CURRENT_CALL_STATE = 20,
    BTIF_HF_EVENT_RESPONSE_HOLD = 21,
    BTIF_HF_EVENT_SERVICE_IND = 22,
    BTIF_HF_EVENT_BATTERY_IND = 23,
    BTIF_HF_EVENT_SIGNAL_IND = 24,
    BTIF_HF_EVENT_ROAM_IND = 25,
    BTIF_HF_EVENT_SMS_IND = 26,
    BTIF_HF_EVENT_VOICE_REC_STATE = 27,
    BTIF_HF_EVENT_VOICE_TAG_NUMBER = 28,
    BTIF_HF_EVENT_SPEAKER_VOLUME = 29,
    BTIF_HF_EVENT_MIC_VOLUME = 30,
    BTIF_HF_EVENT_IN_BAND_RING = 31,
    BTIF_HF_EVENT_NETWORK_OPERATOR = 32,
    BTIF_HF_EVENT_SUBSCRIBER_NUMBER = 33,
    BTIF_HF_EVENT_NO_CARRIER = 34,
    BTIF_HF_EVENT_BUSY = 35,
    BTIF_HF_EVENT_NO_ANSWER = 36,
    BTIF_HF_EVENT_DELAYED = 37,
    BTIF_HF_EVENT_BLACKLISTED = 38,
    BTIF_HF_EVENT_PHONEBOOK_STORAGE = 39,
    BTIF_HF_EVENT_PHONEBOOK_INFO = 40,
    BTIF_HF_EVENT_PHONEBOOK_SIZE = 41,
    BTIF_HF_EVENT_PHONEBOOK_ENTRY = 42,
    BTIF_HF_EVENT_AT_RESULT_DATA = 43,
    BTIF_HF_EVENT_COMMAND_COMPLETE = 44,
    BTIF_HF_EVENT_SIRI_STATUS = 45,
    BTIF_HF_EVENT_READ_AG_INDICATORS_STATUS = 46,
    BTIF_HF_EVENT_BES_TEST = 47,
    BTIF_HF_EVENT_SELECT_CHANNEL = 48,
    BTIF_HF_EVENT_RFCOMM_CONNECTED = 49,
    BTIF_HF_EVENT_REMOTE_NOT_SUPPORT = 50,
    BTIF_HF_EVENT_AUDIO_MOCK_CONNECTED = 51,
    BTIF_HF_EVENT_AUDIO_MOCK_DISCONNECTED = 52,
    BTIF_HF_EVENT_SERVICE_MOCK_CONNECTED = 53,
    BTIF_HF_EVENT_AUDIO_CON_FAIL = 54,
} hf_event_t;

struct hfp_context {
    uint16_t remote_dev_hcihandle;
    /*TODO:should remove the status */
    bt_status_t status;         /* Status of the callback event             */
    bt_hfp_chan_state_t state;
    bt_bdaddr_t remote_dev_bdaddr;
    const char *call_number;
    const char *sim_card_number;
    bt_hfp_call_active_t call;
    bt_hfp_call_setup_t call_setup;
    bt_hfp_call_held_t call_held;
    hf_event_t event;
    uint8_t *audio_data;
    uint16_t audio_data_len;
    uint8_t error_code;
    uint8_t cme_reason;
    uint8_t disc_reason;
    uint8_t disc_reason_saved;
    uint8_t speaker_volume;
    uint8_t mic_volume;
    uint8_t bsir_enable;
    uint8_t voice_rec_state;
    uint8_t ciev_status;
    uint8_t rsp_hold_status;
    void* data;
    uint32_t **chan_sel_channel;
    void *chan_sel_remDev;
    char *ptr;
    uint8_t sco_codec;
    uint8_t sco_con_fail_reason;
};

/* End of hf_gateway_version */

struct hfp_vendor_info {
    uint16_t vendor_id;
    uint16_t product_id;
    uint16_t version_id;
    uint16_t feature_id;  //default is 3, if you do not known, set it 0
};

typedef struct {
    uint8_t device_id;
    struct bdaddr_t *remote;
    // 0: connect_fail or disconnected, 1:connected
    uint8_t connected;
    // state = BTM_CONN_SCO_OPENED, set this param
    uint8_t* connected_codec;
} hfp_set_sco_state_param_t;

typedef void (*hf_event_cb_t) (uint8_t device_id, btif_hf_channel_t* chan, struct hfp_context * ctx);

struct btif_hf_cind_value {
    uint8_t index;
    uint8_t min;
    uint8_t max;
    uint8_t value;
    bool initial_support;
};

struct hshf_control;
struct _hshf_channel;

struct btif_hf_cind_value btif_hf_get_cind_service_value(btif_hf_channel_t* chan_h);
struct btif_hf_cind_value btif_hf_get_cind_call_value(btif_hf_channel_t* chan_h);
struct btif_hf_cind_value btif_hf_get_cind_callsetup_value(btif_hf_channel_t* chan_h);
struct btif_hf_cind_value btif_hf_get_cind_callheld_value(btif_hf_channel_t* chan_h);
struct btif_hf_cind_value btif_hf_get_cind_signal_value(btif_hf_channel_t* chan_h);
struct btif_hf_cind_value btif_hf_get_cind_roam_value(btif_hf_channel_t* chan_h);
struct btif_hf_cind_value btif_hf_get_cind_battchg_value(btif_hf_channel_t* chan_h);
uint32_t btif_hf_get_ag_features(btif_hf_channel_t *chan_h);
uint32_t btif_hf_get_chld_features(btif_hf_channel_t *chan_h);
void app_ibrt_register_sco_link(uint8_t device_id, bt_bdaddr_t *remote);

void btif_register_tws_current_ibrt_slave_role_callback(bool (*cb)(void* addr));
void btif_hf_set_sco_path_status(hfp_set_sco_state_param_t *param);

int btif_hfp_initialize(void);
int btif_hfp_deinit(void);
int btif_hf_register_callback(hf_event_cb_t callback);
int btif_ag_register_callback(hf_event_cb_t callback);
enum HFP_ROLE btif_hfp_get_role(uint8_t device_id);
hf_gateway_version btif_hf_get_version(btif_hf_channel_t* chan_h);
bt_status_t btif_hf_update_indicators_batt_level(btif_hf_channel_t* chan_h, uint32_t level);
bt_status_t btif_hf_batt_report(btif_hf_channel_t* chan_h, uint8_t level);
bt_status_t btif_hf_report_enhanced_safety(btif_hf_channel_t* chan_h, uint8_t value);
bt_status_t btif_hf_enable_voice_recognition(btif_hf_channel_t* chan_h, bool en);
bt_status_t btif_hf_batt_report(btif_hf_channel_t* chan_h, uint8_t level);
bool btif_hf_is_voice_rec_active(btif_hf_channel_t* chan_h);
bt_status_t btif_hf_disable_nrec(btif_hf_channel_t* chan_h);
bt_status_t btif_hf_report_speaker_volume(btif_hf_channel_t* chan_h, uint8_t gain);
bt_status_t btif_hf_report_mic_volume(btif_hf_channel_t* chan_h, uint8_t gain);
bt_status_t btif_hf_attach_voice_tag(btif_hf_channel_t* chan_h);
bt_status_t btif_hf_ind_activation(btif_hf_channel_t* chan_h);
bt_status_t btif_hf_send_at_cmd(btif_hf_channel_t* chan_h, const char *at_str);
bt_status_t btif_hf_list_current_calls(btif_hf_channel_t* chan_h);
bool btif_hf_is_hf_indicators_support(btif_hf_channel_t* chan_h);
bool btif_hf_is_batt_report_support(btif_hf_channel_t* chan_h);
bool btif_hf_is_sco_wait_codec_sync(btif_hf_channel_t* chan_h);
void btif_hf_set_negotiated_codec(btif_hf_channel_t* chan_h, hfp_sco_codec_t codec);
hfp_sco_codec_t btif_hf_get_negotiated_codec(btif_hf_channel_t* chan_h);
bt_status_t btif_hf_answer_call(btif_hf_channel_t* chan_h);
bt_status_t btif_hf_hang_up_call(btif_hf_channel_t* chan_h);
bt_status_t btif_hf_redial_call(btif_hf_channel_t* chan_h);
bt_status_t btif_hf_dial_number(btif_hf_channel_t* chan_h, uint8_t *number, uint16_t len);
bt_status_t btif_hf_dial_memory(btif_hf_channel_t* chan_h, int location);
bt_status_t btif_hf_call_action(int device_id, BT_HFP_CALL_ACTION_T action);
bt_status_t btif_hf_disc_audio_link(btif_hf_channel_t* chan_h);
bt_status_t btif_hf_create_audio_link(btif_hf_channel_t* chan_h);
bt_status_t btif_hf_create_sco_directly(btif_hf_channel_t* chan_h);
bt_status_t btif_hf_call_hold(btif_hf_channel_t* chan_h, btif_hf_hold_call_t action, uint8_t index);
bt_status_t btif_hf_switch_calls(btif_hf_channel_t* hangup_chan_h,
                                 btif_hf_channel_t* answer_chan_h);
uint8_t btif_get_hf_chan_audio_up_flag(btif_hf_channel_t* chan_h);
bt_hfp_chan_state_t btif_get_hf_chan_state(btif_hf_channel_t* chan_h);
bool btif_hf_check_AudioConnect_status(btif_hf_channel_t* chan_h);
btif_hf_channel_t* btif_hfp_get_channel(uint8_t device_id);
int btif_hf_init_channel(btif_hf_channel_t* chan_h);
bt_status_t btif_hf_disconnect_service_link(btif_hf_channel_t* chan_h);
bt_status_t btif_hf_create_service_link(bt_bdaddr_t * bt_addr);
void btif_hf_query_remote_sdp(bt_bdaddr_t *addr);
bool btif_hfp_profile_is_connecting(const bt_bdaddr_t *bdaddr_p);
bool btif_hf_get_remote_bdaddr(btif_hf_channel_t* chan_h, bt_bdaddr_t *bdaddr_p);
void btif_hfp_register_peer_sco_codec_receive_handler(void (*cb)(uint8_t device_id,void * chan,uint8_t codec));
void btif_hf_receive_peer_sco_codec_info(const void* remote, uint8_t codec);
uint16_t btif_hf_get_sco_hcihandle(btif_hf_channel_t* chan_h);
btif_hci_handle_t btif_hf_get_remote_hci_handle(btif_hf_channel_t* chan_h);
bool btif_hf_is_acl_connected(btif_hf_channel_t* chan_h);
struct hshf_control* _hshf_get_control_from_id(uint8_t device_id);
btif_remote_device_t *btif_hf_cmgr_get_remote_device(btif_hf_channel_t* chan_h);
bool btif_hf_check_rfcomm_l2cap_channel_is_creating(bt_bdaddr_t *bdaddr);
uint8 btif_hfp_get_rfcomm_dlci(const bt_bdaddr_t *remote);
bt_status_t btif_hf_send_audio_data(btif_hf_channel_t* chan_h, btif_bt_packet_t *packet);
bt_status_t btif_hf_is_inbandring_enabled(btif_hf_channel_t* chan_h);
bool btif_hfp_is_profile_initiator(const bt_bdaddr_t* remote);
bool btif_hf_is_virtual_call_enabled(btif_hf_channel_t* chan_h);
void btif_hf_set_virtual_call_enable(btif_hf_channel_t* chan_h);
void btif_hf_set_virtual_call_disable(btif_hf_channel_t* chan_h);
bt_status_t btif_hf_indicators_1(btif_hf_channel_t* chan_h);
bt_status_t btif_hf_indicators_2(btif_hf_channel_t* chan_h);
bt_status_t btif_hf_indicators_3(btif_hf_channel_t* chan_h);
bt_status_t btif_hf_send_key_pressed(btif_hf_channel_t* chan_h);

typedef void(*hfp_hold_result_callback)(uint8_t device_id, uint8_t result);
void btif_hfp_hold_result_callback(hfp_hold_result_callback cb);

#ifdef BT_HFP_AG_ROLE
typedef struct {
    bt_hf_network_state_t service;
    bt_hf_service_type_t roam;
    int signal;
    int battery_level;
} bt_ag_device_status_t;

typedef struct {
    bt_hf_network_state_t service;
    bt_hf_service_type_t roam;
    int signal;
    int battery_level;
    int num_active;
    int num_held;
    bt_hf_call_state_t call_state;
} bt_ag_cind_status_t;

typedef struct {
    int index;
    bt_hf_call_direction_t dir;
    bt_hf_call_state_t state;
    bt_hf_call_mode_t mode;
    bt_hf_call_mpty_type_t mpty;
    bt_hf_call_number_type_t number_type;
    const char *number;
} bt_ag_clcc_status_t;

typedef struct {
    int num_active;
    int num_held;
    bt_hf_call_state_t call_state;
    bt_hf_call_number_type_t number_type;
    const char *number;
    const char *caller_name;
} bt_ag_phone_status_t;

typedef struct {
    const char *number;
} bt_hf_dial_call_param_t;

typedef struct {
    int location;
} bt_hf_dial_memory_param_t;

typedef struct {
    bt_hf_call_action_t action;
    int idx;
} bt_hf_call_action_req_param_t;

typedef struct {
    bool wbs_enable;
    bt_hf_audio_codec_t codec;
} bt_hf_wbs_state_param_t;

typedef struct {
    bool service;
    bool roam;
    bool signal;
    bool battery;
} bt_hf_bia_ind_param_t;

typedef struct {
    bt_hf_ind_type_t ind_type;
    int ind_value;
} bt_hf_biev_ind_param_t;

typedef struct {
    char dtmf;
} bt_hf_dtmf_req_param_t;

typedef struct {
    bool start_voice_recognition;
} bt_hf_at_bvra_req_param_t;

typedef struct {
    bool enable_nrec; // noise reduction/echo cancellation
} bt_hf_at_nrec_req_param_t;

typedef struct { // bluetooth hf indicators
    const char *at_string;
} bt_hf_at_bind_req_param_t;

typedef struct {
    const char *unknown_at_cmd;
} bt_hf_unknown_at_req_param_t;

typedef union {
    bt_hf_opened_param_t *opened;
    bt_hf_closed_param_t *closed;
    bt_hf_audio_opened_param_t *audio_opened;
    bt_hf_audio_closed_param_t *audio_closed;
    bt_hf_volume_control_param_t *volume_control;
    bt_hf_wbs_state_param_t *wbs_state;
    bt_hf_bia_ind_param_t *bia_ind;
    bt_hf_biev_ind_param_t *biev_ind;
    bt_hf_dial_call_param_t *dial_call_req;
    bt_hf_dial_memory_param_t *dial_memory_req;
    bt_hf_call_action_req_param_t *call_action_req;
    bt_hf_dtmf_req_param_t *dtmf_req;
    bt_hf_at_bvra_req_param_t *bvra_req;
    bt_hf_at_nrec_req_param_t *nrec_req;
    bt_hf_at_bind_req_param_t *bind_req;
    bt_hf_unknown_at_req_param_t *unknown_at_req;
    bt_hf_volume_change_param_t *volume_change;
} bt_ag_callback_param_t;

struct btif_ag_call_info
{
    uint8_t direction; // 0 outgoing, 1 incoming
    uint8_t state; // 0 active, 1 held, 2 outgoing dialing, 3 outgoing alerting, 4 incoming, 5 waiting, 6 held by Response and Hold
    uint8_t mode; // 0 voice, 1 data, 2 fax
    uint8_t multiparty; // 0 is not one of multiparty call parties, 1 is one of.
    const char* number; // calling number, optional
};

typedef struct {
    uint8_t service;
    uint8_t roam;
    uint8_t signal;
    uint8_t battchg;
    uint8_t call;
    uint8_t callsetup;
    uint8_t callheld;
} bt_ag_ind_status_t;

typedef int (*btif_ag_handler)(btif_hf_channel_t *chan);
typedef int (*btif_ag_handler_int)(btif_hf_channel_t *chan, int n);
typedef int (*btif_ag_handler_str)(btif_hf_channel_t *chan, const char* s);
typedef int (*btif_ag_iterate_call_handler)(btif_hf_channel_t *chan, struct btif_ag_call_info* out);
typedef const char* (*btif_ag_query_operator_handler)(btif_hf_channel_t *chan);

struct btif_ag_module_handler
{
    btif_ag_handler answer_call;
    btif_ag_handler hungup_call;
    btif_ag_handler dialing_last_number;
    btif_ag_handler release_held_calls;
    btif_ag_handler release_active_and_accept_calls;
    btif_ag_handler hold_active_and_accept_calls;
    btif_ag_handler add_held_call_to_conversation;
    btif_ag_handler connect_remote_two_calls;
    btif_ag_handler disable_mobile_nrec;
    btif_ag_handler_int release_specified_active_call;
    btif_ag_handler_int hold_all_calls_except_specified_one;
    btif_ag_handler_int hf_battery_change; /* battery level 0 ~ 100 */
    btif_ag_handler_int hf_spk_gain_change; /* speaker gain 0 ~ 15 */
    btif_ag_handler_int hf_mic_gain_change; /* mic gain 0 ~ 15 */
    btif_ag_handler_int transmit_dtmf_code;
    btif_ag_handler_int memory_dialing_call;
    btif_ag_handler_str dialing_call;
    btif_ag_handler_str handle_at_command;
    btif_ag_query_operator_handler query_current_operator;
    btif_ag_iterate_call_handler iterate_current_call;
    btif_ag_handler cind_req;
    btif_ag_handler clcc_req;
    btif_ag_handler cops_req;
    btif_ag_handler_int hf_enhance_safety_change;
};

uint32_t btif_ag_get_hf_features(btif_hf_channel_t *chan_h);
bool btif_ag_cmee_enabled(btif_hf_channel_t *chan_h);
bt_ag_ind_status_t btif_ag_get_ind_status(btif_hf_channel_t *chan_h);
void btif_ag_set_ind_status(btif_hf_channel_t *chan_h, const bt_ag_ind_status_t *status);
bt_status_t btif_ag_set_curr_at_upper_handle(btif_hf_channel_t* chan_h);
bt_status_t btif_ag_create_service_link(bt_bdaddr_t * bt_addr);
bt_status_t btif_ag_disconnect_service_link(btif_hf_channel_t* chan_h);
bt_status_t btif_ag_create_audio_link(btif_hf_channel_t* chan_h);
bt_status_t btif_ag_disc_audio_link(btif_hf_channel_t* chan_h);
bt_status_t btif_ag_send_service_status(btif_hf_channel_t* chan_h, bool enabled);
bt_status_t btif_ag_send_call_active_status(btif_hf_channel_t* chan_h, bool active);
bt_status_t btif_ag_send_callsetup_status(btif_hf_channel_t* chan_h, uint8_t status);
bt_status_t btif_ag_send_callheld_status(btif_hf_channel_t* chan_h, uint8_t status);
bt_status_t btif_ag_send_mobile_signal_level(btif_hf_channel_t* chan_h, uint8_t level);
bt_status_t btif_ag_send_mobile_roam_status(btif_hf_channel_t* chan_h, bool enabled);
bt_status_t btif_ag_send_mobile_battery_level(btif_hf_channel_t* chan_h, uint8_t level);
bt_status_t btif_ag_send_calling_ring(btif_hf_channel_t* chan_h, const char* number);
bt_status_t btif_ag_set_speaker_gain(btif_hf_channel_t* chan_h, uint8_t volume);
bt_status_t btif_ag_set_inband_ring_tone(btif_hf_channel_t* chan_h, bool enabled);
bt_status_t btif_ag_set_last_dial_number(btif_hf_channel_t* chan_h, bool enabled);
bt_status_t btif_ag_set_microphone_gain(btif_hf_channel_t* chan_h, uint8_t volume);
bt_status_t btif_ag_send_call_waiting_notification(btif_hf_channel_t* chan_h, const char* number);
bt_status_t btif_ag_send_result_code(btif_hf_channel_t* chan_h, const char *data, int len);
bt_status_t btif_ag_register_module_handler(btif_hf_channel_t* chan_h, struct btif_ag_module_handler* handler);
#endif /* BT_HFP_AG_ROLE */

typedef void (*hfp_aud_discon_report_cb)(uint8_t *value, uint8_t len);

void btif_hfp_user_event_callback_register(hf_event_cb_t cb);

void btif_hfp_user_event_callback_deregister(void);

hf_event_cb_t btif_hfp_get_user_event_callback(void);

void btif_hfp_aud_discon_report_callback_register(hfp_aud_discon_report_cb cb);

void btif_hfp_aud_discon_report_callback_deregister(void);

void btif_hfp_report_user_audio_play_stop_status(void);

bool btif_hfp_remote_support_codec_negotiation(btif_hf_channel_t* chan_h);

/**
 ****************************************************************************************
 *    _   _ _____ ____    ___ ____  ____ _____   _____ _   _ _   _  ____
 *   | | | |  ___|  _ \  |_ _| __ )|  _ \_   _| |  ___| | | | \ | |/ ___|
 *   | |_| | |_  | |_) |  | ||  _ \| |_) || |   | |_  | | | |  \| | |
 *   |  _  |  _| |  __/   | || |_) |  _ < | |   |  _| | |_| | |\  | |___
 *   |_| |_|_|   |_|     |___|____/|_| \_\|_|   |_|    \___/|_| \_|\____|
 ****************************************************************************************
 */
uint32_t btif_hfp_profile_save_ctx(const bt_bdaddr_t *remote, uint8_t *buf, uint32_t buf_len);

uint32_t btif_hfp_profile_restore_ctx(const bt_bdaddr_t *bdaddr_p, uint8_t *buf, uint32_t buf_len);

bt_status_t btif_hf_sync_conn_audio_connected(hfp_sco_codec_t codec,uint16_t conhdl);

bt_status_t btif_hf_sync_conn_audio_disconnected(uint16_t conhdl);

bt_status_t btif_hfp_force_disconnect_hfp_profile(uint8_t device_id,uint8_t reason);

void btif_hfp_ibrt_role_switch_handle(const bt_bdaddr_t *remote);

#ifdef __cplusplus
}
#endif

#endif /*__HFP_API_H__*/
