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

#pragma once
#include "earbud_profiles_api.h"
#include "bta_ble_api.h"

#include "nvrecord_extension.h"

#include "app_ibrt_conn_evt.h"

#ifdef BT_SVC_FW_PRODUCT_EARBUDS
#include "bta_tws_ux_api.h"
#else
#include "bta_normal_ux_api.h"
#endif

#define ibrt_role_e bt_ibrt_role_t
#define IBRT_MASTER BT_IBRT_MASTER
#define IBRT_SLAVE  BT_IBRT_SLAVE
#define IBRT_UNKNOW BT_IBRT_UNKNOWN

#define TWS_UI_ROLE_E bt_ui_role_t
#define TWS_UI_MASTER   BT_IBRT_MASTER
#define TWS_UI_SLAVE    BT_IBRT_SLAVE
#define TWS_UI_UNKNOWN  BT_IBRT_UNKNOWN

#define EAR_SIDE_UNKNOWN    BT_LOCATION_UNKNOWN
#define EAR_SIDE_LEFT       BT_LOCATION_LEFT
#define EAR_SIDE_RIGHT      BT_LOCATION_RIGHT
#define EAR_SIDE_NUM        2
#define APP_TWS_SIDE_T bt_location_t

#define ibrt_if_pnp_info bt_dip_pnp_info_t

#define SINGLE_POINT_MODE 1
#define DOUBLE_POINT_MODE 2
#define TRIPLE_POINT_MODE 3
#define multipoint_mode_t uint8_t

#define bud_box_state bta_tws_box_state_t
#define IBRT_BOX_UNKNOWN    BTA_TWS_BOX_UNKNOWN
#define IBRT_IN_BOX_CLOSED  BTA_TWS_IN_BOX_CLOSED
#define IBRT_IN_BOX_OPEN    BTA_TWS_IN_BOX_OPEN
#define IBRT_OUT_BOX        BTA_TWS_OUT_BOX
#define IBRT_OUT_BOX_WEARED BTA_TWS_OUT_BOX_WEARED

#define btif_accessible_mode_t bt_access_mode_t

#define APP_IBRT_IF_LINK_STATUS_CHANGED_CALLBACK ibrt_link_status_changed_cb_t

typedef struct
{
    void (*ibrt_global_state_changed)(ibrt_global_state_change_event *state);
    void (*ibrt_a2dp_state_changed)(const bt_bdaddr_t *addr, ibrt_conn_a2dp_state_change *state);
    void (*ibrt_hfp_state_changed)(const bt_bdaddr_t *addr, ibrt_conn_hfp_state_change *state);
    void (*ibrt_avrcp_state_changed)(const bt_bdaddr_t *addr, ibrt_conn_avrcp_state_change *state);
    void (*ibrt_tws_acl_state_changed)(ibrt_conn_tws_conn_state_event *state, uint8_t reason_code);
    void (*ibrt_mobile_acl_state_changed)(const bt_bdaddr_t *addr, ibrt_mobile_conn_state_event *state, uint8_t reason_code);
    void (*ibrt_sco_state_changed)(const bt_bdaddr_t *addr, ibrt_sco_conn_state_event *state, uint8_t reason_code);
#ifdef BT_SVC_FW_PRODUCT_EARBUDS
    void (*ibrt_tws_role_switch_status_ind)(const bt_bdaddr_t *addr, ibrt_conn_role_change_state state, ibrt_role_e role);
    void (*ibrt_ibrt_state_changed)(const bt_bdaddr_t *addr, ibrt_connection_state_event *state, ibrt_role_e role, uint8_t reason_code);
#endif
    void (*ibrt_access_mode_changed)(btif_accessible_mode_t newAccessMode);
} ibrt_link_status_changed_cb_t;

typedef struct
{
    bool (*accept_incoming_conn_req_hook)(const bt_bdaddr_t *addr, const uint8_t *cod);
    bool (*accept_extra_incoming_conn_req_hook)(const bt_bdaddr_t *incoming_addr, const uint8_t *cod, bt_bdaddr_t *steal_addr);
#ifdef BT_SVC_FW_PRODUCT_EARBUDS
    void (*set_page_para_hook)(bta_tws_page_para_t *page_para);
#endif
} ibrt_ext_conn_policy_cb_t;

typedef struct
{
    void (*ibrt_mgr_pairing_mode_entry_hook)();
    void (*ibrt_mgr_pairing_mode_exit_hook)();
#ifdef BT_SVC_FW_PRODUCT_EARBUDS
    void (*ibrt_mgr_tws_role_switch_comp_hook)(TWS_UI_ROLE_E current_role, uint8_t errCode);
    void (*peer_box_state_update_hook)(bud_box_state box_state);
#endif
} ibrt_mgr_status_changed_cb_t;

