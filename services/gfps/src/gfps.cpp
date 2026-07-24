/***************************************************************************
 *
 * Copyright 2020-2025 BES.
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
/**
 ****************************************************************************************
 * @addtogroup GFPS
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#ifdef GFPS_ENABLED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmsis.h"
#include "cmsis_os.h"

#include "hal_trace.h"

#include "bt_drv_reg_op.h"
#if defined(SPOT_ENABLED)
#include "bt_drv_interface.h"
#include "hal_timer.h"
#endif

#include "nvrecord_ble.h"
#include "nvrecord_fp_account_key.h"

#include "apps.h"
#include "app_media_player.h"

#ifndef BT_SVC_MODULE_IBRT_ENABLED
#include "bta_normal_ux_api.h"
#else
#include "app_ibrt_customif_cmd.h"
#include "bta_tws_ux_api.h"
#endif


#include "gfps.h"
#include "gfps_ble.h"
#include "gfps_rfcomm.h"
#include "gfps_crypto.h"

#include "bes_gfps_api.h"
#include "bes_gap_api.h"

#if defined(SASS_ENABLED)
#include "gfps_sass.h"
#include "gfps_crypto.h"
#endif

#include "customparam_section.h"

#include "bts_ble_api.h"
#include "bts_lea_api.h"

#include "bta_bt_api.h"
#include "bta_ble_api.h"
#include "ble_gfps.h"

#if defined(BT_SVC_MODULE_IBRT_ENABLED)
#include "bts_bt_if.h"
#include "bts_api.h"
#include "bts_tws_api.h"
#include "bta_tws_audio_api.h"
#include "bta_tws_ux_api.h"
#else
#include "bta_normal_ux_api.h"
#include "bta_normal_audio_api.h"
#endif
#if defined(BESUI_GFPS_ID_EN) || defined(BESUI_FIX_GFPS_MODELID_EN)
#include "besui_common.h"
#endif


/************************private macro defination***************************/
#define GFPS_MAIL_MAX (20)
#define GFPS_BATTERY_TIMEROUT_VALUE             (10000)

/************************private type defination****************************/
static GFPSEnv_t gfpsEnv = {0};
static __attribute__((unused)) FpRingStatus_t fp_ring_status = {false, false, 0};
static osMailQId gfps_mailbox = NULL;
static uint8_t gfps_mailbox_cnt = 0;

osTimerId ring_timeout_timer_id = NULL;
static void gfps_find_devices_ring_timeout_handler(void const *param);
osTimerDef (GFPS_FIND_DEVICES_RING_TIMEOUT, gfps_find_devices_ring_timeout_handler);

static void gfps_find_mybuds_timer_cb(void const *n);
osTimerDef (GFPS_FIND_MYBUDS_TIMER, gfps_find_mybuds_timer_cb);
static osTimerId gfps_find_mybuds_timer_id = NULL;

static void gfps_thread(void const *argument);
osThreadDef(gfps_thread, osPriorityNormal, 1, 1024*4, "gfps_thread");
osMailQDef (gfps_mailbox, GFPS_MAIL_MAX, GFPS_MESSAGE_BLOCK);

/****************************function defination****************************/
void gfps_link_connection_event_process(uint8_t devId, GFPS_CONNECTION_EVENT *pEvent);
void gfps_srv_event_rfcomm_process(uint8_t devId, GFPS_SRV_EVENT_RFCOM_PARAM_T *param);
void gfps_srv_event_l2cap_process(uint8_t devId, GFPS_SRV_EVENT_L2CAP_PARAM_T *param);
void gfps_link_connect_process(uint8_t devId, const bt_bdaddr_t *addr);
#if BLE_AUDIO_ENABLED
void gfps_lea_event_handler(const bt_lea_evt_packet_t *evt_pkt);
#endif

/**
 ****************************************************************************************
 *  GFPS BTA Adapter
 ****************************************************************************************
 */

#define gfps_cmd_rsp_timeout_handler_null   (0)
#define gfps_cmd_rsp_handler_null           (0)
#define gfps_cmd_tx_done_handler_null       (0)

#if defined(BT_SVC_MODULE_IBRT_ENABLED)

void app_ibrt_send_gfps_flag_info(uint8_t *p_buff, uint16_t length);
void app_ibrt_send_gfps_flag_info_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);

void app_ibrt_send_gfps_passkey(uint8_t *p_buff, uint16_t length);
void app_ibrt_send_gfps_passkey_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_send_gfps_passkey_rsp_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);

void app_ibrt_send_gfps_ring_info(uint8_t *p_buff, uint16_t length);
void app_ibrt_send_gfps_ring_info_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);

#if BLE_AUDIO_ENABLED
void app_ibrt_send_gfps_ble_disc_cmd(uint8_t *p_buff, uint16_t length);
void app_ibrt_send_gfps_ble_disc_cmd_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);

void app_ibrt_send_streaming_state_cmd(uint8_t *p_buff, uint16_t length);
void app_ibrt_send_streaming_state_cmd_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
#endif

#if defined(SASS_ENABLED)
void app_ibrt_send_sass_info(uint8_t *p_buff, uint16_t length);
void app_ibrt_send_sass_info_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_send_sass_info_rsp_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
#endif // SASS_ENABLED

static const bt_tws_cmd_instance_t g_gfps_cmd_handler_table[]= {
    {
        APP_TWS_CMD_SNED_GFPS_FLAG_INFO,                 "SEND_GFPS_FLAG_INFO",
        app_ibrt_send_gfps_flag_info,
        app_ibrt_send_gfps_flag_info_handler,             0,
        gfps_cmd_rsp_timeout_handler_null,                gfps_cmd_rsp_handler_null,
        gfps_cmd_tx_done_handler_null,
        0
    },
    {
        APP_TWS_CMD_SNED_GFPS_PASSKEY,                   "SEND_GFPS_PASSKEY",
        app_ibrt_send_gfps_passkey,
        app_ibrt_send_gfps_passkey_handler,              RSP_TIMEOUT_DEFAULT,
        gfps_cmd_rsp_timeout_handler_null,               app_ibrt_send_gfps_passkey_rsp_handler,
        gfps_cmd_tx_done_handler_null,
        0
    },

    {
        APP_TWS_CMD_SEND_GFPS_RING_INFO,                 "SEND_GFPS_RING_INFO",
        app_ibrt_send_gfps_ring_info,
        app_ibrt_send_gfps_ring_info_handler,             0,
        gfps_cmd_rsp_timeout_handler_null,                gfps_cmd_rsp_handler_null,
        gfps_cmd_tx_done_handler_null,
        0
    },
#if BLE_AUDIO_ENABLED
    {
        APP_TWS_CMD_SNED_GFPS_BLE_DISC_CMD,               "SEND_GFPS_BLE_DISC",
        app_ibrt_send_gfps_ble_disc_cmd,
        app_ibrt_send_gfps_ble_disc_cmd_handler,           0,
        gfps_cmd_rsp_timeout_handler_null,                 gfps_cmd_rsp_handler_null,
        gfps_cmd_tx_done_handler_null,
        0
    },

    {
        APP_TWS_CMD_SNED_STREAMING_STATE_CMD,             "SEND_STREAMING_STATE",
        app_ibrt_send_streaming_state_cmd,
        app_ibrt_send_streaming_state_cmd_handler,        0,
        gfps_cmd_rsp_timeout_handler_null,                gfps_cmd_rsp_handler_null,
        gfps_cmd_tx_done_handler_null,
        0
    },
#endif // BLE_AUDIO_ENABLED
    {
        APP_TWS_CMD_SHARE_FASTPAIR_INFO,                  "SHARE_FASTPAIR_INFO",
        app_ibrt_share_fastpair_info,
        app_ibrt_share_fastpair_info_received_handler,    0,
        gfps_cmd_rsp_timeout_handler_null,                gfps_cmd_rsp_handler_null,
        gfps_cmd_tx_done_handler_null,
        0
    },
#if defined(SASS_ENABLED)
    {
        APP_TWS_CMD_SEND_SASS_INFO,                       "SEND_SASS_INFO",
        app_ibrt_send_sass_info,
        app_ibrt_send_sass_info_handler,                  RSP_TIMEOUT_DEFAULT,
        gfps_cmd_rsp_timeout_handler_null,                app_ibrt_send_sass_info_rsp_handler,
        gfps_cmd_tx_done_handler_null,
        0
    },
#endif // SASS_ENABLED
};
#endif

#if defined(BT_SVC_MODULE_IBRT_ENABLED)
void app_ibrt_send_gfps_flag_info(uint8_t *p_buff, uint16_t length)
{
    bta_tws_send_cmd_without_rsp(APP_TWS_CMD_SNED_GFPS_FLAG_INFO , p_buff, length);
}

void app_ibrt_send_gfps_flag_info_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    bool flag = *p_buff;
    gfps_set_flag(flag);
    bes_ble_gap_core_register_global_handler(gfps_process_ble_user_confirmation);
}

void app_ibrt_send_gfps_passkey(uint8_t *p_buff, uint16_t length)
{
    bta_tws_send_cmd_with_rsp(APP_TWS_CMD_SNED_GFPS_PASSKEY, p_buff, length);
}

void app_ibrt_send_gfps_passkey_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    //compare this passkey
    if(memcmp(p_buff, gfps_get_additional_passkey(), length))
    {
        memset(p_buff, 0x00, length);
    }
    bta_tws_send_rsp((uint16_t)APP_TWS_CMD_SNED_GFPS_PASSKEY, rsp_seq, p_buff,length);
}

void app_ibrt_send_gfps_passkey_rsp_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    GFPS_TRACE(1, "%s", __func__);
    uint8_t ADDR_ZERO[3]={0x00, 0x00, 0x00};
    gfpsp_ble_additional_pass_key_resp raw_rsp;
    gfps_ble_encrypted_resp en_rsp;
    raw_rsp.message_type = GFPS_PROVIDER_PASSKEY_KEY;
    memcpy(raw_rsp.passkey, p_buff, length);
    gfps_set_additional_pass_key(p_buff, length);

    if (bta_tws_get_sync_state() == BTA_TWS_SYNCED)
    {
        big_little_switch(nv_record_get_ibrt_peer_addr(), raw_rsp.bonding_addr, sizeof(bt_bdaddr_t));
    }
    if(memcmp(p_buff, ADDR_ZERO, length))
    {
        raw_rsp.status = GFPS_STATUS_SUCCESS;
    }
    else
    {
        raw_rsp.status = GFPS_STATUS_FAIL;
    }

    gfps_crypto_encrypt(( const uint8_t * )(&raw_rsp.message_type), sizeof(raw_rsp),gfps_get_keybase_pair_key(), en_rsp.uint128_array);
    ble_app_gfps_send_additional_passkey(gfps_ble_get_conidx(), ( uint8_t * )en_rsp.uint128_array, sizeof(en_rsp));

    gfps_set_if_send_passkey(true);
}

void app_ibrt_send_gfps_ring_info(uint8_t *p_buff, uint16_t length)
{
    bta_tws_send_cmd_without_rsp(APP_TWS_CMD_SEND_GFPS_RING_INFO, p_buff, length);
}

void app_ibrt_send_gfps_ring_info_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    GFPS_TRACE(1, "%s", __func__);
    gfps_data_handler(0, p_buff, length);
}

#if BLE_AUDIO_ENABLED
void app_ibrt_send_gfps_ble_disc_cmd(uint8_t *p_buff, uint16_t length)
{
    GFPS_TRACE(1, "%s", __func__);
    bta_tws_send_cmd_without_rsp(APP_TWS_CMD_SNED_GFPS_BLE_DISC_CMD, p_buff, length);
}

void app_ibrt_send_gfps_ble_disc_cmd_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    GFPS_TRACE(1, "%s", __func__);
    bta_ble_disconnect((ble_bdaddr_t *)p_buff);
}

void app_ibrt_send_streaming_state_cmd(uint8_t *p_buff, uint16_t length)
{
    GFPS_TRACE(1, "%s", __func__);
    bta_tws_send_cmd_without_rsp(APP_TWS_CMD_SNED_STREAMING_STATE_CMD, p_buff, length);
}

void app_ibrt_send_streaming_state_cmd_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    GFPS_TRACE(1, "%s", __func__);
    gfps_rec_peer_streaming_state_handler(p_buff, length);
}
#endif // BLE_AUDIO_ENABLED

#if defined(SASS_ENABLED)
void app_ibrt_send_sass_info(uint8_t *p_buff, uint16_t length)
{
    bta_tws_send_cmd_with_rsp(APP_TWS_CMD_SEND_SASS_INFO, p_buff, length);
}

