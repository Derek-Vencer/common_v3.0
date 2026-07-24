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
#ifndef __NN_KWS_MODEL_PARAMS_H__
#define __NN_KWS_MODEL_PARAMS_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Structure that holds model parameter data for initializing a SpeechNN model.
 */
typedef struct SpeechNNModelParams {
    unsigned char *weight_data;          ///< Pointer to the model's weight data (e.g., binary weights).
    unsigned char *scale_data;           ///< Pointer to scale data used for quantization/dequantization.
    unsigned char *quant_params_data;    ///< Pointer to additional quantization parameters.

    unsigned int weight_data_len;        ///< Length (in bytes) of the weight data.
    unsigned int scale_data_len;         ///< Length (in bytes) of the scale data.
    unsigned int quant_params_data_len;  ///< Length (in bytes) of the quantization parameters data.

    unsigned int out_dim;                ///< Output dimension of the model (e.g., number of output classes or logits).
} SpeechNNModelParams;

#ifdef __cplusplus
}
#endif

#endif
