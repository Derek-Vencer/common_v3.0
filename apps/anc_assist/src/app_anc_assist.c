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
#include "string.h"
#include "app_utils.h"
#if defined(USE_BASIC_THREADS)
#include "app_thread.h"
#else
#include "app_audio_thread.h"
#endif
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_codec.h"
#include "anc_process.h"
#include "audioflinger.h"
#include "hwtimer_list.h"
#include "audio_dump.h"
#include "speech_cfg.h"
#include "anc_assist.h"
#include "app_anc_assist.h"
#include "app_voice_assist_anc.h"
#include "anc_assist_defs.h"
#include "anc_assist_utils.h"
#include "anc_assist_anc.h"
#include "anc_assist_mic.h"
#include "app_anc_assist_tws_sync.h"
#include "anc_assist_resample.h"
#include "assist_ultrasound.h"
#include "app_anc_assist_trigger.h"
#include "app_voice_assist_ultrasound.h"
#include "app_audio.h"
#include "speech_eq.h"
#include "anc_sync_cfg.h"

#ifdef ANC_ASSIST_PROCESS_THREAD
#include "anc_assist_thread.h"
#endif

#define ANC_ASSIST_UPDATE_SYSFREQ

#ifdef ANC_APP
#define ANC_ASSIST_WITH_ANC
#endif

// #define ANC_ASSIST_AUDIO_DUMP_96K

// #define  ANC_ASSIST_AUDIO_DUMP_32K

// #define ANC_ASSIST_AUDIO_DUMP

// #define _LOOP_CNT_FIXED_MAX

/*if open VOICE_ASSIST_DC_FILTER, HEAP_BUFF_SIZE need more*/
// #define VOICE_ASSIST_DC_FILTER


#if defined(ASSIST_LOW_RAM_MOD)
#define ALGO_SAMPLE_RATE    (8000)
#else
#define ALGO_SAMPLE_RATE      (16000)
#endif

#if defined(ASSIST_LOW_RAM_MOD)
#define ALGO_FRAME_LEN      (60)
#else
#define ALGO_FRAME_LEN       (120)
#endif

#define ALGO_FRAME_MS       (1000.0 * ALGO_FRAME_LEN / ALGO_SAMPLE_RATE)

#ifdef VOICE_ASSIST_WD_ENABLED
#define SAMPLE_RATE_MAX     (96000)
#elif defined (VOICE_ASSIST_FF_FIR_LMS)
#define SAMPLE_RATE_MAX     (32000)
#elif defined (VOICE_ASSIST_FF_IIR_LMS)
#define SAMPLE_RATE_MAX     (16000)
#elif defined (ASSIST_LOW_RAM_MOD)
#define SAMPLE_RATE_MAX     (8000)
#else
#define SAMPLE_RATE_MAX     (ALGO_SAMPLE_RATE)
#endif

#if (SAMPLE_RATE_MAX != ALGO_SAMPLE_RATE)
#define ANC_ASSIST_RESAMPLE_ENABLE
#endif

#ifdef ANC_ASSIST_16BIT
#define _SAMPLE_BITS        (16)
#else
#define _SAMPLE_BITS        (24)
#endif

#define MAX_CHANNEL_NUM        (MAX_FF_CHANNEL_NUM + MAX_FB_CHANNEL_NUM + MAX_TALK_CHANNEL_NUM + MAX_REF_CHANNEL_NUM)
#if defined (ASSIST_LOW_RAM_MOD)
#define _FRAME_LEN          (ALGO_FRAME_LEN)
#else
#define _FRAME_LEN          (ALGO_FRAME_LEN * SAMPLE_RATE_MAX / ALGO_SAMPLE_RATE)
#endif
#ifdef VOICE_ASSIST_WD_ENABLED
#define _LOOP_CNT           (2)
#else
#define _LOOP_CNT           (3)
#endif
#define MAX_LOOP_CNT        (3)

#ifdef VOICE_ASSIST_WD_ENABLED
#define _PLAY_SAMPLE_RATE   (96000)
#else
#define _PLAY_SAMPLE_RATE   (48000)
#endif
#define MAX_PLAY_CHANNEL_NUM   (1)
#define _PLAY_FRAME_LEN     (ALGO_FRAME_LEN * _PLAY_SAMPLE_RATE / ALGO_SAMPLE_RATE)

typedef enum {
    _ASSIST_MSG_OPEN_CLOSE = 0,
    _ASSIST_MSG_CLOSE_SPK,
    _ASSIST_MSG_SET_MODE,
    _ASSIST_MSG_USER_RESET,
} _ASSIST_MSG_T;

#define ANC_ASSIST_SIGNAL_SET_MODE      (0xFF)

// Capture stream
#define AF_PINGPONG                    (2)
#ifdef AUDIO_LINEIN
#define STREAM_CAPTURE_ID            AUD_STREAM_ID_3
#else
#define STREAM_CAPTURE_ID            AUD_STREAM_ID_0
#endif
#define MAX_CODEC_CAPTURE_BUF_SIZE        (_FRAME_LEN * sizeof(_PCM_T) * MAX_CHANNEL_NUM * AF_PINGPONG * MAX_LOOP_CNT)
static uint8_t *codec_capture_buf = NULL;

#define CAPTURE_BUF_LEN             (_FRAME_LEN * MAX_LOOP_CNT)
#if defined(ANC_ASSIST_UNUSED_ON_PHONE_CALL)
static anc_assist_pcm_t *capture_buf[MAX_CHANNEL_NUM] = {NULL};
#else
static anc_assist_pcm_t capture_buf[MAX_CHANNEL_NUM][CAPTURE_BUF_LEN];
#endif

// Play stream
#if !defined(ANC_ASSIST_PILOT_TONE_ALWAYS_ON)
#define STREAM_PLAY_ID                AUD_STREAM_ID_0
#define STREAM_PLAY_CODEC            AUD_STREAM_USE_INT_CODEC
#else
#define STREAM_PLAY_ID                AUD_STREAM_ID_3
#define STREAM_PLAY_CODEC            AUD_STREAM_USE_INT_CODEC2
#endif

#define MAX_CODEC_PLAY_BUF_SIZE         (_PLAY_FRAME_LEN * sizeof(_PCM_T) * MAX_PLAY_CHANNEL_NUM * AF_PINGPONG * MAX_LOOP_CNT)
#define PLAY_BUF_LEN                    (_PLAY_FRAME_LEN * MAX_LOOP_CNT)

#if defined(ANC_ASSIST_PILOT_TONE_ALWAYS_ON)
static  uint8_t __attribute__((aligned(4))) codec_play_buf[MAX_CODEC_PLAY_BUF_SIZE];
static  float play_buf[PLAY_BUF_LEN];
#endif

#if defined(FREEMAN_ENABLED_STERO)
#define HEAP_BUFF_SIZE       (1024 * 19)
#else
#define HEAP_BUFF_SIZE       (1024 * 15)
#endif

#if defined(ANC_ASSIST_UNUSED_ON_PHONE_CALL)
static uint8_t *voice_assist_heap_buff = NULL;
static bool need_open_anc_assist = false;
#else
static uint8_t __attribute__((aligned(4))) voice_assist_heap_buff[HEAP_BUFF_SIZE];
#endif

#if defined(VOICE_ASSIST_DC_FILTER)
static EqState* dc_filter[2];
#endif

#include "ext_heap.h"

static int32_t g_sample_rate = ALGO_SAMPLE_RATE;
static int32_t g_sample_bits = _SAMPLE_BITS;
static int32_t g_chan_num = MAX_CHANNEL_NUM;
static int32_t g_frame_len = ALGO_FRAME_LEN;
static int32_t g_loop_cnt = _LOOP_CNT;
static int32_t g_capture_buf_size = MAX_CODEC_CAPTURE_BUF_SIZE;

static int32_t g_app_capture_sample_rate = ALGO_SAMPLE_RATE;
static int32_t g_app_frame_len = ALGO_FRAME_LEN;

static POSSIBLY_UNUSED int32_t g_play_sample_rate = _PLAY_SAMPLE_RATE;
static int32_t g_play_buf_size = MAX_CODEC_PLAY_BUF_SIZE;

static enum APP_SYSFREQ_FREQ_T g_sys_freq = APP_SYSFREQ_32K;
bool g_opened_flag = false;
static bool g_mode_switching_flag = false;
static anc_assist_mode_t g_anc_assist_mode = ANC_ASSIST_MODE_QTY;

static bool g_need_open_mic = true;
static bool g_need_open_spk = true;
static bool g_mic_open_flag = false;
static bool g_spk_open_flag = false;

static anc_assist_pcm_t *g_ff_mic_buf[MAX_FF_CHANNEL_NUM]         = {NULL};
static anc_assist_pcm_t *g_fb_mic_buf[MAX_FB_CHANNEL_NUM]        = {NULL};
static anc_assist_pcm_t *g_talk_mic_buf[MAX_TALK_CHANNEL_NUM]     = {NULL};
static anc_assist_pcm_t *g_ref_mic_buf[MAX_REF_CHANNEL_NUM]     = {NULL};
#if defined(ANC_ASSIST_VPU)
static anc_assist_pcm_t *g_vpu_mic_buf     = NULL;
#endif
static uint32_t mic_cfg_old = 0;
static uint32_t spk_cfg_old = 0;

static uint32_t g_user_status = 0;
static voice_assist_user_info_t init_user_name[ANC_ASSIST_USER_MAX];
static const voice_assist_algo_callback_t *g_user_algo_handler[ANC_ASSIST_USER_MAX] = {NULL};
static const voice_assist_stream_callback_t *g_user_stream_handler[ANC_ASSIST_USER_MAX] = {NULL};

static void _open_mic(void);
static void _open_spk(void);
static void _close_mic(void);
static void _close_spk(void);

static int32_t _anc_assist_open_impl(anc_assist_user_t user, bool need_set_mode);
static int32_t _anc_assist_close_impl(anc_assist_user_t user, bool need_set_mode);
static int32_t app_anc_assist_direct_set_mode(anc_assist_mode_t mode);

