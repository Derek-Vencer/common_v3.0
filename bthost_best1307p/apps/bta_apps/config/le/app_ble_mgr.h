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

#include "bt_le_types.h"

typedef void (*app_ble_mgr_adv_report_cb)(const ble_bdaddr_t *peer_addr, int8_t rssi, const uint8_t *adv_buf, uint8_t len);

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the app ble connection manager
 *
 * @note This function should be called during system initialization
 *       before any Bluetooth connections are established.
 *
 * @param[in] Device Appearance
 */
void app_ble_mgr_init(uint16_t appearance);

/**
 * @brief Deinitialize the app ble connection manager
 *
 * @note This function should be called during system shutdown or
 *       when the service needs to be temporarily disabled.
 */
void app_ble_mgr_deinit(void);

void app_ble_mgr_scan_result_report_cb_register(app_ble_mgr_adv_report_cb adv_report_cb);

#ifdef __cplusplus
}
#endif
