#ifndef __SPEECH_1MIC_PREAF_H__
#define __SPEECH_1MIC_PREAF_H__

#include <stdint.h>
#include "custom_allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t     bypass;
    int32_t     ref_delay;
    float       error_threshold;
    int32_t     filter_len;
    float       ref_pre_scale;
} Speech1MicPreafConfig;

struct Speech1MicPreafState_;

typedef struct Speech1MicPreafState_ Speech1MicPreafState;

Speech1MicPreafState *speech_1mic_preaf_create(int32_t sample_rate, int32_t frame_size, Speech1MicPreafConfig *cfg, custom_allocator *allocator);

int32_t speech_1mic_preaf_destroy(Speech1MicPreafState *st);

int32_t speech_1mic_preaf_process(Speech1MicPreafState *st, int16_t *pcm_buf, int16_t *ref_buf, int32_t pcm_len, int16_t *out_buf);

float speech_1mic_preaf_get_required_mips(Speech1MicPreafState *st);

int32_t speech_1mic_preaf_set_config(Speech1MicPreafState *st, const Speech1MicPreafConfig *cfg);

void speech_1mic_preaf_reset_coef(void *st);

#ifdef __cplusplus
}
#endif

#endif