#ifdef ANC_ASSIST_UPDATE_SYSFREQ
static void _anc_assist_update_sysfreq(void);
static void _anc_assist_reset_sysfreq(void);
#endif

#ifdef ANC_ASSIST_AUDIO_DUMP
typedef short        _DUMP_PCM_T;
static _DUMP_PCM_T audio_dump_buf[ALGO_FRAME_LEN];
#endif

#ifdef ANC_ASSIST_AUDIO_DUMP_32K
typedef short        _DUMP_PCM_T;
static _DUMP_PCM_T audio_dump_buf[_FRAME_LEN];
#endif

#ifdef ANC_ASSIST_AUDIO_DUMP_96K
typedef int16_t        _DUMP_PCM_T;
static _DUMP_PCM_T audio_dump_buf[_FRAME_LEN * _LOOP_CNT];
#endif

#include "cmsis_os.h"

#ifdef APP_ANC_ASSIST_THREAD
#if defined(USE_BASIC_THREADS)
#define ANC_ASSIST_MESSAGE_T APP_MESSAGE_BODY
#else
#define ANC_ASSIST_MESSAGE_T APP_AUDIO_MESSAGE_BODY
#endif

#define ANC_ASSIST_THREAD_STACK_SIZE   (1024 * 2)
static osThreadId anc_assist_thread_tid;
static void app_anc_assist_thread(void const *argument);
osThreadDef(app_anc_assist_thread, osPriorityHigh, 1, ANC_ASSIST_THREAD_STACK_SIZE, "app_anc_assist");

#define ANC_ASSIST_MAILBOX_MAX (20)
static osMailQId anc_assist_mailbox = NULL;
osMailQDef (anc_assist_mailbox, ANC_ASSIST_MAILBOX_MAX, ANC_ASSIST_MESSAGE_T);
#endif

bool ultrasound_process_flag = true;
osMutexId _anc_assist_mutex_id = NULL;
osMutexDef(_anc_assist_mutex);

//dynamic allocation init
static multi_heap_handle_t stream_heap = NULL;

uint32_t app_anc_assist_stream_heap_size(void)
{
#if defined(VOICE_ASSIST_FF_FIR_LMS) || defined(VOICE_ASSIST_FF_IIR_LMS)
    // do not need to count VPU
    uint32_t mic_chan_num = MIC_INDEX_REF + MAX_REF_CHANNEL_NUM;
#else
    uint32_t mic_chan_num = app_anc_get_anc_assist_mic_chan_num();
#endif
    uint32_t anc_assist_buff_size = 0;

    anc_assist_buff_size = MAX_CODEC_CAPTURE_BUF_SIZE / MAX_CHANNEL_NUM * mic_chan_num + 80;//sizeof(struct multi_heap_info) = 44;

#if defined (ANC_ASSIST_UNUSED_ON_PHONE_CALL)
    anc_assist_buff_size += HEAP_BUFF_SIZE;
    anc_assist_buff_size += MAX_CHANNEL_NUM * CAPTURE_BUF_LEN * sizeof(anc_assist_pcm_t);
#endif

    for (uint32_t i = 0; i < ANC_ASSIST_USER_MAX; i++) {
        if ((g_user_algo_handler[i] != NULL) && (g_user_algo_handler[i]->get_heap_buf_size != NULL)) {
            anc_assist_buff_size += g_user_algo_handler[i]->get_heap_buf_size(g_anc_assist_mode);
        }
    }

    return anc_assist_buff_size;
}

static uint32_t stream_heap_init(void)
{
    uint8_t *app_anc_assist_buff_start = NULL;
    uint32_t anc_assist_buff_size = app_anc_assist_stream_heap_size();

    app_anc_assist_buff_start = app_audio_mempool_user_alloc(SYSPOOL_USER_ANC_ASSIST, anc_assist_buff_size);

    stream_heap = heap_register(app_anc_assist_buff_start, anc_assist_buff_size);

    return 0;
}

void *voice_assist_stream_heap_malloc(size_t size)
{
    // ANC_TRACE(0,"[%s] multi_heap_free_size = %d, size - %d", __FUNCTION__, heap_free_size(stream_heap), size);
    void *ptr = heap_malloc(stream_heap, size);
    ASSERT(ptr, "[%s] no memory", __FUNCTION__);

    return ptr;
}

void voice_assist_stream_heap_free(void *ptr)
{
    ASSERT(ptr, "[%s] ptr is null", __FUNCTION__);
    heap_free(stream_heap, ptr);
    // ANC_TRACE(0,"[%s] multi_heap_free_size = %d", __func__, heap_free_size(stream_heap));
}

static void _anc_assist_create_lock(void)
{
    if (_anc_assist_mutex_id == NULL) {
        _anc_assist_mutex_id = osMutexCreate((osMutex(_anc_assist_mutex)));
    }
}

static void _anc_assist_lock(void)
{
    osMutexWait(_anc_assist_mutex_id, osWaitForever);
}

static void _anc_assist_unlock(void)
{
    osMutexRelease(_anc_assist_mutex_id);
}

void app_anc_assist_get_cfg(int *frame_size, int *sample_rate, int *sample_bit, int *ch_num){

    *frame_size = ALGO_FRAME_LEN;
    *sample_rate = ALGO_SAMPLE_RATE;
    *sample_bit = _SAMPLE_BITS;
    *ch_num = MAX_REF_CHANNEL_NUM;
}

int32_t app_anc_assist_set_app_capture_info(uint32_t frame_len, int32_t sample_rate)
{
    g_app_frame_len = frame_len;
    g_app_capture_sample_rate = sample_rate;
    ANC_TRACE(0, "[%s]g_app_frame_len=%d  sample_rate=%d", __func__, g_app_frame_len, sample_rate);

    return 0;
}

static uint32_t _get_mic_ch_map(uint32_t user_status)
{
    uint32_t mic_map = 0;
    for (uint32_t i=0; i<ANC_ASSIST_USER_MAX; i++) {
        if (((user_status & (0x1 << i)) != 0) && (g_user_stream_handler[i] != NULL) && (g_user_stream_handler[i]->get_mic_ch_map != NULL)) {
            mic_map |= g_user_stream_handler[i]->get_mic_ch_map();
        }
    }
    mic_map &= AUD_CHANNEL_MAP_ALL;
    return mic_map;
}

#if defined(ANC_ASSIST_VPU)
static uint32_t _get_vpu_ch_map(uint32_t user_status)
{
    uint32_t vpu_map = 0;
    for (uint32_t i=0; i<ANC_ASSIST_USER_MAX; i++) {
        if (((user_status & (0x1 << i)) != 0) && (g_user_stream_handler[i] != NULL) && (g_user_stream_handler[i]->get_mic_ch_map != NULL)) {
            vpu_map |= g_user_stream_handler[i]->get_mic_ch_map();
        }
    }
    vpu_map &= ~AUD_CHANNEL_MAP_ALL;
    return vpu_map;
}
#endif

static uint32_t POSSIBLY_UNUSED _get_spk_ch_map(uint32_t user_status)
{
    uint32_t spk_ch = 0;

    for (uint32_t i=0; i<ANC_ASSIST_USER_MAX; i++) {
        if (((user_status & (0x1 << i)) != 0) && (g_user_stream_handler[i] != NULL) && (g_user_stream_handler[i]->get_spk_ch_map != NULL)) {
            spk_ch |= g_user_stream_handler[i]->get_spk_ch_map();
        }
    }

    return spk_ch;
}

