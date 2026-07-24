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
#ifndef __BT_HID_SERVICE_H__
#define __BT_HID_SERVICE_H__
#include "bt_stack_event.h"
#include "hid_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BT_HID_EVENT_OPENED = BT_EVENT_HID_OPENED,
    BT_HID_EVENT_CLOSED,
    BT_HID_EVENT_SENSOR_STATE_CHANGED,
    BT_HID_EVENT_TXDONE,
    BT_HID_EVENT_RCV_CTL_DATA,
    BT_HID_EVENT_RCV_INT_DATA,
    BT_HID_EVENT_RCV_SDP_DATA,
    BT_HID_EVENT_END,
} bt_hid_event_t;

typedef int (*bt_hid_callback_t)(const bt_bdaddr_t *bd_addr, bt_hid_event_t event, bt_hid_callback_param_t param);

bt_status_t bt_hid_init(bt_hid_callback_t callback);

bt_status_t bt_hid_cleanup(void);

#if defined(BT_HID_HOST)
bt_status_t bt_hid_host_connect(const bt_bdaddr_t *bd_addr);

bt_status_t bt_hid_host_disconnect(const bt_bdaddr_t *bd_addr);

bt_status_t bt_hid_host_send_virtual_cable_unplug(const bt_bdaddr_t *bd_addr);
#endif // BT_HID_HOST

#if defined(BT_HID_DEVICE)
bt_status_t bt_hid_device_connect(const bt_bdaddr_t *bd_addr);

bt_status_t bt_hid_device_disconnect(const bt_bdaddr_t *bd_addr);

bt_status_t bt_hid_device_send_sensor_report(const bt_bdaddr_t *bd_addr, const struct hid_sensor_report_t *report);

bt_status_t bt_hid_device_send_keyboard_report(const bt_bdaddr_t *remote, uint8_t modifier_key, uint8_t key_code);

bt_status_t bt_hid_device_send_mouse_report(const bt_bdaddr_t *remote, int8_t x_pos, int8_t y_pos, uint8_t clk_buttons);

bt_status_t bt_hid_device_send_mouse_control_report(const bt_bdaddr_t *remote, uint8_t ctl_buttons);

bt_status_t bt_hid_device_send_capture(const bt_bdaddr_t *bd_addr);

bt_status_t bt_hid_device_send_virtual_cable_unplug(const bt_bdaddr_t *bd_addr);
#endif // BT_HID_DEVICE

#if defined(BLE_HID_ENABLE)
#ifdef HOGP_HEAD_TRACKER_PROTOCOL
bt_status_t ble_hid_send_sensor_report(const struct hid_sensor_report_t *report);
#else
#if defined(HID_MOUSE)
bt_status_t ble_hid_send_mouse_input_report(const hid_mouse_boot_input_report_t *report);
bt_status_t ble_hid_send_mouse_control_report(const hid_mousectl_report_t *report);
#else
bt_status_t ble_hid_send_mediakey_report(const struct keyboard_control_key_t *report);
#endif
#endif
#endif //BLE_HID_ENABLE

#ifdef __cplusplus
}
#endif
#endif /* __BT_HID_SERVICE_H__ */

