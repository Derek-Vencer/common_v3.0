#ifndef __SPEECH_3MIC_NS2_H__
#define __SPEECH_3MIC_NS2_H__

#include <stdint.h>
#include "speech_common.h"
#include "custom_allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t     bypass;
    int32_t     wnr_enable;
    int32_t     wind_band_num;
    int32_t     wind_band_freq[MAX_WIND_NUM];
    float       wind_mic_switch_thr;
    //vad (use AI+vpu) 
    int32_t     vpu_bin_start;
    int32_t     vpu_bin_end;
    float       vad_speech_prob_thd;
    float       update_snr;
    //af
    int32_t     outer_echo_af_enable;
    // float       outer_af_error_threshold;
    // float       outer_ff_error_threshold;
    // float       vpu_af_error_threshold;
    // int32_t     outer_af_filter_len;
    // int32_t     vpu_af_filter_len;
    //nlp
    int32_t     echo_supp_enable;
    int32_t     ref_delay;
    int32_t     outer_echo_band_start;
    int32_t     outer_echo_band_end;
    int32_t     vpu_echo_band_start;
    int32_t     vpu_echo_band_end;

    int32_t     post_supp_enable;
    float       pf_denoise_db;
    //change with mode
    float       inner_denoise_db;
    float       vpu_energy_diff_min;
    float       vpu_energy_diff_max;
    // crossover
    int32_t     blend_en;
    int32_t     blend_mix_start;
    int32_t     blend_mix_end;
    int32_t     noise_blend_band;
    float       blend_noise_power_thd;
    float       blend_snr_thd;

    int32_t     comp_num;
    int32_t     comp_freq[MAX_COMP_NUM];
    int32_t     comp_gaindB[MAX_COMP_NUM];
    
    // wdrc
    int32_t     wdrc_enable;
    // gain
    float       pre_gain;
    float       ref_pre_scale;
    float       post_gain;
    // vad
    float       comp_speech_prob_thd;
} Speech3MicNs2Config;

struct Speech3MicNs2State_;

typedef struct Speech3MicNs2State_ Speech3MicNs2State;

Speech3MicNs2State *speech_3mic_ns2_create(void *modeldata, int sample_rate, int frame_size, const Speech3MicNs2Config *cfg, custom_allocator *allocator);

int32_t speech_3mic_ns2_destroy(Speech3MicNs2State *st);

int32_t speech_3mic_ns2_process(Speech3MicNs2State *st, short *pcm_buf, short *ref_buf, int32_t pcm_len, short *out_buf);

int speech_3mic_ns2_set_config(Speech3MicNs2State *st, const Speech3MicNs2Config *cfg);

float speech_3mic_ns2_get_required_mips(Speech3MicNs2State *st);

bool speech_3mic_ns2_get_wind_status(Speech3MicNs2State *st);

void speech_3mic_ns2_set_af_state(Speech3MicNs2State *st, void *af, FILTER_RESET_HANDLER handler);

#ifdef __cplusplus
}
#endif

#endif