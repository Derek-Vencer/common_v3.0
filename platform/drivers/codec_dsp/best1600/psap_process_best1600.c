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
/*******************************************************************************
** namer��PSAP filter
** description��iir, drc and limiter
** version��V1.0
** author�� xuml
** modify��2020.10.6.
*******************************************************************************/

#include "mm_dbg.h"
#include <stdio.h>
#include <string.h>

#include "plat_addr_map.h"
#include CHIP_SPECIFIC_HDR(reg_codec)
#include "cmsis.h"
#include "hal_dma.h"
#include "hal_codec.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_cmu.h"
#include "hal_codec.h"
#include "analog.h"
#include "psap_process.h"

#if defined(AUDIO_PSAP_DEHOWLING_HW) || defined(AUDIO_PSAP_EQ_HW)
#include "anc_process.h"
#endif

/**********************************mocro**************************************/

typedef short int16;
typedef unsigned short uint16;
typedef int   int32;
typedef unsigned int   uint32;



/*
iir coefficients(b/a Q27)
a1
a2
a3

b1
b2
b3
b0
*/
struct _psap_chip_iir_coefs
{
    int32 a1 ;
    int32 a2 ;
    int32 a3 ;

    int32 b1 ;
    int32 b2 ;
    int32 b3 ;
    int32 b0 ;
};

struct _psap_iir_coef
{
    struct _psap_chip_iir_coefs iir0;
    struct _psap_chip_iir_coefs iir1;
};


/*
iir coefficients(b/a Q27)
a1
a2

b1
b2
b3
*/
struct _gain_iir_coefs
{
    int32 a1 ;
    int32 a2 ;

    int32 b1 ;
    int32 b2 ;
    int32 b0 ;
};


#define PSAP_BASE                            ((uint32)CODEC_BASE)
static struct CODEC_T * const codec = (struct CODEC_T *)CODEC_BASE;


#define PSAP_BAND_NUM 17
#define PSAP_IIR_NUM (PSAP_BAND_NUM-1)

#if ANC_PROD_TEST
#define PSAP_DEBUG_TRACE
#else
//#define PSAP_DEBUG_TRACE
#endif

volatile static struct _psap_iir_coef *psap_iir_coef=(volatile struct _psap_iir_coef *)(PSAP_BASE+0x3000);
volatile static struct _gain_iir_coefs *gain_iir_coefs=(volatile struct _gain_iir_coefs *)(PSAP_BASE+0x3000+0x380);

volatile static int psap_open_flag;
volatile static int psap_band_num;
volatile static float psap_ramp_total_gain_l = 1.0f;
volatile static float psap_bands_same_gain_l[PSAP_BAND_NUM];
volatile static float psap_bands_gain_l[PSAP_BAND_NUM];
volatile static int psap_coef_gain_l[PSAP_BAND_NUM];

const aud_item_psap  *psap_cfg_coef_l;

volatile static int psap_output_ch_map = AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1;

// 4 * 70ms
static const anc_iir_coefs   iir_coef_gain_ramp_psap_normal=
{
    .coef_b={512,         1025,          512},
    .coef_a={134217728,   -265823675,    131607997},
};
static void psap_updata_gain(void);
static void psap_updata_coef(unsigned int dest,unsigned int src);

