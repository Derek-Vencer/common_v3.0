/***************************************************************************
 *
 * Copyright 2015-2021 BES.
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
#ifdef DSP_M55_TEST

#include "cmsis.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "hwtest.h"
#include "hal_key.h"
#include "hal_location.h"
#include "hal_codec.h"
#include "hal_sleep.h"
#include "hal_sysfreq.h"
#include "pmu.h"
#include "dsp_m55.h"
#include "string.h"
#ifdef DMA_AUDIO_APP
#include "audioflinger.h"
#include "stream_dma_rpc.h"
#include "dma_audio.h"
#endif

static const uint8_t handshake_msg[] = "Hi";
static bool handshake_done;

static uint32_t pwr_key_click_cnt;

#ifdef SLEEP_TEST
#ifndef NO_TIMER
static void timer_handler(void *param)
{
    static uint32_t wakeup_cnt = 0;
    dsp_m55_send((uint8_t *)&wakeup_cnt, sizeof(wakeup_cnt));
    wakeup_cnt++;
}
#endif

static void dsp_m55_sleep_test(void)
{
#ifndef NO_TIMER
    sleep_test_register_timer_callback(timer_handler);
#endif

    // Jump to next test case and the last one is SLEEP_TEST
}
#endif

#ifdef MCU2DSP_M55_MSG_TEST
#define MAX_SEND_CNT                        10
static uint32_t send_word = 0xabcdef12;
static uint32_t send_cnt;
static uint8_t send_buf[MAX_SEND_CNT][8];

static unsigned int msg_test_rx_handler(const void *data, unsigned int len)
{
    TR_INFO(0, "RX=%u", len);
    HWTEST_DUMP8("%02X ", data, len);

    if (send_cnt < MAX_SEND_CNT) {
        int ret;

        len = sizeof(send_word);
        if (len > sizeof(send_buf[0])) {
            len = sizeof(send_buf[0]);
        }
        memcpy(send_buf[send_cnt], &send_word, len);
        ret = dsp_m55_send(send_buf[send_cnt], len);
        ASSERT(ret == 0, "dsp_m55_send failed: %d", ret);

        send_word += (1 << (8 * (send_cnt % 4)));
        send_cnt++;
    }

    return len;
}

static void msg_test_tx_handler(const void *data, unsigned int len)
{
    TR_INFO(0, "TX=%u", len);
    HWTEST_DUMP8("%02X ", data, len);
}
#endif

#ifdef DMA_AUDIO_APP

#define PLAYBACK1_VERBOSE 1

#define NO_CPU_LOCK
#define NO_PLAY_KEY
//#define NO_DMA_AUD_OFF_KEY

/* PLAYBACK1 STREAM CONFIG */
#define PLAY1_AUD_STREAM_ID (DAUD_NORM_STREAM_ID)
#define PLAY1_SAMP_RATE     (AUD_SAMPRATE_48000)
#define PLAY1_SAMP_BITS     (AUD_BITS_16)
#define PLAY1_SAMP_SIZE     (2)
#define PLAY1_CHAN_NUM      (AUD_CHANNEL_NUM_2)
#define PLAY1_CHAN_MAP      (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)
#define PLAY1_BUFF_FRM_NUM  (20)
#define PLAY1_BUFF_SIZE     (PLAY1_SAMP_RATE/1000 * PLAY1_SAMP_SIZE * PLAY1_CHAN_NUM * PLAY1_BUFF_FRM_NUM)

#ifndef ALIGN4
#define ALIGN4 __attribute__((aligned(4)))
#endif

POSSIBLY_UNUSED static uint8_t ALIGN4 play1_buff[PLAY1_BUFF_SIZE];

static bool playback1_state = false;
static bool dma_aud_state = false;
static uint8_t dma_aud_cmd;

enum DMA_AUDIO_CMD_T {
    DMA_AUDIO_CMD_NONE,
    DMA_AUDIO_CMD_ON,        // dma audio on
    DMA_AUDIO_CMD_OFF,       // dma audio off
    DMA_AUDIO_CMD_LOCK_CPU,  // disable MAIN CPU irq for a while
    DMA_AUDIO_CMD_PLAY1_ON,  // play stream on
    DMA_AUDIO_CMD_PLAY1_OFF, // play stream off
};

