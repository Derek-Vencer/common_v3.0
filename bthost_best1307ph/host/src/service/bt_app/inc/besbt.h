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
#ifndef BESBT_H
#define BESBT_H
#include "bt_common_define.h"
#ifdef __cplusplus
extern "C" {
#endif

#define STACK_READY_BT  0x01
#define STACK_READY_BLE 0x02



void bt_host_thread_init(int priority, void (*init_done_cb)());
void bt_host_thread_deinit();
void bt_host_thread_notify(void);
void bt_host_ready(uint8_t ready_flag);
int Besbt_hook_handler_set(enum BESBT_HOOK_USER_T user, BESBT_HOOK_HANDLER handler);
void app_bt_set_main_priority(int priority);
int app_bt_get_main_priority(void);
void gen_bt_addr_for_debug(void);
bool app_bt_is_besbt_thread(void);
void bt_host_config_checker_logging(uint32_t interval);

unsigned char *bt_get_local_address(void);
void bt_set_local_address(unsigned char *btaddr);
unsigned char *bt_get_ble_local_address(void);
void bt_set_ble_local_address(uint8_t *bleAddr);
const char *bt_get_local_name(void);
void bt_set_local_name(const char *name);
const char *bt_get_ble_local_name(void);
void bt_set_ble_local_name(const char *name);

int app_bt_start_custom_function_in_bt_thread(uint32_t param0, uint32_t param1, uint32_t funcPtr);
int app_bt_call_func_in_bt_thread(uint32_t param0, uint32_t param1, uint32_t param2, uint32_t param3, uint32_t funcPtr);

uint8_t bes_bt_me_count_mobile_link(void);
void bes_bt_a2dp_report_speak_gain(void);
void bes_bt_a2dp_key_handler(uint8_t a2dp_key);
void bes_bt_hfp_report_speak_gain(void);
void app_bt_register_tws_txrx_handler(void (*cb)(void));

void app_bt_l2cap_echo_cus_register(
    void(*echo_req)(const bt_bdaddr_t* bdaddr, uint8_t sigid, uint8_t* data, uint8_t len),
    void(*echo_rsp)(const bt_bdaddr_t* bdaddr,uint8_t sigid, uint8_t* data, uint8_t len));

#ifdef __cplusplus
}
#endif
#endif /* BESBT_H */
