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
#include "app_ibrt_search_pair_ui.h"
#include <string.h>
#include "hal_trace.h"
#include "bluetooth_bt_api.h"
#include "btapp.h"
#include "bts_core_if.h"
#include "app_vendor_cmd_evt.h"
#include "cmsis_os.h"
#include "besbt.h"
#include "stdlib.h"
#include "app_bt.h"
#include "factory_section.h"
#include "a2dp_decoder.h"
#include "app_battery.h"
#include "nvrecord_bt.h"
#include "nvrecord_env.h"
#include "app_status_ind.h"
#include "app_ui_api.h"
#include "bts_bt_if.h"

#ifdef IBRT_UI
#include "app_ibrt_debug.h"
#include "bts_tws_api.h"
#include "bta_tws_ux_api.h"
#endif

#ifdef BESUI_TWS_EN
#include "twsui_comm.h"
#endif
#if defined(BESUI_COMM_EN)
#include "besui_common.h"
#endif

extern "C" {

}
#ifdef IBRT_SEARCH_UI
static void app_tws_inquiry_timeout_handler(void const *param);
osTimerDef (APP_TWS_INQ, app_tws_inquiry_timeout_handler);
static osTimerId app_tws_timer = NULL;

static void app_tws_delay_connect_handler(void const *param);
osTimerDef (APP_TWS_DELAY_CONNECT, app_tws_delay_connect_handler);
static osTimerId app_tws_delay_connect_timer = NULL;


static uint8_t tws_find_process=0;
static uint8_t tws_inquiry_count=0;
#define MAX_TWS_INQUIRY_TIMES   3
#define IBRT_MAX_SEARCH_TIME    10 /* 12.8s */

uint8_t tws_inq_addr_used;
typedef struct
{
    uint8_t used;
    bt_bdaddr_t bdaddr;
} TWS_INQ_ADDR_STRUCT;
TWS_INQ_ADDR_STRUCT tws_inq_addr[5];

bta_tws_box_event_t box_event=BTA_TWS_CLOSE;
static void app_box_handle_timehandler(void const *param);
osTimerDef (APP_BOX_HANDLE, app_box_handle_timehandler);
static osTimerId app_box_handle_timer = NULL;
void app_ibrt_search_ui_init(bool boxOperation, uint8_t boxEvent);

static void app_box_handle_timehandler(void const *param)
{
    bta_tws_box_event_t *box_event_ptr=(bta_tws_box_event_t *)param;
    bta_tws_box_event_t boxAction=*box_event_ptr;
    EARBUDS_TRACE(0,"box event:%d",boxAction);
    app_ibrt_search_ui_init(true,boxAction);
    bta_tws_box_event_entry(boxAction);

    if(BTA_TWS_CLOSE==boxAction)
        app_ibrt_search_ui_init(true,boxAction);

}

static void app_tws_inquiry_timeout_handler(void const *param)
{

    EARBUDS_TRACE(0,"app_tws_inquiry_timeout_handler\n");
    btif_me_inquiry(BTIF_BT_IAC_LIAC, IBRT_MAX_SEARCH_TIME, 0);

}

static void app_tws_delay_connect_handler(void const *parma)
{
    EARBUDS_TRACE(1,"%s",__func__);
    bta_tws_connect_tws_link();
}

bool app_tws_is_addr_in_tws_inq_array(const bt_bdaddr_t* addr)
{
    uint16_t i;
    for(i = 0; i < ARRAY_SIZE(tws_inq_addr); i++)
    {
        if(tws_inq_addr[i].used == 1)
        {
            if(!memcmp(tws_inq_addr[i].bdaddr.address,addr->address,BTIF_BD_ADDR_SIZE))
                return true;
        }
    }
    return false;
}
void app_tws_clear_tws_inq_array(void)
{
    memset(&tws_inq_addr,0,sizeof(tws_inq_addr));
}

int app_tws_fill_addr_to_array(const bt_bdaddr_t*addr)
{
    uint16_t i;
    for(i = 0; i < ARRAY_SIZE(tws_inq_addr); i++)
    {
        if(tws_inq_addr[i].used == 0)
        {
            tws_inq_addr[i].used =1;
            memcpy(tws_inq_addr[i].bdaddr.address,addr->address,BTIF_BD_ADDR_SIZE);
            return 0;
        }
    }

    return -1;
}

