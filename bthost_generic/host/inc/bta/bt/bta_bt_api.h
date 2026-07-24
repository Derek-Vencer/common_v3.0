/****************************************************************************
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
#include "bt_a2dp_types.h"
#include "bt_avrcp_types.h"
#include "bt_dip_types.h"
#include "bt_hfp_types.h"
#include "bt_pbap_types.h"
#include "bt_hid_types.h"
#include "bt_spp_types.h"
#include "bt_iap2_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    BTA_PROFILE_STATUS_SUCCESS                  = 0,
    BTA_PROFILE_STATUS_PENDING                  = 1,
    BTA_PROFILE_STATUS_ERROR_INVALID_PARAMETERS = 2,
    BTA_PROFILE_STATUS_ERROR_NO_CONNECTION      = 3,
    BTA_PROFILE_STATUS_ERROR_CONNECTION_EXISTS  = 4,
    BTA_PROFILE_STATUS_IN_PROGRESS              = 5,
    BTA_PROFILE_STATUS_ERROR_DUPLICATE_REQUEST  = 6,
    BTA_PROFILE_STATUS_ERROR_INVALID_STATE      = 7,
    BTA_PROFILE_STATUS_ERROR_TIMEOUT            = 8,
    BTA_PROFILE_STATUS_ERROR_ROLE_SWITCH_FAILED = 9,
    BTA_PROFILE_STATUS_ERROR_UNEXPECTED_VALUE  = 10,
    BTA_PROFILE_STATUS_ERROR_OP_NOT_ALLOWED    = 11,
} bta_profile_status_t;

typedef enum
{
    BTA_A2DP_IDLE             = 0,
    BTA_A2DP_CODEC_CONFIGURED = 1,
    BTA_A2DP_OPEN             = 2,
    BTA_A2DP_STREAMING        = 3,
    BTA_A2DP_CLOSED           = 4,
} bta_a2dp_state_t;

typedef enum
{
    BTA_AVRCP_DISCONNECTED   = 0,
    BTA_AVRCP_CONNECTED      = 1,
    BTA_AVRCP_PLAYING        = 2,
    BTA_AVRCP_PAUSED         = 3,
    BTA_AVRCP_VOLUME_UPDATED = 4,
} bta_avrcp_state_t;

typedef enum
{
    BTA_HFP_SLC_DISCONNECTED = 0,
    BTA_HFP_CLOSED           = 1,
    BTA_HFP_SCO_CLOSED       = 2,
    BTA_HFP_PENDING          = 3,
    BTA_HFP_SLC_OPEN         = 4,
    BTA_HFP_NEGOTIATE        = 5,
    BTA_HFP_CODEC_CONFIGURED = 6,
    BTA_HFP_SCO_OPEN         = 7,
    BTA_HFP_INCOMING_CALL    = 8,
    BTA_HFP_OUTGOING_CALL    = 9,
    BTA_HFP_RING_INDICATION  = 10,
} bta_hfp_state_t;

typedef enum
{
    BTA_NO_CALL             = 0,
    BTA_CALL_ACTIVE         = 1,
    BTA_CALL_HOLD           = 2,
    BTA_SETUP_INCOMMING     = 3,
    BTA_SETUP_OUTGOING      = 4,
    BTA_SETUP_ALERT         = 5,
} bta_call_status_t;

typedef enum {
    BTA_BT_SLEEP_DEFAULT = 0,
    BTA_BT_SLEEP_A2DP_BASE = 1,
    BTA_BT_SLEEP_ID_0_A2DP = BTA_BT_SLEEP_A2DP_BASE,
    BTA_BT_SLEEP_ID_1_A2DP,
    BTA_BT_SLEEP_ID_2_A2DP,
    BTA_BT_SLEEP_ANC_ASSIST,
    BTA_BT_SLEEP_BT_MEDIA_MANAGER,
    BTA_BT_SLEEP_GAF_BIS_MEDIA_STREAM,
    BTA_BT_SLEEP_GAF_BIS_EXT_MEDIA_STREAM,
    BTA_BT_SLEEP_GAF_MEDIA_STREAM,
    BTA_BT_SLEEP_GAF_MOBILE_STREAM,
    BTA_BT_SLEEP_GAF_NON_CODEC,
    BTA_BT_SLEEP_GAF_HID,
    BTA_BT_SLEEP_GAF_OFFLOAD_STREAM,
    BTA_BT_SLEEP_GAF_WIRELESS_SRV,
    BTA_BT_SLEEP_GAF_WIRELESS_CLI,
    BTA_BT_SLEEP_GAF_DATAPATH_BLE,
    BTA_BT_SLEEP_GAF_PATH_USB,
    // add new user
    BTA_BT_SLEEP_MAX_USER,
} bta_sleep_user_t;

typedef struct
{
    // use big endian
    uint8_t data[BT_UUID_MAX_LEN];
    uint8_t len;
} bta_uuid_t;

/// Host channel classification channel map
typedef union
{
    /// BT channel map 79 bits
    uint8_t bt_chn_map[10];
} bta_bt_chn_map_t;

/**
 ****************************************************************************************
 * @brief       Callback function type for SCO/eSCO connection requests.
 * @param[in]   addr: Pointer to the Bluetooth address of peer device.
 * @return      true to allow the request and proceed with further processing
 *              (typically by the audio manager);
 *              false to immediately reject the SCO/eSCO connection.
 ****************************************************************************************
 */
