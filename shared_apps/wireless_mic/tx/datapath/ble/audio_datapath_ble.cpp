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

#include "app_bt.h"
#include "app_custom.h"
#include "audio_datapath_ble.h"
#include "bluetooth_ble_api.h"
#include "nvrecord_ble.h"
#include "bluetooth_nv_mgr.h"

#include "bt_drv_interface.h"
#include "app_bt_sync.h"
#include "bt_drv_reg_op.h"
#include "hal_dma.h"
#include "audioflinger.h"
#include "app_audio.h"
#include "app_gaf_custom_api.h"
#include "gaf_media_sync.h"
#include "aob_media_api.h"
#include "gaf_media_stream.h"
#include "app_bap_data_path_itf.h"
#include "hci_transport.h"
#include "bta_bt_api.h"
#include "audio_datapath_adc.h"
#include "audio_datapath_manager.h"
#include "app_wireless_mic.h"
#ifdef AUDIO_LC3_ENCODE
#include "lc3_adapter.h"
#include "audio_datapath_encoder.h"
#endif

#ifdef UTILS_ESHELL_EN
#include "eshell.h"
#endif

#define APP_BLE_ADV_DATA_MAX_SIZE       (31)
#define AOB_PERIPH_MAX_DEVICE_NUM       BLE_CONNECTION_MAX //(1)
#define APP_BLE_ADV_VENDOR_DATA         "LEA_TX_MIC"
#define LEA_MIC_ADV_USER                (BLE_ADV_ACTIVITY_USER_1)

#define APP_MIC_TX_ASE_SRC_LID          (0)

#if (BLE_AUDIO_FRAME_US == 5000)
#define AUDIO_DATAPATH_BLE_UC_SDU_FRAME_DURATION           (APP_GAF_BAP_FRAME_DUR_5MS)
#elif (BLE_AUDIO_FRAME_US == 7500)
#define AUDIO_DATAPATH_BLE_UC_SDU_FRAME_DURATION           (APP_GAF_BAP_FRAME_DUR_7_5MS)
#else
#error "Invalid BLE_AUDIO_FRAME_US!"
#endif

#define AUDIO_DATAPATH_BLE_UC_SDU_FRAME_OCTET              (BLE_AUDIO_FRAME_OCTET)
#define AUDIO_DATAPATH_BLE_UC_LOCATION_BF                  (APP_GAF_LOC_FRONT_LEFT_BIT)
#define AUDIO_DATAPATH_BLE_UC_SAMPLING_FREQ                (APP_GAF_BAP_SAMPLING_FREQ_48000HZ)
#define AUDIO_DATAPATH_BLE_UC_FRAMES_SDU                   (APP_BAP_DFT_BC_SRC_NB_FRAME_BLOCKS_SDU)

#define AUDIO_DATAPATH_BLE_OUTPUT_STREAM_TX_SAFE_TIME       (1500)
#define AUDIO_DATAPATH_BLE_OUTPUT_STREAM_RESAMPLE_MAX_TIME  (AUDIO_CODEC_CAPTURE_STREAM_DMA_INTERVAL_US + 2000)

#if defined(__AUDIO_RESAMPLE__)
#define AUDIO_DATAPATH_BLE_OUTPUT_STREAM_TRIGGER_OFFSET     (310)
#else
#define AUDIO_DATAPATH_BLE_OUTPUT_STREAM_TRIGGER_OFFSET     (210)
#endif

static bool audio_datapath_ble_output_stream_started = false;
static bool audio_datapath_ble_output_stream_trigger_ok = false;
static bool audio_datapath_ble_output_stream_ready = true;
static bool audio_datapath_ble_output_stream_sync_onoff = true;
static bool audio_datapath_ble_output_stream_resample_running = false;
static bool audio_datapath_ble_output_stream_resample_done = false;

static uint8_t audio_datapath_ble_output_stream_con_lid = 0;

static int32_t audio_datapath_ble_output_stream_exceed_time = 0;
static int32_t audio_datapath_ble_output_stream_trigger_diff = 0;
static uint32_t audio_datapath_ble_output_stream_trigger_time = 0;
static uint32_t audio_datapath_ble_output_stream_anchor_point = 0;
static uint32_t audio_datapath_ble_output_stream_dma_time = 0;
static uint32_t audio_datapath_ble_output_stream_safe_time[3] = {0};

