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

#ifndef MODULE
#error MODULE required
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>
#include "debug_cfg.h"
// 7-bits 127
#define MODULE2ATTR(module) (module & 0x7f)
#define ATTR2MODULE(attr)   (attr & 0x7f)

#define LOGGER_LEVEL_TRACE  0
#define LOGGER_LEVEL_DEBUG  1
#define LOGGER_LEVEL_INFO   2
#define LOGGER_LEVEL_WARN   3
#define LOGGER_LEVEL_ERROR  4
#define LOGGER_LEVEL_FATAL  5
// 3-bits 7
#define LEVEL2ATTR(level)   ((level & 0x07) << 7)
#define ATTR2LEVEL(attr)    ((attr >> 7) & 0x07)

// 14-bits 16383
#define LINE2ATTR(line)     ((line & 0x3fff) << 10)
#define ATTR2LINE(attr)     ((attr >> 10) & 0x3fff)

#define FLUSH_BIT           (1 << 24)

// 7-bits 127
#define COUNT2ATTR(count)   ((count & 0x7f) << 25)
#define ATTR2COUNT(attr)    ((attr >> 25) & 0x7f)

#define MAKE_ATTR(level, count) (MODULE2ATTR(MODULE_ID) | LEVEL2ATTR(level) | LINE2ATTR(__LINE__) | COUNT2ATTR(count))



void bthost_set_test_level(uint8_t level);
void bthost_set_stack_level(uint8_t level);
void bthost_set_service_level(uint8_t level);
void bthost_set_middleware_level(uint8_t level);

void bthost_dump(uint32_t attr, uint8_t size, const void *data, size_t count);
void bthost_log(uint32_t attr, const char *format, ...) __attribute__((format(printf, 2, 3)));
void bthost_crash(uint32_t attr, const char *format, ...) __attribute__((format(printf, 2, 3))) __attribute__((noreturn));

void bthost_vlog(uint32_t attr, const char *format, va_list args);



#define RAW_PRINT_T(...)
#define RAW_PRINT_D(...)
#define RAW_PRINT_I(...)
#define RAW_PRINT_W(...)
#define RAW_PRINT_E(...)
#define RAW_PRINT_F(...)

#define DUMP8_T(...)
#define DUMP8_D(...)
#define DUMP8_I(...)
#define DUMP8_W(...)
#define DUMP8_E(...)
#define DUMP8_F(...)

#define DUMP16_T(...)
#define DUMP16_D(...)
#define DUMP16_I(...)
#define DUMP16_W(...)
#define DUMP16_E(...)
#define DUMP16_F(...)

#define DUMP32_T(...)
#define DUMP32_D(...)
#define DUMP32_I(...)
#define DUMP32_W(...)
#define DUMP32_E(...)
#define DUMP32_F(...)

#define PRINT_T(format, ...) RAW_PRINT_T(LOGGER_SECTION(format), ##__VA_ARGS__)
#define PRINT_D(format, ...) RAW_PRINT_D(LOGGER_SECTION(format), ##__VA_ARGS__)
#define PRINT_I(format, ...) RAW_PRINT_I(LOGGER_SECTION(format), ##__VA_ARGS__)
#define PRINT_W(format, ...) RAW_PRINT_W(LOGGER_SECTION(format), ##__VA_ARGS__)
#define PRINT_E(format, ...) RAW_PRINT_E(LOGGER_SECTION(format), ##__VA_ARGS__)
#define PRINT_F(format, ...) RAW_PRINT_F(LOGGER_SECTION(format), ##__VA_ARGS__)



#define MODULE_ID EXPAND_CONCAT(MODULE, _MODULE)
#define MODULE_LEVEL EXPAND_CONCAT(MODULE, _LEVEL)



#if (MODULE_LEVEL <= LOGGER_LEVEL_TRACE)
#undef RAW_PRINT_T
#undef DUMP8_T
#undef DUMP16_T
#undef DUMP32_T
#define RAW_PRINT_T(format, ...)     bthost_log(MAKE_ATTR(LOGGER_LEVEL_TRACE, COUNT_VA_ARGS(__VA_ARGS__)), format, ##__VA_ARGS__)
#define DUMP8_T(data, count)    bthost_dump(MAKE_ATTR(LOGGER_LEVEL_TRACE, 0), sizeof(uint8_t), data, count)
#define DUMP16_T(data, count)   bthost_dump(MAKE_ATTR(LOGGER_LEVEL_TRACE, 0), sizeof(uint16_t), data, count)
#define DUMP32_T(data, count)   bthost_dump(MAKE_ATTR(LOGGER_LEVEL_TRACE, 0), sizeof(uint32_t), data, count)
#endif

