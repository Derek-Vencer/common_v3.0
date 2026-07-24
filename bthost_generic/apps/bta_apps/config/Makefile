cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

ifneq ($(BLE_ONLY_ENABLED),1)
obj-y += bt/
endif

ifeq ($(BLE_HOST_SUPPORT),1)
obj-y += le/
endif

ifeq ($(BT_SVC_MODULE_IBRT_ENABLED),1)
obj-y += tws_only/
else
obj-y += non_tws/
endif

subdir-ccflags-y += \
    $(BLUETOOTH_ADAPTER_INCLUDES) \
    $(BT_SERVICE_UX_INCLUDES) \
    $(BLE_APP_INCLUDES) \
    -I$(BT_ROM_PORTING_PATH) \
    $(BT_BLE_AUDIO_POLICY_INCLUDES) \
    -Ibthost/host/inc/bta/bt \
    -Ibthost/apps/bta_apps/earbuds/inc \
    -Ibthost/apps/bta_apps/adapter/earbuds/vv/bt_adapter \
    -Ibthost/host/src/service/ble_app_new/inc \
    -Ithirdparty/userapi \
    -Ithirdparty/audio_codec_lib/liblhdc-dec/inc \
    $(AUDIO_ALGORITHMS_INCLUDES) \
    -Imultimedia/inc/audio/process/resample/include \
    -Iservices/resources \
    -Iplatform/drivers/bt \
    -Iplatform/drivers/ana \
    -Iplatform/drivers/uarthci \
    -Iplatform/drivers/usb/usb_dev/inc \
    -Iplatform/drivers/sensor_hub \
    -Iplatform/hal \
    -Iutils/cqueue \
    -Iutils/list \
    -Iutils/heap \
    -Iutils/crc \
    -Iutils/cppobject \
    -Iutils/eshell \
    -Iutils/rom_utils \
    -Iutils/hsm \
    -Iutils/kfifo \
    -Iutils/cfifo \
    -Iservices/audioflinger \
    -Iutils/lockcqueue \
    -Iutils/hwtimer_list \
    -Iapps \
    -Iapps/key \
    -Iapps/main \
    -Iapps/battery \
    -Iapps/bt_sync \
    -Iapps/common \
    -Iapps/factory \
    -Iapps/anc/inc	\
    -Iapps/btspeaker \
    -Iapps/audioplayers \
    -Iapps/audioplayers/a2dp_decoder \
    -Iapps/audioplayers/audio_trigger/version1 \
    -Iapps/audioplayers/audio_trigger/version1 \
    -Iapps/sensorhub \
    -Iapps/app_test/ble_audio_test \
    -Iapps/earbuds/conn \
    -Iapps/anc/inc \
    -Iapps/common/uart_dma \
    -Iapps/common/chip_bridge \
    -Iapps/earbuds/test \
    -Iapps/psap/inc \
    -Iapps/ble_audio_central_app/ble_audio_usb \
    -Iservices/audio_manager/audio_policy/bredr_policy/inc \
    -Iservices/fs/fat \
    -Iservices/fs/sd \
    -Iservices/fs/fat/ChaN \
    -Iservices/audio_bt \
    -I$(LEA_PLAYER_PATH)/inc \
    -Iservices/ai_voice/manager \
    -Iservices/ai_voice/transport \
    -Iservices/nv_section/factory_section \
    -Iservices/nv_section/log_section \
    -Iservices/norflash_api \
    -Iservices/bt_app/a2dp_codecs/include \
    -Iservices/multimedia/audio/process/resample/include \
    -Iservices/interconnection/green \
    -Iservices/voicepath	\
    -Iservices/app_ai/inc \
    -Iservices/audio_manager \
    -Iservices/audio_manager/test \
    -Iservices/bis_transport/inc \
    -Iservices/overlay \
    -Iservices/ota \
    -Iservices/tota_v2 \
    -Iservices/ota/ai_ota \
    -Iservices/ota/bes_ota/inc \
    -Iservices/voicepath/gsound/gsound_target_api_read_only \
    -Iservices/voicepath/gsound/gsound_custom/inc \
    -Iservices/voicepath/gsound/gsound_target \
    -Iservices/ai_voice/protocol/bixbyvoice \
    -Iservices/ai_voice/protocol/bixbyvoice/bixbyvoice_manager \
    -Iservices/ai_voice/protocol/gma/gma_manager \
    -Iservices/ai_voice/protocol/gma/gma_crypto \
    -Iservices/ai_voice/protocol/dual_mic_recording/voice_manager \
    -Iservices/ai_voice/codec/compression \
    -Iservices/ai_voice/codec/opus121/include \
    -Iservices/app_debug_info_system/inc \
    -I$(BT_SERVICE_MODULE_DIR_PATH)/module/config \
    -Iservices/bt_app \
    -Iservices/osif \
    -Iservices/bt_sync \
    -Iservices/osif \
    -Iservices/gfps/inc \
    -Ibthost/apps/bta_apps/config \
    -Ibthost/apps/bta_apps/config/bt/a2dp_codecs \
    -Ibthost/apps/bta_apps/earbuds_deprecated/inc