enum AUDIO_KEY_PHASE_T {
    AUDIO_KEY_PHASE_STREAM_OFF,
    AUDIO_KEY_PHASE_STREAM_ON,
#ifndef NO_CPU_LOCK
    AUDIO_KEY_PHASE_LOCK_CPU,
#endif
#ifndef NO_PLAY_KEY
    AUDIO_KEY_PHASE_PLAY1_ON,
    AUDIO_KEY_PHASE_PLAY1_OFF,
#endif
    AUDIO_KEY_PHASE_QTY,
};

static uint32_t key_phase = 0;

static void cpu_whoami(void)
{
    uint32_t cpuid = get_cpu_id();
    HWTEST_TRACE(1, "CPUID:%d, CORE:%s", cpuid, cpuid==0?"M55":"M33");
}

void dma_audio_app_init(void)
{
    HWTEST_TRACE(1, "%s:", __func__);

    af_open();

    cpu_whoami();

    dma_aud_state = false;
    dma_aud_cmd = DMA_AUDIO_CMD_NONE;
}

void dma_audio_lock_cpu(void)
{
#define IRQ_DIS_TIME 2000
    uint32_t lock = 0;

    HWTEST_TRACE(1, "Main CPU info:");
    cpu_whoami();

    HWTEST_TRACE(1, "DISABLE CPU IRQ %d seconds", IRQ_DIS_TIME);
    lock = int_lock();
    osDelay(IRQ_DIS_TIME);

    int_unlock(lock);
    HWTEST_TRACE(1, "ENABLE CPU IRQ");
    osDelay(1000);

    HWTEST_TRACE(1, "CPU IRQ TEST DONE");
}

static int play_data_init = 0;
POSSIBLY_UNUSED static uint32_t playback1_data_handler(uint8_t *buf, uint32_t len)
{
    static volatile uint32_t play_tick = 0;
    static int16_t play_data[] = {
        1000, 4000, 8000, 12000, 16000, 20000, 24000, 28000, 32200, 32400, 32600, 32700,
        32700, 32600, 32400, 32200, 28000, 24000, 20000, 16000, 12000, 8000, 4000, 1000,
        -1000, -4000, -8000, -12000, -16000, -20000, -24000, -28000, -32200, -32400, -32600, -32700,
        -32700, -32600, -32400, -32200, -28000, -24000, -20000, -16000, -12000, -8000, -4000, -1000,
    };

#if defined(PLAYBACK1_VERBOSE) && (PLAYBACK1_VERBOSE > 0)
    uint32_t cur_time = TICKS_TO_MS(hal_sys_timer_get());
#endif

    if (play_data_init < 2) {
        int16_t *pcm = (int16_t *)buf;
        uint32_t pcm_len = len / 2, i, j = 0;
        for (i = 0; i < pcm_len; i += 2) {
            if (j % ARRAY_SIZE(play_data) == 0) {
                j = 0;
            }
            pcm[i+0] = play_data[j]>>8;
            pcm[i+1] = play_data[j]>>8;
            j++;
        }
        play_data_init++;
    }

    play_tick++;
    if (play_tick >= 2000/(PLAY1_BUFF_FRM_NUM/2)) {
        play_tick = 0;
#if defined(PLAYBACK1_VERBOSE) && (PLAYBACK1_VERBOSE > 0)
        HWTEST_TRACE(5,"[%8u] PLAY1: buf=%x, len=%4u", cur_time, (int)buf, len);
#endif
        cpu_whoami();
    }
    return 0;
}

