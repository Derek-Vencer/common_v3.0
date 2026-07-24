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
#ifdef AF_STREAM_SW_GAIN_ENABLE
#include "af_stream_sw_gain.h"
#include "cmsis.h"
#include "hal_codec.h"
#include "hal_trace.h"
#include "system_utils.h"

#ifdef RTOS
#include "cmsis_os.h"

#define ENABLE_MULTI_MUTEX_LOCK
#endif

#define MAX_DIG_DBVAL                       (50)
#define ZERODB_DIG_DBVAL                    (0)
#define MIN_DIG_DBVAL                       (-99)

#define MAX_CHANNEL_NUM                     (10)
#define DEFAULT_SMOOTH_TIME_MS              (20)
#define AF_STREAM_PLAYBACK_FADEOUT_TIMEOUT  (300)

#define AF_STREAM_FADE_OUT_SIGNAL_ID        (15)
#define AF_STREAM_FADE_IN_SIGNAL_ID         (14)

// Audioflinger Stream Software Gain Error Codes
#define AF_STREAM_SW_GAIN_OK                (0)
#define AF_STREAM_SW_GAIN_ERROR             (-1)

#ifdef RTOS
static const osMutexAttr_t af_sw_gain_mutex_attr = {
    .name = "sw_gain_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit | osMutexRobust,
    .cb_mem = NULL,
    .cb_size = 0U,
};
#else
static volatile uint32_t af_sw_gain_flag_lock = 0;
#endif

struct AF_SW_GAIN_INSTANCE {
    uint32_t channel_num;
    uint32_t volume;
    float gain;
    float extra_gain[AF_STREAM_SW_GAIN_ADD_USER_NUM];
    float max_gain;
    RampHandle sw_gain_handle;

    bool open_status;
    enum AF_STREAM_SW_GAIN_POS_T pos;
    bool stop_status;
    bool chan_sep_flag;

#ifdef RTOS
    // Stream stop fade out to zero
    osThreadId fade_thread_id;
    RampHandle stop_ramp_handle;
    uint32_t stop_finish_cnt;
    bool stream_start_flag;
#endif

    // Lock
#if defined(RTOS) && defined(ENABLE_MULTI_MUTEX_LOCK)
    osMutexId_t lock;
#endif
};

static struct AF_SW_GAIN_INSTANCE g_af_sw_gain_instance[AUD_STREAM_ID_NUM][AUD_STREAM_NUM] = {0};
#if defined(RTOS) && !defined(ENABLE_MULTI_MUTEX_LOCK)
static osMutexId_t af_sw_gain_mutex = NULL;
#endif

static struct AF_SW_GAIN_INSTANCE *_get_af_sw_gain_instance(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream)
{
    if (id >= AUD_STREAM_ID_NUM || stream >= AUD_STREAM_NUM) {
        ASSERT(true, "%s Error:id, %d, stream:%d", __func__, id, stream);
    }

    return &g_af_sw_gain_instance[id][stream];
}

static void af_stream_sw_gain_lock(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream)
{
#ifndef ENABLE_CAPTURE_RAMP
    if (stream == AUD_STREAM_CAPTURE) {
        return;
    }
#endif

#ifdef RTOS
    osStatus_t ret = osOK;
#ifdef ENABLE_MULTI_MUTEX_LOCK
    struct AF_SW_GAIN_INSTANCE *af_inst = _get_af_sw_gain_instance(id, stream);
    ret = osMutexAcquire(af_inst->lock, osWaitForever);
#else
    ret = osMutexAcquire(af_sw_gain_mutex, osWaitForever);
#endif
    if (ret != osOK) {
        AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, lock is error:%d", __func__, id, stream, ret);
    }
#else
    void * POSSIBLY_UNUSED lr = __builtin_return_address(0);
    static void * POSSIBLY_UNUSED locked_lr;
    ASSERT(af_sw_gain_flag_lock == 0, "audioflinger has been locked by %p. LR=%p", (void *)locked_lr, (void *)lr);
    af_sw_gain_flag_lock = 1;
    locked_lr = lr;
#endif
}

static void af_stream_sw_gain_unlock(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream)
{
#ifndef ENABLE_CAPTURE_RAMP
    if (stream == AUD_STREAM_CAPTURE) {
        return;
    }
#endif

#ifdef RTOS
    osStatus_t ret = osOK;
#ifdef ENABLE_MULTI_MUTEX_LOCK
    struct AF_SW_GAIN_INSTANCE *af_inst = _get_af_sw_gain_instance(id, stream);
    ret = osMutexRelease(af_inst->lock);
#else
    ret = osMutexRelease(af_sw_gain_mutex);
#endif
    if (ret != osOK) {
        AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, unlock is error:%d", __func__, id, stream, ret);
    }
#else
    void * POSSIBLY_UNUSED lr = __builtin_return_address(0);
    static void * POSSIBLY_UNUSED unlocked_lr;
    ASSERT(af_sw_gain_flag_lock == 1, "audioflinger not locked before (lastly unlocked by %p). LR=%p", (void *)unlocked_lr, (void *)lr);
    af_sw_gain_flag_lock = 0;
    unlocked_lr = lr;
#endif
}

