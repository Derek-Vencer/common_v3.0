/***************************************************************************
 *
 * Copyright 2015-2023 BES.
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
#ifndef AF_STREAM_SW_GAIN_H
#define AF_STREAM_SW_GAIN_H

#include "stdbool.h"
#include "hal_aud.h"
#include "audio_ramp.h"

#ifdef __cplusplus
extern "C" {
#endif

// #define ENABLE_CAPTURE_RAMP

enum AF_STREAM_SW_GAIN_POS_T {
    AF_STREAM_SW_GAIN_POS_NORMAL = 0,
    AF_STREAM_SW_GAIN_POS_ALT,
};

enum AF_STREAM_SW_GAIN_ADD_USER_T {
    AF_STREAM_SW_GAIN_PROMPT_USER = 0,
    AF_STREAM_SW_GAIN_ADAPTIVE_VOLUME_USER,

    AF_STREAM_SW_GAIN_ADD_USER_NUM,
};

enum AF_STREAM_SW_GAIN_TYPE_T {
    AF_STREAM_SW_GAIN_TYPE_LINEAR = RAMP_LINEAR,
    AF_STREAM_SW_GAIN_TYPE_EXP = RAMP_EXPONENTIAL,
    AF_STREAM_SW_GAIN_TYPE_SMOOTHSTEP = RAMP_SMOOTHSTEP,
    AF_STREAM_SW_GAIN_TYPE_IIR = RAMP_IIR,
    AF_STREAM_SW_GAIN_TYPE_IIR2 = RAMP_IIR2,
    AF_STREAM_SW_GAIN_TYPE_LIMITER,
};

#define DEFAULT_SW_GAIN_TYPE     (AF_STREAM_SW_GAIN_TYPE_SMOOTHSTEP)

#ifdef AF_STREAM_SW_GAIN_ENABLE
/**
 * @brief Call this function when init software gain module
*/
void af_stream_sw_gain_init(void);

/**
 * @brief Call this function when open stream
 *
 * @param id            Stream ID used by audioflinger.
 * @param stream        Stream direction (playback or capture).
 * @param sample_rate   Sample rate in Hz.
 * @param channel_num   Number of audio channels.
 * @param bits          Bit depth per sample (e.g., 16, 24, 32).
 * @param chan_sep_flag AudioFlinger stream channel separete flag.
 * @param type          Software gain type.
 *
 * @return 0 on success, negative value on failure.
 */
int32_t af_stream_sw_gain_open(enum AUD_STREAM_ID_T id,
                            enum AUD_STREAM_T stream,
                            uint32_t sample_rate,
                            uint32_t channel_num,
                            uint32_t bits,
                            bool chan_sep_flag,
                            enum AF_STREAM_SW_GAIN_TYPE_T type);

/**
 * @brief Call this function when close stream
 *
 * @param id Audioflinger stream id
 * @param stream Audioflinger stream
*/
void af_stream_sw_gain_close(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream);

/**
 * @brief Call this function when setting the software gain coefficient
 *
 * @param id Audioflinger stream id
 * @param stream Audioflinger stream
 * @param gain The software gain coefficient
*/
void af_stream_sw_gain_set_gain_coef(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream, float gain);

/**
 * @brief Call this function when setting the software max gain coefficient
 * Some algorithms need to limit the maximum volume of software gain.
 *
 * @param id Audioflinger stream id
 * @param stream Audioflinger stream
 * @param max_gain The software max gain coefficient
*/
void af_stream_sw_gain_set_max_gain_coef(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream, float max_gain);

/**
 * @brief Call this function when setting the software volume level
 *
 * @param id Audioflinger stream id
 * @param stream Audioflinger stream
 * @param vol The volume level
*/
void af_stream_sw_gain_set_volume_level(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream, uint32_t vol);

/**
 * @brief Call this function when changing smoothing time in linear mode
 *
 * @param id Audioflinger stream id
 * @param stream Audioflinger stream
 * @param time_ms The new smoothing time
*/
void af_stream_sw_gain_set_smooth_time(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream, uint32_t time_ms);

/**
 * @brief Call this function when getting the software gain coefficient
 *
 * @param id Audioflinger stream id
 * @param stream Audioflinger stream
 * @return The software gain coefficient
*/
float af_stream_sw_gain_get_gain_coef(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream);

