cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

export SUPPORT_DYNAMIC_MULTIPOINT ?= 0
ifeq ($(SUPPORT_DYNAMIC_MULTIPOINT),1)
KBUILD_CPPFLAGS += -DSUPPORT_DYNAMIC_MULTIPOINT
endif

export HEARABLE_CTRL_ENABLED ?= 0
ifeq ($(HEARABLE_CTRL_ENABLED),1)
KBUILD_CPPFLAGS += -DHEARABLE_CTRL_ENABLED
endif

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.S))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/gfps.cpp))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/gfps_ble.cpp))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/gfps_rfcomm.cpp))

obj_lib_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/gfps_crypto.c))
obj_lib_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/gfps_sass.cpp))
#CRYPTO_LIB_NAME := libgfps_crypto
#$(CRYPTO_LIB_NAME)-y := $(obj_c:.c=.o)

libgfps_crypto-y := $(obj_lib_c:.c=.o) $(obj_lib_cpp:.cpp=.o)
obj-y := libgfps_crypto.a
obj-y += $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

subdir-ccflags-y += \
    -I$(BT_STACK_INCLUDES) \
    -I$(BLE_APP_INCLUDES) \
    $(BT_SERVICE_UX_INCLUDES) \
    $(BLUETOOTH_ADAPTER_INCLUDES) \
    -I$(BLUETOOTH_SERVICE_DIR_PATH)/bt_app/inc \
    -I$(BLUETOOTH_SERVICE_DIR_PATH)/bt_app/inc/a2dp_codecs \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/adapter_service \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/bt/common \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/bt \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/bt/me \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/bt/ibrt \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/bt/l2cap \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/bt/a2dp \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/bt/avrcp \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/bt/hfp \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/bt/dip \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/bt/source \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/ble/common \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/ble \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/ble/gap \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/ble/gatt \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/ble/aob \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/ble/gfps \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/ble/dp \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/ble/ai \
    -Iservices/gfps/inc \
    -Iutils/cqueue \
    -Iutils/encrypt \
    $(BES_MULTIMEDIA_INCLUDES) \
    -Iplatform/drivers/bt \
    -Iapps/key \
    -Iapps/main \
    $(BT_BLE_AUDIO_POLICY_INCLUDES) \
    -Iapps/earbuds/conn \
    -Iservices/audio_manager/inc \
    -Iservices/audio_bt \
    -Iutils/hwtimer_list \
    -Iservices/nv_section/customparam_section \
    -I$(BLE_APP_DIR_PATH)/inc  \
    -Iutils/hsm

ifeq ($(BESUI_TWS_EN),1)
subdir-ccflags-y += -Iapps/besui
ccflags-y += -Iapps/besui/tws_ui
endif
ifeq ($(BESUI_STEREO_EN),1)
subdir-ccflags-y += -Iapps/besui \
		  	        -Iapps/besui/stereo_ui
endif



