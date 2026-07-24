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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "cmsis_os.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_gpio.h"
#include "hal_sysfreq.h"

#include "heap_api.h"
#include "wav_file.h"
#include "audio_manager.h"
#include "app_wireless_mic.h"
#include "audio_datapath_emmc.h"
#include "audio_datapath_adc.h"
#include "audio_datapath_manager.h"
#include "app_utils.h"
#include "algo_com.h"

#ifdef TX_ALGO_PROCESS
#include "tx_algo.h"
#endif

#ifdef AOS_FS_ENABLE
#include "fs/fs.h"
#endif

#define MIC_EMMC_DUMP_DEBUG                     (0)
#define IS_USE_24BIT_DUMP                       (1)

// warning: EMMC_DUMP_FILE_MAX_SIZE must be divisible by 3, 4, EMMC_CACHE_BUFFER_SIZE.
#define EMMC_DUMP_FILE_MAX_SIZE                 (337488 * 1024)

#define EMMC_MAILBOX_MAX_CNT                    (4)     //(38)
// Write 24K data at a time as recommended by the emmc particle manufacturer, and EMMC_CACHE_BUFFER_SIZE must be divisible by 3, 4.
#define EMMC_CACHE_BUFFER_SIZE                  (4800)  //(24 * 1024)
#define EMMC_HEAP_BUFFER_SIZE                   (EMMC_CACHE_BUFFER_SIZE * EMMC_MAILBOX_MAX_CNT) //896 KBytes

#define EMMC_THREAD_STACK_SIZE                  (1024 * 4)

#define EMMC_INPUT_BUFF_SIZE                    (48 * 4 * AUDIO_CODEC_CAPTURE_STREAM_DMA_INTERVAL_US / 1000)

#ifndef APP_SYSFREQ_USER_APP_EMMC
#define APP_SYSFREQ_USER_APP_EMMC               (APP_SYSFREQ_USER_APP_21)
#endif

osMailQDef (audio_datapath_emmc_mailbox, EMMC_MAILBOX_MAX_CNT, AUDIO_DATAPATH_EMMC_MESSAGE_T);
static osMailQId audio_datapath_emmc_mailbox_id = NULL;

static bool audio_datapath_emmc_capture_stream_started = false;

static bool emmc_audio_dump_enable = false;
static uint8_t audio_datapath_emmc_mode = 0;

static uint32_t emmc_dump_algo_file_size = 0;
static uint32_t emmc_dump_orig_file_size = 0;
static uint32_t emmc_dump_algo_32bit_file_size = 0;

static struct BWF orig_wav_head;
static struct BWF algo_wav_head;
static struct BWF algo_32bit_wav_head;

static uint8_t emmc_heap_start[EMMC_HEAP_BUFFER_SIZE] = {0};
static multi_heap_handle_t emmc_heap = NULL;

static uint8_t* emmc_origin_buff_ptr = NULL;
static uint16_t emmc_origin_buff_len = 0;

static uint8_t* emmc_algo_buff_ptr = NULL;
static uint16_t emmc_algo_buff_len = 0;

static uint8_t* emmc_algo_32bit_buff_ptr = NULL;
static uint16_t emmc_algo_32bit_buff_len = 0;

#if IS_USE_24BIT_DUMP
static uint8_t emmc_audio_input_buff[EMMC_INPUT_BUFF_SIZE] = {0};
#else
static uint8_t* emmc_audio_input_buff = NULL;
#endif

#if 0
static uint64_t emmc_audio_dump_resample_start_time[MIC_DATA_DUMP_PORT_NUM] = {0};
static uint16_t emmc_audio_dump_resample_cycle[MIC_DATA_DUMP_PORT_NUM] = {0};
static uint8_t emmc_audio_dump_port_str[MIC_DATA_DUMP_PORT_NUM][12] = {"origin", "algo", "32bit algo"};
#endif

static const char* fn_path = "data/emmc0";

static void emmc_heap_init(void)
{
    void *start = emmc_heap_start;
    size_t size = EMMC_HEAP_BUFFER_SIZE;

    emmc_heap = multi_heap_register(start, size);
    ASSERT(emmc_heap, "Failed to register d2d heap: start=%p size=0x%08X", start, size);
}

