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

#include "bt_types.h"
#include "bt_tws_types.h"
#include "bt_base_attributes.h"
#include "bta_tws_attributes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    BTA_TWS_SUBSYS_DISABLED, // UI deinitialization completed
    BTA_TWS_SUBSYS_ENABLED,  // UI initialization completed
} bta_tws_subsys_state_t;

typedef enum
{
    BTA_TWS_BOX_UNKNOWN = 0,        // Unknown case state; typically returned for the peer earbud before sync is complete
    BTA_TWS_IN_BOX_CLOSED,          // Earbuds are inside the case and the case is closed
    BTA_TWS_IN_BOX_OPEN,            // Earbuds are inside the case and the case is open
    BTA_TWS_OUT_BOX,                // Earbuds are outside the case and not being worn
    BTA_TWS_OUT_BOX_WEARED,         // Earbuds are outside the case and currently being worn
} bta_tws_box_state_t;

typedef enum
{
    BTA_TWS_OPEN = 0,       // Case opened while earbuds are inside
    BTA_TWS_CLOSE,          // Case closed while earbuds are inside
    BTA_TWS_UNDOCK,         // Earbuds removed from the case
    BTA_TWS_DOCK,           // Earbuds placed into the case
    BTA_TWS_WEAR_UP,        // Earbuds worn on the ears
    BTA_TWS_WEAR_DOWN,      // Earbuds removed from the ears
} bta_tws_box_event_t;

typedef enum
{
    BTA_TWS_BT_DISCONNECTED,  // BT link disconnected
    BTA_TWS_BT_CONNECTED,     // BT link connected
    BTA_TWS_BT_ENCRYPTED,     // BT link encrypted
} bta_tws_bt_link_state_t;

typedef enum
{
    BTA_TWS_BT_DISCONNECTED_EVENT,              // BT link disconnected (event received by both earbuds)
    BTA_TWS_BT_CONNECTING_EVENT,                // Attempting to establish BT connection
    BTA_TWS_BT_CONNECTING_CANCELLED_EVENT,      // BT connection attempt cancelled
    BTA_TWS_BT_CONNECTING_FAILURE_EVENT,        // BT connection attempt failed
    BTA_TWS_BT_CONNECTED_EVENT,                 // BT link successfully established
    BTA_TWS_BT_ENCRYPTED_EVENT,                 // BT link encrypted

    BTA_TWS_IBRT_DISCONNECTED_EVENT,            // IBRT link disconnected (event received by both earbuds)
    BTA_TWS_IBRT_CONNECTED_EVENT,               // IBRT link successfully established (event received by both earbuds)
    BTA_TWS_IBRT_ROLE_CHANGED_EVENT,            // IBRT role-switch completed (event received by both earbuds)
} bta_tws_bt_link_event_t;

typedef enum
{
    BTA_TWS_DISCONNECTED,    // TWS link is disconnected.
    BTA_TWS_SYNCING,        // TWS link is connected; earbuds are synchronizing data.
    BTA_TWS_SYNCED,         // TWS link is connected; earbuds have completed data synchronization.
} bta_tws_sync_state_t; // UI thread

typedef enum
{
    BTA_TWS_LINK_LOSS_RECONNCET,       //connect by link 08 error
    BTA_TWS_OPEN_BOX_RECONNCET,        //connect by open_box
    BTA_TWS_BOX_EVENT_RECONNECT,       //connect by other box event triger
    BTA_TWS_CUSTOMER_RECONNECT,        //connect by customer
} bta_tws_reconn_type_t;

typedef struct {
    bt_bdaddr_t          addr;               // Address of the device to page.            // Reconnet reason. LINK_LOSS is for LINK LOSS(08), TRY_RECONNECT is for OPEN, others is invalid.
    uint16_t             page_times;         // The times of page action about this device.
    uint32_t             page_timeout;       // Should be assigned a value in callback. Page timeout of this page(unit is 1 slot[0.625ms]).
    uint32_t             time_to_next_page;  // Should be assigned a value in callback. Idle time between this page and next page (unit is 1ms).
    bta_tws_reconn_type_t reconnect_type;
} bta_tws_page_para_t;

