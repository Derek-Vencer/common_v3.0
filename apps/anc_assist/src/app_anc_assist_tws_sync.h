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
#ifndef __APP_ANC_ASSIST_TWS_SYNC_H__
#define __APP_ANC_ASSIST_TWS_SYNC_H__

#include "plat_types.h"
#include "hal_aud.h"
#include "anc_assist.h"

typedef enum {
    VOICE_ASSIST_TWS_ROLE_PRIMARY     = 0,
    VOICE_ASSIST_TWS_ROLE_SECONDARY   = 1,
    VOICE_ASSIST_TWS_ROLE_UNKNOWN     = (0xFF)
} voice_assist_sync_role_t;

#ifdef __cplusplus
extern "C" {
#endif

int32_t app_voice_assist_tws_sync_init();

/* Fix compilation issues caused by shared branches in bthost */
int32_t anc_assist_tws_sync_status_change(uint8_t *buf, uint32_t len);

int32_t app_voice_assist_tws_sync_status_change(uint8_t *buf, uint32_t len);

bool app_voice_assist_check_tws_connected(void);

voice_assist_sync_role_t app_voice_assist_get_tws_ui_role(void);

voice_assist_sync_role_t app_voice_assist_get_tws_bcr_role(void);

int32_t app_voice_assist_tws_send_sync_info(uint32_t user, uint8_t *sync_res, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif
