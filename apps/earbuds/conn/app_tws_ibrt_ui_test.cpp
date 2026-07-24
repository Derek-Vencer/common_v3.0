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
#if defined(IBRT_UI)
#include <stdlib.h>
#include <string.h>
#include "hal_trace.h"
#include "apps.h"
#include "app_key.h"
#include "app_anc.h"
#include "bluetooth_bt_api.h"
#include "btapp.h"
#include "factory_section.h"
#include "nvrecord_bt.h"
#include "nvrecord_env.h"
#include "nvrecord_ble.h"
#include "nvrecord_appmode.h"
#include "ddbif.h"
#include "app_ibrt_keyboard.h"
#include "app_tws_ibrt_ui_test.h"
#include "app_tws_ibrt.h"
#include "app_ibrt_keyboard.h"
#include "app_bt.h"
#include "btapp.h"
#include "app_ibrt_nvrecord.h"
#include "app_bt_func.h"
#include "app_ibrt_customif_cmd.h"
#include "app_audio_active_device_manager.h"
#include "app_audio_control.h"
#include "app_audio_bt_device.h"
#include "hal_bootmode.h"
#include "app_bt_media_manager.h"
#include "app_ibrt_debug.h"
#include "app_a2dp.h"
#include "app_trace_rx.h"
#include "bts_am_api.h"
// Need remove this include
#include "audio_policy.h"
#include "bta_tws_ux_api.h"
#include "bta_ble_api.h"
#include "bta_bt_api.h"
#include "bta_tws_audio_api.h"
#include "audio_player_adapter.h"
#if defined(BISTO_ENABLED)
#include "gsound_custom_actions.h"
#endif

#include "apps.h"
#include "bts_bt_if.h"
#include "app_media_player.h"
#include "a2dp_decoder.h"

#ifdef RECORDING_USE_SCALABLE
#include "voice_compression.h"
#endif

#ifdef __AI_VOICE__
#include "app_ai_if.h"
#endif

#if defined(A2DP_SBC_PLC_ENABLED)
#include "hal_location.h"
#endif

#if BLE_AUDIO_ENABLED
#include "bes_aob_api.h"
#include "bluetooth_ble_api.h"
#include "app_audio_bt_device.h"
#include "app_audio_focus_stack.h"
#include "app_audio_active_device_manager.h"
#include "app_audio_control.h"
#include "ble_audio_core_api.h"
#if BT_SERVICE_ENABLE
#include "bt_svc_lea_gap.h"
#endif
#endif

#include "app_audio_control.h"

#ifdef AUDIO_MANAGER_TEST_ENABLE
#include "app_audio_active_device_manager_ut.h"
#include "app_audio_focus_control_test.h"
#include "app_audio_focus_stack_ut.h"
#endif
#include "app_ai_manager_api.h"
#include "bts_module_if.h"
#ifdef BESUI_APP_EN
#include "app_tota_general.h"
#endif
#ifdef BESUI_KEY_EN
#include "twsui_key.h"
#endif
#ifdef BESUI_BTMSG_EN
#include "twsui_btmsg.h"
#endif
#ifdef BESUI_TWS_EN
#include "twsui_comm.h"
#endif
#ifdef BESUI_1WIRE_EN
#include "twsui_uart.h"
#endif
#if defined(BESUI_COMM_EN)
#include "besui_common.h"
#endif

extern "C"
{
    void nv_record_ble_delete_all_entry(void);
}

typedef struct
{
    const char* string;
    void (*cmd_function)(const char* param, uint32_t param_len);
} app_ibrt_test_cmd_table_t;

typedef struct
{
    bt_bdaddr_t master_bdaddr;
    bt_bdaddr_t slave_bdaddr;
} ibrt_pairing_info_t;

#if defined( __BT_ANC_KEY__)&&defined(ANC_APP)
extern void app_anc_key(APP_KEY_STATUS *status, void *param);
#endif

#if defined(A2DP_SBC_PLC_ENABLED)
static uint32_t ch_select;
SRAM_TEXT_LOC int get_ibrt_ch(void){
    return ch_select;
}
#endif

void print_bd_addr(const uint8_t *bd_addr, const char *string)
{
    if (bd_addr == NULL)
    {
        TRACE(1, "Error: BD_ADDR pointer is NULL!\n");
        return;
    }

    const char *addr_label = "unknown";
    if (string != NULL && strlen(string) > 0)
    {
        addr_label = string;
    }

    TRACE(1, "%s power_on_bt_addr: %02X:%02X:%02X:%02X:%02X:%02X\n",
           addr_label,
           bd_addr[0], bd_addr[1], bd_addr[2],
           bd_addr[3], bd_addr[4], bd_addr[5]);
}

static int bd_addr_compare(const uint8_t *addr1, const uint8_t *addr2)
{
    if (addr1 == NULL || addr2 == NULL)
    {
        return 0;
    }

    print_bd_addr(addr1,"local_addr_compare");
    print_bd_addr(addr2,"peer_addr_compare");

    for (int i = 0; i < BD_ADDR_LEN; i++)
    {
        if (addr1[i] > addr2[i])
        {
            return 1;
        }
        else if (addr1[i] < addr2[i])
        {
            return -1;
        }
        else if (addr1[i] == addr2[i])
        {
            continue;
        }
    }

    return 0;
}

WEAK void app_ibrt_initialize_nv_role_callback(void *config, void *record_env)
{
    if (config == NULL || record_env == NULL) {
        TRACE(1, "Error: config/record_env pointer is NULL!\n");
        return;
    }

    TRACE(1,"%s",__func__);
    ibrt_config_t *ibrt_config = (ibrt_config_t *)config;
    nvrecord_env_t *nvrecord_env = (struct nvrecord_env_t *)record_env;

    uint8_t local_bt_addr[BD_ADDR_LEN] = {0};
    if (memcmp(ibrt_config->local_addr.address, local_bt_addr, BD_ADDR_LEN) == 0)
    {
        factory_section_original_btaddr_get(local_bt_addr);
        memcpy(ibrt_config->local_addr.address, local_bt_addr, BD_ADDR_LEN);
    }
    else
    {
        memcpy(local_bt_addr, ibrt_config->local_addr.address, BD_ADDR_LEN);
    }

    uint8_t peer_bt_addr[BD_ADDR_LEN] = {0};
    memcpy(peer_bt_addr, nvrecord_env->ibrt_mode.record.bdAddr.address, BD_ADDR_LEN);

    int cmp_result = bd_addr_compare(local_bt_addr, peer_bt_addr);
    if (cmp_result == 0)
    {
        ibrt_config->nv_role = BT_IBRT_MASTER;
        TRACE(2, "Local addr equals peer addr, set as NV_MASTER\n");
    }
    else
    {
        ibrt_config->nv_role = BT_IBRT_SLAVE;
        TRACE(2, "Local addr not equal to peer addr, set as NV_SLAVE\n");
    }

    nvrecord_env->ibrt_mode.mode = ibrt_config->nv_role;
}

static void generate_default_peer_addr(const uint8_t *local_addr, uint8_t *peer_addr)
{
    if (local_addr == NULL || peer_addr == NULL)
    {
        TRACE(1, "Error: local/peer addr pointer is NULL!\n");
        memset(peer_addr, 0, BD_ADDR_LEN);
        return;
    }

    struct nvrecord_env_t *nvrecord_env;

    nv_record_env_get(&nvrecord_env);

    if (nvrecord_env)
    {
        memcpy(peer_addr, nvrecord_env->ibrt_mode.record.bdAddr.address, BD_ADDR_LEN);
    }
}

int app_ibrt_ui_v2_test_config_load(void *config)
{
    ibrt_config_t *ibrt_config = (ibrt_config_t *)config;
    struct nvrecord_env_t *nvrecord_env;

    nv_record_env_get(&nvrecord_env);
    factory_section_original_btaddr_get(ibrt_config->local_addr.address);
    print_bd_addr(ibrt_config->local_addr.address,"local");

    app_ibrt_initialize_nv_role_callback(ibrt_config, nvrecord_env);
    TRACE(1,"mode:%d",nvrecord_env->ibrt_mode.mode);

#ifdef IBRT_RIGHT_MASTER
    if (BT_IBRT_MASTER == nvrecord_env->ibrt_mode.mode)
    {
#ifdef BESUI_TWS_EN
        if(besui_get_lr_sta() == LEFT_SIDE)
        {
            ibrt_config->audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_LCHNL;
            bts_tws_if_set_local_side(BT_LOCATION_LEFT);
        }
        else if(besui_get_lr_sta() == RIGHT_SIDE)
        {
            ibrt_config->audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_RCHNL;
            bts_tws_if_set_local_side(BT_LOCATION_RIGHT);
        }
#else
        ibrt_config->audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_RCHNL;
        bts_tws_if_set_local_side(BT_LOCATION_RIGHT);
#endif
        generate_default_peer_addr(ibrt_config->local_addr.address,ibrt_config->peer_addr.address);
    }
    else if (BT_IBRT_SLAVE == nvrecord_env->ibrt_mode.mode)
    {
#ifdef BESUI_TWS_EN
        if(besui_get_lr_sta() == LEFT_SIDE)
        {
            ibrt_config->audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_LCHNL;
            bts_tws_if_set_local_side(BT_LOCATION_LEFT);
        }
        else if(besui_get_lr_sta() == RIGHT_SIDE)
        {
            ibrt_config->audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_RCHNL;
            bts_tws_if_set_local_side(BT_LOCATION_RIGHT);
        }
#else
        ibrt_config->audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_LCHNL;
        bts_tws_if_set_local_side(BT_LOCATION_LEFT);
#endif
        generate_default_peer_addr(ibrt_config->local_addr.address,ibrt_config->peer_addr.address);
    }
    else
    {
        ibrt_config->nv_role = BT_IBRT_UNKNOWN;
        ibrt_config->audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_STEREO;
    }
#else
    if (BT_IBRT_SLAVE == nvrecord_env->ibrt_mode.mode)
    {
#ifdef BESUI_TWS_EN
        if(besui_get_lr_sta() == LEFT_SIDE)
        {
            ibrt_config->audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_LCHNL;
            bts_tws_if_set_local_side(BT_LOCATION_LEFT);
        }
        else if(besui_get_lr_sta() == RIGHT_SIDE)
        {
            ibrt_config->audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_RCHNL;
            bts_tws_if_set_local_side(BT_LOCATION_RIGHT);
        }
#else
        ibrt_config->audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_RCHNL;
        bts_tws_if_set_local_side(BT_LOCATION_RIGHT);
#endif
        generate_default_peer_addr(ibrt_config->local_addr.address,ibrt_config->peer_addr.address);
    }
    else if (BT_IBRT_MASTER == nvrecord_env->ibrt_mode.mode)
    {
#ifdef BESUI_TWS_EN
        if(besui_get_lr_sta() == LEFT_SIDE)
        {
            ibrt_config->audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_LCHNL;
            bts_tws_if_set_local_side(BT_LOCATION_LEFT);
        }
        else if(besui_get_lr_sta() == RIGHT_SIDE)
        {
            ibrt_config->audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_RCHNL;
            bts_tws_if_set_local_side(BT_LOCATION_RIGHT);
        }
#else
        ibrt_config->audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_LCHNL;
        bts_tws_if_set_local_side(BT_LOCATION_LEFT);
#endif
        generate_default_peer_addr(ibrt_config->local_addr.address,ibrt_config->peer_addr.address);
    }
    else
    {
        ibrt_config->nv_role = BT_IBRT_UNKNOWN;
        ibrt_config->audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_STEREO;
    }
#endif
    print_bd_addr(ibrt_config->peer_addr.address,"peer_addr");
    print_bd_addr(nvrecord_env->ibrt_mode.record.bdAddr.address,"nv_peer_addr");
#if defined(A2DP_SBC_PLC_ENABLED)
    ch_select = ibrt_config->audio_chnl_sel;
#endif
    bts_core_set_ui_role(nvrecord_env->ibrt_mode.mode);

    return 0;
}

#ifdef APP_TRACE_RX_ENABLE
extern "C" void pmu_reboot(void);
static void app_ibrt_soft_reset_test(const char* param, uint32 len)
{
    EARBUDS_TRACE(1,"soft_reset_test");
    //app_reset();
    pmu_reboot();
}

static void app_ibrt_trigger_assert_test(const char* param, uint32 len)
{
    ASSERT(false, "Test purpose assert!");
}

