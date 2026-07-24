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
#include "tgt_hardware.h"
#include "a2dp_codec_sbc.h"
#ifndef BT_SERVICE_ENABLE
#include "bt_a2dp_types.h"
#include "app_a2dp.h"
#else
#include "bta_bt_api.h"
#endif

const unsigned char a2dp_codec_elements[] = {
    A2D_SBC_IE_SAMP_FREQ_48 | A2D_SBC_IE_SAMP_FREQ_44 | A2D_SBC_IE_CH_MD_MONO | A2D_SBC_IE_CH_MD_STEREO | A2D_SBC_IE_CH_MD_JOINT | A2D_SBC_IE_CH_MD_DUAL,
    A2D_SBC_IE_BLOCKS_16 | A2D_SBC_IE_BLOCKS_12 | A2D_SBC_IE_BLOCKS_8  |A2D_SBC_IE_BLOCKS_4 | A2D_SBC_IE_SUBBAND_8 | A2D_SBC_IE_SUBBAND_4 | A2D_SBC_IE_ALLOC_MD_L | A2D_SBC_IE_ALLOC_MD_S,
    A2D_SBC_IE_MIN_BITPOOL,
    BTA_AV_CO_SBC_MAX_BITPOOL
};

#if defined(CUSTOM_BITRATE) || defined(BTIF_DIP_DEVICE)
static unsigned char a2dp_codec_elements_user_configure[] = {
    A2D_SBC_IE_SAMP_FREQ_48 | A2D_SBC_IE_SAMP_FREQ_44 | A2D_SBC_IE_CH_MD_MONO | A2D_SBC_IE_CH_MD_STEREO | A2D_SBC_IE_CH_MD_JOINT | A2D_SBC_IE_CH_MD_DUAL,
    A2D_SBC_IE_BLOCKS_16 | A2D_SBC_IE_BLOCKS_12 | A2D_SBC_IE_BLOCKS_8  |A2D_SBC_IE_BLOCKS_4 | A2D_SBC_IE_SUBBAND_8 | A2D_SBC_IE_SUBBAND_4 | A2D_SBC_IE_ALLOC_MD_L | A2D_SBC_IE_ALLOC_MD_S,
    A2D_SBC_IE_MIN_BITPOOL,
    BTA_AV_CO_SBC_MAX_BITPOOL
};

void a2dp_avdtpcodec_sbc_user_configure_set(uint32_t bitpool,uint8_t user_configure)
{
    int32_t lock;
    lock = int_lock();
    memcpy((uint8_t*)&a2dp_codec_elements_user_configure,(uint8_t*)a2dp_codec_elements,sizeof(a2dp_codec_elements));

    if(user_configure)
    {
        if(bitpool > BTA_AV_CO_SBC_MAX_BITPOOL)
        {
            bitpool = BTA_AV_CO_SBC_MAX_BITPOOL;
        }
        a2dp_codec_elements_user_configure[3] = bitpool;
    }
    else
    {
        a2dp_codec_elements_user_configure[3] = BTA_AV_CO_SBC_MAX_BITPOOL;
    }
    int_unlock(lock);
}

uint8_t a2dp_avdtpcodec_sbc_user_bitpool_get()
{
    return a2dp_codec_elements_user_configure[3];
}
#endif

void a2dp_codec_sbc_init(void)
{
#ifdef BT_SERVICE_ENABLE
#ifdef CUSTOM_BITRATE
    bta_a2dp_codec_init(BT_A2DP_CODEC_TYPE_SBC,
            BT_A2DP_CODEC_NONE_TYPE_INVALID,
            BT_A2DP_CODEC_PRORITY_2,
            (uint8_t *)&a2dp_codec_elements_user_configur,
            4);
#else
    bta_a2dp_codec_init(BT_A2DP_CODEC_TYPE_SBC,
            BT_A2DP_CODEC_NONE_TYPE_INVALID,
            BT_A2DP_CODEC_PRORITY_2,
            (uint8_t *)&a2dp_codec_elements,
            4);
#endif
#else
#ifdef CUSTOM_BITRATE
    app_a2dp_codec_init(BT_A2DP_CODEC_TYPE_SBC,
            BT_A2DP_CODEC_NONE_TYPE_INVALID,
            BT_A2DP_CODEC_PRORITY_2,
            (uint8_t *)&a2dp_codec_elements_user_configur,
            4);
#else
    app_a2dp_codec_init(BT_A2DP_CODEC_TYPE_SBC,
            BT_A2DP_CODEC_NONE_TYPE_INVALID,
            BT_A2DP_CODEC_PRORITY_2,
            (uint8_t *)&a2dp_codec_elements,
            4);
#endif
#endif
}

#endif /* BT_A2DP_SUPPORT */
