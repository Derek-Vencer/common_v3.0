#!/bin/bash
# [ -z "$1" ] &&{
#     echo The Chip is Null !!!
#     exit
# }

CHIPID=$1
COMMAND=$2

TARGET_LIST="best1307p"
echo "1307p"

LEA_CMD="BLE_AUDIO_ENABLED=1 LEA_ENABLE=1 HOST_GEN_FULL_ECDH_KEY=1 APP_BLE_BIS_DELEG_ENABLE=1 APP_BLE_BIS_SINK_ENABLE=1"

CPU_TYPE="BES2710IUC"
RAM_CMD="RAMCPX_SIZE=0xC000 RAMCP_SIZE=0x2C000 OS_DYNAMIC_MEM_SIZE=0x6000"
FLASH_CMD="FLASH_SIZE=0x200000"
BT_CMD=""
CAP_CMD="CAPSENSOR_ENABLE=1 CAPSENSOR_AT_MCU=1 CAP_CHNL_BONDING_WITH_GPIO=1 CHIP_CAPSENSOR_VER=4 CAPSENSOR_FP_MODE=1 \
CAPSENSOR_TOUCH=1 CAPSENSOR_WEAR=0 CAPSENSOR_SLIDE=0 CAPSENSOR_TRACE_DEBUG=0 CAPSENSOR_SPP_SERVER=1"
LEA_CMD=""

BES_LIB_DIR="BES_LIB_DIR=lib/bes/best1307p/PRO_0001"

echo "|***********************************************|"
echo "|                start build ota                |"
echo "|***********************************************|"
build_ota_cmd="make T=prod_test/ota_copy CHIP=$TARGET_LIST -j DEBUG=1 BES_OTA=1 OTA_BIN_COMPRESSED=1 FLASH_REMAP=0 \
$FLASH_CMD OTA_BOOT_SIZE=0x18000 CHIP_HAS_CP=0 SINGLE_WIRE_DOWNLOAD=1"

echo "|***********************************************|"
echo "|                start build app                |"
echo "|***********************************************|"
build_app_cmd="make T=$TARGET_LIST -j DEBUG=1 \
    A2DP_VIRTUAL_SURROUND=0 AUDIO_LIMITER=1 AUDIO_DRC=1 A2DP_LDAC_ON=0 EQ_SET_CUSTOMER_EN=1 \
    $CAP_CMD $BT_CMD \
    A2DP_CP_ACCEL=0 SCO_CP_ACCEL=1 CHIP_HAS_CP=1 UNIFY_HEAP_ENABLED=1 \
    BESUI_APP_EN=1 TOTA_v2=1 BLE=1 \
    BES_OTA=1 TRACE_BUF_SIZE=4*1024 $RAM_CMD $FLASH_CMD \
    BES_TWSPRO_EN=1 BESUI_TWS_EN=1 \
    ANC_APP=0 ANC_ENABLE=0 APP_ANC_TEST=0 SPP_EQ_TUNING=1 \
    BTLE_NAME_FIXED_EN=0 BTLENAME_USE_SCANRSP_EN=0 BLE_ADDR_SET_LR_SAME_EN=1 \
	USER_SPEECH_DUMP_EN=0 CRASH_FAST_RST_EN=0 PAIRMODE_AUTO_POWEROFF_EN=1 \
    USER_BURN_ADDR_PAIR_EN=1 EVB_DEMO_EN=1"

if [[ "$COMMAND" == "clean" ]];
then
    make T=$TARGET_LIST clean ||{ echo "$LINENO command failed"; exit 1; }
elif [[ "$COMMAND" == "gen_lib" ]];
then
    $build_app_cmd $BES_LIB_DIR -j64 GEN_LIB=1 ||{ echo "$LINENO command failed"; exit 1; }
    $build_ota_cmd $BES_LIB_DIR -j64 GEN_LIB=1 ||{ echo "$LINENO command failed"; exit 1; }
elif [[ "$COMMAND" == 'lst' ]];
then
    $build_app_cmd $BES_LIB_DIR lst all ||{ echo "$LINENO command failed"; exit 1; }
    echo $build_app_cmd $BES_LIB_DIR lst all
else
    $build_app_cmd $BES_LIB_DIR ||{ echo "$LINENO command failed"; exit 1; }
    $build_ota_cmd $BES_LIB_DIR ||{ echo "$LINENO command failed"; exit 1; }
    echo $build_app_cmd $BES_LIB_DIR 
fi
