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
#ifndef __BTS_LEA_API_H__
#define __BTS_LEA_API_H__

#include "bts_lea_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void bts_lea_event_callback_register(const bt_lea_evt_callback cb);

void bts_lea_event_callback_unregister(const bt_lea_evt_callback cb);

ble_bdaddr_t *bts_lea_conn_get_remote_address(uint8_t con_lid);

uint8_t bts_lea_conn_get_remote_con_lid(const ble_bdaddr_t *addr);

void bts_lea_gattc_delete_cache(const ble_bdaddr_t *addr, uint32_t svc_uuid);

void bts_lea_gattc_delete_all_cache(void);

void bts_lea_bap_capa_srv_get_ava_context_bf(const ble_bdaddr_t *addr, uint16_t *context_bf_ava_sink, uint16_t *context_bf_ava_src);

void bts_lea_bap_capa_srv_set_ava_context_bf(const ble_bdaddr_t *addr, uint16_t context_bf_ava_sink, uint16_t context_bf_ava_src);

void bts_lea_csip_set_member_rank(uint8_t rank);

bool bts_lea_csip_set_device_numbers(uint8_t dev_num);

uint8_t bts_lea_csip_get_device_numbers(void);

void bts_lea_csip_set_sirk_encrypted(bool encrypted);

bool bts_lea_csip_is_use_custom_sirk(void);

void bts_lea_csip_get_rsi(bt_lea_rsi_t *rsi);

void bts_lea_mcp_media_play(const ble_bdaddr_t *addr);

void bts_lea_mcp_media_pause(const ble_bdaddr_t *addr);

void bts_lea_mcp_media_stop(const ble_bdaddr_t *addr);

void bts_lea_mcp_media_next(const ble_bdaddr_t *addr);

void bts_lea_mcp_media_prev(const ble_bdaddr_t *addr);

void bts_lea_mcp_media_fast_fw(const ble_bdaddr_t *addr);

void bts_lea_mcp_media_fast_rw(const ble_bdaddr_t *addr);

uint8_t bts_lea_mcp_get_media_state(const ble_bdaddr_t *addr);

void bts_lea_vcp_volume_mute(void);

void bts_ble_vcp_volume_unmute(void);

void bts_lea_vcp_volume_up(const ble_bdaddr_t *addr);

void bts_lea_vcp_volume_down(const ble_bdaddr_t *addr);

uint8_t bts_lea_vcp_convert_le_vol_to_local_vol(uint8_t le_vol);

uint8_t bts_lea_vcp_get_real_time_volume(const ble_bdaddr_t *addr);

uint8_t bts_lea_micp_get_mic_state(const ble_bdaddr_t *addr);

void bts_lea_micp_set_mute(uint8_t mute);

void bts_lea_ccp_originate_call(const ble_bdaddr_t *addr, uint8_t *uri, uint8_t uri_len);

void bts_lea_ccp_retrieve_call(const ble_bdaddr_t *addr, uint8_t call_id);

void bts_lea_ccp_join_call(const ble_bdaddr_t *addr, uint8_t *call_ids, uint8_t call_num);

void bts_lea_ccp_hold_call(const ble_bdaddr_t *addr, uint8_t call_id);

void bts_lea_ccp_terminate_call(const ble_bdaddr_t *addr, uint8_t call_id);

void bts_lea_ccp_accept_call(const ble_bdaddr_t *addr, uint8_t call_id);

uint8_t bts_lea_ccp_get_any_call_id(const ble_bdaddr_t *addr, uint8_t *p_call_id);

bool bts_lea_ccp_is_device_call_active(const ble_bdaddr_t *addr);

int bts_lea_bis_stream_set_resume_callback(void (*resume_cb)(uint8_t con_lid, uint32_t param));

void bts_lea_sink_streaming_handle_event(uint8_t con_lid, uint8_t data, bt_lea_direction_t direction, bts_lea_audio_event_t event);

bts_lea_audio_policy_cfg_t *bts_lea_get_audio_policy_config(void);

void bts_lea_audio_switch_focus(uint8_t local_tws_role);

void bts_lea_audio_switch_uc_foucs_cmp_register(void (*uc_switch_cb)(uint8_t con_lid));

void bts_lea_dump_conn_state(void);

uint16_t bts_lea_bis_src_get_bis_hdl_by_big_id(uint8_t big_id);

const bt_lea_codec_id_t *bts_lea_bis_src_get_codec_id_by_big_id(uint8_t big_id, uint8_t subgrp_idx);

