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
#include "a2dp_codec_lhdc.h"
#include "bta_bt_api.h"

#if defined(A2DP_LHDC_ON)

#if 0
const unsigned char a2dp_codec_lhdc_elements[A2DP_LHDC_OCTET_NUMBER] = {
    0x3A, 0x05, 0x00, 0x00, //Vendor ID
    0x4C, 0x48,         //Codec ID
    (A2DP_LHDC_SR_96000 | A2DP_LHDC_SR_48000 | A2DP_LHDC_SR_44100) | (A2DP_LHDC_FMT_16 | A2DP_LHDC_FMT_24),
};
#else
//V2

const unsigned char a2dp_codec_lhdc_elements[A2DP_LHDC_OCTET_NUMBER] = {
    0x3A, 0x05, 0x00, 0x00, //Vendor ID
    0x33, 0x4c,         //Codec ID
    (A2DP_LHDC_SR_96000 | A2DP_LHDC_SR_48000 | A2DP_LHDC_SR_44100) | (A2DP_LHDC_FMT_16 | A2DP_LHDC_FMT_24) /*| A2DP_LHDC_AR | A2DP_LHDC_JAS*/,
    (A2DP_LHDC_LLC_ENABLE | A2DP_LHDC_MAX_SR_900 | A2DP_LHDC_VERSION_NUM | A2DP_LHDC_LLAC),
    (A2DP_LHDC_COF_CSC_DISABLE | A2DP_LHDC_V4 | A2DP_LHDC_MinBR /*| A2DP_LHDC_MQA | A2DP_LHDC_LARC*/)
};
#endif /* if 0 */

void a2dp_codec_lhdc_init(void)
{
    bta_a2dp_codec_init(BT_A2DP_CODEC_TYPE_NON_A2DP, 
            BT_A2DP_CODEC_NONE_TYPE_LHDC,
            BT_A2DP_CODEC_PRORITY_4,
            (uint8_t *)&a2dp_codec_lhdc_elements,
            sizeof(a2dp_codec_lhdc_elements));
}
#endif /* A2DP_LHDC_ON */
#endif /* BT_A2DP_SUPPORT */
