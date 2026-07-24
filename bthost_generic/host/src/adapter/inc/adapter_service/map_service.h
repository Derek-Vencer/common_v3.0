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
#ifndef __BT_MAP_SERVICE_H__
#define __BT_MAP_SERVICE_H__
#include "adapter_service.h"
#include "map_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BT_MAP_SUPPORT

typedef enum {
    BT_MAP_EVENT_OPENED = BT_EVENT_MAP_OPENED,
    BT_MAP_EVENT_CLOSED,
    BT_MAP_EVENT_FOLDER_LISTING_RSP,
    BT_MAP_EVENT_MSG_LISTING_ITEM_RSP,
    BT_MAP_EVENT_GET_MSG_RSP,
    BT_MAP_EVENT_READ_STATUS_CHANGED,
    BT_MAP_EVENT_END,
} bt_map_event_t;

#if BT_MAP_EVENT_END != BT_EVENT_MAP_END
#error "bt_map_event_t error define"
#endif

typedef int (*bt_map_callback_t)(const bt_bdaddr_t *bd_addr, bt_map_event_t event, bt_map_callback_param_t param);

bt_status_t bt_map_init(bt_map_callback_t callback);
bt_status_t bt_map_cleanup(void);
bt_status_t bt_map_connect(const bt_bdaddr_t *bd_addr);
bt_status_t bt_map_disconnect(const bt_bdaddr_t *bd_addr);
bt_status_t bt_map_get_folder_listing(const bt_bdaddr_t *bd_addr);
bt_status_t bt_map_enter_to_root_folder(const bt_bdaddr_t *bd_addr);
bt_status_t bt_map_enter_to_parent_folder(const bt_bdaddr_t *bd_addr);
bt_status_t bt_map_enter_to_msg_folder(const bt_bdaddr_t *bd_addr);
bt_status_t bt_map_enter_to_child_folder(const bt_bdaddr_t *bd_addr, const char *folder);
bt_status_t bt_map_send_sms(const bt_bdaddr_t *bd_addr, struct bt_map_sms_t *sms);
bt_status_t bt_map_notify_register(const bt_bdaddr_t *bd_addr, bool enable);
bt_status_t bt_map_get_msg_listing(const bt_bdaddr_t *bd_addr);
bt_status_t bt_map_get_unread_message(const bt_bdaddr_t *bd_addr);
bt_status_t bt_map_get_message(const bt_bdaddr_t *bd_addr, uint64_t handle);
bt_status_t bt_map_set_message_status(const bt_bdaddr_t *bd_addr, uint64_t handle, bt_map_msg_status_t status);
int bt_map_is_connected(const bt_bdaddr_t *bd_addr);

#if defined(IBRT)
uint32_t btif_map_profile_save_ctx(const bt_bdaddr_t *remote, uint8_t *buf, uint32_t buf_len);
uint32_t btif_map_profile_restore_ctx(const bt_bdaddr_t *bdaddr_p, uint8_t *buf, uint32_t buf_len);
#endif

#ifdef __cplusplus
}
#endif
#endif /* BT_MAP_SUPPORT */
#endif /* __BT_MAP_SERVICE_H__ */

