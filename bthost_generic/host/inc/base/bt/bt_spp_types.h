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
   BT_RFCOMM_CHANNEL_GS_CONTROL       = 0x0a,     // 0x00001000 => BTIF_APP_SPP_SERVER_START_ID
   BT_RFCOMM_CHANNEL_GS_AUDIO         = 0x0b,     // 0x00002000
   BT_RFCOMM_CHANNEL_TOTA             = 0x0c,     // 0x00004000
   BT_RFCOMM_CHANNEL_BES_OTA          = 0x0d,     // 0x00008000
   BT_RFCOMM_CHANNEL_AI_VOICE         = 0x0e,     // 0x00010000
   BT_RFCOMM_CHANNEL_GREEN            = 0x0f,     // 0x00020000
   BT_RFCOMM_CHANNEL_RED              = 0x10,     // 0x00040000
   BT_RFCOMM_CHANNEL_FP               = 0x11,     // 0x00080000
   BT_RFCOMM_CHANNEL_AMA              = 0x12,     // 0x00100000
   BT_RFCOMM_CHANNEL_BES              = 0x13,     // 0x00200000
   BT_RFCOMM_CHANNEL_BAIDU            = 0x14,     // 0x00400000
   BT_RFCOMM_CHANNEL_TENCENT          = 0x15,     // 0x00800000
   BT_RFCOMM_CHANNEL_ALI              = 0x16,     // 0x01000000
   BT_RFCOMM_CHANNEL_COMMON           = 0x17,     // 0x02000000
   BT_RFCOMM_CHANNEL_COMMON_RECORD    = 0x18,     // 0x04000000
   BT_RFCOMM_CHANNEL_BIXBY_CONTROL    = 0x19,     // 0x08000000
   BT_RFCOMM_CHANNEL_BIXBY_VOICE      = 0x1a,     // 0x10000000
   BT_RFCOMM_CHANNEL_CCMP             = 0x1b,     // 0x20000000
   BT_RFCOMM_CHANNEL_MMA              = 0x1c,     // 0x40000000

   BT_RFCOMM_CHANNEL_CUSTOM_2         = 0x1d,
   BT_RFCOMM_CHANNEL_CUSTOM_1         = 0x1e,
} bt_rfcomm_channel_t;

typedef enum {
    BT_SPP_EVENT_CONNECTED = 0,     // spp connected
    BT_SPP_EVENT_DISCONNECTED,      // spp disconnected
    BT_SPP_EVENT_SEND_DONE,         // spp send data complete
    BT_SPP_EVENT_RECV_DATA,         // spp receive data
    BT_SPP_EVENT_RECV_CREDIT,       // spp channel receive peer credits(indicate how many packets can be sent)
} bta_spp_event_t;

// spp connection handle, alias @rfcomm_handle
typedef uint32_t bt_spp_connhdl_t;

typedef struct {
    bt_rfcomm_channel_t local_server_channel;
    bt_spp_connhdl_t connhdl;
    uint8_t error_code;

    union {
        // params for SEND_DONE event
        struct {
            const uint8_t *priv;
        } send_done;
        // params for RECV_DATA event
        struct {
            uint16_t size;
            const uint8_t *data;
        } recv_data;
        // params for RECV_CREDIT event
        struct {
            uint8_t credits;
        } recv_credit;
    } params;
} bt_spp_event_params_t;

/**
 ****************************************************************************************
 * @brief       Callback for handle spp event.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   event: Spp event.
 * @param[in]   param: Parameters of different events.
 ****************************************************************************************
 */
typedef void (*bt_spp_event_cb)(const bt_bdaddr_t *address, bta_spp_event_t event, const bt_spp_event_params_t *param);

/**
 ****************************************************************************************
 * @brief       Callback for handle spp connection request from spp client.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   local_server_channel: Indicate which channel the connection request come from.
 * @return      Indicate whether to accept the connection request from spp client.
 ****************************************************************************************
 */
typedef bool (*bt_spp_connect_req_cb)(const bt_bdaddr_t *address, bt_rfcomm_channel_t local_server_channel);

typedef struct {
    bt_spp_event_cb client_event_cb;
    bt_spp_event_cb server_event_cb;
    bt_spp_connect_req_cb conn_req_cb;
} bt_spp_callbacks_t;