int psap_set_cfg_coef(const struct_psap_cfg * cfg)
{
    int i,j;
    int BandNum;
    int DestIndex;
    PSAP_ERROR err=PSAP_NO_ERR;

    psap_cfg_coef_l=&(cfg->psap_cfg_l);

    LOG_I("%s", __func__);

    LOG_I("psap_total_gain:%d",psap_cfg_coef_l->psap_total_gain);
    LOG_I("psap_band_num:%d",psap_cfg_coef_l->psap_band_num);

    if(psap_open_flag==0)
    {
        LOG_I("%s: PSAP not opened", __func__);
        return PSAP_OTHER_ERR;
    }

    if(psap_cfg_coef_l==NULL)
    {
        LOG_I("%s: cfg is null", __func__);
        return PSAP_OTHER_ERR;
    }

    BandNum=psap_cfg_coef_l->psap_band_num;

    if(BandNum!=17&&BandNum!=9&&BandNum!=5&&BandNum!=3&&BandNum!=2)
    {
        LOG_I("%s: Error band number!:%d", __func__,BandNum);
        return PSAP_NUM_ERR;
    }

#ifdef PSAP_DEBUG_TRACE
    for( j = 0; j <BandNum-1; j++)
    {
        LOG_I("PSAP_IIR_NUM_%d:",j);

        LOG_I("iir0.coef_b: 0x%08x, 0x%08x, 0x%08x, 0x%08x",\
        psap_cfg_coef_l->psap_iir_coef[j].iir0.coef_b[0], \
        psap_cfg_coef_l->psap_iir_coef[j].iir0.coef_b[1], \
        psap_cfg_coef_l->psap_iir_coef[j].iir0.coef_b[2], \
        psap_cfg_coef_l->psap_iir_coef[j].iir0.coef_b[3]);

        LOG_I("iir0.coef_a: 0x%08x, 0x%08x, 0x%08x, 0x%08x",\
        psap_cfg_coef_l->psap_iir_coef[j].iir0.coef_a[0], \
        psap_cfg_coef_l->psap_iir_coef[j].iir0.coef_a[1], \
        psap_cfg_coef_l->psap_iir_coef[j].iir0.coef_a[2], \
        psap_cfg_coef_l->psap_iir_coef[j].iir0.coef_a[3]);

        LOG_I("iir1.coef_b: 0x%08x, 0x%08x, 0x%08x, 0x%08x", \
        psap_cfg_coef_l->psap_iir_coef[j].iir1.coef_b[0], \
        psap_cfg_coef_l->psap_iir_coef[j].iir1.coef_b[1], \
        psap_cfg_coef_l->psap_iir_coef[j].iir1.coef_b[2], \
        psap_cfg_coef_l->psap_iir_coef[j].iir1.coef_b[3]);

        LOG_I("iir1.coef_a: 0x%08x, 0x%08x, 0x%08x, 0x%08x", \
        psap_cfg_coef_l->psap_iir_coef[j].iir1.coef_a[0], \
        psap_cfg_coef_l->psap_iir_coef[j].iir1.coef_a[1], \
        psap_cfg_coef_l->psap_iir_coef[j].iir1.coef_a[2], \
        psap_cfg_coef_l->psap_iir_coef[j].iir1.coef_a[3]);
    }

    for( j = 0; j <BandNum; j++)
    {
        LOG_I("PSAP_BAND_NUM_%02d_GAIN: 0x%08x", j,psap_cfg_coef_l->psap_band_gain[j]);
    }

    for( j = 0; j <BandNum-1; j++)
    {
        LOG_I("band:%04d",j);

        LOG_I("psap_cpd_ct:0x%04x",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_ct);
        LOG_I("psap_cpd_cs:0x%04x",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_cs);

        LOG_I("psap_cpd_wt:0x%04x",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_wt);
        LOG_I("psap_cpd_ws:0x%04x",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_ws);

        LOG_I("psap_cpd_et:0x%04x",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_et);
        LOG_I("psap_cpd_es:0x%04x",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_es);


        LOG_I("psap_cpd_coefa_at:0x%04x",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_coefa_at);
        LOG_I("psap_cpd_coefb_at:0x%04x",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_coefb_at);

        LOG_I("psap_cpd_tava:0x%04x",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_tava);
        LOG_I("psap_cpd_tavb:0x%04x",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_tavb);

        LOG_I("psap_cpd_coefa_rt:0x%04x",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_coefa_rt);
        LOG_I("psap_cpd_coefb_rt:0x%04x",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_coefb_rt);

        LOG_I("psap_cpd_delay:%d",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_delay);

        LOG_I("psap_cpd_gl:%d",psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_gl);
    }

    LOG_I("psap_adc_gain_offset:%d",psap_cfg_coef_l->psap_adc_gain_offset);

    LOG_I("psap_limiter_coefa_at:%d",psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_coefa_at);
    LOG_I("psap_limiter_coefb_at:%d",psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_coefb_at);
    LOG_I("psap_limiter_th:%d",psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_th);
    LOG_I("psap_limiter_coefa_rt:%d",psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_coefa_rt);
    LOG_I("psap_limiter_coefb_rt:%d",psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_coefb_rt);
    LOG_I("psap_limiter_delay:%d",psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_delay);
#endif

    //init the gain.
    psap_band_num=PSAP_BAND_NUM;

    for(i=0;i<PSAP_BAND_NUM;i++)
    {
        psap_coef_gain_l[i]=0;
    }
    psap_updata_gain();

    psap_band_num = BandNum;

    codec->REG_400 &= ~CODEC_PSAP_ENABLE;
    codec->REG_400 &= ~CODEC_PSAP_ENABLE_CH0;
    codec->REG_400 &= ~CODEC_PSAP_ENABLE_CH1;

    codec->REG_474 &= ~CODEC_PSAP_CPD_ENABLE;
    codec->REG_474 &= ~CODEC_PSAP_IIR_ENABLE;
    codec->REG_474 &= ~CODEC_PSAP_LMT_ENABLE;

    if(BandNum<=4)
    {
        hal_codec_psap_enable(24000000);
    }
    else
    {
        hal_codec_psap_enable(48000000);
    }

#if defined(PSAP_APP)
    analog_aud_apply_anc_adc_gain_offset(ANC_TALKTHRU, psap_cfg_coef_l->psap_adc_gain_offset, 0);
#endif

    for(i=0;i<PSAP_BAND_NUM;i++)
    {
        psap_coef_gain_l[i]=(int)(psap_cfg_coef_l->psap_band_gain[i]*(psap_cfg_coef_l->psap_total_gain/512.0f));
    }

    psap_updata_gain();

    //cfg stage 0 filter coef.
    DestIndex=0;

    codec->REG_474 &= ~CODEC_PSAP_IIR_EXT_BYPASS_1;
    codec->REG_474 &= ~CODEC_PSAP_IIR_EXT_BYPASS_2;
    codec->REG_474 &= ~CODEC_PSAP_IIR_EXT_BYPASS_3;
    codec->REG_474 &= ~CODEC_PSAP_IIR_EXT_BYPASS_4;

    if (BandNum == 17 || BandNum == 9 || BandNum == 5 || BandNum == 3 || BandNum == 2) {
        int SrcIndex0 = (BandNum-1)-1;
        psap_updata_coef(DestIndex,SrcIndex0);
        DestIndex++;
    }

    //cfg stage 1 filter coef.
    if (BandNum == 17 || BandNum == 9 || BandNum == 5 || BandNum == 3) {
        int SrcIndex0 = (BandNum - 1)/2 - 1;
        psap_updata_coef(DestIndex,SrcIndex0);
        DestIndex++;
        codec->REG_474 &= ~CODEC_PSAP_IIR_EXT_BYPASS_1;
    } else {
        codec->REG_474 |= CODEC_PSAP_IIR_EXT_BYPASS_1;
    }

    //cfg stage 2 filter coef.
    if (BandNum == 17 || BandNum == 9 || BandNum == 5) {
        int SrcIndex0 = (BandNum-1)/2 - (BandNum-1)/4-1;
        int SrcIndex1 = (BandNum-1)/2 + (BandNum-1)/4-1;

        psap_updata_coef(DestIndex,SrcIndex0);
        DestIndex++;
        psap_updata_coef(DestIndex,SrcIndex1);
        DestIndex++;
        codec->REG_474 &= ~CODEC_PSAP_IIR_EXT_BYPASS_2;
    } else {
        codec->REG_474 |= CODEC_PSAP_IIR_EXT_BYPASS_2;
    }

    //cfg stage 3 filter coef.
    if (BandNum == 17 || BandNum == 9) {
        int SrcIndex0 = (BandNum-1)/2 - (BandNum-1)/4 - (BandNum-1)/8-1;
        int SrcIndex1 = (BandNum-1)/2 - (BandNum-1)/4 + (BandNum-1)/8-1;
        int SrcIndex2 = (BandNum-1)/2 + (BandNum-1)/4 - (BandNum-1)/8-1;
        int SrcIndex3 = (BandNum-1)/2 + (BandNum-1)/4 + (BandNum-1)/8-1;

        psap_updata_coef(DestIndex,SrcIndex0);
        DestIndex++;
        psap_updata_coef(DestIndex,SrcIndex1);
        DestIndex++;
        psap_updata_coef(DestIndex,SrcIndex2);
        DestIndex++;
        psap_updata_coef(DestIndex,SrcIndex3);
        DestIndex++;

        codec->REG_474 &= ~CODEC_PSAP_IIR_EXT_BYPASS_3;
    } else {
        codec->REG_474 |= CODEC_PSAP_IIR_EXT_BYPASS_3;
    }

    //cfg stage 4 filter coef.
    if (BandNum == 17) {
        int SrcIndex0 = (BandNum-1)/2 - (BandNum-1)/4 - (BandNum-1)/8 - (BandNum-1)/16-1;
        int SrcIndex1 = (BandNum-1)/2 - (BandNum-1)/4 - (BandNum-1)/8 + (BandNum-1)/16-1;
        int SrcIndex2 = (BandNum-1)/2 - (BandNum-1)/4 + (BandNum-1)/8 - (BandNum-1)/16-1;
        int SrcIndex3 = (BandNum-1)/2 - (BandNum-1)/4 + (BandNum-1)/8 + (BandNum-1)/16-1;

        int SrcIndex4 = (BandNum-1)/2 + (BandNum-1)/4 - (BandNum-1)/8 - (BandNum-1)/16-1;
        int SrcIndex5 = (BandNum-1)/2 + (BandNum-1)/4 - (BandNum-1)/8 + (BandNum-1)/16-1;
        int SrcIndex6 = (BandNum-1)/2 + (BandNum-1)/4 + (BandNum-1)/8 - (BandNum-1)/16-1;
        int SrcIndex7 = (BandNum-1)/2 + (BandNum-1)/4 + (BandNum-1)/8 + (BandNum-1)/16-1;

        psap_updata_coef(DestIndex,SrcIndex0);
        DestIndex++;
        psap_updata_coef(DestIndex,SrcIndex1);
        DestIndex++;
        psap_updata_coef(DestIndex,SrcIndex2);
        DestIndex++;
        psap_updata_coef(DestIndex,SrcIndex3);
        DestIndex++;
        psap_updata_coef(DestIndex,SrcIndex4);
        DestIndex++;
        psap_updata_coef(DestIndex,SrcIndex5);
        DestIndex++;
        psap_updata_coef(DestIndex,SrcIndex6);
        DestIndex++;
        psap_updata_coef(DestIndex,SrcIndex7);
        DestIndex++;
        codec->REG_474 &= ~CODEC_PSAP_IIR_EXT_BYPASS_4;
    } else {
        codec->REG_474 |= CODEC_PSAP_IIR_EXT_BYPASS_4;
    }
    //cfg cpd parameters.
    for (i = 0,j = 0; i < PSAP_BAND_NUM-1; i = i + 16/(psap_band_num-1), j++) {
        if (i == 0) {
            codec->REG_450 = SET_BITFIELD(codec->REG_450, CODEC_PSAP_CPD_CT_00, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_ct);
            codec->REG_450 = SET_BITFIELD(codec->REG_450, CODEC_PSAP_CPD_CS_00, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_cs);

            codec->REG_454 = SET_BITFIELD(codec->REG_454, CODEC_PSAP_CPD_WT_00, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_wt);
            codec->REG_454 = SET_BITFIELD(codec->REG_454, CODEC_PSAP_CPD_WS_00, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_ws);

            codec->REG_458 = SET_BITFIELD(codec->REG_458, CODEC_PSAP_CPD_ET_00, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_et);
            codec->REG_458 = SET_BITFIELD(codec->REG_458, CODEC_PSAP_CPD_ES_00, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_es);

            codec->REG_45C = SET_BITFIELD(codec->REG_45C, CODEC_PSAP_CPD_COEFA_AT_00, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_coefa_at);
            codec->REG_45C = SET_BITFIELD(codec->REG_45C, CODEC_PSAP_CPD_COEFB_AT_00, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_coefb_at);

            codec->REG_468 = SET_BITFIELD(codec->REG_468, CODEC_PSAP_CPD_TAVA_00, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_tava);
            codec->REG_468 = SET_BITFIELD(codec->REG_468, CODEC_PSAP_CPD_TAVB_00, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_tavb);

            codec->REG_46C = SET_BITFIELD(codec->REG_46C, CODEC_PSAP_CPD_COEFA_RT_00, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_coefa_rt);
            codec->REG_46C = SET_BITFIELD(codec->REG_46C, CODEC_PSAP_CPD_COEFB_RT_00, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_coefb_rt);

            codec->REG_3F0 = SET_BITFIELD(codec->REG_3F0, CODEC_PSAP_CPD_GL_00, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_gl);
        } else {
            *(&codec->REG_4A0 + (i - 1) * 6) = SET_BITFIELD(*(&codec->REG_4A0 + (i - 1) * 6), CODEC_PSAP_CPD_CT_01, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_ct);
            *(&codec->REG_4A0 + (i - 1) * 6) = SET_BITFIELD(*(&codec->REG_4A0 + (i - 1) * 6), CODEC_PSAP_CPD_CS_01, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_cs);

            *(&codec->REG_4A4 + (i - 1) * 6) = SET_BITFIELD(*(&codec->REG_4A4 + (i - 1) * 6), CODEC_PSAP_CPD_WT_01, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_wt);
            *(&codec->REG_4A4 + (i - 1) * 6) = SET_BITFIELD(*(&codec->REG_4A4 + (i - 1) * 6), CODEC_PSAP_CPD_WS_01, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_ws);

            *(&codec->REG_4A8 + (i - 1) * 6) = SET_BITFIELD(*(&codec->REG_4A8 + (i - 1) * 6), CODEC_PSAP_CPD_ET_01, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_et);
            *(&codec->REG_4A8 + (i - 1) * 6) = SET_BITFIELD(*(&codec->REG_4A8 + (i - 1) * 6), CODEC_PSAP_CPD_ES_01, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_es);

            *(&codec->REG_4AC + (i - 1) * 6) = SET_BITFIELD(*(&codec->REG_4AC + (i - 1) * 6), CODEC_PSAP_CPD_TAVA_01, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_tava);
            *(&codec->REG_4AC + (i - 1) * 6) = SET_BITFIELD(*(&codec->REG_4AC + (i - 1) * 6), CODEC_PSAP_CPD_TAVB_01, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_tavb);

            *(&codec->REG_4B0 + (i - 1) * 6) = SET_BITFIELD(*(&codec->REG_4B0 + (i - 1) * 6), CODEC_PSAP_CPD_COEFA_AT_01, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_coefa_at);
            *(&codec->REG_4B0 + (i - 1) * 6) = SET_BITFIELD(*(&codec->REG_4B0 + (i - 1) * 6), CODEC_PSAP_CPD_COEFB_AT_01, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_coefb_at);

            *(&codec->REG_4B4 + (i - 1) * 6) = SET_BITFIELD(*(&codec->REG_4B4 + (i - 1) * 6), CODEC_PSAP_CPD_COEFA_RT_01, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_coefa_rt);
            *(&codec->REG_4B4 + (i - 1) * 6) = SET_BITFIELD(*(&codec->REG_4B4 + (i - 1) * 6), CODEC_PSAP_CPD_COEFB_RT_01, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_coefb_rt);

            switch(i)
            {
                case 1:
                codec->REG_3F0 = SET_BITFIELD(codec->REG_3F0, CODEC_PSAP_CPD_GL_01, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_gl);
                break;

                case 2:
                codec->REG_3F4 = SET_BITFIELD(codec->REG_3F4, CODEC_PSAP_CPD_GL_02, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_gl);
                break;

                case 3:
                codec->REG_3F4 = SET_BITFIELD(codec->REG_3F4, CODEC_PSAP_CPD_GL_03, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_gl);
                break;

                case 4:
                codec->REG_3F8 = SET_BITFIELD(codec->REG_3F8, CODEC_PSAP_CPD_GL_04, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_gl);
                break;

                case 5:
                codec->REG_3F8 = SET_BITFIELD(codec->REG_3F8, CODEC_PSAP_CPD_GL_05, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_gl);
                break;

                case 6:
                codec->REG_3FC = SET_BITFIELD(codec->REG_3FC, CODEC_PSAP_CPD_GL_06, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_gl);
                break;

                case 7:
                codec->REG_3FC = SET_BITFIELD(codec->REG_3FC, CODEC_PSAP_CPD_GL_07, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_gl);
                break;

                case 8:
                codec->REG_7E0 = SET_BITFIELD(codec->REG_7E0, CODEC_PSAP_CPD_GL_08, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_gl);
                break;

                case 9:
                codec->REG_7E0 = SET_BITFIELD(codec->REG_7E0, CODEC_PSAP_CPD_GL_09, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_gl);
                break;

                case 10:
                codec->REG_7E4 = SET_BITFIELD(codec->REG_7E4, CODEC_PSAP_CPD_GL_10, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_gl);
                break;

                case 11:
                codec->REG_7E4 = SET_BITFIELD(codec->REG_7E4, CODEC_PSAP_CPD_GL_11, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_gl);
                break;

                case 12:
                codec->REG_7E8 = SET_BITFIELD(codec->REG_7E8, CODEC_PSAP_CPD_GL_12, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_gl);
                break;

                case 13:
                codec->REG_7E8 = SET_BITFIELD(codec->REG_7E8, CODEC_PSAP_CPD_GL_13, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_gl);
                break;

                case 14:
                codec->REG_7EC = SET_BITFIELD(codec->REG_7EC, CODEC_PSAP_CPD_GL_14, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_gl);
                break;

                case 15:
                codec->REG_7EC = SET_BITFIELD(codec->REG_7EC, CODEC_PSAP_CPD_GL_15, psap_cfg_coef_l->psap_cpd_cfg[j].psap_cpd_gl);
                break;

                default:
                LOG_I("psap Error band:%d",i);
                break;
            }

        }
    }



    if(psap_cfg_coef_l->psap_cpd_cfg[0].psap_cpd_delay>=0&&psap_cfg_coef_l->psap_cpd_cfg[0].psap_cpd_delay<128) {
        codec->REG_474 = SET_BITFIELD(codec->REG_474, CODEC_PSAP_CPD_DELAY, psap_cfg_coef_l->psap_cpd_cfg[0].psap_cpd_delay);
    } else {
        codec->REG_474 = SET_BITFIELD(codec->REG_474, CODEC_PSAP_CPD_DELAY, 96);
    }

    //cfg limiter parameters
    codec->REG_460 = SET_BITFIELD(codec->REG_460, CODEC_PSAP_LMT_COEFA_AT, psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_coefa_at);
    codec->REG_460 = SET_BITFIELD(codec->REG_460, CODEC_PSAP_LMT_COEFB_AT, psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_coefb_at);

    codec->REG_464 &= ~CODEC_PSAP_LMT_TH_UPDATE;
    codec->REG_464 = SET_BITFIELD(codec->REG_464,CODEC_PSAP_LMT_TH,psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_th);
    codec->REG_464 |= CODEC_PSAP_LMT_TH_UPDATE;

    codec->REG_470 = SET_BITFIELD(codec->REG_470, CODEC_PSAP_LMT_COEFA_RT, psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_coefa_rt);
    codec->REG_470 = SET_BITFIELD(codec->REG_470, CODEC_PSAP_LMT_COEFB_RT, psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_coefb_rt);

    if (psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_delay >= 0 && psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_delay < 128){
        codec->REG_474 = SET_BITFIELD(codec->REG_474, CODEC_PSAP_LMT_DELAY, psap_cfg_coef_l->psap_limiter_cfg.psap_limiter_delay);
    } else {
        codec->REG_474 = SET_BITFIELD(codec->REG_474, CODEC_PSAP_LMT_DELAY, 16);
    }

#if defined(AUDIO_PSAP_DEHOWLING_HW)
    anc_set_dehowling_cfg(&(psap_cfg_coef_l->psap_dehowling_cfg));
#endif
#if defined(AUDIO_PSAP_EQ_HW)
    anc_set_psap_eq_cfg((const struct_psap_eq_cfg *)&(psap_cfg_coef_l->psap_dehowling_cfg.dehowling_l));
#endif

    if(psap_cfg_coef_l->psap_type == PSAP_MODE_ONLY_MIC
        ||psap_cfg_coef_l->psap_type == PSAP_MODE_MIC_MUSIC
        ||psap_cfg_coef_l->psap_type == PSAP_MODE_ONLY_MUSIC) {
        psap_set_mode(AUD_CHANNEL_MAP_CH0, psap_cfg_coef_l->psap_type);
    } else {
        LOG_I("%s Error psap tyep:%d", __func__,psap_cfg_coef_l->psap_type);

        psap_set_mode(AUD_CHANNEL_MAP_CH0, PSAP_MODE_ONLY_MIC);
    }

    codec->REG_474 |= CODEC_PSAP_CPD_ENABLE;
    codec->REG_474 |= CODEC_PSAP_IIR_ENABLE;
    codec->REG_474 |= CODEC_PSAP_LMT_ENABLE;

    codec->REG_400 |= CODEC_PSAP_ENABLE;
    if (psap_output_ch_map & AUD_CHANNEL_MAP_CH0) {
        codec->REG_400 |= CODEC_PSAP_ENABLE_CH0;
    }
    if (psap_output_ch_map & AUD_CHANNEL_MAP_CH1) {
      //  codec->REG_400 |= CODEC_PSAP_ENABLE_CH1; //chip can't work in 1600.
    }

    return err;
}

