#!/bin/bash
#build command "./tools/besui_tool/build.sh best1307p TWSPRO OTA"

CMD1=$1
CMD2=$2
CMD3=$3
echo "CMD1 = "$CMD1
echo "CMD2 = "$CMD2
echo "CMD3 = "$CMD3

PRONAME=$CMD2
BTNAME="(${CMD2})"

#-------------------------------------------------------------------------------------------------------------------------
#user param
PARAM_SW_VERSION="1"
PARAM_FILE="param_twspro.h"
PARAM_PRO_ID="$PRONAME"
PARAM_BT_NAME="$PRONAME"
PARAM_BLE_NAME="${PRONAME}_BLE"
PARAM_CAP_CH="CAP_CH1" #CAP_CH1|CAP_CH2|CAP_CH3
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

#anc param
PARAM_ANC_COEF_LIST_NUM="1"
PARAM_ANC_FF_MIC_CH_L="AUD_CHANNEL_MAP_CH0"
PARAM_ANC_FB_MIC_CH_L="AUD_CHANNEL_MAP_CH1"
PARAM_ANC_TALK_MIC_CH_L="AUD_CHANNEL_MAP_CH0"

BESUI_CMD="SOFTWARE_VERSION_INFO=$PARAM_SW_VERSION BESUI_PARAM_FILE=$PARAM_FILE PARAM_CAP_CH=$PARAM_CAP_CH \
BESUI_PROJECT_ID=$PARAM_PRO_ID BESUI_BT_NAME=$PARAM_BT_NAME BESUI_BLE_NAME=$PARAM_BLE_NAME \
BESUI_TOUCH_KEYDOWN_TH=$PARAM_TOUCH_KEYDOWN_TH BESUI_TOUCH_KEYUP_TH=$PARAM_TOUCH_KEYUP_TH PARAM_NTC_CH=$PARAM_NTC_CH \
BESUI_FMD_MANU_NAME=$PARAM_FMD_MANU_NAME BESUI_FMD_MODEL_NAME=$PARAM_FMD_MODEL_NAME BESUI_FMD_DEV_TYPE=$PARAM_FMD_DEV_TYPE \
BESUI_PRECHG_CURRENT=$PARAM_PRECHG_CURRENT BESUI_CC_CURRENT=$PARAM_CC_CURRENT BESUI_STOP_CURRENT=$PARAM_STOP_CURRENT \
BESUI_CV_VOLT=$PARAM_CV_VOLT BESUI_RECHG_VOLT=$PARAM_RECHG_VOLT BESUI_ANC_COEF_LIST_NUM=$PARAM_ANC_COEF_LIST_NUM \
BESUI_ANC_FF_MIC_CH_L=$PARAM_ANC_FF_MIC_CH_L BESUI_ANC_FB_MIC_CH_L=$PARAM_ANC_FB_MIC_CH_L BESUI_ANC_TALK_MIC_CH_L=$PARAM_ANC_TALK_MIC_CH_L"

#-------------------------------------------------------------------------------------------------------------------------
# DUT_CMD="FORCE_SIGNALINGMODE=1 FORCE_NOSIGNALINGMODE=0"
# BQB_CMD="FORCE_SIGNALINGMODE=0 FORCE_NOSIGNALINGMODE=1"
# I2C_CMD="DEBUG_PORT=2 MCU_I2C_SLAVE=1 NO_SLEEP=1"
CAP_CMD="CAPSENSOR_ENABLE=1 CAPSENSOR_TOUCH=1 CAPSENSOR_SPP_SERVER=1"
LEA_CMD="BLE_AUDIO_ENABLED=1 LEA_ENABLE=1 HOST_GEN_FULL_ECDH_KEY=1 APP_BLE_BIS_DELEG_ENABLE=1 APP_BLE_BIS_SINK_ENABLE=1 \
CTKD_ENABLE=0 IS_CTKD_OVER_BR_EDR_ENABLED=0"

RAM_CMD="RAMCPX_SIZE=0xC000 RAMCP_SIZE=0x2C000 OS_DYNAMIC_MEM_SIZE=0x6000 \
A2DP_CP_ACCEL=1 SCO_CP_ACCEL=1 CHIP_HAS_CP=1 UNIFY_HEAP_ENABLED=1"
FLASH_CMD="FLASH_SIZE=0x200000"
BT_CMD="IBRT_SEARCH_UI=0 BT_DISC_ACL_AFTER_AUTH_KEY_MISSING=1"
ANC_CMD="ANC_APP=0 ANC_ENABLE=0 APP_ANC_TEST=0"
SPEECH_CMD="SPEECH_TX_1MIC_NS=1 SPEECH_TX_EQ=0 SPEECH_TX_DC_FILTER=0 SPEECH_RX_NS2FLOAT=0 SPEECH_RX_EQ=0"
AUDIO_CMD="HW_DAC_IIR_EQ_PROCESS=0 EQ_SET_CUSTOMER_EN=1 EQ_CUSTOM_APP_EN=1 SPP_EQ_TUNING=1 BESUI_PROMPT_ISSUE_EN=1 \
AUDIO_OUTPUT_SW_GAIN=1 AUDIO_OUTPUT_SW_GAIN_BEFORE_DRC=1 USER_DAC_REDUCE_EN=1 DAC_REDUCE_DB=$DAC_REDUCE_DB"
GFPS_CMD="GFPS_ENABLE=1 BESUI_GFPS_ID_EN=0 BESUI_FIX_GFPS_MODELID_EN=1"
FMD_CMD="SPOT_ENABLE=1 BESUI_FMD_UI_EN=1"

