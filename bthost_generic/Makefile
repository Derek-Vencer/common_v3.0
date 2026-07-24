cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

include $(cur_dir)/config.mk

obj-y := host/

ifeq ($(BTHOST_MIDDLEWARE),)
obj-y += apps/
else
ifneq ($(BT_FRAMEWORK),)
obj-y += apps/
endif
endif