void app_ibrt_send_sass_info_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    gfps_sass_set_sync_info(p_buff, length);
    bta_tws_send_rsp((uint16_t)APP_TWS_CMD_SEND_SASS_INFO, rsp_seq, NULL, 0);
}

void app_ibrt_send_sass_info_rsp_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    GFPS_TRACE(1, "%s", __func__);
}
#endif // SASS_ENABLED

#endif // BT_SVC_MODULE_IBRT_ENABLED

uint8_t gfps_bta_get_device_num_max(void)
{
#if defined(BT_SVC_MODULE_IBRT_ENABLED)
    return bta_tws_get_device_num_max();
#else
    return bta_get_device_num_max();
#endif
}

uint8_t gfps_bta_get_bt_connected_dev_list(bt_bdaddr_list_t *p_dev_addr_l)
{
#if defined(BT_SVC_MODULE_IBRT_ENABLED)
    return bta_tws_find_all_connected_bt_device(p_dev_addr_l);
#else
    return bta_find_all_connected_bt_device(p_dev_addr_l);
#endif
}

uint8_t gfps_bta_get_bt_connected_link_num(void)
{
    return gfps_bta_get_bt_connected_dev_list(NULL);
}

uint8_t gfps_bta_get_lea_connected_link_num(void)
{
#if BLE_AUDIO_ENABLED
    return bta_lea_get_connected_dev_num();
#else
    return 0;
#endif
}

void gfps_bta_enable_pairing_mode(bool enable)
{
#if defined(BT_SVC_MODULE_IBRT_ENABLED)
    bta_tws_enable_pairing_mode(enable);
#else
    bta_enable_pairing_mode(enable);
#endif
}

void gfps_bta_connect_bt_device(const bt_bdaddr_t *addr, uint8_t page_count)
{
#if defined(BT_SVC_MODULE_IBRT_ENABLED)
    bta_tws_connect_bt_device(addr, page_count);
#else
    bta_connect_bt_device(addr, page_count, 0);
#endif
}

void gfps_bta_remove_bt_device(const bt_bdaddr_t *addr)
{
#if defined(BT_SVC_MODULE_IBRT_ENABLED)
    bta_tws_remove_bt_device(addr);
#else
    bta_remove_bt_device(addr);
#endif
}

#if defined(BT_SVC_MODULE_IBRT_ENABLED)
bool gfps_event_io_capbility_request_callback(const bt_bdaddr_t *addr, uint8_t local_initiate);
const bts_bt_to_gfps_cbs_t cbs = {
    .io_capbility_request = gfps_event_io_capbility_request_callback,
};
#endif

int gfps_bta_register_iocap_req_cb(void)
{
#if defined(BT_SVC_MODULE_IBRT_ENABLED)
    bts_bt_if_register_gfps_cbs(&cbs);
    return 0;
#else
    // TODO headset callback missing
    return -1;
#endif
}


bool gfps_bta_is_tws_addr(const bt_bdaddr_t *addr)
{
#if defined(BT_SVC_MODULE_TWS_ENABLED)
    return bta_tws_is_tws_addr(addr);
#else
    return false;
#endif
}

bool gfps_bta_is_tws_connected(void)
{
#if defined(BT_SVC_MODULE_TWS_ENABLED)
    return bta_tws_get_sync_state() != BTA_TWS_DISCONNECTED;
#else
    return false;
#endif
}

void gfps_bta_set_device_num_max(uint8_t device_num_max, const bt_bdaddr_t reserved_hint[], uint8_t reserved_hint_count)
{
#if defined(BT_SVC_MODULE_IBRT_ENABLED)
    bta_tws_set_device_num_max(device_num_max, reserved_hint, reserved_hint_count);
#else
    bta_set_device_num_max(device_num_max, reserved_hint, reserved_hint_count);
#endif
}

void gfps_bta_switch_streaming_a2dp(void)
{
#if defined(BT_SVC_MODULE_IBRT_ENABLED)
    bta_tws_switch_streaming_a2dp();
#else
    bta_switch_streaming_a2dp();
#endif
}

void gfps_bta_toggle_a2dp_cis(void)
{
#if defined(BT_SVC_MODULE_IBRT_ENABLED)
    bta_tws_toggle_a2dp_cis();
#else
    bta_toggle_a2dp_cis();
#endif
}

/************************extern function declearation***********************/
#ifdef FIRMWARE_REV
extern "C" void system_get_info(uint8_t *fw_rev_0, uint8_t *fw_rev_1, uint8_t *fw_rev_2, uint8_t *fw_rev_3);
#endif
//MSB->LSB
extern uint8_t ble_public_key[64];
extern uint8_t ble_private_key[32];

static int gfps_mailbox_init(void)
{
    if (!gfps_mailbox)
    {
        gfps_mailbox = osMailCreate(osMailQ(gfps_mailbox), NULL);
    }
    gfps_mailbox_cnt = 0;
    return 0;
}

int gfps_mailbox_free(GFPS_MESSAGE_BLOCK* msg_p)
{
    osStatus status;

    status = osMailFree(gfps_mailbox, msg_p);
    if (osOK != status)
    {
        GFPS_TRACE(2,"%s error status 0x%02x", __func__, status);
        return (int)status;
    }

    if (gfps_mailbox_cnt)
    {
        gfps_mailbox_cnt--;
    }

    return (int)status;
}

int gfps_mailbox_free_all(void)
{
    GFPS_MESSAGE_BLOCK *msg_p = NULL;
    int status = osOK;
    osEvent evt;

    for (uint8_t i=0; i<gfps_mailbox_cnt; i++)
    {
        evt = osMailGet(gfps_mailbox, 500);
        if (evt.status == osEventMail)
        {
            msg_p = (GFPS_MESSAGE_BLOCK *)evt.value.p;
            status = gfps_mailbox_free(msg_p);
        }
        else
        {
            GFPS_TRACE(1, "%s get mailbox timeout!!!", __func__);
            continue;
        }
   }
   return status;
}

int gfps_mailbox_put(uint8_t devId, uint8_t event, uint8_t *param, uint16_t len)
{
    osStatus status = osOK;

    GFPS_MESSAGE_BLOCK *msg_p = NULL;

    if(gfps_mailbox_cnt >= GFPS_MAIL_MAX -1)
    {
        GFPS_TRACE(2,"%s warn gfps_mailbox_cnt is %d", __func__, gfps_mailbox_cnt);
        return osErrorValue;
    }

    msg_p = (GFPS_MESSAGE_BLOCK*)osMailAlloc(gfps_mailbox, 0);
    if (msg_p == NULL)
    {
        gfps_mailbox_free_all();
        msg_p = (GFPS_MESSAGE_BLOCK*)osMailAlloc(gfps_mailbox, 0);
        ASSERT(msg_p, "osMailAlloc error");
    }

    msg_p->devId = devId;
    msg_p->event = event;
    msg_p->len = len;
    if (param && len)
    {
        memcpy((uint8_t *)&(msg_p->p), param, len);
    }

    status = osMailPut(gfps_mailbox, msg_p);
    if (osOK != status)
    {
        GFPS_TRACE(2,"%s error status 0x%02x", __func__, status);
        return (int)status;
    }

    gfps_mailbox_cnt++;

    return (int)status;
}

int gfps_mailbox_get(GFPS_MESSAGE_BLOCK** msg_p)
{
    osEvent evt;

    evt = osMailGet(gfps_mailbox, osWaitForever);
    if (evt.status == osEventMail)
    {
        *msg_p = (GFPS_MESSAGE_BLOCK *)evt.value.p;
        return 0;
    }
    else
    {
        GFPS_TRACE(2,"%s evt.status 0x%02x", __func__, evt.status);
        return -1;
    }
}

static void gfps_thread(void const *argument)
{
    GFPS_MESSAGE_BLOCK* msg_p = NULL;
    POSSIBLY_UNUSED uint16_t len = 0;
    uint8_t devId = 0;
    uint8_t event;
#if defined(SASS_ENABLED)
    GFPS_SASS_PROFILE_EVENT *pEvent;
    uint8_t *p = NULL;
#endif

    while(1)
    {
        if(!gfps_mailbox_get(&msg_p))
        {
            len = msg_p->len;
            devId = msg_p->devId;
            event = msg_p->event;
            GFPS_TRACE(3, "gfps_thread event:%d device_id:%d %d", event, devId, len);
            switch(event)
            {
                case GFPS_EVENT_CONNECTION:
                    gfps_link_connection_event_process(devId, (GFPS_CONNECTION_EVENT *)&(msg_p->p));
                    break;
                case GFPS_EVENT_PROFILE:
                {
#if defined(SASS_ENABLED)
                    uint8_t emptyAddr[6] = {0};
                    memset(emptyAddr, 0, 6);

                    pEvent = (GFPS_SASS_PROFILE_EVENT *)&(msg_p->p);
                    if (pEvent->len > 0) {
                        p = pEvent->param;
                    }

                    gfps_sass_profile_event_handler(pEvent->pro, devId, &(pEvent->btAddr), pEvent->btEvt, p);
#endif
                }
                    break;
                case GFPS_EVENT_FROM_RFCOMM_SEEKER:
                    gfps_srv_event_rfcomm_process(devId, (GFPS_SRV_EVENT_RFCOM_PARAM_T *)&(msg_p->p));
                    break;
                case GFPS_EVENT_FROM_L2CAP_SEEKER:
                     gfps_srv_event_l2cap_process(devId, (GFPS_SRV_EVENT_L2CAP_PARAM_T *)&(msg_p->p));
                    break;
                default:
                    GFPS_TRACE(1,"%s error msg", __func__);
                    break;
            }
            gfps_mailbox_free(msg_p);
        }
    }
}

void gfps_thread_init(void)
{
    if (gfps_mailbox_init())
    {
        GFPS_TRACE(0, "ERROR: TOTA mailbox init failed");
        return;
    }
    osThreadId gfps_thread_tid = osThreadCreate(osThread(gfps_thread), NULL);
    if (gfps_thread_tid == NULL)
    {
        GFPS_TRACE(0, "ERROR: AI thread init failed");
        return;
    }
}

uint8_t gfps_send(uint8_t devId, uint8_t *ptrData, uint32_t length)
{
    uint8_t ret = 0;

#if defined(BT_SVC_IBRT_MODULE_ENABLED)
    if (BT_IBRT_SLAVE == bta_tws_get_ui_role() && bts_tws_if_get_init_done_state())
    {
        return 1;
    }
#endif

    if (IS_BT_DEVICE(devId))
    {
        ret = app_fp_rfcomm_send(GET_BT_ID(devId), ptrData, length);
    }
#if BLE_AUDIO_ENABLED
    else
    {
        if (bes_ble_gfps_l2cap_send(devId, ptrData, length) == BT_STS_SUCCESS)
        {
            ret = 1;
        }
    }
#endif
    GFPS_TRACE(3, "%s send data ret:%d id:%d", __func__, ret, devId);

    return ret;
}

static __attribute__((unused)) void gfps_send_active_components_rsp(uint8_t devId)
{
    FP_MESSAGE_STREAM_T req = {FP_MSG_GROUP_DEVICE_INFO, FP_MSG_DEVICE_INFO_ACTIVE_COMPONENTS_RSP, 0, 1};

#if defined(BT_SVC_MODULE_IBRT_ENABLED)
    if (gfps_bta_is_tws_connected())
    {
        req.data[0] = FP_MSG_BOTH_BUDS_ACTIVE;
    }
    else
    {
        if (bta_tws_get_location() == BT_LOCATION_LEFT)
        {
            req.data[0] = FP_MSG_LEFT_BUD_ACTIVE;
        }
        else
        {
            req.data[0] = FP_MSG_RIGHT_BUD_ACTIVE;
        }
    }
#else
    req.data[0] = FP_MSG_RIGHT_BUD_ACTIVE;
#endif

    gfps_send(devId, (uint8_t *)&req, FP_MESSAGE_RESERVED_LEN+1);
}

void gfps_send_msg_ack(uint8_t devId, uint8_t msgGroup, uint8_t msgCode)
{
    FP_MESSAGE_STREAM_T req = {FP_MSG_GROUP_ACKNOWLEDGEMENT, FP_MSG_ACK, 0, 2};

    req.data[0] = msgGroup;
    req.data[1] = msgCode;

    gfps_send(devId, (uint8_t *)&req, FP_MESSAGE_RESERVED_LEN+2);
}

