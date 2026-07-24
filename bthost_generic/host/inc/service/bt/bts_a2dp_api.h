/****************************************************************************
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

#ifndef __BTS_A2DP_API_H__
#define __BTS_A2DP_API_H__
#include "bt_a2dp_types.h"
#include "bts_bt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 ****************************************************************************************
 *       _    ____  ____  ____       ____ ___ _   _ _  __
 *      / \  |___ \|  _ \|  _ \     / ___|_ _| \ | | |/ /
 *     / _ \   __) | | | | |_) |    \___ \| ||  \| | ' /
 *    / ___ \ / __/| |_| |  __/      ___) | || |\  | . \
 *   /_/   \_\_____|____/|_|        |____/___|_| \_|_|\_\
 *
****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief       Register the a2dp sink callbacks.
 * @param[in]   user: Indicate who registered.
 * @param[in]   callbacks: Pointer to callbacks for handling a2dp sink events.
 * @return      none
 ****************************************************************************************
 */
void bts_a2dp_sink_register_callbacks(bt_a2dp_sink_callback_user_t user, bt_a2dp_sink_callbacks_t *callbacks);

/**
 ****************************************************************************************
 * @brief       Register the a2dp aink callbacks.
 * @param[in]   user: Indicate who registered.
 * @return      none
 ****************************************************************************************
 */
void bts_a2dp_sink_deregister_callbacks(bt_a2dp_sink_callback_user_t user);

/**
 ****************************************************************************************
 * @brief       Register the a2dp revied cmd request delay send response callback.
 * @param[in]   req_delay_cb: Pointer to callbacks for handling a2dp sink events.
 ****************************************************************************************
 */
void bts_a2dp_register_req_delay_rsp_callback(bt_a2dp_cmd_req_delay_rsp_cb req_delay_cb);

/**
 ****************************************************************************************
 * @brief       Initiate a connection to a2dp source.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_a2dp_sink_connect(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Initiate a disconnection to a2dp source.
 * @param[in]   address: Pointer to the Bluetooth address of peer device
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_a2dp_sink_disconnect(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Determine whether the a2dp sink is connected.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      Whether it is the connected.
 ****************************************************************************************
 */
