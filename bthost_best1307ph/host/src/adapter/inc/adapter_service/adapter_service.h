/***************************************************************************
 *
 * Copyright 2015-2022 BES.
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
#ifndef __BT_ADAPTER_LAYER_H__
#define __BT_ADAPTER_LAYER_H__
#include "bt_common_define.h"
#include "bt_stack_status.h"
#include "bt_stack_event.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "spp_service.h"
#include "hci_service.h"
#include "l2cap_service.h"
#include "a2dp_service.h"
#include "hfp_service.h"
#include "avrcp_service.h"
#include "hfp_api.h"
#include "hid_api.h"
#include "map_api.h"
#include "pan_api.h"
#include "dip_api.h"
#include "me_api.h"

typedef int (*bt_adapter_callback_t)(const bt_bdaddr_t *bd_addr, bt_adapter_event_t event, bt_adapter_callback_param_t param);

bt_status_t bt_adapter_init(bt_adapter_callback_t callback);
bt_status_t bt_adapter_cleanup(void);
bt_status_t bt_adapter_start_inquiry(void);
bt_status_t bt_adapter_cancel_inquiry(void);
bt_status_t bt_adapter_connect_acl_with_page_timeout(const bt_bdaddr_t *bd_addr, uint32_t page_timeout, uint32_t time_to_next_page);
bt_status_t bt_adapter_connect_acl(const bt_bdaddr_t *bd_addr);
bt_status_t bt_adapter_disconnect_acl(const bt_bdaddr_t *bd_addr);
bt_status_t bt_adapter_create_bond(const bt_bdaddr_t *bd_addr);
bt_status_t bt_adapter_remove_bond(const bt_bdaddr_t *bd_addr);
bt_status_t bt_adapter_set_property(const bt_adapter_property_param_t *property);

typedef struct _hfp_adapter_context
{
    bool hfp_is_connected;
    uint8_t hfp_call_state;
    uint8_t hfp_callsetup_state;
    uint8_t hfp_callhold_state;
    uint8_t hfp_speak_vol;
} HFP_ADAPTER_CONTEXT;

typedef struct _a2dp_adapter_context
{
    bool a2dp_is_connected;
    bool a2dp_is_streaming;
    uint8_t a2dp_codec_type;
    uint8_t a2dp_sample_rate;
    bool rsv_avdtp_start_signal;
} A2DP_ADAPTER_CONTEXT;

typedef struct _avrcp_adapter_context
{
    bool avrcp_is_connected;
    uint8_t avrcp_playback_status;
} AVRCP_ADAPTER_CONTEXT;

struct BT_ADAPTER_DEVICE_T {
    bt_bdaddr_t remote;
    uint8_t device_id;
    bool acl_is_connected;
    uint8_t acl_bt_role;
    uint8_t acl_link_mode;
    uint16_t acl_conn_hdl;
    uint16_t sniff_interval;

    bool sco_is_connected;
    uint8_t sco_codec_type;
    uint16_t sco_handle;

    uint8_t is_use_local_sbm;
    uint16_t media_active;

    HFP_ADAPTER_CONTEXT hfp_ctx;
    A2DP_ADAPTER_CONTEXT a2dp_ctx;
    AVRCP_ADAPTER_CONTEXT avrcp_ctx;
};

#define BT_ADAPTER_MAX_DEVICE_NUM BT_DEVICE_NUM

struct BT_ADAPTER_MANAGER_T {
    struct BT_ADAPTER_DEVICE_T bt_device[BT_DEVICE_NUM];
    struct BT_ADAPTER_DEVICE_T bt_tws_device;
#ifdef RTOS
    osMutexId adapter_lock;
#endif
    uint8_t access_mode;
    bt_iocap_requirement_t iocap_requirement;
};

void bt_add_event_callback(bt_event_callback_t cb, uint32_t masks);

typedef struct bes_aud_bt_t {
    void (*aud_set_stay_active_mode)(bool keep_active);
    uint8_t (*aud_get_curr_a2dp_device)(void); // app_bt_audio_get_curr_a2dp_device
    uint8_t (*aud_get_curr_sco_device)(void); // app_bt_audio_get_curr_sco_device
    bool (*aud_is_sco_prompt_play_mode)(void); // app_bt_manager.config.sco_prompt_play_mode
    uint8_t (*aud_get_max_sco_number)(void); // btif_sco_get_max_number
    uint8_t (*aud_get_a2dp_codec_type)(int device_id); // bt_a2dp_player_get_codec_type
    int (*aud_switch_sco)(uint16_t sco_handle); // app_bt_Me_switch_sco
    void (*aud_report_hfp_speak_gain)(void); // btapp_hfp_report_speak_gain
    void (*aud_report_a2dp_speak_gain)(void); // btapp_a2dp_report_speak_gain
    bool (*aud_hfp_mic_need_skip_frame)(void); // btapp_hfp_mic_need_skip_frame
    uint8_t (*aud_hfp_need_mute)(void); // btapp_hfp_need_mute
    void (*aud_hfp_set_local_vol)(int id, uint8_t vol); // hfp_volume_local_set
    uint8_t (*aud_adjust_hfp_volume)(uint8_t device_id, bool up, bool adjust_local_vol_level, uint8_t vol_step); // app_bt_hfp_adjust_volume
    uint8_t (*aud_adjust_a2dp_volume)(uint8_t device_id, bool up, bool adjust_local_vol_level, uint8_t vol_step); // app_bt_a2dp_adjust_volume
    bool (*ignore_ring_and_play_tone_self)(int device_id);
#if defined(A2DP_LHDC_ON) || defined(A2DP_LHDCV5_ON)
    bool (*a2dp_lhdc_get_ext_flags)(uint32_t flags); // a2dp_lhdc_get_ext_flags
    uint8_t (*a2dp_lhdc_config_llc_get)(void); // a2dp_lhdc_config_llc_get
#endif
    uint8_t (*a2dp_get_non_type_by_device_id)(uint8_t device_id);
} bes_aud_bt_t;

typedef struct {
    void (*key_click_func)(void);
    void (*key_doubleclick_func)(void);
    void (*key_tripleclick_func)(void);
    void (*key_longpress_func)(void);
}bta_key_click_event_cb_t;

extern const bes_aud_bt_t * const bes_aud_bt;

struct BT_ADAPTER_DEVICE_T *bt_adapter_get_device(int device_id);
struct BT_ADAPTER_DEVICE_T *bt_adapter_get_connected_device_by_id(int device_id);
struct BT_ADAPTER_DEVICE_T *bt_adapter_get_connected_device_by_connhdl(uint16_t connhdl);
struct BT_ADAPTER_DEVICE_T *bt_adapter_get_connected_device_byaddr(const bt_bdaddr_t *remote);
uint8_t bt_adapter_get_hfp_sco_codec_type(int device_id);
uint8_t bt_adapter_count_mobile_link(void);
uint8_t bt_adapter_count_connected_hfp(void);
uint8_t bt_adapter_count_streaming_a2dp(void);
uint8_t bt_adapter_count_streaming_sco(void);
uint8_t bt_adapter_has_incoming_call(void);
int bt_adapter_get_device_id_by_connhdl(uint16_t connhdl);
int bt_adapter_get_device_id_byaddr(const bt_bdaddr_t *remote);
void bt_adapter_mutex_lock(void);
void bt_adapter_mutex_unlock(void);
void bt_adapter_manager_init(void);
void bt_adapter_callback_func_init(void);
void bt_adapter_set_extra_acl_conn_req_update(extra_acl_conn_req_callback func);
void bt_adapter_local_volume_down(void);
void bt_adapter_local_volume_up(void);
void bt_adapter_local_volume_down_with_callback(void (*cb)(uint8_t device_id));
void bt_adapter_local_volume_up_with_callback(void (*cb)(uint8_t device_id));
void bt_adapter_set_a2dp_codec_info(int device_id, uint8_t codec_type, uint8_t sample_rate, uint8_t sample_bit);
void bt_adapter_set_hfp_sco_codec_type(int device_id, uint8_t sco_codec);

void bt_adapter_report_acl_connected(const bt_bdaddr_t *bd_addr, const bt_adapter_acl_opened_param_t *acl_con);
void bt_adapter_report_acl_disconnected(const bt_bdaddr_t *bd_addr, const bt_adapter_acl_closed_param_t *acl_dis);
void bt_adapter_report_sco_connected(const bt_bdaddr_t *bd_addr, const bt_adapter_sco_opened_param_t *sco_con);
void bt_adapter_report_sco_disconnected(const bt_bdaddr_t *bd_addr, const bt_adapter_sco_closed_param_t *sco_dis);
void bt_adapter_report_access_change(const bt_bdaddr_t *bd_addr, const bt_adapter_access_change_param_t *access_change);
void bt_adapter_report_role_discover(const bt_bdaddr_t *bd_addr, const bt_adapter_role_discover_param_t *role_discover);
void bt_adapter_report_role_change(const bt_bdaddr_t *bd_addr, const bt_adapter_role_change_param_t *role_change);
void bt_adapter_report_mode_change(const bt_bdaddr_t *bd_addr, const bt_adapter_mode_change_param_t *mode_change);
void bt_adapter_report_authenticated(const bt_bdaddr_t *bd_addr, const bt_adapter_authenticated_param_t *auth);
void bt_adapter_report_enc_change(const bt_bdaddr_t *bd_addr, const bt_adapter_enc_change_param_t *enc_change);
void bt_adapter_report_inquiry_result(const bt_bdaddr_t *bd_addr, const bt_adapter_inquiry_result_param_t *inq_result);
void bt_adapter_report_inquiry_complete(const bt_bdaddr_t *bd_addr, const bt_adapter_inquiry_complete_param_t *inq_complete);
void bt_adapter_echo_register(void(*echo_req)(const bt_bdaddr_t* bdaddr,uint8_t id, uint8_t* data, uint8_t len),
                              void(*echo_rsp)(const bt_bdaddr_t* bdaddr,uint8_t id, uint8_t* data, uint8_t len));
uint8_t bt_adapter_echo_req_send(const bt_bdaddr_t *bdaddr, uint8_t *data, uint16_t len, uint8_t *sigid);
uint8_t bt_adapter_echo_rsp_send(const bt_bdaddr_t *bdaddr, uint8_t *data, uint16_t len, uint8_t sigid);
void bt_adapter_export_esco_param_cb(void(*cb)(void *bdaddr, uint8_t tesco, uint8_t wesco));
#ifdef BT_HFP_SUPPORT
void bt_adapter_report_hfp_connected(const bt_bdaddr_t *bd_addr, const bt_hf_opened_param_t *hfp_conn);
void bt_adapter_report_hfp_disconnected(const bt_bdaddr_t *bd_addr, const bt_hf_closed_param_t *hfp_disc);
void bt_adapter_report_hfp_ring(const bt_bdaddr_t *bd_addr, const void *param);
void bt_adapter_report_hfp_clip_ind(const bt_bdaddr_t *bd_addr, const bt_hf_clip_ind_param_t *hfp_caller_ind);
void bt_adapter_report_hfp_call_state(const bt_bdaddr_t *bd_addr, const bt_hf_call_ind_param_t *hfp_call);
void bt_adapter_report_hfp_callsetup_state(const bt_bdaddr_t *bd_addr, const bt_hf_callsetup_ind_param_t *hfp_callsetup);
void bt_adapter_report_hfp_callhold_state(const bt_bdaddr_t *bd_addr, const bt_hf_callheld_ind_param_t *hfp_callhold);
void bt_adapter_report_hfp_volume_change(const bt_bdaddr_t *bd_addr, const bt_hf_volume_change_param_t *volume_change);
#endif /* BT_HFP_SUPPORT */
#ifdef BT_A2DP_SUPPORT
void bt_adapter_report_a2dp_connected(const bt_bdaddr_t *bd_addr, const bt_a2dp_opened_param_t *a2dp_conn);
void bt_adapter_report_a2dp_disconnected(const bt_bdaddr_t *bd_addr, const bt_a2dp_closed_param_t *a2dp_disc);
void bt_adapter_report_a2dp_stream_start(const bt_bdaddr_t *bd_addr, const bt_a2dp_stream_start_param_t *a2dp_stream_start);
void bt_adapter_report_a2dp_stream_reconfig(const bt_bdaddr_t *bd_addr, const bt_a2dp_stream_reconfig_param_t *a2dp_stream_reconfig);
void bt_adapter_report_a2dp_stream_suspend(const bt_bdaddr_t *bd_addr, const bt_a2dp_stream_suspend_param_t *a2dp_stream_suspend);
void bt_adapter_report_a2dp_stream_close(const bt_bdaddr_t *bd_addr, const bt_a2dp_stream_close_param_t *a2dp_stream_close);
#endif /* BT_A2DP_SUPPORT */
#ifdef BT_AVRCP_SUPPORT
void bt_adapter_report_avrcp_connected(const bt_bdaddr_t *bd_addr, const bt_avrcp_opened_t *avrcp_conn);
void bt_adapter_report_avrcp_disconnected(const bt_bdaddr_t *bd_addr, const bt_avrcp_closed_t *avrcp_disc);
void bt_adapter_report_avrcp_status_changed(const bt_bdaddr_t *bd_addr, const bt_avrcp_play_status_change_t *avrcp_changed);
#endif /* BT_AVRCP_SUPPORT */
void bes_bt_a2dp_get_device_codec_info(uint8_t dev_num, uint8_t *codec);

void app_bt_get_local_device_address(void *bd_addr);

void bt_adapter_register_key_evt_handle_cb(const bta_key_click_event_cb_t *cbs);

void bta_register_bt_sink_callback(void *cbs);

void bta_register_tws_ibrt_callback(void *cbs);

bt_status_t app_bt_write_sleep_enable(uint8_t user, bool enable);

void bt_adapter_register_eir_manufacture_data_callback(void (*cb)(uint8_t *buff, uint32_t *offset));

#ifdef __cplusplus
}
#endif
#endif /* __BT_ADAPTER_LAYER_H__ */