static GAF_AUDIO_STREAM_ENV_T* audio_datapath_ble_output_stream_stream_env = NULL;

static const app_gaf_bap_qos_req_t ntf_qos_req =
{
    .pres_delay_min_us = 10000,
    .pres_delay_max_us = 40000,
    .pref_pres_delay_min_us = 10000,
    .pref_pres_delay_max_us = 40000,
    .trans_latency_max_ms = 10,
    .framing = APP_ISO_UNFRAMED_MODE,
#if (mHDT_LE_SUPPORT)
    .phy_bf = 0x80,
#else
    .phy_bf = 0x2,
#endif
    .retx_nb = 2,
};

static const app_gaf_bap_cfg_t ntf_codec_cfg =
{
    {
        .location_bf = AUDIO_DATAPATH_BLE_UC_LOCATION_BF,
        .frame_octet = AUDIO_DATAPATH_BLE_UC_SDU_FRAME_OCTET,
        .sampling_freq = AUDIO_DATAPATH_BLE_UC_SAMPLING_FREQ,
        .frame_dur = AUDIO_DATAPATH_BLE_UC_SDU_FRAME_DURATION,
        .frames_sdu = AUDIO_DATAPATH_BLE_UC_FRAMES_SDU,
    },
    {
        0,
    },
};

extern "C" void ble_audio_set_adv_enable_flag(bool adv_enable);

#ifdef AOB_USE_LEGACY_ADV
static void audio_datapath_ble_start_legacy_adv(void)
{
    uint8_t avail_space = 0;
    bes_ble_gap_cus_adv_param_t param = {LEA_MIC_ADV_USER, };
    uint8_t p_adv[APP_BLE_ADV_DATA_MAX_SIZE] = {0};
    uint8_t p_adv_len = 0;
    const char *local_le_name = NULL;
    uint8_t name_len = 0;
    local_le_name = gap_local_le_name(&name_len);

    //Fill adv ad type
    p_adv[p_adv_len++] = 0x02;
    p_adv[p_adv_len++] = 0x01;
    p_adv[p_adv_len++] = 0x06;

    // Fill vendor data
    p_adv[p_adv_len++] = strlen(APP_BLE_ADV_VENDOR_DATA) + 1;
    p_adv[p_adv_len++] = GAP_DT_MANUFACTURER_DATA;
    memcpy(&p_adv[p_adv_len], APP_BLE_ADV_VENDOR_DATA, strlen(APP_BLE_ADV_VENDOR_DATA));
    p_adv_len += strlen(APP_BLE_ADV_VENDOR_DATA);

    // Check if ble name can be added to the adv Data
    avail_space = APP_BLE_ADV_DATA_MAX_SIZE - p_adv_len - BLE_ADV_DATA_STRUCT_HEADER_LEN;
    if (avail_space > BLE_ADV_DATA_STRUCT_HEADER_LEN)
    {
        avail_space = MIN(avail_space, name_len);
        p_adv[p_adv_len++] = avail_space;
        p_adv[p_adv_len++] = (avail_space == name_len) ? BLE_ADV_TYPE_COMPLETE_NAME : BLE_ADV_TYPE_SHORTENED_NAME;
        memcpy(&p_adv[p_adv_len], local_le_name, avail_space - 1);
        p_adv_len += avail_space - 1;
    }

    param.actv_user = LEA_MIC_ADV_USER;
    param.is_custom_adv_flags = true;
    param.type = BLE_ADV_PUBLIC_STATIC;
    param.local_addr = NULL;
    param.peer_addr = NULL;
    param.adv_interval = 20;
    param.adv_type = ADV_TYPE_UNDIRECT;
    param.adv_mode = ADV_MODE_LEGACY;
    param.tx_power_dbm = 4;
    param.adv_data = p_adv;
    param.adv_data_size = p_adv_len;
    param.scan_rsp_data = NULL;
    param.scan_rsp_data_size = 0;

    bes_ble_gap_custom_adv_write_data(&param);
    bes_ble_gap_custom_adv_start(LEA_MIC_ADV_USER);
}
#else
static uint8_t app_ble_impl_start_advertising(bt_addr_type_t own_addr_type,
                                              gap_adv_callback_t cb,
                                              uint32_t cont_filter_legacy_conn_scan,
                                              const bt_bdaddr_t *custom_local_addr)
{
    gap_adv_param_t adv = {0};
    adv.force_start = true;
    adv.enable_scan_req_notify = true;
    adv.connectable = (cont_filter_legacy_conn_scan & 0x02) ? true : false;
    adv.scannable = (cont_filter_legacy_conn_scan & 0x01) ? true : false;
    adv.use_legacy_pdu = (cont_filter_legacy_conn_scan & 0x04) ? true : false;
    adv.include_tx_power_data = true;
    adv.policy = (cont_filter_legacy_conn_scan & 0x08) ? GAP_ADV_ACCEPT_ALL_CONN_SCAN_REQS_IN_LIST : GAP_ADV_ACCEPT_ALL_CONN_SCAN_REQS;
    adv.continue_advertising = (cont_filter_legacy_conn_scan & 0x10) ? true : false;
    adv.own_addr_type = own_addr_type;
    adv.fast_advertising = true;

    if (custom_local_addr) {
        adv.use_custom_local_addr = true;
        adv.custom_local_addr = *custom_local_addr;
    }

    return gap_create_advertising_set(&adv, cb);
}

