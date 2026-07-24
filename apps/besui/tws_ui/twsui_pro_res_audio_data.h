/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifndef __RES_AUDIO_DATA_H__
#define __RES_AUDIO_DATA_H__

#ifdef MEDIA_PLAYER_SUPPORT
const uint8_t EN_POWER_ON [] = {
#include "res/en/voice_twspro/000_POWER_ON_16000_1_18_0.txt"
};

const uint8_t EN_POWER_OFF [] = {
#include "res/en/voice_twspro/001_POWER_OFF_16000_1_18_0.txt"
};

const uint8_t EN_BT_MAX_VOL[] = {
#include "res/en/voice_twspro/040_SND_VOLUME_MAX_16000_1_18_0.txt"
};

const uint8_t EN_BT_MIN_VOL[] = {
#include "res/en/voice_twspro/041_SND_VOLUME_MIN_16000_1_18_0.txt"
};

const uint8_t EN_BT_REFUSE[] = {
#ifndef BESUI_FW_LOG_WN
#include "res/en/voice_twspro/035_SND_REJECT_16000_1_18_0.txt"
#endif
};

const uint8_t EN_BT_HUNG_UP[] = {
#ifndef BESUI_FW_LOG_WN
#include "res/en/voice_twspro/035_SND_REJECT_16000_1_18_0.txt"
#endif
};

const uint8_t EN_BT_CONNECTED [] = {
#include "res/en/voice_twspro/CONNECTED_BLUETOOTH_16000_1_18_0.txt"
};

const uint8_t EN_CHARGE_PLEASE[] = {
#include "res/en/voice_twspro/008_LOW_BATTERY_16000_1_18_0.txt"
};

const uint8_t EN_BT_PAIR_ENABLE[] = {
#include "res/en/voice_twspro/002_PAIRING_16000_1_18_0.txt"
};

const uint8_t EN_BT_DIS_CONNECT [] = {
#include "res/en/voice_twspro/004_DEVICE_DISCONNECTED_16000_1_18_0.txt"
};

const uint8_t EN_BT_INCOMING_CALL [] = {
#ifndef BESUI_FW_LOG_WN
#include "res/en/voice_twspro/042_SND_RINGTONE_16000_1_18_0.txt"
#endif
};

const uint8_t EN_BT_ALEXA_STOP[] = {
#include "res/en/voice_twspro/FIND_ME_ALARM_16000_1_18_0.txt"
};

const uint8_t EN_BT_ANSWER[] = {
#ifdef BESUI_TWS_EN
#include "res/en/voice_twspro/034_SND_ACCEPT_16000_1_18_0.txt"
#endif
};

const uint8_t EN_ANC_ON_MODE[] = {
#if defined(ANC_APP)
#include "res/en/voice_twspro/071_STAY_AWARE_ON_16000_1_18_0.txt"
#endif
};

const uint8_t EN_ANC_AA_MODE[] = {
#if defined(ANC_APP)
#include "res/en/voice_twspro/071_STAY_AWARE_ON_16000_1_18_0.txt"
#endif
};

const uint8_t EN_BT_DOLBY[] = {
#if defined(DOLBY_AUDIO_ENABLE)
// #include "res/en/voice_twspro/SOUND_DOLBY_ON.txt"
#endif
};

const uint8_t EN_ANC_ST_MODE[] = {
// #include "res/en/voice_twspro/SOUND_ANC_ST.txt"
};

const uint8_t EN_FACTORY_RESET[] = {
#include "res/en/voice_twspro/FACTORY_RESET_16000_1_18_0.txt"
};

const uint8_t EN_TAP_SINGLE[] = {
#include "res/en/voice_twspro/027_SND_TAP_SINGLE_16000_1_18_0.txt"
};

const uint8_t EN_TAP_DOUBLE[] = {
#include "res/en/voice_twspro/027_SND_TAP_DOUBLE_16000_1_18_0.txt"
};

const uint8_t EN_TAP_TRIPLE[] = {
#include "res/en/voice_twspro/027_SND_TAP_TRIPLE_16000_1_18_0.txt"
};

const uint8_t EN_ENTER_SOUND[] = {
#include "res/en/voice_twspro/SND_ENTER_sound_exit_1_second_hold_16000_1_18_0.txt"
};

const uint8_t EN_EQ_MODE1[] = {
#include "res/en/voice_twspro/080_PODCAST_MODE_16000_1_18_0.txt"
};
const uint8_t EN_EQ_MODE2[] = {
#include "res/en/voice_twspro/081_CUSTOM_EQ_MODE_16000_1_18_0.txt"
};
const uint8_t EN_EQ_MODE3[] = {
#include "res/en/voice_twspro/082_MUSIC_MODE_16000_1_18_0.txt"
};
const uint8_t EN_EQ_MODE4[] = {
#include "res/en/voice_twspro/BASSBOOST_16000_1_18_0.txt"
};

