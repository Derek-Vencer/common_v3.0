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
 * @brief xxx.
 *
 ****************************************************************************/
#ifdef BLE_I2S_AUDIO_SUPPORT
#ifdef AOB_MOBILE_ENABLED

/****************************** header include ********************************/
#include <stdint.h>
#include "cmsis.h"
#include "cmsis_os.h"
#include "audioflinger.h"
#include "gaf_audio_path.h"
#include "hal_dma.h"
#include "app_trace_rx.h"
#include "bluetooth_bt_api.h"
#include "app_bt_func.h"
#include "app_utils.h"
#include "plat_types.h"
#include "audioflinger.h"
#include "bt_drv_reg_op.h"
#include "bt_drv_interface.h"
#include "app_audio.h"
#include "gaf_audio_path.h"
#include "gaf_mobile_media_stream.h"
#include "aob_media_api.h"
#include "aob_mgr_gaf_evt.h"
#include "gaf_dbg.h"
#include "gaf_media_sync.h"
#include "gaf_media_pid.h"
#include "lc3_process.h"
#include "gaf_codec_lc3.h"
#include "app_bap_data_path_itf.h"
#include "gaf_media_common.h"
#include "app_gaf_custom_api.h"
#include "app_bt_sync.h"
#include "aob_volume_api.h"
#include "audio_dump.h"
#include "app_bt_func.h"
#include "plat_types.h"
#include "app_audio.h"
#include "hal_tdm.h"

#ifdef DMA_AUDIO_APP
#include "dma_audio_host.h"
#include "dma_audio_def.h"
#include "dma_audio_cli.h"
#endif

#include "gaf_media_stream.h"

/***************************** external declaration *****************************/

/***************************** macro defination *******************************/

/***************************** type defination ********************************/

/***************************** variable defination *****************************/

/***************************** function declaration ****************************/
#ifdef HEADSET_AS_TDM0_SLAVE
#define I2S_AUDIO_PLAYBACK_96K
#define I2S_AUDIO_24BIT
#endif

#ifdef I2S_AUDIO_32BIT
#define SAMPLE_SIZE_PLAYBACK            4
#define SAMPLE_SIZE_CAPTURE             4
#elif defined(I2S_AUDIO_24BIT)
#define SAMPLE_SIZE_PLAYBACK            4
#define SAMPLE_SIZE_CAPTURE             4
#else
#define SAMPLE_SIZE_PLAYBACK            2
#define SAMPLE_SIZE_CAPTURE             2
#endif

#define CHAN_NUM_PLAYBACK               2
#define CHAN_NUM_CAPTURE                2

#ifdef I2S_AUDIO_PLAYBACK_96K
#define SAMPLE_RATE_PLAYBACK            AUD_SAMPRATE_96000
#else
#define SAMPLE_RATE_PLAYBACK            AUD_SAMPRATE_48000
#endif

#define SAMPLE_RATE_CAPTURE             AUD_SAMPRATE_48000

#define SAMP_RATE_TO_FRAME_SIZE(n)      (((n) + (1000 - 1)) / 1000)
#define SAMPLE_FRAME_PLAYBACK           SAMP_RATE_TO_FRAME_SIZE(SAMPLE_RATE_PLAYBACK)
#define SAMPLE_FRAME_CAPTURE            SAMP_RATE_TO_FRAME_SIZE(SAMPLE_RATE_CAPTURE)

#define SAMP_TO_BYTE_PLAYBACK(n)        ((n) * SAMPLE_SIZE_PLAYBACK * CHAN_NUM_PLAYBACK)
#define SAMP_TO_BYTE_CAPTURE(n)         ((n) * SAMPLE_SIZE_CAPTURE * CHAN_NUM_CAPTURE)

#define FRAME_SIZE_PLAYBACK             SAMP_TO_BYTE_PLAYBACK(SAMPLE_FRAME_PLAYBACK)
#define FRAME_SIZE_CAPTURE              SAMP_TO_BYTE_CAPTURE(SAMPLE_FRAME_CAPTURE)

#define PLAYBACK_FRAME_NUM              20
#define CAPTURE_FRAME_NUM               20

#define BUFF_ALIGN                      (4 * 4)
#define NON_EXP_ALIGN(val, exp)         (((val) + ((exp) - 1)) / (exp) * (exp))

#define I2S_AUDIO_PLAYBACK_BUFF_SIZE    NON_EXP_ALIGN(FRAME_SIZE_PLAYBACK * PLAYBACK_FRAME_NUM, BUFF_ALIGN)
#define I2S_AUDIO_CAPTURE_BUFF_SIZE     NON_EXP_ALIGN(FRAME_SIZE_CAPTURE * CAPTURE_FRAME_NUM, BUFF_ALIGN)

// #define ALIGNED4                        ALIGNED(4)
// static uint8_t ALIGNED4 i2s_in_buff[I2S_AUDIO_PLAYBACK_BUFF_SIZE] = {0};
// static uint8_t ALIGNED4 dac_out_buff[I2S_AUDIO_PLAYBACK_BUFF_SIZE] = {0};
// static uint8_t ALIGNED4 i2s_out_buff[I2S_AUDIO_CAPTURE_BUFF_SIZE] = {0};
// static uint8_t ALIGNED4 dac_in_buff[I2S_AUDIO_CAPTURE_BUFF_SIZE] = {0};

static uint8_t* i2s_in_buff = NULL;
static uint8_t* dac_out_buff = NULL;
static uint8_t* i2s_out_buff = NULL;
static uint8_t* dac_in_buff = NULL;

static enum AUD_SAMPRATE_T sample_rate_play = SAMPLE_RATE_PLAYBACK;
static enum AUD_SAMPRATE_T sample_rate_cap = SAMPLE_RATE_CAPTURE;

static const uint8_t sample_size_play = SAMPLE_SIZE_PLAYBACK;
static const uint8_t sample_size_cap = SAMPLE_SIZE_CAPTURE;

static uint8_t playback_vol = TGT_ADC_VOL_LEVEL_10;
static uint8_t capture_vol = TGT_ADC_VOL_LEVEL_10;

static uint8_t* pcm_queue_buff[AUD_STREAM_NUM] = {NULL};
static CQueue pcm_queue[AUD_STREAM_NUM];
static osMutexId pcmbuff_mutex_id[AUD_STREAM_NUM] = {NULL};
osMutexDef(pcmbuff_playback_mutex);
osMutexDef(pcmbuff_capture_mutex);

static bool lea_i2s_playback_streaming = false;
static bool lea_i2s_capture_streaming = false;

#if defined(DONGLE_AS_I2S0_MASTER)
static AUD_STREAM_USE_DEVICE_T i2s_device = AUD_STREAM_USE_I2S0_MASTER;
#elif defined(HEADSET_AS_TDM0_SLAVE)
static AUD_STREAM_USE_DEVICE_T i2s_device = AUD_STREAM_USE_TDM0_SLAVE;
#else
static AUD_STREAM_USE_DEVICE_T i2s_device = AUD_STREAM_USE_I2S0_SLAVE;
#endif

static struct I2S_AUDIO_SOURCE_EVENT_CALLBACK_T i2s_audio_source_cb_list;

void i2s_audio_source_config_init(const struct I2S_AUDIO_SOURCE_EVENT_CALLBACK_T *cb_list)
{
    memcpy(&i2s_audio_source_cb_list, cb_list, sizeof(i2s_audio_source_cb_list));
}

static enum AUD_BITS_T sample_size_to_enum(uint32_t size)
{
    if (size == 2) {
        return AUD_BITS_16;
    } else if (size == 4) {
        return AUD_BITS_24;
    } else {
        RUNTIME_ASSERT(false, "%s: Invalid sample size: %u", __FUNCTION__, size);
    }

    return AUD_BITS_NULL;
}

static enum AUD_CHANNEL_NUM_T chan_num_to_enum(uint32_t num)
{
    return (enum AUD_CHANNEL_NUM_T)(AUD_CHANNEL_NUM_1 + (num - 1));
}

