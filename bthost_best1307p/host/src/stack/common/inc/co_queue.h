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
 * @brief xxx.
 *
 ****************************************************************************/
#ifndef __CO_QUEUE_H__
#define __CO_QUEUE_H__
#include "bt_common_define.h"
#if defined(__cplusplus)
extern "C" {
#endif

/****************************** header include ********************************/
#include <stdint.h>

/***************************** external declaration *****************************/

/***************************** macro defination *******************************/
enum {
    CO_QUEUR_OK = 0,
    CO_QUEUR_ERR,
};

/***************************** type defination ********************************/
typedef struct
{
    int read;
    int write;
    int size;
    int len;
    unsigned char *base;
} co_queue_t;

/***************************** variable defination *****************************/

/***************************** function declaration ****************************/
int co_queue_init(co_queue_t *queue, unsigned int size, unsigned char *buf);
int co_queue_length(co_queue_t *queue);
int co_queue_available(co_queue_t *queue);
int co_queue_enqueue(co_queue_t *queue, unsigned char *data, unsigned int len);
int co_queue_dequeue(co_queue_t *queue, unsigned char *data, unsigned int len);
void co_queue_reset(co_queue_t *queue);

#if defined(__cplusplus)
}
#endif
#endif /* __CO_QUEUE_H__ */
