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

#pragma once
#include "bta_bt_api.h"

#ifdef BT_SVC_FW_PRODUCT_EARBUDS
#include "bta_tws_audio_api.h"
#endif

#define IBRT_PROFILE_STATUS_SUCCESS                     BTA_PROFILE_STATUS_SUCCESS
#define IBRT_PROFILE_STATUS_PENDING                     BTA_PROFILE_STATUS_PENDING
#define IBRT_PROFILE_STATUS_ERROR_INVALID_PARAMETERS    BTA_PROFILE_STATUS_ERROR_INVALID_PARAMETERS
#define IBRT_PROFILE_STATUS_ERROR_NO_CONNECTION         BTA_PROFILE_STATUS_ERROR_NO_CONNECTION
#define IBRT_PROFILE_STATUS_ERROR_CONNECTION_EXISTS     BTA_PROFILE_STATUS_ERROR_CONNECTION_EXISTS
#define IBRT_PROFILE_STATUS_IN_PROGRESS                 BTA_PROFILE_STATUS_IN_PROGRESS
#define IBRT_PROFILE_STATUS_ERROR_DUPLICATE_REQUEST     BTA_PROFILE_STATUS_ERROR_DUPLICATE_REQUEST
#define IBRT_PROFILE_STATUS_ERROR_INVALID_STATE         BTA_PROFILE_STATUS_ERROR_INVALID_STATE
#define IBRT_PROFILE_STATUS_ERROR_TIMEOUT               BTA_PROFILE_STATUS_ERROR_TIMEOUT
#define IBRT_PROFILE_STATUS_ERROR_ROLE_SWITCH_FAILED    BTA_PROFILE_STATUS_ERROR_ROLE_SWITCH_FAILED
#define IBRT_PROFILE_STATUS_ERROR_UNEXPECTED_VALUE      BTA_PROFILE_STATUS_ERROR_UNEXPECTED_VALUE
#define IBRT_PROFILE_STATUS_ERROR_OP_NOT_ALLOWED        BTA_PROFILE_STATUS_ERROR_OP_NOT_ALLOWED
#define AppIbrtStatus bta_profile_status_t

#define IBRT_PROFILE_A2DP_IDLE              BTA_A2DP_IDLE
#define IBRT_PROFILE_A2DP_CODEC_CONFIGURED  BTA_A2DP_CODEC_CONFIGURED
#define IBRT_PROFILE_A2DP_OPEN              BTA_A2DP_OPEN
#define IBRT_PROFILE_A2DP_STREAMING         BTA_A2DP_STREAMING
#define IBRT_PROFILE_A2DP_CLOSED            BTA_A2DP_CLOSED
#define AppIbrtA2dpState bta_a2dp_state_t



#define IBRT_PROFILE_AVRCP_DISCONNECTED     BTA_AVRCP_DISCONNECTED
#define IBRT_PROFILE_AVRCP_CONNECTED        BTA_AVRCP_CONNECTED
#define IBRT_PROFILE_AVRCP_PLAYING          BTA_AVRCP_PLAYING
#define IBRT_PROFILE_AVRCP_PAUSED           BTA_AVRCP_PAUSED
#define IBRT_PROFILE_AVRCP_VOLUME_UPDATED   BTA_AVRCP_VOLUME_UPDATED
#define AppIbrtAvrcpState bta_avrcp_state_t

#define IBRT_PROFILE_HFP_SLC_DISCONNECTED   BTA_HFP_SLC_DISCONNECTED
#define IBRT_PROFILE_HFP_CLOSED             BTA_HFP_CLOSED
#define IBRT_PROFILE_HFP_SCO_CLOSED         BTA_HFP_SCO_CLOSED
#define IBRT_PROFILE_HFP_PENDIN             BTA_HFP_PENDING
#define IBRT_PROFILE_HFP_SLC_OPEN           BTA_HFP_SLC_OPEN
#define IBRT_PROFILE_HFP_NEGOTIATE          BTA_HFP_NEGOTIATE
#define IBRT_PROFILE_HFP_CODEC_CONFIGURED   BTA_HFP_CODEC_CONFIGURED
#define IBRT_PROFILE_HFP_SCO_OPEN           BTA_HFP_SCO_OPEN
#define IBRT_PROFILE_HFP_INCOMING_CALL      BTA_HFP_INCOMING_CALL
#define IBRT_PROFILE_HFP_OUTGOING_CALL      BTA_HFP_OUTGOING_CALL
#define IBRT_PROFILE_HFP_RING_INDICATION    BTA_HFP_RING_INDICATION
#define AppIbrtHfpState bta_hfp_state_t

