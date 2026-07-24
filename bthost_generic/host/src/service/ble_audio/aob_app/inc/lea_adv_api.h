/***************************************************************************
 *
 * Copyright (c) 2015-2025 BES Technic
 *
 * Authored by BES CD team (Blueelf Prj).
 *
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

#ifndef __AOB_ADV_API_H__
#define __AOB_ADV_API_H__

#include "bt_common_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

void lea_adv_api_init(void);

void lea_adv_api_on_bonded_dev_connected(const bt_bdaddr_t *device_addr);

/**
 * @brief Enter or exit BLE pairing advertising mode.
 *
 * This API switches the advertising manager into pairing mode or idle mode.
 * In this pairing mode, the device starts advertising for new device pairing
 * and can only use when in idle or pairing mode.
 */
void lea_adv_api_enter_pairing(bool enter);

/**
 * @brief Start reconnect advertising for bonded devices.
 *
 * This API starts directed or undirected reconnect advertising based on
 * the provided bonded device address list. The advertising manager will
 * attempt to reconnect to the devices in the given list.
 *
 * @param dev_list
 *        Pointer to the bonded device address list to reconnect.
 *
 * @param count
 *        Number of devices in the bonded device list.
 */
void lea_adv_api_start_reconnect(const bt_bdaddr_t *dev_list, uint8_t count);

/**
 * @brief Start mixed reconnect advertising.
 *
 * This API starts mixed reconnect advertising, which may involve alternating
 * between directed and undirected advertising, or cycling through multiple
 * bonded devices for reconnect attempts.
 *
 * @param dev_list
 *        Pointer to the bonded device address list to reconnect.
 *
 * @param count
 *        Number of devices in the bonded device list.
 */
void lea_adv_api_start_mix_reconnect(const bt_bdaddr_t *dev_list, uint8_t count);

/**
 * @brief Stop reconnect advertising.
 *
 * This API stops all ongoing reconnect advertising activities and clears
 * reconnect-related states maintained by the advertising manager.
 */
void lea_adv_api_stop_reconnect(void);

/**
 * @brief Enter or exit idle state advertising.
 *
 * This API resume/stops all idle advertising activities in idle state
 *
 * @param enter
 *        Enter or exit connectable when in idle advertising mode.
 * @param adv_interval_ms
 *        Connectable advertising interval when in idle advertising mode.
 */
void lea_adv_api_enter_conenctable(bool enter, uint16_t adv_interval_ms);

/**
 * @brief Update bonded device list for advertising manager.
 *
 * This API updates the bonded device list used by the advertising manager.
 * The updated list may affect subsequent reconnect advertising behaviors.
 *
 * @param dev_list
 *        Pointer to the updated bonded device address list.
 *
 * @param count
 *        Number of devices in the bonded device list.
 */
void lea_adv_api_bonded_update(const bt_bdaddr_t *dev_list, uint8_t count);

/**
 * @brief Enter or exit LEA only mode.
 *
 * This API switches the lea advertising into LEA only mode or dual mode.
 *
 * @param enter_lea_only
 *        Enter or exit lea only mode.
 */
void lea_adv_api_enter_lea_only(bool enter_lea_only);

#ifdef __cplusplus
}
#endif

#endif /// __AOB_ADV_API_H__
