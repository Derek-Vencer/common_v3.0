/***************************************************************************
 *
 * Copyright 2015-2023 BES.
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
#include "gaf_cc_stream_process.h"
#include "cmsis.h"
#include "string.h"
#include "plat_types.h"
#include "hal_trace.h"
#include "speech_memory.h"
#include "iir_resample.h"
#include "app_mcpp.h"
#include "cc_stream_common.h"
#include "audio_ramp.h"

#define GAF_CC_UPSAMPLING_POOL_SIZE   (2 * 1024)

#ifdef ENABLE_CALL_TX_STARTUP_FADEIN
static RampHandle g_gaf_tx_ramp_handle = 0;
#endif

static CC_STREAM_ALGO_CFG_T g_tx_algo_cfg = {0};
static bool g_tx_algo_enabled = false;
static CC_HEAP_ALLOC g_capture_buf_alloc = NULL;

static uint32_t g_capture_upsampling_factor = 1;
static uint8_t *g_capture_upsampling_buf = NULL;
static uint32_t g_capture_upsampling_buf_size = 0;
static IirResampleState *g_capture_upsampling_st = NULL;
static uint8_t *g_capture_upsampling_heap_buf = NULL;
static heap_handle_t g_capture_upsampling_heap = NULL;

void *gaf_cc_upsampling_malloc(size_t size)
{
    if (size == 0)
        return NULL;

    void *ptr = NULL;
    ptr = heap_malloc(g_capture_upsampling_heap, size);
    ASSERT(ptr != NULL, "[%s] no memory: size=%u", __FUNCTION__, size);

    return ptr;
}

void *gaf_cc_upsampling_calloc(size_t nmemb, size_t size)
{
    if (size == 0)
        return NULL;

    void *ptr = gaf_cc_upsampling_malloc(nmemb * size);
    if (ptr) {
        memset(ptr, 0 , nmemb * size);
    }

    return ptr;
}

void gaf_cc_upsampling_free(void *p)
{
    if (p) {
        heap_free(g_capture_upsampling_heap, p);
        p = NULL;
    }
}

static custom_allocator gaf_cc_upsampling_allocator = {
    .malloc = gaf_cc_upsampling_malloc,
    .calloc = gaf_cc_upsampling_calloc,
    .free = gaf_cc_upsampling_free,
};

#if defined(SPEECH_ALGO_DSP)
#ifdef MCPP_VERSION_STRING
static const APP_MCPP_CONFIG_T app_mcpp_lea_call_cfg = {
    .name = "LEA_CALL",
    .stream = {
        {
            .stream_enable = true,
            .server_core = { g_mcpp_core, },
            .pcm_info = {
                {
                    .algo_name = "Call_Capture",
                },
            },
            .in_data_mode = {
                DATA_PROCESS_MODE_DEINTERLEAVE,
            },
            .out_data_mode = {
                DATA_PROCESS_MODE_NONE,
            },
            .is_low_priority_task = {
                true,
            },
            .wait_server_process = true,
        },
    },
};

#define LEA_CALL_CAPTURE_STREAM  APP_MCPP_STREAM_INDEX_1
static APP_MCPP_CONFIG_T *g_mcpp_lea_call_st = NULL;
#else
static uint8_t *g_capture_ref_buf = NULL;
static uint8_t *g_capture_ref2_buf = NULL;
static uint8_t *g_capture_vpu_buf = NULL;
#endif
#endif

int32_t gaf_cc_stream_process_init(void)
{
    return 0;
}

int32_t gaf_cc_stream_process_deinit(void)
{
    return 0;
}

int32_t gaf_cc_stream_process_ctrl(void)
{
    return 0;
}

uint32_t gaf_cc_stream_process_need_capture_buf_size(void)
{
    uint32_t size = 0;
    TRACE(0, "[%s] size: %d", __func__, size);

    return size;
}

int32_t gaf_cc_stream_capture_process_set_buf_alloc(void *buf_alloc)
{
    g_capture_buf_alloc = (CC_HEAP_ALLOC)buf_alloc;

    return 0;
}

static void *gef_cc_stream_get_capture_upsampling_buf(void)
{
    return (void *)g_capture_upsampling_buf;
}

static void gaf_cc_stream_capture_upsampling_open(CC_STREAM_ALGO_CFG_T *algo_cfg, CODEC_INFO_T *encoder_info)
{
    if (algo_cfg->sample_rate < encoder_info->sample_rate) {
        uint32_t encoder_sample = encoder_info->sample_rate;
        uint32_t algo_sample = algo_cfg->sample_rate;
        ASSERT(encoder_sample % algo_sample == 0,
            "%s error. %d, %d. Can not do resample", __func__, encoder_sample, algo_sample);

        g_capture_upsampling_factor = encoder_info->sample_rate / algo_cfg->sample_rate;

        if (algo_cfg->channel_num < g_capture_upsampling_factor){
            TRACE(0, "[%s] ch_num: %d, factor: %d",
                __func__, algo_cfg->channel_num, g_capture_upsampling_factor);
            g_capture_upsampling_buf_size = encoder_info->pcm_size;
            if (!g_capture_upsampling_buf) {
                g_capture_upsampling_buf = (uint8_t*)g_capture_buf_alloc(NULL, g_capture_upsampling_buf_size);
            }
        } else {
            g_capture_upsampling_buf_size = 0;
            g_capture_upsampling_buf = NULL;
        }
        TRACE(0, "[%s] Resample %d--> %d. factor: %d",
            __func__, algo_sample, encoder_sample, g_capture_upsampling_factor);

        if (!g_capture_upsampling_heap_buf) {
            g_capture_upsampling_heap_buf = (uint8_t *)g_capture_buf_alloc(NULL, GAF_CC_UPSAMPLING_POOL_SIZE);
        }
        g_capture_upsampling_heap = heap_register(g_capture_upsampling_heap_buf, GAF_CC_UPSAMPLING_POOL_SIZE);
        g_capture_upsampling_st = multi_iir_resample_init_with_custom_allocator(algo_cfg->frame_len,
                                                        algo_cfg->bits,
                                                        1,
                                                        iir_resample_choose_mode(algo_sample, encoder_sample),
                                                        &gaf_cc_upsampling_allocator);
    } else {
        g_capture_upsampling_st = NULL;
        g_capture_upsampling_factor = 1;
        g_capture_upsampling_buf_size = 0;
    }
}

int32_t gaf_cc_stream_capture_process_open(void *algo_cfg, void *encoder_info)
{
    g_tx_algo_cfg = *(CC_STREAM_ALGO_CFG_T*)algo_cfg;

    if (g_tx_algo_cfg.channel_num == 0) {
        return 0;
    }

    if (g_capture_buf_alloc == NULL) {
        return 0;
    }

    gaf_cc_stream_capture_upsampling_open((CC_STREAM_ALGO_CFG_T*)algo_cfg, (CODEC_INFO_T*)encoder_info);

    TRACE(0, "[LE Call] bypass:%d,frame_len:%d,sample_rate:%d,ch_num:%d,bits:%d",
        g_tx_algo_cfg.bypass,
        g_tx_algo_cfg.frame_len,
        g_tx_algo_cfg.sample_rate,
        g_tx_algo_cfg.channel_num,
        g_tx_algo_cfg.bits);

#ifdef ENABLE_CALL_TX_STARTUP_FADEIN
    RampConfig ramp_cfg = {0};
    ramp_cfg.sample_rate = g_tx_algo_cfg.sample_rate;
    ramp_cfg.bit_depth = g_tx_algo_cfg.bits;
    ramp_cfg.num_channels = g_tx_algo_cfg.channel_num;
    ramp_cfg.ramp_time_ms = 30;
    ramp_cfg.initial_gain = 0;
    ramp_cfg.type = RAMP_SMOOTHSTEP;
    ramp_cfg.silence_time_ms = 200;

    g_gaf_tx_ramp_handle = audio_ramp_open(&ramp_cfg);
    TRACE(0, "%s, gaf_tx_ramp:%d", __func__, g_gaf_tx_ramp_handle);
    audio_ramp_set_target(g_gaf_tx_ramp_handle, 1.0);
#endif

    if (g_tx_algo_cfg.bypass) {
        g_tx_algo_enabled = false;
        return 0;
    } else {
        g_tx_algo_enabled = true;
    }

#if defined(SPEECH_ALGO_DSP)
    uint32_t sample_bytes = g_tx_algo_cfg.bits <= 16 ? 2 : 4;
#ifdef MCPP_VERSION_STRING
    if (g_mcpp_lea_call_st == NULL) {
        g_mcpp_lea_call_st = app_mcpp_create_new_config(&app_mcpp_lea_call_cfg);
    }

    APP_MCPP_STREAM_PCM_INFO_T *cap_pcm_info = &(g_mcpp_lea_call_st->stream[LEA_CALL_CAPTURE_STREAM].pcm_info[0]);

    cap_pcm_info->in.sample_rate = g_tx_algo_cfg.sample_rate;
    cap_pcm_info->in.sample_bytes = sample_bytes;
    cap_pcm_info->in.io_frame_len = g_tx_algo_cfg.frame_len;
    cap_pcm_info->in.algo_frame_len = g_tx_algo_cfg.algo_frame_len;
    cap_pcm_info->in.channel_num = g_tx_algo_cfg.channel_num;
    cap_pcm_info->out.channel_num = 1;

    cap_pcm_info->params[SCO_MODE] = 1;

    app_mcpp_open(g_mcpp_lea_call_st, LEA_CALL_CAPTURE_STREAM);
#else
    APP_MCPP_CFG_T dsp_cfg;

    memset(&dsp_cfg, 0, sizeof(APP_MCPP_CFG_T));
    dsp_cfg.capture.stream_enable  = true;
    dsp_cfg.capture.sample_rate    = g_tx_algo_cfg.sample_rate;
    dsp_cfg.capture.sample_bytes   = sample_bytes;
    dsp_cfg.capture.frame_len      = g_tx_algo_cfg.frame_len;
    dsp_cfg.capture.algo_frame_len = g_tx_algo_cfg.algo_frame_len;
    dsp_cfg.capture.channel_num    = g_tx_algo_cfg.channel_num;
    dsp_cfg.capture.core_server    = APP_MCPP_CORE_M55;
    dsp_cfg.capture.params[1]      = g_tx_algo_cfg.params[1];

    dsp_cfg.playback.stream_enable  = false;

    if (dsp_cfg.capture.params[1] & PCM_CHANNEL_MAP_CAP_REF) {
        dsp_cfg.capture.channel_num -= 1;
        if (!g_capture_ref_buf) {
            g_capture_ref_buf = (uint8_t *)g_capture_buf_alloc(NULL, sample_bytes * g_tx_algo_cfg.frame_len);
        }
    }

    if (dsp_cfg.capture.params[1] & PCM_CHANNEL_MAP_CAP_REF2) {
        dsp_cfg.capture.channel_num -= 1;
        if (!g_capture_ref2_buf) {
            g_capture_ref2_buf = (uint8_t *)g_capture_buf_alloc(NULL, sample_bytes * g_tx_algo_cfg.frame_len);
        }
    }

    if (dsp_cfg.capture.params[1] & PCM_CHANNEL_MAP_CAP_VPU) {
        dsp_cfg.capture.channel_num -= 1;
        if (!g_capture_vpu_buf) {
            g_capture_vpu_buf = (uint8_t *)g_capture_buf_alloc(NULL, sample_bytes * g_tx_algo_cfg.frame_len);
        }
    }

    app_mcpp_open(APP_MCPP_USER_CALL, &dsp_cfg);
#endif
#endif

    return 0;
}

static void gaf_cc_stream_capture_upsampling_close(void)
{
    if (g_capture_upsampling_st) {
        iir_resample_destroy(g_capture_upsampling_st);
        g_capture_upsampling_st = NULL;
        g_capture_upsampling_factor = 1;
    }

    g_capture_upsampling_buf_size = 0;
}

int32_t gaf_cc_stream_capture_process_close(void)
{
    if (g_tx_algo_cfg.channel_num == 0) {
        memset(&g_tx_algo_cfg, 0, sizeof(CC_STREAM_ALGO_CFG_T));
        return 0;
    }

    if (g_capture_buf_alloc == NULL) {
        return 0;
    }

    gaf_cc_stream_capture_upsampling_close();

    if (g_tx_algo_enabled) {
#if defined(SPEECH_ALGO_DSP)
#ifdef MCPP_VERSION_STRING
        if (app_mcpp_close(g_mcpp_lea_call_st, LEA_CALL_CAPTURE_STREAM) == 0) {
            g_mcpp_lea_call_st = NULL;
        } else {
            TRACE(0, "[%s] warning, please check mcpp_lea_call_st.", __func__);
            app_mcpp_close_all(g_mcpp_lea_call_st);
            g_mcpp_lea_call_st = NULL;
        }
#else
        app_mcpp_close(APP_MCPP_USER_CALL);
#endif
#endif
        g_tx_algo_enabled = false;

        uint32_t total = 0, used = 0, max_used = 0;
        speech_memory_info(&total, &used, &max_used);
        TRACE(4, "[%s] total=%d, used=%d, max_used=%d.", __func__, total, used, max_used);
    }

#ifdef ENABLE_CALL_TX_STARTUP_FADEIN
    audio_ramp_close(g_gaf_tx_ramp_handle);
    g_gaf_tx_ramp_handle = 0;
#endif

    g_capture_buf_alloc = NULL;
    memset(&g_tx_algo_cfg, 0, sizeof(CC_STREAM_ALGO_CFG_T));
    return 0;
}

uint32_t POSSIBLY_UNUSED gaf_cc_stream_capture_upsampling_run(uint8_t *buf, uint32_t len)
{
    uint32_t POSSIBLY_UNUSED sample_bytes = g_tx_algo_cfg.bits <= 16 ? 2 : 4;
    uint32_t POSSIBLY_UNUSED pcm_len = len / sample_bytes;

    if (g_capture_upsampling_st) {
        if (g_capture_upsampling_buf){
            iir_resample_process(g_capture_upsampling_st, buf, g_capture_upsampling_buf, pcm_len);
        } else {
            iir_resample_process(g_capture_upsampling_st, buf, buf, pcm_len);
        }
        len *= g_capture_upsampling_factor;
    }
    return len;
}

uint32_t gaf_cc_stream_capture_process_run(uint8_t *buf, uint32_t len)
{
    if (g_tx_algo_cfg.channel_num == 0) {
        return len;
    }

    if (g_capture_buf_alloc == NULL) {
        return len;
    }

#if defined(SPEECH_ALGO_DSP)
    if (g_tx_algo_enabled) {
#ifdef MCPP_VERSION_STRING
        APP_MCPP_PCM_T pcm_cfg = { 0 };
        pcm_cfg.in = buf;
        pcm_cfg.out = buf;
        app_mcpp_process(g_mcpp_lea_call_st, LEA_CALL_CAPTURE_STREAM, &pcm_cfg);
#else
        uint32_t mic_ch_num = g_tx_algo_cfg.channel_num;
        if (g_capture_vpu_buf) {
            mic_ch_num -= 1;
            if (g_tx_algo_cfg.bits == 16) {
                int16_t *capture_vpu_buf = (int16_t *)g_capture_vpu_buf;
                int16_t *pcm_buf = (int16_t *)buf;
                for (uint32_t i = 0; i < g_tx_algo_cfg.frame_len; i++) {
                    capture_vpu_buf[i] = pcm_buf[g_tx_algo_cfg.channel_num * i + mic_ch_num];
                }
            } else {
                int32_t *capture_vpu_buf = (int32_t *)g_capture_vpu_buf;
                int32_t *pcm_buf = (int32_t *)buf;
                for (uint32_t i = 0; i < g_tx_algo_cfg.frame_len; i++) {
                    capture_vpu_buf[i] = pcm_buf[g_tx_algo_cfg.channel_num * i + mic_ch_num];
                }
            }
        }

        if (g_capture_ref2_buf) {
            mic_ch_num -= 1;
            if (g_tx_algo_cfg.bits == 16) {
                int16_t *capture_ref2_buf = (int16_t *)g_capture_ref2_buf;
                int16_t *pcm_buf = (int16_t *)buf;
                for (uint32_t i = 0; i < g_tx_algo_cfg.frame_len; i++) {
                    capture_ref2_buf[i] = pcm_buf[g_tx_algo_cfg.channel_num * i + mic_ch_num];
                }
            } else {
                int32_t *capture_ref2_buf = (int32_t *)g_capture_ref2_buf;
                int32_t *pcm_buf = (int32_t *)buf;
                for (uint32_t i = 0; i < g_tx_algo_cfg.frame_len; i++) {
                    capture_ref2_buf[i] = pcm_buf[g_tx_algo_cfg.channel_num * i + mic_ch_num];
                }
            }
        }

        if (g_capture_ref_buf) {
            mic_ch_num -= 1;
            if (g_tx_algo_cfg.bits == 16) {
                int16_t *capture_ref_buf = (int16_t *)g_capture_ref_buf;
                int16_t *pcm_buf = (int16_t *)buf;
                for (uint32_t i = 0; i < g_tx_algo_cfg.frame_len; i++) {
                    capture_ref_buf[i] = pcm_buf[g_tx_algo_cfg.channel_num * i + mic_ch_num];
                }
            } else {
                int32_t *capture_ref_buf = (int32_t *)g_capture_ref_buf;
                int32_t *pcm_buf = (int32_t *)buf;
                for (uint32_t i = 0; i < g_tx_algo_cfg.frame_len; i++) {
                    capture_ref_buf[i] = pcm_buf[g_tx_algo_cfg.channel_num * i + mic_ch_num];
                }
            }
        }

        if (g_tx_algo_cfg.bits == 16) {
            int16_t *pcm_buf = (int16_t *)buf;
            for (uint32_t i = 0; i < g_tx_algo_cfg.frame_len; i++) {
                for (uint32_t ch = 0; ch < mic_ch_num; ch++) {
                    pcm_buf[mic_ch_num * i + ch] = pcm_buf[g_tx_algo_cfg.channel_num * i + ch];
                }
            }
        } else {
            int32_t *pcm_buf = (int32_t *)buf;
            for (uint32_t i = 0; i < g_tx_algo_cfg.frame_len; i++) {
                for (uint32_t ch = 0; ch < mic_ch_num; ch++) {
                    pcm_buf[mic_ch_num * i + ch] = pcm_buf[g_tx_algo_cfg.channel_num * i + ch];
                }
            }
        }

        APP_MCPP_CAP_PCM_T pcm_cfg;
        memset(&pcm_cfg, 0, sizeof(pcm_cfg));
        pcm_cfg.in = buf;
        pcm_cfg.ref = g_capture_ref_buf;
        pcm_cfg.ref2 = g_capture_ref2_buf;
        pcm_cfg.vpu = g_capture_vpu_buf;
        pcm_cfg.out = buf;
        pcm_cfg.frame_len = g_tx_algo_cfg.frame_len;

        app_mcpp_capture_process(APP_MCPP_USER_CALL, &pcm_cfg);
#endif
    } else
#endif
    {
        if (g_tx_algo_cfg.bits == 16) {
            int16_t *pcm_buf = (int16_t *)buf;
            for (uint32_t i = 0; i < g_tx_algo_cfg.frame_len; i++) {
                pcm_buf[i] = pcm_buf[i * g_tx_algo_cfg.channel_num + 0];
            }
        } else if (g_tx_algo_cfg.bits == 24) {
            int32_t *pcm_buf = (int32_t *)buf;
            for (uint32_t i = 0; i < g_tx_algo_cfg.frame_len; i++) {
                pcm_buf[i] = pcm_buf[i * g_tx_algo_cfg.channel_num + 0];
            }
        }
    }

    len /= g_tx_algo_cfg.channel_num;

#ifdef ENABLE_CALL_TX_STARTUP_FADEIN
    audio_ramp_process(g_gaf_tx_ramp_handle, buf, buf, len, true);
#endif

    len = gaf_cc_stream_capture_upsampling_run(buf, len);

    return len;
}

void gaf_cc_stream_capture_process_buf_deinit(void)
{
    g_capture_upsampling_buf = NULL;
    g_capture_upsampling_heap_buf = NULL;
#if defined(SPEECH_ALGO_DSP) && !defined(MCPP_VERSION_STRING)
    g_capture_ref_buf = NULL;
    g_capture_ref2_buf = NULL;
    g_capture_vpu_buf = NULL;
#endif
}

static CC_STREAM_ALGO_FUNC_LIST_T cc_capture_algo_func_list =
{
    .algo_run = gaf_cc_stream_capture_process_run,
    .algo_open = gaf_cc_stream_capture_process_open,
    .algo_close = gaf_cc_stream_capture_process_close,
    .algo_buf_init = gaf_cc_stream_capture_process_set_buf_alloc,
    .algo_buf_deinit = gaf_cc_stream_capture_process_buf_deinit,
    .get_resample_buf = gef_cc_stream_get_capture_upsampling_buf,
};

void gaf_cc_capture_algo_func_register(void *_func_list)
{
    CC_STREAM_ALGO_FUNC_LIST_T **func_list = (CC_STREAM_ALGO_FUNC_LIST_T**)_func_list;
    *func_list = &cc_capture_algo_func_list;
}

static CC_STREAM_ALGO_CFG_T g_rx_algo_cfg = {0};
static bool g_rx_algo_enabled = false;
static CC_HEAP_ALLOC g_playback_buf_alloc = NULL;

int32_t gaf_cc_stream_playback_process_set_buf_alloc(void *buf_alloc)
{
    g_playback_buf_alloc = (CC_HEAP_ALLOC)buf_alloc;

    return 0;
}

int32_t gaf_cc_stream_playback_process_open(void *algo_cfg, void *decoder_info)
{
    g_rx_algo_cfg = *(CC_STREAM_ALGO_CFG_T*)algo_cfg;

    if (g_rx_algo_cfg.channel_num == 0) {
        return 0;
    }

    if (g_rx_algo_cfg.bypass) {
        g_rx_algo_enabled = false;
        TRACE(0, "[LE Playback] bypass:%d", g_rx_algo_cfg.bypass);
        return 0;
    } else {
        g_rx_algo_enabled = true;
    }

    TRACE(0, "[LE Playback] bypass:%d,frame_len:%d,sample_rate:%d,ch_num:%d,bits:%d",
        g_rx_algo_cfg.bypass,
        g_rx_algo_cfg.frame_len,
        g_rx_algo_cfg.sample_rate,
        g_rx_algo_cfg.channel_num,
        g_rx_algo_cfg.bits);

#if defined(APP_MCPP_CLI_M55) && defined(USE_MCPP_AFTER_M55_DECODER)
    APP_MCPP_CFG_T dsp_cfg;
    uint32_t sample_bytes = g_rx_algo_cfg.bits <= 16 ? 2 : 4;

    memset(&dsp_cfg, 0, sizeof(APP_MCPP_CFG_T));
    dsp_cfg.capture.stream_enable  = false;

    dsp_cfg.playback.stream_enable  = true;
    dsp_cfg.playback.sample_rate    = g_rx_algo_cfg.sample_rate;
    dsp_cfg.playback.sample_bytes   = sample_bytes;
    dsp_cfg.playback.frame_len      = g_rx_algo_cfg.frame_len;
    dsp_cfg.playback.algo_frame_len = g_rx_algo_cfg.algo_frame_len;
    dsp_cfg.playback.channel_num    = g_rx_algo_cfg.channel_num;
    dsp_cfg.playback.core_server    = APP_MCPP_CORE_M55;

    app_mcpp_open(APP_MCPP_USER_AUDIO, &dsp_cfg);
#endif

    return 0;
}

int32_t gaf_cc_stream_playback_process_close(void)
{
    if (g_rx_algo_cfg.channel_num == 0) {
        memset(&g_rx_algo_cfg, 0, sizeof(CC_STREAM_ALGO_CFG_T));
        return 0;
    }

    if (g_playback_buf_alloc == NULL) {
        return 0;
    }

    if (g_rx_algo_enabled) {
#if defined(APP_MCPP_CLI_M55) && defined(USE_MCPP_AFTER_M55_DECODER)
        app_mcpp_close(APP_MCPP_USER_AUDIO);
#endif
    }

    g_rx_algo_enabled = false;
    g_playback_buf_alloc = NULL;
    memset(&g_rx_algo_cfg, 0, sizeof(CC_STREAM_ALGO_CFG_T));

    return 0;
}

uint32_t gaf_cc_stream_playback_process_run(uint8_t *buf, uint32_t len)
{
    if (g_rx_algo_cfg.channel_num == 0) {
        return len;
    }

    if (g_playback_buf_alloc == NULL) {
        return len;
    }

#if defined(APP_MCPP_CLI_M55) && defined(USE_MCPP_AFTER_M55_DECODER)
    if (g_rx_algo_enabled) {
        int pcm_len = 0;
        if(g_rx_algo_cfg.bits == 16) {
            pcm_len = len / sizeof(uint16_t);
        } else {
            pcm_len = len / sizeof(uint32_t);
        }

        APP_MCPP_PLAY_PCM_T pcm_cfg;
        memset(&pcm_cfg, 0, sizeof(pcm_cfg));
        pcm_cfg.in = buf;
        pcm_cfg.out = buf;
        pcm_cfg.frame_len = pcm_len;

        app_mcpp_playback_process(APP_MCPP_USER_AUDIO, &pcm_cfg);
    }
#endif

    return len;
}

void gaf_cc_stream_playback_process_buf_deinit(void)
{
    return;
}

static CC_STREAM_ALGO_FUNC_LIST_T cc_playback_algo_func_list =
{
    .algo_run = gaf_cc_stream_playback_process_run,
    .algo_open = gaf_cc_stream_playback_process_open,
    .algo_close = gaf_cc_stream_playback_process_close,
    .algo_buf_init = gaf_cc_stream_playback_process_set_buf_alloc,
    .algo_buf_deinit = gaf_cc_stream_playback_process_buf_deinit,
};

void gaf_cc_playback_algo_func_register(void *_func_list)
{
    CC_STREAM_ALGO_FUNC_LIST_T **func_list = (CC_STREAM_ALGO_FUNC_LIST_T**)_func_list;
    *func_list = &cc_playback_algo_func_list;
}