/**
 * @brief Get the current volume value.
 *
 * @param id       Stream ID.
 * @param stream   Stream direction.
 *
 * @return Current volume value.
 */
uint32_t af_stream_sw_gain_get_volume(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream);

/**
 * @brief Set an extra gain factor applied on top of the main gain.
 *
 * Can be used for temporary gain scaling (e.g., ducking or boost).
 *
 * @param id           Stream ID.
 * @param stream       Stream direction.
 * @param user         Additional gain control user.
 * @param extra_gain   Additional gain factor.
 */
void af_stream_sw_gain_set_extra_gain(enum AUD_STREAM_ID_T id,
                                    enum AUD_STREAM_T stream,
                                    enum AF_STREAM_SW_GAIN_ADD_USER_T user,
                                    float extra_gain);

/**
 * @brief Check whether the ramping operation has completed.
 *
 * @param id       Stream ID.
 * @param stream   Stream direction.
 *
 * @return true if ramping is done; false otherwise.
 */
bool af_stream_sw_gain_is_finished(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream);

/**
 * @brief Set the current active ramp position.
 *
 * This controls which caller is allowed to invoke af_stream_sw_gain_playback_process().
 * Only the caller matching the current position is permitted to execute processing.
 *
 * @param id       Stream ID.
 * @param stream   Stream direction.
 * @param pos      Ramp processing position (NORMAL or ALT).
 */
void af_stream_sw_gain_set_position(enum AUD_STREAM_ID_T id,
                                enum AUD_STREAM_T stream,
                                enum AF_STREAM_SW_GAIN_POS_T pos);

/**
 * @brief Get the current ramp processing position for the specified stream.
 *
 * This function returns the position currently allowed to call
 * af_stream_sw_gain_playback_process().
 *
 * @param id       Stream ID.
 * @param stream   Stream direction.
 * @return         Current ramp position (AF_STREAM_SW_GAIN_POS_NORMAL or AF_STREAM_SW_GAIN_POS_ALT).
 */
enum AF_STREAM_SW_GAIN_POS_T af_stream_sw_gain_get_position(enum AUD_STREAM_ID_T id,
                                                 enum AUD_STREAM_T stream);

/**
 * @brief Apply ramp processing to capture stream data.
 *
 * This should be called in the audioflinger capture path to apply volume/gain control.
 *
 * @param id       Stream ID.
 * @param stream   Stream direction.
 * @param buf      Audio buffer (in-place modification).
 * @param len      Length of buffer in bytes.
 */
void af_stream_sw_gain_capture_process(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream, uint8_t *buf, uint32_t len);

/**
 * @brief Apply ramp processing to playback stream data.
 *
 * This should be called in the audioflinger playback path to apply volume/gain control.
 *
 * @param id       Stream ID.
 * @param stream   Stream direction.
 * @param buf      Audio buffer (in-place modification).
 * @param len      Length of buffer in bytes.
 */
void af_stream_sw_gain_playback_process(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream, uint8_t *buf, uint32_t len);

#ifdef RTOS
/**
 * @brief Set audioflinger stream start flag.
 *
 * This should be called in the audioflinger interrupt callback.
 *
 * @param id       Stream ID.
 * @param stream   Stream direction.
 */
void af_stream_sw_gain_set_stream_start_flag(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream);

/**
 * @brief Perform a fade-out before the stream is closed.
 *
 * The stream will smoothly ramp down to silence before closing.
 *
 * @param id        Stream ID.
 * @param stream    Stream direction.
 * @param time_ms   Duration of fade-out in milliseconds.
 */
void af_stream_sw_gain_playback_before_close_fadeout(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream, uint32_t time_ms);

/**
 * @brief Apply fadeout processing to playback stream data before stream stop.
 *
 * This should be called in the audioflinger playback path end.
 *
 * @param id       Stream ID.
 * @param stream   Stream direction.
 * @param buf      Audio buffer (in-place modification).
 * @param len      Length of buffer in bytes.
 */
void af_stream_sw_gain_playback_fadeout_process(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream, uint8_t *buf, uint32_t len);
#else
static inline void af_stream_sw_gain_set_stream_start_flag(enum AUD_STREAM_ID_T id,
                                    enum AUD_STREAM_T stream) { }

