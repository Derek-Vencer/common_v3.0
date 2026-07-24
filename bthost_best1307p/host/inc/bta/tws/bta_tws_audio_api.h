/****************************************************************************
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

#pragma once

#include "bt_base_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 ****************************************************************************************
 *    ____  _             _             _ _            _    ____ ___
 *   | __ )| |_ __ _     / \  _   _  __| (_) ___      / \  |  _ \_ _|
 *   |  _ \| __/ _` |   / _ \| | | |/ _` | |/ _ \    / _ \ | |_) | |
 *   | |_) | || (_| |  / ___ \ |_| | (_| | | (_) |  / ___ \|  __/| |
 *   |____/ \__\__,_| /_/   \_\__,_|\__,_|_|\___/  /_/   \_\_|  |___|
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief       Get current audio focus.
 * @return      see@bt_audio_focus_type_t
 ****************************************************************************************
 */
int bta_tws_get_curr_audio_focus_type(void);

/**
 ****************************************************************************************
 * @brief       Switch streaming sco.
 * @return      none
 ****************************************************************************************
 */
void bta_tws_switch_streaming_sco(void);

/**
 ****************************************************************************************
 * @brief       Switch streaming a2dp.
 * @return      none
 ****************************************************************************************
 */
void bta_tws_switch_streaming_a2dp(void);

/**
 ****************************************************************************************
 * @brief       Switch the audio stream from a2dp to cis.
 * @return      none
 ****************************************************************************************
 */
void bta_tws_toggle_a2dp_cis(void);

/**
 ****************************************************************************************
 * @brief       Switch the audio stream from a2dp to cis.
 * @return      none
 ****************************************************************************************
 */
void bta_tws_hold_background_switch(void);

/**
 ****************************************************************************************
 * @brief       Local volume up.
 * @return      none
 ****************************************************************************************
 */
void bta_tws_set_local_volume_up(void);

/**
 ****************************************************************************************
 * @brief       Local volume down.
 * @return      none
 ****************************************************************************************
 */
void bta_tws_set_local_volume_down(void);

/**
 ****************************************************************************************
 * @brief       Local volume up by volume step.
 * @return      none
 ****************************************************************************************
 */
void bta_tws_set_local_volume_up_by_step(uint8_t vol_step);

/**
 ****************************************************************************************
 * @brief       Local volume down by volume step.
 * @return      none
 ****************************************************************************************
 */
void bta_tws_set_local_volume_down_by_step(uint8_t vol_step);

/**
 ****************************************************************************************
 * @brief       Check if there are any sco connections.
 * @return      -true: has sco connected.
 *              -false: not any sco.
 ****************************************************************************************
 */
bool bta_tws_is_sco_active(void);

/**
 ****************************************************************************************
 * @brief       Check if there are any call active.
 * @return      -true: has call active.
 *              -false: not any call active.
 ****************************************************************************************
 */
bool bta_tws_is_call_active(void);

/**
 ****************************************************************************************
 * @brief       Check if there are any music active.
 * @return      -true: has call active.
 *              -false: not any call active.
 ****************************************************************************************
 */
bool bta_tws_is_music_active(void);

/**
 ****************************************************************************************
 * @brief       Get current playing sco device id.
 * @return      device id
 ****************************************************************************************
 */
uint8_t bta_tws_get_curr_playing_sco(void);

/**
 ****************************************************************************************
 * @brief       Get current playing a2dp device id.
 * @return      device id
 ****************************************************************************************
 */
uint8_t bta_tws_get_curr_playing_a2dp(void);

/**
 ****************************************************************************************
 * @brief       Get current a2dp device id.
 * @note        Unlike bta_tws_get_curr_playing_a2dp, when no a2dp is playing, this
 *              function will return a non-0xff default value.
 * @return      device id
 ****************************************************************************************
 */
uint8_t bta_tws_get_curr_a2dp_device(void);

/**
 ****************************************************************************************
 * @brief       Get hfp device for user action.
 * @return      device id
 ****************************************************************************************
 */
uint8_t bta_tws_get_hfp_device_for_user_action(void);

#ifdef __cplusplus
}
#endif
