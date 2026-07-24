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
#ifndef __ME_API__H__
#define __ME_API__H__
#include "bluetooth.h"
#include "hid_api.h"
#include "dip_api.h"
#include "hfp_api.h"
#include "avrcp_api.h"
#include "a2dp_api.h"
#include "conmgr_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t btif_event_type_t;

#define BTIF_BTEVENT_INQUIRY_RESULT                     1
#define BTIF_BTEVENT_INQUIRY_COMPLETE                   2
#define BTIF_BTEVENT_INQUIRY_CANCELED                   3
#define BTIF_BTEVENT_LINK_CONNECT_IND                   4
#define BTIF_BTEVENT_SCO_CONNECT_IND                    5
#define BTIF_BTEVENT_LINK_DISCONNECT                    6
#define BTIF_BTEVENT_LINK_CONNECT_CNF                   7
#define BTIF_BTEVENT_ROLE_DISCOVERED                    8
#define BTIF_BTEVENT_MODE_CHANGE                        9
#define BTIF_BTEVENT_ACCESSIBLE_CHANGE                  10
#define BTIF_BTEVENT_AUTHENTICATED                      11
#define BTIF_BTEVENT_ENCRYPTION_CHANGE                  12
#define BTIF_BTEVENT_SECURITY_CHANGE                    13
#define BTIF_BTEVENT_ROLE_CHANGE                        14
#define BTIF_BTEVENT_SCO_DISCONNECT                     15
#define BTIF_BTEVENT_SCO_CONNECT_CNF                    16
#define BTIF_BTEVENT_SIMPLE_PAIRING_COMPLETE            17
#define BTIF_BTEVENT_REMOTE_FEATURES                    18
#define BTIF_BTEVENT_REM_HOST_FEATURES                  19
#define BTIF_BTEVENT_LINK_SUPERV_TIMEOUT_CHANGED        20
#define BTIF_BTEVENT_SET_SNIFF_SUBRATING_PARMS_CNF      21
#define BTIF_BTEVENT_SNIFF_SUBRATE_INFO                 22
#define BTIF_BTEVENT_SET_INQUIRY_MODE_CNF               23
#define BTIF_BTEVENT_SET_INQ_TX_PWR_LVL_CNF             24
#define BTIF_BTEVENT_SET_EXT_INQUIRY_RESP_CNF           25
#define BTIF_BTEVENT_SET_ERR_DATA_REPORTING_CNF         26
#define BTIF_BTEVENT_KEY_PRESSED                        27
#define BTIF_BTEVENT_QOS_SETUP_COMPLETE                 28

/* Indicates the name of a remote device or cancellation of a name request. */
#define BTIF_BTEVENT_NAME_RESULT                        30
#define BTIF_BTEVENT_SCO_DATA_IND                       31
/**
 * Outgoing SCO data has been sent and the packet is free for re-use by
 *  the application.
 */
#define BTIF_BTEVENT_SCO_DATA_CNF                       32
#define BTIF_BTEVENT_LINK_CONNECT_REQ                   33
/* Incoming link accept complete.  */
#define BTIF_BTEVENT_LINK_ACCEPT_RSP                    34
/* Incoming link reject complete. . */
#define BTEVENT_LINK_REJECT_RSP                         35
#define BTIF_BTEVENT_COMMAND_COMPLETE                   36
#define BTIF_BTEVENT_SCO_CONNECT_REQ                    37
/* Set Audio/Voice settings complete.  */
#define BTIF_BTEVENT_SCO_VSET_COMPLETE                  38
/* SCO link connection process started. */
#define BTIF_BTEVENT_SCO_STARTED                        39
/* Select Device operation complete, "p.select" is valid. */
#define BTIF_BTEVENT_DEVICE_SELECTED                    40
/* The eSCO connection has changed. "p.scoConnect" is valid. */
#define BTIF_BTEVENT_SCO_CONN_CHNG                      41

/* Group: Security-related events. */

/* Indicates access request is successful. "p.secToken" is valid. */
#define BTIF_BTEVENT_ACCESS_APPROVED                    50

/* Indicates access request failed. "p.secToken" is valid. */
#define BTIF_BTEVENT_ACCESS_DENIED                      51
/**
 * Request authorization when "errCode" is BEC_NO_ERROR.
 * "p.remDev" is valid.
 */
#define BTIF_BTEVENT_AUTHORIZATION_REQ                  52
/**
 * Request a Pin for pairing when "errCode" is BEC_NO_ERROR.
 * "p.pinReq" is valid. If p.pinReq.pinLen is > 0 then SEC_SetPin()
 * must be called in response to this event with a pin length >=
 * p.pinReq.pinLen.
 */
#define BTIF_BTEVENT_PIN_REQ                            53
/* Pairing operation is complete. */
#define BTIF_BTEVENT_PAIRING_COMPLETE                   54
/* Authentication operation complete. "p.remDev" is valid. */
#define BTIF_BTEVENT_AUTHENTICATE_CNF                   55
/* Encryption operation complete. "p.remDev" is valid. */
#define BTIF_BTEVENT_ENCRYPT_COMPLETE                   56
/* Security mode 3 operation complete. "p.secMode" is valid. */
#define BTIF_BTEVENT_SECURITY3_COMPLETE                 57
/* A link key is returned. "p.bdLinkKey" is valid.  */
#define BTIF_BTEVENT_RETURN_LINK_KEYS                   58
/* Out of Band data has been received from the host controller. */
#define BTIF_BTEVENT_LOCAL_OOB_DATA                     59
/**
 * Request a Pass Key for simple pairing when "errCode" is BEC_NO_ERROR. The
 * application should call SEC_SetPassKey() to provide the passkey or reject
 * the request, and optionally save the link key.
 */
#define BTIF_BTEVENT_PASS_KEY_REQ                       60
/**
 * Request a User Confirmation for simple pairing when "errCode" is
 * BEC_NO_ERROR.
 */
#define BTIF_BTEVENT_CONFIRM_NUMERIC_REQ                61
#define BTIF_BTEVENT_DISPLAY_NUMERIC_IND                62
#define BTIF_BTEVENT_CONN_PACKET_TYPE_CHNG              63
#define BTIF_SDEVENT_QUERY_RSP                          70
#define BTIF_SDEVENT_QUERY_ERR                          71
#define BTIF_SDEVENT_QUERY_FAILED                       72
#define BTIF_BTEVENT_SELECT_DEVICE_REQ                  80
#define BTIF_BTEVENT_DEVICE_ADDED                       81
#define BTIF_BTEVENT_DEVICE_DELETED                     82
#define BTIF_BTEVENT_MAX_SLOT_CHANGED                   83
#define BTIF_BTEVENT_SNIFFER_CONTROL_DONE               84
#define BTIF_BTEVENT_LINK_POLICY_CHANGED                85
#define BTIF_BTEVENT_DEFAULT_LINK_POLICY_CHANGED        86
#define BTIF_BTEVENT_USER_PASSKEY_NOTIFY_EVENT          89
#define BTIF_BTEVENT_USER_PASSKEY_REQUEST_EVENT         90
#define BTIF_BTEVENT_IBRT_CONNECT_EVENT                 91
#define BTIF_BTEVENT_IBRT_DISCONNECT_EVENT              92
#define BTIF_BTEVENT_SET_BT_BLE_ACTIVE_LINK_COMPLETE    93
/**
 * an ACL connection has received an internal data transmit
 * request while it is in hold, park or sniff mode. The data will still be
 * passed to the radio in park and sniff modes. However, hold mode will
 * block data transmit. It may be necessary to return the ACL to active
 * mode to restore normal data transfer.
 */
#define BTIF_BTEVENT_ACL_DATA_NOT_ACTIVE                99
/**
 * Indicate that an ACL connection is sending or receiving data
 * while it is in active mode. Then, keep resetting the sniff timer.
 */
#define BTIF_BTEVENT_ACL_DATA_ACTIVE                    98
/* Indicates that the HCI failed to initialize. */
#define BTIF_BTEVENT_HCI_INIT_ERROR                     100
#define BTIF_BTEVENT_HCI_INITIALIZED                    101
/* Indicates that a fatal error has occurred in the radio or the HCI transport. */
#define BTIF_BTEVENT_HCI_FATAL_ERROR                    102
/* Indicates that the HCI has been deinitialized. */
#define BTIF_BTEVENT_HCI_DEINITIALIZED                  103
/* Indicates that the HCI cannot be initialized. */
#define BTIF_BTEVENT_HCI_FAILED                         104
#define BTIF_BTEVENT_HCI_COMMAND_SENT                   105

#define BTIF_BTEVENT_COMMAND_CMP_EVENT                  221
#define BTIF_BTEVENT_COMMAND_STATUS_EVENT               222


/*---------------------------------------------------------------------------
 * btif_class_of_device_t type
 *
 *     Bit pattern representing the class of device along with the
 *     supported services. There can be more than one supported service.
 *     Service classes can be ORed together. The Device Class is composed
 *     of a major device class plus a minor device class. ORing together
 *     each service class plus one major device class plus one minor device
 *     class creates the class of device value. The minor device class is
 *     interpreted in the context of the major device class.
 */

typedef uint32_t btif_class_of_device_t;

/* Group: Major Service Classes. Can be ORed together */
#define BTIF_COD_LIMITED_DISCOVERABLE_MODE 0x00002000
#define BTIF_COD_POSITIONING               0x00010000
#define BTIF_COD_NETWORKING                0x00020000
#define BTIF_COD_RENDERING                 0x00040000
#define BTIF_COD_CAPTURING                 0x00080000
#define BTIF_COD_OBJECT_TRANSFER           0x00100000
#define BTIF_COD_AUDIO                     0x00200000
#define BTIF_COD_TELEPHONY                 0x00400000
#define BTIF_COD_INFORMATION               0x00800000

/* Group: Major Device Classes (Select one) */
#define BTIF_COD_MAJOR_MISCELLANEOUS       0x00000000
#define BTIF_COD_MAJOR_COMPUTER            0x00000100
#define BTIF_COD_MAJOR_PHONE               0x00000200
#define BTIF_COD_MAJOR_LAN_ACCESS_POINT    0x00000300
#define BTIF_COD_MAJOR_AUDIO               0x00000400
#define BTIF_COD_MAJOR_PERIPHERAL          0x00000500
#define BTIF_COD_MAJOR_IMAGING             0x00000600
#define BTIF_COD_MAJOR_UNCLASSIFIED        0x00001F00

