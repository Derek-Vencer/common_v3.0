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

#ifndef __BTS_BT_TYPES_H__
#define __BTS_BT_TYPES_H__

#include "bt_types.h"
#include "bt_base_types.h"
#include "bts_common_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 ****************************************************************************************
 *    ____ _____   _____
 *   | __ )_   _| |_   _|   _ _ __   ___  ___
 *   |  _ \ | |     | || | | | '_ \ / _ \/ __|
 *   | |_) || |     | || |_| | |_) |  __/\__ \
 *   |____/ |_|     |_| \__, | .__/ \___||___/
 *                      |___/|_|
 *
 ****************************************************************************************
 */

// same as @BT_ACTIVE_MODE_KEEP_USER_T
typedef enum {
    BTS_KEEP_ACTIVE_MODE_USER_A2DP_STREAMING = 1,   // always in active mode during a2dp streaming
    BTS_KEEP_ACTIVE_MODE_USER_SCO_STREAMING,        // always in active mode during ai voice up-streaming
    BTS_KEEP_ACTIVE_MODE_USER_AI_VOICE_STREAMING,   // always in active mode during synchronous voice prompt playing
    BTS_KEEP_ACTIVE_MODE_USER_OTA,                  // always in active mode during OTA 
    BTS_KEEP_ACTIVE_MODE_USER_ROLE_SWITCH,          // stay in active mode for 15 seconds after role switch
    BTS_KEEP_ACTIVE_MODE_USER_SYNC_VOICE_PROMPT,    // always in active mode during phone call
    BTS_KEEP_ACTIVE_MODE_USER_SPP,                  // always in active mode for speed
} bts_keep_active_mode_user_t;

typedef struct {
    uint8_t data[240];
} bts_extended_inquiry_response_t;

/**
 ****************************************************************************************
 *       ___     ______   ____ ____    _____
 *      / \ \   / /  _ \ / ___|  _ \  |_   _|   _ _ __   ___  ___
 *     / _ \ \ / /| |_) | |   | |_) |   | || | | | '_ \ / _ \/ __|
 *    / ___ \ V / |  _ <| |___|  __/    | || |_| | |_) |  __/\__ \
 *   /_/   \_\_/  |_| \_\\____|_|       |_| \__, | .__/ \___||___/
 *                                          |___/|_|
 *
 ****************************************************************************************
 */

typedef enum {
    BTS_AVRCP_STS_BAD_CMD           = 0x00, // Invalid command
    BTS_AVRCP_STS_BAD_PARAM         = 0x01, // Invalid parameter
    BTS_AVRCP_STS_NOT_FOUND         = 0x02, // Specified parameter is wrong or not found
    BTS_AVRCP_STS_INTERNAL_ERR      = 0x03, // Internal Error
    BTS_AVRCP_STS_NO_ERROR          = 0x04, // Operation Success
    BTS_AVRCP_STS_UID_CHANGED       = 0x05, // UIDs changed
    BTS_AVRCP_STS_RESERVED          = 0x06, // Reserved
    BTS_AVRCP_STS_INV_DIRN          = 0x07, // Invalid direction
    BTS_AVRCP_STS_INV_DIRECTORY     = 0x08, // Invalid directory
    BTS_AVRCP_STS_INV_ITEM          = 0x09, // Invalid Item
    BTS_AVRCP_STS_INV_SCOPE         = 0x0a, // Invalid scope
    BTS_AVRCP_STS_INV_RANGE         = 0x0b, // Invalid range
    BTS_AVRCP_STS_DIRECTORY         = 0x0c, // UID is a directory
    BTS_AVRCP_STS_MEDIA_IN_USE      = 0x0d, // Media in use
    BTS_AVRCP_STS_PLAY_LIST_FULL    = 0x0e, // Playing list full
    BTS_AVRCP_STS_SRCH_NOT_SPRTD    = 0x0f, // Search not supported
    BTS_AVRCP_STS_SRCH_IN_PROG      = 0x10, // Search in progress
    BTS_AVRCP_STS_INV_PLAYER        = 0x11, // Invalid player
    BTS_AVRCP_STS_PLAY_NOT_BROW     = 0x12, // Player not browsable
    BTS_AVRCP_STS_PLAY_NOT_ADDR     = 0x13, // Player not addressed
    BTS_AVRCP_STS_INV_RESULTS       = 0x14, // Invalid results
    BTS_AVRCP_STS_NO_AVBL_PLAY      = 0x15, // No available players
    BTS_AVRCP_STS_ADDR_PLAY_CHGD    = 0x16, // Addressed player changed
} bts_avrcp_status_code_t;