static void app_ibrt_soft_reset_delete_bt_nv_test(const char* param, uint32 len)
{
    EARBUDS_TRACE(1,"soft_reset_after_delete_nv_test");
    nvrec_btdevicerecord *l_NvRecord;
    uint8_t record_num = 0;
    record_num = nv_record_get_paired_dev_list(&l_NvRecord);
    for (uint8_t i = 0; i < record_num; i++) {
        ddbif_delete_record(&l_NvRecord[i].record.bdAddr);
    }
    nv_record_flash_flush();
    osDelay(500);
    app_reset();
}

#ifdef __IAG_BLE_INCLUDE__
static void app_ibrt_nv_record_ble_delete_all_entry_test(const char* param, uint32 len)
{
    nv_record_ble_delete_all_entry();
}
#endif

void app_ibrt_ui_tws_swtich_test(void)
{
    EARBUDS_TRACE(1, "%s",__func__);
}

static void app_ibrt_mgr_pairing_mode_test(const char* param, uint32_t len)
{
    struct nvrecord_env_t *nvrecord_env;

    bta_tws_box_event_entry(BTA_TWS_OPEN);
    nv_record_env_get(&nvrecord_env);

    if (nvrecord_env)
    {
        bta_tws_reconfig_nv_role(bts_tws_if_get_nv_role(), (bt_bdaddr_t *)&nvrecord_env->ibrt_mode.record.bdAddr);
    }

    bta_tws_enable_pairing_mode(true);
}

void app_ibrt_start_power_on_freeman_pairing(const char* param, uint32_t len)
{
    bta_tws_enable_freeman_mode(true);
}

static void app_ibrt_tws_disconnect_test(const char* param, uint32_t len)
{
    EARBUDS_TRACE(0, "tws_disconnect_test");
    bts_tws_if_disconnect_acl_link();
}

static int parse_bt_addr(const char *addr_buf, bt_bdaddr_t *bt_addr, unsigned int lenth)
{
    if(lenth < 12)
    {
        EARBUDS_TRACE(0, "invalid recv bt addr");
        return 0;
    }
    char bt_addr_buff[2];
    uint8_t btLen = 0;
    uint8_t bufferStart = 10;
    while(btLen < 6)
    {
        memset(&bt_addr_buff, 0, sizeof(bt_addr_buff));
        memcpy(bt_addr_buff, addr_buf + bufferStart, 2);
        bt_addr->address[btLen] = (unsigned char)strtol((char*)bt_addr_buff, NULL, 16);
        bufferStart -= 2;
        btLen += 1;
    }
    return 1;
}

void app_ibrt_print_log_with_bt_address(const char *tag, uint8_t *bt_addr)
{
    EARBUDS_TRACE(7, "%s addr: %02x:%02x:%02x:%02x:%02x:%02x", tag ? tag : "",
        bt_addr[0], bt_addr[1], bt_addr[2], bt_addr[3], bt_addr[4], bt_addr[5]);
}

static void app_ibrt_mobile_connect_test(const char* param, uint32_t len)
{
    EARBUDS_TRACE(0, "mobile_connect_test_1");
    bt_bdaddr_t remote_addr = {0,};
    if (parse_bt_addr(param, &remote_addr, len) == 1) {
        bta_tws_connect_bt_device(&remote_addr, 0);
    }
}

static void app_ibrt_mobile_disconnect_test(const char* param, uint32_t len)
{
    EARBUDS_TRACE(0, "mobile_disconnect_test_1");
    bt_bdaddr_t remote_addr = {0,};
    if (parse_bt_addr(param, &remote_addr, len) == 1) {
        bts_bt_if_dev_disconnect_acl_link(&remote_addr);
    }
}

static void app_ibrt_connect_profiles_test(const char* param, uint32_t len)
{
    EARBUDS_TRACE(0, "connect_profiles_test");
    bts_bt_if_dev_connect_all_profiles(NULL);
}

static void app_ibrt_start_ibrt_test_with_addr(const char* param, uint32_t len)
{
    bt_bdaddr_t remote_addr = {0,};

    if (parse_bt_addr(param, &remote_addr, len) == 1)
    {
        EARBUDS_TRACE(0,"start_ibrt_test_with_addr:%02x:%02x:*:*:*:%02x",
            remote_addr.address[0], remote_addr.address[1], remote_addr.address[5]);
        bts_ibrt_if_dev_connect_ibrt_link(&remote_addr);
    }
}

static void app_ibrt_stop_ibrt_test_with_addr(const char* param, uint32_t len)
{
    bt_bdaddr_t remote_addr = {0,};

    if (parse_bt_addr(param, &remote_addr, len) == 1)
    {
        EARBUDS_TRACE(0,"stop_ibrt_test_with_addr:%02x:%02x:*:*:*:%02x",
            remote_addr.address[0], remote_addr.address[1], remote_addr.address[5]);
        bts_ibrt_if_dev_disconnect_ibrt_link(&remote_addr);
    }
}

static void app_ibrt_tws_role_switch_test(const char* param, uint32_t len)
{
    if (bta_tws_get_ui_role() == BT_IBRT_MASTER)
    {
        bta_tws_request_ui_role_switch(BT_IBRT_SLAVE);
    }
    else
    {
        bta_tws_request_ui_role_switch(BT_IBRT_MASTER);
    }
}

static void app_ibrt_tws_role_switch_test_with_addr(const char* param, uint32_t len)
{
    POSSIBLY_UNUSED ibrt_status_t status;
    bt_bdaddr_t remote_addr = {0,};

    if (parse_bt_addr(param, &remote_addr, len) == 1)
    {
        if (bts_bt_if_is_profile_connecting(&remote_addr))
        {
            status = IBRT_STATUS_ERROR_OP_NOT_ALLOWED;
        }
        else
        {
            status = bts_ibrt_conn_tws_role_switch(&remote_addr);
        }
        EARBUDS_TRACE(2, "%s status=%d",__func__,status);
    }
}

void app_ibrt_a2dp_a2dp_set_codec_test(const char* param, uint32_t len)
{
    codec_act_param_t codec_act={0};
    codec_act.action = CODEC_ACT_EN_OR_DISABLE_CODEC;
    codec_act.device_id = 0xff;
    uint32_t param_codec_type[1]={0};
    uint32_t param_sub_codec_type[1]={0};
    uint32_t param_codec_enable;
    bool codec_enable=false;
    uint8_t codec_type[1]={0};
    uint8_t sub_codec_type[1]={0};
    sscanf(param, "%02x%02x%d", &param_codec_type[0], &param_sub_codec_type[0], &param_codec_enable);
    codec_enable = (uint8_t*)param_codec_enable;
    for(int i=0;i<1;i++){
        codec_type[i]=param_codec_type[i];
    }
    for(int i=0;i<1;i++){
        sub_codec_type[i]=param_sub_codec_type[i];
    }
    codec_act.p.enable_codec.codec_type = codec_type[0];
    codec_act.p.enable_codec.sub_codec_type = sub_codec_type[0];
    codec_act.p.enable_codec.enable = codec_enable;
    btif_a2dp_set_codec_parameters((codec_act_param_t *)&codec_act);
}

static void app_ibrt_get_remote_device_count(const char* param, uint32_t len)
{
    EARBUDS_TRACE(1,"remote device count = %d",bts_bt_if_get_dev_acl_connected_count());
}

static void app_ibrt_tws_dump_info_test(const char* param, uint32_t len)
{
    EARBUDS_TRACE(1, "%s",__func__);
    app_ibrt_conn_dump_ibrt_info();
}

#ifdef GFPS_ENABLED
extern "C" void gfps_enter_fastpairing_mode(void);
#endif

static void app_ibrt_enable_access_mode_test(const char* param, uint32_t len)
{
#ifdef GFPS_ENABLED
    gfps_enter_fastpairing_mode();
#endif

    EARBUDS_TRACE(1, "%s",__func__);
    app_ibrt_conn_set_discoverable_connectable(true,true);
}

static void app_ibrt_disable_access_mode_test(const char* param, uint32_t len)
{
    EARBUDS_TRACE(1, "%s",__func__);
    app_ibrt_conn_set_discoverable_connectable(false,false);
}

static void app_ibrt_tws_freeman_pairing_test(const char* param, uint32_t len)
{
    bta_tws_enable_freeman_mode(true);
}

static void app_ibrt_tws_disc_all_mobile_link_test(const char* param, uint32_t len)
{
    EARBUDS_TRACE(1, "%s",__func__);
    bts_bt_if_dev_disconnect_all_links();
}

static void app_ibrt_tws_w4_mobile_connect_test(const char* param, uint32_t len)
{
    EARBUDS_TRACE(1, "%s",__func__);

    if(bts_tws_if_is_nv_master())
    {
         app_bt_set_access_mode(BTIF_BAM_GENERAL_ACCESSIBLE);
    }
    else
    {
         app_bt_set_access_mode(BTIF_BAM_NOT_ACCESSIBLE);
    }
}

static void app_ibrt_tws_mobile_connection_test(const char* param, uint32_t len)
{
    EARBUDS_TRACE(1, "%s",__func__);
    EARBUDS_TRACE(1, "mobile connection state = %d",bts_bt_if_get_dev_acl_state(NULL));
}


static void app_ibrt_get_a2dp_state_test(const char* param, uint32_t len)
{
   POSSIBLY_UNUSED const static char* a2dp_state_strings[] = {
    "IDLE",
    "CODEC_CONFIGURED",
    "OPEN",
    "STREAMING",
    "CLOSED",
    "ABORTING"
   };
    bt_bdaddr_t remote_addr = {0,};
    if(parse_bt_addr(param, &remote_addr, len) == 1){
        bta_a2dp_state_t a2dp_state;
        bta_profile_status_t  status = bta_get_a2dp_state(&remote_addr, &a2dp_state);
        if(BTA_PROFILE_STATUS_SUCCESS == status){
            EARBUDS_TRACE(0,"ibrt_test:a2dp_state=%s",a2dp_state_strings[a2dp_state]);
        }
        else{
            EARBUDS_TRACE(0,"ibrt_test:get a2dp state error");
        }
    }
}

static void app_ibrt_get_avrcp_state_test(const char* param, uint32_t len)
{
    POSSIBLY_UNUSED const static char* avrcp_state_strings[] = {
        "DISCONNECTED",
        "CONNECTED",
        "PLAYING",
        "PAUSED",
        "VOLUME_UPDATED"
    };
    bt_bdaddr_t remote_addr = {0,};
    if(parse_bt_addr(param, &remote_addr, len) == 1){
        bta_avrcp_state_t avrcp_state;
        bta_profile_status_t status = bta_get_avrcp_state(&remote_addr, &avrcp_state);
        if(BTA_PROFILE_STATUS_SUCCESS == status)
        {
            EARBUDS_TRACE(0,"ibrt_test:avrcp_state=%s",avrcp_state_strings[avrcp_state]);
        }
        else
        {
            EARBUDS_TRACE(0,"ibrt_test:get avrcp state error");
        }
    }
}

#ifdef BT_HFP_SUPPORT
static void app_ibrt_get_hfp_state_test(const char* param, uint32_t len)
{
    POSSIBLY_UNUSED const static char* hfp_state_strings[] = {
        "SLC_DISCONNECTED",
        "CLOSED",
        "SCO_CLOSED",
        "PENDING",
        "SLC_OPEN",
        "NEGOTIATE",
        "CODEC_CONFIGURED",
        "SCO_OPEN",
        "INCOMING_CALL",
        "OUTGOING_CALL",
        "RING_INDICATION"
    };
    bt_bdaddr_t remote_addr = {0,};
    if(parse_bt_addr(param, &remote_addr, len) == 1){
        bta_hfp_state_t hfp_state;
        bta_profile_status_t status = bta_hf_get_hfp_state(&remote_addr, &hfp_state);
        if(BTA_PROFILE_STATUS_SUCCESS == status)
        {
            EARBUDS_TRACE(0,"ibrt_test:hfp_state=%s",hfp_state_strings[hfp_state]);
        }
        else
        {
            EARBUDS_TRACE(0,"ibrt_test:get hfp state error");
        }
    }
}

void app_ibrt_get_call_status_test(const char* param, uint32_t len)
{
    POSSIBLY_UNUSED const static char* call_status_strings[] = {
        "NO_CALL",
        "CALL_ACTIVE",
        "HOLD",
        "INCOMMING",
        "OUTGOING",
        "ALERT"
    };
    bt_bdaddr_t remote_addr = {0,};
    if(parse_bt_addr(param, &remote_addr, len) == 1)
    {
        bta_call_status_t call_status;
        bta_profile_status_t status = bta_hf_get_hfp_call_status(&remote_addr, &call_status);
        if(BTA_PROFILE_STATUS_SUCCESS == status)
        {
            EARBUDS_TRACE(0,"ibrt_test:call_status=%s",call_status_strings[call_status]);
        }
        else
        {
            EARBUDS_TRACE(0,"ibrt_test:get call status error");
        }
    }
}
#endif

