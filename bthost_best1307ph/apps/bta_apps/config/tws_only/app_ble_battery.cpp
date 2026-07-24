/***************************************************************************
 *
 * Copyright 2015-2026 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary infomation of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#if defined (BLE_BATT_ENABLE)

#include "cmsis_os2.h"
#include <string.h>

#include "bta_ble_api.h"
#include "bta_gatt_api.h"

#include "bta_tws_ux_api.h"

#include "app_ble_battery.h"

#include "hal_trace.h"

#ifndef TRACE
#define TRACE(attr, str, ...)               TR_INFO(attr, str, ##__VA_ARGS__)
#endif

#define APP_BLE_BATT_GATT_UUID_CCCD         (0x2902)
#define APP_BLE_BATT_GATT_UUID_CPFD         (0x2903)
#define APP_BLE_BATT_SERVICE_UUID           (0x180F)
#define APP_BLE_BATT_NS_BLUETOOTH_SIG       (0x01)
#define APP_BLE_BATT_NS_EXTERNAL            (0x0110)
#define APP_BLE_BATT_NS_SIDE_RIGHT          (0x0800)
#define APP_BLE_BATT_NS_SIDE_LEFT           (0x0400)

typedef enum app_ble_batt_attr_type
{
    APP_BLE_BATT_ATTR_TYPE_SERVICE = 0,

    APP_BLE_BATT_ATTR_TYPE_CHAR_BATT_LEVEL,
    APP_BLE_BATT_ATTR_TYPE_BATT_LEVEL_CCCD,
    APP_BLE_BATT_ATTR_TYPE_BATT_LEVEL_CPFD,

    APP_BLE_BATT_ATTR_TYPE_CHAR_LEVEL_STATUS,
    APP_BLE_BATT_ATTR_TYPE_LEVEL_STATUS_CCCD,

    APP_BLE_BATT_ATTR_TYPE_CHAR_ESTIMATED_SVC_DATE,
    APP_BLE_BATT_ATTR_TYPE_ESTIMATED_SVC_DATE_CCCD,

    APP_BLE_BATT_ATTR_TYPE_CHAR_CRITICAL_STATUS,
    APP_BLE_BATT_ATTR_TYPE_CRITICAL_STATUS_CCCD,

    APP_BLE_BATT_ATTR_TYPE_CHAR_ENERGY_STATUS,
    APP_BLE_BATT_ATTR_TYPE_ENERGY_STATUS_CCCD,

    APP_BLE_BATT_ATTR_TYPE_CHAR_TIME_STATUS,
    APP_BLE_BATT_ATTR_TYPE_TIME_STATUS_CCCD,

    APP_BLE_BATT_ATTR_TYPE_CHAR_HEALTH_STATUS,
    APP_BLE_BATT_ATTR_TYPE_HEALTH_STATUS_CCCD,

    APP_BLE_BATT_ATTR_TYPE_CHAR_HEALTH_INFO,
    APP_BLE_BATT_ATTR_TYPE_HEALTH_INFO_CCCD,

    APP_BLE_BATT_ATTR_TYPE_CHAR_BATT_INFORMATION,
    APP_BLE_BATT_ATTR_TYPE_BATT_INFO_CCCD,

    APP_BLE_BATT_ATTR_TYPE_CHAR_MANUFACTURER_NAME_STRING,
    APP_BLE_BATT_ATTR_TYPE_MANUFACTURER_NAME_STRING_CCCD,

    APP_BLE_BATT_ATTR_TYPE_CHAR_MODEL_NUMBER_STRING,
    APP_BLE_BATT_ATTR_TYPE_MODEL_NUMBER_STRING_CCCD,

    APP_BLE_BATT_ATTR_TYPE_CHAR_SERIAL_NUMBER_STRING,
    APP_BLE_BATT_ATTR_TYPE_SERIAL_NUMBER_STRING_CCCD,

    APP_BLE_BATT_ATTR_TYPE_MAX,
} app_ble_batt_attr_type_e;

static const bt_attr_uuid_t ble_batt_service_uuid_le                = {0x0F, 0x18};
static const bt_attr_uuid_t ble_batt_char_level_uuid_le             = {0x19, 0x2A};
static const bt_attr_uuid_t ble_batt_char_level_status_uuid_le      = {0xED, 0x2B};
static const bt_attr_uuid_t ble_batt_char_est_svc_date_uuid_le      = {0xEF, 0x2B};
static const bt_attr_uuid_t ble_batt_char_critical_status_uuid_le   = {0xE9, 0x2B};
static const bt_attr_uuid_t ble_batt_char_energy_status_le          = {0xF0, 0x2B};
static const bt_attr_uuid_t ble_batt_char_time_status_le            = {0xEE, 0x2B};
static const bt_attr_uuid_t ble_batt_char_health_status_le          = {0xEA, 0x2B};
static const bt_attr_uuid_t ble_batt_char_health_info_le            = {0xEB, 0x2B};
static const bt_attr_uuid_t ble_batt_char_battery_info_le           = {0xEC, 0x2B};
static const bt_attr_uuid_t ble_batt_char_manuf_name_string_le      = {0x29, 0x2A};
static const bt_attr_uuid_t ble_batt_char_model_number_sting_le     = {0x24, 0x2A};
static const bt_attr_uuid_t ble_batt_char_serial_number_sting_le    = {0x25, 0x2A};
static const bt_attr_uuid_t ble_batt_character_cccd_uuid_16_le      = {0x02, 0x29};
static const bt_attr_uuid_t ble_batt_character_cpfd_uuid_16_le      = {0x04, 0x29};

static bt_attr_mem_t ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_MAX] = {{{0}}};

static bt_gatt_attr_t ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_MAX] = {{0}};

typedef struct
{
    uint8_t format;
    uint8_t exponent;
    uint16_t unit;
    uint8_t name_space;
    uint16_t description;
} __attribute__((packed)) batt_attr_cpfd_t;

typedef struct
{
    /// Battery Level
    uint8_t battery_level;
    /// Battery Level Status.
    bas_batt_level_status_t      level_status;
    /// Estimated Service Date.
    bas_estimated_service_date_t estimated_service_date;
    /// Battery Critical Status.
    bas_batt_critical_status_t   critical_status;
    /// Battery Energy Status.
    bas_batt_energy_status_t     energy_status;
    /// Battery Time Status.
    bas_batt_time_status_t       time_status;
    /// Battery Health Status.
    bas_batt_health_status_t     health_status;
    /// Battery Health Information.
    bas_batt_health_info_t       health_info;
    /// Battery Information.
    bas_batt_info_t              info;
    /// Manufacturer Name String
    uint8_t s_mfr_name_val[LEN_MFR_NAME];
    /// Model Number String
    uint8_t s_model_number_val[LEN_MODEL_NUM];
    /// Serial Number String
    uint8_t s_serial_number_val[LEN_SERIAL_NUM];

    uint16_t cccd_batt_level;
    uint16_t cccd_batt_level_status;
    uint16_t cccd_estimated_service_date;
    uint16_t cccd_batt_critical_status;
    uint16_t cccd_batt_energy_status;
    uint16_t cccd_batt_time_status;
    uint16_t cccd_batt_health_status;
    uint16_t cccd_batt_health_info;
    uint16_t cccd_batt_info;
    uint16_t cccd_mfr_name;
    uint16_t cccd_model_number;
    uint16_t cccd_serial_number;

    batt_attr_cpfd_t cpfd_data;
} app_ble_battery_inst_t;

typedef struct
{
    uint8_t battery_level;
    uint8_t name_space;
    uint16_t description;
    bt_gatt_peer_serv_t *peer_service;
    bt_gatt_peer_char_t *peer_battery_level;
} app_ble_peer_batt_service_t;

struct app_ble_batt_environment
{
    uint8_t prf_id;
    app_ble_battery_inst_t instance[BLE_BATTERY_INSTANCE_NUM];
} app_ble_batt_env;

static uint8_t app_ble_batt_get_attr_type_by_attr_ptr(const bt_gatt_attr_t *attr_ptr)
{
    return ((uint32_t)attr_ptr - (uint32_t)&ble_battery_attr_list[0]) / sizeof(bt_gatt_attr_t);
}

static const bt_attr_t *app_ble_batt_get_attribute(app_ble_batt_attr_type_e type)
{
    return ble_battery_attr_list[type].attr_data;
}

static const bt_gatt_attr_t *app_ble_batt_build_attr_list(uint8_t *p_attr_size)
{
    bt_attr_info_t attr_info = {0};

    /* Battery Service */
    attr_info.attr_uuid = ble_batt_service_uuid_le;
    ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_SERVICE] =
        bta_gatts_build_attr(BT_ATTR_TYPE_PRI_SERVICE,
                             &attr_info,
                             &ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_SERVICE]);

    /* Battery Level Characteristic */
    attr_info.attr_prop = BT_GATT_PROP_RD_REQ | BT_GATT_PROP_NTF_PROP;
    attr_info.attr_uuid = ble_batt_char_level_uuid_le;
    ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_CHAR_BATT_LEVEL] =
        bta_gatts_build_attr(BT_ATTR_TYPE_CHARACTER,
                             &attr_info,
                             &ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_CHAR_BATT_LEVEL]);

    /* Battery Level CCCD */
    attr_info.attr_prop = BT_GATT_PROP_WR_REQ | BT_GATT_PROP_RD_REQ;
    attr_info.attr_uuid = ble_batt_character_cccd_uuid_16_le;
    ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_BATT_LEVEL_CCCD] =
        bta_gatts_build_attr(BT_ATTR_TYPE_DESCRIPTOR,
                             &attr_info,
                             &ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_BATT_LEVEL_CCCD]);

    /* Battery Level CPFD */
    attr_info.attr_prop = BT_GATT_PROP_RD_REQ;
    attr_info.attr_uuid = ble_batt_character_cpfd_uuid_16_le;
    ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_BATT_LEVEL_CPFD] =
        bta_gatts_build_attr(BT_ATTR_TYPE_DESCRIPTOR,
                             &attr_info,
                             &ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_BATT_LEVEL_CPFD]);

    /* Battery Level Status Characteristic */
    attr_info.attr_prop = BT_GATT_PROP_RD_REQ | BT_GATT_PROP_NTF_PROP | BT_GATT_PROP_IND_PROP;
    attr_info.attr_uuid = ble_batt_char_level_status_uuid_le;
    ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_CHAR_LEVEL_STATUS] =
        bta_gatts_build_attr(BT_ATTR_TYPE_CHARACTER,
                             &attr_info,
                             &ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_CHAR_LEVEL_STATUS]);

    /* Battery Level Status CCCD */
    attr_info.attr_prop = BT_GATT_PROP_WR_REQ | BT_GATT_PROP_RD_REQ;
    attr_info.attr_uuid = ble_batt_character_cccd_uuid_16_le;
    ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_LEVEL_STATUS_CCCD] =
        bta_gatts_build_attr(BT_ATTR_TYPE_DESCRIPTOR,
                             &attr_info,
                             &ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_LEVEL_STATUS_CCCD]);

    /* Estimated Service Date Characteristic */
    attr_info.attr_prop = BT_GATT_PROP_RD_REQ | BT_GATT_PROP_NTF_PROP | BT_GATT_PROP_IND_PROP;
    attr_info.attr_uuid = ble_batt_char_est_svc_date_uuid_le;
    ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_CHAR_ESTIMATED_SVC_DATE] =
        bta_gatts_build_attr(BT_ATTR_TYPE_CHARACTER,
                             &attr_info,
                             &ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_CHAR_ESTIMATED_SVC_DATE]);

    /* Estimated Service Date CCCD */
    attr_info.attr_prop = BT_GATT_PROP_WR_REQ | BT_GATT_PROP_RD_REQ;
    attr_info.attr_uuid = ble_batt_character_cccd_uuid_16_le;
    ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_ESTIMATED_SVC_DATE_CCCD] =
        bta_gatts_build_attr(BT_ATTR_TYPE_DESCRIPTOR,
                             &attr_info,
                             &ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_ESTIMATED_SVC_DATE_CCCD]);

    /* Critical Status Characteristic */
    attr_info.attr_prop = BT_GATT_PROP_RD_REQ | BT_GATT_PROP_IND_PROP;
    attr_info.attr_uuid = ble_batt_char_critical_status_uuid_le;
    ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_CHAR_CRITICAL_STATUS] =
        bta_gatts_build_attr(BT_ATTR_TYPE_CHARACTER,
                             &attr_info,
                             &ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_CHAR_CRITICAL_STATUS]);

    /* Critical Status CCCD */
    attr_info.attr_prop = BT_GATT_PROP_WR_REQ | BT_GATT_PROP_RD_REQ;
    attr_info.attr_uuid = ble_batt_character_cccd_uuid_16_le;
    ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_CRITICAL_STATUS_CCCD] =
        bta_gatts_build_attr(BT_ATTR_TYPE_DESCRIPTOR,
                             &attr_info,
                             &ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_CRITICAL_STATUS_CCCD]);

    /* Energy Status Characteristic */
    attr_info.attr_prop = BT_GATT_PROP_RD_REQ | BT_GATT_PROP_NTF_PROP | BT_GATT_PROP_IND_PROP;
    attr_info.attr_uuid = ble_batt_char_energy_status_le;
    ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_CHAR_ENERGY_STATUS] =
        bta_gatts_build_attr(BT_ATTR_TYPE_CHARACTER,
                             &attr_info,
                             &ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_CHAR_ENERGY_STATUS]);

    /* Energy Status CCCD */
    attr_info.attr_prop = BT_GATT_PROP_WR_REQ | BT_GATT_PROP_RD_REQ;
    attr_info.attr_uuid = ble_batt_character_cccd_uuid_16_le;
    ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_ENERGY_STATUS_CCCD] =
        bta_gatts_build_attr(BT_ATTR_TYPE_DESCRIPTOR,
                             &attr_info,
                             &ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_ENERGY_STATUS_CCCD]);

    /* Time Status Characteristic */
    attr_info.attr_prop = BT_GATT_PROP_RD_REQ | BT_GATT_PROP_NTF_PROP;
    attr_info.attr_uuid = ble_batt_char_time_status_le;
    ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_CHAR_TIME_STATUS] =
        bta_gatts_build_attr(BT_ATTR_TYPE_CHARACTER,
                             &attr_info,
                             &ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_CHAR_TIME_STATUS]);

    /* Time Status CCCD */
    attr_info.attr_prop = BT_GATT_PROP_WR_REQ | BT_GATT_PROP_RD_REQ;
    attr_info.attr_uuid = ble_batt_character_cccd_uuid_16_le;
    ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_TIME_STATUS_CCCD] =
        bta_gatts_build_attr(BT_ATTR_TYPE_DESCRIPTOR,
                             &attr_info,
                             &ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_TIME_STATUS_CCCD]);

    /* Health Status Characteristic */
    attr_info.attr_prop = BT_GATT_PROP_RD_REQ | BT_GATT_PROP_NTF_PROP | BT_GATT_PROP_IND_PROP;
    attr_info.attr_uuid = ble_batt_char_health_status_le;
    ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_CHAR_HEALTH_STATUS] =
        bta_gatts_build_attr(BT_ATTR_TYPE_CHARACTER,
                             &attr_info,
                             &ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_CHAR_HEALTH_STATUS]);

    /* Health Status CCCD */
    attr_info.attr_prop = BT_GATT_PROP_WR_REQ | BT_GATT_PROP_RD_REQ;
    attr_info.attr_uuid = ble_batt_character_cccd_uuid_16_le;
    ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_HEALTH_STATUS_CCCD] =
        bta_gatts_build_attr(BT_ATTR_TYPE_DESCRIPTOR,
                             &attr_info,
                             &ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_HEALTH_STATUS_CCCD]);

    /* Health Information Characteristic */
    attr_info.attr_prop = BT_GATT_PROP_RD_REQ | BT_GATT_PROP_IND_PROP;
    attr_info.attr_uuid = ble_batt_char_health_info_le;
    ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_CHAR_HEALTH_INFO] =
        bta_gatts_build_attr(BT_ATTR_TYPE_CHARACTER,
                             &attr_info,
                             &ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_CHAR_HEALTH_INFO]);

    /* Health Information CCCD */
    attr_info.attr_prop = BT_GATT_PROP_WR_REQ | BT_GATT_PROP_RD_REQ;
    attr_info.attr_uuid = ble_batt_character_cccd_uuid_16_le;
    ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_HEALTH_INFO_CCCD] =
        bta_gatts_build_attr(BT_ATTR_TYPE_DESCRIPTOR,
                             &attr_info,
                             &ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_HEALTH_INFO_CCCD]);

    /* Battery Information Characteristic */
    attr_info.attr_prop = BT_GATT_PROP_RD_REQ | BT_GATT_PROP_IND_PROP;
    attr_info.attr_uuid = ble_batt_char_battery_info_le;
    ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_CHAR_BATT_INFORMATION] =
        bta_gatts_build_attr(BT_ATTR_TYPE_CHARACTER,
                             &attr_info,
                             &ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_CHAR_BATT_INFORMATION]);

    /* Battery Information CCCD */
    attr_info.attr_prop = BT_GATT_PROP_WR_REQ | BT_GATT_PROP_RD_REQ;
    attr_info.attr_uuid = ble_batt_character_cccd_uuid_16_le;
    ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_BATT_INFO_CCCD] =
        bta_gatts_build_attr(BT_ATTR_TYPE_DESCRIPTOR,
                             &attr_info,
                             &ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_BATT_INFO_CCCD]);

    /* Manufacturer Name String Characteristic */
    attr_info.attr_prop = BT_GATT_PROP_RD_REQ | BT_GATT_PROP_IND_PROP;
    attr_info.attr_uuid = ble_batt_char_manuf_name_string_le;
    ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_CHAR_MANUFACTURER_NAME_STRING] =
        bta_gatts_build_attr(BT_ATTR_TYPE_CHARACTER,
                             &attr_info,
                             &ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_CHAR_MANUFACTURER_NAME_STRING]);

    /* Manufacturer Name String CCCD */
    attr_info.attr_prop = BT_GATT_PROP_WR_REQ | BT_GATT_PROP_RD_REQ;
    attr_info.attr_uuid = ble_batt_character_cccd_uuid_16_le;
    ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_MANUFACTURER_NAME_STRING_CCCD] =
        bta_gatts_build_attr(BT_ATTR_TYPE_DESCRIPTOR,
                             &attr_info,
                             &ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_MANUFACTURER_NAME_STRING_CCCD]);

    /* Model Number String Characteristic */
    attr_info.attr_prop = BT_GATT_PROP_RD_REQ | BT_GATT_PROP_IND_PROP;
    attr_info.attr_uuid = ble_batt_char_model_number_sting_le;
    ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_CHAR_MODEL_NUMBER_STRING] =
        bta_gatts_build_attr(BT_ATTR_TYPE_CHARACTER,
                             &attr_info,
                             &ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_CHAR_MODEL_NUMBER_STRING]);

    /* Model Number String CCCD */
    attr_info.attr_prop = BT_GATT_PROP_WR_REQ | BT_GATT_PROP_RD_REQ;
    attr_info.attr_uuid = ble_batt_character_cccd_uuid_16_le;
    ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_MODEL_NUMBER_STRING_CCCD] =
        bta_gatts_build_attr(BT_ATTR_TYPE_DESCRIPTOR,
                             &attr_info,
                             &ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_MODEL_NUMBER_STRING_CCCD]);

    /* Serial Number String Characteristic */
    attr_info.attr_prop = BT_GATT_PROP_RD_REQ | BT_GATT_PROP_IND_PROP;
    attr_info.attr_uuid = ble_batt_char_serial_number_sting_le;
    ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_CHAR_SERIAL_NUMBER_STRING] =
        bta_gatts_build_attr(BT_ATTR_TYPE_CHARACTER,
                             &attr_info,
                             &ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_CHAR_SERIAL_NUMBER_STRING]);

    /* Serial Number String CCCD */
    attr_info.attr_prop = BT_GATT_PROP_WR_REQ | BT_GATT_PROP_RD_REQ;
    attr_info.attr_uuid = ble_batt_character_cccd_uuid_16_le;
    ble_battery_attr_list[APP_BLE_BATT_ATTR_TYPE_SERIAL_NUMBER_STRING_CCCD] =
        bta_gatts_build_attr(BT_ATTR_TYPE_DESCRIPTOR,
                             &attr_info,
                             &ble_battery_attr_mem[APP_BLE_BATT_ATTR_TYPE_SERIAL_NUMBER_STRING_CCCD]);

    *p_attr_size = APP_BLE_BATT_ATTR_TYPE_MAX;

    return ble_battery_attr_list;
}