const bt_lea_codec_cfg_t *bts_lea_bis_src_get_codec_cfg_by_big_id(uint8_t big_id);

uint32_t bts_lea_bis_src_get_iso_intv_ms_by_big_id(uint8_t big_id);

uint8_t bts_lea_bis_src_send_iso_data_to_all_chnl(uint8_t **payload, uint16_t payload_len, uint32_t ref_time);

int bts_lea_bis_src_get_free_iso_pkt_num(void);

void bts_lea_bis_sink_start(bts_lea_bis_sink_start_param_t *p_param);

void bts_lea_bis_stop_scan(void);

void bts_lea_bis_sink_set_src_id_key(uint8_t *bcast_id, uint8_t *bcast_code);

uint16_t bts_lea_bis_sink_get_pa_hdl_by_pa_lid(uint8_t pa_lid);

void bts_lea_bis_sink_stop();

void bts_lea_bis_sink_set_aud_location_bf(uint32_t aud_loc_bf);

void bts_lea_bis_scan_pa_sync_with_to(const ble_bdaddr_t *p_addr, uint8_t adv_sid, uint16_t sync_to_s);

void bts_lea_bis_scan_pa_sync_cancel(void);

void bts_lea_bis_scan_pa_sync_stop(void);

void bts_lea_bis_scan_past_info_send(uint16_t sync_hdl);

const bt_lea_ascs_ase_t *bts_lea_get_ascs_ase_info(uint8_t ase_lid);

bool bts_lea_is_device_ase_in_streaming_state(const ble_bdaddr_t *addr);

uint8_t bts_lea_get_audio_play_curr_con_lid(void);

uint8_t bts_lea_get_curr_streaming_ase_lid(const ble_bdaddr_t *addr, bt_lea_direction_t direction);

bt_lea_context_type_t bts_lea_get_curr_context_bf_by_ase_lid(uint8_t ase_lid);

void bts_lea_ascs_disable_ase(uint8_t ase_lid);

uint8_t bts_lea_get_stream_ready_ase_lid_list(const ble_bdaddr_t *addr, uint8_t *ase_lid_list);

uint32_t bts_lea_bap_capa_get_location_bf(bt_lea_direction_t direction);

void bts_lea_bap_capa_set_location_bf(uint32_t location_bf);

void bts_lea_iso_dp_data_recv_callback_register(void (*iso_recv_cb)(uint16_t conhdl, uint8_t pkt_status));

void bts_lea_iso_dp_data_recv_callback_deregister(void);

void *bts_lea_iso_dp_get_rx_packet(uint16_t iso_hdl, bts_lea_iso_dp_pkt_buf_t *p_iso_buffer);

void bts_lea_iso_dp_rx_iso_pkt_clean(uint16_t iso_hdl);

void bts_lea_iso_dp_send_data(uint16_t conhdl, uint16_t seq_num, uint8_t *payload, uint16_t payload_len, uint32_t ref_time);

void bts_lea_iso_dp_tx_iso_pkt_clean(uint16_t iso_hdl);

uint8_t bts_lea_get_aud_location_l_r_cnt(uint32_t audio_location_bf);

uint8_t *bts_lea_get_ltv_value_by_type(bt_lea_ltv_data_t *p_ltv_data, uint8_t ltv_type);

void bts_lea_mobile_stream_start(uint8_t con_lid, bts_lea_ase_stream_param_t *p_cfg, bool bidirectional);

const bts_lea_ascc_ase_t *bts_lea_get_ascc_ase_info(uint8_t ase_lid);

void bts_lea_bap_ascc_set_sdu_interval(uint32_t sdu_intv_c2p_us, uint32_t sdu_intv_p2c_us);

void bts_lea_bap_ascc_set_cis_count_in_cig(uint8_t cis_count);

void bts_lea_bap_ascc_prepare_cig_param(const bts_lea_cig_param_t *cig_param);

void bts_lea_bap_ascs_set_codec_config(uint8_t ase_lid, const bt_lea_codec_id_t *codec_id,
                                       const bt_lea_qos_req_t *ntf_qos_req,
                                       const bt_lea_codec_cfg_t *ntf_codec_cfg);

/**
 * @brief Enable or Disable LEA Feature.
 *
 * @param[in] enable    enable or disable
 */
void bts_lea_switch(bool enable);

#ifdef __cplusplus
}
#endif

#endif /* __BTS_LEA_API_H__ */