static void psap_updata_coef(unsigned int dest,unsigned int src)
{
#ifdef PSAP_DEBUG_TRACE
    LOG_I("%s dst_coef_num:%02d,src_coef_num:%02d", __func__,dest,src);
#endif

    if (dest>=PSAP_IIR_NUM||src>=PSAP_IIR_NUM) {
            LOG_I("%s:Error band mum!dest:%d,source:%d", __func__,dest,src);
            return;
    }

    psap_iir_coef[dest].iir0.a1 = -psap_cfg_coef_l->psap_iir_coef[src].iir0.coef_a[1];
    psap_iir_coef[dest].iir0.a2 = -psap_cfg_coef_l->psap_iir_coef[src].iir0.coef_a[2];
    psap_iir_coef[dest].iir0.a3 = -psap_cfg_coef_l->psap_iir_coef[src].iir0.coef_a[3];
    psap_iir_coef[dest].iir0.b1 = psap_cfg_coef_l->psap_iir_coef[src].iir0.coef_b[1];
    psap_iir_coef[dest].iir0.b2 = psap_cfg_coef_l->psap_iir_coef[src].iir0.coef_b[2];
    psap_iir_coef[dest].iir0.b3 = psap_cfg_coef_l->psap_iir_coef[src].iir0.coef_b[3];
    psap_iir_coef[dest].iir0.b0 = psap_cfg_coef_l->psap_iir_coef[src].iir0.coef_b[0];

    psap_iir_coef[dest].iir1.a1 = -psap_cfg_coef_l->psap_iir_coef[src].iir1.coef_a[1];
    psap_iir_coef[dest].iir1.a2 = -psap_cfg_coef_l->psap_iir_coef[src].iir1.coef_a[2];
    psap_iir_coef[dest].iir1.a3 = -psap_cfg_coef_l->psap_iir_coef[src].iir1.coef_a[3];
    psap_iir_coef[dest].iir1.b1 = psap_cfg_coef_l->psap_iir_coef[src].iir1.coef_b[1];
    psap_iir_coef[dest].iir1.b2 = psap_cfg_coef_l->psap_iir_coef[src].iir1.coef_b[2];
    psap_iir_coef[dest].iir1.b3 = psap_cfg_coef_l->psap_iir_coef[src].iir1.coef_b[3];
    psap_iir_coef[dest].iir1.b0 = psap_cfg_coef_l->psap_iir_coef[src].iir1.coef_b[0];

    return;
}

