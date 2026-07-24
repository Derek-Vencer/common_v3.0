#ifndef __SPEECH_2MIC_NS4_PREAF_H__
#define __SPEECH_2MIC_NS4_PREAF_H__

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
    int32_t     outer_echo_af_enable;
    float       ff_error_threshold;
    float       fb_error_threshold;
    int32_t     ff_filter_len;
    int32_t     fb_filter_len;
    int32_t     ff_fb_calib_enable;
    const float*      fb_filter;
    float       ref_pre_scale;
} Speech2MicNs4PreafConfig;

struct Speech2MicNs4PreafState_;

typedef struct Speech2MicNs4PreafState_ Speech2MicNs4PreafState;

Speech2MicNs4PreafState *speech_2mic_ns4_preaf_create(int32_t sample_rate, int32_t frame_size, Speech2MicNs4PreafConfig *cfg, custom_allocator *allocator);

int32_t speech_2mic_ns4_preaf_destroy(Speech2MicNs4PreafState *st);

int32_t speech_2mic_ns4_preaf_process(Speech2MicNs4PreafState *st, int16_t *pcm_buf, int16_t *ref_buf, int32_t pcm_len, int16_t *out_buf);

float speech_2mic_ns4_preaf_get_required_mips(Speech2MicNs4PreafState *st);

int32_t speech_2mic_ns4_preaf_set_config(Speech2MicNs4PreafState *st, const Speech2MicNs4PreafConfig *cfg);

void speech_2mic_ns4_preaf_reset_coef(void *state);

int32_t speech_2mic_ns4_preaf_set_wind_status(Speech2MicNs4PreafState *st, bool wind);

#ifdef __cplusplus
}
#endif

#endif