int app_ble_i2s_audio_pcmbuff_init(AUD_STREAM_T stream)
{
    uint32_t pcmbuff_size = 0;
    if (stream == AUD_STREAM_PLAYBACK) {
        if (pcmbuff_mutex_id[stream] == NULL)
            pcmbuff_mutex_id[stream] = osMutexCreate((osMutex(pcmbuff_playback_mutex)));
        pcmbuff_size = I2S_AUDIO_PLAYBACK_BUFF_SIZE;
    } else if (stream == AUD_STREAM_CAPTURE) {
        if (pcmbuff_mutex_id[stream] == NULL)
            pcmbuff_mutex_id[stream] = osMutexCreate((osMutex(pcmbuff_capture_mutex)));
        pcmbuff_size = I2S_AUDIO_CAPTURE_BUFF_SIZE;
    } else {
        RUNTIME_ASSERT(0, "invalid aud stream: %d", stream);
        return -1;
    }

    RUNTIME_ASSERT(pcmbuff_mutex_id[stream], "%s create os mutex failed.", __func__);

    if (lea_i2s_playback_streaming == false && lea_i2s_capture_streaming == false) {
        app_audio_mempool_init_with_specific_size(app_audio_mempool_size());
        pcm_queue_buff[AUD_STREAM_PLAYBACK] = NULL;
        pcm_queue_buff[AUD_STREAM_CAPTURE] = NULL;
        i2s_in_buff = NULL; dac_out_buff = NULL;
        i2s_out_buff = NULL; dac_in_buff = NULL;
    }

    if (stream == AUD_STREAM_PLAYBACK) {
        if (pcm_queue_buff[stream] == NULL) app_audio_mempool_get_buff(&pcm_queue_buff[stream], pcmbuff_size * 2);
        if (i2s_in_buff  == NULL) app_audio_mempool_get_buff(&i2s_in_buff, pcmbuff_size);
        if (dac_out_buff == NULL) app_audio_mempool_get_buff(&dac_out_buff, pcmbuff_size);

        RUNTIME_ASSERT(pcm_queue_buff[stream] && i2s_in_buff && dac_out_buff, "%s get mempool failed.", __func__);
    }else if(stream == AUD_STREAM_CAPTURE) {
        if (pcm_queue_buff[stream] == NULL) app_audio_mempool_get_buff(&pcm_queue_buff[stream], pcmbuff_size * 2);
        if (i2s_out_buff == NULL) app_audio_mempool_get_buff(&i2s_out_buff, pcmbuff_size);
        if (dac_in_buff  == NULL) app_audio_mempool_get_buff(&dac_in_buff, pcmbuff_size);

        RUNTIME_ASSERT(pcm_queue_buff[stream] && i2s_out_buff && dac_in_buff, "%s get mempool failed.", __func__);
    }

    osMutexWait(pcmbuff_mutex_id[stream], osWaitForever);
    InitCQueue(&pcm_queue[stream], pcmbuff_size * 2, pcm_queue_buff[stream]);
    memset(pcm_queue_buff[stream], 0x00, pcmbuff_size * 2);
    osMutexRelease(pcmbuff_mutex_id[stream]);

    return 0;
}

int app_ble_i2s_audio_pcmbuff_space(AUD_STREAM_T stream)
{
    int len;

    osMutexWait(pcmbuff_mutex_id[stream], osWaitForever);
    len = AvailableOfCQueue(&pcm_queue[stream]);
    osMutexRelease(pcmbuff_mutex_id[stream]);

    return len;
}

int app_ble_i2s_audio_pcmbuff_length(AUD_STREAM_T stream)
{
    int len;

    osMutexWait(pcmbuff_mutex_id[stream], osWaitForever);
    len = LengthOfCQueue(&pcm_queue[stream]);
    osMutexRelease(pcmbuff_mutex_id[stream]);

    return len;
}

int app_ble_i2s_audio_pcmbuff_put(AUD_STREAM_T stream, uint8_t *pcm, uint16_t pcm_len)
{
    int status;

    osMutexWait(pcmbuff_mutex_id[stream], osWaitForever);
    status = EnCQueue(&pcm_queue[stream], pcm, pcm_len);
    osMutexRelease(pcmbuff_mutex_id[stream]);

    return status;
}

int app_ble_i2s_audio_pcmbuff_get(AUD_STREAM_T stream, uint8_t *pcm, uint16_t pcm_len)
{
    unsigned char *e1 = NULL, *e2 = NULL;
    unsigned int len1 = 0, len2 = 0;
    int status;

    osMutexWait(pcmbuff_mutex_id[stream], osWaitForever);
    status = PeekCQueue(&pcm_queue[stream], pcm_len, &e1, &len1, &e2, &len2);
    if (pcm_len == (len1 + len2)) {
        memcpy(pcm, e1, len1);
        memcpy(pcm + len1, e2, len2);
        DeCQueue(&pcm_queue[stream], 0, len1);
        DeCQueue(&pcm_queue[stream], 0, len2);
    }else {
        memset(pcm, 0x00, pcm_len);
        status = -1;
    }
    osMutexRelease(pcmbuff_mutex_id[stream]);

    return status;
}

static uint32_t lea_i2s_playback_data_handler_i2s_in(uint8_t *buf, uint32_t len)
{
    app_ble_i2s_audio_pcmbuff_put(AUD_STREAM_PLAYBACK, buf, len);

    return len;
}

static uint32_t lea_i2s_playback_data_handler_dac_out(uint8_t *buf, uint32_t len)
{
    app_ble_i2s_audio_pcmbuff_get(AUD_STREAM_PLAYBACK, (uint8_t *)buf, len);

    if (i2s_audio_source_cb_list.data_playback_cb) {
        i2s_audio_source_cb_list.data_playback_cb(buf, len);
    }

    return len;
}

static uint32_t lea_i2s_playback_data_handler_dac_in(uint8_t *buf, uint32_t len)
{
    if (i2s_audio_source_cb_list.data_capture_cb) {
        i2s_audio_source_cb_list.data_capture_cb(buf, len);
    }

    app_ble_i2s_audio_pcmbuff_put(AUD_STREAM_CAPTURE, buf, len);

    return len;
}

static uint32_t lea_i2s_playback_data_handler_i2s_out(uint8_t *buf, uint32_t len)
{
    app_ble_i2s_audio_pcmbuff_get(AUD_STREAM_CAPTURE, (uint8_t *)buf, len);

    return len;
}

void app_ble_audio_i2s_playback_stream_onoff(bool onoff)
{
    if (lea_i2s_playback_streaming == onoff)
        return;

    if (onoff) {

        app_ble_i2s_audio_pcmbuff_init(AUD_STREAM_PLAYBACK);

        struct AF_STREAM_CONFIG_T stream_cfg;
        memset(&stream_cfg, 0, sizeof(stream_cfg));

        stream_cfg.bits = sample_size_to_enum(sample_size_play);
        stream_cfg.sample_rate = sample_rate_play;
        stream_cfg.channel_num = chan_num_to_enum(CHAN_NUM_PLAYBACK);
        stream_cfg.device = i2s_device;
        stream_cfg.handler = lea_i2s_playback_data_handler_i2s_in;
        stream_cfg.io_path = AUD_INPUT_PATH_LINEIN;
        stream_cfg.data_ptr = i2s_in_buff;
        stream_cfg.data_size = I2S_AUDIO_PLAYBACK_BUFF_SIZE;
        stream_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1);

        if (stream_cfg.device == AUD_STREAM_USE_TDM0_SLAVE) {
            stream_cfg.sync_start = false;
            if (stream_cfg.bits == AUD_BITS_24)
                stream_cfg.slot_cycles = AUD_BITS_32;
            else
                stream_cfg.slot_cycles = stream_cfg.bits;

            stream_cfg.fs_cycles = HAL_TDM_FS_CYCLES_1;
            stream_cfg.align = AUD_DATA_ALIGN_I2S;
            stream_cfg.fs_edge = AUD_FS_FIRST_EDGE_POS;
        }

        af_stream_open(I2S_AUD_STREAM_ID_PLAYBACK, AUD_STREAM_CAPTURE, &stream_cfg);
        af_stream_start(I2S_AUD_STREAM_ID_PLAYBACK, AUD_STREAM_CAPTURE);

        memset(&stream_cfg, 0, sizeof(stream_cfg));
        stream_cfg.bits = sample_size_to_enum(sample_size_play);
        stream_cfg.sample_rate = sample_rate_play;
        stream_cfg.channel_num = chan_num_to_enum(CHAN_NUM_PLAYBACK);
        stream_cfg.device = AUD_STREAM_USE_INT_CODEC;
        stream_cfg.vol = playback_vol;
        stream_cfg.handler = lea_i2s_playback_data_handler_dac_out;
        stream_cfg.io_path = AUD_OUTPUT_PATH_SPEAKER;
        stream_cfg.data_ptr = dac_out_buff;
        stream_cfg.data_size = I2S_AUDIO_PLAYBACK_BUFF_SIZE;
        stream_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1);

        af_stream_open(I2S_AUD_STREAM_ID_PLAYBACK, AUD_STREAM_PLAYBACK, &stream_cfg);

        if (i2s_audio_source_cb_list.data_prep_cb != NULL) {
            i2s_audio_source_cb_list.data_prep_cb(AUD_STREAM_PLAYBACK);
        }
        af_stream_start(I2S_AUD_STREAM_ID_PLAYBACK, AUD_STREAM_PLAYBACK);

        lea_i2s_playback_streaming = true;

    }else {
        af_stream_stop(I2S_AUD_STREAM_ID_PLAYBACK, AUD_STREAM_CAPTURE);
        af_stream_stop(I2S_AUD_STREAM_ID_PLAYBACK, AUD_STREAM_PLAYBACK);

        af_stream_close(I2S_AUD_STREAM_ID_PLAYBACK, AUD_STREAM_CAPTURE);
        af_stream_close(I2S_AUD_STREAM_ID_PLAYBACK, AUD_STREAM_PLAYBACK);

        lea_i2s_playback_streaming = false;
    }
}