static void audio_datapath_ble_start_ext_adv(void)
{
    uint8_t adv_handle = GAP_INVALID_ADV_HANDLE;
    gap_dt_buf_t adv_data_buf = {0};
    const char *local_le_name = NULL;
    uint8_t len = 0;
    local_le_name = gap_local_le_name(&len);

    bes_ble_gap_stop_generic_adv();

    adv_handle = app_ble_impl_start_advertising(BT_ADDR_TYPE_PUBLIC, bes_ble_gap_app_server_callback, 0x02, NULL);

    gap_dt_add_flags(&adv_data_buf, GAP_FLAGS_LE_LIMITED_DISCOVERABLE_MODE, false);
    gap_dt_add_local_le_name(&adv_data_buf, false, local_le_name);
    gap_dt_add_data_type(&adv_data_buf, GAP_DT_MANUFACTURER_DATA, (uint8_t *)APP_BLE_ADV_VENDOR_DATA, strlen(APP_BLE_ADV_VENDOR_DATA));
    gap_set_adv_data(adv_handle, ppb_get_data(adv_data_buf.ppb), adv_data_buf.ppb->len);
    gap_dt_buf_clear(&adv_data_buf);

    gap_enable_advertising(adv_handle, 0, 0);
}
#endif

void audio_datapath_ble_start_adv(void)
{
    LOG_MSG(0, "%s", __func__);

    if (app_bt_get_curr_access_mode() != BTIF_BAM_NOT_ACCESSIBLE) {
        bt_export_me_write_access_mode(BTIF_BAM_NOT_ACCESSIBLE, false);
    }

    if (bes_ble_gap_connection_count() >= AOB_PERIPH_MAX_DEVICE_NUM) {
        LOG_ERR(0, "%s, conn dev arrive max, don't allow", __func__);
        return;
    }
    bes_ble_gap_force_switch_adv(BLE_SWITCH_USER_BLE_AUDIO, true);

#ifdef AOB_USE_LEGACY_ADV
    audio_datapath_ble_start_legacy_adv();
#else
    audio_datapath_ble_start_ext_adv();
#endif
}

void audio_datapath_ble_stop_adv(void)
{
    LOG_MSG(0, "%s", __func__);
    bes_ble_gap_stop_generic_adv();
}

static void audio_datapath_ble_link_connect_handler(uint8_t conidx, ble_bdaddr_t peer_bdaddr)
{
    LOG_MSG(0, "%s", __func__);
    bes_ble_gap_force_switch_adv(BLE_SWITCH_USER_BLE_AUDIO, false);
    app_ble_refresh_adv_state_by_custom_adv(BLE_ADVERTISING_INTERVAL);
}

static void audio_datapath_ble_link_disconnect_handler(uint8_t conidx, ble_bdaddr_t peer_bdaddr, uint8_t err_code)
{
    LOG_MSG(0, "%s err_code %d", __func__, err_code);

    if (app_wireless_mic_mode_ble()) {
        bes_ble_gap_force_switch_adv(BLE_SWITCH_USER_BLE_AUDIO, true);
        audio_datapath_ble_start_adv();
    }
}

static void app_restore_ase_codec_configurations(uint8_t conidx)
{
    uint8_t ase_lid = conidx * aob_media_get_ascs_ase_number_per_conn(APP_GAF_DIRECTION_MAX) + APP_MIC_TX_ASE_SRC_LID;

    aob_media_ascs_srv_set_codec(ase_lid, AOB_CODEC_ID_LC3, (app_gaf_bap_qos_req_t*)&ntf_qos_req, (app_gaf_bap_cfg_t*)&ntf_codec_cfg);
}