#if (MODULE_LEVEL <= LOGGER_LEVEL_DEBUG)
#undef RAW_PRINT_D
#undef DUMP8_D
#undef DUMP16_D
#undef DUMP32_D
#define RAW_PRINT_D(format, ...)     bthost_log(MAKE_ATTR(LOGGER_LEVEL_DEBUG, COUNT_VA_ARGS(__VA_ARGS__)), format, ##__VA_ARGS__)
#define DUMP8_D(data, count)    bthost_dump(MAKE_ATTR(LOGGER_LEVEL_DEBUG, 0), sizeof(uint8_t), data, count)
#define DUMP16_D(data, count)   bthost_dump(MAKE_ATTR(LOGGER_LEVEL_DEBUG, 0), sizeof(uint16_t), data, count)
#define DUMP32_D(data, count)   bthost_dump(MAKE_ATTR(LOGGER_LEVEL_DEBUG, 0), sizeof(uint32_t), data, count)
#endif

#if (MODULE_LEVEL <= LOGGER_LEVEL_INFO)
#undef RAW_PRINT_I
#undef DUMP8_I
#undef DUMP16_I
#undef DUMP32_I
#define RAW_PRINT_I(format, ...)      bthost_log(MAKE_ATTR(LOGGER_LEVEL_INFO, COUNT_VA_ARGS(__VA_ARGS__)), format, ##__VA_ARGS__)
#define DUMP8_I(data, count)     bthost_dump(MAKE_ATTR(LOGGER_LEVEL_INFO, 0), sizeof(uint8_t), data, count)
#define DUMP16_I(data, count)    bthost_dump(MAKE_ATTR(LOGGER_LEVEL_INFO, 0), sizeof(uint16_t), data, count)
#define DUMP32_I(data, count)    bthost_dump(MAKE_ATTR(LOGGER_LEVEL_INFO, 0), sizeof(uint32_t), data, count)
#endif

#if (MODULE_LEVEL <= LOGGER_LEVEL_WARN)
#undef RAW_PRINT_W
#undef DUMP8_W
#undef DUMP16_W
#undef DUMP32_W
#define RAW_PRINT_W(format, ...)      bthost_log(MAKE_ATTR(LOGGER_LEVEL_WARN, COUNT_VA_ARGS(__VA_ARGS__)), format, ##__VA_ARGS__)
#define DUMP8_W(data, count)     bthost_dump(MAKE_ATTR(LOGGER_LEVEL_WARN, 0), sizeof(uint8_t), data, count)
#define DUMP16_W(data, count)    bthost_dump(MAKE_ATTR(LOGGER_LEVEL_WARN, 0), sizeof(uint16_t), data, count)
#define DUMP32_W(data, count)    bthost_dump(MAKE_ATTR(LOGGER_LEVEL_WARN, 0), sizeof(uint32_t), data, count)
#endif

#if (MODULE_LEVEL <= LOGGER_LEVEL_ERROR)
#undef RAW_PRINT_E
#undef DUMP8_E
#undef DUMP16_E
#undef DUMP32_E
#define RAW_PRINT_E(format, ...)     bthost_log(MAKE_ATTR(LOGGER_LEVEL_ERROR, COUNT_VA_ARGS(__VA_ARGS__)), format, ##__VA_ARGS__)
#define DUMP8_E(data, count)    bthost_dump(MAKE_ATTR(LOGGER_LEVEL_ERROR, 0), sizeof(uint8_t), data, count)
#define DUMP16_E(data, count)   bthost_dump(MAKE_ATTR(LOGGER_LEVEL_ERROR, 0), sizeof(uint16_t), data, count)
#define DUMP32_E(data, count)   bthost_dump(MAKE_ATTR(LOGGER_LEVEL_ERROR, 0), sizeof(uint32_t), data, count)
#endif

