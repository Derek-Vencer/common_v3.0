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
#include <stdio.h>
#include "hal_trace.h"
#include "aud_section.h"
#include "crc_c.h"
#include "tgt_hardware.h"
#include "string.h"
#include "hal_timer.h"
#include "hal_norflash.h"
#include "norflash_api.h"
#include "cmsis.h"
#ifdef __ARMCC_VERSION
#include "link_sym_armclang.h"
#endif

// #define AUDIO_SECTION_DEBUG

// #ifdef AUDIO_SECTION_DEBUG
// #define NV_SECTION_TRACE TRACE
// #else
// #define NV_SECTION_TRACE(level,...)
// #endif

typedef struct {
    uint8_t* start_addr;
    uint8_t* end_addr;
    uint32_t sector_size;
    uint32_t block_size;
    uint32_t page_size;
    uint32_t addr_offset;
} audio_section_info_t;

typedef struct {
    uint8_t* user_start_addr;
    uint8_t* user_end_addr;
    uint32_t erase_size;    // multiple of sector size
    uint32_t cfg_len;       // actual size of the configuration data
    bool used;
} audio_section_user_cfg_t;

#ifndef ALIGN_UP
#define ALIGN_UP(num, align) (((num) + ((align) - 1)) & ~((align) - 1))
#endif

static uint32_t g_audio_section_id = 0;
static audio_section_info_t g_audio_section_info = {0};
static audio_section_user_cfg_t g_audio_section_user_cfg[AUD_SECTION_MAX_USER] = {0};

extern uint32_t __aud_start[];
extern uint32_t __aud_end[];
#define _AUD_SECTION_SIZE   ((uint32_t)__aud_end - (uint32_t)__aud_start)

#ifndef ANC_COEF_LIST_NUM
#define ANC_COEF_LIST_NUM                   0
#endif

#define AUD_SECTION_MAGIC       0xdad1

#define USER_MAGIC_NUMBER       0xBE57
#define USER_VERSION            0x0001

static uint8_t audio_section_inited = false;

static uint32_t audio_section_get_user_total_size(void)
{
    return g_audio_section_info.addr_offset;
}

static uint32_t audio_section_get_crc(const pctool_aud_section *audsec_ptr)
{
    uint32_t crc = 0;

    if (audio_section_get_user_total_size() && (_AUD_SECTION_SIZE < audio_section_get_user_total_size() + ANC_SECTION_SIZE)) {
        crc = audsec_ptr->sec_head.crc;
        NV_SECTION_TRACE(1, "[%s] TEST Mode. Bypass crc check", __func__);
    } else {
        crc = crc32_c(0,(unsigned char *)&(audsec_ptr->sec_body),sizeof(audsec_body)-4);
    }

    return crc;
}

void audio_section_callback(void* param)
{
    POSSIBLY_UNUSED NORFLASH_API_OPERA_RESULT *opera_result;
    opera_result = (NORFLASH_API_OPERA_RESULT*)param;

    NV_SECTION_TRACE(6,"%s:type = %d, addr = 0x%x,len = 0x%x,remain = %d,result = %d,suspend_num= %d.",
                __func__,
                opera_result->type,
                opera_result->addr,
                opera_result->len,
                opera_result->remain_num,
                opera_result->result,
                opera_result->suspend_num);
}

