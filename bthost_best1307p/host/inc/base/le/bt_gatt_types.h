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
 *     ____    _  _____ _____   _____
 *    / ___|  / \|_   _|_   _| |_   _|   _ _ __   ___  ___
 *   | |  _  / _ \ | |   | |     | || | | | '_ \ / _ \/ __|
 *   | |_| |/ ___ \| |   | |     | || |_| | |_) |  __/\__ \
 *    \____/_/   \_\_|   |_|     |_| \__, | .__/ \___||___/
 *                                   |___/|_|
 *
 ****************************************************************************************
 */

#define BT_GATT_ARRAY_EMPTY                                            (0)
#define BT_GATT_UUID_16_LEN                                            (2)
#define BT_GATT_UUID_128_LEN                                           (16)
#define BT_GATT_ATTR_MEM_LEN                                           (28)

#define BT_GATT_PERM_NO_PERM                                           (0x00)
#define BT_GATT_PERM_SEC_NONE                                          (0x00)
#define BT_GATT_PERM_RD_PERM                                           (0x01)
#define BT_GATT_PERM_RD_MITM_AUTH                                      (0x02)
#define BT_GATT_PERM_RD_AUTHOR                                         (0x04)
#define BT_GATT_PERM_RD_ENC                                            (0x08)
#define BT_GATT_PERM_WR_PERM                                           (0x10)
#define BT_GATT_PERM_WR_MITM_AUTH                                      (0x20)
#define BT_GATT_PERM_WR_AUTHOR                                         (0x40)
#define BT_GATT_PERM_WR_ENC                                            (0x80)

#define BT_GATT_PROP_NO_PROP                                           (0x00)
#define BT_GATT_PROP_BROADCAST                                         (0x01) // permit broadcasts of char value using SCCD, if set, SCCD shall exist
#define BT_GATT_PROP_RD_REQ                                            (0x02) // permit read req of the char value
#define BT_GATT_PROP_WR_CMD                                            (0x04) // permit write cmd of the char value
#define BT_GATT_PROP_WR_REQ                                            (0x08) // permit write req of the char value
#define BT_GATT_PROP_NTF_PROP                                          (0x10) // permit notification of the char value, if set, CCCD shall exist
#define BT_GATT_PROP_IND_PROP                                          (0x20) // permit indication of the char value with ack, if set, CCCD shall exist
#define BT_GATT_PROP_SIGNED_WR                                         (0x40) // permit authenticated signed write of the char value
#define BT_GATT_PROP_EXT_PROP                                          (0x80) // additional char properties are defined in the CEPD, if set, CEPD shall exist
#define BT_GATT_PROP_EXT_PROP_RELIABLE_WRITE                           (0x0001) // permit reliable writes of the char value
#define BT_GATT_PROP_EXT_PROP_WRITABLE_AUX                             (0x0002) // permit write CUSD descriptor

#define BT_GATT_CCCD_NOTIFICATION                                      (0x0001) // the char value shall be notified, can only be set if char ntf prop set
#define BT_GATT_CCCD_INDICATION                                        (0x0002) // the char value shall be indicated, can only be set if char ind prop set

typedef enum
{
    BT_GATTS_EVENT_REG_SRVC_CMP,
    BT_GATTS_EVENT_CONN_OPENED = 0x1F00,
    BT_GATTS_EVENT_CONN_CLOSED,
    BT_GATTS_EVENT_CHAR_READ,
    BT_GATTS_EVENT_DESC_READ,
    BT_GATTS_EVENT_CHAR_WRITE,
    BT_GATTS_EVENT_DESC_WRITE,
    BT_GATTS_EVENT_NTF_TX_DONE,
    BT_GATTS_EVENT_INDICATE_CFM,
    BT_GATTS_EVENT_MTU_CHANGED,
    BT_GATTS_EVENT_CONN_UPDATED,
    BT_GATTS_EVENT_CONN_ENCRYPTED,
    BT_GATTS_EVENT_EATT_CREATED,
} bt_gatts_event_t;

