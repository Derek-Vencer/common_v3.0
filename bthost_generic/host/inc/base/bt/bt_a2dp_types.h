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

#pragma once
#include "bt_base_types.h"

#define BT_A2DP_CODEC_TYPE_INVALID       0xBD

/********************************Define spec codec************************************/
#define BT_A2DP_CODEC_TYPE_SBC           0x00

#define BT_A2DP_CODEC_TYPE_MPEG1_2_AUDIO 0x01

#define BT_A2DP_CODEC_TYPE_MPEG2_4_AAC   0x02

#define BT_A2DP_CODEC_TYPE_ATRAC         0x04

#define BT_A2DP_CODEC_TYPE_OPUS          0x08

#define BT_A2DP_CODEC_TYPE_H263          0x01

#define BT_A2DP_CODEC_TYPE_MPEG4_VSP     0x02

#define BT_A2DP_CODEC_TYPE_H263_PROF3    0x03

#define BT_A2DP_CODEC_TYPE_H263_PROF8    0x04

#define BT_A2DP_CODEC_TYPE_LHDC          0xFF

#define BT_A2DP_CODEC_TYPE_LDAC          0xFF

#define BT_A2DP_CODEC_TYPE_NON_A2DP      0xFF

/********************************Define vender codec************************************/
#define BT_A2DP_CODEC_NONE_TYPE_INVALID  (0x00)

#define BT_A2DP_CODEC_NONE_TYPE_LHDC     (0x01)

#define BT_A2DP_CODEC_NONE_TYPE_LHDCV5   (0x02)

#define BT_A2DP_CODEC_NONE_TYPE_LDAC     (0x03)

#define BT_A2DP_CODEC_NONE_TYPE_SCALABLE (0x04)

#define BT_A2DP_CODEC_NONE_TYPE_LC3      (0x05)

#define BT_A2DP_CODEC_NONE_TYPE_L2HC     (0x06)

#define BT_A2DP_CODEC_NONE_TYPE_MIHC     (0x07)

#define BT_A2DP_CODEC_NONE_TYPE_OPUS     (0x08)

typedef enum {
    BT_A2DP_CODEC_PRORITY_0 = 0,
    BT_A2DP_CODEC_PRORITY_1,
    BT_A2DP_CODEC_PRORITY_2,
    BT_A2DP_CODEC_PRORITY_3,
    BT_A2DP_CODEC_PRORITY_4,
    BT_A2DP_CODEC_PRORITY_5,
    BT_A2DP_CODEC_PRORITY_6,
    BT_A2DP_CODEC_PRORITY_7,
    BT_A2DP_CODEC_PRORITY_8,
    BT_A2DP_CODEC_PRORITY_9,
    BT_A2DP_CODEC_PRORITY_10,
    BT_A2DP_CODEC_PRORITY_11,
    BT_A2DP_CODEC_PRORITY_12,
    BT_A2DP_CODEC_PRORITY_13,
    BT_A2DP_CODEC_PRORITY_14,
    BT_A2DP_CODEC_PRORITY_15,
} BT_A2DP_CODEC_PRORITY_ENUM_T;

typedef enum {
    BT_A2DP_CONN_STATE_DISCONNECTED = 0,
    BT_A2DP_CONN_STATE_CONNECTED,
} bt_a2dp_conn_state_t;

typedef enum {
    BT_A2DP_AUDIO_STATE_CLOSED = 0,
    BT_A2DP_AUDIO_STATE_SUSPEND,
    BT_A2DP_AUDIO_STATE_STARTED,
} bt_a2dp_audio_state_t;

typedef enum {
    BT_A2DP_CODEC_NONE = 0,
    BT_A2DP_CODEC_SBC,
    BT_A2DP_CODEC_AAC,
    BT_A2DP_CODEC_LHDC,
    BT_A2DP_CODEC_LHDCV5,
    BT_A2DP_CODEC_LDAC,
    BT_A2DP_CODEC_SCALABLE,
    BT_A2DP_CODEC_LC3,
    BT_A2DP_CODEC_L2HC,
    BT_A2DP_CODEC_MIHC,
} bt_a2dp_codec_type_t;

typedef enum {
    BT_A2DP_SAMPLE_RATE_NONE = 0,
    BT_A2DP_SAMPLE_RATE_16000,
    BT_A2DP_SAMPLE_RATE_24000,
    BT_A2DP_SAMPLE_RATE_32000,
    BT_A2DP_SAMPLE_RATE_44100,
    BT_A2DP_SAMPLE_RATE_48000,
    BT_A2DP_SAMPLE_RATE_88200,
    BT_A2DP_SAMPLE_RATE_96000,
    BT_A2DP_SAMPLE_RATE_176400,
    BT_A2DP_SAMPLE_RATE_192000,
} bt_a2dp_sample_rate_t;