static void audio_section_init(void)
{
    if(audio_section_inited)
        return;

#ifndef FPGA
    enum NORFLASH_API_RET_T result;
    enum HAL_FLASH_ID_T flash_id;
    g_audio_section_info.start_addr = (uint8_t *)__aud_start + ANC_SECTION_SIZE;
    g_audio_section_info.end_addr = (uint8_t *)__aud_end;
    NV_SECTION_TRACE(0, "[%s] section_start: %p, section_end: %p", __func__, g_audio_section_info.start_addr, g_audio_section_info.end_addr);

    flash_id = norflash_api_get_dev_id_by_addr((uint32_t)__aud_start);
    hal_norflash_get_size(flash_id,
                          NULL,
                          &g_audio_section_info.block_size,
                          &g_audio_section_info.sector_size,
                          &g_audio_section_info.page_size);

    result = norflash_api_register(NORFLASH_API_MODULE_ID_AUDIO,
                                   flash_id,
                                   ((uint32_t)__aud_start),
                                   ((uint32_t)__aud_end - (uint32_t)__aud_start),
                                   g_audio_section_info.block_size,
                                   g_audio_section_info.sector_size,
                                   g_audio_section_info.page_size,
                                   ((uint32_t)__aud_end - (uint32_t)__aud_start),
                                   audio_section_callback);
    ASSERT(result == NORFLASH_API_OK, "audio_section_init: module register failed! result = %d.", result);

    audio_section_inited = true;
#endif
}

enum NORFLASH_API_RET_T audio_section_erase(uint32_t start_addr, uint32_t len, bool is_async)
{
    //erase audio device[x] section
    enum NORFLASH_API_RET_T ret = 0;
    uint32_t t_size = 0;
    uint32_t b_size = 0;
    uint32_t s_size = 0;
    uint32_t p_size = 0;
    enum HAL_FLASH_ID_T flash_id;

    norflash_api_get_dev_id(NORFLASH_API_MODULE_ID_AUDIO, &flash_id);
    hal_norflash_get_size(flash_id, &t_size, &b_size, &s_size, &p_size);

    // judge start addr and len sector size alignment
    ASSERT(((start_addr & (s_size - 1)) == 0 &&
            (len & (s_size - 1)) == 0),
           "%s: No sec size alignment! start_addr = 0x%x, len = 0x%x",
           __func__,
           start_addr,
           len);

    do {
        ret = norflash_api_erase(NORFLASH_API_MODULE_ID_AUDIO, start_addr,
                                 s_size, is_async);
        if(ret != NORFLASH_API_OK) {
            NV_SECTION_TRACE(3, "%s:offset = 0x%x,ret = %d.", __func__, start_addr, ret);
            return ret;
        }
        start_addr += s_size;
        len -= s_size;
        if (len == 0) {
            break;
        }
    } while (1);

    return ret;
}

void audio_section_pending_op(enum NORFLASH_API_MODULE_ID_T module,
                              enum NORFLASH_API_OPRATION_TYPE type)
{
    do {
        hal_trace_pause();
        norflash_api_flush();
        hal_trace_continue();

        if (NORFLASH_API_ALL != type) {
            if (0 == norflash_api_get_used_buffer_count(module, type)) {
                break;
            }
        } else {
            if (norflash_api_buffer_is_free(module)) {
                break;
            }
        }
        hal_sys_timer_delay(MS_TO_HWTICKS(10));
    } while (1);

    // NV_SECTION_TRACE(1,"%s: module:%d, type:%d done", __func__, module, type);
}

enum NORFLASH_API_RET_T audio_section_write(uint32_t start_addr, uint8_t* ptr, uint32_t len, bool is_async)
{
    enum NORFLASH_API_RET_T ret = NORFLASH_API_OK;
    uint32_t sec_num;
    uint32_t write_len;
    uint32_t written_len = 0;
    uint32_t t_size = 0;
    uint32_t b_size = 0;
    uint32_t s_size = 0;
    uint32_t p_size = 0;
    enum HAL_FLASH_ID_T flash_id;

    norflash_api_get_dev_id(NORFLASH_API_MODULE_ID_AUDIO, &flash_id);
    hal_norflash_get_size(flash_id, &t_size, &b_size, &s_size, &p_size);

    // judge start addr and len sector size alignment
    ASSERT((start_addr & (s_size - 1)) == 0,
           "%s: No sec size alignment! start_addr = 0x%x",
           __func__,
           start_addr);

