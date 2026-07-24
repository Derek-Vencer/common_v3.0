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

#ifdef GAF_CODEC_CROSS_CORE
#include "cmsis.h"
#include "cmsis_os.h"
#include "hal_trace.h"
#include "plat_types.h"
#include "gaf_dbg.h"
#include "gaf_cc_api.h"
#include "gaf_cc_off_m55.h"
#include "app_dsp_m55.h"
#include "mcu_dsp_m55_app.h"
#include "cc_stream_common.h"
#include "hal_timer.h"
//#include "smf_api.h"
//#include "ble_audio_earphone_info.h"
#ifdef GAF_CODEC_CROSS_CORE
#include "gaf_media_common.h"
#include "app_rpc_api.h"
#if defined(GAF_DECODER_CROSS_CORE_USE_M55) || defined(GAF_ENCODER_CROSS_CORE_USE_M55)
#include "app_dsp_m55.h"
#ifdef CROSS_CORE_COMMON
#include "app_cross_core.h"
#endif
#include "mcu_dsp_m55_app.h"
#elif defined(GAF_DECODER_CROSS_CORE_USE_HIFI) || defined(GAF_ENCODER_CROSS_CORE_USE_HIFI)
#if defined(BTH_AS_MAIN_MCU)
#include "rpc_bth_dsp.h"
#else
#include "rpc_m55_dsp.h"
#endif
#endif
#endif

#if defined(GAF_DECODER_CROSS_CORE_USE_M55) || defined(GAF_ENCODER_CROSS_CORE_USE_M55)
#define RPC_CROSS_CORE_TASK_CMD_TO_ADD      M55_CORE_BRIDGE_TASK_COMMAND_TO_ADD
#define APP_RPC_CROSS_CORE                  APP_RPC_CORE_BTH_M55
#elif defined(GAF_DECODER_CROSS_CORE_USE_HIFI) || defined(GAF_ENCODER_CROSS_CORE_USE_HIFI)
#if defined(BTH_AS_MAIN_MCU)
#define RPC_CROSS_CORE_TASK_CMD_TO_ADD      RPC_BTH_DSP_TASK_CMD_TO_ADD
#define APP_RPC_CROSS_CORE                  APP_RPC_CORE_BTH_DSP
#else
#define RPC_CROSS_CORE_TASK_CMD_TO_ADD      RPC_M55_DSP_TASK_CMD_TO_ADD
#define APP_RPC_CROSS_CORE                  APP_RPC_CORE_DSP_M55
#endif
#endif


/************************private macro defination***************************/

/************************private struct defination****************************/

/************************private variable defination************************/

/**********************private function declaration*************************/
static void gaf_off_m55_capture_init_transmit(uint8_t* ptr, uint16_t len);
static void gaf_off_m55_capture_init_wait_rsp_timeout_handler(uint8_t*, uint16_t);
static void gaf_off_m55_capture_init_rsp_handler(uint8_t* ptr, uint16_t len);
static void gaf_off_m55_capture_deinit_transmit(uint8_t* ptr, uint16_t len);
static void gaf_off_m55_capture_deinit_wait_rsp_timeout_handler(uint8_t* ptr, uint16_t len);
static void gaf_off_m55_capture_retrigger_handler(uint8_t* param, uint16_t len);
static void gaf_off_m55_capture_deinit_rsp_handler(uint8_t* ptr, uint16_t len);
static void gaf_off_m55_capture_data_notify_transmit(uint8_t* ptr, uint16_t len);

static void gaf_off_m55_playback_init_transmit(uint8_t* ptr, uint16_t len);
static void gaf_off_m55_playback_deinit_transmit(uint8_t* ptr, uint16_t len);
static void gaf_off_m55_playback_deinit_wait_rsp_timeout_handler(uint8_t* ptr, uint16_t len);
static void gaf_off_m55_playback_deinit_rsp_handler(uint8_t* ptr, uint16_t len);
static void gaf_off_m55_playback_retrigger_handler(uint8_t* param, uint16_t len);
static void gaf_off_m55_playback_init_rsp_handler(uint8_t* ptr, uint16_t len);
static void gaf_off_m55_playback_data_notify_transmit(uint8_t* ptr, uint16_t len);
#ifdef ISO_USE_INTERSYS2
static void gaf_off_m55_btc_intersys2_notify_transmit(uint8_t* ptr, uint16_t len);
#endif