void app_ibrt_tws_role_get_request_test_with_parameter(const char* param, uint32_t len)
{
    bt_bdaddr_t remote_addr = {0,};
    if(parse_bt_addr(param, &remote_addr, len) == 1){
        POSSIBLY_UNUSED bool is_ibrt_master = bts_bt_if_is_dev_link_connected(&remote_addr);
        EARBUDS_TRACE(1, "is_ibrt_master_connected=%d",is_ibrt_master);
    }
}

static void app_ibrt_get_tws_state_test(const char* param, uint32_t len)
{
    if(bts_tws_if_is_tws_link_connected())
    {
        EARBUDS_TRACE(0,"ibrt_test:TWS CONNECTED");
    }
    else
    {
        EARBUDS_TRACE(0,"ibrt_test:TWS DISCONNECTED");
    }
}
void app_ibrt_tws_avrcp_pause_test(const char* param, uint32_t len)
{
    app_audio_control_media_pause();
}

void app_ibrt_tws_avrcp_play_test(const char* param, uint32_t len)
{
    app_audio_control_media_play();
}

void app_ibrt_tws_avrcp_vol_up_test(const char* param, uint32_t len)
{
    app_audio_control_streaming_volume_up();
}

void app_ibrt_tws_avrcp_vol_down_test(const char* param, uint32_t len)
{
    app_audio_control_streaming_volume_down();
}


static void app_ibrt_tws_avrcp_fast_forward_start_test(const char* param, uint32_t len)
{
    app_audio_control_media_fast_forward_start();
}

static void app_ibrt_tws_avrcp_fast_forward_stop_test(const char* param, uint32_t len)
{
    app_audio_control_media_fast_forward_stop();
}

static void app_ibrt_tws_avrcp_rewind_start_test(const char* param, uint32_t len)
{
    app_audio_control_media_rewind_start();
}

static void app_ibrt_tws_avrcp_rewind_stop_test(const char* param, uint32_t len)
{
    app_audio_control_media_rewind_stop();
}

static void app_ibrt_tws_avrcp_next_track_test(const char* param, uint32_t len)
{
    uint8_t device_id = bts_am_get_curr_a2dp_device();
    if(device_id != BT_DEVICE_INVALID_ID){
        app_ibrt_if_start_user_action_v2(device_id, IBRT_ACTION_FORWARD, 0, 0);
    }
}

static void app_ibrt_tws_avrcp_prev_track_test(const char* param, uint32_t len)
{
    uint8_t device_id = bts_am_get_curr_a2dp_device();
    if(device_id != BT_DEVICE_INVALID_ID){
        app_ibrt_if_start_user_action_v2(device_id, IBRT_ACTION_BACKWARD, 0, 0);
    }
}

static void app_ibrt_ui_print_active_address(const bt_bdaddr_t *remote)
{
    EARBUDS_TRACE(6, "a2dp active phone addr: %02x:%02x:%02x:%02x:%02x:%02x", remote->address[0], remote->address[1],
        remote->address[2], remote->address[3], remote->address[4], remote->address[5]);
}

static void app_ibrt_ui_get_a2dp_active_phone(const char* param, uint32_t len)
{
    uint8_t device_id = bts_am_get_curr_a2dp_device();
    struct BT_DEVICE_T *curr_device = app_bt_get_device(device_id);
    PLAYBACK_INFO_T* playback_info = app_bt_get_music_playback_info();
    BT_AUDIO_DEVICE_T* active_device = app_audio_adm_get_active_device();

    if(!curr_device->a2dp_conn_flag)
    {
        EARBUDS_TRACE(0,"a2dp active phone = None");
        return;
    }
    if (curr_device->a2dp_play_pause_flag == 0){
        uint8_t active_device_id = playback_info->device_id;
        if (active_device_id == active_device->device_id && active_device_id != BT_DEVICE_INVALID_ID) {
             struct BT_DEVICE_T  *active_device_a2dp = app_bt_get_device(active_device_id);
             if(bts_tws_if_is_tws_link_connected()){
                if (active_device_a2dp->a2dp_conn_flag && bts_bt_if_is_dev_link_connected(&active_device_a2dp->remote)){
                    app_ibrt_ui_print_active_address(&active_device_a2dp->remote);
                    return;
                 }
                else{
                    EARBUDS_TRACE(0,"a2dp active phone = None");
                    return;
                }
             }else{
                    app_ibrt_ui_print_active_address(&active_device_a2dp->remote);
                    return;
             }
        }
    }
    app_ibrt_ui_print_active_address(&curr_device->remote);
}


static void app_ibrt_tws_hfp_answer_test(const char* param, uint32_t len)
{
    #if BLE_AUDIO_ENABLED
        app_audio_control_call_answer();
    #else
        uint8_t device_id = bts_am_get_device_for_user_action();
        if(device_id != BT_DEVICE_INVALID_ID){
            app_ibrt_if_start_user_action_v2(device_id, IBRT_ACTION_ANSWER, 0, 0);
        }
    #endif
}

static void app_ibrt_tws_hfp_reject_test(const char* param, uint32_t len)
{
    #if BLE_AUDIO_ENABLED
        app_audio_control_call_terminate();
    #else
        uint8_t device_id = bts_am_get_device_for_user_action();
        if(device_id != BT_DEVICE_INVALID_ID){
            app_ibrt_if_start_user_action_v2(device_id, IBRT_ACTION_HANGUP, 0, 0);
        }
    #endif
}

static void app_ibrt_tws_hfp_hangup_test(const char* param, uint32_t len)
{
    #if BLE_AUDIO_ENABLED
        app_audio_control_call_terminate();
    #else
        uint8_t device_id = bts_am_get_device_for_user_action();
        if(device_id != BT_DEVICE_INVALID_ID){
            app_ibrt_if_start_user_action_v2(device_id, IBRT_ACTION_HANGUP, 0, 0);
        }
    #endif
}

static void app_ibrt_call_redial_test(const char* param, uint32_t len)
{
    #if BLE_AUDIO_ENABLED
        app_audio_control_redial_last_call();
    #else
        uint8_t device_id = bts_am_get_device_for_user_action();
        if(device_id != BT_DEVICE_INVALID_ID){
            app_ibrt_if_start_user_action_v2(device_id, IBRT_ACTION_REDIAL, 0, 0);
        }
    #endif
}

static void app_ibrt_tws_factory_reset_test(const char* param, uint32_t len)
{
    EARBUDS_TRACE(1,"%s", __func__);
    nv_record_rebuild(NV_REBUILD_ALL);
    //pmu_reboot();
}

static void app_ibrt_set_access_mode_test(const char* param, uint32 len)
{
    app_bt_set_access_mode(BTIF_BAM_GENERAL_ACCESSIBLE);
}

static void app_ibrt_get_pnp_info_test(const char* param, uint32 len)
{
#if defined(BT_DIP_SUPPORT)
    struct BT_DEVICE_T *curr_device = app_bt_get_device(BT_DEVICE_ID_1);
    if (!curr_device)
    {
        EARBUDS_TRACE(1, "%s N/A", __func__);
        return;
    }

    bt_dip_pnp_info_t pnp_info = {0};
    bta_dip_get_pnp_info(&curr_device->remote, &pnp_info);

    EARBUDS_TRACE(4, "%s vendor_id %04x product_id %04x product_version %04x",
            __func__, pnp_info.vend_id, pnp_info.prod_id, pnp_info.prod_ver);
#endif
}

static void app_ibrt_send_pause_test(const char* param, uint32 len)
{
    bt_bdaddr_t addr = {0};
    bta_get_addr_by_device_id(BT_DEVICE_ID_1, &addr);
    bta_avrcp_send_pause(&addr);
}

static void app_ibrt_send_play_test(const char* param, uint32 len)
{
    bt_bdaddr_t addr = {0};
    bta_get_addr_by_device_id(BT_DEVICE_ID_1, &addr);
    bta_avrcp_send_play(&addr);
}

static void app_ibrt_send_forward_test(const char* param, uint32 len)
{
    bt_bdaddr_t addr = {0};
    bta_get_addr_by_device_id(BT_DEVICE_ID_1, &addr);
    bta_avrcp_send_forward(&addr);
}

static void app_ibrt_send_backward_test(const char* param, uint32 len)
{
    bt_bdaddr_t addr = {0};
    bta_get_addr_by_device_id(BT_DEVICE_ID_1, &addr);
    bta_avrcp_send_backward(&addr);
}

static void app_ibrt_send_volume_up_test(const char* param, uint32 len)
{
    bt_bdaddr_t addr = {0};
    bta_get_addr_by_device_id(BT_DEVICE_ID_1, &addr);
    bta_avrcp_send_volume_up(&addr);
}

static void app_ibrt_send_volume_down_test(const char* param, uint32 len)
{
    bt_bdaddr_t addr = {0};
    bta_get_addr_by_device_id(BT_DEVICE_ID_1, &addr);
    bta_avrcp_send_volume_down(&addr);
}

static void app_ibrt_send_set_abs_volume_test(const char* param, uint32 len)
{
    static uint8_t abs_vol = 80;
    bt_bdaddr_t addr = {0};
    bta_get_addr_by_device_id(BT_DEVICE_ID_1, &addr);
    bta_avrcp_send_set_abs_volume(&addr, abs_vol++);
}

static void app_ibrt_adjust_local_volume_up_test(const char* param, uint32 len)
{
    bta_tws_set_local_volume_up();
}

static void app_ibrt_adjust_local_volume_down_test(const char* param, uint32 len)
{
    bta_tws_set_local_volume_down();
}

static void app_ibrt_hf_call_redial_test(const char* param, uint32 len)
{
    bt_bdaddr_t addr = {0};
    bta_get_addr_by_device_id(BT_DEVICE_ID_1, &addr);
    bta_hf_call_redial(&addr);
}

static void app_ibrt_hf_call_answer_test(const char* param, uint32 len)
{
    bt_bdaddr_t addr = {0};
    bta_get_addr_by_device_id(BT_DEVICE_ID_1, &addr);
    bta_hf_call_answer(&addr);
}

static void app_ibrt_hf_call_hangup_test(const char* param, uint32 len)
{
    bt_bdaddr_t addr = {0};
    bta_get_addr_by_device_id(BT_DEVICE_ID_1, &addr);
    bta_hf_call_hangup(&addr);
}

static void app_ibrt_switch_streaming_sco_test(const char* param, uint32 len)
{
    bta_tws_switch_streaming_sco();
}

static void app_ibrt_switch_streaming_a2dp_test(const char* param, uint32 len)
{
    bta_tws_switch_streaming_a2dp();
}


static void app_ibrt_a2dp_profile_conn_test(const char* param, uint32 len)
{
    bt_bdaddr_t addr = {0};
    bta_get_addr_by_device_id(BT_DEVICE_ID_1, &addr);
    bta_a2dp_connect(&addr);
}

static void app_ibrt_a2dp_profile_disc_test(const char* param, uint32 len)
{
    bt_bdaddr_t addr = {0};
    bta_get_addr_by_device_id(BT_DEVICE_ID_1, &addr);
    bta_a2dp_disconnect(&addr);
}

static void app_ibrt_send_tota_data_test(const char* param, uint32 len)
{
#ifdef TOTA_v2
#ifdef IS_TOTA_LOG_PRINT_ENABLED
    app_ibrt_if_tota_printf("TOTA TEST");
#endif
#endif
}

static void app_ibrt_connect_tws_test(const char* param, uint32 len)
{
    bts_tws_if_connect_acl_link(0, 0);
}

static void app_ibrt_connect_mobile_test(const char* param, uint32_t len)
{
    int bytes[sizeof(bt_bdaddr_t)] = {0};

    if (len < 17)
    {
        EARBUDS_TRACE(0, "%s wrong len %d '%s'", __func__, len, param);
        return;
    }

    sscanf(param, "%x:%x:%x:%x:%x:%x", bytes+0, bytes+1, bytes+2, bytes+3, bytes+4, bytes+5);

    bt_bdaddr_t addr = {{
        (uint8_t)(bytes[0]&0xff),
        (uint8_t)(bytes[1]&0xff),
        (uint8_t)(bytes[2]&0xff),
        (uint8_t)(bytes[3]&0xff),
        (uint8_t)(bytes[4]&0xff),
        (uint8_t)(bytes[5]&0xff)}};

    EARBUDS_DUMP8("%02x ", addr.address, BD_ADDR_LEN);

    bta_tws_connect_bt_device(&addr, 0);
}

