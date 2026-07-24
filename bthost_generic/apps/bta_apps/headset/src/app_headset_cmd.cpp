#include "app_headset_cmd.h"
#include "app_headset.h"
#include "app_bta_cmd.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "hal_trace.h"
#include "app_trace_rx.h"
#include "bta_normal_ux_api.h"
#ifdef BIS_SELFSCAN_ENABLED
#include "app_bis_selfscan.h"
#endif
#ifndef TRACE
#define TRACE(attr, str, ...)               TR_INFO(attr, str, ##__VA_ARGS__)
#endif

typedef struct
{
    const char* string;
    void (*cmd_function)(const char* p, uint32_t p_len);
} cmd_table_item_t;

static bool str2bdaddr(const char* p, uint32_t len, struct bdaddr_t *out_addr)
{
    int bytes[BT_BD_ADDR_LEN] = {0};

    if (len < 17)
    {
        TRACE(0, "headset: %s unexpected len %d", p ? p : "NULL", len);
        return false;
    }

    if (BT_BD_ADDR_LEN != sscanf(p, "%x:%x:%x:%x:%x:%x", bytes + 0, bytes + 1, bytes + 2, bytes + 3, bytes + 4, bytes + 5))
    {
        TRACE(0, "headset: %s parse fail", p);
        return false;
    }

    bt_bdaddr_t addr =
    {{
        (uint8_t)(bytes[0] & 0xff),
        (uint8_t)(bytes[1] & 0xff),
        (uint8_t)(bytes[2] & 0xff),
        (uint8_t)(bytes[3] & 0xff),
        (uint8_t)(bytes[4] & 0xff),
        (uint8_t)(bytes[5] & 0xff),
    }};

    *out_addr = addr;

    return true;
}

static void cmd_open(const char* p, uint32_t p_len)
{
    app_bta_box_event_entry(APP_HEADSET_OPEN);
}

static void cmd_close(const char* p, uint32_t p_len)
{
    app_bta_box_event_entry(APP_HEADSET_CLOSE);
}

static void cmd_undock(const char* p, uint32_t p_len)
{
    app_bta_box_event_entry(APP_HEADSET_UNDOCK);
}

static void cmd_dock(const char* p, uint32_t p_len)
{
    app_bta_box_event_entry(APP_HEADSET_DOCK);
}

static void cmd_wear_up(const char* p, uint32_t p_len)
{
    app_bta_box_event_entry(APP_HEADSET_WEAR_UP);
}

static void cmd_wear_down(const char* p, uint32_t p_len)
{
    app_bta_box_event_entry(APP_HEADSET_WEAR_DOWN);
}

static void cmd_enter_pairing(const char* p, uint32_t p_len)
{
    bta_enable_pairing_mode(true);
}

static void cmd_exit_pairing(const char* p, uint32_t p_len)
{
    bta_enable_pairing_mode(false);
}

static void cmd_enable_multipoint(const char* p, uint32_t p_len)
{
    bta_set_device_num_max(2, NULL, 0);
}

static void cmd_disable_multipoint(const char* p, uint32_t p_len)
{
    bt_bdaddr_t addr = {};

    if (str2bdaddr(p, p_len, &addr))
    {
        bta_set_device_num_max(1, &addr, 1);
    }
    else
    {
        bta_set_device_num_max(1, NULL, 0);
    }
}

static void cmd_connect_bt_device(const char* p, uint32_t p_len)
{
    bt_bdaddr_t addr = {};

    if (str2bdaddr(p, p_len, &addr))
    {
        bta_connect_bt_device(&addr, 4, 0);
    }
}

static void cmd_connect_lea_device(const char* p, uint32_t p_len)
{
    bt_bdaddr_t addr = {};

    if (str2bdaddr(p, p_len, &addr))
    {
        bta_connect_lea_device(&addr, 8000);
    }
}

static void cmd_remove_bt_device(const char* p, uint32_t p_len)
{
    bt_bdaddr_t addr = {};

    if (str2bdaddr(p, p_len, &addr))
    {
        bta_remove_bt_device(&addr);
    }
}

static void cmd_remove_lea_device(const char* p, uint32_t p_len)
{
    bt_bdaddr_t addr = {};

    if (str2bdaddr(p, p_len, &addr))
    {
        bta_remove_lea_device(&addr);
    }
}

static void cmd_remove_bond(const char* p, uint32_t p_len)
{
    bt_bdaddr_t addr = {};

    if (str2bdaddr(p, p_len, &addr))
    {
        bta_remove_bond(&addr);
    }
}

static void cmd_enable_lea_advertiser(const char* p, uint32_t p_len)
{
    bool enable = atoi(p);
    bta_ux_enable_lea_advertiser(enable);
}

