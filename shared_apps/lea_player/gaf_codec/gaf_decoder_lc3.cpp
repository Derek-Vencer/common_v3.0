/***************************************************************************
 *
 * Copyright 2015-2023 BES.
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
#include "cmsis.h"
#include "cmsis_os.h"
#include "app_utils.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_aud.h"
#include "hal_location.h"
#include "audio_dump.h"
#include "app_overlay.h"
#include "lc3_process.h"
#include "gaf_codec_lc3.h"
#include "gaf_dbg.h"
#include "gaf_codec_common.h"
#include "cc_stream_common.h"
#if defined(GAF_LC3_BES_PLC_ON)
#include "sbcplc.h"
#elif defined(GAF_MUSIC_PLC_ON)
#include "stdio.h"
#include "plc_api.h"
#include "stdarg.h"
#endif

/************************private macro defination***************************/
#define AOB_CALCULATE_CODEC_MIPS       0
#define LC3_AUDIO_DOWNLOAD_DUMP        0

/************************private type defination****************************/
typedef enum
{
    LC3_DEC_FREQ_SET_CHN_1_MIN = 0,
    LC3_DEC_FREQ_SET_1_8000_16000_10 = LC3_DEC_FREQ_SET_CHN_1_MIN,
    LC3_DEC_FREQ_SET_1_16000_16000_10,
    LC3_DEC_FREQ_SET_1_16000_32000_10,
    LC3_DEC_FREQ_SET_1_32000_32000_10,
    LC3_DEC_FREQ_SET_1_32000_48000_10,
    LC3_DEC_FREQ_SET_1_32000_64000_10,
    LC3_DEC_FREQ_SET_1_44100_88200_10,
    LC3_DEC_FREQ_SET_1_48000_32000_10,
    LC3_DEC_FREQ_SET_1_48000_96000_10,
    LC3_DEC_FREQ_SET_CHN_1_MAX,

    LC3_DEC_FREQ_SET_CHN_2_MIN = LC3_DEC_FREQ_SET_CHN_1_MAX,
    LC3_DEC_FREQ_SET_2_8000_32000_10 = LC3_DEC_FREQ_SET_CHN_2_MIN,
    LC3_DEC_FREQ_SET_2_16000_32000_10,
    LC3_DEC_FREQ_SET_2_16000_64000_10,
    LC3_DEC_FREQ_SET_2_32000_64000_10,
    LC3_DEC_FREQ_SET_2_32000_96000_10,
    LC3_DEC_FREQ_SET_2_32000_128000_10,
    LC3_DEC_FREQ_SET_2_44100_176400_10,
    LC3_DEC_FREQ_SET_2_48000_192000_10,
    LC3_DEC_FREQ_SET_2_96000_192000_5,

    LC3_DEC_FREQ_SET_MAX,
}lc3_decoder_freq_setting_e;

/************************private variable defination************************/
#if defined(GAF_LC3_BES_PLC_ON)
static PLC_State *lc3_plc_state1 = NULL;
static POSSIBLY_UNUSED PLC_State *lc3_plc_state2 = NULL;
static float *lc3_cos_buf = NULL;
static int lc3_packeet_len = 0;
static int lc3_smooth_len = 0;
static int plc_is_16bit = 0;
#elif defined(GAF_MUSIC_PLC_ON)
typedef struct _plc_memory{
    void *(*plc_alloc_cb)(void *pool, unsigned size);
    void (*plc_free_cb)(void *pool, void *ptr);
}plc_memory;
plc_memory plc_mem;
#endif

