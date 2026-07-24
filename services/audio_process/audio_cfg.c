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
#include "audio_cfg.h"
#include "iir_process.h"
#include "drc.h"
#include "limiter.h"
#include "dynamic_eq.h"
#include "hw_dac_drc.h"
#include "hw_dac_iir_limiter.h"
#include "aud_section.h"
#include "string.h"
#include "hal_trace.h"
#include "speech_eq_cfg.h"
#include "tgt_hardware.h"
#include "norflash_api.h"
#include "aud_section.h"
#include "crc_c.h"
#include "bt_sco_chain_cfg.h"
#include "bass_enhancer.h"
#include "stereo_process.h"

/****************************************************************
sizeof struct:
IIR_CFG_T:652(40)
DrcConfig:92
LimiterConfig:28
hw_dac_limiter_cfg_t:32
DynamicEqConfig:188
struct_psap_cfg: 4136
****************************************************************/
#ifdef AUDIO_CFG_SECTION_ENABLED
/*
 * Config group counts (can be overridden by tgt defines)
 */
#ifndef SW_IIR_NUM
#define SW_IIR_NUM 1
#endif
#ifndef HW_DAC_IIR_NUM
#define HW_DAC_IIR_NUM 1
#endif
#ifndef DRC_NUM
#define DRC_NUM 1
#endif
#ifndef HW_DRC_NUM
#define HW_DRC_NUM 1
#endif
#ifndef LIMITER_NUM
#define LIMITER_NUM 1
#endif
#ifndef HW_DAC_LIMITER_NUM
#define HW_DAC_LIMITER_NUM 1
#endif
#ifndef DYNAMIC_EQ_NUM
#define DYNAMIC_EQ_NUM 1
#endif
#ifndef VIRTUAL_SURROUND_NUM
#define VIRTUAL_SURROUND_NUM 1
#endif
#ifndef BASS_ENHANCER_NUM
#define BASS_ENHANCER_NUM 1
#endif

#define DEFINE_SECTION_ARRAY(TYPE_SUFFIX, VAR_SUFFIX, CFG_T, COUNT_MACRO) \
    typedef struct { \
        audio_section_t section_head; \
        CFG_T           cfg[COUNT_MACRO]; \
    } AUDIO_SECTION_##TYPE_SUFFIX##_CFG_T; \
    static AUDIO_SECTION_##TYPE_SUFFIX##_CFG_T audio_section_##VAR_SUFFIX##_cfg

#ifdef __SW_IIR_EQ_PROCESS__
DEFINE_SECTION_ARRAY(SW_IIR_EQ, sw_iir_eq, IIR_CFG_T, SW_IIR_NUM);
#endif

#ifdef __HW_DAC_IIR_EQ_PROCESS__
DEFINE_SECTION_ARRAY(HW_DAC_IIR_EQ, hw_dac_iir_eq, IIR_CFG_T, HW_DAC_IIR_NUM);
#endif

#ifdef __AUDIO_DRC__
DEFINE_SECTION_ARRAY(DRC, drc, DrcConfig, DRC_NUM);
#endif

#ifdef __AUDIO_LIMITER__
DEFINE_SECTION_ARRAY(LIMITER, limiter, LimiterConfig, LIMITER_NUM);
#endif

#ifdef __HW_DAC_IIR_LIMITER__
DEFINE_SECTION_ARRAY(DAC_LIMITER, dac_limiter, hw_dac_limiter_cfg_t, HW_DAC_LIMITER_NUM);
#endif

#ifdef __AUDIO_DYNAMIC_EQ__
DEFINE_SECTION_ARRAY(DYNAMIC_EQ, dynamic_eq, DynamicEqConfig, DYNAMIC_EQ_NUM);
#endif

#ifdef __HW_DAC_DRC__
DEFINE_SECTION_ARRAY(HW_DRC, hw_drc, struct_psap_cfg, HW_DRC_NUM);
#endif

