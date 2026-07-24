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

#pragma once

#include "bt_gatt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 ****************************************************************************************
 *          ____  _           ____    _  _____ _____      _    ____ ___
 *         | __ )| |_ __ _   / ___|  / \|_   _|_   _|    / \  |  _ \_ _|
 *         |  _ \| __/ _` | | |  _  / _ \ | |   | |     / _ \ | |_) | |
 *         | |_) | || (_| | | |_| |/ ___ \| |   | |    / ___ \|  __/| |
 *         |____/ \__\__,_|  \____/_/   \_\_|   |_|   /_/   \_\_|  |___|
 *
 ****************************************************************************************
 */

/**
 * @brief       Send MTU exchange request to peer device
 * @param[in]   connhdl       Connection handle
 * @param[in]   pref_mtu_size Preferred MTU size
 * @return      0 if successful, otherwise error code
 */
int bta_gatt_send_mtu_exchange_req(uint16_t connhdl, uint16_t pref_mtu_size);

/**
 * @brief       Send MTU exchange response to peer device
 * @param[in]   connhdl       Connection handle
 * @param[in]   pref_mtu_size Preferred MTU size
 * @return      0 if successful, otherwise error code
 */
int bta_gatt_send_mtu_exchange_rsp(uint16_t connhdl, uint16_t pref_mtu_size);

/**
 * @brief       Connect EATT bearer channel
 * @param[in]   connhdl Connection handle
 * @return      0 if successful, otherwise error code
 */
int bta_gatt_connect_eatt_bearer(uint16_t connhdl);

/**
 * @brief       Disconnect EATT bearer channel
 * @param[in]   connhdl                      Connection handle
 * @param[in]   include_no_initiator_eatt    Whether to include non-initiator EATT
 * @return      0 if successful, otherwise error code
 */
int bta_gatt_disconnect_eatt_bearer(uint16_t connhdl, bool include_no_initiator_eatt);

/**
 * @brief       Check if GATT over BR/EDR is enabled
 * @return      true if enabled, false otherwise
 */
bool bta_gatt_over_bredr_is_enabled();

/**
 * @brief       Register state change callback for GATT over BR/EDR
 * @param[in]   cb Callback function to register
 */
void bta_gatt_over_bredr_register_state_cb(void (*cb)(const bt_bdaddr_t *addr, bool is_connected, uint8_t reason));

/**
 * @brief       Connect to peer device over BR/EDR
 * @param[in]   addr Bluetooth address of peer device
 * @return      0 if successful, otherwise error code
 */
int bta_gatt_over_bredr_connect(const bt_bdaddr_t *addr);

/**
 * @brief       Disconnect from peer device over BR/EDR
 * @param[in]   addr Bluetooth address of peer device
 * @return      0 if successful, otherwise error code
 */
int bta_gatt_over_bredr_disconnect(const bt_bdaddr_t *addr);

/**
 * @brief       Check if connected to peer device over BR/EDR
 * @param[in]   addr Bluetooth address of peer device
 * @return      true if connected, false otherwise
 */
bool bta_gatt_over_bredr_is_connected(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Gatt server API.
 ****************************************************************************************
 */

/**
 * @brief       Build GATT attribute from type, info and memory
 * @param[in]   type     Attribute type
 * @param[in]   info     Attribute information
 * @param[in]   attr_mem Attribute memory
 * @return      Built GATT attribute
 */
bt_gatt_attr_t bta_gatts_build_attr(bt_attr_type_t type, const bt_attr_info_t *info, bt_attr_mem_t *attr_mem);

/**
 * @brief       Register GATT service
 * @param[in]   service    Service attributes
 * @param[in]   attr_count Number of attributes
 * @param[in]   cb         Callback function
 * @param[in]   cfg        Configuration parameters
 * @return      0 if successful, otherwise error code
 */
int bta_gatts_register_service(const bt_gatt_attr_t *service, uint8_t attr_count, bt_gatt_server_cb cb, const bt_gatts_cfg_t *cfg);

/**
 * @brief       Unregister GATT service
 * @param[in]   service Service attributes to unregister
 * @return      0 if successful, otherwise error code
 */
int bta_gatts_unregister_service(const bt_gatt_attr_t *service);

/**
 * @brief       Get GATT service by connection handle and service UUID
 * @param[in]   connhdl        Connection handle
 * @param[in]   service        Service UUID
 * @param[in]   service_inst_id Service instance ID
 * @return      Pointer to service structure or NULL if not found
 */
bt_gatt_svc_t *bta_gatts_get_service(uint16_t connhdl, const uint8_t *service, uint8_t service_inst_id);

/**
 * @brief       Control GATT service visibility
 * @param[in]   service        Service attributes
 * @param[in]   service_uuid_16 16-bit service UUID
 * @param[in]   visible        Visibility flag
 * @param[in]   connhdl        Connection handle
 * @return      0 if successful, otherwise error code
 */
int bta_gatts_control_service(const bt_gatt_attr_t *service, uint16_t service_uuid_16, bool visible, uint16_t connhdl);

/**
 * @brief       Send read response to GATT client
 * @param[in]   connhdl   Connection handle
 * @param[in]   token     Request token
 * @param[in]   error_code Error code
 * @param[in]   rsp_data  Response data
 * @param[in]   data_len  Length of response data
 * @return      0 if successful, otherwise error code
 */
int bta_gatts_send_read_rsp(uint16_t connhdl, uint32_t token, uint8_t error_code, const uint8_t *rsp_data, uint16_t data_len);

/**
 * @brief       Send write response to GATT client
 * @param[in]   connhdl   Connection handle
 * @param[in]   token     Request token
 * @param[in]   error_code Error code
 * @return      0 if successful, otherwise error code
 */
int bta_gatts_send_write_rsp(uint16_t connhdl, uint32_t token, uint8_t error_code);

/**
 * @brief       Send characteristic value notification/indication
 * @param[in]   connhdl   Connection handle
 * @param[in]   is_ntf    True for notification, false for indication
 * @param[in]   character Characteristic to send
 * @param[in]   data      Data to send
 * @param[in]   len       Length of data
 * @return      0 if successful, otherwise error code
 */
int bta_gatts_send_character_value(uint16_t connhdl, bool is_ntf, const bt_gatt_char_notify_t *character, const uint8_t *data, uint16_t len);

/**
 ****************************************************************************************
 * @brief       Gatt client API.
 ****************************************************************************************
 */

/**
 * @brief       Register GATT client profile
 * @param[in]   cb  Callback function
 * @param[in]   cfg Configuration parameters
 * @return      Profile ID if successful, otherwise 0xFF
 */
uint8_t bta_gattc_register_profile(bt_gatt_client_cb cb, const bt_gattc_cfg_t *cfg);

/**
 * @brief       Unregister GATT client profile
 * @param[in]   prf_id Profile ID to unregister
 * @return      0 if successful, otherwise error code
 */
int bta_gattc_unregister_profile(uint8_t prf_id);

/**
 * @brief       Get GATT client profile by ID and connection handle
 * @param[in]   prf_id  Profile ID
 * @param[in]   connhdl Connection handle
 * @return      Pointer to profile structure or NULL if not found
 */
bt_gatt_prf_t *bta_gattc_get_profile(uint8_t prf_id, uint16_t connhdl);

/**
 * @brief       Get characteristic UUID information
 * @param[in]   c      Pointer to the peer characteristic structure
 * @param[out]  param  Structure to receive the characteristic UUID information
 */
void bta_gattc_get_character_uuid(const bt_gatt_peer_char_t *c, bt_gatt_peer_char_uuid_t *param);

/**
 * @brief       Get service UUID information
 * @param[in]   s      Service structure
 * @param[out]  param  UUID parameter structure to fill
 */
void bta_gattc_get_service_uuid(const bt_gatt_peer_serv_t *s, bt_gatt_peer_serv_uuid_t *param);

/**
 * @brief       Discover all primary services on peer device
 * @param[in]   prf    GATT profile instance
 * @return      0 on success, negative value on error
 */
int bta_gattc_discover_all_primary_services(bt_gatt_prf_t *prf);

/**
 * @brief       Discover service by UUID
 * @param[in]   prf          GATT profile instance
 * @param[in]   service_uuid 16-bit UUID (if applicable)
 * @param[in]   uuid_128_le  128-bit UUID in little-endian format (optional)
 * @return      0 on success, negative value on error
 */
int bta_gattc_discover_service(bt_gatt_prf_t *prf, uint16_t service_uuid, const uint8_t *uuid_128_le);

/**
 * @brief       Discover all included services of a given service
 * @param[in]   prf   GATT profile instance
 * @param[in]   s     Peer service structure
 * @return      0 on success, negative value on error
 */
int bta_gattc_discover_all_include_services(bt_gatt_prf_t *prf, bt_gatt_peer_serv_t *s);

/**
 * @brief       Discover all characteristics within a given service
 * @param[in]   prf   GATT profile instance
 * @param[in]   s     Peer service structure
 * @return      0 on success, negative value on error
 */
int bta_gattc_discover_all_characters(bt_gatt_prf_t *prf, bt_gatt_peer_serv_t *s);

/**
 * @brief       Discover all descriptors of a given characteristic
 * @param[in]   prf   GATT profile instance
 * @param[in]   c     Peer characteristic structure
 * @return      0 on success, negative value on error
 */
int bta_gattc_discover_all_descriptors(bt_gatt_prf_t *prf, bt_gatt_peer_char_t *c);

/**
 * @brief       Read characteristic value
 * @param[in]   prf   GATT profile instance
 * @param[in]   c     Peer characteristic structure
 * @return      0 on success, negative value on error
 */
int bta_gattc_read_character_value(bt_gatt_prf_t *prf, bt_gatt_peer_char_t *c);

/**
 * @brief       Write characteristic value (request)
 * @param[in]   prf   GATT profile instance
 * @param[in]   c     Peer characteristic structure
 * @param[in]   data  Data buffer to write
 * @param[in]   len   Length of data
 * @return      0 on success, negative value on error
 */
int bta_gattc_write_character_value(bt_gatt_prf_t *prf, bt_gatt_peer_char_t *c, const uint8_t *data, uint16_t len);

/**
 * @brief       Write characteristic value without response
 * @param[in]   prf          GATT profile instance
 * @param[in]   c            Peer characteristic structure
 * @param[in]   data         Data buffer to write
 * @param[in]   len          Length of data
 * @param[in]   signed_write Use signed write command
 * @return      0 on success, negative value on error
 */
int bta_gattc_write_character_command(bt_gatt_prf_t *prf, bt_gatt_peer_char_t *c, const uint8_t *data, uint16_t len, bool signed_write);

/**
 * @brief       Perform prepare write on characteristic (reliable write)
 * @param[in]   prf     GATT profile instance
 * @param[in]   c       Peer characteristic structure
 * @param[in]   offset  Write offset
 * @param[in]   data    Data buffer
 * @param[in]   len     Data length
 * @return      0 on success, negative value on error
 */
int bta_gattc_reliable_write_character(bt_gatt_prf_t *prf, bt_gatt_peer_char_t *c, uint16_t offset, const uint8_t *data, uint16_t len);

/**
 * @brief       Execute reliable write sequence
 * @param[in]   prf      GATT profile instance
 * @param[in]   c        Peer characteristic structure
 * @param[in]   execute  True to commit writes, false to cancel
 * @return      0 on success, negative value on error
 */
int bta_gattc_reliable_write_execute(bt_gatt_prf_t *prf, bt_gatt_peer_char_t *c, bool execute);

/**
 * @brief       Write descriptor value
 * @param[in]   prf       GATT profile instance
 * @param[in]   c         Peer characteristic structure
 * @param[in]   desc_uuid Descriptor UUID
 * @param[in]   data      Data buffer
 * @param[in]   len       Data length
 * @return      0 on success, negative value on error
 */
int bta_gattc_write_descriptor_value(bt_gatt_prf_t *prf, bt_gatt_peer_char_t *c, uint16_t desc_uuid, const uint8_t *data, uint16_t len);

/**
 * @brief       Read descriptor value
 * @param[in]   prf       GATT profile instance
 * @param[in]   c         Peer characteristic structure
 * @param[in]   desc_uuid Descriptor UUID
 * @return      0 on success, negative value on error
 */
int bta_gattc_read_descriptor_value(bt_gatt_prf_t *prf, bt_gatt_peer_char_t *c, uint16_t desc_uuid);

/**
 * @brief       Read characteristic value by UUID in service range
 * @param[in]   prf         GATT profile instance
 * @param[in]   serv_shd    Service start handle
 * @param[in]   serv_ehdl   Service end handle
 * @param[in]   char_uuid   Characteristic UUID
 * @param[in]   uuid_128_le 128-bit UUID(optional)
 * @return      0 on success, negative value on error
 */
int bta_gattc_read_character_by_uuid(bt_gatt_prf_t *prf, uint16_t serv_shd, uint16_t serv_ehdl, uint16_t char_uuid, const uint8_t *uuid_128_le);

/**
 * @brief       Read multiple characteristic values
 * @param[in]   prf           GATT profile instance
 * @param[in]   multi_var_req Use Read Multiple Variable Value Request
 * @param[in]   count         Number of handles
 * @param[in]   handles       Handle list
 * @param[out]  value_lens    Returned value lengths
 * @return      0 on success, negative value on error
 */
int bta_gattc_read_multi_character_values(bt_gatt_prf_t *prf, bool multi_var_req, uint16_t count, uint16_t *handles, uint16_t *value_lens);

/**
 * @brief       Restore cached service list for a connected device
 * @param[in]   connhdl     Connection handle
 * @param[in]   svc_count   Number of cached services
 * @param[in]   p_svc_list  Service cache list
 * @return      0 on success, negative value on error
 */
int bta_gattc_restore_service(uint16_t connhdl, uint8_t svc_count, const bt_gattc_cache_svc_t *p_svc_list);

#ifdef __cplusplus
}
#endif