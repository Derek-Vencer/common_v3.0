/***************************************************************************
 *
 * Copyright 2015-2026 BES.
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
#ifndef __BLE_DIS_H__
#define __BLE_DIS_H__

#if defined(BLE_DIP_ENABLE)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the app ble device info service
 *
 * @note This function should be called during system initialization
 *       before any Bluetooth connections are established.
 */
void app_ble_dis_init(void);

/**
 * @brief Deinitialize the app ble device info service
 *
 * @note This function should be called during system shutdown or
 *       when the service needs to be temporarily disabled.
 */
void app_ble_dis_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* BLE_DIP_ENABLE */

#endif /* __BLE_DIS_H__ */
