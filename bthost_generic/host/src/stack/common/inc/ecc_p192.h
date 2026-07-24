/***************************************************************************
 *
 * Copyright 2015-2026 BES.
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
#ifndef __ECC_P192_H__
#define __ECC_P192_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ECC_PUB_KEY_192_LEN     (0x30)

/**
 ****************************************************************************************
 * @brief Generate a Secret Key compliant with ECC P192 algorithm
 *
 * If key is forced, just check its validity
 *
 * @param[out] secret_key Private key - MSB First
 ****************************************************************************************
 */
void ecc_p192_gen_new_secret_key(uint8_t* secret_key);

/**
 ****************************************************************************************
 * @brief Generate a new Public key pair using ECC P192 algorithm
 *
 * @param[in] secret_key Private key - MSB First
 *
 * @return status   0 if key generation is started, > 0 otherwise
 ****************************************************************************************
 */
int ecc_p192_gen_new_public_key(const uint8_t* secret_key, uint8_t* out_pub_key);

/**
 ****************************************************************************************
 * @brief Generate a new DHKey using ECC P192 algorithm
 *
 * @param[in] secret_key        Private key                  - MSB First
 * @param[in] public_key_x      Peer public key x coordinate - LSB First
 * @param[in] public_key_y      Peer public key y coordinate - LSB First
 *
 * @return status   0 if key generation is started, > 0 otherwise
 ****************************************************************************************
 */
int ecc_p192_gen_dh_key(const uint8_t* secret_key, const uint8_t* public_key_x, const uint8_t* public_key_y, uint8_t* out_dh_key);

#ifdef __cplusplus
}
#endif
#endif // (BT_EMB_PRESENT)