static bool app_ble_batt_server_callback(bt_gatt_svc_t *svc, bt_gatts_event_t event, bt_gatts_callback_param_t param)
{
    app_ble_battery_inst_t *inst = NULL;

    inst = app_ble_batt_env.instance + svc->service_inst_id;

    switch (event)
    {
        case BT_GATTS_EVENT_CHAR_READ:
        {
            bt_gatts_char_read_t *p = param.char_read;

            uint8_t attr_type = app_ble_batt_get_attr_type_by_attr_ptr(p->char_attr);
            if (attr_type == APP_BLE_BATT_ATTR_TYPE_CHAR_BATT_LEVEL)
            {
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0, (uint8_t *)&inst->battery_level, sizeof(uint8_t));
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_CHAR_LEVEL_STATUS)
            {
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0,
                                        (uint8_t *)&inst->level_status, sizeof(bas_batt_level_status_t));
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_CHAR_ESTIMATED_SVC_DATE)
            {
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0,
                                        (uint8_t *)&inst->estimated_service_date, sizeof(bas_estimated_service_date_t));
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_CHAR_CRITICAL_STATUS)
            {
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0,
                                        (uint8_t *)&inst->critical_status, sizeof(bas_batt_critical_status_t));
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_CHAR_ENERGY_STATUS)
            {
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0,
                                        (uint8_t *)&inst->energy_status + p->value_offset, sizeof(bas_batt_energy_status_t) - p->value_offset);
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_CHAR_TIME_STATUS)
            {
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0,
                                        (uint8_t *)&inst->time_status, sizeof(bas_batt_time_status_t));
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_CHAR_HEALTH_STATUS)
            {
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0,
                                        (uint8_t *)&inst->health_status, sizeof(bas_batt_health_status_t));
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_CHAR_HEALTH_INFO)
            {
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0,
                                        (uint8_t *)&inst->health_info + p->value_offset, sizeof(bas_batt_health_info_t) - p->value_offset);
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_CHAR_BATT_INFORMATION)
            {
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0,
                                        (uint8_t *)&inst->info  + p->value_offset, sizeof(bas_batt_info_t) - p->value_offset);
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_CHAR_MANUFACTURER_NAME_STRING)
            {
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0,
                                        (uint8_t *)inst->s_mfr_name_val + p->value_offset, LEN_MFR_NAME - p->value_offset);
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_CHAR_MODEL_NUMBER_STRING)
            {
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0,
                                        (uint8_t *)inst->s_model_number_val + p->value_offset, LEN_MODEL_NUM - p->value_offset);
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_CHAR_SERIAL_NUMBER_STRING)
            {
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0,
                                        (uint8_t *)inst->s_serial_number_val + p->value_offset, LEN_SERIAL_NUM - p->value_offset);
                return true;
            }
            break;
        }
        case BT_GATTS_EVENT_DESC_READ:
        {
            bt_gatts_desc_read_t *p = param.desc_read;

            uint8_t attr_type = app_ble_batt_get_attr_type_by_attr_ptr(p->desc_attr);

            if (attr_type == APP_BLE_BATT_ATTR_TYPE_BATT_LEVEL_CPFD)
            {
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0, (uint8_t *)&inst->cpfd_data, sizeof(batt_attr_cpfd_t));
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_BATT_LEVEL_CCCD)
            {
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0,
                                        (uint8_t *)&inst->cccd_batt_level, sizeof(inst->cccd_batt_level));
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_LEVEL_STATUS_CCCD)
            {
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0,
                                        (uint8_t *)&inst->cccd_batt_level_status, sizeof(inst->cccd_batt_level_status));
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_ESTIMATED_SVC_DATE_CCCD)
            {
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0,
                                        (uint8_t *)&inst->cccd_estimated_service_date, sizeof(inst->cccd_estimated_service_date));
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_CRITICAL_STATUS_CCCD)
            {
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0,
                                        (uint8_t *)&inst->cccd_batt_critical_status, sizeof(inst->cccd_batt_critical_status));
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_ENERGY_STATUS_CCCD)
            {
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0,
                                        (uint8_t *)&inst->cccd_batt_energy_status, sizeof(inst->cccd_batt_energy_status));
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_TIME_STATUS_CCCD)
            {
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0,
                                        (uint8_t *)&inst->cccd_batt_time_status, sizeof(inst->cccd_batt_time_status));
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_HEALTH_STATUS_CCCD)
            {
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0,
                                        (uint8_t *)&inst->cccd_batt_health_status, sizeof(inst->cccd_batt_health_status));
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_HEALTH_INFO_CCCD)
            {
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0,
                                        (uint8_t *)&inst->cccd_batt_health_info, sizeof(inst->cccd_batt_health_info));
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_BATT_INFO_CCCD)
            {
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0,
                                        (uint8_t *)&inst->cccd_batt_info, sizeof(inst->cccd_batt_info));
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_MANUFACTURER_NAME_STRING_CCCD)
            {
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0,
                                        (uint8_t *)&inst->cccd_mfr_name, sizeof(inst->cccd_mfr_name));
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_MODEL_NUMBER_STRING_CCCD)
            {
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0,
                                        (uint8_t *)&inst->cccd_model_number, sizeof(inst->cccd_model_number));
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_SERIAL_NUMBER_STRING_CCCD)
            {
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0,
                                        (uint8_t *)&inst->cccd_serial_number, sizeof(inst->cccd_serial_number));
                return true;
            }
            break;
        }
        case BT_GATTS_EVENT_DESC_WRITE:
        {
            bt_gatts_desc_write_t *p = param.desc_write;

            if (p->value_len != sizeof(uint16_t))
            {
                bta_gatts_send_write_rsp(svc->connhdl, p->token, BT_ATT_ERR_INVALID_ATTR_VALUE_LEN);
                return true;
            }

            uint8_t attr_type = app_ble_batt_get_attr_type_by_attr_ptr(p->desc_attr);

            uint16_t new_cccd = 0;
            new_cccd = (uint16_t)p->value[0] | ((uint16_t)p->value[1] << 8);

            if (attr_type == APP_BLE_BATT_ATTR_TYPE_BATT_LEVEL_CCCD)
            {
                inst->cccd_batt_level = new_cccd;
                bta_gatts_send_write_rsp(svc->connhdl, p->token, 0);
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_LEVEL_STATUS_CCCD)
            {
                inst->cccd_batt_level_status = new_cccd;
                bta_gatts_send_write_rsp(svc->connhdl, p->token, 0);
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_ESTIMATED_SVC_DATE_CCCD)
            {
                inst->cccd_estimated_service_date = new_cccd;
                bta_gatts_send_write_rsp(svc->connhdl, p->token, 0);
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_CRITICAL_STATUS_CCCD)
            {
                inst->cccd_batt_critical_status = new_cccd;
                bta_gatts_send_write_rsp(svc->connhdl, p->token, 0);
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_ENERGY_STATUS_CCCD)
            {
                inst->cccd_batt_energy_status = new_cccd;
                bta_gatts_send_write_rsp(svc->connhdl, p->token, 0);
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_TIME_STATUS_CCCD)
            {
                inst->cccd_batt_time_status = new_cccd;
                bta_gatts_send_write_rsp(svc->connhdl, p->token, 0);
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_HEALTH_STATUS_CCCD)
            {
                inst->cccd_batt_health_status = new_cccd;
                bta_gatts_send_write_rsp(svc->connhdl, p->token, 0);
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_HEALTH_INFO_CCCD)
            {
                inst->cccd_batt_health_info = new_cccd;
                bta_gatts_send_write_rsp(svc->connhdl, p->token, 0);
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_BATT_INFO_CCCD)
            {
                inst->cccd_batt_info = new_cccd;
                bta_gatts_send_write_rsp(svc->connhdl, p->token, 0);
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_MANUFACTURER_NAME_STRING_CCCD)
            {
                inst->cccd_mfr_name = new_cccd;
                bta_gatts_send_write_rsp(svc->connhdl, p->token, 0);
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_MODEL_NUMBER_STRING_CCCD)
            {
                inst->cccd_model_number = new_cccd;
                bta_gatts_send_write_rsp(svc->connhdl, p->token, 0);
                return true;
            }
            else if (attr_type == APP_BLE_BATT_ATTR_TYPE_SERIAL_NUMBER_STRING_CCCD)
            {
                inst->cccd_serial_number = new_cccd;
                bta_gatts_send_write_rsp(svc->connhdl, p->token, 0);
                return true;
            }

            bta_gatts_send_write_rsp(svc->connhdl, p->token, BT_ATT_ERR_ATTRIBUTE_NOT_FOUND);
            return true;
        }
        default:
        {
            break;
        }
    }

    return false;
}

