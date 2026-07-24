# For BT CONFIG start #
export MCU_WAKEUP_BT_V2 ?= 1

export BT_LOG_POWEROFF ?= 1

export RF_APB_RESTORE ?= 1

ifeq ($(NOSTD),1)
export RF_INIT_XTAL_CAP_FROM_NV ?= 0
else
export RF_INIT_XTAL_CAP_FROM_NV ?= 1
endif

export BLE_EXT_ADV_TX_PWR_INDEPEND ?= 0

export AFH_ASSESS ?= 1

export HW_AGC ?= 1

export BT_RF_I2V_BYPASS ?= 0

export NEW_SWAGC_MODE ?= 0

export BLE_NEW_SWAGC_MODE ?= 0

export BT_INFO_CHECKER ?= 1

export BES_FA_MODE ?= 0

export ACL_DATA_CRC_TEST ?= 0

export BT_FAST_ACK ?= 1

#NO ECC module

export CONTROLLER_DUMP_ENABLE ?= 1

export BT_DEBUG_TPORTS ?= 0

export BT_SIGNALTEST_SLEEP_EN ?= 0

export TX_PULLING_CAL ?= 0

export PCM_FAST_MODE ?= 1

export PCM_PRIVATE_DATA_FLAG ?= 0

export BT_RF_PREFER ?= 2M

export SW_TRIG ?= 1

export RX_IQ_CAL ?= 0

export RX_DC_CAL ?= 1

export TX_IQ_CAL ?= 1

export BT_XTAL_SYNC ?= 0

export BT_EXT_LNA_PA ?=0
export BT_EXT_LNA ?=0
export BT_EXT_PA ?=0

export TX_PWR_USE_0P8_PA  ?= 0

export REDUCE_EDGE_CHL_TXPWR ?= 0

export APP_RSSI ?= 1

ifeq ($(SW_TRIG),1)
KBUILD_CPPFLAGS += -D__SW_TRIG__
endif

export BT_PATCH_CRAM_MODE ?= 1
export BT_RAMRUN_NEW ?= 0
ifeq ($(BT_RAMRUN_NEW),1)
KBUILD_CPPFLAGS += -D__BT_RAMRUN_NEW__
KBUILD_CPPFLAGS += -DBTC_ROM_RAM_BASE=0xC0400000 \
                   -DBTC_ROM_RAM_SIZE=0x00040000*2
else
ifeq ($(BT_PATCH_CRAM_MODE),1)
KBUILD_CPPFLAGS += -DBT_PATCH_CRAM_MODE
KBUILD_CPPFLAGS += -DBTC_ROM_RAM_BASE=0xC04C0000 \
                   -DBTC_ROM_RAM_SIZE=0x00040000
endif
endif

ifeq ($(FORCE_SIGNALINGMODE),1)
ifeq ($(BT_LOG_POWEROFF),1)
KBUILD_CPPFLAGS += -DBT_SIGNALTEST_SLEEP_EN
endif
endif

# For BT CONFIG end #