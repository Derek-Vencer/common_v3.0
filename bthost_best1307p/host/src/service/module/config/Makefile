cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

obj-y += $(src_obj)

subdir-ccflags-y += \
    $(BT_BLE_AUDIO_POLICY_INCLUDES) \
    $(BLUETOOTH_ADAPTER_INCLUDES) \
    $(BT_SERVICE_UX_INCLUDES) \
    $(BLE_APP_INCLUDES) \
    -I$(BLE_APP_DIR_PATH)/inc \
    -I$(BLE_GAF_CORE_DIR_PATH)/inc \
    -I$(BLE_AOB_APP_DIR_PATH)/inc \
    -I./platform/drivers/bt \
    -I./platform/drivers \
    -I./services/audio_manager/inc \
    -I./services/audio_bt \
    -I./services/app_ai/inc \
    -I./services/audio_process \
    -I./services/norflash_api \
    -I./apps/audioplayers/a2dp_decoder \
    -I./apps/audioplayers/audio_trigger/version1 \
    -I./apps/common \
    -I./services/ota \
    -I./services/tota_v2 \
    -I./services/ota/ai_ota \
    -I./services/ota/bes_ota/inc \
    -I./apps/bt_sync \
    -I./apps/key \
    -I./services/bt_sync \
    -I./utils/cqueue \
    -I./utils/list

ifeq ($(AI_VOICE),1)
subdir-ccflags-y += \
    -Iservices/ai_voice/manager

ifeq ($(BISTO_ENABLE),1)
subdir-ccflags-y += \
    -Iservices/ai_voice/protocol/gva/gsound_custom/inc \
    -Iservices/ai_voice/protocol/gva/gsound_target \
    -Iservices/ai_voice/protocol/gva/gsound_target_api_read_only
endif
endif

ifeq ($(IOS_IAP2_BLUETOOTH),1)
subdir-ccflags-y += $(BLUETOOTH_IAP2_INCLUDES) \
                    -Ibthost/host/src/service/iap2/itf
endif
