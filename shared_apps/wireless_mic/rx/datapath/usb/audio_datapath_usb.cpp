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
#include "cmsis_os.h"
#include "hal_trace.h"
#include "hal_aud.h"
#include "hal_usb.h"
#include "hal_timer.h"
#include "hal_gpio.h"

#include "audioflinger.h"
#include "usb_audio_app.h"
#include "usbaudio_thread.h"

#include "app_wireless_mic.h"
#include "audio_datapath_manager.h"
#include "usb_audio_app.h"
#include "usb_audio_queue.h"
// #include "audio_datapath_process.h"
#include "audio_datapath_usb.h"
// #include "audio_datapath_adc.h"
#include "algo_com.h"
#include "rx_algo.h"
#ifdef LTC_ENABLED
#include "ltc_adapter.h"
#endif
#ifdef USB_AUDIO_ANY_RESAMPLE
#include "usb_audio_resample.h"
#ifdef AUDIO_USB_RESAMPLE_ON_CP
#include "audio_cp_subsys_adapter.h"
#endif
#ifdef AUDIO_USB_RESAMPLE_ON_BTH
#include "audio_bt_host_adapter.h"
#endif
#endif

#define USB_AUDIO_MANAGER_DEBOUNCE_TIME_IN_MS                 (150)
// #define AUDIO_USB_CAPTURE_STREAM_INPUT_BUFF_SIZE              (48 * 4 * AUDIO_OUTPUT_DATA_COOKING_INTERVAL_MS * WIFI_SESSION_NUM)
// #define AUDIO_USB_CAPTURE_STREAM_PROCESS_SIZE                 (AUDIO_USB_CAPTURE_STREAM_INPUT_BUFF_SIZE * 5)
// #define AUDIO_USB_CAPTURE_STREAM_FACTORY_INPUT_SIZE           (AUDIO_CODEC_CAPTURE_STREAM_DMA_BUFF_SIZE / 2)
#define AUDIO_USB_CAPTURE_STREAM_INPUT_BUFF_SIZE              (48 * 2 * BLE_AUDIO_FRAME_US / 1000 * 4) //48k 32bit 2ch 7.5ms
#define AUDIO_USB_CAPTURE_STREAM_OUTPUT_BUFF_SIZE             (48 * 2 * 5 * 3) //48k 24bit 2ch 5ms
#define AUDIO_USB_CAPTURE_STREAM_PROCESS_SIZE                 (AUDIO_USB_CAPTURE_STREAM_INPUT_BUFF_SIZE * 1)
#define AUDIO_USB_CAPTURE_STREAM_LTC_BUFF_SIZE                (48 * 4 * AUDIO_OUTPUT_DATA_COOKING_INTERVAL_MS * 5)

#if defined(AUDIO_USB_RESAMPLE_ON_CP) || defined(AUDIO_USB_RESAMPLE_ON_BTH)
#define AUDIO_USB_CAPTURE_STREAM_PROCESS_BUFF_NUM             (2)
#else
#define AUDIO_USB_CAPTURE_STREAM_PROCESS_BUFF_NUM             (1)
#endif

static bool usb_playback_stream_is_running = false;
static bool usb_capture_stream_is_running = false;
static bool usb_capture_stream_started = false;
static bool usb_capture_stream_data_is_ready = false;
static uint8_t usb_capture_stream_process_buff_idx = 0;
static uint8_t usb_capture_stream_channel_num = 2;
static uint8_t usb_capture_stream_process_buff[AUDIO_USB_CAPTURE_STREAM_PROCESS_BUFF_NUM][AUDIO_USB_CAPTURE_STREAM_PROCESS_SIZE] = {0};
uint8_t usb_capture_stream_ltc_buff[AUDIO_USB_CAPTURE_STREAM_LTC_BUFF_SIZE] = {0};
uint16_t usb_capture_stream_ltc_len = 0;
bool audio_datapath_usb_ltc_output = false;

#if defined(AUDIO_USB_ALGO_ON_CP) || defined(AUDIO_USB_ALGO_ON_BTH)
static AUDIO_CP_ALGO_T audio_datapath_usb_algo;
#endif
#ifdef USB_AUDIO_ANY_RESAMPLE
extern AUDIO_USB_STREAM_DATA_T usb_resample_send;
#endif

static uint64_t usb_capture_stream_last_tsf = 0;

