/***************************************************************************
 *
 * Copyright 2015-2022 BES.
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
#ifndef __BLE_COMMON_DEFINE_H__
#define __BLE_COMMON_DEFINE_H__
#ifdef BLE_HOST_SUPPORT
#include "ble_device_info.h"
#ifdef __cplusplus
extern "C" {
#endif

/// APS3 is little endian
#define CPU_LE          1

/// CIS Channel handle mapping
#define BLE_CISHDL_MIN                  (0x100)
#define BLE_ACTID_TO_CISHDL(act_id)     ((uint16_t) (BLE_CISHDL_MIN + (act_id)))
#define BLE_CISHDL_TO_ACTID(cishdl)     ((uint8_t) ((cishdl) - BLE_CISHDL_MIN))
#define BLE_IS_CISHDL(conhdl)           ((((conhdl) >= BLE_CISHDL_MIN) && ((conhdl) < BLE_BISHDL_MIN)) ? true : false)
/// BIS Channel handle mapping
#define BLE_BISHDL_MIN                  (0x200)

#ifdef WALKIE_TALKIE_ENABLED
#define BLE_BISHDL_TO_ACTID(bishdl)     ((uint8_t) ((bishdl) - BLE_BISHDL_MIN)/3) 
/*controller say that each bis iso need 3 ACT_IDX ,so the interval for bis hdl is 3 ,for example 0x205,0x208,0x20b,0x20d*/
#else
#define BLE_ACTID_TO_BISHDL(act_id)     ((uint16_t) (BLE_BISHDL_MIN + (act_id)))
#define BLE_BISHDL_TO_ACTID(bishdl)     ((uint8_t) ((bishdl) - BLE_BISHDL_MIN))
#endif

#define BLE_IS_BISHDL(conhdl)           (((conhdl) >= BLE_BISHDL_MIN) ? true : false)

/// ISO Channel handle mapping
#define BLE_ISOHDL_TO_ACTID(isohdl)     (((isohdl >= BLE_BISHDL_MIN) \
                                                    ? BLE_BISHDL_TO_ACTID(isohdl) \
                                                    : BLE_CISHDL_TO_ACTID(isohdl)))

#ifdef __cplusplus
}
#endif
#endif
#endif /* __BLE_COMMON_DEFINE_H__ */