static int32_t update_stream_cfg(uint32_t user_status)
{
    _anc_assist_lock();
    g_need_open_mic = true;
    g_need_open_spk = true;
    g_loop_cnt      = _LOOP_CNT;
    uint32_t _anc_mic_ch_map = _get_mic_ch_map(user_status);
#ifdef VOICE_ASSIST_WD_ENABLED
    g_sample_rate   = AUD_SAMPRATE_96000;
#elif defined(VOICE_ASSIST_FF_FIR_LMS) || defined(VOICE_ASSIST_FF_IIR_LMS)
    if (app_voice_assist_anc_need_max_sample_rate()) {
        g_sample_rate   = SAMPLE_RATE_MAX;
    } else {
        g_sample_rate   = ALGO_SAMPLE_RATE;
    }
#else
    g_sample_rate   = ALGO_SAMPLE_RATE;
#endif
    enum AUD_IO_PATH_T app_path = AUD_IO_PATH_NULL;

    if (g_anc_assist_mode == ANC_ASSIST_MODE_STANDALONE) {
        ANC_TRACE(0, "[%s] ANC_ASSIST_MODE_STANDALONE", __func__);
    } else if (g_anc_assist_mode == ANC_ASSIST_MODE_PHONE_CALL) {
        ANC_TRACE(0, "[%s] ANC_ASSIST_MODE_PHONE_CALL", __func__);
        g_need_open_mic = false;
        g_loop_cnt = 2;
        g_sample_rate = g_app_capture_sample_rate ;
        g_frame_len = g_app_frame_len;
        app_path = AUD_INPUT_PATH_MAINMIC;
    } else if (g_anc_assist_mode == ANC_ASSIST_MODE_RECORD) {
        ANC_TRACE(0, "[%s] ANC_ASSIST_MODE_RECORD", __func__);
        g_need_open_mic = false;
        // g_need_open_spk = false;
        g_loop_cnt = 1;
        g_sample_rate = AUD_SAMPRATE_16000;
        g_frame_len = g_app_frame_len;
        app_path = AUD_INPUT_PATH_ASRMIC;
    } else if (g_anc_assist_mode == ANC_ASSIST_MODE_MUSIC) {
        ANC_TRACE(0, "[%s] ANC_ASSIST_MODE_MUSIC", __func__);
        g_loop_cnt = 3;
    } else if (g_anc_assist_mode == ANC_ASSIST_MODE_MUSIC_AAC) {
        ANC_TRACE(0, "[%s] ANC_ASSIST_MODE_MUSIC_AAC", __func__);
        g_loop_cnt = 3;
    } else if (g_anc_assist_mode == ANC_ASSIST_MODE_MUSIC_SBC) {
        ANC_TRACE(0, "[%s] ANC_ASSIST_MODE_MUSIC_SBC", __func__);
        g_loop_cnt = 2;
    } else if (g_anc_assist_mode == ANC_ASSIST_MODE_NONE) {
        ANC_TRACE(0, "[%s] ANC_ASSIST_MODE_NONE", __func__);
        g_need_open_mic = false;
        g_need_open_spk = false;
    }else if (g_anc_assist_mode == ANC_ASSIST_MODE_LE_CALL) {
        ANC_TRACE(0, "[%s] ANC_ASSIST_MODE_LE_CALL", __func__);
        g_need_open_mic = false;
        app_path = AUD_INPUT_PATH_MAINMIC;
        g_loop_cnt = 1;
        g_need_open_spk = false;
        g_sample_rate = g_app_capture_sample_rate;
        g_frame_len = g_app_frame_len;

    } else {
        ASSERT(0, "[%s] g_anc_assist_mode(%d) is invalid!", __func__, g_anc_assist_mode);
    }

    if (app_path == AUD_IO_PATH_NULL) {
        anc_assist_mic_update_anc_cfg(_anc_mic_ch_map);
        anc_assist_mic_set_app_cfg(AUD_IO_PATH_NULL);
        anc_assist_mic_parser_index(AUD_INPUT_PATH_ANC_ASSIST);
        g_chan_num = anc_assist_mic_get_ch_num(AUD_INPUT_PATH_ANC_ASSIST);
    } else {
        anc_assist_mic_set_anc_cfg(AUD_INPUT_PATH_ANC_ASSIST);
        anc_assist_mic_set_app_cfg(app_path);
        anc_assist_mic_parser_index(app_path);
        g_chan_num = anc_assist_mic_get_ch_num(app_path);
    }

    if (_get_spk_ch_map(user_status) == 0) {
        g_need_open_spk = false;
    }


#if defined(_LOOP_CNT_FIXED_MAX)
    g_loop_cnt = _LOOP_CNT;
#endif

    if ((g_anc_assist_mode != ANC_ASSIST_MODE_LE_CALL) && (g_anc_assist_mode != ANC_ASSIST_MODE_RECORD)){
        g_frame_len = ALGO_FRAME_LEN * (g_sample_rate / ALGO_SAMPLE_RATE);
    }
    g_capture_buf_size = g_frame_len * sizeof(_PCM_T) * g_chan_num * AF_PINGPONG * g_loop_cnt;
    g_play_buf_size = (MAX_CODEC_PLAY_BUF_SIZE / _LOOP_CNT) * g_loop_cnt;

    ANC_TRACE(0, "[%s] Need to open MIC(%d), SPK(%d)", __func__, g_need_open_mic, g_need_open_spk);
    ANC_TRACE(0, "[%s] fs: %d, chan_num: %d, loop_cnt: %d", __func__, g_sample_rate, g_chan_num, g_loop_cnt);
    _anc_assist_unlock();

    return 0;
}

#ifdef VOICE_ASSIST_WD_ENABLED
static void update_trigger_status(void)
{
    app_anc_assist_trigger_init();

    if (g_mic_open_flag) {
        af_stream_start(STREAM_CAPTURE_ID, AUD_STREAM_CAPTURE);
    }

    if (g_spk_open_flag) {
        af_stream_start(STREAM_PLAY_ID, AUD_STREAM_PLAYBACK);
    }
}
#endif

static int32_t update_codec_status(uint32_t user_status, bool on)
{
    ANC_TRACE(0, "[%s] user_status=%d, on=%d", __func__, user_status, on);
    // MIC
    uint32_t mic_cfg_new = _get_mic_ch_map(user_status);

    // SPK
    uint32_t spk_cfg_new = _get_spk_ch_map(user_status);

    if (on == false) {
        if (spk_cfg_old != spk_cfg_new) {
            if (g_spk_open_flag) {
                _close_spk();
            }
        }

        if (mic_cfg_old != mic_cfg_new) {
            if (g_mic_open_flag) {
                _close_mic();
            }
        }
    } else {
        app_voice_assist_anc_sync_cfg_info();
        if (mic_cfg_old != mic_cfg_new) {
            if (mic_cfg_new) {
                _open_mic();
            }
        }

        if (spk_cfg_old != spk_cfg_new) {
            if (spk_cfg_new) {
                _open_spk();
            }
        }
#ifdef VOICE_ASSIST_WD_ENABLED
        if (mic_cfg_old != mic_cfg_new && spk_cfg_old != spk_cfg_new) {
            if((spk_cfg_new)||(mic_cfg_new))
                update_trigger_status();
        }
#endif
        mic_cfg_old = mic_cfg_new;
        spk_cfg_old = spk_cfg_new;
    }

    return 0;
}

#ifdef APP_ANC_ASSIST_THREAD
static int anc_assist_mailbox_put(ANC_ASSIST_MESSAGE_T *msg_src)
{
    osStatus status = osOK;

    ANC_ASSIST_MESSAGE_T *msg = (ANC_ASSIST_MESSAGE_T *)osMailAlloc(anc_assist_mailbox, 0);

    if (!msg) {
        osEvent evt;
        ANC_TRACE(0, "[ANC_ASSIST] osMailAlloc error dump");
        for (uint8_t i = 0; i < ANC_ASSIST_MAILBOX_MAX; i++)
        {
            evt = osMailGet(anc_assist_mailbox, 0);
            if (evt.status == osEventMail) {
                ANC_TRACE(5, "cnt: %d cmd: %d user: %d open_opt: %d",
                    i,
                    ((ANC_ASSIST_MESSAGE_T *)(evt.value.p))->message_id,
                    ((ANC_ASSIST_MESSAGE_T *)(evt.value.p))->message_Param0,
                    ((ANC_ASSIST_MESSAGE_T *)(evt.value.p))->message_Param1);
            } else {
                ANC_TRACE(2, "cnt:%d %d", i, evt.status);
                break;
            }
        }
        ANC_TRACE(0, "osMailAlloc error dump end");
        ASSERT(0, "[ANC_ASSIST] osMailAlloc error");
    } else {
        memcpy(msg, msg_src, sizeof(ANC_ASSIST_MESSAGE_T));
        status = osMailPut(anc_assist_mailbox, msg);
        if (osOK != status) {
            ANC_TRACE(2, "[%s] WARNING: failed: %d", __func__, status);
        }
    }

    return (int)status;
}

static int anc_assist_mailbox_get(ANC_ASSIST_MESSAGE_T **msg)
{
    int ret = 0;
    osEvent evt;
    evt = osMailGet(anc_assist_mailbox, osWaitForever);

    if (evt.status == osEventMail) {
        *msg = (ANC_ASSIST_MESSAGE_T *)evt.value.p;
    } else {
        ret = -1;
    }

    return ret;
}

static int anc_assist_mailbox_free(ANC_ASSIST_MESSAGE_T *msg)
{
    osStatus status;

    status = osMailFree(anc_assist_mailbox, msg);
    if (osOK != status) {
        ANC_TRACE(2, "[%s] WARNING: failed: %d", __func__, status);
    }

    return (int)status;
}

#endif
static int32_t _assist_open_close_msg_post(anc_assist_user_t user, bool open_opt)
{
    ANC_TRACE(0, "[%s] ", __func__);
#ifdef APP_ANC_ASSIST_THREAD
    ANC_ASSIST_MESSAGE_T msg;
    ANC_TRACE(0, "[%s] g_user: 0x%x, user: %d", __func__, g_user_status, user);

    memset(&msg, 0, sizeof(msg));
    msg.message_id = _ASSIST_MSG_OPEN_CLOSE;
    msg.message_Param0 = user;
    msg.message_Param1 = open_opt;
    anc_assist_mailbox_put(&msg);
#elif defined(USE_BASIC_THREADS)
    APP_MESSAGE_BLOCK msg;
    msg.mod_id = APP_MODULE_VOICE_ASSIST;
    msg.mod_level = APP_MOD_LEVEL_3;
    msg.msg_body.message_id = _ASSIST_MSG_OPEN_CLOSE;
    msg.msg_body.message_Param0 = user;
    msg.msg_body.message_Param1 = open_opt;
    app_mailbox_put(&msg);
#else
    APP_AUDIO_MESSAGE_BLOCK msg;
    msg.mod_id = APP_AUDIO_MODULE_VOICE_ASSIST;
    msg.msg_body.message_id = _ASSIST_MSG_OPEN_CLOSE;
    msg.msg_body.message_Param0 = user;
    msg.msg_body.message_Param1 = open_opt;
    app_audio_mailbox_put(&msg);
#endif

    return 0;
}

static int32_t _assist_user_reset_msg_post(anc_assist_user_t user)
{
    ANC_TRACE(0, "[%s] ", __func__);
#ifdef APP_ANC_ASSIST_THREAD
    ANC_ASSIST_MESSAGE_T msg;
    ANC_TRACE(0, "[%s] g_user: 0x%x, user: %d", __func__, g_user_status, user);

    memset(&msg, 0, sizeof(msg));
    msg.message_id = _ASSIST_MSG_USER_RESET;
    msg.message_Param0 = user;
    anc_assist_mailbox_put(&msg);
#elif defined(USE_BASIC_THREADS)
    APP_MESSAGE_BLOCK msg;
    msg.mod_id = APP_MODULE_VOICE_ASSIST;
    msg.mod_level = APP_MOD_LEVEL_3;
    msg.msg_body.message_id = _ASSIST_MSG_USER_RESET;
    msg.msg_body.message_Param0 = user;
    app_mailbox_put(&msg);
#else
    APP_AUDIO_MESSAGE_BLOCK msg;
    msg.mod_id = APP_AUDIO_MODULE_VOICE_ASSIST;
    msg.msg_body.message_id = _ASSIST_MSG_USER_RESET;
    msg.msg_body.message_Param0 = user;
    app_audio_mailbox_put(&msg);
#endif

    return 0;
}

