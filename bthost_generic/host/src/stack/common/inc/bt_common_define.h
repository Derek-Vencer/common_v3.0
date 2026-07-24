/***************************************************************************
 *
 * Copyright 2015-2022 BES.
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
#ifndef __BT_COMMON_DEFINE_H__
#define __BT_COMMON_DEFINE_H__
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "cmsis.h"
#include "hal_aud.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "bt_sys_config.h"
#include "bt_dispatch.h"
#include "co_list.h"
#include "bt_common_addr.h"

// TODO:

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Baisc Type
 */

typedef unsigned char  byte;
typedef unsigned char  uint8;
typedef unsigned char  uint8_t;
typedef signed   char  int8;
typedef unsigned short uint16;
typedef signed   short int16;
typedef unsigned int   uint32;
typedef signed   int   int32;

#ifndef BOOL_DEFINED
typedef unsigned int BOOL;
#endif

#ifndef __U32_TYPE
#define __U32_TYPE
typedef unsigned int U32;
#endif

#ifndef __U16_TYPE
#define __U16_TYPE
typedef unsigned short U16;
#endif

#ifndef __U8_TYPE
#define __U8_TYPE
typedef unsigned char U8;
#endif

typedef int S32;
typedef short S16;
typedef char S8;

typedef uint8_t UINT8;
typedef uint16_t UINT16;
typedef uint32_t UINT32;

#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TimeT
typedef U32 TimeT;
#endif

#ifndef SUCCESS
#define SUCCESS 0
#endif

#ifndef FAILURE
#define FAILURE 1
#endif

#ifndef INPROGRESS
#define INPROGRESS 2
#endif

#define OUTOFMEMORY 3

#define BUSYSTATUS 4

#define SENDFAILED 5

#define EINVAHNDLE 6

#define EINVACHNNL 7

#define EINVASTATE 8

#define CO_UINT8_VALUE(a) ((uint8_t)((a)&0xFF))

#define CO_SPLIT_128_UUID_LE(uuid_be) \
    CO_UINT8_VALUE(((uint8_t [16](uuid_be)))[15]), \
    CO_UINT8_VALUE(((uint8_t [16](uuid_be)))[14]), \
    CO_UINT8_VALUE(((uint8_t [16](uuid_be)))[13]), \
    CO_UINT8_VALUE(((uint8_t [16](uuid_be)))[12]), \
    CO_UINT8_VALUE(((uint8_t [16](uuid_be)))[11]), \
    CO_UINT8_VALUE(((uint8_t [16](uuid_be)))[10]), \
    CO_UINT8_VALUE(((uint8_t [16](uuid_be)))[9]), \
    CO_UINT8_VALUE(((uint8_t [16](uuid_be)))[8]), \
    CO_UINT8_VALUE(((uint8_t [16](uuid_be)))[7]), \
    CO_UINT8_VALUE(((uint8_t [16](uuid_be)))[6]), \
    CO_UINT8_VALUE(((uint8_t [16](uuid_be)))[5]), \
    CO_UINT8_VALUE(((uint8_t [16](uuid_be)))[4]), \
    CO_UINT8_VALUE(((uint8_t [16](uuid_be)))[3]), \
    CO_UINT8_VALUE(((uint8_t [16](uuid_be)))[2]), \
    CO_UINT8_VALUE(((uint8_t [16](uuid_be)))[1]), \
    CO_UINT8_VALUE(((uint8_t [16](uuid_be)))[0])

#define CO_SPLIT_UINT16_LE(a) \
    CO_UINT8_VALUE(a), CO_UINT8_VALUE((a)>>8)

#define CO_SPLIT_UINT32_LE(a) \
    CO_UINT8_VALUE(a), CO_UINT8_VALUE((a)>>8), \
    CO_UINT8_VALUE((a)>>16), CO_UINT8_VALUE((a)>>24)

#define CO_SPLIT_UINT24_LE(a) \
    CO_UINT8_VALUE(a), CO_UINT8_VALUE((a)>>8), \
    CO_UINT8_VALUE((a)>>16)

