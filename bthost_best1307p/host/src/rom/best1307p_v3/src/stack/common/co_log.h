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
 * @brief xxx.
 *
 ****************************************************************************/
#ifndef CO_LOG_H_
#define CO_LOG_H_

/****************************** header include ********************************/
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

/***************************** external declaration *****************************/

/***************************** macro defination *******************************/

/***************************** type defination ********************************/

/***************************** variable defination *****************************/

/***************************** function declaration ****************************/
#ifdef MOUDLE
#define CO_LOG_MODULE BT_DBG_CONCAT(MOUDLE, _MODULE)
#else
#define CO_LOG_MODULE NULL_MODULE
#endif

typedef union {
    uint32_t data;
    struct {
    uint32_t module_id  : 6;    // max 1 ~ 63
    uint32_t line       : 14;   // max 16383
    uint32_t status     : 9;    // status code
    uint32_t log_level  : 3;    // max 7
    } s;
} bt_log_mod_t;

#define CO_GEN_LOG_MODULE(log_level, status) ((uint32_t)(\
        (((CO_LOG_MODULE) & 0x3F) << 0) | \
        (((__LINE__) & 0x3FFF) << 6) | \
        (((status) & 0x1FF) << 20) | \
        (((log_level) & 0x7) << 29) \
    ))

typedef struct {
    char str[4];
} bt_log_tag_t;

#ifdef BT_LOG_SIMPLIFY
void co_func_log_trace_simplify(uint32_t mod);
#define co_func_log_trace_0(mod) co_func_log_trace_simplify(mod)
#define co_func_log_trace_1(mod, a) co_func_log_trace_simplify(mod)
#define co_func_log_trace_2(mod, a, b) co_func_log_trace_simplify(mod)
#define co_func_log_trace_3(mod, a, b, c) co_func_log_trace_simplify(mod)
#define co_func_log_trace_4(mod, a, b, c, d) co_func_log_trace_simplify(mod)
#else
void co_func_log_trace_0(uint32_t mod);
void co_func_log_trace_1(uint32_t mod, uintptr_t a);
void co_func_log_trace_2(uint32_t mod, uintptr_t a, uintptr_t b);
void co_func_log_trace_3(uint32_t mod, uintptr_t a, uintptr_t b, uintptr_t c);
void co_func_log_trace_4(uint32_t mod, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d);
#endif
void co_func_log_str_0(uint32_t mod, const char *s);
void co_func_log_str_1(uint32_t mod, const char *s, uintptr_t a);
void co_func_log_str_2(uint32_t mod, const char *s, uintptr_t a, uintptr_t b);
void co_func_log_str_3(uint32_t mod, const char *s, uintptr_t a, uintptr_t b, uintptr_t c);
void co_func_log_trace_with_tag_0(uint32_t mod, bt_log_tag_t tag);
void co_func_log_trace_with_tag_1(uint32_t mod, bt_log_tag_t tag, uintptr_t a);
void co_func_log_trace_with_tag_2(uint32_t mod, bt_log_tag_t tag, uintptr_t a, uintptr_t b);
void co_func_log_trace_with_tag_3(uint32_t mod, bt_log_tag_t tag, uintptr_t a, uintptr_t b, uintptr_t c);
const char *co_log_module_str(void);

#define CO_LR_ADDRESS (uintptr_t)__builtin_return_address(0)

#define CO_ASSERT_0(e) { if (!(e)) { \
        co_func_log_trace_1(CO_GEN_LOG_MODULE(DBG_ASSERT_LEVEL, 0), CO_LR_ADDRESS); \
        Plt_Assert(0, co_log_module_str(), CO_LOG_MODULE, __LINE__, CO_LR_ADDRESS); \
    }}

#define CO_ASSERT_1(e, a) { if (!(e)) { \
        co_func_log_trace_2(CO_GEN_LOG_MODULE(DBG_ASSERT_LEVEL, 0), (uintptr_t)(a), CO_LR_ADDRESS); \
        Plt_Assert(0, co_log_module_str(), CO_LOG_MODULE, __LINE__, CO_LR_ADDRESS); \
    }}

