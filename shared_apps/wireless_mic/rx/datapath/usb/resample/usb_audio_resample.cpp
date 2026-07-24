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
#if defined(USB_AUDIO_ANY_RESAMPLE) && !defined(CHIP_ROLE_CP) && !defined(CHIP_SUBSYS_BTH)
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "cmsis.h"
#include "cmsis_os.h"
#include "hal_trace.h"
#include "cqueue.h"
#include "audio_resample_ex_32bit.h"
#include "audio_datapath_usb.h"
#include "audio_datapath_process.h"
#include "resample_coef.h"
#include "usb_audio_queue.h"
#include "resample_api.h"
#include "usb_audio_frm_defs.h"
#include "app_wireless_mic.h"
#include "hal_location.h"
#include "usb_audio_resample.h"

#define AUDIO_USB_RESAMPLE_DATA_QUEUE_BUFFER_SIZE             (AUDIO_USB_DATA_QUEUE_BUFFER_SIZE)
#define AUDIO_USB_RESMAPLE_FRAME_LEN                          (48 * 4 * 1)  // 256 / 48 = 5.3

#if defined(AUDIO_USB_RESAMPLE_ON_CP)
#include "audio_cp_subsys_adapter.h"
static bool usb_resample_on_cp = false;
#endif

#if defined(AUDIO_USB_RESAMPLE_ON_BTH)
#include "audio_bt_host_adapter.h"
static bool usb_resample_on_bth = false;
#endif

#if defined(AUDIO_USB_RESAMPLE_ON_CP) || defined(AUDIO_USB_RESAMPLE_ON_BTH)
AON_MBX_DATA_LOC AUDIO_USB_STREAM_DATA_T usb_resample_send = {0};
#else
static uint8_t usb_resample_data_output_buff_idx[CHAN_NUM_SEND] = {0};
static AUDIO_USB_RESAMPLE_T* audio_usb_resampler[CHAN_NUM_SEND] = {NULL};
static uint8_t audio_usb_resample_output_buff[AUDIO_USB_RESAMPLE_OUTPUT_IDX_NUM][AUDIO_USB_RESMAPLE_FRAME_LEN * CHAN_NUM_SEND];
static int32_t audio_usb_resample_input_buffer[CHAN_NUM_SEND][AUDIO_USB_RESMAPLE_FRAME_LEN / sizeof(int32_t)];
#endif

static bool audio_usb_resample_onoff = true;
static bool audio_usb_resample_is_running = false;
static bool audio_usb_resample_data_is_ready = false;

static CQueue audio_usb_resample_data_queue;
static uint8_t audio_usb_resample_data_queue_buffer[AUDIO_USB_RESAMPLE_DATA_QUEUE_BUFFER_SIZE];

bool audio_usb_output_stream_resample_is_on(void)
{
    return audio_usb_resample_onoff;
}

bool audio_usb_output_stream_resample_running(void)
{
    return !!(audio_usb_resample_onoff && audio_usb_resample_is_running);
}

static uint32_t audio_usb_resample_data_queue_lock(void)
{
    return int_lock();
}

static void audio_usb_resample_data_queue_unlock(uint32_t lock)
{
    int_unlock(lock);
}

static void audio_usb_resample_data_queue_init(void)
{
    uint32_t lock = 0;

    lock = audio_usb_resample_data_queue_lock();

    memset(audio_usb_resample_data_queue_buffer, 0x00, AUDIO_USB_RESAMPLE_DATA_QUEUE_BUFFER_SIZE);
    InitCQueue(&audio_usb_resample_data_queue, AUDIO_USB_RESAMPLE_DATA_QUEUE_BUFFER_SIZE, audio_usb_resample_data_queue_buffer);

    audio_usb_resample_data_queue_unlock(lock);
}

static void audio_usb_resample_data_queue_reset(void)
{
    uint32_t lock = 0;

    lock = audio_usb_resample_data_queue_lock();

    ResetCQueue(&audio_usb_resample_data_queue);

    audio_usb_resample_data_queue_unlock(lock);
}

int audio_usb_resample_data_enqueue(uint8_t* buff, uint16_t len)
{
    int status = -1;
    uint32_t lock = 0;

    if (buff == NULL) {
        return status;
    }

    lock = audio_usb_resample_data_queue_lock();

    if (AvailableOfCQueue(&audio_usb_resample_data_queue) < len) {
        LOG_ERR(0, "audio_usb_resample_data_queue is full !");
    }

    status = EnCQueue(&audio_usb_resample_data_queue, buff, len);

    audio_usb_resample_data_queue_unlock(lock);

    return status;
}