#define CO_COMBINE_UINT16_LE(p) \
    (CO_UINT8_VALUE(((uint8_t *)(p))[0]) | (((uint16_t)CO_UINT8_VALUE(((uint8_t *)(p))[1]))<<8))

#define CO_COMBINE_UINT24_LE(p) \
    (CO_UINT8_VALUE(((uint8_t *)(p))[0]) | (((uint16_t)CO_UINT8_VALUE(((uint8_t *)(p))[1]))<<8) | \
    (((uint16_t)CO_UINT8_VALUE(((uint8_t *)(p))[2]))<<16))

#define CO_COMBINE_UINT32_LE(p) \
    (CO_UINT8_VALUE(((uint8_t *)(p))[0]) | (((uint16_t)CO_UINT8_VALUE(((uint8_t *)(p))[1]))<<8) | \
    (((uint16_t)CO_UINT8_VALUE(((uint8_t *)(p))[2]))<<16) | (((uint16_t)CO_UINT8_VALUE(((uint8_t *)(p))[3]))<<24))

#define CO_SPLIT_UINT16_BE(a) \
    CO_UINT8_VALUE((a)>>8), CO_UINT8_VALUE(a)

#define CO_SPLIT_UINT24_BE(a) \
    CO_UINT8_VALUE((a)>>16), CO_UINT8_VALUE((a)>>8), \
    CO_UINT8_VALUE(a)

#define CO_SPLIT_UINT32_BE(a) \
    CO_UINT8_VALUE((a)>>24), CO_UINT8_VALUE((a)>>16), \
    CO_UINT8_VALUE((a)>>8), CO_UINT8_VALUE(a)

#define CO_COMBINE_UINT16_BE(p) \
    ((((uint16_t)CO_UINT8_VALUE(((uint8_t *)(p))[0]))<<8) | CO_UINT8_VALUE(((uint8_t *)(p))[1]))

#define CO_COMBINE_UINT24_BE(p) \
    ((((uint16_t)CO_UINT8_VALUE(((uint8_t *)(p))[0]))<<16) | (((uint16_t)CO_UINT8_VALUE(((uint8_t *)(p))[1]))<<8) |  \
    (((uint16_t)CO_UINT8_VALUE(((uint8_t *)(p))[2]))))

#define CO_COMBINE_UINT32_BE(p) \
    ((((uint16_t)CO_UINT8_VALUE(((uint8_t *)(p))[0]))<<24) | (((uint16_t)CO_UINT8_VALUE(((uint8_t *)(p))[1]))<<16) |  \
    (((uint16_t)CO_UINT8_VALUE(((uint8_t *)(p))[2]))<<8) | CO_UINT8_VALUE(((uint8_t *)(p))[3]))

static inline uint32_t co_round_size(uint32_t a)
{
    return ((a + 3) / 4) * 4;
}

static inline uint32_t co_range_value_map(uint32_t from_val, uint32_t from_min,
                                          uint32_t from_max, uint32_t to_min, uint32_t to_max)
{
    if (from_val <= from_min)
    {
        return to_min;
    }

    if (from_val >= from_max)
    {
        return to_max;
    }

    return ((from_val - from_min) * (to_max - to_min) + (from_max - from_min) / 2) / (from_max - from_min) + to_min;
}

#define CO_BIT_MASK(bit_index) (1 << (bit_index))

#define CO_MIN_VALUE(a, b) ((a)<=(b) ? (a) : (b))

#define CO_MAX_VALUE(a, b) ((a)>(b) ? (a) : (b))

#define CO_UINT_TO_PTR(a) ((void *)(uintptr_t)(a))

/**
 * byte order
 */

#define HOST_OS_LITTLE_ENDIAN 1