RPC_CROSS_CORE_TASK_CMD_TO_ADD(CROSS_CORE_TASK_CMD_GAF_ENCODE_INIT_WAITING_RSP,
                            "CC_GAF_INIT_WAITING_RSP",
                            gaf_off_m55_capture_init_transmit,
                            NULL,
                            5000,
                            gaf_off_m55_capture_init_wait_rsp_timeout_handler,
                            gaf_off_m55_capture_init_rsp_handler,
                            NULL);

RPC_CROSS_CORE_TASK_CMD_TO_ADD(CROSS_CORE_TASK_CMD_GAF_ENCODE_DEINIT_WAITING_RSP,
                            "CC_GAF_ENCODE_DEINIT_WAITING_RSP",
                            gaf_off_m55_capture_deinit_transmit,
                            NULL,
                            1000,
                            gaf_off_m55_capture_deinit_wait_rsp_timeout_handler,
                            gaf_off_m55_capture_deinit_rsp_handler,
                            NULL);

RPC_CROSS_CORE_TASK_CMD_TO_ADD(CROSS_CORE_TASK_CMD_GAF_RETRIGGER_REQ_NO_RSP,
                            "CC_GAF_RETRIIGER_REQ_NO_RSP",
                            NULL,
                            gaf_off_m55_capture_retrigger_handler,
                            0,
                            NULL,
                            NULL,
                            NULL);

#if defined(GAF_DECODER_CROSS_CORE_USE_M55)
M55_CORE_BRIDGE_INSTANT_COMMAND_TO_ADD(CROSS_CORE_INSTANT_CMD_GAF_ENCODE_DATA_NOTIFY,
                            gaf_off_m55_capture_data_notify_transmit,
                            NULL);

#ifdef ISO_USE_INTERSYS2
M55_CORE_BRIDGE_INSTANT_COMMAND_TO_ADD(CROSS_CORE_INSTANT_CMD_GAF_OPEN_BTC_INTERSYS2,
                            gaf_off_m55_btc_intersys2_notify_transmit,
                            NULL);
#endif

M55_CORE_BRIDGE_INSTANT_COMMAND_TO_ADD(CROSS_CORE_INSTANT_CMD_GAF_DECODE_INIT_WAITING_RSP,
                            gaf_off_m55_playback_init_transmit,
                            NULL);

M55_CORE_BRIDGE_INSTANT_COMMAND_TO_ADD(CROSS_CORE_INSTANT_CMD_GAF_DECODE_INIT_RSP_TO_CORE,
                            NULL,
                            gaf_off_m55_playback_init_rsp_handler);

M55_CORE_BRIDGE_INSTANT_COMMAND_TO_ADD(CROSS_CORE_INSTANT_CMD_GAF_DECODE_DATA_NOTIFY,
                            gaf_off_m55_playback_data_notify_transmit,
                            NULL);
#elif defined(GAF_DECODER_CROSS_CORE_USE_HIFI)
RPC_CROSS_CORE_TASK_CMD_TO_ADD(CROSS_CORE_INSTANT_CMD_GAF_ENCODE_DATA_NOTIFY,
                            "CC_GAF_ENCODE_DATA_NOTIFY",
                            gaf_off_m55_capture_data_notify_transmit,
                            NULL,
                            0,
                            NULL,
                            NULL,
                            NULL);

RPC_CROSS_CORE_TASK_CMD_TO_ADD(CROSS_CORE_INSTANT_CMD_GAF_DECODE_INIT_WAITING_RSP,
                            "CC_GAF_DECODE_INIT_WAITING_RSP",
                            gaf_off_m55_playback_init_transmit,
                            NULL,
                            0,
                            NULL,
                            NULL,
                            NULL);

