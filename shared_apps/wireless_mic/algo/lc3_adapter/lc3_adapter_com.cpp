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
#ifdef LC3_CODEC_ON
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lc3_adapter.h"

const AUDIO_LC3_ENCODED_MAP_T lc3_encode_map[] = {
    {0, 0},
    {AUDIO_LC3_CODEC_BITRATE_96K,   AUDIO_LC3_FRAME_SIZE(AUDIO_LC3_CODEC_BITRATE_96K,   AUDIO_LC3_CODEC_FRAME_US)},
    {AUDIO_LC3_CODEC_BITRATE_128K,  AUDIO_LC3_FRAME_SIZE(AUDIO_LC3_CODEC_BITRATE_128K,  AUDIO_LC3_CODEC_FRAME_US)},
};

#endif