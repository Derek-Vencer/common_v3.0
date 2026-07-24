/***************************************************************************
 *
 * Copyright 2015-2023 BES.
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
#ifndef __BLE_DULT_H__
#define __BLE_DULT_H__

#ifdef SPOT_ENABLED

#ifdef __cplusplus
extern "C" {
#endif

struct ble_dult_cb_t
{
    bool (*get_spot_get_mode)(void);
    void (*start_find_ringtone)(void);
    void (*stop_find_ringtone)(void);
    uint8_t *(*get_eid)(void);
    uint32_t (*sha256_hash)(const void *in_data, int len, void *out_data);
    void (*beacon_encrpt_data)(uint8_t *accKey, uint8_t *inputData, uint32_t inputDataLen, uint8_t *output);
};

void ble_app_dult_init(struct ble_dult_cb_t *cbs);

void ble_app_dult_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* SPOT_ENABLED */

#endif /* __BLE_DULT_H__ */