static void audio_datapath_ble_link_encrypt_handler(uint8_t conidx)
{
    LOG_MSG(0, "%s", __func__);

    app_restore_ase_codec_configurations(conidx);
}

static void audio_datapath_ble_link_bond_handler(uint8_t conidx, uint8_t succ, uint8_t err_code)
{
    LOG_MSG(0,"%s conidx %d succ %d err_code %d", __func__, conidx, succ, err_code);
}

static void audio_datapath_ble_global_handler_ind(ble_event_t *event, void *output)
{
    if (BLE_LINK_CONNECTED_EVENT == event->evt_type) {
        audio_datapath_ble_link_connect_handler(event->p.connect_handled.conidx,
                                                  event->p.connect_handled.peer_bdaddr);
    } else if (BLE_DISCONNECT_EVENT == event->evt_type) {
        audio_datapath_ble_link_disconnect_handler(event->p.disconnect_handled.conidx,
                                                     event->p.disconnect_handled.peer_bdaddr,
                                                     event->p.disconnect_handled.errCode);
    } else if (BLE_CONNECT_ENCRYPT_EVENT == event->evt_type) {
        audio_datapath_ble_link_encrypt_handler(event->p.connect_encrypt_handled.conidx);
    } else if (BLE_CONNECT_BOND_FAIL_EVENT == event->evt_type) {
        audio_datapath_ble_link_bond_handler(event->p.connect_bond_handled.conidx,
                                               event->p.connect_bond_handled.success,
                                               event->p.connect_bond_handled.reason);
    }
}

uint8_t audio_datapath_ble_get_connect_device_num(void)
{
    return bes_ble_gap_connection_count();
}

void audio_datapath_ble_disconnect_all(void)
{
    bes_ble_gap_disconnect_all();
}

void audio_datapath_ble_del_nv_record(uint8_t *bleAddr)
{
    if (bleAddr) {
        bluetooth_nv_mgr_ble_record_del(BLE_NV_REC_DEL_BT_NV_REC_CHG, bleAddr);
    }
}

void audio_datapath_ble_del_all_nv_record(void)
{
    nv_record_ble_delete_all_entry();
}

uint8_t audio_datapath_ble_get_all_paired_dev(bes_ble_bdaddr_t *bleAddr)
{
    BLE_ADDR_INFO_T nv_addr[BLE_RECORD_NUM] = {0};
    uint8_t pairedDevNum = nv_record_blerec_enum_paired_dev_addr(nv_addr);

    LOG_MSG(0, "ble paired dev %d", pairedDevNum);
    for (uint8_t i = 0; i < pairedDevNum; i++) {
        memcpy((void *)&bleAddr[i], (void *)&nv_addr[i], sizeof(BLE_ADDR_INFO_T));
    }
    return pairedDevNum;
}

static void audio_datapath_ble_sink_stream_start(uint8_t con_lid)
{
    audio_datapath_ble_output_stream_con_lid = con_lid;

    audio_manager_datapath_set(AUDIO_OUTPUT_PATH_BIT_BLE);
}

static void audio_datapath_ble_sink_stream_stop(uint8_t con_lid)
{
    audio_manager_datapath_clear(AUDIO_OUTPUT_PATH_BIT_BLE);
}

void audio_datapath_ble_init(void)
{
    LOG_MSG(0, "enter %s", __func__);

    bes_ble_gap_core_register_global_handler(audio_datapath_ble_global_handler_ind);

    gap_write_le_suggested_default_data_length(0x28, 0x4290);

    ble_audio_set_adv_enable_flag(false); //always disable ble audio adv and use custom adv

    // audio_datapath_ble_start_adv();

    app_ble_audio_register_start_callback(audio_datapath_ble_sink_stream_start);
    app_ble_audio_register_stop_callback(audio_datapath_ble_sink_stream_stop);
    app_ble_audio_register_stop_single_stream_callback(audio_datapath_ble_sink_stream_stop);
}

