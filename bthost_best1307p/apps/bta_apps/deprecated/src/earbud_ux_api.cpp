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

#include "../inc/earbud_ux_api.h"

#include "cmsis_os2.h"

#include "apps.h"
#include "hal_trace.h"
#include "nvrecord_bt.h"
#include "nvrecord_env.h"
#include "hal_bootmode.h"
#include "bt_drv_reg_op.h"
#include "factory_section.h"

#include "app_audio_active_device_manager.h"

#ifdef __GMA_VOICE__
#include "gma_crypto.h"
#endif

#ifndef TRACE
#define TRACE(attr, str, ...)   TR_INFO(attr, str, ##__VA_ARGS__)
#endif

void app_ibrt_if_bt_set_local_dev_name(const uint8_t *dev_name, unsigned char len)
{
    bta_bt_set_local_name((const char *)dev_name, len);
}

AppIbrtStatus app_ibrt_if_get_local_name(uint8_t* name_buf, uint8_t max_size)
{
    bta_bt_get_local_name((char *)name_buf, max_size);
    return BTA_PROFILE_STATUS_SUCCESS;
}

void app_ibrt_if_write_bt_local_address(uint8_t* btAddr)
{
    bta_bt_set_local_addr((bt_bdaddr_t *)btAddr);
}

void app_ibrt_if_write_ble_local_address(uint8_t* bleAddr)
{
    bta_ble_set_local_addr((ble_bdaddr_t *)bleAddr);
}

uint8_t *app_ibrt_if_get_bt_local_address(void)
{
    static bt_bdaddr_t addr;
    bta_bt_get_local_addr(&addr);
    return addr.address;
}

uint8_t *app_ibrt_if_get_ble_local_address(void)
{
    static ble_bdaddr_t addr;
    bta_ble_get_local_addr(&addr);
    return addr.addr;
}

uint8_t *app_ibrt_if_get_bt_peer_address(void)
{
    struct nvrecord_env_t *nvrecord_env = NULL;
    nv_record_env_get(&nvrecord_env);
    return nvrecord_env->ibrt_mode.record.bdAddr.address;
}

bool app_ibrt_if_is_any_mobile_connected(void)
{
    return app_ibrt_if_get_connected_remote_dev_count();
}

uint8_t app_ibrt_if_get_connected_remote_dev_count(void)
{
    bt_bdaddr_t addrs[BT_DEVICE_NUM] = {};
    return app_ibrt_if_get_mobile_connected_dev_list(addrs);
}

uint8_t app_ibrt_if_get_connected_mobile_count(void)
{
    return app_ibrt_if_get_connected_remote_dev_count();
}

void app_ibrt_if_enter_non_signalingtest_mode(void)
{
    app_enter_non_signalingtest_mode();
}

bool app_ibrt_if_is_audio_active(uint8_t device_id)
{
    bt_bdaddr_t addr = {0};
    if (!bta_get_addr_by_device_id(device_id, &addr))
    {
        return false;
    }
    return bta_a2dp_is_streaming(&addr) || bta_hf_is_audio_link_connected(&addr);
}

void app_ibrt_if_nvrecord_update_ibrt_mode_tws(bool status)
{
    struct nvrecord_env_t *nvrecord_env = NULL;
    nv_record_env_get(&nvrecord_env);

    nvrecord_env->ibrt_mode.tws_connect_success = status;
    nv_record_env_set(nvrecord_env);
}

int app_ibrt_if_nvrecord_get_latest_mobiles_addr(bt_bdaddr_t *mobile_addr1, bt_bdaddr_t* mobile_addr2)
{
    nvrec_btdevicerecord* pNvRecord = NULL;
    int record_num = nv_record_get_paired_dev_list(&pNvRecord);
    if (record_num <= 0)
    {
        return 0;
    }

    bt_bdaddr_t addr = {0};
    bta_bt_get_local_addr(&addr);

    uint8_t device_num = 0;
    for (uint8_t index = 0; index < record_num && device_num < 2; ++index)
    {
        if (0 == memcmp(pNvRecord[index].record.bdAddr.address, addr.address, BT_BD_ADDR_LEN))
        {
            continue;
        }

        if (0 == device_num)
        {
            *mobile_addr1 = pNvRecord[index].record.bdAddr;
        }
        else if (1 == device_num)
        {
            *mobile_addr2 = pNvRecord[index].record.bdAddr;
        }
        device_num++;
    }

    return device_num;
}

bool app_ibrt_if_nvrecord_get_mobile_addr(bt_bdaddr_t mobile_addr_list[], uint8_t *count)
{
#if !defined(FPGA)
    nvrec_btdevicerecord* pNvRecord = NULL;
    int record_num = nv_record_get_paired_dev_list(&pNvRecord);
    if (record_num <= 0)
    {
        return false;
    }

    bt_bdaddr_t addr = {0};
    bta_bt_get_local_addr(&addr);

    uint8_t device_num = 0;
    for (uint8_t index = 0; index < record_num && device_num < BT_DEVICE_NUM; ++index)
    {
        if (0 == memcmp(pNvRecord[index].record.bdAddr.address, addr.address, BT_BD_ADDR_LEN))
        {
            continue;
        }

        mobile_addr_list[device_num] = pNvRecord[index].record.bdAddr;
        device_num++;
    }

    *count = device_num;
    return true;
#else
    unsigned char POSSIBLY_UNUSED stub_addr_1[6] = {0x6C, 0x04, 0x5E, 0x6C, 0x66, 0x5C};
    unsigned char POSSIBLY_UNUSED stub_addr_2[6] = {0x3E, 0x44, 0x82, 0x73, 0x4D, 0x6C};
    unsigned char POSSIBLY_UNUSED stub_addr_3[6] = {0x26, 0x88, 0x2D, 0x43, 0xE1, 0xBC};

    memcpy(mobile_addr_list[0].address, stub_addr_1, BT_BD_ADDR_LEN);
    memcpy(mobile_addr_list[1].address, stub_addr_2, BT_BD_ADDR_LEN);
    // memcpy(mobile_addr_list[2].address, stub_addr_3, BT_BD_ADDR_LEN);

    *count = 2;

    return true;
#endif
}

void app_ibrt_if_nvrecord_delete_all_mobile_record(void)
{
    nv_record_ddbrec_clear();
}

bool app_ibrt_if_nvrecord_get_mobile_paired_dev_list(nvrec_btdevicerecord *nv_record, uint8_t *count)
{
    nvrec_btdevicerecord* pNvRecord = NULL;
    int record_num = nv_record_get_paired_dev_list(&pNvRecord);
    if (record_num <= 0)
    {
        return false;
    }

    bt_bdaddr_t addr = {0};
    bta_bt_get_local_addr(&addr);

    uint8_t device_num = 0;
    for (uint8_t index = 0; index < record_num && device_num < BT_DEVICE_NUM; ++index)
    {
        if (0 == memcmp(pNvRecord[index].record.bdAddr.address, addr.address, BT_BD_ADDR_LEN))
        {
            continue;
        }

        nv_record[device_num] = pNvRecord[index];
        device_num++;
    }

    *count = device_num;
    return device_num;
}

