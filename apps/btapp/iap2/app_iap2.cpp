#ifdef IOS_IAP2_BLUETOOTH

#include <string.h>
#include "hal_trace.h"
#include "app_iap2.h"
#include "iap2_ota.h"
#include "bta_iap2_api.h"
#include "iap2_mfi_i2c.h"

// Static global variable to store IAP2 information
static bt_iap2_information_t iap2_info = {0};
// Global variable to store accessory callbacks
bt_iap2_accessory_cb_t access_cb;

// Define the IAP2 EA (External Accessory) protocol group for apps
#ifdef IOS_IAP2_BES_OTA_SUPPORT
#define BT_IAP2_OTA_EA_PROTOCOL_IDENTIFIER 0x01
#define BT_IAP2_OTA_EA_PROTOCOL_NAME           "com.iap2.test"
#define BT_IAP2_OTA_EA_PROTOCOL_MATCH_ACTION   BT_IAP2_MATCH_ACTION_DEVICE_MAY_PROMPT_AND_FIND_APP_BUTTON

#define BT_IAP2_DIY_PROTOCOL_1_PROTOCOL_ID 0x02
#else
#define BT_IAP2_DIY_PROTOCOL_1_PROTOCOL_ID 0x01
#endif
#define BT_IAP2_DIY_PROTOCOL_1_PROTOCOL_NAME "com.iap2.proto1"
#define BT_IAP2_DIY_PROTOCOL_1_PROTOCOL_MATCH_ACTION BT_IAP2_MATCH_ACTION_DEVICE_MAY_PROMPT_AND_FIND_APP_BUTTON
bt_iap2_ea_protocol_group_t ea_group[] =
{
    // {protocol_id, protocol_name, match_action}
#ifdef IOS_IAP2_BES_OTA_SUPPORT
    {BT_IAP2_OTA_EA_PROTOCOL_IDENTIFIER, BT_IAP2_OTA_EA_PROTOCOL_NAME, BT_IAP2_OTA_EA_PROTOCOL_MATCH_ACTION},
#endif
    {BT_IAP2_DIY_PROTOCOL_1_PROTOCOL_ID, BT_IAP2_DIY_PROTOCOL_1_PROTOCOL_NAME, BT_IAP2_DIY_PROTOCOL_1_PROTOCOL_MATCH_ACTION},
};


// Default transport component
#define BT_IAP2_BLUETOOTH_TRANSPORT_COMPONENT_ID 1
bt_iap2_bluetooth_transport_component_group_t bt_trans_group =
{
    {BT_IAP2_BLUETOOTH_TRANSPORT_COMPONENT_ID >> 8,  BT_IAP2_BLUETOOTH_TRANSPORT_COMPONENT_ID & 0xFF},
    { 'B', 'l', 'u', 'e', 't', 'o', 'o', 't', 'h', 0 },
    {0}
};

/**
 * Initialize IAP2 information structure with device identification data
 * This function sets up all the identification and capability information
 * that the accessory will provide to connected iOS devices
 */
