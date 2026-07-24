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
#pragma once

#if defined(BT_STACK_LOG_DISABLE) || defined(BT_LOG_SIMPLIFY)
#undef STACK_LEVEL
#define STACK_LEVEL     LOGGER_LEVEL_FATAL
#endif

#define TEST_MODULE     0
#define STACK_MODULE    1
#define SERVICE_MODULE  2

#define L2CAP_MODULE    3
#define HCI_MODULE      4

#define APP_BLE_MODULE  5
#define CS_MODULE       6
#define GAP_MODULE      7
#define GATT_MODULE     8
#define GAF_MODULE      9
#define SMP_MODULE      10

#define NAPP_MODULE     11
#define NBTT_MODULE     12
#define NDIP_MODULE     13
#define HIOD_MODULE     14
#define HIOH_MODULE     15
#define NSPP_MODULE     16
#define HTST_MODULE     17
#define DTCP_MODULE     18
#define NHCI_MODULE     19
#define SSAP_MODULE     20

#define RAP_MODULE      21
#define RAPS_MODULE     22
#define RAPC_MODULE     23

#define BTH_ROM_MODULE  24
#define BTH_MID_MODULE  25

#define L2CAP_LEVEL STACK_LEVEL

#define CS_LEVEL    STACK_LEVEL
#define GAP_LEVEL   STACK_LEVEL
#define GATT_LEVEL  STACK_LEVEL
#define GAF_LEVEL   STACK_LEVEL
#define SMP_LEVEL   STACK_LEVEL

#define DTCP_LEVEL  STACK_LEVEL
#define NHCI_LEVEL  STACK_LEVEL
#define SSAP_LEVEL  STACK_LEVEL

#define APP_BLE_LEVEL   SERVICE_LEVEL

#define NAPP_LEVEL      SERVICE_LEVEL
#define NBTT_LEVEL      SERVICE_LEVEL
#define NDIP_LEVEL      SERVICE_LEVEL
#define HIOD_LEVEL      SERVICE_LEVEL
#define HIOH_LEVEL      SERVICE_LEVEL
#define NSPP_LEVEL      SERVICE_LEVEL
#define HTST_LEVEL      SERVICE_LEVEL

/* temp, to be optimize */
#define RAP_LEVEL   STACK_LEVEL
#define RAPS_LEVEL  STACK_LEVEL
#define RAPC_LEVEL  STACK_LEVEL

#define _MAKE_STR(a) #a
#define MAKE_STR(a) _MAKE_STR(a)
#define _EXPAND_CONCAT(a, b) a##b
#define EXPAND_CONCAT(a, b) _EXPAND_CONCAT(a, b)

#define __logger_str                    EXPAND_CONCAT(__trc, __LINE__)
#if defined(BT_LOGGER_SECTION)
#define LOGGER_SECTION_CONCAT(a, b)     a.b
#define LOGGER_SECTION_NAME             LOGGER_SECTION_CONCAT(.trc_str, __LINE__)
#define LOGGER_SECTION_ATTRIBUTE        __attribute__((section(MAKE_STR(LOGGER_SECTION_NAME))))
#define LOGGER_SECTION(str)             (({ static const char LOGGER_SECTION_ATTRIBUTE __logger_str[] = (str); __logger_str; }))
#else
#define LOGGER_SECTION_ATTRIBUTE
#define LOGGER_SECTION(str)             (str)
#endif
