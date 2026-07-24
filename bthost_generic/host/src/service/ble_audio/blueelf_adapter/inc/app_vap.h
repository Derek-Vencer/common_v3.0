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
#ifndef __APP_VAP_COMMON_H__
#define __APP_VAP_COMMON_H__

#if BLE_AUDIO_ENABLED

/*
 * DEFINES
*/

/*
 * ENUMERATIONS
 */
enum app_vas_char_type
{
    APP_VAS_CHAR_TYPE_MIN = 0,
    /// VA Service Name characteristic
    APP_VAS_CHAR_TYPE_VA_NAME = APP_VAS_CHAR_TYPE_MIN,
    /// UUID characteristic
    APP_VAS_CHAR_TYPE_VA_UUID,
    /// VA Control Point characteristic
    APP_VAS_CHAR_TYPE_VA_CP,
    /// Installed Location characteristic
    APP_VAS_CHAR_TYPE_INSTALLED_LOC,
    /// CCID(Content Control ID) characteristic
    APP_VAS_CHAR_TYPE_VA_CCID,
    /// VA Session State characteristic
    APP_VAS_CHAR_TYPE_VA_SESSION_STATE,
    /// VA Supported Languages characteristic
    APP_VAS_CHAR_TYPE_VA_SUPP_LANG,
    /// VA Supported Features characteristic
    APP_VAS_CHAR_TYPE_VA_SUPP_FEAT,
    /// VA Session Flag characteristic
    APP_VAS_CHAR_TYPE_VA_SESSION_FLAG,

    APP_VAS_CHAR_TYPE_MAX,
};

/// VA Control: Operation code values
enum app_vap_va_cp_opcode
{
    APP_VAP_VA_CP_OPCODE_MIN = 0,
    /// Client requests to initialize the VA to its initial state.
    APP_VAP_VA_CP_OPCODE_INIT = APP_VAP_VA_CP_OPCODE_MIN,
    /// Client requests to start a VA session.
    APP_VAP_VA_CP_OPCODE_START,
    /// Client requests to stop a VA session.
    APP_VAP_VA_CP_OPCODE_STOP,

    APP_VAP_VA_CP_OPCODE_MAX,
};

/// VA Control: Response
enum app_vap_va_cp_response
{
    APP_VAP_VA_CP_RESP_OPCODE_MIN = 0,
    /// Response opcode - rsp
    APP_VAP_VA_CP_RESP_OPCODE_RSP = APP_VAP_VA_CP_RESP_OPCODE_MIN,

    APP_VAP_VA_CP_RESP_CODE_MAX,
};

/// VA Control: Response code values
enum app_vap_va_cp_resp_op_rsp_value
{
    APP_VAP_VA_CP_RESP_OP_RSP_VAL_MIN = 0,
    /// RFU
    APP_VAP_VA_CP_RESP_OP_RSP_VAL_RFU = APP_VAP_VA_CP_RESP_OP_RSP_VAL_MIN,
    /// Response for a successful operation
    APP_VAP_VA_CP_RESP_OP_RSP_VAL_SUCCESS,
    /// Response if an unsupported Op Code
    APP_VAP_VA_CP_RESP_OP_RSP_VAL_OP_NO_SUPP,
    /// Response for an unsuccessful operation
    APP_VAP_VA_CP_RESP_OP_RSP_VAL_OP_FAILED,
    /// Response if an Opcode was written while the VA
    /// is in a Session State that cannot accept it
    /// (e.g., Start VA Session Opcode written in the Session Unavailable state)
    APP_VAP_VA_CP_RESP_OP_RSP_VAL_INVALID_SESSION_STATE,

    APP_VAP_VA_CP_RESP_OP_RSP_VAL_MAX,
};

/// VA Session State values
enum app_vap_session_state
{
    APP_VAP_SESSION_STATE_MIN = 0,
    /// Unavailable
    APP_VAP_SESSION_STATE_UNAVA = APP_VAP_SESSION_STATE_MIN,
    /// Session Reset
    APP_VAP_SESSION_STATE_RESET,
    /// Session Ready
    APP_VAP_SESSION_STATE_READY,
    /// Session Active
    APP_VAP_SESSION_STATE_ACTIVE,

    APP_VAP_SESSION_STATE_MAX,
};

/// VA Session Flag values
enum app_vap_session_flag
{
    /// Listening Now: 0 = The VA is not attentive, 1 = The VA is attentive
    APP_VAP_SESSION_FLAG_LISTENING_NOW_BIT = 0b001,
    /// Processing Now: 0 = The VA is not processing any request, 1 = The VA is processing request
    APP_VAP_SESSION_FLAG_PROCESSING_NOW_BIT = 0b010,
    /// Playback Now: 0 = The VA is silent, 1 = The VA is responding
    APP_VAP_SESSION_FLAG_PLAYBACK_NOW_BIT = 0b100,

    APP_VAP_SESSION_FLAG_MASK = 0b111,

    APP_VAP_SESSION_FLAG_RFU_MASK = 0b11111000,
};

/*
 * FUNCTIONS
*/

#endif /* BLE_AUDIO_ENABLED */

#endif /* __APP_VAP_COMMON__ */