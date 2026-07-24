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
#ifndef __RAS_COMMON_H__
#define __RAS_COMMON_H__

#include "bt_sys_config.h"

// TODO ! do not use nv directly in the stack
#include "bluetooth.h"

#define RAS_RETRIEVE_ALL_REMAINING_LOST_SEGS            0xFF

// RAS Features Support
#define RAS_FEATURES_NONE                               0x0000
#define RAS_FEATURES_REAL_TIME_RANGING_DATA             0x0001
#define RAS_FEATURES_RETRIEVE_LOST_RANGING_DATA_SEGS    0x0002
#define RAS_FEATURES_ABORT_OPERATION                    0x0004
#define RAS_FEATURES_FILTER_RANGING_DATA                0x0008

typedef enum
{
    RAS_RANGING_DATA_DONE_ALL,
    RAS_RANGING_DATA_DONE_PARTIAL,
    RAS_RANGING_DATA_DONE_ABORT = 0x0F,
} ras_rad_done_status_t;

typedef enum
{
    RAS_CCCD_VAL_NONE,
    RAS_CCCD_VAL_NOTIFICATION,
    RAS_CCCD_VAL_INDICATION,
} ras_cccd_val_t;

typedef enum
{
    RAS_ATTR_TYPE_RANGING_SERVICE,
    RAS_ATTR_TYPE_FEATURES,
    RAS_ATTR_TYPE_REAL_TIME_RANGING_DATA,
    RAS_ATTR_TYPE_REAL_TIME_RANGING_DATA_CCCD,
    RAS_ATTR_TYPE_ON_DEMAND_RANGING_DATA,
    RAS_ATTR_TYPE_ON_DEMAND_RANGING_DATA_CCCD,
    RAS_ATTR_TYPE_CONTROL_POINT,
    RAS_ATTR_TYPE_CONTROL_POINT_CCCD,
    RAS_ATTR_TYPE_RANGING_DATA_READY,
    RAS_ATTR_TYPE_RANGING_DATA_READY_CCCD,
    RAS_ATTR_TYPE_RANGING_DATA_OVERWRITTEN,
    RAS_ATTR_TYPE_RANGING_DATA_OVERWRITTEN_CCCD,
    RAS_ATTR_TYPE_UNKNOWN,
} ras_attr_type_t;

#define RAS_FEATURES_SUPP_RT_RAD_MASK      CO_BIT_MASK(0)
#define RAS_FEATURES_SUPP_RLRS_MASK        CO_BIT_MASK(1)
#define RAS_FEATURES_SUPP_ABORT_OP_MASK    CO_BIT_MASK(2)
#define RAS_FEATURES_SUPP_FILTER_RAD_MASK  CO_BIT_MASK(3)

typedef enum
{
    RAS_CP_OPCODE_CMD_GET_RANGING_DATA,
    RAS_CP_OPCODE_CMD_ACK_RANGING_DATA,
    /// @brief RLRD --> Retrieve Lost Ranging Data
    RAS_CP_OPCODE_CMD_RLRD_SEGMENTS,
    RAS_CP_OPCODE_CMD_ABORT_OPERATION,
    RAS_CP_OPCODE_CMD_SET_FILTER,
} ras_cp_cmd_type_t;

typedef enum
{
    RAS_CP_OPCODE_RSP_CMPL_RANGING_DATA,
    RAS_CP_OPCODE_RSP_CMPL_LOST_RANGING_DATA_SEGMENT,
    RAS_CP_OPCODE_RSP_CODE,
} ras_cp_rsp_type_t;