typedef enum
{
    BT_ATT_ERR_NO_ERROR                      = 0x00,
    BT_ATT_ERR_INVALID_HANDLE                = 0x01,
    BT_ATT_ERR_RD_NOT_PERMITTED              = 0x02,
    BT_ATT_ERR_WR_NOT_PERMITTED              = 0x03,
    BT_ATT_ERR_INVALID_PDU                   = 0x04,
    BT_ATT_ERR_INSUFF_AUTHEN                 = 0x05,
    BT_ATT_ERR_REQ_NOT_SUPPORT               = 0x06,
    BT_ATT_ERR_INVALID_OFFSET                = 0x07,
    BT_ATT_ERR_INSUFF_AUTHOR                 = 0x08,
    BT_ATT_ERR_PREPARE_QUEUE_FULL            = 0x09,
    BT_ATT_ERR_ATTRIBUTE_NOT_FOUND           = 0x0A,
    BT_ATT_ERR_ATTRIBUTE_NOT_LONG            = 0x0B,
    BT_ATT_ERR_ENC_KEY_TOO_SHORT             = 0x0C,
    BT_ATT_ERR_INVALID_ATTR_VALUE_LEN        = 0x0D,
    BT_ATT_ERR_UNLIKELY_ERROR                = 0x0E,
    BT_ATT_ERR_INSUFF_ENCRYPT                = 0x0F,
    BT_ATT_ERR_UNSUPP_GROUP_TYPE             = 0x10,
    BT_ATT_ERR_INSUFF_RESOURCES              = 0x11,
    BT_ATT_ERR_DATABASE_OUT_OF_SYNC          = 0x12,
    BT_ATT_ERR_VALUE_NOT_ALLOWED             = 0x13,
    BT_ATT_ERR_APPLICATION_ERROR             = 0x80, // 0x80 ~ 0x9F application error code defined by a higher layer spec
    BT_ATT_ERR_UNKNOWN_ERROR                 = 0x81,
    BT_ATT_ERR_SEND_FAILED                   = 0x82,
    BT_ATT_ERR_MEMORY_FULL                   = 0x83,
    BT_ATT_ERR_RESPONSE_TIMEOUT              = 0x84,
    BT_ATT_ERR_BEARER_CLOSED                 = 0x85,
    BT_ATT_ERR_CONN_CLOSED                   = 0x86,
    BT_ATT_ERR_UUID_NOT_FOUND                = 0x87,
    BT_ATT_ERR_RSP_DATA_INVALID              = 0x88,
    BT_ATT_ERR_RSP_INVALID_HANDLE            = 0x89,
    BT_ATT_ERR_INVALID_CONN                  = 0x8A,
    BT_ATT_ERR_INVALID_MTU                   = 0x8B,
    BT_ATT_ERR_INVALID_UUID_LEN              = 0x8C,
    BT_ATT_ERR_INVALID_VALUE_LEN             = 0x8D,
    BT_ATT_ERR_INVALID_CACHE_HANDLE          = 0x8E,
    BT_ATT_ERR_MUST_SHORT_VALUE              = 0x8F,
    BT_ATT_ERR_WRITE_DECLARE_ATTR            = 0x90,
    BT_ATT_ERR_NO_INCLUDE_EXIST              = 0x91,
    BT_ATT_ERR_ADD_INCL_SERVICE_FAILED       = 0x92,
    BT_ATT_ERR_NOT_FOUND                     = 0x93,
    BT_ATT_ERR_IBRT_SLAVE_CANT_SEND          = 0x94,
    BT_ATT_ERR_PEER_SERVICE_CHANGED          = 0x95,
    BT_ATT_ERR_ROLE_SWITCH                   = 0x96,
    BT_ATT_ERR_COMMON_PROFILE_SERVICE        = 0xE0, // 0xE0 ~ 0xFF common profile and service error codes defined in CSS
    BT_ATT_ERR_COMMON_PROFILE_SERVICE_RFU    = 0xFB, // 0xE0 ~ 0xFB reserved for future use
    BT_ATT_ERR_WRITE_REQ_REJECTED            = 0xFC,
    BT_ATT_ERR_CCCD_IMPROPER_CONFIGED        = 0xFD,
    BT_ATT_ERR_PROC_ALREADY_IN_PROGRESS      = 0xFE,
    BT_ATT_ERR_OUT_OF_RANGE                  = 0xFF,
} bt_att_err_code_t;