typedef struct
{
    // Notifies events related to BT and IBRT link state changes
    void (*bt_link_state_changed)(const bt_bdaddr_t *addr, bta_tws_bt_link_event_t event, bt_ibrt_role_t role, uint8_t reason);
    // Notifies completion of profile exchange
    void (*bt_profile_exchanged)(const bt_bdaddr_t *addr);
} bta_tws_bt_link_state_changed_t;

typedef struct
{
    // Notifies changes in BT access mode
    void (*bt_access_mode_changed)(bt_access_mode_t mode);
    // Notifies transitions between active mode and sniff mode
    void (*bt_link_mode_changed)(const bt_bdaddr_t *address, bt_link_mode_t mode, uint16_t interval, uint8_t status);
    // Notifies completion of remote device name retrieval
    void (*remote_name_request_completed)(const bt_bdaddr_t *address, const uint8_t *name, uint8_t len);
    // Notifies changes in bonding status
    void (*bt_bond_changed)(const bt_bdaddr_t *addr, uint8_t status);
    // Notifies sco state change
    void (*sco_connection_state)(const bt_bdaddr_t *address, bt_connection_state_t state, bt_sco_param_t *param);
} bta_tws_bt_callbacks_t;

typedef struct
{
    // Notifies completion of UI initialization or deinitialization
    void (*bt_subsys_state_changed)(bta_tws_subsys_state_t state);
    // Notifies changes in TWS link and synchronization status
    void (*tws_sync_state_changed)(bta_tws_sync_state_t state, uint8_t reason);
    // Notifies entry into or exit from pairing mode
    void (*pairing_mode_changed)(bool enabled);
    // Notifies the latest case state of the peer earbud
    void (*peer_box_state_changed)(bta_tws_box_state_t box_state);
    // Notifies the latest case state of the local earbud
    void (*local_box_state_changed)(bta_tws_box_state_t box_state);
    // Notifies changes in UI-Role
    void (*ui_role_switch_state_changed)(bt_ui_role_t role);
} bta_tws_ui_state_changed_t;

typedef struct
{
    /**
     ****************************************************************************************
     * @brief       Callback to determine whether to accept an incoming Bluetooth connection request.
     * @param[in]   addr: Address of the device requesting connection.
     * @param[in]   cod: Class of Device.
     * @param[out]  preempt: Pointer to a bt_bdaddr_t variable.
     *              - If accepting this request would cause another device to be removed,
     *                `preempt` will point to a valid buffer.
     *              - Fill in `preempt` to specify which existing device to remove.
     *              - If `preempt` is NULL, no removal will occur.
     * @return      true to accept the connection request, false to reject it.
     ****************************************************************************************
     */
    bool (*accept_connection_request_callback)(const bt_bdaddr_t *addr, const uint8_t cod[3], bt_bdaddr_t *preempt);
    void (*set_page_scan_param_callback)(bta_tws_page_para_t *page_para);
} bta_tws_ui_policy_callbacks_t;

// Customers should use BTA_TWS_UX_USER_CUSTOMER.
// BTA_TWS_UX_USER_INTERNAL_* are reserved for internal non-bthost modules (e.g., BESUI).
typedef enum
{
    BTA_TWS_UX_USER_BTA,
    BTA_TWS_UX_USER_APP,
    BTA_TWS_UX_USER_DEPRECATED,
    BTA_TWS_UX_USER_INTERNAL_1,
    BTA_TWS_UX_USER_CUSTOMER,
    BTA_TWS_UX_USER_MAX,
} bta_tws_ux_user_t;