// ref: gaf_stream_common_start_sync_capture
void audio_datapath_ble_output_stream_set_trigger_time(GAF_AUDIO_STREAM_ENV_T* pStreamEnv)
{
    uint32_t iso_evt_time = gaf_media_common_get_latest_tx_iso_evt_timestamp(pStreamEnv);
    uint32_t current_bt_time = gaf_media_sync_get_curr_time();
    uint32_t trigger_bt_time = iso_evt_time;
    uint32_t iso_interval = pStreamEnv->stream_info.captureInfo.isoIntervalUs;

    while (trigger_bt_time < current_bt_time + iso_interval)
    {
        trigger_bt_time += iso_interval;
    }

    trigger_bt_time += AUDIO_DATAPATH_BLE_OUTPUT_STREAM_TRIGGER_OFFSET;

    LOG_MSG(3, "iso anch:%u, cur time:%u, trigger time:%u", iso_evt_time, current_bt_time, trigger_bt_time);

    gaf_stream_common_update_capture_stream_state(pStreamEnv, GAF_CAPTURE_STREAM_INITIALIZED);
    gaf_stream_common_set_capture_trigger_time(pStreamEnv, trigger_bt_time);
    audio_datapath_ble_output_stream_trigger_time = trigger_bt_time;
}

static void audio_datapath_ble_output_stream_enable_dma_tc(void)
{
    uint8_t dma_ch = af_stream_get_dma_chan(AUDIO_CODEC_CAPTURE_STREAM_ID, AUD_STREAM_CAPTURE);
    uint32_t dma_base = af_stream_get_dma_base_addr(AUDIO_CODEC_CAPTURE_STREAM_ID, AUD_STREAM_CAPTURE);

    if (dma_ch != HAL_DMA_CHAN_NONE) {
        bt_drv_reg_op_enable_dma_tc(dma_ch & 0xFF, dma_base);

        af_stream_dma_tc_irq_enable(AUDIO_CODEC_CAPTURE_STREAM_ID, AUD_STREAM_CAPTURE);
    } else {
        LOG_ERR(1, "%s invalid dma ch", __func__);
    }
}

// ref: gaf_media_prepare_capture_trigger
static void audio_datapath_ble_output_stream_prepare_trigger(void)
{
    uint8_t trigger_channel = 0;

    LOG_MSG(1, "%s start", __func__);

#if defined(IBRT)
    // trigger_channel = app_bt_sync_get_available_trigger_channel(APP_BT_SYNC_OP_MAX, APP_BT_SYNC_POLICY_DEFAULT);
#endif

    btdrv_syn_clr_trigger(trigger_channel);

    af_codec_set_device_bt_sync_source(AUDIO_CODEC_CAPTURE_STREAM_DEVICE, AUD_STREAM_CAPTURE, trigger_channel);
    af_codec_sync_config(AUD_STREAM_CAPTURE, AF_CODEC_SYNC_TYPE_BT, false);
    af_codec_sync_config(AUD_STREAM_CAPTURE, AF_CODEC_SYNC_TYPE_BT, true);
}

static uint32_t audio_datapath_ble_output_stream_get_dma_time_us(void)
{
    uint32_t btclk; //hal slot -- 312.5us
    uint16_t btcnt; //hal microsecond -- 0.5 us
    uint8_t dma_ch = af_stream_get_dma_chan(AUDIO_CODEC_CAPTURE_STREAM_ID, AUD_STREAM_CAPTURE);
    uint32_t dma_base = af_stream_get_dma_base_addr(AUDIO_CODEC_CAPTURE_STREAM_ID, AUD_STREAM_CAPTURE);
    uint32_t dma_time_us = 0;

    if (dma_ch != HAL_DMA_CHAN_NONE) {
        bt_drv_reg_op_dma_tc_clkcnt_get_by_ch(&btclk, &btcnt, dma_ch&0xFF, dma_base);
        dma_time_us = bt_syn_ble_bt_time_to_bts(btclk, btcnt);
    } else {
        LOG_ERR(1, "%s invalid dma ch", __func__);
    }

    return dma_time_us;
}

