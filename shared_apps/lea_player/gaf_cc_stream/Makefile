cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

src_obj := $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

obj-y := $(src_obj)

subdir-ccflags-y += \
    $(BLE_APP_INCLUDES) \
    -Iservices/overlay \
    -Iservices/mcpp \
    -Iservices/mcpp/thirdparty \
    -Iservices/audio_dump/include \
    -Iutils/kfifo \
    -Iutils/cfifo \
    -Iutils/heap \
    -Iapps/common \
    -Iapps/anc/inc \
    $(AUDIO_ALGORITHMS_INCLUDES) \
    -Imultimedia/inc/audio/smf/core/inc \
    -Imultimedia/inc/audio/smf/core/include \
    -Iapps/app_rpc/ \
    -Iapps/app_rpc/rpc_m55_dsp/ \
    -Iapps/app_rpc/rpc_bth_dsp/ \
    -I$(BLE_AOB_APP_DIR_PATH)/gaf_audio
