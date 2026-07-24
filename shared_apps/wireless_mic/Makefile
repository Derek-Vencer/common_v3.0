cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

obj-y += algo/

ifeq ($(WIRELESS_MIC_TX),1)
obj-y += tx/

subdir-ccflags-y += \
    -Ishared_apps/wireless_mic/tx/datapath \
    -Ishared_apps/wireless_mic/tx/datapath/encoder \
    -Ishared_apps/wireless_mic/tx/datapath/adc \
    -Ishared_apps/wireless_mic/tx/datapath/ble \
    -Ishared_apps/wireless_mic/tx/datapath/emmc \
    -Ishared_apps/wireless_mic/tx/datapath/spp \
    -Ishared_apps/wireless_mic/tx/datapath/spp/custom \
    -Ishared_apps/wireless_mic/tx/datapath/hfp \
    -Ishared_apps/wireless_mic/tx/datapath/usb \
    -Ishared_apps/wireless_mic/algo/tx \
    -Ishared_apps/wireless_mic/algo/ns
endif

ifeq ($(WIRELESS_MIC_RX),1)
obj-y += rx/

subdir-ccflags-y += \
    -Ishared_apps/wireless_mic/rx/datapath/ \
    -Ishared_apps/wireless_mic/rx/datapath/ble \
    -Ishared_apps/wireless_mic/rx/datapath/decoder \
    -Ishared_apps/wireless_mic/rx/datapath/usb \
    -Ishared_apps/wireless_mic/rx/datapath/usb/resample \
    -Ishared_apps/wireless_mic/rx/datapath/dac \
    -Ishared_apps/wireless_mic/rx/datapath/i2s \
    -Ishared_apps/wireless_mic/rx/datapath/adc \
    -Ishared_apps/wireless_mic/algo/rx
endif

ifeq ($(BES_LTC_ENABLE),1)
KBUILD_CPPFLAGS += -DLTC_ENABLED
obj-y += ltc/bes/src/
subdir-ccflags-y += -Ishared_apps/wireless_mic/ltc/bes/src
endif

ifeq ($(THIRD_LTC_ENABLE),1)
KBUILD_CPPFLAGS += -DLTC_ENABLED
obj-y += ltc/thirdparty/
subdir-ccflags-y += -Ishared_apps/wireless_mic/ltc/thirdparty/ \
                    -Ishared_apps/wireless_mic/ltc/thirdparty/include/
endif

ifeq ($(NEW_LEA_PLAYER),1)
KBUILD_CPPFLAGS += -DNEW_LEA_PLAYER
export LEA_PLAYER_PATH = shared_apps/lea_player
else
export LEA_PLAYER_PATH = services/lea_player
endif

subdir-ccflags-y += \
    -Ishared_apps/wireless_mic \
    -Ishared_apps/wireless_mic/algo \
    -Ishared_apps/wireless_mic/algo/plc \
    -Ishared_apps/wireless_mic/algo/plc/inc \
    -Ishared_apps/wireless_mic/algo/lc3_adapter \
    -Iapps/main \
    -Iapps/audioplayers \
    -Iapps/common \
    -Iapps/key \
    -Iapps/cross_core_common \
    -Iapps/usbaudio/usb_audio_control \
    -Iapps/btusbaudio \
    -Iapps/bt_sync \
    -Ibthost/stack/bt_if/inc \
    -Ibthost/service/bt_app/inc \
    -Ibthost/stack/bt_profiles/inc \
    -Ibthost/stack/common/ \
    $(BES_STACK_ADAPTER_INCLUDES) \
    $(BLUETOOTH_ADAPTER_INCLUDES) \
    $(BLE_APP_INCLUDES) \
    $(BT_STACK_INCLUDES) \
    $(BT_SERVICE_UX_INCLUDES) \
    -Imultimedia/inc \
    -Imultimedia/inc/audio/process/filters/include \
    -Imultimedia/inc/speech/inc \
    -Imultimedia/inc/audio/process/hdr/include \
    -Imultimedia/inc/speech/inc \
    -Imultimedia/inc/audio/process/sw_gain/include \
    -Imultimedia/inc/audio/process/multipoint_drc/include \
    -Imultimedia/inc/audio/process/audio_channel_merge/include \
    -Iplatform/drivers/ana \
    -Iplatform/drivers/bt \
    -Iplatform/drivers/cp_accel \
    -Iservices/audio_process \
    -Iservices/app_custom \
    -Iservices/app_custom/ble_custom \
    -Iservices/osif/ \
    -Iservices/tota_v2 \
    -Iservices/nv_section/userdata_section \
    -Iservices/overlay \
    -Iservices/resources \
    -Iservices/audio_bt \
    -Iservices/audio_manager \
    -Iservices/audioflinger \
    -I$(LEA_PLAYER_PATH)/inc \
    -Iservices/norflash_api \
    -Itests/anc_usb \
    -Iutils/heap \
    -Iutils/kfifo \
    -Iutils/cfifo \
    -Iutils/cqueue \
    -Iutils/list