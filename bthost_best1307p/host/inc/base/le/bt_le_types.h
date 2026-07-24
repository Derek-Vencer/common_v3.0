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

/**
 ****************************************************************************************
 *    ____  _     _____   _____
 *   | __ )| |   | ____| |_   _|   _ _ __   ___  ___
 *   |  _ \| |   |  _|     | || | | | '_ \ / _ \/ __|
 *   | |_) | |___| |___    | || |_| | |_) |  __/\__ \
 *   |____/|_____|_____|   |_| \__, | .__/ \___||___/
 *                             |___/|_|
 *
 ****************************************************************************************
 */

#define BT_BLE_GAP_ADDR_LEN                                (6)
#define BT_BLE_GAP_KEY_LEN                                 (16)

#define BT_BLE_SMP_AUTH_NON_BONDING                        (0x00)
#define BT_BLE_SMP_AUTH_BONDING                            (0x01)
#define BT_BLE_SMP_AUTH_MITM_PROTECT                       (0x04)
#define BT_BLE_SMP_AUTH_SC_SUPPORT                         (0x08) // LE Secure Connection Pairing support flag
#define BT_BLE_SMP_AUTH_KEYPRESS_NOTIFY                    (0x10) // PairingKeypressNotification shall be generated if both side set support
#define BT_BLE_SMP_AUTH_CT2_SUPPORT                        (0x20) // support for the h7 function or not for some reason

typedef enum
{
    BT_BLE_GAP_CODED_PHY_NO_PREFER_CODING = 0x00,
    BT_BLE_GAP_CODED_PHY_PREFER_S2_CODING = 0x01,
    BT_BLE_GAP_CODED_PHY_PREFER_S8_CODING = 0x02,
    BT_BLE_GAP_CODED_PHY_REQUIR_S2_CODING = 0x03,
    BT_BLE_GAP_CODED_PHY_REQUIR_S8_CODING = 0x04,
} bt_ble_gap_coded_phy_prefer_t;

typedef enum
{
    BT_BLE_GAP_ADV_ACTIVITY_USER_0 = 0,
    BT_BLE_GAP_ADV_ACTIVITY_USER_1,
    BT_BLE_GAP_ADV_ACTIVITY_USER_2,
    BT_BLE_GAP_ADV_ACTIVITY_USER_3,

    BT_BLE_GAP_ADV_ACTIVITY_USER_NUM,
} bt_ble_gap_adv_activity_t;

typedef enum
{
    /// Connectable and scannable undirected advertising
    BT_GAP_ADV_TYPE_UNDIRECT = 0,
    /// Directed connectable with Low Duty Cycle
    BT_GAP_ADV_TYPE_DIRECT_LDC,
    /// Directed connectable with High Duty Cycle
    BT_GAP_ADV_TYPE_DIRECT_HDC,
    /// Non-connectable but Scannable undirected advertising
    BT_GAP_ADV_TYPE_NON_CONN_SCAN,
    /// Non-connectable and nonscannable undirected
    BT_GAP_ADV_TYPE_NON_CONN_NON_SCAN,
    /// Undirected connectable but scannable with extended ADV
    BT_GAP_ADV_TYPE_CONN_EXT_ADV,
    /// Directed connectable no scannable with extended ADV
    BT_GAP_ADV_TYPE_EXT_CON_DIRECT,
    /// Type MAx Number
    BT_GAP_ADV_TYPE_MAX,
} bt_ble_gap_adv_type_t;

typedef enum
{
    /// Legacy advertising
    BT_GAP_ADV_MODE_LEGACY = 0,
    /// Extended advertising
    BT_GAP_ADV_MODE_EXTENDED,
    /// Periodic advertising
    BT_GAP_ADV_MODE_PERIODIC,
} bt_ble_gap_adv_mode_t;

typedef enum
{
    BT_BLE_GAP_ADV_PUBLIC_STATIC = 0,
    BT_BLE_GAP_ADV_PRIVATE_STATIC = 1,
    BT_BLE_GAP_ADV_RPA = 2
} bt_ble_gap_adv_addr_type_t;

