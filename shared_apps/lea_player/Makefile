cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))

obj_tmp_gaf := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gaf_audio/*.cpp))

#aob bis
ifeq ($(CFG_BAP_BC), 0)
obj_cpp += $(filter-out $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gaf_audio/gaf_bis_*.*)), $(obj_tmp_gaf))
else
obj_cpp := $(obj_tmp_aob)
obj_cpp += $(obj_tmp_gaf)
endif

ifeq ($(AOB_MOBILE_ENABLED),1)
ifeq ($(BLE_USB_AUDIO_SUPPORT),1)
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gaf_audio_path/*.cpp))
endif
endif

obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gaf_codec/*.cpp))

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

obj-y := $(src_obj)

obj-y += gaf_cc_stream/

ccflags-y += -DBLUETOOTH_BLE_IMPL

subdir-ccflags-y += \
    $(BT_STACK_INCLUDES) \
    -I$(BT_ROM_PORTING_PATH) \
    $(BLUETOOTH_ADAPTER_INCLUDES) \
    $(BLE_APP_INCLUDES) \
    $(BT_BLE_AUDIO_POLICY_INCLUDES) \
    -I$(BLE_AOB_APP_DIR_PATH)/gaf_audio \
    -Iinclude/cmsis_dsp \
    -Iservices/bt_app \
    -Iservices/fs/fat \
    -Iservices/fs/sd \
    -Iservices/fs/fat/ChaN \
    -Iservices/overlay \
    -Iservices/resources \
    -Iservices/audio_bt \
    -I$(LEA_PLAYER_PATH)/inc \
    $(BT_SERVICE_UX_INCLUDES) \
    $(AUDIO_ALGORITHMS_INCLUDES) \
    $(CODEC_DSP_INCLUDES) \
    -Imultimedia/smf/inc/3th/ \
    -Imultimedia/inc/ \
    -Iutils/simo_fifo/ \
    -Iapps/anc/src/ \
    -Ibthost/apps/bta_apps/dongle/inc/ \
    -Imultimedia/smf/inc/elements/ \
	-Itests/besair_platform/bsp_tester/dac_eq_interface/ \
    -Iplatform/drivers/codec_dsp/ \
    -Imultimedia/smf/inc/common/ \
    -Iapps/cross_core_common/ \
    -Iservices/nv_section/factory_section \
    -Iservices/nv_section/fpga_section \
    -Iplatform/drivers/uarthci \
    -Iplatform/drivers/ana \
    -Iplatform/drivers/bt \
    -Iplatform/drivers/cp_accel \
    -Iutils/cqueue \
    -Iutils/heap \
    -Iservices/bone_sensor \
    -Iservices/audioflinger \
    -Iservices/audio_process \
    -Iservices/audio_dump/include \
    -Iutils/lockcqueue \
    -Iapps/key \
    -Iapps/main \
    -Iapps/common \
    -Iapps/audioplayers \
    -Iapps/audioplayers/a2dp_decoder \
    -Iapps/anc/inc \
    -Iapps/anc_assist/inc \
    -Iapps/anc_assist/src/assist \
    -Iapps/factory \
    -Iapps/ble_audio_central_app/ble_audio_usb \
    -Iservices/mcx \
    -Iservices/interconnection/green \
    -Iutils/hwtimer_list \
    -Ithirdparty/audio_codec_lib/liblhdc-dec/inc \
    -Iservices/nv_section/log_section \
    -Iservices/ai_voice/manager \
    -Iservices/app_ai/inc \
    -Iapps/anc/src/assist \
    -Iapps/anc_assist/inc \
    -Iapps/anc_assist/src/assist/ \
    -Iapps/battery/ \
    -Iutils/crc \
    -Iutils/signal_generator \
    -Iutils/rom_utils \
    $(AUDIO_ALGORITHMS_INCLUDES) \
    -Iservices/bt_app/a2dp_codecs/include \
    -Iservices/ai_voice/protocol/gva/gsound_target \
    -Iservices/ibrt/ibrt_middleware/inc \
    -Iservices/ota \
    -Iservices/audioflinger \
    -Iservices/norflash_api \
    -Iservices/mcpp \
    -Iutils/list \
    -Iutils/kfifo \
    -Iutils/cfifo \
    -Iapps/bt_sync \
    -Iservices/bt_sync \
    -Iservices/audio_manager/inc \
    -Iapps/dsp_m55 \
    -Iapps/dsp_hifi4 \
    -Iplatform/drivers/hifi4 \
    -Iplatform/drivers/codec \
    -Ibthost/apps/earbuds_deprecated/inc/ \
    -Itests/anc_usb/ \
    $(AUDIO_ALGORITHMS_INCLUDES) \
    -Imultimedia/inc/audio/process/sbcplc/include \
    -Imultimedia/inc/audio/process/audio_ramp/ \
    -Imultimedia/inc/audio/process/filters/include/ \
    -Imultimedia/inc/speech/inc/ \
    -Ibthost/host/inc \
    -Ibthost/host/inc/bta/normal \
    -Ibthost/host/src/service/ble_audio/aob_app/inc/ \
    -Ibthost/host/inc/bta/bt/ \
    -Iapps/anc_assist/src/ \

subdir-ccflags-y += -Imultimedia/inc/
subdir-ccflags-y += -Imultimedia/inc/smf

ifneq ($(filter 1,$(VOICE_ASSIST_FIR_LMS_ENABLED) $(VOICE_ASSIST_FF_FIR_LMS)),)
subdir-ccflags-y += -Iapps/voice_assist/inc
endif

BLE_SMF ?= 0
ifeq ($(BLE_SMF),1)
subdir-ccflags-y += -DBLE_SMF
endif

BLE_LC3 ?= 0
ifeq ($(BLE_LC3),1)
subdir-ccflags-y += -DBLE_LC3
endif

GAF_MUSIC_PLC_ON ?= 1
ifeq ($(GAF_LC3_BES_PLC_ON),1)
KBUILD_CPPFLAGS += -DGAF_LC3_BES_PLC_ON
else ifeq ($(GAF_MUSIC_PLC_ON),1)
KBUILD_CPPFLAGS += -DGAF_MUSIC_PLC_ON
endif

ifeq ($(BINAURAL_RECORD_PROCESS),1)
subdir-ccflags-y += -Iservices/binaural_record_process
CFLAGS_gaf_media_stream.o += -DBINAURAL_RECORD_PROCESS
CFLAGS_gaf_stream_process.o += -DBINAURAL_RECORD_PROCESS
endif

ifeq ($(CODEC_DAC_MULTI_VOLUME_TABLE),1)
CFLAGS_gaf_media_stream.o += -DCODEC_DAC_MULTI_VOLUME_TABLE
CFLAGS_aob_volume_api.o += -DCODEC_DAC_MULTI_VOLUME_TABLE
endif
