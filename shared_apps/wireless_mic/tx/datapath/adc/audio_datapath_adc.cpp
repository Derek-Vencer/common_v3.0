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
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "cmsis_os.h"
#include "hal_trace.h"
#include "hal_aud.h"
#include "hal_timer.h"
#include "audioflinger.h"
#include "hal_sysfreq.h"
#include "app_utils.h"

#include "app_wireless_mic.h"
#include "audio_datapath_adc.h"
#include "audio_datapath_manager.h"

#ifdef AUDIO_CODEC_ENABLE
#include "audio_datapath_encoder.h"
#endif

#ifdef TX_ALGO_PROCESS
#include "tx_algo.h"
#endif

#ifdef AUDIO_ALGO_DSP
#include "vowlan_mcu2dsp_adapter.h"
#endif

#ifdef UTILS_ESHELL_EN
#include "eshell.h"
#endif

#define APP_DATAPATH_ADC_DYNAMIC_SYSFREQ                (0)

#define APP_DATAPATH_ADC_FREQ_NORMAL                    (APP_SYSFREQ_26M)
#define APP_DATAPATH_ADC_FREQ_BOOST                     (APP_SYSFREQ_208M)

#ifndef APP_SYSFREQ_USER_APP_MIC
#define APP_SYSFREQ_USER_APP_MIC                        (APP_SYSFREQ_USER_APP_20)
#endif

#define ALGO_COST_TIME_DBG_ON                           (1)
#define MAX_TOTAL_TUNE_RATIO                            (0.005000)

static bool audio_datapath_codec_capture_stream_started = false;
static bool audio_datapath_codec_capture_stream_dump_onoff = false;
static bool audio_datapath_codec_capture_stream_with_encode = false;
static bool audio_datapath_codec_capture_stream_trigger_ok = false;
static bool audio_datapath_codec_capture_stream_data_replace = false;
static bool audio_datapath_codec_capture_stream_mute_flag = false;
static bool audio_datapath_codec_capture_stream_dsp_algo_onoff = false;
static bool audio_datapath_codec_capture_stream_ns_algo_onoff = true;

static uint8_t audio_datapath_codec_capture_stream_dump_size = 8;
static uint8_t audio_datapath_codec_capture_stream_mute_cnt = AUDIO_CODEC_CAPTURE_STREAM_MUTE_CNT;
static uint8_t audio_datapath_codec_capture_stream_data_replace_buff[AUDIO_CODEC_CAPTURE_STREAM_DMA_BUFF_SIZE / 2];
static uint8_t audio_datapath_codec_capture_stream_dma_buff[AUDIO_CODEC_CAPTURE_STREAM_DMA_BUFF_SIZE];

static AUDIO_CODEC_PID_T audio_datapath_codec_capture_stream_pid;

static AUDIO_DATAPTH_CODEC_CAPTURE_STREAM_DATAPATH_OUTPUT_CALLBACK capture_stream_datapath_output_pre_handler = NULL;
static AUDIO_DATAPTH_CODEC_CAPTURE_STREAM_DATAPATH_OUTPUT_CALLBACK capture_stream_datapath_output_post_handler = NULL;

uint8_t audio_datapath_codec_capture_stream_mute_cnt_get(void)
{
    return audio_datapath_codec_capture_stream_mute_cnt;
}

void audio_datapath_codec_capture_stream_data_replace_set(bool set)
{
    audio_datapath_codec_capture_stream_data_replace = set;
}

void audio_datapath_codec_capture_stream_data_replace_handler(uint8_t *buff, uint32_t len)
{
    audio_datapath_codec_capture_stream_data_replace = true;
    memcpy(audio_datapath_codec_capture_stream_data_replace_buff, buff, len);
}

void audio_datapath_codec_capture_stream_mute_set(bool set)
{
    if (audio_datapath_codec_capture_stream_mute_flag != set) {
        LOG_MSG(1,"codec capture stream set %s", (set == true) ? "mute" : "unmute");
    }
    audio_datapath_codec_capture_stream_mute_flag = set;
}

void audio_datapath_codec_capture_stream_with_encode_set(bool set)
{
    audio_datapath_codec_capture_stream_with_encode = set;
}

bool audio_datapath_codec_capture_stream_with_encode_get(void)
{
    return audio_datapath_codec_capture_stream_with_encode;
}

void audio_datapath_codec_capture_stream_output_pre_handler_register(AUDIO_DATAPTH_CODEC_CAPTURE_STREAM_DATAPATH_OUTPUT_CALLBACK cb)
{
    capture_stream_datapath_output_pre_handler = cb;
}

void audio_datapath_codec_capture_stream_output_pre_handler_degister(void)
{
    capture_stream_datapath_output_pre_handler = NULL;
}

