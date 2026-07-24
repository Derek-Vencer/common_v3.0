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
#include "cmsis_os.h"
#include "string.h"
#include "cqueue.h"
#include "list.h"

#include "hal_trace.h"
#include "hal_aud.h"
#include "bluetooth_bt_api.h"
#if defined(USE_BASIC_THREADS)
#include "app_thread.h"
#else
#include "app_audio_thread.h"
#endif
#include "app_audio.h"
#include "app_media_player.h"
#include "app_ring_merge.h"
#include "nvrecord_bt.h"
#include <assert.h>


#include "a2dp_api.h"

#include "btapp.h"
#include "app_bt_media_manager.h"
#include "hal_location.h"

#include "app_anc_assist.h"

#ifdef __AI_VOICE__
#include "app_ai_voice.h"
#endif

static bool app_audio_init = false;
static uint32_t g_app_audio_syspool_start_addr;

#ifndef APP_MEMPOOL_USER_ALLOC_ALIGN
#define APP_MEMPOOL_USER_ALLOC_ALIGN 4u
#endif

#ifdef TOTA_FACTORY_USED
extern uint32_t tota_mempool_get_size(void);
#endif
#ifdef ANC_ASSIST_ENABLED
extern uint32_t app_anc_assist_stream_heap_size(void);
#endif

static layout_t g_active_layout;

static inline uint32_t align_up(uint32_t v, uint32_t a){ return (v + (a-1)) & ~(a-1); }

#ifdef AUDIO_MEMPOOL_DEBUG
#define MP_LOG(fmt, ...) APP_TEST_TRACE(0, "[MP] " fmt, ##__VA_ARGS__)
const layout_t *app_audio_mempool_get_layout(void)
{
    return &g_active_layout;
}

void app_audio_mempool_debug_dump(void)
{
    MP_LOG("base=0x%08x size=%u sco_a2dp=%d", g_active_layout.base_addr, g_active_layout.total_size, g_active_layout.sco_a2dp_active);

    for (int u = 0; u < SYSPOOL_USER_QTY; u++) {
        const slice_t *s = &g_active_layout.slices[u];
        const app_mempool_reg_t *r = &g_active_layout.reg[u];

        if (s->len == 0)
            continue;

        char flag1 = (r->size == 0) ? 'M' : '-';
        char flag2 = r->use_remain ? 'R' : '-';
        char flag3 = r->sco_cowork ? 'S' : '-';
        char flag4 = r->a2dp_cowork ? 'A' : '-';

        MP_LOG("U%d off=0x%05x len=%-6u used=%-6u free=%-6u %c %c %c %c",
            u,
            s->off,
            s->len,
            s->used,
            s->len - s->used,
            flag1, flag2, flag3, flag4);
    }

    MP_LOG("END");
}

void app_audio_mempool_reset(void)
{
    APP_TEST_TRACE(0, "[MP] RESET");

    memset(g_active_layout.reg, 0,
           sizeof(g_active_layout.reg));

    for (int u = 0; u < SYSPOOL_USER_QTY; u++) {
        g_active_layout.slices[u].off  = 0;
        g_active_layout.slices[u].len  = 0;
        g_active_layout.slices[u].used = 0;
    }

    g_active_layout.sco_a2dp_active = false;
}

static int app_audio_mempool_build_layout(layout_t *layout);
int32_t app_audio_mempool_build_ut_layout(void)
{
    return app_audio_mempool_build_layout(&g_active_layout);
}
#endif

uint32_t app_audio_mempool_get_user_free_size(SYSPOOL_USER_t u)
{
    if (u <= SYSPOOL_USER_NONE || u >= SYSPOOL_USER_QTY) {
        AUDIOPLAYERS_TRACE(3, "[%s] Invalid user=%d", __func__, u);
        return 0;  // Invalid user
    }

    slice_t *slice = &g_active_layout.slices[u];
    uint32_t free_size = slice->len - slice->used;

    AUDIOPLAYERS_TRACE(2, "[%s] Free memory: user=%d, free_size=%u",
                       __func__, u, free_size);

    return free_size;
}

uint32_t app_audio_mempool_len(SYSPOOL_USER_t u)
{
    if (u <= SYSPOOL_USER_NONE || u >= SYSPOOL_USER_QTY) {
        AUDIOPLAYERS_TRACE(3, "[%s] Invalid user=%d", __func__, u);
        return 0;  // Invalid user
    }

    uint32_t len = g_active_layout.slices[u].len;

    AUDIOPLAYERS_TRACE(2, "[%s] Free memory: user=%d, free_size=%u",
                       __func__, u, len);

    return len;
}

void *app_audio_mempool_user_alloc(SYSPOOL_USER_t u, uint32_t size)
{
    if (u <= SYSPOOL_USER_NONE || u >= SYSPOOL_USER_QTY) return NULL;

    const app_mempool_reg_t *reg = &g_active_layout.reg[u];
    slice_t *slice = &g_active_layout.slices[u];

    // Check if size is valid and available in the slice
    if (size == 0 || slice->used + size > slice->len) {
        AUDIOPLAYERS_TRACE(3, "[%s] Alloc failed: user=%d, requested_size=%u, available_size=%u",
                           __func__, u, size, slice->len - slice->used);
        return NULL;
    }

    if (reg->sco_cowork && reg->a2dp_cowork) {
        for (int user = 1; user < SYSPOOL_USER_QTY; ++user) {
            if (!g_active_layout.reg[user].a2dp_cowork && !g_active_layout.reg[user].sco_cowork && g_active_layout.slices[user].used != 0) {
                    AUDIOPLAYERS_TRACE(3, "[%s] Invalid memory usage, user=%d with cowork flag, user=%d", __func__, u, user);
                    return NULL;
            }
        }
    } else if (reg->sco_cowork || reg->a2dp_cowork) {
        for (int user = 1; user < SYSPOOL_USER_QTY; ++user) {
            if ((reg->sco_cowork && !g_active_layout.reg[user].sco_cowork) ||
                (reg->a2dp_cowork && !g_active_layout.reg[user].a2dp_cowork)) {
                if (g_active_layout.slices[user].used != 0) {
                    AUDIOPLAYERS_TRACE(3, "[%s] Invalid memory usage, user=%d with cowork flag, user=%d",
                                       __func__, u, user);
                    return NULL;
                }
            }
        }
    } else if (!reg->sco_cowork && !reg->a2dp_cowork) {
        for (int user = 1; user < SYSPOOL_USER_QTY; ++user) {
            if ((g_active_layout.reg[user].a2dp_cowork && g_active_layout.reg[user].sco_cowork)) {
                if (g_active_layout.slices[user].used != 0) {
                    AUDIOPLAYERS_TRACE(3, "[%s] Invalid memory usage, user=%d with cowork flag, user=%d",
                                       __func__, u, user);
                    return NULL;
                }
            }
        }
    }

    void *allocated_ptr = (void *)((uintptr_t)g_active_layout.base_addr + slice->off + slice->used);
    slice->used += size;

    AUDIOPLAYERS_TRACE(2, "[%s] Allocated: user=%d, size=%u, address=%p, used=%u/%u",
                       __func__, u, size, allocated_ptr, slice->used, slice->len);

    return allocated_ptr;
}

