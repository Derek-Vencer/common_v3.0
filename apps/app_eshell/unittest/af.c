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
#if defined(UTILS_ESHELL_EN) && defined(__SYS_AS_MAIN__)
#if !defined(BT_CLOSE) && !defined(BTH_SUBSYS_ONLY)
#include "stdlib.h"
#include "string.h"
#include "cmsis_os.h"
#include "hal_trace.h"
#include "eshell.h"
#include "hal_timer.h"
#include "audioflinger.h"

#ifdef CHIP_BEST1700
#define TEST_CH_MAP                    AUD_CHANNEL_MAP_DIGMIC_CH0
#else
#define TEST_CH_MAP                    AUD_CHANNEL_MAP_CH0
#endif

#define ALIGNED4                        ALIGNED(4)
static uint8_t ALIGNED4 playback_buf[48*4*2];
static uint8_t ALIGNED4 capture_buf[48*4*2];

static const int16_t tone_1k[] = {
    #include "../../../apps/factory/1k_2ch_48k_16bit.txt"
};
static uint32_t tone_offset;
static uint32_t cap_rpos;
static uint32_t cap_wpos;

static uint32_t tone_data_playback(uint8_t *buf, uint32_t len)
{
    const uint8_t *src = (uint8_t *)tone_1k;
    const uint32_t src_len = sizeof(tone_1k);
    uint32_t remain;

    while (len) {
        if (tone_offset >= src_len) {
            tone_offset = 0;
        }
        remain = src_len - tone_offset;
        if (remain > len) {
            remain = len;
        }
        memcpy(buf, src + tone_offset, remain);
        len -= remain;
        tone_offset += remain;
    }

    return 0;
}

