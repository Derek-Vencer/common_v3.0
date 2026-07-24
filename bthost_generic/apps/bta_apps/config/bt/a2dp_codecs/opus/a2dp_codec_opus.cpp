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
#ifdef BT_A2DP_SUPPORT
#include <stdio.h>
#include "cmsis_os.h"
#include "a2dp_codec_opus.h"
#include "bta_bt_api.h"
#include "a2dp_codec_sbc.h"
#if defined(MASTER_USE_OPUS) || defined(ALL_USE_OPUS)

const unsigned char a2dp_codec_opus_elements[] = {
    A2D_SBC_IE_SAMP_FREQ_48 | A2D_SBC_IE_SAMP_FREQ_44 | A2D_SBC_IE_CH_MD_STEREO | A2D_SBC_IE_CH_MD_JOINT,
    A2D_SBC_IE_BLOCKS_16 | A2D_SBC_IE_BLOCKS_12 | A2D_SBC_IE_SUBBAND_8 | A2D_SBC_IE_ALLOC_MD_L,
    A2D_SBC_IE_MIN_BITPOOL,
    BTA_AV_CO_SBC_MAX_BITPOOL
};

void a2dp_codec_opus_init(void)
{
    bta_a2dp_codec_init(BT_A2DP_CODEC_TYPE_NON_A2DP, 
            BT_A2DP_CODEC_NONE_TYPE_OPUS,
            BT_A2DP_CODEC_PRORITY_5,
            (uint8_t *)&a2dp_codec_opus_elements,
            sizeof(a2dp_codec_opus_elements));
}

#endif /* MASTER_USE_OPUS || ALL_USE_OPUS */
#endif /* BT_A2DP_SUPPORT */
