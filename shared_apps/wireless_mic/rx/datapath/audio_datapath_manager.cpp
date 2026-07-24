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
#include "hal_sysfreq.h"

#include "audioflinger.h"
#include "usb_audio_app.h"
#include "usbaudio_thread.h"

#include "app_wireless_mic.h"
// #include "rx_algo.h"

#ifdef AUDIO_CODEC_ENABLE
#include "audio_datapath_decoder.h"
#endif
#ifdef AUD_OUTPUT_BY_DAC
#include "audio_datapath_dac.h"
#endif
#ifdef AUD_OUTPUT_BY_USB
#include "audio_datapath_usb.h"
#endif
// #include "audio_datapath_i2s.h"
#ifdef AUD_INPUT_BY_ADC
#include "audio_datapath_adc.h"
#endif
#include "audio_datapath_manager.h"
// #include "audio_datapath_process.h"

// #include "audio_detect_insert.h"
#ifdef AUD_INPUT_BY_BLE
#include "audio_datapath_ble.h"
#endif
#ifdef UTILS_ESHELL_EN
#include "eshell.h"
#endif

#define AUDIO_CODEC_PLAYBACK_STREAM_START                        (1 << 0)
#define AUDIO_CODEC_PLAYBACK_STREAM_STOP                         (1 << 1)
#define AUDIO_CODEC_CAPTURE_STREAM_START                         (1 << 2)
#define AUDIO_CODEC_CAPTURE_STREAM_STOP                          (1 << 3)
#define AUDIO_I2S_PLAYBACK_STREAM_START                          (1 << 4)
#define AUDIO_I2S_PLAYBACK_STREAM_STOP                           (1 << 5)
#define AUDIO_USB_CAPTURE_STREAM_START                           (1 << 6)
#define AUDIO_USB_CAPTURE_STREAM_STOP                            (1 << 7)

// static bool audio_manager_master_trigger_slave_to_capture = false;
static bool audio_manager_trigger_slave_algo_on = false;
static bool audio_manager_trigger_slave_encode_on = true;

static uint16_t audio_manager_datapath = 0;

static osMutexId audio_manager_mutex_id = NULL;
static const osMutexAttr_t audio_manager_mutex_attr = {
    .name = "audio_manager_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit | osMutexRobust,
    .cb_mem = NULL,
    .cb_size = 0U,
};

static osThreadId audio_manager_tid = NULL;
static void audio_data_manager_thread(void* argument);
static const osThreadAttr_t audio_data_manager_thread_attr = {
    .name       = "audio_data_manager_thread",
    .stack_size = 2 * 1024,
    .priority   = osPriorityAboveNormal,
};

void audio_datapath_manager_init(void)
{
    if (audio_manager_tid == NULL) {
        audio_manager_tid = osThreadNew(audio_data_manager_thread, NULL, &audio_data_manager_thread_attr);
        ASSERT(audio_manager_tid, "create audio manager thread failed.");
    }

    if (audio_manager_mutex_id == NULL) {
        audio_manager_mutex_id = osMutexNew(&audio_manager_mutex_attr);
        ASSERT(audio_manager_mutex_id, "audio manager mutex failed.");
    }

#ifdef AUDIO_CODEC_ENABLE
    audio_data_decoder_init();
#endif

#ifdef AUD_OUTPUT_BY_DAC
    audio_datapath_dac_init();
#endif

#ifdef AUD_OUTPUT_BY_I2S
    audio_datapath_i2s_init();
#endif

#ifdef AUD_OUTPUT_BY_USB
    audio_datapath_usb_init();
#endif

#ifdef AUD_INPUT_BY_BLE
    audio_datapath_ble_init();
#endif

#ifdef AUD_INPUT_BY_ADC
    audio_datapath_adc_init();
#endif

#ifdef AUD_DETECT_PLUG_INOUT
    audio_detect_insert_init();
#endif
}

