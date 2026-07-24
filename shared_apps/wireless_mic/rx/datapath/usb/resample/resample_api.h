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
#ifndef _RESAMPLE_API_H
#define _RESAMPLE_API_H

#include "hal_aud.h"

#define AUDIO_USB_RESAMPLE_OUTPUT_IDX_NUM               (2)

typedef void (*AUDIO_USB_RESAMPLE_CALLBACK)(uint8_t ch, uint8_t *buf, uint32_t len);

typedef struct {
    uint8_t ch;
    void *id;
    AUDIO_USB_RESAMPLE_CALLBACK cb;
    uint8_t *iter_buf;
    uint32_t iter_len;
    uint32_t offset;
    float ratio_step;
} AUDIO_USB_RESAMPLE_T;

#ifdef __cplusplus
extern "C" {
#endif


AUDIO_USB_RESAMPLE_T* app_resample_any_open(enum AUD_CHANNEL_NUM_T chans, AUDIO_USB_RESAMPLE_CALLBACK cb, uint32_t iter_len, float ratio_step);

void app_resample_any_close(AUDIO_USB_RESAMPLE_T* resamp);

void app_resample_reset(AUDIO_USB_RESAMPLE_T* resamp);

void app_resample_any_run(AUDIO_USB_RESAMPLE_T* resamp, uint8_t *buf, uint32_t len);

void app_resample_any_tune(AUDIO_USB_RESAMPLE_T* resamp, float ratio);

#ifdef __cplusplus
}
#endif

#endif