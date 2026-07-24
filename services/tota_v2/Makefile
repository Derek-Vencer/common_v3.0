cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

GEN_LIB_NAME := libtota
root_dir := $(abspath $(dir $(realpath $(firstword $(MAKEFILE_LIST))))/..)
sinclude $(root_dir)/config/lib.mk
$(GEN_LIB_NAME)-y := $(src_obj)

obj-y += $(GEN_LIB_NAME).a

export BUDSODIN2_TOTA ?= 0
ifeq ($(BUDSODIN2_TOTA),1)
subdir-ccflags-y += -DBUDSODIN2_TOTA
endif
KBUILD_CPPFLAGS += -DSDK_TOTA_CMD

ifeq ($(TOTA_EQ_TUNING), 1)
CFLAGS_app_tota_audio_EQ.o += -DTOTA_EQ_TUNING
endif

ifeq ($(ANC_APP),1)
CFLAGS_app_tota_general.o += -DANC_APP
endif

ifeq ($(APP_DEBUG_TOOL), TOTA)
CFLAGS_app_tota_general.o += -DAPP_DEBUG_TOOL_TOTA
endif

CFLAGS_app_tota_general.o += $(LDS_SECTION_FLAGS)

CFLAGS_app_tota.o += -DBESLIB_INFO=$(BESLIB_INFO)

subdir-ccflags-y += -Iplatform/drivers/ana \
                    -Iplatform/hal \
                    -Iservices/bluedroid/adapter/src \
                    -Iapps/btapp/app_rssi/ \
                    -Iapps/key \
                    -Iservices/audio_bt \
                    -Iservices/audio_eq \
                    -Iservices/fs/fat \
                    -Iservices/fs/sd \
                    -Iservices/fs/fat/ChaN \
                    $(BLUETOOTH_ADAPTER_INCLUDES) \
                    $(BT_SERVICE_UX_INCLUDES) \
                    -Iservices/overlay \
                    -Iservices/resources \
                    $(CODEC_DSP_INCLUDES) \
                    $(AUDIO_ALGORITHMS_INCLUDES) \
                    -Iservices/nv_section/aud_section \
                    -Iservices/nv_section/include \
                    -Iservices/bt_app/a2dp_codecs/include \
                    -Iplatform/drivers/uarthci \
                    -Iplatform/drivers/ana \
                    -Iplatform/drivers/bt \
                    -Iutils/cqueue \
                    -Iutils/heap \
                    -Iutils/retention_ram \
                    -Iservices/audioflinger \
                    -Iutils/lockcqueue \
                    -Iutils/intersyshci \
                    -Iutils/boot_struct \
                    -Iutils \
                    -Iutils/hsm \
                    -Iutils/sha256 \
                    -Iutils/kfifo \
                    -Iapps/main \
                    -Iapps/common \
                    -Iapps/audioplayers \
                    -Iservices/app_ai/inc \
                    -Iapps/factory \
                    -Iservices/bridge/ \
                    -Iservices/bt_app \
                    -Iservices/anc_spp_tool \
                    -Iservices/tws/inc \
                    -Iutils/crc \
                    -Ithirdparty/userapi \
                    -Iapps/battery \
                    -Iservices/nv_section/factory_section \
                    -Iservices/ \
                    $(BT_BLE_AUDIO_POLICY_INCLUDES) \
                    -Iapps/earbuds/conn \
                    -Iapps/ota \
                    -Iservices/ota/bes_ota/inc/ \
                    -Iservices/ota/ \
                    -Iservices/audio_manager/inc \
                    -Iservices/audio_process \
                    -Iapps/anc/inc \
                    -Iapps/anc/src \
                    -Iapps/anc_assist/inc \
                    -Iapps/anc_assist/src \
                    -Iapps/voice_assist/inc \
					-Iservices/nv_section/aud_section \
					-Iservices/nv_section/include \
					-Iservices/norflash_api \
                    -Iapps/capsensor \
                    -Iplatform/drivers/ana/$(CHIP) \
                    -Iplatform/drivers/capsensor/$(CHIP) \
                    -Iplatform/drivers/capsensor/best1306p \
                    -Iservices/ota/bes_ota/inc \
                    -I$(BLE_APP_DIR_PATH)/inc \
                    -I$(BLUETOOTH_BTH_DIR_PATH)/service/ble_app_new/inc \
                    -Iservices/nv_section/aud_section \
                    -Iservices/nv_section/include

ifeq ($(BESUI_APP_EN),1)
subdir-ccflags-y += -Iservices/nv_section/factory_section \
					-Iapps/battery
endif

ifeq ($(BESUI_TWS_EN),1)
subdir-ccflags-y += -Iapps/besui \
                    -Iapps/besui/tws_ui
endif
ifeq ($(BESUI_STEREO_EN),1)
subdir-ccflags-y += -Iapps/besui \
		            -Iapps/besui/stereo_ui
endif