void af_stream_sw_gain_init(void)
{
    for (int i = 0; i < AUD_STREAM_ID_NUM; i++) {
        for (int j = 0; j < AUD_STREAM_NUM; j++) {
            g_af_sw_gain_instance[i][j].gain = 1.0;
            g_af_sw_gain_instance[i][j].max_gain = 1.0;
            for (int user = 0; user < AF_STREAM_SW_GAIN_ADD_USER_NUM; user++) {
                g_af_sw_gain_instance[i][j].extra_gain[user] = 1.0;
            }
#if defined(RTOS) && defined(ENABLE_MULTI_MUTEX_LOCK)
#ifdef ENABLE_CAPTURE_RAMP
            g_af_sw_gain_instance[i][j].lock = osMutexNew(&af_sw_gain_mutex_attr);
            ASSERT(g_af_sw_gain_instance[i][j].lock, "[%s]:mutex init error:%d, %d", __func__, i, j);
#else
            if (j == AUD_STREAM_PLAYBACK) {
                g_af_sw_gain_instance[i][j].lock = osMutexNew(&af_sw_gain_mutex_attr);
                ASSERT(g_af_sw_gain_instance[i][j].lock, "[%s]:mutex init error:%d, %d", __func__, i, j);
            }
#endif
#endif
        }
    }

#if defined(RTOS) && !defined(ENABLE_MULTI_MUTEX_LOCK)
    af_sw_gain_mutex = osMutexNew(&af_sw_gain_mutex_attr);
    ASSERT(af_sw_gain_mutex, "[%s]:mutex init errord", __func__);
#endif
}

static float calc_real_sw_gain(struct AF_SW_GAIN_INSTANCE *af_inst)
{
    float real_gain = af_inst->gain;
    for (int i = 0; i < AF_STREAM_SW_GAIN_ADD_USER_NUM; i++) {
        real_gain *= af_inst->extra_gain[i];
    }

    if (real_gain > af_inst->max_gain) {
        real_gain = af_inst->max_gain;
    }

    return real_gain;
}

int32_t af_stream_sw_gain_open(enum AUD_STREAM_ID_T id,
                            enum AUD_STREAM_T stream,
                            uint32_t sample_rate,
                            uint32_t channel_num,
                            uint32_t bits,
                            bool chan_sep_flag,
                            enum AF_STREAM_SW_GAIN_TYPE_T type)
{
#ifndef ENABLE_CAPTURE_RAMP
    if (stream == AUD_STREAM_CAPTURE) {
        AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, not enable capture software gain", __func__, id, stream);
        return AF_STREAM_SW_GAIN_OK;
    }
#endif

    af_stream_sw_gain_lock(id, stream);
    struct AF_SW_GAIN_INSTANCE *af_inst = _get_af_sw_gain_instance(id, stream);

    RampConfig ramp_cfg = {0};
    ramp_cfg.sample_rate = sample_rate;
    ramp_cfg.bit_depth = bits;
    ramp_cfg.num_channels = channel_num;
    ramp_cfg.ramp_time_ms = DEFAULT_SMOOTH_TIME_MS;
    ramp_cfg.initial_gain = 0;
    ramp_cfg.type = (RampType)type;

    RampHandle sw_gain_handle = audio_ramp_open(&ramp_cfg);
    if (sw_gain_handle <= 0) {
        AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, audio ramp open fail:%d", __func__, id, stream, sw_gain_handle);
        af_stream_sw_gain_unlock(id, stream);
        return AF_STREAM_SW_GAIN_ERROR;
    }

    audio_ramp_set_target(sw_gain_handle, calc_real_sw_gain(af_inst));
    af_inst->sw_gain_handle = sw_gain_handle;
    AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, sw_gain_handle:%d", __func__, id, stream, af_inst->sw_gain_handle);

#ifdef RTOS
    ramp_cfg.sample_rate = sample_rate;
    ramp_cfg.bit_depth = bits;
    ramp_cfg.num_channels = channel_num;
    ramp_cfg.ramp_time_ms = DEFAULT_SMOOTH_TIME_MS;
    ramp_cfg.initial_gain = 1.0;
    ramp_cfg.type = RAMP_SMOOTHSTEP;