static void *emmc_heap_malloc(size_t size)
{
    return multi_heap_malloc(emmc_heap, size);
}

static void emmc_heap_free(void *p)
{
    multi_heap_free(emmc_heap, p);
}

static void audio_datapath_emmc_mailbox_init(void)
{
    if (audio_datapath_emmc_mailbox_id == NULL) {
        audio_datapath_emmc_mailbox_id = osMailCreate(osMailQ(audio_datapath_emmc_mailbox), NULL);
    }
    ASSERT(audio_datapath_emmc_mailbox_id, "Failed to Create audio_datapath_emmc_mailbox\n");

    emmc_heap_init();

    LOG_MSG(0, "emmc mailbox init done!");
}

int32_t audio_datapath_emmc_mailbox_put(uint16_t u16_dump_port_id, uint8_t* u8_mic_data_ptr, uint32_t u32_msg_param_len, int32_t resample_delta)
{
    AUDIO_DATAPATH_EMMC_MESSAGE_T* msg_p = NULL;

    msg_p = (AUDIO_DATAPATH_EMMC_MESSAGE_T*)osMailAlloc(audio_datapath_emmc_mailbox_id, 0);
    if (!msg_p) {
        LOG_ERR(0, "emmc mailbox alloc failed!");
        return -1;
    }

    msg_p->dump_port_id = u16_dump_port_id;
    msg_p->mic_data_len = u32_msg_param_len;
    msg_p->mic_data_ptr = u8_mic_data_ptr;
    msg_p->mic_resample_delta = resample_delta;

    return osMailPut(audio_datapath_emmc_mailbox_id, msg_p);
}

static int8_t audio_datapath_emmc_mailbox_free(AUDIO_DATAPATH_EMMC_MESSAGE_T* msg_p)
{
    osStatus status;
    if (msg_p->mic_data_len > 0 && msg_p->mic_data_ptr)
    {
        emmc_heap_free(msg_p->mic_data_ptr);
        msg_p->mic_data_ptr = NULL;
        msg_p->mic_data_len = 0;
    }

    status = osMailFree(audio_datapath_emmc_mailbox_id, msg_p);

    return (int)status;
}

static int8_t audio_datapath_emmc_mailbox_get(AUDIO_DATAPATH_EMMC_MESSAGE_T** msg_p)
{
    osEvent evt;

    app_sysfreq_req(APP_SYSFREQ_USER_APP_EMMC, APP_SYSFREQ_32K);
    evt = osMailGet(audio_datapath_emmc_mailbox_id, osWaitForever);
    app_sysfreq_req(APP_SYSFREQ_USER_APP_EMMC, APP_SYSFREQ_104M);

    if (evt.status == osEventMail) {
        *msg_p = (AUDIO_DATAPATH_EMMC_MESSAGE_T *)evt.value.p;
        return 0;
    }
    return -1;
}

static uint32_t audio_datapath_emmc_mailbox_cnt(void)
{
    return osMailGetCount(audio_datapath_emmc_mailbox_id);
}

#if 0
static int8_t audio_datapath_emmc_date_write(AUDIO_DATAPATH_EMMC_MESSAGE_T *msg_p, char* file_suffix)
{
    char fn_path[128] = "data/emmc0/mic_dump";
    char fn_orig[128] = "";
    FILE* fd = NULL;
    static uint32_t cnt = 0;
    cnt++;
    if (cnt % 20 == 1) {
        LOG_MSG(1, "path:%s data to write!, len: %d", file_suffix, msg_p->mic_data_len);
    }
    sprintf(fn_orig,"%s%s.dat", fn_path, file_suffix);
    fd = fopen(fn_orig, "a");
    if (fd) {
        fwrite(msg_p->mic_data_ptr, 1, msg_p->mic_data_len, fd);
        fclose(fd);
        return 0;
    }
    return -1;
}
#endif

#ifdef AOS_FS_ENABLE
static uint32_t audio_datapath_emmc_get_free_space(const char *path)
{
    int32_t ret;
    uint64_t free_cap = 0;
    uint32_t free_M;
    uint32_t free_B;

    struct statfs stat = {0};

    ret = statfs(path, &stat);
    if (ret != 0) {
        LOG_ERR(2, "statfs fail, path:%s res:%d", path, ret);
        return -1;
    }

    free_cap = (uint64_t)stat.f_bavail * (uint64_t)stat.f_bsize;
    free_M = free_cap / (1024 * 1024);
    free_B = free_cap % (1024 * 1024);
    LOG_MSG(2, "emmc space free:%u(mb) %u(byte)", free_M, free_B);

    return free_M;
}

