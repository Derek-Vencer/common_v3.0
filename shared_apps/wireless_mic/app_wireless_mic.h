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
#ifndef __APP_WIRELESS_MIC__
#define __APP_WIRELESS_MIC__

#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t wireless_dbg_level;

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "[APPM] "
#define LOG_DBG(num,str,...)   if (wireless_dbg_level) \
                                    TRACE(num,LOG_TAG"" str, ##__VA_ARGS__)
#define LOG_MSG(num,str,...)   TRACE(num,LOG_TAG"" str, ##__VA_ARGS__)
#define LOG_ERR(num,str,...)   TRACE(num,LOG_TAG"err:""" str, ##__VA_ARGS__)

#define APP_WIRELESS_MIC_MODE_NONE              (0)
#define APP_WIRELESS_MIC_MODE_WLAN              (1 << 0)
#define APP_WIRELESS_MIC_MODE_D2D               (1 << 1)
#define APP_WIRELESS_MIC_MODE_BT                (1 << 2)
#define APP_WIRELESS_MIC_MODE_BLE               (1 << 3)
#define APP_WIRELESS_MIC_MODE_UAC               (1 << 4)
#define APP_WIRELESS_MIC_MODE_EMMC              (1 << 5)
#define APP_WIRELESS_MIC_MODE_I2S               (1 << 6)

#define APP_WIRELESS_MIC_MODE_CLEAR             (1 << 14)
#define APP_WIRELESS_MIC_MODE_SET               (1 << 15)

#define APP_WIRELESS_MIC_AUDIO_LATENCY          (WIFI_AUDIO_LATENCY_MS * 1000)

int anc_usb_open(void);

void anc_usb_close(void);

void app_wireless_mic_init(void);

uint8_t app_wireless_mic_mode_get(void);

void app_wireless_mic_mode_set(uint8_t mode);

void app_wireless_mic_mode_clear(uint8_t mode);

bool app_wireless_mic_mode_wlan(void);

bool app_wireless_mic_mode_d2d(void);

bool app_wireless_mic_mode_bt(void);

bool app_wireless_mic_mode_ble(void);

bool app_wireless_mic_mode_uac(void);

bool app_wireless_mic_mode_emmc(void);

uint32_t app_wireless_mic_cmd_callback(unsigned char *buf, unsigned int len);

#ifdef __cplusplus
}
#endif

#endif
