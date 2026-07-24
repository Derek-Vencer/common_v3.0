cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

ifneq ($(SPEECH_ROM),1)
obj-y := FastMathFunctions.o
obj-y += FastMathFunctionsF16.o
else
obj-y += arm_atan2_f32.o
obj-y += arm_cos_f32.o
obj-y += arm_sin_f32.o
endif