static void audio_datapath_emmc_delete_file(const char *path)
{
    DIR *dirp;
    FAR struct dirent *entryp;
    int number;
    char spath[512];
    struct stat result;
    Emmc_file_status_t delete_file;
    delete_file.frist_flag = 0;

    /* Open the directory */
    dirp = opendir(path);
    if (!dirp) {
        /* Failed to open the directory */
        TRACE(2, "ERROR: Failed to open directory '%s': %d", path, errno);
        return;
    }

    /* Read each directory entry */
    TRACE(0, "Directory:\n");
    number = 1;
    do {
        entryp = readdir(dirp);
        if (entryp && !strcmp(entryp->d_name, "."))
            continue;
        if (entryp && !strcmp(entryp->d_name, ".."))
            continue;
        if (entryp) {
            snprintf(spath, sizeof(spath) - 1, "%s/%s", path, entryp->d_name);
            stat(spath, &result);
            TRACE(4, "%2d. Type[%d]: %s Size: %ld Name: %s\n",
                  number,
                  entryp->d_type,
                  entryp->d_type == DTYPE_FILE ? "File " : " Dir ",
                  result.st_size,
                  entryp->d_name);

            if (entryp->d_type == DTYPE_FILE) {
                if ( delete_file.frist_flag == 0 ) {
                    memcpy(delete_file.file_patch, spath, sizeof(spath));
                    delete_file.time = result.st_mtime;
                    delete_file.frist_flag = 1;
                }

                if ( delete_file.time > result.st_mtime ) {
                    memcpy(delete_file.file_patch, spath, sizeof(spath));
                    delete_file.time = result.st_mtime;
                }
            }
        }
        number++;
    } while (entryp != NULL);

    closedir(dirp);

    TRACE(0, "delete record:\"%.128s\"",delete_file.file_patch);
    int ret = unlink((const char *)delete_file.file_patch);
    if (ret != 0){
        TRACE(0, "delete error:%d!",ret);
        return ;
    }
    return;
}

static int8_t audio_datapath_emmc_check_free(const char *filepatch)
{
    if (audio_datapath_emmc_get_free_space(filepatch) < 300) {
        LOG_MSG(0, "emmc space is not enough!");
        audio_datapath_emmc_delete_file(filepatch);
        return 1;
    }
    return 0;
}
#else
static int8_t audio_datapath_emmc_check_free(const char *filepatch) { return 0; }
#endif

static osThreadId audio_datapath_emmc_thread_id = NULL;
static void audio_datapath_emmc_thread(void* argument);
static const osThreadAttr_t audio_datapath_emmc_thread_attr = {
    .name       = "audio_datapath_emmc_thread",
    .stack_size = EMMC_THREAD_STACK_SIZE,
    .priority   = (osPriority_t)(osPriorityLow - 1),
};

