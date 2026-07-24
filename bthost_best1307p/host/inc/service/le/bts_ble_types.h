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

#ifndef __BTS_LE_TYPES_H__
#define __BTS_LE_TYPES_H__
#include "bt_le_types.h"
#include "bts_common_types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#define BTS_BLE_GAP_DECISION_DATA_MAX_LEN                   (8)
#define BTS_BLE_GAP_ADV_USER_NUM_MAX                        (19)
#define BTS_BLE_GAP_EXT_ADV_DATA_LEN                        (251)
#define BTS_BLE_GAP_SCAN_RSP_EX_DATA_LEN                    (251)
#define BTS_BLE_ADV_INVALID_INTERVAL                        (0)
#define BTS_BLE_ADVERTISING_INTERVAL_MS                     (160)
#define BTS_BLE_INVALID_CONIDX                              (0xFF)
#define BTS_BLE_FASTPAIR_NORMAL_ADVERTISING_INTERVAL        (160)
#define BTS_BLE_FASTPAIR_FAST_ADVERTISING_INTERVAL          (48)
#define BTS_BLE_FASTPAIR_SPOT_ADVERTISING_INTERVAL          (1000)
#define BTS_BLE_GFPSP_IDX_MAX                               (BLE_CONNECTION_MAX)
#define BTS_BLE_L2CAP_SPSM_GFPS                             (0x0081)
#define BTS_BLE_GFPS_STATE_UNAVAILABLE                      (2)
#define BTS_BLE_GFPS_STATE_READY_CONNECT                    (1)
#define BTS_BLE_GFPS_STATE_NOT_READY_CONNECT                (0)

#define BTS_BLE_GAP_BASIC_ADV_HANDLE                        0x00 // for legacy ble, ota, ai, dp, tile etc. advertising
#define BTS_BLE_GAP_AUDIO_ADV_HANDLE                        0x01 // for ble audio adertising
#define BTS_BLE_GAP_GFPS_ADV_HANDLE                         0x02
#define BTS_BLE_GAP_SPOT_ADV_HANDLE                         0x03
#define BTS_BLE_GAP_SWIFT_ADV_HANDLE                        0x04
#define BTS_BLE_GAP_FINDMY_ADV_HANDLE                       0x05
#define BTS_BLE_GAP_AI_ADV_HANDLE                           0x06
#define BTS_BLE_GAP_CUSTOMER0_ADV_HANDLE                    0x07
#define BTS_BLE_GAP_CUSTOMER1_ADV_HANDLE                    0x08
#define BTS_BLE_GAP_CUSTOMER2_ADV_HANDLE                    0x09
#define BTS_BLE_GAP_CUSTOMER3_ADV_HANDLE                    0x0a
#define BTS_BLE_GAP_MAX_FIXED_ADV_HANDLE                    0x0b