RPC_CROSS_CORE_TASK_CMD_TO_ADD(CROSS_CORE_INSTANT_CMD_GAF_DECODE_INIT_RSP_TO_CORE,
                            "CC_GAF_DECODE_INIT_RSP",
                            NULL,
                            gaf_off_m55_playback_init_rsp_handler,
                            0,
                            NULL,
                            NULL,
                            NULL);

RPC_CROSS_CORE_TASK_CMD_TO_ADD(CROSS_CORE_INSTANT_CMD_GAF_DECODE_DATA_NOTIFY,
                            "CC_GAF_DECODE_DATA_NOTIFY",
                            gaf_off_m55_playback_data_notify_transmit,
                            NULL,
                            0,
                            NULL,
                            NULL,
                            NULL);
#endif

RPC_CROSS_CORE_TASK_CMD_TO_ADD(CROSS_CORE_TASK_CMD_GAF_DECODE_DEINIT_WAITING_RSP,
                            "CC_GAF_DECODE_DEINIT_WAITING_RSP",
                            gaf_off_m55_playback_deinit_transmit,
                            NULL,
                            1000,
                            gaf_off_m55_playback_deinit_wait_rsp_timeout_handler,
                            gaf_off_m55_playback_deinit_rsp_handler,
                            NULL);

RPC_CROSS_CORE_TASK_CMD_TO_ADD(CROSS_CORE_TASK_CMD_GAF_DECODE_RETRIGGER_REQ_NO_RSP,
                            "CC_GAF_RETRIGGER_REQ_NO_RSP",
                            NULL,
                            gaf_off_m55_playback_retrigger_handler,
                            0,
                            NULL,
                            NULL,
                            NULL);

/****************************function defination****************************/

static void gaf_off_m55_capture_feed_init_req(void *initReq)
{
    CC_CAPTURE_INIT_REQ_T *req = (CC_CAPTURE_INIT_REQ_T*)initReq;
    if (req->isBis) {
#ifdef GAF_CODEC_CROSS_CORE
#if defined(DSP_M55) || defined(CP_AS_SUBSYS) || defined(DSP_HIFI4)
#if !defined(GAF_DECODER_CROSS_CORE_USE_HIFI)
        app_cross_core_init(APP_CROSS_CORE_M55_USER_BIS);
#else
        app_cross_core_init(APP_CROSS_CORE_HIFI4_USER_AUDIO_ENCODER);
        int32_t wait_cnt = 0;
        while(app_cross_core_is_running(APP_CROSS_CORE_HIFI4) == false) {
            hal_sys_timer_delay_us(10);
            if (wait_cnt++ > 300000) {    // 3s
                ASSERT(0, "%s: hifi4 is hung", __func__);
            }
        }
#endif
#endif
#endif
    }
    else {
#ifdef GAF_CODEC_CROSS_CORE
#if defined(DSP_M55) || defined(CP_AS_SUBSYS) || defined(DSP_HIFI4)
#if !defined(GAF_DECODER_CROSS_CORE_USE_HIFI)
        app_cross_core_init(APP_CROSS_CORE_M55_USER_AUDIO_ENCODER);
#else
        app_cross_core_init(APP_CROSS_CORE_HIFI4_USER_AUDIO_ENCODER);
        int32_t wait_cnt = 0;
        while(app_cross_core_is_running(APP_CROSS_CORE_HIFI4) == false) {
            hal_sys_timer_delay_us(10);
            if (wait_cnt++ > 300000) {    // 3s
                ASSERT(0, "%s: hifi4 is hung", __func__);
            }
        }
#endif
#endif
#endif
    }
    app_rpc_send_cmd(APP_RPC_CROSS_CORE, CROSS_CORE_TASK_CMD_GAF_ENCODE_INIT_WAITING_RSP, \
                    (uint8_t*)initReq, \
                    sizeof(CC_CAPTURE_INIT_REQ_T));
}

static void gaf_off_m55_capture_init_transmit(uint8_t* ptr, uint16_t len)
{
    app_rpc_send_data_waiting_rsp(APP_RPC_CROSS_CORE, CROSS_CORE_TASK_CMD_GAF_ENCODE_INIT_WAITING_RSP, ptr, len);
}

static void gaf_off_m55_capture_init_wait_rsp_timeout_handler(uint8_t*, uint16_t)
{
    ASSERT(false, "wait for gaf init rsp from off mcu core time out!");
}

