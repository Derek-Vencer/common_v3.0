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
#include "cmsis_os.h"
#include "hal_trace.h"
#include "plat_types.h"
#include "hal_timer.h"

#include "audio_datapath_ble.h"
#include "app_ble.h"
#include "bluetooth_ble_api.h"
#include "app_bt.h"
#include "ble_audio_core_api.h"
#include "ble_audio_mobile_info.h"
#include "aob_csip_api.h"
#include "app_bap.h"
#include "ble_aob_common.h"
#include "app_gaf_define.h"
#include "aob_media_api.h"
#include "nvrecord_ble.h"
#include "bluetooth_nv_mgr.h"
#include "ble_audio_earphone_info.h"
#include "bta_dongle.h"
#include "app_wireless_mic.h"

#ifdef CFG_APP_DATAPATH_SERVER
#include "ble_datapath_server.h"
#endif

#ifdef UTILS_ESHELL_EN
#include "eshell.h"
#endif

#define AOB_SCAN_USE_WHITE_LIST                     (0)
#define AOB_TARGET_DEVICE_NUM                       (BLE_CONNECTION_MAX)
#define APP_BLE_ADV_VENDOR_DATA                     "LEA_TX_MIC"

#define APP_MIC_RX_ASE_SRC_LID                      (0)

#define AUDIO_DATAPATH_BLE_STREAM_AUTO              (1)
#define AUDIO_DATAPATH_BLE_UC_CODEC_ID              (AOB_CODEC_ID_LC3)
#define AUDIO_DATAPATH_BLE_UC_SAMPLING_FREQ         (APP_GAF_BAP_SAMPLING_FREQ_48000HZ)
#define AUDIO_DATAPATH_BLE_UC_DIRECTION_SRC         (BES_BLE_GAF_DIRECTION_SRC)
#define AUDIO_DATAPATH_BLE_UC_CONTEXT_TYPE          (BES_BLE_GAF_CONTEXT_TYPE_GAME_BIT)
#define AUDIO_DATAPATH_BLE_UC_SDU_INTERVAL_US       (BLE_AUDIO_FRAME_US)
#define AUDIO_DATAPATH_BLE_UC_SDU_FRAME_OCTET       (BLE_AUDIO_FRAME_OCTET)
#define AUDIO_DATAPATH_BLE_UC_ISO_INTERVAL          (AUDIO_DATAPATH_BLE_UC_SDU_INTERVAL_US / 1250)

typedef struct
{
    bool scan_enable;
} vow_app_dongle_env_t;

typedef struct {
    bool need_conn;
    uint8_t bleAddr[6];
} BLE_CONN_INFO_T;

static bool audio_datapath_ble_scan_enable = true;
static vow_app_dongle_env_t app_dongle_env = {0};

BLE_CONN_INFO_T bleConnDevInfo[AOB_TARGET_DEVICE_NUM] = {0};

static const AOB_BAP_CIG_PARAM_T dft_cis_timing = {
    .c2p_bn = 1,
    .c2p_nse = 3,
    .c2p_ft = 1,
    .p2c_bn = 0,
    .p2c_nse = 0,
    .p2c_ft = 1,
    .iso_interval_1_25ms = AUDIO_DATAPATH_BLE_UC_ISO_INTERVAL,
};

static bes_lea_ase_cfg_param_t ase_to_start =
{
    AUDIO_DATAPATH_BLE_UC_SAMPLING_FREQ,
    AUDIO_DATAPATH_BLE_UC_SDU_FRAME_OCTET,
    AUDIO_DATAPATH_BLE_UC_DIRECTION_SRC, (const bes_gaf_codec_id_t *)AUDIO_DATAPATH_BLE_UC_CODEC_ID,
    AUDIO_DATAPATH_BLE_UC_CONTEXT_TYPE
};

#define WLSMIC_CONN_RESTART_TIMEOUT_INTERVAL             (500)
osTimerId wlsmic_conn_restart_timer_id = NULL;
static void audio_datapath_ble_conn_restart_timer_handler(void* param);
static const osTimerAttr_t wlsmic_conn_restart_timer_attr = {
    .name = "wlsmic_conn_restart_timer",
    .attr_bits = 0U,
    .cb_mem = NULL,
    .cb_size = 0U,
};

