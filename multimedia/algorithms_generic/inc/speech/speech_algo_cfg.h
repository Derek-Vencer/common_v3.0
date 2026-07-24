/***************************************************************************
 *
 * Copyright 2015-2023 BES.
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
#ifndef __SPEECH_ALGO_CFG_H__
#define __SPEECH_ALGO_CFG_H__

#ifndef SPEECH_HIFI_V1
#include "speech_1mic_preaf.h"
#include "speech_1mic_ns.h"
#include "speech_2mic_preaf.h"
#include "speech_2mic_ns8.h"
#include "speech_ns4.h"
#endif
#include "echo_canceller.h"
#include "speech_eq.h"
#include "compexp.h"
#include "lc_mmse_ns_float.h"

typedef struct {
#if defined(SPEECH_TX_1MIC_PREAF)
    Speech1MicPreafConfig tx_1mic_preaf_cfg;
#endif
#if defined(SPEECH_TX_1MIC_NS)
    Speech1MicNSConfig tx_1mic_ns_cfg;
#endif
#if defined(SPEECH_TX_2MIC_PREAF)
    Speech2MicPreafConfig tx_2mic_preaf_cfg;
#endif
#if defined(SPEECH_TX_2MIC_NS8)
    Speech2MicNs8Config tx_2mic_ns8_cfg;
#endif
#if defined(SPEECH_TX_AEC2FLOAT)
    Ec2FloatConfig tx_aec2float_cfg;
#endif
#if defined(SPEECH_TX_NS4)
    SpeechNs4Config tx_ns4_cfg;
#endif
#if defined(SPEECH_TX_COMPEXP)
    CompexpConfig tx_compexp_cfg;
#endif
#if defined(SPEECH_TX_EQ)
    EqConfig tx_eq_cfg;
#endif
#if defined(SPEECH_RX_NS2FLOAT)
    SpeechNs2FloatConfig rx_ns2float_cfg;
#endif
#if defined(SPEECH_RX_COMPEXP)
    CompexpConfig rx_compexp_cfg;
#endif
#if defined(SPEECH_RX_EQ)
    EqConfig rx_eq_cfg;
#endif
} SpeechAlgoConfig;

#endif