uint8_t app_tws_get_tws_addr_inq_num(void)
{
    uint8_t i,count=0;
    for(i = 0; i < ARRAY_SIZE(tws_inq_addr); i++)
    {
        if(tws_inq_addr[i].used == 1)
        {
            count++;
        }
    }
    return count;
}
/*****************************************************************************
 Prototype    : app_tws_ibrt_update_info
 Description  : config tws info
 Input        : bt_ibrt_role_t twsRole
                bt_bdaddr_t *twsAddr

 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2019/3/26
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_tws_ibrt_update_info(bt_ibrt_role_t ibrtRole,bt_bdaddr_t *ibrtPeerAddr)
{
    EARBUDS_TRACE(0,"%s",__func__);
    uint8_t nv_role = bts_tws_if_get_nv_role();
    uint8_t *bt_peer_addr = bts_tws_if_get_peer_addr();
    if(nv_role ==BT_IBRT_UNKNOWN)
    {
        nv_role = ibrtRole;
        bts_tws_if_update_nv_role(ibrtRole);
    }
    if (NULL != ibrtPeerAddr)
    {
        memcpy(bt_peer_addr, ibrtPeerAddr->address, BD_ADDR_LEN);
        nv_record_update_ibrt_info(nv_role,ibrtPeerAddr);
        uint32_t audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_LRMERGE;
#ifdef IBRT_RIGHT_MASTER
        if (BT_IBRT_MASTER == nv_role)
        {
            EARBUDS_TRACE(0,"MASTER #right");
            audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_RCHNL;
            bts_tws_if_set_local_side(BT_LOCATION_RIGHT);
        }
        else if (BT_IBRT_SLAVE == nv_role)
        {
            EARBUDS_TRACE(0,"SLAVE #left");
            audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_LCHNL;
            bts_tws_if_set_local_side(BT_LOCATION_LEFT);
        }
#else
        if (BT_IBRT_SLAVE == nv_role)
        {
            EARBUDS_TRACE(0,"SLAVE #right");
            audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_RCHNL;
            bts_tws_if_set_local_side(BT_LOCATION_RIGHT);
        }
        else if (BT_IBRT_MASTER == nv_role)
        {
            EARBUDS_TRACE(0,"SLAVE #right");
            audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_LCHNL;
            bts_tws_if_set_local_side(BT_LOCATION_LEFT);
        }
#endif
		bts_bt_if_update_audio_chnl_sel(audio_chnl_sel);
    }
}

void tws_app_stop_find(void)
{
    tws_find_process=0;
    btif_me_unregister_globa_handler((btif_handler *)btif_me_get_bt_handler());
}

void app_bt_manager_ibrt_role_process(const btif_event_t *Event)
{
    switch (btif_me_get_callback_event_type(Event))
    {
        case BTIF_BTEVENT_LINK_CONNECT_IND:
        case BTIF_BTEVENT_LINK_CONNECT_CNF:
            if (BTIF_BEC_NO_ERROR == btif_me_get_callback_event_err_code(Event))
            {
                bt_bdaddr_t *p_remote_dev_addr=NULL;
                uint8_t *bt_local_addr = bts_tws_if_get_local_addr();
                uint8_t nv_role = bts_tws_if_get_nv_role();
                btif_remote_device_t *btm_conn = btif_me_get_callback_event_rem_dev(Event);
                p_remote_dev_addr = btif_me_get_remote_device_bdaddr(btm_conn);

                if(nv_role ==BT_IBRT_UNKNOWN)
                    factory_section_original_btaddr_get(bt_local_addr);

                EARBUDS_TRACE(0,"local:%x remd:%x", bt_local_addr[5], p_remote_dev_addr->address[5]);
                if((bt_local_addr[3]==p_remote_dev_addr->address[3])
                   &&(bt_local_addr[4]==p_remote_dev_addr->address[4])
                   &&(bt_local_addr[5]==p_remote_dev_addr->address[5]))
                {
                    // Tws connection complete, exit limited mode
                    bta_tws_enable_limited_mode(false);

                    if(nv_role ==BT_IBRT_UNKNOWN)
                    {
                        app_tws_ibrt_update_info(BT_IBRT_SLAVE,p_remote_dev_addr);
                        bts_core_set_ui_role(BT_IBRT_SLAVE);
                    }
                    //if(app_ibrt_ui_get_tws_use_same_addr_enable())
                    //ibrt use the same address
                    {
                        memcpy(bt_local_addr, p_remote_dev_addr->address,6);
                    }
                }

            }
            break;
        default:
            break;
    }

}

void app_ibrt_config_the_same_bd_addr(bt_bdaddr_t *ibrtSearchedAddr)
{
    uint8_t *bt_local_addr = bts_tws_if_get_local_addr();
    uint8_t *bt_peer_addr = bts_tws_if_get_peer_addr();

    memcpy(bt_local_addr, ibrtSearchedAddr->address, BD_ADDR_LEN);
    memcpy(bt_peer_addr, ibrtSearchedAddr->address, BD_ADDR_LEN);
    bts_bt_if_update_local_bt_addr(ibrtSearchedAddr->address);
#ifdef __GMA_VOICE__
    btif_me_set_ble_bd_address(ibrtSearchedAddr->address);
#endif
    EARBUDS_TRACE(0,"%s", __func__);
    EARBUDS_DUMP8("%02x ", bt_local_addr, BT_ADDR_OUTPUT_PRINT_NUM);
}

void app_ibrt_reconfig_btAddr_from_nv()
{
    struct nvrecord_env_t *nvrecord_env;
    if(nv_record_env_get(&nvrecord_env)==-1)
    {
        return;
    }
    if(nvrecord_env->ibrt_mode.mode ==BT_IBRT_UNKNOWN)
    {
        return;
    }

    EARBUDS_TRACE(0,"reconfig addr from nv");
    EARBUDS_DUMP8("%02x ",nvrecord_env->ibrt_mode.record.bdAddr.address, BT_ADDR_OUTPUT_PRINT_NUM);
    bt_set_local_address(nvrecord_env->ibrt_mode.record.bdAddr.address);
#ifdef __GMA_VOICE__
    bt_set_ble_local_address(nvrecord_env->ibrt_mode.record.bdAddr.address);
#endif
}

void app_bt_inquiry_call_back(const btif_event_t* event)
{
    EARBUDS_TRACE(2,"\nenter: %s %d\n",__func__,__LINE__);
    uint8_t device_name[64];
    uint8_t device_name_len;
    uint8_t *bt_local_addr = bts_tws_if_get_local_addr();
    factory_section_original_btaddr_get(bt_local_addr);


    switch(btif_me_get_callback_event_type(event))
    {
        case BTIF_BTEVENT_NAME_RESULT:
            EARBUDS_TRACE(2,"\n%s %d BTEVENT_NAME_RESULT\n",__func__,__LINE__);
            break;
        case BTIF_BTEVENT_INQUIRY_RESULT:
            EARBUDS_TRACE(2,"\n%s %d BTEVENT_INQUIRY_RESULT\n",__func__,__LINE__);
            EARBUDS_DUMP8("%02x ", btif_me_get_callback_event_inq_result_bd_addr_addr(event), BT_ADDR_OUTPUT_PRINT_NUM);
            EARBUDS_TRACE(1,"inqmode = %x",btif_me_get_callback_event_inq_result_inq_mode(event));
            EARBUDS_DUMP8("%02x ", btif_me_get_callback_event_inq_result_ext_inq_resp(event), 20);
            ///check the uap and nap if equal ,get the name for tws slave
            EARBUDS_TRACE(0,"##RSSI:%d",(int8_t)btif_me_get_callback_event_rssi(event));
            EARBUDS_TRACE(0,"local %02x %02x %02x %02x %02x %02x\n",
                  bt_local_addr[0],
                  bt_local_addr[1],
                  bt_local_addr[2],
                  bt_local_addr[3],
                  bt_local_addr[4],
                  bt_local_addr[5]);
            if((btif_me_get_callback_event_inq_result_bd_addr(event)->address[5]== bt_local_addr[5])
               && (btif_me_get_callback_event_inq_result_bd_addr(event)->address[4]== bt_local_addr[4])
               && (btif_me_get_callback_event_inq_result_bd_addr(event)->address[3]== bt_local_addr[3]))
            {
                ///check the device is already checked
                EARBUDS_TRACE(0,"<1>");
                if(app_tws_is_addr_in_tws_inq_array(btif_me_get_callback_event_inq_result_bd_addr(event)))
                {
                    break;
                }
                ////if rssi event is eir,so find name derictly
                if(btif_me_get_callback_event_inq_result_inq_mode(event) == BTIF_INQ_MODE_EXTENDED)
                {

                    EARBUDS_TRACE(0,"<2>");
                    uint8_t *eir = (uint8_t *)btif_me_get_callback_event_inq_result_ext_inq_resp(event);
                    //device_name_len = ME_GetExtInqData(eir,0x09,device_name,sizeof(device_name));
                    device_name_len = btif_me_get_ext_inq_data(eir,0x09,device_name,sizeof(device_name));
                    if(device_name_len>0)
                    {
                        EARBUDS_TRACE(3,"<3> search name len %d %s local name %s\n", device_name_len, device_name, bt_get_local_name());
                        ////if name is the same as the local name so we think the device is the tws slave
                        if(!memcmp(device_name,bt_get_local_name(),device_name_len))
                        {
                            EARBUDS_TRACE(0,"<4>");

                            //modify local addr
                            app_ibrt_config_the_same_bd_addr(btif_me_get_callback_event_inq_result_bd_addr(event));
                            btif_me_cancel_inquiry();
                            osTimerStop(app_tws_timer);
                            tws_app_stop_find();
                            app_tws_ibrt_update_info(BT_IBRT_MASTER,btif_me_get_callback_event_inq_result_bd_addr(event));
                            bts_core_set_ui_role(BT_IBRT_MASTER);
                            if(NULL !=app_tws_delay_connect_timer)
                            {
                                osTimerStop(app_tws_delay_connect_timer);
                                osTimerStart(app_tws_delay_connect_timer, 500);
                            }
                        }
                        else
                        {
                            if(app_tws_get_tws_addr_inq_num()<sizeof(tws_inq_addr)/sizeof(tws_inq_addr[0]))
                            {
                                app_tws_fill_addr_to_array(btif_me_get_callback_event_inq_result_bd_addr(event));
                                if(app_tws_get_tws_addr_inq_num()==sizeof(tws_inq_addr)/sizeof(tws_inq_addr[0]))
                                {
                                    ///fail to find a tws slave
                                    btif_me_cancel_inquiry();
                                    tws_app_stop_find();
                                }
                            }
                            else
                            {
                                ///fail to find a tws slave
                                btif_me_cancel_inquiry();
                                tws_app_stop_find();
                            }
                        }
                        break;
                    }
                    /////have no name so just wait for next device
                    //////we can do remote name req for tws slave if eir can't received correctly

                }
            }
            break;
        case BTIF_BTEVENT_INQUIRY_COMPLETE:
            EARBUDS_TRACE(2,"\n%s %d BTEVENT_INQUIRY_COMPLETE\n",__FUNCTION__,__LINE__);
            if(tws_inquiry_count>=MAX_TWS_INQUIRY_TIMES)
            {
                tws_app_stop_find();
                return;
            }
            //if(p_ibrt_ui->super_state ==IBRT_UI_IDLE)
            {
                ////inquiry complete if bt don't find any slave ,so do inquiry again

                uint8_t rand_delay = rand() % 5;
                tws_inquiry_count++;

                if(rand_delay == 0)
                {
                    //btif_me_inquiry(BTIF_BT_IAC_GIAC, IBRT_MAX_SEARCH_TIME, 0);
                    btif_me_inquiry(BTIF_BT_IAC_LIAC, IBRT_MAX_SEARCH_TIME, 0);
                }
                else
                {
                    osTimerStart(app_tws_timer, rand_delay*1000);
                }
            }
            break;
        /** The Inquiry process is canceled. */
        case BTIF_BTEVENT_INQUIRY_CANCELED:
            EARBUDS_TRACE(2,"\n%s %d BTEVENT_INQUIRY_CANCELED\n",__FUNCTION__,__LINE__);
            // tws.notify(&tws);
            break;
        case BTIF_BTEVENT_LINK_CONNECT_CNF:
            EARBUDS_TRACE(3,"\n%s %d BTEVENT_LINK_CONNECT_CNF stats=%x\n",__FUNCTION__,__LINE__,btif_me_get_callback_event_err_code(event));

            //connect fail start inquiry again
            if(btif_me_get_callback_event_err_code(event) ==4 && tws_find_process == 1)
            {
                if(tws_inquiry_count>=MAX_TWS_INQUIRY_TIMES)
                {
                    tws_app_stop_find();
                    return;
                }
                uint8_t rand_delay = rand() % 5;
                tws_inquiry_count++;
                if(rand_delay == 0)
                {
                    //btif_me_inquiry(BTIF_BT_IAC_GIAC, IBRT_MAX_SEARCH_TIME, 0);
                    btif_me_inquiry(BTIF_BT_IAC_LIAC, IBRT_MAX_SEARCH_TIME, 0);
                }
                else
                {
                    osTimerStart(app_tws_timer, rand_delay*1000);
                }
            }
            ///connect succ,so stop the finding tws procedure
            else if(btif_me_get_callback_event_err_code(event) ==0)
            {
                tws_app_stop_find();
            }
            break;
        case BTIF_BTEVENT_LINK_CONNECT_IND:
            EARBUDS_TRACE(3,"\n%s %d BTEVENT_LINK_CONNECT_IND stats=%x\n",__FUNCTION__,__LINE__,btif_me_get_callback_event_err_code(event));
            ////there is a incoming connect so cancel the inquiry and the timer and the  connect creating
            btif_me_cancel_inquiry();
            osTimerStop(app_tws_timer);
            break;
        default:
            //TWS_DBLOG("\n%s %d etype:%d\n",__FUNCTION__,__LINE__,event->eType);
            break;


    }

    //TWS_DBLOG("\nexit: %s %d\n",__FUNCTION__,__LINE__);

}