typedef enum
{
    BT_GATTC_EVENT_OPENED = 0x1F10,
    BT_GATTC_EVENT_CLOSED,
    BT_GATTC_EVENT_MTU_CHANGED,
    BT_GATTC_EVENT_CONN_ENCRYPTED,
    BT_GATTC_EVENT_CONN_UPDATED,
    BT_GATTC_EVENT_SERVICE_CHANGED,
    BT_GATTC_EVENT_SERVICE,
    BT_GATTC_EVENT_INCLUDE,
    BT_GATTC_EVENT_CHARACTER,
    BT_GATTC_EVENT_DESCRIPTOR,
    BT_GATTC_EVENT_CHAR_READ_RSP,
    BT_GATTC_EVENT_DESC_READ_RSP,
    BT_GATTC_EVENT_CHAR_WRITE_RSP,
    BT_GATTC_EVENT_DESC_WRITE_RSP,
    BT_GATTC_EVENT_WRITE_EXEC_RSP,
    BT_GATTC_EVENT_NOTIFY, // profile shall check notify belong self or not
    BT_GATTC_EVENT_EATT_CREATED,
} bt_gattc_event_t;

typedef enum
{
    BT_ATTR_TYPE_PRI_SERVICE = 0x00,
    BT_ATTR_TYPE_CHARACTER,
    BT_ATTR_TYPE_DESCRIPTOR,

    /// TODO:Below are not read yet
    BT_ATTR_TYPE_SEC_SERVICE,
    BT_ATTR_TYPE_INC_SERVICE,

    BT_ATTR_TYPE_MAX,
} bt_attr_type_t;

typedef struct
{
    uint8_t attr_uuid[BT_GATT_UUID_128_LEN];
} bt_attr_uuid_t;

typedef struct
{
    uint8_t attr_flags;
    uint8_t attr_perm;
    uint8_t uuid_le[BT_GATT_UUID_16_LEN];
} __attribute__((packed)) bt_attr_t;

typedef struct
{
    const bt_attr_t *service;
    const bt_attr_t *character;
    uint8_t service_inst_id;
    uint8_t char_instance_id;
    const uint32_t *dummy;
} bt_gatt_char_notify_t;

typedef struct
{
    uint16_t char_value_handle;
    uint16_t value_len;
    const uint8_t *value;
} bt_gatt_char_ntf_hdl_t;

typedef struct
{
    uint8_t prf_id;
    uint8_t con_idx;
    uint16_t connhdl;
} bt_gatt_prf_t;

typedef union
{
    bt_attr_t attr_header;
    uint8_t attr_mem[BT_GATT_ATTR_MEM_LEN];
} bt_attr_mem_t;

typedef struct
{
    /// Attribute properties @see BT_GATT_PROP_NO_PROP
    uint8_t attr_prop;
    /// Attribute permissions @see BT_GATT_PERM_NO_PERM
    uint8_t attr_perm;
    /// Attribute declaration uuid type
    /// length value @see BT_GATT_UUID_16_LEN
    bool is_128_bits_attr_uuid;
    /// Attribute declaration uuid value
    bt_attr_uuid_t attr_uuid;
} bt_attr_info_t;

typedef struct
{
    bt_attr_t *attr_data; // [perm:1B][flags:1B][uuid:2B or 16B][attr_value:nB]
    uint16_t data_len;
    uint16_t inst_id;
} bt_gatt_attr_t;

typedef struct
{
    uint8_t service_inst_id;
    uint8_t con_idx;
    uint16_t connhdl;
} bt_gatt_svc_t;

typedef struct
{
    // Total attr count
    uint8_t attr_cnt;
    // Attribute and hdl map
    struct
    {
        const bt_gatt_attr_t *attr;
        uint16_t attr_hdl;
    } attr_hdl[BT_GATT_ARRAY_EMPTY];
} bt_gatt_attr_hdl_t;

typedef struct
{
    const bt_gatt_attr_t *service;
    int status;
    uint16_t start_handle;
    uint16_t end_handle;
    const bt_gatt_attr_hdl_t *p_attr_map;
} bt_gatts_reg_svc_cmp_t;

typedef struct
{
    void *conn_context;
    const bt_attr_t *service;
    uint8_t con_idx;
    uint8_t error_code;
    uint16_t mtu;
} bt_gatts_conn_param_t;

typedef bt_gatts_conn_param_t bt_gatts_conn_opened_t;
typedef bt_gatts_conn_param_t bt_gatts_conn_closed_t;
typedef bt_gatts_conn_param_t bt_gatts_mtu_changed_t;
typedef bt_gatts_conn_param_t bt_gatts_conn_updated_t;
typedef bt_gatts_conn_param_t bt_gatts_conn_encrypted_t;
typedef bt_gatts_conn_param_t bt_gatts_eatt_created_t;

