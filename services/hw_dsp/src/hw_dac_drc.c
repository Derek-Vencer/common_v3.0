/***************************************************************************
 *
 * Copyright 2015-2021 BES.
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

#include "hal_trace.h"
#include "hw_dac_drc.h"
#include "tgt_hardware.h"

#include "psap_process.h"

#ifndef CODEC_OUTPUT_DEV
#define CODEC_OUTPUT_DEV                    CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV
#endif

extern const struct_psap_cfg * hw_dac_drc_para_list_15p6k[HW_DAC_DRC_PARA_LIST_NUM];
extern const struct_psap_cfg * hw_dac_drc_para_list_46p8k[HW_DAC_DRC_PARA_LIST_NUM];
extern const struct_psap_cfg * hw_dac_drc_para_list_93p7k[HW_DAC_DRC_PARA_LIST_NUM];
extern const struct_psap_cfg * hw_dac_drc_para_list_187p5k[HW_DAC_DRC_PARA_LIST_NUM];

const struct_psap_cfg * WEAK hw_dac_drc_para_list_15p6k[HW_DAC_DRC_PARA_LIST_NUM] = { };
const struct_psap_cfg * WEAK hw_dac_drc_para_list_46p8k[HW_DAC_DRC_PARA_LIST_NUM] = { };
const struct_psap_cfg * WEAK hw_dac_drc_para_list_93p7k[HW_DAC_DRC_PARA_LIST_NUM] = { };
const struct_psap_cfg * WEAK hw_dac_drc_para_list_187p5k[HW_DAC_DRC_PARA_LIST_NUM] = { };

int hw_dac_drc_open(int ch_map)
{
    HW_DSP_TRACE(1,"%s", __func__);
    int err = 0;
    err = psap_open(ch_map);
    return err;
}

void hw_dac_drc_enable(int ch_map)
{
    HW_DSP_TRACE(1,"%s", __func__);
    psap_enable(ch_map);
}

void hw_dac_drc_disable(int ch_map)
{
    HW_DSP_TRACE(1,"%s", __func__);
    psap_disable(ch_map);
}

void hw_dac_drc_close(void)
{
    HW_DSP_TRACE(1,"%s", __func__);
    psap_close();
}

int hw_dac_drc_set_cfg(enum AUD_SAMPRATE_T rate,int index)
{
    HW_DSP_TRACE(1,"%s", __func__);
    const struct_psap_cfg **list=NULL;

    if (index >= HW_DAC_DRC_PARA_LIST_NUM) {
        return 1;
    }

    switch(rate)
    {
        case AUD_SAMPRATE_16000:
            list=hw_dac_drc_para_list_15p6k;
            break;

        // case AUD_SAMPRATE_32000:
        //     list=hw_dac_drc_para_list_46p8k;
        //     break;

        case AUD_SAMPRATE_48000:
            list=hw_dac_drc_para_list_46p8k;
            break;

        case AUD_SAMPRATE_44100:
            list=hw_dac_drc_para_list_46p8k;
            break;

#ifdef __AUDIO_RESAMPLE__
        case AUD_SAMPRATE_50781:
            list=hw_dac_drc_para_list_46p8k;
            break;
#endif
        case AUD_SAMPRATE_96000:
            list=hw_dac_drc_para_list_93p7k;
            break;

        case AUD_SAMPRATE_192000:
            list=hw_dac_drc_para_list_187p5k;
            break;

        default:
            break;
    }

    ASSERT(list!=NULL&&list[index]!=NULL,"The hw dac para of Samprate %d is NULL",rate);

    if(psap_opened())
    {
        psap_set_cfg_coef(list[index]);
    }

    return 0;
}