static void psap_updata_gain(void)
{
    int i,j;
    int32_t psap_gain_ext;
#ifdef PSAP_DEBUG_TRACE
    LOG_I("%s", __func__);
#endif
    if(psap_band_num != 17 && psap_band_num != 9 && psap_band_num != 5 && psap_band_num != 3 && psap_band_num != 2) {
        LOG_I("%s: Error band number!:%d", __func__,psap_band_num);
        return;
    }

    //setting the gain.
    codec->REG_404 = SET_BITFIELD(codec->REG_404, CODEC_PSAP_GAIN_EXT_UPDATE, 0);

    for (i = 0, j = 0; i < PSAP_BAND_NUM - 1; i = i + 16/(psap_band_num-1), j++) {
        psap_gain_ext = (int32_t)(psap_coef_gain_l[j] * psap_bands_same_gain_l[j] * psap_bands_gain_l[j] * psap_ramp_total_gain_l);
        *(&codec->REG_40C + i) = SET_BITFIELD(*(&codec->REG_40C + i), CODEC_PSAP_GAIN_EXT_0, psap_gain_ext);

#ifdef PSAP_DEBUG_TRACE
        LOG_I("band_num:%02d, chip_num:%02d, coef_gain_l:0x%08x, bands_gain_l:%d, total_gain_l:%d", j, i,
            psap_coef_gain_l[j],
            (int32_t)(psap_bands_same_gain_l[j] * 1000),
            (int32_t)(psap_ramp_total_gain_l * 1000));
#endif
    }
    psap_gain_ext = (int32_t)(psap_coef_gain_l[j] * psap_bands_same_gain_l[j] * psap_bands_gain_l[j] * psap_ramp_total_gain_l);
    *(&codec->REG_40C + i) = SET_BITFIELD(*(&codec->REG_40C + i), CODEC_PSAP_GAIN_EXT_0, psap_gain_ext);

#ifdef PSAP_DEBUG_TRACE
    LOG_I("band_num:%02d, chip_num:%02d, coef_gain_l:0x%08x, bands_gain_l:%d, total_gain_l:%d", j, i,
        psap_coef_gain_l[j],
        (int32_t)(psap_bands_same_gain_l[j] * 1000),
        (int32_t)(psap_ramp_total_gain_l * 1000));
#endif

    codec->REG_404 = SET_BITFIELD(codec->REG_404, CODEC_PSAP_GAIN_EXT_UPDATE, 0x1ffff);
}

