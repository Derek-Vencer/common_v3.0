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

#ifdef __cplusplus
extern "C" {
#endif

void app_spp_demo_init();

void app_spp_demo_connect(const bt_bdaddr_t *addr);

void app_spp_demo_send_data(const uint8_t *data, uint16_t size);

#ifdef __cplusplus
}
#endif
