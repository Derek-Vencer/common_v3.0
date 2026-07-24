#!/bin/bash

COMMAND=$1
COMMAND2=$2
set -e

chmod 777 tools/*
chmod 777 tools/com_sdk/*
chmod 777 tools/com_sdk/cust/*

# ./tools/com_sdk/switch_to_others.sh

rm -rf out/*
if [[ "$COMMAND" == "gen_lib" ]];
then
rm -rf lib/bes/*
./tools/com_sdk/build_cust_cmd.sh best1306p gen_lib
./tools/com_sdk/build_cust_cmd.sh best1502p gen_lib
./tools/com_sdk/build_cust_cmd.sh best1502x gen_lib
./tools/com_sdk/build_cust_cmd.sh best1503 gen_lib
# ./tools/com_sdk/build_cust_cmd.sh best1306 gen_lib
./tools/com_sdk/build_cust_cmd.sh best1503p gen_lib
./tools/com_sdk/build_cust_cmd.sh best1307ph gen_lib
./tools/com_sdk/build_cust_cmd.sh best1307p gen_lib
./tools/com_sdk/cust/build_pro_1307p_0001.sh gen_lib
./tools/com_sdk/cust/build_pro_1307p_0002.sh gen_lib
./tools/com_sdk/cust/build_pro_1307p_0003.sh gen_lib
# ./tools/com_sdk/cust/build_pro_1307p_0005.sh gen_lib
./tools/com_sdk/cust/build_pro_1307p_0006.sh gen_lib
./tools/com_sdk/cust/build_pro_1307ph_0001.sh gen_lib
./tools/com_sdk/cust/build_pro_1307ph_0002.sh gen_lib
./tools/com_sdk/cust/build_pro_1307ph_0003.sh gen_lib
./tools/com_sdk/cust/build_pro_1307ph_0004.sh gen_lib
# ./tools/com_sdk/cust/build_pro_1502p_0001.sh gen_lib
./tools/com_sdk/cust/build_pro_1502p_0002.sh gen_lib
else
./tools/com_sdk/build_cust_cmd.sh best1306p
./tools/com_sdk/build_cust_cmd.sh best1502x
./tools/com_sdk/build_cust_cmd.sh best1502p
./tools/com_sdk/build_cust_cmd.sh best1503
# ./tools/com_sdk/build_cust_cmd.sh best1306
./tools/com_sdk/build_cust_cmd.sh best1503p
./tools/com_sdk/build_cust_cmd.sh best1307ph
./tools/com_sdk/build_cust_cmd.sh best1307p
./tools/com_sdk/cust/build_pro_1307p_0001.sh
./tools/com_sdk/cust/build_pro_1307p_0002.sh
./tools/com_sdk/cust/build_pro_1307p_0003.sh
# ./tools/com_sdk/cust/build_pro_1307p_0005.sh
./tools/com_sdk/cust/build_pro_1307p_0006.sh
./tools/com_sdk/cust/build_pro_1307ph_0001.sh
./tools/com_sdk/cust/build_pro_1307ph_0002.sh
./tools/com_sdk/cust/build_pro_1307ph_0003.sh
./tools/com_sdk/cust/build_pro_1307ph_0004.sh
# ./tools/com_sdk/cust/build_pro_1502p_0001.sh
./tools/com_sdk/cust/build_pro_1502p_0002.sh
fi
# ./tools/besui_tool/build.sh TWSPRO OTA
# ./tools/besui_tool/build.sh NOTWS OTA

# ./tools/com_sdk/build_pro_all_1307p.sh