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
#ifndef __BLE_GAP_I_H__
#define __BLE_GAP_I_H__

#include "gap_service.h"
#include "hci_i.h"

/* DEFINES */

/* TYPEDEFINES */
typedef struct
{
    uint32_t adv_interval_min;
    uint32_t adv_interval_max;
    uint16_t adv_event_props;
    uint8_t own_addr_type;
    uint8_t peer_type;
    uint8_t adv_channel_map;
    uint8_t adv_filter_policy;
    bt_bdaddr_t local_addr;
    bt_bdaddr_t peer_addr;
    int8_t adv_tx_power;
    uint8_t primary_adv_phy;
    uint8_t primary_coded_phy_prefer;
    uint8_t secondary_adv_phy;
    uint8_t secondary_coded_phy_prefer;
    uint8_t secondary_adv_max_skip;
    uint8_t scan_req_notify_enable;
} gap_adv_set_param_t;

typedef struct
{
    bool adv_params_set;
    bool adv_enabled;
    uint8_t adv_handle;
    bool legacy_advertising;
    int8_t selected_tx_power;
    bool fast_advertising;
    bool continue_advertising;
    bool start_bg_advertising;
    bool directed_advertising;
    uint8_t activity_priority;
    uint8_t adv_stop_timer;
    bool pa_is_enabled;
    uint16_t duration_10ms;
    gap_dt_buf_t adv_data;
    gap_dt_buf_t scan_rsp_data;
    // Keep it until remove adv set
    gap_decision_t decision_data;
    gap_adv_callback_t adv_callback;
    gap_adv_set_param_t set_param;
    gap_adv_timing_t timing_cfg;
} gap_advertising_t;

typedef struct
{
    bool scan_enabled;
    bool legacy_scanning;
    bool background_scan;
    bool initiating_scan;
    bt_addr_type_t own_addr_type;
    uint8_t scan_handle;
    uint8_t scan_stop_timer;
    uint16_t period_1_28s;
    uint16_t duration_10ms;
    uint16_t scan_interval_slots;
    uint16_t scan_interval_coded;
    uint16_t scan_window_slots;
    uint16_t scan_window_coded;
    gap_scan_param_t param;
    gap_scan_callback_t scan_callback;
} gap_scanning_t;

typedef struct
{
    bool legacy_initiating;
    bool is_conn_creating;
    bool is_direct_initiating;
    uint8_t peer_type;
    bt_addr_type_t own_addr_type;
    bt_bdaddr_t peer_addr;
    uint32_t init_proc_timeout_ms;
    bool timeout_and_restart_initiating;
    bool start_bg_initiating;
    bool peer_is_ble_audio;
    uint8_t init_handle;
    uint8_t init_stop_timer;
    uint8_t pa_adv_handle;
    uint8_t sd_subevent;
    uint16_t scan_interval_slots;
    uint16_t scan_interval_coded;
    uint16_t scan_window_slots;
    uint16_t scan_window_coded;
    uint16_t min_conn_interval_1_25ms;  // 0x06 to 0x0C80 * 1.25ms, 7.5ms to 4s
    uint16_t max_conn_interval_1_25ms;  // 0x06 to 0x0C80 * 1.25ms, 7.5ms to 4s
    uint16_t min_conn_interval_coded;   // 0x06 to 0x0C80 * 1.25ms, 7.5ms to 4s
    uint16_t max_conn_interval_coded;   // 0x06 to 0x0C80 * 1.25ms, 7.5ms to 4s
    uint16_t superv_timeout_10ms;       // 100ms to 32s
    uint16_t min_ce_length_slots;       // min len of connection event, 0x00 to 0xFFFF * 0.625ms
    uint16_t max_ce_length_slots;       // max len of connection event, 0x00 to 0xFFFF * 0.625ms
    gap_init_callback_t init_callback;
    gap_init_param_t init_param;
} gap_initiating_t;

typedef void (*gap_user_confirm_func_t)(void *priv, bool user_confirmed);
typedef void (*gap_data_func_t)(void *priv, const uint8_t *data);
typedef void (*gap_passkey_func_t)(void *priv, uint32_t passkey);
typedef void (*gap_oob_auth_data_callback_t)(void *priv, const gap_smp_oob_auth_data_t *data);

