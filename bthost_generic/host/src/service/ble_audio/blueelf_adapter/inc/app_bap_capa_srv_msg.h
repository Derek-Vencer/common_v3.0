/**
 ****************************************************************************************
 *
 * @file app_bap_capa_srv_msg.h
 *
 * @brief BLE Audio Published Audio Capabilities Server
 *
 * Copyright 2015-2019 BES.
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
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP_BAP
 * @{
 ****************************************************************************************
 */

#ifndef __APP_BAP_CAPA_SRV_MSG_H__
#define __APP_BAP_CAPA_SRV_MSG_H__
#if BLE_AUDIO_ENABLED
#include "app_bap.h"

/// Available Context information structure
typedef struct app_bap_capa_ava_context
{
    /// Connection Local Index
    uint8_t con_lid;
    /// Available Context Type Bitfield
    uint16_t ava_bf;
} app_bap_capa_ava_context_t;

/// Direction information structure
typedef struct app_bap_capa_srv_dir
{
    /// Supported Audio Locations for the direction
    uint32_t location_bf;
    /// Available Audio Contexts for the direction (one for each connection)
    uint16_t context_bf_ava[BLE_CONNECTION_MAX];
    /// Supported Audio Contexts for the direction
    uint16_t context_bf_supp;
    /// Number of PACs for the direction
    uint8_t nb_pacs;
} app_bap_capa_srv_dir_t;

/// PAC Record information structure
typedef struct app_bap_capa_srv_record
{
    /// List header
    struct list_node hdr;
    /// Record identifier
    uint8_t record_id;
    /// Codec ID
    app_gaf_codec_id_t codec_id;
    /// Pointer to Codec Capabilities structure (allocated by Upper Layer)
    app_gaf_bap_capa_t *p_capa;
    /// Pointer to Codec Capabilities Metadata structure (allocated by Upper Layer)
    app_gaf_bap_capa_metadata_t *p_metadata;
} app_bap_capa_srv_record_t;

#ifdef __cplusplus
extern "C" {
#endif
int app_bap_capa_srv_restore_bond_data_req(uint8_t con_lid, uint8_t cli_cfg_bf, uint8_t pac_cli_cfg_bf);

uint8_t app_bap_capa_srv_get_valid_pac_lid(const app_gaf_codec_id_t *p_codec_id, uint8_t direction);

int app_bap_capa_srv_add_pac_v2_record(uint8_t direction, app_gaf_codec_id_t *codec_id,
                                       app_gaf_bap_capa_t *p_capa, app_gaf_bap_capa_metadata_t *p_metadata);

int app_bap_capa_srv_delete_pac_v2_record(uint8_t record_lid);

int app_bap_capa_srv_add_chan_type_record(uint32_t channel_type, const uint8_t *p_desc_val, uint8_t val_len);

int app_bap_capa_srv_delete_chan_type_record(uint8_t record_lid);

int app_bap_capa_srv_add_pref_aud_cfg_record(uint16_t use_case_id, uint16_t data_present_bits,
                                             const app_gaf_pref_aud_cfg_data_t *p_pref_aud_cfg_data);

int app_bap_capa_srv_delete_pref_aud_cfg_record(uint8_t record_lid);

int app_bap_capa_srv_delete_pref_aud_cfg_record_by_use_case(uint16_t use_case_id);
#ifdef __cplusplus
}
#endif

#endif
#endif // APP_BAP_CAPA_SRV_MSG_H_

/// @} APP_BAP
