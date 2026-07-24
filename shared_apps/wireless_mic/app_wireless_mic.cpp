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
#ifdef APP_WIRELESS_MIC
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cmsis.h"
#include "cmsis_os.h"
#include "hal_trace.h"
#include "plat_types.h"
#include "app_wireless_mic.h"
#ifdef UTILS_ESHELL_EN
#include "eshell.h"
#endif

#include "apps.h"
#include "app_bt.h"
#include "app_utils.h"
#include "audio_datapath_manager.h"
#include "audio_datapath_ble.h"
#ifdef AUD_OUTPUT_BY_EMMC
#include "audio_datapath_emmc.h"
#endif
#ifdef AUD_OUTPUT_BY_USB
#include "usbaudio_thread.h"
#include "usb_audio_app.h"
#endif
// #ifdef AUD_OUTPUT_BY_HFP
#include "earbud_ux_api.h"
// #endif
#ifdef APP_TRACE_RX_ENABLE
#include "app_trace_rx.h"
#endif

static uint8_t wireless_mic_mode = APP_WIRELESS_MIC_MODE_NONE;

static osThreadId app_mode_ctrl_tid = NULL;
static void app_mode_ctrl_thread(void* argument);
static const osThreadAttr_t app_mode_ctrl_thread_attr = {
    .name       = "app_mode_ctrl",
    .stack_size = (3 * 1024),
    .priority   = osPriorityAboveNormal,
};

void app_wireless_mic_mode_control_init(void)
{
    if (app_mode_ctrl_tid == NULL) {
        app_mode_ctrl_tid = osThreadNew(app_mode_ctrl_thread, NULL, &app_mode_ctrl_thread_attr);
        ASSERT(app_mode_ctrl_tid, "create app mode control thread failed.");
    }
}

#if defined(AUD_OUTPUT_BY_HFP) && defined(IBRT)
bool app_wireless_mic_disallow_start_reconnect_mobile_cb(const bt_bdaddr_t *addr, const uint16_t active_event)
{
    if (app_wireless_mic_mode_bt() == false) {
        LOG_MSG(1, "not in bt mode, disallow reconnect mb.");
        return true;
    }
    return false;
}

ibrt_ext_conn_policy_cb_t app_wireless_mic_ext_conn_policy_client_cb = {
    .disallow_start_reconnect_mob_hook = app_wireless_mic_disallow_start_reconnect_mobile_cb,
};
#endif

void app_wireless_mic_init(void)
{
    LOG_MSG(1, "%s", __func__);

    app_wireless_mic_mode_control_init();

    audio_datapath_manager_init();

#ifdef WIRELESS_MIC_TX
#if defined(AUD_OUTPUT_BY_HFP) && defined(IBRT)
    app_ibrt_if_register_ext_conn_policy_client_callback(&app_wireless_mic_ext_conn_policy_client_cb);
#endif

    app_wireless_mic_mode_set(APP_WIRELESS_MIC_MODE_BLE);
#endif

#ifdef APP_TRACE_RX_ENABLE
    app_trace_rx_register("wmic", (APP_TRACE_RX_CALLBACK_T)app_wireless_mic_cmd_callback);
#endif
}

void app_wireless_mic_deinit(void)
{

}

void app_wireless_mic_ui_bt_disable(void)
{
    LOG_MSG(1, "enter %s", __func__);

#if defined(AUD_OUTPUT_BY_HFP) && defined(IBRT)
    app_ibrt_if_event_entry(APP_UI_EV_CASE_CLOSE);

    app_ibrt_if_disable_bluetooth();
    // app_bt_set_access_mode(BTIF_BAM_NOT_ACCESSIBLE);

    osDelay(800); //box dedounce timer 600ms
#endif
}

void app_wireless_mic_ui_bt_enable(void)
{
    LOG_MSG(1, "enter %s", __func__);

#if defined(AUD_OUTPUT_BY_HFP) && defined(IBRT)
    app_ibrt_if_enable_bluetooth();

    // app_ibrt_if_enter_freeman_pairing(); // can not reconnect mobile
    app_ibrt_if_event_entry(APP_UI_EV_CASE_OPEN);

    bt_bdaddr_t mobile_addr1, mobile_addr2;
    if (app_ibrt_if_nvrecord_get_latest_mobiles_addr(&mobile_addr1, &mobile_addr2) == 0) {
        app_ibrt_if_event_entry(APP_UI_EV_FREE_MAN_MODE);
    }
#endif
}

void app_wireless_mic_ui_ble_disable(void)
{
    LOG_MSG(1, "enter %s", __func__);

#if defined(AUD_OUTPUT_BY_BLE)
    audio_datapath_ble_disconnect_all();

    audio_datapath_ble_stop_adv();

    app_ibrt_if_event_entry(APP_UI_EV_CASE_CLOSE);

    osDelay(800); //box dedounce timer 600ms
#endif
}