/* Group: Minor Device Class - Computer Major class */
#define BTIF_COD_MINOR_COMP_UNCLASSIFIED   0x00000000
#define BTIF_COD_MINOR_COMP_DESKTOP        0x00000004
#define BTIF_COD_MINOR_COMP_SERVER         0x00000008
#define BTIF_COD_MINOR_COMP_LAPTOP         0x0000000C
#define BTIF_COD_MINOR_COMP_HANDHELD       0x00000010
#define BTIF_COD_MINOR_COMP_PALM           0x00000014
#define BTIF_COD_MINOR_COMP_WEARABLE       0x00000018
#define BTIF_COD_MINOR_COMP_TABLET         0x0000001C

/* Group: Minor Device Class - Phone Major class */
#define BTIF_COD_MINOR_PHONE_UNCLASSIFIED  0x00000000
#define BTIF_COD_MINOR_PHONE_CELLULAR      0x00000004
#define BTIF_COD_MINOR_PHONE_CORDLESS      0x00000008
#define BTIF_COD_MINOR_PHONE_SMART         0x0000000C
#define BTIF_COD_MINOR_PHONE_MODEM         0x00000010
#define BTIF_COD_MINOR_PHONE_ISDN          0x00000014

/* Group: Minor Device Class - LAN Access Point Major class */
#define BTIF_COD_MINOR_LAN_0               0x00000000   /* fully available */
#define BTIF_COD_MINOR_LAN_17              0x00000020   /* 1-17% utilized */
#define BTIF_COD_MINOR_LAN_33              0x00000040   /* 17-33% utilized */
#define BTIF_COD_MINOR_LAN_50              0x00000060   /* 33-50% utilized */
#define BTIF_COD_MINOR_LAN_67              0x00000080   /* 50-67% utilized */
#define BTIF_COD_MINOR_LAN_83              0x000000A0   /* 67-83% utilized */
#define BTIF_COD_MINOR_LAN_99              0x000000C0   /* 83-99% utilized */
#define BTIF_COD_MINOR_LAN_NO_SERVICE      0x000000E0   /* 100% utilized */

/* Group: Minor Device Class - Audio Major class */
#define BTIF_COD_MINOR_AUDIO_UNCLASSIFIED  0x00000000
#define BTIF_COD_MINOR_AUDIO_HEADSET       0x00000004
#define BTIF_COD_MINOR_AUDIO_HANDSFREE     0x00000008
#define BTIF_COD_MINOR_AUDIO_MICROPHONE    0x00000010
#define BTIF_COD_MINOR_AUDIO_LOUDSPEAKER   0x00000014
#define BTIF_COD_MINOR_AUDIO_HEADPHONES    0x00000018
#define BTIF_COD_MINOR_AUDIO_PORTABLEAUDIO 0x0000001C
#define BTIF_COD_MINOR_AUDIO_CARAUDIO      0x00000020
#define BTIF_COD_MINOR_AUDIO_SETTOPBOX     0x00000024
#define BTIF_COD_MINOR_AUDIO_HIFIAUDIO     0x00000028
#define BTIF_COD_MINOR_AUDIO_VCR           0x0000002C
#define BTIF_COD_MINOR_AUDIO_VIDEOCAMERA   0x00000030
#define BTIF_COD_MINOR_AUDIO_CAMCORDER     0x00000034
#define BTIF_COD_MINOR_AUDIO_VIDEOMONITOR  0x00000038
#define BTIF_COD_MINOR_AUDIO_VIDEOSPEAKER  0x0000003C
#define BTIF_COD_MINOR_AUDIO_CONFERENCING  0x00000040
#define BTIF_COD_MINOR_AUDIO_GAMING        0x00000048

/* Group: Minor Device Class - Peripheral Major class */
#define BTIF_COD_MINOR_PERIPH_KEYBOARD     0x00000040
#define BTIF_COD_MINOR_PERIPH_POINTING     0x00000080
#define BTIF_COD_MINOR_PERIPH_COMBOKEY     0x000000C0
/* Group: Minor Device Class - ORed with Peripheral Minor Device class */
#define BTIF_COD_MINOR_PERIPH_UNCLASSIFIED 0x00000000
#define BTIF_COD_MINOR_PERIPH_JOYSTICK     0x00000004
#define BTIF_COD_MINOR_PERIPH_GAMEPAD      0x00000008
#define BTIF_COD_MINOR_PERIPH_REMOTECTRL   0x0000000C
#define BTIF_COD_MINOR_PERIPH_SENSING      0x00000010
#define BTIF_COD_MINOR_PERIPH_DIGITIZER    0x00000014
#define BTIF_COD_MINOR_PERIPH_CARD_RDR     0x00000018

/* Group: Minor Device Class - Imaging Major class */
#define BTIF_COD_MINOR_IMAGE_UNCLASSIFIED  0x00000000
#define BTIF_COD_MINOR_IMAGE_DISPLAY       0x00000010
#define BTIF_COD_MINOR_IMAGE_CAMERA        0x00000020
#define BTIF_COD_MINOR_IMAGE_SCANNER       0x00000040
#define BTIF_COD_MINOR_IMAGE_PRINTER       0x00000080

/* Group: Masks used to isolate the class of device components */
#define BTIF_COD_SERVICE_MASK              0x00ffC000   /* Less LIAC bit */
#define BTIF_COD_MAJOR_MASK                0x00001F00
#define BTIF_COD_MINOR_MASK                0x000000FC
#define BTIF_COD_LIMITED_DISC_MASK         0x00002000   /* LIAC bit */

#define BTIF_CMGR_SNIFF_DISABLED                   (0xFF)
#define BTIF_CMGR_SNIFF_DONT_CARE                  (0)
#define BTIF_CMGR_SNIFF_TIMER                      (10000)
#define BTIF_CMGR_BT_MASTER_SNIFF_TIMER            (22000)
#define BTIF_CMGR_BT_SLAVE_SNIFF_TIMER             (24000)
#define BTIF_CMGR_MOBILE_SNIFF_TIMER               (20000)
#define BTIF_CMGR_SNIFF_TIMER_AFTER_CONFLICT       (2000)
#define BTIF_CMGR_MOBILE_SNIFF_TIMEOUT             (0)

typedef uint8_t (*btif_callback_ext) (const bt_bdaddr_t *);

typedef void btif_cmgr_handler_t;

#ifndef BTIF_CMGR_DEFAULT_SNIFF_EXIT_POLICY
#define BTIF_CMGR_DEFAULT_SNIFF_EXIT_POLICY CMGR_SNIFF_EXIT_ON_AUDIO
#endif /*  */

#ifndef BTIF_CMGR_DEFAULT_SNIFF_TIMER
#define BTIF_CMGR_DEFAULT_SNIFF_TIMER CMGR_SNIFF_DONT_CARE
#endif /*  */

#ifndef BTIF_CMGR_AUDIO_DEFAULT_PARMS
//#define CMGR_AUDIO_DEFAULT_PARMS CMGR_AUDIO_PARMS_S3
#define BTIF_CMGR_AUDIO_DEFAULT_PARMS CMGR_AUDIO_PARMS_S4
#endif /*  */

#ifndef BTIF_CMGR_SNIFF_ATTEMPT
#define BTIF_CMGR_SNIFF_ATTEMPT 3
#endif /*  */

#ifndef BTIF_CMGR_SNIFF_TIMEOUT
#define BTIF_CMGR_SNIFF_TIMEOUT 1
#endif /*  */

#ifndef BTIF_CMGR_SNIFF_PARA_NEGOTIATE_MAX_TIMES
#define BTIF_CMGR_SNIFF_PARA_NEGOTIATE_MAX_TIMES 3
#endif /*  */

typedef uint8_t cmgr_event_t;

#define BTIF_CMEVENT_DATA_LINK_CON_CNF     1

#define BTIF_CMEVENT_DATA_LINK_CON_IND     2

#define BTIF_CMEVENT_DATA_LINK_DIS         3

#define BTIF_CMEVENT_AUDIO_LINK_REQ        4

#define BTIF_CMEVENT_AUDIO_LINK_CON        5

#define BTIF_CMEVENT_AUDIO_LINK_DIS        6

#define BTIF_CMEVENT_AUDIO_DATA            7

#define BTIF_CMEVENT_AUDIO_DATA_SENT       8

#define BTIF_CMEVENT_ENTER_SNIFF_MODE      9

#define BTIF_CMEVENT_EXIT_SNIFF_PARK_MODE  10


//when report_format=0,data_format will set the interval(unit:slot)
//when report_format=1,data_format will set the packet total num
//when report_format=2,data_format has no meaning
#define FLAG_RESET_DATA    2
#define FLAG_START_DATA    1
#define FLAG_GET_DATA        0

#define REP_FORMAT_TIME     0
#define REP_FORMAT_PACKET  1
#define REP_FORMAT_STOP_GET  2

#define  BTIF_EVT_ERROR_SCO_UNKNOWN       (0xE8)
#define  BTIF_EVT_ERROR_ACL_UNKNOWN       (0xE9)

#define BTIF_BTM_RX_FILTER_HCI_TYPE   (0x01)
#define BTIF_BTM_RX_FILTER_L2C_TYPE   (0x02)

#define BTIF_STANDARD_SCAN           0x00
#define BTIF_INTERLACED_SCAN         0x01

#define BTM_NAME_MAX_LEN                         248  /*include '\0'*/
#define BTM_SHORT_NAME_MAX_LEN                   32   /*include '\0'*/

#define MAX_ACTIVE_MODE_MANAGED_LINKS       3
#define UPDATE_ACTIVE_MODE_FOR_ALL_LINKS    MAX_ACTIVE_MODE_MANAGED_LINKS

typedef void (*btif_confirmation_req_callback_t)(struct bdaddr_t *bdaddr, uint32 numeric_value);

typedef void (*btif_sec_conn_callback_t)(bool enable);

typedef struct {
    uint16_t inqInterval;            /* Inquiry scan interval */
    uint16_t inqWindow;              /* Inquiry scan Window */
    uint16_t pageInterval;           /* Page scan interval */
    uint16_t pageWindow;             /* Page scan window */
} btif_access_mode_info_t;

typedef uint8_t btif_link_key_type_t;
struct btm_conn_item_t;

