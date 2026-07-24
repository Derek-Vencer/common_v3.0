/**
 * @copyright Copyright (c) 2015-2022 BES Technic.
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
 */
#ifndef BLE_IF_H_
#define BLE_IF_H_

/***********************************header include********************************************/
#include "stdint.h"
#include "ke_task.h"
#include "hci_ble.h"


#ifdef __cplusplus
extern "C" {
#endif

/***********************************external function declaration*****************************/


/***********************************private macro defination***********************************/

/***********************************private type defination************************************/

/****************************private function declaration************************************/

/****************************private variable defination*************************************/

/**
 ****************************************************************************************
 * @brief Convenient wrapper to ke_msg_alloc()
 *
 * This macro calls ke_msg_alloc() and cast the returned pointer to the
 * appropriate structure. Can only be used if a parameter structure exists
 * for this message (otherwise, use ke_msg_send_basic()).
 *
 * @param[in] id        Message identifier
 * @param[in] dest      Destination Identifier
 * @param[in] src       Source Identifier
 * @param[in] param_str parameter structure tag
 *
 * @return Pointer to the parameter member of the ke_msg.
 ****************************************************************************************
 */
#define BLE_IF_MSG_ALLOC(id, dest, src, param_str) \
    (struct param_str*) ble_if_msg_alloc(id, dest, src, sizeof(struct param_str))

/**
 ****************************************************************************************
 * @brief Convenient wrapper to ke_msg_alloc() for defined structure definition
 *
 * This macro calls ke_msg_alloc() and cast the returned pointer to the appropriate structure.
 * Can only be used if a parameter structure exists for this message (otherwise, use ke_msg_send_basic()).
 *
 * @param[in] id        Message identifier
 * @param[in] dest      Destination Identifier
 * @param[in] src       Source Identifier
 * @param[in] param_str parameter structure tag
 *
 * @return Pointer to the parameter member of the ke_msg.
 ****************************************************************************************
 */
#define BLE_IF_MSG_ALLOC_T(id, dest, src, param_str) (param_str*)ble_if_msg_alloc(id, dest, src, sizeof(param_str))

/**
 ****************************************************************************************
 * @brief Convenient wrapper to ke_msg_alloc()
 *
 * This macro calls ke_msg_alloc() and cast the returned pointer to the
 * appropriate structure with a variable length. Can only be used if a parameter structure exists
 * for this message (otherwise, use ke_msg_send_basic()).Can only be used if the data array is
 * located at the end of the structure.
 *
 * @param[in] id        Message identifier
 * @param[in] dest      Destination Identifier
 * @param[in] src       Source Identifier
 * @param[in] param_str parameter structure tag
 * @param[in] length    length for the data
 *
 * @return Pointer to the parameter member of the ke_msg.
 ****************************************************************************************
 */
#define BLE_IF_MSG_ALLOC_DYN(id, dest, src, param_str,length)  (struct param_str*)ble_if_msg_alloc(id, dest, src, \
    (sizeof(struct param_str) + (length)));

/**
 ****************************************************************************************
 * @brief Convenient wrapper to ke_msg_alloc() for defined structure definition
 *
 * This macro calls ke_msg_alloc() and cast the returned pointer to the appropriate structure with a variable length.
 * Can only be used if a parameter structure exists for this message (otherwise, use ke_msg_send_basic()).
 * Can only be used if the data array is located at the end of the structure.
 *
 * @param[in] id        Message identifier
 * @param[in] dest      Destination Identifier
 * @param[in] src       Source Identifier
 * @param[in] param_str parameter structure tag
 * @param[in] length    length for the data
 *
 * @return Pointer to the parameter member of the ke_msg.
 ****************************************************************************************
 */
#define BLE_IF_MSG_ALLOC_DYN_T(id, dest, src, param_str, length) \
    (param_str*)ble_if_msg_alloc(id, dest, src, (sizeof(param_str) + (length)))

typedef void (*iso_receive_ntf_cb)(uint16_t conhdl, uint8_t pkt_status);

typedef struct host_to_recv_data_callback
{
    hci_to_app_callback_t hci_cb;
} host_to_app_callback_t;

void ble_if_init(host_to_app_callback_t *app_cb);

void ble_if_reset();

void ble_if_schedule();

uint8_t ble_if_task_create(uint8_t task_type, struct ke_task_desc const * p_task_desc);

uint8_t ble_if_task_delete(uint8_t task_type);

uint16_t ble_if_task_check(uint16_t task);

void ble_if_task_msg_retrieve(uint16_t id);

void ble_if_task_state_set(uint16_t const id, uint8_t const state_id);

uint8_t ble_if_task_state_get(uint16_t const id);

void* ble_if_msg_alloc(uint16_t const id, uint16_t const dest_id,
                             uint16_t const src_id, uint16_t const param_len);

void ble_if_msg_free(void* p);

void ble_if_msg_send(void const *param_ptr);

uint8_t ble_if_prf_get_task_from_id(uint8_t api_id);

int ble_if_iso_get_available_packet_num();

#ifdef BLE_ISO_ENABLED
void ble_if_iso_send_packet(struct hci_tx_iso_desc_t *tx_iso_desc);

void ble_if_iso_set_receive_ntf_callback(iso_receive_ntf_cb cb);
#endif

const struct data_path_itf* ble_if_data_path_itf_get(uint8_t type, uint8_t direction);

uint8_t ble_if_gatt_user_nb_get(uint8_t role);

bool ble_if_prf_buds_send_data_via_write_command(uint8_t conidx, uint8_t* ptrData, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif /* BLE_IF_H_ */
