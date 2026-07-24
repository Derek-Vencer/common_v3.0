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
#ifdef ANC_ASSIST_ENABLED
#include "hal_trace.h"
#include "plat_types.h"
#include "anc_assist.h"
#include "app_anc_assist.h"
#include "app_voice_assist_anc.h"
#include <string.h>
#include "app_utils.h"
#include "cmsis_os.h"
#include "tgt_hardware.h"
#include "anc_assist_anc.h"

#if defined(IBRT) && !defined(FREEMAN_ENABLED_STERO)
#if defined(VOICE_ASSIST_FF_FIR_LMS) || defined(VOICE_ASSIST_FF_IIR_LMS)
#define BASE_HEAP_BUFF_SIZE        (15 * 1024)
#elif defined(ASSIST_LOW_RAM_MOD)
#define BASE_HEAP_BUFF_SIZE        (16 * 1024)
#else
#define BASE_HEAP_BUFF_SIZE        (30 * 1024)
#endif
#else
#define BASE_HEAP_BUFF_SIZE        (42 * 1024)
#endif

#ifdef VOICE_ASSIST_VPU_WSD
#define ASSIST_VPU_WSD_BUFF_SIZE (100 * 1024)
#else
#define ASSIST_VPU_WSD_BUFF_SIZE (0)
#endif

#ifdef VOICE_ASSIST_ADAPTIVE_MODE
#define ASSIST_ADAPTIVE_MODE_BUFF_SIZE (35 * 1024)
#else
#define ASSIST_ADAPTIVE_MODE_BUFF_SIZE (0)
#endif

#define HEAP_BUFF_SIZE  (BASE_HEAP_BUFF_SIZE + ASSIST_VPU_WSD_BUFF_SIZE + ASSIST_ADAPTIVE_MODE_BUFF_SIZE)

#if defined(VOICE_ASSIST_FF_FIR_LMS) || defined(VOICE_ASSIST_FF_IIR_LMS)
static uint8_t *anc_assist_heap_buff = NULL;
#else
static uint8_t __attribute__((aligned(4))) anc_assist_heap_buff[HEAP_BUFF_SIZE];
#endif

#if defined(IBRT) && !defined(FREEMAN_ENABLED_STERO)
#define ALGO_CHANNEL_NUM    (4)
#else
#define ALGO_CHANNEL_NUM    (8)
#endif

#if defined(ASSIST_LOW_RAM_MOD)
#define ALGO_SAMPLE_RATE    (8000)
#define ALGO_FRAME_LEN      (60)
#else
#define ALGO_SAMPLE_RATE  	(16000)
#define ALGO_FRAME_LEN   	(120)
#endif

#define ALGO_FRAME_MS       (1000.0 * ALGO_FRAME_LEN / ALGO_SAMPLE_RATE)

#define _SAMPLE_BITS 24
POSSIBLY_UNUSED static uint32_t user_index;
POSSIBLY_UNUSED static const uint8_t* const user_name = (const uint8_t*)"anc";
POSSIBLY_UNUSED static AncAssistRes anc_assist_res;
static AncAssistState *anc_assist_st = NULL;
static int32_t g_sample_bits = _SAMPLE_BITS;
extern AncAssistConfig anc_assist_cfg;
voice_anc_msg_t voice_anc_msg;
static bool g_voice_assist_anc_flag = false;
static uint8_t g_voice_assist_anc_cnt = 0;

WEAK void psap_set_bands_gain_f32(float *gain_l, float *gain_r)
{
    return ;
}

void app_voice_assist_anc_sync_cfg_info()
{
    if (anc_assist_st != NULL) {
        anc_assist_set_cfg_sync(anc_assist_st, &anc_assist_cfg);
    }
}

AncAssistConfig *app_voice_assist_anc_get_cfg(void)
{
    uint32_t cfg_size = anc_assist_get_AncAssistConfig_size();
    ASSERT(cfg_size == sizeof(AncAssistConfig),"[%s] AncAssistConfig size err",__func__);

    return &anc_assist_cfg;
}

bool app_voice_assist_anc_need_max_sample_rate()
{
    if((anc_assist_cfg.fir_lms_en == true) || (anc_assist_cfg.optimal_tf_en == true) || (anc_assist_cfg.pnc_en == true) || (anc_assist_cfg.iir_lms_en == true)) {
        return true;
    } else {
        return false;
    }
}

