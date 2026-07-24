/****************************************************************************
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

#ifndef __BTS_COMMON_TYPES_H__
#define __BTS_COMMON_TYPES_H__

#include "bt_base_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 ****************************************************************************************
 *     ____                                        _____
 *    / ___|___  _ __ ___  _ __ ___   ___  _ __   |_   _|   _ _ __   ___  ___
 *   | |   / _ \| '_ ` _ \| '_ ` _ \ / _ \| '_ \    | || | | | '_ \ / _ \/ __|
 *   | |__| (_) | | | | | | | | | | | (_) | | | |   | || |_| | |_) |  __/\__ \
 *    \____\___/|_| |_| |_|_| |_| |_|\___/|_| |_|   |_| \__, | .__/ \___||___/
 *                                                      |___/|_|
 *
 ****************************************************************************************
 */

typedef int bts_status_t;
#define BTS_ST_SUCCESS          (0x00)
#define BTS_ST_FAILED           (0x01)
#define BTS_ST_PENDING          (0x02)
#define BTS_ST_BUSY             (0x03)
#define BTS_ST_TIMEOUT          (0x04)
#define BTS_ST_INVALID_PARAMS   (0x05)
#define BTS_ST_NO_RESOURCES     (0x06)


#define BTS_UINT32_TO_BE_STREAM(p, u32)     \
    {                                       \
        *(p)++ = (uint8_t) ((u32) >> 24);   \
        *(p)++ = (uint8_t) ((u32) >> 16);   \
        *(p)++ = (uint8_t) ((u32) >> 8);    \
        *(p)++ = (uint8_t) (u32);           \
    }
#define BTS_UINT24_TO_BE_STREAM(p, u24)     \
    {                                       \
        *(p)++ = (uint8_t) ((u24) >> 16);   \
        *(p)++ = (uint8_t) ((u24) >> 8);    \
        *(p)++ = (uint8_t) (u24);           \
    }
#define BTS_UINT16_TO_BE_STREAM(p, u16)     \
    {                                       \
        *(p)++ = (uint8_t) ((u16) >> 8);    \
        *(p)++ = (uint8_t) (u16);           \
    }
#define BTS_UINT8_TO_BE_STREAM(p, u8)       \
    {                                       \
        *(p)++ = (uint8_t) (u8);            \
    }
#define BTS_BE_STREAM_TO_UINT8(u8, p)       \
    {                                       \
        (u8) = (uint8_t) (*(p));            \
        (p) += 1;                           \
    }
#define BTS_BE_STREAM_TO_UINT16(u16, p)                                             \
    {                                                                               \
        (u16) = (uint16_t) (((uint16_t) (*(p)) << 8) + (uint16_t) (*((p) + 1)));    \
        (p) += 2;                                                                   \
    }
#define BTS_BE_STREAM_TO_UINT24(u24, p)                                             \
    {                                                                               \
        (u24) = (((uint32_t) (*((p) + 2))) + ((uint32_t) (*((p) + 1)) << 8) +       \
                 ((uint32_t) (*(p)) << 16));                                        \
        (p) += 3;                                                                   \
    }
#define BTS_BE_STREAM_TO_UINT32(u32, p)                                             \
    {                                                                               \
        (u32) = ((uint32_t) (*((p) + 3)) + ((uint32_t) (*((p) + 2)) << 8) +         \
                 ((uint32_t) (*((p) + 1)) << 16) + ((uint32_t) (*(p)) << 24));      \
        (p) += 4;                                                                   \
    }
#define BTS_BE_STREAM_TO_ARRAY(p, a, len)                                           \
    {                                                                               \
        int ijk;                                                                    \
        for (ijk = 0; ijk < (len); ijk++) ((uint8_t*)(a))[ijk] = *(p)++;            \
    }
#define BTS_ARRAY_TO_BE_STREAM(p, a, len)                                           \
    {                                                                               \
        int ijk;                                                                    \
        for (ijk = 0; ijk < (len); ijk++) *(p)++ = (uint8_t)(a)[ijk];               \
    }


#ifdef __cplusplus
}
#endif

#endif /* __BTS_COMMON_TYPES_H__ */