#define IBRT_PROFILE_NO_CALL            BTA_TWS_NO_CALL
#define IBRT_PROFILE_CALL_ACTIVE        BTA_TWS_CALL_ACTIVE
#define IBRT_PROFILE_HOLD               BTA_TWS_CALL_HOLD
#define IBRT_PROFILE_SETUP_INCOMMING    BTA_TWS_SETUP_INCOMMING
#define IBRT_PROFILE_SETUP_OUTGOING     BTA_TWS_SETUP_OUTGOING
#define IBRT_PROFILE_SETUP_ALERT        BTA_TWS_SETUP_ALERT
#define AppIbrtCallStatus bta_call_status_t

#ifdef __cplusplus
extern "C" {
#endif

void app_ibrt_if_connect_hfp_profile(uint8_t device_id);

void app_ibrt_if_connect_a2dp_profile(uint8_t device_id);

void app_ibrt_if_connect_avrcp_profile(uint8_t device_id);

void app_ibrt_if_disconnect_hfp_profile(uint8_t device_id);

void app_ibrt_if_disconnect_a2dp_profile(uint8_t device_id);

void app_ibrt_if_disconnect_avrcp_profile(uint8_t device_id);

void app_ibrt_if_a2dp_send_play(uint8_t device_id);

void app_ibrt_if_a2dp_send_pause(uint8_t device_id);

void app_ibrt_if_a2dp_send_forward(uint8_t device_id);

void app_ibrt_if_a2dp_send_backward(uint8_t device_id);

void app_ibrt_if_a2dp_send_volume_up(uint8_t device_id);

void app_ibrt_if_a2dp_send_volume_down(uint8_t device_id);

void app_ibrt_if_a2dp_send_set_abs_volume(uint8_t device_id, uint8_t volume);

void app_ibrt_if_hf_create_audio_link(uint8_t device_id);

void app_ibrt_if_hf_disc_audio_link(uint8_t device_id);

void app_ibrt_if_hf_call_redial(uint8_t device_id);

void app_ibrt_if_hf_call_answer(uint8_t device_id);

void app_ibrt_if_hf_call_hangup(uint8_t device_id);

void app_ibrt_if_hf_call_hold(uint8_t device_id);

#define app_ibrt_if_set_local_volume_up bta_tws_set_local_volume_up

#define app_ibrt_if_set_local_volume_down bta_tws_set_local_volume_down

#define app_ibrt_if_get_a2dp_state bta_get_a2dp_state

#define app_ibrt_if_get_avrcp_state bta_get_avrcp_state

#define app_ibrt_if_get_hfp_state bta_hf_get_hfp_state

#define app_ibrt_if_get_hfp_call_status bta_hf_get_hfp_call_status

#define app_ibrt_if_spp_write(channel, buf, length, result_callback) \
    bool status = bta_spp_send_data(channel->rfcomm_handle, buf, length); \
    result_callback(buf, length, status);

void app_ibrt_if_set_a2dp_current_abs_volume(int device_id, uint8_t volume);

void app_ibrt_if_a2dp_set_delay(uint8_t device_id, uint16_t delayMs);

#define app_ibrt_if_a2dp_foreward_streaming_set_delay bta_a2dp_foreward_streaming_set_delay

#define app_ibrt_if_switch_streaming_sco bta_tws_switch_streaming_sco

#define app_ibrt_if_switch_streaming_a2dp bta_tws_switch_streaming_a2dp

#define app_ibrt_if_toggle_a2dp_cis bta_tws_toggle_a2dp_cis

void app_ibrt_if_hf_3way_hungup_incoming(uint8_t device_id);

#define app_ibrt_if_hf_battery_report bta_hf_battery_report

void app_ibrt_if_hf_3way_hungup_active_accept_incomming(uint8_t device_id);

#define app_ibrt_if_hold_background_switch bta_tws_hold_background_switch

#ifdef __cplusplus
}
#endif
