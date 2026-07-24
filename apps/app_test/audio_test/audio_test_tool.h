 /***************************************************************************
 *
 * Copyright 2015-2025 BES.
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
#ifndef __AUDIO_TEST_TOOL_H__
#define __AUDIO_TEST_TOOL_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Usage:
 *  1. CMD Format: e.g. audio_test:anc_switch
 **/

typedef void (*func_handler_t)(const char *cmd);

typedef struct {
    const char *name;
    func_handler_t handler;
} audio_test_func_t;

bool audio_test_tool_check_rsp_flag(void);
void audio_test_tool_set_rsp_flag(bool val);
void audio_test_tool_get_func(const audio_test_func_t **func, uint32_t *size);

#ifdef __cplusplus
}
#endif

#endif