void app_voice_assist_anc_playback_handler(float *play_buf, uint32_t block_len)
{
    if (anc_assist_st != NULL) {
        anc_assist_pilot_get_play_data(anc_assist_st, play_buf, block_len);
    }
}

extern bool pilot_play_get_fadeout_state(void);
uint32_t app_voice_assist_anc_pilot_set_play_fadeout(void)
{
#ifndef VOICE_ASSIST_WD_ENABLED
    if (anc_assist_st != NULL) {
        anc_assist_pilot_set_play_fadeout(anc_assist_st);
    }
    osDelay(anc_assist_cfg.pilot_cfg.gain_smooth_ms + 300);     // 300: More time to fadeout
#else
    if (pilot_play_get_fadeout_state()) {
        osDelay(anc_assist_cfg.pilot_cfg.gain_smooth_ms + 300);
    } else {
        osDelay(10);
    }
#endif
    return 0;
}

void app_voice_assist_pilot_set_play_fadeout(void)
{
    if (anc_assist_st != NULL) {
        anc_assist_pilot_set_play_fadeout(anc_assist_st);
    }
}

void app_voice_assist_anc_reset_pilot_state(void)
{
    if (anc_assist_st != NULL)  {
        anc_assist_reset_pilot_state(anc_assist_st);
    }
}

int32_t app_voice_assist_get_prompt_anc_index(int *anc_index,float *band1, float * band2, float *band3, int32_t channel_idx)
{
    ASSERT(anc_assist_st != NULL,"[%s] st is null",__func__);
    anc_assist_get_prompt_anc_index(anc_assist_st,anc_index,band1,band2,band3,channel_idx);

    return 0;
}

static void _anc_assist_callback(anc_assist_notify_t msg, void *data, uint32_t value)
{

}

int voice_assist_anc_get_heap_buf_size(anc_assist_mode_t assist_mode)
{
#if defined(VOICE_ASSIST_FF_FIR_LMS) || defined(VOICE_ASSIST_FF_IIR_LMS)
    if (assist_mode == ANC_ASSIST_MODE_PHONE_CALL || assist_mode == ANC_ASSIST_MODE_RECORD || assist_mode == ANC_ASSIST_MODE_LE_CALL) {
        return 0;
    } else {
        return HEAP_BUFF_SIZE;
    }
#else
    return 0;
#endif
}

#include "ext_heap.h"
static int32_t voice_assist_anc_open()
{
    //ANC_TRACE(2, "[%s] ", __func__);
   // anc_msg = (voice_anc_msg_t *)voice_assist_get_user_ptr(user_name);
   // ASSERT(anc_msg != NULL, "[%s] anc_msg is NULL", __func__);
  //  memcpy(&anc_assist_cfg, anc_msg->cfg, sizeof(anc_assist_cfg));
#if defined(VOICE_ASSIST_FF_FIR_LMS) || defined(VOICE_ASSIST_FF_IIR_LMS)
    uint32_t heap_size = voice_assist_anc_get_heap_buf_size(app_anc_assist_get_mode());

    if (heap_size) {
        anc_assist_heap_buff = voice_assist_stream_heap_malloc(heap_size);
    } else {
        anc_assist_heap_buff = NULL;
        return 0;
    }
#endif
    ext_heap_init(anc_assist_heap_buff);
    anc_assist_st = anc_assist_create(ALGO_SAMPLE_RATE, g_sample_bits, ALGO_CHANNEL_NUM, ALGO_FRAME_LEN, &anc_assist_cfg, _anc_assist_callback, &ext_allocator);
    int32_t g_play_sample_rate = app_anc_assist_get_play_sample_rate();
    ANC_TRACE(2, "[%s] playback stream sample rate = %d", __FUNCTION__, g_play_sample_rate);
    anc_assist_pilot_set_play_sample_rate(anc_assist_st, g_play_sample_rate);

    return 0;
}

