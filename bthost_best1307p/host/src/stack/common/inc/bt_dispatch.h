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
#ifndef __BT_DISPATCH_H__
#define __BT_DISPATCH_H__

/**
 * MACRO DEFINES
 *
 * @warning Should not change these macro defines
 */

#define BT_DONT_CALL_FUNC_IN_BT_THREAD      (0x01)
#define BT_CALL_FUNC_BY_MAIL_IN_BT_THREAD   (0x02)
#define BT_SYNC_CALL_FUNC_IN_OTHER_THREAD   (0x04)
#define BT_DELAY_CALL_FUNC_IN_BT_THREAD     (0x08)

#define BT_DELAY_CALL_TIME_U24_MAX          (0x00FFFFFF)

/**
 * DNUMERATIONS
 *
 *
 */

/**
 * @enum bt_disaptch_call_flags
 * @brief Defines function dispatching methods for the BT thread context.
 *
 * This enumeration specifies how a target function should be executed
 * depending on the current thread context. It is typically used with
 * bt_defer_curr_func_X_impl() and related deferred execution mechanisms.
 */
enum bt_disaptch_call_flags
{
    /**
     * @brief Forbids calling the function directly outside the BT thread.
     *        The call is only valid when executed within the BT thread and
     *        return true. and The call Will be ignored if invoked from another
     *        thread and return true.
     */
    BT_DISPATCH_CALL_FLAG_BT_CHECK_CALL = BT_DONT_CALL_FUNC_IN_BT_THREAD,

    /**
     * @brief Executes the function asynchronously in the BT thread
     *        via the internal mailbox mechanism.
     *        Suitable for cross-thread calls that don’t need immediate execution.
     */
    BT_DISPATCH_CALL_FLAG_BT_MAIL_CALL = BT_CALL_FUNC_BY_MAIL_IN_BT_THREAD,

    /**
     * @brief Executes the function in another thread under semaphore protection.
     *        Used when accessing shared resources across threads, the call is excuted
     *        in the BT thread and sem wait in caller thread.
     */
    BT_DISPATCH_CALL_FLAG_BT_SYNC_CALL = BT_SYNC_CALL_FUNC_IN_OTHER_THREAD,

    /**
     * @brief Schedules the function for delayed execution in the BT thread.
     *        Typically used for timer-based or deferred tasks.
     */
    BT_DISPATCH_CALL_FLAG_BT_DELAY_CALL = BT_DELAY_CALL_FUNC_IN_BT_THREAD,

    /**
     * @brief Reserved for future use. Mask value: 0xFFF0.
     */
    BT_DISPATCH_CALL_FLAG_RFU_MASK = 0xFFF0,
};

/**
 * TYPEDEFINES
 *
 *
 */

/**
 * @struct bt_defer_param_t
 * @brief Represents a parameter used in deferred BT thread function calls.
 *
 * This structure is used to describe how a parameter is passed to a deferred
 * function execution (e.g., through bt_defer_curr_func_X_impl()). It can
 * either be a fixed value or a dynamically allocated data buffer.
 */
struct bt_defer_param_t
{
    /**
     * @brief Indicates whether the parameter is a fixed (immediate) value.
     *
     * - true: The parameter is a fixed value stored in @ref param.
     * - false: The parameter points to dynamically allocated memory
     *           whose size is specified by @ref alloc_size.
     */
    bool is_fixed_param;

    /**
     * @brief Size (in bytes) of the allocated data if @ref is_fixed_param is false.
     *
     * Ignored when @ref is_fixed_param is true.
     */
    uint16_t alloc_size;

    /**
     * @brief Parameter value or pointer, depending on @ref is_fixed_param.
     *
     * - When @ref is_fixed_param is true, this field stores a 32-bit value directly.
     * - When false, this field stores the address of an allocated data buffer.
     */
    uint32_t param;
};

/**
 * FUNCTIONS DECLARATIONS
 *
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************
 * Internal Deferred Function Implementations (Deprecated)
 *
 * The following functions are low-level internal APIs for deferred function
 * execution in the BT thread. They are kept for backward compatibility
 * but are **not recommended for general use** due to tricky usage patterns.
 *
 * Please use the higher-level macros defined below instead.
 **********************************************************************************/