static int app_ble_battery_send_notify_to_all(bool is_ntf, const bt_gatt_char_notify_t *notify, const uint8_t *data, uint16_t len)
{
    uint8_t conidx = 0;
    uint16_t connhdl = BT_INVALID_HANDLE;

    for (conidx = 0; conidx < BLE_CONNECTION_MAX; conidx++)
    {
        connhdl = bta_ble_get_conhdl_by_conidx(conidx);

        if (connhdl == BT_INVALID_HANDLE)
        {
            continue;
        }

        bta_gatts_send_character_value(connhdl, is_ntf, notify, data, len);
    }

    return 0;
}

int app_ble_report_battery_level(uint8_t instance, uint8_t battery_level)
{
    bt_gatt_char_notify_t notify = {NULL};
    app_ble_battery_inst_t *inst = NULL;

    if (instance >= BLE_BATTERY_INSTANCE_NUM)
    {
        return -1;
    }

    notify.service = app_ble_batt_get_attribute(APP_BLE_BATT_ATTR_TYPE_SERVICE);
    notify.character = app_ble_batt_get_attribute(APP_BLE_BATT_ATTR_TYPE_CHAR_BATT_LEVEL);
    notify.service_inst_id = instance;

    if (battery_level > 100)
    {
        battery_level = 100;
    }

    inst = app_ble_batt_env.instance + instance;
    inst->battery_level = battery_level;

    return app_ble_battery_send_notify_to_all(true, &notify, &battery_level, sizeof(uint8_t));
}

