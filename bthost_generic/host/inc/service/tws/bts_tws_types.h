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

#ifndef __BTS_TWS_TYPES_H__
#define __BTS_TWS_TYPES_H__
#include "bt_tws_types.h"
#include "bts_common_types.h"
#include "app_ibrt_conn_evt.h"

/**
 ****************************************************************************************
 *    _______        ______    _____
 *   |_   _\ \      / / ___|  |_   _|   _ _ __   ___  ___
 *     | |  \ \ /\ / /\___ \    | || | | | '_ \ / _ \/ __|
 *     | |   \ V  V /  ___) |   | || |_| | |_) |  __/\__ \
 *     |_|    \_/\_/  |____/    |_| \__, | .__/ \___||___/
 *                                  |___/|_|
 *
 ****************************************************************************************
 */

// TODO:
#define  IBRT_UI_LONG_POLL_INTERVAL                         (0xD0)
#define  IBRT_UI_DEFAULT_POLL_INTERVAL                      (0x68)
#define  IBRT_UI_SHORT_POLL_INTERVAL                        (0x34)
#define  IBRT_UI_EXTREMELY_SHORT_POLL_INTERVAL              (0x1A)

#define  IBRT_UI_DEFAULT_POLL_INTERVAL_IN_SCO               (0xA8)
#define  IBRT_UI_SHORT_POLL_INTERVAL_IN_SCO                 (0x54)

#define  IBRT_TWS_BT_TPOLL_DEFAULT                          (80)

#define TWS_SYNC_BUF_SIZE           APP_TWS_CTRL_BUFFER_MAX_LEN

typedef enum
{
    TWS_SYNC_CONTINUE_FILLING   = 0,
    TWS_SYNC_BUF_FULL           = 1,
    TWS_SYNC_BUF_SEGMENTED      = 2,
    TWS_SYNC_NO_DATA_FILLED     = 3
} TWS_SYNC_FILL_RET_E;

typedef enum {
    TWS_SYNC_USER_BLE_INFO      = 0,
    TWS_SYNC_USER_OTA           = 1,
    TWS_SYNC_USER_AI_CONNECTION = 2,
    TWS_SYNC_USER_GFPS_INFO     = 3,
    TWS_SYNC_USER_AI_INFO       = 4,
    TWS_SYNC_USER_AI_MANAGER    = 5,
    TWS_SYNC_USER_DIP           = 6,
    TWS_SYNC_USER_LE_AUDIO      = 7,

    TWS_SYNC_USER_NUM,
} TWS_SYNC_USER_E;

typedef void (*TWS_SYNC_INFO_PREPARE_FUNC_T)(uint8_t *buf, uint16_t *totalLen, uint16_t *len, uint16_t expectLen);
typedef void (*TWS_INFO_SYNC_FUNC_T)(uint8_t *buf, uint16_t len, bool isContinueInfo);

typedef struct {
    TWS_SYNC_INFO_PREPARE_FUNC_T sync_info_prepare_handler;
    TWS_INFO_SYNC_FUNC_T sync_info_received_handler;
    TWS_SYNC_INFO_PREPARE_FUNC_T sync_info_prepare_rsp_handler;
    TWS_INFO_SYNC_FUNC_T sync_info_rsp_received_handler;
    TWS_INFO_SYNC_FUNC_T sync_info_rsp_timeout_handler;
} TWS_SYNC_USER_T;

typedef struct {
    TWS_SYNC_USER_E userId;
    bool continueInfo;
    uint16_t infoLen;     // length of the valid data in info[TWS_SYNC_BUF_SIZE-1]
    uint8_t info[TWS_SYNC_BUF_SIZE];
}TWS_SYNC_ENTRY_T;

#define TWS_SYNC_MAX_XFER_SIZE      sizeof(TWS_SYNC_ENTRY_T)

typedef struct {
    uint16_t totalLen;  // length of the valid data in content[TWS_SYNC_MAX_XFER_SIZE]
    uint8_t content[TWS_SYNC_MAX_XFER_SIZE];
}TWS_SYNC_DATA_T;

typedef struct {
    TWS_SYNC_USER_T syncUser[TWS_SYNC_USER_NUM];
    TWS_SYNC_DATA_T sync_data;
} TWS_ENV_T;

