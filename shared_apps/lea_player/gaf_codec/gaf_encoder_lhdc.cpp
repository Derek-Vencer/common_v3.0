/***************************************************************************
 *
 * Copyright 2015-2026 BES.
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
#ifdef LEA_LHDC
#include <stdio.h>
#include <string.h>

#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_location.h"
#include "app_utils.h"
#include "cc_stream_common.h"
#include "gaf_stream_dbg.h"
#include "gaf_codec_common.h"
#include "gaf_codec_lhdc.h"
#include "cmsis.h"
#include "heap_api.h"
#include "lhdc_lea.h"
#include "lhdcv5_util_dec.h"

static void *enc_heap_buff_l = NULL;
static void *enc_heap_buff_r = NULL;
static lhdc_encoder_t encoder_l = NULL;
static lhdc_encoder_t encoder_r = NULL;

static void* encoder_buf_init(int duration, int samplerate, CODEC_ALLOC alloc_cb)
{
    uint32_t enc_heap_size = lhdc_lea_util_enc_get_mem_size(duration, samplerate);
    TRACE(0, "LHDC LL encoder heap size %d", enc_heap_size);
    void * heap_buff = (alloc_cb)(NULL, enc_heap_size);
    if(heap_buff == NULL) {
        TRACE(0, "lhdc heap alloc failed");
    }
    return heap_buff;
}

static lhdc_encoder_t encoder_init(int duration, int samplerate, int depth, void * heap)
{
    lhdc_encoder_t encoder = lhdc_lea_util_init_encoder(duration, samplerate, 0, heap);
    if(encoder == NULL) {
        TRACE(0, "lhdc encoder init failed");
    }
    enum lhdc_pcm_format fmt = depth == 16 ? LHDC_PCM_FORMAT_S16 : LHDC_PCM_FORMAT_S24;
    int ret = lhdc_lea_util_enc_set_format(encoder, fmt);
    if(ret != LHDC_LEA_FRET_SUCCESS) {
        TRACE(0, "lhdc enc set format failed, ret %d", ret);
        return NULL;
    }
    return encoder;
}

static int encoder_process(lhdc_encoder_t encoder, int depth, int stride, int output_size,
    void * input, void * output)
{
    enum lhdc_pcm_format fmt = depth == 16 ? LHDC_PCM_FORMAT_S16 : LHDC_PCM_FORMAT_S24;
    int ret = lhdc_lea_util_enc_process(encoder, fmt, input, stride, output_size, output);
    if(ret != LHDC_LEA_FRET_SUCCESS) {
        TRACE(0, "lhdc enc process failed, ret %d", ret);
    } else {
        TRACE(0, "enc process success");
        DUMP8("0x%02x ", output, 16);
    }
    return ret;
}

static void gaf_audio_lhdc_encoder_buf_init(uint8_t instance_handle,
                                        void *_codec_info, CODEC_ALLOC alloc_cb, CODEC_FREE free_cb)
{
    CODEC_INFO_T *codec_info = (CODEC_INFO_T*)_codec_info;
    int duration = (int)(codec_info->frame_ms*1000);
    int samplerate = codec_info->sample_rate;
    int ch = codec_info->num_channels;
    TRACE(0, "[gaf lhdc][init] duration %dus, samplerate %d, ch %d", duration, samplerate, ch);
    enc_heap_buff_l = encoder_buf_init(duration, samplerate, alloc_cb);
    if(ch == 2) {
        enc_heap_buff_r = encoder_buf_init(duration, samplerate, alloc_cb);
    }
}

static void gaf_audio_lhdc_encoder_init(uint8_t instance_handle, void *_codec_info)
{
    CODEC_INFO_T *codec_info = (CODEC_INFO_T*)_codec_info;
    TRACE(0, "(%s)[%d] (%d,%d,%d,%d,%d,%d)", __func__, __LINE__, codec_info->sample_rate,
                                codec_info->num_channels, (int)(codec_info->frame_ms*1000),
                                codec_info->bits_depth, codec_info->frame_size,
                                codec_info->pcm_size);
    int duration = (int)(codec_info->frame_ms*1000);
    int samplerate = codec_info->sample_rate;
    int depth = codec_info->bits_depth;
    int ch = codec_info->num_channels;
    encoder_l = encoder_init(duration, samplerate, depth, enc_heap_buff_l);
    if(ch == 2) {
        encoder_r = encoder_init(duration, samplerate, depth, enc_heap_buff_r);
    }
    LOG_I("%s end", __func__);
}

static void gaf_audio_lhdc_encoder_deinit(uint8_t instance_handle)
{
    LOG_I("%s instance_handle:%d", __func__, instance_handle);
    encoder_l = NULL;
    encoder_r = NULL;
}

FRAM_TEXT_LOC
static int gaf_audio_lhdc_encode(uint8_t instance_handle, void *codec_info,
    uint32_t inputDataLength, void *input, uint16_t frame_size, uint8_t* output)
{
    TRACE(0, "%s %d %d %d", __func__, __LINE__, inputDataLength, frame_size);
    CODEC_INFO_T *info = (CODEC_INFO_T*)codec_info;
    int depth = info->bits_depth;
    int stride = info->num_channels;
    int bytes_per_ch = info->frame_size;
    int ch = info->num_channels;
    int ret = encoder_process(encoder_l, depth, stride, bytes_per_ch, input, output);
    if(ch ==2 ) {
        int offset = depth == 16 ? 2 : 4;
        char * input_p = (char*)input + offset;
        char * output_p = (char*)output + bytes_per_ch;
        ret = encoder_process(encoder_r, depth, stride, bytes_per_ch, input_p, output_p);
    }
    return ret;
}

static void gaf_audio_lhdc_encoder_buf_deinit(uint8_t instance_handle)
{
    LOG_I("%s instance_handle:%d", __func__, instance_handle);
    enc_heap_buff_l = NULL;
    enc_heap_buff_r = NULL;
}

static uint32_t gaf_audio_lhdc_encoder_set_freq(void *_codec_info,
                                    uint32_t base_freq, uint32_t core_type)
{
    return APP_SYSFREQ_208M;
}

static GAF_ENCODER_FUNC_LIST_T gaf_audio_lhdc_encoder_func_list =
{
    .encoder_init_buf_func = gaf_audio_lhdc_encoder_buf_init,
    .encoder_init_func = gaf_audio_lhdc_encoder_init,
    .encoder_deinit_func = gaf_audio_lhdc_encoder_deinit,
    .encoder_encode_frame_func = gaf_audio_lhdc_encode,
    .encoder_deinit_buf_func = gaf_audio_lhdc_encoder_buf_deinit,
    .encoder_set_freq = gaf_audio_lhdc_encoder_set_freq,
};

void gaf_audio_lhdc_update_encoder_func_list(void *enc_func_list)
{
    GAF_ENCODER_FUNC_LIST_T **encoder_func_list = (GAF_ENCODER_FUNC_LIST_T **)enc_func_list;
    *encoder_func_list = &gaf_audio_lhdc_encoder_func_list;
}

int gaf_audio_lhdc_encoder_get_max_frame_size(void)
{
    return LHDC_MAX_FRAME_SIZE;
}
#endif