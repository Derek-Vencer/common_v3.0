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

#include "cmsis.h"
#include "cmsis_os.h"
#include "hal_trace.h"
#include "cqueue.h"
#include "usb_audio_queue.h"
#include "usb_audio_resample.h"
#include "app_wireless_mic.h"

static osMutexId audio_usb_data_queue_mutex_id = NULL;
static const osMutexAttr_t audio_usb_data_queue_mutex_attr = {
    .name = "audio_usb_data_queue_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit | osMutexRobust,
    .cb_mem = NULL,
    .cb_size = 0U,
};

static CQueue audio_usb_data_queue;
static uint8_t audio_usb_data_queue_buffer[AUDIO_USB_DATA_QUEUE_BUFFER_SIZE];

static uint32_t audio_usb_data_queue_lock(void)
{
#ifdef USB_AUDIO_ANY_RESAMPLE
    if (audio_usb_output_stream_resample_is_on() && !(in_isr() || in_int_locked())) {
        osMutexAcquire(audio_usb_data_queue_mutex_id, osWaitForever);
        return 0;
    }
#endif

    return int_lock();
}

static void audio_usb_data_queue_unlock(uint32_t lock)
{
#ifdef USB_AUDIO_ANY_RESAMPLE
    if (audio_usb_output_stream_resample_is_on() && !(in_isr() || in_int_locked())) {
        osMutexRelease(audio_usb_data_queue_mutex_id);
        return;
    }
#endif

    int_unlock(lock);
}

int audio_usb_data_queue_init(void)
{
    uint32_t lock = 0;

    if (audio_usb_data_queue_mutex_id == NULL) {
        audio_usb_data_queue_mutex_id = osMutexNew(&audio_usb_data_queue_mutex_attr);
    }
    ASSERT(audio_usb_data_queue_mutex_id, "%s create mutex failed.", __func__);

    lock = audio_usb_data_queue_lock();

    memset(audio_usb_data_queue_buffer, 0x00, AUDIO_USB_DATA_QUEUE_BUFFER_SIZE);
    InitCQueue(&audio_usb_data_queue, AUDIO_USB_DATA_QUEUE_BUFFER_SIZE, audio_usb_data_queue_buffer);

    audio_usb_data_queue_unlock(lock);

    return 0;
}

int audio_usb_data_queue_reset(void)
{
    uint32_t lock = 0;

    lock = audio_usb_data_queue_lock();

    ResetCQueue(&audio_usb_data_queue);

    audio_usb_data_queue_unlock(lock);

    return 0;
}

int audio_usb_data_enqueue(uint8_t* buff, uint16_t len)
{
    int status = -1;
    uint32_t lock = 0;

    if (buff == NULL) {
        return status;
    }

    lock = audio_usb_data_queue_lock();

    if (AvailableOfCQueue(&audio_usb_data_queue) < len) {
        LOG_ERR(0, "audio_usb_data_queue is full!");
    }

    status = EnCQueue(&audio_usb_data_queue, buff, len);

    audio_usb_data_queue_unlock(lock);

    return status;
}

int audio_usb_data_dequeue(uint8_t* buff, uint16_t len)
{
    int status = -1;
    uint32_t lock = 0;
    unsigned char *e1 = NULL, *e2 = NULL;
    unsigned int len1 = 0, len2 = 0;

    if (buff == NULL) {
        return status;
    }

    lock = audio_usb_data_queue_lock();

    status = PeekCQueue(&audio_usb_data_queue, len, &e1, &len1, &e2, &len2);
    if (len == (len1 + len2)) {
        memcpy(buff, e1, len1);
        memcpy(buff + len1, e2, len2);
        DeCQueue(&audio_usb_data_queue, 0, len1);
        DeCQueue(&audio_usb_data_queue, 0, len2);
    }else {
        memset(buff, 0x00, len);
        status = -1;
    }

    audio_usb_data_queue_unlock(lock);

    return status;
}

int audio_usb_data_queue_get_length(void)
{
    int len = 0;
    uint32_t lock = 0;

    lock = audio_usb_data_queue_lock();

    len = LengthOfCQueue(&audio_usb_data_queue);

    audio_usb_data_queue_unlock(lock);

    return len;
}