void app_ble_audio_i2s_capture_stream_onoff(bool onoff)
{
    if (lea_i2s_capture_streaming == onoff)
        return;

    if (onoff) {

        app_ble_i2s_audio_pcmbuff_init(AUD_STREAM_CAPTURE);

        struct AF_STREAM_CONFIG_T stream_cfg;
        memset(&stream_cfg, 0, sizeof(stream_cfg));

        stream_cfg.bits = sample_size_to_enum(sample_size_play);
        stream_cfg.sample_rate = sample_rate_play;
        stream_cfg.channel_num = chan_num_to_enum(CHAN_NUM_CAPTURE);
        stream_cfg.device = AUD_STREAM_USE_INT_CODEC;
        stream_cfg.vol = capture_vol;
        stream_cfg.handler = lea_i2s_playback_data_handler_dac_in;
        stream_cfg.io_path = AUD_INPUT_PATH_LINEIN;
        stream_cfg.data_ptr = dac_in_buff;
        stream_cfg.data_size = I2S_AUDIO_CAPTURE_BUFF_SIZE;
        stream_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1);

        af_stream_open(I2S_AUD_STREAM_ID_CAPTURE, AUD_STREAM_CAPTURE, &stream_cfg);

        if (i2s_audio_source_cb_list.data_prep_cb != NULL) {
            i2s_audio_source_cb_list.data_prep_cb(AUD_STREAM_CAPTURE);
        }

        af_stream_start(I2S_AUD_STREAM_ID_CAPTURE, AUD_STREAM_CAPTURE);

        memset(&stream_cfg, 0, sizeof(stream_cfg));
        stream_cfg.bits = sample_size_to_enum(sample_size_cap);
        stream_cfg.sample_rate = sample_rate_cap;
        stream_cfg.channel_num = chan_num_to_enum(CHAN_NUM_CAPTURE);
        stream_cfg.device = i2s_device;
        stream_cfg.vol = capture_vol;
        stream_cfg.handler = lea_i2s_playback_data_handler_i2s_out;
        stream_cfg.io_path = AUD_OUTPUT_PATH_SPEAKER;
        stream_cfg.data_ptr = i2s_out_buff;
        stream_cfg.data_size = I2S_AUDIO_CAPTURE_BUFF_SIZE;
        stream_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1);

        af_stream_open(I2S_AUD_STREAM_ID_CAPTURE, AUD_STREAM_PLAYBACK, &stream_cfg);
        af_stream_start(I2S_AUD_STREAM_ID_CAPTURE, AUD_STREAM_PLAYBACK);

        lea_i2s_capture_streaming = true;
    }else {
        af_stream_stop(I2S_AUD_STREAM_ID_CAPTURE, AUD_STREAM_CAPTURE);
        af_stream_stop(I2S_AUD_STREAM_ID_CAPTURE, AUD_STREAM_PLAYBACK);

        af_stream_close(I2S_AUD_STREAM_ID_CAPTURE, AUD_STREAM_CAPTURE);
        af_stream_close(I2S_AUD_STREAM_ID_CAPTURE, AUD_STREAM_PLAYBACK);

        lea_i2s_capture_streaming = false;
    }
}

void app_ble_audio_i2s_playback_stream_start(void)
{
#ifdef WIFI_ALWAYS_AS_SOURCE
    return;
#endif
    if (i2s_audio_source_cb_list.playback_start_cb != NULL) {
        i2s_audio_source_cb_list.playback_start_cb();
    }

    return app_ble_audio_i2s_playback_stream_onoff(true);
}

void app_ble_audio_i2s_playback_stream_stop(void)
{
#ifdef WIFI_ALWAYS_AS_SOURCE
    return;
#endif
    if (i2s_audio_source_cb_list.playback_stop_cb != NULL) {
        i2s_audio_source_cb_list.playback_stop_cb();
    }

    return app_ble_audio_i2s_playback_stream_onoff(false);
}

void app_ble_audio_i2s_capture_stream_start(void)
{
    if (i2s_audio_source_cb_list.capture_start_cb != NULL) {
        i2s_audio_source_cb_list.capture_start_cb();
    }

    return app_ble_audio_i2s_capture_stream_onoff(true);
}

void app_ble_audio_i2s_capture_stream_stop(void)
{
    if (i2s_audio_source_cb_list.capture_stop_cb != NULL) {
        i2s_audio_source_cb_list.capture_stop_cb();
    }

    return app_ble_audio_i2s_capture_stream_onoff(false);
}

void app_ble_audio_i2s_playback_stream_vol_control(uint32_t volume_level)
{
    struct AF_STREAM_CONFIG_T *cfg;
    uint8_t POSSIBLY_UNUSED old_vol;
    uint32_t ret;

    LOG_I("vol_control level:%d", volume_level);

    playback_vol = volume_level;
    ret = af_stream_get_cfg(I2S_AUD_STREAM_ID_PLAYBACK, AUD_STREAM_PLAYBACK, &cfg, true);
    if (ret == 0) {
        old_vol = cfg->vol;
        if (old_vol != volume_level) {
            cfg->vol = volume_level;
            af_stream_setup(I2S_AUD_STREAM_ID_PLAYBACK, AUD_STREAM_PLAYBACK, cfg);
        }
    }

    if (i2s_audio_source_cb_list.playback_vol_change_cb != NULL) {
        i2s_audio_source_cb_list.playback_vol_change_cb(volume_level);
    }
}

uint8_t app_ble_audio_i2s_playback_stream_vol_get(void)
{
    return playback_vol;
}





static bool low_latency_stream_started = false;
static bool resume_low_latency_stream = false;

static uint8_t low_latency_playback_vol = TGT_ADC_VOL_LEVEL_3;

void app_ble_audio_stream_start_pre_handler(void)
{
#ifdef WIFI_ALWAYS_AS_SOURCE
    return;
#endif
    LOG_I("enter %s", __func__);
    if (low_latency_stream_started == true)
    {
        app_ble_audio_low_latency_playback_stream_stop();
        resume_low_latency_stream = true;
    }
}

void app_ble_audio_stream_stop_post_handler(void)
{
#ifdef WIFI_ALWAYS_AS_SOURCE
    return;
#endif
    LOG_I("enter %s", __func__);
    if (resume_low_latency_stream == true)
    {
        app_ble_audio_low_latency_playback_stream_start();
    }
}

void app_ble_audio_low_latency_playback_stream_start(void)
{
    LOG_I("%s", __func__);
#ifdef DMA_AUDIO_APP
    if ((gaf_audio_is_playback_stream_on() == false) &&
#ifndef WIFI_ALWAYS_AS_SOURCE
        (gaf_audio_is_capture_stream_on() == false) &&
#endif
        (low_latency_stream_started == false)) {
        dma_audio_cli_key_on(DAUD_CLI_KEY_NORMAL);
        app_ble_audio_low_latency_playback_stream_vol_control(low_latency_playback_vol);
        low_latency_stream_started = true;
    }
#endif
}

void app_ble_audio_low_latency_playback_stream_stop(void)
{
    LOG_I("%s", __func__);
#ifdef DMA_AUDIO_APP
    if (low_latency_stream_started == true) {
        dma_audio_cli_key_off(DAUD_CLI_KEY_NORMAL);
        low_latency_stream_started = false;
    }
#endif
}

void app_ble_audio_low_latency_playback_stream_vol_control(uint32_t volume_level)
{
#ifdef DMA_AUDIO_APP
    struct AF_STREAM_CONFIG_T *cfg;
    uint8_t POSSIBLY_UNUSED old_vol;
    uint32_t ret;

    LOG_I("vol_control level:%d", volume_level);

    low_latency_playback_vol = volume_level;
    ret = af_stream_get_cfg(DAUD_STREAM_ID, AUD_STREAM_PLAYBACK, &cfg, true);
    if (ret == 0) {
        old_vol = cfg->vol;
        if (old_vol != volume_level) {
            cfg->vol = volume_level;
            af_stream_setup(DAUD_STREAM_ID, AUD_STREAM_PLAYBACK, cfg);
        }
    }
#endif
}

uint8_t app_ble_audio_low_latency_playback_stream_vol_get(void)
{
    return low_latency_playback_vol;
}




#define I2S_BLE_AUDIO_PLAYBACK_TRIGGER_CHANNEL  0
#define I2S_BLE_AUDIO_CAPTURE_TRIGGER_CHANNEL   1

#define I2S_REC_AUDIO_DUMP   0
extern GAF_AUDIO_STREAM_CONTEXT_TYPE_E g_cur_context_type ;
extern GAF_AUDIO_STREAM_ENV_T gaf_mobile_audio_stream_env;

extern "C" uint32_t btdrv_reg_op_cig_anchor_timestamp(uint8_t link_id);

#ifdef BLE_USB_AUDIO_IS_DONGLE_ROLE

static void gaf_mobile_i2s_start_debounce_handler(void const *param);
osTimerDef (GAF_I2S_CAPTURE_START_DEBOUNCE, gaf_mobile_i2s_start_debounce_handler);
static osTimerId gaf_mobile_i2s_start_debounce_timer = NULL;

static void app_ble_i2s_stream_playback_start_supervisor_timer(void);
static void app_ble_i2s_stream_capture_start_supervisor_timer(void);

//TODO: need a better way to replace I2S_CAPTURE_START_DEBOUNCE_TIME_MS
#define I2S_CAPTURE_START_DEBOUNCE_TIME_MS          50

