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
#ifndef __BT_IF_H__
#define __BT_IF_H__
#include <stdint.h>
#include "bluetooth.h"
#include "hci_api.h"
#include "l2cap_api.h"
#include "me_api.h"

//Application ID,indentify profle app context

#ifdef __cplusplus
extern "C" {
#endif

#define BTH_STACK_ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))

// should greater than (sizeof(rfcomm_sync_data_item_t) * RFCOMM_MAX_SYNC_DATA_ITEMS) + 1
#define RFCOMM_MAX_SYNC_TXRX_CREDIT_DATA_SIZE (30)

typedef enum
{
    BT_STACK_MODE_DUAL = 0,
    BT_STACK_MODE_BT,
    BT_STACK_MODE_BLE,
} BT_STACK_MODE_E;

struct btif_sync_txrx_credit_t {
    bt_bdaddr_t remote_bdaddr;
    uint8_t rfcomm_sync_txrx_credit_data[RFCOMM_MAX_SYNC_TXRX_CREDIT_DATA_SIZE];
} __attribute__ ((packed));

enum pair_event
{
    PAIR_EVENT_NUMERIC_REQ,
    PAIR_EVENT_COMPLETE,
    PAIR_EVENT_FAILED,
};

typedef struct {
    // see @BT_STACK_MODE_E
    uint8_t init_mode;
    char* bt_name;

    // init dongle callback
    void  (*ready_cb)(int status);

    // L2CAP profile
    btif_l2cap_config_t l2cap_cfg;

    // HCI layer param
    btif_hci_trace_param_t hci_trace;
    void (*hci_btc_state_check)(void);
    int (*hci_tx_cb)(const uint8_t *buf, uint16_t size);
    void (*hci_reg_rx_cb)(void (*cb)(const uint8_t *data, uint16_t len));
} bt_stack_init_t;

typedef void (*btif_pairing_callback_t)(enum pair_event evt, void *data);
typedef bool (*btif_bt_sniff_params_callback_t)(btif_sniff_info_t *sniff_info);

void btif_pairing_register_callback(btif_pairing_callback_t callback);
void btif_confirmation_register_callback(btif_confirmation_req_callback_t callback);
void btif_register_sec_conn_callback(btif_sec_conn_callback_t callback);

int bt_stack_init(bt_stack_init_t* param);
int bt_stack_deinit();
int bt_stack_classic_init(void);

void btif_set_local_name(const unsigned char *name, uint8_t len);
int bt_set_local_clock(uint32_t clock);
void bt_process_stack_events(void);
bool btif_is_gatt_over_br_edr_enabled(void);

void btif_set_btstack_chip_config(void *config);
void btif_get_extended_inquiry_response(uint8_t **eir_buf, uint8_t *buf_len, uint8_t *offset);
void btif_set_extended_inquiry_response(const uint8_t *eir, uint8_t len);
void btif_register_bt_sniff_config_callback(btif_bt_sniff_params_callback_t callback);
int btif_me_send_hci_cmd(uint16_t opcode, uint8_t *param_data_ptr, uint8_t param_len);
void btif_confirmation_resp(struct bdaddr_t *bdaddr, bool accept);
void btif_input_user_passkey(struct bdaddr_t *bdaddr, uint32_t passkey);
uint32_t btif_get_class_of_device(void);
uint32_t btif_set_class_of_device(uint8 *cod);
int8 btif_set_secure_connections_host_support(bool support);
int8 btif_set_ble_host_support(bool support);
void btif_osapi_lock_stack(void);
void btif_osapi_unlock_stack(void);
int btif_osapi_lock_is_exist(void);
void btif_osapi_notify_evm(void);
struct btm_conn_item_t *btif_ibrt_create_new_snoop_link(bt_bdaddr_t *remote, uint16 conn_handle);
void btif_conn_ibrt_disconnected_handle(struct btm_conn_item_t *btm_conn);
int8 btif_l2cap_send_data( uint32 l2cap_handle, uint8 *data, uint32 datalen, void *context);
int btif_check_l2cap_mtu_buffer_available(void);

void btif_ecc_gen_new_secret_key_192(uint8_t* secret_key192);
void btif_ecc_gen_new_public_key_192(uint8_t* secret_key,uint8_t* out_public_key);
void btif_hci_enable_cmd_evt_debug(bool enable);
void btif_hci_enable_tx_flow_debug(bool enable);
void btif_hci_enable_tx_0c35_without_alloc(bool enable);
void btif_hci_register_pending_too_many_rx_acl_packets(void (*cb)(void));
uint8_t *btif_hci_get_curr_pending_cmd(uint16_t cmd_opcode);
uint16_t btif_hci_get_curr_cmd_opcode(void);
int btif_hci_count_free_bt_tx_buff(void);

void* btif_avdtp_create_media_channel(uint8_t device_id);
void btif_avdtp_send_cmd(void* stream, uint8_t cmd, uint8_t* data, uint32_t datalen);

void btif_pts_av_set_sink_delay(void);
void btif_pts_rfc_register_channel(void);
void btif_pts_rfc_close(void);
void btif_pts_rfc_close_dlci_0(void);
void btif_pts_rfc_send_data(void);
void btif_pts_l2c_send_data(void);
void btif_register_mhdt_mode_change_callback(void (*cb)(struct bdaddr_t remote, bool isIn_mhdt_mode));
void btif_register_is_pts_address_check_callback(bool (*cb)(void *remote));
btif_remote_device_t *btif_cmgr_pts_get_remDev(btif_cmgr_handler_t *cmgr_handler);

/**
 ****************************************************************************************
 *    ____ _____ ___ _____   ___ ____  ____ _____   _____ _   _ _   _  ____
 *   | __ )_   _|_ _|  ___| |_ _| __ )|  _ \_   _| |  ___| | | | \ | |/ ___|
 *   |  _ \ | |  | || |_     | ||  _ \| |_) || |   | |_  | | | |  \| | |
 *   | |_) || |  | ||  _|    | || |_) |  _ < | |   |  _| | |_| | |\  | |___
 *   |____/ |_| |___|_|     |___|____/|_| \_\|_|   |_|    \___/|_| \_|\____|
 *
 ****************************************************************************************
 */
struct a2dp_command_t {
    bool is_valid;
    uint8_t transaction;
    uint8_t signal_id;
} __attribute__ ((packed));

struct btif_sync_data_to_new_master_t {
    bt_bdaddr_t remote_bdaddr;
    struct a2dp_command_t stream_cmd;
} __attribute__ ((packed));

uint32_t btif_save_app_bt_device_ctx(uint8_t *ctx_buffer,uint8_t psm_context_mask);
uint32_t btif_set_app_bt_device_ctx(uint8_t *ctx_buffer,uint8_t psm_context_mask,uint8_t bt_devices_idx, uint8_t rm_detbl_idx, uint8_t avd_ctx_device_idx);

void btif_ibrt_sync_txrx_credit(struct btif_sync_txrx_credit_t *sync_data, const bt_bdaddr_t *remote);
void btif_ibrt_sync_txrx_credit_handler(struct btif_sync_txrx_credit_t *sync_data, uint16_t length);

void btif_ibrt_stack_clean_slave_status(const bt_bdaddr_t* remote);
bool btif_ibrt_master_wait_remote_new_master_ready(const bt_bdaddr_t *remote);
void btif_ibrt_master_tws_switch_set_start(const bt_bdaddr_t* remote);
void btif_ibrt_slave_tws_switch_set_start(const bt_bdaddr_t* remote);
void btif_ibrt_master_become_slave(const bt_bdaddr_t* remote);
void btif_ibrt_slave_become_master(const bt_bdaddr_t* remote);
void btif_ibrt_clear_side_save_credit_bit(const bt_bdaddr_t* remote);

void btif_hci_set_start_ibrt_reserve_buff(bool reserve);
void btif_hci_register_acl_tx_buff_tss_process(void (*cb)(void));

void btif_ibrt_old_master_receive_ready_req(struct btif_sync_data_to_new_master_t *sync_data, const bt_bdaddr_t *remote);
void btif_ibrt_new_master_receive_ready_rsp(struct btif_sync_data_to_new_master_t *sync_data);

#ifdef __cplusplus
}
#endif /*  */

#endif /*__BT_IF_H_*/
