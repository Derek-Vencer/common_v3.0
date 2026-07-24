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
#ifndef __BLE_GATT_I_H__
#define __BLE_GATT_I_H__

#include "gatt_service.h"

/**
 * DEFINES
 *
 *
 */
#define GATT_SCCD_SET_BROADCAST     (0x0001) // default value is 0x0000

/**
 * TYPEDEFINES
 *
 *
 */

struct attr_context_t;

typedef struct
{
    uint8_t error_code;
    uint8_t req_opcode;
    uint16_t err_handle;
} att_rsp_header_t;

typedef struct
{
    att_rsp_header_t head;
    const uint8_t *pdu_data;
    uint16_t pdu_len;
} att_response_t;

typedef struct att_conn_item_t att_conn_item_t;
typedef struct gatt_proc_node_t gatt_proc_node_t;
typedef struct gatt_service_t gatt_service_t;
typedef struct gatt_include_t gatt_include_t;
typedef struct gatt_character_t gatt_character_t;
typedef struct gatt_descriptor_t gatt_descriptor_t;

/**
 * The exponent field is only used on integer format types. The exponent
 * field has type sint8.
 *
 *      actual value = Characteristic Value * 10^exponent
 *
 * The namespace field is used to identify the organization. The description
 * field is an enumerated value from the organization identified by the
 * namespace field.
 *
 */

typedef struct
{
    uint8_t format;
    uint8_t exponet;
    uint16_t unit;
    uint8_t name_space;
    uint16_t description;
} __attribute__((packed)) gatt_present_format_t;

typedef struct
{
    uint8_t is_initiator: 1;
    uint8_t is_eatt: 1;
    uint8_t is_mtu_exchanged: 1;
    uint8_t tx_pending: 1;
    uint8_t for_prep_write: 1;
    uint8_t rxrm_pending: 1;
    uint8_t rxwp_pending: 1;
} __attribute__((packed)) att_bearer_flag_t;

typedef struct att_bearer_t
{
    struct att_bearer_t *next;
    att_conn_item_t *conn;
    att_bearer_flag_t flag;
    uint8_t bearer_id;
    uint8_t rsp_wait_timer;
    int32_t free_credits;
    uint16_t att_mtu;
    uint16_t connhdl;
    uint32_t l2cap_handle;
    uint32_t defer_token;
} att_bearer_t;

typedef struct
{
    att_bearer_t *next;
} att_bearer_q_t;

typedef struct gatt_srv_user
{
    struct gatt_srv_user *next;
    uint8_t user_lid;
    uint8_t reg_svc_num;
    gatt_server_callback_t svc_callback;
} gatt_srv_usr_t;

typedef struct
{
    gatt_srv_usr_t *next;
} gatt_srv_usr_list_t;

typedef struct gatt_svc_node_t
{
    struct gatt_svc_node_t *next;
    gatt_service_t *service;
    uint16_t reported_mtu;
    uint8_t conn_open_reported: 1;
    uint8_t encrypt_reported: 1;
    gatt_svc_t svc_head;
} gatt_svc_node_t;

typedef struct
{
    gatt_svc_node_t *next;
} gatt_svc_list_t;

typedef struct gatt_peer_srvc_node_t
{
    struct gatt_peer_srvc_node_t *next;
    /// Record prf id for service event [1, GATT_PRF_LAST_ID]
    uint32_t prf_interest_bf;
    /// Service mem for all same uuid service
    gatt_peer_service_t service[GATT_ARRAY_SIZE_1];
} gatt_peer_srvc_node_t;

typedef struct
{
    struct gatt_peer_srvc_node_t *next;
    struct gatt_peer_srvc_node_t *tail;
} gatt_peer_srvc_list_t;

typedef struct gatt_peer_char_node_t
{
    struct gatt_peer_char_node_t *next;
    uint8_t count; // same uuid char count
    uint16_t char_uuid; // set when 16-bit uuid
    gatt_peer_character_t character[GATT_ARRAY_SIZE_1];
    /// This member only exist in the first discovered char
    /// of the total char node list, memery save usage
    /// gatt_peer_service_t *service[GATT_ARRAY_EMPTY];
} gatt_peer_char_node_t;

typedef struct att_prep_write_t
{
    struct att_prep_write_t *next;
    uint16_t attr_handle;
    uint16_t value_offset;
    uint16_t value_len;
} att_prep_write_t;

typedef struct
{
    att_prep_write_t *next;
} att_prep_wr_q_t;

typedef struct
{
    struct single_link_node_t node;
    struct pp_buff *ppb;
    uint8_t bearer_id;
} att_rx_req_node_t;

typedef struct gatt_prf_reg_t
{
    struct gatt_prf_reg_t *next;
    uint8_t multi_device_supp: 1;
    uint8_t eatt_preferred: 1;
    uint8_t enc_required: 1;
    uint8_t connected_count;
    uint8_t prf_id;
    uint8_t prf_size_div_4;
    uint16_t preferred_mtu;
    gatt_profile_callback_t profile_callback;
} gatt_prf_reg_t;

typedef struct
{
    gatt_prf_reg_t *next;
} gatt_prf_reg_list_t;

typedef struct gatt_prf_node_t
{
    struct gatt_prf_node_t *next;
    gatt_prf_reg_t *reg;
    gatt_prf_t prf_head;
} gatt_prf_node_t;

typedef struct
{
    gatt_prf_node_t *next;
} gatt_prf_list_t;

