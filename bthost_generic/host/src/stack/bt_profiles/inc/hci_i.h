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
#ifndef __HCI_I_H__
#define __HCI_I_H__

#include "hci_transport.h"
#include "co_hci.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef void (*hci_tx_buf_tss_process_cb_type)(void);
typedef void (*bt_hci_acl_ecc_softbit_handler_func)(uint16_t*,uint16_t*, uint16_t, uint8_t*);
void register_hci_acl_ecc_softbit_handler_callback(bt_hci_acl_ecc_softbit_handler_func func);

bool hci_tx_buff_process(void);
void hci_rx_handle(struct hci_rx_desc_t *rx_desc);
struct pp_buff *hci_fc_each_link_has_tx_chance(hci_conn_type_t conn_type);

bool hci_tx_buff_process(void);
void hci_rx_handle(struct hci_rx_desc_t *rx_desc);
struct pp_buff *hci_fc_each_link_has_tx_chance(hci_conn_type_t conn_type);

bt_status_t hci_simulate_event(const uint8_t *buff, uint16_t buff_len);
bt_status_t hci_send_cmd_direct(const uint8_t *cmd_packet, uint8_t packet_len);
struct hci_conn_item_t *hci_find_conn_by_address(hci_conn_type_t conn_type, bt_addr_type_t addr_type, const bt_bdaddr_t *peer_addr);
void hci_foreach_acl_conn_item(hci_conn_type_t conn_type, int (*cb)(void *priv_param, void *param), void *param);
struct hci_conn_item_t *hci_find_conn_by_handle(hci_conn_type_t type, uint16_t connhdl);
struct hci_conn_item_t *hci_find_acl_conn_item(hci_conn_type_t type, uint16_t connhdl);
struct hci_conn_item_t *hci_alloc_le_conn_and_report(uint16_t connhdl, uint8_t is_central,
                                                        uint8_t peer_addr_type, const bt_bdaddr_t *peer_addr);

#if defined(__cplusplus)
}
#endif
#endif /* __HCI_I_H__ */