#define BTS_BLE_GAP_DT_FLAGS                                (0x01) // all numerical multi-byte entities and values shall use little-endian byte order
#define BTS_BLE_GAP_DT_SRVC_UUID_16_INCP_LIST               (0x02)
#define BTS_BLE_GAP_DT_SRVC_UUID_16_CMPL_LIST               (0x03)
#define BTS_BLE_GAP_DT_SRVC_UUID_32_INCP_LIST               (0x04)
#define BTS_BLE_GAP_DT_SRVC_UUID_32_CMPL_LIST               (0x05)
#define BTS_BLE_GAP_DT_SRVC_UUID_128_INCP_LIST              (0x06)
#define BTS_BLE_GAP_DT_SRVC_UUID_128_CMPL_LIST              (0x07)
#define BTS_BLE_GAP_DT_SHORT_LOCAL_NAME                     (0x08)
#define BTS_BLE_GAP_DT_COMPLETE_LOCAL_NAME                  (0x09)
#define BTS_BLE_GAP_DT_TX_POWER_LEVEL                       (0x0A)
#define BTS_BLE_GAP_DT_CLASS_OF_DEVICE                      (0x0D)
#define BTS_BLE_GAP_DT_SSP_HASH_C                           (0x0E)
#define BTS_BLE_GAP_DT_SSP_HASH_C_192                       (0x0E)
#define BTS_BLE_GAP_DT_SSP_RANDOMIZER_R                     (0x0F)
#define BTS_BLE_GAP_DT_SSP_RANDOMIZER_R_192                 (0x0F)
#define BTS_BLE_GAP_DT_DEVICE_ID                            (0x10)
#define BTS_BLE_GAP_DT_SM_TK_VALUE                          (0x10)
#define BTS_BLE_GAP_DT_SM_OOB_FLAGS                         (0x11)
#define BTS_BLE_GAP_DT_PERIPERAL_CONN_INTERVAL              (0x12)
#define BTS_BLE_GAP_DT_SRVC_SOLI_16_UUID_LIST               (0x14)
#define BTS_BLE_GAP_DT_SRVC_SOLI_128_UUID_LIST              (0x15)
#define BTS_BLE_GAP_DT_SERVICE_DATA                         (0x16)
#define BTS_BLE_GAP_DT_SERVICE_DATA_16BIT_UUID              (0x16)
#define BTS_BLE_GAP_DT_PUBLIC_TARGET_ADDRESS                (0x17)
#define BTS_BLE_GAP_DT_RANDOM_TARGET_ADDRESS                (0x18)
#define BTS_BLE_GAP_DT_APPEARANCE                           (0x19)
#define BTS_BLE_GAP_DT_ADV_INTERVAL                         (0x1A)
#define BTS_BLE_GAP_DT_LE_DEVICE_ADDRESS                    (0x1B)
#define BTS_BLE_GAP_DT_LE_ROLE                              (0x1C)
#define BTS_BLE_GAP_DT_SSP_HASH_C_256                       (0x1D)
#define BTS_BLE_GAP_DT_SSP_RANDOMIZER_R_256                 (0x1E)
#define BTS_BLE_GAP_DT_SRVC_SOLI_32_UUID_LIST               (0x1F)
#define BTS_BLE_GAP_DT_SERVICE_DATA_32BIT_UUID              (0x20)
#define BTS_BLE_GAP_DT_SERVICE_DATA_128BIT_UUID             (0x21)
#define BTS_BLE_GAP_DT_LE_SC_CONFIRM_VALUE                  (0x22)
#define BTS_BLE_GAP_DT_LE_SC_RANDOM_VALUE                   (0x23)
#define BTS_BLE_GAP_DT_URI                                  (0x24)
#define BTS_BLE_GAP_DT_INDOOR_POSITIONING                   (0x25)
#define BTS_BLE_GAP_DT_TRANSPORT_DISCOVERY_DATA             (0x26)
#define BTS_BLE_GAP_DT_LE_SUPPORTED_FEATURES                (0x27)
#define BTS_BLE_GAP_DT_CHAN_MAP_UPDATE_IND                  (0x28)
#define BTS_BLE_GAP_DT_MESH_PB_ADV                          (0x29)
#define BTS_BLE_GAP_DT_MESH_MESSAGE                         (0x2A)
#define BTS_BLE_GAP_DT_MESH_BEACON                          (0x2B)
#define BTS_BLE_GAP_DT_BIG_INFO                             (0x2C)
#define BTS_BLE_GAP_DT_BROADCAST_CODE                       (0x2D)
#define BTS_BLE_GAP_DT_RESOLVABLE_SET_ID                    (0x2E)
#define BTS_BLE_GAP_DT_ADV_INTERVAL_LONG                    (0x2F)
#define BTS_BLE_GAP_DT_BROADCAST_NAME                       (0x30)
#define BTS_BLE_GAP_DT_ENCRYPTED_ADV_DATA                   (0x31)
#define BTS_BLE_GAP_DT_PA_RSP_TIMING_INFO                   (0x32)
#define BTS_BLE_GAP_DT_3D_INFO_DATA                         (0x3D)
#define BTS_BLE_GAP_DT_MANUFACTURER_DATA                    (0xFF)

