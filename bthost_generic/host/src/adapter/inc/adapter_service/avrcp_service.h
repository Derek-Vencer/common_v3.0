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
#ifndef __BT_AVRCP_SERVICE_H__
#define __BT_AVRCP_SERVICE_H__
#include "avrcp_api.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BT_AVRCP_EVT_OPENED = BT_EVENT_AVRCP_OPENED,
    BT_AVRCP_EVT_CLOSED,
    BT_AVRCP_EVT_VOLUME_CHANGE,
    BT_AVRCP_EVT_PLAY_STATUS_CHANGE,
    BT_AVRCP_EVT_PLAY_POS_CHANGE,
    BT_AVRCP_EVT_TRACK_CHANGE,
    BT_AVRCP_EVT_BATTERY_CHANGE,
    BT_AVRCP_EVT_RECV_PASSTHROUGH_CMD,
    BT_AVRCP_EVT_RECV_PLAY_STATUS,
    BT_AVRCP_EVT_RECV_MEDIA_STATUS,
    BT_AVRCP_SEND_KEY_INFO,
    BT_AVRCP_GET_COMPANY_ID,
    BT_AVRCP_GET_CAPABILITY,
    BT_AVRCP_NOTIFY_CHANGE,
    BT_AVRCP_ADDRESSED_PLAYER_CHANGED,
    BT_AVRCP_BROSING_OPENED,
    BT_AVRCP_BROSING_CMD_INFO,
    BT_AVRCP_BROSING_RSP_INFO,
    BT_AVRCP_BROSING_CLOSED,
    BT_AVRCP_OBEX_OPENED,
    BT_AVRCP_OBEX_CLOSED,
    BT_AVRCP_OBEX_GET_IMAGE_DONE,
    BT_AVRCP_OBEX_GET_THM_DONE,
    BT_AVRCP_OBEX_GET_IMAGE_PROPERTIES_DONE,
    BT_AVRCP_OBEX_EVENT_INFO,
    BT_AVRCP_EVT_END,
} bt_avrcp_event_t;

#if BT_AVRCP_EVT_END != BT_EVENT_AVRCP_END
#error "bt_avrcp_event_t error define"
#endif

typedef int (*bt_avrcp_callback_t)(const bt_bdaddr_t *bd_addr, bt_avrcp_event_t event, bt_avrcp_callback_param_t param);

bt_status_t bt_avrcp_init(bt_avrcp_callback_t callback);
bt_status_t bt_avrcp_cleanup(void);
bt_status_t bt_avrcp_connect(const bt_bdaddr_t *bd_addr);
bt_status_t bt_avrcp_disconnect(const bt_bdaddr_t *bd_addr);
bt_status_t bt_avrcp_send_passthrough_cmd(const bt_bdaddr_t *bd_addr, bt_avrcp_key_code_t key);
bt_status_t bt_avrcp_send_get_play_status(const bt_bdaddr_t *bd_addr);
bt_status_t bt_avrcp_send_get_media_status(const bt_bdaddr_t *bd_addr, uint32_t attr_masks);
bt_status_t bt_avrcp_send_set_abs_volume(const bt_bdaddr_t *bd_addr, uint8_t volume);
bt_status_t bt_avrcp_report_status_change(const bt_bdaddr_t *bd_addr, bt_avrcp_status_change_event_t event, uint32_t param);

#ifdef __cplusplus
}
#endif
#endif /* __BT_AVRCP_SERVICE_H__ */

