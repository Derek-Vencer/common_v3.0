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

#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CFG_APP_DATAPATH_SERVER

typedef enum
{
    /// datapath connect, para_p: uint8_t *conidx
    DP_CONN_DONE,
    /// datapath disconnect, para_p: uint8_t *conidx
    DP_DISCONN_DONE,
    /// change mtu done, para_p: app_dp_mtu_exchange_msg_t *
    DP_MTU_CHANGE_DONE,
    /// receive data , para_p:app_dp_rec_data_msg_t *
    DP_DATA_RECEIVED,
    /// tx done , para_p: NULL
    DP_TX_DONE,
} DP_EVENT_TYPE_E;

typedef struct
{
    uint8_t conidx;
    uint16_t mtu;
} app_dp_mtu_exchange_msg_t;

typedef struct
{
    uint8_t  *data;
    uint16_t data_len;
    uint8_t  conidx;
} app_dp_rec_data_msg_t;

typedef union
{
    uint8_t connect_index;
    uint8_t disconnect_index;
    app_dp_mtu_exchange_msg_t dp_mtu_exchange;
    app_dp_rec_data_msg_t dp_recv_data;
} ble_if_app_dp_param_u;

typedef void(*app_datapath_event_cb)(DP_EVENT_TYPE_E event_type, ble_if_app_dp_param_u *para_p);

typedef void(*app_datapath_server_tx_done_t)(void);
typedef void(*app_datapath_server_data_received_callback_func_t)(uint8_t *p_buff, uint16_t bufLength);
typedef void(*app_datapath_server_disconnected_done_t)(uint8_t conidx);
typedef void(*app_datapath_server_connected_done_t)(uint8_t conidx);
typedef void(*app_datapath_server_mtuexchanged_done_t)(uint8_t conidx, uint16_t mtu);
typedef uint32_t(*app_datapath_server_role_switch_callback_t)(uint8_t conidx, bool is_restore, uint8_t *buf, uint16_t buf_len);

/**
 * @brief Initialize the datapaths service
 *
 * This function initializes the Bluetooth Low Energy data path service,
 * registers the GATT service with the stack, and sets up the
 * necessary attributes for data transmission between earbuds and client.
 *
 * @note This function should be called during system initialization
 *       before any Bluetooth connections are established.
 */
void app_datapaths_init(void);

/**
 * @brief Deinitialize the datapaths service
 *
 * This function unregisters the GATT service from the stack
 * and cleans up any allocated resources for the data path service.
 *
 * @note This function should be called during system shutdown or
 *       when the service needs to be temporarily disabled.
 */
void app_datapaths_deinit(void);

/**
 * @brief Send data via notification
 *
 * This function sends data to the connected client using notification mechanism.
 * Notifications are one-way transmissions that do not require acknowledgment from
 * the client. The data is sent to the TX characteristic of the data path service.
 *
 * @param connhdl Connection index of the target device
 * @param data Pointer to the data buffer to send
 * @param len Length of the data to send
 */
void app_datapath_server_send_data_via_notification(uint16_t connhdl, uint8_t *data, uint32_t len);

/**
 * @brief Send data via indication
 *
 * This function sends data to the connected client using indication mechanism.
 * Indications are two-way transmissions that require acknowledgment from the client.
 * The data is sent to the TX characteristic of the data path service.
 *
 * @param connhdl Connection index of the target device
 * @param data Pointer to the data buffer to send
 * @param len Length of the data to send
 */
void app_datapath_server_send_data_via_indication(uint16_t connhdl, uint8_t *data, uint32_t len);

void app_datapath_server_register_event_callback(app_datapath_event_cb cb);

void app_datapath_server_send_data_via_notification(uint16_t connhdl, uint8_t *data, uint32_t len);

void app_datapath_server_send_data_via_indication(uint16_t connhdl, uint8_t *data, uint32_t len);

void app_datapath_server_register_event_cb(app_datapath_event_cb callback);

void app_datapath_server_register_le_rs_callback(app_datapath_server_role_switch_callback_t callback);

void app_datapath_server_register_rx_done(app_datapath_server_data_received_callback_func_t callback);

#endif /* CFG_APP_DATAPATH_SERVER */

#ifdef __cplusplus
}
#endif
