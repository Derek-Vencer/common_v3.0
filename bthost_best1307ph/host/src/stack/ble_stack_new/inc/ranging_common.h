/***************************************************************************
 *
 * Copyright (c) 2015-2024 BES Technic
 *
 * Authored by BES CD team (Blueelf Prj).
 *
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
#ifndef __RANGING_COMMON_H__
#define __RANGING_COMMON_H__

#include "bt_sys_config.h"
#include "bluetooth.h"
#include "gatt_service.h"
#include "cs_service.h"
#include "ras_common.h"
#include "cobuf.h"

// * lera common status
#define LERA_STS_SHOW_INFO                                  (0x000)
#define LERA_STS_MGR_INIT                                   (0x001)
#define LERA_STS_MGR_DEINIT                                 (0x002)
#define LERA_STS_CALLBACK_REGISTER                          (0x003)
#define LERA_STS_CALLBACK_DEREGISTER                        (0x004)
#define LERA_STS_SERVICE_CONNECTED                          (0x005)
#define LERA_STS_SERVICE_DISCONNECTED                       (0x006)
#define LERA_STS_CONN_INFO_INIT                             (0x007)
#define LERA_STS_CONN_INFO_DEINIT                           (0x008)
#define LERA_STS_SEND_DATA                                  (0x009)
#define LERA_STS_RECV_DATA                                  (0x00A)
#define LERA_STS_REPORT_EVENT                               (0x00B)
#define LERA_STS_REPORT_EXCEPTION                           (0x00C)
#define LERA_STS_CONN_OPENED                                (0x00D)
#define LERA_STS_CONN_CLOSED                                (0x00E)
#define LERA_STS_ATT_MTU_CHANGED                            (0x00F)
#define LERA_STS_CONN_ENCRYPTED                             (0x010)
#define LERA_STS_CONN_UPDATED                               (0x011)
#define LERA_STS_TIMER_START                                (0x012)
#define LERA_STS_TIMER_RESTART                              (0x013)
#define LERA_STS_TIMER_STOP                                 (0x014)
#define LERA_COMMON_INFO_MAX_NUM                            (0x040)

#define LERA_CUSTOM_INFO_MAX_NUM                            (0x080)
#define LERA_CUSTOM_ERROR_BASE                              (LERA_COMMON_INFO_MAX_NUM + LERA_CUSTOM_INFO_MAX_NUM)

#define LERA_STS_ERROR_UNEXPECTED_CASE                      (LERA_CUSTOM_ERROR_BASE + 0x000)
#define LERA_STS_ERROR_NO_CONN_INFO                         (LERA_CUSTOM_ERROR_BASE + 0x001)
#define LERA_STS_ERROR_NO_CALLBACK                          (LERA_CUSTOM_ERROR_BASE + 0x002)
#define LERA_STS_ERROR_ALLOC_FAILED                         (LERA_CUSTOM_ERROR_BASE + 0x003)
#define LERA_STS_ERROR_INVALID_PARAMETER                    (LERA_CUSTOM_ERROR_BASE + 0x004)
#define LERA_STS_ERROR_UNKNOWN_CMD                          (LERA_CUSTOM_ERROR_BASE + 0x005)
#define LERA_STS_ERROR_UNKNOWN_EVENT                        (LERA_CUSTOM_ERROR_BASE + 0x006)
#define LERA_STS_ERROR_REPEAT_INIT                          (LERA_CUSTOM_ERROR_BASE + 0x007)
#define LERA_STS_ERROR_REPEAT_DEINIT                        (LERA_CUSTOM_ERROR_BASE + 0x008)
#define LERA_STS_ERROR_ENV_NOT_READY                        (LERA_CUSTOM_ERROR_BASE + 0x009)
#define LERA_STS_ERROR_EMPTY_POINTER                        (LERA_CUSTOM_ERROR_BASE + 0x00A)
#define LERA_STS_ERROR_INVALID_CONNHDL                      (LERA_CUSTOM_ERROR_BASE + 0x00B)
#define LERA_STS_ERROR_INVALID_CONIDX                       (LERA_CUSTOM_ERROR_BASE + 0x00C)
#define LERA_STS_ERROR_OUT_OF_MEMORY                        (LERA_CUSTOM_ERROR_BASE + 0x00D)
#define LERA_COMMON_ERROR_MAX_NUM                           (0x040)

#define LERA_CS_CUSTOM_INFO_MAX_NUM                         (0x80)
#define LERA_RAP_CUSTOM_INFO_MAX_NUM                        (0x80)
#define LERA_RAPS_CUSTOM_INFO_MAX_NUM                       (0x80)
#define LERA_RAPC_CUSTOM_INFO_MAX_NUM                       (0x80)

// * base offset
#define BT_STS_CS_INFO_BASE                                 (LERA_COMMON_INFO_MAX_NUM)
#define BT_STS_RAP_INFO_BASE                                (LERA_COMMON_INFO_MAX_NUM)
#define BT_STS_RAPS_INFO_BASE                               (LERA_COMMON_INFO_MAX_NUM)
#define BT_STS_RAPC_INFO_BASE                               (LERA_COMMON_INFO_MAX_NUM)
#define BT_STS_CS_ERROR_BASE                                (BT_STS_CS_INFO_BASE   + LERA_CS_CUSTOM_INFO_MAX_NUM   + LERA_COMMON_ERROR_MAX_NUM)
#define BT_STS_RAP_ERROR_BASE                               (BT_STS_RAP_INFO_BASE  + LERA_RAP_CUSTOM_INFO_MAX_NUM  + LERA_COMMON_ERROR_MAX_NUM)
#define BT_STS_RAPS_ERROR_BASE                              (BT_STS_RAPS_INFO_BASE + LERA_RAPS_CUSTOM_INFO_MAX_NUM + LERA_COMMON_ERROR_MAX_NUM)
#define BT_STS_RAPC_ERROR_BASE                              (BT_STS_RAPC_INFO_BASE + LERA_RAPS_CUSTOM_INFO_MAX_NUM + LERA_COMMON_ERROR_MAX_NUM)

// * cs custom status
#define BT_STS_CS_SHOW_INFO                                 (LERA_STS_SHOW_INFO)
#define BT_STS_CS_MGR_INIT                                  (LERA_STS_MGR_INIT)
#define BT_STS_CS_MGR_DEINIT                                (LERA_STS_MGR_DEINIT)
#define BT_STS_CS_REGISTER_CALLBACK                         (LERA_STS_CALLBACK_REGISTER)
#define BT_STS_CS_CONNECTED                                 (LERA_STS_SERVICE_CONNECTED)
#define BT_STS_CS_DISCONNECTED                              (LERA_STS_SERVICE_DISCONNECTED)
#define BT_STS_CS_CONN_INFO_INIT                            (LERA_STS_CONN_INFO_INIT)
#define BT_STS_CS_CONN_INFO_DEINIT                          (LERA_STS_CONN_INFO_DEINIT)

#define BT_STS_CS_TEST_START                                (BT_STS_CS_INFO_BASE + 0x001)
#define BT_STS_CS_TEST_END                                  (BT_STS_CS_INFO_BASE + 0x002)
#define BT_STS_CS_READ_LOCAL_CAPAS                          (BT_STS_CS_INFO_BASE + 0x003)
#define BT_STS_CS_READ_REMOTE_CAPAS                         (BT_STS_CS_INFO_BASE + 0x004)
#define BT_STS_CS_WRITE_CACHED_REMOTE_CAPAS                 (BT_STS_CS_INFO_BASE + 0x005)
#define BT_STS_CS_SET_DEFAULT_SETTINGS                      (BT_STS_CS_INFO_BASE + 0x006)
#define BT_STS_CS_READ_REMOTE_FAE_TABLE                     (BT_STS_CS_INFO_BASE + 0x007)
#define BT_STS_CS_WRITE_CACHED_REMOTE_FAE_TABLE             (BT_STS_CS_INFO_BASE + 0x008)
#define BT_STS_CS_CREATE_CONFIG                             (BT_STS_CS_INFO_BASE + 0x009)
#define BT_STS_CS_REMOVE_CONFIG                             (BT_STS_CS_INFO_BASE + 0x00A)
#define BT_STS_CS_SECURITY_ENABLE                           (BT_STS_CS_INFO_BASE + 0x00B)
#define BT_STS_CS_SET_PROC_PARAMS                           (BT_STS_CS_INFO_BASE + 0x00C)
#define BT_STS_CS_ENABLE_PROCEDURE                          (BT_STS_CS_INFO_BASE + 0x00D)
#define BT_STS_CS_DISABLE_PROCEDURE                         (BT_STS_CS_INFO_BASE + 0x00E)
#define BT_STS_CS_RECV_SUBEVENT_RESULT                      (BT_STS_CS_INFO_BASE + 0x00F)
#define BT_STS_CS_RECV_SUBEVENT_RESULT_CONTINUE             (BT_STS_CS_INFO_BASE + 0x010)
#define BT_STS_CS_SET_CHANNEL_CLASSIFICATION                (BT_STS_CS_INFO_BASE + 0x011)
#define BT_STS_CS_CMD_EXEC_STATUS                           (BT_STS_CS_INFO_BASE + 0x012)
#define BT_STS_CS_READ_LOCAL_CAPAS_CMPL                     (BT_STS_CS_INFO_BASE + 0x013)
#define BT_STS_CS_READ_REMOTE_CAPAS_CMPL                    (BT_STS_CS_INFO_BASE + 0x014)
#define BT_STS_CS_WRITE_CACHED_REMOTE_CAPAS_CMPL            (BT_STS_CS_INFO_BASE + 0x015)
#define BT_STS_CS_SET_DEFAULT_SETTINGS_CMPL                 (BT_STS_CS_INFO_BASE + 0x016)
#define BT_STS_CS_READ_REMOTE_FAE_TABLE_CMPL                (BT_STS_CS_INFO_BASE + 0x017)
#define BT_STS_CS_WRITE_CACHED_REMOTE_FAE_TABLE_CMPL        (BT_STS_CS_INFO_BASE + 0x018)
#define BT_STS_CS_CREATE_CONFIG_CMPL                        (BT_STS_CS_INFO_BASE + 0x019)
#define BT_STS_CS_REMOVE_CONFIG_CMPL                        (BT_STS_CS_INFO_BASE + 0x020)
#define BT_STS_CS_SECURITY_ENABLE_CMPL                      (BT_STS_CS_INFO_BASE + 0x021)
#define BT_STS_CS_SET_PROC_PARAMS_CMPL                      (BT_STS_CS_INFO_BASE + 0x022)
#define BT_STS_CS_PROC_ENABLE_CMPL                          (BT_STS_CS_INFO_BASE + 0x023)
#define BT_STS_CS_PROC_DISABLE_CMPL                         (BT_STS_CS_INFO_BASE + 0x024)
#define BT_STS_CS_RECV_SUBEVENT_RESULT_CMPL                 (BT_STS_CS_INFO_BASE + 0x025)
#define BT_STS_CS_RECV_SUBEVENT_RESULT_CONTINUE_CMPL        (BT_STS_CS_INFO_BASE + 0x026)
#define BT_STS_CS_SET_CHANNEL_CLASSIFICATION_CMPL           (BT_STS_CS_INFO_BASE + 0x027)
#define BT_STS_CS_ADD_PENDING_CONFIG_REMOVAL                (BT_STS_CS_INFO_BASE + 0x028)
#define BT_STS_CS_PERFORM_PENDING_CONFIG_REMOVAL            (BT_STS_CS_INFO_BASE + 0x029)
#define BT_STS_CS_GEN_CONTINUOUS_SUBEVENT_RESULT            (BT_STS_CS_INFO_BASE + 0x02A)
#define BT_STS_CS_FREE_ALL_CONN_CONFIG                      (BT_STS_CS_INFO_BASE + 0x02B)
#define BT_STS_CS_FREE_INNER_SUB_RES_STEP_PARAMS            (BT_STS_CS_INFO_BASE + 0x02C)
#define BT_STS_CS_FREE_PENDING_REMOVAL_CONFIG               (BT_STS_CS_INFO_BASE + 0x02D)
#define BT_STS_CS_LOCAL_SUPP_CAPAS_CHANGED                  (BT_STS_CS_INFO_BASE + 0x02E)
#define BT_STS_CS_ADD_NEW_CONN_CONFIG                       (BT_STS_CS_INFO_BASE + 0x02F)
#define BT_STS_CS_UPDATE_CONN_CONFIG                        (BT_STS_CS_INFO_BASE + 0x030)
#define BT_STS_CS_DEL_CONN_CONFIG                           (BT_STS_CS_INFO_BASE + 0x031)
#define BT_STS_CS_PROC_STATE_TRANSFER                       (BT_STS_CS_INFO_BASE + 0x032)
#define BT_STS_CS_CONFIG_CHECK                              (BT_STS_CS_INFO_BASE + 0x033)
#define BT_STS_CS_PRESET                                    (BT_STS_CS_INFO_BASE + 0x034)

#define BT_STS_CS_ERROR_UNEXPECTED_CASE                     (LERA_STS_ERROR_UNEXPECTED_CASE)
#define BT_STS_CS_ERROR_NO_CONN_INFO                        (LERA_STS_ERROR_NO_CONN_INFO)
#define BT_STS_CS_ERROR_NO_CALLBACK                         (LERA_STS_ERROR_NO_CALLBACK)
#define BT_STS_CS_ERROR_ALLOC_FAILED                        (LERA_STS_ERROR_ALLOC_FAILED)
#define BT_STS_CS_ERROR_INVALID_PARAMETER                   (LERA_STS_ERROR_INVALID_PARAMETER)
#define BT_STS_CS_ERROR_UNKNOWN_CMD                         (LERA_STS_ERROR_UNKNOWN_CMD)
#define BT_STS_CS_ERROR_UNKNOWN_EVENT                       (LERA_STS_ERROR_UNKNOWN_EVENT)
#define BT_STS_CS_ERROR_REPEAT_INIT                         (LERA_STS_ERROR_REPEAT_INIT)

#define BT_STS_CS_ERROR_READ_LOCAL_CAPAS_FAILED             (BT_STS_CS_ERROR_BASE + 0x001)
#define BT_STS_CS_ERROR_READ_REMOTE_CAPAS_FAILED            (BT_STS_CS_ERROR_BASE + 0x002)
#define BT_STS_CS_ERROR_WRITE_CACHED_REMOTE_CAPAS_FAILED    (BT_STS_CS_ERROR_BASE + 0x003)
#define BT_STS_CS_ERROR_SET_DEFAULT_SETTINGS_FAILED         (BT_STS_CS_ERROR_BASE + 0x004)
#define BT_STS_CS_ERROR_READ_REMOTE_FAE_TABLE_FAILED        (BT_STS_CS_ERROR_BASE + 0x005)
#define BT_STS_CS_ERROR_WRITE_CACHED_FAE_TABLE_FAILED       (BT_STS_CS_ERROR_BASE + 0x006)
#define BT_STS_CS_ERROR_CREATE_CONFIG_FAILED                (BT_STS_CS_ERROR_BASE + 0x007)
#define BT_STS_CS_ERROR_REMOVE_CONFIG_FAILED                (BT_STS_CS_ERROR_BASE + 0x008)
#define BT_STS_CS_ERROR_SECURITY_ENABLE_FAILED              (BT_STS_CS_ERROR_BASE + 0x009)
#define BT_STS_CS_ERROR_SET_PROC_PARAMS_FAILED              (BT_STS_CS_ERROR_BASE + 0x00A)
#define BT_STS_CS_ERROR_PROC_ENABLE_FAILED                  (BT_STS_CS_ERROR_BASE + 0x00B)
#define BT_STS_CS_ERROR_PROC_DISABLE_FAILED                 (BT_STS_CS_ERROR_BASE + 0x00C)
#define BT_STS_CS_ERROR_RECV_SUB_RES_FAILED                 (BT_STS_CS_ERROR_BASE + 0x00D)
#define BT_STS_CS_ERROR_RECV_SUB_RES_CONTINUE_FAILED        (BT_STS_CS_ERROR_BASE + 0x00E)
#define BT_STS_CS_ERROR_SET_CHANNEL_CLASSIFICATION_FAILED   (BT_STS_CS_ERROR_BASE + 0x00F)
#define BT_STS_CS_ERROR_TEST_FAILED                         (BT_STS_CS_ERROR_BASE + 0x010)
#define BT_STS_CS_ERROR_UNKNOWN_SPEC_INFO                   (BT_STS_CS_ERROR_BASE + 0x011)
#define BT_STS_CS_ERROR_INVALID_CONFIG_ID                   (BT_STS_CS_ERROR_BASE + 0x012)
#define BT_STS_CS_ERROR_CONFIG_NOT_FOUND                    (BT_STS_CS_ERROR_BASE + 0x013)
#define BT_STS_CS_ERROR_UPDATE_CONFIG_WHEN_PROC_ACTIVE      (BT_STS_CS_ERROR_BASE + 0x014)
#define BT_STS_CS_ERROR_ADD_EXISTED_CONFIG                  (BT_STS_CS_ERROR_BASE + 0x015)
#define BT_STS_CS_ERROR_LOCAL_SUPP_CAPAS_NOT_RECEIVED       (BT_STS_CS_ERROR_BASE + 0x016)
#define BT_STS_CS_ERROR_REMOTE_SUPP_CAPAS_NOT_RECEIVED      (BT_STS_CS_ERROR_BASE + 0x017)
#define BT_STS_CS_ERROR_SPBR_RTT_CONFLICT                   (BT_STS_CS_ERROR_BASE + 0x018)
#define BT_STS_CS_ERROR_PROC_PARAMS_NOT_SET                 (BT_STS_CS_ERROR_BASE + 0x019)
#define BT_STS_CS_ERROR_REJECT_REPEAT_PROC_ENABLE           (BT_STS_CS_ERROR_BASE + 0x01A)
#define BT_STS_CS_ERROR_LOCAL_CAPAS_STILL_NOT_RECEIVED      (BT_STS_CS_ERROR_BASE + 0x01B)
#define BT_STS_CS_ERROR_REMOTE_CAPAS_STILL_NOT_RECEIVED     (BT_STS_CS_ERROR_BASE + 0x01C)
#define BT_STS_CS_ERROR_CONFIG_NOT_CREATED                  (BT_STS_CS_ERROR_BASE + 0x01D)
#define BT_STS_CS_ERROR_DUPLICATED_CONFIG_REMOVAL           (BT_STS_CS_ERROR_BASE + 0x01E)
#define BT_STS_CS_ERROR_CONFIG_NO_CHANGE_WILL_NOT_REFRESH   (BT_STS_CS_ERROR_BASE + 0x01F)
#define BT_STS_CS_ERROR_NOt_SUPPORT                         (BT_STS_CS_ERROR_BASE + 0x020)

// * ras custom status
#define BT_STS_RAP_SHOW_INFO                                (LERA_STS_SHOW_INFO)
#define BT_STS_RAP_MGR_INIT                                 (LERA_STS_MGR_INIT)
#define BT_STS_RAP_MGR_DEINIT                               (LERA_STS_MGR_DEINIT)
#define BT_STS_RAP_ALLOC_FAILED                             (LERA_STS_ERROR_ALLOC_FAILED)
#define BT_STS_RAP_ERROR_REPEAT_INIT                        (LERA_STS_ERROR_REPEAT_INIT)
#define BT_STS_RAP_ERROR_REPEAT_DEINIT                      (LERA_STS_ERROR_REPEAT_DEINIT)

#define BT_STS_RAS_SHOW_INFO                                (LERA_STS_SHOW_INFO)
#define BT_STS_RAS_MGR_INIT                                 (LERA_STS_MGR_INIT)
#define BT_STS_RAS_MGR_DEINIT                               (LERA_STS_MGR_DEINIT)
#define BT_STS_RAS_CONNECTED                                (LERA_STS_SERVICE_CONNECTED)
#define BT_STS_RAS_DISCONNECTED                             (LERA_STS_SERVICE_DISCONNECTED)
#define BT_STS_RAS_CONN_MTU_CHANGED                         (LERA_STS_ATT_MTU_CHANGED)
#define BT_STS_RAS_CONN_ENCRYPTED                           (LERA_STS_CONN_ENCRYPTED)
#define BT_STS_RAS_EXEC_FILTER                              (BT_STS_RAP_INFO_BASE + 0x001)
#define BT_STS_RAS_WRITE_CONTROL_POINT                      (BT_STS_RAP_INFO_BASE + 0x002)
#define BT_STS_RAS_GATT_SERVER_CALLBACK                     (BT_STS_RAP_INFO_BASE + 0x003)
#define BT_STS_RAS_WRITE_RTRD_CCCD                          (BT_STS_RAP_INFO_BASE + 0x004)
#define BT_STS_RAS_WRITE_ODRD_CCCD                          (BT_STS_RAP_INFO_BASE + 0x005)
#define BT_STS_RAS_WRITE_RD_READY_CCCD                      (BT_STS_RAP_INFO_BASE + 0x006)
#define BT_STS_RAS_WRITE_RD_OW_CCCD                         (BT_STS_RAP_INFO_BASE + 0x007)
#define BT_STS_RAS_SERVICE_REGISTER_CMPL                    (BT_STS_RAP_INFO_BASE + 0x008)

#define BT_STS_RAS_NO_UPPER_CALLBACK                        (LERA_STS_ERROR_NO_CALLBACK)
#define BT_STS_RAS_ERROR_ALLOC_FAILED                       (LERA_STS_ERROR_ALLOC_FAILED)
#define BT_STS_RAS_ENV_NOT_READY                            (LERA_STS_ERROR_ENV_NOT_READY)
#define BT_STS_RAS_EMPTY_POINTER                            (LERA_STS_ERROR_EMPTY_POINTER)
#define BT_STS_RAS_INVALID_CONNHDL                          (LERA_STS_ERROR_INVALID_CONNHDL)
#define BT_STS_RAS_ERROR_UNKNOWN_CHARACTERISTIC             (BT_STS_RAP_ERROR_BASE + 0x001)
#define BT_STS_RAS_ERROR_UNKNOWN_DESCRIPTOR                 (BT_STS_RAP_ERROR_BASE + 0x002)
#define BT_STS_RAS_READ_CHARACTERISTIC                      (BT_STS_RAP_ERROR_BASE + 0x002)
#define BT_STS_RAS_REPORT_EXCEPTION                         (BT_STS_RAP_ERROR_BASE + 0x002)

// * raps custom status
#define BT_STS_RAPS_SHOW                                    (LERA_STS_SHOW_INFO)
#define BT_STS_RAPS_MGR_INIT                                (LERA_STS_MGR_INIT)
#define BT_STS_RAPS_MGR_DEINIT                              (LERA_STS_MGR_DEINIT)
#define BT_STS_RAPS_UPPER_CALLBACK_REGISTER                 (LERA_STS_CALLBACK_REGISTER)
#define BT_STS_RAPS_CONN_OPENED                             (LERA_STS_CONN_OPENED)
#define BT_STS_RAPS_CONN_CLOSED                             (LERA_STS_CONN_CLOSED)
#define BT_STS_RAPS_MTU_CHANGED                             (LERA_STS_ATT_MTU_CHANGED)
#define BT_STS_RAPS_CONN_ENCRYPTED                          (LERA_STS_CONN_ENCRYPTED)
#define BT_STS_RAPS_DELETE_INFO                             (LERA_STS_CONN_INFO_DEINIT)

#define BT_STS_RAPS_DELETE_ALL_INFO                         (BT_STS_RAPS_INFO_BASE + 0x001)
#define BT_STS_RAPS_READ_CHARACTERISTIC_VALUE               (BT_STS_RAPS_INFO_BASE + 0x002)
#define BT_STS_RAPS_READ_RAS_FEATURES                       (BT_STS_RAPS_INFO_BASE + 0x002)
#define BT_STS_RAPS_READ_RANGING_DATA_OVERWRITTEN           (BT_STS_RAPS_INFO_BASE + 0x003)
#define BT_STS_RAPS_READ_RANGING_DATA_READY                 (BT_STS_RAPS_INFO_BASE + 0x004)
#define BT_STS_RAPS_CONTROL_POINT_CMD                       (BT_STS_RAPS_INFO_BASE + 0x005)
#define BT_STS_RAPS_CONTROL_POINT_RSP                       (BT_STS_RAPS_INFO_BASE + 0x006)
#define BT_STS_RAPS_READ_CHAR_CCCD                          (BT_STS_RAPS_INFO_BASE + 0x007)
#define BT_STS_RAPS_WRITE_CHAR_CCCD                         (BT_STS_RAPS_INFO_BASE + 0x008)
#define BT_STS_RAPS_SEND_ON_DEMAND_RANGING_DATA             (BT_STS_RAPS_INFO_BASE + 0x009)
#define BT_STS_RAPS_ON_DEMAND_RANGING_DATA_TX_DONE          (BT_STS_RAPS_INFO_BASE + 0x00A)

#define BT_STS_RAPS_RECV_SUBEVENT_RESULT                    (BT_STS_RAPS_INFO_BASE + 0x00B)
#define BT_STS_RAPS_RTM_RECV_SUBEVENT_RESULT                (BT_STS_RAPS_INFO_BASE + 0x00C)
#define BT_STS_RAPS_ODM_RECV_SUBEVENT_RESULT                (BT_STS_RAPS_INFO_BASE + 0x00D)
#define BT_STS_RAPS_STORE_FILTER_CFG_IN_NV                  (BT_STS_RAPS_INFO_BASE + 0x00E)

#define BT_STS_RAPS_READ_RAD_READY_CCCD                     (BT_STS_RAPS_INFO_BASE + 0x00F)
#define BT_STS_RAPS_WRITE_RAD_READY_CCCD                    (BT_STS_RAPS_INFO_BASE + 0x010)

#define BT_STS_RAPS_ODM_CLEAR_RAD                           (BT_STS_RAPS_INFO_BASE + 0x011)
#define BT_STS_RAPS_ODM_REMOVE_RAD                          (BT_STS_RAPS_INFO_BASE + 0x012)
#define BT_STS_RAPS_ODM_RAD_BUF_UPDATE                      (BT_STS_RAPS_INFO_BASE + 0x013)
#define BT_STS_RAPS_W4_ACK_RAD_TIMER_START                  (BT_STS_RAPS_INFO_BASE + 0x014)
#define BT_STS_RAPS_W4_ACK_RAD_TIMER_RESTART                (BT_STS_RAPS_INFO_BASE + 0x015)
#define BT_STS_RAPS_W4_ACK_RAD_TIMER_STOP                   (BT_STS_RAPS_INFO_BASE + 0x016)

#define BT_STS_RAPS_NTF                                     (BT_STS_RAPS_INFO_BASE + 0x017)
#define BT_STS_RAPS_NTF_TX_DONE                             (BT_STS_RAPS_INFO_BASE + 0x018)
#define BT_STS_RAPS_IND                                     (BT_STS_RAPS_INFO_BASE + 0x019)
#define BT_STS_RAPS_IND_CFM                                 (BT_STS_RAPS_INFO_BASE + 0x01A)

#define BT_STS_RAPS_ODM_RAD_RETREIVE_CMPL_IND               (BT_STS_RAPS_INFO_BASE + 0x020)
#define BT_STS_RAPS_ODM_CP_CMPL_LOST_RAD                    (BT_STS_RAPS_INFO_BASE + 0x021)
#define BT_STS_RAPS_ODM_SEND_RAD_READY                      (BT_STS_RAPS_INFO_BASE + 0x022)
#define BT_STS_RAPS_ODM_RAD_BUF_Q_EMPTY                     (BT_STS_RAPS_INFO_BASE + 0x023)
#define BT_STS_RAPS_ODM_RAD_ABORT                           (BT_STS_RAPS_INFO_BASE + 0x024)
#define BT_STS_RAPS_GEN_RANGING_HEADER                      (BT_STS_RAPS_INFO_BASE + 0x025)

//  RAP server Error code
#define BT_STS_RAPS_ERROR_UNEXPECTED                        (LERA_STS_ERROR_UNEXPECTED_CASE)
#define BT_STS_RAPS_ERROR_NO_CONN_INFO                      (LERA_STS_ERROR_NO_CONN_INFO)
#define BT_STS_RAPS_ERROR_NO_UPPER_CALLBACK                 (LERA_STS_ERROR_NO_CALLBACK)
#define BT_STS_RAPS_ERROR_INVALID_PARAM                     (LERA_STS_ERROR_INVALID_PARAMETER)
#define BT_STS_RAPS_ERROR_MGR_NOT_READY                     (LERA_STS_ERROR_ENV_NOT_READY)
#define BT_STS_RAPS_ERROR_REPEAT_INIT                       (LERA_STS_ERROR_REPEAT_INIT)
#define BT_STS_RAPS_ERROR_INVALID_CONNHDL                   (LERA_STS_ERROR_INVALID_CONNHDL)
#define BT_STS_RAPS_ERROR_INVALID_CONNIDX                   (LERA_STS_ERROR_INVALID_CONIDX)
#define BT_STS_RAPS_ERROR_EMPTY_POINTER                     (LERA_STS_ERROR_EMPTY_POINTER)
#define BT_STS_RAPS_ERROR_ALLOC_FAILED                      (LERA_STS_ERROR_ALLOC_FAILED)
#define BT_STS_RAPS_ERROR_OUT_OF_MEMORY                     (LERA_STS_ERROR_OUT_OF_MEMORY)

#define BT_STS_RAPS_ERROR_RECV_RTRV_LOST_WHEN_DOING         (BT_STS_RAPS_ERROR_BASE + 0x001)

#define BT_STS_RAPS_ERROR_NO_RAD_MODE_ENABLED               (BT_STS_RAPS_ERROR_BASE + 0x002)
#define BT_STS_RAPS_ERROR_RTM_NOT_ENABLED                   (BT_STS_RAPS_ERROR_BASE + 0x003)
#define BT_STS_RAPS_ERROR_ODM_NOT_ENABLED                   (BT_STS_RAPS_ERROR_BASE + 0x004)
#define BT_STS_RAPS_ERROR_INIT_FAILED                       (BT_STS_RAPS_ERROR_BASE + 0x005)
#define BT_STS_RAPS_ERROR_TIMER_NEW_FAILED                  (BT_STS_RAPS_ERROR_BASE + 0x006)
#define BT_STS_RAPS_ERROR_RAS_DEINIT_FAILED                 (BT_STS_RAPS_ERROR_BASE + 0x007)
#define BT_STS_RAPS_ERROR_REPEAT_TIMER_STOP_REJ             (BT_STS_RAPS_ERROR_BASE + 0x008)
#define BT_STS_RAPS_ERROR_NO_READY_RAD                      (BT_STS_RAPS_ERROR_BASE + 0x009)
#define BT_STS_RAPS_ERROR_UNEXPECTED_CONTROL_POINT_OPCODE   (BT_STS_RAPS_ERROR_BASE + 0x00A)
#define BT_STS_RAPS_ERROR_NTF_TX_FAILED                     (BT_STS_RAPS_ERROR_BASE + 0x00B)
#define BT_STS_RAPS_ERROR_RAD_READY_IND_FAILED              (BT_STS_RAPS_ERROR_BASE + 0x00C)
#define BT_STS_RAPS_ERROR_NO_RAD_BUF                        (BT_STS_RAPS_ERROR_BASE + 0x00D)
#define BT_STS_RAPS_ERROR_NO_RANGING_DATA                   (BT_STS_RAPS_ERROR_BASE + 0x00E)
#define BT_STS_RAPS_ERROR_RAD_OVERWRITTEN_IND_FAILED        (BT_STS_RAPS_ERROR_BASE + 0x00F)
#define BT_STS_RAPS_ERROR_BUF_SIZE_ASYNC                    (BT_STS_RAPS_ERROR_BASE + 0x010)
#define BT_STS_RAPS_ERROR_BUF_Q_UPDATE                      (BT_STS_RAPS_ERROR_BASE + 0x011)
#define BT_STS_RAPS_ERROR_CCCD_NOT_ENABLED                  (BT_STS_RAPS_ERROR_BASE + 0x012)
#define BT_STS_RAPS_ERROR_CP_CMPL_RAD_RSP_TIMEOUT           (BT_STS_RAPS_ERROR_BASE + 0x013)

// * rapc custom status
#define BT_STS_RAPC_SHOW                                    (LERA_STS_SHOW_INFO)
#define BT_STS_RAPC_MGR_INIT                                (LERA_STS_MGR_INIT)
#define BT_STS_RAPC_MGR_DEINIT                              (LERA_STS_MGR_DEINIT)
#define BT_STS_RAPC_UPPER_CALLBACK_REGISTER                 (LERA_STS_CALLBACK_REGISTER)
#define BT_STS_RAPC_CONN_OPENED                             (LERA_STS_CONN_OPENED)
#define BT_STS_RAPC_CONN_CLOSED                             (LERA_STS_CONN_CLOSED)
#define BT_STS_RAPC_MTU_CHANGED                             (LERA_STS_ATT_MTU_CHANGED)
#define BT_STS_RAPC_CONN_ENCRYPTED                          (LERA_STS_CONN_ENCRYPTED)
#define BT_STS_RAPC_CONN_UPDATED                            (LERA_STS_CONN_UPDATED)
#define BT_STS_RAPC_CONN_INFO_INIT                          (LERA_STS_CONN_INFO_INIT)
#define BT_STS_RAPC_REPORT_EVENT                            (LERA_STS_REPORT_EVENT)
#define BT_STS_RAPC_REPORT_EXCEPTION                        (LERA_STS_REPORT_EXCEPTION)
#define BT_STS_RAPC_TIMER_START                             (LERA_STS_TIMER_START)
#define BT_STS_RAPC_TIMER_RESTART                           (LERA_STS_TIMER_RESTART)
#define BT_STS_RAPC_TIMER_STOP                              (LERA_STS_TIMER_STOP)

#define BT_STS_RAPC_SERVICE_CHANGED                         (BT_STS_RAPC_INFO_BASE + 0x001)
#define BT_STS_RAPC_SERVICE_DISCOVERY                       (BT_STS_RAPC_INFO_BASE + 0x002)
#define BT_STS_RAPC_CHARACTER_DISCOVERY                     (BT_STS_RAPC_INFO_BASE + 0x003)
#define BT_STS_RAPC_CHARACTER_READ_RSP                      (BT_STS_RAPC_INFO_BASE + 0x004)
#define BT_STS_RAPC_DESCRIPTOR_READ_RSP                     (BT_STS_RAPC_INFO_BASE + 0x005)
#define BT_STS_RAPC_CHARACTER_WRITE_RSP                     (BT_STS_RAPC_INFO_BASE + 0x006)
#define BT_STS_RAPC_DESCRIPTOR_WRITE_RSP                    (BT_STS_RAPC_INFO_BASE + 0x007)
#define BT_STS_RAPC_INDICATION_RECEIVED                     (BT_STS_RAPC_INFO_BASE + 0x008)
#define BT_STS_RAPC_NOTIFICATION_RECEIVED                   (BT_STS_RAPC_INFO_BASE + 0x009)
#define BT_STS_RAPC_GATT_CLIENT_CALLBACK                    (BT_STS_RAPC_INFO_BASE + 0x00A)
#define BT_STS_RAPC_READ_RAS_FEATURES                       (BT_STS_RAPC_INFO_BASE + 0x00B)
#define BT_STS_RAPC_RECV_CHAR_VALUE                         (BT_STS_RAPC_INFO_BASE + 0x00C)
#define BT_STS_RAPC_ATTR_FOUND                              (BT_STS_RAPC_INFO_BASE + 0x00D)
#define BT_STS_RAPC_STORE_ATTR_INFO                         (BT_STS_RAPC_INFO_BASE + 0x00E)
#define BT_STS_RAPC_ALL_CHAR_FOUND                          (BT_STS_RAPC_INFO_BASE + 0x00F)
#define BT_STS_RAPC_SEND_CONTROL_POINT_CMD                  (BT_STS_RAPC_INFO_BASE + 0x010)
#define BT_STS_RAPC_RECV_REAL_TIME_RAD_HANDLE               (BT_STS_RAPC_INFO_BASE + 0x011)
#define BT_STS_RAPC_WRITE_CHAR_CCCD                         (BT_STS_RAPC_INFO_BASE + 0x012)
#define BT_STS_RAPC_RECV_CCCD_WRITE_RSP                     (BT_STS_RAPC_INFO_BASE + 0x013)
#define BT_STS_RAPC_CONFIG_PRESET_RAD_MODE                  (BT_STS_RAPC_INFO_BASE + 0x014)
#define BT_STS_RAPC_DISCOVER_PEER_RAS                       (BT_STS_RAPC_INFO_BASE + 0x015)
#define BT_STS_RAPC_RECV_FIRST_RAD_SEG                      (BT_STS_RAPC_INFO_BASE + 0x016)
#define BT_STS_RAPC_RECV_LAST_RAD_SEG                       (BT_STS_RAPC_INFO_BASE + 0x017)
#define BT_STS_RAPC_RECV_RAD_OVERWRITTEN                    (BT_STS_RAPC_INFO_BASE + 0x018)
#define BT_STS_RAPC_INTACT_RAD_GEN                          (BT_STS_RAPC_INFO_BASE + 0x019)
#define BT_STS_RAPC_DEL_CONN_INFO                           (BT_STS_RAPC_INFO_BASE + 0x01A)
#define BT_STS_RAPC_FREE_SINGLE_INFO                        (BT_STS_RAPC_INFO_BASE + 0x01B)
#define BT_STS_RAPC_FREE_ALL_INFO                           (BT_STS_RAPC_INFO_BASE + 0x01C)
#define BT_STS_RAPC_CP_INDICATION_HANDLE                    (BT_STS_RAPC_INFO_BASE + 0x01D)
#define BT_STS_RAPC_CACHE_RAD_SEG                           (BT_STS_RAPC_INFO_BASE + 0x01E)
#define BT_STS_RAPC_REFRESH_RAD_BUFFER                      (BT_STS_RAPC_INFO_BASE + 0x01F)
#define BT_STS_RAPC_CLEAR_LOST_SEG_LIST                     (BT_STS_RAPC_INFO_BASE + 0x020)
#define BT_STS_RAPC_CLEAR_CACHED_SEGS                       (BT_STS_RAPC_INFO_BASE + 0x021)
#define BT_STS_RAPC_CHAR_VALUE                              (BT_STS_RAPC_INFO_BASE + 0x022)
#define BT_STS_RAPC_RECORD_LOST_SEG                         (BT_STS_RAPC_INFO_BASE + 0x023)
#define BT_STS_RAPC_CHECK_RAD_INTEGRITY                     (BT_STS_RAPC_INFO_BASE + 0x024)
#define BT_STS_RAPC_RTM_DISCARD_IMCOMPLETE_RAD              (BT_STS_RAPC_INFO_BASE + 0x025)
#define BT_STS_RAPC_DISCOVER_PEER_CHARACTERISTIC            (BT_STS_RAPC_INFO_BASE + 0x026)

#define BT_STS_RAPC_ERROR_SECOND_INIT                       (LERA_STS_ERROR_REPEAT_INIT)
#define BT_STS_RAPC_ERROR_INVALID_PARAM                     (LERA_STS_ERROR_INVALID_PARAMETER)
#define BT_STS_RAPC_ERROR_MGR_NOT_READY                     (LERA_STS_ERROR_ENV_NOT_READY)
#define BT_STS_RAPC_ERROR_NO_CONN_INFO                      (LERA_STS_ERROR_NO_CONN_INFO)
#define BT_STS_RAPC_ERROR_NO_UPPER_CALLBACK                 (LERA_STS_ERROR_NO_CALLBACK)
#define BT_STS_RAPC_ERROR_ALLOC_FAILED                      (LERA_STS_ERROR_ALLOC_FAILED)
#define BT_STS_RAPC_ERROR_UNEXPECTED                        (LERA_STS_ERROR_UNEXPECTED_CASE)
#define BT_STS_RAPC_ERROR_UNEXPECTED_CHAR                   (LERA_STS_ERROR_UNEXPECTED_CASE)
#define BT_STS_RAPC_ERROR_UNEXPECTED_CP_TYPE                (LERA_STS_ERROR_UNEXPECTED_CASE)
#define BT_STS_RAPC_ERROR_UNKNOWN_CCCD                      (LERA_STS_ERROR_UNEXPECTED_CASE)
#define BT_STS_RAPC_ERROR_UNMATCHED_LENGTH                  (LERA_STS_ERROR_UNEXPECTED_CASE)
#define BT_STS_RAPC_ERROR_UNKNOWN_CP_CMD                    (LERA_STS_ERROR_UNEXPECTED_CASE)
#define BT_STS_RAPC_ERROR_CP_CMD_UNSUPPORTED                (LERA_STS_ERROR_UNEXPECTED_CASE)
#define BT_STS_RAPC_ERROR_UNKNOWN_TIMER                     (LERA_STS_ERROR_UNEXPECTED_CASE)

#define BT_STS_RAPC_ERROR_PRF_ID_INVALID                    (BT_STS_RAPC_ERROR_BASE + 0x001)
#define BT_STS_RAPC_ERROR_CCCD_WRITE_REJ                    (BT_STS_RAPC_ERROR_BASE + 0x002)
#define BT_STS_RAPC_ERROR_RAD_GEN_FAILED                    (BT_STS_RAPC_ERROR_BASE + 0x003)
#define BT_STS_RAPC_ERROR_UNKNOWN_RANGING_COUNTER           (BT_STS_RAPC_ERROR_BASE + 0x004)
#define BT_STS_RAPC_ERROR_NO_CACHED_RAD                     (BT_STS_RAPC_ERROR_BASE + 0x005)
#define BT_STS_RAPC_ERROR_LOST_SEG_INFO_LOST                (BT_STS_RAPC_ERROR_BASE + 0x006)
#define BT_STS_RAPC_ERROR_START_RUNNING_TIMER               (BT_STS_RAPC_ERROR_BASE + 0x007)
#define BT_STS_RAPC_ERROR_STOP_NOT_RUNNING_TIMER            (BT_STS_RAPC_ERROR_BASE + 0x008)
#define BT_STS_RAPC_ERROR_SERVICE_DISCOVERY_FAILRURE        (BT_STS_RAPC_ERROR_BASE + 0x009)
#define BT_STS_RAPC_ERROR_RTM_AND_ODM_SHOULD_MUTEX          (BT_STS_RAPC_ERROR_BASE + 0x00A)
#define BT_STS_RAPC_ERROR_UNKNOWN_CS_PROC_STATE             (BT_STS_RAPC_ERROR_BASE + 0x00B)
#define BT_STS_RAPC_ERROR_CS_PROC_STATE_CHANGED_FAILED      (BT_STS_RAPC_ERROR_BASE + 0x00C)
#define BT_STS_RAPC_ERROR_ABORT_OP_NOT_SUPP                 (BT_STS_RAPC_ERROR_BASE + 0x00D)
#define BT_STS_RAPC_ERROR_RECV_DUPLICATED_RAD_SEG           (BT_STS_RAPC_ERROR_BASE + 0x00E)
#define BT_STS_RAPC_ERROR_CANNOT_RETRIEVE_LOST_SEGS         (BT_STS_RAPC_ERROR_BASE + 0x00F)
#define BT_STS_RAPC_ERROR_CHAR_DISC_FAILED                  (BT_STS_RAPC_ERROR_BASE + 0x011)
#define BT_STS_RAPC_ERROR_IGNORE_CMPL_LOST_SEGS             (BT_STS_RAPS_ERROR_BASE + 0x012)

#define RAP_INVALID_RANING_COUNTER  0xFFFF
#define RAP_INVALID_CONNIDX         GAP_INVALID_CONIDX
#define RAP_INVALID_CONNHDL         GAP_INVALID_CONN_HANDLE
#define RAP_MIN_MTU                 L2CAP_LE_MIN_MTU
#define RAP_MAX_ROLLING_SEG_COUNTER (64)

typedef enum
{
    RAP_RAD_MODE_NONE,
    RAP_RAD_MODE_REAL_TIME,
    RAP_RAD_MODE_ON_DEMAND,
} rap_rad_mode_t;

#define RAP_FILTER_CFG_M0_Packet_Quality_MASK               CO_BIT_MASK(0)
#define RAP_FILTER_CFG_M0_Packet_RSSI_MASK                  CO_BIT_MASK(1)
#define RAP_FILTER_CFG_M0_Packet_Antenna_MASK               CO_BIT_MASK(2)
#define RAP_FILTER_CFG_M0_Measured_Freq_Offset_MASK         CO_BIT_MASK(3)

#define RAP_FILTER_CFG_M1_Packet_Quality_MASK               CO_BIT_MASK(0)
#define RAP_FILTER_CFG_M1_Packet_NADM_MASK                  CO_BIT_MASK(1)
#define RAP_FILTER_CFG_M1_PACKET_RSSI_MASK                  CO_BIT_MASK(2)
#define RAP_FILTER_CFG_M1_ToD_ToA_MASK                      CO_BIT_MASK(3)
#define RAP_FILTER_CFG_M1_Packet_Antenna_MASK               CO_BIT_MASK(4)
#define RAP_FILTER_CFG_M1_Packet_PCT1_MASK                  CO_BIT_MASK(5)
#define RAP_FILTER_CFG_M1_Packet_PCT2_MASK                  CO_BIT_MASK(6)

#define RAP_FILTER_CFG_M2_Antenna_Permutation_Index_MASK    CO_BIT_MASK(0)
#define RAP_FILTER_CFG_M2_Tone_PCT_MASK                     CO_BIT_MASK(1)
#define RAP_FILTER_CFG_M2_Tone_Quality_Indicator_MASK       CO_BIT_MASK(2)
#define RAP_FILTER_CFG_M2_Antenna_Path_1_MASK               CO_BIT_MASK(3)
#define RAP_FILTER_CFG_M2_Antenna_Path_2_MASK               CO_BIT_MASK(4)
#define RAP_FILTER_CFG_M2_Antenna_Path_3_MASK               CO_BIT_MASK(5)
#define RAP_FILTER_CFG_M2_Antenna_Path_4_MASK               CO_BIT_MASK(6)

#define RAP_FILTER_CFG_M3_Packet_Quality_MASK               CO_BIT_MASK(0)
#define RAP_FILTER_CFG_M3_Packet_NADM_MASK                  CO_BIT_MASK(1)
#define RAP_FILTER_CFG_M3_Packet_RSSI_MASK                  CO_BIT_MASK(2)
#define RAP_FILTER_CFG_M3_ToD_ToA_MASK                      CO_BIT_MASK(3)
#define RAP_FILTER_CFG_M3_Packet_Antenna_MASK               CO_BIT_MASK(4)
#define RAP_FILTER_CFG_M3_Packet_PCT1_MASK                  CO_BIT_MASK(5)
#define RAP_FILTER_CFG_M3_Packet_PCT2_MASK                  CO_BIT_MASK(6)
#define RAP_FILTER_CFG_M3_Antenna_Permutation_Index_MASK    CO_BIT_MASK(7)
#define RAP_FILTER_CFG_M3_Tone_PCT_MASK                     CO_BIT_MASK(8)
#define RAP_FILTER_CFG_M3_Tone_Quality_Indicator_MASK       CO_BIT_MASK(9)
#define RAP_FILTER_CFG_M3_Antenna_Path_1_MASK               CO_BIT_MASK(10)
#define RAP_FILTER_CFG_M3_Antenna_Path_2_MASK               CO_BIT_MASK(11)
#define RAP_FILTER_CFG_M3_Antenna_Path_3_MASK               CO_BIT_MASK(12)
#define RAP_FILTER_CFG_M3_Antenna_Path_4_MASK               CO_BIT_MASK(13)

#if 0
#define RAP_FILTER_CFG_M0_DEFAULT_MASK    (0x3C00)    // mode: 0b 00 (0 ~ 1 bit), mask: 0b 11 1100 0000 0000 (2 ~ 15 bit)
#define RAP_FILTER_CFG_M1_DEFAULT_MASK    (0x7F80)    // mode: 0b 01 (0 ~ 1 bit), mask: 0b 11 1111 1000 0000 (2 ~ 15 bit)
#define RAP_FILTER_CFG_M2_DEFAULT_MASK    (0xBF80)    // mode: 0b 10 (0 ~ 1 bit), mask: 0b 11 1111 1000 0000 (2 ~ 15 bit)
#define RAP_FILTER_CFG_M3_DEFAULT_MASK    (0xFFFF)    // mode: 0b 11 (0 ~ 1 bit), mask: 0b 11 1111 1111 1111 (2 ~ 15 bit)
#endif

#define RAP_FILTER_CFG_M0_DEFAULT_MASK  (RAP_FILTER_CFG_M0_Packet_Quality_MASK | RAP_FILTER_CFG_M0_Packet_RSSI_MASK \
                                       | RAP_FILTER_CFG_M0_Packet_Antenna_MASK | RAP_FILTER_CFG_M0_Measured_Freq_Offset_MASK)
#define RAP_FILTER_CFG_M1_DEFAULT_MASK  (RAP_FILTER_CFG_M1_Packet_Quality_MASK | RAP_FILTER_CFG_M1_Packet_NADM_MASK \
                                       | RAP_FILTER_CFG_M1_PACKET_RSSI_MASK    | RAP_FILTER_CFG_M1_ToD_ToA_MASK \
                                       | RAP_FILTER_CFG_M1_Packet_Antenna_MASK | RAP_FILTER_CFG_M1_Packet_PCT1_MASK \
                                       | RAP_FILTER_CFG_M1_Packet_PCT2_MASK)
#define RAP_FILTER_CFG_M2_DEFAULT_MASK  (RAP_FILTER_CFG_M2_Antenna_Permutation_Index_MASK \
                                       | RAP_FILTER_CFG_M2_Tone_PCT_MASK       | RAP_FILTER_CFG_M2_Tone_Quality_Indicator_MASK \
                                       | RAP_FILTER_CFG_M2_Antenna_Path_1_MASK | RAP_FILTER_CFG_M2_Antenna_Path_2_MASK \
                                       | RAP_FILTER_CFG_M2_Antenna_Path_3_MASK | RAP_FILTER_CFG_M2_Antenna_Path_4_MASK)
#define RAP_FILTER_CFG_M3_DEFAULT_MASK  (RAP_FILTER_CFG_M3_Packet_Quality_MASK | RAP_FILTER_CFG_M3_Packet_NADM_MASK \
                                       | RAP_FILTER_CFG_M3_Packet_RSSI_MASK    | RAP_FILTER_CFG_M3_ToD_ToA_MASK \
                                       | RAP_FILTER_CFG_M3_Packet_Antenna_MASK | RAP_FILTER_CFG_M3_Packet_PCT1_MASK \
                                       | RAP_FILTER_CFG_M3_Packet_PCT2_MASK    | RAP_FILTER_CFG_M3_Antenna_Permutation_Index_MASK \
                                       | RAP_FILTER_CFG_M3_Tone_PCT_MASK       | RAP_FILTER_CFG_M3_Tone_Quality_Indicator_MASK \
                                       | RAP_FILTER_CFG_M3_Antenna_Path_1_MASK | RAP_FILTER_CFG_M3_Antenna_Path_2_MASK \
                                       | RAP_FILTER_CFG_M3_Antenna_Path_3_MASK | RAP_FILTER_CFG_M3_Antenna_Path_4_MASK)

// TODO ! move the nv operation to the upper layer (app layer)
typedef struct
{
    uint16_t mask[CS_STEP_MODE_MAX_NUM];
} rap_filter_cfg_t;

#define RAP_FILTERED_STEP_MODE_INVALID_LEN  (0xFF)
uint8_t rap_get_spec_step_data_fd_len(uint8_t mode, cs_step_spec_type_t step_type, uint8_t num_antenna_paths, rap_filter_cfg_t *p_filter_cfg);
uint8_t rap_get_m0_fd_len(cs_step_spec_type_t step_type, uint16_t filter_cfg);
uint8_t rap_get_m1_fd_len(cs_step_spec_type_t step_type, uint16_t filter_cfg);
uint8_t rap_get_m2_fd_len(cs_step_spec_type_t step_type, uint16_t filter_cfg, uint8_t num_antenna_paths);
uint8_t rap_get_m3_fd_len(cs_step_spec_type_t step_type, uint16_t filter_cfg, uint8_t antenna_paths);

void rap_exec_local_rad_filter(cs_recv_sub_res_param_t *p_param, rap_filter_cfg_t *p_filter_cfg, cs_subevent_result_t **pp_data, uint16_t *p_data_len);
void rap_exec_m0_filter(cs_step_spec_type_t step_type, uint16_t filter_cfg, const cs_step_params_t *p_params, cs_step_params_t *p_fd_params);
void rap_exec_m1_filter(cs_step_spec_type_t step_type, uint16_t filter_cfg, const cs_step_params_t *p_params, cs_step_params_t *p_fd_params);
void rap_exec_m2_filter(uint8_t num_antenna_paths, cs_step_spec_type_t step_type, uint16_t filter_cfg, const cs_step_params_t *p_params, cs_step_params_t *p_fd_params);
void rap_exec_m3_filter(uint8_t num_antenna_paths, cs_step_spec_type_t step_type, uint16_t filter_cfg, const cs_step_params_t *p_params, cs_step_params_t *p_fd_params);

uint8_t rap_get_step_data_len_by_step_type(uint8_t mode, cs_step_spec_type_t step_type, uint8_t num_antenna_paths, rap_filter_cfg_t *p_filter_cfg);
bool rap_ranging_done_status_is_end(uint8_t ranging_done_status);
uint8_t rap_get_num_antenna_paths_by_mask(uint8_t antenna_paths_mask);
uint8_t rap_get_antenna_paths_mask_by_num(uint8_t num_antenna_paths);

#ifdef CS_HEAP_DEBUG
#define CS_HEAP_LOG TRACE
#else
#define CS_HEAP_LOG(...) ((void)0)
#endif
#define CS_MALLOC(size)     ({ uint8_t *tmp = cobuf_malloc(size); CS_HEAP_LOG(0, "%s: [%d] chsd malloc %p, %d bytes", __func__, __LINE__, tmp, size); tmp; })
#define CS_FREE(buf)        do { CS_HEAP_LOG(0, "%s: [%d] chsd free %p", __func__, __LINE__, buf); cobuf_free(buf); } while(0)
#define CS_SAFE_FREE(buf)   do { typeof(buf) _buf = (buf); if (_buf) CS_FREE(_buf); } while(0)

#define RAS_CHAR_WITH_CCCD_UUID_TO_IDX_MAP(char_uuid)   (char_uuid - GATT_CHAR_UUID_RAS_REAL_TIME_RANGING_DATA)
#define RAS_CHAR_WITH_CCCD_NUM                          (5)

#endif  /* __RANGING_H__ */