void app_ibrt_if_update_mobile_link_qos(uint8_t device_id, uint8_t tpoll_slot)
{
    bt_bdaddr_t addr = {0};
    if (!bta_get_addr_by_device_id(device_id, &addr))
    {
        return;
    }
    bta_update_bt_device_qos(&addr, tpoll_slot);
}

ibrt_if_pnp_info *app_ibrt_if_get_pnp_info(const bt_bdaddr_t *remote)
{
    static bt_dip_pnp_info_t info;
    memset(&info, 0, sizeof(info));

    bta_dip_get_pnp_info(remote, &info);
    return &info;
}

void app_ibrt_if_set_afh_assess_en(bool status)
{
    bt_drv_reg_op_afh_assess_en(status);
}

int app_ibrt_if_exit_sniff(uint8_t* mobileAddr)
{
    bta_exit_sniff((bt_bdaddr_t *)mobileAddr);
    return 0;
}

bool app_ibrt_if_get_active_device(bt_bdaddr_t* device)
{
    BT_AUDIO_DEVICE_T* active_device = app_audio_adm_get_active_device();

    if (BT_INVALID_DEVICE_ID == active_device->device_id)
    {
        return false;
    }

    if (AUDIO_TYPE_LE_AUDIO == active_device->device_type)
    {
        ble_bdaddr_t addr = {0};
        bool status = bta_ble_get_addr_by_conidx(active_device->device_id, &addr);
        memcpy(device, addr.addr, BT_BD_ADDR_LEN);
        return status;
    }
    else if (AUDIO_TYPE_BT == active_device->device_type)
    {
        return bta_get_addr_by_device_id(active_device->device_id, device);
    }

    return false;
}

#ifdef BT_SVC_FW_PRODUCT_EARBUDS

void app_ibrt_if_all_disconnect_request(void)
{
    bta_tws_remove_all_devices();
    bta_tws_disconnect_tws_link();
}

void app_ibrt_if_disconnect_all_bt_connections(void)
{
    bta_tws_remove_all_bt_devices();
}

void app_ibrt_if_disconnect_mobile_device(const bt_bdaddr_t* remote_addr)
{
    bta_tws_remove_bt_device(remote_addr);
}

void app_ibrt_if_connect_mobile_device(const bt_bdaddr_t *addr, uint8_t page_count)
{
    bta_tws_connect_bt_device(addr, page_count);
}

uint8_t app_ibrt_if_get_mobile_connected_dev_list(bt_bdaddr_t *out_addrs)
{
    bt_bdaddr_t addrs[BT_DEVICE_NUM] = {};
    uint8_t num = bta_tws_find_all_local_bt_device(addrs);

    uint8_t count = 0;
    for (uint8_t index = 0; index < num; ++index)
    {
        if (bta_tws_get_bt_link_state(&addrs[index]) >= BTA_TWS_BT_CONNECTED ||
            bta_tws_get_ibrt_role(&addrs[index]) != BT_IBRT_UNKNOWN)
        {
            out_addrs[count] = addrs[index];
            ++count;
        }
    }

    return count;
}

bool app_ibrt_if_is_earbud_in_pairing_mode(void)
{
    return bta_tws_is_pairing_mode_enabled();
}

void app_ibrt_if_enter_pairing_after_tws_connected(void)
{
    bta_tws_enable_pairing_mode(true);
}

void app_ibrt_if_tws_role_switch_request(void)
{
    if (BT_IBRT_MASTER == bta_tws_get_ui_role())
    {
        bta_tws_request_ui_role_switch(BT_IBRT_SLAVE);
    }
    else
    {
        bta_tws_request_ui_role_switch(BT_IBRT_MASTER);
    }
}

void app_ibrt_if_customer_role_switch(void)
{
    app_ibrt_if_tws_role_switch_request();
}

bool app_ibrt_if_is_left_side(void)
{
    return bta_tws_get_location() == BT_LOCATION_LEFT;
}

bool app_ibrt_if_is_right_side(void)
{
    return bta_tws_get_location() == BT_LOCATION_RIGHT;
}

bool app_ibrt_if_unknown_side(void)
{
    return bta_tws_get_location() == BT_LOCATION_UNKNOWN;
}

bool app_ibrt_if_is_ui_slave(void)
{
    return bta_tws_get_ui_role() == BT_IBRT_SLAVE;
}

bool app_ibrt_if_is_ui_master(void)
{
    return bta_tws_get_ui_role() == BT_IBRT_MASTER;
}

void app_ibrt_if_enter_freeman_pairing(void)
{
    bta_tws_enable_freeman_mode(true);
}

const char* app_ibrt_if_uirole2str(TWS_UI_ROLE_E uiRole)
{
    switch (uiRole)
    {
    case BT_IBRT_MASTER:
        return "[TWS_UI_MASTER]";
    break;
    case BT_IBRT_SLAVE:
        return "[TWS_UI_SLAVE]";
    break;
    case BT_IBRT_UNKNOWN:
        return "[TWS_UI_UNKNOWN]";
    break;
    }

    return "[INVALID]";
}

uint8_t app_ibrt_if_get_tws_current_bt_role(void)
{
    if (bta_tws_get_sync_state() == BTA_TWS_DISCONNECTED)
    {
        return 3; // UNKNOWN
    }

    bt_bdaddr_t addr = {0};
    bta_bt_get_local_addr(&addr);

    if (bta_is_bt_central(&addr))
    {
        return 0; // MASTER
    }
    else
    {
        return 1; // SLAVE
    }
}

void app_ibrt_if_event_entry(app_ui_evt_t event)
{
    switch (event)
    {
    case APP_UI_EV_CASE_OPEN:
        bta_tws_box_event_entry(BTA_TWS_OPEN);
    break;
    case APP_UI_EV_CASE_CLOSE:
        bta_tws_box_event_entry(BTA_TWS_CLOSE);
    break;
    case APP_UI_EV_DOCK:
        bta_tws_box_event_entry(BTA_TWS_DOCK);
    break;
    case APP_UI_EV_UNDOCK:
        bta_tws_box_event_entry(BTA_TWS_UNDOCK);
    break;
    case APP_UI_EV_WEAR_UP:
        bta_tws_box_event_entry(BTA_TWS_WEAR_UP);
    break;
    case APP_UI_EV_WEAR_DOWN:
        bta_tws_box_event_entry(BTA_TWS_WEAR_DOWN);
    break;
    case APP_UI_EV_TWS_PAIRING:
        bta_tws_enable_pairing_mode(true);
    break;
    case APP_UI_EV_FREE_MAN_MODE:
        bta_tws_enable_freeman_mode(true);
    break;
    }
}

