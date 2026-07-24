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

#ifdef RPMSG_SYSFREQ
/***************************************************************************
 * Header Files
 ****************************************************************************/
#include <string.h>

#include "eshell.h"

#include "open-amp/apps/rpmsg_app.h"

/***************************************************************************
 * Private Functions
 ****************************************************************************/
static const char rpmsg_sysfreq_helper[] = \
    "[Usage]:" ESHELL_NEW_LINE
    "    rpmsg_sysfreq init : init rpmsg sysfreq" ESHELL_NEW_LINE
    "    rpmsg_sysfreq deinit: deinit rpmsg sysfreq" ESHELL_NEW_LINE;

static const char rpmsg_sysfreq_init_info[] = \
    "rpmsg sysfreq module init...\r\n" ESHELL_NEW_LINE;

static const char rpmsg_sysfreq_deinit_info[] = \
    "rpmsg sysfreq module deinit...\r\n" ESHELL_NEW_LINE;

static int rpmsg_sysfreq_cmd(eshell_session_t s, int argc, const char *argv[])
{
    if (argc != 2) {
        goto _print_usage;
    }

    if (memcmp(argv[1], "init", strlen("init") + 1) == 0) {
        eshell_session_write(s, rpmsg_sysfreq_init_info, strlen(rpmsg_sysfreq_init_info));
        return rpmsg_sysfreq_init();
    } else if (memcmp(argv[1], "deinit", strlen("deinit") + 1) == 0) {
        eshell_session_write(s, rpmsg_sysfreq_deinit_info, strlen(rpmsg_sysfreq_deinit_info));
        return rpmsg_sysfreq_deinit();
    }

_print_usage:
    eshell_session_write(s, rpmsg_sysfreq_helper, strlen(rpmsg_sysfreq_helper));
    return 1;
}

ESHELL_COMMAND_REGISTRY(ESHELL_CMD_GRP_OPENAMP, "rpmsg_sysfreq",
                        rpmsg_sysfreq_helper, rpmsg_sysfreq_cmd);
#endif