void audio_datapath_codec_capture_stream_output_post_handler_register(AUDIO_DATAPTH_CODEC_CAPTURE_STREAM_DATAPATH_OUTPUT_CALLBACK cb)
{
    capture_stream_datapath_output_post_handler = cb;
}

void audio_datapath_codec_capture_stream_output_post_handler_degister(void)
{
    capture_stream_datapath_output_post_handler = NULL;
}

bool audio_datapath_codec_capture_stream_running(void)
{
    return audio_datapath_codec_capture_stream_started;
}

void audio_datapath_codec_capture_stream_dsp_algo_onoff_set(bool set)
{
    if (set) {
        audio_manager_codec_capture_stream_dsp_algo_start_shot();
    } else {
        audio_manager_codec_capture_stream_dsp_algo_stop_shot();
    }

    audio_datapath_codec_capture_stream_dsp_algo_onoff = set;
}

bool audio_datapath_codec_capture_stream_dsp_algo_onoff_get(void)
{
    return audio_datapath_codec_capture_stream_dsp_algo_onoff;
}

void audio_datapath_codec_capture_stream_ns_algo_onoff_set(bool set)
{
    if (set) {
        audio_manager_codec_capture_stream_ns_algo_start_shot();
    } else {
        audio_manager_codec_capture_stream_ns_algo_stop_shot();
    }

    audio_datapath_codec_capture_stream_ns_algo_onoff = set;
    LOG_MSG(1,"codec capture stream ns algo set %s", (set == true) ? "on" : "off");
}

bool audio_datapath_codec_capture_stream_ns_algo_onoff_get(void)
{
    return audio_datapath_codec_capture_stream_ns_algo_onoff;
}

void audio_datapath_codec_capture_stream_sync_pid_init(void)
{
    memset(&audio_datapath_codec_capture_stream_pid, 0x00, sizeof(audio_datapath_codec_capture_stream_pid));
    audio_datapath_codec_capture_stream_pid.proportiongain = 30;
    audio_datapath_codec_capture_stream_pid.integralgain =   10;
    audio_datapath_codec_capture_stream_pid.derivativegain = 30;
}

float audio_datapath_codec_capture_stream_sync_pid_calc(AUDIO_CODEC_PID_T *pid, int32_t diff)
{
    float increment;
    float pError, dError, iError;

    if (!pid) {
        pid = &audio_datapath_codec_capture_stream_pid;
    }

    pid->error[0] = diff;
    pError = pid->error[0] - pid->error[1];
    iError = pid->error[0];
    dError = pid->error[0] - 2 * pid->error[1] + pid->error[2];

    increment = (pid->proportiongain * pError + pid->integralgain * iError + pid->derivativegain * dError) * 1.0e-6;

    pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];

    pid->result += increment;
    return pid->result;
}

void audio_datapath_codec_capture_stream_sync_tune(float ratio)
{
    // LOG_MSG(1, "tune ratio:%d", (int32_t)(ratio * 1000000));
    if (abs(ratio) > MAX_TOTAL_TUNE_RATIO) {
        if (ratio > 0) {
            ratio = MAX_TOTAL_TUNE_RATIO;
        } else {
            ratio = -MAX_TOTAL_TUNE_RATIO;
        }
    }
    af_stream_tune(AUDIO_CODEC_CAPTURE_STREAM_ID, AUD_STREAM_CAPTURE, ratio);
}

#ifndef TX_ALGO_PROCESS
void audio_copy_32bit_2ch_data_to_32bit_1ch(uint8_t* dest, uint8_t* src, uint32_t src_bytes)
{
    int32_t* s = (int32_t*)src;
    int32_t* d = (int32_t*)dest;
    uint32_t i = src_bytes / 4 / 2;

    for (; i > 0; i--) {
        d[0] = (s[0] + s[1]);
        d += 1;
        s += 2;
    }
}
#endif

