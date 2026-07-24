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
#ifndef _HFP_AUDIO_QUEUE_H
#define _HFP_AUDIO_QUEUE_H

#define AUDIO_HFP_DATA_QUEUE_BUFFER_SIZE                (AUDIO_DATAPATH_HFP_OUTPUT_FRAME_LEN * 2)

#ifdef __cplusplus
extern "C" {
#endif

int audio_hfp_data_queue_init(void);

int audio_hfp_data_enqueue(uint8_t* buff, uint16_t len);

int audio_hfp_data_dequeue(uint8_t* buff, uint16_t len);

int audio_hfp_data_queue_get_length(void);

int audio_hfp_data_discard(uint16_t len);

#ifdef __cplusplus
}
#endif

#endif