/**
 * @brief Bluetooth Service Low Energy Audio Unicast Client Application Programming Interface
 *
 * @copyright Copyright (c) 2015-20223 BES Technic.
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
 */
#ifndef __BT_SVC_LEA_UC_CLIENT_API_H__
#define __BT_SVC_LEA_UC_CLIENT_API_H__

#ifdef AOB_MOBILE_ENABLED
/*****************************header include********************************/
#include "nvrecord_extension.h"
#include "bes_aob_api.h"

/******************************macro defination*****************************/

/******************************type defination******************************/
typedef enum{
    DISCOVER_START    = 0,
    DISCOVER_COMPLEPE = 1,

    DISCOVER_MAX      = 3,
}DISCOVER_OPERATE;

typedef enum{
    SERVICE_ACC_MCC = 0,
    SERVICE_ACC_TBC = 1,

    SERVICE_MAX     = 2,
}DISCOVER_SERVICE;

typedef enum
{
    REQ_BLE_RECONNECT,
    REQ_BLE_DISCONNECT,

    EVT_BLE_RECONNECTING_CANCELLED,
    EVT_BLE_CONNECTED,
    EVT_BLE_RECONNECT_BOND_FAIL,
    EVT_BLE_RECONNECT_FAILED,
    EVT_BLE_RECONNECT_TIMEOUT,
    EVT_BLE_DISCONNECTED,
}BLE_MOBILE_CONNECT_EVENT;

typedef struct
{
    bool                    connected;
    uint8_t                 conidx;
    BLE_ADDR_INFO_T         peer_ble_addr;
} AOB_MOBILE_PHONE_INFO_T;

typedef struct
{
    // true: slave address will be set as dongle role
    // false: device role is determined by address of g_tws_pairing_info
    uint8_t lea_central_pairing_enable : 1;
    // true: audio output switch to PC if there is no link exist 
    // false: Even without a link, the audio doesn't switch to the PC
    uint8_t audio_routing_on_link : 1;
    uint8_t rfu : 6;
    //mobile_reconnect_timeout
    uint32_t mobile_reconnect_timeout;
}ble_audio_mobile_core_config;

/****************************function declearation**************************/
void bt_svc_lea_uc_client_info_init(void);
void bt_svc_lea_uc_client_single_call_info_clear(uint8_t conidx, uint8_t call_id);
bool bt_svc_lea_uc_client_info_get(uint8_t conidx, AOB_MOBILE_PHONE_INFO_T *info);
uint8_t bt_svc_lea_uc_client_get_active_conidx(void);
AOB_SINGLE_CALL_INFO_T *bt_svc_lea_uc_client_info_get_call_info_by_id(uint8_t conidx, uint8_t call_id);
void bt_svc_lea_uc_client_info_clear_call_info(uint8_t conidx, uint8_t call_id);

AOB_SINGLE_CALL_INFO_T *bt_svc_lea_uc_client_get_call_info_by_state(uint8_t conidx, AOB_CALL_STATE_E state);
AOB_SINGLE_CALL_INFO_T *bt_svc_lea_uc_client_get_call_info_by_bearer_lid(uint8_t conidx, uint8_t bearer_lid);
void bt_svc_lea_uc_client_info_bond_bearer_id(uint8_t conidx, uint8_t bearer_lid);
void bt_svc_lea_uc_client_info_update_call_id(uint8_t conidx, uint8_t call_id);
bool bt_svc_lea_uc_client_both_is_connected(void);
bool bt_svc_lea_uc_client_is_any_device_connected(void);
bool bt_svc_lea_uc_client_check_device_connected(uint8_t *p_addr);

void bt_svc_lea_uc_client_register_event_cb(const BLE_AUD_MOB_CORE_EVT_CB_T *cb);

const BLE_AUD_MOB_CORE_EVT_CB_T* bt_svc_lea_uc_client_get_evt_cb();

int bt_svc_lea_uc_client_start_connecting(ble_bdaddr_t *addr, uint32_t timeout_ms);

void bt_svc_lea_uc_client_disconnecting(ble_bdaddr_t *addr);

uint8_t* bt_svc_lea_uc_client_conn_get_connecting_dev(void);

void bt_svc_lea_uc_client_connect_failed(bool is_failed);

bool bt_svc_lea_uc_client_is_connect_failed(void);

bool bt_svc_lea_uc_client_conn_next_paired_dev(ble_bdaddr_t *bdaddr);

void bt_svc_lea_uc_client_start_connect(void);

uint8_t bt_svc_lea_uc_client_discovery_modify_interval(uint8_t con_lid, DISCOVER_OPERATE operate, DISCOVER_SERVICE service);

void bt_svc_lea_uc_client_core_register_event_cb(const BLE_AUD_MOB_CORE_EVT_CB_T *cb);

ble_audio_mobile_core_config* bt_svc_lea_uc_client_get_core_config();
#endif

#define BTS_LEA_UC_CLI_CIG_MAX 2
#define BTS_LEA_UC_CLI_CIS_MAX 2

typedef enum
{
    BTS_LEA_UC_CLI_OPERATE_STATE_BIT = 0,
    BTS_LEA_UC_CLI_OPERATE_FORCE_BIT = 7,
} BT_SVC_LEA_UC_OPERATE_BIT_E;

