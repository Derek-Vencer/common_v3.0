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

#include "mm_dbg.h"
#include "plat_addr_map.h"
#include "cmsis_nvic.h"
#include "hal_timer.h"
#include "filter_debug_trace.h"
#include "string.h"
#include "hal_cmu.h"
#include "dsd_process.h"
#include "hal_dma.h"
#include "hal_location.h"

typedef signed int                          fir_sample_24bits_t;
typedef signed short int                    fir_sample_16bits_t;

static int BitToValue[256][8] = {
{-524288,-524288,-524288,-524288,-524288,-524288,-524288,-524288,},
{-524288,-524288,-524288,-524288,-524288,-524288,-524288,524288,},
{-524288,-524288,-524288,-524288,-524288,-524288,524288,-524288,},
{-524288,-524288,-524288,-524288,-524288,-524288,524288,524288,},
{-524288,-524288,-524288,-524288,-524288,524288,-524288,-524288,},
{-524288,-524288,-524288,-524288,-524288,524288,-524288,524288,},
{-524288,-524288,-524288,-524288,-524288,524288,524288,-524288,},
{-524288,-524288,-524288,-524288,-524288,524288,524288,524288,},
{-524288,-524288,-524288,-524288,524288,-524288,-524288,-524288,},
{-524288,-524288,-524288,-524288,524288,-524288,-524288,524288,},
{-524288,-524288,-524288,-524288,524288,-524288,524288,-524288,},
{-524288,-524288,-524288,-524288,524288,-524288,524288,524288,},
{-524288,-524288,-524288,-524288,524288,524288,-524288,-524288,},
{-524288,-524288,-524288,-524288,524288,524288,-524288,524288,},
{-524288,-524288,-524288,-524288,524288,524288,524288,-524288,},
{-524288,-524288,-524288,-524288,524288,524288,524288,524288,},
{-524288,-524288,-524288,524288,-524288,-524288,-524288,-524288,},
{-524288,-524288,-524288,524288,-524288,-524288,-524288,524288,},
{-524288,-524288,-524288,524288,-524288,-524288,524288,-524288,},
{-524288,-524288,-524288,524288,-524288,-524288,524288,524288,},
{-524288,-524288,-524288,524288,-524288,524288,-524288,-524288,},
{-524288,-524288,-524288,524288,-524288,524288,-524288,524288,},
{-524288,-524288,-524288,524288,-524288,524288,524288,-524288,},
{-524288,-524288,-524288,524288,-524288,524288,524288,524288,},
{-524288,-524288,-524288,524288,524288,-524288,-524288,-524288,},
{-524288,-524288,-524288,524288,524288,-524288,-524288,524288,},
{-524288,-524288,-524288,524288,524288,-524288,524288,-524288,},
{-524288,-524288,-524288,524288,524288,-524288,524288,524288,},
{-524288,-524288,-524288,524288,524288,524288,-524288,-524288,},
{-524288,-524288,-524288,524288,524288,524288,-524288,524288,},
{-524288,-524288,-524288,524288,524288,524288,524288,-524288,},
{-524288,-524288,-524288,524288,524288,524288,524288,524288,},
{-524288,-524288,524288,-524288,-524288,-524288,-524288,-524288,},
{-524288,-524288,524288,-524288,-524288,-524288,-524288,524288,},
{-524288,-524288,524288,-524288,-524288,-524288,524288,-524288,},
{-524288,-524288,524288,-524288,-524288,-524288,524288,524288,},
{-524288,-524288,524288,-524288,-524288,524288,-524288,-524288,},
{-524288,-524288,524288,-524288,-524288,524288,-524288,524288,},
{-524288,-524288,524288,-524288,-524288,524288,524288,-524288,},
{-524288,-524288,524288,-524288,-524288,524288,524288,524288,},
{-524288,-524288,524288,-524288,524288,-524288,-524288,-524288,},
{-524288,-524288,524288,-524288,524288,-524288,-524288,524288,},
{-524288,-524288,524288,-524288,524288,-524288,524288,-524288,},
{-524288,-524288,524288,-524288,524288,-524288,524288,524288,},
{-524288,-524288,524288,-524288,524288,524288,-524288,-524288,},
{-524288,-524288,524288,-524288,524288,524288,-524288,524288,},
{-524288,-524288,524288,-524288,524288,524288,524288,-524288,},
{-524288,-524288,524288,-524288,524288,524288,524288,524288,},
{-524288,-524288,524288,524288,-524288,-524288,-524288,-524288,},
{-524288,-524288,524288,524288,-524288,-524288,-524288,524288,},
{-524288,-524288,524288,524288,-524288,-524288,524288,-524288,},
{-524288,-524288,524288,524288,-524288,-524288,524288,524288,},
{-524288,-524288,524288,524288,-524288,524288,-524288,-524288,},
{-524288,-524288,524288,524288,-524288,524288,-524288,524288,},
{-524288,-524288,524288,524288,-524288,524288,524288,-524288,},
{-524288,-524288,524288,524288,-524288,524288,524288,524288,},
{-524288,-524288,524288,524288,524288,-524288,-524288,-524288,},
{-524288,-524288,524288,524288,524288,-524288,-524288,524288,},
{-524288,-524288,524288,524288,524288,-524288,524288,-524288,},
{-524288,-524288,524288,524288,524288,-524288,524288,524288,},
{-524288,-524288,524288,524288,524288,524288,-524288,-524288,},
{-524288,-524288,524288,524288,524288,524288,-524288,524288,},
{-524288,-524288,524288,524288,524288,524288,524288,-524288,},
{-524288,-524288,524288,524288,524288,524288,524288,524288,},
{-524288,524288,-524288,-524288,-524288,-524288,-524288,-524288,},
{-524288,524288,-524288,-524288,-524288,-524288,-524288,524288,},
{-524288,524288,-524288,-524288,-524288,-524288,524288,-524288,},
{-524288,524288,-524288,-524288,-524288,-524288,524288,524288,},
{-524288,524288,-524288,-524288,-524288,524288,-524288,-524288,},
{-524288,524288,-524288,-524288,-524288,524288,-524288,524288,},
{-524288,524288,-524288,-524288,-524288,524288,524288,-524288,},
{-524288,524288,-524288,-524288,-524288,524288,524288,524288,},
{-524288,524288,-524288,-524288,524288,-524288,-524288,-524288,},
{-524288,524288,-524288,-524288,524288,-524288,-524288,524288,},
{-524288,524288,-524288,-524288,524288,-524288,524288,-524288,},
{-524288,524288,-524288,-524288,524288,-524288,524288,524288,},
{-524288,524288,-524288,-524288,524288,524288,-524288,-524288,},
{-524288,524288,-524288,-524288,524288,524288,-524288,524288,},
{-524288,524288,-524288,-524288,524288,524288,524288,-524288,},
{-524288,524288,-524288,-524288,524288,524288,524288,524288,},
{-524288,524288,-524288,524288,-524288,-524288,-524288,-524288,},
{-524288,524288,-524288,524288,-524288,-524288,-524288,524288,},
{-524288,524288,-524288,524288,-524288,-524288,524288,-524288,},
{-524288,524288,-524288,524288,-524288,-524288,524288,524288,},
{-524288,524288,-524288,524288,-524288,524288,-524288,-524288,},
{-524288,524288,-524288,524288,-524288,524288,-524288,524288,},
{-524288,524288,-524288,524288,-524288,524288,524288,-524288,},
{-524288,524288,-524288,524288,-524288,524288,524288,524288,},
{-524288,524288,-524288,524288,524288,-524288,-524288,-524288,},
{-524288,524288,-524288,524288,524288,-524288,-524288,524288,},
{-524288,524288,-524288,524288,524288,-524288,524288,-524288,},
{-524288,524288,-524288,524288,524288,-524288,524288,524288,},
{-524288,524288,-524288,524288,524288,524288,-524288,-524288,},
{-524288,524288,-524288,524288,524288,524288,-524288,524288,},
{-524288,524288,-524288,524288,524288,524288,524288,-524288,},
{-524288,524288,-524288,524288,524288,524288,524288,524288,},
{-524288,524288,524288,-524288,-524288,-524288,-524288,-524288,},
{-524288,524288,524288,-524288,-524288,-524288,-524288,524288,},
{-524288,524288,524288,-524288,-524288,-524288,524288,-524288,},
{-524288,524288,524288,-524288,-524288,-524288,524288,524288,},
{-524288,524288,524288,-524288,-524288,524288,-524288,-524288,},
{-524288,524288,524288,-524288,-524288,524288,-524288,524288,},
{-524288,524288,524288,-524288,-524288,524288,524288,-524288,},
{-524288,524288,524288,-524288,-524288,524288,524288,524288,},
{-524288,524288,524288,-524288,524288,-524288,-524288,-524288,},
{-524288,524288,524288,-524288,524288,-524288,-524288,524288,},
{-524288,524288,524288,-524288,524288,-524288,524288,-524288,},
{-524288,524288,524288,-524288,524288,-524288,524288,524288,},
{-524288,524288,524288,-524288,524288,524288,-524288,-524288,},
{-524288,524288,524288,-524288,524288,524288,-524288,524288,},
{-524288,524288,524288,-524288,524288,524288,524288,-524288,},
{-524288,524288,524288,-524288,524288,524288,524288,524288,},
{-524288,524288,524288,524288,-524288,-524288,-524288,-524288,},
{-524288,524288,524288,524288,-524288,-524288,-524288,524288,},
{-524288,524288,524288,524288,-524288,-524288,524288,-524288,},
{-524288,524288,524288,524288,-524288,-524288,524288,524288,},
{-524288,524288,524288,524288,-524288,524288,-524288,-524288,},
{-524288,524288,524288,524288,-524288,524288,-524288,524288,},
{-524288,524288,524288,524288,-524288,524288,524288,-524288,},
{-524288,524288,524288,524288,-524288,524288,524288,524288,},
{-524288,524288,524288,524288,524288,-524288,-524288,-524288,},
{-524288,524288,524288,524288,524288,-524288,-524288,524288,},
{-524288,524288,524288,524288,524288,-524288,524288,-524288,},
{-524288,524288,524288,524288,524288,-524288,524288,524288,},
{-524288,524288,524288,524288,524288,524288,-524288,-524288,},
{-524288,524288,524288,524288,524288,524288,-524288,524288,},
{-524288,524288,524288,524288,524288,524288,524288,-524288,},
{-524288,524288,524288,524288,524288,524288,524288,524288,},
{524288,-524288,-524288,-524288,-524288,-524288,-524288,-524288,},
{524288,-524288,-524288,-524288,-524288,-524288,-524288,524288,},
{524288,-524288,-524288,-524288,-524288,-524288,524288,-524288,},
{524288,-524288,-524288,-524288,-524288,-524288,524288,524288,},
{524288,-524288,-524288,-524288,-524288,524288,-524288,-524288,},
{524288,-524288,-524288,-524288,-524288,524288,-524288,524288,},
{524288,-524288,-524288,-524288,-524288,524288,524288,-524288,},
{524288,-524288,-524288,-524288,-524288,524288,524288,524288,},
{524288,-524288,-524288,-524288,524288,-524288,-524288,-524288,},
{524288,-524288,-524288,-524288,524288,-524288,-524288,524288,},
{524288,-524288,-524288,-524288,524288,-524288,524288,-524288,},
{524288,-524288,-524288,-524288,524288,-524288,524288,524288,},
{524288,-524288,-524288,-524288,524288,524288,-524288,-524288,},
{524288,-524288,-524288,-524288,524288,524288,-524288,524288,},
{524288,-524288,-524288,-524288,524288,524288,524288,-524288,},
{524288,-524288,-524288,-524288,524288,524288,524288,524288,},
{524288,-524288,-524288,524288,-524288,-524288,-524288,-524288,},
{524288,-524288,-524288,524288,-524288,-524288,-524288,524288,},
{524288,-524288,-524288,524288,-524288,-524288,524288,-524288,},
{524288,-524288,-524288,524288,-524288,-524288,524288,524288,},
{524288,-524288,-524288,524288,-524288,524288,-524288,-524288,},
{524288,-524288,-524288,524288,-524288,524288,-524288,524288,},
{524288,-524288,-524288,524288,-524288,524288,524288,-524288,},
{524288,-524288,-524288,524288,-524288,524288,524288,524288,},
{524288,-524288,-524288,524288,524288,-524288,-524288,-524288,},
{524288,-524288,-524288,524288,524288,-524288,-524288,524288,},
{524288,-524288,-524288,524288,524288,-524288,524288,-524288,},
{524288,-524288,-524288,524288,524288,-524288,524288,524288,},
{524288,-524288,-524288,524288,524288,524288,-524288,-524288,},
{524288,-524288,-524288,524288,524288,524288,-524288,524288,},
{524288,-524288,-524288,524288,524288,524288,524288,-524288,},
{524288,-524288,-524288,524288,524288,524288,524288,524288,},
{524288,-524288,524288,-524288,-524288,-524288,-524288,-524288,},
{524288,-524288,524288,-524288,-524288,-524288,-524288,524288,},
{524288,-524288,524288,-524288,-524288,-524288,524288,-524288,},
{524288,-524288,524288,-524288,-524288,-524288,524288,524288,},
{524288,-524288,524288,-524288,-524288,524288,-524288,-524288,},
{524288,-524288,524288,-524288,-524288,524288,-524288,524288,},
{524288,-524288,524288,-524288,-524288,524288,524288,-524288,},
{524288,-524288,524288,-524288,-524288,524288,524288,524288,},
{524288,-524288,524288,-524288,524288,-524288,-524288,-524288,},
{524288,-524288,524288,-524288,524288,-524288,-524288,524288,},
{524288,-524288,524288,-524288,524288,-524288,524288,-524288,},
{524288,-524288,524288,-524288,524288,-524288,524288,524288,},
{524288,-524288,524288,-524288,524288,524288,-524288,-524288,},
{524288,-524288,524288,-524288,524288,524288,-524288,524288,},
{524288,-524288,524288,-524288,524288,524288,524288,-524288,},
{524288,-524288,524288,-524288,524288,524288,524288,524288,},
{524288,-524288,524288,524288,-524288,-524288,-524288,-524288,},
{524288,-524288,524288,524288,-524288,-524288,-524288,524288,},
{524288,-524288,524288,524288,-524288,-524288,524288,-524288,},
{524288,-524288,524288,524288,-524288,-524288,524288,524288,},
{524288,-524288,524288,524288,-524288,524288,-524288,-524288,},
{524288,-524288,524288,524288,-524288,524288,-524288,524288,},
{524288,-524288,524288,524288,-524288,524288,524288,-524288,},
{524288,-524288,524288,524288,-524288,524288,524288,524288,},
{524288,-524288,524288,524288,524288,-524288,-524288,-524288,},
{524288,-524288,524288,524288,524288,-524288,-524288,524288,},
{524288,-524288,524288,524288,524288,-524288,524288,-524288,},
{524288,-524288,524288,524288,524288,-524288,524288,524288,},
{524288,-524288,524288,524288,524288,524288,-524288,-524288,},
{524288,-524288,524288,524288,524288,524288,-524288,524288,},
{524288,-524288,524288,524288,524288,524288,524288,-524288,},
{524288,-524288,524288,524288,524288,524288,524288,524288,},
{524288,524288,-524288,-524288,-524288,-524288,-524288,-524288,},
{524288,524288,-524288,-524288,-524288,-524288,-524288,524288,},
{524288,524288,-524288,-524288,-524288,-524288,524288,-524288,},
{524288,524288,-524288,-524288,-524288,-524288,524288,524288,},
{524288,524288,-524288,-524288,-524288,524288,-524288,-524288,},
{524288,524288,-524288,-524288,-524288,524288,-524288,524288,},
{524288,524288,-524288,-524288,-524288,524288,524288,-524288,},
{524288,524288,-524288,-524288,-524288,524288,524288,524288,},
{524288,524288,-524288,-524288,524288,-524288,-524288,-524288,},
{524288,524288,-524288,-524288,524288,-524288,-524288,524288,},
{524288,524288,-524288,-524288,524288,-524288,524288,-524288,},
{524288,524288,-524288,-524288,524288,-524288,524288,524288,},
{524288,524288,-524288,-524288,524288,524288,-524288,-524288,},
{524288,524288,-524288,-524288,524288,524288,-524288,524288,},
{524288,524288,-524288,-524288,524288,524288,524288,-524288,},
{524288,524288,-524288,-524288,524288,524288,524288,524288,},
{524288,524288,-524288,524288,-524288,-524288,-524288,-524288,},
{524288,524288,-524288,524288,-524288,-524288,-524288,524288,},
{524288,524288,-524288,524288,-524288,-524288,524288,-524288,},
{524288,524288,-524288,524288,-524288,-524288,524288,524288,},
{524288,524288,-524288,524288,-524288,524288,-524288,-524288,},
{524288,524288,-524288,524288,-524288,524288,-524288,524288,},
{524288,524288,-524288,524288,-524288,524288,524288,-524288,},
{524288,524288,-524288,524288,-524288,524288,524288,524288,},
{524288,524288,-524288,524288,524288,-524288,-524288,-524288,},
{524288,524288,-524288,524288,524288,-524288,-524288,524288,},
{524288,524288,-524288,524288,524288,-524288,524288,-524288,},
{524288,524288,-524288,524288,524288,-524288,524288,524288,},
{524288,524288,-524288,524288,524288,524288,-524288,-524288,},
{524288,524288,-524288,524288,524288,524288,-524288,524288,},
{524288,524288,-524288,524288,524288,524288,524288,-524288,},
{524288,524288,-524288,524288,524288,524288,524288,524288,},
{524288,524288,524288,-524288,-524288,-524288,-524288,-524288,},
{524288,524288,524288,-524288,-524288,-524288,-524288,524288,},
{524288,524288,524288,-524288,-524288,-524288,524288,-524288,},
{524288,524288,524288,-524288,-524288,-524288,524288,524288,},
{524288,524288,524288,-524288,-524288,524288,-524288,-524288,},
{524288,524288,524288,-524288,-524288,524288,-524288,524288,},
{524288,524288,524288,-524288,-524288,524288,524288,-524288,},
{524288,524288,524288,-524288,-524288,524288,524288,524288,},
{524288,524288,524288,-524288,524288,-524288,-524288,-524288,},
{524288,524288,524288,-524288,524288,-524288,-524288,524288,},
{524288,524288,524288,-524288,524288,-524288,524288,-524288,},
{524288,524288,524288,-524288,524288,-524288,524288,524288,},
{524288,524288,524288,-524288,524288,524288,-524288,-524288,},
{524288,524288,524288,-524288,524288,524288,-524288,524288,},
{524288,524288,524288,-524288,524288,524288,524288,-524288,},
{524288,524288,524288,-524288,524288,524288,524288,524288,},
{524288,524288,524288,524288,-524288,-524288,-524288,-524288,},
{524288,524288,524288,524288,-524288,-524288,-524288,524288,},
{524288,524288,524288,524288,-524288,-524288,524288,-524288,},
{524288,524288,524288,524288,-524288,-524288,524288,524288,},
{524288,524288,524288,524288,-524288,524288,-524288,-524288,},
{524288,524288,524288,524288,-524288,524288,-524288,524288,},
{524288,524288,524288,524288,-524288,524288,524288,-524288,},
{524288,524288,524288,524288,-524288,524288,524288,524288,},
{524288,524288,524288,524288,524288,-524288,-524288,-524288,},
{524288,524288,524288,524288,524288,-524288,-524288,524288,},
{524288,524288,524288,524288,524288,-524288,524288,-524288,},
{524288,524288,524288,524288,524288,-524288,524288,524288,},
{524288,524288,524288,524288,524288,524288,-524288,-524288,},
{524288,524288,524288,524288,524288,524288,-524288,524288,},
{524288,524288,524288,524288,524288,524288,524288,-524288,},
{524288,524288,524288,524288,524288,524288,524288,524288,},
};

