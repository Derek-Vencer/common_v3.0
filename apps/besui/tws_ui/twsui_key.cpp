#ifdef BESUI_TWS_EN

#ifdef BESUI_KEY_EN
#include "twsui_key.h"
#include "app_thread.h"
#include "app_key.h"
#include "app_utils.h"
#include "hal_trace.h"
#include "tgt_hardware.h"
#include "app_tws_ibrt.h"
#include "apps.h"
#include "btapp.h"
#include "hfp_service.h"
#include "app_bt.h"
#include "hal_bootmode.h"
#include "app_factory.h"
#include "hal_norflash.h"
#include "pmu.h"
#include "watchdog.h"
#include "audio_policy.h"
#include "bts_tws_api.h"
#include "bts_core_if.h"
#include "bts_bt_conn.h"
#include "app_ibrt_keyboard.h"
#include "earbud_ux_api.h"
#include "app_ibrt_search_pair_ui.h"

#ifdef BESUI_CHARGE_EN
#include "twsui_charge.h"
#endif
#include "twsui_comm.h"
#include "app_bt_stream.h"
#include "app_bt_media_manager.h"
#include "app_audio_active_device_manager.h"
#if defined(ANC_APP)
#include "app_anc.h"
#endif
#ifdef BESUI_BTMSG_EN
#include "twsui_btmsg.h"
#endif
#include "app_ibrt_customif_cmd.h"
#ifdef BESUI_APP_EN
#include "app_tota_general.h"
#endif

#if defined(OTA_BOOT_SYNC_EN)
#if defined(IBRT_UI)
#include "app_tws_ibrt_ui_test.h"
#include "app_ibrt_tws_ext_cmd.h"
#include "app_tws_ibrt_conn_api.h"
#endif
#endif

#include CHIP_SPECIFIC_HDR(charger)
#ifdef CAPSENSOR_ENABLE
#include "app_capsensor.h"
#endif
#ifdef GFPS_ENABLED
#include "ble_gfps.h"
#include "gfps_ble.h"
#endif
#ifdef SPOT_ENABLED
#include "ble_dult.h"
#endif

#include "nvrecord_env.h"
#include "app_media_player.h"
#include "besui_common.h"

extern HFCALL_MACHINE_ENUM app_get_hfcall_machine(void);
extern uint8_t app_bt_audio_get_curr_a2dp_device(void);

void app_ibrt_besui_key(APP_KEY_STATUS *status, void *param)
{
#ifndef BESUI_KEY_EN
    uint8_t shutdown_key = HAL_KEY_EVENT_LONGLONGPRESS;
#endif
    uint8_t device_id = app_bt_audio_get_device_for_user_action();
    struct BT_DEVICE_T *curr_device = app_bt_get_device(device_id);
    BESUI_TRACE(4, "[UIKEY]%s %d,%d curr_device %x", __func__, status->code, status->event, device_id);


    if(uicom.key_func_onoff)
    {
        BESUI_TRACE(0, "[UIKEY]Disable key function");
        return;
    }

    if(status->event == APP_KEY_EVENT_ON_EAR)
    {
        app_key_gui_post_msg(false, status->code, status->event);
        return;
    }
    else if(status->event == APP_KEY_EVENT_OFF_EAR)
    {
        app_key_gui_post_msg(false, status->code, status->event);
        return;
    }

    //if (BT_IBRT_SLAVE == app_tws_get_ibrt_role(&curr_device->remote) && status->event != shutdown_key)
    // if (BT_IBRT_SLAVE == app_tws_get_ibrt_role(&curr_device->remote) && status->event != HAL_KEY_EVENT_LONGLONGPRESS)
    // if (BT_IBRT_SLAVE == bts_bt_sink_conn_get_ibrt_role(&curr_device->remote) && status->event != HAL_KEY_EVENT_LONGLONGPRESS)
    if(BT_IBRT_SLAVE == bts_core_get_ui_role() && status->event != HAL_KEY_EVENT_DOUBLECLICK_AND_LONGPRESS)
    {
        app_ibrt_keyboard_notify_v2(&curr_device->remote, status, param);
    }
    else
    {
        app_key_gui_post_msg(false, status->code, status->event);
    }

    if(status->event == APP_KEY_EVENT_CLICK)
    {
        media_PlayAudio_single_play(AUD_ID_TAP_SINGLE, 0);
    }
    else if(status->event == APP_KEY_EVENT_DOUBLECLICK)
    {
        media_PlayAudio_single_play(AUD_ID_TAP_DOUBLE, 0);
    }
    else if(status->event == APP_KEY_EVENT_TRIPLECLICK)
    {
        media_PlayAudio_single_play(AUD_ID_TAP_TRIPLE, 0);
    }
}

const APP_KEY_HANDLE  app_ibrt_besui_key_cfg[]
{
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_CLICK},"app_ibrt_ui_test_key", app_ibrt_besui_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_LONGPRESS},"app_ibrt_ui_test_key", app_ibrt_besui_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_LONGLONGPRESS},"app_ibrt_ui_test_key", app_ibrt_besui_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_LONGLONGLONGPRESS},"app_ibrt_ui_test_key", app_ibrt_besui_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_CLICK_AND_LONGPRESS},"app_ibrt_ui_test_key", app_ibrt_besui_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_DOUBLECLICK_AND_LONGPRESS},"app_ibrt_ui_test_key", app_ibrt_besui_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_DOUBLECLICK},"app_ibrt_ui_test_key", app_ibrt_besui_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_TRIPLECLICK},"app_ibrt_ui_test_key", app_ibrt_besui_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_ULTRACLICK},"app_ibrt_ui_test_key", app_ibrt_besui_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_RAMPAGECLICK},"app_ibrt_ui_test_key", app_ibrt_besui_key, NULL},
// #if defined(CAPSENSOR_SLIDE)
//     {{APP_KEY_CODE_PWR,APP_KEY_EVENT_UPSLIDE},"app_ibrt_ui_test_key", app_ibrt_besui_key, NULL},
//     {{APP_KEY_CODE_PWR,APP_KEY_EVENT_DOWNSLIDE},"app_ibrt_ui_test_key", app_ibrt_besui_key, NULL},
// #endif
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_ON_EAR},"app_ibrt_ui_test_key", app_ibrt_besui_key, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_OFF_EAR},"app_ibrt_ui_test_key", app_ibrt_besui_key, NULL},
};

const APP_KEY_HANDLE  app_charge_besui_key_cfg[] //charge key process
{
    // {{APP_KEY_CODE_PWR,APP_KEY_EVENT_CLICK},"app_ibrt_ui_test_key", app_ibrt_besui_key, NULL},
};

void app_ibrt_besui_factory_key(APP_KEY_STATUS *status, void *param)
{
    uictl.poweroff_fast_flag = true;
    app_common_poweroff_process(false, KEY_FUNCTION_POWEROFF);
}

const APP_KEY_HANDLE  app_ibrt_besui_factory_key_cfg[]
{
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_LONGLONGPRESS},"app_ibrt_ui_test_key", app_ibrt_besui_factory_key, NULL},
};


void besui_tws_key_init(void)
{
    BESUI_TRACE(0,"[UIKEY]besui_tws_key_init");
    app_key_handle_clear();

    if(app_current_box_status_open_status())
    {
        BESUI_TRACE(0,"[UIKEY]NORMAL KEY");
        for (uint8_t i=0; i<ARRAY_SIZE(app_ibrt_besui_key_cfg); i++)
        {
            app_key_handle_registration(&app_ibrt_besui_key_cfg[i]);
        }
    }
    else
    {
        if(app_factorymode_get())
        {
            BESUI_TRACE(0,"[UIKEY]FACTORY KEY");
            for (uint8_t i=0; i<ARRAY_SIZE(app_ibrt_besui_factory_key_cfg); i++)
            {
                app_key_handle_registration(&app_ibrt_besui_factory_key_cfg[i]);
            } 
        }
        else
        {
            BESUI_TRACE(0,"[UIKEY]CHARGE KEY");
            for (uint8_t i=0; i<ARRAY_SIZE(app_charge_besui_key_cfg); i++)
            {
                app_key_handle_registration(&app_charge_besui_key_cfg[i]);
            } 
        }
    }
}

static int app_key_gui_handle_process(APP_MESSAGE_BODY *msg_body);
void app_key_gui_modual_init(void)
{
    BESUI_TRACE(0, "[UIKEY][%s]", __func__);
    app_set_threadhandle(APP_MODUAL_KEY_GUI, app_key_gui_handle_process);
}


