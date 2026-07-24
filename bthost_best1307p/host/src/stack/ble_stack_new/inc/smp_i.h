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
typedef enum
{
    SMP_PAIRING_REQ                     = 0x01, // le-u acl-u
    SMP_PAIRING_RSP                     = 0x02, // le-u acl-u
    SMP_PAIRING_CONFIRM                 = 0x03, // le-u
    SMP_PAIRING_RANDOM                  = 0x04, // le-u
    SMP_PAIRING_FAILED                  = 0x05, // le-u acl-u
    SMP_ENCRYPTION_INFO                 = 0x06, // le-u
    SMP_CENTRAL_IDENTIFICATION          = 0x07, // le-u
    SMP_IDENTITY_INFO                   = 0x08, // le-u acl-u
    SMP_IDENTITY_ADDR_INFO              = 0x09, // le-u acl-u
    SMP_SIGNING_INFO                    = 0x0a, // le-u acl-u
    SMP_SECURITY_REQ                    = 0x0b, // le-u
    SMP_PAIRING_PUBLIC_KEY              = 0x0c, // le-u
    SMP_PAIRING_DHKEY_CHECK             = 0x0d, // le-u
    SMP_PAIRING_KEYPRESS_NOTIFY         = 0x0e, // le-u
} smp_opcode_t;

typedef enum
{
    SMP_PHASE_IDLE                      = 0x00,
    SMP_PHASE_PAIRING_START             = 0x01,
    SMP_PHASE_GEN_TK_RAND               = 0x02,
    SMP_PHASE_GEN_CFM_VALUE             = 0x03,
    SMP_PHASE_WAIT_PAIRING_CONFIRM      = 0x04,
    SMP_PHASE_WAIT_PAIRING_RANDOM       = 0x05,
    SMP_PHASE_VERIFY_CONFIRM            = 0x06,
    SMP_PHASE_GEN_STK                   = 0x07,
    SMP_PHASE_GEN_PUB_KEY               = 0x08,
    SMP_PHASE_WAIT_PAIRING_PUBLIC_KEY   = 0x09,
    SMP_PHASE_GEN_DHKEY                 = 0x0a,
    SMP_PHASE_GET_PASSKEY               = 0x0b,
    SMP_PHASE_GET_L_OOB_AUTH_DATA       = 0x0c,
    SMP_PHASE_GET_P_OOB_AUTH_DATA       = 0x0d,
    SMP_PHASE_GEN_RANDOM                = 0x0e,
    SMP_PHASE_GEN_USER_VALUE            = 0x0f,
    SMP_PHASE_START_STAGE_2             = 0x10,
    SMP_PHASE_GEN_LTK                   = 0x11,
    SMP_PHASE_GEN_CHECK_VALUE           = 0x12,
    SMP_PHASE_WAIT_PAIRING_DHKEY_CHECK  = 0x13,
    SMP_PHASE_VERIFY_CHECK              = 0x14,
    SMP_PHASE_WAIT_PEER_LTK_REQ         = 0x15,
    SMP_PHASE_WAIT_ENC_CHANGE           = 0x16,
    SMP_PHASE_START_PHASE_3             = 0x17,
    SMP_PHASE_WAIT_DIST_KEY             = 0x18,
    SMP_PHASE_FINISHED                  = 0x19,
    SMP_PHASE_MAX_NUM                   = 0x1a,
} smp_phase_t;

