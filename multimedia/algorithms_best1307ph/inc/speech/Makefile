cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

ifeq ($(SPEECH_TX_AEC_CODEC_REF),1)
CFLAGS_speech_cfg_default.o += -DSPEECH_TX_AEC_CODEC_REF
endif

ifeq ($(SPEECH_RX_COMPEXP),1)
CFLAGS_speech_cfg_default.o += -DSPEECH_RX_COMPEXP
endif

ifeq ($(SPEECH_RX_HW_EQ),1)
CFLAGS_speech_cfg_default.o += -DSPEECH_RX_HW_EQ
endif

ifeq ($(SPEECH_RX_DAC_EQ),1)
CFLAGS_speech_cfg_default.o += -DSPEECH_RX_DAC_EQ
endif

ccflags-y := \
    -Iapps/audioplayers \
    -Imultimedia/algorithms/inc/speech/inc
