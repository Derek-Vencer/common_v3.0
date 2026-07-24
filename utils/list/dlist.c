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
#include "stdio.h"
#include "hal_trace.h"
#include "string.h"
#include "list.h"

dlist_t *dlist_new(list_free_cb callback, list_mempool_zmalloc zmalloc, list_mempool_free free)
{
    if (!zmalloc) {
        return NULL;
    }

    dlist_t *list = (dlist_t *)zmalloc(sizeof(dlist_t));
    if (list) {
        sys_dlist_init(&list->dlist);
        list->free_cb = callback;
        list->mempool_functions.zmalloc = zmalloc;
        list->mempool_functions.free = free;
    }
    return list;
}

void dlist_clear(dlist_t *list)
{
    ASSERT(list != NULL, "%s", __func__);
    sys_dnode_t *node;

    while ((node = sys_dlist_get(&list->dlist)) != NULL) {
        if (list->free_cb) {
            list->free_cb(node->data);
        }
        if (list->mempool_functions.free) {
            list->mempool_functions.free(node);
        }
        --list->length;
    }
    list->length = 0;
}

void dlist_free(dlist_t *list)
{
    if (list) {
        dlist_clear(list);
        if (list->mempool_functions.free) {
            list->mempool_functions.free(list);
        }
    }
}

bool dlist_remove(dlist_t *list, void *data)
{
    if (!list || !data) {
        return false;
    }
	sys_dnode_t *node;

  	SYS_DLIST_FOR_EACH_NODE(&list->dlist, node){
		if (node->data == data){
			sys_dlist_remove(node);
            if (list->free_cb) {
                list->free_cb(node->data);
            }
            if (list->mempool_functions.free) {
                list->mempool_functions.free(node);
            }
            --list->length;
			return true;
		}
	}
    return false;
}

bool dlist_append(dlist_t *list, void *data)
{
    if (!list || !data) {
        return false;
    }
    sys_dnode_t *node = NULL;

    if (list->mempool_functions.zmalloc) {
        node = (sys_dnode_t *)list->mempool_functions.zmalloc(sizeof(sys_dnode_t));
    }

    if (!node) {
        return false;
    }

    node->data = data;
    sys_dlist_append(&list->dlist, node);
    ++list->length;
    return true;
}

bool dlist_prepend(dlist_t *list, void *data)
{
    if (!list || !data) {
        return false;
    }
    sys_dnode_t *node = NULL;

    if (list->mempool_functions.zmalloc) {
        node = (sys_dnode_t *)list->mempool_functions.zmalloc(sizeof(sys_dnode_t));
    }

    if (!node) {
        return false;
    }

    node->data = data;
    sys_dlist_prepend(&list->dlist, node);
    ++list->length;
    return true;
}

void *dlist_node(const sys_dnode_t *node)
{
	return (node != NULL) ? node->data : NULL;
}

sys_dnode_t *dlist_begin(const dlist_t *list)
{
    return (list != NULL) ? sys_dlist_peek_head((sys_dlist_t *)&list->dlist) : NULL;
}

sys_dnode_t *dlist_end(const dlist_t *list)
{
    return (list != NULL) ? sys_dlist_peek_tail((sys_dlist_t *)&list->dlist) : NULL;
}

sys_dnode_t *dlist_next(const dlist_t *list, const sys_dnode_t *node)
{
    return (list != NULL && node != NULL) ? sys_dlist_peek_next((sys_dlist_t *)&list->dlist, (sys_dnode_t *)node) : NULL;
}

sys_dnode_t *dlist_prev(const dlist_t *list, const sys_dnode_t *node)
{
    return (list != NULL && node != NULL) ? sys_dlist_peek_prev((sys_dlist_t *)&list->dlist, (sys_dnode_t *)node) : NULL;
}

size_t dlist_length(const dlist_t *list)
{
    if (!list) {
        return 0;
    }

    size_t len = sys_dlist_len((sys_dlist_t *)&list->dlist);
    if (list->length != len)
        LIST_TRACE(1, "%s %d!=%d", __func__, list->length, len);
    return len;
}