typedef struct gap_enc_ase_128
{
    uint8_t key[GAP_KEY_LEN];
    uint8_t data[GAP_KEY_LEN];
    hci_cmd_evt_func_t cmd_cb;
    void *priv;
    void *cont;
    void *cmpl;
} gap_enc_aes_128_t;

typedef struct
{
    bt_bdaddr_t peer_addr;
    bt_addr_type_t peer_type;
    uint8_t device_privacy_mode: 1;
    uint8_t is_inuse: 1;
    uint8_t peer_irk[GAP_KEY_LEN];
} gap_resolv_item_t;

typedef struct
{
    uint8_t peer_irk[GAP_KEY_LEN];
    uint8_t peer_csrk[GAP_KEY_LEN];
    uint8_t ltk[GAP_KEY_LEN];
    uint8_t local_ltk[GAP_KEY_LEN];
    uint8_t rand[GAP_RAND_LEN];
    uint8_t local_rand[GAP_RAND_LEN];
    uint8_t ediv[GAP_EDIV_LEN];
    uint8_t local_ediv[GAP_EDIV_LEN];
} gap_key_info_t;

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief Get gap connection item by connhdl
 *
 * @param[in] connhdl  Connection handle
 *
 * @return gap_conn_item_t
 *                     Gap connection stored info
 */
gap_conn_item_t *gap_get_conn_item(uint16_t connhdl);

/**
 * @brief Get gap connection item by bt address
 *
 * @param[in] remote   BDAddress
 *
 * @return gap_conn_item_t
 *                     Gap connection stored info
 */
gap_conn_item_t *gap_get_conn_by_bt_address(const bt_bdaddr_t *remote);

/**
 * @brief Get gap connection item by ble address
 *
 * @param[in] peer_type
 *                     Peer address type
 * @param[in] peer_addr
 *                     Peer BDAddress
 *
 * @return gap_conn_item_t
 *                     Gap connection stored info
 */
gap_conn_item_t *gap_get_conn_by_le_address(bt_addr_type_t peer_type, const bt_bdaddr_t *peer_addr);

/**
 * @brief Get connection used indentity address by connection item
 *
 * @param[in] conn     Connetion item
 *
 * @return ble_bdaddr_t
 *                     ble address
 */
ble_bdaddr_t gap_conn_own_identity_address(const gap_conn_item_t *conn);

/**
 * @brief Get local le indentity address that will used for CTKD (Random or public)
 *
 * @param[in] conn     Connection item run CTKD (SMP IA Exchange)
 *
 * @return ble_bdaddr_t
 *                     ble address
 */
ble_bdaddr_t gap_ctkd_get_le_identity_address(const gap_conn_item_t *conn);

/**
 * @brief Get resolving list item by index
 *
 * @param[in] index    Resolving list item index in list
 *
 * @return const gap_resolv_item_t*
 *                     Return index's resolving list item
 */
const gap_resolv_item_t *gap_resolving_list_get_item(int index);

/**
 * @brief Get items specified by index
 *
 * @param[in] index    Item index
 *
 * @return const gap_filter_item_t*
 *                     Return item in raw device structures format
 */
const gap_filter_item_t *gap_filter_list_get_item(int index);

/**
 * @brief Remove device from resolving list status by index
 *
 * @param[in] index    Device index in resolving list
 * @param[in] check_addr_reso_switch
 *                     Check address resolution state and auto disable and enable control
 *                     Now leave it as false, we can stop all related adv no matter addr reso
 *
 * @return bt_status_t Remove device from resolving list by index status
 */
bt_status_t gap_resolving_list_remove_by_index(int index, bool check_addr_reso_switch);

