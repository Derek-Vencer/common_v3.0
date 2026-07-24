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

typedef enum {
    BT_CONN_STATE_DISCONNECTED = 0,
    BT_CONN_STATE_CONNECTED,
} bt_connection_state_t;

typedef enum {
    BT_NOT_ACCESSIBLE        = 0x00,
    BT_DISCOVERABLE_ONLY     = 0x01,
    BT_CONNECTABLE_ONLY      = 0x02,
    BT_GENERAL_ACCESSIBLE    = 0x03,
    BT_LIMITED_ACCESSIBLE    = 0x13,
} bt_access_mode_t;

typedef enum {
    BT_LINK_ACTIVE_MODE = 0x00,
    BT_LINK_HOLD_MODE   = 0x01,
    BT_LINK_SNIFF_MODE  = 0x02,
    BT_LINK_PARK_MODE   = 0x03,
} bt_link_mode_t;

typedef struct {
    uint8_t version;
    uint16_t company_id;
    uint16_t subversion;
} bt_remote_version_t;

typedef struct {
    uint16_t maxInterval;
    uint16_t minInterval;
    uint16_t attempt;
    uint16_t timeout;
} bt_sniff_info_t;

typedef struct
{
    uint8_t error_code;
    uint8_t codec;
    uint16_t sco_handle;
    uint8_t interval;
    uint8_t window;
} bt_sco_param_t;

typedef enum
{
    BT_ROLE_CENTRAL     = 0x00,
    BT_ROLE_PERIPHERAL  = 0x01,
    BT_ROLE_UNKNOWN     = 0xff,
} bt_role_t;

typedef struct
{
    uint8_t page_scan_repeat_mode;
    int8_t rssi;
    uint16_t clock_offset;
    uint32_t class_of_device;
    uint8_t *eir;// Extended Inquiry response
} bt_inquiry_result_params_t;

typedef enum
{
    BT_CB_USER_BTA = 0,
    BT_CB_USER_APP,
    BT_CB_USER_DEPRECATED,
    BT_CB_USER_CUSTOMER,
    BT_CB_USER_MAX,
} bt_callback_user_t;

typedef void (*bt_inquiry_result_cb)(const bt_bdaddr_t *address, const bt_inquiry_result_params_t *params);

typedef void (*bt_inquiry_complete_cb)(const bt_bdaddr_t *address, uint8_t error_code);

typedef void (*bt_access_mode_changed_cb)(bt_access_mode_t mode);

typedef void (*bt_acl_connection_state_cb)(const bt_bdaddr_t *address, bt_connection_state_t state, uint8_t error_code);

typedef void (*bt_sco_connection_state_cb)(const bt_bdaddr_t *address, bt_connection_state_t state, bt_sco_param_t *param);

typedef void (*bt_link_mode_changed_cb)(const bt_bdaddr_t *address, bt_link_mode_t mode, uint16_t interval, uint8_t error_code);

typedef void (*bt_role_discovered_cb)(const bt_bdaddr_t *address, bt_role_t role, uint8_t error_code);

typedef void (*bt_role_changed_cb)(const bt_bdaddr_t *address, bt_role_t role, uint8_t error_code);

typedef void (*bt_authenticated_cb)(const bt_bdaddr_t *address, uint8_t error_code);

typedef void (*bt_encrypt_changed_cb)(const bt_bdaddr_t *address, bool encrypted, uint8_t error_code);

typedef void (*bt_bond_changed_cb)(const bt_bdaddr_t *address, uint8_t error_code);

typedef void (*bt_remote_name_cb)(const bt_bdaddr_t *address, const uint8_t *name, uint8_t len);

typedef struct
{
    bt_inquiry_result_cb        inquiry_result_cb;
    bt_inquiry_complete_cb      inquiry_complete_cb;
    bt_access_mode_changed_cb   access_mode_changed_cb;
    bt_acl_connection_state_cb  acl_connection_state_cb;
    bt_sco_connection_state_cb  sco_connection_state_cb;
    bt_link_mode_changed_cb     link_mode_changed_cb;
    bt_role_discovered_cb       role_discovered_cb;
    bt_role_changed_cb          role_changed_cb;
    bt_authenticated_cb         authenticated_cb;
    bt_encrypt_changed_cb       encrypt_changed_cb;
    bt_bond_changed_cb          bond_changed_cb;
    bt_remote_name_cb           remote_name_cb;
} bt_callbacks_t;
