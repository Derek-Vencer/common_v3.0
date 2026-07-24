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
#ifndef __ATC_CSISM_H__
#define __ATC_CSISM_H__
#include "gaf_cfg.h"
#include "csis.h"

/**
 * @brief Callback triggered when bonding-related data is updated.
 *
 * Invoked when the client writes to a CSISM characteristic that affects
 * bonding information (e.g., Client Characteristic Configuration updates).
 *
 * @param[in] con_lid            Local connection index.
 * @param[in] char_type_written  Type of characteristic written @see csis_character_type.
 * @param[in] cli_cfg_bf         Client configuration bitfield (e.g., CCCD notify settings).
 */
typedef void (*csism_atc_cb_bond_data)(uint8_t con_lid, uint8_t char_type_written,
                                       uint16_t cli_cfg_bf);

/**
 * @brief Callback triggered after a characteristic value has been sent.
 *
 * Called when a Read Response or a Notification is successfully
 * delivered to the client.
 *
 * @param[in] con_lid        Local connection index.
 * @param[in] is_read_rsp    True if this callback corresponds to a Read Response;
 *                           false for Notification.
 * @param[in] char_type      Characteristic type @see csis_character_type.
 * @param[in] p_val_rsp      Pointer to the value that was sent.
 * @param[in] val_len        Length of the value.
 */
typedef void (*csism_atc_cb_char_val_sent)(uint8_t con_lid, bool is_read_rsp,
                                           uint8_t char_type, const uint8_t *p_val_rsp, uint8_t val_len);

/**
 * @brief Callback triggered when LTK (Long Term Key) is required.
 *
 * Invoked when the CSISM server requires encryption and requests
 * the upper layer to provide or retrieve the LTK.
 *
 * @param[in] con_lid        Local connection index.
 */
typedef void (*csism_atc_cb_ltk_request)(uint8_t con_lid);

/**
 * @brief Callback triggered when RSI (Resolvable Set Identity) generation completes.
 *
 * Called after a new RSI value is generated
 *
 * @param[in] err_code       Error code (0 indicates success).
 * @param[in] rsi            Pointer to the generated 6-byte RSI value.
 */
typedef void (*csism_atc_cb_rsi_generated)(uint16_t err_code, uint8_t *rsi);

/**
 * @brief Callback triggered when the lock state changes.
 *
 * Invoked when the CSISM server lock/unlock state is updated by the client.
 *
 * @param[in] con_lid        Local connection index.
 * @param[in] lock           True if locked; false if unlocked.
 */
typedef void (*csism_atc_cb_lock_state)(uint8_t con_lid, bool lock);

/**
 * @brief CSISM ATC event callback structure.
 *
 * Aggregates all upper-layer callbacks related to CSISM server events.
 */
typedef struct csism_atc_evt_callback
{
    /// Bonding data update callback
    csism_atc_cb_bond_data     cb_bond_data;
    /// Characteristic value sent callback
    csism_atc_cb_char_val_sent cb_val_sent;
    /// LTK request callback
    csism_atc_cb_ltk_request   cb_ltk_req;
    /// RSI generated callback
    csism_atc_cb_rsi_generated cb_rsi;
    /// Lock state change callback
    csism_atc_cb_lock_state    cb_lock;
} csism_atc_evt_cb_t;

typedef struct
{
    /// CSIS rank of this member (0 = primary, >0 = secondary members)
    uint8_t rank;
    /// Size (number of members) in the Coordinated Set
    uint8_t size;
    /// Bitfield indicating which locks are held/referenced
    uint8_t lock_bf;
    /// Set Identity Resolving Key (SIRK)
    uint8_t sirk[CSIS_SIRK_VAL_LEN];
    /// Whether SIRK needs to be encrypted before being exposed to peer
    bool sirk_encrypt_needed;
    /// Preferred ATT MTU size for this coordinated set member
    uint16_t pref_mtu;
    /// If CSISM supports the Coordinated set name characteristic
    /// set_name_len_supp_max != 0, otherwise leave it to zero
    /// @see CSIS_SET_NAME_LEN_MAX to get the max length supported
    uint8_t set_name_len_supp_max;
    /// Length of Coordinated set name
    uint8_t set_name_len;
    /// Coordinated set name value
    const uint8_t *set_name_val;
} csism_atc_init_cfg_t;

/*FUNCTIONS DECLARATION*/
/**
 * @brief ATC set member initilization
 *
 * @param  p_cfg       Initilization configuration
 * @param  p_cb        Event callbacks
 * @param  set_lid_ret Set local index return
 *
 * @return int         status
 */
int csism_atc_init(csism_atc_init_cfg_t *p_cfg, const csism_atc_evt_cb_t *p_cb,
                   uint8_t *set_lid_ret);

/**
 * @brief ATC set member deinitilization
 *
 * @return int         status
 */
int csism_atc_deinit(void);

/**
 * @brief ATC set member restore client configuration
 *
 * @param  con_lid     Connection local index
 * @param  lock        Lock value
 * @param  cli_cfg_bf  Client Configuration Bitfield
 *
 * @return int         status
 */
int csism_atc_restore_cli_cfg_cache(uint8_t con_lid, bool lock, uint8_t cli_cfg_bf);

/**
 * @brief ATC set member set SIRK value
 *
 * @param  sirk        Value
 * @param  enc_sirk_needed
 *                     Present encrypted or plain text sirk
 *
 * @return int         status
 */
int csism_atc_set_sirk(uint8_t *sirk, bool enc_sirk_needed);

/**
 * @brief ATC set member set size value
 *
 * @param  size        Size value
 *
 * @return int         status
 */
int csism_atc_set_size(uint8_t size);

/**
 * @brief ATC set member set rank value
 *
 * @param  rank        Rank value
 *
 * @return int         status
 */
int csism_atc_set_rank(uint8_t rank);

/**
 * @brief ATC set member set lock value
 *
 * @param  con_lid     Connection local index
 * @param  lock        Lock value
 *
 * @return int         status
 */
int csism_atc_set_lock(uint8_t con_lid, bool lock);

/**
 * @brief ATC set member set Name value
 *
 * @param  p_set_name  Coordinated Set Name value
 * @param  set_name_len
 *                     Length of Coordinated Set Name value
 *
 * @return int         status
 */
int csism_atc_set_name(uint8_t con_lid, const uint8_t *p_set_name, uint8_t set_name_len);

/**
 * @brief ATC set member generate RSI value
 *
 * @return int         status
 */
int csism_atc_generate_rsi(void);

/**
 * @brief ATC confirm the ltk value request
 *
 * @param  con_lid     Connection local index
 * @param  ltk         LTK value if found, NULL means not found
 *
 * @return int         status
 */
int csism_atc_ltk_upper_cfm(uint8_t con_lid, const uint8_t *ltk);

#endif /// __ATC_CSISM__