typedef enum {
    BT_A2DP_FRAME_DURATION_NONE = 0,
    BT_A2DP_FRAME_DURATION_2D5ms,
    BT_A2DP_FRAME_DURATION_5ms,
    BT_A2DP_FRAME_DURATION_7D5ms,
    BT_A2DP_FRAME_DURATION_10ms,
} bt_a2dp_frame_duration_t;

typedef enum {
    BT_A2DP_BITS_PER_SAMPLE_NONE = 0,
    BT_A2DP_BITS_PER_SAMPLE_16,
    BT_A2DP_BITS_PER_SAMPLE_24,
    BT_A2DP_BITS_PER_SAMPLE_32,
} bt_a2dp_bits_per_sample_t;

typedef enum {
    BT_A2DP_CHANNEL_MODE_NONE = 0,
    BT_A2DP_CHANNEL_MODE_MONO,
    BT_A2DP_CHANNEL_MODE_STEREO,
} bt_a2dp_channel_mode_t;

typedef struct {
    bt_a2dp_codec_type_t codec;
    bt_a2dp_sample_rate_t sample_rate;
    bt_a2dp_frame_duration_t frame_duration;
    bt_a2dp_bits_per_sample_t bits_per_sample;
    bt_a2dp_channel_mode_t channel_mode;

    union {
        struct {
            uint8_t version;
            uint8_t llc;
        } lhdc;
        struct {
            uint8_t version;
        } mihc;
        struct {
            uint8_t bitpool;
        } sbc;
    } specific_params;
} bt_a2dp_audio_config_t;
typedef struct {
    uint8_t trans_lable;
    uint8_t cmd_id;
    uint16_t data_len;
    uint8_t *cmd_data;
} bt_a2dp_unknown_cmd_t;

/**
 ****************************************************************************************
 * @brief       Callback function type for handling changes in the a2dp connection state.
 * @param[in]   state: a2dp connection state enum.
 * @return      none
 ****************************************************************************************
 */
typedef void (*bt_a2dp_connection_state_cb)(const bt_bdaddr_t *address, bt_a2dp_conn_state_t state, uint8_t error_code);

/**
 ****************************************************************************************
 * @brief       Callback function type for handling changes in the a2dp audio state.
 * @param[in]   state: a2dp audio stream state enum.
 * @return      none
 ****************************************************************************************
 */
typedef void (*bt_a2dp_audio_state_cb)(const bt_bdaddr_t *address, bt_a2dp_audio_state_t state, uint8_t error_code);

/**
 ****************************************************************************************
 * @brief       Callback function type for handling changes in audio configuration.
 * @param[in]   config: Pointer to a2dp audio stream codec config.
 * @return      none
 ****************************************************************************************
 */
typedef void (*bt_a2dp_audio_config_cb)(const bt_bdaddr_t *address, const bt_a2dp_audio_config_t *config);

typedef void (*bt_a2dp_unknown_cmd_cb)(const bt_bdaddr_t *address, const bt_a2dp_unknown_cmd_t *param);

/**
 ****************************************************************************************
 * @brief       Callback function type for handling changes in audio configuration.
 * @param[in]   address: Remote addr.
 * @param[out]  timeout: Change timeout time.
 * @return      true:allow dalay send rsp
 *              false:disallow dalay send rsp
 ****************************************************************************************
 */
typedef bool (*bt_a2dp_cmd_req_delay_rsp_cb)(const bt_bdaddr_t *address, uint8_t *timeout);

typedef struct
{
    bt_a2dp_connection_state_cb    connection_state_cb;
    bt_a2dp_audio_state_cb         audio_state_cb;
    bt_a2dp_audio_config_cb        audio_config_cb;
    bt_a2dp_unknown_cmd_cb         cmd_req_cb;
} bt_a2dp_sink_callbacks_t;

typedef enum
{
    BT_A2DP_SINK_CB_TYPE_CONNECTION,
    BT_A2DP_SINK_CB_TYPE_AUDIO_STATE,
    BT_A2DP_SINK_CB_TYPE_AUDIO_CONFIG,
    BT_A2DP_SINK_CB_TYPE_UNKNOWN_CMD,
} bt_a2dp_sink_cb_type_t;

typedef enum
{
    BT_A2DP_SINK_CB_USER_BTA = 0,
    BT_A2DP_SINK_CB_USER_APP,
    BT_A2DP_SINK_CB_USER_DEPRECATED,
    BT_A2DP_SINK_CB_USER_GFPS,
    BT_A2DP_SINK_CB_USER_MODULE_1,
    BT_A2DP_SINK_CB_USER_CUSTOMER,
    BT_A2DP_SINK_CB_USER_MAX,
} bt_a2dp_sink_callback_user_t;
