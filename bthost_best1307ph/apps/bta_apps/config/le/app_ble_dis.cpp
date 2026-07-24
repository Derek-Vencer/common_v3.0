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
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#if defined(BLE_DIP_ENABLE)

#include "cmsis_os2.h"
#include <string.h>

#include "bta_ble_api.h"
#include "bta_gatt_api.h"

#include "app_ble_dis.h"

#include "hal_trace.h"

#ifndef TRACE
#define TRACE(attr, str, ...)               TR_INFO(attr, str, ##__VA_ARGS__)
#endif

#define MANUFACTURER_NAME                   "DEMO"
#define MODEL_NUMBER_STRING                 "0000"
#define SERIAL_NUMBER_STRING                "0000"
#define HARDWARE_REVISION_STRING            "0000"
#define FIRMWARE_REVISION_STRING            "0000"
#define SOFTWARE_REVISION_STRING            "0000"
#define SYSTEM_ID_STRING                    "0000"

#define PNP_VID_SOURCE                      (0x01)
#define PNP_VENDOR_ID                       (0x02B0)
#define PNP_PRODUCT_ID                      (0x0000)
#define PNP_PRODUCT_VER                     (0x001F)

typedef enum app_ble_dis_attr_type
{
    APP_BLE_DIS_ATTR_TYPE_SERVICE = 0,

    APP_BLE_DIS_ATTR_TYPE_CHAR_MANUF_NAME,
    APP_BLE_DIS_ATTR_TYPE_CHAR_MODEL_NUMBER,
    APP_BLE_DIS_ATTR_TYPE_CHAR_SERIAL_NUMBER,
    APP_BLE_DIS_ATTR_TYPE_CHAR_HW_REVISON,
    APP_BLE_DIS_ATTR_TYPE_CHAR_FW_REVISON,
    APP_BLE_DIS_ATTR_TYPE_CHAR_SW_REVISON,
    APP_BLE_DIS_ATTR_TYPE_CHAR_SYSTEM_ID,
    APP_BLE_DIS_ATTR_TYPE_CHAR_PNP_INFO,

    APP_BLE_DIS_ATTR_TYPE_MAX,
} app_ble_dis_attr_type_e;

typedef struct
{
    uint8_t vendor_id_source; // 0x01 bluetooth sig
    uint16_t vendor_id;
    uint16_t product_id;
    uint16_t product_version;
} __attribute__((packed)) app_ble_dis_pnp_info_t;

static const bt_attr_uuid_t ble_dip_service_uuid_le             = {0x0A, 0x18};
static const bt_attr_uuid_t ble_dip_char_manufacturer_name_le   = {0x29, 0x2A};
static const bt_attr_uuid_t ble_dip_char_model_number_le        = {0x24, 0x2A};
static const bt_attr_uuid_t ble_dip_char_serial_number_le       = {0x25, 0x2A};
static const bt_attr_uuid_t ble_dip_char_hw_revision_le         = {0x27, 0x2A};
static const bt_attr_uuid_t ble_dip_char_fw_revision_le         = {0x26, 0x2A};
static const bt_attr_uuid_t ble_dip_char_sw_revision_le         = {0x28, 0x2A};
static const bt_attr_uuid_t ble_dip_char_system_id_le           = {0x23, 0x2A};
static const bt_attr_uuid_t ble_dip_char_pnp_info_le            = {0x50, 0x2A};

static bt_attr_mem_t ble_dis_attr_mem[APP_BLE_DIS_ATTR_TYPE_MAX] = {{{0}}};

static bt_gatt_attr_t ble_dis_attr_list[APP_BLE_DIS_ATTR_TYPE_MAX] = {{0}};