CHIP_TYPE="best1307ph" #best1306 best1306p best1307p best1307ph best1501 best1501p best1502x best1502p best1503
CPU_TYPE="BES2710IUC"

if [ "$CMD1" = "best1306" ]
then
CHIP_TYPE="best1306"
CPU_TYPE="BES2600IUC"
RAM_CMD="A2DP_CP_ACCEL=1 SCO_CP_ACCEL=1 CHIP_HAS_CP=1 UNIFY_HEAP_ENABLED=1 RAMCPX_SIZE=0xBC00 RAMCP_SIZE=0x2B000 FAST_XRAM_SECTION_SIZE=0x2800"
elif [ "$CMD1" = "best1307p" ]
then
CHIP_TYPE="best1307p"
CPU_TYPE="BES2710IUC"
BES_LIB_DIR="BES_LIB_DIR=lib/bes/best1307p/PRO_0001"
RAM_CMD="RAMCPX_SIZE=0xC000 RAMCP_SIZE=0x2C000 OS_DYNAMIC_MEM_SIZE=0x6000 A2DP_CP_ACCEL=1 SCO_CP_ACCEL=1 CHIP_HAS_CP=1 UNIFY_HEAP_ENABLED=1"
FLASH_CMD="FLASH_SIZE=0x200000"
CAP_CMD="CAPSENSOR_ENABLE=1 CAPSENSOR_AT_MCU=1 CAP_CHNL_BONDING_WITH_GPIO=1 CHIP_CAPSENSOR_VER=4 CAPSENSOR_FP_MODE=1 \
CAPSENSOR_TOUCH=1 CAPSENSOR_WEAR=0 CAPSENSOR_SLIDE=0 CAPSENSOR_TRACE_DEBUG=0 CAPSENSOR_SPP_SERVER=1"
LEA_CMD=""
elif [ "$CMD1" = "best1307ph" ]
then
CHIP_TYPE="best1307ph"
CPU_TYPE="BES2711IUC"
BES_LIB_DIR="BES_LIB_DIR=lib/bes/best1307ph/PRO_0001"
RAM_CMD="RAMCPX_SIZE=0xC000 RAMCP_SIZE=0x2C000 OS_DYNAMIC_MEM_SIZE=0x6000 A2DP_CP_ACCEL=1 SCO_CP_ACCEL=1 CHIP_HAS_CP=1 UNIFY_HEAP_ENABLED=1"
FLASH_CMD="FLASH_SIZE=0x200000"
CAP_CMD="CAPSENSOR_ENABLE=1 CAPSENSOR_AT_MCU=1 CAP_CHNL_BONDING_WITH_GPIO=1 CHIP_CAPSENSOR_VER=4 CAPSENSOR_FP_MODE=0 \
CAPSENSOR_TOUCH=1 CAPSENSOR_WEAR=0 CAPSENSOR_SLIDE=0 CAPSENSOR_TRACE_DEBUG=0 CAPSENSOR_SPP_SERVER=1"
elif [ "$CMD1" = "best1501p" ] || [ "$CMD1" = "best1502x" ] || [ "$CMD1" = "best1501" ]
then
CAP_CMD="CAPSENSOR_ENABLE=1 CAPSENSOR_TOUCH=1 CAPSENSOR_SPP_SERVER=0 \
CHIP_CAPSENSOR_VER=0 SENSOR_HUB=1 CAPSENSOR_AT_SENS=1 SENS_TRC_TO_MCU=1"
SENSORHUB_CMD="make T=sensor_hub -j CHIP=$CHIP_TYPE SENSOR_HUB=1 CAPSENSOR_ENABLE=1 \
SENS_CAP_SENS_DMA_ENABLE=1 SENS_TRC_TO_MCU=1 CHIP_CAPSENSOR_VER=0 \
CAPSENSOR_WEAR=1 CAPSENSOR_TOUCH=1 CAPSENSOR_TRACE_DEBUG=0 CAPSENSOR_SPP_SERVER=1 \
"
$SENSORHUB_CMD
fi

