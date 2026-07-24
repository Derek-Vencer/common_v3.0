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
#include "app_spp.h"
#include "bta_bt_api.h"
#include "hal_trace.h"
// #include "list.h"

#ifndef TRACE
#define TRACE(attr, str, ...)   TR_INFO(attr, str, ##__VA_ARGS__)
#endif

typedef struct {
    bt_spp_connhdl_t connhdl;
    // list_t *data_list;
} app_spp_demo_ctx_t;

static app_spp_demo_ctx_t app_spp_demo_ctx;

static void app_spp_demo_event_cb(const bt_bdaddr_t *addr, bta_spp_event_t event, const bt_spp_event_params_t *param)
{
    TRACE(0, "%s connhdl=%d error_code=%d", __func__, param->connhdl, param->error_code);

    switch (event)
    {
        case BT_SPP_EVENT_CONNECTED:
            app_spp_demo_ctx.connhdl = param->connhdl;
            break;
        case BT_SPP_EVENT_DISCONNECTED:
            app_spp_demo_ctx.connhdl = 0;
            break;
        case BT_SPP_EVENT_RECV_DATA:
            DUMP8("%02x ", param->params.recv_data.data, param->params.recv_data.size);
            break;
        case BT_SPP_EVENT_SEND_DONE:
            TRACE(0, "%s priv=%p", __func__, param->params.send_done.priv);
            break;
        case BT_SPP_EVENT_RECV_CREDIT:
            TRACE(0, "%s credits=%d", __func__, param->params.recv_credit.credits);
            break;
        default:
            break;
    }
}

static bool app_spp_demo_connect_req_cb(const bt_bdaddr_t *addr, bt_rfcomm_channel_t local_server_channel)
{
    TRACE(0, "%s channel=%d", __func__, local_server_channel);
    return true;
}

static bt_spp_callbacks_t spp_test_cbs = {
    .server_event_cb = app_spp_demo_event_cb,
    .conn_req_cb = app_spp_demo_connect_req_cb,
};

void app_spp_demo_init()
{
    TRACE(0, "%s", __func__);

    // uuid 0x00112233 ... eeff
    bta_uuid_t uuid = {{0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00}, 0x10};
    bta_spp_init(BT_RFCOMM_CHANNEL_CUSTOM_1, &uuid, &spp_test_cbs, "spp_demo", 8, (800 * 3));
}

void app_spp_demo_connect(const bt_bdaddr_t *addr)
{
    bta_uuid_t uuid = {{0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00}, 0x10};

    TRACE(0, "%s", __func__);
    bta_spp_initiate_connect(addr, BT_RFCOMM_CHANNEL_CUSTOM_1, &uuid);
}

void app_spp_demo_send_data(const uint8_t *data, uint16_t size)
{
    TRACE(0, "%s", __func__);
    bta_spp_send_data(app_spp_demo_ctx.connhdl, data, size);
}
