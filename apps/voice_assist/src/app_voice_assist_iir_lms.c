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
#ifdef VOICE_ASSIST_FF_IIR_LMS
#include "hal_trace.h"
#include "app_anc_assist.h"
#include "anc_assist.h"
#include "anc_assist_anc.h"
#include "anc_process.h"
#include "audio_dump.h"
#include "app_utils.h"
#include "ae_math.h"
#include "hal_timer.h"
#include "app_anc.h"
#include "app_anc_table.h"
#include "cmsis.h"
#include "anc_ff_iir_lms.h"
#include "anc_fir_lms_aec.h"
#include "anc_fir_lms_eq.h"
#include "event_detection.h"
#include "hal_codec.h"
#include "cmsis_os.h"
#include "app_voice_assist_iir_lms_cfg.h"
#include "app_voice_assist_anc.h"
#include "app_voice_assist_optimal_tf.h"
#include "app_voice_assist_iir_lms.h"
// #include "anc_fir_coeff_config.h"
#include "app_voice_assist_fir_lms_thread.h"
#include "app_voice_assist_event_detection_thread.h"
#include "app_voice_assist_fir_lms_aec_heap.h"
// #include "sensor_hub_core.h"
#if defined(APP_MCPP_CLI)
#include "app_mcpp_comm.h"
#include "app_mcpp.h"
#include "mcpp_server.h"
#endif

#define TRACE VOICE_ASSIST_TRACE

// #define VOICE_FIR_LMS_DUMP_REMOTE
// #define VOICE_FIR_LMS_DUMP_LOCAL
#if defined(VOICE_FIR_LMS_DUMP_LOCAL) || defined(VOICE_FIR_LMS_DUMP_REMOTE)
static short tmp_data[FIR_CAPTURE_BLOCK_SIZE];
#endif

extern const struct_anc_cfg * anc_coef_list_50p7k[];

static uint32_t _index;
static const uint8_t* const user_name = (const uint8_t*)"iir_lms";

// #define HEAP_BUFF_SIZE (232 * 1024 * FIR_CHANNEL_NUM )

#if (FIR_CHANNEL_NUM == 2)
#if defined(VOICE_ASSIST_FF_FIR_LMS_M55)
#define HEAP_BUFF_SIZE (271 * 1024)
#else
#define HEAP_BUFF_SIZE (228 * 1024)
#endif
#else
#if defined(VOICE_ASSIST_FF_FIR_LMS_M55)
#define HEAP_BUFF_SIZE (172 * 1024)
#else
#define HEAP_BUFF_SIZE (100 * 1024)
#endif
#endif

#define EXT_HEAP_ENABLE_MULTI_HEAP
#include "ext_heap.h"
#if defined(APP_MCPP_CLI)
#if (FIR_CHANNEL_NUM == 2)
static  uint8_t SENSOR_HUB_BUFFER_LOCATION fir_ext_heap_event[HEAP_BUFF_EVENT_SIZE];
static  uint8_t *fir_ext_heap = NULL;
#else
static ALIGNED(4) uint8_t fir_ext_heap_event[HEAP_BUFF_EVENT_SIZE];
static ALIGNED(4) uint8_t *fir_ext_heap = NULL;
#endif
#else
static ALIGNED(4) uint8_t fir_ext_heap_event[HEAP_BUFF_EVENT_SIZE];
static ALIGNED(4) uint8_t *fir_ext_heap = NULL;
#endif

static int32_t _iir_tool_debug = 0;
static EventDetectionState *event_st = NULL;

static ANCFFFirLmsSt *fir_st[FIR_CHANNEL_NUM]= {NULL, };
static IIR_LMS_FILTER_CACHE_T cache_cfg[FIR_CHANNEL_NUM];
#ifdef FIR_LMS_AEC_ENABLED
static ANCFFFIRlmsAecSt *aec_st = NULL;
#endif

#if defined(RTOS)
#if (FIR_CHANNEL_NUM == 2)
// stereo
#define FREQ_FIR_LOW       (APP_SYSFREQ_52M)
#define FREQ_FIR_MID       (APP_SYSFREQ_104M)
#define FREQ_FIR_HIGH      (APP_SYSFREQ_208M)
#else
#if defined(VOICE_ASSIST_FF_FIR_LMS_MULTI_THREAD)
// tws dual core
#define FREQ_FIR_LOW       (APP_SYSFREQ_52M)
#define FREQ_FIR_MID       (APP_SYSFREQ_52M)
#define FREQ_FIR_HIGH      (APP_SYSFREQ_104M)
#else
// tws single core
#define FREQ_FIR_LOW       (APP_SYSFREQ_78M)
#define FREQ_FIR_MID       (APP_SYSFREQ_104M)
#define FREQ_FIR_HIGH      (APP_SYSFREQ_208M)
#endif
#endif
typedef enum {
    SET_FREQ_NONE = 0,
    SET_FREQ_FIR_LOW,
    SET_FREQ_FIR_MID,
    SET_FREQ_FIR_HIGH,
    SET_FREQ_TT_LOW,
    SET_FREQ_TT_HIGH,
    SET_FREQ_NUM,
} set_freq_enum_t;
static set_freq_enum_t _set_freq_state = SET_FREQ_NONE;
static void _voice_assist_set_freq(ANC_FF_FIR_LMS_FREQ freq, int line);
#endif
 
#if defined(__NuttX__)
typedef enum {
    SENSOR_CPU0 = 0,
    SENSOR_CPU1 = 1,
    SENSOR_CPU_QTY
} _SENSOR_CPU_T;

static int _thread_switch_to_cpu(osThreadId thread_id, _SENSOR_CPU_T cpu_id)
{
    int thread;
    int ret = 0;
    osThreadDef_t *p_thread_def = NULL;

    p_thread_def = (osThreadDef_t *)thread_id;
    thread = p_thread_def->thread;
    if (thread < 0) {
        TRACE(1, "thread:%d error please check!!!!!\n", thread);
        return 2;
    }

    cpu_set_t cpu_set = 1 << 0;
    cpu_set_t cpu_set_curr;
    if (SENSOR_CPU1 == cpu_id) {
        cpu_set = 1 << 1;
    }
    nxsched_get_affinity(thread, sizeof(cpu_set_curr), &cpu_set_curr);
    if (cpu_set != cpu_set_curr) {
        sched_setaffinity(thread, sizeof(cpu_set), &cpu_set);
        TRACE(2, "thread:%d switch to cpu:%d", thread, cpu_id);
    } else {
        ret = 1;
    }

    return ret;
}
#endif
 
#if defined(RTOS)
static osMutexId _set_freq_mutex_id = NULL;
osMutexDef(_set_freq_mutex);

static void _set_freq_create_lock(void)
{
    if (_set_freq_mutex_id == NULL) {
        _set_freq_mutex_id = osMutexCreate((osMutex(_set_freq_mutex)));
    }
}

static void _set_freq_lock(void)
{
    osMutexWait(_set_freq_mutex_id, osWaitForever);
}

static void _set_freq_unlock(void)
{
    osMutexRelease(_set_freq_mutex_id);
}
#endif

#if !defined(FIR_ADAPT_ANC_M55) && !defined(APP_MCPP_CLI)
static struct_anc_fir_cfg fir_cache_cfg = {
    .anc_fir_cfg_ff_l.fir_bypass_flag = 0,
    .anc_fir_cfg_ff_l.fir_len = LOCAL_FIR_LEN,
    .anc_fir_cfg_ff_l.fir_coef[0] = 0 * 32768 * 256,
    .anc_fir_cfg_mc_l.fir_bypass_flag = 0,
    .anc_fir_cfg_mc_l.fir_len = LOCAL_FIR_LEN,
    .anc_fir_cfg_mc_l.fir_coef[0] = 0.5 * 32768 * 256,
};

static int stop_flag = 0;
int32_t *fir_coeff_cache = NULL;
int32_t *fir_coeff_cache2 = NULL;
int32_t *mc_fir_coeff_cache = NULL;
#endif

static EventDetectionRes event_res;
typedef struct
{
    ANCFFFirLmsSt *anc_inst[FIR_CHANNEL_NUM];
    uint32_t frame_count;
    int32_t mips_needed;
    app_anc_mode_t mode;
    uint32_t ctrl;
    FIR_LMS_CALIB_GAIN calib_gain[FIR_CHANNEL_NUM];
    FIR_LMS_CALIB_GAIN adc_gain[FIR_CHANNEL_NUM];
    bool fir_enable;
    bool fir_status;
    float fb_gain;
    int32_t tt_algo_status;
    noise_status_t noise_status;
} voice_assist_fir_lms_inst;

void app_voice_assist_iir_lms_set_bth_cfg(void* cfg)
{
    memcpy(&bth_cfg,cfg,sizeof(ANC_FF_IIR_LMS_CFG_T));
}

static voice_assist_fir_lms_inst ctx =
{
    .fir_enable = true,
    .fir_status = false,
};

static voice_assist_fir_lms_inst *voice_assist_get_ctx(void)
{
    return &ctx;
}

typedef enum
{
    ANC_FF_FIR_LMS_CMD_SET_CFG = 0,
    ANC_FF_FIR_LMS_CMD_SET_FIR_CFG,
    ANC_FF_FIR_LMS_CMD_SWITCH_MODE,
    ANC_FF_FIR_LMS_CMD_RETURN_RES,
    ANC_FF_FIR_LMS_CMD_SET_IIR,
} ANC_FF_FIR_LMS_CMD;

