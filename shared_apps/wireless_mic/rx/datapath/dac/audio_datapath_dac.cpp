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
#include <unistd.h>
#include <string.h>
#include "cmsis_os.h"
#include "hal_trace.h"
#include "hal_aud.h"
#include "hal_timer.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "bt_drv_reg_op.h"
#include "bt_drv_interface.h"
#include "gaf_media_sync.h"
#include "audioflinger.h"
#include "usb_audio_app.h"
#include "usbaudio_thread.h"

#ifdef RX_ALGO_PROCESS
#include "rx_algo.h"
#endif
#include "app_wireless_mic.h"
#include "audio_datapath_dac.h"
#include "audio_datapath_manager.h"

#ifdef UTILS_ESHELL_EN
#include "eshell.h"
#endif

static bool codec_playback_stream_mute_onoff = false;
static bool codec_playback_stream_sync_onoff = true;
static bool codec_playback_stream_started = false;
static bool codec_playback_stream_trigger_ok = false;
static bool codec_playback_stream_input_data_ready = false;

static uint32_t codec_playback_stream_trigger_time = 0;
static uint32_t codec_playback_stream_dma_snapshot_tsf = 0;
static int32_t codec_playback_stream_dma_snapshot_offset = 0;

static uint8_t codec_playback_stream_dma_buff[AUDIO_CODEC_PLAYBACK_STREAM_DMA_BUFF_SIZE];
static uint8_t codec_playback_stream_input_data[AUDIO_CODEC_PLAYBACK_STREAM_INPUT_BUFF_SIZE];

static uint32_t codec_playback_stream_anchor_point = 0;
static AUDIO_PLAYBACK_CODEC_PID_T codec_playback_stream_pid;

void audio_datapath_dac_init(void)
{

}

void audio_send_decoded_data_to_dac(uint8_t session_id, uint8_t* buff, uint32_t len, uint64_t play_time)
{
    return;
}

uint32_t audio_datapath_dac_ref_tsf_get(void)
{
    return codec_playback_stream_dma_snapshot_tsf - codec_playback_stream_dma_snapshot_offset;
}

static void audio_codec_playback_stream_sync_tune(float ratio)
{
    // LOG_MSG(1, "tune ratio:%d", (int32_t)(ratio * 1000000));
    af_stream_tune(AUDIO_CODEC_PLAYBACK_STREAM_ID, AUD_STREAM_PLAYBACK, ratio);
}

static void audio_codec_playback_stream_sync_pid_init(void)
{
    memset(&codec_playback_stream_pid, 0x00, sizeof(codec_playback_stream_pid));
    codec_playback_stream_pid.proportiongain = 30;
    codec_playback_stream_pid.integralgain =   10;
    codec_playback_stream_pid.derivativegain = 30;
}

