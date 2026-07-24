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

#include "cmsis_os2.h"
#include "bta_normal_ux_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint32_t pairing_timeout_value;

    uint16_t open_reconnect_mobile_max_times;
    bool without_reconnect_when_fetch_out_wear_up;
} app_headset_attributes_t;

typedef enum
{
    APP_HEADSET_OPEN = 0,
    APP_HEADSET_CLOSE,
    APP_HEADSET_UNDOCK,
    APP_HEADSET_DOCK,
    APP_HEADSET_WEAR_UP,
    APP_HEADSET_WEAR_DOWN,
} app_headset_box_event_t;

void app_bta_init(void);
bool app_bta_bootmode_handler(void);

void app_bta_box_event_entry(app_headset_box_event_t event);
void app_headset_set_without_reconnect_when_fetch_out_wear_up(bool value);

#ifdef __cplusplus
}
#endif
