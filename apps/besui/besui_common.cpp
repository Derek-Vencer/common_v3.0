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
#include "stdio.h"
#include "cmsis_os.h"
#include "list.h"
#include "string.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_bootmode.h"
#include "pmu.h"
#include "apps.h"
#include "app_thread.h"
#include "app_media_player.h"
#include "app_utils.h"
#include "norflash_api.h"
#include "nvrecord_env.h"
#include "a2dp_decoder.h"
#include "audio_policy.h"
#include "bts_tws_api.h"
#include "bts_core_if.h"
#include "bta_tws_ux_api.h"
#include "bta_bt_api.h"
#include "app_hfp.h"
#include "app_tota_conn.h"

#ifdef GFPS_ENABLED
#include "ble_gfps.h"
#endif

#ifdef ANC_APP
#include "app_anc.h"
#if defined(USER_NOISE_ADAPTIVE_ANC_EN)
#include "app_voice_assist_noise_adapt_anc.h"
#endif
#endif

#ifdef GFPS_ENABLED
#include "gfps.h"
#endif

#ifdef __MOTION_ENGINE_EN__
#include "app_motionhub.h"
#endif

#ifdef DOLBY_AUDIO_ENABLE
#include "lwh.h"
#endif

#ifdef BESUI_APP_EN
#include "app_tota_general.h"
#include "app_tota.h"
#ifdef APP_MULTIPOINT_ONOFF_EN
#ifdef BES_OTA
#include "ota_spp.h"
#endif
#include "app_ble.h"
#endif
#endif

#ifdef BESUI_KEY_EN
#include "twsui_key.h"
#endif
#ifdef BESUI_BTMSG_EN
#include "twsui_btmsg.h"
#endif
#ifdef BESUI_CHARGE_EN
#include "twsui_charge.h"
#endif
#ifdef BESUI_TWS_EN
#include "twsui_comm.h"
#endif

#ifdef BESUI_1WIRE_EN
#include "twsui_uart.h"
#endif

#ifdef ALGO_INFO_SYNC_EN
#include "nvrecord_extension.h"
#ifdef BESUI_STEREO_EN
#include "stereo_prompt.h"
#include "stereo_key.h"
#include "stereo_led.h"
#include "stereoui.h"
#endif
#endif

#ifdef CAPSENSOR_WEAR
#include "app_capsensor.h"
#endif
#include CHIP_SPECIFIC_HDR(charger)

#include "besui_common.h"

type_uictl_t uictl;
#ifdef BESUI_APP_EN
struct nvrecord_env_t *nvrecord_uienv;
#endif

#ifdef IBRT_SEARCH_UI
#define USER_TIMER_CREATE_EN        1  //user timer use
#else
#define USER_TIMER_CREATE_EN        0 
#endif

#if USER_TIMER_CREATE_EN
#define APP_USER_TIMER_PERIODIC_MS (2000)
static osTimerId app_user_timerId = NULL;
static void app_user_timer_handler(void const *param);
void user_timer_onoff(bool timer_en, uint32_t period_ms);
osTimerDef (APP_USER_TIMER_NAME, (void (*)(void const *))app_user_timer_handler);
static void app_user_timer_handler(void const *param) //1s one times
{
	BESUI_TRACE(0, "[UITIMER]%s", __func__);
    // BESUI_TRACE(0, "[UI32K]%s, 32K/2 = %d", __func__, hal_sys_timer_systick_hz());
    // BESUI_TRACE(0, "[ACIN]%s, charger_acin_status_get() = %d", __func__, charger_acin_status_get());
    // user_timer_onoff(true, 2000);

    static uint8_t count = 0;
    if (count == 0)
    {
        count ++;
        besui_search_ui_enter_limiter_mode();
        user_timer_onoff(true, 1000);
    }
    else if (count == 1)
    {
        count = 0;
        // uint8_t *bt_local_addr;
        // bt_local_addr = (uint8_t *)bt_get_local_address();
        // if((bt_local_addr[0]%2) == 0)
        if(besui_get_lr_sta() == LEFT_SIDE)
        {
            besui_search_ui_start_tws_pair();
        }
        user_timer_onoff(false, 1000);
        osTimerDelete(app_user_timerId);
        app_user_timerId = NULL;
    }
}

void user_timer_onoff(bool timer_en, uint32_t period_ms)
{
	BESUI_TRACE(0, "[UITIMER]%s timer_en %d ", __func__, timer_en);

	if(app_user_timerId == NULL)
	{
    	app_user_timerId = osTimerCreate(osTimer(APP_USER_TIMER_NAME),osTimerOnce,NULL);
        if(!app_user_timerId)
             ASSERT(0, "[UITIMER][%s] osTimerCreate error", __func__);
	}

	if(timer_en)
		osTimerStart(app_user_timerId, period_ms);
	else
		osTimerStop(app_user_timerId);
}
#endif //#if USER_TIMER_CREATE_EN


#ifdef USER_APP_BLE_DIS_EN
void app_random_ble_get(void)
{
    if(uictl.random_local[0] != 0 && uictl.random_local[1] != 0)
    {
        BESUI_TRACE(0, "[UIBLE]: %s, random already have!", __func__);
        return;
    }
    uint8_t temp = 0;
    uint8_t tempcnt = 100;
    for(uint8_t i = 0;i < 2;i ++)
    {
        while(tempcnt--)
        {
            temp = rand();
            if(temp != 0x00 && temp != 0xFF)
                break;
        }
        uictl.random_local[i] = temp;
    }
    BESUI_TRACE(0, "[UIBLE]: %s, uictl.random_local=%02X:%02X", __func__,uictl.random_local[0], uictl.random_local[1]);
}

void ble_random_add_adv(uint8_t * buf)
{
    BESUI_TRACE(0, "[UIBLE]%s", __func__);
    buf[0] = 0x03;
    buf[1] = 0xFF;
    buf[2] = uictl.random_local[0];
    buf[3] = uictl.random_local[1];
    DUMP8("%02X ", buf, sizeof(buf));
}
#endif

#ifdef BESUI_APP_EN
bool app_wear_det_onoff_get(void)
{
    return wear_det_onoff_get();
}
#endif

#if defined(USE_ALGO_EN)
#include "app_ibrt_customif_cmd.h"
#if defined(IBRT)
uint8_t ptrParam[15]={0};
#endif
#ifdef BESSPA_ONOFF_EN
extern "C" int32_t audio_process_stereo_surround_onoff(int32_t onoff);
extern "C" int32_t stereo_surround_status;

void besspa_audio_onoff(uint8_t sta, bool param)
{
    nv_record_env_get(&nvrecord_uienv);
    if(param)
    {
#if defined(BESUI_TWS_EN)
        if(BT_IBRT_SLAVE != bts_core_get_ui_role())
        {
            if(sta == 0)
            {
                media_PlayAudio((AUD_ID_ENUM)AUD_ID_SPATIAL_OFF, 0);
            }
            else //if(sta == 1)
            {
                media_PlayAudio((AUD_ID_ENUM)AUD_ID_SPATIAL_ON, 0);                
            }
        }
#elif defined(BESUI_STEREO_EN)
        if(sta == 0)
            app_voice_report(APP_STATUS_INDICATION_EQ_OFF, 0);
        else //if(sta == 1)
            app_voice_report(APP_STATUS_INDICATION_EQ_ON, 0);
#endif
    }

#ifdef A2DP_LDAC_ON
    if(sta)
    {
        if(app_bt_audio_count_streaming_a2dp())
        {
            if(codec_type_stream_get() == USER_CODEC_LDAC)
                app_sysfreq_req(APP_SYSFREQ_USER_APP_0, APP_SYSFREQ_208M);
            // else
            //     app_sysfreq_req(APP_SYSFREQ_USER_APP_0, APP_SYSFREQ_104M);
        }
    }
#endif
    audio_process_stereo_surround_onoff(sta);
    stereo_surround_status = sta;
#ifdef A2DP_LDAC_ON
    if(!sta)
    {
        app_sysfreq_req(APP_SYSFREQ_USER_APP_0, APP_SYSFREQ_32K);
    }
#endif
#if !defined(FREEMAN_ENABLED_STERO)
    if(BT_IBRT_MASTER == bts_core_get_ui_role()) //close eq/mimi/dolby/ceva need notify slave close
    {
        memset(ptrParam, 0, sizeof(ptrParam));
        ptrParam[0] = APP_TOTA_REGULATING_BES_SPATIAL;
        ptrParam[1] = 2-sta;
        ptrParam[2] = 0;
        tws_ctrl_send_cmd(APP_TWS_CMD_SYNC_BESSPA, ptrParam, 3);
    }
#endif
#ifdef BESUI_APP_EN
    app_tota_algo_send_to_app(OP_TOTA_SET_CUSTOMER_CMD, APP_TOTA_REGULATING_BES_SPATIAL, 2-sta, 0, 0, 2);
#endif
    nvrecord_uienv->space_audio_type = stereo_surround_status;
    nv_record_env_set(nvrecord_uienv);

#ifdef BESSPA_EQ_EN
    besui_algo_set_eq(ALGO_ID_BESSPA, sta);
#endif
    BESUI_TRACE(0,"[UISPA][%s] = %d", __func__, stereo_surround_status);  
}
int besspa_audio_onoff_get(void)
{
    return stereo_surround_status;
}
#endif //BESSPA_ONOFF_EN