const char* app_ibrt_if_ui_event_to_string(app_ui_evt_t type)
{
    switch (type)
    {
    case APP_UI_EV_CASE_OPEN:
        return "CASE_OPEN";
    break;
    case APP_UI_EV_CASE_CLOSE:
        return "CASE_CLOSE";
    break;
    case APP_UI_EV_DOCK:
        return "DOCK";
    break;
    case APP_UI_EV_UNDOCK:
        return "UNDOCK";
    break;
    case APP_UI_EV_WEAR_UP:
        return "WEAR_UP";
    break;
    case APP_UI_EV_WEAR_DOWN:
        return "WEAR_DOWN";
    break;
    case APP_UI_EV_TWS_PAIRING:
        return "TWS_PAIRING";
    break;
    case APP_UI_EV_FREE_MAN_MODE:
        return "FREE_MAN_MODE";
    break;
    }

    return "Unknow CMD";
}

static osTimerId g_disconnection_checker;
static uint32_t g_disconnection_checker_counter;

static void disconnection_checker_timeout(void *param)
{
    if (g_disconnection_checker_counter >= 10)
    {
        osTimerStop(g_disconnection_checker);
        TRACE(0, "%s", __func__);
        return;
    }

    if (app_ibrt_if_is_any_mobile_connected() ||
        bta_tws_get_sync_state() > BTA_TWS_DISCONNECTED)
    {
        g_disconnection_checker_counter++;
    }
    else
    {
        osTimerStop(g_disconnection_checker);
        bta_tws_enable_pairing_mode(true);
    }
}

static void enable_pairing_after_all_connection_disconnected(void)
{
    if (NULL == g_disconnection_checker)
    {
        g_disconnection_checker = osTimerNew(disconnection_checker_timeout, osTimerPeriodic, NULL, NULL);
    }

    g_disconnection_checker_counter = 0;

    osTimerStart(g_disconnection_checker, 100);

    app_ibrt_if_all_disconnect_request();
}

void app_ibrt_if_start_tws_pairing(ibrt_role_e role, uint8_t *peerAddr)
{
    TRACE(0, "%s %d peer:", __func__, role);
    DUMP8("%02x ", peerAddr, BT_BD_ADDR_LEN);

    bta_tws_reconfig_nv_role(role, (bt_bdaddr_t *)peerAddr);

    enable_pairing_after_all_connection_disconnected();
}

void app_ibrt_if_update_tws_pairing_info(ibrt_role_e role, uint8_t* peerAddr)
{
    struct nvrecord_env_t *nvrecord_env = NULL;
    nv_record_env_get(&nvrecord_env);
    memset((uint8_t *)&(nvrecord_env->ibrt_mode), 0xff, sizeof(nvrecord_env->ibrt_mode));
    nv_record_env_set(nvrecord_env);

    bta_tws_reconfig_nv_role(role, (bt_bdaddr_t *)peerAddr);

    nv_record_flash_flush();
    hal_sw_bootmode_set(HAL_SW_BOOTMODE_CUSTOM_OP2_AFTER_REBOOT);
}

void app_ibrt_gma_exchange_ble_key()
{
#ifdef __GMA_VOICE__
    if (bta_tws_get_nv_role() == BT_IBRT_MASTER)
    {
        gma_secret_key_send();
    }
#endif
}

uint8_t app_ibrt_if_support_max_links(void)
{
    return bta_tws_get_device_num_max();
}

bool app_ibrt_if_change_support_max_links_ext(multipoint_mode_t mode, const bt_bdaddr_t reserved_addrs[], int reserved_count)
{
    bta_tws_set_device_num_max(mode, reserved_addrs, reserved_count);
    return true;
}

void app_ibrt_if_init_open_box_state_for_evb(void)
{
    bta_tws_box_event_entry(BTA_TWS_OPEN);
}

#else // BT_SVC_FW_PRODUCT_EARBUDS

void app_ibrt_if_all_disconnect_request(void)
{
    bta_remove_all_devices();
}

void app_ibrt_if_disconnect_all_bt_connections(void)
{
    bta_remove_all_devices();
}

void app_ibrt_if_disconnect_mobile_device(const bt_bdaddr_t* remote_addr)
{
    bta_remove_device(remote_addr);
}

void app_ibrt_if_connect_mobile_device(const bt_bdaddr_t *addr, uint8_t page_count)
{
    bta_page_policy_t policy =
    {
        .addrs = { *addr } 
    };
    bta_set_page_policy(&policy);
    bta_set_all_page_count(0, page_count);
}

uint8_t app_ibrt_if_get_mobile_connected_dev_list(bt_bdaddr_t *out_addrs)
{
    return bta_find_all_connected_bt_device((bt_bdaddr_list_t *)out_addrs);
}

bool app_ibrt_if_is_earbud_in_pairing_mode(void)
{
    return bta_is_pairing_mode_enabled();
}

void app_ibrt_if_enter_pairing_after_tws_connected(void)
{
    bta_enable_pairing_mode(true);
}

void app_ibrt_gma_exchange_ble_key()
{
#ifdef __GMA_VOICE__
    gma_secret_key_send();
#endif
}

uint8_t app_ibrt_if_support_max_links(void)
{
    return bta_get_device_num_max();
}

bool app_ibrt_if_change_support_max_links_ext(multipoint_mode_t mode, const bt_bdaddr_t reserved_addrs[], int reserved_count)
{
    bta_set_device_num_max(mode, reserved_addrs, reserved_count);
    return true;
}

#endif // BT_SVC_FW_PRODUCT_EARBUDS



static APP_IBRT_IF_LINK_STATUS_CHANGED_CALLBACK g_link_status_changed_callback;
void app_ibrt_if_register_link_status_changed_client_callback(APP_IBRT_IF_LINK_STATUS_CHANGED_CALLBACK* cbs)
{
    g_link_status_changed_callback = *cbs;
}

static void report_global_state_changed(ibrt_global_state_enum state)
{
    if (g_link_status_changed_callback.ibrt_global_state_changed)
    {
        ibrt_global_state_change_event event;
        memset(&event, 0, sizeof(event));
        event.state = state;
        g_link_status_changed_callback.ibrt_global_state_changed(&event);
    }
}

