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

typedef uint8_t bt_ibrt_role_t;
#define   BT_IBRT_MASTER       0
#define   BT_IBRT_SLAVE        1
#define   BT_IBRT_UNKNOWN      0xff

typedef enum
{
    BT_LOCATION_UNKNOWN = 0,
    BT_LOCATION_LEFT    = 1,
    BT_LOCATION_RIGHT   = 2,
} bt_location_t;

typedef uint8_t bt_ui_role_t; // BT_IBRT_MASTER BT_IBRT_SLAVE BT_IBRT_UNKNOWN

typedef void (*bt_tws_cmd_send_handler_t)(uint8_t*, uint16_t);
typedef void (*bt_tws_cmd_receivd_handler_t)(uint16_t, uint8_t*, uint16_t);
typedef void (*bt_tws_rsp_timeout_handler_t)(uint16_t, uint8_t*, uint16_t);
typedef void (*bt_tws_rsp_handler_t)(uint16_t, uint8_t*, uint16_t);
typedef void (*bt_tws_cmd_tx_done_handler_t) (uint16_t, uint16_t, uint8_t*, uint16_t);

typedef struct
{
    uint32_t                        cmdcode;
    const char                      *log_cmd_code_str;
    bt_tws_cmd_send_handler_t       tws_cmd_send;
    bt_tws_cmd_receivd_handler_t    cmdhandler;             /**< command handler function */
    uint32_t                        timeout_ms;
    bt_tws_rsp_timeout_handler_t    rsp_timeout_handler;
    bt_tws_rsp_handler_t            rsp_handler;
    bt_tws_cmd_tx_done_handler_t    cmd_tx_done_handler;
    uint8_t                         param;
} __attribute__((packed)) bt_tws_cmd_instance_t;
