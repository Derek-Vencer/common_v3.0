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
#ifndef __BT_A2DP_SERVICE_H__
#define __BT_A2DP_SERVICE_H__
#include "adapter_service.h"
#include "a2dp_api.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BT_A2DP_EVENT_OPENED = BT_EVENT_A2DP_OPENED,
    BT_A2DP_EVENT_CLOSED,
    BT_A2DP_EVENT_STREAM_START,
    BT_A2DP_EVENT_STREAM_RECONFIG,
    BT_A2DP_EVENT_STREAM_SUSPEND,
    BT_A2DP_EVENT_STREAM_CLOSE,
    BT_A2DP_EVENT_STREAM_DATA_IND,
    BT_A2DP_EVENT_CUSTOM_CMD_REQ,
    BT_A2DP_EVENT_CUSTOM_CMD_RSP,
    BT_A2DP_EVENT_END,
} bt_a2dp_event_t;

#if BT_A2DP_EVENT_END != BT_EVENT_A2DP_END
#error "bt_a2dp_event_t error define"
#endif

typedef int (*bt_a2dp_callback_t)(const bt_bdaddr_t *bd_addr, bt_a2dp_event_t event, bt_a2dp_callback_param_t param);

bt_status_t bt_a2dp_init(bt_a2dp_callback_t callback);
bt_status_t bt_a2dp_cleanup(void);
bt_status_t bt_a2dp_connect(const bt_bdaddr_t *bd_addr, enum A2DP_ROLE role_expected);
bt_status_t bt_a2dp_disconnect(const bt_bdaddr_t *bd_addr);
bt_status_t bt_a2dp_accept_unknown_cmd(const bt_bdaddr_t *bd_addr, const bt_a2dp_custom_cmd_req_param_t *cmd, bool accept);
bt_status_t bt_a2dp_send_custom_cmd(const bt_bdaddr_t *bd_addr, uint8_t custom_cmd_id, const uint8_t *data, uint16_t len);

/**
 * standard a2dp (source role) interface
 *
 */

#ifdef BT_SOURCE

typedef enum {
    BT_A2DP_SOURCE_EVENT_OPENED = BT_EVENT_A2DP_SOURCE_OPENED,
    BT_A2DP_SOURCE_EVENT_CLOSED,
    BT_A2DP_SOURCE_EVENT_STREAM_START,
    BT_A2DP_SOURCE_EVENT_STREAM_RECONFIG,
    BT_A2DP_SOURCE_EVENT_STREAM_SUSPEND,
    BT_A2DP_SOURCE_EVENT_STREAM_CLOSE,
    BT_A2DP_SOURCE_EVENT_END,
} bt_a2dp_source_event_t;

#if BT_A2DP_SOURCE_EVENT_END != BT_EVENT_A2DP_SOURCE_END
#error "bt_a2dp_source_callback_t error define"
#endif

typedef int (*bt_a2dp_source_callback_t)(const bt_bdaddr_t *bd_addr, bt_a2dp_source_event_t event, bt_a2dp_callback_param_t param);

bt_status_t bt_a2dp_src_init(bt_a2dp_source_callback_t callback);
bt_status_t bt_a2dp_src_cleanup(void);
bt_status_t bt_a2dp_src_connect(const bt_bdaddr_t *bd_addr);
bt_status_t bt_a2dp_src_disconnect(const bt_bdaddr_t *bd_addr);
bt_status_t bt_a2dp_src_start_stream(const bt_bdaddr_t *bd_addr);
bt_status_t bt_a2dp_src_suspend_stream(const bt_bdaddr_t *bd_addr);

#endif /* BT_SOURCE */

#ifdef __cplusplus
}
#endif
#endif /* __BT_A2DP_SERVICE_H__ */

