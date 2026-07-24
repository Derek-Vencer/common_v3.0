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
#ifndef __GAF_DBG_H__
#define __GAF_DBG_H__

#include <stdint.h>
#include "co_log.h"

/*ENUM*/
#define GAF_LOG_LEVEL_CRITICAL  (0)
#define GAF_LOG_LEVEL_ERROR     (1)
#define GAF_LOG_LEVEL_WARN      (2)
#define GAF_LOG_LEVEL_INFO      (3)
#define GAF_LOG_LEVEL_DEBUG     (4)

#define GAF_LOG_LEVEL_MAX       (5)

/*DEFINE*/
#if (GAF_LEVEL <= LOGGER_LEVEL_DEBUG)
#define GAF_LOG_LEVEL           GAF_LOG_LEVEL_DEBUG
#elif (GAF_LEVEL == LOGGER_LEVEL_INFO)
#define GAF_LOG_LEVEL           GAF_LOG_LEVEL_INFO
#elif (GAF_LEVEL == LOGGER_LEVEL_WARN)
#define GAF_LOG_LEVEL           GAF_LOG_LEVEL_WARN
#elif (GAF_LEVEL == LOGGER_LEVEL_ERROR)
#define GAF_LOG_LEVEL           GAF_LOG_LEVEL_ERROR
#elif (GAF_LEVEL == LOGGER_LEVEL_FATAL)
#define GAF_LOG_LEVEL           GAF_LOG_LEVEL_CRITICAL
#endif

#if (GAF_LOG_LEVEL > GAF_LOG_LEVEL_CRITICAL)
#ifndef GAF_LOG_MODULE
#define GAF_LOG_MODULE                  "GAF_LOG_UNDEF"
#endif /// GAF_LOG_MODULE

#define GAF_MODULE_GEN_AUD              "GEN_AUD"

#define GAF_MODULE_BAP_ISO_AL           "BAP_ISO_AL"

#define GAF_MODULE_BAP_UC_CLI           "BAP_UC_CLI"
#define GAF_MODULE_BAP_UC_SRV           "BAP_UC_SRV"

#define GAF_MODULE_BAP_BC_SINK          "BAP_BC_SINK"
#define GAF_MODULE_BAP_BC_SRC           "BAP_BC_SRC"

#define GAF_MODULE_BAP_SCAN_DELEG       "BAP_SCAN_DELEG"
#define GAF_MODULE_BAP_BC_ASSIST        "BAP_BC_ASSIST"

#define GAF_MODULE_ASCC                 "ASCC"
#define GAF_MODULE_ASCS                 "ASCS"
#define GAF_MODULE_PACC                 "PACC"
#define GAF_MODULE_PACS                 "PACS"
#define GAF_MODULE_BASS                 "BASS"

#define GAF_MODULE_MCC                  "MCC"
#define GAF_MODULE_MCS                  "MCS"
#define GAF_MODULE_TBC                  "TBC"
#define GAF_MODULE_TBS                  "TBS"
#define GAF_MODULE_VCC                  "VCC"
#define GAF_MODULE_VCS                  "VCS"
#define GAF_MODULE_MICC                 "MICC"
#define GAF_MODULE_MICS                 "MICS"
#define GAF_MODULE_AICC                 "AICC"
#define GAF_MODULE_AICS                 "AICS"
#define GAF_MODULE_VOCC                 "VOCC"
#define GAF_MODULE_VOCS                 "VOCS"
#define GAF_MODULE_CSI_CRYPTO           "CSI_CRYPTO"
#define GAF_MODULE_CSI_COORD            "CSI_COORD"
#define GAF_MODULE_CSI_MEMBER           "CSI_MEMBER"
#define GAF_MODULE_CSIS                 "CSI_SRV"
#define GAF_MODULE_CSISC                "CSI_CLI"
#define GAF_MODULE_CAC                  "CAC"
#define GAF_MODULE_CAS                  "CAS"
#define GAF_MODULE_TMAC                 "TMAC"
#define GAF_MODULE_TMAS                 "TMAS"
#define GAF_MODULE_HAC                  "HAC"
#define GAF_MODULE_HAS                  "HAS"
#define GAF_MODULE_OTC                  "OTC"
#define GAF_MODULE_OTS                  "OTS"
#define GAF_MODULE_GMAS                 "GMAS"
#define GAF_MODULE_GMAC                 "GMAC"
#define GAF_MODULE_VAC                  "VAC"
#define GAF_MODULE_VAS                  "VAS"
#define GAF_MODULE_SHS                  "SHS"
#endif

#define GAF_PRINTF(lvl, str, ...)\
                                gaf_log_printf(lvl, LOGGER_SECTION(str), ##__VA_ARGS__)

#if (GAF_LOG_LEVEL >= GAF_LOG_LEVEL_DEBUG)
#define GAF_LOG_D(str, ...)     GAF_PRINTF(GAF_LOG_LEVEL_DEBUG, "[D][%s][%d]"str, GAF_LOG_MODULE, __LINE__, ##__VA_ARGS__)
#else
#define GAF_LOG_D(str, ...)
#endif

#if (GAF_LOG_LEVEL >= GAF_LOG_LEVEL_INFO)
#define GAF_LOG_I(str, ...)     GAF_PRINTF(GAF_LOG_LEVEL_INFO, "[I][%s][%d]"str, GAF_LOG_MODULE, __LINE__, ##__VA_ARGS__)
#else
#define GAF_LOG_I(str, ...)
#endif

#if (GAF_LOG_LEVEL >= GAF_LOG_LEVEL_WARN)
#define GAF_LOG_W(str, ...)     GAF_PRINTF(GAF_LOG_LEVEL_WARN, "[W][%s][%d]"str, GAF_LOG_MODULE, __LINE__, ##__VA_ARGS__)
#else
#define GAF_LOG_W(str, ...)
#endif

#if (GAF_LOG_LEVEL >= GAF_LOG_LEVEL_ERROR)
#define GAF_LOG_E(str, ...)     GAF_PRINTF(GAF_LOG_LEVEL_ERROR, "[E][%s][%d]"str, GAF_LOG_MODULE, __LINE__, ##__VA_ARGS__)
#else
#define GAF_LOG_E(str, ...)
#endif

#if (GAF_LOG_LEVEL >= GAF_LOG_LEVEL_DEBUG)
#define GAF_DUMP8(str, ...)     GAF_LOG_D("[DUMP]"); gaf_log_dump8(str, ##__VA_ARGS__);
#define GAF_DUMP16(str, ...)    GAF_LOG_D("[DUMP]"); gaf_log_dump16(str, ##__VA_ARGS__);
#define GAF_DUMP32(str, ...)    GAF_LOG_D("[DUMP]"); gaf_log_dump32(str, ##__VA_ARGS__);
#else
#define GAF_DUMP8(str, ...)
#define GAF_DUMP16(str, ...)
#define GAF_DUMP32(str, ...)
#endif

int gaf_log_printf(uint8_t log_lvl, const char *format, ...);

int gaf_log_dump8(const char *format, const void *buffer, unsigned int count);

int gaf_log_dump16(const char *format, const void *buffer, unsigned int count);

int gaf_log_dump32(const char *format, const void *buffer, unsigned int count);

#endif /// __GAF_DBG_INC__
