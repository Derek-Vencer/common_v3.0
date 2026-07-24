
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

#ifdef SWIFT_ENABLED

/**
 * @brief Initialize the app ble swift manager
 *
 * @note This function should be called during system initialization
 *       before any Bluetooth connections are established.
 */
void app_ble_swift_init(void);

/**
 * @brief Deinitialize the app ble swift manager
 *
 * @note This function should be called during system shutdown or
 *       when the service needs to be temporarily disabled.
 */
void app_ble_swift_deinit(void);

void app_ble_swift_adv_refresh(void);

void app_swift_enter_pairing_mode(void);

void app_swift_exit_pairing_mode(void);

#endif // SWIFT_ENABLED

#ifdef __cplusplus
}
#endif
