/***************************************************************************
 *
 * Copyright 2015-2026 BES.
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

#ifdef BLE_TOTA_ENABLED

#include "cmsis_os2.h"
#include <string.h>

#include "bta_ble_api.h"
#include "bta_gatt_api.h"

#include "app_ble_tota.h"

#include "hal_trace.h"

#ifndef TRACE
#define TRACE(attr, str, ...)               TR_INFO(attr, str, ##__VA_ARGS__)
#endif

#define TOTA_MAX_TRANS_LEN                  (495)

#define TOTA_ZERO_INDEX(index)              (index & 0xF)

typedef enum app_ble_tota_attr_type
{
    APP_BLE_TOTA_ATTR_TYPE_TOTA_SERVICE = 0,

    APP_BLE_TOTA_ATTR_TYPE_TOTA_CHAR,
    APP_BLE_TOTA_ATTR_TYPE_TOTA_CCCD,

    APP_BLE_TOTA_ATTR_TYPE_MAX,
} app_ble_tota_attr_type_e;

static const bt_attr_uuid_t ble_tota_service_uuid_le    = {0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86};
static const bt_attr_uuid_t ble_tota_char_uuid_le       = {0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97};
static const bt_attr_uuid_t ble_tota_cccd_uuid_le       = {0x02, 0x29};

static bt_attr_mem_t ble_tota_attr_mem[APP_BLE_TOTA_ATTR_TYPE_MAX] = {{{0}}};

static bt_gatt_attr_t ble_tota_attr_list[APP_BLE_TOTA_ATTR_TYPE_MAX] = {{0}};

struct app_ble_tota_environment
{
    app_tota_event_callback app_tota_event_cb;
    bool ntf_enabled[BLE_CONNECTION_MAX];
} app_ble_tota_env;

static const bt_gatt_attr_t *app_ble_tota_build_attr_list(uint8_t *p_attr_size)
{
    bt_attr_info_t attr_info = {0};

    // Below are (2) 128 bits len attributes
    attr_info.is_128_bits_attr_uuid = true;

    /* TOTA Service */
    attr_info.attr_uuid = ble_tota_service_uuid_le;
    ble_tota_attr_list[APP_BLE_TOTA_ATTR_TYPE_TOTA_SERVICE] =
        bta_gatts_build_attr(BT_ATTR_TYPE_PRI_SERVICE,
                             &attr_info,
                             &ble_tota_attr_mem[APP_BLE_TOTA_ATTR_TYPE_TOTA_SERVICE]);

    /* TOTA Characteristic */
    attr_info.attr_prop = BT_GATT_PROP_WR_REQ | BT_GATT_PROP_WR_CMD | BT_GATT_PROP_NTF_PROP;
    attr_info.attr_uuid = ble_tota_char_uuid_le;
    ble_tota_attr_list[APP_BLE_TOTA_ATTR_TYPE_TOTA_CHAR] =
        bta_gatts_build_attr(BT_ATTR_TYPE_CHARACTER,
                             &attr_info,
                             &ble_tota_attr_mem[APP_BLE_TOTA_ATTR_TYPE_TOTA_CHAR]);

    // Below are (1) 16 bits len attributes
    attr_info.is_128_bits_attr_uuid = false;
    /* TOTA Character CCCD */
    attr_info.attr_prop = BT_GATT_PROP_WR_REQ | BT_GATT_PROP_RD_REQ;
    // Need Write enc permissin
    attr_info.attr_perm = BT_GATT_PERM_WR_ENC;
    attr_info.attr_uuid = ble_tota_cccd_uuid_le;
    ble_tota_attr_list[APP_BLE_TOTA_ATTR_TYPE_TOTA_CCCD] =
        bta_gatts_build_attr(BT_ATTR_TYPE_DESCRIPTOR,
                             &attr_info,
                             &ble_tota_attr_mem[APP_BLE_TOTA_ATTR_TYPE_TOTA_CCCD]);

    *p_attr_size = APP_BLE_TOTA_ATTR_TYPE_MAX;

    return ble_tota_attr_list;
}

static const bt_attr_t *app_ble_tota_get_attribute(app_ble_tota_attr_type_e type)
{
    return ble_tota_attr_list[type].attr_data;
}