void app_ia2p_info_init(void)
{
    char Name[]      = "IAP2 ACCESSORY"; // max_len is 32, included \0
    char ModelIdentifier[]      = "Any buds"; // max_len is 16, included \0
    char Manufacturer[]         = "Amazon"; // max_len is 11, included \0
    char SerialNumber[]         = "G2A26Q033205TFDP"; // max_len is 17, included \0
    // Firmware version string
    char firmware_version[]     = "0.0.1"; // max_len is 6, included \0
    // Hardware version string
    char HardwareVersion[]      = "0.0.1"; // max_len is 6, included \0
    // Messages that the accessory can send to the device
    uint8_t MessagesSentByAccessory[]       = {BT_IAP2_HI_BYTE(BT_IAP2_REQUEST_APP_LAUNCH),  BT_IAP2_LO_BYTE(BT_IAP2_REQUEST_APP_LAUNCH)}; // max_len is 6
    // Messages that the accessory can receive from the device
    uint8_t MessagesReceivedFromDevice[]    = {BT_IAP2_HI_BYTE(BT_IAP2_START_EXTERNAL_ACCESSORY_PROTOCOL_SESSION), BT_IAP2_LO_BYTE(BT_IAP2_START_EXTERNAL_ACCESSORY_PROTOCOL_SESSION),
                                                BT_IAP2_HI_BYTE(BT_IAP2_STOP_EXTERNAL_ACCESSORY_PROTOCOL_SESSION), BT_IAP2_LO_BYTE(BT_IAP2_STOP_EXTERNAL_ACCESSORY_PROTOCOL_SESSION)}; // max_len is 4
    // Power providing capability of the accessory
    uint8_t PowerProvidingCapability[]      = {BT_IAP2_POWER_PROVIDING_CAPABILITY_NONE};
    uint8_t MaximumCurrentDrawnFromDevice[] = {BT_IAP2_HI_BYTE(BT_IAP2_MAX_POWER_DRAWN_FROM_THE_DEVICE_NONE), BT_IAP2_LO_BYTE(BT_IAP2_MAX_POWER_DRAWN_FROM_THE_DEVICE_NONE)};
    // Application match team ID for identification
    char AppMatchTeamId[]       = "94KV3E626L"; // max_len is 11, included \0
    // Current language setting: en/zh
    char CurrentLanguage[]      = "en"; // [fixed style]max_len is 3, included \0
    // Supported languages array
    char SupportedLanguage[]    = { 'e', 'n', 0, 0}; // [fixed style]max_len is 4, included two '\0': first '\0' is string end flag, second '\0' is feild end flag
    // Product plan UID for identification
    char ProductPlanUID[]       = "3024e0b1f5494537"; // max_len is 17, included \0
    memcpy(iap2_info.Name, Name, sizeof(Name));
    memcpy(iap2_info.ModelIdentifier, ModelIdentifier, sizeof(ModelIdentifier));
    memcpy(iap2_info.Manufacturer, Manufacturer, sizeof(Manufacturer));
    memcpy(iap2_info.SerialNumber, SerialNumber, sizeof(SerialNumber));
    memcpy(iap2_info.firmware_version, firmware_version, sizeof(firmware_version));
    memcpy(iap2_info.HardwareVersion, HardwareVersion, sizeof(HardwareVersion));
    memcpy(iap2_info.MessagesSentByAccessory, MessagesSentByAccessory, sizeof(MessagesSentByAccessory));
    iap2_info.len_MessagesSentByAccessory = sizeof(MessagesSentByAccessory);
    memcpy(iap2_info.MessagesReceivedFromDevice, MessagesReceivedFromDevice, sizeof(MessagesReceivedFromDevice));
    iap2_info.len_MessagesReceivedFromDevice = sizeof(MessagesReceivedFromDevice);
    memcpy(iap2_info.PowerProvidingCapability, PowerProvidingCapability, sizeof(PowerProvidingCapability));
    iap2_info.len_PowerProvidingCapability = sizeof(PowerProvidingCapability);
    memcpy(iap2_info.MaximumCurrentDrawnFromDevice, MaximumCurrentDrawnFromDevice, sizeof(MaximumCurrentDrawnFromDevice));
    iap2_info.len_MaximumCurrentDrawnFromDevice = sizeof(MaximumCurrentDrawnFromDevice);
    iap2_info.ea_group = ea_group;
    iap2_info.ea_group_count = sizeof(ea_group)/sizeof(ea_group[0]);
    memcpy(iap2_info.AppMatchTeamId, AppMatchTeamId, sizeof(AppMatchTeamId));
    memcpy(iap2_info.CurrentLanguage, CurrentLanguage, sizeof(CurrentLanguage));
    iap2_info.len_CurrentLanguage = sizeof(CurrentLanguage);
    memcpy(iap2_info.SupportedLanguage, SupportedLanguage, sizeof(SupportedLanguage));
    iap2_info.bt_trans_group = &bt_trans_group;
    memcpy(iap2_info.ProductPlanUID, ProductPlanUID, sizeof(ProductPlanUID));

    BTAPP_TRACE(0, "[IAP2]%s", __func__);
}