#if HOST_OS_LITTLE_ENDIAN
static inline uint16_t co_host_to_uint16_le(uint16_t n) { return n; }
static inline uint16_t co_uint16_le_to_host(uint16_t n) { return n; }
static inline uint32_t co_host_to_uint32_le(uint32_t n) { return n; }
static inline uint32_t co_uint32_le_to_host(uint32_t n) { return n; }
static inline uint32_t co_host_to_uint24_le(uint32_t n) { return n; }
static inline uint32_t co_uint24_le_to_host(uint32_t n) { return n; }
#else
static inline uint16_t co_host_to_uint16_le(uint16_t n) {
    return ((n & 0xff) << 8) | (n >> 8);
}
static inline uint32_t co_host_to_uint32_le(uint32_t n) {
    return ((n & 0xff) << 24) | ((n & 0xff00) << 8) | ((n & 0xff0000) >> 8) | ((n & 0xff000000) >> 24);
}
static inline uint32_t co_host_to_uint24_le(uint32_t n) {
    return ((n << 16) & 0xFF0000) | (n &0xFF00) | ((n >> 16) & 0xFF);
}
static inline uint16_t co_uint16_le_to_host(uint16_t n) {
    return co_host_to_uint16_le(n);
}
static inline uint32_t co_uint32_le_to_host(uint32_t n) {
    return co_host_to_uint32_le(n);
}
static inline uint32_t co_uint24_le_to_host(uint32_t n) {
    return co_host_to_uint24_le(n);
}
#endif

#if HOST_OS_LITTLE_ENDIAN
static inline uint16_t co_host_to_uint16_be(uint16_t n) {
    return ((n & 0xff) << 8) | (n >> 8);
}
static inline uint32_t co_host_to_uint32_be(uint32_t n) {
    return ((n & 0xff) << 24) | ((n & 0xff00) << 8) | ((n & 0xff0000) >> 8) | ((n & 0xff000000) >> 24);
}
static inline uint32_t co_host_to_uint24_be(uint32_t n) {
    return ((n << 16) & 0xFF0000) | (n &0xFF00) | ((n >> 16) & 0xFF);
}
static inline uint16_t co_uint16_be_to_host(uint16_t n) {
    return co_host_to_uint16_be(n);
}
static inline uint32_t co_uint32_be_to_host(uint32_t n) {
    return co_host_to_uint32_be(n);
}
static inline uint32_t co_uint24_be_to_host(uint32_t n) {
    return co_host_to_uint24_be(n);
}
#else
static inline uint16_t co_host_to_uint16_be(uint16_t n) { return n; }
static inline uint16_t co_uint16_be_to_host(uint16_t n) { return n; }
static inline uint32_t co_host_to_uint32_be(uint32_t n) { return n; }
static inline uint32_t co_uint32_be_to_host(uint32_t n) { return n; }
static inline uint32_t co_host_to_uint24_be(uint32_t n) { return n; }
static inline uint32_t co_uint24_be_to_host(uint32_t n) { return n; }
#endif

static inline void co_host_uint16_to_ptr_le(uint16_t n, uint8_t *p) {
    p[0] = (uint8_t)(n & 0xFF);
    p[1] = (uint8_t)((n >> 8) & 0xFF);
}
static inline void co_host_uint32_to_ptr_le(uint32_t n, uint8_t *p) {
    p[0] = (uint8_t)(n & 0xFF);
    p[1] = (uint8_t)((n >> 8) & 0xFF);
    p[2] = (uint8_t)((n >> 16) & 0xFF);
    p[3] = (uint8_t)((n >> 24) & 0xFF);
}
static inline void co_host_uint16_to_ptr_be(uint16_t n, uint8_t *p) {
    p[1] = (uint8_t)(n & 0xFF);
    p[0] = (uint8_t)((n >> 8) & 0xFF);
}
static inline void co_host_uint32_to_ptr_be(uint32_t n, uint8_t *p) {
    p[3] = (uint8_t)(n & 0xFF);
    p[2] = (uint8_t)((n >> 8) & 0xFF);
    p[1] = (uint8_t)((n >> 16) & 0xFF);
    p[0] = (uint8_t)((n >> 24) & 0xFF);
}

#ifndef OFFSETOF
#define OFFSETOF(type, member) ((unsigned int) &((type *)0)->member)
#endif

