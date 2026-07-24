/***************************************************************************
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
#include "bt_base_types.h"

typedef enum
{
    BT_LEA_EVT_TW_CONNECTION_STATE                 = 0,
    BT_LEA_EVT_MOB_CONNECTION_STATE                = 1,
    BT_LEA_EVT_ADV_STATE                           = 2,
    BT_LEA_EVT_VOL_CHANGED                         = 3,
    BT_LEA_EVT_VOCS_OFFSET_CHANGED                 = 4,
    BT_LEA_EVT_VOCS_BOND_DATA_CHANGED              = 5,
    BT_LEA_EVT_MEDIA_TRACK_CHANGED                 = 6,
    BT_LEA_EVT_STREAM_STATUS_CHANGED               = 7,
    BT_LEA_EVT_MCP_MCC_CHAR_VALUE                  = 8,
    BT_LEA_EVT_MIC_STATE                           = 9,
    BT_LEA_EVT_CALL_STATE_CHANGE                   = 10,
    BT_LEA_EVT_CALL_SRV_SIG_STRENGTH_VALUE_IND     = 11,
    BT_LEA_EVT_CALL_STATUS_FLAGS_IND               = 12,
    BT_LEA_EVT_CALL_CCP_OPT_SUPPORTED_OPCODE_IND   = 13,
    BT_LEA_EVT_CALL_TERMINATE_REASON_IND           = 14,
    BT_LEA_EVT_CALL_INCOMING_NUM_INF_IND           = 15,
    BT_LEA_EVT_CALL_SVC_CHANGED_IND                = 16,
    BT_LEA_EVT_CALL_ACTION_RESULT_IND              = 17,
    BT_LEA_EVT_ISO_LINK_QUALITY_IND                = 18,
    BT_LEA_EVT_PACS_CCCD_WRITTEN_IND               = 19,
    BT_LEA_EVT_SIRK_REFRESHED                      = 20,
    BT_LEA_EVT_CIS_ESTABLISHED_IND                 = 21,
    BT_LEA_EVT_CIS_DISCONNECTED_IND                = 22,
    BT_LEA_EVT_CIS_REJECTED_IND                    = 23,
    BT_LEA_EVT_CIG_TERMINATED_IND                  = 24,
    BT_LEA_EVT_ASE_NTF_VALUE_IND                   = 25,
    BT_LEA_EVT_ASE_CODEC_CFG_VALUE_IND             = 26,
    BT_LEA_EVT_ASE_METADATA_UPDATE_IND             = 27,
    BT_LEA_EVT_BIS_SINK_STATUS_IND                 = 28,
    BT_LEA_EVT_BIS_SINK_ENABLE_IND                 = 29,
    BT_LEA_EVT_BIS_SINK_DISABLE_IND                = 30,
    BT_LEA_EVT_BIS_SINK_STREAM_START_IND           = 31,
    BT_LEA_EVT_BIS_SINK_STREAM_STOP_IND            = 32,
    BT_LEA_EVT_BIS_DELEGE_SOURCE_ADD_IND           = 33,
    BT_LEA_EVT_BIS_DELEGE_SOURCE_RM_IND            = 34,
    BT_LEA_EVT_BIS_DELEGE_SOURCE_UPD_IND           = 35,
    BT_LEA_EVT_MCP_MCC_SET_CFG_CMP_IND             = 36,
    BT_LEA_EVT_VCP_VCS_CCCD_WRITTEN_IND            = 37,
    BT_LEA_EVT_TMAP_READ_ROLE_CMP_IND              = 38,
    BT_LEA_EVT_ASE_ENABLE_REQ_IND                  = 39,
    BT_LEA_EVT_MCP_MCC_SVC_CHANGED_IND             = 40,
    BT_LEA_EVT_CCP_TBC_SET_CFG_CMP_IND             = 41,

    BT_LEA_EVT_LAST                                = 0xFF,
} bt_lea_evt_t;

typedef enum
{
    BT_LEA_ADV_START         = 0,
    BT_LEA_ADV_FAILED        = 1,
    BT_LEA_ADV_STOP          = 2,
} bt_lea_adv_state_t;

typedef enum
{
    BT_LEA_ADV_IDLE         = 0,
    BT_LEA_ADV_PAIRING      = 1,
    BT_LEA_ADV_RECONNECT    = 2,
    BT_LEA_ADV_UNKNOWN      = 3,
} bt_lea_adv_type_t;

typedef enum
{
    BT_LEA_ACL_DISCONNECTED       = 0,
    BT_LEA_ACL_CONNECTING         = 1,
    BT_LEA_ACL_FAILED             = 3,
    BT_LEA_ACL_CONNECTED          = 4,
    BT_LEA_ACL_BOND_SUCCESS       = 5,
    BT_LEA_ACL_BOND_FAILURE       = 6,
    BT_LEA_ACL_ENCRYPT            = 7,
    BT_LEA_ACL_ATTR_BOND          = 8,
    BT_LEA_ACL_DISCONNECTING      = 9,
} bt_lea_acl_state_t;

typedef enum
{
    BT_LEA_ASCS_ASE_STATE_IDLE             = 0,
    BT_LEA_ASCS_ASE_STATE_CODEC_CONFIGURED = 1,
    BT_LEA_ASCS_ASE_STATE_QOS_CONFIGURED   = 2,
    BT_LEA_ASCS_ASE_STATE_ENABLING         = 3,
    BT_LEA_ASCS_ASE_STATE_STREAMING        = 4,
    BT_LEA_ASCS_ASE_STATE_DISABLING        = 5,
    BT_LEA_ASCS_ASE_STATE_RELEASING        = 6,

    BT_LEA_ASCS_ASE_STATE_MAX,
} bt_lea_ascs_ase_state_t;

typedef enum
{
    /// Sink direction
    BT_LEA_DIRECTION_SINK = 0x00,
    /// Source direction
    BT_LEA_DIRECTION_SRC,

    BT_LEA_DIRECTION_MAX,
} bt_lea_direction_t;

typedef enum
{
    /// Inactive
    BT_LEA_MCP_PLAYBACK_STATE_INACTIVE = 0,
    /// Playing
    BT_LEA_MCP_PLAYBACK_STATE_PLAYING,
    /// Paused
    BT_LEA_MCP_PLAYBACK_STATE_PAUSED,
    /// Seeking
    BT_LEA_MCP_PLAYBACK_STATE_SEEKING,

    BT_LEA_MCP_PLAYBACK_STATE_MAX,
} bt_lea_mcp_playback_state_t;

typedef enum
{
    /// Unspecified - Position
    BT_LEA_CONTEXT_TYPE_UNSPECIFIED = 0,
    BT_LEA_CONTEXT_TYPE_UNSPECIFIED_BIT = (1 << BT_LEA_CONTEXT_TYPE_UNSPECIFIED),
    // Conversation between humans as, for example, in telephony or video calls
    BT_LEA_CONTEXT_TYPE_CONVERSATIONAL = 1,
    BT_LEA_CONTEXT_TYPE_CONVERSATIONAL_BIT = (1 << BT_LEA_CONTEXT_TYPE_CONVERSATIONAL),
    // Media as, for example, in music, public radio, podcast or video soundtrack.
    BT_LEA_CONTEXT_TYPE_MEDIA = 2,
    BT_LEA_CONTEXT_TYPE_MEDIA_BIT = (1 << BT_LEA_CONTEXT_TYPE_MEDIA),
    // Audio associated with video gaming, for example gaming media, gaming effects, music and in-game voice chat
    BT_LEA_CONTEXT_TYPE_GAME = 3,
    BT_LEA_CONTEXT_TYPE_GAME_BIT = (1 << BT_LEA_CONTEXT_TYPE_GAME),
    // Instructional audio as, for example, in navigation, traffic announcements or user guidance
    BT_LEA_CONTEXT_TYPE_INSTRUCTIONAL = 4,
    BT_LEA_CONTEXT_TYPE_INSTRUCTIONAL_BIT = (1 << BT_LEA_CONTEXT_TYPE_INSTRUCTIONAL),
    // Man machine communication as, for example, with voice recognition or virtual assistant
    BT_LEA_CONTEXT_TYPE_MAN_MACHINE = 5,
    BT_LEA_CONTEXT_TYPE_MAN_MACHINE_BIT = (1 << BT_LEA_CONTEXT_TYPE_MAN_MACHINE),
    // Live audio as from a microphone where audio is perceived both through a direct acoustic path and through
    // an LE Audio Stream
    BT_LEA_CONTEXT_TYPE_LIVE = 6,
    BT_LEA_CONTEXT_TYPE_LIVE_BIT = (1 << BT_LEA_CONTEXT_TYPE_LIVE),
    // Sound effects including keyboard and touch feedback;
    // menu and user interface sounds; and other system sounds
    BT_LEA_CONTEXT_TYPE_SOUND_EFFECTS = 7,
    BT_LEA_CONTEXT_TYPE_SOUND_EFFECTS_BIT = (1 << BT_LEA_CONTEXT_TYPE_SOUND_EFFECTS),
    // Attention seeking audio as, for example, in beeps signalling arrival of a message or keyboard clicks
    BT_LEA_CONTEXT_TYPE_ATTENTION_SEEKING = 8,
    BT_LEA_CONTEXT_TYPE_ATTENTION_SEEKING_BIT = (1 << BT_LEA_CONTEXT_TYPE_ATTENTION_SEEKING),
    // Ringtone as in a call alert
    BT_LEA_CONTEXT_TYPE_RINGTONE = 9,
    BT_LEA_CONTEXT_TYPE_RINGTONE_BIT = (1 << BT_LEA_CONTEXT_TYPE_RINGTONE),
    // Immediate alerts as, for example, in a low battery alarm, timer expiry or alarm clock.
    BT_LEA_CONTEXT_TYPE_IMMEDIATE_ALERT = 10,
    BT_LEA_CONTEXT_TYPE_IMMEDIATE_ALERT_BIT = (1 << BT_LEA_CONTEXT_TYPE_IMMEDIATE_ALERT),
    // Emergency alerts as, for example, with fire alarms or other urgent alerts
    BT_LEA_CONTEXT_TYPE_EMERGENCY_ALERT = 11,
    BT_LEA_CONTEXT_TYPE_EMERGENCY_ALERT_BIT = (1 << BT_LEA_CONTEXT_TYPE_EMERGENCY_ALERT),
    // TV - Position
    // Audio associated with a television program and/or with metadata conforming to the Bluetooth Broadcast TV
    // profile
    BT_LEA_CONTEXT_TYPE_TV = 12,
    BT_LEA_CONTEXT_TYPE_TV_BIT = (1 << BT_LEA_CONTEXT_TYPE_TV),
} bt_lea_context_type_t;
/// Unicast group configuration structure (provided by controller after stream establisment)
typedef struct
{
    /// Group synchronization delay time in microseconds
    uint32_t sync_delay_us;
    /// The maximum time, in microseconds, for transmission of SDUs of all CISes from master to slave
    /// (range 0x0000EA to 0x7FFFFF)
    uint32_t tlatency_m2s_us;
    /// The maximum time, in microseconds, for transmission of SDUs of all CISes from slave to master
    /// (range 0x0000EA to 0x7FFFFF)
    uint32_t tlatency_s2m_us;
    /// ISO interval (1.25ms unit, range: 5ms to 4s)
    uint16_t iso_intv_frames;
} bt_lea_cig_cfg_t;

/// Unicast stream configuration structure (provided by controller after stream establishment)
typedef struct
{
    /// Stream synchronization delay time in microseconds
    uint32_t sync_delay_us;
    /// Maximum size, in octets, of the payload from master to slave (Range: 0x00-0xFB)
    uint16_t max_pdu_m2s;
    /// Maximum size, in octets, of the payload from slave to master (Range: 0x00-0xFB)
    uint16_t max_pdu_s2m;
    /// Master to slave PHY, bit 0: 1Mbps, bit 1: 2Mbps, bit 2: LE-Coded
    uint8_t phy_m2s;
    /// Slave to master PHY, bit 0: 1Mbps, bit 1: 2Mbps, bit 2: LE-Coded
    uint8_t phy_s2m;
    /// The burst number for master to slave transmission (0x00: no isochronous data from the master to the slave, range 0x01-0x0F)
    uint8_t bn_m2s;
    /// The burst number for slave to master transmission (0x00: no isochronous data from the slave to the master, range 0x01-0x0F)
    uint8_t bn_s2m;
    /// The flush timeout, in multiples of the ISO_Interval, for each payload sent from the master to the slave (Range: 0x01-0x1F)
    uint8_t ft_m2s;
    /// The flush timeout, in multiples of the ISO_Interval, for each payload sent from the slave to the master (Range: 0x01-0x1F)
    uint8_t ft_s2m;
    /// Maximum number of subevents in each isochronous interval. From 0x1 to 0x1F
    uint8_t nse;
} bt_lea_cis_cfg_t;

typedef struct
{
    /// Cmd return status
    uint16_t status;
    /// Ase_lid
    uint8_t ase_lid;
    /// Number of packets transmitted and unacked
    uint32_t tx_unacked_packets;
    /// Number of flushed transmitted packets
    uint32_t tx_flushed_packets;
    /// Number of packets transmitted during last subevent
    uint32_t tx_last_subevent_packets;
    /// Number of retransmitted packets
    uint32_t retx_packets;
    /// Number of packets received with a CRC error
    uint32_t crc_error_packets;
    /// Number of unreceived packets
    uint32_t rx_unrx_packets;
    /// Number of duplicate packets received
    uint32_t duplicate_packets;
} __attribute__((__packed__)) bt_lea_iso_link_quality_info_t;

/// Structure for BAP_UC_SRV_CIS_STATE indication message
typedef struct
{
    /// Indication code (@see enum bap_uc_srv_ind_code)
    uint16_t ind_code;
    /// Stream local index
    uint8_t stream_lid;
    /// Connection local index of LE connection the CIS is bound with
    uint8_t con_lid;
    /// ASE local index for Sink direction
    uint8_t ase_lid_sink;
    /// ASE local index for Source direction
    uint8_t ase_lid_src;
    /// CIG ID
    uint8_t cig_id;
    /// CIS ID
    uint8_t cis_id;
    /// Connection handle allocated by the controller
    /// GAP_INVALID_CONHDL means that the CIS is not established
    uint16_t conhdl;
    /// Group configuration\n
    /// Meaningful only if conhdl is not GAP_INVALID_CONHDL
    bt_lea_cig_cfg_t cig_config;
    /// Stream configuration\n
    /// Meaningful only if conhdl is not GAP_INVALID_CONHDL
    bt_lea_cis_cfg_t cis_config;
} bt_lea_cis_info_t;

/// Codec Identifier
typedef struct
{
    /// Codec ID value
    uint8_t codec_id[5];
} bt_lea_codec_id_t;

/// Codec Configuration parameters structure
typedef struct
{
    /// Audio Locations of the Audio Channels being configured for the codec (i.e the number of codec frames per
    /// block) and their ordering within a single block of codec frames
    /// When transmitted, part of Codec Specific Configuration only if not equal to 0
    /// When received, 0 shall be interpreted as a single channel with no specified Audio Location
    uint32_t location_bf;
    /// Length of a codec frame in octets
    uint16_t frame_octet;
    /// Sampling Frequency (see #bap_sampling_freq enumeration)
    uint8_t sampling_freq;
    /// Frame Duration (see #bap_frame_dur enumeration)
    uint8_t frame_dur;
    /// Number of blocks of codec frames that shall be sent or received in a single SDU
    uint8_t frames_sdu;
} bt_lea_bap_cfg_param_t;

/// Data value in LTV format
typedef struct
{
    /// Length of data value
    uint8_t len;
    /// Data value
    uint8_t data[0];
} bt_lea_ltv_data_t;

/// Codec Configuration structure
typedef struct
{
    /// Parameters structure
    bt_lea_bap_cfg_param_t param;
    /// Additional Codec Configuration (in LTV format)
    bt_lea_ltv_data_t add_cfg;
} bt_lea_codec_cfg_t;

/// QoS Requirement structure
typedef struct
{
    /// Presentation Delay minimum microseconds
    uint32_t pres_delay_min_us;
    /// Presentation Delay maximum in microseconds
    uint32_t pres_delay_max_us;
    /// Minimum preferred presentation delay in microseconds
    /// 0 means no preference
    /// If not equal to 0, shall be >= pres_delay_min_us
    uint32_t pref_pres_delay_min_us;
    /// Maximum preferred presentation delay in microseconds
    /// 0 means no preference
    /// If not equal to 0, shall be <= pres_delay_max_us
    uint32_t pref_pres_delay_max_us;
    /// Maximum Transport latency in milliseconds
    /// From 5ms (0x5) to 4000ms (0xFA0)
    uint16_t trans_latency_max_ms;
    /// PDU framing arrangement
    uint8_t framing;
    /// Preferred PHY bit field
    uint8_t phy_bf;
    /// Preferred maximum number of retransmissions for each CIS Data PDU
    /// From 0 to 15
    uint8_t retx_nb;
} bt_lea_qos_req_t;

/// QoS Configuration structure
typedef struct
{
    /// PDU framing arrangement
    uint8_t framing;
    /// PHY Bitfield
    uint8_t phy_bf;
    /// Maximum number of retransmissions for each CIS Data PDU
    /// From 0 to 15
    uint8_t retx_nb;
    /// Maximum SDU size
    /// From 0 to 4095 bytes (0xFFF)
    uint16_t max_sdu_size;
    /// Maximum Transport latency in milliseconds
    /// From 5ms (0x5) to 4000ms (0xFA0)
    uint16_t trans_latency_max_ms;
    /// Presentation Delay in microseconds
    uint32_t pres_delay_us;
    /// SDU interval in microseconds
    /// From 255us (0xFF) to 16777215us (0xFFFFFF)
    uint32_t sdu_intv_us;
} bt_lea_qos_cfg_t;

/// Codec Configuration Metadata parameters structure
typedef struct
{
    /// Streaming Audio Contexts bit field
    uint16_t context_bf;
} bt_lea_meta_param_t;

/// Codec Configuration Metadata structure
typedef struct
{
    /// Parameters structure
    bt_lea_meta_param_t param;
    /// Additional Metadata value (in LTV format)
    bt_lea_ltv_data_t add_metadata;
} bt_lea_metadata_t;

typedef struct
{
    /// ASE local index
    uint8_t ase_lid;
    /// Connection local index
    uint8_t con_lid;
    /// ASE Instance local index
    uint8_t ase_id;
    /// ASE Direction
    bt_lea_direction_t direction;
    /// ASE State
    bt_lea_ascs_ase_state_t ase_state;
    /// Codec ID
    bt_lea_codec_id_t codec_id;
    /// Pointer to Codec Configuration structure
    const bt_lea_codec_cfg_t *p_cfg;
    /// QoS Requirements
    bt_lea_qos_req_t qos_req;
    /// CIG ID
    uint8_t cig_id;
    /// CIS ID
    uint8_t cis_id;
    /// CIS Connection Handle
    uint16_t cis_hdl;
    /// QoS Configuration structure
    bt_lea_qos_cfg_t qos_cfg;
    /// Pointer to Metadata structure
    const bt_lea_metadata_t *p_metadata;
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
    /// context type bring by enable req
    uint16_t init_context_bf;
} bt_lea_ascs_ase_t;

typedef struct
{
    /// Bearer local index, reserve value is 0xFF
    uint8_t bearer_lid;
    /// Signal strength in dbm
    uint8_t signal_strength;
    /// Call flags
    uint8_t outgoing_call_flag:1;
    uint8_t withheld_server_flag:1;
    uint8_t withheld_network_flag:1;
    /// Call index, reserve value is 0x00
    uint8_t call_id;
    /// Call state
    uint8_t state;
    /// Length of Incoming or Outgoing Call URI value
    uint8_t uri_len;
    /// Remote Call URI value
    const uint8_t *p_uri;
} bt_lea_call_info_t;

typedef struct
{
    /// Connection local index
    uint8_t con_lid;
    /// Bearer local index, reserve value is 0xFF
    uint8_t bearer_lid;
    /// Call index, reserve value is 0x00
    uint8_t call_id;
    /// Call action opcode
    uint8_t action_opcode;
    /// Call action result
    uint8_t result;
} bt_lea_call_action_ret_t;

typedef struct
{
    bt_lea_acl_state_t                  acl_state;
    uint8_t                             err_code;
} bt_lea_conn_state_t;

typedef struct
{
    bt_lea_evt_t                        type;
    uint8_t                             length;
} bt_lea_evt_header_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    bt_lea_adv_state_t                  adv_state;
    bt_lea_adv_type_t                   adv_type;
    uint8_t                             err_code;
} bt_lea_evt_adv_state_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    bt_lea_conn_state_t                 state;
    uint32_t                            evt_type;
    uint8_t                             conidx;
    ble_bdaddr_t                        peer_bdaddr;
} bt_lea_evt_mob_state_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             con_lid;
    uint8_t                             volume;
    uint8_t                             mute;
    uint8_t                             change_counter;
    bool                                is_local; // volume change triggered by the local or the remote
} bt_lea_evt_vol_changed_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    int16_t                             offset;
    uint8_t                             output_lid;
} bt_lea_evt_vocs_offset_changed_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             output_lid;
    uint8_t                             cli_cfg_bf;
} bt_lea_evt_vocs_bond_data_changed_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             con_lid;
    uint8_t                             media_lid;
} bt_lea_evt_media_track_changed_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             con_lid;
    uint8_t                             ase_lid;
    bt_lea_ascs_ase_state_t             prev_state;
    bt_lea_ascs_ase_state_t             curr_state;
} bt_lea_evt_stream_status_changed_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             con_lid;
    uint8_t                             media_lid;
} bt_lea_evt_mcp_mcc_svc_changed_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             con_lid;
    uint8_t                             media_lid;
    uint8_t                             char_type;
    uint16_t                            val_len;
    const uint8_t                       *val;
} bt_lea_evt_mcp_mcc_char_value_ind_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             mute;
} bt_lea_evt_mic_state_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    bt_lea_iso_link_quality_info_t      param;
} bt_lea_evt_iso_link_quality_ind_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             con_lid;
} bt_lea_evt_pacs_cccd_written_ind_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             con_lid;
    const bt_lea_call_info_t            *param;
} bt_lea_evt_call_state_change_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             con_lid;
    uint8_t                             bearer_lid;
    uint8_t                             value;
} bt_lea_evt_call_srv_sig_strength_value_ind_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             con_lid;
    uint8_t                             bearer_lid;
    bool                                inband_ring;
    bool                                silent_mode;
} bt_lea_evt_call_status_flags_ind_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             con_lid;
    uint8_t                             bearer_lid;
    bool                                local_hold_op_supported;
    bool                                join_op_supported;
} bt_lea_evt_call_ccp_opt_supported_opcode_ind_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             con_lid;
    uint8_t                             bearer_lid;
    uint8_t                             call_id;
    uint8_t                             reason;
} bt_lea_evt_call_terminate_reason_ind_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             con_lid;
    uint8_t                             bearer_lid;
    uint8_t                             call_id;
    uint8_t                             url_len;
    const uint8_t                       *p_url;
} bt_lea_evt_call_incoming_num_inf_ind_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             con_lid;
    uint8_t                             bearer_lid;
} bt_lea_evt_call_svc_changed_ind_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             con_lid;
    const bt_lea_call_action_ret_t      *p_ret;
} bt_lea_evt_call_action_result_ind_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             con_lid;
    uint8_t                             bearer_lid;
    uint8_t                             char_type;
    uint8_t                             err_code;
} bt_lea_evt_ccp_set_cfg_cmp_ind_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    const bt_lea_cis_info_t            *p_cis_estb;
} bt_lea_evt_cis_established_ind_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             con_lid;
    uint8_t                             cig_id;
    uint8_t                             cis_id;
    uint8_t                             reason;
} bt_lea_evt_cis_disconnected_ind_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint16_t                            con_hdl;
    uint8_t                             error;
} bt_lea_evt_cis_rejected_ind_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             cig_id;
    uint8_t                             group_lid;
    uint8_t                             stream_lid;
    uint8_t                             reason;
} bt_lea_evt_cig_terminated_ind_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             opcode;
    uint8_t                             nb_ases;
    uint8_t                             ase_lid;
    uint8_t                             rsp_code;
    uint8_t                             reason;
} bt_lea_evt_ase_ntf_value_ind_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             con_lid;
    uint8_t                             ase_lid;
    bt_lea_codec_id_t                   codec_id;
    uint8_t                             tgt_latency;
    bt_lea_codec_cfg_t                  codec_cfg_req;
    bt_lea_qos_req_t                    ntf_qos_req;
} bt_lea_evt_ase_codec_cfg_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             con_lid;
    uint8_t                             ase_lid;
    const void                          *param;
} bt_lea_evt_ase_enable_req_ind_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             con_lid;
    uint8_t                             ase_lid;
    const void                           *param;
    uint8_t                             state;
} bt_lea_evt_ase_metadata_update_ind_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             grp_lid;
    uint8_t                             state;
    uint32_t                            stream_pos_bf;
} bt_lea_evt_bis_sink_status_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             grp_lid;
} bt_lea_evt_bis_sink_enable_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             grp_lid;
} bt_lea_evt_bis_sink_disable_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             grp_lid;
} bt_lea_evt_bis_sink_stream_start_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             grp_lid;
} bt_lea_evt_bis_sink_stream_stop_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             src_lid;
    uint8_t                             con_lid;
    uint8_t                             pa_syn_req;
} bt_lea_evt_bis_deleg_source_add_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             src_lid;
    uint8_t                             con_lid;
} bt_lea_evt_bis_deleg_source_rm_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             src_lid;
    uint8_t                             con_lid;
    uint8_t                             pa_syn_req;
} bt_lea_evt_bis_deleg_source_upd_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             con_lid;
    uint8_t                             media_lid;
    uint8_t                             char_type;
    uint8_t                             err_code;
} bt_lea_evt_mcp_set_cfg_cmp_ind_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             con_lid;
    uint8_t                             char_type;
    bool                                ntf_enable;
} bt_lea_evt_vcp_vcs_cccd_written_t;

typedef struct
{
    bt_lea_evt_header_t                 header;
    uint8_t                             con_lid;
    uint8_t                             err_code;
    uint16_t                            role_bf;
} bt_lea_evt_tmap_read_role_cmp_ind_t;

typedef union
{
    bt_lea_evt_mob_state_t                         lea_mob_connection_state;
    bt_lea_evt_adv_state_t                         lea_adv_state_changed;
    bt_lea_evt_vol_changed_t                       lea_vol_changed;
    bt_lea_evt_vocs_offset_changed_t               lea_vocs_offset_changed;
    bt_lea_evt_vocs_bond_data_changed_t            lea_vocs_bond_data_changed;
    bt_lea_evt_media_track_changed_t               lea_media_track_changed;
    bt_lea_evt_stream_status_changed_t             lea_stream_state;
    bt_lea_evt_mcp_mcc_char_value_ind_t            lea_mcp_mcc_char_value;
    bt_lea_evt_mic_state_t                         lea_mic_state;
    bt_lea_evt_iso_link_quality_ind_t              lea_iso_link_quality_ind;
    bt_lea_evt_pacs_cccd_written_ind_t             lea_pacs_cccd_written_ind;
    bt_lea_evt_call_state_change_t                 lea_call_state_change;
    bt_lea_evt_call_srv_sig_strength_value_ind_t   lea_call_srv_sig_strength_value_ind;
    bt_lea_evt_call_status_flags_ind_t             lea_call_status_flags_ind;
    bt_lea_evt_call_ccp_opt_supported_opcode_ind_t lea_call_ccp_opt_support_opcode_ind;
    bt_lea_evt_call_terminate_reason_ind_t         lea_call_terminate_reason_ind;
    bt_lea_evt_call_incoming_num_inf_ind_t         lea_call_incoming_num_inf_ind;
    bt_lea_evt_call_svc_changed_ind_t              lea_call_svc_changed_ind;
    bt_lea_evt_call_action_result_ind_t            lea_call_action_result_ind;
    bt_lea_evt_cis_established_ind_t               lea_cis_established_ind;
    bt_lea_evt_cis_disconnected_ind_t              lea_cis_disconnected_ind;
    bt_lea_evt_cis_rejected_ind_t                  lea_cis_rejected_ind;
    bt_lea_evt_cig_terminated_ind_t                lea_cig_terminated_ind;
    bt_lea_evt_ase_ntf_value_ind_t                 lea_ase_ntf_value_ind;
    bt_lea_evt_ase_codec_cfg_t                     lea_ase_codec_cfg_value_ind;
    bt_lea_evt_ase_metadata_update_ind_t           lea_ase_metadata_update_ind;
    bt_lea_evt_bis_sink_status_t                   lea_bis_sink_state_ind;
    bt_lea_evt_bis_sink_enable_t                   lea_bis_sink_enable_ind;
    bt_lea_evt_bis_sink_disable_t                  lea_bis_sink_disable_ind;
    bt_lea_evt_bis_sink_stream_start_t             lea_bis_sink_stream_start_ind;
    bt_lea_evt_bis_sink_stream_stop_t              lea_bis_sink_stream_stop_ind;
    bt_lea_evt_bis_deleg_source_add_t              lea_bis_deleg_source_add_ind;
    bt_lea_evt_bis_deleg_source_rm_t               lea_bis_deleg_source_removed_ind;
    bt_lea_evt_bis_deleg_source_upd_t              lea_bis_deleg_source_upd_ind;
    bt_lea_evt_mcp_set_cfg_cmp_ind_t               lea_mcp_mcc_set_cfg_cmp_ind;
    bt_lea_evt_vcp_vcs_cccd_written_t              lea_vcp_vcs_cccd_written_ind;
    bt_lea_evt_tmap_read_role_cmp_ind_t            lea_tmap_read_role_cmp_ind;
    bt_lea_evt_ase_enable_req_ind_t                lea_ase_enable_req_ind;
    bt_lea_evt_mcp_mcc_svc_changed_t               lea_mcp_mcc_svc_changed_ind;
    bt_lea_evt_ccp_set_cfg_cmp_ind_t               lea_ccp_tbc_set_cfg_cmp_ind;
} bt_lea_evt_packet_t;

typedef void (*bt_lea_evt_callback)(const bt_lea_evt_packet_t *evt_pkt);

typedef struct
{
    uint8_t data[6];
} __attribute__((__packed__)) bt_lea_rsi_t;

/// BIG Info Report
typedef struct
{
    /// Value of the SDU interval in microseconds (Range 0x0000FF-0x0FFFFF)
    uint32_t  sdu_interval;
    /// Value of the ISO Interval (1.25 ms unit)
    uint16_t  iso_interval;
    /// Value of the maximum PDU size (Range 0x0000-0x00FB)
    uint16_t  max_pdu;
    /// VValue of the maximum SDU size (Range 0x0000-0x0FFF)
    uint16_t  max_sdu;
    /// Number of BIS present in the group (Range 0x01-0x1F)
    uint8_t   num_bis;
    /// Number of sub-events (Range 0x01-0x1F)
    uint8_t   nse;
    /// Burst number (Range 0x01-0x07)
    uint8_t   bn;
    /// Pre-transmit offset (Range 0x00-0x0F)
    uint8_t   pto;
    /// Initial retransmission count (Range 0x01-0x0F)
    uint8_t   irc;
    /// PHY used for transmission (0x01: 1M, 0x02: 2M, 0x03: Coded, All other values: RFU)
    uint8_t   phy;
    /// Framing mode (0x00: Unframed, 0x01: Framed, All other values: RFU)
    uint8_t   framing;
    /// True if broadcast isochronous group is encrypted, False otherwise
    bool      encrypted;
} bt_ble_big_info_t;

/**
* @brief BIS Sink related event callbacks.
*/
typedef struct
{
    /**
     * @brief BIS Sink scan state callback.
     *
     * This callback is triggered when the BIS Sink scan procedure starts
     * or stops.
     *
     * @param scan_started  True if scanning has started, false if stopped.
     * @param err_code      Error code indicating the result of the operation.
     *                      Zero indicates success.
     */
    void (*bis_sink_scan_state_cb)(bool scan_started, uint16_t err_code);

    /**
     * @brief BIS Sink scan report callback.
     *
     * This callback is invoked when an Extended Advertising report related
     * to a BIS broadcaster is received during scanning.
     *
     * @param p_addr        Pointer to the advertiser device address.
     * @param ea_sid        Extended Advertising SID.
     * @param bcast_id      Pointer to the Broadcast ID.
     * @param ea_data       Pointer to the extended advertising data.
     * @param ea_data_len   Length of the extended advertising data.
     * @param ea_rssi       RSSI value of the received advertising packet.
     *
     * @return              True to continue scanning, false to stop scanning.
     */
    bool (*bis_sink_scan_report_cb)(const ble_bdaddr_t *p_addr, uint8_t ea_sid, const uint8_t *bcast_id,
                                    const uint8_t *ea_data, uint8_t ea_data_len, int8_t ea_rssi);

    /**
     * @brief Periodic Advertising (PA) synchronization state callback.
     *
     * This callback is triggered when PA synchronization is established
     * or lost for a BIS broadcaster.
     *
     * @param pa_synced     True if PA is successfully synchronized,
     *                      false if synchronization is lost or failed.
     * @param p_addr        Pointer to the broadcaster device address.
     * @param ea_sid        Extended Advertising SID associated with the PA.
     * @param sync_hdl      Local handle identifier of the associated PA synchronization.
     * @param err_code      Error code indicating the result of the operation.
     */
    void (*bis_sink_pa_state_cb)(bool pa_synced, const ble_bdaddr_t *p_addr, uint8_t ea_sid,
                                 uint16_t sync_hdl, uint16_t err_code);

    /**
     * @brief Periodic Advertising data report callback.
     *
     * This callback is invoked when Periodic Advertising data is received
     * from the synchronized broadcaster.
     *
     * @param sync_hdl      Local handle identifier of the associated PA synchronization.
     * @param pa_data       Pointer to the received PA data.
     * @param pa_data_len   Length of the PA data.
     * @param big_info      Pointer to the parsed BIG information, if present.
     */
    void (*bis_sink_pa_report_cb)(uint16_t sync_hdl, const uint8_t *pa_data, uint8_t pa_data_len,
                                  const bt_ble_big_info_t *big_info);

    /**
     * @brief BIS BIG state callback.
     *
     * This callback is triggered when the BIS Sink starts or stops
     * receiving a BIG (Broadcast Isochronous Group).
     *
     * @param sink_started  True if BIS Sink has started, false if stopped.
     * @param sync_hdl      Local handle identifier of the associated PA synchronization.
     * @param grp_lid       Local identifier of the BIG group.
     * @param err_code      Error code indicating the result of the operation.
     */
    void (*bis_sink_big_state_cb)(bool sink_started, uint16_t sync_hdl, uint8_t grp_lid, uint16_t err_code);

} bt_ble_bis_sink_evt_cbs_t;