int app_ble_report_batt_level_status(uint8_t instance, bas_batt_level_status_t level_status, uint8_t send_mode)
{
    bt_gatt_char_notify_t notify = {NULL};
    app_ble_battery_inst_t *inst;

    if (instance >= BLE_BATTERY_INSTANCE_NUM)
    {
        return -1;
    }

    notify.service = app_ble_batt_get_attribute(APP_BLE_BATT_ATTR_TYPE_SERVICE);
    notify.character = app_ble_batt_get_attribute(APP_BLE_BATT_ATTR_TYPE_CHAR_LEVEL_STATUS);
    notify.service_inst_id = instance;

    inst = app_ble_batt_env.instance + instance;
    inst->level_status = level_status;

    return app_ble_battery_send_notify_to_all(send_mode == SEND_MODE_NOTIFY, &notify, (uint8_t *)&level_status, sizeof(bas_batt_level_status_t));
}

int app_ble_report_estimated_service_date(uint8_t instance, bas_estimated_service_date_t date, uint8_t send_mode)
{
    bt_gatt_char_notify_t notify = {NULL};
    app_ble_battery_inst_t *inst;

    if (instance >= BLE_BATTERY_INSTANCE_NUM)
    {
        return -1;
    }

    notify.service = app_ble_batt_get_attribute(APP_BLE_BATT_ATTR_TYPE_SERVICE);
    notify.character = app_ble_batt_get_attribute(APP_BLE_BATT_ATTR_TYPE_CHAR_ESTIMATED_SVC_DATE);
    notify.service_inst_id = instance;

    inst = app_ble_batt_env.instance + instance;
    inst->estimated_service_date = date;

    return app_ble_battery_send_notify_to_all(send_mode == SEND_MODE_NOTIFY, &notify, (uint8_t *)&date, sizeof(bas_estimated_service_date_t));
}