#define BTIF_COMBINATION_KEY            0x00
#define BTIF_LOCAL_UNIT_KEY             0x01
#define BTIF_REMOTE_UNIT_KEY            0x02
#define BTIF_DEBUG_COMBINATION_KEY      0x03
#define BTIF_UNAUTH_COMBINATION_KEY     0x04
#define BTIF_AUTH_COMBINATION_KEY       0x05
#define BTIF_CHANGED_COMBINATION_KEY    0x06
#define BTIF_UNAUTH_SC_COMBINATION_KEY  0x07
#define BTIF_AUTH_SC_COMBINATION_KEY    0x08
#define BTIF_IBRT_STORE_ADDR_ONLY       0x10

typedef struct
{
    bt_bdaddr_t bdAddr;           /* Device Address */
    uint8_t     linkKey[16];
} btif_dev_linkkey;

typedef struct {
    bt_bdaddr_t bdAddr;
    bool trusted;
    bool for_bt_source;
    uint8_t linkKey[16];
    btif_link_key_type_t keyType;
    uint8_t pinLen;
    uint8_t cod[3];
    char remote_dev_name[BTM_SHORT_NAME_MAX_LEN];
} btif_device_record_t;
typedef enum btif_mhdt_state_enum {
    BTIF_MHDT_MODE_DISABLED = 0,
    BTIF_MHDT_MODE_ENTERING,
    BTIF_MHDT_MODE_ENABLED,
    BTIF_MHDT_MODE_EXITING,
} btif_mhdt_state_t ;
typedef struct {
    uint16_t interval_min;
    uint16_t interval_max;
    uint8_t adv_type;
    uint8_t own_addr_type;
    uint8_t peer_addr_type;
    bt_bdaddr_t bd_addr;
    uint8_t adv_chanmap;
    uint8_t adv_filter_policy;
} btif_adv_para_struct_t;

#define BLE_ADV_REPORT_MAX_LEN 31

typedef struct {
    uint8_t type;
    uint8_t addr_type;
    bt_bdaddr_t addr;
    uint8_t data_len;
    uint8_t data[BLE_ADV_REPORT_MAX_LEN];
    int8_t rssi;
} btif_ble_adv_report;

typedef struct {
    uint8_t scan_type;
    uint16_t scan_interval;
    uint16_t scan_window;
    uint8_t own_addr_type;
    uint8_t scan_filter_policy;
} btif_scan_para_struct_t;

typedef struct btif_sniff_info_t {

    /* the mandatory sniff interval range for controllers is between 0x0006 and 0x0540.
     * The value is expressed in 0.625 ms increments (0x0006 = 3.75 ms).
     *
     * The actual interval selected by the radio will be returned in
     * a BTEVENT_MODE_CHANGE event.
     */

    uint16_t maxInterval;

    /* Minimum acceptable interval between each consecutive sniff period.
     * Must be an even number between 0x0002 and 0xFFFE, and be less than
     * "maxInterval". Like maxInterval this value is expressed in
     * 0.625 ms increments.
     */
    uint16_t minInterval;

    /* The number of master-to-slave transmission slots during which
     * a device should listen for traffic (sniff attempt).
     * Expressed in 0.625 ms increments. May be between 0x0001 and 0x7FFF.
     */
    uint16_t attempt;

    /* The amount of time before a sniff timeout occurs. Expressed in
     * 1.25 ms increments. May be between 0x0000 and 0x7FFF, but the mandatory
     * range for controllers is 0x0000 to 0x0028.
     */
    uint16_t timeout;
} btif_sniff_info_t;

typedef struct btif_sniff_timer_t {
    TimeT       timeout;                /* Sniff timer manager timeout param not sniff_mode's timeout*/
    bool        updata_sniff_timer;     /* Updata sniff timer manager timeout param*/
} btif_sniff_timer_t;

struct _evm_timer {
    struct list_node node;          /* Used internally by the Event Manager */
    void *context;              /* Context area for use by callers */
    evm_timer_notify func;      /* Function to call when timer fires */

    /* === Internal use only === */
    TimeT time;                 /* Amount of time to wait */
    TimeT startTime;            /* System time when the timer started */
};

typedef struct {
    uint16_t id;
    uint16_t status;

    uint16_t aud_type;
    uint32_t aud_id;

    uint8_t freq;
    uint8_t device_id;
} APP_AUDIO_STATUS;

enum APP_BT_AUDIO_Q_POS {
    APP_BT_SETTING_Q_POS_HEAD = 0,
    APP_BT_SETTING_Q_POS_TAIL = 1,
};

typedef enum {
    BT_ACTIVE_MODE_KEEP_USER_A2DP_STREAMING = 1, // always in active mode during a2dp streaming
    BT_ACTIVE_MODE_KEEP_USER_SCO_STREAMING,     // always in active mode during ai voice up-streaming
    BT_ACTIVE_MODE_KEEP_USER_AI_VOICE_STREAM,   // always in active mode during OTA
    BT_ACTIVE_MODE_KEEP_USER_OTA,               // stay in active mode for 15 seconds after role switch
    BT_ACTIVE_MODE_KEEP_USER_ROLE_SWITCH,       // stay in active mode during synchronous voice prompt playing
    BT_ACTIVE_MODE_KEEP_USER_SYNC_VOICE_PROMPT, // always in active mode during phone call
    BT_ACTIVE_MODE_KEEP_USER_SPP,               // stay in active mode for speed
} BT_ACTIVE_MODE_KEEP_USER_T;

typedef enum {
    BTIF_BLM_ACTIVE_MODE    = 0x00,
    BTIF_BLM_HOLD_MODE      = 0x01,
    BTIF_BLM_SNIFF_MODE     = 0x02,
} btif_link_mode_t;

typedef struct bt_remver_t
{
    uint8_t     vers;
    uint16_t    compid;
    uint16_t    subvers;
} __attribute__((packed)) bt_remver_t;

typedef uint16_t btif_link_policy_t;

#define BTIF_BLP_DISABLE_ALL         0x0000
#define BTIF_BLP_MASTER_SLAVE_SWITCH 0x0001
#define BTIF_BLP_HOLD_MODE           0x0002
#define BTIF_BLP_SNIFF_MODE          0x0004
#define BTIF_BLP_PARK_MODE           0x0008
#define BTIF_BLP_SCATTER_MODE        0x0010

typedef uint8_t btif_connection_role_t;

#define BTIF_BCR_MASTER   0x00
#define BTIF_BCR_SLAVE    0x01
#define BTIF_BCR_ANY      0x02
#define BTIF_BCR_UNKNOWN  0x03
#define BTIF_BCR_PSLAVE   0x04
#define BTIF_BCR_PMASTER  0x05

typedef uint8_t btif_accessible_mode_t;

#define BTIF_BAM_NOT_ACCESSIBLE     0x00    /* Non-discoverable or connectable */
#define BTIF_BAM_GENERAL_ACCESSIBLE 0x03    /* General discoverable and connectable */
#define BTIF_BAM_LIMITED_ACCESSIBLE 0x13    /* Limited discoverable and connectable */
#define BTIF_BAM_CONNECTABLE_ONLY   0x02    /* Connectable but not discoverable */
#define BTIF_BAM_DISCOVERABLE_ONLY  0x01    /* Discoverable but not connectable */
#define BTIF_BT_DEFAULT_ACCESS_MODE_PAIR       BTIF_BAM_GENERAL_ACCESSIBLE
#define BTIF_BAM_INVALID_ACCESS_MODE    0xFF

#define QOS_SETUP_SERVICE_TYPE_NO_TRAFFIC       0x00
#define QOS_SETUP_SERVICE_TYPE_BEST_EFFORT      0x01
#define QOS_SETUP_SERVICE_TYPE_GUARANTEED       0x02

typedef uint8_t voice_report_role_t;
#define VOICE_REPORT_MASTER   0x00
#define VOICE_REPORT_SLAVE    0x01
#define VOICE_REPORT_LOCAL    0x02

typedef void (*btif_global_handle)(const btif_event_t *Event);

typedef int32_t (*bt_gather_global_srv_uuids)(uint8_t in_uuid_size, uint8_t *out_buff,
    uint32_t out_buff_len, uint32_t *out_len, uint32_t *out_real_len);

typedef struct bt_acl_state_t {
    bt_bdaddr_t remote;
    uint16_t acl_conn_hdl;
    bool acl_is_connected;
    btif_link_mode_t acl_link_mode;
    uint8_t acl_bt_role;
    uint8_t device_id;
    uint16_t sniff_interval;
} bt_acl_state_t;

typedef enum {
    BT_PROFILE_HFP = 1,
    BT_PROFILE_A2DP,
    BT_PROFILE_AVRCP,
} BT_PROFILE_ID_ENUM_T;

typedef enum {
    APP_IBRT_HFP_PROFILE_ID = 1,
    APP_IBRT_A2DP_PROFILE_ID = 2,
    APP_IBRT_AVRCP_PROFILE_ID = 3,
    APP_IBRT_SDP_PROFILE_ID = 4,
    APP_IBRT_HID_PROFILE_ID = 5,
    APP_IBRT_MAX_PROFILE_ID,
} app_ibrt_profile_id_enum;

typedef enum {
    BT_PAGE_SCAN = 1,
    BT_INQUIRY_SCAN,
} BT_SCAN_ENUM_T;

typedef enum {
    BT_COUNT_MOBILE_LINK = 1,
    BT_COUNT_MOBILE_TWS_LINK,
    BT_COUNT_SOURCE_LINK,
    BT_COUNT_TOTAL_ACL_LINK,
    BT_COUNT_CONNECTED_SCO, // app_bt_audio_count_connected_sco
    BT_COUNT_STREAMING_A2DP, // app_bt_audio_count_streaming_a2dp
    BT_COUNT_STREAMING_LINK, // app_bt_audio_count_straming_mobile_links a2dp+sco
} BT_COUNT_LINK_ENUM_T;

typedef enum {
    BT_SELECT_CALL_ACTIVE_HFP_DEVICE = 1,
    BT_SELECT_CURR_A2DP_DEVICE,
    BT_SELECT_CURR_HFP_DEVICE, // app_bt_audio_get_curr_hfp_device
    BT_SELECT_CURR_PLAYING_SCO, // app_bt_audio_get_curr_playing_sco
    BT_SELECT_CURR_PLAYING_A2DP,
    BT_SELECT_CONNECTED_DEVICE, // app_bt_audio_select_connected_device
    BT_SELECT_USER_ACTION_DEVICE, // app_bt_audio_get_device_for_user_action
} BT_SELECT_DEVICE_ENUM_T;

