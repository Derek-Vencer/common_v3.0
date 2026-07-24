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
#ifdef BLE_HOST_SUPPORT

#include "bta_ble_api.h"

#include "app_ble_mgr.h"

#include "hal_trace.h"

#define APP_BLE_MGR_SM_CFM_BY_UPPER         (0)

static struct app_ble_mgr_environment
{
    app_ble_mgr_adv_report_cb adv_report_cb;

} app_ble_mgr_env = {0};

static void app_ble_mgr_global_handler_ind(bt_ble_conn_evt_t *event, void *p_ret_val)
{
    if (BT_BLE_LINK_CONNECTED_EVENT == event->evt_type)
    {

    }
    else if (BT_BLE_DISCONNECT_EVENT  == event->evt_type)
    {

    }
    else if (BT_BLE_CONNECT_ENCRYPT_EVENT == event->evt_type)
    {

    }
    else if (BT_BLE_CONN_PARAM_UPDATE_REQ_EVENT == event->evt_type)
    {

    }
    else if (BT_BLE_RPA_ADDR_PARSED_EVENT == event->evt_type)
    {

    }
    else if (BT_BLE_SMP_PAIRING_REQ_EVENT == event->evt_type)
    {
#if (APP_BLE_MGR_SM_CFM_BY_UPPER == 1)
        ble_bdaddr_t *p_peer_addr = &(event->p.smp_pairing_request.peer_addr);

        bool *p_need_auto_cfm = (bool *)(p_ret_val);
        // Handle pairing request by upper
        *p_need_auto_cfm = false;

        bt_pairing_reqiure_t require =
        {
            .io_cap = BT_BLE_SMP_IO_DISPLAY_YES_NO,
            .has_oob_data = false,
            .auth_req = 0x29,
            .max_enc_key_size = 0x10,
            .init_key_dist = 0x0f,
            .resp_key_dist = 0x0f,
        };

        bta_ble_send_pairing_rsp(p_peer_addr, &require, 0);
#endif
    }
    else if (BT_BLE_CONNECT_NC_EXCH_EVENT == event->evt_type)
    {
#if (APP_BLE_MGR_SM_CFM_BY_UPPER == 1)
        ble_bdaddr_t *p_peer_addr = &(event->p.connect_nc_exch_handled.peer_addr);

        bool *p_need_auto_cfm = (bool *)(p_ret_val);
        // Handle pairing request by upper
        *p_need_auto_cfm = false;

        bt_ble_smp_input_t input =
        {
            .user_confirmed = true,
        };

        bta_ble_send_user_input_rsp(p_peer_addr, BT_BLE_SMP_NC_CMP_CONFIRM, &input);
#endif
    }
    else if (BT_BLE_PASSKEY_DISPLAY_EVENT == event->evt_type)
    {

    }
    else if (BT_BLE_SMP_PAIRING_CMP_EVENT == event->evt_type)
    {

    }
    else if (BT_BLE_SCAN_DATA_REPORT_EVENT == event->evt_type)
    {
        bt_scan_data_report_handled_t *adv_report = &(event->p.scan_data_report_handled);

        if (app_ble_mgr_env.adv_report_cb != NULL)
        {
            app_ble_mgr_env.adv_report_cb(&(adv_report->trans_addr), adv_report->rssi, adv_report->data,
                                          adv_report->length >= 0xFF ? adv_report->length : 0xFF);
        }
    }
}

void app_ble_mgr_init(uint16_t appearance)
{
    bta_ble_register_event_callback(app_ble_mgr_global_handler_ind);

    bt_ble_dev_cfg_t cfg =
    {
        .dev_appearance_uuid_le = appearance,
    };

    bta_ble_set_device_config(&cfg);
}

void app_ble_mgr_deinit(void)
{
    bta_ble_unregister_event_callback(app_ble_mgr_global_handler_ind);
}

void app_ble_mgr_scan_result_report_cb_register(app_ble_mgr_adv_report_cb adv_report_cb)
{
    app_ble_mgr_env.adv_report_cb = adv_report_cb;
}

#endif /* BLE_HOST_SUPPORT */