static void psap_ctrl_reg_init(void)
{
    int i;
#ifdef PSAP_DEBUG_TRACE
    LOG_I("%s", __func__);
#endif

    codec->REG_400 &= ~CODEC_PSAP_ENABLE;

    psap_band_num=PSAP_BAND_NUM;

    psap_ramp_total_gain_l = 0.0f;
    for(i=0;i<PSAP_BAND_NUM;i++)
    {
        psap_coef_gain_l[i] = 0;

        // If don't want to reset gain, can comment following gain.
        psap_bands_same_gain_l[i] = 1.0f;
        psap_bands_gain_l[i] = 1.0f;
    }

    psap_updata_gain();

    codec->REG_408 = SET_BITFIELD(codec->REG_408, CODEC_PSAP_SBD_CNT, 15);

    codec->REG_450 = SET_BITFIELD(codec->REG_450, CODEC_PSAP_CPD_CT_00, 24703);
    codec->REG_450 = SET_BITFIELD(codec->REG_450, CODEC_PSAP_CPD_CS_00, 0x6666);

    codec->REG_454 = SET_BITFIELD(codec->REG_454, CODEC_PSAP_CPD_WT_00, 21153);
    codec->REG_454 = SET_BITFIELD(codec->REG_454, CODEC_PSAP_CPD_WS_00, 0x4000);

    codec->REG_458 = SET_BITFIELD(codec->REG_458, CODEC_PSAP_CPD_ET_00, 12871);
    codec->REG_458 = SET_BITFIELD(codec->REG_458, CODEC_PSAP_CPD_ES_00, 0x2000);

    codec->REG_45C = SET_BITFIELD(codec->REG_45C, CODEC_PSAP_CPD_COEFA_AT_00, 32428);
    codec->REG_45C = SET_BITFIELD(codec->REG_45C, CODEC_PSAP_CPD_COEFB_AT_00, 32768-32428);

    codec->REG_460 = SET_BITFIELD(codec->REG_460, CODEC_PSAP_LMT_COEFA_AT, 28563);
    codec->REG_460 = SET_BITFIELD(codec->REG_460, CODEC_PSAP_LMT_COEFB_AT, 32768-28563);

    codec->REG_468 = SET_BITFIELD(codec->REG_468, CODEC_PSAP_CPD_TAVA_00, 0x7333);
    codec->REG_468 = SET_BITFIELD(codec->REG_468, CODEC_PSAP_CPD_TAVB_00, 0xccc);

    codec->REG_46C = SET_BITFIELD(codec->REG_46C, CODEC_PSAP_CPD_COEFA_RT_00, 32734);
    codec->REG_46C = SET_BITFIELD(codec->REG_46C, CODEC_PSAP_CPD_COEFB_RT_00, 32768-32734);

    codec->REG_464 &= ~CODEC_PSAP_LMT_TH_UPDATE;
    codec->REG_464 = SET_BITFIELD(codec->REG_464, CODEC_PSAP_LMT_TH, 0x7ffff);
    codec->REG_464 |= CODEC_PSAP_LMT_TH_UPDATE;

    codec->REG_470 = SET_BITFIELD(codec->REG_470, CODEC_PSAP_LMT_COEFA_RT, 32765);
    codec->REG_470 = SET_BITFIELD(codec->REG_470, CODEC_PSAP_LMT_COEFB_RT, 32768-32765);

    for(i=1;i<PSAP_BAND_NUM-1;i++)
    {
        *(&codec->REG_4A0 + i - 1) = SET_BITFIELD(*(&codec->REG_4A0 + i - 1), CODEC_PSAP_CPD_CT_01, 24703);
        *(&codec->REG_4A0 + i - 1) = SET_BITFIELD(*(&codec->REG_4A0 + i - 1), CODEC_PSAP_CPD_CS_01, 0x6666);

        *(&codec->REG_4A4 + i - 1) = SET_BITFIELD(*(&codec->REG_4A4 + i - 1), CODEC_PSAP_CPD_WT_01, 21153);
        *(&codec->REG_4A4 + i - 1) = SET_BITFIELD(*(&codec->REG_4A4 + i - 1), CODEC_PSAP_CPD_WS_01, 0x4000);

        *(&codec->REG_4A8 + i - 1) = SET_BITFIELD(*(&codec->REG_4A8 + i - 1), CODEC_PSAP_CPD_ET_01, 12871);
        *(&codec->REG_4A8 + i - 1) = SET_BITFIELD(*(&codec->REG_4A8 + i - 1), CODEC_PSAP_CPD_ES_01, 0x2000);

        *(&codec->REG_4AC + i - 1) = SET_BITFIELD(*(&codec->REG_4AC + i - 1), CODEC_PSAP_CPD_TAVA_01, 0x7333);
        *(&codec->REG_4AC + i - 1) = SET_BITFIELD(*(&codec->REG_4AC + i - 1), CODEC_PSAP_CPD_TAVB_01, 0xccc);

        *(&codec->REG_4B0 + i - 1) = SET_BITFIELD(*(&codec->REG_4B0 + i - 1), CODEC_PSAP_CPD_COEFA_AT_01, 32428);
        *(&codec->REG_4B0 + i - 1) = SET_BITFIELD(*(&codec->REG_4B0 + i - 1), CODEC_PSAP_CPD_COEFB_AT_01, 32768-32428);

        *(&codec->REG_4B4 + i - 1) = SET_BITFIELD(*(&codec->REG_4B4 + i - 1), CODEC_PSAP_CPD_COEFA_RT_01, 32734);
        *(&codec->REG_4B4 + i - 1) = SET_BITFIELD(*(&codec->REG_4B4 + i - 1), CODEC_PSAP_CPD_COEFB_RT_01, 32768-32734);
    }

    codec->REG_3F0 = SET_BITFIELD(codec->REG_3F0, CODEC_PSAP_CPD_GL_01, 27069);
    codec->REG_3F4 = SET_BITFIELD(codec->REG_3F4, CODEC_PSAP_CPD_GL_02, 27069);
    codec->REG_3F4 = SET_BITFIELD(codec->REG_3F4, CODEC_PSAP_CPD_GL_03, 27069);
    codec->REG_3F8 = SET_BITFIELD(codec->REG_3F8, CODEC_PSAP_CPD_GL_04, 27069);
    codec->REG_3F8 = SET_BITFIELD(codec->REG_3F8, CODEC_PSAP_CPD_GL_05, 27069);
    codec->REG_3FC = SET_BITFIELD(codec->REG_3FC, CODEC_PSAP_CPD_GL_06, 27069);
    codec->REG_3FC = SET_BITFIELD(codec->REG_3FC, CODEC_PSAP_CPD_GL_07, 27069);
    codec->REG_7E0 = SET_BITFIELD(codec->REG_7E0, CODEC_PSAP_CPD_GL_08, 27069);
    codec->REG_7E0 = SET_BITFIELD(codec->REG_7E0, CODEC_PSAP_CPD_GL_09, 27069);
    codec->REG_7E4 = SET_BITFIELD(codec->REG_7E4, CODEC_PSAP_CPD_GL_10, 27069);
    codec->REG_7E4 = SET_BITFIELD(codec->REG_7E4, CODEC_PSAP_CPD_GL_11, 27069);
    codec->REG_7E8 = SET_BITFIELD(codec->REG_7E8, CODEC_PSAP_CPD_GL_12, 27069);
    codec->REG_7E8 = SET_BITFIELD(codec->REG_7E8, CODEC_PSAP_CPD_GL_13, 27069);
    codec->REG_7EC = SET_BITFIELD(codec->REG_7EC, CODEC_PSAP_CPD_GL_14, 27069);
    codec->REG_7EC = SET_BITFIELD(codec->REG_7EC, CODEC_PSAP_CPD_GL_15, 27069);

    codec->REG_474 &= ~CODEC_PSAP_IIR_EXT_BYPASS;
    codec->REG_474 &= ~CODEC_PSAP_IIR_EXT_BYPASS_1;
    codec->REG_474 &= ~CODEC_PSAP_IIR_EXT_BYPASS_2;
    codec->REG_474 &= ~CODEC_PSAP_IIR_EXT_BYPASS_3;
    codec->REG_474 &= ~CODEC_PSAP_IIR_EXT_BYPASS_4;

    codec->REG_474 &= ~CODEC_PSAP_IIR_GAINCAL_EXT_BYPASS;
    codec->REG_474 &= ~CODEC_PSAP_IIR_GAINUSE_EXT_BYPASS;

    codec->REG_474 = SET_BITFIELD(codec->REG_474, CODEC_PSAP_IIR_OUTPUT_BIT, 0);
    codec->REG_474 = SET_BITFIELD(codec->REG_474, CODEC_PSAP_CPD_DELAY, 96);

    codec->REG_474 &= ~CODEC_PSAP_CPD_ENABLE;
    codec->REG_474 = SET_BITFIELD(codec->REG_474, CODEC_PSAP_LMT_DELAY, 16);
    codec->REG_474 &= ~CODEC_PSAP_LMT_ENABLE;

    for(i=0;i<PSAP_IIR_NUM;i++)
    {
        psap_iir_coef[i].iir0.a1=0;
        psap_iir_coef[i].iir0.a2=0;
        psap_iir_coef[i].iir0.a3=0;
        psap_iir_coef[i].iir0.b1=0;
        psap_iir_coef[i].iir0.b2=0;
        psap_iir_coef[i].iir0.b3=0;
        psap_iir_coef[i].iir0.b0=0;

        psap_iir_coef[i].iir1.a1=0;
        psap_iir_coef[i].iir1.a2=0;
        psap_iir_coef[i].iir1.a3=0;
        psap_iir_coef[i].iir1.b1=0;
        psap_iir_coef[i].iir1.b2=0;
        psap_iir_coef[i].iir1.b3=0;
        psap_iir_coef[i].iir1.b0=0;
    }

    for(i=0;i<PSAP_BAND_NUM;i++)
    {
        gain_iir_coefs[i].a1=-iir_coef_gain_ramp_psap_normal.coef_a[1];
        gain_iir_coefs[i].a2=-iir_coef_gain_ramp_psap_normal.coef_a[2];
        gain_iir_coefs[i].b1=iir_coef_gain_ramp_psap_normal.coef_b[1];
        gain_iir_coefs[i].b2=iir_coef_gain_ramp_psap_normal.coef_b[2];
        gain_iir_coefs[i].b0=iir_coef_gain_ramp_psap_normal.coef_b[0];
    }

    codec->REG_474 &= ~CODEC_PSAP_IIR_ENABLE;
    codec->REG_400 &= ~CODEC_PSAP_ENABLE;

    return;
}