typedef enum {
    BT_SELECT_ANOTHER_CREATE_SCO_DEVICE = 1,
    BT_SELECT_ANOTHER_STREAMING_A2DP_DEVICE, // app_bt_audio_select_another_streaming_a2dp
} BT_SELECT_ANOTHER_DEVICE_ENUM_T;



typedef uint32_t btif_event_mask_t;

#define BTIF_BEM_NO_EVENTS                    0x00000000
#define BTIF_BEM_ALL_EVENTS                   0xffffffff

#define BTIF_BEM_INQUIRY_RESULT               0x00000001
#define BTIF_BEM_INQUIRY_COMPLETE             0x00000002
#define BTIF_BEM_INQUIRY_CANCELED             0x00000004
#define BTIF_BEM_LINK_CONNECT_IND             0x00000008
#define BTIF_BEM_SCO_CONNECT_IND              0x00000010
#define BTIF_BEM_LINK_DISCONNECT              0x00000020
#define BTIF_BEM_LINK_CONNECT_CNF             0x00000040
#define BTIF_BEM_ROLE_DISCOVERED              0x00000080
#define BTIF_BEM_MODE_CHANGE                  0x00000100
#define BTIF_BEM_ACCESSIBLE_CHANGE            0x00000200
#define BTIF_BEM_AUTHENTICATED                0x00000400
#define BTIF_BEM_ENCRYPTION_CHANGE            0x00000800
#define BTIF_BEM_ROLE_CHANGE                  0x00001000
#define BTIF_BEM_SCO_DISCONNECT               0x00002000
#define BTIF_BEM_SCO_CONNECT_CNF              0x00004000
#define BTIF_BEM_SIMPLE_PAIRING_COMPLETE      0x00008000
#define BTIF_BEM_LINK_POLICY_CHANGED          0x00010000
#define BTIF_BEM_CMD_COMPLETE                 0x00020000
#define BTIF_BEM_CMD_STATUS                   0x00040000

typedef uint32_t btif_iac_t;

#define BTIF_BT_IAC_GIAC 0x9E8B33   /* General/Unlimited Inquiry Access Code */
#define BTIF_BT_IAC_LIAC 0x9E8B00   /* Limited Dedicated Inquiry Access Code */

typedef uint8_t btif_link_type_t;

#define BTIF_BLT_SCO   0x00
#define BTIF_BLT_ACL   0x01
#define BTIF_BLT_ESCO  0x02

typedef U16 btif_acl_packet;

#define BTIF_BAPT_NO_2_DH1  0x0002
#define BTIF_BAPT_NO_3_DH1  0x0004
#define BTIF_BAPT_DM1       0x0008
#define BTIF_BAPT_DH1       0x0010
#define BTIF_BAPT_NO_2_DH3  0x0100
#define BTIF_BAPT_NO_3_DH3  0x0200
#define BTIF_BAPT_DM3       0x0400
#define BTIF_BAPT_DH3       0x0800
#define BTIF_BAPT_NO_2_DH5  0x1000
#define BTIF_BAPT_NO_3_DH5  0x2000
#define BTIF_BAPT_DM5       0x4000
#define BTIF_BAPT_DH5       0x8000

#define BTIF_2M_PACKET     (BTIF_BAPT_DM1|BTIF_BAPT_DH1|BTIF_BAPT_NO_3_DH1|BTIF_BAPT_NO_3_DH3|BTIF_BAPT_DM3|BTIF_BAPT_DH3|BTIF_BAPT_NO_3_DH5)
#define BTIF_3M_PACKET     (BTIF_BAPT_DM1|BTIF_BAPT_DH1|BTIF_BAPT_DM3|BTIF_BAPT_DH3|BTIF_BAPT_DM5|BTIF_BAPT_DH5)
#define BTIF_1_SLOT_PACKET (BTIF_BAPT_DM1|BTIF_BAPT_DH1|BTIF_BAPT_NO_3_DH1|BTIF_BAPT_NO_2_DH3|BTIF_BAPT_NO_3_DH3|BTIF_BAPT_NO_2_DH5|BTIF_BAPT_NO_3_DH5)
#define BTIF_3_SLOT_PACKET (BTIF_BAPT_DM1|BTIF_BAPT_NO_3_DH3|BTIF_BAPT_NO_2_DH5|BTIF_BAPT_NO_3_DH5)

/* Mask must be updated if new policy values are added */
#define BLP_MASK                0xfff0  /* Disables ScatterNet bit */
#define BLP_SCATTER_MASK        0xffe0  /* Enables ScatterNet bit */

/* End of BtAccessibleMode */

typedef uint8_t btif_oob_data_present_t;

#define BTIF_OOB_DATA_NOT_PRESENT  0    /* No Out of Band Data is present */
#define BTIF_OOB_DATA_PRESENT      1    /* Out of Band Data is present    */

typedef uint8_t btif_auth_requirements_t;

#define BTIF_MITM_PROTECT_NOT_REQUIRED  0x00    /* No Man in the Middle protection  */
#define BTIF_MITM_PROTECT_REQUIRED      0x01    /* Man in the Middle protection req */

#define  BTIF_BAS_NOT_AUTHENTICATED  0x00
#define  BTIF_BAS_START_AUTHENTICATE 0x01
#define  BTIF_BAS_WAITING_KEY_REQ    0x02
#define  BTIF_BAS_SENDING_KEY        0x03
#define  BTIF_BAS_WAITING_FOR_IO     0x04
#define  BTIF_BAS_WAITING_FOR_IO_R   0x05
#define  BTIF_BAS_WAITING_FOR_KEY    0x06
#define  BTIF_BAS_WAITING_FOR_KEY_R  0x07
#define  BTIF_BAS_AUTHENTICATED      0x08

typedef uint8_t btif_stack_state_t;

/* The stack has completed initialization of the radio hardware. */
#define BTIF_BTSS_NOT_INITIALIZED 0

/* The stack is initialized. */
#define BTIF_BTSS_INITIALIZED     1

/* The stack has encountered an error while initializing the radio hardware. */
#define BTIF_BTSS_INITIALIZE_ERR  2

/* The stack is deinitializing. */
#define BTIF_BTSS_DEINITIALIZE    3

typedef void (*btif_callback) (const btif_event_t *);
typedef void (*ibrt_cmd_status_callback)(const uint8_t *para);
typedef uint8_t (*btif_callback_ext2) (void);
typedef void (*btif_callback_ext3) (void);

typedef uint8_t btif_inquiry_mode_t;

#define BTIF_INQ_MODE_NORMAL    0   /* Normal Inquiry Response format           */
#define BTIF_INQ_MODE_RSSI      1   /* RSSI Inquiry Response format             */
#define BTIF_INQ_INVALID_RSSI   127   /* RSSI Inquiry Response format             */
#define BTIF_INQ_MODE_EXTENDED  2   /* Extended or RSSI Inquiry Response format */

typedef uint8_t btif_rem_dev_state_t;

#define BTIF_BDS_DISCONNECTED  0x00
#define BTIF_BDS_OUT_CON       0x01 /* Starting an out going connection */
#define BTIF_BDS_IN_CON        0x02 /* In process of incoming connection */
#define BTIF_BDS_CONNECTED     0x03 /* Connected */
#define BTIF_BDS_OUT_DISC      0x04 /* Starting an out going disconnect */
#define BTIF_BDS_OUT_DISC2     0x05 /* Disconnect status received */
#define BTIF_BDS_OUT_CON2      0x06 /* In SCO, used when connection request has
                                       been sent */

typedef uint8_t btif_sco_conn_state_t;

// Macro that defines the SCO connection state
#define BTIF_SCO_STATE_CLOSED          0x00  // disconnected
#define BTIF_SCO_STATE_WAIT_ACL        0x01  // the SCO connection is waiting for the ACL connection to be connected first
#define BTIF_SCO_STATE_CONNECTING      0x02  // outgoing connecting
#define BTIF_SCO_STATE_INCOMING_REQ    0x03  // incoming request
#define BTIF_SCO_STATE_INCOMING_ACCEPT 0x04  // incoming accept
#define BTIF_SCO_STATE_INCOMING_REJECT 0x05  // incoming reject
#define BTIF_SCO_STATE_OPENED          0x06  // connected
#define BTIF_SCO_STATE_INVALID         0xFF  // invalid state

typedef uint8_t btif_eir_data_type_t;

#define BTIF_EIR_FLAGS                0x01
#define BTIF_EIR_SRV_CLASS_16_PART    0x02
#define BTIF_EIR_SRV_CLASS_16_WHOLE   0x03
#define BTIF_EIR_SRV_CLASS_32_PART    0x04
#define BTIF_EIR_SRV_CLASS_32_WHOLE   0x05
#define BTIF_EIR_SRV_CLASS_128_PART   0x06
#define BTIF_EIR_SRV_CLASS_128_WHOLE  0x07
#define BTIF_EIR_REM_NAME_PART        0x08
#define BTIF_EIR_REM_NAME_WHOLE       0x09
#define BTIF_EIR_TX_POWER             0x0A
#define BTIF_EIR_MAN_SPECIFIC         0xFF


#define BTIF_BR_USE_FEC              (0<<0)
#define BTIF_BR_NOT_USE_FEC          (1<<0)
#define BTIF_BR_NO_PKT_PRFER_AVALB   (0<<1)
#define BTIF_BR_USE_1_SLOT_PKT       (1<<1)
#define BTIF_BR_USE_3_SLOT_PKT       (2<<1)
#define BTIF_BR_USE_5_SLOT_PKT       (3<<1)
#define BTIF_EDR_USE_DM1_PKT         (0<<3)
#define BTIF_EDR_USE_2MB_PKT         (1<<3)
#define BTIF_EDR_USE_3MB_PKT         (2<<3)
#define BTIF_EDR_NO_PKT_PRFER_AVALB  (0<<5)
#define BTIF_EDR_USE_1_SLOT_PTK      (1<<5)
#define BTIF_EDR_USE_3_SLOT_PTK      (2<<5)
#define BTIF_EDR_USE_5_SLOT_PTK      (3<<5)


#define CONN_AUTO_ACCEPT_DISABLE                      0x01

#define CONN_AUTO_ACCEPT_ENABLE_WITH_MSS_DISABLE      0x02

#define CONN_AUTO_ACCEPT_ENABLE_WITH_MSS_ENABLE       0x03


/* Group: BES vendore debug CMD sub opcode*/
#define BTIF_DBG_ENABLE_BTPCM  9
#define BTIF_DBG_SET_TWS_LINK  10