static void audio_datapath_emmc_thread(void* argument)
{
    char fn_orig[128] = "";
    char fn_algo[128] = "";
    char fn_algo_32bit[128] = "";
    FILE* fd_orig = NULL;
    FILE* fd_algo = NULL;
    FILE* fd_algo_32bit = NULL;
    AUDIO_DATAPATH_EMMC_MESSAGE_T *msg_p = NULL;
    static uint16_t emmc_audio_dump_cnt = 0;

    sprintf(fn_orig, "%s/%s.pcm", fn_path, "mic_dump_orig");
    sprintf(fn_algo, "%s/%s.pcm", fn_path, "mic_dump_algo");
    sprintf(fn_algo_32bit, "%s/%s.pcm", fn_path, "mic_dump_algo_32bit");

    while (1) {
        if (!audio_datapath_emmc_mailbox_get(&msg_p)) {
            switch (msg_p->dump_port_id)
            {
                case MIC_DATA_DUMP_START:
                {
                    emmc_audio_dump_enable = true;
                    emmc_audio_dump_cnt = 0;
                    break;
                }

                case MIC_DATA_DUMP_STOP:
                {
                    emmc_audio_dump_enable = false;

                    // origin file dump stop
                    if (fd_orig) {
                        orig_wav_head = wav_close(orig_wav_head, emmc_dump_orig_file_size);
                        fseek(fd_orig, 0, SEEK_SET);
                        fwrite(&orig_wav_head, sizeof(struct BWF), 1, fd_orig);
                        fclose(fd_orig);
                        fd_orig = NULL;
                    }

                    if (emmc_origin_buff_ptr) {
                        emmc_heap_free(emmc_origin_buff_ptr);
                        emmc_origin_buff_ptr = NULL;
                    }
                    emmc_origin_buff_len = 0;
                    emmc_dump_orig_file_size = 0;

                    // algo file dump stop
                    if (fd_algo) {
                        algo_wav_head = wav_close(algo_wav_head, emmc_dump_algo_file_size);
                        fseek(fd_algo, 0, SEEK_SET);
                        fwrite(&algo_wav_head, sizeof(struct BWF), 1, fd_algo);
                        fclose(fd_algo);
                        fd_algo = NULL;
                    }

                    if (emmc_algo_buff_ptr) {
                        emmc_heap_free(emmc_algo_buff_ptr);
                        emmc_algo_buff_ptr = NULL;
                    }
                    emmc_algo_buff_len = 0;
                    emmc_dump_algo_file_size = 0;

                    // 32bit algo file dump stop
                    if (fd_algo_32bit) {
                        algo_32bit_wav_head = wav_close(algo_32bit_wav_head, emmc_dump_algo_32bit_file_size);
                        fseek(fd_algo_32bit, 0, SEEK_SET);
                        fwrite(&algo_32bit_wav_head, sizeof(struct BWF), 1, fd_algo_32bit);
                        fclose(fd_algo_32bit);
                        fd_algo_32bit = NULL;
                    }

                    if (emmc_algo_32bit_buff_ptr) {
                        emmc_heap_free(emmc_algo_32bit_buff_ptr);
                        emmc_algo_32bit_buff_ptr = NULL;
                    }
                    emmc_algo_32bit_buff_len = 0;
                    emmc_dump_algo_32bit_file_size = 0;
                    break;
                }

                case MIC_DATA_DUMP_PORT_ORIG:
                {
                    if (emmc_audio_dump_enable == false) {
                        break;
                    }

                    if (msg_p->mic_data_len == 0 || msg_p->mic_data_ptr == NULL) {
                        LOG_ERR(0, "invalid origin data");
                        break;
                    }

                    if (emmc_audio_dump_cnt++ >= 1000) {
                        audio_datapath_emmc_check_free(fn_path);
                        emmc_audio_dump_cnt = 0;
                    }

                    if (emmc_dump_orig_file_size >= EMMC_DUMP_FILE_MAX_SIZE) {
                        orig_wav_head = wav_close(orig_wav_head, emmc_dump_orig_file_size);
                        emmc_dump_orig_file_size = 0;

                        if (fd_orig) {
                            fseek(fd_orig, 0, SEEK_SET);
                            fwrite(&orig_wav_head, sizeof(struct BWF), 1, fd_orig);
                            fclose(fd_orig);
                            fd_orig = NULL;
                        }
                    }

                    if (fd_orig == NULL) {
                        audio_datapath_emmc_check_free(fn_path);
                        sprintf(fn_orig, "%s/%s_%d.wav", fn_path, "mic_dump_orig", hal_sys_timer_get());
                        LOG_MSG(1, "start record:\"%.128s\"", fn_orig);

                        fd_orig = fopen(fn_orig, "wb+");
                        orig_wav_head = wav_init(orig_wav_head, 1, 48000, 24);

                        if (fd_orig) {
                            emmc_dump_orig_file_size = 0;
                            fwrite(&orig_wav_head, sizeof(struct BWF), 1, fd_orig);
                        } else {
                            LOG_ERR(1, "file %s open failed.", fn_orig);
                        }
                    }

                    if (fd_orig) {

                        if (abs(msg_p->mic_resample_delta) >= 20) {
                            //do audio resample
                            LOG_MSG(1, "origin data resample time:%d", msg_p->mic_resample_delta);
                        }

                        fwrite(msg_p->mic_data_ptr, msg_p->mic_data_len, 1, fd_orig);
                        emmc_dump_orig_file_size += msg_p->mic_data_len;
                    }
                    break;
                }

                case MIC_DATA_DUMP_PORT_ALGO:
                {
                    if (emmc_audio_dump_enable == false) {
                        break;
                    }

                    if (msg_p->mic_data_len == 0 || msg_p->mic_data_ptr == NULL) {
                        LOG_ERR(0, "invalid algo data");
                        break;
                    }

                    if (emmc_audio_dump_cnt++ >= 1000) {
                        audio_datapath_emmc_check_free(fn_path);
                        emmc_audio_dump_cnt = 0;
                    }

                    if (emmc_dump_algo_file_size >= EMMC_DUMP_FILE_MAX_SIZE) {
                        algo_wav_head = wav_close(algo_wav_head, emmc_dump_algo_file_size);
                        emmc_dump_algo_file_size = 0;

                        if (fd_algo) {
                            fseek(fd_algo, 0, SEEK_SET);
                            fwrite(&algo_wav_head, sizeof(struct BWF), 1, fd_algo);
                            fclose(fd_algo);
                            fd_algo = NULL;
                        }
                    }

                    if (fd_algo == NULL) {
                        audio_datapath_emmc_check_free(fn_path);
                        sprintf(fn_algo, "%s/%s_%d.wav", fn_path, "mic_dump_algo", hal_sys_timer_get());
                        LOG_MSG(1, "start record:\"%.128s\"", fn_algo);

                        algo_wav_head = wav_init(algo_wav_head, 1, 48000, 24);
                        fd_algo = fopen(fn_algo, "wb+");

                        if (fd_algo) {
                            emmc_dump_algo_file_size = 0;
                            fwrite(&algo_wav_head, sizeof(struct BWF), 1, fd_algo);
                        } else {
                            LOG_ERR(1, "file %s open failed.", fn_algo);
                        }
                    }

                    if (fd_algo) {

                        if (abs(msg_p->mic_resample_delta) >= 20) {
                            //do audio resample
                            LOG_MSG(1, "algo data resample time:%d", msg_p->mic_resample_delta);
                        }

                        fwrite(msg_p->mic_data_ptr, msg_p->mic_data_len, 1, fd_algo);
                        emmc_dump_algo_file_size += msg_p->mic_data_len;
                    }
                    break;
                }

                case MIC_DATA_DUMP_PORT_ALGO_32BIT:
                {
                    if (emmc_audio_dump_enable == false) {
                        break;
                    }

                    if (msg_p->mic_data_len == 0 || msg_p->mic_data_ptr == NULL) {
                        LOG_ERR(0, "invalid 32bit algo data");
                        break;
                    }

                    if (emmc_audio_dump_cnt++ >= 1000) {
                        audio_datapath_emmc_check_free(fn_path);
                        emmc_audio_dump_cnt = 0;
                    }

                    if (emmc_dump_algo_32bit_file_size >= EMMC_DUMP_FILE_MAX_SIZE) {
                        algo_32bit_wav_head = wav_close(algo_32bit_wav_head, emmc_dump_algo_32bit_file_size);
                        emmc_dump_algo_32bit_file_size = 0;

                        if (fd_algo_32bit) {
                            fseek(fd_algo_32bit, 0, SEEK_SET);
                            fwrite(&algo_32bit_wav_head, sizeof(struct BWF), 1, fd_algo_32bit);
                            fclose(fd_algo_32bit);
                            fd_algo_32bit = NULL;
                        }
                    }

                    if (fd_algo_32bit == NULL) {
                        audio_datapath_emmc_check_free(fn_path);
                        sprintf(fn_algo_32bit, "%s/%s_%d.wav", fn_path, "mic_dump_algo_32bit", hal_sys_timer_get());
                        LOG_MSG(1, "start record:\"%.128s\"", fn_algo_32bit);

                        fd_algo_32bit = fopen(fn_algo_32bit, "wb+");
                        algo_32bit_wav_head = wav_init(algo_32bit_wav_head, 1, 48000, 32);

                        if (fd_algo_32bit) {
                            emmc_dump_algo_32bit_file_size = 0;
                            fwrite(&algo_32bit_wav_head, sizeof(struct BWF), 1, fd_algo_32bit);
                        } else {
                            LOG_ERR(1, "file %s open failed.", fn_algo_32bit);
                        }
                    }

                    if (fd_algo_32bit) {

                        if (abs(msg_p->mic_resample_delta) >= 20) {
                            //do audio resample
                            LOG_MSG(1, "32bit algo data resample time:%d", msg_p->mic_resample_delta);
                        }

                        fwrite(msg_p->mic_data_ptr, msg_p->mic_data_len, 1, fd_algo_32bit);
                        emmc_dump_algo_32bit_file_size += msg_p->mic_data_len;
                    }
                    break;
                }

                default:
                {
                    LOG_ERR(1, "emmc recv invalid mailbox id:%d", msg_p->dump_port_id);
                    break;
                }

            }

            audio_datapath_emmc_mailbox_free(msg_p);
            msg_p = NULL;
        }
    }
}

