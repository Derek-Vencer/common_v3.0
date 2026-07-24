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
#ifndef __BLE_ADV_MGR_H__
#define __BLE_ADV_MGR_H__

#include "app_ble_adv.h"

typedef enum
{
    BLE_ADV_USAGE_DEFAULT           = 0x00,
    /// Only one ear will advertise with special params
    BLE_ADV_USAGE_LE_APP            = BLE_ADV_USAGE_DEFAULT,
    /// TWO ear will advertise using public addr
    BLE_ADV_USAGE_LE_AUDIO          = 0x01,
    /// Host does not do any modify on this advertising
    BLE_ADV_USAGE_LE_TRANSPARENT    = 0x02,

    BLE_GAP_ADV_USAGE_MAX,
} ble_adv_usage_e;

typedef enum
{
    /// Freeman or headset adv state
    BLE_ADV_STATE_FREEMAN_STEREO = 0x00,

    /// Primary earbuds adv state
    BLE_ADV_STATE_PRIMARY_TWS_DISCONNECTED = 0x01,
    BLE_ADV_STATE_PRIMARY_TWS_CONNECTED = 0x02,

    /// Secondary earbuds adv state
    BLE_ADV_STATE_SECONDARY_TWS_DISCONNECTED = 0x03,
    BLE_ADV_STATE_SECONDARY_TWS_CONNECTED = 0x04,

    /// Not initialized adv state
    BLE_ADV_STATE_NOT_INITILIZED = 0x05,

    BLE_GAP_ADV_STATE_MAX,
} ble_adv_mgr_state_e;

#ifdef __cplusplus
extern "C" {
#endif

#if defined (BT_SVC_MODULE_TWS_ENABLED)
/**
 * @brief Notify BLE module that TWS connection state has changed.
 *
 * This API is used to inform the BLE subsystem of TWS link state changes.
 * It allows the BLE module to adjust advertising, connection, or role-related
 * behaviors according to the current TWS connectivity.
 *
 * @param connected
 *        True if TWS devices are connected, false otherwise.
 */
void app_ble_adv_mgr_recv_tws_state_changed(bool connected);

/**
 * @brief Notify BLE module that TWS devices entered freeman mode.
 *
 * This API is used to inform the BLE subsystem of enter freeman mode.
 * It allows the BLE module to adjust advertising, connection, or role-related
 * behaviors according to the current freeman mode.
 *
 * @param enter
 *        True if TWS devices entered freeman mode, false otherwise.
 */
void app_ble_adv_mgr_recv_enter_freeman_mode(bool enter);

#endif /// BT_SVC_MODULE_TWS_ENABLED

/**
 * @brief Register a BLE advertising activity.
 *
 * This API creates and registers a BLE advertising activity identified by
 * the given advertising handle. The registered activity is associated with
 * a specific usage type and callback functions, which will be invoked by
 * the advertising manager during the advertising lifecycle.
 *
 * @param adv_handle
 *        Advertising handle used to uniquely identify this advertising activity.
 *
 * @param usage
 *        Advertising usage type, indicating the purpose of this activity
 *        (e.g. Phone application adv, box advertsing, lea adv, etc.).
 *
 * @param adv_activity_func
 *        Pointer to the advertising activity callback functions. These callbacks
 *        are used to handle advertising events such as start, stop, param prepare.
 *
 * @return
 *        Pointer to the registered advertising activity instance.
 *        Returns NULL if registration fails.
 */
ble_adv_activity_t *app_ble_register_advertising(uint8_t adv_handle,
                                                 ble_adv_usage_e usage,
                                                 app_ble_adv_activity_func adv_activity_func);

/**
 * @brief Unregister a BLE advertising activity.
 *
 * This API unregisters and releases the BLE advertising activity associated
 * with the specified advertising handle. The advertising activity will be
 * stopped if it is currently active, and all related resources will be freed.
 *
 * @param adv_handle
 *        Advertising handle used to identify the advertising activity to be removed.
 */
void app_ble_unregister_advertising(uint8_t adv_handle);

#ifdef __cplusplus
}
#endif

#endif /// __BLE_ADV_MGR_H__