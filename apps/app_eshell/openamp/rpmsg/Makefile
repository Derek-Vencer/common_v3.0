###############################################################################
#
#  Copyright 2015-2025 BES.
#  All rights reserved. All unpublished rights reserved.
#
#  No part of this work may be used or reproduced in any form or by any
#  means, or stored in a database or retrieval system, without prior written
#  permission of BES.
#
#  Use of this work is governed by a license granted by BES.
#  This work contains confidential and proprietary information of
#  BES. which is protected by copyright, trade secret,
#  trademark and other intellectual property rights.
#
###############################################################################

cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))

obj-y := $(obj_c:.c=.o)

# ==========
# rpmsg i2c
# ==========
ifeq ($(RPMSG_I2C), 1)
    CFLAGS_rpmsg_i2c.o += -DRPMSG_I2C
endif

# ==============
# rpmsg sysfreq
# ==============
ifeq ($(RPMSG_SYSFREQ), 1)
    CFLAGS_rpmsg_sysfreq.o += -DRPMSG_SYSFREQ
endif