#if defined(USE_BASIC_THREADS)
static int app_anc_assist_thread_handler(APP_MESSAGE_BODY *msg_body)
#else
static int app_anc_assist_thread_handler(APP_AUDIO_MESSAGE_BODY *msg_body)
#endif
{
    uint32_t id = msg_body->message_id;
    // anc_assist_user_t user = ANC_ASSIST_USER_QTY;

    mic_cfg_old = _get_mic_ch_map(g_user_status);
    spk_cfg_old = _get_spk_ch_map(g_user_status);

    ANC_TRACE(0, "[%s] id :%d", __func__, id);

    switch (id) {
        case _ASSIST_MSG_OPEN_CLOSE: {
            anc_assist_user_t user = (uint32_t)(msg_body->message_Param0);
            bool open_opt = msg_body->message_Param1;
            uint32_t user_status_old = g_user_status;
            uint32_t user_status_new = 0;

            if (open_opt == true) {
                user_status_new = user_status_old | (0x1 << user);
            } else {
                user_status_new = user_status_old & ~(0x1 << user);
            }

            ANC_TRACE(0, "[%s] opt: %d, old: 0x%x, new: 0x%x", __func__, open_opt, user_status_old, user_status_new);

            if (user_status_old != user_status_new) {

                if ((user_status_old == 0) && (user_status_new != 0)) {        // Open
                    update_stream_cfg(user_status_new);
#if defined(ANC_ASSIST_UNUSED_ON_PHONE_CALL)
                    if (g_anc_assist_mode != ANC_ASSIST_MODE_PHONE_CALL) {
                        _anc_assist_open_impl(user, false);
                    }else {
                        need_open_anc_assist = true;
                    }
#else
                    _anc_assist_open_impl(user, false);
#endif
                } else if ((user_status_old != 0) && (user_status_new == 0)) {    // Close
#if defined(ANC_ASSIST_UNUSED_ON_PHONE_CALL)
                    if (g_anc_assist_mode != ANC_ASSIST_MODE_PHONE_CALL) {
                        _anc_assist_close_impl(user, false);
                    }else {
                        need_open_anc_assist = false;
                    }
#else
                    _anc_assist_close_impl(user, false);
#endif
                } else {    // Update
                    update_codec_status(user_status_new, false);
                    update_stream_cfg(user_status_new);
                    if (open_opt) {
                        if ((user <= ANC_ASSIST_USER_MAX) && (g_user_algo_handler[user] != NULL) && (g_user_algo_handler[user]->open != NULL)) {
                            g_user_algo_handler[user]->open();
                        }
                    } else {
                        if ((user <= ANC_ASSIST_USER_MAX) && (g_user_algo_handler[user] != NULL) && (g_user_algo_handler[user]->close != NULL)) {
                            g_user_algo_handler[user]->close();
                        }
                    }
                    update_codec_status(user_status_new, true);
                    _anc_assist_update_sysfreq();
                }

                // Save parameters
                g_user_status = user_status_new;
            }
            break;
        }
        case _ASSIST_MSG_SET_MODE:
            app_anc_assist_direct_set_mode(msg_body->message_Param0);
            osSignalSet(msg_body->thread_id, ANC_ASSIST_SIGNAL_SET_MODE);
            osThreadYield();
            break;
        case  _ASSIST_MSG_USER_RESET: {
              anc_assist_user_t user = (uint32_t)(msg_body->message_Param0);
              if ((user <= ANC_ASSIST_USER_MAX) && (g_user_algo_handler[user] != NULL) && (g_user_algo_handler[user]->reset != NULL)) {
                  g_user_algo_handler[user]->reset();
              }
        }
            break;
        default:
            ASSERT(0, "[%s] id(%d) is invalid", __func__, id);
            break;
    }

    return 0;
}

#ifdef APP_ANC_ASSIST_THREAD
static void app_anc_assist_thread(void const *argument)
{
    ANC_ASSIST_MESSAGE_T *msg_p = NULL;

    while(1) {
        if (!anc_assist_mailbox_get(&msg_p)) {
            app_anc_assist_thread_handler(msg_p);
            anc_assist_mailbox_free(msg_p);
        }
    }
}
#endif
int32_t app_anc_assist_init(void)
{
    g_opened_flag = false;
    g_mode_switching_flag = false;
    g_anc_assist_mode = ANC_ASSIST_MODE_STANDALONE;

    g_user_status = 0;

    for (uint32_t i = 0; i<ANC_ASSIST_USER_MAX; i++) {
        g_user_algo_handler[i] = NULL;
    }

    _anc_assist_create_lock();
    anc_assist_mic_reset();
#ifdef APP_ANC_ASSIST_THREAD
    if (anc_assist_mailbox != NULL) {
        ANC_TRACE(1, "[%s] Mailbox is not NULL. Cnt: %d", __func__, osMailGetCount(anc_assist_mailbox));
        anc_assist_mailbox = NULL;
    }

    anc_assist_mailbox = osMailCreate(osMailQ(anc_assist_mailbox), NULL);
    ASSERT(anc_assist_mailbox != NULL, "[%s] Can not create mailbox", __func__);

    if (anc_assist_thread_tid != NULL) {
        ANC_TRACE(1, "[%s] Thread is not NULL", __func__);
        osStatus status;
        status = osThreadTerminate(anc_assist_thread_tid);
        ASSERT(status == osOK, "[%s] Can not create thread. status: %d", __func__, status);
        anc_assist_thread_tid = NULL;
    }
    anc_assist_thread_tid = osThreadCreate(osThread(app_anc_assist_thread), NULL);
    ASSERT(anc_assist_thread_tid != NULL, "[%s] Can not create thread", __func__);
#elif defined(USE_BASIC_THREADS)
    app_set_threadhandle(APP_MODULE_VOICE_ASSIST, app_anc_assist_thread_handler);
#else
    app_audio_set_threadhandle(APP_AUDIO_MODULE_VOICE_ASSIST, app_anc_assist_thread_handler);
#endif
#ifdef ANC_ASSIST_PROCESS_THREAD
    anc_assist_thread_open();
#endif
    return 0;
}

int32_t app_anc_assist_deinit(void)
{
#if defined(USE_BASIC_THREADS)
    app_set_threadhandle(APP_MODULE_VOICE_ASSIST, NULL);
#else
    app_audio_set_threadhandle(APP_AUDIO_MODULE_VOICE_ASSIST, NULL);
#endif
    return 0;
}

uint8_t *voice_assist_get_user_ptr(const uint8_t *name)
{
    for (uint8_t i=0; i<ANC_ASSIST_USER_MAX; i++) {
        if (strcmp((const char *)init_user_name[i].user_name, (const char *)name) == 0) {
            return init_user_name[i].user_ptr;
        }
    }

    return NULL;
}

int32_t app_voice_assist_dsp_register(uint32_t *user, const uint8_t *name, voice_assist_user_register_cfg* voice_assist_callback)
{
    ANC_TRACE(0, "[%s] user: %d", __func__, *user);
    g_user_algo_handler[*user] = voice_assist_callback->assist_algo_dsp;

    return 0;
}

voice_assist_user_info_t *app_anc_assist_get_user_index(void)
{
    return init_user_name;
}

int32_t app_voice_assist_stream_register(const uint8_t* user_name, voice_assist_user_register_cfg* cfg, uint8_t *user_ptr)
{
    static uint32_t init_index = 0;
    init_user_name[init_index].user_name = user_name;
    init_user_name[init_index].user_ptr = user_ptr;
    g_user_stream_handler[init_index] = cfg->voice_assist_callback;

    ANC_TRACE(0,"[%s] i = %d user_name = %s", __func__, init_index, init_user_name[init_index].user_name);

    return init_index++;;
}

app_voice_assist_sync_info_t app_anc_assist_get_user_sync_handler(anc_assist_user_t user)
{
    ASSERT(user < ANC_ASSIST_USER_MAX, "[%s] user (%d) not suppotted", __FUNCTION__, user);
    return g_user_stream_handler[user]->sync_info;
}

// TODO: Currently, just used by sco. Can extend
uint32_t app_anc_assist_get_mic_ch_num(enum AUD_IO_PATH_T path)
{
    return anc_assist_mic_get_ch_num(path);
}

uint32_t app_anc_assist_get_mic_ch_map(enum AUD_IO_PATH_T path)
{
    return anc_assist_mic_get_cfg(path);
}

int32_t app_anc_assist_parser_app_mic_buf(void *buf, uint32_t *len)
{
    _anc_assist_lock();
    anc_assist_mic_parser_app_buf(buf, len);
    _anc_assist_unlock();

    return 0;
}

static bool _need_switch_mode(anc_assist_mode_t old_mode, anc_assist_mode_t new_mode)
{
    if ((old_mode == ANC_ASSIST_MODE_STANDALONE)    &&
        (new_mode == ANC_ASSIST_MODE_MUSIC)) {
            return false;
    }

    if ((old_mode == ANC_ASSIST_MODE_MUSIC)         &&
        (new_mode == ANC_ASSIST_MODE_STANDALONE)) {
            return false;
    }

    if ((old_mode == ANC_ASSIST_MODE_STANDALONE)    &&
        (new_mode == ANC_ASSIST_MODE_MUSIC_AAC)     &&
        (g_loop_cnt == 3)) {
            return false;
    }

    if ((old_mode == ANC_ASSIST_MODE_STANDALONE)    &&
        (new_mode == ANC_ASSIST_MODE_MUSIC_SBC)     &&
        (g_loop_cnt == 2)) {
            return false;
    }

    if ((old_mode == ANC_ASSIST_MODE_MUSIC_AAC)     &&
        (new_mode == ANC_ASSIST_MODE_STANDALONE)) {
            return false;
    }

    if ((old_mode == ANC_ASSIST_MODE_MUSIC_SBC)     &&
        (new_mode == ANC_ASSIST_MODE_STANDALONE)) {
            return false;
    }

    return true;
}

