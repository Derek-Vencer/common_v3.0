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
#ifdef CFG_APP_DATAPATH_SERVER

#include "bta_gatt_api.h"
#ifdef BLE_HOST_SUPPORT
#include "bta_ble_api.h"
#endif

#include "app_datapaths.h"

#include "hal_trace.h"

#define APP_DATAPATHS_CONN_MAX              (3)

#ifndef TRACE
#define TRACE(attr, str, ...)               TR_INFO(attr, str, ##__VA_ARGS__)
#endif

typedef enum app_datapath_attr_type
{
    APP_DPS_ATTR_TYPE_SERVICE = 0,
    APP_DPS_ATTR_TYPE_RX_CHARACTER,
    APP_DPS_ATTR_TYPE_RX_CUDD,
    APP_DPS_ATTR_TYPE_TX_CHARACTER,
    APP_DPS_ATTR_TYPE_TX_CCCD,
    APP_DPS_ATTR_TYPE_TX_CUDD,

    APP_DPS_ATTR_TYPE_MAX,
} app_datapath_attr_type_e;

typedef struct app_datapaths_environment
{
    bool is_initilized;
    struct
    {
        uint16_t connectionHandle;
        uint8_t isNotificationEnabled;
        uint8_t recv_cmd_latest;
    } con_info[APP_DATAPATHS_CONN_MAX];

    app_datapath_event_cb dp_event_callback;
    app_datapath_server_tx_done_t tx_done_callback;
    app_datapath_server_data_received_callback_func_t rx_done_callback;
    app_datapath_server_disconnected_done_t disconnected_done_callback;
    app_datapath_server_connected_done_t connected_done_callback;
    app_datapath_server_mtuexchanged_done_t mtu_exchanged_done_callback;
    app_datapath_server_role_switch_callback_t le_rs_callback;

} app_datapaths_env_t;

static const char custom_tx_desc[] = "App Data Path TX";
static const char custom_rx_desc[] = "App Data Path RX";

static const bt_attr_uuid_t datapath_service_uuid_128_le        = {0x12, 0x34, 0x56, 0x78, 0x90, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01};
static const bt_attr_uuid_t datapath_tx_character_uuid_128_le   = {0x12, 0x34, 0x56, 0x78, 0x91, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x00, 0x02, 0x00, 0x02};
static const bt_attr_uuid_t datapath_rx_character_uuid_128_le   = {0x12, 0x34, 0x56, 0x78, 0x92, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03};
static const bt_attr_uuid_t datapath_character_cudd_uuid_16_le  = {0x01, 0x29};
static const bt_attr_uuid_t datapath_character_cccd_uuid_16_le  = {0x02, 0x29};

static bt_attr_mem_t datapath_attr_mem[APP_DPS_ATTR_TYPE_MAX] = {{{0}}};

static bt_gatt_attr_t datapath_attr_list[APP_DPS_ATTR_TYPE_MAX] = {{0}};

static app_datapaths_env_t app_dp_server_env = {0, {{0}}};

static const bt_attr_t *app_datapaths_get_attribute(app_datapath_attr_type_e type)
{
    return datapath_attr_list[type].attr_data;
}