const codec_freq_setting_t lc3_dec_m55_freq_table[LC3_DEC_FREQ_SET_MAX] =
{
    /* channels, sample_rate, bit_rate, frame_ms, freq */
    {1,         8000,       16000,      10,     APP_SYSFREQ_26M},
    {1,         16000,      16000,      10,     APP_SYSFREQ_26M},
    {1,         16000,      32000,      10,     APP_SYSFREQ_26M},
    {1,         32000,      32000,      10,     APP_SYSFREQ_52M},
    {1,         32000,      48000,      10,     APP_SYSFREQ_52M},
    {1,         32000,      64000,      10,     APP_SYSFREQ_52M},
    {1,         44100,      88200,      10,     APP_SYSFREQ_52M},
    {1,         48000,      32000,      10,     APP_SYSFREQ_52M},
    {1,         48000,      96000,      10,     APP_SYSFREQ_52M},

    {2,         8000,       32000,      10,     APP_SYSFREQ_52M},
    {2,         16000,      32000,      10,     APP_SYSFREQ_52M},
    {2,         16000,      64000,      10,     APP_SYSFREQ_52M},
    {2,         32000,      64000,      10,     APP_SYSFREQ_52M},
    {2,         32000,      64000,      10,     APP_SYSFREQ_52M},
    {2,         32000,      128000,     10,     APP_SYSFREQ_52M},
    {2,         44100,      176400,     10,     APP_SYSFREQ_52M},
    {2,         48000,      192000,     10,     APP_SYSFREQ_52M},
    {2,         96000,      192000,     5,      APP_SYSFREQ_78M},
};

const codec_freq_setting_t lc3_dec_cp_freq_table[LC3_DEC_FREQ_SET_MAX] =
{
    /* channels, sample_rate, bit_rate, frame_ms, freq */
    {1,         8000,       16000,      10,     APP_SYSFREQ_52M},
    {1,         16000,      16000,      10,     APP_SYSFREQ_52M},
    {1,         16000,      32000,      10,     APP_SYSFREQ_52M},
    {1,         32000,      32000,      10,     APP_SYSFREQ_52M},
    {1,         32000,      48000,      10,     APP_SYSFREQ_52M},
    {1,         32000,      64000,      10,     APP_SYSFREQ_52M},
    {1,         44100,      88200,      10,     APP_SYSFREQ_52M},
    {1,         48000,      96000,      10,     APP_SYSFREQ_52M},

    {2,         8000,       32000,      10,     APP_SYSFREQ_52M},
    {2,         16000,      32000,      10,     APP_SYSFREQ_52M},
    {2,         16000,      64000,      10,     APP_SYSFREQ_52M},
    {2,         32000,      64000,      10,     APP_SYSFREQ_78M},
    {2,         32000,      64000,      10,     APP_SYSFREQ_78M},
    {2,         32000,      128000,     10,     APP_SYSFREQ_78M},
    {2,         44100,      176400,     10,     APP_SYSFREQ_78M},
    {2,         48000,      192000,     10,     APP_SYSFREQ_78M},
    {2,         96000,      192000,     5,      APP_SYSFREQ_78M},
};

const codec_freq_setting_t lc3_dec_m33_freq_table[LC3_DEC_FREQ_SET_MAX] =
{
    /* channels, sample_rate, bit_rate, frame_ms, freq */
    {1,         8000,       16000,      10,     APP_SYSFREQ_52M},
    {1,         16000,      16000,      10,     APP_SYSFREQ_52M},
    {1,         16000,      32000,      10,     APP_SYSFREQ_52M},
    {1,         32000,      32000,      10,     APP_SYSFREQ_52M},
    {1,         32000,      48000,      10,     APP_SYSFREQ_52M},
    {1,         32000,      64000,      10,     APP_SYSFREQ_52M},
    {1,         44100,      88200,      10,     APP_SYSFREQ_52M},
    {1,         48000,      96000,      10,     APP_SYSFREQ_52M},

    {2,         8000,       32000,      10,     APP_SYSFREQ_52M},
    {2,         16000,      32000,      10,     APP_SYSFREQ_52M},
    {2,         16000,      64000,      10,     APP_SYSFREQ_52M},
    {2,         32000,      64000,      10,     APP_SYSFREQ_78M},
    {2,         32000,      64000,      10,     APP_SYSFREQ_78M},
    {2,         32000,      128000,     10,     APP_SYSFREQ_78M},
    {2,         44100,      176400,     10,     APP_SYSFREQ_78M},
    {2,         48000,      192000,     10,     APP_SYSFREQ_78M},
    {2,         96000,      192000,     5,      APP_SYSFREQ_208M},
};