static int32_t voice_assist_anc_close(void)
{
    ANC_TRACE(1, "[%s] ...", __func__);

    if (anc_assist_st != NULL)  {
        anc_assist_destroy(anc_assist_st);
        anc_assist_st = NULL;
    }

#if defined(VOICE_ASSIST_FF_FIR_LMS) || defined(VOICE_ASSIST_FF_IIR_LMS)
    if (anc_assist_heap_buff != NULL) {
        voice_assist_stream_heap_free(anc_assist_heap_buff);
        ext_heap_deinit();
    }
#else
    ext_heap_deinit();
#endif

    return 0;
}

extern int32_t anc_assist_get_required_mips(AncAssistState *st);
static int32_t voice_assist_anc_get_freq(void)
{
    if (anc_assist_st != NULL) {
        return anc_assist_get_required_mips(anc_assist_st);
    } else {
        return 0;
    }
}

static int32_t voice_assist_anc_process(voice_assist_process_frame_data_t *process_frame_data)
{
#if defined(VOICE_ASSIST_FF_FIR_LMS) || defined(VOICE_ASSIST_FF_IIR_LMS)
    anc_assist_mode_t assist_mode = app_anc_assist_get_mode();
    if (assist_mode == ANC_ASSIST_MODE_PHONE_CALL || assist_mode == ANC_ASSIST_MODE_RECORD || assist_mode == ANC_ASSIST_MODE_LE_CALL) {
        return 0;
    }
#endif
    anc_assist_res = anc_assist_process(anc_assist_st,
                     process_frame_data->ff_mic, process_frame_data->ff_ch_num,
                     process_frame_data->fb_mic, process_frame_data->fb_ch_num,
                     process_frame_data->talk_mic, process_frame_data->talk_ch_num,
                     process_frame_data->ref, process_frame_data->ref_ch_num,
                     process_frame_data->vpu_mic,
                     process_frame_data->frame_len);

    if (any_of_u32(anc_assist_res.ff_gain_changed, process_frame_data->ff_ch_num, ANC_ASSIST_ALGO_STATUS_CHANGED) ||
        any_of_u32(anc_assist_res.fb_gain_changed, process_frame_data->fb_ch_num, ANC_ASSIST_ALGO_STATUS_CHANGED)) {
#ifdef ANC_APP
       anc_assist_anc_set_gain_coef(anc_assist_res.ff_gain_changed, anc_assist_res.ff_gain_id, anc_assist_res.ff_gain, process_frame_data->ff_ch_num, anc_assist_res.fb_gain_changed, anc_assist_res.fb_gain_id, anc_assist_res.fb_gain, process_frame_data->fb_ch_num);
#endif
    } else {
        ;
    }

    if (anc_assist_res.psap_changed == ANC_ASSIST_ALGO_STATUS_CHANGED) {
        for (int i=0; i<PSAP_BANK_SIZE; ++i) {
            ANC_TRACE(0, "psap_band_gain[%d]*1000 = %d",i, (int32_t)(anc_assist_res.psap_band_gain[i] * 1000));
        }
        psap_set_bands_gain_f32(anc_assist_res.psap_band_gain, NULL);
    }
    // TODO: deal with stereo headphone

    if (anc_assist_res.curve_changed[0] == ANC_ASSIST_ALGO_STATUS_CHANGED) {
#ifdef ANC_APP
       anc_assist_anc_switch_curve(anc_assist_res.curve_id[0], anc_assist_res.curve_index[0]);
#endif
    } else {
        ;
    }

    return 0;
}

static int32_t voice_assist_anc_reset(void)
{
    ANC_TRACE(0,"[%s] ...",__func__);

    if (anc_assist_st != NULL) {
        anc_assist_algo_reset_impl(anc_assist_st, &anc_assist_cfg);
    }

    return 0;
}

static int32_t voice_assist_anc_get_fs(void)
{
    return 16000;
}

static int32_t voice_assist_anc_set_mode(anc_assist_set_mode_t mode)
{
    ANC_TRACE(0,"[%s]...",__func__);
#if defined(VOICE_ASSIST_FF_FIR_LMS) || defined(VOICE_ASSIST_FF_IIR_LMS)
    if (mode == ANC_ASSIST_SET_MODE_ON) {
        voice_assist_anc_open();
    } else if (mode == ANC_ASSIST_SET_MODE_OFF) {
        voice_assist_anc_close();
    }
#endif

    return 0;
}

