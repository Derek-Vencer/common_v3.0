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
#ifndef __BLE_GATT_I_H__
#define __BLE_GATT_I_H__

#include "gatt_service.h"

typedef struct gatt_srv_user
{
    struct gatt_srv_user *next;
    uint8_t user_lid;
    uint8_t reg_svc_num;
    gatt_server_callback_t svc_callback;
} gatt_srv_usr_t;

typedef struct
{
    gatt_srv_usr_t *next;
} gatt_srv_usr_list_t;

#endif /* __BLE_GATT_I_H__ */