typedef struct
{
    uint8_t con_idx;
    uint16_t value_offset;
    void *conn_context;
    const bt_attr_t *service;
    const bt_attr_t *character;
    const bt_gatt_attr_t *char_attr;
    bt_att_err_code_t rsp_error_code;
    uint32_t token;
} bt_gatts_char_read_t;

typedef struct
{
    uint8_t con_idx;
    uint16_t value_offset;
    void *conn_context;
    const bt_attr_t *service;
    const bt_attr_t *character;
    const bt_gatt_attr_t *char_attr;
    const uint8_t *value;
    uint16_t value_len;
    bt_att_err_code_t rsp_error_code;
    uint32_t token;
    uint8_t req_opcode;
} bt_gatts_char_write_t;

typedef struct
{
    uint8_t con_idx;
    uint16_t value_offset;
    void *conn_context;
    const bt_attr_t *service;
    const bt_attr_t *character;
    const bt_gatt_attr_t *char_attr;
    const bt_gatt_attr_t *desc_attr;
    bt_att_err_code_t rsp_error_code;
    uint32_t token;
} bt_gatts_desc_read_t;

typedef struct
{
    uint8_t con_idx;
    uint16_t value_offset;
    void *conn_context;
    const bt_attr_t *service;
    const bt_attr_t *character;
    const bt_gatt_attr_t *char_attr;
    const bt_gatt_attr_t *desc_attr;
    const uint8_t *value;
    uint16_t value_len;
    bt_att_err_code_t rsp_error_code;
    uint32_t token;
    uint8_t req_opcode;
} bt_gatts_desc_write_t;

typedef struct
{
    uint8_t con_idx;
    bt_att_err_code_t error_code;
    void *conn_context;
    const bt_attr_t *service;
    const bt_attr_t *character;
    const bt_gatt_attr_t *char_attr;
    const uint32_t *dummy;
} bt_gatts_indicate_cfm_t;

typedef union
{
    void *param_ptr;
    bt_gatts_reg_svc_cmp_t *reg_svc_cmp;
    bt_gatts_conn_param_t *conn_param;
    bt_gatts_conn_opened_t *opened;
    bt_gatts_conn_closed_t *closed;
    bt_gatts_char_read_t *char_read;
    bt_gatts_desc_read_t *desc_read;
    bt_gatts_char_write_t *char_write;
    bt_gatts_desc_write_t *desc_write;
    bt_gatts_indicate_cfm_t *confirm;
    bt_gatts_mtu_changed_t *mtu_changed;
    bt_gatts_conn_updated_t *conn_updated;
    bt_gatts_conn_encrypted_t *conn_encrypted;
    bt_gatts_eatt_created_t *eatt_created;
} bt_gatts_callback_param_t;

typedef struct
{
    bt_gatt_prf_t *prf;
    const void *conn_ctx;
    uint16_t mtu;
} bt_gattc_param_t;

typedef struct
{
    bt_gatt_prf_t *prf;
    const void *conn_ctx;
    uint8_t reason;
} bt_gattc_closed_t;

typedef bt_gattc_param_t bt_gattc_opened_t;
typedef bt_gattc_param_t bt_gattc_mtu_changed_t;
typedef bt_gattc_param_t bt_gattc_conn_encrypted_t;
typedef bt_gattc_param_t bt_gattc_conn_updated_t;
typedef bt_gattc_param_t bt_gattc_svc_changed_t;
typedef bt_gattc_param_t bt_gattc_eatt_created_t;

struct bt_gatt_peer_serv_t;

typedef struct
{
    uint16_t desc_uuid;
    uint16_t desc_handle;
} bt_gatt_peer_descriptor_t;

typedef struct
{
    bt_gatt_peer_descriptor_t *desc_list;
    uint8_t desc_is_discovered: 1;
    uint8_t desc_disc_pending: 1;
    uint8_t peer_not_exist: 1;
    uint8_t desc_count;
    uint8_t char_index;
    uint8_t end_hdl_offset;
    uint16_t char_prop;
    uint16_t char_value_handle;
} bt_gatt_peer_char_t;

typedef struct
{
    struct bt_gatt_peer_char_node_t *next;
} bt_gatt_peer_char_list_t;

typedef struct
{
    struct bt_gatt_peer_serv_t *incl_service;
    uint16_t incl_handle;
    uint16_t start_handle;
    uint16_t end_handle;
    uint16_t service_uuid; // set when 16-bit uuid
} bt_gatt_peer_include_t;

