#pragma once
#include <stdint.h>

#ifndef EXTERNC
#ifdef __cplusplus
#define EXTERNC extern "C" 
#else
#define EXTERNC
#endif
#endif


#define PLC_API_RET_SUCCESS			(0)
#define PLC_API_RET_INPUT_ERROR		(-1)
#define PLC_API_RET_FAIL			(-2)
#define PLC_API_RET_NOT_SUPPORT		(-3)

#define PLC_API_KEY					0


enum plc_api_data_type_e {
	PLC_API_DATA_TYPE_SHORT_16 = 0,
	PLC_API_DATA_TYPE_INT_32,
	PLC_API_DATA_TYPE_FLOAT_32,
	PLC_API_DATA_TYPE_MAX,
};

enum plc_api_mode_e {
	PLC_API_MODE_MUSIC_PLC = 0,
	PLC_API_MODE_SBC_PLC,
	PLC_API_MODE_TS_PLC,
	PLC_API_MODE_MAX,
};

enum plc_api_param_set_e {
	PLC_API_PARAM_SET_KEY_APP = 0,
	PLC_API_PARAM_SET_PARAM,
	PLC_API_PARAM_SET_MAX,
};

enum plc_api_application_e {
	PLC_API_APPLICATION_VOICE = 0,
	PLC_API_APPLICATION_MUSIC,
	PLC_API_APPLICATION_192K,
	PLC_API_APPLICATION_LOW_LATENCY,
	PLC_API_APPLICATION_LFE,
	PLC_API_APPLICATION_AUTO,
	PLC_API_APPLICATION_MAX,
};

enum plc_api_set_e {
	PLC_API_SET_NOONE = 0,
	PLC_API_SET_MAX,
};

enum plc_api_get_e {
	PLC_API_GET_HEAP_SIZE = 0,
	PLC_API_GET_MAX,
};

enum sbc_plc_codec_type_e{
	SBC_PLC_CODEC_TYPE_SBC = 0,
	SBC_PLC_CODEC_TYPE_AAC = 1,
	SBC_PLC_CODEC_TYPE_LC3 = 2,
	SBC_PLC_CODEC_TYPE_CVSD = 3,
	SBC_PLC_CODEC_TYPE_AMR_NB = 4,
	SBC_PLC_CODEC_TYPE_AMR_WB = 5,
	SBC_PLC_CODEC_TYPE_MAX,
};

typedef struct {
	int32_t fs_hz;
	int16_t channels;
	int16_t frame_samples;
	uint16_t channel_select;
	enum plc_api_data_type_e data_type;
	enum plc_api_mode_e mode;
	enum plc_api_param_set_e param_set;
		struct{
			int32_t key;
			enum plc_api_application_e application;
			union {
				struct{
					int32_t overlap_samples;
					int32_t hold_samples_after_lost;
					int32_t attenuate_samples_after_lost;
					int32_t gain_samples_after_no_lost;
					int32_t seek_samples;
					int32_t no_seek_samples;
					int32_t match_samples;
					bool force_mute_together;
				}music_plc;
				struct {
					enum sbc_plc_codec_type_e codec_type;
					int32_t search_size;
					int32_t match_size;
					int32_t olal;
					int32_t recover;
				}sbc_plc;
				struct {
					int32_t recv;
				}ts_plc;
			};
	};
	void* (*cb_malloc)(int size);
	void (*cb_free)(void* ptr);
	void (*cb_printf)(const char *fmt, ...);
	void* other;
}plc_api_param_t;


typedef int32_t plc_api_ret_t;

EXTERNC plc_api_ret_t plc_api_create(void** p_hd, plc_api_param_t* param);
EXTERNC plc_api_ret_t plc_api_run(void* hd, uint8_t* in, int32_t in_len, int32_t* in_used, uint8_t* out, int32_t* p_out_len, uint16_t is_lost);
EXTERNC plc_api_ret_t plc_api_set(void* hd, plc_api_set_e choose, void* val);
EXTERNC plc_api_ret_t plc_api_get(void* hd, plc_api_get_e choose, void* val);
EXTERNC plc_api_ret_t plc_api_destory(void* hd);

EXTERNC void plc_api_register_music_plc_int16(void);
EXTERNC void plc_api_register_music_plc_int32(void);
EXTERNC void plc_api_register_music_plc_f32(void);

EXTERNC void plc_api_register_sbc_plc_i16(void);

EXTERNC void plc_api_register_ts_plc_i16(void);

