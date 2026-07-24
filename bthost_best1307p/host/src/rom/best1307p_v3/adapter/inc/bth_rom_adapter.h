/***************************************************************************
 *
 * Copyright 2015-2024 BES.
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
 * @brief xxx.
 *
 ****************************************************************************/
#ifndef __BTH_ROM_ADAPTER_H__
#define __BTH_ROM_ADAPTER_H__

#ifdef __cplusplus
extern "C" {
#endif

/****************************** header include ********************************/
#include <stdint.h>

#include "hal_trace.h"

#include "bt_common_define.h"

/***************************** external declaration *****************************/

/***************************** macro defination *******************************/
#define TRACE(attr, str, ...)               TR_INFO(attr, str, ##__VA_ARGS__)

/*****************************  type defination ********************************/

typedef void btif_dip_client_t;

typedef void (*ibrt_cmd_complete_callback)(const uint8_t *para);
typedef void (*bt_hci_cmd_status_callback_t)(const void *para);
typedef void (*btif_hci_vendor_event_callback_func)(uint8_t* pbuf, uint32_t length);
typedef bt_iocap_requirement_t *(*bt_iocap_request_callback_func)(uint8_t device_id, uint16_t connhdl, const bt_bdaddr_t *remote, const bt_iocap_requirement_t *remote_initiate);

/*****************************  variable defination *****************************/

/*****************************  function declaration ****************************/
void *bth_rom_get_coheap_global(void);

void* bth_rom_adapter_bt_fn_get(void);

void* bth_rom_adapter_nv_fn_get(void);

void* bth_rom_adapter_sys_fn_get(void);

void* bth_rom_adapter_utils_fn_get(void);

#ifdef __cplusplus
}
#endif

bool besbt_cfg_need_handle_specially(uint8_t* reason_code);

int bth_rom_hci_send_data(const uint8_t *buf, uint16_t size);

#endif /* __BTH_ROM_ADAPTER_H__ */

