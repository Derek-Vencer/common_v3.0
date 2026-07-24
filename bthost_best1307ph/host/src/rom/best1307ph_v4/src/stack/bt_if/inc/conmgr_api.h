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

#ifndef __CONMGR_API_H__
#define __CONMGR_API_H__

typedef U8 btif_cmgr_sniff_exit_policy_t;

typedef void btif_cmgr_handler_t;

#define BTIF_CMGR_SNIFF_EXIT_ON_SEND     0x01

#define BTIF_CMGR_SNIFF_EXIT_ON_AUDIO    0x02

typedef void (*btif_cmgr_callback) (void *handler, uint8_t event, int status);

#ifdef __cplusplus
extern "C" {
#endif                          /*  */

bool btif_cmgr_is_snifff_timer_running(btif_cmgr_handler_t * cmgr_handler);

uint32_t btif_cmgr_get_cmgrhandler_sniff_timeout(btif_cmgr_handler_t * cmgr_handler);

uint16_t  btif_cmgr_get_cmgrhandler_sniff_interval(btif_cmgr_handler_t * cmgr_handler);

btif_remote_device_t *btif_cmgr_get_cmgrhandler_remdev(btif_cmgr_handler_t *cmgr_handler);

uint16_t btif_cmgr_get_cmgrhandler_remdev_hci_handle(btif_cmgr_handler_t * cmgr_handler);

bt_status_t btif_cmgr_init_sniff_timer(btif_remote_device_t *remDev);

bt_status_t btif_cmgr_deinit_sniff_timer(btif_remote_device_t *remDev);

bt_status_t btif_cmgr_clear_sniff_timer(btif_cmgr_handler_t * Handler);

bt_status_t btif_cmgr_disable_sniff_timer(btif_cmgr_handler_t * Handler);

uint16_t btif_cmgr_get_sco_connect_sco_Hcihandler(btif_cmgr_handler_t * cmgr_handler);

uint8_t btif_cmgr_get_sco_connect_sco_rx_parms_sco_transmission_interval(btif_cmgr_handler_t *
                                                                         cmgr_handler);

uint8_t btif_cmgr_get_sco_connect_sco_rx_parms_sco_retransmission_window(btif_cmgr_handler_t *
                                                                         cmgr_handler);

uint8_t btif_cmgr_set_sco_connect_sco_rx_parms_sco_transmission_interval(
                                                        btif_cmgr_handler_t * cmgr_handler,
                                                        uint8_t interval);

uint8_t btif_cmgr_set_sco_connect_sco_rx_parms_sco_retransmission_window(
                                                        btif_cmgr_handler_t *cmgr_handler,
                                                        uint8_t window);

bool btif_cmgr_is_audio_up(btif_cmgr_handler_t *Handler);

void btif_evm_start_timer(evm_timer_t *timer, TimeT ms);

void *btif_cmgr_get_default_conn_handler(void);

void btif_cmgr_handler_init(void);

bt_status_t btif_cmgr_register_handler(btif_cmgr_handler_t *cmgr_handler,
                                       btif_cmgr_callback callback);

bt_status_t btif_cmgr_create_data_link(btif_cmgr_handler_t *cmgr_handler, const bt_bdaddr_t *bd_addr, uint32_t page_timeout, uint32_t time_to_next_page);

bt_status_t btif_cmgr_remove_data_link(const bt_bdaddr_t *remote);

btif_cmgr_handler_t *btif_cmgr_get_acl_handler(uint16_t conn_handle);

btif_cmgr_handler_t *btif_lock_free_cmgr_get_acl_handler(btif_remote_device_t *remDev);

btif_cmgr_handler_t * btif_cmgr_get_conn_ind_handler(uint16_t conn_handle);

bool btif_cmgr_is_link_up(btif_cmgr_handler_t *cmgr_handler);
btif_remote_device_t *btif_cmgr_pts_get_remDev(btif_cmgr_handler_t *cmgr_handler);

#ifdef __cplusplus
}
#endif                          /*  */
#endif                          /*  */

