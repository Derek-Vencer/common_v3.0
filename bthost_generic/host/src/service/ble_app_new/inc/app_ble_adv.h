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
#ifndef __APP_BLE_ADV_H__
#define __APP_BLE_ADV_H__

#include "gap_service.h"
#include "ble_core_common.h"

#define BLE_BASIC_ADV_HANDLE        0x00 // for legacy ble, ota, ai, dp, tile etc. advertising
#define BLE_AUDIO_ADV_HANDLE        0x01 // for ble audio adertising
#define BLE_GFPS_ADV_HANDLE         0x02
#define BLE_SPOT_ADV_HANDLE         0x03
#define BLE_SWIFT_ADV_HANDLE        0x04
#define BLE_FINDMY_ADV_HANDLE       0x05
#define BLE_AI_ADV_HANDLE           0x06
#define BLE_CUSTOMER0_ADV_HANDLE    0x07
#define BLE_CUSTOMER1_ADV_HANDLE    0x08
#define BLE_CUSTOMER2_ADV_HANDLE    0x09
#define BLE_CUSTOMER3_ADV_HANDLE    0x0a
#define BLE_MAX_FIXED_ADV_HANDLE    0x0b

#define BLE_ADV_ACTV_ENV_NUM_MAX    (7)

typedef struct ble_adv_activity_t
{
    /// Adv info provided by internal
    uint8_t         adv_handle;
    /// Adv param to be filled by external
    BLE_ADV_USER_E  user;
    uint32_t        custom_adv_interval_ms;
    gap_adv_param_t adv_param;
} ble_adv_activity_t;

typedef bool (*app_ble_adv_activity_func)(ble_adv_activity_t *adv);

typedef bool (*app_ble_enable_adv_cb_func)(uint8_t adv_user);

typedef bool (*app_ble_adv_mgr_cb_func)(ble_adv_activity_t *adv, app_ble_adv_activity_func func);

#ifdef __cplusplus
extern "C" {
#endif

/* Internal use adv api */
void app_ble_adv_set_param(BLE_ADV_PARAM_T *param);
void app_ble_enable_adv_cb_register(app_ble_enable_adv_cb_func cb);
ble_adv_activity_t *app_ble_get_advertising_by_user(BLE_ADV_USER_E user);
bool ble_adv_is_allowed(void);

/* External use adv api */
/**
 * @brief Set advertising flags field for BLE advertising.
 *
 * This API configures the advertising flags field in the advertising parameters,
 * including LE general discoverable mode and optional BR/EDR support indication.
 *
 * @param adv_param
 *        Pointer to advertising parameter structure to be configured.
 *
 * @param simu_bredr_support
 *        True to indicate simultaneous BR/EDR and LE controller support,
 *        false to indicate LE-only support.
 */
void app_ble_dt_set_flags(gap_adv_param_t *adv_param, bool simu_bredr_support);

/**
 * @brief Set local device name in advertising data.
 *
 * This API sets a custom local device name in the advertising data parameters.
 *
 * @param adv_param
 *        Pointer to advertising parameter structure to be configured.
 *
 * @param cust_le_name
 *        Pointer to the custom LE local name string.
 */
void app_ble_dt_set_local_name(gap_adv_param_t *adv_param, const char *cust_le_name);

/**
 * @brief Clear advertising parameters.
 *
 * This API clears and resets the advertising parameter structure to a clean
 * state before reconfiguration.
 *
 * @param adv_param
 *        Pointer to advertising parameter structure to be cleared.
 */
void app_ble_clean_adv_param(gap_adv_param_t *adv_param);

/**
 * @brief Set advertising interval for a specific advertising user.
 *
 * This API sets the advertising interval requested by a specific interval
 * request user and advertising user. The final interval may be determined
 * by the advertising manager based on multiple requests.
 *
 * @param adv_intv_user
 *        Advertising interval request user.
 *
 * @param adv_user
 *        Advertising user associated with the interval request.
 *
 * @param interval_ms
 *        Requested advertising interval in milliseconds.
 */
void app_ble_param_set_adv_interval(BLE_ADV_INTERVALREQ_USER_E adv_intv_user,
                                    BLE_ADV_USER_E adv_user,
                                    uint32_t interval_ms);

/**
 * @brief Check whether any BLE advertising activity is currently active.
 *
 * @return
 *        True if at least one advertising activity is active, false otherwise.
 */
bool app_ble_is_in_advertising_state(void);

/**
 * @brief Check whether a specific advertising handle is currently advertising.
 *
 * @param adv_hdl
 *        Advertising handle to be checked.
 *
 * @return
 *        True if the specified advertising handle is advertising, false otherwise.
 */
bool app_ble_is_adv_advertising(uint8_t adv_hdl);

/**
 * @brief Refresh advertising state in a generic manner.
 *
 * This API triggers a generic refresh of the advertising state, allowing the
 * advertising manager to re-evaluate and update advertising activities based
 * on the current configuration and internal state.
 */
void app_ble_refresh_adv_state_generic(void);

/**
 * @brief Set advertising TX power in dBm.
 *
 * This API sets the advertising transmit power directly in dBm for the
 * specified advertising activity.
 *
 * @param adv
 *        Pointer to the advertising activity instance.
 *
 * @param tx_power_dbm
 *        Advertising transmit power in dBm.
 */
void app_ble_set_adv_tx_power_dbm(ble_adv_activity_t *adv, int8_t tx_power_dbm);

/**
 * @brief Set advertising TX power by predefined power level.
 *
 * This API sets the advertising transmit power using a predefined power
 * level enumeration.
 *
 * @param adv
 *        Pointer to the advertising activity instance.
 *
 * @param tx_power_level
 *        Advertising transmit power level.
 */
void app_ble_set_adv_tx_power_level(ble_adv_activity_t *adv,
                                    BLE_ADV_TX_POWER_LEVEL_E tx_power_level);

/**
 * @brief Get local Resolvable Private Address (RPA) by advertising handle.
 *
 * This API returns the local RPA currently associated with the specified
 * advertising handle. The RPA is typically used when privacy is enabled
 * for the corresponding advertising activity.
 *
 * @param adv_hdl
 *        Advertising handle used to identify the advertising activity.
 *
 * @return
 *        Pointer to the local RPA address.
 *        Returns NULL if the advertising handle is invalid or RPA is not available.
 */
const uint8_t *app_ble_get_local_rpa_by_adv_hdl(uint8_t adv_hdl);

/**
 * @brief Get advertising handle by advertising user.
 *
 * This API returns the advertising handle associated with the specified
 * advertising user. It is typically used to map a logical advertising user
 * to its corresponding advertising activity.
 *
 * @param user
 *        Advertising user identifier.
 *
 * @return
 *        Advertising handle associated with the given user.
 */
uint8_t app_ble_get_adv_hdl_by_user(BLE_ADV_USER_E user);

/**
 * @brief Control all the advertising sets enabled or disabled with a user bit
 *
 * @param user
 *        ADV sets switch user
 * @param enable_adv
 *        Enable or disable all adv sets
 */
void app_ble_force_switch_adv(enum BLE_ADV_SWITCH_USER_E user, bool enable_adv);

#ifdef __cplusplus
}
#endif

#endif /// __APP_BLE_ADV_H__