static void audio_datapath_emmc_thread_init(void)
{
    if (audio_datapath_emmc_thread_id == NULL) {
        audio_datapath_emmc_thread_id = osThreadNew(audio_datapath_emmc_thread, NULL, &audio_datapath_emmc_thread_attr);
    }
    ASSERT(audio_datapath_emmc_thread_id, "Failed to Create audio_datapath_emmc_thread\n");

    LOG_MSG(0, "emmc thread init done!");
}

bool audio_datapath_emmc_is_dump_started(void)
{
    return emmc_audio_dump_enable;
}

static int32_t audio_datapath_emmc_update_resample_delta(uint8_t port, bool result)
{
    return 0;
}

bool audio_datapath_emmc_dump_orig_handler(uint8_t* buff, uint32_t len)
{
    if (audio_datapath_emmc_is_dump_started() == false) {
        return false;
    }

    if (len > EMMC_INPUT_BUFF_SIZE) {
        LOG_ERR(1, "invalid input buff size:%d", len);
        return false;
    }

#if MIC_EMMC_DUMP_DEBUG
    static uint32_t cnt = 0;
    if (++cnt % 101 == 0) {
        LOG_MSG(2, "emmc orig dump handler, buf:%p len:%d.", buff, len);
    }
#endif

    if (emmc_origin_buff_ptr == NULL) {
        emmc_origin_buff_ptr = (uint8_t*)emmc_heap_malloc(EMMC_CACHE_BUFFER_SIZE);
        if (emmc_origin_buff_ptr == NULL) {
            LOG_ERR(1, "emmc heap malloc failed, mailbox cnt:%d", audio_datapath_emmc_mailbox_cnt());
            return false;
        }
        emmc_origin_buff_len = 0;
    }

    audio_datapath_emmc_update_resample_delta(MIC_DATA_DUMP_PORT_ORIG, false);

#if IS_USE_24BIT_DUMP
    len = audio_copy_32bit_to_24bit(emmc_audio_input_buff, buff, len);
#else
    emmc_audio_input_buff = buff;
#endif

    if (len + emmc_origin_buff_len >= EMMC_CACHE_BUFFER_SIZE) {
        uint16_t tail_len = EMMC_CACHE_BUFFER_SIZE - emmc_origin_buff_len;
        int32_t resample_delta = audio_datapath_emmc_update_resample_delta(MIC_DATA_DUMP_PORT_ORIG, true);

        memcpy(emmc_origin_buff_ptr + emmc_origin_buff_len, emmc_audio_input_buff, tail_len);
        audio_datapath_emmc_mailbox_put(MIC_DATA_DUMP_PORT_ORIG, emmc_origin_buff_ptr, EMMC_CACHE_BUFFER_SIZE, resample_delta);

        emmc_origin_buff_ptr = (uint8_t*)emmc_heap_malloc(EMMC_CACHE_BUFFER_SIZE);
        if (emmc_origin_buff_ptr == NULL) {
            LOG_ERR(1, "emmc heap malloc failed, mailbox cnt:%d", audio_datapath_emmc_mailbox_cnt());
            return false;
        }

        emmc_origin_buff_len = len - tail_len;
        memcpy(emmc_origin_buff_ptr, emmc_audio_input_buff + tail_len, emmc_origin_buff_len);
    } else {
        memcpy(emmc_origin_buff_ptr + emmc_origin_buff_len, emmc_audio_input_buff, len);
        emmc_origin_buff_len += len;
    }

    return true;
}

