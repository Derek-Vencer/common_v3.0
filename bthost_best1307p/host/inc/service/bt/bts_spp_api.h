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

#ifndef __BTS_SPP_API_H__
#define __BTS_SPP_API_H__
#include "bt_spp_types.h"
#include "bts_bt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 ****************************************************************************************
 * @brief       Initialize a spp server.
 * @param[in]   local_server_channel: Indicate rfcomm channel used by spp server.
 * @param[in]   rx_buff_size: Indicate which channel the connection request come from.
 * @param[in]   params: Indicate which channel the connection request come from.
 * @param[in]   callbacks: Indicate which channel the connection request come from.
 * @return      Status of the operation.
 ****************************************************************************************
 */
bts_status_t bts_spp_init(bt_rfcomm_channel_t local_server_channel, uint16_t rx_buff_size, const bts_spp_sdp_params_t *params, const bt_spp_callbacks_t *callbacks);

/**
 ****************************************************************************************
 * @brief       Initialize a spp server.
 * @param[in]   local_server_channel: Indicate rfcomm channel used by spp server.
 * @return      Status of the operation.
 ****************************************************************************************
 */
bts_status_t bts_spp_deinit(bt_rfcomm_channel_t local_server_channel);

/**
 ****************************************************************************************
 * @brief       Set whether credits are sent by the upper layer.
 * @param[in]   local_server_channel: Indicate rfcomm channel used by spp server.
 * @param[in]   enable: Indicate whether the credits will be sent by yourself.
 *              - true: credits will be sent after data is processed by upper layer,
 *              call@bts_spp_send_credit.
 *              - false: credits are automatically sent by lower layer after data received.
 * @return      Status of the operation.
 ****************************************************************************************
 */
bts_status_t bts_spp_control_credit_by_self(bt_rfcomm_channel_t local_server_channel, bool enable);

/**
 ****************************************************************************************
 * @brief       Initialize a spp server.
 * @param[in]   local_server_channel: Indicate rfcomm channel used by spp server.
 * @param[in]   support_multi_device: Indicate whether allowed to be connected by multiple devices.
 * @return      Status of the operation.
 ****************************************************************************************
 */
bts_status_t bts_spp_server_start(bt_rfcomm_channel_t local_server_channel, bool support_multi_device);

/**
 ****************************************************************************************
 * @brief       Initialize a spp server.
 * @param[in]   local_server_channel: Indicate rfcomm channel used by spp server.
 * @return      Status of the operation.
 ****************************************************************************************
 */
bts_status_t bts_spp_server_stop(bt_rfcomm_channel_t local_server_channel);

/**
 ****************************************************************************************
 * @brief       Initiate a connection use uuid.
 * @param[in]   local_server_channel: Indicate rfcomm channel used by spp server.
 * @param[in]   uuid: Indicate rfcomm channel used by spp server.
 * @param[in]   uuid_len: Indicate rfcomm channel used by spp server.
 * @return      Status of the operation.
 ****************************************************************************************
 */
bts_status_t bts_spp_connect_by_uuid(const bt_bdaddr_t *address, bt_rfcomm_channel_t local_server_channel, const uint8_t *uuid, uint8_t uuid_len);

/**
 ****************************************************************************************
 * @brief       Initiate a connection use remote_server_channel.
 *              - The server channel of the remote has been obtained through other ways.
 * @param[in]   local_server_channel: Indicate rfcomm channel used by spp server.
 * @param[in]   remote_server_channel: Indicate rfcomm channel used by remote spp server.
 * @return      Status of the operation.
 ****************************************************************************************
 */
bts_status_t bts_spp_connect_by_channel(const bt_bdaddr_t *address, bt_rfcomm_channel_t local_server_channel, bt_rfcomm_channel_t remote_server_channel);

/**
 ****************************************************************************************
 * @brief       Initiate a disconnection.
 * @param[in]   connhdl: Spp connection handle.
 * @return      Status of the operation.
 ****************************************************************************************
 */
bts_status_t bts_spp_disconnect(bt_spp_connhdl_t connhdl, uint8_t reason);

/**
 ****************************************************************************************
 * @brief       Send spp data.
 * @param[in]   connhdl: spp connection handle.
 * @param[in]   data: Pointer to the data.
 * @param[in]   size: Data length.
 * @return      Status of the operation.
 ****************************************************************************************
 */
bts_status_t bts_spp_send_data(bt_spp_connhdl_t connhdl, const uint8_t *data, uint16_t size);

/**
 ****************************************************************************************
 * @brief       Get free send buffer size(how many packages can be cached).
 * @note        If free buffer is 0, upper layer cannot continue to send packets.
 * @param[in]   connhdl: spp connection handle.
 * @return      Free send buffer size.
 ****************************************************************************************
 */
uint16_t bts_spp_get_free_send_buf_size(bt_spp_connhdl_t connhdl);

/**
 ****************************************************************************************
 * @brief       Credits can be sent when the received data is processed.
 *              - see@bts_spp_control_credit_by_self
 * @param[in]   connhdl: Spp connection handle.
 * @param[in]   credits: Credits that need to be sent.
 * @return      Status of the operation.
 ****************************************************************************************
 */
bts_status_t bts_spp_send_credit(bt_spp_connhdl_t connhdl, uint8_t credits);

#ifdef __cplusplus
}
#endif

#endif /* __BTS_SPP_API_H__ */