static void i2s_ble_audio_update_timing_test_handler(void)
{
    AOB_BAP_CIG_PARAM_T cig_param;

#ifdef AOB_LOW_LATENCY_MODE
    cig_param.sdu_intv_c2p_us = 5000,  // 5ms = 4 * 1.25ms
#else
#ifdef BLE_AUDIO_FRAME_DUR_7_5MS
    cig_param.sdu_intv_c2p_us = 7500,  // 7.5ms = 6 * 1.25ms
#else
    cig_param.sdu_intv_c2p_us = 10000,  // 5ms = 8 * 1.25ms
#endif
#endif

    cig_param.sdu_intv_p2c_us = cig_param.sdu_intv_c2p_us;

    cig_param.bn_c2p = 1,
    cig_param.nse = 2;
    cig_param.bn_p2c = 1,

#ifdef BLE_USB_AUDIO_SUPPORT
    cig_param.ft_c2p = 2,
    cig_param.ft_p2c = 2,
#else
    cig_param.ft_c2p = 3,
    cig_param.ft_p2c = 3,
#endif

#ifdef AOB_LOW_LATENCY_MODE
    cig_param.iso_interval_1_25ms = 4,  // 5ms = 4 * 1.25ms
#else
#ifdef BLE_AUDIO_FRAME_DUR_7_5MS
    cig_param.iso_interval_1_25ms = 6,  // 7.5ms = 6 * 1.25ms
#else
    cig_param.iso_interval_1_25ms = 8,  // 5ms = 8 * 1.25ms
#endif
#endif

    LOG_I("Update USB AUDIO CIG param:");
    LOG_I("m2s_bn %d", cig_param.bn_c2p);
    LOG_I("m2s_nse %d", cig_param.nse);
    LOG_I("m2s_ft %d", cig_param.ft_c2p);
    LOG_I("s2m_bn %d", cig_param.bn_p2c);
    LOG_I("s2m_ft %d", cig_param.ft_p2c);
    LOG_I("iso_interval %d", cig_param.iso_interval_1_25ms);

    bes_ble_bap_ascc_prepare_cig_parameter(0, &cig_param, BLE_AUDIO_CONNECTION_CNT, NULL);
}
#endif

extern uint8_t aob_media_mobile_get_cur_streaming_ase_lid(uint8_t con_lid, \
    AOB_MGR_DIRECTION_E direction);
extern void aob_media_mobile_release_stream(uint8_t ase_id);

void app_ble_i2s_media_ascc_release_stream_test(void)
{
    LOG_D("%s",__func__);
    aob_media_mobile_release_stream(0);
    aob_media_mobile_release_stream(1);
}

void app_ble_i2s_media_ascc_start_stream_test(void)
{
    LOG_D("%s",__func__);

    const bes_gaf_codec_id_t codec_id_lc3 = {{0x06}};

    bes_lea_ase_cfg_param_t ase_to_start =
    {
        BES_BLE_GAF_SAMPLE_FREQ_48000, 120, BES_BLE_GAF_DIRECTION_SINK, &codec_id_lc3,  BES_BLE_GAF_CONTEXT_TYPE_MEDIA_BIT
    };

    for (uint32_t i = 0; i < BLE_AUDIO_CONNECTION_CNT; i++)\
    {
        bes_lea_mobile_stream_start(i, &ase_to_start, true);
    }
}

AOB_MGR_STREAM_STATE_E app_ble_i2s_media_get_cur_ase_state(void)
{
    uint8_t ase_lid = aob_media_mobile_get_cur_streaming_ase_lid(0, AOB_MGR_DIRECTION_SINK);
    return aob_media_mobile_get_cur_ase_state(ase_lid);
}

static void gaf_media_i2s_prepare_playback_trigger(uint8_t trigger_channel)
{
    LOG_D("%s start trigger_channel %d", __func__, trigger_channel);

    uint8_t adma_ch = HAL_DMA_CHAN_NONE;
    uint32_t dma_base;
    af_stream_dma_tc_irq_enable(I2S_AUD_STREAM_ID_PLAYBACK, AUD_STREAM_PLAYBACK);
    adma_ch = af_stream_get_dma_chan(I2S_AUD_STREAM_ID_PLAYBACK, AUD_STREAM_PLAYBACK);
    dma_base = af_stream_get_dma_base_addr(I2S_AUD_STREAM_ID_PLAYBACK, AUD_STREAM_PLAYBACK);
    if(adma_ch != HAL_DMA_CHAN_NONE)
    {
        bt_drv_reg_op_enable_dma_tc(adma_ch&0xFF, dma_base);
    }
    else
    {
        LOG_I("error adma_ch HAL_DMA_CHAN_NONE %d", adma_ch);
    }
    btdrv_syn_clr_trigger(trigger_channel);
    af_codec_set_device_bt_sync_source(AUD_STREAM_USE_INT_CODEC,AUD_STREAM_PLAYBACK, trigger_channel);
    af_codec_sync_config(AUD_STREAM_PLAYBACK, AF_CODEC_SYNC_TYPE_BT, false);
    af_codec_sync_config(AUD_STREAM_PLAYBACK, AF_CODEC_SYNC_TYPE_BT, true);

    LOG_D("%s end", __func__);
}

static void gaf_media_i2s_prepare_capture_trigger(uint8_t trigger_channel)
{
    LOG_D("%s start", __func__);
    uint8_t adma_ch = 0;
    uint32_t dma_base;
    btdrv_syn_clr_trigger(trigger_channel);

    af_stream_dma_tc_irq_enable(I2S_AUD_STREAM_ID_CAPTURE, AUD_STREAM_CAPTURE);
    adma_ch = af_stream_get_dma_chan(I2S_AUD_STREAM_ID_CAPTURE, AUD_STREAM_CAPTURE);
    dma_base = af_stream_get_dma_base_addr(I2S_AUD_STREAM_ID_CAPTURE, AUD_STREAM_CAPTURE);
    if(adma_ch != HAL_DMA_CHAN_NONE)
    {
        bt_drv_reg_op_enable_dma_tc(adma_ch&0xFF, dma_base);
    }

    af_codec_set_device_bt_sync_source(AUD_STREAM_USE_INT_CODEC,AUD_STREAM_CAPTURE, trigger_channel);
    af_codec_sync_config(AUD_STREAM_CAPTURE, AF_CODEC_SYNC_TYPE_BT, false);
    af_codec_sync_config(AUD_STREAM_CAPTURE, AF_CODEC_SYNC_TYPE_BT, true);

    LOG_D("%s end", __func__);
}

static uint32_t gaf_mobile_calculate_trigger_time(GAF_AUDIO_STREAM_ENV_T* pStreamEnv)
{
    uint32_t latest_iso_bt_time = btdrv_reg_op_cig_anchor_timestamp(
        BLE_ISOHDL_TO_ACTID(pStreamEnv->stream_info.captureInfo.aseChInfo[0].iso_channel_hdl));
    uint32_t current_bt_time = gaf_media_sync_get_curr_time();
    uint32_t trigger_bt_time = latest_iso_bt_time+GAF_AUDIO_CAPTURE_TRIGGER_DELAY_IN_US;

    while (trigger_bt_time < current_bt_time)
    {
        trigger_bt_time += pStreamEnv->stream_info.captureInfo.dmaChunkIntervalUs;
    }

    LOG_I("iso anch %u cur time %u trigger time %u",
        latest_iso_bt_time, current_bt_time, trigger_bt_time);

    return trigger_bt_time;
}

static void* gaf_mobile_audio_get_media_stream_env(void)
{
    return &gaf_mobile_audio_stream_env;
}

/**
 ****************************************************************************************
 * @brief Called when cis establishment
 *
 * @return void
 ****************************************************************************************
 */
void gap_mobile_start_i2s_audio_receiving_dma(void)
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv =
        (GAF_AUDIO_STREAM_ENV_T*)gaf_mobile_audio_get_media_stream_env();

    if (pStreamEnv == NULL)
    {
        LOG_D("%s, pStreamEnv is NULL", __func__);
        return;
    }

    LOG_D("%s", __func__);

    uint32_t trigger_bt_time = gaf_mobile_calculate_trigger_time(pStreamEnv);
    gaf_stream_common_set_capture_trigger_time_generic(pStreamEnv, AUD_STREAM_PLAYBACK,trigger_bt_time);
}

void gap_mobile_start_i2s_audio_transmission_dma(void)
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv =
        (GAF_AUDIO_STREAM_ENV_T*)gaf_mobile_audio_get_media_stream_env();

    if (pStreamEnv == NULL)
    {
        LOG_D("%s, pStreamEnv is NULL", __func__);
        return;
    }
    LOG_D("%s", __func__);

    bes_ble_bap_dp_itf_data_come_callback_register((void *)gaf_mobile_audio_receive_data);
}

static void gaf_i2s_mobile_audio_process_pcm_data_send(void *pStreamEnv_,void *payload_,
    uint32_t payload_size, uint32_t ref_time)
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T* )pStreamEnv_;
#if defined(BLE_AUDIO_USE_TWO_CHANNEL_SINK_FOR_DONGLE) || defined(BLE_AUDIO_STEREO_CHAN_OVER_CIS_CNT)
    uint32_t payload_len_per_channel = payload_size;
#else
    uint32_t payload_len_per_channel = payload_size/pStreamEnv->stream_info.captureInfo.num_channels;
#endif
    uint8_t ase_count = gaf_cis_mobile_stream_type_op_rule.capture_ase_count;
    uint8_t *payload = (uint8_t *)payload_;
    for (uint8_t i = 0; i < ase_count; i++) {
        bes_ble_bap_iso_dp_send_data(pStreamEnv->stream_info.captureInfo.aseChInfo[i].ase_handle,
                                pStreamEnv->stream_context.latestCaptureSeqNum,
                                payload, payload_len_per_channel,
                                ref_time);
        LOG_D("send cis ase index %d data len %d.", i, payload_len_per_channel);
#ifndef BLE_AUDIO_STEREO_CHAN_OVER_CIS_CNT
        payload+=payload_len_per_channel;
#endif
    }
}

