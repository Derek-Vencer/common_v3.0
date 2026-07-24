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
#ifndef __APP_UI_API_H__
#define __APP_UI_API_H__
#include "app_ibrt_conn_evt.h"
#include "app_tws_ibrt.h"
#include "app_ui_evt.h"
#include "app_ui_ipscan_mgr.h"
#include "bts_tws_types.h"
#include "bta_tws_attributes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    /****MOBILE_RECONNECT_TYPE****/
    APP_UI_MOBILE_INTERNAL_RECONNECT,        //SDK Unified logic
    APP_UI_MOBILE_MASTER_CHOICE_CONNECT,     //connect by Choice_Action master side
    APP_UI_MOBILE_LINK_LOSS_RECONNCET,       //connect by 08 Superrange
    APP_UI_MOBILE_OPEN_BOX_RECONNCET,        //connect by open_box
    APP_UI_MOBILE_RS_CONTINUE_RECONNECT,     //new_master_Inheritance_slave_try_reconncet_times_reconncet
    APP_UI_MOBILE_IBRT_COMPLETED_CONTINUE_RECONNECT,    //First, establish the Ibrt, cancel other devices, and after the cancellation is completed, restore the connection of other devices

    /****TWS_RECONNECT_TYPE****/
    APP_UI_TWS_OPEN_BOX_RECONNCET,          //connect by open_box
    APP_UI_TWS_LINK_LOSS_RECONNCET,         //connect by 08 Superrange

    APP_UI_TWS_RECONNECT_NONE               //will be delte The future
} app_ui_reconn_type_t;


typedef struct {
    bt_bdaddr_t          addr;               // Address of the device to page.            // Reconnet reason. LINK_LOSS is for LINK LOSS(08), TRY_RECONNECT is for OPEN, others is invalid.
    uint16_t             page_times;         // The times of page action about this device.
    uint32_t             page_timeout;       // Should be assigned a value in callback. Page timeout of this page(unit is 1 slot[0.625ms]).
    uint32_t             time_to_next_page;  // Should be assigned a value in callback. Idle time between this page and next page (unit is 1ms).
    app_ui_reconn_type_t reconnect_type;
} app_ui_page_para_t;

typedef struct {
    void (*ibrt_global_state_changed)(ibrt_global_state_change_event *state);
    void (*ibrt_a2dp_state_changed)(const bt_bdaddr_t *addr, ibrt_conn_a2dp_state_change *state);
    void (*ibrt_hfp_state_changed)(const bt_bdaddr_t *addr, ibrt_conn_hfp_state_change *state);
    void (*ibrt_avrcp_state_changed)(const bt_bdaddr_t *addr, ibrt_conn_avrcp_state_change *state);
    void (*ibrt_tws_pairing_changed)(ibrt_conn_pairing_state state, uint8_t reason_code);
    void (*ibrt_tws_acl_state_changed)(ibrt_conn_tws_conn_state_event *state, uint8_t reason_code);
    void (*ibrt_mobile_acl_state_changed)(const bt_bdaddr_t *addr, ibrt_mobile_conn_state_event *state, uint8_t reason_code);
    void (*ibrt_sco_state_changed)(const bt_bdaddr_t *addr, ibrt_sco_conn_state_event *state, uint8_t reason_code);
    void (*ibrt_tws_role_switch_status_ind)(const bt_bdaddr_t *addr, ibrt_conn_role_change_state state, bt_ibrt_role_t role);
    void (*ibrt_ibrt_state_changed)(const bt_bdaddr_t *addr, ibrt_connection_state_event *state, bt_ibrt_role_t role, uint8_t reason_code);
    void (*ibrt_access_mode_changed)(btif_accessible_mode_t newAccessMode);
} ibrt_link_status_changed_cb_t;

typedef struct {
    bool (*accept_incoming_conn_req_hook)(const bt_bdaddr_t *addr, const uint8_t *cod);
    bool (*accept_extra_incoming_conn_req_hook)(const bt_bdaddr_t *incoming_addr, const uint8_t *cod, bt_bdaddr_t *steal_addr);
    bool (*disallow_start_reconnect_mob_hook)(const bt_bdaddr_t *addr, const uint16_t active_event);
    bool (*disallow_start_reconnect_tws_hook)(void);
    bool (*disallow_tws_role_switch_hook)(void);
    void (*set_page_para_hook)(app_ui_page_para_t *page_para);
} ibrt_ext_conn_policy_cb_t;

