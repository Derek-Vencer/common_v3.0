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

#ifndef __BTS_LEA_TYPES_H__
#define __BTS_LEA_TYPES_H__
#include "bt_lea_types.h"
#include "bts_common_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 ****************************************************************************************
 *    _     _____    _      _____
 *   | |   | ____|  / \    |_   _|   _ _ __   ___  ___
 *   | |   |  _|   / _ \     | || | | | '_ \ / _ \/ __|
 *   | |___| |___ / ___ \    | || |_| | |_) |  __/\__ \
 *   |_____|_____/_/   \_\   |_| \__, | .__/ \___||___/
 *                               |___/|_|
 *
 ****************************************************************************************
 */

#define BTS_LEA_CCP_INVALID_CALL_ID                 (0xFF)
#define BTS_LEA_INVALID_CONN_LID                    (0xFF)
#define BTS_LEA_INVALID_CONN_HDL                    (0xFFFF)

/*
 * TYPEDEFINES
*/
typedef enum
{
    /// LC3 Codec
    BTS_LEA_CODEC_TYPE_LC3    = 0x06,
    /// ULL
    BTS_LEA_CODEC_TYPE_ULL    = 0x08,

    /// Maximum SIG Codec
    BTS_LEA_CODEC_TYPE_SIG_MAX,
    /// Vendor Specific Codec
    BTS_LEA_CODEC_TYPE_VENDOR = 0xFF,
} bts_lea_codec_type_e;

typedef enum
{
    BTS_LEA_CODEC_FRAME_DURATION_7_5MS    = 0x00,
    BTS_LEA_CODEC_FRAME_DURATION_10MS     = 0x01,
    BTS_LEA_CODEC_FRAME_DURATION_5MS      = 0x02,
    BTS_LEA_CODEC_FRAME_DURATION_2_5MS    = 0x03,
    BTS_LEA_CODEC_FRAME_DURATION_MAX,
} bts_lea_codec_frame_duration_e;

/// Audio Location Bitfield
typedef enum
{
    BTS_LEA_LOC_FRONT_LEFT               = 0x00000001,
    BTS_LEA_LOC_FRONT_RIGHT              = 0x00000002,
    BTS_LEA_LOC_FRONT_CENTER             = 0x00000004,
    BTS_LEA_LOC_LOW_FREQ_EFFECTS_1       = 0x00000008,
    BTS_LEA_LOC_BACK_LEFT                = 0x00000010,
    BTS_LEA_LOC_BACK_RIGHT               = 0x00000020,
    BTS_LEA_LOC_FRONT_LEFT_OF_CENTER     = 0x00000040,
    BTS_LEA_LOC_FRONT_RIGHT_OF_CENTER    = 0x00000080,
    BTS_LEA_LOC_BACK_CENTER              = 0x00000100,
    BTS_LEA_LOC_LOW_FREQ_EFFECTS_2       = 0x00000200,
    BTS_LEA_LOC_SIDE_LEFT                = 0x00000400,
    BTS_LEA_LOC_SIDE_RIGHT               = 0x00000800,
    BTS_LEA_LOC_TOP_FRONT_LEFT           = 0x00001000,
    BTS_LEA_LOC_TOP_FRONT_RIGHT          = 0x00002000,
    BTS_LEA_LOC_TOP_FRONT_CENTER         = 0x00004000,
    BTS_LEA_LOC_TOP_CENTER               = 0x00008000,
    BTS_LEA_LOC_TOP_BACK_LEFT            = 0x00010000,
    BTS_LEA_LOC_TOP_BACK_RIGHT           = 0x00020000,
    BTS_LEA_LOC_TOP_SIDE_LEFT            = 0x00040000,
    BTS_LEA_LOC_TOP_SIDE_RIGHT           = 0x00080000,
    BTS_LEA_LOC_TOP_BACK_CENTER          = 0x00100000,
    BTS_LEA_LOC_BOTTOM_FRONT_CENTER      = 0x00200000,
    BTS_LEA_LOC_BOTTOM_FRONT_LEFT        = 0x00400000,
    BTS_LEA_LOC_BOTTOM_FRONT_RIGHT       = 0x00800000,
    BTS_LEA_LOC_FRONT_LEFT_WIDE          = 0x01000000,
    BTS_LEA_LOC_FRONT_RIGHT_WIDE         = 0x02000000,
    BTS_LEA_LOC_LEFT_SURROUND            = 0x04000000,
    BTS_LEA_LOC_RIGHT_SURROUND           = 0x08000000,

    BTS_LEA_LOC_RFU                      = 0xF0000000,
} bts_lea_location_bf_bit_e;