static void gaf_i2s_mobile_audio_process_pcm_data(GAF_AUDIO_STREAM_ENV_T *_pStreamEnv, uint8_t *ptrBuf, uint32_t length)
{
    uint32_t dmaIrqHappeningTimeUs = 0;
    uint8_t adma_ch = HAL_DMA_CHAN_NONE;
    uint32_t dma_base;
    uint32_t btclk; //hal slot -- 312.5us
    uint16_t btcnt; //hal microsecond -- 0.5 us
    GAF_AUDIO_STREAM_INFO_T stream_info = _pStreamEnv->stream_info;
    GAF_AUDIO_STREAM_CONTEXT_T stream_context = _pStreamEnv->stream_context;

    adma_ch = af_stream_get_dma_chan(I2S_AUD_STREAM_ID_PLAYBACK, AUD_STREAM_PLAYBACK);
    dma_base = af_stream_get_dma_base_addr(I2S_AUD_STREAM_ID_PLAYBACK, AUD_STREAM_PLAYBACK);
    if (adma_ch != HAL_DMA_CHAN_NONE)
    {
        bt_drv_reg_op_dma_tc_clkcnt_get_by_ch(&btclk, &btcnt,adma_ch&0xFF, dma_base);
        dmaIrqHappeningTimeUs = bt_syn_ble_bt_time_to_bts(btclk, btcnt);
    }

    // it's possible the multiple DMA irq triggered message accumulates,
    // so the acuiqred dmaIrqHappeningTimeUs is the last dma irq, which has
    // been handled. For this case, ignore this dma irq message handling
    if ((GAF_CAPTURE_STREAM_STREAMING_TRIGGERED ==stream_context.capture_stream_state) &&
        (dmaIrqHappeningTimeUs ==stream_context.lastCaptureDmaIrqTimeUs))
    {
        LOG_W("accumulated irq messages happen!");
        return;
    }

    if (GAF_CAPTURE_STREAM_STREAMING_TRIGGERED != stream_context.capture_stream_state)
    {
        uint32_t expectedDmaIrqHappeningTimeUs =
            _pStreamEnv->stream_context.lastCaptureDmaIrqTimeUs +
            (uint32_t)_pStreamEnv->stream_context.captureAverageDmaChunkIntervalUs;

        int32_t gapUs = GAF_AUDIO_CLK_32_BIT_DIFF(dmaIrqHappeningTimeUs, expectedDmaIrqHappeningTimeUs);

        int32_t gapUs_abs = GAF_AUDIO_ABS(gapUs);

        if ((gapUs > 0) && (gapUs_abs > (int32_t)_pStreamEnv->stream_info.captureInfo.dmaChunkIntervalUs/2)) {
            LOG_I("%s, gapUs = %d, dmaChunkIntervalUs = %d", __func__, gapUs, _pStreamEnv->stream_info.captureInfo.dmaChunkIntervalUs);
            return;
        }
        gaf_stream_common_update_capture_stream_state(_pStreamEnv,
                                                     GAF_CAPTURE_STREAM_STREAMING_TRIGGERED);
        gaf_stream_common_clr_trigger(stream_context.playbackTriggerChannel);
        uint32_t latest_iso_bt_time = btdrv_reg_op_cig_anchor_timestamp(
            BLE_ISOHDL_TO_ACTID(_pStreamEnv->stream_info.captureInfo.aseChInfo[0].iso_channel_hdl));
        _pStreamEnv->stream_context.usSinceLatestAnchorPoint = GAF_AUDIO_CLK_32_BIT_DIFF(dmaIrqHappeningTimeUs, latest_iso_bt_time);
    }

    gaf_stream_common_playback_timestamp_checker(_pStreamEnv, dmaIrqHappeningTimeUs);

    dmaIrqHappeningTimeUs += (uint32_t)stream_info.captureInfo.dmaChunkIntervalUs;
    LOG_D("length %d encoded_len %d filled timestamp %u", length,
        _pStreamEnv->stream_info.captureInfo.encoded_frame_size,dmaIrqHappeningTimeUs);

    _pStreamEnv->func_list->encoder_func_list->encoder_encode_frame_func(
        _pStreamEnv, dmaIrqHappeningTimeUs,length, ptrBuf,
        &_pStreamEnv->stream_context.codec_alg_context[0],&gaf_i2s_mobile_audio_process_pcm_data_send);
}

static bool gaf_mobile_i2s_is_any_capture_stream_iso_created(GAF_AUDIO_STREAM_ENV_T *pStreamEnv)
{
    GAF_AUDIO_STREAM_COMMON_INFO_T* captureInfo = &(pStreamEnv->stream_info.captureInfo);

    for (uint8_t i = 0; i < GAF_AUDIO_ASE_TOTAL_COUNT; i++)
    {
        if (GAF_AUDIO_INVALID_ISO_CHANNEL != captureInfo->aseChInfo[i].iso_channel_hdl)
        {
            return true;
        }
    }

    return false;
}

POSSIBLY_UNUSED static void gaf_mobile_i2s_processing_received_data(uint8_t* ptrBuf, uint32_t length)
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T*)gaf_mobile_audio_get_media_stream_env();

    if (pStreamEnv && (pStreamEnv->stream_context.capture_stream_state > GAF_CAPTURE_STREAM_INITIALIZED))
    {
        bool isAnyCaptureIsoCreated = gaf_mobile_i2s_is_any_capture_stream_iso_created(pStreamEnv);
        if (isAnyCaptureIsoCreated)
        {
            gaf_i2s_mobile_audio_process_pcm_data(pStreamEnv, ptrBuf, length);
        }
    }
}

