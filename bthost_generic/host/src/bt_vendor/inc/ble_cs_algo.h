
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
#ifndef __BLE_CS_ALGO_H__
#define __BLE_CS_ALGO_H__

#ifdef __cplusplus
extern "C" {
#endif

void le_cs_algo_cal_distance(int antenna_number, int used_freq_number, float *used_freq_list, float *p_r_pct, float *p_t_pct, float *p_pbr_dist);

#ifdef __cplusplus
}
#endif

#endif
