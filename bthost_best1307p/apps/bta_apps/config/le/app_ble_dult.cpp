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
#ifdef SPOT_ENABLED

#include "app_ble_dult.h"
#include "gfps_ble.h"
#include "nvrecord_fp_account_key.h"

#include "bta_ble_api.h"
#include "bta_gatt_api.h"

#include "hal_trace.h"

#define DULT_SERVICE_LEN               0x05
#define DULT_SERVICE_DATA_UUID         0xB2FE
#define GOOGLE_LLC_NETWORK_ID          0x02
#define NEAR_OWNER_STATE               0x01
#define SEPARATED_MODE                 0x00

#define FP_DULT_DEVICE_MODEL_ID        0x00000000002B677D

#define DULT_IDENTIIFIER_TIME_VALUE    300000
static bool button_pressed = false;
static osTimerId dult_identifier_timer_id = NULL;
static void app_dult_identifier_handler(void const *param);
osTimerDef(DULT_IDENTIFIER_TIME, app_dult_identifier_handler);

const uint8_t category_value[8] = {0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const char manuName[] = "bestechnic";
const char modelName[] = "bestechnic";

struct dult_ble_env_tag
{
    uint16_t connhdl;
    uint8_t isIndicationEnabled;
    struct ble_dult_cb_t *cb;
} dult_ble_env = {0};

typedef enum app_ble_dult_attr_type
{
    APP_BLE_DULT_ATTR_TYPE_SERVICE = 0,
    APP_BLE_DULT_ATTR_TYPE_NO_OWNER_CHAR,
    APP_BLE_DULT_ATTR_TYPE_NO_OWNER_CCCD,

    APP_BLE_DULT_ATTR_TYPE_MAX,
} app_ble_dult_attr_type_e;

static const bt_attr_uuid_t dult_service_uuid_128_le            = {0x85, 0x2A, 0x9F, 0x57, 0xC5, 0x2A, 0xED, 0x88, 0x26, 0xC2, 0xF4, 0x12, 0x01, 0x00, 0x19, 0x15};
static const bt_attr_uuid_t dult_service_character_uuid_128_le  = {0x0E, 0x68, 0x21, 0x74, 0x37, 0x48, 0x61, 0xBF, 0x92, 0xFB, 0x68, 0x1D, 0x01, 0x00, 0x0C, 0x8E};
static const bt_attr_uuid_t dult_character_cccd_uuid_16_le      = {0x02, 0x29};

static bt_attr_mem_t ble_dult_attr_mem[APP_BLE_DULT_ATTR_TYPE_MAX] = {{{0}}};

static bt_gatt_attr_t ble_dult_attr_list[APP_BLE_DULT_ATTR_TYPE_MAX] = {{0}};

static const bt_attr_t *app_ble_dult_get_attribute(app_ble_dult_attr_type_e type)
{
    return ble_dult_attr_list[type].attr_data;
}

static const bt_gatt_attr_t *app_ble_dult_build_attr_list(uint8_t *p_attr_size)
{
    bt_attr_info_t attr_info = {0};

    // Below are (2) 128 bits len attributes
    attr_info.is_128_bits_attr_uuid = true;

    attr_info.attr_uuid = dult_service_uuid_128_le;
    ble_dult_attr_list[APP_BLE_DULT_ATTR_TYPE_SERVICE] =
        bta_gatts_build_attr(BT_ATTR_TYPE_PRI_SERVICE, &attr_info, &ble_dult_attr_mem[APP_BLE_DULT_ATTR_TYPE_SERVICE]);

    attr_info.attr_prop = BT_GATT_PROP_WR_REQ | BT_GATT_PROP_IND_PROP;
    attr_info.attr_uuid = dult_service_character_uuid_128_le;
    ble_dult_attr_list[APP_BLE_DULT_ATTR_TYPE_NO_OWNER_CHAR] =
        bta_gatts_build_attr(BT_ATTR_TYPE_CHARACTER, &attr_info, &ble_dult_attr_mem[APP_BLE_DULT_ATTR_TYPE_NO_OWNER_CHAR]);

    attr_info.attr_prop = BT_GATT_PROP_WR_REQ | BT_GATT_PROP_RD_REQ;
    attr_info.attr_uuid = dult_character_cccd_uuid_16_le;
    ble_dult_attr_list[APP_BLE_DULT_ATTR_TYPE_NO_OWNER_CCCD] =
        bta_gatts_build_attr(BT_ATTR_TYPE_DESCRIPTOR, &attr_info, &ble_dult_attr_mem[APP_BLE_DULT_ATTR_TYPE_NO_OWNER_CCCD]);

    *p_attr_size = APP_BLE_DULT_ATTR_TYPE_MAX;

    return ble_dult_attr_list;
}

static void ble_app_dult_connected_evt_handler(uint16_t connhdl)
{
    dult_ble_env.connhdl = connhdl;
}

static void ble_app_dult_disconnected_evt_handler(uint16_t connhdl)
{
    if (connhdl == dult_ble_env.connhdl)
    {
        dult_ble_env.isIndicationEnabled = false;
        dult_ble_env.connhdl = BT_INVALID_HANDLE;
    }
}

static void dult_update_connection_state(uint16_t connhdl)
{
    if (BT_INVALID_HANDLE == dult_ble_env.connhdl || connhdl != dult_ble_env.connhdl)
    {
        dult_ble_env.connhdl = connhdl;
    }
}

static void app_dult_non_owner_actions_ind_handler(uint16_t connhdl, bool ind_enabled)
{
    dult_ble_env.isIndicationEnabled = ind_enabled;

    if (dult_ble_env.isIndicationEnabled)
    {
        dult_update_connection_state(connhdl);
    }
}

void app_dult_send_non_owner_data_via_indication(uint8_t *data, uint32_t len)
{
    uint16_t connhdl = dult_ble_env.connhdl;

    bt_gatt_char_notify_t val_ind =
    {
        .service = app_ble_dult_get_attribute(APP_BLE_DULT_ATTR_TYPE_SERVICE),
        .character = app_ble_dult_get_attribute(APP_BLE_DULT_ATTR_TYPE_NO_OWNER_CHAR),
    };

    bta_gatts_send_character_value(connhdl, false, &val_ind, data, (uint16_t)len);
}

uint8_t app_dult_get_network_id(void)
{
    return GOOGLE_LLC_NETWORK_ID;
}

const char *app_dult_get_manufacturer_name(void)
{
    return manuName;
}

const char *app_dult_get_model_name(void)
{
    return modelName;
}

const uint8_t *app_dult_get_accessory_category_value(void)
{
    return category_value;
}

void app_dult_press_button_handle(APP_KEY_STATUS *status, void *param)
{
    button_pressed = true;
}

bool app_dult_get_button_status()
{
    return button_pressed;
}

POSSIBLY_UNUSED void binaryBytesToString(const unsigned char *input, int length, char *output)
{
    for (uint8_t i = 0; i < length; i++)
    {
        sprintf(output, "%02x", *input);
        output++;
        output++;
        input++;
    }
}

void app_dult_identifier_timer_init(void)
{
    if (dult_identifier_timer_id == NULL)
    {
        dult_identifier_timer_id = osTimerCreate(osTimer(DULT_IDENTIFIER_TIME), osTimerOnce, NULL);
    }

    osTimerStart(dult_identifier_timer_id, DULT_IDENTIIFIER_TIME_VALUE);
}

static void app_dult_identifier_handler(void const *param)
{
    button_pressed = false;
}

static bt_att_err_code_t app_dult_non_owner_actions_write_ind_hander(uint16_t connhdl, const uint8_t *data, uint16_t len)
{
    uint16_t data_id = data[0] | (data[1] << 8);
    TRACE(1, "app_dult_non_onwer_actions_write_ind_hander, data_id is %x", data_id);
    bt_att_err_code_t status = BT_ATT_ERR_NO_ERROR;

    if (dult_ble_env.cb->get_spot_get_mode && dult_ble_env.cb->get_spot_get_mode())
    {
        if (data_id == DULT_GET_PRODUCT_DATA)
        {
            uint64_t modelId;
            dult_get_product_data_resp product_data_rsp;
            product_data_rsp.data_id = DULT_GET_PRODUCT_DATA_RES;
            modelId = FP_DULT_DEVICE_MODEL_ID;
            memset(product_data_rsp.data, 0, DULT_PRODUCT_DATA_LEN);
            big_little_switch((uint8_t *)&modelId, product_data_rsp.data, DULT_PRODUCT_DATA_LEN);
            app_dult_send_non_owner_data_via_indication((uint8_t *)&product_data_rsp, sizeof(product_data_rsp));
        }
        else if (data_id == DULT_GET_MANUFACTURE_NAME)
        {
            dult_get_manufacturer_name_resp manu_name_rsp;
            const char *Name = app_dult_get_manufacturer_name();
            uint8_t nameLen = strlen(Name) > DULT_MANUFACTUR_NAME_LEN ?
                              DULT_MANUFACTUR_NAME_LEN : strlen(Name);

            manu_name_rsp.data_id = DULT_GET_MANUFACTURE_NAME_RSP;
            memcpy(manu_name_rsp.str, (uint8_t *)Name, nameLen);
            app_dult_send_non_owner_data_via_indication((uint8_t *)&manu_name_rsp, 2 + nameLen);
        }
        else if (data_id == DULT_GET_MODEL_NAME)
        {
            dult_get_model_name_resp model_name_rsp;
            const char *Name = app_dult_get_model_name();
            uint8_t nameLen = strlen(Name) > DULT_MODEL_NAME_LEN ?
                              DULT_MODEL_NAME_LEN : strlen(Name);

            model_name_rsp.data_id = DULT_GET_MODEL_NAME_RSP;
            memcpy(model_name_rsp.str, (uint8_t *)Name, nameLen);
            app_dult_send_non_owner_data_via_indication((uint8_t *)&model_name_rsp, 2 + nameLen);
        }
        else if (data_id == DULT_GET_ACCESSORY_CATEGORY)
        {
            dult_get_category_value_resp category_value_rsp;
            category_value_rsp.data_id = DULT_GET_ACCESSORY_CATEGORY_RSP;
            memcpy(category_value_rsp.data, app_dult_get_accessory_category_value(), DULT_CATEGROY_LEN);
            app_dult_send_non_owner_data_via_indication((uint8_t *)&category_value_rsp, sizeof(category_value_rsp));
        }
        else if (data_id == DULT_GET_PROTOCOL_VERSION)
        {
            dult_get_protocol_resp protocol_value_rsp;
            protocol_value_rsp.data_id = DULT_GET_PROTOCOL_VERSION_RSP;
            protocol_value_rsp.prorocol = 0x00010000;
            app_dult_send_non_owner_data_via_indication((uint8_t *)&protocol_value_rsp, sizeof(protocol_value_rsp));
        }
        else if (data_id == DULT_GET_ACCESSORY_CAPABILITIES)
        {
            dult_get_accessory_capability_resp accessory_rsp;
            accessory_rsp.data_id = DULT_GET_ACCESSORY_CAPABILITIES_RSP;
            accessory_rsp.accessory_value = 9;
            app_dult_send_non_owner_data_via_indication((uint8_t *)&accessory_rsp, sizeof(accessory_rsp));
        }
        else if (data_id == DULT_GET_NETWORK_ID)
        {
            dult_get_network_id_resp network_id_rsp;
            network_id_rsp.data_id = DULT_GET_NETWORK_ID_RSP;
            network_id_rsp.networkID = app_dult_get_network_id();
            app_dult_send_non_owner_data_via_indication((uint8_t *)&network_id_rsp, sizeof(network_id_rsp));
        }
        else if (data_id == DULT_GET_FIRMWARE_VERSION)
        {
            dult_get_firmware_version_resp firmware_version_rsp;
            firmware_version_rsp.data_id = DULT_GET_FIRMWARE_VERSION_RSP;
            firmware_version_rsp.firmware_version = 0x00010000;
            app_dult_send_non_owner_data_via_indication((uint8_t *)&firmware_version_rsp, sizeof(firmware_version_rsp));
        }
        else if (data_id == DULT_GET_BATTERY_TYPE)
        {
            dult_get_battery_type_resp battery_type_rsp;
            battery_type_rsp.data_id = DULT_GET_BATTERY_TYPE_RSP;
            battery_type_rsp.battery_type = DULT_RECHARGEDABLE_BATTERY;
            app_dult_send_non_owner_data_via_indication((uint8_t *)&battery_type_rsp, sizeof(battery_type_rsp));
        }
        else if (data_id == DULT_GET_BATTERY_LEVEL)
        {
            dult_get_battery_level_resp battery_level_rsp;
            battery_level_rsp.data_id = DULT_GET_BATTERY_LEVEL_RSP;
            battery_level_rsp.battery_level = DULT_BATTERT_LEVEL_FULL;
            app_dult_send_non_owner_data_via_indication((uint8_t *)&battery_level_rsp, sizeof(battery_level_rsp));
        }
        else if (data_id == DULT_SOUND_START)
        {
            if (dult_ble_env.cb->start_find_ringtone)
            {
                dult_ble_env.cb->start_find_ringtone();
            }
            dult_sound_start_resp sound_rsp;
            sound_rsp.data_id = DULT_SOUND_COMMAND_RESPONSE;
            sound_rsp.cmdcode = DULT_SOUND_START;
            sound_rsp.status = SOUND_SUCCESS;
            app_dult_send_non_owner_data_via_indication((uint8_t *)&sound_rsp, sizeof(sound_rsp));
        }
        else if (data_id == DULT_SOUND_STOP)
        {
            if (dult_ble_env.cb->stop_find_ringtone)
            {
                dult_ble_env.cb->stop_find_ringtone();
            }
            dult_sound_complete_resp sound_complete_rsp;
            sound_complete_rsp.data_id = DULT_SOUND_COMPLETED;
            app_dult_send_non_owner_data_via_indication((uint8_t *)&sound_complete_rsp, sizeof(sound_complete_rsp));
        }
        else if (data_id == DULT_GET_IDENTIFIER)
        {
            if (app_dult_get_button_status() == true)
            {
                dult_get_identifier_payload_resp identifier_rsp;
                uint8_t EID[20];
                uint8_t hash256Result[32];
                uint8_t rsp[18];
                uint8_t eph_identity_key[FP_EPH_IDENTITY_KEY_LEN + 1];
                uint8_t trunted_EID[10];
                uint8_t calculatedHmacFirst8Bytes[8];
                uint8_t hashed_recover_key[8 + 8];
                memset(hashed_recover_key, 0, 16);
                if (dult_ble_env.cb->get_eid)
                {
                    memcpy(EID, dult_ble_env.cb->get_eid(), 20);
                }
                memcpy(trunted_EID, EID, 10);
                identifier_rsp.data_id = DULT_GET_IDENTIFIER_RSP;

                memcpy(eph_identity_key, nv_record_fp_get_eph_identity_key(), FP_EPH_IDENTITY_KEY_LEN);
                eph_identity_key[FP_EPH_IDENTITY_KEY_LEN] = 0x01;
                if (dult_ble_env.cb->sha256_hash)
                {
                    dult_ble_env.cb->sha256_hash(eph_identity_key, 33, hash256Result);
                }
                memcpy(hashed_recover_key, hash256Result, 8);                    //recovery key;

                if (dult_ble_env.cb->beacon_encrpt_data)
                {
                    dult_ble_env.cb->beacon_encrpt_data(hashed_recover_key, trunted_EID, 10, calculatedHmacFirst8Bytes);
                }
                memcpy(rsp, EID, 10);
                memcpy(&rsp[10], calculatedHmacFirst8Bytes, 8);
                memcpy(identifier_rsp.data, rsp, 18);

                app_dult_send_non_owner_data_via_indication((uint8_t *)&identifier_rsp, sizeof(identifier_rsp));
                app_dult_identifier_timer_init();
            }
            else
            {
                dult_cmd_resp cmd_rsp;
                cmd_rsp.cmdcode = DULT_GET_IDENTIFIER;
                cmd_rsp.status = 0xFFFF;
                app_dult_send_non_owner_data_via_indication((uint8_t *)&cmd_rsp, sizeof(cmd_rsp));
            }
        }
        else
        {
        }
    }

    return status;
}

static bool ble_dult_server_callback(bt_gatt_svc_t *svc, bt_gatts_event_t event, bt_gatts_callback_param_t param)
{
    TRACE(3, "%s, event is %d, connhd is %d", __func__, event, svc->connhdl);
    bool ret = true;
    switch (event)
    {
        case BT_GATTS_EVENT_CONN_OPENED:
        {
            ble_app_dult_connected_evt_handler(svc->connhdl);
            break;
        }
        case BT_GATTS_EVENT_CONN_CLOSED:
        {
            ble_app_dult_disconnected_evt_handler(svc->connhdl);
            break;
        }
        case BT_GATTS_EVENT_DESC_WRITE:
        {
            bt_gatts_desc_write_t *p = param.desc_write;
            uint16_t config = CO_COMBINE_UINT16_LE(p->value);
            bool ind_enabled = false;
            if (config & BT_GATT_CCCD_INDICATION)
            {
                ind_enabled = true;
            }
            app_dult_non_owner_actions_ind_handler(svc->connhdl, ind_enabled);
            break;
        }
        case BT_GATTS_EVENT_CHAR_WRITE:
        {
            bt_gatts_char_write_t *p = param.char_write;
            if (p->value_offset != 0 || p->value_len == 0 || p->value == NULL)
            {
                return false;
            }
            p->rsp_error_code = app_dult_non_owner_actions_write_ind_hander(svc->connhdl, p->value, p->value_len);
            ret = (p->rsp_error_code == BT_ATT_ERR_NO_ERROR);
            bta_gatts_send_write_rsp(svc->connhdl, p->token, p->rsp_error_code);
            break;
        }
        case BT_GATTS_EVENT_DESC_READ:
        {
            bt_gatts_desc_read_t *p = param.desc_read;
            uint16_t ntf_ind_bf_packed = 0;
            if (dult_ble_env.isIndicationEnabled)
            {
                ntf_ind_bf_packed = BT_GATT_CCCD_INDICATION;
            }
            bta_gatts_send_read_rsp(svc->connhdl, p->token, 0,
                                    (uint8_t *)&ntf_ind_bf_packed, sizeof(ntf_ind_bf_packed));

            return true;
        }

        default:
        {
            break;
        }

    }
    return ret;
}

void ble_app_dult_init(struct ble_dult_cb_t *cbs)
{
    const bt_gatt_attr_t *attr_list = NULL;
    uint8_t attr_list_len = 0;

    dult_ble_env.connhdl = BT_INVALID_HANDLE;
    dult_ble_env.isIndicationEnabled = false;
    dult_ble_env.cb = cbs;

    attr_list = app_ble_dult_build_attr_list(&attr_list_len);

    bta_gatts_register_service(attr_list, attr_list_len, ble_dult_server_callback, NULL);
}

void ble_app_dult_deinit(void)
{
    memset(&dult_ble_env, 0, sizeof(dult_ble_env));
    bta_gatts_unregister_service(ble_dult_attr_list);
}
#endif /* SPOT_ENABLED */
