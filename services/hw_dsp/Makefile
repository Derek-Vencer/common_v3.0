
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y :=

obj-y += src/hw_filter_codec_iir.o

ifeq ($(HW_DAC_DRC),1)
obj-y += src/hw_dac_drc.o
endif

ifeq ($(HW_FILTER_CODEC_IIR),1)
ifeq ($(ANC_APP),1)
$(error "error: ANC_APP and HW_FILTER_CODEC_IIR can't be used in the same")
endif
ifeq ($(HW_DAC_IIR_EQ_PROCESS),1)
$(error "error: HW_DAC_IIR_EQ_PROCESS and HW_FILTER_CODEC_IIR can't be used in the same")
endif
endif

ccflags-y := \
    -Iservices/hw_dsp/inc \
	$(CODEC_DSP_INCLUDES) \
	-Iservices/nv_section/aud_section \
	-Iservices/nv_section/include

ifeq ($(AUDIO_RESAMPLE),1)
ccflags-y += -D__AUDIO_RESAMPLE__
endif
