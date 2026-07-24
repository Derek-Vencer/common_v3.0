#ifdef BESUI_TWS_EN
#ifdef BESUI_CHARGE_EN
#include "twsui_charge.h"
#include "twsui_comm.h"
#include "app_thread.h"
#include "tgt_hardware.h"
#include "apps.h"
#include "hal_key.h"
#include "hal_trace.h"
#include "app_media_player.h"
#include "app_factory_bt.h"
#include "bta_tws_ux_api.h"
#include "earbud_ux_api.h"
#ifdef USER_APP_BLE_DIS_EN
#include "app_tota.h"
#endif
#ifdef BESUI_KEY_EN
#include "twsui_key.h"
#endif
#ifdef BESUI_BTMSG_EN
#include "twsui_btmsg.h"
#endif
#ifdef BESUI_1WIRE_EN
#include "twsui_uart.h"
#endif
#ifdef USER_SPP_CMD_EN
#include "twsui_spp.h"
#endif
#include "communication_svr.h"
#ifdef GFPS_ENABLED
#include "ble_gfps.h"
#include "gfps.h"
#include "nvrecord_fp_account_key.h"
#endif

#include CHIP_SPECIFIC_HDR(charger)

#include "app_bt.h"
#include "nvrecord_env.h"
#ifdef CAPSENSOR_WEAR 
#ifdef CAPSENSOR_FAC_CALCULATE
#include "capsensor_algorithm.h"
#include "capsensor_factory_cal.h"
#endif
#endif
#include "besui_common.h"
#if defined(ANC_APP)
#include "app_anc.h"
#endif


bool full_sta = false;
void besui_bat_full_sta_set(bool param)
{
    full_sta = param;
}
bool besui_bat_full_sta_get(void)
{
    BESUI_TRACE(0, "[UICHG]%s full_sta %d", __func__, full_sta);
    return full_sta;
}

bool charge_sta = false;
void besui_bat_charge_sta_set(bool param)
{
    charge_sta = param;
}
bool besui_bat_charge_sta_get(void)
{
    BESUI_TRACE(1,"[UICHG]%s charge_sta %d", __func__, charge_sta);
    return charge_sta;
}

static int app_charge_putinout_ui_handle_process(APP_MESSAGE_BODY *msg_body);

void app_charge_putinout_ui_modual_init(void)
{
    BESUI_TRACE(0, "[UICHG]%s", __func__);
    app_set_threadhandle(APP_MODUAL_CHARGE_PUTINOUT_UI, app_charge_putinout_ui_handle_process);
}

void app_charge_putinout_ui_post_msg(uint32_t param)
{
    BESUI_TRACE(1,"[UICHG]%s : 0x%02X", __func__, param);

    APP_MESSAGE_BLOCK msg;
    msg.mod_id = APP_MODUAL_CHARGE_PUTINOUT_UI;
    msg.msg_body.message_id = param;
    app_mailbox_put(&msg);
}

void app_poweron_enter_charge_process(uint8_t poweron_type)
{
    uint8_t msg_event = 0x00;

    BESUI_TRACE(1,"[UICHG]%s poweron_type 0x%02x", __func__, poweron_type);

    uicom.poweron_flag = true;

    if(poweron_type == POWERON_SEND)
    {
        msg_event = 0x80;

#ifdef BESUI_BOX_PUTINOUT_EN
        app_putin_putout_box_init();
#endif
#ifdef BESUI_KEY_EN
        app_key_gui_modual_init();
#endif
#ifdef BESUI_BTMSG_EN
        besui_bt_msg_modual_init();
#endif
#ifdef BESUI_TWS_EN
        app_poweroff_shutdown_ui_modual_init();
#endif
#ifdef BESUI_1WIRE_EN
        app_uart_threadhandle_init();
#endif
    }

    BESUI_TRACE(2,"[UICHG]%s charging_flag %d poweron_type %d", __func__, besui_bat_charge_sta_get(), poweron_type);
    if(besui_bat_charge_sta_get())
    {
        msg_event = msg_event|CHARGE_PLUGIN;
        app_charge_putinout_ui_post_msg(msg_event);
    }
    else
    {
        msg_event = msg_event|CHARGE_PLUGOUT;
        app_charge_putinout_ui_post_msg(msg_event);
    }
}

osTimerId app_poweron_init_timerid = NULL;
void app_poweron_init_timehandler(void const *param)
{
	BESUI_TRACE(1, "[UICHG]%s", __func__);
    app_charge_putinout_ui_post_msg(POWERON_EVENT);

    osTimerDelete(app_poweron_init_timerid);
    app_poweron_init_timerid = NULL;
}
osTimerDef (APP_POWERON_INIT_TIMER_NAME, (void (*)(void const *))app_poweron_init_timehandler);
void app_poweron_init_send_msg_timer_onoff(bool timer_en)
{
	BESUI_TRACE(1, "[UICHG]%s timer_en %d ", __func__, timer_en);

	if(app_poweron_init_timerid == NULL)
	{
    	app_poweron_init_timerid = osTimerCreate(osTimer(APP_POWERON_INIT_TIMER_NAME),osTimerOnce,NULL);
	}

	if(timer_en)
	{
		osTimerStart(app_poweron_init_timerid, 150);
	}
	else
	{
		osTimerStop(app_poweron_init_timerid);
	}
}

osTimerId app_enter_charge_delay_flush_flash_timer = NULL;
void app_enter_charge_delay_flush_flash_timehandler(void const *param)
{
	BESUI_TRACE(0, "[UICHG]%s ", __func__);
    app_charge_putinout_ui_post_msg(NV_RECORD_FLUSH_EVENT);
}

osTimerDef (APP_ENTER_CHARGE_DELAY_FLUSH_FLASH_TIMER, (void (*)(void const *))app_enter_charge_delay_flush_flash_timehandler);