typedef enum
{
    ANC_FF_FIR_LMS_CTRL_SET_FF_FIR_OFF = 0,
    ANC_FF_FIR_LMS_CTRL_SET_FF_FIR_ON,
    ANC_FF_FIR_LMS_CTRL_SET_ANC_MODE,
    ANC_FF_FIR_LMS_CTRL_START_FIR,
    ANC_FF_FIR_LMS_CTRL_STOP_FIR,
    ANC_FF_FIR_LMS_CTRL_SET_FIR_FLAG,
    ANC_FF_FIR_LMS_CTRL_SET_CALIB_GAIN,
    ANC_FF_FIR_LMS_CTRL_SET_ADC_GAIN,
    ANC_FF_FIR_LMS_CTRL_SET_FB_GAIN,
    ANC_FF_FIR_LMS_CTRL_SET_TT_NS_CFG,
    ANC_FF_FIR_LMS_CTRL_SET_TT_ALGO_STATUS,
    ANC_FF_FIR_LMS_CTRL_SET_NOISE_STATUS,
    ANC_FF_FIR_LMS_CTRL_NUM,
} ANC_FF_FIR_LMS_CTRL;

typedef struct
{
    enum ANC_TYPE_T type;
    aud_fir_item cfg;
} voice_assist_anc_fir_cfg;

typedef struct
{
    app_anc_mode_t mode;
    enum ANC_TYPE_T type;
    ANC_GAIN_TIME gain_delay;
} voice_assist_anc_cfg;

#define MAX_ADAPT_IIR_BAND (6)

typedef struct
{
    enum ANC_TYPE_T type;
    uint32_t index;
    int32_t coeffs[6*MAX_ADAPT_IIR_BAND];
    int32_t coeffs_len;
} voice_assist_anc_iir_cfg;

static void app_voice_assist_iir_lms_get_adc_gain_impl(FIR_LMS_CALIB_GAIN *adc_gain);
static int32_t fir_lms_anc_set_cfg(app_anc_mode_t mode, enum ANC_TYPE_T anc_type, ANC_GAIN_TIME anc_gain_delay);
static void voice_assist_fir_lms_adapt_iir_create(uint8_t *fir_heap);
static void voice_assist_fir_lms_adapt_iir_destory(void);

WEAK int hal_codec_get_adc_gain(enum AUD_CHANNEL_MAP_T map, float *gain)
{
    return 0;
}

static struct_anc_cfg iir_cfg;
static void app_voice_assist_init_iir_cfg(const struct_anc_cfg *cfg)
{
    TRACE(0,"[%s]...",__func__);
    memcpy(&iir_cfg, cfg, sizeof(struct_anc_cfg));
}

static void app_voice_assist_set_iir_coeffs(enum ANC_TYPE_T anc_type, uint32_t index, int32_t *coeffs, int32_t len)
{
    if (anc_type == ANC_FEEDFORWARD) {
        memcpy(&iir_cfg.anc_cfg_ff_l.iir_coef[index], coeffs, len * sizeof(int32_t));
    } if (anc_type == ANC_FEEDBACK) {
        memcpy(&iir_cfg.anc_cfg_fb_l.iir_coef[index], coeffs, len * sizeof(int32_t));
    } else if (anc_type == ANC_TALKTHRU) {
        memcpy(&iir_cfg.anc_cfg_tt_l.iir_coef[index], coeffs, len * sizeof(int32_t));
    }
    anc_set_cfg(&iir_cfg, anc_type, 0);
    // TRACE(0,"coeffs[0]=0x%x coeffs[2]=0x%x coeffs[4]=0x%x len=%d",coeffs[0],coeffs[2],coeffs[4],len);
    // for (uint32_t i = 0; i< ADA_FILTER_CNT; i++) {
    //     TRACE(0,"[%s] filter_idx=%d", __func__, i);
    //     TRACE(0,"[%s] b0=0x%x b1=0x%x b2=0x%x", __func__, coeffs[i * 6], coeffs[i * 6 + 1], coeffs[i * 6 + 2]);
    //     TRACE(0,"[%s] a0=0x%x a1=0x%x a2=0x%x", __func__, coeffs[i * 6 + 3], coeffs[i * 6 + 4], coeffs[i * 6 + 5]);
    // }
}

static POSSIBLY_UNUSED int32_t _assist_anc_dsp_result_callback(void *buf, uint32_t len, void *other, uint32_t sub_cmd)
{
    // TRACE(0,"[%s] get data is %d and %d", __func__, ((struct_anc_fir_cfg *)buf)->anc_fir_cfg_ff_l.fir_len, sub_cmd);
    // TRACE(0, "[%s] sub_cmd = %d", __FUNCTION__, sub_cmd);

    switch (sub_cmd) {
    case ANC_FF_FIR_LMS_CMD_SET_CFG:
    {
        voice_assist_anc_cfg *iir_cfg = (voice_assist_anc_cfg *)buf;
        if (iir_cfg->mode > APP_ANC_MODE_OFF && iir_cfg->mode < APP_ANC_MODE_QTY) {
            anc_set_cfg(anc_coef_list_50p7k[iir_cfg->mode], iir_cfg->type, iir_cfg->gain_delay);
        } else {
            TRACE(0, "[%s] invalid mode %d", __FUNCTION__, iir_cfg->mode);
        }
        break;
    }
    case ANC_FF_FIR_LMS_CMD_SET_IIR:
    {
        voice_assist_anc_iir_cfg *iir_cfg = (voice_assist_anc_iir_cfg *)buf;
        app_voice_assist_set_iir_coeffs(iir_cfg->type, iir_cfg->index, iir_cfg->coeffs, iir_cfg->coeffs_len / sizeof(int32_t));
        break;
    }
    case ANC_FF_FIR_LMS_CMD_SWITCH_MODE:
    {
        if (!_iir_tool_debug){//if fir tool working, do not switch anc
            app_anc_mode_t *mode = (app_anc_mode_t *)buf;
            app_anc_switch_locally(*mode);
        }
        break;
    }
    case ANC_FF_FIR_LMS_CMD_RETURN_RES:
    {
        uint32_t ff_ch_num = MAX_FF_CHANNEL_NUM, fb_ch_num = MAX_FB_CHANNEL_NUM;
        EventDetectionRes *res = (EventDetectionRes *)buf;
        if (any_of_u32(res->ff_gain_changed, ff_ch_num, ANC_ASSIST_ALGO_STATUS_CHANGED) ||
            any_of_u32(res->fb_gain_changed, fb_ch_num, ANC_ASSIST_ALGO_STATUS_CHANGED)) {
            anc_assist_anc_set_gain_coef(res->ff_gain_changed, res->ff_gain_id, res->ff_gain, ff_ch_num, res->fb_gain_changed, res->fb_gain_id, res->fb_gain, fb_ch_num);
        }
        VOICE_ASSIST_TRACE(1, "[%s] ANC_FF_FIR_LMS_CMD_RETURN_RES_NOISE_ENERGY: %d", __func__, (int)res->noise_energy[0]);
        // TODO: deal with stereo headphone
        if (res->curve_changed[0]) {
            // anc_assist_anc_switch_curve(res->curve_id[0], res->curve_index[0]);
        }

        if (res->fir_flag_changed) {
            anc_assist_anc_switch_fir_flag(res->fir_flag_id, res->fir_flag); //to do
        }

        if (res->noise_status[0] == NOISE_STATUS_STRONG_ANC) {
            TRACE(4, "[noise_status]change to strong");
        } else if (res->noise_status[0] == NOISE_STATUS_MIDDLE_ANC) {
            TRACE(3, "[noise_status]change to middle");
        } else if (res->noise_status[0] == NOISE_STATUS_LOWER_ANC) {
            TRACE(2, "[noise_status]change to low");
        } else if (res->noise_status[0] == NOISE_STATUS_QUIET_ANC) {
            TRACE(1, "[noise_status]change to quiet");
        } else {
            TRACE(1, "[noise_status]change to quiet extremely");
        }

        break;
    }
    default:
        TRACE(0, "[%s] cmd  = %d,is invalid", __FUNCTION__, sub_cmd);
        break;
    }

    return 0;
}

static POSSIBLY_UNUSED int32_t app_voice_assist_fir_lms_ctrl(anc_assist_user_t user, uint32_t ctrl, uint8_t *buf, uint32_t len)
{
#if defined(APP_MCPP_CLI)
	return app_mcpp_capture_algo_ctl(APP_MCPP_USER_VOICE_ASSIST_FIR_LMS, ctrl, buf, len, APP_MCPP_SYNC);
#elif defined(FIR_ADAPT_ANC_M55)
    return app_anc_assist_ctrl(user, ctrl, buf, len);
#else
    return voice_assist_iir_lms_set_cfg(ctrl, buf, len);
#endif
}

static POSSIBLY_UNUSED int32_t app_voice_assist_fir_lms_ctrl_async(anc_assist_user_t user, uint32_t ctrl, uint8_t *buf, uint32_t len)
{
#if defined(APP_MCPP_CLI)
	return app_mcpp_capture_algo_ctl(APP_MCPP_USER_VOICE_ASSIST_FIR_LMS, ctrl, buf, len, APP_MCPP_NOTIFY);
#elif defined(FIR_ADAPT_ANC_M55)
    return app_anc_assist_ctrl(user, ctrl, buf, len);
#else
    return voice_assist_iir_lms_set_cfg(ctrl, buf, len);
#endif
}

#if defined(APP_MCPP_CLI)
static void app_voice_assist_fir_lms_receive_data_handler(uint8_t* ptr, short len)
{
    app_voice_assist_core_interact_data_t *receive_data = (app_voice_assist_core_interact_data_t *)ptr;
    //TRACE(2, "<receive data>assist_user:%d len:%d", receive_data->user, receive_data->len);
    _assist_anc_dsp_result_callback(receive_data->buf, receive_data->len,
                                                                NULL, receive_data->sub_cmd);
}
#endif

int32_t app_voice_assist_iir_lms_reset(void)
{
#if defined(APP_MCPP_CLI) || defined(FIR_ADAPT_ANC_M55)
    // TODO: Add app_voice_assist_fir_lms_ctrl(ANC_ASSIST_USER_IIR_LMS);
#else
    for (uint32_t i = 0; i < FIR_CHANNEL_NUM; i++) {
        anc_ff_iir_lms_reset(fir_st[i], 0, AUD_CHANNEL_MAP_CH0 << i);
    }
#endif
    return 0;
}