static void dac_1k_test(struct AF_STREAM_CONFIG_T stream_cfg)
{
    int ret = 0;

    stream_cfg.bits = AUD_BITS_16;
    stream_cfg.channel_num = AUD_CHANNEL_NUM_2;
    stream_cfg.channel_map = AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1;
    stream_cfg.sample_rate = 48000;
    stream_cfg.vol = TGT_VOLUME_LEVEL_MAX;
    stream_cfg.handler = tone_data_playback;
    stream_cfg.io_path = AUD_IO_PATH_NULL;
    stream_cfg.data_ptr = playback_buf;
    stream_cfg.data_size = sizeof(playback_buf);

    ret = af_stream_open(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK, &stream_cfg);
    if (ret)
        eshell_putstring("af_stream_open playback failed: %d", ret);

    ret = af_stream_start(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
    if (ret)
        eshell_putstring("af_stream_start playback failed: %d", ret);
}

static uint32_t adda_data_capture(uint8_t *buf, uint32_t len)
{
    POSSIBLY_UNUSED uint32_t cur_time;

    cur_time = hal_sys_timer_get();

    cap_wpos += len;
    if (cap_wpos >= sizeof(capture_buf)) {
        cap_wpos = 0;
    }

    eshell_putstring("[%X] capture : len=%4u wpos=%4u rpos=%4u\r\n", cur_time, len, cap_wpos, cap_rpos);

    return 0;
}

static uint32_t adda_data_playback(uint8_t *buf, uint32_t len)
{
    POSSIBLY_UNUSED uint32_t cur_time;
    int16_t *src;
    int16_t *dst;
    int16_t *src_end;
    int16_t *dst_end;
    uint32_t avail;

    cur_time = hal_sys_timer_get();

    if (cap_wpos >= cap_rpos) {
        avail = cap_wpos - cap_rpos;
    } else {
        avail = sizeof(capture_buf) + cap_wpos - cap_rpos;
    }

    if (avail >= len) {
        src = (int16_t *)(capture_buf + cap_rpos);
        src_end = (int16_t *)(capture_buf + sizeof(capture_buf));
        dst = (int16_t *)buf;
        dst_end = (int16_t *)(buf + len);

        while (dst < dst_end) {
            *dst++ = *src++;
            if (src == src_end) {
                src = (int16_t *)capture_buf;
            }
        }

        cap_rpos = (uint32_t)src - (uint32_t)capture_buf;
    } else {
        memset(buf, 0, len);
    }

    eshell_putstring("[%X] playback: len=%4u wpos=%4u rpos=%4u avail=%4u\r\n", cur_time, len, cap_wpos, cap_rpos, avail);

    return 0;
}

static void adda_test(struct AF_STREAM_CONFIG_T stream_cfg)
{
    int ret = 0;
    enum AUD_CHANNEL_MAP_T chan_map_cap = 0;

    if (stream_cfg.channel_num == AUD_CHANNEL_NUM_2)
        stream_cfg.channel_map = AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1;
    else
        stream_cfg.channel_map = AUD_CHANNEL_MAP_CH0;
    stream_cfg.vol = TGT_VOLUME_LEVEL_MAX;
    stream_cfg.handler = adda_data_playback;
    stream_cfg.io_path = AUD_IO_PATH_NULL;
    stream_cfg.data_ptr = playback_buf;
    stream_cfg.data_size = sizeof(playback_buf);

    ret = af_stream_open(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK, &stream_cfg);
    if (ret)
        eshell_putstring("af_stream_open playback failed: %d", ret);

    for (int i = 0; i < stream_cfg.channel_num; i++) {
        chan_map_cap |= TEST_CH_MAP << i;
    }
    stream_cfg.channel_map = chan_map_cap;
    stream_cfg.handler = adda_data_capture;
    stream_cfg.io_path = AUD_INPUT_PATH_MAINMIC;
    stream_cfg.data_ptr = capture_buf;
    stream_cfg.data_size = sizeof(capture_buf);

    ret = af_stream_open(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE, &stream_cfg);
    if (ret)
        eshell_putstring("af_stream_open capture failed: %d", ret);

    ret = af_stream_start(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
    if (ret)
        eshell_putstring("af_stream_start playback failed: %d", ret);

    ret = af_stream_start(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
    if (ret)
        eshell_putstring("af_stream_start capture failed: %d", ret);

}

static void unitest_af(int argc, char *argv[])
{
    if (argc < 2) {
        eshell_putstring("cmd err\r\n");
        eshell_putstring("cmd format: utest_af item(dac1k/adda) device(1/2/3) bit(16/24/32) samprate(16000/44100/48000) ch_num(1/2)\r\n");
        return;
    }

    struct AF_STREAM_CONFIG_T stream_cfg;

    memset(&stream_cfg, 0, sizeof(stream_cfg));

    af_open();

    if (argc >= 3) {
        if (atoi(argv[2]) == 1)
            stream_cfg.device = AUD_STREAM_USE_INT_CODEC;
        else if (atoi(argv[2]) == 2)
            stream_cfg.device = AUD_STREAM_USE_INT_CODEC2;
        else if (atoi(argv[2]) == 3)
            stream_cfg.device = AUD_STREAM_USE_INT_CODEC2;
        else
            stream_cfg.device = AUD_STREAM_USE_INT_CODEC;
    } else {
        stream_cfg.device = AUD_STREAM_USE_INT_CODEC;
    }

    if (argc >= 4) {
        if (atoi(argv[3]) == 16)
            stream_cfg.bits = AUD_BITS_16;
        else if (atoi(argv[3]) == 24)
            stream_cfg.bits = AUD_BITS_24;
        else if (atoi(argv[3]) == 32)
            stream_cfg.bits = AUD_BITS_32;
        else
            stream_cfg.bits = AUD_BITS_16;
    } else {
        stream_cfg.bits = AUD_BITS_16;
    }

    if (argc >= 5) {
        if (atoi(argv[4]) == 16000)
            stream_cfg.sample_rate = 16000;
        else if (atoi(argv[4]) == 44100)
            stream_cfg.sample_rate = 44100;
        else if (atoi(argv[4]) == 48000)
            stream_cfg.sample_rate = 48000;
        else
            stream_cfg.sample_rate = 16000;
    } else {
        stream_cfg.sample_rate = 16000;
    }

    if (argc >= 6) {
        if (atoi(argv[5]) == 1)
            stream_cfg.channel_num = 1;
        else if (atoi(argv[5]) == 2)
            stream_cfg.channel_num = 2;
        else
            stream_cfg.channel_num = 1;
    } else {
        stream_cfg.channel_num = 1;
    }

    if (strncmp(argv[1], "dac1k", 5) == 0) {
        dac_1k_test(stream_cfg);
    }

    if (strncmp(argv[1], "adda", 4) == 0) {
        adda_test(stream_cfg);
    }

}

ESHELL_DEF_COMMAND(
    ESHELL_CMD_GRP_UNIT_TEST, "utest_af",
    "utest_af item(dac1k/adda) device(1/2/3) bit(16/24/32) samprate(16000/44100/48000) ch_num(1/2)",
    unitest_af);

#endif
#endif // UTILS_ESHELL_EN