void audio_manager_datapath_set(uint16_t path)
{
    osMutexAcquire(audio_manager_mutex_id, osWaitForever);

    uint16_t old_path = audio_manager_datapath;
    audio_manager_datapath |= path;

    LOG_MSG(2, "datapath set %d-->%d", old_path, audio_manager_datapath);

    if (path & AUDIO_INTPUT_PATH_BIT_ADC) {
        audio_manager_codec_capture_stream_start_shot();

        audio_manager_codec_playback_stream_start_shot();
    }

    if (path & AUDIO_OUTPUT_PATH_BIT_I2S) {
        audio_manager_i2s_playback_stream_start_shot();
    }

    // if (old_path == 0 && audio_manager_datapath != 0) {
    //     audio_manager_codec_playback_stream_start_shot();
    // }

    osMutexRelease(audio_manager_mutex_id);
}

void audio_manager_datapath_clear(uint16_t path)
{
    osMutexAcquire(audio_manager_mutex_id, osWaitForever);

    uint16_t old_path = audio_manager_datapath;
    audio_manager_datapath &= ~path;

    LOG_MSG(2, "datapath clear %d-->%d", old_path, audio_manager_datapath);

    if (old_path != 0 && audio_manager_datapath == 0) {
        audio_manager_codec_playback_stream_stop_shot();
    }

    if (path & AUDIO_OUTPUT_PATH_BIT_I2S) {
        audio_manager_i2s_playback_stream_stop_shot();
    }

    if (path & AUDIO_INTPUT_PATH_BIT_ADC) {
        audio_manager_codec_capture_stream_stop_shot();
    }

    osMutexRelease(audio_manager_mutex_id);
}

uint16_t audio_manager_datapath_get(void)
{
    return audio_manager_datapath;
}

void audio_raw_data_dispatch(uint8_t* buff, uint32_t len, bool encoded, uint64_t rx_tsf)
{
    if (!audio_manager_datapath_get()) {
        LOG_DBG(0, "no datapath, ignore raw data.");
        return;
    }

    if (0) {

    }
#ifdef AUDIO_CODEC_ENABLE
    else if (encoded) {
        audio_encoded_data_dispatch(buff, len, rx_tsf);
    }
#endif
    else {
        audio_decoded_data_dispatch(buff, len, rx_tsf);
    }
}

int audio_decoded_data_dispatch(uint8_t* buff, uint32_t len, uint64_t rx_tsf)
{
    return 0;
}

uint8_t audio_ch_to_device_idx(uint8_t ch)
{
    return 0;
}

bool audio_manager_trigger_slave_algo_get(void)
{
    return audio_manager_trigger_slave_algo_on;
}

bool audio_manager_trigger_slave_encode_get(void)
{
    return audio_manager_trigger_slave_encode_on;
}

void audio_manager_codec_playback_stream_start_shot(void)
{
    osSignalSet(audio_manager_tid, AUDIO_CODEC_PLAYBACK_STREAM_START);
}

void audio_manager_codec_playback_stream_stop_shot(void)
{
    osSignalSet(audio_manager_tid, AUDIO_CODEC_PLAYBACK_STREAM_STOP);
}

void audio_manager_codec_capture_stream_start_shot(void)
{
    osSignalSet(audio_manager_tid, AUDIO_CODEC_CAPTURE_STREAM_START);
}

void audio_manager_codec_capture_stream_stop_shot(void)
{
    osSignalSet(audio_manager_tid, AUDIO_CODEC_CAPTURE_STREAM_STOP);
}

void audio_manager_i2s_playback_stream_start_shot(void)
{
    osSignalSet(audio_manager_tid, AUDIO_I2S_PLAYBACK_STREAM_START);
}

void audio_manager_i2s_playback_stream_stop_shot(void)
{
    osSignalSet(audio_manager_tid, AUDIO_I2S_PLAYBACK_STREAM_STOP);
}

void audio_manager_usb_capture_stream_start_shot(void)
{
    osSignalSet(audio_manager_tid, AUDIO_USB_CAPTURE_STREAM_START);
}

void audio_manager_usb_capture_stream_stop_shot(void)
{
    osSignalSet(audio_manager_tid, AUDIO_USB_CAPTURE_STREAM_STOP);
}