int playback1_stream_on(bool on)
{
    int ret = 0;

    if (playback1_state == on) {
        return 0;
    }
    HWTEST_TRACE(1, "====> %s", __func__);
    if (on) {
        struct AF_STREAM_CONFIG_T stream_cfg;

        memset(&stream_cfg, 0, sizeof(stream_cfg));

#ifdef DMA_AUDIO_NORM_PLAY_ON
         /* get stream cfg from rpc server */
        struct DAUD_STREAM_CONFIG_T daud_cfg;

        dma_rpcif_setup_send_cmd_handler(dsp_m55_send);
        dma_rpcif_open(PLAY1_AUD_STREAM_ID, AUD_STREAM_PLAYBACK);
        dma_rpc_get_stream_cfg(PLAY1_AUD_STREAM_ID, AUD_STREAM_PLAYBACK, (void *)&daud_cfg);

        stream_cfg.sample_rate = daud_cfg.sample_rate;
        stream_cfg.bits        = daud_cfg.bits;
        stream_cfg.channel_num = daud_cfg.channel_num;
        stream_cfg.channel_map = daud_cfg.channel_map;
        stream_cfg.device      = daud_cfg.device;
        stream_cfg.vol         = daud_cfg.vol;
        stream_cfg.handler     = daud_cfg.handler;
        stream_cfg.io_path     = daud_cfg.io_path;
        stream_cfg.data_ptr    = daud_cfg.data_ptr;
        stream_cfg.data_size   = daud_cfg.data_size;

        dma_rpcif_close(PLAY1_AUD_STREAM_ID, AUD_STREAM_PLAYBACK);
#else
         /* use local stream cfg */
        stream_cfg.sample_rate = PLAY1_SAMP_RATE;
        stream_cfg.bits        = PLAY1_SAMP_BITS;
        stream_cfg.channel_num = PLAY1_CHAN_NUM;
        stream_cfg.channel_map = PLAY1_CHAN_MAP;
        stream_cfg.device      = AUD_STREAM_USE_INT_CODEC;
        stream_cfg.vol         = 17;
        stream_cfg.handler     = playback1_data_handler;
        stream_cfg.io_path     = AUD_OUTPUT_PATH_SPEAKER;
        stream_cfg.data_ptr    = play1_buff;
        stream_cfg.data_size   = sizeof(play1_buff);
#endif
        HWTEST_TRACE(1, "playback1: buf=%x, size=%d", (int)stream_cfg.data_ptr, (uint32_t)stream_cfg.data_size);
        HWTEST_TRACE(1, "playback1: sample_rate=%d, bits=%d", stream_cfg.sample_rate, stream_cfg.bits);

        play_data_init = 0;

        ret = af_stream_open(PLAY1_AUD_STREAM_ID, AUD_STREAM_PLAYBACK, &stream_cfg);
        ASSERT(ret == 0, "af_stream_open playback failed: %d", ret);

        ret = af_stream_start(PLAY1_AUD_STREAM_ID, AUD_STREAM_PLAYBACK);
        ASSERT(ret == 0, "af_stream_start playback failed: %d", ret);
        HWTEST_TRACE(1, "====> playback1 stream started");
    } else {
        ret = af_stream_stop(PLAY1_AUD_STREAM_ID, AUD_STREAM_PLAYBACK);
        ASSERT(ret == 0, "af_stream_stop playback failed: %d", ret);

        ret = af_stream_close(PLAY1_AUD_STREAM_ID, AUD_STREAM_PLAYBACK);
        ASSERT(ret == 0, "af_stream_close playback failed: %d", ret);
        HWTEST_TRACE(1, "====> playback1 stream stopped");
    }
    playback1_state = on;
    return ret;
}

#ifdef CODEC_SW_SYNC
void hal_codec_sw_sync_low_latency_enable(uint32_t intv_us);
#endif