static int app_key_gui_handle_process(APP_MESSAGE_BODY *msg_body)
{
    bool slave_key_flag = (bool)msg_body->message_Param0;
    uint32_t key_code = msg_body->message_id;
    uint32_t key_event = msg_body->message_Param1;

    if(APP_KEY_CODE_GOOGLE != key_code)
    {
        switch(key_code)
        {
            case APP_KEY_CODE_PWR:
                BESUI_TRACE(0,"[UIKEY]PWR key function");
                app_pwr_key_gui_handler(slave_key_flag, key_event);
                break;

            case APP_KEY_CODE_FN1:
                BESUI_TRACE(0,"[UIKEY]FN1 key function");
                app_fn1_key_gui_handler(slave_key_flag, key_event);
                break;

            default:

                break;
        }
    }

    return 0;
}

void app_key_gui_post_msg(bool slave_key_flag, uint32_t key_code, uint8_t key_event)
{
    BESUI_TRACE(0, "[UIKEY][%s] slave_key_flag: %d, key_code: 0x%08x key_event: %d", __func__, slave_key_flag, key_code, key_event);

    APP_MESSAGE_BLOCK msg;

    msg.mod_id = APP_MODUAL_KEY_GUI;

    msg.msg_body.message_id = key_code;
    msg.msg_body.message_Param0 = slave_key_flag;
    msg.msg_body.message_Param1 = key_event;

    app_mailbox_put(&msg);
}

void app_key_ui_volumeup(void)
{
    app_bt_volumeup();
}

void app_key_ui_volumedown(void)
{
    app_bt_volumedown();
}

//open close siri
a2dp_stream_t * app_bt_get_mobile_a2dp_stream(uint32_t deviceId);
void app_key_ui_close_siri_process(void)
{
	bool en = false;
    static int hf_id = BT_DEVICE_INVALID_ID;
    bt_status_t res = BT_STS_FAILED;
    btif_hf_channel_t* POSSIBLY_UNUSED hf_siri_chnl = NULL;

    if((btif_hf_is_voice_rec_active(app_bt_get_device(BT_DEVICE_ID_1)->hf_channel) == true)
    &&(btif_hf_is_voice_rec_active(app_bt_get_device(BT_DEVICE_ID_2)->hf_channel) == true))
    {
        hf_id = BT_DEVICE_ID_1;
        en = false;
        hf_siri_chnl = app_bt_get_device(BT_DEVICE_ID_1)->hf_channel;

        if((btif_get_hf_chan_state(app_bt_get_device(hf_id)->hf_channel) == BT_HFP_CHAN_STATE_OPEN))
        {
            res = btif_hf_enable_voice_recognition(app_bt_get_device(hf_id)->hf_channel, en);
        }
        BESUI_TRACE(3,"[UISIRI][%s] Line =%d, res = %d", __func__, __LINE__, res);

        hf_id = BT_DEVICE_ID_2;
        en = false;
        hf_siri_chnl = app_bt_get_device(BT_DEVICE_ID_2)->hf_channel;

        if((btif_get_hf_chan_state(app_bt_get_device(hf_id)->hf_channel) == BT_HFP_CHAN_STATE_OPEN))
        {
            res = btif_hf_enable_voice_recognition(app_bt_get_device(hf_id)->hf_channel, en);
        }
        BESUI_TRACE(3,"[UISIRI][%s] Line =%d, res = %d", __func__, __LINE__, res);
    }
    else if(btif_hf_is_voice_rec_active(app_bt_get_device(BT_DEVICE_ID_1)->hf_channel) == true)
    {
        hf_id = BT_DEVICE_ID_1;
        en = false;
        hf_siri_chnl = app_bt_get_device(BT_DEVICE_ID_1)->hf_channel;

        if((btif_get_hf_chan_state(app_bt_get_device(hf_id)->hf_channel) == BT_HFP_CHAN_STATE_OPEN))
        {
            res = btif_hf_enable_voice_recognition(app_bt_get_device(hf_id)->hf_channel, en);
        }
        BESUI_TRACE(3,"[UISIRI][%s] Line =%d, res = %d", __func__, __LINE__, res);
    }
    else if(btif_hf_is_voice_rec_active(app_bt_get_device(BT_DEVICE_ID_2)->hf_channel) == true)
    {
        hf_id = BT_DEVICE_ID_2;
        en = false;
        hf_siri_chnl = app_bt_get_device(BT_DEVICE_ID_2)->hf_channel;

        if((btif_get_hf_chan_state(app_bt_get_device(hf_id)->hf_channel) == BT_HFP_CHAN_STATE_OPEN))
        {
            res = btif_hf_enable_voice_recognition(app_bt_get_device(hf_id)->hf_channel, en);
        }
        BESUI_TRACE(3,"[UISIRI][%s] Line =%d, res = %d", __func__, __LINE__, res);
    }
}

void app_key_ui_open_siri_process(void)
{
    bool en = false;
    static int hf_id = BT_DEVICE_INVALID_ID;
    bt_status_t res = BT_STS_FAILED;
    btif_hf_channel_t* POSSIBLY_UNUSED hf_siri_chnl = NULL;
    uint8_t conn_devices = besui_get_profile_conn_num();

    if((btif_hf_is_voice_rec_active(app_bt_get_device(BT_DEVICE_ID_1)->hf_channel) == false)
	&&(btif_hf_is_voice_rec_active(app_bt_get_device(BT_DEVICE_ID_2)->hf_channel) == false))
    {
        if((btif_a2dp_get_stream_state(app_bt_get_mobile_a2dp_stream(BT_DEVICE_ID_1)) == BT_A2DP_STREAM_STATE_STREAMING)
        &&(btif_a2dp_get_stream_state(app_bt_get_mobile_a2dp_stream(BT_DEVICE_ID_2)) == BT_A2DP_STREAM_STATE_STREAMING))
        {
            en = true;
            hf_id = app_bt_audio_get_curr_a2dp_device();
            hf_siri_chnl = app_bt_get_device(hf_id)->hf_channel;
        }
        else if(btif_a2dp_get_stream_state(app_bt_get_mobile_a2dp_stream(BT_DEVICE_ID_1)) == BT_A2DP_STREAM_STATE_STREAMING)
        {
            en = true;
            hf_id = BT_DEVICE_ID_1;
            hf_siri_chnl = app_bt_get_device(BT_DEVICE_ID_1)->hf_channel;
        }
        else if(btif_a2dp_get_stream_state(app_bt_get_mobile_a2dp_stream(BT_DEVICE_ID_2)) == BT_A2DP_STREAM_STATE_STREAMING)
        {
            en = true;
            hf_id = BT_DEVICE_ID_2;
            hf_siri_chnl = app_bt_get_device(BT_DEVICE_ID_2)->hf_channel;
        }
        else
        {
            if(conn_devices > 1)
            {
                en = true;
                hf_id = BT_DEVICE_ID_2;
                hf_siri_chnl = app_bt_get_device(BT_DEVICE_ID_2)->hf_channel;
            }
            else if(conn_devices == 1)
            {
                en = true;
                hf_id = BT_DEVICE_ID_1;
                hf_siri_chnl = app_bt_get_device(BT_DEVICE_ID_1)->hf_channel;
            }
        }
        if((btif_get_hf_chan_state(app_bt_get_device(hf_id)->hf_channel) == BT_HFP_CHAN_STATE_OPEN))
        {
            res = btif_hf_enable_voice_recognition(app_bt_get_device(hf_id)->hf_channel, en);
        }

        BESUI_TRACE(3,"[UIKEY][%s] Line =%d, res = %d, en = %d", __func__, __LINE__, res, en);
    }
    else
    {
        app_key_ui_close_siri_process(); //iphone 12/15
    }
}

//---------------------------------------------------------------------------------------
#if 1
extern int open_siri_flag;
static int besui_hfp_siri_voice(bool en)
{
    HFCALL_MACHINE_ENUM hfcall_machine = app_get_hfcall_machine();
    BESUI_TRACE(0, "[UISIRI][%s], hfcall_machine = %d", __func__, hfcall_machine);

    static int device_id = BT_DEVICE_INVALID_ID;
    struct BT_DEVICE_T* curr_device = NULL;
    device_id = app_audio_adm_get_bt_active_device();

    if(device_id == BT_DEVICE_INVALID_ID)
    {
        BESUI_TRACE(0, "active device is null");
        return -1;
    }

    curr_device = app_bt_get_device(device_id);

    BESUI_TRACE(0, "[UISIRI][%s], en = %d", __func__, en);
    if(en == false)
    {
        if(btif_hf_is_voice_rec_active(curr_device->hf_channel) == false)
        {
            en = true;
            BESUI_TRACE(0, "[UISIRI][%s]siri audo end, need open", __func__);
        }
    }

    if((btif_get_hf_chan_state(curr_device->hf_channel) == BT_HFP_CHAN_STATE_OPEN)) 
    {
        btif_hf_enable_voice_recognition(curr_device->hf_channel, en);
        BESUI_TRACE(0, "[UISIRI]siri send %d", en);
    } 

    BESUI_TRACE(0, "[UISIRI][%s]id =%d/%d/%d", __func__, device_id, open_siri_flag, en);

    return 0;
}