void app_enter_charge_flush_flash_timer_onoff(bool timer_en)
{
	BESUI_TRACE(0, "[UICHG]%s timer_en %d ", __func__, timer_en);

	if(app_enter_charge_delay_flush_flash_timer == NULL)
	{
    	app_enter_charge_delay_flush_flash_timer = osTimerCreate(osTimer(APP_ENTER_CHARGE_DELAY_FLUSH_FLASH_TIMER),osTimerOnce,NULL);
	}

	if(timer_en)
	{
		osTimerStart(app_enter_charge_delay_flush_flash_timer, 10000);
	}
	else
	{
		osTimerStop(app_enter_charge_delay_flush_flash_timer);
	}
}

void app_charge_plugin_ui_handle_process(uint8_t plugin_type)
{
    BESUI_TRACE(1,"[UICHG]%s plugin_type %d box type 0x%02x", __func__, plugin_type, uicom.box_type);
    if(0xFF == uicom.box_type)
    {
#ifdef GFPS_ENABLED
        gfps_set_battery_datatype(HIDE_UI_INDICATION);
#endif
    }
// #ifdef BESUI_NTC_EN
//     if(!ntc_charger_status())
// #endif
        besui_charger_set_param(true, uicom.charge_ma_val);

    if(plugin_type == 0x80)
    {
        app_status_indication_set(APP_STATUS_INDICATION_CHARGING);
#ifdef BESUI_NTC_EN
        app_ntc_detect_init();
#endif
    }
    else
    {
        if(0xFF == uicom.box_type)
        {
            twsui_param_clear();

            app_siri_timer_onoff(false);
            app_poweron_twspairmode_timer_onoff(false);
            app_switch_role_poweroff_timer_onoff(false);
            app_phone_pairmode_timer_onoff(false);
            // app_phone_discon_pairmode_timer_onoff(false);
            app_phone_linkloss_pairmode_timer_onoff(false);
            app_recover_led_timer_onoff(false, false);
            app_tws_linkloss_pairmode_timer_onoff(false);
            app_enter_charge_flush_flash_timer_onoff(true);

#ifdef BESUI_GAME_EN
#ifndef BESUI_GAME_NV_EN
            app_gamemode_off(false);
#endif
#endif
            app_ibrt_if_event_entry(APP_UI_EV_CASE_CLOSE);
#ifdef GFPS_ENABLED
            app_exit_fastpairing_mode();
#endif
        }
        app_status_indication_set(APP_STATUS_INDICATION_CHARGING);
    }
    
#ifdef BESUI_1WIRE_EN
    if(0xFF == uicom.box_type)
    {
        uicom.box_type = BOX_CMD_CASE_CLOSE;
        uictl.box_sta = 0;
        app_stop_10_second_timer(APP_PAIR_TIMER_ID);
        app_stop_10_second_timer(APP_POWEROFF_TIMER_ID);
        app_pairmode_timer_onoff(false);
        app_poweroff_timer_onoff(false);
    }
    communication_uart_function_enable(false);
#endif

    besui_tws_key_init();
}

osTimerId app_poweron_delay_enter_twspairmode_timer = NULL;
void app_poweron_delay_enter_twspairmode_timehandler(void const *param)
{
#ifdef BESUI_1WIRE_EN
	BESUI_TRACE(1, "[UICHG]%s uart_twspair %d", __func__, uiuart.twspair_flag);

    app_ibrt_if_event_entry(APP_UI_EV_CASE_OPEN);
    osDelay(50);
    if(uiuart.twspair_flag == true)
    {
        bta_tws_enable_pairing_mode(true);
    }
    else
    {
        besui_enter_pairmode();
    }
#endif //#ifdef BESUI_1WIRE_EN
}

osTimerDef (APP_POWERON_DELAY_ENTER_TWSPAIRMODE_TIMER, (void (*)(void const *))app_poweron_delay_enter_twspairmode_timehandler);

void app_poweron_twspairmode_timer_onoff(bool timer_en)
{
	BESUI_TRACE(1, "[UICHG]%s timer_en %d ", __func__, timer_en);

	if(app_poweron_delay_enter_twspairmode_timer == NULL)
	{
    	app_poweron_delay_enter_twspairmode_timer = osTimerCreate(osTimer(APP_POWERON_DELAY_ENTER_TWSPAIRMODE_TIMER),osTimerOnce,NULL);
	}

	if(timer_en)
	{
		osTimerStart(app_poweron_delay_enter_twspairmode_timer, 500);
	}
	else
	{
		osTimerStop(app_poweron_delay_enter_twspairmode_timer);
	}
}