#ifdef __cplusplus
extern "C" {
#endif

void app_bta_earbuds_deprecated_init();

void app_ibrt_if_register_link_status_changed_client_callback(APP_IBRT_IF_LINK_STATUS_CHANGED_CALLBACK* cbs);

void app_ibrt_if_register_mgr_status_changed_client_callback(ibrt_mgr_status_changed_cb_t* cbs);

void app_ibrt_if_register_ext_conn_policy_client_callback(ibrt_ext_conn_policy_cb_t* cbs);

void app_ibrt_if_bt_set_local_dev_name(const uint8_t *dev_name, unsigned char len);

AppIbrtStatus app_ibrt_if_get_local_name(uint8_t* name_buf, uint8_t max_size);

void app_ibrt_if_write_bt_local_address(uint8_t* btAddr);

void app_ibrt_if_write_ble_local_address(uint8_t* bleAddr);

uint8_t *app_ibrt_if_get_bt_local_address(void);

uint8_t *app_ibrt_if_get_ble_local_address(void);

uint8_t *app_ibrt_if_get_bt_peer_address(void);

void app_ibrt_if_all_disconnect_request(void);

void app_ibrt_if_disconnect_all_bt_connections(void);

void app_ibrt_if_disconnect_mobile_device(const bt_bdaddr_t* remote_addr);

void app_ibrt_if_connect_mobile_device(const bt_bdaddr_t *addr, uint8_t page_count);

bool app_ibrt_if_is_any_mobile_connected(void);

uint8_t app_ibrt_if_get_connected_remote_dev_count(void);

uint8_t app_ibrt_if_get_connected_mobile_count(void);

uint8_t app_ibrt_if_get_mobile_connected_dev_list(bt_bdaddr_t *out_addrs);

void app_ibrt_if_enter_non_signalingtest_mode(void);

bool app_ibrt_if_is_earbud_in_pairing_mode(void);

void app_ibrt_if_enter_pairing_after_tws_connected(void);

bool app_ibrt_if_is_audio_active(uint8_t device_id);

void app_ibrt_if_nvrecord_update_ibrt_mode_tws(bool status);

int app_ibrt_if_nvrecord_get_latest_mobiles_addr(bt_bdaddr_t *mobile_addr1, bt_bdaddr_t* mobile_addr2);

bool app_ibrt_if_nvrecord_get_mobile_addr(bt_bdaddr_t mobile_addr_list[],uint8_t *count);

void app_ibrt_if_nvrecord_delete_all_mobile_record(void);

bool app_ibrt_if_nvrecord_get_mobile_paired_dev_list(nvrec_btdevicerecord *nv_record,uint8_t *count);

void app_ibrt_if_update_mobile_link_qos(uint8_t device_id, uint8_t tpoll_slot);

void app_ibrt_gma_exchange_ble_key();

ibrt_if_pnp_info *app_ibrt_if_get_pnp_info(const bt_bdaddr_t *remote);

void app_ibrt_if_set_afh_assess_en(bool status);

uint8_t app_ibrt_if_support_max_links(void);

bool app_ibrt_if_change_support_max_links_ext(multipoint_mode_t mode, const bt_bdaddr_t reserved_addrs[], int reserved_count);

int app_ibrt_if_exit_sniff(uint8_t* mobileAddr);

bool app_ibrt_if_get_active_device(bt_bdaddr_t* device);

#define app_ibrt_if_request_exit_mhdt_mode bta_request_exit_mhdt_mode

#define app_ibrt_if_request_enter_mhdt_mode bta_request_enter_mhdt_mode



#ifdef BT_SVC_FW_PRODUCT_EARBUDS

#define app_ibrt_if_is_in_freeman_mode bta_tws_is_freeman_mode_enabled

#define app_ibrt_if_is_tws_role_switching bta_tws_is_ui_role_switching

void app_ibrt_if_tws_role_switch_request(void);

void app_ibrt_if_customer_role_switch(void);

#define app_ibrt_if_get_ui_role bta_tws_get_ui_role

bool app_ibrt_if_is_left_side(void);

bool app_ibrt_if_is_right_side(void);

bool app_ibrt_if_unknown_side(void);

bool app_ibrt_if_is_ui_slave(void);

bool app_ibrt_if_is_ui_master(void);

const char* app_ibrt_if_uirole2str(TWS_UI_ROLE_E uiRole);

// 0 for MASTER, 1 for SLAVE, 3 for UNKNOWN
uint8_t app_ibrt_if_get_tws_current_bt_role(void);

#define app_ui_evt_t uint8_t
#define APP_UI_EV_CASE_OPEN     1
#define APP_UI_EV_CASE_CLOSE    2
#define APP_UI_EV_DOCK          3
#define APP_UI_EV_UNDOCK        4
#define APP_UI_EV_WEAR_UP       5
#define APP_UI_EV_WEAR_DOWN     6
#define APP_UI_EV_TWS_PAIRING   7
#define APP_UI_EV_FREE_MAN_MODE 8
void app_ibrt_if_event_entry(app_ui_evt_t event);

const char* app_ibrt_if_ui_event_to_string(app_ui_evt_t type);

void app_ibrt_if_enter_freeman_pairing(void);

void app_ibrt_if_start_tws_pairing(ibrt_role_e role, uint8_t* peerAddr);

void app_ibrt_if_update_tws_pairing_info(ibrt_role_e role, uint8_t* peerAddr);

void app_ibrt_if_init_open_box_state_for_evb(void);

#endif // BT_SVC_FW_PRODUCT_EARBUDS


#ifdef __cplusplus
}
#endif