int32_t app_audio_mempool_user_free(SYSPOOL_USER_t user)
{
    slice_t *slice = &g_active_layout.slices[user];

    AUDIOPLAYERS_TRACE(2, "[%s] Freeing memory: user=%d, used=%u, len=%u",
                       __func__, user, slice->used, slice->len);

    slice->used = 0;
    return 0;
}

static int app_audio_mempool_build_layout(layout_t *layout)
{
    uint32_t total_size = syspool_original_size();
    if (total_size == 0) return -1;

    syspool_init_specific_size(total_size);
    layout->base_addr = g_app_audio_syspool_start_addr = (uint32_t)(uintptr_t)syspool_start_addr();
    layout->total_size = total_size;

    uint32_t cursor = total_size;
    uint32_t a2dp_cursor = total_size;
    uint32_t sco_cursor = total_size;
    uint32_t total_allocated = 0; // Track total memory allocated for non-`use_remain` users
    uint32_t total_allocated_sco = 0;
    uint32_t total_allocated_a2dp = 0;

    // 00
    for (int user = 1; user < SYSPOOL_USER_QTY; ++user) {
        const app_mempool_reg_t *reg = &layout->reg[user];
        if (!reg->a2dp_cowork && !reg->sco_cowork && reg->use_remain) {

        uint32_t size = total_size;
        if (size == 0) continue;

        size = align_up(size, 4);

        layout->slices[user].off = 0;
        layout->slices[user].len = size;

        AUDIOPLAYERS_TRACE(2, "[%s] user=%d 00, allocated size=%u offset=%u",
                        __func__, user, size, layout->slices[user].off);
        }
    }
    //11
    // Allocate memory for users with both a2dp_cowork and sco_cowork set to true
    for (int user = 1; user < SYSPOOL_USER_QTY; ++user) {
        const app_mempool_reg_t *reg = &layout->reg[user];
        if (reg->a2dp_cowork && reg->sco_cowork) {

            uint32_t size = reg->size;;
            if (size == 0) continue;

            size = align_up(size, 4);

            layout->slices[user].off = cursor - size;
            layout->slices[user].len = size;
            cursor -= size;
            total_allocated += size;

            AUDIOPLAYERS_TRACE(2, "[%s] user=%d 11, allocated size=%u at offset=%u",
                            __func__, user, size, layout->slices[user].off);
        }
    }
    //10
    // Allocate memory for users with only sco_cowork set to true
    for (int user = 1; user < SYSPOOL_USER_QTY; ++user) {
        const app_mempool_reg_t *reg = &layout->reg[user];
        if (reg->sco_cowork && !reg->use_remain) {
            uint32_t size = reg->size;
            if (size == 0) continue;

            size = align_up(size, 4);

            sco_cursor = cursor;
            layout->slices[user].off = sco_cursor - size;
            layout->slices[user].len = size;
            sco_cursor -= size;
            total_allocated_sco += size;

            AUDIOPLAYERS_TRACE(2, "[%s] user=%d 10, allocated size=%u at offset=%u",
                            __func__, user, size, layout->slices[user].off);
        }
    }
    //10
    // Allocate remaining memory for users with sco_cowork and use_remain set to true
    for (int user = 1; user < SYSPOOL_USER_QTY; ++user) {
        const app_mempool_reg_t *reg = &layout->reg[user];
        if (reg->sco_cowork && reg->use_remain) {
            uint32_t remaining_size = total_size - total_allocated_sco;

            if (remaining_size == 0) continue;

            remaining_size = align_up(remaining_size, 4);

            layout->slices[user].off = sco_cursor - remaining_size;
            layout->slices[user].len = remaining_size;
            sco_cursor -= remaining_size;
            total_allocated_sco += remaining_size;

            AUDIOPLAYERS_TRACE(2, "[%s] user=%d 10&remain,allocated size=%u offset=%u",
                            __func__, user, remaining_size, layout->slices[user].off);
        }

    }
    //01
    // Allocate memory for users with only a2dp_cowork set to true
    for (int user = 1; user < SYSPOOL_USER_QTY; ++user) {
        const app_mempool_reg_t *reg = &layout->reg[user];
        if (reg->a2dp_cowork && !reg->use_remain) {
            uint32_t size = reg->size;
            if (size == 0) continue;

            size = align_up(size, 4);
            a2dp_cursor = cursor;
            layout->slices[user].off = a2dp_cursor - size;
            layout->slices[user].len = size;
            a2dp_cursor -= size;
            total_allocated_a2dp += size;

            AUDIOPLAYERS_TRACE(2, "[%s] user=%d 01, allocated size=%u offset=%u",
                            __func__, user, size, layout->slices[user].off);
        }

    }
    //01
    // Allocate remaining memory for users with a2dp_cowork and use_remain set to true
    for (int user = 1; user < SYSPOOL_USER_QTY; ++user) {
        const app_mempool_reg_t *reg = &layout->reg[user];
        if (reg->a2dp_cowork && reg->use_remain) {
            uint32_t remaining_size = total_size - total_allocated_a2dp;

            if (remaining_size == 0) continue;

            remaining_size = align_up(remaining_size, 4);

            layout->slices[user].off = a2dp_cursor - remaining_size;
            layout->slices[user].len = remaining_size;
            a2dp_cursor -= remaining_size;
            total_allocated_a2dp += remaining_size;

            AUDIOPLAYERS_TRACE(2, "[%s] user=%d 01&remain,allocated size=%u offset=%u",
                            __func__, user, remaining_size, layout->slices[user].off);
        }

    }

    // Check if all memory has been allocated properly
    if (total_allocated_a2dp != total_size || total_allocated_sco != total_size) {
        AUDIOPLAYERS_TRACE(3, "[%s] Warning: total memory allocated (%u) doesn't match total size (%u)",
                           __func__, total_allocated_a2dp, total_size);
        return -2;  // Allocation mismatch
    }

    return 0;
}