typedef enum
{
    BTS_BLE_GAP_ADV_USER_INUSE = 0,
    BTS_BLE_GAP_ADV_USER_ALL = BTS_BLE_GAP_ADV_USER_INUSE,
    BTS_BLE_GAP_ADV_USER_STUB,
    BTS_BLE_GAP_ADV_USER_GFPS,
    BTS_BLE_GAP_ADV_USER_SWIFT,
    BTS_BLE_GAP_ADV_USER_GSOUND,
    BTS_BLE_GAP_ADV_USER_AI,
    BTS_BLE_GAP_ADV_USER_INTERCONNECTION,
    BTS_BLE_GAP_ADV_USER_TILE,
    BTS_BLE_GAP_ADV_USER_OTA,
    BTS_BLE_GAP_ADV_USER_BLE_AUDIO,
    BTS_BLE_GAP_ADV_USER_SPOT,
    BTS_BLE_GAP_ADV_USER_FINDMY,
    BTS_BLE_GAP_ADV_USER_BLE_CUSTOMER_0,
    BTS_BLE_GAP_ADV_USER_BLE_CUSTOMER_1,
    BTS_BLE_GAP_ADV_USER_BLE_CUSTOMER_2,
    BTS_BLE_GAP_ADV_USER_BLE_CUSTOMER_3,
    BTS_BLE_GAP_ADV_USER_BLE_DEMO0,
    BTS_BLE_GAP_ADV_USER_BLE_DEMO1,
    BTS_BLE_GAP_ADV_USER_NUM,
} bts_ble_gap_adv_user_e;

typedef enum
{
    BTS_BLE_GAP_PHY_LE_UNKNOWN = 0x00,
    BTS_BLE_GAP_PHY_LE_1M = 0x01,
    BTS_BLE_GAP_PHY_LE_2M = 0x02,
    BTS_BLE_GAP_PHY_LE_CODED = 0x03,
    BTS_BLE_GAP_PHY_LE_HDT = 0x05,
    BTS_BLE_GAP_PHY_LE_MHDT = 0x08,
} bts_ble_gap_le_phy_t;

typedef enum
{
    /// Mode in non-discoverable
    BTS_GAP_ADV_DISC_MODE_NON_DISC = 0,
    /// Mode in general discoverable
    BTS_GAP_ADV_DISC_MODE_GEN_DISC,
    /// Mode in limited discoverable
    BTS_GAP_ADV_DISC_MODE_LIM_DISC,
    /// Broadcast mode without presence of AD_TYPE_FLAG in advertising data
    BTS_GAP_ADV_DISC_MODE_BEACON,
    BTS_GAP_ADV_DISC_MODE_MAX,
} bts_ble_gap_adv_disc_e;

typedef enum
{
    /// Public or Private Static Address according to device address configuration
    BTS_GAP_OWN_ADDR_STATIC_ADDR,
    /// Generated resolvable private random address
    BTS_GAP_OWN_ADDR_GEN_RSLV_ADDR,
    /// Generated non-resolvable private random address
    BTS_GAP_OWN_ADDR_GEN_NON_RSLV_ADDR,
} bts_ble_gap_own_type_e;

typedef enum
{
    BTS_BLE_GAP_ADV_TX_POWER_LEVEL_0 = 3,
    BTS_BLE_GAP_ADV_TX_POWER_LEVEL_1 = 4,
    BTS_BLE_GAP_ADV_TX_POWER_LEVEL_2 = 5,
} bts_ble_gap_adv_tx_pwr_lvl_e;

typedef enum
{
    /// datapath connect, para_p: uint8_t *conidx
    BTS_BLE_DP_CONN_DONE,
    /// datapath disconnect, para_p: uint8_t *conidx
    BTS_BLE_DP_DISCONN_DONE,
    /// change mtu done, para_p: bts_ble_dp_mtu_exchange_msg_t *
    BTS_BLE_DP_MTU_CHANGE_DONE,
    /// receive data , para_p:bts_ble_dp_rec_data_msg_t *
    BTS_BLE_DP_DATA_RECEIVED,
    /// tx done , para_p: NULL
    BTS_BLE_DP_TX_DONE,
} bts_ble_dp_evt_type_e;

typedef enum
{
    BTS_BLE_TOTA_CCC_CHANGED = 0,
    BTS_BLE_TOTA_DIS_CONN_EVENT,
    BTS_BLE_TOTA_RECEVICE_DATA,
    BTS_BLE_TOTA_MTU_UPDATE,
    BTS_BLE_TOTA_SEND_DONE,
} bts_ble_tota_evt_type_e;

typedef enum
{
    BTS_BLE_OTA_CCC_CHANGED = 0,
    BTS_BLE_OTA_DISCONN,
    BTS_BLE_OTA_RECEVICE_DATA,
    BTS_BLE_OTA_MTU_UPDATE,
    BTS_BLE_OTA_SEND_DONE,
} bts_ble_ota_evt_type_e;

typedef enum
{
    BTS_BLE_TX_PWR_LEVEL_1M          = 0,
    BTS_BLE_TX_PWR_LEVEL_2M          = 1,
    BTS_BLE_TX_PWR_LEVEL_LE_CODED_S8 = 2,
    BTS_BLE_TX_PWR_LEVEL_LE_CODED_S2 = 3,
} bts_ble_phy_pwr_value_e;

typedef enum
{
    BTS_BLE_TX_LOCAL     = 0,
    BTS_BLE_TX_REMOTE    = 1,
} bts_ble_tx_object_e;

typedef enum
{
    BTS_BLE_PHY_UNDEF_VALUE    = 0,
    BTS_BLE_PHY_1MBPS_VALUE    = 1,
    BTS_BLE_PHY_2MBPS_VALUE    = 2,
    BTS_BLE_PHY_CODED_VALUE    = 3,
} bts_le_phy_val_e;

typedef enum
{
    /// The Host has no preferred coding when transmitting on the LE Coded PHY
    BTS_BLE_PHY_OPT_NO_LE_CODED_TX_PREF,
    /// The Host prefers that S=2 coding be used when transmitting on the LE Coded PHY
    BTS_BLE_PHY_OPT_S2_LE_CODED_TX_PREF,
    /// The Host prefers that S=8 coding be used when transmitting on the LE Coded PHY
    BTS_BLE_PHY_OPT_S8_LE_CODED_TX_PREF,
} bts_le_phy_opt_e;

typedef enum
{
    BTS_BLE_SCAN_RESULT_USER_ID_0,
    BTS_BLE_SCAN_RESULT_USER_ID_1,

    BTS_BLE_SCAN_RESULT_USER_MAX,
} bts_ble_scan_result_user_id_e;

typedef enum
{
    BTS_BLE_CONN_PARAM_MODE_DEFAULT        = 0,
    BTS_BLE_CONN_PARAM_MODE_AI_STREAM_ON,
    BTS_BLE_CONN_PARAM_MODE_A2DP_ON,
    BTS_BLE_CONN_PARAM_MODE_HFP_ON,
    BTS_BLE_CONN_PARAM_MODE_OTA,
    BTS_BLE_CONN_PARAM_MODE_OTA_SLOWER,
    BTS_BLE_CONN_PARAM_MODE_SNOOP_EXCHANGE,
    BTS_BLE_CONN_PARAM_MODE_SVC_DISC,
    BTS_BLE_CONN_PARAM_MODE_ISO_DATA,
    BTS_BLE_CONN_PARAM_MODE_RATE_TEST,
    BTS_BLE_CONN_PARAM_MODE_IDLE,
    BTS_BLE_CONN_PARAM_MODE_NUM,
} bts_ble_conn_param_mode_e;

typedef enum
{
    BTS_BLE_SWITCH_USER_RS          = 0, // used for role switch
    BTS_BLE_SWITCH_USER_BOX         = 1, // used for box open/close
    BTS_BLE_SWITCH_USER_AI          = 2, // used for ai
    BTS_BLE_SWITCH_USER_BT_CONNECT  = 3, // used for bt connect
    BTS_BLE_SWITCH_USER_SCO         = 4, // used for sco
    BTS_BLE_SWITCH_USER_IBRT        = 5, // used for ibrt
    BTS_BLE_SWITCH_USER_FPGA        = 6, // used for fpga
    BTS_BLE_SWITCH_USER_BLE_AUDIO   = 7, // used for ble audio
    BTS_BLE_SWITCH_USER_CUSTOM      = 8, // used for custom

    BTS_BLE_SWITCH_USER_NUM,
} bts_ble_adv_switch_user_e;