void app_charge_plugout_ui_handle_process(uint8_t plugout_type)
{
#ifdef BESUI_1WIRE_EN
    BESUI_TRACE(1,"[UICHG]%s plugout_type=0x%02X, box_type=0x%02X, %d, %d", __func__, plugout_type, uicom.box_type,app_poweroff_flag,uicom.poweron_box_flag);
    if(app_poweroff_flag)
    {
        BESUI_TRACE(0,"[UICHG]shutdown do not need poweron");
        return;
    }

#ifdef BOX_CLOSE_ADV_EN
    if(user_get_fmd_mode() && uicom.box_to_ear_poweroff)
    {
        return;
    }
#endif

    if(!uicom.poweron_box_flag)
    {
        besui_charger_set_param(false, uicom.charge_ma_val);
        return;
    }
    else
    {
        if(BOX_CMD_CASE_OPEN != uicom.box_type)
        {
            uicom.box_type = BOX_CMD_CASE_OPEN;
            uictl.box_sta = 1;
            app_status_indication_set(APP_STATUS_INDICATION_POWERON);
            uicom.led_box_open_flag = true;
            app_recover_led_timer_onoff(true, false);

#if BT_DEVICE_NUM > 1
            app_get_history_phone_paired_num();
#endif
            
            app_enter_charge_flush_flash_timer_onoff(false);
            besui_charger_set_param(false, uicom.charge_ma_val);
            // app_phone_discon_pairmode_timer_onoff(false);

#if defined(ANC_APP)
            besui_anc_mode_nv_read();
#endif

            if(plugout_type == 0x80)
            {
               
                media_PlayAudio(AUD_ID_POWER_ON, 0); //poweroff status exit charging status
                if(app_ibrt_get_history_paired_device())  //open box reconnect or enter pairmode
                {
                    app_status_indication_set(APP_STATUS_INDICATION_PAGESCAN);
                    app_ibrt_if_event_entry(APP_UI_EV_CASE_OPEN);
                    osDelay(50);
                    if(uicom.phone_pair_cnt > 1)
                    {
                        app_bt_mobile_set_openreconnect_info(0, CONNECT_OPEN_RECON, true);
                    }
                    else if(uicom.phone_pair_cnt == 1)
                    {
                        app_bt_mobile_set_openreconnect_info(0, CONNECT_OPEN_RECON, false);
                    }
                }
                else
                {
                    app_poweron_twspairmode_timer_onoff(true);
                }   

#ifdef BESUI_NTC_EN
                app_ntc_detect_init();
#endif
            }
            else
            {
                app_open_box_fast_get_battery_level();
                if(app_ibrt_get_history_paired_device())
                {
                    app_status_indication_set(APP_STATUS_INDICATION_PAGESCAN);
                    app_ibrt_if_event_entry(APP_UI_EV_CASE_OPEN);
                    osDelay(50);
                    if(uicom.phone_pair_cnt > 1)
                    {
                        app_bt_mobile_set_openreconnect_info(0, CONNECT_OPEN_RECON, true);
                    }
                    else if(uicom.phone_pair_cnt == 1)
                    {
                        app_bt_mobile_set_openreconnect_info(0, CONNECT_OPEN_RECON, false);
                    }

#ifdef GFPS_ENABLED
                    gfps_set_battery_datatype(SHOW_UI_INDICATION);
#endif
                }
                else
                {
                    app_poweron_twspairmode_timer_onoff(true);
                }
            }
            besui_tws_key_init();
            app_start_10_second_timer(APP_POWEROFF_TIMER_ID);
            app_poweroff_timer_onoff(true);
        }
    }
#endif //#ifdef BESUI_1WIRE_EN   
}

#ifdef BESUI_BOX_PUTINOUT_EN
void app_putin_putout_box_init(void)
{
    uint8_t box_high_level_cnt = 0;
    uint8_t box_low_level_cnt = 0;

    if (app_putin_putout_box_cfg.pin != HAL_IOMUX_PIN_NUM)
    {
        hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&app_putin_putout_box_cfg, 1);
        hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)app_putin_putout_box_cfg.pin, HAL_GPIO_DIR_IN, 1);
    
        while(1)
        {
            BESUI_TRACE(1,"[UICHG]putinout level %d",hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)app_putin_putout_box_cfg.pin));

            if (hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)app_putin_putout_box_cfg.pin))
            {
                box_high_level_cnt++;
                box_low_level_cnt = 0;
            }
            else
            {
                box_low_level_cnt++;
                box_high_level_cnt = 0;
            }

            if((box_high_level_cnt >= 5)||(box_low_level_cnt >= 5))
            {
                break;
            }

            osDelay(10);
        }

        if(box_high_level_cnt >= 5)
        {
            uicom.box_put_sta = PUTOUTBOX_STATUS;
            app_set_speaker_mute_status(false);
#ifdef BESUI_1WIRE_EN
            communication_uart_function_enable(false);
#endif
        }
        else if(box_low_level_cnt >= 5)
        {
            uicom.box_put_sta = PUTINBOX_STATUS;
            app_set_speaker_mute_status(true);
#ifdef BESUI_1WIRE_EN
            communication_uart_function_enable(true);
#endif
        }

        BESUI_TRACE(1,"[UICHG]%s putinout_box_status %d", __func__, uicom.box_put_sta);
        app_putin_putout_box_irq_set(true, uicom.box_put_sta);
    }
}

void app_putin_putout_box_irq_handler(enum HAL_GPIO_PIN_T pin)
{
	BESUI_TRACE(0,"[UICHG]%s",__func__);
	app_putin_putout_box_irq_set(false, 0xff);

    app_charge_putinout_ui_post_msg(PUTOUT_PUTIN_DETECT_EVENT);
}

void app_putin_putout_box_irq_set(bool irq_enable, uint8_t current_box_status)
{
    if(irq_enable)
    {
        if(current_box_status == PUTOUTBOX_STATUS)
        {
            hal_gpio_putin_putout_box_enable_irq((enum HAL_GPIO_PIN_T)app_putin_putout_box_cfg.pin, HAL_GPIO_IRQ_POLARITY_LOW_FALLING, app_putin_putout_box_irq_handler);
        }
        else if(current_box_status == PUTINBOX_STATUS)
        {
            hal_gpio_putin_putout_box_enable_irq((enum HAL_GPIO_PIN_T)app_putin_putout_box_cfg.pin, HAL_GPIO_IRQ_POLARITY_HIGH_RISING, app_putin_putout_box_irq_handler);
        }
    }
    else
    {
        hal_gpio_putin_putout_box_disable_irq((enum HAL_GPIO_PIN_T)app_putin_putout_box_cfg.pin);
    }
}

osTimerId app_putin_putout_scan_detect_process_timer = NULL;
uint8_t app_putin_box_cnt = 0;
uint8_t app_putout_box_cnt = 0;
#define PUTIN_PUTOUT_DETECT_CNT      3

