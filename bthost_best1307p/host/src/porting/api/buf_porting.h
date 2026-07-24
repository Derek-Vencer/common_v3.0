/***************************************************************************
 *
 * Copyright 2015-2025 BES.
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
#ifndef __BUF_PORTING_H__
#define __BUF_PORTING_H__

#if defined(__cplusplus)
extern "C" {
#endif

/****************************************************/
/*                                                  */
/*             STACK BUFFER PORTING                 */
/*                                                  */
/****************************************************/

/**
 * @brief BT STACK buffer init
 *
 */
void bt_stack_buf_init(void);

/**
 * @brief BT STACK buffer deinit
 *
 */
void bt_stack_buf_deinit(void);

/**
 * @brief BT STACK buffer malloc
 *
 * @param[in] size     Buffer size
 * @param[in] ca       Caller func ptr
 * @param[in] line     Caller func line
 *
 * @return unsigned char*...
 */
unsigned char *bt_stack_buf_malloc_with_ca(uint16 size, uint32 ca, uint32 line);

/**
 * @brief BT STACK buffer free
 *
 * @param[in] buf      Buffer ptr
 * @param[in] ca       Caller func ptr
 * @param[in] line     Caller func line
 *
 */
void bt_stack_buf_free_with_ca(unsigned char *buf, uint32 ca, uint32 line);

/**
 * @brief BT STACK buffer check buffer the size of is available for malloc
 *
 * @param[in] size     Buffer size
 *
 * @return bool        Available or not
 */
bool bt_stack_buf_check_buffer_available(int size);

/**
 * @brief BT STACK buffer check buffers the size of is available count
 *
 * @param[in] size     Buffer size
 *
 * @return int         Available buffer count
 */
int bt_stack_buf_count_available_buffers(int size);

/**
 * @brief BT STACK buffer print usage statistics
 *
 */
void bt_stack_buf_print_statistic(void);

/**
 * @brief BT STACK buffer enable debug
 *
 * @param[in] enable   Enable or disable
 *
 */
void bt_stack_buf_enable_debug(bool enable);


/****************************************************/
/*                                                  */
/*               APP BUFFER PORTING                 */
/*                                                  */
/****************************************************/

/**
 * @brief BT APP buffer init
 *
 */
void bt_app_buf_init(void);

/**
 * @brief BT APP buffer deinit
 *
 */
void bt_app_buf_deinit(void);

/**
 * @brief BT APP buffer malloc
 *
 * @param[in] size     Buffer size
 * @param[in] ca       Caller func ptr
 * @param[in] line     Caller func line
 *
 * @return unsigned char*...
 */
unsigned char *bt_app_buf_malloc(uint16 size, uint32 ca, uint32 line);

/**
 * @brief BT APP buffer free
 *
 * @param[in] buf      Buffer ptr
 * @param[in] ca       Caller func ptr
 * @param[in] line     Caller func line
 *
 */
void bt_app_buf_free(unsigned char *buf, uint32 ca, uint32 line);

/**
 * @brief BT APP buffer check buffer the size of is available for malloc
 *
 * @param[in] size     Buffer size
 *
 * @return bool        Available or not
 */
bool bt_app_check_buf_available(uint16_t size);

#if defined(__cplusplus)
}
#endif
#endif /* __BUF_PORTING_H__ */
