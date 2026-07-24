# For BT CONFIG start #


# bthost rom set feature
ifeq ($(BTH_IN_ROM),1)
export CVSD_BYPASS ?= 1
endif

#not support
export BT_LOG_POWEROFF ?= 0
#not support
export RF_APB_RESTORE ?= 0

ifeq ($(NOSTD),1)
export RF_INIT_XTAL_CAP_FROM_NV ?= 0
else
export RF_INIT_XTAL_CAP_FROM_NV ?= 1
endif

export BLE_EXT_ADV_TX_PWR_INDEPEND ?= 0

export AFH_ASSESS ?= 0

export HW_AGC ?= 1

export BT_RF_I2V_BYPASS ?= 1

export NEW_SWAGC_MODE ?= 0

export BLE_NEW_SWAGC_MODE ?= 0

export BT_INFO_CHECKER ?= 1

export BES_FA_MODE ?= 0

export ACL_DATA_CRC_TEST ?= 0

export BT_FAST_ACK ?= 1

#NO ECC module
#NO BESTRX

export CONTROLLER_DUMP_ENABLE ?= 1

export BT_DEBUG_TPORTS ?= 0

export BT_SIGNALTEST_SLEEP_EN ?= 0

export TX_PULLING_CAL ?= 0

export PCM_FAST_MODE ?= 1

export PCM_PRIVATE_DATA_FLAG ?= 0

export BT_RF_PREFER ?= 2M

export SW_TRIG ?= 1

export RX_IQ_CAL ?= 0

export TX_IQ_CAL ?= 1

export BT_XTAL_SYNC ?= 0

export BT_EXT_LNA_PA ?=0
export BT_EXT_LNA ?=0
export BT_EXT_PA ?=0

export TX_PWR_USE_0P8_PA  ?= 0

export REDUCE_EDGE_CHL_TXPWR ?= 0

ifeq ($(SW_TRIG),1)
KBUILD_CPPFLAGS += -D__SW_TRIG__
endif

export MCU_WAKEUP_BT_V2 ?= 1

# For BT CONFIG end #
