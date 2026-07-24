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
#ifndef __BTS_BLE_API_H__
#define __BTS_BLE_API_H__

#include "bts_ble_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void bts_ble_set_device_config(const bt_ble_dev_cfg_t *p_cfg);

void bts_ble_set_local_name(const char *name, uint8_t len);

uint8_t bts_ble_get_local_name(char *name_buf, uint8_t buf_len);

void bts_ble_get_local_addr(ble_bdaddr_t *addr);

int bts_ble_gap_start_connect(ble_bdaddr_t *addr, bts_ble_gap_own_type_e own_type);

void bts_ble_gap_cancel_connecting(void);

bool bts_ble_gap_is_connection_on(uint8_t conidx);

bool bts_ble_gap_is_any_connection_on(void);

uint8_t bts_ble_gap_connection_count(void);

bool bts_ble_gap_is_remote_dev_connected(const ble_bdaddr_t *p_addr);

uint16_t bts_ble_gap_get_conhdl_by_conidx(uint8_t conidx);

uint8_t bts_ble_gap_get_conidx_by_conhdl(uint16_t connhdl);

void bts_ble_gap_update_conn_param(uint8_t conidx, uint32_t min_intv_ms, uint32_t max_intv_ms,
                                   uint32_t superv_timeout_ms, uint16_t max_peripheral_latency);

uint16_t bts_ble_gap_get_conn_interval(uint8_t conidx);

void bts_ble_gap_update_conn_param_mode(bts_ble_conn_param_mode_e mode, bool isEnable);

void bts_ble_gap_conn_subrate_request(uint8_t conidx, uint16_t subrate_min, uint16_t subrate_max,
                                      uint16_t latency_max, uint16_t cont_num, uint16_t timeout);

int8_t bts_ble_gap_get_rssi(uint8_t conidx);

void bts_ble_gap_set_white_list(const ble_bdaddr_t *bdaddr, uint8_t size);

void bts_ble_gap_clear_white_list(void);

void bts_ble_gap_start_disconnect(uint8_t conidx);

void bts_ble_gap_disconnect_all(void);

uint8_t bts_ble_get_conidx_by_addr(const ble_bdaddr_t *peer_addr);

int bts_ble_gap_disconnect_by_addr(const ble_bdaddr_t *peer_addr);

void bts_ble_gap_clear_rpa_list(void);

void bts_ble_gap_set_rpa_list(uint8_t count, const bt_ble_resolving_item_t *item);

void bts_ble_gap_set_bonded_devs_rpa_list(void);

void bts_ble_gap_set_adv_interval(bt_ble_gap_adv_user_t adv_user, uint32_t interval_ms);

void bts_ble_gap_set_adv_tx_pwr_lvl(bts_ble_gap_adv_actv_t *adv, bts_ble_gap_adv_tx_pwr_lvl_e tx_pwr_lvl);

bool bts_ble_gap_adv_check_ibrt_allow(bt_ble_gap_adv_user_t user);

void bts_ble_gap_deregister_adv(bt_ble_gap_adv_user_t user);

bool bts_ble_gap_adv_is_allowed(void);

void bts_ble_gap_refresh_adv(void);

void bts_ble_gap_set_rpa_timeout(uint16_t rpa_timeout);

void bts_ble_gap_update_rpa_when(uint8_t adv_handle, bool adv_data_change, bool scan_rsp_change);

void bts_ble_gap_force_switch_adv(bts_ble_adv_switch_user_e user, bool enable_all);

void bts_ble_gap_custom_adv_write_data(bt_ble_gap_cus_adv_param_t *param);

void bts_ble_gap_custom_adv_start(bt_ble_gap_adv_activity_t actv_user);

void bts_ble_gap_custom_adv_set_filter_policy(bt_ble_gap_adv_activity_t actv_user, uint8_t filter_policy);

void bts_ble_gap_custom_adv_stop(bt_ble_gap_adv_activity_t actv_user);

bool bts_ble_gap_custom_adv_is_enabled(bt_ble_gap_adv_activity_t actv_user);

void bts_ble_gap_custom_adv_evt_cb_register(bt_ble_custom_adv_event_func cb);

void bts_ble_gap_register_data_fill_handle(bt_ble_gap_adv_user_t user, bts_ble_gap_adv_data_fill_func func, bool enable);

void bts_ble_gap_data_fill_enable(bt_ble_gap_adv_user_t user, bool enable);

void bts_ble_gap_set_adv_param(bts_ble_gap_adv_param_p_t *param);

void bts_ble_gap_adv_data_set_flags(bts_ble_gap_adv_param_t *adv_param, bool simu_bredr_support);

