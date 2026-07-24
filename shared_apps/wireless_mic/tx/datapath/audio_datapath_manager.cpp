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

// #include "tx_algo.h"

// #include "lc3_adapter.h"
// #include "audio_datapath_spp.h"
// #include "audio_datapath_hfp.h"
#include "audio_datapath_adc.h"
#include "audio_datapath_ble.h"
#include "app_wireless_mic.h"
#include "audio_datapath_manager.h"

#ifdef AUDIO_CODEC_ENABLE
#include "audio_datapath_encoder.h"
#endif

#ifdef AUD_OUTPUT_BY_HFP
#include "audio_datapath_hfp.h"
#endif

#ifdef AUD_OUTPUT_BY_EMMC
#include "audio_datapath_emmc.h"
#endif

#ifdef AUDIO_ALGO_DSP
#include "vowlan_mcu2dsp_adapter.h"
#endif

#ifdef AUDIO_ALGO_CP_ACCEL
#include "audio_algo_ns_adapter.h"
#endif

#define AUDIO_CODEC_CAPTURE_STREAM_START                        (1 << 0)
#define AUDIO_CODEC_CAPTURE_STREAM_STOP                         (1 << 1)

#define AUDIO_BLE_OUTPUT_STREAM_START                           (1 << 2)
#define AUDIO_BLE_OUTPUT_STREAM_STOP                            (1 << 3)
#define AUDIO_SPP_OUTPUT_STREAM_START                           (1 << 4)
#define AUDIO_SPP_OUTPUT_STREAM_STOP                            (1 << 5)
#define AUDIO_EMMC_OUTPUT_STREAM_START                          (1 << 6)
#define AUDIO_EMMC_OUTPUT_STREAM_STOP                           (1 << 7)
#define AUDIO_HFP_OUTPUT_STREAM_START                           (1 << 8)
#define AUDIO_HFP_OUTPUT_STREAM_STOP                            (1 << 9)

#define AUDIO_CODEC_CAPTURE_STREAM_DSP_ALGO_START               (1 << 10)
#define AUDIO_CODEC_CAPTURE_STREAM_DSP_ALGO_STOP                (1 << 11)

#define AUDIO_CODEC_CAPTURE_STREAM_NS_ALGO_START                (1 << 12)
#define AUDIO_CODEC_CAPTURE_STREAM_NS_ALGO_STOP                 (1 << 13)

static uint16_t audio_manager_datapath = 0;
static uint64_t audio_manager_codec_capture_stream_trigger_time = 0;

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
    audio_datapath_encoder_init();
#endif

#ifdef AUD_INPUT_BY_ADC
    audio_datapath_adc_init();
#endif

#ifdef AUD_OUTPUT_BY_BLE
    audio_datapath_ble_init();
#endif

#ifdef AUD_OUTPUT_BY_SPP
    audio_datapath_spp_init();
#endif

#ifdef AUD_OUTPUT_BY_EMMC
    audio_datapath_emmc_init();
#endif

#ifdef AUD_OUTPUT_BY_HFP
    audio_datapath_hfp_init();
#endif

#ifdef AUD_OUTPUT_BY_USB
    audio_datapath_usb_init();
#endif
}

void audio_manager_codec_capture_stream_trigger_time_set(uint64_t trigger_time)
{
    uint32_t trigger_offset = 0;
    uint8_t remain_mute_cnt = audio_datapath_codec_capture_stream_mute_cnt_get();
    if (audio_datapath_codec_capture_stream_running() && remain_mute_cnt > 0) {
        trigger_offset = remain_mute_cnt * AUDIO_CODEC_CAPTURE_STREAM_DMA_INTERVAL_US;
        if (remain_mute_cnt % 2 != 0) {
            trigger_offset += AUDIO_CODEC_CAPTURE_STREAM_DMA_INTERVAL_US;
        }
        LOG_MSG(2, "trigger time:%u, add offset:%u", (uint32_t)trigger_time, trigger_offset);
    }
    audio_manager_codec_capture_stream_trigger_time = trigger_time + trigger_offset;
}

uint64_t audio_manager_codec_capture_stream_trigger_time_get(void)
{
    return audio_manager_codec_capture_stream_trigger_time;
}

static void audio_manager_codec_capture_stream_start_shot(void)
{
    osSignalSet(audio_manager_tid, AUDIO_CODEC_CAPTURE_STREAM_START);
}

