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

#pragma once

#include "bt_types.h"
#include "bt_panu_types.h"
#include "bt_common_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 ****************************************************************************************
 *    ____ _______         _____          __   __             _____ _____
 *   |  _ \__   __|/\     |  __ \   /\   |  \  | |      /\   |  __ \_   _|
 *   | |_) | | |  /  \    | |__) | /  \  |   \ | |     /  \  | |__) || |
 *   |  _ <  | | / /\ \   |  ___/ / /\ \ | |\ \| |    / /\ \ |  ___/ | |
 *   | |_) | | |/ ____ \  | |    / ____ \| | \   |   / ____ \| |    _| |_
 *   |____/  |_/_/    \_\ |_|   /_/    \_\_|  \_ |  /_/    \_\_|   |_____|
 ****************************************************************************************
 */

/**
 * @brief Register PANU callback functions
 * @param panu_register_callbacks Pointer to callback structure
 */
void bta_panu_register_callbacks(bt_panu_register_t *panu_register_callbacks);

/**
 * @brief Check if PANU is connected with remote device
 * @param remote_addr Remote Bluetooth address
 * @return true if connected, false otherwise
 */
bool bta_panu_is_connected(const bt_bdaddr_t *remote_addr);

/**
 * @brief Connect to PANU profile with remote device (NA role only)
 * @param remote_addr Remote Bluetooth address
 * @return 0 on success, negative value on failure
 */
int bta_panu_connect_profile(const bt_bdaddr_t *remote_addr);

/**
 * @brief Disconnect PANU profile with remote device
 * @param remote_addr Remote Bluetooth address
 */
void bta_panu_disconnect_profile(const bt_bdaddr_t *remote_addr);

/**
 * @brief Send ARP protocol data to remote PANU device
 * @param remote_addr Remote Bluetooth address
 * @param payload_data ARP data payload
 * @param payload_length Length of payload data
 * @return 0 on success, non-zero on failure
 */
int bta_panu_send_ARP_protocol_data(const bt_bdaddr_t *remote_addr,
    const uint8_t *payload_data, uint16_t payload_length);

/**
 * @brief Send IPv4 protocol data to remote PANU device
 * @param remote_addr Remote Bluetooth address
 * @param payload_data IPv4 data payload
 * @param payload_length Length of payload data
 * @return 0 on success, non-zero on failure
 */
int bta_panu_send_IPv4_protocol_data(const bt_bdaddr_t *remote_addr,
    const uint8_t *payload_data, uint16_t payload_length);

/**
 * @brief Send IPv6 protocol data to remote PANU device
 * @param remote_addr Remote Bluetooth address
 * @param payload_data IPv6 data payload
 * @param payload_length Length of payload data
 * @return 0 on success, non-zero on failure
 */
int bta_panu_send_IPv6_protocol_data(const bt_bdaddr_t *remote_addr,
    const uint8_t *payload_data, uint16_t payload_length);

/**
 * @brief Send address specified ethernet data to remote PANU device
 * @param remote_addr Remote Bluetooth address
 * @param data Ethernet data info
 * @return 0 on success, non-zero on failure
 */
int bta_panu_send_address_specified_protocol_data(const bt_bdaddr_t *remote_addr,
    const bt_panu_ethernet_data_info *data);

#ifdef __cplusplus
}
#endif