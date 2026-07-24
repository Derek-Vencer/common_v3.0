
/***************************************************************************
 *
 * Copyright 2015-2022 BES.
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
#ifndef __BT_SPP_SERVICE_H__
#define __BT_SPP_SERVICE_H__
#include "spp_api.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifdef BLE_ONLY_ENABLED
#define bta_spp_create_port(local_server_channel, attr_list, attr_count) ((bt_status_t)0)
#define bta_spp_set_callback(local_server_channel, rx_buff_size, spp_callback, client_callback) ((bt_status_t)0)
#define bta_spp_set_app_layer_give_credit(local_server_channel,app_layer_give_credit) ((bt_status_t)0)
#define bt_adapter_spp_listen(local_server_channel, support_multi_device, accept_callback) ((bt_status_t)0)
#define bta_spp_remove_listen(local_server_channel) ((bt_status_t)0)
#define bta_spp_server_listen(config) ((bt_status_t)0)
#define bta_spp_server_remove_listen(local_server_channel) ((bt_status_t)0)
#define bta_spp_connect(remote, local_server_channel, uuid, uuid_len) ((bt_status_t)0)
#define bta_spp_connect_server_channel(remote, local_server_channel, remote_server_channel) ((bt_status_t)0)
#define bta_spp_disconnect(rfcomm_handle, reason) ((bt_status_t)0)
#define bta_spp_write(rfcomm_handle, data, size) ((bt_status_t)0)
#define bta_spp_get_record_uuid(spp_chan, out) ((bt_status_t)0)
#define bta_spp_give_handled_credits(rfcomm_handle, handled_credits) ((bt_status_t)0)
#define bta_spp_create_channel(device_id, local_server_channel) ((bt_spp_channel_t *)NULL)
#define bta_spp_count_free_tx_buf_cnt(rfcomm_handle) ((uint16_t)0)
#define bta_spp_is_connected(rfcomm_handle) ((bool)0)
#define bta_spp_is_port_connected(remote, local_server_channel) ((bool)0)
#define bta_spp_get_app_id_from_port(local_server_channel) ((uint64_t)0)
#define bta_spp_get_port_from_app_id(spp_app_id) ((uint8_t)0)
#else
bt_status_t bta_spp_create_port(uint8_t local_server_channel, const bt_sdp_record_attr_t *attr_list, uint16_t attr_count);
bt_status_t bta_spp_set_callback(uint8_t local_server_channel, uint16_t rx_buff_size, bt_spp_callback_t spp_callback, bt_spp_callback_t client_callback);
bt_status_t bta_spp_set_app_layer_give_credit(uint8_t local_server_channel, bool app_layer_give_credit);
bt_status_t bt_adapter_spp_listen(uint8_t local_server_channel, bool support_multi_device, bt_spp_accept_callback_t accept_callback);
bt_status_t bta_spp_remove_listen(uint8_t local_server_channel);
bt_status_t bta_spp_server_listen(const bt_spp_server_config_t* config);
bt_status_t bta_spp_server_remove_listen(uint8_t local_server_channel);
bt_status_t bta_spp_connect(const bt_bdaddr_t *remote, uint8_t local_server_channel, const uint8_t *uuid, uint16_t uuid_len);
bt_status_t bta_spp_connect_server_channel(const bt_bdaddr_t *remote, uint8_t local_server_channel, uint8_t remote_server_channel);
bt_status_t bta_spp_disconnect(uint32_t rfcomm_handle, uint8_t reason);
bt_status_t bta_spp_write(uint32_t rfcomm_handle, const uint8_t *data, uint16_t size);
bt_status_t bta_spp_get_record_uuid(const bt_spp_channel_t *spp_chan, bt_spp_uuid_t *out);
bt_status_t bta_spp_give_handled_credits(uint32_t rfcomm_handle, uint16_t handled_credits);
bt_spp_channel_t *bta_spp_create_channel(uint8_t device_id, uint8_t local_server_channel);
uint16_t bta_spp_count_free_tx_buf_cnt(uint32_t rfcomm_handle);
bool bta_spp_is_connected(uint32_t rfcomm_handle);
bool bta_spp_is_port_connected(const bt_bdaddr_t *remote, uint8_t local_server_channel);
uint64_t bta_spp_get_app_id_from_port(uint8_t local_server_channel);
uint8_t bta_spp_get_port_from_app_id(uint64_t spp_app_id);
#endif
#ifdef __cplusplus
}
#endif
#endif /* __BT_SPP_SERVICE_H__ */

