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
#if (defined(BES_OTA) || defined(BES_OTA_BASIC)) && !defined(OTA_OVER_TOTA_ENABLED) && !defined(NUTTX_BLE_OTA)

#include "cmsis_os2.h"

#include "bta_ble_api.h"
#include "bta_gatt_api.h"

#include "app_ble_ota.h"

#include "hal_trace.h"

#define APP_BLE_OTA_MAX_LEN (495)

typedef enum app_ble_ota_attr_type
{
    APP_BLE_OTA_ATTR_TYPE_SERVICE = 0,
    APP_BLE_OTA_ATTR_TYPE_CHARACTER,
    APP_BLE_OTA_ATTR_TYPE_CCCD,

    APP_BLE_OTA_ATTR_TYPE_MAX,
} app_ble_ota_attr_type_e;

typedef struct app_ble_ota_environment
{
    bool is_initilized;
    // Event callback
    app_ota_event_callback ota_event_cb;

} app_ble_ota_env_t;

static const bt_attr_uuid_t ble_ota_service_uuid_128_le         = {0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66};
static const bt_attr_uuid_t ble_ota_character_uuid_128_le       = {0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77};
static const bt_attr_uuid_t ble_ota_character_cccd_uuid_16_le   = {0x02, 0x29};

static bt_attr_mem_t ble_ota_attr_mem[APP_BLE_OTA_ATTR_TYPE_MAX] = {{{0}}};

static bt_gatt_attr_t ble_ota_attr_list[APP_BLE_OTA_ATTR_TYPE_MAX] = {{0}};

static app_ble_ota_env_t app_ota_env = {0};

