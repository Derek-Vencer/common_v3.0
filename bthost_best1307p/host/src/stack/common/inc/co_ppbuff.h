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
#ifndef __CO_PPBUFF_H__
#define __CO_PPBUFF_H__
#include "bt_common_define.h"
#if defined(__cplusplus)
extern "C" {
#endif

struct pp_buff_flag_t {
    uint16_t conn_type: 4;
    uint16_t is_alloc_inplace: 1;
    uint16_t is_buff_auto_grow: 1;
    uint16_t dont_free_rx_ppb : 1;
    uint16_t defer_free_rx_ppb: 1;
    uint16_t insert_to_tx_header: 1;
    uint16_t dont_report_tx_done: 1;
    uint16_t tx_cont_fragment: 1;
    uint16_t dont_auto_free_by_hci: 1;
    uint16_t wait_cmpl_status_event: 1;
    uint16_t is_hci_rx_desc_ppb: 1;
    uint16_t is_sent_by_ble: 1;
    uint16_t dont_free_when_tx_fc: 1;
};

struct pp_buff {
    struct single_link_node_t node;
    struct pp_buff_flag_t ppb_flag;
    uint16_t buff_size;
    uint16_t len; // data len
    uint16_t data_start;
    void *context;
};

uint8_t *ppb_buff_head(struct pp_buff *ppb);
uint8_t *ppb_buff_end(struct pp_buff *ppb);
uint8_t *ppb_get_data(struct pp_buff *ppb);
uint8_t *ppb_data_end(struct pp_buff *ppb);
uint16_t ppb_get_len(struct pp_buff *ppb);
uint16_t ppb_unused_len(struct pp_buff *ppb);

#if defined(__cplusplus)
}
#endif
#endif /* __CO_PPBUFF_H__ */