## ------------------------------------------------------------------- ##
## BT service features config
## ------------------------------------------------------------------- ##
export BT_SERVICE_ENABLE ?= 1
ifeq ($(BT_SERVICE_ENABLE),1)
KBUILD_CPPFLAGS += -DBT_SERVICE_ENABLE

export BTHOST_USE_IBRT ?= 1
ifeq ($(BTHOST_USE_IBRT),1)
export IBRT ?= 1
export IBRT_UI ?= 1
export BES_AUD ?= 1
ifeq ($(IBRT_UI),1)
KBUILD_CPPFLAGS += -DIBRT_UI
endif #IBRT_UI
endif #BTHOST_USE_IBRT

## ------------------------------------------------------------------- ##
## framework config
## ------------------------------------------------------------------- ##
# ****************************************** #
# BT_SVC_FW_PRODUCT Optional:
#     BT_SVC_FW_PRODUCT_DONGLE
#     BT_SVC_FW_PRODUCT_EARBUDS
#     BT_SVC_FW_PRODUCT_HEADSET
#     BT_SVC_FW_PRODUCT_GLASSES
#     BT_SVC_FW_PRODUCT_SPEAKER
#     BT_SVC_FW_PRODUCT_WATCH
#     BT_SVC_FW_PRODUCT_WIRELESSMIC
# ****************************************** #
export BT_SVC_FW_PRODUCT ?= BT_SVC_FW_PRODUCT_EARBUDS

ifeq ($(BT_SVC_FW_PRODUCT),BT_SVC_FW_PRODUCT_EARBUDS)
BT_SVC_MODULE_BT_ENABLED := 1
BT_SVC_MODULE_TWS_ENABLED := 1
BT_SVC_MODULE_IBRT_ENABLED := 1
endif

ifeq ($(BT_SVC_FW_PRODUCT),BT_SVC_FW_PRODUCT_HEADSET)
BT_SVC_MODULE_BT_ENABLED := 0
BT_SVC_MODULE_TWS_ENABLED := 0
BT_SVC_MODULE_IBRT_ENABLED := 0
endif

ifeq ($(BT_SVC_FW_PRODUCT),BT_SVC_FW_PRODUCT_WIRELESSMIC)
BT_SVC_MODULE_BT_ENABLED := 1
BT_SVC_MODULE_TWS_ENABLED := 0
BT_SVC_MODULE_IBRT_ENABLED := 0
endif

ifneq ($(BT_SVC_FW_PRODUCT),)
export BT_SVC_FW_ENABLED ?= 1
KBUILD_CPPFLAGS += -D$(BT_SVC_FW_PRODUCT)
endif
## ------------------------------------------------------------------- ##

## ------------------------------------------------------------------- ##
## module config
## ------------------------------------------------------------------- ##
export BT_SVC_MODULE_ENABLED ?= 0
export BT_SVC_MODULE_BT_ENABLED ?= 1
ifeq ($(BT_SVC_MODULE_BT_ENABLED),1)
BT_SVC_MODULE_ENABLED := 1
KBUILD_CPPFLAGS += -DBT_SVC_MODULE_BT_ENABLED
endif #BT_SVC_MODULE_BT_ENABLED

export BT_SVC_MODULE_TWS_ENABLED ?= 1
ifeq ($(BT_SVC_MODULE_TWS_ENABLED),1)
BT_SVC_MODULE_ENABLED := 1
KBUILD_CPPFLAGS += -DBT_SVC_MODULE_TWS_ENABLED

export BT_SVC_MODULE_TWS_BLE_SEAMLESS_SWITCH ?= 0
ifeq ($(BT_SVC_MODULE_TWS_BLE_SEAMLESS_SWITCH),1)
KBUILD_CPPFLAGS += -DBT_SVC_MODULE_TWS_BLE_SEAMLESS_SWITCH
endif #BT_SVC_MODULE_TWS_BLE_SEAMLESS_SWITCH
endif #BT_SVC_MODULE_TWS_ENABLED

export BT_SVC_MODULE_IBRT_ENABLED ?= 1
ifeq ($(BT_SVC_MODULE_IBRT_ENABLED),1)
BT_SVC_MODULE_ENABLED := 1
KBUILD_CPPFLAGS += -DBT_SVC_MODULE_IBRT_ENABLED
endif #BT_SVC_MODULE_IBRT_ENABLED

export BT_SVC_MODULE_LEA_ENABLED ?= 0
ifeq ($(BLE_AUDIO_ENABLED),1)
BT_SVC_MODULE_LEA_ENABLED := 1
endif

ifeq ($(BT_SVC_MODULE_LEA_ENABLED),1)
BT_SVC_MODULE_ENABLED := 1
KBUILD_CPPFLAGS += -DBT_SVC_MODULE_LEA_ENABLED

BLE ?= 1  # ENABLE BTHOST BLE FRATURE
BLE_AUDIO_ENABLED ?= 1 #ENABLE BTHOST BLE AUDIO FEATURE

