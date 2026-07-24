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
#if defined (AUDIO_EQ_TUNING) || defined(APP_DEBUG_TOOL_TOTA)
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "app_anc.h"

#define MAX_OUTPUT_LEN 512
#define MAX_SAMPLE_RATE_NUM 4
static uint32_t sw_iir_eq_freq[MAX_SAMPLE_RATE_NUM] = {0};
static uint32_t dac_iir_eq_freq[MAX_SAMPLE_RATE_NUM] = {0};
extern int getMaxEqBand(void);
extern int getSampleArray(uint8_t* buf, uint16_t *num);
extern int getSampleRateArray(uint32_t* buf);

char *get_version(char *buf, size_t buf_size) {
    snprintf(buf, buf_size, "1.0");
    return buf;
}

char *get_flow(char *buf, size_t buf_size) {
    // enable this in your project!
    snprintf(buf, buf_size, "spatial_audio,virtual_bass,dynamic_eq,sw_eq,sw_drc,sw_limiter,hw_dac_eq,hw_eq_limiter,hw_drc");
    // just light all audio feature!
    // snprintf(buf, buf_size, "res : 1");
    return buf;
}

char *get_audio_sample_rate(char *buf, size_t buf_size) {
    snprintf(buf, buf_size, "%u", audio_process.sample_rate);
    return buf;
}

char *get_audio_status(char *buf, size_t buf_size) {
    snprintf(buf, buf_size, "%u", audio_process_is_opened());
    return buf;
}

char *get_sw_iir_eq_bands_num(char *buf, size_t buf_size) {
    snprintf(buf, buf_size, "%d", getMaxEqBand());
    return buf;
}

char *get_sample_rate_str(char *buf, size_t buf_size, uint32_t *sample_rate)
{
    buf[0] = '\0';

    for (int i = 0; i < MAX_SAMPLE_RATE_NUM; i++) {
        if (sample_rate[i] == 0) break;

        char temp[32];
        snprintf(temp, sizeof(temp), "%d", sample_rate[i]);

        if (strlen(buf) + strlen(temp) + 2 < buf_size) {
            if (buf[0] != '\0') {
                strncat(buf, ",", buf_size - strlen(buf) - 1);
            }
            strncat(buf, temp, buf_size - strlen(buf) - 1);
        } else {
            break;
        }
    }

    return buf;
}

char *get_sw_iir_eq_sample_rate(char *buf, size_t buf_size) {
    getSampleRateArray(sw_iir_eq_freq);
    get_sample_rate_str(buf, buf_size, sw_iir_eq_freq);
    return buf;
}

char *get_dac_iir_eq_bands_num(char *buf, size_t buf_size) {
    snprintf(buf, buf_size, "%d", getMaxEqBand());
    return buf;
}

char *get_dac_iir_eq_sample_rate(char *buf, size_t buf_size) {
    getSampleRateArray(dac_iir_eq_freq);
    get_sample_rate_str(buf, buf_size, dac_iir_eq_freq);
    return buf;
}

char *get_hw_drc_channel(char *buf, size_t buf_size) {
#ifdef HW_DAC_DRC_UPDATE_CFG
    snprintf(buf, buf_size, "%d", HW_DAC_DRC_CHANNEL);
#endif
    return buf;
}

char *get_hw_drc_aud_version(char *buf, size_t buf_size) {
    snprintf(buf, buf_size, "%d", AUD_SECTION_STRUCT_VERSION);
    return buf;
}

char *get_anc_mode_num(char *buf, size_t buf_size) {
    snprintf(buf, buf_size, "%d", APP_ANC_MODE_QTY);
    return buf;
}

char *get_anc_cur_mode(char *buf, size_t buf_size) {
#ifdef ANC_APP
    snprintf(buf, buf_size, "%d", app_anc_get_curr_mode());
#endif
    return buf;
}

char *get_real_dac_sample_rate(char *buf, size_t buf_size) {
    const uint32_t cand[] = { 46875u, 93750u, 187500u };
    const size_t   N = sizeof(cand)/sizeof(cand[0]);

    uint32_t target = (uint32_t)audio_process.sample_rate;
    uint32_t best = cand[0];
    uint32_t best_err = (target > cand[0]) ? (target - cand[0]) : (cand[0] - target);

    for (size_t i = 1; i < N; ++i) {
        uint32_t err = (target > cand[i]) ? (target - cand[i]) : (cand[i] - target);
        if (err < best_err) {
            best_err = err;
            best = cand[i];
        }
    }

    snprintf(buf, buf_size, "%u", best);
    AUDIO_PROCESS_TRACE(0, "[%s].. sample_rate=%d, best=%d", __func__, audio_process.sample_rate, best);
    return buf;
}

typedef struct {
    const char *key;
    char* (*getter)(char *buf, size_t buf_size);
} InfoEntry;

static InfoEntry info_table[] = {
    { "version", get_version },
    { "flow", get_flow },
    { "audio_cfg.status", get_audio_status },
    { "anc_cfg.mode_num", get_anc_mode_num },
    { "anc_cfg.cur_mode", get_anc_cur_mode },
    { "audio_cfg.sample_rate", get_audio_sample_rate },
    { "sw_iir_eq.bands_num", get_sw_iir_eq_bands_num },
    { "sw_iir_eq.sample_rate", get_sw_iir_eq_sample_rate },
    { "dac_iir_eq.bands_num", get_dac_iir_eq_bands_num },
    { "dac_iir_eq.sample_rate", get_dac_iir_eq_sample_rate },
    { "hw_drc.channel", get_hw_drc_channel },
    { "hw_drc.sample_rate", get_real_dac_sample_rate },
    { "hw_drc.aud_version", get_hw_drc_aud_version },
};

extern void app_debug_tool_printf(const char *fmt, ...);
extern void hal_cmd_set_res_playload(uint8_t* data, int len);

#ifdef AUDIO_EQ_TUNING
#define SEND_TUNING_RESP(PTR, LEN) \
    hal_cmd_set_res_playload((uint8_t*)(PTR), (uint32_t)(LEN))
#elif defined(APP_DEBUG_TOOL_TOTA)
#define SEND_TUNING_RESP(PTR, LEN) \
    app_debug_tool_printf("[CMD] %s;", (const char*)(PTR))
#else
#  error "Please define either AUDIO_EQ_TUNING or APP_DEBUG_TOOL_TOTA"
#endif

static const int g_tuning_info_table_size = sizeof(info_table) / sizeof(info_table[0]);
static char g_audio_tuning_info_output[MAX_OUTPUT_LEN] = {0};

