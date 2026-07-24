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

#ifndef __GAF_OFFLOAD_H__
#define __GAF_OFFLOAD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_aud.h"
#include "lc3_process.h"
#include "gaf_stream_dbg.h"
#include "gaf_media_common.h"
/*****************************header include********************************/

/******************************macro defination*****************************/

/******************************struct defination******************************/

struct hci_iso_timestamp_ull_header_t {
    uint8_t iso_hci_type;
    uint16_t iso_conn_handle;       // conhdl 0~11 pb_flag 12~13 ts_flag 14 rfu 15
    uint16_t iso_data_load_length;  // length 0~13 rfu 14~15
    uint32_t iso_sdu_timestamp;
    uint16_t iso_sdu_sequence_num;
    uint16_t iso_sdu_length;        // sdu_length 0~11 rfu 12~13 ps_flag 14~15
} __attribute__ ((packed));

struct Codec_intr {
    uint32_t pcm_size;
    struct HAL_DMA_DESC_T *dma_desc;
    int16_t *chace_buf;
    uint8_t *src_buf;

    void *(*get_codec_ptr)(struct Codec_intr* ptr, struct HAL_DMA_DESC_T *lli);
    void (*regroup_data)(struct Codec_intr* ptr);
};

struct Iso_intr {
    uint16_t payload_len;
    void *enc_data;
    void *dma_buf;
    void *pcm_buf;
    bool isPLC;
    uint8_t cur_ch;
    uint8_t total_ch;
    void (*gaf_offload_decode_iso_run)(uint8_t cis_ch_id, struct Iso_intr iso_rx_ins);
};

typedef enum
{
    GAF_OFFLOAD_ENCODER_INIT_COMPLETED  = (1 << 0),
    GAF_OFFLOAD_DECODER_INIT_COMPLETED  = (1 << 1),
} GAF_OFFLOAD_DECODE_STATE_E;

struct lea_offload_config_t {
    bool gaf_offload_enable;
    enum AUD_CHANNEL_NUM_T lc3_ch_num;
    CODEC_INFO_T encodec_info;
    CODEC_INFO_T decodec_info;
    void (*codec_in_handler)(uint8_t chan, uint32_t remains, uint32_t error, struct HAL_DMA_DESC_T *lli);
    void (*codec_out_handler)(uint8_t chan, uint32_t remains, uint32_t error, struct HAL_DMA_DESC_T *lli);
    const GAF_DECODER_FUNC_LIST_T*  decoder_func_list;
    const GAF_ENCODER_FUNC_LIST_T*  encoder_func_list;
    uint8_t offload_capture_ch;
    uint8_t offload_playback_ch;
    uint8_t lc3_codec_complete;
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv_ptr;
    uint32_t iso_handler[BLE_CONNECTION_MAX];
};

struct hci_iso_timestamp_test_header_t {
    uint8_t iso_hci_type;
    uint16_t iso_conn_handle;       // conhdl 0~11 pb_flag 12~13 ts_flag 14 rfu 15
    uint16_t iso_data_load_length;  // length 0~13 rfu 14~15
    uint32_t iso_sdu_timestamp;
    uint16_t iso_sdu_sequence_num;
    uint16_t iso_sdu_length;        // sdu_length 0~11 rfu 12~13 ps_flag 14~15
} __attribute__ ((packed));

/****************************function declaration***************************/
uint8_t* get_lea_offload_cfg_param(void);
void gaf_offload_cp_init(void);

#ifdef __cplusplus
}
#endif

#endif /// __GAF_OFFLOAD_H__