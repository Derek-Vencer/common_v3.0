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
#include <stdio.h>
#include <string.h>
#include "app_bta_cmd.h"
#include "bthost_logger.h"


//Note: Interface statistics not added to the test instructions
// void bta_bt_register_callbacks(bt_callback_user_t user, bt_callbacks_t *callbacks);
// bool bta_get_addr_by_device_id(uint8_t device_id, bt_bdaddr_t *addr);
// uint8_t bta_get_device_id_by_addr(const bt_bdaddr_t *addr);
// bool bta_get_remote_version(const bt_bdaddr_t *addr, bt_remote_version_t *remote_version);
// int bta_l2cap_echo_init(bta_l2cap_process_echo_req_cb process_echo_req,
//                         bta_l2cap_process_echo_rsp_cb process_echo_rsp,
//                         bta_l2cap_fill_in_echo_req_cb fill_in_echo_req);
// int bta_l2cap_send_echo_req(const bt_bdaddr_t *addr, const uint8_t *data, uint16_t data_len);
// int bta_l2cap_send_echo_rsp(const bt_bdaddr_t *addr, uint8_t sigid, const uint8_t *data, uint16_t data_len);
// int bta_bt_set_local_name(const char *name, uint8_t len);
// int bta_bt_get_local_name(char *name_buf, uint8_t buf_len);
// int bta_bt_set_local_addr(const bt_bdaddr_t *addr);
// int bta_bt_get_local_addr(bt_bdaddr_t *addr);
// void bta_register_eir_manufacture_data_callback(void (*cb)(uint8_t *buff, uint32_t *offset));
// bool bta_is_bt_central(const bt_bdaddr_t *addr);
// bool bta_is_in_sniff_mode(const bt_bdaddr_t *addr);
// int bta_update_bt_device_qos(const bt_bdaddr_t *addr, uint8_t tpoll_slot);
// int bta_exit_sniff(const bt_bdaddr_t *addr);
// int bta_request_exit_mhdt_mode(const bt_bdaddr_t *bdaddr);
// int bta_request_enter_mhdt_mode(const bt_bdaddr_t *bdaddr, uint8_t tx_rates, uint8_t rx_rates);
// int bta_prevent_sniff_set(const bt_bdaddr_t *bdaddr, bool enable);
// void bta_register_bt_sniff_config_callback(bool (*cb)(bt_sniff_info_t *));
// void bta_register_mhdt_mode_change_callback(void (*cb)(struct bdaddr_t remote, bool isIn_mhdt_mode));
// void bta_register_hci_log_report_callback(uint16_t max_len,
//                 int (*tx_cb)(const uint8_t *buf, uint16_t len),
//                 int (*rx_cb)(const uint8_t *buf, uint16_t len));
// uint16_t bta_l2cap_get_psm_by_cid(uint16_t conhdl, bool scid, uint16_t cid);
// bt_bdaddr_t *bta_get_pts_address(void);
// void bta_a2dp_register_callbacks(bt_a2dp_sink_callback_user_t user, bt_a2dp_sink_callbacks_t *callbacks);
// void bta_a2dp_register_req_delay_rsp_callback(bt_a2dp_cmd_req_delay_rsp_cb req_delay_cb);
// int bta_a2dp_connect(const bt_bdaddr_t *addr);
// int bta_a2dp_disconnect(const bt_bdaddr_t *addr);
// bool bta_a2dp_is_connected(const bt_bdaddr_t *addr);
// bool bta_a2dp_is_initiator(const bt_bdaddr_t *addr);
// bool bta_a2dp_is_streaming(const bt_bdaddr_t *addr);
// uint8_t bta_a2dp_get_lhdc_llc_config(void);
// bool bta_a2dp_lhdc_get_ext_flags(uint32_t flags);
// int bta_a2dp_set_delay(const bt_bdaddr_t *addr, uint16_t delay_ms);
// bool bta_get_codec_elements(const bt_bdaddr_t *addr, const uint8_t **elements, uint8_t *len);
// uint8_t bta_a2dp_accept_unknown_cmd(const bt_bdaddr_t *bd_addr, const bt_a2dp_unknown_cmd_t *cmd, bool accept);
// bta_profile_status_t bta_get_a2dp_state(const bt_bdaddr_t *addr, bta_a2dp_state_t *a2dp_state);
// int bta_a2dp_codec_init(uint8_t codectype, uint8_t nonetype, uint8_t sep_priority, uint8_t *elements, uint8_t len);
// int bta_a2dp_update_quick_switch_state(const bt_bdaddr_t *addr, bool quick_switch_enable);
// void bta_avrcp_register_callbacks(bt_avrcp_callback_user_t user, bt_avrcp_callbacks_t *callbacks);
// int bta_avrcp_connect(const bt_bdaddr_t *addr);
// int bta_avrcp_disconnect(const bt_bdaddr_t *addr);
// bool bta_avrcp_is_connected(const bt_bdaddr_t *addr);
// int bta_avrcp_send_play(const bt_bdaddr_t *addr);
// int bta_avrcp_send_pause(const bt_bdaddr_t *addr);
// int bta_avrcp_send_forward(const bt_bdaddr_t *addr);
// int bta_avrcp_send_backward(const bt_bdaddr_t *addr);
// int bta_avrcp_send_volume_up(const bt_bdaddr_t *addr);
// int bta_avrcp_send_volume_down(const bt_bdaddr_t *addr);
// int bta_avrcp_send_set_abs_volume(const bt_bdaddr_t *addr, uint8_t volume);
// bta_profile_status_t bta_get_avrcp_state(const bt_bdaddr_t *addr, bta_avrcp_state_t *avrcp_state);
// void bta_hf_register_callbacks(bt_hfp_hf_callback_user_t user, bt_hfp_hf_callbacks_t *callbacks);
// void bta_hf_deinit(void);
// int bta_hf_connect(const bt_bdaddr_t *addr);
// int bta_hf_disconnect(const bt_bdaddr_t *addr);
// bool bta_hf_is_connected(const bt_bdaddr_t *addr);
// int bta_hf_connect_audio_link(const bt_bdaddr_t *addr);
// int bta_hf_disconnect_audio_link(const bt_bdaddr_t *addr);
// bool bta_hf_is_audio_link_connected(const bt_bdaddr_t *addr);
// int bta_hf_call_redial(const bt_bdaddr_t *addr);
// int bta_hf_call_answer(const bt_bdaddr_t *addr);
// int bta_hf_call_hangup(const bt_bdaddr_t *addr);
// int bta_hf_call_hold(const bt_bdaddr_t *addr);
// int bta_hf_3way_hungup_incoming(const bt_bdaddr_t *addr);
// int bta_hf_3way_hungup_active_accept_incomming(const bt_bdaddr_t *addr);
// int bta_hf_battery_report(uint8_t level);
// int bta_hf_control_voice_assistant(bool is_enable);
// bt_hfp_call_state_t bta_hf_get_ciev_call_state(const bt_bdaddr_t *addr);
// bt_hfp_callsetup_state_t bta_hf_get_ciev_callsetup_state(const bt_bdaddr_t *addr);
// bt_hfp_callheld_state_t bta_hf_get_ciev_callheld_state(const bt_bdaddr_t *addr);
// bta_profile_status_t bta_hf_get_hfp_state(const bt_bdaddr_t *addr, bta_hfp_state_t *hfp_state);
// bta_profile_status_t bta_hf_get_hfp_call_status(const bt_bdaddr_t *addr, bta_call_status_t *call_status);
// int bta_hfp_hf_send_battery_level(const bt_bdaddr_t *addr, uint8_t level);
// int bta_spp_init(bt_rfcomm_channel_t local_server_channel, const bta_uuid_t *uuid, const bt_spp_callbacks_t *callbacks,
//                  const char *name, uint8_t name_len, uint16_t rx_buff_size);
// int bta_spp_deinit(bt_rfcomm_channel_t local_server_channel);
// int bta_spp_control_credit_by_self(bt_rfcomm_channel_t local_server_channel, bool enable);
// int bta_spp_initiate_connect(const bt_bdaddr_t *addr, bt_rfcomm_channel_t local_server_channel, const bta_uuid_t *uuid);
// int bta_spp_initiate_disconnect(bt_spp_connhdl_t connhdl, uint8_t reason);
// bool bta_spp_send_data(bt_spp_connhdl_t connhdl, const uint8_t *data, uint16_t size);
// uint16_t bta_spp_get_free_send_buf_size(bt_spp_connhdl_t connhdl);
// bool bta_spp_send_credit(bt_spp_connhdl_t connhdl, uint8_t credits);
// int bta_dip_get_pnp_info(const bt_bdaddr_t *address, bt_dip_pnp_info_t *info);
// bool bta_dip_check_is_ios_device(const bt_bdaddr_t *address);
// bool bta_dip_check_is_ios_device_by_vend_id(uint16_t vend_id, uint16_t vend_id_source);
// void bta_register_dip_info_queried_callback(bt_dip_info_queried_callback func);
// void bta_hid_register_callbacks(bt_hid_callbacks_t *callbacks);
// int bta_hid_connect(const bt_bdaddr_t *addr);
// int bta_hid_disconnect(const bt_bdaddr_t *addr);
// bool bta_hid_is_connected(const bt_bdaddr_t *addr);
// int bta_hid_send_sensor_report(const bt_bdaddr_t *addr, const bt_hid_sensor_report_t *reportr);


