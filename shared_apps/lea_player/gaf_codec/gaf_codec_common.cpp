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
#include "cmsis.h"
#include "cmsis_os.h"
#include "hal_trace.h"
#include "plat_types.h"
#include "gaf_dbg.h"
#include "gaf_codec_common.h"

/************************private macro defination***************************/
#define GAF_CODEC_BUF_ENTRY_COUNT    (20)

#ifdef LC3_IN_ROM_V2
lc3_buf_param _lc3_buf_param;
#endif

/************************private strcuture defination****************************/

/************************private variable defination************************/
static GAF_CODEC_BUF_ENTRY_T *codec_buf_entry[GAF_CODEC_BUF_ENTRY_COUNT] = {NULL};

/**********************private function declaration*************************/

/****************************function defination****************************/
static void gaf_codec_alloc_data_store(uint8_t* raw_buf, unsigned size)
{
    ASSERT(raw_buf, "%s size:%d", __func__, size);
    for (uint32_t i = 0; i < GAF_CODEC_BUF_ENTRY_COUNT; i++) {
        if (codec_buf_entry[i] == NULL) {
            GAF_CODEC_BUF_ENTRY_T *entry = (GAF_CODEC_BUF_ENTRY_T *)raw_buf;
            entry->size = size;
            entry->state = GAF_CODEC_BUF_STATE_USED;
            codec_buf_entry[i] = entry;
            TRACE(0,"gaf_codec_store[%d] data:%p size:%d", i,
                    raw_buf + sizeof(GAF_CODEC_BUF_ENTRY_T), size);
            return;
        }
    }
    ASSERT(0, "%s no free entry size:%d data:0x%p", __func__, size, raw_buf);
}

static uint8_t* gaf_codec_get_stored_data(unsigned size)
{
    uint8_t *data = NULL;
    uint32_t best_idx = GAF_CODEC_BUF_ENTRY_COUNT;
    uint32_t best_size = UINT32_MAX;
    for (uint32_t i = 0; i < GAF_CODEC_BUF_ENTRY_COUNT; i++) {
        if (codec_buf_entry[i] == NULL) {
            continue;
        }
        if (!gaf_codec_buf_entry_is_valid(codec_buf_entry[i])) {
            ASSERT(0, "%s entry[%d] corrupted:%p state:0x%x", __func__, 
                    i, codec_buf_entry[i], codec_buf_entry[i]->state);
            continue;
        }
        if (gaf_codec_buf_entry_is_free(codec_buf_entry[i]) && codec_buf_entry[i]->size >= size) {
            if (codec_buf_entry[i]->size < best_size) {
                best_size = codec_buf_entry[i]->size;
                best_idx = i;
                if (best_size == size) {
                    break;
                }
            }
        }
    }
    if (best_idx < GAF_CODEC_BUF_ENTRY_COUNT) {
        codec_buf_entry[best_idx]->state = GAF_CODEC_BUF_STATE_USED;
        data = gaf_codec_buf_entry_to_data(codec_buf_entry[best_idx]);
        TRACE(0,"gaf_codec_reuse[%d] 0x%p size:%d req:%d", best_idx, data, best_size, size);
    }
    return data;
}