static osTimerId usb_audio_manager_debounce_timer = NULL;
static void usb_audio_manager_handler(bool curr_cap, bool curr_play, bool new_cap, bool new_play);
static void usb_audio_manager_debounce_timer_handler(void* param);
static const osTimerAttr_t usb_audio_manager_debounce_timer_attr = {
    .name = "usb_debounce_timer",
    .attr_bits = 0U,
    .cb_mem = NULL,
    .cb_size = 0U,
};

static osThreadId audio_usb_tid = NULL;
static void audio_usb_process_thread(void* argument);
static const osThreadAttr_t audio_usb_process_thread_attr = {
    .name       = "audio_usb_pro_thread",
    .stack_size = (4 * 1024),
    .priority   = osPriorityAboveNormal7,
};

extern "C" void btusb_usb_pre_open(void);

bool audio_datapath_usb_output_normal_mode(void)
{
#ifdef LTC_ENABLED
    if (ltc_input_mode_get() == LTC_INPUT_MODE_FACTORY) {
        return false;
    }
#endif
    return true;
}

uint64_t audio_datapath_usb_ref_tsf_get(void)
{
    // return hal_wifitsf_usb_time_get();
    return 0;
}

uint16_t audio_datapath_usb_irq_interval_get(void)
{
    return USB_AUDIO_FRAME_MS * 1000;
}

void audio_data_usb_capture_stream_data_input(uint8_t* buff, uint32_t len)
{
    uint16_t output_len = 0;
    int status = 0;

    // if (audio_datapath_usb_output_normal_mode() && len == AUDIO_USB_CAPTURE_STREAM_INPUT_BUFF_SIZE) {
        if (0) {
        }
#ifdef USB_AUDIO_ANY_RESAMPLE
        else if (audio_usb_output_stream_resample_is_on()) {
            status = audio_usb_data_enqueue(buff, len);
            audio_usb_output_stream_process_shot();
        }
#endif
        else {
#ifdef RX_ALGO_PROCESS
            rx_algo_process(AUDIO_OUTPUT_PATH_BIT_USB, usb_capture_stream_process_buff[0], buff, len);
#endif
            output_len = len / MAX_INPUT_CH_NUM * usb_capture_stream_channel_num;
            output_len = audio_copy_32bit_to_24bit(usb_capture_stream_process_buff[0], usb_capture_stream_process_buff[0], output_len);
            status = audio_usb_data_enqueue(usb_capture_stream_process_buff[0], output_len);
        }
    // }
    // else if (len == AUDIO_USB_CAPTURE_STREAM_FACTORY_INPUT_SIZE) {
    //     output_len = audio_copy_32bit_to_24bit(buff, buff, len);
    //     status = audio_usb_data_enqueue(buff, output_len);
    //     // audio_usb_output_stream_process_shot();
    // }
    // else {
    //     LOG_ERR(1, "usb recv invalid data len:%d", len);
    // }

    if (usb_capture_stream_data_is_ready == false && audio_usb_data_queue_get_length() >= 2 * AUDIO_USB_CAPTURE_STREAM_OUTPUT_BUFF_SIZE) {
        usb_capture_stream_data_is_ready = true;
        LOG_MSG(1, "usb capture stream data is ready!");
     }

    if (status) {
        audio_datapath_usb_capture_stream_check_dm_status();
    }
}

bool audio_data_usb_capture_stream_running(void)
{
    return usb_capture_stream_is_running;
}

static void usb_audio_manager_debounce_timer_handler(void* param)
{
    static bool curr_cap = false, curr_play = false;
    bool new_cap = usb_capture_stream_is_running;
    bool new_play = usb_playback_stream_is_running;

    LOG_MSG(2, "curr audio status: cap(%d) play(%d)", curr_cap, curr_play);
    LOG_MSG(2, "new audio status: cap(%d) play(%d)", new_cap, new_play);

    usb_audio_manager_handler(curr_cap, curr_play, new_cap, new_play);

    curr_cap = new_cap;
    curr_play = new_play;
}

static void usb_audio_manager_debounce_timer_start(void)
{
    osTimerStop(usb_audio_manager_debounce_timer);
    osTimerStart(usb_audio_manager_debounce_timer, USB_AUDIO_MANAGER_DEBOUNCE_TIME_IN_MS);
}

static void usb_audio_manager_handler(bool curr_cap, bool curr_play, bool new_cap, bool new_play)
{
    if (0) {
    } else if (curr_cap == false && new_cap == true) {
        audio_usb_output_stream_start_shot();
    } else if (curr_cap == true && new_cap == false) {
        audio_usb_output_stream_stop_shot();
    }
}