static int32_t app_anc_assist_direct_set_mode(anc_assist_mode_t mode)
{
    ANC_TRACE(0, "[%s] %d --> %d", __func__, g_anc_assist_mode, mode);

    if (g_anc_assist_mode == mode) {
        ANC_TRACE(0, "[%s] WARNING: Same mode = %d", __func__, mode);
        return 1;
    }

    g_anc_assist_mode = mode;
    if (g_opened_flag) {
        ANC_TRACE(0, "[%s] ------ START ------", __func__);
        _anc_assist_close_impl(ANC_ASSIST_USER_MAX, true);
        update_stream_cfg(g_user_status);
#if defined(ANC_ASSIST_UNUSED_ON_PHONE_CALL)
        if (g_anc_assist_mode != ANC_ASSIST_MODE_PHONE_CALL){
            _anc_assist_open_impl(ANC_ASSIST_USER_MAX, true);
        }else {
            need_open_anc_assist = true;
        }
#else
        _anc_assist_open_impl(ANC_ASSIST_USER_MAX, true);
#endif
        ANC_TRACE(0, "[%s] ------ END ------", __func__);
    } else {
        update_stream_cfg(g_user_status);
#if defined(ANC_ASSIST_UNUSED_ON_PHONE_CALL)
        if (need_open_anc_assist) {
            _anc_assist_open_impl(ANC_ASSIST_USER_MAX, true);
            need_open_anc_assist = false;
        }
#endif
    }

    return 0;
}

static POSSIBLY_UNUSED int32_t _assist_set_mode_msg_post(anc_assist_mode_t mode)
{
    ANC_TRACE(0, "[%s] ", __func__);
#ifdef APP_ANC_ASSIST_THREAD
    ANC_ASSIST_MESSAGE_T msg;
    ANC_TRACE(0, "[%s] mode %d", __func__, mode);

    memset(&msg, 0, sizeof(msg));
    msg.message_id = _ASSIST_MSG_SET_MODE;
    msg.message_Param0 = mode;
    msg.thread_id = osThreadGetId();;
    anc_assist_mailbox_put(&msg);
#elif defined(USE_BASIC_THREADS)
    APP_MESSAGE_BLOCK msg;
    msg.mod_id = APP_MODULE_VOICE_ASSIST;
    msg.mod_level = APP_MOD_LEVEL_3;
    msg.msg_body.message_id = _ASSIST_MSG_SET_MODE;
    msg.msg_body.message_Param0 = mode;
    msg.msg_body.thread_id = osThreadGetId();;
    app_mailbox_put(&msg);
#else
    APP_AUDIO_MESSAGE_BLOCK msg;
    msg.mod_id = APP_AUDIO_MODULE_VOICE_ASSIST;
    msg.msg_body.message_id = _ASSIST_MSG_SET_MODE;
    msg.msg_body.message_Param0 = mode;
    msg.msg_body.thread_id = osThreadGetId();;
    app_audio_mailbox_put(&msg);
#endif

    return 0;
}

int32_t app_anc_assist_set_mode(anc_assist_mode_t mode)
{
    if (g_anc_assist_mode == mode) {
        ANC_TRACE(0, "[%s] WARNING: Same mode = %d", __func__, mode);
        return 1;
    }

    if (_need_switch_mode(g_anc_assist_mode, mode) == false) {
        g_anc_assist_mode = mode;
#ifdef ANC_ASSIST_UPDATE_SYSFREQ
        if (g_opened_flag) {
            _anc_assist_update_sysfreq();
        }
#endif
        return 0;
    }

    ANC_TRACE(1, "[%s] Start", __FUNCTION__);
    g_mode_switching_flag = true;
#ifdef APP_ANC_ASSIST_THREAD
    osEvent evt;
    osSignalClear(osThreadGetId(), ANC_ASSIST_SIGNAL_SET_MODE);
    _assist_set_mode_msg_post(mode);

    evt = osSignalWait(ANC_ASSIST_SIGNAL_SET_MODE, 2000);
    if (evt.status == osEventSignal) {
        if (evt.value.signals & ANC_ASSIST_SIGNAL_SET_MODE) {
            ANC_TRACE(1, "[%s] OK", __FUNCTION__);
        }
    } else if (evt.status == osEventTimeout) {
        ANC_TRACE(1, "[%s] WARNING: Timeout", __FUNCTION__);
        app_anc_assist_direct_set_mode(mode);
    }
#else
    app_anc_assist_direct_set_mode(mode);
#endif
    g_mode_switching_flag = false;

    return 0;
}

anc_assist_mode_t app_anc_assist_get_mode(void)
{
    return g_anc_assist_mode;
}

int32_t app_anc_assist_set_playback_info(int32_t sample_rate)
{
#if defined(ANC_ASSIST_USE_INT_CODEC)
    g_play_sample_rate = (sample_rate == 0) ? _PLAY_SAMPLE_RATE : sample_rate;
#else
    ANC_TRACE(1, "[%s] No need to set playback info when use DAC2", __FUNCTION__);
#endif
    return 0;
}

int32_t app_anc_assist_get_play_sample_rate(void)
{
    return g_play_sample_rate;
}


int32_t app_anc_assist_is_runing(void)
{
    return g_opened_flag;
}

extern AncAssistState *anc_assist_set_st_def(custom_allocator *allocator);
static int32_t _anc_assist_open_impl(anc_assist_user_t user, bool need_set_mode)
{
    ANC_TRACE(0, "[%s] ...", __func__);

    if (g_opened_flag == true) {
        ANC_TRACE(0, "[%s] WARNING: g_opened_flag is true", __func__);
        return 1;
    }

    // Temporary
    app_sysfreq_req(APP_SYSFREQ_USER_VOICE_ASSIST, HAL_CMU_FREQ_104M);
    if (g_need_open_mic) {
        stream_heap_init();
    }

    if (need_set_mode == false) {
        if ((g_user_algo_handler[user] != NULL) && (g_user_algo_handler[user]->open != NULL)) {
            g_user_algo_handler[user]->open();
        }
    } else {
        for (int i = 0; i < ANC_ASSIST_USER_MAX; i++) {
            if ((g_user_status & (0x1 << i)) && (g_user_algo_handler[i] != NULL) && (g_user_algo_handler[i]->set_mode != NULL)) {
                g_user_algo_handler[i]->set_mode(ANC_ASSIST_SET_MODE_ON);
            }
        }
    }

#if defined(ANC_ASSIST_UNUSED_ON_PHONE_CALL)
    //voice_assist get buff for syspool
    voice_assist_heap_buff = voice_assist_stream_heap_malloc(HEAP_BUFF_SIZE);
    for (uint8_t ch = 0; ch < MAX_CHANNEL_NUM; ch++) {
        capture_buf[ch] = voice_assist_stream_heap_malloc(CAPTURE_BUF_LEN * sizeof(anc_assist_pcm_t));
    }
#endif

    for (uint8_t i = 0; i < MAX_FF_CHANNEL_NUM; i++) {
        g_ff_mic_buf[i] = capture_buf[MIC_INDEX_FF + i];
    }
    for (uint8_t i = 0; i < MAX_FB_CHANNEL_NUM; i++) {
        g_fb_mic_buf[i] = capture_buf[MIC_INDEX_FB + i];
    }
    for (uint8_t i = 0; i < MAX_TALK_CHANNEL_NUM; i++) {
        g_talk_mic_buf[i] = capture_buf[MIC_INDEX_TALK + i];
    }
    for (uint8_t i = 0; i < MAX_REF_CHANNEL_NUM; i++) {
        g_ref_mic_buf[i] = capture_buf[MIC_INDEX_REF + i];
    }

    _anc_assist_lock();
    ext_heap_init(voice_assist_heap_buff);

#ifdef VOICE_ASSIST_DC_FILTER
    EqConfig eq_cfg = {
        .bypass = 0,
        .gain = 0.f,
        .num = 1,
        .params = {
            {IIR_BIQUARD_HPF, {{20, 0, 0.707}}},
        },
    };
    for (uint32_t i = 0; i < 2; i++) {
        dc_filter[i] = eq_init_with_custom_allocator(ALGO_SAMPLE_RATE, ALGO_FRAME_LEN, &eq_cfg, &ext_allocator);
    }
#endif

#ifdef ANC_ASSIST_AUDIO_DUMP
    audio_dump_init(ALGO_FRAME_LEN, sizeof(_DUMP_PCM_T), 4);
#endif

#ifdef ANC_ASSIST_AUDIO_DUMP_32K
    audio_dump_init(_FRAME_LEN, sizeof(_DUMP_PCM_T), 4);
#endif

#ifdef ANC_ASSIST_AUDIO_DUMP_96K
    // audio_dump_init(g_frame_len * g_loop_cnt, sizeof(_DUMP_PCM_T), 2);

    audio_dump_init(193, sizeof(_DUMP_PCM_T), 2);
#endif

#ifdef ANC_APP
    anc_assist_anc_init();
    anc_assist_tws_sync_init(ALGO_FRAME_MS);
#endif

#if defined(ANC_ASSIST_RESAMPLE_ENABLE)
    anc_assist_resample_init(g_sample_rate, g_frame_len, &ext_allocator);
#endif

    g_opened_flag = true;
    _anc_assist_unlock();

    _open_mic();
    _open_spk();

#ifdef VOICE_ASSIST_WD_ENABLED
    update_trigger_status();
#endif

#ifdef ANC_ASSIST_UPDATE_SYSFREQ
    _anc_assist_update_sysfreq();
#else
    g_sys_freq = APP_SYSFREQ_26M;
    app_sysfreq_req(APP_SYSFREQ_USER_VOICE_ASSIST, g_sys_freq);

#if defined(ENABLE_CALCU_CPU_FREQ_LOG)
    ANC_TRACE(0, "[%s] Sys freq[%d]: %d", __func__, g_sys_freq, hal_sys_timer_calc_cpu_freq(5, 0));
#endif
#endif

    return 0;
}

