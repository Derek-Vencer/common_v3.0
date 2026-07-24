/***************************************************************************
 *
 * Copyright (c) 2015-2023 BES Technic
 *
 * Authored by BES CD team (Blueelf Prj).
 *
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

#ifndef __CO_MATH_H__
#define __CO_MATH_H__

#include <stdint.h>        // standard integer definitions
#include <stdbool.h>       // boolean definitions
#include <stdlib.h>        // standard library

#define CO_BIT(pos) (1UL<<(pos))

#define CO_BIT_GET(bf, pos) (((((uint8_t*)bf)[((pos) >> 3)])>>((pos) & 0x7)) & 0x1)

#define CO_BIT_SET(bf, pos, val) (((uint8_t*)bf)[((pos) >> 3)]) = ((((uint8_t*)bf)[((pos) >> 3)]) & ~CO_BIT(((pos) & 0x7))) \
                                                                | (((val) & 0x1) << ((pos) & 0x7))

#define CO_ALIGN_HI(val, size) ((((uint32_t) val)+((size)- 1))&~((size)- 1))

#define CO_ALIGN4_HI(val) (((val)+3)&~3)

#define CO_ALIGN4_LO(val) ((val)&~3)

#define CO_ALIGN2_HI(val) (((val)+1)&~1)

#define CO_ALIGN2_LO(val) ((val)&~1)

#define CO_DIVIDE_CEIL(val, div) (((val) + ((div) - 1))/ (div))

#define CO_DIVIDE_ROUND(val, div) (((val) + ((div) >> 1))/ (div))

/// macro to extract a field from a value containing several fields
/// @param[in] __r bit field value
/// @param[in] __f field name
/// @return the value of the register masked and shifted
#define GETF(__r, __f)                                                           \
    (( (__r) & (__f##_MASK) ) >> (__f##_LSB))

/// macro to set a field value into a value  containing several fields.
/// @param[in] __r bit field value
/// @param[in] __f field name
/// @param[in] __v value to put in field
#define SETF(__r, __f, __v)                                                      \
    do {                                                                         \
        __r = (((__r) & ~(__f##_MASK)) | (__v) << (__f##_LSB));                  \
    } while (0)

/// macro to extract a bit field from a value containing several fields
/// @param[in] __r bit field value
/// @param[in] __b bit field name
/// @return the value of the register masked and shifted
#define GETB(__r, __b)                                                           \
    (( (__r) & (__b##_BIT) ) >> (__b##_POS))

/// macro to set a bit field value into a value containing several fields.
/// @param[in] __r bit field value
/// @param[in] __b bit field name
/// @param[in] __v value to put in field
#define SETB(__r, __b, __v)                                                      \
    do {                                                                         \
        __r = (((__r) & ~(__b##_BIT)) | (__v ? 1 : 0) << (__b##_POS));                  \
    } while (0)

/// macro to toggle a bit into a value containing several bits.
/// @param[in] __r bit field value
/// @param[in] __b bit field name
#define TOGB(__r, __b)                                                           \
    do {                                                                         \
        __r = ((__r) ^ (__b##_BIT));                                             \
    } while (0)

/// Get the number of elements within an array, give also number of rows in a 2-D array
#define ARRAY_LEN(array)   (sizeof((array))/sizeof((array)[0]))

/// Get the number of columns within a 2-D array
#define ARRAY_NB_COLUMNS(array)  (sizeof((array[0]))/sizeof((array)[0][0]))

/// count number of bit into a long field
#define CO_BIT_CNT(val) (co_bit_cnt((uint8_t*) &(val), sizeof(val)))

/// Increment value and make sure it's never greater or equals max (else wrap to 0)
#define CO_VAL_INC(_val, _max)      \
    (_val) = (_val) + 1;            \
    if((_val) >= (_max)) (_val) = 0

/// Add value and make sure it's never greater or equals max (else wrap)
/// _add must be less that _max
#define CO_VAL_ADD(_val, _add, _max)      \
    (_val) = (_val) + (_add);             \
    if((_val) >= (_max)) (_val) -= (_max)

/// sub value and make sure it's never greater or equals max (else wrap)
/// _sub must be less that _max
#define CO_VAL_SUB(_val, _sub, _max)      \
    if((_val) < (_sub)) (_val) += _max;   \
    (_val) = (_val) - (_sub)

static inline uint32_t co_mod(uint32_t val, uint32_t div)
{
    return ((val) % (div));
}

#define CO_MOD(val, div) co_mod(val, div)

inline uint32_t co_clz(uint32_t val)
{
    if (val == 0)
    {
        return 32;
    }
    return __builtin_clz(val);
}

inline uint32_t co_ctz(uint32_t val)
{
    if (val == 0)
    {
        return 32;
    }
    return __builtin_ctz(val);
}

inline uint32_t co_min(uint32_t a, uint32_t b)
{
    return a < b ? a : b;
}

inline int32_t co_min_s(int32_t a, int32_t b)
{
    return a < b ? a : b;
}

inline uint32_t co_max(uint32_t a, uint32_t b)
{
    return a > b ? a : b;
}

inline int co_abs(int val)
{
    return (val < 0) ? (0 - val) : val;
}

static inline uint8_t co_bit_cnt(const uint8_t *p_val, uint8_t size)
{
    uint8_t nb_bit = 0;

    uint8_t fast_bit_cnt_v[] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};

    while (size-- > 0)
    {
        nb_bit += (fast_bit_cnt_v[(*p_val) & 0x0F] + fast_bit_cnt_v[(*p_val) >> 4]);

        p_val++;
    }
    return (nb_bit);
}

#endif // _CO_MATH_INC_
