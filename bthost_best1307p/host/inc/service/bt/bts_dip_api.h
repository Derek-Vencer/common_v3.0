/****************************************************************************
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

#ifndef __BTS_DIP_API_H__
#define __BTS_DIP_API_H__

#include "bt_dip_types.h"
#include "bts_bt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 ****************************************************************************************
 * @brief       Callback function type for handling dip query state changed event.
 * @param[in]   state: dip query state.
 * @return      none
 ****************************************************************************************
 */
typedef void (*bts_dip_query_state_cb)(const bt_bdaddr_t *address, bts_dip_query_state_t state, uint8_t error_code);

typedef struct {
    bts_dip_query_state_cb query_state_cb;
} bts_dip_callbacks_t;

typedef enum {
    BTS_DIP_CB_USER_APP = 0,
    BTS_DIP_CB_USER_MAX,
} BTS_DIP_CALLBACK_USER_E;

/**
 ****************************************************************************************
 * @brief       Register dip callbacks.
 * @param[in]   user: Indicate who registered.
 * @param[in]   callbacks: Pointer to callbacks for handling dip events.
 * @return      none
 ****************************************************************************************
 */
void bts_dip_register_callbacks(BTS_DIP_CALLBACK_USER_E user, bts_dip_callbacks_t *callbacks);

/**
 ****************************************************************************************
 * @brief       Deregister dip callbacks.
 * @param[in]   user: Indicate who registered.
 * @return      none
 ****************************************************************************************
 */
void bts_dip_deregister_callbacks(BTS_DIP_CALLBACK_USER_E user);

/**
 ****************************************************************************************
 * @brief       Start query pnp info, query state will indicate by query state callback.
 * @param[in]   address: Pointer to the Bluetooth address of the peer device
 * @return      Status of the operation
 ****************************************************************************************
 */
bts_status_t bts_dip_start_query_pnp_info(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Get pnp info, need wait query complete.
 * @param[in]   address: Pointer to the Bluetooth address of the peer device
 * @param[in]   info: Pointer to dip pnp info
 * @return      Status of the operation
 ****************************************************************************************
 */
bts_status_t bts_dip_get_pnp_info(const bt_bdaddr_t *address, bt_dip_pnp_info_t *info);

/**
 ****************************************************************************************
 * @brief       Check the device at this address whether is ios device.
 * @param[in]   address: Pointer to the Bluetooth address of the peer device.
 * @return      Indicate whether is an ios device
 ****************************************************************************************
 */
bool bts_dip_check_is_ios_device(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Determine whether it is an ios device by the obtained vend_id.
 * @param[in]   vend_id: see @bt_dip_pnp_info_t
 * @param[in]   vend_id_source: see @bt_dip_pnp_info_t
 * @return      Indicate whether is an ios device
 ****************************************************************************************
 */
bool bts_dip_check_is_ios_device_by_vend_id(uint16_t vend_id, uint16_t vend_id_source);

#ifdef __cplusplus
}
#endif

#endif /* __BTS_DIP_API_H__ */