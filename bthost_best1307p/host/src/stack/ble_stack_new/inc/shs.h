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
#ifndef __GAF_SHS_H__
#define __GAF_SHS_H__

#include "gaf_cfg.h"

#include "sh_common.h"

/**
 * @brief Spatial Headphone server initilization
 *
 * @param  sat_feat_bf Features bitfield @see enum shp_sat_feat_bf
 * @param  pref_mtu_size
 *                     Preferred MTU size
 *
 * @return int         status
 */
int shs_init(uint8_t sat_feat_bf, uint16_t pref_mtu_size);

/**
 * @brief Spatial Headphone server deinitilization
 *
 * @return int         status
 */
int shs_deinit(void);

/**
 * @brief Spatial Headphone server set shp sat features
 *
 * @param sat_feat_bf  Features bitfield @see enum shp_sat_feat_bf
 *
 * @return int         status
 */
int shs_set_sat_features(uint8_t sat_feat_bf);

#endif /// __GAF_SHS_INC__
