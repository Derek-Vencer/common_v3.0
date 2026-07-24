/***************************************************************************
 *
 * Copyright (c) 2015-2025 BES Technic
 *
 * Authored by BES CD team (Blueelf Prj).
 *
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
#ifndef __CO_LIST_H__
#define __CO_LIST_H__

/**
 * MACRO DEFINES
 *
 *
 */

#define COLIST_MAX_NODES_ON_LIST                (1500)

#define ITER_CHK(count) (count>COLIST_MAX_NODES_ON_LIST?ITERM_ASSERT(count):(void)(count++))

#define NODE_CHK(node)  (NODE_ASSERT(node, false, (uint32_t)(uintptr_t)__builtin_return_address(0), __LINE__))

#define SNODE_CHK(node) (NODE_ASSERT(node, true, (uint32_t)(uintptr_t)__builtin_return_address(0), __LINE__))

#define DEF_LIST_HEAD(head)  struct list_node head = {0, &(head), &(head) }

#define INIT_LIST_HEAD(head) colist_head_node_init(head)

#define DEF_SINGLE_LINK_HEAD(head) \
    struct single_link_head_t head = {0, {(struct single_link_node_t *)&(head)}, (struct single_link_node_t *)&(head), 0}

#define INIT_SINGLE_LINK_HEAD(list) colist_single_link_init(list)

#define colist_structure(ptr, type, member) \
    ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

#define colist_iterate(pos, head) \
    for (int __list__node__iter__cnt = ((pos = (head)->next), 0); \
         pos != (head); \
         (pos = pos->next), ITER_CHK(__list__node__iter__cnt), NODE_CHK(pos))

#define colist_iterate_prev(pos, head) \
    for (int __list__node__iter__cnt = ((pos = (head)->prev), 0); \
         pos != (head); \
         (pos = pos->prev), ITER_CHK(__list__node__iter__cnt), NODE_CHK(pos))

#define colist_iterate_safe(pos, n, head) \
    for (int __list__node__iter__cnt = ((pos = (head)->next), (n = pos->next), 0); \
         pos != (head); \
         (pos = n), (n = pos->next), ITER_CHK(__list__node__iter__cnt), NODE_CHK(pos))

#define colist_iterate_entry(pos, type, head, member) \
    for (int __list__node__iter__cnt = ((pos = colist_structure((head)->next, type, member)), 0); \
         &pos->member != (head); \
         (pos = colist_structure(pos->member.next, type, member)), ITER_CHK(__list__node__iter__cnt), NODE_CHK(pos))

#define colist_iterate_entry_safe(pos, n, type, head, member) \
    for (int __list__node__iter__cnt = ((pos = colist_structure((head)->next, type, member)), \
         (n = colist_structure(pos->member.next, type, member)), 0); \
         &pos->member != (head); \
         (pos = n), (n = colist_structure(n->member.next, type, member)), ITER_CHK(__list__node__iter__cnt), NODE_CHK(pos))

#define colist_single_link_iterate(pos, list) \
    for (uintptr_t __link__node_next_item = ((pos = (list)->head.next), (uintptr_t)(pos->next)), __list__node__iter__cnt = 0; \
         pos != &((list)->head); \
         (pos = (struct single_link_node_t *)__link__node_next_item), \
         (__link__node_next_item = (uintptr_t)(pos->next)), ITER_CHK(__list__node__iter__cnt), SNODE_CHK(pos))

#define colist_single_link_iterate_trace(tag, pos, list) \
    for (uintptr_t __link__node_next_item = ((pos = (list)->head.next), (uintptr_t)(pos->next)), __list__node__iter__cnt = 0; \
         (pos != &((list)->head)) ? (DEBUG_INFO(0, tag " slink_list: %d %d %p ca=%x %d", (list)->size, __list__node__iter__cnt+1, pos, CO_LR_ADDRESS, __LINE__), true) : (false); \
         (pos = (struct single_link_node_t *)__link__node_next_item), \
         (__link__node_next_item = (uintptr_t)(pos->next)), ITER_CHK(__list__node__iter__cnt), SNODE_CHK(pos))

/**
 * DNUMERATIONS
 *
 *
 */

/**
 * TYPEDEFINES
 *
 *
 */

struct list_node
{
    uint32_t valid_pattern;
    struct list_node *next;
    struct list_node *prev;
};

struct single_link_node_t
{
    uint32_t valid_pattern;
    struct single_link_node_t *next;
};

struct single_link_head_t
{
    uint32_t valid_pattern;
    struct single_link_node_t head;
    struct single_link_node_t *tail;
    uint32_t size;
};

/**
 * FUNCTIONS DECLARATIONS
 *
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

void colist_assert(uintptr_t count);
void colist_node_assert(void *node, bool is_snode, uintptr_t ca, uintptr_t ln);
#define ITERM_ASSERT(count) colist_assert(count)
#define NODE_ASSERT(node, is_snode, ca, ln) colist_node_assert(node, is_snode, ca, ln)

/**
 * @brief Double link list
 *
 */
void colist_head_node_init(struct list_node *head);
void colist_addto_head(struct list_node *node, struct list_node *head);
void colist_addto_tail(struct list_node *node, struct list_node *head);
void colist_insert_after(struct list_node *node, struct list_node *head);
void colist_delete(struct list_node *node);
int colist_is_node_on_list(struct list_node *head, struct list_node *node);
int colist_item_count(struct list_node *head);
struct list_node *colist_get_head(struct list_node *head);
int colist_is_list_empty(struct list_node *head);
bool colist_node_valid_check(void *p_node, bool is_snode);
/**
 * @brief Single link list
 *
 */
void colist_single_link_init(struct single_link_head_t *list);
void colist_single_link_push_head(struct single_link_node_t *new_node, struct single_link_head_t *list);
void colist_single_link_check_push_tail(struct single_link_node_t *new_node, struct single_link_head_t *list, bool check_exist);
void colist_single_link_push_tail(struct single_link_node_t *new_node, struct single_link_head_t *list);
void colist_single_link_push_after(struct single_link_node_t *new_node, struct single_link_node_t *exist_node, struct single_link_head_t *list);
void colist_single_link_delete_entry(struct single_link_head_t *list, struct single_link_node_t *entry);
struct single_link_node_t *colist_single_link_delete_entry_after(struct single_link_head_t *list, struct single_link_node_t *entry);
struct single_link_node_t *colist_single_link_delete_head(struct single_link_head_t *list);
int colist_single_link_is_node_on_list(struct single_link_head_t *list, struct single_link_node_t *node);
uint32_t colist_single_link_size(struct single_link_head_t *list);
struct single_link_node_t *colist_single_link_get_head(struct single_link_head_t *list);
struct single_link_node_t *colist_single_link_pop_head(struct single_link_head_t *list);
int colist_single_link_is_list_empty(struct single_link_head_t *list);

#ifdef __cplusplus
}
#endif

#endif /// __CO_LIST_H__