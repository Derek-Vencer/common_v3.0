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
 * @brief xxx.
 *
 ****************************************************************************/

/****************************** header include ********************************/
#include <stdint.h>
#include "ble_audio_define.h"

/***************************** external declaration *****************************/

/***************************** macro defination *******************************/

/***************************** type defination ********************************/

/***************************** variable defination *****************************/


/***************************** function declaration ****************************/

#ifndef __GAF_AUDIO_PATH_H__
#define __GAF_AUDIO_PATH_H__

#ifdef __cplusplus
extern "C"{
#endif

#define I2S_AUD_STREAM_ID_PLAYBACK      AUD_STREAM_ID_0
#define I2S_AUD_STREAM_ID_CAPTURE       AUD_STREAM_ID_1

typedef void (*I2S_AUDIO_SOURCE_INIT_CALLBACK)(void);
typedef void (*I2S_AUDIO_SOURCE_DEINIT_CALLBACK)(void);
typedef void (*I2S_AUDIO_SOURCE_DATA_PLAYBACK_CALLBACK)(uint8_t * pcm_buf, uint32_t len);
typedef void (*I2S_AUDIO_SOURCE_DATA_CAPTURE_CALLBACK)(uint8_t * pcm_buf, uint32_t len);
typedef void (*I2S_AUDIO_SOURCE_PLAYBACK_STREAM_START_CALLBACK)(void);
typedef void (*I2S_AUDIO_SOURCE_PLAYBACK_STREAM_STOP_CALLBACK)(void);
typedef void (*I2S_AUDIO_SOURCE_CAPTURE_STREAM_START_CALLBACK)(void);
typedef void (*I2S_AUDIO_SOURCE_CAPTURE_STREAM_STOP_CALLBACK)(void);
typedef bool (*I2S_AUDIO_SOURCE_RESET_CODEC_FEASIBILITY_CHECK_CALLBACK)(void);
typedef void (*I2S_AUDIO_SOURCE_DATA_PREPARATION_CALLBACK)(enum AUD_STREAM_T stream);
typedef void (*I2S_ADUIO_SOURCE_PLAYBACK_VOL_CHANGE)(uint32_t level);

struct I2S_AUDIO_SOURCE_EVENT_CALLBACK_T {
    I2S_AUDIO_SOURCE_INIT_CALLBACK                     init_cb;
    I2S_AUDIO_SOURCE_DEINIT_CALLBACK                   deinit_cb;
    I2S_AUDIO_SOURCE_DATA_PLAYBACK_CALLBACK            data_playback_cb;
    I2S_AUDIO_SOURCE_DATA_CAPTURE_CALLBACK             data_capture_cb;
    I2S_AUDIO_SOURCE_RESET_CODEC_FEASIBILITY_CHECK_CALLBACK reset_codec_feasibility_check_cb;
    I2S_AUDIO_SOURCE_DATA_PREPARATION_CALLBACK         data_prep_cb;
    I2S_AUDIO_SOURCE_PLAYBACK_STREAM_START_CALLBACK    playback_start_cb;
    I2S_AUDIO_SOURCE_PLAYBACK_STREAM_STOP_CALLBACK     playback_stop_cb;
    I2S_AUDIO_SOURCE_CAPTURE_STREAM_START_CALLBACK     capture_start_cb;
    I2S_AUDIO_SOURCE_CAPTURE_STREAM_STOP_CALLBACK      capture_stop_cb;

    I2S_ADUIO_SOURCE_PLAYBACK_VOL_CHANGE               playback_vol_change_cb;
};

void app_ble_i2s_audio_init(void);
void app_ble_i2s_media_ascc_release_stream_test(void);
AOB_MGR_STREAM_STATE_E app_ble_i2s_media_get_cur_ase_state(void);
void app_ble_i2s_media_ascc_start_stream_test(void);

void gap_mobile_start_i2s_audio_receiving_dma(void);
void gap_mobile_start_i2s_audio_transmission_dma(void);
void gaf_mobile_i2s_dma_playback_stop(void);
void gaf_mobile_i2s_dma_capture_stop(void);

int gaf_mobile_i2s_audio_media_stream_start_handler(void* _pStreamEnv);
int gaf_mobile_i2s_audio_media_stream_stop_handler(void* _pStreamEnv);
int gaf_mobile_i2s_audio_capture_start_handler(void* _pStreamEnv);
int gaf_mobile_i2s_audio_capture_stop_handler(void* _pStreamEnv);
void gaf_mobile_usb_reconn_start(void);
void gaf_mobile_usb_reconn_stop(void);
bool gaf_mobile_i2s_audio_check_capture_need_start(void);

void app_ble_i2s_stream_inform_playback_state(uint32_t event);
void app_ble_i2s_stream_inform_capture_state(uint32_t event);

void i2s_audio_source_config_init(const struct I2S_AUDIO_SOURCE_EVENT_CALLBACK_T *cb_list);

void app_ble_audio_i2s_playback_stream_start(void);

void app_ble_audio_i2s_playback_stream_stop(void);

void app_ble_audio_i2s_capture_stream_start(void);

void app_ble_audio_i2s_capture_stream_stop(void);

void app_ble_audio_i2s_playback_stream_vol_control(uint32_t volume_level);

uint8_t app_ble_audio_i2s_playback_stream_vol_get(void);

void app_ble_audio_stream_start_pre_handler(void);

void app_ble_audio_stream_stop_post_handler(void);

void app_ble_audio_low_latency_playback_stream_start(void);

void app_ble_audio_low_latency_playback_stream_stop(void);

void app_ble_audio_low_latency_playback_stream_vol_control(uint32_t volume_level);


void app_ble_usb_audio_init(void);

void gap_mobile_start_usb_audio_transmission_dma(void);
void gaf_mobile_usb_dma_playback_stop(void);
void gaf_mobile_usb_dma_capture_stop(void);
uint32_t gaf_mobile_usb_audio_capture_dma_irq_handler(uint8_t* ptrBuf, uint32_t length);
void gaf_mobile_usb_feed_data(uint8_t* ptrBuf, uint32_t length);
void gaf_mobile_usb_processing_received_data(uint8_t* ptrBuf, uint32_t length);

int gaf_mobile_usb_audio_media_stream_start_handler(void* _pStreamEnv);
#ifdef GAF_OFFLOAD_ENABLE
void gaf_mobile_playback_stream_enter_started_state(void);
void gaf_mobile_capture_stream_enter_started_state(void);
void* gaf_mobile_audio_get_media_stream_env(void);
#endif
int gaf_mobile_usb_audio_media_stream_stop_handler(void* _pStreamEnv);
int gaf_mobile_usb_audio_capture_start_handler(void* _pStreamEnv);
int gaf_mobile_usb_audio_capture_stop_handler(void* _pStreamEnv);



typedef struct{
    uint32_t message_id;
    uint32_t message_param0;
    uint32_t message_param1;
} APP_BLE_WIFI_DUAL_MODE_COMM_PACKET_T;

typedef enum {
    APP_BLE_WIFI_COMM_PLAYBACK_STREAM_START = 0,
    APP_BLE_WIFI_COMM_PLAYBACK_STREAM_STOP,
    APP_BLE_WIFI_COMM_CAPTURE_STREAM_START,
    APP_BLE_WIFI_COMM_CAPTURE_STREAM_STOP,
    APP_BLE_WIFI_COMM_PLAYBACK_VOLUME_CTRL,
} APP_BLE_WIFI_DUAL_MODE_COMM_ID_E;

typedef enum {
    APP_BLE_WIFI_MEDIA_CTRL_VOLUME_UP     = (1 << 0),
    APP_BLE_WIFI_MEDIA_CTRL_VOLUME_DOWN   = (1 << 1),
    APP_BLE_WIFI_MEDIA_CTRL_PLAY_PAUSE    = (1 << 2),
    APP_BLE_WIFI_MEDIA_CTRL_SCAN_NEXT     = (1 << 3),
    APP_BLE_WIFI_MEDIA_CTRL_SCAN_PREV     = (1 << 4),
    APP_BLE_WIFI_MEDIA_CTRL_STOP          = (1 << 5),
    APP_BLE_WIFI_MEDIA_CTRL_FAST_FWD      = (1 << 6),
    APP_BLE_WIFI_MEDIA_CTRL_REWIND        = (1 << 7),
    APP_BLE_WIFI_MEDIA_CTRL_VOL_MUTE      = (1 << 8),
    APP_BLE_WIFI_MEDIA_CTRL_VOICE_CMD     = (1 << 9),
    APP_BLE_WIFI_MEDIA_CTRL_REDIAL        = (1 << 10),
    APP_BLE_WIFI_MEDIA_CTRL_HOOK_SWITCH   = (1 << 11),
    APP_BLE_WIFI_MEDIA_CTRL_MIC_MUTE      = (1 << 12),
    APP_BLE_WIFI_MEDIA_CTRL_VOICE_MAIL    = (1 << 13),
} APP_BLE_WIFI_DUAL_MODE_MEDIA_CTRL_CMD_E;

#define PAIRING_INFO_SSID_MAX_LENGTH        (64)
#define PAIRING_INFO_PASSWORD_MAX_LENGTH    (64)

typedef struct{
    uint8_t  ap_ssid[PAIRING_INFO_SSID_MAX_LENGTH];
    uint8_t  ap_password[PAIRING_INFO_PASSWORD_MAX_LENGTH];
    uint16_t ap_ssid_length;
    uint16_t ap_password_length;
} APP_BLE_WIFI_PAIRING_INFO_PACKET_T;

#define BLE_ADV_TYPE_WIFI_HEADER        (0xFF)
#define BLE_ADV_DATA_WIFI_HEADER        "WIFI"
#define BLE_ADV_DATA_WIFI_HEADER_LEN    (4)

void app_ble_wifi_pairing_init(void);

void app_ble_wifi_enter_pairing_mode(void);

bool app_ble_wifi_is_pairing_mode(void);

APP_BLE_WIFI_PAIRING_INFO_PACKET_T* app_get_wifi_pairing_info_ptr(void);

void app_ble_wifi_dual_mode_media_control_send_cmd(APP_BLE_WIFI_DUAL_MODE_MEDIA_CTRL_CMD_E cmd);

void app_ble_wifi_pairing_info_exchange_send_cmd(APP_BLE_WIFI_PAIRING_INFO_PACKET_T pairing_info);

#ifdef DONGLE_AS_I2S_MASTER
void app_ble_wifi_dual_mode_audio_manager_playback_stream_onoff(void);

void app_ble_wifi_dual_mode_audio_manager_capture_stream_onoff(void);
#endif

void app_ble_wifi_dual_mode_audio_manager_send_request_remotely(APP_BLE_WIFI_DUAL_MODE_COMM_ID_E cmd, uint32_t param0, uint32_t param1);

void app_ble_wifi_dual_mode_init(void);

#ifdef __cplusplus
}
#endif

#endif //__GAF_AUDIO_PATH_H__