int app_ble_report_batt_critical_status(uint8_t instance, bas_batt_critical_status_t critical)
{
    bt_gatt_char_notify_t notify = {NULL};
    app_ble_battery_inst_t *inst;

    if (instance >= BLE_BATTERY_INSTANCE_NUM)
    {
        return -1;
    }

    notify.service = app_ble_batt_get_attribute(APP_BLE_BATT_ATTR_TYPE_SERVICE);
    notify.character = app_ble_batt_get_attribute(APP_BLE_BATT_ATTR_TYPE_CHAR_CRITICAL_STATUS);
    notify.service_inst_id = instance;

    inst = app_ble_batt_env.instance + instance;
    inst->critical_status = critical;

    return app_ble_battery_send_notify_to_all(false, &notify, (uint8_t *)&critical, sizeof(bas_batt_critical_status_t));
}

int app_ble_report_batt_energy_status(uint8_t instance, bas_batt_energy_status_t energy, uint8_t send_mode)
{
    bt_gatt_char_notify_t notify = {NULL};
    app_ble_battery_inst_t *inst;

    if (instance >= BLE_BATTERY_INSTANCE_NUM)
    {
        return -1;
    }

    notify.service = app_ble_batt_get_attribute(APP_BLE_BATT_ATTR_TYPE_SERVICE);
    notify.character = app_ble_batt_get_attribute(APP_BLE_BATT_ATTR_TYPE_CHAR_ENERGY_STATUS);
    notify.service_inst_id = instance;

    inst = app_ble_batt_env.instance + instance;
    inst->energy_status = energy;

    return app_ble_battery_send_notify_to_all(send_mode == SEND_MODE_NOTIFY, &notify, (uint8_t *)&energy, sizeof(bas_batt_energy_status_t));
}

