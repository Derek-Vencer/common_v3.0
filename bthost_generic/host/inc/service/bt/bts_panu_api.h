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
#ifndef __BTS_PANU_API_H__
#define __BTS_PANU_API_H__

#include "bt_common_define.h"
#include "bt_stack_status.h"
#include "bt_panu_types.h"
#include "pan_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief BTS PANU API Return Code Definitions
 */
#define BTS_PANU_SUCCESS                0       /* Operation successful */
#define BTS_PANU_ERR_INVALID_PARAM      -1      /* Invalid input parameter */
#define BTS_PANU_ERR_UNSUPPORTED_ROLE   -2      /* Unsupported PANU role */
#define BTS_PANU_ERR_NO_DEVICE_ID       -3      /* No available device ID */
#define BTS_PANU_ERR_CHANNEL_ALLOC_FAIL -4      /* PANU channel allocation failed */
#define BTS_PANU_ERR_NOT_CONNECTED      -5      /* PANU not connected to remote device */

/**
 * @brief Masked MAC Address Print Macro
 */
#define BTS_PANU_PRINT_MAC(addr) \
    (addr)->address[0], (addr)->address[4], (addr)->address[5]

/**
 * @brief Register PANU callback functions
 * @param panu_register_callbacks Pointer to callback structure
 */
void bts_panu_register_callbacks(bt_panu_register_t *panu_register_callbacks);

/**
 * @brief Check if PANU is connected with remote device
 * @param remote_addr Remote Bluetooth address
 * @return true if connected, false otherwise
 */
bool bts_panu_is_connected(const bt_bdaddr_t *remote_addr);

/**
 * @brief Connect to PANU profile with remote device
 * @param remote_addr Remote Bluetooth address
 * @param role PANU role (only PANU_ROLE_NA supported)
 * @return BTS_PANU_SUCCESS on success, negative error code on failure
 */
int bts_panu_connect(const bt_bdaddr_t *remote_addr, bt_panu_role_t role);

/**
 * @brief Disconnect PANU profile with remote device
 * @param remote_addr Remote Bluetooth address
 */
void bts_panu_disconnect(const bt_bdaddr_t *remote_addr);

/**
 * @brief Send ARP protocol data to remote PANU device
 * @param remote_addr Remote Bluetooth address
 * @param payload_data ARP data payload
 * @param payload_length Length of payload data
 * @return 0 on success, non-zero on failure
 */
int bts_panu_send_ARP_protocol_data(const bt_bdaddr_t *remote_addr,
    const uint8_t *payload_data, uint16_t payload_length);

/**
 * @brief Send IPv4 protocol data to remote PANU device
 * @param remote_addr Remote Bluetooth address
 * @param payload_data IPv4 data payload
 * @param payload_length Length of payload data
 * @return 0 on success, non-zero on failure
 */
int bts_panu_send_IPv4_protocol_data(const bt_bdaddr_t *remote_addr,
    const uint8_t *payload_data, uint16_t payload_length);

/**
 * @brief Send IPv6 protocol data to remote PANU device
 * @param remote_addr Remote Bluetooth address
 * @param payload_data IPv6 data payload
 * @param payload_length Length of payload data
 * @return 0 on success, non-zero on failure
 */
int bts_panu_send_IPv6_protocol_data(const bt_bdaddr_t *remote_addr,
    const uint8_t *payload_data, uint16_t payload_length);

/**
 * @brief Send address specified ethernet data to remote PANU device
 * @param remote_addr Remote Bluetooth address
 * @param data Ethernet data info
 * @return 0 on success, non-zero on failure
 */
int bts_panu_send_address_specified_protocol_data(const bt_bdaddr_t *remote_addr,
    const bt_panu_ethernet_data_info *data);

/**
 * @brief Initialize PANU profile
 */
void bts_panu_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __BTS_PANU_API_H__ */