static void besui_siri_onoff(void)
{
    if(!open_siri_flag){
        BESUI_TRACE(0,"[UISIRI]open siri");
        besui_hfp_siri_voice(true);
        open_siri_flag = 1;
    }else{
        BESUI_TRACE(0,"[UISIRI]close siri");
        besui_hfp_siri_voice(false);
        open_siri_flag = 0;
    }
}
#endif
void app_key_ui_open_close_siri_handle(void)
{
    besui_siri_onoff();
}

void app_open_close_siri_process_timehandler(void const *param)
{
	BESUI_TRACE(1, "[UISIRI]%s ", __func__);
    app_key_ui_open_close_siri_handle();
}

osTimerId app_open_close_siri_process_timer = NULL;
osTimerDef (APP_OPEN_CLOSE_SIRI_PROCESS_TIMER, (void (*)(void const *))app_open_close_siri_process_timehandler);
void app_siri_timer_onoff(bool timer_en)
{
    uint8_t conn_devices = besui_get_profile_conn_num();//app_bt_count_connected_device();

    if(conn_devices == 0)
        return;

	BESUI_TRACE(1, "[UISIRI]%s timer_en %d", __func__, timer_en);

	if(app_open_close_siri_process_timer == NULL)
	{
    	app_open_close_siri_process_timer = osTimerCreate(osTimer(APP_OPEN_CLOSE_SIRI_PROCESS_TIMER),osTimerOnce,NULL);
	}

	if(timer_en)
	{
		osTimerStart(app_open_close_siri_process_timer, 100);
	}
	else
	{
		osTimerStop(app_open_close_siri_process_timer);
	}
}


void app_fn1_key_gui_handler(bool other_key_flag, uint8_t key_event)
{
    BESUI_TRACE(2,"[UIKEY]%s other_key_flag %d key_event %d", __func__, other_key_flag, key_event);
    switch(key_event)
    {
        case APP_KEY_EVENT_ON_EAR:  //inear
            app_inoutear_common_handle(other_key_flag, INEAR_STATUS);
#ifdef CAPSENSOR_ENABLE
            if(!other_key_flag)
                twsui_wear_pp_tx(true, INEAR_STATUS);
#endif
            break;

        case APP_KEY_EVENT_OFF_EAR:  //outear
            app_inoutear_common_handle(other_key_flag, OUTEAR_STATUS);
#ifdef CAPSENSOR_ENABLE
            if(!other_key_flag)
                twsui_wear_pp_tx(true, OUTEAR_STATUS);
#endif
            break;
        default:
            break;
    }
}

#if defined(CAPSENSOR_SLIDE)
void app_left_key_gui_upslide(void)
{
    struct nvrecord_env_t *p_nvrecord_env;
    nv_record_env_get(&p_nvrecord_env);
    if(p_nvrecord_env->slide_left_onoff != true)
    {
        BESUI_TRACE(0,"[UIKEY]%s, left silde off, return!", __func__);
        return;
    }
    APP_KEY_STATUS status;
    status.code = BUTTON_MAP_KEY_TYPE;
    status.event = APP_KEY_EVENT_UPSLIDE;
    app_tota_general_button_event_handler(&status, 0);
}
void app_right_key_gui_upslide(void)
{
    struct nvrecord_env_t *p_nvrecord_env;
    nv_record_env_get(&p_nvrecord_env);
    if(p_nvrecord_env->slide_right_onoff != true)
    {
        BESUI_TRACE(0,"[UIKEY]%s, right silde off, return!", __func__);
        return;
    }
    APP_KEY_STATUS status;
    status.code = BUTTON_MAP_KEY_TYPE;
    status.event = APP_KEY_EVENT_UPSLIDE;
    app_tota_general_button_event_handler(&status, 1);
}
void app_key_gui_upslide_process(bool slave_key_flag)
{
    BESUI_TRACE(0,"[UIKEY]%s, slave_key_flag %d", __func__, slave_key_flag);
    if(besui_get_lr_sta() == LEFT_SIDE)
    {
        if(bts_tws_if_is_tws_link_connected())
        {
            if(slave_key_flag)
                app_right_key_gui_upslide();
            else
                app_left_key_gui_upslide();
        }
        else
        {
            app_left_key_gui_upslide();
        }
    }
    else if(besui_get_lr_sta() == RIGHT_SIDE)
    {
        if(bts_tws_if_is_tws_link_connected())
        {
            if(slave_key_flag)
                app_left_key_gui_upslide();
            else
                app_right_key_gui_upslide();
        }
        else
        {
            app_right_key_gui_upslide();
        }
    }
}
void app_left_key_gui_downslide(void)
{
    struct nvrecord_env_t *p_nvrecord_env;
    nv_record_env_get(&p_nvrecord_env);
    if(p_nvrecord_env->slide_left_onoff != true)
    {
        BESUI_TRACE(0,"[UIKEY]%s, left silde off, return!", __func__);
        return;
    }
    APP_KEY_STATUS status;
    status.code = BUTTON_MAP_KEY_TYPE;
    status.event = APP_KEY_EVENT_DOWNSLIDE;
    app_tota_general_button_event_handler(&status, 0);
}
void app_right_key_gui_downslide(void)
{
    struct nvrecord_env_t *p_nvrecord_env;
    nv_record_env_get(&p_nvrecord_env);
    if(p_nvrecord_env->slide_right_onoff != true)
    {
        BESUI_TRACE(0,"[UIKEY]%s, right silde off, return!", __func__);
        return;
    }
    APP_KEY_STATUS status;
    status.code = BUTTON_MAP_KEY_TYPE;
    status.event = APP_KEY_EVENT_DOWNSLIDE;
    app_tota_general_button_event_handler(&status, 1);
}
void app_key_gui_downslide_process(bool slave_key_flag)
{
    BESUI_TRACE(0,"[UIKEY]%s, slave_key_flag %d", __func__, slave_key_flag);
    if(besui_get_lr_sta() == LEFT_SIDE)
    {
        if(bts_tws_if_is_tws_link_connected())
        {
            if(slave_key_flag)
                app_right_key_gui_downslide();
            else
                app_left_key_gui_downslide();
        }
        else
        {
            app_left_key_gui_downslide();
        }
    }
    else if(besui_get_lr_sta() == RIGHT_SIDE)
    {
        if(bts_tws_if_is_tws_link_connected())
        {
            if(slave_key_flag)
                app_left_key_gui_downslide();
            else
                app_right_key_gui_downslide();
        }
        else
        {
            app_right_key_gui_downslide();
        }
    }
}
#endif //#ifdef CAPSENSOR_SLIDE

#ifdef BESUI_TOUCH_POWERON_EN
void besui_touch_poweron(void)
{
    BESUI_TRACE(0,"%s, bootmode = 0x%X", __func__, hal_sw_bootmode_get());
    if ((hal_sw_bootmode_get()&HAL_SW_BOOTMODE_TOUCH_POWERON))
    {
        uictl.touch_poweron = 1;

        app_sysfreq_req(APP_SYSFREQ_USER_APP_INIT, APP_SYSFREQ_32K);
        osDelay(10);
#ifdef CAPSENSOR_ENABLE
        app_mcu_core_capsensor_init();
#endif
        BESUI_TRACE(0,"while(1) ..........................................");
        enum CHARGER_ACIN_STATUS_TYPE_E charger_sta = CHARGER_ACIN_STATUS_PLUGOUT;
        enum CHARGER_ACIN_STATUS_TYPE_E charger_stabuf = CHARGER_ACIN_STATUS_PLUGOUT;
        while(1)
        {
            charger_sta = charger_acin_status_get();
            if(charger_stabuf != charger_sta)
            {
                charger_stabuf = charger_sta;
                TRACE(0, "[ACIN]%s, charger_acin_status_get() = %d", __func__, charger_sta);                
            }

            if(charger_sta == CHARGER_ACIN_STATUS_PLUGIN)
            {
                BESUI_TRACE(0,"ACIN 5V poweron start ...");
                break;
            }
            if(uictl.touch_poweron == 2)
            {
                BESUI_TRACE(0,"touch poweron start ...");
                break;
            }
            osDelay(500);
        }
        uictl.touch_poweron = 0;
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_TOUCH_POWERON);
        pmu_reboot();
    }
}