static void report_a2dp_state_changed(const bt_bdaddr_t *addr, ibrt_conn_a2dp_state state, uint8_t reason, bt_a2dp_codec_type_t codec,
    const bt_a2dp_unknown_cmd_t *cmd)
{
    if (g_link_status_changed_callback.ibrt_a2dp_state_changed)
    {
        ibrt_conn_a2dp_state_change event;
        memset(&event, 0, sizeof(event));
        event.a2dp_state = state;
        event.disc_reason = reason;
        event.addr = *addr;
        event.device_id = bta_get_device_id_by_addr(addr);
        event.cmd = (bt_a2dp_unknown_cmd_t*)cmd;
        // event.delay_report_support = ;
        event.audio_settings.codec = (ibrt_conn_codec_type)codec;
        g_link_status_changed_callback.ibrt_a2dp_state_changed(addr, &event);
    }
}

static void report_mobile_acl_state_changed(const bt_bdaddr_t *addr, ibrt_conn_acl_state state, uint8_t reason)
{
    if (g_link_status_changed_callback.ibrt_mobile_acl_state_changed)
    {
        ibrt_mobile_conn_state_event event;
        memset(&event, 0, sizeof(event));
        event.state.acl_state = state;
        event.state.bluetooth_reason_code = reason;
        event.addr = *addr;
        event.device_id = bta_get_device_id_by_addr(addr);
#ifdef BT_SVC_FW_PRODUCT_EARBUDS
        event.current_role = bta_tws_get_ibrt_role(addr);
#endif
        g_link_status_changed_callback.ibrt_mobile_acl_state_changed(addr, &event, reason);
    }
}

static void report_sco_state_changed(const bt_bdaddr_t *addr, ibrt_conn_sco_state state, uint8_t reason)
{
    if (g_link_status_changed_callback.ibrt_sco_state_changed)
    {
        ibrt_sco_conn_state_event event;
        memset(&event, 0, sizeof(event));
        event.state.sco_state = state;
        event.state.sco_reason_code = reason;
        event.addr = *addr;
        event.device_id = bta_get_device_id_by_addr(addr);
#ifdef BT_SVC_FW_PRODUCT_EARBUDS
        event.current_role = bta_tws_get_ibrt_role(addr);
#endif
        g_link_status_changed_callback.ibrt_sco_state_changed(addr, &event, reason);
    }
}

static void report_access_mode_changed(bt_access_mode_t access_mode)
{
    if (g_link_status_changed_callback.ibrt_access_mode_changed)
    {
        g_link_status_changed_callback.ibrt_access_mode_changed(access_mode);
    }
}

static ibrt_mgr_status_changed_cb_t g_mgr_status_changed_callback;
void app_ibrt_if_register_mgr_status_changed_client_callback(ibrt_mgr_status_changed_cb_t* cbs)
{
    g_mgr_status_changed_callback = *cbs;
}

static void report_pairing_mode_entry()
{
    if (g_mgr_status_changed_callback.ibrt_mgr_pairing_mode_entry_hook)
    {
        g_mgr_status_changed_callback.ibrt_mgr_pairing_mode_entry_hook();
    }
}

static void report_pairing_mode_exit()
{
    if (g_mgr_status_changed_callback.ibrt_mgr_pairing_mode_exit_hook)
    {
        g_mgr_status_changed_callback.ibrt_mgr_pairing_mode_exit_hook();
    }
}

static ibrt_ext_conn_policy_cb_t g_ext_conn_policy_callback;
void app_ibrt_if_register_ext_conn_policy_client_callback(ibrt_ext_conn_policy_cb_t* cbs)
{
    g_ext_conn_policy_callback = *cbs;
}

static void bt_bond_changed_handler(const bt_bdaddr_t *addr, uint8_t status)
{
    report_mobile_acl_state_changed(addr, IBRT_CONN_ACL_SIMPLE_PIARING_COMPLETE, status);
}

static void pairing_mode_changed_handler(bool enabled)
{
    if (enabled)
    {
        report_pairing_mode_entry();
    }
    else
    {
        report_pairing_mode_exit();
    }
}

static bool accept_connection_request_handler(const bt_bdaddr_t *addr, const uint8_t cod[3], bt_bdaddr_t *preempt)
{
    if (preempt)
    {
        if (g_ext_conn_policy_callback.accept_extra_incoming_conn_req_hook)
        {
            return g_ext_conn_policy_callback.accept_extra_incoming_conn_req_hook(addr, cod, preempt);
        }
    }
    else
    {
        if (g_ext_conn_policy_callback.accept_incoming_conn_req_hook)
        {
            return g_ext_conn_policy_callback.accept_incoming_conn_req_hook(addr, cod);
        }
    }

    return true;
}

static void a2dp_connection_state_changed_handler(const bt_bdaddr_t *address, bt_a2dp_conn_state_t state, uint8_t error_code)
{
    switch (state)
    {
        case BT_A2DP_CONN_STATE_DISCONNECTED:
            report_a2dp_state_changed(address, IBRT_CONN_A2DP_IDLE, error_code, BT_A2DP_CODEC_NONE, NULL);
        break;
        case BT_A2DP_CONN_STATE_CONNECTED:
            report_a2dp_state_changed(address, IBRT_CONN_A2DP_OPEN, error_code, BT_A2DP_CODEC_NONE, NULL);
        break;
    }
}

static void a2dp_audio_state_handler(const bt_bdaddr_t *address, bt_a2dp_audio_state_t state, uint8_t error_code)
{
    switch (state)
    {
        case BT_A2DP_AUDIO_STATE_CLOSED:
            report_a2dp_state_changed(address, IBRT_CONN_A2DP_CLOSED, error_code, BT_A2DP_CODEC_NONE, NULL);
        break;
        case BT_A2DP_AUDIO_STATE_SUSPEND:
            report_a2dp_state_changed(address, IBRT_CONN_A2DP_SUSPENED, error_code, BT_A2DP_CODEC_NONE, NULL);
        break;
        case BT_A2DP_AUDIO_STATE_STARTED:
            report_a2dp_state_changed(address, IBRT_CONN_A2DP_STREAMING, error_code, BT_A2DP_CODEC_NONE, NULL);
        break;
    }
}

static void a2dp_audio_config_handler(const bt_bdaddr_t *address, const bt_a2dp_audio_config_t *config)
{
    switch (config->codec)
    {
        case BT_A2DP_CODEC_NONE:
            TRACE(1, "%s: codec: %d - NONE", __func__, config->codec);
            break;
        case BT_A2DP_CODEC_SBC:
            TRACE(1, "%s: codec: %d - SBC", __func__, config->codec);
            bta_a2dp_set_delay(address, 150);
            break;
        case BT_A2DP_CODEC_AAC:
            TRACE(1, "%s: codec: %d - AAC", __func__, config->codec);
            bta_a2dp_set_delay(address, 150);
            break;
        case BT_A2DP_CODEC_LDAC:
            TRACE(1, "%s: codec: %d - LDAC", __func__, config->codec);
            bta_a2dp_set_delay(address, 240);
            break;
        default:
            TRACE(1, "%s: codec: %d", __func__, config->codec);
            bta_a2dp_set_delay(address, 150);
            break;
    }

    report_a2dp_state_changed(address, IBRT_CONN_A2DP_CODEC_CONFIGURED, 0, config->codec, NULL);
    TRACE(1, "a2dp_audio_config_handler:bit pool %d", config->specific_params.sbc.bitpool);
}