uint8_t is_find_tws_peer_device_onprocess(void)
{
    return tws_find_process;
}

void find_tws_peer_device_start(void)
{
    EARBUDS_TRACE(2,"\nibrt_ui_log: %s %d\n",__func__,__LINE__);
    bt_status_t  status;
    app_tws_clear_tws_inq_array();
    if(tws_find_process ==0)
    {
        tws_find_process = 1;
        tws_inquiry_count = 0;
        if (app_tws_timer == NULL)
            app_tws_timer = osTimerCreate(osTimer(APP_TWS_INQ), osTimerOnce, NULL);
        btif_me_set_handler(btif_me_get_bt_handler(),app_bt_inquiry_call_back);
        btif_me_register_global_handler(btif_me_get_bt_handler());

        btif_me_set_event_mask(btif_me_get_bt_handler(), BTIF_BEM_LINK_DISCONNECT|BTIF_BEM_ROLE_CHANGE|BTIF_BEM_INQUIRY_RESULT|
                               BTIF_BEM_INQUIRY_COMPLETE|BTIF_BEM_INQUIRY_CANCELED|BTIF_BEM_LINK_CONNECT_CNF|BTIF_BEM_LINK_CONNECT_IND);

    again:
        EARBUDS_TRACE(2,"\n%s %d\n",__func__,__LINE__);

        status = btif_me_inquiry(BTIF_BT_IAC_LIAC, IBRT_MAX_SEARCH_TIME, 0);
        EARBUDS_TRACE(2,"\n%s %d\n",__func__,__LINE__);
        if (status != BT_STS_PENDING)
        {
            osDelay(500);
            goto again;
        }
        EARBUDS_TRACE(2,"\n%s %d\n",__func__,__LINE__);
    }
}


