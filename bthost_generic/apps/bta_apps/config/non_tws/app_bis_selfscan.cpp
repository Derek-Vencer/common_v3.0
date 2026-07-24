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

#include "cmsis_os.h"
#include <string.h>

#include "bta_ble_api.h"

#include "bt_le_types.h"

#include "app_bis_selfscan.h"
#include "app_datapaths.h"
#include "app_ble_mgr.h"

#include "hal_trace.h"
#include "hal_timer.h"

#ifndef TRACE
#define TRACE(attr, str, ...)               TR_INFO(attr, str, ##__VA_ARGS__)
#endif

#ifdef BIS_SELFSCAN_ENABLED
/*DEFINITIONS*/

#define BISSELFSCAN_DFT_CON_LID                         (0)
#define BISSELFSCAN_SYNC_SCAN_OFFSET_10MS               (50)
#define BISSELFSCAN_AD_TYPE_ADV_BCAST_NAME              (0x30)
#define BISSELFSCAN_AD_TYPE_SERVICE_16_BIT_DATA         (0x16)
#define BISSELFSCAN_DYN_MEM_MAX_LEN                     (100)
#define BISSELSCAN_SELECT_CHAN_LOC_BF                   (0x03)
#define BISSELSCAN_GATT_UUID_PBA_SERVICE                (0x1856) // Public Broadcast Announcement
#define BISSELSCAN_INVALID_PA_SYNC_HDL                  (0xFFFF)

uint8_t empty_bcast_id[APP_BIS_SELFSCAN_BCAST_ID_LEN] = {0};
uint8_t empty_bcast_code[BT_BLE_GAP_KEY_LEN] = {0};

static const app_bis_selfscan_init_cfg_t init_cfg_default = {10, 10, 10, 3};

#define APP_BIS_SELFSCAN_SET_SCAN_STATE(_start)     do { p_app_bis_selfscan_env->state &= ~APP_BIS_SELFSCAN_STATE_SCANNING_BIT;\
                                                         p_app_bis_selfscan_env->state |= (_start ? APP_BIS_SELFSCAN_STATE_SCANNING_BIT : 0); } while (0);
#define APP_BIS_SELFSCAN_SET_SYNC_STATE(_state)     do { p_app_bis_selfscan_env->state &= ~APP_BIS_SELFSCAN_STATE_SYN_MASK;\
                                                         p_app_bis_selfscan_env->state |= (_state & APP_BIS_SELFSCAN_STATE_SYN_MASK); } while (0);
#define APP_BIS_SELFSCAN_SET_SINK_STATE(_state)     do { p_app_bis_selfscan_env->state &= ~APP_BIS_SELFSCAN_STATE_SINK_MASK;\
                                                         p_app_bis_selfscan_env->state |= (_state & APP_BIS_SELFSCAN_STATE_SINK_MASK); } while (0);
#define APP_BIS_SELFSCAN_GET_SCAN_STATE()           ((p_app_bis_selfscan_env->state & APP_BIS_SELFSCAN_STATE_SCANNING_BIT) != 0)
#define APP_BIS_SELFSCAN_GET_SYNC_STATE()           (p_app_bis_selfscan_env->state & APP_BIS_SELFSCAN_STATE_SYN_MASK)
#define APP_BIS_SELFSCAN_GET_SINK_STATE()           (p_app_bis_selfscan_env->state & APP_BIS_SELFSCAN_STATE_SINK_MASK)
#define APP_BIS_SELFSCAN_GET_LEFT_TIMER(curr_t_ms)  (((int)(p_app_bis_selfscan_env->scan_end_to_ms - (curr_t_ms)) > 0) ? \
                                                        (p_app_bis_selfscan_env->scan_end_to_ms - (curr_t_ms)) : 0)

static void app_bis_selfscan_scan_timer_handler(void const *param);
osTimerDef(APP_BIS_SELFSCAN_SCAN_TIMER, app_bis_selfscan_scan_timer_handler);

static void app_bis_selfscan_resume_latest_sync(uint8_t device_id, uint32_t param);

/*ENUMERATIONS*/

enum app_bis_selfscan_state
{
    /// Indicate scan state
    APP_BIS_SELFSCAN_STATE_SCANNING_BIT = 0x01,
    /// Indicate sync state idle
    APP_BIS_SELFSCAN_STATE_SYNC_IDLE    = 0b000,
    /// Indicate sync state ing
    APP_BIS_SELFSCAN_STATE_SYNCING      = 0b010,
    /// Indicate sync state ing
    APP_BIS_SELFSCAN_STATE_SYNCED       = 0b100,
    /// Indicate sync state
    APP_BIS_SELFSCAN_STATE_SYN_MASK     = 0b110,
    /// Indicate sink idle
    APP_BIS_SELFSCAN_STATE_SINK_IDLE    = 0b00000,
    /// Indicate sink success
    APP_BIS_SELFSCAN_STATE_SINKING      = 0b01000,
    /// Indicate sink success
    APP_BIS_SELFSCAN_STATE_SINKED       = 0b10000,
    /// Indicate sink state
    APP_BIS_SELFSCAN_STATE_SINK_MASK    = 0b11000,
};

enum app_bis_selfscan_scan_user
{
    /// Scan is started by app
    APP_BIS_SELFSCAN_SCAN_USER_APP = 0x01,
    /// Scan is started by sync cmd
    APP_BIS_SELFSCAN_SCAN_USER_SYNC = 0x02,
};

typedef struct app_bis_selfscan_src
{
    // Scan result
    struct app_bis_src_scan_result scan_result;
} app_bis_selfscan_src_t;

struct app_bis_selfscan_env
{
    /// Module callbacks
    app_bis_selfscan_evt_cb_t evt_cb;
    /// Maximum scan result pool size
    uint8_t max_result_present;
    /// Scan sync state
    uint8_t state;
    /// Scan user bitfield
    uint8_t scan_user_bf;
    /// Scan timer ID
    osTimerId scan_timer_id;
    /// Scan result pool current size
    uint8_t scan_result_size;
    /// Scan timeout in second, 0 means forever
    uint16_t scan_timeout_s;
    /// Sync PA/BIS timeout in second, 0 means forever
    uint16_t sync_timeout_s;
    /// Restart PA sync after PA sync failed
    uint8_t resync_pa_times_max;
    /// Sync select
    struct
    {
        /// Address info
        ble_bdaddr_t src_addr;
        /// Adv sid
        uint8_t adv_sid;
        /// Is encrypted
        bool encrypted;
        /// Is from delegator
        uint8_t rx_src_id;
        /// Sync err
        uint16_t err_code;
        /// Broadcast code
        uint8_t bcast_code[BT_BLE_GAP_KEY_LEN];
        /// Broadcast ID
        uint8_t bcast_id[APP_BIS_SELFSCAN_BCAST_ID_LEN];
        /// Current Sync PA lid
        uint16_t sync_hdl;
    } select_src;
    /// Wait resume sync
    bool need_resume_sync;
    /// backgroud scan enable
    bool bg_scan_running;
    /// Restart sync pa count
    uint8_t resync_pa_cnt;
    /// Scan timer running time end to
    uint32_t scan_end_to_ms;
    /// backgroud resume need
    bool bg_resume_need;
} *p_app_bis_selfscan_env = NULL;

