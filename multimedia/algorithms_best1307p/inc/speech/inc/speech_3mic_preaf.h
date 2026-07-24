#ifndef __SPEECH_3MIC_PREAF_H__
#define __SPEECH_3MIC_PREAF_H__

#include <stdint.h>
#include <stdbool.h>
#include "custom_allocator.h"
#include "speech_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t     bypass;
    int32_t     ref_delay;
    //af
    int32_t     outer_echo_af_enable;
    float       talk_af_error_threshold;
    float       ff_af_error_threshold;
    float       fb_af_error_threshold;
    int32_t     outer_af_filter_len;
    int32_t     fb_af_filter_len;
    int32_t     ff_fb_calib_enable;
    const float*      fb_filter;
    float       ref_pre_scale;
} Speech3MicPreafConfig;

struct Speech3MicPreafState_;

typedef struct Speech3MicPreafState_ Speech3MicPreafState;

Speech3MicPreafState *speech_3mic_preaf_create(int32_t sample_rate, int32_t frame_size, Speech3MicPreafConfig *cfg, custom_allocator *allocator);

int32_t speech_3mic_preaf_destroy(Speech3MicPreafState *st);

int32_t speech_3mic_preaf_process(Speech3MicPreafState *st, int16_t *pcm_buf, int16_t *ref_buf, int32_t pcm_len, int16_t *out_buf);

float speech_3mic_preaf_get_required_mips(Speech3MicPreafState *st);

int32_t speech_3mic_preaf_set_config(Speech3MicPreafState *st, const Speech3MicPreafConfig *cfg);

int32_t speech_3mic_preaf_reset_config_with_mode(Speech3MicPreafState *st, const Speech3MicPreafConfig *cfg);

void speech_3mic_preaf_reset_coef(void *state);

int32_t speech_3mic_preaf_set_wind_status(Speech3MicPreafState *st, bool wind);

#ifdef __cplusplus
}
#endif

#endif