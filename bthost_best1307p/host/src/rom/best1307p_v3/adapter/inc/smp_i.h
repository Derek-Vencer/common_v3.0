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
#ifndef __BLE_SMP_I_H__
#define __BLE_SMP_I_H__

#include "gap_i.h"

/**
 * DEFINES
 *
 *
 */
#define SMP_SECURE_PAIRING_FLAG 0x10
#define SMP_PAIRING_METHOD_MASK 0x0f

/**
 * ENUMERATIONS
 *
 *
 */

/**
 * TYPEDEFINES
 *
 *
 */

struct smp_conn_item_t;
struct smp_receive_t;

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * FUNCTIONS DECLARATIONS
 *
 *
 */

bool smp_random(void *priv, gap_key_callback_t cmpl);
bool smp_e(const uint8_t *key_128_le, const uint8_t *plain_128_le, gap_key_callback_t cmpl, void *priv);
bool smp_gen_irk(gap_key_callback_t func, void *priv);
bool smp_gen_csrk(gap_key_callback_t func, void *priv);
bool smp_ah(const uint8_t *k_128_le, const uint8_t *r_24_le, gap_key_callback_t func, void *priv);
bool smp_aes_ccm(const uint8_t *key_128_le, const uint8_t *nonce_le, const uint8_t *m, uint16_t m_len, bool encrypt,
                 uint8_t add_auth_data, gap_key_callback_t func, void *priv);
bool smp_aes_cmac(const uint8_t *k_128_le, const uint8_t *m_le, uint16_t m_len, gap_key_callback_t func, void *priv);
bool smp_f5_gen_key_T(const uint8_t *DHKey_256_le, gap_key_callback_t func, void *priv);
bool smp_f5_gen_mackey_ltk(const uint8_t *key_T_128_le, const uint8_t *Ra_128_le, const uint8_t *Rb_128_le,
                           const uint8_t *A, const uint8_t *B, gap_key_callback_t mackey, gap_key_callback_t ltk, void *priv);
bool smp_f6(const uint8_t *W_128_le, const uint8_t *N1_128_le, const uint8_t *N2_128_le,
            const uint8_t *R_128_le, const uint8_t *IOcap_24_le, const uint8_t *A1_56_le, const uint8_t *A2_56_le,
            gap_key_callback_t func, void *priv);
bool smp_g2(const uint8_t *PKax_U_256_le, const uint8_t *PKbx_V_256_le, const uint8_t *Ra_X_128_le,
            const uint8_t *Rb_Y_128_le, gap_key_callback_t func, void *priv);
bool smp_h6(const uint8_t *W_128_le, const uint8_t *key_id_32_le, gap_key_callback_t func, void *priv);
bool smp_h7(const uint8_t *salt_128_le, const uint8_t *W_128_le, gap_key_callback_t func, void *priv);
bool smp_h8(const uint8_t *k_128_le, const uint8_t *s_128_le, const uint8_t *key_id_32_le, gap_key_callback_t func,
            void *priv);
bool smp_big_gsk_gen(const uint8_t *gltk_128_le, const uint8_t *rand_gskd_128_le, gap_key_callback_t func, void *priv);
bool smp_signature(uint8_t *m_le, uint16_t len, uint32_t sign_counter, gap_key_callback_t func, void *priv);
bool smp_linkkey_to_iltk(const uint8_t *linkkey, bool ct2, gap_key_callback_t func, void *priv);
bool smp_iltk_to_ltk(const uint8_t *iltk, gap_key_callback_t func, void *priv);
bool smp_ltk_to_ilk(const uint8_t *ltk, bool ct2, gap_key_callback_t func, void *priv);
bool smp_ilk_to_linkkey(const uint8_t *ilk, gap_key_callback_t func, void *priv);
void smp_aes_key_xor(const uint8_t *a, const uint8_t *b, gap_key_callback_t func, void *priv);
void smp_aes_key_shift(const uint8_t *key, gap_key_callback_t func, void *priv);
void smp_pairing_end(gap_conn_item_t *conn, uint8_t error_code);

bt_status_t smp_send_pairing_request(gap_conn_item_t *conn, const smp_requirements_t *p_requirements);
bt_status_t smp_send_security_request(gap_conn_item_t *conn, uint8_t auth_req);
bt_status_t smp_bredr_ctkd_request(uint16_t connhdl);
void smp_continue_bredr_pairing(gap_conn_item_t *conn);
void smp_receive_peer_ltk_req(gap_conn_item_t *conn, struct hci_ev_le_ltk_request *p);
bt_status_t smp_check_send_ltk_key_reply(gap_conn_item_t *conn, bool negative_reply, const uint8_t *ltk_reply);
bt_status_t smp_check_enable_encryption(gap_conn_item_t *conn, const gap_ltk_enc_info_t *enc_info,
                                        const uint8_t *ltk);
bt_status_t smp_check_send_pairing_keys(gap_conn_item_t *conn, const ble_bdaddr_t *ia, const uint8_t *irk, const uint8_t *csrk);
void smp_input_oob_legacy_tk(uint16_t peer_type_or_connhdl, const bt_bdaddr_t *peer_addr, const uint8_t *tk);
void smp_input_6_digit_passkey(uint16_t peer_type_or_connhdl, const bt_bdaddr_t *peer_addr, uint32_t passkey);
void smp_input_numeric_confirm(uint16_t peer_type_or_connhdl, const bt_bdaddr_t *peer_addr, bool user_confirmed);
void smp_input_peer_oob_auth_data(uint16_t peer_type_or_connhdl, const bt_bdaddr_t *peer_addr,
                                  const gap_smp_oob_auth_data_t *data);
void smp_input_local_oob_auth_data(uint16_t peer_type_or_connhdl, const bt_bdaddr_t *peer_addr,
                                   const gap_smp_oob_auth_data_t *data);

#if defined(__cplusplus)
}
#endif

#endif /* __BLE_SMP_I_H__ */