cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

ifneq ($(SPEECH_ROM), 1)
obj-y := SupportFunctions.o
obj-y += SupportFunctionsF16.o
else
obj-y += arm_copy_q15.o
obj-y += arm_copy_q31.o
endif