int32_t audio_get_info_callback(uint8_t *buf, uint32_t len)
{
    char key[64];
    uint32_t n = (len < sizeof(key)-1) ? len : (sizeof(key)-1);
    memcpy(key, buf, n);
    key[n] = '\0';

    g_audio_tuning_info_output[0] = '\0';

    char temp[128];
    AUDIO_PROCESS_TRACE(0, "[%s].. buf=%s", __func__, key);
    int found = 0;

    if (strcmp(key, "all_cfg") == 0) {
        for (int i = 0; i < g_tuning_info_table_size; ++i) {
            info_table[i].getter(temp, sizeof(temp));
            strncat(g_audio_tuning_info_output, info_table[i].key, sizeof(g_audio_tuning_info_output) - strlen(g_audio_tuning_info_output) - 1);
            strncat(g_audio_tuning_info_output, ":", sizeof(g_audio_tuning_info_output) - strlen(g_audio_tuning_info_output) - 1);
            strncat(g_audio_tuning_info_output, temp, sizeof(g_audio_tuning_info_output) - strlen(g_audio_tuning_info_output) - 1);
            strncat(g_audio_tuning_info_output, ";", sizeof(g_audio_tuning_info_output) - strlen(g_audio_tuning_info_output) - 1);
        }
        found = 1;
    }

    if (!found) {
        for (int i = 0; i < g_tuning_info_table_size; ++i) {
            if (strcmp(key, info_table[i].key) == 0) {
#ifdef APP_DEBUG_TOOL_TOTA
                info_table[i].getter(temp, sizeof(temp));
                strncat(g_audio_tuning_info_output, info_table[i].key, sizeof(g_audio_tuning_info_output) - strlen(g_audio_tuning_info_output) - 1);
                strncat(g_audio_tuning_info_output, ":", sizeof(g_audio_tuning_info_output) - strlen(g_audio_tuning_info_output) - 1);
                strncat(g_audio_tuning_info_output, temp, sizeof(g_audio_tuning_info_output) - strlen(g_audio_tuning_info_output) - 1);
#else
                info_table[i].getter(g_audio_tuning_info_output, sizeof(g_audio_tuning_info_output));
#endif
                found = 1;
                break;
            }
        }
    }

    if (!found) {
        size_t prefix_len = strlen(key);
        for (int i = 0; i < g_tuning_info_table_size; ++i) {
            if (strncmp(info_table[i].key, key, prefix_len) == 0 &&
                info_table[i].key[prefix_len] == '.') {
                info_table[i].getter(temp, sizeof(temp));

                const char *short_key = info_table[i].key + prefix_len + 1;

                strncat(g_audio_tuning_info_output, short_key, sizeof(g_audio_tuning_info_output) - strlen(g_audio_tuning_info_output) - 1);
                strncat(g_audio_tuning_info_output, ":", sizeof(g_audio_tuning_info_output) - strlen(g_audio_tuning_info_output) - 1);
                strncat(g_audio_tuning_info_output, temp, sizeof(g_audio_tuning_info_output) - strlen(g_audio_tuning_info_output) - 1);
                strncat(g_audio_tuning_info_output, ";", sizeof(g_audio_tuning_info_output) - strlen(g_audio_tuning_info_output) - 1);
                // strncat(g_audio_tuning_info_output, '\n', sizeof(g_audio_tuning_info_output) - strlen(g_audio_tuning_info_output) - 1);
                found = 1;
            }
        }
    }

    if (!found) {
        snprintf(g_audio_tuning_info_output, sizeof(g_audio_tuning_info_output), "res:1 UNKNOWN");
    }
    AUDIO_PROCESS_TRACE(0, "[%s].. res_buf=%s len=%d", __func__, g_audio_tuning_info_output, strlen(g_audio_tuning_info_output));
    SEND_TUNING_RESP((uint8_t *)g_audio_tuning_info_output, strlen(g_audio_tuning_info_output));

    return 0;
}
#endif

#if defined(AUDIO_EQ_SW_IIR_UPDATE_CFG) && !defined(USB_EQ_TUNING)
#ifndef USB_AUDIO_APP
int audio_eq_sw_iir_callback(uint8_t *buf, uint32_t  len)
{
    AUDIO_PROCESS_TRACE(3,"[%s] len = %d, sizeof(struct) = %d", __func__, len, sizeof(IIR_CFG_T));

    if (len != sizeof(IIR_CFG_T))
    {
        return 1;
    }

    memcpy(&audio_process.sw_iir_cfg, buf, sizeof(IIR_CFG_T));
    AUDIO_PROCESS_TRACE(3,"band num:%d gain0:%d, gain1:%d",
                                                (int32_t)audio_process.sw_iir_cfg.num,
                                                (int32_t)(audio_process.sw_iir_cfg.gain0*10),
                                                (int32_t)(audio_process.sw_iir_cfg.gain1*10));
    for (uint8_t i = 0; i<audio_process.sw_iir_cfg.num; i++){
        AUDIO_PROCESS_TRACE(5,"band num:%d type %d gain:%d fc:%d q:%d", i,
			                           (int32_t)(audio_process.sw_iir_cfg.param[i].type),
                                                (int32_t)(audio_process.sw_iir_cfg.param[i].gain*10),
                                                (int32_t)(audio_process.sw_iir_cfg.param[i].fc*10),
                                                (int32_t)(audio_process.sw_iir_cfg.param[i].Q*10));
    }

#ifdef __SW_IIR_EQ_PROCESS__
    {
        iir_set_cfg(&audio_process.sw_iir_cfg);
    }
#endif

#ifdef __AUDIO_DYNAMIC_BOOST__
#ifdef DYNAMIC_BOOST_USE_SW_EQ
    audio_dynamic_boost_set_new_customer_iir_eq(&audio_process.sw_iir_cfg, AUDIO_EQ_TYPE_SW_IIR);
#endif
#endif

    return 0;
}
#else
int audio_eq_sw_iir_callback(uint8_t *buf, uint32_t  len)
{
    // IIR_CFG_T *rx_iir_cfg = NULL;

    // rx_iir_cfg = (IIR_CFG_T *)buf;

    // AUDIO_PROCESS_TRACE(3,"[%s] left gain = %f, right gain = %f", __func__, rx_iir_cfg->gain0, rx_iir_cfg->gain1);

    // for(int i=0; i<rx_iir_cfg->num; i++)
    // {
    //     AUDIO_PROCESS_TRACE(5,"[%s] iir[%d] gain = %f, f = %f, Q = %f", __func__, i, rx_iir_cfg->param[i].gain, rx_iir_cfg->param[i].fc, rx_iir_cfg->param[i].Q);
    // }

    // audio_eq_set_cfg(NULL,(const IIR_CFG_T *)rx_iir_cfg,AUDIO_EQ_TYPE_SW_IIR);

    iir_update_cfg_tbl(buf, len);

    return 0;
}
#endif
#endif