#ifndef CONTAINER_OF
#define CONTAINER_OF(ptr, type, member) ((type *)( (char *)ptr - OFFSETOF(type,member) ))
#endif

U32 be_to_host32(const U8* ptr);

#define STR_BE32(buff,num) ( (((U8*)buff)[0] = (U8) ((num)>>24)),  \
                              (((U8*)buff)[1] = (U8) ((num)>>16)),  \
                              (((U8*)buff)[2] = (U8) ((num)>>8)),   \
                              (((U8*)buff)[3] = (U8) (num)) )

#define STR_BE16(buff,num) ( (((U8*)buff)[0] = (U8) ((num)>>8)),    \
                              (((U8*)buff)[1] = (U8) (num)) )

#define BEtoHost16(ptr)  (U16)( ((U16) *((U8*)(ptr)) << 8) | \
                                ((U16) *((U8*)(ptr)+1)) )

#define BEtoHost32(ptr)  (U32)( ((U32) *((U8*)(ptr)) << 24)   | \
                                ((U32) *((U8*)(ptr)+1) << 16) | \
                                ((U32) *((U8*)(ptr)+2) << 8)  | \
                                ((U32) *((U8*)(ptr)+3)) )

/* Store value into a buffer in Little Endian format */
#define StoreLE16(buff,num) ( ((buff)[1] = (U8) ((num)>>8)),    \
                              ((buff)[0] = (U8) (num)) )

#define StoreLE32(buff,num) ( ((buff)[3] = (U8) ((num)>>24)),  \
                              ((buff)[2] = (U8) ((num)>>16)),  \
                              ((buff)[1] = (U8) ((num)>>8)),   \
                              ((buff)[0] = (U8) (num)) )

/* Store value into a buffer in Big Endian format */
#define StoreBE16(buff,num) ( ((buff)[0] = (U8) ((num)>>8)),    \
                              ((buff)[1] = (U8) (num)) )

#define StoreBE32(buff,num) ( ((buff)[0] = (U8) ((num)>>24)),  \
                              ((buff)[1] = (U8) ((num)>>16)),  \
                              ((buff)[2] = (U8) ((num)>>8)),   \
                              ((buff)[3] = (U8) (num)) )

#define LEtoHost16(ptr)  (U16)(((U16) *((U8*)(ptr)+1) << 8) | \
        (U16) *((U8*)(ptr)))

#define LETOHOST16(ptr)  (uint16_t)( ((uint16_t) *((uint8_t*)(ptr)+1) << 8) | \
                                ((uint16_t) *((uint8_t*)(ptr))) )

#define LETOHOST32(ptr)  (uint32_t)( ((uint32_t) *((uint8_t*)(ptr)+3) << 24) | \
                                ((uint32_t) *((uint8_t*)(ptr)+2) << 16) | \
                                ((uint32_t) *((uint8_t*)(ptr)+1) << 8)  | \
                                ((uint32_t) *((uint8_t*)(ptr))) )

/**
 * bt packet
 */

#define BTIF_BTP_FLAG_NONE       0x0000  /* No current flags */
#define BTIF_BTP_FLAG_INUSE      0x0001  /* Used only by packet owner */
#define BTIF_BTP_FLAG_LSYSTEM    0x0002  /* Used only by L2CAP */
#define BTIF_BTP_FLAG_TAIL       0x0004  /* Used only by L2CAP Applications */
#define BTIF_BTP_FLAG_RDEV       0x0008  /* Used only by L2CAP */
#define BTIF_BTP_FLAG_FCS        0x0010  /* FCS field is valid, set only by L2CAP */
#define BTIF_BTP_FLAG_NON_FLUSH  0x0020  /* Used by L2CAP, HCI or packet owner */
#define BTIF_BTP_FLAG_ENHANCED   0x0040  /* Used only by L2CAP */
#define BTIF_BTP_FLAG_SEGMENTED  0x0080  /* Used only by L2CAP */
#define BTIF_BTP_FLAG_TXDONE     0x0100  /* Used only by L2CAP */
#define BTIF_BTP_FLAG_USER       0x0200  /* Used only by L2CAP */
#define BTIF_BTP_FLAG_IMMEDIATE  0x0400  /* Used only by L2CAP */