typedef enum
{
    BT_BLE_LINK_CONNECTED_EVENT               = 0,
    BT_BLE_CONNECT_BOND_EVENT                 = 1,    //pairing success
    BT_BLE_CONNECT_BOND_FAIL_EVENT            = 2,    //pairing failed
    BT_BLE_CONNECT_NC_EXCH_EVENT              = 3,    //Numeric Comparison - Exchange of Numeric Value
    BT_BLE_CONNECT_ENCRYPT_EVENT              = 4,    //encrypt complete
    BT_BLE_CONNECTING_STOPPED_EVENT           = 5,
    BT_BLE_CONNECTING_FAILED_EVENT            = 6,
    BT_BLE_DISCONNECT_EVENT                   = 7,
    BT_BLE_CONN_PARAM_UPDATE_REQ_EVENT        = 8,
    BT_BLE_CONN_PARAM_UPDATE_FAILED_EVENT     = 9,
    BT_BLE_CONN_PARAM_UPDATE_SUCCESSFUL_EVENT = 10,
    BT_BLE_SET_RANDOM_BD_ADDR_EVENT           = 11,
    BT_BLE_ADV_STARTED_EVENT                  = 12,
    BT_BLE_ADV_STARTING_FAILED_EVENT          = 13,
    BT_BLE_ADV_STOPPED_EVENT                  = 14,
    BT_BLE_SCAN_STARTED_EVENT                 = 15,
    BT_BLE_SCAN_DATA_REPORT_EVENT             = 16,
    BT_BLE_SCAN_STARTING_FAILED_EVENT         = 17,
    BT_BLE_SCAN_STOPPED_EVENT                 = 18,
    BT_BLE_CREDIT_BASED_CONN_REQ_EVENT        = 19,
    BT_BLE_RPA_ADDR_PARSED_EVENT              = 20,
    BT_BLE_GET_TX_PWR_LEVEL                   = 21,
    BT_BLE_TX_PWR_REPORT_EVENT                = 22,
    BT_BLE_PATH_LOSS_REPORT_EVENT             = 23,
    BT_BLE_SET_RAL_CMP_EVENT                  = 24,
    BT_BLE_SUBRATE_CHANGE_EVENT               = 25,
    BT_BLE_ENCRYPT_LTK_REPORT_EVENT           = 26,
    BT_BLE_MTU_EXECHANGE_EVENT                = 27,
    BT_BLE_SMP_PAIRING_CMP_EVENT              = 28,
    BT_BLE_SMP_PAIRING_REQ_EVENT              = 29,
    BT_BLE_SMP_PAIRING_RSP_EVENT              = 30,
    BT_BLE_PASSKEY_DISPLAY_EVENT              = 31,
    BT_BLE_PASSKEY_REQUEST_EVENT              = 32,
    BT_BLE_CONN_SECURITY_UPDATE_EVENT         = 33,
    BT_BLE_PHY_UPDATE_COMPLETE_EVENT          = 34,
    BT_BLE_DATA_LEN_UPDATE_COMPLETE_EVENT     = 35,
    BT_BLE_MTU_EXCHANGE_REQ_EVENT             = 36,
    BT_BLE_SMP_CTKD_KEY_DERIVED_EVENT         = 37,
    BT_BLE_GATT_SVC_CHANGED_EVENT             = 38,

    BT_BLE_EVENT_NUM_MAX,
} bt_ble_evt_type_t;

typedef enum
{
    BT_BD_ADDR_TYPE_PUBLIC = 0x00,
    BT_BD_ADDR_TYPE_RANDOM = 0x01,
    BT_BD_ADDR_TYPE_PUB_IA = 0x02,
    BT_BD_ADDR_TYPE_RND_IA = 0x03,
} bt_bdaddr_type_t;

typedef enum
{
    BT_BLE_GAP_ADV_USER_INUSE = 0,
    BT_BLE_GAP_ADV_USER_ALL = BT_BLE_GAP_ADV_USER_INUSE,
    BT_BLE_GAP_ADV_USER_STUB,
    BT_BLE_GAP_ADV_USER_GFPS,
    BT_BLE_GAP_ADV_USER_SWIFT,
    BT_BLE_GAP_ADV_USER_GSOUND,
    BT_BLE_GAP_ADV_USER_AI,
    BT_BLE_GAP_ADV_USER_INTERCONNECTION,
    BT_BLE_GAP_ADV_USER_TILE,
    BT_BLE_GAP_ADV_USER_OTA,
    BT_BLE_GAP_ADV_USER_BLE_AUDIO,
    BT_BLE_GAP_ADV_USER_SPOT,
    BT_BLE_GAP_ADV_USER_FINDMY,

    BT_BLE_GAP_ADV_USER_CONST_MAX = 11,

    BT_BLE_GAP_ADV_USER_BLE_CUSTOMER_0,
    BT_BLE_GAP_ADV_USER_BLE_CUSTOMER_1,
    BT_BLE_GAP_ADV_USER_BLE_CUSTOMER_2,
    BT_BLE_GAP_ADV_USER_BLE_CUSTOMER_3,
    BT_BLE_GAP_ADV_USER_BLE_DEMO0,
    BT_BLE_GAP_ADV_USER_BLE_DEMO1,
    BT_BLE_GAP_ADV_USER_NUM,
} bt_ble_gap_adv_user_t;