#define CO_ASSERT_2(e, a, b) { if (!(e)) { \
        co_func_log_trace_3(CO_GEN_LOG_MODULE(DBG_ASSERT_LEVEL, 0), (uintptr_t)(a), (uintptr_t)(b), CO_LR_ADDRESS); \
        Plt_Assert(0, co_log_module_str(), CO_LOG_MODULE, __LINE__, CO_LR_ADDRESS); \
    }}

#define CO_ASSERT_3(e, a, b, c) { if (!(e)) { \
        co_func_log_trace_4(CO_GEN_LOG_MODULE(DBG_ASSERT_LEVEL, 0), (uintptr_t)(a), (uintptr_t)(b), (uintptr_t)(c), CO_LR_ADDRESS); \
        Plt_Assert(0, co_log_module_str(), CO_LOG_MODULE, __LINE__, CO_LR_ADDRESS); \
    }}

#define CO_LOG_MAIN_0(status) \
    co_func_log_trace_0(CO_GEN_LOG_MODULE(DBG_MAIN_FLOW_LEVEL, (status)))
#define CO_LOG_MAIN_1(status, a) \
    co_func_log_trace_1(CO_GEN_LOG_MODULE(DBG_MAIN_FLOW_LEVEL, (status)), (uintptr_t)(a))
#define CO_LOG_MAIN_2(status, a, b) \
    co_func_log_trace_2(CO_GEN_LOG_MODULE(DBG_MAIN_FLOW_LEVEL, (status)), (uintptr_t)(a), (uintptr_t)(b))
#define CO_LOG_MAIN_3(status, a, b, c) \
    co_func_log_trace_3(CO_GEN_LOG_MODULE(DBG_MAIN_FLOW_LEVEL, (status)), (uintptr_t)(a), (uintptr_t)(b), (uintptr_t)(c))
#define CO_LOG_MAIN_4(status, a, b, c, d) \
    co_func_log_trace_4(CO_GEN_LOG_MODULE(DBG_MAIN_FLOW_LEVEL, (status)), (uintptr_t)(a), (uintptr_t)(b), (uintptr_t)(c), (uintptr_t)(d))