int32_t app_voice_assist_iir_lms_enable_iir(bool enable)
{
    voice_assist_fir_lms_inst *ctx = voice_assist_get_ctx();

    ctx->fir_enable = enable;

    app_voice_assist_iir_lms_set_iir_status(ctx->fir_status);

    return 0;
}

int32_t app_voice_assist_iir_lms_set_iir_status(bool enable)
{
    voice_assist_fir_lms_inst *ctx = voice_assist_get_ctx();

    ctx->fir_status = enable;

    if (ctx->fir_enable & ctx->fir_status) {
        app_voice_assist_fir_lms_ctrl(_index, ANC_FF_FIR_LMS_CTRL_SET_FF_FIR_ON, NULL, 0);
    } else {
        app_voice_assist_fir_lms_ctrl(_index, ANC_FF_FIR_LMS_CTRL_SET_FF_FIR_OFF, NULL, 0);
    }

    return 0;
}

int32_t app_voice_assist_iir_lms_set_tt_algo_status(int32_t enable)
{
    app_voice_assist_fir_lms_ctrl(_index, ANC_FF_FIR_LMS_CTRL_SET_TT_ALGO_STATUS, (uint8_t *)&enable, sizeof(int32_t));

    return 0;
}

int32_t app_voice_assist_iir_lms_set_anc_mode(app_anc_mode_t mode)
{
    app_voice_assist_fir_lms_ctrl(_index, ANC_FF_FIR_LMS_CTRL_SET_ANC_MODE, &mode, sizeof(mode));

    return 0;
}

int32_t app_voice_assist_iir_lms_update_adc_gain(void)
{
    voice_assist_fir_lms_inst *ctx = voice_assist_get_ctx();
    app_voice_assist_iir_lms_get_adc_gain_impl(ctx->adc_gain);
#if defined(APP_MCPP_CLI)
    app_voice_assist_fir_lms_ctrl(ANC_ASSIST_USER_FIR_LMS, ANC_FF_FIR_LMS_CTRL_SET_ADC_GAIN, (uint8_t *)&ctx->adc_gain, sizeof(ctx->adc_gain));
#else
    for (uint32_t i = 0; i < FIR_CHANNEL_NUM; i++) {
        anc_ff_iir_lms_set_adc_gain(fir_st[i], &ctx->adc_gain[i]);
    }
#endif

    return 0;
}

int32_t app_voice_assist_iir_lms_set_fir_flag(uint32_t fir_flag)
{
    app_voice_assist_fir_lms_ctrl_async(_index, ANC_FF_FIR_LMS_CTRL_SET_FIR_FLAG, (uint8_t *)&fir_flag, sizeof(fir_flag));

    return 0;
}

int32_t app_voice_assist_iir_lms_set_fb_gain(float gain)
{
    app_voice_assist_fir_lms_ctrl(_index, ANC_FF_FIR_LMS_CTRL_SET_FB_GAIN, (uint8_t *)&gain, sizeof(gain));

    return 0;
}

int32_t app_voice_assist_iir_lms_set_noise_status(noise_status_t noise_status)
{
    app_voice_assist_fir_lms_ctrl(_index, ANC_FF_FIR_LMS_CTRL_SET_NOISE_STATUS, (uint8_t *)&noise_status, sizeof(noise_status));

    return 0;
}

int32_t app_voice_assist_iir_lms_start_fir(void)
{
    app_voice_assist_fir_lms_ctrl(_index, ANC_FF_FIR_LMS_CTRL_START_FIR, NULL, 0);

    return 0;
}

int32_t app_voice_assist_iir_lms_stop_fir(void)
{
    app_voice_assist_fir_lms_ctrl(_index, ANC_FF_FIR_LMS_CTRL_STOP_FIR, NULL, 0);

    return 0;
}

#if defined(APP_MCPP_CLI) || defined(FIR_ADAPT_ANC_M55)
static int32_t _voice_assist_fir_lms_return_res_handler(EventDetectionRes *res);
#endif

static int32_t fir_lms_return_res_handler(EventDetectionRes *res)
{
#if defined(APP_MCPP_CLI) || defined(FIR_ADAPT_ANC_M55)
    return _voice_assist_fir_lms_return_res_handler(res);
#else
    return _assist_anc_dsp_result_callback(res, sizeof(EventDetectionRes), NULL, ANC_FF_FIR_LMS_CMD_RETURN_RES);
#endif
}

#if defined(APP_MCPP_CLI) || defined(FIR_ADAPT_ANC_M55)
static int32_t _voice_assist_anc_set_cfg_handler(app_anc_mode_t mode, enum ANC_TYPE_T anc_type, ANC_GAIN_TIME anc_gain_delay);
#endif

static POSSIBLY_UNUSED int32_t fir_lms_anc_set_cfg(app_anc_mode_t mode, enum ANC_TYPE_T anc_type, ANC_GAIN_TIME anc_gain_delay)
{
    if (mode <= APP_ANC_MODE_OFF || mode >= APP_ANC_MODE_QTY) {
        TRACE(0, "[%s] invalid mode %d", __FUNCTION__, mode);
        return -1;
    }

#if defined(APP_MCPP_CLI) || defined(FIR_ADAPT_ANC_M55)
    return _voice_assist_anc_set_cfg_handler(mode, anc_type, anc_gain_delay);
#else
    return anc_set_cfg(anc_coef_list_50p7k[mode - 1], anc_type, anc_gain_delay);
#endif
}

#if defined(APP_MCPP_CLI) || defined(FIR_ADAPT_ANC_M55)
static int32_t _voice_assist_set_iir_handler(enum ANC_TYPE_T anc_type, uint32_t index, uint8_t *coeffs, int32_t len);
#endif


static int32_t fir_lms_anc_set_iir(enum ANC_TYPE_T anc_type, uint32_t index, int32_t *coeffs, int32_t len)
{
    TRACE(0,"[%s] index=%d, coeffs[0]=0x%x coeffs[1]=0x%x coeffs[2]=0x%x len=%d", __func__, index, coeffs[0], coeffs[1], coeffs[2],len);

#if defined(APP_MCPP_CLI) || defined(FIR_ADAPT_ANC_M55)
    ASSERT(len % 6 == 0, "[%s] Each stage must contains 6 coeffs", __FUNCTION__);
    ASSERT(len / 6 <= MAX_ADAPT_IIR_BAND, "[%s] max support 6 band", __FUNCTION__);
    return _voice_assist_set_iir_handler(anc_type, index, (uint8_t *)coeffs, len * sizeof(int32_t));
#else
    app_voice_assist_set_iir_coeffs(anc_type, index, coeffs, len);
    return 0;
#endif
}

WEAK int32_t app_anc_get_calib_gain(FIR_LMS_CALIB_GAIN *calib_gain)
{
    for (uint32_t i = 0; i < FIR_CHANNEL_NUM; i++) {
        calib_gain[i].ff_gain = 512;
        calib_gain[i].fb_gain = 512;
        calib_gain[i].mc_gain = 512;
    }
    TRACE(0, "[%s] Warning: please impl this function", __FUNCTION__);

    return 0;
}

static void _voice_assist_ff_iir_convergence(void)
{
    VOICE_ASSIST_TRACE(0, "[%s] Warning: please impl this function", __FUNCTION__);
}

//map from hal_codec_apply_anc_adc_gain_offset
static void app_voice_assist_iir_lms_get_adc_gain_impl(FIR_LMS_CALIB_GAIN *adc_gain)
{
    float gain;
    hal_codec_get_adc_gain(AUD_CHANNEL_MAP_CH0, &gain);
    adc_gain[0].ff_gain = gain;
    hal_codec_get_adc_gain(AUD_CHANNEL_MAP_CH2, &gain);
    adc_gain[0].fb_gain = gain;
    TRACE(0,"dig ff_l gain:%de-2 ,fb_l gain:%de-2",(int)(adc_gain[0].ff_gain*100),(int)(adc_gain[0].fb_gain*100));
#if (FIR_CHANNEL_NUM == 2)
    hal_codec_get_adc_gain(AUD_CHANNEL_MAP_CH1, &gain);
    adc_gain[1].ff_gain = gain;
    hal_codec_get_adc_gain(AUD_CHANNEL_MAP_CH3, &gain);
    adc_gain[1].fb_gain = gain;
    TRACE(0,"dig ff_r gain:%de-2 ,fb_r gain:%de-2",(int)(adc_gain[1].ff_gain*100),(int)(adc_gain[1].fb_gain*100));
#endif
}