typedef enum
{
    BT_BLE_ADV_SWITCH_USER_RS          = 0, // used for role switch
    BT_BLE_ADV_SWITCH_USER_BOX         = 1, // used for box open/close
    BT_BLE_ADV_SWITCH_USER_AI          = 2, // used for ai
    BT_BLE_ADV_SWITCH_USER_BT_CONNECT  = 3, // used for bt connect
    BT_BLE_ADV_SWITCH_USER_SCO         = 4, // used for sco
    BT_BLE_ADV_SWITCH_USER_IBRT        = 5, // used for ibrt
    BT_BLE_ADV_SWITCH_USER_FPGA        = 6, // used for fpga
    BT_BLE_ADV_SWITCH_USER_BLE_AUDIO   = 7, // used for ble audio
    BT_BLE_ADV_SWITCH_USER_CUSTOM      = 8, // used for custom

    BT_BLE_ADV_SWITCH_USER_NUM,
} bt_ble_adv_switch_user_t;

typedef enum
{
    BT_BLE_SMP_NC_CMP_CONFIRM = 0x00,
    BT_BLE_SMP_PASSKEY_ENTRY,
    BT_BLE_SMP_OOB_TERM_KEY,
    BT_BLE_SMP_OOB_DATA_LOCAL,
    BT_BLE_SMP_OOB_DATA_PEER,
    BT_BLE_SMP_LONG_TERM_KEY,

    BT_BLE_SMP_INPUT_TYPE_MAX,
} bt_ble_smp_input_type_e;

typedef struct
{
    uint8_t conidx;
    uint8_t role; //0 = Central / 1 = Peripheral
    uint16_t connhdl;
    uint16_t con_interval;
    uint16_t con_latency;
    uint16_t sup_to;
    ble_bdaddr_t peer_bdaddr;
} bt_connect_handled_t;

typedef struct
{
    uint8_t conidx;
    bool success;
    uint16_t reason;
} bt_connect_bond_handled_t;

typedef struct
{
    uint8_t conidx;
    uint16_t connhdl;
    uint32_t confirm_value; // 6 digits
    /// Peer device address
    ble_bdaddr_t peer_addr;
} bt_connect_nc_exch_handled_t;

typedef struct
{
    uint8_t conidx;
    uint8_t pairing_lvl;
    uint8_t addr_type;
    uint8_t addr[BT_BLE_GAP_ADDR_LEN];
} bt_connect_encrypt_handled_t;

typedef struct
{
    uint8_t peer_bdaddr[BT_BLE_GAP_ADDR_LEN];
    uint8_t peer_type;
    uint8_t err_code;
} bt_stopped_connecting_handled_t;

typedef struct
{
    uint8_t actv_idx;
    uint8_t err_code;
    ble_bdaddr_t peer_bdaddr;
} bt_connecting_failed_handled_t;

typedef struct
{
    uint8_t conidx;
    uint8_t errCode;
    uint16_t connhdl;
    ble_bdaddr_t peer_bdaddr;
} bt_disconnect_handled_t;

typedef struct
{
    uint8_t conidx;
    /// Connection interval minimum
    uint16_t intv_min;
    /// Connection interval maximum
    uint16_t intv_max;
    /// Latency
    uint16_t latency;
    /// Supervision timeout
    uint16_t time_out;
} bt_conn_param_update_req_handled_t;

typedef struct
{
    uint8_t conidx;
    uint8_t err_code;
} bt_conn_param_update_failed_handled_t;

typedef struct
{
    uint8_t conidx;
    ///Connection interval value
    uint16_t con_interval;
    ///Connection latency value
    uint16_t con_latency;
    ///Supervision timeout
    uint16_t sup_to;
} bt_conn_param_update_successful_handled_t;

typedef struct
{
    uint8_t *new_bdaddr;
} bt_set_random_bd_addr_handled_t;

typedef struct
{
    uint8_t actv_user;
} bt_adv_started_handled_t;

typedef struct
{
    uint8_t actv_user;
    uint8_t err_code;
} bt_adv_starting_failed_handled_t;