/**
 * Callback function called when an external accessory session starts
 * @param addr Bluetooth address of the connected device
 * @param protocol_id Protocol identifier
 * @param protocol_sess_id Session identifier
 */
void ext_accessory_start_ea_session_cb(uint8_t* addr, uint8_t protocol_id, uint16_t protocol_sess_id)
{
    bt_iap2_link_info_t link_info;
    memcpy(&(link_info.addr.addr), addr, sizeof(bt_bdaddr_t));
    link_info.type = BT_IAP2_LINK_TYPE_BT;
    // Currently empty implementation - can be extended for session start handling
    // example:
    //   // void iap2_diy_protocol_1_start_handle(const bt_bdaddr_t *addr);
    //
    //   if(protocol_id == IAP2_DIY_PROTOCOL_1_PROTOCOL_ID)
    //   {
    //        iap2_diy_protocol_1_start_handle((bt_bdaddr_t* )&link_info.addr.addr); // input parameter type is up to the callback implement
    //   }
#ifdef IOS_IAP2_BES_OTA_SUPPORT
    if(BT_IAP2_OTA_EA_PROTOCOL_IDENTIFIER == protocol_id)
    {
        iap2_ota_connect_handle((uint8_t *)(&link_info.addr.addr));
    }
#endif
}

/**
 * Callback function called when an external accessory session stops
 * @param addr Bluetooth address of the connected device
 * @param protocol_id Protocol identifier
 */
void ext_accessory_stop_ea_session_cb(uint8_t* addr, uint8_t protocol_id)
{
    bt_iap2_link_info_t link_info;
    memcpy(&(link_info.addr.addr), addr, sizeof(bt_bdaddr_t));
    link_info.type = BT_IAP2_LINK_TYPE_BT;
    // Currently empty implementation - can be extended for session stop handling
    // example:
    //   // void iap2_diy_protocol_1_stop_handle(const bt_bdaddr_t *addr);
    //
    //   if(protocol_id == IAP2_DIY_PROTOCOL_1_PROTOCOL_ID)
    //   {
    //        iap2_diy_protocol_1_stop_handle((bt_bdaddr_t* )&link_info.addr.addr); // input parameter type is up to the callback implement
    //   }
#ifdef IOS_IAP2_BES_OTA_SUPPORT
    if(BT_IAP2_OTA_EA_PROTOCOL_IDENTIFIER == protocol_id)
    {
        iap2_ota_disconnect_handle(); // only support one connect, so don't need to enter address
    }
#endif
}

/**
 * Callback function called when data is received from external accessory
 * @param addr Bluetooth address of the connected device
 * @param protocol_id Protocol identifier
 * @param data_len Length of received data
 * @param buf Buffer containing received data
 */
void ext_accessory_data_recv_cb(uint8_t* addr, uint8_t protocol_id, uint16_t data_len, uint8_t* buf)
{
    bt_iap2_link_info_t link_info;
    memcpy(&(link_info.addr.addr), addr, sizeof(bt_bdaddr_t));
    link_info.type = BT_IAP2_LINK_TYPE_BT;
    // Currently empty implementation - can be extended for data processing
    // example:
    //   // void iap2_diy_protocol_1_recv_handle(const bt_bdaddr_t *addr, uint8_t* buf, uint16_t data_len);
    //
    //   if(protocol_id == IAP2_DIY_PROTOCOL_1_PROTOCOL_ID)
    //   {
    //        iap2_diy_protocol_1_recv_handle((bt_bdaddr_t* )&link_info.addr.addr, buf, data_len); // input parameter type is up to the callback implement
    //   }
#ifdef IOS_IAP2_BES_OTA_SUPPORT
    if(BT_IAP2_OTA_EA_PROTOCOL_IDENTIFIER == protocol_id)
    {
        iap2_ota_recv_data_handle(buf, data_len); // only support one connect, so don't need to enter address
    }
#endif
}

/**
 * Callback function called when EA Session transmission is done
 * @param addr Bluetooth address of the connected device(EA Session TX done)
 */