typedef bool (*bta_sco_connection_reqeust_callback_t)(const bt_bdaddr_t *addr);

typedef void (*BSIR_event_callback_t)(uint8_t is_in_band_ring);
typedef void (*bta_l2cap_process_echo_req_cb)(const bt_bdaddr_t *addr, uint8_t sigid, const uint8_t *data, uint16_t data_len);
typedef void (*bta_l2cap_process_echo_rsp_cb)(const bt_bdaddr_t *addr, const uint8_t *data, uint16_t data_len);
typedef void (*bta_l2cap_fill_in_echo_req_cb)(const bt_bdaddr_t *addr, uint8_t *data, uint16_t data_len);

/**
 ****************************************************************************************
 * @brief       Config checker logging in main loop.
 * @param[in]   interval: Logging interval in milliseconds. Set to 0 to disable.
 ****************************************************************************************
 */
void bta_bt_config_checker_logging(uint32_t interval);

void bta_bt_register_callbacks(bt_callback_user_t user, bt_callbacks_t *callbacks);

/**
 ****************************************************************************************
 * @brief       Convert the device id to address.
 * @param[in]   device_id: Bluetooth device local id.
 * @param[out]  addr: Pointer to the Bluetooth address of peer device.
 * @return      Whether successful or not.
 ****************************************************************************************
 */
