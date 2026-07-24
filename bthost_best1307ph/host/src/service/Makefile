cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

ifneq ($(wildcard $(cur_dir)../bta/Makefile),)
src_obj += ../bta/
endif

ifneq ($(wildcard $(cur_dir)../adapter/Makefile),)
src_obj += ../adapter/
endif

ifneq ($(wildcard $(cur_dir)../bt_vendor/Makefile),)
src_obj += ../bt_vendor/
endif

ifneq ($(wildcard $(cur_dir)../porting/Makefile),)
src_obj += ../porting/
endif

ifneq ($(wildcard $(cur_dir)../pts/Makefile),)
src_obj += ../pts/
endif

ifeq ($(BTH_IN_ROM),1)
ifneq ($(wildcard $(cur_dir)../rom/Makefile),)
src_obj += ../rom/
endif
endif

ifneq ($(wildcard $(cur_dir)common/Makefile),)
src_obj += common/
endif

ifneq ($(wildcard $(cur_dir)audio_policy/Makefile),)
src_obj += audio_policy/
endif

ifneq ($(BLE_ONLY_ENABLED),1)
ifneq ($(wildcard $(cur_dir)bt_app/Makefile),)
src_obj += bt_app/
endif
ifeq ($(BT_SOURCE),1)
ifneq ($(wildcard $(cur_dir)bt_source/Makefile),)
src_obj += bt_source/
endif
endif
endif

ifeq ($(BLE),1)
ifneq ($(wildcard $(cur_dir)ble_app_new/Makefile),)
src_obj += ble_app_new/
endif
ifeq ($(BLE_AUDIO_ENABLED),1)
ifneq ($(wildcard $(cur_dir)ble_audio/aob_app/Makefile),)
src_obj += ble_audio/aob_app/
endif
ifneq ($(wildcard $(cur_dir)ble_audio/blueelf_adapter/Makefile),)
src_obj += ble_audio/blueelf_adapter/
endif
endif #BLE_AUDIO_ENABLED
endif #BLE

ifeq ($(NEARLINK_HOST_SUPPORT),1)
ifneq ($(wildcard $(cur_dir)near_app/Makefile),)
src_obj += near_app/
endif
endif

ifeq ($(IOS_IAP2_BLUETOOTH),1)
ifneq ($(wildcard $(cur_dir)iap2/Makefile),)
src_obj += iap2/
endif
endif

ifneq ($(filter 1,$(BLE_AUDIO_ENABLED) $(BT_SERVICE_ENABLE)),)
ifneq ($(wildcard $(cur_dir)module/Makefile),)
src_obj += module/
endif
endif

ccflags-y += -DBLUETOOTH_BT_IMPL
ccflags-y += -DBLUETOOTH_BLE_IMPL

BTSERVICE_LIB_NAME := libbt_service

ifneq ($(GEN_LIB_NAME_BT),)
BTSERVICE_LIB_NAME := $(BTSERVICE_LIB_NAME)$(GEN_LIB_NAME_BT)
endif

$(BTSERVICE_LIB_NAME)-y := $(src_obj)
obj-y := $(BTSERVICE_LIB_NAME).a
