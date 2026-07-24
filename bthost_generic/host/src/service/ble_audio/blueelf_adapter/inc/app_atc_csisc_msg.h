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
/**
 ****************************************************************************************
 * @addtogroup APP_ACC

 * @{
 ****************************************************************************************
 */

#ifndef __APP_ATC_CSISC_MSG_H__
#define __APP_ATC_CSISC_MSG_H__
#if BLE_AUDIO_ENABLED
#include "bluetooth.h"

#define APP_ATC_CSIS_SETS_SIZE_MAX       (BLE_AUDIO_CONNECTION_CNT)    //APP_ATC_CSIS_SETS_SIZE_MAX need >= BLE_AUDIO_CONNECTION_CNT
#define APP_ATC_CSIS_SETS_NUM_MAX        (1)
#define APP_ATC_CSIS_SIRK_VAL_LEN        (16)

/// SIRK
typedef struct csisc_sirk
{
    /// SIRK
    uint8_t sirk[APP_ATC_CSIS_SIRK_VAL_LEN];
} csisc_sirk_t;

typedef struct
{
    uint8_t         grp_device_numbers;
    uint8_t         rank_index;
    uint8_t         lock_status;
    uint8_t         key_lid;
    csisc_sirk_t     sirk;
} APP_ATC_CSISC_SETS_INFO_T;

typedef struct
{
    //when discover and bond server, inuse will be set to true, otherwise set to false.
    bool                         inuse;
    //when csisc env struct init flow completes successfully, discover_cmp_done will be set to true, otherwise set to false.
    bool                         discover_cmp_done;
    uint8_t                      con_lid;
    uint8_t                      active_sets_index;
    APP_ATC_CSISC_SETS_INFO_T    sets_info[APP_ATC_CSIS_SETS_NUM_MAX];
} APP_ATC_CSISC_DEV_INFO_T;

typedef struct
{
    APP_ATC_CSISC_DEV_INFO_T     device_info[APP_ATC_CSIS_SETS_SIZE_MAX];
} APP_ATC_CSISC_ENV_T;

typedef struct
{
    // csisc con_lid
    int csip_conlid;

    // csisc set_lid
    int csip_setlid;

    // csisc lock states
    int csip_lock;
} ATC_CSISC_LOCK_T;

#ifdef AOB_MOBILE_ENABLED

int app_atc_csisc_start(uint8_t con_lid);

#endif
#endif
#endif // APP_ATC_CSISC_MSG_H_

/// @} APP_ATC_CSISC_MSG_H_
