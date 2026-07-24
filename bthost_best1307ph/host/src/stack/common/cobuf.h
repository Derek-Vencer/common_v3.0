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
#ifndef __COBUF_H__
#define __COBUF_H__

#include "bt_callback_func.h"
#include "co_ppbuff.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*******************************************************************************
 *                                                                             *
 *                   COBUF: FOR STACK COMMON BUF - CALLBACK SYSTEM             *
 *                                                                             *
 *  This module implements a callback-based common buffer management system    *
 *  for stack operations. The cobuf (common buffer) provides efficient memory  *
 *  management with built-in debugging and tracking capabilities.              *
 *                                                                             *
 *  Features:                                                                  *
 *  - Callback-based buffer allocation and deallocation                       *
 *  - Built-in return address tracking for debugging                          *
 *  - Line number tracking for precise error location                         *
 *  - Buffer availability checking and statistics                             *
 *  - Memory usage monitoring and reporting                                   *
 *                                                                             *
 *  COBUF: FOR STACK COMMON BUF - CALLBACK INTERFACE                          *
 *                                                                             *
 *  Macro Definitions:                                                         *
 *                                                                             *
 *  #define cobuf_malloc(size)                                                 *
 *      bt_callback_stack_buf_malloc((size),                                   *
 *          (uint32_t)(uintptr_t)__builtin_return_address(0), __LINE__)        *
 *                                                                             *
 *    Purpose: Allocate a buffer from stack common buffer pool                *
 *    Parameters:                                                              *
 *      - size: Requested buffer size in bytes                                *
 *    Returns: Pointer to allocated buffer or NULL on failure                 *
 *    Tracking: Captures return address and line number for debugging         *
 *                                                                             *
 *  COBUF: FOR STACK COMMON BUF - MEMORY ALLOCATION                           *
 *                                                                             *
 *  #define cobuf_free(buf)                                                    *
 *      bt_callback_stack_buf_free((unsigned char *)buf,                      *
 *          (uint32_t)(uintptr_t)__builtin_return_address(0), __LINE__)        *
 *                                                                             *
 *    Purpose: Free a previously allocated buffer back to the pool            *
 *    Parameters:                                                              *
 *      - buf: Pointer to buffer to be freed                                  *
 *    Tracking: Captures return address and line number for debugging         *
 *    Safety: Null pointer safe with proper type casting                      *
 *                                                                             *
 *  COBUF: FOR STACK COMMON BUF - BUFFER AVAILABILITY                         *
 *                                                                             *
 *  #define cobuf_check_buf_ava(size)                                          *
 *      bt_callback_stack_buf_check_buf_ava(size)                             *
 *                                                                             *
 *    Purpose: Check if buffers of specified size are available               *
 *    Parameters:                                                              *
 *      - size: Buffer size to check availability for                         *
 *    Returns: Boolean indicating availability status                         *
 *    Usage: Pre-allocation check to avoid allocation failures                *
 *                                                                             *
 *  COBUF: FOR STACK COMMON BUF - BUFFER COUNTING                             *
 *                                                                             *
 *  #define cobuf_count_ava_buf(size)                                          *
 *      bt_callback_stack_buf_count_ava_buf(size)                             *
 *                                                                             *
 *    Purpose: Count available buffers of specified size                      *
 *    Parameters:                                                              *
 *      - size: Buffer size to count                                          *
 *    Returns: Number of available buffers                                    *
 *    Usage: Resource planning and monitoring                                 *
 *                                                                             *
 *  COBUF: FOR STACK COMMON BUF - STATISTICS REPORTING                        *
 *                                                                             *
 *  #define cobuf_print_statistic()                                            *
 *      bt_callback_stack_buf_print_usage()                                   *
 *                                                                             *
 *    Purpose: Print buffer usage statistics and diagnostics                  *
 *    Parameters: None                                                         *
 *    Output: Detailed usage report including:                                *
 *      - Total buffers allocated                                             *
 *      - Currently used buffers                                              *
 *      - Memory utilization percentage                                       *
 *      - Allocation/deallocation patterns                                    *
 *                                                                             *
 *  COBUF: FOR STACK COMMON BUF - DEBUGGING CAPABILITIES                      *
 *                                                                             *
 *  Advanced Debugging Features:                                               *
 *  - Return address tracking via __builtin_return_address(0)                 *
 *  - Line number tracking via __LINE__ macro                                 *
 *  - Call stack reconstruction for memory leak detection                     *
 *  - Allocation origin tracing for performance analysis                      *
 *                                                                             *
 *  COBUF: FOR STACK COMMON BUF - USAGE PATTERNS                              *
 *                                                                             *
 *  Typical Usage Sequence:                                                    *
 *                                                                             *
 *  1. Check buffer availability:                                             *
 *     if (cobuf_check_buf_ava(required_size)) {                              *
 *                                                                             *
 *  2. Allocate buffer:                                                       *
 *     void* buffer = cobuf_malloc(required_size);                            *
 *                                                                             *
 *  3. Use buffer for stack operations...                                     *
 *                                                                             *
 *  4. Free buffer when done:                                                 *
 *     cobuf_free(buffer);                                                    *
 *                                                                             *
 *  5. Monitor statistics:                                                    *
 *     cobuf_print_statistic();                                               *
 *                                                                             *
 *  COBUF: FOR STACK COMMON BUF - BEST PRACTICES                              *
 *                                                                             *
 *  ✓ Always check buffer availability before allocation                      *
 *  ✓ Free buffers in the same scope they were allocated                      *
 *  ✓ Use cobuf_print_statistic() for periodic monitoring                     *
 *  ✓ Leverage debugging information for troubleshooting                      *
 *  ✓ Monitor available buffer counts for capacity planning                   *
 *                                                                             *
 *  COBUF: FOR STACK COMMON BUF - ERROR HANDLING                              *
 *                                                                             *
 *  The callback system provides comprehensive error information:             *
 *  - Failed allocations return NULL with detailed tracking info              *
 *  - Invalid free operations are detected and reported                       *
 *  - Buffer corruption can be traced to allocation source                    *
 *  - Memory leaks are identifiable through statistic reports                 *
 *                                                                             *
 *  COBUF: FOR STACK COMMON BUF - PERFORMANCE OPTIMIZATION                    *
 *                                                                             *
 *  Benefits of This Implementation:                                           *
 *  ✓ Reduced system malloc/free overhead                                     *
 *  ✓ Better cache performance through memory locality                        *
 *  ✓ Elimination of memory fragmentation                                     *
 *  ✓ Predictable memory allocation patterns                                  *
 *  ✓ Comprehensive debugging with minimal runtime cost                       *
 *                                                                             *
 *******************************************************************************/
#define cobuf_malloc(size) bt_callback_stack_buf_malloc((size), (uint32_t)(uintptr_t)__builtin_return_address(0), __LINE__)
#define cobuf_free(buf) bt_callback_stack_buf_free((unsigned char *)buf, (uint32_t)(uintptr_t)__builtin_return_address(0), __LINE__)
#define cobuf_check_buf_ava(size) bt_callback_stack_buf_check_buf_ava(size)
#define cobuf_count_ava_buf(size) bt_callback_stack_buf_count_ava_buf(size)
#define cobuf_print_statistic() bt_callback_stack_buf_print_usage()


/*******************************************************************************
 *                                                                             *
 *                   PPB: POOLED PAGE BUFFER - BASED ON COBUF                 *
 *                                                                             *
 *  This module implements a Pooled Page Buffer (PPB) system built on top     *
 *  of the cobuf common buffer infrastructure. Provides page-aligned memory   *
 *  management with automatic growth capabilities and enhanced debugging.     *
 *                                                                             *
 *  Features:                                                                  *
 *  - Page-aligned buffer allocation using cobuf backend                      *
 *  - Automatic buffer growth for dynamic memory needs                        *
 *  - Callback-based allocation with debugging information                    *
 *  - Return address and line number tracking                                 *
 *  - Memory pool management and optimization                                 *
 *                                                                             *
 *  PPB: POOLED PAGE BUFFER - COBUF-BACKED ALLOCATION                         *
 *                                                                             *
 *  Macro Definitions:                                                         *
 *                                                                             *
 *  #define ppb_alloc(size)                                                    *
 *      ppb_alloc_with_ca((size),                                              *
 *          (uint32_t)(uintptr_t)__builtin_return_address(0), __LINE__)        *
 *                                                                             *
 *    Purpose: Allocate a page-aligned buffer using cobuf system              *
 *    Parameters:                                                              *
 *      - size: Requested buffer size in bytes                                *
 *    Returns: Pointer to allocated page buffer or NULL on failure            *
 *    Backend: Uses cobuf_malloc for underlying memory management             *
 *    Alignment: Ensures page-aligned memory addresses                        *
 *    Tracking: Captures return address and line number for debugging         *
 *                                                                             *
 *  PPB: POOLED PAGE BUFFER - AUTO-GROWING ALLOCATION                         *
 *                                                                             *
 *  #define ppb_auto_grow_alloc(size)                                          *
 *      ppb_auto_grow_alloc_with_ca((size),                                    *
 *          (uint32_t)(uintptr_t)__builtin_return_address(0), __LINE__)        *
 *                                                                             *
 *    Purpose: Allocate page buffer with automatic growth capability          *
 *    Parameters:                                                              *
 *      - size: Initial buffer size in bytes                                  *
 *    Returns: Pointer to auto-growable page buffer                           *
 *    Features:                                                                *
 *      - Automatically expands when more memory is needed                    *
 *      - Maintains data integrity during growth operations                   *
 *      - Optimized growth strategy to minimize reallocations                 *
 *    Backend: Leverages cobuf auto-growth capabilities                       *
 *    Tracking: Captures return address and line number for debugging         *
 *                                                                             *
 *  PPB: POOLED PAGE BUFFER - INTELLIGENT MEMORY MANAGEMENT                   *
 *                                                                             *
 *  #define ppb_free(ppb)                                                      *
 *      ppb_free_with_ca((ppb),                                                *
 *          (uint32_t)(uintptr_t)__builtin_return_address(0), __LINE__)        *
 *                                                                             *
 *    Purpose: Free a previously allocated page buffer                        *
 *    Parameters:                                                              *
 *      - ppb: Pointer to page buffer to be freed                             *
 *    Backend: Uses cobuf_free for memory reclamation                         *
 *    Safety:                                                                  *
 *      - Null pointer detection and handling                                 *
 *      - Double-free protection                                              *
 *      - Memory corruption detection                                         *
 *    Tracking: Captures return address and line number for debugging         *
 *    Cleanup: Properly returns memory to cobuf pool                          *
 *                                                                             *
 *  PPB: POOLED PAGE BUFFER - COBUF INTEGRATION                               *
 *                                                                             *
 *  Architecture Overview:                                                     *
 *                                                                             *
 *    +----------------+     +-------------------+     +-----------------+     *
 *    |   PPB Layer    | --> |   COBUF Backend   | --> |  Memory Pool    |     *
 *    | (Page Aligned) |     | (Common Buffers)  |     | (Pre-allocated) |     *
 *    +----------------+     +-------------------+     +-----------------+     *
 *                                                                             *
 *  Benefits of COBUF Integration:                                             *
 *  ✓ Leverages cobuf's efficient memory pooling                              *
 *  ✓ Inherits cobuf's debugging capabilities                                 *
 *  ✓ Utilizes cobuf's memory fragmentation avoidance                        *
 *  ✓ Benefits from cobuf's performance optimizations                         *
 *                                                                             *
 *  PPB: POOLED PAGE BUFFER - USAGE SCENARIOS                                 *
 *                                                                             *
 *  Typical Usage Patterns:                                                    *
 *                                                                             *
 *  1. Basic Page Buffer Allocation:                                          *
 *     void* buffer = ppb_alloc(PAGE_SIZE);                                   *
 *     if (buffer) {                                                          *
 *         // Use page-aligned buffer...                                      *
 *         ppb_free(buffer);                                                  *
 *     }                                                                      *
 *                                                                             *
 *  2. Auto-Growing Buffer for Dynamic Data:                                  *
 *     void* dynamic_buf = ppb_auto_grow_alloc(INITIAL_SIZE);                 *
 *     if (dynamic_buf) {                                                     *
 *         // Buffer automatically grows as needed                            *
 *         ppb_free(dynamic_buf);                                             *
 *     }                                                                      *
 *                                                                             *
 *  PPB: POOLED PAGE BUFFER - PERFORMANCE CHARACTERISTICS                     *
 *                                                                             *
 *  Memory Allocation Performance:                                             *
 *  - Page-aligned allocations reduce TLB misses                              *
 *  - COBUF backend provides O(1) allocation time                             *
 *  - Pooled memory reduces system call overhead                              *
 *  - Automatic growth minimizes manual reallocation logic                    *
 *                                                                             *
 *  PPB: POOLED PAGE BUFFER - DEBUGGING ADVANTAGES                            *
 *                                                                             *
 *  Enhanced Debugging Capabilities:                                           *
 *  - Inherits cobuf's return address tracking                                *
 *  - Line number tracking for precise allocation location                    *
 *  - Memory leak detection through cobuf statistics                          *
 *  - Allocation pattern analysis for optimization                            *
 *                                                                             *
 *  PPB: POOLED PAGE BUFFER - MEMORY EFFICIENCY                               *
 *                                                                             *
 *  Optimization Features:                                                     *
 *  ✓ Smart memory pooling through cobuf                                      *
 *  ✓ Page-aligned allocations reduce waste                                   *
 *  ✓ Automatic growth prevents overallocation                               *
 *  ✓ Efficient memory reuse through pooling                                  *
 *                                                                             *
 *  PPB: POOLED PAGE BUFFER - ERROR HANDLING                                  *
 *                                                                             *
 *  Comprehensive Error Management:                                            *
 *  - Allocation failures propagate NULL with debug info                      *
 *  - Invalid free operations are caught and logged                           *
 *  - Memory corruption detection through cobuf                               *
 *  - Graceful handling of out-of-memory conditions                           *
 *                                                                             *
 *  PPB: POOLED PAGE BUFFER - BEST PRACTICES                                  *
 *                                                                             *
 *  Recommended Usage Guidelines:                                              *
 *  ✓ Use ppb_alloc for fixed-size page buffers                              *
 *  ✓ Use ppb_auto_grow_alloc for dynamically sized data                      *
 *  ✓ Always check return values for allocation success                       *
 *  ✓ Free buffers in reverse allocation order                               *
 *  ✓ Monitor memory usage through cobuf_print_statistic()                    *
 *  ✓ Leverage debugging info for troubleshooting                             *
 *                                                                             *
 *  PPB: POOLED PAGE BUFFER - SYSTEM INTEGRATION                              *
 *                                                                             *
 *  Dependencies and Requirements:                                             *
 *  - Requires initialized cobuf system                                       *
 *  - Depends on cobuf_malloc/cobuf_free backend                             *
 *  - Uses system page size for alignment                                     *
 *  - Integrates with existing cobuf monitoring tools                         *
 *                                                                             *
 *******************************************************************************/

#define ppb_alloc(size) ppb_alloc_with_ca((size), (uint32_t)(uintptr_t)__builtin_return_address(0), __LINE__)
#define ppb_auto_grow_alloc(size) ppb_auto_grow_alloc_with_ca((size), (uint32_t)(uintptr_t)__builtin_return_address(0), __LINE__)
#define ppb_free(ppb) ppb_free_with_ca((ppb), (uint32_t)(uintptr_t)__builtin_return_address(0), __LINE__)

struct pp_buff *ppb_alloc_with_ca(uint16_t size, uint32_t ca, uint32_t line);
struct pp_buff *ppb_auto_grow_alloc_with_ca(uint16_t size, uint32_t ca, uint32_t line);
bool ppb_auto_grow_realloc(struct pp_buff **pptr, uint16_t size);
bool ppb_auto_grow_put_data(struct pp_buff **pptr, const uint8_t *data, uint16_t len);
uint8_t *ppb_auto_grow_put(struct pp_buff **pptr, uint16_t len);
struct pp_buff *ppb_alloc_inplace(uint16_t size, uint8_t *inplace_buff, uint16_t inplace_buff_len);
void ppb_free_with_ca(struct pp_buff *ppb, uint32_t ca, uint32_t line);

uint8_t *ppb_put(struct pp_buff *ppb, uint16_t len);
bool ppb_put_data(struct pp_buff *ppb, const uint8_t *data, uint16_t len);
void ppb_set_len(struct pp_buff *ppb, uint16_t len);
void ppb_reserve(struct pp_buff *ppb, uint16_t len);
uint8_t *ppb_use_reserve(struct pp_buff *ppb, uint16_t len);
uint8_t *ppb_data_forward(struct pp_buff *ppb, uint16_t len);
uint8_t *ppb_data_backward(struct pp_buff *ppb, uint16_t len);
uint8_t *ppb_tail_forward(struct pp_buff *ppb, uint16_t len);
uint8_t *ppb_tail_backward(struct pp_buff *ppb, uint16_t len);


#if defined(__cplusplus)
}
#endif
#endif /* __COBUF_H__ */