static void gaf_i2s_mobile_audio_process_encoded_data(GAF_AUDIO_STREAM_ENV_T *pStreamEnv, uint8_t *ptrBuf, uint32_t length)
{
    LOG_D("%s start, len = %d", __func__,length);
    uint32_t btclk; //hal slot -- 312.5us
    uint16_t btcnt; //hal  microsecond -- 0.5 us
    uint32_t dmaIrqHappeningTimeUs = 0;
    uint8_t adma_ch = HAL_DMA_CHAN_NONE;
    uint32_t dma_base;
    uint32_t len = 0;

    adma_ch = af_stream_get_dma_chan(I2S_AUD_STREAM_ID_CAPTURE, AUD_STREAM_CAPTURE);
    dma_base = af_stream_get_dma_base_addr(I2S_AUD_STREAM_ID_CAPTURE, AUD_STREAM_CAPTURE);
    len = length;
    if (adma_ch != HAL_DMA_CHAN_NONE)
    {
        bt_drv_reg_op_dma_tc_clkcnt_get_by_ch(&btclk, &btcnt, adma_ch&0xFF, dma_base);
        dmaIrqHappeningTimeUs = bt_syn_ble_bt_time_to_bts(btclk, btcnt);
    }

    // it's possible the multiple DMA irq triggered message accumulates,
    // so the acuiqred dmaIrqHappeningTimeUs is the last dma irq, which has
    // been handled. For this case, ignore this dma irq message handling
    if ((GAF_PLAYBACK_STREAM_STREAMING_TRIGGERED ==
    pStreamEnv->stream_context.playback_stream_state) &&
    (dmaIrqHappeningTimeUs ==
    pStreamEnv->stream_context.lastPlaybackDmaIrqTimeUs))
    {
        memset(ptrBuf, 0, length);
        return;
    }

    gaf_stream_common_updated_expeceted_playback_seq_and_time(pStreamEnv, GAF_AUDIO_DFT_PLAYBACK_LIST_IDX, dmaIrqHappeningTimeUs);

    if (GAF_PLAYBACK_STREAM_START_TRIGGERING ==
        pStreamEnv->stream_context.playback_stream_state)
    {
        gaf_stream_common_update_playback_stream_state(pStreamEnv,
            GAF_PLAYBACK_STREAM_STREAMING_TRIGGERED);
        gaf_stream_common_clr_trigger(pStreamEnv->stream_context.captureTriggerChannel);
#ifdef GAF_DECODER_CROSS_CORE_USE_M55
        if (true == is_support_ble_audio_mobile_m55_decode)
        {
            pStreamEnv->stream_context.lastestPlaybackSeqNumR--;
            pStreamEnv->stream_context.lastestPlaybackSeqNumL--;
        }
        else
        {
            pStreamEnv->stream_context.lastestPlaybackSeqNum[GAF_AUDIO_DFT_PLAYBACK_LIST_IDX]--;
        }
#else
            pStreamEnv->stream_context.lastestPlaybackSeqNum[GAF_AUDIO_DFT_PLAYBACK_LIST_IDX]--;
#endif

        LOG_I("Update playback seq to 0x%x", pStreamEnv->stream_context.lastestPlaybackSeqNum[GAF_AUDIO_DFT_PLAYBACK_LIST_IDX]);
    }

    bool isOneChannelNoData = false;
    uint8_t ase_count = gaf_cis_mobile_stream_type_op_rule.playback_ase_count;
    GAF_AUDIO_STREAM_COMMON_INFO_T playbackInfo = pStreamEnv->stream_info.playbackInfo;
    uint8_t num_channels = playbackInfo.num_channels;
    uint32_t sample_cnt;

#ifdef __BLE_AUDIO_24BIT__
    if (num_channels == 1)
    {
        sample_cnt = (len/sizeof(int32_t))/2;
    }
    else if (num_channels == 2)
    {
        sample_cnt = (len/sizeof(int32_t));
    }
    else if (num_channels == 4)
    {
        sample_cnt = (len/sizeof(int32_t))*2;
    }
    else
    {
        RUNTIME_ASSERT(0, "%s:%d,num_channels err:%d", __func__, __LINE__, num_channels)
    }
    int32_t tmp_buf[ase_count][sample_cnt];
#else
    if (num_channels == 1)
    {
        sample_cnt = (len/sizeof(int16_t))/2;
    }
    else if (num_channels == 2)
    {
        sample_cnt = (len/sizeof(int16_t));
    }
    //todo: need to verrity lc3 codec 4ch
    else if (num_channels == 4)
    {
        sample_cnt = (len/sizeof(int16_t))*2;
    }
    else
    {
        RUNTIME_ASSERT(0, "%s:%d,num_channels err:%d", __func__, __LINE__, num_channels)
    }
    int16_t tmp_buf[ase_count][sample_cnt];
#endif
    memset(tmp_buf, 0, sizeof(tmp_buf));

    LOG_I("contextType %d ase_count %d,sample_cnt %d,num_channel %d",
        pStreamEnv->stream_info.contextType, ase_count,sample_cnt,num_channels);

    uint8_t detected_ase_index = 0;
    for (uint8_t i = 0; i < GAF_AUDIO_ASE_TOTAL_COUNT; i++) {
        if (playbackInfo.aseChInfo[i].iso_channel_hdl == GAF_AUDIO_INVALID_ISO_CHANNEL) {
             continue;
        }

        int ret = 0;
        POSSIBLY_UNUSED int32_t diff_bt_time = 0;
        gaf_media_data_t *decoder_frame_p = NULL;
        GAF_AUDIO_STREAM_COMMON_INFO_T playback_Info = pStreamEnv->stream_info.playbackInfo;
        decoder_frame_p = gaf_mobile_audio_get_packet(pStreamEnv, dmaIrqHappeningTimeUs,
                playback_Info.aseChInfo[i].iso_channel_hdl);

        LOG_I("%s %d %d %d length %d", __func__, detected_ase_index, decoder_frame_p->data_len,
                playback_Info.aseChInfo[i].iso_channel_hdl, length);

        ret = pStreamEnv->func_list->decoder_func_list->decoder_decode_frame_func
                (pStreamEnv, decoder_frame_p->data_len, decoder_frame_p->sdu_data,
                &pStreamEnv->stream_context.codec_alg_context[detected_ase_index], tmp_buf[detected_ase_index]);

        LOG_D("dec ret %d ", ret);
        if (detected_ase_index == 0)
        {
            diff_bt_time = GAF_AUDIO_CLK_32_BIT_DIFF(decoder_frame_p->time_stamp +
                                                    (GAF_AUDIO_FIXED_MOBILE_SIDE_PRESENT_LATENCY_US),
                                                    dmaIrqHappeningTimeUs);
            gaf_media_pid_adjust(AUD_STREAM_CAPTURE, &(pStreamEnv->stream_context.playback_pid_env),
                                            diff_bt_time);
            LOG_D("index %d Decoded seq 0x%02x expected play time %u local time %u diff %d", detected_ase_index, decoder_frame_p->pkt_seq_nb,
                    decoder_frame_p->time_stamp, dmaIrqHappeningTimeUs, diff_bt_time);
        }
        gaf_stream_data_free(decoder_frame_p);

        detected_ase_index++;
    }
#if defined(I2S_REC_AUDIO_DUMP)
    for(int ch = 0; ch < num_channels; ch++)
    {
        audio_dump_clear_up();
        audio_dump_add_channel_data(0, tmp_buf+ch*sample_cnt, sample_cnt);
        audio_dump_run();
    }
#endif
    // Merge stream data
    if (!isOneChannelNoData) {
#if BLE_AUDIO_STEREO_CHAN_OVER_CIS_CNT == 1
        memcpy((void *)ptrBuf, (void *)tmp_buf[0], len);
#else
#ifdef __BLE_AUDIO_24BIT__
        uint16_t i = 0;

        if (num_channels = 1)
        {
            for (uint32_t samples = 0; samples < sample_cnt; samples++)
            {
                ((int32_t *)ptrBuf)[i++] = (int32_t)tmp_buf[0][samples];
                ((int32_t *)ptrBuf)[i++] = (int32_t)tmp_buf[1][samples];
            }
        }
        else
        {
            for (uint32_t samples = 0; samples < sample_cnt; samples++)
            {
                ((int32_t *)ptrBuf)[samples] =
                    (int32_t)(((int32_t)tmp_buf[0][samples] + (int32_t)tmp_buf[1][samples])/2);
            }
        }
#else
        uint16_t i = 0;
        if (num_channels == 1)
        {
            for (uint32_t samples = 0; samples < sample_cnt; samples++)
            {
                ((int16_t *)ptrBuf)[i++] = (int16_t)tmp_buf[0][samples];
                ((int16_t *)ptrBuf)[i++] = (int16_t)tmp_buf[1][samples];
            }
        }
        else
        {
            for (uint32_t samples = 0; samples < sample_cnt; samples++)
            {
                ((int16_t *)ptrBuf)[samples] =
                    (int16_t)(((int16_t)tmp_buf[0][samples] + (int16_t)tmp_buf[1][samples])/2);
            }
        }
        //LOG_I("dump source record pcm data:");
        //DUMP8_I(ptrBuf, 16);
#endif
#endif
    } else {
       memset(ptrBuf, 0, length);
    }
}

POSSIBLY_UNUSED static void gaf_mobile_i2s_feed_data(uint8_t* ptrBuf, uint32_t length)
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T*)gaf_mobile_audio_get_media_stream_env();
    if (pStreamEnv && (pStreamEnv->stream_context.playback_stream_state > GAF_PLAYBACK_STREAM_INITIALIZED)) {
        gaf_i2s_mobile_audio_process_encoded_data(pStreamEnv, ptrBuf, length);
    }
}

void gaf_mobile_i2s_dma_playback_stop(void)
{
    uint8_t POSSIBLY_UNUSED adma_ch = HAL_DMA_CHAN_NONE;
    uint32_t dma_base;
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T*)gaf_mobile_audio_get_media_stream_env();

    if (pStreamEnv) {
        af_stream_dma_tc_irq_disable(I2S_AUD_STREAM_ID_PLAYBACK, AUD_STREAM_PLAYBACK);
        adma_ch = af_stream_get_dma_chan(I2S_AUD_STREAM_ID_PLAYBACK, AUD_STREAM_PLAYBACK);
        dma_base = af_stream_get_dma_base_addr(I2S_AUD_STREAM_ID_PLAYBACK, AUD_STREAM_PLAYBACK);
        if (adma_ch != HAL_DMA_CHAN_NONE)
        {
            bt_drv_reg_op_disable_dma_tc(adma_ch&0xFF, dma_base);
        }

        btdrv_syn_clr_trigger(pStreamEnv->stream_context.playbackTriggerChannel);

        if (pStreamEnv->stream_context.capture_stream_state > GAF_CAPTURE_STREAM_INITIALIZED) {
            gaf_stream_common_update_capture_stream_state(pStreamEnv, GAF_CAPTURE_STREAM_INITIALIZED);
        }
    }
}


void gaf_mobile_i2s_dma_capture_stop(void)
{
    uint8_t POSSIBLY_UNUSED adma_ch = HAL_DMA_CHAN_NONE;
    uint32_t dma_base;
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T*)gaf_mobile_audio_get_media_stream_env();

    if (!pStreamEnv)
    {
       return;
    }

    af_stream_dma_tc_irq_disable(I2S_AUD_STREAM_ID_CAPTURE, AUD_STREAM_CAPTURE);
    adma_ch = af_stream_get_dma_chan(I2S_AUD_STREAM_ID_CAPTURE, AUD_STREAM_CAPTURE);
    dma_base = af_stream_get_dma_base_addr(I2S_AUD_STREAM_ID_CAPTURE, AUD_STREAM_CAPTURE);
    if (adma_ch != HAL_DMA_CHAN_NONE)
    {
        bt_drv_reg_op_disable_dma_tc(adma_ch&0xFF, dma_base);
    }

    btdrv_syn_clr_trigger(pStreamEnv->stream_context.captureTriggerChannel);

    bes_ble_bap_dp_itf_data_come_callback_deregister();

    if (pStreamEnv->stream_context.playback_stream_state > GAF_PLAYBACK_STREAM_INITIALIZED)
    {
        gaf_stream_common_update_playback_stream_state(pStreamEnv, GAF_PLAYBACK_STREAM_INITIALIZED);
    }
}

static bool app_ble_is_i2s_playback_on = false;
static bool app_ble_is_i2s_capture_on = false;

static void gaf_mobile_i2s_set_sysfreq(void)
{
    if (app_ble_is_i2s_capture_on && app_ble_is_i2s_playback_on)
    {
        app_sysfreq_req(APP_SYSFREQ_USER_I2S_AUDIO_STREAMING, APP_SYSFREQ_26M);
    }
    else if (app_ble_is_i2s_capture_on && !app_ble_is_i2s_playback_on)
    {
        app_sysfreq_req(APP_SYSFREQ_USER_I2S_AUDIO_STREAMING, APP_SYSFREQ_26M);
    }
    else if (!app_ble_is_i2s_capture_on && app_ble_is_i2s_playback_on)
    {
        app_sysfreq_req(APP_SYSFREQ_USER_I2S_AUDIO_STREAMING, APP_SYSFREQ_26M);
    }
    else
    {
        app_sysfreq_req(APP_SYSFREQ_USER_I2S_AUDIO_STREAMING, APP_SYSFREQ_32K);
        af_set_priority(AF_USER_AI, osPriorityAboveNormal);
    }
}