int32_t app_audio_mempool_register(const app_mempool_reg_t *info)
{
    if (!info || info->user <= SYSPOOL_USER_NONE || info->user >= SYSPOOL_USER_QTY) {
        AUDIOPLAYERS_TRACE(3, "[%s] Invalid registration: user=%d", __func__, info ? info->user : -1);
        return -1;  // Invalid user
    }

    g_active_layout.reg[info->user] = *info;
    AUDIOPLAYERS_TRACE(1, "[%s] Registered user=%d, size=%u, a2dp_cowork=%d, sco_cowork=%d",
                       __func__, info->user, info->size, info->a2dp_cowork, info->sco_cowork);

    g_active_layout.slices[info->user].off = 0;
    g_active_layout.slices[info->user].len = 0;
    g_active_layout.slices[info->user].used = 0;

    return 0;  // Successful registration
}

static void register_defaults(void)
{
    static const app_mempool_reg_t regs[] = {
        { SYSPOOL_USER_SCO_CALL, 0, true, false, true },
        { SYSPOOL_USER_A2DP_AUDIO, 0, false, true, true },
        { SYSPOOL_USER_ANC_TEST, 0, false, false, true },
        { SYSPOOL_USER_APP_CAPTURE, APP_CAPTURE_AUDIO_BUFFER_SIZE, false, true, false },
#ifdef TOTA_FACTORY_USED
        { SYSPOOL_USER_TOTA,        tota_mempool_get_size(), false, true, false},
#endif
#ifdef ANC_ASSIST_ENABLED
        { SYSPOOL_USER_ANC_ASSIST,  app_anc_assist_stream_heap_size(), false, true, false},
#endif
    };

    for (size_t i = 0; i < sizeof(regs) / sizeof(regs[0]); ++i)
        app_audio_mempool_register(&regs[i]);
}

int32_t app_audio_sys_mempool_init(void)
{
    register_defaults();
    // Build the layout and initialize syspool (calls syspool_init_specific_size)
    return app_audio_mempool_build_layout(&g_active_layout);
}

#if (defined(PLAYBACK_USE_I2S) || defined(FREEMAN_ENABLED_STERO))
#define APP_MEDIA_BUFFER_SIZE (11*1024)
#elif (PROMPT_USE_AAC)
#define APP_MEDIA_BUFFER_SIZE (50*1024)
#else
//SBC_Original need 5888, SBC Reduce need 5888+2424
#define APP_MEDIA_BUFFER_SIZE (5888+2424)
#endif

static uint8_t media_buffer[APP_MEDIA_BUFFER_SIZE] __attribute__((aligned(4)));

static uint32_t media_buff_size_used;

app_ble_audio_callback_t g_app_ble_audio_start_cb = NULL;
app_ble_audio_callback_t g_app_ble_audio_stop_cb = NULL;
app_ble_audio_callback_t g_app_ble_audio_stop_single_stream_cb = NULL;

int app_media_mempool_init(void)
{
    AUDIOPLAYERS_TRACE(1, "init the app media mem pool size %d.", APP_MEDIA_BUFFER_SIZE);
    media_buff_size_used = 0;
    memset((uint8_t *)media_buffer, 0, APP_MEDIA_BUFFER_SIZE);

    return 0;
}

uint32_t app_media_mempool_free_buff_size()
{
    return APP_MEDIA_BUFFER_SIZE - media_buff_size_used;
}

int app_media_mempool_get_buff(uint8_t **buff, uint32_t size)
{
    uint32_t buff_size_free;
    uint8_t* media_buf_addr = (uint8_t *)media_buffer;
    buff_size_free = app_media_mempool_free_buff_size();

    if (size % 4){
        size = size + (4 - size % 4);
    }

    AUDIOPLAYERS_TRACE(2,"Get media buf, current free %d to allocate %d", buff_size_free, size);

    ASSERT(size <= buff_size_free, "[%s] size = %d > free size = %d", __func__, size, buff_size_free);

    *buff = media_buf_addr + media_buff_size_used;

    media_buff_size_used += size;
    AUDIOPLAYERS_TRACE(3,"Allocate %d, now used %d left %d",
        size, media_buff_size_used, app_media_mempool_free_buff_size());

    return 0;
}

osPoolDef (app_audio_status_mempool, 20, APP_AUDIO_STATUS);
osPoolId   app_audio_status_mempool = NULL;

// control queue access
osMutexId g_app_audio_queue_mutex_id = NULL;
osMutexDef(g_app_audio_queue_mutex);

// control pcmbuff access
static CQueue app_audio_pcm_queue;
static osMutexId app_audio_pcmbuff_mutex_id = NULL;
osMutexDef(app_audio_pcmbuff_mutex);

#ifdef __AUDIO_QUEUE_SUPPORT__
typedef struct {
  list_t *audio_list;
}APP_AUDIO_CONFIG;

APP_AUDIO_CONFIG app_audio_conifg = {
    .audio_list = NULL
};

#endif

void LOCK_APP_AUDIO_QUEUE()
{
    osMutexWait(g_app_audio_queue_mutex_id, osWaitForever);
}

void UNLOCK_APP_AUDIO_QUEUE()
{
    osMutexRelease(g_app_audio_queue_mutex_id);
}

uint32_t app_audio_lr_balance(uint8_t *buf, uint32_t len, int8_t balance)
{
	short *balance_buf=(short *)buf;
	uint32_t balance_len = len/2;
	float factor;

	ASSERT((balance >= -100) && (balance <= 100), "balance = %d is invalid!", balance);

	if(balance > 0)
	{
		//reduce L channel
		factor = 1 - 0.01 * balance;
		for(uint32_t i=0; i<balance_len;i+=2)
		{
			balance_buf[i] = (short) (factor * balance_buf[i]);
		}
	}
	else if(balance < 0)
	{
		//reduce R channel
		factor = 1 + 0.01 * balance;
		for(uint32_t i=0; i<balance_len;i+=2)
		{
			balance_buf[i+1] = (short) (factor * balance_buf[i+1]);
		}
	}
    return 0;
}