typedef enum
{
    SMP_EVENT_NONE                      = 0x00,
    SMP_EVENT_PAIRING_REQ               = SMP_PAIRING_REQ,              // 0x01
    SMP_EVENT_PAIRING_RSP               = SMP_PAIRING_RSP,              // 0x02
    SMP_EVENT_PAIRING_CONFIRM           = SMP_PAIRING_CONFIRM,          // 0x03
    SMP_EVENT_PAIRING_RANDOM            = SMP_PAIRING_RANDOM,           // 0x04
    SMP_EVENT_PAIRING_FAILED            = SMP_PAIRING_FAILED,           // 0x05
    SMP_EVENT_ENCRYPTION_INFO           = SMP_ENCRYPTION_INFO,          // 0x06
    SMP_EVENT_CENTRAL_IDENTIFICATION    = SMP_CENTRAL_IDENTIFICATION,   // 0x07
    SMP_EVENT_IDENTITY_INFO             = SMP_IDENTITY_INFO,            // 0x08
    SMP_EVENT_IDENTITY_ADDR_INFO        = SMP_IDENTITY_ADDR_INFO,       // 0x09
    SMP_EVENT_SIGNING_INFO              = SMP_SIGNING_INFO,             // 0x0a
    SMP_EVENT_SECURITY_REQ              = SMP_SECURITY_REQ,             // 0x0b
    SMP_EVENT_PAIRING_PUBLIC_KEY        = SMP_PAIRING_PUBLIC_KEY,       // 0x0c
    SMP_EVENT_PAIRING_DHKEY_CHECK       = SMP_PAIRING_DHKEY_CHECK,      // 0x0d
    SMP_EVENT_PAIRING_KEYPRESS_NOTIFY   = SMP_PAIRING_KEYPRESS_NOTIFY,  // 0x0e
    SMP_EVENT_RECV_PASSKEY              = 0x11,
    SMP_EVENT_RECV_L_OOB_AUTH_DATA      = 0x12,
    SMP_EVENT_RECV_P_OOB_AUTH_DATA      = 0x13,
    SMP_EVENT_RECV_RANDOM               = 0x14,
    SMP_EVENT_RECV_STK                  = 0x15,
    SMP_EVENT_RECV_CFM_VALUE            = 0x16,
    SMP_EVENT_RECV_PUB_KEY              = 0x17,
    SMP_EVENT_RECV_DHKEY                = 0x18,
    SMP_EVENT_RECV_USER_VALUE           = 0x19,
    SMP_EVENT_RECV_LTK                  = 0x1a,
    SMP_EVENT_RECV_CHECK_VALUE          = 0x1b,
    SMP_EVENT_RECV_PEER_LTK_REQ         = 0x1c,
    SMP_EVENT_RECV_ENC_CHANGE           = 0x1d,
} smp_event_t;

/**
 * TYPEDEFINES
 *
 *
 */
typedef struct
{
    bool bonding_type;
    bool require_mitm_protection;
    bool secure_connection_support;
    bool keypress_notify;
    bool ct2; // security h7 function support or not
} smp_auth_requirements_t;

typedef struct
{
    uint8_t opcode;
    uint8_t io_cap; // RFU if SC pairing intiated over BR/EDR
    uint8_t oob_flag; // 0x00 OOB auth data not present, 0x01 present, RFU if SC pairing intiated over BR/EDR
    uint8_t auth_req; // RFU if SC pairing intiated over BR/EDR except the CT2 bit
    uint8_t max_enc_key_size; // max encryption key size the device can support, 7 to 16 octets
    uint8_t init_key_dist; // which keys the initiator is requesting to distribute / generate or use during the Transport Specific Key Distribution phase
    uint8_t resp_key_dist; // which keys the initiator is requesting the responder to distribute / generate or use during the TSKD phase
} __attribute__((packed)) smp_pairing_req_t;

typedef smp_pairing_req_t smp_pairing_rsp_t;

typedef struct
{
    smp_phase_t curr_phase;
    smp_event_t allow_event;
    smp_phase_t next_phase;
    smp_event_t second_event;
} smp_transfer_t;

typedef struct
{
    uint16_t initiator: 1;
    uint16_t secure_debug_mode: 1;
    uint16_t pairing_random_received: 1;
    uint16_t pairing_confirm_received: 1;
    uint16_t pairing_dhkey_check_received: 1;
    uint16_t peer_ltk_req_received: 1;
    uint16_t local_key_dist_request: 1;
} smp_conn_flag;