/// @brief response code value actually only 1 bytes
typedef enum
{
    // * 0x00 is reserved for future use.
    RAS_CP_RSP_CODE_SUCCESS = 0x01,         // * Normal response for a successful operation.
    RAS_CP_RSP_CODE_OP_CODE_NOT_SUPPORTED,  // * Normal response if an unsupported Op Code is received.
    RAS_CP_RSP_CODE_INVALID_PARAM,          // * Normal response if Parameter received does not meet the requirements of the service.
    RAS_CP_RSP_CODE_SUCCESS_PERSISTED,      // * Normal response for a successful write operation where the values written to the RAS Control Point are being persisted.
    RAS_CP_RSP_CODE_ABORT_UNSUCCESSFUL,     // * Normal response if a request for Abort is unsuccessful.
    RAS_CP_RSP_CODE_PROC_NOT_CMPL,          // * Normal response if unable to complete a prcedure for any reason.
    RAS_CP_RSP_CODE_SERVER_BUSY,            // * Normal response if the Server is still busy with other requests.
    RAS_CP_RSP_CODE_NO_RECORDS_FOUND,       // * Normal response if the requested Ranging Counter is not found.
    // * 0x09~0xFF is reserved for future use.
} ras_cp_rsp_code_t;

typedef uint32_t ras_features_t;

#define RAS_RANGING_COUNTER_MOD    (0x1000)
typedef struct
{
    uint16_t ranging_counter    : 12;
    uint16_t config_id          : 4;
    uint8_t  selected_tx_power;
    uint8_t  antenna_paths_mask;
} __attribute__((packed)) ras_ranging_header_t;

typedef struct
{
    uint16_t    start_acl_conn_evt;
    uint16_t    freq_compensation;
    uint16_t    ranging_done_status   : 4;
    uint16_t    subevent_done_status  : 4;
    uint16_t    ranging_abort_reason  : 4;
    uint16_t    subevent_abort_reason : 4;
    uint8_t     refer_power_level;
    uint8_t     num_steps_reported;
} __attribute__((packed)) ras_subevent_header_t;

typedef struct
{
    uint8_t     step_mode;
    /// @brief variable, mode- and role-specific information being reported
    uint8_t     step_data[];
} __attribute__((packed)) ras_step_params_t;

typedef struct
{
    ras_subevent_header_t sub_header;
    ras_step_params_t     step_params[];
} __attribute__((packed)) ras_subevent_result_t;

typedef struct
{
    uint8_t first_seg : 1;
    uint8_t last_seg  : 1;
    uint8_t rolling_seg_counter : 6;
} __attribute__((packed)) ras_seg_header_t;

typedef struct
{
    ras_seg_header_t seg_header;
    uint8_t          seg_data[];
} __attribute__((packed)) ras_rad_seg_t;

/// @brief Get_Ranging_Data & ACK_Ranging_Data & Complete_Ranging_Data_Response Parameters
typedef struct
{
    uint16_t ranging_counter;
} ras_cp_ranging_data_params_t;

typedef ras_cp_ranging_data_params_t ras_cp_get_rad_params_t;
typedef ras_cp_ranging_data_params_t ras_cp_ack_rad_params_t;
typedef ras_cp_ranging_data_params_t ras_cp_rad_cmpl_params_t;

/// @brief Retrieve_Lost_Ranging_Data_Segments & Complete_Lost_Ranging_Data_Segment_Response Parameters
typedef struct
{
    uint16_t ranging_counter;
    uint8_t  start_segment;
    uint8_t  end_segment;
} __attribute__((packed)) ras_cp_rlrds_params_t;

typedef struct { } ras_cp_abort_op_params_t;