const uint8_t EN_FMD_IDMODE[] = {
#if defined(BESUI_FMD_UI_EN)
#ifdef BESUI_TWS_EN
#include "res/en/voice_tws/SOUND_PAIR_ENABLE.txt"
#endif
#endif
};

const uint8_t EN_BT_BEASTGAME_MODE[] = {
#ifndef BESUI_FW_LOG_WN
// #include "res/en/voice_twspro/SOUND_BEASTMODE_GAME.txt"
#endif
};

const uint8_t EN_GAME_OFF[] = {
#ifndef BESUI_FW_LOG_WN
// #include "res/en/voice_twspro/SOUND_GAME_OFF.txt"
#endif
};

const uint8_t EN_TWS_CONN[] = {
// #include "res/en/voice_twspro/SOUND_TWS_CONN.txt"
};

const uint8_t EN_BT_SPATIAL_ON[] = {
#if defined(__VIRTUAL_SURROUND__) || defined(__VIRTUAL_SURROUND_STEREO__)
// #include "res/en/voice_twspro/SOUND_SPATIAL_ON.txt"
#endif
};

const uint8_t EN_BT_SPATIAL_OFF[] = {
// #include "res/en/voice_twspro/SOUND_SPATIAL_OFF.txt"
};

const uint8_t EN_BT_WEAR_DETECT[] = {
// #include "res/en/voice_twspro/SOUND_WEAR_DETECT.txt"
};

const uint8_t EN_BT_SIGNATURE[] = {
#if defined(DOLBY_AUDIO_ENABLE)
// #include "res/en/voice_twspro/SOUND_DOLBY_OFF.txt"
#endif
};

// ##################################### sdk prompt ##################################
const uint8_t EN_SOUND_ZERO[] = {
// #include "res/en/SOUND_ZERO.txt"
};
const uint8_t EN_SOUND_ONE[] = {
// #include "res/en/SOUND_ONE.txt"
};
const uint8_t EN_SOUND_TWO[] = {
//#include "res/en/SOUND_THREE.txt"
};

const uint8_t EN_SOUND_THREE[] = {
//#include "res/en/SOUND_THREE.txt"
};
const uint8_t EN_SOUND_FOUR[] = {
//#include "res/en/SOUND_FOUR.txt"
};
const uint8_t EN_BT_OVER[] = {
//#include "res/en/SOUND_OVER.txt"
};
const uint8_t EN_SOUND_FIVE[] = {
//#include "res/en/SOUND_FIVE.txt"
};
const uint8_t EN_SOUND_SIX[] = {
//#include "res/en/SOUND_SIX.txt"
};
const uint8_t EN_SOUND_SEVEN [] = {
//#include "res/en/SOUND_SEVEN.txt"
};
const uint8_t EN_SOUND_EIGHT [] = {
//#include "res/en/SOUND_EIGHT.txt"
};
const uint8_t EN_SOUND_NINE [] = {
//#include "res/en/SOUND_NINE.txt"
};

const uint8_t EN_SOUND_PROMPT_ADAPTIVE_ANC [] = {
//#include "res/adapt_anc/SOUND_PROMPT_ADAPTIVE_ANC.txt"
};
const uint8_t EN_SOUND_CUSTOM_LEAK_DETECT [] = {
//#include "res/ld/SOUND_PROMPT_CUSTOM_LEAK.txt"
};

const uint8_t EN_BT_PAIRING[] = {
//#include "res/en/SOUND_PAIRING.txt"
};

const uint8_t EN_BT_PAIRING_FAIL[] = {
//#include "res/en/SOUND_PAIRING_FAIL.txt"
};

const uint8_t EN_BT_PAIRING_SUCCESS[] = {
//#include "res/en/SOUND_PAIRING_SUCCESS.txt"
};


const uint8_t EN_BT_WARNING[] = {
#include "res/en/SOUND_WARNING.txt"
};

const uint8_t EN_CHARGE_FINISH[] = {
//#include "res/en/SOUND_CHARGE_FINISH.txt"
};

const uint8_t EN_LANGUAGE_SWITCH[] = {
//#include "res/en/SOUND_LANGUAGE_SWITCH.txt"
};

const uint8_t EN_BT_ALEXA_START[] = {
//#include "res/en/SOUND_ALEXA_START.txt"
};

const uint8_t EN_BT_GSOUND_MIC_OPEN[] = {
//#include "res/en/SOUND_GSOUND_MIC_OPEN.txt"
};

const uint8_t EN_BT_GSOUND_MIC_CLOSE[] = {
//#include "res/en/SOUND_GSOUND_MIC_CLOSE.txt"
};

const uint8_t EN_BT_GSOUND_NC[] = {
//#include "res/en/SOUND_GSOUND_NC.txt"
};

#ifdef __INTERACTION__
const uint8_t EN_BT_FINDME[] = {
//#include "res/en/SOUND_FINDME.txt"
};
#endif

const uint8_t EN_BT_MUTE[] = {
//#include "res/SOUND_MUTE.txt"
};

#endif
#endif /* __RES_AUDIO_DATA_H__ */