static void a2dp_unknown_cmd_handler(const bt_bdaddr_t *address, const bt_a2dp_unknown_cmd_t *param)
{
    report_a2dp_state_changed(address, IBRT_CONN_A2DP_UNKOWN_CMD, 0, BT_A2DP_CODEC_NONE, param);
}

static void hf_connection_state_handler(const bt_bdaddr_t *address, bt_hfp_conn_state_t state, uint8_t error_code)
{
    if (!g_link_status_changed_callback.ibrt_hfp_state_changed)
    {
        return;
    }

    ibrt_conn_hfp_state_change event;
    memset(&event, 0, sizeof(event));
    event.addr = *address;
    event.device_id = bta_get_device_id_by_addr(address);
    event.error_code = error_code;

    switch (state)
    {
        case BT_HFP_CONN_STATE_DISCONNECTED:
            event.hfp_state = IBRT_CONN_HFP_SLC_DISCONNECTED;
        break;
        case BT_HFP_CONN_STATE_CONNECTED:
            event.hfp_state = IBRT_CONN_HFP_SLC_OPEN;
        break;
    }
    g_link_status_changed_callback.ibrt_hfp_state_changed(address, &event);
}

static void hf_audio_status_handler(const bt_bdaddr_t *address, bt_hfp_audio_state_t state, bt_hfp_audio_codec_t codec, uint8_t error_code)
{
    if (!g_link_status_changed_callback.ibrt_hfp_state_changed)
    {
        return;
    }

    ibrt_conn_hfp_state_change event;
    memset(&event, 0, sizeof(event));
    event.addr = *address;
    event.device_id = bta_get_device_id_by_addr(address);
    event.error_code = error_code;

    switch (state)
    {
    case BT_HFP_AUDIO_STATE_DISCONNECTED:
        report_sco_state_changed(address, IBRT_CONN_SCO_DISCONNECTED, error_code);
        event.hfp_state = IBRT_CONN_HFP_SCO_CLOSED;
        event.codec = (ibrt_conn_codec_type)codec;
    break;
    case BT_HFP_AUDIO_STATE_CONNECTED:
        report_sco_state_changed(address, IBRT_CONN_SCO_CONNECTED, error_code);
        event.hfp_state = IBRT_CONN_HFP_SCO_OPEN;
        event.codec = (ibrt_conn_codec_type)codec;
    break;
    }
    g_link_status_changed_callback.ibrt_hfp_state_changed(address, &event);
}

static void hf_ring_indication_handler(const bt_bdaddr_t *address)
{
    if (!g_link_status_changed_callback.ibrt_hfp_state_changed)
    {
        return;
    }

    ibrt_conn_hfp_state_change event;
    memset(&event, 0, sizeof(event));
    event.addr = *address;
    event.device_id = bta_get_device_id_by_addr(address);
    event.hfp_state = IBRT_CONN_HFP_RING_IND;
    g_link_status_changed_callback.ibrt_hfp_state_changed(address, &event);
}

static void hf_call_status_handler(const bt_bdaddr_t *address, bt_hfp_call_state_t call)
{
    if (!g_link_status_changed_callback.ibrt_hfp_state_changed)
    {
        return;
    }

    ibrt_conn_hfp_state_change event;
    memset(&event, 0, sizeof(event));
    event.addr = *address;
    event.device_id = bta_get_device_id_by_addr(address);
    event.hfp_state = IBRT_CONN_HFP_CALL_IND;
    event.ciev_status = call;
    g_link_status_changed_callback.ibrt_hfp_state_changed(address, &event);
}

static void hf_callsetup_status_handler(const bt_bdaddr_t *address, bt_hfp_callsetup_state_t callsetup)
{
    if (!g_link_status_changed_callback.ibrt_hfp_state_changed)
    {
        return;
    }

    ibrt_conn_hfp_state_change event;
    memset(&event, 0, sizeof(event));
    event.addr = *address;
    event.device_id = bta_get_device_id_by_addr(address);
    event.hfp_state = IBRT_CONN_HFP_CALLSETUP_IND;
    event.ciev_status = callsetup;
    g_link_status_changed_callback.ibrt_hfp_state_changed(address, &event);
}

static void hf_callheld_status_handler(const bt_bdaddr_t *address, bt_hfp_callheld_state_t callheld)
{
    if (!g_link_status_changed_callback.ibrt_hfp_state_changed)
    {
        return;
    }

    ibrt_conn_hfp_state_change event;
    memset(&event, 0, sizeof(event));
    event.addr = *address;
    event.device_id = bta_get_device_id_by_addr(address);
    event.hfp_state = IBRT_CONN_HFP_CALLHELD_IND;
    event.ciev_status = callheld;
    g_link_status_changed_callback.ibrt_hfp_state_changed(address, &event);
}

static void hf_service_availability_handler(const bt_bdaddr_t *address, bt_hfp_service_avail_state_t state)
{
    if (!g_link_status_changed_callback.ibrt_hfp_state_changed)
    {
        return;
    }

    ibrt_conn_hfp_state_change event;
    memset(&event, 0, sizeof(event));
    event.addr = *address;
    event.device_id = bta_get_device_id_by_addr(address);
    event.hfp_state = IBRT_CONN_HFP_CIEV_SERVICE_IND;
    event.ciev_status = state;
    g_link_status_changed_callback.ibrt_hfp_state_changed(address, &event);
}

static void hf_signal_strength_handler(const bt_bdaddr_t *address, uint8_t signal_strength)
{
    if (!g_link_status_changed_callback.ibrt_hfp_state_changed)
    {
        return;
    }

    ibrt_conn_hfp_state_change event;
    memset(&event, 0, sizeof(event));
    event.addr = *address;
    event.device_id = bta_get_device_id_by_addr(address);
    event.hfp_state = IBRT_CONN_HFP_CIEV_SIGNAL_IND;
    event.ciev_status = signal_strength;
    g_link_status_changed_callback.ibrt_hfp_state_changed(address, &event);
}

static void hf_roaming_status_handler(const bt_bdaddr_t *address, bt_hfp_roam_state_t type)
{
    if (!g_link_status_changed_callback.ibrt_hfp_state_changed)
    {
        return;
    }

    ibrt_conn_hfp_state_change event;
    memset(&event, 0, sizeof(event));
    event.addr = *address;
    event.device_id = bta_get_device_id_by_addr(address);
    event.hfp_state = IBRT_CONN_HFP_CIEV_ROAM_IND;
    event.ciev_status = type;
    g_link_status_changed_callback.ibrt_hfp_state_changed(address, &event);
}

