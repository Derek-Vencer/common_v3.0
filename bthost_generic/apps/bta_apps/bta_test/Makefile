cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*/*.c))

obj-y := $(obj_c:.c=.o)

subdir-ccflags-y += -DMODULE=SERVICE

ccflags-y += \
	-Iinclude/cmsis_dsp \
	-Iutils/heap \
	-Iutils/eshell/include \
	$(BLUETOOTH_ADAPTER_INCLUDES) \
	$(BTH_STACK_INCLUDES) \
	$(BT_SERVICE_UX_INCLUDES)
