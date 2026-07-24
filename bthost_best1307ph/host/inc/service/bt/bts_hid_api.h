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

#ifndef __BTS_HID_API_H__
#define __BTS_HID_API_H__
#include "bt_hid_types.h"
#include "bts_bt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BTS_HID_CB_USER_APP      = 0,
    BTS_HID_CB_USER_MAX,
} BTS_HID_CALLBACK_USER_E;

/**
 ****************************************************************************************
 * @brief       Register hid device callbacks.
 * @param[in]   user: Registrant
 * @param[in]   callbacks: Pointer to callbacks for handling dip events.
 * @return      none
 ****************************************************************************************
 */
void bts_hid_register_callbacks(BTS_HID_CALLBACK_USER_E user, bt_hid_callbacks_t *callbacks);

/**
 * @brief       Deregister dip callbacks.
 * @param[in]   user: Registrant
 * @return      none
 */
void bts_hid_deregister_callbacks(BTS_HID_CALLBACK_USER_E user);

/**
 ****************************************************************************************
 *    _   _ ___ ____       ____  _______     _____ ____ _____
 *   | | | |_ _|  _ \     |  _ \| ____\ \   / /_ _/ ___| ____|
 *   | |_| || || | | |    | | | |  _|  \ \ / / | | |   |  _|
 *   |  _  || || |_| |    | |_| | |___  \ V /  | | |___| |___
 *   |_| |_|___|____/     |____/|_____|  \_/  |___\____|_____|
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief       Initiate connection to hid host
 * @param[in]   address: Pointer to the Bluetooth address of the peer device.
 * @return      Status of the operation
 ****************************************************************************************
 */
bts_status_t bts_hid_device_connect(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Initiate disconnection to hid host
 * @param[in]   address: Pointer to the Bluetooth address of the peer device.
 * @return      Status of the operation
 ****************************************************************************************
 */
bts_status_t bts_hid_device_disconnect(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Determine whether the hid is connected.
 * @param[in]   address: Pointer to the Bluetooth address of the peer device.
 * @return      Status of the operation.
 ****************************************************************************************
 */
bool bts_hid_device_is_connected(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       User set custom descriptor
 * @param[in]   descriptor: Pointer to the custom descriptor.
 * @return      Status of the operation.
 ****************************************************************************************
 */
bts_status_t bts_hid_device_set_custom_descriptor(const bt_hid_descriptor_list_t *descriptor);

/**
 ****************************************************************************************
 * @brief       Hid device send sensor state.
 * @param[in]   report: Pointer to the custom descriptor.
 * @return      Status of the operation.
 ****************************************************************************************
 */
bts_status_t bts_hid_device_send_sensor_report(const bt_bdaddr_t *address, const bt_hid_sensor_report_t *report);

/**
 ****************************************************************************************
 * @brief       Hid keyboard device send report.
 * @param[in]   report: Pointer to the custom descriptor
 * @return      Status of the operation
 ****************************************************************************************
 */
bts_status_t bts_hid_device_send_keyboard_report(const bt_bdaddr_t *address, uint8_t modifier_key, uint8_t key_code);

/**
 ****************************************************************************************
 * @brief       Hid mouse device send report.
 * @param[in]   x_pos:
 * @param[in]   y_pos:
 * @param[in]   clk_buttons:
 * @return      Status of the operation
 ****************************************************************************************
 */
bts_status_t bts_hid_device_send_mouse_report(const bt_bdaddr_t *address, int8_t x_pos, int8_t y_pos, uint8_t clk_buttons);

/**
 * @brief       Hid mouse device send control report.
 * @param[in]   ctl_buttons:
 * @return      Status of the operation
 */
bts_status_t bts_hid_device_send_mouse_control_report(const bt_bdaddr_t *address, uint8_t ctl_buttons);

/**
 ****************************************************************************************
 * @brief       Hid device send capture.
 * @param[in]   address: Pointer to the Bluetooth address of the peer device.
 * @return      Status of the operation
 ****************************************************************************************
 */
bts_status_t bts_hid_device_send_capture(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Indicate virtual cable has been unplugged to hid host.
 * @param[in]   address: Pointer to the Bluetooth address of the peer device.
 * @return      Status of the operation
 ****************************************************************************************
 */
bts_status_t bts_hid_device_send_virtual_cable_unplug(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Indicate virtual cable has been unplugged to hid host.
 * @param[in]   address: Pointer to the Bluetooth address of the peer device.
 * @return      Status of the operation
 ****************************************************************************************
 */
bts_status_t bts_hid_device_send_consumer_ctrl_request(uint8_t ctrl_key);

/**
 ****************************************************************************************
 *    _   _ ___ ____       _   _  ___  ____ _____
 *   | | | |_ _|  _ \     | | | |/ _ \/ ___|_   _|
 *   | |_| || || | | |    | |_| | | | \___ \ | |
 *   |  _  || || |_| |    |  _  | |_| |___) || |
 *   |_| |_|___|____/     |_| |_|\___/|____/ |_|
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief       Initiate connection to hid device.a2dp_callback_parms_t
 * @param[in]   address: Pointer to the Bluetooth address of the peer device.
 * @return      Status of the operation.
 ****************************************************************************************
 */
bts_status_t bts_hid_host_connect(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Initiate disconnection to hid device.
 * @param[in]   address: Pointer to the Bluetooth address of the peer device.
 * @return      Status of the operation.
 ****************************************************************************************
 */
bts_status_t bts_hid_host_disconnect(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Indicate virtual cable has been unplugged to hid device.
 * @param[in]   address: Pointer to the Bluetooth address of the peer device.
 * @return      Status of the operation.
 ****************************************************************************************
 */
bts_status_t bts_hid_host_send_virtual_cable_unplug(const bt_bdaddr_t *address);

#ifdef __cplusplus
}
#endif

#endif /* __BTS_HID_API_H__ */