static void hf_battery_level_handler(const bt_bdaddr_t *address, uint8_t battery_level)
{
    if (!g_link_status_changed_callback.ibrt_hfp_state_changed)
    {
        return;
    }

    ibrt_conn_hfp_state_change event;
    memset(&event, 0, sizeof(event));
    event.addr = *address;
    event.device_id = bta_get_device_id_by_addr(address);
    event.hfp_state = IBRT_CONN_HFP_CIEV_BATTCHG_IND;
    event.ciev_status = battery_level;
    g_link_status_changed_callback.ibrt_hfp_state_changed(address, &event);
}

static void hf_spk_volume_change_handler(const bt_bdaddr_t *address, uint8_t volume)
{
    if (!g_link_status_changed_callback.ibrt_hfp_state_changed)
    {
        return;
    }

    ibrt_conn_hfp_state_change event;
    memset(&event, 0, sizeof(event));
    event.addr = *address;
    event.device_id = bta_get_device_id_by_addr(address);
    event.hfp_state = IBRT_CONN_HFP_SPK_VOLUME_IND;
    event.volume_ind = volume;
    g_link_status_changed_callback.ibrt_hfp_state_changed(address, &event);
}

static void hf_mic_volume_change_handler(const bt_bdaddr_t *address, uint8_t volume)
{
    if (!g_link_status_changed_callback.ibrt_hfp_state_changed)
    {
        return;
    }

    ibrt_conn_hfp_state_change event;
    memset(&event, 0, sizeof(event));
    event.addr = *address;
    event.device_id = bta_get_device_id_by_addr(address);
    event.hfp_state = IBRT_CONN_HFP_MIC_VOLUME_IND;
    event.volume_ind = volume;
    g_link_status_changed_callback.ibrt_hfp_state_changed(address, &event);
}

static void hf_in_band_ringtone_setting_handler(const bt_bdaddr_t *address, bt_hfp_in_band_ringtone_setting_t state)
{
    if (!g_link_status_changed_callback.ibrt_hfp_state_changed)
    {
        return;
    }

    ibrt_conn_hfp_state_change event;
    memset(&event, 0, sizeof(event));
    event.addr = *address;
    event.device_id = bta_get_device_id_by_addr(address);
    event.hfp_state = IBRT_CONN_HFP_IN_BAND_RING_IND;
    event.in_band_ring_enable = state;
    g_link_status_changed_callback.ibrt_hfp_state_changed(address, &event);
}

static void hf_voice_recognition_status_handler(const bt_bdaddr_t *address, bt_hfp_voice_recog_state_t state)
{
    if (!g_link_status_changed_callback.ibrt_hfp_state_changed)
    {
        return;
    }

    ibrt_conn_hfp_state_change event;
    memset(&event, 0, sizeof(event));
    event.addr = *address;
    event.device_id = bta_get_device_id_by_addr(address);
    event.hfp_state = IBRT_CONN_HFP_VR_STATE_IND;
    event.voice_rec_state = state;
    g_link_status_changed_callback.ibrt_hfp_state_changed(address, &event);
}

static void hf_query_current_calls_handler(const bt_bdaddr_t *address, int index, bt_hfp_curr_call_direction_t dir,
    bt_hfp_curr_call_state_t state, bt_hfp_curr_call_mrty_t mpty, const char *number)
{
    if (!g_link_status_changed_callback.ibrt_hfp_state_changed)
    {
        return;
    }

    ibrt_conn_hfp_state_change event;
    memset(&event, 0, sizeof(event));
    event.addr = *address;
    event.device_id = bta_get_device_id_by_addr(address);
    event.hfp_state = IBRT_CONN_HFP_CURRENT_CALL_STATE;

    event.curr_call.num_len = strlen(number);
    uint8_t len = MIN(event.curr_call.num_len, MAX_CLCC_NUM_SIZE);
    memcpy(event.curr_call.number, number, len);
    event.curr_call.number[len] = 0;
    g_link_status_changed_callback.ibrt_hfp_state_changed(address, &event);
}

static void hf_custom_at_cmd_handler(const bt_bdaddr_t *address, const char *data)
{
    if (!g_link_status_changed_callback.ibrt_hfp_state_changed)
    {
        return;
    }

    ibrt_conn_hfp_state_change event;
    memset(&event, 0, sizeof(event));
    event.addr = *address;
    event.device_id = bta_get_device_id_by_addr(address);
    event.hfp_state = IBRT_CONN_HFP_AT_RESULT_DATA;

    event.at_cmd_data = (char*)data;
    g_link_status_changed_callback.ibrt_hfp_state_changed(address, &event);
}

static void avrcp_connection_state_handler(const bt_bdaddr_t *address, bt_avrcp_conn_state_t state, uint8_t error_code)
{
    if (!g_link_status_changed_callback.ibrt_avrcp_state_changed)
    {
        return;
    }

    ibrt_conn_avrcp_state_change event;
    memset(&event, 0, sizeof(event));
    event.addr = *address;
    event.device_id = bta_get_device_id_by_addr(address);

    switch (state)
    {
        case BT_AVRCP_CONN_STATE_DISCONNECTED:
            event.avrcp_state = IBRT_CONN_AVRCP_DISCONNECTED;
            event.erro_reason = error_code;
            g_link_status_changed_callback.ibrt_avrcp_state_changed(address, &event);
        break;
        case BT_AVRCP_CONN_STATE_CONNECTED:
            event.avrcp_state = IBRT_CONN_AVRCP_CONNECTED;
            event.erro_reason = error_code;
            g_link_status_changed_callback.ibrt_avrcp_state_changed(address, &event);
        break;
    }
}

static void avrcp_recv_play_status_rsp(const bt_bdaddr_t *address, uint32_t song_length, uint32_t song_position, bt_avrcp_playback_status_t status)
{
    if (!g_link_status_changed_callback.ibrt_avrcp_state_changed)
    {
        return;
    }

    ibrt_conn_avrcp_state_change event;
    memset(&event, 0, sizeof(event));
    event.addr = *address;
    event.device_id = bta_get_device_id_by_addr(address);
    event.avrcp_state = IBRT_CONN_AVRCP_PLAY_STATUS_CHANGED;
    event.playback_status = status;
    event.play_position = song_position;
    event.play_length = song_length;
    g_link_status_changed_callback.ibrt_avrcp_state_changed(address, &event);
}

