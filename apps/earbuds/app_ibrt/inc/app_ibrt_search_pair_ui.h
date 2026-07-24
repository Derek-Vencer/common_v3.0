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

#ifdef __cplusplus
extern "C" {
#endif

void app_ibrt_search_ui_init(bool boxOperation, uint8_t evt_type);
void app_ibrt_search_ui_config_load(void *config);
void app_ibrt_enter_limited_mode(void);
void app_ibrt_exit_limited_mode(void);
void app_ibrt_reconfig_btAddr_from_nv();
void app_ibrt_start_tws_searching();
void app_start_tws_serching_direactly();

#ifdef __cplusplus
}
#endif