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

#ifdef RPMSG_I2C
/***************************************************************************
 * Header Files
 ****************************************************************************/
#include <string.h>

#include "eshell.h"

#include "open-amp/apps/rpmsg_app.h"

/***************************************************************************
 * Private Functions
 ****************************************************************************/
static const char rpmsg_i2c_helper[] = \
    "[Usage]:" ESHELL_NEW_LINE
    "    rpmsg_i2c init : init rpmsg i2c device" ESHELL_NEW_LINE
    "    rpmsg_i2c deinit: deinit rpmsg i2c device" ESHELL_NEW_LINE;

static int rpmsg_i2c_cmd(eshell_session_t s, int argc, const char *argv[])
{
    if (argc != 2) {
        goto __print_usage;
    }

    if (memcmp(argv[1], "init", strlen("init") + 1) == 0) {
        eshell_session_printf(s, "rpmsg i2c module init..." ESHELL_NEW_LINE);
        return rpmsg_i2c_init();
    } else if (memcmp(argv[1], "deinit", strlen("deinit") + 1) == 0) {
        eshell_session_printf(s, "rpmsg i2c module init..." ESHELL_NEW_LINE);
        return rpmsg_i2c_deinit();
    }

__print_usage:
    eshell_session_write(s, rpmsg_i2c_helper, strlen(rpmsg_i2c_helper));
    return 1;
}

ESHELL_COMMAND_REGISTRY(ESHELL_CMD_GRP_OPENAMP, "rpmsg_i2c",
                        rpmsg_i2c_helper, rpmsg_i2c_cmd);
#endif
