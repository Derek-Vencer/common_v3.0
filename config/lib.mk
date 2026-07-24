ifeq ($(USE_TRACE_ID), 1)
GEN_LIB_NAME := $(GEN_LIB_NAME)_log_compressed
endif

ifeq ($(BLE), 1)
GEN_LIB_NAME := $(GEN_LIB_NAME)_ble
endif

ifeq ($(GFPS_ENABLE), 1)
GEN_LIB_NAME := $(GEN_LIB_NAME)_gfps
endif

ifeq ($(SASS_ENABLE), 1)
GEN_LIB_NAME := $(GEN_LIB_NAME)_sass
endif

ifeq ($(GATT_OVER_BR_EDR), 1)
GEN_LIB_NAME := $(GEN_LIB_NAME)_btgatt
endif

ifeq ($(ANC_APP), 1)
GEN_LIB_NAME := $(GEN_LIB_NAME)_anc
endif

## AI
ifeq ($(AI_VOICE), 1)
GEN_LIB_NAME := $(GEN_LIB_NAME)_ai
endif

## BT
export GEN_LIB_NAME_BT := $(GEN_LIB_NAME)

ifeq ($(FREEMAN_ENABLED_STERO), 1)
GEN_LIB_NAME_BT := $(GEN_LIB_NAME_BT)_stero
endif

ifeq ($(ARM_CMNS), 1)
GEN_LIB_NAME_BT := $(GEN_LIB_NAME_BT)_cmns
endif

ifeq ($(BLE), 1)
GEN_LIB_NAME_BT := $(GEN_LIB_NAME_BT)_ble
endif

ifeq ($(BLE_CS_ENABLED), 1)
GEN_LIB_NAME_BT := $(GEN_LIB_NAME_BT)_cs
endif

ifeq ($(BLE_AUDIO_ENABLED), 1)
GEN_LIB_NAME_BT := $(GEN_LIB_NAME_BT)_lea
endif

ifeq ($(FREEMAN_ENABLED_STERO), 1)
GEN_LIB_NAME_BT := $(GEN_LIB_NAME_BT)_stereo
endif

ifeq ($(IBRT),1)
GEN_LIB_NAME_BT := $(GEN_LIB_NAME_BT)_ibrt
endif

ifeq ($(SUPPORT_REMOTE_COD), 1)
GEN_LIB_NAME_BT := $(GEN_LIB_NAME_BT)_cod
endif

ifeq ($(BT_RF_PREFER), 2M)
GEN_LIB_NAME_BT := $(GEN_LIB_NAME_BT)_2m
endif

ifeq ($(BT_RF_PREFER), 3M)
GEN_LIB_NAME_BT := $(GEN_LIB_NAME_BT)_3m
endif


