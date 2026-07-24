/******************************************************************************
 * @file     arm_math_utils.h
 * @brief    Public header file for CMSIS DSP Library
 * @version  V1.9.0
 * @date     20. July 2020
 ******************************************************************************/
/*
 * Copyright (c) 2010-2020 Arm Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _ARM_MATH_UTILS_H_

#define _ARM_MATH_UTILS_H_

#include "arm_math_types.h"

#ifdef   __cplusplus
extern "C"
{
#endif

  /**
   * @brief Macros required for reciprocal calculation in Normalized LMS
   */

#define INDEX_MASK         0x0000003F


#define SQ(x) ((x) * (x))

#define ROUND_UP(N, S) ((((N) + (S) - 1) / (S)) * (S))

#ifdef   __cplusplus
}
#endif

#endif /*ifndef _ARM_MATH_UTILS_H_ */