POSSIBLY_UNUSED static void gaf_mobile_i2s_playback_vol_changed(uint32_t level)
{
    for (uint32_t i = 0; i < BLE_AUDIO_CONNECTION_CNT; i++)
    {
        uint8_t ase_lid = app_bap_uc_cli_get_ase_lid(i, APP_GAF_BAP_UC_ASE_STATE_STREAMING, BES_BLE_GAF_DIRECTION_SINK);
        if (ase_lid != 0xFF)
        {
            aob_mobile_vol_set_abs(i, level);
        }
    }
}

static void gaf_mobile_i2s_start_debounce_handler(void const *param)
{
    if (!app_ble_is_i2s_capture_on) {
        app_ble_i2s_stream_capture_start_supervisor_timer();
        app_ble_is_i2s_capture_on = true;
        gaf_mobile_i2s_set_sysfreq();
        ble_audio_central_stream_send_message(BLE_AUDIO_CENTRAL_REQ_STREAM_CAPTURE_START, 0, 0);
    }
}

POSSIBLY_UNUSED static void gaf_mobile_i2s_playback_start(void)
{
    if (!app_ble_is_i2s_playback_on)
    {
        app_ble_i2s_stream_playback_start_supervisor_timer();
        app_ble_is_i2s_playback_on = true;
        gaf_mobile_i2s_set_sysfreq();
        ble_audio_central_stream_send_message(BLE_AUDIO_CENTRAL_REQ_STREAM_PLAYBACK_START, 0, 0);
    }
}

void gaf_mobile_i2s_playback_stop(void)
{
    if (app_ble_is_i2s_playback_on)
    {
        app_ble_i2s_stream_playback_start_supervisor_timer();
        app_ble_is_i2s_playback_on = false;
        gaf_mobile_i2s_set_sysfreq();
        gaf_mobile_i2s_dma_playback_stop();
        ble_audio_central_stream_send_message(BLE_AUDIO_CENTRAL_REQ_STREAM_PLAYBACK_STOP, 0, 0);
    }
}

POSSIBLY_UNUSED static void gaf_mobile_i2s_capture_start(void)
{
    LOG_D("%s", __func__);
    osTimerStart(gaf_mobile_i2s_start_debounce_timer, I2S_CAPTURE_START_DEBOUNCE_TIME_MS);
}

POSSIBLY_UNUSED static void gaf_mobile_i2s_capture_stop(void)
{
    LOG_D("%s", __func__);
    osTimerStop(gaf_mobile_i2s_start_debounce_timer);
    if (app_ble_is_i2s_capture_on) {
        app_ble_is_i2s_capture_on = false;
        app_ble_i2s_stream_capture_start_supervisor_timer();
        gaf_mobile_i2s_set_sysfreq();
        gaf_mobile_i2s_dma_capture_stop();
        ble_audio_central_stream_send_message(BLE_AUDIO_CENTRAL_REQ_STREAM_CAPTURE_STOP, 0, 0);
    }
}

POSSIBLY_UNUSED static void gaf_mobile_i2s_prepare_handler(enum AUD_STREAM_T stream)
{
    if (AUD_STREAM_PLAYBACK == stream)
    {
        gaf_media_i2s_prepare_playback_trigger(I2S_BLE_AUDIO_PLAYBACK_TRIGGER_CHANNEL);
    }
    else
    {
        gaf_media_i2s_prepare_capture_trigger(I2S_BLE_AUDIO_CAPTURE_TRIGGER_CHANNEL);
    }
}

POSSIBLY_UNUSED static bool gaf_mobile_i2s_reset_codec_feasibility_check_handler(void)
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T*)gaf_mobile_audio_get_media_stream_env();
    if (pStreamEnv)
    {
        LOG_I("cp state %d pb state %d", pStreamEnv->stream_context.capture_stream_state,
            pStreamEnv->stream_context.playback_stream_state);
    }

    if (pStreamEnv &&
        ((GAF_CAPTURE_STREAM_INITIALIZED == pStreamEnv->stream_context.capture_stream_state) ||
        (GAF_CAPTURE_STREAM_START_TRIGGERING == pStreamEnv->stream_context.capture_stream_state)))
    {
        return false;
    }

    if (pStreamEnv &&
        ((GAF_PLAYBACK_STREAM_INITIALIZED == pStreamEnv->stream_context.playback_stream_state) ||
        (GAF_PLAYBACK_STREAM_START_TRIGGERING == pStreamEnv->stream_context.playback_stream_state)))
    {
        return false;
    }

    return true;
}

static const I2S_AUDIO_SOURCE_EVENT_CALLBACK_T ble_i2s_audio_stream_func_cb_list =
{
    .data_playback_cb = gaf_mobile_i2s_processing_received_data,
    .data_capture_cb = gaf_mobile_i2s_feed_data,
    .reset_codec_feasibility_check_cb = gaf_mobile_i2s_reset_codec_feasibility_check_handler,
    .data_prep_cb = gaf_mobile_i2s_prepare_handler,
    .playback_start_cb = gaf_mobile_i2s_playback_start,
    .playback_stop_cb = gaf_mobile_i2s_playback_stop,

    .capture_start_cb = gaf_mobile_i2s_capture_start,
    .capture_stop_cb = gaf_mobile_i2s_capture_stop,

    .playback_vol_change_cb = gaf_mobile_i2s_playback_vol_changed,
};

void app_ble_i2s_stream_stm_post_op_checker(BLE_AUDIO_CENTRAL_STREAM_EVENT_E event)
{
    switch (event)
    {
        case EVT_PLAYBACK_STREAM_STARTED:
        {
            if (!app_ble_is_i2s_playback_on)
            {
                ble_audio_central_stream_send_message(BLE_AUDIO_CENTRAL_REQ_STREAM_PLAYBACK_STOP, 0, 0);
            }
            break;
        }
        case EVT_CAPTURE_STREAM_STARTED:
        {
            if (!app_ble_is_i2s_capture_on)
            {
                ble_audio_central_stream_send_message(BLE_AUDIO_CENTRAL_REQ_STREAM_CAPTURE_STOP, 0, 0);
            }
            break;
        }
        case EVT_PLAYBACK_STREAM_STOPPED:
        {
            if (app_ble_is_i2s_playback_on)
            {
                ble_audio_central_stream_send_message(BLE_AUDIO_CENTRAL_REQ_STREAM_PLAYBACK_START, 0, 0);
            }
            break;
        }
        case EVT_CAPTURE_STREAM_STOPPED:
        {
            if (app_ble_is_i2s_capture_on)
            {
                ble_audio_central_stream_send_message(BLE_AUDIO_CENTRAL_REQ_STREAM_CAPTURE_START, 0, 0);
            }
            break;
        }
        default:
            break;
    }
}

#define BLE_I2S_STREAM_CONTROL_SUPERVISOR_TIMEOUT_MS  3000
#define BLE_USE_STREAM_CONTROL_RETRY_TIMES  5
static void ble_use_stream_playback_control_supervisor_timer_cb(void const *n);
osTimerDef(BLE_I2S_STREAM_PLAYBACK_CONTROL_SUPERVISOR_TIMER, ble_use_stream_playback_control_supervisor_timer_cb);
static osTimerId ble_use_stream_playback_control_supervisor_timer_id = NULL;
static uint8_t ble_i2s_stream_playback_control_try_times_till_now = 0;

static void ble_use_stream_playback_control_supervisor_timer_cb(void const *n)
{
    if (ble_i2s_stream_playback_control_try_times_till_now < BLE_USE_STREAM_CONTROL_RETRY_TIMES)
    {
        if (app_ble_is_i2s_playback_on)
        {
            LOG_I("Start ble i2s playback timeout!Retry count %d.", ble_i2s_stream_playback_control_try_times_till_now);
            ble_audio_central_stream_send_message(BLE_AUDIO_CENTRAL_REQ_STREAM_PLAYBACK_START, 0, 0);
        }
        else
        {
            LOG_I("Stopp ble i2s playback timeout!Retry count %d.", ble_i2s_stream_playback_control_try_times_till_now);
            ble_audio_central_stream_send_message(BLE_AUDIO_CENTRAL_REQ_STREAM_PLAYBACK_STOP, 0, 0);
        }

        ble_i2s_stream_playback_control_try_times_till_now++;
        osTimerStart(ble_use_stream_playback_control_supervisor_timer_id, BLE_I2S_STREAM_CONTROL_SUPERVISOR_TIMEOUT_MS);
    }
}

static void app_ble_i2s_stream_playback_start_supervisor_timer(void)
{
    if (NULL == ble_use_stream_playback_control_supervisor_timer_id)
    {
        ble_use_stream_playback_control_supervisor_timer_id =
            osTimerCreate(osTimer(BLE_I2S_STREAM_PLAYBACK_CONTROL_SUPERVISOR_TIMER), osTimerOnce, NULL);
    }

    ble_i2s_stream_playback_control_try_times_till_now = 0;
    osTimerStart(ble_use_stream_playback_control_supervisor_timer_id, BLE_I2S_STREAM_CONTROL_SUPERVISOR_TIMEOUT_MS);
}

static void app_ble_i2s_stream_playback_stop_supervisor_timer(void)
{
    osTimerStop(ble_use_stream_playback_control_supervisor_timer_id);
}

