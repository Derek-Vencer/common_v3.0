cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

obj-y := $(src_obj)

subdir-ccflags-y += \
    $(BLUETOOTH_ADAPTER_INCLUDES) \
    $(BT_SERVICE_UX_INCLUDES) \
    $(BT_BLE_AUDIO_POLICY_INCLUDES) \
    -I./services/audio_manager \
    -I./services/audio_bt \
    -I./services/app_ai/inc \
    -I./services/audio_manager/audio_policy/bredr_policy/inc \
    -I./services/tota_v2 \
    -I./services/bt_sync \
    -I./services/btservice/common/config \
    -I./services/gfps/inc \
    -I./$(LEA_PLAYER_PATH)/inc \
    -I./platform/drivers/bt \
    -I./utils/cqueue \
    -I./utils/hsm \
    -I./utils/list \
    -I./utils/cfifo \
    -I./bthost/service/ble_audio/aob_app/inc \
    -I./apps/audioplayers/a2dp_decoder \
    -I./apps/audioplayers/audio_trigger/version1 \
    -I./apps/earbuds/conn \
    -I./apps/bt_sync \
    -I./apps/key \
    -I./apps/ota

ifeq ($(SPA_AUDIO_ENABLE),1)
subdir-ccflags-y += -Ithirdparty/userapi/spa_dummy_app/inc
ifeq ($(SPA_AUDIO_SEC),1)
ccflags-y += -Ithirdparty/userapi/spa_dummy_app/sec
endif
endif
