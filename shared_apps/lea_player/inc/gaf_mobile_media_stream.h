/**
 * @copyright Copyright (c) 2015-2021 BES Technic.
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
 */


#ifndef __GAF_MOBILE_MEDIA_STREAM_H__
#define __GAF_MOBILE_MEDIA_STREAM_H__

#ifdef AOB_MOBILE_ENABLED
/*****************************header include********************************/
#include "gaf_media_common.h"
#include "gaf_media_pid.h"

/******************************macro defination*****************************/
#ifdef LEA_FOUR_CIS_ENABLED
#define GAF_MOB_MAXIMUM_CONNECTION_COUNT            (4)
#else
#define GAF_MOB_MAXIMUM_CONNECTION_COUNT            (2)
#endif

#define GAF_MOBILE_AUDIO_STREAM_CAPTURE_CHANNEL_NUM      AUD_CHANNEL_NUM_2

#if defined (BLE_AUDIO_USE_TWO_MIC_SRC_FOR_DONGLE) || defined (BLE_AUDIO_STEREO_CHAN_OVER_CIS_CNT)
#define GAF_MOBILE_AUDIO_STREAM_PLAYBACK_CHANNEL_NUM     AUD_CHANNEL_NUM_2
#else
#define GAF_MOBILE_AUDIO_STREAM_PLAYBACK_CHANNEL_NUM     AUD_CHANNEL_NUM_1
#endif

#ifdef __BLE_AUDIO_24BIT__
#define GAF_MOBILE_AUDIO_STREAM_BIT_NUM            AUD_BITS_24
#else
#define GAF_MOBILE_AUDIO_STREAM_BIT_NUM            AUD_BITS_16
#endif

/*
* Note: Temporary solution on mobile phone
* 1. The timestamp received by the phone is always less than the current bt time.
* 2. Lost packet
*/
#define GAF_MOBILE_TEMP_SOLUTION_ENABLE

#define GAF_MOBILE_AUDIO_MAX_DIFF_BT_TIME   (4000)

/******************************type defination******************************/

/****************************function declaration***************************/
#ifdef __cplusplus
extern "C"{
#endif

typedef enum
{
    GAF_MOBILE_INPUT_UNKONW = 0,
    // Linein audio path
    GAF_MOBILE_INPUT_LINEIN,
    // I2S audio path
    GAF_MOBILE_INPUT_I2S,
    // Analog MIC audio path
    GAF_MOBILE_INPUT_AMIC,
    // Digital MIC audio path
    GAF_MOBILE_INPUT_DMIC,
    // USB audio path
    GAF_MOBILE_INPUT_USB,
    // WIFI audio path
    GAF_MOBILE_INPUT_WIFI,
    // A2DP audio path
    GAF_MOBILE_INPUT_A2DP,
    // SCO audio path
    GAF_MOBILE_INPUT_SCO,
    // SMF audio path
    GAF_MOBILE_INPUT_SMF,
    // SPDIF audio path
    GAF_MOBILE_INPUT_SPDIF,
    // MAX audio path
    GAF_MOBILE_INPUT_MAX,
} GAF_MOBILE_INPUT_TYPE_E;

typedef struct
{
    //see@GAF_MOBILE_INPUT_TYPE_E
    uint8_t  audio_path;
    //0: cis, 1: bis
    uint8_t  stream_type;
    //Playback param
    uint32_t play_sample_rate;
    uint32_t play_frame_size;
    uint8_t  play_ch_num;
    uint8_t  play_bits;
    // Capture param
    uint32_t cap_sample_rate;
    uint32_t cap_frame_size;
    uint8_t  cap_ch_num;
    uint8_t  cap_bits;
} gaf_mobile_audio_param_t;


void gaf_mobile_audio_stream_update_and_start_handler(uint8_t ase_lid, uint8_t con_lid);
void gaf_mobile_audio_stream_update_and_stop_handler(uint8_t ase_lid, uint8_t con_lid);

void gaf_mobile_audio_clear_playback_buf_list(GAF_AUDIO_STREAM_ENV_T* _pStreamEnv);
void gaf_mobile_audio_stream_init(void);
void gaf_mobile_audio_receive_data(uint16_t conhdl);
uint8_t gaf_mobile_audio_get_enabled_capture_ase_count(void);
uint8_t gaf_mobile_audio_get_enabled_playback_ase_count(void);

void gaf_mobile_audio_param_set(gaf_mobile_audio_param_t* param);
void gaf_mobile_audio_data_write(uint8_t* data, uint32_t len);
void gaf_mobile_audio_data_read(uint8_t* data, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif
#endif /* #ifndef __GAF_MOBILE_MEDIA_STREAM_H__ */