void audio_manager_data_cooking_start_shot(uint8_t path)
{
    if (path == AUDIO_OUTPUT_PATH_BIT_DAC) {
        // osSignalSet(audio_process_id, AUDIO_OUTPUT_DATA_COOKING_START);
    }
    // else if ((path == AUDIO_OUTPUT_PATH_BIT_USB) && ((audio_manager_datapath & AUDIO_OUTPUT_PATH_BIT_DAC) == 0)) {
    //     osSignalSet(audio_process_id, AUDIO_OUTPUT_DATA_COOKING_START);
    // } else if ((path == AUDIO_OUTPUT_PATH_BIT_I2S) && ((audio_manager_datapath & AUDIO_OUTPUT_PATH_BIT_DAC) == 0)) {
    //     osSignalSet(audio_process_id, AUDIO_OUTPUT_DATA_COOKING_START);
    // }
}

uint64_t audio_manager_data_cooking_ref_tsf(void)
{
#ifdef AUD_OUTPUT_BY_DAC
    // if (audio_manager_datapath & AUDIO_OUTPUT_PATH_BIT_DAC) {
        return audio_datapath_dac_ref_tsf_get();
    // }
#endif

#ifdef AUD_OUTPUT_BY_USB
    if (audio_manager_datapath & AUDIO_OUTPUT_PATH_BIT_USB) {
        return audio_datapath_usb_ref_tsf_get();
    }
#endif

    return 0;
}

void audio_manager_capture_start(uint8_t session_id)
{

}

void audio_manager_capture_stop(uint8_t session_id)
{

}

uint64_t audio_manager_get_codec_playback_trigger_time(void)
{
    return 0;
}

static void audio_data_manager_thread(void* argument)
{
    osEvent evt;
    POSSIBLY_UNUSED uint32_t signals = 0;

    while (1) {
        evt = osSignalWait(0x0, osWaitForever);
        signals = evt.value.signals;

        if (evt.status == osEventSignal) {

#ifdef AUD_OUTPUT_BY_DAC
            if (signals & AUDIO_CODEC_PLAYBACK_STREAM_STOP) {
                audio_datapath_dac_playback_stream_onoff(false);
            }

            if (signals & AUDIO_CODEC_PLAYBACK_STREAM_START) {
                audio_datapath_dac_playback_stream_onoff(true);
            }
#endif

#ifdef AUD_INPUT_BY_ADC
            if (signals & AUDIO_CODEC_CAPTURE_STREAM_STOP) {
                audio_datapath_adc_capture_stream_onoff(false);
            }

            if (signals & AUDIO_CODEC_CAPTURE_STREAM_START) {
                audio_datapath_adc_capture_stream_onoff(true);
            }
#endif

#ifdef AUD_OUTPUT_BY_I2S
            if (signals & AUDIO_I2S_PLAYBACK_STREAM_STOP) {
                audio_datapath_i2s_playback_stream_onoff(false);
            }

            if (signals & AUDIO_I2S_PLAYBACK_STREAM_START) {
                audio_datapath_i2s_playback_stream_onoff(true);
            }
#endif

#ifdef AUD_OUTPUT_BY_USB
            if (signals & AUDIO_USB_CAPTURE_STREAM_STOP) {
                audio_datapath_usb_capture_stream_onoff(false);
            }

            if (signals & AUDIO_USB_CAPTURE_STREAM_START) {
                audio_datapath_usb_capture_stream_onoff(true);
            }
#endif
        }
    }
}

void audio_manager_audio_session_free(void)
{
    LOG_MSG(1, "%s", __func__);
}

#ifdef UTILS_ESHELL_EN
static void audio_data_manager_dac_output_cmd(int argc, char *argv[])
{
    bool on_off = false;
    if (argc < 2) {
        LOG_ERR(0, "error arg.");
        return ;
    }

    on_off = atoi(argv[1]);
    if (on_off) {
        audio_manager_datapath_set(AUDIO_OUTPUT_PATH_BIT_DAC);
    } else {
        audio_manager_datapath_clear(AUDIO_OUTPUT_PATH_BIT_DAC);
    }
    LOG_MSG(1, "### amgr dac output = %d", on_off);
}
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "rx_dac_output", "rx_dac_output 0/1", audio_data_manager_dac_output_cmd);

