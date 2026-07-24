/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#include <stdio.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define OS_ENTER_CRITICAL()     uint32_t os_lock = bt_callback_int_lock()
#define OS_EXIT_CRITICAL()      bt_callback_int_unlock(os_lock)
#define OSTimeDly(a)            osDelay((a)*2)

int bt_int_lock();
void bt_int_unlock(int v);

void osapi_init(void);
void osapi_lock_stack(void);
void osapi_unlock_stack(void);
int osapi_lock_is_exist(void);
bool osapi_is_locked_by_self(void);
void osapi_notify_evm(void);

unsigned int Plt_GetTicks(void);
unsigned int Plt_GetTicksMax(void);

#if defined(__cplusplus)
}
#endif