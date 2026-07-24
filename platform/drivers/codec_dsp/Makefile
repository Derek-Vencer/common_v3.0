cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

ifeq ($(CHIP_SUBTYPE),best1402)
obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S $(cur_dir)$(CHIP_SUBTYPE)/*.c $(cur_dir)$(CHIP_SUBTYPE)/*.cpp $(cur_dir)$(CHIP_SUBTYPE)/*.S))
else
obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S $(cur_dir)$(CHIP)/*.c $(cur_dir)$(CHIP)/*.cpp $(cur_dir)$(CHIP)/*.S))
endif

obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

ccflags-y := \
	$(CODEC_DSP_INCLUDES) \
	-Iinclude/cmsis_dsp \
	-Iplatform/drivers/ana \
	-Iservices/nv_section/aud_section \
	-Iservices/nv_section/include \
	-Imultimedia/algorithms/src/speech/src/process/common/inc \
	$(AUDIO_ALGORITHMS_INCLUDES) \
	-Imultimedia/inc \
	-Iutils/crc32 \
	-Iinclude/cmsis_dsp \
	-Iutils/hwtimer_list

ifeq ($(USB_AUDIO_APP),1)
CFLAGS_fir_process_$(CHIP).o += -DFIR_USE_IRQ -DFIR_USE_PINGPONG
endif

ifeq ($(AUDIO_DYNAMIC_BOOST),1)
ccflags-y += -D__AUDIO_DYNAMIC_BOOST__
endif

ifeq ($(A2DP_VIRTUAL_SURROUND_HWFIR),1)
ccflags-y += -DHW_FIR_MUTI_CHANNEL
endif

ccflags-y += -Wno-error=unused-but-set-variable

ifeq ($(CHIP_SUBTYPE),best1402)
ifeq ($(AUDIO_RESAMPLE),1)
CFLAGS_anc_process_$(CHIP_SUBTYPE).o += -D__AUDIO_RESAMPLE__
CFLAGS_psap_process_$(CHIP_SUBTYPE).o += -D__AUDIO_RESAMPLE__
CFLAGS_sidetone_process_$(CHIP_SUBTYPE).o += -D__AUDIO_RESAMPLE__
endif
else
ifeq ($(AUDIO_RESAMPLE),1)
CFLAGS_anc_process_$(CHIP).o += -D__AUDIO_RESAMPLE__
CFLAGS_psap_process_$(CHIP).o += -D__AUDIO_RESAMPLE__
CFLAGS_sidetone_process_$(CHIP).o += -D__AUDIO_RESAMPLE__
endif
endif

ifeq ($(ANC_FF_ENABLED),1)
CFLAGS_anc_process_$(CHIP).o += -DANC_FF_ENABLED
endif
ifeq ($(ANC_FB_ENABLED),1)
CFLAGS_anc_process_$(CHIP).o += -DANC_FB_ENABLED
endif

ifeq ($(HW_DAC_IIR_EQ_PROCESS),1)
CFLAGS_anc_cfg.o += -D__HW_DAC_IIR_EQ_PROCESS__
endif
ifeq ($(HW_IIR_EQ_PROCESS),1)
CFLAGS_anc_cfg.o += -D__HW_IIR_EQ_PROCESS__
endif
ifeq ($(AUDIO_RESAMPLE),1)
CFLAGS_anc_cfg.o += -D__AUDIO_RESAMPLE__
CFLAGS_eq_cfg.o += -D__AUDIO_RESAMPLE__
endif
ifeq ($(PSAP_APP),1)
CFLAGS_anc_cfg.o += -DPSAP_APP
endif
