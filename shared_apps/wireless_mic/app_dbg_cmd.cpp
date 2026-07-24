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
#ifdef APP_TRACE_RX_ENABLE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cmsis.h"
#include "cmsis_os.h"
#include "hal_trace.h"
#include "plat_types.h"
#include "app_wireless_mic.h"
#include "app_trace_rx.h"
#include "audio_datapath_manager.h"
#include "audio_datapath_ble.h"
#include "audio_datapath_adc.h"

#ifdef AUD_OUTPUT_BY_EMMC
#include "audio_datapath_emmc.h"
#endif

#ifdef TX_ALGO_PROCESS
#include "tx_algo.h"
#endif

#if defined(CFG_APP_DATAPATH_CLIENT) || defined(CFG_APP_DATAPATH_SERVER)
#include "app_custom_cmd.h"
#include "app_ble_cmd_handler.h"
#endif

typedef struct
{
    const char* string;
    void (*cmd_function)(const char* param, unsigned int param_len);
} app_wireless_mic_cmd_table_t;

uint8_t wireless_dbg_level = 0;

static void app_wireless_mic_mode_set_cmd(const char* param, unsigned int param_len)
{
    uint8_t app_mode = 0;

    app_mode = 1 << (atoi(param) - 1);

    LOG_MSG(1, "app_mode:0x%02x.", app_mode);
    app_wireless_mic_mode_set(app_mode);
}

void app_wireless_mic_mode_clear_cmd(const char* param, unsigned int param_len)
{
    uint8_t app_mode = 0;

    app_mode = 1 << (atoi(param) - 1);

    LOG_MSG(1, "app_mode:0x%02x.", app_mode);
    app_wireless_mic_mode_clear(app_mode);
}

void app_wireless_mic_dbg_onoff_cmd(const char* param, unsigned int param_len)
{
    wireless_dbg_level = atoi(param);

    LOG_MSG(1, "wireless_dbg_level:0x%02x.", wireless_dbg_level);
}

#ifdef CFG_APP_DATAPATH_SERVER
static void app_wireless_mic_test_ble_send_cmd(const char* param, unsigned int param_len)
{
    CUSTOM_CMD_RET_STATUS_E ret;
    uint8_t test_cmd[128] = {0};
    uint8_t test_cmd_len = param_len > 128 ? 128 : param_len;
    memcpy(test_cmd, param, test_cmd_len);

#ifdef WIRELESS_MIC_RX
    uint8_t* condix = audio_datapath_ble_server_get_client_conn();
    for (uint8_t i = 0; i < BLE_CONNECTION_MAX; i++) {
        if (condix[i] != INVALID_CONNECTION_INDEX) {
            audio_datapath_ble_server_set_active_client_conidx(i);

            ret = BLE_send_custom_command(OP_CUSTOM_DATA, (uint8_t*)test_cmd, test_cmd_len, TRANSMISSION_VIA_NOTIFICATION);
            if (ret) {
                LOG_ERR(1, "ble send custom cmd failed:%d", ret);
            } else {
                LOG_MSG(1, "ble send custom cmd:%s", test_cmd);
            }
        }
    }
#elif defined(WIRELESS_MIC_TX)
    ret = BLE_send_custom_command(OP_CUSTOM_DATA, (uint8_t*)test_cmd, test_cmd_len, TRANSMISSION_VIA_NOTIFICATION);
    if (ret) {
        LOG_ERR(1, "ble send custom cmd failed:%d", ret);
    } else {
        LOG_MSG(1, "ble send custom cmd:%s", test_cmd);
    }
#endif
}
#endif

#ifdef CFG_APP_DATAPATH_CLIENT
static void app_wireless_mic_test_ble_recv_cmd_handler(uint32_t funcCode, uint8_t* ptrParam, uint32_t paramLen, uint8_t devId)
{
    LOG_MSG(2, "ble recv custom cmd:%s", ptrParam);
    DUMP8("%02x", ptrParam, paramLen);
}

BLE_CUSTOM_COMMAND_TO_ADD(CUSTOM_CMD_GROUP_INDEX_CUSTOM, OP_CUSTOM_DATA,
                          app_wireless_mic_test_ble_recv_cmd_handler,
                          false,
                          0,
                          NULL);
#endif

