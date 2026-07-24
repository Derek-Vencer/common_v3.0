/***************************************************************************
 *
 * Copyright (c) 2015-2024 BES Technic
 *
 * Authored by BES CD team (Blueelf Prj).
 *
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
#ifndef __RAP_SERVER_H__
#define __RAP_SERVER_H__

#include "cs_i.h"
#include "ranging_common.h"
#include "ras_i.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    RAPS_EXCEPTION_OUT_OF_MEMORY,
    RAPS_EXCEPTION_W4_ACK_OD_RAD_TIMEOUT,
} raps_exception_t;

typedef enum
{
    RAPS_EVENT_CONNECTED,
    RAPS_EVENT_DISCONNECTED,
    RAPS_EVENT_GET_FILTER_CFG_FROM_NV,
    RAPS_EVENT_STORE_FILTER_CFG_IN_NV,
} raps_event_t;

typedef struct
{
    uint16_t connhdl;
    uint16_t mtu;
    uint8_t  err_code;
} raps_event_connected_param_t;

typedef struct
{
    uint16_t connhdl;
    uint8_t  err_code;
} raps_event_disconnected_param_t;

typedef struct
{
    uint16_t           connhdl;
    rap_filter_cfg_t **pp_filter_cfg;
} raps_event_get_filter_cfg_param_t;

typedef struct
{
    uint16_t          connhdl;
    rap_filter_cfg_t *p_filter_cfg;
} raps_event_store_filter_cfg_param_t;

typedef union
{
    void *param_ptr;
    raps_event_connected_param_t        *connected;
    raps_event_disconnected_param_t     *disconnected;
    raps_event_get_filter_cfg_param_t   *get_filter_cfg;
    raps_event_store_filter_cfg_param_t *store_filter_cfg;
} raps_event_param_t;

typedef struct
{
    uint16_t connhdl;
} raps_exception_out_of_memory_param_t;

typedef struct
{
    uint16_t connhdl;
} raps_exception_w4_ack_timeout_param_t;

typedef union
{
    void *param_ptr;
    raps_exception_out_of_memory_param_t  *out_of_memory;
    raps_exception_w4_ack_timeout_param_t *w4_ack_timeout;
} raps_exception_param_t;

typedef void (*raps_event_cb_t)(raps_event_t event, raps_event_param_t param);
typedef void (*raps_exception_cb_t)(raps_exception_t exception, raps_exception_param_t param);

typedef struct
{
    raps_event_cb_t     event_cb;
    raps_exception_cb_t exception_cb;
} raps_upper_cb_t;

// TODO consider to do the configuration outside (i.e. in the Makefile by defining Macros)
typedef struct
{
    /// @brief true or false, the RAP server will support the real time ranging data mode
    uint8_t real_time_mode : 1;
    /// @brief true or false, the RAP server will support the retrieval of the lost ranging data (Control Point Operation)
    uint8_t cp_lost_retrieval : 1;
    /// @brief true or false, the RAP server will support the abort operation of Control Point
    uint8_t cp_abort_op : 1;
    /// @brief true or false, the RAP server will support the set filter operation of Control Point
    uint8_t cp_set_filter : 1;
    /// @brief do not use it, reserve for future use
    uint8_t reserved : 4;
} raps_capas_t;

// TODO add more for configuration
typedef struct
{
    raps_capas_t raps_capas;
    ras_config_t ras_cfg;
} raps_config_t;

bt_status_t raps_init(raps_config_t *init_cfg);
bt_status_t raps_deinit(void);

extern void raps_register_callback(const raps_upper_cb_t *cb);
extern void raps_recv_subevent_result_handle(cs_recv_sub_res_param_t *p_recv_sub_res_param);

#ifdef __cplusplus
}
#endif

#endif  /* __RAP_SERVER_H__ */