if [ "$CMD3" = "OTA" ] || [ "$CMD3" = "ota" ]
then
echo "|***********************************************|"
echo "|                start build ota                |"
echo "|***********************************************|"
make T=prod_test/ota_copy CHIP=$CHIP_TYPE $BES_LIB_DIR -j DEBUG=1 BES_OTA=1 OTA_BIN_COMPRESSED=1 FLASH_REMAP=0 \
$FLASH_CMD OTA_BOOT_SIZE=0x18000 CHIP_HAS_CP=0 SINGLE_WIRE_DOWNLOAD=1 PROGRAMMER_HAL_FULL_INIT=1
fi

#-------------------------------------------------------------------------------------------------------------------------
[[ $PRONAME == TWSPRO ]] &&{
    build_cmd="make T=$CHIP_TYPE -j $BES_LIB_DIR DEBUG=1 TRACE_BUF_SIZE=16*1024 TRACE_BAUD_RATE=3000000 \
    $AUDIO_CMD $ANC_CMD $RAM_CMD $FLASH_CMD $GFPS_CMD $FMD_CMD \
    $DUT_CMD $BQB_CMD $I2C_CMD $SPEECH_CMD $CAP_CMD $BT_CMD $LEA_CMD $BESUI_CMD \
    BESUI_APP_EN=1 TOTA_v2=1 BLE=1 \
    BES_TWSPRO_EN=1 BESUI_TWS_EN=1 BES_OTA=1 \
    BTLE_NAME_FIXED_EN=0 BTLENAME_USE_SCANRSP_EN=0 BLE_ADDR_SET_LR_SAME_EN=0 \
    USER_SPEECH_DUMP_EN=0 CRASH_FAST_RST_EN=0 PAIRMODE_AUTO_POWEROFF_EN=1 BESUI_TWS_PAIR_USE_KEY_EN=1 \
    BESUI_TOUCH_POWERON_EN=1 CHG_OUT_PWRON=1 BATTERY_SWITCH_ROLE_EN=1 \
    USER_BURN_ADDR_PAIR_EN=0 EVB_DEMO_EN=1 BOX_COMM_UNUSE_EN=1 BESUI_NTC_EN=0"
}
[[ $PRONAME == NOTWS ]] &&{
    build_cmd="make T=$CHIP_TYPE -j $BES_LIB_DIR DEBUG=1 TRACE_BUF_SIZE=16*1024 TRACE_BAUD_RATE=3000000 \
    $AUDIO_CMD $ANC_CMD $RAM_CMD $FLASH_CMD $GFPS_CMD $FMD_CMD \
    $DUT_CMD $BQB_CMD $I2C_CMD $SPEECH_CMD $CAP_CMD $BT_CMD $LEA_CMD $BESUI_CMD \
    BESUI_APP_EN=1 TOTA_v2=1 BLE=1 \
    BES_NOTWS_EN=1 BESUI_TWS_EN=1 FREEMAN_ENABLED_STERO=1 FREEMAN_OTA_ENABLE=1 BES_OTA=1 \
    BTLE_NAME_FIXED_EN=0 BTLENAME_USE_SCANRSP_EN=0 BLE_ADDR_SET_LR_SAME_EN=0 \
    USER_SPEECH_DUMP_EN=0 CRASH_FAST_RST_EN=0 PAIRMODE_AUTO_POWEROFF_EN=1 \
    BESUI_TOUCH_POWERON_EN=1 CHG_OUT_PWRON=1 \
    USER_BURN_ADDR_PAIR_EN=0 EVB_DEMO_EN=1 BOX_COMM_UNUSE_EN=1 BESUI_NTC_EN=0"    
}
#-------------------------------------------------------------------------------------------------------------------------
chmod 777 ./tools/fill_sec_base.pl
if [ "$CMD3" = "clean" ] || [ "$CMD3" = "CLEAN" ]
then
    echo "clean"
    $build_cmd clean
    exit 
fi

if [ "$CMD3" = "lst" ] || [ "$CMD3" = "LST" ]
then
    echo "all lst"
    $build_cmd all lst
    exit
fi

rm -rf out/$CHIP_TYPE/apps/besui/besui_common.o
echo "|***********************************************|"
echo "|                start build app                |"
echo "|***********************************************|"

#-------------------------------------------------------------------------------------------------------------------------
BIN_FILE="out/$CHIP_TYPE/$CHIP_TYPE.bin"
COMPILE_CMD=$build_cmd

check_bin_exists() {
    if [ -f "$BIN_FILE" ]; then
        return 0
    else
        return 1
    fi
}

