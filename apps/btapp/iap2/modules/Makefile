cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y += iap2_i2c/

ifeq ($(IOS_IAP2_BES_OTA_SUPPORT),1)
obj-y += iap2_ota/
endif

subdir-ccflags-y += \
    -Ibthost/host/inc/bta/bt