typedef enum {
    BT_PROPERTY_TYPE_LOCAL_BDNAME = 0x01,   // only set
    BT_PROPERTY_TYPE_LOCAL_BDADDR,          // only set
    BT_PROPERTY_TYPE_ACCESS_MODE,           // only set
    BT_PROPERTY_TYPE_REMOTE_NAME,           // only get
} bt_property_data_type_t;

typedef enum {
    BT_ROLE_MASTER  = 0x00,
    BT_ROLE_SLAVE   = 0x01,
} bt_link_role_t;

typedef enum {
    BT_ACCESS_NOT_ACCESSIBLE        = 0x00,
    BT_ACCESS_DISCOVERABLE_ONLY     = 0x01,
    BT_ACCESS_CONNECTABLE_ONLY      = 0x02,
    BT_ACCESS_GENERAL_ACCESSIBLE    = 0x03,
    BT_ACCESS_LIMITED_ACCESSIBLE    = 0x13,
} btif_access_mode_t;

typedef struct {
    uint8_t error_code;
    uint8_t acl_bt_role;
    uint16_t conn_handle;
    uint8_t device_id;
    uint8_t local_is_source;
} bt_adapter_acl_opened_param_t;

typedef struct {
    uint8_t error_code;
    uint8_t disc_reason;
    uint16_t conn_handle;
    uint8_t device_id;
} bt_adapter_acl_closed_param_t;

typedef struct {
    uint8_t error_code;
} bt_adapter_acl_connect_req_param_t;

typedef struct {
    uint8_t error_code;
    uint8_t codec;
    uint16_t sco_handle;
    uint8_t interval;
    uint8_t window;
} bt_adapter_sco_opened_param_t;

typedef struct {
    uint8_t error_code;
    uint8_t disc_reason;
    uint16_t sco_handle;
} bt_adapter_sco_closed_param_t;

typedef struct {
    uint8_t device_id;
    const uint8_t *remote;
} bt_adapter_sco_conn_req_param_t;

typedef struct {
    btif_access_mode_t access_mode;
} bt_adapter_access_change_param_t;

typedef struct {
    uint8_t error_code;
    bt_link_role_t acl_bt_role;
} bt_adapter_role_discover_param_t;

typedef struct {
    uint8_t error_code;
    bt_link_role_t acl_bt_role;
} bt_adapter_role_change_param_t;

typedef struct {
    uint8_t error_code;
    btif_link_mode_t acl_link_mode;
    uint16_t sniff_interval;
} bt_adapter_mode_change_param_t;

typedef struct {
    uint8_t error_code;
} bt_adapter_authenticated_param_t;

typedef struct {
    uint8_t error_code;
    uint8_t encrypted;
} bt_adapter_enc_change_param_t;

#define BT_RSSI_INVALID_VALUE 127

typedef struct {
    bt_bdaddr_t remote;
    uint8_t page_scan_repeat_mode;
    int8_t rssi;
    uint16_t clock_offset;
    uint32_t class_of_device;
    uint8_t *eir;
} bt_adapter_inquiry_result_param_t;

typedef struct {
    uint8_t error_code;
} bt_adapter_inquiry_complete_param_t;

typedef struct {
    uint8_t error_code;
} bt_adapter_bond_state_change_param_t;

typedef struct {
    bt_property_data_type_t type;
    int len;
    uintptr_t data;
} bt_adapter_property_param_t;

typedef union {
    bt_adapter_acl_opened_param_t *acl_opened;
    bt_adapter_acl_closed_param_t *acl_closed;
    bt_adapter_acl_connect_req_param_t *acl_connect_req;
    bt_adapter_sco_opened_param_t *sco_opened;
    bt_adapter_sco_closed_param_t *sco_closed;
    bt_adapter_access_change_param_t *access_change;
    bt_adapter_role_discover_param_t *role_discover;
    bt_adapter_role_change_param_t *role_change;
    bt_adapter_mode_change_param_t *mode_change;
    bt_adapter_authenticated_param_t *authenticated;
    bt_adapter_enc_change_param_t *enc_change;
    bt_adapter_inquiry_result_param_t *inq_result;
    bt_adapter_inquiry_complete_param_t *inq_complete;
    bt_adapter_bond_state_change_param_t *bond_change;
    bt_adapter_property_param_t *perperty_state;
} bt_adapter_callback_param_t;

typedef enum {
    BT_ADAPTER_EVENT_ACL_OPENED = BT_EVENT_ACL_OPENED,
    BT_ADAPTER_EVENT_ACL_CLOSED,
    BT_ADAPTER_EVENT_ACL_CONNECT_REQ,
    BT_ADAPTER_EVENT_SCO_OPENED,
    BT_ADAPTER_EVENT_SCO_CLOSED,
    BT_ADAPTER_EVENT_ACCESS_CHANGE,
    BT_ADAPTER_EVENT_ROLE_DISCOVER,
    BT_ADAPTER_EVENT_ROLE_CHANGE,
    BT_ADAPTER_EVENT_MODE_CHANGE,
    BT_ADAPTER_EVENT_AUTHENTICATED,
    BT_ADAPTER_EVENT_ENC_CHANGE,
    BT_ADAPTER_EVENT_INQUIRY_RESULT,
    BT_ADAPTER_EVENT_INQUIRY_COMPLETE,
    BT_ADAPTER_EVENT_BOND_STATE_CHANGE,
    BT_ADAPTER_EVENT_END,
} bt_adapter_event_t;

#if BT_ADAPTER_EVENT_END != BT_EVENT_LINK_END
#error "bt_adapter_event_t error define"
#endif

typedef union {
    void *param_ptr;
    bt_adapter_callback_param_t bt;
#ifdef BT_HFP_SUPPORT
    bt_hf_callback_param_t hf;
#ifdef BT_HFP_AG_ROLE
    bt_ag_callback_param_t ag;
#endif
#endif
#ifdef BT_A2DP_SUPPORT
    bt_a2dp_callback_param_t av;
#endif
#ifdef BT_AVRCP_SUPPORT
    bt_avrcp_callback_param_t ar;
#endif
#if defined(BT_HID_DEVICE) || defined(BT_HID_HOST)
    bt_hid_callback_param_t hid;
#endif
#ifdef BT_MAP_SUPPORT
    bt_map_callback_param_t map;
#endif
#ifdef BT_DIP_SUPPORT
    bt_dip_callback_param_t dip;
#endif
} BT_CALLBACK_PARAM_T;

typedef int (*bt_event_callback_t)(const bt_bdaddr_t *bd_addr, BT_EVENT_T event, BT_CALLBACK_PARAM_T param);

void btif_add_bt_event_callback(bt_event_callback_t cb, uint32_t masks);

bt_status_t btif_me_set_clear_all_filters(void);

bt_status_t btif_me_set_inquiry_no_filter(void);

bt_status_t btif_me_set_inquiry_cod_filter(uint32_t class_of_device_value, uint32_t class_of_device_mask_value);

bt_status_t btif_me_set_inquiry_address_filter(uint32_t bdaddr_high_4_byte, uint32_t bdaddr_low_2_byte);

bt_status_t btif_me_set_connect_no_filter(uint32_t auto_accept_flag);

bt_status_t btif_me_set_connect_cod_filter(uint32_t class_of_device_value, uint32_t class_of_device_mask_value, uint32_t auto_accept_flag);

bt_status_t btif_me_set_connect_address_filter(uint32_t bdaddr_high_4_byte, uint32_t bdaddr_low_2_byte, uint32_t auto_accept_flag);

typedef struct {

    /*  May be any number between 0x0000 and 0xFFFE.  The value is expressed in 0.625 ms
     * increments.
     */
    uint16_t maxLatency;

    /*  any number between 0x0000 and 0xFFFE.  The value is expressed in
     * 0.625 ms increments.
     */
    uint16_t minRemoteTimeout;

    /* Minimum base sniff subrate timeout that the local device may use.
     * May be any number between 0x0000 and 0xFFFE.  The value is expressed in
     * 0.625 ms increments.
     */
    uint16_t minLocalTimeout;
} btif_sniff_subrate_parms_t;

typedef struct {
    struct list_node node;          /* For internal stack use only. */
    btif_callback callback;     /* Pointer to callback function */
    btif_event_mask_t emask;   /* For internal stack use only. */
} btif_handler;

struct conn_handler {
    struct list_node    node;
    struct bdaddr_t     remote;
    bool                use;
    btif_cmgr_callback  callback;
    btif_sniff_info_t   sniff_info;             /*record the sniff infomation               */
    btif_handler        btHandler;
    uint8_t             sniff_timer;            /*record the timer                          */
    uint32              sniff_timeout;          /* Timeout value of the sniff timer         */
    uint32              timer_start_time_tick;  /* record the time tick that sniff timer start  */
};

/* Internal types required for BtRemoteDevice structure */
typedef uint8_t btif_auth_state_t;

typedef uint8_t btif_encrypt_state_t;

typedef uint8_t btif_authorize_state_t;

typedef uint8_t btif_sec_access_state_t;

typedef uint8_t btif_link_rx_state_t;

typedef uint8_t btif_op_type_t;

typedef struct {

    /* Reserved */
    uint8_t flags;

    /* Service Type: 0 = No Traffic, 1 = Best Effort, 2 Guaranteed */
    uint8_t serviceType;

    /* Token Rate in octets per second */
    uint32_t tokenRate;

    /* Peak Bandwidth in octets per second */
    uint32_t peakBandwidth;

    /* Latency in microseconds */
    uint32_t latency;

    /* Delay Variation in microseconds */
    uint32_t delayVariation;
} btif_qos_info_t;

typedef enum {
    BTIF_STOP_IN_PENDING_SUCESS,
    BTIF_STOP_IN_ONGOING_SUCESS,
    BTIF_STOP_ACTIVITY_FAIL,
    BTIF_STOP_INVAILD
} BTIF_BT_ACTIVITY_STOP_E;

typedef struct
{
    U8   psRepMode;
    U8   psMode;
    U16  clockOffset;
} bt_page_scanInfo_t;

struct dbg_send_prefer_rate
{
    uint16_t conhdl;
    uint8_t rate;
};

typedef struct
{
    bt_bdaddr_t bdAddr;           /* Device Address */
    bt_page_scanInfo_t  psi;              /* Page scan info used for connecting */
    U8              psPeriodMode;
    U32     classOfDevice;

    /* RSSI in dBm (-127 to +20). Only valid when controller reports RSSI with
     * in inquiry results (also see ME_SetInquiryMode). Otherwise it will be
     * set to BT_INVALID_RSSI.
     */
    S8              rssi;

    /* Extended Inquiry response.  Only valid when controller reports an
     * extended inquiry (also see ME_SetInquiryMode).  Otherwise it will be
     * set to all 0's.
     */
    U8              extInqResp[240];

    /* Describes the format of the current inquiry result */
    U8   inqMode;

} bt_Inquiry_result_t;