static uint32_t gaf_codec_buf_get_alloc_size(uint32_t size)
{
// default size for lc3 rom
#ifdef LC3_IN_ROM_V2
    if(size == 30720 || size == 38912 || size == 34816)
    {
        if(_lc3_buf_param.is_encoded)
        {
            if(_lc3_buf_param.channels == 1)
            {
                if(_lc3_buf_param.sample_rate == 48000)
                {
                    switch(_lc3_buf_param.frame_dms)
                    {
                        case 100: size = 18 * 1024; break;
                        case 75:  size = 16 * 1024; break;
                        case 50:  size = 14 * 1024; break;
                        case 25:  size = 12 * 1024; break;
                        default: ASSERT(0, "lc3 frame_dms no support:%d", _lc3_buf_param.frame_dms);
                    }
                }else if(_lc3_buf_param.sample_rate == 32000)
                {
                    switch(_lc3_buf_param.frame_dms)
                    {
                        case 100: size = 16 * 1024; break;
                        case 75:  size = 14 * 1024; break;
                        case 50:  size = 13 * 1024; break;
                        case 25:  size = 11 * 1024; break;
                        default: ASSERT(0, "lc3 frame_dms no support:%d", _lc3_buf_param.frame_dms);
                    }
                }else if(_lc3_buf_param.sample_rate == 16000)
                {
                    switch(_lc3_buf_param.frame_dms)
                    {
                        case 100: size = 12 * 1024; break;
                        case 75:  size = 12 * 1024; break;
                        case 50:  size = 11 * 1024; break;
                        case 25:  size = 10 * 1024; break;
                        default: ASSERT(0, "lc3 frame_dms no support:%d", _lc3_buf_param.frame_dms);
                    }
                }
            }else if(_lc3_buf_param.channels == 2){
                if(_lc3_buf_param.sample_rate == 48000)
                {
                    switch(_lc3_buf_param.frame_dms)
                    {
                        case 100: size = 25 * 1024; break;
                        case 75:  size = 23 * 1024; break;
                        case 50:  size = 20 * 1024; break;
                        case 25:  size = 17 * 1024; break;
                        default: ASSERT(0, "lc3 frame_dms no support:%d", _lc3_buf_param.frame_dms);
                    }
                }else if(_lc3_buf_param.sample_rate == 32000)
                {
                    switch(_lc3_buf_param.frame_dms)
                    {
                        case 100: size = 22 * 1024; break;
                        case 75:  size = 20 * 1024; break;
                        case 50:  size = 18 * 1024; break;
                        case 25:  size = 16 * 1024; break;
                        default: ASSERT(0, "lc3 frame_dms no support:%d", _lc3_buf_param.frame_dms);
                    }
                }else if(_lc3_buf_param.sample_rate == 16000)
                {
                    switch(_lc3_buf_param.frame_dms)
                    {
                        case 100: size = 18 * 1024; break;
                        case 75:  size = 17 * 1024; break;
                        case 50:  size = 16 * 1024; break;
                        case 25:  size = 15 * 1024; break;
                        default: ASSERT(0, "lc3 frame_dms no support:%d", _lc3_buf_param.frame_dms);
                    }
                }
            }
        }else{
            if(_lc3_buf_param.channels == 1)
            {
                if(_lc3_buf_param.sample_rate == 48000)
                {
                    switch(_lc3_buf_param.frame_dms)
                    {
                        case 100: size = 19 * 1024; break;
                        case 75:  size = 17 * 1024; break;
                        case 50:  size = 13 * 1024; break;
                        case 25:  size = 11 * 1024; break;
                        default: ASSERT(0, "lc3 frame_dms no support:%d", _lc3_buf_param.frame_dms);
                    }
                }else if(_lc3_buf_param.sample_rate == 32000)
                {
                    switch(_lc3_buf_param.frame_dms)
                    {
                        case 100: size = 14 * 1024; break;
                        case 75:  size = 12 * 1024; break;
                        case 50:  size = 10 * 1024; break;
                        case 25:  size = 8 * 1024; break;
                        default: ASSERT(0, "lc3 frame_dms no support:%d", _lc3_buf_param.frame_dms);
                    }
                }else if(_lc3_buf_param.sample_rate == 16000)
                {
                    switch(_lc3_buf_param.frame_dms)
                    {
                        case 100: size = 8 * 1024; break;
                        case 75:  size = 7 * 1024; break;
                        case 50:  size = 6 * 1024; break;
                        case 25:  size = 5 * 1024; break;
                        default: ASSERT(0, "lc3 frame_dms no support:%d", _lc3_buf_param.frame_dms);
                    }
                }
            }else{
                if(_lc3_buf_param.sample_rate == 48000)
                {
                    switch(_lc3_buf_param.frame_dms)
                    {
                        case 100: size = 29 * 1024; break;
                        case 75:  size = 26 * 1024; break;
                        case 50:  size = 21 * 1024; break;
                        case 25:  size = 18 * 1024; break;
                        default: ASSERT(0, "lc3 frame_dms no support:%d", _lc3_buf_param.frame_dms);
                    }
                }else if(_lc3_buf_param.sample_rate == 32000)
                {
                    switch(_lc3_buf_param.frame_dms)
                    {
                        case 100: size = 21 * 1024; break;
                        case 75:  size = 19 * 1024; break;
                        case 50:  size = 16 * 1024; break;
                        case 25:  size = 14 * 1024; break;
                        default: ASSERT(0, "lc3 frame_dms no support:%d", _lc3_buf_param.frame_dms);
                    }
                }else if(_lc3_buf_param.sample_rate == 16000)
                {
                    switch(_lc3_buf_param.frame_dms)
                    {
                        case 100: size = 13 * 1024; break;
                        case 75:  size = 12 * 1024; break;
                        case 50:  size = 10 * 1024; break;
                        case 25:  size = 9 * 1024; break;
                        default: ASSERT(0, "lc3 frame_dms no support:%d", _lc3_buf_param.frame_dms);
                    }
                }
            }
        }
    }
#endif
    return size;
}

