cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.cpp))

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

obj-y := $(src_obj)

subdir-ccflags-y += \
    $(BLUETOOTH_ADAPTER_INCLUDES) \
    $(BT_BLE_AUDIO_POLICY_INCLUDES) \
    $(CODEC_DSP_INCLUDES) \
    $(BT_SERVICE_UX_INCLUDES) \
    -Iapps/key \
    -Iapps/main \
    -Iapps/common \
    -Iapps/earbuds/conn \
    -Iservices/audio_manager/inc \
    -Iservices/audio_bt \
    -I$(BT_SERVICE_MODULE_DIR_PATH)/module/config \
    -Iservices/nv_section/factory_section \
    -Iplatform/drivers/bt \
    -Itests/anc_usb \
    -Iapps/audioplayers \
    -Iservices/audio_process \
    -Imultimedia/inc/audio/process/filters/include \
    -Iutils/cqueue \
    -Iutils/heap/ \
    -Iutils/hsm

