/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#ifndef __APP_BIS_SELFSCAN__
#define __APP_BIS_SELFSCAN__

#include "cmsis_os2.h"

#include "bt_le_types.h"

#ifdef BIS_SELFSCAN_ENABLED

/*INCLUDE*/

/*DEFINITIONS*/
#define APP_BIS_SELFSCAN_BCAST_ID_LEN       (3)
#define APP_BIS_SELFSCAN_INVALID_SRC_LID    (0xFF)
#define APP_BIS_SELFSCAN_BG_SCAN_ENABLE     (0)
#define APP_BIS_SELFSCAN_MAX_SCAN_RESULT    (10)
#define APP_BIS_SELFSCAN_MAX_NAME_LEN       (32)
#define APP_BIS_SELFSCAN_BG_SCAN_DUR_MS     (2000)

/*ENUMERATIONS*/
enum selfscan_sync_state
{
    /// SYNC success
    APP_BIS_SELFSCAN_SYNC_SUCCESS = 0,
    /// SYNC ing
    APP_BIS_SELFSCAN_SYNC_RUNNING,
    /// SYNC failed
    APP_BIS_SELFSCAN_SYNC_FAILED,
};

enum selfscan_scan_cmd
{
    APP_BIS_SELFSCAN_SCAN_START = 0,
    APP_BIS_SELFSCAN_SCAN_STOP = 1,
};

enum selfscan_state
{
    APP_BIS_SELFSCAN_STATE_IDLE = 0,
    APP_BIS_SELFSCAN_STATE_RECV = 1,
    APP_BIS_SELFSCAN_STATE_BUSY = 2,
};

enum selfscan_code
{
    APP_BIS_SELFSCAN_ASCLL = 0,
    APP_BIS_SELFSCAN_UTF8 = 1,
    APP_BIS_SELFSCAN_UNICODE = 2,
};

enum selfscan_opcode
{
    // Opcode min
    APP_BIS_SELFSCAN_OP_MIN = 0,
    /// Scan state get
    APP_BIS_SELFSCAN_OP_GET_SCAN_STATE = 0x39,
    /// Scan result get
    APP_BIS_SELFSCAN_OP_GET_SELECT_SRC = 0x3A,
    /// Scan Command
    APP_BIS_SELFSCAN_OP_SCAN_COMMAND   = 0x79,
    /// Source select command
    APP_BIS_SELFSCAN_OP_SRC_SELECT_CMD = 0x7A,
    /// Source term sync
    APP_BIS_SELFSCAN_OP_STOP_SYNC      = 0x7B,
    /// Source broadcast code set
    APP_BIS_SELFSCAN_OP_BCAST_CODE     = 0x7C,
    /// Scan Result notify
    APP_BIS_SELFSCAN_OP_SCAN_RESULT_NTF = 0x8C,
    /// Select src notify
    APP_BIS_SELFSCAN_OP_SELECT_SRC_NTF = 0x8D,
    /// Broadcast code request
    APP_BIS_SELFSCAN_OP_BCAST_CODE_REQ_NTF = 0x8E,
    /// Opcode max
    APP_BIS_SELFSCAN_OP_MAX,
};

/*STRUCTURES*/
struct app_bis_src_scan_result
{
    /// Src adv addr
    ble_bdaddr_t src_addr;
    /// Src adv sid
    uint8_t adv_sid;
    /// Broadcast ID
    uint8_t broadcast_id[APP_BIS_SELFSCAN_BCAST_ID_LEN];
    /// Broadcast Encryption
    bool encrypted;
    /// Public Broadcast Announcement features
    uint8_t pba_feature;
    /// Broadcast Name length  (If PBA is not preset and this will be 0)
    uint8_t broadcast_name_len;
    /// Broadcast Name (If PBA is not preset and this will be NONE)
    uint8_t broadcast_name[APP_BIS_SELFSCAN_MAX_NAME_LEN];
};

/*CALLBACKS*/
/// Callback for app_bis_selfscan scan state
typedef void (*app_bis_selfscan_cb_scan_state)(bool started);
/// Callback for app_bis_selfscan scan state
typedef void (*app_bis_selfscan_cb_scan_result)(uint8_t src_lid, const struct app_bis_src_scan_result *p_result);
/// Callback for app_bis_selfscan sync state
typedef void (*app_bis_selfscan_cb_sync_state)(const ble_bdaddr_t *p_src_addr, uint8_t adv_sid, enum selfscan_sync_state sync_state, uint16_t error);
/// Callback for app_bis_selfscan broadcast code required
typedef void (*app_bis_selfscan_cb_bcast_code_req)(uint8_t src_lid);
/// Callback function called when stream state updated
typedef void (*app_bis_selfscan_cb_stream_state)(bool started);