const uint16_t app_bis_selfscan_pool_size = sizeof(struct app_bis_selfscan_env) +
                                            sizeof(app_bis_selfscan_src_t) * APP_BIS_SELFSCAN_MAX_SCAN_RESULT +
                                            BISSELFSCAN_DYN_MEM_MAX_LEN;

static uint8_t app_bis_selfscan_buf[app_bis_selfscan_pool_size] = {0};
static app_bis_selfscan_src_t *const app_bis_selfscan_result_pool =
    (app_bis_selfscan_src_t *) &app_bis_selfscan_buf[sizeof(struct app_bis_selfscan_env)];
static uint8_t *const app_bis_selfscan_dyn_mem = &app_bis_selfscan_buf[app_bis_selfscan_pool_size - BISSELFSCAN_DYN_MEM_MAX_LEN];

/*INTERNAL FUNCTIONS DECLARATION*/
static app_bis_selfscan_src_t *app_bis_selfscan_get_scan_result_by_src_lid(uint8_t src_lid_input);
static app_bis_selfscan_src_t *app_bis_selfscan_get_scan_result_by_addr_info(const ble_bdaddr_t *p_src_addr, uint8_t adv_sid);
static uint16_t app_bis_selfscan_store_scan_result_into_pool(const struct app_bis_src_scan_result *scan_result);
static void app_bis_selfscan_cleanup_scan_result_pool(void);
static void app_bis_selfscan_dp_event_cb(DP_EVENT_TYPE_E event_type, ble_if_app_dp_param_u *para_p);
static int app_bis_selfscan_start_sync_impl(ble_bdaddr_t *p_addr, uint8_t adv_sid, bool is_enc, const uint8_t *p_bcast_code);
static int app_bis_selfscan_stop_sync_impl(void);
static int app_bis_selfscan_start_scan(uint8_t user_bf, uint16_t scan_timeout_10ms);
static int app_bis_selfscan_stop_scan(uint8_t user_bf);
static int app_bis_selfscan_start_sync(uint8_t select_src_lid, const uint8_t *p_bcast_code);
static int app_bis_selfscan_stop_sync(void);
static int app_bis_selfscan_set_bcast_code(const uint8_t *bcast_code);
static void app_bis_selfscan_bg_scan_enable(bool start_scan, ble_bdaddr_t *src_addr);
static bool app_bis_selfscan_get_ad_ltv(const uint8_t *p_data_ltv, uint8_t total_len, uint8_t ad_type, const uint8_t **pp_ltv_get);

/*EXTERNAL FUNCTIONS DECLARATION*/
int app_bis_selfscan_init(const app_bis_selfscan_init_cfg_t *p_init_cfg, const app_bis_selfscan_evt_cb_t *p_evt_cb)
{
    uint16_t size = 0;

    if (p_app_bis_selfscan_env != NULL)
    {
        return 0;
    }

    if (p_init_cfg == NULL)
    {
        p_init_cfg = &init_cfg_default;
    }

    if (p_init_cfg->max_result_present == 0)
    {
        return -1;
    }

    if (p_evt_cb != NULL &&
            (p_evt_cb->cb_bcast_code_req == NULL ||
             p_evt_cb->cb_scan_result == NULL ||
             p_evt_cb->cb_scan_state == NULL ||
             p_evt_cb->cb_sync_state == NULL ||
             p_evt_cb->cb_stream_state == NULL))
    {
        return -1;
    }

    size = sizeof(app_bis_selfscan_buf);

    p_app_bis_selfscan_env = (struct app_bis_selfscan_env *)app_bis_selfscan_buf;

    if (p_app_bis_selfscan_env == NULL)
    {
        return -2;
    }

    memset(p_app_bis_selfscan_env, 0, size);
    // Scan result pool capacity
    p_app_bis_selfscan_env->max_result_present = (p_init_cfg->max_result_present >= APP_BIS_SELFSCAN_MAX_SCAN_RESULT ?
                                                  APP_BIS_SELFSCAN_MAX_SCAN_RESULT : p_init_cfg->max_result_present);
    // Store custom callbacks
    if (p_evt_cb != NULL)
    {
        p_app_bis_selfscan_env->evt_cb = *p_evt_cb;
    }
    // Parameter for scan and sync
    p_app_bis_selfscan_env->scan_timeout_s = p_init_cfg->scan_timeout_s;
    p_app_bis_selfscan_env->sync_timeout_s = p_init_cfg->sync_timeout_s;
    p_app_bis_selfscan_env->resync_pa_times_max = p_init_cfg->resync_pa_times_max;
    // Scan timer
    p_app_bis_selfscan_env->scan_timer_id =
        osTimerCreate(osTimer(APP_BIS_SELFSCAN_SCAN_TIMER), osTimerOnce, NULL);
    // PA lid default invalid
    p_app_bis_selfscan_env->select_src.sync_hdl = BISSELSCAN_INVALID_PA_SYNC_HDL;

    if (p_app_bis_selfscan_env->scan_timer_id == NULL)
    {
        p_app_bis_selfscan_env = NULL;
        return -2;
    }

    if (p_evt_cb == NULL)
    {
        app_datapath_server_register_event_callback(app_bis_selfscan_dp_event_cb);
    }

    TRACE(1, "%s scan %d, sync %d, resync %d", __func__, p_app_bis_selfscan_env->scan_timeout_s,
          p_app_bis_selfscan_env->sync_timeout_s, p_app_bis_selfscan_env->resync_pa_times_max);

    return 0;
}

int app_bis_selfscan_deinit(void)
{
    if (p_app_bis_selfscan_env == NULL)
    {
        return 0;
    }

    if (APP_BIS_SELFSCAN_GET_SCAN_STATE() != 0
            || APP_BIS_SELFSCAN_GET_SYNC_STATE() != 0
            || APP_BIS_SELFSCAN_GET_SINK_STATE() != 0)
    {
        return -3;
    }

    // Scan result pool clear
    app_bis_selfscan_cleanup_scan_result_pool();
    // Stop timer and delete it
    osTimerStop(p_app_bis_selfscan_env->scan_timer_id);
    osTimerDelete(p_app_bis_selfscan_env->scan_timer_id);

    p_app_bis_selfscan_env = NULL;

    return 0;
}

int app_bis_selfscan_register_evt_cb(const app_bis_selfscan_evt_cb_t *p_evt_cb)
{
    if (p_evt_cb != NULL)
    {
        p_app_bis_selfscan_env->evt_cb = *p_evt_cb;
        TRACE(0, "%s %p", __func__, p_evt_cb);
    }

    return 0;
}

int app_bis_selfscan_unregister_evt_cb(void)
{
    memset(&p_app_bis_selfscan_env->evt_cb, 0, sizeof(app_bis_selfscan_evt_cb_t));
    return 0;
}

