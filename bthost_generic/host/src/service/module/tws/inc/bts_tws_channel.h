/***************************************************************************
 *
 * Copyright 2015-2024 BES.
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
 * @brief xxx.
 *
 ****************************************************************************/
#ifndef __BTS_TWS_CHANNEL_H__
#define __BTS_TWS_CHANNEL_H__

/****************************** header include ********************************/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
/***************************** external declaration *****************************/

/***************************** macro defination *******************************/

/***************************** type defination ********************************/
typedef enum
{
    BTS_TWS_CHANNEL_EVENT_UNKONWN = 0,
    BTS_TWS_CHANNEL_EVENT_CONNECTED,
    BTS_TWS_CHANNEL_EVENT_DISCONNECTED,
    BTS_TWS_CHANNEL_EVENT_DATA_IND,
    BTS_TWS_CHANNEL_EVENT_DATA_SENT,
    BTS_TWS_CHANNEL_EVENT_SET_IDLE,
    BTS_TWS_CHANNEL_EVENT_MAX,
} BTS_TWS_CHANNEL_EVENT_E;

/***************************** function declaration ****************************/
bool bts_tws_channel_init(void);
bool bts_tws_channel_deinit(void);
bool bts_tws_channel_sending(void);
void bts_tws_channel_clear_sending(void);
void bts_tws_channel_client_create(uint16_t conn_handle);
void bts_tws_channel_server_create(void (*callback)(uint16_t event));
void bts_tws_channel_data_received_register(void (*callback)(uint8_t*, uint16_t));
void bts_tws_channel_send_cmd(uint8_t* cmd, uint16_t len);
bool bts_tws_channel_send_cmd_no_wait(uint8_t* cmd, uint16_t len);
bool bts_tws_channel_is_connected(void);

#ifdef __cplusplus
}
#endif

#endif

