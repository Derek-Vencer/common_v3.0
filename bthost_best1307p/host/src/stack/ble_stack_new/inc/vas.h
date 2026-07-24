/***************************************************************************
 *
 * Copyright (c) 2015-2025 BES Technic
 *
 * Authored by BES CD team (Blueelf Prj).
 *
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
#ifndef __VAP_VAS_H__
#define __VAP_VAS_H__

#include "gaf_prf.h"
#include "gaf_cfg.h"

#include "va_common.h"

#define VAS_CONNECTION_MAX          (GAF_CONNECTION_MAX)

/// Callback for vas bond data
typedef void (*vas_cb_bond_data_evt)(uint8_t va_lid, uint8_t con_lid, uint8_t char_type, uint16_t cli_cfg);
/// Callback function called to inform upper layer that a client device has requested an operation
typedef void (*vas_cb_cp_operation_req)(uint8_t va_lid, uint8_t con_lid, uint8_t opcode);

/*Structure*/
typedef struct p_evt_cb
{
    /// Callback function called when client configuration for (G)VAS has been updated
    vas_cb_bond_data_evt cb_bond_data;
    /// Callback function called to inform upper layer that a client device has requested an operation
    vas_cb_cp_operation_req cb_cp_operation_req;
} vas_evt_cb_t;

typedef struct vas_inst_configurations
{
    /// Preferred MTU size
    uint16_t pref_mtu;
    /// Content Control ID
    uint8_t ccid;
    /// VA supported features @see enum vap_supported_features
    uint8_t va_supp_feat_bf;
    /// Every Sting value max length
    uint8_t per_str_val_len_max;
    /// VA Name length
    uint8_t va_name_len;
    /// Installed loc length
    uint8_t installed_loc_len;
    /// Suppoted languages
    uint8_t supp_languages_len;
    /// VA Name value
    const uint8_t *p_va_name;
    /// Installed loc value
    const uint8_t *p_installed_loc;
    /// Suppoted languages
    const uint8_t *p_supported_languages;
    /// VA UUID
    uint8_t va_uuid[GATT_UUID_LEN_128BITS];
} vas_inst_cfg_t;

typedef struct vas_cfg
{
    /// Generic va service instant config
    const vas_inst_cfg_t *p_gvas_inst_cfg;
    /// Number of VAS instant
    uint8_t nb_vas_inst_supp;
} vas_init_cfg_t;

typedef union vas_char_value
{
    /// Content Control ID
    uint8_t ccid;
    /// VA supported features @see enum vap_supported_features
    uint8_t va_supp_feat_bf;
    /// VA UUID
    uint8_t va_uuid[GATT_UUID_LEN_128BITS];
    /// String value
    struct
    {
        /// Value length
        uint8_t val_len;
        /// VA Name value
        const uint8_t *p_val;
    } str_val;
} vas_update_char_val_u;

/*FUCNTION DECLARATION*/
/**
 * @brief Voice assistant server initilization
 *
 * @param  p_init_cfg  Initilization configuration
 * @param  p_evt_cb    Event callbacks
 *
 * @return int         status
 */
int vas_init(const vas_init_cfg_t *p_init_cfg, const vas_evt_cb_t *p_evt_cb);

/**
 * @brief Voice assistant server deinitilization
 *
 * @return int         status
 */
int vas_deinit(void);

/**
 * @brief Voice assistant server add vas instant
 *
 * @param  p_vas_inst_cfg
 *                     VAS instant configuration
 * @param  p_va_lid_ret
 *                     VAS instant local index return
 *
 * @return int         status
 */
int vas_add_vas_instant(const vas_inst_cfg_t *p_vas_inst_cfg, uint8_t *p_va_lid_ret);

#if (GAF_USE_CACHE_GATT_CCCD)
/**
 * @brief Voice assistant server restore client configuration bitfield
 *
 * @param  con_lid     Connection local index
 * @param  va_lid      VA local index
 * @param  cli_cfg_bf  client configuration bitfield
 *
 * @return int         status
 */
int vas_restore_cli_cfg_cache(uint8_t con_lid, uint8_t va_lid, uint16_t cli_cfg_bf);
#endif /// (GAF_USE_CACHE_GATT_CCCD)

/**
 * @brief Voice assistant server confirm VA CP operation request
 *
 * @param  rsp_val     CP response Val
 * @param  va_lid      VA local index
 *
 * @return int         status
 */
int vas_va_cp_operation_req_cfm(enum vap_va_cp_resp_op_rsp_value rsp_val, uint8_t va_lid);

/**
 * @brief Voice assistant server set VA session state
 *
 * @param  con_lid     Connection local index
 * @param  va_lid      VA local index
 * @param  state       Session state @see vap_session_state
 * @param  flag        Session flag @see vap_session_flag
 *
 * @return int         status
 */
int vas_set_va_session_state_flag(uint8_t con_lid, uint8_t va_lid,
                                  enum vap_session_state state, enum vap_session_flag flag);

/**
 * @brief Voice assistant server update character value except session state and session flag character
 *
 * @param[in] va_lid   VA local index
 * @param[in] char_type
 *                     Character type @see enum vas_char_type
 * @param[in] p_update_value
 *                     Value that should be updated to
 *
 * @return int         status
 */
int vas_update_character_value_req(uint8_t va_lid, uint8_t char_type,
                                   const vas_update_char_val_u *p_update_value);

#endif /// __VAP_VAS__