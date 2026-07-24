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
#ifndef _USB_AUDIO_QUEUE_H
#define _USB_AUDIO_QUEUE_H

#define AUDIO_USB_DATA_QUEUE_MAX_MTU                (10)
#define AUDIO_USB_DATA_QUEUE_DATA_COST_MS           (USB_AUDIO_FRAME_MS * AUDIO_USB_DATA_QUEUE_MAX_MTU)
// #define AUDIO_USB_DATA_QUEUE_BUFFER_SIZE            (48 * 3 * WIFI_SESSION_NUM * AUDIO_USB_DATA_QUEUE_DATA_COST_MS)
#define AUDIO_USB_DATA_QUEUE_BUFFER_SIZE            (48 * 3 * 4 * AUDIO_USB_DATA_QUEUE_DATA_COST_MS)

#ifdef __cplusplus
extern "C" {
#endif

int audio_usb_data_queue_init(void);

int audio_usb_data_queue_reset(void);

int audio_usb_data_enqueue(uint8_t* buff, uint16_t len);

int audio_usb_data_dequeue(uint8_t* buff, uint16_t len);

int audio_usb_data_queue_get_length(void);

#ifdef __cplusplus
}
#endif

#endif