LC3_Dec_Info g_lc3_dec_info[PLAYBACK_INSTANCE_MAX];
#ifdef LC3_IN_ROM_V2
extern lc3_buf_param _lc3_buf_param;
#endif

#if AOB_CALCULATE_CODEC_MIPS
aob_codec_time_info_t aob_decode_time_info = {0};
#endif

/********************** function declaration*************************/
#if defined(GAF_MUSIC_PLC_ON)
static void *PlcAlloc(int size){
    static int sizeTotal = 0;
    sizeTotal += size;
    TRACE(0, "sizeTotal:%d", sizeTotal);
    return plc_mem.plc_alloc_cb(0, size);
}
static void PlcFree(void *ptr){
    plc_mem.plc_free_cb(0, ptr);
}
static void PlcPrint(const char* fmt, ...) {
    static char buf[256];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    TRACE(0, "%s", buf);
}

static bool music_plc_create(void **hd, void *dec_info){
// int fs, int ch, int width, int frameSample, bool isFloat = 0){
    plc_api_register_music_plc_int16();
    plc_api_register_music_plc_int32();
    plc_api_register_music_plc_f32();

    LC3_Dec_Info *lc3_dec_info = (LC3_Dec_Info *)dec_info;
    plc_mem.plc_alloc_cb = lc3_dec_info->cb_alloc;
    plc_mem.plc_free_cb = lc3_dec_info->cb_free;
    plc_api_param_t plcParam;
    memset(&plcParam, 0, sizeof(plc_api_param_t));
    plcParam.fs_hz= lc3_dec_info->sample_rate;
    plcParam.channels= lc3_dec_info->channels;
    plcParam.frame_samples= lc3_dec_info->frame_samples;
    plcParam.channel_select = 0x00ff;
    uint16_t width = lc3_dec_info->bitalign >> 3;
    if(lc3_dec_info->is_float)
        plcParam.data_type = plc_api_data_type_e::PLC_API_DATA_TYPE_FLOAT_32;
    else if(width==2)
        plcParam.data_type = plc_api_data_type_e::PLC_API_DATA_TYPE_SHORT_16;
    else
        plcParam.data_type = plc_api_data_type_e::PLC_API_DATA_TYPE_INT_32;
    plcParam.mode = (plc_api_mode_e)PLC_API_MODE_MUSIC_PLC;
    plcParam.param_set = (plc_api_param_set_e)PLC_API_PARAM_SET_PARAM;
    uint32_t sample_per_ms = plcParam.fs_hz / 1000;
    plcParam.music_plc.overlap_samples = 1 * sample_per_ms;
    plcParam.music_plc.hold_samples_after_lost = 0 * sample_per_ms;
    plcParam.music_plc.attenuate_samples_after_lost = 50 * sample_per_ms;
    plcParam.music_plc.gain_samples_after_no_lost = 50 * sample_per_ms;
    plcParam.music_plc.seek_samples = 10 * sample_per_ms;
    plcParam.music_plc.no_seek_samples = 10 * sample_per_ms;
    plcParam.music_plc.match_samples = 4 * sample_per_ms;
    plcParam.cb_malloc = PlcAlloc;
    plcParam.cb_free = PlcFree;
    plcParam.cb_printf = PlcPrint;
    if (lc3_dec_info->frame_dms < 75)
    {
        plcParam.music_plc.overlap_samples /= 2;
    }
    LOG_I("plc_api_create enter");
    plc_api_ret_t ret = plc_api_create(hd, &plcParam);
    if(ret != PLC_API_RET_SUCCESS){
        TRACE(0, "init fail");
        return -1;
    }
    LOG_I("plc_api_create success");
    return true;
}