void gfps_send_msg_nak(uint8_t devId, uint8_t reason, uint8_t msgGroup, uint8_t msgCode)
{
    FP_MESSAGE_STREAM_T req = {FP_MSG_GROUP_ACKNOWLEDGEMENT, FP_MSG_NAK, 0, 3};

    req.data[0] = reason;
    req.data[1] = msgGroup;
    req.data[2] = msgCode;

    gfps_send(devId, (uint8_t *)&req, FP_MESSAGE_RESERVED_LEN+3);
}

void gfps_send_sync_ring_status(uint8_t devId, uint8_t status)
{
    FP_MESSAGE_STREAM_T req = {FP_MSG_GROUP_DEVICE_ACTION, FP_MSG_DEVICE_ACTION_RING, 0, 1};

    req.data[0] = status;

    gfps_send(devId, (uint8_t *)&req, FP_MESSAGE_RESERVED_LEN+1);
}

uint32_t gfps_get_model_id(void)
{
    return gfpsEnv.keyInfo.model_id;
}

void gfps_get_key_info(void)
{
    gfpsEnv.keyInfo.model_id = Get_ModelId();
    switch(gfpsEnv.keyInfo.model_id)
    {
        //default model id(bes moddel id)
        case FP_DEVICE_MODEL_ID:
        {
            memcpy (gfpsEnv.keyInfo.public_anti_spoofing_key,
                    ble_public_key, sizeof(ble_public_key));
            memcpy (gfpsEnv.keyInfo.private_anti_spoofing_key,
                    ble_private_key, sizeof(ble_private_key));
        }
        break;

       default:
       {
#if defined(BESUI_FIX_GFPS_MODELID_EN)
           gfpsEnv.keyInfo.model_id = besui_gfps_get_nv_model_id();
#else
           gfpsEnv.keyInfo.model_id = FP_DEVICE_MODEL_ID;
#endif
           memcpy (gfpsEnv.keyInfo.public_anti_spoofing_key,
                    ble_public_key, sizeof(ble_public_key));
           memcpy (gfpsEnv.keyInfo.private_anti_spoofing_key,
                    ble_private_key, sizeof(ble_private_key));
       }
    }
}

const uint8_t* gfps_get_public_key(void) {
  return  gfpsEnv.keyInfo.public_anti_spoofing_key;
}

const uint8_t* gfps_get_private_key(void) {
  return gfpsEnv.keyInfo.private_anti_spoofing_key;
}

void gfps_enter_pairing_mode_handler(void)
{
    gfps_bta_enable_pairing_mode(true);
#ifdef __INTERCONNECTION__
    clear_discoverable_adv_timeout_flag();
    app_interceonnection_start_discoverable_adv(INTERCONNECTION_BLE_FAST_ADVERTISING_INTERVAL,
            APP_INTERCONNECTION_FAST_ADV_TIMEOUT_IN_MS);
#endif
}

void gfps_enter_fastpairing_mode(void)
{
    GFPS_TRACE(0,"[FP] enter fast pair mode");
    btif_sec_set_io_capabilities(3);
    btif_sec_set_authrequirements(0);
    bes_bt_me_confirmation_register_callback(NULL);
    app_ble_core_unregister_global_handler_ind(gfps_process_ble_user_confirmation);
    gfps_set_if_just_accept_retroactive(false);
    gfps_set_in_fastpairing_mode_flag(true);
    bts_ble_gap_refresh_adv();
}

bool gfps_is_in_fastpairing_mode(void)
{
    return gfpsEnv.isFastPairMode;
}

void gfps_set_in_fastpairing_mode_flag(bool isEnabled)
{
    gfpsEnv.isFastPairMode = isEnabled;
    GFPS_TRACE(1,"[FP]mode is set to %d", gfpsEnv.isFastPairMode);
}

void gfps_exit_fastpairing_mode(void)
{
    gfps_bta_enable_pairing_mode(false);

    gfps_set_in_fastpairing_mode_flag(false);
    // reset ble adv
    app_ble_refresh_adv_state_generic();
}

void gfps_set_ring_mode(uint8_t ring_mode)
{
    GFPS_TRACE(2, "%s ring_mode:%d", __func__,ring_mode);

    gfpsEnv.ringMode = ring_mode;
}

uint8_t gfps_get_ring_mode()
{
    return gfpsEnv.ringMode;
}

void gfps_start_find_ringtone(void)
{
#ifdef MEDIA_PLAYER_SUPPORT
#if defined(GFPS_FIND_VOICE_LOOP_EN) && defined(BESUI_TWS_EN)
    media_PlayAudio_standalone_locally(AUDIO_ID_FIND_TILE, 0);
#else
    media_PlayAudio(AUDIO_ID_FIND_TILE, 0);
#endif
#endif
}

void gfps_stop_find_ringtone(void)
{
    app_voice_stop(APP_STATUS_INDICATION_TILE_FIND, 0);
}

POSSIBLY_UNUSED static void gfps_delay_to_start_find_mybuds()
{
    if (gfps_find_mybuds_timer_id == NULL)
        gfps_find_mybuds_timer_id = osTimerCreate(osTimer(GFPS_FIND_MYBUDS_TIMER), osTimerOnce, NULL);
    osTimerStart(gfps_find_mybuds_timer_id, 1000);
}

#if defined(BESUI_FMD_UI_EN)
void app_voice_start_fmd_find(bool param)
{
#ifdef MEDIA_PLAYER_SUPPORT
    if(param)
        media_PlayAudio(AUDIO_ID_FIND_TILE, 0);
    else
        media_PlayAudio_standalone_locally(AUDIO_ID_FIND_TILE, 0);
#endif
}
#endif

#if defined(GFPS_FIND_VOICE_LOOP_EN) || defined(SPOT_ENABLED)
uint8_t prompt_curr_volume = TGT_VOLUME_LEVEL_1;
uint8_t app_prompt_gfps_set_volume(uint8_t value)
{
    prompt_curr_volume = value;
    return value;
}
uint8_t app_prompt_gfps_get_volume(void)
{
    return prompt_curr_volume;
}

osTimerId gfps_find_voice_process_timer = NULL;
void gfps_find_voice_timer_onoff(bool timer_en);

void gfps_find_voice_timehandler(void const *param)
{
	GFPS_TRACE(1, "[UITIMER]%s", __func__);

    gfps_find_voice_timer_onoff(true);

    prompt_curr_volume ++;
    if(prompt_curr_volume >= TGT_VOLUME_LEVEL_MAX)
        prompt_curr_volume = TGT_VOLUME_LEVEL_MAX-1;
	GFPS_TRACE(1, "[UITIMER]%s prompt_curr_volume = %d ", __func__, prompt_curr_volume);
    app_prompt_gfps_set_volume(prompt_curr_volume);

    gfps_start_find_ringtone();
}

osTimerDef (GFPS_FIND_VOICE_PROCESS_TIMER, (void (*)(void const *))gfps_find_voice_timehandler);

void gfps_find_voice_timer_onoff(bool timer_en)
{
	GFPS_TRACE(1, "[UITIMER]%s timer_en %d ", __func__, timer_en);

	if(gfps_find_voice_process_timer == NULL)
	{
    	gfps_find_voice_process_timer = osTimerCreate(osTimer(GFPS_FIND_VOICE_PROCESS_TIMER),osTimerOnce,NULL);
        if(!gfps_find_voice_process_timer)
        {
             ASSERT(0, "[UITIMER][%s] osTimerCreate error", __func__);
        }
	}

	if(timer_en)
	{
		osTimerStart(gfps_find_voice_process_timer, 3000);
	}
	else
	{
		osTimerStop(gfps_find_voice_process_timer);
	}
}
uint8_t app_get_find_my_buds_cur_status(void)
{
    GFPS_TRACE(1,"[UIGFPS]%s, gfpsEnv.findState = %d", __func__, gfpsEnv.findState);
    if(gfpsEnv.findState == GFPS_MUSIC_OFF_FIND_ON)
        return 1;

    return 0;
}
#endif

void gfps_find_sm(bool find_on_off)
{
    bool music_on_off = 0;
    osTimerStop(gfps_find_mybuds_timer_id);
    GFPS_TRACE(3, "%s %d %d", __func__, find_on_off, gfpsEnv.findState);
#if GFPS_FIND_VOICE_LOOP_EN
    if(find_on_off)
    {
        app_prompt_gfps_set_volume(TGT_VOLUME_LEVEL_1);
        gfps_find_voice_timer_onoff(true);
    }
    else
    {
        gfps_find_voice_timer_onoff(false);
    }
#endif
    switch(gfpsEnv.findState)
    {
        case GFPS_MUSIC_OFF_FIND_OFF:
            if(find_on_off)
            {
                gfps_start_find_ringtone();
                if(music_on_off)
                    gfpsEnv.findState=GFPS_MUSIC_ON_FIND_ON;
                else
                    gfpsEnv.findState=GFPS_MUSIC_OFF_FIND_ON;
            }
            else
            {
                if(music_on_off)
                {
                    gfpsEnv.findState=GFPS_MUSIC_ON_FIND_OFF;
                }
                gfps_stop_find_ringtone();
            }
            break;

        case GFPS_MUSIC_OFF_FIND_ON:
            if(find_on_off)
            {
                if(music_on_off)
                {
                    gfps_stop_find_ringtone();
                    gfps_delay_to_start_find_mybuds();
                    gfpsEnv.findState=GFPS_MUSIC_ON_FIND_OFF;//DELAY TO SATRT SM,TO OPEN FIND
                }
                else
                {
                    gfps_start_find_ringtone();
                }
                //gfps_start_find_ringtone();
            }
            else
            {
                gfps_stop_find_ringtone();
                if(music_on_off)
                    gfpsEnv.findState=GFPS_MUSIC_ON_FIND_OFF;
                else
                    gfpsEnv.findState=GFPS_MUSIC_OFF_FIND_OFF;
            }
            break;

        case GFPS_MUSIC_ON_FIND_OFF:
            if(find_on_off)
            {
                if(music_on_off)
                {
                    gfpsEnv.findState=GFPS_MUSIC_ON_FIND_ON;
                }
                else
                {
                    gfpsEnv.findState=GFPS_MUSIC_OFF_FIND_ON;
                }
                gfps_start_find_ringtone();
            }
            else
            {
                if(!music_on_off)
                {
                    gfpsEnv.findState=GFPS_MUSIC_OFF_FIND_OFF;
                }
                gfps_stop_find_ringtone();
            }
            break;

        case GFPS_MUSIC_ON_FIND_ON:
            if(find_on_off)
            {
                if(!music_on_off)
                {
                    gfpsEnv.findState=GFPS_MUSIC_OFF_FIND_ON;
                }
                gfps_start_find_ringtone();
            }
            else
            {
                gfps_stop_find_ringtone();
                if(music_on_off)
                {
                    gfpsEnv.findState=GFPS_MUSIC_ON_FIND_OFF;
                }
                else
                {
                    gfpsEnv.findState=GFPS_MUSIC_OFF_FIND_OFF;
                }
            }
            break;
    }
}

static void gfps_find_mybuds_timer_cb(void const *n)
{
    gfps_find_sm(true);
}

void gfps_set_find_my_buds_peer_status(bool onoff)
{
    gfpsEnv.findPeer = onoff;
}

void gfps_set_find_my_buds_stereo(uint8_t mode)
{
    switch (mode)
    {
        case GFPS_FIND_MY_BUDS_CMD_START:
#if defined(BESUI_STEREO_EN) && defined(GFPS_FIND_VOICE_LOOP_EN)
            if(!app_get_find_my_buds_cur_status())
                gfps_find_sm(true);
#else
            media_PlayAudio_standalone(AUDIO_ID_FIND_MY_BUDS, 0);
#endif
            break;
        case GFPS_FIND_MY_BUDS_CMD_STOP:
#if defined(BESUI_STEREO_EN) && defined(GFPS_FIND_VOICE_LOOP_EN)
            gfps_find_sm(false);
#endif
            app_voice_stop(APP_STATUS_INDICATION_FIND_MY_BUDS, 0);
        default:
            break;
    }
}

