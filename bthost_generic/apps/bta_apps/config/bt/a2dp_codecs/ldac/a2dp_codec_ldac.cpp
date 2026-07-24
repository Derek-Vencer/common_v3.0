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

#include "a2dp_codec_ldac.h"
#include "bta_bt_api.h"

#if defined(A2DP_LDAC_ON)

const unsigned char a2dp_codec_ldac_elements[A2DP_LDAC_OCTET_NUMBER] =
{
    0x2d, 0x01, 0x00, 0x00, //Vendor ID
    0xaa, 0x00,     //Codec ID
    (A2DP_LDAC_SR_96000|A2DP_LDAC_SR_88200|A2DP_LDAC_SR_48000 |A2DP_LDAC_SR_44100),
//    (A2DP_LDAC_SR_48000 |A2DP_LDAC_SR_44100),
    (A2DP_LDAC_CM_MONO|A2DP_LDAC_CM_DUAL|A2DP_LDAC_CM_STEREO),
};

void a2dp_codec_ldac_init(void)
{
    bta_a2dp_codec_init(BT_A2DP_CODEC_TYPE_NON_A2DP, 
            BT_A2DP_CODEC_NONE_TYPE_LDAC,
            BT_A2DP_CODEC_PRORITY_3,
            (uint8_t *)&a2dp_codec_ldac_elements,
            sizeof(a2dp_codec_ldac_elements));
}
#endif /* A2DP_LDAC_ON */
#endif /* BT_A2DP_SUPPORT */