static bool ble_ota_server_callback(bt_gatt_svc_t *svc, bt_gatts_event_t event, bt_gatts_callback_param_t param)
{
    app_ota_event_param_t cb_param = {0};

    cb_param.connhdl = svc->connhdl;

    switch (event)
    {
        case BT_GATTS_EVENT_CHAR_WRITE:
        {
            bt_gatts_char_write_t *p = param.char_write;

            if (p->value_offset != 0 || p->value_len == 0 || p->value == NULL)
            {
                return false;
            }

            if (app_ota_env.ota_event_cb)
            {
                cb_param.event_type = APP_OTA_RECEVICE_DATA;
                cb_param.conidx = svc->con_idx;
                cb_param.param.receive_data.data_len = p->value_len;
                cb_param.param.receive_data.data = (uint8_t *)p->value;
                app_ota_env.ota_event_cb(&cb_param);
            }

            bta_gatts_send_write_rsp(svc->connhdl, p->token, 0);
            return true;
        }
        case BT_GATTS_EVENT_DESC_WRITE:
        {
            bt_gatts_desc_write_t *p = param.desc_write;

            uint16_t config = *(uint16_t *)(p->value);

            if (app_ota_env.ota_event_cb)
            {
                cb_param.event_type   = APP_OTA_CCC_CHANGED;
                cb_param.conidx       = svc->con_idx;
                cb_param.param.ntf_en = (config & BT_GATT_CCCD_NOTIFICATION) != 0;
                app_ota_env.ota_event_cb(&cb_param);
            }
            return true;
        }
        case BT_GATTS_EVENT_DESC_READ:
        {
            bt_gatts_desc_read_t *p = param.desc_read;
            uint16_t cccd_config = BT_GATT_CCCD_NOTIFICATION;
            bta_gatts_send_read_rsp(svc->connhdl, p->token, 0, (uint8_t *)&cccd_config, sizeof(uint16_t));
            return true;
        }
        case BT_GATTS_EVENT_NTF_TX_DONE:
        case BT_GATTS_EVENT_INDICATE_CFM:
        {
            bt_gatts_indicate_cfm_t *p = param.confirm;
            if (app_ota_env.ota_event_cb)
            {
                cb_param.event_type   = APP_OTA_SEND_DONE;
                cb_param.conidx       = svc->con_idx;
                cb_param.param.status = p->error_code;
                app_ota_env.ota_event_cb(&cb_param);
            }
            break;
        }
        case BT_GATTS_EVENT_MTU_CHANGED:
        {
            bt_gatts_mtu_changed_t *p = param.mtu_changed;
            if (app_ota_env.ota_event_cb)
            {
                cb_param.event_type = APP_OTA_MTU_UPDATE;
                cb_param.conidx     = svc->con_idx;
                cb_param.param.mtu  = p->mtu;
                app_ota_env.ota_event_cb(&cb_param);
            }
            break;
        }
        case BT_GATTS_EVENT_CONN_CLOSED:
        {
            bt_gatts_conn_closed_t *p = param.closed;
            if (app_ota_env.ota_event_cb)
            {
                cb_param.event_type = APP_OTA_DIS_CONN;
                cb_param.conidx     = svc->con_idx;
                cb_param.param.status = p->error_code;
                app_ota_env.ota_event_cb(&cb_param);
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

static const bt_attr_t *app_ble_ota_get_attribute(app_ble_ota_attr_type_e type)
{
    return ble_ota_attr_list[type].attr_data;
}

static const bt_gatt_attr_t *app_ble_ota_build_attr_list(uint8_t *p_attr_size)
{
    bt_attr_info_t attr_info = {0};

    if (app_ota_env.is_initilized == true)
    {
        return ble_ota_attr_list;
    }

    attr_info.is_128_bits_attr_uuid = true;

    attr_info.attr_uuid = ble_ota_service_uuid_128_le;
    ble_ota_attr_list[APP_BLE_OTA_ATTR_TYPE_SERVICE] =
        bta_gatts_build_attr(BT_ATTR_TYPE_PRI_SERVICE, &attr_info, &ble_ota_attr_mem[APP_BLE_OTA_ATTR_TYPE_SERVICE]);

    attr_info.attr_prop = BT_GATT_PROP_WR_REQ | BT_GATT_PROP_WR_CMD | BT_GATT_PROP_NTF_PROP;
    attr_info.attr_uuid = ble_ota_character_uuid_128_le;
    ble_ota_attr_list[APP_BLE_OTA_ATTR_TYPE_CHARACTER] =
        bta_gatts_build_attr(BT_ATTR_TYPE_CHARACTER, &attr_info, &ble_ota_attr_mem[APP_BLE_OTA_ATTR_TYPE_CHARACTER]);

    attr_info.is_128_bits_attr_uuid = false;
    attr_info.attr_prop = BT_GATT_PROP_WR_REQ | BT_GATT_PROP_RD_REQ;
    attr_info.attr_uuid = ble_ota_character_cccd_uuid_16_le;
    ble_ota_attr_list[APP_BLE_OTA_ATTR_TYPE_CCCD] =
        bta_gatts_build_attr(BT_ATTR_TYPE_DESCRIPTOR, &attr_info, &ble_ota_attr_mem[APP_BLE_OTA_ATTR_TYPE_CCCD]);

    *p_attr_size = APP_BLE_OTA_ATTR_TYPE_MAX;

    return ble_ota_attr_list;
}

void app_ble_ota_init(void)
{
    bt_gatts_cfg_t cfg = {.preferred_mtu = APP_BLE_OTA_MAX_LEN};
    const bt_gatt_attr_t *attr_list = NULL;
    uint8_t attr_list_len = 0;

    if (app_ota_env.is_initilized == true)
    {
        return;
    }

    attr_list = app_ble_ota_build_attr_list(&attr_list_len);

    if (bta_gatts_register_service(attr_list, attr_list_len, ble_ota_server_callback, &cfg) == 0)
    {
        app_ota_env.is_initilized = true;
    }
}

void app_ble_ota_deinit(void)
{
    if (app_ota_env.is_initilized == false)
    {
        return;
    }

    bta_gatts_unregister_service(app_ble_ota_build_attr_list(NULL));

    app_ota_env.is_initilized = false;
}

void app_ota_event_reg(app_ota_event_callback cb)
{
    if (app_ota_env.ota_event_cb == NULL)
    {
        app_ota_env.ota_event_cb = cb;
    }
}

void app_ota_event_unreg(void)
{
    app_ota_env.ota_event_cb = NULL;
}

bool app_ota_send_notification(uint8_t conidx, uint8_t *data, uint32_t len)
{
    int status = 0;

    bt_gatt_char_notify_t val_ntf =
    {
        .service = app_ble_ota_get_attribute(APP_BLE_OTA_ATTR_TYPE_SERVICE),
        .character = app_ble_ota_get_attribute(APP_BLE_OTA_ATTR_TYPE_CHARACTER),
    };

    status = bta_gatts_send_character_value(bta_ble_get_conhdl_by_conidx(conidx), true, &val_ntf, data, (uint16_t)len);

    return (status == 0);
}

bool app_ota_send_indication(uint8_t conidx, uint8_t *data, uint32_t len)
{
    int status = 0;

    bt_gatt_char_notify_t val_ind =
    {
        .service = app_ble_ota_get_attribute(APP_BLE_OTA_ATTR_TYPE_SERVICE),
        .character = app_ble_ota_get_attribute(APP_BLE_OTA_ATTR_TYPE_CHARACTER),
    };

    status = bta_gatts_send_character_value(bta_ble_get_conhdl_by_conidx(conidx), false, &val_ind, data, (uint16_t)len);

    return (status == 0);
}

void app_ota_send_rx_cfm(uint8_t conidx)
{

}

#endif // (defined(BES_OTA) || defined(BES_OTA_BASIC)) && !defined(OTA_OVER_TOTA_ENABLED) && !defined(NUTTX_BLE_OTA)