    NV_SECTION_TRACE(1,"%s: write: 0x%x,0x%x", __func__, start_addr, len);
    sec_num = (len + s_size - 1) / s_size;
    for (int i = 0; i < sec_num; i++) {
        if (len - written_len > s_size) {
            write_len = s_size;
        } else {
            write_len = len - written_len;
        }

        do {
            ret = norflash_api_write(NORFLASH_API_MODULE_ID_AUDIO,
                                     start_addr + written_len,
                                     ptr + written_len,
                                     write_len,
                                     is_async);

            if (NORFLASH_API_OK == ret)
            {
                NV_SECTION_TRACE(1, "%s: norflash_api_write ok!", __func__);
                written_len += write_len;
                break;
            }
            else if (NORFLASH_API_BUFFER_FULL == ret)
            {
                NV_SECTION_TRACE(1, "%s: buffer full! To flush it.", __func__);
                audio_section_pending_op(NORFLASH_API_MODULE_ID_AUDIO, NORFLASH_API_WRITTING);
            } else {
                ASSERT(0, "%s: norflash_api_write failed. ret = %d", __FUNCTION__, ret);
            }
        } while (1);
    }
    // NV_SECTION_TRACE(1,"%s: write: 0x%x,0x%x done.", __func__, start_addr, len);
    return NORFLASH_API_OK;
}

int anccfg_loadfrom_audsec(const struct_anc_cfg *list[], const struct_anc_cfg *list_44p1k[], uint32_t count)
{
#ifdef PROGRAMMER

    return 1;

#else // !PROGRAMMER

#ifdef CHIP_BEST1000
    ASSERT(0, "[%s] Can not support anc load in this branch!!!", __func__);
#else
    unsigned int i;
    const pctool_aud_section *audsec_ptr;

    NV_SECTION_TRACE(1,"%s:PCTOOL_AUDSEC_RESERVED_LEN:%d", __func__,PCTOOL_AUDSEC_RESERVED_LEN);
    NV_SECTION_TRACE(1,"%s:sizeof(pctool_anc_config_t):%d", __func__,sizeof(pctool_anc_config_t));

    norflash_api_flash_operation_start((uint32_t) __aud_start);
    audsec_ptr = (pctool_aud_section *)__aud_start;
    NV_SECTION_TRACE(3,"0x%x,0x%x,0x%x",audsec_ptr->sec_head.magic,audsec_ptr->sec_head.version,audsec_ptr->sec_head.crc);
    if (audsec_ptr->sec_head.magic != AUD_SECTION_MAGIC) {
        norflash_api_flash_operation_end((uint32_t) __aud_start);
        NV_SECTION_TRACE(0,"Invalid aud section - magic");
        return 1;
    }
    if (audio_section_get_crc(audsec_ptr) != audsec_ptr->sec_head.crc){
        norflash_api_flash_operation_end((uint32_t) __aud_start);
        NV_SECTION_TRACE(0,"crc verify failure, invalid aud section.");
        return 1;
    }
    NV_SECTION_TRACE(0,"Valid aud section.");
    for (i = 0; i < ANC_COEF_LIST_NUM; i++)
        list[i] = (struct_anc_cfg *)&(audsec_ptr->sec_body.anc_config.anc_config_arr[i].anc_cfg[PCTOOL_SAMPLERATE_48X8K]);
    for (i = 0; i < ANC_COEF_LIST_NUM; i++)
        list_44p1k[i] = (struct_anc_cfg *)&(audsec_ptr->sec_body.anc_config.anc_config_arr[i].anc_cfg[PCTOOL_SAMPLERATE_44_1X8K]);
    norflash_api_flash_operation_end((uint32_t) __aud_start);
#endif

    return 0;

#endif // !PROGRAMMER
}