void app_wireless_mic_ui_ble_enable(void)
{
    LOG_MSG(1, "enter %s", __func__);

#if defined(AUD_OUTPUT_BY_BLE)
    app_ibrt_if_event_entry(APP_UI_EV_CASE_OPEN);

    audio_datapath_ble_start_adv();
#endif
}

void app_wireless_mic_ui_usb_enable(void)
{

}

void app_wireless_mic_ui_usb_disable(void)
{

}

void app_wireless_mic_ui_emmc_enable(void)
{

}

void app_wireless_mic_ui_emmc_disable(void)
{

}

static void app_mode_clear(uint8_t mode)
{
    if (mode && !(mode & wireless_mic_mode)) {
        LOG_MSG(1, "%s: mode=0x%02x has not been set.", __func__, mode);
        return ;
    }
    if (mode == APP_WIRELESS_MIC_MODE_NONE) {
        mode = wireless_mic_mode;
    }

    mode = mode & wireless_mic_mode;
    wireless_mic_mode = wireless_mic_mode & (~mode);
    LOG_MSG(2, "%s: current mode is=0x%02x", __func__, wireless_mic_mode);

    if (mode & APP_WIRELESS_MIC_MODE_BLE) {
        app_wireless_mic_ui_ble_disable();
    }
    if (mode & APP_WIRELESS_MIC_MODE_BT) {
        app_wireless_mic_ui_bt_disable();
    }
    if (mode & APP_WIRELESS_MIC_MODE_UAC) {
        app_wireless_mic_ui_usb_disable();
    }
    if (mode & APP_WIRELESS_MIC_MODE_EMMC) {
        app_wireless_mic_ui_emmc_disable();
    }
}

static void app_mode_set(uint8_t mode)
{
    if (mode & wireless_mic_mode) {
        LOG_MSG(1, "%s: mode=0x%02x already exist.", __func__, mode);
        return ;
    }
    if (mode == APP_WIRELESS_MIC_MODE_NONE) {
        app_mode_clear(mode);
        return ;
    }

    if ((mode & 0x0F) && (wireless_mic_mode & 0x0F)) {
        app_mode_clear(wireless_mic_mode & 0x0F);
    }

    wireless_mic_mode = wireless_mic_mode | mode;
    LOG_MSG(2, "%s: current mode is=0x%02x", __func__, wireless_mic_mode);

    if (mode & APP_WIRELESS_MIC_MODE_BLE) {
        app_wireless_mic_ui_ble_enable();
    }
    if (mode & APP_WIRELESS_MIC_MODE_BT) {
        app_wireless_mic_ui_bt_enable();
    }
    if (mode & APP_WIRELESS_MIC_MODE_UAC) {
        app_wireless_mic_ui_usb_enable();
    }
    if (mode & APP_WIRELESS_MIC_MODE_EMMC) {
        app_wireless_mic_ui_emmc_enable();
    }
}

uint8_t app_wireless_mic_mode_get(void)
{
    return wireless_mic_mode;
}

void app_wireless_mic_mode_clear(uint8_t mode)
{
    osSignalSet(app_mode_ctrl_tid, mode | APP_WIRELESS_MIC_MODE_CLEAR);
}

void app_wireless_mic_mode_set(uint8_t mode)
{
    osSignalSet(app_mode_ctrl_tid, mode | APP_WIRELESS_MIC_MODE_SET);
}

static void app_mode_ctrl_thread(void* argument)
{
    osEvent evt;
    uint32_t signals = 0;

    while (1) {
        evt = osSignalWait(0x0, osWaitForever);
        signals = evt.value.signals;
        if (evt.status == osEventSignal) {
            if (signals & APP_WIRELESS_MIC_MODE_CLEAR) {
                app_mode_clear(signals);
            }
            if (signals & APP_WIRELESS_MIC_MODE_SET) {
                app_mode_set(signals);
            }
        }
    }
}

bool app_wireless_mic_mode_bt(void)
{
    return !!(wireless_mic_mode & APP_WIRELESS_MIC_MODE_BT);
}

bool app_wireless_mic_mode_ble(void)
{
    return !!(wireless_mic_mode & APP_WIRELESS_MIC_MODE_BLE);
}

bool app_wireless_mic_mode_uac(void)
{
    return !!(wireless_mic_mode & APP_WIRELESS_MIC_MODE_UAC);
}

bool app_wireless_mic_mode_emmc(void)
{
    return !!(wireless_mic_mode & APP_WIRELESS_MIC_MODE_EMMC);
}
#endif