typedef struct att_conn_item_t
{
    gap_conn_item_t head;
    gatt_prf_list_t prf_list;
    gatt_svc_list_t svc_list;
    att_bearer_q_t bearer_q;
    att_prep_wr_q_t prep_wr_q;
    uint8_t bearer_id_seed;
    uint8_t last_tx_bearer;
    uint8_t bredr_att_added: 1;
    uint8_t use_prev_tx_bearer: 1;
    uint8_t mtu_ready_to_report: 1;
    uint8_t cli_mtu_exchanged: 1;
    uint8_t srv_mtu_exch_ongoing: 1;
    uint8_t user_profile_started: 1;
    uint8_t profile_is_allocated: 1;
    uint8_t srvc_changed_ongoing: 1;
    // Every proc in different queue own a bearer id
    struct single_link_head_t rx_req_pendq;
    struct single_link_head_t req_ind_tx_q;
    struct single_link_head_t cmd_ntf_tx_q;
    struct single_link_head_t cmd_ntf_tx_wait_done_q;
} att_conn_item_t;

typedef struct gatt_multi_notify_item_t
{
    union
    {
        gatt_multi_notify_val_t notify;
        struct
        {
            const uint8_t *character;
            const uint8_t *value;
            uint16_t value_len;
            uint8_t char_instance_id;
            const uint8_t *service;
            uint8_t service_inst_id;
            const uint32_t *dummy;
        };
    };
    uint16_t cccd_config;
    gatt_service_t *s;
    gatt_character_t *c;
} gatt_multi_notify_item_t;

typedef struct gattc_write_attribute
{
    gatt_prf_t *prf;
    gatt_peer_character_t *c;
    union
    {
        uint16_t desc_uuid;
        uint16_t attr_handle;
    };
    bool use_write_cmd;
    bool signed_write;
    uint16_t data_len;
    uint16_t offset;
    const uint8_t *data;
} gattc_write_attr_val_t;

typedef struct gatts_register_service
{
    union
    {
        uint8_t user_lid;
        gatt_server_callback_t cb;
    };
    const gatt_attribute_t *service;
    uint16_t attr_count;
    union
    {
        gatts_cfg_t srvc_cfg;
        gatts_usr_cfg_t user_cfg;
    };
} gatts_reg_service_t;

typedef struct gattc_reg_client
{
    gatt_profile_callback_t cb;
    gattc_cfg_t cfg;
} gattc_reg_cli_t;

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * FUNCTIONS DECLARATIONS
 *
 *
 */

/**
 * @brief Get gatt init configurations
 *
 * @param[in] cfg      GAP configurations to fill with
 *
 */
void gatt_get_config(gap_config_t *cfg);

/**
 * @brief Get characteristic declaration's 16 bits uuid
 *
 * @param[in] character
 *                     Character specified to get 16 bits uuid @see GATT_DECL_CHAR for more informations
 *
 * @return uint16_t    The 16 bits uuid to specified characteristic, zero means not 16 bits uuid
 */
uint16_t gatts_get_char_byte_16_bit_uuid(const uint8_t *character);

/**
 * @brief Get characteristic declaration's uuid's 16 bits part value
 *
 * @param[in] character
 *                     Character specified to get uuid 16 bits part @see GATT_DECL_CHAR for more informations
 *
 * @return uint16_t    The 16 bits part of uuid to specified characteristic
 */
uint16_t gatts_get_char_byte_16_bit_part_uuid(const uint8_t *character);

/**
 * @brief Get gatt characteristic attribute's uuid's 16 bits part value
 *
 * @param[in] attr     Attribute specified to get uuid 16 bits part @see gatt_attribute_t for more informations
 *
 * @return uint16_t    The 16 bits part of uuid to specified gatt characteristic attribute
 */
uint16_t gatts_get_character_16_bit_part_uuid(const gatt_attribute_t *attr);

/**
 * @brief Get gatt service attribute's uuid's 16 bits part value
 *
 * @param[in] attr     Attribute specified to get uuid 16 bits part @see gatt_attribute_t for more informations
 *
 * @return uint16_t    The 16 bits part of uuid to specified gatt characteristic attribute
 */
uint16_t gatts_get_service_16_bit_part_uuid(const gatt_attribute_t *attr);

/**
 * @brief Construct gatt connection bitfield
 *
 * @param[in] con_idx  Connection local index
 *
 * @return uint32_t    Connection bitfield
 */
uint32_t gatt_conn_bf(uint8_t con_idx);

/**
 * @brief Get att protocol connection context by connection handle
 *
 * @param[in] connhdl  Connection handle
 *
 * @return att_conn_item_t*
 */
att_conn_item_t *att_get_conn_item(uint16_t connhdl);

/**
 * @brief Get att protocol connection context by bt address
 *
 * @param[in] peer_addr
 *                     Peer BDAddress
 *
 * @return att_conn_item_t*
 */
att_conn_item_t *att_get_conn_by_bt_address(const bt_bdaddr_t *peer_addr);

/**
 * @brief Get att protocol connection context by ble address
 *
 * @param[in] peer_type
 *                     Peer address type
 * @param[in] peer_addr
 *                     Peer BDAddress
 *
 * @return att_conn_item_t*
 */
att_conn_item_t *att_get_conn_by_le_address(bt_addr_type_t peer_type, const bt_bdaddr_t *peer_addr);

#if defined(__cplusplus)
}
#endif
#endif /* __BLE_GATT_I_H__ */