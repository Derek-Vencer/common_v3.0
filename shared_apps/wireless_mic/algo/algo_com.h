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
#ifndef __ALGO_COM_H__
#define __ALGO_COM_H__

#ifdef __cplusplus
extern "C" {
#endif

uint32_t audio_copy_32bit_to_24bit(uint8_t* dest, uint8_t* src, uint32_t src_bytes);

#ifdef __cplusplus
}
#endif

#endif