static void ble_use_stream_capture_control_supervisor_timer_cb(void const *n);
osTimerDef(BLE_I2S_STREAM_CAPTURE_CONTROL_SUPERVISOR_TIMER, ble_use_stream_capture_control_supervisor_timer_cb);
static osTimerId ble_use_stream_capture_control_supervisor_timer_id = NULL;
static uint8_t ble_i2s_stream_capture_control_try_times_till_now = 0;

static void ble_use_stream_capture_control_supervisor_timer_cb(void const *n)
{
    if (ble_i2s_stream_capture_control_try_times_till_now < BLE_USE_STREAM_CONTROL_RETRY_TIMES)
    {
        if (app_ble_is_i2s_capture_on)
        {
            LOG_I("Start ble i2s capture timeout!Retry count %d.", ble_i2s_stream_capture_control_try_times_till_now);
            ble_audio_central_stream_send_message(BLE_AUDIO_CENTRAL_REQ_STREAM_CAPTURE_START, 0, 0);
        }
        else
        {
            LOG_I("Stop ble i2s capture timeout!Retry count %d.", ble_i2s_stream_capture_control_try_times_till_now);
            ble_audio_central_stream_send_message(BLE_AUDIO_CENTRAL_REQ_STREAM_CAPTURE_STOP, 0, 0);
        }

        ble_i2s_stream_capture_control_try_times_till_now++;
        osTimerStart(ble_use_stream_capture_control_supervisor_timer_id, BLE_I2S_STREAM_CONTROL_SUPERVISOR_TIMEOUT_MS);
    }
}

static void app_ble_i2s_stream_capture_start_supervisor_timer(void)
{
    if (NULL == ble_use_stream_capture_control_supervisor_timer_id)
    {
        ble_use_stream_capture_control_supervisor_timer_id =
            osTimerCreate(osTimer(BLE_I2S_STREAM_CAPTURE_CONTROL_SUPERVISOR_TIMER), osTimerOnce, NULL);
    }

    ble_i2s_stream_capture_control_try_times_till_now = 0;
    osTimerStart(ble_use_stream_capture_control_supervisor_timer_id, BLE_I2S_STREAM_CONTROL_SUPERVISOR_TIMEOUT_MS);
}

static void app_ble_i2s_stream_capture_stop_supervisor_timer(void)
{
    osTimerStop(ble_use_stream_capture_control_supervisor_timer_id);
}

void app_ble_i2s_stream_inform_playback_state(uint32_t event)
{
    if (app_ble_is_i2s_playback_on && (EVT_PLAYBACK_STREAM_STARTED == event))
    {
        app_ble_i2s_stream_playback_stop_supervisor_timer();
    }
    else if ((!app_ble_is_i2s_playback_on) && (EVT_PLAYBACK_STREAM_STOPPED == event))
    {
        app_ble_i2s_stream_playback_stop_supervisor_timer();
    }

    ble_audio_central_stream_send_message((BLE_AUDIO_CENTRAL_STREAM_EVENT_E)event, 0, 0);
}

void app_ble_i2s_stream_inform_capture_state(uint32_t event)
{
    if (app_ble_is_i2s_capture_on && (EVT_CAPTURE_STREAM_STARTED == event))
    {
        app_ble_i2s_stream_capture_stop_supervisor_timer();
    }
    else if ((!app_ble_is_i2s_capture_on) && (EVT_CAPTURE_STREAM_STOPPED == event))
    {
        app_ble_i2s_stream_capture_stop_supervisor_timer();
    }

    ble_audio_central_stream_send_message((BLE_AUDIO_CENTRAL_STREAM_EVENT_E)event, 0, 0);
}

void app_ble_i2s_audio_init(void)
{
    ble_audio_central_stream_stm_init();

    ble_audio_central_stream_start_receiving_handler_register(gap_mobile_start_i2s_audio_receiving_dma);;
    ble_audio_central_stream_start_transmission_handler_register(gap_mobile_start_i2s_audio_transmission_dma);
    ble_audio_central_stream_post_operation_check_cb_register(app_ble_i2s_stream_stm_post_op_checker);

    i2s_ble_audio_update_timing_test_handler();
#ifndef DONGLE_AS_I2S_MASTER
    i2s_audio_source_config_init(&ble_i2s_audio_stream_func_cb_list);
#endif

    ble_audio_central_stream_stm_startup();

    if (gaf_mobile_i2s_start_debounce_timer == NULL) {
        gaf_mobile_i2s_start_debounce_timer =
            osTimerCreate (osTimer(GAF_I2S_CAPTURE_START_DEBOUNCE), osTimerOnce, NULL);
    }
}

/// Transmit i2s data to remote side.
int gaf_mobile_i2s_audio_media_stream_start_handler(void* _pStreamEnv)
{
    LOG_I("%s", __func__);

    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T *)_pStreamEnv;
    if (GAF_CAPTURE_STREAM_IDLE == pStreamEnv->stream_context.capture_stream_state)
    {
        app_sysfreq_req(APP_SYSFREQ_USER_AOB_PLAYBACK, APP_SYSFREQ_208M);
        af_set_priority(AF_USER_AI, osPriorityHigh);

        pStreamEnv->func_list->stream_func_list.capture_init_stream_buf_func(pStreamEnv);
        pStreamEnv->func_list->encoder_func_list->encoder_init_func(pStreamEnv);
        pStreamEnv->stream_context.captureTriggerChannel = I2S_BLE_AUDIO_PLAYBACK_TRIGGER_CHANNEL;
        LOG_D("%s captureTriggerChannel %d", __func__,pStreamEnv->stream_context.captureTriggerChannel);

        gaf_media_pid_init(&(pStreamEnv->stream_context.playback_pid_env));

        gaf_stream_common_update_capture_stream_state(pStreamEnv, GAF_CAPTURE_STREAM_INITIALIZED);

        app_ble_i2s_stream_inform_playback_state(EVT_PLAYBACK_STREAM_STARTED);
        LOG_D("%s end", __func__);

        return 0;
    }

    return -1;
}


int gaf_mobile_i2s_audio_media_stream_stop_handler(void* _pStreamEnv)
{
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T *)_pStreamEnv;

    pStreamEnv->func_list->encoder_func_list->encoder_deinit_func(pStreamEnv);
    pStreamEnv->func_list->stream_func_list.capture_deinit_stream_buf_func(pStreamEnv);

    app_bt_sync_release_trigger_channel(pStreamEnv->stream_context.captureTriggerChannel);

    gaf_stream_common_update_capture_stream_state(pStreamEnv, GAF_CAPTURE_STREAM_IDLE);
#ifdef GAF_CODEC_CROSS_CORE
#ifdef DSP_M55
    app_dsp_m55_deinit();
#endif
#endif
    af_set_priority(AF_USER_AI, osPriorityAboveNormal);

    app_ble_i2s_stream_inform_playback_state(EVT_PLAYBACK_STREAM_STOPPED);

    app_sysfreq_req(APP_SYSFREQ_USER_AOB_PLAYBACK, APP_SYSFREQ_32K);
    return 0;
}

int gaf_mobile_i2s_audio_capture_start_handler(void* _pStreamEnv)
{
    LOG_I("%s", __func__);
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T *)_pStreamEnv;
    uint8_t ase_count = gaf_cis_mobile_stream_type_op_rule.playback_ase_count;

    app_sysfreq_req(APP_SYSFREQ_USER_AOB_CAPTURE, APP_SYSFREQ_208M);
    af_set_priority(AF_USER_AI, osPriorityHigh);

    pStreamEnv->func_list->stream_func_list.playback_init_stream_buf_func(pStreamEnv);
    pStreamEnv->func_list->decoder_func_list->decoder_init_func(pStreamEnv, ase_count);
    pStreamEnv->stream_context.playbackTriggerChannel = I2S_BLE_AUDIO_CAPTURE_TRIGGER_CHANNEL;

    gaf_media_pid_init(&(pStreamEnv->stream_context.capture_pid_env));
    gaf_stream_common_update_playback_stream_state(pStreamEnv, GAF_PLAYBACK_STREAM_INITIALIZED);

    app_ble_i2s_stream_inform_capture_state(EVT_CAPTURE_STREAM_STARTED);

#if defined(I2S_REC_AUDIO_DUMP)
    audio_dump_init(480, sizeof(short), 2);
#endif
    return 0;
}

int gaf_mobile_i2s_audio_capture_stop_handler(void* _pStreamEnv)
{
    LOG_I("%s", __func__);
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = (GAF_AUDIO_STREAM_ENV_T *)_pStreamEnv;

    pStreamEnv->func_list->decoder_func_list->decoder_deinit_func();
    pStreamEnv->func_list->stream_func_list.playback_deinit_stream_buf_func(pStreamEnv);
    gaf_stream_common_update_playback_stream_state(pStreamEnv, GAF_PLAYBACK_STREAM_IDLE);

    app_bt_sync_release_trigger_channel(pStreamEnv->stream_context.playbackTriggerChannel);
    af_set_priority(AF_USER_AI, osPriorityAboveNormal);

    app_ble_i2s_stream_inform_capture_state(EVT_CAPTURE_STREAM_STOPPED);

    app_sysfreq_req(APP_SYSFREQ_USER_AOB_CAPTURE, APP_SYSFREQ_32K);

    return 0;
}

bool gaf_mobile_i2s_audio_check_capture_need_start(void)
{
    return true;
    // return i2s_audio_check_capture_need_start();
}

#endif
#endif
