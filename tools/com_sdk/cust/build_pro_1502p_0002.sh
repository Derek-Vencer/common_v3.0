#!/bin/bash

COMMAND=$1
set -e

################################### var define ###################################
LIB_DIR_CFG="BES_LIB_DIR=lib/bes/best1502p/PRO_0002 "

GEN_LIB="GEN_LIB=1"

BASIC_BT_CFG=" \
POWER_ON_ENTER_TWS_PAIRING_ENABLED=1
BT_SVC_FW_PRODUCT=BT_SVC_FW_PRODUCT_EARBUDS "

ENHANCE_BT_CFG="HFP_SUPPORT_LC3_SWB=0"

GFPS_CFG="GFPS_ENABLE=1"

EQ_CFG="ANC_APP=1 ANC_ENABLE=1 HW_DAC_IIR_EQ_PROCESS=1 HW_DAC_DRC=1"

BLE_CFG="BLE=1 BIS_SELFSCAN_ENABLED=0 BLE_AUDIO_ENABLED=1 CTKD_ENABLE=1 IS_CTKD_OVER_BR_EDR_ENABLED=1 GAF_CONVERSATIONAL_STREAM_PROCESS_ENABLE=1"

BES_OTA_CFG="BES_OTA=1 TEST_OVER_THE_AIR_v2=1 TOTA_v2=1 OTA_BIN_COMPRESSED=1"

POLICY_CFG="BT_PAUSE_A2DP=1 BT_MUTE_A2DP=0"

IOS_FINDMY_CFG="IOS_IAP2_BLUETOOTH=1"

CAP_CFG="CAPSENSOR_ENABLE=1 CAPSENSOR_AT_MCU=1 CAPSENSOR_TOUCH=1 CAPSENSOR_WEAR=1 CAPSENSOR_SLIDE=1 CAPSENSOR_TRACE_DEBUG=0 CAPSENSOR_FP_MODE=1 CHIP_CAPSENSOR_VER=4 CAPSENSOR_READ_DATA_POLLING=1 CAPSENSOR_SPP_SERVER=1"

DEBUG_CFG="APP_TRACE_RX_ENABLE=1 APP_RX_API_ENABLE=1 AUDIO_DEBUG=0"

# AUDIO_DEBUG_CFG="AUDIO_DEBUG=1"

build_ota_copy_cmd="make T=prod_test/ota_copy DEBUG=0 -j8 OTA_BIN_COMPRESSED=1 CHIP=best1502p SINGLE_WIRE_DOWNLOAD=1 \
${LIB_DIR_CFG} \
"

build_cust_cmd="make T=best1502p \
${LIB_DIR_CFG} \
${BASIC_BT_CFG} \
${ENHANCE_BT_CFG} \
${POLICY_CFG} \
${CAP_CFG} \
${EQ_CFG} \
${GFPS_CFG} \
${BES_OTA_CFG} \
${IOS_FINDMY_CFG} \
${BLE_CFG} \
${DEBUG_CFG} \
-j64"

#default: 1mic off + 2mic on + AVSS off

if [[ "$COMMAND" == "clean" ]];
then
    $build_ota_copy_cmd clean ||{ echo "$LINENO command failed"; exit 1; }
    $build_cust_cmd clean ||{ echo "$LINENO command failed"; exit 1; }
elif [[ "$COMMAND" == "gen_lib" ]];
then
    $build_ota_copy_cmd GEN_LIB=1 ||{ echo "$LINENO command failed"; exit 1; }
    $build_cust_cmd GEN_LIB=1 ||{ echo "$LINENO command failed"; exit 1; }
elif [[ "$COMMAND" == 'lst' ]];
then
    $build_cust_cmd lst all ||{ echo "$LINENO command failed"; exit 1; }
else
    $build_ota_copy_cmd ||{ echo "$LINENO command failed"; exit 1; }
    $build_cust_cmd ||{ echo "$LINENO command failed"; exit 1; }
fi

echo $build_ota_copy_cmd
echo $build_cust_cmd $default_cfg