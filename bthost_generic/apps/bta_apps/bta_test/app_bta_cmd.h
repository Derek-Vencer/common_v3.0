/***************************************************************************
 *
 * Copyright 2015-2026 BES.
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
#ifndef __APP_BTA_CMD_H__
#define __APP_BTA_CMD_H__

#include "bta_bt_api.h"

typedef struct
{
    const char* string;
    void (*cmd_function)(const char* param, uint32_t param_len);
} bta_cmd_table_t;

#ifdef __cplusplus
extern "C" {
#endif

bool app_bta_add_cmd_table(uint8_t cmd_number, const bta_cmd_table_t* cmd_table);

bool app_bta_delete_cmd_table(const bta_cmd_table_t* cmd_table);

// Note:Each product UI needs to call initialization
void app_bta_cmd_init(void);

bool app_bta_scan_bdaddr_from_string(const char* param, uint32_t len, bt_bdaddr_t *out_addr);

// Init subtest cmd table
void app_bta_bt_api_cmd_init(void);

#ifdef __cplusplus
}
#endif

#endif // __APP_BTA_CMD_H__

