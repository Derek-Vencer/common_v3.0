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
#ifndef __BESUI_DEFINE_H__
#define __BESUI_DEFINE_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(BESUI_TWS_EN) || defined(BESUI_STEREO_EN)
#undef  IBRT_UI_RECONNECT_MOBILE_WAIT_RESPONSE_TIMEOUT
#undef  IBRT_UI_DISABLE_BT_SCAN_TIMEOUT
#undef  IBRT_UI_DELAY_RECONN_MOBILE_MAX_TIMES
#undef  IBRT_UI_OPEN_RECONNECT_MOBILE_MAX_TIMES
#undef  IBRT_UI_OPEN_RECONNECT_TWS_MAX_TIMES
#undef  IBRT_UI_CLOSE_BOX_DEFAULT_DELAY_DISCONNECT_TWS_TIME

#define IBRT_UI_RECONNECT_MOBILE_WAIT_RESPONSE_TIMEOUT        (1000)//1s
#define IBRT_UI_DISABLE_BT_SCAN_TIMEOUT                       (320000)//5min
#define IBRT_UI_DELAY_RECONN_MOBILE_MAX_TIMES                 (0)
#define IBRT_UI_OPEN_RECONNECT_MOBILE_MAX_TIMES               (1)
#define IBRT_UI_OPEN_RECONNECT_TWS_MAX_TIMES                  (1)
#define IBRT_UI_CLOSE_BOX_DEFAULT_DELAY_DISCONNECT_TWS_TIME   (0)

//------------------------------------------------------------------------------
#if defined(CAPSENSOR_ENABLE)
#undef CAP_KEYDOWN_TH
#undef CAP_KEYUP_TH
#define CAP_KEYDOWN_TH                                         BESUI_TOUCH_KEYDOWN_TH  // key down
#define CAP_KEYUP_TH                                           BESUI_TOUCH_KEYUP_TH    // key up

#if !defined(BES_TWSPRO_EN)
#undef CAP_KEY_ULTRA_INTERVAL 
#define CAP_KEY_ULTRA_INTERVAL                                 (0)   // ultra  key interval
#endif
#endif

#ifdef BESUI_TOUCH_POWERON_EN
#undef CAP_SAMP_FS
#undef CAP_CHNUM
#undef CAP_KEY_LONGLONG_INTERVAL

#define CAP_SAMP_FS                                            (uictl.touch_poweron==1?510:51) //510  /* ms */
#define CAP_CHNUM                                              1
#define CAP_KEY_LONGLONG_INTERVAL                              (3000)  // 3000ms
#define CAP_KEY_LONG_INTERVAL2                                 (100) //for tpuch poweron
#endif

#if defined(BESUI_COMM_EN) && defined(ANC_APP)
#undef ANC_COEF_LIST_NUM
#define ANC_COEF_LIST_NUM                   BESUI_ANC_COEF_LIST_NUM

#undef ANC_FF_MIC_CH_L
#undef ANC_FB_MIC_CH_L
#undef ANC_TALK_MIC_CH_L
#define ANC_FF_MIC_CH_L                     BESUI_ANC_FF_MIC_CH_L
#define ANC_FB_MIC_CH_L                     BESUI_ANC_FB_MIC_CH_L
#define ANC_TALK_MIC_CH_L                   BESUI_ANC_TALK_MIC_CH_L
#endif

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#ifdef BESUI_GFPS_ID_EN
//----------------------------------------------------------------------------------------------------
#if defined(BES_TWSPRO_EN) || defined(BES_NOTWS_EN)
#define USER_GFPS_MODEL_ID          0x2B677D
#define USER_GFPS_PUBLIC_KEY        {0x3E, 0x08, 0x3B, 0x0A, 0x5C, 0x04, 0x78, 0x84, 0xBE, 0x41, \
                                     0xBE, 0x7E, 0x52, 0xD1, 0x0C, 0x68, 0x64, 0x6C, 0x4D, 0xB6, \
                                     0xD9, 0x20, 0x95, 0xA7, 0x32, 0xE9, 0x42, 0x40, 0xAC, 0x02, \
                                     0x54, 0x48, 0x99, 0x49, 0xDA, 0xE1, 0x0D, 0x9C, 0xF5, 0xEB, \
                                     0x29, 0x35, 0x7F, 0xB1, 0x70, 0x55, 0xCB, 0x8C, 0x8F, 0xBF, \
                                     0xEB, 0x17, 0x15, 0x3F, 0xA0, 0xAA, 0xA5, 0xA2, 0xC4, 0x3C, \
                                     0x1B, 0x48, 0x60, 0xDA} 
#define USER_GFPS_PRIVATE_KEY       {0xCD, 0xF8, 0xAA, 0xC0, 0xDF, 0x4C, 0x93, 0x63, 0x2F, 0x48, \
                                     0x20, 0xA6, 0xD8, 0xAB, 0x22, 0xF3, 0x3A, 0x94, 0xBF, 0x8E, \
                                     0x4C, 0x90, 0x25, 0xB3, 0x44, 0xD2, 0x2E, 0xDE, 0x0F, 0xB7, \
                                     0x22, 0x1F}
//----------------------------------------------------------------------------------------------------
#endif
#endif //#ifdef BESUI_GFPS_ID_EN