bool bts_ble_gap_adv_data_set_local_name(bts_ble_gap_adv_param_t *adv_param, const char *cust_le_name);

bool bts_ble_gap_adv_data_service_data(bts_ble_gap_dt_buf_t *buf, uint16_t service_uuid, const uint8_t *data, uint8_t len);

bool bts_ble_gap_adv_data_tx_power(bts_ble_gap_dt_buf_t *buf, uint8_t tx_power_level);

bool bts_ble_gap_adv_data_data_type(bts_ble_gap_dt_buf_t *buf, uint8_t type, const uint8_t *data, uint16_t len);

bool bts_ble_gap_adv_data_raw_data(bts_ble_gap_dt_buf_t *buf, const uint8_t *data, uint16_t len);

void bts_ble_gap_start_scan(bts_ble_scan_param_t *param);

void bts_ble_gap_stop_scan(void);

uint8_t bts_ble_gap_set_ecdh_key_pair(const uint8_t *p_sec_key_256, const uint8_t *p_pub_key_256);

void bts_ble_gap_adv_report_callback_register(bts_ble_scan_result_user_id_e id, bts_ble_adv_data_report_cb cb);

void bts_ble_gap_adv_report_callback_deregister(bts_ble_scan_result_user_id_e id);

void bts_ble_gap_conn_event_handler_register(bts_ble_conn_evt_report_cb cb);

void bts_ble_gap_conn_event_handler_deregister(void);

void bts_ble_set_tx_rx_pref_phy(uint32_t tx_pref_phy, uint32_t rx_pref_phy);

void bts_ble_gap_core_event_handler_register(bt_ble_core_evt_handler_func handler);

void bts_ble_gap_core_event_handler_unregister(bt_ble_core_evt_handler_func handler);

ble_bdaddr_t bts_ble_gap_get_local_identity_addr(uint8_t conidx);

const uint8_t *bts_ble_gap_get_adv_rpa_addr(uint8_t adv_hdl);

void bts_ble_gap_set_public_address(const bt_bdaddr_t *public_addr);

uint8_t bts_ble_gap_set_le_tx_pwr(uint16_t connhdl, int8_t tx_pwr);

int bts_ble_gap_start_authentication(uint8_t conidx, const bt_pairing_reqiure_t *p_req);

int bts_ble_gap_enable_link_encryption(uint8_t conidx, const uint8_t *ediv, const uint8_t *rand, const uint8_t *ltk);

int bts_ble_gap_send_pairing_response(uint8_t conidx, const bt_pairing_reqiure_t *p_req, uint8_t error);

int bts_ble_gap_send_input_data_confirm(uint8_t conidx, bt_ble_smp_input_type_e type, const bt_ble_smp_input_t *p_input);

void bts_ble_gap_smp_require_modify_cb_register(void (*ble_smp_require_modify)(uint16_t, bt_pairing_reqiure_t *));

void bts_ble_gap_set_local_irk(const bt_ble_gap_irk_t *irk);

void bts_ble_gap_get_local_irk(bt_ble_gap_irk_t *irk);

void bts_ble_gap_get_peer_irk_by_addr(const ble_bdaddr_t *addr, bt_ble_gap_irk_t *irk);

bool bts_ble_gap_get_peer_solved_addr(uint8_t conidx, ble_bdaddr_t *p_addr);

bts_ble_conn_timing_t bts_ble_get_connection_timing(uint8_t conidx);

const char *bts_ble_gap_get_peer_device_name(uint8_t conidx);

uint16_t bts_ble_gap_get_peer_lmp_version(uint8_t conidx);

void bts_ble_gap_get_conn_tx_pwr(uint8_t conidx, bts_ble_tx_object_e obj, bts_ble_phy_pwr_value_e phy);

void bts_ble_gap_tx_power_report_enable(uint8_t conidx, bool local_enable, bool remote_enable);

void bts_ble_gap_set_phy(uint8_t conidx, uint8_t tx_phy_bf, uint8_t rx_phy_bf, uint8_t phy_opt);

void bts_ble_gap_read_phy(uint8_t conidx);

void bts_ble_force_switch_adv(bt_ble_adv_switch_user_t user, bool enable);

#ifdef BLE_WALKIE_TALKIE
int bts_walkie_gap_init(bts_ble_walkie_gap_callback *cb);

int bts_walkie_gap_deinit();

uint8_t bts_walkie_gap_adv_create(bts_ble_walkie_gap_adv_param_t *param);

int bts_walkie_gap_adv_start(uint8_t adv_handle, uint32_t duartion);

int bts_walkie_gap_adv_stop(uint8_t adv_handle);

int bta_talkie_gap_adv_set_data(uint8_t adv_handle, uint8_t *data, uint8_t data_len);