static float audio_codec_playback_stream_sync_pid_calc(AUDIO_PLAYBACK_CODEC_PID_T *pid, int32_t diff)
{
    float increment;
    float pError, dError, iError;

    if (!pid) {
        pid = &codec_playback_stream_pid;
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

uint32_t audio_codec_playback_stream_get_dma_time_us(void)
{
    uint32_t btclk; //hal slot -- 312.5us
    uint16_t btcnt; //hal microsecond -- 0.5 us
    uint8_t dma_ch = af_stream_get_dma_chan(AUDIO_CODEC_PLAYBACK_STREAM_ID, AUD_STREAM_PLAYBACK);
    uint32_t dma_base = af_stream_get_dma_base_addr(AUDIO_CODEC_PLAYBACK_STREAM_ID, AUD_STREAM_PLAYBACK);
    uint32_t dma_time_us = 0;

    if (dma_ch != HAL_DMA_CHAN_NONE) {
        bt_drv_reg_op_dma_tc_clkcnt_get_by_ch(&btclk, &btcnt, dma_ch&0xFF, dma_base);
        dma_time_us = bt_syn_ble_bt_time_to_bts(btclk, btcnt);
    } else {
        LOG_ERR(1, "%s invalid dma ch", __func__);
    }

    return dma_time_us;
}

static uint32_t audio_codec_playback_stream_handler(uint8_t* buff, uint32_t len)
{
    uint32_t current_time = 0;
    uint32_t expect_time = 0;
    int32_t  diff_time = 0;
    float  ratio = 0.f;
    static float last_ratio = 0.f;
    static uint64_t dma_cycle = 0;
    static uint8_t err_cnt = 0;

    current_time = gaf_media_sync_get_curr_time();
    codec_playback_stream_dma_snapshot_tsf = audio_codec_playback_stream_get_dma_time_us();

    if (codec_playback_stream_trigger_ok == false) {
        codec_playback_stream_trigger_ok = true;
        codec_playback_stream_dma_snapshot_offset = codec_playback_stream_dma_snapshot_tsf - \
                codec_playback_stream_trigger_time - AUDIO_CODEC_PLAYBACK_STREAM_DMA_INTERVAL_US;
        LOG_MSG(1, "codec playback stream trigger ok! diff time:%d", codec_playback_stream_dma_snapshot_offset);
    }

    if (!codec_playback_stream_anchor_point) {
        dma_cycle = 0;
        last_ratio = 0.f;
        codec_playback_stream_anchor_point = codec_playback_stream_dma_snapshot_tsf;
        LOG_MSG(1, "codec playback stream anchor point:%u", (uint32_t)codec_playback_stream_anchor_point);
    } else {
        dma_cycle++;
    }

    if (codec_playback_stream_input_data_ready) {
        codec_playback_stream_input_data_ready = false;
#ifdef RX_ALGO_PROCESS
        rx_algo_process(AUDIO_OUTPUT_PATH_BIT_DAC, buff, codec_playback_stream_input_data, sizeof(codec_playback_stream_input_data));
#endif
        if (codec_playback_stream_mute_onoff) {
            memset(buff, 0x00, len);
        }
    } else {
        memset(buff, 0x00, len);
        LOG_MSG(0, "dac no playback data!");
    }

    expect_time = codec_playback_stream_anchor_point + dma_cycle * AUDIO_CODEC_PLAYBACK_STREAM_DMA_INTERVAL_US;
    diff_time = expect_time - codec_playback_stream_dma_snapshot_tsf;

    if (codec_playback_stream_sync_onoff) {
        if (abs(diff_time) > 20) {
            LOG_ERR(1, "unexpected dma tsf:%d", diff_time);
            if (++err_cnt >= 10) {
                LOG_MSG(0, "reset datapath dac anchor point!!!");
                codec_playback_stream_anchor_point = 0;
                err_cnt = 0;
            }
        } else {
            ratio = audio_codec_playback_stream_sync_pid_calc(NULL, -diff_time);
            if (ratio != last_ratio) {
                audio_codec_playback_stream_sync_tune(ratio);
                last_ratio = ratio;
            }
            err_cnt = 0;
        }
    }

    audio_manager_data_cooking_start_shot(AUDIO_OUTPUT_PATH_BIT_DAC);

    LOG_DBG(3, "curr time:%d, expect time:%d, dma time:%d", current_time, expect_time, codec_playback_stream_dma_snapshot_tsf);

    return 0;
}

void audio_data_codec_playback_stream_mute_onoff(bool on_off)
{
    codec_playback_stream_mute_onoff = on_off;
    LOG_MSG(1, "codec_playback_stream_mute %s", on_off == true ? "on" : "off");
}

bool audio_data_codec_playback_stream_running(void)
{
    return !!(codec_playback_stream_started && codec_playback_stream_trigger_ok);
}

void audio_data_codec_playback_stream_data_input(uint8_t* data, uint32_t len)
{
    if (len == AUDIO_CODEC_PLAYBACK_STREAM_INPUT_BUFF_SIZE) {
        memcpy(codec_playback_stream_input_data, data, len);
        codec_playback_stream_input_data_ready = true;
    } else {
        LOG_ERR(2, "%s invalid data len:%d", __func__, len);
    }
}

void audio_datapath_dac_playback_stream_onoff(bool on_off)
{
    if (codec_playback_stream_started == on_off) {
        return;
    }

    LOG_MSG(1, "audio_codec_playback_stream %s", on_off == true ? "on" : "off");

    if (on_off) {
        audio_codec_playback_stream_sync_pid_init();

        struct AF_STREAM_CONFIG_T stream_cfg;
        memset(&stream_cfg, 0, sizeof(stream_cfg));

        stream_cfg.bits = AUDIO_CODEC_PLAYBACK_STREAM_SAMPLE_BITS;
        stream_cfg.sample_rate = AUDIO_CODEC_PLAYBACK_STREAM_SAMPLE_RATE;
        stream_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)AUDIO_CODEC_PLAYBACK_STREAM_CHANNEL_MAP;
        stream_cfg.channel_num = AUDIO_CODEC_PLAYBACK_STREAM_CHANNEL_NUM;
        stream_cfg.vol = TGT_VOLUME_LEVEL_MAX;
        stream_cfg.device = AUDIO_CODEC_PLAYBACK_STREAM_DEVICE;
        stream_cfg.io_path = AUD_OUTPUT_PATH_SPEAKER;
        stream_cfg.handler = audio_codec_playback_stream_handler;
        stream_cfg.data_ptr = codec_playback_stream_dma_buff;
        stream_cfg.data_size = AUDIO_CODEC_PLAYBACK_STREAM_DMA_BUFF_SIZE;
        af_stream_open(AUDIO_CODEC_PLAYBACK_STREAM_ID, AUD_STREAM_PLAYBACK, &stream_cfg);

#ifdef RX_ALGO_PROCESS
        rx_algo_init(AUDIO_OUTPUT_PATH_BIT_DAC, stream_cfg.sample_rate, stream_cfg.data_size / stream_cfg.channel_num / 2 / 4, stream_cfg.channel_num);
#endif

        uint8_t dma_ch = af_stream_get_dma_chan(AUDIO_CODEC_PLAYBACK_STREAM_ID, AUD_STREAM_PLAYBACK);
        uint32_t dma_base = af_stream_get_dma_base_addr(AUDIO_CODEC_PLAYBACK_STREAM_ID, AUD_STREAM_PLAYBACK);

        if (dma_ch != HAL_DMA_CHAN_NONE) {
            bt_drv_reg_op_enable_dma_tc(dma_ch & 0xFF, dma_base);

            af_stream_dma_tc_irq_enable(AUDIO_CODEC_PLAYBACK_STREAM_ID, AUD_STREAM_PLAYBACK);
        } else {
            LOG_ERR(1, "%s invalid dma ch", __func__);
            af_stream_close(AUDIO_CODEC_PLAYBACK_STREAM_ID, AUD_STREAM_PLAYBACK);
            return;
        }

        audio_codec_playback_stream_sync_tune(0);
        af_set_priority(AF_USER_DATAPATH_DAC, osPriorityRealtime);
        af_stream_start(AUDIO_CODEC_PLAYBACK_STREAM_ID, AUD_STREAM_PLAYBACK);
    } else {
        af_stream_stop(AUDIO_CODEC_PLAYBACK_STREAM_ID, AUD_STREAM_PLAYBACK);
        af_stream_close(AUDIO_CODEC_PLAYBACK_STREAM_ID, AUD_STREAM_PLAYBACK);
        af_set_priority(AF_USER_DATAPATH_DAC, osPriorityAboveNormal);
        codec_playback_stream_trigger_ok = false;
        codec_playback_stream_trigger_time = 0;
        codec_playback_stream_anchor_point = 0;
        codec_playback_stream_dma_snapshot_tsf = 0;

#ifdef RX_ALGO_PROCESS
        rx_algo_deinit(AUDIO_OUTPUT_PATH_BIT_DAC);
#endif
    }

    codec_playback_stream_started = on_off;
}