typedef enum {
    BTS_AVRCP_MEDIA_ATTR_MASK_NONE          = 0x00,
    BTS_AVRCP_MEDIA_ATTR_MASK_TITLE         = 0x01,
    BTS_AVRCP_MEDIA_ATTR_MASK_ARTIST        = 0x02,
    BTS_AVRCP_MEDIA_ATTR_MASK_ALBUM         = 0x04,
    BTS_AVRCP_MEDIA_ATTR_MASK_TRACK         = 0x08,
    BTS_AVRCP_MEDIA_ATTR_MASK_NUM_TRACKS    = 0x10,
    BTS_AVRCP_MEDIA_ATTR_MASK_GENRE         = 0x20,
    BTS_AVRCP_MEDIA_ATTR_MASK_PLAYING_TIME  = 0x40,
    BTS_AVRCP_MEDIA_ATTR_MASK_COVER_HANDLE  = 0x80,
} bts_avrcp_media_attr_id_mask_t;

typedef enum {
    BTS_AVRCP_PLAYER_SETTING_ATTR_ILLEGAL   = 0x00,
    BTS_AVRCP_PLAYER_SETTING_ATTR_EQUALIZER = 0x01, // Supports equalizer settings
    BTS_AVRCP_PLAYER_SETTING_ATTR_REPEAT    = 0x02, // Supports repeat mode
    BTS_AVRCP_PLAYER_SETTING_ATTR_SHUFFLE   = 0x03, // Supports shuffle mode
    BTS_AVRCP_PLAYER_SETTING_ATTR_SCAN      = 0x04, // Supports track scan (preview) mode
} bts_avrcp_player_setting_attr_id_t;

typedef enum {
    BTS_AVRCP_SCOPE_MEDIA_PLAYER_LIST               = 0x00, // Contains all available media players
    BTS_AVRCP_SCOPE_MEDIA_PLAYER_VIRTUAL_FILESYSTEM = 0x01, // The virtual filesystem containing the media content of the browsed player
    BTS_AVRCP_SCOPE_SEARCH                          = 0x02, // The results of a search operation on the browsed player
    BTS_AVRCP_SCOPE_NOW_PLAYING                     = 0x03, // The Now Playing list (or queue) of the addressed player
} bts_avrcp_scope_t;

typedef struct
{
    uint16_t UID_Counter;
    uint32_t direction;
    uint32_t UID;
} bts_avrcp_change_path_params_t;

typedef struct
{
    uint8_t scope;
    uint32_t start_item;
    uint32_t end_item;
    uint32_t media_mask;    // see @bts_avrcp_media_attr_id_mask_t
} bts_avrcp_get_folder_items_params_t;

typedef struct
{
    uint8_t scope;
    uint16_t UID_Counter;
    uint32_t UID;
    uint32_t media_mask;
} bts_avrcp_get_item_attrs_params_t;

/**
 ****************************************************************************************
 *    _   _ _____ ____    _____
 *   | | | |  ___|  _ \  |_   _|   _ _ __   ___  ___
 *   | |_| | |_  | |_) |   | || | | | '_ \ / _ \/ __|
 *   |  _  |  _| |  __/    | || |_| | |_) |  __/\__ \
 *   |_| |_|_|   |_|       |_| \__, | .__/ \___||___/
 *                             |___/|_|
 *
 ****************************************************************************************
 */

typedef enum {
    BTS_HFP_HF_FEAT_ECNR        = 0x0001,
    BTS_HFP_HF_FEAT_3WAY        = 0x0002,
    BTS_HFP_HF_FEAT_CLI         = 0x0004,
    BTS_HFP_HF_FEAT_VR          = 0x0008,
    BTS_HFP_HF_FEAT_RVC         = 0x0010,
    BTS_HFP_HF_FEAT_ECS         = 0x0020,
    BTS_HFP_HF_FEAT_ECC         = 0x0040,
    BTS_HFP_HF_FEAT_CODEC       = 0x0080,
    BTS_HFP_HF_FEAT_HF_IND      = 0x0100,
    BTS_HFP_HF_FEAT_ESCO_S4_T2  = 0x0200,
} bts_hfp_hf_feat_mask_t;

typedef enum {
    BTS_HFP_AG_FEAT_3WAY        = 0x0001,
    BTS_HFP_AG_FEAT_ECNR        = 0x0002,
    BTS_HFP_AG_FEAT_VR          = 0x0004,
    BTS_HFP_AG_FEAT_INBAND      = 0x0008,
    BTS_HFP_AG_FEAT_VTAG        = 0x0010,
    BTS_HFP_AG_FEAT_REJ_CALL    = 0x0020,
    BTS_HFP_AG_FEAT_ECS         = 0x0040,
    BTS_HFP_AG_FEAT_ECC         = 0x0080,
    BTS_HFP_AG_FEAT_EXT_ERR     = 0x0100,
    BTS_HFP_AG_FEAT_CODEC       = 0x0200,
    BTS_HFP_AG_FEAT_HF_IND      = 0x0400,
    BTS_HFP_AG_FEAT_ESCO_S4_T2  = 0x0800,
} bts_hfp_ag_feat_mask_t;

typedef enum {
    BTS_HFP_CALL_NUMBER_TYPE_UNKNOWN = 0x81,
    BTS_HFP_CALL_NUMBER_TYPE_INTERNATIONAL = 0x91,
} bts_hfp_curr_call_number_type_t;