#if (MODULE_LEVEL <= LOGGER_LEVEL_FATAL)
#undef RAW_PRINT_F
#undef DUMP8_F
#undef DUMP16_F
#undef DUMP32_F
#define RAW_PRINT_F(format, ...)     bthost_log(MAKE_ATTR(LOGGER_LEVEL_FATAL, COUNT_VA_ARGS(__VA_ARGS__)) | FLUSH_BIT, format, ##__VA_ARGS__)
#define DUMP8_F(data, count)    bthost_dump(MAKE_ATTR(LOGGER_LEVEL_FATAL, 0) | FLUSH_BIT, sizeof(uint8_t), data, count)
#define DUMP16_F(data, count)   bthost_dump(MAKE_ATTR(LOGGER_LEVEL_FATAL, 0) | FLUSH_BIT, sizeof(uint16_t), data, count)
#define DUMP32_F(data, count)   bthost_dump(MAKE_ATTR(LOGGER_LEVEL_FATAL, 0) | FLUSH_BIT, sizeof(uint32_t), data, count)
#endif

#define RAW_RUNTIME_ASSERT(cond, format, ...) if (!(cond)) { bthost_crash(MAKE_ATTR(LOGGER_LEVEL_FATAL, COUNT_VA_ARGS(__VA_ARGS__)) | FLUSH_BIT, format, ##__VA_ARGS__); }
#define RUNTIME_ASSERT(cond, format, ...) RAW_RUNTIME_ASSERT(cond, LOGGER_SECTION(format), ##__VA_ARGS__)


/*
 * ADAPTER
 * Compatible with the old API
 */

#define DEBUG_VERBOSE(unused, format, ...)  PRINT_D(format, ##__VA_ARGS__)
#define DEBUG_INFO(unused, format, ...)     PRINT_I(format, ##__VA_ARGS__)
#define DEBUG_WARNING(unused, format, ...)  PRINT_W(format, ##__VA_ARGS__)
#define DEBUG_ERROR(unused, format, ...)    PRINT_E(format, ##__VA_ARGS__)
#define DEBUG_IMM(unused, format, ...)      PRINT_F(format, ##__VA_ARGS__)
#define DEBUG_AUTO_LOG(unused, format, ...) PRINT_I(format, ##__VA_ARGS__)

#define _COUNT_VA_ARGS_(\
     _0, _1,  _2,  _3,  _4,  _5,  _6,  _7,  \
     _8, _9,  _10, _11, _12, _13, _14, _15, \
    _16, _17, _18, _19, _20, _21, _22, _23, \
    _24, _25, _26, _27, _28, _29, _30, _31, \
    _32, _33, _34, _35, _36, _37, _38, _39, \
    _40, _41, _42, _43, _44, _45, _46, _47, \
    _48, _49, _50, _51, _52, _53, _54, _55, \
    _56, _57, _58, _59, _60, _61, _62, _63, \
    N, ...) N

#define COUNT_VA_ARGS(...) _COUNT_VA_ARGS_( \
    0, ##__VA_ARGS__,\
    63, 62, 61, 60, 59, 58, 57, 56, \
    55, 54, 53, 52, 51, 50, 49, 48, \
    47, 46, 45, 44, 43, 42, 41, 40, \
    39, 38, 37, 36, 35, 34, 33, 32, \
    31, 30, 29, 28, 27, 26, 25, 24, \
    23, 22, 21, 20, 19, 18, 17, 16, \
    15, 14, 13, 12, 11, 10,  9,  8, \
     7,  6,  5,  4,  3,  2,  1,  0)

#define _UINT32_CAST_0()
#define _UINT32_CAST_1(a) (uint32_t)(a)
#define _UINT32_CAST_2(a, b) (uint32_t)(a), (uint32_t)(b)
#define _UINT32_CAST_3(a, b, c) (uint32_t)(a), (uint32_t)(b), (uint32_t)(c)
#define _UINT32_CAST_4(a, b, c, d) (uint32_t)(a), (uint32_t)(b), (uint32_t)(c), (uint32_t)(d)
#define UINT32_CAST(...) EXPAND_CONCAT(_UINT32_CAST_, COUNT_VA_ARGS(__VA_ARGS__))(__VA_ARGS__)

extern const char *bthost_module_strs[];
extern const char bthost_strict_format_0[];
extern const char bthost_strict_format_1[];
extern const char bthost_strict_format_2[];
extern const char bthost_strict_format_3[];
extern const char bthost_strict_format_4[];
extern const char bthost_tag_placeholder[];

