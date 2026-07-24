cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

ifeq ($(AUDIO_CODEC_ENABLE),1)
obj-y += encoder/
endif

ifeq ($(AUD_INPUT_BY_ADC),1)
obj-y += adc/
endif

ifeq ($(AUD_OUTPUT_BY_BLE),1)
obj-y += ble/
endif

ifeq ($(AUD_OUTPUT_BY_SPP),1)
obj-y += spp/
endif

ifeq ($(AUD_OUTPUT_BY_EMMC),1)
obj-y += emmc/
endif

ifeq ($(AUD_OUTPUT_BY_HFP),1)
obj-y += hfp/
endif

ifeq ($(AUD_OUTPUT_BY_USB),1)
obj-y += usb/
endif