bool audio_datapath_emmc_dump_algo_handler(uint8_t* buff, uint32_t len)
{
    if (audio_datapath_emmc_is_dump_started() == false) {
        return false;
    }

    if (len > EMMC_INPUT_BUFF_SIZE) {
        LOG_ERR(1, "invalid input buff size:%d", len);
        return false;
    }

#if MIC_EMMC_DUMP_DEBUG
    static uint32_t cnt = 0;
    if (++cnt % 101 == 0) {
        LOG_MSG(2, "emmc algo dump handler, buf:%p len:%d.", buff, len);
    }
#endif

    if (emmc_algo_buff_ptr == NULL) {
        emmc_algo_buff_ptr = (uint8_t*)emmc_heap_malloc(EMMC_CACHE_BUFFER_SIZE);
        if (emmc_algo_buff_ptr == NULL) {
            LOG_ERR(1, "emmc heap malloc failed, mailbox cnt:%d", audio_datapath_emmc_mailbox_cnt());
            return false;
        }
        emmc_algo_buff_len = 0;
    }

    audio_datapath_emmc_update_resample_delta(MIC_DATA_DUMP_PORT_ALGO, false);

#if IS_USE_24BIT_DUMP
    len = audio_copy_32bit_to_24bit(emmc_audio_input_buff, buff, len);
#else
    emmc_audio_input_buff = buff;
#endif

    if (len + emmc_algo_buff_len >= EMMC_CACHE_BUFFER_SIZE) {
        uint16_t tail_len = EMMC_CACHE_BUFFER_SIZE - emmc_algo_buff_len;
        int32_t resample_delta = audio_datapath_emmc_update_resample_delta(MIC_DATA_DUMP_PORT_ALGO, true);

        memcpy(emmc_algo_buff_ptr + emmc_algo_buff_len, emmc_audio_input_buff, tail_len);
        audio_datapath_emmc_mailbox_put(MIC_DATA_DUMP_PORT_ALGO, emmc_algo_buff_ptr, EMMC_CACHE_BUFFER_SIZE, resample_delta);

        emmc_algo_buff_ptr = (uint8_t*)emmc_heap_malloc(EMMC_CACHE_BUFFER_SIZE);
        if (emmc_algo_buff_ptr == NULL) {
            LOG_ERR(1, "emmc heap malloc failed, mailbox cnt:%d", audio_datapath_emmc_mailbox_cnt());
            return false;
        }

        emmc_algo_buff_len = len - tail_len;
        memcpy(emmc_algo_buff_ptr, emmc_audio_input_buff + tail_len, emmc_algo_buff_len);
    } else {
        memcpy(emmc_algo_buff_ptr + emmc_algo_buff_len, emmc_audio_input_buff, len);
        emmc_algo_buff_len += len;
    }

    return true;
}