static void audio_manager_codec_capture_stream_stop_shot(void)
{
    osSignalSet(audio_manager_tid, AUDIO_CODEC_CAPTURE_STREAM_STOP);
}

static void audio_manager_ble_output_stream_start_shot(void)
{
    osSignalSet(audio_manager_tid, AUDIO_BLE_OUTPUT_STREAM_START);
}

static void audio_manager_ble_output_stream_stop_shot(void)
{
    osSignalSet(audio_manager_tid, AUDIO_BLE_OUTPUT_STREAM_STOP);
}

static void audio_manager_spp_output_stream_start_shot(void)
{
    osSignalSet(audio_manager_tid, AUDIO_SPP_OUTPUT_STREAM_START);
}

static void audio_manager_spp_output_stream_stop_shot(void)
{
    osSignalSet(audio_manager_tid, AUDIO_SPP_OUTPUT_STREAM_STOP);
}

static void audio_manager_emmc_output_stream_start_shot(void)
{
    osSignalSet(audio_manager_tid, AUDIO_EMMC_OUTPUT_STREAM_START);
}

static void audio_manager_emmc_output_stream_stop_shot(void)
{
    osSignalSet(audio_manager_tid, AUDIO_EMMC_OUTPUT_STREAM_STOP);
}

static void audio_manager_hfp_output_stream_start_shot(void)
{
    osSignalSet(audio_manager_tid, AUDIO_HFP_OUTPUT_STREAM_START);
}

static void audio_manager_hfp_output_stream_stop_shot(void)
{
    osSignalSet(audio_manager_tid, AUDIO_HFP_OUTPUT_STREAM_STOP);
}

void audio_manager_codec_capture_stream_dsp_algo_start_shot(void)
{
    osSignalSet(audio_manager_tid, AUDIO_CODEC_CAPTURE_STREAM_DSP_ALGO_START);
}

void audio_manager_codec_capture_stream_dsp_algo_stop_shot(void)
{
    osSignalSet(audio_manager_tid, AUDIO_CODEC_CAPTURE_STREAM_DSP_ALGO_STOP);
}

void audio_manager_codec_capture_stream_ns_algo_start_shot(void)
{
    osSignalSet(audio_manager_tid, AUDIO_CODEC_CAPTURE_STREAM_NS_ALGO_START);
}

void audio_manager_codec_capture_stream_ns_algo_stop_shot(void)
{
    osSignalSet(audio_manager_tid, AUDIO_CODEC_CAPTURE_STREAM_NS_ALGO_STOP);
}

void audio_manager_datapath_set(uint16_t path)
{
    osMutexAcquire(audio_manager_mutex_id, osWaitForever);

    uint16_t old_path = audio_manager_datapath;
    audio_manager_datapath |= path;

    LOG_MSG(2, "datapath set %d-->%d", old_path, audio_manager_datapath);

    if (old_path == 0 && audio_manager_datapath != 0) {
        audio_manager_codec_capture_stream_start_shot();
    } else {

        if (path & AUDIO_OUTPUT_PATH_BIT_BLE) {
            audio_manager_ble_output_stream_start_shot();
        }

        if (path & AUDIO_OUTPUT_PATH_BIT_SPP) {
            audio_manager_spp_output_stream_start_shot();
        }

        if (path & AUDIO_OUTPUT_PATH_BIT_EMMC) {
            audio_manager_emmc_output_stream_start_shot();
        }

        if (path & AUDIO_OUTPUT_PATH_BIT_HFP) {
            audio_manager_hfp_output_stream_start_shot();
        }
    }

    osMutexRelease(audio_manager_mutex_id);
}

void audio_manager_datapath_clear(uint16_t path)
{
    osMutexAcquire(audio_manager_mutex_id, osWaitForever);

    uint16_t old_path = audio_manager_datapath;
    audio_manager_datapath &= ~path;

    LOG_MSG(2, "datapath clear %d-->%d", old_path, audio_manager_datapath);

    if (path & AUDIO_OUTPUT_PATH_BIT_BLE) {
        audio_manager_ble_output_stream_stop_shot();
    }

    if (path & AUDIO_OUTPUT_PATH_BIT_SPP) {
        audio_manager_spp_output_stream_stop_shot();
    }

    if (path & AUDIO_OUTPUT_PATH_BIT_EMMC) {
        audio_manager_emmc_output_stream_stop_shot();
    }

    if (path & AUDIO_OUTPUT_PATH_BIT_HFP) {
        audio_manager_hfp_output_stream_stop_shot();
    }

    if (old_path != 0 && audio_manager_datapath == 0) {
        audio_manager_codec_capture_stream_stop_shot();
    }

    osMutexRelease(audio_manager_mutex_id);
}