int32_t voice_assist_iir_lms_open_inner(uint8_t *buffer)
{
    TRACE(0, "[%s] fir lms start stream", __func__);

    static FIR_LMS_CALIB_GAIN calib_gain[FIR_CHANNEL_NUM];
    app_anc_get_calib_gain(calib_gain);
    static uint32_t *share_list[3];
    share_list[0] = ( uint32_t*)(&calib_gain);
    share_list[1] = ( uint32_t*)(&bth_cfg);
    share_list[2] = ( uint32_t*)buffer;

#if !defined(APP_MCPP_CLI) && !defined(FIR_ADAPT_ANC_M55)
    stop_flag = 0;
    voice_assist_iir_lms_open(share_list);
#else
    _voice_assist_set_freq(ANC_FF_FIR_LMS_FREQ_LOW, __LINE__);
#endif

#if defined(APP_MCPP_CLI)
    APP_MCPP_CFG_T dsp_cfg;
    memset(&dsp_cfg, 0, sizeof(APP_MCPP_CFG_T));
    dsp_cfg.capture.stream_enable = true;
    dsp_cfg.capture.sample_rate = FIR_CAPTURE_SAMPLE_RATE;
    dsp_cfg.capture.sample_bytes = 4;
    dsp_cfg.capture.algo_frame_len = FIR_CAPTURE_BLOCK_SIZE;
    dsp_cfg.capture.params[1] = app_mcpp_get_cap_channel_map(false, false, false);
    dsp_cfg.capture.channel_num = MIC_INDEX_QTY;
    dsp_cfg.capture.delay = FIR_CAPTURE_BLOCK_SIZE * 3;
#if defined(VOICE_ASSIST_FF_FIR_LMS_BTH_M55)
	dsp_cfg.capture.core_server = APP_MCPP_CORE_M55;
#elif defined(VOICE_ASSIST_FF_FIR_LMS_BTH_SENS)
    dsp_cfg.capture.core_server = APP_MCPP_CORE_SENS;
#else
	ASSERT(0, "[%s] FIR LMS can only run on M55 or SENS", __FUNCTION__);
#endif
    // dsp_cfg.capture.params[2] = (int)&calib_gain;
    dsp_cfg.capture.params[2] = (int)&share_list;

    app_mcpp_open(APP_MCPP_USER_VOICE_ASSIST_FIR_LMS, &dsp_cfg);
    app_mcpp_capture_async_ctl_cb_register(APP_MCPP_USER_VOICE_ASSIST_FIR_LMS, app_voice_assist_fir_lms_receive_data_handler);

    voice_assist_fir_lms_inst *ctx = voice_assist_get_ctx();
    if (ctx->fir_enable & ctx->fir_status) {
        app_voice_assist_fir_lms_ctrl(_index, ANC_FF_FIR_LMS_CTRL_SET_FF_FIR_ON, NULL, 0);
    } else {
        app_voice_assist_fir_lms_ctrl(_index, ANC_FF_FIR_LMS_CTRL_SET_FF_FIR_OFF, NULL, 0);
    }
#endif

    anc_fir_lms_eq_init(FIR_CAPTURE_SAMPLE_RATE, FIR_CAPTURE_BLOCK_SIZE);

    //fir_coeff_cache = fir_lms_coeff_cache(fir_st);
    //mc_fir_coeff_cache = mc_fir_lms_coeff_cache(fir_st);

#ifdef VOICE_FIR_LMS_DUMP_LOCAL
    audio_dump_init(FIR_CAPTURE_BLOCK_SIZE, sizeof(short), 3);
#endif

    // close fb anc for adaptive anc, it is better not to open it during the init state
    // anc_set_gain(0, 0, ANC_FEEDBACK);

    if (!_iir_tool_debug){
        app_voice_assist_init_iir_cfg(anc_coef_list_50p7k[0]);
    }

    return 0;
}

extern uint8_t is_a2dp_or_sco_on_starting(void);
int32_t voice_assist_iir_lms_close_inner(void);
int32_t app_anc_assist_iir_lms_close(void)
{
    voice_assist_iir_lms_close_inner();
    return 0;
}

int32_t app_voice_assist_iir_lms_open_v1(void)
{
    TRACE(0, "[%s] fir lms open stream", __func__);

    app_anc_assist_open(_index);

    return 0;
}

int32_t app_voice_assist_iir_lms_close_v1(void)
{
    TRACE(0, "[%s] fir lms close stream", __func__);

    app_anc_assist_close(_index);

    return 0;
}

int32_t voice_assist_iir_lms_close_inner(void)
{
    TRACE(0, "[%s] fir lms close stream", __func__);

    //app_anc_assist_close(_index);

#if defined(APP_MCPP_CLI)
    app_mcpp_close(APP_MCPP_USER_VOICE_ASSIST_FIR_LMS);
#elif defined(FIR_ADAPT_ANC_M55)
#else
    voice_assist_iir_lms_close();
#endif

    app_sysfreq_req(APP_SYSFREQ_USER_FIR_LMS, APP_SYSFREQ_32K);

#ifdef VOICE_FIR_LMS_DUMP_LOCAL
    audio_dump_deinit();
#endif
    return 0;
}

static int32_t voice_assist_iir_get_heap_buf_size(anc_assist_mode_t assist_mode)
{
    if (assist_mode == ANC_ASSIST_MODE_PHONE_CALL || assist_mode == ANC_ASSIST_MODE_RECORD || assist_mode == ANC_ASSIST_MODE_LE_CALL) {
        return 0;
    } else {
        return HEAP_BUFF_SIZE;
    }
}

static uint8_t *g_iir_heap_buffer = NULL;
static int32_t voice_assist_iir_lms_open_v1()
{
    uint32_t heap_size = voice_assist_iir_get_heap_buf_size(app_anc_assist_get_mode());

    if (heap_size) {
        g_iir_heap_buffer = voice_assist_stream_heap_malloc(heap_size);
    }
    voice_assist_iir_lms_open_inner(g_iir_heap_buffer);

    return 0;
}

static int32_t voice_assist_iir_lms_close_v1()
{
    voice_assist_iir_lms_close_inner();
    if (g_iir_heap_buffer != NULL) {
        voice_assist_stream_heap_free(g_iir_heap_buffer);
        g_iir_heap_buffer = NULL;
    }
    return 0;
}

static int32_t _voice_assist_iir_lms_callback(void *buf, uint32_t len, void *other);
static int32_t voice_assist_iir_process(voice_assist_process_frame_data_t *process_frame_data)
{
    static const uint8_t* const ptr_name = (const uint8_t*)"anc";
    AncAssistRes *anc_assist_res = ((voice_anc_msg_t *)voice_assist_get_user_ptr(ptr_name))->res;
    ASSERT(anc_assist_res != NULL, "[%s] anc_assist_res is NULL", __func__);

    anc_assist_pcm_t * input_data[MIC_INDEX_QTY+1];
    uint32_t j = 0;
    for (uint32_t i = 0; i < MAX_FF_CHANNEL_NUM; i++) {
        input_data[j] = process_frame_data->ff_mic[i];
        j++;
    }
    for (uint32_t i = 0; i < MAX_FB_CHANNEL_NUM; i++) {
        input_data[j] = process_frame_data->fb_mic[i];
        j++;
    }
    for (uint32_t i = 0; i < MAX_TALK_CHANNEL_NUM; i++) {
        input_data[j] = process_frame_data->talk_mic[i];
        j++;
    }
    for (uint32_t i = 0; i < MAX_REF_CHANNEL_NUM; i++) {
        input_data[j] = process_frame_data->ref[i];
        j++;
    }
#if defined(ANC_ASSIST_VPU)
        input_data[MIC_INDEX_QTY] = g_vpu_mic_buf + offset;
#else
        input_data[MIC_INDEX_QTY] = NULL;
#endif
    _voice_assist_iir_lms_callback(input_data, process_frame_data->frame_len, anc_assist_res);

    return 0;
}

static int32_t voice_assist_iir_lms_get_fs(void)
{
    return FIR_CAPTURE_SAMPLE_RATE;
}

static int32_t voice_assist_iir_lms_set_mode(anc_assist_set_mode_t mode)
{
    ANC_TRACE(0,"[%s]...",__func__);
    uint32_t heap_size = voice_assist_iir_get_heap_buf_size(app_anc_assist_get_mode());

    if (mode == ANC_ASSIST_SET_MODE_ON) {
        if (heap_size) {
            g_iir_heap_buffer = voice_assist_stream_heap_malloc(heap_size);
            voice_assist_fir_lms_adapt_iir_create(g_iir_heap_buffer);
        }
    } else if (mode == ANC_ASSIST_SET_MODE_OFF) {
        if (fir_st[0]){
            voice_assist_fir_lms_adapt_iir_destory();
            if (g_iir_heap_buffer != NULL) {
                voice_assist_stream_heap_free(g_iir_heap_buffer);
                g_iir_heap_buffer = NULL;
            }
        }
    }

    return 0;
}

static const voice_assist_algo_callback_t iir_assist_algo_dsp = {
    .open               = voice_assist_iir_lms_open_v1,
    .close              = voice_assist_iir_lms_close_v1,
    .cap_process        = voice_assist_iir_process,
    .get_fs             = voice_assist_iir_lms_get_fs,
    .set_mode           = voice_assist_iir_lms_set_mode,
    .get_heap_buf_size  = voice_assist_iir_get_heap_buf_size,
};

static int32_t voice_assist_iir_get_mic_map()
{
    uint32_t mic_map = 0;

    mic_map |= ANC_FF_MIC_CH_L | ANC_FB_MIC_CH_L | ANC_TALK_MIC_CH_L | ANC_REF_MIC_CH_L | ANC_REF_MIC_CH_R;
#if defined(FREEMAN_ENABLED_STERO)
    mic_map |= ANC_FF_MIC_CH_R | ANC_FB_MIC_CH_R | ANC_TALK_MIC_CH_R;
#endif

    AncAssistConfig *cfg = app_voice_assist_anc_get_cfg();
    cfg->iir_lms_en = true;

    TRACE(0, "[%s]", __FUNCTION__);

    return mic_map;
}

