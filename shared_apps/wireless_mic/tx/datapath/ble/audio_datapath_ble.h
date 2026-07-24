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
#ifndef _AUDIO_DATAPATH_BLE_H_
#define _AUDIO_DATAPATH_BLE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "bluetooth_ble_api.h"

/**
 * @brief ble start connectable adv
 */
void audio_datapath_ble_start_adv(void);

/**
 * @brief ble stop adv
 */
void audio_datapath_ble_stop_adv(void);

/**
 * @brief Get the number of ble connections
 *
 * @return uint8_t
 */
uint8_t audio_datapath_ble_get_connect_device_num(void);

/**
 * @brief disconnt all ble acl link
 */
void audio_datapath_ble_disconnect_all(void);

/**
 * @brief delete ble nv record by addr
 *
 * @param bleAddr xx:xx:xx:xx:xx:xx
 */
void audio_datapath_ble_del_nv_record(uint8_t *bleAddr);

/**
 * @brief delete all ble nv record
 */
void audio_datapath_ble_del_all_nv_record(void);

/**
 * @brief Get ble all nv record (max BLE_RECORD_NUM 5)
 *
 * @return uint8_t
 */
uint8_t audio_datapath_ble_get_all_paired_dev(bes_ble_bdaddr_t *bleAddr);

void audio_datapath_ble_init(void);

void audio_datapath_ble_output_stream_onoff(bool onoff);

#ifdef __cplusplus
}
#endif

#endif //_VOW_DATAPATH_BLE_H_