void app_audio_mempool_init_with_specific_size(uint32_t size)
{
	syspool_init_specific_size(size);
}

// for sco
uint32_t app_audio_mempool_original_size(void)
{
    int32_t syspool_start_addr_offset = (uint32_t)(uintptr_t)syspool_start_addr() - g_app_audio_syspool_start_addr;
    TRACE(0, "[%s].. syspool start offset %d", __func__, syspool_start_addr_offset);
    return app_audio_mempool_len(SYSPOOL_USER_SCO_CALL) - syspool_start_addr_offset;
    // return syspool_original_size();
}

// for a2dp
uint32_t app_audio_mempool_size(void)
{
    int32_t syspool_start_addr_offset = (uint32_t)(uintptr_t)syspool_start_addr() - g_app_audio_syspool_start_addr;
    TRACE(0, "[%s].. syspool start offset %d", __func__, syspool_start_addr_offset);
    return app_audio_mempool_len(SYSPOOL_USER_A2DP_AUDIO) - syspool_start_addr_offset;
}

int app_audio_pcmbuff_init(uint8_t *buff, uint16_t len)
{
    if (app_audio_pcmbuff_mutex_id == NULL)
        app_audio_pcmbuff_mutex_id = osMutexCreate((osMutex(app_audio_pcmbuff_mutex)));

    if ((buff == NULL)||(app_audio_pcmbuff_mutex_id == NULL))
        return -1;

    osMutexWait(app_audio_pcmbuff_mutex_id, osWaitForever);
    InitCQueue(&app_audio_pcm_queue, len, buff);
    memset(buff, 0x00, len);
    osMutexRelease(app_audio_pcmbuff_mutex_id);

    return 0;
}

int app_audio_pcmbuff_space(void)
{
    int len;

    osMutexWait(app_audio_pcmbuff_mutex_id, osWaitForever);
    len = AvailableOfCQueue(&app_audio_pcm_queue);
    osMutexRelease(app_audio_pcmbuff_mutex_id);

    return len;
}

int app_audio_pcmbuff_length(void)
{
    int len;

    osMutexWait(app_audio_pcmbuff_mutex_id, osWaitForever);
    len = LengthOfCQueue(&app_audio_pcm_queue);
    osMutexRelease(app_audio_pcmbuff_mutex_id);

    return len;
}

int app_audio_pcmbuff_put(uint8_t *buff, uint16_t len)
{
    int status;

    osMutexWait(app_audio_pcmbuff_mutex_id, osWaitForever);
    status = EnCQueue(&app_audio_pcm_queue, buff, len);
    osMutexRelease(app_audio_pcmbuff_mutex_id);

    return status;
}

int app_audio_pcmbuff_get(uint8_t *buff, uint16_t len)
{
    unsigned char *e1 = NULL, *e2 = NULL;
    unsigned int len1 = 0, len2 = 0;
    int status;

    osMutexWait(app_audio_pcmbuff_mutex_id, osWaitForever);
    status = PeekCQueue(&app_audio_pcm_queue, len, &e1, &len1, &e2, &len2);
    if (len==(len1+len2)){
        memcpy(buff,e1,len1);
        memcpy(buff+len1,e2,len2);
        DeCQueue(&app_audio_pcm_queue, 0, len1);
        DeCQueue(&app_audio_pcm_queue, 0, len2);
    }else{
        memset(buff, 0x00, len);
        status = -1;
    }
    osMutexRelease(app_audio_pcmbuff_mutex_id);

    return status;
}

int app_audio_pcmbuff_discard(uint16_t len)
{
    int status;

    osMutexWait(app_audio_pcmbuff_mutex_id, osWaitForever);
    status = DeCQueue(&app_audio_pcm_queue, 0, len);
    osMutexRelease(app_audio_pcmbuff_mutex_id);

    return status;
}

FRAM_TEXT_LOC
void app_audio_memcpy_16bit(int16_t *des, int16_t *src, int len)
{
    // Check input
    for(int i=0; i<len; i++)
    {
        des[i] = src[i];
    }
}

FRAM_TEXT_LOC
void app_audio_memset_16bit(int16_t *des, int16_t val, int len)
{
    // Check input
    for(int i=0; i<len; i++)
    {
        des[i] = val;
    }
}

#ifdef __AUDIO_QUEUE_SUPPORT__
#if defined(USE_BASIC_THREADS)
int app_audio_sendrequest_param(uint16_t id, uint8_t status, uint32_t ptr, uint32_t param)
{
    uint32_t audevt;
    APP_MESSAGE_BLOCK msg;

    AUDIOPLAYERS_TRACE(7,"%s ca=%p type %d%s op %d%s aud %d", __func__, __builtin_return_address(0),
                id, player2str(id), status, audio_op_to_str((enum APP_BT_SETTING_T)status), ptr);

    if(app_audio_init == false)
        return -1;

    msg.mod_id = APP_MODULE_AUDIO;
    msg.mod_level = APP_MOD_LEVEL_3;
    APP_AUDIO_SET_MESSAGE(audevt, id, status);
    msg.msg_body.message_id = audevt;
    msg.msg_body.message_ptr = ptr;
    msg.msg_body.message_Param0 = param;
    app_mailbox_put(&msg);

    return 0;
}
#else
int app_audio_sendrequest_param(uint16_t id, uint8_t status, uint32_t ptr, uint32_t param)
{
    uint32_t audevt;
    APP_AUDIO_MESSAGE_BLOCK msg;

    AUDIOPLAYERS_TRACE(7,"%s ca=%p type %d%s op %d%s aud %d", __func__, __builtin_return_address(0),
                id, player2str(id), status, audio_op_to_str((enum APP_BT_SETTING_T)status), ptr);

    if(app_audio_init == false)
        return -1;

    msg.mod_id = APP_AUDIO_MODULE_AUDIO;
    APP_AUDIO_SET_MESSAGE(audevt, id, status);
    msg.msg_body.message_id = audevt;
    msg.msg_body.message_ptr = ptr;
    msg.msg_body.message_Param0 = param;
    app_audio_mailbox_put(&msg);

    return 0;
}
#endif

