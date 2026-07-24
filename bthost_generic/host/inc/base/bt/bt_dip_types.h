/***************************************************************************
 *
 * Copyright 2015-2025 BES.
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

typedef struct
{
    uint16_t spec_id;           // SpecificationID
    uint16_t vend_id;           // VendorID
    uint16_t prod_id;           // ProductID
    uint16_t prod_ver;          // Version
    uint8_t  prim_rec;          // PrimaryRecord
    uint16_t vend_id_source;    // VendorIDSource
} bt_dip_pnp_info_t;

typedef void (*bt_dip_info_queried_callback)(uint8_t *bdaddr, bt_dip_pnp_info_t *pnp_info);
