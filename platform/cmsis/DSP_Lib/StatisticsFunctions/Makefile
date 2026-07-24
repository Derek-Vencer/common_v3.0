cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

ifneq ($(SPEECH_ROM), 1)
obj-y := StatisticsFunctions.o
obj-y += StatisticsFunctionsF16.o
else
obj-y := arm_power_f32.o
endif