#if defined(DOLBY_AUDIO_ENABLE)
void dolby_audio_onoff(uint8_t sta)
{
    nv_record_env_get(&nvrecord_uienv);
#if defined(BESUI_TWS_EN) && !defined(ALGO_DELAY_ONOFF_EN)
    if(BT_IBRT_SLAVE != bts_core_get_ui_role())
    {
	    if(sta == 0)
	        media_PlayAudio(AUD_ID_SIGNATURE, 0);
	    else if(sta == 1)
	        media_PlayAudio(AUD_ID_DOLBY, 0);
	}
#endif
    if(sta)
    {
        if(app_bt_audio_count_streaming_a2dp())
        {
            dolby_audio_init(uictl.audio_sample_rate);
        }
    }
    uictl.dolby_onoff_sta = sta;
    dolby_lwh_onoff(uictl.dolby_onoff_sta);
    if(!sta)
    {
        dolby_audio_deinit();
    }
#if !defined(FREEMAN_ENABLED_STERO)
    if(BT_IBRT_MASTER == bts_core_get_ui_role()) //close eq/mimi/dolby/ceva need notify slave close
    {
        memset(ptrParam, 0, sizeof(ptrParam));
        ptrParam[0] = APP_TOTA_REGULATING_DOLBY;
        ptrParam[1] = 2-sta;
        ptrParam[2] = uictl.dolby_mode;
        tws_ctrl_send_cmd(APP_TWS_CMD_SYNC_DOLBY, ptrParam, 3);
    }
#endif
#ifdef BESUI_APP_EN
    app_tota_algo_send_to_app(OP_TOTA_SET_CUSTOMER_CMD, APP_TOTA_REGULATING_DOLBY, 2-sta, uictl.dolby_mode, 0, 3);
#endif
    nvrecord_uienv->dolby_onoff = uictl.dolby_onoff_sta;
    nvrecord_uienv->dolby_mode = uictl.dolby_mode;
    nv_record_env_set(nvrecord_uienv);
    BESUI_TRACE(0,"[DOLBY][%s] = %d", __func__, uictl.dolby_onoff_sta);  
}
uint8_t dolby_audio_onoff_get(void)
{
    return uictl.dolby_onoff_sta;  
}
#endif
#endif //#if defined(USE_ALGO_EN)

#ifdef OTA_BOOT_COPY_EN
void app_ota_boot_copy(uint8_t ota_type)
{
    BESUI_TRACE(0,"[UIOTA][%s]", __func__);
    osDelay(100);
    if(ota_type)
    {
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT); //ota_boot
        // app_otaMode_enter(NULL,NULL);     

        BESUI_TRACE(1,"%s",__func__);

        hal_norflash_disable_protection(HAL_FLASH_ID_0);

        hal_sw_bootmode_set(HAL_SW_BOOTMODE_ENTER_HIDE_BOOT);
    #ifdef __KMATE106__
        app_status_indication_set(APP_STATUS_INDICATION_OTA);
        media_PlayAudio(AUD_ID_BT_WARNING, 0);
        osDelay(1200);
    #endif
    #ifdef SECURE_BOOT_WORKAROUND_SOLUTION
        app_wdt_reopen(10);
        hal_cmu_sys_reboot();
    #else
        pmu_reboot();
    #endif
    }
    else
    {
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT); //single wire mode
        hal_sw_bootmode_set(HAL_SW_BOOTMODE_SINGLE_LINE_DOWNLOAD);
        pmu_reboot();
    }
}
#endif

#ifdef ALGO_INFO_SYNC_EN
extern void bt_audio_updata_eq(uint8_t index);
void algo_send_request(uint32_t message_id, uint32_t param0, uint32_t param1, uint32_t param2, uint32_t param3, uint32_t ptr)
{
    int status;
    APP_MESSAGE_BLOCK msg;

    msg.mod_id = APP_MODUAL_ALGO;
#if defined(USE_BASIC_THREADS)
    msg.mod_level = APP_MOD_LEVEL_1;
#endif
    msg.msg_body.message_id = message_id;           //algo_id
    msg.msg_body.message_Param0 = param0;           //algo_onoff
    msg.msg_body.message_Param1 = param1;           //algo_mode1
    msg.msg_body.message_Param2 = param2;           //algo_mode2
    msg.msg_body.message_Param3 = (float)param3;    //algo_mode3
    msg.msg_body.message_ptr = ptr;                 //algo_voice

    BESUI_TRACE(7,"[UIALGO][%s], %d, %d, %d, %d, %d, %d", __func__, message_id, param0, param1, param2, (uint32_t)param3, ptr);

    status = app_mailbox_put(&msg);
    if(status)
        BESUI_TRACE(0,"[UIALGO]app_mailbox_put error");
    else
        BESUI_TRACE(0,"[UIALGO]app_mailbox_put ok");
}

