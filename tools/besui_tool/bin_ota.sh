#!/bin/bash
#build command "./tools/besui_tool/build.sh best1307p TWSPRO OTA"

CMD1=$1
CMD2=$2
CMD3=$3
CMD4=$4
echo "CMD1 = "$CMD1
echo "CMD2 = "$CMD2
echo "CMD3 = "$CMD3
echo "CMD4 = "$CMD4

CHIP_TYPE=$CMD1
CPU_TYPE=$CMD2
PRONAME=$CMD3
BTNAME=$CMD4

BIN_FILE="out/$CHIP_TYPE/$CHIP_TYPE.bin"

rm -rf out/$CHIP_TYPE/apps/besui/besui_common.o

#-------------------------------------------------------------------------------------------------------------------------
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
LST_FILE="out/$CHIP_TYPE/$CHIP_TYPE.lst"
MAP_FILE="out/$CHIP_TYPE/$CHIP_TYPE.map"
delname="*.lst*"
find $otabin_path -type f -name "$delname" -exec rm {} \;
delname="*.map*"
find $otabin_path -type f -name "$delname" -exec rm {} \;

if [ -f "$LST_FILE" ]; then
cp -f $LST_FILE $otabin_path
fi
if [ -f "$MAP_FILE" ]; then
cp -f $MAP_FILE $otabin_path
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