POSSIBLY_UNUSED static void audio_datpath_ble_output_stream_hci_tx_done_handler(uint8_t type)
{
    if (type == HCI_ISO_TYPE) {
        uint32_t iso_evt_time = gaf_media_common_get_latest_tx_iso_evt_timestamp(audio_datapath_ble_output_stream_stream_env);
        uint32_t current_time = gaf_media_sync_get_curr_time();
        audio_datapath_ble_output_stream_safe_time[0] = GAF_AUDIO_CLK_32_BIT_DIFF(current_time, iso_evt_time);

        // curr safe time
        if (audio_datapath_ble_output_stream_safe_time[0] <= AUDIO_DATAPATH_BLE_OUTPUT_STREAM_TX_SAFE_TIME) {
            LOG_ERR(2, "hci iso tx done too late, iso time:%u, tx done:%u", iso_evt_time, current_time);
        }

        // max safe time
        if (audio_datapath_ble_output_stream_safe_time[0] > audio_datapath_ble_output_stream_safe_time[1]) {
            audio_datapath_ble_output_stream_safe_time[1] = audio_datapath_ble_output_stream_safe_time[0];
        }

        // min safe time
        if (audio_datapath_ble_output_stream_safe_time[2] == 0) {
            audio_datapath_ble_output_stream_safe_time[2] = audio_datapath_ble_output_stream_safe_time[0];
        } else if (audio_datapath_ble_output_stream_safe_time[0] < audio_datapath_ble_output_stream_safe_time[2]) {
            audio_datapath_ble_output_stream_safe_time[2] = audio_datapath_ble_output_stream_safe_time[0];
        }
    }
}

static void audio_datapath_ble_output_stream_pre_handler(uint8_t* buff, uint32_t len)
{
    audio_datapath_ble_output_stream_dma_time = audio_datapath_ble_output_stream_get_dma_time_us();

    if (audio_datapath_ble_output_stream_trigger_ok == false) {
        if (audio_datapath_ble_output_stream_stream_env == NULL) {
            LOG_ERR(1, "%s invalid lea stream env", __func__);
            return;
        }

        uint32_t diff_time = GAF_AUDIO_CLK_32_BIT_DIFF(audio_datapath_ble_output_stream_trigger_time, audio_datapath_ble_output_stream_dma_time);
        audio_datapath_ble_output_stream_trigger_diff = diff_time - AUDIO_CODEC_CAPTURE_STREAM_DMA_INTERVAL_US;
        LOG_MSG(3, "trigger time:%u, first dma time:%u, diff time:%d",
                audio_datapath_ble_output_stream_trigger_time, audio_datapath_ble_output_stream_dma_time, diff_time);

        gaf_stream_common_update_capture_stream_state(audio_datapath_ble_output_stream_stream_env, GAF_CAPTURE_STREAM_STREAMING_TRIGGERED);
        audio_datapath_ble_output_stream_trigger_ok = true;
        LOG_MSG(0, "ble output stream trigger ok!");
    }
}