void app_putin_putout_status_switch_detect(void)
{
    uint8_t gpio_level = 0;
    bool need_set_irq_flag = false;

    gpio_level = hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)app_putin_putout_box_cfg.pin);

    BESUI_TRACE(2,"[UICHG]%s gpio_level %d box_status %d", __func__, gpio_level, uicom.box_put_sta);

    if(uicom.box_put_sta == PUTINBOX_STATUS)
    {
        if(gpio_level)
        {
            app_putin_box_cnt = 0;
            if(++app_putout_box_cnt >= PUTIN_PUTOUT_DETECT_CNT)
            {
                app_putout_box_cnt = 0;
                need_set_irq_flag = true;
                uicom.box_put_sta = PUTOUTBOX_STATUS;
                app_charge_putinout_ui_post_msg(PUTOUT_BOX);
            }
        }
        else
        {
            app_putout_box_cnt = 0;
            if(++app_putin_box_cnt >= PUTIN_PUTOUT_DETECT_CNT)
            {
                app_putin_box_cnt = 0;
                need_set_irq_flag = true;
            }
        }
    }
    else if(uicom.box_put_sta == PUTOUTBOX_STATUS)
    {
        if(!gpio_level)
        {
            app_putout_box_cnt = 0;
            if(++app_putin_box_cnt >= PUTIN_PUTOUT_DETECT_CNT)
            {
                app_putin_box_cnt = 0;
                need_set_irq_flag = true;
                uicom.box_put_sta = PUTINBOX_STATUS;
                app_charge_putinout_ui_post_msg(PUTIN_BOX);
            }
        }
        else
        {
            app_putin_box_cnt = 0;
            if(++app_putout_box_cnt >= PUTIN_PUTOUT_DETECT_CNT)
            {
                app_putout_box_cnt = 0;
                need_set_irq_flag = true;
            }
        }
    }

    if(need_set_irq_flag)
    {
        need_set_irq_flag = false;
        app_putinout_scan_timer_onoff(false);
        app_putin_putout_box_irq_set(true, uicom.box_put_sta);
    }
}

void app_putin_putout_scan_detect_process_timehandler(void const *param)
{
	BESUI_TRACE(1, "[UICHG]%s ", __func__);

    app_putin_putout_status_switch_detect();
}

osTimerDef (APP_PUTIN_PUTOUT_SCAN_DETECT_PROCESS_TIMER, (void (*)(void const *))app_putin_putout_scan_detect_process_timehandler);

void app_putinout_scan_timer_onoff(bool timer_en)
{
	BESUI_TRACE(1, "[UICHG]%s timer_en %d ", __func__, timer_en);

	if(app_putin_putout_scan_detect_process_timer == NULL)
	{
    	app_putin_putout_scan_detect_process_timer = osTimerCreate(osTimer(APP_PUTIN_PUTOUT_SCAN_DETECT_PROCESS_TIMER),osTimerPeriodic,NULL);
	}

	if(timer_en)
	{
        app_putin_box_cnt = 0;
        app_putout_box_cnt = 0;
		osTimerStart(app_putin_putout_scan_detect_process_timer, 100);
	}
	else
	{
        app_putin_box_cnt = 0;
        app_putout_box_cnt = 0;
		osTimerStop(app_putin_putout_scan_detect_process_timer);
	}
}

osTimerId besui_delay_putinputout_event_handle_timer = NULL;
void besui_delay_putinputout_event_handle_timehandler(void const *param)
{
	BESUI_TRACE(1, "[UICHG]%s", __func__);       
    app_putinout_box_event_process(uicom.box_put_sta, true);
}
osTimerDef (BESUI_DELAY_PUTINPUTOUT_EVENT_TIMER_NAME, (void (*)(void const *))besui_delay_putinputout_event_handle_timehandler);
void besui_delay_putinout_timer_onoff(bool timer_en)
{
	BESUI_TRACE(1, "[UICHG]%s timer_en %d ", __func__, timer_en);

	if(besui_delay_putinputout_event_handle_timer == NULL)
	{
    	besui_delay_putinputout_event_handle_timer = osTimerCreate(osTimer(BESUI_DELAY_PUTINPUTOUT_EVENT_TIMER_NAME),osTimerOnce,NULL);
	}

	if(timer_en)
	{
		osTimerStart(besui_delay_putinputout_event_handle_timer, 600);
	}
	else
	{
		osTimerStop(besui_delay_putinputout_event_handle_timer);
	}
}


void app_putinout_box_event_process(uint8_t putinputout_event, bool phone_con_flag)
{
    uint8_t conn_devices = besui_get_profile_conn_num();
    bta_tws_box_state_t local_box_state = bta_tws_get_box_state(false);

    BESUI_TRACE(4,"[UICHG]%s putevent %02X conn %d localstate %d con %d", __func__, putinputout_event, conn_devices, local_box_state, phone_con_flag);
    if(putinputout_event == PUTINBOX_STATUS)
    {
        if(local_box_state == BTA_TWS_OUT_BOX)
        {
            app_ibrt_if_event_entry(APP_UI_EV_DOCK);
        }
        app_set_speaker_mute_status(true);
#ifdef BESUI_1WIRE_EN
        communication_uart_function_enable(true);
#endif
    }
    else if(putinputout_event == PUTOUTBOX_STATUS)
    {
#ifdef BOX_CLOSE_ADV_EN
        if(user_get_fmd_mode() && uicom.box_to_ear_poweroff)
        {
            return;
        }
#endif
        app_uart_open_box_handle();
        
        if(((local_box_state == BTA_TWS_IN_BOX_OPEN)&&(conn_devices > 0))||(!phone_con_flag))
        {
            app_ibrt_if_event_entry(APP_UI_EV_UNDOCK);
        }
        app_set_speaker_mute_status(false);
#ifdef BESUI_1WIRE_EN
        communication_uart_function_enable(false);
#endif
    }
}


void app_putin_putout_box_ui_handle(uint8_t putinout_status)
{
    app_putinout_box_event_process(putinout_status, true);
}
#endif