#if defined(PSAP_APP)
int psapcfg_loadfrom_audsec(const struct_psap_cfg *list[], const struct_psap_cfg *list_44p1k[], uint32_t count)
{
#ifdef PROGRAMMER

    return 1;

#else // !PROGRAMMER

    unsigned int i;
    const pctool_aud_section *audsec_ptr;

    NV_SECTION_TRACE(1,"%s", __func__);

    norflash_api_flash_operation_start((uint32_t) __aud_start);
    audsec_ptr = (pctool_aud_section *)__aud_start;
    NV_SECTION_TRACE(3,"0x%x,0x%x,0x%x",audsec_ptr->sec_head.magic,audsec_ptr->sec_head.version,audsec_ptr->sec_head.crc);
    if (audsec_ptr->sec_head.magic != AUD_SECTION_MAGIC) {
        norflash_api_flash_operation_end((uint32_t) __aud_start);
        NV_SECTION_TRACE(0,"Invalid aud section - magic");
        return 1;
    }
    if (audio_section_get_crc(audsec_ptr) != audsec_ptr->sec_head.crc) {
        norflash_api_flash_operation_end((uint32_t) __aud_start);
        NV_SECTION_TRACE(0,"crc verify failure, invalid aud section.");
        return 1;
    }
    NV_SECTION_TRACE(0,"Valid aud section.");
    for (i = 0; i < PSAP_COEF_LIST_NUM; i++)
        list[i] = (struct_psap_cfg *)&(audsec_ptr->sec_body.psap_config.psap_config_arr[i].psap_cfg[PCTOOL_SAMPLERATE_48X8K]);
    for (i = 0; i < PSAP_COEF_LIST_NUM; i++)
        list_44p1k[i] = (struct_psap_cfg *)&(audsec_ptr->sec_body.psap_config.psap_config_arr[i].psap_cfg[PCTOOL_SAMPLERATE_44_1X8K]);
    norflash_api_flash_operation_end((uint32_t) __aud_start);

    return 0;

#endif // !PROGRAMMER
}
#endif

#if defined(AUDIO_ANC_SPKCALIB_HW)
int spkcalibcfg_loadfrom_audsec(const struct_spkcalib_cfg *list[], const struct_spkcalib_cfg *list_44p1k[], uint32_t count)
{
#ifdef PROGRAMMER

    return 1;

#else // !PROGRAMMER

    unsigned int i;
    const pctool_aud_section *audsec_ptr;

    NV_SECTION_TRACE(1,"%s", __func__);

    norflash_api_flash_operation_start((uint32_t) __aud_start);
    audsec_ptr = (pctool_aud_section *)__aud_start;
    NV_SECTION_TRACE(3,"0x%x,0x%x,0x%x",audsec_ptr->sec_head.magic,audsec_ptr->sec_head.version,audsec_ptr->sec_head.crc);
    if (audsec_ptr->sec_head.magic != AUD_SECTION_MAGIC) {
        norflash_api_flash_operation_end((uint32_t) __aud_start);
        NV_SECTION_TRACE(0,"Invalid aud section - magic");
        return 1;
    }
    if (audio_section_get_crc(audsec_ptr) != audsec_ptr->sec_head.crc) {
        norflash_api_flash_operation_end((uint32_t) __aud_start);
        NV_SECTION_TRACE(0,"crc verify failure, invalid aud section.");
        return 1;
    }
    NV_SECTION_TRACE(0,"Valid aud section.");
    for (i = 0; i < SPKCALIB_COEF_LIST_NUM; i++)
        list[i] = (struct_spkcalib_cfg *)&(audsec_ptr->sec_body.spkcalib_config.spkcalib_config_arr[i].spkcalib_cfg[PCTOOL_SAMPLERATE_48X8K]);
    for(i = 0; i < SPKCALIB_COEF_LIST_NUM; i++)
        list_44p1k[i] = (struct_spkcalib_cfg *)&(audsec_ptr->sec_body.spkcalib_config.spkcalib_config_arr[i].spkcalib_cfg[PCTOOL_SAMPLERATE_44_1X8K]);
    norflash_api_flash_operation_end((uint32_t) __aud_start);

    return 0;

#endif // !PROGRAMMER
}
#endif

void audio_section_nv_register(void)
{
    audio_section_init();
}