#define BTIF_BT_PACKET_HEADER_LEN 25

typedef struct btif_bt_packet_t {
    struct list_node node;
    uint8_t *data;              /* Points to a buffer of user data.  */
    uint16_t dataLen;           /* Indicates the length of "data" in bytes. */
    uint16_t flags;             /* Must be initialized to BTIF_BTP_FLAG_NONE by
                                 * applications running on top of L2CAP.
                                 */

    uint8_t l2cap_priority;

    /* Group: The following fields are for internal use only by the stack. */
    void *ulpContext;
    uint8_t *tail;
    uint16_t tailLen;

    uint16_t llpContext;
    uint16_t remoteCid;

    uint8_t hciPackets;
    uint8_t headerLen;
    uint8_t header[BTIF_BT_PACKET_HEADER_LEN];
} btif_bt_packet_t;

/**
 * error code
 */

typedef uint8_t btif_error_code_t;

#define BTIF_BEC_NO_ERROR                       0x00
#define BTIF_BEC_UNKNOWN_HCI_CMD                0x01
#define BTIF_BEC_UNKNOWN_CONN_IDENTIFIER        0x02
#define BTIF_BEC_HARDWARE_FAILURE               0x03
#define BTIF_BEC_PAGE_TIMEOUT                   0x04
#define BTIF_BEC_AUTHENTICATE_FAILURE           0x05
#define BTIF_BEC_MISSING_KEY                    0x06
#define BTIF_BEC_MEMORY_FULL                    0x07
#define BTIF_BEC_CONNECTION_TIMEOUT             0x08
#define BTIF_BEC_MAX_CONNECTIONS                0x09
#define BTIF_BEC_MAX_SCO_CONNECTIONS            0x0a
#define BTIF_BEC_ACL_ALREADY_EXISTS             0x0b
#define BTIF_BEC_COMMAND_DISALLOWED             0x0c
#define BTIF_BEC_LIMITED_RESOURCE               0x0d
#define BTIF_BEC_SECURITY_ERROR                 0x0e
#define BTIF_BEC_PERSONAL_DEVICE                0x0f
#define BTIF_BEC_HOST_TIMEOUT                   0x10
#define BTIF_BEC_UNSUPPORTED_FEATURE            0x11
#define BTIF_BEC_INVALID_HCI_PARM               0x12
#define BTIF_BEC_USER_TERMINATED                0x13
#define BTIF_BEC_LOW_RESOURCES                  0x14
#define BTIF_BEC_POWER_OFF                      0x15
#define BTIF_BEC_LOCAL_TERMINATED               0x16
#define BTIF_BEC_REPEATED_ATTEMPTS              0x17
#define BTIF_BEC_PAIRING_NOT_ALLOWED            0x18
#define BTIF_BEC_UNKNOWN_LMP_PDU                0x19
#define BTIF_BEC_UNSUPPORTED_REMOTE             0x1a
#define BTIF_BEC_SCO_OFFSET_REJECT              0x1b
#define BTIF_BEC_SCO_INTERVAL_REJECT            0x1c
#define BTIF_BEC_SCO_AIR_MODE_REJECT            0x1d
#define BTIF_BEC_INVALID_LMP_PARM               0x1e
#define BTIF_BEC_UNSPECIFIED_ERR                0x1f
#define BTIF_BEC_UNSUPPORTED_LMP_PARM           0x20
#define BTIF_BEC_ROLE_CHG_NOT_ALLOWED           0x21
#define BTIF_BEC_LMP_RESPONSE_TIMEOUT           0x22
#define BTIF_BEC_LMP_TRANS_COLLISION            0x23
#define BTIF_BEC_LMP_PDU_NOT_ALLOWED            0x24
#define BTIF_BEC_ENCRYP_MODE_NOT_ACC            0x25
#define BTIF_BEC_UNIT_KEY_USED                  0x26
#define BTIF_BEC_QOS_NOT_SUPPORTED              0x27
#define BTIF_BEC_INSTANT_PASSED                 0x28
#define BTIF_BEC_PAIR_UNITKEY_NO_SUPP           0x29
#define BTIF_BEC_NOT_FOUND                      0xf1
#define BTIF_BEC_REQUEST_CANCELLED              0xf2
#define BTIF_BEC_INVALID_SDP_PDU                0xd1
#define BTIF_BEC_SDP_DISCONNECT                 0xd2
#define BTIF_BEC_SDP_NO_RESOURCES               0xd3
#define BTIF_BEC_SDP_INTERNAL_ERR               0xd4
#define BTIF_BEC_STORE_LINK_KEY_ERR             0xe0
#define BTIF_BEC_BT_LINK_REAL_DISCONNECTED      0xb8
#define BTIF_BEC_BT_CANCEL_PAGE                 0xb9

