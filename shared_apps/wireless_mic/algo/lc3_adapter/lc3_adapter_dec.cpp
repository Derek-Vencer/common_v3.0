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
#ifdef AUDIO_LC3_DECODE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hal_trace.h"
#include "hal_aud.h"
#include "heap_api.h"
#include "lc3_process.h"
#include "lc3_adapter.h"

#ifdef AUDIO_DECODE_ON_CP
#define AUDIO_LC3_DECODER_NUM                       (MAX_INPUT_CH_NUM)
#if 1//def CHIP_ROLE_CP
#define AUDIO_LC3_SESSION_ID_START                  (0)
#define AUDIO_LC3_SESSION_ID_END                    (AUDIO_LC3_DECODER_NUM - 1)
#else
#define AUDIO_LC3_SESSION_ID_START                  (MAX_INPUT_CH_NUM / 2)
#define AUDIO_LC3_SESSION_ID_END                    (MAX_INPUT_CH_NUM - 1)
#endif
#else
#define AUDIO_LC3_DECODER_NUM                       (MAX_INPUT_CH_NUM)
#define AUDIO_LC3_SESSION_ID_START                  (0)
#define AUDIO_LC3_SESSION_ID_END                    (AUDIO_LC3_DECODER_NUM - 1)
#endif

#define AUDIO_LC3_DECODER_RAM_SIZE                  (17 * 1024)
#define AUDIO_LC3_DECODER_HEAP_SIZE                 (AUDIO_LC3_DECODER_RAM_SIZE * AUDIO_LC3_DECODER_NUM)

static LC3_Dec_Info* p_lc3_dec_info[AUDIO_LC3_DECODER_NUM] = {NULL};
heap_handle_t lc3_decode_heap_handle = NULL;
static uint8_t lc3_decode_mempool_buffer[AUDIO_LC3_DECODER_HEAP_SIZE] = {0};

static uint8_t audio_lc3_session_to_decoder_id(uint8_t session_id)
{
    if (session_id >= AUDIO_LC3_SESSION_ID_START && session_id <= AUDIO_LC3_SESSION_ID_END) {
#ifdef CHIP_ROLE_CP
        return session_id;
#else
        return session_id - AUDIO_LC3_SESSION_ID_START;
#endif
    } else {
        ASSERT(0, "invalid lc3 session id:%d", session_id);
        return 0;
    }
}

static void audio_lc3_decode_heap_init(void)
{
    ASSERT(lc3_decode_mempool_buffer, "lc3_decode_mempool_buffer is null.");
    memset(lc3_decode_mempool_buffer, 0x00, AUDIO_LC3_DECODER_HEAP_SIZE);

    lc3_decode_heap_handle = heap_register(lc3_decode_mempool_buffer, AUDIO_LC3_DECODER_HEAP_SIZE);
    ASSERT(lc3_decode_heap_handle, "%s register heap failed.", __func__);
}

static void* audio_lc3_decode_heap_calloc(uint32_t size)
{
    void *ptr = NULL;

    size = ROUND_UP(size, 4);
    ptr = heap_malloc(lc3_decode_heap_handle, size);
    if (ptr) {
        memset(ptr, 0x00, size);
    }

    return ptr;
}

static void audio_lc3_decode_heap_free(void* ptr)
{
    ASSERT(ptr, "%s invalid param.", __func__)

    heap_free(lc3_decode_heap_handle, ptr);
}

static void* lc3_alloc(void* pool, uint32_t size)
{
    return audio_lc3_decode_heap_calloc(size);
}

static void lc3_free(void* pool, void* ptr)
{
    audio_lc3_decode_heap_free(ptr);
}