static void audio_usb_playback_stream_start(void)
{
    LOG_MSG(1, "%s", __func__);

    if (!usb_playback_stream_is_running) {
        usb_playback_stream_is_running = true;

        usb_audio_manager_debounce_timer_start();
    }
}

static void audio_usb_playback_stream_stop(void)
{
    LOG_MSG(1, "%s", __func__);

    if (usb_playback_stream_is_running) {
        usb_playback_stream_is_running = false;

        usb_audio_manager_debounce_timer_start();
    }
}

static void audio_usb_capture_stream_start(void)
{
    LOG_MSG(1, "%s", __func__);

    if (!usb_capture_stream_is_running) {
        usb_capture_stream_is_running = true;

        usb_audio_manager_debounce_timer_start();
    }
}

static void audio_usb_capture_stream_stop(void)
{
    LOG_MSG(1, "%s", __func__);

    if (usb_capture_stream_is_running) {
        usb_capture_stream_is_running = false;
        usb_capture_stream_last_tsf = 0;
        usb_capture_stream_data_is_ready = false;

        usb_audio_manager_debounce_timer_start();
    }
}

#if 0
static void audio_usb_reset_stream(void)
{
    LOG_MSG(1, "%s", __func__);

    audio_usb_capture_stream_stop();
    audio_usb_playback_stream_stop();
    usb_audio_manager_debounce_timer_handler(NULL);
}
#endif

void audio_usb_capture_data_send(uint8_t* buff, uint32_t len)
{
    if (buff == NULL || len == 0) {
        LOG_ERR(1, "%s invalid param.", __func__);
        return;
    }

    if (usb_capture_stream_is_running == false) {
        return;
    }

    if (usb_capture_stream_data_is_ready == false) {
        LOG_MSG(1, "usb capture stream data not ready.");
        return;
    }

#if 0
    uint64_t current_usb_tsf = 0;
    int32_t usb_tsf_diff = 0;
    current_usb_tsf = hal_wifitsf_usb_time_get();
    if (usb_capture_stream_last_tsf != 0) {
        usb_tsf_diff = current_usb_tsf - usb_capture_stream_last_tsf;
    }
    usb_capture_stream_last_tsf = current_usb_tsf;

    if (wireless_dbg_level == 2) {
        LOG_MSG(3, "usb tsf:%u, diff:%d, len:%d", (uint32_t)current_usb_tsf, usb_tsf_diff, len);
    }
#endif

    if (0) {
    }
#ifdef USB_AUDIO_ANY_RESAMPLE
    else if (audio_usb_output_stream_resample_is_on() && audio_datapath_usb_output_normal_mode()) {
        audio_usb_output_stream_resample_process(buff, len);
    }
#endif
    else if ((uint32_t)audio_usb_data_queue_get_length() >= len) {
        audio_usb_data_dequeue(buff, len);
    } else {
        memset(buff, 0x00, len);
        LOG_MSG(0, "usb no capture data!");
    }

    // LOG_MSG(1, "usb cache mtu:%d, len:%d", audio_usb_data_queue_get_length() / len, len);

    audio_manager_data_cooking_start_shot(AUDIO_OUTPUT_PATH_BIT_USB);
}

static const struct USB_AUDIO_SOURCE_EVENT_CALLBACK_T wifi_usb_audio_stream_func_cb_list =
{
    .data_send_cb      = audio_usb_capture_data_send,

    .playback_start_cb = audio_usb_playback_stream_start,
    .playback_stop_cb  = audio_usb_playback_stream_stop,

    .capture_start_cb  = audio_usb_capture_stream_start,
    .capture_stop_cb   = audio_usb_capture_stream_stop,

    // .reset_cb          = audio_usb_reset_stream,
};

void audio_datapath_usb_init(void)
{
    usb_audio_source_config_init(&wifi_usb_audio_stream_func_cb_list);

    usb_os_init();

    if (usb_audio_manager_debounce_timer == NULL) {
        usb_audio_manager_debounce_timer = osTimerNew(usb_audio_manager_debounce_timer_handler, osTimerOnce,
                                                      NULL, &usb_audio_manager_debounce_timer_attr);
        ASSERT(usb_audio_manager_debounce_timer, "create usb timer failed.");
    }

    if (audio_usb_tid == NULL) {
        audio_usb_tid = osThreadNew(audio_usb_process_thread, NULL, &audio_usb_process_thread_attr);
        ASSERT(audio_usb_tid, "create usb audio thread failed.");
    }

    audio_usb_data_queue_init();

#ifdef USB_AUDIO_ANY_RESAMPLE
    audio_usb_output_stream_resample_init();
#endif

    audio_manager_usb_capture_stream_start_shot();
}