#ifdef AUDIO_EQ_HW_FIR_UPDATE_CFG
int audio_eq_hw_fir_callback(uint8_t *buf, uint32_t  len)
{
    AUDIO_PROCESS_TRACE(3,"[%s] len = %d, sizeof(struct) = %d", __func__, len, sizeof(FIR_CFG_T));

    if (len != sizeof(FIR_CFG_T))
    {
        return 1;
    }

    FIR_CFG_T *rx_fir_cfg = NULL;

    rx_fir_cfg = (FIR_CFG_T *)buf;

    AUDIO_PROCESS_TRACE(3,"[%s] left gain = %d, right gain = %d", __func__, rx_fir_cfg->gain0, rx_fir_cfg->gain1);

    AUDIO_PROCESS_TRACE(6,"[%s] len = %d, coef: %d, %d......%d, %d", __func__, rx_fir_cfg->len, rx_fir_cfg->coef[0], rx_fir_cfg->coef[1], rx_fir_cfg->coef[rx_fir_cfg->len-2], rx_fir_cfg->coef[rx_fir_cfg->len-1]);

    rx_fir_cfg->gain0 = 6;
    rx_fir_cfg->gain1 = 6;

    if(rx_fir_cfg)
    {
        memcpy(&audio_process.fir_cfg, rx_fir_cfg, sizeof(audio_process.fir_cfg));
        audio_process.fir_enable = true;
        fir_set_cfg(&audio_process.fir_cfg);
    }
    else
    {
        audio_process.fir_enable = false;
    }

    return 0;
}
#endif

#ifdef AUDIO_DYNAMIC_BOOST_UPDATE_CFG
int audio_dynamic_boost_callback(uint8_t *buf, uint32_t  len)
{
    AUDIO_PROCESS_TRACE(3,"[%s] len = %d, sizeof(struct) = %d", __func__, len, sizeof(DynamicBoostConfig));

    if (len != sizeof(DynamicBoostConfig))
    {
        return 1;
    }

    memcpy(&audio_process.dynamic_boost_cfg, buf, sizeof(DynamicBoostConfig));
    audio_process.dynamic_boost_update = true;

    AUDIO_PROCESS_TRACE(10,"debug: %d ,freq: %d, order: %d, CT: %d ,CS*10: %d, WT: %d, WS*10: %d, ET: %d, ES*10: %d,makeup_gain: %d",
                (int32_t)(audio_process.dynamic_boost_cfg.debug),
                (int32_t)(audio_process.dynamic_boost_cfg.xover_freq[0] ),
                (int32_t)(audio_process.dynamic_boost_cfg.order),
                (int32_t)(audio_process.dynamic_boost_cfg.CT),
                (int32_t)(audio_process.dynamic_boost_cfg.CS*10),
                (int32_t)(audio_process.dynamic_boost_cfg.WT),
                (int32_t)(audio_process.dynamic_boost_cfg.WS*10),
                (int32_t)(audio_process.dynamic_boost_cfg.ET),
                (int32_t)(audio_process.dynamic_boost_cfg.ES*10),
                (int32_t)(audio_process.dynamic_boost_cfg.makeup_gain));

    for (uint8_t i = 0; i<audio_process.dynamic_boost_cfg.eq_num; i++){
        AUDIO_PROCESS_TRACE(3,"freq: %d , Q*100: %d , gain: %d",
                (int32_t)(audio_process.dynamic_boost_cfg.boost_eq[i].freq),
                (int32_t)(audio_process.dynamic_boost_cfg.boost_eq[i].Q*100),
                (int32_t)(audio_process.dynamic_boost_cfg.boost_eq[i].gain));
    }
    AUDIO_PROCESS_TRACE(1,"[%s] !!! dynamic_boost_update: %d", __func__,audio_process.dynamic_boost_update);
    return 0;
}
#endif

#ifdef HW_DAC_DRC_UPDATE_CFG
int audio_hw_drc_callback(uint8_t *buf, uint32_t  len)
{
    uint32_t hw_drc_cfg_len = sizeof(struct_psap_cfg) / 2 * HW_DAC_DRC_CHANNEL;

	AUDIO_PROCESS_TRACE(3,"[%s] len = %d, sizeof(struct) = %d", __func__, len, hw_drc_cfg_len);

    if (len != hw_drc_cfg_len)
    {
        AUDIO_PROCESS_TRACE(1,"[%s] WARNING: Length is different", __func__);

        return 1;
    }

	memcpy(&audio_process.hw_dac_drc_cfg, buf, hw_drc_cfg_len);
    audio_process.hw_dac_drc_update = true;

    return 0;
}
#endif

#ifdef HW_DAC_IIR_LIMITER_UPDATE_CFG
int audio_hw_dac_iir_limiter_callback(uint8_t *buf, uint32_t  len)
{
	AUDIO_PROCESS_TRACE(3,"[%s] len = %d, sizeof(struct) = %d", __func__, len, sizeof(hw_dac_limiter_cfg_t));

    if (len != sizeof(hw_dac_limiter_cfg_t))
    {
        AUDIO_PROCESS_TRACE(1,"[%s] WARNING: Length is different", __func__);

        return 1;
    }

    memcpy(&audio_process.hw_dac_limiter_cfg, buf, sizeof(hw_dac_limiter_cfg_t));

    AUDIO_PROCESS_TRACE(0,"L ch, enable = %d, delay=%d, thd=%d , att_ms=%d, rls_ms=%d", audio_process.hw_dac_limiter_cfg.limiter_param_ch0.enable,
                                                                                audio_process.hw_dac_limiter_cfg.limiter_param_ch0.delay_ms,
                                                                                (int32_t)(audio_process.hw_dac_limiter_cfg.limiter_param_ch0.thd),
                                                                                (int32_t)(audio_process.hw_dac_limiter_cfg.limiter_param_ch0.att_ms),
                                                                                (int32_t)(audio_process.hw_dac_limiter_cfg.limiter_param_ch0.rls_ms));
    AUDIO_PROCESS_TRACE(0,"R ch, enable = %d, delay=%d, thd=%d , att_ms=%d, rls_ms=%d", audio_process.hw_dac_limiter_cfg.limiter_param_ch1.enable,
                                                                                audio_process.hw_dac_limiter_cfg.limiter_param_ch1.delay_ms,
                                                                                (int32_t)(audio_process.hw_dac_limiter_cfg.limiter_param_ch1.thd),
                                                                                (int32_t)(audio_process.hw_dac_limiter_cfg.limiter_param_ch1.att_ms),
                                                                                (int32_t)(audio_process.hw_dac_limiter_cfg.limiter_param_ch1.rls_ms));
    hw_dac_limiter_set_cfg(&audio_process.hw_dac_limiter_cfg);
    hw_dac_limiter_disable();
    hw_dac_limiter_enable();

    return 0;
}
#endif

