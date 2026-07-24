/***************************************************************************
 *
 * Copyright (c) 2015-2024 BES Technic
 *
 * Authored by BES CD team (Blueelf Prj).
 *
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
#ifndef __RANGING_H__
#define __RANGING_H__

#include "bt_sys_config.h"

#include "ranging_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (BLE_RAP_CLIENT_SUPPORT)
#include "rap_client.h"

// * the upper layer is ble ranging app, and this app only need to known the information about the client
// * there is no need for it to know the existence of the RAP server, because it is only bulit upon the RAP client
void rap_register_client_callback(const rapc_upper_cb_t *cb);
#endif

#if (BLE_RAP_SERVER_SUPPORT)
#include "rap_server.h"

void rap_register_server_callback(const raps_upper_cb_t *cb);
#endif

void rap_init(void);
void rap_deinit(void);

uint16_t rap_get_connhdl_by_peer_addr(const ble_bdaddr_t *p_ble_bdaddr);

#ifdef __cplusplus
}
#endif

#endif  /* __RANGING_H__ */