static uint32_t audio_codec_capture_stream_handler(uint8_t *buff, uint32_t len)
{
#if APP_DATAPATH_ADC_DYNAMIC_SYSFREQ
    app_sysfreq_req(APP_SYSFREQ_USER_APP_MIC, APP_DATAPATH_ADC_FREQ_BOOST);
#endif

    uint8_t* data_ptr = buff;
    uint32_t data_len = len;

#if ALGO_COST_TIME_DBG_ON
    uint32_t start_ticks = hal_sys_timer_get();
#endif

    if (!audio_datapath_codec_capture_stream_trigger_ok) {
        audio_datapath_codec_capture_stream_trigger_ok = true;
        LOG_MSG(0, "codec capture stream trigger ok!");
    }

    if (capture_stream_datapath_output_pre_handler) {
        capture_stream_datapath_output_pre_handler(data_ptr, data_len);
    }

    if (audio_datapath_codec_capture_stream_data_replace) {
        memcpy(buff, audio_datapath_codec_capture_stream_data_replace_buff, len);
    }

    if (audio_datapath_codec_capture_stream_dump_onoff) {
        LOG_MSG(3, "af ch:%d, len:%d, ptr:%p", AUDIO_CODEC_CAPTURE_STREAM_CHANNEL_NUM, data_len, data_ptr);
        DUMP8("0x%02x ", data_ptr, audio_datapath_codec_capture_stream_dump_size);
    }

#ifdef AUDIO_DUMP_UPLOAD
    audio_dump_clear_up();
    uint32_t audio_dump_frame_size = len / AUDIO_CODEC_CAPTURE_STREAM_SAMPLE_BYTES / AUDIO_CODEC_CAPTURE_STREAM_CHANNEL_NUM;
    uint32_t audio_dump_multi_channels = AUDIO_CODEC_CAPTURE_STREAM_CHANNEL_NUM;
    audio_dump_add_channel_data_from_multi_channels(0, buff, audio_dump_frame_size, audio_dump_multi_channels, 0);
#endif

#ifdef TX_ALGO_PROCESS
    tx_algo_process(data_ptr, &data_len);
#else
    audio_copy_32bit_2ch_data_to_32bit_1ch(data_ptr, data_ptr, data_len);
    data_len = data_len / 2;
#endif

#ifdef AUDIO_DUMP_UPLOAD
    audio_dump_add_channel_data(1, data_ptr, audio_dump_frame_size);
    audio_dump_run();
#endif

    if (audio_datapath_codec_capture_stream_mute_flag) {
        memset(data_ptr, 0x00, data_len);
    }

#ifdef AUDIO_CODEC_ENABLE
    if (audio_datapath_codec_capture_stream_with_encode) {
        audio_datapath_encode_handler(data_ptr, data_len, &data_ptr, &data_len);
    }
#endif

#if ALGO_COST_TIME_DBG_ON
    static uint32_t max_cost_time = 0;
    uint32_t cost_time = TICKS_TO_US(hal_sys_timer_get() - start_ticks);
    if (cost_time > max_cost_time) {
        max_cost_time = cost_time;
        LOG_MSG(1, "algo max time cost:%uus", max_cost_time);
        // ASSERT(max_cost_time < WIFI_AUDIO_FRAME_MS * 1000, "cost so long time.");
    }
#endif

    if (audio_datapath_codec_capture_stream_mute_cnt > 0) {
        audio_datapath_codec_capture_stream_mute_cnt--;
        memset(data_ptr, 0x00, data_len);
#if APP_DATAPATH_ADC_DYNAMIC_SYSFREQ
        app_sysfreq_req(APP_SYSFREQ_USER_APP_MIC, APP_DATAPATH_ADC_FREQ_NORMAL);
#endif
        return 0;
    }

    if (capture_stream_datapath_output_post_handler) {
        capture_stream_datapath_output_post_handler(data_ptr, data_len);
    }

#if APP_DATAPATH_ADC_DYNAMIC_SYSFREQ
    app_sysfreq_req(APP_SYSFREQ_USER_APP_MIC, APP_DATAPATH_ADC_FREQ_NORMAL);
#endif
    return 0;
}