static void gaf_off_m55_capture_init_rsp_handler(uint8_t* ptr, uint16_t len)
{
    gaf_cc_capture_init_rsp(ptr);
}

static void gaf_off_m55_capture_deinit_transmit(uint8_t* ptr, uint16_t len)
{
    app_rpc_send_data_waiting_rsp(APP_RPC_CROSS_CORE, CROSS_CORE_TASK_CMD_GAF_ENCODE_DEINIT_WAITING_RSP, ptr, len);
}

static void gaf_off_m55_capture_deinit_handler(void *ptr)
{
    LOG_D("%s", __func__);
    CC_CAPTURE_DEINIT_RSP_T *deinitRsp = (CC_CAPTURE_DEINIT_RSP_T*)ptr;
    if (deinitRsp->closeEncoderCore)
    {
        if (deinitRsp->isBis) {
#ifdef GAF_ENCODER_CROSS_CORE_USE_M55
            app_cross_core_deinit(APP_CROSS_CORE_M55_USER_BIS);
#else
            app_cross_core_deinit(APP_CROSS_CORE_HIFI4_USER_AUDIO_ENCODER);
#endif
        }
        else {
#ifdef GAF_ENCODER_CROSS_CORE_USE_M55
            app_cross_core_deinit(APP_CROSS_CORE_M55_USER_AUDIO_ENCODER);
#else
            app_cross_core_deinit(APP_CROSS_CORE_HIFI4_USER_AUDIO_ENCODER);
#endif
        }
    }
    gaf_cc_capture_deinit_rsp(ptr);
}

static void gaf_off_m55_capture_deinit_wait_rsp_timeout_handler(uint8_t* ptr, uint16_t len)
{
    LOG_I("BTH get gaf_encoder_deinit_rsp from m55 timeout");
    gaf_off_m55_capture_deinit_handler(ptr);
}

static void gaf_off_m55_capture_deinit_rsp_handler(uint8_t* ptr, uint16_t len)
{
    /// bth received m55_deinit_rsp,and to close m55
    LOG_I("BTH gets gaf_encoder_deinit_rsp from m55");
    gaf_off_m55_capture_deinit_handler(ptr);
}

static void gaf_off_m55_capture_retrigger_handler(uint8_t* param, uint16_t len)
{
    // To do: gaf audio media retrigger api
}

static void gaf_off_m55_capture_feed_deinit_req(void *deinitReq)
{
    app_rpc_send_cmd(APP_RPC_CROSS_CORE, CROSS_CORE_TASK_CMD_GAF_ENCODE_DEINIT_WAITING_RSP, \
        (uint8_t*)deinitReq, \
        sizeof(CC_CAPTURE_DEINIT_REQ_T));
}

static void gaf_off_m55_capture_data_notify(void)
{
#ifdef GAF_DECODER_CROSS_CORE_USE_M55
    app_dsp_m55_bridge_send_cmd(CROSS_CORE_INSTANT_CMD_GAF_ENCODE_DATA_NOTIFY, NULL, 0);
#else
    app_rpc_send_data_no_rsp(APP_RPC_CROSS_CORE, CROSS_CORE_INSTANT_CMD_GAF_ENCODE_DATA_NOTIFY, NULL, 0);
#endif
}

static void gaf_off_m55_capture_data_notify_transmit(uint8_t* ptr, uint16_t len)
{
#ifdef GAF_DECODER_CROSS_CORE_USE_M55
    app_dsp_m55_bridge_send_instant_cmd_data(CROSS_CORE_INSTANT_CMD_GAF_ENCODE_DATA_NOTIFY, ptr, len);
#else
    app_rpc_send_data_no_rsp(APP_RPC_CROSS_CORE, CROSS_CORE_INSTANT_CMD_GAF_ENCODE_DATA_NOTIFY, ptr, len);
#endif
}

#ifdef ISO_USE_INTERSYS2
POSSIBLY_UNUSED static void gaf_off_m55_btc_intersys2_notify(void)
{
#ifdef GAF_DECODER_CROSS_CORE_USE_M55
    app_dsp_m55_bridge_send_cmd(CROSS_CORE_INSTANT_CMD_GAF_OPEN_BTC_INTERSYS2, NULL, 0);
#endif
}

