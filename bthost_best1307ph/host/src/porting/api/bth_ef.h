/***************************************************************************
 *
 * Copyright 2015-2024 BES.
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
 * @brief xxx.
 *
 ****************************************************************************/
#ifndef __BTH_EF_H__
#define __BTH_EF_H__

/****************************** header include ********************************/
#include <stdint.h>
#include "nvrecord_bt.h"

#if defined(__cplusplus)
extern "C" {
#endif
/***************************** external declaration *****************************/

/***************************** macro defination *******************************/

/***************************** type defination ********************************/

/***************************** variable defination *****************************/

/***************************** function declaration ****************************/

#define bth_ef_nv_open() nv_record_open(section_usrdata_ddbrecord)

#define bth_ef_nv_close() nv_record_flash_flush()

#define bth_ef_nv_record_blerec_init() nv_record_blerec_init()

#define bth_ef_nv_list_saved_flash() nv_record_flash_flush()

#define bth_ef_nv_find_record(bdAddr, record) nv_record_ddbrec_find((bdAddr), (record))

#define bth_ef_nv_delete_record(bdAddr) nv_record_ddbrec_delete(bdAddr)

#define bth_ef_nv_enum_device_records(index, record) nv_record_enum_dev_records((unsigned short)(index), (record))

#define bth_ef_nv_add_record(record) nv_record_add(section_usrdata_ddbrecord, (btif_device_record_t *)(record))

#if defined(__cplusplus)
}
#endif

#endif //__BTH_EF_H__