typedef enum {
    BTS_HFP_CALL_MODE_VOICE = 0,
    BTS_HFP_CALL_MODE_DATA,
    BTS_HFP_CALL_MODE_FAX,
} bts_hfp_curr_call_mode_t;

typedef enum
{
    BTS_HFP_CALL_ACTION_CHLD_0 = 0, // Action for CHLD 0
    BTS_HFP_CALL_ACTION_CHLD_1,     // Action for CHLD 1
    BTS_HFP_CALL_ACTION_CHLD_2,     // Action for CHLD 2
    BTS_HFP_CALL_ACTION_CHLD_3,     // Action for CHLD 3
    BTS_HFP_CALL_ACTION_CHLD_4,     // Action for CHLD 4
    BTS_HFP_CALL_ACTION_CHLD_1x,    // Action for CHLD 1x
    BTS_HFP_CALL_ACTION_CHLD_2x,    // Action for CHLD 2x
    BTS_HFP_CALL_ACTION_ATA,        // Action to answer call
    BTS_HFP_CALL_ACTION_CHUP,       // Action to hang up
    BTS_HFP_CALL_ACTION_REDIAL,
    BTS_HFP_CALL_ACTION_BTRH_0,     // Action for BTRH 0
    BTS_HFP_CALL_ACTION_BTRH_1,     // Action for BTRH 1
    BTS_HFP_CALL_ACTION_BTRH_2,     // Action for BTRH 2
} bts_hfp_call_action_t;

// HF Indicators HFP 1.7
typedef enum {
    BTS_HFP_HF_IND_ENHANCED_DRIVER_SAFETY   = 1,
    BTS_HFP_HF_IND_BATTERY_LEVEL_STATUS     = 2,
} bts_hfp_hf_ind_type_t;

/**
 ****************************************************************************************
 *    ____ ___ ____    _____
 *   |  _ \_ _|  _ \  |_   _|   _ _ __   ___  ___
 *   | | | | || |_) |   | || | | | '_ \ / _ \/ __|
 *   | |_| | ||  __/    | || |_| | |_) |  __/\__ \
 *   |____/___|_|       |_| \__, | .__/ \___||___/
 *                          |___/|_|
 *
 ****************************************************************************************
 */

typedef enum {
    BTS_DIP_DEVICE_NONE = 0,
    BTS_DIP_DEVICE_IOS,
    BTS_DIP_DEVICE_ANDROID,
} bts_dip_device_type_t;

typedef enum {
    BTS_DIP_QUERY_STATE_STARTED = 0,
    BTS_DIP_QUERY_STATE_SUCCEED,
    BTS_DIP_QUERY_STATE_FAILED,
} bts_dip_query_state_t;

/**
 ****************************************************************************************
 *    ____  ____  ____    _____
 *   / ___||  _ \|  _ \  |_   _|   _ _ __   ___  ___
 *   \___ \| |_) | |_) |   | || | | | '_ \ / _ \/ __|
 *    ___) |  __/|  __/    | || |_| | |_) |  __/\__ \
 *   |____/|_|   |_|       |_| \__, | .__/ \___||___/
 *                             |___/|_|
 *
 ****************************************************************************************
 */

#define BTS_SERIAL_PORT_UUID (0x1101)

typedef struct
{
    const uint8_t *uuid;            // service uuid (usually same as the uuid in profile_desc)
    uint8_t uuid_len;               // 16-byte(128-bit) or 2-byte(16-bit) uuid len
} bts_spp_service_class_params_t;

typedef struct
{
    uint8_t rfcomm_channel;         // RFCOMM protocol channel number, see @BTS_RFCOMM_CHANNEL
} bts_spp_protocol_desc_params_t;

typedef struct
{
    const uint8_t *uuid;            // serial port profile uuid 0x1101 => {0x01, 0x11}
    uint8_t uuid_len;               // 2-byte(16-bit) uuid len
    uint16_t version_number;        // service version, version 1.2(0x0102) or greater
} bts_spp_profile_desc_params_t;

typedef struct
{
    const char *name;               // service name
    uint8_t name_len;               // service name len
} bts_spp_service_name_params_t;

typedef struct
{
    uint16_t language;              // according to ISO 639:1988 (E/F)
    uint16_t character_encoding;    // IANA's database
    uint16_t base_attribute_id;     // offset of service_name
} bts_spp_language_base_params_t;

typedef struct
{
    // Necessary parameters
    bts_spp_service_class_params_t  *service_class;
    bts_spp_protocol_desc_params_t  *protocol_desc;
    // Necessary parameter if spp version 1.2(0x0102) or greater
    bts_spp_profile_desc_params_t   *profile_desc;
    // Optional parameters, if not need set to NULL
    bts_spp_service_name_params_t   *service_name;
    bts_spp_language_base_params_t  *language_base;
} bts_spp_sdp_params_t;

#ifdef __cplusplus
}
#endif

#endif /* __BTS_BT_TYPES_H__ */