#ifdef AUDIO_REVERB_UPDATE_CFG
int audio_reverb_callback(uint8_t *buf, uint32_t len)
{
    AUDIO_PROCESS_TRACE(3, "[%s] len = %d, sizeof(struct) = %d", __FUNCTION__, len, sizeof(ReverbConfig));

    if (len != sizeof(ReverbConfig))
    {
        AUDIO_PROCESS_TRACE(1, "[%s] WARNING: length is different", __FUNCTION__);
        AUDIO_PROCESS_DUMP8("0x%x, ", buf, len);
        return 1;
    }

    if (audio_process.reverb_st == NULL)
    {
        AUDIO_PROCESS_TRACE(1, "[%s] WARNING: audio_process.reverb_st = NULL", __FUNCTION__);
        AUDIO_PROCESS_TRACE(1, "[%s] WARNING: Please Play music, then tuning Reverb", __FUNCTION__);

        return 2;
    }

	memcpy(&audio_process.reverb_cfg, buf, sizeof(ReverbConfig));
    audio_process.reverb_update = true;

    return 0;
}
#endif

#if defined(__IIR_EQ_PROCESS_LR_2CH__) && defined(AUDIO_EQ_HW_DAC_IIR_UPDATE_CFG)
int audio_eq_iir_channel_select_callback(uint8_t *buf, uint32_t len)
{
    AUDIO_PROCESS_TRACE(3,"[%s] len = %d, sizeof(struct) = %d", __func__, len, sizeof(int));
    if (len != sizeof(int))
    {
        return 1;
    }

    iir_channel_status = *(int *)buf;
    AUDIO_PROCESS_TRACE(3,"[%s] iir_channel_status = %d !", __func__, iir_channel_status);

    return 0;
}
#endif

#if defined(AUDIO_EQ_HW_DAC_IIR_UPDATE_CFG) && !defined(USB_EQ_TUNING)
#if defined(__IIR_EQ_PROCESS_LR_2CH__)
HW_CODEC_IIR_CFG_T hw_iir_cfg_dac_lr;
#endif
int audio_eq_hw_dac_iir_callback(uint8_t *buf, uint32_t  len)
{
    AUDIO_PROCESS_TRACE(3,"[%s] len = %d, sizeof(struct) = %d", __func__, len, sizeof(IIR_CFG_T));

    if (len != sizeof(IIR_CFG_T))
    {
        return 1;
    }

#if defined(__IIR_EQ_PROCESS_LR_2CH__)
    AUDIO_PROCESS_TRACE(3,"[%s] iir_channel_status = %d !", __func__, iir_channel_status);    // L -> 1 ; R -> 2 ; stereo -> 3
    if (iir_channel_status == IIR_CHANNEL_SELECT_LCHNL)
#endif
    {
#if defined(TEST_AUDIO_CUSTOM_EQ)
        audio_eq_merge_custom_eq(&audio_process.hw_dac_iir_cfg, (IIR_CFG_T *)buf, audio_eq_hw_dac_iir_cfg_list[0]);
#else
        memcpy(&audio_process.hw_dac_iir_cfg, buf, sizeof(IIR_CFG_T));
#endif

        AUDIO_PROCESS_TRACE(0, "LCHNL num: %d, gain0: %d, gain1: %d",
                                                    (int32_t)audio_process.hw_dac_iir_cfg.num,
                                                    (int32_t)(audio_process.hw_dac_iir_cfg.gain0*10),
                                                    (int32_t)(audio_process.hw_dac_iir_cfg.gain1*10));
        for (uint8_t i = 0; i<audio_process.hw_dac_iir_cfg.num; i++){
            AUDIO_PROCESS_TRACE(0, "[%d] type: %d, gain: %d, fc: %d, q: %d", i,
                                                    (int32_t)(audio_process.hw_dac_iir_cfg.param[i].type),
                                                    (int32_t)(audio_process.hw_dac_iir_cfg.param[i].gain*10),
                                                    (int32_t)(audio_process.hw_dac_iir_cfg.param[i].fc*10),
                                                    (int32_t)(audio_process.hw_dac_iir_cfg.param[i].Q*10));
        }
#if defined(__IIR_EQ_PROCESS_LR_2CH__)
    } else if (iir_channel_status == IIR_CHANNEL_SELECT_RCHNL) {
#if defined(TEST_AUDIO_CUSTOM_EQ)
        audio_eq_merge_custom_eq(&audio_process.hw_dac_iir_cfg2, (IIR_CFG_T *)buf, audio_eq_hw_dac_iir_cfg_list2[0]);
#else
        memcpy(&audio_process.hw_dac_iir_cfg2, buf, sizeof(IIR_CFG_T));
#endif
        AUDIO_PROCESS_TRACE(0, "RCHNL num: %d, gain0: %d, gain1: %d",
                                                    (int32_t)audio_process.hw_dac_iir_cfg2.num,
                                                    (int32_t)(audio_process.hw_dac_iir_cfg2.gain0*10),
                                                    (int32_t)(audio_process.hw_dac_iir_cfg2.gain1*10));
        for (uint8_t i = 0; i<audio_process.hw_dac_iir_cfg2.num; i++){
            AUDIO_PROCESS_TRACE(0, "[%d] type: %d, gain: %d, fc: %d, q: %d", i,
                                                    (int32_t)(audio_process.hw_dac_iir_cfg2.param[i].type),
                                                    (int32_t)(audio_process.hw_dac_iir_cfg2.param[i].gain*10),
                                                    (int32_t)(audio_process.hw_dac_iir_cfg2.param[i].fc*10),
                                                    (int32_t)(audio_process.hw_dac_iir_cfg2.param[i].Q*10));
        }
    } else if (iir_channel_status == IIR_CHANNEL_SELECT_STEREO) {
#if defined(TEST_AUDIO_CUSTOM_EQ)
        audio_eq_merge_custom_eq(&audio_process.hw_dac_iir_cfg, (IIR_CFG_T *)buf, audio_eq_hw_dac_iir_cfg_list[0]);
        audio_eq_merge_custom_eq(&audio_process.hw_dac_iir_cfg2, (IIR_CFG_T *)buf, audio_eq_hw_dac_iir_cfg_list2[0]);
#else
        memcpy(&audio_process.hw_dac_iir_cfg, buf, sizeof(IIR_CFG_T));
        memcpy(&audio_process.hw_dac_iir_cfg2, buf, sizeof(IIR_CFG_T));
#endif

        AUDIO_PROCESS_TRACE(0, "LCHNL num: %d, gain0: %d, gain1: %d",
                                                    (int32_t)audio_process.hw_dac_iir_cfg.num,
                                                    (int32_t)(audio_process.hw_dac_iir_cfg.gain0*10),
                                                    (int32_t)(audio_process.hw_dac_iir_cfg.gain1*10));
        for (uint8_t i = 0; i<audio_process.hw_dac_iir_cfg.num; i++){
            AUDIO_PROCESS_TRACE(0, "[%d] type: %d, gain: %d, fc: %d, q: %d", i,
                                                    (int32_t)(audio_process.hw_dac_iir_cfg.param[i].type),
                                                    (int32_t)(audio_process.hw_dac_iir_cfg.param[i].gain*10),
                                                    (int32_t)(audio_process.hw_dac_iir_cfg.param[i].fc*10),
                                                    (int32_t)(audio_process.hw_dac_iir_cfg.param[i].Q*10));
        }

        AUDIO_PROCESS_TRACE(0, "RCHNL num: %d, gain0: %d, gain1: %d",
                                                    (int32_t)audio_process.hw_dac_iir_cfg2.num,
                                                    (int32_t)(audio_process.hw_dac_iir_cfg2.gain0*10),
                                                    (int32_t)(audio_process.hw_dac_iir_cfg2.gain1*10));
        for (uint8_t i = 0; i<audio_process.hw_dac_iir_cfg2.num; i++){
            AUDIO_PROCESS_TRACE(0, "[%d] type: %d, gain: %d, fc: %d, q: %d", i,
                                                    (int32_t)(audio_process.hw_dac_iir_cfg2.param[i].type),
                                                    (int32_t)(audio_process.hw_dac_iir_cfg2.param[i].gain*10),
                                                    (int32_t)(audio_process.hw_dac_iir_cfg2.param[i].fc*10),
                                                    (int32_t)(audio_process.hw_dac_iir_cfg2.param[i].Q*10));
        }
    } else {
        AUDIO_PROCESS_TRACE(3,"[ERR] Invalid iir_channel_status = %d !", iir_channel_status);
#endif
    }

#ifdef __HW_DAC_IIR_EQ_PROCESS__
    {
        HW_CODEC_IIR_CFG_T *hw_iir_cfg_dac = NULL;
        enum AUD_SAMPRATE_T sample_rate_hw_dac_iir;
#ifdef __AUDIO_RESAMPLE__
        sample_rate_hw_dac_iir=hal_codec_get_real_sample_rate(audio_process.sample_rate,1);
        AUDIO_PROCESS_TRACE(3,"audio_process.sample_rate:%d, sample_rate_hw_dac_iir: %d.", audio_process.sample_rate, sample_rate_hw_dac_iir);
#else
        sample_rate_hw_dac_iir = audio_process.sample_rate;
#endif
#if defined(__IIR_EQ_PROCESS_LR_2CH__)
        hw_iir_cfg_dac = hw_codec_iir_get_cfg(sample_rate_hw_dac_iir,&audio_process.hw_dac_iir_cfg);
        memcpy(&(hw_iir_cfg_dac_lr.iir_filtes_l),&(hw_iir_cfg_dac->iir_filtes_l),sizeof(HW_CODEC_IIR_FILTERS_T));

        hw_iir_cfg_dac = hw_codec_iir_get_cfg(sample_rate_hw_dac_iir,&audio_process.hw_dac_iir_cfg2);
        memcpy(&(hw_iir_cfg_dac_lr.iir_filtes_r),&(hw_iir_cfg_dac->iir_filtes_l),sizeof(HW_CODEC_IIR_FILTERS_T));

        hw_codec_iir_set_cfg(&hw_iir_cfg_dac_lr, sample_rate_hw_dac_iir, HW_CODEC_IIR_DAC);
#else
        hw_iir_cfg_dac = hw_codec_iir_get_cfg(sample_rate_hw_dac_iir,&audio_process.hw_dac_iir_cfg);
        ASSERT(hw_iir_cfg_dac != NULL, "[%s] %d codec IIR parameter error!", __func__, (uint32_t)hw_iir_cfg_dac);

        // hal_codec_iir_dump(hw_iir_cfg_dac);

        hw_codec_iir_set_cfg(hw_iir_cfg_dac, sample_rate_hw_dac_iir, HW_CODEC_IIR_DAC);
#endif

    }
#endif

#ifdef __AUDIO_DYNAMIC_BOOST__
#ifdef DYNAMIC_BOOST_USE_HW_EQ
        audio_dynamic_boost_set_new_customer_iir_eq(&audio_process.hw_dac_iir_cfg, AUDIO_EQ_TYPE_HW_DAC_IIR);
#endif
#endif

    return 0;
}
#endif