static void app_ibrt_connect_ibrt_test(const char* param, uint32_t len)
{
    int bytes[sizeof(bt_bdaddr_t)] = {0};

    if (len < 17)
    {
        EARBUDS_TRACE(0, "%s wrong len %d '%s'", __func__, len, param);
        return;
    }

    sscanf(param, "%x:%x:%x:%x:%x:%x", bytes+0, bytes+1, bytes+2, bytes+3, bytes+4, bytes+5);

    bt_bdaddr_t addr = {{
            (uint8_t)(bytes[0]&0xff),
            (uint8_t)(bytes[1]&0xff),
            (uint8_t)(bytes[2]&0xff),
            (uint8_t)(bytes[3]&0xff),
            (uint8_t)(bytes[4]&0xff),
            (uint8_t)(bytes[5]&0xff)}};

    EARBUDS_DUMP8("%02x ", addr.address, BD_ADDR_LEN);

    bts_ibrt_if_dev_connect_ibrt_link(&addr);
}

static void app_ibrt_role_switch_test(const char* param, uint32_t len)
{
    int bytes[sizeof(bt_bdaddr_t)] = {0};

    if (len < 17)
    {
        EARBUDS_TRACE(0, "%s wrong len %d '%s'", __func__, len, param);
        return;
    }

    sscanf(param, "%x:%x:%x:%x:%x:%x", bytes+0, bytes+1, bytes+2, bytes+3, bytes+4, bytes+5);

    bt_bdaddr_t addr = {{
            (uint8_t)(bytes[0]&0xff),
            (uint8_t)(bytes[1]&0xff),
            (uint8_t)(bytes[2]&0xff),
            (uint8_t)(bytes[3]&0xff),
            (uint8_t)(bytes[4]&0xff),
            (uint8_t)(bytes[5]&0xff)}};

    EARBUDS_DUMP8("%02x ", addr.address, BD_ADDR_LEN);

    bts_ibrt_conn_tws_role_switch(&addr);
}

#ifdef TOTA_v2
static void app_ibrt_tota_v2_send_data_test(const char* param, uint32_t len)
{
    if (bt_defer_curr_func_0(app_ibrt_tota_v2_send_data_test))
    {
        EARBUDS_TRACE(0, "tota_v2_send_data: queue to bt thread");
        return;
    }
    struct BT_DEVICE_T *curr_device = NULL;
    curr_device = app_bt_get_connected_device_byaddr(bta_get_pts_address());
    if (curr_device == NULL)
    {
        EARBUDS_TRACE(0, "tota_send_data: device not connected");
        return;
    }
    uint8_t *data = (uint8_t *)curr_device;
    EARBUDS_TRACE(0, "tota_v2_send_data: %p", data);
    app_ibrt_if_start_user_action_v2(curr_device->device_id, IBRT_ACTION_SEND_TOTA_DATA, (uint32_t)(uintptr_t)data, 128);
}
static void app_ibrt_tota_v2_more_data(const char* param, uint32_t len)
{
    if (bt_defer_curr_func_0(app_ibrt_tota_v2_more_data))
    {
        EARBUDS_TRACE(0, "tota_v2_more_data: queue to bt thread");
        return;
    }
    struct BT_DEVICE_T *curr_device = NULL;
    curr_device = app_bt_get_connected_device_byaddr(bta_get_pts_address());
    if (curr_device == NULL)
    {
        EARBUDS_TRACE(0, "tota_v2_more_data: device not connected");
        return;
    }
    uint8_t *data = (uint8_t *)curr_device;
    EARBUDS_TRACE(0, "tota_v2_more_data: 1st packet");
    app_ibrt_if_start_user_action_v2(curr_device->device_id, IBRT_ACTION_SEND_TOTA_DATA, (uint32_t)(uintptr_t)data, 128);
    EARBUDS_TRACE(0, "tota_v2_more_data: 2nd packet");
    app_ibrt_if_start_user_action_v2(curr_device->device_id, IBRT_ACTION_SEND_TOTA_DATA, (uint32_t)(uintptr_t)data, 128);
    EARBUDS_TRACE(0, "tota_v2_more_data: 3rd packet");
    app_ibrt_if_start_user_action_v2(curr_device->device_id, IBRT_ACTION_SEND_TOTA_DATA, (uint32_t)(uintptr_t)data, 128);
    EARBUDS_TRACE(0, "tota_v2_more_data: 4th packet");
    app_ibrt_if_start_user_action_v2(curr_device->device_id, IBRT_ACTION_SEND_TOTA_DATA, (uint32_t)(uintptr_t)data, 128);
    EARBUDS_TRACE(0, "tota_v2_more_data: 5th packet");
    app_ibrt_if_start_user_action_v2(curr_device->device_id, IBRT_ACTION_SEND_TOTA_DATA, (uint32_t)(uintptr_t)data, 128);
}
#endif

static void bestest_tws_connect(const char* param, uint32_t len)
{
    bta_tws_connect_tws_link();
}

static void bestest_auto_connect(const char* param, uint32_t len)
{
    if (param == NULL || len == 0 || strlen(param) == 0)
    {
        TRACE(0, "[bestest_auto_connect] invalid param, param=%p, len=%u", param, len);
        return;
    }

    uint8_t page_count = (uint16_t)atoi(param);

    bta_tws_connect_all_bt_devices(page_count);
}

static void bestest_all_disconnect(const char* param, uint32_t len)
{
    bta_tws_remove_all_devices();
}

void app_ibrt_tws_dump_audio_state_test(const char* param, uint32_t len)
{
    app_bt_audio_state_checker();
}

static void app_ibrt_mgr_open_box_event_test(const char* param, uint32_t len)
{
    bta_tws_box_event_entry(BTA_TWS_OPEN);
}

static void app_ibrt_mgr_fetch_out_box_event_test(const char* param, uint32_t len)
{
    bta_tws_box_event_entry(BTA_TWS_UNDOCK);
}

static void app_ibrt_mgr_put_in_box_event_test(const char* param, uint32_t len)
{
    bta_tws_box_event_entry(BTA_TWS_DOCK);
}

static void app_ibrt_mgr_close_box_event_test(const char* param, uint32_t len)
{
    bta_tws_box_event_entry(BTA_TWS_CLOSE);
}

static void app_ibrt_mgr_wear_up_event_test(const char* param, uint32_t len)
{
    bta_tws_box_event_entry(BTA_TWS_WEAR_UP);
}

static void app_ibrt_mgr_wear_down_event_test(const char* param, uint32_t len)
{
    bta_tws_box_event_entry(BTA_TWS_WEAR_DOWN);
}

void app_ibrt_mgr_free_man_test(void)
{
    hal_sw_bootmode_set(HAL_SW_BOOTMODE_CUSTOM_OP1_AFTER_REBOOT);
    app_reset();
}

static void app_ibrt_mgr_free_man_test_1(const char* param, uint32_t len)
{
    bta_tws_enable_freeman_mode(true);
}

static void app_ibrt_mgr_dump_ui_status_test(const char* param, uint32_t len)
{
    // app_ibrt_if_dump_ui_status();
}

void app_ibrt_mgr_switch_single_point_test(const char* param, uint32_t len)
{
    bta_tws_set_device_num_max(1, NULL, 0);
}

void app_ibrt_mgr_switch_double_point_test(const char* param, uint32_t len)
{
    bta_tws_set_device_num_max(2, NULL, 0);
}

void app_ibrt_mgr_switch_triple_point_test(const char* param, uint32_t len)
{
    bta_tws_set_device_num_max(3, NULL, 0);
}

static void app_ibrt_mgr_switch2walkie_talkie_test(const char* param, uint32_t len)
{
    app_switch_mode(NV_APP_WALKIE_TALKIE, true);
}

static void app_ibrt_dump_media_info_test(const char* param, uint32_t len)
{
    uint8_t support_max_remote_link = app_ibrt_conn_support_max_mobile_dev();
    for (int i = 0; i < support_max_remote_link; i++)
    {
        struct BT_DEVICE_T* bt_device = NULL;
        bt_device = app_bt_get_device(i);
        if(bt_device && bt_device ->acl_is_connected)
        {
            uint8_t btAddr[6] = {0};
            memcpy(btAddr, bt_device->remote.address, 6);
            uint8_t abs_vol = bt_device->a2dp_current_abs_volume;
            uint8_t a2dp_vol = a2dp_volume_get((enum BT_DEVICE_ID_T)i);
#ifdef MEDIA_PLAYER_SUPPORT
            uint8_t stream_vol = app_bt_stream_local_volume_get();
            uint8_t audio_codec = bta_get_a2dp_codec_type_by_id(i);
#else
            uint8_t stream_vol = 0x0;
            uint8_t audio_codec = 0x0;
#endif
            char ptrMediaInfo[128];
            sprintf(ptrMediaInfo, "[d%d]vol abs:%d, local:%d, stream:%d, codec:%d, rate:%d, audio:%d,",
                    i, abs_vol, a2dp_vol,stream_vol,
                    bt_device->codec_type, bt_device->sample_rate, audio_codec);
            app_ibrt_print_log_with_bt_address((const char *)ptrMediaInfo, btAddr);
        }
    }
}

static void app_ibrt_mgr_monitor_dump_test(const char* param, uint32_t len)
{
    // app_ui_monitor_dump();
}

void app_ibrt_mgr_user_role_switch_test0(const char* param, uint32_t len)
{
    uint8_t role = bts_core_get_ui_role();
    if (role == BT_IBRT_MASTER)
    {
        bta_tws_request_ui_role_switch(BT_IBRT_SLAVE);
    }
    else if (role == BT_IBRT_SLAVE)
    {
        bta_tws_request_ui_role_switch(BT_IBRT_MASTER);
    }
}

static void app_ibrt_mgr_user_role_switch_test(const char* param, uint32_t len)
{
    bta_tws_request_ui_role_switch(BT_IBRT_MASTER);
}

static void app_ibrt_mgr_user_role_switch_test1(const char* param, uint32_t len)
{
    bta_tws_request_ui_role_switch(BT_IBRT_SLAVE);
}

static void app_ibrt_mgr_enable_page_test(const char* param, uint32_t len)
{
    bta_tws_block_page(false);
}

static void app_ibrt_mgr_disable_page_test(const char* param, uint32_t len)
{
    bta_tws_block_page(true);
}

static void app_ibrt_mgr_shutdown_test(const char* param, uint32_t len)
{
    bta_tws_shutdown();
}

#if BLE_AUDIO_ENABLED
void app_ibrt_mgr_switch_lea(nvrec_appmode_e new_mode)
{
    bool enable = (new_mode == NV_APP_LEA_DISABLE_MODE);

    // * impl low layer lea switch operations
    bta_lea_switch(enable);

    // * for earbuds, notify peer to trigger peer switch
    bta_tws_send_cmd_without_rsp(APP_TWS_CMD_SEND_NV_APP_MODE, (uint8_t *)&new_mode, sizeof(nvrec_appmode_e));

    // * remove all bt pairing records
    nv_record_ddbrec_clear();
    // * remove all ble pairing records
    nv_record_ble_delete_all_entry();

    // * switch app mode, and reboot is necessary for lea switch
    app_switch_mode(new_mode, true);
}

void app_ibrt_mgr_disable_lea_test(const char* param, uint32_t len)
{
    app_ibrt_mgr_switch_lea(NV_APP_LEA_DISABLE_MODE);
}

void app_ibrt_mgr_enable_lea_test(const char* param, uint32_t len)
{
    app_ibrt_mgr_switch_lea(NV_APP_EARBUDS_MONO);
}
#endif

static void app_prompt_PlayAudio(const char* param, uint32_t len)
{
#ifdef MEDIA_PLAYER_SUPPORT
    audio_player_play_prompt(AUD_ID_BT_CALL_INCOMING_CALL, 0);
#endif
}

static void app_prompt_locally_PlayAudio(const char* param, uint32_t len)
{
#ifdef MEDIA_PLAYER_SUPPORT
    media_PlayAudio_locally(AUD_ID_BT_CALL_INCOMING_CALL, 0);
#endif
}

static void app_prompt_remotely_PlayAudio(const char* param, uint32_t len)
{
#ifdef MEDIA_PLAYER_SUPPORT
    media_PlayAudio_remotely(AUD_ID_BT_CALL_INCOMING_CALL, 0);
#endif
}