export BT_SVC_MODULE_LEA_BC_SRC_ENABLED ?= 0
ifeq ($(BT_SVC_MODULE_LEA_BC_SRC_ENABLED),1)
KBUILD_CPPFLAGS += -DBT_SVC_MODULE_LEA_BC_SRC_ENABLED
export APP_BLE_BIS_SRC_ENABLE := 1
export BT_SERVICE_HEAP_SIZE ?= 4*1024
export AUDIO_OUTPUT_ROUTE_SELECT := 1
export PLAYBACK_FORCE_48K=1
export ISO_BUF_NB ?= 20
export ISO_BUF_SIZE ?= 250
endif #BT_SVC_MODULE_LEA_BC_SRC_ENABLED

endif #BT_SVC_MODULE_LEA_ENABLED

export BT_SVC_MODULE_WT_ENABLED ?= 0
ifeq ($(BT_SVC_MODULE_WT_ENABLED),1)
BT_SVC_MODULE_ENABLED := 1
KBUILD_CPPFLAGS += -DBT_SVC_MODULE_WT_ENABLED
endif #BT_SVC_MODULE_WT_ENABLED
endif #BT_SERVICE_ENABLE
## ------------------------------------------------------------------- ##
## BT service includes header file path
## ------------------------------------------------------------------- ##
export BT_SERVICE_MODULE_DIR_PATH = bthost/host/src/service/

export BT_SERVICE_UX_INCLUDES = \
    -Ibthost/host/src/service/module/common/inc \
    -Ibthost/host/src/service/module/common/if \
    -Ibthost/host/src/service/module/bt/inc \
    -Ibthost/host/src/service/module/ibrt/inc \
    -Ibthost/host/src/service/module/tws/inc \
    -Ibthost/host/src/service/module/lea/api \
    -Ibthost/host/src/service/module/lea/adapter_api \
    -Ibthost/host/src/service/module/lea/adapter_api/ble_audio_central \
    -Ibthost/host/src/service/module/walkie_talkie/inc  \
    -Ibthost/host/src/service/module/base/inc \
    -Ibthost/host/src/bta/dongle/inc \
    -Ibthost/host/src/bta/normal/inc \
    -Ibthost/host/src/bta/speaker/inc \
    -Ibthost/host/src/bta/wmic/inc \
    -Ibthost/host/src/bta/wmic/api \
    -Ibthost/host/inc/base/bt \
    -Ibthost/host/inc/base/le \
    -Ibthost/host/inc/base/lea \
    -Ibthost/host/inc/base/tws \
    -Ibthost/host/inc/base \
    -Ibthost/host/inc/bta/bt \
    -Ibthost/host/inc/bta/dongle \
    -Ibthost/host/inc/bta/normal \
    -Ibthost/host/inc/bta/sound \
    -Ibthost/host/inc/bta/wmic \
    -Ibthost/host/inc/bta/tws \
    -Ibthost/host/inc/service/am \
    -Ibthost/host/inc/service/bt \
    -Ibthost/host/inc/service/le \
    -Ibthost/host/inc/service/lea \
    -Ibthost/host/inc/service/tws \
    -Ibthost/host/inc/service \
    -Iapps/earbuds/app_ibrt/inc

export BTA_LEGACY_INCLUDES = \
    -Ibthost/host/src/bta/tws/legacy/earbuds_ui/inc

## ------------------------------------------------------------------- ##
## bt service api includes header file path
## ------------------------------------------------------------------- ##
export BT_SERVICE_API_DIR_PATH = bthost/host/inc/service

export BT_SERVICE_API_INCLUDES := \
    -I$(BT_SERVICE_API_DIR_PATH)/ \
    -I$(BT_SERVICE_API_DIR_PATH)/am \
    -I$(BT_SERVICE_API_DIR_PATH)/bt \
    -I$(BT_SERVICE_API_DIR_PATH)/le \
    -I$(BT_SERVICE_API_DIR_PATH)/lea \
    -I$(BT_SERVICE_API_DIR_PATH)/tws \
    -Ibthost/host/inc/base/bt \
    -Ibthost/host/inc/base/le \
    -Ibthost/host/inc/base/lea \
    -Ibthost/host/inc/base/tws \
    -Ibthost/host/inc/base

## ------------------------------------------------------------------- ##
## audio policy includes header file path
## ------------------------------------------------------------------- ##
export BLE_AUD_DIR_PATH := bthost/host/src/service/audio_policy/lea

ifeq ($(BT_SVC_MODULE_IBRT_ENABLED),1)
export BT_AUD_DIR_PATH := bthost/host/src/service/audio_policy/tws
else
export BT_AUD_DIR_PATH := bthost/host/src/service/audio_policy/normal
endif

export BT_AUD_FOCUS_DIR_PATH := bthost/host/src/service/audio_policy/bt_focus/inc

export BT_BLE_AUDIO_POLICY_INCLUDES := \
    -I$(BLE_AUD_DIR_PATH)/inc  \
    -I$(BT_AUD_DIR_PATH)/inc  \
    -I$(BT_AUD_FOCUS_DIR_PATH)
## ------------------------------------------------------------------- ##