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
#if defined(BT_SVC_MODULE_TWS_ENABLED)
#include <stdio.h>
#include <string.h>

#if defined(BES_OTA)
#include "app_ibrt_ota_cmd.h"
#endif

#if defined(__INTERACTION__)
#include "app_ibrt_ota_update.h"
#endif

#if defined(__BT_SYNC__)
#include "app_bt_sync.h"
#endif

#ifdef __GMA_VOICE__
#include "gma_crypto.h"
#endif

#ifdef __BIXBY_VOICE__
#include "app_bixby_thirdparty_if.h"
#endif

#ifdef CUSTOMER_APP_BOAT
#include "app_tota_general.h"
#endif

#include "hal_trace.h"
#include "bts_tws_types.h"
#include "bts_tws_api.h"
#include "audio_prompt_sbc.h"
#include "app_media_player.h"
#include "app_bt_media_manager.h"
#include "app_ibrt_voice_report.h"
#include "a2dp_decoder.h"

void app_ibrt_send_forward_prompt_playing_request_handler(uint8_t *p_buff, uint16_t length);
void app_ibrt_prompt_playing_request_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);

#ifdef __GMA_VOICE__
void app_ibrt_gma_secret_key_sync(uint8_t *p_buff, uint16_t length);
void app_ibrt_gma_secret_key_sync_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
#endif

#if defined(DUAL_MIC_RECORDING) && !defined(SINGLE_DEVICE_REC)
void app_ibrt_tws_audio_send(uint8_t* p_buff, uint16_t length);
void app_ibrt_tws_audio_send_handler(uint16_t rsp_seq, uint8_t* p_buff, uint16_t length);
void app_ibrt_tws_audio_send_done(uint16_t cmdcode, uint16_t rsp_seq, uint8_t* ptrParam, uint16_t paramLen);
void app_ibrt_tws_update_bitrate(uint8_t* p_buff, uint16_t length);
void app_ibrt_tws_update_bitrate_handler(uint16_t rsp_seq, uint8_t* p_buff, uint16_t length);
void app_ibrt_tws_update_bitrate_done(uint16_t cmdcode, uint16_t rsp_seq, uint8_t* ptrParam, uint16_t paramLen);
void app_ibrt_tws_report_buf_lvl(uint8_t* p_buff, uint16_t length);
void app_ibrt_tws_report_buf_lvl_handler(uint16_t rsp_seq, uint8_t* p_buff, uint16_t length);
void app_ibrt_tws_report_buf_lvl_done(uint16_t cmdcode, uint16_t rsp_seq,
                                                  uint8_t *ptrParam, uint16_t paramLen);
void app_ibrt_tws_share_record_info(uint8_t* p_buff, uint16_t length);
void app_ibrt_tws_share_record_info_handler(uint16_t rsp_seq, uint8_t* p_buff, uint16_t length);
void app_ibrt_tws_share_record_info_timeout_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_tws_share_record_info_rsp_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
#endif

#if defined(ANC_APP)
void app_ibrt_sync_anc_status(uint8_t *p_buff, uint16_t length);
void app_ibrt_sync_anc_status_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
#endif

#if defined(PSAP_APP)
void app_ibrt_sync_psap_status(uint8_t *p_buff, uint16_t length);
void app_ibrt_sync_psap_status_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
#endif

#if defined(ANC_ASSIST_ENABLED) && !defined(VOICE_ASSIST_STEREO)
void app_ibrt_sync_anc_assist_status(uint8_t *p_buff, uint16_t length);
void app_ibrt_sync_anc_assist_status_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
#endif

#if defined(__BIXBY_VOICE__)
void app_ibrt_sync_bixby_status(uint8_t *p_buff, uint16_t length);
void app_ibrt_sync_bixby_status_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_sync_bixby_status_rsp_timeout_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_sync_bixby_status_rsp_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
#endif

void app_ibrt_sync_audio_process(uint8_t *p_buff, uint16_t length);
void app_ibrt_sync_audio_process_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);

#ifdef CUSTOMER_APP_BOAT
void app_ibrt_sync_tota_factory_reset(uint8_t *p_buff, uint16_t length);
__attribute__((weak)) void app_tota_sync_factory_reset_handle(uint8_t *key, uint16_t length);
void app_ibrt_sync_tota_factory_reset_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);