    sw_gain_handle = audio_ramp_open(&ramp_cfg);
    if (sw_gain_handle <= 0) {
        AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, stop ramp open fail:%d", __func__, id, stream, sw_gain_handle);
        af_stream_sw_gain_unlock(id, stream);
        return AF_STREAM_SW_GAIN_ERROR;
    }
    af_inst->stop_ramp_handle = sw_gain_handle;
    af_inst->stop_finish_cnt = 0;
    af_inst->stream_start_flag = false;
#endif

    af_inst->channel_num = channel_num;
    af_inst->chan_sep_flag = chan_sep_flag;
    af_inst->open_status = true;
    af_inst->stop_status = false;

    af_stream_sw_gain_unlock(id, stream);
    return AF_STREAM_SW_GAIN_OK;
}

void af_stream_sw_gain_close(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream)
{
    struct AF_SW_GAIN_INSTANCE *af_inst = _get_af_sw_gain_instance(id, stream);

    if (!af_inst->open_status) {
        AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, ramp is not open", __func__, id, stream);
        return;
    }

    af_stream_sw_gain_lock(id, stream);
    af_inst->open_status = false;
    af_inst->stop_status = false;
    af_inst->pos = AF_STREAM_SW_GAIN_POS_NORMAL;
    audio_ramp_close(af_inst->sw_gain_handle);
    af_inst->sw_gain_handle = 0;

#ifdef RTOS
    af_inst->stream_start_flag = false;
    af_inst->stop_finish_cnt = 0;
    if (af_inst->stop_ramp_handle) {
        audio_ramp_close(af_inst->stop_ramp_handle);
        af_inst->stop_ramp_handle = 0;
        af_inst->fade_thread_id = NULL;
    }
#endif

    af_stream_sw_gain_unlock(id, stream);
}

void af_stream_sw_gain_set_gain_coef(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream, float gain)
{
    af_stream_sw_gain_lock(id, stream);
    struct AF_SW_GAIN_INSTANCE *af_inst = _get_af_sw_gain_instance(id, stream);
    af_inst->gain = gain;
    AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, set gain:(x1000):%d", __func__, id, stream, (int32_t)(gain * 1000));

    if (!af_inst->open_status) {
        AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, ramp is not open", __func__, id, stream);
        af_stream_sw_gain_unlock(id, stream);
        return;
    }

    if (af_inst->stop_status) {
        AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, ramp is closed", __func__, id, stream);
        af_stream_sw_gain_unlock(id, stream);
        return;
    }

    int ret = audio_ramp_set_target(af_inst->sw_gain_handle, calc_real_sw_gain(af_inst));
    if (ret != AUDIO_RAMP_OK) {
        AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, set gain fail:%d", __func__, id, stream, ret);
    }
    af_stream_sw_gain_unlock(id, stream);
}

void af_stream_sw_gain_set_max_gain_coef(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream, float max_gain)
{
    af_stream_sw_gain_lock(id, stream);
    struct AF_SW_GAIN_INSTANCE *af_inst = _get_af_sw_gain_instance(id, stream);
    af_inst->max_gain = max_gain;
    AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, set max gain:(x1000):%d", __func__, id, stream, (int32_t)(max_gain * 1000));

    if (!af_inst->open_status) {
        AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, ramp is not open", __func__, id, stream);
        af_stream_sw_gain_unlock(id, stream);
        return;
    }

    if (af_inst->stop_status) {
        AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, ramp is closed", __func__, id, stream);
        af_stream_sw_gain_unlock(id, stream);
        return;
    }

    int ret = audio_ramp_set_target(af_inst->sw_gain_handle, calc_real_sw_gain(af_inst));
    if (ret != AUDIO_RAMP_OK) {
        AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, set gain fail:%d", __func__, id, stream, ret);
    }
    af_stream_sw_gain_unlock(id, stream);
}

void af_stream_sw_gain_set_volume_level(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream, uint32_t vol)
{
    af_stream_sw_gain_lock(id, stream);
    struct AF_SW_GAIN_INSTANCE *af_inst = _get_af_sw_gain_instance(id, stream);
    af_inst->volume = vol;

    if (!af_inst->open_status) {
        AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, ramp is not open", __func__, id, stream);
        af_stream_sw_gain_unlock(id, stream);
        return;
    }

    float coef = 0.0;
    CODEC_DAC_VOL_T dac_vol_db = hal_codec_get_dac_volume(vol);

    if (dac_vol_db == ZERODB_DIG_DBVAL) {
        coef = 1.0;
    } else if (dac_vol_db <= MIN_DIG_DBVAL) {
        coef = 0.0;
    } else {
        if (dac_vol_db > MAX_DIG_DBVAL) {
            dac_vol_db = MAX_DIG_DBVAL;
        }
        coef = db_to_float(dac_vol_db);
    }

    af_stream_sw_gain_set_gain_coef(id, stream, coef);
    af_stream_sw_gain_unlock(id, stream);
}