int anc_section_nv_erase(uint32_t start_addr, uint32_t len, bool is_async)
{
    enum NORFLASH_API_RET_T ret = 0;
    uint32_t t_size = 0;
    uint32_t b_size = 0;
    uint32_t s_size = 0;
    uint32_t p_size = 0;
    enum HAL_FLASH_ID_T flash_id;

    norflash_api_get_dev_id(NORFLASH_API_MODULE_ID_ANC, &flash_id);
    hal_norflash_get_size(flash_id, &t_size, &b_size, &s_size, &p_size);

    len = ((len + (s_size-1))/s_size) * s_size;

    // judge start addr and len sector size alignment
    ASSERT(((start_addr & (s_size - 1)) == 0 &&
            (len & (s_size - 1)) == 0),
           "%s: No sec size alignment! start_addr = 0x%x, len = 0x%x",
           __func__,
           start_addr,
           len);

    do
    {
        if (len == 0)
        {
            break;
        }
        ret = norflash_api_erase(NORFLASH_API_MODULE_ID_ANC, start_addr,
                                 s_size, is_async);
        if(ret != NORFLASH_API_OK)
        {
            NV_SECTION_TRACE(3, "%s:offset = 0x%x,ret = %d.", __func__, start_addr, ret);
            return ret;
        }
        start_addr += s_size;
        if(len >= s_size)
            len -= s_size;
    } while (1);

    return (int)ret;
}


int anc_section_nv_write(uint32_t start_addr, uint8_t* ptr, uint32_t len, bool is_async)
{
    enum NORFLASH_API_RET_T ret = NORFLASH_API_OK;
    uint32_t sec_num;
    uint32_t write_len;
    uint32_t written_len = 0;
    uint32_t t_size = 0;
    uint32_t b_size = 0;
    uint32_t s_size = 0;
    uint32_t p_size = 0;
    enum HAL_FLASH_ID_T flash_id;

    norflash_api_get_dev_id(NORFLASH_API_MODULE_ID_ANC, &flash_id);
    hal_norflash_get_size(flash_id, &t_size, &b_size, &s_size, &p_size);

    // judge start addr and len sector size alignment
    ASSERT((start_addr & (s_size - 1)) == 0,
           "%s: No sec size alignment! start_addr = 0x%x",
           __func__,
           start_addr);

    NV_SECTION_TRACE(1,"%s: write: 0x%x,0x%x", __func__, start_addr, len);
    sec_num = (len + s_size - 1) / s_size;
    for (int i = 0; i < sec_num; i++)
    {
        if (len - written_len > s_size)
        {
            write_len = s_size;
        }
        else
        {
            write_len = len - written_len;
        }

        do
        {
            ret = norflash_api_write(NORFLASH_API_MODULE_ID_ANC,
                                     start_addr + written_len,
                                     ptr + written_len,
                                     write_len,
                                     is_async);

            if (NORFLASH_API_OK == ret)
            {
                NV_SECTION_TRACE(1, "%s: norflash_api_write ok!", __func__);
                written_len += write_len;
                break;
            }
            else if (NORFLASH_API_BUFFER_FULL == ret)
            {
                NV_SECTION_TRACE(1, "%s: buffer full! To flush it.", __func__);
                audio_section_pending_op(NORFLASH_API_MODULE_ID_ANC, NORFLASH_API_WRITTING);
            }
            else
            {
                ASSERT(0, "%s: norflash_api_write failed. ret = %d", __FUNCTION__, ret);
            }
        } while (1);
    }
    // NV_SECTION_TRACE(1,"%s: write: 0x%x,0x%x done.", __func__, start_addr, len);
    return 0;
}