static bool music_plc_run(void *lc3_dec_info, uint8_t *iobuf, bool isLost){
	LC3_Dec_Info *info = (LC3_Dec_Info *)lc3_dec_info;
	void *hd = info->lc3_plc_state;
    if(!hd){
        return true;
    }
    plc_api_ret_t ret;
	int in_len = info->frame_samples * (info->bitalign>>3) * info->channels;
    int out_len = in_len;
    if(isLost==false){
        ret = plc_api_run(hd, (uint8_t*)iobuf, in_len, 0, (uint8_t*)iobuf, &out_len, 0x0000);
    }
    else{
        ret = plc_api_run(hd, NULL, 0, NULL, (uint8_t*)iobuf, &out_len, 0xffff);
    }
    if(ret != PLC_API_RET_SUCCESS){
        TRACE(0, "run fail, %d", ret);
        return false;
    }
    return true;
} 
#endif

static uint32_t gaf_audio_lc3_decoder_set_freq(void *_codec_info,
                                    uint32_t base_freq, uint32_t core_type)
{
    CODEC_INFO_T* codec_info = (CODEC_INFO_T*)_codec_info;
    uint32_t lc3_dec_freq = base_freq;
    uint32_t sample_rate = codec_info->sample_rate;
    uint16_t frame_size = codec_info->frame_size * codec_info->num_channels;
    uint32_t bit_rate = (uint32_t)(frame_size * 8000 / codec_info->frame_ms);
    uint8_t frame_ms = (uint8_t)(codec_info->frame_ms);
    const codec_freq_setting_t *freq_table = NULL;
    uint32_t index = LC3_DEC_FREQ_SET_CHN_1_MIN;
    uint32_t indexMax = LC3_DEC_FREQ_SET_CHN_1_MAX;

    if (44100 == sample_rate){
        bit_rate = bit_rate * 44100 / 48000;
    }

    if (CP_CORE == core_type) {
        freq_table = lc3_dec_cp_freq_table;
    }
    else if (M55_CORE == core_type) {
        freq_table = lc3_dec_m55_freq_table;
    }
    else {
        freq_table = lc3_dec_m33_freq_table;
    }

    if (2 == codec_info->num_channels) {
        index = LC3_DEC_FREQ_SET_CHN_2_MIN;
        indexMax = LC3_DEC_FREQ_SET_MAX;
    }
    for (; index < indexMax; index++)
    {
        if (sample_rate == freq_table[index].sample_rate && frame_ms == freq_table[index].frame_ms)
        {
            lc3_dec_freq = freq_table[index].freq;
        }
        else if (frame_ms <= 5)
        {
            lc3_dec_freq = APP_SYSFREQ_208M;
            break;
        }
    }

    LOG_I("lc3 dec freq:%d bit_rate:%d", lc3_dec_freq, bit_rate);
    return lc3_dec_freq;
}

