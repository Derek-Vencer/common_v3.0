#ifndef ADAPTIVE_VOLUME_H
#define ADAPTIVE_VOLUME_H

#include <stdint.h>
#include "custom_allocator.h"

typedef struct {
    bool    debug;
    float   CT;
    float   CS;
    float   WT;
    float   WS;
    float   ET;
    float   ES;
    float   attack_time;
    float   release_time;
    float   makeup_gain;
    float   smooth_factor;
} AdaptiveVolumeConfig;

typedef struct AdaptiveVolumeState_ AdaptiveVolumeState;

#ifdef __cplusplus
extern "C" {
#endif

AdaptiveVolumeState *adaptive_volume_create(const AdaptiveVolumeConfig *config);

AdaptiveVolumeState *adaptive_volume_custom_allocator(const AdaptiveVolumeConfig *config, custom_allocator *allocator);

void adaptive_volume_switch(AdaptiveVolumeState *st);

float adaptive_volume_gain_estimate(AdaptiveVolumeState *st, float ff_noise_energy_db, float ref_energy_db);

void adaptive_volume_destroy(AdaptiveVolumeState* st);

void adaptive_volume_set_config(AdaptiveVolumeState *st, const AdaptiveVolumeConfig *cfg);
#ifdef __cplusplus
}
#endif

#endif