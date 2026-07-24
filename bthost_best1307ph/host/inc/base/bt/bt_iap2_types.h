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

#pragma once
#include "bt_base_types.h"

// Note: Don't change the buffer parameters by user
// support EDR 2M/3M, L2CAP_CFG_MTU is 1021 in 3M_PACK situation, buffer should greater than 1021
// single sending-frame max is 1000, otherwise will split to multi-frame
#define BT_IAP2_MAX_BUFFER_LEN          1100
// BT_IAP2_MAX_SESSION_BUFFER_LEN must >= (BT_IAP2_MAX_BUFFER_LEN + 200), minimux is 1300
#define BT_IAP2_MAX_SESSION_BUFFER_LEN  1300

#define BT_IAP2_HI_BYTE(X) (((X) >> 8) & 0xFF)
#define BT_IAP2_LO_BYTE(X) ((X) & 0xFF)

#define BT_IAP2_MATCH_ACTION_NO_PROMPT                                 0
#define BT_IAP2_MATCH_ACTION_DEVICE_MAY_PROMPT_AND_FIND_APP_BUTTON     1
#define BT_IAP2_MATCH_ACTION_DEVICE_NO_PROMPT_AND_FIND_APP_BUTTON      2

// macro from NameMaxLen to BT_IAP2_NAME_MAX_LEN
#define BT_IAP2_NAME_MAX_LEN                         (32)
// macro from ModelIdentifierMaxLen to BT_IAP2_MODEL_IDENTIFIER_MAX_LEN
#define BT_IAP2_MODEL_IDENTIFIER_MAX_LEN             (16)
// macro from ManufacturerMaxLen to BT_IAP2_BT_IAP2_MANUFACTURER_MAX_LEN
#define BT_IAP2_MANUFACTURER_MAX_LEN                (11)
// macro from SerialNumberMaxLen to BT_IAP2_BT_IAP2_SERIAL_NUMBER_MAX_LEN
#define BT_IAP2_SERIAL_NUMBER_MAX_LEN                (17)
// macro from FirmwareVersionMaxLen to BT_IAP2_BT_IAP2_FIRMWARE_VERSION_MAX_LEN
#define BT_IAP2_FIRMWARE_VERSION_MAX_LEN             (6)
// macro from HardwareVersionMaxLen to BT_IAP2_HARDWARE_VERSION_MAX_LEN
#define BT_IAP2_HARDWARE_VERSION_MAX_LEN             (6)
// macro from MessagesSentByAccessoryMaxLen to BT_IAP2_MESSAGES_SENT_BY_ACCESSORY_MAX_LEN
#define BT_IAP2_MESSAGES_SENT_BY_ACCESSORY_MAX_LEN     (6)
// macro from MessagesReceivedFromDeviceMaxLen to BT_IAP2_MESSAGES_RECEIVED_FROM_DEVICE_MAX_LEN
#define BT_IAP2_MESSAGES_RECEIVED_FROM_DEVICE_MAX_LEN  (4)
// macro from PowerProvidingCapabilityMaxLen to BT_IAP2_POWER_PROVIDING_CAPABILITY_MAX_LEN
#define BT_IAP2_POWER_PROVIDING_CAPABILITY_MAX_LEN    (1)
// macro from MaximumCurrentDrawnFromDeviceMaxLen to BT_IAP2_MAXIMUM_CURRENT_DRAWN_FROM_DEVICE_MAX_LEN
#define BT_IAP2_MAXIMUM_CURRENT_DRAWN_FROM_DEVICE_MAX_LEN  (2)
// macro from AppMatchTeamIdMaxLen to BT_IAP2_APP_MATCH_TEAM_ID_MAX_LEN
#define BT_IAP2_APP_MATCH_TEAM_ID_MAX_LEN              (11)
// macro from CurrentLanguageMaxLen to BT_IAP2_CURRENT_LANGUAGE_MAX_LEN
#define BT_IAP2_CURRENT_LANGUAGE_MAX_LEN             (3)
// macro from SupportedLanguageMaxLen to BT_IAP2_SUPPORTED_LANGUAGE_MAX_LEN
#define BT_IAP2_SUPPORTED_LANGUAGE_MAX_LEN           (4)
// macro from BluetoothTransportComponentNameMaxLen to BT_IAP2_BLUETOOTH_TRANSPORT_COMPONENT_NAME_MAX_LEN
#define BT_IAP2_BLUETOOTH_TRANSPORT_COMPONENT_NAME_MAX_LEN      (15)
// macro from ProductPlanUIDMaxLen to BT_IAP2_PRODUCT_PLAN_UID_MAX_LEN
#define BT_IAP2_PRODUCT_PLAN_UID_MAX_LEN              (17)

#define BT_IAP2_POWER_PROVIDING_CAPABILITY_NONE        0 // Shall be None if the accessory does not provide power to the device

#define BT_IAP2_MAX_POWER_DRAWN_FROM_THE_DEVICE_NONE   0   // Shall be 0 if accessory does not draw power from the device

#define BT_IAP2_START_EXTERNAL_ACCESSORY_PROTOCOL_SESSION                                   0xEA00 /* from device */

#define BT_IAP2_STOP_EXTERNAL_ACCESSORY_PROTOCOL_SESSION                                    0xEA01 /* from device */

#define BT_IAP2_STATUS_EXTERNAL_ACCESSORY_PROTOCOL_SESSION                                  0xEA03 /* to device */

#define BT_IAP2_REQUEST_APP_LAUNCH                        (0xEA02) /* from accessory */

