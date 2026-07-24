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
#ifndef __AUD_DATAPATH_USB_H__
#define __AUD_DATAPATH_USB_H__

#define AUDIO_USB_OUTPUT_STREAM_START                         (1 << 0)
#define AUDIO_USB_OUTPUT_STREAM_STOP                          (1 << 1)
#define AUDIO_USB_OUTPUT_STREAM_PROCESS                       (1 << 2)

void audio_datapath_usb_init(void);

uint64_t audio_datapath_usb_ref_tsf_get(void);

uint16_t audio_datapath_usb_irq_interval_get(void);

void audio_data_usb_capture_stream_data_input(uint8_t* buff, uint32_t len);

bool audio_data_usb_capture_stream_running(void);

uint8_t* audio_datapath_usb_capture_stream_channel_num_get(void);

bool audio_datapath_usb_capture_stream_channel_num_set(uint8_t ch_num);

void audio_datapath_usb_capture_stream_onoff(bool onoff);

void audio_usb_output_stream_start_shot(void);

void audio_usb_output_stream_stop_shot(void);

void audio_usb_output_stream_process_shot(void);

void audio_datapath_usb_capture_stream_check_dm_status(void);

void audio_datapath_usb_capture_stream_algo_on_cp_onoff_set(bool* onoff);

void audio_datapath_usb_capture_stream_algo_on_cp_cfg_set(void);

void audio_datapath_usb_capture_stream_algo_on_cp_channel_mode_set(uint8_t* mode);

void audio_datapath_usb_capture_stream_algo_on_cp_sw_gain_set(float* gain);

void audio_datapath_usb_capture_stream_algo_on_cp_mpdrc_onoff_set(bool* onoff);

#endif