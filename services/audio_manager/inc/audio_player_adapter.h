/***************************************************************************
 *
 * Copyright 2015-2022 BES.
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
#ifndef __AUDIO_PLAYER_ADAPTER_H__
#define __AUDIO_PLAYER_ADAPTER_H__

#include "app_audio_bt_device.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Music
#define  AUDIO_STREAM_MUSIC               0x1
/// Prompt
#define  AUDIO_STREAM_MEDIA               0x2
/// Call
#define  AUDIO_STREAM_VOICE               0x4

#define  AUDIO_STREAM_LOCAL               0x8
/// Walkie talkie stream
#define  AUDIO_STREAM_W_T                 0x100

typedef enum {
    AUD_ID_INVALID  = -1,
    AUD_ID_POWER_ON = 0x0,
    AUD_ID_POWER_OFF = 0x01,
    AUD_ID_LANGUAGE_SWITCH = 0x02,

    AUD_ID_NUM_0 = 0x03,
    AUD_ID_NUM_1 = 0x04,
    AUD_ID_NUM_2 = 0x05,
    AUD_ID_NUM_3 = 0x06,
    AUD_ID_NUM_4 = 0x07,
    AUD_ID_NUM_5 = 0x08,
    AUD_ID_NUM_6 = 0x09,
    AUD_ID_NUM_7 = 0x0A,
    AUD_ID_NUM_8 = 0x0B,
    AUD_ID_NUM_9 = 0x0C,

    AUD_ID_BT_PAIR_ENABLE = 0x0D,
    AUD_ID_BT_PAIRING = 0x0E,
    AUD_ID_BT_PAIRING_SUC = 0x0F,
    AUD_ID_BT_PAIRING_FAIL = 0x10,
    AUD_ID_BT_CALL_REFUSE = 0x11,
    AUD_ID_BT_CALL_OVER = 0x12,
    AUD_ID_BT_CALL_ANSWER = 0x13,
    AUD_ID_BT_CALL_HUNG_UP = 0x14,
    AUD_ID_BT_CALL_INCOMING_CALL = 0x15,
    AUD_ID_BT_CALL_INCOMING_NUMBER = 0x16,
    AUD_ID_BT_CHARGE_PLEASE = 0x17,
    AUD_ID_BT_CHARGE_FINISH = 0x18,
    AUD_ID_BT_CLEAR_SUCCESS = 0x19,
    AUD_ID_BT_CLEAR_FAIL = 0x1A,
    AUD_ID_BT_CONNECTED = 0x1B,
    AUD_ID_BT_DIS_CONNECT = 0x1C,
    AUD_ID_BT_WARNING = 0x1D,
    AUDIO_ID_BT_ALEXA_START = 0x1E,
    AUDIO_ID_FIND_MY_BUDS = 0x1F,
    AUDIO_ID_FIND_TILE = 0x20,
    AUDIO_ID_BT_ALEXA_STOP = 0x21,
    AUDIO_ID_BT_GSOUND_MIC_OPEN = 0x22,
    AUDIO_ID_BT_GSOUND_MIC_CLOSE = 0x23,
    AUDIO_ID_BT_GSOUND_NC = 0x24,
    AUDIO_ID_BT_MUTE = 0x25,
    AUD_ID_RING_WARNING = 0x26,
#ifdef __INTERACTION__
    AUD_ID_BT_FINDME = 0x27,
#endif
    AUD_ID_ANC_PROMPT = 0x28,
    AUD_ID_ANC_MUTE = 0x29,
    AUD_ID_ANC_MODE0 = 0x2A,
    AUD_ID_ANC_MODE1 = 0x2B,
#if BLE_AUDIO_ENABLED
    AUD_ID_LE_AUD_INCOMING_CALL = 0x2C,
#endif
#ifdef SLT_AUTO_TEST
    AUD_ID_TONE_1K = 0x2D,
#endif
    AUD_ID_CUSTOM_LEAK_DETECT = 0x2E,
    AUD_ID_STEREO_TEST = 0x2F,
    MAX_RECORD_NUM = 0x30,
    AUD_ID_WT_FIND_DIVICE = 0x32,//walkie build process find divce
    AUD_ID_WT_ERROR = 0x3f,//walkie build process error
	AUD_ID_OPENEAR_PROMPT = 0x40,

#ifdef BESUI_TWS_EN
    AUD_ID_ANC_ON_MODE = 0x50,
    AUD_ID_ANC_AA_MODE = 0x51,
    AUD_ID_BT_BEASTGAME_MODE = 0x52,
    AUD_ID_SPATIAL_ON = 0x53,
    AUD_ID_SPATIAL_OFF = 0x54,
    AUD_ID_BT_MAX_VOL = 0x55,
	AUD_ID_WEAR_DETECT = 0x56,
    AUD_ID_SIGNATURE = 0x57,
    AUD_ID_DOLBY_ON = 0x58,
    AUD_ID_ANC_ST_MODE = 0x59,
    AUD_ID_GAME_OFF = 0x5A,
    AUD_ID_TWS_CONN = 0x5B,
    AUD_ID_TWS_DISCONN = 0x5C,
    AUD_ID_KEY_VOICE = 0x5D,
    AUD_ID_FACTORY_RESET = 0x5E,
    AUD_ID_TAP_SINGLE = 0x5F,
    AUD_ID_TAP_DOUBLE = 0x60,
    AUD_ID_TAP_TRIPLE = 0x61,
    AUD_ID_ENTER_SOUND = 0x62,
    AUD_ID_EQ_MODE1 = 0x63,
    AUD_ID_EQ_MODE2 = 0x64,
    AUD_ID_EQ_MODE3 = 0x65,
    AUD_ID_EQ_MODE4 = 0x66,
#elif BESUI_STEREO_EN
    AUD_ID_MAX_VOLUME=0x50,
    AUD_ID_EQ_OFF = 0x51,
    AUD_ID_EQ_ON = 0x52,
    AUD_ID_GAME_MODE = 0x53,
    AUD_ID_MUSIC_MODE = 0x54,
    AUD_ID_DOLBY_ON = 0x55,
#endif

    AUD_ID_ENUM_BOTTOM = 0x10000000,    // AUD_ID_ENUM is uint32_t
} AUD_ID_ENUM;

typedef uint32_t (*Audio_player_output_read_callback)(uint8_t *data_ptr, uint32_t data_len);

typedef struct
{
    BT_AUDIO_DEVICE_T  audio_device;
    uint32_t aud_id;
}Audio_device_t;

typedef struct
{
    // 0: player send,  1:device read
    uint8_t data_transfer_mode;
    void (*output_start)(uint32_t sample_rate, uint8_t bits_depth, uint8_t num_channels, uint32_t frame_samples);
    void (*output_stop)();
    void (*output_set_volume)(uint8_t vol);
    // data_transfer_mode = 0
    void (*output_data)(uint8_t *data_ptr, uint32_t data_len);
    // data_transfer_mode = 1
    void (*output_receive_ntf)(void);
    void (*output_reg_read_cb)(Audio_player_output_read_callback cb);
} Audio_output_route_t;

typedef struct
{
    int32_t (*play)( uint16_t stream_type, const Audio_device_t* device);
    int32_t (*stop)( uint16_t stream_type, const Audio_device_t* device);
    int32_t (*pause)(uint16_t stream_type, const Audio_device_t* device);
    void (*play_prompt)(uint32_t id, uint8_t device_id);
    void (*stop_prompt)(uint32_t id, uint8_t device_id);
    int32_t (*seek)(int64_t time_in_ms);
    void (*dump_playback_status)(void);
    int32_t (*set_volume)(uint16_t volume);
    void (*volume_up)();
    void (*volume_down)();
    void (*restore_volume)(uint8_t *bdAddr);
    int32_t (*swap_sco)( uint16_t stream_type, uint8_t device_id);
    void (*set_output_route)(Audio_output_route_t *outpu_route);
    void (*start_capture)(const Audio_device_t* device);
    void (*stop_capture)(const Audio_device_t* device);
}Audio_Player_Adapter_t;

/**
 ****************************************************************************************
 * @brief init adapter
 *
 * @param[in] audio_player        PAC local index
 * @return void
 ****************************************************************************************
 */

