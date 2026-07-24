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
 * @addtogroup APP_VAP_VAC

 * @{
 ****************************************************************************************
 */
#ifndef __APP_VAP_VAC_MSG_H__
#define __APP_VAP_VAC_MSG_H__
#if BLE_AUDIO_ENABLED
#include "bluetooth.h"
#ifdef __cplusplus
extern "C" {
#endif

#define APP_VAP_VAC_DFT_NB_VA_SUPP          (2)

/**
 * @brief Voice Assistant client vas discovery
 *
 * @param[in] con_lid  Local connection ID
 *
 * @return int         Status
 */
int app_vap_vac_start(uint8_t con_lid);

/**
 * @brief Voice Assistant client control api
 *
 * @param[in] con_lid  Local connection ID
 * @param[in] va_lid   Voice Asssistant local index
 * @param[in] opcode   Operation code
 *
 * @return int         Status
 */
int app_vap_vac_control(uint8_t con_lid, uint8_t va_lid, uint8_t opcode);

#ifdef __cplusplus
}
#endif
#endif
#endif // APP_VAP_VAC_MSG_H_
