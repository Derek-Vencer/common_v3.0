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
/**
 * NOTE:
 *  1.  APP_GAF_BAP_CONTEXT_TYPE_CONVERSATIONAL is call case
 *      a. ifdef STREAM_PROCESS_ENABLE_USE_CC_CAPTURE, call tx algo run in M55, so bth process rx pcm
 *      b. ifndef STREAM_PROCESS_ENABLE_USE_CC_CAPTURE, bth process both tx and rx, because tx and rx algo share
 *          speech init and deinit, so just call in capture. Will fix this issue in future
 *  2.  APP_GAF_BAP_CONTEXT_TYPE_LIVE is binaural recording case
 *      a. Need to enable BINAURAL_RECORD_PROCESS
 *  3.  APP_GAF_BAP_CONTEXT_TYPE_MEDIA is music case
 *
 * TODO:
 *  1.  Separate speech init and deinit for capture and playback
 *  2.  Provice API to get dynamic RAM from call and binaural recording
 **/
#include "gaf_stream_process.h"
// Platform
#include "cmsis.h"
#include "string.h"
#include "hal_trace.h"
#include "audio_dump.h"
#include "app_audio.h"
// Algo process
#include "app_mcpp.h"
#if defined(SPEECH_ALGO_DSP)
#ifdef MCPP_VERSION_STRING
#include "bt_sco_chain_mcpp.h"
#endif
#endif

#include "bt_sco_chain.h"
#ifdef GAF_CONVERSATIONAL_STREAM_PROCESS_ENABLE
#include "bt_sco_chain_lea_reframe.h"
#endif
#include "af_stream_sw_gain.h"
#include "audio_ramp.h"
#include "audio_process.h"
#include "speech_memory.h"
#include "arm_math_ex.h"
#include "iir_resample.h"
#ifdef BINAURAL_RECORD_PROCESS
#include "binaural_record_process.h"
#endif
#include "signal_generator.h"
// Bluetooth
#include "app_bap.h"
#include "bes_aob_api.h"
#include "app_audio_active_device_manager.h"

// #define GAF_STREAM_CAPTURE_AUDIO_DUMP
// #define GAF_STREAM_CAPTURE_1K_TONE
// #define DISABLE_ALL_STREAM_PROCESS

#ifdef BINAURAL_RECORD_PROCESS
#define GAF_STREAM_LIVE_ENABLED
#endif

#define GAF_STREAM_UNKNOWN_CONTEXT ( \
    BES_BLE_GAF_CONTEXT_TYPE_UNSPECIFIED_BIT )

#define GAF_STREAM_CALL_CONTEXT ( \
    BES_BLE_GAF_CONTEXT_TYPE_CONVERSATIONAL_BIT | \
    BES_BLE_GAF_CONTEXT_TYPE_INSTRUCTIONAL_BIT | \
    BES_BLE_GAF_CONTEXT_TYPE_MAN_MACHINE_BIT | \
    BES_BLE_GAF_CONTEXT_TYPE_RINGTONE_BIT | \
    BES_BLE_GAF_CONTEXT_TYPE_TV_BIT )

#define GAF_STREAM_LIVE_CONTEXT ( \
    BES_BLE_GAF_CONTEXT_TYPE_LIVE_BIT )

#define GAF_STREAM_MEDIA_CONTEXT ( \
    BES_BLE_GAF_CONTEXT_TYPE_MEDIA_BIT )

typedef enum {
    GAF_STREAM_SCENE_NONE = 0,
    GAF_STREAM_SCENE_CALL,
    GAF_STREAM_SCENE_LIVE,
    GAF_STREAM_SCENE_MEDIA,
    GAF_STREAM_SCENE_MAX
} gaf_stream_scene_t;

#if defined(ENABLE_CALL_TX_STARTUP_FADEIN) && !defined(STREAM_PROCESS_ENABLE_USE_CC_CAPTURE)
static RampHandle g_gaf_tx_ramp_handle = 0;
#endif

static uint16_t g_capture_stream = BES_BLE_GAF_CONTEXT_TYPE_UNSPECIFIED_BIT;
static uint16_t g_playback_stream = BES_BLE_GAF_CONTEXT_TYPE_UNSPECIFIED_BIT;