#define BT_IAP2_APP_LAUNCH_METHOD_ID_WITH_USER_ALERT      (0) // default value
#define BT_IAP2_APP_LAUNCH_METHOD_ID_WITHOUT_USER_ALERT   (1)

typedef struct {
    uint8_t i2c_sda_mux_io;
    uint8_t i2c_scl_mux_io;
    uint8_t i2c_sda_io;
    uint8_t i2c_sck_io;
} bt_iap2_i2c_gpio_config_t;

typedef enum
{
    BT_IAP2_LINK_TYPE_NONE,
    BT_IAP2_LINK_TYPE_BT,
    BT_IAP2_LINK_TYPE_BLE,
}__attribute__ ((packed)) bt_iap2_link_type_t;

typedef union
{
    bt_bdaddr_t addr;
}__attribute__ ((packed)) bt_iap2_link_addr_t;

typedef struct
{
    bt_iap2_link_type_t type;
    bt_iap2_link_addr_t addr;
}__attribute__ ((packed)) bt_iap2_link_info_t;

typedef struct
{
    uint8_t protocol_id;
    const uint8_t ea_protocol_name[40]; // include '\0'
    uint8_t match_action;
} bt_iap2_ea_protocol_group_t;

typedef struct
{
    uint8_t ComponentIdentifier[2];
    uint8_t ComponentName[BT_IAP2_BLUETOOTH_TRANSPORT_COMPONENT_NAME_MAX_LEN];
    uint8_t MediaAccessControlAddress[6];
} bt_iap2_bluetooth_transport_component_group_t;

typedef struct
{
    uint8_t Name[BT_IAP2_NAME_MAX_LEN];
    uint8_t ModelIdentifier[BT_IAP2_MODEL_IDENTIFIER_MAX_LEN];
    uint8_t Manufacturer[BT_IAP2_MANUFACTURER_MAX_LEN];
    uint8_t SerialNumber[BT_IAP2_SERIAL_NUMBER_MAX_LEN];
    uint8_t firmware_version[BT_IAP2_FIRMWARE_VERSION_MAX_LEN];
    uint8_t HardwareVersion[BT_IAP2_HARDWARE_VERSION_MAX_LEN];

    uint8_t MessagesSentByAccessory[BT_IAP2_MESSAGES_SENT_BY_ACCESSORY_MAX_LEN];
    uint8_t len_MessagesSentByAccessory;

    uint8_t MessagesReceivedFromDevice[BT_IAP2_MESSAGES_RECEIVED_FROM_DEVICE_MAX_LEN];
    uint8_t len_MessagesReceivedFromDevice;

    uint8_t PowerProvidingCapability[BT_IAP2_POWER_PROVIDING_CAPABILITY_MAX_LEN];
    uint8_t len_PowerProvidingCapability;

    uint8_t MaximumCurrentDrawnFromDevice[BT_IAP2_MAXIMUM_CURRENT_DRAWN_FROM_DEVICE_MAX_LEN];
    uint8_t len_MaximumCurrentDrawnFromDevice;

    bt_iap2_ea_protocol_group_t* ea_group;
    uint8_t ea_group_count;
    uint8_t AppMatchTeamId[BT_IAP2_APP_MATCH_TEAM_ID_MAX_LEN];
    uint8_t CurrentLanguage[BT_IAP2_CURRENT_LANGUAGE_MAX_LEN];
    uint8_t len_CurrentLanguage;
    uint8_t SupportedLanguage[BT_IAP2_SUPPORTED_LANGUAGE_MAX_LEN];
    bt_iap2_bluetooth_transport_component_group_t* bt_trans_group;
    uint8_t ProductPlanUID[BT_IAP2_PRODUCT_PLAN_UID_MAX_LEN];
} bt_iap2_information_t;

typedef void (*bt_iap2_ext_accessory_notify_start_ea_session_cb)(uint8_t* addr, uint8_t protocol_id, uint16_t protocol_sess_id);
typedef void (*bt_iap2_ext_accessory_notify_stop_ea_session_cb)(uint8_t* addr, uint8_t protocol_id);
typedef void (*bt_iap2_ext_accessory_notify_data_recv_cb)(uint8_t* addr, uint8_t protocol_id, uint16_t data_len, uint8_t* buf);
typedef void (*bt_iap2_ext_accessory_notify_data_tx_done_cb)(uint8_t* addr);
typedef void (*bt_iap2_accessory_authen_state_cb)(uint8_t* addr, bool authen_state);
typedef void (*bt_iap2_accessory_ident_state_cb)(uint8_t* addr, bool ident_state);

typedef struct {
    bt_iap2_ext_accessory_notify_start_ea_session_cb start_ea_cb;
    bt_iap2_ext_accessory_notify_stop_ea_session_cb stop_ea_cb;
    bt_iap2_ext_accessory_notify_data_recv_cb ea_data_recv_cb;
    bt_iap2_ext_accessory_notify_data_tx_done_cb ea_data_tx_done_cb;
    bt_iap2_accessory_authen_state_cb authen_state_cb;
    bt_iap2_accessory_ident_state_cb ident_state_cb;
} bt_iap2_accessory_cb_t;

typedef int (*bt_mfi_i2c_read_reg_data_cb)(uint8_t device_addr, uint8_t reg_addr, uint8_t *buf, int len);
typedef int (*bt_mfi_i2c_write_reg_data_cb)(uint8_t device_addr, uint8_t reg_addr, uint8_t *buf, int len);