const DSD_CFG_T audio_eq_hw_fir_cfg_176_4k = {
    .gain0 = 6,
    .gain1 = 6,
    .len = 384,
    .coef =
    {
	-16, 9, 14, 23, 37, 55, 79, 110, 149, 198, 257, 328, 414, 515, 633, 771, 929, 1110, 1315, 1546, 1804, 2090, 2405, 2751, 3125, 3530, 3963, 4424, 4910, 
        5420, 5949, 6493, 7048, 7608, 8166, 8715, 9246, 9751, 10218, 10637, 10997, 11284, 11487, 11592, 11586, 11453, 11182, 10758, 10168, 9401, 8443, 7286,
        5920, 4337, 2534, 506, -1748, -4223, -6917, -9819, -12918, -16199, -19643, -23225, -26919, -30694, -34514, -38341, -42131, -45840, -49416, -52808,
        -55960, -58816, -61317, -63403, -65014, -66091, -66575, -66409, -65540, -63918, -61498, -58238, -54105, -49073, -43124, -36248, -28445, -19727, 
        -10115, 355, 11639, 23678, 36399, 49717, 63536, 77744, 92219, 106826, 121418, 135840, 149926, 163501, 176386, 188394, 199337, 209024, 217265, 
        223871, 228661, 231458, 232096, 230421, 226291, 219585, 210196, 198043, 183065, 165229, 144529, 120988, 94661, 65634, 34028, -4, -36273, -74555, 
        -114593, -156093, -198730, -242148, -285958, -329746, -373070, -415465, -456448, -495516, -532157, -565847, -596059, -622267, -643946, -660584, 
        -671681, -676756, -675353, -667043, -651434, -628170, -596937, -557469, -509551, -453023, -387781, -313784, -231051, -139668, -39785, 68379, 184539, 
        308343, 439375, 577152, 721131, 870709, 1025226, 1183972, 1346187, 1511070, 1677779, 1845443, 2013166, 2180028, 2345102, 2507450, 2666138, 2820239, 
        2968844, 3111062, 3246035, 3372940, 3490997, 3599474, 3697696, 3785047, 3860976, 3925002, 3976721, 4015802, 4041995, 4055134, 4055134, 4041995,
        4015802, 3976721, 3925002, 3860976, 3785047, 3697696, 3599474, 3490997, 3372940, 3246035, 3111062, 2968844, 2820239, 2666138, 2507450, 2345102, 
        2180028, 2013166, 1845443, 1677779, 1511070, 1346187, 1183972, 1025226, 870709, 721131, 577152, 439375, 308343, 184539, 68379, -39785, -139668, 
        -231051, -313784, -387781, -453023, -509551, -557469, -596937, -628170, -651434, -667043, -675353, -676756, -671681, -660584, -643946, -622267, 
        -596059, -565847, -532157, -495516, -456448, -415465, -373070, -329746, -285958, -242148, -198730, -156093, -114593, -74555, -36273, -4, 34028,
        65634, 94661, 120988, 144529, 165229, 183065, 198043, 210196, 219585, 226291, 230421, 232096, 231458, 228661, 223871, 217265, 209024, 199337, 188394, 
    176386, 163501, 149926, 135840, 121418, 106826, 92219, 77744, 63536, 49717, 36399, 23678, 11639, 355, -10115, -19727, -28445, -36248, -43124, -49073, 
    -54105, -58238, -61498, -63918, -65540, -66409, -66575, -66091, -65014, -63403, -61317, -58816, -55960, -52808, -49416, -45840, -42131, -38341, -34514,
    -30694, -26919, -23225, -19643, -16199, -12918, -9819, -6917, -4223, -1748, 506, 2534, 4337, 5920, 7286, 8443, 9401, 10168, 10758, 11182, 11453, 11586, 
    11592, 11487, 11284, 10997, 10637, 10218, 9751, 9246, 8715, 8166, 7608, 7048, 6493, 5949, 5420, 4910, 4424, 3963, 3530, 3125, 2751, 2405, 2090, 1804, 
    1546, 1315, 1110, 929, 771, 633, 515, 414, 328, 257, 198, 149, 110, 79, 55, 37, 23, 14, 9, -16,

    }
};