#ifdef AUDIO_EQ_HW_IIR_UPDATE_CFG
int audio_eq_hw_iir_callback(uint8_t *buf, uint32_t  len)
{
    AUDIO_PROCESS_TRACE(3,"[%s] len = %d, sizeof(struct) = %d", __func__, len, sizeof(IIR_CFG_T));

    if (len != sizeof(IIR_CFG_T))
    {
        return 1;
    }

    memcpy(&audio_process.hw_iir_cfg, buf, sizeof(IIR_CFG_T));
    AUDIO_PROCESS_TRACE(3,"band num:%d gain0:%d, gain1:%d",
                                                (int32_t)audio_process.hw_iir_cfg.num,
                                                (int32_t)(audio_process.hw_iir_cfg.gain0*10),
                                                (int32_t)(audio_process.hw_iir_cfg.gain1*10));

    for (uint8_t i = 0; i<audio_process.hw_iir_cfg.num; i++)
    {
        AUDIO_PROCESS_TRACE(5,"band num:%d type %d gain:%d fc:%d q:%d", i,
			                                    (int32_t)(audio_process.hw_iir_cfg.param[i].type),
                                                (int32_t)(audio_process.hw_iir_cfg.param[i].gain*10),
                                                (int32_t)(audio_process.hw_iir_cfg.param[i].fc*10),
                                                (int32_t)(audio_process.hw_iir_cfg.param[i].Q*10));
    }

#ifdef __HW_IIR_EQ_PROCESS__
    {
        HW_IIR_CFG_T *hw_iir_cfg=NULL;
#ifdef __AUDIO_RESAMPLE__
        enum AUD_SAMPRATE_T sample_rate_hw_iir=AUD_SAMPRATE_50781;
#else
        enum AUD_SAMPRATE_T sample_rate_hw_iir=audio_process.sample_rate;
#endif
        hw_iir_cfg = hw_iir_get_cfg(sample_rate_hw_iir,&audio_process.hw_iir_cfg);
        ASSERT(hw_iir_cfg != NULL, "[%s] %p codec IIR parameter error!", __func__, hw_iir_cfg);
        hw_iir_set_cfg(hw_iir_cfg);
    }
#endif

    return 0;
}
#endif