bool audio_datapath_usb_capture_stream_channel_num_set(uint8_t ch_num)
{
    if (ch_num > 0 && ch_num <= USB_AUDIO_SEND_CHAN && (ch_num % 2 == 0)) {
        usb_capture_stream_channel_num = ch_num;
        return true;
    } else {
        LOG_ERR(1, "invalid usb cap ch num:%d", ch_num);
    }
    return false;
}

uint8_t* audio_datapath_usb_capture_stream_channel_num_get(void)
{
    return &usb_capture_stream_channel_num;
}

void audio_datapath_usb_capture_stream_onoff(bool onoff)
{
    if (usb_capture_stream_started == onoff) {
        return;
    }

    LOG_MSG(1, "audio_usb_capture_stream %s", onoff == true ? "on" : "off");

    if (onoff) {
        btusb_usb_pre_open();
        // usb_audio_reopen(usb_capture_stream_channel_num);
        usb_audio_app(1);
    } else {
        usb_capture_stream_is_running = false;
        usb_playback_stream_is_running = false;
        usb_audio_app(0);
    }
    usb_capture_stream_started = onoff;
}

void audio_usb_output_stream_start_shot(void)
{
    osSignalSet(audio_usb_tid, AUDIO_USB_OUTPUT_STREAM_START);
}

void audio_usb_output_stream_stop_shot(void)
{
    osSignalSet(audio_usb_tid, AUDIO_USB_OUTPUT_STREAM_STOP);
}

void audio_usb_output_stream_process_shot(void)
{
    osSignalSet(audio_usb_tid, AUDIO_USB_OUTPUT_STREAM_PROCESS);
}