bt_status_t gap_start_aes_128_encrypt(const uint8_t *key, const uint8_t *data, hci_cmd_evt_func_t cmd_cb, void *priv, void *cont, void *cmpl);
bt_status_t gap_send_hci_le_encrypt(const uint8_t *key, const uint8_t *data, hci_cmd_evt_func_t cmd_cb, void *priv, void *cont, void *cmpl);
bt_status_t gap_send_hci_le_rand(hci_cmd_evt_func_t cmpl_status_cb, void *priv, void *cont);
bt_status_t gap_send_raw_hci_cmd(uint16_t cmd_opcode, uint16_t cmd_len, const uint8_t *data_little_endian, hci_cmd_evt_func_t cmd_cb, void *priv);
bt_status_t gap_send_raw_hci_cmd_extra(uint16_t cmd_opcode, uint16_t cmd_len, const uint8_t *data_little_endian,
                                       hci_cmd_evt_func_t cmd_cb, void *priv, void *cont, void *cmpl);
bool gap_dt_add_peripheral_conn_interval(gap_dt_buf_t *buf, uint16_t min_interval_1_25ms, uint16_t max_interval_1_25ms);
bool gap_dt_add_manufacturer_data(gap_dt_buf_t *buf, uint16_t company_id, const uint8_t *data_ptr, uint8_t data_len);
const bt_bdaddr_t *gap_le_static_address(void);
bt_status_t gap_get_peer_oob_auth_data(gap_conn_item_t *conn);
bt_status_t gap_start_tx_block_authentication(gap_conn_item_t *conn, gap_who_started_auth_t who);
bt_status_t gap_start_rx_block_authentication(gap_conn_item_t *conn, gap_who_started_auth_t who);
bt_status_t gap_tx_rx_block_authentication(gap_conn_item_t *conn, gap_who_started_auth_t who);
bt_status_t gap_start_eatt_block_authentication(gap_conn_item_t *conn, gap_who_started_auth_t who);
void gap_report_le_conn_disconnected(gap_conn_item_t *conn, uint8_t reason);
void gap_report_le_conn_cache_data(gap_conn_item_t *conn, const gatt_client_cache_t *cli_cache,
                                   const gatt_server_cache_t *srv_cache);
void gap_report_le_conn_restore_done(gap_conn_item_t *conn);
void gap_report_mtu_is_exchanged(gap_conn_item_t *conn, uint16_t mtu);
bt_status_t gap_send_hci_le_read_local_p256_public_key(hci_cmd_evt_func_t cmpl_status_cb, void *priv, void *cont);
bt_status_t gap_send_hci_le_gen_dhkey(const uint8_t *pkx, const uint8_t *pky, bool use_debug_private_key,
                                      hci_cmd_evt_func_t cmpl_status_cb, void *priv, void *cont);
bt_status_t gap_send_hci_le_enable_encryption(gap_conn_item_t *conn, const uint8_t *rand, const uint8_t *ediv, const uint8_t *ltk,
                                              uint8_t hdt_mic_length, uint8_t encryption_type,
                                              hci_cmd_evt_func_t cmd_cb, void *priv, void *cont);
bt_status_t gap_send_le_ltk_request_reply(gap_conn_item_t *conn, const uint8_t *ltk, bool positive_reply);
bt_status_t gap_ask_user_numeric_comparison(gap_conn_item_t *conn, uint32_t user_confirm_value, void *priv);
bt_status_t gap_ask_user_ltk_req_reply(gap_conn_item_t *conn, const gap_ltk_enc_info_t *p_req_enc);
bt_status_t gap_ask_input_6_digit_passkey(gap_conn_item_t *conn, gap_passkey_func_t passkey_cb, void *priv);
bt_status_t gap_ask_display_6_digit_passkey(gap_conn_item_t *conn, uint32_t passkey);
bt_status_t gap_get_tk_from_oob_data(gap_conn_item_t *conn, gap_data_func_t tk_cb, void *priv);
bt_status_t gap_get_peer_oob_auth_data(gap_conn_item_t *conn);
bt_status_t gap_get_local_oob_auth_data(gap_conn_item_t *conn);
void gap_gen_local_random(uint8_t *random, uint8_t bytes);
bool gap_is_pairing_method_accept(const gap_bond_sec_t *sec, uint8_t accept_method);
smp_requirements_t gap_get_local_smp_requirements(const gap_conn_item_t *conn, smp_requirements_t *peer_req);
int gatt_conn_event_handler(uintptr_t connhdl, gap_conn_event_t event, gap_conn_callback_param_t param);
bt_status_t gatt_conn_ready_handler(gap_conn_item_t *conn);
void gatt_continue_rx_packet(gap_conn_item_t *item);
void gatt_continue_tx_packet(gap_conn_item_t *item);
void gap_report_mtu_is_exchanged(gap_conn_item_t *conn, uint16_t mtu);
void gap_report_mtu_exchange_req(gap_conn_item_t *conn, uint16_t mtu_request);
void gap_report_smp_pairing_complete(gap_conn_item_t *conn, uint8_t err_code);
void gap_report_gatt_over_bredr_state(gap_conn_item_t *bredr_conn, bool connected, uint8_t reason, uint16_t mtu);
void gap_report_smp_pairing_keys_request(gap_conn_item_t *conn, uint8_t key_dist_bf);
void gap_get_local_secret_public_key_pair(const uint8_t **pp_sec_key_256, const uint8_t **pp_pub_key_256);
void gap_ctkd_notify_ltk_derived(gap_conn_item_t *bredr_conn, bt_addr_type_t peer_type,
                                 const bt_bdaddr_t *peer_addr, bool wait_peer_kdist, const uint8_t *ltk);