static uint32_t g_capture_channel_num = 0;
static uint32_t g_playback_channel_num = 0;

static struct AF_STREAM_CONFIG_T g_capture_stream_cfg;
static struct AF_STREAM_CONFIG_T g_playback_stream_cfg;

static uint8_t *g_capture_stream_buf = NULL;
static uint8_t *g_playback_stream_buf = NULL;
static uint32_t g_capture_stream_buf_size = 0;
static uint32_t g_playback_stream_buf_size = 0;

static POSSIBLY_UNUSED bool g_call_algo_inited = false;

#ifndef STREAM_PROCESS_ENABLE_USE_CC_CAPTURE
static POSSIBLY_UNUSED uint8_t *g_aec_echo_buf = NULL;
#endif

uint8_t *g_capture_upsampling_buf = NULL;
uint32_t g_capture_upsampling_buf_size = 0;

gaf_stream_scene_t gaf_stream_get_scene(uint16_t context)
{
#ifdef DISABLE_ALL_STREAM_PROCESS
    return GAF_STREAM_SCENE_NONE;
#endif

    if (context & GAF_STREAM_CALL_CONTEXT) {
        return GAF_STREAM_SCENE_CALL;
    }

    if ((context == GAF_STREAM_UNKNOWN_CONTEXT) &&
        (bes_ble_aob_call_if_get_any_call_by_conidx(app_audio_adm_get_active_device()->device_id, NULL) != AOB_CALL_STATE_IDLE)) {
        return GAF_STREAM_SCENE_CALL;
    }

#ifdef GAF_STREAM_LIVE_ENABLED
    if (context & GAF_STREAM_LIVE_CONTEXT) {
        return GAF_STREAM_SCENE_LIVE;
    }
#else
    if (context & GAF_STREAM_LIVE_CONTEXT) {
        return GAF_STREAM_SCENE_CALL;
    }
#endif

    if (context & GAF_STREAM_MEDIA_CONTEXT) {
        return GAF_STREAM_SCENE_MEDIA;
    }

    return GAF_STREAM_SCENE_NONE;
}

bool gaf_stream_process_context_is_call(uint16_t context)
{
    return gaf_stream_get_scene(context) == GAF_STREAM_SCENE_CALL;
}

bool gaf_stream_process_context_is_live(uint16_t context)
{
    return gaf_stream_get_scene(context) == GAF_STREAM_SCENE_LIVE;
}

bool gaf_stream_process_context_is_media(uint16_t context)
{
    return gaf_stream_get_scene(context) == GAF_STREAM_SCENE_MEDIA;
}

extern "C" bool is_le_call_mode(void)
{
#ifdef STREAM_PROCESS_ENABLE_USE_CC_CAPTURE
    // NOTE: Capture algo is running on M55, so don't need to to do any algo on M33.
    return gaf_stream_process_context_is_call(g_playback_stream);
#else
    return gaf_stream_process_context_is_call(g_capture_stream);
#endif
}

extern "C" bool is_le_media_mode(void)
{
    return gaf_stream_process_context_is_media(g_playback_stream);
}

/************************************************************
 * Declaration
 ************************************************************/
// Call Scene
static uint32_t gaf_stream_capture_call_get_size(void);
static void gaf_stream_capture_call_open(struct AF_STREAM_CONFIG_T *stream_cfg);
static void gaf_stream_capture_call_close(void);
static uint32_t gaf_stream_capture_call_process(uint8_t *buf, uint32_t len);

static uint32_t gaf_stream_playback_call_get_size(void);
static void gaf_stream_playback_call_open(struct AF_STREAM_CONFIG_T *stream_cfg);
static void gaf_stream_playback_call_close(void);
static uint32_t gaf_stream_playback_call_process(uint8_t *buf, uint32_t len);

// Live Scene
static uint32_t gaf_stream_capture_live_get_size(void);
static void gaf_stream_capture_live_open(struct AF_STREAM_CONFIG_T *stream_cfg);
static void gaf_stream_capture_live_close(void);
static uint32_t gaf_stream_capture_live_process(uint8_t *buf, uint32_t len);

// Media Scene
static uint32_t gaf_stream_playback_media_get_size(void);
static void gaf_stream_playback_media_open(struct AF_STREAM_CONFIG_T *stream_cfg);
static void gaf_stream_playback_media_close(void);
static uint32_t gaf_stream_playback_media_process(uint8_t *buf, uint32_t len);

