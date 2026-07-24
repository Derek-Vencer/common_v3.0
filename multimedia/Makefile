cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

mmlib_obj :=

export MULTIMEDIA_DEBUG ?=1
ifeq ($(MULTIMEDIA_DEBUG),1)
KBUILD_CPPFLAGS += -DMULTIMEDIA_DEBUG
else
KBUILD_CPPFLAGS += -DNDEBUG
endif

ifneq ($(NO_MULTIMEDIA_CODEC),1)
mmlib_obj += ../multimedia/libs/
endif

GEN_LIB_NAME := $(CHIP)_libmultimedia

ifeq ($(ROM_UTILS_ON),1)
GEN_LIB_NAME := $(GEN_LIB_NAME)_romaac
endif

ifeq ($(A2DP_CP_ACCEL),1)
GEN_LIB_NAME := $(GEN_LIB_NAME)_cp
endif

ifeq ($(LC3PLUS_ENABLE),1)
GEN_LIB_NAME := $(GEN_LIB_NAME)_lc3plus
endif

root_dir := $(abspath $(dir $(realpath $(firstword $(MAKEFILE_LIST))))/..)
sinclude $(root_dir)/config/lib.mk
$(GEN_LIB_NAME)-y := $(mmlib_obj)

obj-y := $(GEN_LIB_NAME).a

subdir-ccflags-y += \
	-Iutils/heap \
	-Iinclude/cmsis_dsp \
	$(NN_INCLUDE) \
	$(CODEC_DSP_INCLUDES) \
	-I$(APPS_DIR)/common \
	-Imultimedia/inc/

ifneq ($(LTO_CFLAGS),)
KBUILD_CFLAGS := $(filter-out $(LTO_CFLAGS),$(KBUILD_CFLAGS))
KBUILD_CPPFLAGS := $(filter-out $(LTO_CPPFLAGS),$(KBUILD_CPPFLAGS))
endif
