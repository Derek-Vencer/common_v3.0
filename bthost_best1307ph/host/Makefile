cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

#***********************************
# Git revision
#***********************************
BTH_VERSION := $(shell git -C $(cur_dir) rev-parse HEAD || echo "unknown")
KBUILD_CPPFLAGS += -DBTH_VERSION=\"$(BTH_VERSION)\"

#***********************************
# Build source
#***********************************
ifeq ($(BT_FRAMEWORK), 1)
obj-y += src/framework/
export BTHOST_MIDDLEWARE := 1
endif

ifneq ($(BTHOST_MIDDLEWARE),)
obj-y += src/middleware/
else
obj-y += src/service/
endif

ifneq ($(BTH_IN_ROM), 1)
obj-y += src/stack/
endif