static void avrcp_recv_register_notification_rsp(const bt_bdaddr_t *address, bool changed, const bt_avrcp_notification_params_t *params)
{
    if (!g_link_status_changed_callback.ibrt_avrcp_state_changed)
    {
        return;
    }

    ibrt_conn_avrcp_state_change event;
    memset(&event, 0, sizeof(event));
    event.addr = *address;
    event.device_id = bta_get_device_id_by_addr(address);

    switch (params->event)
    {
        case BT_AVRCP_EVENT_VOL_CHANGED:
            event.avrcp_state = IBRT_CONN_AVRCP_VOLUME_UPDATED;
            event.volume = params->params.volume;
            g_link_status_changed_callback.ibrt_avrcp_state_changed(address, &event);
        break;
        case BT_AVRCP_EVENT_PLAY_STATUS_CHANGED:
            event.avrcp_state = IBRT_CONN_AVRCP_PLAYBACK_STATUS_CHANGED;
            event.playback_status = params->params.playback_status;
            g_link_status_changed_callback.ibrt_avrcp_state_changed(address, &event);
        break;
        case BT_AVRCP_EVENT_PLAY_POS_CHANGED:
            event.avrcp_state = IBRT_CONN_AVRCP_PLAY_POS_CHANGED;
            event.play_position = params->params.position;
            g_link_status_changed_callback.ibrt_avrcp_state_changed(address, &event);
        break;
        default:
        break;
    }
}

static void avrcp_recv_set_absolute_volume(const bt_bdaddr_t *address, uint8_t volume)
{
    if (!g_link_status_changed_callback.ibrt_avrcp_state_changed)
    {
        return;
    }

    ibrt_conn_avrcp_state_change event;
    memset(&event, 0, sizeof(event));
    event.addr = *address;
    event.device_id = bta_get_device_id_by_addr(address);
    event.avrcp_state = IBRT_CONN_AVRCP_VOLUME_UPDATED;
    event.volume = volume;
    g_link_status_changed_callback.ibrt_avrcp_state_changed(address, &event);
}

#ifdef BT_SVC_FW_PRODUCT_EARBUDS

static void report_tws_acl_state_changed(ibrt_conn_acl_state acl_state, uint8_t reason)
{
    if (g_link_status_changed_callback.ibrt_tws_acl_state_changed)
    {
        ibrt_conn_tws_conn_state_event event;
        memset(&event, 0, sizeof(event));
        event.state.acl_state = acl_state;
        event.state.bluetooth_reason_code = reason;
        event.current_role = bta_tws_get_ui_role();
        g_link_status_changed_callback.ibrt_tws_acl_state_changed(&event, reason);
    }
}

static void report_tws_role_switch(const bt_bdaddr_t *addr, bt_ibrt_role_t role)
{
    if (g_link_status_changed_callback.ibrt_tws_role_switch_status_ind)
    {
        g_link_status_changed_callback.ibrt_tws_role_switch_status_ind(
            addr, IBRT_CONN_ROLE_SWAP_COMPLETE, role);
    }
}

static void report_ibrt_state_changed(const bt_bdaddr_t *addr, ibrt_conn_ibrt_state state, bt_ibrt_role_t role, uint8_t reason)
{
    if (g_link_status_changed_callback.ibrt_ibrt_state_changed)
    {
        ibrt_connection_state_event event;
        memset(&event, 0, sizeof(event));
        event.state.ibrt_state = state;
        event.state.ibrt_reason_code = reason;
        event.addr = *addr;
        event.device_id = bta_get_device_id_by_addr(addr);
        event.current_role = role;
        g_link_status_changed_callback.ibrt_ibrt_state_changed(addr, &event, role, reason);
    }
}

static void report_ui_role_switch_comp(bt_ui_role_t role)
{
    if (g_mgr_status_changed_callback.ibrt_mgr_tws_role_switch_comp_hook)
    {
        g_mgr_status_changed_callback.ibrt_mgr_tws_role_switch_comp_hook(role, 0);
    }
}

static void report_peer_box_state_update(bta_tws_box_state_t box_state)
{
    if (g_mgr_status_changed_callback.peer_box_state_update_hook)
    {
        g_mgr_status_changed_callback.peer_box_state_update_hook(box_state);
    }
}

static void bt_link_state_changed_handler(const bt_bdaddr_t *addr, bta_tws_bt_link_event_t event, bt_ibrt_role_t role, uint8_t reason)
{
    switch (event)
    {
        case BTA_TWS_BT_DISCONNECTED_EVENT:
            report_mobile_acl_state_changed(addr, IBRT_CONN_ACL_DISCONNECTED, reason);
        break;
        case BTA_TWS_BT_CONNECTING_EVENT:
            report_mobile_acl_state_changed(addr, IBRT_CONN_ACL_CONNECTING, reason);
        break;
        case BTA_TWS_BT_CONNECTING_CANCELLED_EVENT:
            report_mobile_acl_state_changed(addr, IBRT_CONN_ACL_CONNECTING_CANCELED, reason);
        break;
        case BTA_TWS_BT_CONNECTING_FAILURE_EVENT:
            report_mobile_acl_state_changed(addr, IBRT_CONN_ACL_CONNECTING_FAILURE, reason);
        break;
        case BTA_TWS_BT_CONNECTED_EVENT:
            report_mobile_acl_state_changed(addr, IBRT_CONN_ACL_RAW_CONNECTED, reason);
        break;
        case BTA_TWS_BT_ENCRYPTED_EVENT:
            report_mobile_acl_state_changed(addr, IBRT_CONN_ACL_CONNECTED, reason);
            report_mobile_acl_state_changed(addr, IBRT_CONN_ACL_AUTH_COMPLETE, reason);
        break;
        case BTA_TWS_IBRT_DISCONNECTED_EVENT:
            report_ibrt_state_changed(addr, IBRT_CONN_IBRT_DISCONNECTED, role, reason);
        break;
        case BTA_TWS_IBRT_CONNECTED_EVENT:
            report_ibrt_state_changed(addr, IBRT_CONN_IBRT_ACL_CONNECTED, role, reason);
        break;
        case BTA_TWS_IBRT_ROLE_CHANGED_EVENT:
            report_tws_role_switch(addr, role);
        break;
    }
}

static void bt_profile_exchanged_hander(const bt_bdaddr_t *addr)
{
    bt_ibrt_role_t role = bta_tws_get_ibrt_role(addr);
    report_ibrt_state_changed(addr, IBRT_CONN_IBRT_CONNECTED, role, 0);
}

static void bt_subsys_state_changed_handler(bta_tws_subsys_state_t state)
{
    switch (state)
    {
        case BTA_TWS_SUBSYS_DISABLED:
            report_global_state_changed(IBRT_BLUETOOTH_DISABLED);
        break;
        case BTA_TWS_SUBSYS_ENABLED:
            report_global_state_changed(IBRT_BLUETOOTH_ENABLED);
        break;
    }
}

static void tws_sync_state_changed_handler(bta_tws_sync_state_t state, uint8_t reason)
{
    switch (state)
    {
        case BTA_TWS_DISCONNECTED:
            report_tws_acl_state_changed(IBRT_CONN_ACL_DISCONNECTED, reason);
        break;
        case BTA_TWS_SYNCING:
            report_tws_acl_state_changed(IBRT_CONN_ACL_CONNECTED, reason);
        break;
        case BTA_TWS_SYNCED:
            report_tws_acl_state_changed(IBRT_CONN_ACL_PROFILES_CONNECTED, reason);
        break;
    }
}

