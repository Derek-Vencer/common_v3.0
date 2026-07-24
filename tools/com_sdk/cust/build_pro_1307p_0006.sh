#!/bin/bash

###############################################################################
# Build script for LL
# Usage:
#   ./build_dewalt.sh [clean|gen_lib|lst]
###############################################################################

COMMAND=$1

PARAM_SW_VERSION="1"
PARAM_FILE="param_twspro.h"
PARAM_CAP_CH="CAP_CH1" #CAP_CH1|CAP_CH2|CAP_CH3
PARAM_PRO_ID="$PRONAME"
PARAM_BT_NAME="$PRONAME"
PARAM_BLE_NAME="${PRONAME}_BLE"
PARAM_TOUCH_KEYDOWN_TH="-18000"
PARAM_TOUCH_KEYUP_TH="-12000"
DAC_REDUCE_DB="7"
PARAM_NTC_CH="HAL_GPADC_CHAN_3"
PARAM_FMD_MANU_NAME="bestechnic"
PARAM_FMD_MODEL_NAME="bestechnic"
PARAM_FMD_DEV_TYPE="0x96"

#charger param
PARAM_PRECHG_CURRENT="CHARGER_CHARGE_PRECHARGE_CURRENT_20MA"
PARAM_CC_CURRENT="CHARGER_CHARGE_CONSTANT_CURRENT_80MA"
PARAM_STOP_CURRENT="CHARGER_CHARGE_STOP_CURRENT_6MA"
PARAM_CV_VOLT="CHARGER_CHARGE_CONSTANT_VOLTAGE_4450MV"
PARAM_RECHG_VOLT="CHARGER_CHARGE_RECHARGE_VOLTAGE_200MV"

BESUI_CMD="SOFTWARE_VERSION_INFO=$PARAM_SW_VERSION BESUI_PARAM_FILE=$PARAM_FILE PARAM_CAP_CH=$PARAM_CAP_CH \
BESUI_PROJECT_ID=$PARAM_PRO_ID BESUI_BT_NAME=$PARAM_BT_NAME BESUI_BLE_NAME=$PARAM_BLE_NAME \
BESUI_TOUCH_KEYDOWN_TH=$PARAM_TOUCH_KEYDOWN_TH BESUI_TOUCH_KEYUP_TH=$PARAM_TOUCH_KEYUP_TH PARAM_NTC_CH=$PARAM_NTC_CH \
BESUI_FMD_MANU_NAME=$PARAM_FMD_MANU_NAME BESUI_FMD_MODEL_NAME=$PARAM_FMD_MODEL_NAME BESUI_FMD_DEV_TYPE=$PARAM_FMD_DEV_TYPE"

BES_LIB_DIR="BES_LIB_DIR=lib/bes/best1307p/PRO_0006"

build_ota_cmd="make T=prod_test/ota_copy CHIP=best1307p -j DEBUG=1 BES_OTA=1 OTA_BIN_COMPRESSED=1 \
FLASH_REMAP=0 FLASH_SIZE=0x200000 OTA_BOOT_SIZE=0x18000 CHIP_HAS_CP=0 SINGLE_WIRE_DOWNLOAD=1 $BES_LIB_DIR"

build_app_cmd="make T=best1307p -j BES_OTA=1 OTA_BIN_COMPRESSED=1 FLASH_SIZE=0x200000 POWER_ON_ENTER_TWS_PAIRING_ENABLED=1 \
ANC_APP=1 APP_ANC_TEST=1 VOICE_ASSIST_NOISE=1 ANC_ASSIST_ENABLED=1 ASSIST_WIND=0 \
BLE=1 GATT_OVER_BR_EDR=1 SPEECH_TX_1MIC_NS=1 SPEECH_TX_2MIC_NS8=0 \
A2DP_LDAC_ON=1 A2DP_LHDCV5_ON=1 A2DP_VIRTUAL_SURROUND=0 AUDIO_DRC=1 SPP_EQ_TUNING=1 \
BESUI_APP_EN=1 BES_TWSPRO_EN=1 BESUI_TWS_EN=1 USER_BURN_ADDR_PAIR_EN=1 BESUI_TWS_PAIR_USE_KEY_EN=0 \
BESUI_PRECHG_CURRENT=$PARAM_PRECHG_CURRENT BESUI_CC_CURRENT=$PARAM_CC_CURRENT BESUI_STOP_CURRENT=$PARAM_STOP_CURRENT \
BESUI_CV_VOLT=$PARAM_CV_VOLT BESUI_RECHG_VOLT=$PARAM_RECHG_VOLT \
RAMCP_SIZE=150*1024 RAMCPX_SIZE=38*1024 FAST_XRAM_SECTION_SIZE=2*1024 TOTA_v2=1 TRACE_BUF_SIZE=20*1024 $BESUI_CMD $BES_LIB_DIR"


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
echo "   Building for LL   "
echo "-------------------------"
echo $build_ota_cmd
echo $build_app_cmd
