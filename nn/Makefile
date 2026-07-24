cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y :=
subdir-ccflags-y := -DBES_MODIFICATION

ifeq ($(BECO),1)
obj-y += beco/
endif

ifeq ($(NN_LIB),1)
obj-y += cmsis-nn/
endif

ifeq ($(ETHOSU_ARCH), u55)
obj-y += ethosu-driver/
subdir-ccflags-y += -DETHOSU -DETHOSU55 -DETHOSU_ARCH=$(ETHOSU_ARCH)

ifeq ($(ETHOSU_DRIVER_TEST),1)
# TODO: Move test code to ethosu-driver folder
obj-y += ethosu-platform/

else ifeq ($(TFLITE_IF_TEST),1)
export TFLITE_MICRO_ENABLE := 1

else ifeq ($(EXECUTORCH_IF_TEST),1)
export EXECUTORCH_ENABLE := 1

else
export TFLITE_MICRO_ENABLE := 1
obj-y += ethosu-platform/
obj-y += ethosu-software/
endif
endif

ifeq ($(TFLITE_MICRO_ENABLE),1)
obj-y += tflite-micro/
endif

ifeq ($(EXECUTORCH_ENABLE),1)
obj-y += executorch/
endif

ifneq ($(ETHOSU_LOG_ENABLE),)
subdir-ccflags-y += -DETHOSU_LOG_ENABLE=$(ETHOSU_LOG_ENABLE)
endif

ifneq ($(ETHOSU_LOG_SEVERITY),)
subdir-ccflags-y += -DETHOSU_LOG_SEVERITY=$(ETHOSU_LOG_SEVERITY)
endif

ifneq ($(NN_DATA_RAM_LOC),)
subdir-ccflags-y += -DNN_DATA_RAM_LOC=$(NN_DATA_RAM_LOC)
endif

ifneq ($(NN_MODEL_RAM_LOC),)
subdir-ccflags-y += -DNN_MODEL_RAM_LOC=$(NN_MODEL_RAM_LOC)
endif

ifeq ($(NN_INCLUDES),)
export NN_INCLUDES := -Inn/cmsis-nn/Include \
                      -Iinclude/cmsis_dsp \
                      -Iplatform/cmsis/DSP_Lib/Include \
                      -Inn/beco/Include
endif
