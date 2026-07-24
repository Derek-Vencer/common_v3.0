#!/bin/bash
COMMAND=$1

TARGET_LIST="best1306p"
BES_LIB_DIR="BES_LIB_DIR=lib/bes/best1306p/PRO_0001"
set -e

################################### var define ###################################
APP_CUSTOMER_CFG="IGNORE_POWER_ON_KEY_DURING_BOOT_UP=1 IS_AUTOPOWEROFF_ENABLED=0 APP_TRACE_RX_ENABLE=1 APP_RX_API_ENABLE=1 AUDIO_DEBUG_CMD=0 \
                 TRACE_BUF_SIZE=40*1024 USE_TRACE_ID=0"

OTA_CFG="BES_OTA=1 OTA_BIN_COMPRESSED=0"

BLE_CFG="BLE=1 GATT_OVER_BR_EDR=0 GFPS_ENABLE=0 SPOT_ENABLE=0"

TWS_CFG="POWER_ON_ENTER_TWS_PAIRING_ENABLED=1"

SPEECH_2MIC_CFG="SPEECH_TX_DC_FILTER=1 SPEECH_TX_2MIC_NS8=1 SPEECH_TX_2MIC_PREAF=1 SPEECH_TX_EQ=1"

AUDIO_CFG=" AUDIO_BASS_ENHANCER=1"

DECODE_DECODER_CFG="A2DP_AAC_ON=1 A2DP_LDAC_ON=1 A2DP_LHDC_ON=0 A2DP_LHDC_V3=0 A2DP_LHDCV5_ON=0"

SYS_CFG=" ALLOW_WARNING=1 A2DP_CP_ACCEL=1 SCO_CP_ACCEL=1 CHIP_HAS_CP=1"

################################### end of var define ###################################
build_app_cmd="make T=$TARGET_LIST $APP_CUSTOMER_CFG $BLE_CFG $OTA_CFG $DECODE_DECODER_CFG $SYS_CFG $SPEECH_2MIC_CFG $TWS_CFG $BES_LIB_DIR -j64"
build_ota_cmd="make T=prod_test/ota_copy CHIP=best1306p OTA_BIN_COMPRESSED=0 DEBUG=0 $BES_LIB_DIR -j64"

if [[ "$COMMAND" == "clean" ]];
then
    rm -rf out/*
elif [[ "$COMMAND" == "gen_lib" ]];
then
    $build_app_cmd GEN_LIB=1 ||{ echo "$LINENO command failed"; exit 1; }
    $build_ota_cmd GEN_LIB=1 ||{ echo "$LINENO command failed"; exit 1; }
elif [[ "$COMMAND" == 'lst' ]];
then
    $build_app_cmd lst all ||{ echo "$LINENO command failed"; exit 1; }
    echo $build_app_cmd lst all
else
    $build_app_cmd ||{ echo "$LINENO command failed"; exit 1; }
    $build_ota_cmd ||{ echo "$LINENO command failed"; exit 1; }
    echo $build_app_cmd
    echo $build_ota_cmd
fi
