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
#ifndef __BLE_RAPC_H__
#define __BLE_RAPC_H__

#include "bt_sys_config.h"

#include "cs_service.h"
#include "ras_common.h"

typedef struct
{
    cs_step_spec_type_t  step_type;
    cs_step_mode_t       step_mode;
    uint8_t             *p_step_data;
} lera_fmt_step_data_t;

typedef struct
{
    struct single_link_node_t node;
    uint8_t                   step_channel;
    lera_fmt_step_data_t      fmt_step_data;
} lera_local_fmt_step_t;

typedef struct
{
    uint16_t                    length;
    const cs_subevent_header_t *p_header;
    struct single_link_head_t   fmt_step_l;
    uint8_t                     mode_counter[CS_STEP_MODE_MAX_NUM];
} lera_local_fmt_sub_res_t;

typedef struct
{
    struct single_link_node_t node;
    uint16_t                  length;
    cs_subevent_result_t     *p_sub_res;
} lera_local_sub_res_t;

typedef struct
{
    uint16_t ranging_counter;
    uint8_t  config_id;
} lera_rad_header_t;

typedef struct
{
    struct single_link_node_t node;
    // * rad will be set intact when all subevent result have been cached
    bool                      intact;
    bool                      protected;
    lera_rad_header_t         rad_header;
    // * elem is `lera_local_sub_res_t`
    struct single_link_head_t sub_res_q;
} lera_local_rad_t;

typedef struct
{
    struct single_link_node_t node;
    lera_fmt_step_data_t      fmt_step_data;
} lera_peer_fmt_step_t;

typedef struct
{
    uint16_t                     length;
    /// @brief the subevent result header that defined by the RAS not the CS
    const ras_subevent_header_t *p_header;
    /// @brief the list contains all the step params of this subevent result (maybe filtered)
    ///         actually, every element of it contains related pointers that point to the corresponding position of the intact ranging data
    ///         which is reported by the RAP client module
    struct single_link_head_t    fmt_step_l;
    uint8_t                      mode_counter[CS_STEP_MODE_MAX_NUM];
} lera_peer_fmt_sub_res_t;

typedef struct
{
    uint16_t ffo;
    uint32_t rtt_dist;
    float    pbr_dist;
} lera_dist_info_t;

#endif  /* __BLE_RAPC_H__ */
