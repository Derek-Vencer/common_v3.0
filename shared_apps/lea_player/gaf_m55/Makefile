cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)../gaf_codec/*.cpp))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)../gaf_cc_stream/cc_stream_common.cpp))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)../gaf_cc_stream/gaf_cc_stream_process.cpp))

src_obj := $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

obj-y := $(src_obj)

GAF_MUSIC_PLC_ON ?= 1
ifeq ($(GAF_LC3_BES_PLC_ON),1)
KBUILD_CPPFLAGS += -DGAF_LC3_BES_PLC_ON
else ifeq ($(GAF_MUSIC_PLC_ON),1)
KBUILD_CPPFLAGS += -DGAF_MUSIC_PLC_ON
endif


subdir-ccflags-y += \
    $(BLE_APP_INCLUDES) \
    -Iservices/overlay \
    -Iservices/mcpp \
    -Iservices/mcpp/thirdparty \
    -Iservices/audio_dump/include \
    -I$(LEA_PLAYER_PATH)/inc \
    -Iutils/kfifo \
    -Iutils/cfifo \
    -Iutils/heap \
    -Iapps/common \
    -Iapps/anc/inc \
    $(AUDIO_ALGORITHMS_INCLUDES) \
    -Imultimedia/smf/inc/elements/ \
    -Iapps/cross_core_common/ \
    -Imultimedia/smf/inc/common/ \
    -Imultimedia/smf/inc/3th/ \
    -Imultimedia/inc/speech/inc/ \
    -Imultimedia/inc/audio/process/audio_ramp/ \
    -I$(BLE_AOB_APP_DIR_PATH)/gaf_audio

ccflags-y += -Iapps/dsp_m55
subdir-ccflags-y += -Imultimedia/inc/
subdir-ccflags-y += -Imultimedia/inc/smf