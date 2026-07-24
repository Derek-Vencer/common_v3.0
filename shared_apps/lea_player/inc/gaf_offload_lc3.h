/***************************************************************************
 *
 * Copyright 2015-2023 BES.
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

#ifndef __GAF_OFFLOAD_LC3_H__
#define __GAF_OFFLOAD_LC3_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*GAF_AUDIO_ENCODER_DIRECT_SEND_FRAME_FUNC)(const uint8_t *payload, uint16_t payload_size);

void* bes_heap_init_lc3(uint8_t* app_registered_heap, uint32_t heap_size);

void* bes_heap_malloc_lc3(int32_t length);

void bes_heap_free_lc3(void* ptr);

const GAF_DECODER_FUNC_LIST_T* gaf_offload_lc3_decoder_funcs();

const GAF_ENCODER_FUNC_LIST_T* gaf_offload_lc3_encoder_funcs();

#ifdef __cplusplus
}
#endif

#endif /// __GAF_OFFLOAD_H__