typedef enum
{
    BTS_BLE_GFPS_EVT_LE_CONNECTED = 0,
    BTS_BLE_GFPS_EVT_LE_DISCONNECTED,
    BTS_BLE_GFPS_EVT_L2CAP_CONNECTED,
    BTS_BLE_GFPS_EVT_L2CAP_DISCONNECTED,
    BTS_BLE_GFPS_EVT_L2CAP_RX_REC,
    BTS_BLE_GFPS_EVT_KEY_BASE_PAIRING_CCCD,
    BTS_BLE_GFPS_EVT_KEY_BASE_PAIRING_IND,
    BTS_BLE_GFPS_EVT_PASS_KEY_CCCD,
    BTS_BLE_GFPS_EVT_PASS_KEY_IND,
    BTS_BLE_GFPS_EVT_ACCOUNT_KEY_IND,
    BTS_BLE_GFPS_EVT_WRITE_NAME_IND,
    BTS_BLE_GFPS_EVT_SPOT_WRITE_BEACON_IND,
    BTS_BLE_GFPS_EVT_SPOT_BEACON_CCCD,
    BTS_BLE_GFPS_EVT_GET_SPOT_VERSION,
    BTS_BLE_GFPS_EVT_GEN_SPOT_NOUNCE,
    BTS_BLE_GFPS_EVT_GET_SPOT_NOUNCE,
    BTS_BLE_GFPS_EVT_GET_MODEL_ID,
    BTS_BLE_GFPS_EVT_ADDITIONAL_PASS_KEY_IND,
    BTS_BLE_GFPS_EVT_ADDITIONAL_PASS_KEY_CCCD,
    BTS_BLE_GFPS_EVT_GET_ADDITIONAL_PASS_KEY,
    BTS_BLE_GFPS_EVT_GET_EVENT_STREAM,
} bts_ble_gfps_event_e;

typedef enum{
    /// Server initiated notification
    BTS_BLE_AI_GATT_NOTIFY     = 0x00,
    /// Server initiated indication
    BTS_BLE_AI_GATT_INDICATE   = 0x01,
} BTS_BLE_AI_SEND_EVENT_TYPE_E;

typedef enum{
    /// Server initiated notification
    BTS_BLE_AI_CMD     = 0x00,
    /// Server initiated indication
    BTS_BLE_AI_DATA    = 0x01,
    /// Does not distinguish the data types of CMD and DATA
    BTS_BLE_AI_ANY     = 0x02,
} BTS_BLE_AI_DATA_TYPE_E;

typedef enum{
    BTS_BLE_AI_SPEC_AMA     = 0x00,
    BTS_BLE_AI_SPEC_DMA,
    BTS_BLE_AI_SPEC_GMA,
    BTS_BLE_AI_SPEC_SMART,
    BTS_BLE_AI_SPEC_TENCENT,
    BTS_BLE_AI_SPEC_RECORDING,
    BTS_BLE_AI_SPEC_COMMON,
} BTS_BLE_AI_TYPE_E;

typedef enum{
    BTS_BLE_AI_SVC_ADD_DONE = 0x00,
    BTS_BLE_AI_CONN,
    BTS_BLE_AI_DISCONN,
    BTS_BLE_AI_MTU_CHANGE,
    BTS_BLE_AI_TX_DONE_EVENT,
    BTS_BLE_AI_RECEIVED_EVENT,
    BTS_BLE_AI_CHANGE_CCC_EVENT,
} BTS_BLE_AI_EVENT_TYPE_E;

typedef struct
{
    // parameter specifies the type of scan to perform,
    // 0:Passive, 1:Active
    uint8_t scanType;
    // Scanning Filter Policy, see@BLE_SCAN_FILTER_POLICY
    uint8_t scanFolicyType;
    // Scan window
    uint16_t scanWindowMs;
    // Scan Interval
    uint16_t scanIntervalMs;
    // Scan Duration
    uint16_t scanDurationMs;
} bts_ble_scan_param_t;

typedef struct
{
    uint16_t conn_interval_1_25ms;
    uint16_t peripheral_latency;
    uint16_t superv_timeout_ms;
    uint16_t central_clock_accuracy;
    uint16_t subrate_factor;
    uint16_t conn_continuation_number;
} bts_ble_conn_timing_t;

// typedef struct
// {
//     /// BD Address of device
//     uint8_t addr[BT_BLE_GAP_ADDR_LEN];
//     /// Address type of the device 0=public/1=private random
//     uint8_t addr_type;
// } bts_ble_bdaddr_t;

// typedef struct
// {
//     /// BD Address of device
//     uint8_t addr[BT_BLE_GAP_ADDR_LEN];
// } bts_bdaddr_t;

