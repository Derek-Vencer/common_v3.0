/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
 * @addtogroup APP_ACC
 * @{
 ****************************************************************************************
 */

#ifndef __APP_ACC_H__
#define __APP_ACC_H__
#if BLE_AUDIO_ENABLED

#include "bluetooth.h"
#include "app_gaf_define.h"

#ifdef __cplusplus
extern "C" {
#endif

int app_acc_start(uint8_t con_lid, bool is_mobile);

int app_acc_restore_service_bond_data(uint8_t con_lid, uint16_t uuid, const app_gaf_prf_svc_t *range,
                                      uint8_t count, const app_gaf_prf_char_t *chars,
                                      const app_gaf_prf_desc_t *descs, const uint16_t *p_uuid_list);

#ifdef __cplusplus
}
#endif

#endif
#endif // APP_ACC_H_

/// @} APP_ACC