static void audio_datapath_ble_output_stream_post_handler(uint8_t* buff, uint32_t len)
{
    static uint16_t seq_num = 0;
    static uint16_t dma_cycle = 0;
    static float last_ratio = 0.f;
    static uint32_t last_dma_time = 0;
    static uint32_t trigger_time = 0;
    float ratio = 0.f;
    int32_t diff_time = 0;
    uint32_t current_time = 0;
    uint32_t iso_evt_time = 0;
    uint32_t expect_time = 0;
    uint32_t dma_time = 0;
    uint32_t dma_time_diff = 0;
    GAF_AUDIO_STREAM_ENV_T* pStreamEnv = audio_datapath_ble_output_stream_stream_env;

    if (pStreamEnv == NULL) {
        LOG_ERR(1, "%s invalid lea stream env", __func__);
        return;
    }

    dma_time = audio_datapath_ble_output_stream_dma_time;
    iso_evt_time = gaf_media_common_get_latest_tx_iso_evt_timestamp(pStreamEnv);

    if (audio_datapath_ble_output_stream_ready == false) {
        if (!audio_datapath_ble_output_stream_exceed_time && !audio_datapath_ble_output_stream_anchor_point) {
            trigger_time = iso_evt_time + AUDIO_DATAPATH_BLE_OUTPUT_STREAM_TRIGGER_OFFSET;

            audio_datapath_ble_output_stream_exceed_time = GAF_AUDIO_CLK_32_BIT_DIFF(dma_time, trigger_time);
            if (audio_datapath_ble_output_stream_exceed_time <= AUDIO_DATAPATH_BLE_OUTPUT_STREAM_RESAMPLE_MAX_TIME) {
                dma_cycle = 0;
                last_dma_time = 0;
                audio_datapath_ble_output_stream_resample_running = true;
            } else {
                LOG_ERR(3, "unexpect trigger time:%u, dma time:%u, exceed time:%d",
                        (uint32_t)trigger_time, (uint32_t)dma_time, audio_datapath_ble_output_stream_exceed_time);
                trigger_time = 0;
                audio_datapath_ble_output_stream_exceed_time = 0;
                return;
            }
            LOG_MSG(3, "expect trigger time:%u, dma time:%u, exceed time:%d",
                    (uint32_t)trigger_time, (uint32_t)dma_time, audio_datapath_ble_output_stream_exceed_time);
            return;
        }

        dma_cycle++;
        expect_time = trigger_time + dma_cycle * AUDIO_CODEC_CAPTURE_STREAM_DMA_INTERVAL_US;

        if (last_dma_time) {
            audio_datapath_ble_output_stream_exceed_time -= GAF_AUDIO_CLK_32_BIT_DIFF((last_dma_time + AUDIO_CODEC_CAPTURE_STREAM_DMA_INTERVAL_US), dma_time);
        }

        LOG_MSG(6, "expect:%u, dma:%u, diff:%u, cycle:%u, exceed:%d", (uint32_t)expect_time, (uint32_t)dma_time,
                (uint32_t)(dma_time - last_dma_time), (uint32_t)dma_cycle, audio_datapath_ble_output_stream_exceed_time);
        last_dma_time = dma_time;

        if (!audio_datapath_ble_output_stream_resample_done && abs(audio_datapath_ble_output_stream_exceed_time) > 15) {
            ratio = audio_datapath_codec_capture_stream_sync_pid_calc(NULL, -audio_datapath_ble_output_stream_exceed_time);
            if (ratio != last_ratio) {
                audio_datapath_codec_capture_stream_sync_tune(ratio);
                last_ratio = ratio;
            }
        } else {
            audio_datapath_codec_capture_stream_sync_tune(0);
            audio_datapath_ble_output_stream_resample_done = true;
            audio_datapath_ble_output_stream_anchor_point = expect_time;
            if (dma_cycle >= 200 && (dma_cycle % 2) == 0) {
                audio_datapath_codec_capture_stream_sync_pid_init();
                audio_datapath_ble_output_stream_ready = true;
                audio_datapath_ble_output_stream_trigger_diff = 0;
                audio_datapath_ble_output_stream_resample_running = false;
                dma_cycle = 0;
                last_ratio = 0.f;
                seq_num = 0;
                LOG_MSG(0, "ble output stream resample done!");
            }
        }
        return;
    }

    seq_num++;
    current_time = gaf_media_sync_get_curr_time();
    iso_evt_time = gaf_media_common_get_latest_tx_iso_evt_timestamp(pStreamEnv);
    dma_time = audio_datapath_ble_output_stream_dma_time;
    dma_time_diff = dma_time - last_dma_time;
    last_dma_time = dma_time;
    dma_time = dma_time - audio_datapath_ble_output_stream_trigger_diff - AUDIO_DATAPATH_BLE_OUTPUT_STREAM_TRIGGER_OFFSET;

    for (uint8_t i = 0; i < GAF_AUDIO_ASE_TOTAL_COUNT; i++) {
        if (GAF_AUDIO_INVALID_ISO_CHANNEL == pStreamEnv->stream_info.captureInfo.aseChInfo[i].iso_channel_hdl) {
            continue;
        }

        app_bap_dp_itf_send_data_directly(pStreamEnv->stream_info.captureInfo.aseChInfo[i].ase_handle, seq_num, buff, len, current_time);
    }

    if (!audio_datapath_ble_output_stream_anchor_point) {
        seq_num = 0;
        last_ratio = 0.f;
        last_dma_time = 0;
        audio_datapath_ble_output_stream_anchor_point = dma_time;
        LOG_MSG(1, "audio_datapath_ble_output_stream_anchor_point:%u", audio_datapath_ble_output_stream_anchor_point);
    }

    if (iso_evt_time < dma_time) {
        if (iso_evt_time >= (dma_time + AUDIO_CODEC_CAPTURE_STREAM_DMA_INTERVAL_US)) {
            LOG_MSG(2, "bt clk bit flip, iso time:%u dma time:%u", iso_evt_time, dma_time);
        } else {
            LOG_ERR(2, "iso evt time less than dma time, iso time:%u dma time:%u", iso_evt_time, dma_time);
            iso_evt_time += AUDIO_CODEC_CAPTURE_STREAM_DMA_INTERVAL_US;
        }
    }

    diff_time = GAF_AUDIO_CLK_32_BIT_DIFF(dma_time + AUDIO_CODEC_CAPTURE_STREAM_DMA_INTERVAL_US, iso_evt_time);

    if (audio_datapath_ble_output_stream_sync_onoff) {
        if (abs(diff_time) > 20) {
            LOG_ERR(3, "unexpected dma time diff:%d, iso time:%u, dma time:%u", diff_time, iso_evt_time, dma_time);
        } else {
            ratio = audio_datapath_codec_capture_stream_sync_pid_calc(NULL, -diff_time);
            if (ratio != last_ratio) {
                audio_datapath_codec_capture_stream_sync_tune(ratio);
                last_ratio = ratio;
            }
        }
    }

    LOG_DBG(8, "curr:%u, iso:%u, dma:%u, diff:%d/%d, safe:%d/%d/%d", current_time, iso_evt_time, dma_time,
                diff_time, dma_time_diff, audio_datapath_ble_output_stream_safe_time[0],
                audio_datapath_ble_output_stream_safe_time[1], audio_datapath_ble_output_stream_safe_time[2]);
}