static const voice_assist_algo_callback_t anc_assist_algo_dsp = {
    .open               = voice_assist_anc_open,
    .close              = voice_assist_anc_close,
    .cap_process        = voice_assist_anc_process,
    .reset              = voice_assist_anc_reset,
    .get_freq           = voice_assist_anc_get_freq,
    .get_fs             = voice_assist_anc_get_fs,
    .set_mode           = voice_assist_anc_set_mode,
    .get_heap_buf_size  = voice_assist_anc_get_heap_buf_size,
};

int32_t app_voice_assist_get_mic_map()
{
    uint32_t anc_mic_map = 0;
    anc_mic_map |= ANC_FF_MIC_CH_L | ANC_FB_MIC_CH_L;
#if defined(FREEMAN_ENABLED_STERO)
    anc_mic_map |= ANC_FF_MIC_CH_R | ANC_FB_MIC_CH_R;
#endif

#if !defined(VOICE_ASSIST_WIND_SINGLE_MIC)
#if defined(FREEMAN_ENABLED_STERO)
    anc_mic_map |= ANC_FF_MIC_CH_R;
#else
    anc_mic_map |= ANC_TALK_MIC_CH_L;
#endif
#endif

    ANC_TRACE(0, "[%s] ANC_ASSIST_USER_ANC", __func__);

    AncAssistConfig *cfg = app_voice_assist_anc_get_cfg();
    //NOTE : set all cfg false,anc should be user 0
    cfg->ff_howling_en = false;
    cfg->fb_howling_en = false;
    cfg->psap_ns_en = false;
    cfg->noise_en = false;
    cfg->wind_en = false;
    cfg->noise_classify_en = false;
    cfg->wind_single_mic_en = false;
    cfg->pilot_en = false;
    cfg->pnc_en = false;
    cfg->wsd_en = false;
    cfg->extern_kws_en = false;
    cfg->ultrasound_en = false;
    cfg->prompt_adaptive_en = false;
    cfg->fir_lms_en = false;
    cfg->ada_iir_en = false;
    cfg->optimal_tf_en = false;
    cfg->fir_anc_open_leak_en = false;
    cfg->adaptive_mode_en = false;
    cfg->extern_adj_eq_rev_en = false;

    cfg->vpu_wsd_en = false;
    cfg->vpu_aec_en = false;
#ifndef ASSIST_LOW_RAM_MOD
    cfg->ff_howling_en = true;
    cfg->fb_howling_en = true;
#endif
    cfg->wind_en = true;

    return anc_mic_map;
}


static const voice_assist_stream_callback_t voice_assist_anc_stream = {
    .get_mic_ch_map       = app_voice_assist_get_mic_map,
};

static voice_assist_user_register_cfg voice_assist_anc_stream_cfg = {
    .voice_assist_callback = &voice_assist_anc_stream,
    .assist_algo_dsp = &anc_assist_algo_dsp,
};

int32_t app_voice_assist_anc_init(void)
{
    voice_anc_msg.cfg = &anc_assist_cfg;
    voice_anc_msg.res = &anc_assist_res;
    user_index = app_voice_assist_stream_register(user_name, &voice_assist_anc_stream_cfg, (uint8_t *)&voice_anc_msg);
    app_voice_assist_dsp_register(&user_index, user_name , &voice_assist_anc_stream_cfg);

    return 0;
}

int32_t app_voice_assist_open_anc()
{
    uint32_t lock = int_lock();
    g_voice_assist_anc_cnt++;
    if (g_voice_assist_anc_flag == true) {
        int_unlock(lock);
        return 0;
    }
    g_voice_assist_anc_flag = true;
    int_unlock(lock);

    app_anc_assist_open(user_index);

    return 0;
}

int32_t app_voice_assist_reset_anc()
{
    app_anc_assist_reset(user_index);

    return 0;
}

int32_t app_voice_assist_close_anc()
{
    uint32_t lock = int_lock();
    g_voice_assist_anc_cnt--;

    if (g_voice_assist_anc_cnt < 0) {
        g_voice_assist_anc_cnt = 0;
    }

    if (g_voice_assist_anc_cnt != 0) {
        int_unlock(lock);
        return 0;
    }
    g_voice_assist_anc_flag = false;
    int_unlock(lock);

    ANC_TRACE(0, "[%s] close", __func__);

    app_anc_assist_close(user_index);
    return 0;
}
#endif