int audio_usb_resample_data_dequeue(uint8_t* buff, uint16_t len)
{
    int status = -1;
    uint32_t lock = 0;
    unsigned char *e1 = NULL, *e2 = NULL;
    unsigned int len1 = 0, len2 = 0;

    if (buff == NULL) {
        return status;
    }

    lock = audio_usb_resample_data_queue_lock();

    status = PeekCQueue(&audio_usb_resample_data_queue, len, &e1, &len1, &e2, &len2);
    if (len == (len1 + len2)) {
        memcpy(buff, e1, len1);
        memcpy(buff + len1, e2, len2);
        DeCQueue(&audio_usb_resample_data_queue, 0, len1);
        DeCQueue(&audio_usb_resample_data_queue, 0, len2);
    }else {
        memset(buff, 0x00, len);
        status = -1;
    }

    audio_usb_resample_data_queue_unlock(lock);

    return status;
}

int audio_usb_resample_data_queue_get_length(void)
{
    int len = 0;
    uint32_t lock = 0;

    lock = audio_usb_resample_data_queue_lock();

    len = LengthOfCQueue(&audio_usb_resample_data_queue);

    audio_usb_resample_data_queue_unlock(lock);

    return len;
}

void audio_usb_output_stream_resample_init(void)
{
    audio_usb_resample_data_queue_init();

#if defined(AUDIO_USB_RESAMPLE_ON_CP) || defined(AUDIO_USB_ALGO_ON_CP)
    audio_cp_subsys_first_init();
#endif

#if defined(AUDIO_USB_RESAMPLE_ON_BTH) || defined (AUDIO_USB_ALGO_ON_BTH)
    audio_bt_host_first_init();
#endif
}

#if defined(AUDIO_USB_RESAMPLE_ON_CP) || defined(AUDIO_USB_RESAMPLE_ON_BTH)
void audio_usb_output_stream_resample_status_set(bool is_running)
{
    audio_usb_resample_is_running = is_running;
}
#else
//channel num is usb send channel, according to usb host, maybe 2ch or 4ch.
//ch is resample ch idx, maybe 0,1,2,3
static void app_resample_cb(uint8_t ch, uint8_t *buff, uint32_t len)
{
    // LOG_DBG(2, "%s len:%d", __func__, len);
    uint8_t channel_num = *(uint8_t*)audio_datapath_usb_capture_stream_channel_num_get();
    uint16_t output_len = 0;
    uint8_t output_idx = usb_resample_data_output_buff_idx[ch];
    usb_resample_data_output_buff_idx[ch] = (output_idx == AUDIO_USB_RESAMPLE_OUTPUT_IDX_PING) ?
                                            AUDIO_USB_RESAMPLE_OUTPUT_IDX_PONG : AUDIO_USB_RESAMPLE_OUTPUT_IDX_PING;

    //32bit 1ch to 24bit multi(2/4) ch
    uint8_t* resample_buff = audio_usb_resample_output_buff[output_idx];
    for (uint16_t i = 0; i < len / 4; i++) {
        resample_buff[channel_num * 3 * i + ch * 3]     = buff[4 * i];
        resample_buff[channel_num * 3 * i + ch * 3 + 1] = buff[4 * i + 1];
        resample_buff[channel_num * 3 * i + ch * 3 + 2] = buff[4 * i + 2];
    }

    if ((ch + 1) == channel_num) {
        output_len = len * channel_num * 3 / 4;
        audio_usb_resample_data_enqueue(resample_buff, output_len);
    }
}

static void audio_usb_output_stream_resample_run(uint8_t ch, uint8_t* buff, uint32_t len)
{
    if (audio_usb_resampler[ch] != NULL) {
        app_resample_any_run(audio_usb_resampler[ch], buff, len);
    }
}
#endif

void audio_usb_output_stream_resample_tune(float ratio)
{
#if defined(AUDIO_USB_RESAMPLE_ON_CP) || defined(AUDIO_USB_RESAMPLE_ON_BTH)
    usb_resample_send.ratio = ratio;
#else
    for (uint8_t ch = 0; ch < CHAN_NUM_SEND; ch++) {
        app_resample_any_tune(audio_usb_resampler[ch], ratio);
    }
#endif
}

