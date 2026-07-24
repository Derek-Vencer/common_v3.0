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
#ifndef __VAP_VAC_H__
#define __VAP_VAC_H__

#include "gaf_cfg.h"
#include "gaf_prf.h"

#include "va_common.h"

#define VAC_CONNECTION_MAX                  (GAF_CONNECTION_MAX)

#define VAC_CP_NTF_RX_TIMEOUT_DUR_MS        (gaf_prf_get_rx_ntf_ind_timeout_in_ms())

typedef struct vac_prf_svc_info vac_prf_svc_t;
typedef struct vac_resonse vac_resp_t;

/// Callback for vac bond data
typedef void (*vac_cb_bond_data_evt)(uint8_t con_lid, uint8_t va_lid, const vac_prf_svc_t *param);
/// Callback for vac discovery done
typedef void (*vac_cb_discovery_cmp_evt)(uint8_t con_lid, uint16_t err_code);
/// Callback for vac gatt set cfg complete
typedef void (*vac_cb_set_cfg_cmp_evt)(uint8_t con_lid, uint8_t va_lid, uint8_t char_type, uint16_t err_code);
/// Callback for vac cccd cfg value received or read
typedef void (*vac_cb_cfg_evt)(uint8_t con_lid, uint8_t va_lid, uint8_t char_type, bool enabled, uint16_t err_code);
/// Callback for vac op control complete
typedef void (*vac_cb_va_ctrl_cmp_evt)(uint8_t con_lid, uint8_t va_lid, uint8_t op_code, const vac_resp_t *p_resp, uint16_t err_code);
/// Callback for vas ccid or read
typedef void (*vac_cb_va_ccid_val_evt)(uint8_t con_lid, uint8_t va_lid, uint8_t ccid, uint16_t err_code);
/// Callback for vas session flag or read
typedef void (*vac_cb_supp_features_evt)(uint8_t con_lid, uint8_t va_lid, uint8_t supp_feat_bf, uint16_t err_code);
/// Callback for vas session state ntf or read
typedef void (*vac_cb_session_state_evt)(uint8_t con_lid, uint8_t va_lid, uint8_t session_state, uint16_t err_code);
/// Callback for vas session flag or read
typedef void (*vac_cb_session_flag_evt)(uint8_t con_lid, uint8_t va_lid, uint8_t session_flag, uint16_t err_code);
/// Callback for vas session flags or read
typedef void (*vac_cb_char_value_evt)(uint8_t con_lid, uint8_t va_lid, uint8_t char_type, const uint8_t *val, uint16_t val_len, uint16_t err_code);
/// Callback function called when prf status event generated
typedef gatt_prf_status_event_callback vac_cb_prf_status_evt;

/*Structure*/
typedef struct vac_evt_cb
{
    /// Callback function called when client configuration for VAS has been updated
    vac_cb_bond_data_evt cb_bond_data;
    /// Callback function called when VAS is discovered or failed
    vac_cb_discovery_cmp_evt cb_discovery_cmp;
    /// Callback for vac gatt cmd set cfg complete
    vac_cb_set_cfg_cmp_evt cb_set_cfg_cmp;
    /// Callback function called when a cfg of cccd is received
    vac_cb_cfg_evt cb_cfg_value;
    /// Callback function called when a control cmd is complete with status
    vac_cb_va_ctrl_cmp_evt cb_va_ctrl_cmp;
    /// Callback function called when a ccid is received with status
    vac_cb_va_ccid_val_evt cb_va_ccid_val;
    /// Callback function called when a va supp features is received with status
    vac_cb_supp_features_evt cb_va_supp_feat;
    /// Callback function called when a session state is received with status
    vac_cb_session_state_evt cb_session_state;
    /// Callback function called when a session flag is received with status
    vac_cb_session_flag_evt cb_session_flag;
    /// Callback function called when a character value is received with status
    vac_cb_char_value_evt cb_char_val;
    /// Callback function called when prf status event generated
    vac_cb_prf_status_evt cb_prf_status_event;
} vac_evt_cb_t;

typedef struct vac_cfg
{
    /// Max support tbs instant found
    uint8_t max_supp_vas_inst;
    /// Preferred MTU size
    uint16_t pref_mtu;
} vac_cfg_t;

struct vac_prf_svc_info
{
    /// Service handler range
    gatt_prf_svc_range_t svc_range;
    /// Service UUID
    uint16_t uuid;
};

typedef union vac_resp_val
{
    /// CP Response - RSP - val @see vap_cp_resp_op_rsp_value
    uint8_t rsp_val;
} vac_resp_val_u;

struct vac_resonse
{
    /// CP Response - RSP - val @see vap_cp_response
    uint8_t resp_op;
    /// CP Response - RSP - val @see vap_cp_resp_op_rsp_value
    vac_resp_val_u val;
};

/*FUNCTION DECLARATION*/
/**
 * @brief Voice Assistant client initilization
 *
 * @param  p_init_cfg  Initilization configuration
 * @param  p_cb        Event callbacks
 *
 * @return int         status
 */
int vac_init(const vac_cfg_t *p_init_cfg, const vac_evt_cb_t *p_cb);

/**
 * @brief Voice Assistant client deinitilization
 *
 * @return int         status
 */
int vac_deinit(void);

/**
 * @brief Voice Assistant client VAS service discovery
 *
 * @param  con_lid     Connection local index
 *
 * @return int         status
 */
int vac_service_discovery(uint8_t con_lid);

/**
 * @brief Voice Assistant client read character value
 *
 * @param  con_lid     Connection local index
 * @param  va_lid      Voice assistant local index
 * @param  char_type   Character type
 *
 * @return int         status
 */
int vac_character_value_read(uint8_t con_lid, uint8_t va_lid, uint8_t char_type);

/**
 * @brief Voice Assistant client write character cccd
 *
 * @param  con_lid     Connection local index
 * @param  va_lid      Voice assistant local index
 * @param  char_type   Character type
 * @param  enable_ntf  Enable or disable notify
 *
 * @return int         status
 */
int vac_character_cccd_write(uint8_t con_lid, uint8_t va_lid, uint8_t char_type, bool enable_ntf);

/**
 * @brief Voice Assistant client VA control
 *
 * @param  con_lid     Connection local index
 * @param  va_lid      Voice assistant local index
 * @param  op_code     Operation code @see enum va_opcode
 *
 * @return int         status
 */
int vac_va_control(uint8_t con_lid, uint8_t va_lid, uint8_t op_code);

#endif /// __VAP_VAC_INT__