void dma_audio_on(bool on)
{
    if (on == dma_aud_state) {
        return;
    }
    HWTEST_TRACE(1, "%s: on=%d", __func__, on);

    if (on) {
        int ret = 0;
        struct AF_STREAM_CONFIG_T stream_cfg;

        hal_sysfreq_req(HAL_SYSFREQ_USER_APP_2, HAL_CMU_FREQ_52M);

#ifdef CODEC_SW_SYNC
        hal_codec_sw_sync_play3_open();
        hal_codec_sw_sync_cap2_open();
#endif
        // config playback stream
        memset(&stream_cfg, 0, sizeof(stream_cfg));
#ifdef DMA_RPC_CLI
        struct DAUD_STREAM_CONFIG_T daud_cfg;

        dma_rpcif_setup_send_cmd_handler(dsp_m55_send);
        dma_rpcif_open(DAUD_STREAM_ID, AUD_STREAM_PLAYBACK);
        dma_rpc_get_stream_cfg(DAUD_STREAM_ID, AUD_STREAM_PLAYBACK, (void *)&daud_cfg);

        stream_cfg.sample_rate = daud_cfg.sample_rate;
        stream_cfg.bits        = daud_cfg.bits;
        stream_cfg.channel_num = daud_cfg.channel_num;
        stream_cfg.channel_map = daud_cfg.channel_map;
        stream_cfg.device      = daud_cfg.device;
        stream_cfg.io_path     = daud_cfg.io_path;
        stream_cfg.vol         = daud_cfg.vol;
        stream_cfg.handler     = daud_cfg.handler;
        stream_cfg.data_ptr    = daud_cfg.data_ptr;
        stream_cfg.data_size   = daud_cfg.data_size;
        stream_cfg.sync_start  = true;
#else
        ASSERT(false, "%s: %d, DMA_RPC_CLI is not define", __func__, __LINE__);
#endif
        HWTEST_TRACE(1, "[dma_audio]: playback: rate=%d,bits=%d data_ptr=%x, data_size=%d",
            stream_cfg.sample_rate, stream_cfg.bits,
            (uint32_t)stream_cfg.data_ptr, stream_cfg.data_size);

        ret = af_stream_open(DAUD_STREAM_ID, AUD_STREAM_PLAYBACK, &stream_cfg);
        ASSERT(ret == 0, "af_stream_open playback failed: %d", ret);

        // config capture stream
        memset(&stream_cfg, 0, sizeof(stream_cfg));
#ifdef DMA_RPC_CLI
        dma_rpcif_open(DAUD_STREAM_ID, AUD_STREAM_CAPTURE);
        dma_rpc_get_stream_cfg(DAUD_STREAM_ID, AUD_STREAM_CAPTURE, (void *)&daud_cfg);

        stream_cfg.sample_rate = daud_cfg.sample_rate;
        stream_cfg.bits        = daud_cfg.bits;
        stream_cfg.channel_num = daud_cfg.channel_num;
        stream_cfg.channel_map = daud_cfg.channel_map;
        stream_cfg.device      = daud_cfg.device;
        stream_cfg.io_path     = daud_cfg.io_path;
        stream_cfg.vol         = daud_cfg.vol;
        stream_cfg.handler     = daud_cfg.handler;
        stream_cfg.data_ptr    = daud_cfg.data_ptr;
        stream_cfg.data_size   = daud_cfg.data_size;
        stream_cfg.sync_start  = true;
#else
        ASSERT(false, "%s: %d, DMA_RPC_CLI is not define", __func__, __LINE__);
#endif

        HWTEST_TRACE(1, "[dma_audio]: capture: rate=%d,bits=%d data_ptr=%x, data_size=%d",
            stream_cfg.sample_rate, stream_cfg.bits,
            (uint32_t)stream_cfg.data_ptr, stream_cfg.data_size);

        ret = af_stream_open(DAUD_STREAM_ID, AUD_STREAM_CAPTURE, &stream_cfg);
        ASSERT(ret == 0, "af_stream_open capture failed: %d", ret);
        ret = af_stream_start(DAUD_STREAM_ID, AUD_STREAM_PLAYBACK);
        ASSERT(ret == 0, "af_stream_start playback failed: %d", ret);

        ret = af_stream_start(DAUD_STREAM_ID, AUD_STREAM_CAPTURE);
        ASSERT(ret == 0, "af_stream_start capture failed: %d", ret);
        HWTEST_TRACE(1, "====> dma audio on");

#ifdef CODEC_SW_SYNC
        hal_codec_sw_sync_low_latency_enable(120);
#endif
    } else {
        af_stream_stop(DAUD_STREAM_ID, AUD_STREAM_CAPTURE);
        af_stream_stop(DAUD_STREAM_ID, AUD_STREAM_PLAYBACK);
        af_stream_close(DAUD_STREAM_ID, AUD_STREAM_CAPTURE);
        af_stream_close(DAUD_STREAM_ID, AUD_STREAM_PLAYBACK);
#ifdef DMA_RPC_CLI
        dma_rpcif_close(DAUD_STREAM_ID, AUD_STREAM_PLAYBACK);
        dma_rpcif_close(DAUD_STREAM_ID, AUD_STREAM_CAPTURE);
#endif
#ifdef CODEC_SW_SYNC
        hal_codec_sw_sync_play3_close();
        hal_codec_sw_sync_cap2_close();
#endif
        hal_sysfreq_req(HAL_SYSFREQ_USER_APP_2, HAL_CMU_FREQ_32K);
        HWTEST_TRACE(1, "====> dma audio off");
    }
    dma_aud_state = on;
}

static unsigned int dma_audio_rx_handler(const void *data, unsigned int len)
{
#ifdef DMA_RPC_CLI
    return dma_rpcif_rx_handler(data, len);
#else
    return len;
#endif
}

static void dma_audio_tx_handler(const void *data, unsigned int len)
{
#ifdef DMA_RPC_CLI
    dma_rpcif_tx_handler(data, len);
#endif
}

