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
#ifdef USB_AUDIO_ANY_RESAMPLE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "cmsis.h"
#include "hal_trace.h"
#include "audio_resample_ex_32bit.h"
#include "resample_coef.h"
#include "resample_api.h"

// static uint8_t audio_usb_resample_inter_buffer[1652 * WIFI_SESSION_NUM] = {0};
static uint8_t audio_usb_resample_inter_buffer[1652 * 4] = {0};
static uint16_t inter_buffer_idx = 0;
extern "C" struct RESAMPLE_COEF_T resample_coef_any_up256;

AUDIO_USB_RESAMPLE_T* app_resample_any_open(enum AUD_CHANNEL_NUM_T chans, AUDIO_USB_RESAMPLE_CALLBACK cb, uint32_t iter_len, float ratio_step)
{
    const struct RESAMPLE_COEF_T *coef = &resample_coef_any_up256;
    AUDIO_USB_RESAMPLE_T *resamp;
    struct RESAMPLE_CFG_T cfg;
    enum RESAMPLE_STATUS_T status;
    uint32_t size, resamp_size;

    resamp_size = audio_resample_ex_get_buffer_size_32bit(chans, AUD_BITS_32, coef->phase_coef_num);

    size = sizeof(AUDIO_USB_RESAMPLE_T);
    size += ALIGN(iter_len, 4);
    size += resamp_size;

    resamp = (AUDIO_USB_RESAMPLE_T*)&audio_usb_resample_inter_buffer[inter_buffer_idx];
    inter_buffer_idx += sizeof(AUDIO_USB_RESAMPLE_T);
    resamp->cb = cb;
    resamp->iter_buf = &audio_usb_resample_inter_buffer[inter_buffer_idx];
    inter_buffer_idx += ALIGN(iter_len, 4);
    resamp->iter_len = iter_len;
    resamp->offset = iter_len;
    resamp->ratio_step = ratio_step;

    memset(&cfg, 0, sizeof(cfg));
    cfg.chans = chans;
    cfg.bits = AUD_BITS_32;
    cfg.ratio_step = ratio_step;
    cfg.coef = coef;
    cfg.buf = &audio_usb_resample_inter_buffer[inter_buffer_idx];
    cfg.size = resamp_size;
    inter_buffer_idx += resamp_size;

    status = audio_resample_ex_open_32bit(&cfg, (RESAMPLE_ID*)&resamp->id);
    ASSERT(status == RESAMPLE_STATUS_OK, "%s: Failed to open resample: %d", __func__, status);

    TRACE(2, "resample need buff size:%d, inter buff used:%d", size, inter_buffer_idx);
    ASSERT(inter_buffer_idx <= sizeof(audio_usb_resample_inter_buffer), "resample inter buff not enough!");
    return resamp;
}

void app_resample_any_close(AUDIO_USB_RESAMPLE_T* resamp)
{
    audio_resample_ex_close_32bit((RESAMPLE_ID*)resamp->id);
}

void app_resample_reset(AUDIO_USB_RESAMPLE_T* resamp)
{
    audio_resample_ex_flush_32bit((RESAMPLE_ID*)resamp->id);
    resamp->offset = resamp->iter_len;
}

void app_resample_any_run(AUDIO_USB_RESAMPLE_T* resamp, uint8_t *buf, uint32_t len)
{
    uint32_t in_size, out_size;
    struct RESAMPLE_IO_BUF_T io;
    enum RESAMPLE_STATUS_T status;

    if (resamp == NULL) {
        TRACE(1, "%s invalid param.", __func__);
        goto exit;
    }

    io.out_cyclic_start = NULL;
    io.out_cyclic_end = NULL;

    if (resamp->offset < resamp->iter_len) {
        io.in = buf;
        io.in_size = len;
        io.out = resamp->iter_buf + resamp->offset;
        io.out_size = resamp->iter_len - resamp->offset;

        status = audio_resample_ex_run_32bit((RESAMPLE_ID*)resamp->id, &io, &in_size, &out_size);
        if (status != RESAMPLE_STATUS_OUT_FULL && status != RESAMPLE_STATUS_IN_EMPTY &&
            status != RESAMPLE_STATUS_DONE) {
            TRACE(1, "resample run err:%d", status);
            goto exit;
        }

        buf += in_size;
        len -= in_size;
        resamp->offset += out_size;

        ASSERT(len == 0 || resamp->offset == resamp->iter_len,
            "%s: Bad resample offset: len=%d offset=%u iter_len=%u",
            __func__, len, resamp->offset, resamp->iter_len);

        if (resamp->offset == resamp->iter_len) {
            if (resamp->cb) {
                resamp->cb(resamp->ch, resamp->iter_buf, resamp->iter_len);
            }
        }
    }

    while (len) {
        io.in = buf;
        io.in_size = len;
        io.out = resamp->iter_buf;
        io.out_size = resamp->iter_len;

        status = audio_resample_ex_run_32bit((RESAMPLE_ID*)resamp->id, &io, &in_size, &out_size);
        if (status != RESAMPLE_STATUS_OUT_FULL && status != RESAMPLE_STATUS_IN_EMPTY &&
            status != RESAMPLE_STATUS_DONE) {
            TRACE(1, "resample run err:%d", status);
            goto exit;
        }

        ASSERT(in_size <= len, "%s: Bad resample in_size: in_size=%u len=%u", __func__, in_size, len);
        ASSERT(out_size <= resamp->iter_len, "%s: Bad resample out_size: out_size=%u iter_len=%u", __func__, out_size, resamp->iter_len);

        buf += in_size;
        len -= in_size;
        if (out_size == resamp->iter_len) {
            if (resamp->cb) {
                resamp->cb(resamp->ch, resamp->iter_buf, resamp->iter_len);
            }
        } else {
            resamp->offset = out_size;
            ASSERT(len == 0, "%s: Bad resample len: len=%u in_size=%u", __func__, len, in_size);
        }
    }
    return;

exit:
    if (resamp) {
        app_resample_reset(resamp);
    }

    memset(buf, 0x00, len);
    return;
}

void app_resample_any_tune(AUDIO_USB_RESAMPLE_T* resamp, float ratio)
{
    if (resamp) {
        float new_step = resamp->ratio_step + resamp->ratio_step * ratio;
        audio_resample_ex_set_ratio_step_32bit((RESAMPLE_ID*)resamp->id, new_step);
    }
}
#endif