void besui_touch_poweroff(void)
{
    BESUI_TRACE(0,"%s", __func__);
    hal_sw_bootmode_set(HAL_SW_BOOTMODE_TOUCH_POWERON);
    osDelay(10);
    nv_record_flash_flush();

    uictl.poweroff_fast_flag = true;
    uictl.touch_poweroff_flag = true;
    // pmu_reboot();
    // app_shutdown();
}
#endif

#ifdef IBRT_SEARCH_UI
// void besui_enter_pairing_mode(void)
// {
        // BESUI_TRACE(0,"%s", __func__);
//     app_ibrt_if_event_entry(APP_UI_EV_CASE_OPEN);
//     bta_tws_enable_pairing_mode(true);
// }

void besui_exit_pairing_mode(void)
{
    BESUI_TRACE(0,"%s", __func__);
    bta_tws_enable_pairing_mode(false);
}

void besui_search_ui_enter_limiter_mode(void)
{
    // BESUI_TRACE(0,"%s", __func__);
    app_ibrt_if_event_entry(APP_UI_EV_CASE_OPEN);
    app_ibrt_enter_limited_mode();
}

void besui_search_ui_start_tws_pair(void)
{
    BESUI_TRACE(0,"%s", __func__);
    if(BT_IBRT_UNKNOWN == bts_tws_if_get_nv_role())
    {
        app_start_tws_serching_direactly();
    }
    else
    {
        bt_key_handle_func_doubleclick();
    }
}

// void enter_reconnect_mode(void)
// {
//     BESUI_TRACE(0,"%s", __func__);
//     app_ibrt_if_event_entry(APP_UI_EV_CASE_OPEN);
// }
#endif

#if defined(BESUI_FMD_UI_EN)
void spot_adv_timer_onoff(bool timer_en);

uint8_t spot_adv_cnt = 0;
osTimerId spot_adv_timer_id = NULL;
#define SPOT_ADV_TIMEOUT        (5*6) //5min
void spot_adv_timehandler(void const *param)
{
    spot_adv_cnt ++;
    if(spot_adv_cnt >= SPOT_ADV_TIMEOUT)
    {
        spot_adv_cnt = 0;
        fmd_utp_mode_off(twsui_get_bat_level());

        spot_adv_timer_onoff(false);
        osTimerDelete(spot_adv_timer_id);
        spot_adv_timer_id = NULL;
    }
    else
    {
        spot_adv_timer_onoff(true);
    }
	TRACE(0, "[UITIMER]%s, spot_adv_cnt = %d", __func__, spot_adv_cnt);
}
osTimerDef (SPOT_ADV_TIMER_DEF, (void (*)(void const *))spot_adv_timehandler);
void spot_adv_timer_onoff(bool timer_en)
{
	TRACE(1, "[UITIMER]%s timer_en %d ", __func__, timer_en);
    
	if(spot_adv_timer_id == NULL)
	{
    	spot_adv_timer_id = osTimerCreate(osTimer(SPOT_ADV_TIMER_DEF),osTimerOnce,NULL);
	}

	if(timer_en)
    {
        osTimerStart(spot_adv_timer_id, 10000);        
    }
    else
    {
        osTimerStop(spot_adv_timer_id);
        spot_adv_cnt = 0;
    }
}

extern "C" void app_dult_press_button_handle(APP_KEY_STATUS *status, void *param);
void app_key_gui_6click_process(bool slave_key_flag)
{
    TRACE(0, "[UIFMD]%s, slave_key_flag = %d", __func__, slave_key_flag);
    uint8_t keyflag = 0;
    if(TWS_UI_SLAVE != app_ibrt_if_get_ui_role())
    {
        if(besui_get_lr_sta() == LEFT_SIDE)
        {
            if(bts_tws_if_is_tws_link_connected())
            {
                if(slave_key_flag) //msg from slave
                    keyflag = 1;
                else //left is master, run
                    keyflag = 2;
            }
        }
        else if(besui_get_lr_sta() == RIGHT_SIDE)
        {
            if(slave_key_flag) //msg from left
                keyflag = 2;
            else
                keyflag = 1;
        }

        if(keyflag == 1) //right 6click
        {
            app_dult_press_button_handle(NULL, NULL);
            media_PlayAudio(AUD_ID_TAP_SINGLE, 0);
            fmd_refresh_adv(twsui_get_bat_level());
            spot_adv_timer_onoff(true);
        }
        else if(keyflag == 2) //left 6click
        {
            gfps_ble_spot_press_stop_ring_handle(NULL, NULL);
        }
    }
}
#endif

// void besui_charger_onoff(void)
// { 
//     static bool onoff = false;
//     onoff = !onoff;
//     charger_param_set_onoff(onoff, uicom.charge_ma_val);
// }

void app_pwr_key_gui_handler(bool slave_key_flag, uint8_t key_event)
{
    uint8_t conn_devices = besui_get_profile_conn_num();
    // ibrt_ctrl_t *p_ibrt_ctrl = NULL;
    // p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
    BESUI_TRACE(2,"[UIKEY]%s slave_key_flag %d key_event %d", __func__, slave_key_flag, key_event);
    switch(key_event)
    {
        case APP_KEY_EVENT_CLICK:
            app_key_gui_click_process(slave_key_flag);
// #ifdef BESUI_TOUCH_POWERON_EN
//             besui_touch_poweroff();
//             pmu_reboot();
// #endif
        // besui_charger_onoff();
// #ifdef BESUI_TWS_PAIR_USE_KEY_EN
//         twsui_tws_pair_use_key();
// #endif
            break;

        case APP_KEY_EVENT_DOUBLECLICK:
            if(conn_devices > 0)
            {
                app_key_gui_doubleclick_process(slave_key_flag);
            }
            break;

        case APP_KEY_EVENT_TRIPLECLICK:
            app_key_gui_tripleclick_process(slave_key_flag);
            break;

        case APP_KEY_EVENT_ULTRACLICK:
            app_key_gui_ultraclick_process(slave_key_flag);
            break;

        case APP_KEY_EVENT_RAMPAGECLICK:
            app_key_gui_rampageclick_process(slave_key_flag);
#if defined(BESUI_FMD_UI_EN)
            app_key_gui_6click_process(slave_key_flag);
#endif
            break;

        case APP_KEY_EVENT_LONGPRESS:
            // besui_reconnect_last_mobile_phone();
            #if 0
            if((conn_devices == 0)&&(!bts_tws_if_is_tws_link_connected()))
            {
                app_ibrt_if_event_entry(APP_UI_EV_CASE_OPEN);
                osDelay(50);
                bta_tws_enable_pairing_mode(true);
            }
            #endif

            if(app_bt_get_curr_access_mode() == BTIF_BAM_GENERAL_ACCESSIBLE)//pairing mode
            {
                BESUI_TRACE(0,"[UIKEY]pairing mode,longpress invalid");
                break;
            }
            app_key_gui_longpress_process(slave_key_flag);
            break;

        case APP_KEY_EVENT_LONGLONGPRESS:
            app_key_gui_longlongpress_process(slave_key_flag);
            break;
        case APP_KEY_EVENT_LONGLONGLONGPRESS:
            app_key_gui_longlonglongpress_process(slave_key_flag);
            break;

        case APP_KEY_EVENT_CLICK_AND_LONGPRESS:
            //fmd
            break;

        case APP_KEY_EVENT_DOUBLECLICK_AND_LONGPRESS:
#ifdef BESUI_TOUCH_POWERON_EN
            besui_touch_poweroff();
#endif
            uictl.shutdown_type = SHUTDOWN_TOUCH_PRESS;
            app_poweroff_shutdown_ui_post_msg(slave_key_flag, KEY_FUNCTION_POWEROFF);
            break;
#if defined(CAPSENSOR_SLIDE)
        case APP_KEY_EVENT_UPSLIDE:
            app_key_gui_upslide_process(slave_key_flag);
            break;
        case APP_KEY_EVENT_DOWNSLIDE:
            app_key_gui_downslide_process(slave_key_flag);
            break;
#endif
        default:
            break;
    }
}

