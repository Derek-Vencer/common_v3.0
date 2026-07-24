
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj_c := $(filter-out audio_test_debug_tool.c,$(obj_c))

ifeq ($(APP_DEBUG_TOOL),TOTA)
obj_c += audio_test_debug_tool.c
CFLAGS_audio_test_cmd.o += -DAPP_DEBUG_TOOL_TOTA
endif

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

subdir-ccflags-y += \
    -Iservices/audio_bt \
    -Iservices/bone_sensor \
    $(BLUETOOTH_ADAPTER_INCLUDES) \
    -Iservices/resources \
    -Iservices/overlay \
    -Iutils/cqueue \
    -Iservices/mcpp \
	-Imultimedia/inc \
    $(AUDIO_ALGORITHMS_INCLUDES) \
    $(CODEC_DSP_INCLUDES) \
    -Iservices/audio_process \
    -Iutils/kfifo \
    -Iapps/anc/inc \
    -Iapps/anc/src \
    -Iapps/anc_assist/inc \
    -Iapps/anc_assist/src \
    -Iservices/nv_section/aud_section \
    -Iservices/nv_section/include \
    ${BECO_INCLUDE}


ifeq ($(AUDIO_DYNAMIC_BOOST),1)
CFLAGS_audio_test_cmd.o += -D__AUDIO_DYNAMIC_BOOST__
endif

ifeq ($(A2DP_KARAOKE),1)
CFLAGS_audio_test_cmd.o += -DA2DP_KARAOKE
endif

ifeq ($(ANC_APP),1)
CFLAGS_audio_test_cmd.o += -DANC_APP
CFLAGS_audio_test_debug_tool.o += -DANC_APP
endif

ifeq ($(HW_FIR_EQ_PROCESS),1)
CFLAGS_audio_test_cmd.o += -D__HW_FIR_EQ_PROCESS__
CFLAGS_audio_test_debug_tool.o += -D__HW_FIR_EQ_PROCESS__
endif

ifeq ($(HW_IIR_EQ_PROCESS),1)
CFLAGS_audio_test_cmd.o += -D__HW_IIR_EQ_PROCESS__
CFLAGS_audio_test_debug_tool.o += -D__HW_IIR_EQ_PROCESS__
endif

ifeq ($(SW_IIR_EQ_PROCESS),1)
CFLAGS_audio_test_cmd.o += -D__SW_IIR_EQ_PROCESS__
CFLAGS_audio_test_debug_tool.o += -D__SW_IIR_EQ_PROCESS__
endif

ifeq ($(HW_DAC_IIR_EQ_PROCESS),1)
CFLAGS_audio_test_cmd.o += -D__HW_DAC_IIR_EQ_PROCESS__
CFLAGS_audio_test_debug_tool.o += -D__HW_DAC_IIR_EQ_PROCESS__
endif

ifeq ($(IIR_EQ_PROCESS_LR_2CH),1)
CFLAGS_audio_test_cmd.o += -D__IIR_EQ_PROCESS_LR_2CH__
CFLAGS_audio_test_debug_tool.o += -D__IIR_EQ_PROCESS_LR_2CH__
endif

ifeq ($(HW_DAC_IIR_LIMITER),1)
CFLAGS_audio_test_cmd.o += -D__HW_DAC_IIR_LIMITER__
CFLAGS_audio_test_debug_tool.o += -D__HW_DAC_IIR_LIMITER__
endif

ifeq ($(AUDIO_DYNAMIC_EQ),1)
CFLAGS_audio_test_cmd.o += -D__AUDIO_DYNAMIC_EQ__
CFLAGS_audio_test_debug_tool.o += -D__AUDIO_DYNAMIC_EQ__
endif

ifeq ($(HW_DAC_DRC),1)
CFLAGS_audio_test_cmd.o += -D__HW_DAC_DRC__
CFLAGS_audio_test_debug_tool.o += -D__HW_DAC_DRC__
endif

ifeq ($(AUDIO_DRC),1)
CFLAGS_audio_test_cmd.o += -D__AUDIO_DRC__
CFLAGS_audio_test_debug_tool.o += -D__AUDIO_DRC__
endif

ifeq ($(AUDIO_LIMITER),1)
CFLAGS_audio_test_cmd.o += -D__AUDIO_LIMITER__
CFLAGS_audio_test_debug_tool.o += -D__AUDIO_LIMITER__
endif

ifeq ($(AUDIO_DYNAMIC_EQ),1)
CFLAGS_audio_test_cmd.o += -D__AUDIO_DYNAMIC_EQ__
CFLAGS_audio_test_debug_tool.o += -D__AUDIO_DYNAMIC_EQ__
endif

ifeq ($(AUDIO_BASS_ENHANCER),1)
ccflags-y += -D__AUDIO_BASS_ENHANCER__
endif