bool audio_datapath_emmc_dump_algo_32bit_handler(uint8_t* buff, uint32_t len)
{
    if (audio_datapath_emmc_is_dump_started() == false) {
        return false;
    }

    if (len > EMMC_INPUT_BUFF_SIZE) {
        LOG_ERR(1, "invalid input buff size:%d", len);
        return false;
    }

#if MIC_EMMC_DUMP_DEBUG
    static uint32_t cnt = 0;
    if (++cnt % 101 == 0) {
        LOG_MSG(2, "emmc 32bit algo dump handler, buf:%p len:%d.", buff, len);
    }
#endif

    if (emmc_algo_32bit_buff_ptr == NULL) {
        emmc_algo_32bit_buff_ptr = (uint8_t*)emmc_heap_malloc(EMMC_CACHE_BUFFER_SIZE);
        if (emmc_algo_32bit_buff_ptr == NULL) {
            LOG_ERR(1, "emmc heap malloc failed, mailbox cnt:%d", audio_datapath_emmc_mailbox_cnt());
            return false;
        }
        emmc_algo_32bit_buff_len = 0;
    }

    audio_datapath_emmc_update_resample_delta(MIC_DATA_DUMP_PORT_ALGO_32BIT, false);

    if (len + emmc_algo_32bit_buff_len >= EMMC_CACHE_BUFFER_SIZE) {
        uint16_t tail_len = EMMC_CACHE_BUFFER_SIZE - emmc_algo_32bit_buff_len;
        int32_t resample_delta = audio_datapath_emmc_update_resample_delta(MIC_DATA_DUMP_PORT_ALGO_32BIT, true);

        memcpy(emmc_algo_32bit_buff_ptr + emmc_algo_32bit_buff_len, buff, tail_len);
        audio_datapath_emmc_mailbox_put(MIC_DATA_DUMP_PORT_ALGO_32BIT, emmc_algo_32bit_buff_ptr, EMMC_CACHE_BUFFER_SIZE, resample_delta);

        emmc_algo_32bit_buff_ptr = (uint8_t*)emmc_heap_malloc(EMMC_CACHE_BUFFER_SIZE);
        if (emmc_algo_32bit_buff_ptr == NULL) {
            LOG_ERR(1, "emmc heap malloc failed, mailbox cnt:%d", audio_datapath_emmc_mailbox_cnt());
            return false;
        }

        emmc_algo_32bit_buff_len = len - tail_len;
        memcpy(emmc_algo_32bit_buff_ptr, buff + tail_len, emmc_algo_32bit_buff_len);
    } else {
        memcpy(emmc_algo_32bit_buff_ptr + emmc_algo_32bit_buff_len, buff, len);
        emmc_algo_32bit_buff_len += len;
    }

    return true;
}

