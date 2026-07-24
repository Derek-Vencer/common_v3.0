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
#ifndef __BES_SOURCE_API_H__
#define __BES_SOURCE_API_H__
#ifdef __cplusplus
extern "C" {
#endif

typedef enum bt_source_event {
    BT_SOURCE_EVENT_SEARCH_RESULT = 0, // one result with information
    BT_SOURCE_EVENT_SEARCH_COMPLETE, // complete with status
    BT_SOURCE_EVENT_A2DP_SOURCE_CONNECT_FAIL,
    BT_SOURCE_EVENT_A2DP_SOURCE_STREAM_OPEN,
    BT_SOURCE_EVENT_A2DP_SOURCE_STREAM_CLOSE,
} bt_source_event_t;

typedef struct bt_source_event_param_t bt_source_event_param_t;

typedef struct bt_source_event_param_t {
    union {
        struct {
            app_bt_search_result_t *result;
        } search_result;
        struct {
            char status;
        } search_complete;
        struct {
            bt_bdaddr_t *addr;
        } a2dp_source_connect_fail;
        struct {
            bt_bdaddr_t *addr;
            int device_id;
        } a2dp_source_stream_open;
        struct {
            bt_bdaddr_t *addr;
            int device_id;
        } a2dp_source_stream_close;
    } p;
} bt_source_event_param_t;

typedef int (*bt_source_callback_t)(bt_source_event_t event, bt_source_event_param_t *param);

void bes_bt_source_start_a2dp_stream(uint8_t device_id);

void bes_bt_source_suspend_a2dp_stream(uint8_t device_id);

void bes_bt_source_toggle_a2dp_stream(uint8_t device_id);

uint32_t bes_bt_source_write_a2dp_data(bool packet_type, uint8_t encoded_type, uint8_t * pcm_buf, uint32_t len, uint16_t frame_len);

uint32_t bes_bt_source_write_a2dp_pcm_data(uint8_t * pcm_buf, uint32_t len);

void bes_bt_source_reconnect_hfp_profile(const bt_bdaddr_t *remote);

void bes_bt_source_reconnect_a2dp_profile(const bt_bdaddr_t *remote);

void bes_bt_source_reconnect_avrcp_profile(const bt_bdaddr_t *remote);

#if defined(BT_HID_HOST)
void bes_bt_source_reconnect_hid_profile(const bt_bdaddr_t *remote);
#endif // BT_HID_HOST

void bes_bt_source_search_device(void);

void bes_bt_source_register_bt_callback(bt_source_callback_t cb);

void bes_bt_source_a2dp_stream_buffer_init();

void bes_bt_source_a2dp_stream_buffer_deinit();

#ifdef __cplusplus
}
#endif
#endif /* __BES_SOURCE_API_H__ */
