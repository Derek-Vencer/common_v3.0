/****************************************************************************
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

#ifndef __BTS_IAP2_API_H__
#define __BTS_IAP2_API_H__
#include "bt_iap2_types.h"
#include "bts_bt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 ****************************************************************************************
 * @brief       Enable/disable default accessory name for IAP2 service
 * @param[in]   enable: True to enable default accessory name, false to disable
 * @return      None
 ****************************************************************************************
 */
void bts_iap2_service_enable_default_accessory_name(bool enable);

/**
 ****************************************************************************************
 * @brief       Enable/disable default reconnect feature for IAP2 service
 * @param[in]   enable: True to enable default reconnect, false to disable
 * @return      None
 ****************************************************************************************
 */
void bts_iap2_service_enable_default_reconnect(bool enable);

/**
 ****************************************************************************************
 * @brief       Enable/disable default mfi i2c module for IAP2 service
 * @param[in]   enable: True to enable default mfi i2c module, false to disable
 * @return      None
 ****************************************************************************************
 */
void bts_iap2_service_enable_default_i2c_module(bool enable);

/**
 ****************************************************************************************
 * @brief       Enable/disable receive flow control for IAP2 service[Not implemented, reserved interface]
 * @param[in]   enable: True to enable receive flow control, false to disable
 * @return      None
 ****************************************************************************************
 */
void bts_iap2_service_enable_recv_flow_control(bool enable);

/**
 ****************************************************************************************
 * @brief       Enable/disable send flow control for IAP2 service[Not implemented, reserved interface]
 * @param[in]   enable: True to enable send flow control, false to disable
 * @return      None
 ****************************************************************************************
 */
void bts_iap2_service_enable_send_flow_control(bool enable);

/**
 ****************************************************************************************
 * @brief       Enable/disable delay detection for IAP2 service
 * @param[in]   enable: True to enable delay detection, false to disable
 * @return      None
 ****************************************************************************************
 */
void bts_iap2_service_enable_delay_detect(bool enable);

/**
 ****************************************************************************************
 * @brief       Set delay detection time for IAP2 service
 * @param[in]   delay_time: Delay time in milliseconds
 * @return      None
 ****************************************************************************************
 */
void bts_iap2_service_set_delay_detect_time(uint16_t delay_time);

/**
 ****************************************************************************************
 * @brief       Enable/disable printing information for IAP2 service
 * @param[in]   enable: True to enable printing information, false to disable
 * @return      None
 ****************************************************************************************
 */
void bts_iap2_service_enable_print_info_when_initialed(bool enable);

/**
 ****************************************************************************************
 * @brief       Enable/disable checking mfi chip for IAP2 service
 * @param[in]   enable: True to enable checking mfi chip, false to disable
 * @return      None
 ****************************************************************************************
 */
void bts_iap2_service_enable_check_mfi_when_initialed(bool enable);

/**
 ****************************************************************************************
 * @brief       Register IAP2 service with information and callback
 * @param[in]   info: Pointer to IAP2 information structure
 * @param[in]   cb: Pointer to IAP2 accessory callback structure
 * @return      None
 ****************************************************************************************
 */
void bts_iap2_service_register(bt_iap2_information_t* info, bt_iap2_accessory_cb_t* cb);

/**
 ****************************************************************************************
 * @brief       Check if sending EA data is allowed for specified device[Now, only for TWS environment]
 * @param[in]   addr: Pointer to Bluetooth address of peer device
 * @return      True if allowed, false otherwise
 ****************************************************************************************
 */
bool bts_iap2_service_send_ea_data_is_allowed(bt_bdaddr_t* addr);

/**
 ****************************************************************************************
 * @brief       Send EA data to specified device
 * @param[in]   addr: Pointer to Bluetooth address of peer device
 * @param[in]   protocol_id: Protocol ID for the data
 * @param[in]   session_id: Session ID for the data[Just fix it to 0, reserved parameter]
 * @param[in]   len: Length of data to send
 * @param[in]   buf: Pointer to data buffer
 * @return      True if successful, false otherwise
 ****************************************************************************************
 */
bool bts_iap2_service_send_ea_data(bt_bdaddr_t* addr, uint8_t protocol_id, uint16_t session_id, uint16_t len, uint8_t* buf);

/**
 ****************************************************************************************
 * @brief       Launch application on remote device
 * @param[in]   addr: Pointer to Bluetooth address of peer device
 * @param[in]   app_bundle_id: Pointer to application bundle ID
 * @param[in]   app_bundle_id_len: Length of application bundle ID[Needs to include '\0']
 * @param[in]   AppLaunchMethod: Application launch method
 * @return      True if successful, false otherwise
 ****************************************************************************************
 */
bool bts_iap2_service_app_launch(bt_bdaddr_t* addr, uint8_t* app_bundle_id, uint8_t app_bundle_id_len, uint8_t AppLaunchMethod);

/**
 ****************************************************************************************
 * @brief       Check if IAP2 service is connected to specified device
 * @param[in]   addr: Pointer to Bluetooth address of peer device
 * @return      True if connected, false otherwise
 ****************************************************************************************
 */
bool bts_iap2_service_is_connected(bt_bdaddr_t* addr);

/**
 ****************************************************************************************
 * @brief       Reconnect IAP2 service to specified device
 * @param[in]   addr: Pointer to Bluetooth address of peer device
 * @return      True if reconnection initiated, false otherwise
 ****************************************************************************************
 */
bool bts_iap2_service_reconnect(bt_bdaddr_t* addr);

/**
 ****************************************************************************************
 * @brief       Get local SPP channel used by IAP2 service
 * @return      Local SPP channel number
 ****************************************************************************************
 */
uint8_t bts_iap2_service_spp_local_channel();

/**
 ****************************************************************************************
 * @brief       Set MFi I2C GPIO configuration for IAP2 service
 * @param[in]   iap2_mfi_i2c_io_config: Pointer to I2C GPIO configuration structure
 * @return      None
 ****************************************************************************************
 */
void bts_iap2_service_set_mfi_i2c_gpio_config(bt_iap2_i2c_gpio_config_t* iap2_mfi_i2c_io_config);

/**
 ****************************************************************************************
 * @brief       Register MFi I2C callback for IAP2 service
 * @param[in]   read_cb: Pointer to I2C READ REG callback
 * @param[in]   write_cb: Pointer to I2C WRTIE REG callback
 * @return      None
 ****************************************************************************************
 */
void bts_iap2_service_register_mfi_i2c_callback(bt_mfi_i2c_read_reg_data_cb read_cb, bt_mfi_i2c_write_reg_data_cb write_cb);

/**
 ****************************************************************************************
 * @brief       Check MFi I2C state is idle
 * @return      True if idle, false otherwise
 ****************************************************************************************
 */
bool bts_iap2_service_check_mfi_i2c_is_idle();

/**
 ****************************************************************************************
 * @brief       Read MFi Version information
 * @return      None
 ****************************************************************************************
 */
void bts_iap2_service_read_mfi_version();

/**
 ****************************************************************************************
 * @brief       Read MFi Certificate information
 * @return      >0 means certificate length, <=0 means read failed
 ****************************************************************************************
 */
int bts_iap2_service_read_mfi_certificate(uint8_t* buf, uint16_t buf_len);

#ifdef __cplusplus
}
#endif

#endif
