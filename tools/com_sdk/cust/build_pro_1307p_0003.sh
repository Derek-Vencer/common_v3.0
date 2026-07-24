#!/bin/bash

###############################################################################
# Build script for E61
# Usage:
#   ./build_dewalt.sh [clean|gen_lib|lst]
###############################################################################

COMMAND=$1

BES_LIB_DIR="BES_LIB_DIR=lib/bes/best1307p/PRO_0003"

build_ota_cmd="make T=prod_test/ota_copy CHIP=best1307p -j DEBUG=1 BES_OTA=1 OTA_BIN_COMPRESSED=1 \
FLASH_REMAP=0 FLASH_SIZE=0x200000 CHIP_HAS_CP=0 SINGLE_WIRE_DOWNLOAD=1 $BES_LIB_DIR"

build_app_cmd="make T=best1307p -j BES_OTA=1 OTA_BIN_COMPRESSED=1 FLASH_SIZE=0x200000 \
POWER_ON_ENTER_TWS_PAIRING_ENABLED=1 BLE=1 TOTA_v2=1 ANC_APP=1 APP_ANC_TEST=1 \
SPEECH_TX_2MIC_NS8=0 SPEECH_TX_1MIC_NS=1 A2DP_LDAC_ON=0 AUDIO_DYNAMIC_EQ=1 \
AUDIO_BASS_ENHANCER=0 SPP_EQ_TUNING=1 CAPSENSOR_ENABLE=1 CAPSENSOR_AT_MCU=1 \
CAP_CHNL_BONDING_WITH_GPIO=1 CHIP_CAPSENSOR_VER=4 CAPSENSOR_FP_MODE=1 \
CAPSENSOR_TOUCH=1 CAPSENSOR_WEAR=0 CAPSENSOR_SLIDE=0 CAPSENSOR_TRACE_DEBUG=0 \
CAPSENSOR_SPP_SERVER=1 RAMCP_SIZE=0X30000 RAMCPX_SIZE=0x4000 APP_TRACE_RX_ENABLE=1 APP_RX_API_ENABLE=1 TRACE_BUF_SIZE=26*1024 $BES_LIB_DIR"

###############################################################################
# Dispatch by COMMAND
###############################################################################
if [[ "$COMMAND" == "clean" ]]; then
    $build_ota_cmd clean || { echo "$LINENO command failed"; exit 1; }
    $build_app_cmd clean || { echo "$LINENO command failed"; exit 1; }
elif [[ "$COMMAND" == "gen_lib" ]]; then
    $build_ota_cmd GEN_LIB=1 || { echo "$LINENO command failed"; exit 1; }
    $build_app_cmd GEN_LIB=1 || { echo "$LINENO command failed"; exit 1; }
elif [[ "$COMMAND" == "lst" ]]; then
    $build_ota_cmd lst all || { echo "$LINENO command failed"; exit 1; }
    $build_app_cmd lst all || { echo "$LINENO command failed"; exit 1; }
else
    $build_ota_cmd || { echo "$LINENO command failed"; exit 1; }
    $build_app_cmd || { echo "$LINENO command failed"; exit 1; }
fi

echo "-------------------------"
echo "   Building for E61   "
echo "-------------------------"
echo $build_ota_cmd
echo $build_app_cmd