POSSIBLY_UNUSED static uint32_t aob_lc3_get_sysfreq(void)
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
static int aob_stream_lc3_decode_bits(LC3_Dec_Info *p_lc3_dec_info,
        void *input_bytes, int32_t num_bytes, void *output_samples, bool isPlc)
{
    int32_t bfi_ext = 0;
    int32_t err = LC3_API_OK;
    if (isPlc)
    {
        bfi_ext = 1;
    }

    if(num_bytes == 0)
    {
        memset(input_bytes, 0, p_lc3_dec_info->frame_size);
        num_bytes = p_lc3_dec_info->frame_size;
        bfi_ext = 1;
        // LOG_I("%s, get empty packet, num_bytes:%d", __func__, num_bytes);
    }

#if AOB_CALCULATE_CODEC_MIPS
    uint32_t cpu_freq = 0;
    POSSIBLY_UNUSED uint32_t stime = 0, etime = 0;
    uint32_t use_time_in_us = 0, total_etime_in_ms = 0;
    if (!aob_decode_time_info.codec_started)
    {
        aob_decode_time_info.codec_started = true;
        aob_decode_time_info.total_time_in_us = 0;
        aob_decode_time_info.start_time_in_ms = hal_sys_timer_get();
    }

    stime = hal_sys_timer_get();
#endif

    err = p_lc3_dec_info->cb_decode_interlaced(p_lc3_dec_info, p_lc3_dec_info->scratch,
            input_bytes, num_bytes, output_samples, bfi_ext);

#if defined(GAF_LC3_BES_PLC_ON)
   if (p_lc3_dec_info->channels == 1)
   {
        if(plc_is_16bit == 1)
        {
            if (bfi_ext)
            {
                a2dp_plc_bad_frame(lc3_plc_state1, (short *)output_samples, (short *)output_samples, lc3_cos_buf, lc3_packeet_len, 1, 0);
            }
            else {
                a2dp_plc_good_frame(lc3_plc_state1, (short *)output_samples, (short *)output_samples, lc3_cos_buf, lc3_packeet_len, 1, 0);
            }
        }else{
            if (bfi_ext)
            {
                a2dp_plc_bad_frame_24bit(lc3_plc_state1, (int32_t *)output_samples, (int32_t *)output_samples, lc3_cos_buf, lc3_packeet_len, 1, 0);
            }
            else {
                a2dp_plc_good_frame_24bit(lc3_plc_state1, (int32_t *)output_samples, (int32_t *)output_samples, lc3_cos_buf, lc3_packeet_len, 1, 0);
            }
        }
    }
   else {
        if(plc_is_16bit == 1)
        {
            if (bfi_ext)
            {
                a2dp_plc_bad_frame(lc3_plc_state1, (short *)output_samples, (short *)output_samples, lc3_cos_buf, lc3_packeet_len, 2, 0);
                a2dp_plc_bad_frame(lc3_plc_state2, (short *)output_samples, (short *)output_samples, lc3_cos_buf, lc3_packeet_len, 2, 1);
            }
            else {
                a2dp_plc_good_frame(lc3_plc_state1, (short *)output_samples, (short *)output_samples, lc3_cos_buf, lc3_packeet_len, 2, 0);
                a2dp_plc_good_frame(lc3_plc_state2, (short *)output_samples, (short *)output_samples, lc3_cos_buf, lc3_packeet_len, 2, 1);
            }
        }else{
            if (bfi_ext)
            {
                a2dp_plc_bad_frame_24bit(lc3_plc_state1, (int32_t *)output_samples, (int32_t *)output_samples, lc3_cos_buf, lc3_packeet_len, 2, 0);
                a2dp_plc_bad_frame_24bit(lc3_plc_state2, (int32_t *)output_samples, (int32_t *)output_samples, lc3_cos_buf, lc3_packeet_len, 2, 1);
            }
            else {
                a2dp_plc_good_frame_24bit(lc3_plc_state1, (int32_t *)output_samples, (int32_t *)output_samples, lc3_cos_buf, lc3_packeet_len, 2, 0);
                a2dp_plc_good_frame_24bit(lc3_plc_state2, (int32_t *)output_samples, (int32_t *)output_samples, lc3_cos_buf, lc3_packeet_len, 2, 1);
            }
        }
    }
#elif defined(GAF_MUSIC_PLC_ON)
    music_plc_run(p_lc3_dec_info, (uint8_t*)output_samples, bfi_ext);
#endif

#if AOB_CALCULATE_CODEC_MIPS
    etime = hal_sys_timer_get();
    use_time_in_us = TICKS_TO_US(etime - stime);
    total_etime_in_ms = TICKS_TO_MS(etime - aob_decode_time_info.start_time_in_ms);
    aob_decode_time_info.total_time_in_us += use_time_in_us;
    cpu_freq = aob_lc3_get_sysfreq();
    if (total_etime_in_ms)
    {
        aob_decode_time_info.codec_mips =
            cpu_freq * (aob_decode_time_info.total_time_in_us / 1000) / total_etime_in_ms;
    }
    LOG_I("err %d ticks:%d time:%d us",
        err, (etime - stime), use_time_in_us);
    LOG_I("freq %d use:%d ms total:%d ms mips: %d M", cpu_freq,
        aob_decode_time_info.total_time_in_us/1000,
        total_etime_in_ms, aob_decode_time_info.codec_mips);
#endif
    return (int)err;
}

static void gaf_audio_lc3_decoder_buf_init(uint8_t instance_handle,
                                void *_codec_info, CODEC_ALLOC alloc_cb, CODEC_FREE free_cb)
{
    LOG_I("%s instance_handle:%d", __func__, instance_handle);
    g_lc3_dec_info[instance_handle].cb_alloc = alloc_cb;
    g_lc3_dec_info[instance_handle].cb_free = free_cb;
}

