cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

ifneq ($(SPEECH_ROM), 1)
obj-y := TransformFunctions.o
obj-y += TransformFunctionsF16.o
else
obj-y := \
	arm_cfft_init_f32.o \
	arm_rfft_fast_init_f32.o
endif