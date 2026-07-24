/**
 ****************************************************************************************
 *
 * @file app_bap_capa_cli_msg.h
 *
 * @brief BLE Audio Published Audio Capabilities Client
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

#ifndef __APP_BAP_CAPA_CLI_MSG_H__
#define __APP_BAP_CAPA_CLI_MSG_H__
#if BLE_AUDIO_ENABLED
#include "bluetooth.h"
#include "app_bap.h"

#define APP_BAP_DFT_CAPA_C_SINK_SUPP_NUN          (3)
#define APP_BAP_DFT_CAPA_C_SRC_SUPP_NUN           (3)

#define APP_BAP_DFT_CAPA_C_SINK_LOCATION_BF       (APP_GAF_LOC_SIDE_RIGHT_BIT | APP_GAF_LOC_SIDE_LEFT_BIT)
#define APP_BAP_DFT_CAPA_C_SRC_LOCATION_BF        (APP_GAF_LOC_SIDE_RIGHT_BIT | APP_GAF_LOC_SIDE_LEFT_BIT)
#define APP_BAP_DFT_CAPA_C_SINK_CONTEXT_BF        (APP_GAF_BAP_CONTEXT_TYPE_MEDIA_BIT | APP_GAF_BAP_CONTEXT_TYPE_RINGTONE_BIT | APP_GAF_BAP_CONTEXT_TYPE_CONVERSATIONAL_BIT | APP_GAF_BAP_CONTEXT_TYPE_GAME_BIT)
#define APP_BAP_DFT_CAPA_C_SRC_CONTEXT_BF         (APP_GAF_BAP_CONTEXT_TYPE_MEDIA_BIT | APP_GAF_BAP_CONTEXT_TYPE_RINGTONE_BIT | APP_GAF_BAP_CONTEXT_TYPE_CONVERSATIONAL_BIT | APP_GAF_BAP_CONTEXT_TYPE_GAME_BIT)
#define APP_BAP_DFT_CAPA_C_SAMPLING_FREQ_BF       (APP_GAF_BAP_SAMPLING_FREQ_96000HZ_BIT | APP_GAF_BAP_SAMPLING_FREQ_48000HZ_BIT |\
                                                    APP_GAF_BAP_SAMPLING_FREQ_24000HZ_BIT | APP_GAF_BAP_SAMPLING_FREQ_32000HZ_BIT | APP_GAF_BAP_SAMPLING_FREQ_16000HZ_BIT)
#define APP_BAP_DFT_CAPA_C_FRAME_DURATION_BF      (APP_GAF_BAP_FRAME_DUR_10MS_BIT | APP_GAF_BAP_FRAME_DUR_7_5MS_BIT)
#define APP_BAP_DFT_CAPA_C_FRAME_OCT_MIN          20
#define APP_BAP_DFT_CAPA_C_FRAME_OCT_MAX          1000

#ifdef LEA_LHDC
#define APP_BAP_DFT_CAPA_C_LHDC_CODEC_ID               "\xFF\x35\x4C\x05\x3A"
#define APP_BAP_DFT_CAPA_C_LHDC_SAMPLING_FREQ_BF       (APP_GAF_BAP_SAMPLING_FREQ_48000HZ_BIT | APP_GAF_BAP_SAMPLING_FREQ_96000HZ_BIT)
#define APP_BAP_DFT_CAPA_C_LHDC_FRAME_DURATION_BF      (APP_GAF_BAP_FRAME_DUR_10MS_BIT | APP_GAF_BAP_FRAME_DUR_7_5MS_BIT | APP_GAF_BAP_FRAME_DUR_5MS_BIT | APP_GAF_BAP_FRAME_DUR_2_5MS_BIT)
#define APP_BAP_DFT_CAPA_C_LHDC_CHAN_CNT_BF            2
#define APP_BAP_DFT_CAPA_C_LHDC_FRAME_OCT_MIN          160
#define APP_BAP_DFT_CAPA_C_LHDC_FRAME_OCT_MAX          1000
#define APP_BAP_DFT_CAPA_C_LHDC_MAX_FRAMES_SDU         1
#endif

/// PAC Record information structure
typedef struct app_bap_capa_cli_supp
{
    /// Supported Audio Locations for the direction
    uint32_t location_bf;
    /// Supported Audio Contexts for the direction
    uint16_t context_bf_supp;
} app_bap_capa_cli_supp_t;

/// PAC Record information structure
typedef struct app_bap_capa_record_cfg
{
    /// List header
    struct list_node hdr;
    /// Configuration identifier
    uint8_t cfg_id;
    /// Codec ID
    app_gaf_codec_id_t codec_id;
    /// Codec Capabilities Length
    uint8_t cfg_len;
    /// Pointer to Codec Capabilities structure
    app_gaf_bap_cfg_t *p_cfg;
    /// Metadata Length
    uint8_t metadata_len;
    /// Pointer to Codec Capabilities Metadata structure
    app_gaf_bap_cfg_metadata_t *p_metadata;
} app_bap_capa_record_cfg_t;

#ifdef AOB_MOBILE_ENABLED

#ifdef __cplusplus
extern "C" {
#endif
int app_bap_capa_cli_start(uint8_t con_idx);

app_bap_capa_record_cfg_t *app_bap_capa_cli_get_pac_record(uint8_t con_lid, uint8_t direction, const void *codec_id, uint8_t sampleRate);
int app_bap_capa_get_peer_audio_location_bf(uint8_t con_lid, uint8_t direction);

bool app_bap_capa_cli_is_peer_support_stereo_channel(uint8_t con_lid, uint8_t direction);
bool app_bap_capa_cli_is_codec_capa_support_stereo_channel(uint8_t con_lid, uint8_t direction, const void *codec_id, uint8_t sampleRate);

#ifdef __cplusplus
}
#endif
#endif

#endif
#endif // APP_BAP_CAPA_CLI_MSG_H_

/// @} APP_BAP
