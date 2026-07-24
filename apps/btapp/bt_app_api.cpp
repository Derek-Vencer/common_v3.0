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
/*INCLUDE*/
#include "cmsis_os.h"

#include "bluetooth.h"
#include "bes_me_api.h"
#include "app_keyhandle.h"

#ifdef BT_SVC_MODULE_TWS_ENABLED
extern void app_tws_cmd_register_cmd_table(void);
#endif

#ifdef BT_APP_RSSI
#include "app_rssi/app_rssi.h"
#endif /*BT_APP_RSSI*/

#ifdef IOS_IAP2_BLUETOOTH
#include "app_iap2.h"
#endif

/*FUNCTIONS*/
int bt_app_init(void)
{
#ifndef BT_BUILD_WITH_CUSTOMER_HOST
#ifndef BLE_ONLY_ENABLED
    bes_bt_app_init();
#endif
#endif

#ifdef BT_APP_RSSI
    app_rssi_init();
#endif

#ifdef APP_KEY_ENABLE
#ifndef BLE_ONLY_ENABLED
    app_key_handle_init();
#endif
#endif

#ifdef BT_SVC_MODULE_TWS_ENABLED
    app_tws_cmd_register_cmd_table();
#endif

#ifdef IOS_IAP2_BLUETOOTH
    app_iap2_init();
#endif

    return 0;
}

int bt_app_deinit(void)
{
    return 0;
}