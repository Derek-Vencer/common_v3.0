cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))

obj-y := $(obj_c:.c=.o)

ifeq ($(SENSOR_HUB),1)
ccflags-y += -DSENSOR_HUB
endif

subdir-ccflags-y += \
    -Iapps/app_eshell \
    -Iplatform/drivers/ana \
    -Iplatform/drivers/ana/$(CHIP) \
    -Iplatform/hal \
    -Itests/besair_platform/bsp_tester/coremark \
    -Iutils/boot_struct

ifeq ($(BTH_IF_COMM_RPC_EN),1)
subdir-ccflags-y += \
    -Iapps/bth_if \
    -Iapps/bth_if/bt_if_client
endif