static int audio_lc3_decode(uint8_t session_id, void* input, void** output, uint32_t input_len)
{
    int err = LC3_API_ERROR;
    uint8_t dec_id = audio_lc3_session_to_decoder_id(session_id);
    LC3_Dec_Info* dec_info = NULL;

    if (dec_id >= AUDIO_LC3_DECODER_NUM) {
        TRACE(2, "%s invalid dec id:%d", __func__, dec_id);
        return;
    }

    dec_info = p_lc3_dec_info[dec_id];
    if (dec_info && dec_info->cb_decode_interlaced) {
        err = dec_info->cb_decode_interlaced(dec_info, dec_info->scratch, input, input_len, dec_info->enabled_frame_buff, 0);

        if (err) {
            TRACE(1, "%s err:%d", __func__, err);
            *output = NULL;
        } else {
            *output = dec_info->enabled_frame_buff;
        }
    }
    return err;
}

int audio_lc3_decoder_init(void** decode_cb, uint32_t* frame_size)
{
    int err = LC3_API_ERROR;
    LC3_Dec_Info* dec_info = NULL;
    static bool decoder_inited = false;

    if (decoder_inited) {
        return err;
    }

    audio_lc3_decode_heap_init();

    for (uint8_t dec_id = 0; dec_id < AUDIO_LC3_DECODER_NUM; dec_id++) {
        p_lc3_dec_info[dec_id] = (LC3_Dec_Info*)audio_lc3_decode_heap_calloc(sizeof(LC3_Dec_Info));
        ASSERT(p_lc3_dec_info[dec_id], "%s malloc failed.", __func__);

        dec_info                     = p_lc3_dec_info[dec_id];
        dec_info->bitrate            = 0;
        dec_info->is_lc3plus         = 0;
        dec_info->is_interlaced      = 1;
        dec_info->epmode             = LC3_API_EP_OFF;
        dec_info->ltpf_flag          = 1;
        dec_info->quan_prec          = 48;
        dec_info->sample_rate        = AUD_SAMPRATE_48000;
        dec_info->channels           = 1;
        dec_info->bitwidth           = AUD_BITS_24;
        dec_info->bitalign           = 32;
        dec_info->frame_dms          = (AUDIO_LC3_CODEC_FRAME_US / 100); //dms means 0.1ms
        dec_info->frame_size         = AUDIO_lc3_decODED_FRAME_SIZE;
        dec_info->cb_alloc           = &lc3_alloc;
        dec_info->cb_free            = &lc3_free;
        dec_info->enabled_frame_buff = lc3_alloc(0, AUDIO_LC3_DECODED_FRAME_SIZE);

        err = lc3_api_decoder_init(dec_info);
        ASSERT(LC3_API_OK == err, "%s/%d err %d", __func__,__LINE__, err);
        ASSERT(dec_info->instance, "%s/%d", __func__, __LINE__);
        ASSERT(dec_info->cb_decode_interlaced, "%s/%d", __func__, __LINE__);
        ASSERT(dec_info->frame_samples, "%s/%d", __func__, __LINE__);
        ASSERT(dec_info->frame_size, "%s/%d", __func__, __LINE__);

        /* Print info */
        if (dec_id == 0) {
            TRACE(1, "[lc3]");
            TRACE(1, "decoder size:     %d", dec_info->instance_size);
            TRACE(1, "Scratch size:     %d", dec_info->scratch_size);
            TRACE(1, "Sample rate:      %d", dec_info->sample_rate);
            TRACE(1, "Channels:         %d", dec_info->channels);
            TRACE(1, "Frame samples:    %d", dec_info->frame_samples);
            TRACE(1, "Frame length:     %d", dec_info->frame_size);
            TRACE(1, "Output format:    %d bits", dec_info->bitwidth);
            TRACE(1, "Output format:    %d bits align", dec_info->bitalign);
            TRACE(1, "Target bitrate:   %d", dec_info->bitrate);
            TRACE(1, " ");
        }
    }

    *decode_cb = (void*)audio_lc3_decode;
    *frame_size = AUDIO_LC3_DECODED_FRAME_SIZE;

    decoder_inited = true;

    return err;
}
#endif