int app_ble_report_batt_time_status(uint8_t instance, bas_batt_time_status_t time_status, uint8_t send_mode)
{
    bt_gatt_char_notify_t notify = {NULL};
    app_ble_battery_inst_t *inst;

    if (instance >= BLE_BATTERY_INSTANCE_NUM)
    {
        return -1;
    }

    notify.service = app_ble_batt_get_attribute(APP_BLE_BATT_ATTR_TYPE_SERVICE);
    notify.character = app_ble_batt_get_attribute(APP_BLE_BATT_ATTR_TYPE_CHAR_TIME_STATUS);
    notify.service_inst_id = instance;

    inst = app_ble_batt_env.instance + instance;
    inst->time_status = time_status;

    return app_ble_battery_send_notify_to_all(send_mode == SEND_MODE_NOTIFY, &notify, (uint8_t *)&time_status, sizeof(bas_batt_time_status_t));
}

int app_ble_report_batt_health_status(uint8_t instance, bas_batt_health_status_t health_status, uint8_t send_mode)
{
    bt_gatt_char_notify_t notify = {NULL};
    app_ble_battery_inst_t *inst;

    if (instance >= BLE_BATTERY_INSTANCE_NUM)
    {
        return -1;
    }

    notify.service = app_ble_batt_get_attribute(APP_BLE_BATT_ATTR_TYPE_SERVICE);
    notify.character = app_ble_batt_get_attribute(APP_BLE_BATT_ATTR_TYPE_CHAR_HEALTH_STATUS);
    notify.service_inst_id = instance;

    inst = app_ble_batt_env.instance + instance;
    inst->health_status = health_status;

    return app_ble_battery_send_notify_to_all(send_mode == SEND_MODE_NOTIFY, &notify, (uint8_t *)&health_status, sizeof(bas_batt_health_status_t));
}