#ifdef __VIRTUAL_SURROUND_STEREO__
DEFINE_SECTION_ARRAY(VIRTUAL_3D, virtual_3d, VirtualSurroundConfig, VIRTUAL_SURROUND_NUM);
#endif
#ifdef __AUDIO_BASS_ENHANCER__
DEFINE_SECTION_ARRAY(VIRTUALBASS, virtualbass, BassEnhancerConfig, BASS_ENHANCER_NUM);
#endif

static uint8_t audio_process_type_index_map[AUDIO_PROCESS_TYPE_NUM] = {
    [0 ... (AUDIO_PROCESS_TYPE_NUM - 1)] = AUD_SECTION_MAX_USER
};

#define AUDIO_CFG_OK                (0)
#define AUDIO_CFG_ERR_INVALID_TYPE  (-1)
#define AUDIO_CFG_ERR_INVALID_PARAM (-2)
#define AUDIO_CFG_ERR_INDEX_OOB     (-3)
#define AUDIO_CFG_ERR_SIZE_MISMATCH (-4)
#define AUDIO_CFG_ERR_STORE_FAIL    (-5)
#define AUDIO_CFG_ERR_LOAD_FAIL     (-6)

static int32_t audio_cfg_get_section_info(enum AUDIO_PROCESS_TYPE_T type,
                                      uint8_t **section_buf,
                                      uint32_t *section_size,
                                      uint32_t *elem_size,
                                      uint32_t *elem_count)
{
    uint8_t *buf = NULL;
    uint32_t sec_sz = 0, e_sz = 0, e_cnt = 0;

    switch (type) {
#ifdef __SW_IIR_EQ_PROCESS__
    case AUDIO_PROCESS_TYPE_SW_IIR_EQ:
        buf = (uint8_t *)&audio_section_sw_iir_eq_cfg;
        sec_sz = sizeof(AUDIO_SECTION_SW_IIR_EQ_CFG_T);
        e_sz = sizeof(IIR_CFG_T);
        e_cnt = SW_IIR_NUM;
        break;
#endif

#ifdef __HW_DAC_IIR_EQ_PROCESS__
    case AUDIO_PROCESS_TYPE_HW_DAC_IIR_EQ:
        buf = (uint8_t *)&audio_section_hw_dac_iir_eq_cfg;
        sec_sz = sizeof(AUDIO_SECTION_HW_DAC_IIR_EQ_CFG_T);
        e_sz = sizeof(IIR_CFG_T);
        e_cnt = HW_DAC_IIR_NUM;
        break;
#endif

#ifdef __AUDIO_DRC__
    case AUDIO_PROCESS_TYPE_DRC:
        buf = (uint8_t *)&audio_section_drc_cfg;
        sec_sz = sizeof(AUDIO_SECTION_DRC_CFG_T);
        e_sz = sizeof(DrcConfig);
        e_cnt = DRC_NUM;
        break;
#endif

#ifdef __HW_DAC_DRC__
    case AUDIO_PROCESS_TYPE_HW_DRC:
        buf = (uint8_t *)&audio_section_hw_drc_cfg;
        sec_sz = sizeof(AUDIO_SECTION_HW_DRC_CFG_T);
        e_sz = sizeof(struct_psap_cfg);
        e_cnt = HW_DRC_NUM;
        break;
#endif

#ifdef __AUDIO_LIMITER__
    case AUDIO_PROCESS_TYPE_LIMITER:
        buf = (uint8_t *)&audio_section_limiter_cfg;
        sec_sz = sizeof(AUDIO_SECTION_LIMITER_CFG_T);
        e_sz = sizeof(LimiterConfig);
        e_cnt = LIMITER_NUM;
        break;
#endif

#ifdef __HW_DAC_IIR_LIMITER__
    case AUDIO_PROCESS_TYPE_HW_DAC_LIMITER:
        buf = (uint8_t *)&audio_section_dac_limiter_cfg;
        sec_sz = sizeof(AUDIO_SECTION_DAC_LIMITER_CFG_T);
        e_sz = sizeof(hw_dac_limiter_cfg_t);
        e_cnt = HW_DAC_LIMITER_NUM;
        break;
#endif

#ifdef __AUDIO_DYNAMIC_EQ__
    case AUDIO_PROCESS_TYPE_DYNAMIC_EQ:
        buf = (uint8_t *)&audio_section_dynamic_eq_cfg;
        sec_sz = sizeof(AUDIO_SECTION_DYNAMIC_EQ_CFG_T);
        e_sz = sizeof(DynamicEqConfig);
        e_cnt = DYNAMIC_EQ_NUM;
        break;
#endif

#ifdef __VIRTUAL_SURROUND_STEREO__
    case AUDIO_PROCESS_TYPE_VIRTUAL_3D:
        buf = (uint8_t *)&audio_section_virtual_3d_cfg;
        sec_sz = sizeof(AUDIO_SECTION_VIRTUAL_3D_CFG_T);
        e_sz = sizeof(VirtualSurroundConfig);
        e_cnt = VIRTUAL_SURROUND_NUM;
        break;
#endif

#ifdef __AUDIO_BASS_ENHANCER__
    case AUDIO_PROCESS_TYPE_VIRTUALBASS:
        buf = (uint8_t *)&audio_section_virtualbass_cfg;
        sec_sz = sizeof(AUDIO_SECTION_VIRTUALBASS_CFG_T);
        e_sz = sizeof(BassEnhancerConfig);
        e_cnt = BASS_ENHANCER_NUM;
        break;
#endif

    default:
        return AUDIO_CFG_ERR_INVALID_TYPE;
    }

    if (section_buf) *section_buf = buf;
    if (section_size) *section_size = sec_sz;
    if (elem_size) *elem_size = e_sz;
    if (elem_count) *elem_count = e_cnt;
    return AUDIO_CFG_OK;
}

