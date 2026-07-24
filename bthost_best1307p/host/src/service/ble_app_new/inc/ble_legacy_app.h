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
#ifndef __BLE_LEGACY_APP_H__
#define __BLE_LEGACY_APP_H__

#include "bluetooth.h"
#include "app_ble.h"

#ifdef __cplusplus
extern "C" {
#endif

int ble_legacy_app_init(void);
int ble_legacy_app_deinit(void);

void ble_legacy_app_tws_sync_info(void);
void ble_legacy_app_tws_sync_info_receive_handler(uint8_t *p_info, uint16_t len);
void ble_legacy_app_tws_state_changed(bool connected);

/**
 * @brief CUSTOM Adv register prepare adv param handler api
 *
 * @param[in] adv_handle Custom adv handle
 * @param[in] adv_activity_func Custom adv prepare handler
 *
 * @return ble_adv_activity_t* adv activity register success or NULL
 */
ble_adv_activity_t *ble_legacy_app_register_advertising(uint8_t adv_handle, app_ble_adv_activity_func adv_activity_func);

#ifdef __cplusplus
}
#endif

#endif /* __BLE_LEGACY_APP_H__ */
