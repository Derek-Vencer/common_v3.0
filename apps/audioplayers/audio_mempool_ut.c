/***************************************************************************
 *
 * Copyright 2015-2026 BES.
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
#include "app_audio.h"
#include "hal_aud.h"
#include "hal_trace.h"
#include "audio_mempool_ut.h"

#ifdef AUDIO_MEMPOOL_DEBUG
#define LOG_LINE(str) APP_TEST_TRACE(0, "%s", str)

static void audio_mempool_ut_help(void)
{
    LOG_LINE("mempool cmds:");
    LOG_LINE("  mempool reset");
    LOG_LINE("  mempool reg <user> <size> <sco> <a2dp> <remain>");
    LOG_LINE("  mempool layout");
    LOG_LINE("  mempool len <user>");
    LOG_LINE("  mempool alloc <user> <size>");
    LOG_LINE("  mempool alloc_loop <user> <size>");
    LOG_LINE("  mempool free <user>");
    LOG_LINE("  mempool stress <user> <size> <cnt>");
    LOG_LINE("  mempool dump");
    LOG_LINE("  mempool verify");
}

static void audio_mempool_ut_reset(const char *cmd)
{
    (void)cmd;
    app_audio_mempool_reset();
    APP_TEST_TRACE(0, "[UT][MP] reset done");
}

static void audio_mempool_ut_reg(const char *cmd)
{
    int u;
    uint32_t size;
    int sco, a2dp, remain;

    if (sscanf(cmd, "%*s %d %u %d %d %d",
               &u, &size, &sco, &a2dp, &remain) != 5) {
        APP_TEST_TRACE(0, "usage: mempool reg <user> <size> <sco> <a2dp> <remain>");
        return;
    }

    if (u <= SYSPOOL_USER_NONE || u >= SYSPOOL_USER_QTY) {
        APP_TEST_TRACE(0, "invalid user %d", u);
        return;
    }

    app_mempool_reg_t r = {
        .user        = (SYSPOOL_USER_t)u,
        .size        = size,
        .sco_cowork  = sco ? true : false,
        .a2dp_cowork = a2dp ? true : false,
        .use_remain  = remain ? true : false,
    };

    int ret = app_audio_mempool_register(&r);
    APP_TEST_TRACE(0,
        "[UT][MP] reg user=%d size=%u sco=%d a2dp=%d remain=%d ret=%d",
        u, size, sco, a2dp, remain, ret);
}

static void audio_mempool_ut_layout(void)
{
    int ret = app_audio_mempool_build_ut_layout();
    APP_TEST_TRACE(0,"build_layout ret=%d", ret);

    for (int u = 0; u < SYSPOOL_USER_QTY; u++) {
        uint32_t len = app_audio_mempool_len(u);
        if (len)
            APP_TEST_TRACE(0,"user[%d] total=%u", u, len);
    }
}

static void audio_mempool_ut_len(const char *cmd)
{
    int u;
    sscanf(cmd, "%*s %d", &u);
    APP_TEST_TRACE(0,"user[%d] total=%u free=%u", u, app_audio_mempool_len(u), app_audio_mempool_get_user_free_size(u));
}

static void audio_mempool_ut_alloc(const char *cmd)
{
    int u;
    uint32_t size;
    sscanf(cmd, "%*s %d %u", &u, &size);

    void *p = app_audio_mempool_user_alloc(u, size);
    APP_TEST_TRACE(0, "alloc user=%d size=%u ptr=%p free=%u", u, size, p, app_audio_mempool_get_user_free_size(u));
}

static void audio_mempool_ut_alloc_loop(const char *cmd)
{
    int u;
    uint32_t size;
    sscanf(cmd, "%*s %d %u", &u, &size);

    int cnt = 0;
    while (app_audio_mempool_user_alloc(u, size))
        cnt++;

    APP_TEST_TRACE(0, "alloc_loop user=%d size=%u cnt=%d free=%u", u, size, cnt, app_audio_mempool_get_user_free_size(u));
}

static void audio_mempool_ut_free(const char *cmd)
{
    int u;
    sscanf(cmd, "%*s %d", &u);

    app_audio_mempool_user_free(u);

    APP_TEST_TRACE(0, "free user=%d free=%u", u, app_audio_mempool_get_user_free_size(u));
}

static void audio_mempool_ut_stress(const char *cmd)
{
    int u, cnt;
    uint32_t size;
    sscanf(cmd, "%*s %d %u %d", &u, &size, &cnt);

    for (int i = 0; i < cnt; i++) {
        if (!app_audio_mempool_user_alloc(u, size)) {
            APP_TEST_TRACE(0,"stress fail at %d", i);
            break;
        }
    }

    APP_TEST_TRACE(0, "stress done free=%u", app_audio_mempool_get_user_free_size(u));
}

static void audio_mempool_ut_verify(void)
{
    for (int u = 0; u < SYSPOOL_USER_QTY; u++) {
        uint32_t total = app_audio_mempool_len(u);
        if (!total)
            continue;

        app_audio_mempool_user_free(u);

        uint32_t free = app_audio_mempool_get_user_free_size(u);
        if (free != total) {
            APP_TEST_TRACE(0, "VERIFY FAIL user=%d total=%u free=%u", u, total, free);
            return;
        }
    }

    APP_TEST_TRACE(0,"VERIFY PASS");
}

static void audio_mempool_ut_dump(void)
{
#ifdef AUDIO_MEMPOOL_DEBUG
    app_audio_mempool_debug_dump();
#else
    APP_TEST_TRACE(0,"AUDIO_MEMPOOL_DEBUG not enabled");
#endif
}

void audio_mempool_ut_cmd(const char *cmd)
{
    char sub[16] = {0};
    sscanf(cmd, "%15s", sub);

    if (!strcmp(sub, "help"))        audio_mempool_ut_help();
    else if (!strcmp(sub, "reg"))    audio_mempool_ut_reg(cmd);
    else if (!strcmp(sub, "layout")) audio_mempool_ut_layout();
    else if (!strcmp(sub, "len"))    audio_mempool_ut_len(cmd);
    else if (!strcmp(sub, "alloc"))  audio_mempool_ut_alloc(cmd);
    else if (!strcmp(sub, "alloc_loop")) audio_mempool_ut_alloc_loop(cmd);
    else if (!strcmp(sub, "free"))   audio_mempool_ut_free(cmd);
    else if (!strcmp(sub, "reset"))  audio_mempool_ut_reset(cmd);
    else if (!strcmp(sub, "stress")) audio_mempool_ut_stress(cmd);
    else if (!strcmp(sub, "dump"))   audio_mempool_ut_dump();
    else if (!strcmp(sub, "verify")) audio_mempool_ut_verify();
    else APP_TEST_TRACE(0,"unknown mempool cmd");
}
#else
void audio_mempool_ut_cmd(const char *cmd) { (void)cmd; }
#endif