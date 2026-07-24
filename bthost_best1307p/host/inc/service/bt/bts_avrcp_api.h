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

#ifndef __BTS_AVRCP_API_H__
#define __BTS_AVRCP_API_H__
#include "bt_avrcp_types.h"
#include "bts_bt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 ****************************************************************************************
 * @brief       Callback for TG received get play status command from CT
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 ****************************************************************************************
 */
typedef void (*bts_avrcp_recv_get_play_status_cmd_cb)(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Callback for TG received register notification command from CT.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   event_id: register notification event.
 * @param[in]   interval: notification interval.
 ****************************************************************************************
 */
typedef void (*bts_avrcp_recv_register_notification_cmd_cb)(const bt_bdaddr_t *address, bt_avrcp_event_id_t event_id, uint32_t interval);

/**
 ****************************************************************************************
 * @brief       Register avrcp CT/TG event callbacks
 * @param[in]   user: Indicate who registered
 * @param[in]   callbacks: Pointer to callbacks for handling CT/TG events.
 * @return      void
 ****************************************************************************************
 */
void bts_avrcp_register_callbacks(bt_avrcp_callback_user_t user, bt_avrcp_callbacks_t *callbacks);

/**
 ****************************************************************************************
 * @brief       Deregister avrcp CT/TG event callbacks.
 * @param[in]   user: Indicate who registered.
 * @return      void
 ****************************************************************************************
 */
void bts_avrcp_deregister_callbacks(bt_avrcp_callback_user_t user);

/**
 ****************************************************************************************
 * @brief       Initiate connection.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      Status of the operation.
 ****************************************************************************************
 */
bts_status_t bts_avrcp_connect(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Initiate disconnection.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      Status of the operation.
 ****************************************************************************************
 */
bts_status_t bts_avrcp_disconnect(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Determine whether the avrcp is connected.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      status of the operation
 ****************************************************************************************
 */
bool bts_avrcp_is_connected(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Determine whether the current connection is initiated actively.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      status of the operation
 ****************************************************************************************
 */
bool bts_avrcp_is_initiator(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 *       ___     ______   ____ ____        ____ _____
 *      / \ \   / /  _ \ / ___|  _ \      / ___|_   _|
 *     / _ \ \ / /| |_) | |   | |_) |    | |     | |
 *    / ___ \ V / |  _ <| |___|  __/     | |___  | |
 *   /_/   \_\_/  |_| \_\\____|_|         \____| |_|
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief       CT send passthrough command
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   key: passthrough key code
 * @param[in]   is_press: key is press state or release state
 * @return      Status of the operation
 ****************************************************************************************
 */
bts_status_t bts_avrcp_ct_send_passthrough_cmd(const bt_bdaddr_t *address, bt_avrcp_key_t key, bool is_press);

/**
 ****************************************************************************************
 * @brief       CT send get capabilities command
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   capability_id
 * @return      Status of the operation
 ****************************************************************************************
 */
bts_status_t bts_avrcp_ct_get_capabilities(const bt_bdaddr_t *address, bt_avrcp_capability_t capability_id);

/**
 ****************************************************************************************
 * @brief       CT send get element attributes command
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   attr_mask
 * @return      Status of the operation
 ****************************************************************************************
 */
bts_status_t bts_avrcp_ct_get_element_attributes(const bt_bdaddr_t *address, bts_avrcp_media_attr_id_mask_t attr_mask);

/**
 ****************************************************************************************
 * @brief       CT send get play status command
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      Status of the operation
 ****************************************************************************************
 */
bts_status_t bts_avrcp_ct_get_play_status(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       CT send register notification command
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   event_id: Register notification event
 * @param[in]   interval: Specifies the time interval (in seconds) at which the change in playback position will be notified.
                - Applicable only for EventID EVENT_PLAYBACK_POS_CHANGED. For other events, value of this parameter is ignored.
 * @return      Status of the operation
 ****************************************************************************************
 */
bts_status_t bts_avrcp_ct_register_notification(const bt_bdaddr_t *address, bt_avrcp_event_id_t event_id, uint32_t interval);

/**
 ****************************************************************************************
 * @brief       CT send set addressed playern command
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   player_id: Unique media mlayer id.
 * @return      Status of the operation
 ****************************************************************************************
 */
bts_status_t bts_avrcp_ct_set_addressed_player(const bt_bdaddr_t *address, uint16_t player_id);

/**
 ****************************************************************************************
 * @brief       CT send set browsed playern command
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   player_id: Unique media mlayer id.
 * @return      Status of the operation
 ****************************************************************************************
 */
bts_status_t bts_avrcp_ct_set_browsed_player(const bt_bdaddr_t *address, uint16_t player_id);

/**
 ****************************************************************************************
 * @brief       CT send set change path command
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   params: change path command params
 * @return      Status of the operation
 ****************************************************************************************
 */
bts_status_t bts_avrcp_ct_change_path(const bt_bdaddr_t *address, const bts_avrcp_change_path_params_t *params);

/**
 ****************************************************************************************
 * @brief       CT send get folder items command
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   params: get folder items command params
 * @return      Status of the operation
 ****************************************************************************************
 */
bts_status_t bts_avrcp_ct_get_folder_items(const bt_bdaddr_t *address, const bts_avrcp_get_folder_items_params_t *params);

/**
 ****************************************************************************************
 * @brief       CT send get item attributes command
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   params:
 * @return      Status of the operation
 ***************************************************************************************
 */
bts_status_t bts_avrcp_ct_get_item_attributes(const bt_bdaddr_t *address, const bts_avrcp_get_item_attrs_params_t *params);

/**
 ****************************************************************************************
 * @brief       CT send get total number of items command
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   scope:
 * @return      Status of the operation
 ****************************************************************************************
 */
bts_status_t bts_avrcp_ct_get_total_number_of_items(const bt_bdaddr_t *address, bts_avrcp_scope_t scope);

/**
 ****************************************************************************************
 * @brief       CT send search command
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   str: The value of UTF-8
 * @param[in]   len: Length of the search string
 * @return      Status of the operation
 ****************************************************************************************
 */
bts_status_t bts_avrcp_ct_search(const bt_bdaddr_t *address, const char *str, uint16_t len);

/**
 ****************************************************************************************
 * @brief       CT send play item command
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   scope: The scope in which the UID of the media element item or folder item
 * @param[in]   uid: The UID of the media element item or folder item
 * @param[in]   uid_counter:
 * @return      Status of the operation
 ****************************************************************************************
 */
bts_status_t bts_avrcp_ct_play_item(const bt_bdaddr_t *address, bts_avrcp_scope_t scope, uint8_t uid, uint16_t uid_counter);

/**
 ****************************************************************************************
 * @brief       CT send add to now playing command
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   scope: The scope in which the UID of the media element item or folder item
 * @param[in]   uid: The UID of the media element item or folder item
 * @param[in]   uid_counter:
 * @return      Status of the operation
 ****************************************************************************************
 */
bts_status_t bts_avrcp_ct_add_to_now_playing(const bt_bdaddr_t *address, bts_avrcp_scope_t scope, uint8_t uid, uint16_t uid_counter);

/**
 ****************************************************************************************
 * @brief       CT send next group command
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   scope:
 * @return      Status of the operation
 ****************************************************************************************
 */
bts_status_t bts_avrcp_ct_next_group(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       CT send previous group command.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   scope:
 * @return      Status of the operation.
 ****************************************************************************************
 */
bts_status_t bts_avrcp_ct_previous_group(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       CT get image properties from TG, corresponding done event will report.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      Status of the operation.
 ****************************************************************************************
 */
bts_status_t bts_avrcp_send_cover_art_get_image_properties(const bt_bdaddr_t *address, const char *image_handle);

/**
 ****************************************************************************************
 * @brief       CT get image from TG, corresponding done event will report.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      Status of the operation.
 ****************************************************************************************
 */
bts_status_t bts_avrcp_send_cover_art_get_image(const bt_bdaddr_t *address, const char *image_handle, const char *descriptor, uint16_t descriptor_len);

/**
 ****************************************************************************************
 * @brief       CT get linked thumbnail from TG, corresponding done event will report.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      Status of the operation.
 ****************************************************************************************
 */
bts_status_t bts_avrcp_send_cover_art_get_linked_thumbnail(const bt_bdaddr_t *address, const char *image_handle);

/**
 ****************************************************************************************
 * @brief       CT send set absolute volume command
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   volume:
 * @return      Status of the operation
 ****************************************************************************************
 */
bts_status_t bts_avrcp_ct_set_absolute_volume(const bt_bdaddr_t *address, uint8_t volume);

/**
 ****************************************************************************************
 * @brief       Send media play command.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 ****************************************************************************************
 */
bts_status_t bts_avrcp_ct_send_play(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Send media pause command.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 ****************************************************************************************
 */
bts_status_t bts_avrcp_ct_send_pause(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Send media forward command.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 ****************************************************************************************
 */
bts_status_t bts_avrcp_ct_send_forward(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Send media backward command.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 ****************************************************************************************
 */
bts_status_t bts_avrcp_ct_send_backward(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Send media volume up command.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 ****************************************************************************************
 */
bts_status_t bts_avrcp_ct_send_volume_up(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Send media volume down command.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 ****************************************************************************************
 */
bts_status_t bts_avrcp_ct_send_volume_down(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 *       ___     ______   ____ ____       _____ ____
 *      / \ \   / /  _ \ / ___|  _ \     |_   _/ ___|
 *     / _ \ \ / /| |_) | |   | |_) |      | || |  _
 *    / ___ \ V / |  _ <| |___|  __/       | || |_| |
 *   /_/   \_\_/  |_| \_\\____|_|          |_| \____|
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief       TG send register notification response
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   params: different params for each register notification event
 * @return      Status of the operation
 ****************************************************************************************
 */
bts_status_t bts_avrcp_tg_send_register_notification_rsp(const bt_bdaddr_t *address, const bt_avrcp_notification_params_t *params);

bts_status_t bts_avrcp_tg_send_get_total_number_of_items_rsp(const bt_bdaddr_t *address, bts_avrcp_status_code_t status, uint16_t uid_counter, uint32_t num_items);

bts_status_t bts_avrcp_tg_send_search_rsp(const bt_bdaddr_t *address, bts_avrcp_status_code_t status, uint16_t uid_counter, uint32_t num_items);

bts_status_t bts_avrcp_tg_send_play_item_rsp(const bt_bdaddr_t *address, bts_avrcp_status_code_t status);

bts_status_t bts_avrcp_tg_send_add_to_now_playing_rsp(const bt_bdaddr_t *address, bts_avrcp_status_code_t status);

bts_status_t bts_avrcp_tg_send_playback_status_changed(const bt_bdaddr_t *address, bt_avrcp_playback_status_t status);

#ifdef __cplusplus
}
#endif

#endif /* __BTS_AVRCP_API_H__ */