/**
 * @brief Generate a fixed defer parameter object.
 *
 * This function creates a `bt_defer_param_t` structure based on the provided
 * fixed parameter value. It is typically used to wrap constant or immutable
 * parameters into a standard deferred-call parameter type, allowing them
 * to be passed through the thread RPC / deferred execution mechanism safely.
 *
 * @param[in] param  Fixed 32-bit value to be encapsulated into a `bt_defer_param_t` structure.
 *
 * @return A `bt_defer_param_t` instance that encapsulates the given parameter.
 *
 * @note The returned structure can be directly passed to defer-related APIs such as
 *       `bt_exec_async()`.
 *
 * @see bt_defer_param_t
 * @see bt_exec_async
 */
struct bt_defer_param_t bt_fixed_param_impl(uint32_t param);

/**
 * @brief  Allocate and wrap a data buffer as a deferred task parameter.
 *
 * This function allocates a new `bt_defer_param_t` object and copies data from
 * the specified buffer into it. The resulting parameter can then be passed to
 * deferred execution APIs such as `bt_exec_async()`.
 *
 * It is typically used when the deferred function needs access to a specific
 * data payload rather than a simple constant value.
 *
 * @param[in] data_ptr     Pointer to the source data buffer.
 * @param[in] alloc_size   Size (in bytes) of the data to copy and encapsulate.
 *
 * @return A `bt_defer_param_t` instance that owns a dynamically allocated
 *         buffer containing a copy of the input data.
 *
 * @note The memory associated with the returned parameter will be automatically
 *       released by the defer framework after the deferred function completes.
 *
 * @see bt_defer_param_t
 * @see bt_fixed_param_impl
 * @see bt_exec_async
 */
struct bt_defer_param_t bt_alloc_param_size(const void *data_ptr, uint16_t alloc_size);

/**
 * @brief  Defer execution of a function with no parameters in the current context.
 *
 * This function schedules a zero-parameter callback (`func`) to be executed
 * asynchronously within the defer framework, typically in the context of the
 * Bluetooth task thread or RPC task.
 *
 * @param[in] func   Function pointer to be executed later.
 *                   The function must match the expected defer callback prototype.
 * @param[in] flags  Control flags specifying the defer behavior
 *                   (e.g., immediate, queued, high-priority, etc.).
 *
 * @return A task or message ID representing the deferred execution request.
 *
 * @note This is the zero-parameter version of the internal defer API series.
 *
 * @see bt_execute_func_1_impl
 * @see bt_defer_param_t
 */
uint32_t bt_execute_func_0_impl(uintptr_t func, uint32_t flags);

/**
 * @brief Defer execution of a function with 1 parameter to the BT thread.
 *
 * @param func   Pointer to the function to be called (uintptr_t cast).
 * @param param0 Deferred parameter, see @ref bt_defer_param_t.
 * @param flags  Dispatch behavior flags, see @ref bt_disaptch_call_flags.
 *
 * @return Execution result or status code depending on implementation.
 */
uint32_t bt_execute_func_1_impl(uintptr_t func, struct bt_defer_param_t param0, uint32_t flags);

/**
 * @brief Defer execution of a function with 2 parameters to the BT thread.
 *
 * @param func   Pointer to the function to be called (uintptr_t cast).
 * @param param0 First deferred parameter.
 * @param param1 Second deferred parameter.
 * @param flags  Dispatch behavior flags, see @ref bt_disaptch_call_flags.
 *
 * @return Execution result or status code depending on implementation.
 */
uint32_t bt_execute_func_2_impl(uintptr_t func, struct bt_defer_param_t param0, struct bt_defer_param_t param1, uint32_t flags);