void app_key_gui_inoutear_pp(bool play_flag)
{
    HFCALL_MACHINE_ENUM hfcall_machine = app_get_hfcall_machine();
    POSSIBLY_UNUSED struct BT_DEVICE_T* a2dp_device = app_bt_get_device(app_bt_audio_get_curr_a2dp_device());
    uint8_t conn_devices = besui_get_profile_conn_num();//app_bt_count_connected_device();

    BESUI_TRACE(2,"[UIKEY]%s role %d con %d", __func__, bts_core_get_ui_role(), conn_devices);
    if(BT_IBRT_SLAVE == bts_core_get_ui_role()||(conn_devices == 0))
    {
        BESUI_TRACE(0,"[UIKEY]slave or no connect, do not need send cmd");
        return;
    }

    BESUI_TRACE(2,"[UIKEY]PLAY %d streaming %d %d", a2dp_device->a2dp_play_pause_flag, app_bt_is_a2dp_streaming(BT_DEVICE_ID_1), app_bt_is_a2dp_streaming(BT_DEVICE_ID_2));
    
    switch(hfcall_machine)
    {
        case HFCALL_MACHINE_CURRENT_IDLE_ANOTHER_IDLE:
            BESUI_TRACE(1,"[UIKEY]%s play flag %d", __func__, play_flag);
            if(play_flag)
            {
                BESUI_TRACE(1, "[UIKEY]%s,play = %d", __func__, a2dp_device->a2dp_play_pause_flag);
                if(a2dp_device)
                {
                    if(a2dp_device->a2dp_play_pause_flag == 0 || a2dp_device->a2dp_streamming == false)
                    {
                        if(app_bt_is_a2dp_streaming(BT_DEVICE_ID_1)||app_bt_is_a2dp_streaming(BT_DEVICE_ID_2))
                        {
                            a2dp_handleKey(AVRCP_KEY_PLAY); //computer is streaming, it need to pause, iphone need play
                        }
                        else
                        {
                            a2dp_handleKey(AVRCP_KEY_PLAY);
                        }
                    }
                    else
                    {
                        if(app_bt_is_a2dp_streaming(BT_DEVICE_ID_1)||app_bt_is_a2dp_streaming(BT_DEVICE_ID_2))
                        {
                            if(!app_bt_device_is_computer(BT_DEVICE_ID_1) && !app_bt_device_is_computer(BT_DEVICE_ID_2))
                               a2dp_handleKey(AVRCP_KEY_PLAY); //streaming so slow,example ios
                        }
                        else
                        {
                            //a2dp_handleKey(AVRCP_KEY_PLAY);
                        }
                    }
                }
            }
            else
            {
                BESUI_TRACE(1, "[UIKEY]%s,pause = %d", __func__, a2dp_device->a2dp_play_pause_flag);
                if(a2dp_device)
                {
                    if(a2dp_device->a2dp_play_pause_flag == 1)
                    {
                        a2dp_handleKey(AVRCP_KEY_PAUSE);
                    }
                    else
                    {
                        if(app_bt_is_a2dp_streaming(BT_DEVICE_ID_1)||app_bt_is_a2dp_streaming(BT_DEVICE_ID_2))
                        {
                            if(!app_bt_device_is_computer(BT_DEVICE_ID_1) && !app_bt_device_is_computer(BT_DEVICE_ID_2))
                                a2dp_handleKey(AVRCP_KEY_PAUSE);
                        }
                    }
                }
            }
            break;

        default:

            break;
    }
}

void app_key_gui_inoutear_call(bool answer_flag)
{
    HFCALL_MACHINE_ENUM hfcall_machine = app_get_hfcall_machine();
    uint8_t conn_devices = besui_get_profile_conn_num();//app_bt_count_connected_device();
    uint8_t device_id = app_bt_audio_get_curr_hfp_device();
    struct BT_DEVICE_T *curr_device = app_bt_get_device(device_id);

    BESUI_TRACE(4,"[UIKEY]%s role %d con %d, hfcall_machine %d", __func__, bts_core_get_ui_role(), conn_devices, hfcall_machine);
    if(BT_IBRT_SLAVE == bts_core_get_ui_role()||(conn_devices == 0))
    {
        BESUI_TRACE(0,"[UIKEY]slave or no connect, do not need send cmd");
        return;
    }

    switch(hfcall_machine)
    {
        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_IDLE:
            if(answer_flag)
            {
                hfp_handle_key(HFP_KEY_ANSWER_CALL);
            }
            break;

        case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_IDLE:
            if((answer_flag)&&(curr_device->switch_sco_to_earbud))
            {
                hfp_handle_key(HFP_KEY_ADD_TO_EARPHONE);
            }
            else if((!answer_flag)&&(!curr_device->switch_sco_to_earbud))
            {
                hfp_handle_key(HFP_KEY_CHANGE_TO_PHONE);
            }
            break;

        default:
            break;
    }
}

void app_left_key_gui_click(void)
{
    HFCALL_MACHINE_ENUM hfcall_machine = app_get_hfcall_machine();
    POSSIBLY_UNUSED struct BT_DEVICE_T* a2dp_device = app_bt_get_device(app_bt_audio_get_curr_a2dp_device());
#ifdef BESUI_APP_EN
    APP_KEY_STATUS status;
#endif
    switch(hfcall_machine)
    {
        case HFCALL_MACHINE_CURRENT_IDLE_ANOTHER_IDLE:
#ifdef BESUI_APP_EN
        status.code = BUTTON_MAP_KEY_TYPE;
        status.event = APP_KEY_EVENT_CLICK;
        app_tota_general_button_event_handler(&status, 0);
#endif
            break;
        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_IDLE:
            hfp_handle_key(HFP_KEY_ANSWER_CALL);
        break;

        case HFCALL_MACHINE_CURRENT_OUTGOING_ANOTHER_IDLE:
            //hfp_handle_key(HFP_KEY_HANGUP_CALL);
        break;

        case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_IDLE:
        break;

        case HFCALL_MACHINE_CURRENT_3WAY_INCOMMING_ANOTHER_IDLE:
            //hfp_handle_key(HFP_KEY_THREEWAY_HANGUP_AND_ANSWER);
        break;

        case HFCALL_MACHINE_CURRENT_3WAY_HOLD_CALLING_ANOTHER_IDLE:
            //hfp_handle_key(HFP_KEY_THREEWAY_HANGUP_AND_ANSWER);
        break;

        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_INCOMMING:
        break;

        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_OUTGOING:
        break;

        case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_CHANGETOPHONE:
            //hfp_handle_key(HFP_KEY_DUAL_HF_HANGUP_ANOTHER_ADDTOEARPHONE);
        break;

        case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_HOLD:
            //hfp_handle_key(HFP_KEY_DUAL_HF_HANGUP_CURR_ANSWER_ANOTHER);
        break;

        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_CALLING:
            hfp_handle_key(HFP_KEY_DUAL_HF_CHANGETOPHONE_ANSWER_CURR);
        break;

        default:

            break;
    }
}

void app_right_key_gui_click(void)
{
    HFCALL_MACHINE_ENUM hfcall_machine = app_get_hfcall_machine();
    POSSIBLY_UNUSED struct BT_DEVICE_T* a2dp_device = app_bt_get_device(app_bt_audio_get_curr_a2dp_device());
#ifdef BESUI_APP_EN
    APP_KEY_STATUS status;
#endif
    switch(hfcall_machine)
    {
        case HFCALL_MACHINE_CURRENT_IDLE_ANOTHER_IDLE:
#ifdef BESUI_APP_EN
        status.code = BUTTON_MAP_KEY_TYPE;
        status.event = APP_KEY_EVENT_CLICK;
        app_tota_general_button_event_handler(&status, 1);
#endif
            break;
        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_IDLE:
            hfp_handle_key(HFP_KEY_ANSWER_CALL);
        break;

        case HFCALL_MACHINE_CURRENT_OUTGOING_ANOTHER_IDLE:
            // hfp_handle_key(HFP_KEY_HANGUP_CALL);
        break;

        case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_IDLE:
            hfp_handle_key(HFP_KEY_HANGUP_CALL);
        break;

        case HFCALL_MACHINE_CURRENT_3WAY_INCOMMING_ANOTHER_IDLE:
            //hfp_handle_key(HFP_KEY_THREEWAY_HANGUP_AND_ANSWER);
        break;

        case HFCALL_MACHINE_CURRENT_3WAY_HOLD_CALLING_ANOTHER_IDLE:
            //hfp_handle_key(HFP_KEY_THREEWAY_HANGUP_AND_ANSWER);
        break;

        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_INCOMMING:
        break;

        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_OUTGOING:
        break;

        case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_CHANGETOPHONE:
            //hfp_handle_key(HFP_KEY_DUAL_HF_HANGUP_ANOTHER_ADDTOEARPHONE);
        break;

        case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_HOLD:
            //hfp_handle_key(HFP_KEY_DUAL_HF_HANGUP_CURR_ANSWER_ANOTHER);
        break;

        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_CALLING:
            hfp_handle_key(HFP_KEY_DUAL_HF_CHANGETOPHONE_ANSWER_CURR);
        break;

        default:

            break;
    }
}

