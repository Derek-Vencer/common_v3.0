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

#include "bluetooth_ble_api.h"

/**
 * @brief Get the number of ble connections
 *
 * @return uint8_t
 */
uint8_t audio_datapath_ble_get_connect_device_num(void);

/**
 * @brief Get ble all nv record (max BLE_RECORD_NUM 5)
 *
 * @return uint8_t
 */
uint8_t audio_datapath_ble_get_all_paired_dev(bes_ble_bdaddr_t *bleAddr);

/**
 * @brief Establish ble acl link with addr
 *
 * @param bleAddr
 */
void audio_datapath_ble_start_connect(bes_ble_bdaddr_t *bleAddr);

/**
 * @brief disconnt all ble acl link
 */
void audio_datapath_ble_disconnect_all(void);

/**
 * @brief disconnt ble acl link by addr
 *
 * @param bes_ble_bdaddr_t
 */
void audio_datapath_ble_stop_connect_by_addr(bes_ble_bdaddr_t *bleAddr);

/**
 * @brief delete ble nv record by addr
 *
 * @param bes_ble_bdaddr_t
 */
void audio_datapath_ble_del_nv_record(bes_ble_bdaddr_t *bleAddr);

/**
 * @brief delete all ble nv record
 */
void audio_datapath_ble_del_all_nv_record(void);

/**
 * @brief enter scan to pairing with new device
 *
 * @param bool
 */
void audio_datapath_ble_enter_scan_to_pairing(bool scan);

void audio_datapath_ble_aob_media_start(uint8_t condix, uint16_t frame_octet);

void audio_datapath_ble_init(void);

bes_lea_ase_cfg_param_t* audio_datapath_ble_ase_info(void);

void audio_datapath_ble_dump_dev_info(void);

#ifdef __cplusplus
extern "C" {
#endif

void audio_datapath_ble_server_set_active_client_conidx(uint8_t conidx);

uint8_t audio_datapath_ble_server_get_active_client_conidx(void);

uint8_t* audio_datapath_ble_server_get_client_conn(void);

#ifdef __cplusplus
}
#endif

#endif