int32_t gaf_stream_process_init(void)
{
    TRACE(1, "[%s]", __func__);
    g_capture_stream_buf = NULL;
    g_capture_stream_buf_size = 0;
    g_playback_stream_buf = NULL;
    g_playback_stream_buf_size = 0;

    audio_process_reset_audio_buffer();

    return 0;
}

int32_t gaf_stream_process_deinit(void)
{
    return 0;
}

uint32_t gaf_stream_process_capture_buf_init(uint16_t stream)
{
    uint32_t size = 0;

    switch (gaf_stream_get_scene(stream)) {
    case GAF_STREAM_SCENE_CALL:
        size = gaf_stream_capture_call_get_size();
        break;
    case GAF_STREAM_SCENE_LIVE:
        size = gaf_stream_capture_live_get_size();
        break;
    default:
        break;
    }

    TRACE(3, "[%s] stream:0x%x, size: %d", __func__, stream, size);

    if (g_capture_stream_buf == NULL) {
        g_capture_stream_buf = (uint8_t *)app_audio_mempool_calloc(size, sizeof(int8_t));
        g_capture_stream_buf_size = size;
    } else if (g_capture_stream_buf_size != size) {
        TRACE(3, "[%s] warning: old_size(%d) != new_size(%d)", __func__, g_capture_stream_buf_size, size);
    }

    return size;
}

int32_t gaf_stream_process_set_capture_channel_num(uint32_t channel_num)
{
    g_capture_channel_num = channel_num;

    return 0;
}

uint32_t get_gaf_stream_capture_sample_rate(uint16_t stream, uint32_t sample_rate)
{
    uint32_t cap_sample_rate = sample_rate;

    switch (gaf_stream_get_scene(stream)) {
#ifdef LEA_CALL_FIX_ADC_SAMPLE_RATE
    case GAF_STREAM_SCENE_CALL:
        if (sample_rate % 16000 == 0) {
            cap_sample_rate = 16000;
        }
        break;
#endif
    default:
        break;
    }

    if ((cap_sample_rate > sample_rate) && (cap_sample_rate % sample_rate != 0)) {
        return sample_rate;
    } else if ((cap_sample_rate < sample_rate) && (sample_rate % cap_sample_rate != 0)) {
        return sample_rate;
    }

    return cap_sample_rate;
}

int32_t gaf_stream_process_capture_open(uint16_t stream, struct AF_STREAM_CONFIG_T *stream_cfg)
{
    TRACE(2, "[%s] stream: 0x%x", __func__, stream);
    TRACE(4, "[%s] sample_rate:%d, bits:%d, ch_num:%d", __func__,
        stream_cfg->sample_rate,
        stream_cfg->bits,
        stream_cfg->channel_num);

    memcpy(&g_capture_stream_cfg, stream_cfg, sizeof(struct AF_STREAM_CONFIG_T));
    g_capture_stream = stream;

    switch (gaf_stream_get_scene(g_capture_stream)) {
    case GAF_STREAM_SCENE_CALL:
        g_capture_stream = BES_BLE_GAF_CONTEXT_TYPE_CONVERSATIONAL_BIT;
        gaf_stream_capture_call_open(stream_cfg);
        break;
    case GAF_STREAM_SCENE_LIVE:
        g_capture_stream = BES_BLE_GAF_CONTEXT_TYPE_LIVE_BIT;
        gaf_stream_capture_live_open(stream_cfg);
        break;
    default:
        TRACE(2, "[%s] 0x%x don't define algorithm", __func__, g_capture_stream);
        break;
    }

#ifdef GAF_STREAM_CAPTURE_AUDIO_DUMP
    uint32_t frame_len = stream_cfg->data_size / stream_cfg->channel_num / (stream_cfg->bits <= AUD_BITS_16 ? 2 : 4) / 2;
    audio_dump_init(frame_len, sizeof(int32_t), 1);
#endif

#ifdef GAF_STREAM_CAPTURE_1K_TONE
    signal_generator_init(SG_TYPE_TONE_SIN_1K, stream_cfg->sample_rate, stream_cfg->bits, 1);
#endif

#if defined(ENABLE_CALL_TX_STARTUP_FADEIN) && !defined(STREAM_PROCESS_ENABLE_USE_CC_CAPTURE)
    RampConfig ramp_cfg = {0};
    ramp_cfg.sample_rate = stream_cfg->sample_rate;
    ramp_cfg.bit_depth = stream_cfg->bits;
    ramp_cfg.num_channels = 1;
    ramp_cfg.ramp_time_ms = 30;
    ramp_cfg.initial_gain = 0;
    ramp_cfg.type = RAMP_SMOOTHSTEP;
    ramp_cfg.silence_time_ms = 200;

    g_gaf_tx_ramp_handle = audio_ramp_open(&ramp_cfg);

    TRACE(0, "%s, tx_ramp:%d", __func__, g_gaf_tx_ramp_handle);
    audio_ramp_set_target(g_gaf_tx_ramp_handle, 1.0);
#endif

    return 0;
}