#ifdef BESUI_FIX_GFPS_MODELID_EN
#define USER_GFPS_MODEL_ID          0x2B677D //bes model id
#define USER_GFPS_PUBLIC_KEY        {0x3E, 0x08, 0x3B, 0x0A, 0x5C, 0x04, 0x78, 0x84, 0xBE, 0x41, \
                                     0xBE, 0x7E, 0x52, 0xD1, 0x0C, 0x68, 0x64, 0x6C, 0x4D, 0xB6, \
                                     0xD9, 0x20, 0x95, 0xA7, 0x32, 0xE9, 0x42, 0x40, 0xAC, 0x02, \
                                     0x54, 0x48, 0x99, 0x49, 0xDA, 0xE1, 0x0D, 0x9C, 0xF5, 0xEB, \
                                     0x29, 0x35, 0x7F, 0xB1, 0x70, 0x55, 0xCB, 0x8C, 0x8F, 0xBF, \
                                     0xEB, 0x17, 0x15, 0x3F, 0xA0, 0xAA, 0xA5, 0xA2, 0xC4, 0x3C, \
                                     0x1B, 0x48, 0x60, 0xDA} 
#define USER_GFPS_PRIVATE_KEY       {0xCD, 0xF8, 0xAA, 0xC0, 0xDF, 0x4C, 0x93, 0x63, 0x2F, 0x48, \
                                     0x20, 0xA6, 0xD8, 0xAB, 0x22, 0xF3, 0x3A, 0x94, 0xBF, 0x8E, \
                                     0x4C, 0x90, 0x25, 0xB3, 0x44, 0xD2, 0x2E, 0xDE, 0x0F, 0xB7, \
                                     0x22, 0x1F}
#endif //#ifdef BESUI_GFPS_ID_EN

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#ifdef BESUI_TOUCH_POWERON_EN
#define HAL_SW_BOOTMODE_TOUCH_POWERON           (1 << 29)
#endif


#if 1
//tws cmd
enum{
   USER_TWS_CMD_INVALID = 0,
   USER_TWS_CMD_ENTER_PAIRMODE,
   USER_TWS_CMD_SLAVE_LINKLOSS,
   USER_TWS_CMD_RANDOM,
   USER_TWS_CMD_CLEAR_PHONE_NV,
#if defined(BESUI_FMD_UI_EN)
   USER_TWS_CMD_FMD_ADV,
#endif
   USER_TWS_CMD_TEST_EQTUNE,
   USER_TWS_CMD_MAX,
};

//msg cmd
enum{
   USER_MSG_CMD_INVALID = 0,
   USER_MSG_CMD_2BUDS_CLOSE,
   USER_MSG_CMD_LED_STA_NEXT,
   USER_MSG_CMD_WEAR_PROMPT,
   USER_MSG_CMD_CLEAR_PHONE_NV,
   USER_MSG_CMD_AI,
   USER_MSG_CMD_AI_KWS,

   USER_MSG_CMD_TEST_EQTUNE,
   USER_MSG_CMD_MAX,
};

#define TWS_CONNECTED_EVENT                             0x01
#define TWS_DISCONNECTED_EVENT                          0x02
#define PHONE_CONNECTED_EVENT                           0x03
#define PHONE_DISCONECTED_EVENT                         0x04
#define ENTER_PAIRMODE_EVENT                            0x05
#define EXIT_PHONEMODE_EVENT                            0x06
#define OPENRECONNECTED_TIMEOUT_ENTER_PAIRMODE_EVENT    0x07
#define RECONNECTED_TIMEOUT_ENTER_PAIRMODE_EVENT        0x08
#define TIMEROUT_POWEROFF_EVENT                         0x09
#define PHONE_CANCEL_DISCONNECTED_EVENT                 0x0a

#define SLAVE_ENTER_PAIRMODE_EVENT                      0x0b
#define SLAVE_LINKLOSS_EVENT                            0x0c
#define AUTH_START_EVENT                                0x0d
#define AUTH_SUCCESS_EVENT                              0x0e
#define TWS_STATUS_EVENT                                0x0f
#define BT_SINGLE_PAIRMODE                              0x10
#define BT_MSG_SOMETHING_EVENT                          0x11

#define CHARGE_PLUGIN                                   0x01
#define CHARGE_PLUGOUT                                  0x02
#define PUTIN_BOX                                       0x03
#define PUTOUT_BOX                                      0x04
#define WEAR_UP                                         0x05
#define WEAR_DOWN                                       0x06
#define PUTOUT_PUTIN_DETECT_EVENT                       0x07
#define POWERON_EVENT                                   0x08
#define NV_RECORD_FLUSH_EVENT                           0x09

#define AUTO_POWEROFF                                   0x01
#define LOWBATTERY_POWEROFF                             0x02
#define KEY_FUNCTION_POWEROFF                           0x03
#define CLEARPAIRLIST_POWEROFF                          0x04

#define DEFAULT_TYPE                                    0xff
#define END_CALL_TYPE                                   0x01
#define REJECT_CALL_TYPE                                0x02

#define DISCONNECTED_TYPE                               0x01
#define HANGUP_REJECT_TYPE                              0x02
#define AUDIO_CONNECTED_NOCALL                          0x03

#define INEAR_STATUS                                    0x01
#define OUTEAR_STATUS                                   0x02

#define UNKNOW_SIDE                                     0xff
#define LEFT_SIDE                                       0x00
#define RIGHT_SIDE                                      0x01

#define UNKNOW_STATUS                                   0xff
#define PUTINBOX_STATUS                                 0x03
#define PUTOUTBOX_STATUS                                0x04

#endif

#endif //#if defined(BESUI_TWS_EN) || defined(BESUI_STEREO_EN)

#ifdef __cplusplus
}
#endif

#endif//__BESUI_DEFINE_H__