static uint8_t app_bis_selfscan_get_scan_state(void)
{
    return APP_BIS_SELFSCAN_GET_SCAN_STATE();
}

static void app_bis_selfscan_send_scan_state(void)
{
    uint8_t scan_state = app_bis_selfscan_get_scan_state();

    app_datapath_server_send_data_via_notification(BISSELFSCAN_DFT_CON_LID, &scan_state, sizeof(scan_state));
}

static void app_bis_selfscan_send_scan_result(uint8_t src_lid, const struct app_bis_src_scan_result *p_result)
{
    uint16_t size = sizeof(ble_bdaddr_t) + 8 * sizeof(uint8_t) + p_result->broadcast_name_len;

    uint8_t app_bis_selfscan_dyn_mem_[BISSELFSCAN_DYN_MEM_MAX_LEN];
    uint8_t *value = app_bis_selfscan_dyn_mem_;

    if (value)
    {
        value[0] = APP_BIS_SELFSCAN_OP_SCAN_RESULT_NTF;
        value += sizeof(uint8_t);
        value[0] = src_lid;
        value += sizeof(uint8_t);
        memcpy(value, &p_result->src_addr, sizeof(ble_bdaddr_t));
        value += sizeof(ble_bdaddr_t);
        value[0] = p_result->adv_sid;
        value += sizeof(uint8_t);
        value[0] = APP_BIS_SELFSCAN_ASCLL;
        value += sizeof(uint8_t);
        value[0] = p_result->broadcast_name_len;
        value += sizeof(uint8_t);
        memcpy(value, p_result->broadcast_name, p_result->broadcast_name_len);
        value += p_result->broadcast_name_len;
        value[0] = APP_BIS_SELFSCAN_ASCLL;
        value += sizeof(uint8_t);
        value[0] = p_result->pba_feature;
        value += sizeof(uint8_t);
        /// TODO:extended descrptor
        value[0] = 0;
        value += sizeof(uint8_t);
        app_datapath_server_send_data_via_notification(BISSELFSCAN_DFT_CON_LID, value - size, size);
    }
}

static uint8_t app_bis_selfscan_get_auracast_state(void)
{
    uint8_t state = APP_BIS_SELFSCAN_GET_SCAN_STATE();

    if (state == APP_BIS_SELFSCAN_STATE_SYNC_IDLE)
    {
        state = APP_BIS_SELFSCAN_STATE_IDLE;
    }
    else if (state == APP_BIS_SELFSCAN_STATE_SYNCING)
    {
        state = APP_BIS_SELFSCAN_STATE_BUSY;
    }
    else
    {
        state = APP_BIS_SELFSCAN_STATE_RECV;
    }

    return state;
}

static void app_bis_selfscan_send_src_select_state(bool need_op_present)
{
    uint16_t size = sizeof(ble_bdaddr_t) + (3 + need_op_present) * sizeof(uint8_t) + 0;

    uint8_t *value = app_bis_selfscan_dyn_mem;

    if (value)
    {
        if (need_op_present)
        {
            value[0] = APP_BIS_SELFSCAN_OP_SCAN_RESULT_NTF;
            value += sizeof(uint8_t);
        }
        value[0] = app_bis_selfscan_get_auracast_state();
        value += sizeof(uint8_t);
        value[0] = p_app_bis_selfscan_env->select_src.err_code;
        value += sizeof(uint8_t);
        memcpy(value, &p_app_bis_selfscan_env->select_src.src_addr, sizeof(ble_bdaddr_t));
        value += sizeof(ble_bdaddr_t);
        value[0] = p_app_bis_selfscan_env->select_src.adv_sid;
        value += sizeof(uint8_t);
        /*memcpy(value, p_result->broadcast_name, p_result->broadcast_name_len);
        value += p_result->broadcast_name_len;*/
        app_datapath_server_send_data_via_notification(BISSELFSCAN_DFT_CON_LID, value - size, size);
    }
}

static void app_bis_selfscan_inform_upper_scan_state(bool started)
{
    if (p_app_bis_selfscan_env->evt_cb.cb_scan_state != NULL)
    {
        p_app_bis_selfscan_env->evt_cb.cb_scan_state(started);
    }
}

static void app_bis_selfscan_inform_upper_scan_result(uint8_t src_lid, const struct app_bis_src_scan_result *p_result)
{
    if (p_app_bis_selfscan_env->evt_cb.cb_scan_result != NULL)
    {
        p_app_bis_selfscan_env->evt_cb.cb_scan_result(src_lid, p_result);
    }
}

static void app_bis_selfscan_inform_upper_sync_state(enum selfscan_sync_state sync_state, uint16_t err_code)
{
    if (p_app_bis_selfscan_env->evt_cb.cb_sync_state != NULL)
    {
        p_app_bis_selfscan_env->evt_cb.cb_sync_state(&p_app_bis_selfscan_env->select_src.src_addr,
                                                     p_app_bis_selfscan_env->select_src.adv_sid,
                                                     sync_state, err_code);
    }
}

static void app_bis_selfscan_inform_upper_bcast_code_req(uint8_t src_lid)
{
    if (p_app_bis_selfscan_env->evt_cb.cb_bcast_code_req != NULL)
    {
        p_app_bis_selfscan_env->evt_cb.cb_bcast_code_req(src_lid);
    }
}

static void app_bis_selfscan_inform_upper_stream_state(bool started)
{
    if (p_app_bis_selfscan_env->evt_cb.cb_stream_state != NULL)
    {
        p_app_bis_selfscan_env->evt_cb.cb_stream_state(started);
    }
}

static bool app_bis_selfscan_dp_rx_validation(const uint8_t *rx_data, uint16_t data_len)
{
    TRACE(1, "%p %d", rx_data, data_len);
    if (rx_data == NULL || data_len == 0)
    {
        return false;
    }

    switch (rx_data[0])
    {
        case APP_BIS_SELFSCAN_OP_GET_SCAN_STATE:
        case APP_BIS_SELFSCAN_OP_GET_SELECT_SRC:
        case APP_BIS_SELFSCAN_OP_STOP_SYNC:
        {
            if (data_len != sizeof(uint8_t))
            {
                return false;
            }
        }
        break;
        // Stop scan
        case APP_BIS_SELFSCAN_OP_SCAN_COMMAND:
        {
            if (data_len != 2 * sizeof(uint8_t) ||
                    rx_data[1] > APP_BIS_SELFSCAN_SCAN_STOP)
            {
                return false;
            }
        }
        break;
        // Start sync
        case APP_BIS_SELFSCAN_OP_SRC_SELECT_CMD:
        {
            /// TODO:
        }
        break;
        // Set broadcast code
        case APP_BIS_SELFSCAN_OP_BCAST_CODE:
        {
            if (data_len != sizeof(uint8_t) + BT_BLE_GAP_KEY_LEN)
            {
                return false;
            }
        }
        break;
        default:
            return false;
    }

    return true;
}