int32_t gaf_stream_process_capture_close(void)
{
    TRACE(2, "[%s] stream: %d", __func__, g_capture_stream);

#ifdef GAF_STREAM_CAPTURE_1K_TONE
    signal_generator_deinit();
#endif

#if defined(ENABLE_CALL_TX_STARTUP_FADEIN) && !defined(STREAM_PROCESS_ENABLE_USE_CC_CAPTURE)
    audio_ramp_close(g_gaf_tx_ramp_handle);
    g_gaf_tx_ramp_handle = 0;
#endif

    switch (gaf_stream_get_scene(g_capture_stream)) {
    case GAF_STREAM_SCENE_CALL:
        gaf_stream_capture_call_close();
        break;
    case GAF_STREAM_SCENE_LIVE:
        gaf_stream_capture_live_close();
        break;
    default:
        TRACE(2, "[%s] 0x%x don't define algorithm", __func__, g_capture_stream);
        break;
    }

    g_capture_stream = BES_BLE_GAF_CONTEXT_TYPE_UNSPECIFIED_BIT;
    g_capture_channel_num = 0;

    return 0;
}

template <typename T>
static int gaf_stream_process_capture_data_extraction(T *pcm_buf, uint32_t pcm_len, uint32_t channel_num)
{
    pcm_len = pcm_len / channel_num;
    for (uint32_t i = 0; i < pcm_len; i++) {
        pcm_buf[i] = pcm_buf[i * channel_num];
    }

    return pcm_len;
}

uint32_t gaf_stream_process_capture_run(uint8_t *buf, uint32_t len)
{
    uint32_t POSSIBLY_UNUSED sample_bytes = g_capture_stream_cfg.bits <= AUD_BITS_16 ? 2 : 4;
    uint32_t POSSIBLY_UNUSED pcm_len = len / sample_bytes;

    switch (gaf_stream_get_scene(g_capture_stream)) {
    case GAF_STREAM_SCENE_CALL:
        len = gaf_stream_capture_call_process(buf, len);
        break;
    case GAF_STREAM_SCENE_LIVE:
        len = gaf_stream_capture_live_process(buf, len);
        break;
    default:
#ifndef STREAM_PROCESS_ENABLE_USE_CC_CAPTURE
        pcm_len = gaf_stream_process_capture_data_extraction(buf, pcm_len, g_capture_channel_num);
        len = pcm_len * sample_bytes;
#endif
        // Add 18 dB gain for other casse
        if (g_capture_stream_cfg.bits == AUD_BITS_16) {
            int16_t *pcm_buf = (int16_t *)buf;
            for (uint32_t i=0; i<pcm_len; i++) {
                pcm_buf[i] = __SSAT(((int32_t)(pcm_buf[i])) << 3, 16);
            }
        } else if (g_capture_stream_cfg.bits == 24) {
            int32_t *pcm_buf = (int32_t *)buf;
            for (uint32_t i=0; i<pcm_len; i++) {
                pcm_buf[i] = __SSAT(pcm_buf[i] << 3, 24);
            }
        } else {
            ASSERT(0, "[%s] bits(%d) is invalid", __func__, g_capture_stream_cfg.bits);
        }
    }

#if defined(ENABLE_CALL_TX_STARTUP_FADEIN) && !defined(STREAM_PROCESS_ENABLE_USE_CC_CAPTURE)
    audio_ramp_process(g_gaf_tx_ramp_handle, buf, buf, len, true);
#endif

#ifdef GAF_STREAM_CAPTURE_1K_TONE
    signal_generator_loop_get_data(buf, pcm_len);
#endif

#ifdef GAF_STREAM_CAPTURE_AUDIO_DUMP
    audio_dump_add_channel_data(0, buf, pcm_len);
    audio_dump_run();
#endif

    return len;
}