void find_tws_peer_device_stop(void)
{
    btif_me_cancel_inquiry();
    tws_app_stop_find();
}

void app_start_tws_serching_direactly()
{
    btif_accessible_mode_t mode;

    mode = app_bt_get_curr_access_mode();
    EARBUDS_TRACE(1,"ibrt_ui_log:search tws direactly access_mode:%d",mode);
    if ((BTIF_BAM_GENERAL_ACCESSIBLE==mode)||(BTIF_BAM_LIMITED_ACCESSIBLE==mode))
    {
		if(NULL==app_tws_delay_connect_timer)
			app_tws_delay_connect_timer=osTimerCreate(osTimer(APP_TWS_DELAY_CONNECT),osTimerOnce,NULL );
        if (is_find_tws_peer_device_onprocess())
        {
            find_tws_peer_device_stop();
        }
        else
        {
            bts_tws_if_update_nv_role(TWS_ROLE_UNKNOW);
            find_tws_peer_device_start();

            //  app_status_indication_set(APP_STATUS_INDICATION_CONNECTING);
        }
    }
}


static void app_ibrt_battery_handle_process_normal(uint32_t status,  union APP_BATTERY_MSG_PRAMS prams)
{

    switch (status)
    {
        case APP_BATTERY_STATUS_CHARGING:
        {
            uint8_t nv_role = bts_tws_if_get_nv_role();
            EARBUDS_TRACE(1,"charger:%d",prams.charger);
            if (prams.charger == APP_BATTERY_CHARGER_PLUGIN)
            {
                EARBUDS_TRACE(1,"APP_BATTERY_CHARGER_PLUGIN nv_role %02x", nv_role);
                if (nv_role == BT_IBRT_UNKNOWN)
                {
                    return;
                }
               // if(p_ui_ctrl->config.check_plugin_excute_closedbox_event==true)
               if(app_ui_get_config()->check_plugin_excute_closedbox_event)
                    box_event=BTA_TWS_CLOSE;
                else
                    box_event=BTA_TWS_DOCK;

                if(app_box_handle_timer!=NULL)
                {
                    osTimerStop(app_box_handle_timer);
                    osTimerStart(app_box_handle_timer,500);
                }

            }
            else if (prams.charger == APP_BATTERY_CHARGER_PLUGOUT)
            {
                EARBUDS_TRACE(1,"APP_BATTERY_CHARGER_PLUGOUT nv_role %02x", nv_role);
                if (nv_role == BT_IBRT_UNKNOWN)
                {
                    return;
                }
                box_event=BTA_TWS_UNDOCK;

                if(app_box_handle_timer!=NULL)
                {
                    osTimerStop(app_box_handle_timer);
                    osTimerStart(app_box_handle_timer,500);
                }
            }
        }
            break;
        case APP_BATTERY_STATUS_INVALID:
        default:
            break;

    }


}


