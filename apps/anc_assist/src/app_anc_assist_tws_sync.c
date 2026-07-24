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
#include "hal_trace.h"
#include "anc_assist_anc.h"
#include "app_anc_assist.h"
#include "app_anc_assist_tws_sync.h"
#if !defined(VOICE_ASSIST_STEREO)
#include "bts_core_if.h"
#include "bts_tws_api.h"
#include "app_ibrt_customif_cmd.h"
#endif

#define SYNC_BUF_SIZE (60)

typedef struct {
    uint32_t user;
    uint32_t len;
    uint8_t buf[SYNC_BUF_SIZE];
} anc_assist_sync_info_t;

voice_assist_sync_role_t app_voice_assist_get_tws_ui_role(void)
{
    return bts_core_get_ui_role();
}

voice_assist_sync_role_t app_voice_assist_get_tws_bcr_role(void)
{
#if defined(VOICE_ASSIST_STEREO)
    return VOICE_ASSIST_TWS_ROLE_PRIMARY;
#else
    return bts_tws_if_get_tws_link_bt_role();
#endif
}

bool app_voice_assist_check_tws_connected(void)
{
#if defined(VOICE_ASSIST_STEREO)
    return true;
#else
    return bts_tws_if_is_tws_link_connected();
#endif
}

int32_t anc_assist_tws_sync_status_change(uint8_t *buf, uint32_t len)
{
    app_voice_assist_tws_sync_status_change(buf, len);

    return 0;
}

int32_t app_voice_assist_tws_sync_status_change(uint8_t *buf, uint32_t len)
{
    ASSERT(len == sizeof(anc_assist_sync_info_t), "[%s] len(%d) != sizeof(anc_assist_sync_info_t)(%d)", __func__, len, sizeof(anc_assist_sync_info_t));

    anc_assist_sync_info_t info = *((anc_assist_sync_info_t *)buf);
    app_anc_assist_get_user_sync_handler(info.user)(info.buf, info.len, 0);

    return 0;
}

int32_t app_voice_assist_tws_send_sync_info(uint32_t user, uint8_t *sync_res, uint32_t len)
{
    anc_assist_sync_info_t info;
    info.user = user;
    info.len = len;
    memcpy(info.buf, sync_res, len);

#if !defined(VOICE_ASSIST_STEREO)
    if (app_voice_assist_check_tws_connected()) {
        tws_ctrl_send_cmd(APP_TWS_CMD_SYNC_ANC_ASSIST_STATUS, (uint8_t *)&info, sizeof(anc_assist_sync_info_t));
    }
#endif
    return 0;
}

int32_t app_voice_assist_tws_sync_init()
{

    return 0;
}