static const bt_gatt_attr_t *app_datapaths_build_attr_list(uint8_t *p_attr_size)
{
    bt_attr_info_t attr_info = {0};

    if (app_dp_server_env.is_initilized == true)
    {
        return datapath_attr_list;
    }

    // Below are (3) 128 bits len attributes
    attr_info.is_128_bits_attr_uuid = true;

    attr_info.attr_uuid = datapath_service_uuid_128_le;
    datapath_attr_list[APP_DPS_ATTR_TYPE_SERVICE] =
        bta_gatts_build_attr(BT_ATTR_TYPE_PRI_SERVICE, &attr_info, &datapath_attr_mem[APP_DPS_ATTR_TYPE_SERVICE]);

    attr_info.attr_prop = BT_GATT_PROP_WR_REQ | BT_GATT_PROP_WR_CMD | BT_GATT_PROP_RD_REQ;
    attr_info.attr_uuid = datapath_rx_character_uuid_128_le;
    datapath_attr_list[APP_DPS_ATTR_TYPE_RX_CHARACTER] =
        bta_gatts_build_attr(BT_ATTR_TYPE_CHARACTER, &attr_info, &datapath_attr_mem[APP_DPS_ATTR_TYPE_RX_CHARACTER]);

    attr_info.attr_prop = BT_GATT_PROP_NTF_PROP;
    attr_info.attr_uuid = datapath_tx_character_uuid_128_le;
    datapath_attr_list[APP_DPS_ATTR_TYPE_TX_CHARACTER] =
        bta_gatts_build_attr(BT_ATTR_TYPE_CHARACTER, &attr_info, &datapath_attr_mem[APP_DPS_ATTR_TYPE_TX_CHARACTER]);

    // Below are (3) 16 bits len attributes
    attr_info.is_128_bits_attr_uuid = false;

    attr_info.attr_prop = BT_GATT_PROP_WR_REQ | BT_GATT_PROP_RD_REQ;
    attr_info.attr_uuid = datapath_character_cudd_uuid_16_le;
    datapath_attr_list[APP_DPS_ATTR_TYPE_RX_CUDD] =
        bta_gatts_build_attr(BT_ATTR_TYPE_DESCRIPTOR, &attr_info, &datapath_attr_mem[APP_DPS_ATTR_TYPE_RX_CUDD]);

    attr_info.attr_prop = BT_GATT_PROP_WR_REQ | BT_GATT_PROP_RD_REQ;
    attr_info.attr_uuid = datapath_character_cudd_uuid_16_le;
    datapath_attr_list[APP_DPS_ATTR_TYPE_TX_CUDD] =
        bta_gatts_build_attr(BT_ATTR_TYPE_DESCRIPTOR, &attr_info, &datapath_attr_mem[APP_DPS_ATTR_TYPE_TX_CUDD]);

    attr_info.attr_prop = BT_GATT_PROP_WR_REQ | BT_GATT_PROP_RD_REQ;
    // Need Write enc permissin
    attr_info.attr_perm = BT_GATT_PERM_WR_ENC;
    attr_info.attr_uuid = datapath_character_cccd_uuid_16_le;
    datapath_attr_list[APP_DPS_ATTR_TYPE_TX_CCCD] =
        bta_gatts_build_attr(BT_ATTR_TYPE_DESCRIPTOR, &attr_info, &datapath_attr_mem[APP_DPS_ATTR_TYPE_TX_CCCD]);

    *p_attr_size = APP_DPS_ATTR_TYPE_MAX;

    return datapath_attr_list;
}

static uint16_t app_datapaths_alloc_con_info_with_connhdl(uint16_t connhdl)
{
    uint8_t idx = 0;

    for (idx = 0; idx < APP_DATAPATHS_CONN_MAX; idx++)
    {
        if (app_dp_server_env.con_info[idx].connectionHandle == connhdl ||
                app_dp_server_env.con_info[idx].connectionHandle == BT_INVALID_HANDLE)
        {
            app_dp_server_env.con_info[idx].connectionHandle = connhdl;
            return 0;
        }
    }

    return -1;
}

static uint16_t app_datapaths_free_con_info_by_connhdl(uint16_t connhdl)
{
    uint8_t idx = 0;

    for (idx = 0; idx < APP_DATAPATHS_CONN_MAX; idx++)
    {
        if (app_dp_server_env.con_info[idx].connectionHandle == connhdl)
        {
            app_dp_server_env.con_info[idx].connectionHandle = BT_INVALID_HANDLE;
            app_dp_server_env.con_info[idx].isNotificationEnabled = false;
            return 0;
        }
    }

    return -1;
}

static void app_datapaths_set_latest_recv_cmd_by_connhdl(uint16_t connhdl, uint32_t cmd)
{
    uint8_t idx = 0;

    for (idx = 0; idx < APP_DATAPATHS_CONN_MAX; idx++)
    {
        if (app_dp_server_env.con_info[idx].connectionHandle == connhdl)
        {
            app_dp_server_env.con_info[idx].recv_cmd_latest = cmd;
        }
    }
}

