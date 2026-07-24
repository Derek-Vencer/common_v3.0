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
# rpchan
# ==========
ifeq ($(USE_BES_RPCHAN), 1)
    CFLAGS_rpchan.o += -DUSE_BES_RPCHAN

    ifeq ($(DISABLE_ESHELL_RPCHAN), 1)
        CFLAGS_rpchan.o += -DDISABLE_ESHELL_RPCHAN
    endif
endif