#ifdef AUDIO_DYNAMIC_EQ_UPDATE_CFG
int audio_dynamic_eq_callback(uint8_t *buf, uint32_t len)
{
    AUDIO_PROCESS_TRACE(3,"[%s] len = %d, sizeof(struct) = %d", __func__, len, sizeof(DynamicEqConfig));

    if (len != sizeof(DynamicEqConfig))
    {
        return 1;
    }

    memcpy(&audio_process.dynamic_eq_cfg, buf, sizeof(DynamicEqConfig));
    audio_process.dynamic_eq_update = true;
    audio_process.dynamic_eq_cfg.debug = audio_dynamic_eq_cfg.debug;

    AUDIO_PROCESS_TRACE(3,"switch_on: %d, debug: %d, eq_num: %d",audio_process.dynamic_eq_cfg.switch_on,
            audio_process.dynamic_eq_cfg.debug, audio_process.dynamic_eq_cfg.eq_num);

    for (uint8_t i = 0; i < audio_process.dynamic_eq_cfg.eq_num; i++) {
        AUDIO_PROCESS_TRACE(3,"gain: %d , fc: %d , Q*100: %d",
                (int32_t)(audio_process.dynamic_eq_cfg.dyeq_param_cfg[i].dyeq_eq_cfg.gain),
                (int32_t)(audio_process.dynamic_eq_cfg.dyeq_param_cfg[i].dyeq_eq_cfg.fc),
                (int32_t)(audio_process.dynamic_eq_cfg.dyeq_param_cfg[i].dyeq_eq_cfg.Q*100));
    }

    int onoff = audio_process.dynamic_eq_cfg.switch_on;
    audio_dynamic_eq_switch(onoff);

    AUDIO_PROCESS_TRACE(1,"[%s] !!! dynamic_eq_update: %d", __func__,audio_process.dynamic_eq_update);
    return 0;
}
#endif

#ifdef AUDIO_LIMITER_UPDATE_CFG
int audio_limiter_callback(uint8_t *buf, uint32_t  len)
{
	AUDIO_PROCESS_TRACE(3,"[%s] len = %d, sizeof(struct) = %d", __func__, len, sizeof(LimiterConfig));

    if (len != sizeof(LimiterConfig))
    {
        AUDIO_PROCESS_TRACE(1,"[%s] WARNING: Length is different", __func__);

        return 1;
    }

    if (audio_process.limiter_st == NULL)
    {
        AUDIO_PROCESS_TRACE(1,"[%s] WARNING: audio_process.limiter_st = NULL", __func__);
        AUDIO_PROCESS_TRACE(1,"[%s] WARNING: Please Play music, then tuning Limiter", __func__);

        return 2;
    }

	memcpy(&audio_process.limiter_cfg, buf, sizeof(LimiterConfig));
    audio_process.limiter_update = true;

    return 0;
}
#endif

#ifdef VIRTUAL_SURROUND_UPDATE_CFG
int audio_virtual_surround_callback(uint8_t *buf, uint32_t len)
{
    AUDIO_PROCESS_TRACE(3, "[%s] len = %d, sizeof(struct) = %d", __FUNCTION__, len, sizeof(VirtualSurroundConfig));

    if (len != sizeof(VirtualSurroundConfig))
    {
        AUDIO_PROCESS_TRACE(1, "[%s] WARNING: length is different", __FUNCTION__);
        AUDIO_PROCESS_DUMP8("0x%x, ", buf, len);
        return 1;
    }

    if (stereo_surround_st == NULL)
    {
        AUDIO_PROCESS_TRACE(1, "[%s] WARNING: stereo_surround_st = NULL", __FUNCTION__);
        AUDIO_PROCESS_TRACE(1, "[%s] WARNING: Please Play music, then tuning virtual surround", __FUNCTION__);

        return 2;
    }

    memcpy(&audio_process.virtual_surround_cfg, buf, sizeof(VirtualSurroundConfig));
    audio_process.virtual_surround_update = true;

    int onoff = audio_process.virtual_surround_cfg.switch_on;
    audio_process_stereo_surround_onoff(onoff);
    return 0;
}
#endif

#ifdef AUDIO_DRC_UPDATE_CFG
int audio_drc_callback(uint8_t *buf, uint32_t  len)
{
	AUDIO_PROCESS_TRACE(3,"[%s] len = %d, sizeof(struct) = %d", __func__, len, sizeof(DrcConfig));

    if (len != sizeof(DrcConfig))
    {
        AUDIO_PROCESS_TRACE(1,"[%s] WARNING: Length is different", __func__);

        return 1;
    }

    if (audio_process.drc_st == NULL)
    {
        AUDIO_PROCESS_TRACE(1,"[%s] WARNING: audio_process.limiter_st = NULL", __func__);
        AUDIO_PROCESS_TRACE(1,"[%s] WARNING: Please Play music, then tuning Limiter", __func__);

        return 2;
    }

	memcpy(&audio_process.drc_cfg, buf, sizeof(DrcConfig));
    audio_process.drc_update = true;

    return 0;
}
#endif

#ifdef AUDIO_BASS_ENHANCER_UPDATE_CFG
int audio_bass_enhancer_callback(uint8_t *buf, uint32_t len)
{
    AUDIO_PROCESS_TRACE(3, "[%s] len = %d, sizeof(struct) = %d", __FUNCTION__, len, sizeof(BassEnhancerConfig));

    if (len != sizeof(BassEnhancerConfig))
    {
        AUDIO_PROCESS_TRACE(1, "[%s] WARNING: length is different", __FUNCTION__);
        AUDIO_PROCESS_DUMP8("0x%x, ", buf, len);
        return 1;
    }

    if (audio_process.bass_st == NULL)
    {
        AUDIO_PROCESS_TRACE(1, "[%s] WARNING: audio_process.bass_st = NULL", __FUNCTION__);
        AUDIO_PROCESS_TRACE(1, "[%s] WARNING: Please Play music, then tuning bass_enhancer", __FUNCTION__);

        return 2;
    }

	memcpy(&audio_process.bass_enhancer_cfg, buf, sizeof(BassEnhancerConfig));
    audio_process.bass_enhancer_update = true;

    int onoff = audio_process.bass_enhancer_cfg.switch_on;
    audio_virtual_bass_switch(onoff);
    return 0;
}
#endif

#include "app_anc_utils.h"
int audio_anc_mc_switch_callback(uint8_t *buf, uint32_t  len)
{
    AUDIO_PROCESS_TRACE(0, "[%s] len = %d, sizeof(uint32_t) = %d", __func__, len, sizeof(uint32_t));

    if (len != sizeof(uint32_t)) {
        return 1;
    }

#if defined(ANC_APP)
    uint32_t on = *((uint32_t *)buf);

    AUDIO_PROCESS_TRACE(0, "[%s] on/off: %d", __func__, on);
    if (on) {
        app_anc_set_global_gain_f32(ANC_MUSICCANCLE, 1, 1);
    } else {
        app_anc_set_global_gain_f32(ANC_MUSICCANCLE, 0, 0);
    }
#else
    AUDIO_PROCESS_TRACE(0, "[%s] WARNING: ANC_APP is disabled", __func__);
#endif

    return 0;
}