#if defined(WIRELESS_MIC_TX)
void audio_datapath_adc_dummy_test_onoff_cmd(const char* param, unsigned int param_len)
{
    uint8_t onoff = 0;

    onoff = atoi(param);
    if (onoff) {
        audio_manager_datapath_set(AUDIO_OUTPUT_PATH_BIT_ADC_DUMMY_TEST);
    } else {
        audio_manager_datapath_clear(AUDIO_OUTPUT_PATH_BIT_ADC_DUMMY_TEST);
    }
    LOG_MSG(1, "### adc dump test on = %u", onoff);
}

void audio_datapath_adc_tx_capture_stream_mute_cmd(const char* param, unsigned int param_len)
{
    uint8_t onoff = 0;

    onoff = atoi(param);
    if (onoff) {
        audio_datapath_codec_capture_stream_mute_set(true);
    } else {
        audio_datapath_codec_capture_stream_mute_set(false);
    }
}

#ifdef AUD_OUTPUT_BY_EMMC
void audio_datapath_emmc_dump_set_mode_cmd(const char* param, unsigned int param_len)
{
    uint8_t mode = 0;

    mode = atoi(param);
    if (mode > 0 && mode <= 4) {
        audio_datapath_emmc_dump_mode_set(mode);
        audio_manager_datapath_set(AUDIO_OUTPUT_PATH_BIT_EMMC);
    } else {
        audio_datapath_emmc_dump_mode_set(0);
        audio_manager_datapath_clear(AUDIO_OUTPUT_PATH_BIT_EMMC);
    }
    LOG_MSG(1, "### audio_datapath_emmc_dump_set_mode = %u", mode);
}
#endif

#ifdef TX_ALGO_PROCESS
void audio_datapath_adc_tx_algo_all_onoff_cmd(const char* param, unsigned int param_len)
{
    uint8_t onoff = 0;

    onoff = atoi(param);
    if (onoff) {
        tx_algo_onoff_set(true);
    } else {
        tx_algo_onoff_set(false);
    }
}

void audio_datapath_adc_tx_algo_ns_onoff_cmd(const char* param, unsigned int param_len)
{
    uint8_t onoff = 0;

    onoff = atoi(param);
    if (onoff) {
        audio_datapath_codec_capture_stream_ns_algo_onoff_set(true);
    } else {
        audio_datapath_codec_capture_stream_ns_algo_onoff_set(false);
    }
}

void audio_datapath_adc_tx_algo_sw_pre_gain_cmd(const char* param, unsigned int param_len)
{
    float gain = 0;

    gain = atoi(param);
    if (abs(gain) >= 50.0f) {
        LOG_MSG(0, "sw_gain should between (-50.0, 50.0)");
        return ;
    }

    LOG_MSG(1, "tx_pre_gain = (f*1000):%d", (uint32_t)(gain*1000));
    tx_pre_gain_set(gain);
}

void audio_datapath_adc_tx_algo_sw_post_gain_cmd(const char* param, unsigned int param_len)
{
    float gain = 0;

    gain = atoi(param);
    if (abs(gain) >= 50.0f) {
        LOG_MSG(0, "sw_gain should between (-50.0, 50.0)");
        return ;
    }

    LOG_MSG(1, "tx_post_gain = (f*1000):%d", (uint32_t)(gain*1000));
    tx_post_gain_set(gain);
}

void audio_datapath_adc_tx_algo_mpdrc_onoff_cmd(const char* param, unsigned int param_len)
{
    uint8_t onoff = 0;

    onoff = atoi(param);
    if (onoff) {
        tx_algo_mpdrc_onoff_set(true);
    } else {
        tx_algo_mpdrc_onoff_set(false);
    }
}

void audio_datapath_adc_tx_algo_iir_onoff_cmd(const char* param, unsigned int param_len)
{
    uint8_t onoff = 0;

    onoff = atoi(param);
    if (onoff) {
        iir_switch(true);
    } else {
        iir_switch(false);
    }
}

void audio_datapath_adc_tx_algo_hdr_mode_cmd(const char* param, unsigned int param_len)
{
    uint8_t mode = 0;

    mode = atoi(param);
    tx_hdr_mode_set(mode);
}
#endif

#elif defined(WIRELESS_MIC_RX)

static void audio_datapath_ble_scan_cmd(const char* param, unsigned int param_len)
{
    uint8_t onoff = 0;

    onoff = atoi(param);
    if (onoff) {
        audio_datapath_ble_enter_scan_to_pairing(true);
    } else {
        audio_datapath_ble_enter_scan_to_pairing(false);
    }
}