void gap_ctkd_notify_link_key_derived(gap_conn_item_t *conn, const bt_bdaddr_t *bt_addr, const uint8_t *link_key, bool wait_peer_ia);
void gap_ltk_from_lk_generated(gap_conn_item_t *conn, const uint8_t *ltk, bool waiting_peer_kdist);
void gap_lk_from_ltk_generated(gap_conn_item_t *conn, const uint8_t *link_key, bool waiting_peer_ia);
void gap_recv_smp_encrypted(gap_conn_item_t *conn, bool new_pair, uint8_t error_code);
bool gap_recv_smp_security_request(gap_conn_item_t *conn, bool local_request, uint8_t auth_req);
bool gap_recv_local_security_requirements(gap_conn_item_t *conn, gap_who_started_auth_t who, uint32_t ca);
bool gap_recv_smp_pairing_requirements(gap_conn_item_t *conn, bool is_pairing_request, const smp_requirements_t *p_smp_req);
void gap_legacy_ltk_ediv_rand_generated(gap_conn_item_t *conn);
void gap_recv_peer_legacy_ltk_ediv_rand(gap_conn_item_t *conn);
void gap_recv_peer_irk_ia(gap_conn_item_t *conn);
void gap_recv_peer_csrk(gap_conn_item_t *conn, const uint8_t *csrk);
void gap_recv_l2cap_conn_param_update_req(uint16_t connhdl, uint8_t trans_id, const void *l2cap_params);
void gap_recv_l2cap_conn_param_update_rsp(uint16_t connhdl, uint8_t result);
bool gap_is_directly_send_sec_error_rsp(void);
bool gap_is_directly_report_sec_error(void);
void gap_set_default_key_size(uint8_t key_size);
void gap_set_default_auth_req(uint8_t mitm_auth);
void gap_pts_set_ble_l2cap_test(bool test);
bool gap_is_pts_ble_l2cap_test(void);
void gap_pts_set_use_passkey_entry(void);
void gap_pts_set_use_oob_method(void);
void gap_pts_set_no_mitm_auth(void);
void gap_pts_set_display_only(void);
void gap_pts_set_keyboard_only(void);
void gap_pts_set_no_bonding(void);
void gap_pts_set_dont_start_smp(bool dont_auto_start_smp);
bool gap_pts_get_dont_start_smp(void);
void gap_pts_gen_linkkey_from_ltk(void);
void gap_pts_set_dist_irk_only(void);
void gap_pts_set_dist_csrk(void);

/**
 * bes tws fast pair host support apis
 * usage: set enable before le adv/scan
*/

void gap_vendor_enable_besfp(uint8_t *tws_bt_addr, uint8_t *peer_le_addr);
void gap_vendor_disable_besfp();
bool gap_vendor_is_besfp_enabled();
uint8_t *gap_vender_besfp_get_tws_bt_addr();
uint8_t *gap_vender_besfp_get_peer_le_addr();

#if defined(__cplusplus)
}
#endif
#endif /* __BLE_GAP_I_H__ */