static void gfps_set_find_my_buds(uint8_t cmd)
{
    GFPS_TRACE(2,"%s, cmd = %d", __func__, cmd);
#if defined(BT_SVC_MODULE_IBRT_ENABLED)
    if(GFPS_FIND_MY_BUDS_CMD_STOP_DUAL == cmd)
    {
        gfps_set_ring_mode(GFPS_RING_MODE_BOTH_OFF);
        gfps_find_sm(false);
        gfps_set_find_my_buds_peer_status(0);
    }
    else if(GFPS_FIND_MY_BUDS_CMD_START_MASTER_ONLY == cmd)    //right ring, stop left
    {
#if GFPS_FIND_VOICE_LOOP_EN
        if(!app_get_find_my_buds_cur_status())
#endif
        gfps_set_ring_mode(GPFS_RING_MODE_RIGHT_ON);
        if (bta_tws_get_location() == BT_LOCATION_LEFT)
        {
            gfps_set_find_my_buds_peer_status(1);
            gfps_find_sm(false);
        }
        else
        {
#if GFPS_FIND_VOICE_LOOP_EN
            if(!app_get_find_my_buds_cur_status())
#endif
            gfps_find_sm(true);
        }
    }
    else if(GFPS_FIND_MY_BUDS_CMD_START_SLAVE_ONLY == cmd)    //left ring, stop right
    {
#if GFPS_FIND_VOICE_LOOP_EN
        if(!app_get_find_my_buds_cur_status())
#endif
        gfps_set_ring_mode(GFPS_RING_MODE_LEFT_ON);
        if (bta_tws_get_location() == BT_LOCATION_LEFT)
        {
#if GFPS_FIND_VOICE_LOOP_EN
        if(!app_get_find_my_buds_cur_status())
#endif
            gfps_find_sm(true);
        }
        else
        {
           gfps_set_find_my_buds_peer_status(1);
           gfps_find_sm(false);
        }
    }
    else if(GFPS_FIND_MY_BUDS_CMD_START_DUAL == cmd)    //both ring
    {
#if GFPS_FIND_VOICE_LOOP_EN
        if(!app_get_find_my_buds_cur_status())
            gfps_find_sm(true);
        else
            gfps_set_find_my_buds_peer_status(1);
#else
        gfps_set_ring_mode(GFPS_RING_MODE_BOTH_ON);
        gfps_find_sm(true);
        gfps_set_find_my_buds_peer_status(1);
#endif
    }
#else
    gfps_set_find_my_buds_stereo(cmd);
#endif
}

static void gfps_find_devices_ring_timeout_handler(void const *param)
{
    GFPS_TRACE(0,"gfps_find_devices_ring_timeout_handler");
    app_bt_start_custom_function_in_bt_thread(GFPS_FIND_MY_BUDS_CMD_STOP_DUAL, 0, \
                                (uint32_t)gfps_set_find_my_buds);
}

void gfps_ring_timer_set(uint8_t period)
{
    GFPS_TRACE(2,"%s, period = %d", __func__, period);
    if (ring_timeout_timer_id == NULL)
    {
        ring_timeout_timer_id = osTimerCreate(osTimer(GFPS_FIND_DEVICES_RING_TIMEOUT), osTimerOnce, NULL);
    }

    osTimerStop(ring_timeout_timer_id);
    if(period)
    {
        osTimerStart(ring_timeout_timer_id, period*1000);
    }
}

static void gfps_ring_request_handling(uint8_t devId, uint8_t* requestdata, uint16_t datalen)
{
    GFPS_TRACE(1,"%s,[RFCOMM][FMD] request",__func__);
    GFPS_DUMP8("%02x ", requestdata, datalen);
#if defined(BT_SVC_MODULE_IBRT_ENABLED)
    switch (requestdata[0])
    {
        case GFPS_FIND_MY_BUDS_CMD_START_MASTER_ONLY://ring right
            if (!(bta_tws_get_location() == BT_LOCATION_LEFT))
            {
                if (bta_tws_get_ui_role() == BT_IBRT_MASTER)      //right is master
                {
                    if (bta_tws_get_box_state(false) == BTA_TWS_IN_BOX_OPEN)
                    {
                        GFPS_TRACE(0,"right phone-right master in box");
                        gfps_send_msg_nak(devId, FP_MSG_NAK_REASON_NOT_ALLOWED,FP_MSG_GROUP_DEVICE_ACTION, FP_MSG_DEVICE_ACTION_RING);
                        gfps_send_sync_ring_status(devId, GFPS_FIND_MY_BUDS_CMD_STOP_DUAL);

                        gfps_send_msg_ack(devId, FP_MSG_GROUP_DEVICE_ACTION, FP_MSG_DEVICE_ACTION_RING);
                        if (datalen > 1)
                        {
                             gfps_ring_timer_set(GFPS_FIND_MY_BUDS_CMD_STOP_DUAL);
                         }

                         gfps_set_find_my_buds(GFPS_FIND_MY_BUDS_CMD_STOP_DUAL);
                         return;
                    }
                    else
                    {
                        GFPS_TRACE(0,"right phone-right master out box");
                    }

                }
                else    //right is slave
                {
                    if(bta_tws_get_box_state(false) == BTA_TWS_IN_BOX_OPEN)
                    {
                        GFPS_TRACE(0,"right phone-right slave in box");
                        return;
                    }
                    {
                         GFPS_TRACE(0,"right phone-right slave out box");
                     }
                }
            }
            else if (bta_tws_get_location() == BT_LOCATION_LEFT)
            {
                if (bta_tws_get_ui_role() == BT_IBRT_MASTER)     //left is master
                {
                    if (bta_tws_get_box_state(true) == BTA_TWS_IN_BOX_OPEN)
                    {
                        GFPS_TRACE(0,"right phone-left master but right in box");
                        gfps_send_msg_nak(devId, FP_MSG_NAK_REASON_NOT_ALLOWED,FP_MSG_GROUP_DEVICE_ACTION, FP_MSG_DEVICE_ACTION_RING);
                        gfps_send_sync_ring_status(devId, GFPS_FIND_MY_BUDS_CMD_STOP_DUAL);

                        gfps_send_msg_ack(devId, FP_MSG_GROUP_DEVICE_ACTION, FP_MSG_DEVICE_ACTION_RING);
                        if (datalen > 1)
                        {
                            gfps_ring_timer_set(GFPS_FIND_MY_BUDS_CMD_STOP_DUAL);
                        }
                        gfps_set_find_my_buds(GFPS_FIND_MY_BUDS_CMD_STOP_DUAL);
                        return;
                    }
                    else
                    {
                        GFPS_TRACE(0,"right phone-left master but right out box");
                    }
                }
                else
                {
                    if(bta_tws_get_box_state(false) == BTA_TWS_IN_BOX_OPEN)
                    {
                        GFPS_TRACE(0,"right phone-left slave int box");
                    }
                    else
                    {
                        GFPS_TRACE(0,"right phone-left slave out box");
                    }
                }
            }
            break;
        case GFPS_FIND_MY_BUDS_CMD_START_SLAVE_ONLY://ring left
            if (bta_tws_get_location() == BT_LOCATION_LEFT)
            {
                if (bta_tws_get_ui_role()  == BT_IBRT_MASTER)   //left is master
                {
                    if (bta_tws_get_box_state(false) == BTA_TWS_IN_BOX_OPEN)
                    {
                        GFPS_TRACE(0,"left phone-left master in box");
                        gfps_send_msg_nak(devId, FP_MSG_NAK_REASON_NOT_ALLOWED,FP_MSG_GROUP_DEVICE_ACTION, FP_MSG_DEVICE_ACTION_RING);
                        gfps_send_sync_ring_status(devId, GFPS_FIND_MY_BUDS_CMD_STOP_DUAL);
                        return;
                    }
                    else
                    {
                        GFPS_TRACE(0,"left phone-left master out box");
                    }
                }
                else  //left is slave
                {
                    if (bta_tws_get_box_state(false) == BTA_TWS_IN_BOX_OPEN)
                    {
                        GFPS_TRACE(0,"left phone-left slave in box");
                        return;
                    }
                    else
                    {
                        GFPS_TRACE(0,"left phone-left slave out box");
                    }
                }

            }
            else if (!(bta_tws_get_location() == BT_LOCATION_LEFT))
            {
                if (bta_tws_get_ui_role() == BT_IBRT_MASTER)//right is master
                {
                     if (bta_tws_get_box_state(true) == BTA_TWS_IN_BOX_OPEN)
                    {
                        GFPS_TRACE(0,"left phone-right master but left in box");
                        gfps_send_msg_nak(devId, FP_MSG_NAK_REASON_NOT_ALLOWED,FP_MSG_GROUP_DEVICE_ACTION, FP_MSG_DEVICE_ACTION_RING);
                        gfps_send_sync_ring_status(devId, GFPS_FIND_MY_BUDS_CMD_STOP_DUAL);
                        return;
                    }
                    else
                    {
                        GFPS_TRACE(0,"left phone-right master but left out box");
                    }
                }
                else
                {
                    GFPS_TRACE(0,"left phone-right slave in box or out box");
                }
            }
            break;

            case GFPS_FIND_MY_BUDS_CMD_START_DUAL:
            if (bta_tws_get_location() == BT_LOCATION_LEFT)
            {
                if (bta_tws_get_ui_role()  == BT_IBRT_MASTER)    //left is master
                {
                    if (bta_tws_get_box_state(false) == BTA_TWS_IN_BOX_OPEN)
                    {
                        GFPS_TRACE(0,"DUAL left phone-left master in box");
                        gfps_send_msg_nak(devId, FP_MSG_NAK_REASON_NOT_ALLOWED,FP_MSG_GROUP_DEVICE_ACTION, FP_MSG_DEVICE_ACTION_RING);
                        if (bta_tws_get_box_state(true) == BTA_TWS_IN_BOX_OPEN)
                        {
                            gfps_send_sync_ring_status(devId, GFPS_FIND_MY_BUDS_CMD_STOP_DUAL);
                            return ;
                        }
                        else
                        {
                            gfps_send_sync_ring_status(devId, GFPS_FIND_MY_BUDS_CMD_START_MASTER_ONLY);
                            return ;
                        }
                    }
                    else
                    {
                        if (bta_tws_get_box_state(true) == BTA_TWS_IN_BOX_OPEN)
                        {
                            gfps_send_msg_nak(devId, FP_MSG_NAK_REASON_NOT_ALLOWED,FP_MSG_GROUP_DEVICE_ACTION, FP_MSG_DEVICE_ACTION_RING);
                            gfps_send_sync_ring_status(devId, GFPS_FIND_MY_BUDS_CMD_START_SLAVE_ONLY);

                            gfps_send_msg_ack(devId, FP_MSG_GROUP_DEVICE_ACTION, FP_MSG_DEVICE_ACTION_RING);

                            if (datalen > 1)
                            {
                                gfps_ring_timer_set(GFPS_FIND_MY_BUDS_CMD_START_SLAVE_ONLY);
                            }
                            GFPS_TRACE(0,"DAUL need left ring");
                            return; 
                        }

                        GFPS_TRACE(0,"DAUL left phone-left master BOTH out box");
                    }
                }
                else    //left is slave
                {
                    if (bta_tws_get_box_state(false) == BTA_TWS_IN_BOX_OPEN)
                    {
                        GFPS_TRACE(0,"left phone-left slave in box");
                        return;
                    }
                    else
                    {
                        GFPS_TRACE(0,"left phone-left slave out box");
                        if (bta_tws_get_box_state(true) == BTA_TWS_IN_BOX_OPEN)
                        {
                            gfps_set_find_my_buds(GFPS_FIND_MY_BUDS_CMD_START_SLAVE_ONLY);
                        }
                        else
                        {
                            gfps_set_find_my_buds(GFPS_FIND_MY_BUDS_CMD_START_DUAL);
                        }
                        return;
                    }
                }
            }
            else if (!(bta_tws_get_location() == BT_LOCATION_LEFT))
            {
                if (bta_tws_get_ui_role() == BT_IBRT_MASTER)    //right is master
                {
                    if (bta_tws_get_box_state(false) == BTA_TWS_IN_BOX_OPEN)
                    {
                        gfps_send_msg_nak(devId, FP_MSG_NAK_REASON_NOT_ALLOWED,FP_MSG_GROUP_DEVICE_ACTION, FP_MSG_DEVICE_ACTION_RING);
                        if (bta_tws_get_box_state(true) == BTA_TWS_IN_BOX_OPEN)
                        {
                            GFPS_TRACE(0,"DUAL phone-right master and BOTH in box");
                            gfps_send_sync_ring_status(devId, GFPS_FIND_MY_BUDS_CMD_STOP_DUAL);
                            return ;
                        }
                        else
                        {
                            GFPS_TRACE(0,"DUAL phone-right master and in box");
                            gfps_send_sync_ring_status(devId, GFPS_FIND_MY_BUDS_CMD_START_SLAVE_ONLY);
                            return ;
                        }
                    }
                    else
                    {
                        if (bta_tws_get_box_state(true) == BTA_TWS_IN_BOX_OPEN)
                        {
                            GFPS_TRACE(0,"DUAL phone-right master BUT LEFT in box");
                            gfps_send_msg_nak(devId, FP_MSG_NAK_REASON_NOT_ALLOWED,FP_MSG_GROUP_DEVICE_ACTION, FP_MSG_DEVICE_ACTION_RING);
                            gfps_send_sync_ring_status(devId, GFPS_FIND_MY_BUDS_CMD_START_MASTER_ONLY);

                            gfps_send_msg_ack(devId, FP_MSG_GROUP_DEVICE_ACTION, FP_MSG_DEVICE_ACTION_RING);

                            if (datalen > 1)
                            {
                                gfps_ring_timer_set(GFPS_FIND_MY_BUDS_CMD_START_MASTER_ONLY);
                            }
                            GFPS_TRACE(0,"DUAL ring right");

                            gfps_set_find_my_buds(GFPS_FIND_MY_BUDS_CMD_START_MASTER_ONLY);
                            return;
                        }
                        else
                        {
                            GFPS_TRACE(0,"DUAL phone-right master BOTH NOT IN box");
                        }
                    }
                }
                else    //left is slave
                {
                    GFPS_TRACE(0,"left phone-right slave in box or out box");
                    if (bta_tws_get_box_state(false) == BTA_TWS_IN_BOX_OPEN)
                    {
                        GFPS_TRACE(0,"left phone-left slave in box");
                        return ;
                    }
                    else
                    {
                         GFPS_TRACE(0,"left phone-left slave out box");
                         if (bta_tws_get_box_state(true) == BTA_TWS_IN_BOX_OPEN)
                         {
                             gfps_set_find_my_buds(GFPS_FIND_MY_BUDS_CMD_START_MASTER_ONLY);
                         }
                         else
                         {
                             gfps_set_find_my_buds(GFPS_FIND_MY_BUDS_CMD_START_DUAL);
                         }
                         return;
                    }
                    return;
                }

            }
            break;

            default:
                break;
    }

    if (bta_tws_get_box_state(true) == BTA_TWS_IN_BOX_OPEN)
    {
        GFPS_TRACE(0,"######both earbud is in box");
        gfps_send_msg_nak(devId, FP_MSG_NAK_REASON_NOT_ALLOWED, FP_MSG_GROUP_DEVICE_ACTION, FP_MSG_DEVICE_ACTION_RING);
        return;
    }
#endif // BT_SVC_MODULE_IBRT_ENABLED

    gfps_send_msg_ack(devId, FP_MSG_GROUP_DEVICE_ACTION, FP_MSG_DEVICE_ACTION_RING);
     if (datalen > 1)
    {
        gfps_ring_timer_set(requestdata[1]);
    }

    gfps_set_find_my_buds(requestdata[0]);
}