#if defined(USER_CHARGE_EXT_EN)
bool twsui_box_charge_full_shutdown(void) //10s
{
    #define CHARGE_FULL_TIMEOUT     (6*60*2) //full poweroff timeouts
    static uint16_t full_cnt = 0;
    if(user_get_putinout_sta() != CHARGE_PLUGIN) //CASE_CLOSE
    {
        full_cnt = 0;
        return false;
    }

    if(charger_charge_status_get()==CHARGER_CHARGE_STATUS_DONE)
    {
        full_cnt ++;
        BESUI_TRACE(0,"[UICHG]%s, full_cnt=%d", __func__, full_cnt);
        if(full_cnt >= CHARGE_FULL_TIMEOUT)
        {
            full_cnt = 0;
            BESUI_TRACE(0,"charge full, shutdown");
            return true;
        }
    }
    else
    {
        full_cnt = 0;
    }
    return false;
}
#endif

/*
void besui_auto_open_when_no_5v(uint8_t param)
{
    if(uictl.run_time_10s > 6)
    {
        BESUI_TRACE(0, "[UI5V]%s, poweron run time 20s, can't run case open", __func__);
        return;
    }
    bud_box_state local_box_state = app_ui_get_local_box_state();

    BESUI_TRACE(0, "[UI5V]%s, local_box_state = %d, param = %d", __func__, local_box_state, param);

    if(uicom.box_rx_open == 0)
    {
        BESUI_TRACE(0, "[UI5V]%s, powern audo open", __func__);
        uicom.box_rx_open = 0xFF;
        app_uart_open_box_handle();
        
        // if(((local_box_state == BTA_TWS_IN_BOX_OPEN)&&(conn_devices > 0))||(!phone_con_flag))
        {
            app_ibrt_if_event_entry(APP_UI_EV_UNDOCK);
        }
        app_set_speaker_mute_status(false);
        #ifdef BESUI_1WIRE_EN
        communication_uart_function_enable(false);
        #endif
    }
}
*/

#ifdef BOX_CLOSE_ADV_EN
#define ADV_ON_TIMEOUT          (60*17) //17min
#define ADV_STAY_TIMEOUT        (10)  //ADV stay time  10s
#define RTC_PWRKEY_SET_TIME     946684800 // 2000-1-1 00:00:00 6

static uint16_t adv_on_cnt = 0;
// static bool adv_start = false; //true:on  false:off
static uint8_t fmd_mode = false; //true:on  false:off
osTimerId user_adv_process_timerid = NULL;
void user_adv_timer_onoff(bool timer_en);

void user_adv_clear(void)
{
    BESUI_TRACE(0, "[UIFMD]%s", __func__);
    adv_on_cnt = 0;
    //adv_start = false;
    //fmd_mode = FMD_MODE_OFF;
}
/*
void user_set_adv_onoff(bool param)
{
    BESUI_TRACE(0, "[UIFMD]%s, %d", __func__, param);
    adv_start = param;
}

bool user_get_adv_onoff(void)
{
    BESUI_TRACE(0, "[UIFMD]%s, %d", __func__, adv_start);
    return adv_start;
}
*/

void user_set_fmd_mode(uint8_t param)
{
    BESUI_TRACE(0, "[UIFMD]%s, %d", __func__, param);
    fmd_mode = param;

    if(fmd_mode == FMD_MODE_OFF)
    {
        uicom.box_to_ear_poweroff = false;
        user_adv_clear();
        user_adv_timer_onoff(false);
    }

    nv_record_env_get(&nvrecord_uienv);
    nvrecord_uienv->fmd_mode = fmd_mode;
    nv_record_env_set(nvrecord_uienv);
}

uint8_t user_get_fmd_mode(void)
{
    static uint8_t fmdbuf = 0;
    if(fmdbuf != fmd_mode)
    {
        fmdbuf = fmd_mode;
        BESUI_TRACE(0, "[UIFMD]%s, %d", __func__, fmd_mode);        
    }
    return fmd_mode;
}

#ifdef BESUI_FMD_RTC_EN
#include "pmu.h"
static void besui_pmu_rtc_calendar_print(uint32_t time)
{
    struct RTC_CALENDAR_FORMAT_T cal;

    rtc_unix_to_calendar(&time, &cal);

    BESUI_TRACE(0, "[%s]: %d/%d/%d %d:%d:%d week:%d, time:%u\n", __func__,
        cal.year, cal.month, cal.day, cal.hour, cal.minute, cal.second, cal.week, time);
}

static bool rtc_alarm_happened = false;
static void besui_rtc_calendar_alarm_irq_cb(struct RTC_CALENDAR_FORMAT_T *cal)
{
    // BESUI_TRACE(0, "****** Enter %s ******", __func__);

    ASSERT(!rtc_alarm_happened, "%s: Check alarm irq!", __func__);

    rtc_alarm_happened = true;

    besui_pmu_rtc_calendar_print(rtc_calendar_to_unix(cal));
}

static void besui_rtc_alarm_irq_cb_set(void)
{
    pmu_rtc_calendar_alarm_irq_handler_set(besui_rtc_calendar_alarm_irq_cb);
}

static uint32_t besui_pmu_rtc_calendar_get(void)
{
    struct RTC_CALENDAR_FORMAT_T cal = {0, };
    pmu_rtc_calendar_get(&cal);

    return rtc_calendar_to_unix(&cal);
}

static void besui_pmu_rtc_calendar_alarm_set(uint32_t time)
{
    // BESUI_TRACE(0, "****** Enter %s ******", __func__);

    besui_pmu_rtc_calendar_print(time);

    struct RTC_CALENDAR_FORMAT_T cal;
    rtc_unix_to_calendar(&time, &cal);
    pmu_rtc_calendar_alarm_set(&cal);
}

