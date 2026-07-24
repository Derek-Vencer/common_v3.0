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
#ifndef __APP_TWS_IBRT_CMD_HANDLER_H__
#define __APP_TWS_IBRT_CMD_HANDLER_H__
#include "app_tws_ibrt.h"
#include "app_tws_ibrt_cmd_audio_analysis.h"
#include "app_tws_profile_sync.h"
#include "app_ibrt_voice_report.h"
#include "bts_tws_types.h"

#ifndef APP_IBRT_CMD_RSP_MASK
#define APP_IBRT_CMD_RSP_MASK       0x80000000
#endif

#ifndef APP_IBRT_FULL_CMD_MASK
#define APP_IBRT_FULL_CMD_MASK      0x0000FFFF
#endif

#ifndef APP_IBRT_CMD_PREFIX
#define APP_IBRT_CMD_PREFIX         0x0000
#endif

#define IBRT_DESC_SIZE                                    (8)

#define IBRT_WAIT_PROFILE_TIMEOUT                             (10000)
#define IBRT_NEW_PROFILE_WAIT_TIMEOUT                         (245)
#define IBRT_WAIT_PROFILE_EXCHANGE_COMPLETE_TIMEOUT           (500)

#define APP_TWS_IBRT_MAX_DATA_SIZE              (672)
#define APP_TWS_IBRT_CMDCODE_SIZE               sizeof(uint16_t)
#define APP_TWS_IBRT_CMDSEQ_SIZE                sizeof(uint16_t)
#define APP_TWS_IBRT_CMDHEAD_SIZE               (APP_TWS_IBRT_CMDCODE_SIZE+APP_TWS_IBRT_CMDSEQ_SIZE)

#ifndef FREEMAN_ENABLED_STERO
#define IBRT_BESAUD_RX_BUFF_SIZE                (672)
#define IBRT_BESAUD_TX_BUFF_SIZE                (672*4)
#else
#define IBRT_BESAUD_RX_BUFF_SIZE                (0)
#define IBRT_BESAUD_TX_BUFF_SIZE                (0)
#endif
#define IBRT_TIMEOUT_INVALID                    (0)

#define app_ibrt_sync_dts_coc_data_handler_null (0)

#define RSP_TIMEOUT_VOICE_REPORT        (1000)

typedef enum
{
    APP_TWS_ACCEPT,
    APP_TWS_NOT_ACCEPT,
} app_tws_switch_e;

#define IBRT_TWS_PERFORM_USER_ACITON                    0xff
#define IBRT_TWS_PERFORM_CONNECT_SECOND_MOBILE          0x01
#define IBRT_TWS_PERFORM_DISCONNECT_MOBILE_TWS_LINK     0x02
#define IBRT_TWS_PERFORM_PEER_TWS_SWITCH_DONE           0x03
#define IBRT_TWS_PERFORM_AVRCP_NOTIFY_REGISTER          0x04
#define IBRT_TWS_PERFORM_COMMON_SM_WITH_MOBILE          0x05
#define IBRT_TWS_PERFORM_A2DP_STREAM_COMMAND_RSP_ACTION 0x06
#define IBRT_TWS_PERFORM_A2DP_STREAM_COMMAND_RSP_DONE   0x07
#define IBRT_TWS_PERFORM_SCO_CODEC_INFO_SYNC            0x08
#define IBRT_TWS_PERFORM_DELETE_PEER_NV_ADDR            0x09
#define IBRT_TWS_PERFORM_NEW_MASTER_READY_REQ           0x0A
#define IBRT_TWS_PERFORM_NEW_MASTER_READY_RSP           0x0B
#define IBRT_TWS_PERFORM_RELEASE_SPP_DLC_CONNECTION     0x0C
#define IBRT_TWS_PERFORM_ACRCP_CT_SDP_INFO              0x0D
#define IBRT_TWS_PERFORM_SET_TWS_LINK_ID_REQ            0x0E
#define IBRT_TWS_PERFORM_SET_TWS_LINK_ID_RSP            0x0F
#define IBRT_TWS_PERFORM_PROFILE_COMPLETE               0x10

#define IBRT_TWS_CMD_MAX_CACHE_MSG                      (6)

typedef void (*app_tws_cmd_send_via_ble_t)(uint8_t*, uint16_t);
typedef bool (*app_tws_exchange_profile_immediate)(void *remote);

