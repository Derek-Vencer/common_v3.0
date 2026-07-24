/**
 * @brief Bluetooth Service Low Energy Audio Broadcast Source Application Programming Interface
 *
 * @copyright Copyright (c) 2015-20223 BES Technic.
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
#ifndef __BT_SVC_LEA_BC_SOURCE_API_H__
#define __BT_SVC_LEA_BC_SOURCE_API_H__

/*****************************header include********************************/
#include <stdint.h>

/******************************macro defination*****************************/
#define BT_SVC_LEA_BC_SRC_VOLUME_INVALID       0xFF

#define BT_SVC_LEA_BC_SET_INPUT_MODE(input_mode, play_mode, tran_mode) do { \
        input_mode = (play_mode<<5) | tran_mode; \
    } while(0);

#define BT_SVC_LEA_BC_GET_INPUT_MODE(input_mode, play_mode, tran_mode) do { \
        tran_mode = input_mode & 0x1F; \
        play_mode = input_mode>>5; \
    } while(0);

/******************************type defination******************************/
typedef enum
{
    // Module closed state
    BT_SVC_LEA_BC_SRC_STATE_CLOSE = 0,
    // Module opened state
    BT_SVC_LEA_BC_SRC_STATE_OPENED,
    // Module stream state
    BT_SVC_LEA_BC_SRC_STATE_STREAM_STARTED,
    // Module unknow state
    BT_SVC_LEA_BC_SRC_STATE_UNKNOWN,
} BT_SVC_LEA_BC_SRC_STATE_E;

typedef enum
{
    // Absolute volume
    BT_SVC_LEA_BC_SRC_VOL_ABS = 0,
    // Up volume
    BT_SVC_LEA_BC_SRC_VOL_UP,
    // Down volume
    BT_SVC_LEA_BC_SRC_VOL_DOWN,
} BT_SVC_LEA_BC_SRC_VOLUME_E;

typedef enum
{
    // One audio channel plays synchronously locally and remotely
    BT_SVC_LEA_BC_SRC_PLAY_MONO=0,
    // Two audio channels, one channel plays locally and the other
    // channel plays synchronously on remote devices
    BT_SVC_LEA_BC_SRC_PLAY_STEREO_TWO_DEV,
    // Two audio channels are placed in one stream and played
    // synchronously on both local and remote devices
    BT_SVC_LEA_BC_SRC_PLAY_STEREO_ONE_DEV_ONE_ISO,
    // Two audio channels placed in two streams,
    // playing synchronously on local and remote devices
    BT_SVC_LEA_BC_SRC_PLAY_STEREO_ONE_DEV_TWO_ISO,
    //One audio channel plays onlu remote
    BT_SVC_LEA_BC_SRC_PLAY_MONO_ONLY_REMOTE,
    // Two audio channels are placed in one stream and played
    // only on remote devices
    BT_SVC_LEA_BC_SRC_PLAY_STEREO_ONLY_REMOTE_ONE_ISO,
    // Two audio channels placed in two iso streams,
    // only on remote devices
    BT_SVC_LEA_BC_SRC_PLAY_STEREO_ONLY_REMOTE_TWO_ISO,
    // Unkonw mode
    BT_SVC_LEA_BC_SRC_PLAY_UNKONW,
} BT_SVC_LEA_BC_SRC_PLAY_MODE_E;

typedef enum
{
    // External module calls interface to send data in
    BT_SVC_LEA_BC_SRC_USE_SEND=0,
    // Calling the callback function registered with an external module to read data
    BT_SVC_LEA_BC_SRC_SVC_READ,
} BT_SVC_LEA_BC_SRC_DATA_TANS_MODE_E;

/// function return error code
typedef enum
{
    // Only play locally
    BT_SVC_LEA_BC_SRC_ONLY_LOCAL = 0,
    // Only play remote
    BT_SVC_LEA_BC_SRC_ONLY_REMOTE,
    // Synchronize playback between local and remote devices
    BT_SVC_LEA_BC_SRC_LOCAL_AND_REMOTE,
} BT_SVC_LEA_BC_SRC_AUDIO_PLAYBACK_DEVICE_E;

typedef enum
{
    // Stream start event, param: NULL
    BT_SVC_LEA_BC_SRC_EVENT_STREAM_START,
    // Stream stop event, param: NULL
    BT_SVC_LEA_BC_SRC_EVENT_STREAM_STOP,
    // Unkonw event
    APP_BIS_TRAN_EVENT_UNKNOWN,
} BT_SVC_LEA_BC_SRC_EVENT_E;