static void app_bis_selfscan_bg_scan_data_report_handler(const ble_bdaddr_t *bleAddr, int8_t rssi, const uint8_t *adv_buf, uint8_t len)
{
    const uint8_t *p_bcast_name = NULL;
    uint8_t broadcast_name_len = 0;

    if (p_app_bis_selfscan_env->bg_resume_need == false)
    {
        return;
    }

    if (app_bis_selfscan_get_ad_ltv(adv_buf, len, BISSELFSCAN_AD_TYPE_ADV_BCAST_NAME, &p_bcast_name))
    {
        broadcast_name_len = (p_bcast_name[0] - 1);
        p_bcast_name = p_bcast_name + 2;
        DUMP8("%c", p_bcast_name, broadcast_name_len);
    }
    else
    {
        return;
    }

    if (memcmp(bleAddr, &p_app_bis_selfscan_env->select_src.src_addr, sizeof(ble_bdaddr_t)) == 0)
    {
        TRACE(1, "Find select src");
        DUMP8("%02x ", bleAddr, 7);

        p_app_bis_selfscan_env->bg_resume_need = false;
        // Start resume latest sync
        app_bis_selfscan_resume_latest_sync(0, 0);
    }
}

static void app_bis_selfscan_bg_scan_enable(bool start_scan, ble_bdaddr_t *src_addr)
{
    TRACE(1, "%s start %d", __func__, start_scan);

    if (start_scan == true)
    {
        p_app_bis_selfscan_env->bg_scan_running = true;

        app_ble_mgr_scan_result_report_cb_register(app_bis_selfscan_bg_scan_data_report_handler);

        bta_ble_scan_param_t scan_param =
        {
            // Scan window
            .scan_window_ms = 20,
            // Scan Interval
            .scan_interval_ms = 500,
            // Scan Duration
            /// Means never stop
            .scan_duration_ms = APP_BIS_SELFSCAN_BG_SCAN_DUR_MS,
        };

        bta_ble_gap_start_scan(&scan_param);
    }
    else
    {
        bta_ble_gap_stop_scan();
        app_ble_mgr_scan_result_report_cb_register(NULL);

        p_app_bis_selfscan_env->bg_scan_running = false;
    }
}

void app_bis_selfscan_dp_rx_data_handler(const uint8_t *rx_data, uint16_t data_len)
{
    uint8_t opcode = rx_data[0];

    switch (opcode)
    {
        // Scan state
        case APP_BIS_SELFSCAN_OP_GET_SCAN_STATE:
        {
            app_bis_selfscan_send_scan_state();
        }
        break;
        // Src select state
        case APP_BIS_SELFSCAN_OP_GET_SELECT_SRC:
        {
            app_bis_selfscan_send_src_select_state(false);
        }
        break;
        // Stop scan
        case APP_BIS_SELFSCAN_OP_SCAN_COMMAND:
        {
            if (rx_data[1] == APP_BIS_SELFSCAN_SCAN_START)
            {
                app_bis_selfscan_start_scan(APP_BIS_SELFSCAN_SCAN_USER_APP, 0);
            }
            else if (rx_data[1] == APP_BIS_SELFSCAN_SCAN_STOP)
            {
                app_bis_selfscan_stop_scan(APP_BIS_SELFSCAN_SCAN_USER_APP);
            }
        }
        break;
        // Start sync
        case APP_BIS_SELFSCAN_OP_SRC_SELECT_CMD:
        {
            uint8_t bcast_code_len = data_len - 2;
            uint8_t bcast_code[BT_BLE_GAP_KEY_LEN] = {0};

            if (data_len > 2)
            {
                memcpy(bcast_code, &rx_data[2], BT_BLE_GAP_KEY_LEN > bcast_code_len ?
                       bcast_code_len : BT_BLE_GAP_KEY_LEN);
            }

            app_bis_selfscan_start_sync(rx_data[1], data_len > 2 ? bcast_code : NULL);
        }
        break;
        // Stop sync
        case APP_BIS_SELFSCAN_OP_STOP_SYNC:
            app_bis_selfscan_stop_sync();
            break;
        // Set broadcast code
        case APP_BIS_SELFSCAN_OP_BCAST_CODE:
            app_bis_selfscan_set_bcast_code(&rx_data[1]);
            break;
        default:
            break;
    }
}

static void app_bis_selfscan_dp_event_cb(DP_EVENT_TYPE_E event_type, ble_if_app_dp_param_u *para_p)
{
    switch (event_type)
    {
        case DP_DATA_RECEIVED:
        {
            if (app_bis_selfscan_dp_rx_validation(para_p->dp_recv_data.data, para_p->dp_recv_data.data_len) == false)
            {
                TRACE(1, "Invalid RX data");
                break;
            }

            app_bis_selfscan_dp_rx_data_handler(para_p->dp_recv_data.data, para_p->dp_recv_data.data_len);
        }
        break;
        default:
            break;
    }
}

static void app_bis_selfscan_update_select_src_info(const ble_bdaddr_t *p_addr, uint8_t adv_sid,
                                                    bool is_enc, const uint8_t *p_bcast_code)
{
    if (p_addr != NULL)
    {
        p_app_bis_selfscan_env->select_src.src_addr = *p_addr;
    }
    p_app_bis_selfscan_env->select_src.adv_sid = adv_sid;
    p_app_bis_selfscan_env->select_src.encrypted = is_enc;
    // Bcast code
    if (p_bcast_code != NULL)
    {
        memcpy(p_app_bis_selfscan_env->select_src.bcast_code, p_bcast_code, BT_BLE_GAP_KEY_LEN);
    }
}

static bool app_bis_selfscan_restart_pa_sync(void)
{
    if (p_app_bis_selfscan_env->resync_pa_cnt >= p_app_bis_selfscan_env->resync_pa_times_max)
    {
        return false;
    }

    // Start local sync and also wait peer past if needed
    app_bis_selfscan_resume_latest_sync(0, 0);
    // Increase restart pa sync count
    TRACE(1, "Restart PA sync, curr try cnt %d, max %d",
          p_app_bis_selfscan_env->resync_pa_cnt + 1,
          p_app_bis_selfscan_env->resync_pa_times_max);
    p_app_bis_selfscan_env->resync_pa_cnt++;
    return true;
}