uint32_t gaf_stream_process_playback_buf_init(uint16_t stream)
{
    uint32_t size = 0;

    switch (gaf_stream_get_scene(stream)) {
    case GAF_STREAM_SCENE_CALL:
        size = gaf_stream_playback_call_get_size();
        break;
    case GAF_STREAM_SCENE_MEDIA:
        size = gaf_stream_playback_media_get_size();
        break;
    default:
        break;
    }

    TRACE(3, "[%s] stream:0x%x, size: %d", __func__, stream, size);

    if (g_playback_stream_buf == NULL) {
        g_playback_stream_buf = (uint8_t *)app_audio_mempool_calloc(size, sizeof(int8_t));
        g_playback_stream_buf_size = size;
    } else if (g_playback_stream_buf_size != size) {
        TRACE(3, "[%s] warning: old_size(%d) != new_size(%d)", __func__, g_playback_stream_buf_size, size);
    }

    return size;
}

uint32_t get_gaf_stream_playback_sample_rate(uint16_t stream, uint32_t sample_rate)
{
    uint32_t play_sample_rate = sample_rate;

    switch (gaf_stream_get_scene(stream)) {
#ifdef LEA_CALL_FIX_DAC_SAMPLE_RATE
    case GAF_STREAM_SCENE_CALL:
        if (sample_rate % 16000 == 0) {
            play_sample_rate = 16000;
        }
        break;
#endif
    default:
        break;
    }

    if ((play_sample_rate > sample_rate) && (play_sample_rate % sample_rate != 0)) {
        return sample_rate;
    } else if ((play_sample_rate < sample_rate) && (sample_rate % play_sample_rate != 0)) {
        return sample_rate;
    }

    return play_sample_rate;
}

int32_t gaf_stream_process_playback_open(uint16_t stream, struct AF_STREAM_CONFIG_T *stream_cfg)
{
    TRACE(2, "[%s] stream: 0x%x", __func__, stream);
    TRACE(4, "[%s] sample_rate:%d, bits:%d, ch_num:%d", __func__,
        stream_cfg->sample_rate,
        stream_cfg->bits,
        stream_cfg->channel_num);

    memcpy(&g_playback_stream_cfg, stream_cfg, sizeof(struct AF_STREAM_CONFIG_T));
    g_playback_stream = stream;
    g_playback_channel_num = stream_cfg->channel_num;

    switch (gaf_stream_get_scene(g_playback_stream)) {
    case GAF_STREAM_SCENE_CALL:
        g_playback_stream = BES_BLE_GAF_CONTEXT_TYPE_CONVERSATIONAL_BIT;
        gaf_stream_playback_call_open(stream_cfg);
        break;
    case GAF_STREAM_SCENE_MEDIA:
    default:
        g_playback_stream = BES_BLE_GAF_CONTEXT_TYPE_MEDIA_BIT;
        gaf_stream_playback_media_open(stream_cfg);
        break;
    }

    return 0;
}

int32_t gaf_stream_process_playback_close(void)
{
    TRACE(2, "[%s] stream: %d", __func__, g_playback_stream);

    switch (gaf_stream_get_scene(g_playback_stream)) {
    case GAF_STREAM_SCENE_CALL:
        gaf_stream_playback_call_close();
        break;
    case GAF_STREAM_SCENE_MEDIA:
    default:
        gaf_stream_playback_media_close();
        break;
    }

    g_playback_stream = BES_BLE_GAF_CONTEXT_TYPE_UNSPECIFIED_BIT;
    g_playback_channel_num = 0;

    return 0;
}

uint32_t gaf_stream_process_playback_run(uint8_t *buf, uint32_t len)
{
    switch (gaf_stream_get_scene(g_playback_stream)) {
    case GAF_STREAM_SCENE_CALL:
        len = gaf_stream_playback_call_process(buf, len);
        break;
    case GAF_STREAM_SCENE_MEDIA:
    default:
        len = gaf_stream_playback_media_process(buf, len);
        break;
    }

    return len;
}