static int32_t voice_assist_iir_set_mic_cfg()
{
#if(FIR_CHANNEL_NUM == 1)
#if defined(CODEC_ECHO_PATH_VER) && (CODEC_ECHO_PATH_VER >= 2)
    //ref_l:music data;ref_r:music+anc
    hal_codec_set_echo_ch1(HAL_CODEC_ECHO_CHAN_DAC_DATA_DBG_OUT_L); //on the one channel mode, need to use another echo ch
    hal_codec_set_echo_ch1_rate(HAL_CODEC_ECHO_RATE_384K);
    hal_codec_set_echo1_path(HAL_CODEC_ECHO_PATH_HBF4_DATA_IN);
#else
    //ref_l:music data;ref_r:music+anc 1306p
#if defined(CHIP_BEST1307P) || defined(CHIP_BEST1503) ||defined(CHIP_BEST1307PH)
    // only 1ch ecmic
    hal_codec_set_echo_path(HAL_CODEC_ECHO_PATH_DAC1_DAC2_PSAP);//ref_l
#elif defined(CHIP_BEST1502P)
    hal_codec_set_echo1_path(HAL_CODEC_ECHO_PATH_DAC1_DAC2_PSAP); //ref_l
    hal_codec_set_echo_path(HAL_CODEC_ECHO_PATH_ALL); //ref_r
#else
    hal_codec_set_echo_path(HAL_CODEC_ECHO_PATH_DAC1_DAC2_PSAP);//ref_l
    hal_codec_set_echo1_path(HAL_CODEC_ECHO_PATH_ALL);//ref_r
#endif
#endif
#elif (FIR_CHANNEL_NUM == 2)
    hal_codec_set_echo_path(HAL_CODEC_ECHO_PATH_ALL);
    hal_codec_set_echo1_path(HAL_CODEC_ECHO_PATH_ALL);
#endif
    TRACE(0, "[%s]", __FUNCTION__);
    anc_adc_data_select(ANC_ADC_ADC_ADD_MC); //fb DAM data

    return 0;
}

static const voice_assist_stream_callback_t voice_assist_iir_stream = {
    .get_mic_ch_map       = voice_assist_iir_get_mic_map,
    .set_mic_cfg          = voice_assist_iir_set_mic_cfg,
};

static voice_assist_user_register_cfg voice_assist_iir_stream_cfg = {
    .voice_assist_callback = &voice_assist_iir_stream,
    .assist_algo_dsp = &iir_assist_algo_dsp,
};

int32_t app_voice_assist_iir_lms_init(void)
{
#if defined(FIR_ADAPT_ANC_M55)
    app_anc_assist_result_register(_index, _assist_anc_dsp_result_callback);
#endif
    _index = app_voice_assist_stream_register(user_name, &voice_assist_iir_stream_cfg, NULL);
    app_voice_assist_dsp_register(&_index, user_name, &voice_assist_iir_stream_cfg);

    return 0;
}


static POSSIBLY_UNUSED inline bool any_of_lms_state(ANCFFFirLmsSt **status, uint32_t len, void *expected_status)
{
	for (uint32_t i = 0; i < len; i++) {
		if (status[i] == expected_status)
			return true;
	}

	return false;
}

static int voice_assist_iir_lms_update_cfg(voice_assist_fir_lms_inst *ctx);

static int32_t voice_assist_event_detection_process(process_frame_data_t *buf, EventController* event_control)
{
    if (event_st != NULL) {
//     TRACE(0, "[%s] Sys freq: %d", __func__, hal_sys_timer_calc_cpu_freq(5, 0));
    // uint32_t start_time = hal_fast_sys_timer_get();
        event_detection_process(event_st, buf, FIR_CAPTURE_BLOCK_SIZE, &event_res, event_control);
    // TRACE(0, "[%s] ------------event_st--------%d us", __func__, FAST_TICKS_TO_US(hal_fast_sys_timer_get() - start_time));
    // static uint32_t cnt2 = 0;
    // TRACE(0,"[event_detection_done] cnt = %d", cnt2++);
        if (any_of_u32(event_res.ff_gain_changed, MAX_FF_CHANNEL_NUM, ANC_ASSIST_ALGO_STATUS_CHANGED) ||
            any_of_u32(event_res.fb_gain_changed, MAX_FB_CHANNEL_NUM, ANC_ASSIST_ALGO_STATUS_CHANGED) ||
            any_of_u32(event_res.curve_changed, MAX_FB_CHANNEL_NUM, ANC_ASSIST_ALGO_STATUS_CHANGED) ||
            event_res.fir_flag_changed) {
            // TRACE(1, "fir res is change and send to bth");
            fir_lms_return_res_handler(&event_res);

        }
    }
    return 0;
}

static int32_t voice_assist_anc_ff_fir_lms_process(process_frame_data_t *buf, EventController* event_control)
{
    if (any_of_lms_state(fir_st, FIR_CHANNEL_NUM, NULL))
        return 0;

    voice_assist_fir_lms_inst *ctx = voice_assist_get_ctx();

    voice_assist_iir_lms_update_cfg(ctx);

// TRACE(0, "[%s] Sys freq: %d", __func__, hal_sys_timer_calc_cpu_freq(5, 0));
// uint32_t start_time2 = hal_fast_sys_timer_get();
    for (uint32_t i = 0; i < FIR_CHANNEL_NUM; i++) {
#if (FIR_CHANNEL_NUM == 2)
#if defined(ANC_ASSIST_SUPPORT_FOUR_REF)
        // 4ref stereo anc ref is in ref channel 2 and channel 3
        POSSIBLY_UNUSED static anc_assist_pcm_t ref_anc[FIR_CAPTURE_BLOCK_SIZE];
        for (uint32_t j = 0; j < FIR_CAPTURE_BLOCK_SIZE; j++){
            ref_anc[j] =  buf->ref[i+2][j]- buf->ref[i][j];
        }
        anc_assist_pcm_t* mic_data[3] = {buf->ff_mic[i], buf->fb_mic[i], ref_anc};
#else
        //2ref
        anc_assist_pcm_t* mic_data[3] = {buf->ff_mic[i], buf->fb_mic[i], buf->ref[i]};
#endif
#else
#if (ANC_REF_MIC_CH_R==0)
        // 1503 only 1 ref path
        anc_assist_pcm_t* mic_data[3] = {buf->ff_mic[i], buf->fb_mic[i], NULL};
#else
        // tws anc ref is in ref channel 1
        POSSIBLY_UNUSED static anc_assist_pcm_t ref_anc[FIR_CAPTURE_BLOCK_SIZE];
        for (uint32_t j = 0; j < FIR_CAPTURE_BLOCK_SIZE; j++){
            ref_anc[j] =  buf->ref[1][j]- buf->ref[0][j];
        }
        anc_assist_pcm_t* mic_data[3] = {buf->ff_mic[i], buf->fb_mic[i], ref_anc};
#endif
#endif
        anc_ff_iir_lms_process(fir_st[i], mic_data, FIR_CAPTURE_BLOCK_SIZE, event_control, AUD_CHANNEL_MAP_CH0 << i);
    }
// TRACE(0,"[%s] ------------ff_fir--------%d us", __func__, FAST_TICKS_TO_US(hal_fast_sys_timer_get() - start_time2));
#if defined(APP_MCPP_CLI)
    // mcpp_srv_ff_fir_process_done();
#endif

    ctx->frame_count += 1;

    return 0;
}

#if (FIR_CAPTURE_SAMPLE_RATE == 32000)
static int16_t dump_data[240];
static POSSIBLY_UNUSED void dump_fir_coeff(int32_t *fir_coeff_cache, uint32_t len)
{
    int32_t *tmp_p = fir_coeff_cache;
    for (uint32_t i = 0; i < 240; i++) {
        dump_data[i] = 0x5555;
    }
    audio_dump_add_channel_data(0, dump_data, 240);

    for (uint32_t j = 0; j < 240; j++) {
        dump_data[j] = *tmp_p++ >> 8;
    }
    audio_dump_add_channel_data(1, dump_data, 240);

    for (uint32_t j = 0; j < 232; j++) {
        dump_data[j] = *tmp_p++ >> 8;
    }
    audio_dump_add_channel_data(2, dump_data, 232);

    audio_dump_run();
}
#elif (FIR_CAPTURE_SAMPLE_RATE == 16000)
static int16_t dump_data[FIR_CAPTURE_BLOCK_SIZE];

static POSSIBLY_UNUSED void dump_fir_coeff(int32_t *fir_coeff_cache, uint32_t len)
{
    int32_t *tmp_p = fir_coeff_cache;
    for (uint32_t i = 0; i < 8; i++) {
        dump_data[i] = 0x5555;
    }

    for (uint32_t j = 8; j < FIR_CAPTURE_BLOCK_SIZE; j++) {
        dump_data[j] = *tmp_p++ >> 8;
    }
    audio_dump_add_channel_data(0, dump_data, FIR_CAPTURE_BLOCK_SIZE);
    for (uint32_t j = 0; j < FIR_CAPTURE_BLOCK_SIZE; j++) {
        dump_data[j] = *tmp_p++ >> 8;
    }
    audio_dump_add_channel_data(1, dump_data, FIR_CAPTURE_BLOCK_SIZE);

    audio_dump_run();
}
#endif

int voice_assist_iir_lms_process(process_frame_data_t *process_frame_data);