int algo_msg_process(APP_MESSAGE_BODY *msg_body)
{
    POSSIBLY_UNUSED uint32_t algo_id    = msg_body->message_id;
    POSSIBLY_UNUSED uint32_t algo_onoff = msg_body->message_Param0;
    POSSIBLY_UNUSED uint32_t algo_mode1 = msg_body->message_Param1;
    POSSIBLY_UNUSED uint32_t algo_mode2 = msg_body->message_Param2;
    POSSIBLY_UNUSED uint32_t algo_mode3 = (uint32_t)msg_body->message_Param3;
    POSSIBLY_UNUSED uint32_t algo_voice = msg_body->message_ptr;

    nv_record_env_get(&nvrecord_uienv);

    BESUI_TRACE(7,"[UIALGO][%s] %d, %d, %d, %d, %d, %d", __func__, algo_id, algo_onoff, algo_mode1, algo_mode2, algo_mode3, algo_voice);

//-------------------------------------------------------------------------------------------------------------
#if defined(EQ_SET_CUSTOMER_EN)
    if(algo_id == ALGO_ID_EQ)
    {
#ifdef BESSPA_ONOFF_EN 
        if(algo_onoff == ALGO_ON && besspa_audio_onoff_get())
            besspa_audio_onoff(0, false);
#endif
#if defined(DOLBY_AUDIO_ENABLE)
        if(algo_onoff == ALGO_ON && dolby_audio_onoff_get())
            dolby_audio_onoff(0);
#endif
        if(algo_onoff == ALGO_ON) //open
        {
            nvrecord_uienv->eq_onoff = ALGO_ON;
            nvrecord_uienv->eq_mode = algo_mode1;
            bt_audio_updata_eq(nvrecord_uienv->eq_mode);
        }
        else if(algo_onoff == ALGO_OFF) //close
        {
            nvrecord_uienv->eq_onoff = ALGO_OFF;
            bt_audio_updata_eq(0);
        }
        if(nvrecord_uienv->eq_mode == 0xFF)
            nvrecord_uienv->eq_mode = 0;
        nv_record_env_set(nvrecord_uienv);
        // nv_record_flash_flush();
        return 0;
    }
#endif //#if defined(EQ_SET_CUSTOMER_EN)

//-------------------------------------------------------------------------------------------------------------
#if defined(DOLBY_AUDIO_ENABLE)
    if(algo_id == ALGO_ID_DOLBY)
    {
#if defined(EQ_SET_CUSTOMER_EN)
        if(algo_onoff == ALGO_ON)
        {
            nvrecord_uienv->eq_onoff = ALGO_OFF;
            if(nvrecord_uienv->eq_mode == 0xFF)
                nvrecord_uienv->eq_mode = 0;
            nv_record_env_set(nvrecord_uienv);
            bt_audio_updata_eq(0);
#ifdef BESUI_APP_EN
            app_tota_algo_send_to_app(OP_TOTA_SET_CUSTOMER_CMD, APP_TOTA_REGULATING_EQ, nvrecord_uienv->eq_onoff, nvrecord_uienv->eq_mode, 0, 3);
#endif
#if !defined(FREEMAN_ENABLED_STERO)
            if(BT_IBRT_MASTER == bts_core_get_ui_role()) //close eq/mimi/dolby/ceva need notify slave close
            {
                memset(ptrParam, 0, sizeof(ptrParam));
                ptrParam[0] = APP_TOTA_REGULATING_EQ;
                ptrParam[1] = nvrecord_uienv->eq_onoff;
                ptrParam[2] = nvrecord_uienv->eq_mode;
                tws_ctrl_send_cmd(APP_TWS_CMD_SYNC_EQ, ptrParam, 3);
            }
#endif
        }
#endif
#ifdef BESSPA_ONOFF_EN
        if(algo_onoff == ALGO_ON && besspa_audio_onoff_get())
            besspa_audio_onoff(0, false);
#endif

        if(algo_mode1 != 0xFF)
            uictl.dolby_mode = algo_mode1;
        if(algo_onoff == ALGO_ON)
            dolby_audio_onoff(1);
        else if(algo_onoff == ALGO_OFF)
            dolby_audio_onoff(0);
        if(algo_onoff == ALGO_ON && app_bt_audio_count_streaming_a2dp())
        {
            dolby_mode_set(uictl.dolby_mode);
        }

        nv_record_env_set(nvrecord_uienv);
        // nv_record_flash_flush();
        return 0;
    }
#endif //#if defined(DOLBY_AUDIO_ENABLE)

//-------------------------------------------------------------------------------------------------------------
#if defined(BESSPA_ONOFF_EN)
    if(algo_id == ALGO_ID_BESSPA)
    {
#if defined(EQ_SET_CUSTOMER_EN)
        if(algo_onoff == ALGO_ON)
        {
            nvrecord_uienv->eq_onoff = ALGO_OFF;
            if(nvrecord_uienv->eq_mode == 0xFF)
                nvrecord_uienv->eq_mode = 0;
            nv_record_env_set(nvrecord_uienv);
            bt_audio_updata_eq(0);
#ifdef BESUI_APP_EN
            app_tota_algo_send_to_app(OP_TOTA_SET_CUSTOMER_CMD, APP_TOTA_REGULATING_EQ, nvrecord_uienv->eq_onoff, nvrecord_uienv->eq_mode, 0, 3);
#endif
#if !defined(FREEMAN_ENABLED_STERO)
            if(BT_IBRT_MASTER == bts_core_get_ui_role()) //close eq/mimi/dolby/ceva need notify slave close
            {
                memset(ptrParam, 0, sizeof(ptrParam));
                ptrParam[0] = APP_TOTA_REGULATING_EQ;
                ptrParam[1] = nvrecord_uienv->eq_onoff;
                ptrParam[2] = nvrecord_uienv->eq_mode;
                tws_ctrl_send_cmd(APP_TWS_CMD_SYNC_EQ, ptrParam, 3);
            }
#endif
        }
#endif

#if defined(DOLBY_AUDIO_ENABLE)
        if(algo_onoff == ALGO_ON && dolby_audio_onoff_get())
            dolby_audio_onoff(0);
#endif

        if(algo_onoff == ALGO_ON)
            besspa_audio_onoff(1, true);
        else if(algo_onoff == ALGO_OFF)
            besspa_audio_onoff(0, true);
        nv_record_env_set(nvrecord_uienv);
        // nv_record_flash_flush();
        return 0;
    }
#endif //#if defined(BESSPA_ONOFF_EN)

//-------------------------------------------------------------------------------------------------------------
#if defined(ANC_APP)
    if(algo_id == ALGO_ID_ANC)
    {
        //algo_id algo_onoff algo_mode1 algo_mode2 algo_mode3 algo_voice
//-------------------------------------------------------------------------------------------------------------
#if defined(USER_NOISE_ADAPTIVE_ANC_EN)
        if(algo_onoff != 0x01)
        {
            app_voice_assist_noise_adapt_anc_close();
        }

        if(algo_onoff == 0x01)
        {
            app_voice_assist_noise_adapt_anc_open();
            if(algo_mode1 == 1)
                besui_anc_set_mode(APP_ANC_MODE1);
            else if(algo_mode1 == 2)
                besui_anc_set_mode(APP_ANC_MODE2);
            else //if(algo_mode1 == 3)
                besui_anc_set_mode(APP_ANC_MODE3);
            if(algo_mode2)
                media_PlayAudio(AUD_ID_ANC_ON_MODE, 0);
        }
        else if(algo_onoff == 0x02)
        {
            besui_anc_set_mode(APP_ANC_MODE4); //Ambeint
            media_PlayAudio(AUD_ID_ANC_AA_MODE, 0);
        }
        else if(algo_onoff == 0x04)
        {
            besui_anc_set_mode(APP_ANC_MODE_OFF);
            media_PlayAudio(AUD_ID_SPATIAL_OFF, 0);
        }
        else if(algo_onoff == 0x05)
        {
            besui_anc_set_mode(APP_ANC_MODE1); //anc low
            if(algo_mode2)
                media_PlayAudio(AUD_ID_ANC_ON_MODE, 0);
        }
        else if(algo_onoff == 0x06)
        {
            besui_anc_set_mode(APP_ANC_MODE2); //anc mid
            if(algo_mode2)
                media_PlayAudio(AUD_ID_ANC_ON_MODE, 0);
        }
        else if(algo_onoff == 0x07)
        {
            besui_anc_set_mode(APP_ANC_MODE3); //anc high
            if(algo_mode2)
                media_PlayAudio(AUD_ID_ANC_ON_MODE, 0);
        }

#ifdef BESUI_APP_EN
        app_tota_anc_send_to_app(algo_onoff);
#endif
#ifdef BESUI_TWS_EN
        besui_anc_tws_sync();
#endif
        if(besui_anc_adapt_get_onoff())
            nvrecord_uienv->anc_lmh_mode = 1;
        else if(besui_get_curr_anc_mode() == APP_ANC_MODE1)
            nvrecord_uienv->anc_lmh_mode = 5;
        else if(besui_get_curr_anc_mode() == APP_ANC_MODE2)
            nvrecord_uienv->anc_lmh_mode = 6;
        else if(besui_get_curr_anc_mode() == APP_ANC_MODE3)
            nvrecord_uienv->anc_lmh_mode = 7;
        nv_record_env_set(nvrecord_uienv);
        return 0;
#endif //#if defined(USER_NOISE_ADAPTIVE_ANC_EN)
//-------------------------------------------------------------------------------------------------------------
        if(algo_onoff == 0x01)
        {
            besui_anc_set_mode(APP_ANC_MODE1);
#if defined(BESUI_STEREO_EN)
            app_voice_report(APP_STATUS_INDICATION_ANC_ON,0);
#elif defined(BESUI_TWS_EN)
            media_PlayAudio((AUD_ID_ENUM)AUD_ID_ANC_ON_MODE, 0);
#endif
        }
        else if(algo_onoff == 0x02)
        {
            besui_anc_set_mode(APP_ANC_MODE2);
#if defined(BESUI_STEREO_EN)
            app_voice_report(APP_STATUS_INDICATION_ANC_AMBIENT,0);
#elif defined(BESUI_TWS_EN)
            media_PlayAudio((AUD_ID_ENUM)AUD_ID_ANC_AA_MODE, 0);
#endif
        }
        else if(algo_onoff == 0x04)
        {
            besui_anc_set_mode(APP_ANC_MODE_OFF);
#if defined(BESUI_STEREO_EN)
            app_voice_report(APP_STATUS_INDICATION_ANC_OFF,0);
#elif defined(BESUI_TWS_EN)
            media_PlayAudio((AUD_ID_ENUM)AUD_ID_SPATIAL_OFF, 0);
#endif
        }
#ifdef BESUI_APP_EN
        app_tota_anc_send_to_app(algo_onoff);
#endif
#ifdef BESUI_TWS_EN
        besui_anc_tws_sync();
#endif
    }
#endif //#if defined(ANC_APP)

//-------------------------------------------------------------------------------------------------------------
#ifdef OTA_BOOT_COPY_EN
    if(algo_id == ALGO_ID_OTABOOT)
    {
        BESUI_TRACE(0,"ALGO_ID_OTABOOT");
#ifdef BESUI_KEY_EN
        app_key_gui_to_otaboot_or_single(1, 0);
#else
        app_ota_boot_copy(true);
#endif //#ifdef BESUI_KEY_EN
    }
#endif //#ifdef OTA_BOOT_COPY_EN

//-------------------------------------------------------------------------------------------------------------
#ifdef APP_MULTIPOINT_ONOFF_EN
    if(algo_id == ALGO_ID_MULTIPOINT)
    {
        BESUI_TRACE(0,"[UIAPP]ALGO_ID_MULTIPOINT");
        if(algo_onoff == 0x01) //multipoint on
        {
            bta_tws_set_device_num_max(3, NULL, 0);
        }
        else if(algo_onoff == 0x00) //multipoint off
        {
            uint8_t remote_addr[6] = {0};
            uint8_t spp_remote[6] = {0};
            //uint8_t *ble_remote = NULL;
            ble_bdaddr_t ble_remote = {0};
            spp_remote_addr_read(spp_remote);
            BESUI_TRACE(0,"[UIAPP]spp dev is %02X:%02X:%02X:%02X:%02X:%02X", spp_remote[0], spp_remote[1], spp_remote[2],spp_remote[3],spp_remote[4],spp_remote[5]);
            memcpy(remote_addr, spp_remote, 6);
            for(uint8_t i = 0;i < 2;i ++)
            {
                app_ble_get_peer_solved_addr(i, &ble_remote);
                BESUI_TRACE(0,"[UIAPP]ble dev %d is %02X:%02X:%02X:%02X:%02X:%02X", i,
                ble_remote.addr[0], ble_remote.addr[1], ble_remote.addr[2],ble_remote.addr[3],ble_remote.addr[4],ble_remote.addr[5]);
#if (BTHOST_VERSION ==1)
                if(app_ble_is_remote_mobile_connected(&ble_remote)) //V1
#else
                if(app_ble_is_remote_dev_connected(&ble_remote)) //V3
#endif
                {
                    memcpy(remote_addr, ble_remote.addr, 6);
                }
            }
            BESUI_TRACE(0,"[UIAPP]app dev is %02X:%02X:%02X:%02X:%02X:%02X", remote_addr[0], remote_addr[1], remote_addr[2],remote_addr[3],remote_addr[4],remote_addr[5]);
            //if (!app_ui_change_mode_ext(false, false, NULL)
            bta_tws_set_device_num_max(1, (bt_bdaddr_t *)remote_addr, 0);
        }
    }
#endif //#ifdef APP_MULTIPOINT_ONOFF_EN

//-------------------------------------------------------------------------------------------------------------
#if defined(EQ_CUSTOM_APP_EN)
    if(algo_id == ALGO_ID_EQ_CUSTOM)
    {
#ifdef BESSPA_ONOFF_EN 
        if(besspa_audio_onoff_get())
            besspa_audio_onoff(0, false);
#endif
#if defined(DOLBY_AUDIO_ENABLE)
        if(dolby_audio_onoff_get())
            dolby_audio_onoff(0);
#endif
        if(algo_onoff == ALGO_ON)
        {
            nvrecord_uienv->eq_onoff = ALGO_OFF;
            nvrecord_uienv->eq_mode = algo_mode1;
            bt_audio_set_eq_custom(nvrecord_uienv->eq_custom);
        }
        nv_record_env_set(nvrecord_uienv);
        // nv_record_flash_flush();
        return 0;
    }
#endif //#if defined(EQ_CUSTOM_APP_EN)

//-------------------------------------------------------------------------------------------------------------
    return 0;
}

