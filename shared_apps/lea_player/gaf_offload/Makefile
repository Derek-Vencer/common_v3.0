cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ccflags-y += \
    $(BT_STACK_INCLUDES) \
    -Iapps/common \
    -Iutils/cqueue \
    -Iutils/heap \
    -Imultimedia/inc \
    -Iplatform/drivers/bt \
    -Itests/anc_usb \
    -Iservices/audio_process/ \
    -I$(LEA_PLAYER_PATH)/inc/ \
    -Iplatform/drivers/cp_accel/ \
    -Iplatform/drivers/codec/ \
    -Iutils/cfifo/ \
    -Iutils/heap/ \
    -Iplatform/drivers/ana/ \
    $(BLUETOOTH_ADAPTER_INCLUDES) \
    $(BT_SERVICE_UX_INCLUDES) \
    -Iapps/audioplayers/ \
    -Iservices/audio_bt/ \
	-Iservices/audio_process \
    -Iplatform/hal/best1502p

ccflags-y += \
	-Iapps/wireless_mic/algo/tx/ \
    -Imultimedia/inc/audio/process/filters/include \
    -Imultimedia/inc/speech/inc \
    -Imultimedia/inc/audio/process/hdr/include \
    -Imultimedia/inc/speech/inc \
    -Imultimedia/inc/audio/process/sw_gain/include \
    -Imultimedia/inc/audio/process/multipoint_drc/include \
    -Imultimedia/inc/audio/process/audio_channel_merge/include \
	-Imultimedia/inc/audio/process/musicplc \
    -Iservices/audio_dump/include \
    -Iapps/wireless_mic/rx/usb_new/

ifeq ($(AUDIO_RESAMPLE),1)
CFLAGS_gaf_offload_stream_cfg.o += -D__AUDIO_RESAMPLE__
endif