void app_key_gui_click_process(bool slave_key_flag)
{
    if(besui_get_lr_sta() == LEFT_SIDE)
    {
        if(bts_tws_if_is_tws_link_connected())
        {
            if(slave_key_flag)
                app_right_key_gui_click();
            else
                app_left_key_gui_click();
        }
        else
        {
            app_left_key_gui_click();
        }
    }
    else if(besui_get_lr_sta() == RIGHT_SIDE)
    {
        if(bts_tws_if_is_tws_link_connected())
        {
            if(slave_key_flag)
                app_left_key_gui_click();
            else
                app_right_key_gui_click();
        }
        else
        {
            app_right_key_gui_click();
        }
    }
}

#if defined(BESUI_PLAY_ANSWER_PROMPT)
void app_answer_call_voice_timer_onoff(bool timer_en);
osTimerId app_answer_call_delay_voice_process_timer = NULL;
void app_answer_call_delay_voice_process_timehandler(void)
{
    media_PlayAudio(AUD_ID_BT_CALL_ANSWER, 0);
    app_answer_call_voice_timer_onoff(false);
    osTimerDelete(app_answer_call_delay_voice_process_timer);
    app_answer_call_delay_voice_process_timer = NULL;
}

osTimerDef (APP_ANSWER_CALL_DELAY_VOICE_PROCESS_TIMER, (void (*)(void const *))app_answer_call_delay_voice_process_timehandler);

void app_answer_call_voice_timer_onoff(bool timer_en)
{
	BESUI_TRACE(1, "[UITIMER]%s timer_en %d ", __func__, timer_en);

	if(app_answer_call_delay_voice_process_timer == NULL)
	{
        app_answer_call_delay_voice_process_timer = osTimerCreate(osTimer(APP_ANSWER_CALL_DELAY_VOICE_PROCESS_TIMER),osTimerOnce,NULL);
        if(!app_answer_call_delay_voice_process_timer)
        {
             ASSERT(0, "[UITIMER][%s] osTimerCreate error", __func__);
        }
    }

	if(timer_en)
	{
        osTimerStart(app_answer_call_delay_voice_process_timer, 550);
	}
	else
	{
		osTimerStop(app_answer_call_delay_voice_process_timer);
	}
}
#endif //BESUI_PLAY_ANSWER_PROMPT

void app_left_key_gui_doubleclick(void)
{
    HFCALL_MACHINE_ENUM hfcall_machine = app_get_hfcall_machine();
    POSSIBLY_UNUSED struct BT_DEVICE_T* a2dp_device = app_bt_get_device(app_bt_audio_get_curr_a2dp_device());
#ifdef BESUI_APP_EN
    APP_KEY_STATUS status;
#endif
    BESUI_TRACE(2, "[UIKEY]%s, hfcall_machine = %d",__func__, hfcall_machine);
    switch(hfcall_machine)
    {
        case HFCALL_MACHINE_CURRENT_IDLE_ANOTHER_IDLE:
#ifdef BESUI_APP_EN
        status.code = BUTTON_MAP_KEY_TYPE;
        status.event = APP_KEY_EVENT_DOUBLECLICK;
        app_tota_general_button_event_handler(&status, 0);
#endif
            break;
        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_IDLE: //7
            uicom.call_end_prompt_type = REJECT_CALL_TYPE;
            hfp_handle_key(HFP_KEY_HANGUP_CALL);
        break;

        case HFCALL_MACHINE_CURRENT_OUTGOING_ANOTHER_IDLE: //8
            uicom.call_end_prompt_type = END_CALL_TYPE;
            hfp_handle_key(HFP_KEY_HANGUP_CALL);
        break;

        case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_IDLE: //9
            uicom.call_end_prompt_type = END_CALL_TYPE;
            hfp_handle_key(HFP_KEY_HANGUP_CALL);
        break;

        case HFCALL_MACHINE_CURRENT_3WAY_INCOMMING_ANOTHER_IDLE:
            //hfp_handle_key(HFP_KEY_THREEWAY_HOLD_AND_ANSWER);
        break;

        case HFCALL_MACHINE_CURRENT_3WAY_HOLD_CALLING_ANOTHER_IDLE:
            //hfp_handle_key(HFP_KEY_THREEWAY_HOLD_AND_ANSWER);
        break;

        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_INCOMMING:
        break;

        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_OUTGOING:
        break;

        case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_CHANGETOPHONE:
            uicom.call_end_prompt_type = END_CALL_TYPE;
            hfp_handle_key(HFP_KEY_DUAL_HF_HANGUP_ANOTHER_ADDTOEARPHONE);
        break;

        case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_HOLD:
            //hfp_handle_key(HFP_KEY_DUAL_HF_HANGUP_CURR_ANSWER_ANOTHER);
        break;

        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_CALLING: //12
            uicom.call_end_prompt_type = REJECT_CALL_TYPE;
            hfp_handle_key(HFP_KEY_DUAL_HF_HANGUP_CURR_ANSWER_ANOTHER);
        break;

        default:

            break;
    }
}

void app_right_key_gui_doubleclick(void)
{
    HFCALL_MACHINE_ENUM hfcall_machine = app_get_hfcall_machine();
    POSSIBLY_UNUSED struct BT_DEVICE_T* a2dp_device = app_bt_get_device(app_bt_audio_get_curr_a2dp_device());
#ifdef BESUI_APP_EN
    APP_KEY_STATUS status;
#endif
    BESUI_TRACE(2, "[UIKEY]%s, hfcall_machine = %d",__func__, hfcall_machine);
    switch(hfcall_machine)
    {
        case HFCALL_MACHINE_CURRENT_IDLE_ANOTHER_IDLE:
#ifdef BESUI_APP_EN
        status.code = BUTTON_MAP_KEY_TYPE;
        status.event = APP_KEY_EVENT_DOUBLECLICK;
        app_tota_general_button_event_handler(&status, 1);
#endif
            break;
        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_IDLE: //7
            uicom.call_end_prompt_type = REJECT_CALL_TYPE;
            hfp_handle_key(HFP_KEY_HANGUP_CALL);
        break;

        case HFCALL_MACHINE_CURRENT_OUTGOING_ANOTHER_IDLE: //8
            uicom.call_end_prompt_type = END_CALL_TYPE;
            hfp_handle_key(HFP_KEY_HANGUP_CALL);
        break;

        case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_IDLE: //9
            uicom.call_end_prompt_type = END_CALL_TYPE;
            hfp_handle_key(HFP_KEY_HANGUP_CALL);
        break;

        case HFCALL_MACHINE_CURRENT_3WAY_INCOMMING_ANOTHER_IDLE:
            //hfp_handle_key(HFP_KEY_THREEWAY_HOLD_AND_ANSWER);
        break;

        case HFCALL_MACHINE_CURRENT_3WAY_HOLD_CALLING_ANOTHER_IDLE:
            //hfp_handle_key(HFP_KEY_THREEWAY_HOLD_AND_ANSWER);
        break;

        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_INCOMMING:
        break;

        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_OUTGOING:
        break;

        case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_CHANGETOPHONE:
            uicom.call_end_prompt_type = END_CALL_TYPE;
            hfp_handle_key(HFP_KEY_DUAL_HF_HANGUP_ANOTHER_ADDTOEARPHONE);
        break;

        case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_HOLD:
            //hfp_handle_key(HFP_KEY_DUAL_HF_HANGUP_CURR_ANSWER_ANOTHER);
        break;

        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_CALLING: //12
            uicom.call_end_prompt_type = REJECT_CALL_TYPE;
            hfp_handle_key(HFP_KEY_DUAL_HF_HANGUP_CURR_ANSWER_ANOTHER);
        break;

        default:

            break;
    }
}

void app_key_gui_doubleclick_process(bool slave_key_flag)
{
    if(besui_get_lr_sta() == LEFT_SIDE)
    {
        if(bts_tws_if_is_tws_link_connected())
        {
            if(slave_key_flag)
                app_right_key_gui_doubleclick();
            else
                app_left_key_gui_doubleclick();
        }
        else
        {
            app_left_key_gui_doubleclick();
        }
    }
    else if(besui_get_lr_sta() == RIGHT_SIDE)
    {
        if(bts_tws_if_is_tws_link_connected())
        {
            if(slave_key_flag)
                app_left_key_gui_doubleclick();
            else
                app_right_key_gui_doubleclick();
        }
        else
        {
            app_right_key_gui_doubleclick();
        }
    }
}


