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
#ifndef __BLE_MULTI_CHIP_UPDATA_H__
#define __BLE_MULTI_CHIP_UPDATA_H__

#include "bluetooth.h"
#include "app_ble.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CFG_APP_MCUC

typedef enum {
    MCUC_CHAR_DATA,
    MCUC_CHAR_MAX_NUM,
} mcuc_char_enum_t;
typedef enum {
    /// RX Client Characteristic Configuration
    MCUC_DESC_RX_CFG,
    /// RX Characteristic User Description
    MCUC_DESC_CUD,

    MCUC_DESC_MAX_NUM,

    MCUC_DESC_MASK = 0x10,
} mcuc_desc_enum_t;

typedef enum {
    /// Notified Char
    MCUC_NTF_AND_WRITE_CHAR_DATA               = MCUC_CHAR_DATA,
    /// Read/Write Ntf Char Client characteristic configuration.
    MCUC_RD_WR_NTF_CCC_CFG           = (MCUC_DESC_RX_CFG | MCUC_DESC_MASK),
    /// Read Ntf Char Characteristic user description.
    MCUC_RD_NTF_AND_WR_CUD                  = (MCUC_DESC_CUD | MCUC_DESC_MASK),
} mcuc_code_enum_t;

typedef struct {
    void(*mcuc_connected_done_cb)(uint8_t conidx);
    void(*mcuc_disconnected_done_cb)(uint8_t conidx);
    void(*mcuc_mtu_exchanged_done_cb)(uint8_t conidx, uint16_t mtu);
    void(*mcuc_discover_done_cb)(uint8_t conidx);
    void(*mcuc_data_received_cb)(uint8_t conidx, const uint8_t *p_buff, uint16_t bufLength);
    void(*mcuc_tx_done_cb)(uint8_t conidx, uint8_t err_code);
} app_mcuc_event_cb_t;

void ble_mcuc_start_discover(uint8_t conidx);
void app_mcuc_register_callback(app_mcuc_event_cb_t *callback);
void app_mcuc_control_notification(uint8_t conidx, bool isEnable);
bool app_mcuc_send_data_via_write_command(uint8_t conidx, uint8_t* ptrData, uint32_t length);
bool app_mcuc_send_data_via_write_request(uint8_t conidx, uint8_t* ptrData, uint32_t length);
void ble_mcuc_init(void);
#endif
#ifdef CFG_APP_MCUS
typedef enum
{
    APP_MCUS_CCC_CHANGED = 0,
    APP_MCUS_DIS_CONN,
    APP_MCUS_RECEVICE_DATA,
    APP_MCUS_MTU_UPDATE,
    APP_MCUS_SEND_DONE,
} APP_MCUS_EVENT_TYPE_E;
typedef struct {
    void(*mcus_connected_done_cb)(uint8_t conidx);
    void(*mcus_disconnected_done_cb)(uint8_t conidx);
    void(*mcus_mtu_exchanged_done_cb)(uint8_t conidx, uint16_t mtu);
    void(*mcus_discover_done_cb)(uint8_t conidx);
    void(*mcus_data_received_cb)(uint8_t conidx, const uint8_t *p_buff, uint16_t bufLength);
    void(*mcus_tx_done_cb)(uint8_t conidx, uint8_t err_code);
} app_mcus_event_cb_t;
void ble_mcus_init(void);
void app_mcus_register_callback(app_mcus_event_cb_t* cb);
void app_mcus_event_unreg(void);
void app_mcus_send_rx_cfm(uint8_t conidx);
bool app_mcus_send_data_via_ntf(uint8_t conidx, uint8_t *data, uint32_t len);
bool app_mcus_send_data_via_ind(uint8_t conidx, uint8_t *data, uint32_t len);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BLE_MULTI_CHIP_UPDATA_H__ */
