/**
 ****************************************************************************************
 *
 * @file app_bap_uc_cli_msg.h
 *
 * @brief BLE Audio Audio Stream Control Service Client
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

#ifndef __APP_BAP_UC_CLI_MSG_H__
#define __APP_BAP_UC_CLI_MSG_H__
#if BLE_AUDIO_ENABLED
#include "app_bap.h"

#define APP_BAP_DFT_ASCC_NB_SINK_ASE              (2)
#define APP_BAP_DFT_ASCC_NB_SRC_ASE               (2)
#define APP_BAP_DFT_ASCC_NB_ASE_CFG               (APP_BAP_DFT_ASCC_NB_SINK_ASE + APP_BAP_DFT_ASCC_NB_SRC_ASE)
#define APP_BAP_DFT_ASCC_TOTAL_NB_ASE_CFG         (APP_BAP_DFT_ASCC_NB_ASE_CFG * BLE_AUDIO_CONNECTION_CNT)

#define APP_BAP_DFT_ASCC_CIG_NUM_MAX              (2)

#define APP_BAP_DFT_ASCC_SUPP_PHY_BF              APP_PHY_ALL
#define APP_BAP_DFT_ASCC_PRFE_PHY                 APP_PHY_2MBPS_VALUE
#define APP_BAP_DFT_ASCC_PRFE_RATES_BF            APP_GAF_PHY_RATES_BIT_LE_HDT_3
#define APP_BAP_DFT_ASCC_DP_ID                    0
#define APP_BAP_DFT_ASCC_CTL_DELAY_US             0x0102
#define APP_BAP_DFT_ASCC_PACKING_TYPE             APP_ISO_PACKING_SEQUENTIAL
#define APP_BAP_DFT_ASCC_FRAMING_TYPE             APP_ISO_UNFRAMED_MODE
#define APP_BAP_DFT_ASCC_SCA                      4
/// we prepare two cis in cig for rejoin situation
#define APP_BAP_DFT_ASCC_CIS_NUM                  2

#define APP_BAP_DFT_ASE_STATE_TO_MS               (2000)

/// QoS Configuration structure (short)
typedef struct app_bap_uc_cli_qos_cfg
{
    /// PHY
    uint8_t phy;
    /// Maximum number of retransmissions for each CIS Data PDU
    /// From 0 to 15
    uint8_t retx_nb;
    /// Maximum SDU size
    /// From 0 to 4095 bytes (0xFFF)
    uint16_t max_sdu_size;
    /// Presentation delay in microseconds
    uint32_t pres_delay_us;
} app_bap_uc_cli_qos_cfg_t;

/// ASCC ASE Information structure
typedef struct app_bap_ascc_ase
{
    /// ASE local index
    uint8_t ase_lid;
    /// ASE State
    uint8_t ase_state;
    /// Connection local index
    uint8_t con_lid;
    /// ASE instance index
    uint8_t ase_instance_idx;
    /// ASE Direction
    enum app_gaf_direction direction;
    /// Codec ID
    app_gaf_codec_id_t codec_id;
    /// Pointer to Codec Configuration
    app_gaf_bap_cfg_t *p_cfg;
    /// QoS configuration
    app_bap_uc_cli_qos_cfg_t qos_cfg;
    /// Pointer to Metadata structure
    app_gaf_bap_cfg_metadata_t *p_metadata;
    /// CIG index
    uint8_t cig_id;
    /// CIS index
    uint8_t cis_id;
    /// CIS Connection Handle
    uint16_t cis_hdl;
    /// current audio stream type, @see gaf_bap_context_type_bf
    //uint16_t context_bf;
    /// CIG sync delay in us
    uint32_t cig_sync_delay;
    /// CIS sync delay in us
    uint32_t cis_sync_delay;
    /// iso interval in us
    uint32_t iso_interval_us;
    /// bn count from master to slave
    uint32_t bn_m2s;
    /// bn count from slave to master
    uint32_t bn_s2m;
} app_bap_ascc_ase_t;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef AOB_MOBILE_ENABLED
uint8_t app_bap_uc_cli_iso_send_data_to_all_channel(uint8_t **payload, uint16_t payload_len, uint32_t ref_time);
int app_bap_uc_cli_update_metadata(uint8_t ase_lid, app_gaf_bap_cfg_metadata_t *metadata);
int app_bap_uc_cli_prepare_cig_param(uint8_t grp_lid, const app_gaf_cig_config_t *cig_param,
                                     uint8_t cis_num, const app_gaf_cis_config_t *opt_cis_param);
/// For HDT
int app_bap_uc_cli_set_preferred_phy_rates(uint8_t phy, uint8_t rates_bf, uint8_t pkt_fmt);

int app_bap_uc_cli_discovery_start(uint8_t con_lid);
#endif

#ifdef __cplusplus
}
#endif

#endif
#endif // APP_BAP_UC_CLI_MSG_H_

/// @} APP_BAP
