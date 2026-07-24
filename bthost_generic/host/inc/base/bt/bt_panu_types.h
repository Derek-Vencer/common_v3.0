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

#include "bt_base_types.h"
#include "bt_common_addr.h"

/**
 * @brief PANU callback function structure
 */
typedef struct {
    /**
     * @brief Callback when PANU channel opened
     * @param remote_addr Remote Bluetooth address
     * @param success true if opened successfully, false otherwise
     */
    void (*panu_opened)(const bt_bdaddr_t *remote_addr, bool success);

    /**
     * @brief Callback when PANU channel closed
     * @param remote_addr Remote Bluetooth address
     * @param error_code Error code (PAN_SUCCESS/PAN_ERROR etc.)
     */
    void (*panu_closed)(const bt_bdaddr_t *remote_addr, uint8_t error_code);

    /**
     * @brief Callback when receive ARP protocol data
     * @param remote_addr Remote Bluetooth address
     * @param payload_data ARP data payload
     * @param payload_length Length of payload data
     */
    void (*panu_receive_ARP_protocol_data)(const bt_bdaddr_t *remote_addr, const uint8_t *payload_data, uint16_t payload_length);

    /**
     * @brief Callback when receive IPv4 protocol data
     * @param remote_addr Remote Bluetooth address
     * @param payload_data IPv4 data payload
     * @param payload_length Length of payload data
     */
    void (*panu_receive_IPv4_protocol_data)(const bt_bdaddr_t *remote_addr, const uint8_t *payload_data, uint16_t payload_length);

    /**
     * @brief Callback when receive IPv6 protocol data
     * @param remote_addr Remote Bluetooth address
     * @param payload_data IPv6 data payload
     * @param payload_length Length of payload data
     */
    void (*panu_receive_IPv6_protocol_data)(const bt_bdaddr_t *remote_addr, const uint8_t *payload_data, uint16_t payload_length);
} bt_panu_register_t;

/**
 * @brief PANU ethernet data info structure
 */
typedef struct {
    uint16_t network_protocol_type;
    uint16_t payload_length;
    uint8_t *payload_data;
    uint8_t *dest_address;
    uint8_t *source_address;
} bt_panu_ethernet_data_info;

/**
 * @brief PANU role definition
 */
typedef enum {
    PANU_ROLE_NA = 0,    /* Network Access client */
    PANU_ROLE_GN,        /* Group Ad-hoc client */
    PANU_ROLE_DIRECT,    /* Direct PANU client */
    PANU_ROLE_INVALID    /* Invalid role */
} bt_panu_role_t;