#define STRICT_TRACE(status, tag, ...) RAW_PRINT_T(EXPAND_CONCAT(bthost_strict_format_, COUNT_VA_ARGS(__VA_ARGS__)), bthost_module_strs[MODULE_ID], status, __LINE__, tag ? tag : bthost_tag_placeholder __VA_OPT__(,) UINT32_CAST(__VA_ARGS__))
#define STRICT_DEBUG(status, tag, ...) RAW_PRINT_D(EXPAND_CONCAT(bthost_strict_format_, COUNT_VA_ARGS(__VA_ARGS__)), bthost_module_strs[MODULE_ID], status, __LINE__, tag ? tag : bthost_tag_placeholder __VA_OPT__(,) UINT32_CAST(__VA_ARGS__))
#define STRICT_INFO(status, tag, ...)  RAW_PRINT_I(EXPAND_CONCAT(bthost_strict_format_, COUNT_VA_ARGS(__VA_ARGS__)), bthost_module_strs[MODULE_ID], status, __LINE__, tag ? tag : bthost_tag_placeholder __VA_OPT__(,) UINT32_CAST(__VA_ARGS__))
#define STRICT_WARN(status, tag, ...)  RAW_PRINT_W(EXPAND_CONCAT(bthost_strict_format_, COUNT_VA_ARGS(__VA_ARGS__)), bthost_module_strs[MODULE_ID], status, __LINE__, tag ? tag : bthost_tag_placeholder __VA_OPT__(,) UINT32_CAST(__VA_ARGS__))
#define STRICT_ERROR(status, tag, ...) RAW_PRINT_E(EXPAND_CONCAT(bthost_strict_format_, COUNT_VA_ARGS(__VA_ARGS__)), bthost_module_strs[MODULE_ID], status, __LINE__, tag ? tag : bthost_tag_placeholder __VA_OPT__(,) UINT32_CAST(__VA_ARGS__))
#define STRICT_FATAL(status, tag, ...) RAW_PRINT_F(EXPAND_CONCAT(bthost_strict_format_, COUNT_VA_ARGS(__VA_ARGS__)), bthost_module_strs[MODULE_ID], status, __LINE__, tag ? tag : bthost_tag_placeholder __VA_OPT__(,) UINT32_CAST(__VA_ARGS__))

#define CO_LR_ADDRESS (uint32_t)__builtin_return_address(0)
#define CO_ASSERT(cond, ...) RAW_RUNTIME_ASSERT(cond, EXPAND_CONCAT(bthost_strict_format_, COUNT_VA_ARGS(__VA_ARGS__)), bthost_module_strs[MODULE_ID], 0, __LINE__, bthost_tag_placeholder, CO_LR_ADDRESS __VA_OPT__(,) UINT32_CAST(__VA_ARGS__))
#define CO_ASSERT_0 CO_ASSERT
#define CO_ASSERT_1 CO_ASSERT
#define CO_ASSERT_2 CO_ASSERT
#define CO_ASSERT_3 CO_ASSERT



#define CO_LOG_MAIN(status, ...) STRICT_TRACE(status, NULL, ##__VA_ARGS__)
#define CO_LOG_MAIN_0 CO_LOG_MAIN
#define CO_LOG_MAIN_1 CO_LOG_MAIN
#define CO_LOG_MAIN_2 CO_LOG_MAIN
#define CO_LOG_MAIN_3 CO_LOG_MAIN
#define CO_LOG_MAIN_4 CO_LOG_MAIN
#define CO_LOG_MAIN_S(status, tag, ...) STRICT_TRACE(status, MAKE_STR(tag), ##__VA_ARGS__)
#define CO_LOG_MAIN_S_0 CO_LOG_MAIN_S
#define CO_LOG_MAIN_S_1 CO_LOG_MAIN_S
#define CO_LOG_MAIN_S_2 CO_LOG_MAIN_S
#define CO_LOG_MAIN_S_3 CO_LOG_MAIN_S
#define CO_LOG_MAIN_WITH_STR(status, str, ...) STRICT_TRACE(status, str, ##__VA_ARGS__)
#define CO_LOG_MAIN_WITH_STR_0 CO_LOG_MAIN_WITH_STR
#define CO_LOG_MAIN_WITH_STR_1 CO_LOG_MAIN_WITH_STR
#define CO_LOG_MAIN_WITH_STR_2 CO_LOG_MAIN_WITH_STR
#define CO_LOG_MAIN_WITH_STR_3 CO_LOG_MAIN_WITH_STR