static uint8_t app_datapaths_get_latest_recv_cmd_by_connhdl(uint16_t connhdl)
{
    uint8_t idx = 0;

    for (idx = 0; idx < APP_DATAPATHS_CONN_MAX; idx++)
    {
        if (app_dp_server_env.con_info[idx].connectionHandle == connhdl)
        {
            return app_dp_server_env.con_info[idx].recv_cmd_latest;
        }
    }

    return 0;
}

static void app_datapaths_register_tx_ntf_cccd_by_connhdl(uint16_t connhdl, bool enable)
{
    uint8_t idx = 0;

    for (idx = 0; idx < APP_DATAPATHS_CONN_MAX; idx++)
    {
        if (app_dp_server_env.con_info[idx].connectionHandle == connhdl)
        {
            app_dp_server_env.con_info[idx].isNotificationEnabled = enable;
        }
    }
}

static bool app_datapaths_get_tx_ntf_en_by_connhdl(uint16_t connhdl)
{
    uint8_t idx = 0;

    for (idx = 0; idx < APP_DATAPATHS_CONN_MAX; idx++)
    {
        if (app_dp_server_env.con_info[idx].connectionHandle == connhdl)
        {
            return app_dp_server_env.con_info[idx].isNotificationEnabled;
        }
    }

    return false;
}

static void app_datapaths_connected(uint8_t conidx, uint16_t connhdl)
{
    uint16_t status = app_datapaths_alloc_con_info_with_connhdl(connhdl);

    if (status != 0)
    {
        return;
    }

    TRACE(0, "app datapath server connected.");

    app_datapaths_register_tx_ntf_cccd_by_connhdl(connhdl, true);

    if (NULL != app_dp_server_env.connected_done_callback)
    {
        app_dp_server_env.connected_done_callback(conidx);
    }

    if (app_dp_server_env.dp_event_callback)
    {
        app_dp_server_env.dp_event_callback(DP_CONN_DONE, (ble_if_app_dp_param_u *)&conidx);
    }
}

static void app_datapath_server_mtu_exchanged(uint8_t conidx, uint16_t connhdl, uint16_t mtu)
{
    if (NULL != app_dp_server_env.mtu_exchanged_done_callback)
    {
        app_dp_server_env.mtu_exchanged_done_callback(conidx, mtu);
    }

    if (app_dp_server_env.dp_event_callback)
    {
        app_dp_mtu_exchange_msg_t msg_data;
        msg_data.conidx = conidx;
        msg_data.mtu    = mtu;
        app_dp_server_env.dp_event_callback(DP_MTU_CHANGE_DONE, (ble_if_app_dp_param_u *)&msg_data);
    }
}

static void app_datapaths_disconnected(uint8_t conidx, uint16_t connhdl)
{
    if (app_datapaths_free_con_info_by_connhdl(connhdl) == 0)
    {
        TRACE(0, "app datapath server dis-connected.");
    }
    else
    {
        return;
    }

    if (NULL != app_dp_server_env.disconnected_done_callback)
    {
        app_dp_server_env.disconnected_done_callback(conidx);
    }

    if (app_dp_server_env.dp_event_callback)
    {
        app_dp_server_env.dp_event_callback(DP_DISCONN_DONE, (ble_if_app_dp_param_u *)&conidx);
    }
}

static void app_datapaths_tx_ccc_changed(uint8_t conidx, uint16_t connhdl, bool notify_enabled)
{
    if (notify_enabled)
    {
        app_datapaths_connected(conidx, connhdl);
    }
    else
    {
        app_datapaths_register_tx_ntf_cccd_by_connhdl(connhdl, false);
    }
}

static void app_datapaths_tx_data_sent(uint16_t connhdl, const uint32_t *dummy)
{
    TRACE(0, "%s connhdl = 0x%x, dummy = %d", __func__, connhdl, (uint32_t)dummy);

    if (NULL != app_dp_server_env.tx_done_callback)
    {
        app_dp_server_env.tx_done_callback();
    }

    if (app_dp_server_env.dp_event_callback)
    {
        app_dp_server_env.dp_event_callback(DP_TX_DONE, NULL);
    }
}

static void app_datapaths_rx_data_received(uint8_t conidx, uint16_t connhdl, const uint8_t *data, uint16_t len)
{
    // loop back the received data
    if (app_datapaths_get_tx_ntf_en_by_connhdl(connhdl))
    {
        app_datapath_server_send_data_via_notification(connhdl, (uint8_t *)data, len);
    }

    TRACE(2, "%s length %d", __func__, len);

    uint8_t cmd = *data;

    app_datapaths_alloc_con_info_with_connhdl(connhdl);
    app_datapaths_set_latest_recv_cmd_by_connhdl(connhdl, cmd);

    if (NULL != app_dp_server_env.rx_done_callback)
    {
        app_dp_server_env.rx_done_callback((uint8_t *)data, len);
    }

    if (app_dp_server_env.dp_event_callback)
    {
        app_dp_rec_data_msg_t data_msg;
        data_msg.data     = (uint8_t *)data;
        data_msg.data_len = len;
        data_msg.conidx   = conidx;
        app_dp_server_env.dp_event_callback(DP_DATA_RECEIVED, (ble_if_app_dp_param_u *)&data_msg);
    }
}

static void app_datapaths_read_latest_cmd_received(uint16_t connhdl, uint32_t token)
{
    uint8_t latest_cmd = app_datapaths_get_latest_recv_cmd_by_connhdl(connhdl);
    bta_gatts_send_read_rsp(connhdl, token, 0, (uint8_t *)&latest_cmd, sizeof(latest_cmd));
}

static void app_datapaths_send_desc_read_response(uint16_t connhdl, uint32_t token, bool is_tx_desc)
{
    uint8_t *buf = is_tx_desc ? (uint8_t *)custom_tx_desc : (uint8_t *)custom_rx_desc;
    uint16_t size = is_tx_desc ? sizeof(custom_tx_desc) : sizeof(custom_rx_desc);
    bta_gatts_send_read_rsp(connhdl, token, 0, buf, size);
}

static bool app_datapaths_callback(bt_gatt_svc_t *svc, bt_gatts_event_t event, bt_gatts_callback_param_t param)
{
    switch (event)
    {
        case BT_GATTS_EVENT_CHAR_WRITE:
        {
            bt_gatts_char_write_t *p = param.char_write;
            if (p->value_offset != 0 || p->value_len == 0 || p->value == NULL)
            {
                return false;
            }
            app_datapaths_rx_data_received(svc->con_idx, svc->connhdl, p->value, p->value_len);
            bta_gatts_send_write_rsp(svc->connhdl, p->token, 0);
            return true;
        }
        case BT_GATTS_EVENT_CHAR_READ:
        {
            bt_gatts_char_read_t *p = param.char_read;
            if (p->value_offset != 0)
            {
                return false;
            }

            app_datapaths_read_latest_cmd_received(svc->connhdl, p->token);
            return true;
        }
        case BT_GATTS_EVENT_DESC_WRITE:
        {
            bt_gatts_desc_write_t *p = param.desc_write;
            uint16_t config = *(uint16_t *)(p->value);
            bool notify_enabled = (config & BT_GATT_CCCD_NOTIFICATION) != 0;
            app_datapaths_tx_ccc_changed(svc->con_idx, svc->connhdl, notify_enabled);
            bta_gatts_send_write_rsp(svc->connhdl, p->token, 0);
            return true;
        }
        case BT_GATTS_EVENT_NTF_TX_DONE:
        case BT_GATTS_EVENT_INDICATE_CFM:
        {
            bt_gatts_indicate_cfm_t *p = param.confirm;
            app_datapaths_tx_data_sent(svc->connhdl, p->dummy);
            break;
        }
        case BT_GATTS_EVENT_MTU_CHANGED:
        {
            bt_gatts_mtu_changed_t *p = param.mtu_changed;
            app_datapath_server_mtu_exchanged(svc->con_idx, svc->connhdl, p->mtu);
            break;
        }
        case BT_GATTS_EVENT_CONN_CLOSED:
        {
            app_datapaths_disconnected(svc->con_idx, svc->connhdl);
            break;
        }
        case BT_GATTS_EVENT_DESC_READ:
        {
            bt_gatts_desc_read_t *p = param.desc_read;
            // Check cccd notify enable bit
            if (p->desc_attr->attr_data == app_datapaths_get_attribute(APP_DPS_ATTR_TYPE_TX_CCCD))
            {
                uint16_t cccd_config = app_datapaths_get_tx_ntf_en_by_connhdl(svc->connhdl) ? 0x0001 : 0x0000;
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0, (uint8_t *)&cccd_config, sizeof(cccd_config));
                return true;
            }

            bool is_tx_desc = (p->desc_attr->attr_data == app_datapaths_get_attribute(APP_DPS_ATTR_TYPE_TX_CUDD));
            app_datapaths_send_desc_read_response(svc->connhdl, p->token, is_tx_desc);
            return true;
        }
        default:
        {
            break;
        }
    }

    return 0;
}

void app_datapaths_init(void)
{
    const bt_gatt_attr_t *attr_list = NULL;
    uint8_t attr_list_len = 0;
    uint8_t idx = 0;

    if (app_dp_server_env.is_initilized == true)
    {
        return;
    }

    for (idx = 0; idx < APP_DATAPATHS_CONN_MAX; idx++)
    {
        app_dp_server_env.con_info[idx].connectionHandle = BT_INVALID_HANDLE;
        app_dp_server_env.con_info[idx].isNotificationEnabled = false;
    }

    attr_list = app_datapaths_build_attr_list(&attr_list_len);

    if (bta_gatts_register_service(attr_list, attr_list_len, app_datapaths_callback, NULL) == 0)
    {
        app_dp_server_env.is_initilized = true;
    }
}

void app_datapaths_deinit(void)
{
    if (app_dp_server_env.is_initilized == false)
    {
        return;
    }

    bta_gatts_unregister_service(app_datapaths_build_attr_list(NULL));

    app_dp_server_env.is_initilized = false;
}

void app_datapath_server_send_data_via_notification(uint16_t connhdl, uint8_t *data, uint32_t len)
{
    bt_gatt_char_notify_t val_ntf =
    {
        .service = app_datapaths_get_attribute(APP_DPS_ATTR_TYPE_SERVICE),
        .character = app_datapaths_get_attribute(APP_DPS_ATTR_TYPE_TX_CHARACTER),
        // To validate dummy callback
        .dummy = (uint32_t *)(uint32_t)data[0],
    };

    bta_gatts_send_character_value(connhdl, true, &val_ntf, data, (uint16_t)len);
}

void app_datapath_server_send_data_via_indication(uint16_t connhdl, uint8_t *data, uint32_t len)
{
    bt_gatt_char_notify_t val_ind =
    {
        .service = app_datapaths_get_attribute(APP_DPS_ATTR_TYPE_SERVICE),
        .character = app_datapaths_get_attribute(APP_DPS_ATTR_TYPE_TX_CHARACTER),
    };

    bta_gatts_send_character_value(connhdl, false, &val_ind, data, (uint16_t)len);
}

void app_datapath_server_register_event_callback(app_datapath_event_cb cb)
{
    app_dp_server_env.dp_event_callback = cb;
}

void app_datapath_server_register_event_cb(app_datapath_event_cb callback)
{
    app_datapath_server_register_event_callback(callback);
}

void app_datapath_server_register_tx_done(app_datapath_server_tx_done_t callback)
{
    app_dp_server_env.tx_done_callback = callback;
}

void app_datapath_server_register_rx_done(app_datapath_server_data_received_callback_func_t callback)
{
    app_dp_server_env.rx_done_callback = callback;
}

void app_datapath_server_register_disconnected_done(app_datapath_server_disconnected_done_t callback)
{
    app_dp_server_env.disconnected_done_callback = callback;
}

void app_datapath_server_register_connected_done(app_datapath_server_connected_done_t callback)
{
    app_dp_server_env.connected_done_callback = callback;
}

void app_datapath_server_register_mtu_exchanged_done(app_datapath_server_mtuexchanged_done_t callback)
{
    app_dp_server_env.mtu_exchanged_done_callback = callback;
}

