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
#ifndef __BTS_BT_SRC_IF_H__
#define __BTS_BT_SRC_IF_H__

/****************************** header include ********************************/
#include <stdint.h>

/***************************** external declaration *****************************/

/***************************** macro defination *******************************/

/*****************************  type defination ********************************/
typedef enum
{
    BTS_BT_SRC_PRF_A2DP  = 0X01,
    BTS_BT_SRC_PRF_AVRCP = 0X02,
    BTS_BT_SRC_PRF_HFP   = 0X04,
} BTS_SRC_PROFILE_BIT;

typedef enum
{
    BTS_BT_SRC_EVENT_SCAN_REPORT = 0,
    BTS_BT_SRC_EVENT_ACL_CONN,
    BTS_BT_SRC_EVENT_ACL_DISCONN,
    BTS_BT_SRC_EVENT_STREAM_START,
    BTS_BT_SRC_EVENT_STREAM_STOP,
    BTS_BT_SRC_EVENT_MAX,
} BTS_SRC_EVENT_E;

typedef enum
{
    BTS_BT_SRC_DIRECTION_TX    = 1,
    BTS_BT_SRC_DIRECTION_RX    = 2,
    BTS_BT_SRC_DIRECTION_TX_RX = 3,
    BTS_BT_SRC_DIRECTION_MAX,
} BTS_SRC_STREAM_DIRECTION_E;

typedef struct
{
    uint8_t*    addr;
    char*       name;
    int8_t      rssi;
    uint32_t    class_of_device;
} bts_bt_src_event_scan_t;

typedef struct
{
    // connect index
    uint8_t con_idx;
    // see@BT_SVC_ERROR_CODE_E
    int status;
    // address
    uint8_t addr[6];
} bts_bt_src_event_acl_t;

typedef struct
{
    // connect index
    uint8_t con_idx;
    // Event status,see@BT_SVC_ERROR_CODE_E
    int     status;
    // Out stream type@BT_SVC_DATA_PATH_TYPE_E
    uint8_t out_type;
} bts_bt_src_event_stream_t;

typedef struct
{
    // event type, see@BTS_SRC_EVENT_E
    uint8_t event_type;
    union
    {
        bts_bt_src_event_scan_t   scan_report;
        bts_bt_src_event_acl_t    conn;
        bts_bt_src_event_acl_t    disconn;
        bts_bt_src_event_stream_t stream_start;
        bts_bt_src_event_stream_t stream_stop;
    } param;
} bts_bt_src_event_t;

typedef struct
{
    uint32_t sample_rate;
    uint32_t frame_size;
    uint8_t  ch_num;
    uint8_t  bits_byte;
} bts_bt_src_audio_param_t;

typedef struct
{
    bts_bt_src_audio_param_t playback;
    bts_bt_src_audio_param_t capture;
}bts_bt_src_input_param_t;

typedef struct
{
    // Input source, see@BT_SVC_AUDIO_PATH_TYPE_E
    uint8_t  input_type;
    // Support profile, see@BTS_SRC_PROFILE_BIT
    uint8_t  profile_map;
    // When capturing audio switch, Automatic switching A2DP <---> SCO
    bool     auto_switch;
    // Audio input parameter
    bts_bt_src_input_param_t input_param;
    // Event report
    void (*event_callback)(bts_bt_src_event_t* param);
} bts_bt_src_open_t;

typedef struct
{
    uint32_t playback_sample_rate;
    uint8_t  playback_ch_num;
    uint8_t  playback_bits_byte;
    uint32_t playback_frame_size;
    uint32_t capture_sample_rate;
    uint8_t  capture_ch_num;
    uint8_t  capture_bits_byte;
    uint32_t capture_frame_size;
} bts_bt_src_stream_param_t;

typedef struct
{
    int (*open)(void);
    int (*close)(void);
    int (*stream_start)(uint8_t* dev_addr, bts_bt_src_stream_param_t* param);
    int (*stream_stop)(void);
    int (*stream_vol_set)(uint8_t direction, int level);
    int (*data_send)(uint8_t* data, uint32_t data_len);
    uint32_t (*data_get)(uint8_t* data, uint32_t data_len);
} bts_bt_src_output_func_t;

typedef struct
{
    int (*open)(uint8_t input_type);
    int (*close)(void);
    int (*stream_set)(bool operate, uint8_t directiom);
} bts_bt_src_input_func_t;

/*****************************  variable defination *****************************/

/*****************************  function declaration ****************************/

/**
 ***************************************************************************
 * @brief BT scan start
 * @param[in] : none
 * @param[out] : none
 * @return: error code, see@bt_status_t
 ***************************************************************************
 */
int bts_bt_src_scan_start(void);

int bts_bt_src_scan_stop(void);

int bts_bt_src_conn(uint8_t* addr, uint32_t timeout_ms);

int bts_bt_src_disconn(uint8_t* addr);

int bts_bt_src_open(bts_bt_src_open_t* param);

int bts_bt_src_close(void);

int bts_bt_src_stream(bool operate, uint8_t directiom);

int bts_bt_src_stream_start(uint8_t direction, bts_bt_src_stream_param_t* param);
int bts_bt_src_stream_stop(uint8_t direction);
int bts_bt_src_vol_set(uint8_t direction, int level);
int bts_bt_src_data_send(uint8_t* data, uint32_t data_len);
int bts_bt_src_data_get(uint8_t* data, uint32_t data_len);

void bts_bt_src_input_af_get(bts_bt_src_input_func_t** input);
void bts_bt_src_input_usb_get(bts_bt_src_input_func_t** input);

void bts_bt_src_a2dp_func_get(bts_bt_src_output_func_t* func);
void bts_bt_src_ag_func_get(bts_bt_src_output_func_t* func);

#endif // __BTS_BT_SRC_IF_H__