// Hardware fir filter parallel flag
#define HW_FIR_PARALLEL_RUN

#define FIR_MEM_LEN                         512

enum FIR_CHNL_ID_T {
    FIR_CHNL_ID_0 = 0,
    FIR_CHNL_ID_1 ,
    FIR_CHNL_ID_2 ,
    FIR_CHNL_ID_3 ,
    FIR_CHNL_ID_QTY,
};

/*
1	100	fir_stream_enable_ch0	RW	0	0	1: used for anc fir or adc downsample fir
1	100	fir_stream_enable_ch1	RW	0	1	
1	100	fir_stream_enable_ch2	RW	0	2	
1	100	fir_stream_enable_ch3	RW	0	3	
1	100	fir_enable_ch0	RW	0	4	1: used for memory to memory fir
1	100	fir_enable_ch1	RW	0	5	
1	100	fir_enable_ch2	RW	0	6	
1	100	fir_enable_ch3	RW	0	7	
1	100	dma_ctrl_rx_fir	RW	0	8	
1	100	dma_ctrl_tx_fir	RW	0	9	
1	100	fir_upsample_ch0	RW	0	10	1: use for memory to memory fir upsample
1	100	fir_upsample_ch1	RW	0	11	
1	100	fir_upsample_ch2	RW	0	12	
1	100	fir_upsample_ch3	RW	0	13	
1	100	mode_32bit_fir	RW	0	14	 all coef/sample to memory; not used together with any 16 bit mode
1	100	fir_reserved_reg0	RW	0	15	not used
1	100	mode_16bit_fir_tx_ch0	RW	0	16	 sample memory write as 16bit mode
1	100	mode_16bit_fir_rx_ch0	RW	0	17	 coef memory read as 16bit mode
1	100	mode_16bit_fir_tx_ch1	RW	0	18	
1	100	mode_16bit_fir_rx_ch1	RW	0	19	
1	100	mode_16bit_fir_tx_ch2	RW	0	20	
1	100	mode_16bit_fir_rx_ch2	RW	0	21	
1	100	mode_16bit_fir_tx_ch3	RW	0	22	
1	100	mode_16bit_fir_rx_ch3	RW	0	23	
24	100		x			

*/
struct _fir_config0
{
	uint32_t fir_stream_enable_ch0 : 1;
	uint32_t fir_stream_enable_ch1 : 1;
	uint32_t fir_stream_enable_ch2 : 1;
	uint32_t fir_stream_enable_ch3 : 1;

