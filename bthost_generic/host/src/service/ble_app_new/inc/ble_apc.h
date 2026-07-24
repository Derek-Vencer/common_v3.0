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
#ifndef __APP_BLE_APC_H__
#define __APP_BLE_APC_H__

enum apcs_character_type
{
    APCS_CHAR_TYPE_MIN = 0,
    // Echo cancellation functionality of the part of the server device to which the Control applies.
    APCS_CHAR_TYPE_EC_SWITCH = APCS_CHAR_TYPE_MIN,
    // Noise Reduction functionality of the part of the server device to which the Control applies
    APCS_CHAR_TYPE_NR_SWITCH,
    // Noise Reduction functionality of the part of the server device to which the Control applies
    APCS_CHAR_TYPE_NR_SETTING,
    // Binauralization functionality of the part of the server device to which the Control applies
    APCS_CHAR_TYPE_BR_SWITCH,
    // Scene Steering functionality of the part of the server device to which the Control applies
    APCS_CHAR_TYPE_SS_SWITCH,

    APCS_CHAR_TYPE_MAX,
};

#endif /// __APP_BLE_APC_H__