/***************************************************************************
 *
 * Copyright 2015-2024 BES.
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
 * @brief xxx.
 *
 ****************************************************************************/
#ifndef __BT_STACK_STATUS_H__
#define __BT_STACK_STATUS_H__

#ifdef __cplusplus
extern "C" {
#endif

/****************************** header include ********************************/

/***************************** external declaration *****************************/

/***************************** macro defination *******************************/

/***************************** type defination ********************************/

/***************************** variable defination *****************************/

/***************************** function declaration ****************************/
typedef int bt_status_t;
#define BT_STS_SUCCESS                      0x00   /* Host error code: Successful and complete */
#define BT_STS_FAILED                       0x01   /* Operation failed */
#define BT_STS_PENDING                      0x02   /* Successfully started but pending */
#define BT_STS_DISCONNECT                   0x03   /* Link disconnected */
#define BT_STS_NO_LINK                      0x04   /* No Link layer Connection exists */
#define BT_STS_IN_USE                       0x05   /* Operation failed - already in use. */
#define BT_STS_MEDIA_BUSY                   0x06   /* IrDA specific return codes: Media is busy */
#define BT_STS_MEDIA_NOT_BUSY               0x07   /* IRDA: Media is not busy */
#define BT_STS_NO_PROGRESS                  0x08   /* IRDA: IrLAP not making progress */
#define BT_STS_LINK_OK                      0x09   /* IRDA: No progress condition cleared */
#define BT_STS_SDU_OVERRUN                  0x0a  /* IRDA: Sent more data than current SDU size */
#define BT_STS_BUSY                         0x0b  /* Bluetooth specific return codes */
#define BT_STS_ACL_ALREADY_EXISTS           0x0b
#define BT_STS_NO_RESOURCES                 0x0c
#define BT_STS_NOT_FOUND                    0x0d
#define BT_STS_DEVICE_NOT_FOUND             0x0e
#define BT_STS_CONNECTION_FAILED            0x0f
#define BT_STS_TIMEOUT                      0x10
#define BT_STS_NO_CONNECTION                0x11
#define BT_STS_INVALID_PARM                 0x12
#define BT_STS_IN_PROGRESS                  0x13
#define BT_STS_RESTRICTED                   0x14
#define BT_STS_INVALID_TYPE                 0x15
#define BT_STS_HCI_INIT_ERR                 0x16
#define BT_STS_NOT_SUPPORTED                0x17
#define BT_STS_CONTINUE                     0x18
#define BT_STS_CANCELLED                    0x19
#define BT_STS_NOT_ALLOW                    0x1a
#define BT_STS_ONGOING                      0x1b
#define BT_STS_NOT_BOND                     0x1c
#define BT_STS_NOT_ENCRYPT                  0x1d
#define BT_STS_ERROR_RESULT                 0x1e
#define BT_STS_RECV_ERROR                   0x1f
#define BT_STS_QUEUE_FULL                   0x20
#define BT_STS_INVALID_ADV_SET_ID           0x21
#define BT_STS_INVALID_ADV_HANDLE           0x22
#define BT_STS_INVALID_ADV_INTERVAL         0x23
#define BT_STS_INVALID_PEER_ADDRESS         0x24
#define BT_STS_INVALID_CONN_HANDLE          0x25
#define BT_STS_INVALID_DURATION_VALUE       0x26
#define BT_STS_INVALID_CIS_COUNT            0x27
#define BT_STS_INVALID_CIG_ID               0x28
#define BT_STS_INVALID_CIS_ID               0x29
#define BT_STS_INVALID_CIG_ITEM             0x2A
#define BT_STS_INVALID_STATUS               0x2B
#define BT_STS_INVALID_CIS_HANDLE           0x2C
#define BT_STS_INVALID_PARAMS               0x2D
#define BT_STS_INVALID_BIG_ID               0x2E
#define BT_STS_INVALID_BIG_ITEM             0x2F
#define BT_STS_INVALID_SMP_PHASE            0x30
#define BT_STS_INVALID_METHOD               0x31
#define BT_STS_INVALID_PHASE                0x32
#define BT_STS_INVALID_EVENT                0x33
#define BT_STS_INVALID_ROLE                 0x34
#define BT_STS_INVALID_CALLBACK             0x35
#define BT_STS_INVALID_SERVICE_INSTANCE     0x36
#define BT_STS_INVALID_CHARACTER_INSTANCE   0x37
#define BT_STS_INVALID_CONN_BITFILEDS       0x38
#define BT_STS_INVALID_VALUE                0x39
#define BT_STS_INVALID_PROC_CODE            0x3a
#define BT_STS_INVALID_COUNT                0x3b
#define BT_STS_INVALID_HANDLE               0x3c
#define BT_STS_INVALID_UUID                 0x3d
#define BT_STS_INVALID_PRF_ID               0x3e
#define BT_STS_INVALID_CHAR_HANDLE          0x3f
#define BT_STS_INVALID_INCL_HANDLE          0x40
#define BT_STS_INVALID_SERV_HANDLE          0x41
#define BT_STS_INVALID_PDU_LEN              0x42
#define BT_STS_INVALID_BEARER               0x43
#define BT_STS_INVALID_TIMER_ID             0x44
#define BT_STS_INVALID_RSP_OPCODE           0x44
#define BT_STS_INVALID_REQ_OPCODE           0x45
#define BT_STS_INVALID_OPCODE               0x46
#define BT_STS_INVALID_PROC                 0x47
#define BT_STS_INVALID_MTU                  0x48
#define BT_STS_INVALID_REGISTER             0x49
#define BT_STS_INVALID_VALUE_LEN            0x4a
#define BT_STS_INVALID_DESC_TYPE            0x4b
#define BT_STS_INVALID_DESC_VALUE           0x4c
#define BT_STS_INVALID_CHAR_TYPE            0x4d
#define BT_STS_INVALID_CHAR_VALUE           0x4e
#define BT_STS_INVALID_CHAR_VALUE_TYPE      0x4f
#define BT_STS_INVALID_CHAR_VALUE_VALUE     0x50
#define BT_STS_INVALID_INCL_TYPE            0x51
#define BT_STS_INVALID_INCL_VALUE           0x52
#define BT_STS_INVALID_SERV_TYPE            0x53
#define BT_STS_INVALID_SERV_VALUE           0x54
#define BT_STS_INVALID_ADDRESS              0x55
#define BT_STS_INVALID_STATE                0x56
#define BT_STS_INVALID_HEADER_LEN           0x57
#define BT_STS_INVALID_DEVICE_ID            0x58
#define BT_STS_INVALID_CONN_INDEX           0x59
#define BT_STS_SET_ENABLED_ADV_PARAMS       0x60
#define BT_STS_COBUF_MALLOC_FAILED          0x61
#define BT_STS_PPBUF_MALLOC_FAILED          0x62
#define BT_STS_ALLOC_HCI_CMD_FAILED         0x63
#define BT_STS_HIGH_DUTY_ADV_NOT_SUPPORT    0x64
#define BT_STS_ADV_DATA_TOO_LONG            0x65
#define BT_STS_SCAN_RSP_DATA_TOO_LONG       0x66
#define BT_STS_SET_ADV_PARAMS_FAILED        0x67
#define BT_STS_ENABLE_ADV_FAILED            0x68
#define BT_STS_NOT_IDENTITY_ADDRESS         0x69
#define BT_STS_DISABLE_ADV_FAILED           0x6A
#define BT_STS_RESOUCE_NOT_AVAILABLE        0x6B
#define BT_STS_REACH_MAX_NUMBER             0x6C
#define BT_STS_CIS_ALREADY_OPENED           0x6D
#define BT_STS_TOO_MUCH_BIS_COUNT           0x6E
#define BT_STS_ISO_CONIDX_NOT_FOUND         0x6F
#define BT_STS_SET_ADV_PARAM_FAILED         0x70
#define BT_STS_SCAN_FORCE_CANCELED          0x71
#define BT_STS_SCAN_TIMER_TIMEOUT           0x72
#define BT_STS_ADV_TIMER_TIMEOUT            0x73
#define BT_STS_ADV_CANCEL_AND_CONTINUE      0x74
#define BT_STS_INIT_TIMER_TIMEOUT           0x75
#define BT_STS_INIT_FORCE_CANCELED          0x76
#define BT_STS_INIT_CANCEL_AND_CONTINUE     0x77
#define BT_STS_START_ADV_TIMER_FAILED       0x78
#define BT_STS_START_SCAN_TIMER_FAILED      0x79
#define BT_STS_START_INIT_TIMER_FAILED      0x7A
#define BT_STS_CIS_IS_NOT_CONFIGUABLE       0x7B
#define BT_STS_SEND_CMD_FAILED              0x7C
#define BT_STS_POWER_OFF                    0x7D
#define BT_STS_NO_DEVICE_IN_FILTER_LIST     0x7E
#define BT_STS_ADDRESS_RESO_ENABLE          0x7F
#define BT_STS_RESOLV_DISABLE_ADVTIVITY     0x80
#define BT_STS_INIT_PENDING_WHEN_OP_RESOLV  0x81
#define BT_STS_ADV_ENABLED_WHEN_OP_FILTER   0x82
#define BT_STS_SCAN_ENABLED_WHEN_OP_FILTER  0x83
#define BT_STS_INIT_PENDING_WHEN_OP_FILTER  0x84
#define BT_STS_ADDRESS_ALREADY_IN_LIST      0x85
#define BT_STS_ADDRESS_NOT_IN_THE_LIST      0x86
#define BT_STS_LIST_IS_FULL                 0x87
#define BT_STS_PAIRING_OR_ENC_ONGODING      0x88
#define BT_STS_BREDR_SMP_PAIRING_DISALLOW   0x89
#define BT_STS_SMP_PAIIRNG_NOT_CENTRAL      0x8A
#define BT_STS_PEER_REQUIRE_MITM_PROTECT    0x8B
#define BT_STS_PEER_REQUIRE_SEC_PAIRING     0x8C
#define BT_STS_MITM_PROTECT_REQUIRED        0x8D
#define BT_STS_SECURE_PAIRING_REQUIRED      0x8E
#define BT_STS_SHORT_ENC_KEY_SIZE           0x8F
#define BT_STS_MORE_STRENTH_METHOD_REQUIRED 0x90
#define BT_STS_REQUEST_CENTRAL_ENCRYPT      0x91
#define BT_STS_SECURITY_REQ_NOT_ALLOWED     0x92
#define BT_STS_MTU_IS_NOT_EXCHANGED         0x93
#define BT_STS_GET_TX_BEARER_FAILED         0x94
#define BT_STS_ATT_READ_ERROR               0x95
#define BT_STS_ATT_WRITE_ERROR              0x96
#define BT_STS_DISCOVER_SERVICE_FAILED      0x97
#define BT_STS_DISCOVER_INCLUDE_FAILED      0x98
#define BT_STS_DISCOVER_CHARACTER_FAILED    0x99
#define BT_STS_DISCOVER_DESCRIPTOR_FAILED   0x9A
#define BT_STS_READ_WRITE_PREPARE_FAILED    0x9B
#define BT_STS_READ_VALUE_FAILED            0x9C
#define BT_STS_SEND_DATA_FAILED             0x9D
#define BT_STS_IRK_DISTED_DEVICE_NOT_EXIST  0x9E
#define BT_STS_NOT_RANDOM_STATIC_ADDRESS    0x9F
#define BT_STS_ALLOC_ADVERTISING_FAILED     0xa0
#define BT_STS_PREV_ADV_FORCE_CANCELED      0xa1
#define BT_STS_ADV_CANCELED_AND_RESTART     0xa2
#define BT_STS_ADV_FORCE_DISABLED           0xa3
#define BT_STS_HFP_SCO_ACTIVE               0xa4
#define BT_STS_ADV_STATUS                   0xa5
#define BT_STS_ADV_DATA_STATUS              0xa6
#define BT_STS_SCAN_STATUS                  0xa7
#define BT_STS_INIT_STATUS                  0xa8
#define BT_STS_CONN_STATUS                  0xa9
#define BT_STS_GLOBAL_STATUS                0xaa
#define BT_STS_START_ADV                    0xab
#define BT_STS_START_CONN_ADV               0xac
#define BT_STS_REFRESH_ADV                  0xad
#define BT_STS_SMP_PAIRING                  0xae
#define BT_STS_SMP_ENCRYPTION               0xaf
#define BT_STS_LTK_TO_LINKKEY               0xb0
#define BT_STS_LINKKEY_TO_LTK               0xb1
#define BT_STS_SMP_GEN_LTK                  0xb2
#define BT_STS_SMP_RECV_LTK                 0xb3
#define BT_STS_SMP_REPLY_LTK                0xb4
#define BT_STS_FREE_CIG_CIS                 0xb5
#define BT_STS_FREE_BIG_BIS                 0xb6
#define BT_STS_NEW_CIG_CIS                  0xb7
#define BT_STS_REAL_LINK_DISCONNECTED       0xb8 // dont change
#define BT_STS_BT_CANCEL_PAGE               0xb9 // dont change
#define BT_STS_BT_DISCONNECT_ITSELF         0xba // dont change
#define BT_STS_BT_IBRT_SLAVE_CLEANUP        0xbb // dont change
#define BT_STS_PAST_RECEIVED                0xbc
#define BT_STS_TERM_SYNC_STATUS             0xbd
#define BT_STS_CREATE_SYNC_STATUS           0xbe
#define BT_STS_PA_SYNC_LOST                 0xbf
#define BT_STS_PA_SYNC_ESTABLISH            0xc0
#define BT_STS_DATA_LENGTH_UPDATE           0xc1
#define BT_STS_RECV_L2CAP_PARAM_UPDATE_RSP  0xc2
#define BT_STS_CONN_ESTABLISHED             0xc3
#define BT_STS_CREATE_LE_CONN               0xc4
#define BT_STS_START_LE_SCAN                0xc5
#define BT_STS_READ_LE_FEATURE              0xc6
#define BT_STS_MAX_ADV_DATA_LENGTH          0xc7
#define BT_STS_NOT_READY                    0xc8 // 200 dont change
#define BT_STS_QUERY_FAILED                 0xc9 // 201 dont change
#define BT_STS_ALREADY_EXIST                0xca // 202 dont change
#define BT_STS_MAX_ADV_SET_NUM              0xcb
#define BT_STS_MAX_FILTER_LIST_SIZE         0xcc
#define BT_STS_MAX_RESOLVING_LIST_SIZE      0xcd
#define BT_STS_BR_START_CTKD                0xce
#define BT_STS_LE_START_CTKD                0xcf
#define BT_STS_SET_KEY_STRENGTH             0xd0
#define BT_STS_SET_SECURITY_LEVEL           0xd1
#define BT_STS_LOCAL_CSRK                   0xd2
#define BT_STS_LOCAL_IRK                    0xd3
#define BT_STS_PEER_IDENTITY                0xd4
#define BT_STS_LEGACY_EDIV_LTK              0xd5
#define BT_STS_GAP_GEN_LRPA                 0xd6
#define BT_STS_GATT_CLIENT_SUPP_FEATURE     0xd7
#define BT_STS_GATT_SERVER_SUPP_FEATURE     0xd8
#define BT_STS_GATT_DEVICE_NAME             0xd9
#define BT_STS_GATT_APPEARANCE              0xda
#define BT_STS_GATT_CENTRAL_ADDR_RESOLUTION 0xdb
#define BT_STS_ONLY_USE_RPA_AFTER_BONDING   0xdc
#define BT_STS_PERIPH_PERFERRED_CONN_PARAMS 0xdd
#define BT_STS_GATT_PEER_SEC_LEVEL          0xde
#define BT_STS_GATT_DATABASE_HASH_VALUE     0xdf
#define BT_STS_CHARACTER_NOT_REGISTERED     0xe0
#define BT_STS_LENGTH_TOO_LONG              0xe1
#define BT_STS_BEARER_NOT_FOUND             0xe2
#define BT_STS_ALLOC_FAILED                 0xe3
#define BT_STS_PREPARE_WRITE_NOT_MATCH      0xe4
#define BT_STS_REACH_UPPER_LIMIT            0xe5
#define BT_STS_REACH_LOWER_LIMIT            0xe6
#define BT_STS_COUNT_NOT_MATCH              0xe7
#define BT_STS_HANDLE_NOT_IN_ORDER          0xe8
#define BT_STS_SERVICE_NOT_MATCH            0xe9
#define BT_STS_GATT_ADD_INCLUDE             0xea
#define BT_STS_GATT_ADD_SERVICE             0xeb
#define BT_STS_ALREADY_REGISTERED           0xec
#define BT_STS_PRF_REGISTER                 0xed
#define BT_STS_SVC_REGISTER                 0xee
#define BT_STS_CANT_UNREGISTER              0xef
#define BT_STS_RECV_REQ                     0xf0
#define BT_STS_RECV_RSP                     0xf1
#define BT_STS_NOT_SUPP_MULTI_CONN          0xf2
#define BT_STS_OPEN_PROFILE                 0xf3
#define BT_STS_OPEN_SERVICE                 0xf4
#define BT_STS_CLOSE_PROFILE                0xf3
#define BT_STS_CLOSE_SERVICE                0xf4
#define BT_STS_GATT_CONN_READY              0xf5
#define BT_STS_MTU_EXCHANGED                0xf6
#define BT_STS_START_TIMER_FAILED           0xf7
#define BT_STS_ATT_BEARER                   0xf8
#define BT_STS_SEND_REQ                     0xf9
#define BT_STS_SEND_RSP                     0xfa
#define BT_STS_PRF_UNREGISTER               0xfb
#define BT_STS_SVC_UNREGISTER               0xfc
#define BT_STS_GATT_PROC                    0xfd
#define BT_STS_RECV_ERROR_RSP               0xfe
#define BT_STS_SEND_ERROR_RSP               0xff
#define BT_STS_REGISTER_NOTIFY              0x100
#define BT_STS_REGISTER_NOT_FOUND           0x101
#define BT_STS_GATT_SERVICE                 0x102
#define BT_STS_GATT_INCLUDE                 0x103
#define BT_STS_GATT_CHARACTER               0x104
#define BT_STS_GATT_DESCRIPTOR              0x105
#define BT_STS_SERVICE_NOT_FOUND            0x106
#define BT_STS_CHARACTER_NOT_FOUND          0x107
#define BT_STS_SERVICE_EVENT                0x108
#define BT_STS_SECURITY_KEY                 0x109
#define BT_STS_EMPTY_PPB                    0x10a
#define BT_STS_REALLOC_PPB_FAILED           0x10b
#define BT_STS_PPB_PUT_INVALID_DATA         0x10c
#define BT_STS_PPB_PUT_DATA_TOO_LONG        0x10d
#define BT_STS_PPB_FREE_NOT_ALLOWED         0x10e
#define BT_STS_COBUF_INVALID_NEXT_BLOCK     0x10f
#define BT_STS_COBUF_INVALID_HANDLE         0x110
#define BT_STS_COBUF_INVALID_ALLOC_SIZE     0x111
#define BT_STS_COBUF_NULL_BUFF_PTR          0x112
#define BT_STS_COBUF_ALLOC_FAILED           0x113
#define BT_STS_COBUF_ALLOC_PEAK_SIZE        0x114
#define BT_STS_ACCEPT_CONN_UPDATE_REQ       0x115
#define BT_STS_SEND_CONN_UPDATE_REQ         0x116
#define BT_STS_RESOLVE_RPA_FAILED           0x117
#define BT_STS_DEVICE_NOT_PAIRED            0x118
#define BT_STS_SET_FILTER_LIST              0x119
#define BT_STS_OWN_ADDRESS_TYPE             0x11a
#define BT_STS_DIFFERENT_BLE_IDENTITY_ADDR  0x11b
#define BT_STS_RECORD_NOT_FOUND             0x11c
#define BT_STS_REFRESH_ADV_STATE            0x11d
#define BT_STS_NEW_BIG_BIS                  0x11e
#define BT_STS_BLE_ONLY_ENABLED             0x11f
#define BT_STS_CENTRAL_ROLE_NOT_SUPPORT     0x120
#define BT_STS_NOT_REGISTERED               0x121
#define BT_STS_CHANNEL_STATUS               0x122
#define BT_STS_INVALID_GFPS                 0x123
#define BT_STS_SEC_IO_CAP                   0x124
#define BT_STS_SEC_AUTH_REQUIRE             0x125
#define BT_STS_MAX_PA_LIST_SIZE             0x126
#define BT_STS_NOT_SUPPORT                  0x127
#define BT_STS_RX_BLOCK_AUTH_FAIL           0x128
#define BT_STS_TX_BLOCK_AUTH_FAIL           0x129
#define BT_STS_EATT_BLOCK_AUTH_FAIL         0x12a
#define BT_STS_NOT_AUTHENTICATED            0x12b
#define BT_STS_NOT_AUTHORIZED               0x12c
#define BT_STS_NOT_ENCRYPTED                0x12d
#define BT_STS_TERMINATE                    0x12e
#define BT_STS_NO_BONDED_DEVICE             0x12f
#define BT_STS_TOO_MANY_ACTIVE_CCCD         0x130
#define BT_STS_PEER_SERVICE_CHANGED         0x131
#define BT_STS_DATABASE_HASH                0x132
#define BT_STS_GEN_DATABASE_HASH_FAILED     0x133
#define BT_STS_SERVICE_CHANGED              0x134
#define BT_STS_SMP_AUTHENCIATION            0x135
#define BT_STS_INVALID_BIG_COUNT            0x136
#define BT_STS_INVALID_BIS_INDEX            0x137
#define BT_STS_INVALID_ISO_CONIDX           0x138
#define BT_STS_INVALID_ISO_HANDLE           0x139
#define BT_STS_INVALID_CIG_OP_ID            0x13a
#define BT_STS_INVALID_CONN_TYPE            0x13b
#define BT_STS_INVALID_SMP_CONN             0x13c
#define BT_STS_INVALID_UUID_LEN             0x13d
#define BT_STS_INVALID_LENGTH               0x13e
#define BT_STS_INVALID_GATT_SERVICE         0x13f
#define BT_STS_INVALID_GATT_INCLUDE         0x140
#define BT_STS_INVALID_GATT_CHARACTER       0x141
#define BT_STS_INVALID_GATT_DESCRIPTOR      0x142
#define BT_STS_INVALID_BDADDR               0x143
#define BT_STS_INVALID_LOCAL_ADDRESS        0x144
#define BT_STS_INVALID_ADV_DATA             0x145
#define BT_STS_ALREADY_DONE_OR_ERROR        0x146
#define BT_STS_DISABLE_ADVERTISING          0x147
#define BT_STS_CONN_ADV_PARAM_NOT_FOUND     0x148
#define BT_STS_BTGATT_PROFILE_DATA_SAVE     0x149
#define BT_STS_BTGATT_PROFILE_DATA_RESTORE  0x14A
#define BT_STS_CANCEL_INITIATING            0x14B
#define BT_STS_RESO_LIST_ADD_ALL            0x14C
#define BT_STS_INVALID_CHANNEL_ID           0x14D
#define BT_STS_INVALID_PEER_ID              0x14E
#define BT_STS_INVALID_CHANNEL              0x14F
#define BT_STS_INVALID_CONNECTION           0x150
#define BT_STS_INVALID_IDENTIFIER           0x151
#define BT_STS_INVALID_FLAG                 0x152
#define BT_STS_INVALID_ATTR_HANDLE          0x153
#define BT_STS_ADD_FAILED                   0x154
#define BT_STS_CURR_PROC_WAIT_MORE_PDU      0x155
#define BT_STS_NOT_IN_THE_SAME_SERVICE      0x156
#define BT_STS_NOT_SAME_TYPE                0x157
#define BT_STS_ACTIVE_STATUS                0x158
#define BT_STS_NO_CLIENT_NOTIFIED           0x159
#define BT_STS_AUTHOR_REQ_FAILED            0x15A
#define BT_STS_READ_REQ_FAILED              0x15B
#define BT_STS_WRITE_REQ_FAILED             0x15C
#define BT_STS_CALL_METHOD_FAILED           0x15D
#define BT_STS_OUTSTANDING_CMD_MISMATCH     0x15E
#define BT_STS_OUTSTANDING_CMD_IS_NULL      0x15F
#define BT_STS_SLE_ADDRESS                  0x160
#define BT_STS_CREATE_FAILED                0x161
#define BT_STS_CANCEL_FAILED                0x162
#define BT_STS_WRITE_WORKING_STATE          0x163
#define BT_STS_WRITE_REPORT_INFO            0x164
#define BT_STS_NEAR_SERVICE                 0x165
#define BT_STS_NEAR_PROPERTY                0x166
#define BT_STS_PROPERTY_NOT_FOUND           0x167
#define BT_STS_START_DISCOVER               0x168
#define BT_STS_REPORT_NOT_FOUND             0x169
#define BT_STS_NEAR_HID_REPORT              0x16A
#define BT_STS_INVALID_DESCRIPTOR           0x16B
#define BT_STS_RECV_REPORT                  0x16C
#define BT_STS_RECV_SERVICE                 0x16D
#define BT_STS_HID_DESCRIPTOR               0x16E
#define BT_STS_WORKING_STATE                0x16F
#define BT_STS_REPORT_INFO                  0x170
#define BT_STS_READ_REQ                     0x171
#define BT_STS_WRITE_REQ                    0x172
#define BT_STS_WRITE_RSP                    0x173
#define BT_STS_GATT_DEFER_RSP               0x174
#define BT_STS_INVALID_DEFER_CODE           0x175
#define BT_STS_SET_DECISION_DATA            0x176
#define BT_STS_DECISION_RESOLVABLE_TAG      0x177
#define BT_STS_SET_DECISION_INSTRUCTIONS    0x178
#define BT_STS_ADD_ADVS_TO_MONITORED_LIST   0x179
#define BT_STS_RM_ADVS_TO_MONITORED_LIST    0x17A
#define BT_STS_CLR_ADVS_TO_MONITORED_LIST   0x17B
#define BT_STS_EN_MONITORING_ADVERTISERS    0x17C
#define BT_STS_SRV_USER_REGISTER            0x17D
#define BT_STS_SRV_USER_UNREGISTER          0x17E
#define BT_STS_BAP_CIS_UPDATE               0x17F

/*BT Link layer Error*/
#define BT_LL_ERR_NO_ERROR                  0x00
#define BT_LL_ERR_UNKNOWN_HCI_COMMAND       0x01
#define BT_LL_ERR_UNKNOWN_CONNECTION_ID     0x02
#define BT_LL_ERR_HARDWARE_FAILURE          0x03
#define BT_LL_ERR_PAGE_TIMEOUT              0x04
#define BT_LL_ERR_AUTH_FAILURE              0x05
#define BT_LL_ERR_PIN_MISSING               0x06
#define BT_LL_ERR_MEMORY_CAPA_EXCEED        0x07
#define BT_LL_ERR_CON_TIMEOUT               0x08
#define BT_LL_ERR_CON_LIMIT_EXCEED          0x09
#define BT_LL_ERR_SYNC_CON_LIMIT_DEV_EXCEED 0x0A
#define BT_LL_ERR_CON_ALREADY_EXISTS        0x0B
#define BT_LL_ERR_COMMAND_DISALLOWED        0x0C
#define BT_LL_ERR_CON_REJ_LIMITED_RES       0x0D
#define BT_LL_ERR_CON_REJ_SECURITY_REASONS  0x0E
#define BT_LL_ERR_CON_REJ_UNACCEPT_BDADDR   0x0F
#define BT_LL_ERR_CON_ACCEPT_TIMEOUT_EXCEED 0x10
#define BT_LL_ERR_UNSUPPORTED               0x11
#define BT_LL_ERR_INVALID_HCI_PARAM         0x12
#define BT_LL_ERR_REMOTE_USER_TERM_CON      0x13
#define BT_LL_ERR_REMOTE_DEV_TERM_LOW_RES   0x14
#define BT_LL_ERR_REMOTE_DEV_POWER_OFF      0x15
#define BT_LL_ERR_CON_TERM_BY_LOCAL_HOST    0x16
#define BT_LL_ERR_REPEATED_ATTEMPTS         0x17
#define BT_LL_ERR_PAIRING_NOT_ALLOWED       0x18
#define BT_LL_ERR_UNKNOWN_LMP_PDU           0x19
#define BT_LL_ERR_UNSUPPORTED_REMOTE_FEAT   0x1A
#define BT_LL_ERR_SCO_OFFSET_REJECTED       0x1B
#define BT_LL_ERR_SCO_INTERVAL_REJECTED     0x1C
#define BT_LL_ERR_SCO_AIR_MODE_REJECTED     0x1D
#define BT_LL_ERR_INVALID_LMP_PARAM         0x1E
#define BT_LL_ERR_UNSPECIFIED_ERROR         0x1F
#define BT_LL_ERR_UNSUPPORTED_LMP_PARAM     0x20
#define BT_LL_ERR_ROLE_CHANGE_NOT_ALLOWED   0x21
#define BT_LL_ERR_LMP_RSP_TIMEOUT           0x22
#define BT_LL_ERR_LMP_COLLISION             0x23
#define BT_LL_ERR_LMP_PDU_NOT_ALLOWED       0x24
#define BT_LL_ERR_ENC_MODE_NOT_ACCEPT       0x25
#define BT_LL_ERR_LINK_KEY_CANT_CHANGE      0x26
#define BT_LL_ERR_QOS_NOT_SUPPORTED         0x27
#define BT_LL_ERR_INSTANT_PASSED            0x28
#define BT_LL_ERR_PAIR_UNIT_KEY_NOT_SUP     0x29
#define BT_LL_ERR_DIFF_TRANS_COLLISION      0x2A
#define BT_LL_ERR_QOS_UNACCEPTABLE_PARAM    0x2C
#define BT_LL_ERR_QOS_REJECTED              0x2D
#define BT_LL_ERR_CHANNEL_CLASS_NOT_SUP     0x2E
#define BT_LL_ERR_INSUFFICIENT_SECURITY     0x2F
#define BT_LL_ERR_PARAM_OUT_OF_MAND_RANGE   0x30
#define BT_LL_ERR_ROLE_SWITCH_PEND          0x32 /* LM_ROLE_SWITCH_PENDING               */
#define BT_LL_ERR_RESERVED_SLOT_VIOLATION   0x34 /* LM_RESERVED_SLOT_VIOLATION           */
#define BT_LL_ERR_ROLE_SWITCH_FAIL          0x35 /* LM_ROLE_SWITCH_FAILED                */
#define BT_LL_ERR_EIR_TOO_LARGE             0x36 /* LM_EXTENDED_INQUIRY_RESPONSE_TOO_LARGE */
#define BT_LL_ERR_SP_NOT_SUPPORTED_HOST     0x37
#define BT_LL_ERR_HOST_BUSY_PAIRING         0x38
#define BT_LL_ERR_CONTROLLER_BUSY           0x3A
#define BT_LL_ERR_UNACCEPTABLE_CON_PARAM    0x3B
#define BT_LL_ERR_ADV_TIMEOUT               0x3C
#define BT_LL_ERR_TERMINATED_MIC_FAILURE    0x3D
#define BT_LL_ERR_CON_FAILED_TO_BE_EST      0x3E
#define BT_LL_ERR_CCA_REJ_USE_CLOCK_DRAG    0x40
#define BT_LL_ERR_TYPE0_SUBMAP_NOT_DEFINED  0x41
#define BT_LL_ERR_UNKNOWN_ADVERTISING_ID    0x42
#define BT_LL_ERR_LIMIT_REACHED             0x43
#define BT_LL_ERR_OP_CANCELED_BY_HOST       0x44
#define BT_LL_ERR_PKT_TOO_LONG              0x45

typedef enum {
    NEAR_STS_SUCCESS                      = 0x00,
    NEAR_STS_FAILED                       ,
    NEAR_STS_PENDING                      ,
    NEAR_STS_ONGOING                      ,
    NEAR_STS_IN_PROGRESS                  ,
    NEAR_STS_IN_USE                       ,
    NEAR_STS_IS_EMPTY                     ,
    NEAR_STS_IS_FULL                      ,
    NEAR_STS_IS_BUSY                      ,
    NEAR_STS_POWER_OFF                    ,
    NEAR_STS_LIST_FULL                    ,
    NEAR_STS_QUEUE_FULL                   ,
    NEAR_STS_ERROR_RESULT                 ,
    NEAR_STS_ERROR_STATUS                 ,
    NEAR_STS_RECV_ERROR                   ,
    NEAR_STS_START_FAILED                 ,
    NEAR_STS_STOP_FAILED                  ,
    NEAR_STS_ENABLE_FAILED                ,
    NEAR_STS_DISABLE_FAILED               ,
    NEAR_STS_READ_FAILED                  ,
    NEAR_STS_WRITE_FAILED                 ,
    NEAR_STS_CONFIG_FAILED                ,
    NEAR_STS_SET_FAILED                   ,
    NEAR_STS_ADD_FAILED                   ,
    NEAR_STS_CONN_FAILED                  ,
    NEAR_STS_SEND_FAILED                  ,
    NEAR_STS_SEND_DATA_FAILED             ,
    NEAR_STS_SEND_CMD_FAILED              ,
    NEAR_STS_CREATE_FAILED                ,
    NEAR_STS_CANCEL_FAILED                ,
    NEAR_STS_REGISTER_FAILED              ,
    NEAR_STS_AUTHOR_REQ_FAILED            ,
    NEAR_STS_READ_REQ_FAILED              ,
    NEAR_STS_WRITE_REQ_FAILED             ,
    NEAR_STS_CALL_METHOD_FAILED           ,
    NEAR_STS_APP_INIT                     ,
    NEAR_STS_INITIALIZE                   ,
    NEAR_STS_APP_READY                    ,
    NEAR_STS_STACK_READY                  ,
    NEAR_STS_CANCELLED                    ,
    NEAR_STS_RESTRICTED                   ,
    NEAR_STS_CONNECT                      ,
    NEAR_STS_DISCONNECT                   ,
    NEAR_STS_TERMINATE                    ,
    NEAR_STS_REGISTER                     ,
    NEAR_STS_UNREGISTER                   ,
    NEAR_STS_CANCEL                       ,
    NEAR_STS_DISCOVER                     ,
    NEAR_STS_START_DISCOVER               ,
    NEAR_STS_CONN_OPENED                  ,
    NEAR_STS_CONN_CLOSED                  ,
    NEAR_STS_CHANNEL_OPENED               ,
    NEAR_STS_CHANNEL_CLOSED               ,
    NEAR_STS_INITIATING                   ,
    NEAR_STS_SCANNING                     ,
    NEAR_STS_START_SCAN                   ,
    NEAR_STS_STOP_SCAN                    ,
    NEAR_STS_BROADCAST                    ,
    NEAR_STS_START_BROADCAST              ,
    NEAR_STS_STOP_BROADCAST               ,
    NEAR_STS_TIMEOUT                      ,
    NEAR_STS_PROC_TIMEOUT                 ,
    NEAR_STS_MULTIPLE_INSTANCE            ,
    NEAR_STS_PEER_MULTIPLE_INSTANCE       ,
    NEAR_STS_NO_LINK                      ,
    NEAR_STS_NO_CONNECTION                ,
    NEAR_STS_NO_RESOURCES                 ,
    NEAR_STS_NO_BOND_DEVICE               ,
    NEAR_STS_NO_CLIENT_NOTIFIED           ,
    NEAR_STS_NOT_AUTHENTICATED            ,
    NEAR_STS_NOT_AUTHORIZED               ,
    NEAR_STS_NOT_ENCRYPTED                ,
    NEAR_STS_NOT_READY                    ,
    NEAR_STS_NOT_OPEN                     ,
    NEAR_STS_NOT_ALLOW                    ,
    NEAR_STS_NOT_AVAILABLE                ,
    NEAR_STS_NOT_IN_LIST                  ,
    NEAR_STS_NOT_IN_ORDER                 ,
    NEAR_STS_NOT_MATCH                    ,
    NEAR_STS_NOT_SAME_TYPE                ,
    NEAR_STS_NOT_IN_SAME_SERVICE          ,
    NEAR_STS_NOT_SUPPORT                  ,
    NEAR_STS_NOT_FOUND                    ,
    NEAR_STS_CONN_NOT_FOUND               ,
    NEAR_STS_LINK_NOT_FOUND               ,
    NEAR_STS_CHANNEL_NOT_FOUND            ,
    NEAR_STS_RECROD_NOT_FOUND             ,
    NEAR_STS_DEVICE_NOT_FOUND             ,
    NEAR_STS_SERVICE_NOT_FOUND            ,
    NEAR_STS_PROPERTY_NOT_FOUND           ,
    NEAR_STS_REPORT_NOT_FOUND             ,
    NEAR_STS_USE_EXIST_CONN               ,
    NEAR_STS_CONN_ALREADY_EXIST           ,
    NEAR_STS_ALREADY_EXIST                ,
    NEAR_STS_ALREADY_OPEN                 ,
    NEAR_STS_ALREADY_IN_LIST              ,
    NEAR_STS_ALREADY_REGISTERED           ,
    NEAR_STS_INVALID_PARM                 ,
    NEAR_STS_INVALID_TYPE                 ,
    NEAR_STS_INVALID_ID                   ,
    NEAR_STS_INVALID_INSTANCE             ,
    NEAR_STS_INVALID_INSTANCE_ID          ,
    NEAR_STS_INVALID_IDENTIFIER           ,
    NEAR_STS_INVALID_PEER_ID              ,
    NEAR_STS_INVALID_CHANNEL_ID           ,
    NEAR_STS_INVALID_TCID                 ,
    NEAR_STS_INVALID_OPID                 ,
    NEAR_STS_INVALID_UUID                 ,
    NEAR_STS_INVALID_UUID_LEN             ,
    NEAR_STS_INVALID_VALUE                ,
    NEAR_STS_INVALID_LENGTH               ,
    NEAR_STS_INVALID_SIZE                 ,
    NEAR_STS_INVALID_DATA                 ,
    NEAR_STS_INVALID_PROC                 ,
    NEAR_STS_INVALID_OPCODE               ,
    NEAR_STS_INVALID_MTU                  ,
    NEAR_STS_INVALID_PDU                  ,
    NEAR_STS_INVALID_FRAME                ,
    NEAR_STS_INVALID_HEADER               ,
    NEAR_STS_INVALID_CONN                 ,
    NEAR_STS_INVALID_CHANNEL              ,
    NEAR_STS_INVALID_CONIDX               ,
    NEAR_STS_INVALID_INDEX                ,
    NEAR_STS_INVALID_HANDLE               ,
    NEAR_STS_INVALID_CONN_HANDLE          ,
    NEAR_STS_INVALID_ATTR_HANDLE          ,
    NEAR_STS_INVALID_SERVICE_HANDLE       ,
    NEAR_STS_INVALID_PROPERTY_HANDLE      ,
    NEAR_STS_INVALID_INTERVAL             ,
    NEAR_STS_INVALID_WINDOW               ,
    NEAR_STS_INVALID_DURATION             ,
    NEAR_STS_INVALID_PERIOD               ,
    NEAR_STS_INVALID_TIMEOUT              ,
    NEAR_STS_INVALID_PEER_TYPE            ,
    NEAR_STS_INVALID_ADDR_TYPE            ,
    NEAR_STS_INVALID_ADDRESS              ,
    NEAR_STS_INVALID_PEER_ADDRESS         ,
    NEAR_STS_INVALID_COUNT                ,
    NEAR_STS_INVALID_ITEM                 ,
    NEAR_STS_INVALID_FLAG                 ,
    NEAR_STS_INVALID_STATE                ,
    NEAR_STS_INVALID_STATUS               ,
    NEAR_STS_INVALID_METHOD               ,
    NEAR_STS_INVALID_PHASE                ,
    NEAR_STS_INVALID_EVENT                ,
    NEAR_STS_INVALID_ROLE                 ,
    NEAR_STS_INVALID_CALLBACK             ,
    NEAR_STS_INVALID_SERVICE              ,
    NEAR_STS_INVALID_PROPERTY             ,
    NEAR_STS_INVALID_SERVICE_ATTR         ,
    NEAR_STS_INVALID_PROPERTY_ATTR        ,
    NEAR_STS_INVALID_REPORT               ,
    NEAR_STS_INVALID_DESCRIPOTR           ,
    NEAR_STS_INVALID_BITFILED             ,
    NEAR_STS_ALLOC_FAILED                 ,
    NEAR_STS_ALLOC_PPB_FAILED             ,
    NEAR_STS_ALLOC_HCI_FAILED             ,
    NEAR_STS_REACH_MAX_SIZE               ,
    NEAR_STS_REACH_MAX_COUNT              ,
    NEAR_STS_REACH_MAX_NUMBER             ,
    NEAR_STS_LENGTH_TOO_LONG              ,
    NEAR_STS_SIZE_TOO_LARGE               ,
    NEAR_STS_DATA_TOO_LONG                ,
    NEAR_STS_VALUE_TOO_LARGE              ,
    NEAR_STS_REACH_UPPER_LIMIT            ,
    NEAR_STS_REACH_LOWER_LIMIT            ,
    NEAR_STS_OVERFLOW                     ,
    NEAR_STS_UNDERFLOW                    ,
    NEAR_STS_WAIT_MORE_PDU                ,
    NEAR_STS_RECV_REQ                     ,
    NEAR_STS_RECV_RSP                     ,
    NEAR_STS_RECV_SERVICE                 ,
    NEAR_STS_RECV_PROPERTY                ,
    NEAR_STS_RECV_REPORT                  ,
    NEAR_STS_SEND_REQ                     ,
    NEAR_STS_SEND_RSP                     ,
    NEAR_STS_OUTSTANDING_CMD_MISMATCH     ,
    NEAR_STS_OUTSTANDING_CMD_IS_NULL      ,
    NEAR_STS_SLE_ADDRESS                  ,
    NEAR_STS_WRITE_WORKING_STATE          ,
    NEAR_STS_WRITE_REPORT_INFO            ,
    NEAR_STS_SERVICE                      ,
    NEAR_STS_PROPERTY                     ,
    NEAR_STS_PEER_SERVICE                 ,
    NEAR_STS_PEER_PROPERTY                ,
    NEAR_STS_HID_REPORT                   ,
    NEAR_STS_HID_DESCRIPTOR               ,
    NEAR_STS_WORKING_STATE                ,
    NEAR_STS_REPORT_INFO                  ,
    NEAR_STS_DEVICE_ADDRESS               ,
    NEAR_STS_LOCAL_ADDRESS                ,
    NEAR_STS_PEER_ADDRESS                 ,
} near_status_t;


#ifdef __cplusplus
}
#endif

#endif /* __BT_STACK_STATUS_H__ */