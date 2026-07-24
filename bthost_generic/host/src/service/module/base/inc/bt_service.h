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
#ifndef __BT_SERVICE_H__
#define __BT_SERVICE_H__

/*****************************header include********************************/
#include <stdint.h>

/*********************external declaration*************************/

/**********************private function declaration*************************/

/************************private macro defination***************************/
typedef enum
{
    // No error
    BT_SVC_ERROR_NO = 0,
    // Unknown reason error
    BT_SVC_ERROR_UNKNOW,
    // State error
    BT_SVC_ERROR_STATE,
    // Parameter invalid
    BT_SVC_ERROR_PARAM_INVALID,
    // Operation prohibition
    BT_SVC_ERROR_OPER_PRO,
    // Insufficient resources
    BT_SVC_ERROR_INS_RESOURCES,
    // Insufficient resources
    BT_SVC_ERROR_UNSUPPORT,
    // Role error
    BT_SVC_ERROR_ROLE,
    // Busy error
    BT_SVC_ERROR_BUSY,
    // Busy error
    BT_SVC_ERROR_ALREADY,
} BT_SVC_ERROR_CODE_E;

typedef enum
{
    BT_SVC_AUD_PATH_UNKONW = 0,
    // Linein audio path
    BT_SVC_AUD_PATH_LINEIN,
    // I2S audio path
    BT_SVC_AUD_PATH_I2S,
    // Analog MIC audio path
    BT_SVC_AUD_PATH_AMIC,
    // Digital MIC audio path
    BT_SVC_AUD_PATH_DMIC,
    // USB audio path
    BT_SVC_AUD_PATH_USB,
    // WIFI audio path
    BT_SVC_AUD_PATH_WIFI,
    // A2DP audio path
    BT_SVC_AUD_PATH_A2DP,
    // SCO audio path
    BT_SVC_AUD_PATH_SCO,
    // LEA CIS audio path
    BT_SVC_AUD_PATH_LEA_UC,
    // LEA BIS audio path
    BT_SVC_AUD_PATH_LEA_BC,
    // SMF audio path
    BT_SVC_AUD_PATH_SMF,
    // SPDIF audio path
    BT_SVC_AUD_PATH_SPDIF,
    // MAX audio path
    BT_SVC_AUD_PATH_MAX,
} BT_SVC_DATA_PATH_TYPE_E;

/************************private variable defination************************/

void bt_service_init(uint8_t app_type);

void bt_service_deinit(void);

#endif
