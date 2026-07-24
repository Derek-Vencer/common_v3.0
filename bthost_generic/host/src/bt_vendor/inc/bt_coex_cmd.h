
/***************************************************************************
 *
 * Copyright (c) 2015-2026 BES Technic
 *
 * Authored by BES CD team (Blueelf Prj).
 *
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means: or stored in a database or retrieval system: without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright: trade secret:
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#ifndef __BT_COEX_CMD_H__
#define __BT_COEX_CMD_H__

#define INVALID_COEX_CONHDL (0xFF)
#define INVALID_COEX_JIT_BUFFER (0xFF)
#define DEFAULT_COEX_JIT_BUFFER (0x90)

bool bt_coex_mgr_set_music_info(uint16_t con_hdl, uint8_t jitter_buffer);

#endif
