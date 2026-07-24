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

#include "../inc/earbud_profiles_api.h"

void app_ibrt_if_connect_hfp_profile(uint8_t device_id)
{
    bt_bdaddr_t addr = {0};
    if (!bta_get_addr_by_device_id(device_id, &addr))
    {
        return;
    }
    bta_hf_connect(&addr);
}

void app_ibrt_if_connect_a2dp_profile(uint8_t device_id)
{
    bt_bdaddr_t addr = {0};
    if (!bta_get_addr_by_device_id(device_id, &addr))
    {
        return;
    }
    bta_a2dp_connect(&addr);
}

void app_ibrt_if_connect_avrcp_profile(uint8_t device_id)
{
    bt_bdaddr_t addr = {0};
    if (!bta_get_addr_by_device_id(device_id, &addr))
    {
        return;
    }
    bta_avrcp_connect(&addr);
}

void app_ibrt_if_disconnect_hfp_profile(uint8_t device_id)
{
    bt_bdaddr_t addr = {0};
    if (!bta_get_addr_by_device_id(device_id, &addr))
    {
        return;
    }
    bta_hf_disconnect(&addr);
}

void app_ibrt_if_disconnect_a2dp_profile(uint8_t device_id)
{
    bt_bdaddr_t addr = {0};
    if (!bta_get_addr_by_device_id(device_id, &addr))
    {
        return;
    }
    bta_a2dp_disconnect(&addr);
}

void app_ibrt_if_disconnect_avrcp_profile(uint8_t device_id)
{
    bt_bdaddr_t addr = {0};
    if (!bta_get_addr_by_device_id(device_id, &addr))
    {
        return;
    }
    bta_avrcp_disconnect(&addr);
}

void app_ibrt_if_a2dp_send_play(uint8_t device_id)
{
    bt_bdaddr_t addr = {0};
    if (!bta_get_addr_by_device_id(device_id, &addr))
    {
        return;
    }
    bta_avrcp_send_play(&addr);
}

void app_ibrt_if_a2dp_send_pause(uint8_t device_id)
{
    bt_bdaddr_t addr = {0};
    if (!bta_get_addr_by_device_id(device_id, &addr))
    {
        return;
    }
    bta_avrcp_send_pause(&addr);
}

void app_ibrt_if_a2dp_send_forward(uint8_t device_id)
{
    bt_bdaddr_t addr = {0};
    if (!bta_get_addr_by_device_id(device_id, &addr))
    {
        return;
    }
    bta_avrcp_send_forward(&addr);
}

void app_ibrt_if_a2dp_send_backward(uint8_t device_id)
{
    bt_bdaddr_t addr = {0};
    if (!bta_get_addr_by_device_id(device_id, &addr))
    {
        return;
    }
    bta_avrcp_send_backward(&addr);
}

void app_ibrt_if_a2dp_send_volume_up(uint8_t device_id)
{
    bt_bdaddr_t addr = {0};
    if (!bta_get_addr_by_device_id(device_id, &addr))
    {
        return;
    }
    bta_avrcp_send_volume_up(&addr);
}

void app_ibrt_if_a2dp_send_volume_down(uint8_t device_id)
{
    bt_bdaddr_t addr = {0};
    if (!bta_get_addr_by_device_id(device_id, &addr))
    {
        return;
    }
    bta_avrcp_send_volume_down(&addr);
}

void app_ibrt_if_a2dp_send_set_abs_volume(uint8_t device_id, uint8_t volume)
{
    bt_bdaddr_t addr = {0};
    if (!bta_get_addr_by_device_id(device_id, &addr))
    {
        return;
    }
    bta_avrcp_send_set_abs_volume(&addr, volume);
}

void app_ibrt_if_hf_create_audio_link(uint8_t device_id)
{
    bt_bdaddr_t addr = {0};
    if (!bta_get_addr_by_device_id(device_id, &addr))
    {
        return;
    }
    bta_hf_connect_audio_link(&addr);
}

void app_ibrt_if_hf_disc_audio_link(uint8_t device_id)
{
    bt_bdaddr_t addr = {0};
    if (!bta_get_addr_by_device_id(device_id, &addr))
    {
        return;
    }
    bta_hf_disconnect_audio_link(&addr);
}

void app_ibrt_if_hf_call_redial(uint8_t device_id)
{
    bt_bdaddr_t addr = {0};
    if (!bta_get_addr_by_device_id(device_id, &addr))
    {
        return;
    }
    bta_hf_call_redial(&addr);
}

void app_ibrt_if_hf_call_answer(uint8_t device_id)
{
    bt_bdaddr_t addr = {0};
    if (!bta_get_addr_by_device_id(device_id, &addr))
    {
        return;
    }
    bta_hf_call_answer(&addr);
}

void app_ibrt_if_hf_call_hangup(uint8_t device_id)
{
    bt_bdaddr_t addr = {0};
    if (!bta_get_addr_by_device_id(device_id, &addr))
    {
        return;
    }
    bta_hf_call_hangup(&addr);
}

void app_ibrt_if_hf_call_hold(uint8_t device_id)
{
    bt_bdaddr_t addr = {0};
    if (!bta_get_addr_by_device_id(device_id, &addr))
    {
        return;
    }
    bta_hf_call_hold(&addr);
}

void app_ibrt_if_set_a2dp_current_abs_volume(int device_id, uint8_t volume)
{
    bt_bdaddr_t addr = {0};
    if (!bta_get_addr_by_device_id(device_id, &addr))
    {
        return;
    }
    // bta_a2dp_set_current_abs_volume(&addr, volume);
    bta_avrcp_send_set_abs_volume(&addr, volume);
}

void app_ibrt_if_a2dp_set_delay(uint8_t device_id, uint16_t delayMs)
{
    bt_bdaddr_t addr = {0};
    if (!bta_get_addr_by_device_id(device_id, &addr))
    {
        return;
    }
    bta_a2dp_set_delay(&addr, delayMs);
}



void app_ibrt_if_hf_3way_hungup_incoming(uint8_t device_id)
{
    bt_bdaddr_t addr = {0};
    if (!bta_get_addr_by_device_id(device_id, &addr))
    {
        return;
    }
    bta_hf_3way_hungup_incoming(&addr);
}

void app_ibrt_if_hf_3way_hungup_active_accept_incomming(uint8_t device_id)
{
    bt_bdaddr_t addr = {0};
    if (!bta_get_addr_by_device_id(device_id, &addr))
    {
        return;
    }
    bta_hf_3way_hungup_active_accept_incomming(&addr);
}
