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
#ifndef __APP_ASSIST_ANC_H__
#define __APP_ASSIST_ANC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#if defined(ANC_ASSIST_ENABLED)
#include "app_anc_assist.h"
#include "app_voice_assist_ai_voice.h"
#include "app_voice_assist_wd.h"
#include "app_voice_assist_anc.h"
#include "app_voice_assist_pilot_anc.h"
#include "app_voice_assist_ultrasound.h"
#if defined(VOICE_ASSIST_CUSTOM_LEAK_DETECT)
#include "app_voice_assist_custom_leak_detect.h"
#endif
#include "app_voice_assist_prompt_leak_detect.h"
#include "app_voice_assist_noise_adapt_anc.h"
#if defined(VOICE_ASSIST_NOISE_CLASSIFY)
#include "app_voice_assist_noise_classify_adapt_anc.h"
#endif
#if defined(VOICE_ASSIST_FF_FIR_LMS)
#include "app_voice_assist_fir_lms.h"
#include "app_voice_assist_fir_anc_open_leak.h"
#endif
#if defined(VOICE_ASSIST_FF_IIR_LMS)
#include "app_voice_assist_iir_lms.h"
#endif
#if defined(AUDIO_ADAPTIVE_VOLUME)
#include "app_voice_assist_adaptive_volume.h"
#endif
#if defined(VOICE_ASSIST_ADA_IIR)
#include "app_voice_assist_ada_iir.h"
#endif
#if defined(VOICE_ASSIST_ONESHOT_ADAPTIVE_ANC)
#include "app_voice_assist_oneshot_adapt_anc.h"
#endif
#if defined(VOICE_ASSIST_VPU_WSD)
#include "app_voice_assist_vpu_wsd.h"
#endif
#if defined(AUDIO_ADAPTIVE_EQ)
#include "app_voice_assist_adaptive_eq.h"
#endif
#if defined(AUDIO_ADJ_EQ)
#include "app_voice_assist_adj_eq.h"
#endif
#include "app_voice_assist_optimal_tf.h"
#include "app_voice_assist_pnc_adapt_anc.h"
#if defined(AUDIO_ADJ_EQ_REV)
#include "app_voice_assist_adj_eq_rev.h"
#endif
#if defined(VOICE_ASSIST_ADAPTIVE_MODE)
#include "app_voice_assist_adaptive_mode.h"
#endif
#endif

int32_t app_voice_assist_init();

#ifdef __cplusplus
}
#endif

#endif