void app_datapath_server_register_le_rs_callback(app_datapath_server_role_switch_callback_t callback)
{
    app_dp_server_env.le_rs_callback = callback;
}

#ifdef BLE_HOST_SUPPORT
extern "C" uint32_t ble_datapath_save_ctx(uint8_t conidx, uint8_t *buf, uint32_t buf_len)
{
    uint16_t offset = 0;
    uint8_t idx = 0;
    uint16_t *total_len = (uint16_t *)buf;

    TRACE(1, "%s conidx:%d", __func__, conidx);

    if (buf_len < 2 * sizeof(uint8_t) + sizeof(uint16_t))
    {
        TRACE(1, "%s no more ctx buf:%d", __func__, buf_len);
        return offset;
    }

    for (idx = 0; idx < BLE_CONNECTION_MAX; idx++)
    {
        if (app_dp_server_env.con_info[idx].connectionHandle ==
                bta_ble_get_conhdl_by_conidx(conidx))
        {
            break;
        }
    }

    *total_len = 0;
    offset += 2;

    if (idx < BLE_CONNECTION_MAX)
    {
        buf[offset] = app_dp_server_env.con_info[idx].recv_cmd_latest;
        offset += sizeof(app_dp_server_env.con_info[idx].recv_cmd_latest);
        buf[offset] = app_dp_server_env.con_info[idx].isNotificationEnabled;
        offset += sizeof(app_dp_server_env.con_info[idx].isNotificationEnabled);
    }
    else
    {
        buf[offset] = 0xFF;
        offset += sizeof(app_dp_server_env.con_info[idx].recv_cmd_latest);
    }

    *total_len = offset - 2;

#if 0
    // Custom save context callback
    if (le_rs_callback != NULL)
    {
        offset += le_rs_callback(conidx, false, buf + offset, buf_len - offset);
    }
#endif

    return offset;
}

extern "C" uint32_t ble_datapath_restore_ctx(uint8_t conidx, uint8_t *buf, uint32_t buf_len)
{
    uint16_t offset = 0;
    uint8_t is_notify_enabled = false;
    uint8_t cmd_code = 0;
    uint16_t total_len = 0;

    TRACE(1, "%s conidx:%d", __func__, conidx);

    if (buf_len < sizeof(uint8_t) + sizeof(total_len))
    {
        TRACE(1, "%s no more ctx buf:%d", __func__, buf_len);
        return 0;
    }

    total_len = *(uint16_t *)buf;
    offset += sizeof(total_len);

    if (total_len < sizeof(uint8_t))
    {
        TRACE(1, "%s invalid ctx buf:%d", __func__, total_len);
        return 0;
    }

    cmd_code = buf[offset];
    offset += sizeof(cmd_code);

    if (cmd_code != 0xFF)
    {
        uint16_t connhdl = bta_ble_get_conhdl_by_conidx(conidx);
        uint16_t status = app_datapaths_alloc_con_info_with_connhdl(connhdl);

        if (status == 0)
        {
            is_notify_enabled = buf[offset];
            offset += sizeof(is_notify_enabled);
            // Restore cmd latest write
            app_datapaths_set_latest_recv_cmd_by_connhdl(connhdl, cmd_code);
            // Callback upper in bthread
            app_datapaths_tx_ccc_changed(conidx, connhdl, is_notify_enabled);
        }
    }

#if 0
    // Custom restore context callback
    if (le_rs_callback != NULL)
    {
        offset += le_rs_callback(conidx, true, buf + offset, buf_len - offset);
    }
#endif

    return offset;
}
#endif

#ifdef __SW_IIR_EQ_PROCESS__
int audio_config_eq_iir_via_config_structure(uint8_t *buf, uint32_t  len);
void BLE_iir_eq_handler(uint32_t funcCode, uint8_t *ptrParam, uint32_t paramLen)
{

    audio_config_eq_iir_via_config_structure(BLE_custom_command_raw_data_buffer_pointer(), BLE_custom_command_received_raw_data_size());
}
#endif

#endif /* CFG_APP_DATAPATH_SERVER */
