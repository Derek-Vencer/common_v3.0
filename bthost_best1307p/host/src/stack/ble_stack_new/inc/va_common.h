/***************************************************************************
 *
 * Copyright (c) 2015-2025 BES Technic
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
#ifndef __VAP_VA_COMMON_H__
#define __VAP_VA_COMMON_H__

#include "gaf_cfg.h"

/*
 * ENUMERATIONS
 */
enum vas_char_type
{
    VAS_CHAR_TYPE_MIN = 0,
    /// VA Service Name characteristic
    VAS_CHAR_TYPE_VA_NAME = VAS_CHAR_TYPE_MIN,
    /// UUID characteristic
    VAS_CHAR_TYPE_VA_UUID,
    /// VA Control Point characteristic
    VAS_CHAR_TYPE_VA_CP,
    /// Installed Location characteristic
    VAS_CHAR_TYPE_INSTALLED_LOC,
    /// CCID (Content Control ID) characteristic
    VAS_CHAR_TYPE_VA_CCID,
    /// VA Session State characteristic
    VAS_CHAR_TYPE_VA_SESSION_STATE,
    /// VA Supported Languages characteristic
    VAS_CHAR_TYPE_VA_SUPP_LANG,
    /// VA Supported Features characteristic
    VAS_CHAR_TYPE_VA_SUPP_FEAT,
    /// VA Session Flag characteristic
    VAS_CHAR_TYPE_VA_SESSION_FLAG,

    VAS_CHAR_TYPE_MAX,
};

/// VA Control: Operation code values
enum vap_va_cp_opcode
{
    VAP_VA_CP_OPCODE_MIN = 0,
    /// Client requests to initialize the VA to its initial state.
    VAP_VA_CP_OPCODE_INIT = VAP_VA_CP_OPCODE_MIN,
    /// Client requests to start a VA session.
    VAP_VA_CP_OPCODE_START,
    /// Client requests to stop a VA session.
    VAP_VA_CP_OPCODE_STOP,

    VAP_VA_CP_OPCODE_MAX,
};

/// VA Control: Response
enum vap_va_cp_response
{
    VAP_VA_CP_RESP_OPCODE_MIN = 0,
    /// Response opcode - rsp
    VAP_VA_CP_RESP_OPCODE_RSP = VAP_VA_CP_RESP_OPCODE_MIN,

    VAP_VA_CP_RESP_CODE_MAX,
};

/// VA Control: Response code values
enum vap_va_cp_resp_op_rsp_value
{
    VAP_VA_CP_RESP_OP_RSP_VAL_MIN = 0,
    /// RFU
    VAP_VA_CP_RESP_OP_RSP_VAL_RFU = VAP_VA_CP_RESP_OP_RSP_VAL_MIN,
    /// Response for a successful operation
    VAP_VA_CP_RESP_OP_RSP_VAL_SUCCESS,
    /// Response if an unsupported Op Code
    VAP_VA_CP_RESP_OP_RSP_VAL_OP_NO_SUPP,
    /// Response for an unsuccessful operation
    VAP_VA_CP_RESP_OP_RSP_VAL_OP_FAILED,
    /// Response if an Opcode was written while the VA
    /// is in a Session State that cannot accept it
    /// (e.g., Start VA Session Opcode written in the Session Unavailable state)
    VAP_VA_CP_RESP_OP_RSP_VAL_INVALID_SESSION_STATE,

    VAP_VA_CP_RESP_OP_RSP_VAL_MAX,
};

/// VA Session State values
enum vap_session_state
{
    VAP_SESSION_STATE_MIN = 0,
    /// Unavailable
    VAP_SESSION_STATE_RESET = VAP_SESSION_STATE_MIN,
    /// Session Reset
    VAP_SESSION_STATE_UNAVA,
    /// Session Ready
    VAP_SESSION_STATE_READY,
    /// Session Active
    VAP_SESSION_STATE_ACTIVE,

    VAP_SESSION_STATE_MAX,
};

/// VA Session Flag values
enum vap_session_flag
{
    /// Listening Now: 0 = The VA is not attentive, 1 = The VA is attentive
    VAP_SESSION_FLAG_LISTENING_NOW_BIT = 0b001,
    /// Processing Now: 0 = The VA is not processing any request, 1 = The VA is processing request
    VAP_SESSION_FLAG_PROCESSING_NOW_BIT = 0b010,
    /// Playback Now: 0 = The VA is silent, 1 = The VA is responding
    VAP_SESSION_FLAG_PLAYBACK_NOW_BIT = 0b100,

    VAP_SESSION_FLAG_MASK = 0b111,

    VAP_SESSION_FLAG_RFU_MASK = 0b11111000,
};

enum vap_supported_features
{
    /// Session Flags Enabled
    VAP_SUPP_FEAT_SESSION_FLAG_EN_BIT = 0b01,
    /// RFU
    VAP_SUPP_FEAT_RFU_MASK = 0b11111110,
};

/*
 * STRUCTURES
*/

/// VA Control Point Write
typedef struct va_cp_packed
{
    /// Request opcode
    uint8_t  op;
    uint8_t  pdu[GAF_ARRAY_EMPTY];
} __attribute__((packed)) va_cp_p_t;

/// VA Control Point Response
typedef struct va_cp_resp_packed
{
    /// Response opcode
    uint8_t  resp_op;
    /// At least one octect rsp val
    uint8_t  pdu[1];
} __attribute__((packed)) va_cp_resp_p_t;

#endif /* __VAP_VA_COMMON__ */