void app_ibrt_battery_callback(APP_BATTERY_MV_T currvolt, uint8_t currlevel,enum APP_BATTERY_STATUS_T curstatus,uint32_t status, union APP_BATTERY_MSG_PRAMS prams)
{
    switch (curstatus)
    {
        case APP_BATTERY_STATUS_NORMAL:
        case APP_BATTERY_STATUS_CHARGING:
            app_ibrt_battery_handle_process_normal(status,prams);
            break;

        default:
            break;
    }


}

#ifdef BOX_DET_USE_GPIO
#define BOX_DET_PIN HAL_IOMUX_PIN_P1_0

static void box_det_pin_irq_set(enum HAL_GPIO_IRQ_POLARITY_T polarity);

static void box_det_pin_irq_update(void)
{
    if (hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)BOX_DET_PIN))
        box_det_pin_irq_set(HAL_GPIO_IRQ_POLARITY_LOW_FALLING);
    else
        box_det_pin_irq_set(HAL_GPIO_IRQ_POLARITY_HIGH_RISING);
}

static void box_det_handler(uint32_t val)
{
    uint8_t nv_role = bts_tws_if_get_nv_role();
    EARBUDS_TRACE(0,"%s: %d", __func__, nv_role);
    box_det_pin_irq_update();
    if (val)
    {
        box_event = IBRT_FETCH_OUT_EVENT;
    }
    else
    {
        box_event=IBRT_CLOSE_BOX_EVENT;
    }

    if (nv_role == BT_IBRT_UNKNOWN)
        return;

    if (app_box_handle_timer)
    {
        osTimerStop(app_box_handle_timer);
        osTimerStart(app_box_handle_timer,500);
    }
}

