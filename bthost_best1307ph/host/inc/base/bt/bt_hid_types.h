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

// see @MAX_DESCRIPTOR_COUNT
#define BT_MAX_DESCRIPTOR_COUNT 6

typedef enum {
    BT_HID_CONN_STATE_DISCONNECTED = 0,
    BT_HID_CONN_STATE_CONNECTED,
} bt_hid_conn_state_t;

typedef struct {
    uint8_t descriptor_type;
    uint16_t descriptor_list_len;
    const uint8_t *descriptor_list_data;
} bt_hid_descriptor_list_t;

typedef struct {
    uint16_t hid_version;
    uint16_t parser_version;
    uint8_t device_subclass;
    uint8_t contry_code;
    bool virtual_cable;
    bool reconnect_initiate;
    bool boot_device;
    uint8_t decrp_count;
    bt_hid_descriptor_list_t decrp[BT_MAX_DESCRIPTOR_COUNT];
} bt_hid_sdp_record_t;

typedef struct {
    int16_t rx; // -32767 to 32767 represent [-314159265, 314159265] * 10^-8 rad
    int16_t ry;
    int16_t rz;
    int16_t vx; // -32767 to 32767 represent [-32, 32] rad/sec
    int16_t vy;
    int16_t vz;
    uint8_t counter;
} __attribute__((packed)) bt_hid_sensor_report_t;

typedef void (*bt_hid_conn_state_cb)(const bt_bdaddr_t *address, bt_hid_conn_state_t state, uint8_t error_code);

typedef void (*bt_hid_sensor_state_cb)(const bt_bdaddr_t *address, uint8_t sensor_state, uint8_t error_code);

typedef void (*bt_hid_recv_control_data_cb)(const bt_bdaddr_t *address, const uint8_t *data, uint8_t len, uint8_t error_code);

typedef void (*bt_hid_recv_interrupt_data_cb)(const bt_bdaddr_t *address, const uint8_t *data, uint8_t len, uint8_t error_code);

typedef void (*bt_hid_recv_sdp_record_cb)(const bt_bdaddr_t *address, const bt_hid_sdp_record_t *record, uint8_t error_code);

typedef void (*bt_hid_send_done_cb)(const bt_bdaddr_t *address, uint8_t error_code);

typedef struct {
    bt_hid_conn_state_cb           conn_state_cb;
    bt_hid_sensor_state_cb         sensor_state_cb;
    bt_hid_recv_control_data_cb    recv_control_data_cb;
    bt_hid_recv_interrupt_data_cb  recv_interrupt_data_cb;
    bt_hid_recv_sdp_record_cb      recv_sdp_record_cb;
    bt_hid_send_done_cb            send_done_cb;
} bt_hid_callbacks_t;