void *gaf_codec_buf_alloc_with_reuse(void *pool, uint32_t size, CODEC_ALLOC alloc_cb)
{
    size = gaf_codec_buf_get_alloc_size(size);
    TRACE(0,"gaf_codec_alloc size:%d", size);
    uint8_t *data = gaf_codec_get_stored_data(size);
    if (data != NULL) {
        return data;
    }
    uint8_t *raw_buf = (uint8_t *)alloc_cb(pool, sizeof(GAF_CODEC_BUF_ENTRY_T) + size);
    if (raw_buf == NULL) {
        ASSERT(0, "%s alloc failed size:%d", __func__, size);
        return NULL;
    }
    gaf_codec_alloc_data_store(raw_buf, size);
    return gaf_codec_buf_entry_to_data((GAF_CODEC_BUF_ENTRY_T *)raw_buf);
}

/*
 * Mark buffer as FREE for reuse, NOT actually free the underlying memory.
 * The raw_buf (including embedded entry header) is retained so that
 * gaf_get_stored_data() can match and reuse it on next alloc request,
 * avoiding repeated alloc/free overhead across M33/M55/CP cores.
 * Memory is truly reclaimed only when the syspool itself is deinitialized.
 */
void gaf_codec_buf_free(void *pool, void *data)
{
    ASSERT(data, "%s data is NULL", __func__);
    GAF_CODEC_BUF_ENTRY_T *entry = gaf_codec_buf_data_to_entry(data);
    for (uint32_t i = 0; i < GAF_CODEC_BUF_ENTRY_COUNT; i++) {
        if (codec_buf_entry[i] == entry) {
            ASSERT(gaf_codec_buf_entry_is_used(codec_buf_entry[i]), 
                    "%s entry[%d] state:0x%x data:%p size:%d", __func__, i,
                    codec_buf_entry[i]->state, data, codec_buf_entry[i]->size);

            codec_buf_entry[i]->state = GAF_CODEC_BUF_STATE_FREE;
            TRACE(0,"gaf_codec_free[%d]:%p size:%d", i, data, codec_buf_entry[i]->size);
            return;
        }
    }
    ASSERT(0, "%s unknown data:%p", __func__, data);
    return;
}

void gaf_codec_buf_entry_init(void)
{
    TRACE(0,"%s count:%d", __func__, GAF_CODEC_BUF_ENTRY_COUNT);
    for (uint32_t i = 0; i < GAF_CODEC_BUF_ENTRY_COUNT; i++) {
        codec_buf_entry[i] = NULL;
    }
}