static void app_bis_selfscan_pa_sync_state_handler(bool synced, const ble_bdaddr_t *p_addr, uint8_t ea_sid,
                                                   uint16_t sync_hdl, uint16_t reason)
{
    TRACE(0, "pa_state_cb: synced=%d, sync_hdl=%d, err_code=%d", synced, sync_hdl, reason);
    uint8_t curr_sync_state = APP_BIS_SELFSCAN_GET_SYNC_STATE();

    if (synced == false)
    {
        APP_BIS_SELFSCAN_SET_SYNC_STATE(APP_BIS_SELFSCAN_STATE_SYNC_IDLE);
        // Clear PA lid
        p_app_bis_selfscan_env->select_src.sync_hdl = BISSELSCAN_INVALID_PA_SYNC_HDL;
        // PA sync is failed
        if (curr_sync_state == APP_BIS_SELFSCAN_STATE_SYNCING)
        {
            // If Faile to be established or timeout sync, retry if needed
            if (reason == 0x3e || reason == 0x08)
            {
                // Restart PA sync success
                if (app_bis_selfscan_restart_pa_sync() == true)
                {
                    return;
                }
            }

            app_bis_selfscan_send_src_select_state(true);
            app_bis_selfscan_inform_upper_sync_state(APP_BIS_SELFSCAN_SYNC_FAILED, reason);
        }
        else if (curr_sync_state == APP_BIS_SELFSCAN_STATE_SYNCED)// PA sync is terminated
        {
            // PA lost or term befor BIG sync established
            if (APP_BIS_SELFSCAN_GET_SINK_STATE() == 0)
            {
                p_app_bis_selfscan_env->select_src.rx_src_id = APP_BIS_SELFSCAN_INVALID_SRC_LID;

                if (reason == 0x08)
                {
                    // Restart PA sync success
                    if (app_bis_selfscan_restart_pa_sync() == true)
                    {
                        return;
                    }
                }
                else if (p_app_bis_selfscan_env->select_src.err_code == 0x13)
                {
#if (APP_BIS_SELFSCAN_BG_SCAN_ENABLE)
                    TRACE(1, "Sink is stopped due to %d, start BG scan", reason);
                    p_app_bis_selfscan_env->bg_resume_need = true;
                    app_bis_selfscan_bg_scan_enable(true, NULL);
#endif
                }
            }

            if (p_app_bis_selfscan_env->need_resume_sync)
            {
                p_app_bis_selfscan_env->need_resume_sync = false;
                app_bis_selfscan_resume_latest_sync(0, 0);
                return;
            }
        }
        else
        {
            TRACE(0, "Sync state idle, do not handle pa term evt");
        }
    }
    else if (curr_sync_state == APP_BIS_SELFSCAN_STATE_SYNCING)
    {
        APP_BIS_SELFSCAN_SET_SYNC_STATE(APP_BIS_SELFSCAN_STATE_SYNCED);
        // Mark BIS Sinking
        APP_BIS_SELFSCAN_SET_SINK_STATE(APP_BIS_SELFSCAN_STATE_SINKING);
        // Record PA sync lid
        p_app_bis_selfscan_env->select_src.sync_hdl = sync_hdl;
    }
    else
    {
        TRACE(0, "Sync state idle, do not handle pa estb evt");
    }

    // Clear count
    p_app_bis_selfscan_env->resync_pa_cnt = 0;
}

static void app_bis_selfscan_scan_state_callback(bool started, uint16_t err_code)
{
    TRACE(0, "scan_state_cb: started=%d, err_code=%d", started, err_code);

    if (started == false)
    {
        osTimerStop(p_app_bis_selfscan_env->scan_timer_id);
        p_app_bis_selfscan_env->scan_user_bf = 0;
        p_app_bis_selfscan_env->scan_end_to_ms = 0;
        APP_BIS_SELFSCAN_SET_SCAN_STATE(false);
        app_bis_selfscan_inform_upper_scan_state(false);
    }
}

static bool app_bis_selfscan_get_ad_ltv(const uint8_t *p_data_ltv, uint8_t total_len, uint8_t ad_type, const uint8_t **pp_ltv_get)
{
    const uint8_t *p_end = p_data_ltv + total_len;

    while (p_end > p_data_ltv)
    {
        if (p_data_ltv[1] == ad_type)
        {
            if (pp_ltv_get != NULL)
            {
                *pp_ltv_get = p_data_ltv;
                return true;
            }
        }

        p_data_ltv += p_data_ltv[0] + 1;
    }

    return false;
}

static uint16_t app_bis_selfscan_scan_result_add(const struct app_bis_src_scan_result *p_result, uint8_t *p_src_lid)
{
    uint16_t status = app_bis_selfscan_store_scan_result_into_pool(p_result);

    if (status == 0)
    {
        app_bis_selfscan_send_scan_result(p_app_bis_selfscan_env->scan_result_size - 1, p_result);
        app_bis_selfscan_inform_upper_scan_result(p_app_bis_selfscan_env->scan_result_size - 1, p_result);
    }

    TRACE(1, "store result: %d", status);

    if (p_src_lid != NULL && status == 0)
    {
        *p_src_lid = p_app_bis_selfscan_env->scan_result_size - 1;
    }

    return status;
}

static bool app_bis_selfscan_source_report_callback(const ble_bdaddr_t *p_addr, uint8_t ea_sid,
                                                    const uint8_t *bcast_id, const uint8_t *ea_data, uint8_t ea_data_len, int8_t ea_rssi)
{
    uint8_t broadcast_name_len = 0;
    const uint8_t *p_bcast_name = NULL;
    const uint8_t *p_pba_info = ea_data;
    uint8_t pba_feature = 0;
    bool encrypted = false;

    if (APP_BIS_SELFSCAN_GET_SCAN_STATE() == 0)
    {
        return false;
    }

    if (memcmp(bcast_id, empty_bcast_id, sizeof(empty_bcast_id)) == 0)
    {
        TRACE(1, "no bcast id, view as no pa related");
        return false;
    }

    if (app_bis_selfscan_get_ad_ltv(ea_data, ea_data_len, BISSELFSCAN_AD_TYPE_ADV_BCAST_NAME, &p_bcast_name))
    {
        broadcast_name_len = (p_bcast_name[0] - 1);
        p_bcast_name = p_bcast_name + 2;
    }
    else
    {
        TRACE(1, "no broadcast name");
    }

    while (app_bis_selfscan_get_ad_ltv(p_pba_info, ea_data_len - (p_pba_info - ea_data),
                                       BISSELFSCAN_AD_TYPE_SERVICE_16_BIT_DATA, &p_pba_info))
    {
        if (BISSELSCAN_GATT_UUID_PBA_SERVICE != *(uint16_t *)(p_pba_info + 2))
        {
            p_pba_info += (p_pba_info[0] + 1);
            continue;
        }

        pba_feature = *(p_pba_info + 4);
        TRACE(1, "pba feature: 0x%x", pba_feature);
        encrypted = ((pba_feature & 0x01) != 0);
        break;
    }

    app_bis_selfscan_bg_scan_data_report_handler(p_addr, ea_rssi, ea_data, ea_data_len);

    uint16_t size = (sizeof(struct app_bis_src_scan_result) + broadcast_name_len);

    struct app_bis_src_scan_result *p_result = (struct app_bis_src_scan_result *)app_bis_selfscan_dyn_mem;

    if (p_result != NULL)
    {
        memset(p_result, 0, size);
        p_result->adv_sid = ea_sid;
        p_result->src_addr = *p_addr;
        p_result->encrypted = encrypted;
        p_result->pba_feature = pba_feature;
        memcpy(p_result->broadcast_id, bcast_id, sizeof(p_result->broadcast_id));
        p_result->broadcast_name_len = broadcast_name_len;

        if (p_bcast_name != NULL)
        {
            memcpy(p_result->broadcast_name, p_bcast_name, broadcast_name_len);
        }

        uint16_t status = app_bis_selfscan_scan_result_add(p_result, NULL);

        TRACE(1, "store result: %d", status);
    }

    return false;
}