static const bt_gatt_attr_t *app_ble_dis_build_attr_list(uint8_t *p_attr_size)
{
    bt_attr_info_t attr_info = {0};

    /* Device Info Service */
    attr_info.attr_uuid = ble_dip_service_uuid_le;
    ble_dis_attr_list[APP_BLE_DIS_ATTR_TYPE_SERVICE] =
        bta_gatts_build_attr(BT_ATTR_TYPE_PRI_SERVICE,
                             &attr_info,
                             &ble_dis_attr_mem[APP_BLE_DIS_ATTR_TYPE_SERVICE]);

    attr_info.attr_prop = BT_GATT_PROP_RD_REQ;

    attr_info.attr_uuid = ble_dip_char_manufacturer_name_le;
    ble_dis_attr_list[APP_BLE_DIS_ATTR_TYPE_CHAR_MANUF_NAME] =
        bta_gatts_build_attr(BT_ATTR_TYPE_CHARACTER,
                             &attr_info,
                             &ble_dis_attr_mem[APP_BLE_DIS_ATTR_TYPE_CHAR_MANUF_NAME]);

    attr_info.attr_uuid = ble_dip_char_model_number_le;
    ble_dis_attr_list[APP_BLE_DIS_ATTR_TYPE_CHAR_MODEL_NUMBER] =
        bta_gatts_build_attr(BT_ATTR_TYPE_CHARACTER,
                             &attr_info,
                             &ble_dis_attr_mem[APP_BLE_DIS_ATTR_TYPE_CHAR_MODEL_NUMBER]);

    attr_info.attr_uuid = ble_dip_char_serial_number_le;
    ble_dis_attr_list[APP_BLE_DIS_ATTR_TYPE_CHAR_SERIAL_NUMBER] =
        bta_gatts_build_attr(BT_ATTR_TYPE_CHARACTER,
                             &attr_info,
                             &ble_dis_attr_mem[APP_BLE_DIS_ATTR_TYPE_CHAR_SERIAL_NUMBER]);

    attr_info.attr_uuid = ble_dip_char_hw_revision_le;
    ble_dis_attr_list[APP_BLE_DIS_ATTR_TYPE_CHAR_HW_REVISON] =
        bta_gatts_build_attr(BT_ATTR_TYPE_CHARACTER,
                             &attr_info,
                             &ble_dis_attr_mem[APP_BLE_DIS_ATTR_TYPE_CHAR_HW_REVISON]);

    attr_info.attr_uuid = ble_dip_char_fw_revision_le;
    ble_dis_attr_list[APP_BLE_DIS_ATTR_TYPE_CHAR_FW_REVISON] =
        bta_gatts_build_attr(BT_ATTR_TYPE_CHARACTER,
                             &attr_info,
                             &ble_dis_attr_mem[APP_BLE_DIS_ATTR_TYPE_CHAR_FW_REVISON]);

    attr_info.attr_uuid = ble_dip_char_sw_revision_le;
    ble_dis_attr_list[APP_BLE_DIS_ATTR_TYPE_CHAR_SW_REVISON] =
        bta_gatts_build_attr(BT_ATTR_TYPE_CHARACTER,
                             &attr_info,
                             &ble_dis_attr_mem[APP_BLE_DIS_ATTR_TYPE_CHAR_SW_REVISON]);

    attr_info.attr_uuid = ble_dip_char_system_id_le;
    ble_dis_attr_list[APP_BLE_DIS_ATTR_TYPE_CHAR_SYSTEM_ID] =
        bta_gatts_build_attr(BT_ATTR_TYPE_CHARACTER,
                             &attr_info,
                             &ble_dis_attr_mem[APP_BLE_DIS_ATTR_TYPE_CHAR_SYSTEM_ID]);

    attr_info.attr_uuid = ble_dip_char_pnp_info_le;
    ble_dis_attr_list[APP_BLE_DIS_ATTR_TYPE_CHAR_PNP_INFO] =
        bta_gatts_build_attr(BT_ATTR_TYPE_CHARACTER,
                             &attr_info,
                             &ble_dis_attr_mem[APP_BLE_DIS_ATTR_TYPE_CHAR_PNP_INFO]);

    *p_attr_size = APP_BLE_DIS_ATTR_TYPE_MAX;

    return ble_dis_attr_list;
}

