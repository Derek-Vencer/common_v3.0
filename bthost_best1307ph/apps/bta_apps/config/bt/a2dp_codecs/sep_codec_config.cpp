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
#include "app_a2dp.h"
#include "a2dp_codec_sbc.h"
#include "a2dp_codec_aac.h"
#include "a2dp_codec_ldac.h"
#include "a2dp_codec_lhdc.h"
#include "a2dp_codec_lhdcv5.h"
#include "a2dp_codec_lc3.h"
#include "a2dp_codec_scalable.h"
#include "a2dp_codec_opus.h"
#include "a2dp_codec_l2hc.h"
#include "a2dp_codec_mihc.h"

#ifdef BT_A2DP_SUPPORT

extern "C" void bta_sep_codec_config_init(void)
{
    a2dp_codec_sbc_init();

#if defined(A2DP_AAC_ON)
    a2dp_codec_aac_init();
#endif

#if defined(A2DP_LDAC_ON)
    a2dp_codec_ldac_init();
#endif

#if defined(A2DP_LHDC_ON)
    a2dp_codec_lhdc_init();
#endif

#if defined(A2DP_LHDCV5_ON)
    a2dp_codec_lhdcv5_init();
#endif

#if defined(MASTER_USE_OPUS) || defined(ALL_USE_OPUS)
    a2dp_codec_opus_init();
#endif

#if defined(A2DP_SCALABLE_ON)
    a2dp_codec_scalable_init();
#endif

#if defined(A2DP_LC3_ON)
    a2dp_codec_lc3_init();
#endif

#if defined(A2DP_L2HC_ON)
    a2dp_codec_l2hc_init();
#endif

#if defined(A2DP_MIHC_ON)
    a2dp_codec_mihc_init();
#endif
}

// static apps_a2dp_sink_interface_t g_a2dp_sink_interface =
// {
//     .codec_init = bta_apps_a2dp_codec_init_cb,
// };

// void bta_sep_codec_config_init(void)
// {
//     app_a2dp_register_bta_sink_interface(&g_a2dp_sink_interface);
// }
#endif
