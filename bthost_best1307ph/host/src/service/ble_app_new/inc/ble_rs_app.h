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
 ****************************************************************************/
#ifndef __BLE_ROLESWITCH_H__
#define __BLE_ROLESWITCH_H__

#include "bluetooth.h"

#ifdef __cplusplus
extern "C" {
#endif

int ble_rs_app_init(void);
int ble_rs_app_deinit(void);

void ble_rs_app_tws_sync_info_receive_handler(uint8_t *p_info, uint16_t len);
void ble_rs_app_tws_state_changed(bool connected);
void ble_rs_app_roleswitch_complete(uint8_t new_role);
void ble_rs_app_roleswitch_start(uint8_t curr_ui_role);

#ifdef __cplusplus
}
#endif

#endif /* __BLE_ROLESWITCH_H__ */
