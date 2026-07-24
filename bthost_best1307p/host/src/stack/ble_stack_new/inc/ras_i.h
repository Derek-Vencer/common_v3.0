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
#ifndef __RAS_I_H__
#define __RAS_I_H__

#include "bt_sys_config.h"

#include "gatt_service.h"
#include "ras_common.h"

typedef enum
{
    RAS_EXCEPTION_CONTROL_POINT_OPERATION,
} ras_exception_t;

typedef struct
{
    uint16_t          connhdl;
    ras_cp_rsp_code_t rsp_code;
} ras_exception_cp_op_param_t;

typedef union
{
    void *param_ptr;
    ras_exception_cp_op_param_t *cp_op;
} ras_exception_param_t;

typedef struct
{
    /// @brief We assume that RAS is connected only after the related LE ACL connection is encrypted
    void (*conn_opened_cb)(uint8_t con_idx, uint16_t connhdl);
    /// @brief release the related connection mgr of this RAS in RAP server (upper layer)
    void (*conn_closed_cb)(uint16_t connhdl);

    void (*mtu_changed_cb)(uint16_t connhdl, uint16_t mtu);
    void (*conn_encrypted_cb)(uint16_t connhdl);

    void (*write_control_point_cb)(uint16_t connhdl, ras_cp_cmd_type_t op_code, ras_cp_cmd_param_t param, uint8_t param_len);

    att_error_code_t (*read_char_val_cb)(uint16_t connhdl, uint16_t char_uuid, const uint8_t **const pp_data, uint16_t *const p_len);

    att_error_code_t (*read_char_cccd_cb)(uint16_t connhdl, uint16_t char_uuid, uint16_t *const p_cccd_val);
    att_error_code_t (*write_char_cccd_cb)(uint16_t connhdl, uint16_t char_uuid, uint16_t *const p_cccd_val);

    void (*char_ntf_tx_done_cb)(uint16_t connhdl, uint16_t char_uuid, uint8_t err_code);
    void (*char_ind_cfm_cb)(uint16_t char_uuid, ras_char_ind_cfm_param_t param);

    void (*exception_cb)(ras_exception_t exception, ras_exception_param_t param);
} ras_upper_cb_t;

typedef struct
{
    bool     eatt_preferred;
    uint16_t pref_mtu;
    // TODO
} ras_config_t;

#define RAS_CHAR_CCCD_NUM                   5

typedef struct
{
    uint8_t             con_idx;
    uint16_t            connhdl;
    /// @brief Service Instance Identification, unused now
    uint8_t             svc_ins_id;
    /// TODO consider whether store config or not, it
    ras_config_t        config;
    /// @brief RAS Features characteristic value
    ras_features_t      features_char_val;
    /// @brief RAS Ranging Data Ready characterisitc value ~ Ranging Counter
    uint16_t            rad_ready_char_val;
    /// @brief RAS Ranging Data overwritten characterisitc value ~ Ranging Counter
    uint16_t            rad_overwritten_char_val;
    // *seemingly, we do not need always store control point characteristic on the server, and we just need perform the control point command immediately
    uint16_t            char_cccd_val[RAS_CHAR_CCCD_NUM];
} ras_info_t;

extern bt_status_t ras_init(const ras_config_t *init_cfg, const ras_upper_cb_t *cb);
extern bt_status_t ras_deinit();

uint8_t ras_get_gap_con_idx_by_connhdl(uint16_t connhdl);

extern bt_status_t ras_notify_real_time_ranging_data(const uint8_t con_idx, const uint8_t *data, const uint16_t len);
extern bt_status_t ras_notify_on_demand_ranging_data(const uint8_t con_idx, const uint8_t *data, const uint16_t len);
extern bt_status_t ras_indicate_real_time_ranging_data(const uint8_t con_idx, const uint8_t *data, const uint16_t len);
extern bt_status_t ras_indicate_on_demand_ranging_data(const uint8_t con_idx, const uint8_t *data, const uint16_t len);
extern bt_status_t ras_indicate_control_point(const uint8_t con_idx, const uint8_t *data, const uint16_t len);
extern bt_status_t ras_indicate_ranging_data_ready(const uint8_t con_idx, const uint8_t *data, const uint16_t len);
extern bt_status_t ras_indicate_ranging_data_overwritten(const uint8_t con_idx, const uint8_t *data, const uint16_t len);

#endif  /* __RAS_I_H__ */