#ifdef CFG_APP_DATAPATH_SERVER
static uint8_t audio_datapath_ble_client_conn[BLE_CONNECTION_MAX];
static uint8_t audio_datapath_ble_active_client_conidx = INVALID_CONNECTION_INDEX;
extern "C" void app_datapath_server_register_disconnected_done(app_datapath_server_disconnected_done_t callback);
extern "C" void app_datapath_server_register_connected_done(app_datapath_server_connected_done_t callback);
#endif

void audio_datapath_ble_add_connDev_info(uint8_t *bleAddr)
{
    uint8_t idx = 0;
    bool found = false;
    uint8_t empty_addr[6] = {0};

    for (idx = 0; idx < AOB_TARGET_DEVICE_NUM; idx++) {
        if (!memcmp(&bleConnDevInfo[idx].bleAddr, bleAddr, 6)) {
            bleConnDevInfo[idx].need_conn = true;
            found = true;
            break;
        }
    }

    for (idx = 0; idx < AOB_TARGET_DEVICE_NUM && found == false; idx++) {
        if (!memcmp(bleConnDevInfo[idx].bleAddr, empty_addr, 6)) {
            bleConnDevInfo[idx].need_conn = true;
            memcpy(bleConnDevInfo[idx].bleAddr, bleAddr, 6);
            found = true;
            break;
        }
    }

    if (found == true &&wlsmic_conn_restart_timer_id &&
            !osTimerIsRunning(wlsmic_conn_restart_timer_id)) {
        osTimerStart(wlsmic_conn_restart_timer_id, WLSMIC_CONN_RESTART_TIMEOUT_INTERVAL);
    }
}

void audio_datapath_ble_connDev_info_connect_handler(uint8_t *bleAddr)
{
    for (uint8_t i = 0; i < AOB_TARGET_DEVICE_NUM; i++) {
        if (!memcmp(bleConnDevInfo[i].bleAddr, bleAddr, 6)) {
            bleConnDevInfo[i].need_conn = false;
            break;
        }
    }
}

void audio_datapath_ble_remove_connDev_info(uint8_t *bleAddr)
{
    for (uint8_t i = 0; i < AOB_TARGET_DEVICE_NUM; i++) {
        if (!memcmp(bleConnDevInfo[i].bleAddr, bleAddr, 6)) {
            memset(&bleConnDevInfo[i], 0, sizeof(BLE_CONN_INFO_T));
            break;
        }
    }
}

uint8_t audio_datapath_ble_get_connDev_cnt(void)
{
    uint8_t connDevCnt = 0;
    uint8_t empty_addr[6] = {0};
    for (uint8_t i = 0; i < AOB_TARGET_DEVICE_NUM; i++) {
        if (memcmp(bleConnDevInfo[i].bleAddr, empty_addr, 6)) {
            connDevCnt++;
        }
    }
    return connDevCnt;
}

void audio_datapath_ble_dump_dev_info(void)
{
    uint8_t empty_addr[6] = {0};
    for (uint8_t i = 0; i < AOB_TARGET_DEVICE_NUM; i++) {
        if (memcmp(bleConnDevInfo[i].bleAddr, empty_addr, 6)) {
            TRACE(0,"dump ble conn info[%d] %d:", i, bleConnDevInfo[i].need_conn);
            DUMP8("%02x ", bleConnDevInfo[i].bleAddr, 6);
        }
    }
}

static void audio_datapath_ble_start_scan(void)
{
    if (app_bt_get_curr_access_mode() != BTIF_BAM_NOT_ACCESSIBLE) {
        bt_export_me_write_access_mode(BTIF_BAM_NOT_ACCESSIBLE, false);
    }

    if (bes_ble_gap_connection_count() >= AOB_TARGET_DEVICE_NUM) {
        TRACE(0, "%s, conn dev arrive max, don't allow", __func__);
        return;
    }

    bta_dongle_scan_start();
}

bes_lea_ase_cfg_param_t* audio_datapath_ble_ase_info(void)
{
    return &ase_to_start;
}

