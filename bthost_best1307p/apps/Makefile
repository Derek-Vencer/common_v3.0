cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y += bta_apps/config/

ifneq (,$(findstring EARBUDS,$(BT_SVC_FW_PRODUCT)))
ifneq ($(wildcard $(cur_dir)bta_apps/earbuds/Makefile),)
obj-y += bta_apps/earbuds/
obj-y += bta_apps/deprecated/
endif

ifeq ($(CUSTOMER_ADAPTER_VV_SUPPORT),1)
obj-y += bta_apps/adapter/earbuds/vv/
endif

endif

ifneq (,$(findstring DONGLE,$(BT_SVC_FW_PRODUCT)))
ifneq ($(wildcard $(cur_dir)bta_apps/dongle/Makefile),)
obj-y += bta_apps/dongle/
endif
endif

ifneq (,$(findstring SOUND,$(BT_SVC_FW_PRODUCT)))
ifneq ($(wildcard $(cur_dir)bta_apps/sound/Makefile),)
obj-y += bta_apps/sound/
endif
endif

ifneq (,$(findstring WIRELESSMIC,$(BT_SVC_FW_PRODUCT)))
ifneq ($(wildcard $(cur_dir)bta_apps/wmic/Makefile),)
obj-y += bta_apps/wmic/
endif
endif

ifneq (,$(findstring HEADSET,$(BT_SVC_FW_PRODUCT)))
ifneq ($(wildcard $(cur_dir)bta_apps/headset/Makefile),)
obj-y += bta_apps/headset/
obj-y += bta_apps/deprecated/
endif
endif

ifneq (,$(findstring GLASSES,$(BT_SVC_FW_PRODUCT)))
obj-y += bta_apps/config/
ifneq ($(wildcard $(cur_dir)bta_apps/glasses/Makefile),)
obj-y += bta_apps/glasses/
obj-y += bta_apps/deprecated/
endif

ifeq ($(CUSTOMER_ADAPTER_VV_SUPPORT),1)
obj-y += bta_apps/adapter/earbuds/vv/
endif

endif

ifneq (,$(findstring BLE_ONLY,$(BT_SVC_FW_PRODUCT)))
ifneq ($(wildcard $(cur_dir)bta_apps/ble_only/Makefile),)
obj-y += bta_apps/ble_only/
endif
endif
