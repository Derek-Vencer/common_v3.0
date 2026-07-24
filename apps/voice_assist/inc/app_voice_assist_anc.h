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
#ifndef __APP_VOICE_ASSIST_ANC_H__
#define __APP_VOICE_ASSIST_ANC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "anc_assist.h"

typedef struct
{
    AncAssistConfig *cfg;
    AncAssistRes *res;
} voice_anc_msg_t;

int app_voice_assist_anc_get_heap_buf_size();
void app_voice_assist_anc_sync_cfg_info();
void app_voice_assist_anc_playback_handler(float *play_buf, uint32_t block_len);
uint32_t app_voice_assist_anc_pilot_set_play_fadeout(void);
AncAssistConfig *app_voice_assist_anc_get_cfg(void);
bool app_voice_assist_anc_need_max_sample_rate();
void app_voice_assist_pilot_set_play_fadeout(void);
void app_voice_assist_anc_reset_pilot_state(void);
int32_t app_voice_assist_get_prompt_anc_index(int *anc_index,float *band1, float * band2, float *band3, int32_t channel_idx);
int32_t app_voice_assist_anc_init(void);
int32_t app_voice_assist_open_anc();
int32_t app_voice_assist_close_anc();
int32_t app_voice_assist_reset_st_cfg(void);
int32_t assist_algo_dsp_init(void);
AncAssistConfig *app_voice_assist_anc_get_cfg(void);
int32_t app_voice_assist_reset_anc();

#ifdef __cplusplus
}
#endif

#endif