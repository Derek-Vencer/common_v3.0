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
#include <string.h>
#include "hal_trace.h"
#include "cmsis_os.h"
#include "audio_test_tool.h"
#include "audio_process.h"
#include "audio_process_tuning.h"

static bool g_cmd_send_rsp = false;

extern void app_debug_tool_printf(const char *fmt, ...);

bool audio_test_tool_check_rsp_flag(void)
{
    return g_cmd_send_rsp;
}

void audio_test_tool_set_rsp_flag(bool val)
{
    g_cmd_send_rsp = val;
}

static void audio_test_get_audio_info(const char *cmd)
{
    APP_TEST_TRACE(0, "[%s] cmd len: %d", __func__, strlen(cmd));

    uint32_t len = *((uint32_t *)cmd);
    const uint8_t *payload = (const uint8_t *)cmd + sizeof(len);

    audio_get_info_callback((uint8_t*)payload, len);

    g_cmd_send_rsp = true;
}

static void audio_test_get_frame_size(const char *cmd)
{
    APP_TEST_TRACE(0, "[%s] cmd len: %d", __func__, strlen(cmd));

    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] frame_size: 256");
}

#ifdef __HW_DAC_DRC__
#include "psap_process.h"
static struct_psap_cfg hw_dac_drc;
#ifndef HW_DAC_DRC_CHANNEL
#define HW_DAC_DRC_CHANNEL 1
#endif
#endif
static void audio_test_hw_dac_drc(const char *cmd)
{
#ifdef __HW_DAC_DRC__
    uint32_t len = *((uint32_t *)cmd);
    uint32_t hw_drc_real_len = sizeof(struct_psap_cfg) / 2 * HW_DAC_DRC_CHANNEL;

    if (len != hw_drc_real_len) {
        APP_TEST_TRACE(0, "[%s] len is invalid: %d, %d", __func__, len, hw_drc_real_len);
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: hw drc len is invalid: %d, %d;", len, hw_drc_real_len);
        return;
    }

    memcpy(&hw_dac_drc.psap_cfg_l, cmd + sizeof(uint32_t), len);

    APP_TEST_TRACE(0, "[%s] hw_dac_drc: %d, %d, %d", __func__,
        hw_dac_drc.psap_cfg_l.psap_total_gain,
        hw_dac_drc.psap_cfg_l.psap_band_num,
        hw_dac_drc.psap_cfg_l.psap_type);

    if (audio_process_is_opened() == false) {
        APP_TEST_TRACE(0, "[%s] Failed: audio_process is closed", __func__);
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: Music is stopped;");
        return;
    }

    psap_set_cfg_coef(&hw_dac_drc);

    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 0; info: HW DRC applied!");
#else
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: Need to enable HW_DAC_DRC!");
    APP_TEST_TRACE(0, "[%s] Need to enable __HW_DAC_DRC__", __func__);
#endif
}

#include "iir_process.h"
static void audio_test_get_audio_eq_bands(const char *cmd)
{
    APP_TEST_TRACE(0, "[%s] cmd len: %d", __func__, strlen(cmd));

    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] audio_eq_bands: %d", IIR_PARAM_NUM);
}

static void audio_test_get_psap_band_gain_Q(const char *cmd)
{
    APP_TEST_TRACE(0, "[%s] cmd len: %d", __func__, strlen(cmd));

    g_cmd_send_rsp = true;
#if defined(CHIP_BEST1501) || defined(CHIP_BEST1501P) || defined(CHIP_BEST1502)
    app_debug_tool_printf("[CMD] psap_band_gain_Q: %d", false);
#else
    app_debug_tool_printf("[CMD] psap_band_gain_Q: %d", true);
#endif
}

static void audio_test_get_aud_item_ver(const char *cmd)
{
    APP_TEST_TRACE(0, "[%s] cmd len: %d", __func__, strlen(cmd));

    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] aud_item_ver: %d", AUD_SECTION_STRUCT_VERSION);
}

