cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

include $(cur_dir)/config.mk

obj-y := host/

ifneq ($(BT_SERVICE_ENABLE),1)
export BT_SVC_FW_PRODUCT ?= BT_SVC_FW_PRODUCT_BLE_ONLY
export BT_SVC_FW_ENABLED := 1
endif

obj-y += apps/