static int32_t _voice_assist_iir_lms_callback(void * buf, uint32_t len, void *other)
{
#ifdef VOICE_FIR_LMS_DUMP_LOCAL
    anc_assist_pcm_t ** input_data_tmp = buf;
    audio_dump_clear_up();
    for(int32_t i = 0; i < FIR_CAPTURE_BLOCK_SIZE; i++) {
        tmp_data[i] = (int32_t)input_data_tmp[0][i] >> 5;
    }
    audio_dump_add_channel_data(0, tmp_data, FIR_CAPTURE_BLOCK_SIZE);

    for(int32_t i = 0; i < FIR_CAPTURE_BLOCK_SIZE; i++) {
        tmp_data[i] = (int32_t)input_data_tmp[1][i] >> 5;
    }
    audio_dump_add_channel_data(1, tmp_data, FIR_CAPTURE_BLOCK_SIZE);

    for(int32_t i = 0; i < FIR_CAPTURE_BLOCK_SIZE; i++) {
        tmp_data[i] = (int32_t)input_data_tmp[2][i] >> 5;
    }
    audio_dump_add_channel_data(2, tmp_data, FIR_CAPTURE_BLOCK_SIZE);
    audio_dump_run();
#endif

    anc_assist_pcm_t **input_data = buf;

#ifdef ANC_ASSIST_16BIT
    anc_fir_lms_eq_process_q15(input_data, FIR_CAPTURE_BLOCK_SIZE);
#else
    anc_fir_lms_eq_process_f32(input_data, FIR_CAPTURE_BLOCK_SIZE);
#endif

#if defined(APP_MCPP_CLI)
    static float _temp_buf[MIC_INDEX_QTY * FIR_BLOCK_SIZE];
    for (uint32_t j = 0; j < MIC_INDEX_QTY; j++) {
        memcpy(_temp_buf + j * FIR_CAPTURE_BLOCK_SIZE, input_data[j], FIR_CAPTURE_BLOCK_SIZE * sizeof(uint32_t));
    }

    APP_MCPP_CAP_PCM_T pcm_cfg;
    memset(&pcm_cfg, 0, sizeof(pcm_cfg));
    pcm_cfg.in = _temp_buf;
    pcm_cfg.frame_len = len;
    pcm_cfg.out = _temp_buf;

    app_mcpp_capture_process(APP_MCPP_USER_VOICE_ASSIST_FIR_LMS, &pcm_cfg);

#elif defined(FIR_ADAPT_ANC_M55)
#else
    if (stop_flag == 0) {
        int loop_cnt = 1;
        int offset = 0;

        ASSERT(((len % FIR_CAPTURE_BLOCK_SIZE) == 0), "[%s] the data_len %d is error ", __func__,len);
        loop_cnt = len / FIR_CAPTURE_BLOCK_SIZE;
        for (int i = 0; i < loop_cnt; i++) {
            anc_assist_pcm_t *ff_data[MAX_FF_CHANNEL_NUM] = {};
            anc_assist_pcm_t *fb_data[MAX_FB_CHANNEL_NUM] = {};
            anc_assist_pcm_t *talk_data[MAX_TALK_CHANNEL_NUM] = {};
            anc_assist_pcm_t *ref_data[MAX_REF_CHANNEL_NUM] = {};
            anc_assist_pcm_t *vpu_data = NULL;
            uint32_t j = 0;
            for (uint32_t i = 0; i < MAX_FF_CHANNEL_NUM; i++){
                ff_data[i] = input_data[j] + offset;
                j++;
            }
            for (uint32_t i = 0; i < MAX_FB_CHANNEL_NUM; i++){
                fb_data[i] = input_data[j] + offset;
                j++;
            }
            for (uint32_t i = 0; i < MAX_TALK_CHANNEL_NUM; i++){
                talk_data[i] = input_data[j] + offset;
                j++;;
            }
            for (uint32_t i = 0; i < MAX_REF_CHANNEL_NUM; i++){
                ref_data[i] = input_data[j] + offset;
                j++;
            }
        #if defined(ANC_ASSIST_VPU)
            vpu_data = input_data[j];
        #endif


            // TRACE(0, "[%s] fir lms process...", __func__);
            process_frame_data_t process_frame_data;

            process_frame_data.ff_mic = ff_data;
            process_frame_data.ff_ch_num = MAX_FF_CHANNEL_NUM;
            process_frame_data.fb_mic = fb_data;
            process_frame_data.fb_ch_num = MAX_FB_CHANNEL_NUM;
            process_frame_data.talk_mic = talk_data;
            process_frame_data.talk_ch_num = MAX_TALK_CHANNEL_NUM;
            process_frame_data.ref = ref_data;
            process_frame_data.ref_ch_num = MAX_REF_CHANNEL_NUM;
            process_frame_data.vpu_mic = vpu_data;
            process_frame_data.frame_len = FIR_CAPTURE_BLOCK_SIZE;
            voice_assist_iir_lms_process(&process_frame_data);

            offset += FIR_CAPTURE_BLOCK_SIZE;
        }
    } else {
        return 0;
    }
#endif
    return 0;
}

#if !defined(APP_MCPP_CLI) && !defined(FIR_ADAPT_ANC_M55)
void set_fir_cache(void)
{
    for (uint32_t i = 0; i < LOCAL_FIR_LEN; i++)
        fir_cache_cfg.anc_fir_cfg_ff_l.fir_coef[i]=fir_coeff_cache[i];
    anc_set_fir_cfg(&fir_cache_cfg, ANC_FEEDFORWARD);
}
#endif

#if defined(FIR_ADAPT_ANC_M55) || defined(APP_MCPP_CLI)
// static int32_t *fir_coeff_cache = NULL;
static app_voice_assist_core_interact_data_t interact_data;

static int32_t _voice_assist_fir_lms_return_res_handler(EventDetectionRes *res)
{
    uint8_t user = _index;
    uint8_t cmd = ANC_FF_FIR_LMS_CMD_RETURN_RES;
#if defined(APP_MCPP_CLI)
    interact_data.user = user;
    interact_data.sub_cmd = cmd;

    memcpy(interact_data.buf, res, sizeof(EventDetectionRes));
    interact_data.len = sizeof(EventDetectionRes);

    mcpp_capture_ctl_cmd_send_handler_done(APP_MCPP_USER_VOICE_ASSIST_FIR_LMS, &interact_data, sizeof(interact_data));
#elif defined(FIR_ADAPT_ANC_M55)
    anc_assist_dsp_send_result_to_bth(user, (uint8_t *)res, sizeof(EventDetectionRes), (uint8_t)cmd);
#endif
    return 0;
}

static int32_t _voice_assist_anc_set_cfg_handler(app_anc_mode_t mode, enum ANC_TYPE_T anc_type, ANC_GAIN_TIME anc_gain_delay)
{
    static voice_assist_anc_cfg iir_cfg;

    uint8_t user = _index;
    uint8_t cmd = ANC_FF_FIR_LMS_CMD_SET_CFG;

    iir_cfg.mode = mode;
    iir_cfg.type = anc_type;
    iir_cfg.gain_delay = anc_gain_delay;

    // TRACE(0, "[%s] sub_cmd = %d", __FUNCTION__, cmd);
#if defined(APP_MCPP_CLI)
    interact_data.user = user;
    interact_data.sub_cmd = cmd;

    memcpy(interact_data.buf, &iir_cfg, sizeof(voice_assist_anc_cfg));
    interact_data.len = sizeof(voice_assist_anc_cfg);

    mcpp_capture_ctl_cmd_send_handler_done(APP_MCPP_USER_VOICE_ASSIST_FIR_LMS, &interact_data, sizeof(interact_data));
#elif defined(FIR_ADAPT_ANC_M55)
    anc_assist_dsp_send_result_to_bth(user, (uint8_t *)&iir_cfg, sizeof(voice_assist_anc_cfg), (uint8_t)cmd);
#endif
    return 0;
}

static int32_t _voice_assist_set_iir_handler(enum ANC_TYPE_T anc_type, uint32_t index, uint8_t *coeffs, int32_t len)
{
    static voice_assist_anc_iir_cfg iir_cfg;

    uint8_t user = _index;
    uint8_t cmd = ANC_FF_FIR_LMS_CMD_SET_IIR;

    iir_cfg.type = anc_type;
    iir_cfg.index = index;
    memcpy(iir_cfg.coeffs, coeffs, len);
    iir_cfg.coeffs_len = len;

#if defined(APP_MCPP_CLI)
    interact_data.user = user;
    interact_data.sub_cmd = cmd;

    memcpy(interact_data.buf, &iir_cfg, sizeof(voice_assist_anc_iir_cfg));
    interact_data.len = sizeof(voice_assist_anc_iir_cfg);

    mcpp_capture_ctl_cmd_send_handler_done(APP_MCPP_USER_VOICE_ASSIST_FIR_LMS, &interact_data, sizeof(interact_data));
#elif defined(FIR_ADAPT_ANC_M55)
    anc_assist_dsp_send_result_to_bth(user, (uint8_t *)&iir_cfg, sizeof(voice_assist_anc_iir_cfg), (uint8_t)cmd);
#endif
    return 0;
}
#endif


static void voice_assist_fir_lms_adapt_iir_create(uint8_t *fir_heap)
{
    fir_ext_heap = fir_heap;
    ext_heap_init(fir_ext_heap);
    static FIR_LMS_CALIB_GAIN calib_gain[FIR_CHANNEL_NUM];
    app_anc_get_calib_gain(calib_gain);

    for (uint32_t i = 0; i < FIR_CHANNEL_NUM; i++) {
        fir_st[i] = anc_ff_iir_lms_create(FIR_CAPTURE_SAMPLE_RATE, FIR_CAPTURE_BLOCK_SIZE, &bth_cfg ,&ext_allocator, &calib_gain[i], &cache_cfg[i]);//144604
        anc_ff_iir_lms_register_set_fir_freq_handler(fir_st[i], _voice_assist_set_freq);
        anc_ff_iir_lms_register_set_iir_handler(fir_st[i], fir_lms_anc_set_iir);
        anc_ff_iir_lms_register_convergence_handler(fir_st[i],_voice_assist_ff_iir_convergence);

        voice_assist_fir_lms_inst *ctx = voice_assist_get_ctx();
        voice_assist_iir_lms_update_cfg(ctx);
        if (ctx->fir_enable & ctx->fir_status) {
            app_voice_assist_fir_lms_ctrl(_index, ANC_FF_FIR_LMS_CTRL_SET_FF_FIR_ON, NULL, 0);
        } else {
            app_voice_assist_fir_lms_ctrl(_index, ANC_FF_FIR_LMS_CTRL_SET_FF_FIR_OFF, NULL, 0);
        }
    // fir_coeff_cache = fir_lms_coeff_cache(fir_st);
        TRACE(1,"[%s] buff used %d",__func__, ext_heap_get_used_buff_size());// 158400 - 6164 = 152236(149k)
    }

    voice_assist_fir_lms_inst *ctx = voice_assist_get_ctx();
    for (uint32_t i = 0; i < FIR_CHANNEL_NUM; i++) {
        ctx->anc_inst[i] = fir_st[i];
    }
}