float af_stream_sw_gain_get_gain_coef(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream)
{
    struct AF_SW_GAIN_INSTANCE *af_inst = _get_af_sw_gain_instance(id, stream);
    return af_inst->gain;
}

uint32_t af_stream_sw_gain_get_volume(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream)
{
    struct AF_SW_GAIN_INSTANCE *af_inst = _get_af_sw_gain_instance(id, stream);
    return af_inst->volume;
}

void af_stream_sw_gain_set_extra_gain(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream,
                                        enum AF_STREAM_SW_GAIN_ADD_USER_T user, float extra_gain)
{
    if (user >= AF_STREAM_SW_GAIN_ADD_USER_NUM) {
        AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, extra_user:%d is invaild", __func__, id, stream, user);
        return;
    }

    af_stream_sw_gain_lock(id, stream);
    struct AF_SW_GAIN_INSTANCE *af_inst = _get_af_sw_gain_instance(id, stream);
    af_inst->extra_gain[user] = extra_gain;
    // AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, user:%d, set extra gain:(x1000):%d", __func__, id, stream, user, (int32_t)(extra_gain * 1000));

    if (!af_inst->open_status) {
        AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, ramp is not open", __func__, id, stream);
        af_stream_sw_gain_unlock(id, stream);
        return;
    }

    if (af_inst->stop_status) {
        AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, ramp is closed", __func__, id, stream);
        af_stream_sw_gain_unlock(id, stream);
        return;
    }

    int ret = audio_ramp_set_target(af_inst->sw_gain_handle, calc_real_sw_gain(af_inst));
    if (ret != AUDIO_RAMP_OK) {
        AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, set extra gain fail:%d", __func__, id, stream, ret);
    }
    af_stream_sw_gain_unlock(id, stream);
}

void af_stream_sw_gain_set_smooth_time(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream, uint32_t time_ms)
{
    struct AF_SW_GAIN_INSTANCE *af_inst = _get_af_sw_gain_instance(id, stream);

    if (!af_inst->open_status) {
        AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, ramp is not open", __func__, id, stream);
        return;
    }

    if (af_inst->stop_status) {
        AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, ramp is closed", __func__, id, stream);
        return;
    }

    int ret = audio_ramp_set_ramp_time(af_inst->sw_gain_handle, time_ms);
    if (ret != AUDIO_RAMP_OK) {
        AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, set smooth time fail:%d", __func__, id, stream, ret);
    }
}

bool af_stream_sw_gain_is_finished(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream)
{
    struct AF_SW_GAIN_INSTANCE *af_inst = _get_af_sw_gain_instance(id, stream);

    if (!af_inst->open_status) {
        AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, ramp is not open", __func__, id, stream);
        return true;
    }

    return audio_ramp_is_finished(af_inst->sw_gain_handle);
}

void af_stream_sw_gain_set_position(enum AUD_STREAM_ID_T id,
                                enum AUD_STREAM_T stream,
                                enum AF_STREAM_SW_GAIN_POS_T pos)
{
    struct AF_SW_GAIN_INSTANCE *af_inst = _get_af_sw_gain_instance(id, stream);
    AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, set position:%d", __func__, id, stream, pos);
    af_inst->pos = pos;
}

enum AF_STREAM_SW_GAIN_POS_T af_stream_sw_gain_get_position(enum AUD_STREAM_ID_T id,
                                                 enum AUD_STREAM_T stream)
{
    struct AF_SW_GAIN_INSTANCE *af_inst = _get_af_sw_gain_instance(id, stream);

    if (!af_inst->open_status) {
        AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, ramp is not open", __func__, id, stream);
        return AF_STREAM_SW_GAIN_POS_NORMAL;
    }

    return af_inst->pos;
}

void af_stream_sw_gain_capture_process(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream, uint8_t *buf, uint32_t len)
{
    struct AF_SW_GAIN_INSTANCE *af_inst = _get_af_sw_gain_instance(id, stream);

    if (!af_inst->open_status) {
        // AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, ramp is not open", __func__, id, stream);
        return;
    }

    if (af_inst->chan_sep_flag) {
        void *data_buf[MAX_CHANNEL_NUM] = {0};
        uint32_t per_chan_len = len / af_inst->channel_num;
        for (uint32_t ch = 0; ch < af_inst->channel_num; ch++) {
            data_buf[ch] = buf + ch * per_chan_len * 2;
        }
        audio_ramp_process_sep_chan(af_inst->sw_gain_handle, data_buf, data_buf, per_chan_len);
    } else {
        audio_ramp_process(af_inst->sw_gain_handle, buf, buf, len, true);
    }
}