static void box_det_irq_handler(enum HAL_GPIO_PIN_T pin)
{
    uint8_t val = hal_gpio_pin_get_val(pin);
    EARBUDS_TRACE(0,"%s: %d, %d", __func__, pin, val);
    app_ibrt_peripheral_run1((uint32_t)box_det_handler, (uint32_t)val);
}

static void box_det_pin_irq_set(enum HAL_GPIO_IRQ_POLARITY_T polarity)
{
    struct HAL_GPIO_IRQ_CFG_T box_det_pin_cfg;
    box_det_pin_cfg.irq_debounce = true;
    box_det_pin_cfg.irq_handler = box_det_irq_handler;
    box_det_pin_cfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;

    box_det_pin_cfg.irq_enable = true;
    box_det_pin_cfg.irq_polarity = polarity;
    hal_gpio_setup_irq((enum HAL_GPIO_PIN_T)BOX_DET_PIN, &box_det_pin_cfg);
}

static void box_det_pin_init(void)
{
    const struct HAL_IOMUX_PIN_FUNCTION_MAP box_det_gpio_cfg =
    {
        BOX_DET_PIN, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE
    };

    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&box_det_gpio_cfg, 1);
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)box_det_gpio_cfg.pin, HAL_GPIO_DIR_IN, 1);

    box_det_pin_irq_update();
}
#endif

