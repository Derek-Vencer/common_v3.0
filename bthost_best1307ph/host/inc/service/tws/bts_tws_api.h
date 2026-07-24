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
#ifndef __BTS_TWS_API_H__
#define __BTS_TWS_API_H__

#include "bts_tws_types.h"

typedef bool (*bts_sniff_params_callback_t)(btif_sniff_info_t *sniff_info);
/**
 ****************************************************************************************
 * @brief Get ibrt constate
 *
 * @param[in] addr       Mobile address
 *
 * @return ibrt constate
 * <table>
 * <th>Description   replace api "uint32_t app_ibrt_conn_get_ibrt_constate(void* addr)"
 * </table>
 ****************************************************************************************
 */
uint32_t bts_ibrt_if_get_ibrt_constate(void* addr);

/**
 ****************************************************************************************
 * @brief Initiate ibrt connection for mobile device
 *
 * @param[in] addr       Mobile address
 *
 * @return An error status
 * <table>
 * <th>Description   replace api "uint16_t app_tws_ibrt_get_ibrt_handle(const bt_bdaddr_t *p_bd_addr)"
 * </table>
 ****************************************************************************************
 */
uint16_t bts_ibrt_if_get_dev_ibrt_handle(const bt_bdaddr_t *p_bd_addr);

/**
 ****************************************************************************************
 * @brief Initiate ibrt connection for mobile device
 *
 * @param[in] addr       Mobile address
 *
 * @return An error status
 * <table>
 * <th>Description   replace api "ibrt_status_t app_ibrt_conn_connect_ibrt(const bt_bdaddr_t *addr)"
 * </table>
 ****************************************************************************************
 */
