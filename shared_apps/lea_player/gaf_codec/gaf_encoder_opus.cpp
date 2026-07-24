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
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "cmsis.h"
#include "cmsis_os.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_location.h"
#include "app_trace_rx.h"
#include "app_utils.h"
#include "plat_types.h"
#include "gaf_dbg.h"
#include "audio_dump.h"
#include "app_overlay.h"
#include "opus_api.h"
#include "gaf_codec_opus.h"
#include "gaf_codec_common.h"
#include "cc_stream_common.h"
#include "heap_api.h"

static char opus_enc_heap_pool[60 * 1024];
static multi_heap_handle_t opus_memory_heap = NULL;
static void* opus_malloc(int size)
{
    void *buf = heap_malloc(opus_memory_heap, size);
    TRACE(0, "(%s)[%d] %d,%p", __func__, __LINE__, size, buf);
    return buf;
}
static void* opus_realloc(void *rmem, int newsize)
{
    void *buf = heap_realloc(opus_memory_heap, rmem, newsize);
    TRACE(0, "(%s)[%d] %d,%p,%p", __func__, __LINE__, newsize, rmem, buf);
    return buf;
}
static void opus_free(void *rmem)
{
    heap_free(opus_memory_heap, rmem);
    TRACE(0, "(%s)[%d] %p", __func__, __LINE__, rmem);
    return;
}
static void opus_print(const char* fmt, ...)
{
    static char buf[256];
    va_list ap;
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);
    TRACE(0, "%s", buf);
}
static OpusApi_BasePort_t opusApiBasePort;
static void gaf_audio_opus_encoder_buf_init(uint8_t instance_handle,
                                        void *_codec_info, CODEC_ALLOC alloc_cb, CODEC_FREE free_cb)
{
    opus_memory_heap = heap_register(opus_enc_heap_pool,
                                    sizeof(opus_enc_heap_pool));
    opusApiBasePort.malloc_cb = opus_malloc;
    opusApiBasePort.realloc_cb = opus_realloc;
    opusApiBasePort.free_cb = opus_free;
    opusApiBasePort.print_cb = opus_print;
}

static void * opus_encoder = NULL;
static void gaf_audio_opus_encoder_init(uint8_t instance_handle, void *_codec_info)
{
    CODEC_INFO_T *codec_info = (CODEC_INFO_T*)_codec_info;
    const int frameDMs = (int)codec_info->frame_ms*10;
    const int bitrate = codec_info->frame_size/(frameDMs/10)*8000;
    const int complexity = 0;
    bool haveHead = false;
    TRACE(0, "(%s)[%d] (%d,%d,%d,%d,%d,%d)", __func__, __LINE__, codec_info->sample_rate,
                                codec_info->num_channels, (int)codec_info->frame_ms,
                                codec_info->bits_depth, codec_info->frame_size,
                                codec_info->pcm_size);
    OpusApiRet_t ret = opus_api_create_encoder(&opus_encoder, &opusApiBasePort, 
                        codec_info->sample_rate, codec_info->num_channels,
                        haveHead, OPUS_API_ENC_CHOOSE_NORMAL);
    if (ret != OPUS_API_RET_SUCCESS) {
        TRACE(0, "(%s)[%d] opus create fail, %d,(%p,%d,%d,%d)", __func__, __LINE__, ret,
            opus_encoder, codec_info->sample_rate,
            codec_info->num_channels, haveHead);
    }
    ret = opus_api_encoder_set(opus_encoder, OPUS_API_ENC_SET_BIT_RATE, (void*)bitrate);
    if (ret != OPUS_API_RET_SUCCESS){
        TRACE(0, "(%s)[%d] opus set fail, %d", __func__, __LINE__, ret);
        return ;
    }
    ret = opus_api_encoder_set(opus_encoder,
        OPUS_API_ENC_SET_FRAME_DURATION_0P1MS, (void*)frameDMs);
    if (ret != OPUS_API_RET_SUCCESS){
        TRACE(0, "(%s)[%d] opus set fail, %d", __func__, __LINE__, ret);
        return ;
    }
    ret = opus_api_encoder_set(opus_encoder, OPUS_API_ENC_SET_USE_VBR, (void*)false);
    if (ret != OPUS_API_RET_SUCCESS){
        TRACE(0, "(%s)[%d] opus set fail, %d", __func__, __LINE__, ret);
        return ;
    }
    ret = opus_api_encoder_set(opus_encoder, OPUS_API_ENC_SET_COMPLEXITY, (void*)complexity);
    if (ret != OPUS_API_RET_SUCCESS){
        TRACE(0, "(%s)[%d] opus set fail, %d", __func__, __LINE__, ret);
        return ;
    }
    LOG_I("%s end", __func__);
}