typedef enum
{
    BTS_LEA_CODEC_SAMPLE_FREQ_8000    = 0x01,
    BTS_LEA_CODEC_SAMPLE_FREQ_11025,
    BTS_LEA_CODEC_SAMPLE_FREQ_16000,
    BTS_LEA_CODEC_SAMPLE_FREQ_22050,
    BTS_LEA_CODEC_SAMPLE_FREQ_24000,
    BTS_LEA_CODEC_SAMPLE_FREQ_32000,
    BTS_LEA_CODEC_SAMPLE_FREQ_44100,
    BTS_LEA_CODEC_SAMPLE_FREQ_48000,
    BTS_LEA_CODEC_SAMPLE_FREQ_88200,
    BTS_LEA_CODEC_SAMPLE_FREQ_96000,
    BTS_LEA_CODEC_SAMPLE_FREQ_176400,
    BTS_LEA_CODEC_SAMPLE_FREQ_192000,
    BTS_LEA_CODEC_SAMPLE_FREQ_384000,
} bts_lea_codec_sample_freq_e;

typedef enum
{
    BTS_LEA_AUDIO_MUSIC_QOS_CONFIG_IND          = 0x00,
    BTS_LEA_AUDIO_MUSIC_ENABLE_REQ,
    BTS_LEA_AUDIO_MUSIC_RELEASE_REQ,
    BTS_LEA_AUDIO_MUSIC_STREAM_START_IND,
    BTS_LEA_AUDIO_MUSIC_STREAM_STOP_IND,

    BTS_LEA_AUDIO_CALL_QOS_CONFIG_IND,
    BTS_LEA_AUDIO_CALL_ENABLE_REQ,
    BTS_LEA_AUDIO_CALL_RELEASE_REQ,
    BTS_LEA_AUDIO_CALL_STREAM_START_IND,
    BTS_LEA_AUDIO_CALL_SINGLE_STREAM_STOP_IND,
    BTS_LEA_AUDIO_CALL_CAPTURE_STREAM_STOP_IND,
    BTS_LEA_AUDIO_CALL_PLAYBACK_STREAM_STOP_IND,
    BTS_LEA_AUDIO_CALL_ALL_STREAMS_STOP_IND,
    BTS_LEA_AUDIO_CALL_RINGING_IND,
    BTS_LEA_AUDIO_CALL_ACTIVE_IND,
    BTS_LEA_AUDIO_CALL_TERMINATE_IND,
    BTS_LEA_AUDIO_CALL_ALERTING_IND,        //16-0x10

    BTS_LEA_AUDIO_FLEXIBLE_ENABLE_REQ,
    BTS_LEA_AUDIO_FLEXIBLE_RELEASE_REQ,
    BTS_LEA_AUDIO_FLEXIBLE_STREAM_START_IND,
    BTS_LEA_AUDIO_FLEXIBLE_CAPTURE_STREAM_STOP_IND,
    BTS_LEA_AUDIO_FLEXIBLE_PLAYBACK_STREAM_STOP_IND,
    BTS_LEA_AUDIO_FLEXIBLE_ALL_STREAMS_STOP_IND,  //22-0x16

    BTS_LEA_AUDIO_BIS_STREAM_START_IND,
    BTS_LEA_AUDIO_BIS_STREAM_STOP_IND,

    BTS_LEA_AUDIO_CIS_CONNECTED_IND,
    BTS_LEA_AUDIO_PROMPT_SOUND_ENABLE_REQ,
    BTS_LEA_AUDIO_PROMPT_SOUND_START,      //30
    BTS_LEA_AUDIO_PROMPT_SOUND_STOP,
    BTS_LEA_AUDIO_LE_LINK_CONNECTED_IND,
    BTS_LEA_AUDIO_LE_LINK_DISCONCETED_IND,     //25-0x19

    BTS_LEA_AUDIO_UPDATE_CONTEXT_TYPE_TO_CALL,
    BTS_LEA_AUDIO_UPDATE_CONTEXT_TYPE_TO_MUSIC,
    BTS_LEA_AUDIO_UPDATE_CONTEXT_TYPE_TO_FLEXIBLE,
    BTS_LEA_AUDIO_EVENT_ROUTE_CALL_TO_BT,
    BTS_LEA_AUDIO_TOGGLE_A2DP_CIS_REQ,
    BTS_LEA_AUDIO_MEDIA_PLAYSTATUS_CHANGED,
    BTS_LEA_AUDIO_CALL_STATUS_CHANGED, //32-0x1F
    BTS_LEA_AUDIO_MAX_IND,
} bts_lea_audio_event_t;

