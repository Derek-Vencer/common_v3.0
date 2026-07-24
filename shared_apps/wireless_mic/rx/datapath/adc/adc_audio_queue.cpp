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
#include "adc_audio_queue.h"
#include "app_wireless_mic.h"

#define AUDIO_ADC_QUEUE_USE_MUTEX
#ifdef AUDIO_ADC_QUEUE_USE_MUTEX
static osMutexId audio_adc_data_queue_mutex_id = NULL;
static const osMutexAttr_t audio_adc_data_queue_mutex_attr = {
    .name = "adc_queue_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit | osMutexRobust,
    .cb_mem = NULL,
    .cb_size = 0U,
};
#endif

#define AUDIO_ADC_DATA_QUEUE_BUFFER_SIZE            (48 * 4 * 1 * 10)

static CQueue audio_adc_data_queue;
static uint8_t audio_adc_data_queue_buffer[AUDIO_ADC_DATA_QUEUE_BUFFER_SIZE];

static uint32_t audio_adc_data_queue_lock(void)
{
#ifdef AUDIO_ADC_QUEUE_USE_MUTEX
    if (!(in_isr() || in_int_locked())) {
        osMutexAcquire(audio_adc_data_queue_mutex_id, osWaitForever);
    }
    return 0;
#else
    return int_lock();
#endif
}

static void audio_adc_data_queue_unlock(uint32_t lock)
{
#ifdef AUDIO_ADC_QUEUE_USE_MUTEX
    if (!(in_isr() || in_int_locked())) {
        osMutexRelease(audio_adc_data_queue_mutex_id);
    }
#else
    int_unlock(lock);
#endif
}

int audio_adc_data_queue_init(void)
{
    uint32_t lock = 0;

#ifdef AUDIO_ADC_QUEUE_USE_MUTEX
    if (audio_adc_data_queue_mutex_id == NULL) {
        audio_adc_data_queue_mutex_id = osMutexNew(&audio_adc_data_queue_mutex_attr);
    }
    ASSERT(audio_adc_data_queue_mutex_id, "%s create mutex failed.", __func__);
#endif

    lock = audio_adc_data_queue_lock();

    memset(audio_adc_data_queue_buffer, 0x00, AUDIO_ADC_DATA_QUEUE_BUFFER_SIZE);
    InitCQueue(&audio_adc_data_queue, AUDIO_ADC_DATA_QUEUE_BUFFER_SIZE, audio_adc_data_queue_buffer);

    audio_adc_data_queue_unlock(lock);

    return 0;
}

int audio_adc_data_enqueue(uint8_t* buff, uint16_t len)
{
    int status = -1;
    uint32_t lock = 0;

    if (buff == NULL) {
        return status;
    }

    lock = audio_adc_data_queue_lock();

    if (AvailableOfCQueue(&audio_adc_data_queue) < len) {
        LOG_ERR(0, "audio_adc_data_queue is full!");
        DeCQueue(&audio_adc_data_queue, NULL, len);
    }

    status = EnCQueue(&audio_adc_data_queue, buff, len);

    audio_adc_data_queue_unlock(lock);

    return status;
}

int audio_adc_data_dequeue(uint8_t* buff, uint16_t len)
{
    int status = -1;
    uint32_t lock = 0;
    unsigned char *e1 = NULL, *e2 = NULL;
    unsigned int len1 = 0, len2 = 0;

    if (buff == NULL) {
        return status;
    }

    lock = audio_adc_data_queue_lock();

    status = PeekCQueue(&audio_adc_data_queue, len, &e1, &len1, &e2, &len2);
    if (len == (len1 + len2)) {
        memcpy(buff, e1, len1);
        memcpy(buff + len1, e2, len2);
        DeCQueue(&audio_adc_data_queue, 0, len1);
        DeCQueue(&audio_adc_data_queue, 0, len2);
    }else {
        memset(buff, 0x00, len);
        status = -1;
    }

    audio_adc_data_queue_unlock(lock);

    return status;
}

int audio_adc_data_queue_get_length(void)
{
    int len = 0;
    uint32_t lock = 0;

    lock = audio_adc_data_queue_lock();

    len = LengthOfCQueue(&audio_adc_data_queue);

    audio_adc_data_queue_unlock(lock);

    return len;
}

int audio_adc_data_discard(uint16_t len)
{
    int status = -1;
    uint32_t lock = 0;

    lock = audio_adc_data_queue_lock();

    if (LengthOfCQueue(&audio_adc_data_queue) >= len) {
        status = DeCQueue(&audio_adc_data_queue, NULL, len);
    }

    audio_adc_data_queue_unlock(lock);

    return status;
}