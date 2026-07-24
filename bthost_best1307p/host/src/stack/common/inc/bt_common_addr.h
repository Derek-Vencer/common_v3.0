/***************************************************************************
 *
 * Copyright 2015-2024 BES.
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
 * @brief xxx.
 *
 ****************************************************************************/
#ifndef __BT_COMMON_ADDR_H__
#define __BT_COMMON_ADDR_H__

#ifdef __cplusplus
extern "C" {
#endif

/****************************** header include ********************************/
#include <stdint.h>

/***************************** external declaration *****************************/

/***************************** macro defination *******************************/

/***************************** type defination ********************************/

// TODO:
#define BT_ADDRESS_LEN         (6)

typedef struct bdaddr_t {
    uint8_t address[BT_ADDRESS_LEN];
} __attribute__ ((packed)) bt_bdaddr_t;

typedef enum {
    BT_ADDR_TYPE_PUBLIC = 0x00,
    BT_ADDR_TYPE_RANDOM = 0x01,
    BT_ADDR_TYPE_PUB_IA = 0x02,
    BT_ADDR_TYPE_RND_IA = 0x03,
} bt_addr_type_t;

typedef struct ble_bdaddr_t
{
    uint8_t addr[BT_ADDRESS_LEN];
    uint8_t addr_type;
} __attribute__ ((packed)) ble_bdaddr_t;

/***************************** variable defination *****************************/

/***************************** function declaration ****************************/

#ifdef __cplusplus
}
#endif

#endif /* __BT_COMMON_ADDR_H__ */