int audio_section_nv_erase(uint32_t start_addr, uint32_t len, bool is_async)
{
    enum NORFLASH_API_RET_T ret = 0;
    uint32_t t_size = 0;
    uint32_t b_size = 0;
    uint32_t s_size = 0;
    uint32_t p_size = 0;
    enum HAL_FLASH_ID_T flash_id;

    norflash_api_get_dev_id(NORFLASH_API_MODULE_ID_AUDIO, &flash_id);
    hal_norflash_get_size(flash_id, &t_size, &b_size, &s_size, &p_size);

    len = ((len + (s_size-1))/s_size) * s_size;

    // judge start addr and len sector size alignment
    ASSERT(((start_addr & (s_size - 1)) == 0 &&
            (len & (s_size - 1)) == 0),
           "%s: No sec size alignment! start_addr = 0x%x, len = 0x%x",
           __func__,
           start_addr,
           len);

    do {
        if (len == 0) {
            break;
        }
        ret = norflash_api_erase(NORFLASH_API_MODULE_ID_AUDIO, start_addr,
                                 s_size, is_async);
        if(ret != NORFLASH_API_OK)
        {
            NV_SECTION_TRACE(3, "%s:offset = 0x%x,ret = %d.", __func__, start_addr, ret);
            return ret;
        }
        start_addr += s_size;
        if (len >= s_size)
            len -= s_size;
    } while (1);

    return (int)ret;
}

int audio_section_nv_write(uint32_t start_addr, uint8_t* ptr, uint32_t len, bool is_async)
{
    enum NORFLASH_API_RET_T ret = NORFLASH_API_OK;
    uint32_t sec_num;
    uint32_t write_len;
    uint32_t written_len = 0;
    uint32_t t_size = 0;
    uint32_t b_size = 0;
    uint32_t s_size = 0;
    uint32_t p_size = 0;
    enum HAL_FLASH_ID_T flash_id;

    norflash_api_get_dev_id(NORFLASH_API_MODULE_ID_AUDIO, &flash_id);
    hal_norflash_get_size(flash_id, &t_size, &b_size, &s_size, &p_size);

    // judge start addr and len sector size alignment
    ASSERT((start_addr & (s_size - 1)) == 0,
           "%s: No sec size alignment! start_addr = 0x%x",
           __func__,
           start_addr);

    NV_SECTION_TRACE(1,"%s: write: 0x%x,0x%x", __func__, start_addr, len);
    sec_num = (len + s_size - 1) / s_size;
    for (int i = 0; i < sec_num; i++) {
        if (len - written_len > s_size) {
            write_len = s_size;
        } else {
            write_len = len - written_len;
        }

        do {
            ret = norflash_api_write(NORFLASH_API_MODULE_ID_AUDIO,
                                     start_addr + written_len,
                                     ptr + written_len,
                                     write_len,
                                     is_async);

            if (NORFLASH_API_OK == ret)
            {
                NV_SECTION_TRACE(1, "%s: norflash_api_write ok!", __func__);
                written_len += write_len;
                break;
            }
            else if (NORFLASH_API_BUFFER_FULL == ret)
            {
                NV_SECTION_TRACE(1, "%s: buffer full! To flush it.", __func__);
                audio_section_pending_op(NORFLASH_API_MODULE_ID_AUDIO, NORFLASH_API_WRITTING);
            } else {
                ASSERT(0, "%s: norflash_api_write failed. ret = %d", __FUNCTION__, ret);
            }
        } while (1);
    }
    // NV_SECTION_TRACE(1,"%s: write: 0x%x,0x%x done.", __func__, start_addr, len);
    return 0;
}

