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
#ifndef __GAF_SHS_COMMON_H__
#define __GAF_SHS_COMMON_H__

#include "gaf_cfg.h"

/// Characteristic type values for Telephony and Media Audio Service
enum shp_char_type
{
    /// SHP SAT Features characteristic
    SHP_CHAR_TYPE_SAT_FEAT = 0,
    SHP_CHAR_TYPE_MAX,
};

/// SHP SAT Features characteristic bit field meaning
enum shp_sat_feat_bf
{
    /// Indicate if Server supports SAT Audio Source feature (= 1) or not - Bit
    SHP_SAT_FEAT_AUDIO_SOURCE_BIT   = CO_BIT(0),
    /// Indicate if Server supports SAT 96 kbps Sink feature (= 1) or not - Bit
    SHP_SAT_FEAT_SINK_96_KBPS_BIT   = CO_BIT(1),
    /// Indicate if Server supports SAT 64 kbps Source feature (= 1) or not - Bit
    SHP_SAT_FEAT_SRC_64KBPS_BIT     = CO_BIT(2),
    /// Indicate if Server supports SAT 64 kbps Sink feature (= 1) or not - Bit
    SHP_SAT_FEAT_SINK_64KBPS_BIT    = CO_BIT(3),
    /// Mask indicating that all sat features are supported
    SHP_SAT_FEAT_ALLSUPP_MASK       = 0x0F,
    /// Mask indicating RFU bits
    SHP_SAT_FEAT_RFU_MASK           = 0xF0,
};

#endif /// __GAF_SHS_COMMON__