typedef struct
{
    /* Event causing callback. Always valid.*/
    uint8_t   eType;

    /* Error code. See BtEventType for guidance on whether errCode is valid. */
    uint8_t   errCode;

    /* Pointer to handler. Only valid for events directed to BtHandlers. */
    btif_handler   *handler;

    bt_bdaddr_t bdAddr;     /* Device Address */
    uint16_t conn_handle;   /* acl link conn handle */
    void *btm_conn;

    /* Parameters */
    union
    {
        btif_accessible_mode_t   aMode;      /* New access mode */
        void    *meToken;    /* Me command token */
        U8                 pMode;      /* Simple Pairing Mode */

        bt_Inquiry_result_t *inqResult;

        void  *secToken;   /* Security Token */
        void    *token;

        /* Information for BTEVENT_PAIRING_COMPLETE */
        struct
        {
            btif_link_key_type_t   keyType;
        } pairingInfo;

        /* Information for BTEVENT_PIN_REQ */
        struct
        {
            /* If > 0 then the pin length returned in SEC_SetPin must be >= pinLen */
            U8              pinLen;
        } pinReq;

        /* Information for BTEVENT_SET_INQUIRY_MODE_CNF */
        btif_inquiry_mode_t      inqMode;

        /* Information for BTEVENT_SET_INQ_TX_PWR_LVL_CNF */
        S8       inqTxPwr;

        /* Information for BTEVENT_REMOTE_FEATURES */
        struct
        {
            U8      features[8];
        } remoteFeatures;

        /* Information for BTEVENT_REM_HOST_FEATURES */
        struct
        {
            U8      features[8];
        } remHostFeatures;

        /* Information for BTEVENT_LINK_SUPERV_TIMEOUT_CHANGED */
        struct
        {
            U16             timeout;
        } linkSupervision;

        /* Information for BTEVENT_MAX_SLOT_CHANGED */
        struct
        {
            U16             connHandle;
            U8              maxSlot;
        } maxSlotChanged;

        /* Information for BTEVENT_CONN_PACKET_TYPE_CHNG */
        struct
        {
            U16 connHandle;
            U16 packetType;
        } packetTypeChanged;

        /* Information for BTEVENT_QOS_SETUP_COMPLETE */
        struct
        {

            /* Reserved */
            U8 flags;

            /* Service Type: 0 = No Traffic, 1 = Best Effort, 2 Guaranteed */
            U8  serviceType;

            /* Token Rate in octets per second */
            U32 tokenRate;

            /* Peak Bandwidth in octets per second */
            U32 peakBandwidth;

            /* Latency in microseconds */
            U32 latency;

            /* Delay Variation in microseconds */
            U32 delayVariation;
        } qos;

        /* Result for BTEVENT_SET_SNIFF_SUBRATING_PARMS_CNF */
        btif_sniff_subrate_parms_t *sniffSubrateParms;

        /* Information for BTEVENT_SNIFF_SUBRATE_INFO */
        struct
        {
            /* Maximum latency for data being transmitted from the local
             * device to the remote device.
             */
            U16 maxTxLatency;

            /* Maximum latency for data being received by the local
             * device from the remote device.
             */
            U16 maxRxLatency;

            /* The base sniff subrate timeout in baseband slots that the
             * remote device shall use.
             */
            U16 minRemoteTimeout;

            /* The base sniff subrate timeout in baseband slots that the
             * local device will use.
             */
            U16 minLocalTimeout;
        } sniffSubrateInfo;

        /* Information for BTEVENT_CONFIRM_NUMERIC_REQ, BTEVENT_PASS_KEY_REQ,
         * and BTEVENT_DISPLAY_NUMERIC_IND
         */
        struct
        {
            U32             numeric;     /* Numeric value received from Secure
                                          * Simple Pairing (not valid for
                                          * BTEVENT_PASS_KEY_REQ
                                          */

            U8   bondingMode;  /* Bonding has been requested */
        } userIoReq;

        /* Result for BTEVENT_ENCRYPTION_CHANGE event. */
        struct
        {
            U8              mode; /* New encryption mode (uses the
                                   * BtEncryptMode type) */
        } encrypt;

        /* Result for BTEVENT_KEY_PRESSED */
        struct
        {
            U8  parm;   /* The value of the keypress parameter */
        } keyPress;

        struct
        {
            /* If disconnection was successful, contains BEC_NO_ERROR.
             * errCode will contain the disconnect reason.
             *
             * Unsuccessful disconnections will contain an error code
             * as generated by the radio. In this case, errCode can be
             * ignored.
             */
            btif_error_code_t status;
            U8 device_id;
        } disconnect;

        /* Result for the BTEVENT_SCO_DATA_CNF event */
        struct
        {
            void   *scoCon;     /* SCO connection */
            void       *scoPacket;  /* SCO Packet Handled */
        } scoPacketHandled;

        /* Result for the BTEVENT_SCO_CONNECT_CNF && BTEVENT_SCO_CONNECT_IND
           events.
         */
        struct
        {
            U16             scoHandle;  /* SCO Connection handle for HCI */
            void   *scoCon;     /* SCO connection */
            U8      scoLinkType;/* SCO link type */
            U8      device_id;  /* Belong device id */
            U8      interval;
            U8      window;
            void   *scoTxParms; /* Pointer to eSCO TX parameters */
            void   *scoRxParms; /* Pointer to eSCO RX parameters */
        } scoConnect;

        /* Result for the BTEVENT_SCO_DATA_IND event */
        struct
        {
            U16     scoHandle;  /* SCO Connection handle for HCI */
            void   *scoCon;     /* SCO connection. */
            U8              len;        /* SCO data len */
            U8             *ptr;        /* SCO data ptr */
            U8   errFlags;   /* Erroneous Data Reporting */
        } scoDataInd;

        /* Result for the BTEVENT_SECURITY_CHANGE and
         * BTEVENT_SECURITY3_COMPLETE events
         */
        struct
        {
            U8    mode;    /* New security mode (uses the BtSecurityMode
                            * type). */
            BOOL  encrypt; /* Indicate if encryption set or not */
        } secMode;

        /* Results for the BTEVENT_MODE_CHANGE event */
        struct
        {
            uint8_t      curMode;
            U16             interval;
        } modeChange;

        /* Results for BTEVENT_ROLE_CHANGE */
        struct
        {
            uint8_t   newRole;    /* New role */
        } roleChange;

        /* Results for BTEVENT_ACL_DATA_ACTIVE */
        struct
        {
            uint16_t   dataLen;    /* ACL data length */
        } aclDataActive;

        /* Results for BTEVENT_ACL_DATA_NOT_ACTIVE */
        struct
        {
            uint16_t   dataLen;    /* ACL data length */
        } aclDataNotActive;
        struct
        {
            uint8_t status;
            uint16_t name_len;
            const uint8_t *name; // 248 bytes in utf-8
        } name_rsp;
        struct
        {
            uint32_t passkey;
        } userPasskeyNotify;
        struct
        {
            uint8  num_hci_cmd_packets;
            uint16 cmd_opcode;
            const uint8_t  *param;
            uint8  sub_param_len;
        }cmd_cmpl_t;

        struct
        {
            uint8  num_hci_cmd_packets;
            uint16 cmd_opcode;
        }cmd_status_t;

    } edata;
} event_t;

typedef struct
{
    event_t  *evt;
    btif_event_mask_t mask;
} me_event_t;

typedef struct
{
    uint8_t link_id;
    uint32_t timeslice;
}__attribute__((packed)) me_link_env_t;
typedef enum
{
    A2DP_MODE = 0,
    ESCO_MODE,
    CIS_MODE,
    INVALID_MODE = 3, //bt controller rom limited to set 3
    BIS_SCAN_MODE,
} link_traffic_mode_t;

enum btif_le_phy_rate
{
    /// 1 Mbits/s Rate
    LE_RATE_1MBPS   = 0,
    /// 2 Mbits/s Rate
    LE_RATE_2MBPS   = 1,
    /// 125 Kbits/s Rate
    LE_RATE_125KBPS = 2,
    /// 500 Kbits/s Rate
    LE_RATE_500KBPS = 3,
    /// Undefined rate (used for reporting when no packet is received)
    LE_RATE_UNDEF   = 4,

    LE_RATE_MAX     = 4,
};   //must be synced with @enum phy_rate

typedef struct {
    bool (*find_record_device)(const bt_bdaddr_t *bd_addr, btif_device_record_t *rec_dev);
    bool (*link_key_notify)(const bt_bdaddr_t *bd_addr, btif_device_record_t *rec_dev);
    bool (*encrypt_changed)(const bt_bdaddr_t *bd_addr, uint8_t *cod);
} me_nv_operator;

typedef void (*ibrt_disconnect_callback)(const btif_event_t *event);

uint16_t btif_me_get_acl_conn_handle(const bt_bdaddr_t *addr);
void btif_me_set_sniffer_env(uint8_t sniffer_acitve, uint8_t sniffer_role,
                             uint8_t * monitored_addr, uint8_t * sniffer_addr);
BOOL btif_me_get_remote_device_initiator(btif_remote_device_t * rdev);
btif_remote_device_t* btif_me_get_remote_device_by_handle(uint16_t hci_handle);
btif_remote_device_t* btif_me_get_remote_device_by_addr(const bt_bdaddr_t *remote);
btif_remote_device_t* btif_me_get_remote_device_by_bdaddr(bt_bdaddr_t *bdaddr);
btif_connection_role_t btif_me_get_remote_device_role(btif_remote_device_t * rdev);
void btif_me_set_remote_device_role(uint16_t conn_handle, uint8_t role);
BOOL *btif_me_get_remote_device_new_link_key(btif_remote_device_t * rdev);
bool btif_me_role_switch_pending(uint16_t handle);
bool is_btif_me_current_role_bcr_master(btif_remote_device_t * device);
bt_status_t btif_me_inquiry(uint32_t lap, uint8_t len, uint8_t maxResp);