POSSIBLY_UNUSED static void gaf_off_m55_btc_intersys2_notify_transmit(uint8_t* ptr, uint16_t len)
{
#ifdef GAF_DECODER_CROSS_CORE_USE_M55
    app_dsp_m55_bridge_send_instant_cmd_data(CROSS_CORE_INSTANT_CMD_GAF_OPEN_BTC_INTERSYS2, ptr, len);
#endif
}
#endif

static void gaf_off_m55_playback_feed_init_req(void *initReq)
{
    CC_PLAYBACK_INIT_REQ_T *req = (CC_PLAYBACK_INIT_REQ_T*)initReq;
    if (req->isBis) {
#ifdef GAF_CODEC_CROSS_CORE
#if defined(DSP_M55) || defined(CP_AS_SUBSYS)
#if !defined(GAF_DECODER_CROSS_CORE_USE_HIFI)
        app_cross_core_init(APP_CROSS_CORE_M55_USER_BIS);
#else
        app_cross_core_init(APP_CROSS_CORE_HIFI4_USER_AUDIO_DECODER);
        int32_t wait_cnt = 0;
        while(app_cross_core_is_running(APP_CROSS_CORE_HIFI4) == false) {
            hal_sys_timer_delay_us(10);
            if (wait_cnt++ > 300000) {    // 3s
                ASSERT(0, "%s: hifi4 is hung", __func__);
            }
        }
#endif
#endif
#endif
    }
    else
    {
#ifdef GAF_CODEC_CROSS_CORE
#if defined(DSP_M55) || defined(CP_AS_SUBSYS)
#if !defined(GAF_DECODER_CROSS_CORE_USE_HIFI)
    app_cross_core_init(APP_CROSS_CORE_M55_USER_AUDIO_DECODER);
#else
    app_cross_core_init(APP_CROSS_CORE_HIFI4_USER_AUDIO_DECODER);
    int32_t wait_cnt = 0;
    while(app_cross_core_is_running(APP_CROSS_CORE_HIFI4) == false) {
        hal_sys_timer_delay_us(10);
        if (wait_cnt++ > 300000) {    // 3s
            ASSERT(0, "%s: hifi4 is hung", __func__);
        }
    }
#endif
#endif
#endif
    }

    app_rpc_send_cmd(APP_RPC_CROSS_CORE, CROSS_CORE_INSTANT_CMD_GAF_DECODE_INIT_WAITING_RSP,
                    (uint8_t*)initReq,
                    sizeof(CC_PLAYBACK_INIT_REQ_T));
}

static void gaf_off_m55_playback_init_transmit(uint8_t* ptr, uint16_t len)
{
#ifdef GAF_DECODER_CROSS_CORE_USE_M55
    app_dsp_m55_bridge_send_instant_cmd_data(CROSS_CORE_INSTANT_CMD_GAF_DECODE_INIT_WAITING_RSP, ptr, len);
#else
    app_rpc_send_data_no_rsp(APP_RPC_CROSS_CORE, CROSS_CORE_INSTANT_CMD_GAF_DECODE_INIT_WAITING_RSP, ptr, len);
#endif
}

static void gaf_off_m55_playback_init_rsp_handler(uint8_t* ptr, uint16_t len)
{
    gaf_cc_playback_init_rsp(ptr);
}

static void gaf_off_m55_playback_deinit_transmit(uint8_t* ptr, uint16_t len)
{
    app_rpc_send_data_waiting_rsp(APP_RPC_CROSS_CORE, CROSS_CORE_TASK_CMD_GAF_DECODE_DEINIT_WAITING_RSP, ptr, len);
}

static void gaf_off_m55_playback_data_notify_transmit(uint8_t* ptr, uint16_t len)
{
#ifdef GAF_DECODER_CROSS_CORE_USE_M55
    app_dsp_m55_bridge_send_instant_cmd_data(CROSS_CORE_INSTANT_CMD_GAF_DECODE_DATA_NOTIFY, ptr, len);
#else
    app_rpc_send_data_no_rsp(APP_RPC_CROSS_CORE, CROSS_CORE_INSTANT_CMD_GAF_DECODE_DATA_NOTIFY, ptr, len);
#endif
}