void audio_player_adapter_init(const Audio_Player_Adapter_t* audio_player);
/**
 ****************************************************************************************
 * @brief play audio
 *
 * @param[in] stream_type         PAC local index
 * @param[in] device              Record identifier
 *
 * @return playback status
 ****************************************************************************************
 */

int32_t audio_player_play(uint16_t stream_type, const Audio_device_t* device);

/**
 ****************************************************************************************
 * @brief audio_player_playback_stop
 *
 * @param[in] stream_type         AUDIO_STREAM_MUSIC/
 * @param[in] device              device
 *
 * @return playback status
 ****************************************************************************************
 */
int32_t audio_player_playback_stop(uint16_t stream_type, const Audio_device_t* device);

/**
 ****************************************************************************************
 * @brief audio_player_pause
 *
 * @return playback status
 ****************************************************************************************
 */
int32_t audio_player_pause(uint16_t stream_type, const Audio_device_t* device);

/**
 ****************************************************************************************
 * @brief audio_player_seek
 *
 * @param[in] time_in_ms
 *
 * @return playback status
 ****************************************************************************************
 */
int32_t audio_player_seek(int64_t time_in_ms);

/**
 ****************************************************************************************
 * @brief audio_player_dump_playback_status
 *
 * @param[in] time_in_ms
 *
 * @return playback status
 ****************************************************************************************
 */