void app_ibrt_search_ui_init(bool boxOperation, uint8_t boxEvent)
{
    app_bt_global_handle_hook_set(APP_BT_GOLBAL_HANDLE_HOOK_USER_0,app_bt_manager_ibrt_role_process);

    if((app_ui_get_config()->check_plugin_excute_closedbox_event) || (false==boxOperation))
    {
#ifdef BOX_DET_USE_GPIO
        box_det_pin_init();
#else
        app_battery_register(app_ibrt_battery_callback);
#endif

        if (app_box_handle_timer == NULL)
            app_box_handle_timer = osTimerCreate(osTimer(APP_BOX_HANDLE), osTimerOnce, &box_event);
    }
    else if((bta_tws_box_event_t)boxEvent != BTA_TWS_CLOSE)
    {
        bta_tws_enable_access_mode(true);
    }

}

void app_ibrt_remove_history_paired_device(void)
{
    bt_status_t            retStatus;
    btif_device_record_t   record;
    uint8_t *bt_local_addr = bts_tws_if_get_local_addr();
    uint8_t *bt_peer_addr = bts_tws_if_get_peer_addr();
    int paired_dev_count = nv_record_get_paired_dev_count();

    EARBUDS_TRACE(0,"Remove all history tws nv records.");
    EARBUDS_TRACE(0,"Master addr:");
    EARBUDS_DUMP8("%02x ", bt_local_addr, BT_ADDR_OUTPUT_PRINT_NUM);
    EARBUDS_TRACE(0,"Slave addr:");
    EARBUDS_DUMP8("%02x ", bt_peer_addr, BT_ADDR_OUTPUT_PRINT_NUM);

    for (int32_t index = paired_dev_count - 1; index >= 0; index--)
    {
        retStatus = nv_record_enum_dev_records(index, &record);
        if (BT_STS_SUCCESS == retStatus)
        {
            EARBUDS_TRACE(1,"The index %d of nv records:", index);
            EARBUDS_DUMP8("%02x ", record.bdAddr.address, BT_ADDR_OUTPUT_PRINT_NUM);
            if (!memcmp(record.bdAddr.address, bt_local_addr, BTIF_BD_ADDR_SIZE) ||
                !memcmp(record.bdAddr.address, bt_peer_addr, BTIF_BD_ADDR_SIZE))
            {
                nv_record_ddbrec_delete(&record.bdAddr);
                EARBUDS_TRACE(1,"Delete the nv record entry %d", index);
            }
        }
    }

    memset(bt_local_addr, 0, BTIF_BD_ADDR_SIZE);
    memset(bt_peer_addr, 0, BTIF_BD_ADDR_SIZE);
}