static inline void af_stream_sw_gain_playback_before_close_fadeout(enum AUD_STREAM_ID_T id,
                                    enum AUD_STREAM_T stream,
                                    uint32_t time_ms) { }

static inline void af_stream_sw_gain_playback_fadeout_process(enum AUD_STREAM_ID_T id,
                                    enum AUD_STREAM_T stream,
                                    uint8_t *buf,
                                    uint32_t len) { }
#endif
#else
static inline void af_stream_sw_gain_init(void) { }

static inline int32_t af_stream_sw_gain_open(enum AUD_STREAM_ID_T id,
                                    enum AUD_STREAM_T stream,
                                    uint32_t sample_rate,
                                    uint32_t channel_num,
                                    uint32_t bits,
                                    bool chan_sep_flag,
                                    enum AF_STREAM_SW_GAIN_TYPE_T type) { return 0; }

static inline void af_stream_sw_gain_close(enum AUD_STREAM_ID_T id,
                                    enum AUD_STREAM_T stream) { }

static inline void af_stream_sw_gain_set_gain_coef(enum AUD_STREAM_ID_T id,
                                    enum AUD_STREAM_T stream,
                                    float gain) { }

static inline void af_stream_sw_gain_set_max_gain_coef(enum AUD_STREAM_ID_T id,
                                    enum AUD_STREAM_T stream,
                                    float max_gain) { }

static inline void af_stream_sw_gain_set_volume_level(enum AUD_STREAM_ID_T id,
                                    enum AUD_STREAM_T stream,
                                    uint32_t vol) { }

static inline void af_stream_sw_gain_set_smooth_time(enum AUD_STREAM_ID_T id,
                                    enum AUD_STREAM_T stream,
                                    uint32_t time_ms) { }

static inline float af_stream_sw_gain_get_gain_coef(enum AUD_STREAM_ID_T id,
                                            enum AUD_STREAM_T stream) { return 1.0; }

static inline uint32_t af_stream_sw_gain_get_volume(enum AUD_STREAM_ID_T id,
                                    enum AUD_STREAM_T stream) { return TGT_VOLUME_LEVEL_MAX; }

static inline void af_stream_sw_gain_set_extra_gain(enum AUD_STREAM_ID_T id,
                                    enum AUD_STREAM_T stream,
                                    enum AF_STREAM_SW_GAIN_ADD_USER_T user,
                                    float extra_gain) { }

static inline bool af_stream_sw_gain_is_finished(enum AUD_STREAM_ID_T id,
                                    enum AUD_STREAM_T stream) { return true; }

static inline void af_stream_sw_gain_set_position(enum AUD_STREAM_ID_T id,
                                    enum AUD_STREAM_T stream,
                                    enum AF_STREAM_SW_GAIN_POS_T pos) { }

static inline enum AF_STREAM_SW_GAIN_POS_T af_stream_sw_gain_get_position(enum AUD_STREAM_ID_T id,
                                    enum AUD_STREAM_T stream) { return AF_STREAM_SW_GAIN_POS_NORMAL; }

static inline void af_stream_sw_gain_capture_process(enum AUD_STREAM_ID_T id,
                                    enum AUD_STREAM_T stream,
                                    uint8_t *buf,
                                    uint32_t len) { }

static inline void af_stream_sw_gain_playback_process(enum AUD_STREAM_ID_T id,
                                    enum AUD_STREAM_T stream,
                                    uint8_t *buf,
                                    uint32_t len) { }

static inline void af_stream_sw_gain_set_stream_start_flag(enum AUD_STREAM_ID_T id,
                                    enum AUD_STREAM_T stream) { }

static inline void af_stream_sw_gain_playback_before_close_fadeout(enum AUD_STREAM_ID_T id,
                                    enum AUD_STREAM_T stream,
                                    uint32_t time_ms) { }

static inline void af_stream_sw_gain_playback_fadeout_process(enum AUD_STREAM_ID_T id,
                                    enum AUD_STREAM_T stream,
                                    uint8_t *buf,
                                    uint32_t len) { }
#endif

#ifdef __cplusplus
}
#endif

#endif /* AF_STREAM_SW_GAIN_H */