void app_ibrt_sync_tota_audio_eq(uint8_t *p_buff, uint16_t length);
void app_ibrt_sync_tota_audio_eq_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_sync_tota_button_settings_control(uint8_t *p_buff, uint16_t length);
void app_ibrt_sync_tota_button_settings_control_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
#endif

#if defined(__BT_SYNC__)
void app_ibrt_set_sync_time(uint8_t *p_buff, uint16_t length);
void app_ibrt_set_sync_time_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_set_sync_time_done(uint16_t cmdcode, uint16_t rsp_seq, uint8_t *ptrParam, uint16_t paramLen);
#endif

void app_ibrt_sync_mix_prompt_req(uint8_t *p_buff, uint16_t length);
void app_ibrt_sync_mix_prompt_hanlder(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);

void app_ibrt_prompt_play_req_hanlder(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_prompt_req(uint8_t *p_buff, uint16_t length);

void app_ibrt_stop_peer_prompt_hanlder(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_stop_peer_prompt_req(uint8_t *p_buff, uint16_t length);

/*
*cmd table for tws exchange
*/
static const bt_tws_cmd_instance_t g_internal_cmd_handler_table[]=
{
    {
        APP_TWS_CMD_SYNC_MIX_PROMPT_REQ,                "SYNC_MIX_PROMPT_REQ",
        app_ibrt_sync_mix_prompt_req,
        app_ibrt_sync_mix_prompt_hanlder,               0,
        app_ibrt_cmd_rsp_timeout_handler_null,          app_ibrt_cmd_rsp_handler_null,
        app_ibrt_cmd_tx_done_handler_null,
        0
    },
    {
        APP_TWS_CMD_STOP_PEER_PROMPT_REQ,               "STOP_PEER_PROMPT_REQ",
        app_ibrt_stop_peer_prompt_req,
        app_ibrt_stop_peer_prompt_hanlder,              0,
        app_ibrt_cmd_rsp_timeout_handler_null,          app_ibrt_cmd_rsp_handler_null,
        app_ibrt_cmd_tx_done_handler_null,
        0
    },
    {
        APP_TWS_CMD_LET_PEER_PLAY_PROMPT,               "LET_PEER_PLAY_PROMPT",
        app_ibrt_prompt_req,
        app_ibrt_prompt_play_req_hanlder,               0,
        app_ibrt_cmd_rsp_timeout_handler_null,          app_ibrt_cmd_rsp_handler_null,
        app_ibrt_cmd_tx_done_handler_null,
        0
    },
    {
        APP_TWS_CMD_SET_SYNC_TIME,                      "SET_SYNC_TIME",
        app_ibrt_set_sync_time,
        app_ibrt_set_sync_time_handler,                 0,
        app_ibrt_cmd_rsp_timeout_handler_null,          app_ibrt_cmd_rsp_handler_null,
        app_ibrt_set_sync_time_done,
        0
    },
    {
        APP_TWS_CMD_FORWARD_PROMPT_PLAYING_REQ,          "FORWARD_PROMPT_PLAYING_REQ",
        app_ibrt_send_forward_prompt_playing_request_handler,
        app_ibrt_prompt_playing_request_handler,         0,
        app_ibrt_cmd_rsp_timeout_handler_null,           app_ibrt_cmd_rsp_handler_null,
        app_ibrt_cmd_tx_done_handler_null,
        0
    },
#ifdef __GMA_VOICE__
    {
        APP_TWS_CMD_GMA_SECRET_KEY,                     "GMA_SECRET_KEY_SYNC",
        app_ibrt_gma_secret_key_sync,
        app_ibrt_gma_secret_key_sync_handler,           0,
        app_ibrt_cmd_rsp_timeout_handler_null,          app_ibrt_cmd_rsp_handler_null,
        app_ibrt_cmd_tx_done_handler_null,
        0
    },
#endif
#if defined(DUAL_MIC_RECORDING) && !defined(SINGLE_DEVICE_REC)
    {
        APP_TWS_CMD_DMA_AUDIO,                          "TWS_CMD_DMA_AUDIO",
        app_ibrt_tws_audio_send,
        app_ibrt_tws_audio_send_handler,               0,
        app_ibrt_cmd_rsp_timeout_handler_null,           app_ibrt_cmd_rsp_handler_null,
        app_ibrt_tws_audio_send_done,
        0
    },
    {
        APP_TWS_CMD_UPDATE_BITRATE,                     "TWS_CMD_UPDATE_BITRATE",
        app_ibrt_tws_update_bitrate,
        app_ibrt_tws_update_bitrate_handler,               0,
        app_ibrt_cmd_rsp_timeout_handler_null,           app_ibrt_cmd_rsp_handler_null,
        app_ibrt_tws_update_bitrate_done,
        0
    },
    {
        APP_TWS_CMD_REPORT_BUF_LVL,                     "TWS_CMD_REPORT_BUF_LVL",
        app_ibrt_tws_report_buf_lvl,
        app_ibrt_tws_report_buf_lvl_handler,               0,
        app_ibrt_cmd_rsp_timeout_handler_null,           app_ibrt_cmd_rsp_handler_null,
        app_ibrt_tws_report_buf_lvl_done,
        0
    },
    {
        APP_TWS_CMD_RECORD_INFO,                        "TWS_CMD_SHARE_RECORD_INFO",
        app_ibrt_tws_share_record_info,
        app_ibrt_tws_share_record_info_handler,            RSP_TIMEOUT_DEFAULT,
        app_ibrt_tws_share_record_info_timeout_handler,    app_ibrt_tws_share_record_info_rsp_handler,
        app_ibrt_cmd_tx_done_handler_null,
        0
    },
#endif
#if defined(ANC_APP)
    {
        APP_TWS_CMD_SYNC_ANC_STATUS,                            "SYNC_ANC_STATUS",
        app_ibrt_sync_anc_status,
        app_ibrt_sync_anc_status_handler,                       0,
        app_ibrt_cmd_rsp_timeout_handler_null,                  app_ibrt_cmd_rsp_handler_null,
        app_ibrt_cmd_tx_done_handler_null,
        0
    },
#endif
#if defined(PSAP_APP)
    {
        APP_TWS_CMD_SYNC_PSAP_STATUS,                           "SYNC_PSAP_STATUS",
        app_ibrt_sync_psap_status,
        app_ibrt_sync_psap_status_handler,                      0,
        app_ibrt_cmd_rsp_timeout_handler_null,                  app_ibrt_cmd_rsp_handler_null,
        app_ibrt_cmd_tx_done_handler_null,
        0
    },
#endif
#if defined(ANC_ASSIST_ENABLED) && !defined(VOICE_ASSIST_STEREO)
    {
        APP_TWS_CMD_SYNC_ANC_ASSIST_STATUS,                     "SYNC_ANC_ASSIST_STATUS",
        app_ibrt_sync_anc_assist_status,
        app_ibrt_sync_anc_assist_status_handler,                0,
        app_ibrt_cmd_rsp_timeout_handler_null,                  app_ibrt_cmd_rsp_handler_null,
        app_ibrt_cmd_tx_done_handler_null,
        0
    },
#endif
    {
        APP_TWS_CMD_SYNC_AUDIO_PROCESS,                         "SYNC_AUDIO_PROCESS",
        app_ibrt_sync_audio_process,
        app_ibrt_sync_audio_process_handler,                    0,
        app_ibrt_cmd_rsp_timeout_handler_null,                  app_ibrt_cmd_rsp_handler_null,
        app_ibrt_cmd_tx_done_handler_null,
        0
    },
#if defined(__BIXBY_VOICE__)
    {
        APP_TWS_CMD_SYNC_BIXBY_STATE,                           "SYNC_BIXBY_STATE",
        app_ibrt_sync_bixby_status,
        app_ibrt_sync_bixby_status_handler,                     RSP_TIMEOUT_FAST_ACK,
        app_ibrt_sync_bixby_status_rsp_timeout_handler,         app_ibrt_sync_bixby_status_rsp_handler,
        app_ibrt_cmd_tx_done_handler_null,
        0
    },
#endif
#ifdef CUSTOMER_APP_BOAT
    {
        APP_TWS_CMD_SYNC_TOTA_FACTORY_RESET,            "SYNC_TOTA_FACTORY_RESET",
        app_ibrt_sync_tota_factory_reset,
        app_ibrt_sync_tota_factory_reset_handler,        0,
        app_ibrt_cmd_rsp_timeout_handler_null,          app_ibrt_cmd_rsp_handler_null,
        app_ibrt_cmd_tx_done_handler_null,
        0
    },
    {
        APP_TWS_CMD_SYNC_TOTA_AUDIO_EQ,                    "SYNC_TOTA_AUDIO_EQ",
        app_ibrt_sync_tota_audio_eq,
        app_ibrt_sync_tota_audio_eq_handler,                0,
        app_ibrt_cmd_rsp_timeout_handler_null,            app_ibrt_cmd_rsp_handler_null,
        app_ibrt_cmd_tx_done_handler_null,
        0
    },
    {
        APP_TWS_CMD_SYNC_TOTA_BUTTON_SETTINGS_CONTROL,      "SYNC_TOTA_BUTTON_SETTINGS_CONTROL",
        app_ibrt_sync_tota_button_settings_control,
        app_ibrt_sync_tota_button_settings_control_handler,                0,
        app_ibrt_cmd_rsp_timeout_handler_null,            app_ibrt_cmd_rsp_handler_null,
        app_ibrt_cmd_tx_done_handler_null,
        0
    },
#endif
//new command pair add here
};

/*****************************************************************************
 Prototype    : app_ibrt_sync_mix_prompt_req
 Description  :
 Input        : uint8_t *p_buff
                uint16_t length
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History      :
 Date         : 2019/11/15
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_sync_mix_prompt_req(uint8_t *p_buff, uint16_t length)
{
    bts_tws_if_send_cmd_without_rsp(APP_TWS_CMD_SYNC_MIX_PROMPT_REQ, p_buff, length);
}
/*****************************************************************************
 Prototype    : app_ibrt_sync_mix_prompt_hanlder
 Description  : When slave receive snoop info,send response to master
 Input        : uint8_t *p_buff
                uint16_t length
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2019/11/15
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_sync_mix_prompt_hanlder(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    app_ibrt_sync_mix_prompt_req_handler(p_buff,length);
}

/*****************************************************************************
 Prototype    : app_ibrt_stop_peer_prompt_req
 Description  :
 Input        : uint8_t *p_buff
                uint16_t length
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History      :
 Date         : 2019/12/4
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_stop_peer_prompt_req(uint8_t *p_buff, uint16_t length)
{
    bts_tws_if_send_cmd_without_rsp(APP_TWS_CMD_STOP_PEER_PROMPT_REQ, p_buff, length);
}
/*****************************************************************************
 Prototype    : app_ibrt_stop_peer_prompt_hanlder
 Description  : Stop the local prompt playing
 Input        : uint8_t *p_buff
                uint16_t length
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2019/12/4
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_stop_peer_prompt_hanlder(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    app_tws_cmd_stop_prompt_handler(p_buff, length);
}

__attribute__((weak)) void app_tws_cmd_stop_prompt_handler(uint8_t* ptrParam, uint16_t paramLen)
{

}


void app_ibrt_send_forward_prompt_playing_request_handler(uint8_t *p_buff, uint16_t length)
{
    bts_tws_if_send_cmd_without_rsp(APP_TWS_CMD_FORWARD_PROMPT_PLAYING_REQ, p_buff, length);
}

void app_ibrt_prompt_playing_request_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    app_tws_voice_prompt_to_play_t *req = (app_tws_voice_prompt_to_play_t*)p_buff;
    // TRACE(0,"(d%d)Get prompt 0x%x playing request from peer dev.", req->dev_id, req->voicePrompt);

#ifdef MEDIA_PLAYER_SUPPORT
    media_PlayAudio((AUD_ID_ENUM)req->voicePrompt, req->dev_id);
#endif
}

/*****************************************************************************
 Prototype    : app_ibrt_sync_mix_prompt_req
 Description  :
 Input        : uint8_t *p_buff
                uint16_t length
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History      :
 Date         : 2019/11/15
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_prompt_req(uint8_t *p_buff, uint16_t length)
{
    voice_report_role_t report_role =  app_ibrt_voice_report_get_role();

    if (report_role == VOICE_REPORT_SLAVE)
    {
        bts_tws_if_send_cmd_without_rsp(APP_TWS_CMD_LET_PEER_PLAY_PROMPT, p_buff, length);
    }
}

/*****************************************************************************
 Prototype    : app_ibrt_voice_prompt_play_req_hanlder
 Description  : Handling function of the voice prompt playing request from peer device.
 Input        : uint8_t *p_buff
                uint16_t length
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2019/12/4
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_prompt_play_req_hanlder(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
#ifdef MEDIA_PLAYER_SUPPORT
    app_tws_cmd_prompt_play_req_handler(p_buff,length);
#endif
}

void app_ibrt_set_sync_time(uint8_t *p_buff, uint16_t length)
{
    bts_tws_if_send_cmd_without_rsp(APP_TWS_CMD_SET_SYNC_TIME, p_buff, length);
}

void app_ibrt_set_sync_time_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    app_bt_sync_tws_cmd_handler(p_buff, length);
}

void app_ibrt_set_sync_time_done(uint16_t cmdcode, uint16_t rsp_seq, uint8_t *ptrParam, uint16_t paramLen)
{
    app_bt_sync_send_tws_cmd_done(ptrParam, paramLen);
}

#ifdef __GMA_VOICE__
void app_ibrt_gma_secret_key_sync(uint8_t *p_buff, uint16_t length)
{
    TRACE(0,"%s", __func__);
    bts_tws_if_send_cmd_without_rsp(APP_TWS_CMD_GMA_SECRET_KEY, p_buff, length);
}
void app_ibrt_gma_secret_key_sync_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    TRACE(0,"%s", __func__);
    gma_set_secret_key((char *)p_buff);
}

#endif

#if defined(DUAL_MIC_RECORDING) && !defined(SINGLE_DEVICE_REC)
void app_ibrt_tws_audio_send(uint8_t* p_buff, uint16_t length)
{
    TRACE(0,"%s", __func__);
    app_recording_send_data_to_master();
}

void app_ibrt_tws_audio_send_handler(uint16_t rsp_seq, uint8_t* p_buff, uint16_t length)
{
    ai_function_handle(CALLBACK_STORE_SLAVE_DATA, (void*)p_buff, length, AI_SPEC_RECORDING, app_ai_get_device_id_from_index(AI_SPEC_RECORDING));
    //TRACE(0,"%s", __func__);
}

void app_ibrt_tws_audio_send_done(uint16_t cmdcode, uint16_t rsp_seq, uint8_t* ptrParam,
        uint16_t paramLen)
{
    TRACE(0,"%s", __func__);
    app_recording_audio_send_done();
}

void app_ibrt_tws_update_bitrate(uint8_t* p_buff, uint16_t length)
{
    TRACE(0,"%s", __func__);
    bts_tws_if_send_cmd_without_rsp(APP_TWS_CMD_UPDATE_BITRATE, p_buff, length);
}

void app_ibrt_tws_update_bitrate_handler(uint16_t rsp_seq, uint8_t* p_buff, uint16_t length)
{
#ifdef VOC_ENCODE_SCALABLE
    SCALABLE_CONFIG_BITRATE_T *info = (SCALABLE_CONFIG_BITRATE_T *)p_buff;
    TRACE(0,"%s bitrate:%d, cursor:%d", __func__, info->bitrate, info->cursor);

    app_recording_update_scalable_bitrate(info->bitrate, info->cursor);
#endif
}

void app_ibrt_tws_update_bitrate_done(uint16_t cmdcode, uint16_t rsp_seq, uint8_t* ptrParam,
        uint16_t paramLen)
{
    TRACE(0,"%s", __func__);
}

void app_ibrt_tws_report_buf_lvl(uint8_t* p_buff, uint16_t length)
{
    bts_tws_if_send_cmd_without_rsp(APP_TWS_CMD_REPORT_BUF_LVL, p_buff, length);
}

void app_ibrt_tws_report_buf_lvl_handler(uint16_t rsp_seq, uint8_t* p_buff, uint16_t length)
{
#ifdef VOC_ENCODE_SCALABLE
    app_recording_on_peer_buf_lvl_received(*p_buff);
#endif
}

void app_ibrt_tws_report_buf_lvl_done(uint16_t cmdcode, uint16_t rsp_seq,
                                                  uint8_t *ptrParam, uint16_t paramLen)
{
    TRACE(0,"%s", __func__);
}

void app_ibrt_tws_share_record_info(uint8_t* p_buff, uint16_t length)
{
    bts_tws_if_send_cmd_with_rsp(APP_TWS_CMD_RECORD_INFO, p_buff, length);
}

void app_ibrt_tws_share_record_info_handler(uint16_t rsp_seq, uint8_t* p_buff, uint16_t length)
{
    app_recording_info_slave_received(rsp_seq, p_buff);
}

void app_ibrt_tws_share_record_info_timeout_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    TRACE(0,"%s", __func__);
}

void app_ibrt_tws_share_record_info_rsp_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    app_recording_info_master_received(p_buff);
}
#endif

#if defined(ANC_APP)
extern "C" int32_t app_anc_tws_sync_change(uint8_t *buf, uint32_t len);

void app_ibrt_sync_anc_status(uint8_t *p_buff, uint16_t length)
{
    bts_tws_if_send_cmd_without_rsp(APP_TWS_CMD_SYNC_ANC_STATUS, p_buff, length);
}

void app_ibrt_sync_anc_status_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    app_anc_tws_sync_change(p_buff, length);
}
#endif

#if defined(PSAP_APP)
void app_ibrt_sync_psap_status(uint8_t *p_buff, uint16_t length)
{
    bts_tws_if_send_cmd_without_rsp(APP_TWS_CMD_SYNC_PSAP_STATUS, p_buff, length);
}

void app_ibrt_sync_psap_status_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    TRACE(0,"[%s] TODO ...", __func__);
}
#endif

#if defined(ANC_ASSIST_ENABLED) && !defined(VOICE_ASSIST_STEREO)
extern "C" int32_t anc_assist_tws_sync_status_change(uint8_t *buf, uint32_t len);

void app_ibrt_sync_anc_assist_status(uint8_t *p_buff, uint16_t length)
{
    bts_tws_if_send_cmd_without_rsp(APP_TWS_CMD_SYNC_ANC_ASSIST_STATUS, p_buff, length);
}

void app_ibrt_sync_anc_assist_status_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    anc_assist_tws_sync_status_change(p_buff, length);
}
#endif

#if defined(__BIXBY_VOICE__)
void app_ibrt_sync_bixby_status(uint8_t *p_buff, uint16_t length)
{
    bts_tws_if_send_cmd_with_rsp(APP_TWS_CMD_SYNC_BIXBY_STATE, p_buff, length);
}

void app_ibrt_sync_bixby_status_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    app_ibrt_bixby_status_send_handle(p_buff, length);
    bts_tws_ctrl_send_rsp(APP_TWS_CMD_SYNC_BIXBY_STATE, rsp_seq, p_buff, length);
}

void app_ibrt_sync_bixby_status_rsp_timeout_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    app_ibrt_bixby_status_send_rsp_timeout_handle(p_buff, length);
}

void app_ibrt_sync_bixby_status_rsp_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    app_ibrt_bixby_status_send_rsp_handle(p_buff, length);
}
#endif

extern "C" int32_t audio_process_tws_sync_change(uint8_t *buf, uint32_t len);

void app_ibrt_sync_audio_process(uint8_t *p_buff, uint16_t length)
{
    bts_tws_if_send_cmd_without_rsp(APP_TWS_CMD_SYNC_AUDIO_PROCESS, p_buff, length);
}

void app_ibrt_sync_audio_process_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    audio_process_tws_sync_change(p_buff, length);
}

#ifdef CUSTOMER_APP_BOAT
void app_ibrt_sync_tota_factory_reset(uint8_t *p_buff, uint16_t length)
{
    bts_tws_if_send_cmd_without_rsp(APP_TWS_CMD_SYNC_TOTA_FACTORY_RESET, p_buff, length);
}

__attribute__((weak)) void app_tota_sync_factory_reset_handle(uint8_t *key, uint16_t length)
{

}

void app_ibrt_sync_tota_factory_reset_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    app_tota_sync_factory_reset_handle(p_buff, length);
}

void app_ibrt_sync_tota_audio_eq(uint8_t *p_buff, uint16_t length)
{
    bts_tws_if_send_cmd_without_rsp(APP_TWS_CMD_SYNC_TOTA_AUDIO_EQ, p_buff, length);
}
void app_ibrt_sync_tota_audio_eq_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    app_tota_sync_audio_eq_handle(p_buff, length);
}
void app_ibrt_sync_tota_button_settings_control(uint8_t *p_buff, uint16_t length)
{
    bts_tws_if_send_cmd_without_rsp(APP_TWS_CMD_SYNC_TOTA_BUTTON_SETTINGS_CONTROL, p_buff, length);
}
void app_ibrt_sync_tota_button_settings_control_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    app_tota_general_button_event_info_set(p_buff, length);
}
#endif

void app_tws_cmd_register_cmd_table(void)
{
    bts_tws_if_add_cmd_table(APP_TWS_CMD_INTERNAL_APP_USER,
                                ARRAY_SIZE(g_internal_cmd_handler_table),
                                (const bt_tws_cmd_instance_t *)&g_internal_cmd_handler_table);

    a2dp_audio_register_cmd_table();

#if defined(BES_OTA)
    app_ibrt_ota_register_cmd_table();
#endif

#ifdef __INTERACTION__
    app_ibrt_ota_update_register_cmd_table();
#endif
}

#endif /* BT_SVC_MODULE_TWS_ENABLED */
