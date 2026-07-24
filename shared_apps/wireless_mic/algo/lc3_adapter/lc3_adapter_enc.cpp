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
#ifdef AUDIO_LC3_ENCODE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hal_trace.h"
#include "hal_aud.h"
#include "heap_api.h"
#include "lc3_process.h"
#include "lc3_adapter.h"

static LC3_Enc_Info* p_lc3_enc_info = NULL;
heap_handle_t lc3_encode_heap_handle = NULL;
static uint8_t lc3_encode_mempool_buffer[50 * 1024] = {0};

static void audio_lc3_encode_heap_init(void)
{
    ASSERT(lc3_encode_mempool_buffer, "lc3_encode_mempool_buffer is null.");

    memset(lc3_encode_mempool_buffer, 0x00, sizeof(lc3_encode_mempool_buffer));
    lc3_encode_heap_handle = heap_register(lc3_encode_mempool_buffer, sizeof(lc3_encode_mempool_buffer));

    ASSERT(lc3_encode_heap_handle, "%s register heap failed.", __func__);
}

static void* audio_lc3_encode_heap_calloc(uint32_t size)
{
    void *ptr = NULL;

    size = ROUND_UP(size, 4);
    ptr = heap_malloc(lc3_encode_heap_handle, size);
    if (ptr) {
        memset(ptr, 0x00, size);
    }

    return ptr;
}

static void audio_lc3_encode_heap_free(void* ptr)
{
    ASSERT(ptr, "%s invalid param.", __func__)

    heap_free(lc3_encode_heap_handle, ptr);
}

static void* lc3_alloc(void* pool, uint32_t size)
{
    return audio_lc3_encode_heap_calloc(size);
}

static void lc3_free(void* pool, void* ptr)
{
    audio_lc3_encode_heap_free(ptr);
}

static int audio_lc3_encode(void* input, void** output, uint32_t output_len)
{
    int err = LC3_API_ERROR;
    if (p_lc3_enc_info->cb_encode_interlaced) {
        err = p_lc3_enc_info->cb_encode_interlaced(p_lc3_enc_info, p_lc3_enc_info->scratch,
                                                input, p_lc3_enc_info->enabled_frame_buff, output_len);

        if (err) {
            TRACE(1, "%s err:%d", __func__, err);
        } else {
            *output = p_lc3_enc_info->enabled_frame_buff;
        }
    }
    return err;
}

int audio_lc3_encoder_init(void** encode_cb, uint32_t bitrate, uint32_t frame_us)
{
    int err = LC3_API_ERROR;
    uint32_t frame_size = bitrate * frame_us / 1000000 / 8;
    LC3_Enc_Info* enc_info = NULL;
    static bool encoder_inited = false;

    if (encoder_inited) {
        return err;
    }

    audio_lc3_encode_heap_init();

    p_lc3_enc_info = (LC3_Enc_Info*)audio_lc3_encode_heap_calloc(sizeof(LC3_Enc_Info));
    ASSERT(p_lc3_enc_info, "%s malloc failed.", __func__);

    enc_info                     = p_lc3_enc_info;
    enc_info->bitrate            = 0;
    enc_info->is_lc3plus         = 0;
    enc_info->is_interlaced      = 1;
#ifdef LC3S_ENABLE
    enc_info->is_lc3s            = 1;
#endif
    enc_info->epmode             = LC3_API_EP_OFF;
    enc_info->ltpf_flag          = 1;
    enc_info->quan_prec          = 48;
    enc_info->sample_rate        = AUD_SAMPRATE_48000;
    enc_info->channels           = 1;
    enc_info->bitwidth           = AUD_BITS_24;
    enc_info->bitalign           = 32;
    enc_info->frame_dms          = (frame_us / 100);//(AUDIO_LC3_CODEC_FRAME_US / 100);
    enc_info->frame_size         = frame_size;//AUDIO_LC3_ENCODED_FRAME_SIZE;
    enc_info->cb_alloc           = &lc3_alloc;
    enc_info->cb_free            = &lc3_free;
    enc_info->enabled_frame_buff = lc3_alloc(0, enc_info->frame_size);

    err = lc3_api_encoder_init(enc_info);
    ASSERT(LC3_API_OK == err, "%s/%d err %d", __func__,__LINE__, err);
    ASSERT(enc_info->instance, "%s/%d", __func__, __LINE__);
    ASSERT(enc_info->cb_encode_interlaced, "%s/%d", __func__, __LINE__);
    ASSERT(enc_info->frame_samples, "%s/%d", __func__, __LINE__);
    ASSERT(enc_info->frame_size, "%s/%d", __func__, __LINE__);

    /* Print info */
    TRACE(1, "[lc3]");
    TRACE(1, "Encoder size:     %d", enc_info->instance_size);
    TRACE(1, "Scratch size:     %d", enc_info->scratch_size);
    TRACE(1, "Sample rate:      %d", enc_info->sample_rate);
    TRACE(1, "Channels:         %d", enc_info->channels);
    TRACE(1, "Frame samples:    %d", enc_info->frame_samples);
    TRACE(1, "Frame length:     %d", enc_info->frame_size);
    TRACE(1, "Output format:    %d bits", enc_info->bitwidth);
    TRACE(1, "Output format:    %d bits align", enc_info->bitalign);
    TRACE(1, "Target bitrate:   %d", enc_info->bitrate);
    TRACE(1, " ");

    *encode_cb = (void*)audio_lc3_encode;

    encoder_inited = true;

    return err;
}
#endif