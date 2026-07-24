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
#include <stdio.h>
#include "cmsis_os.h"
#include "a2dp_codec_lhdcv5.h"
#include "bta_bt_api.h"

#if defined(A2DP_LHDCV5_ON)

#ifdef APP_SOUND_ENABLE
const unsigned char a2dp_codec_lhdcv5_elements[A2DP_LHDCV5_OCTET_NUMBER] = {
    0x3A, 0x05, 0x00, 0x00, //Vendor ID
    0x35, 0x4c,         //Codec ID
    (A2DP_LHDCV5_SR_48000 | A2DP_LHDCV5_SR_44100),
    (/*A2DP_LHDCV5_FMT_32 |*/ A2DP_LHDCV5_FMT_24 | A2DP_LHDCV5_FMT_16 | A2DP_LHDCV5_MAX_BR_600 | A2DP_LHDCV5_MIN_BR_64),
    (A2DP_LHDCV5_FRAME_5MS | A2DP_LHDCV5_VERSION_NUM),
    (A2DP_LHDCV5_HAS_AR | A2DP_LHDCV5_HAS_JAS | A2DP_LHDCV5_HAS_META | A2DP_LHDCV5_LL_MODE),
    (0x00/*A2DP_LHDCV5_AR_ON*/)
};
#else
const unsigned char a2dp_codec_lhdcv5_elements[A2DP_LHDCV5_OCTET_NUMBER] = {
    0x3A, 0x05, 0x00, 0x00, //Vendor ID
    0x35, 0x4c,         //Codec ID
    (A2DP_LHDCV5_SR_192000 | A2DP_LHDCV5_SR_96000 | A2DP_LHDCV5_SR_48000 | A2DP_LHDCV5_SR_44100),
    (/*A2DP_LHDCV5_FMT_32 |*/ A2DP_LHDCV5_FMT_24 | A2DP_LHDCV5_FMT_16 | A2DP_LHDCV5_MAX_BR_1000 | A2DP_LHDCV5_MIN_BR_64),
    (A2DP_LHDCV5_FRAME_5MS | A2DP_LHDCV5_VERSION_NUM),
    (A2DP_LHDCV5_HAS_AR | A2DP_LHDCV5_HAS_JAS | A2DP_LHDCV5_HAS_META | A2DP_LHDCV5_LL_MODE | A2DP_LHDCV5_LOSSLESS_MODE | A2DP_LHDCV5_LOSSLESS_96K |A2DP_LHDCV5_LOSSLESS_24BIT),
    (A2DP_LHDCV5_LOSSLESS_RAW/*A2DP_LHDCV5_AR_ON*/)
};
#endif

void a2dp_codec_lhdcv5_init(void)
{
    bta_a2dp_codec_init(BT_A2DP_CODEC_TYPE_NON_A2DP, 
            BT_A2DP_CODEC_NONE_TYPE_LHDCV5,
            BT_A2DP_CODEC_PRORITY_5,
            (uint8_t *)&a2dp_codec_lhdcv5_elements,
            sizeof(a2dp_codec_lhdcv5_elements));
}
#endif /* A2DP_LHDCV5_ON */
