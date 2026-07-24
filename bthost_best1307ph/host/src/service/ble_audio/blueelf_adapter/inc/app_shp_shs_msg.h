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
 * @addtogroup APP_SHP

 * @{
 ****************************************************************************************
 */

#ifndef __APP_SHP_SHS_MSG_H__
#define __APP_SHP_SHS_MSG_H__
#include "bluetooth.h"

#if BLE_AUDIO_ENABLED
/**
 * @brief Set shp shs sat features bitfiled
 *
 * @param[in] sat_feat_bf
 *                     SHP SAT features bf
 *
 * @return int         status
 */
int app_shp_shs_set_role(uint8_t sat_feat_bf);

#endif
#endif // APP_SHP_SHS_MSG_H_

/// @} APP_SHP_SHS_MSG_H_