	uint32_t fir_enable_ch0 : 1;
	uint32_t fir_enable_ch1 : 1;
	uint32_t fir_enable_ch2 : 1;
	uint32_t fir_enable_ch3 : 1;
	
	uint32_t dma_ctrl_rx_fir : 1;
	uint32_t dma_ctrl_tx_fir : 1;

	uint32_t fir_upsample_ch0 : 1;
	uint32_t fir_upsample_ch1 : 1;
	uint32_t fir_upsample_ch2 : 1;
	uint32_t fir_upsample_ch3 : 1;

	uint32_t mode_32bit_fir : 1;
	uint32_t fir_reserved_reg0 : 1;

	uint32_t mode_16bit_fir_tx_ch0 : 1;
	uint32_t mode_16bit_fir_rx_ch0 : 1;
	uint32_t mode_16bit_fir_tx_ch1 : 1;
	uint32_t mode_16bit_fir_rx_ch1 : 1;
	uint32_t mode_16bit_fir_tx_ch2 : 1;
	uint32_t mode_16bit_fir_rx_ch2 : 1;
	uint32_t mode_16bit_fir_tx_ch3 : 1;
	uint32_t mode_16bit_fir_rx_ch3 : 1;
    
	uint32_t Reserved : 8;
};



/*
3	104	fir_access_offset_ch0	RW	0	0	
3	104	fir_access_offset_ch1	RW	0	3	
3	104	fir_access_offset_ch2	RW	0	6	
3	104	fir_access_offset_ch3	RW	0	9	
12	104		x			
*/
struct _fir_config1
{
	uint32_t fir_access_offset_ch0 : 3;
	uint32_t fir_access_offset_ch1 : 3;	
	uint32_t fir_access_offset_ch2 : 3;
	uint32_t fir_access_offset_ch3 : 3;	
    
	uint32_t Reserved : 20;
	
};


/*
1	108	stream0_fir1_ch0	RW	0	0	0: stream mode; 1: fir mode
2	108	fir_mode_ch0	RW	0	1	fir operation mode;  0: normal fir;   1: synthesis1;  2: synthesis2;
10	108	fir_order_ch0	RW	0	3	
9	108	fir_sample_start_ch0	RW	0	13	
9	108	fir_sample_num_ch0	RW	0	22	
1	108	fir_do_remap_ch0	RW	0	31	
32	108		x			
*/
struct _fir_ch_config0
{
	uint32_t stream0_fir1 : 1;
	uint32_t fir_mode : 2;	
	uint32_t fir_order : 10;	
	uint32_t fir_sample_start : 9;	
	uint32_t fir_sample_num : 9;	
	uint32_t fir_do_remap : 1;	
};


/*
9	10c	fir_result_base_addr_ch0	RW	0	0	
6	10c	fir_slide_offset_ch0	RW	0	9	
6	10c	fir_burst_length_ch0	RW	0	15	
4	10c	fir_gain_sel_ch0	RW	0	21	
7	10c	fir_loop_num_ch0	RW	0	25	
32	10c		x			
*/
struct _fir_ch_config1
{
	uint32_t fir_result_base_addr : 9;
	uint32_t fir_slide_offset : 6;	
	uint32_t fir_burst_length : 6;	
	uint32_t fir_gain_sel : 4;	
	uint32_t fir_loop_num : 7;		
};

struct _fir_ch_config{
    volatile struct _fir_ch_config0 *fir_ch_config0;
    volatile struct _fir_ch_config1 *fir_ch_config1;
    volatile int32_t *sample;
    volatile int32_t *coef;
};

struct FIR_SETTINGS_T {
    volatile struct _fir_config0 *fir_config0;
    volatile struct _fir_config1 *fir_config1;
    volatile struct _fir_ch_config fir_ch_config[4];
};

#define FIR_BASE                            ((uint32_t)CODEC_BASE)

static volatile const struct FIR_SETTINGS_T fir_setings = {
    .fir_config0   = (volatile struct _fir_config0 *)(FIR_BASE + 0x100),
    .fir_config1   = (volatile struct _fir_config1 *)(FIR_BASE + 0x104),		
    .fir_ch_config[0] = {
        .fir_ch_config0    = (volatile struct _fir_ch_config0 *)(FIR_BASE + 0x108),
        .fir_ch_config1   = (volatile struct _fir_ch_config1 *)(FIR_BASE + 0x10c),
        .sample =  (volatile int32_t *)(FIR_BASE + 0x8000),        
        .coef     =  (volatile int32_t *)(FIR_BASE + 0x9000),
    },
    .fir_ch_config[1] = {
        .fir_ch_config0    = (volatile struct _fir_ch_config0 *)(FIR_BASE + 0x110),
        .fir_ch_config1   = (volatile struct _fir_ch_config1 *)(FIR_BASE + 0x114),
        .sample =  (volatile int32_t *)(FIR_BASE + 0xa000),        
        .coef     =  (volatile int32_t *)(FIR_BASE + 0xb000),
    },
    .fir_ch_config[2] = {
        .fir_ch_config0    = (volatile struct _fir_ch_config0 *)(FIR_BASE + 0x118),
        .fir_ch_config1   = (volatile struct _fir_ch_config1 *)(FIR_BASE + 0x11c),
        .sample =  (volatile int32_t *)(FIR_BASE + 0xc000),        
        .coef     =  (volatile int32_t *)(FIR_BASE + 0xd000),
    },
    .fir_ch_config[3] = {
        .fir_ch_config0    = (volatile struct _fir_ch_config0 *)(FIR_BASE + 0x120),
        .fir_ch_config1   = (volatile struct _fir_ch_config1 *)(FIR_BASE + 0x124),
        .sample =  (volatile int32_t *)(FIR_BASE + 0xe000),        
        .coef     =  (volatile int32_t *)(FIR_BASE + 0xf000),
    },
};