static void app_prompt_standalone_PlayAudio(const char* param, uint32_t len)
{
#ifdef MEDIA_PLAYER_SUPPORT
    media_PlayAudio_standalone(AUD_ID_BT_CALL_INCOMING_CALL, 0);
#endif
}

static void app_prompt_test_stop_all(const char* param, uint32_t len)
{
#ifdef MEDIA_PLAYER_SUPPORT
    app_prompt_stop_all();
#endif
}


#ifdef SLT_AUTO_TEST
static void app_prompt_1k_tone_test(const char* param, uint32_t len)
{
    uint8_t option = atoi((const char *)param);
    if(option)
    {
        media_PlayAudio_continuous_start(AUD_ID_TONE_1K, 0);
    }
    else
    {
        media_PlayAudio_continuous_end(AUD_ID_TONE_1K, 0);
    }
}
#endif

#ifdef ANC_APP
static void app_ibrt_anc_switch_test(const char* param, uint32_t len)
{
   app_anc_mode_t mode_int = APP_ANC_MODE_OFF;
   mode_int = (app_anc_mode_t)atoi((const char*)param);
   app_anc_switch(mode_int);
}
#endif

extern void bt_change_to_iic(APP_KEY_STATUS *status, void *param);
static void app_ibrt_ui_iic_uart_switch_test(const char* param, uint32_t len)
{
    bt_change_to_iic(NULL,NULL);
}

static bool trace_disabled;
static void app_trace_test_enable(const char* param, uint32_t len)
{
    if (trace_disabled) 
    {
        trace_disabled = false;
        hal_trace_continue();
    }
}

static void app_trace_test_disable(const char* param, uint32_t len)
{
    if (!trace_disabled) {
        trace_disabled = true;
        hal_trace_pause();
}
}

extern void bt_drv_reg_op_trigger_controller_assert(void);
static void app_test_bt_drv_reg_op_trigger_controller_assert(const char* param, uint32_t len)
{
    bt_drv_reg_op_trigger_controller_assert();
}

static void app_trace_test_get_history_trace(const char* param, uint32_t len)
{
    const uint8_t* preBuf;
    const uint8_t* postBuf;
    uint32_t preBufLen, postBufLen;
    hal_trace_get_history_buffer(&preBuf, &preBufLen, &postBuf, &postBufLen);
    EARBUDS_TRACE(0, "prelen %d postlen %d", preBufLen, postBufLen);
    uint8_t tmpTest[16];
    memset(tmpTest, 0, sizeof(tmpTest));
    memcpy(tmpTest, preBuf,
        (preBufLen > (sizeof(tmpTest)-1)?(sizeof(tmpTest)-1):preBufLen));
    EARBUDS_TRACE(0, "preBuf:");
    EARBUDS_TRACE(0, "%s", tmpTest);
    memset(tmpTest, 0, sizeof(tmpTest));
    memcpy(tmpTest, postBuf,
        (postBufLen > (sizeof(tmpTest)-1)?(sizeof(tmpTest)-1):postBufLen));
    EARBUDS_TRACE(0, "postBuf:");
    EARBUDS_TRACE(0, "%s", tmpTest);
}

static void app_trigger_rx_close_test(const char* param, uint32_t len)
{
    hal_trace_rx_sleep();
}

#ifdef GPIO_WAKEUP_ENABLE
#ifndef WAKEUP_PIN_NUM
#define WAKEUP_PIN_NUM                                  1_5
#endif
#define MAKE_WAKEUP_PIN_A(a)                            HAL_IOMUX_PIN_P ## a
#define MAKE_WAKEUP_PIN(a)                              MAKE_WAKEUP_PIN_A(a)
#define WAKEUP_PIN                                      MAKE_WAKEUP_PIN(WAKEUP_PIN_NUM)

static void app_trigger_gpio_irq_wakeup_test_handler(enum HAL_GPIO_PIN_T pin)
{
    EARBUDS_TRACE(0, "pull up pin:%d", pin);
    hal_trace_rx_wakeup();
}

static void app_trigger_gpio_irq_wakeup_set(bool en)
{
    uint32_t i;
    struct HAL_GPIO_IRQ_CFG_T gpio_cfg = {0, };
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP wakeup_iomux_tbl[] = {
        {WAKEUP_PIN, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLDOWN_ENABLE},
    };

    EARBUDS_TRACE(0, "%s pull up en:%d", __func__, en);

    gpio_cfg.irq_enable = en;
    gpio_cfg.irq_handler = app_trigger_gpio_irq_wakeup_test_handler;
    gpio_cfg.irq_debounce = false;
    gpio_cfg.irq_polarity = HAL_GPIO_IRQ_POLARITY_HIGH_RISING;
    gpio_cfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;

    for (i = 0; i < ARRAY_SIZE(wakeup_iomux_tbl); i++){
        hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&wakeup_iomux_tbl[i], 1);
        hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)wakeup_iomux_tbl[i].pin, HAL_GPIO_DIR_IN, 1);
        hal_gpio_setup_irq((enum HAL_GPIO_PIN_T)wakeup_iomux_tbl[i].pin, &gpio_cfg);
    }
}


static void app_trigger_gpio_irq_wakeup_enable_test(const char* param, uint32_t len)
{
    app_trigger_gpio_irq_wakeup_set(true);
}

static void app_trigger_gpio_irq_wakeup_disable_test(const char* param, uint32_t len)
{
    app_trigger_gpio_irq_wakeup_set(false);
}


POSSIBLY_UNUSED static void app_trigger_gpio_out_operation(const char* param, uint32_t len)
{
    static bool gpio_out_init = false;
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP out_table[] = {
            {HAL_IOMUX_PIN_P1_5, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
            {HAL_IOMUX_PIN_P1_6, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        };
    if(!gpio_out_init){
        hal_iomux_init(out_table, ARRAY_SIZE(out_table));
        for (uint32_t i = 0; i < ARRAY_SIZE(out_table); i++){
            hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)out_table[i].pin, HAL_GPIO_DIR_OUT, 1);
        }
    }
    for (uint32_t i = 0; i < ARRAY_SIZE(out_table); i++){
        hal_gpio_pin_set((enum HAL_GPIO_PIN_T)out_table[i].pin);
        osDelay(500);
        hal_gpio_pin_clr((enum HAL_GPIO_PIN_T)out_table[i].pin);
    }
}

#endif

#if defined( __BT_ANC_KEY__)&&defined(ANC_APP)
static void app_test_anc_handler(const char* param, uint32_t len)
{
    app_anc_key(NULL, NULL);
}
#endif

#ifdef __AI_VOICE__
extern void app_test_toggle_hotword_supervising(void);
static void app_test_thirdparty_hotword(const char* param, uint32_t len)
{
    app_test_toggle_hotword_supervising();
}
#endif
#ifdef SENSOR_HUB
#include "rx_sens_trc.h"
#ifdef CORE_BRIDGE_DEMO_MSG
#include "mcu_sensor_hub_app.h"

static void app_test_mcu_sensorhub_demo_req_no_rsp(const char* param, uint32_t len)
{
    app_mcu_sensor_hub_send_demo_req_no_rsp();
}

static void app_test_mcu_sensorhub_demo_req_with_rsp(const char* param, uint32_t len)
{
    app_mcu_sensor_hub_send_demo_req_with_rsp();
}

static void app_test_mcu_sensorhub_demo_instant_req(const char* param, uint32_t len)
{
    app_mcu_sensor_hub_send_demo_instant_req();
}
#endif

static void app_test_mcu_sensorhub_control_sensor_trace_open(const char* param, uint32_t len)
{
    sensor_hub_trace_server_open();
}

static void app_test_mcu_sensorhub_control_sensor_trace_close(const char* param, uint32_t len)
{
    sensor_hub_trace_server_close();
}
#endif   /* SENSOR_HUB */

static void test_master_tws_pairing(const char* param, uint32_t len)
{
    bt_bdaddr_t twsMasterBtAddr;

    if(parse_bt_addr(param, &twsMasterBtAddr, len) == 1)
    {
        bta_tws_box_event_entry(BTA_TWS_OPEN);
        bta_tws_reconfig_nv_role(BT_IBRT_MASTER, &twsMasterBtAddr);
    }
}

static void test_slave_tws_pairing(const char* param, uint32_t len)
{
    bt_bdaddr_t twsSlaveBtAddr;

    if(parse_bt_addr(param, &twsSlaveBtAddr, len) == 1)
    {
        bta_tws_box_event_entry(BTA_TWS_OPEN);
        bta_tws_reconfig_nv_role(BT_IBRT_SLAVE, &twsSlaveBtAddr);
    }
}

static void master_update_tws_pair_info_test_func(const char* param, uint32_t len)
{
    bt_bdaddr_t twsSlaveBtAddr;

    if(parse_bt_addr(param, &twsSlaveBtAddr, len) == 1)
    {
        bts_tws_if_update_tws_pairing_info(BT_IBRT_MASTER, (uint8_t *)&twsSlaveBtAddr);
    }
}

static void slave_update_tws_pair_info_test_func(const char* param, uint32_t len)
{
    bt_bdaddr_t twsMasterBtAddr;

    if(parse_bt_addr(param, &twsMasterBtAddr, len) == 1)
    {
        bts_tws_if_update_tws_pairing_info(BT_IBRT_SLAVE, (uint8_t *)&twsMasterBtAddr);
    }
}

static void m_write_bt_local_addr_test(const char* param, uint32_t len)
{
    bt_bdaddr_t twsMasterBtAddr;

    if(parse_bt_addr(param, &twsMasterBtAddr, len) == 1)
    {
        bta_bt_set_local_addr(&twsMasterBtAddr);
    }
}

static void s_write_bt_local_addr_test(const char* param, uint32_t len)
{
    bt_bdaddr_t twsSlaveBtAddr;

    if(parse_bt_addr(param, &twsSlaveBtAddr, len) == 1)
    {
        bta_bt_set_local_addr(&twsSlaveBtAddr);
    }
}

static void m_write_ble_local_addr_test(const char* param, uint32_t len)
{
#ifdef BLE_HOST_SUPPORT
    bt_bdaddr_t twsMasterBleAddr;

    if(parse_bt_addr(param, &twsMasterBleAddr, len) == 1)
    {
        bta_ble_set_local_addr((ble_bdaddr_t *)&twsMasterBleAddr);
    }
#endif
}

static void s_write_ble_local_addr_test(const char* param, uint32_t len)
{
#ifdef BLE_HOST_SUPPORT
    bt_bdaddr_t twsSlaveBleAddr;

    if(parse_bt_addr(param, &twsSlaveBleAddr, len) == 1)
    {
        bta_ble_set_local_addr((ble_bdaddr_t *)&twsSlaveBleAddr);
    }
#endif
}

static void turn_on_jlink_test(const char* param, uint32_t len)
{
    hal_iomux_set_jtag();
    hal_cmu_jtag_clock_enable();
}

#ifdef AUDIO_MANAGER_TEST_ENABLE
static void app_audio_adm_api_ut_test(const char* param, uint32_t len)
{
    app_audio_adm_api_ut();
}

static void app_audio_arm_api_ut_test0(const char* param, uint32_t len)
{
    app_audio_arm_api_ut(0);
}

static void app_audio_arm_api_ut_test1(const char* param, uint32_t len)
{
    app_audio_arm_api_ut(1);
}

static void app_audio_arm_api_ut_test2(const char* param, uint32_t len)
{
    app_audio_arm_api_ut(2);
}

static void app_audio_focus_stack_test(const char* param, uint32_t len)
{
    app_audio_focus_stack_unit_test();
}

static void app_audio_focus_media_ctrl_test(const char* param, uint32_t len)
{
    app_audio_focus_ctrl_stub_test();
}
#endif

#ifdef MEDIA_PLAYER_SUPPORT
extern void increase_dst_mtu(void);
extern void decrease_dst_mtu(void);
static void app_test_increase_dst_mtu(const char* param, uint32_t len)
{
    increase_dst_mtu();
}

static void app_test_decrease_dst_mtu(const char* param, uint32_t len)
{
    decrease_dst_mtu();
}
#endif

#if BLE_AUDIO_ENABLED
static void app_ibrt_tws_dump_ble_conn_state(const char* param, uint32_t len)
{
    bes_ble_audio_dump_conn_state();
}

