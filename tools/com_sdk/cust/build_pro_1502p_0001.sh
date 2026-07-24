#!/bin/bash

TARGET_LIST="best1502p"
CHIPID="best1502p"
COMMAND=$1

TYPE_CFG="FREEMAN_ENABLED_STERO=0"
COM_CFG="DEBUG=1 APP_TRACE_RX_ENABLE=1 APP_RX_API_ENABLE=1 TRACE_BUF_SIZE=16*1024"
OTA_CFG="BES_OTA=1 OTA_BIN_COMPRESSED=1"
ENC_CFG="SPEECH_TX_2MIC_NS8=1"
CODEC_CFG="A2DP_LDAC_ON=1"
ADV_CFG="TOTA_v2=1 BLE=1 GFPS_ENABLE=0 SPOT_ENABLE=0"
ANC_CFG="ANC_ENABLE=1 ANC_ASSIST_ENABLED=1 VOICE_ASSIST_FF_FIR_LMS=1 \
         SENSOR_HUB=1 APP_RPC_ENABLE=1 \
         APP_MCPP_CLI=BTH APP_MCPP_BTH_SENS=1 \
         LARGE_SENS_RAM=1 SENS_FIR_LMS_SIZE=0x20000 \
         VOICE_ASSIST_FF_FIR_LMS_BTH_SENS=1 \
         APP_ANC_ASSIST_THREAD=1 AUDIO_ANC_TT_HW=1 \
         SENSORHUB_START_WHEN_MAIN_MCU_ON=0 SENS_TRC_TO_MCU=1"
LIB_CFG="BES_LIB_DIR=lib/bes/best1502p/PRO_0001"
PAIR_CFG="POWER_ON_ENTER_TWS_PAIRING_ENABLED=1"
set -e


BUILD_CUSTOMER_CFG="-j $TYPE_CFG $COM_CFG $OTA_CFG $ALGO_CFG $ENC_CFG $CODEC_CFG $ADV_CFG $ANC_CFG $PAIR_CFG"

################################### var define ###################################

build_speech_lib_cmd="make T=noapp_test RTOS=0 COMPILE_ONLY=1 GEN_LIB=1 DSP_LIB=1 SPEECH_LIB=1 \
multimedia/algorithms/src/speech/src/ TOOLCHAIN=armclang ALLOW_WARNING=1 -j CHIP=$CHIPID \
AUDIO_ANC_FIR_HW=1 SPEECH_ARMCC_LIB=1 FREEMAN_ENABLED_STERO=0 $LIB_CFG"

build_sensor_hub_cmd="make T=sensor_hub -j8 SENS_TRC_TO_MCU=1 CHIP=$CHIPID VOICE_ASSIST_FF_FIR_LMS=1 \
MCU_I2C_SLAVE=1 LARGE_SENS_RAM=1 SENS_USE_SENS_MAIN=1 SPEECH_ARMCC_LIB=1 APP_MCPP_CLI=SENS \
APP_MCPP_SRV=bes APP_MCPP_BTH_SENS=1 AUDIO_ANC_FIR_HW=1 FIR_LMS_AEC_ENABLED=1 CP_IN_SAME_EE=1 \
SENS_FIR_LMS_SIZE=0x20000 VOICE_ASSIST_FF_FIR_LMS_CP_ACCEL=1 VOICE_ASSIST_FF_FIR_LMS_MULTI_THREAD=0 \
FREEMAN_ENABLED_STERO=0 $LIB_CFG"

build_cust_cmd="make T=$TARGET_LIST $BUILD_CUSTOMER_CFG $LIB_CFG -j64"

build_ota_copy_cmd="make T=prod_test/ota_copy CHIP=$CHIPID DEBUG=1 OTA_BIN_COMPRESSED=1 $LIB_CFG -j64"

if [[ "$COMMAND" == "clean" ]];
then
    $build_ota_copy_cmd clean ||{ echo "$LINENO command failed"; exit 1; }
    $build_speech_lib_cmd clean ||{ echo "$LINENO command failed"; exit 1; }
    $build_sensor_hub_cmd clean ||{ echo "$LINENO command failed"; exit 1; }
    $build_cust_cmd clean ||{ echo "$LINENO command failed"; exit 1; }
elif [[ "$COMMAND" == "gen_lib" ]];
then
    $build_speech_lib_cmd ||{ echo "$LINENO command failed"; exit 1; }
    $build_sensor_hub_cmd GEN_LIB=1 ||{ echo "$LINENO command failed"; exit 1; }
    $build_cust_cmd GEN_LIB=1 ||{ echo "$LINENO command failed"; exit 1; }
    $build_ota_copy_cmd GEN_LIB=1 ||{ echo "$LINENO command failed"; exit 1; }
elif [[ "$COMMAND" == 'lst' ]];
then
    $build_sensor_hub_cmd lst all ||{ echo "$LINENO command failed"; exit 1; }
    $build_cust_cmd lst all ||{ echo "$LINENO command failed"; exit 1; }
else
    $build_speech_lib_cmd ||{ echo "$LINENO command failed"; exit 1; }
    $build_sensor_hub_cmd ||{ echo "$LINENO command failed"; exit 1; }
    $build_cust_cmd ||{ echo "$LINENO command failed"; exit 1; }
    $build_ota_copy_cmd ||{ echo "$LINENO command failed"; exit 1; }
fi
echo $build_speech_lib_cmd
echo $build_sensor_hub_cmd
echo $build_cust_cmd
echo $build_ota_copy_cmd