static void audio_datapath_ble_link_connect_handler(uint8_t *peer_bdaddr)
{
    uint8_t connCnt = bes_ble_gap_connection_count();
    bool is_tx_dev_connected = connCnt ? true : false;
    ble_bdaddr_t peer_addr;
    peer_addr.addr_type = BES_GAP_STATIC_ADDR;
    memcpy(peer_addr.addr, peer_bdaddr, 6);
    uint8_t conidx = bes_ble_get_con_id_by_addr(&peer_addr);
    POSSIBLY_UNUSED uint16_t connhdl = bes_ble_gap_get_conhdl_from_conidx(conidx);

    TRACE(0, "%s conidx %d connCnt %d", __func__, conidx, connCnt);
    if (connCnt == AOB_TARGET_DEVICE_NUM) {
        bta_dongle_scan_stop();
        if (wlsmic_conn_restart_timer_id) {
            osTimerStop(wlsmic_conn_restart_timer_id);
        }
    } else if (connCnt > AOB_TARGET_DEVICE_NUM) {
        bta_dongle_disconn_dev(peer_bdaddr);
        return;
    }

    audio_datapath_ble_connDev_info_connect_handler(peer_bdaddr);

    if (is_tx_dev_connected)
    {
        // gattc_cache_restore_handles(connhdl, (gatt_client_cache_t *)gatt_cache_for_mic);

        bes_ble_bap_ascc_set_cis_count_in_cig(AOB_TARGET_DEVICE_NUM);
        // Use dft cis timing @see dft_cis_timing
        bes_ble_bap_ascc_prepare_cig_param(&dft_cis_timing);
        // Use 5ms or 7.5ms sdu interval
        bes_ble_bap_ascc_set_sdu_interval(AUDIO_DATAPATH_BLE_UC_SDU_INTERVAL_US, AUDIO_DATAPATH_BLE_UC_SDU_INTERVAL_US);
    }
}

static void audio_datapath_ble_link_disconnect_handler(uint8_t *peer_bdaddr)
{
    TRACE(0, "%s", __func__);

    if (bes_ble_gap_connection_count() < AOB_TARGET_DEVICE_NUM) {
        audio_datapath_ble_add_connDev_info(peer_bdaddr);
    }
}

static void audio_datapath_ble_scan_status_check(void)
{
    uint8_t local_dev_cnt = audio_datapath_ble_get_connDev_cnt();
    uint8_t gap_connected_num = bes_ble_gap_connection_count();

    TRACE(0, "%s enable:%d ble cnt:%d %d %d", __func__, audio_datapath_ble_scan_enable, local_dev_cnt, gap_connected_num, AOB_TARGET_DEVICE_NUM);

    if (audio_datapath_ble_scan_enable && local_dev_cnt < AOB_TARGET_DEVICE_NUM && gap_connected_num < AOB_TARGET_DEVICE_NUM) {
        audio_datapath_ble_start_scan();
    }
}

static void audio_datapath_ble_scan_report_handler(uint8_t *bleAddr,
                                            const uint8_t *adv_buf, uint8_t len)
{
    if (ble_audio_is_ux_mobile() == false) {
        return;
    }

    BLE_ADV_DATA_T *particle = NULL;

    for (uint8_t offset = 0; offset < len;)
    {
        /// get the particle of adv data
        particle = (BLE_ADV_DATA_T *)(adv_buf + offset);
        bool isConnecting = app_ble_is_connection_on_by_addr(bleAddr);
        bool isConnected = ble_audio_mobile_check_device_connected(bleAddr);

        if ((GAP_DT_MANUFACTURER_DATA == particle->type) &&
            (!memcmp(particle->value, APP_BLE_ADV_VENDOR_DATA, particle->length - 1)) &&
            (!isConnecting) && (!isConnected) &&
            (bes_ble_gap_connection_count() < AOB_TARGET_DEVICE_NUM) )
        {
            audio_datapath_ble_add_connDev_info(bleAddr);
            break;
        }
        offset += (ADV_PARTICLE_HEADER_LEN + particle->length);
    }
}

