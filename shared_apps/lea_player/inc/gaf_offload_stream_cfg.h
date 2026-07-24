#ifndef __GAF_OFFLOAD_CFG_H__
#define __GAF_OFFLOAD_CFG_H__
#include "gaf_media_common.h"



void codec_open_test(void);
bool gaf_offload_audio_stream_start(GAF_MEDIA_DWELLING_INFO_T* pDewellingInfo, GAF_AUDIO_STREAM_ENV_T* pStreamEnv, uint8_t con_lid);
bool gaf_mobile_offload_audio_stream_start(GAF_MEDIA_DWELLING_INFO_T* pDewellingInfo, GAF_AUDIO_STREAM_ENV_T* pStreamEnv, uint8_t con_lid);
void gaf_audio_flexible_playback_dma_dsp_start_handler(uint8_t con_lid);
void gaf_audio_flexible_capture_dma_dsp_start_handler(uint8_t con_lid);
void gaf_mobile_offload_stop_playback_stream(void);
void gaf_mobile_offload_stop_capture_stream(void);
void gaf_offload_stop_playback_stream(GAF_AUDIO_STREAM_ENV_T* pStreamEnv);
void gaf_offload_stop_capture_stream(GAF_AUDIO_STREAM_ENV_T* pStreamEnv);
void gaf_offload_close_cp(void);
void gaf_mobile_offload_close_cp(void);
void gaf_offload_capture_vol_change(float gain);
void gaf_offload_playback_vol_change(uint8_t gain);
#endif /// __GAF_OFFLOAD_CFG_H__