bt_status_t btif_me_ble_read_mesh_list(void);
bt_status_t btif_me_ble_clear_mesh_list(void);
bt_status_t btif_me_ble_add_dev_to_mesh_list(uint8_t addr_type, bt_bdaddr_t * addr);
bt_status_t btif_me_ble_remove_dev_to_mesh_list(uint8_t addr_type, bt_bdaddr_t * addr);
void btif_me_set_mesh_list_callback(void (*cb)(uint16_t opcode, uint8_t status, uint8_t size));

uint8_t btif_sec_set_io_capabilities(uint8_t ioCap);
uint8_t btif_sec_set_authrequirements(uint8_t authRequirements);
uint8_t btif_sec_set_general_bonding(bool general_bonding);
void btif_sec_allow_responder_trigger_auth(bool responder_trigger_auth);
void btif_sec_set_min_enc_key_size(uint8_t min_enc_key_size);
uint8_t btif_me_get_callback_event_type(const btif_event_t * event);
bt_bdaddr_t *btif_me_get_callback_event_address(const btif_event_t *event);
uint16_t btif_me_get_callback_event_handle(const btif_event_t *event);
btif_remote_device_t *btif_me_get_callback_event_rem_dev(const btif_event_t *event);
uint8_t btif_me_get_callback_event_rem_dev_role(const btif_event_t * event);
bt_bdaddr_t *btif_me_get_callback_event_rem_dev_bd_addr(const btif_event_t * event);
bt_bdaddr_t *btif_me_get_callback_event_disconnect_rem_dev_bd_addr(const btif_event_t *
        event);
btif_remote_device_t *btif_me_get_callback_event_disconnect_rem_dev(const btif_event_t *
        event);
btif_remote_device_t *btif_me_get_callback_event_role_change_rem_dev(const btif_event_t *event);
uint8_t btif_me_get_callback_event_disconnect_rem_dev_disc_reason_saved(const btif_event_t *
        event);

bool btif_is_source(uint8_t device_id);
void btif_me_reset_this_device_to_source(btif_remote_device_t *item);

uint8_t btif_me_get_pendCons(void);

uint8_t btif_me_get_source_activeCons(void);

void btif_me_reset_l2cap_sigid(const bt_bdaddr_t *addr);

uint8_t btif_me_get_callback_event_max_slot(const btif_event_t *event);
bool btif_me_get_cmd_status_pkt(const btif_event_t *event, void *out);
bool btif_me_get_cmd_complete_pkt(const btif_event_t *event, void *out);
uint16_t btif_me_get_callback_event_packet_type(const btif_event_t * event);

uint8_t btif_me_get_callback_event_role_change_new_role(const btif_event_t * event);
bt_bdaddr_t *btif_me_get_callback_event_inq_result_bd_addr(const btif_event_t * event);
bt_bdaddr_t *btif_me_get_callback_event_name_rsp_bd_addr(const btif_event_t * event);
uint8_t *btif_me_get_callback_event_inq_result_bd_addr_addr(const btif_event_t * event);
uint8_t btif_me_get_callback_event_inq_result_inq_mode(const btif_event_t * event);
int8_t btif_me_get_callback_event_rssi(const btif_event_t *event);
uint8_t *btif_me_get_callback_event_inq_result_ext_inq_resp(const btif_event_t * event);
uint32_t btif_me_get_callback_event_inq_result_classofdevice(const btif_event_t *event);
uint8_t btif_me_get_callback_event_err_code(const btif_event_t * event);
uint8_t btif_me_get_callback_event_a_mode(const btif_event_t * event);
uint16_t btif_me_get_callback_event_max_slot_changed_connHandle(const btif_event_t * event);
uint8_t btif_me_get_callback_event_max_slot_changed_max_slot(const btif_event_t * event);
uint8_t btif_me_get_callback_event_mode_change_curMode(const btif_event_t * event);
uint16_t btif_me_get_callback_event_mode_change_interval(const btif_event_t * event);

uint16_t btif_me_get_callback_event_remote_dev_name(const btif_event_t * event, const uint8_t** ppName);
uint32_t btif_me_get_callback_event_passkey_notify_passkey(const btif_event_t *event);

bt_status_t btif_me_get_remote_device_name(const bt_bdaddr_t * bdAddr);

uint8_t btif_me_get_ext_inq_data(uint8_t * eir, btif_eir_data_type_t type,
                                 uint8_t * outBuffer, uint8_t Length);
bt_status_t btif_me_cancel_inquiry(void);
void btif_me_set_handler(void *handler, btif_callback cb);
bool btif_me_current_bt_role_is_master(btif_remote_device_t *rem_dev_ptr);
bool btif_me_current_bt_role_is_slave(btif_remote_device_t *rem_dev_ptr);
bool btif_me_is_conn_preferred_as_slave(btif_remote_device_t *rem_dev_ptr);
bool btif_me_is_conn_preferred_as_master(btif_remote_device_t *rem_dev_ptr);
bt_status_t btif_me_disconnect_link(btif_handler * handler,
                                    btif_remote_device_t * rdev);
bt_status_t btif_me_set_link_policy(btif_remote_device_t *rdev, btif_link_policy_t policy);
bt_status_t btif_me_set_link_lowlayer_monitor(btif_remote_device_t * rdev, uint8_t control_flag,uint8_t report_format,
        uint32_t data_format,uint8_t report_unit);
bt_status_t btif_me_set_link_lowlayer_monitor_by_handle(uint16_t connhdl, uint8_t control_flag,uint8_t report_format,
        uint32_t data_format,uint8_t report_unit);
void btif_me_fake_tws_disconnect(uint16_t hci_handle, uint8_t reason);
void btif_me_fake_mobile_disconnect(uint16_t hci_handle, uint8_t reason);
void btif_me_fake_tws_connect(uint8_t status, bt_bdaddr_t * bdAddr);
void btif_me_reset_bt_controller(void);
void btif_me_fake_mobile_connect(uint8_t status, uint16_t hci_handle, bt_bdaddr_t *bdAddr);

bt_status_t btif_me_set_accessible_mode(btif_accessible_mode_t mode,
                                        const btif_access_mode_info_t * info);
bt_status_t btif_me_write_page_timeout(uint16_t timeout);
bt_status_t btif_me_switch_role(uint16_t conn_handle);
void btif_me_set_bt_source_event_handler(btif_handler *handler);
bt_status_t btif_me_register_global_handler(void *handler);
void *btif_me_register_accept_handler(void *handler);
bt_status_t btif_me_set_event_mask(void *handler, btif_event_mask_t mask);
void *btif_me_get_bt_handler(void);
bt_status_t btif_me_set_bt_address(const uint8_t * btAddr);
bt_status_t btif_me_set_ble_bd_address(const uint8_t * btAddr);
bt_status_t btif_me_set_ble_tx_pwr(uint16_t connhdl, int8_t tx_pwr);
bt_status_t btif_me_write_automatic_flush_timeout(uint16 connhandle, uint16 flush_timeout);
bt_status_t btif_enum_device_record(U16 dev_id, btif_device_record_t *record);
bt_bdaddr_t *btif_me_get_remote_device_bdaddr(const btif_remote_device_t * rdev);
btif_rem_dev_state_t btif_me_get_remote_device_state(btif_remote_device_t * rdev);
bool btif_me_get_remote_device_cod(btif_remote_device_t *rdev, uint8_t *cod);
btif_link_mode_t btif_me_get_remote_device_mode(btif_remote_device_t * rdev);
btif_authorize_state_t btif_me_get_remote_device_auth_state(btif_remote_device_t * rdev);
bt_status_t btif_me_write_link_superv_timeout(uint16_t handle, uint16_t slots);
btif_link_mode_t btif_me_get_current_mode(btif_remote_device_t * rdev);
btif_connection_role_t btif_me_get_current_role(btif_remote_device_t * rdev);

bt_status_t btif_me_start_sniff(uint16_t conn_handle, btif_sniff_info_t* info);

bt_status_t btif_me_stop_sniff(uint16_t conn_handle);

bt_status_t btif_me_accept_incoming_link(const bt_bdaddr_t *remote, btif_connection_role_t role);

bt_status_t btif_me_reject_incoming_link(const bt_bdaddr_t *remote, btif_error_code_t reason);

void btif_me_response_acl_conn_req(bt_bdaddr_t *remote, bool accept, uint8_t reason);

bt_status_t btif_me_start_tws_role_switch(uint16_t slaveConnHandle, uint16_t mobileConnHandle);
bt_status_t btif_me_set_sco_tx_silence(uint16_t connHandle, uint8_t silence_on);
bool btif_me_is_sending_data_to_peer_dev_pending(void);
btif_handler *btif_me_get_me_handler(void);
bt_status_t btif_me_force_disconnect_link_with_reason(uint16_t connhdl, uint8_t reason, bool forceDisconnect);
void btif_me_write_bt_sleep_enable(uint8_t sleep_en);
void btif_me_write_bt_page_scan_type(uint8_t scan_type);
void btif_me_write_bt_inquiry_scan_type(uint8_t scan_type);

void btif_me_init_handler(btif_handler * handler);
bt_status_t btif_me_dbg_sniffer_interface(uint16_t connHandle, uint8_t subCode);
uint8_t *btif_me_get_remote_device_version(btif_remote_device_t * rdev);
bt_status_t btif_create_acl_to_slave_with_page_timeout(const bt_bdaddr_t * bdAddr, uint32_t page_timeout, uint32_t time_to_next_page);
bt_status_t btif_create_acl_to_slave(const bt_bdaddr_t * bdAddr);
void btif_me_unregister_globa_handler(btif_handler * handler);
void btif_me_set_inquiry_mode(uint8_t mode);
void btif_me_inquiry_result_setup(uint8_t *inquiry_buff, bool rssi,
                                  bool extended_mode);
btif_remote_device_t *btif_me_enumerate_remote_devices(uint32_t devid);

bool btif_me_is_connection_simple_pairing_completed(const bt_bdaddr_t *remote);

uint8_t btif_me_get_remote_sevice_encrypt_state(btif_remote_device_t* rdev);

uint8_t btif_me_get_remote_device_disc_reason_saved(btif_remote_device_t * device);

void btif_me_register_snoop_acl_connection_callback(void (*conn)(uint8_t device_id, const void* remote, void* btm_conn), void (*disc)(uint8_t device_id, void* remote));

void btif_me_register_pending_too_many_rx_acl_packets_callback(void (*cb)(void));

uint8_t btif_me_get_remote_device_disc_reason(btif_remote_device_t * device);
bool btif_me_wait_acl_complete(void* remote, void (*cb)(uint8_t device_id, void* remote, bool succ, uint8_t errcode));

