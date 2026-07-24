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

#include "bt_le_types.h"
#include "bt_lea_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 ****************************************************************************************
 *          ____  _           ____  _     _____      _    ____ ___
 *         | __ )| |_ __ _   | __ )| |   | ____|    / \  |  _ \_ _|
 *         |  _ \| __/ _` |  |  _ \| |   |  _|     / _ \ | |_) | |
 *         | |_) | || (_| |  | |_) | |___| |___   / ___ \|  __/| |
 *         |____/ \__\__,_|  |____/|_____|_____| /_/   \_\_|  |___|
 *
 ****************************************************************************************
 */

/* --------------------------------------------------------------------------
 * BLE Core Event & Connection
 * ------------------------------------------------------------------------*/

/**
 * @brief       Register BLE core event callback
 * @param[in]   cb  Callback function to receive BLE core events
 */
void bta_ble_register_event_callback(bt_ble_core_evt_handler_func cb);

/**
 * @brief       Unregister BLE core event callback
 * @param[in]   cb  Previously registered callback function
 */
void bta_ble_unregister_event_callback(bt_ble_core_evt_handler_func cb);

/**
 * @brief       Get BLE device address by connection index
 * @param[in]   conidx  Connection index
 * @param[out]  addr    Pointer to address structure to fill
 * @return      true if address found, false otherwise
 */
bool bta_ble_get_addr_by_conidx(uint8_t conidx, ble_bdaddr_t *addr);

/**
 * @brief       Get connection index by BLE device address
 * @param[in]   addr  BLE device address
 * @return      Connection index if found, otherwise invalid index
 */
uint8_t bta_ble_get_conidx_by_addr(const ble_bdaddr_t *addr);

/**
 * @brief       Get connection handle by connection index
 * @param[in]   conidx  Connection index
 * @return      Connection handle
 */
uint16_t bta_ble_get_conhdl_by_conidx(uint8_t conidx);

/**
 * @brief       Check if a BLE connection is active by device address
 * @param[in]   addr  BLE device address
 * @return      true if connected, false otherwise
 */
bool bta_ble_is_connection_on_by_addr(const ble_bdaddr_t *addr);

/**
 * @brief       Disconnect BLE device by address
 * @param[in]   addr  BLE device address
 * @return      0 if successful, non-zero otherwise
 */
int bta_ble_disconnect(const ble_bdaddr_t *addr);

/**
 * @brief       Disconnect all active BLE connections
 * @return      0 if successful, non-zero otherwise
 */
int bta_ble_disconnect_all(void);

/**
 * @brief       Update BLE connection parameters for a given device
 * @param[in]   addr                  BLE device address
 * @param[in]   min_intv_ms           Minimum connection interval in milliseconds
 * @param[in]   max_intv_ms           Maximum connection interval in milliseconds
 * @param[in]   superv_timeout_ms     Supervision timeout in milliseconds
 * @param[in]   max_peripheral_latency
 *                                       Maximum peripheral latency
 * @return      0 if successful, non-zero otherwise
 */
int bta_ble_update_conn_param(const ble_bdaddr_t *addr, uint32_t min_intv_ms, uint32_t max_intv_ms,
                              uint32_t superv_timeout_ms, uint8_t max_peripheral_latency);

/**
 * @brief       Get current connection interval for a device
 * @param[in]   addr  BLE device address
 * @return      Connection interval in units
 */
uint16_t bta_ble_get_conn_interval(const ble_bdaddr_t *addr);

/**
 * @brief       Remove bonding information for a device
 * @param[in]   addr  BLE device address
 * @return      0 if successful, non-zero otherwise
 */
int bta_ble_remove_bond_info(const ble_bdaddr_t *addr);

/**
 * @brief       Send pairing response to a device
 *
 * @param[in] addr     BLE device address
 * @param[in] require  SMP Pairing requirements
 * @param[in] error    SMP Pairing failed code if upper want to reject request
 *
 * @return      0 if successful, non-zero otherwise
 */
int bta_ble_send_pairing_rsp(const ble_bdaddr_t *addr, const bt_pairing_reqiure_t *require, uint8_t error);

/**
 * @brief       Send pairing user data cfm to a device
 *
 * @param[in] addr     BLE device address
 * @param[in] type     SMP Pairing user data type
 * @param[in] p_input  SMP Pairing user data input
 *
 */
int bta_ble_send_user_input_rsp(const ble_bdaddr_t *addr,
                                bt_ble_smp_input_type_e type, const bt_ble_smp_input_t *p_input);

/* --------------------------------------------------------------------------
 * BLE Local Device Info
 * ------------------------------------------------------------------------*/
/**
 * @brief       Set local BLE device configurations
 * @param[in]   p_cfg configurations
 * @return      0 if successful, non-zero otherwise
 */
int bta_ble_set_device_config(const bt_ble_dev_cfg_t *p_cfg);

/**
 * @brief       Set local BLE device name
 * @param[in]   name  Name string
 * @param[in]   len   Name length
 * @return      0 if successful, non-zero otherwise
 */
int bta_ble_set_local_name(const char *name, uint8_t len);

/**
 * @brief       Get local BLE device name
 * @param[out]  name_buf  Buffer to receive local name
 * @param[in]   buf_len   Buffer length
 * @return      0 if successful, non-zero otherwise
 */
int bta_ble_get_local_name(char *name_buf, uint8_t buf_len);

/**
 * @brief       Set local BLE address
 * @param[in]   addr  Local BLE address
 * @return      0 if successful, non-zero otherwise
 */
int bta_ble_set_local_addr(const ble_bdaddr_t *addr);

/**
 * @brief       Get local BLE address
 * @param[out]  addr  Pointer to address structure to fill
 * @return      0 if successful, non-zero otherwise
 */
int bta_ble_get_local_addr(ble_bdaddr_t *addr);

/**
 * @brief       Get local IRK (Identity Resolving Key)
 * @param[out]  irk  Pointer to IRK structure to fill
 * @return      0 if successful, non-zero otherwise
 */
int bta_ble_get_local_irk(bt_ble_gap_irk_t *irk);

/**
 * @brief       Get peer IRK by BLE address
 * @param[in]   addr  Peer device address
 * @param[out]  irk   Pointer to IRK structure to fill
 * @return      0 if successful, non-zero otherwise
 */
int bta_ble_get_peer_irk_by_addr(const ble_bdaddr_t *addr, bt_ble_gap_irk_t *irk);

/**
 * @brief       Add device to resolving list
 * @param[in]   addr  Peer device address
 * @param[in]   irk   Peer IRK
 * @return      0 if successful, non-zero otherwise
 */
int bta_ble_add_resolving_list(const ble_bdaddr_t *addr, const bt_ble_gap_irk_t *irk);

/**
 * @brief       Remove all devices from resolving list
 * @return      0 if successful, non-zero otherwise
 */
int bta_ble_clear_resolving_list(void);

/**
 * @brief       Add device to filter accept list
 * @param[in]   addr  Peer device address
 * @return      0 if successful, non-zero otherwise
 */
int bta_ble_add_filter_accept_list(const ble_bdaddr_t *addr);

/**
 * @brief       Remove all devices from filter accept list
 * @return      0 if successful, non-zero otherwise
 */
int bta_ble_clear_filter_accept_list(void);

/* --------------------------------------------------------------------------
 * BLE Custom Advertising
 * ------------------------------------------------------------------------*/

/**
 * @brief       Write custom advertising data
 * @param[in]   param  Custom advertising parameter structure
 * @return      0 if successful, non-zero otherwise
 */
int bta_ble_custom_adv_write_info(bt_ble_gap_cus_adv_param_t *param);

/**
 * @brief       Start custom advertising
 * @param[in]   actv_user  Advertising activity type / user
 * @return      0 if successful, non-zero otherwise
 */
int bta_ble_custom_adv_start(bt_ble_gap_adv_activity_t actv_user);

int bta_ble_custom_adv_start_with_policy(bt_ble_gap_adv_activity_t actv_user, uint8_t filter_policy);

/**
 * @brief       Stop custom advertising
 * @param[in]   actv_user  Advertising activity type / user
 * @return      0 if successful, non-zero otherwise
 */
int bta_ble_custom_adv_stop(bt_ble_gap_adv_activity_t actv_user);

/**
 * @brief       Check if custom advertising is enabled
 * @param[in]   actv_user  Advertising activity type / user
 * @return      true if advertising is active, false otherwise
 */
bool bta_ble_custom_adv_is_enabled(bt_ble_gap_adv_activity_t actv_user);

/**
 * @brief       Deregister BLE advertising for a specific user
 * @param[in]   user  Advertising user identifier
 * @return      0 if successful, non-zero otherwise
 */
int bta_ble_deregister_adv(bt_ble_gap_adv_user_t user);

/**
 * @brief       Force enable or disable BLE advertising for a specific user
 * @param[in]   user    Advertising switch user identifier
 * @param[in]   enable  true to enable advertising, false to disable
 * @return      0 if successful, non-zero otherwise
 */
int bta_ble_force_switch_adv(bt_ble_adv_switch_user_t user, bool enable);

/**
 ****************************************************************************************
 *         ____  _           _     _____    _         _    ____ ___
 *        | __ )| |_ __ _   | |   | ____|  / \       / \  |  _ \_ _|
 *        |  _ \| __/ _` |  | |   |  _|   / _ \     / _ \ | |_) | |
 *        | |_) | || (_| |  | |___| |___ / ___ \   / ___ \|  __/| |
 *        |____/ \__\__,_|  |_____|_____/_/   \_\ /_/   \_\_|  |___|
 *
 ****************************************************************************************
 */

/* --------------------------------------------------------------------------
 * LE Audio / Media / Call / Volume / Microphone
 * ------------------------------------------------------------------------*/

/**
 * @brief       Register LE Audio event callback
 * @param[in]   cb  Callback function
 */
void bta_lea_register_event_callback(bt_lea_evt_callback cb);

/**
 * @brief       Check if any LE Audio device is connected
 * @return      true if any LEA device connected, false otherwise
 */
bool bta_lea_is_any_lea_device_connected(void);

/**
 * @brief       Media control functions (play, pause, forward, backward, fast forward/reverse)
 * @param[in]   addr  Device address
 * @return      0 if successful, non-zero otherwise
 */
int bta_lea_mcp_media_play(const ble_bdaddr_t *addr);
int bta_lea_mcp_media_pause(const ble_bdaddr_t *addr);
int bta_lea_mcp_media_next_track(const ble_bdaddr_t *addr);
int bta_lea_mcp_media_prev_track(const ble_bdaddr_t *addr);
int bta_lea_mcp_media_fast_fw(const ble_bdaddr_t *addr);
int bta_lea_mcp_media_fast_rw(const ble_bdaddr_t *addr);

/**
 * @brief       Call control functions (answer, terminate, hold, retrieve, originate, join)
 * @param[in]   addr     Device address
 * @param[in]   call_id  Call identifier
 * @return      0 if successful, non-zero otherwise
 */
int bta_lea_ccp_call_answer(const ble_bdaddr_t *addr, uint8_t call_id);
int bta_lea_ccp_call_terminate(const ble_bdaddr_t *addr, uint8_t call_id);
int bta_lea_ccp_call_hold(const ble_bdaddr_t *addr, uint8_t call_id);
int bta_lea_ccp_call_retrieve(const ble_bdaddr_t *addr, uint8_t call_id);
int bta_lea_ccp_call_originate(const ble_bdaddr_t *addr, uint8_t *uri, uint8_t uri_len);
int bta_lea_ccp_call_join(const ble_bdaddr_t *addr, uint8_t *call_ids, uint8_t call_num);


/**
 * @brief       Get the number of connected LE Audio device.
 *
 * @return uint8_t the number of connected LE Audio device
 */
uint8_t bta_lea_get_connected_dev_num(void);

/**
 * @brief       Check if device is in active call
 * @param[in]   addr  Device address
 * @return      true if calling, false otherwise
 */
bool bta_lea_ccp_is_call_active(const ble_bdaddr_t *addr);

/**
 * @brief       Volume control functions
 * @return      0 if successful, non-zero otherwise
 */
int bta_lea_vcp_volume_mute(void);
int bta_lea_vcp_volume_unmute(void);
int bta_lea_vcp_volume_up(const ble_bdaddr_t *addr);
int bta_lea_vcp_volume_down(const ble_bdaddr_t *addr);

/**
 * @brief       Convert LE volume to local volume
 * @param[in]   le_vol  LE volume
 * @return      Local volume
 */
uint8_t bta_lea_vcp_convert_le_vol_to_local_vol(uint8_t le_vol);

/**
 * @brief       Get real-time volume of remote device
 * @param[in]   addr  Device address
 * @return      Current volume
 */
uint8_t bta_lea_vcp_get_real_time_volume(const ble_bdaddr_t *addr);

/**
 * @brief       Get microphone state of a device
 * @param[in]   addr  Device address
 * @return      Microphone state
 */
uint8_t bta_lea_micp_get_mic_state(const ble_bdaddr_t *addr);

/**
 * @brief       Set microphone mute state
 * @param[in]   mute  true to mute, false to unmute
 * @return      0 if successful, non-zero otherwise
 */
int bta_lea_micp_set_mute(uint8_t mute);

/**
 * @brief       Get the RSI for CSIP (Coordinated Set Identification Profile)
 * @param[out]  rsi  Pointer to RSI structure to fill
 * @return      0 if successful, non-zero otherwise
 */
int bta_lea_csip_get_rsi(bt_lea_rsi_t *rsi);

uint8_t bta_lea_mcp_get_media_state(const ble_bdaddr_t *addr);

/**
 * @brief       Set whether the SIRK (Set Identity Resolving Key) is encrypted for CSIP
 * @param[in]   encrypted  true to enable encrypted sirk, false plain text sirk
 * @return      0 if successful, non-zero otherwise
 */
int bta_lea_csip_set_sirk_encrypted(bool encrypted);

/**
 * @brief       Set codec configuration for a BAP ASE
 * @param[in]   ase_lid       ASE local identifier
 * @param[in]   codec_id      Codec identifier structure
 * @param[in]   ntf_qos_req   Pointer to QoS request for notification
 * @param[in]   ntf_codec_cfg Pointer to codec configuration for notification
 * @return      0 if successful, non-zero otherwise
 */
int bta_lea_bap_set_codec_config(uint8_t ase_lid, const bt_lea_codec_id_t *codec_id,
                                 const bt_lea_qos_req_t *ntf_qos_req,
                                 const bt_lea_codec_cfg_t *ntf_codec_cfg);

/**
 * @brief       Get ASE (Audio Stream Endpoint) information for a given ASE LID
 * @param[in]   ase_lid    ASE local identifier
 * @param[out]  ase_info   Pointer to ASE information structure to fill
 * @return      0 if successful, non-zero otherwise
 */
int bta_lea_bap_get_ase_info(uint8_t ase_lid, bt_lea_ascs_ase_t *ase_info);

/**
 * @brief       Get the current LE Audio context type by ASE LID
 * @param[in]   ase_lid  ASE local identifier
 * @return      Current context type for the ASE
 */
bt_lea_context_type_t bta_lea_get_curr_context_bf_by_ase_lid(uint8_t ase_lid);

/**
 * @brief       Enable or Disable LEA Feature.
 *
 * @param[in] enable Enable or Disable
 */
void bta_lea_switch(bool enable);

/**
 * @brief       Delete cached LEA GATT service data on a device
 * @param[in]   addr      BLE device address
 * @param[in]   svc_uuid  Service UUID to delete from cache
 * @return      0 if successful, non-zero otherwise
 */
int bta_lea_gattc_delete_cache(const ble_bdaddr_t *addr, uint32_t svc_uuid);

/**
 * @brief       Delete all cached LEA GATT services for all devices
 * @return      0 if successful, non-zero otherwise
 */
int bta_lea_gattc_delete_all_cache(void);

/**
 * @brief       Check if a remote BLE device supports LE Audio (LEA)
 * @param[in]   addr  Remote BLE device address
 * @return      true if the device supports LE Audio, false otherwise
 */
bool bta_is_remote_support_lea(const ble_bdaddr_t *addr);

#ifdef __cplusplus
}
#endif