// use cases for fp message stream
void gfps_enable_bt_silence_mode(uint8_t devId, bool isEnable)
{
    if (gfpsEnv.fpCap.env.isSilentModeSupported)
    {
        FP_MESSAGE_STREAM_T req = {FP_MSG_GROUP_BLUETOOTH_EVENT, 0, 0, 0};
        if (isEnable)
        {
            req.messageCode = FP_MSG_BT_EVENT_ENABLE_SILENCE_MODE;
        }
        else
        {
            req.messageCode = FP_MSG_BT_EVENT_DISABLE_SILENCE_MODE;
        }

        gfps_send(devId, (uint8_t *)&req, FP_MESSAGE_RESERVED_LEN);
    }
    else
    {
        GFPS_TRACE(0,"fp silence mode is not supported.");
    }
}

void gfps_send_model_id(uint8_t devId)
{
    GFPS_TRACE(1,"%s",__func__);
    uint32_t model_id = gfps_get_model_id();

    uint8_t  modelID[3];
    modelID[0] = (model_id >> 16) & 0xFF;
    modelID[1] = (model_id >> 8) & 0xFF;
    modelID[2] = ( model_id )&0xFF;

    uint16_t rawDataLen = sizeof(modelID);

    FP_MESSAGE_STREAM_T req =
        {FP_MSG_GROUP_DEVICE_INFO,
         FP_MSG_DEVICE_INFO_MODEL_ID,
         (uint8_t)(rawDataLen >> 8),
         (uint8_t)(rawDataLen & 0xFF)};
    memcpy(req.data, modelID, sizeof(modelID));

    gfps_send(devId, ( uint8_t * )&req, FP_MESSAGE_RESERVED_LEN + rawDataLen);
}

void gfps_get_updated_ble_addr(uint8_t devId, uint8_t *addr)
{
    bool isIdentity = false;
    uint8_t ptr[6] = {0};
    POSSIBLY_UNUSED ble_bdaddr_t bleAddr;
    if (devId == 0xFF)
    {
        return;
    }

    if (IS_BT_DEVICE(devId))
    {
        bt_bdaddr_t btAddr;
        if (bta_get_addr_by_device_id(GET_BT_ID(devId), &btAddr))
        {
            if (nv_record_blerec_is_paired_from_addr(btAddr.address))
            {
                GFPS_TRACE(1, "%s get paired dev from nv", __func__);
                isIdentity = true;
            }
        }
    }
    else
    {
        isIdentity = true;
    }

    if (isIdentity)
    {
        bleAddr = bts_ble_gap_get_local_identity_addr(0xFF);
        memcpy(ptr, bleAddr.addr, 6);
    }
    else
    {
        gfps_ble_get_addr(ptr);
    }

    for (uint8_t index = 0; index < 6; index++)
    {
        addr[index] = ptr[5 - index];
    }
}

void gfps_send_ble_addr(uint8_t devId)
{
    FP_MESSAGE_STREAM_T req =
        {FP_MSG_GROUP_DEVICE_INFO,
         FP_MSG_DEVICE_INFO_BLE_ADD_UPDATED,
         0,
         6};

    if (devId != 0xFF)
    {
        gfps_get_updated_ble_addr(devId, req.data);
        gfps_send(devId,( uint8_t * )&req, FP_MESSAGE_RESERVED_LEN + 6);
    }
}

void gfps_send_battery_levels(uint8_t devId)
{ 
    uint8_t batteryLevelCount = 0;
    FP_MESSAGE_STREAM_T req =
        {FP_MSG_GROUP_DEVICE_INFO,
         FP_MSG_DEVICE_INFO_BATTERY_UPDATED,
         0,
         0};

    gfps_get_battery_levels(&batteryLevelCount, req.data);
    req.dataLenLowByte = batteryLevelCount;

    gfps_send(devId, ( uint8_t * )&req, FP_MESSAGE_RESERVED_LEN + batteryLevelCount);
}

#if defined(SPOT_ENABLED)
void gfps_send_firmware_version(uint8_t devId)
{
    GFPS_TRACE(1,"%s",__func__);
    FP_MESSAGE_STREAM_T req =
        {FP_MSG_GROUP_DEVICE_INFO,
         FP_MSG_DEVICE_INFO_FIRMWARE_VERSION,
         0};
    char firware_revision[17]= {0};
    uint8_t fw_rev_0 = 0, fw_rev_1 = 0, fw_rev_2 = 0,fw_rev_3=1;
#ifdef FIRMWARE_REV
    system_get_info(&fw_rev_0, &fw_rev_1, &fw_rev_2, &fw_rev_3);
#endif
    snprintf(firware_revision, sizeof(firware_revision), "%d.%d.%d.%d", fw_rev_0, fw_rev_1, fw_rev_2,fw_rev_3);
    memcpy(&req.data[0], firware_revision,strlen(firware_revision));
    req.dataLenLowByte = strlen(firware_revision);

    gfps_send(devId,( uint8_t * )&req, FP_MESSAGE_RESERVED_LEN + strlen(firware_revision));
}

void gfps_send_eddystone_identifier_state(uint8_t devId)
{
    GFPS_TRACE(1,"%s",__func__);
    FP_MESSAGE_STREAM_T req =
        {FP_MSG_GROUP_DEVICE_INFO,
         FP_MSG_DEVICE_INFO_EDD_IDENTIFIER,
         0,
         24};
    uint32_t current_time = nv_record_fp_get_current_beacon_time() + GET_CURRENT_MS()/1000;
    uint8_t time[4];
    memcpy(time, &current_time, sizeof(uint32_t));
    big_little_switch(time, &req.data[0], sizeof(uint32_t));

    memcpy(&req.data[4], nv_record_fp_get_spot_adv_data(), 20);

    gfps_send(devId,( uint8_t * )&req, FP_MESSAGE_RESERVED_LEN + 24);
}

void gfps_spot_event_handler(uint8_t devId)
{
    FP_MESSAGE_STREAM_T req = {FP_MSG_GROUP_DEVICE_CAPABLITY_SYNC, FP_MSG_DEVICE_CAPABLITY_ENABLE_EDD_TRACK, 0, 7};

    req.data[0] = nv_record_fp_get_spot_adv_enable_value();
    if(devId != 0xFF)
    {
        gfps_get_updated_ble_addr(devId, &req.data[1]);
        gfps_send(devId, (uint8_t *)&req, FP_MESSAGE_RESERVED_LEN + 7);
    }
}
#endif

uint8_t gfps_get_bt_iocap(void)
{
    return gfpsEnv.bt_iocap;
}

void gfps_set_bt_iocap(uint8_t ioCap)
{
    if (gfpsEnv.btSetIocap)
    {
        gfpsEnv.btSetIocap(ioCap);
    }
}

uint8_t gfps_get_bt_auth(void)
{
    return gfpsEnv.bt_authrequirements;
}

void gfps_set_bt_auth(uint8_t auth)
{
    if (gfpsEnv.btSetAuthrequirements)
    {
        gfpsEnv.btSetAuthrequirements(auth);
    }
}

void gfps_enter_pairing_mode(void)
{
    if (gfpsEnv.enterPairingMode)
    {
        gfpsEnv.enterPairingMode();
    }
}

uint8_t *gfps_get_last_response(void)
{
    return gfpsEnv.pendingLastResponse;
}

bool gfps_is_last_response_pending(void)
{
    return gfpsEnv.isLastResponsePending;
}

void gfps_reg_battery_handler(gfps_get_battery_info_cb cb)
{
    gfpsEnv.getBatteryHandler = cb;
}

void gfps_set_battery_datatype(GFPS_BATTERY_DATA_TYPE_E batteryDataType)
{
    if (gfpsEnv.batteryDataType != batteryDataType)
    {
        gfpsEnv.batteryDataType = batteryDataType;
        app_ble_refresh_adv_state_generic();
    }
}

GFPS_BATTERY_DATA_TYPE_E gfps_get_battery_datatype(void)
{
    return gfpsEnv.batteryDataType;
}

void gfps_enable_battery_info(bool isEnable)
{
    gfpsEnv.isBatteryInfoIncluded = isEnable;
    app_ble_refresh_adv_state_generic();
}

bool gfps_is_battery_enabled(void)
{
    return gfpsEnv.isBatteryInfoIncluded;
}

void gfps_get_battery_levels(uint8_t *pCount, uint8_t *pBatteryLevel)
{
    *pCount = 0;
    if (gfpsEnv.getBatteryHandler)
    {
        gfpsEnv.getBatteryHandler(pCount, pBatteryLevel);
    }
}

void gfps_enter_connectable_mode_req_handler(uint8_t *response)
{
    GFPS_TRACE(2,"%s isLastResponsePending:%d", __func__, gfpsEnv.isLastResponsePending);
    GFPS_TRACE(0,"response data:");
    GFPS_DUMP8("%02x ", response, GFPSP_ENCRYPTED_RSP_LEN);

    bt_bdaddr_list_t dev_addr_l = { 0 };

    if (gfps_bta_get_bt_connected_dev_list(&dev_addr_l) >= gfps_bta_get_device_num_max())
    {
        memcpy(gfpsEnv.pendingLastResponse, response, GFPSP_ENCRYPTED_RSP_LEN);
        gfpsEnv.isLastResponsePending = true;
        gfps_bta_remove_bt_device(&dev_addr_l.addrs[0]);
    }
    else
    {
        gfpsEnv.isLastResponsePending = false;
        bes_ble_gfps_send_keybase_pairing(gfps_ble_get_connect_dev(), response, GFPSP_ENCRYPTED_RSP_LEN);
        GFPS_TRACE(0,"wait for pair req maybe classic or ble");

        gfps_enter_pairing_mode();

        gfps_set_bt_iocap(1);

        gfps_set_bt_auth(1);
    }
}

