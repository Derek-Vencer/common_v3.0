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
#include "gaf_stream_resample_process.h"
#include "gaf_stream_process.h"
#include "iir_resample.h"
#include "custom_allocator.h"
#include "heap_api.h"
#include "audio_dump.h"
#include "hal_trace.h"
#include "app_audio.h"

// #define GAF_CAPTURE_UPSAMPLING_DUMP
// #define GAF_PLAYBACK_DOWNSAMPLING_DUMP

/**********capture**********/
static POSSIBLY_UNUSED uint32_t g_capture_upsample_factor = 1;
static POSSIBLY_UNUSED IirResampleState *g_capture_upsample_st = NULL;
static uint8_t *g_capture_upsample_buf = NULL;
static POSSIBLY_UNUSED uint32_t g_capture_upsample_buf_size = 0;
static POSSIBLY_UNUSED AUD_BITS_T g_capture_bits = AUD_BITS_NULL;

static heap_handle_t g_cap_resample_heap = NULL;
static uint8_t *g_cap_resample_heap_buf = NULL;
static POSSIBLY_UNUSED uint32_t g_cap_resample_heap_size = 0;

/**********playback**********/
static uint32_t g_playback_downsample_factor = 1;
static IirResampleState *g_playback_downsample_st = NULL;
uint8_t *g_playback_downsample_buf = NULL;
uint32_t g_playback_downsample_buf_size = 0;
static AUD_BITS_T g_playback_bits = AUD_BITS_NULL;
static uint8_t *g_remap_downsampling_buf = NULL;

static heap_handle_t g_play_resample_heap = NULL;
static uint8_t *g_play_resample_heap_buf = NULL;
static uint32_t g_play_resample_heap_size = 0;

/***********************common***********************/
void gaf_stream_resample_process_init(void)
{
    TRACE(1, "[%s]", __func__);
    g_capture_upsample_buf = NULL;
    g_cap_resample_heap_buf = NULL;
    g_cap_resample_heap = NULL;
    g_playback_downsample_buf = NULL;
    g_play_resample_heap_buf = NULL;
    g_play_resample_heap = NULL;
}

/***********************capture upsampling***********************/
static void *_cap_resample_malloc(size_t size)
{
    if (size == 0)
        return NULL;

    void *ptr = heap_malloc(g_cap_resample_heap, size);
    ASSERT(ptr != NULL, "[%s] no memory: size=%u", __FUNCTION__, size);

    return ptr;
}

static void *_cap_resample_calloc(size_t nmemb, size_t size)
{
    if (size == 0)
        return NULL;

    void *ptr = _cap_resample_malloc(nmemb * size);
    if (ptr) {
        memset(ptr, 0 , nmemb * size);
    }

    return ptr;
}

static void _cap_resample_free(void *p)
{
    if (p) {
        heap_free(g_cap_resample_heap, p);
        p = NULL;
    }
}

static POSSIBLY_UNUSED custom_allocator cap_resample_allocator = {
    .malloc = _cap_resample_malloc,
    .calloc = _cap_resample_calloc,
    .free   = _cap_resample_free,
};

