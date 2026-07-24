/***************************************************************************
 *
 * Copyright 2015-2025 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifndef AUDIO_RAMP_H
#define AUDIO_RAMP_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#define AUDIO_RAMP_VERSION "RAMP_V2.0"

// #define AUDIO_RAMP_DEBUG_TRACE
// #define USE_CMSIS_FOR_IIR2
#define MAX_RAMPS (20)

// Audio Ramp Error Codes
#define AUDIO_RAMP_OK                      (0)
#define AUDIO_RAMP_ERROR_HANDLE_INVALID   (-1)
#define AUDIO_RAMP_ERROR_INPUT_INVALID    (-2)
#define AUDIO_RAMP_ERROR_HANDLE_NOACTIVE  (-3)
#define AUDIO_RAMP_ERROR_ALLOC_FAILED     (-4)

typedef int RampHandle;

typedef enum {
    RAMP_LINEAR,
    RAMP_EXPONENTIAL,
    RAMP_SMOOTHSTEP,
    RAMP_IIR,
    RAMP_IIR2
} RampType;

typedef struct {
    int sample_rate;
    int bit_depth;
    int num_channels;
    int ramp_time_ms;
    float initial_gain;
    RampType type;
    int silence_time_ms;
} RampConfig;

/**
 * @brief Call this function to get an audio ramp handle.
 *
 * @param cfg A configuration structure that defines sample rate, bit depth,
 *            initial gain, ramp type, and ramp duration.
 * @return RampHandle A valid handle if successful, negative error code otherwise. A valid RampHandle is greater than 0.
 */
RampHandle audio_ramp_open(const RampConfig* cfg);

/**
 * @brief Close an existing audio ramp handle and free its resources.
 *
 * @param handle The handle returned by audio_ramp_open().
 */
void audio_ramp_close(RampHandle handle);

/**
 * @brief Get the current target gain value for the ramp to reach.
 *
 * @param handle The handle returned by audio_ramp_open().
 * @param target Pointer to float where the current target gain will be stored.
 * @return 0 if successful, otherwise a negative error code.
 */
int32_t audio_ramp_get_target(RampHandle handle, float *target);

/**
 * @brief Set a new target gain value for the ramp to reach.
 *
 * @param handle The handle returned by audio_ramp_open().
 * @param new_target The new target gain value (usually between 0.0 and 1.0).
 * @return 0 if successful, otherwise a negative error code.
 */
int32_t audio_ramp_set_target(RampHandle handle, float new_target);

/**
 * @brief Set a new ramp transition time in milliseconds.
 *
 * @param handle The handle returned by audio_ramp_open().
 * @param new_time Ramp transition time in milliseconds.
 * @return 0 if successful, otherwise a negative error code.
 */
int32_t audio_ramp_set_ramp_time(RampHandle handle, int new_time);

/**
 * @brief Check whether the ramp has completed and the gain has reached the target.
 *
 * @param handle The handle returned by audio_ramp_open().
 * @return 1 if ramp is finished, 0 if not finished, negative error code if invalid.
 */
int audio_ramp_is_finished(RampHandle handle);

/**
 * @brief Process one audio buffer using gain ramping.
 *
 * @param handle The handle returned by audio_ramp_open().
 * @param in Input audio buffer.
 * @param out Output audio buffer.
 * @param len Data len to process.
 * @param interleave_flag 0 if data is planar, 1 if data is interleaved.
 * @return 0 if successful, otherwise a negative error code.
 */
int32_t audio_ramp_process(RampHandle handle, void *in, void *out, uint32_t len, int32_t interleave_flag);

/**
 * @brief Process one audio buffer using gain ramping separate channel.
 *
 * @param handle The handle returned by audio_ramp_open().
 * @param in Input audio buffer.
 * @param out Output audio buffer.
 * @param len Data len per channel to process.
 * @return 0 if successful, otherwise a negative error code.
 */
int32_t audio_ramp_process_sep_chan(RampHandle handle, void **in, void **out, uint32_t len);

/**
 * @brief Mix two input buffers with complementary ramp gain and output to one or two buffers.
 *
 * This function performs a crossfade between two audio inputs using the current ramp gain.
 * The gain `g` is retrieved from the ramp instance and applied as:
 *   - out1 = in1 * g + in2 * (1 - g)
 *   - out2 = in1 * (1 - g) + in2 * g
 *
 * Either `out1` or `out2` can be NULL if that output is not needed.
 *
 * @param handle The handle returned by audio_ramp_open().
 * @param in1 First input buffer.
 * @param in2 Second input buffer.
 * @param out1 Output buffer for the first mix (can be NULL).
 * @param out2 Output buffer for the second mix (can be NULL).
 * @param len Data len to process.
 * @param interleave_flag 0 if data is planar, 1 if interleaved.
 * @return 0 if successful, otherwise a negative error code.
 */
int32_t audio_ramp_mix_process(RampHandle handle, void *in1, void *in2, void *out1, void *out2, uint32_t len, int32_t interleave_flag);

#ifdef __cplusplus
}
#endif

#endif
