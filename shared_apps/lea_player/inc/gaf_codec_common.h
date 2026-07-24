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

#ifndef __GAF_CODEC_COMMMON_H__
#define __GAF_CODEC_COMMMON_H__

#ifdef __cplusplus
extern "C"{
#endif
/*****************************header include********************************/

/******************************macro defination*****************************/
#define GAF_CODEC_BUF_STATE_FREE        0x46524545
#define GAF_CODEC_BUF_STATE_USED        0x55534544

/******************************struct defination******************************/
typedef void *(*CODEC_ALLOC)(void *pool, uint32_t size);
typedef void (*CODEC_FREE)(void *pool, void *ptr);
typedef void (*GAF_DECODER_INIT_BUFFER)(uint8_t instance_handle, void *codec_info, CODEC_ALLOC alloc_cb, CODEC_FREE free_cb);
typedef void (*GAF_DECODER_INIT_FUNC)(uint8_t instance_handle, void *codec_info);
typedef void (*GAF_DECODER_DEINIT_FUNC)(uint8_t instance_handle);
typedef int (*GAF_DECODER_DECODE_FRAME_FUNC)(uint8_t instance_handle, void *codec_info,
            uint32_t inputDataLength, void *input, void *output, bool isPlc);
typedef void (*GAF_DECODER_DEINIT_BUFFER)(uint8_t instance_handle);
typedef uint32_t (*GAF_DECODER_SET_FREQ)(void *codec_info, uint32_t base_freq, uint32_t core_type);

typedef void (*GAF_ENCODER_INIT_BUFFER)(uint8_t instance_handle, void *codec_info, CODEC_ALLOC alloc_cb, CODEC_FREE free_cb);
typedef void (*GAF_ENCODER_INIT_FUNC)(uint8_t instance_handle, void *codec_info);
typedef void (*GAF_ENCODER_DEINIT_FUNC)(uint8_t instance_handle);
typedef int (*GAF_ENCODER_ENCODE_FRAME_FUNC)(uint8_t instance_handle, void *codec_info,
            uint32_t inputDataLength, void *input, uint16_t frame_size, uint8_t *output);
typedef void (*GAF_ENCODER_DEINIT_BUFFER)(uint8_t instance_handle);
typedef uint32_t (*GAF_ENCODER_SET_FREQ)(void *codec_info, uint32_t base_freq, uint32_t core_type);

typedef struct
{
    GAF_DECODER_INIT_BUFFER   decoder_init_buf_func;
    GAF_DECODER_INIT_FUNC     decoder_init_func;
    GAF_DECODER_DEINIT_FUNC   decoder_deinit_func;
    GAF_DECODER_DECODE_FRAME_FUNC decoder_decode_frame_func;
    GAF_DECODER_DEINIT_BUFFER   decoder_deinit_buf_func;
    GAF_DECODER_SET_FREQ      decoder_set_freq;
} GAF_DECODER_FUNC_LIST_T;

typedef struct
{
    GAF_ENCODER_INIT_BUFFER   encoder_init_buf_func;
    GAF_ENCODER_INIT_FUNC     encoder_init_func;
    GAF_ENCODER_DEINIT_FUNC   encoder_deinit_func;
    GAF_ENCODER_ENCODE_FRAME_FUNC encoder_encode_frame_func;
    GAF_ENCODER_DEINIT_BUFFER   encoder_deinit_buf_func;
    GAF_ENCODER_SET_FREQ      encoder_set_freq;
} GAF_ENCODER_FUNC_LIST_T;

#pragma pack(1)
typedef struct
{
    uint32_t sample_rate;
    uint8_t num_channels;
    uint8_t bits_depth;
    float frame_ms;
    uint16_t frame_size;
    uint32_t pcm_size;
} CODEC_INFO_T;
#pragma pack()

enum
{
    CODEC_OK = 0,
    CODEC_ERR = 1,
};

typedef enum
{
    LC3 = 0,
    LC3PLUS = 1,
    GSBC = 2,
    LHDC = 3,
    OPUS = 4,
    PCM = 5,
    ULL = 6,
} CODEC_TYPE_E;

typedef struct
{
    uint32_t start_time_in_ms;
    uint32_t total_time_in_us;
    bool codec_started;
    uint32_t codec_mips;
} aob_codec_time_info_t;

typedef struct
{
    uint8_t channels;
    uint32_t sample_rate;
    uint32_t bit_rate;
    uint8_t frame_ms;
    uint32_t freq;
} codec_freq_setting_t;

typedef struct
{
    bool is_encoded;
    int32_t sample_rate;
    int32_t frame_dms;
    int32_t channels;
} lc3_buf_param;

typedef struct {
    uint32_t size;
    uint32_t state;
} __attribute__((aligned(4))) GAF_CODEC_BUF_ENTRY_T;

static inline bool gaf_codec_buf_entry_is_free(const GAF_CODEC_BUF_ENTRY_T *e)
{
    return e->state == GAF_CODEC_BUF_STATE_FREE;
}

static inline bool gaf_codec_buf_entry_is_used(const GAF_CODEC_BUF_ENTRY_T *e)
{
    return e->state == GAF_CODEC_BUF_STATE_USED;
}

static inline bool gaf_codec_buf_entry_is_valid(const GAF_CODEC_BUF_ENTRY_T *e)
{
    return e->state == GAF_CODEC_BUF_STATE_FREE || e->state == GAF_CODEC_BUF_STATE_USED;
}

static inline uint8_t *gaf_codec_buf_entry_to_data(GAF_CODEC_BUF_ENTRY_T *e)
{
    return (uint8_t *)(e + 1);
}

static inline GAF_CODEC_BUF_ENTRY_T *gaf_codec_buf_data_to_entry(void *e)
{
    return (GAF_CODEC_BUF_ENTRY_T *)e - 1;
}

/****************************function declaration***************************/

/**
 * @brief Allocate a codec buffer, reusing a previously freed one when possible.
 *
 * Internally embeds a GAF_CODEC_BUF_ENTRY_T header before the data region.
 * On reuse, returns a previously freed buffer whose size >= @p size
 * (best-fit). On miss, calls @p alloc_cb to allocate
 * (sizeof(GAF_CODEC_BUF_ENTRY_T) + size) bytes from @p pool.
 *
 * @param pool      Platform-specific memory pool handle, forwarded to @p alloc_cb.
 * @param size      Required data size in bytes (before header).
 * @param alloc_cb  Callback that performs the actual raw memory allocation.
 * @return Pointer to the usable data region (after the embedded header).
 */
void *gaf_codec_buf_alloc_with_reuse(void *pool, uint32_t size, CODEC_ALLOC alloc_cb);

/**
 * @brief Mark a codec buffer as free for reuse.
 *
 * Does NOT actually free the underlying memory. The raw buffer (including
 * the embedded entry header) is retained so that a subsequent call to
 * gaf_codec_buf_alloc_with_reuse() can match and reuse it, avoiding
 * repeated alloc/free overhead across M33/M55/CP cores.
 * Memory is truly reclaimed only when the syspool itself is deinitialized.
 *
 * @param pool  Platform-specific memory pool handle (unused, kept for API symmetry).
 * @param data  Pointer previously returned by gaf_codec_buf_alloc_with_reuse().
 */
void gaf_codec_buf_free(void *pool, void *data);

/**
 * @brief Reset all codec buffer entry slots to initial (empty) state.
 *
 * Must be called before the first use of gaf_codec_buf_alloc_with_reuse(),
 * typically during codec stream initialization.
 */
void gaf_codec_buf_entry_init(void);

#ifdef __cplusplus
}
#endif

#endif /// __GAF_CODEC_COMMMON_H__