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
#include "bthost_logger.h"
#include "app_bta_cmd.h"
#if defined(UTILS_ESHELL_EN)
#include "eshell.h"
#endif

#define APP_BTA_CMD_TABLE_MAX    20

typedef struct
{
    struct
    {
        uint8_t cmd_number;
        const bta_cmd_table_t* table_list;
    } cmd_table[APP_BTA_CMD_TABLE_MAX];
} app_bta_cmd_env_t;

static app_bta_cmd_env_t app_bta_cmd_env = {0};

bool app_bta_scan_bdaddr_from_string(const char* param, uint32_t len, bt_bdaddr_t *out_addr)
{
    int bytes[sizeof(bt_bdaddr_t)] = {0};

    if (len < 17)
    {
        DEBUG_INFO(0, "%s wrong len %d '%s'", __func__, len, param);
        return false;
    }

    DEBUG_INFO(0,  "%s '%s'", __func__,param);


    if (6 != sscanf(param, "%x:%x:%x:%x:%x:%x", bytes+0, bytes+1, bytes+2, bytes+3, bytes+4, bytes+5))
    {
        DEBUG_INFO(0, "%s parse address failed %s sscanf=%d", __func__, param,sscanf(param, "%x:%x:%x:%x:%x:%x", bytes+0, bytes+1, bytes+2, bytes+3, bytes+4, bytes+5));
        return false;
    }

    bt_bdaddr_t addr = {{
        (uint8_t)(bytes[0]&0xff),
        (uint8_t)(bytes[1]&0xff),
        (uint8_t)(bytes[2]&0xff),
        (uint8_t)(bytes[3]&0xff),
        (uint8_t)(bytes[4]&0xff),
        (uint8_t)(bytes[5]&0xff)}};

    *out_addr = addr;

    return true;
}

/**
 * Parses the length of the command string and retrieves the parameter part with its length from the buffer.
 * The command string is the segment before the first '|' delimiter, and the parameter is the segment after.
 *
 * @param buf        Pointer to the input buffer containing the command and potential parameter
 * @param buf_len    Total length of the input buffer in bytes
 * @param param      Output pointer to store the address of the parameter part (after '|')
 * @param param_len  Output pointer to store the length of the parameter part
 *
 * @return Length of the command string (before the first '|').
 *         Returns 0 if input is invalid (NULL buffer or zero length).
 *         If no '|' is found, returns total buffer length and leaves param/param_len unchanged.
 *
 * Note: When '|' is found, param_len includes all bytes from the position after '|' to the end of the buffer.
 */
static unsigned int parse_cmd_str_length_and_param_from_buf(const char *buf, unsigned int buf_len, char **param, unsigned int *param_len)
{
    // Check for invalid input (NULL buffer or zero length)
    if (buf == NULL || buf_len == 0)
    {
        DEBUG_INFO(2, "%s: input command buf is empty(NULL).", __func__);
        return 0;
    }

    // Default length is the entire buffer if no delimiter is found
    unsigned int cmd_str_len = buf_len;
    // Find the first occurrence of the '|' delimiter
    char *sep_ptr = strstr(buf, "|");
    // If delimiter found, update command length and set parameter pointer
    if (sep_ptr != NULL)
    {
        cmd_str_len = sep_ptr - buf;
        *param = sep_ptr + 1;
        *param_len = buf_len - cmd_str_len - 1; // Subtract 1 to exclude the '|' itself from parameter length
    }
    return cmd_str_len;
}