typedef struct
{
    bool                            used;
    uint32_t                        last_cmdseq;
    uint32_t                        system_time;
    uint16_t                        cmd_len;
    uint8_t                         *cmd_buffer;
} app_tws_cmd_cache_msg_t;

typedef struct
{
    struct list_node                node;
    app_tws_cmd_cache_msg_t         curr_msg;
    app_tws_cmd_cache_msg_t         cache_msg[IBRT_TWS_CMD_MAX_CACHE_MSG];
    // void                            *msg_p;
    // void                            *refresh_msg_p;
    int32_t                         timeout_ms;
    uint16_t                        cmdcode;
} app_tws_cmd_timer_instance_t;

typedef enum
{
    HANDLE_BASED,
    ADDR_BASED,
} cmd_type_e;

typedef struct
{
    uint16_t                        opcode;
    cmd_type_e                      cmd_type;
    uint8_t                         type_pos;
    const char                      *log_cmd_code_str;
} __attribute__((packed)) app_ibrt_cmd_filter_t;

typedef struct
{
    uint16_t  cmdcode;
    uint16_t  cmdseq;
    uint8_t   content[APP_TWS_IBRT_MAX_DATA_SIZE];
} __attribute__((packed)) app_tws_ibrt_cmd_t;

#ifdef __cplusplus
extern "C" {
#endif

void app_ibrt_cmd_handler_init(void);
void app_ibrt_cmd_handler_register_cmd_tx_done_cb(bt_tws_cmd_tx_done_handler_t handler);
void app_ibrt_clear_cmd_mailbox(void);
void app_ibrt_send_cmd_via_ble_register(app_tws_cmd_send_via_ble_t func);
void app_ibrt_cmd_rx_handler(uint8_t* p_data_buff, uint16_t length);
void app_ibrt_data_send_handler(void);
void app_ibrt_data_receive_handler(void);
void app_ibrt_cmd_rsp_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
bool app_ibrt_send_cmd_without_rsp(uint16_t cmdcode, uint8_t *p_buff, uint16_t length);
void app_ibrt_send_cmd_with_rsp(uint16_t cmdcode, uint8_t *p_buff, uint16_t length);
void bts_tws_add_cmd_table(app_tws_cmd_user_e user, uint8_t cmd_number, const bt_tws_cmd_instance_t *cmd_table);

void app_ibrt_set_codec_type(uint8_t *p_buff, uint16_t length);
void app_ibrt_set_codec_type_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_send_tws_switch_cmd(uint8_t *p_buff, uint16_t length);
void app_ibrt_tws_switch_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_send_cmd_rsp(uint8_t *p_buff, uint16_t length);
bool app_ibrt_send_cmd_process_timeout(uint16 ms);
void app_ibrt_set_codec_type_rsp_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_set_codec_type_rsp_timeout_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);

