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
#ifndef __BAP_PAC_COMMON_H__
#define __BAP_PAC_COMMON_H__

#include "gaf_cfg.h"
#include "generic_audio.h"

#define PACS_MAX_PRES_DELAY_US               (0x00FFFFFF)

/// Characteristic type values for Published Audio Capabilities Service
enum pacs_char_type
{
    /// Available Audio Contexts characteristic
    PACS_CHAR_TYPE_CONTEXT_AVA = 0,
    /// Supported Audio Contexts characteristic
    PACS_CHAR_TYPE_CONTEXT_SUPP,
    /// Sink Audio Locations characteristic
    PACS_CHAR_TYPE_LOC_SINK,
    /// Source Audio Locations characteristic
    PACS_CHAR_TYPE_LOC_SRC,
    /// Channel capabilities characteristic
    PACS_CHAR_TYPE_CHAN_CAPA,
    /// Prefer Audio Configuration characteristic
    PACS_CHAR_TYPE_PREF_AUD_CFG,
    /// SINK PAC characteristic
    PACS_CHAR_TYPE_SINK_PAC,
    /// SRC PAC characteristic
    PACS_CHAR_TYPE_SRC_PAC,
    /// SINK PAC V2 characteristic
    PACS_CHAR_TYPE_SINK_PAC_V2,
    /// SRC PAC V2 characteristic
    PACS_CHAR_TYPE_SRC_PAC_V2,
    /// MAY BE MORE THAN ONE PAC
    PACS_CHAR_TYPE_MAX,
};

enum pacs_channel_type_id
{
    /// Generic Channels
    PACS_CHAN_TYPE_ID_GENERIC   = 0x0000,
    /// Ambisonics
    PACS_CHAN_TYPE_ID_AMBI      = 0x0001,
    /// RFU
    PACS_CHAN_TYPE_ID_MAX       = 0x0002,
};

/// Bitfield of supported audio channels
enum pacs_ambisonic_channels
{
    PACS_AMBISONIC_CHAN_RFU     = 0x0000,
    PACS_AMBISONIC_CHAN_W       = 0x0001,
    PACS_AMBISONIC_CHAN_X       = 0x0002,
    PACS_AMBISONIC_CHAN_Y       = 0x0004,
    PACS_AMBISONIC_CHAN_Z       = 0x0008,

    PACS_AMBISONIC_CHAN_MASK    = 0x000F,
};

/// Bitfield of supported normalization schemes
enum pacs_ambisonics_normalization
{
    PACS_AMBISONIC_NORMLZ_RFU   = 0x00,
    PACS_AMBISONIC_NORMLZ_MAX_N = 0x01,
    PACS_AMBISONIC_NORMLZ_SN3D  = 0x02,
    PACS_AMBISONIC_NORMLZ_N3D   = 0x04,

    PACS_AMBISONIC_NORMLZ_MASK  = 0x07,
};

enum pacs_pref_aud_cfg_data_present_bit
{
    /// Preferred Audio Configuration Unicast AC ID
    PACS_PREF_AC_DATA_UNICAST_AC_ID        = 0b01,
    /// Preferred Audio Configuration Broadcast AC ID
    PACS_PREF_AC_DATA_BROADCAST_AC_ID      = 0b10,
    /// Preferred Audio Configuration
    PACS_PREF_AC_DATA_AUDIO_CONFIG_ID_MASK = 0b11,
    /// Preferred Sink PAC Record List
    PACS_PREF_AC_DATA_SINK_PAC_RECORD_LIST = 0x04,
    /// Preferred QoS Setting Sink
    PACS_PREF_AC_DATA_SINK_QOS_SETTING     = 0x08,
    /// Preferred Source PAC Record List
    PACS_PREF_AC_DATA_SRC_PAC_RECORD_LIST  = 0x10,
    /// Preferred QoS Setting Source
    PACS_PREF_AC_DATA_SRC_QOS_SETTING      = 0x20,
    /// Preferred Codec Configuration
    PACS_PREF_AC_DATA_CODEC_CONFIGURATION  = 0x40,
    /// Preferred Presentation Delay
    PACS_PREF_AC_DATA_PRES_DELAY_US_SINK   = 0x80,
    /// Preferred Presentation Delay
    PACS_PREF_AC_DATA_PRES_DELAY_US_SRC    = 0x100,

    PACS_PREF_AC_DATA_MASK                 = 0x1FF,
};

typedef union pacs_pref_aud_cfg_data_aud_cfg_id
{
    uint8_t ac_id[13];
    uint8_t uc_ac_id[13];
    uint8_t bc_ac_id[4];
} pacs_pref_ac_data_aud_cfg_id_u;