int32_t gaf_stream_resample_capture_up_init(GAF_AUDIO_STREAM_INFO_T *stream_info, struct AF_STREAM_CONFIG_T *stream_cfg)
{
#ifndef STREAM_PROCESS_ENABLE_USE_CC_CAPTURE
    uint32_t frame_len = stream_cfg->data_size / stream_cfg->channel_num / (stream_cfg->bits <= AUD_BITS_16 ? 2 : 4) / 2;
    g_capture_bits = stream_cfg->bits;

    int32_t codec_sample_rate = (int32_t)stream_info->captureInfo.codec_info.sample_rate;
    int32_t stream_sample_rate = (int32_t)stream_cfg->sample_rate;
    int channel_num = stream_info->captureInfo.codec_info.num_channels;

    if (codec_sample_rate != stream_sample_rate) {
        ASSERT(codec_sample_rate % stream_sample_rate == 0, "[%s] %d, %d. Can not do resample", __func__, codec_sample_rate, stream_sample_rate);
        g_capture_upsample_factor = codec_sample_rate / stream_sample_rate;
        TRACE(4, "[%s] Resample %d--> %d. factor: %d", __func__, stream_sample_rate, codec_sample_rate, g_capture_upsample_factor);

        if (stream_cfg->channel_num < g_capture_upsample_factor * channel_num){
            TRACE(4, "[%s] stream ch_num:%d, factor:%d, codec ch_num:%d",  __func__, stream_cfg->channel_num, g_capture_upsample_factor, channel_num);
            g_capture_upsample_buf_size = frame_len * g_capture_upsample_factor * channel_num * (stream_cfg->bits <= AUD_BITS_16 ? 2 : 4);
        } else {
            g_capture_upsample_buf_size = 0;
        }

        if (g_capture_upsample_buf_size && g_capture_upsample_buf == NULL) {
            g_capture_upsample_buf = (uint8_t *)app_audio_mempool_calloc(g_capture_upsample_buf_size, sizeof(int8_t));
        } else if (g_capture_upsample_buf != NULL) {
            TRACE(1, "[%s] g_capture_upsample_buf not NULL", __func__);
        }

        if (g_cap_resample_heap == NULL && g_cap_resample_heap_buf == NULL) {
            g_cap_resample_heap_size = frame_len * channel_num * g_capture_upsample_factor * sizeof(float) + 1024;
            g_cap_resample_heap_buf = (uint8_t *)app_audio_mempool_calloc(g_cap_resample_heap_size, sizeof(int8_t));
            g_cap_resample_heap = heap_register(g_cap_resample_heap_buf, g_cap_resample_heap_size);
        }

        g_capture_upsample_st = multi_iir_resample_init_with_custom_allocator(frame_len * channel_num,
                                                        stream_cfg->bits,
                                                        channel_num,
                                                        iir_resample_choose_mode(stream_sample_rate, codec_sample_rate),
                                                        &cap_resample_allocator);
    } else {
        g_capture_upsample_st = NULL;
        g_capture_upsample_factor = 1;
        g_capture_upsample_buf_size = 0;
    }

#ifdef GAF_CAPTURE_UPSAMPLING_DUMP
    data_dump_init();
#endif
#endif

    return 0;
}

int32_t gaf_stream_resample_capture_up_deinit(void)
{
#ifndef STREAM_PROCESS_ENABLE_USE_CC_CAPTURE
    if (g_capture_upsample_st) {
        iir_resample_destroy(g_capture_upsample_st);
        g_capture_upsample_st = NULL;
        g_capture_upsample_factor = 1;
        g_capture_upsample_buf_size = 0;
    }

#ifdef GAF_CAPTURE_UPSAMPLING_DUMP
    data_dump_deinit();
#endif
#endif
    return 0;
}

uint32_t gaf_stream_resample_capture_up_run(uint8_t **buf, uint32_t len)
{
#ifndef STREAM_PROCESS_ENABLE_USE_CC_CAPTURE
    if (g_capture_upsample_st) {
#ifdef GAF_CAPTURE_UPSAMPLING_DUMP
        data_dump_run(0, "tx_algo_16k", *buf, len);
#endif

        uint32_t POSSIBLY_UNUSED pcm_len = len / (g_capture_bits <= AUD_BITS_16 ? 2 : 4);
        uint8_t *pcm_buf = *buf;
        if (g_capture_upsample_buf){
            iir_resample_process(g_capture_upsample_st, pcm_buf, g_capture_upsample_buf, pcm_len);
            *buf = g_capture_upsample_buf;
        } else {
            iir_resample_process(g_capture_upsample_st, pcm_buf, pcm_buf, pcm_len);
        }
        len *= g_capture_upsample_factor;

#ifdef GAF_CAPTURE_UPSAMPLING_DUMP
        data_dump_run(1, "tx_algo_32k", *buf, len);
#endif
    }
#endif

    return len;
}

/***********************playback downsampling***********************/
static void *_play_resample_malloc(size_t size)
{
    if (size == 0)
        return NULL;

    void *ptr = heap_malloc(g_play_resample_heap, size);
    ASSERT(ptr != NULL, "[%s] no memory: size=%u", __FUNCTION__, size);

    return ptr;
}

static void *_play_resample_calloc(size_t nmemb, size_t size)
{
    if (size == 0)
        return NULL;

    void *ptr = _play_resample_malloc(nmemb * size);
    if (ptr) {
        memset(ptr, 0 , nmemb * size);
    }

    return ptr;
}

static void _play_resample_free(void *p)
{
    if (p) {
        heap_free(g_play_resample_heap, p);
        p = NULL;
    }
}

static custom_allocator play_resample_allocator = {
    .malloc = _play_resample_malloc,
    .calloc = _play_resample_calloc,
    .free   = _play_resample_free,
};

uint32_t gaf_stream_resample_need_dn_buf_size(void)
{
    return g_playback_downsample_buf_size;
}

