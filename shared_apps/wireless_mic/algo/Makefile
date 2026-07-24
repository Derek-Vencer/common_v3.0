cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

ifeq ($(LC3_CODEC_ON),1)
obj-y += lc3_adapter/
endif

ifeq ($(TX_ALGO_PROCESS), 1)
obj-y += tx/
endif

ifeq ($(RX_ALGO_PROCESS), 1)
obj-y += rx/
endif

ifeq ($(SPEECH_TX_1MIC_NS_48KF), 1)
obj-y += ns/
endif