typedef struct
{
    uint8_t actv_user;
} bt_adv_stopped_handled_t;

typedef struct
{
    uint16_t extended_adv: 1;
    uint16_t legacy_pdu: 1;
    uint16_t connectable: 1;
    uint16_t scannable: 1;
    uint16_t directed: 1;
    uint16_t scan_rsp: 1;
    uint16_t cmpl_adv_data: 1;
    uint16_t incmpl_adv_data: 1;
    uint16_t has_more_data: 1;
    uint16_t peer_anonymous: 1;
    uint16_t direct_unresolv_addr: 1;
    uint16_t primary_phy: 2;
    uint16_t secondary_phy: 2;
} bt_gap_adv_flags_t;

typedef struct
{
    bt_gap_adv_flags_t adv;
    bt_bdaddr_t peer_addr;
    bt_bdaddr_type_t peer_type;
    bt_bdaddr_type_t direct_addr_type;
    bt_bdaddr_t direct_addr; // TargetA or Public IA or Random IA
    const uint8_t *data;
    uint8_t data_length;
    uint8_t adv_set_id;
    uint16_t sync_handle;
    uint16_t pa_interval;
    int8_t tx_power;
    int8_t rssi;
    uint16_t pa_event_counter;
    uint8_t subevent;
    uint8_t response_slot; // 0x00 to 0xFF, the response slot the data was received in
    bool aux_sync_subevent_ind_not_transmitted;
    bool fail_rx_aux_sync_subevent_rsp;
    bool fail_rx_aux_sync_subevent_ind;
    uint8_t cte_type; // 0x00 to 0x02, 0xFF no cte
    bt_ble_gap_coded_phy_prefer_t primary_coding;
    bt_ble_gap_coded_phy_prefer_t secondary_coding;
} bt_gap_adv_report_t;

typedef struct
{
    /// Transmitter device address
    ble_bdaddr_t trans_addr;
    /// RSSI (between -127 and +20 dBm)
    int8_t rssi;
    /// Bit field providing information about the received report (@see enum gapm_adv_report_info)
    uint8_t info;
    /// Report data length
    uint16_t length;
    /// Report data
    const uint8_t *data;
    /// Raw adv info
    const bt_gap_adv_report_t *adv;
} bt_scan_data_report_handled_t;

typedef struct
{
    uint8_t actv_idx;
    uint8_t err_code;
} bt_scan_starting_failed_handled_t;

typedef struct
{
    uint8_t  conidx;
    bool     isEnhanced;    // true: eatt 0x17; false: le 0x14
    uint16_t spsm;
    uint16_t mtu;
    uint16_t mps;
    uint16_t initial_credits;
} bt_credit_based_conn_req_handled_t;

typedef struct
{
    /// Connection index
    uint8_t conidx;
    /// Random address
    bt_bdaddr_t random_addr;
    /// Resolved public address
    bt_bdaddr_t resolved_addr;
} bt_random_addr_resolved_handled_t;

typedef struct
{
    /// Connection index
    uint8_t conidx;
    //object (see enum #ble_tx_object_e)
    uint8_t object;
    /// PHY (see enum #ble_phy_pwr_value_e)
    uint8_t phy;
    /// Current transmit power level (dBm)
    int8_t tx_pwr;
    /// object is BLE_TX_LOCAL, data is max transmit power level (dBm)
    /// object is BLE_TX_REMOTE, data is flags,
    ///                          bit[0] minimum power level
    ///                          bit[1] maximum power level
    int8_t data;
} bt_read_tx_pwr_handled_t;

typedef struct
{
    /// Connection index
    uint8_t conidx;
    //object (see enum #ble_tx_target_e)
    uint8_t object;
    /// PHY (see enum #ble_phy_pwr_value_e)
    uint8_t phy;
    /// bit[0] - Sender is at the minimum supported power level
    /// bit[1] - Sender is at the maximum supported power level
    uint8_t flags;
    /// Transmit Power level (dBm)
    int8_t tx_pwr;
    /// Delta (dB)
    int8_t delta;
} bt_tx_pwr_change_reporting_handled_t;

typedef struct
{
    /// Connection index
    uint8_t conidx;
    //object (see enum #ble_tx_target_e)
    uint8_t curr_path_loss;
    /// curr_path_loss Units:dB
    uint8_t zone_entered;
    //0x00 low zone,0x01 middle zone,0x02 high zone
} bt_path_loss_report_handled_t;

typedef struct
{
    uint8_t status;
} bt_path_set_ral_cmp_handled_t;