typedef struct
{
    /**
     * Recommended advertising interval for user initiated connectable modes:
     *      connectable undirected mode
     *      limited discoverable and connectable undirected mode
     *      general discoverable and connectable undirected mode
     *      connectable directed low duty cycle mode
     */
    uint32_t min_adv_fast_interval_ms; // LE 1M PHY
    uint32_t max_adv_fast_interval_ms; // LE 1M PHY
    uint32_t min_adv_fast_interval_coded_ms; // LE Coded PHY
    uint32_t max_adv_fast_interval_coded_ms; // LE Coded PHY
    /**
     * Recommended advertising interval for user initiated non-connectable modes:
     *      non-discoverable mode
     *      non-connectable mode
     *      limited discoverable mode
     *      general discoverable mode
     * Note: When advertising interval values of less than 100ms are used for
     * non-connectable or scannable undirected advertising in environments where
     * the advertiser can interfere with other devices, it is recommended that
     * steps be taken to minimize the interference. For example, the advertising
     * might be alternately enabled for only a few seconds and disabled for
     * several minutes.
     */
    uint32_t min_adv_slow_interval_ms; // LE 1M PHY
    uint32_t max_adv_slow_interval_ms; // LE 1M PHY
    uint32_t min_adv_slow_interval_coded_ms; // LE Coded PHY
    uint32_t max_adv_slow_interval_coded_ms; // LE Coded PHY
    /**
     * Recommended advertising interval for background advertising other then Directed Connectable High Duty Mode:
     */
    uint32_t min_adv_bg_interval_ms; // LE 1M PHY
    uint32_t max_adv_bg_interval_ms; // LE 1M PHY
    uint32_t min_adv_bg_interval_coded_ms; // LE Coded PHY
    uint32_t max_adv_bg_interval_coded_ms; // LE Coded PHY
} bts_ble_gap_adv_timing_t;

typedef struct
{
    void *p_buf;
} bts_ble_gap_dt_buf_t;

typedef struct
{
    // Resolvable Tag present
    bool resolvable_tag_present;
    // Decision Data length
    uint8_t decision_data_len;
    // Decision Data
    uint8_t decision_data[BTS_BLE_GAP_DECISION_DATA_MAX_LEN];
} bts_ble_gap_decision_t;

typedef struct
{
    bool force_start;
    bool connectable;
    bool scannable;
    bool directed_adv;
    bool anonymous_adv;
    bool high_duty_directed_adv; // <= 3.75ms adv interval
    bool use_legacy_pdu;
    bool include_tx_power_data; // include TxPower in the ext header of at least one adv pdu
    bool use_decisions_pdu; // Use decision PDUs when advertising
    bool include_adva_in_decisions; // Include AdvA in the extended header of all decision PDUs
    bool include_adi_in_decisions; // Include ADI in the extended header of all decision PDUs
    bool own_addr_use_rpa;
    uint8_t own_addr_type;
    uint8_t peer_type;
    bt_bdaddr_t peer_addr;
    uint8_t policy;
    bool use_custom_local_addr;
    bool use_fake_btc_rpa_when_no_irk_exist;
    bt_bdaddr_t custom_local_addr;
    bool limited_discoverable_mode;
    bool fast_advertising;
    bool continue_advertising;
    bool start_bg_advertising;
    bool has_prefer_adv_tx_power;
    bool has_custom_adv_timing;
    bool enable_scan_req_notify;
    int8_t adv_tx_power;
    uint8_t activity_priority;
    uint8_t adv_channel_map;
    uint8_t secondary_adv_max_skip;
    bts_ble_gap_le_phy_t primary_adv_phy;
    bt_ble_gap_coded_phy_prefer_t primary_coded_phy_prefer;
    bts_ble_gap_le_phy_t secondary_adv_phy;
    bt_ble_gap_coded_phy_prefer_t secondary_coded_phy_prefer;
    bts_ble_gap_adv_timing_t adv_timing;
    bts_ble_gap_dt_buf_t adv_data;
    bts_ble_gap_dt_buf_t scan_rsp_data;
    bts_ble_gap_decision_t decision_data;
    uint32_t duration_ms;
    uint8_t max_ext_adv_evts;
} bts_ble_gap_adv_param_t;

typedef struct bts_ble_gap_adv_actv_t
{
    uint8_t adv_handle;
    bool adv_is_started;
    bts_ble_gap_adv_user_e user;
    uint32_t custom_adv_interval_ms;
    bts_ble_gap_adv_param_t adv_param;
    bool (*adv_activity_func)(struct bts_ble_gap_adv_actv_t *adv);
    bt_bdaddr_t local_rpa;
} bts_ble_gap_adv_actv_t;