uint8_t btif_me_get_device_id_from_addr(const bt_bdaddr_t *addr);
uint8_t btif_me_get_device_id_from_rdev(btif_remote_device_t *rdev);
BTIF_BT_ACTIVITY_STOP_E  btif_me_stop_pending_page_activity(const bt_bdaddr_t* addr);
void btif_me_coex_register_page_event_handle(void (*func)(uint8_t is_page));
void btif_register_is_peer_addr_handle(bool (*func)(const uint8_t *addr));

void  btif_me_event_report(me_event_t *event);

void btif_me_set_creating_source_link(const bt_bdaddr_t *remote, bool mark_as_source);
bool btif_me_is_creating_source_link(const void *remote);

void btif_me_set_accepting_source_link(bool set_source_link);
bool btif_me_is_accepting_source_link(const bt_bdaddr_t *remote);

void btif_me_register_is_creating_source_link(bool (*cb)(const void *remote));
void btif_me_mark_as_source_link(btif_remote_device_t *conn);

void btif_me_init_peer_headset_addr(uint8_t *p_remote_addr);
bt_bdaddr_t * btif_me_get_peer_headset_addr(void);
uint8_t btif_me_get_remote_device_link_mode(btif_remote_device_t* rdev);
uint8_t btif_me_get_remote_device_bt_role(btif_remote_device_t* rdev);
bt_status_t btif_me_change_packet_type(btif_remote_device_t *rdev, btif_acl_packet packetTypes);
bt_status_t btif_me_read_controller_memory(uint32_t addr, uint32_t len,uint8_t type);
bt_status_t btif_me_write_controller_memory(uint32_t addr,uint32_t val,uint8_t type);
bool btif_me_is_in_sniff_mode(bt_bdaddr_t *remote);
bool btif_me_is_in_active_mode(bt_bdaddr_t *remote);

void btif_me_set_conn_tws_link(uint16_t conn_handle, uint8_t is_tws_link);
bt_status_t btif_me_bt_dbg_send_prefer_rate(uint16_t conhdl,uint8_t rate);
bt_status_t btif_me_bt_dbg_set_txpwr_link_thd(uint8_t index, uint8_t enable,uint8_t link_id,
    uint16_t rssi_avg_nb_pkt, int8_t rssi_high_thd, int8_t rssi_low_thd, int8_t rssi_below_low_thd, int8_t rssi_interf_thd);
bt_status_t btif_me_read_avg_rssi(uint16_t connhld);

bt_status_t btif_me_qos_set_up(uint16_t conn_handle);
bt_status_t btif_me_qos_setup_with_tpoll(uint16_t conn_handle, uint32_t tpoll_slot);

bt_status_t btif_me_qos_setup_with_tpoll_generic(uint16_t conn_handle, uint32_t tpoll_slot, uint8_t service_type);

void btif_report_bt_event(const bt_bdaddr_t *bd_addr, BT_EVENT_T event, void *param);

/**
 ****************************************************************************************
 * @brief bt set channel classification map which related Set AFH Host Channel Classification command
 * AFH_Host_Channel_Classification: Size: 10 octets (79 bits meaningful)
 * This parameter contains 80 1-bit fields
 * The nth such field (in the range 0 to 78) contains the value for channel n:
 *    0: channel n is bad
 *    1: channel n is unknown
 *
 * The most significant bit (bit 79) is reserved for future use
 * At least (Nmin == 20) channels shall be marked as unknown.
 *
 * default all bits value is 1
 ****************************************************************************************
 */
bt_status_t  btif_me_set_afh_chnl_classification(uint8_t *chnl_map);
bt_status_t btif_me_input_user_passkey(btif_remote_device_t *rdev,uint32_t passkey);
uint32_t btif_me_get_user_passkey(btif_remote_device_t *rdev);
void btif_me_user_passkey_input_timeout_ms(uint32_t timeout_ms);
void btif_me_set_host_pin_code(uint8_t *pin, uint8_t pinlen);
void btif_me_write_host_ssp_mode(uint8_t enable);
void btif_me_set_testmode_enable(uint8_t enable);
uint8_t btif_me_get_testmode_enable(void);
#if mHDT_SUPPORT
bt_status_t btif_me_mhdt_enter_mhdt_mode(const bt_bdaddr_t *addr,uint8 tx_rates,uint8 rx_rates);
bt_status_t btif_me_mhdt_exit_mhdt_mode(const bt_bdaddr_t *addr);
bt_status_t btif_me_mhdt_write_supervision_timeout(btif_remote_device_t *rdev,uint16 mhdt_timeout);
bt_status_t btif_me_mhdt_read_supervision_timeout(btif_remote_device_t *rdev);
bt_status_t btif_me_mhdt_get_bt_data_rate(btif_remote_device_t *rdev,uint8 *tx_rates,uint8 *rx_rates);
btif_mhdt_state_t btif_me_mhdt_get_mhdt_state(btif_remote_device_t *rdev);
#endif
void btif_me_write_dbg_sniffer(const uint8_t subcode, const uint16_t connhandle);
void btif_me_register_get_local_device_callback(void (*cb)(void *remote));
void btif_me_register_notify_save_creadit_callback(void (*cb)(const bt_bdaddr_t *remote));
void btif_me_write_tws_link(uint8_t link_id);
void btif_me_write_btpcm_en(bool en);
bt_remver_t btif_me_get_remote_version_by_handle(uint16_t conn_handle);
uint8_t btif_me_get_remote_class_of_device(btif_remote_device_t *rdev);
bt_status_t btif_dbg_ibrt_update_time_slice(uint8_t nb, me_link_env_t* multi_ibrt);
bt_status_t btif_dbg_set_bt_ble_active_link(link_traffic_mode_t traffic_mode, uint16_t link_handle);
bt_status_t btif_me_enable_ble_audio_dbg_trc_report(bool isEnabled);
bt_status_t btif_dbg_ibrt_switch_relay_mode(uint16_t mobile_conhdl, uint8_t mode);

void btif_me_chip_init_noraml_test_mode_switch(void);

uint8_t btif_me_get_callback_event_encrypt_mode(const btif_event_t * event);
btif_dev_linkkey *btif_me_get_callback_link_key(const btif_event_t *event);
bt_status_t btif_me_auth_req(uint16_t conn_handle);
void btif_me_write_scan_activity_specific(uint16_t opcode, uint16_t scan_interval, uint16_t scan_window);
bt_status_t btif_me_bt_dbg_set_iso_quality_rep_thr(uint16_t conn_handle, uint16_t qlty_rep_evt_cnt_thr,
    uint16_t tx_unack_pkts_thr, uint16_t tx_flush_pkts_thr, uint16_t tx_last_subevent_pkts_thr, uint16_t retrans_pkts_thr,
    uint16_t crc_err_pkts_thr, uint16_t rx_unreceived_pkts_thr, uint16_t duplicate_pkts_thr);
bt_status_t btif_me_bt_dbg_le_tx_power_request(uint16_t conn_handle, uint8_t enable, int8_t delta, enum btif_le_phy_rate rx_rate);
void btif_register_debug_trace_callback(void (*cb)(uint8_t* p_buf, uint16_t buf_len));
void btif_me_register_nv_operator(me_nv_operator *op);
void btif_me_register_bt_calibration_event_cb(void (*cb)(uint16_t opcode, uint8_t *data));

bt_status_t btif_cmgr_set_sniff_timer(btif_cmgr_handler_t * cmgr_handler,
                                      btif_sniff_info_t * SniffInfo, void *sniff_mgr);

btif_sniff_info_t*btif_cmgr_get_cmgrhandler_sniff_info(btif_cmgr_handler_t *cmgr_handler);

bt_status_t btif_cmgr_set_sniff_info_by_handle(uint16_t conn_handle, btif_sniff_info_t * SniffInfo);

uint16_t btif_l2cap_get_psm_by_cid(uint16_t conhdl, bool scid, uint16_t cid);

/**
 ****************************************************************************************
 *    __  __ _____   ___ ____  ____ _____   _____ _   _ _   _  ____
 *   |  \/  | ____| |_ _| __ )|  _ \_   _| |  ___| | | | \ | |/ ___|
 *   | |\/| |  _|    | ||  _ \| |_) || |   | |_  | | | |  \| | |
 *   | |  | | |___   | || |_) |  _ < | |   |  _| | |_| | |\  | |___
 *   |_|  |_|_____| |___|____/|_| \_\|_|   |_|    \___/|_| \_|\____|
 ****************************************************************************************
 */
bt_status_t btif_me_enable_fastack(uint16_t conhdl, uint8_t direction, uint8_t enable);
bt_status_t btif_me_start_ibrt(U16 slaveConnHandle, U16 mobileConnHandle);
bt_status_t btif_me_stop_ibrt(uint16_t mobile_conhdl,uint8_t reason);
bt_status_t btif_me_suspend_ibrt(void);
bt_status_t btif_me_ibrt_mode_init(bool enable);
bt_status_t btif_me_ibrt_role_switch(uint16_t mobile_conhdl);
void btif_me_set_devctx_link(uint8_t acl_array_idx, btif_remote_device_t * rm_dev);
bt_bdaddr_t*  btif_me_get_devctx_btaddr(uint8_t acl_array_idx);
btif_remote_device_t* btif_me_get_remote_device(uint8_t acl_array_idx);
void btif_me_free_tws_outgoing_dev(uint8_t *peer_tws_addr);
btif_remote_device_t*  btif_me_get_devctx_remote_device(uint8_t acl_array_idx);

bt_status_t btif_me_resume_ibrt(uint8_t enable);
void btif_me_ibrt_simu_hci_event_disallow(uint8_t opcode1, uint8_t opcode2);
void btif_me_set_ecc_ibrt_data_test(uint8_t  ecc_data_test_en, uint8_t ecc_data_len, uint16_t ecc_count, uint32_t data_pattern);
void btif_me_send_prefer_rate(uint16_t connhdl, uint8_t rate);
void btif_me_configure_keeping_both_scan(bool isToKeepBothScan);
uint8_t btif_me_get_mobile_link_num(void);
btif_remote_device_t *btif_me_conn_acl_search_by_handle(uint16 conn_handle);
bool btif_me_get_bt_mhdt_btc_remote_feature(const bt_bdaddr_t *remote);
void btif_me_set_bt_mhdt_btc_remote_feature(const bt_bdaddr_t *remote, uint8 remote_support);
#ifdef __cplusplus
}
#endif

#endif /* __ME_API_H__ */
