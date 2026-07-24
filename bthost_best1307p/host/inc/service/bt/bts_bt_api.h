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

#ifndef __BTS_BT_API_H__
#define __BTS_BT_API_H__

#include "bts_bt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 ****************************************************************************************
 * @brief       Register hid device callbacks.
 * @param[in]   user: Registrant
 * @param[in]   callbacks: Pointer to callbacks for handling dip events.
 * @return      none
 ****************************************************************************************
 */
void bts_bt_register_callbacks(bt_callback_user_t user, bt_callbacks_t *callbacks);

/**
 ****************************************************************************************
 * @brief       Deregister dip callbacks.
 * @param[in]   user: Registrant
 * @return      none
 ****************************************************************************************
 */
void bts_bt_deregister_callbacks(bt_callback_user_t user);

/**
 ****************************************************************************************
 * @brief       Obtain the device id through the device address.
 * @param[in]   device_id: Local device index, start from 0.
 * @param[out]  address: Pointer to the bluetooth address of corresponding device_id.
 * @return      Status of operation.
 ****************************************************************************************
 */
bts_status_t bts_bt_get_device_address(uint8_t device_id, bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Obtain the device address through the device id.
 * @param[in]   address: Pointer to the bluetooth address of peer device.
 * @return      None
 ****************************************************************************************
 */
uint8_t bts_bt_get_device_id(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Obtain the device connnection handle through the device address.
 * @param[in]   address: Pointer to the bluetooth address of peer device.
 * @return      Device connection handle.
 ****************************************************************************************
 */
uint16_t bts_bt_get_device_conhdl(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Obtain the device address through the device id.
 * @return      None
 ****************************************************************************************
 */
bts_status_t bts_bt_start_inquiry(void);

bts_status_t bts_bt_cancel_inquiry(void);

bts_status_t bts_bt_write_extended_inquiry_response(const bts_extended_inquiry_response_t *eir);

bts_status_t bts_bt_write_access_mode(bt_access_mode_t mode);

bts_status_t bts_bt_acl_connect_with_page_timeout(const bt_bdaddr_t *address, uint32_t page_timeout, uint32_t time_to_next_page);

bts_status_t bts_bt_acl_connect(const bt_bdaddr_t *address);

bts_status_t bts_bt_acl_disconnect(const bt_bdaddr_t *address);

bts_status_t bts_bt_acl_cancel_connect(const bt_bdaddr_t *address);
/**
 ****************************************************************************************
 * @brief       Whether the device as this address has connected.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      none
 ****************************************************************************************
 */
bool bts_bt_is_connected(const bt_bdaddr_t *address);

bts_status_t bts_bt_sco_connect_rsp(const bt_bdaddr_t *address, bool accept);

bts_status_t bts_bt_sco_connect_req(const bt_bdaddr_t *address);

bts_status_t bts_bt_write_page_timeout(uint16_t timeout_slot);

bts_status_t bts_bt_write_link_supervision_timeout(const bt_bdaddr_t *address, uint16_t timeout_slot);

bts_status_t bts_bt_set_local_address(const bt_bdaddr_t *address);

bts_status_t bts_bt_get_local_address(bt_bdaddr_t *address);

bts_status_t bts_bt_set_local_name(const char *name, uint8_t len);

bts_status_t bts_bt_get_local_name(char *name_buf, uint8_t buf_len);

bts_status_t bts_bt_get_remote_name(const bt_bdaddr_t *address);

bts_status_t bts_bt_get_class_of_device(const bt_bdaddr_t *address, uint8_t *cod);

bts_status_t bts_bt_enter_sniff_mode(const bt_bdaddr_t *address, const bt_sniff_info_t *info);

bts_status_t bts_bt_exit_sniff_mode(const bt_bdaddr_t *address);

bts_status_t bts_bt_keep_active_mode(const bt_bdaddr_t *address, bts_keep_active_mode_user_t user, bool keep_active_mode);

bts_status_t bts_bt_get_remote_version(const bt_bdaddr_t *address, bt_remote_version_t *remote_version);

void bts_bt_request_exit_mhdt_mode(const bt_bdaddr_t *bdaddr);

void bts_bt_request_enter_mhdt_mode(const bt_bdaddr_t *bdaddr, uint8_t tx_rates, uint8_t rx_rates);

void bts_register_bt_sniff_config_callback(bool (*cb)(bt_sniff_info_t *));

void bts_register_mhdt_mode_change_callback(void (*cb)(struct bdaddr_t remote, bool isIn_mhdt_mode));
#ifdef __cplusplus
}
#endif

#endif /* __BTS_BT_API_H__ */