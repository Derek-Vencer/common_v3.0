/***************************************************************************
 *
 * Copyright (c) 2015-2024 BES Technic
 *
 * Authored by BES CD team (Blueelf Prj).
 *
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means: or stored in a database or retrieval system: without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright: trade secret:
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifndef __BLE_RANGING_LOG_H__
#define __BLE_RANGING_LOG_H__

#include "bluetooth.h"
#include "hal_trace.h"

/*ENUM*/
#define LERA_LOG_LEVEL_CRITICAL  (0)
#define LERA_LOG_LEVEL_ERROR     (1)
#define LERA_LOG_LEVEL_WARN      (2)
#define LERA_LOG_LEVEL_INFO      (3)
#define LERA_LOG_LEVEL_DEBUG     (4)

#define LERA_LOG_LEVEL_MAX       (5)

/*DEFINE*/
#ifndef LERA_LOG_LEVEL
#define LERA_LOG_LEVEL           LERA_LOG_LEVEL_DEBUG
#endif

#ifndef LERA_LOG_MODULE
#define LERA_LOG_MODULE          "LERA_LOG_UNDEF"
#endif /// LERA_LOG_MODULE

#if defined (BUILD_BTH_ROM)
#define LERA_PRINTF(lvl, str, ...)\
                                lera_log_printf(lvl, str, ##__VA_ARGS__)
#else
#define LERA_PRINTF(lvl, str, ...)\
                                TRACE(TR_MOD(BLESTACK), str, ##__VA_ARGS__)
#endif

#if (LERA_LOG_LEVEL >= LERA_LOG_LEVEL_DEBUG)
#define LERA_LOG_D(str, ...)     LERA_PRINTF(LERA_LOG_LEVEL_DEBUG, "[D][%s][%d]"str, LERA_LOG_MODULE, __LINE__, ##__VA_ARGS__)
#else
#define LERA_LOG_D(str, ...)
#endif

#if (LERA_LOG_LEVEL >= LERA_LOG_LEVEL_INFO)
#define LERA_LOG_I(str, ...)     LERA_PRINTF(LERA_LOG_LEVEL_INFO, "[I][%s][%d]"str, LERA_LOG_MODULE, __LINE__, ##__VA_ARGS__)
#else
#define LERA_LOG_I(str, ...)
#endif

#if (LERA_LOG_LEVEL >= LERA_LOG_LEVEL_WARN)
#define LERA_LOG_W(str, ...)     LERA_PRINTF(LERA_LOG_LEVEL_WARN, "[W][%s][%d]"str, LERA_LOG_MODULE, __LINE__, ##__VA_ARGS__)
#else
#define LERA_LOG_W(str, ...)
#endif

#if (LERA_LOG_LEVEL >= LERA_LOG_LEVEL_ERROR)
#define LERA_LOG_E(str, ...)     LERA_PRINTF(LERA_LOG_LEVEL_ERROR, "[E][%s][%d]"str, LERA_LOG_MODULE, __LINE__, ##__VA_ARGS__)
#else
#define LERA_LOG_E(str, ...)
#endif

#if (LERA_LOG_LEVEL >= LERA_LOG_LEVEL_DEBUG)
#define LERA_DUMP8(str, ...)     LERA_LOG_D("[DUMP]"); lera_log_dump8(str, ##__VA_ARGS__);
#define LERA_DUMP16(str, ...)    LERA_LOG_D("[DUMP]"); lera_log_dump16(str, ##__VA_ARGS__);
#define LERA_DUMP32(str, ...)    LERA_LOG_D("[DUMP]"); lera_log_dump32(str, ##__VA_ARGS__);
#else
#define LERA_DUMP8(str, ...)
#define LERA_DUMP16(str, ...)
#define LERA_DUMP32(str, ...)
#endif

int lera_log_printf(uint8_t log_lvl, const char *format, ...);

int lera_log_dump8(const char *format, const void *buffer, unsigned int count);

int lera_log_dump16(const char *format, const void *buffer, unsigned int count);

int lera_log_dump32(const char *format, const void *buffer, unsigned int count);
#endif  /* __BLE_RANGING_LOG_H__ */