void audio_player_dump_playback_status(void);

/**
 ****************************************************************************************
 * @brief audio_player_play_prompt
 *
 * @param[in] id
 * @param[in] device
 *
 * @return void
 ****************************************************************************************
 */
void audio_player_play_prompt(uint32_t id, uint8_t device_id);

/**
 ****************************************************************************************
 * @brief audio_player_stop_prompt
 *
 * @param[in] id
 * @param[in] device
 *
 * @return void
 ****************************************************************************************
 */
void audio_player_stop_prompt(uint32_t id, uint8_t device_id);

/**
 ****************************************************************************************
 * @brief audio_player_volume_up
 *
 * @return void
 ****************************************************************************************
 */
void audio_player_volume_up();

/**
 ****************************************************************************************
 * @brief audio_player_volume_up
 *
 * @return void
 ****************************************************************************************
 */
void audio_player_volume_down();

/**
 ****************************************************************************************
 * @brief audio_player_set_volume
 *
 * @param[in] stream_type
 * @param[in] device
 *
 * @return playback status
 ****************************************************************************************
 */
 
int32_t audio_player_set_volume(uint16_t volume);

/**
 ****************************************************************************************
 * @brief audio_player_restore_volume
 *
 * @param[in] device
 *
 * @return void
 ****************************************************************************************
 */
void audio_player_restore_volume(uint8_t *bdAddr);

/**
 ****************************************************************************************
 * @brief audio_player_swap_sco
 *
 * @param[in] stream_type
 * @param[in] device_id
 *
 * @return void
 ****************************************************************************************
 */
int32_t audio_player_swap_sco(uint16_t stream_type, uint8_t device_id);

/**
 ****************************************************************************************
 * @brief audio_player_start_capture
 *
 * @param[in] stream_type
 * @param[in] device
 *
 * @return void
 ****************************************************************************************
 */
void audio_player_start_capture(const Audio_device_t* device);

/**
 ****************************************************************************************
 * @brief audio_player_stop_capture
 *
 * @param[in] stream_type
 * @param[in] device
 *
 * @return void
 ****************************************************************************************
 */
void audio_player_stop_capture(const Audio_device_t* device);

/**
 ****************************************************************************************
 * @brief Convert audio ID to string
 *
 * @param[in] audio ID
 * @param[in] audio string
 *
 * @return void
 ****************************************************************************************
 */
const char *aud_id2str(uint16_t aud_id);

#ifdef __cplusplus
}
#endif

#endif /* __AUDIO_PLAYER_ADAPTER_H__ */