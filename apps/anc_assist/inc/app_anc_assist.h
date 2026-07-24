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
#ifndef __APP_ANC_ASSIST_H__
#define __APP_ANC_ASSIST_H__

#include "plat_types.h"
#include "hal_aud.h"
#include "tgt_hardware.h"
#include "anc_assist_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ANC_FF_MIC_CH_L
#define ANC_FF_MIC_CH_L       (0xFF)
#endif

#ifndef ANC_FF_MIC_CH_R
#define ANC_FF_MIC_CH_R       (0xFF)
#endif

#ifndef ANC_FB_MIC_CH_L
#define ANC_FB_MIC_CH_L       (0xFF)
#endif

#ifndef ANC_FB_MIC_CH_R
#define ANC_FB_MIC_CH_R       (0xFF)
#endif

#ifndef ANC_TALK_MIC_CH_L
#define ANC_TALK_MIC_CH_L     (0xFF)
#endif

#ifndef ANC_TALK_MIC_CH_R
#define ANC_TALK_MIC_CH_R     (0xFF)
#endif

#ifndef ANC_REF_MIC_CH_L
#define ANC_REF_MIC_CH_L      (0xFF)
#endif

#ifndef ANC_REF_MIC_CH_R
#define ANC_REF_MIC_CH_R      (0xFF)
#endif

#ifndef ANC_SPK_CH
#define ANC_SPK_CH            (AUD_CHANNEL_MAP_CH0)
#endif

#define ANC_ASSIST_USER_MAX 32

typedef enum {
    ANC_ASSIST_MODE_NONE = 0,
    ANC_ASSIST_MODE_STANDALONE,
    ANC_ASSIST_MODE_PHONE_CALL,
    ANC_ASSIST_MODE_RECORD,
    ANC_ASSIST_MODE_MUSIC,
    ANC_ASSIST_MODE_MUSIC_AAC,
    ANC_ASSIST_MODE_MUSIC_SBC,
    ANC_ASSIST_MODE_LE_CALL,

    ANC_ASSIST_MODE_QTY
} anc_assist_mode_t;

typedef struct {
    anc_assist_pcm_t **ff_mic;
    uint8_t ff_ch_num;
    anc_assist_pcm_t **fb_mic;
    uint8_t fb_ch_num;
    anc_assist_pcm_t **talk_mic;
    uint8_t talk_ch_num;
    anc_assist_pcm_t **ref;
    uint8_t ref_ch_num;
    anc_assist_pcm_t *vpu_mic;
    uint32_t frame_len;
} voice_assist_process_frame_data_t;

typedef enum {
    ANC_ASSIST_SET_MODE_ON = 0,
    ANC_ASSIST_SET_MODE_OFF
} anc_assist_set_mode_t;

typedef uint32_t anc_assist_user_t;
typedef int (*app_voice_assist_handler_t)(void);
typedef int (*app_voice_assist_set_mode_t)(anc_assist_set_mode_t mode);
typedef int (*app_voice_assist_get_heap_buf_size_t)(anc_assist_mode_t mode);
typedef int (*app_voice_assist_capture_process_t)(voice_assist_process_frame_data_t *process_frame_data);
typedef int (*app_voice_assist_sync_info_t)(void *buf, uint32_t len, uint32_t sub_cmd);

typedef struct {
    app_voice_assist_handler_t              open;
    app_voice_assist_handler_t              close;
    app_voice_assist_handler_t              reset;
    app_voice_assist_handler_t              get_freq;
    app_voice_assist_handler_t              get_fs;
    app_voice_assist_set_mode_t             set_mode;
    app_voice_assist_get_heap_buf_size_t    get_heap_buf_size;

    app_voice_assist_capture_process_t      cap_process;
} voice_assist_algo_callback_t;

typedef struct {
    const uint8_t *user_name;
    uint8_t *user_ptr;
} voice_assist_user_info_t;

typedef struct {
    app_voice_assist_handler_t             get_mic_ch_map;
    app_voice_assist_handler_t             set_mic_cfg;
    app_voice_assist_handler_t             get_spk_ch_map;
    app_voice_assist_sync_info_t           sync_info;
} voice_assist_stream_callback_t;

typedef struct {
    const voice_assist_stream_callback_t *voice_assist_callback;
    const voice_assist_algo_callback_t *assist_algo_dsp;
} voice_assist_user_register_cfg;

int32_t app_anc_assist_init(void);
int32_t app_anc_assist_deinit(void);
uint32_t app_anc_assist_stream_heap_size(void);
voice_assist_user_info_t *app_anc_assist_get_user_index(void);
int32_t app_anc_assist_is_runing(void);
uint32_t app_anc_assist_get_mic_ch_num(enum AUD_IO_PATH_T path);
uint32_t app_anc_assist_get_mic_ch_map(enum AUD_IO_PATH_T path);
int32_t app_anc_assist_set_mode(anc_assist_mode_t mode);
anc_assist_mode_t app_anc_assist_get_mode(void);
int32_t app_anc_assist_set_playback_info(int32_t sample_rate);
int32_t app_anc_assist_get_play_sample_rate(void);
int32_t app_anc_assist_open(anc_assist_user_t user);
int32_t app_anc_assist_close(anc_assist_user_t user);
int32_t app_anc_assist_process(void *pcm_buf, uint32_t pcm_len);
int32_t app_anc_assist_process_interval(void *buf, uint32_t len);
int32_t app_anc_assist_parser_app_mic_buf(void *buf, uint32_t *len);
uint32_t app_anc_assist_get_frame_len(void);

uint32_t app_anc_get_anc_assist_buff_size(void);
int32_t app_anc_assist_set_app_capture_info(uint32_t frame_len, int32_t sample_rate);
void app_anc_assist_set_custom_leak_working_status(int32_t status);
void app_anc_assist_get_cfg(int *frame_size, int *sample_rate, int *sample_bit, int *ch_num);
void *voice_assist_stream_heap_malloc(size_t size);
void voice_assist_stream_heap_free(void *ptr);
int32_t app_voice_assist_stream_register(const uint8_t* user_name, voice_assist_user_register_cfg* cfg, uint8_t *user_ptr);
uint8_t *voice_assist_get_user_ptr(const uint8_t *name);
int32_t app_voice_assist_dsp_register(uint32_t *user, const uint8_t *name, voice_assist_user_register_cfg* voice_assist_callback);
int32_t app_anc_assist_reset(anc_assist_user_t user);
app_voice_assist_sync_info_t app_anc_assist_get_user_sync_handler(anc_assist_user_t user);
#ifdef __cplusplus
}
#endif

#endif