static void audio_test_audio_eq_cfg(const char *cmd)
{
    uint32_t len = *((uint32_t *)cmd);
    static IIR_CFG_T audio_eq_cfg;

    if (len != sizeof(IIR_CFG_T)) {
        APP_TEST_TRACE(0, "[%s] len is invalid: %d, %d", __func__, len, sizeof(IIR_CFG_T));
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: EQ len is invalid: %d, %d;", len, sizeof(IIR_CFG_T));
        return;
    }

    memcpy(&audio_eq_cfg, cmd + sizeof(uint32_t), len);

    APP_TEST_TRACE(0, "[%s] audio_eq_cfg: %d, %d, %d", __func__,
        (int32_t)(audio_eq_cfg.gain0 * 100),
        (int32_t)(audio_eq_cfg.gain1 * 100),
        audio_eq_cfg.num);

#if defined(__HW_DAC_IIR_EQ_PROCESS__)
    audio_eq_set_cfg(NULL, &audio_eq_cfg, AUDIO_EQ_TYPE_HW_DAC_IIR);
#elif defined(__SW_IIR_EQ_PROCESS__)
    audio_eq_set_cfg(NULL, &audio_eq_cfg, AUDIO_EQ_TYPE_SW_IIR);
#else
    APP_TEST_TRACE(0, "[%s] Need to enable EQ MACRO", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: Need to enable EQ MACRO;");
#endif
}

extern int audio_eq_hw_dac_iir_callback(uint8_t *buf, uint32_t  len);
static void audio_test_dac_iir_eq(const char *cmd)
{
    uint32_t len = *((uint32_t *)cmd);
    if (len != sizeof(IIR_CFG_T)) {
        APP_TEST_TRACE(0, "[%s] len invalid: %u (expect %u)", __func__, len, (unsigned)sizeof(IIR_CFG_T));
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: DAC IIR len invalid: %u vs %u;", len, (unsigned)sizeof(IIR_CFG_T));
        return;
    }

    static IIR_CFG_T s_new_cfg;
    memcpy(&s_new_cfg, cmd + sizeof(uint32_t), len);

    APP_TEST_TRACE(0, "[%s] new IIR: g0=%d, g1=%d, num=%d",
                   __func__, (int)(s_new_cfg.gain0 * 100), (int)(s_new_cfg.gain1 * 100), s_new_cfg.num);

    if (!audio_process_is_opened()) {
        APP_TEST_TRACE(0, "[%s] Failed: audio_process is closed", __func__);
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: Music is stopped;");
        return;
    }

#if defined(__HW_DAC_IIR_EQ_PROCESS__)
    audio_eq_hw_dac_iir_callback((uint8_t *)&s_new_cfg, len);
#else
    APP_TEST_TRACE(0, "[%s] Need to enable HW_DAC_IIR_EQ_PROCESS", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: Need to enable HW_DAC_IIR_EQ_PROCESS;");
#endif
}

extern int audio_eq_iir_channel_select_callback(uint8_t *buf, uint32_t len);
void audio_test_eq_iir_channel_select(const char *cmd)
{
#if defined(__IIR_EQ_PROCESS_LR_2CH__)
    uint32_t len = *((uint32_t *)cmd);
    if (len != sizeof(int)) {
        APP_TEST_TRACE(0, "[%s] len invalid: %u (expect %u)", __func__, len, (unsigned)sizeof(int));
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: IIR CH len invalid: %u vs %u;", len, (unsigned)sizeof(int));
        return;
    }

    int32_t iir_channel_status = *(int *)(cmd + sizeof(int));
    audio_eq_iir_channel_select_callback((uint8_t *)&iir_channel_status, len);
    APP_TEST_TRACE(3,"[%s] iir_channel_status = %d !", __func__, iir_channel_status);
#else
    APP_TEST_TRACE(3,"[%s] Need enable IIR_EQ_PROCESS_LR_2CH", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: Need enable IIR_EQ_PROCESS_LR_2CH;");
#endif
}

extern int audio_eq_sw_iir_callback(uint8_t *buf, uint32_t  len);
static void audio_test_sw_iir_eq(const char *cmd)
{
    uint32_t len = *((uint32_t *)cmd);
    IIR_CFG_T audio_eq_cfg;

    if (len != sizeof(IIR_CFG_T)) {
        APP_TEST_TRACE(0, "[%s] len is invalid: %d, %d", __func__, len, sizeof(IIR_CFG_T));
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: SW IIR len is invalid: %d, %d;", len, sizeof(IIR_CFG_T));
        return;
    }

    memcpy(&audio_eq_cfg, cmd + sizeof(uint32_t), len);

    APP_TEST_TRACE(0, "[%s] audio_eq_cfg: %d, %d, %d", __func__,
        (int32_t)(audio_eq_cfg.gain0 * 100),
        (int32_t)(audio_eq_cfg.gain1 * 100),
        audio_eq_cfg.num);

    if (audio_process_is_opened() == false) {
        APP_TEST_TRACE(0, "[%s] Failed: audio_process is closed", __func__);
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: Music is stopped;");
        return;
    }

#if defined(__SW_IIR_EQ_PROCESS__)
    audio_eq_sw_iir_callback((uint8_t *)&audio_eq_cfg, len);
#else
    APP_TEST_TRACE(0, "[%s] Need to enable SW_IIR_EQ_PROCESS", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: Need to enable SW_IIR_EQ_PROCESS;");
#endif
}

#include "hw_dac_iir_limiter.h"
static void audio_test_dac_limiter(const char *cmd)
{
    uint32_t len = *((uint32_t *)cmd);
    hw_dac_limiter_cfg_t hw_dac_limiter_cfg;
    APP_TEST_TRACE(3,"[%s] len = %d, sizeof(struct) = %d", __func__, len, sizeof(hw_dac_limiter_cfg_t));

    if (len != sizeof(hw_dac_limiter_cfg_t)) {
        APP_TEST_TRACE(0, "[%s] len is invalid: %d, %d", __func__, len, sizeof(hw_dac_limiter_cfg_t));
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: DAC limiter len is invalid: %d, %d;", len, sizeof(hw_dac_limiter_cfg_t));
        return;
    }

    memcpy(&hw_dac_limiter_cfg, cmd + sizeof(uint32_t), len);

    APP_TEST_TRACE(0,"L ch, enable = %d, delay=%d, thd=%d , att_ms=%d, rls_ms=%d", hw_dac_limiter_cfg.limiter_param_ch0.enable,
                                                                                hw_dac_limiter_cfg.limiter_param_ch0.delay_ms,
                                                                                (int32_t)(hw_dac_limiter_cfg.limiter_param_ch0.thd),
                                                                                (int32_t)(hw_dac_limiter_cfg.limiter_param_ch0.att_ms),
                                                                                (int32_t)(hw_dac_limiter_cfg.limiter_param_ch0.rls_ms));
    APP_TEST_TRACE(0,"R ch, enable = %d, delay=%d, thd=%d , att_ms=%d, rls_ms=%d", hw_dac_limiter_cfg.limiter_param_ch1.enable,
                                                                                hw_dac_limiter_cfg.limiter_param_ch1.delay_ms,
                                                                                (int32_t)(hw_dac_limiter_cfg.limiter_param_ch1.thd),
                                                                                (int32_t)(hw_dac_limiter_cfg.limiter_param_ch1.att_ms),
                                                                                (int32_t)(hw_dac_limiter_cfg.limiter_param_ch1.rls_ms));

    if (audio_process_is_opened() == false) {
        APP_TEST_TRACE(0, "[%s] Failed: audio_process is closed", __func__);
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: Music is stopped;");
        return;
    }

#if defined(__HW_DAC_IIR_LIMITER__)
    hw_dac_limiter_set_cfg(&hw_dac_limiter_cfg);
    hw_dac_limiter_disable();
    hw_dac_limiter_enable();
#else
    APP_TEST_TRACE(0, "[%s] Need to enable HW_DAC_IIR_LIMITER", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: Need to enable HW_DAC_IIR_LIMITER;");
#endif

}

#include "dynamic_eq.h"
extern int audio_dynamic_eq_callback(uint8_t *buf, uint32_t  len);
static void audio_test_dynamic_eq(const char *cmd)
{
    uint32_t len = *((uint32_t *)cmd);
    uint8_t* POSSIBLY_UNUSED payload = (uint8_t*)cmd + sizeof(uint32_t);

    if (len != sizeof(DynamicEqConfig)) {
        APP_TEST_TRACE(0, "[%s] len invalid: %u (expect %u)", __func__, len, (unsigned)sizeof(DynamicEqConfig));
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: len invalid: %u vs %u;", len, (unsigned)sizeof(DynamicEqConfig));
        return;
    }
#if defined(__AUDIO_DYNAMIC_EQ__)
    if (!audio_process_is_opened()) {
        APP_TEST_TRACE(0, "[%s] Failed: audio_process is closed", __func__);
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: Music is stopped;");
        return;
    }

    int rc = audio_dynamic_eq_callback(payload, len);

    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: %d; info: set dynamic EQ;", rc);
#else
    APP_TEST_TRACE(0, "[%s] Need __AUDIO_DYNAMIC_EQ__", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: enable __AUDIO_DYNAMIC_EQ__;");
    return;
#endif
}

#include "bass_enhancer.h"
extern int audio_bass_enhancer_callback(uint8_t *buf, uint32_t len);
static void audio_test_bass_enhancer_cfg(const char *cmd)
{
    uint32_t len = *((uint32_t *)cmd);
    uint8_t* POSSIBLY_UNUSED payload = (uint8_t*)cmd + sizeof(uint32_t);

    if (len != sizeof(BassEnhancerConfig)) {
        APP_TEST_TRACE(0, "[%s] len invalid: %u (expect %u)", __func__, len, (unsigned)sizeof(BassEnhancerConfig));
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: len invalid: %u vs %u;", len, (unsigned)sizeof(BassEnhancerConfig));
        return;
    }
#if defined(__AUDIO_BASS_ENHANCER__)
    if (!audio_process_is_opened()) {
        APP_TEST_TRACE(0, "[%s] Failed: audio_process is closed", __func__);
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: Music is stopped;");
        return;
    }

    int rc = audio_bass_enhancer_callback(payload, len);

    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: %d; info: set bass enhance;", rc);
#else
    APP_TEST_TRACE(0, "[%s] Need __AUDIO_BASS_ENHANCER__", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: enable __AUDIO_BASS_ENHANCER__;");
    return;
#endif
}

#include "stereo_process.h"
extern int audio_virtual_surround_callback(uint8_t *buf, uint32_t len);
static void audio_test_virtual_surround_cfg(const char *cmd)
{
    uint32_t len = *((uint32_t *)cmd);
    uint8_t* POSSIBLY_UNUSED payload = (uint8_t*)cmd + sizeof(uint32_t);

    if (len != sizeof(VirtualSurroundConfig)) {
        APP_TEST_TRACE(0, "[%s] len invalid: %u (expect %u)", __func__, len, (unsigned)sizeof(VirtualSurroundConfig));
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: len invalid: %u vs %u;", len, (unsigned)sizeof(VirtualSurroundConfig));
        return;
    }
#if defined (__VIRTUAL_SURROUND__) || defined (__VIRTUAL_SURROUND_STEREO__)
    if (!audio_process_is_opened()) {
        APP_TEST_TRACE(0, "[%s] Failed: audio_process is closed", __func__);
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: Music is stopped;");
        return;
    }

    int rc = audio_virtual_surround_callback(payload, len);

    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: %d; info: set Spa Audio;", rc);
#else
    APP_TEST_TRACE(0, "[%s] Need __VIRTUAL_SURROUND__", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: enable __VIRTUAL_SURROUND__;");
    return;
#endif
}

#include "limiter.h"
extern int audio_limiter_callback(uint8_t *buf, uint32_t  len);
static void audio_test_limiter_cfg(const char *cmd)
{
    uint32_t len = *((uint32_t *)cmd);
    uint8_t* POSSIBLY_UNUSED payload = (uint8_t*)cmd + sizeof(uint32_t);

    if (len != sizeof(LimiterConfig)) {
        APP_TEST_TRACE(0, "[%s] len invalid: %u (expect %u)", __func__, len, (unsigned)sizeof(LimiterConfig));
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: len invalid: %u vs %u;", len, (unsigned)sizeof(LimiterConfig));
        return;
    }
#if defined(__AUDIO_LIMITER__)
    if (!audio_process_is_opened()) {
        APP_TEST_TRACE(0, "[%s] Failed: audio_process is closed", __func__);
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: Music is stopped;");
        return;
    }

    int rc = audio_limiter_callback(payload, len);

    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: %d; info: set Limiter;", rc);
#else
    APP_TEST_TRACE(0, "[%s] Need __AUDIO_LIMITER__", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: enable __AUDIO_LIMITER__;");
    return;
#endif
}

#include "drc.h"
extern int audio_drc_callback(uint8_t *buf, uint32_t  len);
static void audio_test_sw_drc_cfg(const char *cmd)
{
    uint32_t len = *((uint32_t *)cmd);
    uint8_t* POSSIBLY_UNUSED payload = (uint8_t*)cmd + sizeof(uint32_t);

    if (len != sizeof(DrcConfig)) {
        APP_TEST_TRACE(0, "[%s] len invalid: %u (expect %u)", __func__, len, (unsigned)sizeof(DrcConfig));
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: len invalid: %u vs %u;", len, (unsigned)sizeof(DrcConfig));
        return;
    }
#if defined(__AUDIO_DRC__)
    if (!audio_process_is_opened()) {
        APP_TEST_TRACE(0, "[%s] Failed: audio_process is closed", __func__);
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: Music is stopped;");
        return;
    }

    int rc = audio_drc_callback(payload, len);

    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: %d; info: set Drc;", rc);
#else
    APP_TEST_TRACE(0, "[%s] Need __AUDIO_DRC__", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: enable __AUDIO_DRC__;");
    return;
#endif
}

#include "audio_cfg.h"
static void audio_test_burn_sw_iir_eq_cfg(const char *cmd)
{
    uint32_t len = *((uint32_t *)cmd);
    static IIR_CFG_T audio_eq_cfg;

    if (len != sizeof(IIR_CFG_T)) {
        APP_TEST_TRACE(0, "[%s] len is invalid: %d, %d", __func__, len, sizeof(IIR_CFG_T));
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: EQ len is invalid: %d, %d;", len, sizeof(IIR_CFG_T));
        return;
    }

    memcpy(&audio_eq_cfg, cmd + sizeof(uint32_t), len);

    APP_TEST_TRACE(0, "[%s] audio_eq_cfg: %d, %d, %d", __func__,
        (int32_t)(audio_eq_cfg.gain0 * 100),
        (int32_t)(audio_eq_cfg.gain1 * 100),
        audio_eq_cfg.num);

#if defined(__HW_DAC_IIR_EQ_PROCESS__) || defined(__SW_IIR_EQ_PROCESS__)
    int res = store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_SW_IIR_EQ, (uint8_t *)&audio_eq_cfg);
    if (res) {
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; error code = %d;", res);
    }
#else
    APP_TEST_TRACE(0, "[%s] Need to enable EQ MACRO", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: Need to enable EQ MACRO;");
#endif
}

static void audio_test_burn_hw_dac_iir_eq_cfg(const char *cmd)
{
    uint32_t len = *((uint32_t *)cmd);
    static IIR_CFG_T hw_dac_iir_eq_cfg;

    if (len != sizeof(IIR_CFG_T)) {
        APP_TEST_TRACE(0, "[%s] len invalid: %u != %u", __func__, len, (uint32_t)sizeof(IIR_CFG_T));
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: HW DAC IIR len invalid: %u != %u;", len, (uint32_t)sizeof(IIR_CFG_T));
        return;
    }

    memcpy(&hw_dac_iir_eq_cfg, cmd + sizeof(uint32_t), len);

    // Print key fields without float: gain0/gain1 in 0.1 dB step, num
    APP_TEST_TRACE(0, "[%s] gain0=%d, gain1=%d, num=%d", __func__,
                   (int)(hw_dac_iir_eq_cfg.gain0), (int)(hw_dac_iir_eq_cfg.gain1), hw_dac_iir_eq_cfg.num);

#if defined(__HW_DAC_IIR_EQ_PROCESS__)
    int res = store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_HW_DAC_IIR_EQ, (uint8_t *)&hw_dac_iir_eq_cfg);
    if (res) {
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; error code = %d;", res);
    }
#else
    APP_TEST_TRACE(0, "[%s] Need to enable HW_DAC_IIR_EQ MACRO", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: Need to enable HW_DAC_IIR_EQ MACRO;");
#endif
}

#include "dynamic_eq.h"
static void audio_test_burn_dynamic_eq_cfg(const char *cmd)
{
    uint32_t len = *((uint32_t *)cmd);
    static DynamicEqConfig dynamic_eq_cfg;

    if (len != sizeof(DynamicEqConfig)) {
        APP_TEST_TRACE(0, "[%s] len is invalid: %d, %d", __func__, len, sizeof(DynamicEqConfig));
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: Dynamic eq len is invalid: %d, %d;", len, sizeof(DynamicEqConfig));
        return;
    }

    memcpy(&dynamic_eq_cfg, cmd + sizeof(uint32_t), len);

    APP_TEST_TRACE(0, "[%s] switch_on = %d, debug = %d, offset = %d, gain = %d, eq_num = %d", __func__, dynamic_eq_cfg.switch_on, \
                                                                                dynamic_eq_cfg.debug, \
                                                                                (uint32_t)(10*dynamic_eq_cfg.offset), \
                                                                                (uint16_t)(10*dynamic_eq_cfg.gain), \
                                                                                dynamic_eq_cfg.eq_num);

#if defined(__AUDIO_DYNAMIC_EQ__)
    int res = store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_DYNAMIC_EQ, (uint8_t *)&dynamic_eq_cfg);
    if (res) {
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; error code = %d;", res);
    }
#else
    APP_TEST_TRACE(0, "[%s] Need to enable Dynamic eq MACRO", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: Need to enable Dynamic eq MACRO;");
#endif
}

#include "drc.h"
static void audio_test_burn_drc_cfg(const char *cmd)
{
    uint32_t len = *((uint32_t *)cmd);
    static DrcConfig drc_cfg;

    if (len != sizeof(DrcConfig)) {
        APP_TEST_TRACE(0, "[%s] len is invalid: %d, %d", __func__, len, sizeof(DrcConfig));
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: DRC len is invalid: %d, %d;", len, sizeof(DrcConfig));
        return;
    }

    memcpy(&drc_cfg, cmd + sizeof(uint32_t), len);

    APP_TEST_TRACE(0, "[%s] knee = %d, band_num = %d, look_ahead_time = %d", __func__,
        drc_cfg.knee, \
        drc_cfg.band_num, \
        drc_cfg.look_ahead_time);

#if defined(__AUDIO_DRC__)
    int res = store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_DRC, (uint8_t *)&drc_cfg);
    if (res) {
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; error code = %d;", res);
    }
#else
    APP_TEST_TRACE(0, "[%s] Need to enable DRC MACRO", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: Need to enable DRC MACRO;");
#endif
}

static void audio_test_burn_hw_drc_cfg(const char *cmd)
{
#ifdef __HW_DAC_DRC__
    uint32_t len = *((uint32_t *)cmd);
    uint32_t hw_drc_real_len = sizeof(struct_psap_cfg) / 2 * HW_DAC_DRC_CHANNEL;
    static struct_psap_cfg hw_drc_cfg;

    if (len != hw_drc_real_len) {
        APP_TEST_TRACE(0, "[%s] len is invalid: %d, %d", __func__, len, hw_drc_real_len);
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: hw drc len is invalid: %d, %d;", len, hw_drc_real_len);
        return;
    }

    memcpy(&hw_drc_cfg, cmd + sizeof(uint32_t), len);

    APP_TEST_TRACE(0, "[%s] total_gain = %d, band_num = %d, psap_type = %d", __func__,
        hw_drc_cfg.psap_cfg_l.psap_total_gain, \
        hw_drc_cfg.psap_cfg_l.psap_band_num, \
        hw_drc_cfg.psap_cfg_l.psap_type);

    int res = store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_DRC, (uint8_t *)&hw_drc_cfg);
    if (res) {
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; error code = %d;", res);
    }
#else
    APP_TEST_TRACE(0, "[%s] Need to enable __HW_DAC_DRC__", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: Need to enable hw drc MACRO;");
#endif
}

#include "limiter.h"
static void audio_test_burn_limiter_cfg(const char *cmd)
{
    uint32_t len = *((uint32_t *)cmd);
    static LimiterConfig limiter_cfg;

    if (len != sizeof(LimiterConfig)) {
        APP_TEST_TRACE(0, "[%s] len is invalid: %d, %d", __func__, len, sizeof(LimiterConfig));
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: Limiter len is invalid: %d, %d;", len, sizeof(LimiterConfig));
        return;
    }

    memcpy(&limiter_cfg, cmd + sizeof(uint32_t), len);

    APP_TEST_TRACE(0, "[%s] limiter_cfg: keen=%d, look_ahead_time=%d, release_time=%d", __func__,
        limiter_cfg.knee,
        limiter_cfg.look_ahead_time,
        limiter_cfg.release_time);

#if defined(__AUDIO_LIMITER__)
    int res = store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_LIMITER, (uint8_t *)&limiter_cfg);
    if (res) {
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; error code = %d;", res);
    }
#else
    APP_TEST_TRACE(0, "[%s] Need to enable Limiter MACRO", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: Need to enable Limiter MACRO;");
#endif
}

static void audio_test_burn_hw_dac_limiter_cfg(const char *cmd)
{
    uint32_t len = *((uint32_t *)cmd);
    static hw_dac_limiter_cfg_t dac_limiter_cfg;

    if (len != sizeof(hw_dac_limiter_cfg_t)) {
        APP_TEST_TRACE(0, "[%s] len invalid: %u != %u", __func__, len, (uint32_t)sizeof(hw_dac_limiter_cfg_t));
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: DAC limiter len invalid: %u != %u;", len, (uint32_t)sizeof(hw_dac_limiter_cfg_t));
        return;
    }

    memcpy(&dac_limiter_cfg, cmd + sizeof(uint32_t), len);

    // Print essential fields with scaled floats (x100) to avoid float in TRACE
    const hw_dac_limiter_param_t *p0 = &dac_limiter_cfg.limiter_param_ch0;
    const hw_dac_limiter_param_t *p1 = &dac_limiter_cfg.limiter_param_ch1;
    APP_TEST_TRACE(0, "[%s] CH0: en=%d, delay=%dms, thd=%d, att=%d, rls=%d", __func__,
                   p0->enable, p0->delay_ms,
                   (int)(p0->thd * 100), (int)(p0->att_ms * 100), (int)(p0->rls_ms * 100));
    APP_TEST_TRACE(0, "[%s] CH1: en=%d, delay=%dms, thd=%d, att=%d, rls=%d", __func__,
                   p1->enable, p1->delay_ms,
                   (int)(p1->thd * 100), (int)(p1->att_ms * 100), (int)(p1->rls_ms * 100));

#if defined(__HW_DAC_IIR_LIMITER__)
    int res = store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_HW_DAC_LIMITER, (uint8_t *)&dac_limiter_cfg);
    if (res) {
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; error code = %d;", res);
    }
#else
    APP_TEST_TRACE(0, "[%s] Need to enable HW_DAC_IIR_LIMITER MACRO", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: Need to enable HW_DAC_IIR_LIMITER MACRO;");
#endif
}

static void audio_test_burn_virtual_3D(const char *cmd)
{
    uint32_t len = *((uint32_t *)cmd);
    static VirtualSurroundConfig vs_cfg;

    if (len != sizeof(VirtualSurroundConfig)) {
        APP_TEST_TRACE(0, "[%s] len invalid: %u != %u", __func__, len, (uint32_t)sizeof(VirtualSurroundConfig));
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: Virtual3D len invalid: %u != %u;", len, (uint32_t)sizeof(VirtualSurroundConfig));
        return;
    }

    memcpy(&vs_cfg, cmd + sizeof(uint32_t), len);

    // Print key fields (floats scaled x100)
    APP_TEST_TRACE(0, "[%s] on=%d, pre_delay=%d,%d,%d, allpass=%d,%d,%d,%d,%d,%d", __func__,
                   vs_cfg.switch_on, vs_cfg.pre_delay_1, vs_cfg.pre_delay_2, vs_cfg.pre_delay_3,
                   vs_cfg.allpass_len1, vs_cfg.allpass_len2, vs_cfg.allpass_len3,
                   vs_cfg.allpass_len4, vs_cfg.allpass_len5, vs_cfg.allpass_len6);
    APP_TEST_TRACE(0, "[%s] reverb_in=%d,%d, k=%d,%d,%d, center=%d, surround=%d, out=%d", __func__,
                   (int)(vs_cfg.reverb_input_ratio_1 * 100), (int)(vs_cfg.reverb_input_ratio_2 * 100),
                   (int)(vs_cfg.k1 * 100), (int)(vs_cfg.k2 * 100), (int)(vs_cfg.k3 * 100),
                   (int)(vs_cfg.center_channel_ratio * 100), (int)(vs_cfg.surround_channel_ratio * 100),
                   (int)(vs_cfg.output_ratio * 100));
    APP_TEST_TRACE(0, "[%s] hrtf_in=%d, hrtf_out=%d, hrtf_idx=%d", __func__,
                   (int)(vs_cfg.hrtf_input_ratio * 100), (int)(vs_cfg.hrtf_output_ratio * 100), vs_cfg.hrtf_select_index);

#if defined(__VIRTUAL_SURROUND_STEREO__)
    int res = store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_VIRTUAL_3D, (uint8_t *)&vs_cfg);
    if (res) {
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; error code = %d;", res);
    }
#else
    APP_TEST_TRACE(0, "[%s] Need to enable VIRTUAL_SURROUND_STEREO MACRO", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: Need to enable VIRTUAL_SURROUND_STEREO MACRO;");
#endif
}

static void audio_test_burn_virtualbass_cfg(const char *cmd)
{
    uint32_t len = *((uint32_t *)cmd);
    static BassEnhancerConfig be_cfg;

    if (len != sizeof(BassEnhancerConfig)) {
        APP_TEST_TRACE(0, "[%s] len invalid: %u != %u", __func__, len, (uint32_t)sizeof(BassEnhancerConfig));
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: BassEnhancer len invalid: %u != %u;", len, (uint32_t)sizeof(BassEnhancerConfig));
        return;
    }

    memcpy(&be_cfg, cmd + sizeof(uint32_t), len);

    APP_TEST_TRACE(0, "[%s] on=%d, low_cut=%d, high_cut=%d, gain0=%d, gain1=%d", __func__,
                   be_cfg.switch_on, be_cfg.low_cut_freq, be_cfg.high_cut_freq,
                   (int)(be_cfg.gain0 * 10), (int)(be_cfg.gain1 * 10));

#if defined(__AUDIO_BASS_ENHANCER__)
    int res = store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_VIRTUALBASS, (uint8_t *)&be_cfg);
    if (res) {
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; error code = %d;", res);
    }
#else
    APP_TEST_TRACE(0, "[%s] Need to enable AUDIO_BASS_ENHANCER MACRO", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: Need to enable AUDIO_BASS_ENHANCER MACRO;");
#endif
}

extern int audio_anc_mc_switch_callback(uint8_t *buf, uint32_t  len);
static void audio_test_anc_mc_switch(const char *cmd)
{
    uint32_t len = *((uint32_t *)cmd);
    uint8_t* POSSIBLY_UNUSED payload = (uint8_t*)cmd + sizeof(uint32_t);

    if (len != sizeof(uint32_t)) {
        APP_TEST_TRACE(0, "[%s] len invalid: %u (expect %u)", __func__, len, (unsigned)sizeof(uint32_t));
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: len invalid: %u vs %u;", len, (unsigned)sizeof(uint32_t));
        return;
    }
#if defined(ANC_APP)
    int rc = audio_anc_mc_switch_callback(payload, len);

    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: %d; info: set mc;", rc);
#else
    APP_TEST_TRACE(0, "[%s] Need ANC_APP", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: enable ANC_APP;");
    return;
#endif
}

extern int audio_anc_switch_callback(uint8_t *buf, uint32_t len);
static void audio_test_anc_mode_switch(const char *cmd)
{
    uint32_t len = *((uint32_t *)cmd);
    uint8_t* POSSIBLY_UNUSED payload = (uint8_t*)cmd + sizeof(uint32_t);

    if (len != sizeof(uint32_t)) {
        APP_TEST_TRACE(0, "[%s] len invalid: %u (expect %u)", __func__, len, (unsigned)sizeof(uint32_t));
        g_cmd_send_rsp = true;
        app_debug_tool_printf("[CMD] res: 1; info: len invalid: %u vs %u;", len, (unsigned)sizeof(uint32_t));
        return;
    }
#if defined(ANC_APP)
    int rc = audio_anc_switch_callback(payload, len);

    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: %d; info: set anc mode;", rc);
#else
    APP_TEST_TRACE(0, "[%s] Need ANC_APP", __func__);
    g_cmd_send_rsp = true;
    app_debug_tool_printf("[CMD] res: 1; info: enable ANC_APP;");
    return;
#endif
}

#if 0
#include "audio_cfg.h"
#include "drc.h"
#include "limiter.h"
#include "dynamic_eq.h"
#include "iir_process.h"
extern const IIR_CFG_T audio_eq_anc_hw_dac_iir_cfg;
extern const DrcConfig audio_drc_cfg;
extern const LimiterConfig audio_limiter_cfg;
extern const DynamicEqConfig audio_dynamic_eq_cfg;

static void audio_test_aud_burn_test_store(const char *cmd)
{
    APP_TEST_TRACE(0, "------store eq cfg----------, num = %d", audio_eq_anc_hw_dac_iir_cfg.num);
    for (int i = 0; i < audio_eq_anc_hw_dac_iir_cfg.num; i++) {
        APP_TEST_TRACE(0, "type = %d, gain = %d, freq = %d, Q = %d, ", (uint32_t)(audio_eq_anc_hw_dac_iir_cfg.param[0].type*10), \
                                                                                (uint32_t)(audio_eq_anc_hw_dac_iir_cfg.param[0].gain*10), \
                                                                                (uint32_t)(audio_eq_anc_hw_dac_iir_cfg.param[0].fc*10), \
                                                                                (uint32_t)(audio_eq_anc_hw_dac_iir_cfg.param[0].Q*10));
    }
    store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_IIR_EQ, (uint8_t *)&audio_eq_anc_hw_dac_iir_cfg);
    osDelay(1000);

    APP_TEST_TRACE(0, "------store drc cfg----------");
    APP_TEST_TRACE(0, "knee = %d, band_num = %d, look_ahead_time = %d, ", audio_drc_cfg.knee, \
                                                                                audio_drc_cfg.band_num, \
                                                                                audio_drc_cfg.look_ahead_time);
    store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_DRC, (uint8_t *)&audio_drc_cfg);
    osDelay(1000);

    store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_LIMITER, (uint8_t *)&audio_limiter_cfg);
    osDelay(1000);

    APP_TEST_TRACE(0, "------store dynamic eq cfg----------");
    APP_TEST_TRACE(0, "switch_on = %d, debug = %d, offset = %d, gain = %d, eq_num = %d", audio_dynamic_eq_cfg.switch_on, \
                                                                                audio_dynamic_eq_cfg.debug, \
                                                                                (uint32_t)(10*audio_dynamic_eq_cfg.offset), \
                                                                                (uint16_t)(10*audio_dynamic_eq_cfg.gain), \
                                                                                audio_dynamic_eq_cfg.eq_num);
    store_audio_cfg_into_audio_section(AUDIO_PROCESS_TYPE_DYNAMIC_EQ, (uint8_t *)&audio_dynamic_eq_cfg);
    osDelay(1000);
}

static void audio_test_aud_burn_test_lord(const char *cmd)
{
    IIR_CFG_T *eq_cfg = (IIR_CFG_T *)load_audio_cfg_from_audio_section(AUDIO_PROCESS_TYPE_IIR_EQ, 0);
    APP_TEST_TRACE(0, "------lord eq cfg----------, num = %d", eq_cfg->num);
    for (int i = 0; i < eq_cfg->num; i++) {
        APP_TEST_TRACE(0, "type = %d, gain = %d, freq = %d, Q = %d, ", (uint32_t)(eq_cfg->param[0].type*10), \
                                                                                (uint32_t)(eq_cfg->param[0].gain*10), \
                                                                                (uint32_t)(eq_cfg->param[0].fc*10), \
                                                                                (uint32_t)(eq_cfg->param[0].Q*10));
    }

    osDelay(1000);
    DrcConfig *drc_cfg = load_audio_cfg_from_audio_section(AUDIO_PROCESS_TYPE_DRC, 0);
    if (drc_cfg) {
        APP_TEST_TRACE(0, "------lord drc cfg----------");
        APP_TEST_TRACE(0, "knee = %d, band_num = %d, look_ahead_time = %d, ", drc_cfg->knee, \
                                                                                    drc_cfg->band_num, \
                                                                                    drc_cfg->look_ahead_time);
    }

    osDelay(1000);
    load_audio_cfg_from_audio_section(AUDIO_PROCESS_TYPE_LIMITER, 0);

    osDelay(1000);
    DynamicEqConfig *dy_eq = load_audio_cfg_from_audio_section(AUDIO_PROCESS_TYPE_DYNAMIC_EQ, 0);
    APP_TEST_TRACE(0, "------lord dynamic eq cfg----------");
    APP_TEST_TRACE(0, "switch_on = %d, debug = %d, offset = %d, gain = %d, eq_num = %d", dy_eq->switch_on, \
                                                                                dy_eq->debug, \
                                                                                (uint32_t)(10*dy_eq->offset), \
                                                                                (uint16_t)(10*dy_eq->gain), \
                                                                                dy_eq->eq_num);
}
#endif

const audio_test_func_t audio_test_tool_func[]= {
    {"get_info",            audio_test_get_audio_info},
    {"get_frame_size",      audio_test_get_frame_size},
    {"get_audio_eq_bands",  audio_test_get_audio_eq_bands},
    {"get_psap_band_gain_Q",audio_test_get_psap_band_gain_Q},
    {"get_aud_item_ver",    audio_test_get_aud_item_ver},
    {"audio_eq_cfg",        audio_test_audio_eq_cfg},
    {"sw_iir_eq",           audio_test_sw_iir_eq},
    {"iir_channel",         audio_test_eq_iir_channel_select},
    {"dac_iir_eq",          audio_test_dac_iir_eq},
    {"dyeq",                audio_test_dynamic_eq},
    {"drc",                 audio_test_sw_drc_cfg},
    {"hw_psap_cfg",         audio_test_hw_dac_drc},
    {"limiter",             audio_test_limiter_cfg},
    {"dac_limiter",         audio_test_dac_limiter},
    {"virtual_3D",          audio_test_virtual_surround_cfg},
    {"virtualbass",         audio_test_bass_enhancer_cfg},
    {"aud_burn_eq",         audio_test_burn_sw_iir_eq_cfg},// sw eq
    {"aud_burn_hw_dac_eq",  audio_test_burn_hw_dac_iir_eq_cfg},// two dac eq
    {"aud_burn_dy_eq",      audio_test_burn_dynamic_eq_cfg},
    {"aud_burn_drc",        audio_test_burn_drc_cfg},
    {"aud_burn_hw_drc",     audio_test_burn_hw_drc_cfg},
    {"aud_burn_limiter",    audio_test_burn_limiter_cfg},
    {"aud_burn_dac_limiter",audio_test_burn_hw_dac_limiter_cfg},
    {"burn_virtual_3D",     audio_test_burn_virtual_3D},
    {"burn_virtualbass",    audio_test_burn_virtualbass_cfg},
    {"anc_mode_switch",     audio_test_anc_mode_switch},
    {"anc_mc_switch",       audio_test_anc_mc_switch},
#if 0
    {"aud_burn_store",       audio_test_aud_burn_test_store},
    {"aud_burn_lord",       audio_test_aud_burn_test_lord},
#endif
};

void audio_test_tool_get_func(const audio_test_func_t **func, uint32_t *size)
{
    *func = audio_test_tool_func;
    *size = ARRAY_SIZE(audio_test_tool_func);
}