void audio_datapath_ble_enter_create_ble_link_handler(void)
{
    BLE_ADDR_INFO_T nv_addr[BLE_RECORD_NUM] = {0};
    uint8_t pairedDevNum = nv_record_blerec_enum_paired_dev_addr(nv_addr);

    TRACE(0, "%s paired num %d", __func__, pairedDevNum);

#if !(AOB_SCAN_USE_WHITE_LIST)
    for (uint8_t i = 0; i < pairedDevNum; i++) {
        audio_datapath_ble_add_connDev_info(nv_addr[i].addr);
    }

    if (pairedDevNum < AOB_TARGET_DEVICE_NUM) {
        audio_datapath_ble_start_scan();
    }
#endif

    if (wlsmic_conn_restart_timer_id) {
        osTimerStart(wlsmic_conn_restart_timer_id, WLSMIC_CONN_RESTART_TIMEOUT_INTERVAL);
    }
}

static void audio_datapath_ble_conn_restart_timer_handler(void* param)
{
    static bool dump_info = false;
    bool isConnecting = false;
    bool isConnected = false;
    for (uint8_t i = 0; i < AOB_TARGET_DEVICE_NUM; i++) {
        if (bleConnDevInfo[i].need_conn) {
            isConnecting = app_ble_is_connection_on_by_addr(bleConnDevInfo[i].bleAddr);
            isConnected = ble_audio_mobile_check_device_connected(bleConnDevInfo[i].bleAddr);
            if ((!isConnecting) && (!isConnected)) {
                bta_dongle_conn_dev(bleConnDevInfo[i].bleAddr);
            }
            dump_info = true;
        }
    }

    if (dump_info) {
        audio_datapath_ble_dump_dev_info();
        dump_info = false;
    }
}

static void audio_datapath_ble_global_handler_ind(ble_event_t *event, void *output)
{
    if (event == NULL) {
        LOG_ERR(1, "%s invalid param.", __func__);
    }

    if (BLE_SCAN_DATA_REPORT_EVENT != event->evt_type) {
        LOG_MSG(1, "ble global event:%d", event->evt_type);
    }

    if (BLE_SMP_PAIRING_CMP_EVENT == event->evt_type) { //BLE_SCAN_STOPPED_EVENT
        audio_datapath_ble_scan_status_check();
    }
}

void audio_datapath_ble_dongle_open_handler(void)
{
#if (AOB_SCAN_USE_WHITE_LIST)
    ble_bdaddr_t bdaddr[2] = {0};
    // ble_audio_mobile_get_peripherals_addr(bdaddr[0].addr, bdaddr[1].addr);
    bes_ble_gap_set_white_list(BLE_WHITE_LIST_USER_CENTRAL, bdaddr, 2);
    audio_datapath_ble_add_connDev_info(bdaddr[0].addr);
    audio_datapath_ble_add_connDev_info(bdaddr[1].addr);
#endif

    bes_ble_bap_set_activity_type(GAF_BAP_ACT_TYPE_CIS_AUDIO);
    app_bap_set_device_num_to_be_connected(AOB_TARGET_DEVICE_NUM);
    bes_ble_gap_core_register_global_handler(audio_datapath_ble_global_handler_ind);
    audio_datapath_ble_enter_create_ble_link_handler();
    TRACE(0, "%s", __func__);
}

static void audio_datapath_ble_dongle_pairing_check(void)
{
    const bta_dongle_info_t* bta_dongle_info = NULL;

    TRACE(0, "%s, %d", __func__, app_dongle_env.scan_enable);
    if (!app_dongle_env.scan_enable)
    {
        return;
    }

    bta_dongle_get_info(&bta_dongle_info);
    if (!bta_dongle_info)
    {
        return;
    }

    if (bta_dongle_info->conn_dev_count < AOB_TARGET_DEVICE_NUM)
    {
        bta_dongle_scan_start();
    }
}