typedef struct pacs_pref_aud_cfg_data_pac_record
{
    uint8_t pref_pac_num;
    struct pref_pac_info
    {
        uint8_t pac_set_id;
        uint8_t pac_idx;
    } pac_info[GAF_ARRAY_EMPTY];
} pacs_pref_ac_data_pac_rec_t;

typedef struct pacs_pref_aud_cfg_data_qos_setting
{
    uint32_t sdu_interval_us;
    uint8_t framing_type;
    uint16_t max_sdu_size;
    uint8_t rtn;
    uint16_t trans_latency_ms;
} pacs_pref_ac_data_qos_setting_t;

typedef struct pacs_pref_aud_cfg_data_codec_cfg
{
    uint8_t codec_id[GEN_AUD_CODEC_ID_LEN];
    gen_aud_cc_t codec_cfg;
} pacs_pref_ac_data_specific_cc_t;

typedef struct pacs_pref_aud_cfg_data_codec_cfg_ptr
{
    uint8_t codec_id[GEN_AUD_CODEC_ID_LEN];
    gen_aud_cc_ptr_t codec_cfg;
} pacs_pref_ac_data_specific_cc_ptr_t;

typedef struct pacs_pref_aud_cfg_data_pres_delay
{
    uint32_t pres_delay_us;
} pacs_pref_ac_data_pres_delay_t;

typedef struct pacs_pref_aud_cfg_data
{
    const pacs_pref_ac_data_aud_cfg_id_u *pref_aud_cfg_id;
    const pacs_pref_ac_data_pac_rec_t *pref_pac_sink;
    const pacs_pref_ac_data_qos_setting_t *pref_qos_setting_sink;
    const pacs_pref_ac_data_pac_rec_t *pref_pac_src;
    const pacs_pref_ac_data_qos_setting_t *pref_qos_setting_src;
    const pacs_pref_ac_data_specific_cc_t *pref_codec_cfg;
    const pacs_pref_ac_data_pres_delay_t *pref_pres_delay_sink;
    const pacs_pref_ac_data_pres_delay_t *pref_pres_delay_src;
} pacs_pref_aud_cfg_data_t;

typedef struct pacs_pref_aud_cfg_data_ptr
{
    const pacs_pref_ac_data_aud_cfg_id_u *pref_aud_cfg_id;
    const pacs_pref_ac_data_pac_rec_t *pref_pac_sink;
    const pacs_pref_ac_data_qos_setting_t *pref_qos_setting_sink;
    const pacs_pref_ac_data_pac_rec_t *pref_pac_src;
    const pacs_pref_ac_data_qos_setting_t *pref_qos_setting_src;
    const pacs_pref_ac_data_specific_cc_ptr_t *pref_codec_cfg_ptr;
    const pacs_pref_ac_data_pres_delay_t *pref_pres_delay_sink;
    const pacs_pref_ac_data_pres_delay_t *pref_pres_delay_src;
} pacs_pref_aud_cfg_data_ptr_t;

typedef struct pacs_preferred_audio_cfg
{
    /// @see pacs_pref_aud_cfg_data_present_bit
    uint16_t data_present_bits;
    /// @see generic_audio_context_type_bf
    uint16_t use_case_id;
    /// @see pacs_pref_aud_cfg_data
    pacs_pref_aud_cfg_data_t data_list;
} pacs_pref_aud_cfg_data_set_t;

typedef struct pacs_preferred_audio_cfg_ptr
{
    /// @see pacs_pref_aud_cfg_data_present_bit
    uint16_t data_present_bits;
    /// @see pacs_pref_aud_cfg_data
    pacs_pref_aud_cfg_data_ptr_t data_ptr_list;
} pacs_pref_aud_cfg_data_set_ptr_t;

typedef struct pacs_generic_chan_descriptor
{
    /// Bitdata of supported Audio Channels that
    /// are used together as one channel set.
    /// @see gen_aud_supp_loc_bf_e
    uint32_t gen_aud_chan_mask;
} pacs_gen_chan_desc_t;

typedef struct pacs_ambisonics_descriptor
{
    /// Bitfield of supported audio channels @see pacs_ambisonic_channels
    uint16_t ambisonics_channels;
    /// Bitfield of supported normalization schemes @see pacs_ambisonics_normalization
    uint8_t ambisonics_normalization;
} pacs_ambisonics_desc_t;

typedef union pacs_chan_type_descriptor
{
    /// Generic channel type
    pacs_gen_chan_desc_t gen_chan_desc;
    /// Ambisonics channel type
    pacs_ambisonics_desc_t ambisonics_desc;
} pacs_chan_type_desc_u;

#endif /// __BAP_PAC_COMMON__