void app_ibrt_send_conn_profile_req(uint8_t *p_buff, uint16_t length);
void app_ibrt_conn_profile_req_handler(uint16_t req_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_conn_profile_rsp_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_send_disc_profile_req(uint8_t *p_buff, uint16_t length);
void app_ibrt_disc_profile_req_handler(uint16_t req_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_disc_profile_rsp_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_send_disc_rfcomm_req(uint8_t *p_buff, uint16_t length);
void app_ibrt_disc_rfcomm_req_handler(uint16_t req_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_disc_rfcomm_rsp_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_new_master_is_ready(const bt_bdaddr_t *remote);
void app_ibrt_sync_profile_init_status(ibrt_mobile_info_t *p_mobile_info, uint64_t profile_mask);
void app_ibrt_exchange_profile_timer_deinit(ibrt_mobile_info_t *p_mobile_info);
uint8_t app_ibrt_send_profile_data(ibrt_mobile_info_t *p_mobile_info,uint64_t profile_mask,bool tx_silence);
void app_ibrt_fast_ack_req_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_send_fast_ack_req(uint8_t *p_buff, uint16_t length);
void app_ibrt_ibrt_fast_ack_rsp_timeout_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_fast_ack_rsp_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_sync_tota_encode_status_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_sync_tota_encode_status(uint8_t *p_buff, uint16_t length);
void app_ibrt_perform_action(uint8_t *p_buff, uint16_t length);
void app_ibrt_perform_action_handler_v2(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_init_tws_trx_cmd_queue(void);
void app_ibrt_reset_tws_trx_cmd_queue(void);
void app_ibrt_notify_link_policy(uint8_t *p_buff, uint16_t length);
void app_ibrt_notify_link_policy_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_ui_debug_controller_timer_cb(void const *current_evt);
void app_ibrt_recieve_bt_controller_profile_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_send_bt_controller_profile(uint8_t *p_buff, uint16_t input_length);
void app_ibrt_sync_volume_info_send(uint8_t *p_buff, uint16_t length);
void app_ibrt_get_volume_info_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_share_common_info(uint8_t *p_buff, uint16_t length);
void app_ibrt_share_common_info_hanlder(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_share_common_info_rsp_timeout_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_share_common_info_rsp_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);

void app_ibrt_let_master_prepare_rs(uint8_t *p_buff, uint16_t length);
void app_ibrt_master_prepare_rs(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_let_slave_continue_rs(uint8_t *p_buff, uint16_t length);
void app_ibrt_slave_continue_rs(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_send_mobile_link_playback_info(uint8_t *p_buff, uint16_t length);
void app_ibrt_send_mobile_link_playback_info_handler(uint16_t rsp_seq, uint8_t *ptrParam, uint16_t paramLen);

void app_ibrt_custom_play_speed_tuning_rsp_timeout_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_custom_play_speed_tuning_rsp_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);

#define APP_TWS_EXT_CMDTYPE_MASK 0xe800

#define APP_TWS_EXT_CMDCODE_MASK 0xefff

#define APP_TWS_EXT_CMD_RSP_BIT  0x1000

#define APP_TWS_BESAPP_EXT_CMD_PREFIX 0xe100

#define APP_TWS_CUSTOM_EXT_CMD_PREFIX 0xe200

typedef struct
{
    uint16_t                        ext_cmdcode;
    uint16_t                        ext_cmdseq;
} app_tws_ext_cmd_head_t;

typedef void (*app_tws_ext_cmd_rx_handler)(bool is_response, app_tws_ext_cmd_head_t *extcmd, uint32_t length);

typedef struct
{
    uint16_t                        ext_cmdcode;
    const char                      *ext_cmdcode_str;
    app_tws_ext_cmd_rx_handler    ext_cmd_rx_handler;
} app_tws_ext_cmd_handler_t;

typedef struct
{
    uint16_t                        ext_cmdtype;
    uint16_t                        ext_cmd_count;
    const app_tws_ext_cmd_handler_t *first_ext_cmd_handler;
} app_tws_ext_cmd_table;

bool app_ibrt_register_ext_cmd_table(const app_tws_ext_cmd_handler_t* first_cmd, uint16_t cmd_count);

void app_ibrt_tws_send_ext_cmd(uint16_t cmdcode, app_tws_ext_cmd_head_t *cmd, uint32_t length);

void app_ibrt_tws_send_ext_cmd_rsp(uint16_t cmdcode, app_tws_ext_cmd_head_t *cmd, uint32_t length);

void app_tws_ibrt_allow_send_profile_init(app_tws_exchange_profile_immediate allow_send_profile_immediatelly);

#ifdef ENHANCED_TWS_UI_ENABLED
typedef void (*preview_ui_handler_t)(uint8_t *buf, uint16_t len);
void set_preview_ui_consensus_tws_msg_handler(preview_ui_handler_t handler, preview_ui_handler_t jumbo_handler);
#endif

#ifdef __cplusplus
}
#endif

void app_ibrt_sync_tota_leak_deteck(uint8_t *p_buff, uint16_t length);
void app_ibrt_sync_tota_leak_deteck_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
uint8_t app_ibrt_tota_get_leak_detect_status();
void app_ibrt_tota_send_leak_deteck_status(uint8_t *p_buff, uint16_t length);
void app_ibrt_tota_send_leak_detect_status_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_conn_send_user_action_v2(uint8_t *p_buff, uint16_t length);

const bt_tws_cmd_instance_t *bts_tws_find_cmd_instance(uint16_t cmdcode);
app_tws_cmd_timer_instance_t *bts_tws_get_cmd_timer_instance(uint16_t cmdcode);
void bts_tws_add_cmd_timer_instance(app_tws_cmd_timer_instance_t *instance);
void bts_tws_remove_cmd_timer_instance(app_tws_cmd_timer_instance_t *instance);
void bts_tws_ctrl_callback_pending_msg_timeout(app_tws_cmd_timer_instance_t *instance, const bt_tws_cmd_instance_t *cmd_handler);
void app_ibrt_conn_notify_peer_profile_complete(const bt_bdaddr_t *addr);
#endif/*__APP_TWS_IBRT_CMD_HANDLER__*/
