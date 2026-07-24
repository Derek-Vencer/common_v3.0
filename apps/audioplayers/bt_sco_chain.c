/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#include "plat_types.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_codec.h"
#include "hal_aud.h"
#include "audio_dump.h"
#include "speech_cfg.h"
#include "bt_sco_chain.h"
#include "bt_sco_chain_cfg.h"
#include "bt_sco_chain_tuning.h"
#include "hal_overlay.h"
#include "mpu.h"
#if !defined(CHIP_BEST1307P) && !defined(CHIP_BEST1307)
#include "adaptive_volume.h"
#endif
#if defined(SPEECH_TX_2MIC_NS4)||defined(SPEECH_TX_3MIC_NS)
#include "app_anc.h"
#endif
#include "app_utils.h"
#include "arm_math_ex.h"

#if defined(SPEECH_ALGO_DSP)
#include "app_mcpp.h"
#endif

#ifdef BESUI_TWS_EN
#include "twsui_comm.h"
#endif
#if defined(BESUI_COMM_EN)
#include "besui_common.h"
#endif
#ifdef BESUI_STEREO_EN
#include "stereo_prompt.h"
#include "app_thread.h"
#include "stereoui.h"
#ifdef USER_FACTORY_TRACE_RX_EN
#include "uart_factory.h"
#endif
#endif

#if defined(SPEECH_TX_24BIT)
typedef int     SPEECH_PCM_T;
#else
typedef short   SPEECH_PCM_T;
#endif

#if !defined(SPEECH_TX_1MIC_NS) && !defined(SPEECH_TX_2MIC_NS8)
#if defined(SPEECH_ROM_PATCH_9_P_1) || defined(SPEECH_ROM_PATCH_9_P_5)
#include "patch.h"
#include "hal_location.h"
extern const unsigned char besnet_weights[];
#endif
#endif

#if defined(SPEECH_TX_2MIC_NS4) || defined(SPEECH_TX_3MIC_PREAF) || defined(SPEECH_TX_3MIC_NS)
#include "app_anc_table.h"
static uint32_t curr_mode = 0;
extern const float normal_calib_filter[128];
extern const float tt_calib_filter[128];
#endif

#if defined(SPEECH_TX_3MIC_NS)
static uint32_t NO_AI = 0;
#endif

#if defined(SCO_CP_ACCEL)
#include "hal_location.h"
#include "bt_sco_chain_cp.h"

