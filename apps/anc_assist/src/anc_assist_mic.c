 /***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#include "anc_assist_mic.h"
#include "hal_codec.h"
#include "hal_trace.h"
#include "tgt_hardware.h"
#include <string.h>

#ifndef ANC_FF_MIC_CH_L
#define ANC_FF_MIC_CH_L            (0xFF)
#endif

#ifndef ANC_FF_MIC_CH_R
#define ANC_FF_MIC_CH_R            (0xFF)
#endif

#ifndef ANC_FB_MIC_CH_L
#define ANC_FB_MIC_CH_L            (0xFF)
#endif

#ifndef ANC_FB_MIC_CH_R
#define ANC_FB_MIC_CH_R            (0xFF)
#endif

#ifndef ANC_TALK_MIC_CH_L
#define ANC_TALK_MIC_CH_L        (0xFF)
#endif

#ifndef ANC_TALK_MIC_CH_R
#define ANC_TALK_MIC_CH_R        (0xFF)
#endif

#ifndef ANC_REF_MIC_CH_L
#define ANC_REF_MIC_CH_L        (0xFF)
#endif

#ifndef ANC_REF_MIC_CH_R
#define ANC_REF_MIC_CH_R        (0xFF)
#endif

#ifndef ANC_SPK_CH
#define ANC_SPK_CH                (AUD_CHANNEL_MAP_CH0)
#endif

static uint32_t anc_mic_cfg        = 0;
static uint32_t app_mic_cfg     = 0;
static uint32_t all_mic_cfg     = 0;
static uint32_t anc_mic_ch_num     = 0;
static uint32_t app_mic_ch_num    = 0;
static uint32_t all_mic_ch_num     = 0;

static uint32_t ff_l_mic_index    = MIC_INDEX_QTY;
static uint32_t ff_r_mic_index  = MIC_INDEX_QTY;
static uint32_t fb_l_mic_index    = MIC_INDEX_QTY;
static uint32_t fb_r_mic_index  = MIC_INDEX_QTY;
static uint32_t talk_l_mic_index = MIC_INDEX_QTY;
static uint32_t talk_r_mic_index = MIC_INDEX_QTY;
static uint32_t ref_l_mic_index  = MIC_INDEX_QTY;
static uint32_t ref_r_mic_index  = MIC_INDEX_QTY;

static uint32_t codec_dev_get_mic_cfg(enum AUD_IO_PATH_T path)
{
    if (path == AUD_IO_PATH_NULL) {
        return 0;
    } else {
        return hal_codec_get_input_path_cfg(path) & AUD_CHANNEL_MAP_ALL;
    }
}

static uint32_t codec_dev_get_mic_path_ch_num(enum AUD_IO_PATH_T path)
{
    return hal_codec_get_input_path_chan_num(path);
}

static uint32_t codec_dev_get_mic_map_ch_num(uint32_t ch_map)
{
    return hal_codec_get_input_map_chan_num(ch_map);
}

int32_t anc_assist_mic_reset(void)
{
    STATIC_ASSERT(ANC_FF_MIC_CH_L     != 0xFF, "Need define ANC_FF_MIC_CH_L in tgt_hardware.h");
    STATIC_ASSERT(ANC_FF_MIC_CH_R     != 0xFF, "Need define ANC_FF_MIC_CH_L in tgt_hardware.h");
    STATIC_ASSERT(ANC_FB_MIC_CH_L     != 0xFF, "Need define ANC_FB_MIC_CH_L in tgt_hardware.h");
    STATIC_ASSERT(ANC_FB_MIC_CH_R     != 0xFF, "Need define ANC_FB_MIC_CH_L in tgt_hardware.h");
    STATIC_ASSERT(ANC_TALK_MIC_CH_L != 0xFF, "Need define ANC_TALK_MIC_CH_L in tgt_hardware.h");
    STATIC_ASSERT(ANC_TALK_MIC_CH_R != 0xFF, "Need define ANC_TALK_MIC_CH_R in tgt_hardware.h");
    STATIC_ASSERT(ANC_REF_MIC_CH_L  != 0xFF, "Need define ANC_REF_MIC_CH_L  in tgt_hardware.h");
    STATIC_ASSERT(ANC_REF_MIC_CH_R  != 0xFF, "Need define ANC_REF_MIC_CH_R  in tgt_hardware.h");

    anc_mic_cfg        = codec_dev_get_mic_cfg(AUD_INPUT_PATH_ANC_ASSIST);
    app_mic_cfg     = 0;

    anc_mic_ch_num     = codec_dev_get_mic_path_ch_num(AUD_INPUT_PATH_ANC_ASSIST);
    app_mic_ch_num    = 0;

    all_mic_cfg     = anc_mic_cfg;
    all_mic_ch_num     = anc_mic_ch_num;

    ANC_TRACE(4, "[%s] CH MAP: 0x%x", __func__, anc_mic_cfg);
    ANC_TRACE(4, "[%s] CH NUM: %d", __func__, anc_mic_ch_num);

    // TODO: Check ANC VMIC includes other VMIC

    return 0;
}

int32_t anc_assist_mic_parser_app_buf(void *buf, uint32_t *len)
{
    ASSERT(*len % sizeof(int32_t) == 0, "[%s]buf_len err", __FUNCTION__);
    int32_t *pcm_buf = (int32_t  *)buf;
    uint32_t pcm_len = *len / sizeof(int32_t);

    uint32_t in_offset = 0;
    uint32_t out_offset = 0;

    for(in_offset = 0, out_offset = 0; in_offset < pcm_len; in_offset += all_mic_ch_num, out_offset += app_mic_ch_num) {
        uint32_t all_ch_cnt = 0;
        uint32_t app_ch_cnt = 0;

        uint32_t app_cfg = app_mic_cfg;
        uint32_t all_cfg = all_mic_cfg;
        while (app_cfg) {
            if (all_cfg & 0x1) {
                if (app_cfg & 0x1) {
                    pcm_buf[out_offset + app_ch_cnt] = pcm_buf[in_offset + all_ch_cnt];
                    app_ch_cnt++;
                }
                all_ch_cnt++;
            }

        all_cfg >>= 1;
        app_cfg >>= 1;
        }
    }
    *len = (*len) / all_mic_ch_num * app_mic_ch_num;

    return 0;
}

int32_t anc_assist_mic_parser_anc_buf(bool pcm_interval, enum AUD_IO_PATH_T path, anc_assist_pcm_t *anc_buf,
                                        uint32_t anc_frame_len, void *buf, uint32_t pcm_len, uint32_t pcm_bytes)
{
    uint32_t ch_num = (path == AUD_INPUT_PATH_ANC_ASSIST) ? anc_mic_ch_num : all_mic_ch_num;
    uint32_t frame_len = pcm_len / ch_num;

    for (int32_t ch = 0; ch < (int32_t)ch_num; ch++) {
        int plane_index = -1;
        if      (ch == ff_l_mic_index)   plane_index = MIC_INDEX_FF_L;
        else if (ch == ff_r_mic_index)   plane_index = MIC_INDEX_FF_R;
        else if (ch == fb_l_mic_index)   plane_index = MIC_INDEX_FB_L;
        else if (ch == fb_r_mic_index)   plane_index = MIC_INDEX_FB_R;
        else if (ch == talk_l_mic_index) plane_index = MIC_INDEX_TALK_L;
        else if (ch == talk_r_mic_index) plane_index = MIC_INDEX_TALK_R;
        else if (ch == ref_l_mic_index)  plane_index = MIC_INDEX_REF_L;
        else if (ch == ref_r_mic_index)  plane_index = MIC_INDEX_REF_R;
        else continue;

        anc_assist_pcm_t *dst = NULL;
#ifdef ANC_ASSIST_UNUSED_ON_PHONE_CALL
        anc_assist_pcm_t **planes = (anc_assist_pcm_t **)anc_buf;
        dst = planes[plane_index];
#else
        dst = ((anc_assist_pcm_t*)anc_buf) + (uint32_t)anc_frame_len * plane_index;
#endif

        if (!dst) continue;

        uint32_t shift_bits = 0;

#ifdef ANC_ASSIST_16BIT
        if (pcm_bytes == 4) {
            shift_bits = 8;
        }
#endif

        if (pcm_bytes == 4) {
            int32_t *pcm_buf = (int32_t *)buf;
            if (pcm_interval) {
                for (uint32_t i = 0; i < frame_len; i++) {
                    dst[i] = (anc_assist_pcm_t)(pcm_buf[ch * frame_len + i] >> shift_bits);
                }
            } else {
                for (uint32_t i = 0; i < frame_len; i++) {
                    dst[i] = (anc_assist_pcm_t)(pcm_buf[i * ch_num + ch] >> shift_bits);
                }
            }
        } else {
            int16_t *pcm_buf = (int16_t *)buf;
            if (pcm_interval) {
                for (uint32_t i = 0; i < frame_len; i++) {
                    dst[i] = (anc_assist_pcm_t)(pcm_buf[ch * frame_len + i] >> shift_bits);
                }
            } else {
                for (uint32_t i = 0; i < frame_len; i++) {
                    dst[i] = (anc_assist_pcm_t)(pcm_buf[i * ch_num + ch] >> shift_bits);
                }
            }
        }
    }

    return 0;
}

int32_t anc_assist_mic_parser_index(enum AUD_IO_PATH_T path)
{
    uint32_t pos     = 0;
    uint32_t index     = 0;
    uint32_t mic_cfg = 0;

    if (path == AUD_INPUT_PATH_ANC_ASSIST) {
        mic_cfg = anc_mic_cfg;
    } else {
        mic_cfg = all_mic_cfg;
    }

    ff_l_mic_index    = MIC_INDEX_QTY;
    ff_r_mic_index  = MIC_INDEX_QTY;
    fb_l_mic_index    = MIC_INDEX_QTY;
    fb_r_mic_index  = MIC_INDEX_QTY;
    talk_l_mic_index = MIC_INDEX_QTY;
    talk_r_mic_index = MIC_INDEX_QTY;
    ref_l_mic_index  = MIC_INDEX_QTY;
    ref_r_mic_index  = MIC_INDEX_QTY;

    while (mic_cfg) {
        if (mic_cfg & 0x1) {
            if ((0x1 << pos) & ANC_FF_MIC_CH_L) {
                ff_l_mic_index = index;
            } else if ((0x1 << pos) & ANC_FF_MIC_CH_R) {
                ff_r_mic_index = index;
            } else if ((0x1 << pos) & ANC_FB_MIC_CH_L) {
                fb_l_mic_index = index;
            } else if ((0x1 << pos) & ANC_FB_MIC_CH_R) {
                fb_r_mic_index = index;
            } else if ((0x1 << pos) & ANC_TALK_MIC_CH_L) {
                talk_l_mic_index = index;
            } else if ((0x1 << pos) & ANC_TALK_MIC_CH_R) {
                talk_r_mic_index = index;
            } else if ((0x1 << pos) & ANC_REF_MIC_CH_L) {
                ref_l_mic_index = index;
            } else if ((0x1 << pos) & ANC_REF_MIC_CH_R) {
                ref_r_mic_index = index;
            }

            index++;
        }

        pos++;
        mic_cfg >>= 1;
    }

    ANC_TRACE(7, "[%s] MIC INDEX: ff_l: %d, ff_r: %d, fb_l: %d, fb_r: %d",
        __func__, ff_l_mic_index, ff_r_mic_index, fb_l_mic_index, fb_r_mic_index );
    ANC_TRACE(7, "[%s] MIC INDEX: talk_l: %d, talk_r: %d, ref_l: %d, ref_r: %d",
        __func__, talk_l_mic_index, talk_r_mic_index, ref_l_mic_index, ref_r_mic_index);

    return 0;
}

int32_t anc_assist_mic_set_app_cfg(enum AUD_IO_PATH_T path)
{

    app_mic_cfg = codec_dev_get_mic_cfg(path);
    app_mic_ch_num = codec_dev_get_mic_map_ch_num(app_mic_cfg);

    // Update all mic cfg and channel number
    all_mic_cfg = anc_mic_cfg | app_mic_cfg;
    all_mic_ch_num = codec_dev_get_mic_map_ch_num(all_mic_cfg);

    ANC_TRACE(3, "[%s] CH NUM: app: %d, all: %d", __func__, app_mic_ch_num, all_mic_ch_num);
    ANC_TRACE(3, "[%s] CH MAP: app: 0x%x, all: 0x%x", __func__, app_mic_cfg, all_mic_cfg);

    return 0;
}

int32_t anc_assist_mic_set_anc_cfg(enum AUD_IO_PATH_T path)
{
    ASSERT(path == AUD_INPUT_PATH_ANC_ASSIST, "[%s] path: %d", __func__, path);

    anc_mic_cfg = codec_dev_get_mic_cfg(path);
    anc_mic_ch_num = codec_dev_get_mic_map_ch_num(anc_mic_cfg);

    // Update all mic cfg and channel number
    all_mic_cfg = anc_mic_cfg | app_mic_cfg;
    all_mic_ch_num = codec_dev_get_mic_map_ch_num(all_mic_cfg);

    ANC_TRACE(3, "[%s] CH NUM: anc: %d, all: %d", __func__, anc_mic_ch_num, all_mic_ch_num);
    ANC_TRACE(3, "[%s] CH MAP: anc: 0x%x, all: 0x%x", __func__, anc_mic_cfg, all_mic_cfg);

    return 0;
}

int32_t anc_assist_mic_update_anc_cfg(uint32_t mic_ch_map)
{
    anc_mic_cfg = mic_ch_map;
    anc_mic_ch_num = codec_dev_get_mic_map_ch_num(anc_mic_cfg);

    // Update all mic cfg and channel number
    all_mic_cfg = anc_mic_cfg | app_mic_cfg;
    all_mic_ch_num = codec_dev_get_mic_map_ch_num(all_mic_cfg);

    ANC_TRACE(3, "[%s] CH NUM: anc: %d, all: %d", __func__, anc_mic_ch_num, all_mic_ch_num);
    ANC_TRACE(3, "[%s] CH MAP: anc: 0x%x, all: 0x%x", __func__, anc_mic_cfg, all_mic_cfg);

    return 0;
}

uint32_t anc_assist_mic_get_cfg(enum AUD_IO_PATH_T path)
{
    if (path == AUD_INPUT_PATH_ANC_ASSIST) {
        return anc_mic_cfg;
    } else {
        return all_mic_cfg;
    }
}

uint32_t anc_assist_mic_get_ch_num(enum AUD_IO_PATH_T path)
{
    if (path == AUD_INPUT_PATH_ANC_ASSIST) {
        return anc_mic_ch_num;
    } else {
        return all_mic_ch_num;
    }
}

uint32_t anc_assist_mic_anc_mic_is_enabled(anc_assist_mic_index_t index)
{
    if ((index == MIC_INDEX_FF_L) && (anc_mic_cfg & ANC_FF_MIC_CH_L)) {
        return true;
    } else if ((index == MIC_INDEX_FF_R) && (anc_mic_cfg & ANC_FF_MIC_CH_R)) {
        return true;
    } else if ((index == MIC_INDEX_FB_L) && (anc_mic_cfg & ANC_FB_MIC_CH_L)) {
        return true;
    } else if ((index == MIC_INDEX_FB_R) && (anc_mic_cfg & ANC_FB_MIC_CH_R)) {
        return true;
    } else if ((index == MIC_INDEX_TALK_L) && (anc_mic_cfg & ANC_TALK_MIC_CH_L)) {
        return true;
    } else if ((index == MIC_INDEX_TALK_R) && (anc_mic_cfg & ANC_TALK_MIC_CH_R)) {
        return true;
    } else if ((index == MIC_INDEX_REF_L) && (anc_mic_cfg & ANC_REF_MIC_CH_L)) {
        return true;
    } else if ((index == MIC_INDEX_REF_R) && (anc_mic_cfg & ANC_REF_MIC_CH_R)) {
        return true;
    } else {
        return false;
    }
}