#if defined(SASS_ENABLED)
void gfps_sass_event_handler(uint8_t devId, uint8_t evt, void *param)
{
    GFPS_TRACE(3,"%s id:%d evt:0x%0x", __func__, devId, evt);
    /* bt_bdaddr_t currAddr;

    if(IS_BT_DEVICE(devId))
    {
        bta_get_addr_by_device_id(GET_BT_ID(devId), currAddr.address);
    }
    else
    {
        ble_bdaddr_t GetPeerAddr = {{0}};
        app_ble_get_peer_solved_addr(devId, &GetPeerAddr);
        memcpy(currAddr.address, GetPeerAddr.addr, 6);
    }
    gfps_sass_connect_handler(devId, &currAddr); */

    if (evt != FP_MSG_SASS_GET_CAPBILITY && evt != FP_MSG_SASS_GET_SWITCH_PREFERENCE && \
        evt != FP_MSG_SASS_GET_CONN_STATUS && evt != FP_MSG_SASS_SWITCH_BACK && \
        evt != FP_MSG_SASS_SWITCH_ACTIVE_SOURCE)
    {
        gfps_send_msg_ack(devId, FP_MSG_GROUP_SASS, evt);
    }

    gfps_sass_handler(devId, evt, param);
}
#endif

#if defined(GFPS_BR_EDR_SEC_CONN_SWITCH_ENABLED)
struct gfps_sc_info
{
    uint8_t org : 1;        // origin secure connection host support flag
    uint8_t cur : 1;        // current secure connection host support flag
    uint8_t pending : 1;    // secure connection host support pending restore
};
struct gfps_sc_info g_gfps_sc_info = { 0 };

// * This function will be registered to the BES BT thread.
// *    So for multi-thread safety, the other two functions should also be called in the same thread (use bt_exec_sync_x or bt_exec_async_x).
void gfps_recv_sec_conn_set_cmpl(bool enable)
{
    g_gfps_sc_info.org = enable;
    g_gfps_sc_info.cur = g_gfps_sc_info.pending ? g_gfps_sc_info.cur : enable;
    GFPS_TRACE(0, "%s: org(%d) cur(%d) pending(%d)", __func__, g_gfps_sc_info.org, g_gfps_sc_info.cur, g_gfps_sc_info.pending);
}

// * write sec conn host supp by calling driver api directly instead of sending hci cmd
void gfps_write_sec_conn_host_supp(bool enable)
{
    GFPS_TRACE(0, "%s: cur %d=>%d", __func__, g_gfps_sc_info.cur, enable);
    g_gfps_sc_info.pending = true;
    bt_drv_reg_op_wr_sec_con_host_supp(enable);
    g_gfps_sc_info.cur = enable;
}

void gfps_restore_sec_conn_host_supp(void)
{
    GFPS_TRACE(0, "%s: cur %d=>%d", __func__, g_gfps_sc_info.cur, g_gfps_sc_info.org);
    g_gfps_sc_info.pending = false;
    bt_drv_reg_op_wr_sec_con_host_supp(g_gfps_sc_info.org);
    g_gfps_sc_info.cur = g_gfps_sc_info.org;
}
#else // GFPS_BR_EDR_SEC_CONN_SWITCH_ENABLED
void gfps_set_le_con_allow_use_same_addr(uint8_t conidx, bool enable)
{
    static uint8_t conn_bf = 0;
    static bool curr_en = false;

    // * conidx masked by 0xF won't exceed 7 (BLE LINKS MAX NUM), so 8 bits is enough
    uint8_t conn_mask = CO_BIT_MASK((conidx & 0x0F));
    uint8_t new_bf = 0;

    new_bf = enable ? (conn_bf | conn_mask) : (conn_bf & (~conn_mask));
    GFPS_TRACE(0, "%s: conn_bf 0x%02x -> 0x%02x", __func__, conn_bf, new_bf);
    conn_bf = new_bf;

    // * conn_bf != 0 means that some conn(s) require(s) for allowing use the same addr
    // * 1. if any conn requires enable & curr is disabled
    // * 2. if none requires enable & curr is enabled
    // *   curr_en should be reversed and updated to the controller
    if ((conn_bf != 0) ^ curr_en)
    {
        curr_en = !curr_en;
        bt_drv_reg_op_set_le_con_allow_use_same_addr(curr_en);
    }
}
#endif // GFPS_BR_EDR_SEC_CONN_SWITCH_ENABLED

void app_ibrt_share_fastpair_info(uint8_t *p_buff, uint16_t length)
{
#if defined(BT_SVC_MODULE_IBRT_ENABLED)
    bta_tws_send_cmd_without_rsp(APP_TWS_CMD_SHARE_FASTPAIR_INFO, p_buff, length);
#endif
}

void app_tws_send_fastpair_info_to_slave(void)
{
#if defined(BT_SVC_MODULE_IBRT_ENABLED)
    GFPS_TRACE(0,"Send fastpair info to secondary device.");
    NV_FP_ACCOUNT_KEY_RECORD_T *pFpData = nv_record_get_fp_data_structure_info();
    app_ibrt_share_fastpair_info(( uint8_t * )pFpData, sizeof(NV_FP_ACCOUNT_KEY_RECORD_T));
#endif
}

void app_ibrt_share_fastpair_info_received_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
#if SASS_ENABLED && defined(BT_SVC_MODULE_IBRT_ENABLED)
    NV_FP_ACCOUNT_KEY_RECORD_T *pFpData = ( NV_FP_ACCOUNT_KEY_RECORD_T * )p_buff;
    nv_record_update_fp_data_structure(pFpData);
    if (length > sizeof(NV_FP_ACCOUNT_KEY_RECORD_T))
    {
        gfps_sass_set_sync_info(p_buff + sizeof(NV_FP_ACCOUNT_KEY_RECORD_T), length - sizeof(NV_FP_ACCOUNT_KEY_RECORD_T));
    }
#endif
}

void gfps_tws_info_prepare_handler(uint8_t *buf, uint16_t *totalLen, uint16_t *len, uint16_t expectLen)
{
    GFPS_TRACE(0,"Send fastpair info to secondary device.");
    NV_FP_ACCOUNT_KEY_RECORD_T *pFpData = nv_record_get_fp_data_structure_info();
    memcpy(buf, pFpData, sizeof(NV_FP_ACCOUNT_KEY_RECORD_T));
    *totalLen = *len = sizeof(NV_FP_ACCOUNT_KEY_RECORD_T);

#if defined(SASS_ENABLED)
    uint16_t sassLen = 0;
    gfps_sass_get_sync_info(buf+(*len), &sassLen);
    *totalLen += sassLen;
    *len += sassLen;   
#endif  
}

void gfps_tws_info_received_handler(uint8_t *buf, uint16_t length, bool isContinueInfo)
{
#if defined(BT_SVC_MODULE_IBRT_ENABLED)
    uint16_t offset = 0;
    nv_record_fp_update_all(buf);
    offset += sizeof(NV_FP_ACCOUNT_KEY_RECORD_T);
#if defined(SASS_ENABLED)
    gfps_sass_set_sync_info(buf + offset, length - offset);
#endif
#endif
}

void gfps_send_ble_disconnect_cmd_to_slave(uint8_t *buf, uint16_t length)
{
#if BLE_AUDIO_ENABLED && defined(BT_SVC_MODULE_IBRT_ENABLED)
    bta_tws_send_cmd_without_rsp(APP_TWS_CMD_SNED_GFPS_BLE_DISC_CMD, buf, length);
#endif
}

void gfps_send_streaming_state_to_master(bt_bdaddr_t *addr, uint8_t state)
{
#if BLE_AUDIO_ENABLED && defined(BT_SVC_MODULE_IBRT_ENABLED)
    GFPS_STREAMING_STATE_T syncInfo;
    memcpy(syncInfo.devAddr.address, addr->address, sizeof(bt_bdaddr_t));
    syncInfo.state = state;
    bta_tws_send_cmd_without_rsp(APP_TWS_CMD_SNED_STREAMING_STATE_CMD, (uint8_t *)&syncInfo, sizeof(GFPS_STREAMING_STATE_T));
#endif
}

void gfps_rec_peer_streaming_state_handler(uint8_t *p_buff, uint16_t length)
{
#if defined(SASS_ENABLED) && defined(BT_SVC_MODULE_IBRT_ENABLED)
    GFPS_STREAMING_STATE_T *syncInfo = (GFPS_STREAMING_STATE_T *)p_buff;
    gfps_sass_set_peer_streaming_state(&(syncInfo->devAddr), syncInfo->state);
#endif
}
#endif

void gfps_tws_sync_init(void)
{
#if defined(BT_SVC_MODULE_IBRT_ENABLED)
    // TODO: freddie move to isolated ota file
    TWS_SYNC_USER_T userGfps = {
        gfps_tws_info_prepare_handler,
        gfps_tws_info_received_handler,
        NULL,
        NULL,
        NULL,
    };

    bts_tws_if_register_tws_sync_user(TWS_SYNC_USER_GFPS_INFO, &userGfps);
#endif
}

void gfps_sync_info(void)
{
#if defined(BT_SVC_MODULE_IBRT_ENABLED)
    if (BT_IBRT_SLAVE != bta_tws_get_ui_role())
    {
        uint8_t info[FP_TWS_MAX_LEN];
        uint16_t offset = 0;
        GFPS_TRACE(0,"Send fastpair info to secondary device.");
        NV_FP_ACCOUNT_KEY_RECORD_T *pFpData = nv_record_get_fp_data_structure_info();
        memcpy(info, pFpData, sizeof(NV_FP_ACCOUNT_KEY_RECORD_T));
        offset += sizeof(NV_FP_ACCOUNT_KEY_RECORD_T);

#if defined(SASS_ENABLED)
        uint16_t sassLen = 0;
        gfps_sass_get_sync_info(info + offset, &sassLen);
        offset += sassLen;
        ASSERT(offset <= FP_TWS_MAX_LEN, "Len exceed FP_TWS_MAX_LEN");
#endif
        bta_tws_send_cmd(APP_TWS_CMD_SHARE_FASTPAIR_INFO, info, offset);
    }
#endif
}

void gfps_info_received_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
#if defined(BT_SVC_MODULE_IBRT_ENABLED)
    uint16_t offset = 0;
    nv_record_fp_update_all((uint8_t *)p_buff);
    offset += sizeof(NV_FP_ACCOUNT_KEY_RECORD_T);
#if defined(SASS_ENABLED)
    gfps_sass_set_sync_info(p_buff + offset, length - offset);
#endif
#endif
}

void gfps_role_switch_prepare(void)
{
#if defined(BT_SVC_MODULE_IBRT_ENABLED)
    gfps_sync_info();
#endif
}

void gfps_set_anc_current_state_info(uint8_t current_state_info)
{
    //need to get anc state
}

uint8_t gfps_get_anc_current_state_info(void)
{
    //need to update anc state
    return 0;
}

void gfps_notify_hearable_control_state(uint8_t devId)
{
    GFPS_TRACE(2,"%s,devId is %d", __func__, devId);

    FpANCState_t ancdata;
    FP_MESSAGE_STREAM_T req =
        {FP_MSG_GROUP_HEARABLE_CONTROL,
         FP_MSG_HC_NTF_ANC_STATE,
         0,
         0x04};

    ancdata.version = FP_HEARABLE_CONTROL_VERSION;
    //if transport off and buds are on head, current state is off
    ancdata.ui_transparent = 0x01;
    ancdata.ui_off = 0x01;
    ancdata.ui_anc = 0x01;

    ancdata.settable_transparent = 0x01;
    ancdata.settable_off = 0x01;
    ancdata.settable_anc = 0x01;

    ancdata.current_state_info = gfps_get_anc_current_state_info();

    memcpy(req.data, (uint8_t *)&ancdata, 0x04);
    gfps_send(devId,( uint8_t * )&req, FP_MESSAGE_RESERVED_LEN + 0x04);
}

void gfps_set_hearable_control_state(uint8_t devId, uint8_t *data)
{
    GFPS_TRACE(2,"%s,devId is %d", __func__, devId);
    GFPS_DUMP8("%02x ", data, 17);

    gfps_set_anc_current_state_info(data[0]);

    gfps_notify_hearable_control_state(devId);
    gfps_send_msg_ack(devId, FP_MSG_GROUP_HEARABLE_CONTROL, FP_MSG_HC_SET_ANC_STATE);
}

