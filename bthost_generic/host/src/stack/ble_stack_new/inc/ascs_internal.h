
/***************************************************************************
 *
 * Copyright (c) 2015-2023 BES Technic
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
#ifndef __BAP_ASCS_H__
#define __BAP_ASCS_H__

#include "gaf_prf.h"
#include "gaf_cfg.h"
#include "generic_audio.h"

#include "asc_common.h"

#define ASCS_CONNECTION_MAX                     (GAF_CONNECTION_MAX)

#define ASCS_MAX_SUPP_CIS_NUM                   (GAF_SUPP_MAX_CIS_NUM)

#define ASCS_MAX_SUPP_ASE_NUM_PER_DIRECTION     (7)

/// ASCS callback for read ase char
typedef void (*ascs_cb_get_ase_value_buf)(uint8_t con_lid, uint8_t ase_id,
                                          const uint8_t **p_ase_value_buf, const uint8_t *p_buf_len);
/// ASCS callback for write ase cp char
typedef int (*ascs_cb_set_ase_cp_value)(uint32_t token, uint8_t con_lid, const uint8_t *char_value_buf, uint8_t buf_len);
/// ASCS callback for bond data
typedef void (*ascs_cb_bond_data)(uint8_t con_lid, uint8_t char_type, uint16_t cli_cfg_bf);
/// ASCS callback for character value notify sent, ase_id valid when is_cp_ntf == false, otherwise p_cp_ntf is valid
typedef void (*ascs_cb_val_ntf_sent)(uint8_t con_lid, bool is_cp_ntf, uint8_t ase_id, const ascs_ase_cp_rsp_t *p_cp_ntf);
/// ASCS callback for service disconnect
typedef void (*ascs_cb_disconnected)(uint8_t con_lid);
/// ASCS callback for service connected
typedef void (*ascs_cb_connected)(uint8_t con_lid);

typedef struct ascs_upper_callback
{
    /// value read
    ascs_cb_get_ase_value_buf cb_ase_value_buf_get;
    /// value write
    ascs_cb_set_ase_cp_value cb_ase_cp_write;
    /// cccd read
    ascs_cb_bond_data cb_bond_data;
    /// val ntf sent
    ascs_cb_val_ntf_sent cb_val_ntf_sent;
    /// ascs disconnect
    ascs_cb_disconnected cb_disconnected;
    /// ascs connected
    ascs_cb_connected cb_connected;
} ascs_upper_cb_t;

typedef struct ascs_init_cfg
{
    /// Preferred MTU size
    uint16_t pref_mtu;
    /// Sink ASE number supported |
    /// Source ASE number supported
    uint8_t num_ase_supp_bf;
} ascs_init_cfg_t;

/*FUNCTIONS*/
/**
 * @brief ASCS restore CCCD value
 *
 * @param  con_lid     Connection local index
 * @param  cli_cfg_bf  Client Configuration Bitfield
 *
 * @return int         status
 */
int ascs_restore_cli_cfg_cache(uint8_t con_lid, uint16_t cli_cfg_bf);

/**
 * @brief ASCS init
 *
 * @param  ascs_init_cfg  ASCS init configuration
 * @param  ascs_upper_cb  ASCS upper callback
 *
 * @return int            status
 */
int ascs_init(const ascs_init_cfg_t *ascs_init_cfg, const ascs_upper_cb_t *ascs_upper_cb);

/**
 * @brief ASCS deinit
 *
 * @return int            status
 */
int ascs_deinit(void);

/**
 * @brief ASCS ASE CP RSP add more ASE
 *
 * @param  con_lid        connection link ID
 * @param  ase_id         ASE ID
 * @param  op_code        ASE CP op code
 * @param  rsp_code       ASE CP RSP code
 * @param  reason         ASE CP RSP reason
 *
 * @return int            status
 */
int ascs_ase_cp_rsp_add_more_ase(uint8_t con_lid, uint8_t ase_id, uint8_t op_code,
                                 uint8_t rsp_code, uint8_t reason);

/**
 * @brief  Send ASE CP notify
 *
 * @param  con_lid        connection link ID
 *
 * @return int            status
 */
int ascs_send_ase_cp_notify(uint8_t con_lid);

/**
 * @brief  Send ASE state notify
 *
 * @param  con_lid        connection link ID
 * @param  ase_id         ASE ID
 * @param  state          ASE state
 *
 * @return int            status
 */
int ascs_send_ase_state_notify(uint8_t con_lid, uint8_t ase_id,
                               const uint8_t *p_buf, uint8_t buf_len);

#endif /// __BAP_ASCS__