cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.cpp))

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

obj-y := $(src_obj)

subdir-ccflags-y += \
    $(BLUETOOTH_ADAPTER_INCLUDES) \
    $(BT_BLE_AUDIO_POLICY_INCLUDES) \
    $(BT_SERVICE_UX_INCLUDES) \
    $(BTA_LEGACY_INCLUDES) \
    -I$(BLE_APP_DIR_PATH)/inc \
    -I./services/audio_manager/inc \
    -I./services/audio_bt \
    -I./services/app_ai/inc \
    -I./services/tota_v2 \
    -I./services/bt_sync \
    -Iservices/nv_section/factory_section \
    -I$(BT_SERVICE_MODULE_DIR_PATH)/module/config \
    -I./services/gfps/inc \
    -I./$(LEA_PLAYER_PATH)/inc \
    -I./platform/drivers/bt \
    -I./utils/cqueue \
    -I./utils/hsm \
    -I./utils/list \
    -I./utils/cfifo \
    -Iutils/heap/ \
    -I./bthost/host/src/service/ble_audio/aob_app/inc \
    -I./apps/audioplayers/a2dp_decoder \
    -I./apps/audioplayers/audio_trigger/version1 \
    -I./apps/earbuds/conn \
    -Iapps/common \
    -I./apps/bt_sync \
    -I./apps/key

ifeq ($(SPA_AUDIO_ENABLE),1)
subdir-ccflags-y += -Ithirdparty/userapi/spa_dummy_app/inc
ifeq ($(SPA_AUDIO_SEC),1)
ccflags-y += -Ithirdparty/userapi/spa_dummy_app/sec
endif
endif

ifeq ($(IOS_IAP2_BLUETOOTH),1)
subdir-ccflags-y += $(BLUETOOTH_IAP2_INCLUDES)
endif

ifeq ($(DUAL_MIC_RECORDING), 1)
subdir-ccflags-y += -I./services/ai_voice/manager \
                    -I./services/ai_voice/protocol/dual_mic_recording/voice_manager
endif

ifeq ($(ENHANCED_TWS_UI_ENABLED), 1)
subdir-ccflags-y += -Ibthost/host/src/bta/tws/enhanced/tws_ui
endif