/************************************************************
 * Call Scene Implementation
 ************************************************************/
WEAK int speech_get_lea_algo_ram_usage(void)
{
    TRACE(1, "[%s] FIXME: Check Call tx algo RAM usage", __func__);
    return 1024 * 60;
}
static uint32_t gaf_stream_capture_call_get_size(void)
{
    return speech_get_lea_algo_ram_usage();
}

static void gaf_stream_capture_call_open(struct AF_STREAM_CONFIG_T *stream_cfg)
{
#ifndef STREAM_PROCESS_ENABLE_USE_CC_CAPTURE
#if defined(SPEECH_ALGO_DSP)
#ifdef MCPP_VERSION_STRING
    bt_sco_chain_mcpp_enable(true);
    bt_sco_chain_mcpp_pre_open();
#else
    speech_enable_mcpp(true);
#endif
#endif
    uint32_t frame_len = stream_cfg->data_size / stream_cfg->channel_num / (stream_cfg->bits <= AUD_BITS_16 ? 2 : 4) / 2;
#ifdef GAF_CONVERSATIONAL_STREAM_PROCESS_ENABLE
    if (g_playback_stream_cfg.sample_rate != 0){
        uint32_t rx_frame_len = g_playback_stream_cfg.data_size / g_playback_stream_cfg.channel_num / (g_playback_stream_cfg.bits <= AUD_BITS_16 ? 2 : 4) / 2;
        bt_sco_chain_lea_reframe_open(stream_cfg->sample_rate, g_playback_stream_cfg.sample_rate, frame_len, rx_frame_len, frame_len, g_capture_stream_buf, g_capture_stream_buf_size);
    } else {
        bt_sco_chain_lea_reframe_open(stream_cfg->sample_rate, stream_cfg->sample_rate, frame_len, frame_len, frame_len, g_capture_stream_buf, g_capture_stream_buf_size);
    }
#else
    if (g_playback_stream_cfg.sample_rate != 0){
        uint32_t rx_frame_len = g_playback_stream_cfg.data_size / g_playback_stream_cfg.channel_num / (g_playback_stream_cfg.bits <= AUD_BITS_16 ? 2 : 4) / 2;
        speech_init2(stream_cfg->sample_rate, g_playback_stream_cfg.sample_rate, frame_len, rx_frame_len, frame_len, g_capture_stream_buf, g_capture_stream_buf_size);
    } else {
        speech_init2(stream_cfg->sample_rate, stream_cfg->sample_rate, frame_len, frame_len, frame_len, g_capture_stream_buf, g_capture_stream_buf_size);
    }
#endif
    g_aec_echo_buf = (uint8_t *)speech_calloc(frame_len, (stream_cfg->bits <= AUD_BITS_16 ? 2 : 4));
    g_call_algo_inited = true;
#endif
}

static void gaf_stream_capture_call_close(void)
{
#ifndef STREAM_PROCESS_ENABLE_USE_CC_CAPTURE
    g_call_algo_inited = false;
    speech_free(g_aec_echo_buf);
#ifdef GAF_CONVERSATIONAL_STREAM_PROCESS_ENABLE
    bt_sco_chain_lea_reframe_close();
#else
    speech_deinit();
#endif
#endif
}

template <typename T>
static void gaf_stream_split_capture_data(T *pcm_buf, T *echo_buf, uint32_t pcm_len)
{
    static_assert(SPEECH_CODEC_CAPTURE_CHANNEL_NUM >= 1, "Invalid channel num");

    T *src = pcm_buf;
    T *dst = pcm_buf;

    constexpr uint32_t CH = SPEECH_CODEC_CAPTURE_CHANNEL_NUM;
    constexpr bool HAS_ECHO =
#if defined(SPEECH_TX_AEC_CODEC_REF)
        true;
#else
        false;
#endif

    uint32_t frame_cnt = pcm_len / (CH + (HAS_ECHO ? 1 : 0));
    while (frame_cnt--) {
        for (uint32_t ch = 0; ch < CH; ch++) {
            *dst++ = *src++;
        }

        if (HAS_ECHO) {
            *echo_buf++ = *src++;
        }
    }
}

