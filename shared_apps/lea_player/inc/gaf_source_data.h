
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
#ifndef __GAF_SOURCE_DATA_H__
#define __GAF_SOURCE_DATA_H__

#ifdef __cplusplus
extern "C" {
#endif
/*****************************header include********************************/

/******************************macro defination*****************************/

/******************************type defination******************************/

/****************************function declaration***************************/
void gaf_source_playback_start(void *pStreamEnv);

void gaf_source_capture_start(void *pStreamEnv);

void gaf_source_stop(uint32_t stream);

void gaf_source_stream_data_tx_read(uint8_t* data, uint32_t len);

void gaf_source_stream_data_tx_write(uint8_t* data, uint32_t len, uint8_t bits_byte);

void gaf_source_stream_data_rx_read(uint8_t* data, uint32_t len);

#ifdef __cplusplus
}
#endif
#endif /* __APP_USB_HW_TIMER_H__ */