uint16_t gfps_data_handler(uint8_t devId, uint8_t* ptr, uint16_t len)
{
    FP_MESSAGE_STREAM_T* pMsg = (FP_MESSAGE_STREAM_T *)ptr;
    uint16_t datalen = 0;
    uint16_t msgLen = ((pMsg->dataLenHighByte << 8)|pMsg->dataLenLowByte) + \
                       FP_MESSAGE_RESERVED_LEN;
    GFPS_TRACE(4,"gfps receives msg group %d code %d len %d %d", pMsg->messageGroup, pMsg->messageCode, len, msgLen);
    if (len < msgLen)
    {
        return 0;
    }

    switch (pMsg->messageGroup)
    {
        case FP_MSG_GROUP_DEVICE_INFO:
        {
            switch (pMsg->messageCode)
            {
                case FP_MSG_DEVICE_INFO_ACTIVE_COMPONENTS_REQ:
                    gfps_send_active_components_rsp(devId);
                    break;
                case FP_MSG_DEVICE_INFO_TELL_CAPABILITIES:
                    gfpsEnv.fpCap.content = pMsg->data[0];
                    GFPS_TRACE(3,"cap 0x%x isCompanionAppInstalled %d isSilentModeSupported %d",
                        gfpsEnv.fpCap.content, gfpsEnv.fpCap.env.isCompanionAppInstalled,
                        gfpsEnv.fpCap.env.isSilentModeSupported);
                    break;
                default:
                    break;
            }
            break;
        }
        case FP_MSG_GROUP_DEVICE_ACTION:
        {
            switch (pMsg->messageCode)
            {
                case FP_MSG_DEVICE_ACTION_RING:
                    datalen = (pMsg->dataLenHighByte<<8)+pMsg->dataLenLowByte;
                    gfps_ring_request_handling(devId, pMsg->data, datalen);
                    break;
                default:
                    break;
            }
            break;
        }

        case FP_MSG_GROUP_SASS:
        {
#if defined(SASS_ENABLED)
            gfps_sass_event_handler(devId, pMsg->messageCode, pMsg->data);
#endif
            break;
        }

        case FP_MSG_GROUP_DEVICE_CAPABLITY_SYNC:
        {
#if defined(SPOT_ENABLED)
            switch(pMsg->messageCode)
            {
                case FP_MSG_DEVICE_CAPABLITY_CAP_UPDATE_REQ:
                    gfps_spot_event_handler(devId);
                    break;
                default:
                    break;
            }
#endif
            break;
        }

        case FP_MSG_GROUP_HEARABLE_CONTROL:
        {
#ifdef HEARABLE_CTRL_ENABLED
            switch(pMsg->messageCode)
            {
                case FP_MSG_HC_GET_ANC_STATE:
                    gfps_notify_hearable_control_state(devId);
                    break;
                case FP_MSG_HC_SET_ANC_STATE:
                    gfps_set_hearable_control_state(devId, pMsg->data);
                    break;
                default:
                    break;
            }
#endif
            break;
        }

        case FP_MSG_GROUP_ACKNOWLEDGEMENT:
            break;

        default:
            msgLen = 0;
            break;
    }
    return msgLen;
}

static void gfps_srv_connect_handler(uint8_t devId, const bt_bdaddr_t *pBtAddr)
{
    gfps_link_connect_process(devId, pBtAddr);

#if defined(BT_SVC_MODULE_IBRT_ENABLED)
    if (BT_IBRT_SLAVE != bta_tws_get_ui_role() && bts_tws_if_get_init_done_state())
#endif
    {
        gfps_send_model_id(devId);
        gfps_send_ble_addr(devId);
        gfps_send_battery_levels(devId);
#if defined(SASS_ENABLED)
        gfps_sass_send_info_to_seeker(devId);
#endif

#if defined(SPOT_ENABLED)
        gfps_send_firmware_version(devId);
        gfps_send_eddystone_identifier_state(devId);
        gfps_spot_event_handler(devId);
#endif

#if defined(BT_SVC_MODULE_IBRT_ENABLED)
        gfps_sync_info();
#endif

#if defined(SASS_ENABLED)
        gfps_sass_exe_pending_switch_media(devId);
#endif
    }
}

static void gfps_srv_disconnect_handler(uint8_t devId)
{
    return;
}

osTimerId gfps_accept_keybase_timeout_timer_id = NULL;
static void gfps_ble_accept_keybase_timeout_handler(void const *param);
osTimerDef (GFPS_ACCEPT_KEYBASE_TIMEOUT, gfps_ble_accept_keybase_timeout_handler);

void gfps_keybase_timer_set()
{
    TRACE(1,"%s,", __func__);
    if (gfps_accept_keybase_timeout_timer_id == NULL)
    {
        gfps_accept_keybase_timeout_timer_id = osTimerCreate(osTimer(GFPS_ACCEPT_KEYBASE_TIMEOUT), osTimerOnce, NULL);
    }
    osTimerStart(gfps_accept_keybase_timeout_timer_id, 60*1000);
}
static void gfps_ble_accept_keybase_timeout_handler(void const *param)
{
    gfpsEnv.JustAcceptRetro = false;
}

bool gfps_get_if_just_accept_retroative_connect()
{
    return gfpsEnv.JustAcceptRetro;
}

void gfps_set_if_just_accept_retroactive(bool value)
{
    gfpsEnv.JustAcceptRetro = value;
}

void gfps_link_connect_process(uint8_t devId, const bt_bdaddr_t *addr)
{
    gfpsEnv.JustAcceptRetro = true;
    gfps_keybase_timer_set();
#ifdef SASS_ENABLED
    if (NULL == gfps_sass_get_connected_dev(devId))
    {
        gfps_sass_connect_handler(devId, (bt_bdaddr_t *)addr);
    }
#endif
}

void gfps_link_disconnect_process(uint8_t devId, const bt_bdaddr_t *addr, uint8_t errCode)
{
    bool isDisconnectedWithMobile = true;

#if defined(BT_SVC_MODULE_IBRT_ENABLED)
    isDisconnectedWithMobile = !gfps_bta_is_tws_addr(addr);
#endif // BT_SVC_MODULE_IBRT_ENABLED

    if (isDisconnectedWithMobile)
    {
        if (gfps_is_last_response_pending())
        {
            gfps_enter_connectable_mode_req_handler(gfps_get_last_response());
        }
    }

#if defined(GFPS_BR_EDR_SEC_CONN_SWITCH_ENABLED)
    // to be optimized
    {
        uint8_t dev_count = gfps_bta_get_bt_connected_link_num();
        GFPS_TRACE(0, "%s: count=%d", __func__, dev_count);
        if (dev_count == 0)
        {
            bt_exec_async_0(false, gfps_restore_sec_conn_host_supp);
        }
    }
#endif  // GFPS_BR_EDR_SEC_CONN_SWITCH_ENABLED

#if defined(SASS_ENABLED)
    gfps_sass_disconnect_handler(devId, addr, errCode);
#endif

    return;
}

void gfps_link_destroy_process(uint8_t devId)
{
#if defined(SASS_ENABLED)
    gfps_sass_check_if_need_reconnect(devId);
#endif
}

void gfps_link_connection_event_process(uint8_t devId, GFPS_CONNECTION_EVENT *pEvent)
{
    if (!pEvent)
    {
        return;
    }

    switch(pEvent->event)
    {
        case GFPS_EVENT_LINK_CONNECTED:
            gfps_link_connect_process(devId, &(pEvent->addr));
            break;
        case GFPS_EVENT_LINK_DISCONNECTED:
            gfps_link_disconnect_process(devId, &(pEvent->addr), pEvent->reason);
            break;
        case GFPS_EVENT_LINK_DESTORY:
            gfps_link_destroy_process(devId);
            break;
        default:
            break;
    }
}

void gfps_link_connect_handler(uint8_t devId, const bt_bdaddr_t *addr)
{
    GFPS_CONNECTION_EVENT pEvent;
    if (addr)
    {
        memcpy(pEvent.addr.address, addr, sizeof(bt_bdaddr_t));
    }
    pEvent.event = GFPS_EVENT_LINK_CONNECTED;
    gfps_mailbox_put(devId, GFPS_EVENT_CONNECTION, (uint8_t *)&pEvent, sizeof(GFPS_CONNECTION_EVENT));
}

void gfps_link_disconnect_handler(uint8_t devId, const bt_bdaddr_t *addr, uint8_t errCode)
{
    GFPS_CONNECTION_EVENT pEvent;
    if (addr)
    {
        memcpy(pEvent.addr.address, addr, sizeof(bt_bdaddr_t));
    }
    else
    {
        memset(pEvent.addr.address, 0, sizeof(bt_bdaddr_t));
    }

    pEvent.reason = errCode;
    pEvent.event = GFPS_EVENT_LINK_DISCONNECTED;
    gfps_mailbox_put(devId, GFPS_EVENT_CONNECTION, (uint8_t *)&pEvent, sizeof(GFPS_CONNECTION_EVENT));
}

void gfps_link_destory_handler(void)
{
    GFPS_CONNECTION_EVENT pEvent;
    pEvent.event = GFPS_EVENT_LINK_DESTORY;
    gfps_mailbox_put(0xFF, GFPS_EVENT_CONNECTION, (uint8_t *)&pEvent, sizeof(GFPS_CONNECTION_EVENT));
}

void gfps_disconnect(uint8_t devId)
{
    if (IS_BT_DEVICE(devId))
    {
        app_fp_disconnect_rfcomm(GET_BT_ID(devId));
    }
#if BLE_AUDIO_ENABLED
    else
    {
        bes_ble_gfps_l2cap_disconnect(devId);
    }
#endif
}

void gfps_srv_event_rfcomm_process(uint8_t devId, GFPS_SRV_EVENT_RFCOM_PARAM_T *param)
{
    uint16_t consumeLen = 0;
    uint16_t dataLen;
    uint8_t *buf;
    GFPS_TRACE(3, "%s, param->event:%d, devId is %d", __func__, param->event, devId);
    switch (param->event)
    {
        case FP_SRV_EVENT_CONNECTED:
            gfps_srv_connect_handler(devId, &(param->p.addr));
            break;

        case FP_SRV_EVENT_DISCONNECTED:
            gfps_srv_disconnect_handler(devId);
            break;

        case FP_SRV_EVENT_RFCOMM_DATA_IND:
            buf = param->p.data.pBuf;
            dataLen = param->p.data.len;
            while (dataLen)
            {
                consumeLen = gfps_data_handler(devId, buf, dataLen);
                if (!consumeLen)
                {
                    break;
                }
                app_fp_rfcomm_data_done(devId, consumeLen, buf, &dataLen);
            }
            break;

        case FP_SRV_EVENT_SENT_DONE:
            break;

        default:
            break;
    }
    return;
}

void gfps_srv_event_l2cap_process(uint8_t devId, GFPS_SRV_EVENT_L2CAP_PARAM_T *param)
{
    uint16_t dataLen;
    uint8_t *buf;
    GFPS_TRACE(3, "%s, param->event:%d, devId is %d", __func__, param->event, devId);
    switch (param->event)
    {
        case FP_SRV_EVENT_CONNECTED:
            gfps_srv_connect_handler(devId, &(param->p.addr));
            break;
    
        case FP_SRV_EVENT_DISCONNECTED:
            gfps_srv_disconnect_handler(devId);
            break;
        case FP_SRV_EVNET_L2CAP_DATA_IND:
            buf = param->p.data.pBuf;
            dataLen = param->p.data.len;
            gfps_data_handler(devId, buf, dataLen);
#if defined(BT_SVC_MODULE_IBRT_ENABLED)
            if(buf[0] == FP_MSG_GROUP_DEVICE_ACTION && buf[1] == FP_MSG_DEVICE_ACTION_RING)
            {
                bta_tws_send_cmd(APP_TWS_CMD_SEND_GFPS_RING_INFO, buf, dataLen);
            }
            break;
    
        case FP_SRV_EVENT_SENT_DONE:
            break;
    
        default:
            break;
    }
    return;
}


uint16_t gfps_event_rfcomm_callback(uint8_t devId, GFPS_SRV_EVENT_RFCOM_PARAM_T *param)
{
    uint16_t ret = 0;
    gfps_mailbox_put(devId, GFPS_EVENT_FROM_RFCOMM_SEEKER, (uint8_t *)param, sizeof(GFPS_SRV_EVENT_RFCOM_PARAM_T));
    return ret;
}

uint16_t gfps_event_l2cap_callback(uint8_t devId, GFPS_SRV_EVENT_L2CAP_PARAM_T *param)
{
    uint16_t ret = 0;
    gfps_mailbox_put(devId, GFPS_EVENT_FROM_L2CAP_SEEKER, (uint8_t *)param, sizeof(GFPS_SRV_EVENT_L2CAP_PARAM_T));
    return ret;
}


bool gfps_event_io_capbility_request_callback(const bt_bdaddr_t *addr, uint8_t local_initiate)
{
    bool io_cap_request = false;
    if(gfps_is_in_subsequent_pair_mode())
    {
        io_cap_request = true;
    }
    GFPS_TRACE(2,"%s, io_cap_request is %d", __func__, io_cap_request);
    return io_cap_request;
}