void ext_accessory_data_tx_done_cb(uint8_t *addr)
{
    bt_iap2_link_info_t link_info;
    memcpy(&(link_info.addr.addr), addr, sizeof(bt_bdaddr_t));
    link_info.type = BT_IAP2_LINK_TYPE_BT;
    // Currently empty implementation - can be extended for tx done handling
    BTAPP_TRACE(0, "[IAP2][scn:%d] iap2 ea packet tx done", bta_iap2_service_spp_local_channel());
}

/**
 * Callback function called for accessory authentication state changes
 * @param addr Bluetooth address of the connected device
 * @param authen_state Authentication state (true = authenticated, false = not authenticated)
 */
void accessory_authen_cb(uint8_t* addr, bool authen_state)
{
    bt_iap2_link_info_t link_info;
    memcpy(&(link_info.addr.addr), addr, sizeof(bt_bdaddr_t));
    link_info.type = BT_IAP2_LINK_TYPE_BT;
    // Currently empty implementation - can be extended for authentication handling
    BTAPP_TRACE(0, "[IAP2]%s: authen_state %d addr %x:%x:...:%x", __func__, authen_state,
                        link_info.addr.addr.address[0], link_info.addr.addr.address[1], link_info.addr.addr.address[5]);
}

/**
 * Callback function called for accessory identification state changes
 * @param addr Bluetooth address of the connected device
 * @param ident_state Identification state (true = identified, false = not identified)
 */
void accessory_ident_cb(uint8_t* addr, bool ident_state)
{
    bt_iap2_link_info_t link_info;
    memcpy(&(link_info.addr.addr), addr, sizeof(bt_bdaddr_t));
    link_info.type = BT_IAP2_LINK_TYPE_BT;
    // Currently empty implementation - can be extended for identification handling
    BTAPP_TRACE(0, "[IAP2]%s: ident_state %d addr %x:%x:...:%x", __func__, ident_state,
                        link_info.addr.addr.address[0], link_info.addr.addr.address[1], link_info.addr.addr.address[5]);
}

/**
 * Initialize accessory callback functions
 * This function sets up all the callback functions that will be used
 * for handling different IAP2 events and communications
 */
void app_iap2_init_accessory_cb(void)
{
    BTAPP_TRACE(0, "[IAP2]%s", __func__);
    access_cb.authen_state_cb = accessory_authen_cb;
    access_cb.ident_state_cb = accessory_ident_cb;
    access_cb.start_ea_cb = ext_accessory_start_ea_session_cb;
    access_cb.stop_ea_cb = ext_accessory_stop_ea_session_cb;
    access_cb.ea_data_recv_cb = ext_accessory_data_recv_cb;
    access_cb.ea_data_tx_done_cb = ext_accessory_data_tx_done_cb;
}

void app_iap2_bes_ota_init(void)
{
#ifdef IOS_IAP2_BES_OTA_SUPPORT
    BTAPP_TRACE(0, "[IAP2]%s", __func__);
    iap2_ota_set_ea_session_protocol_id(BT_IAP2_OTA_EA_PROTOCOL_IDENTIFIER);
#endif
}

void app_iap2_service_config_init(void)
{
    BTAPP_TRACE(0, "[IAP2]%s", __func__);
    bta_iap2_service_enable_default_reconnect(true);
    bta_iap2_service_enable_check_mfi_when_initialed(false);
    bta_iap2_service_register_mfi_i2c_callback(NULL, NULL); // if NULL, means use stack default i2c
}

/**
 * Main initialization function for IAP2 application
 * This function initializes all components required for IAP2 communication
 */
void app_iap2_init(void)
{
    // [MUST]First, init iap2 service config
    app_iap2_service_config_init();

#ifdef IOS_IAP2_BES_OTA_SUPPORT
    app_iap2_bes_ota_init();
#endif

    // [MUST]Second, init ident info - Initialize device identification information
    app_ia2p_info_init();

    // [MUST]Third, init accessory cb - Initialize accessory callbacks
    app_iap2_init_accessory_cb();

    // [MUST]Forth, register iap2 service - Register the IAP2 service with the system
    bta_iap2_service_register(&iap2_info, &access_cb);
}

#endif