static void gaf_audio_opus_encoder_deinit(uint8_t instance_handle)
{
    LOG_I("%s instance_handle:%d", __func__, instance_handle);
    opus_api_destory_encoder(opus_encoder);
}

POSSIBLY_UNUSED static uint32_t aob_opus_get_sysfreq(void)
{
    enum HAL_CMU_FREQ_T cpu_freq = hal_sysfreq_get();

    switch(cpu_freq)
    {
        case HAL_CMU_FREQ_32K:
            return 0;
            break;
        case HAL_CMU_FREQ_26M:
            return 26;
            break;
        case HAL_CMU_FREQ_52M:
            return 52;
            break;
        case HAL_CMU_FREQ_78M:
            return 78;
            break;
        case HAL_CMU_FREQ_104M:
            return 104;
            break;
        case HAL_CMU_FREQ_208M:
            return 208;
            break;
        default:
            return 0;
            break;
    }
}

FRAM_TEXT_LOC
static int gaf_audio_opus_encode(uint8_t instance_handle, void *codec_info,
    uint32_t inputDataLength, void *input, uint16_t frame_size, uint8_t* output)
{
    TRACE(0, "%s %d %d %d", __func__, __LINE__, inputDataLength, frame_size);
    CODEC_INFO_T *info = (CODEC_INFO_T*)codec_info;
    int samples = inputDataLength/info->num_channels/2;
    int output_size = frame_size;
    OpusApiRet_t ret = opus_api_encoder_run(opus_encoder, (short *)input, samples, output, &output_size);
    return ret;
}

static void gaf_audio_opus_encoder_buf_deinit(uint8_t instance_handle)
{
    LOG_I("%s instance_handle:%d", __func__, instance_handle);
    opus_memory_heap = NULL;
}

static uint32_t gaf_audio_opus_encoder_set_freq(void *_codec_info,
                                    uint32_t base_freq, uint32_t core_type)
{
    return APP_SYSFREQ_208M;
}

static GAF_ENCODER_FUNC_LIST_T gaf_audio_opus_encoder_func_list =
{
    .encoder_init_buf_func = gaf_audio_opus_encoder_buf_init,
    .encoder_init_func = gaf_audio_opus_encoder_init,
    .encoder_deinit_func = gaf_audio_opus_encoder_deinit,
    .encoder_encode_frame_func = gaf_audio_opus_encode,
    .encoder_deinit_buf_func = gaf_audio_opus_encoder_buf_deinit,
    .encoder_set_freq = gaf_audio_opus_encoder_set_freq,
};

void gaf_audio_opus_update_encoder_func_list(void *enc_func_list)
{
    GAF_ENCODER_FUNC_LIST_T **encoder_func_list = (GAF_ENCODER_FUNC_LIST_T **)enc_func_list;
    *encoder_func_list = &gaf_audio_opus_encoder_func_list;
}

int gaf_audio_opus_encoder_get_max_frame_size(void)
{
    return OPUS_MAX_FRAME_SIZE;
}