#ifdef APP_MULTIPOINT_ONOFF_EN
void besui_multipoint_init(void)
{
    if(nvrecord_uienv->muiltipoint_onoff)
    {
        TRACE(0, "[UIMULTI]%s, muiltipoint need on", __func__);
        bta_tws_set_device_num_max(3, NULL, 0);
    }
    else
    {
        TRACE(0, "[UIMULTI]%s, muiltipoint need off", __func__);
        bta_tws_set_device_num_max(1, NULL, 0);
    }
}
#endif

void algo_sync_to_slave(uint8_t param)
{
    BESUI_TRACE(0,"[UIALGO]%s", __func__);

    nv_record_env_get(&nvrecord_uienv);
//-------------------------------------------------------------------------------------------------------------
#if defined(USE_ALGO_EN) && !defined(FREEMAN_ENABLED_STERO)
    if(BT_IBRT_MASTER != bts_core_get_ui_role()) //close eq/mimi/dolby/ceva need notify slave close
    {
		return;
	}
    nv_record_env_get(&nvrecord_uienv);
    memset(ptrParam, 0, sizeof(ptrParam));
#if defined(EQ_SET_CUSTOMER_EN)
	if(param == 1)
	{
        ptrParam[0] = APP_TOTA_REGULATING_EQ;
        ptrParam[1] = nvrecord_uienv->eq_onoff;
        ptrParam[2] = nvrecord_uienv->eq_mode;
        tws_ctrl_send_cmd(APP_TWS_CMD_SYNC_EQ, ptrParam, 3);
#ifdef BESUI_APP_EN
        app_tota_algo_send_to_app(OP_TOTA_SET_CUSTOMER_CMD, APP_TOTA_REGULATING_EQ, nvrecord_uienv->eq_onoff, nvrecord_uienv->eq_mode, 0, 3);
#endif
	}
#endif //#if defined(EQ_SET_CUSTOMER_EN)

//-------------------------------------------------------------------------------------------------------------
#if defined(BESSPA_ONOFF_EN)
    if(param == 2)
    {
        ptrParam[0] = APP_TOTA_REGULATING_BES_SPATIAL;
        ptrParam[1] = 2-stereo_surround_status;
        ptrParam[2] = 0;
        tws_ctrl_send_cmd(APP_TWS_CMD_SYNC_BESSPA, ptrParam, 3);
#ifdef BESUI_APP_EN
        app_tota_algo_send_to_app(OP_TOTA_SET_CUSTOMER_CMD, APP_TOTA_REGULATING_BES_SPATIAL, 2-stereo_surround_status, 0, 0, 2);
#endif
	}
#endif //#if defined(BESSPA_ONOFF_EN)

//-------------------------------------------------------------------------------------------------------------
#if defined(DOLBY_AUDIO_ENABLE)
    if(param == 3)
    {
        ptrParam[0] = APP_TOTA_REGULATING_DOLBY;
        ptrParam[1] = 2-uictl.dolby_onoff_sta;
        ptrParam[2] = uictl.dolby_mode;
        tws_ctrl_send_cmd(APP_TWS_CMD_SYNC_DOLBY, ptrParam, 3);
#ifdef BESUI_APP_EN
        app_tota_algo_send_to_app(OP_TOTA_SET_CUSTOMER_CMD, APP_TOTA_REGULATING_DOLBY, 2-uictl.dolby_onoff_sta, uictl.dolby_mode, 0, 3);
#endif
	}
#endif //#if defined(DOLBY_AUDIO_ENABLE)

//-------------------------------------------------------------------------------------------------------------
#if defined(WEAR_DETECT_PROMPT_EN)
    if(param == 4)
    {
        ptrParam[0] = TWS_SYNC_WEAR_PROMPT;
        ptrParam[1] = nvrecord_uienv->wear_prompt_onoff;
        tws_ctrl_send_cmd(APP_TWS_CMD_CAPSENSOR_WEAR, ptrParam, 2);
	}
#endif

//-------------------------------------------------------------------------------------------------------------
#if defined(EQ_CUSTOM_APP_EN)
    if(param == 5)
    {
        if(nvrecord_uienv->eq_mode == 0xFF)
        {
            ptrParam[0] = APP_TOTA_REGULATING_EQ;//custom_cmd_type;
            ptrParam[1] = nvrecord_uienv->eq_onoff;
            ptrParam[2] = nvrecord_uienv->eq_mode;
            bt_audio_set_eq_custom(nvrecord_uienv->eq_custom);
            memcpy(&ptrParam[3], nvrecord_uienv->eq_custom, EQBAND_NUM);
            tws_ctrl_send_cmd(APP_TWS_CMD_SYNC_EQ, ptrParam, 3+EQBAND_NUM);        
        }
	}
#endif
//-------------------------------------------------------------------------------------------------------------
#endif //#if defined(USE_ALGO_EN) && !defined(FREEMAN_ENABLED_STERO)
}
#endif //#ifdef ALGO_INFO_SYNC_EN

void besui_charger_set_param(bool onoff, uint8_t current)
{
    TRACE(0, "[UICHARGE]%s, onoff = %d, current %d", __func__, onoff, current);

    struct CHARGER_CHARGE_MODULE_CFG_T charge_cfg;

    charger_charge_module_cfg_get(&charge_cfg);

    charge_cfg.prechg_current = BESUI_PRECHG_CURRENT;
    charge_cfg.cc_current     = (enum CHARGER_CHARGE_CONSTANT_CURRENT_E)current;
    charge_cfg.stop_current   = BESUI_STOP_CURRENT;
    charge_cfg.cv_volt        = BESUI_CV_VOLT;
    charge_cfg.rechg_volt     = BESUI_RECHG_VOLT;
    charge_cfg.rechg_en       = 0;
    charge_cfg.chg_en         = onoff;

    charger_charge_module_cfg_set(&charge_cfg);
    charger_charge_module_cfg_get(&charge_cfg);

    TRACE(0, "[UICHARGE]prechg_current = %d", charge_cfg.prechg_current);
    TRACE(0, "[UICHARGE]cc_current     = %d", charge_cfg.cc_current);
    TRACE(0, "[UICHARGE]stop_current   = %d", charge_cfg.stop_current);
    TRACE(0, "[UICHARGE]cv_volt        = %d", charge_cfg.cv_volt);
    TRACE(0, "[UICHARGE]rechg_volt     = %d", charge_cfg.rechg_volt);
    TRACE(0, "[UICHARGE]rechg_en       = %d", charge_cfg.rechg_en);
    TRACE(0, "[UICHARGE]chg_en         = %d", charge_cfg.chg_en);
}

static uint8_t codec_type = 0;
void codec_type_stream_set(uint8_t type)
{
    if(type < USER_CODEC_MAX)
        codec_type = type;
    BESUI_TRACE(0,"[UICODEC][%s] = %d", __func__, type);
}
uint8_t codec_type_stream_get(void)
{
    //BESUI_TRACE(0,"[UICODEC][%s] = %d", __func__, codec_type);
    return codec_type;
}