static bool app_ble_tota_server_callback(bt_gatt_svc_t *svc, bt_gatts_event_t event, bt_gatts_callback_param_t param)
{
    app_tota_event_param_t cb_param = {0};

    cb_param.connhdl = svc->connhdl;

    uint8_t conidx = TOTA_ZERO_INDEX(svc->con_idx);

    if (conidx >= BLE_CONNECTION_MAX)
    {
        return false;
    }

    switch (event)
    {
        case BT_GATTS_EVENT_CHAR_WRITE:
        {
            bt_gatts_char_write_t *p = param.char_write;
            if (p->value_offset != 0 || p->value_len == 0 || p->value == NULL)
            {
                return false;
            }
            if (app_ble_tota_env.app_tota_event_cb)
            {
                cb_param.event_type   = APP_TOTA_RECEVICE_DATA;
                cb_param.conidx = p->con_idx;
                cb_param.param.receive_data.data_len = p->value_len;
                cb_param.param.receive_data.data     = (uint8_t *)p->value;
                app_ble_tota_env.app_tota_event_cb(&cb_param);
            }
            bta_gatts_send_write_rsp(svc->connhdl, p->token, 0);
            return true;
        }
        case BT_GATTS_EVENT_DESC_WRITE:
        {
            bt_gatts_desc_write_t *p = param.desc_write;
            uint16_t config = *(uint16_t *)(p->value);
            bool notify_enabled = false;
            if (config & BT_GATT_CCCD_NOTIFICATION)
            {
                notify_enabled = true;
            }
            app_ble_tota_env.ntf_enabled[conidx] = notify_enabled;
            if (app_ble_tota_env.app_tota_event_cb)
            {
                cb_param.event_type   = APP_TOTA_CCC_CHANGED;
                cb_param.conidx = p->con_idx;
                cb_param.param.ntf_en = notify_enabled;
                app_ble_tota_env.app_tota_event_cb(&cb_param);
            }
            return true;
        }
        case BT_GATTS_EVENT_DESC_READ:
        {
            bt_gatts_desc_read_t *p = param.desc_read;
            uint16_t cccd_config = app_ble_tota_env.ntf_enabled[conidx] ? BT_GATT_CCCD_NOTIFICATION : 0x0000;
            bta_gatts_send_read_rsp(svc->connhdl, p->token, 0, (uint8_t *)&cccd_config, sizeof(uint16_t));
            return true;
        }
        case BT_GATTS_EVENT_NTF_TX_DONE:
        case BT_GATTS_EVENT_INDICATE_CFM:
        {
            bt_gatts_indicate_cfm_t *p = param.confirm;
            if (app_ble_tota_env.app_tota_event_cb)
            {
                cb_param.event_type   = APP_TOTA_SEND_DONE;
                cb_param.conidx = p->con_idx;
                cb_param.param.status = p->error_code;
                app_ble_tota_env.app_tota_event_cb(&cb_param);
            }
            break;
        }
        case BT_GATTS_EVENT_MTU_CHANGED:
        {
            bt_gatts_mtu_changed_t *p = param.mtu_changed;
            if (app_ble_tota_env.app_tota_event_cb)
            {
                cb_param.event_type = APP_TOTA_MTU_UPDATE;
                cb_param.conidx = p->con_idx;
                cb_param.param.mtu = p->mtu;
                app_ble_tota_env.app_tota_event_cb(&cb_param);
            }
            break;
        }
        case BT_GATTS_EVENT_CONN_CLOSED:
        {
            bt_gatts_conn_closed_t *p = param.closed;
            if (app_ble_tota_env.app_tota_event_cb)
            {
                cb_param.event_type = APP_TOTA_DIS_CONN_EVENT;
                cb_param.conidx = p->con_idx;
                cb_param.param.status = p->error_code;
                app_ble_tota_env.app_tota_event_cb(&cb_param);
            }
            break;
        }
        default:
        {
            break;
        }
    }

    return 0;
}

void app_ble_tota_init(void)
{
    bt_gatts_cfg_t cfg = {.preferred_mtu = TOTA_MAX_TRANS_LEN};
    uint8_t attr_size = 0;

    memset(&app_ble_tota_env, 0, sizeof(app_ble_tota_env));

    bta_gatts_register_service(app_ble_tota_build_attr_list(&attr_size), attr_size, app_ble_tota_server_callback, &cfg);
}

void app_ble_tota_deinit(void)
{
    bta_gatts_unregister_service(ble_tota_attr_list);
}

void app_tota_event_reg(app_tota_event_callback cb)
{
    if (!app_ble_tota_env.app_tota_event_cb)
    {
        app_ble_tota_env.app_tota_event_cb = cb;
    }
}

void app_tota_event_unreg(void)
{
    app_ble_tota_env.app_tota_event_cb = NULL;
}

bool app_tota_send_notification(uint8_t conidx, uint8_t *data, uint32_t len)
{
    int status = 0;
    uint16_t connhdl = bta_ble_get_conhdl_by_conidx(conidx);
    bt_gatt_char_notify_t val_ntf = {0};

    if (connhdl == BT_INVALID_HANDLE)
    {
        return false;
    }

    val_ntf.service = app_ble_tota_get_attribute(APP_BLE_TOTA_ATTR_TYPE_TOTA_SERVICE);
    val_ntf.character = app_ble_tota_get_attribute(APP_BLE_TOTA_ATTR_TYPE_TOTA_CHAR);

    status = bta_gatts_send_character_value(connhdl, true, &val_ntf, data, (uint16_t)len);

    return (status == 0);
}

bool app_tota_send_indication(uint8_t conidx, uint8_t *data, uint32_t len)
{
    int status = 0;
    uint16_t connhdl = bta_ble_get_conhdl_by_conidx(conidx);
    bt_gatt_char_notify_t val_ind = {0};

    if (connhdl == BT_INVALID_HANDLE)
    {
        return false;
    }

    val_ind.service = app_ble_tota_get_attribute(APP_BLE_TOTA_ATTR_TYPE_TOTA_SERVICE);
    val_ind.character = app_ble_tota_get_attribute(APP_BLE_TOTA_ATTR_TYPE_TOTA_CHAR);

    status = bta_gatts_send_character_value(connhdl, false, &val_ind, data, (uint16_t)len);

    return (status == 0);
}

#endif /* BLE_TOTA_ENABLED */