static void gaf_audio_lc3_decoder_init(uint8_t instance_handle, void *_codec_info)
{
    LOG_I("%s instance_handle:%d", __func__, instance_handle);
    CODEC_INFO_T *codec_info = (CODEC_INFO_T*)_codec_info;
    LC3_Dec_Info *p_lc3_dec_info = &g_lc3_dec_info[instance_handle];

    p_lc3_dec_info->sample_rate = codec_info ->sample_rate;
    p_lc3_dec_info->channels   = codec_info ->num_channels;
    p_lc3_dec_info->bitwidth   = codec_info ->bits_depth;
    p_lc3_dec_info->bitalign   = (p_lc3_dec_info->bitwidth == 24) ? 32 : 0;
    p_lc3_dec_info->frame_dms  = (uint8_t)(codec_info ->frame_ms * 10);
    p_lc3_dec_info->frame_size = codec_info ->frame_size * p_lc3_dec_info->channels;
    p_lc3_dec_info->bitrate    = 0;
    p_lc3_dec_info->plcMeth    = LC3_API_PLC_ADVANCED;
    p_lc3_dec_info->epmode     = LC3_API_EP_OFF;
    p_lc3_dec_info->is_interlaced = 1;
#ifdef LC3_IN_ROM_V2
    _lc3_buf_param.is_encoded  = false;
    _lc3_buf_param.sample_rate = p_lc3_dec_info->sample_rate;
    _lc3_buf_param.frame_dms   = p_lc3_dec_info->frame_dms;
    _lc3_buf_param.channels    = p_lc3_dec_info->channels;
#endif

    int err = lc3_api_decoder_init(p_lc3_dec_info);
    ASSERT(LC3_API_OK == err, "%s err %d", __func__, err);

    LOG_I("[lc3]");
    LOG_I("Bits:             %d", p_lc3_dec_info->bitwidth);
    LOG_I("Bits align:       %d", p_lc3_dec_info->bitalign);
    LOG_I("Sample rate:      %d", p_lc3_dec_info->sample_rate);
    LOG_I("Channels:         %d", p_lc3_dec_info->channels);
    LOG_I("Frame samples:    %d", p_lc3_dec_info->frame_samples);
    LOG_I("Frame length:     %d", p_lc3_dec_info->frame_size);
    LOG_I("PLC mode:         %d", p_lc3_dec_info->plcMeth);
    LOG_I("Bitrate:          %d", p_lc3_dec_info->bitrate);

#if defined(GAF_LC3_BES_PLC_ON)
        lc3_packeet_len = p_lc3_dec_info->sample_rate * p_lc3_dec_info->frame_dms / 10000 * 2;
        lc3_smooth_len = lc3_packeet_len * 4;
        if(p_lc3_dec_info->bitwidth == 16){
            plc_is_16bit = 1;
        }else{
            plc_is_16bit = 0;
        }
        lc3_plc_state1 = (PLC_State *)p_lc3_dec_info->cb_alloc(0, sizeof(PLC_State));
        a2dp_plc_lc3_init(lc3_plc_state1, A2DP_PLC_CODEC_TYPE_LC3, lc3_packeet_len / 2);
        if(p_lc3_dec_info->channels == 2)
        {
            lc3_plc_state2 = (PLC_State *)p_lc3_dec_info->cb_alloc(0, sizeof(PLC_State));
            a2dp_plc_lc3_init(lc3_plc_state2, A2DP_PLC_CODEC_TYPE_LC3, lc3_packeet_len / 2);
        }
        lc3_cos_buf = (float *)p_lc3_dec_info->cb_alloc(0, lc3_smooth_len * sizeof(float));
        cos_generate(lc3_cos_buf, lc3_smooth_len, lc3_packeet_len);
#elif defined(GAF_MUSIC_PLC_ON)
        LOG_I("music_plc_create enter");
        music_plc_create(&p_lc3_dec_info->lc3_plc_state, p_lc3_dec_info);
#endif

#if AOB_CALCULATE_CODEC_MIPS
    memset(&aob_decode_time_info, 0, sizeof(aob_codec_time_info_t));
#endif

    /* lc3 audio dump parameters init */
#if (LC3_AUDIO_DOWNLOAD_DUMP)
    uint32_t num_channels = codec_info->num_channels;
    uint32_t dump_frame_len = (uint32_t)(codec_info->sample_rate * codec_info->frame_ms/1000);
    auto bitalign = (codec_info->bits_depth == 24)? 32 : codec_info->bits_depth;
    audio_dump_init(dump_frame_len, bitalign/8, num_channels);
    TRACE(0, "dump_frame_len:%d, bitalign:%d", dump_frame_len, bitalign);
#endif

    LOG_I("%s end", __func__);
}