static int32_t _anc_assist_close_impl(anc_assist_user_t user, bool need_set_mode)
{
    ANC_TRACE(0, "[%s] ...", __func__);

    if (g_opened_flag == false) {
        ANC_TRACE(0, "[%s] WARNING: g_opened_flag is false", __func__);
        return 1;
    }

    // Temporary
    app_sysfreq_req(APP_SYSFREQ_USER_VOICE_ASSIST, HAL_CMU_FREQ_104M);

    _close_spk();
    _close_mic();

#ifdef VOICE_ASSIST_WD_ENABLED
    app_anc_assist_trigger_deinit();
#endif

#ifdef VOICE_ASSIST_DC_FILTER
    for (uint32_t i = 0; i < 2; i++) {
        eq_destroy(dc_filter[i]);
    }
#endif

    _anc_assist_lock();
    g_opened_flag = false;

#ifdef ANC_APP
    anc_assist_anc_deinit();
#endif

#if defined(ANC_ASSIST_RESAMPLE_ENABLE)
    anc_assist_resample_deinit(g_sample_rate);
#endif

    //attention : shoule check,set mode dont reset user
    if (need_set_mode == false) {
        if ((g_user_algo_handler[user] != NULL) && (g_user_algo_handler[user]->close != NULL)) {
            g_user_algo_handler[user]->close();
        }
    } else {
        for (int i = 0; i < ANC_ASSIST_USER_MAX; i++) {
            if ((g_user_status & (0x1 << i)) && (g_user_algo_handler[i] != NULL) && (g_user_algo_handler[i]->set_mode != NULL)) {
                g_user_algo_handler[i]->set_mode(ANC_ASSIST_SET_MODE_OFF);
            }
        }
    }

#if defined(ANC_ASSIST_UNUSED_ON_PHONE_CALL)
    voice_assist_stream_heap_free(voice_assist_heap_buff);
    for (uint8_t ch = 0; ch < MAX_CHANNEL_NUM; ch++) {
        voice_assist_stream_heap_free(capture_buf[ch]);
    }
#endif
    ext_heap_deinit();
    app_audio_mempool_user_free(SYSPOOL_USER_ANC_ASSIST);
	_anc_assist_unlock();

    g_sys_freq = APP_SYSFREQ_32K;
    app_sysfreq_req(APP_SYSFREQ_USER_VOICE_ASSIST, g_sys_freq);

#if defined(ENABLE_CALCU_CPU_FREQ_LOG)
    ANC_TRACE(0, "[%s] Sys freq[%d]: %d", __func__, g_sys_freq, hal_sys_timer_calc_cpu_freq(5, 0));
#endif

    return 0;
}

int32_t app_anc_assist_open(anc_assist_user_t user)
{
    ANC_TRACE(0, "[%s] g_user: 0x%x, user: %d", __func__, g_user_status, user);

    _assist_open_close_msg_post(user, true);

    return 0;
}

int32_t app_anc_assist_close(anc_assist_user_t user)
{
    ANC_TRACE(0, "[%s] g_user: 0x%x, user: %d", __func__, g_user_status, user);

    _assist_open_close_msg_post(user, false);

    return 0;
}

int32_t app_anc_assist_reset(anc_assist_user_t user)
{
    ANC_TRACE(0, "[%s] g_user: 0x%x, user: %d", __func__, g_user_status, user);

    if ((g_user_status & (0x1 << user)) == false) {
        ANC_TRACE(0, "[%s] skip close", __FUNCTION__);
        return 0;
    }

    _assist_user_reset_msg_post(user);

    return 0;
}

int32_t POSSIBLY_UNUSED  _process_frame_32k(anc_assist_pcm_t **ff_mic, uint8_t ff_ch_num,
                       anc_assist_pcm_t **fb_mic, uint8_t fb_ch_num,
                       anc_assist_pcm_t **talk_mic, uint8_t talk_ch_num,
                       anc_assist_pcm_t **ref, uint8_t ref_ch_num,
                       uint32_t frame_len)
{
    voice_assist_process_frame_data_t process_frame_date;
    process_frame_date.ff_mic = ff_mic;
    process_frame_date.ff_ch_num = ff_ch_num;
    process_frame_date.fb_mic = fb_mic;
    process_frame_date.fb_ch_num = fb_ch_num;
    process_frame_date.talk_mic = talk_mic;
    process_frame_date.talk_ch_num = talk_ch_num;
    process_frame_date.ref = ref;
    process_frame_date.ref_ch_num = ref_ch_num;
    process_frame_date.frame_len = frame_len;

    if (g_anc_assist_mode == ANC_ASSIST_MODE_RECORD) {
        return 0;
    }

    for (uint16_t i=0; i<ANC_ASSIST_USER_MAX; i++) {
        if (((g_user_status & (0x1 << i)) != 0) && (g_user_algo_handler[i] != NULL) && (g_user_algo_handler[i]->cap_process != NULL)) {
            if ((g_user_algo_handler[i]->get_fs != NULL) && (g_user_algo_handler[i]->get_fs() == 32000)) {
                g_user_algo_handler[i]->cap_process(&process_frame_date);
            }
        }
    }
    return 0;
}

int32_t POSSIBLY_UNUSED  _process_frame(anc_assist_pcm_t **ff_mic, uint8_t ff_ch_num,
                       anc_assist_pcm_t **fb_mic, uint8_t fb_ch_num,
                       anc_assist_pcm_t **talk_mic, uint8_t talk_ch_num,
                       anc_assist_pcm_t **ref, uint8_t ref_ch_num,
                       anc_assist_pcm_t *vpu_mic,
                       uint32_t frame_len)
{
#ifdef ANC_ASSIST_AUDIO_DUMP
    uint32_t dump_ch = 0;
    audio_dump_clear_up();

    // TODO: Use capture buf
    for (uint32_t i=0; i<ALGO_FRAME_LEN; i++) {
        audio_dump_buf[i] = (_PCM_T)ff_mic[0][i] >> RIGHT_SHIFT;
    }
    audio_dump_add_channel_data(dump_ch++, audio_dump_buf, ALGO_FRAME_LEN);

    for (uint32_t i=0; i<ALGO_FRAME_LEN; i++) {
        audio_dump_buf[i] = (_PCM_T)fb_mic[0][i] >> RIGHT_SHIFT;
    }
    audio_dump_add_channel_data(dump_ch++, audio_dump_buf, ALGO_FRAME_LEN);

    for (uint32_t i=0; i<ALGO_FRAME_LEN; i++) {
        audio_dump_buf[i] = (_PCM_T)talk_mic[0][i] >> RIGHT_SHIFT;
    }
    audio_dump_add_channel_data(dump_ch++, audio_dump_buf, ALGO_FRAME_LEN);

    for (uint32_t i=0; i<ALGO_FRAME_LEN; i++) {
        audio_dump_buf[i] = (_PCM_T)ref[0][i] >> RIGHT_SHIFT;
    }
    audio_dump_add_channel_data(dump_ch++, audio_dump_buf, ALGO_FRAME_LEN);

    audio_dump_run();
#endif

    voice_assist_process_frame_data_t process_frame_date;
    process_frame_date.ff_mic = ff_mic;
    process_frame_date.ff_ch_num = ff_ch_num;
    process_frame_date.fb_mic = fb_mic;
    process_frame_date.fb_ch_num = fb_ch_num;
    process_frame_date.talk_mic = talk_mic;
    process_frame_date.talk_ch_num = talk_ch_num;
    process_frame_date.ref = ref;
    process_frame_date.ref_ch_num = ref_ch_num;
    process_frame_date.vpu_mic = vpu_mic;
    process_frame_date.frame_len = frame_len;

#ifdef VOICE_ASSIST_DC_FILTER
    eq_process_float(dc_filter[0], ff_mic[0], ALGO_FRAME_LEN);
    eq_process_float(dc_filter[1], talk_mic[0], ALGO_FRAME_LEN);
#endif

    for (uint16_t i=0; i<ANC_ASSIST_USER_MAX; i++) {
        if (((g_user_status & (0x1 << i)) != 0) && (g_user_algo_handler[i] != NULL) && (g_user_algo_handler[i]->cap_process != NULL)) {
            if ((g_user_algo_handler[i]->get_fs != NULL) && (g_user_algo_handler[i]->get_fs() == 16000)) {
                g_user_algo_handler[i]->cap_process(&process_frame_date);
            }
        }
    }

#ifdef ANC_APP
    anc_assist_tws_sync_heartbeat();
#endif

    return 0;
}