static void audio_datapath_ble_dump_dev_info_cmd(const char* param, unsigned int param_len)
{
    audio_datapath_ble_dump_dev_info();
}

static void audio_datapath_manager_adc_input_cmd(const char* param, unsigned int param_len)
{
    uint8_t onoff = 0;

    onoff = atoi(param);
    if (onoff) {
        audio_manager_datapath_set(AUDIO_INTPUT_PATH_BIT_ADC);
    } else {
        audio_manager_datapath_clear(AUDIO_INTPUT_PATH_BIT_ADC);
    }
    LOG_MSG(1, "### amgr adc input = %d", onoff);
}
#endif

const app_wireless_mic_cmd_table_t app_wireless_mic_cmd_table[] =
{
    /***************** tx and rx common dbg cmd start ******************/
    {"app_mode_set",            app_wireless_mic_mode_set_cmd},
    {"app_mode_clear",          app_wireless_mic_mode_clear_cmd},
    {"app_dbg_on",              app_wireless_mic_dbg_onoff_cmd},
#ifdef CFG_APP_DATAPATH_SERVER
    {"app_ble_cmd",             app_wireless_mic_test_ble_send_cmd},
#endif
    /***************** tx and rx common dbg cmd end *******************/

#if defined(WIRELESS_MIC_TX)
    /***************** tx dbg cmd start ************************/
    {"tx_adc_input",            audio_datapath_adc_dummy_test_onoff_cmd},
    {"tx_adc_mute",             audio_datapath_adc_tx_capture_stream_mute_cmd},
#ifdef AUD_OUTPUT_BY_EMMC
    {"tx_emmc_dump",            audio_datapath_emmc_dump_set_mode_cmd},
#endif
#ifdef TX_ALGO_PROCESS
    {"tx_algo_all",             audio_datapath_adc_tx_algo_all_onoff_cmd},
    {"tx_algo_ns",              audio_datapath_adc_tx_algo_ns_onoff_cmd},
    {"tx_algo_sw_pre",          audio_datapath_adc_tx_algo_sw_pre_gain_cmd},
    {"tx_algo_sw_post",         audio_datapath_adc_tx_algo_sw_post_gain_cmd},
    {"tx_algo_mpdrc",           audio_datapath_adc_tx_algo_mpdrc_onoff_cmd},
    {"tx_algo_iir",             audio_datapath_adc_tx_algo_iir_onoff_cmd},
    {"tx_algo_hdr",             audio_datapath_adc_tx_algo_hdr_mode_cmd},
#endif

    /***************** tx dbg cmd end **************************/
#elif defined(WIRELESS_MIC_RX)
    /***************** rx dbg cmd start ************************/
    {"rx_scan_on",              audio_datapath_ble_scan_cmd},
    {"rx_dev_dump",             audio_datapath_ble_dump_dev_info_cmd},
    {"rx_adc_input",            audio_datapath_manager_adc_input_cmd},
    /***************** rx dbg cmd end **************************/
#endif
};

static void app_wireless_mic_cmd_handler(const char* buf, unsigned int length)
{
    char *param_addr = NULL;
    uint32_t param_len = 0;
    const app_wireless_mic_cmd_table_t* table = NULL;

    for (uint32_t i = 0; i < ARRAY_SIZE(app_wireless_mic_cmd_table); i++) {
        table = &app_wireless_mic_cmd_table[i];
        if ((strncmp((char *)buf, table->string, strlen(table->string)) == 0) ||
            strstr(table->string, (char *)buf)) {
            param_addr = strstr((char *)buf, "|");
            if (param_addr != NULL) {
                param_addr++;
                param_len = length - (param_addr - (char *)buf);
            }

            table->cmd_function(param_addr, param_len);
            break;
        }

        if (i == ARRAY_SIZE(app_wireless_mic_cmd_table)) {
            LOG_ERR(0, "ERROR:can not find handle function");
        }
    }
}

uint32_t app_wireless_mic_cmd_callback(unsigned char *buf, unsigned int len)
{
    LOG_MSG(1, "wmic cmd: %s", buf);
    app_wireless_mic_cmd_handler((const char*)buf, strlen((const char*)buf));

    return 0;
}
#endif