static void app_bis_selfscan_sink_pa_data_callback(uint16_t sync_hdl, const uint8_t *pa_data,
                                                   uint8_t pa_data_len, const bt_ble_big_info_t *big_info)
{
    TRACE(1, "%s sync_hdl %d, len %d", __func__, sync_hdl, pa_data_len);
}

static void app_bis_selfscan_sink_started_handler(uint8_t grp_lid)
{
    if (APP_BIS_SELFSCAN_GET_SINK_STATE() == 0)
    {
        TRACE(0, "Sink state idle, do not handle bis estb evt");
        return;
    }

    TRACE(1, "grp_lid %d sink started", grp_lid);

    APP_BIS_SELFSCAN_SET_SINK_STATE(APP_BIS_SELFSCAN_STATE_SINKED);

    app_bis_selfscan_send_src_select_state(true);
    app_bis_selfscan_inform_upper_sync_state(APP_BIS_SELFSCAN_SYNC_SUCCESS, 0);
    app_bis_selfscan_inform_upper_stream_state(true);
}

static void app_bis_selfscan_sink_stopped_handler(uint8_t grp_lid, uint16_t err_code)
{
    if (APP_BIS_SELFSCAN_GET_SINK_STATE() == 0)
    {
        TRACE(0, "Sync state idle, do not handle bis term evt");
        return;
    }

    app_bis_selfscan_src_t *p_scan_result = NULL;
    uint16_t sync_hdl = p_app_bis_selfscan_env->select_src.sync_hdl;

    TRACE(1, "grp_lid %d sink stopped %d", grp_lid, err_code);
    p_app_bis_selfscan_env->select_src.err_code = err_code;

    if (err_code == 0x3d)
    {
        p_app_bis_selfscan_env->select_src.encrypted = true;

        p_scan_result = app_bis_selfscan_get_scan_result_by_addr_info(&p_app_bis_selfscan_env->select_src.src_addr,
                                                                      p_app_bis_selfscan_env->select_src.adv_sid);

        if (p_scan_result != NULL)
        {
            p_scan_result->scan_result.encrypted = true;
        }
    }
    else if (err_code == 0x16)
    {
        if (APP_BIS_SELFSCAN_GET_SINK_STATE() != 0)
        {
            //bes_ble_audio_bis_stream_set_resume_callback(app_bis_selfscan_resume_latest_sync); // registe the resume callback of bis stream
        }
    }

    APP_BIS_SELFSCAN_SET_SINK_STATE(APP_BIS_SELFSCAN_STATE_SINK_IDLE);

    app_bis_selfscan_send_src_select_state(true);
    app_bis_selfscan_inform_upper_sync_state(APP_BIS_SELFSCAN_SYNC_FAILED, err_code);
    app_bis_selfscan_inform_upper_stream_state(false);

    // If pa was disconnected first before bis term and bis term reason was not local term
    if (sync_hdl == BISSELSCAN_INVALID_PA_SYNC_HDL && err_code != 0x16)
    {
#if (APP_BIS_SELFSCAN_BG_SCAN_ENABLE)
        TRACE(1, "Sink is stopped due to %d, start BG scan", err_code);
        p_app_bis_selfscan_env->bg_resume_need = true;
        app_bis_selfscan_bg_scan_enable(true, NULL);
#endif
    }
}

static void app_bis_selfscan_sink_big_state_handler(bool sink_started, uint16_t sync_hdl, uint8_t grp_lid, uint16_t err_code)
{
    if (sink_started == true)
    {
        app_bis_selfscan_sink_started_handler(grp_lid);
    }
    else
    {
        app_bis_selfscan_sink_stopped_handler(grp_lid, err_code);
    }
}

static int app_bis_selfscan_start_scan(uint8_t user_bf, uint16_t scan_timeout_10ms)
{
    if (p_app_bis_selfscan_env == NULL)
    {
        return -7;
    }

    if (p_app_bis_selfscan_env->bg_scan_running)
    {
        TRACE(1, "BG scan is enabled, disable it first");
        app_bis_selfscan_bg_scan_enable(false, NULL);
    }

    uint32_t curr_time = TICKS_TO_MS(hal_sys_timer_get());

    p_app_bis_selfscan_env->scan_user_bf |= user_bf;

    if (p_app_bis_selfscan_env->scan_end_to_ms != 0 || osTimerIsRunning(p_app_bis_selfscan_env->scan_timer_id))
    {
        if (scan_timeout_10ms != 0 &&
                (APP_BIS_SELFSCAN_GET_LEFT_TIMER(curr_time) > (scan_timeout_10ms * 10)))
        {
            TRACE(1, "user:%x, scan with smaller TO %u", user_bf, scan_timeout_10ms * 10);
            return 0;
        }

        osTimerStop(p_app_bis_selfscan_env->scan_timer_id);
        p_app_bis_selfscan_env->scan_end_to_ms = 0;
    }

    if (scan_timeout_10ms != 0)
    {
        osTimerStart(p_app_bis_selfscan_env->scan_timer_id, scan_timeout_10ms * 10);
        // Record end to time for scan
        p_app_bis_selfscan_env->scan_end_to_ms = curr_time + scan_timeout_10ms * 10;
    }

    if (APP_BIS_SELFSCAN_GET_SCAN_STATE() != 0)
    {
        return 0;
    }

    if (APP_BIS_SELFSCAN_SCAN_USER_APP == user_bf)
    {
        // Clear cache
        app_bis_selfscan_cleanup_scan_result_pool();
    }

    bt_ble_bis_sink_evt_cbs_t event_callback =
    {
        .bis_sink_scan_state_cb     = app_bis_selfscan_scan_state_callback,
        .bis_sink_scan_report_cb    = app_bis_selfscan_source_report_callback,
        .bis_sink_pa_state_cb       = app_bis_selfscan_pa_sync_state_handler,
        .bis_sink_pa_report_cb      = app_bis_selfscan_sink_pa_data_callback,
        .bis_sink_big_state_cb      = app_bis_selfscan_sink_big_state_handler,
    };

    bta_lea_bis_set_sink_param(BISSELSCAN_SELECT_CHAN_LOC_BF,
                               p_app_bis_selfscan_env->select_src.bcast_code, true, &event_callback);

    APP_BIS_SELFSCAN_SET_SCAN_STATE(true);
    app_bis_selfscan_inform_upper_scan_state(true);

    return 0;
}

static int app_bis_selfscan_stop_scan(uint8_t user_bf)
{
    if (p_app_bis_selfscan_env == NULL)
    {
        return -7;
    }

    p_app_bis_selfscan_env->scan_user_bf &= ~user_bf;

    if (p_app_bis_selfscan_env->scan_user_bf != 0)
    {
        TRACE(1, "scan user bf %x", p_app_bis_selfscan_env->scan_user_bf);
        return -8;
    }

    if (APP_BIS_SELFSCAN_GET_SCAN_STATE() == 0)
    {
        return 0;
    }

    osTimerStop(p_app_bis_selfscan_env->scan_timer_id);
    p_app_bis_selfscan_env->scan_end_to_ms = 0;

    bta_lea_bis_enable_sink_scan(false);

    APP_BIS_SELFSCAN_SET_SCAN_STATE(false);
    app_bis_selfscan_inform_upper_scan_state(false);

    return 0;
}