#define CO_LOG_MAIN_S_0(status, tag) \
    co_func_log_trace_with_tag_0(CO_GEN_LOG_MODULE(DBG_MAIN_FLOW_LEVEL, (status)), (bt_log_tag_t){#tag})
#define CO_LOG_MAIN_S_1(status, tag, a) \
    co_func_log_trace_with_tag_1(CO_GEN_LOG_MODULE(DBG_MAIN_FLOW_LEVEL, (status)), (bt_log_tag_t){#tag}, (uintptr_t)(a))
#define CO_LOG_MAIN_S_2(status, tag, a, b) \
    co_func_log_trace_with_tag_2(CO_GEN_LOG_MODULE(DBG_MAIN_FLOW_LEVEL, (status)), (bt_log_tag_t){#tag}, (uintptr_t)(a), (uintptr_t)(b))
#define CO_LOG_MAIN_S_3(status, tag, a, b, c) \
    co_func_log_trace_with_tag_3(CO_GEN_LOG_MODULE(DBG_MAIN_FLOW_LEVEL, (status)), (bt_log_tag_t){#tag}, (uintptr_t)(a), (uintptr_t)(b), (uintptr_t)(c))
#define CO_LOG_MAIN_WITH_STR_0(status, s) \
    co_func_log_str_0(CO_GEN_LOG_MODULE(DBG_MAIN_FLOW_LEVEL, (status)), (s))
#define CO_LOG_MAIN_WITH_STR_1(status, s, a) \
    co_func_log_str_1(CO_GEN_LOG_MODULE(DBG_MAIN_FLOW_LEVEL, (status)), (s), (uintptr_t)(a))
#define CO_LOG_MAIN_WITH_STR_2(status, s, a, b) \
    co_func_log_str_2(CO_GEN_LOG_MODULE(DBG_MAIN_FLOW_LEVEL, (status)), (s), (uintptr_t)(a), (uintptr_t)(b))
#define CO_LOG_MAIN_WITH_STR_3(status, s, a, b, c) \
    co_func_log_str_3(CO_GEN_LOG_MODULE(DBG_MAIN_FLOW_LEVEL, (status)), (s), (uintptr_t)(a), (uintptr_t)(b), (uintptr_t)(c))

/**
 * Usage:
 *
 * 0. make sure globle cfg control DBG_DEBUG_PRINT_ENABLE is 1
 *
 * 1. set your  module in your source code file at the top(MUST).
 *      #undef MOUDLE
 *      #define MOUDLE BB
 *
 * 2. define the module debug level in debug_cfg.h,
 *      NOTICE,if no define, then no print
 *      #define BB_LEVEL INFO_LEVEL
 *
 * 3. use DEBUG_INFO,DEBUG_WARNING,and DEBUG_ERROR in your module's codes
 *
 */

#if DBG_DEBUG_PRINT_ENABLE == 1

#define DEBUGPRINT2(num,...) TRACE(num,__VA_ARGS__)
#define CO_DEBUG_PRINTF(num,...) do { DEBUGPRINT2(num,__VA_ARGS__); } while(0);

#if (BT_DBG_CONCAT(MOUDLE, _LEVEL) >= DBG_ERROR_LEVEL)
#define DEBUG_ERROR(num,...) CO_DEBUG_PRINTF(num, __VA_ARGS__)
#define CO_LOG_ERR_0(status) \
    co_func_log_trace_0(CO_GEN_LOG_MODULE(DBG_ERROR_LEVEL, (status)))
#define CO_LOG_ERR_1(status, a) \
    co_func_log_trace_1(CO_GEN_LOG_MODULE(DBG_ERROR_LEVEL, (status)), (uintptr_t)(a))
#define CO_LOG_ERR_2(status, a, b) \
    co_func_log_trace_2(CO_GEN_LOG_MODULE(DBG_ERROR_LEVEL, (status)), (uintptr_t)(a), (uintptr_t)(b))
#define CO_LOG_ERR_3(status, a, b, c) \
    co_func_log_trace_3(CO_GEN_LOG_MODULE(DBG_ERROR_LEVEL, (status)), (uintptr_t)(a), (uintptr_t)(b), (uintptr_t)(c))
#define CO_LOG_ERR_4(status, a, b, c, d) \
    co_func_log_trace_4(CO_GEN_LOG_MODULE(DBG_ERROR_LEVEL, (status)), (uintptr_t)(a), (uintptr_t)(b), (uintptr_t)(c), (uintptr_t)(d))
#define CO_LOG_ERR_S_0(status, tag) \
    co_func_log_trace_with_tag_0(CO_GEN_LOG_MODULE(DBG_ERROR_LEVEL, (status)), (bt_log_tag_t){#tag})
#define CO_LOG_ERR_S_1(status, tag, a) \
    co_func_log_trace_with_tag_1(CO_GEN_LOG_MODULE(DBG_ERROR_LEVEL, (status)), (bt_log_tag_t){#tag}, (uintptr_t)(a))
#define CO_LOG_ERR_S_2(status, tag, a, b) \
    co_func_log_trace_with_tag_2(CO_GEN_LOG_MODULE(DBG_ERROR_LEVEL, (status)), (bt_log_tag_t){#tag}, (uintptr_t)(a), (uintptr_t)(b))
#define CO_LOG_ERR_S_3(status, tag, a, b, c) \
    co_func_log_trace_with_tag_3(CO_GEN_LOG_MODULE(DBG_ERROR_LEVEL, (status)), (bt_log_tag_t){#tag}, (uintptr_t)(a), (uintptr_t)(b), (uintptr_t)(c))
#else
#define DEBUG_ERROR(...)
#define CO_LOG_ERR_0(status)
#define CO_LOG_ERR_1(status, a)
#define CO_LOG_ERR_2(status, a, b)
#define CO_LOG_ERR_3(status, a, b, c)
#define CO_LOG_ERR_4(status, a, b, c, d)
#define CO_LOG_ERR_S_0(status, tag)
#define CO_LOG_ERR_S_1(status, tag, a)
#define CO_LOG_ERR_S_2(status, tag, a, b)
#define CO_LOG_ERR_S_3(status, tag, a, b, c)
#endif

#if (BT_DBG_CONCAT(MOUDLE, _LEVEL) >= DBG_WARNING_LEVEL)
#define DEBUG_WARNING(num,...) CO_DEBUG_PRINTF(num, __VA_ARGS__)
#define CO_LOG_WAR_0(status) \
    co_func_log_trace_0(CO_GEN_LOG_MODULE(DBG_WARNING_LEVEL, (status)))
#define CO_LOG_WAR_1(status, a) \
    co_func_log_trace_1(CO_GEN_LOG_MODULE(DBG_WARNING_LEVEL, (status)), (uintptr_t)(a))
#define CO_LOG_WAR_2(status, a, b) \
    co_func_log_trace_2(CO_GEN_LOG_MODULE(DBG_WARNING_LEVEL, (status)), (uintptr_t)(a), (uintptr_t)(b))
#define CO_LOG_WAR_3(status, a, b, c) \
    co_func_log_trace_3(CO_GEN_LOG_MODULE(DBG_WARNING_LEVEL, (status)), (uintptr_t)(a), (uintptr_t)(b), (uintptr_t)(c))
#define CO_LOG_WAR_4(status, a, b, c, d) \
    co_func_log_trace_4(CO_GEN_LOG_MODULE(DBG_WARNING_LEVEL, (status)), (uintptr_t)(a), (uintptr_t)(b), (uintptr_t)(c), (uintptr_t)(d))
#define CO_LOG_WAR_S_0(status, tag) \
    co_func_log_trace_with_tag_0(CO_GEN_LOG_MODULE(DBG_WARNING_LEVEL, (status)), (bt_log_tag_t){#tag})
#define CO_LOG_WAR_S_1(status, tag, a) \
    co_func_log_trace_with_tag_1(CO_GEN_LOG_MODULE(DBG_WARNING_LEVEL, (status)), (bt_log_tag_t){#tag}, (uintptr_t)(a))
#define CO_LOG_WAR_S_2(status, tag, a, b) \
    co_func_log_trace_with_tag_2(CO_GEN_LOG_MODULE(DBG_WARNING_LEVEL, (status)), (bt_log_tag_t){#tag}, (uintptr_t)(a), (uintptr_t)(b))
#define CO_LOG_WAR_S_3(status, tag, a, b, c) \
    co_func_log_trace_with_tag_3(CO_GEN_LOG_MODULE(DBG_WARNING_LEVEL, (status)), (bt_log_tag_t){#tag}, (uintptr_t)(a), (uintptr_t)(b), (uintptr_t)(c))
#else
#define DEBUG_WARNING(...)
#define CO_LOG_WAR_0(status)
#define CO_LOG_WAR_1(status, a)
#define CO_LOG_WAR_2(status, a, b)
#define CO_LOG_WAR_3(status, a, b, c)
#define CO_LOG_WAR_4(status, a, b, c, d)
#define CO_LOG_WAR_S_0(status, tag)
#define CO_LOG_WAR_S_1(status, tag, a)
#define CO_LOG_WAR_S_2(status, tag, a, b)
#define CO_LOG_WAR_S_3(status, tag, a, b, c)
#endif

#if (BT_DBG_CONCAT(MOUDLE, _LEVEL) >= DBG_INFO_LEVEL)
#define DEBUG_INFO(num,...) CO_DEBUG_PRINTF(num, __VA_ARGS__)
#define CO_LOG_INFO_0(status) \
    co_func_log_trace_0(CO_GEN_LOG_MODULE(DBG_INFO_LEVEL, (status)))
#define CO_LOG_INFO_1(status, a) \
    co_func_log_trace_1(CO_GEN_LOG_MODULE(DBG_INFO_LEVEL, (status)), (uintptr_t)(a))
#define CO_LOG_INFO_2(status, a, b) \
    co_func_log_trace_2(CO_GEN_LOG_MODULE(DBG_INFO_LEVEL, (status)), (uintptr_t)(a), (uintptr_t)(b))
#define CO_LOG_INFO_3(status, a, b, c) \
    co_func_log_trace_3(CO_GEN_LOG_MODULE(DBG_INFO_LEVEL, (status)), (uintptr_t)(a), (uintptr_t)(b), (uintptr_t)(c))
#define CO_LOG_INFO_4(status, a, b, c, d) \
    co_func_log_trace_4(CO_GEN_LOG_MODULE(DBG_INFO_LEVEL, (status)), (uintptr_t)(a), (uintptr_t)(b), (uintptr_t)(c), (uintptr_t)(d))
#define CO_LOG_INFO_S_0(status, tag) \
    co_func_log_trace_with_tag_0(CO_GEN_LOG_MODULE(DBG_INFO_LEVEL, (status)), (bt_log_tag_t){#tag})
#define CO_LOG_INFO_S_1(status, tag, a) \
    co_func_log_trace_with_tag_1(CO_GEN_LOG_MODULE(DBG_INFO_LEVEL, (status)), (bt_log_tag_t){#tag}, (uintptr_t)(a))
#define CO_LOG_INFO_S_2(status, tag, a, b) \
    co_func_log_trace_with_tag_2(CO_GEN_LOG_MODULE(DBG_INFO_LEVEL, (status)), (bt_log_tag_t){#tag}, (uintptr_t)(a), (uintptr_t)(b))
#define CO_LOG_INFO_S_3(status, tag, a, b, c) \
    co_func_log_trace_with_tag_3(CO_GEN_LOG_MODULE(DBG_INFO_LEVEL, (status)), (bt_log_tag_t){#tag}, (uintptr_t)(a), (uintptr_t)(b), (uintptr_t)(c))
#else
#define DEBUG_INFO(...)
#define CO_LOG_INFO_0(status)
#define CO_LOG_INFO_1(status, a)
#define CO_LOG_INFO_2(status, a, b)
#define CO_LOG_INFO_3(status, a, b, c)
#define CO_LOG_INFO_4(status, a, b, c, d)
#define CO_LOG_INFO_S_0(status, tag)
#define CO_LOG_INFO_S_1(status, tag, a)
#define CO_LOG_INFO_S_2(status, tag, a, b)
#define CO_LOG_INFO_S_3(status, tag, a, b, c)
#endif

#if (BT_DBG_CONCAT(MOUDLE, _LEVEL) >= DBG_VERBOSE_LEVEL)
#define DEBUG_VERBOSE(num,...) CO_DEBUG_PRINTF(num, __VA_ARGS__)
#define CO_LOG_VBS_0(status) \
    co_func_log_trace_0(CO_GEN_LOG_MODULE(DBG_VERBOSE_LEVEL, (status)))
#define CO_LOG_VBS_1(status, a) \
    co_func_log_trace_1(CO_GEN_LOG_MODULE(DBG_VERBOSE_LEVEL, (status)), (uintptr_t)(a))
#define CO_LOG_VBS_2(status, a, b) \
    co_func_log_trace_2(CO_GEN_LOG_MODULE(DBG_VERBOSE_LEVEL, (status)), (uintptr_t)(a), (uintptr_t)(b))
#define CO_LOG_VBS_3(status, a, b, c) \
    co_func_log_trace_3(CO_GEN_LOG_MODULE(DBG_VERBOSE_LEVEL, (status)), (uintptr_t)(a), (uintptr_t)(b), (uintptr_t)(c))
#define CO_LOG_VBS_4(status, a, b, c, d) \
    co_func_log_trace_4(CO_GEN_LOG_MODULE(DBG_VERBOSE_LEVEL, (status)), (uintptr_t)(a), (uintptr_t)(b), (uintptr_t)(c), (uintptr_t)(d))
#define CO_LOG_VBS_S_0(status, tag) \
    co_func_log_trace_with_tag_0(CO_GEN_LOG_MODULE(DBG_VERBOSE_LEVEL, (status)), (bt_log_tag_t){#tag})
#define CO_LOG_VBS_S_1(status, tag, a) \
    co_func_log_trace_with_tag_1(CO_GEN_LOG_MODULE(DBG_VERBOSE_LEVEL, (status)), (bt_log_tag_t){#tag}, (uintptr_t)(a))
#define CO_LOG_VBS_S_2(status, tag, a, b) \
    co_func_log_trace_with_tag_2(CO_GEN_LOG_MODULE(DBG_VERBOSE_LEVEL, (status)), (bt_log_tag_t){#tag}, (uintptr_t)(a), (uintptr_t)(b))
#define CO_LOG_VBS_S_3(status, tag, a, b, c) \
    co_func_log_trace_with_tag_3(CO_GEN_LOG_MODULE(DBG_VERBOSE_LEVEL, (status)), (bt_log_tag_t){#tag}, (uintptr_t)(a), (uintptr_t)(b), (uintptr_t)(c))
#else
#define DEBUG_VERBOSE(...)
#define CO_LOG_VBS_0(status)
#define CO_LOG_VBS_1(status, a)
#define CO_LOG_VBS_2(status, a, b)
#define CO_LOG_VBS_3(status, a, b, c)
#define CO_LOG_VBS_4(status, a, b, c, d)
#define CO_LOG_VBS_S_0(status, tag)
#define CO_LOG_VBS_S_1(status, tag, a)
#define CO_LOG_VBS_S_2(status, tag, a, b)
#define CO_LOG_VBS_S_3(status, tag, a, b, c)
#endif

#if (BT_DBG_CONCAT(MOUDLE, _LEVEL) >= INFO_LEVEL)
void _debug_print_dump_data(char *mem, int mem_size);
#define DEBUG_DUMP_DATA(memaddr, memlen) do { \
        DEBUGPRINT2(2,"\r\n"  "module %s #%d: ", BT_DBG_GET_NAME(MOUDLE), __LINE__);\
        DEBUGPRINT2(0,"\r\n");\
        Plt_DUMP8("%02x ", (char *)memaddr,memlen);\
    } while(0);
#else
#define DEBUG_DUMP_DATA(...)
#endif

#define DEBUG_RAW(...) DEBUG_PRINT(__VA_ARGS__)
#define DEBUG_IMM(...) TRACE_IMM(__VA_ARGS__)

#else

#define DEBUG_ERROR(...)
#define CO_LOG_ERR_0(status)
#define CO_LOG_ERR_1(status, a)
#define CO_LOG_ERR_2(status, a, b)
#define CO_LOG_ERR_3(status, a, b, c)
#define CO_LOG_ERR_4(status, a, b, c, d)
#define CO_LOG_ERR_S_0(status, tag)
#define CO_LOG_ERR_S_1(status, tag, a)
#define CO_LOG_ERR_S_2(status, tag, a, b)
#define CO_LOG_ERR_S_3(status, tag, a, b, c)

#define DEBUG_WARNING(...)
#define CO_LOG_WAR_0(status)
#define CO_LOG_WAR_1(status, a)
#define CO_LOG_WAR_2(status, a, b)
#define CO_LOG_WAR_3(status, a, b, c)
#define CO_LOG_WAR_4(status, a, b, c, d)
#define CO_LOG_WAR_S_0(status, tag)
#define CO_LOG_WAR_S_1(status, tag, a)
#define CO_LOG_WAR_S_2(status, tag, a, b)
#define CO_LOG_WAR_S_3(status, tag, a, b, c)

#define DEBUG_INFO(...)
#define CO_LOG_INFO_0(status)
#define CO_LOG_INFO_1(status, a)
#define CO_LOG_INFO_2(status, a, b)
#define CO_LOG_INFO_3(status, a, b, c)
#define CO_LOG_INFO_4(status, a, b, c, d)
#define CO_LOG_INFO_S_0(status, tag)
#define CO_LOG_INFO_S_1(status, tag, a)
#define CO_LOG_INFO_S_2(status, tag, a, b)
#define CO_LOG_INFO_S_3(status, tag, a, b, c)

#define DEBUG_VERBOSE(...)
#define CO_LOG_VBS_0(status)
#define CO_LOG_VBS_1(status, a)
#define CO_LOG_VBS_2(status, a, b)
#define CO_LOG_VBS_3(status, a, b, c)
#define CO_LOG_VBS_4(status, a, b, c, d)
#define CO_LOG_VBS_S_0(status, tag)
#define CO_LOG_VBS_S_1(status, tag, a)
#define CO_LOG_VBS_S_2(status, tag, a, b)
#define CO_LOG_VBS_S_3(status, tag, a, b, c)

#define DEBUG_ASSERT(expr)
#define DEBUG_DUMP_DATA(...)
#define DEBUG_RAW(...)
#define DEBUG_IMM(...)
#endif /* DBG_DEBUG_PRINT_ENABLE */

#endif // CO_LOG_H_