typedef struct {
    void (*ibrt_mgr_event_run_comp_hook)(app_ui_evt_t box_evt);
    void (*ibrt_mgr_pairing_mode_entry_hook)();
    void (*ibrt_mgr_pairing_mode_exit_hook)();
    void (*ibrt_mgr_pairing_mode_timeout_pre_hook)();
    void (*ibrt_mgr_pairing_mode_timeout_post_hook)();
    void (*ibrt_mgr_tws_role_switch_comp_hook)(bt_ui_role_t current_role, uint8_t errCode);
    void (*peer_box_state_update_hook)(bud_box_state box_state);
    void (*local_box_state_update_hook)(bud_box_state box_state);
    void (*pre_handle_box_event_hook)(app_ui_evt_t box_evt);
} ibrt_mgr_status_changed_cb_t;

typedef void (*ibrt_vender_event_handler_ind)(uint8_t, uint8_t *, uint8_t);

typedef void (*notify_btmob_sm_destroyed_t)(const bt_bdaddr_t *addr);

typedef enum
{
    TWS_CHNL_USER_EARBUD,
    TWS_CHNL_USER_SOUND,
    TWS_CHNL_USER_MAX,
} tws_chnl_user_t;

typedef enum
{
    PRIO_DISCONNECTED,
    PRIO_CONENCTING,
    PRIO_CONNECTED,
} prio_state_t;

typedef void (*tws_chnl_recv_func_t)(uint8_t *, uint16_t);

void app_ui_init();

bta_tws_attributes_t* app_ui_get_config();

void *app_ui_get_devices_ctx(uint8_t index);

void app_ui_custom_role_switch_cb_ind(const bt_bdaddr_t *addr, ibrt_conn_role_change_state state, bt_ibrt_role_t role);

void app_ui_exit_pairing_mode_ext(bool notify_peer);

int app_ui_reconfig_env(const bta_tws_attributes_t *config);

void *app_ui_get_box_evt_que(void);

void app_ui_push_box_evt_into_queue(const void* user_action_evt);

bool app_ui_sync_box_state(bud_box_state box_state);

void app_ui_dump_status();

void app_ui_monitor_dump(void);

bud_box_state app_ui_get_local_box_state(void);

void app_ui_set_local_box_state(bud_box_state state);

bud_box_state app_ui_get_peer_box_state(void);

void app_ui_set_peer_box_state(bud_box_state box_state);

void app_ui_update_scan_type_policy(scanUpdateEvent event);

void app_ui_send_tws_reconnect_event(app_ui_evt_t reconect_event);

uint8_t app_ui_get_connected_remote_dev_count();

void app_ui_choice_mobile_connect(const bt_bdaddr_t *mobile_addr, uint8_t max_try_count);

bool app_ui_any_mobile_device_connected(void);

bool app_ui_max_mobile_device_connected(void);

bool app_ui_allow_steal_connection();

void app_ui_set_accepting_device(const bt_bdaddr_t *addr);

bool app_ui_accept_conn_req_ongoing();

bool app_ui_is_accepting_device(const bt_bdaddr_t *addr);

void app_ui_clr_pending_incoming_conn_req(void);

bool app_ui_accept_pending_incoming_conn_req(bt_bdaddr_t *addr);

void app_ui_register_link_status_callback(ibrt_link_status_changed_cb_t *cb);

void app_ui_register_mgr_status_callback(ibrt_mgr_status_changed_cb_t *cb);

void app_ui_register_ext_conn_policy_callback(ibrt_ext_conn_policy_cb_t *cb);

void app_ui_register_local_stream_callback(bool(*cb)(const bt_bdaddr_t *addr, uint8_t state));

void app_ui_register_vender_event_update_ind(ibrt_vender_event_handler_ind handler);

void best_mgr_pairing_mode_entry_hook();

void best_mgr_pairing_mode_exit_hook();

void best_mgr_pairing_mode_timeout_pre_hook();

void best_mgr_pairing_mode_timeout_post_hook();

void best_mgr_tws_role_switch_comp_hook(bt_ui_role_t current_role, uint8_t errCode);

bool app_ui_is_addr_null(bt_bdaddr_t *addr);

bool app_ui_pop_from_pending_list(bt_bdaddr_t *addr);

bool app_ui_get_from_pending_list(bt_bdaddr_t *addr);