typedef enum {
    PP_PING = 0,
    PP_PANG = 1
} FIR_PP_T;

typedef enum {
    FIR_DATA_IDLE,
    FIR_DATA_BUSY,
    FIR_DATA_DONE,
} FIR_DATA_STATE_T;

typedef struct {
    FIR_DATA_STATE_T state;
    uint16_t eq_num;
    int16_t *eq_buf;
} FIR_DATA_T;

#define PP_PINGPANG(v) \
    (v == PP_PING ? PP_PANG: PP_PING)


typedef struct{
    FIR_PP_T    pp;
    uint32_t    single_run_eq_buf_len;
    FIR_DATA_T  data[2];

    int16_t offset_num;

    uint16_t coef_len;
    uint32_t sample_rate;
    uint32_t sample_bits;
    enum AUD_CHANNEL_NUM_T ch_num;
} FIR_RUN_CFG_T;

#define FIREQ_DMA_LINKLIST_NUM_MAX (200)

static struct HAL_DMA_DESC_T fir_dma_in_desc[2][FIREQ_DMA_LINKLIST_NUM_MAX];
static struct HAL_DMA_DESC_T fir_dma_out_desc[2][FIREQ_DMA_LINKLIST_NUM_MAX];

static struct HAL_DMA_CH_CFG_T fir_dma_cfg_in, fir_dma_cfg_out;
static struct HAL_DMA_2D_CFG_T fir_in_2d_src, fir_in_2d_dst;
static struct HAL_DMA_2D_CFG_T fir_out_2d_src, fir_out_2d_dst;

volatile static int hw_fir_open_flag=0;
static FIR_RUN_CFG_T fir_run_cfg;
static uint32_t fir_sample_start_l,fir_sample_start_r;
volatile static int fir_finished_flag=0;

static int timer_tick_start=0;
static int timer_tick_end=0;
// Used by fir filter input, fir filter just can deal with 32bits
static void memcpy_samples(void *dest, void *src, int num)
{
    fir_sample_24bits_t *sample_src = (fir_sample_24bits_t *)src;
    fir_sample_24bits_t *sample_dest = (fir_sample_24bits_t *)dest;
    
    if(fir_run_cfg.sample_bits == 16)
    {
        for (int i = 0; i < num/2; i++)
        {
            sample_dest[i] = sample_src[i];
        }
    }
    else
    {        
        for (int i = 0; i < num; i++)
        {
            sample_dest[i] = sample_src[i];
        }
    }

}

static void memset_samples(uint8_t *dest, int value, int num)
{
    if(fir_run_cfg.sample_bits == 16)
    {
        fir_sample_16bits_t *sample_dest = (fir_sample_16bits_t *)dest;
        for (int i = 0; i < num; i++)
        {
            sample_dest[i] = value;
        }
    }
    else
    {
        // 24 or 32 bits
        fir_sample_24bits_t *sample_dest = (fir_sample_24bits_t *)dest;
        for (int i = 0; i < num; i++)
        {
            sample_dest[i] = value;
        }        
    }
}

static void coefcpy_int16_to_int32(volatile int32_t *dest, const int32_t *src, int16_t num)
{
    int16_t i;

    for (i = 0; i < num; i++)
    {
        //dest[i] = src[i]*(int32_t)256;
        dest[i] = src[i];
    }

    return;
}

static void coefset_int16_to_int32(volatile int32_t *dest, int16_t value, int16_t num)
{
    int16_t i;
    for (i = 0; i < num; i++)
    {
        dest[i] = value;
    }
    return;
}

// hardware
inline static int dsd_start(int PingPang)
{
    

    if(fir_run_cfg.ch_num==1)
    {
        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {
            hal_dma_sg_2d_start(&fir_dma_in_desc[PingPang][0], &fir_dma_cfg_in, NULL, &fir_in_2d_dst);
            hal_dma_sg_2d_start(&fir_dma_out_desc[PingPang][0], &fir_dma_cfg_out, &fir_out_2d_src, NULL);
        }
        else
        {
            hal_audma_sg_start(&fir_dma_in_desc[PingPang][0], &fir_dma_cfg_in);
            hal_audma_sg_start(&fir_dma_out_desc[PingPang][0], &fir_dma_cfg_out);
        }
        
        fir_setings.fir_config0->fir_enable_ch0= 1 ;
    }
    else if(fir_run_cfg.ch_num==2)
    {

        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {
            hal_dma_sg_2d_start(&fir_dma_in_desc[PingPang][0], &fir_dma_cfg_in, &fir_in_2d_src, &fir_in_2d_dst);
            hal_dma_sg_2d_start(&fir_dma_out_desc[PingPang][0], &fir_dma_cfg_out, &fir_out_2d_src, &fir_out_2d_dst);
        }
        else
        {
            hal_dma_sg_2d_start(&fir_dma_in_desc[PingPang][0], &fir_dma_cfg_in, &fir_in_2d_src, NULL);
            hal_dma_sg_2d_start(&fir_dma_out_desc[PingPang][0], &fir_dma_cfg_out, NULL, &fir_out_2d_dst);
        }        

        fir_setings.fir_config0->fir_enable_ch0= 1 ;
        fir_setings.fir_config0->fir_enable_ch1= 1 ;
    }
    //timer_tick_start=hal_sys_timer_get();   
   // LOG_I("%s:%d\n", __func__,timer_tick_start);

	return 0;
}

inline static int dsd_stop(void)
{
    //LOG_I("%s\n", __func__);
    if(fir_run_cfg.ch_num==1)
    {
        fir_setings.fir_config0->fir_enable_ch0= 0 ;
    }   
    else if(fir_run_cfg.ch_num==2)
    {
        fir_setings.fir_config0->fir_enable_ch0= 0 ;
        fir_setings.fir_config0->fir_enable_ch1= 0 ;
    }

    hal_dma_stop(fir_dma_cfg_in.ch);
    hal_dma_stop(fir_dma_cfg_out.ch);
    
    return 0;
}

static void dsd_irq_handler(uint8_t chan, uint32_t remain_tsize, uint32_t error, struct HAL_DMA_DESC_T *lli)
{
    FIR_PP_T pp;


    //    timer_tick_end=hal_sys_timer_get();   


  //  LOG_I("%s.error:%d.\n", __func__,error);
    dsd_stop();  

    // init parameter
    pp = fir_run_cfg.pp;

    // store output data, set new data

    
    //LOG_I("%s.fir_run_cfg.data[pp].eq_num:%d.\n", __func__,fir_run_cfg.data[pp].eq_num);

    fir_run_cfg.offset_num = 0;
    fir_run_cfg.data[pp].state = FIR_DATA_DONE;

    fir_finished_flag=1;
    
}