typedef enum
{
    // One audio channel plays synchronously locally and remotely
    BT_SVC_LEA_UC_CLI_PLAY_MONO=0,
    // Two audio channels, one channel plays locally and the other
    // channel plays synchronously on remote devices
    BT_SVC_LEA_UC_CLI_PLAY_ONE_CIS,
    // Two audio channels are placed in one stream and played
    // synchronously on both local and remote devices
    BT_SVC_LEA_UC_CLI_PLAY_TWO_CIS,
    // 
    BT_SVC_LEA_UC_CLI_PLAY_UNKONW,
} BT_SVC_LEA_UC_CLI_PLAY_MODE_E;

typedef enum
{
    BTS_LEA_UC_CLI_EVENT_SCAN_REPORT = 0,
    BTS_LEA_UC_CLI_EVENT_ACL_CONN,
    BTS_LEA_UC_CLI_EVENT_ACL_DISCONN,
    BTS_LEA_UC_CLI_EVENT_STREAM_START,
    BTS_LEA_UC_CLI_EVENT_STREAM_STOP,
} BT_SVC_LEA_UC_CLI_EVENT_E;

typedef struct
{
    uint8_t addr[6];
    uint8_t force;
    uint8_t source:1;
    uint8_t sink:1;
    uint8_t cig_id;
} bts_lea_uc_cli_stream_start_t;

typedef struct
{
    uint8_t*    addr;
    uint8_t     addr_type;
    char*       name;
    int8_t      rssi;
    // Private data length, range:0-255
    uint8_t     data_len;
    // Private data
    const uint8_t* data;
} bts_lea_uc_cli_event_scan_t;

typedef struct
{
    // see@BT_SVC_ERROR_CODE_E
    int status;
    // connect index
    uint8_t con_idx;
    // address
    uint8_t addr[6];
} bts_lea_uc_cli_event_acl_t;

typedef struct
{
    // connect index
    uint8_t con_idx;
    // ase direction
    uint8_t direction;
    // Event status,see@BT_SVC_ERROR_CODE_E
    int     status;
} bts_lea_uc_cli_event_stream_t;

typedef struct
{
    // event type, see@BT_SVC_LEA_UC_CLI_EVENT_E
    uint8_t event_type;
    union
    {
        bts_lea_uc_cli_event_scan_t scan_report;
        bts_lea_uc_cli_event_acl_t  conn;
        bts_lea_uc_cli_event_acl_t  disconn;
        bts_lea_uc_cli_event_stream_t stream_start;
        bts_lea_uc_cli_event_stream_t stream_stop;
    } param;
} bt_svc_lea_uc_cli_event_t;

typedef struct
{
    uint32_t sample_rate;
    uint32_t frame_size;
    uint8_t  ch_num;
    uint8_t  bits_byte;
} bt_svc_lea_uc_cli_audio_param_t;

typedef struct
{
    uint8_t  tx_rx_integrated;
    bt_svc_lea_uc_cli_audio_param_t playback;
    bt_svc_lea_uc_cli_audio_param_t capture;
} bt_svc_lea_uc_cli_input_param_t;

typedef struct
{
    uint8_t  codec_id[5];
    uint8_t  cig_count;
    struct
    {
        uint16_t sdu_interval_us;
        uint16_t iso_interval_us;
        uint8_t  ft;
        uint8_t  cis_count;
        struct
        {
            uint8_t  nse;
            uint8_t  phy;
            uint8_t  bn;
            uint16_t pdu_max_tx;
            uint16_t pdu_max_rx;
            uint16_t hdt_rates_bf;
            uint16_t hdt_mic_length;
            uint8_t hdt_pkt_fmt;
            uint8_t hdt_pld_window_size;
        } cis_param[BTS_LEA_UC_CLI_CIS_MAX];
    } cig_param[BTS_LEA_UC_CLI_CIG_MAX];
} bt_svc_lea_uc_cli_output_param_t;

typedef struct
{
    // see@BT_SVC_AUDIO_INPUT_TYPE_E
    uint8_t  input_type;
    uint8_t  play_mode;
    uint8_t  auto_pair;

    bt_svc_lea_uc_cli_input_param_t  input_param;
    bt_svc_lea_uc_cli_output_param_t output_param;

    void (*event_callback)(bt_svc_lea_uc_cli_event_t* param);
} bt_svc_lea_uc_cli_param_t;

int bts_lea_uc_cli_open(bt_svc_lea_uc_cli_param_t* param);

int bts_lea_uc_cli_close();

int bts_lea_uc_cli_scan_start(void);

int bts_lea_uc_cli_scan_stop(void);

int bts_lea_uc_cli_conn(uint8_t *addr, uint8_t addr_type, uint32_t timeout_ms);

int bts_lea_uc_cli_disconn(uint8_t *ble_addr);

int bts_lea_uc_cli_stream(bool operate, bts_lea_uc_cli_stream_start_t* param);

uint8_t* bt_svc_lea_uc_client_connecting_dev(void);

#endif //__BT_SVC_LEA_API_H__
