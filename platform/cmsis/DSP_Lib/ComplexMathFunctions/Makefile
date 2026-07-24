cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

ifneq ($(SPEECH_ROM),1)
obj-y := ComplexMathFunctions.o
obj-y += ComplexMathFunctionsF16.o
else
obj-y := arm_cmplx_mag_squared_f32.o
endif