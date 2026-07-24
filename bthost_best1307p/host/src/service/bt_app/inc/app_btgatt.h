/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#ifndef __APP_BTGATT_H__
#define __APP_BTGATT_H__
#include "bluetooth.h"

#if defined(__GATT_OVER_BR_EDR__)

#define ATT_SERVICE_UUID 0x0118
#ifdef __cplusplus
extern "C" {
#endif

#define GATT_OVER_BREDR_EVENT_CONTROL_CONNECTED                     (0x21)
#define GATT_OVER_BREDR_EVENT_CONTROL_DISCONNECTED                  (0x22)
#define GATT_OVER_BREDR_EVENT_CONTROL_DATA_IND                      (0x23)
#define GATT_OVER_BREDR_EVENT_CONTROL_DATA_SENT                     (0x24)
#define GATT_OVER_BREDR_EVENT_CONTROL_SET_IDLE                      (0x30)

bool app_btgatt_over_br_edr_enabled(void);
bool app_btgatt_is_connected(uint8_t device_id);
bool app_btgatt_is_connected_by_conidx(uint8_t con_idx);
void app_btgatt_client_create(const bt_bdaddr_t *remote);
void app_btgatt_disconnect(uint8_t device_id);
void app_btgatt_report_prf_state(uint8_t device_id, const bt_bdaddr_t *remote, bool is_connected, uint8_t reason);
void app_btgatt_state_callback_register(void (*p_cb)(const bt_bdaddr_t *remote, bool is_connected, uint8_t reason));
#ifdef __cplusplus
}
#endif

#endif
#endif
