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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "cmsis_os.h"
#include "hal_trace.h"

#include "audio_datapath_manager.h"
#include "audio_datapath_adc.h"
#include "audio_datapath_hfp.h"
#include "hfp_audio_queue.h"
#include "app_wireless_mic.h"

#ifdef UTILS_ESHELL_EN
#include "eshell.h"
#endif

static bool audio_datapath_hfp_output_stream_started = false;
static bool audio_datapath_hfp_output_stream_cache_ok = false;
static bool audio_datapath_hfp_output_stream_need_unlock = false;
static uint8_t* audio_datapath_hfp_output_buff = NULL;
static uint8_t  audio_datapath_hfp_output_buff_idx = 0;
static uint16_t audio_datapath_hfp_output_buff_size = 0;

static osSemaphoreId audio_hfp_out_stream_sem_id = NULL;
static const osSemaphoreAttr_t audio_hfp_out_stream_sem_attr = {
    .name = "audio_hfp_out_stream_sem",
    .attr_bits = 0U,
    .cb_mem = NULL,
    .cb_size = 0U,
};

static osThreadId audio_hfp_proc_tid = NULL;
static void audio_datapath_hfp_process_thread(void* argument);
static const osThreadAttr_t audio_hfp_proc_thread_attr = {
    .name       = "audio_hfp_proc_thread",
    .stack_size = (2 * 1024),
    .priority   = osPriorityHigh7,
};

uint32_t bt_sco_codec_capture_data(uint8_t *buf, uint32_t len);
uint8_t* app_bt_stream_sco_capture_buffer_get(void);
uint32_t app_bt_stream_sco_capture_buffer_size_get(void);

void audio_datapath_hfp_output_stream_lock_sco_player(void)
{
    audio_datapath_hfp_output_stream_need_unlock = true;
    osSemaphoreWait(audio_hfp_out_stream_sem_id, osWaitForever);
}

void audio_datapath_hfp_output_stream_unlock_sco_player(void)
{
    if (audio_datapath_hfp_output_stream_need_unlock) {
        audio_datapath_hfp_output_stream_need_unlock = false;
        osSemaphoreRelease(audio_hfp_out_stream_sem_id);
    }
}

static void audio_datapath_hfp_output_handler(uint8_t* buff, uint32_t len)
{
    audio_hfp_data_enqueue(buff, len);

    if (!audio_datapath_hfp_output_stream_cache_ok) {
        if (audio_hfp_data_queue_get_length() >= AUDIO_DATAPATH_HFP_OUTPUT_FRAME_LEN) {
            audio_datapath_hfp_output_stream_cache_ok = true;
            LOG_MSG(0, "hfp output stream cache ok!");
        }
    }

    audio_datapath_hfp_output_stream_unlock_sco_player();
}

void audio_datapath_hfp_output_stream_process_shot(void)
{
    osSignalSet(audio_hfp_proc_tid, 0x01);
}

void audio_datapath_hfp_output_stream_onoff(bool onoff)
{
    if (audio_datapath_hfp_output_stream_started == onoff) {
        return;
    }

    LOG_MSG(1, "audio_hfp_output_stream %s", onoff == true ? "on" : "off");

    if (onoff) {
        audio_datapath_hfp_output_buff_idx = 0;
        audio_datapath_hfp_output_buff = app_bt_stream_sco_capture_buffer_get();
        audio_datapath_hfp_output_buff_size = app_bt_stream_sco_capture_buffer_size_get() / 2;
        ASSERT(audio_datapath_hfp_output_buff && audio_datapath_hfp_output_buff_size, "%s invalid output buff.", __func__);
        ASSERT(AUDIO_HFP_DATA_QUEUE_BUFFER_SIZE >= audio_datapath_hfp_output_buff_size, "%s hfp data queue too small.", __func__);

        audio_hfp_data_queue_init();
        audio_datapath_codec_capture_stream_with_encode_set(false);
        audio_datapath_codec_capture_stream_output_post_handler_register(audio_datapath_hfp_output_handler);
    } else {
        audio_datapath_hfp_output_stream_cache_ok = false;
        audio_datapath_codec_capture_stream_output_post_handler_degister();
    }

    audio_datapath_hfp_output_stream_started = onoff;
}

void audio_datapath_hfp_init(void)
{
    audio_hfp_data_queue_init();

    if (audio_hfp_proc_tid == NULL) {
        audio_hfp_proc_tid = osThreadNew(audio_datapath_hfp_process_thread, NULL, &audio_hfp_proc_thread_attr);
        ASSERT(audio_hfp_proc_tid, "create audio hfp proc thread failed.");
    }

    if (audio_hfp_out_stream_sem_id == NULL) {
        audio_hfp_out_stream_sem_id = osSemaphoreNew(1, 0, &audio_hfp_out_stream_sem_attr);
    }
    ASSERT(audio_hfp_out_stream_sem_id, "%s create semaphore failed.", __func__);
}

void audio_datapath_hfp_process_thread(void* argument)
{
    osEvent evt;
    uint8_t* data_ptr;

    while (1) {
        evt = osSignalWait(0x0, osWaitForever);

        if (evt.status == osEventSignal) {
            data_ptr = audio_datapath_hfp_output_buff + audio_datapath_hfp_output_buff_idx * audio_datapath_hfp_output_buff_size;

            if (audio_datapath_hfp_output_stream_cache_ok &&
                audio_hfp_data_queue_get_length() >= audio_datapath_hfp_output_buff_size) {
                audio_hfp_data_dequeue(data_ptr, audio_datapath_hfp_output_buff_size);
            } else {
                memset(data_ptr, 0x00, audio_datapath_hfp_output_buff_size);
                LOG_ERR(0, "hfp no playback data!");
            }

            bt_sco_codec_capture_data(data_ptr, audio_datapath_hfp_output_buff_size);

            if (audio_datapath_hfp_output_buff_idx) {
                audio_datapath_hfp_output_buff_idx = 0;
            } else {
                audio_datapath_hfp_output_buff_idx = 1;
            }
        }
    }
}