void af_stream_sw_gain_playback_process(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream, uint8_t *buf, uint32_t len)
{
    struct AF_SW_GAIN_INSTANCE *af_inst = _get_af_sw_gain_instance(id, stream);

    if (!af_inst->open_status) {
        // AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, ramp is not open", __func__, id, stream);
        return;
    }

    if (af_inst->chan_sep_flag) {
        void *data_buf[MAX_CHANNEL_NUM] = {0};
        uint32_t per_chan_len = len / af_inst->channel_num;
        for (uint32_t ch = 0; ch < af_inst->channel_num; ch++) {
            data_buf[ch] = buf + ch * per_chan_len * 2;
        }
        audio_ramp_process_sep_chan(af_inst->sw_gain_handle, data_buf, data_buf, per_chan_len);
    } else {
        audio_ramp_process(af_inst->sw_gain_handle, buf, buf, len, true);
    }
}

#ifdef RTOS
void af_stream_sw_gain_set_stream_start_flag(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream)
{
    struct AF_SW_GAIN_INSTANCE *af_inst = _get_af_sw_gain_instance(id, stream);

    if (!af_inst->open_status) {
        // AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, ramp is not open", __func__, id, stream);
        return;
    }

    if (!af_inst->stream_start_flag) {
        af_inst->stream_start_flag = true;
        AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, set stream_start_flag is true", __func__, id, stream);
    }
}

void af_stream_sw_gain_playback_before_close_fadeout(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream, uint32_t time_ms)
{
    struct AF_SW_GAIN_INSTANCE *af_inst = _get_af_sw_gain_instance(id, stream);

    if (!af_inst->open_status) {
        AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, ramp is not open", __func__, id, stream);
        return;
    }

    if (af_inst->stop_status) {
        AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, ramp is closed", __func__, id, stream);
        return;
    }

    if (!af_inst->stream_start_flag) {
        AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, stream not start", __func__, id, stream);
        return;
    }

    af_inst->stop_status = true;
    af_inst->stop_finish_cnt = 0;
    af_inst->fade_thread_id = osThreadGetId();
    audio_ramp_set_ramp_time(af_inst->stop_ramp_handle, time_ms);
    audio_ramp_set_target(af_inst->stop_ramp_handle, 0.0);
    osSignalClear(af_inst->fade_thread_id, (1 << AF_STREAM_FADE_OUT_SIGNAL_ID));
    osSignalWait((1 << AF_STREAM_FADE_OUT_SIGNAL_ID), AF_STREAM_PLAYBACK_FADEOUT_TIMEOUT);
    af_inst->fade_thread_id = NULL;
}

void af_stream_sw_gain_playback_fadeout_process(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream, uint8_t *buf, uint32_t len)
{
    struct AF_SW_GAIN_INSTANCE *af_inst = _get_af_sw_gain_instance(id, stream);

    if (af_inst->open_status && af_inst->stop_status) {
        if (af_inst->chan_sep_flag) {
            void *data_buf[MAX_CHANNEL_NUM] = {0};
            uint32_t per_chan_len = len / af_inst->channel_num;
            for (uint32_t ch = 0; ch < af_inst->channel_num; ch++) {
                data_buf[ch] = buf + ch * per_chan_len * 2;
            }
            audio_ramp_process_sep_chan(af_inst->stop_ramp_handle, data_buf, data_buf, per_chan_len);
        } else {
            audio_ramp_process(af_inst->stop_ramp_handle, buf, buf, len, true);
        }
        // AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, ramp is finish:%d", __func__, id, stream, audio_ramp_is_finished(af_inst->stop_ramp_handle));
        if (af_inst->fade_thread_id && audio_ramp_is_finished(af_inst->stop_ramp_handle)) {
            af_inst->stop_finish_cnt++;
            AF_STREAM_SW_GAIN_TRACE(0, "%s, ID:%d, STREAM:%d, Fadeout finish, cnt:%d", __func__, id, stream, af_inst->stop_finish_cnt);
            if (af_inst->stop_finish_cnt > 2) {
                osSignalSet(af_inst->fade_thread_id, (1 << AF_STREAM_FADE_OUT_SIGNAL_ID));
            }
        }
    }
}
#endif
#endif
