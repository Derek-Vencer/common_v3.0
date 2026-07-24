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
#ifndef _GAF_STREAM_RESAMPLE_PROCESS_H_
#define _GAF_STREAM_RESAMPLE_PROCESS_H_

#include "plat_types.h"
#include "audioflinger.h"
#include "gaf_media_common.h"

#ifdef __cplusplus
extern "C" {
#endif

void gaf_stream_resample_process_init(void);

int32_t gaf_stream_resample_capture_up_init(GAF_AUDIO_STREAM_INFO_T *stream_info, struct AF_STREAM_CONFIG_T *stream_cfg);
int32_t gaf_stream_resample_capture_up_deinit(void);
uint32_t gaf_stream_resample_capture_up_run(uint8_t **buf, uint32_t len);

uint32_t gaf_stream_resample_need_dn_buf_size(void);
int32_t gaf_stream_resample_remap_dn_ptrbuf_length(uint8_t** ptrBuf, uint32_t *length);
uint8_t* gaf_stream_resample_get_dn_ptrbuf(void);

int32_t gaf_stream_resample_playback_dn_init(GAF_AUDIO_STREAM_INFO_T *stream_info, struct AF_STREAM_CONFIG_T *stream_cfg);
int32_t gaf_stream_resample_playback_dn_deinit(void);
uint32_t gaf_stream_resample_playback_dn_run(uint8_t *buf, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif // _GAF_STREAM_RESAMPLE_PROCESS_H_