void app_ibrt_enter_limited_mode(void)
{
    bts_tws_if_update_nv_role(TWS_ROLE_UNKNOW);

    app_ibrt_remove_history_paired_device();
    EARBUDS_TRACE(0,"ibrt_ui_log:power on enter limited mode");
    bta_tws_enable_limited_mode(true);
}

void app_ibrt_exit_limited_mode(void)
{
    EARBUDS_TRACE(0,"ibrt_ui_log:exit limited mode");
    bta_tws_enable_limited_mode(false);
}

void app_ibrt_search_ui_config_load(void *config)
{
    struct nvrecord_env_t *nvrecord_env = NULL;
    ibrt_config_t *ibrt_config = (ibrt_config_t *)config;
    memset(ibrt_config, 0, sizeof(ibrt_config_t));

    EARBUDS_TRACE(0,"search ui: %s", __func__);

    nv_record_env_get(&nvrecord_env);
    ibrt_config->nv_role = nvrecord_env->ibrt_mode.mode;

    factory_section_original_btaddr_get(ibrt_config->local_addr.address);

#ifdef BESUI_TWS_EN
    memcpy(ibrt_config->peer_addr.address, nvrecord_env->ibrt_mode.record.bdAddr.address, BD_ADDR_LEN);
    if(besui_get_lr_sta() == LEFT_SIDE)
    {
        ibrt_config->audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_LCHNL;
        bts_tws_if_set_local_side(BT_LOCATION_LEFT);
    }
    else if(besui_get_lr_sta() == RIGHT_SIDE)
    {
        ibrt_config->audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_RCHNL;
        bts_tws_if_set_local_side(BT_LOCATION_RIGHT);
    }
#else
#ifdef IBRT_RIGHT_MASTER
    if (BT_IBRT_MASTER == ibrt_config->nv_role)
    {
        memcpy(ibrt_config->peer_addr.address, nvrecord_env->ibrt_mode.record.bdAddr.address, BD_ADDR_LEN);
        ibrt_config->audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_RCHNL;
        bts_tws_if_set_local_side(BT_LOCATION_RIGHT);
    }
    else if (BT_IBRT_SLAVE == ibrt_config->nv_role)
    {
        memcpy(ibrt_config->peer_addr.address, nvrecord_env->ibrt_mode.record.bdAddr.address, BD_ADDR_LEN);
        ibrt_config->audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_LCHNL;
        bts_tws_if_set_local_side(BT_LOCATION_LEFT);
    }
#else
    if (BT_IBRT_SLAVE == ibrt_config->nv_role)
    {
        memcpy(ibrt_config->peer_addr.address, nvrecord_env->ibrt_mode.record.bdAddr.address, BD_ADDR_LEN);
        ibrt_config->audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_RCHNL;
        bts_tws_if_set_local_side(BT_LOCATION_RIGHT);
    }
    else if (BT_IBRT_MASTER == ibrt_config->nv_role)
    {
        memcpy(ibrt_config->peer_addr.address, nvrecord_env->ibrt_mode.record.bdAddr.address, BD_ADDR_LEN);
        ibrt_config->audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_LCHNL;
        bts_tws_if_set_local_side(BT_LOCATION_LEFT);
    }
#endif
#endif
    else
    {
        ibrt_config->audio_chnl_sel =  A2DP_AUDIO_CHANNEL_SELECT_LRMERGE;
    }

    bts_core_set_ui_role(ibrt_config->nv_role);

    EARBUDS_TRACE(0,"current ibrt_mode.mode(nv_role)=%d ", ibrt_config->nv_role);
    EARBUDS_TRACE(0,"load local_addr: %02x:%02x:*:*:*:%02x",ibrt_config->local_addr.address[0],
        ibrt_config->local_addr.address[1], ibrt_config->local_addr.address[5]);
    EARBUDS_TRACE(0,"load peer_addr: %02x:%02x:*:*:*:%02x", ibrt_config->peer_addr.address[0],
        ibrt_config->peer_addr.address[1], ibrt_config->peer_addr.address[5]);
}

#endif


