/***************************************************************************
 *
 * Copyright 2015-2022 BES.
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

#if defined(UTILS_ESHELL_EN) && (defined(PSRAM_ENABLE) || defined(DDR_ENABLE))
#include "cmsis.h"
#include "plat_types.h"
#include "plat_addr_map.h"
#include "hal_location.h"

#if defined(DDR_ENABLE)
#define FB_LOC      DDR_BSS_LOC
#elif defined(PSRAM_ENABLE)
#define FB_LOC      PSRAM_NC_BSS_LOC
#else
#define FB_LOC
#endif

#ifndef FB_BUFFER_HEIGHT
#define FB_BUFFER_HEIGHT                    512
#endif

#ifndef FB_BUFFER_WIDTH
#define FB_BUFFER_WIDTH                     512
#endif

#ifndef FB_BUFFER_BPP
#define FB_BUFFER_BPP                       4
#endif

FB_LOC ALIGNED(64) uint8_t fb_big_buffer0[FB_BUFFER_HEIGHT*FB_BUFFER_WIDTH*FB_BUFFER_BPP];
FB_LOC ALIGNED(64) uint8_t fb_big_buffer1[FB_BUFFER_HEIGHT*FB_BUFFER_WIDTH*FB_BUFFER_BPP];
void *frame_test_buf0 = fb_big_buffer0;
void *frame_test_buf1 = fb_big_buffer1;

#endif