int psap_opened(void)
{
    return psap_open_flag;
}

void psap_set_total_gain_f32(float gain_l, float gain_r)
{
    LOG_I( "[%s] gain_l(x1000): %d", __func__, (int32_t)(gain_l*1000));

    psap_ramp_total_gain_l = gain_l;

    psap_updata_gain();

    return;
}

void psap_set_bands_same_gain_f32(float gain_l, float gain_r, uint32_t index_start, uint32_t index_end)
{
    ASSERT(index_start <= index_end, "[%s] index_start(%d) > index_end(%d)", __func__, index_start, index_end);
    ASSERT(index_end < PSAP_BAND_NUM, "[%s] index_end(%d) >= PSAP_BAND_NUM(%d)", __func__, index_end, PSAP_BAND_NUM);

    LOG_I( "[%s] gain_l(x1000): %d, index: %d ~ %d", __func__, (int32_t)(gain_l * 1000), index_start, index_end);

    for(int i=0; i<index_start; i++) {
        psap_bands_same_gain_l[i] = 1.0f;
    }

    for(int i=index_start; i<=index_end; i++) {
        psap_bands_same_gain_l[i] = gain_l;
    }

    for(int i=index_end+1; i<PSAP_BAND_NUM; i++) {
        psap_bands_same_gain_l[i] = 1.0f;
    }

    psap_updata_gain();

    return;
}

