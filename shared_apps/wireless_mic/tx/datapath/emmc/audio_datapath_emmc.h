/***************************************************************************
 *
 * Copyright 2015-2024 BES.
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
#ifndef _AUD_DATAPATH_EMMC_H_
#define _AUD_DATAPATH_EMMC_H_

#ifdef __cplusplus
extern "C" {
#endif

enum MIC_DATA_DUMP_PORT_T {
    MIC_DATA_DUMP_PORT_ORIG = 0,
    MIC_DATA_DUMP_PORT_ALGO,
    MIC_DATA_DUMP_PORT_ALGO_32BIT,
    MIC_DATA_DUMP_PORT_NUM,
    MIC_DATA_DUMP_START,
    MIC_DATA_DUMP_STOP,
};

typedef struct {
    uint16_t    dump_port_id;
    uint16_t    mic_data_len;
    uint8_t*    mic_data_ptr;
    int32_t     mic_resample_delta;
} AUDIO_DATAPATH_EMMC_MESSAGE_T;

#ifdef AOS_FS_ENABLE
typedef struct {
    uint8_t file_patch[512];
    uint8_t frist_flag;
    time_t time;
} Emmc_file_status_t;
#endif

void audio_datapath_emmc_dump_mode_set(uint8_t mode);

void audio_datapath_emmc_init(void);

void audio_datapath_emmc_deinit(void);

void audio_datapath_emmc_output_stream_onoff(bool onoff);

#ifdef __cplusplus
}
#endif

#endif