int bts_walkie_gap_scan_creat(uint16_t interval_ms, uint16_t windows_ms, bool use_filter_list);

int bts_walkie_gap_scan_start(bool filter_duplicates, uint32_t duration_ms);

int bts_walkie_gap_scan_stop();

int bts_walkie_gap_pa_sync_create(const uint8_t *mac_addr);

int bts_walkie_gap_pa_sync_stop(uint16_t pa_sync_hdl);

int bts_walkie_gap_pa_set_data(uint8_t adv_handle, const uint8_t *pa_data, uint8_t data_len);

int bts_walkie_gap_set_white_list(uint8_t *mac_addr, uint8_t count);

int bts_walkie_gap_clear_white_list();

int bts_walkie_gap_set_mesh_list(uint8_t *mac_addr, uint8_t count);

int bts_walkie_gap_clear_mesh_list();
#endif  //BLE_WALKIE_TALKIE

#if (defined(BES_OTA) || defined(BES_OTA_BASIC))&& !defined(OTA_OVER_TOTA_ENABLED)
void bts_ble_ota_event_register(bts_ble_ota_event_cb cb);

void bts_ble_ota_event_unregister(void);

void bts_ble_ota_send_rx_cfm(uint8_t conidx);

bool bts_ble_ota_send_notification(uint8_t conidx, uint8_t *ptrData, uint32_t length);

bool bts_ble_ota_send_indication(uint8_t conidx, uint8_t *ptrData, uint32_t length);
#endif

#ifdef BLE_TOTA_ENABLED
void bts_ble_tota_event_register(bts_ble_tota_event_cb cb);

void bts_ble_tota_event_unregister(void);

bool bts_ble_tota_send_notification(uint8_t conidx, uint8_t *ptrData, uint32_t length);

bool bts_ble_tota_send_indication(uint8_t conidx, uint8_t *ptrData, uint32_t length);
#endif

#ifdef __AI_VOICE_BLE_ENABLE__
void bts_ble_ai_gatt_event_register(bts_ble_ai_event_cb cb);

void bts_ble_ai_gatt_data_send(bts_ble_ai_data_send_param_t *param);
#endif

#ifdef TILE_DATAPATH
void bts_ble_tile_event_cb_register(bts_ble_tile_event_cb cb);

void bts_ble_tile_send_via_notification(uint8_t conidx, uint8_t *ptrData, uint32_t length);
#endif

#ifdef CFG_APP_DATAPATH_SERVER
void bts_ble_datapath_server_event_handler_register(bts_ble_datapath_event_cb cb);

void bts_ble_datapath_server_send_data_via_notification(uint8_t conidx, uint8_t *ptrData, uint32_t length);

void bts_ble_datapath_server_send_data_via_indication(uint8_t conidx, uint8_t *ptrData, uint32_t length);
#endif

#if (CFG_APP_DATAPATH_CLIENT)
void bts_ble_datapath_client_control_notification(uint8_t conidx, bool isEnable);

void bts_ble_datapath_client_send_data_via_write_cmd(uint8_t conidx, uint8_t *ptrData, uint32_t length);

void bts_ble_datapath_client_send_data_via_write_req(uint8_t conidx, uint8_t *ptrData, uint32_t length);

void bts_ble_datapath_client_event_handler_register(void *callback);
#endif

#ifdef GFPS_ENABLED
void bts_ble_gfps_init(bts_ble_gap_adv_actv_prepare_func func);

void bts_ble_gfps_register_event_callback(bts_ble_gfps_event_cb func);

#ifdef SPOT_ENABLED
void bts_ble_spot_init(bts_ble_gap_adv_actv_prepare_func func);
#endif

void bts_ble_dult_init(bts_ble_dult_cb_t *cbs);

void bts_ble_gfps_send_beacon_data(uint8_t conidx, uint8_t *data, uint16_t length);

uint8_t bts_ble_gfps_l2cap_send(uint8_t conidx, uint8_t *ptrData, uint32_t length);

void bts_ble_gfps_l2cap_disconnect(uint8_t conidx);

void bts_ble_gfps_send_keybase_pairing(uint8_t conidx, uint8_t *data, uint16_t length);

void bts_ble_gfps_send_passkey(uint8_t conidx, const uint8_t *data, uint32_t length);

void bts_ble_gfps_send_additional_passkey(uint8_t conidx, const uint8_t *data, uint32_t length);

void bts_ble_gfps_send_naming_packet(uint8_t conidx, const uint8_t *data, uint32_t length);

#endif /* BLE_APP_GFPS */

#ifdef SWIFT_ENABLED
void bts_ble_swift_enter_pairing_mode(void);

void bts_ble_swift_exit_pairing_mode(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BTS_BLE_API__ */