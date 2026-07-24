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

#pragma once
#include "bt_base_types.h"

typedef enum
{
    BT_AUDIO_UNKNOWN = 0,
    /* Usage value to use when the usage is media, such as music.*/
    BT_AUDIO_MEDIA,
    /* Usage value to use when the usage is voice communications, such as AI VOICE */
    BT_AUDIO_AI_VOICE,
    BT_AUDIO_CALL,
    BT_AUDIO_RINGTONE,
    BT_AUDIO_FLEXIBLE,
    BT_AUDIO_BIS,
    BT_AUDIO_MAX,
} bt_audio_focus_type_t;
