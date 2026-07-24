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
#include "cmsis_os.h"
#include "rx_algo.h"
#include "hal_trace.h"
#include "speech_memory.h"
#include "audio_datapath_manager.h"
// #include "audio_datapath_process.h"
#include "app_wireless_mic.h"
#ifdef UTILS_ESHELL_EN
#include "eshell.h"
#endif
#ifdef LTC_ENABLED
#include "ltc_adapter.h"
#endif
#ifdef AUDIO_I2S_ALGO_ON_CP
#include "audio_datapath_i2s.h"
#endif
#if defined(AUDIO_USB_ALGO_ON_CP) || defined(AUD_OUTPUT_BY_USB)
#include "audio_datapath_usb.h"
#endif

#define INPUT_SESSION_NUM                       (MAX_INPUT_CH_NUM)
#define SPEECH_ALGO_POOL_SIZE                   (1024 * 20)
#define RX_ALGO_PROCESS_BUFFER_SIZE             (48 * INPUT_SESSION_NUM * BLE_AUDIO_FRAME_US / 1000)

static uint8_t speech_algo_buf[SPEECH_ALGO_POOL_SIZE];

static bool rx_algo_heap_inited = false;
static bool rx_algo_inited[AUDIO_OUTPUT_PATH_NUM] = {false, false, false};
static bool rx_algo_run_local[AUDIO_OUTPUT_PATH_NUM] = {false, false, false};
static uint8_t rx_algo_out_ch[AUDIO_OUTPUT_PATH_NUM] = {0};
static bool rx_algo_on[AUDIO_OUTPUT_PATH_NUM] = {true, true, true};
static float rx_sw_gain[AUDIO_OUTPUT_PATH_NUM] = {0.0f};
static uint8_t rx_ch_merge_mode[AUDIO_OUTPUT_PATH_NUM] = {0};
static bool rx_mpdrc_onoff[AUDIO_OUTPUT_PATH_NUM] = {false, false, false};
static AudioChannelMergeState *rx_ch_merge_st[AUDIO_OUTPUT_PATH_NUM] = {NULL};
static SwGainState *rx_sw_gain_st[AUDIO_OUTPUT_PATH_NUM] = {NULL};
static MultiPointDrcState *rx_mpdrc_st[AUDIO_OUTPUT_PATH_NUM] = {NULL};

static float rx_algo_input_buff[RX_ALGO_PROCESS_BUFFER_SIZE] = {0};
static float rx_algo_output_buff[RX_ALGO_PROCESS_BUFFER_SIZE] = {0};
static uint8_t rx_algo_datapath_str[AUDIO_OUTPUT_PATH_NUM][4] = {"dac", "usb", "i2s"};

#ifdef LTC_ENABLED
#define LTC_BACKUP_BUFF_SIZE            (48 * 4 * 5)
uint8_t ltc_backup_buff[LTC_BACKUP_BUFF_SIZE] = {0};
uint16_t ltc_backup_buff_len = 0;

extern bool audio_datapath_usb_ltc_output;
extern uint8_t usb_capture_stream_ltc_buff[];
#endif

static osMutexId rx_algo_mutex_id = NULL;
static const osMutexAttr_t rx_algo_mutex_attr = {
    .name = "rx_algo_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit | osMutexRobust,
    .cb_mem = NULL,
    .cb_size = 0U,
};