static int dma_audio_key_event_handler(uint32_t key_code, uint8_t key_event)
{
    HWTEST_TRACE(1, "[dma_audio_key]: key_code=%x, key_event=%x", key_code, key_event);

    if (key_event == HAL_KEY_EVENT_CLICK) {
        if (key_code == HAL_KEY_CODE_PWR) {
            key_phase++;
            if (key_phase >= AUDIO_KEY_PHASE_QTY) {
                key_phase = AUDIO_KEY_PHASE_STREAM_OFF;
#ifdef NO_DMA_AUD_OFF_KEY
                key_phase++;
#endif
            }
            switch (key_phase) {
            case AUDIO_KEY_PHASE_STREAM_OFF:
                dma_aud_cmd = DMA_AUDIO_CMD_OFF;
                break;
            case AUDIO_KEY_PHASE_STREAM_ON:
                dma_aud_cmd = DMA_AUDIO_CMD_ON;
                break;
#ifndef NO_CPU_LOCK
            case AUDIO_KEY_PHASE_LOCK_CPU:
                dma_aud_cmd = DMA_AUDIO_CMD_LOCK_CPU;
                break;
#endif
#ifndef NO_PLAY_KEY
            case AUDIO_KEY_PHASE_PLAY1_ON:
                dma_aud_cmd = DMA_AUDIO_CMD_PLAY1_ON;
                break;
            case AUDIO_KEY_PHASE_PLAY1_OFF:
                dma_aud_cmd = DMA_AUDIO_CMD_PLAY1_OFF;
                break;
#endif
            default:
                key_phase = 0;
                break;
            }
        }
    }

    return 0;
}

static void dma_audio_cmd_handler(void)
{
    uint8_t cmd = dma_aud_cmd;

    if (cmd != DMA_AUDIO_CMD_NONE) {
        switch (cmd) {
        case DMA_AUDIO_CMD_ON:
            dma_audio_on(true);
            break;
        case DMA_AUDIO_CMD_OFF:
            dma_audio_on(false);
            break;
        case DMA_AUDIO_CMD_LOCK_CPU:
            dma_audio_lock_cpu();
            break;
        case DMA_AUDIO_CMD_PLAY1_ON:
            playback1_stream_on(true);
            break;
        case DMA_AUDIO_CMD_PLAY1_OFF:
            playback1_stream_on(false);
            break;
        default:
            break;
        }
        dma_aud_cmd = DMA_AUDIO_CMD_NONE;
    }
}

static void dma_audio_app_thread(void)
{
    while(1) {
        dma_audio_cmd_handler();
#ifndef RTOS
        af_thread();
#else
        osDelay(20);
#endif
    }
}
#endif /* DMA_AUDIO_APP */

static unsigned int rx_handler(const void *data, unsigned int len)
{
    if (!handshake_done) {
        if (data && len == (sizeof(handshake_msg) - 1) &&
                memcmp(data, handshake_msg, len) == 0) {
            TR_INFO(0, "dsp_m55 alive!");
            handshake_done = true;

            return len;
        }
    }
#ifdef DMA_AUDIO_APP
    return dma_audio_rx_handler(data, len);
#endif
#ifdef MCU2DSP_M55_MSG_TEST
    return msg_test_rx_handler(data, len);
#endif

    return len;
}

static void tx_handler(const void *data, unsigned int len)
{
#ifdef DMA_AUDIO_APP
    dma_audio_tx_handler(data, len);
#endif
#ifdef MCU2DSP_M55_MSG_TEST
    msg_test_tx_handler(data, len);
#endif
}

static int key_event_process(uint32_t key_code, uint8_t key_event)
{
    HWTEST_TRACE(3,"%s: code=0x%X, event=%u", __FUNCTION__, key_code, key_event);

    if (key_event == HAL_KEY_EVENT_CLICK) {
        if (key_code == HAL_KEY_CODE_PWR) {
            pwr_key_click_cnt++;
        }
    }
#ifdef DMA_AUDIO_APP
    if (dma_audio_key_event_handler(key_code, key_event) == 0) {
        return 0;
    }
#endif

    return 0;
}

void dsp_m55_test(void)
{
    int ret;

    HWTEST_TRACE(0, "%s", __func__);

    hal_key_open(false, key_event_process);

#ifdef MAIN_RAM_USE_SYS_RAM
    ret = dsp_m55_open_in_sys_ram(rx_handler, tx_handler);
#else
    ret = dsp_m55_open(rx_handler, tx_handler);
#endif
    ASSERT(ret == 0, "dsp_m55_open failed: %d", ret);

#ifdef DMA_AUDIO_APP
    dma_audio_app_init();
    dma_audio_app_thread();
#endif

#ifdef SLEEP_TEST
    dsp_m55_sleep_test();
#endif
}

#endif