void gfps_env_init(void)
{
    memset((uint8_t *)&gfpsEnv, 0, sizeof(GFPSEnv_t));
    gfpsEnv.batteryDataType = HIDE_UI_INDICATION;
#if defined(BT_SVC_MODULE_IBRT_ENABLED)
    gfpsEnv.isBatteryInfoIncluded = true;
#else
    gfpsEnv.isBatteryInfoIncluded = false;
#endif
    gfpsEnv.enterPairingMode = gfps_enter_pairing_mode_handler;
    gfpsEnv.btSetIocap = btif_sec_set_io_capabilities;
    gfpsEnv.btSetAuthrequirements = btif_sec_set_authrequirements;

    gfpsEnv.fpCap.env.isCompanionAppInstalled = false;
    gfpsEnv.fpCap.env.isSilentModeSupported   = false;
}

#if defined(SASS_ENABLED)
void gfps_event_bt_profiles_cb(const bt_bdaddr_t *addr, GFPS_PROFILE_ID_E profile, uint8_t event, uint8_t param)
{
    uint8_t device_id = bta_get_device_id_by_addr(addr);
    GFPS_SASS_PROFILE_EVENT pEvent;

    pEvent.pro = profile;
    memcpy(pEvent.btAddr.address, (uint8_t *)addr, sizeof(bt_bdaddr_t));
    pEvent.btEvt = event;
    pEvent.len = 1;
    pEvent.param[0] = param;

    gfps_mailbox_put(SET_BT_ID(device_id), GFPS_EVENT_PROFILE, (uint8_t *)&pEvent, sizeof(GFPS_SASS_PROFILE_EVENT));
}

void gfps_a2dp_connection_state_cb(const bt_bdaddr_t *address, bt_a2dp_conn_state_t state, uint8_t error_code)
{
    gfps_event_bt_profiles_cb(address, GFPS_PROFILE_A2DP, BT_A2DP_SINK_CB_TYPE_CONNECTION, state);
}

void gfps_a2dp_audio_state_cb(const bt_bdaddr_t *address, bt_a2dp_audio_state_t state, uint8_t error_code)
{
    gfps_event_bt_profiles_cb(address, GFPS_PROFILE_A2DP, BT_A2DP_SINK_CB_TYPE_AUDIO_STATE, state);
}

void gfps_hfp_hf_connection_state_cb(const bt_bdaddr_t *address, bt_hfp_conn_state_t state, uint8_t error_code)
{
    gfps_event_bt_profiles_cb(address, GFPS_PROFILE_HFP, BT_HFP_HF_CB_TYPE_CONNECTION_STATE, state);
}

void gfps_hfp_hf_audio_status_cb(const bt_bdaddr_t *address, bt_hfp_audio_state_t state, bt_hfp_audio_codec_t codec, uint8_t error_code)
{
    gfps_event_bt_profiles_cb(address, GFPS_PROFILE_HFP, BT_HFP_HF_CB_TYPE_AUDIO_STATUS, state);
}

void gfps_hfp_hf_call_status_cb(const bt_bdaddr_t *address, bt_hfp_call_state_t call)
{
    gfps_event_bt_profiles_cb(address, GFPS_PROFILE_HFP, BT_HFP_HF_CB_TYPE_CALL_STATUS, call);
}

void gfps_hfp_hf_callsetup_status_cb(const bt_bdaddr_t *address, bt_hfp_callsetup_state_t callsetup)
{
    gfps_event_bt_profiles_cb(address, GFPS_PROFILE_HFP, BT_HFP_HF_CB_TYPE_CALLSETUP_STATUS, callsetup);
}

void gfps_avrcp_connection_state_cb(const bt_bdaddr_t *address, bt_avrcp_conn_state_t state, uint8_t error_code)
{
    gfps_event_bt_profiles_cb(address, GFPS_PROFILE_AVRCP, BT_AVRCP_CB_TYPE_CONNECTION, state);
}

void gfps_avrcp_recv_register_notification_rsp_cb(const bt_bdaddr_t *address, bool changed, const bt_avrcp_notification_params_t *params)
{
    if (params->event == BT_AVRCP_EVENT_PLAY_STATUS_CHANGED)
    {
        gfps_event_bt_profiles_cb(address, GFPS_PROFILE_AVRCP, BT_AVRCP_CB_TYPE_RECV_REGISTER_NTF_RSP, params->params.playback_status);
    }
}

inline void gfps_register_bt_profiles_cb(void)
{
    static bt_a2dp_sink_callbacks_t a2dp_sink_callbacks =
    {
        .connection_state_cb = gfps_a2dp_connection_state_cb,
        .audio_state_cb = gfps_a2dp_audio_state_cb,
    };
    bta_a2dp_register_callbacks(BT_A2DP_SINK_CB_USER_GFPS, &a2dp_sink_callbacks);

    static bt_hfp_hf_callbacks_t hf_callbacks =
    {
        .connection_state_cb = gfps_hfp_hf_connection_state_cb,
        .audio_status_cb = gfps_hfp_hf_audio_status_cb,
        .call_status_cb = gfps_hfp_hf_call_status_cb,
        .callsetup_status_cb = gfps_hfp_hf_callsetup_status_cb,
    };
    bta_hf_register_callbacks(BT_HFP_HF_CB_USER_GFPS, &hf_callbacks);

    static bt_avrcp_callbacks_t avrcp_callbacks =
    {
        .connection_state_cb = gfps_avrcp_connection_state_cb,
        .notification_rsp_cb = gfps_avrcp_recv_register_notification_rsp_cb,
    };
    bta_avrcp_register_callbacks(BT_AVRCP_CB_USER_GFPS, &avrcp_callbacks);
}
#endif

void gfps_init(void)
{
    nv_record_fp_account_key_init();

    gfps_env_init();

    gfps_crypto_init();
        
    gfps_get_key_info();
#ifndef IS_USE_CUSTOM_FP_INFO
    gfps_crypto_set_p256_key(ble_public_key, ble_private_key);
#else
    gfps_crypto_set_p256_key(gfps_get_public_key(),gfps_get_private_key());
#endif

#if defined(SASS_ENABLED)
    gfps_sass_init();
#endif

    gfps_ble_init();

    app_fp_rfcomm_init();

    gfps_thread_init();

#if defined(SASS_ENABLED)
    gfps_register_bt_profiles_cb();
#endif

    gfps_bta_register_iocap_req_cb();

#if BLE_AUDIO_ENABLED
    bta_lea_register_event_callback(gfps_lea_event_handler);
#endif

#if defined(BT_SVC_MODULE_TWS_ENABLED)
    bts_tws_if_add_cmd_table(APP_TWS_CMD_GFPS_USER, ARRAY_SIZE(g_gfps_cmd_handler_table), (const bt_tws_cmd_instance_t *)&g_gfps_cmd_handler_table);
#endif

#if defined(GFPS_BR_EDR_SEC_CONN_SWITCH_ENABLED)
    bes_bt_me_register_sec_conn_callback(gfps_recv_sec_conn_set_cmpl);
#endif
}

#if BLE_AUDIO_ENABLED
void gfps_lea_event_handler(const bt_lea_evt_packet_t *evt_pkt)
{
    uint8_t conidx = 0xFF;
    bool valid = true;
    GFPS_SASS_PROFILE_EVENT pEvent;
    bt_lea_evt_header_t *header = (bt_lea_evt_header_t *)evt_pkt;
#if defined(SASS_ENABLED)
    SassLeaParam *sassParam = (SassLeaParam *)pEvent.param;
#endif
    pEvent.pro = GFPS_PROFILE_LEA;
    ble_bdaddr_t GetPeerAddr = {{0}};

    if (evt_pkt == NULL)
    {
        return;
    }
    GFPS_TRACE(2, "%s type:%d", __func__, header->type);

    switch (header->type)
    {
        case BT_LEA_EVT_MOB_CONNECTION_STATE:
        {
            bt_lea_evt_mob_state_t *connEvt = (bt_lea_evt_mob_state_t *)header;
            valid = false;
            conidx = connEvt->conidx;
            GFPS_TRACE(3, "%s d(%d) connection state:%d", __func__, conidx, connEvt->state.acl_state);
            GFPS_DUMP8("%2x ", connEvt->peer_bdaddr.addr, 6);
            if (connEvt->state.acl_state == BT_LEA_ACL_DISCONNECTED)
            {
                gfps_link_disconnect_handler(conidx, (bt_bdaddr_t *)connEvt->peer_bdaddr.addr, connEvt->state.err_code);
            }
            else if(connEvt->state.acl_state == BT_LEA_ACL_CONNECTED)
            {
                gfps_link_connect_handler(conidx, (bt_bdaddr_t *)connEvt->peer_bdaddr.addr);

                memcpy(pEvent.btAddr.address, (bt_bdaddr_t *)connEvt->peer_bdaddr.addr, sizeof(bt_bdaddr_t));
            }
            else
            {
            }
        }
        break;

#if defined(SASS_ENABLED)
        case BT_LEA_EVT_STREAM_STATUS_CHANGED:
        {
            bt_lea_evt_stream_status_changed_t *streamEvt = (bt_lea_evt_stream_status_changed_t *)header;
            if (streamEvt->curr_state == BT_LEA_ASCS_ASE_STATE_CODEC_CONFIGURED ||
                streamEvt->curr_state == BT_LEA_ASCS_ASE_STATE_QOS_CONFIGURED)
            {
                valid = false;
            }
            else
            {
                pEvent.btEvt = SASS_EVENT_LEA_STREAM_STATE;
                pEvent.len = sizeof(SassLeaParam);
                conidx = streamEvt->con_lid;
                sassParam->conLid = streamEvt->con_lid;
                sassParam->aseLid = streamEvt->ase_lid;
                sassParam->state = streamEvt->curr_state;
            }
        }
        break;

        case BT_LEA_EVT_MCP_MCC_CHAR_VALUE:
        {
            bt_lea_evt_mcp_mcc_char_value_ind_t *pbEvt = (bt_lea_evt_mcp_mcc_char_value_ind_t *)header;

            if (pbEvt->char_type != 12)
            {
                valid = false;
                break;
            }

            uint8_t state = pbEvt->val ? *pbEvt->val : 0;

            GFPS_TRACE(2, "%s playback state:%d", __func__, state);
            if (state == BT_LEA_MCP_PLAYBACK_STATE_PLAYING ||
                state == BT_LEA_MCP_PLAYBACK_STATE_PAUSED ||
                state == BT_LEA_MCP_PLAYBACK_STATE_INACTIVE)
            {
                pEvent.btEvt = SASS_EVENT_LEA_PLAYBACK_STATE;
                pEvent.len = sizeof(SassLeaParam);
                conidx = pbEvt->con_lid;
                sassParam->conLid = pbEvt->con_lid;
                sassParam->state = state;
            }
            else
            {
                 valid = false;
            }
        }
        break;

        case BT_LEA_EVT_CALL_STATE_CHANGE:
        {
            bt_lea_evt_call_state_change_t *pbEvt = (bt_lea_evt_call_state_change_t *)header;
            bts_lea_ccp_single_call_info_t *p_state_ind = (bts_lea_ccp_single_call_info_t *)pbEvt->param;
            pEvent.btEvt = SASS_EVENT_LEA_CALL_STATE;
            pEvent.len = sizeof(SassLeaParam);
            conidx = pbEvt->con_lid;
            sassParam->conLid = pbEvt->con_lid;
            sassParam->state = p_state_ind->state;
        }
        break;

        case BT_LEA_EVT_ASE_METADATA_UPDATE_IND:
        {
            bt_lea_evt_ase_metadata_update_ind_t *pbEvt= (bt_lea_evt_ase_metadata_update_ind_t *)header;

            pEvent.btEvt = SASS_EVENT_LEA_METADATA_UPDATA_STATE;
            pEvent.len = sizeof(SassLeaParam);
            conidx = pbEvt->con_lid;
            sassParam->conLid = pbEvt->con_lid;
            sassParam->state = pbEvt->state;
            sassParam->aseLid = pbEvt->ase_lid;
        }
        break;
#endif

        default:
            valid = false;
            break;
    }

    if (valid)
    {
        app_ble_get_peer_solved_addr(conidx, &GetPeerAddr);
        memcpy(pEvent.btAddr.address, GetPeerAddr.addr, sizeof(GetPeerAddr.addr));
        gfps_mailbox_put(conidx, GFPS_EVENT_PROFILE, (uint8_t *)&pEvent, sizeof(GFPS_SASS_PROFILE_EVENT));
    }

}
#endif

#endif