static uint8_t codec_type1 = USER_CODEC_UNKNOWN;
static uint8_t codec_type2 = USER_CODEC_UNKNOWN;
void codec_type_set(uint8_t id, uint8_t type)
{
    const char *codec_str[] = {
        "AAC",
        "SBC",
        "LHDC",
        "LDAC",
        "UNKNOWN",
    };
    if(id == BT_DEVICE_ID_1)
        codec_type1 = type;
    else if(id == BT_DEVICE_ID_2)
        codec_type2 = type;
    if(codec_type1 > USER_CODEC_MAX || codec_type2 > USER_CODEC_MAX)
        return;
    BESUI_TRACE(0,"[UICODEC][%s] codec_type = %s,%s", __func__, codec_str[codec_type1], codec_str[codec_type2]);
}
uint8_t codec_type_get(uint8_t id)
{
    //BESUI_TRACE(0,"[UICODEC][%s] = %d", __func__, codec_type);
    if(id == BT_DEVICE_ID_1)
        return codec_type1;
    else if(id == BT_DEVICE_ID_2)
        return codec_type2;
    return USER_CODEC_UNKNOWN;
}

#ifdef CODEC_TYPE_APP_EN
void codec_type_to_app(void)
{
    BESUI_TRACE(0,"[UICODEC][%s]", __func__);
#ifdef BESUI_APP_EN
    uint8_t resData[4]={0};
    resData[0] = APP_TOTA_CODEC_TYPE;
#ifdef A2DP_LDAC_ON
    resData[1] = 0x07;
#else
    resData[1] = 0x03;
#endif
                                  //AAC  SBC  LHDC  LDAC  
    const uint8_t codec_flag[] = {0x01, 0x02, 0x08, 0x04, USER_CODEC_UNKNOWN};
    uint8_t codecnum = 0;
    codecnum = codec_type_get(BT_DEVICE_ID_1);
    if(codecnum >= USER_CODEC_MAX)
        codecnum = USER_CODEC_MAX;
    resData[2] = codec_flag[codecnum];
    codecnum = codec_type_get(BT_DEVICE_ID_2);
    if(codecnum >= USER_CODEC_MAX)
        codecnum = USER_CODEC_MAX;
    resData[3] = codec_flag[codecnum];
    BESUI_TRACE(0, "[UIAPP]%s, %02X %02X %02X %02X", __func__, resData[0], resData[1], resData[2], resData[3]);
    app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD, TOTA_NO_ERROR, resData, 4);
#endif
}
#endif //#ifdef CODEC_TYPE_APP_EN


uint8_t besui_get_local_vol(bool param, uint8_t device_id)
{
    TWS_VOLUME_SYNC_INFO_T_V2 volume_info;
    volume_info.a2dp_local_volume = a2dp_volume_local_get(device_id);
    volume_info.hfp_local_volume = hfp_volume_local_get(device_id);
    BESUI_TRACE(0, "[UIVOL]%s, %d, %d", __func__, volume_info.a2dp_local_volume, volume_info.hfp_local_volume);
    if(param)
        return volume_info.a2dp_local_volume;
    else
        return volume_info.hfp_local_volume;
}

#ifdef APP_SYNC_GAME_EN
void app_sync_earbuds_latency(uint8_t param)
{
    uint8_t resData[2]={0};
    resData[0] = APP_TOTA_GAME;
    resData[1] = param;

    BESUI_TRACE(0, "[UIGAME]%s, %02X %02X", __func__, resData[0], resData[1]);
    app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD, TOTA_NO_ERROR, resData, 2);
}
#endif

#ifdef APP_SYNC_VOLUME_EN
void app_sync_earbuds_volume(void)
{
    uint8_t resData[3]={0,0xFF, 0xFF};
    resData[0] = APP_TOTA_VOLUME;
    if(besui_get_profile_conn_num())
        resData[1] = besui_get_local_vol(true, BT_DEVICE_ID_1);
    if(besui_get_profile_conn_num() > 1)
        resData[2] = besui_get_local_vol(true, BT_DEVICE_ID_2);

    BESUI_TRACE(0, "[UIVOL]%s, %02X %02X %02X", __func__, resData[0], resData[1], resData[2]);
    app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD, TOTA_NO_ERROR, resData, 3);
}
#endif

const char *codec_type_stream_get_str(void)
{
    const char *codec_str[] = {
        "AAC",
        "SBC",
        "LHDC",
        "LDAC",
        "UNKNOWN",
    };
    return codec_str[codec_type_stream_get()];
}

const char *cpu_freq_get_str(void)
{
    // uint32_t freq_val = 0;
    // freq_val = hal_sys_timer_calc_cpu_freq(5, 0);
    // //BESUI_TRACE(0,"[UIFREQ][%s] = %d", __func__, freq_val);
    HAL_CMU_FREQ_T freq_cur = HAL_CMU_FREQ_26M;
    freq_cur = hal_sysfreq_get();
    if(freq_cur == HAL_CMU_FREQ_32K)       //0
        return "32K";
    else if(freq_cur == HAL_CMU_FREQ_26M)  //3
        return "26M";
    else if(freq_cur == HAL_CMU_FREQ_52M)  //4
        return "52M";
    else if(freq_cur == HAL_CMU_FREQ_78M)  //5
        return "78M";
    else if(freq_cur == HAL_CMU_FREQ_104M) //6
        return "104M";
    else if(freq_cur == HAL_CMU_FREQ_208M) //7
        return "208M";
    return "26M";
}

bool audio_channel_stereo = false;
void besui_audio_set_channel(bool onoff) //true:stereo
{
    if(onoff)
    {
        a2dp_audio_set_channel_select(A2DP_AUDIO_CHANNEL_SELECT_STEREO);
        BESUI_TRACE(0,"[UIALGO][%s], A2DP_AUDIO_CHANNEL_SELECT_STEREO", __func__);
    }
    else
    {
#ifdef BESUI_TWS_EN
        if(besui_get_lr_sta() == LEFT_SIDE)
        {
            a2dp_audio_set_channel_select(A2DP_AUDIO_CHANNEL_SELECT_LCHNL);  // left
            BESUI_TRACE(0,"[UIALGO][%s], A2DP_AUDIO_CHANNEL_SELECT_LCHNL", __func__);
        }
        else if(besui_get_lr_sta() == RIGHT_SIDE)
        {
            a2dp_audio_set_channel_select(A2DP_AUDIO_CHANNEL_SELECT_RCHNL);  // right
            BESUI_TRACE(0,"[UIALGO][%s], A2DP_AUDIO_CHANNEL_SELECT_RCHNL", __func__);
        }
#else
        if (bts_tws_if_get_nv_role() == BT_IBRT_SLAVE)
            a2dp_audio_set_channel_select(A2DP_AUDIO_CHANNEL_SELECT_LCHNL);  // left
        else
            a2dp_audio_set_channel_select(A2DP_AUDIO_CHANNEL_SELECT_RCHNL);  // right
#endif
    }
    audio_channel_stereo = onoff;
}

bool besui_audio_get_channel(void)
{
    return audio_channel_stereo; //true:stereo
}

void besui_algo_set_eq(uint8_t id, bool sta)
{
    BESUI_TRACE(0, "[UIEQ]%s, id = %d, sta= %d", __func__, id, sta);
#ifdef DOLBY_AUDIO_ENABLE
    if(id == ALGO_ID_DOLBY)
    {
        if(sta)
            bt_audio_updata_eq(7);
        else
            bt_audio_updata_eq(0);
    }
#endif

#ifdef BESSPA_EQ_EN
    if(id == ALGO_ID_BESSPA)
    {
        if(nvrecord_uienv->eq_onoff == ALGO_OFF)
        {
            if(sta)
                bt_audio_updata_eq(7);
            else
                bt_audio_updata_eq(0);
        }
    }
#endif
}

#ifdef BESUI_FIX_GFPS_MODELID_EN
void besui_gfps_set_nv_model_id(void)
{
    BESUI_TRACE(0, "[UIGFPS]%s", __func__);
    nv_record_env_set(nvrecord_uienv);
    nv_record_flash_flush();
}
uint32_t besui_gfps_get_nv_model_id(void)
{
    nv_record_env_get(&nvrecord_uienv);
	return (nvrecord_uienv->gfps_id[0]<<16) | (nvrecord_uienv->gfps_id[1]<<8) | nvrecord_uienv->gfps_id[2];
}
uint8_t *besui_gfps_get_nv_public_key(void)
{
    nv_record_env_get(&nvrecord_uienv);
	return nvrecord_uienv->gfps_public_key;
}
uint8_t *besui_gfps_get_nv_private_key(void)
{
    nv_record_env_get(&nvrecord_uienv);
	return nvrecord_uienv->gfps_private_key;
}
#endif