static int dsd_buf_init(int16_t *input, uint32_t total_num, int16_t PingPang)
{  
    int link_num = 0, loop = 0, i = 0;

    LOG_I("%s,len:%d,ch:%d.\n", __func__, total_num,fir_run_cfg.ch_num);

    fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_sample_start=fir_sample_start_l;
    fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->fir_sample_start=fir_sample_start_r;

    loop = (total_num)/(FIR_MEM_LEN-fir_run_cfg.coef_len);
    
    ASSERT(loop <= FIREQ_DMA_LINKLIST_NUM_MAX, "[%s] loop:%u <= FIREQ_DMA_LINKLIST_NUM_MAX:%d", __func__, loop,FIREQ_DMA_LINKLIST_NUM_MAX);

    loop=loop/fir_run_cfg.ch_num;

    //config input DMA
    fir_dma_cfg_in.dst = 0; // useless
    fir_dma_cfg_in.dst_bsize = HAL_DMA_BSIZE_8;
    fir_dma_cfg_in.dst_periph = HAL_AUDMA_FIR_TX;
    fir_dma_cfg_in.handler = 0; //NULL
    fir_dma_cfg_in.src_bsize = HAL_DMA_BSIZE_8;
    fir_dma_cfg_in.src_periph = 0; // useless
    fir_dma_cfg_in.try_burst = 1;
    fir_dma_cfg_in.type = HAL_DMA_FLOW_M2P_DI_DMA; 

    if(fir_run_cfg.sample_bits==AUD_BITS_16)
    {
        fir_dma_cfg_in.dst_width = HAL_DMA_WIDTH_HALFWORD;
        fir_dma_cfg_in.src_width = HAL_DMA_WIDTH_HALFWORD;
    }
    else
    {
        fir_dma_cfg_in.dst_width = HAL_DMA_WIDTH_WORD;
        fir_dma_cfg_in.src_width = HAL_DMA_WIDTH_WORD;
    }

    if(loop!=0)	
    {
        fir_dma_cfg_in.src_tsize = FIR_MEM_LEN-fir_run_cfg.coef_len;	    
        link_num = 0;

        for (i = 0; i < loop; i++) 
        {

          /*  if(fir_run_cfg.sample_bits==AUD_BITS_16)
            {
                fir_dma_cfg_in.src = (uint32_t)(input) + fir_dma_cfg_in.src_tsize*fir_run_cfg.ch_num*i*2;
            }
            else*/
            {
                fir_dma_cfg_in.src = (uint32_t)(input) + fir_dma_cfg_in.src_tsize*fir_run_cfg.ch_num*i*4;
            }
            
            hal_audma_init_desc(&fir_dma_in_desc[PingPang][link_num], &fir_dma_cfg_in, &fir_dma_in_desc[PingPang][link_num + 1], 0);
            fir_dma_in_desc[PingPang][link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].sample + fir_sample_start_l);
            fir_sample_start_l=(fir_dma_cfg_in.src_tsize+ fir_sample_start_l)%FIR_MEM_LEN;
           
            link_num++;


       /*     if(fir_run_cfg.sample_bits==AUD_BITS_16)
            {
                fir_dma_cfg_in.src = (uint32_t)(input) + (fir_dma_cfg_in.src_tsize*fir_run_cfg.ch_num*i+1)*2;
            }
            else*/
            {
                fir_dma_cfg_in.src = (uint32_t)(input) + (fir_dma_cfg_in.src_tsize*fir_run_cfg.ch_num*i+8)*4;
            }

            if(i==loop-1)
            {
                hal_audma_init_desc(&fir_dma_in_desc[PingPang][link_num], &fir_dma_cfg_in, 0, 0); //disable interupt
            }
            else
            {
                hal_audma_init_desc(&fir_dma_in_desc[PingPang][link_num], &fir_dma_cfg_in, &fir_dma_in_desc[PingPang][link_num + 1], 0);
            }
            fir_dma_in_desc[PingPang][link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_1].sample  + fir_sample_start_r);
            fir_sample_start_r=(fir_dma_cfg_in.src_tsize+ fir_sample_start_r)%FIR_MEM_LEN;

            
            link_num++;

        }

    }
    else
    {
        fir_dma_cfg_in.src_tsize =total_num/fir_run_cfg.ch_num;
        link_num = 0;

        fir_dma_cfg_in.src = (uint32_t)(input);
        hal_audma_init_desc(&fir_dma_in_desc[PingPang][link_num], &fir_dma_cfg_in, &fir_dma_in_desc[PingPang][link_num + 1], 0); //disable interupt
        fir_dma_in_desc[PingPang][link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].sample   + fir_sample_start_l);
        fir_sample_start_l=(fir_dma_cfg_in.src_tsize+ fir_sample_start_l)%FIR_MEM_LEN;

        link_num++;
        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {
            fir_dma_cfg_in.src = (uint32_t)(input)+1*2;
        }
        else
        {
            fir_dma_cfg_in.src = (uint32_t)(input)+1*4;
        }
        hal_audma_init_desc(&fir_dma_in_desc[PingPang][link_num], &fir_dma_cfg_in, 0, 0); //disable interupt
        fir_dma_in_desc[PingPang][link_num].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_1].sample   + fir_sample_start_r);
        fir_sample_start_r=(fir_dma_cfg_in.src_tsize+ fir_sample_start_r)%FIR_MEM_LEN;

    }

    fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_sample_num= fir_dma_cfg_in.src_tsize/16;
    fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->fir_sample_num= fir_dma_cfg_in.src_tsize/16;

    //config fir filter out.
    if(loop!=0)
    {
        fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->fir_loop_num=loop;
        fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config1->fir_loop_num=loop;
    }
    else
    {
        fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->fir_loop_num=1;
        fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config1->fir_loop_num=1;
    }



    //config out DMA
    fir_dma_cfg_out.dst_bsize = HAL_DMA_BSIZE_1;
    fir_dma_cfg_out.dst_periph = 0; //useless
    fir_dma_cfg_out.handler = dsd_irq_handler;
    fir_dma_cfg_out.src = 0; // useless
    fir_dma_cfg_out.src_bsize = HAL_DMA_BSIZE_4;
    fir_dma_cfg_out.src_periph = HAL_AUDMA_FIR_RX;
    fir_dma_cfg_out.try_burst = 1;
    fir_dma_cfg_out.type = HAL_DMA_FLOW_P2M_SI_DMA;


    if(fir_run_cfg.sample_bits==AUD_BITS_16)
    {
        fir_dma_cfg_out.dst_width = HAL_DMA_WIDTH_HALFWORD;
        fir_dma_cfg_out.src_width = HAL_DMA_WIDTH_HALFWORD;
    }
    else
    {
        fir_dma_cfg_out.dst_width = HAL_DMA_WIDTH_WORD;
        fir_dma_cfg_out.src_width = HAL_DMA_WIDTH_WORD;
    }
    

    if(loop!=0)
    {
        fir_dma_cfg_out.src_tsize = (FIR_MEM_LEN-fir_run_cfg.coef_len)/16;
        link_num = 0;

        for (i = 0; i < loop; i++)
        {
           /* if(fir_run_cfg.sample_bits==AUD_BITS_16)
            {
                fir_dma_cfg_out.dst = (uint32_t)(input) + fir_dma_cfg_out.src_tsize*fir_run_cfg.ch_num*i*2;
            }
            else*/
            {
                fir_dma_cfg_out.dst = (uint32_t)(input) + fir_dma_cfg_out.src_tsize*fir_run_cfg.ch_num*i*4;
            }               
            
            hal_audma_init_desc(&fir_dma_out_desc[PingPang][link_num], &fir_dma_cfg_out, &fir_dma_out_desc[PingPang][link_num + 1], 0);
            fir_dma_out_desc[PingPang][link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].coef +fir_run_cfg.coef_len);

            link_num++;
          /*  if(fir_run_cfg.sample_bits==AUD_BITS_16)
            {
                fir_dma_cfg_out.dst = (uint32_t)(input) + (fir_dma_cfg_out.src_tsize*fir_run_cfg.ch_num*i+1)*2;
            }
            else*/
            {
                fir_dma_cfg_out.dst = (uint32_t)(input) + (fir_dma_cfg_out.src_tsize*fir_run_cfg.ch_num*i+1)*4;
            }               

            if(i==loop-1)
            {
                hal_audma_init_desc(&fir_dma_out_desc[PingPang][link_num], &fir_dma_cfg_out, 0, 1); //enable interupt
            }
            else
            {
                hal_audma_init_desc(&fir_dma_out_desc[PingPang][link_num], &fir_dma_cfg_out, &fir_dma_out_desc[PingPang][link_num + 1], 0);
            }
            fir_dma_out_desc[PingPang][link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_1].coef +fir_run_cfg.coef_len);

            link_num++;
        }

    }
    else
    {
        fir_dma_cfg_out.src_tsize =total_num/fir_run_cfg.ch_num;
        link_num = 0;

        fir_dma_cfg_out.dst = (uint32_t)(input);
        hal_audma_init_desc(&fir_dma_out_desc[PingPang][link_num], &fir_dma_cfg_out, &fir_dma_out_desc[PingPang][link_num + 1], 0); //disable interupt
        fir_dma_out_desc[PingPang][link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].coef+fir_run_cfg.coef_len);

        link_num++;
        
        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {
            fir_dma_cfg_out.dst = (uint32_t)(input) + 1*2;
        }
        else
        {
            fir_dma_cfg_out.dst = (uint32_t)(input) + 1*4;
        }
        
        hal_audma_init_desc(&fir_dma_out_desc[PingPang][link_num], &fir_dma_cfg_out, 0, 1); //enable interupt
        fir_dma_out_desc[PingPang][link_num].src =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_1].coef+fir_run_cfg.coef_len);

    }
    memset(&fir_in_2d_src, 0, sizeof(fir_in_2d_src));
    memset(&fir_out_2d_dst, 0, sizeof(fir_out_2d_dst));
    fir_in_2d_src.xmodify = 8;
    fir_in_2d_src.xcount = FIR_MEM_LEN;
    fir_in_2d_src.ymodify = 0;
    fir_in_2d_src.ycount = 1;        

    fir_out_2d_dst.xmodify = 1;
    fir_out_2d_dst.xcount = FIR_MEM_LEN;
    fir_out_2d_dst.ymodify = 0;
    fir_out_2d_dst.ycount = 1;
     
    //LOG_I("%s:%d loop %d, ch1 %d, ch2 %d\n", __func__, __LINE__, loop, fir_dma_cfg_in.ch, fir_dma_cfg_out.ch);
   
    return 0;
}

