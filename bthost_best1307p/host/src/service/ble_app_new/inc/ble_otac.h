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
#ifndef __BLE_OTAC_H__
#define __BLE_OTAC_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    OTAC_CHAR_DATA,
    OTAC_CHAR_MAX_NUM,
} otac_char_enum_t;

typedef enum {
    /// RX Client Characteristic Configuration
    OTAC_DESC_RX_CFG,
    /// RX Characteristic User Description
    OTAC_DESC_CUD,

    OTAC_DESC_MAX_NUM,

    OTAC_DESC_MASK = 0x10,
} otac_desc_enum_t;

typedef enum {
    /// Notified Char
    OTAC_NTF_AND_WRITE_CHAR_DATA               = OTAC_CHAR_DATA,

    /// Read/Write Ntf Char —— Client characteristic configuration.
    OTAC_RD_WR_NTF_CCC_CFG           = (OTAC_DESC_RX_CFG | OTAC_DESC_MASK),
    /// Read Ntf Char —— Characteristic user description.
    OTAC_RD_NTF_AND_WR_CUD                  = (OTAC_DESC_CUD | OTAC_DESC_MASK),
} otac_code_enum_t;

typedef struct {
    void(*otac_connected_done_cb)(uint8_t conidx);
    void(*otac_disconnected_done_cb)(uint8_t conidx);
    void(*otac_mtu_exchanged_done_cb)(uint8_t conidx, uint16_t mtu);
    void(*otac_discover_done_cb)(uint8_t conidx);
    void(*otac_data_received_cb)(uint8_t conidx, const uint8_t *p_buff, uint16_t bufLength);
    void(*otac_tx_done_cb)(uint8_t conidx, uint8_t err_code);
} app_otac_event_cb_t;

void ble_otac_start_discover(uint8_t conidx);
void app_otac_register_callback(app_otac_event_cb_t *callback);
void app_otac_control_notification(uint8_t conidx, bool isEnable);
bool app_otac_send_data_via_write_command(uint8_t conidx, uint8_t* ptrData, uint32_t length);
bool app_otac_send_data_via_write_request(uint8_t conidx, uint8_t* ptrData, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif //__BLE_OTAC_H__
