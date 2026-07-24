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
#ifndef __HW_DAC_DRC_H__
#define __HW_DAC_DRC_H__

#include "hal_aud.h"

#ifdef __cplusplus
extern "C" {
#endif

int hw_dac_drc_open(int ch_map);
void hw_dac_drc_enable(int ch_map);
int hw_dac_drc_set_cfg(enum AUD_SAMPRATE_T rate,int index);
void hw_dac_drc_disable(int ch_map);
void hw_dac_drc_close(void);

#ifdef __cplusplus
}
#endif

#endif
