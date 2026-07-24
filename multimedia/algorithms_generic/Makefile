
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

ifeq ($(MM_PLATFORM),hifi4)
export __hifi4__ ?= 1
endif
ifeq ($(__hifi4__),1)
subdir-ccflags-y += -D__hifi4__=1
endif

mmlib_obj :=

ifneq ($(SPEECH_ALGO),1)
ifneq ($(wildcard $(cur_dir)/src/algorithm/fft/src/Makefile),)
mmlib_obj += src/algorithm/fft/src/
endif

ifneq ($(wildcard $(cur_dir)/src/audio/process/iir/src/Makefile),)
mmlib_obj += src/audio/process/iir/src/
endif

ifneq ($(wildcard $(cur_dir)/src/audio/process/adp/src/Makefile),)
mmlib_obj += src/audio/process/adp/src/
endif

ifneq ($(wildcard $(cur_dir)/src/audio/process/fir2iir/src/Makefile),)
mmlib_obj += src/audio/process/fir2iir/src/
endif

ifneq ($(wildcard $(cur_dir)/src/audio/process/common/src/Makefile),)
mmlib_obj += src/audio/process/common/src/
endif

ifneq ($(wildcard $(cur_dir)/src/audio/process/drc/src/Makefile),)
mmlib_obj += src/audio/process/drc/src/
endif

ifneq ($(wildcard $(cur_dir)/src/audio/process/limiter/src/Makefile),)
mmlib_obj += src/audio/process/limiter/src/
endif

ifneq ($(wildcard $(cur_dir)/src/audio/process/resample/src/Makefile),)
mmlib_obj += src/audio/process/resample/src/
endif

ifneq ($(wildcard $(cur_dir)/src/audio/process/integer_resampling/src/Makefile),)
mmlib_obj += src/audio/process/integer_resampling/src/
endif

ifneq ($(wildcard $(cur_dir)/src/audio/process/bes_fir_filter/src/Makefile),)
mmlib_obj += src/audio/process/bes_fir_filter/src/
endif

ifneq ($(wildcard $(cur_dir)/src/audio/process/adj_mc/src/Makefile),)
mmlib_obj += src/audio/process/adj_mc/src/
endif

ifneq ($(wildcard $(cur_dir)/src/audio/process/reverb/src/Makefile),)
mmlib_obj += src/audio/process/reverb/src/
endif

ifneq ($(wildcard $(cur_dir)/src/audio/process/sbcplc/src/Makefile),)
mmlib_obj += src/audio/process/sbcplc/src/
endif

ifneq ($(wildcard $(cur_dir)/src/audio/process/dynamic_boost/src/Makefile),)
mmlib_obj += src/audio/process/dynamic_boost/src/
endif

ifneq ($(wildcard $(cur_dir)/src/audio/process/dynamic_eq/src/Makefile),)
mmlib_obj += src/audio/process/dynamic_eq/src/
endif

ifneq ($(wildcard $(cur_dir)/src/audio/process/bass_enhancer/src/Makefile),)
mmlib_obj += src/audio/process/bass_enhancer/src/
endif

ifneq ($(wildcard $(cur_dir)/src/audio/process/virtual_surround_process/src/Makefile),)
mmlib_obj += src/audio/process/virtual_surround_process/src/
endif

ifneq ($(wildcard $(cur_dir)/src/audio/process/adaptive_volume/src/Makefile),)
mmlib_obj += src/audio/process/adaptive_volume/src/
endif

ifneq ($(wildcard $(cur_dir)/src/audio/process/musicplc/Makefile),)
mmlib_obj += src/audio/process/musicplc/
endif

ifneq ($(wildcard $(cur_dir)/src/audio/process/head_track/Makefile),)
mmlib_obj += src/audio/process/head_track/
endif

ifneq ($(wildcard $(cur_dir)/src/audio/process/audio_ramp/Makefile),)
mmlib_obj += src/audio/process/audio_ramp/
endif
endif

ifneq ($(wildcard $(cur_dir)/src/audio/process/floatlimiter/src/Makefile),)
mmlib_obj += src/audio/process/floatlimiter/src/
endif

export MULTIMEDIA_DEBUG ?=1
ifeq ($(MULTIMEDIA_DEBUG),1)
KBUILD_CPPFLAGS += -DMULTIMEDIA_DEBUG
else
KBUILD_CPPFLAGS += -DNDEBUG
endif

ifeq ($(FM_ENABLE),1)
ifneq ($(wildcard $(cur_dir)src//fm/src/Makefile),)
mmlib_obj += src/fm/
endif
endif


ifneq ($(wildcard $(cur_dir)/src/speech/src/Makefile),)
mmlib_obj += src/speech/src/
endif

ifeq ($(__hifi4__),1)
mmlib_obj += src/hifi_dsp_lib/
endif

GEN_LIB_NAME := $(CHIP)_libmultimedia

ifeq ($(AUDIO_SECTION_ENABLE), 1)
GEN_LIB_NAME := $(GEN_LIB_NAME)_eq
endif

ifeq ($(CHIP_SUBSYS),sensor_hub)
GEN_LIB_NAME := $(GEN_LIB_NAME)_$(CHIP_SUBSYS)
endif

root_dir := $(abspath $(dir $(realpath $(firstword $(MAKEFILE_LIST))))/..)
sinclude $(root_dir)/config/lib.mk
$(GEN_LIB_NAME)-y := $(mmlib_obj)

rel_src_obj :=
rel_src_obj += inc/audio/process/resample/coef/
rel_src_obj += inc/speech/

obj-y := $(GEN_LIB_NAME).a $(rel_src_obj)

ifeq ($(SPEECH_ARMCC_LIB),1)
ifeq ($(FREEMAN_ENABLED_STERO),1)
obj-y += lib_speech_${CPU}_stero.a
else
# obj-y += lib_speech_${CPU}.a
endif
endif

subdir-ccflags-y += \
	-Iutils/heap \
	-Iinclude/cmsis_dsp \
	-Imultimedia/algorithms/inc/hifi_dsp_lib \
	-Imultimedia/algorithms/inc/hifi_dsp_lib/dsp \
	$(NN_INCLUDE) \
	$(CODEC_DSP_INCLUDES) \
	-I$(APPS_DIR)/common \
	-Imultimedia/inc/

ifneq ($(LTO_CFLAGS),)
KBUILD_CFLAGS := $(filter-out $(LTO_CFLAGS),$(KBUILD_CFLAGS))
KBUILD_CPPFLAGS := $(filter-out $(LTO_CPPFLAGS),$(KBUILD_CPPFLAGS))
endif

ifeq ($(__hifi4__),1)
subdir-ccflags-y += \
    -Imultimedia/algorithms/inc/hifi_dsp_lib \
    -Imultimedia/algorithms/inc/hifi_dsp_lib/dsp
endif