bool bts_a2dp_sink_is_connected(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Determine whether the current connection is initiated actively.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      Whether it is the initiator.
 ****************************************************************************************
 */
bool bts_a2dp_sink_is_initiator(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Determine whether the current is in streaming state.
 * @param[in]   address: Pointer to the Bluetooth address of peer device
 * @return      Whether it is the streaming.
 ****************************************************************************************
 */
bool bts_a2dp_sink_is_streaming(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Send delay report by a2dp sink.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   delay_ms: Time of delay.
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_a2dp_sink_delay_report(const bt_bdaddr_t *address, uint16_t delay_ms);

/**
 ****************************************************************************************
 * @brief       Enable/Disable local a2dp codec.
 * @param[in]   codec: Codec type enum
 * @param[in]   enable: whether to enable the current codec
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_a2dp_sink_control_local_codec(bt_a2dp_codec_type_t codec, bool enable);

/**
 ****************************************************************************************
 * @brief       The a2dp sink initiates the process of switching codecs.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   codec: codec type enum
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_a2dp_sink_switch_codec(const bt_bdaddr_t *address, bt_a2dp_codec_type_t codec);

/**
 ****************************************************************************************
 * @brief       Check whether peer device supports this codec.
 *               - Needs to wait until discover ACP process completed.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   codec: codec type enum.
 * @return      status of the operation
 ****************************************************************************************
 */
bool bts_a2dp_sink_remote_codec_support(const bt_bdaddr_t *address, bt_a2dp_codec_type_t codec);

/**
 ****************************************************************************************
 * @brief       Get current a2dp audio codec.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      codec type enum
 ****************************************************************************************
 */
bt_a2dp_codec_type_t bts_a2dp_sink_get_current_codec(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Get current a2dp audio config.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      audio config
 ****************************************************************************************
 */
bts_status_t bts_a2dp_sink_get_audio_config(const bt_bdaddr_t *address, bt_a2dp_audio_config_t *config);

bool bts_a2dp_sink_get_codec_elements(const bt_bdaddr_t *address, const uint8_t **elements, uint8_t *len);

/**
 ****************************************************************************************
 * @brief       Report current a2dp volume through avrcp volume changed notify.
 * @note        This function only report streaming device volume.
 * @return      none
 ****************************************************************************************
 */
void bts_a2dp_sink_report_speaker_gain(void);

/**
 ****************************************************************************************
 * @brief       Get current a2dp audio config.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   local_volume: Pointer to the Bluetooth address of peer device.
 * @return      none
 ****************************************************************************************
 */
void bts_a2dp_sink_set_local_volume(const bt_bdaddr_t *address, uint8_t local_volume);

void bts_a2dp_sink_set_bt_volume(const bt_bdaddr_t *address, uint8_t bt_volume);

uint8_t bts_a2dp_sink_get_local_volume(const bt_bdaddr_t *address);

uint8_t bts_a2dp_sink_get_bt_volume(const bt_bdaddr_t *address);

uint8_t bts_a2dp_sink_convert_bt_volume_to_local_volume(uint8_t bt_volume);

uint8_t bts_a2dp_sink_convert_local_volume_to_bt_volume(uint8_t local_volume);

bool bts_a2dp_sink_remote_is_support_delay_report(const bt_bdaddr_t *address);

bts_status_t bts_a2dp_sink_accept_unknown_cmd(const bt_bdaddr_t *bd_addr, const bt_a2dp_unknown_cmd_t *cmd, bool accept);

int bts_a2dp_codec_init(uint8_t codectype, uint8_t nonetype, uint8_t sep_priority, uint8_t *elements, uint8_t len);

/**
 ****************************************************************************************
 *       _    ____  ____  ____       ____   ___  _   _ ____   ____ _____
 *      / \  |___ \|  _ \|  _ \     / ___| / _ \| | | |  _ \ / ___| ____|
 *     / _ \   __) | | | | |_) |    \___ \| | | | | | | |_) | |   |  _|
 *    / ___ \ / __/| |_| |  __/      ___) | |_| | |_| |  _ <| |___| |___
 *   /_/   \_\_____|____/|_|        |____/ \___/ \___/|_| \_\\____|_____|
 *
 ****************************************************************************************
 */

typedef struct {
    bt_a2dp_connection_state_cb connection_state_cb;
    bt_a2dp_audio_state_cb      audio_state_cb;
    bt_a2dp_audio_config_cb     audio_config_cb;
} bts_a2dp_src_callbacks_t;

typedef enum {
    BTS_A2DP_SRC_CB_USER_APP = 0,
    BTS_A2DP_SRC_CB_USER_MAX,
} BTS_A2DP_SRC_CALLBACK_USER_E;

/**
 ****************************************************************************************
 * @brief       Register the a2dp source callbacks.
 * @param[in]   user: Indicate who registered.
 * @param[in]   callbacks: Pointer to callbacks for handling a2dp source events
 * @return      none
 ****************************************************************************************
 */
void bts_a2dp_src_register_callbacks(BTS_A2DP_SRC_CALLBACK_USER_E user, bts_a2dp_src_callbacks_t *callbacks);

/**
 ****************************************************************************************
 * @brief       Register the a2dp source callbacks.
 * @param[in]   user: Indicate who registered.
 * @return      none
 ****************************************************************************************
 */
void bts_a2dp_src_deregister_callbacks(BTS_A2DP_SRC_CALLBACK_USER_E user);

/**
 ****************************************************************************************
 * @brief       Initiate a connection to a2dp sink.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_a2dp_src_connect(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Initiate a disconnection to a2dp sink.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_a2dp_src_disconnect(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Start a2dp source stream.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_a2dp_src_stream_start(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Suspend a2dp source stream.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_a2dp_src_stream_suspend(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Toggle a2dp source stream.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      status of the operation
 ****************************************************************************************
 */
bts_status_t bts_a2dp_src_stream_toggle(const bt_bdaddr_t *address);

#ifdef __cplusplus
}
#endif

#endif /* __BTS_A2DP_API_H__ */