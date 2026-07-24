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
#ifndef __RAP_CLIENT_H__
#define __RAP_CLIENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "gatt_service.h"
#include "ranging_common.h"

#define RAPC_INVALID_PRF_ID         GATT_PRF_INVALID
#define RAPC_CP_CMD_SEND_SUCCESS    true
#define RAPC_CP_CMD_SEND_FAILED    false

typedef enum
{
    RAPC_TIMER_RTM_W4_1ST_RAD_SEG,
    RAPC_TIMER_RTM_W4_NEXT_RAD_SEG,
    RAPC_TIMER_ODM_W4_RAD_READY,
    RAPC_TIMER_ODM_W4_1ST_RAD_SEG,
    RAPC_TIMER_ODM_W4_NEXT_RAD_SEG,
    RAPC_TIMER_TYPE_MAX_NUM,
} rapc_timer_type_t;

typedef enum
{
    RAPC_EXCEPTION_ATTR_DISC_FAILED,
    RAPC_EXCEPTION_TIMER_TIMEOUT,
    RAPC_EXCEPTION_CP_RSP_ERROR,

    RAPC_EXCEPTION_CONNECTION_LOSS,
    RAPC_EXCEPTION_RAS_DISCOVERY_IS_DOING,  // busy
    RAPC_EXCEPTION_RAS_INVALID_CCCD_PROP,
    RAPC_EXCEPTION_OUT_OF_MEMORY,
    RAPC_EXCEPTION_CANNOT_RETRIEVE_LOST_SEGS,
    // TODO
} rapc_exception_t;

typedef enum
{
    RAPC_EVENT_CONN_OPENED,
    RAPC_EVENT_CONN_CLOSED,
    RAPC_EVENT_SERVICE_DISCOVERY_CMPL,
    RAPC_EVENT_RECV_REAL_TIME_RANGING_DATA,
    RAPC_EVENT_SERVER_ON_DEMAND_RANGING_DATA_READY,
    RAPC_EVENT_SERVER_ON_DEMAND_RANGING_DATA_OVERWRITTEN,
    RAPC_EVENT_RECV_ON_DEMAND_RANGING_DATA,
} rapc_event_t;

typedef struct
{
    uint16_t connhdl;
    uint16_t mtu;
} rapc_event_conn_opened_param_t;

typedef struct
{
    uint16_t connhdl;
    uint8_t  err_code;
} rapc_event_conn_closed_param_t;

typedef struct
{
    uint16_t       connhdl;
    uint8_t        err_code;
    ras_features_t peer_feat;
} rapc_event_svc_disc_cmpl_param_t;

typedef struct
{
    uint16_t                     connhdl;
    const ras_ranging_header_t  *p_rad_header;
    uint8_t                      res_counter;
    const ras_subevent_result_t *p_ras_res;
    uint16_t                     len;
} rapc_event_recv_rtm_rad_param_t;

typedef struct
{
    uint16_t connhdl;
    uint16_t ranging_counter;
} racp_rad_info_param_t;

typedef racp_rad_info_param_t rapc_event_rad_ready_param_t;
typedef racp_rad_info_param_t rapc_event_rad_overwritten_param_t;

typedef struct
{
    uint16_t connhdl;
    uint8_t *p_intact_rad;
    uint16_t rad_len;
} rapc_event_recv_odm_rad_param_t;

typedef union
{
    void *param_ptr;
    rapc_event_conn_opened_param_t      *opened;
    rapc_event_conn_closed_param_t      *closed;
    rapc_event_svc_disc_cmpl_param_t    *svc_disc_cmpl;
    rapc_event_recv_rtm_rad_param_t     *recv_rtm_rad;
    rapc_event_rad_ready_param_t        *rad_ready;
    rapc_event_rad_overwritten_param_t  *rad_overwritten;
    rapc_event_recv_odm_rad_param_t     *recv_odm_rad;
} rapc_event_param_t;

typedef struct
{
    uint16_t        connhdl;
    ras_attr_type_t attr_type;
    uint8_t         err_code;
} rapc_exception_attr_disc_fail_param_t;

typedef struct
{
    uint16_t          connhdl;
    rapc_timer_type_t timer_type;
} rapc_exception_timer_timeout_param_t;

typedef struct
{
    uint16_t          connhdl;
    ras_cp_rsp_code_t rsp_code;
} rapc_exception_cp_rsp_error_param_t;

typedef struct
{
    uint16_t connhdl;
    uint8_t  err_code;
} rapc_exception_common_param_t;

typedef rapc_exception_common_param_t rapc_exception_conn_loss_param_t;
typedef rapc_exception_common_param_t rapc_exception_ras_disc_ongoing_param_t;
typedef rapc_exception_common_param_t rapc_exception_out_of_memory_param_t;

typedef struct
{
    uint16_t connhdl;
    uint16_t ranging_counter;
} rapc_exception_cannot_rtrv_lost_segs_param_t;

typedef union
{
    void *param_ptr;
    rapc_exception_attr_disc_fail_param_t        *attr_disc_fail;
    rapc_exception_timer_timeout_param_t         *timer_timeout;
    rapc_exception_cp_rsp_error_param_t          *cp_rsp_error;

    rapc_exception_conn_loss_param_t             *conn_loss;
    rapc_exception_ras_disc_ongoing_param_t      *ras_disc_ongoing;
    rapc_exception_out_of_memory_param_t         *out_of_memory;
    rapc_exception_cannot_rtrv_lost_segs_param_t *cannot_rtrv_lost_segs;
} rapc_exception_param_t;

typedef void (*rapc_event_cb_t)(rapc_event_t event, rapc_event_param_t param);
typedef void (*rapc_exception_cb_t)(rapc_exception_t exception, rapc_exception_param_t param);

typedef struct
{
    rapc_event_cb_t     event_cb;
    rapc_exception_cb_t exception_cb;
} rapc_upper_cb_t;

typedef struct
{
    bool           eatt_preferred;
    uint16_t       pref_mtu;
    rap_rad_mode_t preset_mode;
} rapc_config_t;

bt_status_t rapc_init(rapc_config_t *init_cfg);
bt_status_t rapc_deinit(void);

void rapc_register_callback(const rapc_upper_cb_t *cb);

rap_filter_cfg_t *rapc_get_filter_cfg(uint16_t connhdl);
void rapc_config_preset_rad_mode(uint16_t connhdl, rap_rad_mode_t rad_mode);

uint16_t rapc_get_connhdl_by_addr(const ble_bdaddr_t* p_ble_bdaddr);
rap_rad_mode_t rapc_get_current_rad_mode(uint16_t connhdl);
bool rapc_enable_rad_on_demand_mode(uint16_t connhdl);
bool rapc_enable_rad_real_time_mode(uint16_t connhdl);
bool rapc_disable_rad_rtm(uint16_t connhdl);
bool rapc_disable_rad_odm(uint16_t connhdl);

void rapc_discover_peer_ranging_service(uint16_t connhdl);
bool rapc_peer_supp_set_filter(uint16_t connhdl);
void rapc_get_on_demand_ranging_data(uint16_t connhdl, uint16_t ranging_counter);
void rapc_abort_on_demand_proc(uint16_t connhdl);
bt_status_t rapc_set_ranging_data_filter(uint16_t connhdl, uint8_t step_mode, uint16_t mask);

#if (BLE_GAP_CS_SUPPORT)
int rapc_cs_event_handler(cs_event_t event, cs_event_param_t param);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __RAP_CLIENT_H__ */