void psap_set_bands_gain_f32(float *gain_l, float *gain_r)
{
    if (gain_l != NULL) {
        LOG_I( "PSAP: Set left bands gain");
        for(int i=0; i<PSAP_BAND_NUM; i++) {
            psap_bands_gain_l[i] = *(gain_l + i);
        }
    } else {
        LOG_I( "PSAP: Reset left bands gain");
        for(int i=0; i<PSAP_BAND_NUM; i++) {
            psap_bands_gain_l[i] = 1.0f;
        }
    }

    if (gain_r != NULL) {
        LOG_I( "[%s] WARNING: Don't support right channel!", __func__);
    }

    psap_updata_gain();
}

void psap_enable(int ch_map)
{
    psap_set_total_gain_f32(1.0, 1.0);

    return;
}

void psap_disable(int ch_map)
{
    int i;

    LOG_I("%s", __func__);

    //setting the gain to mute.
    codec->REG_404 = SET_BITFIELD(codec->REG_404, CODEC_PSAP_GAIN_EXT_UPDATE, 0);

    for (i = 0; i < PSAP_BAND_NUM; i++) {
        *(&codec->REG_40C + i) = SET_BITFIELD(*(&codec->REG_40C + i), CODEC_PSAP_GAIN_EXT_0, 0);
    }

    codec->REG_404 = SET_BITFIELD(codec->REG_404, CODEC_PSAP_GAIN_EXT_UPDATE, 0x1ffff);

    return;
}
int psap_open(int ch_map)
{
    PSAP_ERROR err=PSAP_NO_ERR;

    LOG_I("%s", __func__);

    if(psap_open_flag==1)
    {
        LOG_I("%s psap have been opened! ", __func__);
        return err;
    }
    if (ch_map & AUD_CHANNEL_MAP_CH0) {
        psap_output_ch_map |= AUD_CHANNEL_MAP_CH0;
    } else {
        psap_output_ch_map &= ~AUD_CHANNEL_MAP_CH0;
    }

    if (ch_map & AUD_CHANNEL_MAP_CH1) {
        psap_output_ch_map |= AUD_CHANNEL_MAP_CH1;
    } else {
        psap_output_ch_map &= ~AUD_CHANNEL_MAP_CH1;
    }

    hal_codec_psap_enable(48000000);

    psap_ctrl_reg_init();

#if defined(AUDIO_PSAP_DEHOWLING_HW)
    anc_open(ANC_DEHOWLING);
#endif
#if defined(AUDIO_PSAP_EQ_HW)
    anc_open(PSAP_EQ);
#endif
    psap_open_flag=1;

    return err;
}