void audio_datapath_emmc_dump_mode_set(uint8_t mode)
{
    audio_datapath_emmc_mode = mode;
    LOG_MSG(1, "%s-->%d", __func__, mode);
}

void audio_datapath_emmc_output_stream_onoff(bool onoff)
{
    if (audio_datapath_emmc_capture_stream_started == onoff) {
        LOG_DBG(1, "audio_emmc_capture_stream already %s", onoff == true ? "on" : "off");
        return;
    }

#ifdef TX_ALGO_PROCESS
    tx_algo_emmc_dump_algo_32bit_degister();
    tx_algo_emmc_dump_algo_24bit_degister();
    tx_algo_emmc_dump_orig_24bit_degister();
#endif

    if (onoff) {
#ifdef TX_ALGO_PROCESS
        if (audio_datapath_emmc_mode == 1) {
            tx_algo_emmc_dump_algo_24bit_register(audio_datapath_emmc_dump_algo_handler);
        } else if (audio_datapath_emmc_mode == 2) {
            tx_algo_emmc_dump_algo_32bit_register(audio_datapath_emmc_dump_algo_32bit_handler);
        } else if (audio_datapath_emmc_mode == 3) {
            tx_algo_emmc_dump_orig_24bit_register(audio_datapath_emmc_dump_orig_handler);
            tx_algo_emmc_dump_algo_24bit_register(audio_datapath_emmc_dump_algo_handler);
        } else if (audio_datapath_emmc_mode == 4) {
            tx_algo_emmc_dump_orig_24bit_register(audio_datapath_emmc_dump_orig_handler);
            tx_algo_emmc_dump_algo_32bit_register(audio_datapath_emmc_dump_algo_32bit_handler);
        } else {
            LOG_ERR(2, "%s unspport mode:%d", __func__, audio_datapath_emmc_mode);
            return;
        }
#endif
        audio_datapath_emmc_mailbox_put(MIC_DATA_DUMP_START, NULL, 0, 0);
    } else {
        audio_datapath_emmc_mailbox_put(MIC_DATA_DUMP_STOP, NULL, 0, 0);
    }

    audio_datapath_emmc_capture_stream_started = onoff;
}

void audio_datapath_emmc_init(void)
{
    app_sysfreq_req(APP_SYSFREQ_USER_APP_EMMC, APP_SYSFREQ_26M);

    audio_datapath_emmc_mailbox_init();

    audio_datapath_emmc_thread_init();
}

void audio_datapath_emmc_deinit(void)
{
    app_sysfreq_req(APP_SYSFREQ_USER_APP_EMMC, APP_SYSFREQ_32K);
}