# get time(Linux & macOS)
get_file_time() {
    local file=$1
    local os=$(uname)
    if [ "$os" = "Linux" ]; then
        stat -c "%Y" "$file"
    elif [ "$os" = "Darwin" ]; then
        stat -f "%B" "$file"
    else
        echo "Do not support os: $os"
        exit 1
    fi
}

main_build_check() 
{
    if check_bin_exists; then
        pre_time=$(get_file_time "$BIN_FILE")
    else
        pre_time=0
    fi
    
    $COMPILE_CMD
    
    if ! check_bin_exists; then
        echo "❌ Build failed: no $BIN_FILE"
        exit 1
    fi
    
    post_time=$(get_file_time "$BIN_FILE")
    
    if [ $post_time -gt $pre_time ]; then
        echo "✅ Build success: $BIN_FILE already update"
    else
        echo "❌ Build failed: $BIN_FILE no update"
        exit 1
    fi
}

main_build_check

#-------------------------------------------------------------------------------------------------------------------------
echo "|***********************************************|"
echo "|                ota process start              |"
echo "|***********************************************|"
# BESUI_TIME=$(date +"%s.%N")
BESUI_DATE_TIME=$(date +"%Y-%m-%d %T.%N")
printf "[BESUI_DATE_TIME = %-s]\n" "$BESUI_DATE_TIME"
echo " "

echo "project = "$PRONAME
echo "btname  = "$BTNAME

tool_path=tools/besui_tool
path_bin=out/$CHIP_TYPE/$CHIP_TYPE.bin
ota_out_bin=out/prod_test/ota_copy/ota_copy.bin

#-------------------------------------------------------------------------------------------------------------------------
otabin_path=$tool_path/ota_bin
rm -rf $otabin_path
mkdir $otabin_path
cp -f $ota_out_bin $otabin_path

chmod 777 ./tools/build_compressed_ota.sh

path_del=$tool_path
# delname="*${CPU_TYPE}*"
delname="*BES27*"
find $path_del -type f -name "$delname" -exec rm {} \;
echo "delete "$delname.bin

cp -f $path_bin $tool_path/123.bin
python tools/generate_crc32_of_image.py $tool_path/123.bin
mv $tool_path/123.bin.converted.bin $tool_path/crc123.bin
./tools/build_compressed_ota.sh $tool_path/crc123.bin $tool_path/converted.bin
path_ota_bin=$tool_path/converted.bin
# python tools/generate_crc32_of_image.py $tool_path/converted.bin
# path_ota_bin=$tool_path/converted.bin.converted.bin
# rm -rf $tool_path/converted.bin

#-------------------------------------------------------------------------------------------------------------------------
DATE=`date +%Y%m%d` 
echo date = $DATE

keyword_ver="SW_VERSION=" 
VERSION="V0.0.1"
VERSION=$(sed -n '/'$keyword_ver'/p' $path_bin) 
VERSION=${VERSION##*=}
echo VERSION=$VERSION

keyword_crc="CRC32_OF_IMAGE=0x"
crc_path=$tool_path/crc123.bin
# CRC_VAL="0x00000000"
CRC_VAL=$(sed -n '/'$keyword_crc'/p' $crc_path)
CRC_VAL=${CRC_VAL##*0x}
echo CRC = $CRC_VAL

name_wire=${PRONAME}_WIRE_${CPU_TYPE}_${DATE}_${VERSION}_$CRC_VAL${BTNAME}.bin
echo "wire file = "$name_wire
mv $tool_path/crc123.bin "$tool_path/$name_wire"
rm -rf $tool_path/123.bin

#-------------------------------------------------------------------------------------------------------------------------
#OTA name process
name_ota=${PRONAME}_OTA_${CPU_TYPE}_${DATE}_${VERSION}_$CRC_VAL${BTNAME}.bin  #ota file in phone
echo "ota  file = "$name_ota
mv $path_ota_bin "$tool_path/$name_ota"
#-------------------------------------------------------------------------------------------------------------------------
cp -f $path_bin $otabin_path/app.bin

if [ "$CMD3" = "lst" ] || [ "$CMD3" = "LST" ]
then
    LST_FILE="out/$CHIP_TYPE/$CHIP_TYPE.lst"
    MAP_FILE="out/$CHIP_TYPE/$CHIP_TYPE.map"
    rm -rf $otabin_path/$CHIP_TYPE.lst
    rm -rf $otabin_path/$CHIP_TYPE.map
    if [ -f "$LST_FILE" ]; then
    cp -f $LST_FILE $otabin_path
    fi
    if [ -f "$MAP_FILE" ]; then
    cp -f $MAP_FILE $otabin_path
    fi
fi
#-------------------------------------------------------------------------------------------------------------------------

rm -rf crc.bin
rm -rf magic.bin
echo "delete no use bin"
echo "|***********************************************|"
echo "|                ota process end                |"
echo "|***********************************************|"
echo " "
exit 0