static void gaf_off_m55_playback_deinit_handler(void *ptr)
{
    LOG_D("%s", __func__);
    CC_PLAYBACK_DEINIT_RSP_T *deinitRsp = (CC_PLAYBACK_DEINIT_RSP_T*)ptr;

    if (deinitRsp->closeDecoderCore)
    {
        if (deinitRsp->isBis) {
#ifdef GAF_DECODER_CROSS_CORE_USE_M55
            app_cross_core_deinit(APP_CROSS_CORE_M55_USER_BIS);
#else
            app_cross_core_deinit(APP_CROSS_CORE_HIFI4_USER_AUDIO_DECODER);
#endif
        }
        else {
#ifdef GAF_DECODER_CROSS_CORE_USE_M55
            app_cross_core_deinit(APP_CROSS_CORE_M55_USER_AUDIO_DECODER);
#else
            app_cross_core_deinit(APP_CROSS_CORE_HIFI4_USER_AUDIO_DECODER);
#endif
        }
    }
    gaf_cc_playback_deinit_rsp(ptr);
}

static void gaf_off_m55_playback_deinit_wait_rsp_timeout_handler(uint8_t* ptr, uint16_t len)
{
    LOG_I("BTH wait for gaf_deinit_rsp from m55 time out!");
    gaf_off_m55_playback_deinit_handler(ptr);
}

static void gaf_off_m55_playback_deinit_rsp_handler(uint8_t* ptr, uint16_t len)
{
    LOG_I("BTH gets gaf_decoder_deinit_rsp from m55");
    gaf_off_m55_playback_deinit_handler(ptr);
}

static void gaf_off_m55_playback_retrigger_handler(uint8_t* param, uint16_t len)
{
    // To do: retrigger audio media
}

static void gaf_off_m55_playback_feed_deinit_req(void *deinitReq)
{
    app_rpc_send_cmd(APP_RPC_CROSS_CORE, CROSS_CORE_TASK_CMD_GAF_DECODE_DEINIT_WAITING_RSP, (uint8_t*)deinitReq, sizeof(CC_PLAYBACK_DEINIT_REQ_T));
}

static void gaf_off_m55_playback_data_notify(void)
{
#ifdef GAF_DECODER_CROSS_CORE_USE_M55
    app_dsp_m55_bridge_send_instant_cmd_data(CROSS_CORE_INSTANT_CMD_GAF_DECODE_DATA_NOTIFY, NULL, 0);
#else
    app_rpc_send_data_no_rsp(APP_RPC_CROSS_CORE, CROSS_CORE_INSTANT_CMD_GAF_DECODE_DATA_NOTIFY, NULL, 0);
#endif
}

static ENCODER_CORE_FUNC_LIST_T gaf_off_m55_capture_func_list =
{
    .encoder_core_init = gaf_off_m55_capture_feed_init_req,
    .encoder_core_deinit = gaf_off_m55_capture_feed_deinit_req,
    .encoder_core_notify = gaf_off_m55_capture_data_notify,
};

static DECODER_CORE_FUNC_LIST_T gaf_off_m55_playback_func_list =
{
    .decoder_core_int = gaf_off_m55_playback_feed_init_req,
    .decoder_core_deinit = gaf_off_m55_playback_feed_deinit_req,
    .decoder_core_notify = gaf_off_m55_playback_data_notify,
};

void gaf_off_m55_capture_update_func_list(void *_func_list)
{
    ENCODER_CORE_FUNC_LIST_T **func_list = (ENCODER_CORE_FUNC_LIST_T**)_func_list;
    *func_list = &gaf_off_m55_capture_func_list;
}

void gaf_off_m55_playback_update_func_list(void *_func_list)
{
    DECODER_CORE_FUNC_LIST_T **func_list = (DECODER_CORE_FUNC_LIST_T**)_func_list;
    *func_list = &gaf_off_m55_playback_func_list;
}

#endif