static void app_ibrt_ui_dump_adm_active_info(const char* param, uint32_t len)
{
    PLAYBACK_INFO_T* audioContrlInfo = app_bt_get_music_playback_info();
    CALL_STATE_INFO_T* callInfo = app_bt_get_call_state_info();
    uint8_t deviceType = audioContrlInfo->device_type;
    uint8_t deviceId = audioContrlInfo->device_id;
    uint8_t playState = (uint8_t)audioContrlInfo->playback_status;
    uint8_t call_id = callInfo->device_id;
    uint8_t callType = callInfo->device_type;
    uint8_t callState = (uint8_t)callInfo->state;
    char ptrAudioCtr[128];
    uint8_t btAddr[6] = {0};
    switch (deviceType)
    {
        case AUDIO_TYPE_BT:
        {
            struct BT_ADAPTER_DEVICE_T *btDevice = bt_adapter_get_device(deviceId);
            if(NULL != btDevice)
            {
                sprintf(ptrAudioCtr, "audio_control: device: %d, type: %d, music: %d", deviceId, deviceType, playState);
                memcpy(btAddr, btDevice->remote.address, 6);
                app_ibrt_print_log_with_bt_address((const char *)ptrAudioCtr, btAddr);
            }
            break;
        }
        case AUDIO_TYPE_LE_AUDIO:
        {
            ble_bdaddr_t *pLEMobileInfo = bes_ble_aob_conn_get_remote_address(deviceId);
            if(NULL != pLEMobileInfo)
            {
                sprintf(ptrAudioCtr, "audio_control: device: %d, type: %d, music: %d", deviceId, deviceType, playState);
                memcpy(btAddr, pLEMobileInfo->addr, 6);
                app_ibrt_print_log_with_bt_address((const char *)ptrAudioCtr, btAddr);
            }
            break;
        }
        default:
            sprintf(ptrAudioCtr, "audio_control: device: %d, type: %d, music: %d", deviceId, deviceType, playState);
            app_ibrt_print_log_with_bt_address((const char *)ptrAudioCtr, btAddr);
            break;

    }
    memset(btAddr, 0, 6);
    memset(ptrAudioCtr, 0, 128);
    switch (callType)
    {
        case AUDIO_TYPE_BT:
        {
            struct BT_ADAPTER_DEVICE_T *btDevice = bt_adapter_get_device(call_id);
            if(NULL != btDevice)
            {
                sprintf(ptrAudioCtr, "audio_control: device: %d, type: %d, call: %d", call_id, callType, callState);
                memcpy(btAddr, btDevice->remote.address, 6);
                app_ibrt_print_log_with_bt_address((const char *)ptrAudioCtr, btAddr);
            }
            break;
        }
        case AUDIO_TYPE_LE_AUDIO:
        {
            ble_bdaddr_t *pLEMobileInfo = bes_ble_aob_conn_get_remote_address(deviceId);
            if(NULL != pLEMobileInfo)
            {
                sprintf(ptrAudioCtr, "audio_control: device: %d, type: %d, call: %d", call_id, callType, callState);
                memcpy(btAddr, pLEMobileInfo->addr, 6);
                app_ibrt_print_log_with_bt_address((const char *)ptrAudioCtr, btAddr);
            }
            break;
        }
        default:
            sprintf(ptrAudioCtr, "audio_control: device: %d, type: %d, call: %d", call_id, callType, callState);
            app_ibrt_print_log_with_bt_address((const char *)ptrAudioCtr, btAddr);
            break;

    }

}
#endif

#ifdef CFG_LE_PWR_CTRL
void app_ibrt_path_loss_test(const char* param, uint32_t len)
{
    EARBUDS_TRACE(0,"%s", __func__);
    appm_set_path_loss_rep_param_cmd(0, 1, 1, 1, 0, 0, 1);
}
#endif

#ifdef ANC_ASSIST_ENABLED
#include "bluetooth_bt_api.h"

extern "C" void media_PlayAudio(AUD_ID_ENUM id,uint8_t device_id);

static void app_test_adaptive_eq(const char* param, uint32_t len)
{
    audio_player_play_prompt(AUD_ID_ANC_PROMPT,0);
}
#endif

#ifdef HEARING_AID_ENABLED
extern "C" int hearing_aid_stream_start(bool on);

static void app_hearing_aid_start(const char* param, uint32_t len)
{
    hearing_aid_stream_start(true);
}

static void app_hearing_aid_stop(const char* param, uint32_t len)
{
    hearing_aid_stream_start(false);
}
#endif

#if BLE_AUDIO_ENABLED
/***************************************************************************
Function    :  app_ibrt_tws_active_device_test
Parameters:
Return:
Description:
    test the correct of active device:
    1、if current is a2dp start,the action is send avrcp pause key to stop a2dp play.
    2、if current is a2dp suspend,the action is send avrcp play key to start a2dp play
    3、if current is incoming call,the action is answer call.
    4、if current is call active，the action is hungup call.
****************************************************************************/

static void app_ibrt_tws_active_device_test(const char* param, uint32_t len)
{
#if BLE_AUDIO_ENABLED
    struct BT_DEVICE_T *curr_device = NULL;
    uint8_t state_action = IBRT_ACTION_PLAY;
    BT_AUDIO_DEVICE_T *active_device = app_audio_adm_get_active_device();
    ASSERT(active_device,"no active device");
    uint8_t device_id = active_device->device_id;
    curr_device = app_bt_get_device(device_id);
    if(curr_device->hfchan_call == BT_HFP_CALL_ACTIVE)
    {
        state_action = IBRT_ACTION_HANGUP;
    }
    else if(curr_device->hfchan_callSetup == BT_HFP_CALL_SETUP_IN)
    {
        state_action = IBRT_ACTION_ANSWER;
    }
    else if(curr_device->a2dp_streamming)
    {
        state_action = IBRT_ACTION_PAUSE;
    }
    else
    {
        state_action = IBRT_ACTION_PLAY;
    }
    EARBUDS_TRACE(2,"active_device is:d%x,action:%d",device_id, state_action);
    app_ibrt_if_start_user_action_v2(device_id, state_action , 0, 0);
#endif
}

static void app_ibrt_tws_force_set_bt_as_act_dev_test(const char* param, uint32_t len)
{
#if BLE_AUDIO_ENABLED
    BT_AUDIO_DEVICE_T bt_device = {
          .device_type = AUDIO_TYPE_BT,
          .device_id = 0 };

    app_adm_force_set_active_device(&bt_device);
#endif
}

static void app_ibrt_tws_force_set_ble_as_act_dev_test(const char* param, uint32_t len)
{
#if BLE_AUDIO_ENABLED
     BT_AUDIO_DEVICE_T ble_device = {
          .device_type = AUDIO_TYPE_LE_AUDIO,
          .device_id = 0 };

    app_adm_force_set_active_device(&ble_device);
#endif
}
#endif

static void app_ibrt_tws_switch_a2dp_test(const char* param, uint32_t len)
{
    bta_tws_switch_streaming_a2dp();
}

void app_ibrt_tws_switch_sco_test(const char* param, uint32_t len)
{
    bta_tws_hold_background_switch();
}

void app_ibrt_tws_audio_ctrl_test(const char* param, uint32_t len)
{
    bt_key_handle_func_click();
}

void app_terminate_call_test(const char* param, uint32_t len)
{
    app_audio_control_call_terminate();
}

void app_hold_call_test(const char* param, uint32_t len)
{
    app_audio_control_handle_three_way_incoming_call();
}

void app_reject_3way_incoming(const char* param, uint32_t len)
{
    app_audio_control_release_hold_call_and_3way_incoming();
}

void app_reject_3way_active(const char* param, uint32_t len)
{
    app_audio_control_release_active_call();
}

void app_answer_call_test(const char* param, uint32_t len)
{
    app_audio_control_call_answer();
}

void app_play_muisc_test(const char* param, uint32_t len)
{
    app_audio_control_media_play();
}

void app_pause_music_test(const char* param, uint32_t len)
{
    app_audio_control_media_pause();
}

void app_dump_hfp_machine_test(const char* param, uint32_t len)
{
    app_bt_audio_get_current_hfp_machine();
}

void app_ibrt_a2dp_enable_aac_test(const char* param, uint32_t len)
{
    app_bt_a2dp_disable_aac_codec(false);
}

void app_ibrt_a2dp_disable_aac_test(const char* param, uint32_t len)
{
    app_bt_a2dp_disable_aac_codec(true);
}

void app_ibrt_a2dp_enable_sbc_test(const char* param, uint32_t len)
{
    app_bt_a2dp_disable_sbc_codec(false);
}

void app_ibrt_a2dp_disable_sbc_test(const char* param, uint32_t len)
{
    app_bt_a2dp_disable_sbc_codec(true);
}

#ifdef VOICE_SERVER
static void app_ibrt_mgr_voice_server_playback_start_test(const char* param, uint32 len)
{
    app_voice_server_start();
}

static void app_ibrt_mgr_voice_server_playback_stop_test(const char* param, uint32 len)
{
    app_voice_server_stop();
}
#endif

#ifdef SLT_TEST_ENABLE
static void app_ibrt_slt_test_mcu_freq_clkout_enable(const char* param, uint32 len)
{
    EARBUDS_TRACE(0,"%s", __func__);
    hal_iomux_set_mcu_clock_out();
}

static void app_ibrt_slt_test_mcu_freq_clkout_disable(const char* param, uint32 len)
{
    EARBUDS_TRACE(0,"%s", __func__);
    hal_iomux_clr_clock_out();
}

static void app_ibrt_slt_test_mcu_req_208m(const char* param, uint32 len)
{
    EARBUDS_TRACE(0,"%s", __func__);
    app_sysfreq_req(APP_SYSFREQ_SLT_TEST, APP_SYSFREQ_208M);
}

static void app_ibrt_slt_test_mcu_req_104m(const char* param, uint32 len)
{
    EARBUDS_TRACE(0,"%s", __func__);
    app_sysfreq_req(APP_SYSFREQ_SLT_TEST, APP_SYSFREQ_104M);
}

static void app_ibrt_slt_test_mcu_req_52m(const char* param, uint32 len)
{
    EARBUDS_TRACE(0,"%s", __func__);
    app_sysfreq_req(APP_SYSFREQ_SLT_TEST, APP_SYSFREQ_52M);
}

static void app_ibrt_slt_test_mcu_req_26m(const char* param, uint32 len)
{
    EARBUDS_TRACE(0,"%s", __func__);
    app_sysfreq_req(APP_SYSFREQ_SLT_TEST, APP_SYSFREQ_26M);
}

// Need call this function before enter deep sleep!
static void app_ibrt_slt_test_mcu_req_32k(const char* param, uint32 len)
{
    EARBUDS_TRACE(0,"%s", __func__);
    app_sysfreq_req(APP_SYSFREQ_SLT_TEST, APP_SYSFREQ_32K);
}
#endif

// #ifdef SINGLE_WIRE_DOWNLOAD
#include "hal_bootmode.h"
#include "hal_cmu.h"
static void app_single_download_test(const char* param, uint32 len)
{
    EARBUDS_TRACE (1, "%s ",__func__);
    hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT);
    hal_sw_bootmode_set(HAL_SW_BOOTMODE_SINGLE_LINE_DOWNLOAD);
    pmu_reboot();
    osDelay(500);
}
// #endif

#ifdef SPA_AUDIO_SEC
#include "tz_audio_process.h"
#include "ree_audio_process.h"

static void app_tz_audio_process_demo_func_enable_disable(const char* cmd, uint32_t cmd_len)
{
    int enable_disable = 0;

    if (1 != sscanf(cmd, "%d", &enable_disable))
    {
        EARBUDS_TRACE(0,"%s invalid param %s", __func__, cmd);
        return;
    }

    EARBUDS_TRACE(0,"%s len %d param[0]: 0x%02x 0x%02x",__func__,cmd_len,cmd[0],enable_disable);
    tz_audio_process_demo_func_enable_disable(enable_disable);
}

static void app_ree_audio_process_demo_func_enable_disable(const char* cmd, uint32_t cmd_len)
{
    int enable_disable = 0;

    if (1 != sscanf(cmd, "%d", &enable_disable))
    {
        EARBUDS_TRACE(0,"%s invalid param %s", __func__, cmd);
        return;
    }

    EARBUDS_TRACE(0,"%s len %d param[0]: 0x%02x 0x%02x",__func__,cmd_len,cmd[0],enable_disable);
    ree_audio_test_demo_enable_disable(enable_disable);
}
#endif