int32_t audio_section_register_user(uint32_t cfg_len)
{
    if (!g_audio_section_info.start_addr) {
        return -1;
    }

    for (int i=g_audio_section_id; i<AUD_SECTION_MAX_USER; i++) {
        if (!g_audio_section_user_cfg[i].used) {
            g_audio_section_user_cfg[i].cfg_len = cfg_len;
            g_audio_section_user_cfg[i].erase_size = ALIGN_UP(cfg_len + AUDIO_SECTION_CFG_RESERVED_LEN, g_audio_section_info.sector_size);
            g_audio_section_user_cfg[i].user_start_addr = g_audio_section_info.start_addr + g_audio_section_info.addr_offset;
            NV_SECTION_TRACE(1, "section_start = %p, user_start = %p", g_audio_section_info.start_addr, g_audio_section_user_cfg[i].user_start_addr);
            g_audio_section_info.addr_offset += g_audio_section_user_cfg[i].erase_size;
            g_audio_section_user_cfg[i].user_end_addr = g_audio_section_info.start_addr + g_audio_section_info.addr_offset;
            ASSERT(g_audio_section_user_cfg[i].user_end_addr <= g_audio_section_info.end_addr, "[%s] user_end_addr %p exceeds section end_addr %p", \
                                                                                                __func__, g_audio_section_user_cfg[i].user_end_addr, \
                                                                                                g_audio_section_info.end_addr);
            NV_SECTION_TRACE(1,"********************[%s]********************", __func__);
            NV_SECTION_TRACE(1, "start_addr =       %p ", g_audio_section_user_cfg[i].user_start_addr);
            NV_SECTION_TRACE(1, "end_addr =         %p ", g_audio_section_user_cfg[i].user_end_addr);
            NV_SECTION_TRACE(1, "offset_addr =      0x%x ", g_audio_section_info.addr_offset);
            NV_SECTION_TRACE(1, "user =             %d ", i);
            NV_SECTION_TRACE(1, "cfg_len =          %d ", cfg_len);
            NV_SECTION_TRACE(1,"********************END********************");
            g_audio_section_user_cfg[i].used = true;
            g_audio_section_id = i;
            return i;
        }
    }

    NV_SECTION_TRACE(3,"[%s] WARNING: more than max user %d", __func__, AUD_SECTION_MAX_USER);
    return -1;
}

uint8_t* audio_section_get_user_addr_offset(uint32_t user)
{
    return g_audio_section_user_cfg[user].user_start_addr;
}

int32_t audio_section_store_user_cfg(uint32_t user, uint8_t *cfg, uint32_t len)
{
    audio_section_t *section_ptr = (audio_section_t *)cfg;
    uint32_t addr_start = 0;
    uint32_t crc = 0;
    enum NORFLASH_API_RET_T flash_opt_res;
    bool is_async = false;

    if (!audio_section_inited) {
        NV_SECTION_TRACE(2,"[%s] ERROR: Audio section have not been inited!", __func__);
        return -1;
    }

    if (user >= AUD_SECTION_MAX_USER) {
        NV_SECTION_TRACE(2,"[%s] ERROR: Invalid user!", __func__);
        return -1;
    }

    if (len != g_audio_section_user_cfg[user].cfg_len) {
        NV_SECTION_TRACE(2,"[%s] ERROR: len!", __func__);
        return -1;
    }

    if (!cfg) {
        NV_SECTION_TRACE(2,"[%s] ERROR: cfg is NULL!", __func__);
        return -1;
    }

    addr_start = (uint32_t)audio_section_get_user_addr_offset(user);

    if (addr_start == 0) {
        NV_SECTION_TRACE(2,"[%s] ERROR: Can not get addr", __func__);
        return -1;
    }

    section_ptr->head.magic = USER_MAGIC_NUMBER;
    section_ptr->head.version = USER_VERSION;
    section_ptr->cfg_len = len;
    section_ptr->user = user;

    // calculate crc
    crc = crc32_c(0, (unsigned char *)section_ptr + AUDIO_SECTION_CFG_RESERVED_LEN , len - AUDIO_SECTION_CFG_RESERVED_LEN);
    section_ptr->head.crc = crc;

    NV_SECTION_TRACE(2,"[%s] addr_start = 0x%x", __func__, addr_start);
    NV_SECTION_TRACE(2,"[%s] erase_size = 0x%x", __func__, g_audio_section_user_cfg[user].erase_size);
    NV_SECTION_TRACE(2,"[%s] cfg_len = %d", __func__, len);

    flash_opt_res = audio_section_erase(addr_start, g_audio_section_user_cfg[user].erase_size, is_async);
    if (flash_opt_res) {
        NV_SECTION_TRACE(2,"[%s] ERROR: erase flash res = %d", __func__, flash_opt_res);
        return flash_opt_res;
    }

    flash_opt_res = audio_section_write(addr_start, (uint8_t *)section_ptr, len, is_async);
    if (flash_opt_res) {
        NV_SECTION_TRACE(2,"[%s] ERROR: write flash res = %d", __func__, flash_opt_res);
        return flash_opt_res;
    }

    NV_SECTION_TRACE(1,"********************[%s]********************", __func__);
    NV_SECTION_TRACE(1,"magic:      0x%x", section_ptr->head.magic);
    NV_SECTION_TRACE(1,"version:    0x%x", section_ptr->head.version);
    NV_SECTION_TRACE(1,"crc:        0x%x", section_ptr->head.crc);
    NV_SECTION_TRACE(1,"user:       %d", section_ptr->user);
    NV_SECTION_TRACE(1,"cfg_len:    %d", section_ptr->cfg_len);
    NV_SECTION_TRACE(0,"********************END********************");

    return 0;
}

