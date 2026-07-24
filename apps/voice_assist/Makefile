cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.S))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ifeq ($(APP_MCPP_CLI),SENS)
obj-y += ../../apps/anc/src/app_anc_table.o
else ifeq ($(APP_MCPP_CLI),CP_SUBSYS)
obj-y += ../../apps/anc/src/app_anc_table.o
endif

subdir-ccflags-y += \
	-Imultimedia/inc \
	-Imultimedia/algorithms/src/speech/src/process/anc_assist/ \
	-Iservices/audio_dump/include \
	-Iinclude/cmsis_dsp \
	$(CODEC_DSP_INCLUDES) \
	-Iplatform/drivers/cp_accel \
	-Iservices/nv_section/aud_section \
	-Iservices/nv_section/include \
	-Iservices/app_ai/inc\
	$(AUDIO_ALGORITHMS_INCLUDES) \
	$(BLUETOOTH_ADAPTER_INCLUDES) \
	-Iapps/anc_assist/src/ \
	-Iapps/anc_assist/inc \
	-Iapps/anc/src\
	-Iapps/anc/inc\
	-Iapps/common \
	-Iapps/audioplayers \
	-Iapps/voice_assist/inc \
	-Iservices/audio_bt \
	-Iservices/mcpp \
	-Iservices/audioflinger \
	-Iutils/kfifo \
	-Iutils/mfifo \
	-Iutils/heap \
	-Iutils/stream_mcps \
	-Iutils/anc_fir_coeff_config \
	-Itests/sensor_hub/core/ \
	${BECO_INCLUDE}
