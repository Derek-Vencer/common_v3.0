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
#ifndef __AUDIO_CFG_H__
#define __AUDIO_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

enum AUDIO_PROCESS_TYPE_T {
    AUDIO_PROCESS_TYPE_SW_IIR_EQ = 0,
    AUDIO_PROCESS_TYPE_HW_DAC_IIR_EQ,
    AUDIO_PROCESS_TYPE_DRC,
    AUDIO_PROCESS_TYPE_HW_DRC,
    AUDIO_PROCESS_TYPE_LIMITER,
    AUDIO_PROCESS_TYPE_HW_DAC_LIMITER,
    AUDIO_PROCESS_TYPE_DYNAMIC_EQ,
    AUDIO_PROCESS_TYPE_VIRTUAL_3D,//Spa Audio(360 audio)
    AUDIO_PROCESS_TYPE_VIRTUALBASS,
    AUDIO_PROCESS_TYPE_HFP_CFG,
    AUDIO_PROCESS_TYPE_NUM,
};

void audio_cfg_section_init(void);

int store_audio_cfg_into_audio_section(enum AUDIO_PROCESS_TYPE_T type, uint8_t *cfg);
void *load_audio_cfg_from_audio_section(enum AUDIO_PROCESS_TYPE_T type, uint8_t index);
void *load_audio_cfg_and_len_from_audio_section(enum AUDIO_PROCESS_TYPE_T type, uint8_t index, uint32_t *cfg_len);

#ifdef __cplusplus
}
#endif

#endif