bool bta_get_addr_by_device_id(uint8_t device_id, bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Convert the address to device id.
 * @param[in]   addr: Pointer to the Bluetooth address of peer device.
 * @return      Bluetooth device local id.
 ****************************************************************************************
 */
uint8_t bta_get_device_id_by_addr(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Convert the address to connection handle.
 * @param[in]   addr: Pointer to the Bluetooth address of peer device.
 * @return      Bluetooth device connection handle.
 ****************************************************************************************
 */
uint16_t bta_get_conhdl_by_addr(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Disconnect Bluetooth ACL.
 * @param[in]   addr: Pointer to the Bluetooth address of peer device.
 * @return      Bluetooth device connection handle.
 ****************************************************************************************
 */
void bta_bt_acl_disconnect(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Get bt access mode.
 * @param[in]   addr: Pointer to the Bluetooth address of peer device.
 * @return      Bluetooth device connection handle.
 ****************************************************************************************
 */
bt_access_mode_t bta_get_access_mode();

/**
 ****************************************************************************************
 *@ brief linearly maps unsigned values from one range to another range
 *@ param [in] from_val : from the input value to be mapped to val
 *@ param [in] from_min : from minimum value of input range
 *@ param [in] from_max : maximum value of input range from _max
 *@ param [in] to_min : The minimum value of the output range
 *@ param [in] to_max : The maximum value of the output range
 *@ return     The unsigned integer value after  mapping (if the input value exceeds the range,
               it is usually clamped or calculated in linear proportion)
****************************************************************************************
*/
uint32_t bta_unsigned_range_value_map(uint32_t from_val, uint32_t from_min, uint32_t from_max, uint32_t to_min, uint32_t to_max);

/**
 ****************************************************************************************
 * @brief       Get remote lmp version.
 * @param[in]   addr: Bluetooth device local id.
 * @param[in]   remote_version: Bluetooth device local id.
 * @return      Bluetooth device connection handle.
 ****************************************************************************************
 */
bool bta_get_remote_version(const bt_bdaddr_t *addr, bt_remote_version_t *remote_version);

/**
 ****************************************************************************************
 * @brief       Register l2cap enco callback.
 * @param[in]   process_echo_req: Process echo req callback.
 * @param[in]   process_echo_rsp: Process echo rsp callback.
 * @param[in]   fill_in_echo_req: Fill echo req data callback when send echo req.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_l2cap_echo_init(bta_l2cap_process_echo_req_cb process_echo_req,
                        bta_l2cap_process_echo_rsp_cb process_echo_rsp,
                        bta_l2cap_fill_in_echo_req_cb fill_in_echo_req);

/**
 ****************************************************************************************
 * @brief       Send l2cap echo req.
 * @param[in]   addr: Pointer to the Bluetooth address of peer device.
 * @param[in]   data: Echo req data.
 * @param[in]   data_len: Echo req data len.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_l2cap_send_echo_req(const bt_bdaddr_t *addr, const uint8_t *data, uint16_t data_len);

/**
 ****************************************************************************************
 * @brief       Send l2cap echo rsp.
 * @param[in]   addr: Pointer to the Bluetooth address of peer device.
 * @param[in]   sigid: l2cap signal id, see@bta_l2cap_process_echo_req_cb.
 * @param[in]   data: Echo rsp data.
 * @param[in]   data_len: Echo rsp data.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_l2cap_send_echo_rsp(const bt_bdaddr_t *addr, uint8_t sigid, const uint8_t *data, uint16_t data_len);

/**
 ****************************************************************************************
 * @brief       Set BR/EDR local name.
 * @param[in]   name: Pointer to the name.
 * @param[in]   len: name len.
 * @note        This function not write back to factory.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_bt_set_local_name(const char *name, uint8_t len);

/**
 ****************************************************************************************
 * @brief       Get BR/EDR local name.
 * @param[out]  name_buf: Pointer to the name buffer.
 * @param[in]   buf_len: buffer len.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_bt_get_local_name(char *name_buf, uint8_t buf_len);

/**
 ****************************************************************************************
 * @brief       Set BR/EDR local address.
 * @param[in]   addr: Pointer to the Bluetooth address of local device.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_bt_set_local_addr(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Get BR/EDR local address.
 * @param[out]  addr: Pointer to the Bluetooth address of local device.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_bt_get_local_addr(bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Register callback for fill manufacture eir data.
 * @note        eir_manufacture_data_callback will be call.
 ****************************************************************************************
 */
void bta_register_eir_manufacture_data_callback(void (*cb)(uint8_t *buff, uint32_t *offset));

/**
 ****************************************************************************************
 * @brief       Check connection is central.
 * @param[in]   addr: Pointer to the Bluetooth address of peer device.
 * @return      Central or not.
 ****************************************************************************************
 */
bool bta_is_bt_central(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       enable/exit bt sleep mode.
 * @param[in]   user: user type.
 * @param[in]   enable: enable/disable.
 ****************************************************************************************
 */
void bta_bt_write_sleep_enable(uint8_t user, bool enable);

/**
 ****************************************************************************************
 * @brief       Check connection is in sniff.
 * @param[in]   addr: Pointer to the Bluetooth address of peer device.
 * @return      In sniff mode or not.
 ****************************************************************************************
 */
bool bta_is_in_sniff_mode(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Update connection qos.
 * @param[in]   addr: Pointer to the Bluetooth address of peer device.
 * @param[in]   tpoll_slot:
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_update_bt_device_qos(const bt_bdaddr_t *addr, uint8_t tpoll_slot);

/**
 ****************************************************************************************
 * @brief       Exit sniff mode.
 * @param[in]   addr: Pointer to the Bluetooth address of peer device.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_exit_sniff(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Exit mhdt mode.
 * @param[in]   bdaddr: Pointer to the Bluetooth address of peer device.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_request_exit_mhdt_mode(const bt_bdaddr_t *bdaddr);

/**
 ****************************************************************************************
 * @brief       Enter mhdt mode.
 * @param[in]   bdaddr: Pointer to the Bluetooth address of peer device.
 * @param[in]   tx_rates: Pointer to the Bluetooth address of peer device.
 * @param[in]   rx_rates: Pointer to the Bluetooth address of peer device.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_request_enter_mhdt_mode(const bt_bdaddr_t *bdaddr, uint8_t tx_rates, uint8_t rx_rates);

/**
 ****************************************************************************************
 * @brief       set link policy prevent evnet.
 * @param[in]   enable: true:prevent sniff   false:allow sniff
 *              addr: mobile device address.
 ****************************************************************************************
 */
int bta_prevent_sniff_set(const bt_bdaddr_t *bdaddr, bool enable);

/**
 ****************************************************************************************
 * @brief       Register bt sniff config callback.
 * @param[in]   cb: Pointer to the sniff config callback.
 ****************************************************************************************
 */
void bta_register_bt_sniff_config_callback(bool (*cb)(bt_sniff_info_t *));

/**
 ****************************************************************************************
 * @brief       Register mhdt mode changed callback.
 * @param[in]   cb: Pointer to the mhdt mode changed callback.
 ****************************************************************************************
 */
void bta_register_mhdt_mode_change_callback(void (*cb)(struct bdaddr_t remote, bool isIn_mhdt_mode));

void bta_register_hci_log_report_callback(uint16_t max_len,
                int (*tx_cb)(const uint8_t *buf, uint16_t len),
                int (*rx_cb)(const uint8_t *buf, uint16_t len));

/**
 ****************************************************************************************
 * @brief       Retrieves the PSM associated with a given CID.
 *
 * @param[in]   conhdl  Connection handle identifying the ACL link.
 * @param[in]   scid    Set to true if `cid` is a Source CID, false if it is a Destination CID.
 * @param[in]   cid     Channel Identifier (either SCID or DCID based on `scid`).
 *
 * @return      The PSM value corresponding to the specified CID. Returns 0 if not found.
 ****************************************************************************************
 */
uint16_t bta_l2cap_get_psm_by_cid(uint16_t conhdl, bool scid, uint16_t cid);

/**
 ****************************************************************************************
 * @brief       APP get pts test device addr.
 * @return      addr: Pointer to the Bluetooth address of the pts device.
 ****************************************************************************************
 */
bt_bdaddr_t *bta_get_pts_address(void);

/**
 ****************************************************************************************
 * @brief       Set BT AFH Host channel classification to controller.
 * @return      Status of sending BT Host channel classification cmd.
 ****************************************************************************************
 */
int bta_bt_set_afh_chn_classification(bta_bt_chn_map_t channel_map);

/**
 ****************************************************************************************
 * @brief       Start inquiry devices.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_bt_start_inquiry(void);

/**
 ****************************************************************************************
 * @brief       Cancel inquiry devices process.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_bt_cancel_inquiry(void);

/**
 ****************************************************************************************
 *    ____  _             _    ____  ____  ____       _    ____ ___
 *   | __ )| |_ __ _     / \  |___ \|  _ \|  _ \     / \  |  _ \_ _|
 *   |  _ \| __/ _` |   / _ \   __) | | | | |_) |   / _ \ | |_) | |
 *   | |_) | || (_| |  / ___ \ / __/| |_| |  __/   / ___ \|  __/| |
 *   |____/ \__\__,_| /_/   \_\_____|____/|_|     /_/   \_\_|  |___|
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief       Register the a2dp sink callbacks.
 * @param[in]   user: Indicate who registered.
 * @param[in]   callbacks: Pointer to callbacks for handling a2dp sink events.
 ****************************************************************************************
 */
void bta_a2dp_register_callbacks(bt_a2dp_sink_callback_user_t user, bt_a2dp_sink_callbacks_t *callbacks);

/**
 ****************************************************************************************
 * @brief       Register the a2dp revied cmd request delay send response callback.
 * @param[in]   req_delay_cb: Pointer to callbacks for handling a2dp sink events.
 ****************************************************************************************
 */
void bta_a2dp_register_req_delay_rsp_callback(bt_a2dp_cmd_req_delay_rsp_cb req_delay_cb);

/**
 ****************************************************************************************
 * @brief       Initiate a connection to a2dp source.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_a2dp_connect(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Initiate a disconnection to a2dp source.
 * @param[in]   address: Pointer to the Bluetooth address of peer device
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_a2dp_disconnect(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Determine whether the a2dp sink is connected.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      Whether it is the connected.
 ****************************************************************************************
 */
bool bta_a2dp_is_connected(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Determine whether the current connection is initiated actively.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      Whether it is the initiator.
 ****************************************************************************************
 */
bool bta_a2dp_is_initiator(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Determine whether the current is in streaming state.
 * @param[in]   address: Pointer to the Bluetooth address of peer device
 * @return      Whether it is the streaming.
 ****************************************************************************************
 */
bool bta_a2dp_is_streaming(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Get a2dp lhdc llc config.
 * @return      llc config.
 ****************************************************************************************
 */
uint8_t bta_a2dp_get_lhdc_llc_config(void);

/**
 ****************************************************************************************
 * @brief       Get a2dp lhdc ext flags.
 * @return      ext flags.
 ****************************************************************************************
 */
bool bta_a2dp_lhdc_get_ext_flags(uint32_t flags);

/**
 ****************************************************************************************
 * @brief       Send delay report by a2dp sink.
 * @param[in]   addr: Pointer to the Bluetooth address of peer device.
 * @param[in]   delay_ms: Time of delay.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_a2dp_set_delay(const bt_bdaddr_t *addr, uint16_t delay_ms);

/**
 ****************************************************************************************
 * @brief       Send delay report by a2dp sink.
 * @param[in]   addr: Pointer to the Bluetooth address of peer device.
 * @param[out]  elements: Pointer to the codec elements buffer.
 * @param[in]   len: codec elements buffer len.
 * @return      Whether successful or not.
 ****************************************************************************************
 */
bool bta_get_codec_elements(const bt_bdaddr_t *addr, const uint8_t **elements, uint8_t *len);

/**
 ****************************************************************************************
 * @brief       Accept a2dp custom cmd.
 * @param[in]   bd_addr: Pointer to the Bluetooth address of peer device.
 * @param[in]   cmd: Pointer to callbacks for handling a2dp sink events.
 * @param[in]   accept: Whether to accept.
 * @return      Status of operation.
 ****************************************************************************************
 */
uint8_t bta_a2dp_accept_unknown_cmd(const bt_bdaddr_t *bd_addr, const bt_a2dp_unknown_cmd_t *cmd, bool accept);

/**
 ****************************************************************************************
 * @brief       Get A2DP stream state of specific mobile device address.
 * @param[in]   addr: Specify the mobile device address to get A2DP stream state.
 * @param[out]  a2dp_state: Pointer to A2DP stream state.
 * @return      Profile status.
 ****************************************************************************************
 */
bta_profile_status_t bta_get_a2dp_state(const bt_bdaddr_t *addr, bta_a2dp_state_t *a2dp_state);

/**
 ****************************************************************************************
 * @brief       Register A2DP sep capability.
 * @param[in]   codectype: Specify A2DP stream codec.
 * @param[in]   nonetype: Specify A2DP stream vender codec.
 * @param[in]   sep_priority: Specify A2DP stream codec sep priority.
 * @param[in]   elements: Specify A2DP stream codec elements.
 * @param[in]   len: Specify A2DP stream codec len of elements.
 ****************************************************************************************
 */
int bta_a2dp_codec_init(uint8_t codectype, uint8_t nonetype, uint8_t sep_priority, uint8_t *elements, uint8_t len);

/**
 ****************************************************************************************
 * @brief       Register A2DP sep capability.
 * @param[in]   bd_addr: Pointer to the Bluetooth address of peer device.
 * @param[in]   quick_switch_enable: Indicates that the current state supports fast switching mode
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_a2dp_update_quick_switch_state(const bt_bdaddr_t *addr, bool quick_switch_enable);
/**
 ****************************************************************************************
 *    ____  _             ___     ______   ____ ____       _    ____ ___
 *   | __ )| |_ __ _     / \ \   / /  _ \ / ___|  _ \     / \  |  _ \_ _|
 *   |  _ \| __/ _` |   / _ \ \ / /| |_) | |   | |_) |   / _ \ | |_) | |
 *   | |_) | || (_| |  / ___ \ V / |  _ <| |___|  __/   / ___ \|  __/| |
 *   |____/ \__\__,_| /_/   \_\_/  |_| \_\\____|_|     /_/   \_\_|  |___|
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief       Register avrcp event callbacks
 * @param[in]   user: Indicate who registered
 * @param[in]   callbacks: Pointer to callbacks for handling avrcp events.
 ****************************************************************************************
 */
void bta_avrcp_register_callbacks(bt_avrcp_callback_user_t user, bt_avrcp_callbacks_t *callbacks);

/**
 ****************************************************************************************
 * @brief       Initiate avrcp connection.
 * @param[in]   addr: Pointer to the Bluetooth address of peer device.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_avrcp_connect(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Initiate disconnection.
 * @param[in]   addr: Pointer to the Bluetooth address of peer device.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_avrcp_disconnect(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Determine whether the avrcp is connected.
 * @param[in]   addr: Pointer to the Bluetooth address of peer device.
 * @return      Whether connected.
 ****************************************************************************************
 */
bool bta_avrcp_is_connected(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Send media play command.
 * @param[in]   addr: Pointer to the Bluetooth address of peer device.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_avrcp_send_play(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Send media pause command.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_avrcp_send_pause(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Send media forward command.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_avrcp_send_forward(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Send media backward command.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_avrcp_send_backward(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Send media volume up command.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_avrcp_send_volume_up(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Send media volume down command.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_avrcp_send_volume_down(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Send media fast forward command.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   press:   true to send a "press" (key down) event, false to send a "release" (key up) event.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_avrcp_send_fast_forward(const bt_bdaddr_t *addr, bool press);

/**
 ****************************************************************************************
 * @brief       Send media rewind command.
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   press:   true to send a "press" (key down) event, false to send a "release" (key up) event.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_avrcp_send_rewind(const bt_bdaddr_t *addr, bool press);

/**
 ****************************************************************************************
 * @brief       CT send set absolute volume command
 * @param[in]   address: Pointer to the Bluetooth address of peer device.
 * @param[in]   volume:
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_avrcp_send_set_abs_volume(const bt_bdaddr_t *addr, uint8_t volume);

/**
 ****************************************************************************************
 * @brief       Get AVRCP playback state of specific mobile device address.
 * @param[in]   addr: Specify the mobile device address to get AVRCP palyback state.
 * @param[out]  avrcp_state: Pointer AVRCP palyback state.
 * @return      Error status
 ****************************************************************************************
 */
bta_profile_status_t bta_get_avrcp_state(const bt_bdaddr_t *addr, bta_avrcp_state_t *avrcp_state);

/**
 ****************************************************************************************
 *    ____  _          _   _ _____ ____       _    ____ ___
 *   | __ )| |_ __ _  | | | |  ___|  _ \     / \  |  _ \_ _|
 *   |  _ \| __/ _` | | |_| | |_  | |_) |   / _ \ | |_) | |
 *   | |_) | || (_| | |  _  |  _| |  __/   / ___ \|  __/| |
 *   |____/ \__\__,_| |_| |_|_|   |_|     /_/   \_\_|  |___|
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief       Register the hf event callbacks.
 * @param[in]   user: Registered user.
 * @param[in]   callbacks: Pointer to callbacks for handling hfp hf events.
 ****************************************************************************************
 */
void bta_hf_register_callbacks(bt_hfp_hf_callback_user_t user, bt_hfp_hf_callbacks_t *callbacks);

/**
 ****************************************************************************************
 * @brief       Ddinit the hfp profile.
 ****************************************************************************************
 */
void bta_hf_deinit(void);

/**
 ****************************************************************************************
 * @brief       Connect to an audio gateway device.
 * @param[in]   addr: Pointer to the Bluetooth address of the AG device.
 *              - Must not be NULL.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_hf_connect(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Disconnect from an audio gateway device.
 * @param[in]   addr: Pointer to the Bluetooth address of the AG device.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_hf_disconnect(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Determine whether the hf is connected.
 * @param[in]   addr: Pointer to the Bluetooth address of the AG device.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
bool bta_hf_is_connected(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Establish an audio connection(eSCO/SCO) with an audio gateway.
 * @param[in]   addr: Pointer to the Bluetooth address of the AG device.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_hf_connect_audio_link(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Terminate the audio connection with an audio gateway.
 * @param[in]   addr: Pointer to the Bluetooth address of the AG device.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_hf_disconnect_audio_link(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Check whether SCO/eSCO is connected.
 * @param[in]   addr: Pointer to the Bluetooth address of the AG device.
 * @return      Whether connected.
 ****************************************************************************************
 */
bool bta_hf_is_audio_link_connected(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Place a call using the last number
 * @param[in]   addr: Pointer to the Bluetooth address of the AG device.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_hf_call_redial(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Answer call
 * @param[in]   addr: Pointer to the Bluetooth address of the AG device.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_hf_call_answer(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Hung up call
 * @param[in]   addr: Pointer to the Bluetooth address of the AG device.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_hf_call_hangup(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Hold call.
 * @param[in]   addr: Pointer to the Bluetooth address of the AG device.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_hf_call_hold(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Hungup incoming when 3way incoming.
 * @param[in]   addr: Pointer to the Bluetooth address of the AG device.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_hf_3way_hungup_incoming(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Hold call.
 * @param[in]   addr: Pointer to the Bluetooth address of the AG device.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_hf_3way_hungup_active_accept_incomming(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Report battery level.
 * @param[in]   level: battery level.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_hf_battery_report(uint8_t level);

/**
 ****************************************************************************************
 * @brief       Enable/Disable voice assistant.
 * @param[in]   is_enable:
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_hf_control_voice_assistant(bool is_enable);

/**
 ****************************************************************************************
 * @brief       Get ciev call state.
 * @param[in]   addr: Pointer to the Bluetooth address of the AG device.
 * @return      Call status.
 ****************************************************************************************
 */
bt_hfp_call_state_t bta_hf_get_ciev_call_state(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Get ciev call setup state.
 * @return      Call setup status.
 ****************************************************************************************
 */
bt_hfp_callsetup_state_t bta_hf_get_ciev_callsetup_state(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Get ciev call held state.
 * @return      Call held status.
 ****************************************************************************************
 */
bt_hfp_callheld_state_t bta_hf_get_ciev_callheld_state(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Get HFP state iof specific mobile device address.
 * @param[in]   addr:Specify the mobile device address to get HFP state
 * @param[out]  hfp_state:Pointer at which pointer to indicate HFP state
 * @return      An error status
 ****************************************************************************************
 */
bta_profile_status_t bta_hf_get_hfp_state(const bt_bdaddr_t *addr, bta_hfp_state_t *hfp_state);

/**
 ****************************************************************************************
 * @brief       Get HFP call status of specific mobile device address.
 * @param[in]   addr:Specify the mobile device address to get HFP Call status.
 * @param[out]  call_status:Pointer at which pointer to indicate HFP Call status.
 * @return      An error status
 ****************************************************************************************
 */
bta_profile_status_t bta_hf_get_hfp_call_status(const bt_bdaddr_t *addr, bta_call_status_t *call_status);

/**
 ****************************************************************************************
 * @brief       HFP send battery level to specific mobile device address.
 * @param[in]   addr:Specify the mobile device address to get HFP Call status.
 * @param[out]  level:Battery level(0~100).
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_hfp_hf_send_battery_level(const bt_bdaddr_t *addr, uint8_t level);

/**
 ****************************************************************************************
 * @brief       Send AT commands to the mobile phone at the specified address
 * @param[in]   addr:Specify the mobile device's address
 * @param[out]  at_cmd_str:The command string to be sent.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_hf_send_at_cmd(const bt_bdaddr_t *bd_addr, const char *at_cmd_str);

/**
 ****************************************************************************************
 *    ____  _          ____  ____  ____       _    ____ ___
 *   | __ )| |_ __ _  / ___||  _ \|  _ \     / \  |  _ \_ _|
 *   |  _ \| __/ _` | \___ \| |_) | |_) |   / _ \ | |_) | |
 *   | |_) | || (_| |  ___) |  __/|  __/   / ___ \|  __/| |
 *   |____/ \__\__,_| |____/|_|   |_|     /_/   \_\_|  |___|
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief       Initialize a spp server.
 * @param[in]   local_server_channel: Indicate rfcomm channel used by spp server.
 * @param[in]   uuid: Spp server uuid.
 * @param[in]   callbacks: Indicate which channel the connection request come from.
 * @param[in]   name: Sdp record name.
 * @param[in]   name_len: Sdp record name length.
 * @param[in]   rx_buff_size: Indicate which channel the connection request come from.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_spp_init(bt_rfcomm_channel_t local_server_channel, const bta_uuid_t *uuid, const bt_spp_callbacks_t *callbacks,
                 const char *name, uint8_t name_len, uint16_t rx_buff_size);

/**
 ****************************************************************************************
 * @brief       Deinitialize a spp server.
 * @param[in]   local_server_channel: Indicate rfcomm channel used by spp server.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_spp_deinit(bt_rfcomm_channel_t local_server_channel);

/**
 ****************************************************************************************
 * @brief       Set whether credits are sent by the upper layer.
 * @param[in]   local_server_channel: Indicate rfcomm channel used by spp server.
 * @param[in]   enable: Indicate whether the credits will be sent by yourself.
 *              - true: credits will be sent after data is processed by upper layer,
 *              call@bta_spp_send_credit.
 *              - false: credits are automatically sent by lower layer after data received.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_spp_control_credit_by_self(bt_rfcomm_channel_t local_server_channel, bool enable);

/**
 ****************************************************************************************
 * @brief       Initiate a connection use uuid.
 * @param[in]   addr: Pointer to the Bluetooth address of the remote spp.
 * @param[in]   local_server_channel: Indicate rfcomm channel used by spp server.
 * @param[in]   uuid: Indicate rfcomm channel used by spp server.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_spp_initiate_connect(const bt_bdaddr_t *addr, bt_rfcomm_channel_t local_server_channel, const bta_uuid_t *uuid);

/**
 ****************************************************************************************
 * @brief       Initiate a disconnection.
 * @param[in]   connhdl: Spp connection handle.
 * @param[in]   reason: Disconnect reason.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_spp_initiate_disconnect(bt_spp_connhdl_t connhdl, uint8_t reason);

/**
 ****************************************************************************************
 * @brief       Send spp data.
 * @param[in]   connhdl: spp connection handle.
 * @param[in]   data: Pointer to the data.
 * @param[in]   size: Data length.
 * @return      Status of the operation.
 ****************************************************************************************
 */
bool bta_spp_send_data(bt_spp_connhdl_t connhdl, const uint8_t *data, uint16_t size);

/**
 ****************************************************************************************
 * @brief       Get free send buffer size(how many packages can be cached).
 * @note        If free buffer is 0, upper layer cannot continue to send packets.
 * @param[in]   connhdl: spp connection handle.
 * @return      Free send buffer size.
 ****************************************************************************************
 */
uint16_t bta_spp_get_free_send_buf_size(bt_spp_connhdl_t connhdl);

/**
 ****************************************************************************************
 * @brief       Credits can be sent when the received data is processed.
 *              - see@bta_spp_control_credit_by_self
 * @param[in]   connhdl: Spp connection handle.
 * @param[in]   credits: Credits that need to be sent.
 * @return      Status of the operation.
 ****************************************************************************************
 */
bool bta_spp_send_credit(bt_spp_connhdl_t connhdl, uint8_t credits);

/**
 ****************************************************************************************
 *    ____  _          ____ ___ ____       _    ____ ___
 *   | __ )| |_ __ _  |  _ \_ _|  _ \     / \  |  _ \_ _|
 *   |  _ \| __/ _` | | | | | || |_) |   / _ \ | |_) | |
 *   | |_) | || (_| | | |_| | ||  __/   / ___ \|  __/| |
 *   |____/ \__\__,_| |____/___|_|     /_/   \_\_|  |___|
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief       Get pnp info, need wait query complete.
 * @param[in]   address: Pointer to the Bluetooth address of the peer device
 * @param[in]   info: Pointer to dip pnp info
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_dip_get_pnp_info(const bt_bdaddr_t *address, bt_dip_pnp_info_t *info);

/**
 ****************************************************************************************
 * @brief       Check the device at this address whether is ios device.
 * @param[in]   address: Pointer to the Bluetooth address of the peer device.
 * @return      Indicate whether is an ios device
 ****************************************************************************************
 */
bool bta_dip_check_is_ios_device(const bt_bdaddr_t *address);

/**
 ****************************************************************************************
 * @brief       Determine whether it is an ios device by the obtained vend_id.
 * @param[in]   vend_id: see @bt_dip_pnp_info_t
 * @param[in]   vend_id_source: see @bt_dip_pnp_info_t
 * @return      Indicate whether is an ios device
 ****************************************************************************************
 */
bool bta_dip_check_is_ios_device_by_vend_id(uint16_t vend_id, uint16_t vend_id_source);

/**
 ****************************************************************************************
 * @brief       Register dip info queried callback handler.
 * @param[in]   func: see @bt_dip_info_queried_callback
 * @return      None
 ****************************************************************************************
 */
void bta_register_dip_info_queried_callback(bt_dip_info_queried_callback func);

/**
 ****************************************************************************************
 *    ____  _          _   _ ___ ____       _    ____ ___
 *   | __ )| |_ __ _  | | | |_ _|  _ \     / \  |  _ \_ _|
 *   |  _ \| __/ _` | | |_| || || | | |   / _ \ | |_) | |
 *   | |_) | || (_| | |  _  || || |_| |  / ___ \|  __/| |
 *   |____/ \__\__,_| |_| |_|___|____/  /_/   \_\_|  |___|
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief       Register hid device callbacks.
 * @param[in]   callbacks: Pointer to callbacks for handling hid events.
 * @return      none
 ****************************************************************************************
 */
void bta_hid_register_callbacks(bt_hid_callbacks_t *callbacks);

/**
 ****************************************************************************************
 * @brief       Initiate connection to hid host
 * @param[in]   addr: Pointer to the Bluetooth address of the peer device.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_hid_connect(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Initiate disconnection to hid host
 * @param[in]   addr: Pointer to the Bluetooth address of the peer device.
 * @return      0 if successful, non-zero otherwise.
 ****************************************************************************************
 */
int bta_hid_disconnect(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Determine whether the hid is connected.
 * @param[in]   addr: Pointer to the Bluetooth address of the peer device.
 * @return      Status of the operation.
 ****************************************************************************************
 */
bool bta_hid_is_connected(const bt_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief       Hid device send sensor state.
 * @param[in]   addr: Pointer to the Bluetooth address of the peer device.
 * @param[in]   report: Pointer to the custom descriptor.
 * @return      0 if successful, non-zero otherwise..
 ****************************************************************************************
 */
int bta_hid_send_sensor_report(const bt_bdaddr_t *addr, const bt_hid_sensor_report_t *reportr);

/**
 ****************************************************************************************
 *    ____ _______         _____  ____          _____             _____ _____
 *   |  _ \__   __|/\     |  __ \|  _ \   /\   |  __ \      /\   |  __ \_   _|
 *   | |_) | | |  /  \    | |__) | |_) | /  \  | |__) |    /  \  | |__) || |
 *   |  _ <  | | / /\ \   |  ___/|  _ < / /\ \ |  ___/    / /\ \ |  ___/ | |
 *   | |_) | | |/ ____ \  | |    | |_) / ____ \| |       / ____ \| |    _| |_
 *   |____/  |_/_/    \_\ |_|    |____/_/    \_\_|      /_/    \_\_|   |_____|
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief       Register the pbap event callbacks.
 * @param[in]   user: Registered user.
 * @param[in]   callbacks: Pointer to callbacks for handling pbap hf events.
 ****************************************************************************************
 */
void bta_pbap_register_callbacks(bt_pbap_callback_user_t user, bt_pbap_callbacks_t *callbacks);

/**
 ****************************************************************************************
 * @brief       Get the size of the phonebook from the remote PBAP server
 * @param[in]   remote: Pointer to the Bluetooth address of the remote device
 * @param[in]   search_param: Pointer to the PBAP search parameter structure for filtering
 * @return      None
 ****************************************************************************************
 */
void bta_pbap_client_get_phonebook_size(const bt_bdaddr_t *remote, bt_pbap_request_search_param_t *search_param);

/**
 ****************************************************************************************
 * @brief       Get the phonebook data (PB) from the remote PBAP server
 * @param[in]   remote: Pointer to the Bluetooth address of the remote device
 * @param[in]   search_param: Pointer to the PBAP search parameter structure for filtering
 * @return      None
 ****************************************************************************************
 */
void bta_pbap_client_get_phonebook_pb(const bt_bdaddr_t *remote, bt_pbap_request_search_param_t *search_param);

/**
 ****************************************************************************************
 * @brief       Search vCard entries by telephone number from the remote PBAP server
 * @param[in]   remote: Pointer to the Bluetooth address of the remote device
 * @param[in]   search_param: Pointer to the PBAP search parameter structure for filtering
 * @return      True if the search request is sent successfully, False otherwise
 ****************************************************************************************
 */
bool bta_pbap_client_list_search_vcard_by_telephone(const bt_bdaddr_t *remote, bt_pbap_request_search_param_t *search_param);

#ifdef __cplusplus
}
#endif
