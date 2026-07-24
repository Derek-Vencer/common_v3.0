cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

subdir-ccflags-y += \
	-Iapps/btapp/bt_apps \
	-Iapps/btapp/app_rssi \
	-Iapps/btapp/bt_app \
	-Iapps/btapp/iap2

obj-y += bt_app/

ifeq ($(BT_APP_RSSI),1)
obj-y += app_rssi/
endif

ifeq ($(BT_SVC_MODULE_TWS_ENABLED),1)
obj-y += app_tws_cmd/
endif

ifeq ($(BLE_USB_AUDIO_SUPPORT),1)
obj-y += ble_usb_audio/
endif

ifeq ($(BLE_WALKIE_TALKIE),1)
obj-y += ble_walkie_talkie/
endif

ifeq ($(IOS_IAP2_BLUETOOTH),1)
obj-y += iap2/
endif