int dsd_process(uint8_t *buf, uint32_t total_num)
{
    FIR_PP_T pp_empty;
    FIR_PP_T pp_full;
    FIR_DATA_STATE_T state_full;
    int i;

    if(fir_run_cfg.sample_bits == AUD_BITS_16)
    {
        total_num = total_num/2;
    }
    else if(fir_run_cfg.sample_bits == AUD_BITS_24)
    {
        total_num = total_num/4;
    }
    else
    {
        ASSERT(0, "[%s] bits(%d) is invalid", __func__, fir_run_cfg.sample_bits);
    }

    //unsigned int *testdata=(unsigned int *)buf;
    //LOG_I("%s,total_num:%d.0x%x,0x%x,0x%x,0x%x,\n", __func__, total_num,testdata[0],testdata[1],testdata[2],testdata[3]);
   // LOG_I("ticks:%d\n",hal_sys_timer_get());

    timer_tick_start=hal_sys_timer_get();

    if(hw_fir_open_flag==1)
    {
        //LOG_I("[%s] len=%d pp=%d state=(%d,%d)", __func__, len, fir_run_cfg.pp, fir_run_cfg.data[0].state, fir_run_cfg.data[1].state);
        if(fir_run_cfg.sample_bits==AUD_BITS_16)
        {
            ASSERT(total_num <= fir_run_cfg.single_run_eq_buf_len, "[%s] len(%u) > single_run_eq_buf_len(%u)", __func__, total_num, fir_run_cfg.single_run_eq_buf_len);
        }
        else
        {
            ASSERT(total_num <= fir_run_cfg.single_run_eq_buf_len/2, "[%s] len(%u) > single_run_eq_buf_len(%u)", __func__, total_num, fir_run_cfg.single_run_eq_buf_len);
        }


        pp_full = fir_run_cfg.pp;
        pp_empty = PP_PINGPANG(pp_full);


        // app --> eq
//        memcpy_samples(fir_run_cfg.data[pp_empty].eq_buf, buf, total_num);

        
        if(1)
        {
            unsigned char *Bit_In=(unsigned char *)buf;
            int *Pcm_Out=(int *)(fir_run_cfg.data[pp_empty].eq_buf);
            unsigned char val_l,val_r;
            const int *Pcm_In;
            
            for(i=0;i<total_num;i=i+2)
            {
                val_l=Bit_In[i*4+1];
                val_r=Bit_In[i*4+4+1];
                
                //memcpy(Pcm_Out,&(BitToValue[val_l][0]),8*4);
                Pcm_In=&(BitToValue[val_l][0]);

                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In;

              //  Pcm_Out=Pcm_Out+8;

             //   memcpy(Pcm_Out,&(BitToValue[val_r][0]),8*4);
             
                Pcm_In=&(BitToValue[val_r][0]);

                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In;
                                
                val_l=Bit_In[i*4];
                val_r=Bit_In[i*4+4];
                
              //  memcpy(Pcm_Out,&(BitToValue[val_l][0]),8*4);

                Pcm_In=&(BitToValue[val_l][0]);

                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In;


             //   memcpy(Pcm_Out,&(BitToValue[val_r][0]),8*4);
                Pcm_In=&(BitToValue[val_r][0]);

                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In++;
                *Pcm_Out++=*Pcm_In;
                
            }

        }
        
        
        while(fir_finished_flag==0)
        {
            LOG_I("[%s] waiting......:%d\n", __func__,fir_finished_flag);
        }
        fir_finished_flag=0;
        
        // Make a copy of full state, because IRQ might change it
        state_full = fir_run_cfg.data[pp_full].state;

        ASSERT(fir_run_cfg.data[pp_empty].state == FIR_DATA_IDLE, "Invalid pp_empty state: pp_empty=%d state_empty=%d state_full=%d)",
        pp_empty, fir_run_cfg.data[pp_empty].state, state_full);

        // eq --> af
        if (state_full == FIR_DATA_DONE)
        {
            memcpy_samples(buf, fir_run_cfg.data[pp_full].eq_buf, total_num);
            fir_run_cfg.data[pp_full].state = FIR_DATA_IDLE;
        } 
        else
        {
            // FIR_DATA_IDLE or FIR_DATA_BUSY
            memset_samples(buf, 0, total_num);
            if (state_full == FIR_DATA_BUSY) 
            {
                LOG_I("[%s] pp_full is no full!!!", __func__);
                // Skip this frame
                return 0;
            }
        } 
        
        // Update state
        fir_run_cfg.data[pp_empty].eq_num = total_num;
        fir_run_cfg.data[pp_empty].state = FIR_DATA_BUSY;
        fir_run_cfg.pp = pp_empty;

        fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_sample_start=fir_sample_start_l;
        fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->fir_sample_start=fir_sample_start_r;

        for(i=0;i<(total_num*16)/128;)
        {
            fir_dma_in_desc[pp_empty][i].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_0].sample + fir_sample_start_l);
            fir_sample_start_l=(fir_dma_cfg_in.src_tsize+ fir_sample_start_l)%FIR_MEM_LEN;
            i++;
            fir_dma_in_desc[pp_empty][i].dst =(uint32_t) (fir_setings.fir_ch_config[FIR_CHNL_ID_1].sample  + fir_sample_start_r);
            fir_sample_start_r=(fir_dma_cfg_in.src_tsize+ fir_sample_start_r)%FIR_MEM_LEN;
            i++;
        }

        dsd_start(pp_empty);

        timer_tick_end=hal_sys_timer_get();
        //LOG_I("%s:%d\n", __func__,timer_tick_end-timer_tick_start);

        
        return 0;
    }
    else
    {
        LOG_I("[%s]HW FIR MUSET BE OPENED FIRSTLY!\n", __func__);
        return 1;
    }
        
}



static int dsd_set_cfg(const DSD_CFG_T *cfg)
{
    int8_t gain0 = 0;
    int8_t gain1 = 0;
    int16_t coef_len = 0;
    const int32_t *coef_ptr = NULL;

    LOG_I("[%s]", __func__);

    if(hw_fir_open_flag==1)
    {   
        gain0 = cfg->gain0;
        gain1 = cfg->gain1;
        coef_len = cfg->len;
        coef_ptr = cfg->coef;

        fir_run_cfg.coef_len = coef_len;

        ASSERT(coef_ptr != NULL, "[%s] coef == NULL", __func__);
        ASSERT(coef_len < FIR_MEM_LEN, "[%s] coef_len(%d) > FIR_MEM_LEN", __func__, coef_len);

        fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_order=coef_len;
        fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_sample_start=fir_sample_start_l=coef_len;
        fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_sample_num=0;	

        fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->fir_result_base_addr=coef_len;
        fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->fir_gain_sel=gain0;

        fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->fir_order=coef_len;
        fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->fir_sample_start=fir_sample_start_r=coef_len;
        fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->fir_sample_num=0;	

        fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config1->fir_result_base_addr=coef_len;
        fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config1->fir_gain_sel=gain1;

        coefcpy_int16_to_int32(fir_setings.fir_ch_config[FIR_CHNL_ID_0].coef, coef_ptr, coef_len);
        coefcpy_int16_to_int32(fir_setings.fir_ch_config[FIR_CHNL_ID_1].coef, coef_ptr, coef_len);
        
        coefset_int16_to_int32(fir_setings.fir_ch_config[FIR_CHNL_ID_0].sample, 0x0, FIR_MEM_LEN);
        coefset_int16_to_int32(fir_setings.fir_ch_config[FIR_CHNL_ID_1].sample, 0x0, FIR_MEM_LEN);

        dsd_buf_init(fir_run_cfg.data[0].eq_buf,fir_run_cfg.single_run_eq_buf_len/(2),0);
        dsd_buf_init(fir_run_cfg.data[1].eq_buf,fir_run_cfg.single_run_eq_buf_len/(2),1);

        fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_sample_start=fir_sample_start_l=coef_len;
        fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->fir_sample_start=fir_sample_start_r=coef_len;

        
        return 0;
    }
    else
    {
        LOG_I("[%s]HW FIR MUSET BE OPENED FIRSTLY!\n", __func__);
        return 1;
    }
}