static void audio_datapath_ble_event_cb(bta_dongle_event_t* event)
{
    const bta_dongle_info_t* bta_dongle_info = NULL;

    switch (event->event)
    {
        case BTA_DONGLE_EVENT_OPEN:
        {
            TRACE(0, "[%s]:dongle_open: %d", __func__, event->param.open.status);
            audio_datapath_ble_dongle_open_handler();
        }
        break;
        case BTA_DONGLE_EVENT_CLOSE:
        {
            TRACE(0, "[%s]:dongle_close: %d", __func__, event->param.close.status);
        }
        break;
        case BTA_DONGLE_EVENT_START_SCAN:
        {
            TRACE(0, "[%s]:dongle_scan_start: %d", __func__, event->param.start_scan.status);
            app_dongle_env.scan_enable = true;
        }
        break;
        case BTA_DONGLE_EVENT_STOP_SCAN:
        {
            TRACE(0, "[%s]:dongle_scan_stop: %d", __func__, event->param.start_scan.status);
            app_dongle_env.scan_enable = false;
            audio_datapath_ble_scan_status_check();
        }
        break;
        case BTA_DONGLE_EVENT_SCAN_REPORT:
        {
            TRACE(0, "[%s]:dongle_scan_report: name=%s, rssi=%d", __func__,
                event->param.scan_report.name? event->param.scan_report.name: "dev no name",
                event->param.scan_report.rssi);

            bta_dongle_get_info(&bta_dongle_info);
            if (bta_dongle_info->open_param.output_type != BTA_DONGLE_DATA_PATH_LEA_UC) {
                break;
            }
            audio_datapath_ble_scan_report_handler(event->param.scan_report.addr,
                                            event->param.scan_report.data,
                                            event->param.scan_report.data_len);
        }
        break;
        case BTA_DONGLE_EVENT_DEV_CONN:
        {
            TRACE(0, "[%s]:dongle_conn: %d", __func__, event->param.dev_conn.status);
            DUMP8("%02x ", event->param.dev_conn.addr, 6);
            if (event->param.dev_conn.status == BTA_DONGLE_ERROR_NO) {
                audio_datapath_ble_link_connect_handler(event->param.dev_conn.addr);
            }
            audio_datapath_ble_dongle_pairing_check();
        }
        break;
        case BTA_DONGLE_EVENT_DEV_DISCONN:
        {
            TRACE(0, "[%s]:dongle_disconn: %d", __func__, event->param.dev_disconn.status);
            DUMP8("%02x ", event->param.dev_disconn.addr, 6);
            audio_datapath_ble_link_disconnect_handler(event->param.dev_disconn.addr);
            audio_datapath_ble_dongle_pairing_check();
        }
        break;
        case BTA_DONGLE_EVENT_STREAM_START:
        {
            TRACE(0, "[%s]:dongle_stream_start", __func__);
        }
        break;
        case BTA_DONGLE_EVENT_STREAM_STOP:
        {
            TRACE(0, "[%s]:dongle_stream_stop", __func__);
        }
        break;
        default:
        break;
    }
}

#ifdef CFG_APP_DATAPATH_SERVER
static void audio_datapath_ble_server_connected_handler(uint8_t conidx)
{
    LOG_MSG(1, "datapath ble connected conidx:%d", conidx);

    for (uint8_t i = 0; i < BLE_CONNECTION_MAX; i++) {
        if (audio_datapath_ble_client_conn[i] == INVALID_CONNECTION_INDEX) {
            audio_datapath_ble_client_conn[i] = conidx;
            break;
        }
    }
}

static void audio_datapath_ble_server_disconnected_handler(uint8_t conidx)
{
    LOG_MSG(1, "datapath ble disconnected conidx:%d", conidx);

    for (uint8_t i = 0; i < BLE_CONNECTION_MAX; i++) {
        if (audio_datapath_ble_client_conn[i] == conidx) {
            audio_datapath_ble_client_conn[i] = INVALID_CONNECTION_INDEX;
            break;
        }
    }
}

void audio_datapath_ble_server_set_active_client_conidx(uint8_t conidx)
{
    audio_datapath_ble_active_client_conidx = conidx;
    LOG_MSG(1, "datapath server active conidx:%d", audio_datapath_ble_active_client_conidx);
}

uint8_t audio_datapath_ble_server_get_active_client_conidx(void)
{
    return audio_datapath_ble_active_client_conidx;
}

uint8_t* audio_datapath_ble_server_get_client_conn(void)
{
    return audio_datapath_ble_client_conn;
}
#endif

