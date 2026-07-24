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
#include "a2dp_codec_scalable.h"
#include "bta_bt_api.h"
#include "a2dp_api.h"

#if defined(A2DP_SCALABLE_ON)
btif_avdtp_codec_t a2dp_scalable_avdtpcodec;

//Vendor Specific value (8bit) : 0x78 (not support UHQ)  0xF8  (support UHQ)
// - bit 7 : 96kHz   sampling frequency supported
// - bit 6 : 32kHz   sampling frequency supported
// - bit 5 : 44.1kHz sampling frequency supported
// - bit 4 : 48kHz   sampling frequency supported
// - bit 3 : high quality supported 
// - bit 2 : additional information (current 0)
// - bit 1 : additional information (current 0)
// - bit 0 : additional information (current 0)
//<1byte Vendor Specific values for Scalable codec> 
const unsigned char a2dp_codec_scalable_elements[A2DP_SCALABLE_OCTET_NUMBER] =
{
    0x75,0x0,0x0,0x0, // vendor id
    0x03,0x01,          // vendor specific codec id
#if (defined(A2DP_SCALABLE_UHQ_SUPPORT) && defined(A2DP_SCALABLE_SSC_SUPPORT))
    0xf8,                 // vendor specific value
#elif defined(A2DP_SCALABLE_SSC_SUPPORT)
    0x3c,                 // vendor specific value
#else
    0x78,                // vendor specific value
#endif
};

void a2dp_codec_scalable_init(void)
{
    bta_a2dp_codec_init(BT_A2DP_CODEC_TYPE_NON_A2DP, 
            BT_A2DP_CODEC_NONE_TYPE_SCALABLE,
            BT_A2DP_CODEC_PRORITY_4,
            (uint8_t *)&a2dp_codec_scalable_elements,
            sizeof(a2dp_codec_scalable_elements));
}
#endif /* A2DP_SCALABLE_ON */
#endif /* BT_A2DP_SUPPORT */