/**
 ****************************************************************************************
 * @brief       Registers callbacks to monitor the link state with BT devices.
 * @param[in]   user: Callbacks registered under the same user will override each other.
 * @param[in]   hooks: A set of callbacks.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_register_bt_link_state_changed_hook(bta_tws_ux_user_t user, const bta_tws_bt_link_state_changed_t *hooks);

/**
 ****************************************************************************************
 * @brief       Registers callbacks to monitor BT-related events.
 * @param[in]   user: Callbacks registered under the same user will override each other.
 * @param[in]   hooks: A set of callbacks.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_register_bt_callbacks(bta_tws_ux_user_t user, const bta_tws_bt_callbacks_t *callbacks);

/**
 ****************************************************************************************
 * @brief       Registers callbacks to monitor UI-related events.
 * @param[in]   user: Callbacks registered under the same user will override each other.
 * @param[in]   hooks: A set of callbacks.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_register_ui_state_changed_hook(bta_tws_ux_user_t user, const bta_tws_ui_state_changed_t *hooks);

/**
 ****************************************************************************************
 * @brief       Sets callbacks to influence UI behavior.
 * @param[in]   hooks: A set of callbacks.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_set_ui_policy_callbacks(const bta_tws_ui_policy_callbacks_t *value);

/**
 ****************************************************************************************
 * @brief       Initializes TWS.
 * @param[in]   attributes: Attributes that affect UI behavior.
 *              These attributes are immutable after initialization.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_init(const bt_am_attributes_t *am_attributes, const bta_tws_attributes_t *tws_attributes);

/**
 ****************************************************************************************
 * @brief       Attempts to role-switch to UI-Slave and then disconnect the TWS link.
 *              If no TWS connection exists, all local links are disconnected immediately.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_shutdown();

/**
 ****************************************************************************************
 * @brief       Retrieves the addresses of all local BT state machines.
 *              A BT link may not yet be established with the corresponding devices.
 * @param[out]  out_addrs: Array to store the addresses.
 * @return      Number of addresses in the out_addrs.
 ****************************************************************************************
 */
uint8_t bta_tws_find_all_local_bt_device(bt_bdaddr_t *out_addrs);

/**
 ****************************************************************************************
 * @brief       Gets the link state of a BT device.
 * @param[in]   addr: Device address.
 * @return      Link state.
 ****************************************************************************************
 */