typedef enum
{
    /// Unspecified - Position
    BTS_LEA_CONTEXT_TYPE_UNSPECIFIED = 0,
    BTS_LEA_CONTEXT_TYPE_UNSPECIFIED_BIT = (1 << BTS_LEA_CONTEXT_TYPE_UNSPECIFIED),
    /// Conversation between humans as, for example, in telephony or video calls
    BTS_LEA_CONTEXT_TYPE_CONVERSATIONAL = 1,
    BTS_LEA_CONTEXT_TYPE_CONVERSATIONAL_BIT = (1 << BTS_LEA_CONTEXT_TYPE_CONVERSATIONAL),
    /// Media as, for example, in music, public radio, podcast or video soundtrack.
    BTS_LEA_CONTEXT_TYPE_MEDIA = 2,
    BTS_LEA_CONTEXT_TYPE_MEDIA_BIT = (1 << BTS_LEA_CONTEXT_TYPE_MEDIA),
    /// Audio associated with video gaming, for example gaming media, gaming effects, music and in-game voice chat
    BTS_LEA_CONTEXT_TYPE_GAME = 3,
    BTS_LEA_CONTEXT_TYPE_GAME_BIT = (1 << BTS_LEA_CONTEXT_TYPE_GAME),
    /// Instructional audio as, for example, in navigation, traffic announcements or user guidance
    BTS_LEA_CONTEXT_TYPE_INSTRUCTIONAL = 4,
    BTS_LEA_CONTEXT_TYPE_INSTRUCTIONAL_BIT = (1 << BTS_LEA_CONTEXT_TYPE_INSTRUCTIONAL),
    /// Man machine communication as, for example, with voice recognition or virtual assistant
    BTS_LEA_CONTEXT_TYPE_MAN_MACHINE = 5,
    BTS_LEA_CONTEXT_TYPE_MAN_MACHINE_BIT = (1 << BTS_LEA_CONTEXT_TYPE_MAN_MACHINE),
    /// Live audio as from a microphone where audio is perceived both through a direct acoustic path and through
    /// an LE Audio Stream
    BTS_LEA_CONTEXT_TYPE_LIVE = 6,
    BTS_LEA_CONTEXT_TYPE_LIVE_BIT = (1 << BTS_LEA_CONTEXT_TYPE_LIVE),
    /// Sound effects including keyboard and touch feedback;
    /// menu and user interface sounds; and other system sounds
    BTS_LEA_CONTEXT_TYPE_SOUND_EFFECTS = 7,
    BTS_LEA_CONTEXT_TYPE_SOUND_EFFECTS_BIT = (1 << BTS_LEA_CONTEXT_TYPE_SOUND_EFFECTS),
    /// Attention seeking audio as, for example, in beeps signalling arrival of a message or keyboard clicks
    BTS_LEA_CONTEXT_TYPE_ATTENTION_SEEKING = 8,
    BTS_LEA_CONTEXT_TYPE_ATTENTION_SEEKING_BIT = (1 << BTS_LEA_CONTEXT_TYPE_ATTENTION_SEEKING),
    /// Ringtone as in a call alert
    BTS_LEA_CONTEXT_TYPE_RINGTONE = 9,
    BTS_LEA_CONTEXT_TYPE_RINGTONE_BIT = (1 << BTS_LEA_CONTEXT_TYPE_RINGTONE),
    /// Immediate alerts as, for example, in a low battery alarm, timer expiry or alarm clock.
    BTS_LEA_CONTEXT_TYPE_IMMEDIATE_ALERT = 10,
    BTS_LEA_CONTEXT_TYPE_IMMEDIATE_ALERT_BIT = (1 << BTS_LEA_CONTEXT_TYPE_IMMEDIATE_ALERT),
    /// Emergency alerts as, for example, with fire alarms or other urgent alerts
    BTS_LEA_CONTEXT_TYPE_EMERGENCY_ALERT = 11,
    BTS_LEA_CONTEXT_TYPE_EMERGENCY_ALERT_BIT = (1 << BTS_LEA_CONTEXT_TYPE_EMERGENCY_ALERT),
    /// TV - Position
    /// Audio associated with a television program and/or with metadata conforming to the Bluetooth Broadcast TV
    /// profile
    BTS_LEA_CONTEXT_TYPE_TV = 12,
    BTS_LEA_CONTEXT_TYPE_TV_BIT = (1 << BTS_LEA_CONTEXT_TYPE_TV),
} bts_lea_context_type_e;