static void voice_assist_fir_lms_adapt_iir_destory(void)
{
    voice_assist_fir_lms_inst *ctx = voice_assist_get_ctx();
    TRACE(0,"fir_st[0] %p",fir_st[0]);
    for (uint32_t i = 0; i < FIR_CHANNEL_NUM; i++) {
        if (fir_st[i]) {
            anc_ff_iir_lms_destroy(fir_st[i]);
            fir_st[i] = NULL;
            ctx->anc_inst[i] = NULL;
        }
    }


    if (fir_ext_heap) {
        ext_heap_deinit();
        fir_ext_heap = NULL;
    }

}

int voice_assist_iir_lms_open(void *share_list_buf)
{
    uint32_t **share_list = (uint32_t **)(share_list_buf);

    static FIR_LMS_CALIB_GAIN calib_gain[FIR_CHANNEL_NUM];
    memcpy(&calib_gain, share_list[0], sizeof(FIR_LMS_CALIB_GAIN)*FIR_CHANNEL_NUM);

    // memcpy(&cfg, share_list[1], sizeof(ANC_FF_IIR_LMS_CFG_T));
    voice_assist_fir_lms_inst *ctx = voice_assist_get_ctx();

    POSSIBLY_UNUSED uint16_t frame_size = FIR_CAPTURE_BLOCK_SIZE;
    POSSIBLY_UNUSED uint16_t blocks = 1;
    uint32_t adaptive_on = (share_list[2] != NULL);

#if defined(RTOS)
    _set_freq_state = SET_FREQ_NONE;
    _set_freq_create_lock();
#endif

#ifdef FIR_LMS_AEC_ENABLED
	anc_aec_heap_init();
    aec_st = anc_fir_lms_aec_init(FIR_CAPTURE_SAMPLE_RATE, FIR_CAPTURE_BLOCK_SIZE, anc_aec_allocator());
#endif

    ext_heap_init_event(fir_ext_heap_event);
    event_st = event_detection_create(FIR_CAPTURE_SAMPLE_RATE, FIR_CAPTURE_BLOCK_SIZE, &bth_cfg.event_cfg, &ext_allocator_event,&event_res);
    TRACE(1,"event_st buff used %d",ext_heap_event_get_used_buff_size());//33644 = 33K

#if defined(APP_MCPP_CLI) && defined(VOICE_ASSIST_FF_FIR_LMS_MULTI_THREAD)
    event_detection_thread_init();
    fir_lms_thread_init();
#endif

    if (adaptive_on) {
        voice_assist_fir_lms_adapt_iir_create((uint8_t *)share_list[2]);
    }

    ctx->frame_count = 0;
    ctx->mips_needed = 40;
    ctx->ctrl = 0;
    memcpy(&ctx->calib_gain, calib_gain, sizeof(FIR_LMS_CALIB_GAIN)*FIR_CHANNEL_NUM);
#ifdef VOICE_FIR_LMS_DUMP_REMOTE
    audio_dump_init(FIR_CAPTURE_BLOCK_SIZE, sizeof(short), 3);
#endif
    return 0;
}

static void voice_assist_event_detection_close(void)
{
    TRACE(0,"[%s]...",__func__);
    event_detection_destroy(event_st);
    event_st = NULL;
}

int voice_assist_iir_lms_close(void)
{
    TRACE(0, "[%s] fir lms deinit...", __func__);
#ifdef FIR_LMS_AEC_ENABLED
    if (aec_st) {
        anc_fir_lms_aec_destroy(aec_st);
        aec_st = NULL;
    }
    anc_aec_heap_deinit();
#endif

    voice_assist_event_detection_close();
    ext_heap_deinit_event();

    voice_assist_fir_lms_adapt_iir_destory();

    return 0;
}

int voice_assist_iir_lms_reset(void)
{
    TRACE(0, "[%s] fir lms reset...", __func__);
    for (uint32_t i = 0; i < FIR_CHANNEL_NUM; i++) {
        anc_ff_iir_lms_reset(fir_st[i], 0, AUD_CHANNEL_MAP_CH0 << i);
    }
    return 0;
}

int voice_assist_iir_lms_get_freq(void)
{
    voice_assist_fir_lms_inst *ctx = voice_assist_get_ctx();

    return ctx->mips_needed;
}

static void _voice_assist_set_freq(ANC_FF_FIR_LMS_FREQ freq, int line)
{
#if defined(RTOS)
    _set_freq_lock();
    voice_assist_fir_lms_inst *ctx = voice_assist_get_ctx();
    uint32_t mode = app_anc_table_get_custom_mode(ctx->mode);
    if (ANC_FF_FIR_LMS_FREQ_NONE == freq) {
        app_sysfreq_req(APP_SYSFREQ_USER_FIR_LMS, APP_SYSFREQ_32K);
        _set_freq_state = ANC_FF_FIR_LMS_FREQ_NONE;
    }
    if (mode == ANC_CUSTOM_MODE_FIR_ANC || mode == ANC_CUSTOM_MODE_ANC) {
        if (mode == ANC_CUSTOM_MODE_FIR_ANC && freq == ANC_FF_FIR_LMS_FREQ_LOW) {
            freq = ANC_FF_FIR_LMS_FREQ_MID;
        }
        if (ANC_FF_FIR_LMS_FREQ_LOW == freq) {
            if (SET_FREQ_FIR_LOW == _set_freq_state) {
                _set_freq_unlock();
                return;
            }
            app_sysfreq_req(APP_SYSFREQ_USER_FIR_LMS, FREQ_FIR_LOW);
            _set_freq_state = SET_FREQ_FIR_LOW;
        } else if (ANC_FF_FIR_LMS_FREQ_MID == freq) {
            if (SET_FREQ_FIR_MID == _set_freq_state) {
                _set_freq_unlock();
                return;
            }

            app_sysfreq_req(APP_SYSFREQ_USER_FIR_LMS, FREQ_FIR_MID);
            _set_freq_state = SET_FREQ_FIR_MID;
        } else if (ANC_FF_FIR_LMS_FREQ_HIGH == freq) {
            if (SET_FREQ_FIR_HIGH == _set_freq_state) {
                _set_freq_unlock();
                return;
            }
            app_sysfreq_req(APP_SYSFREQ_USER_FIR_LMS, FREQ_FIR_HIGH);

            _set_freq_state = SET_FREQ_FIR_HIGH;
        }
    } else {
        if (ANC_FF_FIR_LMS_FREQ_LOW == freq) {
            if (SET_FREQ_TT_LOW == _set_freq_state) {
                _set_freq_unlock();
                return;
            }

            app_sysfreq_req(APP_SYSFREQ_USER_FIR_LMS, FREQ_FIR_LOW);

            _set_freq_state = SET_FREQ_TT_LOW;
        } else if (ANC_FF_FIR_LMS_FREQ_HIGH == freq) {
            if (SET_FREQ_TT_HIGH == _set_freq_state) {
                _set_freq_unlock();
                return;
            }
            app_sysfreq_req(APP_SYSFREQ_USER_FIR_LMS, FREQ_FIR_HIGH);

            _set_freq_state = SET_FREQ_TT_HIGH;
        }
    }
    _set_freq_unlock();
    TRACE(4, "[%s]:mode:%d,%d,%d", __func__, mode, freq, line);
#endif
}

int voice_assist_iir_lms_set_cfg(uint32_t ctrl, uint8_t *tgt_cfg, uint32_t ptr_len)
{
    TRACE(0, "[%s] ctrl %d", __func__, ctrl);

    switch (ctrl) {
        case ANC_FF_FIR_LMS_CTRL_SET_ANC_MODE:
        {
            voice_assist_fir_lms_inst *ctx = voice_assist_get_ctx();
            app_anc_mode_t *mode = (app_anc_mode_t *)tgt_cfg;
            ctx->mode = *mode;
            TRACE(0, "[%s] mode = %d", __FUNCTION__, ctx->mode);
            if (event_st){
                event_detection_set_anc_on_flag(event_st, true);
                event_detection_reset(event_st, &event_res);
            }
#if defined(APP_MCPP_CLI)
            //if switch anc mode, init fir again.
            anc_fir_coeff_config_init();
#endif
            for (uint32_t i = 0; i < FIR_CHANNEL_NUM; i++) {
                anc_ff_iir_lms_clear_ada_iir_cache(fir_st[i],&cache_cfg[i]);
            }
            break;
        }
        case ANC_FF_FIR_LMS_CTRL_SET_FIR_FLAG:
        {
            uint32_t *flag = (uint32_t *)tgt_cfg;
            if (event_st){
                event_detection_set_fir_flag(event_st, *flag);
            }
            break;
        }
        case ANC_FF_FIR_LMS_CTRL_SET_CALIB_GAIN:
        {
            voice_assist_fir_lms_inst *ctx = voice_assist_get_ctx();
            memcpy(&ctx->calib_gain, tgt_cfg, sizeof(FIR_LMS_CALIB_GAIN) * FIR_CHANNEL_NUM);
            ctx->ctrl |= (0x1 << ctrl);
            break;
        }
        case ANC_FF_FIR_LMS_CTRL_SET_ADC_GAIN:
        {
            voice_assist_fir_lms_inst *ctx = voice_assist_get_ctx();
            memcpy(&ctx->adc_gain, tgt_cfg, sizeof(FIR_LMS_CALIB_GAIN) * FIR_CHANNEL_NUM);
            ctx->ctrl |= (0x1 << ctrl);
            break;
        }
        case ANC_FF_FIR_LMS_CTRL_SET_FB_GAIN:
        {
            voice_assist_fir_lms_inst *ctx = voice_assist_get_ctx();
            float *gain = (float *)tgt_cfg;
            ctx->fb_gain = *gain;
            ctx->ctrl |= (0x1 << ctrl);
            break;
        }
        case ANC_FF_FIR_LMS_CTRL_SET_NOISE_STATUS:
        {
            voice_assist_fir_lms_inst *ctx = voice_assist_get_ctx();
            noise_status_t *noise_status = (noise_status_t *)tgt_cfg;
            ctx->noise_status = *noise_status;
            ctx->ctrl |= (0x1 << ctrl);
            break;
        }
        default:
        {
            voice_assist_fir_lms_inst *ctx = voice_assist_get_ctx();
            ctx->ctrl |= (0x1 << ctrl);
            TRACE(0, "[%s] receive ctrl %d", __FUNCTION__, ctrl);
            break;
        }
    }

    return 0;
}