uint16_t user_volt = 0;
void user_bat_volt_set(uint16_t dat) //10s
{
    user_volt = dat;
}

uint16_t user_bat_volt_get(void)
{
    BESUI_TRACE(0, "[UIBAT]%s, user_volt= %d", __func__, user_volt);
    return user_volt;
}

char *user_bat_volt_get_str(void)
{
    //BESUI_TRACE(0, "[UIBAT]%s, user_volt= %d", __func__, user_volt);
    static char batstr[7] = {0};
    batstr[0] = user_volt/1000+'0';
    batstr[1] = '.';
    batstr[2] = user_volt%1000/100+'0';
    batstr[3] = user_volt%100/10+'0';
    batstr[4] = user_volt%100%10+'0';
    batstr[5] = 'V';
    batstr[6] = '\0';
    return batstr;
}

void besui_algo_mips_trace(uint8_t algo, bool sta)
{
#ifdef USE_ALGO_EN
    #define MIPS_RATE       20
    static uint8_t mips_cnt = 0;
    POSSIBLY_UNUSED static int32_t m_time = 0;
    POSSIBLY_UNUSED static int32_t e_time = 0;

#ifdef DOLBY_AUDIO_ENABLE
    if(algo == ALGO_ID_DOLBY && !dolby_audio_onoff_get())
        return;
#endif
#ifdef BESSPA_ONOFF_EN
    if(algo == ALGO_ID_BESSPA && !besspa_audio_onoff_get())
        return;
#endif

    mips_cnt ++;
    if(sta == false)
    {
        if(mips_cnt == (MIPS_RATE-1))
            m_time = hal_fast_sys_timer_get();
    }
    else
    {
        if(mips_cnt >= MIPS_RATE)
        {
            mips_cnt = 0;
            e_time = hal_fast_sys_timer_get();
#ifdef DOLBY_AUDIO_ENABLE
            if(algo == ALGO_ID_DOLBY && dolby_audio_onoff_get())
                BESUI_TRACE(0, "[DOLBY]%d, mips = %d us", !bts_tws_if_is_local_left_side(), FAST_TICKS_TO_US(e_time - m_time));
#endif
#ifdef BESSPA_ONOFF_EN
            if(algo == ALGO_ID_BESSPA && besspa_audio_onoff_get())
                BESUI_TRACE(0, "[BESSPA]mips = %d us", FAST_TICKS_TO_US(e_time - m_time)); 
#endif
        }
    }
#endif //#ifdef USE_ALGO_EN
}

uint16_t peer_sta = 0;
void user_set_peer_box_sta(uint16_t param)
{
    BESUI_TRACE(0,"[UIBOX][%s] = %d", __func__, param);
    peer_sta = param;
}
uint16_t user_get_peer_box_sta(void)
{
    BESUI_TRACE(0,"[UIBOX][%s] = %d", __func__, peer_sta);
    return peer_sta;
}

uint8_t putinout_sta = 0;
void user_set_putinout_sta(uint8_t param)
{
    BESUI_TRACE(0,"[UIBOX][%s] = %d", __func__, param);
    putinout_sta = param;
}
uint8_t user_get_putinout_sta(void)
{
    BESUI_TRACE(0,"[UIBOX][%s] = %d", __func__, putinout_sta);
    return putinout_sta;
}

bool enc_sta = 0;
void user_set_enc_sta(bool param)
{
    TRACE(0,"[UIENC][%s] = %d", __func__, param);
    enc_sta = param;
}
bool user_get_enc_sta(void)
{
    TRACE(0,"[UIENC][%s] = %d", __func__, enc_sta);
    return enc_sta;
}


uint8_t call_sta = 0;
void user_set_call_sta(uint8_t param)
{
    TRACE(0,"[UIENC][%s] = %d", __func__, param);
    call_sta = param;
}
uint8_t user_get_call_sta(void)
{
    TRACE(0,"[UIENC][%s] = %d", __func__, call_sta);
    return call_sta;
}

bool besui_get_call_ring_sta(void)
{
    bool calllag = false;
    if(app_bt_audio_count_connected_sco() || user_get_enc_sta() || user_get_call_sta())
    {
        calllag = true;
    }
    TRACE(0,"[UIENC][%s] = %d", __func__, calllag);
    return calllag;
}

#if defined(ANC_APP)
const char *user_trace_get_anc_mode(void)
{
    uint8_t param_num = besui_get_curr_anc_mode();

    const char *param_str[] = {
        "APP_ANC_MODE_OFF",
        "APP_ANC_MODE1",
        "APP_ANC_MODE2",
        "APP_ANC_UNKNOWN",
    };
    if(param_num <= APP_ANC_MODE2)
        return param_str[param_num];
    else
        return param_str[3];
}

