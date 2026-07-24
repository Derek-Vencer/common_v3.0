#pragma once
#include "stdint.h"
#include "custom_allocator.h"

#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif

EXTERNC struct SpeechAlgoConfig;
EXTERNC int speech_algo_tx_open(int32_t sample_rate, int32_t frame_len, custom_allocator *allocator);
EXTERNC int speech_algo_rx_open(int32_t sample_rate, int32_t frame_len, custom_allocator *allocator);
EXTERNC int speech_algo_tx_process(int16_t *pcm_buf, int16_t *ref_buf, int16_t pcm_len, int16_t *out);
EXTERNC int speech_algo_rx_process(int16_t *pcm_buf, int16_t pcm_len, int16_t *out);
EXTERNC int speech_algo_tx_close(void);
EXTERNC int speech_algo_rx_close(void);
EXTERNC int speech_store_config(void *cfg, uint16_t cfg_len);
