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
    BT_A2DP_CODEC_OPUS,
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
