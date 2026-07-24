/***************************************************************************
 *
 * Copyright 2015-2024 BES.
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
#ifndef __BLE_THROUGHPUT_H__
#define __BLE_THROUGHPUT_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BLE_THROUGHPUT_TEST

typedef enum custom_event_type
{
    CUSTOM_EVENT_GATTC_NOTIFICATION_EVENT           = 0,
    CUSTOM_EVENT_GATTS_WRITE_EVENT                  = 1,
    CUSTOM_EVENT_GATTC_WRITE_RESPOND_EVENT          = 2,
    CUSTOM_EVENT_GATTC_WRITE_CMD_TX_DONE            = 3,
    CUSTOM_EVENT_GATTS_NOTIFICATION_TX_DONE_EVENT   = 4,
} custom_event_type_e;

typedef enum
{
    NUMERIC_ACCEPT = 0,
    NUMERIC_REJECT = 1,
} numeric_status_t;

typedef struct
{
    uint8_t *ediv;
    uint8_t *rand;
    uint8_t *ltk;
} smp_encrypt_data_t;

typedef struct
{
    uint8_t ltk[16];
    uint32_t len;
} SMP_LTK_INFO_T;

typedef struct
{
    uint8_t status;
    uint16_t conn_handle;
    uint8_t role;
    ble_bdaddr_t peer_addr;
    bt_bdaddr_t loc_resolve_addr;
    bt_bdaddr_t peer_resolve_addr;
    struct connect_param_t
    {
        uint16_t conn_interval;
        uint16_t peripheral_latency;
        uint16_t superv_timeout;
        uint8_t central_clock_accuracy;
    } conn_param;
} gap_connect_complete_param_t;

typedef struct
{
    uint8_t auth_method;
    uint8_t passkey[6];
} passkey_display_t;

typedef struct
{
    uint16_t attr_handle;
    uint16_t attr_value_len;
    const uint8_t *attr_value;
} gattc_notification_t;

typedef struct
{
    uint16_t attr_handle;
    uint8_t attr_status;
    uint8_t write_op;
    uint16_t value_len;
    uint16_t offset;
    const uint8_t *value;
} gatt_write_t;

typedef struct
{
    uint16_t attr_handle;
    uint8_t status;
    uint8_t write_op;
    uint16_t value_len;
    uint16_t offset;
    const uint8_t *value;
} gatt_write_rsp_t;

typedef struct
{
    custom_event_type_e evt_type;
    uint16_t conn_handle;
    uint16_t reserve;
    union
    {
        gattc_notification_t notification;
        gatt_write_t write;
        gatt_write_rsp_t write_rsp;
    } u;
} custom_event_t;

typedef enum
{
    GATT_WRITE_REQUEST,
    GATT_WRITE_COMMAND,
    GATT_SIGNED_WRITE_COMMAND,
    GATT_PREPARE_WRITE_REQUEST,
    GATT_EXECUTE_WRITE_REQUEST,
} gatt_write_opertation_t;

typedef enum
{
    GATT_CANCEL_WRITE,
    GATT_EXECUTE_WRITE,
} gatt_write_flag_t;

typedef struct
{
    uint16_t connhdl;
    uint16_t mtu;
} gatt_mtu_req_t;

typedef gatt_mtu_req_t gatt_mtu_rsp_t;

typedef struct
{
    uint8_t write_op;
    uint8_t write_flag;
    uint16_t attr_handle;
    uint16_t offset;
    uint16_t len;
    const uint8_t *data;
} gatt_write_data_t;

void ble_throughput_init(void);
void ble_throughput_gattc_discover_service_test(uint32_t connhdl, uint32_t svc_uuid);
void ble_throughput_gatt_throughput_test(uint32_t connhdl, uint32_t att_hdl, uint32_t write_op, uint32_t start);

void ble_throughput_start_connectable_adv(void);
uint32_t ble_throughput_client_gatt_mtu_exchange_and_basic_svc_discovery(uint16_t conn_handle, uint16_t mtu);
uint32_t ble_throughput_gatts_mtu_exchange_reply(uint16_t conn_handle, uint16_t mtu);
uint32_t ble_throughput_gattc_write(uint8_t prf_id, uint16_t conn_handle, const gatt_write_data_t *p_write_data);
uint32_t ble_throughput_gatts_send_notification_by_handle(uint16_t conn_handle, const gatt_char_notify_by_handle_t *notify);
uint32_t ble_throughput_gattc_discover_service(uint8_t prf_id, uint16_t conn_handle, uint16_t svc_uuid);
uint8_t ble_throughput_gattc_add_profile(gatt_profile_callback_t cb, const gattc_cfg_t *cfg);
uint32_t ble_throughput_gatts_add_service(const gatt_attribute_t *service, uint16_t attr_count, gatt_server_callback_t cb, const gatts_cfg_t *cfg);
uint32_t ble_throughput_start_gatt_mtu_exchange(uint16_t conn_handle, uint16_t mtu);
void ble_throughput_set_rx_buffer_len(uint16_t connhdl, uint16_t max_rx_octets, uint16_t max_rx_time_us);

#endif /* BLE_THROUGHPUT_TEST */

#ifdef __cplusplus
}
#endif
#endif /* __BLE_THROUGHPUT_H__ */