static uint32_t gaf_stream_capture_call_process(uint8_t *buf, uint32_t len)
{
    uint32_t POSSIBLY_UNUSED sample_bytes = g_capture_stream_cfg.bits <= AUD_BITS_16 ? 2 : 4;
    uint32_t POSSIBLY_UNUSED pcm_len = len / sample_bytes;

#ifndef STREAM_PROCESS_ENABLE_USE_CC_CAPTURE
#ifdef SPEECH_TX_AEC_CODEC_REF
        ASSERT(pcm_len % (SPEECH_CODEC_CAPTURE_CHANNEL_NUM + 1) == 0, "[%s] pcm_len(%d) should be divided by %d", __FUNCTION__, pcm_len, SPEECH_CODEC_CAPTURE_CHANNEL_NUM + 1);
        if (g_capture_stream_cfg.bits == AUD_BITS_16){
            gaf_stream_split_capture_data((int16_t *)buf, (int16_t *)g_aec_echo_buf, pcm_len);
        } else if (g_capture_stream_cfg.bits == AUD_BITS_24){
            gaf_stream_split_capture_data((int32_t *)buf, (int32_t *)g_aec_echo_buf, pcm_len);
        }
        pcm_len = pcm_len / (SPEECH_CODEC_CAPTURE_CHANNEL_NUM + 1) * SPEECH_CODEC_CAPTURE_CHANNEL_NUM;
#endif
#ifdef GAF_CONVERSATIONAL_STREAM_PROCESS_ENABLE
        if (g_call_algo_inited) {
            bt_sco_chain_lea_reframe_tx_process(buf, g_aec_echo_buf, (int32_t *)&pcm_len);
            len = pcm_len * sample_bytes;
        }
#else
        if (g_call_algo_inited) {
            speech_tx_process(buf, g_aec_echo_buf, (int32_t *)&pcm_len);
            len = pcm_len * sample_bytes;
        }
#endif
#endif

    return len;
}

static uint32_t gaf_stream_playback_call_get_size(void)
{
#ifdef STREAM_PROCESS_ENABLE_USE_CC_CAPTURE
    /**
     * RX Algo:
     *  16k sample rate: 15k RAM
     *  32k sample rate: 25k RAM
     *  48k sample rate: 42k RAM, BLE ringtone state, only RX.
     */
    TRACE(1, "[%s] FIXME: Check Call rx algo RAM usage", __func__);
    return 1024 * 50;
#else
    return 0;
#endif
}

static void gaf_stream_playback_call_open(struct AF_STREAM_CONFIG_T *stream_cfg)
{
#ifdef STREAM_PROCESS_ENABLE_USE_CC_CAPTURE
    uint32_t frame_len = stream_cfg->data_size / stream_cfg->channel_num / (stream_cfg->bits <= AUD_BITS_16 ? 2 : 4) / 2;
    speech_init2(stream_cfg->sample_rate, stream_cfg->sample_rate, frame_len, frame_len, frame_len, g_playback_stream_buf, g_playback_stream_buf_size);
    g_call_algo_inited = true;
#endif
}

static void gaf_stream_playback_call_close(void)
{
#ifdef STREAM_PROCESS_ENABLE_USE_CC_CAPTURE
    g_call_algo_inited = false;
    speech_deinit();
#endif
}

