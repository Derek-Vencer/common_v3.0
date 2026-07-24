cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ifeq ($(BT_FRAMEWORK),1)
obj-y += framework_apps/watch/
else
obj-y += bta_apps/config/
endif

ifeq ($(BT_SERVICE_ENABLE),1)
obj-y += bta_apps/bta_test/
endif

ifeq ($(BT_FRAMEWORK)$(BTHOST_MIDDLEWARE), )

ifneq (,$(findstring EARBUDS,$(BT_SVC_FW_PRODUCT)))
ifneq ($(wildcard $(cur_dir)bta_apps/earbuds/Makefile),)
obj-y += bta_apps/earbuds/
obj-y += bta_apps/deprecated/
endif

ifeq ($(CUSTOMER_ADAPTER_VV_SUPPORT),1)
obj-y += bta_apps/adapter/earbuds/vv/
endif

ifeq ($(CUSTOMER_ADAPTER_HM_SUPPORT),1)
obj-y += bta_apps/adapter/hm/earbuds/
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

ifneq (,$(findstring SPEAKER,$(BT_SVC_FW_PRODUCT)))
ifneq ($(wildcard $(cur_dir)bta_apps/speaker/Makefile),)
obj-y += bta_apps/speaker/
obj-y += bta_apps/deprecated/
endif
endif

endif #$(BT_FRAMEWORK)$(BTHOST_MIDDLEWARE)
