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
#ifndef __CS_I_H__
#define __CS_I_H__

#include "bt_sys_config.h"

#include "bluetooth.h"
#include "cs_service.h"

enum cs_gap_evt
{
    /// Indicate that CS is connected
    CS_GAP_EVT_LE_ENCRYPTED,
    CS_GAP_EVT_LE_UNENCRYPTED,
    CS_GAP_EVT_LE_DISCONNECTED,
};

typedef struct
{
    uint8_t t_ip1_time;
    uint8_t t_ip2_time;
    uint8_t t_fcs_time;
    uint8_t t_pm_time;
} cs_step_time_dura_t;

typedef struct
{
    cs_basic_config_t   basic_config;
    cs_step_time_dura_t step_time_dura;
} cs_inner_basic_config_t;

typedef struct
{
    struct single_link_node_t node;
    uint8_t                   config_id;
    cs_inner_basic_config_t   inner_basic_config;
    cs_set_proc_params_t      inner_cfg_params;
    bool                      proc_params_is_set;
} cs_conn_config_t;

typedef struct
{
    /// @brief preconfigure number will not exceed the `num_config_supp` with both sides.
    uint8_t                preconfig_num;
    cs_basic_config_t     *p_basic_configs[CS_MAX_CONFIG_NUM];
    cs_default_settings_t *p_default_settings;
    cs_set_proc_params_t  *p_set_proc_params;
} cs_preset_t;

typedef struct
{
    // TODO use the preset
    cs_preset_t     preset;
    cs_supp_capas_t local_supp_capas;
} cs_local_info_t;

bt_status_t cs_init(void);
bt_status_t cs_deinit(void);
void cs_intrinsic_event_handler(uint8_t subcode, const uint8_t *evt_data, uint8_t len);
void cs_recv_gap_conn_event(enum cs_gap_evt event, uint16_t connhdl);

#endif  /* __CS_I_H__ */