void audio_usb_process_thread(void* argument)
{
    osEvent evt;
    uint32_t signals = 0;
    POSSIBLY_UNUSED uint8_t channel_num = 0;

    while (1) {
        evt = osSignalWait(0x0, osWaitForever);
        signals = evt.value.signals;

        if (evt.status == osEventSignal) {
            if (signals & AUDIO_USB_OUTPUT_STREAM_PROCESS) {
                channel_num = *(uint8_t*)audio_datapath_usb_capture_stream_channel_num_get();

                while (audio_usb_data_queue_get_length() >= AUDIO_USB_CAPTURE_STREAM_PROCESS_SIZE) {
                    audio_usb_data_dequeue((uint8_t*)usb_capture_stream_process_buff[usb_capture_stream_process_buff_idx], AUDIO_USB_CAPTURE_STREAM_PROCESS_SIZE);

#ifdef USB_AUDIO_ANY_RESAMPLE
                    if (audio_usb_output_stream_resample_running()) {
#if defined(AUDIO_USB_RESAMPLE_ON_CP) || defined(AUDIO_USB_RESAMPLE_ON_BTH)
                        usb_resample_send.data = (uint8_t*)usb_capture_stream_process_buff[usb_capture_stream_process_buff_idx];
                        usb_resample_send.len = AUDIO_USB_CAPTURE_STREAM_PROCESS_SIZE;
#ifdef LTC_ENABLED
                        usb_resample_send.ltc_mode = ltc_output_mode_get();
                        if (ltc_output_mode_get() && audio_datapath_usb_ltc_output) {
                            usb_resample_send.ltc_data = (uint8_t*)usb_capture_stream_ltc_buff;
                            usb_resample_send.ltc_len = AUDIO_USB_CAPTURE_STREAM_LTC_BUFF_SIZE;
                            audio_datapath_usb_ltc_output = false;
                        }
#endif
                        int ret = -1;
                        if (channel_num == 2) {
                            if (audio_cp_subsys_is_runnig()) {
                                ret = audio_cp_subsys_usb_capture_stream_req(USB_STREAM_DATA_INPUT, (uint8_t*)&usb_resample_send, sizeof(usb_resample_send));
                            }
                        } else if (channel_num == 4 || channel_num == 8) {
                            if (audio_bt_host_is_runnig()) {
                                ret = audio_bt_host_usb_capture_stream_req(USB_STREAM_DATA_INPUT, (uint8_t*)&usb_resample_send, sizeof(usb_resample_send));
                            }
                        } else {
                            ASSERT(0, "invalid usb channel num:%d", channel_num);
                        }

                        if (ret) {
                            LOG_ERR(1, "resample req send failed:%d", ret);
                        }
#else
                        for (uint16_t i = 0; i < AUDIO_USB_RESMAPLE_FRAME_LEN / sizeof(int32_t); i++) {
                            if (channel_num >= 2) {
                                audio_usb_resample_input_buffer[0][i] = usb_capture_stream_process_buff[0][channel_num * i];
                                audio_usb_resample_input_buffer[1][i] = usb_capture_stream_process_buff[0][channel_num * i + 1];
                            }

                            if (channel_num == 4) {
                                audio_usb_resample_input_buffer[2][i] = usb_capture_stream_process_buff[0][channel_num * i + 2];
                                audio_usb_resample_input_buffer[3][i] = usb_capture_stream_process_buff[0][channel_num * i + 3];
                            }
                        }

                        for (uint8_t ch = 0; ch < channel_num; ch++) {
                            audio_usb_output_stream_resample_run(ch, (uint8_t*)audio_usb_resample_input_buffer[ch], AUDIO_USB_RESMAPLE_FRAME_LEN);
                        }
#endif
                    } else {
                        LOG_ERR(0, "resample not running!");
                    }
#endif
                    if (++usb_capture_stream_process_buff_idx >= AUDIO_USB_CAPTURE_STREAM_PROCESS_BUFF_NUM) {
                        usb_capture_stream_process_buff_idx = 0;
                    }
                }
            }

            if (signals & AUDIO_USB_OUTPUT_STREAM_STOP) {
#ifdef USB_AUDIO_ANY_RESAMPLE
                audio_usb_output_stream_resample_onoff(false);
#endif
#ifdef RX_ALGO_PROCESS
                rx_algo_deinit(AUDIO_OUTPUT_PATH_BIT_USB);
#endif
#if defined(AUDIO_USB_ALGO_ON_CP) || defined(AUDIO_USB_RESAMPLE_ON_CP)
                if (usb_capture_stream_channel_num == 2) {
                    audio_cp_subsys_deinit(APP_CP_SUBSYS_USER_USB);
                }
#endif
#if defined(AUDIO_USB_ALGO_ON_BTH) || defined(AUDIO_USB_RESAMPLE_ON_BTH)
                if (usb_capture_stream_channel_num == 4 || usb_capture_stream_channel_num == 8) {
                    audio_bt_host_deinit(APP_BT_HOST_USER_USB);
                }
#endif
                if (audio_datapath_usb_output_normal_mode()) {
                    audio_manager_datapath_clear(AUDIO_OUTPUT_PATH_BIT_USB);
                }
                audio_usb_data_queue_reset();
            }

            if (signals & AUDIO_USB_OUTPUT_STREAM_START) {
#if defined(AUDIO_USB_ALGO_ON_CP) || defined(AUDIO_USB_RESAMPLE_ON_CP)
                if (usb_capture_stream_channel_num == 2) {
                    audio_cp_subsys_init(APP_CP_SUBSYS_USER_USB);
                }
#endif
#if defined(AUDIO_USB_ALGO_ON_BTH) || defined(AUDIO_USB_RESAMPLE_ON_BTH)
                if (usb_capture_stream_channel_num == 4 || usb_capture_stream_channel_num == 8) {
                    audio_bt_host_init(APP_BT_HOST_USER_USB);
                }
#endif
#ifdef RX_ALGO_PROCESS
                rx_algo_init(AUDIO_OUTPUT_PATH_BIT_USB, 48000, 48 * BLE_AUDIO_FRAME_US / 1000, usb_capture_stream_channel_num);
#endif
#ifdef USB_AUDIO_ANY_RESAMPLE
                audio_usb_output_stream_resample_onoff(true);
#endif
                if (audio_datapath_usb_output_normal_mode()) {
                    audio_manager_datapath_set(AUDIO_OUTPUT_PATH_BIT_USB);
                }
                audio_usb_data_queue_reset();
            }
        }
    }
}

