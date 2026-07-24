/***************************************************************************
 *
 * Copyright 2015-2024 BES.
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
#include "cmsis_os.h"
#include "tx_algo.h"
#include "hal_trace.h"
#include "speech_memory.h"
#include "audio_datapath_adc.h"
#include "app_wireless_mic.h"
#include "audio_datapath_manager.h"
#ifdef AUDIO_ALGO_DSP
#include "vowlan_mcu2dsp_adapter.h"
#endif
#ifdef UTILS_ESHELL_EN
#include "eshell.h"
#endif
#include "speech_eq.h"

#ifdef SPEECH_TX_1MIC_NS_48KF
#include "audio_algo_ns_adapter.h"
#endif

#ifdef SPEECH_TX_1MIC_NS_48KF
#define SPEECH_ALGO_POOL_SIZE                   (1024 * 120)
#else
#define SPEECH_ALGO_POOL_SIZE                   (1024 * 50)
#endif

#define TX_FRAME_SIZE                           (AUDIO_CODEC_CAPTURE_STREAM_DMA_BUFF_SIZE / AUDIO_CODEC_CAPTURE_STREAM_SAMPLE_BYTES / \
                                                 AUDIO_CODEC_CAPTURE_STREAM_CHANNEL_NUM / 2)

static uint8_t speech_algo_buf[SPEECH_ALGO_POOL_SIZE];
// static float out_pcm[TX_FRAME_SIZE];
static float *out_pcm = NULL;
static bool tx_algo_inited = false;
static bool tx_algo_on = true;

static int tx_hdr_mode = 0;
static int tx_eq_mode = 0;
static int tx_ga_mode = 0;
static float tx_pre_gain = 0.0f;
static float tx_post_gain = 0.0f;

static AudioHdrState *hdr_st = NULL;
static MultiPointDrcState *tx_mpdrc_st = NULL;
static SwGainState *tx_pre_gain_st = NULL;
static SwGainState *tx_post_gain_st = NULL;
static EqState *tx_dc_filter_eq_st = NULL;
static EqState *tx_eq_st = NULL;

static TX_ALGO_EMMC_DUMP_CALLBACK tx_algo_emmc_dump_orig_24bit = NULL;
static TX_ALGO_EMMC_DUMP_CALLBACK tx_algo_emmc_dump_algo_24bit = NULL;
static TX_ALGO_EMMC_DUMP_CALLBACK tx_algo_emmc_dump_algo_32bit = NULL;

static osMutexId tx_algo_mutex_id = NULL;
static const osMutexAttr_t tx_algo_mutex_attr = {
    .name = "tx_algo_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit | osMutexRobust,
    .cb_mem = NULL,
    .cb_size = 0U,
};

const EqConfig tx_dc_filter_eq_cfg = {
    .bypass = 0,
    .gain   = 0.f,
    .num    = 1,
    .params = {
        {IIR_BIQUARD_HPF, {{20, 0, 0.707f}}},
    },
};

const AudioHdrConfig hdr_config_merge =
{
    .hdr_mode   = 0,
    .max_lin_thd = 0.708, // -3db
    .gain_diff = 10, // 20db
    .window_time = 400,
    .fade_time = 5,
};

const AudioHdrConfig hdr_config_lowgain_chn =
{
    .hdr_mode   = 1,
    .max_lin_thd = 0.562, // -5db
    .gain_diff = 10, // 20db
    .window_time = 400,
    .fade_time = 5,
};

const AudioHdrConfig hdr_config_highgain_chn =
{
    .hdr_mode   = 2,
    .max_lin_thd = 0.562, // -5db
    .gain_diff = 10, // 20db
    .window_time = 400,
    .fade_time = 5,
};

const MultiPointDrcConfig tx_mpdrc_config0 =
{
    .x1 = { -80.0, -80.0 },
    .x2 = { -50.0, -50.0 },
    .x3 = { -40.0, -40.0 },
    .x4 = { -30.0, -30.0 },
    .x5 = { -5.0, -5.0 },
    .x6 = { -3.0, -5.0 },
    .attack_time = 3,
    .release_time = 60,
};

const MultiPointDrcConfig tx_mpdrc_config1 =
{
    .x1 = { -80.0, -80.0 },
    .x2 = { -50.0, -50.0 },
    .x3 = { -35.0, -15.0 },
    .x4 = { -15.0, -15.0 },
    .x5 = { -2.0, -2.0 },
    .x6 = { -0.0, -2.0 },
    .attack_time = 50,
    .release_time = 600,
};

const MultiPointDrcConfig tx_mpdrc_config2 =
{
    .x1 = { -80.0, -80.0 },
    .x2 = { -50.0, -50.0 },
    .x3 = { -40.0, -40.0 },
    .x4 = { -30.0, -30.0 },
    .x5 = { -20.0, -20.0 },
    .x6 = { -0.0, -0.0 },
    .attack_time = 3,
    .release_time = 60,
};

const MultiPointDrcConfig tx_mpdrc_config3 =
{
    .x1 = { -80.0, -80.0 },
    .x2 = { -50.0, -50.0 },
    .x3 = { -40.0, -40.0 },
    .x4 = { -30.0, -30.0 },
    .x5 = { -20.0, -20.0 },
    .x6 = { -0.0, -0.0 },
    .attack_time = 3,
    .release_time = 60,
};

const EqConfig tx_eq_cfg_default = {
    .bypass = 0,
    .gain = 0,
    .num = 1,
    .params = {
        {IIR_BIQUARD_PEAKINGEQ, {{600, 8.0, 4}}},
    }
};

const EqConfig tx_eq_cfg_LC = {
    .bypass = 0,
    .gain = 0,
    .num = 1,
    .params = {
        {IIR_BIQUARD_HPF, {{200, 0.0, 1}}},
    }
};

const EqConfig tx_eq_cfg_off = {
    .bypass = 0,
    .gain = 0,
    .num = 0,
    .params = {
        {IIR_BIQUARD_PEAKINGEQ, {{600, 0.0, 4}}},
    }
};

void tx_algo_emmc_dump_orig_24bit_register(TX_ALGO_EMMC_DUMP_CALLBACK cb)
{
    tx_algo_emmc_dump_orig_24bit = cb;
}

void tx_algo_emmc_dump_orig_24bit_degister(void)
{
    tx_algo_emmc_dump_orig_24bit = NULL;
}

void tx_algo_emmc_dump_algo_24bit_register(TX_ALGO_EMMC_DUMP_CALLBACK cb)
{
    tx_algo_emmc_dump_algo_24bit = cb;
}

void tx_algo_emmc_dump_algo_24bit_degister(void)
{
    tx_algo_emmc_dump_algo_24bit = NULL;
}

void tx_algo_emmc_dump_algo_32bit_register(TX_ALGO_EMMC_DUMP_CALLBACK cb)
{
    tx_algo_emmc_dump_algo_32bit = cb;
}

void tx_algo_emmc_dump_algo_32bit_degister(void)
{
    tx_algo_emmc_dump_algo_32bit = NULL;
}

int tx_hdr_mode_get(void)
{
    return tx_hdr_mode;
}

int tx_ga_mode_get(void)
{
    return tx_ga_mode;
}

int tx_eq_mode_get(void)
{
    return tx_eq_mode;
}

float tx_pre_gain_get(void)
{
    return tx_pre_gain;
}

float tx_post_gain_get(void)
{
    return tx_post_gain;
}

void tx_hdr_mode_set(int mode)
{
    if (!tx_algo_inited) {
        TRACE(0, "tx algo not inited!!!");
        return ;
    }
    tx_hdr_mode = mode;
    switch (tx_hdr_mode) {
    case 0:
        audio_hdr_set_config(hdr_st, &hdr_config_merge);
        break;
    case 1:
        audio_hdr_set_config(hdr_st, &hdr_config_lowgain_chn);
        break;
    case 2:
        audio_hdr_set_config(hdr_st, &hdr_config_highgain_chn);
        break;
    default:
        tx_hdr_mode = 0;
        audio_hdr_set_config(hdr_st, &hdr_config_merge);
        break;
    }
    TRACE(1, "tx hdr mode set is: %d", tx_hdr_mode);
}

void tx_ga_mode_set(int mode)
{
    if (!tx_algo_inited) {
        TRACE(0, "tx algo not inited!!!");
        return ;
    }
    tx_ga_mode = mode;
    switch (tx_ga_mode) {
    case 0:
        multipoint_drc_set_config(tx_mpdrc_st, &tx_mpdrc_config0);
        break;
    case 1:
        multipoint_drc_set_config(tx_mpdrc_st, &tx_mpdrc_config1);
        break;
    case 2:
        multipoint_drc_set_config(tx_mpdrc_st, &tx_mpdrc_config2);
        break;
    case 3:
        multipoint_drc_set_config(tx_mpdrc_st, &tx_mpdrc_config3);
        break;
    default:
        tx_ga_mode = 3;
        multipoint_drc_set_config(tx_mpdrc_st, &tx_mpdrc_config3);
        break;
    }
    TRACE(1, "tx ga mode set is: %d", tx_ga_mode);
}

void tx_eq_mode_set(int mode)
{
    if (!tx_algo_inited) {
        TRACE(0, "tx algo not inited!!!");
        return ;
    }
    tx_eq_mode = mode;
    switch (tx_eq_mode) {
    case 0:
        eq_set_config(tx_eq_st, &tx_eq_cfg_default);
        break;
    case 1:
        eq_set_config(tx_eq_st, &tx_eq_cfg_LC);
        break;
    case 2:
        eq_set_config(tx_eq_st, &tx_eq_cfg_off);
        break;
    default:
        tx_eq_mode = 2;
        eq_set_config(tx_eq_st, &tx_eq_cfg_off);
        break;
    }
    TRACE(1, "tx eq mode set is: %d", tx_eq_mode);
}

void tx_pre_gain_set(float gain)
{
    if (!tx_algo_inited) {
        TRACE(0, "tx algo not inited!!!");
        return ;
    }
    tx_pre_gain = gain;
    sw_gain_set_config(tx_pre_gain_st, tx_pre_gain);
    TRACE(1, "tx pre gain set is: (f*1000):%d", (uint32_t)(tx_pre_gain*1000));
}

void tx_post_gain_set(float gain)
{
    if (!tx_algo_inited) {
        TRACE(0, "tx algo not inited!!!");
        return ;
    }
    tx_post_gain = gain;
    sw_gain_set_config(tx_post_gain_st, tx_post_gain);
    TRACE(1, "tx post gain set is: (f*1000):%d", (uint32_t)(tx_post_gain*1000));
}

void tx_algo_init(enum AUD_SAMPRATE_T sample_rate, int32_t frame_size, enum AUD_BITS_T sample_bit, enum AUD_CHANNEL_NUM_T ch_num, bool heap_inited)
{
    TRACE(1, "[%s]...", __func__);

    if (tx_algo_inited) {
        return;
    }

    if (tx_algo_mutex_id == NULL) {
        tx_algo_mutex_id = osMutexNew(&tx_algo_mutex_attr);
        ASSERT(tx_algo_mutex_id, "create tx algo mutex failed.");
    }

    osMutexAcquire(tx_algo_mutex_id, osWaitForever);

    if (heap_inited) {
        TRACE(1, "[%s] heap is inited, no need to re-apply...", __func__);
    } else {
        speech_heap_init(speech_algo_buf, SPEECH_ALGO_POOL_SIZE);
    }

#ifdef AUDIO_ALGO_DSP
    if (audio_datapath_codec_capture_stream_dsp_algo_onoff_get()) {
        audio_manager_codec_capture_stream_dsp_algo_start_shot();
    }
#endif

    custom_allocator *allocator = default_allocator();
    ASSERT(allocator, "invalid allocator");

    tx_dc_filter_eq_st = multi_eq_init_with_custom_allocator(sample_rate, frame_size*ch_num, ch_num, &tx_dc_filter_eq_cfg, allocator);

    out_pcm = (float *)speech_calloc(frame_size, sizeof(float));

    hdr_st = audio_hdr_create(sample_rate, frame_size, sample_bit, ch_num, &hdr_config_merge, allocator);

    tx_eq_st = multi_eq_init_with_custom_allocator(sample_rate, frame_size, 1, &tx_eq_cfg_default, allocator);

    tx_pre_gain_st = sw_gain_create(sample_rate, frame_size, 1, 0.0f, allocator);
    tx_post_gain_st = sw_gain_create(sample_rate, frame_size, 1, 0.0f, allocator);

    tx_mpdrc_st = multipoint_drc_create(sample_rate, frame_size, 1, &tx_mpdrc_config0, allocator);

    multipoint_drc_switch(tx_mpdrc_st, false);

#ifdef SPEECH_TX_1MIC_NS_48KF
    audio_algo_ns_init();
#endif

    tx_algo_inited = true;

    osMutexRelease(tx_algo_mutex_id);
}

void tx_algo_deinit(void)
{
    TRACE(1, "[%s]...", __func__);

    if (!tx_algo_inited) {
        return;
    }

    osMutexAcquire(tx_algo_mutex_id, osWaitForever);

#ifdef AUDIO_ALGO_DSP
    if (audio_datapath_codec_capture_stream_dsp_algo_onoff_get()) {
        audio_manager_codec_capture_stream_dsp_algo_stop_shot();
    }
#endif
    if (tx_dc_filter_eq_st) {
        eq_destroy(tx_dc_filter_eq_st);
    }

    if (hdr_st) {
        audio_hdr_destroy(hdr_st);
        hdr_st = NULL;
    }

    if (tx_mpdrc_st) {
        multipoint_drc_destroy(tx_mpdrc_st);
        tx_mpdrc_st = NULL;
    }

    if (tx_pre_gain_st) {
        sw_gain_destroy(tx_pre_gain_st);
        tx_pre_gain_st = NULL;
    }

    if (tx_post_gain_st) {
        sw_gain_destroy(tx_post_gain_st);
        tx_post_gain_st = NULL;
    }

#ifdef SPEECH_TX_1MIC_NS_48KF
    audio_algo_ns_deinit();
#endif

    if (tx_eq_st) {
        eq_destroy(tx_eq_st);
    }

    speech_free(out_pcm);

    tx_algo_inited = false;

    osMutexRelease(tx_algo_mutex_id);
}

void tx_algo_process(uint8_t *buff, uint32_t *len)
{
    if (!tx_algo_inited) {
        return;
    }

    if (tx_algo_on) {
        uint32_t pcm_len = *len / 4;
        *len = *len / 2;

        eq_process_int24(tx_dc_filter_eq_st, (int32_t *)buff, pcm_len);

        if (audio_datapath_codec_capture_stream_mute_cnt_get() > 0) {
            return;
        }

        audio_hdr_process(hdr_st, (uint8_t*)buff, out_pcm, pcm_len);
        pcm_len = pcm_len / 2;

        if (tx_algo_emmc_dump_orig_24bit) {
            float_to_int(out_pcm, (int32_t *)buff, pcm_len);
            tx_algo_emmc_dump_orig_24bit(buff, *len);
        }

        eq_process_float(tx_eq_st, out_pcm, pcm_len);

        sw_gain_process(tx_pre_gain_st, out_pcm, pcm_len);

#ifdef AUDIO_ALGO_DSP
        if (vowlan_dsp_hifi4_is_runnig() && audio_datapath_codec_capture_stream_dsp_algo_onoff_get()) {
            int ret = vowlan_dsp_hifi4_algo_proc_req(true, (void*)out_pcm, *len);
            ASSERT(ret == 0, "dsp algo proc err:%d", ret);
        }
#endif

#ifdef SPEECH_TX_1MIC_NS_48KF
        if (audio_datapath_codec_capture_stream_ns_algo_onoff_get()) {
            audio_algo_ns_process(out_pcm, *len);
        }
#endif

        sw_gain_process(tx_post_gain_st, out_pcm, pcm_len);

        if (tx_algo_emmc_dump_algo_32bit) {
            float_to_int(out_pcm, (int32_t *)buff, pcm_len);
            tx_algo_emmc_dump_algo_32bit(buff, *len);
        }

        multipoint_drc_process(tx_mpdrc_st, out_pcm, pcm_len);
        float_to_int(out_pcm, (int32_t *)buff, pcm_len);

        if (tx_algo_emmc_dump_algo_24bit) {
            tx_algo_emmc_dump_algo_24bit(buff, *len);
        }
    } else if (AUDIO_CODEC_CAPTURE_STREAM_CHANNEL_NUM == AUD_CHANNEL_NUM_2) {
        int32_t* s = (int32_t*)buff;
        int32_t* d = (int32_t*)buff;
        uint32_t i = *len / 4 / 2;

        for (; i > 0; i--) {
            d[0] = (s[0] + s[1]);
            d += 1;
            s += 2;
        }
        *len = *len / 2;

#ifdef AUDIO_ALGO_DSP
        if (vowlan_dsp_hifi4_is_runnig() && audio_datapath_codec_capture_stream_dsp_algo_onoff_get()) {
            int ret = vowlan_dsp_hifi4_algo_proc_req(true, (void*)buff, *len);
            ASSERT(ret == 0, "dsp algo proc err:%d", ret);
        }
#endif
    } else {
        ASSERT(0, "%s invalid ch num.", __func__);
    }
}

#ifdef UTILS_ESHELL_EN
static void tx_algo_onoff_cmd(int argc, char *argv[])
{
    bool onoff = 0;

    if (argc != 2) {
        LOG_ERR(0, "error arg.");
        return ;
    }

    onoff = atoi(argv[1]);
    tx_algo_on = onoff;

    TRACE(1, "tx algo onoff:%d", onoff);
}

static void tx_algo_sw_pre_onoff_cmd(int argc, char *argv[])
{
    int flag = 0;

    if (argc != 2) {
        TRACE(0, "error arg.");
        return ;
    }

    flag = atoi(argv[1]);
    sw_gain_switch(tx_pre_gain_st, flag);
    TRACE(1, "tx_algo_sw_pre_gain_onoff = %d", flag);
}

static void tx_algo_sw_post_onoff_cmd(int argc, char *argv[])
{
    int flag = 0;

    if (argc != 2) {
        TRACE(0, "error arg.");
        return ;
    }

    flag = atoi(argv[1]);
    sw_gain_switch(tx_post_gain_st, flag);
    TRACE(1, "tx_algo_sw_post_gain_onoff = %d", flag);
}

static void tx_algo_mpdrc_onoff_cmd(int argc, char *argv[])
{
    int flag = 0;

    if (argc != 2) {
        TRACE(0, "error arg.");
        return ;
    }

    flag = atoi(argv[1]);
    multipoint_drc_switch(tx_mpdrc_st, flag);
    TRACE(1, "tx_algo_mpdrc_onoff = %d", flag);
}

static void tx_algo_iir_onoff_cmd(int argc, char *argv[])
{
    int flag = 0;

    if (argc != 2) {
        TRACE(0, "error arg.");
        return ;
    }

    flag = atoi(argv[1]);
    iir_switch(flag);
    TRACE(1, "tx_algo_iir_onoff = %d", flag);
}

static void tx_algo_sw_pre_config_cmd(int argc, char *argv[])
{
    float gain = 0.0f;

    if (argc != 2) {
        TRACE(0, "error arg.");
        return ;
    }

    gain = (float)atof(argv[1]);
    if (abs(gain) >= 50.0f) {
        TRACE(0, "sw_gain should between (-50.0, 50.0)");
        return ;
    }

    TRACE(1, "tx_pre_gain = (f*1000):%d", (uint32_t)(gain*1000));
    tx_pre_gain_set(gain);
}

static void tx_algo_sw_post_config_cmd(int argc, char *argv[])
{
    float gain = 0.0f;

    if (argc != 2) {
        TRACE(0, "error arg.");
        return ;
    }

    gain = (float)atof(argv[1]);
    if (abs(gain) >= 50.0f) {
        TRACE(0, "sw_gain should between (-50.0, 50.0)");
        return ;
    }

    TRACE(1, "tx_post_gain = (f*1000):%d", (uint32_t)(gain*1000));
    tx_post_gain_set(gain);
}

static void tx_algo_hdr_config_cmd(int argc, char *argv[])
{
    int mode = 0;

    if (argc != 2) {
        TRACE(0, "error arg.");
        return ;
    }

    mode = atoi(argv[1]);
    TRACE(1, "tx_hdr_mode = %d", mode);
    tx_hdr_mode_set(mode);
}

static void tx_algo_mpdrc_config_cmd(int argc, char *argv[])
{
    int mode = 0;

    if (argc != 2) {
        TRACE(0, "error arg.");
        return ;
    }

    mode = atoi(argv[1]);
    TRACE(1, "tx_ga_mode = %d", mode);
    tx_ga_mode_set(mode);
}

static void tx_algo_iir_config_cmd(int argc, char *argv[])
{
    int mode = 0;

    if (argc != 2) {
        TRACE(0, "error arg.");
        return ;
    }

    mode = atoi(argv[1]);
    TRACE(1, "tx_eq_mode = %d", mode);
    tx_eq_mode_set(mode);
}

static void get_current_tx_algo_param(int argc, char *argv[])
{
    int hdr_mod_val, ga_mode_val, eq_mode_val;
    float pre_gain_val, post_gain_val;

    hdr_mod_val = tx_hdr_mode_get();
    ga_mode_val = tx_ga_mode_get();
    eq_mode_val = tx_eq_mode_get();
    pre_gain_val = tx_pre_gain_get();
    post_gain_val = tx_post_gain_get();

    TRACE(6, "%s: hdr = %d, ga = %d, eq = %d, pre = (f*1000):%d, post = (f*1000):%d", \
          __func__, hdr_mod_val, ga_mode_val, eq_mode_val, (uint32_t)(pre_gain_val*1000), (uint32_t)(post_gain_val*1000));
}

ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "tx_algo_on", "tx_algo_on 0/1", tx_algo_onoff_cmd);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "tx_algo_sw_pre", "mic_tx_algo_sw_pre 0/1", tx_algo_sw_pre_onoff_cmd);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "tx_algo_sw_post", "mic_tx_algo_sw_post 0/1", tx_algo_sw_post_onoff_cmd);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "tx_algo_iir", "tx_algo_iir 0/1", tx_algo_iir_onoff_cmd);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "tx_algo_mpdrc", "tx_algo_mpdrc 0/1", tx_algo_mpdrc_onoff_cmd);

ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "tx_algo_sw_pre_cfg", "tx_algo_sw_pre_cfg -50-50", tx_algo_sw_pre_config_cmd);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "tx_algo_sw_post_cfg", "tx_algo_sw_post_cfg -50-50", tx_algo_sw_post_config_cmd);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "tx_algo_hdr_cfg", "tx_algo_hdr_cfg 0-2", tx_algo_hdr_config_cmd);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "tx_algo_iir_cfg", "tx_algo_iir_cfg 0-2", tx_algo_iir_config_cmd);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "tx_algo_mpdrc_cfg", "tx_algo_mpdrc_cfg 0-3", tx_algo_mpdrc_config_cmd);

ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "tx_algo_param_get", "tx_algo_param_get", get_current_tx_algo_param);
#else
void tx_algo_onoff_set(bool onoff)
{
    tx_algo_on = onoff;
    TRACE(1, "tx algo onoff:%d", onoff);
}

void tx_algo_mpdrc_onoff_set(bool onoff)
{
    multipoint_drc_switch(tx_mpdrc_st, onoff);
    TRACE(1, "tx_algo_mpdrc_onoff = %d", onoff);
}
#endif