void app_ui_push_device_to_pending_list(bt_bdaddr_t *addr);

uint16_t app_ui_pending_conn_req_list_size(void);

bool app_ui_event_has_been_queued(const bt_bdaddr_t* remote_addr,app_ui_evt_t event);

app_ui_evt_t app_ui_get_active_event(const bt_bdaddr_t* remote_addr);

bool app_ui_high_priority_event_interrupt_reconnec(uint16_t link_id);

bool app_ui_disallow_reconnect_mobile_by_peer_status(void);

bool app_ui_notify_peer_to_destroy_device(const bt_bdaddr_t *addr, bool delete_record);

bool app_ui_destroy_device_ongoing(void);

bool app_ui_destroy_device(const bt_bdaddr_t *del_nv_addr, bool delete_record);

void app_ui_destroy_the_other_device(const bt_bdaddr_t *active_addr, bool need_delete_nv);

void app_ui_map_valid_device2nvrecord(void);

void app_ui_load_device_from_record_when_no_instance(void);

bool app_ui_has_available_mobile_resource(void);

void app_ui_keep_only_mobile_link();

ibrt_status_t app_ui_dev_connect_ibrt_link(const bt_bdaddr_t *addr);

void app_ui_set_notify_btmob_sm_destroyed(notify_btmob_sm_destroyed_t callback);

void app_ui_btmob_sm_update_priority(const bt_bdaddr_t *addr, prio_state_t prio_state);

/**
 ****************************************************************************************
 * @brief To disconnect all connections and then shutdown the system
 *
 ****************************************************************************************
 */
void app_ui_shutdown(void);

void app_ui_set_enter_pairing_flag(void);

void app_ui_clr_enter_pairing_flag(void);

bool app_ui_is_enter_pairing_mode(void);

void app_ui_pairing_ctl_init(void);

/**
 ****************************************************************************************
 * @brief To get current pairing mode state
 *
 * @return true                     In pairing mode
 * @return false                    Not in pairing mode
 ****************************************************************************************
 */
bool app_ui_in_pairing_mode(void);

bool app_ui_is_dev_pairing_state(void);

void app_ui_update_dev_pairing_state(bool state);

void app_ui_register_ui_secure_request_callback(bool (*cb)(void *bdaddr, uint8_t local_initiate, bool is_bt_acl, bool conn_req));

/**
 ****************************************************************************************
 * @brief Enter pairing mode
 *
 * @param timeout                   Pairing timeout value
 * @param notify_peer               If true, notify peer enter pairing
 ****************************************************************************************
 */
void app_ui_enter_pairing_mode(uint32_t timeout, bool notify_peer);

/**
 ****************************************************************************************
 * @brief Exit pairing mode
 *
 * @param notify_peer               If true, notify peer exit pairing
 ****************************************************************************************
 */
void app_ui_exit_pairing_mode(bool notify_peer);

void app_ui_pairing_evt_handler(pairing_evt_e evt, uint32_t para);

bool app_ui_allow_start_ibrt_in_pairing(void);

bool app_ui_is_both_sides_reload_complete(void);

bool app_ui_support_bleaudio(void);

void app_ui_cancel_all_connecting_bt_device(void);

bool app_ui_support_multipoint(void);

/**
 ****************************************************************************************
 * @brief role switch > ibrt role-switch
 *
 * @param[in] switch2master         if true will switch to master else to slave
 ****************************************************************************************
 */
bool app_ui_user_role_switch(bool switch2master);

/**
 ****************************************************************************************
 * @brief user get is any connected device doing role switch
 *
 ****************************************************************************************
 */
bool app_ui_role_switch_ongoing(void);

/**
 ****************************************************************************************
 * @brief trigger role switch by local and peer box state
 *
 ****************************************************************************************
 */
void app_ui_trigger_role_switch_by_box_state(void);

/* TODO: will be removed */
bool app_ui_change_mode(bool enable_bleaudio, bool enable_multipoint, const bt_bdaddr_t *addr);

/* TODO: will be removed */
bool app_ui_change_mode_ext(bool enable_leaudio, bool enable_multipoint, const bt_bdaddr_t *addr);

uint8_t app_ui_support_max_links();

bool app_ui_change_support_max_links_ext(multipoint_mode_t mode, const bt_bdaddr_t reserved_addrs[], int reserved_count);
/**
 ****************************************************************************************
 * @brief enable/disable page function
 *
 * @param enable                    If true, enable page; else, disable page
 ****************************************************************************************
 */