typedef struct
{
    bt_ble_gap_adv_activity_t adv_actv_user;
    bool isBleFlagsAdvDataConfiguredByAppLayer;
    uint32_t advUser;
    /// Advertising filtering policy (@see enum adv_filter_policy)
    uint32_t PeriodicIntervalMin;
    uint32_t PeriodicIntervalMax;
    uint8_t filter_pol;
    bt_ble_gap_adv_type_t advType;
    bt_ble_gap_adv_mode_t advMode;
    bts_ble_gap_adv_disc_e discMode;
    uint32_t advInterval;
    uint32_t advUserInterval[BTS_BLE_GAP_ADV_USER_NUM];
    // Maximum power level
    bts_ble_gap_adv_tx_pwr_lvl_e advTxPwr;
    uint8_t advDataLen;
    uint8_t advData[BTS_BLE_GAP_EXT_ADV_DATA_LEN];
    uint8_t scanRspDataLen;
    uint8_t scanRspData[BTS_BLE_GAP_SCAN_RSP_EX_DATA_LEN];
    uint8_t localAddrType;
    uint8_t localAddr[BT_BLE_GAP_ADDR_LEN];
    ble_bdaddr_t peerAddr;
    /// Lower layer fill device name is false
    bool disable_dev_name_fill;
}  __attribute__((__packed__)) bts_ble_gap_adv_param_p_t;

typedef struct
{
    uint8_t conidx;
    uint16_t mtu;
} bts_ble_dp_mtu_exchange_msg_t;

typedef struct
{
    uint8_t  *data;
    uint16_t data_len;
    uint8_t  conidx;
} bts_ble_dp_rec_data_msg_t;

typedef union
{
    uint8_t connect_index;
    uint8_t disconnect_index;
    bts_ble_dp_mtu_exchange_msg_t dp_mtu_exchange;
    bts_ble_dp_rec_data_msg_t dp_recv_data;
} bts_ble_dp_evt_param_u;

typedef struct
{
    /// tota event type, see@bts_ble_tota_evt_type_e
    uint8_t event_type;
    uint8_t conidx;
    uint16_t connhdl;
    union
    {
        /// BTS_BLE_TOTA_CCC_CHANGED
        uint8_t ntf_en;
        // BTS_BLE_TOTA_MTU_UPDATE
        uint16_t mtu;
        // BTS_BLE_TOTA_SEND_DONE
        uint8_t status;
        // BTS_BLE_TOTA_RECEVICE_DATA
        struct
        {
            uint16_t data_len;
            uint8_t *data;
        } receive_data;
    } param;
} bts_ble_tota_evt_param_t;

typedef struct
{
    /// tota event type, see@bts_ble_ota_evt_type_e
    uint8_t event_type;
    uint8_t conidx;
    uint16_t connhdl;
    union
    {
        // BTS_BLE_OTA_CCC_CHANGED
        uint8_t ntf_en;
        // BTS_BLE_OTA_MTU_UPDATE
        uint16_t mtu;
        // BTS_BLE_OTA_SEND_DONE
        uint8_t status;
        // BTS_BLE_OTA_RECEVICE_DATA
        struct
        {
            uint16_t data_len;
            uint8_t *data;
        } receive_data;
    } param;
} bts_ble_ota_evt_param_t;

typedef struct
{
    bool is_contain_pa;
    uint16_t pa_interval_1_25ms; // unit 1.25ms
    uint32_t ea_max_interval_ms; // unit ms
    uint32_t ea_min_interval_ms;
    uint8_t primary_adv_phy; // see gap_le_phy_t
    uint8_t secondary_adv_phy; //
} bts_ble_walkie_gap_adv_param_t; // see ble_walkie_gap_adv_param

typedef struct
{
    void (*adv_start)(uint8_t adv_hdl, uint8_t error_code_ea, uint8_t error_code_pa);
    void (*adv_stop)(uint8_t adv_hdl, uint8_t error_code, uint8_t error_code_pa);
    void (*scan_evt)(bool enable, uint8_t error_code);
    void (*recv_ea_data)(const uint8_t *data, uint8_t date_len, const uint8_t *mac, uint8_t rssi);
    void (*recv_pa_data)(uint16_t pa_handle, const uint8_t *data, uint8_t date_len, const uint8_t *mac, uint8_t rssi);
    void (*pa_sync_est)(uint16_t pa_handle, uint8_t *mac, uint8_t status);
    void (*pa_sync_lost)(uint16_t pa_handle, uint8_t *mac);
    void (*pa_sync_terminate)(uint16_t pa_handle, uint8_t *mac);
    void (*add_mesh_list)(uint8_t status);
    void (*clear_mesh_list)(uint8_t status);
} bts_ble_walkie_gap_callback; // see ble_walkie_gap_callback

