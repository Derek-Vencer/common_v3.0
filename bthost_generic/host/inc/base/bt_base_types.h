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

#include <stdbool.h>
#include <stdint.h>
#include "bt_common_addr.h"

#define BT_BD_ADDR_LEN          (6)
#define BT_INVALID_DEVICE_ID    (0xff)
#define BT_INVALID_HANDLE       (0xffff)

#define BT_UUID_MAX_LEN         (16)

#ifndef BT_DEVICE_NUM
#define BT_DEVICE_NUM 2
#endif

typedef struct bt_bdaddr_list
{
    bt_bdaddr_t addrs[BT_DEVICE_NUM];
} bt_bdaddr_list_t;
