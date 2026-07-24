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
#ifndef __ECC_P256_H__
#define __ECC_P256_H__
#include <stdint.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

#define ECC_PUB_KEY_256_LEN     (0x40)
#define ECC_DH_KEY_LEN          (0x20)

/**
 * @brief ECC p256 key generation result callback
 *
 */
typedef void (*ecc_key256_gen_result_cb)(void *context, const uint8_t *output, uint8_t output_len);

/**
 ****************************************************************************************
 * @brief Generate a Secret Key compliant with ECC P256 algorithm
 *
 * If key is forced, just check its validity
 *
 * @param[out] secret_key Private key - MSB First
 * @param[in]  forced True if provided key is forced, else generate it.
 *
 * @return int status
 ****************************************************************************************
 */
int ecc_gen_new_secret_key_256(uint8_t *secret_key, bool forced);

/**
 ****************************************************************************************
 * @brief Generate a public Key compliant with ECC P256 algorithm
 *
 * If key is forced, just check its validity
 *
 * @param[in] secret_key Private key - MSB First
 * @param[out]  out_public_key Public key - MSB First
 *
 * @return int status
 ****************************************************************************************
 */
int ecc_gen_new_public_key_256(uint8_t *secret_key, uint8_t *out_public_key);

/**
 ****************************************************************************************
 * @brief Calculate DHkey sync call
 *
 * @param[in]  secret_key  Private key             - MSB First
 * @param[in]  pub_key_x   Public key x coordinate - LSB First
 * @param[in]  pub_key_y   Public key y coordinate - LSB First
 * @param[out] out_dh_key  DHkey calculated - LSB First
 *
 * @return int status
 ****************************************************************************************
 */
int ecc_p256_gen_dh_key(const uint8_t *secret_key, const uint8_t *public_key_x, const uint8_t *public_key_y, uint8_t *out_dh_key);

/**
 ****************************************************************************************
 * @brief Calculate DHkey async call
 *
 * @param[in]  secret_key  Private key             - MSB First
 * @param[in]  pub_key_x   Public key x coordinate - LSB First
 * @param[in]  pub_key_y   Public key y coordinate - LSB First
 * @param[in]  context     Upper provided context, return with @see result_cb
 * @param[in]  result_cb   Result callback
 *
 * @return int status
 ****************************************************************************************
 */
int ecc_p256_gen_dh_key_async(const uint8_t *secret_key, const uint8_t *pub_key_x, const uint8_t *pub_key_y,
                              void *context, ecc_key256_gen_result_cb result_cb);

/**
 ****************************************************************************************
 * @brief Retrieve debug private and public keys
 *
 * @param[out] secret_key Private key             - MSB First
 * @param[out] pub_key_x  Public key x coordinate - LSB First
 * @param[out] pub_key_y  Public key y coordinate - LSB First
 ****************************************************************************************
 */
void ecc_get_debug_keys(const uint8_t **secret_key, const uint8_t **pub_key_x, const uint8_t **pub_key_y);

#ifdef __cplusplus
}
#endif
#endif /* __ECC_P256_H__ */
