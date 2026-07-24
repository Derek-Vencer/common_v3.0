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
#if defined(A2DP_L2HC_ON)
#include <stdio.h>
#include "a2dp_codec_l2hc.h"
#include "bta_bt_api.h"

const unsigned char a2dp_codec_l2hc_elements[] = {
    0xCF, 0x0C, 0x00, 0x00, // Vendor ID: 0x0000 0CCF
    0x01, 0xCA,             // Codec ID : 0xCAO1
    (A2DP_L2HC_VERS_CAP | A2DP_L2HC_RES_32 | A2DP_L2HC_RES_24 | A2DP_L2HC_RES_16),
    (A2DP_L2HC_SR_192000 | A2DP_L2HC_SR_176400 | A2DP_L2HC_SR_96000 | A2DP_L2HC_SR_88200 | A2DP_L2HC_SR_48000 | A2DP_L2HC_SR_44100 | A2DP_L2HC_SR_32000),
    (A2DP_L2HC_BR_1920 | A2DP_L2HC_BR_1600 | A2DP_L2HC_BR_1280 | A2DP_L2HC_BR_960 | A2DP_L2HC_BR_640 | A2DP_L2HC_BR_480 | A2DP_L2HC_BR_320),
    (A2DP_L2HC_BR_256  | A2DP_L2HC_BR_192  | A2DP_L2HC_BR_128  | A2DP_L2HC_BR_96  | A2DP_L2HC_BR_64 | A2DP_L2HC_FRAME_7D5MS | A2DP_L2HC_FRAME_10MS),
    A2DP_L2HC_FRAME_5MS | A2DP_L2HC_CM_MONO | A2DP_L2HC_CM_DUAL,
    A2DP_L2HC_ALL_BITS_ZERO,
};

void a2dp_codec_l2hc_init(void)
{
    bta_a2dp_codec_init(BT_A2DP_CODEC_TYPE_NON_A2DP, 
            BT_A2DP_CODEC_NONE_TYPE_L2HC,
            BT_A2DP_CODEC_PRORITY_5,
            (uint8_t *)&a2dp_codec_l2hc_elements,
            sizeof(a2dp_codec_l2hc_elements));
}
#endif /* A2DP_L2HC_ON */
#endif /* BT_A2DP_SUPPORT */