static void audio_data_manager_i2s_output_cmd(int argc, char *argv[])
{
    bool on_off = false;
    if (argc < 2) {
        LOG_ERR(0, "error arg.");
        return ;
    }

    on_off = atoi(argv[1]);
    if (on_off) {
        audio_manager_datapath_set(AUDIO_OUTPUT_PATH_BIT_I2S);
    } else {
        audio_manager_datapath_clear(AUDIO_OUTPUT_PATH_BIT_I2S);
    }
    LOG_MSG(1, "### amgr i2s output = %d", on_off);
}
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "rx_i2s_output", "rx_i2s_output 0/1", audio_data_manager_i2s_output_cmd);

static void audio_data_manager_adc_input_cmd(int argc, char *argv[])
{
    bool on_off = false;
    if (argc < 2) {
        LOG_ERR(0, "error arg.");
        return ;
    }

    on_off = atoi(argv[1]);
    if (on_off) {
        audio_manager_codec_capture_stream_start_shot();
    } else {
        audio_manager_codec_capture_stream_stop_shot();
    }
    LOG_MSG(1, "### amgr adc input = %d", on_off);
}
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "rx_adc_input", "rx_adc_input 0/1", audio_data_manager_adc_input_cmd);

static void audio_data_manager_usb_output_cmd(int argc, char *argv[])
{
    bool on_off = false;
    uint8_t channel = 0;
    if (argc < 3) {
        LOG_ERR(0, "error arg.");
        return ;
    }

    on_off = atoi(argv[1]);
    channel = atoi(argv[2]);

    if (on_off) {
        if (channel != *(uint8_t*)audio_datapath_usb_capture_stream_channel_num_get() &&
            audio_datapath_usb_capture_stream_channel_num_set(channel)) {
            audio_manager_usb_capture_stream_stop_shot();
            audio_manager_usb_capture_stream_start_shot();
        }
    } else {
        audio_manager_usb_capture_stream_stop_shot();
    }

    LOG_MSG(1, "### amgr usb output = %d, channel = %d", on_off, channel);
}
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "rx_usb_output", "rx_usb_output 0/1 2/4", audio_data_manager_usb_output_cmd);

static void audio_data_manager_dsp_algo_onoff_cmd(int argc, char *argv[])
{
    if (argc < 2) {
        LOG_ERR(0, "error arg.");
        return ;
    }

    audio_manager_trigger_slave_algo_on = atoi(argv[1]);

    LOG_MSG(1, "### amgr m2s algo onff = %d", audio_manager_trigger_slave_algo_on);
}
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "rx_m2s_algo", "rx_m2s_algo 0/1", audio_data_manager_dsp_algo_onoff_cmd);

static void audio_data_manager_encode_onoff_cmd(int argc, char *argv[])
{
    if (argc < 2) {
        LOG_ERR(0, "error arg.");
        return ;
    }

    audio_manager_trigger_slave_encode_on = atoi(argv[1]);

    LOG_MSG(1, "### amgr m2s encode onff = %d", audio_manager_trigger_slave_encode_on);
}
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "rx_m2s_encode", "rx_m2s_encode 0/1", audio_data_manager_encode_onoff_cmd);

uint8_t d2d_fixed_tx_idx = 0xFF;
static void audio_data_manager_set_tx_idx_cmd(int argc, char *argv[])
{
    if (argc < 2) {
        LOG_ERR(0, "error arg.");
        return ;
    }

    d2d_fixed_tx_idx = atoi(argv[1]);

    LOG_MSG(1, "### amgr set tx idx = %d", d2d_fixed_tx_idx);
}
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "rx_m2s_tx_idx", "rx_m2s_tx_idx 0/1/2/3", audio_data_manager_set_tx_idx_cmd);
#endif