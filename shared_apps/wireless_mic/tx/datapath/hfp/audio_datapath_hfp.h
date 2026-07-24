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
#ifndef _AUDIO_DATAPATH_HFP_H_
#define _AUDIO_DATAPATH_HFP_H_

#define AUDIO_DATAPATH_HFP_OUTPUT_FRAME_LEN             (48 * 4 * 1 * 15) //48k 24bit 1ch 15ms

#ifdef __cplusplus
extern "C" {
#endif

void audio_datapath_hfp_init(void);

void audio_datapath_hfp_output_stream_onoff(bool onoff);

void audio_datapath_hfp_output_stream_process_shot(void);

void audio_datapath_hfp_output_stream_lock_sco_player(void);

void audio_datapath_hfp_output_stream_unlock_sco_player(void);

#ifdef __cplusplus
}
#endif

#endif