#define SCO_CP_ACCEL_ALGO_START()                                              \
    *_pcm_len = pcm_len;                                                       \
    }                                                                          \
    CP_TEXT_SRAM_LOC                                                           \
    void _speech_tx_process_mid(short *pcm_buf, short *ref_buf, short *fb_buf, int *_pcm_len, sco_status_t *status) \
    {                                                                          \
        int pcm_len = *_pcm_len;

#define SCO_CP_ACCEL_ALGO_END()                                                 \
    *_pcm_len = pcm_len;                                                        \
    }                                                                           \
    void _speech_tx_process_post(short *pcm_buf, short *ref_buf, int *_pcm_len, sco_status_t *status) \
    {                                                                           \
        int pcm_len = *_pcm_len;

#define SCO_CP_ACCEL_ALGO_INIT_START()                                          \
        return 0;                                                               \
    }                                                                           \
    CP_TEXT_SRAM_LOC                                                            \
    int _speech_tx_init_mid(int sample_rate, int frame_len)                     \
    {                                                                           \

#define SCO_CP_ACCEL_ALGO_INIT_END()                                            \
        return 0;                                                               \
    }                                                                           \
    int _speech_tx_init_post(int sample_rate, int frame_len)                    \
    {

#define SPEECH_TX_AEC2FLOAT_CORE 1
#define SPEECH_TX_NS2FLOAT_CORE 0
#define SPEECH_RX_NS2FLOAT_CORE 0

#else
#define SCO_CP_ACCEL_ALGO_START()
#define SCO_CP_ACCEL_ALGO_END()

#define SCO_CP_ACCEL_ALGO_INIT_START()
#define SCO_CP_ACCEL_ALGO_INIT_END()

#define SPEECH_TX_AEC2FLOAT_CORE 0
#define SPEECH_TX_NS2FLOAT_CORE 0
#define SPEECH_RX_NS2FLOAT_CORE 0

#endif

//#define BT_SCO_CHAIN_PROFILE
#ifdef USER_SPEECH_DUMP_EN
#define BT_SCO_CHAIN_AUDIO_DUMP
#endif
// #define TWS_SWITCH_ACCORDING_NOISE
#define BT_SCO_LOW_RAM
//#define SPEECH_TX_3MIC_SWAP_CHANNELS

/* #define SPEECH_DC_FILTER_RPC */
#if defined(SPEECH_DC_FILTER_RPC)
#include "speech_dc_filter_frontend.h"
#endif

#if defined(BT_SCO_CHAIN_PROFILE)
static uint32_t tx_start_ticks = 0;
static uint32_t tx_end_ticks = 0;
#endif

#ifdef SPEECH_ALGO_DSP
static bool g_mcpp_enable = false;
POSSIBLY_UNUSED int speech_enable_mcpp(bool en)
{
    AUDIOPLAYERS_TRACE(0, "%s, old:%d, new:%d", __func__, g_mcpp_enable, en);
    g_mcpp_enable = en;
    return 0;
}
#endif

extern const SpeechConfig speech_cfg_default;
static SpeechConfig *speech_cfg = NULL;

FrameResizeState *speech_frame_resize_st = NULL;

#if defined(SPEECH_TX_24BIT)
int32_t *aec_echo_buf = NULL;
#else
short *aec_echo_buf = NULL;
#endif
short *af_out_buf = NULL;

#if defined(SPEECH_BONE_SENSOR)
#include "speech_bone_sensor.h"
static SPEECH_PCM_T *vpu_buf = NULL;
#endif

#if defined(SPEECH_TX_REF)
// Use to free buffer
#if defined(SPEECH_TX_24BIT)
static int32_t *aec_echo_buf_ptr;
#else
static short *aec_echo_buf_ptr;
#endif
#endif

/*--------------------TX state--------------------*/
#if defined(SPEECH_TX_DC_FILTER)
SpeechDcFilterState *speech_tx_dc_filter_st = NULL;
#endif

// AEC
#if defined(SPEECH_TX_AEC2FLOAT)
static Ec2FloatState *speech_tx_aec2float_st = NULL;
#endif

//1mic_ns
#if defined(SPEECH_TX_1MIC_PREAF)
Speech1MicPreafState *speech_tx_1mic_preaf_st = NULL;
#endif

#if defined(SPEECH_TX_1MIC_NS)
Speech1MicNSState *speech_tx_1mic_ns_st = NULL;
void *model_data = NULL;
#endif

#if defined(CHIP_BEST1307) && defined(SPEECH_TX_NS9)
SpeechNs9State *speech_tx_ns9_st = NULL;
#endif

// 2MIC NS
#if defined(SPEECH_TX_2MIC_PREAF)
Speech2MicPreafState *speech_tx_2mic_preaf_st = NULL;
#endif

#if defined(SPEECH_TX_2MIC_NS4_PREAF)
Speech2MicNs4PreafState *speech_tx_2mic_ns4_preaf_st = NULL;
#endif

#if defined(SPEECH_TX_2MIC_NS4)
SensorMicDenoiseState *speech_tx_2mic_ns4_st = NULL;
void *inner_model_data = NULL;
void *outer_model_data = NULL;
#endif

#if defined(SPEECH_TX_2MIC_NS7)
Speech2MicNs7State *speech_tx_2mic_ns7_st = NULL;
#endif

#if defined(SPEECH_TX_2MIC_NS8)
Speech2MicNs8State *speech_tx_2mic_ns8_st = NULL;
void *model_data_s = NULL;
#endif

#if defined(SPEECH_TX_2MIC_NS10)
Speech2MicNs10State *speech_tx_2mic_ns10_st = NULL;
#endif

#if defined(SPEECH_TX_3MIC_PREAF)
Speech3MicPreafState *speech_tx_3mic_preaf_st = NULL;
#endif

#if defined(SPEECH_TX_3MIC_NS)
Speech3MicNsState *speech_tx_3mic_ns_st = NULL;
void *model_data_s = NULL;
#endif

#if defined(SPEECH_TX_3MIC_NS2)
Speech3MicNs2State *speech_tx_3mic_ns2_st = NULL;
void *model_data_l = NULL;
#endif

// NS
#if defined(SPEECH_TX_NS2FLOAT)
SpeechNs2FloatState *speech_tx_ns2float_st = NULL;
#endif

#if defined(SPEECH_TX_NS3)
static Ns3State *speech_tx_ns3_st = NULL;
#endif

#if defined(SPEECH_TX_NS4)
SpeechNs4State *speech_tx_ns4_st = NULL;
#endif

// Gain
#if defined(SPEECH_TX_COMPEXP)
static CompexpState *speech_tx_compexp_st = NULL;
#endif

#if defined(SPEECH_TX_AGC)
static AgcState *speech_tx_agc_st = NULL;
#endif

// EQ
#if defined(SPEECH_TX_EQ)
EqState *speech_tx_eq_st = NULL;
#endif

/*--------------------RX state--------------------*/
// NS
#if defined(SPEECH_RX_NS2FLOAT)
SpeechNs2FloatState *speech_rx_ns2float_st = NULL;
#endif

#if defined(SPEECH_RX_NS3)
static Ns3State *speech_rx_ns3_st = NULL;
#endif

// GAIN
#if defined(SPEECH_RX_AGC)
static AgcState *speech_rx_agc_st = NULL;
#endif

#if defined(SPEECH_RX_COMPEXP)
static MultiCompexpState *speech_rx_compexp_st = NULL;
#endif

// EQ
#if defined(SPEECH_RX_EQ)
EqState *speech_rx_eq_st = NULL;
#endif

#if defined(SPEECH_RX_HW_EQ)
SpeechHwEqState *speech_rx_hw_eq_st = NULL;
#endif

#if defined(SPEECH_RX_DAC_EQ)
SpeechDacEqState *speech_rx_dac_eq_st = NULL;
#endif

static bool dualmic_enable = true;
POSSIBLY_UNUSED  bool wind_enable = true;

void switch_dualmic_status(void)
{
    AUDIOPLAYERS_TRACE(3,"[%s] dualmic status: %d -> %d", __FUNCTION__, dualmic_enable, !dualmic_enable);
    dualmic_enable ^= true;
}

static int speech_tx_sample_rate = 16000;
static int speech_rx_sample_rate = 16000;
static int speech_tx_frame_len = 256;
static int speech_rx_frame_len = 256;
static int speech_sco_frame_len = 240;

#ifndef SCO_CP_ACCEL_FIFO
static bool speech_tx_frame_resizer_enable = false;
static bool speech_rx_frame_resizer_enable = false;
#endif

extern bool bt_sco_codec_is_cvsd(void);

static int32_t _speech_tx_process_(void *pcm_buf, void *ref_buf, int32_t *pcm_len);
static int32_t _speech_rx_process_(void *pcm_buf, int32_t *pcm_len);
enum APP_SYSFREQ_FREQ_T speech_get_proper_sysfreq(int *needed_mips);

void *speech_get_ext_buff(int size)
{
    void *pBuff = NULL;
    if (size % 4)
    {
        size = size + (4 - size % 4);
    }

    pBuff = speech_calloc(size, sizeof(uint8_t));
    AUDIOPLAYERS_TRACE(2,"[%s] len:%d", __func__, size);

    return pBuff;
}

int speech_get_lea_algo_ram_usage(void)
{
    AUDIOPLAYERS_TRACE(1, "[%s] FIXME: Check Call tx algo RAM usage", __func__);
    int size = 0;
#if defined(SPEECH_TX_2MIC_NS8)
    size = 1024 * 160;
#else
    size = 1024 * 60;
#endif
    return size;
}

int speech_store_config(const SpeechConfig *cfg)
{
    if (speech_cfg)
    {
        memcpy(speech_cfg, cfg, sizeof(SpeechConfig));
#ifdef SPEECH_TX_3MIC_NS
        speech_cfg->tx_3mic_preaf[0].fb_filter = normal_calib_filter;
        speech_cfg->tx_3mic_preaf[2].fb_filter = tt_calib_filter;
#endif
#ifdef SPEECH_TX_2MIC_NS4
        speech_cfg->tx_2mic_ns4[0].fb_filter = normal_calib_filter;
        speech_cfg->tx_2mic_ns4[2].fb_filter = tt_calib_filter;
#endif
#ifdef SPEECH_TX_3MIC_PREAF
        speech_cfg->tx_3mic_preaf[0].fb_filter = normal_calib_filter;
        speech_cfg->tx_3mic_preaf[2].fb_filter = tt_calib_filter;
#endif
    }
    else
    {
        AUDIOPLAYERS_TRACE(1,"[%s] WARNING: Please phone call...", __func__);
    }

    return 0;
}

#if defined(SPEECH_TX_2MIC_NS4)||defined(SPEECH_TX_3MIC_NS)
POSSIBLY_UNUSED static uint8_t last_anc_mode = 0;
POSSIBLY_UNUSED static uint8_t get_config_index_for_anc_mode(anc_custom_mode_t anc_mode)
{
    if (anc_mode == ANC_CUSTOM_MODE_OFF) {
        return 0;
    } else if (anc_mode < ANC_CUSTOM_MODE_TT || anc_mode == ANC_CUSTOM_MODE_ADAPTIVE) {
        return 1;
    } else {
        return 2;
    }
}
#endif

#define HARDWARE_ECHO_DELAY (70)
static int echo_delay = HARDWARE_ECHO_DELAY;

int _speech_tx_init_pre(int sample_rate, int frame_len)
{
    AUDIOPLAYERS_TRACE(3,"[%s] Start, sample_rate: %d, frame_len: %d", __func__, sample_rate, frame_len);

#if defined(SPEECH_TX_DC_FILTER)
    int channel_num = SPEECH_CODEC_CAPTURE_CHANNEL_NUM;
    int data_separation = 0;

    speech_tx_dc_filter_st = speech_dc_filter_create_with_custom_allocator(sample_rate, frame_len, &speech_cfg->tx_dc_filter, default_allocator());
    speech_dc_filter_ctl(speech_tx_dc_filter_st, SPEECH_DC_FILTER_SET_CHANNEL_NUM, &channel_num);
    speech_dc_filter_ctl(speech_tx_dc_filter_st, SPEECH_DC_FILTER_SET_DATA_SEPARATION, &data_separation);
#endif

    echo_delay = HARDWARE_ECHO_DELAY;

#if !defined(SPEECH_TX_1MIC_NS) && !defined(SPEECH_TX_2MIC_NS8)
#if defined(SPEECH_ROM_PATCH_9_P_1) || defined(SPEECH_ROM_PATCH_9_P_5)
    patch_open(PATCH_CTRL_ID_0, 0);
    //model_init
    patch_enable(PATCH_CTRL_ID_0, PATCH_TYPE_DATA, 0x22024580, (uint32_t)&besnet_weights);
    patch_enable(PATCH_CTRL_ID_0, PATCH_TYPE_DATA, 0x22024908, (uint32_t)&besnet_weights);
#endif
#endif

#if defined(CHIP_BEST1307) && defined(SPEECH_TX_NS9)
    speech_tx_ns9_st = speech_ns9_create(sample_rate, frame_len, &speech_cfg->tx_ns9, default_allocator());
#endif

#if defined(SPEECH_TX_1MIC_PREAF)
    speech_tx_1mic_preaf_st = speech_1mic_preaf_create(sample_rate, frame_len, &speech_cfg->tx_1mic_preaf, default_allocator());
#endif

#if defined(SPEECH_TX_2MIC_PREAF)
    speech_tx_2mic_preaf_st = speech_2mic_preaf_create(sample_rate, frame_len, &speech_cfg->tx_2mic_preaf, default_allocator());
#endif

#if defined(SPEECH_TX_2MIC_NS4_PREAF)
    curr_mode = get_config_index_for_anc_mode(app_anc_table_get_custom_mode(app_anc_get_curr_mode()));
    speech_tx_2mic_ns4_preaf_st = speech_2mic_ns4_preaf_create(sample_rate, frame_len, &speech_cfg->tx_2mic_ns4_preaf[curr_mode], default_allocator());
#endif

#if defined(SPEECH_TX_3MIC_PREAF) && defined(SPEECH_TX_3MIC_NS)
#if defined(ANC_APP)
    curr_mode = get_config_index_for_anc_mode(app_anc_table_get_custom_mode(app_anc_get_curr_mode()));
#else
    curr_mode = 0;
#endif    
    speech_tx_3mic_preaf_st = speech_3mic_preaf_create(sample_rate, frame_len, &speech_cfg->tx_3mic_preaf[curr_mode], default_allocator());
#endif

#if defined(SPEECH_TX_3MIC_PREAF) && defined(SPEECH_TX_3MIC_NS2)
    speech_tx_3mic_preaf_st = speech_3mic_preaf_create(sample_rate, frame_len, &speech_cfg->tx_3mic_preaf[3], default_allocator());
#endif

    SCO_CP_ACCEL_ALGO_INIT_START();

#if defined(SPEECH_TX_1MIC_NS)
#if defined(CHIP_BEST1307S) && !defined(SPEECH_ROM_PATCH_1307S)
    model_data = speech_1mic_ns_get_model_data_94l();
#elif defined(CHIP_BEST1307P) || defined(SPEECH_ROM_PATCH_1307S)
    model_data = speech_1mic_ns_get_model_data_96l();
#elif defined(CHIP_BEST1307PH)
    model_data = speech_nn_ns_get_model_data_95l_rom_v2();
#else
    model_data = speech_nn_ns_get_model_data_96l();
#endif
    speech_tx_1mic_ns_st = speech_1mic_ns_create(model_data, &speech_cfg->tx_1mic_ns, default_allocator());

#if defined(SPEECH_TX_1MIC_PREAF)
    speech_1mic_ns_set_af_state(speech_tx_1mic_ns_st, speech_tx_1mic_preaf_st, speech_1mic_preaf_reset_coef);
#endif
#endif

#if defined(SPEECH_TX_2MIC_NS7)
    speech_heap_set_cp(1);
    speech_tx_2mic_ns7_st = speech_2mic_ns7_create(sample_rate, frame_len, &speech_cfg->tx_2mic_ns7, default_allocator());
    speech_heap_set_cp(0);
#endif

#if defined(SPEECH_TX_2MIC_NS8)
#if defined(CHIP_BEST1307P) || defined(CHIP_BEST1307)
    model_data_s = speech_2mic_ns8_get_model_data_95s();
#else
    model_data_s = speech_nn_ns_get_model_data_95s();
#endif
    speech_tx_2mic_ns8_st = speech_2mic_ns8_create(model_data_s, sample_rate, frame_len, &speech_cfg->tx_2mic_ns8, default_allocator());
    echo_delay += speech_2mic_ns8_get_delay(speech_tx_2mic_ns8_st);
#if defined(SPEECH_TX_2MIC_PREAF)
    speech_2mic_ns8_set_af_state(speech_tx_2mic_ns8_st, speech_tx_2mic_preaf_st, speech_2mic_preaf_reset_coef);
#endif
#endif

#if defined(SPEECH_TX_2MIC_NS10)
    speech_tx_2mic_ns10_st = speech_2mic_ns10_create(sample_rate, frame_len, &speech_cfg->tx_2mic_ns10, default_allocator());
    echo_delay += speech_2mic_ns10_get_delay(speech_tx_2mic_ns10_st);
#endif

#if defined(SPEECH_TX_2MIC_NS4)
    curr_mode = get_config_index_for_anc_mode(app_anc_table_get_custom_mode(app_anc_get_curr_mode()));
#if defined(CHIP_BEST1307P) || defined(CHIP_BEST1307)
    inner_model_data = speech_2mic_ns8_get_model_data_112l();
    outer_model_data = speech_2mic_ns8_get_model_data_91s();
#else
    inner_model_data = speech_nn_ns_get_model_data_112l();
    outer_model_data = speech_nn_ns_get_model_data_91s();
#endif
    speech_tx_2mic_ns4_st = sensormic_denoise_create(inner_model_data, outer_model_data, sample_rate, frame_len, &speech_cfg->tx_2mic_ns4[curr_mode], default_allocator());
#if defined(SPEECH_TX_2MIC_NS4_PREAF)
    sensormic_denoise_set_af_state(speech_tx_2mic_ns4_st, speech_tx_2mic_ns4_preaf_st, speech_2mic_ns4_preaf_reset_coef);
#endif    
    last_anc_mode = curr_mode;
#endif

#if defined(SPEECH_TX_3MIC_NS)
#if defined(ANC_APP)
    curr_mode = get_config_index_for_anc_mode(app_anc_table_get_custom_mode(app_anc_get_curr_mode()));
#else
    curr_mode = 0;
#endif
#if defined(CHIP_BEST1307P) || defined(CHIP_BEST1307)
    model_data_s = speech_3mic_ns_get_model_data_95s();
#else
    model_data_s = speech_nn_ns_get_model_data_95s();
#endif
    speech_tx_3mic_ns_st = speech_3mic_ns_create(model_data_s, sample_rate, frame_len, &speech_cfg->tx_3mic_ns[curr_mode], default_allocator());
    last_anc_mode = curr_mode;
#if defined(SPEECH_TX_3MIC_PREAF)
    speech_3mic_ns_set_af_state(speech_tx_3mic_ns_st, speech_tx_3mic_preaf_st, speech_3mic_preaf_reset_coef);
#endif
#endif

#if defined(SPEECH_TX_3MIC_NS2)
#if defined(CHIP_BEST1307P) || defined(CHIP_BEST1307)
    model_data_l = speech_1mic_ns_get_model_data_96l();
#else
    model_data_l = speech_nn_ns_get_model_data_96l();
#endif
    speech_tx_3mic_ns2_st = speech_3mic_ns2_create(model_data_l, sample_rate, frame_len, &speech_cfg->tx_3mic_ns2, default_allocator());
#if defined(SPEECH_TX_3MIC_PREAF)
    speech_3mic_ns2_set_af_state(speech_tx_3mic_ns2_st, speech_tx_3mic_preaf_st, speech_3mic_preaf_reset_coef);
#endif
#endif

    SCO_CP_ACCEL_ALGO_INIT_END();

#if defined(SPEECH_TX_REF)
#if defined(SPEECH_TX_24BIT)
    aec_echo_buf = (int32_t *)speech_calloc(frame_len, sizeof(int32_t));
#else
    aec_echo_buf = (short *)speech_calloc(frame_len, sizeof(short));
#endif
    aec_echo_buf_ptr = aec_echo_buf;
// #endif
#endif

#if defined(SPEECH_TX_AEC2FLOAT)
    af_out_buf = (short *)speech_calloc(frame_len, sizeof(short));
#elif defined(SPEECH_TX_REF) || defined(CHIP_BEST1307P) || defined(CHIP_BEST1307PH)
    af_out_buf = (short *)speech_calloc(frame_len * AEC_OUT_BUF_CHAN_NUM, sizeof(short));
#endif

    AUDIOPLAYERS_TRACE(2, "[%s] echo delay is %d", __FUNCTION__, echo_delay);

#if defined(SPEECH_TX_AEC2FLOAT)
    speech_tx_aec2float_st = ec2float_create_with_custom_allocator(sample_rate, frame_len, SPEECH_TX_AEC2FLOAT_CORE, &speech_cfg->tx_aec2float, default_allocator());
#endif

#if defined(SPEECH_TX_NS2FLOAT)
    speech_tx_ns2float_st = speech_ns2float_create(sample_rate, frame_len, SPEECH_TX_NS2FLOAT_CORE, &speech_cfg->tx_ns2float);
#endif

#if defined(SPEECH_TX_NS3)
    speech_tx_ns3_st = ns3_create(sample_rate, frame_len, &speech_cfg->tx_ns3);
#if defined(SPEECH_TX_3MIC_NS)
    speech_3mic_ns_set_ns_state(speech_tx_3mic_ns_st, speech_tx_ns3_st);
#endif
#endif

#if defined(SPEECH_TX_NS4)
#ifdef BT_SCO_LOW_RAM
    if (16000 == sample_rate)
        speech_tx_ns4_st = speech_ns4_create(sample_rate, frame_len/2, &speech_cfg->tx_ns4);
    else
        speech_tx_ns4_st = speech_ns4_create(sample_rate, frame_len, &speech_cfg->tx_ns4);
#else
    speech_tx_ns4_st = speech_ns4_create(sample_rate, frame_len, &speech_cfg->tx_ns4);
#endif
#endif

#if defined(SPEECH_TX_COMPEXP)
    speech_tx_compexp_st = compexp_create_with_custom_allocator(sample_rate, frame_len, &speech_cfg->tx_compexp, default_allocator());
#endif

#if defined(SPEECH_TX_AGC)
    speech_tx_agc_st = agc_state_create(sample_rate, frame_len, &speech_cfg->tx_agc);
#endif

#if defined(SPEECH_TX_EQ)
    speech_tx_eq_st = eq_init_with_custom_allocator(sample_rate, frame_len, &speech_cfg->tx_eq, default_allocator());
#endif

    AUDIOPLAYERS_TRACE(1,"[%s] End", __func__);

    return 0;
}

sco_status_t status = {
    .vad = true,
    .wind = false,
};

void sco_status_reset(sco_status_t *status)
{
    status->vad = true;
    status->wind = false;
}

int speech_tx_init(int sample_rate, int ap_frame_len, int cp_frame_len)
{
    _speech_tx_init_pre(sample_rate, ap_frame_len);
#if defined(SCO_CP_ACCEL)
    _speech_tx_init_mid(sample_rate, cp_frame_len);
    _speech_tx_init_post(sample_rate, ap_frame_len);
#endif
    sco_status_reset(&status);
    return 0;
}

int speech_rx_init(int sample_rate, int frame_len)
{
    AUDIOPLAYERS_TRACE(3,"[%s] Start, sample_rate: %d, frame_len: %d", __func__, sample_rate, frame_len);

#if defined(SPEECH_RX_NS2FLOAT)
#ifdef BT_SCO_LOW_RAM
    if (16000 == sample_rate)
    {
        speech_rx_ns2float_st = speech_ns2float_create_with_custom_allocator(sample_rate, frame_len/2, SPEECH_RX_NS2FLOAT_CORE, &speech_cfg->rx_ns2float, default_allocator());
    }
    else
    {
        speech_rx_ns2float_st = speech_ns2float_create_with_custom_allocator(sample_rate, frame_len, SPEECH_RX_NS2FLOAT_CORE, &speech_cfg->rx_ns2float, default_allocator());
    }
#else
    speech_rx_ns2float_st = speech_ns2float_create_with_custom_allocator(sample_rate, frame_len, SPEECH_RX_NS2FLOAT_CORE, &speech_cfg->rx_ns2float, default_allocator());
#endif
#endif

#if defined(SPEECH_RX_NS3)
    speech_rx_ns3_st = ns3_create(sample_rate, frame_len, &speech_cfg->rx_ns3);
#endif

#if defined(SPEECH_RX_AGC)
    speech_rx_agc_st = agc_state_create(sample_rate, frame_len, &speech_cfg->rx_agc);
#endif

#if defined(SPEECH_RX_COMPEXP)
    speech_rx_compexp_st = multi_compexp_create_with_custom_allocator(sample_rate, frame_len, &speech_cfg->rx_compexp, default_allocator());
#endif

#if defined(SPEECH_RX_EQ)
    speech_rx_eq_st = eq_init_with_custom_allocator(sample_rate, frame_len, &speech_cfg->rx_eq, default_allocator());
#endif

#if defined(SPEECH_RX_HW_EQ)
    int32_t sample_bits =
#if defined(SPEECH_RX_24BIT)
        24;
#else
        16;
#endif
    speech_rx_hw_eq_st = speech_hw_eq_init(sample_rate, sample_bits, frame_len, &speech_cfg->rx_hw_eq);
#endif

#if defined(SPEECH_RX_DAC_EQ)
#if defined(SPEECH_CODEC_FIXED_SAMPLE_RATE)
    int32_t dac_sample_rate = SPEECH_CODEC_FIXED_SAMPLE_RATE;
#else
    int32_t dac_sample_rate = sample_rate;
#endif
#if defined(__AUDIO_RESAMPLE__)
    dac_sample_rate = hal_codec_get_real_sample_rate(sample_rate, 1);
#endif
    speech_rx_dac_eq_st = speech_dac_eq_init(dac_sample_rate, frame_len, &speech_cfg->rx_dac_eq);
#endif

    AUDIOPLAYERS_TRACE(1,"[%s] End", __func__);

    return 0;
}

float speech_sysfreq_to_mips(enum HAL_CMU_FREQ_T sysfreq);

int speech_init2(int tx_sample_rate, int rx_sample_rate,
                     int tx_frame_len, int rx_frame_len,
                     int sco_frame_len,
                     uint8_t *buf, int len)
{
    AUDIOPLAYERS_TRACE(1,"[%s] Start...", __func__);

    //ASSERT(frame_ms == SPEECH_PROCESS_FRAME_MS, "[%s] frame_ms(%d) != SPEECH_PROCESS_FRAME_MS(%d)", __func__,
    //                                                                                                frame_ms,
    //                                                                                                SPEECH_PROCESS_FRAME_MS);

    speech_tx_sample_rate = tx_sample_rate;
    speech_rx_sample_rate = rx_sample_rate;

    speech_tx_frame_len = tx_frame_len;
    speech_rx_frame_len = rx_frame_len;
    speech_sco_frame_len = sco_frame_len;

    speech_heap_init(buf, len);
#if 1//defined(SCO_OPTIMIZE_FOR_RAM)
    enum HAL_OVERLAY_ID_T id = hal_overlay_get_curr_id();
    if(id != HAL_OVERLAY_ID_QTY){
        speech_heap_add_block((uint8_t *)hal_overlay_get_text_free_addr(id), hal_overlay_get_text_free_size(id));
    }
#endif
    // When phone call again, speech cfg will be default.
    // If do not want to reset speech cfg, need define bt_sco_chain_init()
    // and call in apps.cpp: app_init()
    speech_cfg = (SpeechConfig *)speech_calloc(1, sizeof(SpeechConfig));
    speech_store_config(&speech_cfg_default);

#ifdef AUDIO_DEBUG
    speech_tuning_open();
#endif

#ifndef SCO_CP_ACCEL_FIFO
    int aec_enable = 0;
#if defined(SPEECH_TX_REF)
    aec_enable = 1;
#endif

    int capture_sample_size = sizeof(int16_t), playback_sample_size = sizeof(int16_t);
#if defined(SPEECH_TX_24BIT)
    capture_sample_size = sizeof(int32_t);
#endif
#if defined(SPEECH_RX_24BIT)
    playback_sample_size = sizeof(int32_t);
#endif

    CAPTURE_HANDLER_T tx_handler = (tx_frame_len == sco_frame_len) ? NULL : _speech_tx_process_;
    PLAYBACK_HANDLER_T rx_handler = ((rx_sample_rate/rx_frame_len) == (tx_sample_rate/sco_frame_len)) ? NULL : _speech_rx_process_;

    speech_tx_frame_resizer_enable = (tx_handler != NULL);
    speech_rx_frame_resizer_enable = (rx_handler != NULL);

    if (speech_tx_frame_resizer_enable || speech_rx_frame_resizer_enable) {
        speech_frame_resize_st = frame_resize_create(speech_sco_frame_len,
                                                     SPEECH_CODEC_CAPTURE_CHANNEL_NUM,
                                                     speech_tx_frame_len,
                                                     capture_sample_size,
                                                     playback_sample_size,
                                                     aec_enable,
                                                     tx_handler,
                                                     rx_handler
                                                    );
    }
#endif

#if defined(SPEECH_BONE_SENSOR)
    vpu_buf = (SPEECH_PCM_T *)speech_calloc(speech_tx_frame_len, sizeof(SPEECH_PCM_T));
#endif

#if defined(SCO_CP_ACCEL)
    // NOTE: change channel number for different case.
    sco_cp_init(speech_tx_frame_len, SPEECH_CODEC_CAPTURE_CHANNEL_NUM);
#endif

#if defined(SPEECH_ALGO_DSP)
    if (g_mcpp_enable) {
#ifdef SPEECH_BONE_SENSOR
        bool vpu_flag = true;
#else
        bool vpu_flag = false;
#endif
        APP_MCPP_CFG_T dsp_cfg;
        memset(&dsp_cfg, 0, sizeof(APP_MCPP_CFG_T));
        dsp_cfg.capture.stream_enable = true;
        dsp_cfg.capture.sample_rate = speech_tx_sample_rate;
        dsp_cfg.capture.sample_bytes = capture_sample_size;
        dsp_cfg.capture.algo_frame_len = speech_tx_frame_len;
        dsp_cfg.capture.params[0] = (bt_sco_codec_is_cvsd() == false);
        dsp_cfg.capture.params[1] = app_mcpp_get_cap_channel_map(true, false, vpu_flag);
        dsp_cfg.capture.channel_num = SPEECH_CODEC_CAPTURE_CHANNEL_NUM;
#if defined(SPEECH_ALGO_DSP_M55)
        dsp_cfg.capture.core_server = APP_MCPP_CORE_M55;
#elif defined(SPEECH_ALGO_DSP_HIFI)
        dsp_cfg.capture.core_server = APP_MCPP_CORE_HIFI;
#elif defined(SPEECH_ALGO_DSP_SENS)
        dsp_cfg.capture.core_server = APP_MCPP_CORE_SENS;
#endif

        dsp_cfg.playback.stream_enable = true;
        dsp_cfg.playback.sample_rate = speech_rx_sample_rate;
        dsp_cfg.playback.sample_bytes = playback_sample_size;
        dsp_cfg.playback.algo_frame_len = speech_rx_frame_len;
        dsp_cfg.playback.channel_num = 1;
#if defined(SPEECH_ALGO_DSP_M55)
        dsp_cfg.playback.core_server = APP_MCPP_CORE_M55;
#elif defined(SPEECH_ALGO_DSP_HIFI)
        dsp_cfg.playback.core_server = APP_MCPP_CORE_HIFI;
#elif defined(SPEECH_ALGO_DSP_SENS)
        dsp_cfg.playback.core_server = APP_MCPP_CORE_SENS;
#endif

        app_mcpp_open(APP_MCPP_USER_CALL, &dsp_cfg);
    }
#endif

    speech_rx_init(speech_rx_sample_rate, speech_rx_frame_len);

#if defined(SCO_CP_ACCEL_FIFO)
    speech_tx_init(speech_tx_sample_rate, speech_sco_frame_len, speech_tx_frame_len);
#else
    speech_tx_init(speech_tx_sample_rate, speech_tx_frame_len, speech_tx_frame_len);
#endif

    //speech_rx_init(speech_rx_sample_rate, speech_rx_frame_len);

#if !defined(SCO_CP_ACCEL) && !defined(SPEECH_DC_FILTER_RPC)
    int needed_freq = 0;
    enum APP_SYSFREQ_FREQ_T min_system_freq = speech_get_proper_sysfreq(&needed_freq);

    enum APP_SYSFREQ_FREQ_T freq = hal_sysfreq_get();

    if (freq < min_system_freq) {
        freq = min_system_freq;

        if (freq > (int)HAL_CMU_FREQ_104M) {
            AUDIOPLAYERS_TRACE(2, "[%s] required mips %d(needed freq %d) is too large, keep 104M", __FUNCTION__, freq, needed_freq);
            freq = HAL_CMU_FREQ_104M;
        }

        app_sysfreq_req(APP_SYSFREQ_USER_BT_SCO, freq);
        AUDIOPLAYERS_TRACE(2,"[%s]: app_sysfreq_req %d", __FUNCTION__, freq);
    }
#endif
    //app_sysfreq_req(APP_SYSFREQ_USER_BT_SCO, APP_SYSFREQ_104M);

// DUMP: Multi MICs + Ref + VPU + OUT
#if defined(BT_SCO_CHAIN_AUDIO_DUMP)
    uint32_t dump_ch = SPEECH_CODEC_CAPTURE_CHANNEL_NUM + 1;
    if (aec_enable) {
        dump_ch += 1;
    }
#if defined(SPEECH_BONE_SENSOR)
    dump_ch += 1;
#endif
    //audio_dump_init(speech_tx_frame_len, sizeof(short), dump_ch);
    audio_dump_init(speech_tx_frame_len, sizeof(short), 6);
#endif

    AUDIOPLAYERS_TRACE(1,"[%s] End", __func__);

    return 0;
}

int speech_init(int tx_sample_rate, int rx_sample_rate,
                     int tx_frame_ms, int rx_frame_ms,
                     int sco_frame_ms,
                     uint8_t *buf, int len)
{
    int tx_frame_len = SPEECH_FRAME_MS_TO_LEN(tx_sample_rate, tx_frame_ms);
    int rx_frame_len = SPEECH_FRAME_MS_TO_LEN(rx_sample_rate, rx_frame_ms);
    int sco_frame_len = SPEECH_FRAME_MS_TO_LEN(tx_sample_rate, sco_frame_ms);

    return speech_init2(tx_sample_rate, rx_sample_rate,
                        tx_frame_len, rx_frame_len,
                        sco_frame_len,
                        buf, len);
}

int speech_tx_deinit(void)
{
    AUDIOPLAYERS_TRACE(1,"[%s] Start...", __func__);

#if defined(SPEECH_TX_EQ)
    eq_destroy(speech_tx_eq_st);
#endif

#if defined(SPEECH_TX_AGC)
    agc_state_destroy(speech_tx_agc_st);
#endif

#if defined(SPEECH_TX_1MIC_PREAF)
    speech_1mic_preaf_destroy(speech_tx_1mic_preaf_st);
#endif

#if defined(SPEECH_TX_3MIC_PREAF)
    speech_3mic_preaf_destroy(speech_tx_3mic_preaf_st);
#endif

#if defined(SPEECH_TX_3MIC_NS)
    speech_3mic_ns_destroy(speech_tx_3mic_ns_st);
#endif

#if defined(SPEECH_TX_3MIC_NS2)
    speech_3mic_ns2_destroy(speech_tx_3mic_ns2_st);
#endif

#if defined(SPEECH_TX_2MIC_NS4)
    sensormic_denoise_destroy(speech_tx_2mic_ns4_st);
#endif

#if defined(SPEECH_TX_2MIC_NS7)
    speech_heap_set_cp(1);
    speech_2mic_ns7_destroy(speech_tx_2mic_ns7_st);
    speech_heap_set_cp(0);
#endif

#if defined(SPEECH_TX_2MIC_PREAF)
    speech_2mic_preaf_destroy(speech_tx_2mic_preaf_st);
#endif

#if defined(SPEECH_TX_2MIC_NS4_PREAF)
    speech_2mic_ns4_preaf_destroy(speech_tx_2mic_ns4_preaf_st);
#endif

#if defined(SPEECH_TX_2MIC_NS8)
    speech_2mic_ns8_destroy(speech_tx_2mic_ns8_st);
#endif

#if defined(SPEECH_TX_NS2FLOAT)
    speech_ns2float_destroy(speech_tx_ns2float_st);
#endif

#ifdef SPEECH_TX_NS3
    ns3_destroy(speech_tx_ns3_st);
#endif

#ifdef SPEECH_TX_NS4
    speech_ns4_destroy(speech_tx_ns4_st);
#endif

#if defined(SPEECH_TX_1MIC_NS)
    speech_1mic_ns_destroy(speech_tx_1mic_ns_st);
#endif

#if defined(CHIP_BEST1307) && defined(SPEECH_TX_NS9)
    speech_ns9_destory(speech_tx_ns9_st);
#endif

#ifdef SPEECH_TX_COMPEXP
    compexp_destroy(speech_tx_compexp_st);
#endif

#if defined(SPEECH_TX_AEC2FLOAT)
    ec2float_destroy(speech_tx_aec2float_st);
#endif

#if defined(SPEECH_TX_REF)
    speech_free(aec_echo_buf_ptr);
#endif

#if defined(SPEECH_TX_REF) || defined(CHIP_BEST1307P) || defined(CHIP_BEST1307PH)
    speech_free(af_out_buf);
#endif

#if defined(SPEECH_TX_DC_FILTER)
    speech_dc_filter_destroy(speech_tx_dc_filter_st);
#endif

    AUDIOPLAYERS_TRACE(1,"[%s] End", __func__);

    return 0;
}

int speech_rx_deinit(void)
{
    AUDIOPLAYERS_TRACE(1,"[%s] Start...", __func__);

#if defined(SPEECH_RX_EQ)
    eq_destroy(speech_rx_eq_st);
#endif

#if defined(SPEECH_RX_HW_EQ)
    speech_hw_eq_destroy(speech_rx_hw_eq_st);
#endif

#if defined(SPEECH_RX_DAC_EQ)
    speech_dac_eq_destroy(speech_rx_dac_eq_st);
#endif

#if defined(SPEECH_RX_AGC)
    agc_state_destroy(speech_rx_agc_st);
#endif

#if defined(SPEECH_RX_COMPEXP)
    multi_compexp_destroy(speech_rx_compexp_st);
#endif

#if defined(SPEECH_RX_NS2FLOAT)
    speech_ns2float_destroy(speech_rx_ns2float_st);
#endif

#ifdef SPEECH_RX_NS3
    ns3_destroy(speech_rx_ns3_st);
#endif

    AUDIOPLAYERS_TRACE(1,"[%s] End", __func__);

    return 0;
}

int speech_deinit(void)
{
    AUDIOPLAYERS_TRACE(1,"[%s] Start...", __func__);

#if defined(SCO_CP_ACCEL)
    sco_cp_deinit();
#endif

#if defined(SPEECH_ALGO_DSP)
    if (g_mcpp_enable) {
        app_mcpp_close(APP_MCPP_USER_CALL);
        speech_enable_mcpp(false);
    }
#endif

    speech_rx_deinit();
    speech_tx_deinit();

#if defined(SPEECH_BONE_SENSOR)
    speech_free(vpu_buf);
#endif

#ifndef SCO_CP_ACCEL_FIFO
    if (speech_frame_resize_st != NULL) {
        frame_resize_destroy(speech_frame_resize_st);
        speech_tx_frame_resizer_enable = false;
        speech_rx_frame_resizer_enable = false;
        speech_frame_resize_st = NULL;
    }
#endif

#ifdef AUDIO_DEBUG
    speech_tuning_close();
#endif

#if defined(BT_SCO_CHAIN_AUDIO_DUMP)
    audio_dump_deinit();
#endif

    speech_free(speech_cfg);
    speech_cfg = NULL;

    size_t total = 0, used = 0, max_used = 0;
    speech_memory_info(&total, &used, &max_used);
    AUDIOPLAYERS_TRACE(3,"SPEECH MALLOC MEM: total - %d, used - %d, max_used - %d.", total, used, max_used);
    ASSERT(used == 0, "[%s] used != 0", __func__);

    AUDIOPLAYERS_TRACE(1,"[%s] End", __func__);

    return 0;
}

float speech_tx_get_required_mips(void)
{
    float mips = 0;

#if defined(SPEECH_TX_DC_FILTER)
    mips += speech_dc_filter_get_required_mips(speech_tx_dc_filter_st);
#endif

#if defined(SPEECH_TX_AEC2FLOAT)
    mips += ec2float_get_required_mips(speech_tx_aec2float_st);
#endif

#if defined(SPEECH_TX_2MIC_NS7)
    mips += speech_2mic_ns7_get_required_mips(speech_tx_2mic_ns7_st);
#endif

#if defined(SPEECH_TX_2MIC_NS8)
    mips += speech_2mic_ns8_get_required_mips(speech_tx_2mic_ns8_st);
#endif

#if defined(SPEECH_TX_2MIC_NS10)
    mips += speech_2mic_ns10_get_required_mips(speech_tx_2mic_ns10_st);
#endif

#if defined(SPEECH_TX_2MIC_NS4)
    mips += sensormic_denoise_get_required_mips(speech_tx_2mic_ns4_st);
#endif

#if defined(SPEECH_TX_3MIC_NS)
    mips += speech_3mic_get_required_mips(speech_tx_3mic_ns_st);
#endif

#if defined(SPEECH_TX_3MIC_NS2)
    mips += speech_3mic_ns2_get_required_mips(speech_tx_3mic_ns2_st);
#endif

#if defined(SPEECH_TX_NS2FLOAT)
    mips += speech_ns2float_get_required_mips(speech_tx_ns2float_st);
#endif

#if defined(SPEECH_TX_NS3)
    mips += ns3_get_required_mips(speech_tx_ns3_st);
#endif

#if defined(SPEECH_TX_COMPEXP)
    mips += compexp_get_required_mips(speech_tx_compexp_st);
#endif

#if defined(SPEECH_TX_AGC)
    mips += agc_get_required_mips(speech_tx_agc_st);
#endif

#if defined(SPEECH_TX_EQ)
    mips += eq_get_required_mips(speech_tx_eq_st);
#endif

    return mips;
}

float speech_rx_get_required_mips(void)
{
    float mips = 0;
#if defined(SPEECH_RX_NS2FLOAT)
    mips += speech_ns2float_get_required_mips(speech_rx_ns2float_st);
#endif

#if defined(SPEECH_RX_NS3)
    mips += ns3_get_required_mips(speech_rx_ns3_st);
#endif

#if defined(SPEECH_RX_AGC)
    mips += agc_get_required_mips(speech_rx_agc_st);
#endif

#if defined(SPEECH_RX_COMPEXP)
    mips += multi_compexp_get_required_mips(speech_rx_compexp_st);
#endif

#if defined(SPEECH_RX_EQ)
    mips += eq_get_required_mips(speech_rx_eq_st);
#endif

#if defined(SPEECH_RX_HW_EQ)
    mips += speech_hw_eq_get_required_mips(speech_rx_hw_eq_st);
#endif

#if defined(SPEECH_RX_DAC_EQ)
    mips += speech_dac_eq_get_required_mips(speech_rx_dac_eq_st);
#endif

    return mips;
}

float speech_get_required_mips(void)
{
    return speech_tx_get_required_mips() + speech_rx_get_required_mips();
}

#define SYSTEM_BASE_MIPS (18)

enum APP_SYSFREQ_FREQ_T speech_get_proper_sysfreq(int *needed_mips)
{
    enum APP_SYSFREQ_FREQ_T freq = APP_SYSFREQ_32K;
    int required_mips = (int)ceilf(speech_get_required_mips() + SYSTEM_BASE_MIPS);

    if (required_mips >= 104)
        freq = APP_SYSFREQ_208M;
    else if (required_mips >= 78)
        freq = APP_SYSFREQ_104M;
    else if (required_mips >= 52)
        freq = APP_SYSFREQ_78M;
    else if (required_mips >= 26)
        freq = APP_SYSFREQ_52M;
    else
        freq = APP_SYSFREQ_26M;

    *needed_mips = required_mips;

    return freq;
}

int speech_set_config(const SpeechConfig *cfg)
{
#if defined(SPEECH_TX_DC_FILTER)
    speech_dc_filter_set_config(speech_tx_dc_filter_st, &cfg->tx_dc_filter);
#endif
#if defined(SPEECH_TX_AEC2FLOAT)
    ec2float_set_config(speech_tx_aec2float_st, &cfg->tx_aec2float, SPEECH_TX_AEC2FLOAT_CORE);
#endif
#if defined(SPEECH_TX_2MIC_NS4)
    curr_mode = get_config_index_for_anc_mode(app_anc_table_get_custom_mode(app_anc_get_curr_mode()));
    sensormic_denoise_set_config(speech_tx_2mic_ns4_st, &cfg->tx_2mic_ns4[curr_mode]);
#endif
#if defined(SPEECH_TX_2MIC_NS7)
    //speech_2mic_ns7_set_config(speech_tx_2mic_ns7_st, &cfg->tx_2mic_ns7);
#endif
#if defined(SPEECH_TX_2MIC_PREAF)
    speech_2mic_preaf_set_config(speech_tx_2mic_preaf_st, &cfg->tx_2mic_preaf);
#endif
#if defined(SPEECH_TX_2MIC_NS8)
    speech_2mic_ns8_set_config(speech_tx_2mic_ns8_st, &cfg->tx_2mic_ns8, model_data_s);
#endif
#if defined(SPEECH_TX_2MIC_NS10)
    speech_2mic_ns10_set_config(speech_tx_2mic_ns10_st, &cfg->tx_2mic_ns10);
#endif
#if defined(SPEECH_TX_3MIC_PREAF) && defined(SPEECH_TX_3MIC_NS)
    curr_mode = get_config_index_for_anc_mode(app_anc_table_get_custom_mode(app_anc_get_curr_mode()));
    speech_3mic_preaf_set_config(speech_tx_3mic_preaf_st, &cfg->tx_3mic_preaf[curr_mode]);
#endif
#if defined(SPEECH_TX_3MIC_PREAF) && defined(SPEECH_TX_3MIC_NS2)
    speech_3mic_preaf_set_config(speech_tx_3mic_preaf_st, &cfg->tx_3mic_preaf[3]);
#endif
#if defined(SPEECH_TX_3MIC_NS)
    curr_mode = get_config_index_for_anc_mode(app_anc_table_get_custom_mode(app_anc_get_curr_mode()));
    speech_3mic_ns_set_config(speech_tx_3mic_ns_st, &cfg->tx_3mic_ns[curr_mode], model_data_s);
#endif
#if defined(SPEECH_TX_3MIC_NS2)
    speech_3mic_ns2_set_config(speech_tx_3mic_ns2_st, &cfg->tx_3mic_ns2);
#endif
#if defined(SPEECH_TX_NS2FLOAT)
    speech_ns2float_set_config(speech_tx_ns2float_st, &cfg->tx_ns2float, 0);
#endif
#if defined(SPEECH_TX_NS3)
    ns3_set_config(speech_tx_ns3_st, &cfg->tx_ns3);
#endif
#if defined(SPEECH_TX_NS4)
    //speech_ns4_set_config(speech_tx_ns4_st, &cfg->tx_ns4);
#endif
#if defined(SPEECH_TX_COMPEXP)
    compexp_set_config(speech_tx_compexp_st, &cfg->tx_compexp);
#endif
#if defined(SPEECH_TX_AGC)
    agc_set_config(speech_tx_agc_st, &cfg->tx_agc);
#endif
#if defined(SPEECH_TX_EQ)
    eq_set_config(speech_tx_eq_st, &cfg->tx_eq);
#endif
#if defined(SPEECH_RX_NS2FLOAT)
    speech_ns2float_set_config(speech_rx_ns2float_st, &cfg->rx_ns2float, SPEECH_RX_NS2FLOAT_CORE);
#endif
#if defined(SPEECH_RX_NS3)
    ns3_set_config(speech_rx_ns3_st, &cfg->rx_ns3);
#endif
#if defined(SPEECH_RX_COMPEXP)
    multi_compexp_set_config(speech_rx_compexp_st, &cfg->rx_compexp);
#endif
#if defined(SPEECH_RX_AGC)
    agc_set_config(speech_rx_agc_st, &cfg->rx_agc);
#endif
#if defined(SPEECH_RX_EQ)
    eq_set_config(speech_rx_eq_st, &cfg->rx_eq);
#endif
#if defined(SPEECH_RX_HW_EQ)
    speech_hw_eq_set_config(speech_rx_hw_eq_st, &cfg->rx_hw_eq);
#endif
#if defined(SPEECH_RX_DAC_EQ)
    speech_dac_eq_set_config(speech_rx_dac_eq_st, &cfg->rx_dac_eq);
#endif
    // Add more process

    return 0;
}

#ifdef BT_SCO_CHAIN_AUDIO_DUMP
static int16_t dump_data[256];
#endif
void _speech_tx_process_pre(short *pcm_buf, short *ref_buf, int *_pcm_len, sco_status_t *status)
{
    int pcm_len = *_pcm_len;
    int POSSIBLY_UNUSED frame_len = pcm_len / SPEECH_CODEC_CAPTURE_CHANNEL_NUM;

#if defined(BT_SCO_CHAIN_PROFILE)
    tx_start_ticks = hal_fast_sys_timer_get();
#endif

    // AUDIOPLAYERS_TRACE(2,"[%s] pcm_len = %d", __func__, pcm_len);

#ifdef AUDIO_DEBUG
    if (speech_tuning_get_status())
    {
        speech_set_config(speech_cfg);

        speech_tuning_set_status(false);

        // If has MIPS problem, can move this block code into speech_rx_process or return directly
        // return 0
    }
#endif

#if defined(BT_SCO_CHAIN_AUDIO_DUMP)
    audio_dump_clear_up();
    uint32_t POSSIBLY_UNUSED dump_index = 0;
#endif

// NOTE: If more case need this, remove MACRO control
#if defined(SPEECH_ALGO_DSP) || defined(SPEECH_BONE_SENSOR)
    ASSERT(frame_len == speech_tx_frame_len, "[%s] frame_len(%d) != speech_tx_frame_len(%d)", \
            __func__, frame_len, speech_tx_frame_len);
#endif

#if defined(SPEECH_BONE_SENSOR)
#if defined(SPEECH_TX_24BIT)
    speech_bone_sensor_get_data(vpu_buf, frame_len, SPEECH_BS_CHANNEL_X, 24);
#else
    speech_bone_sensor_get_data(vpu_buf, frame_len, SPEECH_BS_CHANNEL_X, 16);
#endif
#endif

#if defined(SPEECH_ALGO_DSP)
#if defined(BT_SCO_CHAIN_AUDIO_DUMP)
    dump_index = 0;
    for (uint32_t ch=0; ch<SPEECH_CODEC_CAPTURE_CHANNEL_NUM; ch++) {
        audio_dump_add_channel_data_from_multi_channels_32bit_to_16bit(dump_index++, pcm_buf, frame_len, SPEECH_CODEC_CAPTURE_CHANNEL_NUM, ch, 8);
    }
    if (ref_buf) {
        audio_dump_add_channel_data_from_multi_channels_32bit_to_16bit(dump_index++, ref_buf, frame_len, 1, 0, 8);
    }
#if defined(SPEECH_BONE_SENSOR)
    audio_dump_add_channel_data_from_multi_channels_32bit_to_16bit(dump_index++, vpu_buf, frame_len, 1, 0, 8);
#endif
#endif

    if (g_mcpp_enable) {
        APP_MCPP_CAP_PCM_T pcm_cfg = {0};
        memset(&pcm_cfg, 0, sizeof(pcm_cfg));

        pcm_cfg.ref = ref_buf;
#if defined(SPEECH_BONE_SENSOR)
        pcm_cfg.vpu = vpu_buf;
#else
        pcm_cfg.vpu = NULL;
#endif
        pcm_cfg.out = pcm_buf;
        pcm_cfg.in = pcm_buf;
        pcm_cfg.frame_len = frame_len;
        app_mcpp_capture_process(APP_MCPP_USER_CALL, &pcm_cfg);
    }

    *_pcm_len = frame_len;

#if defined(BT_SCO_CHAIN_AUDIO_DUMP)
    audio_dump_add_channel_data_from_multi_channels_32bit_to_16bit(dump_index++, pcm_buf, frame_len, 1, 0, 8);
    //audio_dump_run();
#endif
    return;
#endif

#if defined(SPEECH_TX_DC_FILTER)
#if defined(SPEECH_TX_24BIT)
    speech_dc_filter_process_int24(speech_tx_dc_filter_st, (int32_t *)pcm_buf, pcm_len);
#else
    speech_dc_filter_process(speech_tx_dc_filter_st, pcm_buf, pcm_len);
#endif
#endif

#if defined(SPEECH_TX_24BIT)
#if defined(SPEECH_TX_REF)
    arm_q23_to_q15((int32_t *)ref_buf, (int16_t *)ref_buf, pcm_len / SPEECH_CODEC_CAPTURE_CHANNEL_NUM);
#endif

    arm_q23_to_q15((int32_t *)pcm_buf, (int16_t *)pcm_buf, pcm_len);
#endif

#if defined(SPEECH_TX_2MIC_SWAP_CHANNELS)
    for (uint32_t i = 0; i < pcm_len; i += 2) {
        int16_t tmp = pcm_buf[i];
        pcm_buf[i] = pcm_buf[i + 1];
        pcm_buf[i + 1] = tmp;
    }
#endif

#if defined(SPEECH_TX_3MIC_SWAP_CHANNELS)
    for (uint32_t i = 0; i < pcm_len; i += 3) {
        int16_t tmp = pcm_buf[i];
        pcm_buf[i] = pcm_buf[i + 2];         // talk
        pcm_buf[i + 2] = pcm_buf[i + 1];     // ff
        pcm_buf[i + 1] = tmp;                // fb
    }
#endif

// #if defined(SPEECH_TX_3MIC_NS)
//     for (uint32_t i = 0; i < pcm_len; i += 3) {
//         pcm_buf[i] *= 4;
//         pcm_buf[i + 1] *= 8;
//         pcm_buf[i + 2] *= 8;
//     }
// #endif
    //audio_dump_add_channel_data_from_multi_channels(0, pcm_buf, pcm_len / 3, 3, 0);
    //audio_dump_add_channel_data_from_multi_channels(1, pcm_buf, pcm_len / 3, 3, 1);
    //audio_dump_add_channel_data_from_multi_channels(2, pcm_buf, pcm_len / 3, 3, 2);
//#endif

#if defined(BT_SCO_CHAIN_AUDIO_DUMP)
    for (uint32_t ch = 0; ch < SPEECH_CODEC_CAPTURE_CHANNEL_NUM; ch++) {
        audio_dump_add_channel_data_from_multi_channels(ch, pcm_buf, frame_len, SPEECH_CODEC_CAPTURE_CHANNEL_NUM, ch);
    }
    if (ref_buf) {
        audio_dump_add_channel_data(3, ref_buf, frame_len);
    }
#if defined(SPEECH_TX_3MIC_NS) || defined (SPEECH_TX_3MIC_PREAF)
    arm_fill_q15(5000 * last_anc_mode, dump_data, frame_len);
#endif
    audio_dump_add_channel_data(4, dump_data, frame_len);
#endif

#if defined(SPEECH_TX_2MIC_NS7)
    for (int i = 0; i < frame_len; i++){
        af_out_buf[i] = pcm_buf[2 * i + 1];
    }
#endif

#if defined(SPEECH_TX_1MIC_PREAF)
    speech_1mic_preaf_process(speech_tx_1mic_preaf_st, pcm_buf, ref_buf, pcm_len, af_out_buf);
#endif

#if defined(SPEECH_TX_1MIC_NS)
#if defined(SPEECH_TX_1MIC_PREAF)
    speech_1mic_ns_process(speech_tx_1mic_ns_st, pcm_buf, ref_buf, pcm_len, af_out_buf);//output is pcm_buf
    speech_copy_int16(pcm_buf, af_out_buf, pcm_len);
#else
    speech_1mic_ns_process(speech_tx_1mic_ns_st, pcm_buf, ref_buf, pcm_len, pcm_buf);
#endif
#endif

#if defined(CHIP_BEST1307) && defined(SPEECH_TX_NS9)
    speech_ns9_process(speech_tx_ns9_st, pcm_buf, ref_buf, pcm_len);
#endif

#if defined(SPEECH_TX_2MIC_PREAF)
    speech_2mic_preaf_process(speech_tx_2mic_preaf_st, pcm_buf, ref_buf, pcm_len, af_out_buf);
#endif

#if defined(SPEECH_TX_2MIC_NS4_PREAF)
#if defined(SPEECH_TX_2MIC_NS4)
    speech_2mic_ns4_preaf_set_wind_status(speech_tx_2mic_ns4_preaf_st, status->wind);
#endif
    curr_mode = get_config_index_for_anc_mode(app_anc_table_get_custom_mode(app_anc_get_curr_mode()));
    if (curr_mode != last_anc_mode) {
        AUDIOPLAYERS_TRACE(0, "[%s] mode: %d -> %d", __FUNCTION__, last_anc_mode, curr_mode);
        speech_2mic_ns4_preaf_set_config(speech_tx_2mic_ns4_preaf_st, &speech_cfg->tx_2mic_ns4_preaf[curr_mode]);
    }
    for (int32_t i = 0; i < pcm_len / 2; i++)
    {
        af_out_buf[3 * i + 0] = pcm_buf[2 * i + 0];
        af_out_buf[3 * i + 1] = pcm_buf[2 * i + 1];
        af_out_buf[3 * i + 2] = pcm_buf[2 * i + 1];
    }
    speech_2mic_ns4_preaf_process(speech_tx_2mic_ns4_preaf_st, pcm_buf, ref_buf, pcm_len, af_out_buf);
#endif

#if defined(SPEECH_TX_3MIC_NS) && !defined(SPEECH_ROM_PATCH_1307P_3MIC)
     speech_3mic_ns_preprocess(speech_tx_3mic_ns_st, pcm_buf, ref_buf, pcm_len, af_out_buf);
#endif

#if defined(SPEECH_TX_3MIC_PREAF)
#if defined(SPEECH_TX_3MIC_NS)
    speech_3mic_preaf_set_wind_status(speech_tx_3mic_preaf_st, status->wind);
#endif
#if defined(ANC_APP)
    curr_mode = get_config_index_for_anc_mode(app_anc_table_get_custom_mode(app_anc_get_curr_mode()));
#else
    curr_mode = 0;
#endif    
    if (curr_mode != last_anc_mode) {
        AUDIOPLAYERS_TRACE(0, "[%s] mode: %d -> %d", __FUNCTION__, last_anc_mode, curr_mode);
        speech_3mic_preaf_reset_config_with_mode(speech_tx_3mic_preaf_st, &speech_cfg->tx_3mic_preaf[curr_mode]);
    }
    speech_copy_int16(af_out_buf, pcm_buf, pcm_len);
    speech_3mic_preaf_process(speech_tx_3mic_preaf_st, pcm_buf, ref_buf, pcm_len, af_out_buf);
#endif

#if defined(SPEECH_TX_3MIC_PREAF) && defined(SPEECH_TX_3MIC_NS2)
    speech_3mic_preaf_set_wind_status(speech_tx_3mic_preaf_st, status->wind);
    speech_copy_int16(af_out_buf, pcm_buf, pcm_len);
    speech_3mic_preaf_process(speech_tx_3mic_preaf_st, pcm_buf, ref_buf, pcm_len, af_out_buf);
#endif

#if defined(SPEECH_TX_2MIC_NS7) || (SPEECH_TX_2MIC_NS8) || (SPEECH_TX_3MIC_NS)
#ifndef SCO_CP_ACCEL
    int16_t *fb_buf = af_out_buf;
#endif
#endif

    SCO_CP_ACCEL_ALGO_START();
#if defined(SPEECH_TX_2MIC_NS4)
    curr_mode = get_config_index_for_anc_mode(app_anc_table_get_custom_mode(app_anc_get_curr_mode()));
    if (curr_mode != last_anc_mode) {
        AUDIOPLAYERS_TRACE(0, "[%s] mode: %d -> %d", __FUNCTION__, last_anc_mode, curr_mode);
        sensormic_denoise_reset_config_with_mode(speech_tx_2mic_ns4_st,&speech_cfg->tx_2mic_ns4[curr_mode]);
        last_anc_mode = curr_mode;
    }
#if defined(SPEECH_TX_2MIC_NS4_PREAF)    
    sensormic_denoise_process(speech_tx_2mic_ns4_st, pcm_buf, ref_buf, pcm_len, fb_buf);
#else
    speech_copy_int16(fb_buf, pcm_buf, pcm_len);
    sensormic_denoise_process(speech_tx_2mic_ns4_st, pcm_buf, ref_buf, pcm_len, fb_buf);
#endif
    // Channel num: two-->one
    pcm_len = pcm_len / 2;
    speech_copy_int16(pcm_buf, fb_buf, pcm_len);
    if (wind_enable == true) {
        status->wind = sensormic_denoise_get_wind_status(speech_tx_2mic_ns4_st);
    }
#endif

#if defined(SPEECH_TX_2MIC_NS7)
    speech_2mic_ns7_process(speech_tx_2mic_ns7_st, pcm_buf, ref_buf, pcm_len, fb_buf);
    // Channel num: two-->one
    pcm_len >>= 1;
    for (int32_t i = 0; i < pcm_len; i++){
        pcm_buf[i] = fb_buf[i];
    }
#endif

#if defined(SPEECH_TX_2MIC_NS8)
#if defined(SPEECH_TX_2MIC_PREAF)
    speech_2mic_ns8_process(speech_tx_2mic_ns8_st, pcm_buf, ref_buf, pcm_len, fb_buf);
    speech_copy_int16(pcm_buf, fb_buf, pcm_len / 2);
#else
    speech_2mic_ns8_process(speech_tx_2mic_ns8_st, pcm_buf, ref_buf, pcm_len, pcm_buf);
#endif
    pcm_len >>= 1;
#endif

#if defined(SPEECH_TX_2MIC_NS10)
    speech_2mic_ns10_process(speech_tx_2mic_ns10_st, pcm_buf, ref_buf, pcm_len, pcm_buf);
    pcm_len >>= 1;
#endif

#if defined(SPEECH_TX_3MIC_NS)
#if defined(ANC_APP)
    curr_mode = get_config_index_for_anc_mode(app_anc_table_get_custom_mode(app_anc_get_curr_mode()));
#else
    curr_mode = 0;
#endif    
    if (curr_mode != last_anc_mode) {
        AUDIOPLAYERS_TRACE(0, "[%s] mode: %d -> %d", __FUNCTION__, last_anc_mode, curr_mode);
        speech_3mic_ns_reset_config_with_mode(speech_tx_3mic_ns_st,&speech_cfg->tx_3mic_ns[curr_mode]);
        last_anc_mode = curr_mode;
    }
    NO_AI = 0;
    if(NO_AI){
        speech_cfg->tx_3mic_ns[curr_mode].nn_gain_enable = 0;
    } else {
        speech_cfg->tx_3mic_ns[curr_mode].nn_gain_enable = 1;
    }
    speech_3mic_ns_reset_config_with_mode(speech_tx_3mic_ns_st,&speech_cfg->tx_3mic_ns[curr_mode]);
#if defined(SPEECH_TX_3MIC_PREAF)
    speech_3mic_ns_process(speech_tx_3mic_ns_st, pcm_buf, ref_buf, pcm_len, fb_buf);
    speech_copy_int16(pcm_buf, fb_buf, pcm_len / 3);
#else
    speech_3mic_ns_process(speech_tx_3mic_ns_st, pcm_buf, ref_buf, pcm_len, pcm_buf);
#endif
    if (wind_enable == true) {
        status->wind = speech_3mic_ns_get_wind_status(speech_tx_3mic_ns_st);
    }
    // Channel num: three-->one
    pcm_len = pcm_len / 3;
#endif

#if defined(SPEECH_TX_3MIC_NS2)
#ifndef SCO_CP_ACCEL
    int16_t *fb_buf = af_out_buf;
#endif
#ifndef SPEECH_TX_3MIC_PREAF
    speech_copy_int16(fb_buf, pcm_buf, pcm_len);
#endif
    speech_3mic_ns2_process(speech_tx_3mic_ns2_st, pcm_buf, ref_buf, pcm_len, fb_buf);
    pcm_len = pcm_len / 3;
    speech_copy_int16(pcm_buf, fb_buf, pcm_len);
        if (wind_enable == true) {
        status->wind = speech_3mic_ns2_get_wind_status(speech_tx_3mic_ns2_st);
    }
#endif
    SCO_CP_ACCEL_ALGO_END();

#if defined(SPEECH_TX_AEC2FLOAT)
    ec2float_process(speech_tx_aec2float_st, pcm_buf, ref_buf, pcm_len, af_out_buf);
    speech_copy_int16(pcm_buf, af_out_buf, pcm_len);
#endif

#if defined(SPEECH_TX_NS2FLOAT)
    speech_ns2float_process(speech_tx_ns2float_st, pcm_buf, pcm_len);
#endif


#if defined(SPEECH_TX_NS3)
    ns3_process(speech_tx_ns3_st, pcm_buf, pcm_len);
#endif

#if defined(SPEECH_TX_NS4)
    speech_ns4_process(speech_tx_ns4_st, pcm_buf, pcm_len);
#endif

#if defined(BT_SCO_CHAIN_AUDIO_DUMP)
    //audio_dump_add_channel_data(4, pcm_buf, pcm_len);
#endif
//
#if defined(SPEECH_TX_COMPEXP)
    compexp_process(speech_tx_compexp_st, pcm_buf, pcm_len);
#endif

#if defined(SPEECH_TX_AGC)
    agc_process(speech_tx_agc_st, pcm_buf, pcm_len);
#endif

#if defined(SPEECH_TX_EQ)
    eq_process(speech_tx_eq_st, pcm_buf, pcm_len);
#endif

#if defined(TWS_SWITCH_ACCORDING_NOISE)
    app_ibrt_ui_noise_reset();
#if defined(SPEECH_TX_NS3)
    float noise_data = ns3_get_noise_power(speech_tx_ns3_st, 300, 1000);
    float snr_data = ns3_get_snr(speech_tx_ns3_st, 300, 1000);
#else
    float noise_data = ec2float_get_noise_power(speech_tx_aec2float_st);
    float snr_data = ec2float_get_snr(speech_tx_aec2float_st);
#endif
    app_ibrt_ui_noise_process(noise_data, snr_data);
#endif
  
#if defined(BT_SCO_CHAIN_AUDIO_DUMP)
    audio_dump_add_channel_data(5, pcm_buf, pcm_len);
    audio_dump_run();
#endif

#if defined(SPEECH_TX_24BIT)
    arm_q15_to_q23((int16_t *)pcm_buf, (int32_t *)pcm_buf, pcm_len);
#endif

    *_pcm_len = pcm_len;

#if defined(BT_SCO_CHAIN_PROFILE)
    tx_end_ticks = hal_fast_sys_timer_get();
    AUDIOPLAYERS_TRACE(2,"[%s] takes %d us", __FUNCTION__,
          FAST_TICKS_TO_US(tx_end_ticks - tx_start_ticks));
#endif
}

static POSSIBLY_UNUSED NOINLINE void test_cp_crash(void)
{
    AUDIOPLAYERS_TRACE(1, "%s", __FUNCTION__);
}

#if defined(SCO_CP_ACCEL)
CP_TEXT_SRAM_LOC
int sco_cp_algo(short *pcm_buf, short *ref_buf, short *fb_buf, int *_pcm_len, sco_status_t *status)
{
#if defined(SCO_TRACE_CP_ACCEL)
    AUDIOPLAYERS_TRACE(1,"[%s] ...", __func__);
#endif

    //AUDIOPLAYERS_TRACE(0,"[%s] cp vad = %d", __FUNCTION__, status->vad);

    // test_cp_crash();

    _speech_tx_process_mid(pcm_buf, ref_buf, fb_buf, _pcm_len, status);

    return 0;
}
#endif

int32_t _speech_tx_process_(void *pcm_buf, void *ref_buf, int32_t *_pcm_len)
{
#if defined(BESUI_TWS_EN) || defined(BESUI_STEREO_EN) || defined(USER_FACTORY_TRACE_RX_EN)
#if (SPEECH_CODEC_CAPTURE_CHANNEL_NUM >= 2)
	short *pcm_buf1 = (short *)pcm_buf;
#endif
    if(app_get_mic_hfp_enc_onoff())
    {
        _speech_tx_process_pre(pcm_buf, ref_buf, (int *)_pcm_len, &status);
#if defined(SCO_CP_ACCEL)
        sco_cp_process(pcm_buf, ref_buf, af_out_buf, (int *)_pcm_len, &status);
        _speech_tx_process_post(pcm_buf, ref_buf, (int *)_pcm_len, &status);
#endif
    }
    else
    {
#if (SPEECH_CODEC_CAPTURE_CHANNEL_NUM >= 2)
        int pcm_len = *_pcm_len;
        for(uint32_t i=0; i<pcm_len/2; i++)
		{
			if(app_get_mic_hfp_enc_test_type() == ONLY_OPEN_MASTER_MIC) 
			{	
                BESUI_TRACE(0,"[UISCO]ENC TEST 1");
			    //pcm_buf1[i] = pcm_buf1[2*i];      // Left channel
                pcm_buf1[i] = pcm_buf1[2*i+1]; // Right channel
			}
			else if(app_get_mic_hfp_enc_test_type() == ONLY_OPEN_SLAVE_MIC) 
			{		
                BESUI_TRACE(0,"[UISCO]ENC TEST 2");	
                pcm_buf1[i] = pcm_buf1[2*i];      // Left channel
			    //pcm_buf1[i] = pcm_buf1[2*i+1]; // Right channel
			} 
		}
        pcm_len >>= 1;	
       *_pcm_len = pcm_len;	
#if defined(SCO_CP_ACCEL)
       _speech_tx_process_post(pcm_buf1, ref_buf, (int *)_pcm_len, &status);
#endif
#else
#if defined(SCO_CP_ACCEL)
	    short *pcm_buf1 = (short *)pcm_buf;
       _speech_tx_process_post(pcm_buf1, ref_buf, (int *)_pcm_len, &status);
#endif
#endif
    }
#else
    _speech_tx_process_pre(pcm_buf, ref_buf, (int *)_pcm_len, &status);
#if defined(SCO_CP_ACCEL)
    sco_cp_process(pcm_buf, ref_buf, af_out_buf, (int *)_pcm_len, &status);
    _speech_tx_process_post(pcm_buf, ref_buf, (int *)_pcm_len, &status);
#endif
#endif
    return 0;
}

int32_t _speech_rx_process_(void *pcm_buf, int32_t *_pcm_len)
{
    int32_t pcm_len = *_pcm_len;

#if defined(SPEECH_ALGO_DSP)
    if (g_mcpp_enable) {
        APP_MCPP_PLAY_PCM_T pcm_cfg;
        memset(&pcm_cfg, 0, sizeof(pcm_cfg));
        pcm_cfg.in = pcm_buf;
        pcm_cfg.out = pcm_buf;
        pcm_cfg.frame_len = pcm_len;
        app_mcpp_playback_process(APP_MCPP_USER_CALL, &pcm_cfg);
    }
#endif

#if defined(BT_SCO_CHAIN_PROFILE)
    uint32_t start_ticks = hal_fast_sys_timer_get();
#endif

#if defined(BT_SCO_CHAIN_AUDIO_DUMP)
    // audio_dump_add_channel_data(0, pcm_buf, pcm_len);
#endif

#if defined(SPEECH_RX_24BIT)
#if defined(SPEECH_RX_NS2FLOAT)
#ifdef BT_HFP_SUPPORT
    extern bool btapp_hfp_is_call_active(void);
    extern bool btapp_hfp_is_pc_call_active(void);
    if (btapp_hfp_is_pc_call_active() || btapp_hfp_is_call_active())
    {
        speech_ns2float_set_ringtone_mode(speech_rx_ns2float_st, false);
    }
    else
#endif
    {
        speech_ns2float_set_ringtone_mode(speech_rx_ns2float_st, true);
    }

#ifdef BT_SCO_LOW_RAM
    int32_t *pcm_buf32 = (int32_t *)pcm_buf;
    if (16000 == speech_rx_sample_rate)
    {
        speech_ns2float_process_int24(speech_rx_ns2float_st, pcm_buf32, pcm_len/2);
        speech_ns2float_process_int24(speech_rx_ns2float_st, pcm_buf32+pcm_len/2, pcm_len/2);
    }
    else
    {
        speech_ns2float_process_int24(speech_rx_ns2float_st, pcm_buf, pcm_len);
    }
#else
    speech_ns2float_process_int24(speech_rx_ns2float_st, pcm_buf, pcm_len);
#endif
#endif

#if defined(SPEECH_RX_COMPEXP)
    multi_compexp_process_int24(speech_rx_compexp_st, pcm_buf, pcm_len);
#endif

#if defined(SPEECH_RX_EQ)
    eq_process_int24(speech_rx_eq_st, pcm_buf, pcm_len);
#endif

#if defined(SPEECH_RX_HW_EQ)
    speech_hw_eq_process_int24(speech_rx_hw_eq_st, pcm_buf, pcm_len);
#endif

#else
#if defined(SPEECH_RX_NS2FLOAT)
    // FIXME
    int16_t *pcm_buf16 = (int16_t *)pcm_buf;
    for(int i=0; i<pcm_len; i++)
    {
        pcm_buf16[i] = (int16_t)(pcm_buf16[i] * 0.94);
    }
#ifdef BT_SCO_LOW_RAM
    if (16000 == speech_rx_sample_rate)
    {
        speech_ns2float_process(speech_rx_ns2float_st, pcm_buf16, pcm_len/2);
        speech_ns2float_process(speech_rx_ns2float_st, pcm_buf16+pcm_len/2, pcm_len/2);
    }
    else
    {
        speech_ns2float_process(speech_rx_ns2float_st, pcm_buf, pcm_len);
    }
#else
    speech_ns2float_process(speech_rx_ns2float_st, pcm_buf, pcm_len);
#endif
#endif

#ifdef SPEECH_RX_NS3
    ns3_process(speech_rx_ns3_st, pcm_buf, pcm_len);
#endif

#if defined(SPEECH_RX_AGC)
    agc_process(speech_rx_agc_st, pcm_buf, pcm_len);
#endif

#if defined(SPEECH_RX_COMPEXP)
    multi_compexp_process(speech_rx_compexp_st, pcm_buf, pcm_len);
#endif

#if defined(SPEECH_RX_EQ)
    eq_process(speech_rx_eq_st, pcm_buf, pcm_len);
#endif

#if defined(SPEECH_RX_HW_EQ)
    speech_hw_eq_process(speech_rx_hw_eq_st, pcm_buf, pcm_len);
#endif
#endif

#if defined(BT_SCO_CHAIN_AUDIO_DUMP)
    // audio_dump_add_channel_data(1, pcm_buf, pcm_len);
    // audio_dump_run();
#endif

    *_pcm_len = pcm_len;

#if defined(BT_SCO_CHAIN_PROFILE)
    uint32_t end_ticks = hal_fast_sys_timer_get();
    AUDIOPLAYERS_TRACE(2,"[%s] takes %d us", __FUNCTION__,
        FAST_TICKS_TO_US(end_ticks - start_ticks));
#endif

    return 0;
}

int speech_tx_process(void *pcm_buf, void *ref_buf, int *pcm_len)
{
#ifdef SCO_CP_ACCEL_FIFO
    _speech_tx_process_(pcm_buf, ref_buf, (int32_t *)pcm_len);
#else
    if (speech_tx_frame_resizer_enable == false) {
        _speech_tx_process_(pcm_buf, ref_buf, (int32_t *)pcm_len);
    } else {
        // MUST use (int32_t *)??????
        frame_resize_process_capture(speech_frame_resize_st, pcm_buf, ref_buf, (int32_t *)pcm_len);
    }
#endif

    return 0;
}

int speech_rx_process(void *pcm_buf, int *pcm_len)
{
#ifdef SCO_CP_ACCEL_FIFO
    _speech_rx_process_(pcm_buf, (int32_t *)pcm_len);
#else
    if (speech_rx_frame_resizer_enable == false) {
        _speech_rx_process_(pcm_buf, (int32_t *)pcm_len);
    } else {
        frame_resize_process_playback(speech_frame_resize_st, pcm_buf, (int32_t *)pcm_len);
    }
#endif

    return 0;
}

int speech_tx_process_audio_dump(void *pcm_buf, int *_pcm_len, int channel_num)
{
#if defined(BT_SCO_CHAIN_AUDIO_DUMP)
	//AUDIOPLAYERS_TRACE(1,"[%s] ...channel_num: %d", __func__, channel_num);
	// int pcm_len = *_pcm_len;

	// for(int i = 0; i < channel_num; i++) {
	// 	audio_dump_add_channel_data_from_multi_channels(i, pcm_buf, pcm_len/channel_num, channel_num, (i%2));
	// }
    // audio_dump_run();
#endif
    return 0;
}
