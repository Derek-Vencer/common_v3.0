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
/**
 ****************************************************************************************
 * @addtogroup APP_VAP_VAS

 * @{
 ****************************************************************************************
 */
#ifndef __APP_VAP_VAS_MSG_H__
#define __APP_VAP_VAS_MSG_H__
#if BLE_AUDIO_ENABLED
#include "bluetooth.h"
#ifdef __cplusplus
extern "C" {
#endif

#define APP_VAP_GVAS_CCID                   (0x03)
#define APP_VAP_GVAS_VA_NAME               "BES AI Voice 1"
#define APP_VAP_GVAS_VA_UUID               "\x02\xB0"
#define APP_VAP_GVAS_INSTALLED_LOC          "Living Room"
#define APP_VAP_GVAS_VA_SUPP_LANGUAGES     "ZH,EN"

#define APP_VAP_VAS_DFT_NB_VA_SUPP          (1)
#define APP_VAP_VAS_CCID                    (0x04)
#define APP_VAP_VAS_VA_NAME                "BES AI Voice 2"
#define APP_VAP_VAS_VA_UUID                "\x02\xB0"
#define APP_VAP_VAS_INSTALLED_LOC           "Conference Room"
#define APP_VAP_VAS_VA_SUPP_LANGUAGES      "ZH,EN"

/**
 * @brief Voice Assistant server update session state and session flag api
 *
 * @param[in] con_lid  Local connection ID
 * @param[in] va_lid   Voice Asssistant local index
 * @param[in] state    Voice Asssistant session state
 * @param[in] va_lid   Voice Asssistant session flag
 *
 * @return int         Status
 */
int app_vap_vas_session_update(uint8_t con_lid, uint8_t va_lid, uint8_t state, uint8_t flag);

/**
 * @brief Voice Assistant server update character value
 *
 * @param[in] va_lid   Voice Asssistant local index
 * @param[in] char_type
 *                     Character Type
 * @param[in] val      Value updated
 * @param[in] val_len  Length of value updated
 *
 * @return int         Status
 */
int app_vap_vas_update_char_value_req(uint8_t va_lid, uint8_t char_type, uint8_t *val, uint8_t val_len);

#ifdef __cplusplus
}
#endif
#endif
#endif // APP_VAP_VAS_MSG_H_
