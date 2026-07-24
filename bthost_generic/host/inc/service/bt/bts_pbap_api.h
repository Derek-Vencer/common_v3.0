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
#ifndef __BTS_PBAP_API_H__
#define __BTS_PBAP_API_H__

#include "bts_bt_types.h"
#include "bt_pbap_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 ****************************************************************************************
 * @brief       Register PBAP client callback functions
 * @param[in]   user: Callback user identifier to distinguish different callback users
 * @param[in]   callbacks: Pointer to the PBAP client callback function structure
 * @return      None
 ****************************************************************************************
 */
void bts_pbap_register_callbacks(bt_pbap_callback_user_t user, bt_pbap_callbacks_t *callbacks);

/**
 ****************************************************************************************
 * @brief       Deregister PBAP client callback functions
 * @param[in]   user: Callback user identifier to be deregistered
 * @return      None
 ****************************************************************************************
 */
void bts_pbap_deregister_callbacks(bt_pbap_callback_user_t user);

/**
 ****************************************************************************************
 *    _____  ____          _____     _____ _      _____ ______ _   _ _______
 *   |  __ \|  _ \   /\   |  __ \   / ____| |    |_   _|  ____| \ | |__   __|
 *   | |__) | |_) | /  \  | |__) | | |    | |      | | | |__  |  \| |  | |
 *   |  ___/|  _ < / /\ \ |  ___/  | |    | |      | | |  __| | . ` |  | |
 *   | |    | |_) / ____ \| |      | |____| |____ _| |_| |____| |\  |  | |
 *   |_|    |____/_/    \_\_|       \_____|______|_____|______|_| \_|  |_|
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief       Get the size of the phonebook from the remote PBAP server
 * @param[in]   remote: Pointer to the Bluetooth address of the remote device
 * @param[in]   search_param: Pointer to the PBAP search parameter structure for filtering
 * @return      None
 ****************************************************************************************
 */
void bts_pbap_client_get_phonebook_size(const bt_bdaddr_t *remote, const bt_pbap_request_search_param_t *search_param);

/**
 ****************************************************************************************
 * @brief       Get the phonebook data (PB) from the remote PBAP server
 * @param[in]   remote: Pointer to the Bluetooth address of the remote device
 * @param[in]   search_param: Pointer to the PBAP search parameter structure for filtering
 * @return      None
 ****************************************************************************************
 */
void bts_pbap_client_get_phonebook_pb(const bt_bdaddr_t *remote, const bt_pbap_request_search_param_t *search_param);

/**
 ****************************************************************************************
 * @brief       Search vCard entries by telephone number from the remote PBAP server
 * @param[in]   remote: Pointer to the Bluetooth address of the remote device
 * @param[in]   search_param: Pointer to the PBAP search parameter structure for filtering
 * @return      True if the search request is sent successfully, False otherwise
 ****************************************************************************************
 */
bool bts_pbap_client_list_search_vcard_by_telephone(const bt_bdaddr_t *remote, const bt_pbap_request_search_param_t *search_param);

#ifdef __cplusplus
}
#endif

#endif /* __BTS_HFP_API_H__ */