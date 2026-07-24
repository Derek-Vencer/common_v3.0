/***************************************************************************
 *
 * Copyright 2015-2024 BES.
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
#include <stddef.h>
#include "hal_trace.h"
#include "app_voice_assist.h"

int32_t app_voice_assist_init()
{
#if defined(ANC_ASSIST_ENABLED)
    app_voice_assist_anc_init();

#if defined(VOICE_ASSIST_ADA_IIR)
    app_voice_assist_ada_iir_init();
#endif

#if defined(AUDIO_ADAPTIVE_EQ)
    app_voice_assist_adaptive_eq_init();
#endif

#if defined(AUDIO_ADAPTIVE_VOLUME)
    ANC_TRACE(0,"[%s] adaptive_volume_init", __func__);
    app_voice_assist_adaptive_volume_init();
#endif

#if defined(AUDIO_ADJ_EQ_REV)
    app_voice_assist_adj_eq_rev_init();
#endif

#if defined(__AI_VOICE__)
    app_voice_assist_ai_voice_init();
#endif

#if defined(VOICE_ASSIST_CUSTOM_LEAK_DETECT)
    app_voice_assist_custom_leak_detect_init();
#endif

#if defined(VOICE_ASSIST_FF_FIR_LMS)
    app_voice_assist_fir_anc_open_leak_init();
    app_voice_assist_fir_lms_init();
#endif

#if defined(VOICE_ASSIST_FF_IIR_LMS)
    app_voice_assist_iir_lms_init();
#endif

#if defined(VOICE_ASSIST_NOISE)
    app_voice_assist_noise_adapt_anc_init();
#endif

#if defined(VOICE_ASSIST_NOISE_CLASSIFY)
    app_voice_assist_noise_classify_adapt_anc_init();
#endif

#if defined(VOICE_ASSIST_ONESHOT_ADAPTIVE_ANC)
    app_voice_assist_oneshot_adapt_anc_init();
#endif

#ifdef VOICE_ASSIST_VPU_WSD
    app_voice_assist_vpu_wsd_init();
#endif

#if defined(VOICE_ASSIST_OPTIMAL_TF_ANC)
    app_voice_assist_optimal_tf_anc_init();
#endif

#if defined(VOICE_ASSIST_PILOT_ANC_ENABLED)
    app_voice_assist_pilot_anc_init();
#endif

    app_voice_assist_pnc_adapt_anc_init();

#if defined(VOICE_ASSIST_PROMPT_LEAK_DETECT)
    app_voice_assist_prompt_leak_detect_init();
#endif

#if defined(VOICE_ASSIST_WD_ENABLED)
    app_voice_assist_ultrasound_init();
#endif

#if defined(VOICE_ASSIST_ADAPTIVE_MODE)
    app_voice_assist_adaptive_mode_init();
#endif

    app_voice_assist_wd_init();

    voice_assist_user_info_t *user_index = app_anc_assist_get_user_index();
    for (uint8_t i=0; i < ANC_ASSIST_USER_MAX ; ++i) {
        if (user_index[i].user_name != NULL) {
            ANC_TRACE(0,"[%s] USER_NAME : %s index : %d", __func__, user_index[i].user_name, i);
        }
    }
#endif

    return 0;
}
