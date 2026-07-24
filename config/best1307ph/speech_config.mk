# For TWS SCO DMA snapshot and low delay
export PCM_FAST_MODE ?= 1

export PCM_PRIVATE_DATA_FLAG ?= 0

export SPEECH_CODEC ?= 1

export CVSD_BYPASS ?= 1

export LOW_DELAY_SCO ?= 0

# 1mic tx
export SPEECH_ROM_PATCH_9_P_1 ?= 0

export SPEECH_TX_DC_FILTER ?= 1

export SPEECH_DC_FILTER_IN_ROM ?= 1

export SPEECH_TX_AEC2FLOAT ?= 0

export SPEECH_TX_COMPEXP ?= 0

export SPEECH_TX_EQ ?= 1

export SPEECH_TX_1MIC_PREAF ?= 0

export SPEECH_1MIC_PREAF_IN_ROM ?= 1

export SPEECH_TX_1MIC_NS ?= 0

export SPEECH_TX_1MIC_NS_IN_ROM ?= 1

# 2mic tx
export SPEECH_TX_2MIC_PREAF ?= 0

export SPEECH_2MIC_PREAF_IN_ROM ?= 1

export SPEECH_TX_2MIC_NS8 ?= 0

export SPEECH_2MIC_NS8_IN_ROM ?= 1

export SPEECH_ROM_PATCH_1307P ?= 1

export SPEECH_TX_2MIC_NS4 ?= 0

# 3mic tx
export SPEECH_TX_3MIC_PREAF ?= 0

export SPEECH_3MIC_PREAF_IN_ROM ?= 1

export SPEECH_TX_3MIC_NS ?= 0

export SPEECH_TX_3MIC_NS2 ?= 0

export SPEECH_TX_3MIC_NS_IN_ROM ?= 1

export SPEECH_ROM_PATCH_1307P_3MIC ?= 1
# rx
export SPEECH_RX_COMPEXP ?= 0

export SPEECH_RX_EQ ?= 1

export SPEECH_RX_NS2FLOAT ?= 0

export SPEECH_NS2FLOAT_IN_ROM ?= 1

export LPC_PLC_IN_ROM ?= 1

export SPEECH_EQ_IN_ROM ?= 1

export SPEECH_NN_NS_IN_ROM ?= 1