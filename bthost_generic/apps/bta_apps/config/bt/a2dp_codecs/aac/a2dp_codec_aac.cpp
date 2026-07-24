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
#include "a2dp_codec_aac.h"
#ifndef BT_SERVICE_ENABLE
#include "bt_a2dp_types.h"
#include "app_a2dp.h"
#else
#include "bta_bt_api.h"
#endif

#if defined(A2DP_AAC_ON)

const unsigned char a2dp_codec_aac_elements[A2DP_AAC_OCTET_NUMBER] = {
    A2DP_AAC_OCTET0_MPEG2_AAC_LC,
    A2DP_AAC_OCTET1_SAMPLING_FREQUENCY_44100,
    A2DP_AAC_OCTET2_CHANNELS_1 | A2DP_AAC_OCTET2_CHANNELS_2 | A2DP_AAC_OCTET2_SAMPLING_FREQUENCY_48000,
    A2DP_AAC_OCTET3_VBR_SUPPORTED | ((MAX_AAC_BITRATE >> 16) & 0x7f),
    /* left bit rate 0 for unkown */
    (MAX_AAC_BITRATE >> 8) & 0xff,
    (MAX_AAC_BITRATE) & 0xff
};
#if defined(CUSTOM_BITRATE) || defined(BTIF_DIP_DEVICE)
static uint32_t aac_bitrate_user_config = MAX_AAC_BITRATE;
static unsigned char a2dp_codec_aac_elements_user_configure[A2DP_AAC_OCTET_NUMBER] = {
    A2DP_AAC_OCTET0_MPEG2_AAC_LC,
    A2DP_AAC_OCTET1_SAMPLING_FREQUENCY_44100,
    A2DP_AAC_OCTET2_CHANNELS_1 | A2DP_AAC_OCTET2_CHANNELS_2 | A2DP_AAC_OCTET2_SAMPLING_FREQUENCY_48000,
    A2DP_AAC_OCTET3_VBR_SUPPORTED | ((MAX_AAC_BITRATE >> 16) & 0x7f),
    /* left bit rate 0 for unkown */
    (MAX_AAC_BITRATE >> 8) & 0xff,
    (MAX_AAC_BITRATE) & 0xff
};

void a2dp_avdtpcodec_aac_user_configure_set(uint32_t bitrate,uint8_t user_configure)
{
    int32_t lock;
    lock = int_lock();

    memcpy((uint8_t*)&a2dp_codec_aac_elements_user_configure,(uint8_t*)a2dp_codec_aac_elements,sizeof(a2dp_codec_aac_elements));

    if(user_configure)
    {
        if(bitrate > MAX_AAC_BITRATE)
        {
            bitrate = MAX_AAC_BITRATE;
        }
        a2dp_codec_aac_elements_user_configure[3] = A2DP_AAC_OCTET3_VBR_SUPPORTED | ((bitrate >> 16) & 0x7f);
        a2dp_codec_aac_elements_user_configure[4] = (bitrate>>8)&0xff;
        a2dp_codec_aac_elements_user_configure[5] = (bitrate)&0xff;
        aac_bitrate_user_config = bitrate;
    }
    else
    {
        a2dp_codec_aac_elements_user_configure[3] = A2DP_AAC_OCTET3_VBR_SUPPORTED | ((MAX_AAC_BITRATE >> 16) & 0x7f);
        a2dp_codec_aac_elements_user_configure[4] = (MAX_AAC_BITRATE >> 8) & 0xff;
        a2dp_codec_aac_elements_user_configure[5] = (MAX_AAC_BITRATE) & 0xff;
        aac_bitrate_user_config = MAX_AAC_BITRATE;
    }

    int_unlock(lock);
}

uint32_t a2dp_avdtpcodec_aac_user_bitrate_get()
{
    return aac_bitrate_user_config;
}
#endif

void a2dp_codec_aac_init(void)
{
#ifdef BT_SERVICE_ENABLE
#ifdef CUSTOM_BITRATE
    bta_a2dp_codec_init(BT_A2DP_CODEC_TYPE_MPEG2_4_AAC,
            BT_A2DP_CODEC_NONE_TYPE_INVALID,
            BT_A2DP_CODEC_PRORITY_3,
            (uint8_t *)&a2dp_codec_aac_elements_user_configure,
            sizeof(a2dp_codec_aac_elements_user_configure));
#else
    bta_a2dp_codec_init(BT_A2DP_CODEC_TYPE_MPEG2_4_AAC,
            BT_A2DP_CODEC_NONE_TYPE_INVALID,
            BT_A2DP_CODEC_PRORITY_3,
            (uint8_t *)&a2dp_codec_aac_elements,
            sizeof(a2dp_codec_aac_elements));
#endif

#else
#ifdef CUSTOM_BITRATE
    app_a2dp_codec_init(BT_A2DP_CODEC_TYPE_MPEG2_4_AAC,
            BT_A2DP_CODEC_NONE_TYPE_INVALID,
            BT_A2DP_CODEC_PRORITY_3,
            (uint8_t *)&a2dp_codec_aac_elements_user_configure,
            sizeof(a2dp_codec_aac_elements_user_configure));
#else
    app_a2dp_codec_init(BT_A2DP_CODEC_TYPE_MPEG2_4_AAC,
            BT_A2DP_CODEC_NONE_TYPE_INVALID,
            BT_A2DP_CODEC_PRORITY_3,
            (uint8_t *)&a2dp_codec_aac_elements,
            sizeof(a2dp_codec_aac_elements));
#endif
#endif
}

#endif /* A2DP_AAC_ON */
#endif /* BT_A2DP_SUPPORT */