typedef struct
{
    /// Connection index
    uint8_t conidx;
    /// status
    uint8_t status;
    /// Subrate Factor
    uint16_t sub_factor;
    /// Peripheral_Latency
    uint16_t per_latency;
    /// Continuation Number
    uint16_t cont_num;
    /// Supervision timeout (timeout * 10ms)
    uint16_t timeout;
} bt_subrate_change_handled_t;

typedef struct
{
    /// Connection index
    uint8_t conidx;
    /// LTK found flag
    uint8_t ltk_existed;
} bt_le_conn_encrypt_ltk_handled_t;

typedef struct
{
    /// Connection index
    uint8_t conidx;
    /// mtu
    uint16_t mtu;
} bt_mtu_exechange_handled_t;

typedef struct
{
    ble_bdaddr_t identity_address;
    uint8_t irk[16];
    uint8_t ediv[2];
    uint8_t rand[8];
    uint8_t ltk[16];
    uint8_t ltk_len;
    uint8_t signing_key[16];
} bt_security_info_t;

typedef struct
{
    uint8_t conidx;
    uint8_t err_code;
    bt_security_info_t initiator;
    bt_security_info_t responde;
} bt_pair_complete_handled_t;

typedef struct
{
    uint8_t io_cap;
    uint8_t has_oob_data;
    uint8_t auth_req;
    uint8_t max_enc_key_size;
    uint8_t init_key_dist;
    uint8_t resp_key_dist;
} bt_pairing_reqiure_t;

typedef struct
{
    /// Connection index
    uint8_t conidx;
    /// Connection handle
    uint16_t connhdl;
    /// Peer device address
    ble_bdaddr_t peer_addr;
    /// Pairing response features
    bt_pairing_reqiure_t feature;
} bt_pair_request_handled_t;

typedef struct
{
    /// Connection index
    uint8_t conidx;
    /// Connection handle
    uint16_t connhdl;
    /// Peer device address
    ble_bdaddr_t peer_addr;
    /// Pairing request features
    bt_pairing_reqiure_t feature;
} bt_pair_response_handled_t;

typedef struct
{
    /// Connection index
    uint8_t conidx;
    /// Connection handle
    uint16_t connhdl;
    /// Peer device address
    ble_bdaddr_t peer_addr;
    /// Pairing passkey displayed when passkey display
    uint32_t passkey_display;
} bt_pair_passkey_handled_t;

typedef struct
{
    /// Connection index
    uint8_t conidx;
    /// Connection handle
    uint16_t connhdl;
    /// mtu
    uint16_t peer_mtu;
} bt_mtu_exechange_req_t;

typedef struct
{
    /// BT linkey or LE LTK
    bool is_linkkey;
    /// Address type from IA
    uint8_t peer_type;
    /// Address from IA
    bt_bdaddr_t peer_addr;
    /// Key size
    uint8_t enc_key_size;
    /// Key
    uint8_t enc_key[16];
} bt_smp_ctkd_key_derived_t;

typedef struct
{
    /// Connection index
    uint8_t conidx;
    /// Connection handle
    uint16_t connhdl;
    uint16_t tx_octets;
    uint16_t rx_octets;
    uint16_t tx_time;
    uint16_t rx_time;
} bt_data_length_updated_t;

typedef struct
{
    /// Connection index
    uint8_t conidx;
    /// Connection handle
    uint16_t connhdl;
    uint8_t status;
    uint8_t tx_phy;
    uint8_t rx_phy;
} bt_phy_update_complete_t;

typedef struct
{
    uint8_t conidx;
    uint16_t connhdl;
} bt_gatt_svc_changed_handled_t;