//BES vendor error code
#define BT_ECODE_DISCONNECT_ITSELF      0xba
#define BT_ECODE_SWITCH_TIME_OUT        0Xbe
#define BT_ECODE_IBRT_SLAVE_CLEANUP     0xbb
#define BT_ECODE_SDP_OPEN_TIMEOUT       0xbc
#define BT_ECODE_SDP_ClIENT_TX_TIMEOUT  0xbd

#define BT_CONN_HANDLE_MASK                     0x0FFF

#define BTIF_BD_ADDR_SIZE   6
#define BTIF_LINK_KEY_SIZE  16

typedef enum {
    HCI_CONN_TYPE_INVALID = 0,
    HCI_CONN_TYPE_BT_ACL,
    HCI_CONN_TYPE_BT_SCO,
    HCI_CONN_TYPE_LE_ACL,
    HCI_CONN_TYPE_LE_ISO,
} hci_conn_type_t;

/**
 * bt device
 */
#ifndef BT_DEVICE_NUM
#define BT_DEVICE_NUM 2
#endif

enum BT_DEVICE_ID_T {
    BT_DEVICE_ID_1 = 0,
    BT_DEVICE_ID_2 = 1,
    BT_DEVICE_ID_3 = 2,
    BT_DEVICE_ID_N = BT_DEVICE_NUM,
    BT_DEVICE_TWS_ID = 0x0f,
    BT_DEVICE_AUTO_CHOICE_ID = 0xee,
    BT_DEVICE_SEND_AVRCP_PLAY = 0xf0,
    BT_DEVICE_INVALID_ID = 0xff,
};

#define BLE_DEVICE_ID_BASE 0x30
#define BLE_DEVICE_ID_1 (BLE_DEVICE_ID_BASE + 0)
#define BLE_DEVICE_ID_2 (BLE_DEVICE_ID_BASE + 1)
#define BLE_DEVICE_ID_3 (BLE_DEVICE_ID_BASE + 2)
#define BLE_DEVICE_ID_N (BLE_DEVICE_ID_BASE + 0x10)

#define BT_INVALID_CONN_HANDLE 0xFFFF

#define GAP_LE_CONN_ID_PREFIX 0x8000
#define GAP_LE_CONN_ID_1 (GAP_LE_CONN_ID_PREFIX|BLE_DEVICE_ID_1)
#define GAP_LE_CONN_ID_2 (GAP_LE_CONN_ID_PREFIX|BLE_DEVICE_ID_2)
#define GAP_LE_CONN_ID_3 (GAP_LE_CONN_ID_PREFIX|BLE_DEVICE_ID_3)

#define GAP_BT_CONN_ID_PREFIX 0x4000
#define GAP_BT_CONN_ID_1 (GAP_BT_CONN_ID_PREFIX|BT_DEVICE_ID_1)
#define GAP_BT_CONN_ID_2 (GAP_BT_CONN_ID_PREFIX|BT_DEVICE_ID_2)
#define GAP_BT_CONN_ID_3 (GAP_BT_CONN_ID_PREFIX|BT_DEVICE_ID_3)