static void besui_pmu_rtc_calendar_time_set(uint32_t time)
{
    // BESUI_TRACE(0, "****** Enter %s ******", __func__);

    struct RTC_CALENDAR_FORMAT_T cal;

    rtc_unix_to_calendar(&time, &cal);

    pmu_rtc_calendar_set(&cal);
}

bool besui_rtc_set_wakeup(void)
{
    BESUI_TRACE(0, "[UIFMD]%s", __func__);
    uint32_t time;

    besui_pmu_rtc_calendar_time_set(RTC_PWRKEY_SET_TIME);

    time=besui_pmu_rtc_calendar_get();
    besui_pmu_rtc_calendar_print(time);

    if(!nv_record_fp_get_spot_adv_enable_value())
    {
        BESUI_TRACE(0, "[UIFMD]%s, fmd off", __func__);
        return false;
    }

    user_set_fmd_mode(FMD_MODE_RTC_POWEROFF);
    user_get_fmd_mode();

    pmu_rtc_calendar_pwron_enable(true);
    pmu_rtc_calendar_alarm_clear();
    besui_rtc_alarm_irq_cb_set();

    time=besui_pmu_rtc_calendar_get();
    besui_pmu_rtc_calendar_print(time);
    time += ADV_ON_TIMEOUT;
    besui_pmu_rtc_calendar_alarm_set(time); //30s reboot

    uictl.poweroff_fast_flag = true;
    app_shutdown();
    // pmu_shutdown();

    return true;
}

void besui_rtc_get_wakeup(void)
{
    nv_record_env_get(&nvrecord_uienv);
    BESUI_TRACE(0, "[UIFMD]%s, nvrecord_uienv->fmd_mode = %d", __func__, nvrecord_uienv->fmd_mode);

    uint32_t cause_sta = pmu_boot_cause_get(); //0x20
    BESUI_TRACE(0, "[UIFMD]%s, cause_sta = 0x%02X", __func__, cause_sta);
    if(!(cause_sta & PMU_BOOT_CAUSE_RTC))
    {
        nvrecord_uienv->fmd_mode = FMD_MODE_OFF;
    }
    user_set_fmd_mode(nvrecord_uienv->fmd_mode);
}
#endif //#ifdef BESUI_FMD_RTC_EN

void user_adv_case_open_fake(void)
{
#if BT_DEVICE_NUM > 1
    app_get_history_phone_paired_num();
#endif

    app_open_box_fast_get_battery_level();
    if(app_ibrt_get_history_paired_device())
    {
        app_status_indication_set(APP_STATUS_INDICATION_PAGESCAN);
        app_ibrt_if_event_entry(APP_UI_EV_CASE_OPEN);
        osDelay(50);
        if(uicom.phone_pair_cnt > 1)
        {
            app_bt_mobile_set_openreconnect_info(0, CONNECT_OPEN_RECON, true);
        }
        else if(uicom.phone_pair_cnt == 1)
        {
            app_bt_mobile_set_openreconnect_info(0, CONNECT_OPEN_RECON, false);
        }

        #ifdef GFPS_ENABLED
        gfps_set_battery_datatype(SHOW_UI_INDICATION);
        #endif
    }
    else
    {
        app_poweron_twspairmode_timer_onoff(true);
    }
}

void user_adv_process_timehandler(void const *param)
{
	adv_on_cnt ++;
	BESUI_TRACE(0, "[UIFMD][UITIMER]%s adv_on_cnt = %d", __func__, adv_on_cnt);    
    if(adv_on_cnt >= ADV_ON_TIMEOUT)
    {
        // if(!user_get_adv_onoff())
        // {
        //     user_set_adv_onoff(true);
        //if(!user_get_fmd_mode())
        if(adv_on_cnt < (ADV_ON_TIMEOUT+ADV_STAY_TIMEOUT))
        {
            //user_set_fmd_mode(FMD_MODE_POWERON);
            //app_enter_fastpairing_mode();
            //app_putin_putout_box_ui_handle(PUTOUT_BOX);
            //app_uart_open_box_handle();

            //CASE_OPEN
            //app_ibrt_if_event_entry(APP_UI_EV_UNDOCK);

            // app_open_box_fast_get_battery_level();
            // app_ibrt_if_event_entry(APP_UI_EV_CASE_OPEN);
			if(adv_on_cnt == ADV_ON_TIMEOUT)
                user_adv_case_open_fake();
            uictl.box_sta = 1;
            TRACE(0, "[UIFMD][UITIMER]%s, fmd_adv_start!!!", __func__);
        }
        else
        {
            //if(adv_on_cnt >= (ADV_ON_TIMEOUT+ADV_STAY_TIMEOUT)) //adv off
            {
                adv_on_cnt = 0;
                // adv_start = false;
                //app_exit_fastpairing_mode();
                uictl.box_sta = 0;
                app_putin_putout_box_ui_handle(PUTIN_BOX);
                app_uart_close_box_handle();
                if(user_get_fmd_mode() == FMD_MODE_RTC_POWEROFF)
                {
                    besui_rtc_set_wakeup();
                }
                TRACE(0, "[UIFMD][UITIMER]%s, fmd_adv_end!!!", __func__);
            }
        }
        //osTimerDelete(user_adv_process_timerid);
        //user_adv_process_timerid = NULL;
    }
    user_adv_timer_onoff(true);
}
osTimerDef (USER_ADV_PROCESS_TIMER_NAME, (void (*)(void const *))user_adv_process_timehandler);
void user_adv_timer_onoff(bool timer_en)
{
    uint8_t dut_flag = bt_test_mode_dut_get();
    BESUI_TRACE(2, "%s dut_flag %d", __func__, dut_flag);
    if (dut_flag == 0)
    {
        if(!nv_record_fp_get_spot_adv_enable_value())
        {
            BESUI_TRACE(0, "[UIFMD]%s, fmd off", __func__);
            return;
        }
    }
    else
    {
        return;
    }
	BESUI_TRACE(0, "[UIFMD][UITIMER]%s timer_en %d ", __func__, timer_en);

	if(user_adv_process_timerid == NULL)
	{
        user_adv_process_timerid = osTimerCreate(osTimer(USER_ADV_PROCESS_TIMER_NAME),osTimerOnce,NULL);
        if(!user_adv_process_timerid)
            ASSERT(0, "[UIFMD][UITIMER][%s] osTimerCreate error", __func__);
	}

	if(timer_en)
		osTimerStart(user_adv_process_timerid, 1000);
	else
		osTimerStop(user_adv_process_timerid);
}
#endif //#ifdef BOX_CLOSE_ADV_EN
//------------------------------------------------------------------------------------------

