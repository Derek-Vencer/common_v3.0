cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

ifneq ($(SPEECH_ROM), 1)
obj-y := BasicMathFunctions.o
obj-y += BasicMathFunctionsF16.o
else
obj-y := arm_mult_q15.o
obj-y += arm_negate_q15.o
obj-y += arm_dot_prod_f32.o
obj-y += arm_add_f32.o
obj-y += arm_sub_f32.o
endif