typedef union
{
    bt_connect_handled_t connect_handled;
    bt_connect_bond_handled_t connect_bond_handled;
    bt_connect_nc_exch_handled_t connect_nc_exch_handled;
    bt_connect_encrypt_handled_t connect_encrypt_handled;
    bt_stopped_connecting_handled_t stopped_connecting_handled;
    bt_connecting_failed_handled_t connecting_failed_handled;
    bt_disconnect_handled_t disconnect_handled;
    bt_conn_param_update_req_handled_t conn_param_update_req_handled;
    bt_conn_param_update_failed_handled_t conn_param_update_failed_handled;
    bt_conn_param_update_successful_handled_t conn_param_update_successful_handled;
    bt_set_random_bd_addr_handled_t set_random_bd_addr_handled;
    bt_adv_started_handled_t adv_started_handled;
    bt_adv_starting_failed_handled_t adv_starting_failed_handled;
    bt_adv_stopped_handled_t adv_stopped_handled;
    bt_scan_data_report_handled_t scan_data_report_handled;
    bt_scan_starting_failed_handled_t scan_starting_failed_handled;
    bt_credit_based_conn_req_handled_t credit_based_conn_req_handled;
    bt_random_addr_resolved_handled_t random_addr_resolved_handled;
    bt_read_tx_pwr_handled_t read_tx_power_handled;
    bt_tx_pwr_change_reporting_handled_t tx_power_change_reporting_handled;
    bt_path_loss_report_handled_t path_loss_report_handled;
    bt_path_set_ral_cmp_handled_t set_ral_cmp_handled;
    bt_subrate_change_handled_t subrate_change_handled;
    bt_le_conn_encrypt_ltk_handled_t le_conn_encrypt_ltk_handled;
    bt_mtu_exechange_handled_t mtu_exec_handled;
    bt_pair_complete_handled_t pairing_cmp_handled;
    bt_pair_request_handled_t smp_pairing_request;
    bt_pair_response_handled_t smp_pairing_response;
    bt_pair_passkey_handled_t smp_passkey_request;
    bt_pair_passkey_handled_t smp_passkey_display;
    bt_mtu_exechange_req_t mtu_exchange_req;
    bt_smp_ctkd_key_derived_t ctkd_key_derived;
    bt_data_length_updated_t data_length_updated;
    bt_phy_update_complete_t conn_phy_updated;
    bt_gatt_svc_changed_handled_t gatt_svc_changed;
} bt_ble_evt_handled_t;

typedef struct
{
    /// Activity identifier for the advertising instance
    bt_ble_gap_adv_activity_t actv_user;
    /// Logical advertising user index or leave it zero by default
    bt_ble_gap_adv_user_t adv_user;
    /// Whether custom advertising flags are used (true: use provided flags)
    bool is_custom_adv_flags;
    /// Advertising filter policy (e.g. allow all, whitelist, etc.)
    uint8_t filter_policy;
    /// Local address type used in advertising (public, random, RPA, etc.)
    bt_ble_gap_adv_addr_type_t own_addr_type;
    /// Pointer to local device address (optional, depends on own_addr_type)
    const uint8_t *local_addr;
    /// Target peer device address (only used for directed advertising)
    const ble_bdaddr_t *peer_addr;
    /// Advertising interval (unit: 0.625 ms)
    uint32_t adv_interval;
    /// Advertising type (connectable, non-connectable, directed, etc.)
    bt_ble_gap_adv_type_t adv_type;
    /// Advertising mode (e.g. legacy, extended, periodic)
    bt_ble_gap_adv_mode_t adv_mode;
    /// Transmit power level for advertising (dBm)
    int8_t tx_power_dbm;
    /// Pointer to advertising data payload
    const uint8_t *adv_data;
    /// Advertising data length (bytes)
    uint8_t adv_data_size;
    /// Pointer to scan response data payload
    const uint8_t *scan_rsp_data;
    /// Scan response data length (bytes)
    uint8_t scan_rsp_data_size;
} bt_ble_gap_cus_adv_param_t;

typedef union
{
    /// Numric Compare cfm
    bool user_confirmed;
    /// Passkey
    uint32_t passkey_6_digit;
    // OOB Term Key
    uint8_t oob_term_key[16];
    /// OOB Auth data peer/local
    struct
    {
        uint8_t pkx[32];
        uint8_t pky[32];
        uint8_t rand[16];
        uint8_t confirm[16];
    } oob_auth_data;
    /// LongTerm key
    struct
    {
        bool ltk_exist;
        uint8_t ltk[16];
    } ltk_cfm_value;
} bt_ble_smp_input_t;

typedef struct
{
    uint8_t data[BT_BLE_GAP_KEY_LEN];
} __attribute__((__packed__)) bt_ble_gap_irk_t;

typedef struct
{
    /// Device Name used in gatt and adv
    const uint8_t *p_dev_name;
    /// Length of Device Name used in gatt and adv
    uint8_t dev_name_len;
    /// Device Apperance
    uint16_t dev_appearance_uuid_le;
} bt_ble_dev_cfg_t;

typedef struct
{
    bt_ble_evt_type_t evt_type;
    bt_ble_evt_handled_t p;
} bt_ble_conn_evt_t;

typedef void (*bt_ble_core_evt_handler_func)(bt_ble_conn_evt_t *evt, void *);