int app_ble_report_batt_health_info(uint8_t instance, bas_batt_health_info_t health_info)
{
    bt_gatt_char_notify_t notify = {NULL};
    app_ble_battery_inst_t *inst;

    if (instance >= BLE_BATTERY_INSTANCE_NUM)
    {
        return -1;
    }

    notify.service = app_ble_batt_get_attribute(APP_BLE_BATT_ATTR_TYPE_SERVICE);
    notify.character = app_ble_batt_get_attribute(APP_BLE_BATT_ATTR_TYPE_CHAR_HEALTH_INFO);
    notify.service_inst_id = instance;

    inst = app_ble_batt_env.instance + instance;
    inst->health_info = health_info;

    return app_ble_battery_send_notify_to_all(false, &notify, (uint8_t *)&health_info, sizeof(bas_batt_health_info_t));
}

int app_ble_report_batt_info(uint8_t instance, bas_batt_info_t info)
{
    bt_gatt_char_notify_t notify = {NULL};
    app_ble_battery_inst_t *inst;

    if (instance >= BLE_BATTERY_INSTANCE_NUM)
    {
        return -1;
    }

    notify.service = app_ble_batt_get_attribute(APP_BLE_BATT_ATTR_TYPE_SERVICE);
    notify.character = app_ble_batt_get_attribute(APP_BLE_BATT_ATTR_TYPE_CHAR_BATT_INFORMATION);
    notify.service_inst_id = instance;

    inst = app_ble_batt_env.instance + instance;
    inst->info = info;

    return app_ble_battery_send_notify_to_all(false, &notify, (uint8_t *)&info, sizeof(bas_batt_info_t));
}