static uint8_t app_ble_dis_get_attr_type_by_attr_ptr(const bt_gatt_attr_t *attr_ptr)
{
    return ((uint32_t)attr_ptr - (uint32_t)&ble_dis_attr_list[0]) / sizeof(bt_gatt_attr_t);
}

static bool app_ble_dip_server_callback(bt_gatt_svc_t *svc, bt_gatts_event_t event, bt_gatts_callback_param_t param)
{
    switch (event)
    {
        case BT_GATTS_EVENT_CHAR_READ:
        {
            bt_gatts_char_read_t *p = param.char_read;
            uint8_t attr_type = app_ble_dis_get_attr_type_by_attr_ptr(p->char_attr);

            if (attr_type == APP_BLE_DIS_ATTR_TYPE_CHAR_MANUF_NAME)
            {
                char manufacturer_name[] = {MANUFACTURER_NAME};
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0, (uint8_t *)manufacturer_name, strlen(manufacturer_name));
                return true;
            }
            else if (attr_type == APP_BLE_DIS_ATTR_TYPE_CHAR_MODEL_NUMBER)
            {
                char model_number[] = {MODEL_NUMBER_STRING};
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0, (uint8_t *)model_number, strlen(model_number));
                return true;
            }
            else if (attr_type == APP_BLE_DIS_ATTR_TYPE_CHAR_SERIAL_NUMBER)
            {
                char serial_number[] = {SERIAL_NUMBER_STRING};
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0, (uint8_t *)serial_number, strlen(serial_number));
                return true;
            }
            else if (attr_type == APP_BLE_DIS_ATTR_TYPE_CHAR_HW_REVISON)
            {
                char hw_revision[] = {HARDWARE_REVISION_STRING};
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0, (uint8_t *)hw_revision, strlen(hw_revision));
                return true;
            }
            else if (attr_type == APP_BLE_DIS_ATTR_TYPE_CHAR_FW_REVISON)
            {
                char fw_revision[] = {FIRMWARE_REVISION_STRING};
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0, (uint8_t *)fw_revision, strlen(fw_revision));
                return true;
            }
            else if (attr_type == APP_BLE_DIS_ATTR_TYPE_CHAR_SW_REVISON)
            {
                char sw_revision[] = {SOFTWARE_REVISION_STRING};
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0, (uint8_t *)sw_revision, strlen(sw_revision));
                return true;
            }
            else if (attr_type == APP_BLE_DIS_ATTR_TYPE_CHAR_SYSTEM_ID)
            {
                char system_id[] = {SYSTEM_ID_STRING};
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0, (uint8_t *)system_id, strlen(system_id));
                return true;
            }
            else if (attr_type == APP_BLE_DIS_ATTR_TYPE_CHAR_PNP_INFO)
            {
                app_ble_dis_pnp_info_t pnp_info;
                pnp_info.vendor_id_source = PNP_VID_SOURCE;
                pnp_info.vendor_id = PNP_VENDOR_ID;
                pnp_info.product_id = PNP_PRODUCT_ID;
                pnp_info.product_version = PNP_PRODUCT_VER;
                bta_gatts_send_read_rsp(svc->connhdl, p->token, 0, (uint8_t *)&pnp_info, sizeof(pnp_info));
                return true;
            }
            break;
        }
        default:
        {
            break;
        }
    }

    return 0;
}

void app_ble_dis_init(void)
{
    uint8_t attr_size = 0;

    bta_gatts_register_service(app_ble_dis_build_attr_list(&attr_size), attr_size, app_ble_dip_server_callback, NULL);
}

void app_ble_dis_deinit(void)
{
    bta_gatts_unregister_service(ble_dis_attr_list);
}

#endif /* BLE_DIP_ENABLE */