void audio_datapath_ble_init(void)
{
    int ret = 0;
    bta_dongle_open_t open_param = {0};

    if (NULL == wlsmic_conn_restart_timer_id) {
        wlsmic_conn_restart_timer_id = osTimerNew(audio_datapath_ble_conn_restart_timer_handler, osTimerPeriodic,
                                                  NULL, &wlsmic_conn_restart_timer_attr);
    }

#ifdef CFG_APP_DATAPATH_SERVER
    for (uint8_t i = 0; i < BLE_CONNECTION_MAX; i++) {
        audio_datapath_ble_client_conn[i] = BLE_INVALID_CONNECTION_INDEX;
    }
    app_datapath_server_register_connected_done(audio_datapath_ble_server_connected_handler);
    app_datapath_server_register_disconnected_done(audio_datapath_ble_server_disconnected_handler);
#endif

    memset(&bleConnDevInfo, 0, sizeof(bleConnDevInfo));

    open_param.input_type  = BTA_DONGLE_DATA_PATH_USB;
    open_param.output_type = BTA_DONGLE_DATA_PATH_LEA_UC;
    open_param.stream_auto = AUDIO_DATAPATH_BLE_STREAM_AUTO;
    open_param.event_cb    = audio_datapath_ble_event_cb;

    open_param.output_param.lea_uc.sdu_interval_us = AUDIO_DATAPATH_BLE_UC_SDU_INTERVAL_US;
    open_param.output_param.lea_uc.frame_octet     = AUDIO_DATAPATH_BLE_UC_SDU_FRAME_OCTET;
    memcpy(open_param.output_param.lea_uc.codec_id, AUDIO_DATAPATH_BLE_UC_CODEC_ID, sizeof(open_param.output_param.lea_uc.codec_id));

    ret = bta_dongle_open(&open_param);

    TRACE(0, "%s ret %d", __func__, ret);
}

uint8_t audio_datapath_ble_get_connect_device_num(void)
{
    return bes_ble_gap_connection_count();
}

uint8_t audio_datapath_ble_get_all_paired_dev(bes_ble_bdaddr_t *bleAddr)
{
    BLE_ADDR_INFO_T nv_addr[BLE_RECORD_NUM] = {0};
    uint8_t pairedDevNum = nv_record_blerec_enum_paired_dev_addr(nv_addr);

    TRACE(0,"ble paired dev %d", pairedDevNum);
    for (uint8_t i = 0; i < pairedDevNum; i++) {
        memcpy((void *)&bleAddr[i], (void *)&nv_addr[i], sizeof(BLE_ADDR_INFO_T));
    }
    return pairedDevNum;
}

void audio_datapath_ble_start_connect(uint8_t *bleAddr)
{
    if (!bleAddr) {
        return;
    }
    audio_datapath_ble_add_connDev_info(bleAddr);
}

void audio_datapath_ble_disconnect_all(void)
{
    bes_ble_gap_disconnect_all();
}

void audio_datapath_ble_stop_connect_by_addr(uint8_t *bleAddr)
{
    if (bleAddr) {
        bta_dongle_disconn_dev(bleAddr);
    }
}

void audio_datapath_ble_del_nv_record(uint8_t *bleAddr)
{
    if (bleAddr) {
        audio_datapath_ble_remove_connDev_info(bleAddr);
        bluetooth_nv_mgr_ble_record_del(BLE_NV_REC_DEL_BT_NV_REC_CHG, bleAddr);
    }
}

void audio_datapath_ble_del_all_nv_record(void)
{
    memset(&bleConnDevInfo, 0, sizeof(bleConnDevInfo));
    nv_record_ble_delete_all_entry();
}

void audio_datapath_ble_enter_scan_to_pairing(bool scan)
{
    if (scan == false) {
        audio_datapath_ble_scan_enable = false;
        bta_dongle_scan_stop();
        return;
    }

    if (bes_ble_gap_connection_count() < AOB_TARGET_DEVICE_NUM) {
        audio_datapath_ble_scan_enable = true;
        audio_datapath_ble_start_scan();
    }
}

#ifdef UTILS_ESHELL_EN
static void audio_datapath_ble_scan_cmd(int argc, char *argv[])
{
    if (argc != 2) {
        TRACE(1, "%s invalid param.", __func__);
        return;
    }
    uint8_t enable = atoi(argv[1]);

    audio_datapath_ble_enter_scan_to_pairing(enable);
}
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_BT_COMMON, "vow_ble_scan", "ble scan 0/1", audio_datapath_ble_scan_cmd);

static void audio_datapath_ble_dump_dev_info_cmd(int argc, char *argv[])
{
    audio_datapath_ble_dump_dev_info();
}
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_BT_COMMON, "vow_ble_dump", "ble dump", audio_datapath_ble_dump_dev_info_cmd);

#endif