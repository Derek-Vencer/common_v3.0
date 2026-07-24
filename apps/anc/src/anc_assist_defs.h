/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#ifndef __ANC_ASSIST_DEFS_H__
#define __ANC_ASSIST_DEFS_H__

#include "plat_types.h"
#include "anc_assist.h"

#ifdef ANC_ASSIST_16BIT
typedef int16_t         _PCM_T;
typedef int16_t anc_assist_pcm_t;
#define RIGHT_SHIFT (0)
#else
typedef int32_t         _PCM_T;
typedef float anc_assist_pcm_t;
#define RIGHT_SHIFT (8)
#endif

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif