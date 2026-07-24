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

#if defined(USE_BES_RPCHAN) && !defined(DISABLE_ESHELL_RPCHAN)
/***************************************************************************
 * Header Files
 ****************************************************************************/
#include <string.h>

#include "cmsis_os.h"
#include "eshell.h"
#include "open-amp/rpchan/rpchan.h"
#include "open-amp/rptun.h"

/***************************************************************************
 * Private Datas
 ****************************************************************************/
/**
 *  _____    ____            _
 *  |  _  \  |  _ \   ___   | |               _  ___
 *  | |_| |  | |_| | / _ \  | |___   ___     | |/__ \
 *  |     /  | ___/ / / \_\ |  _  \ /   \    |  /  \ \
 *  | |\ \   | |    | |  __ | / | | | n  \   | |   | |
 *  | | \ \  | |    | \_/ / | | | | | u   \  | |   | |
 *  |_|  \_\ |_|     \___/  |_| |_| \___/\_\ |_|   |_|
 */
static const char rpchan_logo[] =                                       \
" _____    ____           _\r\n"                                        \
"|  _  \\  |  _ \\   ___   | |               _  ___\r\n"                \
"| |_| |  | |_| | / _ \\  | |___   ___     | |/__ \\\r\n"               \
"|     /  | ___/ / / \\_\\ |  _  \\ /   \\    |  /  \\ \\\r\n"          \
"| |\\ \\   | |    | |  __ | / | | | n  \\   | |   | |\r\n"             \
"| | \\ \\  | |    | \\_/ / | | | | | u   \\  | |   | |\r\n"            \
"|_|  \\_\\ |_|     \\___/  |_| |_| \\___/\\_\\ |_|   |_|\r\n";


static const char rpchan_helper[] =                                     \
        "rpchan <command> [core_name]\r\n"                              \
        "  These are common rpchan control commands:\r\n"               \
        "    start   start the rpmsg channel with specified core\r\n"   \
        "    stop    stop the rpmsg channel with specified core\r\n"    \
        "    reboot  reboot the rpmsg channel with specified core\r\n"  \
        "    panic   panic will triger specified core crash\r\n"        \
        "  These are some rpchan message commands:\r\n"                 \
        "    ping    ping remote core modify times\r\n"                 \
        "            e.g.: ping mcuc0 3\r\n"                            \
        "  You can get status by following commands:\r\n"               \
        "    status  list core's status\r\n";

/***************************************************************************
 * Private Functions
 ****************************************************************************/
static void rpchan_help(eshell_session_t session)
{
    eshell_session_printf(session, "%s", rpchan_logo);
    eshell_session_printf(session, "RPtun version: v%s\r\n", rptun_get_version_str());
    eshell_session_printf(session, "%s", rpchan_helper);
}

static int rpchan_cmd_start(eshell_session_t session, int argc, char *argv[])
{
    int index = 0;
    int status = 0;
    char *core_name = NULL;

    if (argc == 0) {
        if (rpchan_start_all()) {
            return 1;
        } else {
            return 0;
        }
    }

    for (; index < argc; index++) {
        core_name = argv[index];
        status = rpchan_start_by_name(core_name);
        if (status) {
            eshell_session_printf(session, "start %s failed, status/0x%x", core_name, status);
            return 1;
        }
    }
    return 0;
}

static int rpchan_cmd_stop(eshell_session_t session, int argc, char *argv[])
{
    int index = 0;
    int status = 0;
    char *core_name = NULL;

    if (argc == 0) {
        if (rpchan_stop_all()) {
            return 1;
        } else {
            return 0;
        }
    }

    for (; index < argc; index++) {
        core_name = argv[index];
        status = rpchan_stop_by_name(core_name);
        if (status) {
            eshell_session_printf(session, "stop %s failed, status/0x%x", core_name, status);
            return 1;
        }
    }
    return 0;
}

static int rpchan_cmd_panic(eshell_session_t session, int argc, char *argv[])
{
    int index = 0;
    int status = 0;
    char *core_name = NULL;

    if (argc == 0) {
        if (rpchan_panic_all()) {
            return 1;
        } else {
            return 0;
        }
    }

    for (; index < argc; index++) {
        core_name = argv[index];
        status = rpchan_panic_by_name(core_name);
        if (status) {
            eshell_session_printf(session, "panic %s failed, status/0x%x", core_name, status);
            return 1;
        }
    }
    return 0;
}