/*
* Connection step: tws_slave_pairing_test->tws_master_pairing_test->
* w4_host_connect -> start_ibrt_test
*/
const static app_ibrt_test_cmd_table_t app_ibrt_test_cmd_table[]=
{
    {"soft_reset",                  app_ibrt_soft_reset_test},
// #ifdef SINGLE_WIRE_DOWNLOAD
    {"test_single_download",        app_single_download_test},
// #endif
    {"reboot_with_delete_nv",       app_ibrt_soft_reset_delete_bt_nv_test},
#ifdef __IAG_BLE_INCLUDE__
    {"delete_all_ble_nv_info",      app_ibrt_nv_record_ble_delete_all_entry_test},
#endif
    {"ib_set_access_mode",          app_ibrt_set_access_mode_test},
    {"ib_get_pnp_info",             app_ibrt_get_pnp_info_test},
    {"ib_send_pause",               app_ibrt_send_pause_test},
    {"ib_send_play",                app_ibrt_send_play_test},
    {"ib_send_forward",             app_ibrt_send_forward_test},
    {"ib_send_backward",            app_ibrt_send_backward_test},
    {"ib_send_volumeup",            app_ibrt_send_volume_up_test},
    {"ib_send_volumedn",            app_ibrt_send_volume_down_test},
    {"ib_send_setabsvol",           app_ibrt_send_set_abs_volume_test},
    {"ib_adj_local_volup",          app_ibrt_adjust_local_volume_up_test},
    {"ib_adj_local_voldn",          app_ibrt_adjust_local_volume_down_test},
    {"ib_call_redial",              app_ibrt_hf_call_redial_test},
    {"ib_call_answer",              app_ibrt_hf_call_answer_test},
    {"ib_call_hangup",              app_ibrt_hf_call_hangup_test},
    {"ib_switch_sco",               app_ibrt_switch_streaming_sco_test},
    {"ib_switch_a2dp",              app_ibrt_switch_streaming_a2dp_test},
    {"ib_conn_a2dp",                app_ibrt_a2dp_profile_conn_test},
    {"ib_disc_a2dp",                app_ibrt_a2dp_profile_disc_test},
    {"ib_tota_send",                app_ibrt_send_tota_data_test},
    {"ib_c_m",                      app_ibrt_connect_mobile_test},
    {"ib_c_i",                      app_ibrt_connect_ibrt_test},
    {"ib_r_s",                      app_ibrt_role_switch_test},
#ifdef TOTA_v2
    {"tota_v2_send_data",           app_ibrt_tota_v2_send_data_test},
    {"tota_v2_more_data",           app_ibrt_tota_v2_more_data},
#endif

    {"tws_connect",                 bestest_tws_connect},
    {"auto_connect",                bestest_auto_connect},
    {"all_disconnect",              bestest_all_disconnect},
    {"trigger_assert",              app_ibrt_trigger_assert_test},

#if defined(IBRT_UI)
    {"open_box_event_test",         app_ibrt_mgr_open_box_event_test},
    {"fetch_out_box_event_test",    app_ibrt_mgr_fetch_out_box_event_test},
    {"put_in_box_event_test",       app_ibrt_mgr_put_in_box_event_test},
    {"close_box_event_test",        app_ibrt_mgr_close_box_event_test},
    {"wear_up_event_test",          app_ibrt_mgr_wear_up_event_test},
    {"wear_down_event_test",        app_ibrt_mgr_wear_down_event_test},
    {"pairing_mode_test",           app_ibrt_mgr_pairing_mode_test},
#ifdef VOICE_SERVER
    {"voice_server_playback_start", app_ibrt_mgr_voice_server_playback_start_test},
    {"voice_server_playback_stop",  app_ibrt_mgr_voice_server_playback_stop_test},
#endif
    {"free_man_test",               app_ibrt_mgr_free_man_test_1},
    {"dump_ui_status_test",         app_ibrt_mgr_dump_ui_status_test},
    {"switch2single_point",         app_ibrt_mgr_switch_single_point_test},
    {"switch2double_point",         app_ibrt_mgr_switch_double_point_test},
    {"switch2triple_point",         app_ibrt_mgr_switch_triple_point_test},
    {"switch2walkie_talkie",        app_ibrt_mgr_switch2walkie_talkie_test},
    {"monitor_dump_test",           app_ibrt_mgr_monitor_dump_test},
    {"user_rs_switch",              app_ibrt_mgr_user_role_switch_test0},
    {"user_rs_to_master",           app_ibrt_mgr_user_role_switch_test},
    {"user_rs_to_slave",            app_ibrt_mgr_user_role_switch_test1},
    {"enable_page",                 app_ibrt_mgr_enable_page_test},
    {"disable_page",                app_ibrt_mgr_disable_page_test},
    {"shutdown",                    app_ibrt_mgr_shutdown_test},
#if BLE_AUDIO_ENABLED
    {"disable_lea",                 app_ibrt_mgr_disable_lea_test},
    {"enable_lea",                  app_ibrt_mgr_enable_lea_test},
#endif
#endif
    {"iic_switch", app_ibrt_ui_iic_uart_switch_test},
    {"enable_access_mode_test",app_ibrt_enable_access_mode_test},
    {"disable_access_mode_test", app_ibrt_disable_access_mode_test},
    {"enter_freeman_test",app_ibrt_tws_freeman_pairing_test},
    {"tws_connect_test",app_ibrt_connect_tws_test},
    {"tws_disconnect_test",app_ibrt_tws_disconnect_test},
    {"connect_profiles_test",app_ibrt_connect_profiles_test},
    {"role_switch",app_ibrt_tws_role_switch_test},
    {"disc_all_mobile_test",app_ibrt_tws_disc_all_mobile_link_test},
    {"w4_mobile_connect_test",app_ibrt_tws_w4_mobile_connect_test},
    {"get_curr_link_count_test",app_ibrt_get_remote_device_count},
    {"dump_ibrt_conn_info",app_ibrt_tws_dump_info_test},
    {"get_mobile_conn_status_test",app_ibrt_tws_mobile_connection_test},
    {"get_tws_state",app_ibrt_get_tws_state_test},
    {"pause_music_test",app_ibrt_tws_avrcp_pause_test},
    {"play_music_test",app_ibrt_tws_avrcp_play_test},
    {"avrcp_vol_up_test",app_ibrt_tws_avrcp_vol_up_test},
    {"avrcp_vol_down_test",app_ibrt_tws_avrcp_vol_down_test},
    {"avrcp_next_track_test",app_ibrt_tws_avrcp_next_track_test},
    {"avrcp_prev_track_test",app_ibrt_tws_avrcp_prev_track_test},
    {"avrcp_fast_forward_start_test", app_ibrt_tws_avrcp_fast_forward_start_test},
    {"avrcp_fast_forward_stop_test", app_ibrt_tws_avrcp_fast_forward_stop_test},
    {"avrcp_rewind_start_test", app_ibrt_tws_avrcp_rewind_start_test},
    {"avrcp_rewind_stop_test", app_ibrt_tws_avrcp_rewind_stop_test},
    {"a2dp_active_phone",app_ibrt_ui_get_a2dp_active_phone},
    {"answer_call_test",app_ibrt_tws_hfp_answer_test},
    {"hfp_reject_test",app_ibrt_tws_hfp_reject_test},
    {"terminate_call_test",app_ibrt_tws_hfp_hangup_test},
    {"call_redial",app_ibrt_call_redial_test},
    {"app_prompt_PlayAudio",app_prompt_PlayAudio},
    {"app_prompt_locally_PlayAudio",app_prompt_locally_PlayAudio},
    {"app_prompt_remotely_PlayAudio",app_prompt_remotely_PlayAudio},
    {"app_prompt_standalone_PlayAudio",app_prompt_standalone_PlayAudio},
    {"app_prompt_test_stop_all",app_prompt_test_stop_all},
    {"trace_test_enable",app_trace_test_enable},
    {"trace_test_disable",app_trace_test_disable},
    {"test_get_history_trace",app_trace_test_get_history_trace},
    {"rx_close",app_trigger_rx_close_test},
    {"dump_a2dp_info", app_ibrt_dump_media_info_test},
    {"switch_a2dp_test", app_ibrt_tws_switch_a2dp_test},

#ifdef GPIO_WAKEUP_ENABLE
    {"gpio_wakeup_enable",  app_trigger_gpio_irq_wakeup_enable_test},
    {"gpio_wakeup_disable", app_trigger_gpio_irq_wakeup_disable_test},
#endif

    {"dump_audio_state_info", app_ibrt_tws_dump_audio_state_test},
    {"factory_reset", app_ibrt_tws_factory_reset_test},
#if BLE_AUDIO_ENABLED
    {"active device test",app_ibrt_tws_active_device_test},
    {"force_set_bt_act_dev",app_ibrt_tws_force_set_bt_as_act_dev_test},
    {"force_set_ble_act_dev",app_ibrt_tws_force_set_ble_as_act_dev_test},
#endif
    {"switch_sco_test",app_ibrt_tws_switch_sco_test},
    {"audio_ctrl_test",app_ibrt_tws_audio_ctrl_test},
    {"terminate_call_test",app_terminate_call_test},
    {"answer_call_test",app_answer_call_test},
    {"play_music_test",app_play_muisc_test},
    {"pause_music_test",app_pause_music_test},
    {"hold_call_test",app_hold_call_test},
    {"reject_3way_incoming_test",app_reject_3way_incoming},
    {"reject_3way_active_test",app_reject_3way_active},
    {"dump_hfp_machine",app_dump_hfp_machine_test},
    {"set_enable_aac_test", app_ibrt_a2dp_enable_aac_test},
    {"set_disable_aac_test", app_ibrt_a2dp_disable_aac_test},
    {"set_enable_sbc_test", app_ibrt_a2dp_enable_sbc_test},
    {"set_disable_sbc_test", app_ibrt_a2dp_disable_sbc_test},
#if defined( __BT_ANC_KEY__)&&defined(ANC_APP)
    {"toggle_anc_test",app_test_anc_handler},
#endif
#ifdef __AI_VOICE__
    {"toggle_hw_thirdparty",app_test_thirdparty_hotword},
#endif
#ifdef SENSOR_HUB
#ifdef CORE_BRIDGE_DEMO_MSG
    {"no_rsp_sensorhub_mcu_demo_cmd",app_test_mcu_sensorhub_demo_req_no_rsp},
    {"with_rsp_sensorhub_mcu_demo_cmd",app_test_mcu_sensorhub_demo_req_with_rsp},
    {"instant_sensorhub_mcu_demo_cmd",app_test_mcu_sensorhub_demo_instant_req},
#endif
    {"test_open_sensorhub_mcu_set_trace_cmd",app_test_mcu_sensorhub_control_sensor_trace_open},
    {"test_close_sensorhub_mcu_set_trace_cmd",app_test_mcu_sensorhub_control_sensor_trace_close},
#endif /*SENSOR_HUB*/

    {"trigger_controller_assert",app_test_bt_drv_reg_op_trigger_controller_assert},
    {"turn_on_jlink_test",turn_on_jlink_test},
#ifdef MEDIA_PLAYER_SUPPORT
    {"increase_dst_mtu",app_test_increase_dst_mtu},
    {"decrease_dst_mtu",app_test_decrease_dst_mtu},
#endif
    {"dip_test", app_ibrt_get_pnp_info_test},
#if BLE_AUDIO_ENABLED
    {"dump_ble_cnn_state",app_ibrt_tws_dump_ble_conn_state},
    {"adm_print",app_ibrt_ui_dump_adm_active_info},
#endif

#ifdef AUDIO_MANAGER_TEST_ENABLE
    {"adm_api_ut",app_audio_adm_api_ut_test},
    {"rdm_api_ut0",app_audio_arm_api_ut_test0},
    {"rdm_api_ut1",app_audio_arm_api_ut_test1},
    {"rdm_api_ut2",app_audio_arm_api_ut_test2},
    {"audio_focus_stack_test",app_audio_focus_stack_test},
    {"audio_focus_ctrl_test",app_audio_focus_media_ctrl_test},
#endif

#ifdef CFG_LE_PWR_CTRL
    {"path_loss_test", app_ibrt_path_loss_test},
#endif

#ifdef ANC_ASSIST_ENABLED
    {"adaptive_eq", app_test_adaptive_eq},
#endif

#ifdef HEARING_AID_ENABLED
    {"hearing_aid_start", app_hearing_aid_start},
    {"hearing_aid_stop", app_hearing_aid_stop},
#endif

#if SLT_AUTO_TEST
    {"gpio_out",                    app_trigger_gpio_out_operation},
#endif
#ifdef GPIO_WAKEUP_ENABLE
    {"gpio_wakeup_enable",	app_trigger_gpio_irq_wakeup_enable_test},
    {"gpio_wakeup_disable", app_trigger_gpio_irq_wakeup_disable_test},
#endif
    // SLT TEST
    // CPU freq
#ifdef SLT_TEST_ENABLE
    {"slt_test_cpu_req_clkout_enable",  app_ibrt_slt_test_mcu_freq_clkout_enable},
    {"slt_test_cpu_req_clkout_disable", app_ibrt_slt_test_mcu_freq_clkout_disable},
    {"slt_test_cpu_req_208m",           app_ibrt_slt_test_mcu_req_208m},
    {"slt_test_cpu_req_104m",           app_ibrt_slt_test_mcu_req_104m},
    {"slt_test_cpu_req_52m",            app_ibrt_slt_test_mcu_req_52m},
    {"slt_test_cpu_req_26m",            app_ibrt_slt_test_mcu_req_26m},
    {"slt_test_cpu_req_32k",            app_ibrt_slt_test_mcu_req_32k},
#endif
    {"get_a2dp_state_test",         app_ibrt_get_a2dp_state_test},
    {"get_avrcp_state_test",        app_ibrt_get_avrcp_state_test},
#ifdef BT_HFP_SUPPORT
    {"get_hfp_state_test",          app_ibrt_get_hfp_state_test},
    {"get_call_status",             app_ibrt_get_call_status_test},
#endif
    {"tws_get_role_test",           app_ibrt_tws_role_get_request_test_with_parameter},
    {"test_master_tws_pairing",     test_master_tws_pairing},
    {"test_slave_tws_pairing",      test_slave_tws_pairing},
    {"master_update_tws_pair_info_test_func",master_update_tws_pair_info_test_func},
    {"slave_update_tws_pair_info_test_func",slave_update_tws_pair_info_test_func},
    {"m_write_bt_local_addr_test",  m_write_bt_local_addr_test},
    {"s_write_bt_local_addr_test",  s_write_bt_local_addr_test},
    {"m_write_ble_local_addr_test", m_write_ble_local_addr_test},
    {"s_write_ble_local_addr_test", s_write_ble_local_addr_test},
    {"start_ibrt1_test",            app_ibrt_start_ibrt_test_with_addr},
    {"stop_ibrt1_test",             app_ibrt_stop_ibrt_test_with_addr},
    {"mobile1_connect_test",        app_ibrt_mobile_connect_test},
    {"mobile1_disconnect_test",     app_ibrt_mobile_disconnect_test},
    {"tws_role_swap_test",          app_ibrt_tws_role_switch_test_with_addr},
    {"set_codec_test",              app_ibrt_a2dp_a2dp_set_codec_test}, // ldac enable set_codec_test|ff0301 disable set_codec_test|ff0300

#ifdef ANC_APP
    {"anc_switch",                  app_ibrt_anc_switch_test},
#endif

#ifdef SLT_AUTO_TEST
    {"tone_1k_prompt",              app_prompt_1k_tone_test},
    {"gpio_pull_down",              app_trigger_gpio_irq_pull_down},
#endif
#ifdef SPA_AUDIO_SEC
    {"tz_demo_func_enable_disable",app_tz_audio_process_demo_func_enable_disable},
    {"ree_demo_func_enable_disable",app_ree_audio_process_demo_func_enable_disable},
#endif
};