static void gaf_audio_lc3_decoder_deinit(uint8_t instance_handle)
{
    LOG_I("%s", __func__);
}

FRAM_TEXT_LOC
static int gaf_audio_lc3_decode(uint8_t instance_handle, void *codec_info,
            uint32_t inputDataLength, void *input, void *output, bool isPlc)
{
    int ret = LC3_API_ERROR;
    LC3_Dec_Info *p_lc3_dec_info = &g_lc3_dec_info[instance_handle];
    ret = aob_stream_lc3_decode_bits(p_lc3_dec_info, input, inputDataLength, output, isPlc);
 #if LC3_AUDIO_DOWNLOAD_DUMP
    auto frame_samples = p_lc3_dec_info->frame_samples;
    audio_dump_clear_up();
    audio_dump_add_channel_data(0, output, frame_samples);
    audio_dump_run();
#endif

    if (LC3_API_OK == ret) {
        return CODEC_OK;
    }
    return CODEC_ERR;
}

static void gaf_audio_lc3_decoder_buf_deinit(uint8_t instance_handle)
{
    LOG_I("%s instance_handle:%d", __func__, instance_handle);
    LC3_Dec_Info *p_lc3_dec_info = &g_lc3_dec_info[instance_handle];
    ASSERT(p_lc3_dec_info->cb_free, "%s cb_free is NULL", __func__);
    if(p_lc3_dec_info->cb_uninit) {
        p_lc3_dec_info->cb_uninit(p_lc3_dec_info);
    }
    lc3_api_decoder_extra_buf_deinit(p_lc3_dec_info);
#if defined(GAF_LC3_BES_PLC_ON)
    p_lc3_dec_info->cb_free(0, lc3_plc_state1);
    lc3_plc_state1 = NULL;
    if(lc3_plc_state2 != NULL) {
        p_lc3_dec_info->cb_free(0, lc3_plc_state2);
        lc3_plc_state2 = NULL;
    }
    p_lc3_dec_info->cb_free(0, lc3_cos_buf);
    lc3_cos_buf = NULL;
#elif defined(GAF_MUSIC_PLC_ON)
    plc_api_destory(p_lc3_dec_info->lc3_plc_state);
#endif
    p_lc3_dec_info->instance = NULL;
    p_lc3_dec_info->scratch = NULL;
    p_lc3_dec_info->enabled_frame_buff = NULL;
}

static GAF_DECODER_FUNC_LIST_T gaf_lc3_decoder_func_list =
{
    .decoder_init_buf_func = gaf_audio_lc3_decoder_buf_init,
    .decoder_init_func = gaf_audio_lc3_decoder_init,
    .decoder_deinit_func = gaf_audio_lc3_decoder_deinit,
    .decoder_decode_frame_func = gaf_audio_lc3_decode,
    .decoder_deinit_buf_func = gaf_audio_lc3_decoder_buf_deinit,
    .decoder_set_freq = gaf_audio_lc3_decoder_set_freq,
};

void gaf_audio_lc3_update_decoder_func_list(void *_dec_func_list)
{
    GAF_DECODER_FUNC_LIST_T **dec_func_list = (GAF_DECODER_FUNC_LIST_T **)_dec_func_list;
    *dec_func_list = &gaf_lc3_decoder_func_list;
}