int32_t gaf_stream_resample_remap_dn_ptrbuf_length(uint8_t** ptrBuf, uint32_t *length)
{
    if (g_playback_downsample_buf != NULL) {
        g_remap_downsampling_buf = *ptrBuf;
        *ptrBuf = g_playback_downsample_buf;
        *length = *length * g_playback_downsample_factor;
    }

    return 0;
}

uint8_t* gaf_stream_resample_get_dn_ptrbuf(void)
{
    return g_remap_downsampling_buf;
}

int32_t gaf_stream_resample_playback_dn_init(GAF_AUDIO_STREAM_INFO_T *stream_info, struct AF_STREAM_CONFIG_T *stream_cfg)
{
    uint32_t frame_len = stream_cfg->data_size / stream_cfg->channel_num / (stream_cfg->bits <= AUD_BITS_16 ? 2 : 4) / 2;
    g_playback_bits = stream_cfg->bits;
    int32_t codec_sample_rate = (int32_t)stream_info->playbackInfo.codec_info.sample_rate;
    int32_t stream_sample_rate = (int32_t)stream_cfg->sample_rate;

    if (codec_sample_rate != stream_sample_rate) {
        ASSERT(codec_sample_rate % stream_sample_rate == 0, "[%s] %d, %d. Can not do resample", __func__, codec_sample_rate, stream_sample_rate);
        g_playback_downsample_factor = codec_sample_rate / stream_sample_rate;
        TRACE(4, "[%s] Resample %d--> %d. factor: %d. ch_num: %d", __func__, codec_sample_rate, stream_sample_rate, g_playback_downsample_factor, stream_cfg->channel_num);
        g_playback_downsample_buf_size = frame_len * g_playback_downsample_factor * stream_cfg->channel_num * (stream_cfg->bits <= AUD_BITS_16 ? 2 : 4);

        if (g_playback_downsample_buf == NULL) {
            g_playback_downsample_buf = (uint8_t *)app_audio_mempool_calloc(g_playback_downsample_buf_size, sizeof(int8_t));
        } else {
            TRACE(1, "[%s] g_playback_downsample_buf not NULL", __func__);
        }

        if (g_play_resample_heap == NULL) {
            g_play_resample_heap_size = frame_len * stream_cfg->channel_num * g_playback_downsample_factor * sizeof(float) + 1024;
            g_play_resample_heap_buf = (uint8_t *)app_audio_mempool_calloc(g_play_resample_heap_size, sizeof(int8_t));
            g_play_resample_heap = heap_register(g_play_resample_heap_buf, g_play_resample_heap_size);
        }

        g_playback_downsample_st = multi_iir_resample_init_with_custom_allocator(frame_len * stream_cfg->channel_num * g_playback_downsample_factor,
                                                        stream_cfg->bits,
                                                        stream_cfg->channel_num,
                                                        iir_resample_choose_mode(codec_sample_rate, stream_sample_rate),
                                                        &play_resample_allocator);
    } else {
        g_playback_downsample_st = NULL;
        g_playback_downsample_factor = 1;
        g_playback_downsample_buf_size = 0;
        g_remap_downsampling_buf = NULL;
    }

#ifdef GAF_PLAYBACK_DOWNSAMPLING_DUMP
    data_dump_init();
#endif

    return 0;
}

int32_t gaf_stream_resample_playback_dn_deinit(void)
{
    if (g_playback_downsample_st) {
        iir_resample_destroy(g_playback_downsample_st);
        g_playback_downsample_st = NULL;
        g_playback_downsample_factor = 1;
        g_playback_downsample_buf_size = 0;
    }

    g_remap_downsampling_buf = NULL;

#ifdef GAF_PLAYBACK_DOWNSAMPLING_DUMP
    data_dump_deinit();
#endif
    return 0;
}

uint32_t gaf_stream_resample_playback_dn_run(uint8_t *buf, uint32_t len)
{
    uint32_t POSSIBLY_UNUSED sample_bytes = g_playback_bits <= AUD_BITS_16 ? 2 : 4;
    uint32_t POSSIBLY_UNUSED pcm_len = len / sample_bytes;

    if (g_playback_downsample_st) {
#ifdef GAF_PLAYBACK_DOWNSAMPLING_DUMP
        data_dump_run(0, "rx_algo_32k", buf, len);
#endif

        iir_resample_process(g_playback_downsample_st, buf, buf, pcm_len);
        len /= g_playback_downsample_factor;

#ifdef GAF_PLAYBACK_DOWNSAMPLING_DUMP
        data_dump_run(1, "rx_algo_16k", buf, len);
#endif
    }

    return len;
}