static void cmd_enable_br_edr_supported(const char* p, uint32_t p_len)
{
    bool enable = atoi(p);
    bta_ux_enable_br_edr_supported(enable);
}

static void cmd_block_page_when_streaming(const char* p, uint32_t p_len)
{
    bool block = atoi(p);
    bta_block_page_when_streaming(block);
}

static void cmd_without_reconnect_when_fetch_out_wear_up(const char* p, uint32_t p_len)
{
    bool value = atoi(p);
    app_headset_set_without_reconnect_when_fetch_out_wear_up(value);
}

#if BLE_AUDIO_ENABLED
static void cmd_bis_source_open_a2dp(const char* p, uint32_t p_len)
{
    bta_input_audio_type audio_type = BTA_AUDIO_TYPE_A2DP;
    app_bta_bis_src_set_audio_type(audio_type);
    app_bta_bis_source_open();
    TRACE(0, "LEA BIS source open command executed with audio type %d", audio_type);
}

static void cmd_bis_source_open_linein(const char* p, uint32_t p_len)
{
    bta_input_audio_type audio_type = BTA_AUDIO_TYPE_LINEIN;
    app_bta_bis_src_set_audio_type(audio_type);
    app_bta_bis_source_open();
    TRACE(0, "LEA BIS source open command executed with audio type %d", audio_type);
}

static void cmd_bis_source_open_usb(const char* p, uint32_t p_len)
{
    bta_input_audio_type audio_type = BTA_AUDIO_TYPE_USB;
    app_bta_bis_src_set_audio_type(audio_type);
    app_bta_bis_source_open();
    TRACE(0, "LEA BIS source open command executed with audio type %d", audio_type);
}
#endif

#ifdef BIS_SELFSCAN_ENABLED
static void cmd_bis_slef_scan_enable(const char* p, uint32_t p_len)
{
    TRACE(0, "LEA BIS self scan enable");
    app_bis_selfscan_enable_scan(true);
}
#endif

static const cmd_table_item_t cmd_table[] =
{
    { "open", cmd_open },
    { "close", cmd_close },
    { "undock", cmd_undock },
    { "dock", cmd_dock },
    { "wear_up", cmd_wear_up },
    { "wear_down", cmd_wear_down },
    { "enter_pairing", cmd_enter_pairing },
    { "exit_pairing", cmd_exit_pairing },
    { "enable_multipoint", cmd_enable_multipoint },
    { "disable_multipoint", cmd_disable_multipoint },
    { "connect_bt", cmd_connect_bt_device },
    { "connect_lea", cmd_connect_lea_device },
    { "remove_bt", cmd_remove_bt_device },
    { "remove_lea", cmd_remove_lea_device },
    { "remove_bond", cmd_remove_bond },
    { "enable_lea_advertiser", cmd_enable_lea_advertiser },
    { "enable_br_edr_supported", cmd_enable_br_edr_supported },
    { "block_page_when_streaming", cmd_block_page_when_streaming },
    { "without_reconnect_when_fetch_out_wear_up", cmd_without_reconnect_when_fetch_out_wear_up },
#if BLE_AUDIO_ENABLED
    { "bis_source_open", cmd_bis_source_open_a2dp },
    { "bis_source_open", cmd_bis_source_open_linein },
    { "bis_source_open", cmd_bis_source_open_usb },
#endif
#ifdef BIS_SELFSCAN_ENABLED
    { "bis_self_scan_enable", cmd_bis_slef_scan_enable },
#endif
};

static unsigned int cmd_callback(unsigned char *buf, unsigned int len)
{
    TRACE(0, "headset: %s", buf);

    constexpr uint8_t ITEM_NUM = sizeof(cmd_table) / sizeof(cmd_table_item_t);
    int index = 0;

    for (; index < ITEM_NUM; ++index)
    {
        if ((strncmp((char*)buf, cmd_table[index].string, strlen(cmd_table[index].string)) == 0) ||
             strstr(cmd_table[index].string, (char*)buf))
        {
            unsigned int p_len = 0;
            char *p = strstr((char*)buf, "|");
            if(p != NULL)
            {
                ++p;
                p_len = len - (p - (char *)buf);
            }

            cmd_table[index].cmd_function(p, p_len);
            break;
        }
    }

    if (ITEM_NUM == index)
    {
        TRACE(0, "headset: %s not found", __func__);
    }

    return 0;
}

void app_headset_cmd_init()
{
    app_trace_rx_register("headset", cmd_callback);
    app_bta_add_cmd_table(sizeof(cmd_table)/sizeof(cmd_table[0]),
        (bta_cmd_table_t *)cmd_table);
}