extern bool app_audio_list_playback_exist(void);
#if defined(MEDIA_PLAYER_SUPPORT)&&(!defined(PROMPT_SELF_MANAGEMENT))
static uint8_t app_audio_get_list_playback_num(void)
{
    APP_AUDIO_STATUS *audio_handle = NULL;
    list_node_t *node = NULL;
    uint8_t num=0;
    for (node = list_begin(app_audio_conifg.audio_list); node != list_end(app_audio_conifg.audio_list); node = list_next(node))
    {
        audio_handle = (APP_AUDIO_STATUS *)list_node(node);
        if (audio_handle->id == APP_PLAY_BACK_AUDIO)
            num++;
    }
    return num;
}
#endif

#endif

static bool need_flush_flash_switch_audio = false;
void app_audio_switch_flash_flush_req(void)
{
    uint32_t lock;

    lock = int_lock();
    need_flush_flash_switch_audio = true;
    int_unlock(lock);
}

static void app_audio_switch_flash_proc(void)
{
    uint32_t lock;
    bool need_flush_flash = false;

    lock = int_lock();
    if (need_flush_flash_switch_audio){
        need_flush_flash_switch_audio = false;
        need_flush_flash = true;
    }
    int_unlock(lock);

    if (need_flush_flash){
#ifndef FPGA
        nv_record_flash_flush();
#endif
    }
}

#if defined(USE_BASIC_THREADS)
static int app_audio_handle_process(APP_MESSAGE_BODY *msg_body)
#else
static int app_audio_handle_process(APP_AUDIO_MESSAGE_BODY *msg_body)
#endif
{
    int nRet = 0;

    APP_AUDIO_STATUS aud_status = {0};
    POSSIBLY_UNUSED APP_AUDIO_STATUS next_status = {0};

    if (app_audio_init == false)
        return -1;

    APP_AUDIO_GET_ID(msg_body->message_id, aud_status.id);
    APP_AUDIO_GET_STATUS(msg_body->message_id, aud_status.status);
    APP_AUDIO_GET_AUD_ID(msg_body->message_ptr, aud_status.aud_id);
    APP_AUDIO_GET_FREQ(msg_body->message_Param0, aud_status.freq);

    switch (aud_status.status)
    {
    case APP_BT_SETTING_OPEN:
#ifdef __AUDIO_QUEUE_SUPPORT__
        AUDIOPLAYERS_TRACE(4,
                    "=======>APP_BT_SETTING_OPEN,##before status_id: 0x%x%s, aud_id: 0x%x, len = %d",
                    aud_status.id,
                    player2str(aud_status.id),
                    aud_status.aud_id,
                    list_length(app_audio_conifg.audio_list));

        if (app_audio_list_append(&aud_status))
        {
            app_bt_stream_open(&aud_status);
            AUDIOPLAYERS_TRACE(3,
                        "=======>APP_BT_SETTING_OPEN, ##after status_id: 0x%x%s, len = %d",
                        aud_status.id,
                        player2str(aud_status.id),
                        list_length(app_audio_conifg.audio_list));
            if(aud_status.id == APP_BT_STREAM_HFP_PCM)
            {
                app_bt_media_set_current_media(BT_STREAM_VOICE);
            }
        }
        else if(!app_bt_stream_isrun(APP_PLAY_BACK_AUDIO))
        {
            app_bt_stream_open(&aud_status);
        }
#else
        app_bt_stream_open(&aud_status);
#endif

        break;
    case APP_BT_SETTING_CLOSE:
        app_audio_switch_flash_proc();
#ifdef __AUDIO_QUEUE_SUPPORT__
        AUDIOPLAYERS_TRACE(2,
                    "=======>APP_BT_SETTING_CLOSE, current id: 0x%x%s",
                    aud_status.id,
                    player2str(aud_status.id));
        app_bt_stream_close(aud_status.id);
        app_audio_switch_flash_proc();
#if defined(MEDIA_PLAYER_SUPPORT)&&(!defined(PROMPT_SELF_MANAGEMENT))
        if (aud_status.id == APP_PLAY_BACK_AUDIO)
        {
            AUDIOPLAYERS_TRACE(1, "=======>APP_BT_SETTING_CLOSE, list: %d", app_audio_get_list_playback_num());
            if (app_audio_get_list_playback_num() == 1)
            {
                AUDIOPLAYERS_TRACE(0, "=======>APP_BT_SETTING_CLOSE MEDIA");
                bt_media_stop(BT_STREAM_MEDIA, BT_DEVICE_ID_1, aud_status.aud_id);
            }
            app_prompt_inform_completed_event();
        }
#endif
        if (app_audio_list_rmv_callback(&aud_status, &next_status, APP_BT_SETTING_Q_POS_HEAD))
        {
            AUDIOPLAYERS_TRACE(4,
                        "=======>APP_BT_SETTING_CLOSE, %p, next id: 0x%x%s, status %d",
                        &next_status,
                        next_status.id,
                        player2str(next_status.id),
                        next_status.status);
#ifdef __AI_VOICE__
            if (app_ai_voice_is_need2block_a2dp() &&
                ((APP_BT_STREAM_A2DP_SBC == next_status.id) || (APP_BT_STREAM_A2DP_AAC == next_status.id)))
            {
            }
            else
#endif
            {
                app_bt_stream_open(&next_status);
            }
        }
#else
        app_bt_stream_close(aud_status.id);
        app_audio_switch_flash_proc();
#endif
        break;
    case APP_BT_SETTING_SETUP:
        app_bt_stream_setup(aud_status.id, msg_body->message_ptr);
        break;
    case APP_BT_SETTING_RESTART:
        app_bt_stream_restart(&aud_status);
        break;
    case APP_BT_SETTING_CLOSEALL:
        app_bt_stream_closeall();
#ifdef __AUDIO_QUEUE_SUPPORT__
        app_audio_list_clear();
#endif
#if defined(MEDIA_PLAYER_SUPPORT)
        app_prompt_stop_all();
#endif
        app_audio_switch_flash_proc();
        break;
#if defined(MEDIA_PLAYER_SUPPORT)
    case APP_BT_SETTING_START_PROMPT:
    {
        app_play_audio_onoff(true, &aud_status);
        break;
    }
    case APP_BT_SETTING_STOP_PROMPT:
    {
        app_play_audio_onoff(false, &aud_status);
        app_prompt_inform_completed_event();
        break;
    }
#endif
    case APP_BT_SETTING_START_BLEAUDIO:
        APP_AUDIO_GET_FREQ(msg_body->message_Param0, aud_status.device_id);
        if(!app_audio_list_alredy_exist(&aud_status))
        {
            app_audio_list_append(&aud_status);
        }
        if (g_app_ble_audio_start_cb)
        {
            //when ase_id equal bis enum,it maybe crash.stream_type unused!
            g_app_ble_audio_start_cb((uint8_t)aud_status.device_id, 0, (uint8_t)aud_status.aud_id);
        }
        break;
    case APP_BT_SETTING_STOP_BLEAUDIO:
        APP_AUDIO_GET_FREQ(msg_body->message_Param0, aud_status.device_id);
        app_audio_list_rmv_by_info(&aud_status);
        if (g_app_ble_audio_stop_cb)
        {
            g_app_ble_audio_stop_cb((uint8_t)aud_status.device_id, 0, (uint8_t)aud_status.aud_id);
        }
        break;
    case APP_BT_SETTING_STOP_SINGLE_STREAM:
        APP_AUDIO_GET_FREQ(msg_body->message_Param0, aud_status.device_id);
        if (g_app_ble_audio_stop_single_stream_cb)
        {
            g_app_ble_audio_stop_single_stream_cb((uint8_t)aud_status.device_id, 0, (uint8_t)aud_status.aud_id);
        }
        break;
    case APP_BT_SETTING_OUTPUT_ROUTE:
    {
        app_bt_stream_set_output_route((void *)msg_body->message_ptr);
    } break;
    default:
        break;
    }

    return nRet;
}

