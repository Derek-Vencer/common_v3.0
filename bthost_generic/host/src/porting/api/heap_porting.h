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
#ifndef __HEAP_PORTING_H__
#define __HEAP_PORTING_H__

#if defined(__cplusplus)
extern "C" {
#endif

struct bt_heap_global_t
{
    void *bt_heap_b_handle;
    void *bt_heap_s_handle;
    uint8_t *bt_heap_b_pool;
    uint8_t *bt_heap_s_pool;
    uint16_t bt_heap_max_sml_size;
    uint16_t bt_heap_max_big_size;
    uint16_t bt_heap_big_low_limit;
    uint16_t bt_heap_big_high_limit;
    bool bt_heap_enable_debug;
    uint16_t peak_size_b_pool;
    uint16_t peak_size_s_pool;
};

struct bt_heap_buff_header_t
{
    uint16_t buf_seqn;
    uint16_t alloc_ln;
    uint32_t alloc_ca;
};

struct bt_heap_block_t
{
    intptr_t header;
    union
    {
        struct bt_heap_block_t *next_free;
        uint8_t data[1];
    };
};

struct bt_heap_header_t
{
    void *lock;
    size_t total_bytes;
    size_t free_bytes;
    size_t minimum_free_bytes;
    struct bt_heap_block_t *last_block;
    struct bt_heap_block_t first_block; /* initial 'free block', never allocated */
};

struct bt_heap_buff_t;

/// COHEAP API
void bt_heap_init(struct bt_heap_global_t *bt_heap);

void bt_heap_deinit(struct bt_heap_global_t *bt_heap);

bool bt_heap_check_malloc_available(struct bt_heap_global_t *bt_heap, uint16_t size);

void *bt_heap_get_heap_from_buffer(struct bt_heap_global_t *bt_heap, struct bt_heap_buff_t *buffer);

uint32_t bt_heap_lock(struct bt_heap_header_t *heap);

void bt_heap_unlock(struct bt_heap_header_t *heap, uint32_t flags);

struct bt_heap_buff_t *bt_heap_bt_malloc_with_ca(struct bt_heap_global_t *bt_heap, uint16_t size, uint32_t ca, uint32_t line, bool allow_fail_alloc);

void bt_heap_free_with_ca(struct bt_heap_global_t *bt_heap, struct bt_heap_buff_t *p, uint32_t ca, uint32_t line);

void bt_heap_dump_statistics(struct bt_heap_global_t *bt_heap);

void bt_heap_enable_debug(struct bt_heap_global_t *bt_heap, bool enable);

#if defined(__cplusplus)
}
#endif
#endif /* __HEAP_PORTING_H__ */