/**
 * @brief Defer execution of a function with 3 parameters to the BT thread.
 *
 * @param func   Pointer to the function to be called (uintptr_t cast).
 * @param param0 First deferred parameter.
 * @param param1 Second deferred parameter.
 * @param param2 Third deferred parameter.
 * @param flags  Dispatch behavior flags, see @ref bt_disaptch_call_flags.
 *
 * @return Execution result or status code depending on implementation.
 */
uint32_t bt_execute_func_3_impl(uintptr_t func, struct bt_defer_param_t param0, struct bt_defer_param_t param1, struct bt_defer_param_t param2,
                                uint32_t flags);

/**
 * @brief Defer execution of a function with 4 parameters to the BT thread.
 *
 * @param func   Pointer to the function to be called (uintptr_t cast).
 * @param param0 First deferred parameter.
 * @param param1 Second deferred parameter.
 * @param param2 Third deferred parameter.
 * @param param3 Fourth deferred parameter.
 * @param flags  Dispatch behavior flags, see @ref bt_disaptch_call_flags.
 *
 * @return Execution result or status code depending on implementation.
 */
uint32_t bt_execute_func_4_impl(uintptr_t func, struct bt_defer_param_t param0, struct bt_defer_param_t param1, struct bt_defer_param_t param2,
                                struct bt_defer_param_t param3, uint32_t flags);

/**
 * @brief Set bt sync call wait timeout for semaphore
 *
 * @param[in] sem_wait_to_ms Semaphore aqucire timeout ms
 *
 */
void bt_disaptch_set_sem_timeout(uint32_t sem_wait_to_ms);

/**********************************************************************************
 * Recommended Deferred Call Macros
 *
 * Use these macros instead of the internal *_impl functions for a safer,
 * clearer, and maintainable API.
 *
 * - bt_exec_async_X: Non-blocking deferred call (false, no return value)
 * - bt_exec_sync_X: Blocking call with return value; locks the current thread
 **********************************************************************************/
#define bt_fixed_param(param)\
        bt_fixed_param_impl((uint32_t)(uintptr_t)(param))
#define bt_alloc_param(value_ptr)\
        bt_alloc_param_size((value_ptr), sizeof(*(value_ptr)))

#define bt_exec_sync_0(func)\
        bt_execute_func_0_impl((uintptr_t)(func), BT_SYNC_CALL_FUNC_IN_OTHER_THREAD)
#define bt_exec_sync_1(func,a)\
        bt_execute_func_1_impl((uintptr_t)(func),(a), BT_SYNC_CALL_FUNC_IN_OTHER_THREAD)
#define bt_exec_sync_2(func,a,b)\
        bt_execute_func_2_impl((uintptr_t)(func),(a),(b), BT_SYNC_CALL_FUNC_IN_OTHER_THREAD)
#define bt_exec_sync_3(func,a,b,c)\
        bt_execute_func_3_impl((uintptr_t)(func),(a),(b),(c), BT_SYNC_CALL_FUNC_IN_OTHER_THREAD)
#define bt_exec_sync_4(func,a,b,c,d)\
        bt_execute_func_4_impl((uintptr_t)(func),(a),(b),(c),(d), BT_SYNC_CALL_FUNC_IN_OTHER_THREAD)

#define bt_exec_async_0(post, func)\
        do { (void)bt_execute_func_0_impl((uintptr_t)(func), ((post == false) ? 0x00 : BT_CALL_FUNC_BY_MAIL_IN_BT_THREAD)); } while (0)
#define bt_exec_async_1(post, func,a)\
        do { (void)bt_execute_func_1_impl((uintptr_t)(func),(a), ((post == false) ? 0x00 : BT_CALL_FUNC_BY_MAIL_IN_BT_THREAD)); } while (0)
#define bt_exec_async_2(post, func,a,b)\
        do { (void)bt_execute_func_2_impl((uintptr_t)(func),(a),(b), ((post == false) ? 0x00 : BT_CALL_FUNC_BY_MAIL_IN_BT_THREAD)); } while (0)
#define bt_exec_async_3(post, func,a,b,c)\
        do { (void)bt_execute_func_3_impl((uintptr_t)(func),(a),(b),(c), ((post == false) ? 0x00 : BT_CALL_FUNC_BY_MAIL_IN_BT_THREAD)); } while (0)