bta_tws_bt_link_state_t bta_tws_get_bt_link_state(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Retrieves the local page status of a specific BT device.
 * @param[in]   addr: Device address.
 * @param[out]  paging: Whether the device is being paged.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_get_page_status(const bt_bdaddr_t *addr, bool *paging /*, bool *page_count_exist */);

/**
 ****************************************************************************************
 * @brief       Gets the IBRT-Role of a BT device.
 * @param[in]   addr: Device address.
 * @return      IBRT-Role.
 *              Returns UNKNOWN if IBRT is not yet connected.
 ****************************************************************************************
 */
bt_ibrt_role_t bta_tws_get_ibrt_role(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Gets the profiles-exchange status between earbuds.
 * @param[in]   addr: Device address.
 * @return      Whether the profiles-exchange is completed.
 ****************************************************************************************
 */
bool bta_tws_is_profile_exchanged(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Gets the TWS sync state.
 * @return      TWS state.
 ****************************************************************************************
 */
bta_tws_sync_state_t bta_tws_get_sync_state(void);

/**
 ****************************************************************************************
 * @brief       Gets the location.
 * @return      Location.
 ****************************************************************************************
 */
bt_location_t bta_tws_get_location(void);

/**
 ****************************************************************************************
 * @brief       Sets the location.
 * @param       location: Location.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_set_location(bt_location_t location);

/**
 ****************************************************************************************
 * @brief       Gets the box state of the earbuds.
 * @param[in]   peer: Specifies whether to retrieve the box state of the local (false)
 *                    or the peer (true).
 * @return      Box state.
 ****************************************************************************************
 */
bta_tws_box_state_t bta_tws_get_box_state(bool peer);

/**
 ****************************************************************************************
 * @brief       Notifies the UI of a box event.
 * @param[in]   event: Box event.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_box_event_entry(bta_tws_box_event_t event);

/**
 ****************************************************************************************
 * @brief       Master switch for access mode.
 * @param[in]   enable: true to enable, false to disable.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_enable_access_mode(bool enable);

// bool bta_tws_is_access_mode_enabled(void); // unsupported

/**
 ****************************************************************************************
 * @brief       Enters or exits pairing mode.
 * @param[in]   enable: true to enter pairing mode, false to exit.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_enable_pairing_mode(bool enable);

/**
 ****************************************************************************************
 * @brief       Checks whether the earbuds is in pairing mode.
 * @return      true if in pairing mode, false otherwise.
 ****************************************************************************************
 */
bool bta_tws_is_pairing_mode_enabled(void);

/**
 ****************************************************************************************
 * @brief       Enters or exits freeman mode.
 *              TWS link will be disconnected or rejected in freeman mode.
 * @param[in]   enable: true to enter freeman mode, false to exit.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_enable_freeman_mode(bool enable);

/**
 ****************************************************************************************
 * @brief       Checks whether the earbuds is in freeman mode.
 * @return      true if in freeman mode, false otherwise.
 ****************************************************************************************
 */
bool bta_tws_is_freeman_mode_enabled(void);

/**
 ****************************************************************************************
 * @brief       Attempts to establish a connection with the specified BT device.
 * @param[in]   addr: Device address.
 * @param[in]   page_count: Number of page attempts.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_connect_bt_device(const bt_bdaddr_t *addr, uint8_t page_count);

/**
 ****************************************************************************************
 * @brief       Attempts to connect to all BT devices in the device list.
 * @param[in]   page_count: Number of page attempts for each device at most.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_connect_all_bt_devices(uint8_t page_count);

/**
 ****************************************************************************************
 * @brief       Removes the specified BT device.
 *              Any associated link (IBRT & BT-ACL) will be disconnected,
 *              and ongoing page attempts will be discarded.
 * @param[in]   addr: Device address.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_remove_bt_device(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Removes all BT devices.
 *              Any associated link (IBRT & BT-ACL) will be disconnected,
 *              and ongoing page attempts will be discarded.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_remove_all_bt_devices(void);

/**
 ****************************************************************************************
 * @brief       Start directed target LEA advertising
 *              to establish a connection with the specified LEA device.
 * @param[in]   addr: Device address.
 * @param[in]   ms: Advertising duration in milliseconds (rounded to second precision).
 * @return      None
 ****************************************************************************************
 */
void bta_tws_connect_lea_device(const bt_bdaddr_t *addr, uint32_t ms);

/**
 ****************************************************************************************
 * @brief       Removes the specified device.
 *              Any associated link (IBRT & BT-ACL & LE-ACL) will be disconnected,
 *              and ongoing page attempts will be discarded.
 * @param[in]   addr: Device address.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_remove_device(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Removes all devices.
 *              Any associated link (IBRT & BT-ACL & LE-ACL) will be disconnected,
 *              and ongoing page attempts will be discarded.
 * @param[in]   addr: Device address.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_remove_all_devices(void);

/**
 ****************************************************************************************
 * @brief       Removes the bond with the specified device.
 *              Any associated link (IBRT & BT-ACL) will be disconnected,
 *              and ongoing page attempts will be discarded.
 * @param[in]   addr: Device address.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_remove_bond(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Attempts to connect TWS.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_connect_tws_link(void);

/**
 ****************************************************************************************
 * @brief       Disconnect TWS.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_disconnect_tws_link(void);

/**
 ****************************************************************************************
 * @brief       Cancels paging to all devices and clears their remaining page attempts.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_clear_all_device_page();

/**
 ****************************************************************************************
 * @brief       Cancels paging tws and clears their remaining page attempts.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_clear_tws_page();

/**
 ****************************************************************************************
 * @brief       Enables or disables blocking of all page activities.
 *              When blocking is enabled, ongoing page activity
 *              will be immediately cancelled, and no new page activity can proceed.
 *              However, no page attempts are discarded—remain queued.
 *              When blocking is disabled, previously blocked
 *              page activities are immediately resumed.
 * @param[in]   block: true to block page, false to unblock.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_block_page(bool block);

/**
 ****************************************************************************************
 * @brief       Configures whether to block paging during streaming.
 * @param[in]   block: true to block page, false to unblock.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_block_page_when_streaming(bool block);

/**
 ****************************************************************************************
 * @brief Configures whether a new incoming Bluetooth connection is allowed to preempt
 *        an existing connection when the maximum connection limit is reached,
 *        during HFP or A2DP streaming.
 *
 * Preemption refers to disconnecting an existing link to accept a new connection request.
 * Disabling preemption during streaming helps avoid disrupting ongoing calls or music.
 *
 * @param[in] allow_preempt_during_hfp   Set to true to allow preemption while HFP streaming.
 * @param[in] allow_preempt_during_a2dp  Set to true to allow preemption while A2DP streaming.
 * @return    None
 ****************************************************************************************
 */
void bta_tws_config_preempt_during_streaming(bool allow_preempt_during_hfp, bool allow_preempt_during_a2dp);

/**
 ****************************************************************************************
 * @brief       Gets the maximum number of devices that can be managed.
 * @return      Maximum device count.
 ****************************************************************************************
 */
uint8_t bta_tws_get_device_num_max(void);

/**
 ****************************************************************************************
 * @brief       Sets the maximum number of devices that can be managed.
 *              If the new limit is smaller than the current number of devices,
 *              some devices may be removed.
 * @param[in]   device_num_max: New maximum device count.
 * @param[in]   reserved_hint: Array of device addresses to prioritize for retention
 *              if device removal is required.
 * @param[in]   reserved_hint_count: Number of entries in the reserved_hint array.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_set_device_num_max(uint8_t device_num_max, const bt_bdaddr_t reserved_hint[], uint8_t reserved_hint_count);

/**
 ****************************************************************************************
 * @brief       Gets NV-Role.
 * @return      NV-Role.
 ****************************************************************************************
 */
bt_ibrt_role_t bta_tws_get_nv_role(void);

/**
 ****************************************************************************************
 * @brief       Gets UI-Role.
 * @return      UI-Role.
 ****************************************************************************************
 */
bt_ui_role_t bta_tws_get_ui_role(void);

/**
 ****************************************************************************************
 * @brief       Checks whether a UI role-switch is currently in progress.
 * @return      true if a UI role-switch is ongoing, false otherwise.
 ****************************************************************************************
 */
bool bta_tws_is_ui_role_switching(void);

/**
 ****************************************************************************************
 * @brief       Initiates a UI role-switch request.
 * @param[in]   role_expected: The desired UI-Role.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_request_ui_role_switch(bt_ui_role_t role_expected);

void bta_tws_set_cmd_table(uint8_t cmd_number, const bt_tws_cmd_instance_t *cmd_table);
void bta_tws_send_cmd(uint32_t opcode, uint8_t *buf, uint16_t len);
bool bta_tws_send_cmd_without_rsp(uint16_t cmdcode, uint8_t *p_buff, uint16_t length);
void bta_tws_send_cmd_with_rsp(uint16_t cmdcode, uint8_t *p_buff, uint16_t length);
void bta_tws_send_rsp(uint16_t rsp_code, uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);

/**
 ****************************************************************************************
 * @brief       Reconfigures the NV-Role and peer earbud address.
 *              This function updates the values in memory and persists them to NV storage.
 *              The runtime update is not guaranteed to take effect well.
 *              For reliable behavior, reboot the device immediately after calling this function.
 * @param[in]   role: The new NV-Role.
 * @param[in]   peer_addr: Address of the peer earbud.
 * @return      None
 ****************************************************************************************
 */
void bta_tws_reconfig_nv_role(bt_ibrt_role_t nv_role, const bt_bdaddr_t *peer_addr);

void bta_tws_register_sniff_params_callback(bool (*cb)(bt_sniff_info_t *));

void bta_tws_enable_limited_mode(bool enable);
// bool bta_tws_is_limited_mode_enabled(void); // unsupported

bool bta_tws_is_tws_addr(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief Get the connected device address list.
 *
 * @param[in] p_dev_addr_l device address list
 * @return uint8_t the number of address entries
 *
 * Examples:
 *
 *
 * ```
 * bt_bdaddr_list_t dev_addr_l = { 0 };
 * uint8_t count = bta_tws_find_all_connected_bt_device(&dev_addr_l); // * `dev_addr_l` will be filled with connected address, `count` is the number of address entries
 * count = bta_tws_find_all_connected_bt_device(NULL); // * only the connected bt device device count will be got
 * ```
 ****************************************************************************************
 */
uint8_t bta_tws_find_all_connected_bt_device(bt_bdaddr_list_t *p_dev_addr_l);

#ifdef __cplusplus
}
#endif
