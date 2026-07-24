cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

subdir-ccflags-y += \
    $(BT_SERVICE_UX_INCLUDES) \
    -Iplatform/drivers/ana \
    -Iutils/cqueue \
    -Iinclude

ifeq ($(NO_WDT),1)
CFLAGS_app_watchdog.o += -DNO_WDT
endif

ifneq ($(APP_THREAD_STACK_SIZE),)
CFLAGS_app_thread.o += -DAPP_THREAD_STACK_SIZE=$(APP_THREAD_STACK_SIZE)
endif

ifeq ($(APP_TRACE_RX_HEAP_SIZE),)
ifeq ($(HW_DAC_DRC),1)
APP_TRACE_RX_HEAP_SIZE := 8192
endif
endif

ifneq ($(APP_TRACE_RX_HEAP_SIZE),)
CFLAGS_app_trace_rx.o += -DAPP_TRACE_RX_HEAP_SIZE=$(APP_TRACE_RX_HEAP_SIZE)
endif

ifeq ($(AUDIO_DEBUG_CMD),1)
CFLAGS_app_trace_rx.o += -DAUDIO_DEBUG_CMD
endif

ifneq ($(APP_DEBUG_TOOL),)
CFLAGS_app_trace_rx.o += -DAUDIO_DEBUG_CMD
endif