static void set_page_scan_param_handler(bta_tws_page_para_t *page_para)
{
    if (g_ext_conn_policy_callback.set_page_para_hook)
    {
        g_ext_conn_policy_callback.set_page_para_hook(page_para);
    }
}

#else // BT_SVC_FW_PRODUCT_EARBUDS

static void bt_acl_connection_state_handler(const bt_bdaddr_t *addr, bt_connection_state_t state, uint8_t error_code)
{
    switch (state)
    {
    case BT_CONN_STATE_DISCONNECTED:
        report_mobile_acl_state_changed(addr, IBRT_CONN_ACL_DISCONNECTED, error_code);
    break;
    case BT_CONN_STATE_CONNECTED:
        report_mobile_acl_state_changed(addr, IBRT_CONN_ACL_RAW_CONNECTED, error_code);
    break;
    }
}

static void bt_encrypt_changed_handler(const bt_bdaddr_t *addr, bool encrypted, uint8_t error_code)
{
    if (encrypted)
    {
        report_mobile_acl_state_changed(addr, IBRT_CONN_ACL_CONNECTED, error_code);
        report_mobile_acl_state_changed(addr, IBRT_CONN_ACL_AUTH_COMPLETE, error_code);
    }
}

static void bt_subsys_state_changed_handler(bta_subsys_state_t state)
{
    switch (state)
    {
        case BTA_SUBSYS_DISABLED:
            report_global_state_changed(IBRT_BLUETOOTH_DISABLED);
        break;
        case BTA_SUBSYS_ENABLED:
            report_global_state_changed(IBRT_BLUETOOTH_ENABLED);
        break;
    }
}

#endif // BT_SVC_FW_PRODUCT_EARBUDS

void app_bta_earbuds_deprecated_init()
{
#ifdef BT_SVC_FW_PRODUCT_EARBUDS

    const bta_tws_bt_link_state_changed_t bt_link_state_changed =
    {
        .bt_link_state_changed = bt_link_state_changed_handler,
        .bt_profile_exchanged = bt_profile_exchanged_hander,
    };
    bta_tws_register_bt_link_state_changed_hook(BTA_TWS_UX_USER_DEPRECATED, &bt_link_state_changed);

    const bta_tws_bt_callbacks_t bt_callbacks =
    {
        .bt_access_mode_changed = report_access_mode_changed,
        .bt_bond_changed = bt_bond_changed_handler,
    };
    bta_tws_register_bt_callbacks(BTA_TWS_UX_USER_DEPRECATED, &bt_callbacks);

    const bta_tws_ui_state_changed_t ui_state_changed =
    {
        .bt_subsys_state_changed = bt_subsys_state_changed_handler,
        .tws_sync_state_changed = tws_sync_state_changed_handler,
        .pairing_mode_changed = pairing_mode_changed_handler,
        .peer_box_state_changed = report_peer_box_state_update,
        .ui_role_switch_state_changed = report_ui_role_switch_comp,
    };
    bta_tws_register_ui_state_changed_hook(BTA_TWS_UX_USER_DEPRECATED, &ui_state_changed);

    bta_tws_ui_policy_callbacks_t ui_policy =
    {
        .accept_connection_request_callback = accept_connection_request_handler, 
        .set_page_scan_param_callback = set_page_scan_param_handler,
    };
    bta_tws_set_ui_policy_callbacks(&ui_policy);

#else // BT_SVC_FW_PRODUCT_EARBUDS

    static bt_callbacks_t bt_callbacks =
    {
        .access_mode_changed_cb = report_access_mode_changed,
        .acl_connection_state_cb = bt_acl_connection_state_handler,
        .encrypt_changed_cb = bt_encrypt_changed_handler,
        .bond_changed_cb = bt_bond_changed_handler,
    };
    bta_bt_register_callbacks(BT_CB_USER_DEPRECATED, &bt_callbacks);

    const bta_ui_state_changed_t ui_state_changed =
    {
        .bt_subsys_state_changed = bt_subsys_state_changed_handler,
        .pairing_mode_changed = pairing_mode_changed_handler,
    };
    bta_register_ui_state_changed_hook(BTA_UX_USER_DEPRECATED, &ui_state_changed);

    bta_set_accept_connection_callback(accept_connection_request_handler);

#endif // BT_SVC_FW_PRODUCT_EARBUDS

    static bt_a2dp_sink_callbacks_t a2dp_sink_callbacks =
    {
        .connection_state_cb = a2dp_connection_state_changed_handler,
        .audio_state_cb = a2dp_audio_state_handler,
        .audio_config_cb = a2dp_audio_config_handler,
        .cmd_req_cb      = a2dp_unknown_cmd_handler,
    };
    bta_a2dp_register_callbacks(BT_A2DP_SINK_CB_USER_DEPRECATED, &a2dp_sink_callbacks);

    static bt_hfp_hf_callbacks_t hf_callbacks =
    {
        .connection_state_cb = hf_connection_state_handler,
        .audio_status_cb = hf_audio_status_handler,
        .voice_recog_status_cb = hf_voice_recognition_status_handler,
        .service_availability_cb = hf_service_availability_handler,
        .signal_strength_cb = hf_signal_strength_handler,
        .roaming_status_cb = hf_roaming_status_handler,
        .battery_level_cb = hf_battery_level_handler,
        .call_status_cb = hf_call_status_handler,
        .callsetup_status_cb = hf_callsetup_status_handler,
        .callheld_status_cb = hf_callheld_status_handler,
        .current_calls_cb = hf_query_current_calls_handler,
        .spk_volume_change_cb = hf_spk_volume_change_handler,
        .mic_volume_change_cb = hf_mic_volume_change_handler,
        .in_band_ringtone_setting_cb = hf_in_band_ringtone_setting_handler,
        .ring_indication_cb = hf_ring_indication_handler,
        .custom_at_cmd_cb = hf_custom_at_cmd_handler,
    };
    bta_hf_register_callbacks(BT_HFP_HF_CB_USER_DEPRECATED, &hf_callbacks);

    static bt_avrcp_callbacks_t avrcp_callbacks =
    {
        .connection_state_cb = avrcp_connection_state_handler,
        .recv_play_status_cb = avrcp_recv_play_status_rsp,
        .set_absolute_volume_cb = avrcp_recv_set_absolute_volume,
        .notification_rsp_cb = avrcp_recv_register_notification_rsp,
    };
    bta_avrcp_register_callbacks(BT_AVRCP_CB_USER_DEPRECATED, &avrcp_callbacks);

}
