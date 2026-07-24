/***************************************************************************
 *
 * Copyright 2015-2023 BES.
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
#ifdef CFG_APP_DATAPATH_CLIENT
#ifdef BLE_HOST_SUPPORT

#include "bta_ble_api.h"
#include "bta_gatt_api.h"

#include "app_datapathc.h"

#include "app_ble_cmd_handler.h"

#include "hal_trace.h"
#include "string.h"

#define APP_DATAPATHC_GATT_UUID_CCCD        (0x2902)
#define APP_DATAPATHC_GATT_UUID_CURD        (0x2901)

#define APP_DATAPATHC_CONIDX(conidx)        (0x0F & (conidx))

static const bt_attr_uuid_t datapathc_service_uuid_128_le        = {0x12, 0x34, 0x56, 0x78, 0x90, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01};
static const bt_attr_uuid_t datapathc_tx_character_uuid_128_le   = {0x12, 0x34, 0x56, 0x78, 0x91, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x00, 0x02, 0x00, 0x02};
static const bt_attr_uuid_t datapathc_rx_character_uuid_128_le   = {0x12, 0x34, 0x56, 0x78, 0x92, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03};

typedef struct
{
    bt_gatt_prf_t head;
    bool peer_write_notified;
    bt_gatt_peer_serv_t *peer_service;
    bt_gatt_peer_char_t *peer_char[DPC_CHAR_MAX_NUM];
} dpc_prf_t;

typedef struct app_datapaths_environment
{
    uint8_t dpc_prf_id = 0;
    app_datapath_client_event_cb_t *dpc_client_cb = NULL;
    dpc_prf_t prf_conn[BLE_CONNECTION_MAX] = {{0}};
} app_datapaths_env_t;

app_datapaths_env_t app_datapathc_env = {0};

static dpc_prf_t *ble_datapath_client_get_prf(uint16_t connhdl)
{
    uint8_t conidx = 0;

    for (conidx = 0; conidx < BLE_CONNECTION_MAX; conidx++)
    {
        if (bta_ble_get_conhdl_by_conidx(conidx) == connhdl)
        {
            return &app_datapathc_env.prf_conn[conidx];
        }
    }

    return NULL;
}

int ble_datapath_client_read_req(uint16_t connhdl, uint16_t code)
{
    dpc_prf_t *prf = NULL;
    bt_gatt_peer_char_t *c = NULL;
    int status = -2;

    bool is_desc = (code & DPC_DESC_MASK) ? true : false;
    uint16_t desc_uuid = 0;

    prf = ble_datapath_client_get_prf(connhdl);
    if (prf == NULL)
    {
        return status;
    }

    switch (code)
    {
        case DPC_NTF_CHAR_DATA:
        {
            c = prf->peer_char[DPC_CHAR_DATA_RX];
        }
        break;
        case DPC_WRITE_CHAR_DATA:
        {
            c = prf->peer_char[DPC_CHAR_DATA_TX];
        }
        break;
        case DPC_RD_WR_NTF_CCC_CFG:
        {
            c = prf->peer_char[DPC_CHAR_DATA_RX];
            desc_uuid = APP_DATAPATHC_GATT_UUID_CCCD;
        }
        break;
        case DPC_RD_NTF_CUD:
        {
            c = prf->peer_char[DPC_CHAR_DATA_RX];
            desc_uuid = APP_DATAPATHC_GATT_UUID_CURD;
        }
        break;
        case DPC_RD_WR_CUD:
        {
            c = prf->peer_char[DPC_CHAR_DATA_TX];
            desc_uuid = APP_DATAPATHC_GATT_UUID_CURD;
        }
        break;
        default:
            break;
    }
    if (c == NULL)
    {
        return status;
    }

    if (!is_desc)
    {
        status = bta_gattc_read_character_value(&prf->head, c);
    }
    else
    {
        status = bta_gattc_read_descriptor_value(&prf->head, c, desc_uuid);
    }

    return status;
}

static int ble_datapath_client_write_req(uint16_t connhdl, uint16_t code,
                                         uint8_t write_cmd, const uint8_t *value, uint16_t len)
{
    dpc_prf_t *prf = NULL;
    bt_gatt_peer_char_t *c = NULL;
    uint16_t notify_enabled = 0x0001;
    int status = -2;

    if (code >= DPC_CHAR_MAX_NUM || value == NULL || len == 0)
    {
        return status;
    }

    prf = ble_datapath_client_get_prf(connhdl);
    if (prf == NULL)
    {
        return status;
    }

    c = prf->peer_char[code];
    if (c == NULL)
    {
        return status;
    }

    if (code == DPC_CHAR_DATA_RX)
    {
        notify_enabled = value[0] ? true : false;
        status = bta_gattc_write_descriptor_value(&prf->head, c, APP_DATAPATHC_GATT_UUID_CCCD, (uint8_t *)&notify_enabled, sizeof(notify_enabled));
        prf->peer_write_notified = notify_enabled;
    }
    else if (code == DPC_CHAR_DATA_TX)
    {
        if (write_cmd)
        {
            status = bta_gattc_write_character_command(&prf->head, c, value, len, false);
        }
        else
        {
            status = bta_gattc_write_character_value(&prf->head, c, value, len);
        }
    }

    return status;
}

static void ble_datapath_client_start_discover(uint16_t connhdl)
{
    dpc_prf_t *prf = ble_datapath_client_get_prf(connhdl);
    if (prf == NULL)
    {
        return;
    }

    const uint8_t *dp_service_uuid_le = datapathc_service_uuid_128_le.attr_uuid;
    bta_gattc_discover_service(&prf->head, 0, dp_service_uuid_le);

    return;
}

static void ble_datapath_client_conn_handler(bt_gatt_prf_t *prf, bool is_central)
{
    uint16_t connhdl = prf->connhdl;
    uint8_t conidx = APP_DATAPATHC_CONIDX(prf->con_idx);

    dpc_prf_t *prf_get = ble_datapath_client_get_prf(connhdl);
    if (prf_get == NULL)
    {
        return;
    }

    prf_get->head = *prf;

    if (is_central == true)
    {
        ble_datapath_client_start_discover(connhdl);
    }

    if (app_datapathc_env.dpc_client_cb && app_datapathc_env.dpc_client_cb->dpc_connected_done_cb)
    {
        app_datapathc_env.dpc_client_cb->dpc_connected_done_cb(conidx);
    }
}

static void ble_datapath_client_disconn_handler(dpc_prf_t *prf)
{
    uint8_t conidx = APP_DATAPATHC_CONIDX(prf->head.con_idx);

    if (app_datapathc_env.dpc_client_cb && app_datapathc_env.dpc_client_cb->dpc_disconnected_done_cb)
    {
        app_datapathc_env.dpc_client_cb->dpc_disconnected_done_cb(conidx);
    }
}

static void ble_datapath_client_mtu_changed(dpc_prf_t *prf, uint16_t mtu)
{
    uint8_t conidx = APP_DATAPATHC_CONIDX(prf->head.con_idx);

    if (app_datapathc_env.dpc_client_cb && app_datapathc_env.dpc_client_cb->dpc_mtu_exchanged_done_cb)
    {
        app_datapathc_env.dpc_client_cb->dpc_mtu_exchanged_done_cb(conidx, mtu);
    }
}

static void ble_datapath_client_discover_complete(dpc_prf_t *prf)
{
    uint8_t conidx = APP_DATAPATHC_CONIDX(prf->head.con_idx);
    uint8_t value[2] = {0x01, 00};

    ble_datapath_client_write_req(prf->head.connhdl, DPC_CHAR_DATA_RX, false, value, 2);

    if (app_datapathc_env.dpc_client_cb && app_datapathc_env.dpc_client_cb->dpc_discover_done_cb)
    {
        app_datapathc_env.dpc_client_cb->dpc_discover_done_cb(conidx);
    }
}

static void ble_datapath_client_char_read_complete(dpc_prf_t *prf,
                                                   uint8_t error_code,
                                                   const bt_gatt_peer_char_t *c,
                                                   const uint8_t *value, uint16_t len)
{
    POSSIBLY_UNUSED dpc_char_enum_t char_enum = DPC_CHAR_MAX_NUM;

    if (error_code)
    {
        TRACE(0, "%s err code:%d", __func__, error_code);
    }

    if (c == prf->peer_char[DPC_CHAR_DATA_RX])
    {
        char_enum = DPC_CHAR_DATA_RX;
    }
    else if (c == prf->peer_char[DPC_CHAR_DATA_TX])
    {
        char_enum = DPC_CHAR_DATA_TX;
    }

    if (value)
    {
        TRACE(0, "%s char_enum %d read cmp:", __func__, char_enum);
        DUMP8("%02x ", value, len);
    }
}

static void ble_datapath_client_desc_read_complete(dpc_prf_t *prf,
                                                   uint8_t error_code,
                                                   const bt_gatt_peer_char_t *c,
                                                   uint16_t desc_uuid,
                                                   const uint8_t *value, uint16_t len)
{
    POSSIBLY_UNUSED dpc_char_enum_t char_enum = DPC_CHAR_MAX_NUM;
    POSSIBLY_UNUSED dpc_desc_enum_t desc_enum = DPC_DESC_MAX_NUM;

    if (error_code)
    {
        TRACE(0, "%s err code:%d", __func__, error_code);
    }

    if (c == prf->peer_char[DPC_CHAR_DATA_RX])
    {
        char_enum = DPC_CHAR_DATA_RX;
        if (desc_uuid == APP_DATAPATHC_GATT_UUID_CCCD)
        {
            desc_enum = DPC_DESC_RX_CFG;
        }
        else if (desc_uuid == APP_DATAPATHC_GATT_UUID_CURD)
        {
            desc_enum = DPC_DESC_RX_CUD;
        }
    }
    else if (c == prf->peer_char[DPC_CHAR_DATA_TX])
    {
        char_enum = DPC_CHAR_DATA_TX;
        if (desc_uuid == APP_DATAPATHC_GATT_UUID_CURD)
        {
            desc_enum = DPC_DESC_TX_CUD;
        }
    }

    if (value)
    {
        TRACE(0, "%s char_enum %d desc_enmu %d read cmp:", __func__, char_enum, desc_enum);
        DUMP8("%02x ", value, len);
    }
}

static void ble_datapath_client_char_write_complete(dpc_prf_t *prf,
                                                    uint8_t error_code,
                                                    const bt_gatt_peer_char_t *c)
{
    uint8_t conidx = APP_DATAPATHC_CONIDX(prf->head.con_idx);

    if (error_code)
    {
        TRACE(0, "%s err_code %d", __func__, error_code);
    }

    if (app_datapathc_env.dpc_client_cb && app_datapathc_env.dpc_client_cb->dpc_tx_done_cb)
    {
        app_datapathc_env.dpc_client_cb->dpc_tx_done_cb(conidx, error_code);
    }
}

static void ble_datapath_client_desc_write_complete(dpc_prf_t *prf,
                                                    uint8_t error_code,
                                                    const bt_gatt_peer_char_t *c,
                                                    uint16_t desc_uuid)
{
    TRACE(0, "%s err_code %d", __func__, error_code);
}

static void ble_datapath_client_rx_data_received(dpc_prf_t *prf,
                                                 const bt_gatt_peer_char_t *c,
                                                 const uint8_t *value, uint16_t len)
{
    uint8_t conidx = APP_DATAPATHC_CONIDX(prf->head.con_idx);

    // TRACE(0, "%s len %d", __func__, len);
    // DUMP8("%02x ", value, len);
#ifndef __INTERCONNECTION__
    BLE_custom_command_receive_data((uint8_t *)value, len, prf->head.con_idx);
#endif
    if (app_datapathc_env.dpc_client_cb && app_datapathc_env.dpc_client_cb->dpc_data_received_cb)
    {
        app_datapathc_env.dpc_client_cb->dpc_data_received_cb(conidx, value, len);
    }
}

static bool ble_datapath_client_callback(bt_gatt_prf_t *prf, bt_gattc_event_t event, bt_gattc_callback_param_t param)
{
    dpc_prf_t *dpc = ble_datapath_client_get_prf(prf->connhdl);

    if (dpc == NULL)
    {
        return false;
    }

    switch (event)
    {
        case BT_GATTC_EVENT_OPENED:
        {
            ble_datapath_client_conn_handler(prf, true);
            break;
        }
        case BT_GATTC_EVENT_CLOSED:
        {
            ble_datapath_client_disconn_handler(dpc);
            break;
        }
        case BT_GATTC_EVENT_MTU_CHANGED:
        {
            ble_datapath_client_mtu_changed(dpc, param.mtu_changed->mtu);
            break;
        }
        case BT_GATTC_EVENT_SERVICE:
        {
            bt_gattc_service_t *p = param.service;
            bt_gatt_peer_serv_t *s = p->service;
            bta_gattc_discover_all_characters(prf, s);
            break;
        }
        case BT_GATTC_EVENT_CHARACTER:
        {
            bt_gattc_character_t *p = param.character;
            bt_gatt_peer_char_t *c = p->character;
            uint8_t index = DPC_CHAR_DATA_RX;

            bt_gatt_peer_char_uuid_t char_uuid = {0};

            bta_gattc_get_character_uuid(c, &char_uuid);

            const uint8_t *p_uuid_128 = char_uuid.uuid_le;
            const uint8_t *rx_char_uuid = datapathc_tx_character_uuid_128_le.attr_uuid;

            if (memcmp(p_uuid_128, rx_char_uuid, 16))
            {
                index = DPC_CHAR_DATA_TX;
            }

            if (index < DPC_CHAR_MAX_NUM)
            {
                if (p->error_code == 0)
                {
                    dpc->peer_char[index] = c;
                }
                else
                {
                    dpc->peer_char[index] = NULL;
                }
            }
            if (p->discover_cmpl)
            {
                ble_datapath_client_discover_complete(dpc);
            }
            break;
        }
        case BT_GATTC_EVENT_CHAR_READ_RSP:
        {
            bt_gattc_char_read_rsp_t *p = param.char_read_rsp;
            ble_datapath_client_char_read_complete(dpc, p->error_code, p->character, p->value, p->value_len);
            break;
        }
        case BT_GATTC_EVENT_DESC_READ_RSP:
        {
            bt_gattc_desc_read_rsp_t *p = param.desc_read_rsp;
            ble_datapath_client_desc_read_complete(dpc, p->error_code, p->character, p->desc_uuid, p->value, p->value_len);
            break;
        }
        case BT_GATTC_EVENT_CHAR_WRITE_RSP:
        {
            bt_gattc_char_write_rsp_t *p = param.char_write_rsp;
            ble_datapath_client_char_write_complete(dpc, p->error_code, p->character);
            break;
        }
        case BT_GATTC_EVENT_DESC_WRITE_RSP:
        {
            bt_gattc_desc_write_rsp_t *p = param.desc_write_rsp;
            ble_datapath_client_desc_write_complete(dpc, p->error_code, p->character, p->desc_uuid);
            break;
        }
        case BT_GATTC_EVENT_NOTIFY:
        {
            bt_gattc_recv_notify_t *ntf = param.notify;
            if (ntf->character == dpc->peer_char[DPC_CHAR_DATA_RX])
            {
                ble_datapath_client_rx_data_received(dpc, ntf->character, ntf->value, ntf->value_len);
            }
            break;
        }
        default:
            break;
    }
    return 0;
}

void app_datapathc_init(void)
{
    bt_gattc_cfg_t prf_cfg = {0};

    app_datapathc_env.dpc_prf_id = bta_gattc_register_profile(ble_datapath_client_callback, &prf_cfg);
}

void app_datapathc_deinit(void)
{
    bta_gattc_unregister_profile(app_datapathc_env.dpc_prf_id);
}

void app_datapath_client_register_callback(app_datapath_client_event_cb_t *callback)
{
    app_datapathc_env.dpc_client_cb = callback;
}

void app_datapath_client_control_notification(uint8_t conidx, bool isEnable)
{
    uint16_t connhdl = bta_ble_get_conhdl_by_conidx(conidx);
    uint8_t value[2] = {0};
    dpc_prf_t *prf = ble_datapath_client_get_prf(connhdl);
    if (prf == NULL)
    {
        return;
    }

    if (isEnable != prf->peer_write_notified)
    {
        value[0] = isEnable;
        ble_datapath_client_write_req(connhdl, DPC_CHAR_DATA_RX, false, value, 2);
    }
}

void app_datapath_client_send_data_via_write_command(uint8_t conidx, uint8_t *ptrData, uint32_t length)
{
    uint16_t connhdl = bta_ble_get_conhdl_by_conidx(conidx);
    ble_datapath_client_write_req(connhdl, DPC_CHAR_DATA_TX, true, ptrData, length);
}

void app_datapath_client_send_data_via_write_request(uint8_t conidx, uint8_t *ptrData, uint32_t length)
{
    uint16_t connhdl = bta_ble_get_conhdl_by_conidx(conidx);
    ble_datapath_client_write_req(connhdl, DPC_CHAR_DATA_TX, false, ptrData, length);
}

#endif /* BLE_HOST_SUPPORT */
#endif /* CFG_APP_DATAPATH_CLIENT */