uint16_t audio_manager_datapath_get(void)
{
    return audio_manager_datapath;
}

void audio_manager_codec_capture_stream_start_pre_handler(void)
{
    if (0) {

    }
#ifdef AUD_OUTPUT_BY_BLE
    else if (audio_manager_datapath_get() & AUDIO_OUTPUT_PATH_BIT_BLE) {
        audio_datapath_ble_output_stream_onoff(true);
    }
#endif
#ifdef AUD_OUTPUT_BY_SPP
    else if (audio_manager_datapath_get() & AUDIO_OUTPUT_PATH_BIT_SPP) {
        audio_datapath_spp_output_stream_onoff(true);
    }
#endif
#ifdef AUD_OUTPUT_BY_HFP
    else if (audio_manager_datapath_get() & AUDIO_OUTPUT_PATH_BIT_HFP) {
        audio_datapath_hfp_output_stream_onoff(true);
    }
#endif

#ifdef AUD_OUTPUT_BY_EMMC
    if (audio_manager_datapath_get() & AUDIO_OUTPUT_PATH_BIT_EMMC) {
        audio_datapath_emmc_output_stream_onoff(true);
    }
#endif
}

static void audio_data_manager_thread(void* argument)
{
    osEvent evt;
    uint32_t signals = 0;

    while (1) {
        evt = osSignalWait(0x0, osWaitForever);
        signals = evt.value.signals;

        if (evt.status == osEventSignal) {

#ifdef AUD_INPUT_BY_ADC
            if (signals & AUDIO_CODEC_CAPTURE_STREAM_START) {
                audio_datapath_codec_capture_stream_onoff(true);
            }
#endif

#ifdef AUD_OUTPUT_BY_BLE
            if (signals & AUDIO_BLE_OUTPUT_STREAM_STOP) {
                audio_datapath_ble_output_stream_onoff(false);
            }

            if (signals & AUDIO_BLE_OUTPUT_STREAM_START) {
                audio_datapath_ble_output_stream_onoff(true);
            }
#endif

#ifdef AUD_OUTPUT_BY_SPP
            if (signals & AUDIO_SPP_OUTPUT_STREAM_STOP) {
                audio_datapath_spp_output_stream_onoff(false);
            }

            if (signals & AUDIO_SPP_OUTPUT_STREAM_START) {
                audio_datapath_spp_output_stream_onoff(true);
            }
#endif

#ifdef AUD_OUTPUT_BY_EMMC
            if (signals & AUDIO_EMMC_OUTPUT_STREAM_STOP) {
                audio_datapath_emmc_output_stream_onoff(false);
            }

            if (signals & AUDIO_EMMC_OUTPUT_STREAM_START) {
                audio_datapath_emmc_output_stream_onoff(true);
            }
#endif

#ifdef AUD_OUTPUT_BY_HFP
            if (signals & AUDIO_HFP_OUTPUT_STREAM_STOP) {
                audio_datapath_hfp_output_stream_onoff(false);
            }

            if (signals & AUDIO_HFP_OUTPUT_STREAM_START) {
                audio_datapath_hfp_output_stream_onoff(true);
            }
#endif

#ifdef AUDIO_ALGO_DSP
            if (signals & AUDIO_CODEC_CAPTURE_STREAM_DSP_ALGO_STOP) {
                vowlan_dsp_hifi4_deinit();
            }

            if (signals & AUDIO_CODEC_CAPTURE_STREAM_DSP_ALGO_START) {
                vowlan_dsp_hifi4_init();
            }
#endif

#ifdef AUDIO_ALGO_CP_ACCEL
            if (signals & AUDIO_CODEC_CAPTURE_STREAM_NS_ALGO_STOP) {
                audio_algo_ns_deinit();
            }

            if (signals & AUDIO_CODEC_CAPTURE_STREAM_NS_ALGO_START) {
                audio_algo_ns_init();
            }
#endif

#ifdef AUD_INPUT_BY_ADC
            if (signals & AUDIO_CODEC_CAPTURE_STREAM_STOP) {
                audio_datapath_codec_capture_stream_onoff(false);
            }
#endif
        }
    }
}
