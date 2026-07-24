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
#if defined(A2DP_LC3_ON)
#include <stdio.h>
#include "cmsis_os.h"
#include "a2dp_codec_lc3.h"
#include "bta_bt_api.h"

#if defined(A2DP_LC3_HR)
const unsigned char a2dp_codec_lc3_elements[A2DP_LC3_OCTET_NUMBER] = {
    0xA9, 0x08, 0x00, 0x00, //Vendor ID 0x0000038F  0x000008A9
    0x01, 0x00,         //Codec ID : LC3:0x8FAA, LC3 plus:0x8FAB LC3 plus:0x001
    (A2DP_LC3_FRAME_LEN_10MS | A2DP_LC3_FRAME_LEN_5MS | A2DP_LC3_FRAME_LEN_2POINT5MS),
    (A2DP_LC3_CH_MD_MONO | A2DP_LC3_CH_MD_STEREO),
    (A2DP_LC3_SR_48000),
    (A2DP_LC3_SR_96000),
    (A2DP_LC3_FMT_16BIT | A2DP_LC3_FMT_24BIT | A2DP_LC3_FMT_32BIT),
};
#else
const unsigned char a2dp_codec_lc3_elements[A2DP_LC3_OCTET_NUMBER] = {
    0x8F, 0x03, 0x00, 0x00, //Vendor ID 0x0000038F
    0xAA, 0x8F,         //Codec ID : LC3:0x8FAA, LC3 plus:0x8FAB
    (A2DP_LC3_SR_96000 | A2DP_LC3_SR_48000 | A2DP_LC3_SR_44100 | A2DP_LC3_SR_32000 | 
        A2DP_LC3_SR_24000 | A2DP_LC3_SR_16000| A2DP_LC3_SR_8000),
    (A2DP_LC3_FMT_16BIT | A2DP_LC3_FMT_24BIT | A2DP_LC3_FMT_32BIT | 
        A2DP_LC3_FRAME_LEN_10MS | A2DP_LC3_FRAME_LEN_7POINT5MS| A2DP_LC3_FRAME_LEN_5MS | A2DP_LC3_FRAME_LEN_2POINT5MS),
    (A2DP_LC3_BITRATE_900kBPS | A2DP_LC3_BITRATE_600kBPS | A2DP_LC3_BITRATE_500kBPS | A2DP_LC3_BITRATE_400kBPS | 
        A2DP_LC3_BITRATE_300kBPS | A2DP_LC3_BITRATE_200kBPS |A2DP_LC3_BITRATE_100kBPS  | A2DP_LC3_BITRATE_64kBPS),
    (A2DP_LC3_CH_MD_MONO | A2DP_LC3_CH_MD_STEREO | A2DP_LC3_CH_MD_MUlTI_MONO | A2DP_LC3_BR_MODE_AUTO),
};
#endif

void a2dp_codec_lc3_init(void)
{
    bta_a2dp_codec_init(BT_A2DP_CODEC_TYPE_NON_A2DP, 
            BT_A2DP_CODEC_NONE_TYPE_LC3,
            BT_A2DP_CODEC_PRORITY_3,
            (uint8_t *)&a2dp_codec_lc3_elements,
            sizeof(a2dp_codec_lc3_elements));
}
#endif /* A2DP_LC3_ON */
#endif /* BT_A2DP_SUPPORT */