typedef enum
{
    BTS_LEA_CCP_CALL_STATE_INCOMING                     = 0,
    BTS_LEA_CCP_CALL_STATE_DIALING                      = 1,
    BTS_LEA_CCP_CALL_STATE_ALERTING                     = 2,
    BTS_LEA_CCP_CALL_STATE_ACTIVE                       = 3,
    BTS_LEA_CCP_CALL_STATE_LOCAL_HELD                   = 4,
    BTS_LEA_CCP_CALL_STATE_REMOTE_HELD                  = 5,
    BTS_LEA_CCP_CALL_STATE_LOCAL_AND_REMOTE_HELD        = 6,
    BTS_LEA_CCP_CALL_STATE_IDLE                         = 7,
} bts_lea_ccp_call_state_e;

/// Codec Capabilities parameters structure
typedef struct
{
    /// Supported Sampling Frequencies bit field (see #bap_sampling_freq_bf enumeration)\n
    /// 0 means that the field is not part of the Codec Specific Capabilities\n
    /// Mandatory for LC3
    uint16_t sampling_freq_bf;
    /// Supported Frame Durations bit field (see #bap_freq_dur_bf enumeration)\n
    /// 0 means that the field is not part of the Codec Specific Capabilities\n
    /// Mandatory for LC3
    uint8_t frame_dur_bf;
    /// Supported Audio Channel Counts\n
    /// 0 means that the field is not part of the Codec Specific Capabilities\n
    /// For LC3, absence in the Codec Specific Capabilities is equivalent to 1 channel supported (forced to 0x01
    /// on reception side)
    uint8_t chan_cnt_bf;
    /// Supported Octets Per Codec Frame - Minimum\n
    /// Not part of the Codec Specific Capabilities is equal to 0 and frame_octet_max also equal to 0\n
    /// Mandatory for LC3
    uint16_t frame_octet_min;
    /// Supported Octets Per Codec Frame - Maximum\n
    /// Not part of the Codec Specific Capabilities is equal to 0 and frame_octet_min also equal to 0\n
    /// Mandatory for LC3
    uint16_t frame_octet_max;
    /// Supported Maximum Codec Frames Per SDU\n
    /// 0 means that the field is not part of the Codec Specific Capabilities\n
    /// For LC3, absence in the Codec Specific Capabilities is equivalent to 1 Frame Per SDU (forced to 1 on
    /// reception side)
    uint8_t max_frames_sdu;
} bts_lea_codec_capa_param_t;

