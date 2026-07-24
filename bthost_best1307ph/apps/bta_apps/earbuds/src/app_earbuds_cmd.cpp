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

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "app_bta_cmd.h"

#include "hal_trace.h"

#ifdef BIS_SELFSCAN_ENABLED
#include "app_bis_selfscan.h"
#endif

#ifndef TRACE
#define TRACE(attr, str, ...)               TR_INFO(attr, str, ##__VA_ARGS__)
#endif

POSSIBLY_UNUSED static void str2hex(const char *src_str, uint8_t *dest_buf, uint8_t hex_number)
{
    uint8_t i;
    uint8_t lb, hb;

    for (i = 0; i < hex_number; i++)
    {
        lb = src_str[(i << 1) + 1];
        hb = src_str[i << 1];

        if (hb >= '0' && hb <= '9')
        {
            dest_buf[i] = hb - '0';
        }
        else if (hb >= 'A' && hb < 'G')
        {
            dest_buf[i] = hb - 'A' + 10;
        }
        else if (hb >= 'a' && hb < 'g')
        {
            dest_buf[i] = hb - 'a' + 10;
        }
        else
        {
            dest_buf[i] = 0;
        }

        dest_buf[i] <<= 4;

        if (lb >= '0' && lb <= '9')
        {
            dest_buf[i] += lb - '0';
        }
        else if (lb >= 'A' && lb < 'G')
        {
            dest_buf[i] += lb - 'A' + 10;
        }
        else if (lb >= 'a' && lb < 'g')
        {
            dest_buf[i] += lb - 'a' + 10;
        }
    }
}

#ifdef BIS_SELFSCAN_ENABLED
static void cmd_bis_self_scan_enable(const char *p, uint32_t p_len)
{
    TRACE(0, "LEA BIS self scan enable");
    app_bis_selfscan_enable_scan(true);
}

static void cmd_bis_self_scan_disable(const char *p, uint32_t p_len)
{
    TRACE(0, "LEA BIS self scan disable");
    app_bis_selfscan_enable_scan(false);
}

static void cmd_bis_self_scan_sync(const char *p, uint32_t p_len)
{
    uint8_t src_lid = 0;
    uint8_t bcast_code[16] = {0};
    str2hex(p, &src_lid, 1);
    str2hex(p + 2, bcast_code, ((p_len - 2) / 2));

    TRACE(0, "LEA BIS self scan sync %d", src_lid);
    DUMP8("%x ", bcast_code, 16);

    app_bis_selfscan_select_sync_source(src_lid, bcast_code);
}

static void cmd_bis_self_scan_stop_sync(const char *p, uint32_t p_len)
{
    TRACE(0, "LEA BIS self scan stop sync");
    app_bis_selfscan_stop_sync_source();
}

#endif

static const bta_cmd_table_t cmd_table[] =
{
#ifdef BIS_SELFSCAN_ENABLED
    { "bis_self_scan_enable", cmd_bis_self_scan_enable },
    { "bis_self_scan_disable", cmd_bis_self_scan_disable },
    { "bis_self_scan_sync", cmd_bis_self_scan_sync },
    { "bis_self_scan_stop", cmd_bis_self_scan_stop_sync },
#endif
};

void app_earbuds_cmd_init(void)
{
    app_bta_add_cmd_table(sizeof(cmd_table) / sizeof(cmd_table[0]), cmd_table);
}

void app_earbuds_cmd_deinit(void)
{
    app_bta_delete_cmd_table(cmd_table);
}
