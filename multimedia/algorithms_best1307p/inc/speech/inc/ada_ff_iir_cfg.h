#ifndef __ADA_FF_IIR_CFG_H__
#define __ADA_FF_IIR_CFG_H__

#include "stdint.h"

typedef struct{
    uint32_t debug_en;
    float delta_thresh;
    float loss_thresh;
    int32_t max_cnt;
    float g_gain_delta;
    float g_gain_threshold;
    float gain_delta;
    float freq_delta;
    float q_delta;
    float mu;

    uint32_t start_freq;
    uint32_t end_freq;
    uint32_t start_filter_idx;
} ADA_FF_IIR_CFG_T;

#endif