#define CO_LOG_VBS(status, ...) STRICT_DEBUG(status, NULL, ##__VA_ARGS__)
#define CO_LOG_VBS_0 CO_LOG_VBS
#define CO_LOG_VBS_1 CO_LOG_VBS
#define CO_LOG_VBS_2 CO_LOG_VBS
#define CO_LOG_VBS_3 CO_LOG_VBS
#define CO_LOG_VBS_4 CO_LOG_VBS
#define CO_LOG_VBS_S(status, tag, ...) STRICT_DEBUG(status, MAKE_STR(tag), ##__VA_ARGS__)
#define CO_LOG_VBS_S_0 CO_LOG_VBS_S
#define CO_LOG_VBS_S_1 CO_LOG_VBS_S
#define CO_LOG_VBS_S_2 CO_LOG_VBS_S
#define CO_LOG_VBS_S_3 CO_LOG_VBS_S

#define CO_LOG_INFO(status, ...) STRICT_INFO(status, NULL, ##__VA_ARGS__)
#define CO_LOG_INFO_0 CO_LOG_INFO
#define CO_LOG_INFO_1 CO_LOG_INFO
#define CO_LOG_INFO_2 CO_LOG_INFO
#define CO_LOG_INFO_3 CO_LOG_INFO
#define CO_LOG_INFO_4 CO_LOG_INFO
#define CO_LOG_INFO_S(status, tag, ...) STRICT_INFO(status, MAKE_STR(tag), ##__VA_ARGS__)
#define CO_LOG_INFO_S_0 CO_LOG_INFO_S
#define CO_LOG_INFO_S_1 CO_LOG_INFO_S
#define CO_LOG_INFO_S_2 CO_LOG_INFO_S
#define CO_LOG_INFO_S_3 CO_LOG_INFO_S
#define CO_LOG_INFO_WITH_STR(status, str, ...) STRICT_INFO(status, str, ##__VA_ARGS__)
#define CO_LOG_INFO_WITH_STR_0 CO_LOG_INFO_WITH_STR
#define CO_LOG_INFO_WITH_STR_1 CO_LOG_INFO_WITH_STR
#define CO_LOG_INFO_WITH_STR_2 CO_LOG_INFO_WITH_STR
#define CO_LOG_INFO_WITH_STR_3 CO_LOG_INFO_WITH_STR

#define CO_LOG_WAR(status, ...) STRICT_WARN(status, NULL, ##__VA_ARGS__)
#define CO_LOG_WAR_0 CO_LOG_WAR
#define CO_LOG_WAR_1 CO_LOG_WAR
#define CO_LOG_WAR_2 CO_LOG_WAR
#define CO_LOG_WAR_3 CO_LOG_WAR
#define CO_LOG_WAR_4 CO_LOG_WAR
#define CO_LOG_WAR_S(status, tag, ...) STRICT_WARN(status, MAKE_STR(tag), ##__VA_ARGS__)
#define CO_LOG_WAR_S_0 CO_LOG_WAR_S
#define CO_LOG_WAR_S_1 CO_LOG_WAR_S
#define CO_LOG_WAR_S_2 CO_LOG_WAR_S
#define CO_LOG_WAR_S_3 CO_LOG_WAR_S

#define CO_LOG_ERR(status, ...) STRICT_ERROR(status, NULL, ##__VA_ARGS__)
#define CO_LOG_ERR_0 CO_LOG_ERR
#define CO_LOG_ERR_1 CO_LOG_ERR
#define CO_LOG_ERR_2 CO_LOG_ERR
#define CO_LOG_ERR_3 CO_LOG_ERR
#define CO_LOG_ERR_4 CO_LOG_ERR
#define CO_LOG_ERR_S(status, tag, ...) STRICT_ERROR(status, MAKE_STR(tag), ##__VA_ARGS__)
#define CO_LOG_ERR_S_0 CO_LOG_ERR_S
#define CO_LOG_ERR_S_1 CO_LOG_ERR_S
#define CO_LOG_ERR_S_2 CO_LOG_ERR_S
#define CO_LOG_ERR_S_3 CO_LOG_ERR_S



#define LOG_V(format, ...) PRINT_T(format, ##__VA_ARGS__)
#define LOG_D(format, ...) PRINT_D(format, ##__VA_ARGS__)
#define LOG_I(format, ...) PRINT_I(format, ##__VA_ARGS__)
#define LOG_W(format, ...) PRINT_W(format, ##__VA_ARGS__)
#define LOG_E(format, ...) PRINT_E(format, ##__VA_ARGS__)
#define LOG_IMM(format, ...) PRINT_F(format, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif
