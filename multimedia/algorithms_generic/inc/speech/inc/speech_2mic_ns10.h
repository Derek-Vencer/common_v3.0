#ifndef __SPEECH_2MIC_NS10_H__
#define __SPEECH_2MIC_NS10_H__

#include <stdint.h>
#include "custom_allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t     bypass;

    float       dist;
    int32_t     ref_delay;

    int32_t     wind_supp_enable;
    int32_t     echo_supp_enable;
    int32_t     clip_enable;
    int32_t     post_supp_enable;
    int32_t     wdrc_enable;

    float       pre_gain;
    float       post_gain;

    float       denoise_dB;
    float       coh_denoise_dB;
} Speech2MicNs10Config;

struct Speech2MicNs10State_;

typedef struct Speech2MicNs10State_ Speech2MicNs10State;

Speech2MicNs10State *speech_2mic_ns10_create(int32_t sample_rate, int32_t frame_size, Speech2MicNs10Config *cfg, custom_allocator *allocator);

int32_t speech_2mic_ns10_set_config(Speech2MicNs10State *st, const Speech2MicNs10Config *cfg);

int32_t speech_2mic_ns10_destroy(Speech2MicNs10State *st);

int32_t speech_2mic_ns10_process(Speech2MicNs10State *st, int16_t *pcm_buf, int16_t *ref_buf, int32_t pcm_len, int16_t *out_buf);

float speech_2mic_ns10_get_required_mips(Speech2MicNs10State *st);

int32_t speech_2mic_ns10_get_delay(Speech2MicNs10State *st);

#ifdef __cplusplus
}
#endif

#endif