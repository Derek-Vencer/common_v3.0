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
#ifndef __BT_SCO_CHAIN_CFG_H__
#define __BT_SCO_CHAIN_CFG_H__

#include "speech_cfg.h"

typedef struct {
#if defined(SPEECH_TX_DC_FILTER)
    SpeechDcFilterConfig    tx_dc_filter;
#endif
#if defined(SPEECH_TX_AEC2FLOAT)
    Ec2FloatConfig          tx_aec2float;
#endif
#if defined(SPEECH_TX_1MIC_PREAF)
    Speech1MicPreafConfig     tx_1mic_preaf;
#endif
#if defined(SPEECH_TX_1MIC_NS)
    Speech1MicNSConfig     tx_1mic_ns;
#endif
#if defined(SPEECH_TX_NS9)
    SpeechNs9Config         tx_ns9;
#endif
#if defined(SPEECH_TX_2MIC_NS4)
    SensorMicDenoiseConfig  tx_2mic_ns4;
#endif
#if defined(SPEECH_TX_2MIC_NS7)
    Speech2MicNs7Config     tx_2mic_ns7;
#endif
#if defined(SPEECH_TX_2MIC_PREAF)
    Speech2MicPreafConfig     tx_2mic_preaf;
#endif
#if defined(SPEECH_TX_2MIC_NS8)
    Speech2MicNs8Config     tx_2mic_ns8;
#endif
#if defined(SPEECH_TX_2MIC_NS10)
    Speech2MicNs10Config    tx_2mic_ns10;
#endif
#if defined(SPEECH_TX_3MIC_PREAF)
    Speech3MicPreafConfig     tx_3mic_preaf[4];
#endif
#if defined(SPEECH_TX_3MIC_NS)
    Speech3MicNsConfig      tx_3mic_ns[3];
#endif
#if defined(SPEECH_TX_3MIC_NS2)
    Speech3MicNs2Config      tx_3mic_ns2;
#endif
#if defined(SPEECH_TX_NS2FLOAT)
    SpeechNs2FloatConfig    tx_ns2float;
#endif
#if defined(SPEECH_TX_NS3)
    Ns3Config               tx_ns3;
#endif
#if defined(SPEECH_TX_NS4)
    SpeechNs4Config         tx_ns4;
#endif
#if defined(SPEECH_TX_COMPEXP)
    CompexpConfig           tx_compexp;
#endif
#if defined(SPEECH_TX_AGC)
    AgcConfig               tx_agc;
#endif
#if defined(SPEECH_TX_EQ)
    EqConfig                tx_eq;
#endif
#if defined(SPEECH_RX_NS2FLOAT)
    SpeechNs2FloatConfig    rx_ns2float;
#endif
#if defined(SPEECH_RX_NS3)
    Ns3Config               rx_ns3;
#endif
#if defined(SPEECH_RX_COMPEXP)
    MultiCompexpConfig      rx_compexp;
#endif
#if defined(SPEECH_RX_AGC)
    AgcConfig               rx_agc;
#endif
#if defined(SPEECH_RX_EQ)
    EqConfig                rx_eq;
#endif
#if defined(SPEECH_RX_HW_EQ)
    EqConfig                rx_hw_eq;
#endif
#if defined(SPEECH_RX_DAC_EQ)
    EqConfig                rx_dac_eq;
#endif
    // Add more process
} SpeechConfig;

#endif




























