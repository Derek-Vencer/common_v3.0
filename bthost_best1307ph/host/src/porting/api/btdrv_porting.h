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
#ifndef __BTDRV_PORTING_H__
#define __BTDRV_PORTING_H__

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief BT DRV control afh assess api
 *
 * @param[in] en       Enable or disable afh assess
 *
 */
void bt_drv_reg_op_afh_assess_en(bool en);

/**
 * @brief BT DRV control same le address connection limitation api
 *
 * @param[in] enable   No limits or only allow different addressess
 *
 */
void bt_drv_reg_op_set_le_con_allow_use_same_addr(bool enable);

#if defined(__cplusplus)
}
#endif
#endif /* __BTDRV_PORTING_H__ */
