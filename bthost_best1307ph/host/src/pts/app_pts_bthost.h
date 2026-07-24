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
#ifndef __APP_PTS_BTHOST_H__
#define __APP_PTS_BTHOST_H__

#include "bluetooth.h"

typedef struct
{
    const char* string;
    void (*cmd_function)(const char* param, uint32_t param_len);
} app_bt_host_cmd_table_t;

typedef struct
{
    //a2dp profile config
    bool need_handle_specially;
    uint8_t handle_code;
    //hfp profile config
    bool hfp_hf_pts_acs_bv_09_i;
    bool hfp_ag_pts_ecc;
    //
    bool allow_accpet_sco_req;
} app_pts_config_t;

#ifdef __cplusplus
extern "C" {
#endif

app_pts_config_t *app_pts_get_cfg(void);

bool app_bt_host_add_cmd_table(uint8_t cmd_number, const app_bt_host_cmd_table_t* cmd_table);

bool app_bt_host_delete_cmd_table(const app_bt_host_cmd_table_t* cmd_table);

bt_bdaddr_t *app_bt_get_pts_address(void);

unsigned int app_bt_host_cmd_callback(unsigned char *buf, unsigned int length);

bool app_bt_scan_bdaddr_from_string(const char* param, uint32_t len, bt_bdaddr_t *out_addr);

void app_pts_cmd_init(void);

#ifdef __cplusplus
}
#endif

#endif // __APP_PTS_BTHOST_H__