const MultiPointDrcConfig rx_algo_mpdrc_config[AUDIO_OUTPUT_PATH_NUM] =
{
    { //dac config
        .x1 = { -80.0, -80.0 },
        .x2 = { -50.0, -50.0 },
        .x3 = { -40.0, -40.0 },
        .x4 = { -30.0, -30.0 },
        .x5 = { -20.0, -20.0 },
        .x6 = { -0.0, -0.0 },
        .attack_time = 3,
        .release_time = 60,
    },

    { // usb config
        .x1 = { -80.0, -80.0 },
        .x2 = { -50.0, -50.0 },
        .x3 = { -40.0, -40.0 },
        .x4 = { -30.0, -30.0 },
        .x5 = { -20.0, -20.0 },
        .x6 = { -0.0, -0.0 },
        .attack_time = 3,
        .release_time = 60,
    },

    { // i2s config
        .x1 = { -80.0, -80.0 },
        .x2 = { -50.0, -50.0 },
        .x3 = { -40.0, -40.0 },
        .x4 = { -30.0, -30.0 },
        .x5 = { -20.0, -20.0 },
        .x6 = { -0.0, -0.0 },
        .attack_time = 3,
        .release_time = 60,
    },
};

const AudioChannelMergeConfig channel_merge_config_mono =
{
#if (INPUT_SESSION_NUM == 8)
    {{1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
     {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
     {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
     {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
     {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
     {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
     {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
     {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}}
#elif (INPUT_SESSION_NUM == 4)
    {{1.0f, 1.0f, 1.0f, 1.0f},
     {1.0f, 1.0f, 1.0f, 1.0f},
     {1.0f, 1.0f, 1.0f, 1.0f},
     {1.0f, 1.0f, 1.0f, 1.0f}}
#elif (INPUT_SESSION_NUM == 2)
    {{1.0f, 1.0f},
     {1.0f, 1.0f}}
#else
    {{1.0f},
     {1.0f}}
#endif
};

const AudioChannelMergeConfig channel_merge_config_stereo =
{
#if (INPUT_SESSION_NUM == 8)
    {{1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f},
     {1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f},
     {1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f},
     {1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f}}
#elif (INPUT_SESSION_NUM == 4)
    {{1.0f, 0.0f, 1.0f, 0.0f},
     {0.0f, 1.0f, 0.0f, 1.0f},
     {1.0f, 0.0f, 1.0f, 0.0f},
     {0.0f, 1.0f, 0.0f, 1.0f}}
#elif (INPUT_SESSION_NUM == 2)
    {{1.0f, 0.0f},
     {0.0f, 1.0f}}
#else
    {{1.0f},
     {1.0f}}
#endif
};

uint8_t rx_algo_audio_output_path_to_idx(uint8_t datapath)
{
    for (uint8_t idx = 0; idx < AUDIO_OUTPUT_PATH_NUM; idx++) {
        if (datapath & (0x1 << idx)) {
            return idx;
        }
    }
    return 0xFF;
}

void audio_copy_4ch_32bit_to_2ch_32bit(uint8_t* dest, uint8_t* src, uint32_t src_bytes)
{
    uint32_t i = src_bytes / 16;
    int32_t *d = (int32_t *)dest;
    int32_t *s = (int32_t *)src;

    for (;i > 0; i--) {
        d[0] = (s[0]+s[1]+s[2]+s[3]) / 4;
        d[1] = d[0];

        // d[0] = s[0];
        // d[1] = s[1];

        d += 2;
        s += 4;
    }
}

void rx_algo_init(uint8_t datapath, int32_t sample_rate, int32_t frame_size, int32_t ch_num)
{
    uint8_t idx = rx_algo_audio_output_path_to_idx(datapath);

    if (idx >= AUDIO_OUTPUT_PATH_NUM || rx_algo_inited[idx] == true) {
        return;
    }

#ifdef AUD_OUTPUT_BY_USB
    if (idx == AUDIO_OUTPUT_PATH_BIT_USB && ch_num > USB_AUDIO_SEND_CHAN) {
        LOG_ERR(1, "datapath[usb] set invalid ch num:%d", ch_num);
        return;
    }
#endif

    if (rx_algo_mutex_id == NULL) {
        rx_algo_mutex_id = osMutexNew(&rx_algo_mutex_attr);
        ASSERT(rx_algo_mutex_id, "create rx algo mutex failed.");
    }

    osMutexAcquire(rx_algo_mutex_id, osWaitForever);

    if (rx_algo_heap_inited == false) {
        speech_heap_init(speech_algo_buf, SPEECH_ALGO_POOL_SIZE);
        rx_algo_heap_inited = true;
    }

    rx_algo_run_local[idx] = true;

    if (datapath == AUDIO_OUTPUT_PATH_BIT_DAC) {
        if (ch_num != 2) {
            LOG_ERR(1, "datapath[dac] set invalid ch num:%d", ch_num);
            goto exit;
        }
    } else if (datapath == AUDIO_OUTPUT_PATH_BIT_I2S) {
        if (ch_num != 2) {
            LOG_ERR(1, "datapath[i2s] set invalid ch num:%d", ch_num);
            goto exit;
        }
#ifdef AUDIO_I2S_ALGO_ON_CP
        audio_datapath_i2s_playback_stream_algo_on_cp_cfg_set();
        rx_algo_run_local[idx] = false;
#endif
    } else if (datapath == AUDIO_OUTPUT_PATH_BIT_USB) {
        if (ch_num % 2) {
            LOG_ERR(1, "datapath[i2s] set invalid ch num:%d", ch_num);
            goto exit;
        }
#if defined(AUDIO_USB_ALGO_ON_CP) || defined(AUDIO_USB_ALGO_ON_BTH)
        audio_datapath_usb_capture_stream_algo_on_cp_cfg_set();
        rx_algo_run_local[idx] = false;
#endif
    }

    if (rx_algo_run_local[idx]) {
        custom_allocator *allocator = default_allocator();
        ASSERT(allocator, "invalid allocator");
        rx_ch_merge_st[idx] = audio_channel_merge_create(sample_rate, frame_size, ch_num, &channel_merge_config_stereo, allocator);
        rx_sw_gain_st[idx] = sw_gain_create(sample_rate, frame_size, ch_num, 0.0f, allocator);
        rx_mpdrc_st[idx] = multipoint_drc_create(sample_rate, frame_size, ch_num, &rx_algo_mpdrc_config[idx], allocator);
        // disable mdrc as customer not need.
        rx_mpdrc_st[idx]->open_flag = false;
    }

    rx_algo_out_ch[idx] = ch_num;

    rx_algo_inited[idx] = true;

    LOG_MSG(1, "datapath[%s] rx algo init done.", rx_algo_datapath_str[idx]);

exit:
    osMutexRelease(rx_algo_mutex_id);
}

void rx_algo_deinit(uint8_t datapath)
{
    uint8_t idx = rx_algo_audio_output_path_to_idx(datapath);

    if (idx >= AUDIO_OUTPUT_PATH_NUM || rx_algo_inited[idx] == false) {
        return;
    }

    osMutexAcquire(rx_algo_mutex_id, osWaitForever);

    if (0) {
    }
#ifdef AUDIO_I2S_ALGO_ON_CP
    else if (datapath == AUDIO_OUTPUT_PATH_BIT_I2S) {
    }
#endif
#ifdef AUDIO_USB_ALGO_ON_CP
    else if (datapath == AUDIO_OUTPUT_PATH_BIT_USB) {
    }
#endif
    else {
        if (rx_ch_merge_st[idx]) {
            audio_channel_merge_destroy(rx_ch_merge_st[idx]);
            rx_ch_merge_st[idx] = NULL;
        }

        if (rx_sw_gain_st[idx]) {
            sw_gain_destroy(rx_sw_gain_st[idx]);
            rx_sw_gain_st[idx] = NULL;
        }

        if (rx_mpdrc_st[idx]) {
            multipoint_drc_destroy(rx_mpdrc_st[idx]);
            rx_mpdrc_st[idx] = NULL;
        }
    }

    rx_algo_out_ch[idx] = 0;
    rx_algo_inited[idx] = false;

    osMutexRelease(rx_algo_mutex_id);

    LOG_MSG(1, "datapath[%s] rx algo deinit done.", rx_algo_datapath_str[idx]);
}

void rx_algo_process(uint8_t datapath, uint8_t* dest, uint8_t* src, uint32_t src_bytes)
{
    uint8_t idx = rx_algo_audio_output_path_to_idx(datapath);
    uint32_t frame_len = src_bytes / 4 / INPUT_SESSION_NUM;
    uint32_t input_pcm_len = frame_len * INPUT_SESSION_NUM;
    uint32_t output_pcm_len = frame_len * rx_algo_out_ch[idx];

    if (idx >= AUDIO_OUTPUT_PATH_NUM || rx_algo_inited[idx] == false) {
        memset(dest, 0x0, src_bytes);
        return;
    }

    if (src_bytes > RX_ALGO_PROCESS_BUFFER_SIZE * sizeof(float)) {
        LOG_ERR(1, "rx algo input size exceed! %d,%d", src_bytes, RX_ALGO_PROCESS_BUFFER_SIZE * sizeof(float));
        return;
    }

    osMutexAcquire(rx_algo_mutex_id, osWaitForever);

    if (rx_algo_on[idx] && rx_algo_run_local[idx]) {
        int_to_float((int32_t *)src, rx_algo_input_buff, input_pcm_len);

        audio_channel_merge_process(rx_ch_merge_st[idx], rx_algo_input_buff, rx_algo_output_buff, input_pcm_len);

        sw_gain_process(rx_sw_gain_st[idx], rx_algo_output_buff, output_pcm_len);

        multipoint_drc_process(rx_mpdrc_st[idx], rx_algo_output_buff, output_pcm_len);

        float_to_int(rx_algo_output_buff, (int32_t *)dest, output_pcm_len);
    } else {
#if (INPUT_SESSION_NUM == 2)
        if (rx_algo_out_ch[idx] == 2) {
            memcpy(dest, src, src_bytes);
        }
#elif (INPUT_SESSION_NUM == 4)
        if (rx_algo_out_ch[idx] == 2) {
            audio_copy_4ch_32bit_to_2ch_32bit(dest, src, src_bytes);
        } else if (rx_algo_out_ch[idx] == 4) {
            memcpy(dest, src, src_bytes);
        }
#elif (INPUT_SESSION_NUM == 8)
        if (rx_algo_out_ch[idx] == 2) {
            audio_copy_8ch_32bit_to_2ch_32bit(dest, src, src_bytes);
        } else if (rx_algo_out_ch[idx] == 8) {
            memcpy(dest, src, src_bytes);
        }
#endif
    }

#ifdef LTC_ENABLED
    if (ltc_output_mode_get() && (datapath == AUDIO_OUTPUT_PATH_BIT_DAC)) {
        int32_t ltc_buff[output_pcm_len / 2] = {0};
        int32_t* dest_int32 = (int32_t*)dest;
        uint32_t ltc_output_len = output_pcm_len / 2 * 4;
        if (ltc_output_len <= (uint32_t)ltc_frame_queue_get_length()) {
            ltc_frame_dequeue((uint8_t*)ltc_buff, ltc_output_len);

            if (ltc_backup_buff_len == LTC_BACKUP_BUFF_SIZE) {
                memcpy(usb_capture_stream_ltc_buff, ltc_backup_buff, LTC_BACKUP_BUFF_SIZE);
                audio_datapath_usb_ltc_output = true;
                ltc_backup_buff_len = 0;
            } else if (ltc_backup_buff_len > LTC_BACKUP_BUFF_SIZE) {
                LOG_ERR(1, "ltc len not aligned! ltc out len:%d, buff size:%d", ltc_output_len, LTC_BACKUP_BUFF_SIZE);
                osMutexRelease(rx_algo_mutex_id);
                return;
            }
            memcpy((uint8_t*)ltc_backup_buff + ltc_backup_buff_len, (uint8_t*)ltc_buff, ltc_output_len);
            ltc_backup_buff_len += ltc_output_len;

            for (uint16_t i = 0; i < (output_pcm_len / 2); i++) {
                dest_int32[2 * i] = ltc_buff[i];
                if (ltc_output_mode_get() == LTC_OUTPUT_MODE_LOUT) {
                    dest_int32[2 * i + 1] = 0;
                }
            }
        }
    }
#endif

    osMutexRelease(rx_algo_mutex_id);
}

uint8_t rx_algo_merge_channel_mode_get(uint8_t datapath)
{
    uint8_t idx = rx_algo_audio_output_path_to_idx(datapath);

    if (idx >= AUDIO_OUTPUT_PATH_NUM || rx_algo_inited[idx] == false) {
        return 0xFF;
    }

    return rx_ch_merge_mode[idx];
}

void rx_algo_merge_channel_mode_set(uint8_t datapath, uint8_t mode)
{
    uint8_t idx = rx_algo_audio_output_path_to_idx(datapath);

    if (idx >= AUDIO_OUTPUT_PATH_NUM || rx_algo_inited[idx] == false) {
        return;
    }

    rx_ch_merge_mode[idx] = mode;

    if (0) {
    }
#ifdef AUDIO_I2S_ALGO_ON_CP
    else if (datapath == AUDIO_OUTPUT_PATH_BIT_I2S) {
        audio_datapath_i2s_playback_stream_algo_on_cp_channel_mode_set(&rx_ch_merge_mode[idx]);
    }
#endif
#if defined(AUDIO_USB_ALGO_ON_CP) || defined(AUDIO_USB_ALGO_ON_BTH)
    else if (datapath == AUDIO_OUTPUT_PATH_BIT_USB) {
        audio_datapath_usb_capture_stream_algo_on_cp_channel_mode_set(&rx_ch_merge_mode[idx]);
    }
#endif
    else if (rx_ch_merge_st[idx]) {
        if (mode == 1) {
            audio_channel_merge_set_config(rx_ch_merge_st[idx], &channel_merge_config_stereo);
        } else {
            audio_channel_merge_set_config(rx_ch_merge_st[idx], &channel_merge_config_mono);
        }
    }

    LOG_MSG(2, "datapath[%s] merge ch mode:%d ...", rx_algo_datapath_str[idx], mode);
    return;
}

bool rx_algo_mdrc_switch_get(uint8_t datapath)
{
    uint8_t idx = rx_algo_audio_output_path_to_idx(datapath);

    if (idx >= AUDIO_OUTPUT_PATH_NUM || rx_algo_inited[idx] == false || rx_mpdrc_st[idx] == NULL) {
        return false;
    }

    return rx_mpdrc_st[idx]->open_flag;
}

void rx_algo_mdrc_switch_set(uint8_t datapath, bool onoff)
{
    uint8_t idx = rx_algo_audio_output_path_to_idx(datapath);

    if (idx >= AUDIO_OUTPUT_PATH_NUM || rx_algo_inited[idx] == false) {
        return ;
    }

    rx_mpdrc_onoff[idx] = onoff;

    if (0) {
    }
#ifdef AUDIO_I2S_ALGO_ON_CP
    else if (datapath == AUDIO_OUTPUT_PATH_BIT_I2S) {
        audio_datapath_i2s_playback_stream_algo_on_cp_mpdrc_onoff_set(&rx_mpdrc_onoff[idx]);
    }
#endif
#if defined(AUDIO_USB_ALGO_ON_CP) || defined(AUDIO_USB_ALGO_ON_BTH)
    else if (datapath == AUDIO_OUTPUT_PATH_BIT_USB) {
        audio_datapath_usb_capture_stream_algo_on_cp_mpdrc_onoff_set(&rx_mpdrc_onoff[idx]);
    }
#endif
    else if (rx_mpdrc_st[idx]) {
        multipoint_drc_switch(rx_mpdrc_st[idx], onoff);
    }

    LOG_MSG(2, "datapath[%s] mdrc switch set:%d ...", rx_algo_datapath_str[idx], onoff);
}

float rx_algo_sw_gain_get(uint8_t datapath)
{
    uint8_t idx = rx_algo_audio_output_path_to_idx(datapath);

    if (idx >= AUDIO_OUTPUT_PATH_NUM || rx_algo_inited[idx] == false) {
        LOG_ERR(0, "idx[%d] invalid or algo not inited.", idx);
        return 0.0f;
    }

    return rx_sw_gain[idx];
}

void rx_algo_sw_gain_set(uint8_t datapath, float gain)
{
    uint8_t idx = rx_algo_audio_output_path_to_idx(datapath);

    if (idx >= AUDIO_OUTPUT_PATH_NUM || rx_algo_inited[idx] == false) {
        return;
    }

    rx_sw_gain[idx] = gain;

    if (0) {
    }
#ifdef AUDIO_I2S_ALGO_ON_CP
    else if (datapath == AUDIO_OUTPUT_PATH_BIT_I2S) {
        audio_datapath_i2s_playback_stream_algo_on_cp_sw_gain_set(&rx_sw_gain[idx]);
    }
#endif
#if defined(AUDIO_USB_ALGO_ON_CP) || defined(AUDIO_USB_ALGO_ON_BTH)
    else if (datapath == AUDIO_OUTPUT_PATH_BIT_USB) {
        audio_datapath_usb_capture_stream_algo_on_cp_sw_gain_set(&rx_sw_gain[idx]);
    }
#endif
    else if (rx_sw_gain_st[idx]) {
        sw_gain_set_config(rx_sw_gain_st[idx], rx_sw_gain[idx]);
    }
    LOG_MSG(2, "datapath[%s] sw gain set: (f*1000):%d ...", rx_algo_datapath_str[idx], (uint32_t)(rx_sw_gain[idx]*1000));
}

#ifdef UTILS_ESHELL_EN
static void rx_algo_onoff_cmd(int argc, char *argv[])
{
    bool onoff = 0;

    if (argc != 2) {
        LOG_ERR(0, "error arg.");
        return ;
    }

    onoff = atoi(argv[1]);
    for (uint8_t idx = 0; idx < AUDIO_OUTPUT_PATH_NUM; idx++) {
        rx_algo_on[idx] = onoff;
#if defined(AUDIO_I2S_ALGO_ON_CP)
        if (idx == rx_algo_audio_output_path_to_idx(AUDIO_OUTPUT_PATH_BIT_I2S)) {
            audio_datapath_i2s_playback_stream_algo_on_cp_onoff_set(&rx_algo_on[idx]);
        }
#endif
#if defined(AUDIO_USB_ALGO_ON_CP) || defined(AUDIO_USB_ALGO_ON_BTH)
        if (idx == rx_algo_audio_output_path_to_idx(AUDIO_OUTPUT_PATH_BIT_USB)) {
            audio_datapath_usb_capture_stream_algo_on_cp_onoff_set(&rx_algo_on[idx]);
        }
#endif
    }

    LOG_MSG(1, "rx algo onoff:%d", onoff);
}

static void rx_algo_sw_gain_onoff_cmd(int argc, char *argv[])
{
    bool onoff = 0;

    if (argc != 2) {
        LOG_ERR(0, "error arg.");
        return ;
    }

    onoff = atoi(argv[1]);
    for (uint8_t idx = 0; idx < AUDIO_OUTPUT_PATH_NUM; idx++) {
        if (rx_algo_inited[idx] == false) {
            LOG_ERR(1, "datapath[%s] rx algo not inited.", rx_algo_datapath_str[idx]);
            break;
        }
        sw_gain_switch(rx_sw_gain_st[idx], onoff);
#if defined(AUDIO_I2S_ALGO_ON_CP)
        if (idx == rx_algo_audio_output_path_to_idx(AUDIO_OUTPUT_PATH_BIT_I2S)) {
            audio_datapath_i2s_playback_stream_algo_on_cp_onoff_set(&rx_algo_on[idx]);
        }
#endif
#if defined(AUDIO_USB_ALGO_ON_CP) || defined(AUDIO_USB_ALGO_ON_BTH)
        if (idx == rx_algo_audio_output_path_to_idx(AUDIO_OUTPUT_PATH_BIT_USB)) {
            audio_datapath_usb_capture_stream_algo_on_cp_onoff_set(&rx_algo_on[idx]);
        }
#endif
    }

    LOG_MSG(1, "rx algo sw gain onoff:%d", onoff);
}

static void rx_algo_mpdrc_onoff_cmd(int argc, char *argv[])
{
    bool onoff = 0;

    if (argc != 2) {
        LOG_ERR(0, "error arg.");
        return ;
    }

    onoff = atoi(argv[1]);
    for (uint8_t idx = 0; idx < AUDIO_OUTPUT_PATH_NUM; idx++) {
        rx_algo_mdrc_switch_set(0x1 << idx, onoff);
    }

    LOG_MSG(1, "rx algo mdrc onoff:%d", onoff);
}

static void rx_algo_sw_gain_set_cmd(int argc, char *argv[])
{
    float gain = 0.0f;

    if (argc != 2) {
        LOG_ERR(0, "error arg.");
        return ;
    }

    gain = (float)atof(argv[1]);
    if (abs(gain) >= 50.0f) {
        LOG_ERR(0, "sw_gain should between (-50.0, 50.0)");
        return ;
    }

    for (uint8_t idx = 0; idx < AUDIO_OUTPUT_PATH_NUM; idx++) {
        rx_algo_sw_gain_set(0x1 << idx, gain);
    }

    LOG_MSG(1, "rx algo sw gain: (f*1000):%d", (uint32_t)(gain*1000));
}

static void rx_algo_set_merge_channel_mode_cmd(int argc, char *argv[])
{
    int mode = 0;

    if (argc != 2) {
        LOG_ERR(0, "error arg.");
        return ;
    }

    mode = atoi(argv[1]);
    for (uint8_t idx = 0; idx < AUDIO_OUTPUT_PATH_NUM; idx++) {
        rx_algo_merge_channel_mode_set(0x1 << idx, mode);
    }
    LOG_MSG(1, "rx algo merge ch mode:%d", mode);

    return;
}

static void rx_algo_get_current_status_cmd(int argc, char *argv[])
{
    for (uint8_t idx = 0; idx < AUDIO_OUTPUT_PATH_NUM; idx++) {
        if (rx_algo_inited[idx]) {
            LOG_MSG(4, "datapath[%s] sw gain[%d/(f*1000):%d], mdrc[%d], ch mode[%d]", rx_algo_datapath_str[idx], rx_sw_gain_st[idx]->open_flag,
                    (uint32_t)(rx_sw_gain[idx]*1000), rx_mpdrc_st[idx]->open_flag, rx_ch_merge_mode[idx]);
        } else {
            LOG_ERR(1, "datapath[%s] rx algo not inited.", rx_algo_datapath_str[idx]);
        }
    }
}
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "rx_algo_on", "rx_algo_on 0/1", rx_algo_onoff_cmd);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "rx_algo_sw_gain_on", "rx_algo_sw_gain_on 0/1", rx_algo_sw_gain_onoff_cmd);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "rx_algo_sw_gain_set", "rx_algo_sw_gain_set -50-50", rx_algo_sw_gain_set_cmd);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "rx_algo_mdrc_on", "rx_algo_mdrc_on 0/1", rx_algo_mpdrc_onoff_cmd);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "rx_algo_ch_mode", "rx_algo_ch_mode 0/1", rx_algo_set_merge_channel_mode_cmd);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "rx_algo_get_status", "rx_algo_get_status", rx_algo_get_current_status_cmd);
#endif