int audio_ping_callback(uint8_t *buf, uint32_t len)
{
    //AUDIO_PROCESS_TRACE(0,"");
    return 0;
}

int audio_anc_switch_callback(uint8_t *buf, uint32_t len)
{
    AUDIO_PROCESS_TRACE(0, "[%s] len = %d, sizeof(uint32_t) = %d", __func__, len, sizeof(uint32_t));

    if (len != sizeof(uint32_t)) {
        return 1;
    }

#if defined(ANC_APP)
    uint32_t mode = *((uint32_t *)buf);

    if (mode < APP_ANC_MODE_QTY) {
        AUDIO_PROCESS_TRACE(0, "[%s] mode: %d", __func__, mode);
        app_anc_switch(mode);
    } else {
        AUDIO_PROCESS_TRACE(0, "[%s] WARNING: mode(%d) >= APP_ANC_MODE_QTY", __func__, mode);
    }
#else
    AUDIO_PROCESS_TRACE(0, "[%s] WARNING: ANC_APP is disabled", __func__);
#endif

    return 0;
}

#if defined(AUDIO_EQ_TUNING)
typedef struct {
    uint8_t type;
    // EQ
    uint8_t max_eq_band_num;
    uint16_t sample_rate_num;
    uint8_t sample_rate[20];
    // ANC
    uint32_t anc_mode;

    // Add new items in order for compatibility
} query_eq_info_t;

#define CMD_TYPE_QUERY_DUT_EQ_INFO  0x00
int audio_cmd_callback(uint8_t *buf, uint32_t len)
{
    uint8_t type;
    query_eq_info_t  info;

    memset(&info, 0, sizeof(info));

    type = buf[0];

    AUDIO_PROCESS_TRACE(2,"%s type: %d", __func__, type);
    switch (type) {
        case CMD_TYPE_QUERY_DUT_EQ_INFO:
            info.type = CMD_TYPE_QUERY_DUT_EQ_INFO;

            // EQ
            info.max_eq_band_num = getMaxEqBand();
            getSampleArray(info.sample_rate, &info.sample_rate_num);

            // ANC
#if defined(ANC_APP)
            info.anc_mode = app_anc_get_curr_mode();
#else
            info.anc_mode = 0;
#endif

            hal_cmd_set_res_playload((uint8_t*)&info, sizeof(info));
            break;
        default:
            break;
    }

    return 0;
}

#ifdef AUDIO_SECTION_ENABLE
int audio_cfg_burn_sw_eq_callback(uint8_t *buf, uint32_t  len)
{
    AUDIO_PROCESS_TRACE(3,"[%s] len = %d, sizeof(struct) = %d", __func__, len, sizeof(IIR_CFG_T));

    if (len != sizeof(IIR_CFG_T)) {
        return 1;
    }

    int res = 0;
    res = store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_SW_IIR_EQ, buf);

    if(res) {
        AUDIO_PROCESS_TRACE(2,"[%s] ERROR: res = %d", __func__, res);
        res += 100;
    } else {
        AUDIO_PROCESS_TRACE(1,"[%s] Store sw iir eq cfg into audio section!!!", __func__);
    }

    return res;
}

int audio_cfg_burn_hw_dac_eq_callback(uint8_t *buf, uint32_t  len)
{
    AUDIO_PROCESS_TRACE(3,"[%s] len = %d, sizeof(struct) = %d", __func__, len, sizeof(IIR_CFG_T));

    if (len != sizeof(IIR_CFG_T)) {
        return 1;
    }

    int res = 0;
    res = store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_HW_DAC_IIR_EQ, buf);

    if(res) {
        AUDIO_PROCESS_TRACE(2,"[%s] ERROR: res = %d", __func__, res);
        res += 100;
    } else {
        AUDIO_PROCESS_TRACE(1,"[%s] Store dac iir eq cfg into audio section!!!", __func__);
    }

    return res;
}

int audio_cfg_burn_drc_callback(uint8_t *buf, uint32_t  len)
{
    AUDIO_PROCESS_TRACE(3,"[%s] len = %d, sizeof(struct) = %d", __func__, len, sizeof(DrcConfig));

    if (len != sizeof(DrcConfig)) {
        return 1;
    }

    int res = 0;
    res = store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_DRC, buf);

    if(res) {
        AUDIO_PROCESS_TRACE(2,"[%s] ERROR: res = %d", __func__, res);
        res += 100;
    } else {
        AUDIO_PROCESS_TRACE(1,"[%s] Store drc cfg into audio section!!!", __func__);
    }

    return res;
}

int audio_cfg_burn_hw_drc_callback(uint8_t *buf, uint32_t  len)
{
    int res = 0;
#ifdef __HW_DAC_DRC__
    AUDIO_PROCESS_TRACE(3,"[%s] len = %d, sizeof(struct) = %d", __func__, len, sizeof(struct_psap_cfg));

    if (len != sizeof(struct_psap_cfg)) {
        return 1;
    }

    res = store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_HW_DRC, buf);

    if(res) {
        AUDIO_PROCESS_TRACE(2,"[%s] ERROR: res = %d", __func__, res);
        res += 100;
    } else {
        AUDIO_PROCESS_TRACE(1,"[%s] Store hw drc cfg into audio section!!!", __func__);
    }
#endif
    return res;
}

int audio_cfg_burn_limiter_callback(uint8_t *buf, uint32_t  len)
{
    AUDIO_PROCESS_TRACE(3,"[%s] len = %d, sizeof(struct) = %d", __func__, len, sizeof(LimiterConfig));

    if (len != sizeof(LimiterConfig)) {
        return 1;
    }

    int res = 0;
    res = store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_LIMITER, buf);

    if(res) {
        AUDIO_PROCESS_TRACE(2,"[%s] ERROR: res = %d", __func__, res);
        res += 100;
    } else {
        AUDIO_PROCESS_TRACE(1,"[%s] Store audio cfg into audio section!!!", __func__);
    }

    return res;
}

int audio_cfg_burn_hw_dac_iir_limiter_callback(uint8_t *buf, uint32_t  len)
{
    AUDIO_PROCESS_TRACE(3,"[%s] len = %d, sizeof(struct) = %d", __func__, len, sizeof(hw_dac_limiter_cfg_t));

    if (len != sizeof(hw_dac_limiter_cfg_t)) {
        return 1;
    }

    int res = 0;
    res = store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_HW_DAC_LIMITER, buf);

    if(res) {
        AUDIO_PROCESS_TRACE(2,"[%s] ERROR: res = %d", __func__, res);
        res += 100;
    } else {
        AUDIO_PROCESS_TRACE(1,"[%s] Store audio cfg into audio section!!!", __func__);
    }

    return res;
}