unsigned int app_bta_cmd_callback(unsigned char *buf, unsigned int length)
{
    // Check len
    DEBUG_INFO(0, "[%s] cmd: %s", __func__, buf);
    uint8_t cmd_number = 0;
    int cmd_count = 0;
    char *para_addr=NULL;
    unsigned int para_len=0;
    const bta_cmd_table_t* cmd_table;
    unsigned int cmd_str_len = 0;
    cmd_str_len = parse_cmd_str_length_and_param_from_buf((char *)buf, length, &para_addr, &para_len); // the length of the command string
    if (cmd_str_len == 0)
    {
        DEBUG_INFO(2,"%s: invalid cmd! command string is not exist in buf", __func__);
        return 0;
    }

    for (int tab_num=0; tab_num<APP_BTA_CMD_TABLE_MAX; tab_num++)
    {
        if (!app_bta_cmd_env.cmd_table[tab_num].table_list)
        {
            continue;
        }

        cmd_number = app_bta_cmd_env.cmd_table[tab_num].cmd_number;
        cmd_table  = app_bta_cmd_env.cmd_table[tab_num].table_list;
        for (cmd_count = 0; cmd_count < cmd_number; cmd_count++)
        {
            if (((cmd_str_len == strlen(cmd_table[cmd_count].string)) && (strncmp((char*)buf, cmd_table[cmd_count].string, cmd_str_len) == 0)) ||
                    strstr(cmd_table[cmd_count].string, (char*)buf))
            {
                if (para_addr)
                {
                    DEBUG_INFO(2,"%s: cmd string: %s, param string: %s", __func__, cmd_table[cmd_count].string, para_addr);
                }
                cmd_table[cmd_count].cmd_function(para_addr, para_len);
                break;
            }
        }

        if (cmd_count < cmd_number)
        {
            break;
        }
    }

    if (cmd_count >= cmd_number)
    {
        DEBUG_INFO(0,"bt host not found,cmd=%s", buf);
    }

    return 0;
}

bool app_bta_add_cmd_table(uint8_t cmd_number, const bta_cmd_table_t *cmd_table)
{
#if defined(APP_TRACE_RX_ENABLE) || defined(UTILS_ESHELL_EN)
    for (int i=0; i<APP_BTA_CMD_TABLE_MAX; ++i)
    {
        if (!app_bta_cmd_env.cmd_table[i].table_list)
        {
            app_bta_cmd_env.cmd_table[i].cmd_number = cmd_number;
            app_bta_cmd_env.cmd_table[i].table_list = cmd_table;
            return true;
        }
    }

    DEBUG_INFO(0, "bt host add add cmd table fail! table=%p", cmd_table);
    return false;
#else
    DEBUG_INFO(0, "bt host cmd add table, not open APP_TRACE_RX_ENABLE");
    return false;
#endif
}

bool app_bta_delete_cmd_table(const bta_cmd_table_t* cmd_table)
{
#if defined(APP_TRACE_RX_ENABLE) || defined(UTILS_ESHELL_EN)
    for (int i=0; i<APP_BTA_CMD_TABLE_MAX; ++i)
    {
        if (app_bta_cmd_env.cmd_table[i].table_list == cmd_table)
        {
            app_bta_cmd_env.cmd_table[i].cmd_number = 0;
            app_bta_cmd_env.cmd_table[i].table_list = NULL;
            return true;
        }
    }

    DEBUG_INFO(0, "bt host delete cmd table fail! table=%p", cmd_table);
    return false;
#else
    DEBUG_INFO(0, "bt host cmd delete table, not open APP_TRACE_RX_ENABLE");
    return false;
#endif
}

void app_bta_cmd_init(void)
{
    // app_bta_bt_cmd_init();
}

#if defined(UTILS_ESHELL_EN)

const static char app_bta_cmd_helper[] =     \
    "BTA_TEST usage:\r\n"                        \
    "   BTA_TEST <cmd>\r\n";

void app_bta_cmd_usage(void)
{
    eshell_putstring("%s", app_bta_cmd_helper);
}

void app_bta_cmd_entry(int argc, char *argv[])
{
    char *cmd_name = NULL;
    char cmd_buffer[128] = {0};
    size_t cmd_buffer_offset = 0;
    eshell_putstring("argc %d %s",argc,argv[1]);

    if (argc < 2) {
        app_bta_cmd_usage();
        return;
    }

    cmd_name = argv[1];
    cmd_buffer_offset += sprintf(&cmd_buffer[cmd_buffer_offset], "%s|", cmd_name);

    if (argc > 2) {
        for (int arg_index = 2; arg_index < argc; arg_index++) {
            cmd_buffer_offset += sprintf(&cmd_buffer[cmd_buffer_offset], "%s ", argv[arg_index]);
        }
    }

    app_bta_cmd_callback((unsigned char *)cmd_buffer, cmd_buffer_offset-1);

    return;
}

ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_BT_COMMON, "BTA_TEST", app_bta_cmd_helper, app_bta_cmd_entry);
#endif