#ifdef BTM_MAX_LINK_NUMS
#define BT_ACL_MAX_LINK_NUMS   BTM_MAX_LINK_NUMS
#else
#define BT_ACL_MAX_LINK_NUMS   3
#endif

#define Plt_TICKS_TO_MS(ticks) TICKS_TO_MS(ticks)

bool co_uuid_is_equal_le(const uint8_t *uuid_a_le, uint16_t a, const uint8_t *uuid_b_le, uint16_t b);
bool co_uuid_16_equal_le(uint16_t uuid_16_bit, const uint8_t *uuid_b_le, uint16_t b);

typedef struct btif_event_t btif_event_t;

enum BESBT_HOOK_USER_T {
    BESBT_HOOK_USER_0 = 0,
    BESBT_HOOK_USER_1,
    BESBT_HOOK_USER_2,
    BESBT_HOOK_USER_3,
    BESBT_HOOK_USER_QTY
};

typedef void (*BESBT_HOOK_HANDLER)(void);

typedef struct APP_KEY_STATUS APP_KEY_STATUS;

typedef struct
{
    const char* string;
    void (*cmd_function)(const char* param, uint32_t param_len);
} app_bt_cmd_handle_t;

enum BT_ACL_CONN_TYPE
{
    BT_ACL_CONNECT_REQ  = 0,
    BT_ACL_CONNECT      = 1,
    BT_ACL_DISCONNECT   = 2
};

typedef struct app_bt_search_result {
    bt_bdaddr_t *addr;
    const char *name;
    int name_len;
} app_bt_search_result_t;

#define BTM_RX_FILTER_HCI_TYPE   (0x01)
#define BTM_RX_FILTER_L2C_TYPE   (0x02)
#define BT_EIR_GLOBAL_SRV_UUIDS_GATHER_CALLBACK_COUNT 3

typedef enum {
    HCI_CMD_COMPLETE_USER_IBRT_CMD        = 0,
    HCI_CMD_COMPLETE_USER_BLE             = 1,
    HCI_CMD_COMPLETE_USER_RESEVER_1       = 2,
    HCI_CMD_COMPLETE_USER_RESEVER_2       = 3,
    HCI_CMD_COMPLETE_USER_RESEVER_3       = 4,
    HCI_CMD_COMPLETE_USER_RESEVER_4       = 5,
    HCI_CMD_COMPLETE_USER_NUM,
} HCI_CMD_COMPLETE_USER_E;

#define BTM_BONDING_NOT_REQUIRED 0x00
#define BTM_DEDICATED_BONDING    0x02
#define BTM_GENERAL_BONDING      0x04
#define BTM_BONDING_NOT_ALLOWED  0x10

#define BTM_AUTH_MITM_PROTECT_NOT_REQUIRED  0x00
#define BTM_AUTH_MITM_PROTECT_REQUIRED      0x01

#define BTM_IO_DISPLAY_ONLY   0
#define BTM_IO_DISPLAY_YESNO  1
#define BTM_IO_KEYBOARD_ONLY  2
#define BTM_IO_NO_IO          3

typedef struct {
    uint8_t bonding_mode;
    uint8_t mitm_protection_required;
    uint8_t io_cap;
    uint8_t oob_present;
} bt_iocap_requirement_t;