int dsd_open(enum AUD_SAMPRATE_T sample_rate, enum AUD_BITS_T sample_bits,enum AUD_CHANNEL_NUM_T ch_num, void *eq_buf, uint32_t len)
{
    LOG_I("[%s] sample_rate:%d,sample_bits:%d,ch_num:%d,len=%u", __func__,sample_rate,sample_bits,ch_num,len);

    // Check parameter
    fir_run_cfg.sample_rate = sample_rate;
    fir_run_cfg.sample_bits = sample_bits;
    fir_run_cfg.ch_num = ch_num;

    // Parameter initialize
    fir_run_cfg.pp = PP_PING;
    fir_run_cfg.data[0].state = FIR_DATA_IDLE;
    fir_run_cfg.data[1].state = FIR_DATA_IDLE;

    ASSERT(eq_buf && ((uint32_t)eq_buf & 0x3) == 0, "%s: Invalid eq_buf=%p", __func__, eq_buf);
    ASSERT(len && (len & 0x3) == 0, "%s: Invalid eq_buf len=%u", __func__, len);

    memset(eq_buf, 0, len);

    fir_run_cfg.single_run_eq_buf_len = len / sizeof(int16_t) / 2;
    fir_run_cfg.data[0].eq_buf = eq_buf;
    fir_run_cfg.data[1].eq_buf = fir_run_cfg.data[0].eq_buf + fir_run_cfg.single_run_eq_buf_len;

    // NOTE: offset_num MUST be initialized here as dsd_run() will not do that
    fir_run_cfg.offset_num = 0;

    fir_setings.fir_config0->fir_stream_enable_ch0=0;
    fir_setings.fir_config0->fir_stream_enable_ch1=0;

    fir_setings.fir_config0->fir_enable_ch0=0;
    fir_setings.fir_config0->fir_enable_ch1=0;
    //fir_setings.fir_config0->fir_enable_ch2=0;
    //fir_setings.fir_config0->fir_enable_ch3=0;

    fir_setings.fir_config0->dma_ctrl_rx_fir=1;
    fir_setings.fir_config0->dma_ctrl_tx_fir=1;

    if(sample_bits==AUD_BITS_16)
    {
        fir_setings.fir_config0->mode_32bit_fir=0;

        fir_setings.fir_config0->mode_16bit_fir_tx_ch0=1;
        fir_setings.fir_config0->mode_16bit_fir_rx_ch0=1;
        fir_setings.fir_config0->mode_16bit_fir_tx_ch1=1;
        fir_setings.fir_config0->mode_16bit_fir_rx_ch1=1;
        //fir_setings.fir_config0->mode_16bit_fir_tx_ch2=0;
        //fir_setings.fir_config0->mode_16bit_fir_rx_ch2=0;
        //fir_setings.fir_config0->mode_16bit_fir_tx_ch3=0;
        //fir_setings.fir_config0->mode_16bit_fir_rx_ch3=0;
    }
    else if(sample_bits==AUD_BITS_24)
    {
        fir_setings.fir_config0->mode_32bit_fir=0;

        fir_setings.fir_config0->mode_16bit_fir_tx_ch0=0;
        fir_setings.fir_config0->mode_16bit_fir_rx_ch0=0;
        fir_setings.fir_config0->mode_16bit_fir_tx_ch1=0;
        fir_setings.fir_config0->mode_16bit_fir_rx_ch1=0;
        //fir_setings.fir_config0->mode_16bit_fir_tx_ch2=0;
        //fir_setings.fir_config0->mode_16bit_fir_rx_ch2=0;
        //fir_setings.fir_config0->mode_16bit_fir_tx_ch3=0;
        //fir_setings.fir_config0->mode_16bit_fir_rx_ch3=0;
    }
    else if(sample_bits==AUD_BITS_32)
    {
        fir_setings.fir_config0->mode_32bit_fir=1;

        fir_setings.fir_config0->mode_16bit_fir_tx_ch0=0;
        fir_setings.fir_config0->mode_16bit_fir_rx_ch0=0;
        fir_setings.fir_config0->mode_16bit_fir_tx_ch1=0;
        fir_setings.fir_config0->mode_16bit_fir_rx_ch1=0;
        //fir_setings.fir_config0->mode_16bit_fir_tx_ch2=0;
        //fir_setings.fir_config0->mode_16bit_fir_rx_ch2=0;
        //fir_setings.fir_config0->mode_16bit_fir_tx_ch3=0;
        //fir_setings.fir_config0->mode_16bit_fir_rx_ch3=0;
    }
    else
    {
         ASSERT(false, "%s: Invalid sample bits:%d", __func__, sample_bits);
    }
    
    fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->stream0_fir1=1;
    fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_mode=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_order=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_sample_start=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_sample_num=0;	

    fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->fir_result_base_addr=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->fir_slide_offset=16;
    // fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->fir_burst_length=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->fir_gain_sel=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config1->fir_loop_num=0;
    // fir_setings.fir_ch_config[FIR_CHNL_ID_0].fir_ch_config0->fir_do_remap=0;


    fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->stream0_fir1=1;
    fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->fir_mode=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->fir_order=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->fir_sample_start=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->fir_sample_num=0;	

    fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config1->fir_result_base_addr=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config1->fir_slide_offset=16;
    //  fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config1->fir_burst_length=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config1->fir_gain_sel=0;
    fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config1->fir_loop_num=0;
    //  fir_setings.fir_ch_config[FIR_CHNL_ID_1].fir_ch_config0->fir_do_remap=0;

    fir_sample_start_l=0;
    fir_sample_start_r=0;
    
    memset(&fir_dma_cfg_in, 0, sizeof(fir_dma_cfg_in));
    fir_dma_cfg_in.ch = hal_audma_get_chan(HAL_AUDMA_FIR_TX, HAL_DMA_HIGH_PRIO);

    memset(&fir_dma_cfg_out, 0, sizeof(fir_dma_cfg_out));
    fir_dma_cfg_out.ch = hal_audma_get_chan(HAL_AUDMA_FIR_RX, HAL_DMA_HIGH_PRIO);

    hw_fir_open_flag=1;

    fir_finished_flag=1;

    if(AUD_SAMPRATE_176400==fir_run_cfg.sample_rate)
    {
        dsd_set_cfg(&audio_eq_hw_fir_cfg_176_4k);
    }
    else
    {
        dsd_set_cfg(&audio_eq_hw_fir_cfg_176_4k);
    }

    //LOG_I("[%s]hw_fir_open_flag:%d.\n", __func__,hw_fir_open_flag);

    return 0;
}

int dsd_close(void)
{
    LOG_I("%s\n", __func__);

    if(hw_fir_open_flag==1)
    {   
        dsd_stop();
        
        hal_dma_free_chan(fir_dma_cfg_in.ch);
        hal_dma_free_chan(fir_dma_cfg_out.ch);
        
        fir_dma_cfg_in.ch=HAL_DMA_CHAN_NONE;
        fir_dma_cfg_out.ch=HAL_DMA_CHAN_NONE;
        
        hw_fir_open_flag=0;
    }

    return 0;
}
