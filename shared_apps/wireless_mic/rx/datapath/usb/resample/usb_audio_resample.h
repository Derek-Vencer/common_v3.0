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
#ifndef _USB_AUDIO_RESAMPLE_H
#define _USB_AUDIO_RESAMPLE_H

#include "usb_audio_queue.h"

#define AUDIO_USB_RESAMPLE_WATER_LINE_UPPER         (AUDIO_USB_DATA_QUEUE_MAX_MTU * 2 / 3)
#define AUDIO_USB_RESAMPLE_WATER_LINE_LOWER         (AUDIO_USB_DATA_QUEUE_MAX_MTU * 1 / 3)
#define AUDIO_USB_RESAMPLE_WATER_LINE_BASE          (AUDIO_USB_DATA_QUEUE_MAX_MTU / 2)

#define AUDIO_USB_RESAMPLE_RATIO_SLOW               (0.000100f)
#define AUDIO_USB_RESAMPLE_RATIO_FAST               (-0.000100f)

typedef struct{
    float proportiongain;
    float integralgain;
    float derivativegain;
    float error[3];
    float result;
} AUDIO_USB_RESAMPLE_PID_T;

void audio_usb_output_stream_resample_init(void);

void audio_usb_output_stream_resample_status_set(bool is_running);

void audio_usb_output_stream_resample_onoff(bool onoff);

bool audio_usb_output_stream_resample_is_on(void);

bool audio_usb_output_stream_resample_running(void);

void audio_usb_output_stream_resample_process(uint8_t* buff, uint32_t len);

void audio_usb_output_stream_resample_tune(float ratio);

int audio_usb_resample_data_enqueue(uint8_t* buff, uint16_t len);

int audio_usb_resample_data_dequeue(uint8_t* buff, uint16_t len);

int audio_usb_resample_data_queue_get_length(void);

#endif