void psap_close(void)
{
    LOG_I("%s", __func__);

    psap_set_mode(AUD_CHANNEL_MAP_CH0,PSAP_MODE_ONLY_MIC);

    hal_codec_psap_disable();
#if defined(AUDIO_PSAP_DEHOWLING_HW)
    anc_close(ANC_DEHOWLING);
#endif
#if defined(AUDIO_PSAP_EQ_HW)
    anc_close(PSAP_EQ);
#endif
    psap_open_flag=0;

    return;
}

int psap_get_gain(int32_t *gain_ch_l, int32_t *gain_ch_r, uint32_t band_index)
{
    PSAP_ERROR err=PSAP_NO_ERR;

    LOG_I("%s", __func__);

    if(psap_open_flag==0)
    {
        LOG_I("%s psap have been not opened! ", __func__);
        *gain_ch_l=0;
        *gain_ch_r=0;
        return PSAP_OTHER_ERR;
    }

    if(band_index>psap_band_num)
    {
        LOG_I("%s Error band_index!:%d ", __func__,band_index);
        *gain_ch_l=0;
        *gain_ch_r=0;
        return PSAP_OTHER_ERR;
    }

    *gain_ch_l=psap_coef_gain_l[band_index];
    *gain_ch_r=psap_coef_gain_l[band_index];

    LOG_I("gain_ch_l:%d,gain_ch_r:%d,band_index:%d",*gain_ch_l,*gain_ch_r,band_index);

    return err;
}
int psap_set_gain(int32_t gain_ch_l, int32_t gain_ch_r, uint32_t band_index)
{
    PSAP_ERROR err=PSAP_NO_ERR;

    LOG_I("%s", __func__);

    LOG_I("gain_ch_l:0x%08x,gain_ch_r:0x%08x,band_index:%d", gain_ch_l,gain_ch_r,band_index);

    if(psap_open_flag==0)
    {
        LOG_I("%s psap have been not opened! ", __func__);
        return PSAP_OTHER_ERR;
    }

    if(band_index>psap_band_num)
    {
        LOG_I("%s Error band_index!:%d ", __func__,band_index);
        return PSAP_OTHER_ERR;
    }

    psap_coef_gain_l[band_index]=gain_ch_l;

    psap_updata_gain();

    return err;
}

int psap_set_mode(int ch_map, PSAP_MODE psap_mode)
{
    PSAP_ERROR err=PSAP_NO_ERR;
    struct HAL_CODEC_PSAP_CFG_T psap_cfg;
    LOG_I("%s", __func__);

    if(psap_open_flag==0)
    {
        LOG_I("%s psap have been not opened! ", __func__);
        return PSAP_OTHER_ERR;
    }
    psap_cfg.din0_samp_delay=0;
    psap_cfg.din0_gain=1.0f;
    psap_cfg.din1_sign=0;

    switch(psap_mode)
    {
        case PSAP_MODE_ONLY_MIC:
        psap_cfg.mode=HAL_CODEC_PSAP_MODE_ADC;
        break;
        case PSAP_MODE_MIC_MUSIC:
        psap_cfg.mode=HAL_CODEC_PSAP_MODE_ADC_DAC;
        break;
        case PSAP_MODE_ONLY_MUSIC:
        psap_cfg.mode=HAL_CODEC_PSAP_MODE_DAC;
        break;
        default:
        psap_cfg.mode=HAL_CODEC_PSAP_MODE_ADC;
        break;
    }

    hal_codec_psap_setup(AUD_CHANNEL_MAP_CH0,&psap_cfg);

    return err;
}