static uint32_t gaf_stream_playback_call_process(uint8_t *buf, uint32_t len)
{
    uint32_t sample_bytes = g_playback_stream_cfg.bits <= AUD_BITS_16 ? 2 : 4;
    uint32_t pcm_len = len / sample_bytes;

    if (g_playback_channel_num > 1) {
        pcm_len = pcm_len / g_playback_channel_num;
        if (sample_bytes == 2) {
            int16_t *pcm16 = (int16_t *)buf;
            for (uint32_t i = 0; i < pcm_len; i++) {
                int32_t cache_data = 0;
                for (uint32_t ch = 0; ch < g_playback_channel_num; ch++) {
                    cache_data += (pcm16[i * g_playback_channel_num + ch] / g_playback_channel_num);
                }
                pcm16[i] = cache_data;
            }
        } else if (sample_bytes == 4) {
            int32_t *pcm32 = (int32_t *)buf;
            for (uint32_t i = 0; i < pcm_len; i++) {
                int32_t cache_data = 0;
                for (uint32_t ch = 0; ch < g_playback_channel_num; ch++) {
                    cache_data += (pcm32[i * g_playback_channel_num + ch] / g_playback_channel_num);
                }
                pcm32[i] = cache_data;
            }
        }
    }

#ifdef GAF_CONVERSATIONAL_STREAM_PROCESS_ENABLE
    if (g_call_algo_inited) {
        bt_sco_chain_lea_reframe_rx_process(buf, (int32_t *)&pcm_len);
    }
#else
    if (g_call_algo_inited) {
        speech_rx_process(buf, (int32_t *)&pcm_len);
    }
#endif

    if (g_playback_channel_num > 1) {
        if (sample_bytes == 2) {
            int16_t *pcm16 = (int16_t *)buf;
            for (int32_t i = pcm_len - 1; i >= 0; i--) {
                for (uint32_t ch = 0; ch < g_playback_channel_num; ch++) {
                    pcm16[i * g_playback_channel_num + ch] = pcm16[i];
                }
            }
        } else if (sample_bytes == 4) {
            int32_t *pcm32 = (int32_t *)buf;
            for (int32_t i = pcm_len - 1; i >= 0; i--) {
                for (uint32_t ch = 0; ch < g_playback_channel_num; ch++) {
                    pcm32[i * g_playback_channel_num + ch] = pcm32[i];
                }
            }
        }
        pcm_len = pcm_len * g_playback_channel_num;
    }

    len = pcm_len * sample_bytes;

    return len;
}

/************************************************************
 * Live Scene Implementation
 ************************************************************/
static uint32_t gaf_stream_capture_live_get_size(void)
{
    return 1024 * 50;
}

static void gaf_stream_capture_live_open(struct AF_STREAM_CONFIG_T *stream_cfg)
{
#ifdef BINAURAL_RECORD_PROCESS
    TRACE(1, "[%s] Binaural Recording...", __func__);
    uint32_t sample_rate = stream_cfg->sample_rate;
    uint32_t frame_len = stream_cfg->data_size / stream_cfg->channel_num / (stream_cfg->bits <= AUD_BITS_16 ? 2 : 4) / 2;
#ifdef BINAURAL_RECORD_ADC_HIGH_SAMPLE_RATE
    extern uint32_t gaf_stream_get_real_capture_sample_rate(void);
    sample_rate = gaf_stream_get_real_capture_sample_rate();
#endif
    binaural_record_process_init(sample_rate, stream_cfg->bits, stream_cfg->channel_num, frame_len, g_capture_stream_buf, g_capture_stream_buf_size);
#endif
}

static void gaf_stream_capture_live_close(void)
{
#ifdef BINAURAL_RECORD_PROCESS
    TRACE(1, "[%s] Binaural Recording...", __func__);
    binaural_record_process_deinit();
#endif
}

static uint32_t gaf_stream_capture_live_process(uint8_t *buf, uint32_t len)
{
#ifdef BINAURAL_RECORD_PROCESS
    uint32_t sample_bytes = g_capture_stream_cfg.bits <= AUD_BITS_16 ? 2 : 4;
    uint32_t pcm_len = len / sample_bytes;
    binaural_record_process_run(buf, pcm_len);
#endif

    return len;
}

/************************************************************
 * Media Scene Implementation
 ************************************************************/
static uint32_t gaf_stream_playback_media_get_size(void)
{
    return audio_process_need_audio_buf_size();
}

static void gaf_stream_playback_media_open(struct AF_STREAM_CONFIG_T *stream_cfg)
{
    uint32_t frame_len = stream_cfg->data_size / stream_cfg->channel_num / (stream_cfg->bits <= AUD_BITS_16 ? 2 : 4) / 2;
    audio_process_open(stream_cfg->sample_rate,
        stream_cfg->bits, stream_cfg->channel_num, stream_cfg->channel_num, frame_len, g_playback_stream_buf, g_playback_stream_buf_size);
}

static void gaf_stream_playback_media_close(void)
{
    audio_process_close();
}

static uint32_t gaf_stream_playback_media_process(uint8_t *buf, uint32_t len)
{
    audio_process_run(buf, len);

    return len;
}