/// Codec Capabilities structure
typedef struct
{
    /// Parameters structure
    bts_lea_codec_capa_param_t param;
    /// Additional Codec Capabilities (in LTV format)
    bt_lea_ltv_data_t add_capa;
} bts_lea_codec_capa_t;

typedef struct
{
    uint32_t ch_bf;
    uint8_t *bc_id;
    uint8_t *bc_code;
    struct
    {
        void (*bis_sink_scan_state_cb)(bool scan_or_pa_sync, bool started, uint32_t param);
        bool (*bis_sink_select_source)(ble_bdaddr_t *addr, uint8_t adv_sid, uint8_t *bcast_id,
                                       uint8_t *adv_data, uint8_t adv_data_len, int8_t rssi);
        void (*bis_sink_started_callback)(uint8_t grp_lid);
        void (*bis_sink_stoped_callback)(uint8_t grp_lid, uint16_t err_code);
        void (*bis_sink_metadata_cb)(uint8_t subgrp_lid, uint8_t *buf, uint8_t buf_len);
    } event_callback;
} bts_lea_bis_sink_start_param_t;

typedef struct
{
    /// Time_Stamp
    uint32_t        time_stamp;
    /// Packet Sequence Number
    uint16_t        pkt_seq_nb;
    /// length of the ISO SDU (in bytes)
    uint16_t        sdu_length;
    /// Reception status (@see enum hci_iso_pkt_stat_flag)
    uint8_t         status;
    /// SDU
    uint8_t         *sdu;
} bts_lea_iso_dp_pkt_buf_t;

typedef struct
{
    bts_lea_codec_sample_freq_e sample_rate;
    uint16_t frame_octet;
    bt_lea_direction_t direction;
    const bt_lea_codec_id_t *codec_id;
    bts_lea_context_type_e context_type_bit;
} bts_lea_ase_stream_param_t;

typedef struct
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
} bts_lea_ascc_qos_cfg_t;

typedef struct
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
    bt_lea_direction_t direction;
    /// Codec ID
    bt_lea_codec_id_t codec_id;
    /// Pointer to Codec Configuration
    bt_lea_codec_cfg_t *p_cfg;
    /// QoS configuration
    bts_lea_ascc_qos_cfg_t qos_cfg;
    /// Pointer to Metadata structure
    bt_lea_metadata_t *p_metadata;
    /// CIS index
    uint8_t cis_id;
    /// CIS Connection Handle
    uint16_t cis_hdl;
    /// current audio stream type
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
} bts_lea_ascc_ase_t;

typedef struct
{
    // Central to Peripheral Burst Number
    int c2p_bn;
    // Central to Peripheral Number of Subevent
    int c2p_nse;
    // Central to Peripheral Flush Timeout
    int c2p_ft;
    // Peripheral to Central Burst Number
    int p2c_bn;
    // Peripheral to Central Number of Subevent
    int p2c_nse;
    // Peripheral to Central Flush Timeout
    int p2c_ft;
    // ISO interval 1.25ms * N
    int iso_interval_1_25ms;
} bts_lea_cig_param_t;

typedef struct
{
    bool mute_new_music_stream;
    bool pause_new_music_stream;
} bts_lea_audio_policy_cfg_t;

typedef struct
{
    uint8_t outgoing_call_flag:        1;
    uint8_t withheld_server_flag:      1;
    uint8_t withheld_network_flag:     1;
    uint8_t reserved:                  5;
} bts_lea_ccp_call_flags_t;

typedef struct
{
    /// Bearer local index, reserve value is 0xFF
    uint8_t                         bearer_lid;
    uint8_t                         signal_strength;
    /// Call flags
    bts_lea_ccp_call_flags_t        call_flags;
    /// Call index, reserve value is 0x00
    uint8_t                         call_id;
    /// Call state
    bts_lea_ccp_call_state_e        state;
    /// Length of Incoming or Outgoing Call URI value
    uint8_t                         uri_len;
    /// Remote Call URI value
    uint8_t                         *uri;
} bts_lea_ccp_single_call_info_t;

#ifdef __cplusplus
}
#endif

#endif /* __BTS_LEA_TYPES_H__ */