#if defined(USER_NOISE_ADAPTIVE_ANC_EN)
bool adapt_sta = false;
void besui_anc_adapt_set_onoff(bool param)
{
    adapt_sta = param;
    BESUI_TRACE(0, "[UIADAPT][%s], %d", __func__, adapt_sta);
}
bool besui_anc_adapt_get_onoff(void)
{
    BESUI_TRACE(0, "[UIADAPT][%s], %d", __func__, adapt_sta);

    return adapt_sta;
}
#endif
#endif
void besui_trace_shutdown_type(void) //shutdown call this function
{
    const char *shutdown_str[] = {
        "SHUTDOWN_DEFAULT",
        "SHUTDOWN_TOUCH_PRESS",
        "SHUTDOWN_PAIRMODE_TIMEOUT",
        "SHUTDOWN_RECONNECT_TIMEOUT",
        "SHUTDOWN_CLEAR_PAIR",
        "SHUTDOWN_BAT_CHARGE_FULL",
        "SHUTDOWN_BAT_LOW",
        "SHUTDOWN_NTC_TEMP",
        "SHUTDOWN_FACTORY_RESET",
        "SHUTDOWN_SWITCH_ROLE",
        "SHUTDOWN_BOX_CMD",
        "SHUTDOWN_BOX_RST",
        "SHUTDOWN_ADDR_SWAP_OK",
        "SHUTDOWN_SHIP_MODE",
        "SHUTDOWN_SYNC_PEER",
        "SHUTDOWN_DUT_TIMEOUT",
        "SHUTDOWN_HFP_CMD",
        "SHUTDOWN_LINEIN_INSERT",
        "SHUTDOWN_HFP_RST",

        "SHUTDOWN_MAX",
    };
    BESUI_TRACE(0, "[UISHUTDOWN], [%s]", shutdown_str[uictl.shutdown_type]);
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
uint8_t besui_get_profile_conn_num(void)
{
    uint8_t conn_cnt = 0;
    if(app_bt_get_device(BT_DEVICE_ID_1)->profile_mgr.profile_connected)
        conn_cnt ++;
    if(app_bt_get_device(BT_DEVICE_ID_2)->profile_mgr.profile_connected)
        conn_cnt ++;
    BESUI_TRACE(0,"%s, conn_cnt = %d", __func__, conn_cnt);

    return conn_cnt;
}

bool besui_get_spp_sta(void)
{
    bool sta = spp_connect_sta();
    BESUI_TRACE(0,"%s, sta = %d", __func__, sta);
    
    return sta;
}

bool ble_sta = false;
void besui_set_ble_sta(bool param)
{
    ble_sta = param;
    BESUI_TRACE(0,"%s, ble_sta = %d", __func__, ble_sta);
}

bool besui_get_ble_sta(void)
{
    BESUI_TRACE(0,"%s, ble_sta = %d", __func__, ble_sta);
    return ble_sta;
}

uint8_t phone_devcnt = 0;
void besui_set_nv_paired_dev_count(uint8_t param)
{
    BESUI_TRACE(0,"[UIRECONN]%s, %d", __func__, param);
    phone_devcnt = param;
}

uint8_t besui_get_nv_paired_dev_count(void)
{
    BESUI_TRACE(0,"[UIRECONN]%s, %d", __func__, phone_devcnt);
    return phone_devcnt; //phone
}

#if defined(BESUI_TWS_EN)
void besui_set_addr(uint8_t *local, uint8_t *peer)
{
    memcpy(uictl.local_addr, local, 6);
    memcpy(uictl.peer_addr, peer, 6);
}
void besui_get_addr(void) //uint8_t * buf
{
    memcpy(uictl.local_addr, bt_get_local_address(), 6);
    BESUI_TRACE(0, "[UIVER]local btaddr = %02X:%02X:%02X:%02X:%02X:%02X", uictl.local_addr[0],uictl.local_addr[1], uictl.local_addr[2],
                                                                          uictl.local_addr[3],uictl.local_addr[4],uictl.local_addr[5]);
    BESUI_TRACE(0, "[UIVER]peer btaddr  = %02X:%02X:%02X:%02X:%02X:%02X", uictl.peer_addr[0],uictl.peer_addr[1], uictl.peer_addr[2],
                                                                          uictl.peer_addr[3],uictl.peer_addr[4],uictl.peer_addr[5]);
    BESUI_TRACE(0, "[UIVER]connector addr = %02X:%02X:%02X:%02X:%02X:%02X", nv_record_get_ibrt_peer_addr()[0],nv_record_get_ibrt_peer_addr()[1], nv_record_get_ibrt_peer_addr()[2],
                                                                             nv_record_get_ibrt_peer_addr()[3],nv_record_get_ibrt_peer_addr()[4],nv_record_get_ibrt_peer_addr()[5]);
}
#endif

static const char* box_state2str(bta_tws_box_state_t box_state)
{
    switch (box_state)
    {
        case BTA_TWS_BOX_UNKNOWN:
            return "BOX_UNKNOWN";
        case BTA_TWS_IN_BOX_CLOSED:
            return "IN_BOX_CLOSED";
        case BTA_TWS_IN_BOX_OPEN:
            return "IN_BOX_OPEN";
        case BTA_TWS_OUT_BOX:
            return "OUT_BOX";
        case BTA_TWS_OUT_BOX_WEARED:
            return "OUT_BOX_WEARED";
    }

    return "UNKNOWN_STATE";
}

void besui_system_info_trace(void) //10s
{
    BESUI_TRACE(0, "[UIVER]Start----------------------------------------------------------------");
    BESUI_TRACE(0, "[UIVER]Project Name = %s", BESUI_PROJECT_ID);
    BESUI_TRACE(0, "[UIVER]BT  Name     = %s", BESUI_BT_NAME);
    BESUI_TRACE(0, "[UIVER]BLE Name     = %s", BESUI_BLE_NAME);    
    BESUI_TRACE(0, "[UIVER]SW_VERSION   = V%d.%d.%d", SOFTWARE_VERSION_INFO/100,SOFTWARE_VERSION_INFO/10%10,SOFTWARE_VERSION_INFO%10);
    BESUI_TRACE(0, "[UIVER]Build Date   = %s, %s", __DATE__, __TIME__);
    BESUI_TRACE(0, "[UIVER]Cpu Freq     = %s", cpu_freq_get_str());
    BESUI_TRACE(0, "[UIVER]Cpu Usage AP = %d%%", uictl.usage_ap);
#if defined(CP_IN_SAME_EE) || defined(CHIP_HAS_CP)
    BESUI_TRACE(0, "[UIVER]Cpu Usage CP = %d%%", uictl.usage_cp);
#endif
    BESUI_TRACE(0, "[UIVER]Device Num   = %d", besui_get_profile_conn_num());
    BESUI_TRACE(0, "[UIVER]Audio  Num   = %d", app_bt_audio_count_streaming_a2dp());
    BESUI_TRACE(0, "[UIVER]Sco    Num   = %d", app_bt_audio_count_connected_sco());
    BESUI_TRACE(0, "[UIVER]SPP    Sta   = %d", besui_get_spp_sta());
    BESUI_TRACE(0, "[UIVER]BLE    Sta   = %d", besui_get_ble_sta());
    BESUI_TRACE(0, "[UIVER]Codec Type   = %s", codec_type_stream_get_str());
#if defined(DOLBY_AUDIO_ENABLE)
    BESUI_TRACE(0, "[UIVER]Dolby Algo   = %s", dolby_audio_onoff_get()?"ON":"OFF");
#endif
#if defined(BESSPA_ONOFF_EN)
    BESUI_TRACE(0, "[UIVER]Besspa Algo  = %s", besspa_audio_onoff_get()?"ON":"OFF");
#endif
    BESUI_TRACE(0, "[UIVER]Battery Volt = %s", user_bat_volt_get_str());
#if defined(ANC_APP)
    BESUI_TRACE(0, "[UIVER]ANC mode     = %s", user_trace_get_anc_mode());
#endif
#if defined(CAPSENSOR_WEAR)
    BESUI_TRACE(0, "[UIVER]Wear detect  = [%d]-[%d]", besui_ear_sta_get(false), besui_ear_sta_get(true));
#endif
#if defined(BESUI_TWS_EN)
    BESUI_TRACE(0, "[UIVER]Curr Box Sta = %s",box_state2str(bta_tws_get_box_state(false)));
    BESUI_TRACE(0, "[UIVER]Peer Box Sta = %s",box_state2str((bta_tws_box_state_t)user_get_peer_box_sta()));
    besui_get_addr();
#endif
    BESUI_TRACE(0, "[UIVER]End-----------------------------------------------------------------");
}

#ifdef IBRT
#ifdef BESUI_BTMSG_EN
static void bt_link_state_changed_handler(const bt_bdaddr_t *addr, bta_tws_bt_link_event_t event, bt_ibrt_role_t role, uint8_t reason)
{
	BESUI_TRACE(0, "[UICONN]%s, state = %d, reason = %d", __func__, event, reason);
    uint8_t device_id = BT_DEVICE_NUM;
    uint8_t reconnect_device_id = BT_DEVICE_NUM;

    switch (event)
    {
        case BTA_TWS_BT_DISCONNECTED_EVENT:
            uictl.auth_start_flag = 0;
            if(reason == 0x08)
            {
                device_id = app_bt_compare_connect_addr((uint8_t*)addr);
                if(device_id < BT_DEVICE_NUM)
                {
                    app_bt_clear_mobile_connect_info(device_id);
                    device_id = app_bt_compare_openreconnect_addr((uint8_t*)addr);
                    if(device_id < BT_DEVICE_NUM) //may be disconnected addr is reconnect addr.so clear status
                        app_bt_mobile_clear_openreconnect_info(device_id, false);
                    besui_bt_msg_put(PHONE_DISCONECTED_EVENT, reason, device_id);
                }

                device_id = app_bt_compare_reconnect_addr((uint8_t*)addr);
                if(device_id >= BT_DEVICE_NUM)
                {
                    if(app_bt_get_reconnected_type() == 3) //two device reconnecting
                    {}
                    else if(app_bt_get_reconnected_type() == 2) //device id 1 is reconnecting
                        app_bt_mobile_set_reconnect_info(0, (uint8_t*)addr);
                    else if(app_bt_get_reconnected_type() == 1) //device id 0 is reconnecting
                        app_bt_mobile_set_reconnect_info(1, (uint8_t*)addr);
                    else if(app_bt_get_reconnected_type() == 0)  //no reconnecting
                        app_bt_mobile_set_reconnect_info(0, (uint8_t*)addr);
                }
            }
            else
            {
                device_id = app_bt_compare_connect_addr((uint8_t*)addr);
                BESUI_TRACE(0, "[UIBT]%s, connect device_id=%d", __func__, device_id);
                if(device_id < BT_DEVICE_NUM)
                {
                    app_bt_clear_mobile_connect_info(device_id);
                    device_id = app_bt_compare_openreconnect_addr((uint8_t*)addr);
                    BESUI_TRACE(0, "[UIBT]%s, openreconnect device_id=%d", __func__, device_id);
                    if(device_id < BT_DEVICE_NUM) //may be disconnected addr is reconnect addr.so clear status
                        app_bt_mobile_clear_openreconnect_info(device_id, false);
                    besui_bt_msg_put(PHONE_DISCONECTED_EVENT, reason, device_id);
                }
                else
                {
                    device_id = app_bt_compare_openreconnect_addr((uint8_t*)addr);
                    if(device_id < BT_DEVICE_NUM)
                        besui_bt_msg_put(PHONE_DISCONECTED_EVENT, reason, device_id);
                    //else
                        //besui_bt_msg_put(PHONE_DISCONECTED_EVENT, reason, BT_DEVICE_NUM);
                }
            }
#ifdef BESUI_PROMPT_ISSUE_EN
            prompt_set_flag(false);
#endif
            break;
        case BTA_TWS_BT_CONNECTING_CANCELLED_EVENT:
            uictl.auth_start_flag = 0;
            break;
        case BTA_TWS_BT_CONNECTING_FAILURE_EVENT:
            uictl.auth_start_flag = 0;
            reconnect_device_id = app_bt_compare_openreconnect_addr((uint8_t*)addr);
            if(reconnect_device_id < BT_DEVICE_NUM)
                besui_bt_msg_put(OPENRECONNECTED_TIMEOUT_ENTER_PAIRMODE_EVENT, 0xff, reconnect_device_id);
            else
            {
                reconnect_device_id = app_bt_compare_reconnect_addr((uint8_t*)addr);
                if(reconnect_device_id < BT_DEVICE_NUM)
                    besui_bt_msg_put(RECONNECTED_TIMEOUT_ENTER_PAIRMODE_EVENT, 0xff, reconnect_device_id);
            }
            break;
        case BTA_TWS_BT_CONNECTED_EVENT:
            uictl.auth_start_flag = 0;
#ifdef BESUI_PROMPT_ISSUE_EN
            prompt_issue_timer_onoff(true, 1000); //master
#endif
            break;
        case BTA_TWS_BT_ENCRYPTED_EVENT:
            uictl.auth_start_flag = 0;
            if(reason == 0x00)
            {
                besui_bt_msg_put(AUTH_SUCCESS_EVENT, 0xff, reconnect_device_id);
            }
            break;

        case BTA_TWS_IBRT_DISCONNECTED_EVENT:
#ifdef BESUI_PROMPT_ISSUE_EN
            prompt_set_flag(false);
#endif
        break;
        case BTA_TWS_IBRT_CONNECTED_EVENT:
#ifdef BESUI_PROMPT_ISSUE_EN
            prompt_issue_timer_onoff(true, 1000); //slave
#endif
        break;
        case BTA_TWS_IBRT_ROLE_CHANGED_EVENT:
        break;
        default:
            break;
    }
}

static void tws_link_state_changed_handler(bta_tws_sync_state_t state, uint8_t reason)
{
	BESUI_TRACE(0, "[UICONN]%s, state = %d, reason_code = %d", __func__, state, reason);
    if(state == BTA_TWS_DISCONNECTED || state == BTA_TWS_SYNCING)
    {
        besui_bt_msg_put(TWS_STATUS_EVENT, 0xff, 0);
    }

    switch (state)
    {
        case BTA_TWS_DISCONNECTED:
            besui_bt_msg_put(TWS_DISCONNECTED_EVENT, reason, BT_DEVICE_NUM);
            break;
        case BTA_TWS_SYNCED:
#ifdef USER_TOTA_SPP_SYNC_KEY_EN
            if(bta_tws_get_ui_role() == BT_IBRT_MASTER)
                tota_spp_aeskey_to_slave();
#endif
            besui_bt_msg_put(TWS_CONNECTED_EVENT, 0xff, BT_DEVICE_NUM);
            break;
        default:
            break;
    }
}

static void pairing_mode_changed_handler(bool enabled)
{
    if (enabled)
    {
        besui_bt_msg_put(ENTER_PAIRMODE_EVENT, 0xff, BT_DEVICE_NUM);
    }
}

static void profile_connected_handler(const bt_bdaddr_t *addr)
{
    nv_record_all_ddbrec_print();
    uint8_t device_id = bta_get_device_id_by_addr(addr);
    if(!app_bt_get_mobile_connected_status(device_id))
    {
        app_bt_set_mobile_connected_status(device_id, true);
        app_bt_set_mobile_connected_info(device_id, (uint8_t*)addr);
        besui_bt_msg_put(PHONE_CONNECTED_EVENT, 0xff, device_id);

        uint8_t device_id = app_bt_compare_openreconnect_addr((uint8_t*)addr);
        if(device_id < BT_DEVICE_NUM)
        {
            app_bt_mobile_set_openreconnect_info(device_id, CONNECT_DEFAULT, false);
        }

        device_id = app_bt_compare_reconnect_addr((uint8_t*)addr);
        if(device_id < BT_DEVICE_NUM)
        {
            app_bt_mobile_clear_reconnect_info(device_id);
        }

        if(besui_get_profile_conn_num() >= BT_DEVICE_NUM)
        {
            BESUI_TRACE(0,"connected two device, need clear reconnect flag");
            app_bt_mobile_set_openreconnect_info(device_id, CONNECT_DEFAULT, true);
            app_bt_mobile_clear_reconnect_info(0);
            app_bt_mobile_clear_reconnect_info(1);
        }
    }
}

static void a2dp_connection_state_changed_handler(const bt_bdaddr_t *addr, bt_a2dp_conn_state_t state, uint8_t error_code)
{
    switch (state)
    {
        case BT_A2DP_CONN_STATE_DISCONNECTED:
        break;
        case BT_A2DP_CONN_STATE_CONNECTED:
            profile_connected_handler(addr);
        break;
    }
}

static void hfp_connection_state_changed_handler(const bt_bdaddr_t *addr, bt_hfp_conn_state_t state, uint8_t error_code)
{
    switch (state)
    {
        case BT_HFP_CONN_STATE_DISCONNECTED:
        break;
        case BT_HFP_CONN_STATE_CONNECTED:
            profile_connected_handler(addr);
        break;
    }

}

static void a2dp_audio_state_changed_handler(const bt_bdaddr_t *addr, bt_a2dp_audio_state_t state, uint8_t error_code)
{
    switch (state)
    {
        case BT_A2DP_AUDIO_STATE_CLOSED:
        break;
        case BT_A2DP_AUDIO_STATE_SUSPEND:
#ifdef BESUI_APP_EN
#if defined(VOICE_ASSIST_CUSTOM_LEAK_DETECT)
#ifdef TOTA_v2
            if(app_tota_get_detect_flag())
            {
                app_tota_start_leak_detect();
                app_tota_set_leak_detect_value();
            }
#endif
#endif
#endif
        break;
        case BT_A2DP_AUDIO_STATE_STARTED:
        break;
    }
}
#endif // BESUI_BTMSG_EN

#ifdef BESUI_TWS_EN
static void bt_access_mode_changed_handler(bt_access_mode_t mode)
{
    if(mode == BT_GENERAL_ACCESSIBLE)
    {
        if (bta_tws_get_sync_state() == BTA_TWS_SYNCED)
        {
            besui_bt_msg_put(BT_SINGLE_PAIRMODE, 0xff, 0);
        }
    }
}

static void peer_box_state_changed_handler(bta_tws_box_state_t box_state)
{
    user_set_peer_box_sta(box_state);
}
#endif // BESUI_TWS_EN

static void besui_ibrt_init()
{
#ifdef BESUI_BTMSG_EN
    const bta_tws_bt_link_state_changed_t link_state_changed =
    {
        .bt_link_state_changed = bt_link_state_changed_handler,
    };
    bta_tws_register_bt_link_state_changed_hook(BTA_TWS_UX_USER_INTERNAL_1, &link_state_changed);

    static bt_a2dp_sink_callbacks_t a2dp_sink_callbacks =
    {
        .connection_state_cb = a2dp_connection_state_changed_handler,
        .audio_state_cb = a2dp_audio_state_changed_handler,
    };
    bta_a2dp_register_callbacks(BT_A2DP_SINK_CB_USER_MODULE_1, &a2dp_sink_callbacks);

    static bt_hfp_hf_callbacks_t hfp_hf_callbacks =
    {
        .connection_state_cb = hfp_connection_state_changed_handler,
    };
    bta_hf_register_callbacks(BT_HFP_HF_CB_USER_MODULE_1, &hfp_hf_callbacks);
#endif // BESUI_BTMSG_EN

#ifdef BESUI_TWS_EN
    const bta_tws_bt_callbacks_t bt_callbacks =
    {
        .bt_access_mode_changed = bt_access_mode_changed_handler,
    };
    bta_tws_register_bt_callbacks(BTA_TWS_UX_USER_INTERNAL_1, &bt_callbacks);
#endif // BESUI_TWS_EN

    const bta_tws_ui_state_changed_t ui_state_changed =
    {
#ifdef BESUI_BTMSG_EN
        .tws_sync_state_changed = tws_link_state_changed_handler,
        .pairing_mode_changed = pairing_mode_changed_handler,
#endif // BESUI_BTMSG_EN
#ifdef BESUI_TWS_EN
        .peer_box_state_changed = peer_box_state_changed_handler,
#endif // BESUI_TWS_EN
    };
    bta_tws_register_ui_state_changed_hook(BTA_TWS_UX_USER_INTERNAL_1, &ui_state_changed);
}
#endif // IBRT

void besui_common_init(void)
{
    BESUI_TRACE(0, "%s", __func__);

    media_PlayAudio_locally(AUD_ID_POWER_ON, 0);

    nv_record_env_get(&nvrecord_uienv);

#ifdef BESUI_TWS_EN
#if defined(__VIRTUAL_SURROUND__) || defined(__VIRTUAL_SURROUND_STEREO__)
    besspa_nv_init();
#endif
#endif

#ifdef APP_MULTIPOINT_ONOFF_EN
    besui_multipoint_init();
#endif 

#if USER_TIMER_CREATE_EN
    // if (uicom.poweron_first == true)
    {
        user_timer_onoff(true, 100);
    }
#endif

#ifdef APP_MULTIPOINT_ONOFF_EN
    if(nvrecord_uienv->muiltipoint_onoff == 0)
    {
        BESUI_TRACE(0, "[UIMULTI]%s, muiltipoint need off", __func__);
        bta_tws_set_device_num_max(1, NULL, 0);
    }
#endif 

#ifdef DOLBY_AUDIO_ENABLE
    dolby_lwh_init();
#endif

#ifdef BESUI_STEREO_EN
    if(nvrecord_uienv->game_mode_onoff)
        stereo_game_mode_onoff(true, false);
#endif

#ifdef IBRT
    besui_ibrt_init();
#endif

#ifdef BESUI_NTC_EN
    app_ntc_detect_init(); //tmp test
#endif

#ifdef BESUI_FMD_RTC_EN
    besui_rtc_get_wakeup();
#endif

    uictl.poweron_init_ok = 0xA5;
}