int32_t audio_section_load_user_cfg(uint32_t user, uint8_t *cfg, uint32_t len)
{
    uint32_t addr_start = (uint32_t)audio_section_get_user_addr_offset(user);
    uint32_t crc = 0;

    if (!audio_section_inited) {
        NV_SECTION_TRACE(2,"[%s] ERROR: Audio section have not been inited!", __func__);
        return -1;
    }

    if (user >= AUD_SECTION_MAX_USER) {
        NV_SECTION_TRACE(2,"[%s] ERROR: Invalid user!", __func__);
        return -1;
    }

    if (len != g_audio_section_user_cfg[user].cfg_len) {
        NV_SECTION_TRACE(2,"[%s] ERROR: len!", __func__);
        return -1;
    }

    if (!cfg) {
        NV_SECTION_TRACE(2,"[%s] ERROR: cfg is NULL!", __func__);
        return -1;
    }

    audio_section_t *section_ptr = (audio_section_t *)(addr_start);

    NV_SECTION_TRACE(0, "[%s] start: %p, end: %p, erase_size: %x, RESERVED: %x", __func__, g_audio_section_user_cfg[user].user_start_addr, \
                                                                                            g_audio_section_user_cfg[user].user_end_addr, \
                                                                                            g_audio_section_user_cfg[user].erase_size, \
                                                                                            AUDIO_SECTION_CFG_RESERVED_LEN);

    NV_SECTION_TRACE(1,"********************[%s]********************", __func__);
    NV_SECTION_TRACE(1,"magic:      0x%x", section_ptr->head.magic);
    NV_SECTION_TRACE(1,"version:    0x%x", section_ptr->head.version);
    NV_SECTION_TRACE(1,"crc:        0x%x", section_ptr->head.crc);
    NV_SECTION_TRACE(1,"user:       %d", section_ptr->user);
    NV_SECTION_TRACE(1,"cfg_len:    %d", section_ptr->cfg_len);
    NV_SECTION_TRACE(0,"********************END********************");

    uint32_t lock = int_lock_global();
    if(section_ptr->head.magic != USER_MAGIC_NUMBER) {
        int_unlock_global(lock);
        NV_SECTION_TRACE(3,"[%s] WARNING: Different magic number (%x != %x)", __func__, section_ptr->head.magic, USER_MAGIC_NUMBER);
        return -1;
    }

    // Calculate crc and check crc value
    crc = crc32_c(0, (unsigned char *)section_ptr + AUDIO_SECTION_CFG_RESERVED_LEN, len - AUDIO_SECTION_CFG_RESERVED_LEN);

    if(section_ptr->head.crc != crc) {
        int_unlock_global(lock);
        NV_SECTION_TRACE(3,"[%s] WARNING: Different crc (%x != %x)", __func__, section_ptr->head.crc, crc);
        return -2;
    }

    if(section_ptr->user != user) {
        int_unlock_global(lock);
        NV_SECTION_TRACE(3,"[%s] WARNING: Different user (%d != %d)", __func__, section_ptr->user, user);
        return -3;
    }

    if(section_ptr->cfg_len != len) {
        int_unlock_global(lock);
        NV_SECTION_TRACE(3,"[%s] WARNING: Different length (%d != %d)", __func__, section_ptr->cfg_len, len);
        return -4;
    }

    memcpy(cfg, section_ptr, len);
    int_unlock_global(lock);

    return 0;
}