static int app_charge_putinout_ui_handle_process(APP_MESSAGE_BODY *msg_body)
{
    if(bt_test_mode_dut_get())
    {
        return 0;
    }
    uint8_t putinout_type = (uint8_t)msg_body->message_id;
    uint8_t poweron_type = 0;
    
    poweron_type = (putinout_type&0x80);
    putinout_type = (putinout_type&0x0f);
    BESUI_TRACE(1,"[UICHG]%s, %d",__func__, putinout_type);

    const char *put_str[] = {
        "NULL",
        "CHARGE_PLUGIN",    //CASE_CLOSE 
        "CHARGE_PLUGOUT",   //CASE_CLOSE->CASE_OPEN
        "PUTIN_BOX",        //UNDOCK->DOCK
        "PUTOUT_BOX",       //DOCK->UNDOCK
    };
// #ifdef EVB_DEMO_EN
//     if(putinout_type < PUTOUT_BOX)
//         return 0;
//     putinout_type = PUTOUT_BOX;
// #endif
    if(putinout_type <= PUTOUT_BOX)
    {
        BESUI_TRACE(1,"[UICHG]ear_putinout = %s",put_str[putinout_type]);
        user_set_putinout_sta(putinout_type);

#ifdef CAPSENSOR_WEAR
        //in-1  out-0  case_close-2
#ifdef CAPSENSOR_FAC_CALCULATE
        if(putinout_type == PUTOUT_BOX)
            capsensor_set_box_state(0);
        else if(putinout_type == PUTIN_BOX)
            capsensor_set_box_state(1);
#endif
#endif
#ifdef BOX_COMM_UNUSE_EN
        if(user_get_putinout_sta() == CHARGE_PLUGIN) //in box        
        {
#if defined(ANC_APP)
            besui_anc_temp_onoff(false, true); //anc off
#endif
            app_ibrt_if_event_entry(APP_UI_EV_DOCK);
            app_uart_close_box_handle();
            uictl.box_sta = 0;
        }
        else if(user_get_putinout_sta() == PUTIN_BOX)
        {
        }
        else
        {
            app_uart_open_box_handle();
            // app_ibrt_if_event_entry(APP_UI_EV_UNDOCK);
            uictl.box_sta = 1;
        }
#endif
//------------------------------------------------------------------------------------------
#ifdef BOX_CLOSE_ADV_EN
        if(nv_record_fp_get_spot_adv_enable_value() && (putinout_type == CHARGE_PLUGIN || user_get_fmd_mode()!=FMD_MODE_OFF))
        {
            if(user_get_fmd_mode() == FMD_MODE_OFF)
                user_set_fmd_mode(FMD_MODE_POWERON);
            user_adv_clear();
            user_adv_timer_onoff(true);
        }

        if(user_get_fmd_mode() == FMD_MODE_RTC_POWEROFF)
        {
            user_adv_clear();
            user_adv_timer_onoff(true);
            adv_on_cnt = ADV_ON_TIMEOUT;
        }
#endif
//------------------------------------------------------------------------------------------
    }

    switch(putinout_type)
    {
        case CHARGE_PLUGIN:
            app_charge_plugin_ui_handle_process(poweron_type);
            break;

        case CHARGE_PLUGOUT:
            if(uicom.poweron_flag == false) //avoid poweron not run POWERON_SEND 
                app_poweron_enter_charge_process(POWERON_SEND);
            app_charge_plugout_ui_handle_process(poweron_type);
            // besui_auto_open_when_no_5v(putinout_type);
            break;

        case PUTOUT_PUTIN_DETECT_EVENT:
#ifdef BESUI_BOX_PUTINOUT_EN
            app_putinout_scan_timer_onoff(true);
#endif
            break;

        case PUTIN_BOX:
#ifdef BESUI_BOX_PUTINOUT_EN
            uicom.key_func_onoff = true;
            app_putin_putout_box_ui_handle(PUTIN_BOX);
#endif
            break;

        case PUTOUT_BOX:
#ifdef BESUI_BOX_PUTINOUT_EN
            uicom.key_func_onoff = false;
            app_putin_putout_box_ui_handle(PUTOUT_BOX);
#endif
            break;

        case WEAR_UP:

            break;

        case WEAR_DOWN:

            break;

        case POWERON_EVENT:
            app_poweron_enter_charge_process(POWERON_SEND);
            break;
        default:

            break;
    }

    return 0;
}