void app_ui_set_page_enabled(bool enable);

/**
 ****************************************************************************************
 * @brief Get current enable or disable page
 *
 * @return true                     Enable page
 * @return false                    Disable Page
 ****************************************************************************************
 */
bool app_ui_enabled_page(void);

bool app_ui_need_delay_mob_reconn();

bool app_ui_custom_disallow_reconn_dev(const bt_bdaddr_t *addr, uint16_t active_event);

bool app_ui_custom_disallow_reconn_tws(void);

void app_ui_custom_set_page_para(const bt_bdaddr_t *addr, app_ui_reconn_type_t reconnect_type, uint16_t page_times, uint32_t *page_timeout, uint32_t *time_to_next_page);

/**
 ****************************************************************************************
 * @brief exit eabud mode for enter other mode
 *
 * @return None
 ****************************************************************************************
 */
void app_ui_exit_earbud_mode(void);

/**
 * @brief Get remote BT name
 *
 * @param remote_addr           - [in]  Remote device BT address
 * @param nameStr               - [out] name string pointer
 * @param nameLen               - [out] name length pointer
 * @return true                 - Success
 * @return false                - Failed
 */
bool app_ui_get_remote_name(const bt_bdaddr_t *remote_addr, char *nameStr, uint8_t *nameLen);

bt_bdaddr_t* app_ui_get_peer_tws_device_address();
/**
 * @brief Cancel Page
 *
 * @param addr                - Cancel page mobile addr or tws addr
 */
void app_ui_cancel_page(bt_bdaddr_t *addr);

/**
 * @brief is disconnect event
 *
 * @param evt                - event
 */
bool app_ui_is_disconnect_evt(app_ui_evt_t evt);

/**
 * @brief is reconnect event
 *
 * @param evt                - event
 */
bool app_ui_is_reconnect_evt(app_ui_evt_t evt);

/**
 * @brief put the device at the top of NV record
 *
 * @param device address
 */
 void app_ui_put_dev_at_nv_top(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief Except the TWS earbud, the other project without open/close box, dock/undock,
 * wear up/down, is suggested to use the following interfaces:
 * 1. enable relevant interface
 * 2. start access management
 * 3. start reconnect the latest one/two mobile by config
 ****************************************************************************************
 */
bool app_ui_bt_open();

/**
 ****************************************************************************************
 * @brief Except the TWS earbud, the other project without open/close box, dock/undock,
 * wear up/down, is suggested to use the following interfaces:
 * 1. disable access
 * 2. disconnect all bt connections
 * 3. disable relevant interface
 ****************************************************************************************
 */
bool app_ui_bt_close();

void app_ui_start_reconnect_all_mobile(uint8_t page_count);

/**
 * @brief send msg(event) to btmgr
 *
 * @param e                     - event
 */
bool best_btmgr_send_msg(app_ui_evt_t e);


bool best_btmgr_tws_chnl_register(tws_chnl_user_t user, tws_chnl_recv_func_t user_handle);

bool best_btmgr_tws_chnl_send_data(tws_chnl_user_t user, uint8_t *data_buf, uint16_t data_len);

void bt_update_pscan_para(uint16_t scan_interval, uint16_t scan_window);
void bt_update_iscan_para(uint16_t scan_interval, uint16_t scan_window);
void bt_set_pscan_enabled(bool enable);
void bt_set_iscan_enabled(bool enable);

#if BLE_AUDIO_ENABLED
#include "bluetooth_ble_api.h"

/**
 ****************************************************************************************
 * @brief Return true if any ble audio device connected
 ****************************************************************************************
 */
bool app_ui_any_ble_audio_links(void);

/**
 ****************************************************************************************
 * @brief Return true if any ble audio device connected
 *
 * @param[in] conidx                The LE connection index
 ****************************************************************************************
 */
void app_ui_keep_only_the_leaudio_device(uint8_t conidx);


// TODO:: will move to app_ui.h
/**
 ****************************************************************************************
 * @brief Return true if any ble audio device connected
 *
 * @param[in] conidx                The LE connection index
 ****************************************************************************************
 */
void app_ui_notify_bt_nv_recored_changed(bt_bdaddr_t* mobile_addr);

#endif

#ifdef __cplusplus
}
#endif
#endif /* __APP_UI_API_H__ */