typedef struct
{
    uint8_t *p_buf;
    uint16_t len;
} bts_ble_gfps_packet_t;

typedef struct
{
    bt_bdaddr_t localAddr;
    bt_bdaddr_t peerAddr;
} bts_ble_gfps_connection_t;

typedef struct
{
    uint8_t                 conidx;
    bts_ble_gfps_event_e    event;
    uint8_t                 *outData;
    uint16_t                outLen;
    union
    {
        bool                        enabled;
        bts_ble_gfps_connection_t   conn;
        bts_ble_gfps_packet_t       packet;
    } p;
} bts_ble_gfps_event_param_t;

typedef struct
{
    /// ai type see@BTS_BLE_AI_TYPE_E
    uint8_t  ai_type;
    /// Connection index
    uint8_t  conidx;
    /// Connection handle
    uint16_t connhdl;
    /// gatt event type, see@BTS_BLE_AI_SEND_EVENT_TYPE_E
    uint8_t  gatt_event_type;
    /// send data type. see@BTS_BLE_AI_DATA_TYPE_E
    uint8_t  data_type;
    /// data lenth
    uint32_t data_len;
    /// data pointer
    uint8_t  *data;
} bts_ble_ai_data_send_param_t;

typedef struct
{
    /// ai type see@BTS_BLE_AI_TYPE_E
    uint8_t  ai_type;
    /// gatt event type, see@BTS_BLE_AI_EVENT_TYPE_E
    uint8_t  event_type;
    /// Connection index
    uint8_t  conidx;
    /// Connection handle
    uint16_t connhdl;
    union {
        /// BTS_BLE_AI_MTU_CHANGE
        uint16_t mtu;
        /// BTS_BLE_AI_SVC_ADD_DONE
        struct {
            uint16_t start_hdl;
            uint16_t att_num;
        } svc_add_done;
        /// BTS_BLE_AI_RECEIVED_EVENT
        struct {
            uint8_t  data_type;
            uint32_t data_len;
            uint8_t  *data;
        } received;
        /// BTS_BLE_AI_CHANGE_CCC_EVENT
        struct {
            uint8_t data_type;
            uint8_t ntf_ind_flag;
        } chnage_ccc;
    }data;
} bts_ble_ai_event_param_t;

typedef struct app_ble_tile_event_param {
    uint8_t conidx;
    uint8_t event_type; // APP_BLE_TILE_EVENT_TYPE_E
    union {
        uint8_t result; // APP_BLE_TILE_TX_DATA_DONE_EVENT
        struct {
            uint16_t interval;
            uint16_t latency;
            uint16_t conn_sup_timeout;
        } ble_conn_param;
        struct { // APP_BLE_TILE_CH_CONN_EVENT APP_BLE_TILE_CH_DISCONN_EVENT
            uint8_t hdl;
            uint8_t status;
        } channel_conn;
        struct {
            uint8_t data_len;
            uint8_t *data;
        } receive;
    } data;
} bts_ble_tile_event_param_t;

typedef void (*bts_ble_gap_adv_data_fill_func)(void *advParam);
typedef bool (*bts_ble_gap_adv_actv_prepare_func)(bts_ble_gap_adv_actv_t *adv);
typedef void (*bts_ble_datapath_event_cb)(bts_ble_dp_evt_type_e evt_type, bts_ble_dp_evt_param_u *para_p);
typedef void (*bts_ble_tota_event_cb)(bts_ble_tota_evt_param_t *param);
typedef void (*bts_ble_ota_event_cb)(bts_ble_ota_evt_param_t *param);
typedef void (*bts_ble_adv_data_report_cb)(ble_bdaddr_t *ble_addr, int8_t rssi, uint8_t evt_type, uint8_t *adv_buf, uint8_t len);
typedef void (*bts_ble_conn_evt_report_cb)(bt_ble_evt_handled_t param, bt_ble_evt_type_t type);
typedef uint8_t (*bts_ble_gfps_event_cb)(bts_ble_gfps_event_param_t *entry);
typedef void (*bts_ble_ai_event_cb)(bts_ble_ai_event_param_t *param);
typedef void (*bts_ble_tile_event_cb)(bts_ble_tile_event_param_t *param);

#ifdef __cplusplus
}
#endif

#endif /* __BTS_LE_TYPES_H__ */