typedef struct bt_gatt_peer_serv_t
{
    bt_gatt_peer_include_t *incl_list;
    bt_gatt_peer_char_list_t char_list;
    uint8_t incl_is_discovered: 1;
    uint8_t incl_disc_pending: 1;
    uint8_t peer_not_exist: 1;
    uint8_t incl_count;
    uint8_t srvc_index;
    uint8_t count; // same uuid service count
    uint16_t service_uuid; // set when 16-bit uuid
    uint16_t attr_handle;
    uint16_t end_handle;
} bt_gatt_peer_serv_t;

typedef struct
{
    const void *conn_ctx;
    bt_gatt_peer_serv_t *service;
    bt_att_err_code_t error_code;
    uint8_t count; // same uuid service count
    uint16_t service_uuid; // set when 16-bit uuid
    uint8_t discover_cmpl;
} bt_gattc_service_t;

typedef struct
{
    const void *conn_ctx;
    bt_gatt_peer_serv_t *owner_service;
    bt_gatt_peer_serv_t *service;
    bt_att_err_code_t error_code;
    uint8_t discover_cmpl;
    uint16_t service_uuid; // set when 16-bit uuid
} bt_gattc_include_t;

typedef struct
{
    const void *conn_ctx;
    bt_gatt_peer_serv_t *service;
    bt_gatt_peer_char_t *character;
    bt_att_err_code_t error_code;
    uint16_t char_prop;
    uint16_t char_value_handle;
    uint16_t char_end_handle;
    uint8_t discover_cmpl;
    uint8_t discover_idx;
    uint8_t count; // same uuid characteristic count
    uint16_t char_uuid; // set when 16-bit uuid
    uint8_t uuid_le[BT_GATT_UUID_128_LEN]; // always set
} bt_gattc_character_t;

typedef struct
{
    const void *conn_ctx;
    bt_gatt_peer_char_t *owner_character;
    bt_att_err_code_t error_code;
    uint8_t discover_cmpl;
    uint16_t char_prop;
    uint16_t char_value_handle;
    uint16_t desc_handle;
    uint16_t desc_uuid; // set when 16-bit uuid
    const uint8_t *desc_uuid_le; // always set
} bt_gattc_descriptor_t;

typedef struct
{
    const void *conn_ctx;
    bt_gatt_peer_serv_t *service;
    bt_gatt_peer_char_t *character;
    bt_att_err_code_t error_code;
    uint16_t attr_handle;
    uint16_t value_len;
    const uint8_t *value;
} bt_gattc_char_read_rsp_t;

typedef struct
{
    const void *conn_ctx;
    bt_gatt_peer_serv_t *service;
    bt_gatt_peer_char_t *character;
    bt_att_err_code_t error_code;
    uint16_t desc_handle;
    uint16_t desc_uuid;
    uint16_t value_len;
    const uint8_t *value;
} bt_gattc_desc_read_rsp_t;

typedef struct
{
    const void *conn_ctx;
    bt_gatt_peer_serv_t *service;
    bt_gatt_peer_char_t *character;
    bt_att_err_code_t error_code;
    uint16_t attr_handle;
    bool prepare_mismatch;
    bool reliable_write_done;
    uint16_t value_offset;
    uint16_t value_write_len;
    const uint8_t *value_write;
    uint8_t req_opcode;
} bt_gattc_char_write_rsp_t;

typedef struct
{
    const void *conn_ctx;
    bt_gatt_peer_serv_t *service;
    bt_gatt_peer_char_t *character;
    bt_att_err_code_t error_code;
    uint16_t attr_handle;
    uint16_t desc_uuid;
    uint16_t value_write_len;
    const uint8_t *value_write;
    uint8_t req_opcode;
} bt_gattc_desc_write_rsp_t;

typedef struct
{
    const void *conn_ctx;
    bt_gatt_peer_serv_t *service;
    bt_gatt_peer_char_t *character;
    bt_att_err_code_t error_code;
} bt_gattc_write_exec_rsp_t;

typedef struct
{
    const void *conn_ctx;
    bt_gatt_peer_serv_t *service;
    bt_gatt_peer_char_t *character;
    bool is_indicate;
    uint16_t value_len;
    const uint8_t *value;
} bt_gattc_recv_notify_t;

