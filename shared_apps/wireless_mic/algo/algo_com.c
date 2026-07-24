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
#include <stdio.h>

uint32_t audio_copy_32bit_to_24bit(uint8_t* dest, uint8_t* src, uint32_t src_bytes) {
    uint32_t i = src_bytes / 4;
    uint32_t* s32 = (uint32_t*)src;
    uint8_t* d = dest;

    for (; i >= 4; i -= 4) {
        uint32_t sample1 = *s32++;
        uint32_t sample2 = *s32++;
        uint32_t sample3 = *s32++;
        uint32_t sample4 = *s32++;

        *d++ = (uint8_t)(sample1 & 0xFF);
        *d++ = (uint8_t)((sample1 >> 8) & 0xFF);
        *d++ = (uint8_t)((sample1 >> 16) & 0xFF);

        *d++ = (uint8_t)(sample2 & 0xFF);
        *d++ = (uint8_t)((sample2 >> 8) & 0xFF);
        *d++ = (uint8_t)((sample2 >> 16) & 0xFF);

        *d++ = (uint8_t)(sample3 & 0xFF);
        *d++ = (uint8_t)((sample3 >> 8) & 0xFF);
        *d++ = (uint8_t)((sample3 >> 16) & 0xFF);

        *d++ = (uint8_t)(sample4 & 0xFF);
        *d++ = (uint8_t)((sample4 >> 8) & 0xFF);
        *d++ = (uint8_t)((sample4 >> 16) & 0xFF);
    }

    for (; i > 0; i--) {
        uint32_t sample = *s32++;
        *d++ = (uint8_t)(sample & 0xFF);
        *d++ = (uint8_t)((sample >> 8) & 0xFF);
        *d++ = (uint8_t)((sample >> 16) & 0xFF);
    }

    return (src_bytes * 3 / 4);
}