#define bt_exec_async_4(post, func,a,b,c,d)\
        do { (void)bt_execute_func_4_impl((uintptr_t)(func),(a),(b),(c),(d), ((post == false) ? 0x00 : BT_CALL_FUNC_BY_MAIL_IN_BT_THREAD)); } while (0)

/**
 * @note Deprecated Interfaces
 *
 * The following APIs (`bt_defer_curr_func_X_impl`) are planned for deprecation.
 * Although their design allows flexible thread context switching, the usage
 * pattern is somewhat tricky and **not recommended for general developers**.
 *
 * Please migrate to one of the following clearer alternatives:
 * - **bt_exec_async_X** — Non-blocking deferred call (false, no return value).
 * - **bt_exec_sync_X** — Blocking call with return value; locks the
 *   current thread until execution completes.
 *
 * These new interfaces provide a more intuitive and maintainable way to
 * perform thread-safe function dispatching in the BT thread.
 */
#define bt_defer_curr_func_0(func)\
        bt_execute_func_0_impl((uintptr_t)(func), BT_DONT_CALL_FUNC_IN_BT_THREAD)
#define bt_defer_curr_func_1(func,a)\
        bt_execute_func_1_impl((uintptr_t)(func),(a), BT_DONT_CALL_FUNC_IN_BT_THREAD)
#define bt_defer_curr_func_2(func,a,b)\
        bt_execute_func_2_impl((uintptr_t)(func),(a),(b), BT_DONT_CALL_FUNC_IN_BT_THREAD)
#define bt_defer_curr_func_3(func,a,b,c)\
        bt_execute_func_3_impl((uintptr_t)(func),(a),(b),(c), BT_DONT_CALL_FUNC_IN_BT_THREAD)
#define bt_defer_curr_func_4(func,a,b,c,d)\
        bt_execute_func_4_impl((uintptr_t)(func),(a),(b),(c),(d), BT_DONT_CALL_FUNC_IN_BT_THREAD)

#define bt_delay_call_func_0(func,delay_ms_u24)\
        do { (void)bt_execute_func_0_impl((uintptr_t)(func), \
            (CO_MIN_VALUE(delay_ms_u24, BT_DELAY_CALL_TIME_U24_MAX) << 8) | BT_DELAY_CALL_FUNC_IN_BT_THREAD); } while (0)
#define bt_delay_call_func_1(func,delay_ms_u24,a)\
        do { (void)bt_execute_func_1_impl((uintptr_t)(func),(a), \
            (CO_MIN_VALUE(delay_ms_u24, BT_DELAY_CALL_TIME_U24_MAX) << 8) | BT_DELAY_CALL_FUNC_IN_BT_THREAD); } while (0)
#define bt_delay_call_func_2(func,delay_ms_u24,a,b)\
        do { (void)bt_execute_func_2_impl((uintptr_t)(func),(a),(b), \
            (CO_MIN_VALUE(delay_ms_u24, BT_DELAY_CALL_TIME_U24_MAX) << 8) | BT_DELAY_CALL_FUNC_IN_BT_THREAD); } while (0)
#define bt_delay_call_func_3(func,delay_ms_u24,a,b,c)\
        do { (void)bt_execute_func_3_impl((uintptr_t)(func),(a),(b),(c), \
            (CO_MIN_VALUE(delay_ms_u24, BT_DELAY_CALL_TIME_U24_MAX) << 8) | BT_DELAY_CALL_FUNC_IN_BT_THREAD); } while (0)
#define bt_delay_call_func_4(func,delay_ms_u24,a,b,c,d)\
        do { (void)bt_execute_func_4_impl((uintptr_t)(func),(a),(b),(c),(d), \
            (CO_MIN_VALUE(delay_ms_u24, BT_DELAY_CALL_TIME_U24_MAX) << 8) | BT_DELAY_CALL_FUNC_IN_BT_THREAD); } while (0)

#ifdef __cplusplus
}
#endif

#endif /// __BT_DISPATCH_H__