static int app_bis_selfscan_start_sync_impl(ble_bdaddr_t *p_addr, uint8_t adv_sid, bool is_enc, const uint8_t *p_bcast_code)
{
    app_bis_selfscan_update_select_src_info(p_addr, adv_sid, is_enc, p_bcast_code);
    p_app_bis_selfscan_env->bg_resume_need = false;
    p_app_bis_selfscan_env->select_src.err_code = 0;

    if (p_bcast_code != NULL)
    {
        bta_lea_bis_set_sink_param(BISSELSCAN_SELECT_CHAN_LOC_BF,
                                   p_app_bis_selfscan_env->select_src.bcast_code, false, NULL);
    }

    app_bis_selfscan_start_scan(APP_BIS_SELFSCAN_SCAN_USER_SYNC,
                                p_app_bis_selfscan_env->sync_timeout_s * 100 +
                                BISSELFSCAN_SYNC_SCAN_OFFSET_10MS);

    bta_lea_bis_enable_bis_sync(true, false, (ble_bdaddr_t *)p_addr,
                                adv_sid, p_app_bis_selfscan_env->sync_timeout_s);

    APP_BIS_SELFSCAN_SET_SYNC_STATE(APP_BIS_SELFSCAN_STATE_SYNCING);

    app_bis_selfscan_inform_upper_sync_state(APP_BIS_SELFSCAN_SYNC_RUNNING, 0);

    return 0;
}

static int app_bis_selfscan_start_sync(uint8_t select_src_lid, const uint8_t *p_bcast_code)
{
    app_bis_selfscan_src_t *p_src_to_sync = NULL;

    if (p_app_bis_selfscan_env == NULL)
    {
        return -7;
    }

    if (APP_BIS_SELFSCAN_GET_SYNC_STATE() != 0 ||
            APP_BIS_SELFSCAN_GET_SINK_STATE() != 0)
    {
        TRACE(1, "sync state %x", APP_BIS_SELFSCAN_GET_SYNC_STATE());
        return -8;
    }

    TRACE(1, "select src_lid %d", select_src_lid);

    p_src_to_sync = app_bis_selfscan_get_scan_result_by_src_lid(select_src_lid);

    if (p_src_to_sync == NULL)
    {
        return -9;
    }

    TRACE(1, "adv sid %d enc %d", p_src_to_sync->scan_result.adv_sid,
          p_src_to_sync->scan_result.encrypted);
    TRACE(1, "bcastid %d %d %d", p_src_to_sync->scan_result.broadcast_id[0],
          p_src_to_sync->scan_result.broadcast_id[1],
          p_src_to_sync->scan_result.broadcast_id[2]);

    p_app_bis_selfscan_env->select_src.src_addr = p_src_to_sync->scan_result.src_addr;
    p_app_bis_selfscan_env->select_src.adv_sid = p_src_to_sync->scan_result.adv_sid;
    p_app_bis_selfscan_env->select_src.encrypted = p_src_to_sync->scan_result.encrypted;

    if (p_bcast_code == NULL && p_src_to_sync->scan_result.encrypted == true)
    {
        p_app_bis_selfscan_env->select_src.err_code = 0x3d;
        app_bis_selfscan_send_src_select_state(true);
        app_bis_selfscan_inform_upper_bcast_code_req(select_src_lid);
        return -10;
    }

    app_bis_selfscan_start_sync_impl(&p_src_to_sync->scan_result.src_addr,
                                     p_src_to_sync->scan_result.adv_sid,
                                     p_src_to_sync->scan_result.encrypted,
                                     p_bcast_code);
    return 0;
}

static int app_bis_selfscan_stop_sync_impl(void)
{
    if (APP_BIS_SELFSCAN_GET_SYNC_STATE() == APP_BIS_SELFSCAN_STATE_SYNCING)
    {
        bta_lea_bis_enable_bis_sync(false, true, NULL, 0, 0);
    }
    else if (APP_BIS_SELFSCAN_GET_SYNC_STATE() == APP_BIS_SELFSCAN_STATE_SYNCED ||
             APP_BIS_SELFSCAN_GET_SINK_STATE() != 0)
    {
        bta_lea_bis_enable_bis_sync(false, false, NULL, 0, 0);
    }

    return 0;
}

static int app_bis_selfscan_stop_sync(void)
{
    if (p_app_bis_selfscan_env == NULL)
    {
        return -7;
    }

    return app_bis_selfscan_stop_sync_impl();
}

static int app_bis_selfscan_set_bcast_code(const uint8_t *bcast_code)
{
    if (bcast_code == NULL)
    {
        return -1;
    }

    // Restart sync
    if (p_app_bis_selfscan_env->select_src.err_code != 0x3d)
    {
        return -3;
    }

    if (APP_BIS_SELFSCAN_GET_SYNC_STATE() == APP_BIS_SELFSCAN_STATE_SYNC_IDLE)
    {
        memcpy(p_app_bis_selfscan_env->select_src.bcast_code, bcast_code, BT_BLE_GAP_KEY_LEN);
    }
    else
    {
        TRACE(1, "Invalid sync state to set bcast code");
        return -3;
    }

    // Restart sync
    //if (p_app_bis_selfscan_env->select_src.err_code == 0x3d)
    {
        app_bis_selfscan_start_scan(APP_BIS_SELFSCAN_SCAN_USER_SYNC,
                                    p_app_bis_selfscan_env->sync_timeout_s * 100 +
                                    BISSELFSCAN_SYNC_SCAN_OFFSET_10MS);

        bta_lea_bis_set_sink_param(BISSELSCAN_SELECT_CHAN_LOC_BF,
                                   p_app_bis_selfscan_env->select_src.bcast_code, false, NULL);

        bta_lea_bis_enable_bis_sync(true, false, &p_app_bis_selfscan_env->select_src.src_addr,
                                    p_app_bis_selfscan_env->select_src.adv_sid,
                                    p_app_bis_selfscan_env->sync_timeout_s);

        APP_BIS_SELFSCAN_SET_SYNC_STATE(APP_BIS_SELFSCAN_STATE_SYNCING);

        app_bis_selfscan_inform_upper_sync_state(APP_BIS_SELFSCAN_SYNC_RUNNING, 0);
    }

    return 0;
}