ibrt_status_t bts_ibrt_if_dev_connect_ibrt_link(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief Disconnect ibrt connection for mobile device
 *
 * @param[in] addr       Mobile address
 *
 * @return An error status
 * <table>
 * <th>Description   replace api "ibrt_status_t app_ibrt_conn_disconnect_ibrt(const bt_bdaddr_t *addr)"
 * </table>
 ****************************************************************************************
 */
ibrt_status_t bts_ibrt_if_dev_disconnect_ibrt_link(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief Check a2dp profile is exchanged
 *
 * @param[in] addr       remote address
 *
 * @return bool
 * <table>
 * <th>Description   replace api "bool app_ibrt_a2dp_profile_is_exchanged(const bt_bdaddr_t *addr)"
 * </table>
 ****************************************************************************************
 */
bool bts_ibrt_if_a2dp_profile_is_exchanged(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief Check hfp profile is exchanged
 *
 * @param[in] addr       remote address
 *
 * @return bool
 * <table>
 * <th>Description
 * </table>
 ****************************************************************************************
 */
bool bts_ibrt_if_hfp_profile_is_exchanged(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief Check if profile_exchanged for the mobile
 *
 * @param[in] addr       remote address
 *
 * @return
 * <table>
 * <tr><th>Value        <th>Description
 * <tr><td>True  <td>profile_exchanged
 * <tr><td>False  <td>profile_exchanged
 * </table>
 * <table>
 * <th>Description   replace api "bool app_ibrt_conn_is_profile_exchanged(const bt_bdaddr_t *addr)"
 * </table>
 ****************************************************************************************
 */
bool bts_ibrt_if_is_profile_exchanged(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief Get the number of local snoop mobile devices
 *
 * @return The number of local snoop mobile devices
 ****************************************************************************************
 */
uint8_t bts_ibrt_if_get_dev_ibrt_connected_count();

bool bts_ibrt_if_is_sync_a2dp_status_onprocess(const bt_bdaddr_t *bdaddr);

#ifdef __cplusplus
extern "C" {
#endif
/**
 ****************************************************************************************
 * @brief Check mobile ibrt snoop link is connected
 *
 * @param[in] addr       Mobile address
 *
 * @return bool
 * <table>
 * <th>Description   replace api "bool app_tws_ibrt_slave_ibrt_link_connected(const bt_bdaddr_t *p_mobile_addr)"
 * </table>
 ****************************************************************************************
 */
bool bts_ibrt_if_is_ibrt_link_connected(const bt_bdaddr_t *p_dev_addr);

#ifdef __cplusplus
}
#endif

/**
 ****************************************************************************************
 * @brief Check ibrt is idle
 *
 * @param[in] addr       remote address
 *
 * @return bool
 * <table>
 * <th>Description   replace api "bool app_ibrt_conn_is_ibrt_idle(const bt_bdaddr_t *addr)"
 * </table>
 ****************************************************************************************
 */

bool bts_ibrt_if_is_ibrt_idle(const bt_bdaddr_t *addr);

#ifdef __cplusplus
 extern "C" {
#endif

/**
 ****************************************************************************************
 * @brief Get tws addr
 *
 * @param[in] const uint8_t* pBdAddr    write tws addr point
 *
 * @return bool
 * <table>
 * <tr><th>Value        <th>Description
 * <tr><td>True   <td> get tws addr success
 * <tr><td>False  <td> get tws addr fail
 * </table>
 * <table>
 * <th>Description   replace api "bool app_ibrt_if_is_tws_addr(const uint8_t* pBdAddr)"
 * <th>Description   replace api "bool app_ibrt_internal_is_tws_addr(const uint8_t* pBdAddr)"
 * </table>
 ****************************************************************************************
 */
bool bts_tws_if_is_tws_addr(const uint8_t* pBdAddr);

/**
 ****************************************************************************************
 * @brief Check if current tws nv role is master
 *
 * @return
 * <table>
 * <tr><th>Value        <th>Description
 * <tr><td>True  <td>current nv role is master
 * <tr><td>False  <td>current nv role not master
 * </table>
 * <table>
 * <th>Description   replace api "bool app_ibrt_conn_is_nv_master(void)"
 * <th>Description   replace api "bool app_ibrt_if_is_nv_master(void)"
 * <th>Description   replace api "bool app_ibrt_middleware_is_nv_master(void)"
 * </table>
 ****************************************************************************************
 */
bool bts_tws_if_is_nv_master(void);

/******************************************************************************************
 * @brief  exit sniff with tws
 *
 * @return An error status
 * <table>
 * <th>Description   replace api "bt_status_t app_tws_ibrt_exit_sniff_with_tws(void)"
 * </table>
 ****************************************************************************************
 */
bt_status_t bts_tws_if_tws_link_exit_sniff_mode(void);

/******************************************************************************************
 * @brief  enter sniff with tws
 *
 * @return An error status
 * <table>
 * <th>Description
 * </table>
 ****************************************************************************************
 */
bt_status_t bts_tws_if_bt_link_enter_sniff_mode(btif_sniff_info_t *info);
/**
 ****************************************************************************************
 * @brief register callback for disallow sdk do role switch
 *
 * @param[in] cb
 *
 * @return void
 * <table>
 * <th>Description   replace api "void app_ibrt_conn_reg_disallow_role_switch_callback(bool (*cb)(void))"
 * <th>Description   replace api "void app_ibrt_if_reg_disallow_role_switch_callback(earbud_ux_tws_switch_disallow_cb cb)"
 * </table>
 ****************************************************************************************
 */
void bts_tws_if_reg_disallow_role_switch_callback(bool (*cb)(void));

/******************************************************************************************
 * @brief Request modify tws bandwidth.Please use this API with caution
 *
 * @param[in] TWS_TIMING_CONTROL_USER_E user
 * <table>
 * <th>Description   TWS_TIMING_CONTROL_USER_SNIFF = 0,
 * <th>Description   TWS_TIMING_CONTROL_USER_DEFAULT,
 * <th>Description   TWS_TIMING_CONTROL_USER_A2DP,
 * <th>Description   TWS_TIMING_CONTROL_USER_OTA,
 * <th>Description   TWS_TIMING_CONTROL_USER_AI_VOICE,
 * <th>Description   TWS_TIMING_CONTROL_USER_START_IBRT,
 * <th>Description   TWS_TIMING_CONTROL_USER_IBRT_SWITCH,
 * <th>Description   TWS_TIMING_CONTROL_USER_FAST_COMMUNICATION,
 * <th>Description   TWS_TIMING_CONTROL_USER_PAGE_ONGOING,
 * </table>
 * @param[in] bool enable
 * <table>
 * <th>Description   true enable user request tws bandwidth
 * <th>Description   false disable user request tws bandwidth
 * </table>
 * <table>
 * <th>Description   replace api "app_ibrt_if_request_modify_tws_bandwidth"
 * </table> 
 *****************************************************************************************/
void bts_tws_if_request_modify_tws_bandwidth(TWS_TIMING_CONTROL_USER_E user, bool enable);

/******************************************************************************************
 * @brief Check tws send data mtu size.
 *
 * @return uint32_t
 * <table>
 * <th>Description   replace api "uint32_t app_ibrt_if_get_tws_mtu_size(void)"
 * <th>Description   replace api "uint32_t tws_ctrl_get_mtu_size(void)"
 * </table>
 *****************************************************************************************/
uint32_t bts_tws_if_get_mtu_size(void);

/******************************************************************************************
 * @brief Initial tws connection
 *
 * @param[in] timeout               page_timeout (unit is slot --625us)
 * @param[in] time_to_next_page     the idle time when page timeout before page next device (ms)
 *
 * @return An error status
 * <table>
 * <th>Description   replace api " void app_ibrt_if_conn_tws_connect_request(bool isInPairingMode, uint32_t timeout)"
 * </table>
 *****************************************************************************************/
ibrt_status_t bts_tws_if_connect_acl_link(uint32_t timeout, uint32_t time_to_next_page);

/******************************************************************************************
 * @brief Disconnect tws link
 *
 * @return An error status
 * <table>
 * <th>Description   replace api "bta_profile_status_t app_ibrt_if_tws_disconnect_request(void)"
 * </table>
 *****************************************************************************************/
ibrt_status_t  bts_tws_if_disconnect_acl_link(void);

/******************************************************************************************
 * @brief Check tws link is connecting.
 *
 * @return bool
 * <th>Description    replace api��"bool app_ibrt_conn_is_tws_link_connecting()"
 *****************************************************************************************/
bool bts_tws_if_is_tws_link_connecting(void);

/******************************************************************************************
 * @brief Check tws is connected.
 *
 * @return bool
 * <table>
 * <tr><th>Value        <th>Description
 * <tr><td>True   <td>tws besaud l2cap is connected
 * <tr><td>False  <td>tws besaud l2cap not conencted
 * </table>
 * <table>
 * <th>Description   replace api "bool app_tws_ibrt_is_besaud_connnected()"
 * </table>
 *****************************************************************************************/
bool bts_tws_if_is_besaud_connnected(void);

/**
 ****************************************************************************************
 * @brief Check if tws link is connected
 *
 * @return
 * <table>
 * <tr><th>Value        <th>Description
 * <tr><td>True  <td>tws link is connected
 * <tr><td>False  <td>tws link not conencted
 * </table>
 * <table>
 * <th>Description   replace api "bool app_ibrt_conn_is_tws_connected(void)"
 * <th>Description   replace api "bool app_ibrt_if_is_tws_link_connected(void)"
 * <th>Description   replace api "bool app_tws_ibrt_tws_link_connected(void)"
 * </table>
 ****************************************************************************************
 */
bool bts_tws_if_is_tws_link_connected(void);

/**
 ****************************************************************************************
 * @brief Check whether the passed-in value is equal to tws connection handle
 *
 * @return uint16_t  tws handle
 * <table>
 * <th>Description   replace api "uint16_t app_tws_get_tws_conhdl(void)"
 * <th>Description   replace api "uint16_t app_ibrt_conn_tws_conhdl_get(void)"
 * </table>
 ****************************************************************************************
 */
uint16_t bts_tws_if_get_tws_acl_handle(void);

/**
 ****************************************************************************************
 * @brief Set earbud side.
 *
 * @param[in] side               Only can be set to left or right side
 * <table>
 * <th>Description   replace api "void app_ibrt_if_set_side(bt_location_t side)"
 * </table>
 ****************************************************************************************
 */
void bts_tws_if_set_local_side(bt_location_t side);

/**
 ****************************************************************************************
 * @brief Check tws side is left
 *
 * @return bool
 * <table>
 * <th>Description   replace api "bool app_ibrt_middleware_is_left_side(void)"
 * <th>Description   replace api "bool app_ibrt_if_is_left_side(void)"
 * </table>
 ****************************************************************************************
 */
bool bts_tws_if_is_local_left_side(void);

/**
 ****************************************************************************************
 * @brief Check tws side is right
 *
 * @return bool
 * <table>
 * <th>Description   replace api "bool app_ibrt_middleware_is_right_side(void)"
 * <th>Description   replace api "bool app_ibrt_if_is_right_side(void)"
 * </table>
 ****************************************************************************************
 */
bool bts_tws_if_is_local_right_side(void);

void bts_tws_if_switch_prepare_done(IBRT_ROLE_SWITCH_USER_E user, uint32_t role);

/**
 ****************************************************************************************
 * @brief Initialize and make preparation for TWS synchronization.
 ****************************************************************************************
 */
void bts_tws_if_prepare_user_sync_tws_info(void);

/**
 ****************************************************************************************
 * @brief Fill tws information into pending sync list.
 *
 * @param[in] id                User id for tws information filling
 *
 * @return none
 ****************************************************************************************
 */
void bts_tws_if_user_sync_tws_info(TWS_SYNC_USER_E id);

/**
 ****************************************************************************************
 * @brief Flush pending tws sync information to peer device.
 ****************************************************************************************
 */
void bts_tws_if_flush_user_sync_tws_info();

/**
 ****************************************************************************************
 * @brief Register sync user callback
 *
 * @param[in] id       TWS_SYNC_USER_E
 *
 * @param[in] user     TWS_SYNC_USER_T *
 *
 ****************************************************************************************
 */
void bts_tws_if_register_tws_sync_user(TWS_SYNC_USER_E id, TWS_SYNC_USER_T *user);

/**
 ****************************************************************************************
 * @brief Set ui-role
 *
 * @param[in] ui_role       ui_role
 * <table>
 * <th>Description   replace api "void app_ibrt_conn_send_user_action_v2(uint8_t *p_buff, uint16_t length)"
 * </table>
 ****************************************************************************************
 */
void bts_tws_if_send_user_action(uint8_t *p_buff, uint16_t length);

/**
 ****************************************************************************************
 * @brief Get tws-role
 *
 * @param[in] tws bt role
 * <table>
 * <th>Description   replace api "btif_connection_role_t app_tws_ibrt_get_local_tws_role(void)"
 * </table>
 ****************************************************************************************
 */
btif_connection_role_t bts_tws_if_get_tws_link_bt_role(void);


/**
 ****************************************************************************************
 * @brief Convert uiRole to string
 *
 * @param[in] uiRole
 *
 * @return pointer of the string
 ****************************************************************************************
 */
const char* bts_core_uirole2str(bt_ui_role_t uiRole);

/**
 ****************************************************************************************
 * @brief Set ui-role
 *
 * @param[in] ui_role       ui_role
 * <table>
 * <th>Description   replace api "void app_ibrt_conn_set_ui_role(bt_ui_role_t ui_role)"
 * </table>
 ****************************************************************************************
 */
void bts_core_set_ui_role(bt_ui_role_t ui_role);

/**
 ****************************************************************************************
 * @brief Get ui-role
 *
 * @return ui-role
 * <table>
 * <th>Description   replace api "bt_ui_role_t app_ibrt_conn_get_ui_role(void)"
 * <th>Description   replace api "bt_ui_role_t app_ibrt_middleware_get_ui_role(void)"
 * <th>Description   replace api "bt_ui_role_t app_ui_get_current_role(void)"
 * <th>Description   replace api "bt_ui_role_t app_ibrt_if_get_ui_role(void)"
 * </table>
 ****************************************************************************************
 */
bt_ui_role_t bts_core_get_ui_role(void);

/**
 ****************************************************************************************
 * @brief report apps callback to customif ui
 *
 ****************************************************************************************
 */
void bts_tws_if_register_apps_cbs(const bts_tws_to_apps_cbs_t *cb);

uint8_t bts_get_ui_role(void);

bool bts_ui_role_is_slave(void);

uint8_t bts_tws_if_get_tws_link_mode(void);

uint8_t *bts_tws_if_get_local_addr(void);

uint8_t *bts_tws_if_get_peer_addr(void);

uint8_t bts_tws_if_get_nv_role(void);

void bts_tws_if_update_nv_role(uint8_t role);

bool bts_tws_if_is_search_ui(void);

bool bts_tws_if_get_init_done_state(void);

void bts_tws_if_update_tws_pairing_info(bt_ibrt_role_t role, uint8_t* peerAddr);

uint16_t bts_tws_if_get_acl_interval(void);

void bts_tws_if_add_cmd_table(app_tws_cmd_user_e user, uint8_t cmd_number, const bt_tws_cmd_instance_t *cmd_table);

bool bts_tws_if_send_cmd_without_rsp(uint16_t cmdcode, uint8_t *p_buff, uint16_t length);

void bts_tws_if_send_cmd_with_rsp(uint16_t cmdcode, uint8_t *p_buff, uint16_t length);

void bts_tws_register_sniff_params_callback(bts_sniff_params_callback_t callback);
#ifdef __cplusplus
}
#endif

#endif
