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
#ifdef SWIFT_ENABLED // MSFT bluetooth swift pair

#include "cmsis_os2.h"

#include "bta_ble_api.h"
#include "bta_bt_api.h"
#include "bta_tws_ux_api.h"

#include "app_ble_swift.h"

#include "hal_trace.h"
#include "bt_drv_reg_op.h"
#include "string.h"

#define SWIFT_PAIR_MODE_LE_ONLY         (0)
#define SWIFT_PAIR_MODE_LE_AND_BT       (1)
#define SWIFT_PAIR_MODE_BT              (2)
#define SWIFT_PAIR_MODE                 SWIFT_PAIR_MODE_BT

#define SWIFT_MS_HEADER_LEN             (6)
#define SWIFT_MS_VENDER_ID              (0xff0600)
#define SWIFT_MS_BEACON                 (0x03)
#if (SWIFT_PAIR_MODE == SWIFT_PAIR_MODE_LE_ONLY)
#define SWIFT_MS_BEACON_SUB             (0)
#define SWIFT_ADV_DISPLAY_ICON          (0)
#define SWIFT_ADV_BT_ADDR               (0)
#elif (SWIFT_PAIR_MODE == SWIFT_PAIR_MODE_LE_AND_BT)
#define SWIFT_MS_BEACON_SUB             (0x02)
#define SWIFT_ADV_DISPLAY_ICON          (1)
#define SWIFT_ADV_BT_ADDR               (0)
#else
#define SWIFT_MS_BEACON_SUB             (0x01)
#define SWIFT_ADV_DISPLAY_ICON          (1)
#define SWIFT_ADV_BT_ADDR               (1)
#endif
#define SWIFT_RSSI                      (0x80)
#define SWIFT_DISPLAY_ICON_LEN          (3)
#define SWIFT_DISPLAY_ICON              (0x040424)

#define BLE_SWIFT_ADVERTISING_INTERVAL  (100)

// adv mode should be ADV_MODE_EXTENDED or ADV_MODE_LEGACY
#define SWIFT_ADV_MODE_LEGACY           (0)
#define SWIFT_ADV_MODE_EXTEND           (1)
#define SWIFT_ADV_MODE                  SWIFT_ADV_MODE_LEGACY

#define SWIFT_ADV_SERVICE_DATA_MAX_LEN  (68)

static bool enable_swift = false;

static bool app_swift_adv_param_prepare(bt_ble_gap_cus_adv_param_t *adv_param)
{
    uint8_t *data_ptr = (uint8_t *)(adv_param->adv_data);
    uint8_t *data_start = data_ptr;
    uint8_t *length_ptr = NULL;
    uint8_t max_len = 0;
    int left_len = 0;

    if (!enable_swift)
    {
        return false;
    }

#if (BLE_LEGACY_APP_ENABLED)
    // NV Slave only can start swift when tws is disconnected
    if (BT_IBRT_SLAVE == bta_tws_get_nv_role() &&
            bta_tws_get_sync_state() != BTA_TWS_DISCONNECTED)
    {
        return false;
    }
#else
    // UI role slave can not start swift adv
    if (BT_IBRT_SLAVE == bta_tws_get_ui_role())
    {
        return false;
    }
#endif /* BLE_LEGACY_APP_ENABLED */

    adv_param->adv_type = BT_GAP_ADV_TYPE_UNDIRECT;

#if (SWIFT_ADV_MODE == SWIFT_ADV_MODE_EXTEND)
    adv_param->adv_mode = BT_GAP_ADV_MODE_EXTENDED;
#else
    adv_param->adv_mode = BT_GAP_ADV_MODE_LEGACY;
#endif

    adv_param->own_addr_type = BT_BLE_GAP_ADV_RPA;
    adv_param->tx_power_dbm = btdrv_reg_op_txpwr_idx_to_rssidbm(4);

    data_ptr[0] = 0x02;
    data_ptr[1] = 0x01;
    data_ptr[2] = 0x0a; // BR + LE + Discovrable
    data_ptr += 3;

    // reserve for length
    length_ptr = data_ptr;
    data_ptr += 1;

    data_ptr[0] = (SWIFT_MS_VENDER_ID >> 16) & 0xFF;
    data_ptr[1] = (SWIFT_MS_VENDER_ID >> 8) & 0xFF;
    data_ptr[2] = SWIFT_MS_VENDER_ID  & 0xFF;
    data_ptr[3] = SWIFT_MS_BEACON;
    data_ptr[4] = SWIFT_MS_BEACON_SUB;
    data_ptr[5] = SWIFT_RSSI;
    data_ptr += 6;

#if SWIFT_ADV_BT_ADDR
    bta_bt_get_local_addr((bt_bdaddr_t *)data_ptr);
    data_ptr += sizeof(bt_bdaddr_t);
#endif

#if SWIFT_ADV_DISPLAY_ICON
    data_ptr[0] = (SWIFT_DISPLAY_ICON >> 16) & 0xFF;
    data_ptr[1] = (SWIFT_DISPLAY_ICON >> 8) & 0xFF;
    data_ptr[2] = SWIFT_DISPLAY_ICON  & 0xFF;
    data_ptr += 3;
#endif

    max_len = (BT_GAP_ADV_MODE_LEGACY == adv_param->adv_mode) ? 31 : SWIFT_ADV_SERVICE_DATA_MAX_LEN;
    left_len = max_len - (data_ptr - data_start);

    bta_ble_get_local_name((char *)data_ptr, left_len);
    data_ptr += strlen((char *)data_ptr) + 1; // +1 for '\0'

    *length_ptr = data_ptr - length_ptr - 1; // -1 for length_ptr itself

    adv_param->adv_data_size = data_ptr - data_start;

    return true;
}

void app_ble_swift_init(void)
{
    enable_swift = false;
}

void app_ble_swift_deinit(void)
{
    enable_swift = false;
    bta_ble_custom_adv_stop(BT_BLE_GAP_ADV_ACTIVITY_USER_0);
}

void app_ble_swift_adv_refresh(void)
{
    uint8_t adv_data[SWIFT_ADV_SERVICE_DATA_MAX_LEN] = {0};
    bt_ble_gap_cus_adv_param_t adv_param;

    memset(&adv_param, 0, sizeof(adv_param));

    adv_param.actv_user = BT_BLE_GAP_ADV_ACTIVITY_USER_0;
    adv_param.adv_user = BT_BLE_GAP_ADV_USER_SWIFT;
    adv_param.adv_data = adv_data;

    if (app_swift_adv_param_prepare(&adv_param) == true)
    {
        bta_ble_custom_adv_write_info(&adv_param);
        bta_ble_custom_adv_start(BT_BLE_GAP_ADV_ACTIVITY_USER_0);
    }
    else
    {
        bta_ble_custom_adv_stop(BT_BLE_GAP_ADV_ACTIVITY_USER_0);
    }
}

void app_swift_enter_pairing_mode(void)
{
    enable_swift = true;

    app_ble_swift_adv_refresh();
}

void app_swift_exit_pairing_mode(void)
{
    enable_swift = false;

    bta_ble_custom_adv_stop(BT_BLE_GAP_ADV_ACTIVITY_USER_0);
}

#endif /* SWIFT_ENABLED */
