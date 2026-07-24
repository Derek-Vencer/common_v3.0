cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

include $(cur_dir)/info.mk

ccflags-y += -DBLUETOOTH_BT_IMPL
ccflags-y += -DBLUETOOTH_BLE_IMPL
subdir-ccflags-y += -DMODULE=STACK

export BTH_STACK_INCLUDES += \
    $(BT_STACK_INCLUDES)  \
    $(BLE_STACK_INCLUDES) \
    -I$(BLUETOOTH_STACK_DIR_PATH)/ble_stack_new/inc \
    -I$(BLUETOOTH_STACK_DIR_PATH)/common

ifeq ($(BTH_VERSION),)
BTH_VERSION := $(shell git -C $(cur_dir) rev-parse HEAD || echo "unknown")
KBUILD_CPPFLAGS += -DBTH_VERSION=\"$(BTH_VERSION)\"
endif

export BT_STACK_LOG_DISABLE ?= 0
ifeq ($(BT_STACK_LOG_DISABLE),1)
KBUILD_CPPFLAGS += -DBT_STACK_LOG_DISABLE
endif

ifneq ($(wildcard $(cur_dir)/transport/Makefile),)
src_obj += transport/
endif

ifneq ($(wildcard $(cur_dir)/common/Makefile),)
src_obj += common/
endif

ifneq ($(BLE_ONLY_ENABLED),1)
ifneq ($(wildcard $(cur_dir)/bt_if/Makefile),)
src_obj += bt_if/
endif
ifneq ($(wildcard $(cur_dir)/bt_profiles/Makefile),)
src_obj += bt_profiles/
endif
endif

ifeq ($(NEARLINK_HOST_SUPPORT),1)
ifneq ($(wildcard $(cur_dir)/nearlink/Makefile),)
src_obj += nearlink/
endif
endif

ifeq ($(BLE),1)
ifneq ($(wildcard $(cur_dir)/ble_stack_new/Makefile),)
src_obj += ble_stack_new/
endif
endif

ifeq ($(BUILD_BTH_ROM),1)
ifneq ($(wildcard $(cur_dir)/rom/Makefile),)
src_obj += rom/
endif
endif

BTHOST_LIB_NAME := libbt_stack

ifneq ($(GEN_LIB_NAME_BT),)
BTHOST_LIB_NAME := $(BTHOST_LIB_NAME)$(GEN_LIB_NAME_BT)
endif

$(BTHOST_LIB_NAME)-y := $(src_obj)
obj-y := $(BTHOST_LIB_NAME).a