struct smp_conn_item_t;
struct smp_receive_t;

typedef void (*smp_recv_func_t)(struct smp_conn_item_t *smp, const struct smp_receive_t *recv);
typedef void (*smp_rx_handle_t)(struct smp_conn_item_t *smp, smp_event_t event, const struct smp_receive_t *recv);

typedef struct smp_receive_t
{
    uint8_t error_code;
    uint16_t data_len;
    const uint8_t *data;
    smp_recv_func_t cmpl;
} smp_receive_t;

typedef struct smp_conn_item_t
{
    smp_conn_flag flag;
    smp_phase_t phase;
    uint8_t smp_rsp_wait_timer;
    uint8_t init_key_dist;
    uint8_t resp_key_dist;
    uint8_t wait_peer_kdist;
    smp_event_t curr_expect_event;
    smp_pairing_method_t method;
    smp_pairing_req_t pair_req;
    smp_pairing_rsp_t pair_rsp;
    smp_pairing_req_t *l_pair;
    smp_pairing_req_t *p_pair;
    uint8_t tk[GAP_KEY_LEN];
    uint8_t l_rand[GAP_KEY_LEN];
    uint8_t p_rand[GAP_KEY_LEN];
    uint8_t c1_p2_le[GAP_KEY_LEN];
    uint8_t l_confirm[GAP_KEY_LEN];
    uint8_t p_confirm[GAP_KEY_LEN];
    uint8_t p_legacy_ediv[2];
    uint8_t p_legacy_rand[8];
    uint8_t iltk[GAP_KEY_LEN];
    uint8_t ilk[GAP_KEY_LEN];
    const uint8_t *l_secret_key;
    uint8_t l_pkx[GAP_PUB_KEY_LEN];
    uint8_t l_pky[GAP_PUB_KEY_LEN];
    uint8_t p_pkx[GAP_PUB_KEY_LEN];
    uint8_t p_pky[GAP_PUB_KEY_LEN];
    uint8_t l_dhkey[GAP_PUB_KEY_LEN];
    uint8_t l_mackey[GAP_KEY_LEN];
    uint8_t l_dhkey_check_value[GAP_KEY_LEN];
    uint8_t p_dhkey_check_value[GAP_KEY_LEN];
    uint8_t ra[GAP_KEY_LEN];
    uint8_t rb[GAP_KEY_LEN];
    uint8_t a_addr[GAP_ADDR_LEN + 1];
    uint8_t a_iocap[3];
    uint8_t b_addr[GAP_ADDR_LEN + 1];
    uint8_t b_iocap[3];
    const uint8_t *a_rand;
    const uint8_t *b_rand;
    uint32_t user_confirm_value;
    uint32_t passkey;
    uint8_t passkey_entry_repeat;
    uint16_t connhdl;
    gap_conn_item_t *conn;
    struct hci_ev_le_ltk_request ltk_req;
    smp_rx_handle_t curr_rx_handle;
    gap_key_info_t key_data;
} smp_conn_item_t;

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
bt_status_t smp_check_send_ltk_key_reply(gap_conn_item_t *conn, bool negative_reply,
                                         const uint8_t *ltk_reply, uint8_t key_type);
bt_status_t smp_check_enable_encryption(gap_conn_item_t *conn, const gap_ltk_enc_info_t *enc_info,
                                        const uint8_t *ltk);
bt_status_t smp_check_send_pairing_rsp(gap_conn_item_t *conn, const smp_requirements_t *p_require, smp_error_code_t err_code);
bt_status_t smp_check_send_pairing_keys(gap_conn_item_t *conn, const ble_bdaddr_t *ia, const uint8_t *irk, const uint8_t *csrk);
void smp_receive_enc_change(gap_conn_item_t *conn, uint8_t opcode, struct hci_ev_encryption_change_v3 *p);
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