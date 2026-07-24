cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
ifeq ($(SPEECH_ALGO),1)
obj-y := $(filter-out %speech_cfg_default.c,$(obj-y))
obj-y := $(filter-out %speech_armcc_func.c,$(obj-y))
ifeq ($(SPEECH_HIFI_V1),1)
obj-y := $(filter-out %speech_algo_v2.c,$(obj-y))
obj-y := $(filter-out %speech_algo_cfg_v2.c,$(obj-y))
obj-y := $(filter-out %anc_assist_cfg_default.c,$(obj-y))
else
obj-y := $(filter-out %speech_algo_v1.c,$(obj-y))
obj-y := $(filter-out %speech_algo_cfg_v1.c,$(obj-y))
endif
else
obj-y := $(filter-out %speech_algo_v1.c,$(obj-y))
obj-y := $(filter-out %speech_algo_cfg_v1.c,$(obj-y))
obj-y := $(filter-out %speech_algo_v2.c,$(obj-y))
obj-y := $(filter-out %speech_algo_cfg_v2.c,$(obj-y))
endif
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

ifeq ($(SPEECH_ALGO),1)
ifeq ($(SPEECH_TX_1MIC_PREAF),1)
ccflags-y += -DSPEECH_TX_1MIC_PREAF
endif
ifeq ($(SPEECH_TX_1MIC_NS),1)
ccflags-y += -DSPEECH_TX_1MIC_NS
endif
ifeq ($(SPEECH_TX_2MIC_PREAF),1)
ccflags-y += -DSPEECH_TX_2MIC_PREAF
endif
ifeq ($(SPEECH_TX_2MIC_NS8),1)
ccflags-y += -DSPEECH_TX_2MIC_NS8
endif
ifeq ($(SPEECH_TX_AEC2FLOAT),1)
ccflags-y += -DSPEECH_TX_AEC2FLOAT
endif
ifeq ($(SPEECH_TX_NS4),1)
ccflags-y += -DSPEECH_TX_NS4
endif
ifeq ($(SPEECH_TX_EQ),1)
ccflags-y += -DSPEECH_TX_EQ
endif
ifeq ($(SPEECH_TX_COMPEXP),1)
ccflags-y += -DSPEECH_TX_COMPEXP
endif
ifeq ($(SPEECH_RX_NS2FLOAT),1)
ccflags-y += -DSPEECH_RX_NS2FLOAT
endif
ifeq ($(SPEECH_RX_EQ),1)
ccflags-y += -DSPEECH_RX_EQ
endif
ifeq ($(SPEECH_RX_COMPEXP),1)
ccflags-y += -DSPEECH_RX_COMPEXP
endif
endif

ifeq ($(MM_PLATFORM),hifi4)
ccflags-y += -D__hifi4__=1
endif

ccflags-y += \
    -Iapps/audioplayers \
	-Imultimedia/smf/demo/libs/sco \
    -Imultimedia/algorithms/src/speech/src/process/common/inc

ifeq ($(SPEECH_HIFI_V1),1)
ccflags-y += -Iservices/multimedia/speech/inc
ccflags-y += -DSPEECH_HIFI_V1
else
ccflags-y += -Imultimedia/algorithms/inc/speech/inc
endif