unsigned int app_tws_ibrt_ui_cmd_callback(unsigned char *buf, unsigned int length)
{
    // Check len
    EARBUDS_TRACE(0, "[%s] cmd: %s", __func__, buf);
    int cmd_count;
    uint8_t cmd_number;
    char *para_addr=NULL;
    unsigned int para_len=0;

    cmd_number = ARRAY_SIZE(app_ibrt_test_cmd_table);
    for (cmd_count = 0; cmd_count < cmd_number; cmd_count++)
    {
        if ((strncmp((char*)buf, app_ibrt_test_cmd_table[cmd_count].string,
                strlen(app_ibrt_test_cmd_table[cmd_count].string)) == 0) ||
                strstr(app_ibrt_test_cmd_table[cmd_count].string, (char*)buf))
        {
            para_addr = strstr((char*)buf, "|");
            if(para_addr != NULL)
            {
                para_addr++;
                para_len = length - (para_addr - (char *)buf);
            }

            app_ibrt_test_cmd_table[cmd_count].cmd_function(para_addr, para_len);
            break;
        }
    }

    if (cmd_count >= cmd_number)
    {
        EARBUDS_TRACE(0,"bt host not found,cmd=%s", buf);
    }

    return 0;
}
#endif

void app_tws_ibrt_ui_cmd_init(void)
{
#ifdef APP_TRACE_RX_ENABLE
    EARBUDS_TRACE(0, "bt host cmd init");
    app_trace_rx_register("IBRT_UI", app_tws_ibrt_ui_cmd_callback);
#else
    EARBUDS_TRACE(0, "bt host cmd init fail!, not open APP_TRACE_RX_ENABLE");
#endif

    return;
}

void app_tws_ibrt_ui_cmd_deinit(void)
{
#ifdef APP_TRACE_RX_ENABLE
    EARBUDS_TRACE(0, "bt host cmd deinit");
    app_trace_rx_deregister("IBRT_UI");
#else
    EARBUDS_TRACE(0, "bt host cmd deinit fail!, not open APP_TRACE_RX_ENABLE");
#endif
}

void app_tws_ibrt_test_key_io_event(APP_KEY_STATUS *status, void *param)
{
    EARBUDS_TRACE(0,"app_tws_ibrt_test_key_io_event");
    EARBUDS_TRACE(3, "%s 000%d,%d",__func__, status->code, status->event);
#if 0//defined(IBRT_UI)
    switch(status->event)
    {
        case APP_KEY_EVENT_CLICK:
            if (status->code== APP_KEY_CODE_FN1)
            {
                bta_tws_box_event_entry(BTA_TWS_OPEN);
            }
            else if (status->code== APP_KEY_CODE_FN2)
            {
                bta_tws_box_event_entry(BTA_TWS_UNDOCK);
            }
            else
            {
                bta_tws_box_event_entry(BTA_TWS_WEAR_UP);
            }
            break;

        case APP_KEY_EVENT_DOUBLECLICK:
            if (status->code== APP_KEY_CODE_FN1)
            {
                bta_tws_box_event_entry(BTA_TWS_CLOSE);
            }
            else if (status->code== APP_KEY_CODE_FN2)
            {
                bta_tws_box_event_entry(BTA_TWS_DOCK);
            }
            else
            {
                bta_tws_box_event_entry(BTA_TWS_WEAR_DOWN);
            }
            break;

        case APP_KEY_EVENT_LONGPRESS:
            if (status->code== APP_KEY_CODE_FN1)
            {
            }
            else if (status->code== APP_KEY_CODE_FN2)
            {
            }
            else
            {
            }
            break;

        case APP_KEY_EVENT_TRIPLECLICK:
            break;

        case HAL_KEY_EVENT_LONGLONGPRESS:
            break;

        case APP_KEY_EVENT_ULTRACLICK:
            break;

        case APP_KEY_EVENT_RAMPAGECLICK:
            break;
    }
#endif
}

void app_ibrt_raw_ui_test_key(APP_KEY_STATUS *status, void *param)
{
#ifndef BESUI_KEY_EN
    uint8_t shutdown_key = HAL_KEY_EVENT_LONGLONGPRESS;
#endif
    uint8_t device_id = bts_am_get_device_for_user_action();
    struct BT_DEVICE_T *curr_device = app_bt_get_device(device_id);
    EARBUDS_TRACE(4, "%s %d,%d curr_device %x", __func__, status->code, status->event, device_id);

#ifdef BESUI_KEY_EN
    if(BT_IBRT_SLAVE == bts_ibrt_if_is_ibrt_link_connected(&curr_device->remote))
#else
    if (bts_ibrt_if_is_ibrt_link_connected(&curr_device->remote) && status->event != shutdown_key)
#endif
    {
        app_ibrt_keyboard_notify_v2(&curr_device->remote, status, param);
    }
    else
    {
#ifdef BESUI_KEY_EN
        app_key_gui_post_msg(false, status->code, status->event);
#else
#ifdef IBRT_SEARCH_UI
        app_ibrt_search_ui_handle_key_v2(&curr_device->remote, status, param);
#else
        app_ibrt_normal_ui_handle_key_v2(&curr_device->remote, status, param);
#endif
#endif
    }
}

#ifdef TOTA_FACTORY_USED
extern void app_bt_key_enter_tota_mode(APP_KEY_STATUS *status, void *param);
#endif
#ifdef DIRAC_AUDIO_ENABLE
void dirac_audio_onoff(APP_KEY_STATUS *status, void *param);
#endif

const APP_KEY_HANDLE  app_ibrt_ui_v2_test_key_cfg[] =
{
#if defined(__AI_VOICE__) || defined(BISTO_ENABLED)
    {{APP_KEY_CODE_GOOGLE, APP_KEY_EVENT_FIRST_DOWN}, "google assistant key", app_ai_manager_key_event_handle, NULL},
    {{APP_KEY_CODE_GOOGLE, APP_KEY_EVENT_UP}, "google assistant key", app_ai_manager_key_event_handle, NULL},
    {{APP_KEY_CODE_GOOGLE, APP_KEY_EVENT_LONGPRESS}, "google assistant key", app_ai_manager_key_event_handle, NULL},
    {{APP_KEY_CODE_GOOGLE, APP_KEY_EVENT_CLICK}, "google assistant key", app_ai_manager_key_event_handle, NULL},
    {{APP_KEY_CODE_GOOGLE, APP_KEY_EVENT_DOUBLECLICK}, "google assistant key", app_ai_manager_key_event_handle, NULL},
#endif
#ifdef DIRAC_AUDIO_ENABLE
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_CLICK},"bt dirac_audio_onoff key",dirac_audio_onoff, NULL},
#else
#if defined(__BT_ANC_KEY__)&&defined(ANC_APP)
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_CLICK},"bt anc key",app_anc_key, NULL},
#elif TOTA_FACTORY_USED
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_CLICK},"app_bt_key_enter_tota_mode", app_bt_key_enter_tota_mode, NULL},
#else
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_CLICK},"app_ibrt_ui_test_key", app_ibrt_raw_ui_test_key, NULL},
#endif
#endif //#ifdef DIRAC_AUDIO_ENABLE
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_LONGPRESS},"app_ibrt_ui_test_key", app_ibrt_raw_ui_test_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_LONGLONGPRESS},"app_ibrt_ui_test_key", app_ibrt_raw_ui_test_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_DOUBLECLICK},"app_ibrt_ui_test_key", app_ibrt_raw_ui_test_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_TRIPLECLICK},"app_ibrt_ui_test_key", app_ibrt_raw_ui_test_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_ULTRACLICK},"app_ibrt_ui_test_key", app_ibrt_raw_ui_test_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_RAMPAGECLICK},"app_ibrt_ui_test_key", app_ibrt_raw_ui_test_key, NULL},

    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_CLICK},"app_ibrt_service_test_key", app_tws_ibrt_test_key_io_event, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_DOUBLECLICK},"app_ibrt_service_test_key", app_tws_ibrt_test_key_io_event, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_LONGPRESS},"app_ibrt_service_test_key", app_tws_ibrt_test_key_io_event, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_CLICK},"app_ibrt_service_test_key", app_tws_ibrt_test_key_io_event, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_DOUBLECLICK},"app_ibrt_service_test_key", app_tws_ibrt_test_key_io_event, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_LONGPRESS},"app_ibrt_service_test_key", app_tws_ibrt_test_key_io_event, NULL},
    {{APP_KEY_CODE_FN3,APP_KEY_EVENT_CLICK},"app_ibrt_service_test_key", app_tws_ibrt_test_key_io_event, NULL},
    {{APP_KEY_CODE_FN3,APP_KEY_EVENT_DOUBLECLICK},"app_ibrt_service_test_key", app_tws_ibrt_test_key_io_event, NULL},
};

void app_tws_ibrt_raw_ui_test_key_init(void)
{
#ifdef APP_KEY_ENABLE
    EARBUDS_TRACE(0,"app_tws_ibrt_raw_ui_test_key_init");
    app_key_handle_clear();
    for (uint8_t i=0; i<ARRAY_SIZE(app_ibrt_ui_v2_test_key_cfg); i++)
    {
        app_key_handle_registration(&app_ibrt_ui_v2_test_key_cfg[i]);
    }
#endif
}

#endif