typedef struct _evm_timer evm_timer_t;
typedef void (*evm_timer_notify) (evm_timer_t *);
typedef struct { /* empty */ } btif_remote_device_t;
typedef bool (*ibrt_rx_data_filter_func)(const bt_bdaddr_t *bdaddr,uint8_t rx_filter_type,void*para);
typedef void (*l2cap_sdp_disconnect_callback)(const bt_bdaddr_t *bdaddr);
typedef uint8 (*bt_get_ibrt_role_callback)(const bt_bdaddr_t *para);
typedef uint8 (*bt_get_ui_role_callback)(void);
typedef uint8_t (*bt_get_tss_state_callback)(const bt_bdaddr_t *para);
typedef bool (*bt_user_allow_accept_sco_callback)(uint8_t device_id, void *addr);
typedef bool (*extra_acl_conn_req_callback)(uint8_t *remote, uint8_t *cod);
typedef void (*bt_cmgr_sniff_timeout_ext_handler)(evm_timer_t * timer, unsigned int* skipInternalHandler);
typedef bool (*bt_remote_is_mobile_callback_t)(const bt_bdaddr_t *remote);
typedef void (*bt_eir_fill_manufacture_data)(uint8_t *buff, uint32_t* offset);
typedef void (*bt_register_sdp_disc_callback_t)(const bt_bdaddr_t* addr);
typedef void (*l2cap_process_bredr_smp_req_callback_func)(uint8 device_id, uint16 conn_handle, uint16 len, uint8 *data);
typedef void (*btif_event_callback_t) (const btif_event_t *);
typedef uint16_t (*bt_get_ibrt_handle_callback_t)(const bt_bdaddr_t* remote);
typedef uint8_t (*bt_get_ibrt_role_callback_t)(const bt_bdaddr_t* addr);
typedef uint8_t (*bt_get_ibrt_ui_role_callback_t)(void);
typedef int (*bt_a2dp_stream_command_pack_callback_t)(void* remote, uint8_t transaction, uint8_t signal_id);
typedef bool (*bt_ibrt_rx_data_filter_callback_t)(const bt_bdaddr_t* addr, uint8_t rx_filter_type, void* para);
typedef void (*bt_sco_codec_info_sync_callback_t)(const bt_bdaddr_t* remote, uint8_t codec);
typedef bool (*bt_avrcp_register_notify_callback_t)(uint8_t event);
typedef void (*bt_avrcp_register_notify_response_callback_t)(uint8_t event);
typedef void (*bt_hci_tx_tss_process_callback_t)(void);
typedef void (*bt_stack_create_acl_failed_callback_t)(const bt_bdaddr_t *);
typedef void (*bt_acl_conn_state_change_cb_t)(uint8_t type,bt_bdaddr_t* addr, uint32_t para);
typedef void (*bt_pair_state_change_cb_t)(bt_bdaddr_t* addr, uint32_t para);
typedef uint8_t (*btif_hci_sync_airmode_check_ind_func)(uint8_t status, bt_bdaddr_t *bdaddr);

#define LMP_VERSION_4_2     0x08
#define LMP_VERSION_5_0     0x09
#define LMP_VERSION_5_1     0x0A
#define LMP_VERSION_5_2     0x0B
#define LMP_VERSION_5_3     0x0C
#define LMP_VERSION_5_4     0x0D
#define LMP_VERSION_6_0     0x0E

/**
 * @note Deprecated Interfaces
 *
 */

#define bt_thread_call_func_1(func,a)                       bt_exec_async_1(true,func,a)
#define bt_thread_call_func_2(func,a,b)                     bt_exec_async_2(true,func,a,b)
#define bt_thread_call_func_3(func,a,b,c)                   bt_exec_async_3(true,func,a,b,c)

#define bt_defer_call_func_1(func,a)                        bt_exec_async_1(false,func,a)

/* Entry list impl by co_list */
typedef struct _list_entr
{
    struct list_node node;
} list_entry_t;

#define initialize_list_head(ListHead)      INIT_LIST_HEAD((struct list_node *)ListHead)

#define is_list_empty(ListHead)             colist_is_list_empty((struct list_node *)ListHead)

#define is_node_on_list(ListHead,node)      colist_is_node_on_list((struct list_node *)ListHead, (struct list_node *)node)

#define insert_tail_list(ListHead,node)     colist_addto_tail((struct list_node *)node, (struct list_node *)ListHead)

#define remove_head_list(ListHead)          ({ struct list_node *head = colist_get_head((struct list_node *)ListHead);\
                                            if (head != NULL) { colist_delete(head);} (struct _list_entr *)head; })

#ifdef __cplusplus
}
#endif
#endif /* __BT_COMMON_DEFINE_H__ */