void audio_cfg_section_init(void)
{
    AUDIO_PROCESS_TRACE(2,"[%s] ..", __func__);

    audio_section_nv_register();

#define REGISTER_SECTION(TYPE_ENUM, TYPE_SUFFIX) \
    do { \
        audio_process_type_index_map[TYPE_ENUM] = audio_section_register_user(sizeof(AUDIO_SECTION_##TYPE_SUFFIX##_CFG_T)); \
    } while(0)

#ifdef __SW_IIR_EQ_PROCESS__
    REGISTER_SECTION(AUDIO_PROCESS_TYPE_SW_IIR_EQ, SW_IIR_EQ);
#endif
#ifdef __HW_DAC_IIR_EQ_PROCESS__
    REGISTER_SECTION(AUDIO_PROCESS_TYPE_HW_DAC_IIR_EQ, HW_DAC_IIR_EQ);
#endif
#ifdef __AUDIO_DRC__
    REGISTER_SECTION(AUDIO_PROCESS_TYPE_DRC, DRC);
#endif
#ifdef __HW_DAC_DRC__
    REGISTER_SECTION(AUDIO_PROCESS_TYPE_HW_DRC, HW_DRC);
#endif
#ifdef __AUDIO_LIMITER__
    REGISTER_SECTION(AUDIO_PROCESS_TYPE_LIMITER, LIMITER);
#endif
#ifdef __HW_DAC_IIR_LIMITER__
    REGISTER_SECTION(AUDIO_PROCESS_TYPE_HW_DAC_LIMITER, DAC_LIMITER);
#endif
#ifdef __AUDIO_DYNAMIC_EQ__
    REGISTER_SECTION(AUDIO_PROCESS_TYPE_DYNAMIC_EQ, DYNAMIC_EQ);
#endif
#ifdef __VIRTUAL_SURROUND_STEREO__
    REGISTER_SECTION(AUDIO_PROCESS_TYPE_VIRTUAL_3D, VIRTUAL_3D);
#endif
#ifdef __AUDIO_BASS_ENHANCER__
    REGISTER_SECTION(AUDIO_PROCESS_TYPE_VIRTUALBASS, VIRTUALBASS);
#endif
}

int32_t store_audio_cfg_into_audio_section_by_index(enum AUDIO_PROCESS_TYPE_T type,
                                               uint8_t index,
                                               const uint8_t *cfg,
                                               uint32_t cfg_len)
{
    uint8_t *section_buf = NULL;
    uint32_t section_size = 0, elem_size = 0, elem_count = 0;
    int32_t r = audio_cfg_get_section_info(type, &section_buf, &section_size, &elem_size, &elem_count);
    if (r != AUDIO_CFG_OK) {
        AUDIO_PROCESS_TRACE(2,"[%s] ERROR: invalid type %d", __func__, type);
        return AUDIO_CFG_ERR_INVALID_TYPE;
    }
    if (!cfg || cfg_len == 0) {
        AUDIO_PROCESS_TRACE(2,"[%s] ERROR: null cfg or zero len", __func__);
        return AUDIO_CFG_ERR_INVALID_PARAM;
    }
    if (cfg_len != elem_size) {
        AUDIO_PROCESS_TRACE(2,"[%s] ERROR: size mismatch %u != %u", __func__, cfg_len, elem_size);
        return AUDIO_CFG_ERR_SIZE_MISMATCH;
    }
    if (index >= elem_count) {
        AUDIO_PROCESS_TRACE(2,"[%s] ERROR: index(%d) out of range(%d)", __func__, index, elem_count);
        return AUDIO_CFG_ERR_INDEX_OOB;
    }

    /* Load existing section to preserve header and other elements */
    // r = audio_section_load_user_cfg(audio_process_type_index_map[type], section_buf, section_size);
    // if (r) {
    //     AUDIO_PROCESS_TRACE(2,"[%s] ERROR: load res = %d", __func__, r);
    //     return AUDIO_CFG_ERR_LOAD_FAIL;
    // }

    /* Write into indexed element */
    memcpy(section_buf + AUDIO_SECTION_CFG_RESERVED_LEN + (elem_size * index), cfg, elem_size);

    r = audio_section_store_user_cfg(audio_process_type_index_map[type], section_buf, section_size);
    if (r) {
        AUDIO_PROCESS_TRACE(2,"[%s] ERROR: store res = %d", __func__, r);
        return AUDIO_CFG_ERR_STORE_FAIL;
    }
    AUDIO_PROCESS_TRACE(1,"[%s] Stored type %d, index %d", __func__, type, index);
    return AUDIO_CFG_OK;
}

int32_t store_audio_cfg_into_audio_section_all(enum AUDIO_PROCESS_TYPE_T type,
                                           const uint8_t *cfg,
                                           uint32_t cfg_len)
{
    uint8_t *section_buf = NULL;
    uint32_t section_size = 0, elem_size = 0, elem_count = 0;
    int32_t r = audio_cfg_get_section_info(type, &section_buf, &section_size, &elem_size, &elem_count);
    if (r != AUDIO_CFG_OK) {
        AUDIO_PROCESS_TRACE(2,"[%s] ERROR: invalid type %d", __func__, type);
        return AUDIO_CFG_ERR_INVALID_TYPE;
    }
    if (!cfg || cfg_len == 0) {
        AUDIO_PROCESS_TRACE(2,"[%s] ERROR: null cfg or zero len", __func__);
        return AUDIO_CFG_ERR_INVALID_PARAM;
    }
    uint32_t expect = elem_size * elem_count;
    if (cfg_len != expect) {
        AUDIO_PROCESS_TRACE(2,"[%s] ERROR: size mismatch %u != %u", __func__, cfg_len, expect);
        return AUDIO_CFG_ERR_SIZE_MISMATCH;
    }

    /* Load existing section to preserve header */
    // r = audio_section_load_user_cfg(audio_process_type_index_map[type], section_buf, section_size);
    // if (r) {
    //     AUDIO_PROCESS_TRACE(2,"[%s] ERROR: load res = %d", __func__, r);
    //     return AUDIO_CFG_ERR_LOAD_FAIL;
    // }

    /* Overwrite entire cfg array portion */
    memcpy(section_buf + AUDIO_SECTION_CFG_RESERVED_LEN, cfg, expect);

    r = audio_section_store_user_cfg(audio_process_type_index_map[type], section_buf, section_size);
    if (r) {
        AUDIO_PROCESS_TRACE(2,"[%s] ERROR: store res = %d", __func__, r);
        return AUDIO_CFG_ERR_STORE_FAIL;
    }
    AUDIO_PROCESS_TRACE(1,"[%s] Stored ALL cfgs for type %d", __func__, type);
    return AUDIO_CFG_OK;
}

int32_t store_audio_cfg_into_audio_section(enum AUDIO_PROCESS_TYPE_T type, uint8_t *cfg)
{
    uint8_t *section_buf = NULL;
    uint32_t section_size = 0, elem_size = 0, elem_count = 0;
    int32_t r = audio_cfg_get_section_info(type, &section_buf, &section_size, &elem_size, &elem_count);
    if (r != AUDIO_CFG_OK) {
        AUDIO_PROCESS_TRACE(2,"[%s] ERROR: invalid type %d", __func__, type);
        return AUDIO_CFG_ERR_INVALID_TYPE;
    }
    return store_audio_cfg_into_audio_section_by_index(type, 0, cfg, elem_size);
}

void *load_audio_cfg_and_len_from_audio_section(enum AUDIO_PROCESS_TYPE_T type, uint8_t index, uint32_t *cfg_len)
{
    uint8_t *section_buf = NULL;
    uint32_t section_size = 0, elem_size = 0, elem_count = 0;
    void *res_ptr = NULL;

    ASSERT(type < AUDIO_PROCESS_TYPE_NUM, "[%s] type(%d) is invalid", __func__, type);

    int32_t r = audio_cfg_get_section_info(type, &section_buf, &section_size, &elem_size, &elem_count);
    if (r != AUDIO_CFG_OK) {
        AUDIO_PROCESS_TRACE(2,"[%s] ERROR: invalid type %d", __func__, type);
        return NULL;
    }

    r = audio_section_load_user_cfg(audio_process_type_index_map[type], section_buf, section_size);
    if (r) {
        AUDIO_PROCESS_TRACE(2,"[%s] ERROR: load res = %d", __func__, r);
        return NULL;
    }

    if (index >= elem_count) {
        AUDIO_PROCESS_TRACE(2,"[%s] ERROR: index(%d) out of range(%d)", __func__, index, elem_count);
        return NULL;
    }

    *cfg_len = elem_size;
    res_ptr = (section_buf + AUDIO_SECTION_CFG_RESERVED_LEN + (elem_size * index));
    return res_ptr; /* returning first element (index 0) is also the base of the array */
}

void *load_audio_cfg_from_audio_section(enum AUDIO_PROCESS_TYPE_T type, uint8_t index)
{
    uint32_t len = 0;
    return load_audio_cfg_and_len_from_audio_section(type, index, &len);
}
#else
void audio_cfg_section_init(void) {;}
int store_audio_cfg_into_audio_section(enum AUDIO_PROCESS_TYPE_T type, uint8_t *cfg) {return 0;}
void *load_audio_cfg_from_audio_section(enum AUDIO_PROCESS_TYPE_T type, uint8_t index) {return NULL;}
void *load_audio_cfg_and_len_from_audio_section(enum AUDIO_PROCESS_TYPE_T type, uint8_t index, uint32_t *cfg_len) {return NULL;}
#endif