void audio_datapath_ble_output_stream_con_lid_set(uint8_t con_lid)
{
    audio_datapath_ble_output_stream_con_lid = con_lid;
}

void audio_datapath_ble_output_stream_onoff(bool onoff)
{
    uint8_t con_lid = audio_datapath_ble_output_stream_con_lid;

    LOG_MSG(1, "datapath ble output stream --> %s", onoff == true ? "on" : "off");

    if (audio_datapath_ble_output_stream_started == onoff) {
        return;
    }

    if (onoff) {
        GAF_AUDIO_STREAM_ENV_T* pStreamEnv = _gaf_refresh_all_stream_info_by_con_lid(con_lid);

        if (pStreamEnv == NULL) {
            LOG_ERR(1, "%s invalid lea stream env", __func__);
            return;
        }

        bta_bt_write_sleep_enable(BTA_BT_SLEEP_AUDIO_DATAPATH_BLE, 0);

        // hci_tx_done_cb_register(audio_datpath_ble_output_stream_hci_tx_done_handler);

        gaf_stream_common_update_capture_stream_state(pStreamEnv, GAF_CAPTURE_STREAM_INITIALIZING);

        audio_datapath_ble_output_stream_enable_dma_tc();

        if (!audio_datapath_codec_capture_stream_running()) {

            audio_datapath_ble_output_stream_prepare_trigger();

            audio_datapath_ble_output_stream_set_trigger_time(pStreamEnv);

            audio_datapath_ble_output_stream_ready = true;
        } else {
            audio_datapath_ble_output_stream_ready = false;
            audio_datapath_ble_output_stream_resample_done = false;
            audio_datapath_ble_output_stream_exceed_time = 0;
        }

        audio_datapath_ble_output_stream_stream_env = pStreamEnv;

#ifdef AUDIO_LC3_ENCODE
        audio_datapath_codec_capture_stream_with_encode_set(true);
        audio_datapath_encode_frame_size_update(AUDIO_LC3_CODEC_BITRATE_COUNT);
#endif
        audio_datapath_codec_capture_stream_output_pre_handler_register(audio_datapath_ble_output_stream_pre_handler);
        audio_datapath_codec_capture_stream_output_post_handler_register(audio_datapath_ble_output_stream_post_handler);
    } else {
        audio_datapath_codec_capture_stream_output_pre_handler_degister();
        audio_datapath_codec_capture_stream_output_post_handler_degister();

        bta_bt_write_sleep_enable(BTA_BT_SLEEP_AUDIO_DATAPATH_BLE, 1);

        // hci_tx_done_cb_register(NULL);

        gaf_stream_common_update_capture_stream_state(audio_datapath_ble_output_stream_stream_env, GAF_CAPTURE_STREAM_IDLE);
        audio_datapath_ble_output_stream_stream_env = NULL;

        audio_datapath_ble_output_stream_trigger_ok = false;
        audio_datapath_ble_output_stream_trigger_time = 0;
        audio_datapath_ble_output_stream_anchor_point = 0;
        audio_datapath_ble_output_stream_trigger_diff = 0;

        memset(audio_datapath_ble_output_stream_safe_time, 0x0, sizeof(audio_datapath_ble_output_stream_safe_time));
    }

    audio_datapath_ble_output_stream_started = onoff;
}