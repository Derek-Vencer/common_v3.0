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
#ifndef __BT_HFP_SERVICE_H__
#define __BT_HFP_SERVICE_H__
#include "adapter_service.h"
#include "hfp_api.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * standard hfp (hf role) interface
 *
 */

typedef enum {
    BT_HF_EVENT_OPENED = BT_EVENT_HF_OPENED,
    BT_HF_EVENT_CLOSED,
    BT_HF_EVENT_AUDIO_OPENED,
    BT_HF_EVENT_AUDIO_CLOSED,
    BT_HF_EVENT_NETWORK_STATE,
    BT_HF_EVENT_NETWORK_ROAMING,
    BT_HF_EVENT_NETWORK_SIGNAL,
    BT_HF_EVENT_BATTERY_LEVEL,
    BT_HF_EVENT_CALL_IND,
    BT_HF_EVENT_CALLSETUP_IND,
    BT_HF_EVENT_CALLHELD_IND,
    BT_HF_EVENT_RING_IND,
    BT_HF_EVENT_CLIP_IND,
    BT_HF_EVENT_CALL_WAITING_IND,
    BT_HF_EVENT_RESP_AND_HOLD,
    BT_HF_EVENT_CURRENT_CALLS,
    BT_HF_EVENT_VOLUME_CHANGE,
    BT_HF_EVENT_AT_CMD_COMPLETE,
    BT_HF_EVENT_CURRENT_OPERATOR,
    BT_HF_EVENT_SUBSCRIBER_INFO,
    BT_HF_EVENT_VOICE_RECOGNITION_STATE,
    BT_HF_EVENT_IN_BAND_RING_STATE,
    BT_HF_EVENT_LAST_VOICE_TAG_NUMBER,
    BT_HF_EVENT_RECEIVE_UNKNOWN,
    BT_HF_EVENT_END,
} bt_hf_event_t;

#if BT_HF_EVENT_END != BT_EVENT_HF_END
#error "bt_hf_event_t error define"
#endif

typedef int (*bt_hf_callback_t)(const bt_bdaddr_t *bd_addr, bt_hf_event_t event, bt_hf_callback_param_t param);

bt_status_t bt_hf_init(bt_hf_callback_t callback);
bt_status_t bt_hf_cleanup(void);
bt_status_t bt_hf_connect(const bt_bdaddr_t *bd_addr);
bt_status_t bt_hf_disconnect(const bt_bdaddr_t *bd_addr);
bt_status_t bt_hf_connect_audio(const bt_bdaddr_t *bd_addr);
bt_status_t bt_hf_disconnect_audio(const bt_bdaddr_t *bd_addr);
bt_status_t bt_hf_start_voice_recognition(const bt_bdaddr_t *bd_addr);
bt_status_t bt_hf_stop_voice_recognition(const bt_bdaddr_t *bd_addr);
bt_status_t bt_hf_volume_control(const bt_bdaddr_t *bd_addr, bt_hf_volume_type_t type, int volume);
bt_status_t bt_hf_dial(const bt_bdaddr_t *bd_addr, const char *number); // if number is NULL, redial last number
bt_status_t bt_hf_dial_memory(const bt_bdaddr_t *bd_addr, int location);
bt_status_t bt_hf_handle_call_action(const bt_bdaddr_t *bd_addr, bt_hf_call_action_t action, int idx);
bt_status_t bt_hf_query_current_calls(const bt_bdaddr_t *bd_addr);
bt_status_t bt_hf_query_current_operator_name(const bt_bdaddr_t *bd_addr);
bt_status_t bt_hf_query_cclk_time(const bt_bdaddr_t *bd_addr);
bt_status_t bt_hf_retrieve_subscriber_info(const bt_bdaddr_t *bd_addr);
bt_status_t bt_hf_send_dtmf(const bt_bdaddr_t *bd_addr, char code);
bt_status_t bt_hf_request_last_voice_tag_number(const bt_bdaddr_t *bd_addr);
bt_status_t bt_hf_send_at_cmd(const bt_bdaddr_t *bd_addr, const char *at_cmd_str);
bt_hfp_state_t bt_hf_get_state(int device_id);
/**
 * standard hfp (ag role) interface
 *
 */