/// @brief Bits 0-1: Mode;  Bits 2-15: Filter bit mask
typedef struct
{
    union
    {
        uint16_t cfg;
        struct
        {
            uint16_t mode : 2;
            union
            {
                uint16_t param : 14;
                struct mode_0_mask
                {
                    uint16_t pkt_qty     : 1;
                    uint16_t pkt_rssi    : 1;
                    uint16_t pkt_antenna : 1;
                    uint16_t measured_freq_offset : 1;
                    uint16_t rfu         : 10;
                } m0;
                struct mode_1_mask
                {
                    uint16_t pkt_qty     : 1;
                    uint16_t pkt_nadm    : 1;
                    uint16_t pkt_rssi    : 1;
                    uint16_t tod_toa     : 1;
                    uint16_t pkt_antenna : 1;
                    uint16_t pkt_pct1    : 1;
                    uint16_t pkt_pct2    : 1;
                    uint16_t rfu         : 7;
                } m1;
                struct mode_2_mask
                {
                    uint16_t antenna_permutation_idx : 1;
                    uint16_t tone_pct       : 1;
                    uint16_t tone_qty_indicator      : 1;
                    uint16_t antenna_path_1 : 1;
                    uint16_t antenna_path_2 : 1;
                    uint16_t antenna_path_3 : 1;
                    uint16_t antenna_path_4 : 1;
                    uint16_t rfu            : 7;
                } m2;
                struct mode_3_mask
                {
                    uint16_t pkt_qty        : 1;
                    uint16_t pkt_nadm       : 1;
                    uint16_t pkt_rssi       : 1;
                    uint16_t tod_toa        : 1;
                    uint16_t pkt_antenna    : 1;
                    uint16_t pkt_pct1       : 1;
                    uint16_t pkt_pct2       : 1;
                    uint16_t antennad_permutation_idx : 1;
                    uint16_t tone_pct       : 1;
                    uint16_t tone_qty_indicator       : 1;
                    uint16_t antenna_path_1 : 1;
                    uint16_t antenna_path_2 : 1;
                    uint16_t antenna_path_3 : 1;
                    uint16_t antenna_path_4 : 1;
                } m3;
            } mask;
        } sub;
    } ctx;
} __attribute__((packed)) ras_cp_set_filter_params_t;


typedef struct
{
    /// @brief All other values are reserved for future use.
    ras_cp_rsp_code_t rsp_code;
} ras_cp_rsp_code_params_t;

typedef ras_cp_rlrds_params_t ras_cp_rtrv_lost_segs_params_t;
typedef ras_cp_rlrds_params_t ras_cp_lost_rad_cmpl_params_t;

typedef union
{
    void *param_ptr;
    ras_cp_get_rad_params_t             *get_ranging_data_params;
    ras_cp_ack_rad_params_t             *ack_ranging_data_params;
    ras_cp_rtrv_lost_segs_params_t      *retrieve_lost_segs_params;
    ras_cp_abort_op_params_t            *abort_op_params;
    ras_cp_set_filter_params_t          *set_filter_params;
} ras_cp_cmd_param_t;

typedef union
{
    void *param_ptr;
    ras_cp_rad_cmpl_params_t      *cmpl_ranging_data_rsp_params;
    ras_cp_lost_rad_cmpl_params_t *cmpl_lost_rad_seg_rsp_params;
    ras_cp_rsp_code_params_t      *rsp_code_params;
} ras_cp_rsp_param_t;

typedef struct
{
    uint8_t op_code;
    /// @brief variable, actual type is `ras_cp_cmd_param_t` or `ras_cp_rsp_param_t`
    uint8_t params[];
} __attribute__((packed)) ras_control_point_t;

typedef struct
{
    uint16_t ranging_counter;
} __attribute__((packed)) ras_ranging_data_ready_t;

typedef struct
{
    uint16_t ranging_counter;
} __attribute__((packed)) ras_ranging_data_overwritten_t;

typedef struct
{
    uint16_t connhdl;
    uint8_t  err_code;
} ras_common_char_ind_cfm_param_t;
typedef ras_common_char_ind_cfm_param_t ras_real_time_ind_cfm_param_t;
typedef ras_common_char_ind_cfm_param_t ras_on_demand_ind_cfm_param_t;
typedef ras_common_char_ind_cfm_param_t ras_rad_ready_ind_cfm_param_t;
typedef ras_common_char_ind_cfm_param_t ras_rad_overwritten_ind_cfm_param_t;

typedef struct
{
    uint16_t connhdl;
    uint8_t  err_code;
    ras_cp_rsp_code_t cp_rsp_code;
} ras_cp_rsp_ind_cfm_param_t;

typedef union
{
    void *param_ptr;
    ras_real_time_ind_cfm_param_t       *real_time;
    ras_on_demand_ind_cfm_param_t       *on_demand;
    ras_cp_rsp_ind_cfm_param_t          *cp_rsp;
    ras_rad_ready_ind_cfm_param_t       *rad_ready;
    ras_rad_overwritten_ind_cfm_param_t *rad_overwritten;
} ras_char_ind_cfm_param_t;

#endif  /* __RAS_COMMON_H__ */
