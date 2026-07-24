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
#include "a2dp_codec_mihc.h"
#include "bta_bt_api.h"

#ifdef A2DP_MIHC_ON
static a2dp_mihc_info_t a2dp_mihc_info[BT_DEVICE_NUM];  // Currently only used by decoder as bt sink
static btif_avdtp_codec_t a2dp_mihc_avdtpcodec;

const unsigned char a2dp_codec_mihc_elements[] = {
    0x8F, 0x03, 0x00, 0x00, // Vendor ID: 0x0000 038F
    0x31, 0x4D,             // Codec ID : 0x4D31
    A2DP_MIHC_VERSION_0,    // Version(Previous versions are supported by default)
    (A2DP_MIHC_SR_96000 | A2DP_MIHC_SR_48000 | A2DP_MIHC_SR_44100 | A2DP_MIHC_SR_32000 | A2DP_MIHC_SR_24000 | A2DP_MIHC_SR_16000),
    (A2DP_MIHC_BD_24 | A2DP_MIHC_BD_16 | A2DP_MIHC_FD_5MS | A2DP_MIHC_CBR),
    (A2DP_MIHC_MODE_LOSSY | A2DP_MIHC_MODE_LOSSLESS | A2DP_MIHC_MODE_LOW_LATENCY | A2DP_MIHC_MODE_ADATPIVE | A2DP_MIHC_CM_MONO | A2DP_MIHC_CM_STEREO),
    A2DP_MIHC_MIN_BR_DEFAULT,   // min-bitrate is version-dependent,
    A2DP_MIHC_SUB_VERSION_2 | A2DP_MIHC_SUB_VERSION_1,    // A2DP_MIHC_MD multi channel mode not support
    A2DP_MIHC_ALL_BITS_ZERO,
};

void a2dp_codec_mihc_init(void)
{
    bta_a2dp_codec_init(BT_A2DP_CODEC_TYPE_NON_A2DP, 
            BT_A2DP_CODEC_NONE_TYPE_MIHC,
            BT_A2DP_CODEC_PRORITY_5,
            (uint8_t *)&a2dp_codec_mihc_elements,
            sizeof(a2dp_codec_mihc_elements));
}

a2dp_mihc_info_t* a2dp_codec_mihc_get_info(int index)
{
    if (index < 0 || index >= BT_DEVICE_NUM)
    {
        return NULL;
    }

    return &a2dp_mihc_info[index];
}

#endif /* A2DP_MIHC_ON */

#endif /* BT_A2DP_SUPPORT */