//-----------------------------------------------------------------------------------------
#ifdef VIO_VOLTAGE_CONFIG_EN
void pmu_open_vio_0v(void) //vio output 0V
{
#if defined(CHIP_BEST1501P)
    pmu_module_set_volt(PMU_SENSOR, 0, 0); 
    pmu_module_config(PMU_SENSOR,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
#endif
}
void pmu_open_vio_3v(void) //vio output 3V
{
#if defined(CHIP_BEST1501P)
    pmu_module_set_volt(PMU_SENSOR, PMU_LDO_SENSOR_3_0V, PMU_LDO_SENSOR_3_0V); 
    pmu_module_config(PMU_SENSOR,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
#else
    pmu_module_set_volt(PMU_USB, PMU_USB_3_0V, PMU_USB_3_0V);     
    pmu_module_config(PMU_USB,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
#endif
}
void pmu_open_vio_1p8v(void) //vio output 1.8V
{
#if defined(CHIP_BEST1501P)
    pmu_module_set_volt(PMU_SENSOR, PMU_LDO_SENSOR_1_8V, PMU_LDO_SENSOR_1_8V); 
    pmu_module_config(PMU_SENSOR,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
#else
    pmu_module_config(PMU_USB,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
#endif
}
void pmu_open_vio_3p3v(void) //vio output 3.3V
{
#if defined(CHIP_BEST1501P)
    pmu_module_set_volt(PMU_SENSOR, PMU_LDO_SENSOR_3_3V, PMU_LDO_SENSOR_3_3V); 
    pmu_module_config(PMU_SENSOR,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
#endif
}

void pmu_open_vio_out(void) //vio output
{
    // pmu_open_vio_0v(); //ota
#if defined(CHIP_BEST1501P)
    pmu_open_vio_1p8v();
#else
    pmu_open_vio_3p3v();
#endif
}
#endif


#ifdef USER_CHARGE_EXT_EN
uint8_t charger_ext_get_full(void)
{
    uint8_t val = CHARGER_CHARGE_STATUS_IDLE;
    if (hal_gpio_pin_get_val(HAL_IOMUX_PIN_P1_4)) //high-full, low-charing
    {
        val = CHARGER_CHARGE_STATUS_DONE; //full
    }
    TRACE(0, "[UICHG]%s, val=%d", __func__, val);
    return val;
}

enum CHARGER_CHARGE_STATUE_E charger_charge_status_get(void)
{
    return (enum CHARGER_CHARGE_STATUE_E)charger_ext_get_full();
}

void charger_mos_onoff(bool sta) //1-on-charge   0-off
{
    TRACE(0, "[UIMOS]%s, %d", __func__, sta);
    if(sta)
        hal_gpio_pin_clr(HAL_GPIO_PIN_P1_2);
    else
        hal_gpio_pin_set(HAL_GPIO_PIN_P1_2);
}

void charger_1501p_onoff(bool sta) //0-on   1-off
{
    TRACE(0, "[UICHG]%s, %d", __func__, sta);
    if(sta)
        hal_gpio_pin_clr(HAL_GPIO_PIN_P2_0); //low open
    else
        hal_gpio_pin_set(HAL_GPIO_PIN_P2_0); //high close
    // charger_mos_onoff(sta);
}

void besui_charger_set_param(bool open_close_flag, uint8_t battery_current)
{
    TRACE(0, "[UICHG]%s, %d, %d", __func__, open_close_flag, battery_current);
    if(battery_current == 3 && open_close_flag) //CHARGER_CHARGE_CONSTANT_CURRENT_80MA
        hal_gpio_pin_set(HAL_GPIO_PIN_P1_7); //2C
    else
        hal_gpio_pin_clr(HAL_GPIO_PIN_P1_7); //1C
    if(ntc_charger_status())
        charger_1501p_onoff(0);
    else
        charger_1501p_onoff(open_close_flag);
}
#else
#if defined(CHIP_BEST1501P) || defined(CHIP_BEST1502X) || defined(CHIP_BEST1501)
enum CHARGER_CHARGE_STATUE_E charger_charge_status_get(void)
{
    return CHARGER_CHARGE_STATUS_IDLE;
}
#endif
#endif

WEAK void besui_charger_set_param(bool open_close_flag, uint8_t battery_current)
{
#if 0
    struct CHARGER_CHARGE_MODULE_CFG_T charge_cfg;

    charger_charge_module_cfg_get(&charge_cfg);

    BESUI_TRACE(8,"[UICHARGE]%s %d %d %d %d %d %d %d %d",__func__,
    charge_cfg.prechg_current,charge_cfg.cc_current,charge_cfg.stop_current,
    charge_cfg.cv_volt,charge_cfg.rechg_volt,
    charge_cfg.ntc_det_en,charge_cfg.rechg_en,charge_cfg.chg_en);

    charge_cfg.prechg_current = CHARGER_CHARGE_PRECHARGE_CURRENT_10MA;
    BESUI_TRACE(1,"[UICHARGE]%s battery_current %d", __func__, battery_current);
    charge_cfg.cc_current = battery_current;//CHARGER_CHARGE_CONSTANT_CURRENT_60MA;
    //charge_cfg.cc_current = CHARGER_CHARGE_CONSTANT_CURRENT_80MA;
    charge_cfg.stop_current = CHARGER_CHARGE_STOP_CURRENT_4MA;
    charge_cfg.cv_volt = CHARGER_CHARGE_CONSTANT_VOLTAGE_4200MV;
    charge_cfg.rechg_volt = CHARGER_CHARGE_RECHARGE_VOLTAGE_200MV; //CHARGER_CHARGE_RECHARGE_VOLTAGE_150MV
    charge_cfg.ntc_det_en = 0;
    charge_cfg.rechg_en = 0;
    
    if(open_close_flag)
    {
        charge_cfg.chg_en = 1;
    }
    else
    {
        charge_cfg.chg_en = 0;
    }
    //BESUI_TRACE(1,"[UICHARGE]%s chg_en %d", __func__, charge_cfg.chg_en);
    charger_charge_module_cfg_set(&charge_cfg);

    charger_charge_module_cfg_get(&charge_cfg);

    BESUI_TRACE(8,"[UICHARGE]get %s %d %d %d %d %d %d %d %d",__func__,
    charge_cfg.prechg_current,charge_cfg.cc_current,charge_cfg.stop_current,
    charge_cfg.cv_volt,charge_cfg.rechg_volt,
    charge_cfg.ntc_det_en,charge_cfg.rechg_en,charge_cfg.chg_en);

    //charger_charge_enable();
#endif
}

//-----------------------------------------------------------------------------------------
#endif
#endif