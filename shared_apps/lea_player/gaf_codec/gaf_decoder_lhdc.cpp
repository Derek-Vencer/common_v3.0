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

static void *dec_heap_buff_l = NULL;
static void *dec_heap_buff_r = NULL;
static lhdc_decoder_t decoder_l = NULL;
static lhdc_decoder_t decoder_r = NULL;
static CODEC_FREE dec_free_cb = NULL;

static void* decoder_buf_init(int duration, int samplerate, CODEC_ALLOC alloc_cb)
{
    uint32_t dec_heap_size = lhdc_lea_util_dec_get_mem_size(duration, samplerate);
    TRACE(0, "LHDC LL decoder heap size %d", dec_heap_size);
    void * heap_buff = (alloc_cb)(NULL, dec_heap_size);
    if(heap_buff == NULL) {
        TRACE(0, "lhdc heap alloc failed");
    }
    return heap_buff;
}

static lhdc_decoder_t decoder_init(int duration, int samplerate, int depth, void * heap)
{
    lhdc_decoder_t decoder = lhdc_lea_util_init_decoder(duration, samplerate, 0, heap);
    if(decoder == NULL) {
        TRACE(0, "lhdc decoder init failed");
    }
    enum lhdc_pcm_format fmt = depth == 16 ? LHDC_PCM_FORMAT_S16 : LHDC_PCM_FORMAT_S24;
    int ret = lhdc_lea_util_dec_set_format(decoder, fmt);
    if(ret != LHDC_LEA_FRET_SUCCESS) {
        TRACE(0, "lhdc dec set format failed, ret %d", ret);
        return NULL;
    }
    return decoder;
}

static int decoder_process(lhdc_decoder_t decoder, int depth, int stride, int input_size,
    void * input, void * output)
{
    enum lhdc_pcm_format fmt = depth == 16 ? LHDC_PCM_FORMAT_S16 : LHDC_PCM_FORMAT_S24;
    int ret = lhdc_lea_util_dec_process(decoder, input, input_size, fmt, output, stride);
    if(ret != LHDC_LEA_FRET_SUCCESS) {
        TRACE(0, "lhdc dec process failed, ret %d", ret);
    } else {
        TRACE(0, "dec process success");
        DUMP8("0x%02x ", output, 16);
    }
    return ret;
}


static void gaf_audio_lhdc_decoder_buf_init(uint8_t instance_handle,
                                void *_codec_info, CODEC_ALLOC alloc_cb, CODEC_FREE free_cb)
{
    CODEC_INFO_T *codec_info = (CODEC_INFO_T*)_codec_info;
    int duration = (int)(codec_info->frame_ms*1000);
    int samplerate = codec_info->sample_rate;
    int ch = codec_info->num_channels;
    TRACE(0, "[gaf lhdc][init] duration %dus, samplerate %d, ch %d", duration, samplerate, ch);
    dec_heap_buff_l = decoder_buf_init(duration, samplerate, alloc_cb);
    if(ch == 2) {
        dec_heap_buff_r = decoder_buf_init(duration, samplerate, alloc_cb);
    }
    dec_free_cb = free_cb;
}

static void gaf_audio_lhdc_decoder_init(uint8_t instance_handle, void *_codec_info)
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
    decoder_l = decoder_init(duration, samplerate, depth, dec_heap_buff_l);
    if(ch == 2) {
        decoder_r = decoder_init(duration, samplerate, depth, dec_heap_buff_r);
    }
    LOG_I("%s end", __func__);
}

static void gaf_audio_lhdc_decoder_deinit(uint8_t instance_handle)
{
    LOG_I("%s instance_handle:%d", __func__, instance_handle);
    decoder_l = NULL;
    decoder_r = NULL;
}

FRAM_TEXT_LOC
static int gaf_audio_lhdc_decode(uint8_t instance_handle, void *codec_info,
            uint32_t inputDataLength, void *input, void *output, bool isPlc)
{
    TRACE(0, "%s %d %d", __func__, __LINE__, inputDataLength);
    CODEC_INFO_T *info = (CODEC_INFO_T*)codec_info;
    int depth = info->bits_depth;
    int stride = info->num_channels;
    int bytes_per_ch = info->frame_size;
    int ch = info->num_channels;
    int ret = decoder_process(decoder_l, depth, stride, bytes_per_ch, input, output);
    if(ch ==2 ) {
        int offset = depth == 16 ? 2 : 4;
        char * input_p = (char*)input + bytes_per_ch;
        char * output_p = (char*)output + offset;
        ret = decoder_process(decoder_r, depth, stride, bytes_per_ch, input_p, output_p);
    }
    return ret;
}

static void gaf_audio_lhdc_decoder_buf_deinit(uint8_t instance_handle)
{
    LOG_I("%s instance_handle:%d", __func__, instance_handle);
    if(dec_free_cb) {
        if(dec_heap_buff_l) {
            dec_free_cb(NULL, dec_heap_buff_l);
            dec_heap_buff_l = NULL;
        }
        if(dec_heap_buff_r) {
            dec_free_cb(NULL, dec_heap_buff_r);
            dec_heap_buff_r = NULL;
        }
    }
}

static uint32_t gaf_audio_lhdc_decoder_set_freq(void *_codec_info,
                                    uint32_t base_freq, uint32_t core_type)
{
    return APP_SYSFREQ_208M;
}

static GAF_DECODER_FUNC_LIST_T gaf_lhdc_decoder_func_list =
{
    .decoder_init_buf_func = gaf_audio_lhdc_decoder_buf_init,
    .decoder_init_func = gaf_audio_lhdc_decoder_init,
    .decoder_deinit_func = gaf_audio_lhdc_decoder_deinit,
    .decoder_decode_frame_func = gaf_audio_lhdc_decode,
    .decoder_deinit_buf_func = gaf_audio_lhdc_decoder_buf_deinit,
    .decoder_set_freq = gaf_audio_lhdc_decoder_set_freq,
};

void gaf_audio_lhdc_update_decoder_func_list(void *_dec_func_list)
{
    GAF_DECODER_FUNC_LIST_T **dec_func_list = (GAF_DECODER_FUNC_LIST_T **)_dec_func_list;
    *dec_func_list = &gaf_lhdc_decoder_func_list;
}
#endif