static int32_t app_anc_assist_process_impl(bool pcm_interval, void *buf, uint32_t len)
{
    _anc_assist_lock();

    if (!g_opened_flag) {
        ANC_TRACE(1, "[%s] WARNING: g_opened_flag is false", __func__);
        _anc_assist_unlock();
        return -1;
    }
    if (g_mode_switching_flag) {
        ANC_TRACE(1, "[%s] mode is switching", __func__);
        _anc_assist_unlock();
        return -1;
    }

    const bool is_call =
        (g_anc_assist_mode == ANC_ASSIST_MODE_PHONE_CALL) ||
        (g_anc_assist_mode == ANC_ASSIST_MODE_LE_CALL);

#ifdef ANC_ASSIST_16BIT
    #define ANC_ASSIST_PCM_BYTES  sizeof(int16_t)
#else
    #define ANC_ASSIST_PCM_BYTES  sizeof(int32_t)
#endif

    uint32_t pcm_bytes = is_call ? sizeof(int32_t) : ANC_ASSIST_PCM_BYTES;
    enum AUD_IO_PATH_T path = is_call ? AUD_INPUT_PATH_MAINMIC : AUD_INPUT_PATH_ANC_ASSIST;

    uint32_t pcm_len = len / pcm_bytes;
    uint32_t frame_len = pcm_len / g_chan_num;

    anc_assist_mic_parser_anc_buf(pcm_interval, path, (anc_assist_pcm_t *)capture_buf,
                                        CAPTURE_BUF_LEN, buf, pcm_len, pcm_bytes);

#ifndef ANC_ASSIST_PROCESS_THREAD
    uint32_t offset = 0;
    uint32_t loop_cnt = frame_len / ALGO_FRAME_LEN;
#endif

#ifdef ANC_ASSIST_AUDIO_DUMP_96K
    uint32_t dump_ch = 0;
    audio_dump_clear_up();
    int offseti = 105;

    for (uint32_t i = offseti,j = 0; i < offseti + 193; i++,j++) {

        audio_dump_buf[j] = (_PCM_T)g_fb_mic_buf[0][i] >> RIGHT_SHIFT;
    }
    audio_dump_add_channel_data(dump_ch++, audio_dump_buf, 193);

    for (uint32_t i=offseti, j=0; i<193+offseti; i++, j++) {
        audio_dump_buf[j] = (_PCM_T)g_ref_mic_buf[i] >> RIGHT_SHIFT;
    }
    audio_dump_add_channel_data(dump_ch++, audio_dump_buf, 193);

    audio_dump_run();
#endif

#if defined(ANC_ASSIST_RESAMPLE_ENABLE)
    if (g_sample_rate != ALGO_SAMPLE_RATE) {
        ASSERT(g_sample_rate == SAMPLE_RATE_MAX, "[%s] g_sample_rate(%d) is invalid!", __func__, g_sample_rate);

        for (uint32_t cnt = 0, offset = 0; cnt < g_loop_cnt; cnt++, offset += _FRAME_LEN) {
#ifdef ANC_ASSIST_AUDIO_DUMP_32K
            uint32_t dump_ch = 0;
            audio_dump_clear_up();

            // TODO: Use capture buf
            for (uint32_t i=0; i<_FRAME_LEN; i++) {
                audio_dump_buf[i] = (_PCM_T) (g_ff_mic_buf[0] + offset)[i] >> RIGHT_SHIFT;
            }
            audio_dump_add_channel_data(dump_ch++, audio_dump_buf, _FRAME_LEN);

            for (uint32_t i=0; i<_FRAME_LEN; i++) {
                audio_dump_buf[i] = (_PCM_T)(g_fb_mic_buf[0] + offset)[i] >> RIGHT_SHIFT;
            }
            audio_dump_add_channel_data(dump_ch++, audio_dump_buf, _FRAME_LEN);

            for (uint32_t i=0; i<_FRAME_LEN; i++) {
                audio_dump_buf[i] = (_PCM_T)(g_ref_mic_buf[0] + offset)[i] >> RIGHT_SHIFT;
            }
            audio_dump_add_channel_data(dump_ch++, audio_dump_buf, _FRAME_LEN);

            for (uint32_t i=0; i<_FRAME_LEN; i++) {
                audio_dump_buf[i] = (_PCM_T)(g_ref_mic_buf[1] + offset)[i] >> RIGHT_SHIFT;
            }
            audio_dump_add_channel_data(dump_ch++, audio_dump_buf, _FRAME_LEN);
#endif
        anc_assist_pcm_t *ff_mic_buf[MAX_FF_CHANNEL_NUM];
        anc_assist_pcm_t *fb_mic_buf[MAX_FB_CHANNEL_NUM];
        anc_assist_pcm_t *talk_mic_buf[MAX_TALK_CHANNEL_NUM];
        anc_assist_pcm_t *ref_mic_buf[MAX_REF_CHANNEL_NUM];

        for (uint8_t i = 0; i < MAX_FF_CHANNEL_NUM; i++) {
            ff_mic_buf[i] = g_ff_mic_buf[i] + offset;
        }

        for (uint8_t i = 0; i < MAX_FB_CHANNEL_NUM; i++) {
            fb_mic_buf[i] = g_fb_mic_buf[i] + offset;
        }

        for (uint8_t i = 0; i < MAX_TALK_CHANNEL_NUM; i++) {
            talk_mic_buf[i] = g_talk_mic_buf[i] + offset;
        }

        for (uint8_t i = 0; i < MAX_REF_CHANNEL_NUM; i++) {
            ref_mic_buf[i] = g_ref_mic_buf[i] + offset;
        }

        _process_frame_32k(ff_mic_buf, MAX_FF_CHANNEL_NUM,
                           fb_mic_buf, MAX_FB_CHANNEL_NUM,
                           talk_mic_buf, MAX_TALK_CHANNEL_NUM,
                           ref_mic_buf, MAX_REF_CHANNEL_NUM,
                           _FRAME_LEN);

#ifdef ANC_ASSIST_AUDIO_DUMP_32K
            audio_dump_run();
#endif
        }

#ifdef ANC_ASSIST_16BIT
        anc_assist_resample_process_q16((anc_assist_pcm_t *)capture_buf, CAPTURE_BUF_LEN, frame_len);
#else
        anc_assist_resample_process((anc_assist_pcm_t *)capture_buf, CAPTURE_BUF_LEN, frame_len);
#endif

#ifndef ANC_ASSIST_PROCESS_THREAD
        loop_cnt /= (g_sample_rate / ALGO_SAMPLE_RATE);
#endif
        frame_len /= (g_sample_rate / ALGO_SAMPLE_RATE);
    }
#endif
    _anc_assist_unlock();

#if defined(ANC_ASSIST_PROCESS_THREAD)
    float *in_buf[MIC_INDEX_QTY] = {NULL};
    for (uint8_t i = 0; i < MAX_FF_CHANNEL_NUM; i++) {
        in_buf[MIC_INDEX_FF + i] = g_ff_mic_buf[i];
    }

    for (uint8_t i = 0; i < MAX_FB_CHANNEL_NUM ; i++) {
        in_buf[MIC_INDEX_FB + i] = g_fb_mic_buf[i];
    }

    for (uint8_t i = 0; i < MAX_TALK_CHANNEL_NUM ; i++) {
        in_buf[MIC_INDEX_TALK + i] = g_talk_mic_buf[i];
    }

    for (uint8_t i = 0; i < MAX_REF_CHANNEL_NUM ; i++) {
        in_buf[MIC_INDEX_REF + i] = g_ref_mic_buf[i];
    }
    anc_assist_thread_capture_process(in_buf, frame_len);
#else
    for (uint32_t cnt = 0; cnt < loop_cnt; cnt++) {
        anc_assist_pcm_t *ff_mic_buf[MAX_FF_CHANNEL_NUM];
        anc_assist_pcm_t *fb_mic_buf[MAX_FB_CHANNEL_NUM];
        anc_assist_pcm_t *talk_mic_buf[MAX_TALK_CHANNEL_NUM];
        anc_assist_pcm_t *ref_mic_buf[MAX_REF_CHANNEL_NUM];

        for (uint8_t i = 0; i < MAX_FF_CHANNEL_NUM; i++) {
            ff_mic_buf[i] = g_ff_mic_buf[i] + offset;
        }

        for (uint8_t i = 0; i < MAX_FB_CHANNEL_NUM; i++) {
            fb_mic_buf[i] = g_fb_mic_buf[i] + offset;
        }

        for (uint8_t i = 0; i < MAX_TALK_CHANNEL_NUM; i++) {
            talk_mic_buf[i] = g_talk_mic_buf[i] + offset;
        }
        for (uint8_t i = 0; i < MAX_REF_CHANNEL_NUM; i++) {
            ref_mic_buf[i] = g_ref_mic_buf[i] + offset;
        }
#if defined(ANC_ASSIST_VPU)
        anc_assist_pcm_t *vpu_mic_buf = g_vpu_mic_buf + offset;
#else
        anc_assist_pcm_t *vpu_mic_buf = NULL;
#endif
            _process_frame(ff_mic_buf, MAX_FF_CHANNEL_NUM,
                    fb_mic_buf, MAX_FB_CHANNEL_NUM,
                    talk_mic_buf, MAX_TALK_CHANNEL_NUM,
                    ref_mic_buf, MAX_REF_CHANNEL_NUM,
                    vpu_mic_buf,
                    ALGO_FRAME_LEN);
        offset += ALGO_FRAME_LEN;
    }
#endif

    return 0;
}

int32_t app_anc_assist_process(void *buf, uint32_t len)
{
    app_anc_assist_process_impl(false, buf, len);

    return 0;
}

int32_t app_anc_assist_process_interval(void *buf, uint32_t len)
{
    app_anc_assist_process_impl(true, buf, len);

    return 0;
}

static uint32_t codec_capture_stream_callback(uint8_t *buf, uint32_t len)
{
#ifdef ANC_ASSIST_UPDATE_SYSFREQ
    _anc_assist_update_sysfreq();
#endif
    app_anc_assist_process(buf, len);
#ifdef ANC_ASSIST_UPDATE_SYSFREQ
    _anc_assist_reset_sysfreq();
#endif
    return len;
}


static void _open_mic(void)
{
    if (g_mic_open_flag == true) {
        ANC_TRACE(0, "[%s] WARNING: MIC is opened", __func__);
        return;
    }

    if (g_need_open_mic) {
        af_set_priority(AF_USER_VOICE_ASSIST, osPriorityHigh);
        codec_capture_buf = voice_assist_stream_heap_malloc(g_capture_buf_size);

        struct AF_STREAM_CONFIG_T stream_cfg;

        memset(&stream_cfg, 0, sizeof(stream_cfg));
        stream_cfg.channel_num     = (enum AUD_CHANNEL_NUM_T)g_chan_num;
        stream_cfg.sample_rate     = (enum AUD_SAMPRATE_T)g_sample_rate;
        stream_cfg.bits         = (enum AUD_BITS_T)g_sample_bits;
        stream_cfg.vol             = 12;
        stream_cfg.chan_sep_buf = false;
        stream_cfg.device       = AUD_STREAM_USE_INT_CODEC;
        stream_cfg.io_path      = AUD_INPUT_PATH_ANC_ASSIST;
        stream_cfg.channel_map    = anc_assist_mic_get_cfg(AUD_INPUT_PATH_ANC_ASSIST);
        stream_cfg.handler      = codec_capture_stream_callback;
        stream_cfg.data_size    = g_capture_buf_size;
        stream_cfg.data_ptr     = codec_capture_buf;

        ANC_TRACE(0, "[%s] sample_rate:%d, data_size:%d, sample_bit:%d", __func__, stream_cfg.sample_rate, stream_cfg.data_size,stream_cfg.bits);

        af_stream_open(STREAM_CAPTURE_ID, AUD_STREAM_CAPTURE, &stream_cfg);
#ifndef VOICE_ASSIST_WD_ENABLED
        af_stream_start(STREAM_CAPTURE_ID, AUD_STREAM_CAPTURE);
#endif
        for (uint32_t i = 0; i < ANC_ASSIST_USER_MAX; i++) {
            if ((g_user_stream_handler[i] != NULL) && (g_user_stream_handler[i]->set_mic_cfg != NULL)) {
                g_user_stream_handler[i]->set_mic_cfg();
            }
        }

        g_mic_open_flag = true;
    }
}