/// volume sync@TGT_VOLUME_LEVEL_T
typedef enum {
    // Volume 0, is mute
    BT_SVC_LEA_BC_SRC_VOLUME_LEVEL_MUTE = 0,
    // Volume 1
    BT_SVC_LEA_BC_SRC_VOLUME_LEVEL_1,
    // Volume 2
    BT_SVC_LEA_BC_SRC_VOLUME_LEVEL_2,
    // Volume 3
    BT_SVC_LEA_BC_SRC_VOLUME_LEVEL_3,
    // Volume 4
    BT_SVC_LEA_BC_SRC_VOLUME_LEVEL_4,
    // Volume 5
    BT_SVC_LEA_BC_SRC_VOLUME_LEVEL_5,
    // Volume 6
    BT_SVC_LEA_BC_SRC_VOLUME_LEVEL_6,
    // Volume 7
    BT_SVC_LEA_BC_SRC_VOLUME_LEVEL_7,
    // Volume 8
    BT_SVC_LEA_BC_SRC_VOLUME_LEVEL_8,
    // Volume 9
    BT_SVC_LEA_BC_SRC_VOLUME_LEVEL_9,
    // Volume 10
    BT_SVC_LEA_BC_SRC_VOLUME_LEVEL_10,
    // Volume 11
    BT_SVC_LEA_BC_SRC_VOLUME_LEVEL_11,
    // Volume 12
    BT_SVC_LEA_BC_SRC_VOLUME_LEVEL_12,
    // Volume 13
    BT_SVC_LEA_BC_SRC_VOLUME_LEVEL_13,
    // Volume 14
    BT_SVC_LEA_BC_SRC_VOLUME_LEVEL_14,
    // Volume 15
    BT_SVC_LEA_BC_SRC_VOLUME_LEVEL_15,
    // Volume max
    BT_SVC_LEA_BC_SRC_VOLUME_LEVEL_MAX,
    // Volume level quantity
    BT_SVC_LEA_BC_SRC_VOLUME_LEVEL_QTY
}BT_SVC_LEA_BC_SRC_VOLUME_LEVEL_E;

typedef struct
{
    BT_SVC_LEA_BC_SRC_EVENT_E event_type;
} bt_svc_lea_bc_src_event_t;

typedef struct
{
    // Input source type, see@BT_SVC_DATA_PATH_TYPE_E
    uint8_t input_type;
    // Input mode, see@, see@BT_SVC_LEA_BC_SRC_PLAY_MODE_E, BT_SVC_LEA_BC_SRC_TRAN_MODE_E
    // | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
    // | paly mode |   transport mode  |
    uint8_t input_mode;
    // Broadcast ID
    uint8_t broadcast_id[3];
    // Enabling broadcast encryption function
    uint8_t encrypt_enable;
    // Broadcast encryption key
    uint8_t encrypt_key[16];
    // Synchronize volume, 0:disenable, 1:enable
    uint8_t sync_volume;
    // Private Data length of Extended  Advertising Manufacturers
    uint8_t adv_mfrs_data_len;
    // Private Data of Extended  Advertising Manufacturers
    uint8_t *adv_mfrs_data;
    // Company ID
    uint16_t company_id;
    // Company name
    const char *bcast_name;
    // Callback function for event notification
    void (*event_callback)(bt_svc_lea_bc_src_event_t* event);
} bt_svc_lea_bc_src_param_t;

typedef struct
{
    /// bis transport module state 
    uint8_t state;
    /// bis transport module input_type
    uint8_t input_type;
} bt_svc_lea_bc_src_get_info_t;


/****************************function declearation**************************/
/**
 ****************************************************************************************
 * @brief Open bis source transport function
 * @param[in]  : param_inf, Open parameter information, see@bt_svc_lea_bc_src_param_t
 * @param[out] : none
 * @return: error code, see@BT_SVC_ERROR_CODE_E
 ****************************************************************************************
 */
int bt_svc_lea_bc_src_open(bt_svc_lea_bc_src_param_t *param_info);

/**
 ****************************************************************************************
 * @brief Close bis source transport function
 * @param[in]  : none
 * @param[out] : none
 * @return: error code, see@BT_SVC_ERROR_CODE_E
 ****************************************************************************************
 */
int bt_svc_lea_bc_src_close(void);

/**
 ****************************************************************************************
 * @brief Get bis transport module info function
 * @param[in]  : info, bis transport info ptr
 * @param[out] : none
 * @return: error code, see@BT_SVC_ERROR_CODE_E
 ****************************************************************************************
 */
int bt_svc_lea_bc_src_get_info(bt_svc_lea_bc_src_get_info_t *info);

/**
 ****************************************************************************************
 * @brief Set bis stream state
 * @param[in]  : operate, false:stop stream, true:start stream
 * @param[out] : none
 * @return: error code, see@BT_SVC_ERROR_CODE_E
 ****************************************************************************************
 */
int bt_svc_lea_bc_src_stream(bool operate);

/**
 ****************************************************************************************
 * @brief Local playback prompt sound in bis source state function
 * @param[in]  : prompt_id, Prompt tone type, see@AUD_ID_ENUM
 * @param[out] : none
 * @return: error code, see@BT_SVC_ERROR_CODE_E
 ****************************************************************************************
 */
int bt_svc_lea_bc_src_play_prompt(uint8_t prompt_id);


#endif //__BT_SVC_LEA_API_H__