static int rpchan_cmd_reboot(eshell_session_t session, int argc, char *argv[])
{
    int index = 0;
    int status = 0;
    char *core_name = NULL;

    if (argc == 0) {
        eshell_session_printf(session, "reboot not support all");
        return 1;
    }

    for (; index < argc; index++) {
        core_name = argv[index];
        status = rpchan_stop_by_name(core_name);
        if (status) {
            eshell_session_printf(session, "reboot %s failed, status/0x%x", core_name, status);
            return 1;
        }
        status = rpchan_start_by_name(core_name);
        if (status) {
            eshell_session_printf(session, "reboot %s failed, status/0x%x", core_name, status);
            return 1;
        }
    }
    return 0;
}

static int rpchan_cmd_ping(eshell_session_t session, int argc, char *argv[])
{
    int status = 0;
    char *core_name = NULL;
    struct rptun_ping_s ping_config = {
        .len = 0,
        .ack = true,
        .sleep = 1,
        .times = 6,
    };

    if (argc == 0) {
        eshell_session_printf(session, "ping not support all\n");
        return 1;
    }

    core_name = argv[0];
    if (argc == 1) {
        status = rptun_ping_sync(core_name, &ping_config);
    } else if (argc == 2) {
        ping_config.times = atoi(argv[1]);
        status = rptun_ping_sync(core_name, &ping_config);
    } else {
        eshell_session_printf(session, "Error Command: see `rpchan help`\n");
        return 1;
    }

    if (status) {
        return 1;
    }

    return 0;
}

static int rpchan_cmd_status(eshell_session_t session, int argc, char *argv[])
{
    int core_count = 0;
    int unique_id = 0;
    char *core_list = NULL;
    const char *local_core_name = NULL;
    const char *remote_core_name = NULL;
    size_t core_list_size = 0;
    RPCHAN_CORE_ID_T core_id = 0;
    RPCHAN_STATUS_T chan_status = 0;
    rpchan_get_core_count(&core_count);

    core_list_size = core_count * RPCHAN_NAME_LENGTH;
    core_list      = malloc(core_list_size);
    if (core_list == NULL) {
        eshell_session_printf(session, "system memory not enough");
        return 1;
    }
    memset(core_list, 0, core_list_size);

    rpchan_get_core_list(core_list, core_list_size);
    eshell_session_printf(session, "=================================================\n");
    eshell_session_printf(session, "  CID\t|UID\t|Name    |Status\n");
    for (int index = 0, offset = 0; index < core_count; index++) {
        remote_core_name = &core_list[offset];
        rpchan_get_core_id(remote_core_name, &core_id, true);
        rpchan_get_status(core_id, &chan_status);
        rpchan_get_core_name(core_id, &local_core_name, false);
        rpchan_get_unique_id(core_id, &unique_id);
        eshell_session_printf(session, "-------------------------------------------------\n");
        eshell_session_printf(session, "  %d\t|%d\t|%-8s|%s%s%s\n", core_id, unique_id,
                              remote_core_name, local_core_name,
                              ((chan_status == RPCHAN_STATUS_START)?"=====":"==x=="),
                              &core_list[offset]);
        offset += RPCHAN_NAME_LENGTH;
    }
    free(core_list);

    return 0;
}

static int rpchan_cmd(eshell_session_t session, int argc, const char *argv[])
{
    const char *action = NULL;

    if (argc < 2) {
        rpchan_help(session);
        return 1;
    }

    action = argv[1];

    if ((strcmp("start", action)) == 0) {
        return rpchan_cmd_start(session, argc - 2, (char **)&argv[2]);
    } else if ((strcmp("stop", action)) == 0) {
        return rpchan_cmd_stop(session, argc - 2, (char **)&argv[2]);
    } else if ((strcmp("reboot", action)) == 0) {
        return rpchan_cmd_reboot(session, argc - 2, (char **)&argv[2]);
    } else if ((strcmp("panic", action)) == 0) {
        return rpchan_cmd_panic(session, argc - 2, (char **)&argv[2]);
    } else if ((strcmp("ping", action)) == 0) {
        return rpchan_cmd_ping(session, argc - 2, (char **)&argv[2]);
    } else if ((strcmp("status", action)) == 0){
        return rpchan_cmd_status(session, argc - 2, (char **)&argv[2]);
    } else if ((strcmp("help", action)) == 0){
        rpchan_help(session);
        return 0;
    }

    eshell_session_printf(session, "Error Command, use `rpchan help` get command list");
    return 2;
}

ESHELL_COMMAND_REGISTRY(ESHELL_CMD_GRP_OPENAMP, "rpchan", rpchan_helper, rpchan_cmd);
#endif