void audio_datapath_codec_capture_stream_onoff(bool on_off)
{
    if (audio_datapath_codec_capture_stream_started == on_off) {
        return;
    }

    LOG_MSG(1, "audio_codec_capture_stream %s", on_off == true ? "on" : "off");

    if (on_off) {
        app_sysfreq_req(APP_SYSFREQ_USER_APP_MIC, APP_DATAPATH_ADC_FREQ_BOOST);

        struct AF_STREAM_CONFIG_T stream_cfg;

        memset(&stream_cfg, 0, sizeof(struct AF_STREAM_CONFIG_T));
        stream_cfg.bits = AUDIO_CODEC_CAPTURE_STREAM_SAMPLE_BITS;
        stream_cfg.sample_rate = AUDIO_CODEC_CAPTURE_STREAM_SAMPLE_RATE;
        stream_cfg.channel_num = AUDIO_CODEC_CAPTURE_STREAM_CHANNEL_NUM;
        stream_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)AUDIO_CODEC_CAPTURE_STREAM_CHANNEL_MAP;
        stream_cfg.data_ptr = audio_datapath_codec_capture_stream_dma_buff;
        stream_cfg.data_size = AUDIO_CODEC_CAPTURE_STREAM_DMA_BUFF_SIZE;
        stream_cfg.handler = audio_codec_capture_stream_handler;
        stream_cfg.device = AUDIO_CODEC_CAPTURE_STREAM_DEVICE;
        stream_cfg.io_path = AUD_INPUT_PATH_MAINMIC;

#ifdef TX_ALGO_PROCESS
        tx_algo_init(stream_cfg.sample_rate, stream_cfg.data_size / AUDIO_CODEC_CAPTURE_STREAM_SAMPLE_BYTES / stream_cfg.channel_num / 2,
                     stream_cfg.bits, stream_cfg.channel_num, false);
#endif
        audio_datapath_codec_capture_stream_trigger_ok = false;
        audio_datapath_codec_capture_stream_mute_cnt = AUDIO_CODEC_CAPTURE_STREAM_MUTE_CNT;
        audio_datapath_codec_capture_stream_sync_pid_init();

        af_set_priority(AF_USER_DATAPATH_ADC, osPriorityRealtime);
        af_stream_open(AUDIO_CODEC_CAPTURE_STREAM_ID, AUD_STREAM_CAPTURE, &stream_cfg);

        audio_datapath_codec_capture_stream_sync_tune(0);
        audio_manager_codec_capture_stream_start_pre_handler();
        af_stream_start(AUDIO_CODEC_CAPTURE_STREAM_ID, AUD_STREAM_CAPTURE);
    } else {
        af_stream_stop(AUDIO_CODEC_CAPTURE_STREAM_ID, AUD_STREAM_CAPTURE);
        af_stream_close(AUDIO_CODEC_CAPTURE_STREAM_ID, AUD_STREAM_CAPTURE);
        af_set_priority(AF_USER_DATAPATH_ADC, osPriorityAboveNormal);

        audio_datapath_codec_capture_stream_data_replace_set(false);
        audio_datapath_codec_capture_stream_mute_set(false);
#ifdef TX_ALGO_PROCESS
        tx_algo_deinit();
#endif
        app_sysfreq_req(APP_SYSFREQ_USER_APP_MIC, APP_SYSFREQ_32K);
    }

    audio_datapath_codec_capture_stream_started = on_off;
}

void audio_datapath_adc_init(void)
{

}

#ifdef UTILS_ESHELL_EN
static void audio_datapath_codec_capture_stream_onoff_cmd(int argc, char *argv[])
{
    if (argc != 2) {
        LOG_ERR(0, "error arg.");
        return ;
    }

    bool onoff = atoi(argv[1]);

    if (onoff) {
        audio_datapath_codec_capture_stream_onoff(true);
    } else {
        audio_datapath_codec_capture_stream_onoff(false);
    }
}

ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "tx_adc_on", "tx_adc_on 0/1", audio_datapath_codec_capture_stream_onoff_cmd);

static void audio_datapath_codec_capture_stream_dump_cmd(int argc, char *argv[])
{
    if (argc != 2 && argc != 3) {
        LOG_ERR(0, "error arg.");
        return ;
    }
    audio_datapath_codec_capture_stream_dump_onoff = atoi(argv[1]);

    if (argc == 3) {
        audio_datapath_codec_capture_stream_dump_size = atoi(argv[2]) > 32 ? 32 : atoi(argv[2]);
    } else {
        audio_datapath_codec_capture_stream_dump_size = 8;
    }

    LOG_MSG(0,"### audio_datapath_codec_capture_stream_dump_onoff = %u", audio_datapath_codec_capture_stream_dump_onoff);
}

ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "tx_mic_dump", "tx_mic_dump 0/1 1-32", audio_datapath_codec_capture_stream_dump_cmd);

#ifdef AUDIO_ALGO_DSP
static void audio_datapath_codec_dsp_algo_onoff_cmd(int argc, char *argv[])
{
    if (argc < 2) {
        LOG_ERR(0, "error arg.");
        return ;
    }

    audio_datapath_codec_capture_stream_dsp_algo_onoff = atoi(argv[1]);
    LOG_MSG(1, "### audio_datapath_codec_capture_stream_dsp_algo_onoff = %u", audio_datapath_codec_capture_stream_dsp_algo_onoff);
}

static bool audio_datapath_codec_tflm_test_onoff = false;

static void audio_datapath_codec_tflm_test_onoff_cmd(int argc, char *argv[])
{
    if (argc < 2) {
        LOG_ERR(0, "error arg.");
        return ;
    }

    audio_datapath_codec_tflm_test_onoff = atoi(argv[1]);
    if (audio_datapath_codec_tflm_test_onoff) {
        audio_datapath_codec_capture_stream_dsp_algo_onoff = false;
        vowlan_dsp_hifi4_tflm_test_req();
    }

    LOG_MSG(1, "### audio_datapath_codec_tflm_test_onoff = %u", audio_datapath_codec_tflm_test_onoff);
}
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "tx_algo_dsp", "tx_algo_dsp 0/1", audio_datapath_codec_dsp_algo_onoff_cmd);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "tx_tflm_test", "tx_tflm_test 0/1", audio_datapath_codec_tflm_test_onoff_cmd);
#endif
#endif