typedef struct
{
    uint16_t char_uuid; // only for 16-bit uuid, 0 for 128-bit uuid
    uint8_t uuid_16_le[BT_GATT_UUID_16_LEN];
    const uint8_t *uuid_le;
} bt_gatt_peer_char_uuid_t;

typedef struct
{
    uint16_t service_uuid; // only for 16-bit uuid, 0 for 128-bit uuid
    uint8_t uuid_16_le[BT_GATT_UUID_16_LEN];
    const uint8_t *uuid_le;
} bt_gatt_peer_serv_uuid_t;

typedef struct
{
    uint16_t prf_size;
    uint16_t preferred_mtu;
    bool eatt_preferred;
    bool enc_required; // encrypt required to read/write peer service
} bt_gattc_cfg_t;

typedef union
{
    void *param_ptr;
    bt_gattc_opened_t *opened;
    bt_gattc_closed_t *closed;
    bt_gattc_mtu_changed_t *mtu_changed;
    bt_gattc_conn_encrypted_t *conn_encrypted;
    bt_gattc_conn_updated_t *conn_updated;
    bt_gattc_svc_changed_t *service_changed;
    bt_gattc_service_t *service;
    bt_gattc_include_t *incl_service;
    bt_gattc_character_t *character;
    bt_gattc_descriptor_t *descriptor;
    bt_gattc_char_read_rsp_t *char_read_rsp;
    bt_gattc_desc_read_rsp_t *desc_read_rsp;
    bt_gattc_char_write_rsp_t *char_write_rsp;
    bt_gattc_desc_write_rsp_t *desc_write_rsp;
    bt_gattc_write_exec_rsp_t *write_exec_rsp;
    bt_gattc_recv_notify_t *notify;
    bt_gatts_eatt_created_t *eatt_created;
} bt_gattc_callback_param_t;

typedef struct
{
    /// Reserved memery size pre-allocated when open
    uint16_t svc_size;
    /// Preferred mtu size
    uint16_t preferred_mtu;
    /// EATT preferred
    uint8_t eatt_preferred: 1;
    /// Add sdp record for btgatt
    uint8_t btgatt_add_sdp: 1;
    /// Activate service only when is accessed
    uint8_t activate_only_accessed: 1;
    /// Service instant id when same service
    uint8_t service_inst_id;
} bt_gatts_cfg_t;

typedef struct
{
    /// Descriptor handle
    uint16_t desc_hdl;
    /// UUID length
    uint8_t uuid_len;
    /// UUID
    uint8_t uuid[BT_GATT_UUID_128_LEN];
} bt_gattc_cache_desc_t;

typedef struct
{
    /// attribute handle
    uint16_t handle;
    /// Value handle
    uint16_t val_hdl;
    /// End handle
    uint16_t end_hdl;
    /// Characteristic properties
    uint8_t prop;
    /// Ext prop when set in @see prop
    uint8_t reliable_write: 1;
    uint8_t writable_aux: 1;
    /// UUID length
    uint8_t uuid_len;
    /// UUID
    uint8_t uuid[BT_GATT_UUID_128_LEN];
    /// Descriptor count
    uint8_t desc_count;
    /// Descriptor list
    const bt_gattc_cache_desc_t *p_desc;
} bt_gattc_cache_char_t;

typedef struct
{
    /// attribute handle
    uint16_t handle;
    /// included service start handle
    uint16_t start_hdl;
    /// included service  end handle
    uint16_t end_hdl;
    /// UUID length
    uint8_t uuid_len;
    /// UUID
    uint8_t uuid[BT_GATT_UUID_128_LEN];
} bt_gattc_cache_inc_svc_t;

typedef struct
{
    /// start handle
    uint16_t shdl;
    /// end handle
    uint16_t ehdl;
    /// UUID length
    uint8_t uuid_len;
    /// UUID
    uint8_t uuid[BT_GATT_UUID_128_LEN];
    /// Include service count
    uint8_t inc_svc_count;
    /// Characteristic count
    uint8_t char_count;
    /// Include service list
    const bt_gattc_cache_inc_svc_t *p_inc_svc;
    /// Characteristic list
    const bt_gattc_cache_char_t *p_char;
} bt_gattc_cache_svc_t;

typedef bool (*bt_gatt_server_cb)(bt_gatt_svc_t *svc, bt_gatts_event_t event, bt_gatts_callback_param_t param);
typedef bool (*bt_gatt_client_cb)(bt_gatt_prf_t *prf, bt_gattc_event_t event, bt_gattc_callback_param_t param);
