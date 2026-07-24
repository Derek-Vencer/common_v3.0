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
#include "app_utils.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_aud.h"
#include "hal_location.h"
#include "audio_dump.h"
#include "app_overlay.h"
#include "opus_api.h"
#include "gaf_codec_opus.h"
#include "gaf_dbg.h"
#include "gaf_codec_common.h"
#include "cc_stream_common.h"
#include "heap_api.h"

static char opus_dec_heap_pool[36 * 1024];
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
static void gaf_audio_opus_decoder_buf_init(uint8_t instance_handle,
                                    void *_codec_info, CODEC_ALLOC alloc_cb, CODEC_FREE free_cb)
{
    opus_memory_heap = heap_register(opus_dec_heap_pool,
                                    sizeof(opus_dec_heap_pool));
    opusApiBasePort.malloc_cb = opus_malloc;
    opusApiBasePort.realloc_cb = opus_realloc;
    opusApiBasePort.free_cb = opus_free;
    opusApiBasePort.print_cb = opus_print;
}

static void * opus_decoder = NULL;
static void gaf_audio_opus_decoder_init(uint8_t instance_handle, void *_codec_info)
{
    CODEC_INFO_T *codec_info = (CODEC_INFO_T*)_codec_info;
    const int frameDMs = (int)codec_info->frame_ms*10;
    const int bitrate = codec_info->frame_size/(frameDMs/10)*8000;
    bool haveHead = false;
    TRACE(0, "(%s)[%d] (%d,%d,%d,%d)", __func__, __LINE__, codec_info->sample_rate,
            codec_info->num_channels, bitrate, frameDMs);
    OpusApiRet_t ret = opus_api_create_decoder(&opus_decoder, &opusApiBasePort,
            codec_info->sample_rate, codec_info->num_channels, OPUS_API_DEC_CHOOSE_NORMAL);
    if (ret != OPUS_API_RET_SUCCESS) {
        TRACE(0, "opus create fail, %d,(%p,%d,%d,%d)", ret, opus_decoder,
            codec_info->sample_rate, codec_info->num_channels, haveHead);
    }
    LOG_I("%s end", __func__);
}

static void gaf_audio_opus_decoder_deinit(uint8_t instance_handle)
{
    LOG_I("%s", __func__);
    opus_api_destory_decoder(opus_decoder);
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
static int gaf_audio_opus_decode(uint8_t instance_handle, void *codec_info,
            uint32_t inputDataLength, void *input, void *output, bool isPlc)
{
    CODEC_INFO_T *info = (CODEC_INFO_T*)codec_info;
    int output_samples = (int)(info->sample_rate*info->frame_ms)/1000;
    //TRACE(0, "%s %d %d %d %d", __func__, __LINE__, inputDataLength, isPlc, output_samples);
    OpusApiRet_t ret = opus_api_decoder_run(opus_decoder, (unsigned char*)input,
                inputDataLength, (short int*)output, &output_samples, isPlc);
    return ret;
}

static void gaf_audio_opus_decoder_buf_deinit(uint8_t instance_handle)
{
    LOG_I("%s instance_handle:%d", __func__, instance_handle);
    opus_memory_heap = NULL;
}

static uint32_t gaf_audio_opus_decoder_set_freq(void *_codec_info,
                                    uint32_t base_freq, uint32_t core_type)
{
    return APP_SYSFREQ_208M;
}

static GAF_DECODER_FUNC_LIST_T gaf_opus_decoder_func_list =
{
    .decoder_init_buf_func = gaf_audio_opus_decoder_buf_init,
    .decoder_init_func = gaf_audio_opus_decoder_init,
    .decoder_deinit_func = gaf_audio_opus_decoder_deinit,
    .decoder_decode_frame_func = gaf_audio_opus_decode,
    .decoder_deinit_buf_func = gaf_audio_opus_decoder_buf_deinit,
    .decoder_set_freq = gaf_audio_opus_decoder_set_freq,
};

void gaf_audio_opus_update_decoder_func_list(void *_dec_func_list)
{
    GAF_DECODER_FUNC_LIST_T **dec_func_list = (GAF_DECODER_FUNC_LIST_T **)_dec_func_list;
    *dec_func_list = &gaf_opus_decoder_func_list;
}