static void bta_bt_api_cmd_disconnect_test(const char* param, uint32_t param_len)
{
    bt_bdaddr_t addr;

    if (!app_bta_scan_bdaddr_from_string(param, param_len, &addr))
    {
        return;
    }

    DEBUG_INFO(2, "bta_bt_test disconnect addr:%02x:%02x:%02x:%02x:%02x:%02x",
            addr.address[0], addr.address[1], addr.address[2],
            addr.address[3], addr.address[4], addr.address[5]);

    bta_bt_acl_disconnect((const bt_bdaddr_t *)&addr);
}

static void bta_bt_api_cmd_get_conhandle_test(const char* param, uint32_t param_len)
{
    bt_bdaddr_t addr;

    if (!app_bta_scan_bdaddr_from_string(param, param_len, &addr))
    {
        return;
    }

    DEBUG_INFO(2, "bta_bt_test get_conhdl addr:%02x:%02x:%02x:%02x:%02x:%02x",
            addr.address[0], addr.address[1], addr.address[2],
            addr.address[3], addr.address[4], addr.address[5]);

    uint16_t conhdl = bta_get_conhdl_by_addr((const bt_bdaddr_t *)&addr);

    DEBUG_INFO(0, "bta_bt_test curr access_mode %d", conhdl);
}

static void bta_bt_api_cmd_get_accessmode_test(const char* param, uint32_t param_len)
{
    bt_access_mode_t access_mode = bta_get_access_mode();
    DEBUG_INFO(0, "bta_bt_tes curr access_mode %d", access_mode);
}

static bta_cmd_table_t bta_bt_api_cmd_table[] =
{
    {"bta_bt_disconect",               bta_bt_api_cmd_disconnect_test},
    {"bta_bt_get_conhandle",           bta_bt_api_cmd_get_conhandle_test},
    {"bta_bt_get_accessmode",          bta_bt_api_cmd_get_accessmode_test},
};

void app_bta_bt_api_cmd_init(void)
{
    app_bta_add_cmd_table(sizeof(bta_bt_api_cmd_table)/sizeof(bta_bt_api_cmd_table[0]),
        bta_bt_api_cmd_table);
}