#ifdef __AUDIO_QUEUE_SUPPORT__
#if defined(MEDIA_PLAYER_SUPPORT)&&(!defined(PROMPT_SELF_MANAGEMENT))
//go through the audio list to find out the first stream which is not AI_VOICE
static bool app_audio_list_find_next_available_id(uint16_t audio_id, APP_AUDIO_STATUS *audioId)
{
    APP_AUDIO_STATUS *audio_handle = NULL;
    list_node_t *node = NULL;

    for (node = list_begin(app_audio_conifg.audio_list); node != list_end(app_audio_conifg.audio_list); node = list_next(node))
    {
        audio_handle = (APP_AUDIO_STATUS *)list_node(node);
        if (audio_id == audio_handle->id)
        {
            *audioId = *audio_handle;
            return true;
        }
    }

    return false;
}
#endif

static void app_audio_handle_free(void* data)
{
#if defined(MEDIA_PLAYER_SUPPORT)&&(!defined(PROMPT_SELF_MANAGEMENT))
    APP_AUDIO_STATUS * status = (APP_AUDIO_STATUS * )data;

    if(status->id == APP_PLAY_BACK_AUDIO)
    {
        AUDIOPLAYERS_TRACE(2,"=======>app_audio_handle_free playback , aud_id: 0x%x, type = 0x%x", status->aud_id, status->aud_type);
    }
#endif
    osPoolFree (app_audio_status_mempool, data);
}

void app_audio_list_create()
{
    if (app_audio_conifg.audio_list == NULL)
    {
        app_audio_conifg.audio_list = list_new(app_audio_handle_free, NULL, NULL);
    }
}

bool app_audio_list_stream_exist()
{
    APP_AUDIO_STATUS *audio_handle = NULL;
    list_node_t *node = NULL;

    for (node = list_begin(app_audio_conifg.audio_list); node != list_end(app_audio_conifg.audio_list); node = list_next(node)) {

        audio_handle = (APP_AUDIO_STATUS *)list_node(node);

        if (audio_handle->id == APP_BT_STREAM_HFP_PCM ||
            audio_handle->id == APP_BT_STREAM_HFP_CVSD ||
            audio_handle->id == APP_BT_STREAM_HFP_VENDOR ||
            audio_handle->id == APP_BT_STREAM_A2DP_SBC ||
            audio_handle->id == APP_BT_STREAM_A2DP_AAC ||
#ifdef __AI_VOICE__
            audio_handle->id == APP_BT_STREAM_AI_VOICE ||
#endif
#ifdef AUDIO_LINEIN
            audio_handle->id == APP_PLAY_LINEIN_AUDIO ||
#endif
            audio_handle->id == APP_BT_STREAM_A2DP_VENDOR
        ) {
            AUDIOPLAYERS_TRACE(3,"%s id 0x%x%s", __func__, audio_handle->id, player2str(audio_handle->id));
            return true;
        }
    }

    return false;
}

bool app_audio_list_alredy_exist(APP_AUDIO_STATUS* aud_status)
{
    APP_AUDIO_STATUS *audio_handle = NULL;
    list_node_t *node = NULL;
    for (node = list_begin(app_audio_conifg.audio_list);
     node != list_end(app_audio_conifg.audio_list);
     node = list_next(node)) {
        audio_handle = (APP_AUDIO_STATUS *)list_node(node);
        if (aud_status->id == audio_handle->id) {
            return true;
        }
    }
     return false;
}

bool app_audio_list_filter_exist(APP_AUDIO_STATUS* aud_status)
{
#if defined(MEDIA_PLAYER_SUPPORT)&&(!defined(PROMPT_SELF_MANAGEMENT))
    APP_AUDIO_STATUS *audio_handle = NULL;
    list_node_t *node = NULL;
    uint8_t cnt = 0;

    if (aud_status->id != APP_PLAY_BACK_AUDIO){
        return false;
    }

    if(aud_status->aud_id == AUD_ID_BT_CALL_INCOMING_CALL) {
        for (node = list_begin(app_audio_conifg.audio_list);
             node != list_end(app_audio_conifg.audio_list);
             node = list_next(node)) {
            audio_handle = (APP_AUDIO_STATUS *)list_node(node);
            if (audio_handle->id == APP_PLAY_BACK_AUDIO
                && audio_handle->aud_id == AUD_ID_BT_CALL_INCOMING_CALL) {
                AUDIOPLAYERS_TRACE(3,"%s id 0x%x%s", __func__, audio_handle->id, player2str(audio_handle->id));
                return true;
            }
        }
    } else {
        for (node = list_begin(app_audio_conifg.audio_list);
             node != list_end(app_audio_conifg.audio_list);
             node = list_next(node)) {
            audio_handle = (APP_AUDIO_STATUS *)list_node(node);
            if (cnt++ > 1) {
                AUDIOPLAYERS_TRACE(2,"%s cnt %d", __func__, cnt);
                return true;
            }
        }
    }
#endif
    return false;
}

