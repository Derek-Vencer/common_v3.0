/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#ifndef __L2CAP_API_H__
#define __L2CAP_API_H__
#include "bluetooth.h"
#include "me_api.h"
#include "l2cap_service.h"
#ifdef __cplusplus
extern "C" {
#endif

enum PSM_CONTEXT_TYPE
{
     PSM_CONTEXT_SDP     = 0x01,  //SDP
     PSM_CONTEXT_RFC     = 0x02,  //RFCOMM MUX,HFP and SPP share
     PSM_CONTEXT_AVDTP   = 0x04,  //A2DP
     PSM_CONTEXT_AVCTP   = 0x08,  //AVRCP
     PSM_CONTEXT_BTGATT  = 0x10,  //GATT OVER BREDR

     PSM_CONTEXT_INVALID = 0x80,
};

typedef struct l2cap_conn l2cap_conn_t;

typedef struct {
    uint8_t smp_over_br_edr: 1;
    uint8_t gatt_over_br_edr: 1;
    uint8_t eatt_over_br_edr: 1;
} btif_l2cap_config_t;

btif_l2cap_config_t btif_l2cap_config_get(void);
void btif_l2cap_config_set(btif_l2cap_config_t *cfg);

void btif_l2cap_echo_init(void(*req_func)(const bt_bdaddr_t* bdaddr, uint8_t sigid, uint8_t* data, uint8_t len),
                          void(*res_func)(const bt_bdaddr_t* bdaddr, uint8_t sigid, uint8_t* data, uint8_t len),
                          void(*data_func)(const bt_bdaddr_t* bdaddr));
void btif_l2cap_process_echo_req_rewrite_rsp_data(const bt_bdaddr_t* bdaddr, uint8_t sigid, uint8_t* data, uint8_t len);
void btif_l2cap_process_echo_res_analyze_data(const bt_bdaddr_t* bdaddr,uint8_t sigid, uint8_t* data, uint8_t len);
uint8_t btif_l2cap_fill_in_echo_req_data(const bt_bdaddr_t *bdaddr, uint8_t *data, uint16_t len, uint8_t *sigid);
uint8_t btif_l2cap_fill_in_enco_rsp_data(const bt_bdaddr_t *bdaddr, uint8_t *data, uint16_t len, uint8_t sigid);

void btif_l2cap_register_sdp_disconnect_callback(void (*cb)(const bt_bdaddr_t* addr));
void btif_l2cap_register_get_ibrt_role_callback(uint8_t (*cb)(const bt_bdaddr_t* addr));
void btif_l2cap_register_get_ui_role_callback(uint8_t (*cb)(void));
void btif_l2cap_register_get_tss_state_callback(uint8_t (*cb)(const bt_bdaddr_t* addr));

uint16_t btif_l2cap_get_tx_mtu(uint32_t l2cap_handle);
void btif_create_besaud_extra_channel(void* remote_addr, bt_l2cap_callback_t l2cap_callback);
void btif_besaud_extra_channel_send_data(uint32_t l2cap_handle, const uint8_t *data, uint16_t len, void *context);
void btif_l2cap_reset_conn_sigid(const bt_bdaddr_t *addr);
bool btif_l2cap_is_profile_channel_connected(const bt_bdaddr_t *remote, uint8_t psm_context_mask);
bool btif_l2cap_is_dlci_channel_connected(uint32_t session_l2c_handle, uint64_t app_id);
bt_status_t btif_l2cap_send_bredr_security_manager_rsp(uint8 device_id, uint16 conn_handle, uint16 len, uint8 *data);

#ifdef __cplusplus
}
#endif
#endif /* __L2CAP_API_H__ */