static void app_bis_selfscan_resume_latest_sync(uint8_t device_id, uint32_t param)
{
    TRACE(1, "Resume BIS selfscan:%d %d %d %d", device_id, param,
          APP_BIS_SELFSCAN_GET_SYNC_STATE(),
          p_app_bis_selfscan_env->select_src.err_code);

    if (APP_BIS_SELFSCAN_GET_SYNC_STATE() != APP_BIS_SELFSCAN_STATE_SYNC_IDLE &&
            APP_BIS_SELFSCAN_GET_SINK_STATE() != 0)
    {
        TRACE(1, "Invalid state to resume sync, %x, %d", APP_BIS_SELFSCAN_GET_SYNC_STATE(),
              p_app_bis_selfscan_env->select_src.err_code);
        return;
    }

    app_bis_selfscan_start_scan(APP_BIS_SELFSCAN_SCAN_USER_SYNC,
                                p_app_bis_selfscan_env->sync_timeout_s * 100 +
                                BISSELFSCAN_SYNC_SCAN_OFFSET_10MS);

    bta_lea_bis_set_sink_param(BISSELSCAN_SELECT_CHAN_LOC_BF,
                               p_app_bis_selfscan_env->select_src.bcast_code, false, NULL);

    bta_lea_bis_enable_bis_sync(true, false, &p_app_bis_selfscan_env->select_src.src_addr,
                                p_app_bis_selfscan_env->select_src.adv_sid,
                                p_app_bis_selfscan_env->sync_timeout_s);

    APP_BIS_SELFSCAN_SET_SYNC_STATE(APP_BIS_SELFSCAN_STATE_SYNCING);

    app_bis_selfscan_inform_upper_sync_state(APP_BIS_SELFSCAN_SYNC_RUNNING, 0);
}

static void app_bis_selfscan_scan_timer_handler(void const *param)
{
    if (p_app_bis_selfscan_env == NULL)
    {
        return;
    }

    TRACE(1, "%s", __func__);

    app_bis_selfscan_stop_scan(APP_BIS_SELFSCAN_SCAN_USER_APP | APP_BIS_SELFSCAN_SCAN_USER_SYNC);
}

static void app_bis_selfscan_cleanup_scan_result_pool(void)
{
    p_app_bis_selfscan_env->scan_result_size = 0;
}

static uint16_t app_bis_selfscan_store_scan_result_into_pool(const struct app_bis_src_scan_result *scan_result)
{
    app_bis_selfscan_src_t *src_node = NULL;

    if (app_bis_selfscan_get_scan_result_by_addr_info(&scan_result->src_addr, scan_result->adv_sid))
    {
        return -6;
    }

    if (p_app_bis_selfscan_env->scan_result_size >= p_app_bis_selfscan_env->max_result_present)
    {
        return -5;
    }

    if (p_app_bis_selfscan_env->scan_result_size >= APP_BIS_SELFSCAN_MAX_SCAN_RESULT)
    {
        return -5;
    }

    src_node = &app_bis_selfscan_result_pool[p_app_bis_selfscan_env->scan_result_size];

    memcpy(&src_node->scan_result, scan_result, sizeof(*scan_result) + (scan_result->broadcast_name_len >= APP_BIS_SELFSCAN_MAX_NAME_LEN ?
                                                                        APP_BIS_SELFSCAN_MAX_NAME_LEN : scan_result->broadcast_name_len));

    p_app_bis_selfscan_env->scan_result_size++;

    TRACE(1, "now pool size:%d max:%d", p_app_bis_selfscan_env->scan_result_size, p_app_bis_selfscan_env->max_result_present);

    return 0;
}

static app_bis_selfscan_src_t *app_bis_selfscan_get_scan_result_by_src_lid(uint8_t src_lid_input)
{
    app_bis_selfscan_src_t *src_node = NULL;

    if (src_lid_input >= p_app_bis_selfscan_env->scan_result_size)
    {
        return NULL;
    }

    src_node = &app_bis_selfscan_result_pool[src_lid_input];

    return src_node;
}

static app_bis_selfscan_src_t *app_bis_selfscan_get_scan_result_by_addr_info(const ble_bdaddr_t *p_src_addr, uint8_t adv_sid)
{
    app_bis_selfscan_src_t *src_node = NULL;
    struct app_bis_src_scan_result *p_result = NULL;
    uint8_t src_index = 0;

    if (p_src_addr == NULL)
    {
        return NULL;
    }

    for (src_index = 0; src_index < p_app_bis_selfscan_env->scan_result_size; src_index++)
    {
        src_node = &app_bis_selfscan_result_pool[src_index];

        p_result = &(src_node->scan_result);

        if (p_result->adv_sid != adv_sid ||
                memcmp(p_src_addr, &p_result->src_addr, sizeof(p_result->src_addr)) != 0)
        {
            continue;
        }

        return src_node;
    }

    return NULL;
}

uint8_t appp_bis_selfscan_get_src_lid_by_addr_info(const ble_bdaddr_t *p_src_addr, uint8_t adv_sid)
{
    app_bis_selfscan_src_t *src_node = NULL;
    struct app_bis_src_scan_result *p_result = NULL;
    uint8_t src_index = 0;

    if (p_src_addr == NULL)
    {
        return APP_BIS_SELFSCAN_INVALID_SRC_LID;
    }

    for (src_index = 0; src_index < p_app_bis_selfscan_env->scan_result_size; src_index++)
    {
        src_node = &app_bis_selfscan_result_pool[src_index];

        p_result = &(src_node->scan_result);

        if (p_result->adv_sid != adv_sid ||
                memcmp(p_src_addr, &p_result->src_addr, sizeof(p_result->src_addr)) != 0)
        {
            continue;
        }

        return src_index;
    }

    return APP_BIS_SELFSCAN_INVALID_SRC_LID;
}

int app_bis_selfscan_enable_scan(bool enable)
{
    uint8_t scan_cmd[2] = {APP_BIS_SELFSCAN_OP_SCAN_COMMAND};

    scan_cmd[1] = enable ? APP_BIS_SELFSCAN_SCAN_START : APP_BIS_SELFSCAN_SCAN_STOP;

    app_bis_selfscan_dp_rx_data_handler(scan_cmd, sizeof(scan_cmd));

    return 0;
}

int app_bis_selfscan_select_sync_source(uint8_t src_lid, const uint8_t *p_bcast_code)
{
    uint8_t sync_cmd[2 + BT_BLE_GAP_KEY_LEN] = {APP_BIS_SELFSCAN_OP_SRC_SELECT_CMD, src_lid};

    if (p_bcast_code != NULL)
    {
        memcpy(sync_cmd + 2, p_bcast_code, BT_BLE_GAP_KEY_LEN);
    }

    if (app_bis_selfscan_get_scan_result_by_src_lid(src_lid) == NULL)
    {
        return -1;
    }

    app_bis_selfscan_dp_rx_data_handler(sync_cmd, (p_bcast_code != NULL) ? sizeof(sync_cmd) : 2);

    return 0;
}

int app_bis_selfscan_stop_sync_source(void)
{
    uint8_t stop_sync_cmd[] = {APP_BIS_SELFSCAN_OP_STOP_SYNC};

    app_bis_selfscan_dp_rx_data_handler(stop_sync_cmd, sizeof(stop_sync_cmd));

    return 0;
}

#endif /* BIS_SELFSCAN_ENABLED */