typedef struct {
    void (*keyboard_request_handler)(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
    void (*ui_perform_user_action)(uint8_t *p_buff, uint16_t length);
    void (*ui_role_updated)(uint8_t newRole);
}bts_tws_to_apps_cbs_t;

typedef struct
{
    uint32_t connect_no_03_timeout;
    uint32_t disconnect_no_05_timeout;

    uint8_t audio_sync_mismatch_resume_version;
    bool tws_switch_tx_data_protect;
    uint32_t tws_cmd_send_timeout;
    uint32_t tws_cmd_send_counter_threshold;
    uint16_t tws_connection_timeout;

    bool lowlayer_monitor_enable;
    bool allow_sniff_in_sco;
    uint16_t reconnect_ibrt_max_times;
    bool is_changed_to_ui_master_on_tws_disconnected;
    uint8_t llmonitor_report_format;
    uint32_t llmonitor_report_count;
} bts_tws_config_t;

typedef enum
{
    IBRT_UI_NO_ERROR,
    IBRT_UI_RSP_TIMEOUT,
    IBRT_UI_NOT_ACCEPT,
    IBRT_UI_CONNECT_FAILED,
    IBRT_UI_PAGE_TIMEOUT             = 0x04,
    IBRT_UI_STATUS_ERROR,
    IBRT_CONN_CONNECTION_TIMEOUT     = 0x08,
    IBRT_UI_ACL_ALREADY_EXIST        = 0x0B,
    IBRT_UI_CMD_DISALLOWED           = 0X0C,
    IBRT_UI_LIMITED_RESOURCE         = 0x0D,
    IBRT_UI_UNACCEPTABLE_ADDR        = 0x0F,
    IBRT_UI_HOST_ACCEPT_TIMEOUT      = 0x10,
    IBRT_CONN_TERM_USER_REQ          = 0x13,
    IBRT_UI_CONN_TERM_LOW_RESOURCES  = 0x14,
    IBRT_CONN_TERM_BY_LOCAL_HOST     = 0x16,
    IBRT_CONN_ERROR_LMP_RSP_TIMEOUT  = 0x22,
    IBRT_UI_MOBILE_CONN_DISCONNECTED = 0X2A,
    IBRT_UI_TWS_CONN_DISCONNECTED    = 0X2B,
    IBRT_UI_SNOOP_DISCONNECTED       = 0X2C,
    IBRT_UI_EST_OR_SYNC_TIMEOUT      = 0x3E,
    IBRT_CONN_NO_PROFILE_CONNECTED   = 0x50,
    IBRT_UI_CONNECTION_INCOMING      = 0x99,
    IBRT_UI_EVT_STATUS_ERROR         = 0x9A,
    IBRT_UI_TWS_CMD_SEND_FAILED      = 0X9B,
    IBRT_UI_MOBILE_PAIR_CANCLED      = 0X9C,
    BT_LINK_REAL_DISCONNECTED        = 0xB8,
    IBRT_BT_CANCEL_PAGE              = 0xB9,
    IBRT_UI_SWITCH_IN_POCESS         = 0xBA,
    IBRT_UI_SWITCH_TIME_OUT          = 0XBE,
    IBRT_LMP_TX_BUFFER_OVERFLOW      = 0xBD,
    IBRT_CONN_LINK_KEY_MISSING       = 0xF1,
} ibrt_conn_error_e;

/*
 * IBRT RAW_UI EVENT
 */
typedef enum {
    IBRT_STATUS_SUCCESS                  = 0,
    IBRT_STATUS_PENDING                  = 1,
    IBRT_STATUS_ERROR_INVALID_PARAMETERS = 2,
    IBRT_STATUS_ERROR_NO_CONNECTION      = 3,
    IBRT_STATUS_ERROR_CONNECTION_EXISTS  = 4,
    IBRT_STATUS_IN_PROGRESS              = 5,
    IBRT_STATUS_ERROR_DUPLICATE_REQUEST  = 6,
    IBRT_STATUS_ERROR_INVALID_STATE      = 7,
    IBRT_STATUS_ERROR_TIMEOUT            = 8,
    IBRT_STATUS_ERROR_ROLE_SWITCH_FAILED = 9,
    IBRT_STATUS_ERROR_UNEXPECTED_VALUE  = 10,
    IBRT_STATUS_ERROR_OP_NOT_ALLOWED    = 11,

    // Start vendor section
    IBRT_STATUS_VENDOR_START = 0x80,
    // BTCLIENT_STATUS_ERROR_SAMPLE = BTCLIENT_STATUS_VENDOR_START + 0,
    // End vendor
} ibrt_status_t;

#define  IBRT_UI_INVALID_RSSI                      (100)

#define  IBRT_UI_MIN_RSSI                          (-100)

#define  STEAL_MOBILE_TIMEOUT                      (1000)

#define IBRT_HOST_CANCEL_PAGE                      (2)

typedef struct
{
    uint16_t battery_volt;
    uint16_t  mobile_conhandle;
    uint16_t  tws_conhandle;
} app_ui_rssi_battery_info_t;

#define  IBRT_STOP_IBRT                         1

//HCI opcode
#define    IBRT_HCI_CREATE_CON_CMD_OPCODE       0x0405
#define    IBRT_HCI_EXIT_SNIFF_MODE_CMD_OPCODE  0x0804
#define    IBRT_HCI_SWITCH_ROLE_CMD_OPCODE      0x080B
#define    IBRT_HCI_STOP_IBRT_OPCODE            0xFCA8
#define    IBRT_HCI_START_IBRT_OPCODE           0xFCA3
#define    IBRT_HCI_RESET_OPCODE                0x0C03
#define    IBRT_HCI_DSIC_CON_CMD_OPCODE         0x0406
#define    IBRT_HCI_SET_ENV_CMD_OPCODE          0xFC8E
#define    IBRT_HCI_CREATE_CON_CANCEL_CMD_OPCODE    0x0408

typedef enum
{
    OUTGOING_CONNECTION_REQ = 0,
    INCOMMING_CONNECTION_REQ,

}connection_direction_t;

typedef struct {
    uint16_t opcode;              //Command Opcode
    const uint8_t *param;         //Return Parameters
    uint8_t param_len;            //Byte count
    uint8_t  num_hci_cmd_packets; //The number of command packets that the controller allows the host to send
} __attribute__ ((packed)) ibrt_cmd_comp_t;

typedef struct
{
    uint8     status;
    struct bdaddr_t bdaddr;
}__attribute__ ((packed)) ibrt_create_conn_cancel_complete_t;

typedef struct
{
    uint8_t  status;
    uint8_t  num_hci_cmd_packets;
    uint16_t cmd_opcode;
    bt_bdaddr_t bdaddr;
} __attribute__ ((packed)) ibrt_cmd_status_t;

typedef enum {
    ACL_CONNECTED_USR_TERMINATE        = 0,
    ACL_CONNECTING_CANCELED ,
    ACL_CONNECTING_FAILURE,
} acl_disconnect_reason;

typedef struct
{
    bool tws_switch_according_to_rssi_value;
    uint8_t rssi_threshold;
    uint8_t role_switch_timer_threshold;

    uint32_t connect_no_03_timeout;
    uint32_t disconnect_no_05_timeout;
} ibrt_conn_config_t;

typedef enum
{
    FREEMAN_MODE,
    IBRT_MODE,
}ibrt_mode_e;

typedef uint8_t tws_role_e;
#define   TWS_MASTER       0
#define   TWS_SLAVE        1
#define   TWS_ROLE_UNKNOW  0xff

typedef struct
{
    bt_bdaddr_t   mobile_addr;
    uint8_t       mobile_connected;
    uint8_t       sco_status;
} __attribute__((packed)) app_tws_start_ibrt_info_t;

typedef struct
{
    bt_bdaddr_t     local_addr;
    tws_role_e      current_ibrt_role;

    bt_bdaddr_t     peer_addr;
    tws_role_e      peer_ibrt_role;

    tws_role_e      nv_role;
} app_tws_buds_info_t;

typedef struct
{
    int32_t a2dp_volume;
    int32_t hfp_volume;
} ibrt_volume_info_t;

typedef void (*ibrt_post_func)(void);

//Register for BES UI
typedef struct {
    void (*send_mgr_info_hanlder_cb)(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
    void (*send_mgr_info_rsp_handler_cb)(uint8_t *p_buff, uint16_t length);
    void (*send_mgr_info_rsp_timeout_hanlder_cb)(uint8_t *p_buff, uint16_t length);
    void (*send_mgr_info_fail_cb)(uint8_t *p_buff);

    void (*link_run_complete_info_handler)(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
    void (*link_run_complete_info_rsp_handler)(uint8_t *p_buff, uint16_t length);
    void (*link_run_complete_info_rsp_timeout_handler)(uint8_t *p_buff, uint16_t length);

    void (*peer_run_complete_info_handler)(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
    void (*peer_run_complete_info_rsp_handler)(uint8_t *p_buff, uint16_t length);
    void (*peer_run_complete_info_rsp_timeout_handler)(uint8_t *p_buff, uint16_t length);

    void (*sync_info_handler)(uint16_t rsp_seq,uint8_t *p_buff, uint16_t length);
    void (*sync_info_rsp_handler)(uint8_t *p_buff, uint16_t length);
    void (*sync_info_rsp_timeout_handler)(uint8_t *p_buff, uint16_t length);

    void (*destroy_device_handler)(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
    void (*destroy_device_rsp_handler)(uint8_t *p_buff, uint16_t length);
    void (*destroy_device_rsp_timeout_handler)(uint8_t *p_buff, uint16_t length);

    void (*notify_ui_info_handler)(uint8_t *p_buff, uint16_t length);

    void (*common_chnl_recv_handler)(uint8_t *p_buff, uint16_t length);

#if BLE_AUDIO_ENABLED
    void (*notify_peer_bt_nv_recored_changed_handler)(uint8_t *p_buff, uint16_t length);
    void (*sync_deivce_irk_handler)(uint8_t *p_buff, uint16_t length);
#endif

    void (*send_dev_mgr_hanlder_cb)(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
    void (*send_dev_mgr_rsp_handler_cb)(uint8_t *p_buff, uint16_t length);
    void (*send_dev_mgr_rsp_timeout_hanlder_cb)(uint8_t *p_buff, uint16_t length);
} app_ibrt_cmd_ui_handler_cb;

/*************************************************************************************************
 *
 * TWS cmd types define
 *
 ************************************************************************************************/

#define APP_TWS_CMD_REFRESH_MASK    (0x01)


#define RSP_TIMEOUT_DEFAULT                        (5000)
#define RSP_TIMEOUT_FAST_ACK                       (2000)

//Get tws cmd user table by cmdcodec
#ifndef APP_IBRT_CMD_MASK
#define APP_IBRT_CMD_MASK                           0x0F00
#endif

typedef enum
{
    APP_TWS_CMD_DEFAULT_USER                        = 0,
    APP_TWS_CMD_CUSTOM_USER,
    APP_TWS_CMD_CUSTOM_DEMO_USER,
    APP_TWS_CMD_GFPS_USER,
    APP_TWS_CMD_RSSI_USER,
    APP_TWS_CMD_AI_USER,
    APP_TWS_CMD_A2DP_DECODER_USER,
    APP_TWS_CMD_INTERNAL_APP_USER,
    APP_TWS_CMD_OTA_USER,
    APP_TWS_CMD_OTA_INTERACTION_USER,
    //add new tws cmd table
    APP_TWS_CMD_MAX_USER_NUM,
} app_tws_cmd_user_e;

#define APP_IBRT_CMD_BASE                              0x8000

#ifndef APP_IBRT_CUSTOM_CMD_PREFIX
#define APP_IBRT_CUSTOM_CMD_PREFIX                     ((uint16_t)APP_TWS_CMD_CUSTOM_USER << 8)
#endif

#ifndef APP_IBRT_CUSTOM_DEMO_CMD_PREFIX
#define APP_IBRT_CUSTOM_DEMO_CMD_PREFIX                ((uint16_t)APP_TWS_CMD_CUSTOM_DEMO_USER << 8)
#endif

#ifndef APP_RSSI_CMD_PREFIX
#define APP_RSSI_CMD_PREFIX                            ((uint16_t)APP_TWS_CMD_RSSI_USER << 8)
#endif

#ifndef APP_AI_CMD_PREFIX
#define APP_AI_CMD_PREFIX                              ((uint16_t)APP_TWS_CMD_AI_USER << 8)
#endif

#ifndef APP_A2DP_DECODER_CMD_PREFIX
#define APP_A2DP_DECODER_CMD_PREFIX                    ((uint16_t)APP_TWS_CMD_A2DP_DECODER_USER << 8)
#endif

#ifndef APP_INTERNAL_CMD_PREFIX
#define APP_INTERNAL_CMD_PREFIX                        ((uint16_t)APP_TWS_CMD_INTERNAL_APP_USER << 8)
#endif

#ifndef APP_OTA_CMD_PREFIX
#define APP_OTA_CMD_PREFIX                             ((uint16_t)APP_TWS_CMD_OTA_USER << 8)
#endif

#ifndef APP_OTA_INTERACTION_CMD_PREFIX
#define APP_OTA_INTERACTION_CMD_PREFIX                 ((uint16_t)APP_TWS_CMD_OTA_INTERACTION_USER << 8)
#endif

#define APP_TWS_CMD_GET_PEER_MOBILE_RSSI               (0x8015|APP_RSSI_CMD_PREFIX)

#define app_ibrt_cmd_rsp_timeout_handler_null          (0)
#define app_ibrt_cmd_rsp_handler_null                  (0)
#define app_ibrt_cmd_rx_handler_null                   (0)
#define app_ibrt_cmd_tx_done_handler_null              (0)

typedef enum
{
    APP_TWS_CMD_PROFILE_DATA_EXCHANGE               = 0x8001,
    APP_TWS_CMD_PROFILE_DATA_EXCHANGE_DONE          = 0x8002,
    APP_TWS_CMD_RSP                                 = 0x8003,
    APP_TWS_CMD_SWITCH_ROLE                         = 0x8004,
    APP_TWS_CMD_SEND_PLAYBACK_INFO                  = 0x8005,
    APP_TWS_CMD_SET_TRIGGER_TIME                    = 0x8006,
    APP_TWS_CMD_NEED_RETRIGGER                      = 0x8007,
    APP_TWS_CMD_SYNC_TUNE                           = 0x8008,
    APP_TWS_CMD_SET_LATENCYFACTOR                   = 0x8009,
    APP_TWS_CMD_STOP_IBRT                           = 0x800B,
    APP_TWS_CMD_SET_ENV                             = 0x800C,
    APP_TWS_CMD_PROFILE_DATA_REQ                    = 0x800D,
    APP_TWS_CMD_FAST_ACK_REQ                        = 0x800E,
    APP_TWS_CMD_A2DP_STATUS_SYNC                    = 0x800F,
    APP_TWS_CMD_HFP_STATUS_SYNC                     = 0x8010,
    APP_TWS_CMD_VOICE_REPORT_REQUEST                = 0x8011,
    APP_TWS_CMD_VOICE_REPORT_START                  = 0x8012,
    APP_TWS_CMD_KEYBOARD_REQUEST                    = 0x8013,
    APP_TWS_CMD_SET_LINK_POLICY                     = 0x8016,
    APP_TWS_CMD_SEND_CONTROLLER_PROFILE             = 0x8017,
    APP_TWS_CMD_SYNC_VOLUME_INFO                    = 0x8018,
    APP_TWS_CMD_PERFORM_ACTION                      = 0x8019,
    APP_TWS_CMD_EXIT_MOBILE_SNIFF_MODE              = 0x801A,
    APP_TWS_CMD_STOP_IBRT_FAILED                    = 0x801B,
    APP_TWS_CMD_SHARE_COMMON_INFO                   = 0x801C,
    APP_TWS_CMD_SYNC_MIX_PROMPT_REQ                 = (0x801D|APP_INTERNAL_CMD_PREFIX),
    APP_TWS_CMD_STOP_PEER_PROMPT_REQ                = (0x801E|APP_INTERNAL_CMD_PREFIX),
    APP_TWS_CMD_LET_PEER_PLAY_PROMPT                = (0x801F|APP_INTERNAL_CMD_PREFIX),
    APP_TWS_CMD_LET_MASTER_PREPARE_RS               = 0x8020, // for slave triggered role switch, BISTO need the master to control the gsound role switch
    APP_TWS_CMD_LET_SLAVE_CONTINUE_RS               = 0x8021, // for slave triggered role switch, tell the slave gsound role switch has completed
    APP_TWS_CMD_MOBILE_LINK_PLAYBACK_INFO           = 0x8024,
    APP_TWS_CMD_SEND_IBRT_MGR_INFO                  = 0x8026,
    APP_TWS_CMD_NOTIFY_RUNNING_IDLE_INFO            = 0x8027,
    APP_TWS_CMD_CONN_PROFILE_REQ                    = 0x8028,
    APP_TWS_CMD_DISC_PROFILE_REQ                    = 0x8029,
    APP_TWS_CMD_DISC_RFCOMM_REQ                     = 0x802A,
    APP_TWS_CMD_SYNC_TOTA_ENCODE_STATUS             = 0x802B,
    APP_TWS_CMD_SEND_CUSTOM_PLAY_SPEED_TUNING_REQ   = (0x802C|APP_A2DP_DECODER_CMD_PREFIX),
    APP_TWS_CMD_SYNC_TARGET_BUF_CNT_REQ             = (0x802D|APP_A2DP_DECODER_CMD_PREFIX),
    APP_TWS_CMD_START_IBRT_FAILED                   = 0x802E,
    APP_TWS_CMD_SET_SYNC_TIME                       = (0x802F|APP_INTERNAL_CMD_PREFIX),
    APP_TWS_CMD_LET_MASTER_SEND_AT_CHLD             = 0x8030,
    APP_TWS_CMD_ENHANCED_ROLESWITCH                 = 0x8031,
    APP_TWS_CMD_SYNC_TXRX_CREDIT                    = 0x8032,
    APP_TWS_CMD_SYNC_INFO                           = 0x8033,
    APP_TWS_CMD_DESTROY_DEVICE                      = 0x8034,
    APP_TWS_CMD_NOTIFY_UI_INFO                      = 0x8035,
    APP_TWS_CMD_COMMON_CHNL                         = 0X8036,
    APP_TWS_CMD_NOTIFY_BT_NV_RECORED_CHANGED        = 0x8037,
    APP_TWS_CMD_SYNC_REMOTE_SMP_DEV                 = 0x8038,
    APP_TWS_CMD_SWITCH_BACKGROUND                   = 0x803A,
    APP_TWS_CMD_REFILL_FRAMES                       = 0x803B,
    APP_TWS_CMD_SHARE_BLE_CTKD_INFO                 = 0x803C,
    APP_TWS_CMD_FORWARD_PROMPT_PLAYING_REQ          = (0x803D|APP_INTERNAL_CMD_PREFIX),
    APP_TWS_CMD_SEND_DEV_MGR_INFO                   = 0x8042,
    APP_TWS_CMD_SEND_BIS_SELFSCAN_INFO              = 0x8044,
    APP_TWS_CMD_SEND_DEMO_APP_INFO                  = 0x8045,
    APP_TWS_CMD_EXCH_BLE_AUDIO_INFO                 = 0x8046,
    APP_TWS_CMD_SYNC_DEV_INFO                       = 0x8047,
    APP_TWS_CMD_SHARE_SERVICE_INFO                  = 0x8048,
    APP_TWS_CMD_REQ_TRIGGER_SYNC_CAPTURE            = 0x8049,
    APP_TWS_CMD_CAPTURE_US_SINCE_LATEST_ANCHOR      = 0x804A,
    APP_TWS_CMD_GMA_SECRET_KEY                      = (0x804C|APP_INTERNAL_CMD_PREFIX),
    APP_TWS_CMD_BISTO_DIP_SYNC                      = 0x804D,
    APP_TWS_CMD_DMA_AUDIO                           = (0x804E|APP_INTERNAL_CMD_PREFIX),
    APP_TWS_CMD_UPDATE_BITRATE                      = (0x804F|APP_INTERNAL_CMD_PREFIX),
    APP_TWS_CMD_REPORT_BUF_LVL                      = (0x8050|APP_INTERNAL_CMD_PREFIX),
    APP_TWS_CMD_SYNC_ANC_STATUS                     = (0x8051|APP_INTERNAL_CMD_PREFIX),
    APP_TWS_CMD_SYNC_PSAP_STATUS                    = (0x8052|APP_INTERNAL_CMD_PREFIX),
    APP_TWS_CMD_SYNC_ANC_ASSIST_STATUS              = (0x8053|APP_INTERNAL_CMD_PREFIX),
    APP_TWS_CMD_SYNC_AUDIO_PROCESS                  = (0x8054|APP_INTERNAL_CMD_PREFIX),
    APP_TWS_CMD_NOTIFY_SLAVE_MAX_LINK_CHANGE        = 0x8055,
    APP_TWS_CMD_CONTROL_SBM                         = (0x8056|APP_A2DP_DECODER_CMD_PREFIX),
    APP_TWS_CMD_SYNC_BIXBY_STATE                    = (0x8057|APP_INTERNAL_CMD_PREFIX),
    APP_TWS_CMD_SHARE_LINK_INFO                     = 0x8058,
    APP_TWS_CMD_ROLE_SWITCH_MONITOR                 = 0x8059,
    APP_TWS_CMD_RECORD_INFO                         = (0x8060|APP_INTERNAL_CMD_PREFIX),
    APP_TWS_CMD_OTA_UPDATE_NOW                      = (0x8062|APP_OTA_INTERACTION_CMD_PREFIX),
    APP_TWS_CMD_UPDATE_SECTION                      = (0x8063|APP_OTA_INTERACTION_CMD_PREFIX),
    APP_TWS_CMD_CHECK_UPDATE_INFO                   = (0x8064|APP_OTA_INTERACTION_CMD_PREFIX),
    APP_TWS_CMD_SYNC_BREAKPIONT                     = (0x8066|APP_OTA_INTERACTION_CMD_PREFIX),
    APP_TWS_CMD_VALIDATION_DONE                     = (0x8067|APP_OTA_INTERACTION_CMD_PREFIX),
    APP_TWS_CMD_OTA_GET_VERSION_CMD                 = (0x8068|APP_OTA_CMD_PREFIX),
    APP_TWS_CMD_OTA_SELECT_SIDE_CMD                 = (0x8069|APP_OTA_CMD_PREFIX),
    APP_TWS_CMD_OTA_BP_CHECK_CMD                    = (0x806A|APP_OTA_CMD_PREFIX),
    APP_TWS_CMD_OTA_START_OTA_CMD                   = (0x806B|APP_OTA_CMD_PREFIX),
    APP_TWS_CMD_OTA_OTA_CONFIG_CMD                  = (0x806C|APP_OTA_CMD_PREFIX),
    APP_TWS_CMD_OTA_SEGMENT_CRC_CMD                 = (0x806D|APP_OTA_CMD_PREFIX),
    APP_TWS_CMD_OTA_IMAGE_CRC_CMD                   = (0x806E|APP_OTA_CMD_PREFIX),
    APP_TWS_CMD_OTA_IMAGE_OVERWRITE_CMD             = (0x806F|APP_OTA_CMD_PREFIX),
    APP_TWS_CMD_OTA_SET_USER_CMD                    = (0x8070|APP_OTA_CMD_PREFIX),
    APP_TWS_CMD_OTA_GET_OTA_VERSION_CMD             = (0x8071|APP_OTA_CMD_PREFIX),
    APP_TWS_CMD_OTA_ROLE_SWITCH_CMD                 = (0x8072|APP_OTA_CMD_PREFIX),
    APP_TWS_CMD_OTA_MOBILE_DISC_CMD                 = (0x8073|APP_OTA_CMD_PREFIX),
    APP_TWS_CMD_OTA_UPDATE_RD_CMD                   = (0x8074|APP_OTA_CMD_PREFIX),
    APP_TWS_CMD_OTA_IMAGE_BUFF                      = (0x8075|APP_OTA_CMD_PREFIX),
    APP_TWS_CMD_GMA_OTA                             = (0X8076|APP_OTA_CMD_PREFIX),
    APP_TWS_CMD_COMMON_OTA                          = (0X8077|APP_OTA_CMD_PREFIX),
    APP_TWS_CMD_BLE_ROLE_SWITCH_SHARE_INFO          = 0x8078,
    APP_TWS_CMD_NOTIFY_RUN_COMPLETE_INFO            = 0x807A,
    APP_TWS_CMD_SYNC_IAP2_INFO                      = 0x807B,
    APP_TWS_CMD_AVRCP_STATUS_SYNC                   = 0x807C,
    APP_TWS_CMD_SEND_NV_APP_MODE                    = 0x807D,
    APP_TWS_CMD_PREVIEW_UI_CONSENSUS_JUMBO          = 0x80fe,
    APP_TWS_CMD_PREVIEW_UI_CONSENSUS                = 0x80ff,

    APP_TWS_CMD_SPECIAL_ALL_CANCEL                  = 0xffff,
} app_tws_cmd_code_e;

#endif /* __BTS_TWS_TYPES_H__ */