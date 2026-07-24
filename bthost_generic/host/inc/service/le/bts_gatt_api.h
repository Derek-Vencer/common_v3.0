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
#ifndef __BTS_GATT_API_H__
#define __BTS_GATT_API_H__

#include "bts_gatt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

bt_gatt_attr_t bts_gatts_build_attribute(bt_attr_type_t type, const bt_attr_info_t *info, bt_attr_mem_t *attr_mem);

int bts_gatts_register_service(const bt_gatt_attr_t *service, uint16_t attr_count, bt_gatt_server_cb cb, const bt_gatts_cfg_t *cfg);

int bts_gatts_unregister_service(const bt_gatt_attr_t *service);

bt_gatt_svc_t *bts_gatts_get_service(uint16_t connhdl, const uint8_t *service, uint8_t service_inst_id);

int bts_gatts_control_service(const bt_gatt_attr_t *service, uint16_t service_uuid_16, bool visible, uint16_t connhdl);

int bts_gatts_send_read_rsp(uint16_t connhdl, uint32_t token, uint8_t error_code, const uint8_t *rsp_data, uint16_t data_len);

int bts_gatts_send_write_rsp(uint16_t connhdl, uint32_t token, uint8_t error_code);

int bts_gatts_send_character_value(uint16_t connhdl, bool is_ntf, const bt_gatt_char_notify_t *character, const uint8_t *data, uint16_t len);

int bts_gatts_send_char_value_by_handle(uint16_t connhdl, bool is_ntf, const bt_gatt_char_ntf_hdl_t *character);

uint16_t bts_gatts_get_char_value_handle(const uint8_t *service, uint8_t service_inst_id, const uint8_t *character, uint8_t char_inst_id);

uint8_t bts_gattc_register_profile(bt_gatt_client_cb cb, const bt_gattc_cfg_t *cfg);

int bts_gattc_unregister_profile(uint8_t prf_id);

bt_gatt_prf_t *bts_gattc_get_profile(uint8_t prf_id, uint16_t connhdl);

void bts_gattc_get_character_uuid(const bt_gatt_peer_char_t *c, bt_gatt_peer_char_uuid_t *param);

void bts_gattc_get_service_uuid(const bt_gatt_peer_serv_t *s, bt_gatt_peer_serv_uuid_t *param);

int bts_gattc_discover_all_primary_services(bt_gatt_prf_t *prf);

int bts_gattc_discover_service(bt_gatt_prf_t *prf, uint16_t service_uuid, const uint8_t *uuid_128_le);

int bts_gattc_discover_all_include_services(bt_gatt_prf_t *prf, bt_gatt_peer_serv_t *s);

int bts_gattc_discover_all_characters(bt_gatt_prf_t *prf, bt_gatt_peer_serv_t *s);

int bts_gattc_discover_all_descriptors(bt_gatt_prf_t *prf, bt_gatt_peer_char_t *c);

int bts_gattc_read_character_value(bt_gatt_prf_t *prf, bt_gatt_peer_char_t *c);

int bts_gattc_write_character_value(bt_gatt_prf_t *prf, bt_gatt_peer_char_t *c, const uint8_t *data, uint16_t len);

int bts_gattc_write_character_command(bt_gatt_prf_t *prf, bt_gatt_peer_char_t *c, const uint8_t *data, uint16_t len, bool signed_write);

int bts_gattc_reliable_write_character(bt_gatt_prf_t *prf, bt_gatt_peer_char_t *c, uint16_t offset, const uint8_t *data, uint16_t len);

int bts_gattc_reliable_write_execute(bt_gatt_prf_t *prf, bt_gatt_peer_char_t *c, bool execute);

int bts_gattc_write_descriptor_value(bt_gatt_prf_t *prf, bt_gatt_peer_char_t *c, uint16_t desc_uuid, const uint8_t *data, uint16_t len);

int bts_gattc_read_descriptor_value(bt_gatt_prf_t *prf, bt_gatt_peer_char_t *c, uint16_t desc_uuid);

int bts_gattc_read_character_by_handle(bt_gatt_prf_t *prf, uint16_t char_value_handle, bool long_value);

int bts_gattc_read_character_by_uuid(bt_gatt_prf_t *prf, uint16_t serv_shd, uint16_t serv_ehdl, uint16_t char_uuid, const uint8_t *uuid_128_le);

int bts_gattc_read_descriptor_by_handle(bt_gatt_prf_t *prf, uint16_t desc_handle, bool long_value);

int bts_gattc_read_multi_char_values(bt_gatt_prf_t *prf, bool multi_var_req, uint16_t count, uint16_t *handles, uint16_t *value_lens);

int bts_gattc_write_character_by_handle(bt_gatt_prf_t *prf, uint16_t char_value_handle, const uint8_t *data, uint16_t len, bool write_cmd);

int bts_gattc_write_descriptor_by_handle(bt_gatt_prf_t *prf, uint16_t desc_handle, const uint8_t *data, uint16_t len);

int bts_gattc_prepare_write_by_handle(bt_gatt_prf_t *prf, uint16_t attr_handle, uint16_t offset, const uint8_t *data, uint16_t len);

int bts_gattc_prepare_write_execute(bt_gatt_prf_t *prf, bool execute);

int bts_gattc_restore_service(uint16_t connhdl, uint8_t svc_count, const bt_gattc_cache_svc_t *p_svc_list);

uint16_t bts_gatt_get_connhdl_from_context(const void *conn_ctx);

int bts_gatt_start_att_mtu_exchange(uint16_t connhdl, uint16_t pref_mtu_size);

int bts_gatt_send_mtu_exchange_rsp(uint16_t connhdl, uint16_t pref_mtu_size);

int bts_gatt_create_att_bearer_over_bredr(uint16_t connhdl);

bool bts_gatt_is_over_bredr_enabled(void);

bool bts_gatt_is_att_bearer_over_bredr_connected(uint16_t connhdl);

int bts_gatt_disconnect_att_bearers_over_bredr(uint16_t connhdl, bool include_no_initiator_att, uint8_t reason);

int bts_gatt_create_eatt_bearer(uint16_t connhdl);

int bts_gatt_disconnect_eatt_bearers(uint16_t connhdl, bool include_no_initiator_eatt);

void bts_gatt_over_bredr_register_state_cb(void (*cb)(const bt_bdaddr_t *addr, bool is_connected, uint8_t reason));

int bts_gatt_over_bredr_connect(const bt_bdaddr_t *addr);

int bts_gatt_over_bredr_disconnect(const bt_bdaddr_t *addr);

#ifdef __cplusplus
}
#endif

#endif /* __BTS_GATT_API_H__ */