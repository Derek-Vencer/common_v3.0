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

#if (defined(BES_OTA) || defined(BES_OTA_BASIC)) && !defined(OTA_OVER_TOTA_ENABLED) && !defined(NUTTX_BLE_OTA)

typedef enum
{
    APP_OTA_CCC_CHANGED = 0,
    APP_OTA_DIS_CONN,
    APP_OTA_RECEVICE_DATA,
    APP_OTA_MTU_UPDATE,
    APP_OTA_SEND_DONE,
} APP_OTA_EVENT_TYPE_E;

typedef struct
{
    uint8_t event_type;
    uint8_t conidx;
    uint16_t connhdl;
    union
    {
        uint8_t ntf_en; // APP_OTA_CCC_CHANGED
        uint16_t mtu; // APP_OTA_MTU_UPDATE
        uint8_t status; // APP_OTA_SEND_DONE
        struct   // APP_OTA_RECEVICE_DATA
        {
            uint16_t data_len;
            uint8_t *data;
        } receive_data;
    } param;
} app_ota_event_param_t;

typedef void(*app_ota_event_callback)(app_ota_event_param_t *param);

/**
 * @brief Initialize the app ble ota manager
 *
 * @note This function should be called during system initialization
 *       before any Bluetooth connections are established.
 */
void app_ble_ota_init(void);

/**
 * @brief Deinitialize the app ble ota manager
 *
 * @note This function should be called during system shutdown or
 *       when the service needs to be temporarily disabled.
 */
void app_ble_ota_deinit(void);

void app_ota_event_reg(app_ota_event_callback cb);

void app_ota_event_unreg(void);

void app_ota_send_rx_cfm(uint8_t conidx);

bool app_ota_send_notification(uint8_t conidx, uint8_t *data, uint32_t len);

bool app_ota_send_indication(uint8_t conidx, uint8_t *data, uint32_t len);

#endif // (defined(BES_OTA) || defined(BES_OTA_BASIC)) && !defined(OTA_OVER_TOTA_ENABLED) && !defined(NUTTX_BLE_OTA)

#ifdef __cplusplus
}
#endif