static void _close_mic(void)
{
    if (g_mic_open_flag == false) {
        ANC_TRACE(0, "[%s] WARNING: MIC is closed", __func__);
        return;
    }

    ANC_TRACE(0, "[%s] ...", __func__);
    af_stream_stop(STREAM_CAPTURE_ID, AUD_STREAM_CAPTURE);
    af_stream_close(STREAM_CAPTURE_ID, AUD_STREAM_CAPTURE);

    voice_assist_stream_heap_free(codec_capture_buf);
    af_set_priority(AF_USER_VOICE_ASSIST, osPriorityAboveNormal);

    g_mic_open_flag = false;
}

#if (defined VOICE_ASSIST_PILOT_ANC_ENABLED) || (defined VOICE_ASSIST_WD_ENABLED)
#ifdef VOICE_ASSIST_WD_ENABLED
static const int16_t local_96kHz_pcm_data[] = {
    #include "pilot_oed_pcm.h"
};
#endif

#define ULTRASOUND_VOL (DB2LIN(-40))

static uint32_t codec_play_stream_callback(uint8_t *buf, uint32_t len)
{
    uint32_t pcm_len = len / sizeof(_PCM_T);
    uint32_t frame_len = pcm_len;
    _PCM_T *pcm_buf = (_PCM_T *)buf;

#ifdef VOICE_ASSIST_WD_ENABLED
    app_anc_assist_trigger_checker();
#endif

    app_voice_assist_anc_playback_handler(play_buf, frame_len);

    for (int32_t i=0; i<frame_len; i++) {
        pcm_buf[i] = (_PCM_T)play_buf[i];
    }

#ifdef VOICE_ASSIST_WD_ENABLED
    for (int32_t i=0; i<ARRAY_SIZE(local_96kHz_pcm_data); i++) {
        pcm_buf[i] = speech_ssat_int24(pcm_buf[i] + (int32_t)(ULTRASOUND_VOL * (local_96kHz_pcm_data[i] << 8)));
    }
#endif

#if 0//def ANC_ASSIST_AUDIO_DUMP_96K
    audio_dump_clear_up();
    for (uint32_t i=0; i<frame_len; i++) {
        audio_dump_buf[i] = (_PCM_T)pcm_buf[i] >> RIGHT_SHIFT;
    }
    audio_dump_add_channel_data(0, audio_dump_buf, frame_len);
    audio_dump_run();
#endif

    return len;
}

static void _open_spk(void)
{
    if (g_spk_open_flag == true) {
        ANC_TRACE(0, "[%s] WARNING: SPK is opened", __func__);
        return;
    }

    if (g_need_open_spk) {
        struct AF_STREAM_CONFIG_T stream_cfg;

        memset(&stream_cfg, 0, sizeof(stream_cfg));
        stream_cfg.channel_num = AUD_CHANNEL_NUM_1;
        stream_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)AUD_CHANNEL_MAP_CH0;
        stream_cfg.sample_rate = _PLAY_SAMPLE_RATE;
        stream_cfg.bits = AUD_BITS_24;
        stream_cfg.vol = TGT_VOLUME_LEVEL_MAX;
        stream_cfg.chan_sep_buf = false;
        stream_cfg.device       = STREAM_PLAY_CODEC;
        stream_cfg.handler      = codec_play_stream_callback;
        stream_cfg.data_size    = g_play_buf_size;
        stream_cfg.data_ptr     = codec_play_buf;

        ANC_TRACE(0, "[%s] sample_rate:%d, data_size:%d", __func__, stream_cfg.sample_rate, stream_cfg.data_size);

        af_stream_open(STREAM_PLAY_ID, AUD_STREAM_PLAYBACK, &stream_cfg);

        // add the pingpang
#ifdef VOICE_ASSIST_WD_ENABLED
        int32_t *pcm_buf = (int32_t *)codec_play_buf;
        for (int32_t i=0; i<ARRAY_SIZE(local_96kHz_pcm_data); i++) {
            pcm_buf[i] = (int32_t)(ULTRASOUND_VOL * (local_96kHz_pcm_data[i] << 8));
        }

        pcm_buf = (int32_t *)(codec_play_buf + g_play_buf_size / 2);
        for (int32_t i=0; i<ARRAY_SIZE(local_96kHz_pcm_data); i++) {
            pcm_buf[i] = (int32_t)(ULTRASOUND_VOL * (local_96kHz_pcm_data[i] << 8));
        }
#endif

#ifndef VOICE_ASSIST_WD_ENABLED
        af_stream_start(STREAM_PLAY_ID, AUD_STREAM_PLAYBACK);
#endif

        g_spk_open_flag = true;
    }
}

extern bool pilot_play_get_fadeout_state(void);
static void _close_spk(void)
{
    if (g_spk_open_flag == false) {
        ANC_TRACE(0, "[%s] WARNING: SPK is closed", __func__);
        return;
    }

    app_voice_assist_anc_pilot_set_play_fadeout();

    ANC_TRACE(0, "[%s] ...", __func__);
    af_stream_stop(STREAM_PLAY_ID, AUD_STREAM_PLAYBACK);
    af_stream_close(STREAM_PLAY_ID, AUD_STREAM_PLAYBACK);

    g_spk_open_flag = false;
}

#else

static void _open_spk(void)
{
    ;
}

static void _close_spk(void)
{
    ;
}
#endif

#ifdef ANC_ASSIST_UPDATE_SYSFREQ
#define ANC_ASSIST_BASE_MIPS     (3)
#define ANC_ASSIST_RESAMPLE_MIPS (8)
#define ULTRA_INFRASOUND_BASE_MIPS (20)     //need to be changed

static uint32_t _get_freq(void)
{
    uint32_t _all_freq_mips = 0;
    for (uint32_t i=0; i<ANC_ASSIST_USER_MAX; i++) {
        if (((g_user_status & (0x1 << i)) != 0) && (g_user_algo_handler[i] != NULL) && (g_user_algo_handler[i]->get_freq != NULL)) {
            _all_freq_mips += g_user_algo_handler[i]->get_freq();
        }
    }

    if (g_anc_assist_mode == ANC_ASSIST_MODE_RECORD) {
        _all_freq_mips += 20;
    }

   // ANC_TRACE(0,"[%s] g_user_status = %d _all_freq_mips = %d", __func__, g_user_status, _all_freq_mips);
    return _all_freq_mips;
}

static enum APP_SYSFREQ_FREQ_T _anc_assist_get_sysfreq(void)
{
    enum APP_SYSFREQ_FREQ_T freq = APP_SYSFREQ_32K;
#ifdef VOICE_ASSIST_WD_ENABLED
    int32_t required_mips = _get_freq() + ANC_ASSIST_BASE_MIPS + ULTRA_INFRASOUND_BASE_MIPS;
#else
    int32_t required_mips = _get_freq() + ANC_ASSIST_BASE_MIPS;
#endif

#ifdef ANC_ASSIST_RESAMPLE_ENABLE
    if (g_sample_rate != ALGO_SAMPLE_RATE) {
        required_mips += ANC_ASSIST_RESAMPLE_MIPS;
    }
#endif
    // ANC_TRACE(0, "[%s] Required mips: %dM", __func__,  required_mips);

    if (required_mips >= 96) {
        freq = APP_SYSFREQ_208M;
    } else if(required_mips >= 72) {
        freq = APP_SYSFREQ_104M;
    } else if (required_mips >= 48) {
        freq = APP_SYSFREQ_78M;
    } else if (required_mips >= 24) {
        freq = APP_SYSFREQ_52M;
    } else {
        freq = APP_SYSFREQ_26M;
    }

    // NOTE: Optimize power consumption for special project
    if (g_anc_assist_mode == ANC_ASSIST_MODE_PHONE_CALL) {
        // if (required_mips < 24) {
        //     freq = APP_SYSFREQ_32K;
        // }
    } else if (g_anc_assist_mode == ANC_ASSIST_MODE_RECORD) {
        // if (required_mips < 24) {
        //     freq = APP_SYSFREQ_32K;
        // }
    } else if (g_anc_assist_mode == ANC_ASSIST_MODE_MUSIC) {
        // if (required_mips < 24) {
        //     freq = APP_SYSFREQ_32K;
        // }
    } else if (g_anc_assist_mode == ANC_ASSIST_MODE_MUSIC_AAC) {
        // if (required_mips < 24) {
        //     freq = APP_SYSFREQ_32K;
        // }

    } else if (g_anc_assist_mode == ANC_ASSIST_MODE_MUSIC_SBC) {
        // if (required_mips < 24) {
        //     freq = APP_SYSFREQ_32K;
        // }

    }

    return freq;
}

static void _anc_assist_update_sysfreq(void)
{
    g_sys_freq = _anc_assist_get_sysfreq();
    app_sysfreq_req(APP_SYSFREQ_USER_VOICE_ASSIST, g_sys_freq);

#if defined(ENABLE_CALCU_CPU_FREQ_LOG)
    ANC_TRACE(0, "[%s] Sys freq[%d]: %d", __func__, g_sys_freq, hal_sys_timer_calc_cpu_freq(5, 0));
#endif
}

static void _anc_assist_reset_sysfreq(void)
{
    app_sysfreq_req(APP_SYSFREQ_USER_VOICE_ASSIST, APP_SYSFREQ_32K);

#if defined(ENABLE_CALCU_CPU_FREQ_LOG)
    ANC_TRACE(0, "[%s] Sys freq[%d]: %d", __func__, APP_SYSFREQ_32K, hal_sys_timer_calc_cpu_freq(5, 0));
#endif
}

#endif

uint32_t app_anc_assist_get_frame_len(void)
{
    return g_frame_len;
}