int audio_cfg_burn_dynamic_eq_callback(uint8_t *buf, uint32_t  len)
{
    AUDIO_PROCESS_TRACE(3,"[%s] len = %d, sizeof(struct) = %d", __func__, len, sizeof(DynamicEqConfig));

    if (len != sizeof(DynamicEqConfig)) {
        return 1;
    }

    int res = 0;
    res = store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_DYNAMIC_EQ, buf);

    if(res) {
        AUDIO_PROCESS_TRACE(2,"[%s] ERROR: res = %d", __func__, res);
        res += 100;
    } else {
        AUDIO_PROCESS_TRACE(1,"[%s] Store limiter cfg into audio section!!!", __func__);
    }

    return res;
}
#endif // #if defined(AUDIO_SECTION_ENABLE)
#endif  // #if defined(AUDIO_EQ_TUNING)

#ifdef USB_EQ_TUNING

int audio_eq_usb_iir_callback(uint8_t *buf, uint32_t  len)
{
	IIR_CFG_T* cur_cfg;

	AUDIO_PROCESS_TRACE(2,"usb_iir_cb: len=[%d - %d]", len, sizeof(IIR_CFG_T));

	if (len != sizeof(IIR_CFG_T)) {
        return 1;
    }

	cur_cfg = (IIR_CFG_T*)buf;
	AUDIO_PROCESS_TRACE(2,"-> sample_rate[%d], num[%d]", /*cur_cfg->samplerate,*/ audio_process.sample_rate, cur_cfg->num);

#if defined(AUDIO_EQ_SW_IIR_UPDATE_CFG)
	audio_process.sw_iir_cfg.gain0 = cur_cfg->gain0;
	audio_process.sw_iir_cfg.gain1 = cur_cfg->gain1;
#endif

#if defined(AUDIO_EQ_HW_DAC_IIR_UPDATE_CFG)
#if defined(AUDIO_EQ_SW_IIR_UPDATE_CFG)
	audio_process.hw_dac_iir_cfg.gain0 = 0;
	audio_process.hw_dac_iir_cfg.gain1 = 0;
#else
    audio_process.hw_dac_iir_cfg.gain0 = cur_cfg->gain0;
	audio_process.hw_dac_iir_cfg.gain1 = cur_cfg->gain1;
#endif
#endif

#if defined(AUDIO_EQ_HW_DAC_IIR_UPDATE_CFG)
	if (cur_cfg->num > AUD_DAC_IIR_NUM_EQ) {
		audio_process.hw_dac_iir_cfg.num = AUD_DAC_IIR_NUM_EQ;
	} else {
		audio_process.hw_dac_iir_cfg.num = cur_cfg->num;
	}
    AUDIO_PROCESS_TRACE(1,"-> hw_dac_iir_num[%d]", audio_process.hw_dac_iir_cfg.num);
#endif

#if defined(AUDIO_EQ_SW_IIR_UPDATE_CFG)
	audio_process.sw_iir_cfg.num = cur_cfg->num - audio_process.hw_dac_iir_cfg.num;
    AUDIO_PROCESS_TRACE(1,"-> sw_iir_num[%d]", audio_process.sw_iir_cfg.num);
#endif
	//AUDIO_PROCESS_TRACE(2,"-> iir_num[%d - %d]", audio_process.hw_dac_iir_cfg.num, audio_process.sw_iir_cfg.num);

#if defined(AUDIO_EQ_HW_DAC_IIR_UPDATE_CFG)
	if (audio_process.hw_dac_iir_cfg.num) {
		memcpy((void*)(&audio_process.hw_dac_iir_cfg.param[0]),
					(void*)(&cur_cfg->param[0]),
					audio_process.hw_dac_iir_cfg.num*sizeof(IIR_PARAM_T));
	}
#endif

#if defined(AUDIO_EQ_SW_IIR_UPDATE_CFG)
	if (audio_process.sw_iir_cfg.num) {

		memcpy((void*)(&audio_process.sw_iir_cfg.param[0]),
					(void*)(&cur_cfg->param[audio_process.hw_dac_iir_cfg.num]),
					audio_process.sw_iir_cfg.num * sizeof(IIR_PARAM_T));

	} else {

		// set a default filter
		audio_process.sw_iir_cfg.num = 1;

		audio_process.sw_iir_cfg.param[0].fc = 1000.0;
		audio_process.sw_iir_cfg.param[0].gain = 0.0;
		audio_process.sw_iir_cfg.param[0].type = IIR_TYPE_PEAK;
		audio_process.sw_iir_cfg.param[0].Q = 1.0;

	}
#endif

	if (audio_process.sample_rate) {
		audio_process.update_cfg = true;
	}

	audio_process.eq_updated_cfg = true;

	return 0;
}

void audio_eq_usb_eq_update (void)
{
	if (audio_process.update_cfg) {

#if defined(AUDIO_EQ_HW_DAC_IIR_UPDATE_CFG)
		HW_CODEC_IIR_CFG_T *hw_iir_cfg_dac = NULL;

		if (audio_process.hw_dac_iir_cfg.num) {
                    enum AUD_SAMPRATE_T sample_rate_hw_dac_iir;
#ifdef __AUDIO_RESAMPLE__
                    sample_rate_hw_dac_iir=hal_codec_get_real_sample_rate(audio_process.sample_rate,1);
                    AUDIO_PROCESS_TRACE(3,"audio_process.sample_rate:%d, sample_rate_hw_dac_iir: %d.", audio_process.sample_rate, sample_rate_hw_dac_iir);
#else
                    sample_rate_hw_dac_iir = audio_process.sample_rate;
#endif
			hw_iir_cfg_dac = hw_codec_iir_get_cfg(sample_rate_hw_dac_iir, &audio_process.hw_dac_iir_cfg);

			hw_codec_iir_set_cfg(hw_iir_cfg_dac, sample_rate_hw_dac_iir, HW_CODEC_IIR_DAC);
		} else {
            AUDIO_PROCESS_TRACE(3,"warning hw_dac_iir_cfg.num = 0!!");
		}
#endif

#if defined(AUDIO_EQ_SW_IIR_UPDATE_CFG)
		iir_set_cfg(&audio_process.sw_iir_cfg);
#endif

		audio_process.update_cfg = false;

/*
		AUDIO_PROCESS_TRACE(4,"USB EQ Update: num[%d-%d]",
				audio_process.hw_dac_iir_cfg.num, audio_process.sw_iir_cfg.num);
*/
	}

}

#endif	// USB_EQ_TUNING