void app_left_key_gui_tripleclick(void)
{
    HFCALL_MACHINE_ENUM hfcall_machine = app_get_hfcall_machine();
    POSSIBLY_UNUSED struct BT_DEVICE_T* a2dp_device = app_bt_get_device(app_bt_audio_get_curr_a2dp_device());
    BESUI_TRACE(2, "[UIKEY]%s, hfcall_machine = %d",__func__, hfcall_machine);    
#ifdef BESUI_APP_EN
    APP_KEY_STATUS status;
#endif
    BESUI_TRACE(2, "[UIKEY]%s, hfcall_machine = %d",__func__, hfcall_machine);
    switch(hfcall_machine)
    {
        case HFCALL_MACHINE_CURRENT_IDLE_ANOTHER_IDLE:
#ifdef BESUI_APP_EN
        status.code = BUTTON_MAP_KEY_TYPE;
        status.event = APP_KEY_EVENT_TRIPLECLICK;
        app_tota_general_button_event_handler(&status, 0);
#endif
            break;
        case HFCALL_MACHINE_CURRENT_AUDIO_OR_ANOTHER_AUDIO:
#ifdef BESUI_APP_EN
        status.code = BUTTON_MAP_KEY_TYPE;
        status.event = APP_KEY_EVENT_TRIPLECLICK;
        app_tota_general_button_event_handler(&status, 0);
#endif
            break;
        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_IDLE:
            //hfp_handle_key(HFP_KEY_ANSWER_CALL);
        break;

        case HFCALL_MACHINE_CURRENT_OUTGOING_ANOTHER_IDLE:
            //hfp_handle_key(HFP_KEY_HANGUP_CALL);
        break;

        case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_IDLE:
            //hfp_handle_key(HFP_KEY_HANGUP_CALL);
        break;

        case HFCALL_MACHINE_CURRENT_3WAY_INCOMMING_ANOTHER_IDLE:
            //hfp_handle_key(HFP_KEY_THREEWAY_HOLD_AND_ANSWER);
        break;

        case HFCALL_MACHINE_CURRENT_3WAY_HOLD_CALLING_ANOTHER_IDLE:
            //hfp_handle_key(HFP_KEY_THREEWAY_HOLD_AND_ANSWER);
        break;

        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_INCOMMING:
        break;

        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_OUTGOING:
        break;

        case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_CHANGETOPHONE:
            //hfp_handle_key(HFP_KEY_DUAL_HF_CHANGETOPHONE_ANOTHER_ADDTOEARPHONE);
        break;

        case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_HOLD:
            //hfp_handle_key(HFP_KEY_DUAL_HF_HANGUP_CURR_ANSWER_ANOTHER);
        break;

        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_CALLING:
            //hfp_handle_key(HFP_KEY_DUAL_HF_CHANGETOPHONE_ANSWER_ANOTHER);
        break;

        default:

            break;
    }
}

void app_right_key_gui_tripleclick(void)
{
    HFCALL_MACHINE_ENUM hfcall_machine = app_get_hfcall_machine();
    POSSIBLY_UNUSED struct BT_DEVICE_T* a2dp_device = app_bt_get_device(app_bt_audio_get_curr_a2dp_device());
#ifdef BESUI_APP_EN
    APP_KEY_STATUS status;
#endif

    switch(hfcall_machine)
    {
        case HFCALL_MACHINE_CURRENT_IDLE_ANOTHER_IDLE:
#ifdef BESUI_APP_EN
        status.code = BUTTON_MAP_KEY_TYPE;
        status.event = APP_KEY_EVENT_TRIPLECLICK;
        app_tota_general_button_event_handler(&status, 1);
#endif
            break;
        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_IDLE:
            //hfp_handle_key(HFP_KEY_ANSWER_CALL);
        break;

        case HFCALL_MACHINE_CURRENT_OUTGOING_ANOTHER_IDLE:
            //hfp_handle_key(HFP_KEY_HANGUP_CALL);
        break;

        case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_IDLE:
            //hfp_handle_key(HFP_KEY_HANGUP_CALL);
        break;

        case HFCALL_MACHINE_CURRENT_3WAY_INCOMMING_ANOTHER_IDLE:
            //hfp_handle_key(HFP_KEY_THREEWAY_HOLD_AND_ANSWER);
        break;

        case HFCALL_MACHINE_CURRENT_3WAY_HOLD_CALLING_ANOTHER_IDLE:
            //hfp_handle_key(HFP_KEY_THREEWAY_HOLD_AND_ANSWER);
        break;

        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_INCOMMING:
        break;

        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_OUTGOING:
        break;

        case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_CHANGETOPHONE:
            //hfp_handle_key(HFP_KEY_DUAL_HF_CHANGETOPHONE_ANOTHER_ADDTOEARPHONE);
        break;

        case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_HOLD:
            //hfp_handle_key(HFP_KEY_DUAL_HF_HANGUP_CURR_ANSWER_ANOTHER);
        break;

        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_CALLING:
            //hfp_handle_key(HFP_KEY_DUAL_HF_CHANGETOPHONE_ANSWER_ANOTHER);
        break;

        default:

            break;
    }
}

void app_key_gui_tripleclick_process(bool slave_key_flag)
{
#ifdef TRIPLE_DUT_EN
    app_key_gui_to_dut_test();
    return;
#endif
    if(besui_get_lr_sta() == LEFT_SIDE)
    {
        if(bts_tws_if_is_tws_link_connected())
        {
            if(slave_key_flag)
                app_right_key_gui_tripleclick();
            else
                app_left_key_gui_tripleclick();
        }
        else
        {
            app_left_key_gui_tripleclick();
        }
    }
    else if(besui_get_lr_sta() == RIGHT_SIDE)
    {
        if(bts_tws_if_is_tws_link_connected())
        {
            if(slave_key_flag)
                app_left_key_gui_tripleclick();
            else
                app_right_key_gui_tripleclick();
        }
        else
        {
            app_right_key_gui_tripleclick();
        }
    }
}

void app_key_gui_to_dut_test(void)
{
    hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT);
    app_factorymode_enter();
}

void app_key_gui_to_otaboot_or_single(bool ota_flag, bool single_flag)
{
    BESUI_TRACE(1,"[UIKEY]%s ota_flag %d",__func__, ota_flag);

    if((ota_flag)&&(single_flag))
    {
        return;
    }

    if(ota_flag)
    {
#if defined(OTA_BOOT_SYNC_EN)
        if(!uictl.tws_recv_otaboot_flag)
        {
            uint8_t ptrParam[2]={0};
            memset(ptrParam, 0, sizeof(ptrParam));
            ptrParam[0] = 0x01;
            //ptrParam[1] = 0x01;
            tws_ctrl_send_cmd(APP_TWS_CMD_SYNC_OTABOOT, ptrParam, 1);
            BESUI_TRACE(1,"[UIKEY][%s] APP_TWS_CMD_SYNC_OTABOOT",__func__);
            osDelay(100);
        }
#endif
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT); //ota_boot
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
    else if(single_flag)
    {
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT); //single wire mode
        hal_sw_bootmode_set(HAL_SW_BOOTMODE_SINGLE_LINE_DOWNLOAD);
        pmu_reboot();
    }
}


void app_left_key_gui_ultraclick(bool param)
{
    //app_key_gui_to_otaboot_or_single(false, true);
}


void app_right_key_gui_ultraclick(bool param)
{
    //app_key_gui_to_otaboot_or_single(false, true);
}

void app_key_gui_ultraclick_process(bool slave_key_flag)
{
#ifdef BESUI_TWS_PAIR_USE_KEY_EN
    if(app_bt_audio_count_streaming_a2dp()) //switch eq
    {
        besui_eq_mode_key_switch();
    }
    else
    {
        twsui_tws_pair_use_key();        
    }
#else
    if(besui_get_lr_sta() == LEFT_SIDE)
    {
        if(bts_tws_if_is_tws_link_connected())
        {
            if(slave_key_flag)
                app_right_key_gui_ultraclick(true);
            else
                app_left_key_gui_ultraclick(true);
        }
        else
        {
            app_left_key_gui_ultraclick(true);
        }
    }
    else if(besui_get_lr_sta() == RIGHT_SIDE)
    {
        if(bts_tws_if_is_tws_link_connected())
        {
            if(slave_key_flag)
                app_left_key_gui_ultraclick(true);
            else
                app_right_key_gui_ultraclick(true);
        }
        else
        {
            app_right_key_gui_ultraclick(true);
        }
    }
#endif
}

void app_left_key_gui_rampageclick(bool limited_enter_dut_flag)
{

}

void app_right_key_gui_rampageclick(bool limited_enter_dut_flag)
{

}

void app_key_gui_rampageclick_process(bool slave_key_flag)
{
    if(besui_get_lr_sta() == LEFT_SIDE)
    {
        if(bts_tws_if_is_tws_link_connected())
        {
            if(slave_key_flag)
                app_right_key_gui_rampageclick(true);
            else
                app_left_key_gui_rampageclick(true);
        }
        else
        {
            app_left_key_gui_rampageclick(true);
        }
    }
    else if(besui_get_lr_sta() == RIGHT_SIDE)
    {
        if(bts_tws_if_is_tws_link_connected())
        {
            if(slave_key_flag)
                app_left_key_gui_rampageclick(true);
            else
                app_right_key_gui_rampageclick(true);
        }
        else
        {
            app_right_key_gui_rampageclick(true);
        }
    }
}