/*Structure*/
typedef struct app_bis_selfscan_evt_cb
{
    /// Callback function called when scan state updated
    app_bis_selfscan_cb_scan_state cb_scan_state;
    /// Callback function called when scan result generated
    app_bis_selfscan_cb_scan_result cb_scan_result;
    /// Callback function called when sync state changed
    app_bis_selfscan_cb_sync_state cb_sync_state;
    /// Callback function called when broadcast code required
    app_bis_selfscan_cb_bcast_code_req cb_bcast_code_req;
    /// Callback function called when stream state updated
    app_bis_selfscan_cb_stream_state cb_stream_state;

} app_bis_selfscan_evt_cb_t;

typedef struct app_bis_selfscan_init_cfg
{
    /// Maximum scan result stored
    uint8_t max_result_present;
    /// Scan timeout in second, 0 means forever
    uint16_t scan_timeout_s;
    /// Sync PA/BIS timeout in second, 0 means forever
    uint16_t sync_timeout_s;
    /// Restart PA sync when err occured, 0 means never restart
    uint16_t resync_pa_times_max;

} app_bis_selfscan_init_cfg_t;

/*FUNCTIONS DECALRATION*/
int app_bis_selfscan_init(const app_bis_selfscan_init_cfg_t *p_init_cfg, const app_bis_selfscan_evt_cb_t *p_evt_cb);
void app_bis_selfscan_tws_state_handler(bool connected);

/**
 * @brief Register BIS self-scan event callbacks
 *
 * Registers a set of callback functions to receive BIS self-scan related
 * events, such as source discovery, synchronization status changes, and
 * errors.
 *
 * @param[in] p_evt_cb  Pointer to BIS self-scan event callback structure
 *
 * @return 0 on success, non-zero on failure
 */
int app_bis_selfscan_register_evt_cb(const app_bis_selfscan_evt_cb_t *p_evt_cb);

/**
 * @brief Unregister BIS self-scan event callbacks
 *
 * Unregisters the previously registered BIS self-scan event callbacks.
 * After this API is called, no BIS self-scan events will be reported to
 * the application.
 *
 * @return 0 on success, non-zero on failure
 */
int app_bis_selfscan_unregister_evt_cb(void);

/**
 * @brief Get source local ID by advertising address and SID
 *
 * Finds the internal source local ID (src_lid) corresponding to a broadcast
 * source, based on the advertiser address and advertising SID.
 *
 * @param[in] p_src_addr  Pointer to the broadcast source device address
 * @param[in] adv_sid     Advertising SID of the broadcast source
 *
 * @return Source local ID if found, or an invalid ID if not found
 */
uint8_t appp_bis_selfscan_get_src_lid_by_addr_info(const ble_bdaddr_t *p_src_addr,
                                                   uint8_t adv_sid);

/**
 * @brief Enable or disable BIS self-scan
 *
 * Enables or disables scanning for BIS broadcast sources. When enabled,
 * the stack starts scanning and reporting discovered broadcast sources
 * through registered callbacks.
 *
 * @param[in] enable  true to enable scanning, false to disable scanning
 *
 * @return 0 on success, non-zero on failure
 */
int app_bis_selfscan_enable_scan(bool enable);

/**
 * @brief Select a broadcast source and start synchronization
 *
 * Selects a broadcast source identified by its local source ID and
 * initiates BIS synchronization. If the broadcast is encrypted,
 * the broadcast code must be provided.
 *
 * @param[in] src_lid        Local source ID of the broadcast source
 * @param[in] p_bcast_code  Pointer to broadcast code (NULL if not encrypted)
 *
 * @return 0 on success, non-zero on failure
 */
int app_bis_selfscan_select_sync_source(uint8_t src_lid,
                                        const uint8_t *p_bcast_code);

/**
 * @brief Stop synchronization with the current broadcast source
 *
 * Stops BIS synchronization with the currently selected broadcast source
 * and releases related resources.
 *
 * @return 0 on success, non-zero on failure
 */
int app_bis_selfscan_stop_sync_source(void);

#endif /* BIS_SELFSCAN_ENABLED */

#endif /*__APP_BIS_SELFSCAN__*/