int app_ble_report_mfr_name(uint8_t instance, uint8_t *name, uint8_t len)
{
    bt_gatt_char_notify_t notify = {NULL};
    app_ble_battery_inst_t *inst;

    if (instance >= BLE_BATTERY_INSTANCE_NUM)
    {
        return -1;
    }

    notify.service = app_ble_batt_get_attribute(APP_BLE_BATT_ATTR_TYPE_SERVICE);
    notify.character = app_ble_batt_get_attribute(APP_BLE_BATT_ATTR_TYPE_CHAR_MANUFACTURER_NAME_STRING);
    notify.service_inst_id = instance;

    inst = app_ble_batt_env.instance + instance;
    memcpy(inst->s_mfr_name_val, name, (len > LEN_MFR_NAME) ? LEN_MFR_NAME : len);

    return app_ble_battery_send_notify_to_all(false, &notify, name, len);
}

int app_ble_report_model_number(uint8_t instance, uint8_t *model_number, uint8_t len)
{
    bt_gatt_char_notify_t notify = {NULL};
    app_ble_battery_inst_t *inst;

    if (instance >= BLE_BATTERY_INSTANCE_NUM)
    {
        return -1;
    }

    notify.service = app_ble_batt_get_attribute(APP_BLE_BATT_ATTR_TYPE_SERVICE);
    notify.character = app_ble_batt_get_attribute(APP_BLE_BATT_ATTR_TYPE_CHAR_MODEL_NUMBER_STRING);
    notify.service_inst_id = instance;

    inst = app_ble_batt_env.instance + instance;
    memcpy(inst->s_model_number_val, model_number, (len > LEN_MODEL_NUM) ? LEN_MODEL_NUM : len);

    return app_ble_battery_send_notify_to_all(false, &notify, model_number, len);
}

int app_ble_report_serial_number(uint8_t instance, uint8_t *serial_number, uint8_t len)
{
    bt_gatt_char_notify_t notify = {NULL};
    app_ble_battery_inst_t *inst;

    if (instance >= BLE_BATTERY_INSTANCE_NUM)
    {
        return -1;
    }

    notify.service = app_ble_batt_get_attribute(APP_BLE_BATT_ATTR_TYPE_SERVICE);
    notify.character = app_ble_batt_get_attribute(APP_BLE_BATT_ATTR_TYPE_CHAR_SERIAL_NUMBER_STRING);
    notify.service_inst_id = instance;

    inst = app_ble_batt_env.instance + instance;
    memcpy(inst->s_serial_number_val, serial_number, (len > LEN_SERIAL_NUM) ? LEN_SERIAL_NUM : len);

    return app_ble_battery_send_notify_to_all(false, &notify, serial_number, len);
}

void app_ble_batt_init(void)
{
    app_ble_battery_inst_t *inst = NULL;
    bt_gatts_cfg_t cfg = {0};
    int i = 0;
    uint8_t attr_size = 0;
    bool is_left = (BT_IBRT_SLAVE == bta_tws_get_nv_role());

    memset(&app_ble_batt_env, 0, sizeof(app_ble_batt_env));

    TRACE(0, "batt init: mode %d instance %d", is_left, BLE_BATTERY_INSTANCE_NUM);

    for (; i < BLE_BATTERY_INSTANCE_NUM; i += 1)
    {
        cfg.service_inst_id = i;
        inst = app_ble_batt_env.instance + i;
        memset(inst, 0, sizeof(app_ble_battery_inst_t));

        inst->battery_level = 100;
        memcpy(inst->s_mfr_name_val, MANUFACTURER_NAME,
               (sizeof(MANUFACTURER_NAME) > LEN_MFR_NAME) ? LEN_MFR_NAME : sizeof(MANUFACTURER_NAME));
        memcpy(inst->s_model_number_val, MODEL_NUMBER,
               (sizeof(MODEL_NUMBER) > LEN_MODEL_NUM) ? LEN_MODEL_NUM : sizeof(MODEL_NUMBER));
        memcpy(inst->s_serial_number_val, SERIAL_NUMBER,
               (sizeof(SERIAL_NUMBER) > LEN_SERIAL_NUM) ? LEN_SERIAL_NUM : sizeof(SERIAL_NUMBER));
        inst->cpfd_data.name_space = APP_BLE_BATT_NS_BLUETOOTH_SIG;

        if (i == 0)
        {
            inst->cpfd_data.description = APP_BLE_BATT_NS_EXTERNAL;
        }
        else if (i == 1)
        {
            // mode 0: right;   mode 1: left
            if (is_left == false)
            {
                // Audio Location Definitions, side right
                inst->cpfd_data.description = APP_BLE_BATT_NS_SIDE_RIGHT;
            }
            else
            {
                // Audio Location Definitions, side left
                inst->cpfd_data.description = APP_BLE_BATT_NS_SIDE_LEFT;
            }
        }

        bta_gatts_register_service(app_ble_batt_build_attr_list(&attr_size), attr_size, app_ble_batt_server_callback, &cfg);
    }
}

void app_ble_batt_deinit(void)
{
    bta_gatts_unregister_service(ble_battery_attr_list);

    memset(&app_ble_batt_env.instance, 0, sizeof(app_ble_batt_env.instance));
}

#endif /* BLE_BATT_ENABLE */