void app_left_key_gui_longpress(void)
{
    HFCALL_MACHINE_ENUM hfcall_machine = app_get_hfcall_machine();
    POSSIBLY_UNUSED struct BT_DEVICE_T* a2dp_device = app_bt_get_device(app_bt_audio_get_curr_a2dp_device());
#ifdef BESUI_APP_EN
    APP_KEY_STATUS status;
#endif
    switch(hfcall_machine)
    {
        case HFCALL_MACHINE_CURRENT_IDLE_ANOTHER_IDLE:
#ifdef BESUI_APP_EN
        status.code = BUTTON_MAP_KEY_TYPE;
        status.event = APP_KEY_EVENT_INITLONGPRESS;
        app_tota_general_button_event_handler(&status, 0);
#endif
            break;
        case HFCALL_MACHINE_CURRENT_AUDIO_OR_ANOTHER_AUDIO:
#ifdef BESUI_APP_EN
        status.code = BUTTON_MAP_KEY_TYPE;
        status.event = APP_KEY_EVENT_TRIPLECLICK;
        app_tota_general_button_event_handler(&status, 0);
#endif
            break;
        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_IDLE: //7
            uicom.call_end_prompt_type = REJECT_CALL_TYPE;
            hfp_handle_key(HFP_KEY_HANGUP_CALL);
        break;

        case HFCALL_MACHINE_CURRENT_OUTGOING_ANOTHER_IDLE: //8
            uicom.call_end_prompt_type = END_CALL_TYPE;
            hfp_handle_key(HFP_KEY_HANGUP_CALL);
        break;

        case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_IDLE: //9
            uicom.call_end_prompt_type = END_CALL_TYPE;
            hfp_handle_key(HFP_KEY_HANGUP_CALL);
        break;  

        case HFCALL_MACHINE_CURRENT_3WAY_INCOMMING_ANOTHER_IDLE:
            //hfp_handle_key(HFP_KEY_THREEWAY_HOLD_REL_INCOMING);
        break;

        case HFCALL_MACHINE_CURRENT_3WAY_HOLD_CALLING_ANOTHER_IDLE:
            //hfp_handle_key(HFP_KEY_THREEWAY_HANGUP_AND_ANSWER);
        break;

        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_INCOMMING:
        break;

        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_OUTGOING:
        break;

        case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_CHANGETOPHONE:
            //hfp_handle_key(HFP_KEY_DUAL_HF_HANGUP_ANOTHER_ADDTOEARPHONE);
        break;

        case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_HOLD:
            //hfp_handle_key(HFP_KEY_DUAL_HF_HANGUP_CURR_ANSWER_ANOTHER);
        break;

        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_CALLING:
            //hfp_handle_key(HFP_KEY_DUAL_HF_HANGUP_ANOTHER);
        break;

        default:
            break;
    }
}

void app_right_key_gui_longpress(void)
{
    HFCALL_MACHINE_ENUM hfcall_machine = app_get_hfcall_machine();
    POSSIBLY_UNUSED struct BT_DEVICE_T* a2dp_device = app_bt_get_device(app_bt_audio_get_curr_a2dp_device());
#ifdef BESUI_APP_EN
    APP_KEY_STATUS status;
#endif
    switch(hfcall_machine)
    {
        case HFCALL_MACHINE_CURRENT_IDLE_ANOTHER_IDLE:
#ifdef BESUI_APP_EN
        status.code = BUTTON_MAP_KEY_TYPE;
        status.event = APP_KEY_EVENT_INITLONGPRESS;
        app_tota_general_button_event_handler(&status, 1);
#endif
            break;
        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_IDLE: //7
            uicom.call_end_prompt_type = REJECT_CALL_TYPE;
            hfp_handle_key(HFP_KEY_HANGUP_CALL);
        break;

        case HFCALL_MACHINE_CURRENT_OUTGOING_ANOTHER_IDLE: //8
            uicom.call_end_prompt_type = END_CALL_TYPE;
            hfp_handle_key(HFP_KEY_HANGUP_CALL);
        break;

        case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_IDLE: //9
            uicom.call_end_prompt_type = END_CALL_TYPE;
            hfp_handle_key(HFP_KEY_HANGUP_CALL);
        break;        

        case HFCALL_MACHINE_CURRENT_3WAY_INCOMMING_ANOTHER_IDLE:
            //hfp_handle_key(HFP_KEY_THREEWAY_HOLD_REL_INCOMING);
        break;

        case HFCALL_MACHINE_CURRENT_3WAY_HOLD_CALLING_ANOTHER_IDLE:
            //hfp_handle_key(HFP_KEY_THREEWAY_HANGUP_AND_ANSWER);
        break;

        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_INCOMMING:
        break;

        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_OUTGOING:
        break;

        case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_CHANGETOPHONE:
            //hfp_handle_key(HFP_KEY_DUAL_HF_HANGUP_ANOTHER_ADDTOEARPHONE);
        break;

        case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_HOLD:
            //hfp_handle_key(HFP_KEY_DUAL_HF_HANGUP_CURR_ANSWER_ANOTHER);
        break;

        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_CALLING:
            //hfp_handle_key(HFP_KEY_DUAL_HF_HANGUP_ANOTHER);
        break;

        default:
            break;
    }
}

void app_key_gui_longpress_process(bool slave_key_flag)
{
    BESUI_TRACE(3, "[UIKEY]%s,role = %d, slave_key_flag=%d", __func__, besui_get_lr_sta(), slave_key_flag);
    if(besui_get_lr_sta() == LEFT_SIDE)
    {
        if(bts_tws_if_is_tws_link_connected())
        {
            if(slave_key_flag)
                app_right_key_gui_longpress();
            else
                app_left_key_gui_longpress();
        }
        else
        {
            app_left_key_gui_longpress();
        }
    }
    else if(besui_get_lr_sta() == RIGHT_SIDE)
    {
        if(bts_tws_if_is_tws_link_connected())
        {
            if(slave_key_flag)
                app_left_key_gui_longpress();
            else
                app_right_key_gui_longpress();
        }
        else
        {
            app_right_key_gui_longpress();
        }
    }
}

void app_left_key_gui_longlongpress(bool slave_key_flag)
{
    besui_enter_pairmode();
}

void app_right_key_gui_longlongpress(bool slave_key_flag)
{
    besui_enter_pairmode();
}

void app_key_gui_longlongpress_process(bool slave_key_flag)
{
    if(besui_get_lr_sta() == LEFT_SIDE)
    {
        if(bts_tws_if_is_tws_link_connected())
        {
            if(slave_key_flag)
                app_right_key_gui_longlongpress(slave_key_flag);
            else
                app_left_key_gui_longlongpress(slave_key_flag);
        }
        else
        {
            app_left_key_gui_longlongpress(slave_key_flag);
        }
    }
    else if(besui_get_lr_sta() == RIGHT_SIDE)
    {
        if(bts_tws_if_is_tws_link_connected())
        {
            if(slave_key_flag)
                app_left_key_gui_longlongpress(slave_key_flag);
            else
                app_right_key_gui_longlongpress(slave_key_flag);
        }
        else
        {
            app_right_key_gui_longlongpress(slave_key_flag);
        }
    }
}

void app_left_key_gui_longlonglongpress(bool slave_key_flag)
{
    //PDL Reset / Restore to Factory Defaults
}

void app_right_key_gui_longlonglongpress(bool slave_key_flag)
{
    //PDL Reset / Restore to Factory Defaults
}

void app_key_gui_longlonglongpress_process(bool slave_key_flag)
{
    if(besui_get_lr_sta() == LEFT_SIDE)
    {
        if(bts_tws_if_is_tws_link_connected())
        {
            if(slave_key_flag)
                app_right_key_gui_longlonglongpress(slave_key_flag);
            else
                app_left_key_gui_longlonglongpress(slave_key_flag);
        }
        else
        {
            app_left_key_gui_longlonglongpress(slave_key_flag);
        }
    }
    else if(besui_get_lr_sta() == RIGHT_SIDE)
    {
        if(bts_tws_if_is_tws_link_connected())
        {
            if(slave_key_flag)
                app_left_key_gui_longlonglongpress(slave_key_flag);
            else
                app_right_key_gui_longlonglongpress(slave_key_flag);
        }
        else
        {
            app_right_key_gui_longlonglongpress(slave_key_flag);
        }
    }
}

#endif

#endif