void audio_usb_output_stream_resample_onoff(bool onoff)
{
    uint8_t* usb_channel_num = audio_datapath_usb_capture_stream_channel_num_get();

    LOG_MSG(1, "usb output stream resample: %s", onoff ? "on" : "off");

    if (onoff) {
        if (0) {
        }
#if defined(AUDIO_USB_RESAMPLE_ON_CP)
        else if (*usb_channel_num == 2) {
            audio_cp_subsys_init(APP_CP_SUBSYS_USER_USB);
            audio_cp_subsys_usb_capture_stream_req(USB_STREAM_START, usb_channel_num, 1);
            usb_resample_on_cp = true;
        }
#endif
#if defined(AUDIO_USB_RESAMPLE_ON_BTH)
        else {
            audio_bt_host_init(APP_BT_HOST_USER_USB);
            audio_bt_host_usb_capture_stream_req(USB_STREAM_START, usb_channel_num, 1);
            usb_resample_on_bth = true;
        }
#else
        else {
            for (uint8_t ch = 0; ch < CHAN_NUM_SEND; ch++) {
                if (audio_usb_resampler[ch] == NULL) {
                    audio_usb_resampler[ch] = app_resample_any_open(AUD_CHANNEL_NUM_1, app_resample_cb, AUDIO_USB_RESMAPLE_FRAME_LEN, 1.0f);
                    ASSERT(audio_usb_resampler[ch], "resample open failed.");
                    audio_usb_resampler[ch]->ch = ch;
                    usb_resample_data_output_buff_idx[ch] = AUDIO_USB_RESAMPLE_OUTPUT_IDX_PING;
                }
            }
        }
        audio_usb_resample_is_running = true;
#endif
        audio_usb_resample_data_queue_reset();
    } else {
#if defined(AUDIO_USB_RESAMPLE_ON_CP)
        if (usb_resample_on_cp) {
            usb_resample_on_cp = false;
            audio_cp_subsys_usb_capture_stream_req(USB_STREAM_STOP, NULL, 0);
            osDelay(5);
            audio_cp_subsys_deinit(APP_CP_SUBSYS_USER_USB);
        }
#endif
#if defined(AUDIO_USB_RESAMPLE_ON_BTH)
        if (usb_resample_on_bth) {
            usb_resample_on_bth = false;
            audio_bt_host_usb_capture_stream_req(USB_STREAM_STOP, NULL, 0);
            osDelay(5);
            audio_bt_host_deinit(APP_BT_HOST_USER_USB);
        }
#else
        // for (uint8_t ch = 0; ch < CHAN_NUM_SEND; ch++) {
        //     if (audio_usb_resampler[ch] != NULL) {
        //         app_resample_any_close(audio_usb_resampler[ch]);
        //         audio_usb_resampler[ch] = NULL;
        //     }
        // }
#endif
        audio_usb_resample_is_running = false;
        audio_usb_resample_data_is_ready = false;
        audio_usb_resample_data_queue_reset();
    }
}

void audio_usb_output_stream_resample_process(uint8_t* buff, uint32_t len)
{
    uint32_t raw_data_len = audio_usb_data_queue_get_length();
    uint32_t reample_data_len = audio_usb_resample_data_queue_get_length();
    uint8_t cache_mtu = reample_data_len / len;

    if (audio_usb_resample_data_is_ready) {
        if (cache_mtu) {
            audio_usb_resample_data_dequeue(buff, len);
        } else {
            memset(buff, 0x00, len);
            LOG_ERR(0, "usb no resample data!");
        }
    } else {
        if (cache_mtu >= AUDIO_USB_RESAMPLE_WATER_LINE_BASE) {
            audio_usb_resample_data_is_ready = true;
            LOG_MSG(0, "usb resample data is ready!");
        } else {
            memset(buff, 0x00, len);
            LOG_MSG(0, "usb resample data not ready!");
            return;
        }
    }

    if (cache_mtu >= AUDIO_USB_RESAMPLE_WATER_LINE_UPPER) {
        audio_usb_output_stream_resample_tune(AUDIO_USB_RESAMPLE_RATIO_SLOW);
        LOG_DBG(2, "cache mtu:%d, data len:%4d, %4d, tune slow", cache_mtu, raw_data_len, reample_data_len);
    } else if (cache_mtu <= AUDIO_USB_RESAMPLE_WATER_LINE_LOWER) {
        audio_usb_output_stream_resample_tune(AUDIO_USB_RESAMPLE_RATIO_FAST);
        LOG_DBG(2, "cache mtu:%d, data len:%4d, %4d, tune fast", cache_mtu, raw_data_len, reample_data_len);
    }
}

#ifdef UTILS_ESHELL_EN
#include "eshell.h"
static void audio_usb_resample_onoff_cmd(int argc, char *argv[])
{
    float ratio = 0.f;
    int native = 0;
    int ratio_int = 0;
    bool onoff = false;

    if (argc != 2 && argc != 4) {
        LOG_ERR(0, "error arg.");
        return ;
    }

    onoff = atoi(argv[1]);

    if (audio_usb_resample_onoff != onoff) {
        audio_usb_data_queue_init();
        audio_usb_resample_data_queue_init();
        audio_usb_resample_onoff = onoff;
    }

    if (argc == 4) {
        native = atoi(argv[2]);
        ratio_int = atoi(argv[3]);

        if (native == 0) {
            ratio = ratio_int / 1000000.0;
        } else {
            ratio = ratio_int / 1000000.0 * -1;
        }
        audio_usb_output_stream_resample_tune(ratio);
    }

    LOG_MSG(1, "### audio_usb_resample_onoff = %u", audio_usb_resample_onoff);
    LOG_MSG(1, "### native = %d, ratio = %d, (f*10^6):%d", native, ratio_int, (uint32_t)(ratio*1000000));
}
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "rx_usb_resample", "rx_usb_resample 0/1", audio_usb_resample_onoff_cmd);
#endif
#endif