bool app_audio_list_playback_exist(void)
{
#if defined(MEDIA_PLAYER_SUPPORT)&&(!defined(PROMPT_SELF_MANAGEMENT))
    APP_AUDIO_STATUS *audio_handle = NULL;
    list_node_t *node = NULL;

    for (node = list_begin(app_audio_conifg.audio_list);
         node != list_end(app_audio_conifg.audio_list);
         node = list_next(node)) {
        audio_handle = (APP_AUDIO_STATUS *)list_node(node);
#if defined(A2DP_SOURCE_TEST) && defined(APP_LINEIN_A2DP_SOURCE)
        if ((audio_handle->id == APP_PLAY_BACK_AUDIO)
            || (audio_handle->id == APP_A2DP_SOURCE_LINEIN_AUDIO)) {
#else
        if (audio_handle->id == APP_PLAY_BACK_AUDIO) {
#endif
            AUDIOPLAYERS_TRACE(1,"%s", __func__);
            return true;
        }
    }
#endif
    return false;
}

void app_audio_list_playback_clear(void)
{
#if defined(MEDIA_PLAYER_SUPPORT)&&(!defined(PROMPT_SELF_MANAGEMENT))
    APP_AUDIO_STATUS *audio_handle = NULL;
    list_node_t *node = NULL;

    for (node = list_begin(app_audio_conifg.audio_list);
         node != list_end(app_audio_conifg.audio_list);
         node = list_next(node)) {
        audio_handle = (APP_AUDIO_STATUS *)list_node(node);
        if (audio_handle->id == APP_PLAY_BACK_AUDIO){
            list_remove(app_audio_conifg.audio_list, list_node(node));
        }
    }
#endif
}

bool app_audio_list_append(APP_AUDIO_STATUS* aud_status)
{
    APP_AUDIO_STATUS *data_to_append = NULL;
    bool add_data_to_head_of_list = false;
    bool ret = true;

    AUDIOPLAYERS_TRACE(3,"%s id 0x%x%s", __func__, aud_status->id, player2str(aud_status->id));
#if defined(MEDIA_PLAYER_SUPPORT)&&(!defined(PROMPT_SELF_MANAGEMENT))
#if defined(A2DP_SOURCE_TEST) && defined(APP_LINEIN_A2DP_SOURCE)
    if ((aud_status->id == APP_PLAY_BACK_AUDIO) || (aud_status->id == APP_A2DP_SOURCE_LINEIN_AUDIO))
#else
    if (aud_status->id == APP_PLAY_BACK_AUDIO)
#endif
    {
        //ignore redundant ring ind from hfp...
        if (app_audio_list_filter_exist(aud_status))
        {
            return false;
        }

        if (app_audio_list_playback_exist())
        {
            if (list_length(app_audio_conifg.audio_list) >= MAX_AUDIO_BUF_LIST)
            {
                if (app_audio_list_stream_exist())
                {
                    data_to_append = (APP_AUDIO_STATUS *)osPoolCAlloc (app_audio_status_mempool);
                    memcpy(data_to_append, (const void *)list_front(app_audio_conifg.audio_list), sizeof(APP_AUDIO_STATUS));
                    add_data_to_head_of_list = true;
                }

                app_audio_list_clear();
                app_prompt_stop_all();
                AUDIOPLAYERS_TRACE(0,"#####app_audio_list_append error!!! FIXME!!!!\n ");
            }
            else
            {
                ret = false;
            }
        }
    }
    else
#endif
    {
        add_data_to_head_of_list = true;
    }

    if (data_to_append == NULL)
    {
        data_to_append = (APP_AUDIO_STATUS *)osPoolCAlloc (app_audio_status_mempool);
        memcpy(data_to_append, aud_status, sizeof(APP_AUDIO_STATUS));
    }

    if (add_data_to_head_of_list)
    {
        list_prepend(app_audio_conifg.audio_list, (void*)data_to_append);
    }
    else
    {
        list_append(app_audio_conifg.audio_list, (void*)data_to_append);
    }
    if (data_to_append)
    {
        AUDIOPLAYERS_TRACE(7,"%s id 0x%x%s status %d len %d ret %d aud_id %p",
            __func__, data_to_append->id, player2str(data_to_append->id), \
            data_to_append->status, list_length(app_audio_conifg.audio_list),\
            ret, data_to_append);
    }
    return ret;
}

bool app_audio_list_rmv_callback(APP_AUDIO_STATUS *status_close, APP_AUDIO_STATUS *status_next, enum APP_BT_AUDIO_Q_POS pos)
{
    void *data_to_remove = NULL;
    bool ret = false;

    //for status: first bt_a2dp->APP_BT_SETTING_CLOSE,then ring-> APP_BT_SETTING_CLOSE
    AUDIOPLAYERS_TRACE(3, "pre audio list len %d close_id 0x%x%s, pos:%d",
                list_length(app_audio_conifg.audio_list),
                status_close->id,
                player2str(status_close->id), pos);

    if(0 == list_length(app_audio_conifg.audio_list))
    {
        AUDIOPLAYERS_TRACE(0, "no listed audio anymore.");
        return false;
    }

    POSSIBLY_UNUSED APP_AUDIO_STATUS *audio_handle = NULL;
    POSSIBLY_UNUSED list_node_t *node = NULL;
#if defined(MEDIA_PLAYER_SUPPORT)&&(!defined(PROMPT_SELF_MANAGEMENT))
    if (APP_PLAY_BACK_AUDIO == status_close->id)
    {
        // 1. find the first prompt and remove it
        for (node = list_begin(app_audio_conifg.audio_list); node != list_end(app_audio_conifg.audio_list); node = list_next(node))
        {
            audio_handle = (APP_AUDIO_STATUS *)list_node(node);
            AUDIOPLAYERS_TRACE(2, "Aud list, ID:0x%x%s", audio_handle->id, player2str(audio_handle->id));

            if (APP_PLAY_BACK_AUDIO == audio_handle->id)
            {
                data_to_remove = list_node(node);
                list_remove(app_audio_conifg.audio_list, data_to_remove);
                break;
            }
        }

        // 2. find the next audio entry to play
        // 2.1 find the prompt
        ret = app_audio_list_find_next_available_id(APP_PLAY_BACK_AUDIO, status_next);

        // 2.2 if no prompt, find music
        if (!ret)
        {
            ret = app_audio_list_find_next_available_id(APP_BT_STREAM_A2DP_SBC, status_next);
        }

        // 2.3 otherwise, just pop the next one no matter what's it
        if (!ret)
        {
            if (list_length(app_audio_conifg.audio_list) > 0)
            {
                node = list_begin(app_audio_conifg.audio_list);
                if (node)
                {
                    memcpy(status_next, list_node(node), sizeof(APP_AUDIO_STATUS));
                }
                ret = true;
            }
        }

        // 2.4 in case music was not pushed into audio list for corner case,
        //     check whether music is in active media list while call is not
        if (!ret)
        {
            if (bt_media_is_media_active_by_music() &&
                (!bt_media_is_media_active_by_call()))
            {
                AUDIOPLAYERS_TRACE(0, "music in active list but not pushed into audio list for some reason!!!");
                status_next->id = APP_BT_STREAM_A2DP_SBC;
                ret = true;
            }
        }
    }
    else //maybe...a2dp send >> APP_BT_SETTING_CLOSE,  when ring
#endif
    {
        if (app_audio_list_stream_exist())
        {
#ifdef __AI_VOICE__
            if (APP_BT_STREAM_AI_VOICE == status_close->id)
            {
                for (node = list_begin(app_audio_conifg.audio_list);
                     node != list_end(app_audio_conifg.audio_list);
                     node = list_next(node)) {
                    audio_handle = (APP_AUDIO_STATUS *)list_node(node);
                    AUDIOPLAYERS_TRACE(2, "Aud list, ID:0x%x%s", audio_handle->id, player2str(audio_handle->id));

                    if (APP_BT_STREAM_AI_VOICE == audio_handle->id)
                    {
                        data_to_remove = list_node(node);
                    }
                }
            }
            else
#endif
            {
                if (APP_BT_SETTING_Q_POS_HEAD == pos)
                {
                    data_to_remove = list_front(app_audio_conifg.audio_list);
                }
                else if (APP_BT_SETTING_Q_POS_TAIL == pos)
                {
                    data_to_remove = list_back(app_audio_conifg.audio_list);
                }
            }
        }

        if (data_to_remove)
        {
            list_remove(app_audio_conifg.audio_list, data_to_remove);
        }
    }

    AUDIOPLAYERS_TRACE(3,"Updated audio list len = %d ret %d data %p", list_length(app_audio_conifg.audio_list), ret, data_to_remove);
    return ret;
}

void app_audio_list_rmv_by_info(APP_AUDIO_STATUS *info)
{
    AUDIOPLAYERS_TRACE(4, "%s id:0x%x%s list_len:%d", __func__, info->id, player2str(info->id), list_length(app_audio_conifg.audio_list));

    list_node_t *node = NULL, *next = NULL;
    APP_AUDIO_STATUS *nodeInfo = NULL;
    for (node = list_begin(app_audio_conifg.audio_list); node != list_end(app_audio_conifg.audio_list); )
    {
        next = list_next(node);

        nodeInfo = (APP_AUDIO_STATUS *)list_node(node);

        if (nodeInfo)
        {
            AUDIOPLAYERS_TRACE(3, "%s listed id:0x%x, aud_id:%p, list_len:%d", __func__,
                    nodeInfo->id, nodeInfo, list_length(app_audio_conifg.audio_list));
            // TODO: add more info check here
            if ((info->id == nodeInfo->id))
            {
                AUDIOPLAYERS_TRACE(3, "%s remove:0x%x%s", __func__, info->id, player2str(info->id));
                list_remove(app_audio_conifg.audio_list, nodeInfo);
                break;
            }
        }
        else
        {
            AUDIOPLAYERS_TRACE(0, "ERROR: null pointer detected!!!");
            break;
        }

        node = next;
    }
}

void app_audio_list_clear()
{
    list_clear(app_audio_conifg.audio_list);
}
#endif

void app_audio_open(void)
{
    if(app_audio_init)
    {
        return;
    }
    if (g_app_audio_queue_mutex_id == NULL)
    {
        g_app_audio_queue_mutex_id = osMutexCreate((osMutex(g_app_audio_queue_mutex)));
    }
    else
    {
        ASSERT(0, "[%s] ERROR: g_app_audio_queue_mutex_id != NULL", __func__);
    }

    if (app_audio_status_mempool == NULL)
        app_audio_status_mempool = osPoolCreate(osPool(app_audio_status_mempool));
    ASSERT(app_audio_status_mempool, "[%s] ERROR: app_audio_status_mempool != NULL", __func__);

#ifdef __AUDIO_QUEUE_SUPPORT__
    app_audio_list_create();
#endif
#ifdef MEDIA_PLAYER_SUPPORT
    app_prompt_list_init();
#endif

#ifndef PROMPT_SELF_MANAGEMENT
    app_ring_merge_init();
#endif

#if defined(USE_BASIC_THREADS)
    app_set_threadhandle(APP_MODULE_AUDIO, app_audio_handle_process);
#else
    app_audio_thread_init();

    app_audio_set_threadhandle(APP_AUDIO_MODULE_AUDIO, app_audio_handle_process);
#endif

    app_bt_stream_init();

    app_audio_sys_mempool_init();

    app_audio_init = true;
}

void app_audio_close(void)
{
#if defined(USE_BASIC_THREADS)
    app_set_threadhandle(APP_MODULE_AUDIO, NULL);
#else
    app_audio_set_threadhandle(APP_AUDIO_MODULE_AUDIO, NULL);
#endif
    app_audio_init = false;
}

void app_ble_audio_register_start_callback(app_ble_audio_callback_t cb)
{
    g_app_ble_audio_start_cb = cb;
}

void app_ble_audio_register_stop_callback(app_ble_audio_callback_t cb)
{
    g_app_ble_audio_stop_cb = cb;
}

void app_ble_audio_register_stop_single_stream_callback(app_ble_audio_callback_t cb)
{
    g_app_ble_audio_stop_single_stream_cb = cb;
}