void audio_datapath_usb_capture_stream_check_dm_status(void)
{
    // static uint8_t err_cnt = 0;
    // int dp, dm;

    // usb_get_dp_dm_status(&dp, &dm);
    // if (dm && dp) {
    //     err_cnt++;
    // } else {
    //     err_cnt = 0;
    // }

    // if (err_cnt >= 20) {
    //     err_cnt = 0;
    //     hal_usb_detect_disconn();
    //     LOG_MSG(0, "usb detect discon!");
    // }
}

#if defined(AUDIO_USB_ALGO_ON_CP) || defined(AUDIO_USB_ALGO_ON_BTH)
void audio_datapath_usb_capture_stream_algo_on_cp_onoff_set(bool* onoff)
{
    if (usb_capture_stream_channel_num == 2) {
        if (audio_cp_subsys_is_runnig()) {
            audio_cp_subsys_usb_capture_stream_req(USB_STREAM_ALGO_ONOFF, (uint8_t*)onoff, sizeof(bool));
        }
    } else {
        if (audio_bt_host_is_runnig()) {
            audio_bt_host_usb_capture_stream_req(USB_STREAM_ALGO_ONOFF, (uint8_t*)onoff, sizeof(bool));
        }
    }
}

void audio_datapath_usb_capture_stream_algo_on_cp_cfg_set(void)
{
    memset(&audio_datapath_usb_algo, 0x00, sizeof(audio_datapath_usb_algo));

    audio_datapath_usb_algo.channel_merge_config_mono = (const AudioChannelMergeConfig*)ADDR_CPU_TO_DEV(&channel_merge_config_mono);
    audio_datapath_usb_algo.channel_merge_config_stereo = (const AudioChannelMergeConfig*)ADDR_CPU_TO_DEV(&channel_merge_config_stereo);
    audio_datapath_usb_algo.rx_algo_mpdrc_config = (const MultiPointDrcConfig*)ADDR_CPU_TO_DEV(&rx_algo_mpdrc_config[1]);

    if (usb_capture_stream_channel_num == 2) {
        if (audio_cp_subsys_is_runnig()) {
            audio_cp_subsys_usb_capture_stream_req(USB_STREAM_ALGO_CFG_SET, (uint8_t*)&audio_datapath_usb_algo, sizeof(audio_datapath_usb_algo));
        }
    } else {
        if (audio_bt_host_is_runnig()) {
            audio_bt_host_usb_capture_stream_req(USB_STREAM_ALGO_CFG_SET, (uint8_t*)&audio_datapath_usb_algo, sizeof(audio_datapath_usb_algo));
        }
    }
}

void audio_datapath_usb_capture_stream_algo_on_cp_channel_mode_set(uint8_t* mode)
{
    if (usb_capture_stream_channel_num == 2) {
        if (audio_cp_subsys_is_runnig()) {
            audio_cp_subsys_usb_capture_stream_req(USB_STREAM_ALGO_CH_MODE_SET, (uint8_t*)mode, sizeof(uint8_t));
        }
    } else {
        if (audio_bt_host_is_runnig()) {
            audio_bt_host_usb_capture_stream_req(USB_STREAM_ALGO_CH_MODE_SET, (uint8_t*)mode, sizeof(uint8_t));
        }
    }
}

void audio_datapath_usb_capture_stream_algo_on_cp_sw_gain_set(float* gain)
{
    if (usb_capture_stream_channel_num == 2) {
        if (audio_cp_subsys_is_runnig()) {
            audio_cp_subsys_usb_capture_stream_req(USB_STREAM_ALGO_SW_GAIN_SET, (uint8_t*)gain, sizeof(float));
        }
    } else {
        if (audio_bt_host_is_runnig()) {
            audio_bt_host_usb_capture_stream_req(USB_STREAM_ALGO_SW_GAIN_SET, (uint8_t*)gain, sizeof(float));
        }
    }
}

void audio_datapath_usb_capture_stream_algo_on_cp_mpdrc_onoff_set(bool* onoff)
{
    if (usb_capture_stream_channel_num == 2) {
        if (audio_cp_subsys_is_runnig()) {
            audio_cp_subsys_usb_capture_stream_req(USB_STREAM_ALGO_MPDRC_MODE_ONOFF, (uint8_t*)onoff, sizeof(bool));
        }
    } else {
        if (audio_bt_host_is_runnig()) {
            audio_bt_host_usb_capture_stream_req(USB_STREAM_ALGO_MPDRC_MODE_ONOFF, (uint8_t*)onoff, sizeof(bool));
        }
    }
}
#endif