static int voice_assist_iir_lms_update_cfg(voice_assist_fir_lms_inst *ctx)
{
    for (uint32_t i = 0; i < ANC_FF_FIR_LMS_CTRL_NUM; i++) {
        if (ctx->ctrl & (0x1 << i)) {
            TRACE(0, "[%s] process ctrl %d", __FUNCTION__, i);

            switch (i) {
                case ANC_FF_FIR_LMS_CTRL_SET_FF_FIR_ON:
                    for (uint32_t i = 0; i < FIR_CHANNEL_NUM; i++) {
                        if (event_st){
                            anc_ff_iir_lms_set_ff_iir_status(fir_st[i], event_st,true);
                        }
                    }
                    break;
                case ANC_FF_FIR_LMS_CTRL_SET_FF_FIR_OFF:
                    for (uint32_t i = 0; i < FIR_CHANNEL_NUM; i++) {
                        if (event_st){
                            anc_ff_iir_lms_set_ff_iir_status(fir_st[i], event_st, false);
                        }
                    }
                    break;
                case ANC_FF_FIR_LMS_CTRL_START_FIR:
                {
                    for (uint32_t i = 0; i < FIR_CHANNEL_NUM; i++) {
                        anc_ff_iir_lms_reset_ff_FIR(fir_st[i], AUD_CHANNEL_MAP_CH0 << i);
                    }
                    ctx->frame_count = 0;
                    if (event_st){
                        event_detection_set_anc_on_flag(event_st, true);
                    }
                    break;
                }
                case ANC_FF_FIR_LMS_CTRL_SET_CALIB_GAIN:
                {
                    for (uint32_t i = 0; i < FIR_CHANNEL_NUM; i++) {
                        anc_ff_iir_lms_set_ff_iir_calib_gain(fir_st[i], &ctx->calib_gain[i]);
                     }
                    break;
                }
                case ANC_FF_FIR_LMS_CTRL_SET_ADC_GAIN:
                {
                    for (uint32_t i = 0; i < FIR_CHANNEL_NUM; i++) {
                        anc_ff_iir_lms_set_adc_gain(fir_st[i], &ctx->adc_gain[i]);
                    }
                    break;
                }
                case ANC_FF_FIR_LMS_CTRL_SET_FB_GAIN:
                {
                    for (uint32_t i = 0; i < FIR_CHANNEL_NUM; i++) {
                        anc_ff_iir_lms_update_FB_gain(fir_st[i], ctx->fb_gain);
                    }
                    break;
                }
                case ANC_FF_FIR_LMS_CTRL_STOP_FIR:
                {
                    // if reset ANC_FEEDFORWARD which will cause pop noise (fir coeff change abruptly)
                    ctx->frame_count = 0;
                    break;
                }
                case ANC_FF_FIR_LMS_CTRL_SET_NOISE_STATUS:
                {
                    event_detection_set_noise_status(event_st, ctx->noise_status);
                    break;
                }
                default:
                    TRACE(0, "[%s] invalid ctrl %d", __FUNCTION__, i);
                    break;
            }
            ctx->ctrl &= ~(0x1 << i);
        }
    }

    return 0;
}

int voice_assist_iir_lms_process(process_frame_data_t *process_frame_data)
{
#ifdef VOICE_FIR_LMS_DUMP_REMOTE
        audio_dump_clear_up();
        for(int32_t i = 0; i < FIR_CAPTURE_BLOCK_SIZE; i++) {
            tmp_data[i] = (int32_t)process_frame_data->ff_mic[0][i] >> 5;
        }
        audio_dump_add_channel_data(0, tmp_data, FIR_CAPTURE_BLOCK_SIZE);

        for(int32_t i = 0; i < FIR_CAPTURE_BLOCK_SIZE; i++) {
            tmp_data[i] = (int32_t)process_frame_data->fb_mic[0][i] >> 5;
        }
        audio_dump_add_channel_data(1, tmp_data, FIR_CAPTURE_BLOCK_SIZE);

        for(int32_t i = 0; i < FIR_CAPTURE_BLOCK_SIZE; i++) {
            tmp_data[i] = (int32_t)process_frame_data->ref[0][i] >> 5;
        }
        audio_dump_add_channel_data(2, tmp_data, FIR_CAPTURE_BLOCK_SIZE);
        audio_dump_run();
#endif

        // float *mic_data[6] = {ff_mic_buf[0], fb_mic_buf[0], talk_mic_buf[0],ref_mic_buf[0], ref_mic_buf[1], process_frame_data->vpu_mic + offset,};
        // TRACE(0, "[%s] fir lms process...", __func__);
#ifdef FIR_LMS_AEC_ENABLED
    voice_assist_fir_lms_inst *ctx = voice_assist_get_ctx();
    if (app_anc_table_get_custom_mode(ctx->mode) == ANC_CUSTOM_MODE_FIR_ANC && aec_st) {
//     TRACE(0, "[%s] Sys freq: %d", __func__, hal_sys_timer_calc_cpu_freq(5, 0));
// uint32_t start_time = hal_fast_sys_timer_get();
#ifdef ANC_ASSIST_16BIT
        anc_fir_lms_aec_process_q15(aec_st, process_frame_data->fb_mic[0], process_frame_data->ref[0], process_frame_data->fb_mic[0]);
#else
        anc_fir_lms_aec_process_f32(aec_st, process_frame_data->fb_mic[0], process_frame_data->ref[0], process_frame_data->fb_mic[0]);
#endif
//     TRACE(0,"-----aec process---------------%d us",FAST_TICKS_TO_US(hal_fast_sys_timer_get() - start_time));
    }
#endif

    EventController event_control;
    voice_assist_event_detection_process(process_frame_data, &event_control);
        // event_control.update_Pz = 1;
        // event_control.update_Sz = 1;
        // event_control.update_MC = 0;
        // event_control.update_W = 1;
        // event_control.fir_flag = 1;
        // event_control.fir_open_again = 0;

        //mc fir ff fir
    voice_assist_anc_ff_fir_lms_process(process_frame_data, &event_control);

    return 0;
}


#if defined(FIR_ADAPT_ANC_M55)
#include "app_voice_assist_dsp.h"
app_voice_assist_dsp_t voice_assist_fir_lms_m55 = {
    .open          = voice_assist_iir_lms_open,
    .close         = voice_assist_iir_lms_close,
    .reset         = voice_assist_iir_lms_reset,
    .ctrl          = voice_assist_iir_lms_set_cfg,
    .process       = voice_assist_iir_lms_process,
    .get_freq      = voice_assist_iir_lms_get_freq,
};

int32_t assist_fir_lms_dsp_init(void)
{
    TRACE(1, "[%s]fir lms m55 init", __func__);
    anc_assist_dsp_register(_index, &voice_assist_fir_lms_m55, ANC_ASSIST_USER_FS_32K);
    return 0;
}
#endif

void iir_tool_iir_anc_trace(void)
{
    TRACE(0,"%s...",__func__);
    TRACE(0,"debug_en %d",bth_cfg.iir_cfg.debug_en);
    TRACE(0,"max_cnt %d",bth_cfg.iir_cfg.max_cnt);
    TRACE(0,"delta_thresh %de-6",(int)(bth_cfg.iir_cfg.delta_thresh*1000000));
    TRACE(0,"loss_thresh %de-4",(int)(bth_cfg.iir_cfg.loss_thresh*10000));

    TRACE(0,"g_gain_delta %d",(int)(bth_cfg.iir_cfg.g_gain_delta));
    TRACE(0,"g_gain_threshold %d",(int)(bth_cfg.iir_cfg.g_gain_threshold));
    TRACE(0,"gain_delta %de-2",(int)(bth_cfg.iir_cfg.gain_delta*100));
    TRACE(0,"freq_delta %d",(int)(bth_cfg.iir_cfg.freq_delta));
    TRACE(0,"q_delta %de-2",(int)(bth_cfg.iir_cfg.q_delta*100));
    TRACE(0,"mu %de-2",(int)(bth_cfg.iir_cfg.mu*100));

    TRACE(0,"start_freq %d",bth_cfg.iir_cfg.start_freq);
    TRACE(0,"end_freq %d",bth_cfg.iir_cfg.end_freq);
    TRACE(0,"start_filter_idx %d",bth_cfg.iir_cfg.start_filter_idx);
}

//for fir tool
void iir_tool_iir_anc_open(int32_t current_mode, void *cfg)
{
    _iir_tool_debug = 1;
    app_voice_assist_iir_lms_set_iir_status(true);
    memcpy(&iir_cfg, cfg, sizeof(struct_anc_cfg));//init iir coef
    iir_tool_iir_anc_trace();
    for (uint32_t i = 0; i < FIR_CHANNEL_NUM; i++) {
        anc_ff_iir_lms_clear_ada_iir_cache(fir_st[i],&cache_cfg[i]);
    }
    app_voice_assist_iir_lms_open_v1();
}

void iir_tool_iir_anc_close(void)
{
    if (_iir_tool_debug == 0)
        return;

    app_voice_assist_iir_lms_close_v1();
    // app_anc_switch(APP_ANC_MODE_OFF);
    osDelay(500);
    //open anc
    _iir_tool_debug = 0;
}

#endif