#ifdef BT_HFP_AG_ROLE

typedef enum {
    BT_AG_EVENT_OPENED = BT_EVENT_AG_OPENED,
    BT_AG_EVENT_CLOSED,
    BT_AG_EVENT_AUDIO_OPENED,
    BT_AG_EVENT_AUDIO_CLOSED,
    BT_AG_EVENT_VOLUME_CONTROL,
    BT_AG_EVENT_WBS_STATE,
    BT_AG_EVENT_BIA_IND,
    BT_AG_EVENT_BIEV_IND,
    BT_AG_EVENT_DIAL_CALL_REQ,
    BT_AG_EVENT_DIAL_MEMORY_REQ,
    BT_AG_EVENT_CALL_ACTION_REQ,
    BT_AG_EVENT_AT_DTMF_REQ,
    BT_AG_EVENT_AT_BVRA_REQ,
    BT_AG_EVENT_AT_NREC_REQ,
    BT_AG_EVENT_AT_CNUM_REQ,
    BT_AG_EVENT_AT_CIND_REQ,
    BT_AG_EVENT_AT_COPS_REQ,
    BT_AG_EVENT_AT_CLCC_REQ,
    BT_AG_EVENT_AT_CKPD_REQ,
    BT_AG_EVENT_AT_BIND_REQ,
    BT_AG_EVENT_UNKNOWN_AT_REQ,
    BT_AG_EVENT_END,
} bt_ag_event_t;

#if BT_AG_EVENT_END != BT_EVENT_AG_END
#error "bt_ag_event_t error define"
#endif

typedef int (*bt_ag_callback_t)(const bt_bdaddr_t *bd_addr, bt_ag_event_t event, bt_ag_callback_param_t param);

bt_status_t bt_ag_init(bt_ag_callback_t callback);
bt_status_t bt_ag_cleanup(void);
bt_status_t bt_ag_connect(const bt_bdaddr_t *bd_addr);
bt_status_t bt_ag_disconnect(const bt_bdaddr_t *bd_addr);
bt_status_t bt_ag_connect_audio(const bt_bdaddr_t *bd_addr);
bt_status_t bt_ag_disconnect_audio(const bt_bdaddr_t *bd_addr);
bt_status_t bt_ag_start_voice_recoginition(const bt_bdaddr_t *bd_addr);
bt_status_t bt_ag_stop_voice_recoginition(const bt_bdaddr_t *bd_addr);
bt_status_t bt_ag_volume_control(const bt_bdaddr_t *bd_addr, bt_hf_volume_type_t type, int volume);
bt_status_t bt_ag_set_curr_at_upper_handle(const bt_bdaddr_t *bd_addr);
bt_status_t bt_ag_cops_response(const bt_bdaddr_t *bd_addr, const char *operator_name);
bt_status_t bt_ag_clcc_response(const bt_bdaddr_t *bd_addr, const bt_ag_clcc_status_t *status);
bt_status_t bt_ag_cind_response(const bt_bdaddr_t *bd_addr, const bt_ag_cind_status_t *status);
bt_status_t bt_ag_send_response_code(const bt_bdaddr_t *bd_addr, bt_hf_at_response_t response_code, int cme_error);
bt_status_t bt_ag_device_status_change(const bt_bdaddr_t *bd_addr, const bt_ag_device_status_t *status);
bt_status_t bt_ag_phone_status_change(const bt_bdaddr_t *bd_addr, const bt_ag_phone_status_t *status);
bt_status_t bt_ag_send_at_result(const bt_bdaddr_t *bd_addr, const char *at_result);
bt_status_t bt_ag_set_sco_allowed(const bt_bdaddr_t *bd_addr, bool sco_enable);
bt_status_t bt_ag_send_bsir(const bt_bdaddr_t *bd_addr, bool in_band_ring_enable);
int bt_ag_is_noise_reduction_supported(const bt_bdaddr_t *bd_addr);
int bt_ag_is_voice_recognition_supported(const bt_bdaddr_t *bd_addr);

#endif /* BT_HFP_AG_ROLE */

#ifdef __cplusplus
}
#endif
#endif /* __BT_HFP_SERVICE_H__ */

