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
** namer��FIR filter
** description��1~511 FIR filter
** version��V1.0
** author�� xuml
** modify��2015.7.29.
*******************************************************************************/

#include "mm_dbg.h"
#include <stdio.h>
#include <string.h>


#define ARM_BES1000


#ifdef ARM_BES1000
#include "plat_addr_map.h"
#include "cmsis.h"
#include "hal_dma.h"
#include "hal_sysfreq.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_cmu.h"
#include "hal_codec.h"
#include "analog.h"
#endif

#include "anc_process.h"

static void anc_ctrl_reg_init(void);

/**********************************mocro**************************************/

typedef short int16;
typedef unsigned short uint16;
typedef int   int32;
typedef unsigned int   uint32;



#define TESTLEN (4410)

#define TESTOUTLEN (TESTLEN)

#define MEMLEN 512
//#define COEFLEN (128)




//#define FRAMELEN (MEMLEN-COEFLEN)


#define FRAMELEN (MEMLEN-129)


#define COEFLEN1 (129)
#define COEFLEN2 (128)
#define COEFLEN3 (127)
#define COEFLEN4 (126)


/**********************************mocro**************************************/


// 31:27	Rev
// 26 : 18	result_base_addr	r / w	Fir�����coef memory��洢����ʼ��ַ
// 17 : 9	fir_sample_num	r / w	һ���жϼ����������.  (���ó�ʵ�ʵ��� - 1)
// 8 : 0	fir_sample_start	r / w	��sample memory��ĳ��sample��ʼ����fir



struct fir_config
{
	uint32 fir_sample_start : 9;
	uint32 fir_sample_num : 9;
	uint32 result_base_addr : 9;
	uint32 Rev : 5;
};


// 16:8	Sample cur addr	r	Sample address being calculated.
// 4	Fir_irq_mask	r / w	Irq mask, 0 for mask
// 3	Fir_irq_status	r / w	Read 1, fir finished, trigger interrupt;  write 1, clear interrupt
// 2	Fir_busy	R	Read 1, fir busy;
// 1	fir_start	w	Write 1��trigger fir start
// 0	fir_mode	r / w	0��used for ANC    1��used for configurable fir


struct fir_control
{
	uint32 fir_mode : 1;
	uint32 fir_start : 1;
	uint32 Fir_busy : 1;
	uint32 Fir_irq_status : 1;
	uint32 Fir_irq_mask : 1;
	uint32 Sample_cur_addr : 9;
	uint32 Rev : 18;
};

// λ	Name	Type	����
// 31:25	rev
// 24 : 16	fs_fir_order_Ch1	r / w	Fs fir order Ch1(���ó�ʵ�ʽ��� - 1)
// 15 : 9	rev
// 8 : 0	Fs_fir_order_Ch0	r / w	Fs fir order Ch0(���ó�ʵ�ʽ��� - 1)
// If 121 tap, need to config 120


struct _Fs_fir_order
{
	uint32 Fs_fir_order_Ch0 : 9;
	uint32 Rev0 : 7;
	uint32 Fs_fir_order_Ch1 : 9;
	uint32 Rev1 : 7;
};


// Pdu fir order : 0x12008
// λ	Name	Type	����
// 31 : 25	Rev
// 24 : 16	Pdu_fir_order_Ch1	r / w	Pdu fir order Ch1(���ó�ʵ�ʽ��� - 1)
// 15 : 9	Rev
// 8 : 0	Pdu_fir_order_Ch0	r / w	Pdu fir order Ch0(���ó�ʵ�ʽ��� - 1)
// If 121 tap, need to config 120

struct _Pdu_fir_order
{
	uint32 Pdu_fir_order_Ch0 : 9;
	uint32 Rev0 : 7;
	uint32 Pdu_fir_order_Ch1 : 9;
	uint32 Rev1 : 7;
};




//Gain select:  0x1200c
//λ	Name	Type	����
//31:23	Rev
//22:20	Pre_pdu_gain_ch1	r/w	0 for bypass, 1 for left shift 1, ��..
//18:16	Pre_pdu_gain_ch0	r/w	0 for bypass, 1 for left shift 1, ��..
//15:12	Fs gain select ch1	r/w
//11:8	Pdu gain select ch1	r/w
//7:4	Fs gain select ch0	r/w
///3:0	Pdu gain select ch0	r/w	6 for 0db



struct _Fir_Gain
{


	uint32 Pdu_gain_select_ch0 : 4;
	uint32 Fs_gain_select_ch0 : 4;
	uint32 Pdu_gain_select_ch1 : 4;
	uint32 Fs_gain_select_ch1 : 4;
	uint32 Pre_pdu_gain_ch0 : 3;
	uint32 Rev1 : 1;
	uint32 Pre_pdu_gain_ch1 : 3;
	uint32 Rev : 9;
};





// ANC enable : 0x12000
//
// λ	Name	Type	����
// 31 : 10	Rev	R
// 9	Apb_anc_sel_pdu_ch1	r / w	1 : anc access memory, 0 : apb access memory
// 8	Apb_anc_sel_pdu_ch0	r / w	1 : anc access memory, 0 : apb access memory
// 7	Apb_anc_sel_fs_ch1	r / w	1 : anc access memory, 0 : apb access memory
// 6	Sel_gpadc_ch1	r / w	Same as Sel_gpadc_ch0
// 5	Sel_gpadc_ch0	r / w	0 : sel sigma - delta adc for anc, 1 : sel gpadc for anc
// 4	Anc_mode_ch1	r / w	Anc mode ch1, 0 for feedforward, 1 for feedback
// 3	Anc_mode_ch0	r / w	Anc mode ch0, 0 for feedforward, 1 for feedback
// 2	Anc_enable_ch1	r / w	ʹ��ANC ch1  1 = enable��
// 1	Anc_enable_ch0	r / w	ʹ��ANC ch0  1 = enable��
// 0	Apb_anc_sel_fs_ch0	r / w	1 : anc access memory, 0 : apb access memory



struct _ANC_enble
{
	uint32 Apb_anc_sel_fs_ch0 : 1;
	uint32 Anc_enable_ch0 : 1;
	uint32 Anc_enable_ch1 : 1;
	uint32 Anc_mode_ch0 : 1;
	uint32 Anc_mode_ch1 : 1;
	uint32 Sel_gpadc_ch0 : 1;
	uint32 Sel_gpadc_ch1 : 1;
	uint32 Apb_anc_sel_fs_ch1 : 1;
	uint32 Apb_anc_sel_pdu_ch0 : 1;
	uint32 Apb_anc_sel_pdu_ch1 : 1;
	uint32 Gpadc_unsigned : 1;
	// BBG
#if 1
	uint32 apb_anc_sel_fs_sample_ch0 : 1;
	uint32 apb_anc_sel_fs_sample_ch1 : 1;
	uint32 apb_anc_sel_pdu_sample_ch0 : 1;
	uint32 apb_anc_sel_pdu_sample_ch1 : 1;
	uint32 Rev : 17;
#else
	uint32 Rev : 21;
#endif
};







//Mute Gain coef :  0x12010
//λ	Name	Type	����
//31:16	Rev
//15:8	Mute gain coef ch1	r/w
//7:0	Mute gain coef ch0	r/w	1.7, 0x7f  for  0db gain

struct _Mute_Gain_coef
{
	int32 Mute_gain_coef_ch0 : 8;
	int32 Mute_gain_coef_ch1 : 8;
	// BBG
#if 1
	uint32 Mute_gain_pass0_ch0 : 1;
	uint32 Mute_gain_pass0_ch1 : 1;
	uint32 cap_gpadc : 1;
	uint32 cap_gpadc_48k : 1;
	uint32 anc_sr_sel : 2;
	uint32 anc_phase_sel : 4;
	uint32 anc_coef_sel_ch0 : 1;
	uint32 anc_coef_sel_ch1 : 1;
	uint32 Cap_L:1;
	uint32 Cap_R:1;
	uint32 Dual_anc:1;
	uint32 Rev :1;
#else
	uint32 Rev : 16;
#endif
};

static int8_t open_flag;

static int8_t max_gain0;
static int8_t max_gain1;


/********************************* const **************************************/
//lowpass
const int16 FirCoef1[COEFLEN1] = {

	-30, -19, 24, 9, -5, -29, 4, 29, 15,
	-33, -30, 22, 48, -5, -57, -22, 58, 51,
	-42, -80, 12, 97, 31, -98, -80, 75, 126,
	-27, -156, -41, 159, 120, -125, -193, 53, 243,
	51, -252, -174, 206, 292, -100, -379, -60, 406,
	257, -348, -458, 191, 624, 68, -707, -415, 656,
	822, -419, -1249, -73, 1650, 961, -1977, -2753, 2192,
	10181, 14118, 10181, 2192, -2753, -1977, 961, 1650, -73,
	-1249, -419, 822, 656, -415, -707, 68, 624, 191,
	-458, -348, 257, 406, -60, -379, -100, 292, 206,
	-174, -252, 51, 243, 53, -193, -125, 120, 159,
	-41, -156, -27, 126, 75, -80, -98, 31, 97,
	12, -80, -42, 51, 58, -22, -57, -5, 48,
	22, -30, -33, 15, 29, 4, -29, -5, 9,
	24, -19, -30

};

const int16 FirCoef2[COEFLEN2] = {
	1009, 3974, 8501, 11754, 10306, 3862, -3237, -5517, -1932,
	2853, 3539, 36, -2916, -1833, 1391, 2391, 146, -2016,
	-1225, 1139, 1691, -150, -1601, -668, 1123, 1159, -464,
	-1283, -185, 1093, 681, -692, -948, 206, 972, 248,
	-791, -584, 475, 754, -110, -750, -225, 601, 470,
	-357, -592, 79, 585, 175, -471, -362, 286, 458,
	-75, -457, -121, 375, 268, -237, -347, 77, 354,
	73, -298, -189, 199, 256, -80, -269, -35, 234,
	126, -165, -181, 79, 198, 6, -178, -76, 133,
	122, -73, -140, 13, 131, 40, -103, -76, 64,
	93, -22, -92, -15, 77, 42, -52, -58, 25,
	60, 0, -53, -19, 39, 32, -23, -36, 7,
	34, 6, -27, -15, 18, 20, -10, -19, 0,
	19, 3, -11, -13, 15, 5, 5, -33, 27,
	4, -16,
	//7
};
const int16 FirCoef3[COEFLEN3] = {

	-14, 38, 23, -7, -23, 1, 29, 13, -29,
	-29, 21, 45, -4, -55, -21, 55, 50, -41,
	-78, 12, 95, 30, -96, -79, 73, 124, -26,
	-154, -41, 157, 118, -124, -191, 53, 241, 51,
	-250, -173, 205, 291, -99, -377, -60, 404, 256,
	-347, -457, 190, 622, 68, -705, -414, 655, 821,
	-419, -1248, -73, 1649, 961, -1977, -2753, 2191, 10181,
	14118, 10181, 2191, -2753, -1977, 961, 1649, -73, -1248,
	-419, 821, 655, -414, -705, 68, 622, 190, -457,
	-347, 256, 404, -60, -377, -99, 291, 205, -173,
	-250, 51, 241, 53, -191, -124, 118, 157, -41,
	-154, -26, 124, 73, -79, -96, 30, 95, 12,
	-78, -41, 50, 55, -21, -55, -4, 45, 21,
	-29, -29, 13, 29, 1, -23, -7, 23, 38,
	-14
};
const int16 FirCoef4[COEFLEN4] = {
	1057, 4108, 8675, 11817, 10134, 3529, -3470, -5451, -1669,
	3007, 3420, -185, -2959, -1661, 1536, 2322, -27, -2052,
	-1085, 1248, 1620, -288, -1606, -542, 1191, 1075, -568,
	-1255, -74, 1119, 590, -759, -894, 294, 961, 160,
	-819, -514, 533, 713, -182, -743, -154, 625, 414,
	-404, -559, 138, 579, 118, -489, -316, 323, 429,
	-120, -450, -75, 387, 230, -264, -322, 113, 344,
	37, -304, -157, 217, 233, -106, -258, -6, 234,
	100, -175, -161, 97, 186, -15, -176, -56, 138,
	105, -85, -128, 27, 127, 24, -105, -62, 70,
	83, -32, -86, -4, 75, 32, -55, -49, 31,
	55, -7, -50, -13, 40, 25, -26, -31, 11,
	31, 1, -26, -10, 19, 16, -11, -18, 3,
	17, 3, -16, -7, 11, 22, -40, 19, 2,
	//-4
};



/*******************************data struction***********************************/


#ifdef ARM_BES1000

// �ܵĵ�ַ�ռ䣺  0xa000 ~0x19fff
// ����������:
// APB_CODEC  space : 0x0a000 ~0x0ffff
// CODEC REIGSTER SPACE : 0X10000~11FFF
// Anc register space : 0x12000~13fff

// ANC memory space :
// Fs filter coef Ch0 memory : 0x14000~107ff
// Fs filter coef Ch1 memory : 0x14800~10fff
// Pdu filter coef Ch0 memory : 0x15000~117ff
// Pdu filter coef Ch1 memory : 0x15800~11fff
// Fs filter sample Ch0 memory : 0x16000~127ff
// Fs filter sample Ch1 memory : 0x16800~12fff
// Pdu filter sample Ch0 memory : 0x17000~137ff
// Pdu filter sample Ch1 memory : 0x17800~13fff

#define ANC_BASE                            (CODEC_BASE + 0x8000)

int16 *FsCoefOutMemCh0 = (int16 *)(CODEC_BASE + 0xA000);
int16 *FsCoefOutMemCh1 = (int16 *)(CODEC_BASE + 0xA800);
int16 *PduCoefOutMemCh0 = (int16 *)(CODEC_BASE + 0xB000);
int16 *PduCoefOutMemCh1 = (int16 *)(CODEC_BASE + 0xB800);

int16 *FsSampleMemCh0 = (int16 *)(CODEC_BASE + 0xC000);
int16 *FsSampleMemCh1 = (int16 *)(CODEC_BASE + 0xC800);
int16 *PduSampleMemCh0 = (int16 *)(CODEC_BASE + 0xD000);
int16 *PduSampleMemCh1 = (int16 *)(CODEC_BASE + 0xD800);

volatile struct _Mute_Gain_coef *Mute_Gain_coef_p=(volatile struct _Mute_Gain_coef *)(ANC_BASE + 0x10);

volatile struct _ANC_enble *ANC_enble_p=(volatile struct _ANC_enble *)ANC_BASE;

volatile struct _Fs_fir_order *Fs_fir_order_p = (volatile struct _Fs_fir_order *)(ANC_BASE + 0x04);
volatile struct _Pdu_fir_order *Pdu_fir_order_p = (volatile struct _Pdu_fir_order*)(ANC_BASE + 0x08);
volatile struct _Fir_Gain *Fir_Gain_p = (volatile struct _Fir_Gain *)(ANC_BASE + 0x0c);


volatile struct fir_config *Fs_Ch0_fir_config_p = (volatile struct fir_config *)(ANC_BASE + 0x20);
volatile struct fir_control *Fs_Ch0_fir_control_p = (volatile struct fir_control *)(ANC_BASE + 0x24);

volatile struct fir_config *Fs_Ch1_fir_config_p = (volatile struct fir_config *)(ANC_BASE + 0x28);
volatile struct fir_control *Fs_Ch1_fir_control_p = (volatile struct fir_control *)(ANC_BASE + 0x2c);

volatile struct fir_config *Pdu_Ch0_fir_config_p = (volatile struct fir_config *)(ANC_BASE + 0x30);
volatile struct fir_control *Pdu_Ch0_fir_control_p = (volatile struct fir_control *)(ANC_BASE + 0x34);

volatile struct fir_config *Pdu_Ch1_fir_config_p = (volatile struct fir_config *)(ANC_BASE + 0x38);
volatile struct fir_control *Pdu_Ch1_fir_control_p = (volatile struct fir_control *)(ANC_BASE + 0x3c);


volatile int32 Fs_Ch0_FinishedFlag=0;
volatile int32 Fs_Ch1_FinishedFlag=0;
volatile int32 Pdu_Ch0_FinishedFlag=0;
volatile int32 Pdu_Ch1_FinishedFlag=0;

volatile int32 DMAFlag=0;

volatile uint32 fir_sample_start_Fs_ch0=0;
volatile uint32 fir_sample_start_Fs_ch1=0;
volatile uint32 fir_sample_start_Pdu_ch0=0;
volatile uint32 fir_sample_start_Pdu_ch1=0;

static uint8_t chan_enabled[2];
static int8_t cur_gain[2];
static HAL_CODEC_DAC_RESET_CALLBACK anc_dac_reset_fp;
static HAL_CODEC_DAC_RESET_CALLBACK auxmic_dac_reset_fp;


#else


int16 FirCoefOutMem1[MEMLEN] = { 0 };
int16 FirCoefOutMem2[MEMLEN] = { 0 };
int16 FirCoefOutMem3[MEMLEN] = { 0 };
int16 FirCoefOutMem4[MEMLEN] = { 0 };

int16 FirDataMem1[MEMLEN] = { 0 };
int16 FirDataMem2[MEMLEN] = { 0 };
int16 FirDataMem3[MEMLEN] = { 0 };
int16 FirDataMem4[MEMLEN] = { 0 };



struct fir_config Fs_Ch0_fir_config;
struct fir_config Fs_Ch1_fir_config;
struct fir_config Pdu_Ch0_fir_config;
struct fir_config Pdu_Ch1_fir_config;

struct fir_control Fs_Ch0_fir_control;
struct fir_control Fs_Ch1_fir_control;
struct fir_control Pdu_Ch0_fir_control;
struct fir_control Pdu_Ch1_fir_control;

struct _Fs_fir_order Fs_fir_order;
struct _Pdu_fir_order Pdu_fir_order;
struct _Fir_Gain Fir_Gain;
struct _ANC_enble ANC_enble;



int16 *FsCoefOutMemCh0 = FirCoefOutMem1;
int16 *FsCoefOutMemCh1 = FirCoefOutMem2;
int16 *PduCoefOutMemCh0 = FirCoefOutMem3;
int16 *PduCoefOutMemCh1 = FirCoefOutMem4;


int16 *FsSampleMemCh0 = FirDataMem1;
int16 *FsSampleMemCh1 = FirDataMem2;
int16 *PduSampleMemCh0 = FirDataMem3;
int16 *PduSampleMemCh1 = FirDataMem4;




struct _Fs_fir_order *Fs_fir_order_p = &Fs_fir_order;
struct _Pdu_fir_order *Pdu_fir_order_p = &Pdu_fir_order;
struct _Fir_Gain *Fir_Gain_p = &Fir_Gain;
struct _ANC_enble *ANC_enble_p = &ANC_enble;

struct fir_config *Fs_Ch0_fir_config_p = &Fs_Ch0_fir_config;
struct fir_control *Fs_Ch0_fir_control_p = &Fs_Ch0_fir_control;

struct fir_config *Fs_Ch1_fir_config_p = &Fs_Ch1_fir_config;
struct fir_control *Fs_Ch1_fir_control_p = &Fs_Ch1_fir_control;

struct fir_config *Pdu_Ch0_fir_config_p = &Pdu_Ch0_fir_config;
struct fir_control *Pdu_Ch0_fir_control_p = &Pdu_Ch0_fir_control;

struct fir_config *Pdu_Ch1_fir_config_p = &Pdu_Ch1_fir_config;
struct fir_control *Pdu_Ch1_fir_control_p = &Pdu_Ch1_fir_control;




#endif



/*******************************FIR hardware filter*********************************/

#ifdef ARM_BES1000

void CODEC_IRQHandler(void)
{

	if (Fs_Ch0_fir_control_p->Fir_irq_status == 1)
	{
		Fs_Ch0_fir_control_p->Fir_irq_status = 1;
		Fs_Ch0_FinishedFlag=1;

	}

	if (Fs_Ch1_fir_control_p->Fir_irq_status == 1)
	{
		Fs_Ch1_fir_control_p->Fir_irq_status = 1;
		Fs_Ch1_FinishedFlag=1;
	}

	if (Pdu_Ch0_fir_control_p->Fir_irq_status == 1)
	{
		Pdu_Ch0_fir_control_p->Fir_irq_status = 1;
		Pdu_Ch0_FinishedFlag=1;
	}

	if (Pdu_Ch1_fir_control_p->Fir_irq_status == 1)
	{
		Pdu_Ch1_fir_control_p->Fir_irq_status = 1;
		Pdu_Ch1_FinishedFlag=1;

	}

	return;
}


static void DMA_IRQHandler(uint8_t chan, uint32_t remain_dst_tsize, uint32_t error, struct HAL_DMA_DESC_T *lli)
{
	DMAFlag=1;

}


#endif


static void memcpy_int16_to_int16_with_gain(int16_t *dest, const int16_t *src, int16_t num,int16_t gain)
{
    int16_t i;
    for (i = 0; i < num; i++){
        dest[i] = (src[i]*(int32_t)gain)/64; //
    }
    return;
}

//
void memcpy_int16_to_int16(int16 *dest, const int16 *src, int16 num)
{

#ifdef ARM_BES1000


	struct HAL_DMA_CH_CFG_T dma_cfg;

	DMAFlag=0;

	memset(&dma_cfg, 0, sizeof(dma_cfg));
	dma_cfg.ch = hal_gpdma_get_chan(HAL_GPDMA_MEM, HAL_DMA_HIGH_PRIO);

	dma_cfg.dst = (uint32)dest; // useless
	dma_cfg.dst_bsize = HAL_DMA_BSIZE_1;
	dma_cfg.dst_periph = 0;
	dma_cfg.dst_width = HAL_DMA_WIDTH_HALFWORD;

	dma_cfg.handler = DMA_IRQHandler;

	dma_cfg.src_bsize = HAL_DMA_BSIZE_1;
	dma_cfg.src_periph = 0; // useless
	dma_cfg.src_tsize = num;
	dma_cfg.src_width = HAL_DMA_WIDTH_HALFWORD;

	dma_cfg.try_burst = 1;
	dma_cfg.type = HAL_DMA_FLOW_M2M_DMA;
	dma_cfg.src = (uint32)src;

	hal_gpdma_start(&dma_cfg);

	while(DMAFlag==0)
	{

	}

	hal_gpdma_free_chan(dma_cfg.ch);



#else


	int32 i;

	for (i = 0; i < num;i++)
	{
		dest[i] = src[i];
	}

#endif

	return;
}


void memset_int16_to_int16(int16 *dest, int16 value, int16 num)
{
	int32 i;

	for (i = 0; i < num; i++)
	{
		dest[i] = value;
	}

	return;
}


//init
void FsHardwarCh0(const int16 Coef[], int16 CoefLen)
{
	ANC_enble_p->Apb_anc_sel_fs_ch0 = 0;

	memcpy_int16_to_int16(FsCoefOutMemCh0, Coef, CoefLen);
	memset_int16_to_int16(FsSampleMemCh0, 0x0, CoefLen);


	Fs_Ch0_fir_config_p->fir_sample_start = CoefLen;
	Fs_Ch0_fir_config_p->result_base_addr = CoefLen;
	Fs_fir_order_p->Fs_fir_order_Ch0 = CoefLen-1;


	Fs_Ch0_fir_control_p->fir_mode = 1;
	Fs_Ch0_fir_control_p->Fir_irq_status = 1;
	Fs_Ch0_fir_control_p->Fir_irq_mask = 1;

	Fir_Gain_p->Fs_gain_select_ch0 = 6;


	return;
}

void FsHardwarCh1(const int16 Coef[], int16 CoefLen)
{
	ANC_enble_p->Apb_anc_sel_fs_ch1 = 0;

	memcpy_int16_to_int16(FsCoefOutMemCh1, Coef, CoefLen);
	memset_int16_to_int16(FsSampleMemCh1, 0x0, CoefLen);

	Fs_Ch1_fir_config_p->fir_sample_start = CoefLen;
	Fs_Ch1_fir_config_p->result_base_addr = CoefLen;
	Fs_fir_order_p->Fs_fir_order_Ch1 = CoefLen-1;

	Fs_Ch1_fir_control_p->fir_mode = 1;
	Fs_Ch1_fir_control_p->Fir_irq_status =1 ;
	Fs_Ch1_fir_control_p->Fir_irq_mask = 1;

	Fir_Gain_p->Fs_gain_select_ch1 = 6;

	return;
}
void PduHardwarCh0(const int16 Coef[], int16 CoefLen)
{
	ANC_enble_p->Apb_anc_sel_pdu_ch0 = 0;

	memcpy_int16_to_int16(PduCoefOutMemCh0, Coef, CoefLen);
	memset_int16_to_int16(PduSampleMemCh0, 0x0, CoefLen);

	Pdu_Ch0_fir_config_p->fir_sample_start = CoefLen;
	Pdu_Ch0_fir_config_p->result_base_addr = CoefLen;
	Pdu_fir_order_p->Pdu_fir_order_Ch0 = CoefLen-1;


	Pdu_Ch0_fir_control_p->fir_mode = 1;
	Pdu_Ch0_fir_control_p->Fir_irq_status = 1;
	Pdu_Ch0_fir_control_p->Fir_irq_mask = 1;

	Fir_Gain_p->Pdu_gain_select_ch0 = 6;

	return;
}
void PduHardwarCh1(const int16 Coef[], int16 CoefLen)
{
	ANC_enble_p->Apb_anc_sel_pdu_ch1 = 0;

	memcpy_int16_to_int16(PduCoefOutMemCh1, Coef, CoefLen);
	memset_int16_to_int16(PduSampleMemCh1, 0x0, CoefLen);

	Pdu_Ch1_fir_config_p->fir_sample_start = CoefLen;
	Pdu_Ch1_fir_config_p->result_base_addr = CoefLen;
	Pdu_fir_order_p->Pdu_fir_order_Ch1 = CoefLen-1;


	Pdu_Ch1_fir_control_p->fir_mode = 1;
	Pdu_Ch1_fir_control_p->Fir_irq_status = 1;
	Pdu_Ch1_fir_control_p->Fir_irq_mask = 1;

	Fir_Gain_p->Pdu_gain_select_ch1 = 6;

	return;
}



//calculating
void FirHardwareCal1(const int16 Input[], int16 Output[], uint32 FrameLen)
{

#ifndef ARM_BES1000

	__int64 sum;
	uint32 i,j;
	uint32 FirDataReadNum = (Fs_Ch0_fir_config_p->fir_sample_start + MEMLEN - Fs_fir_order_p->Fs_fir_order_Ch0) % MEMLEN;

#endif

	uint32 fir_sample_start = Fs_Ch0_fir_config_p->fir_sample_start;

	ANC_enble_p->Apb_anc_sel_fs_ch0 = 0;


	if (MEMLEN - fir_sample_start+1 >= FrameLen)
	{

		memcpy_int16_to_int16(FsSampleMemCh0 + fir_sample_start, Input, FrameLen);
		fir_sample_start = fir_sample_start + FrameLen;
	}
	else
	{
		memcpy_int16_to_int16(FsSampleMemCh0 + fir_sample_start, Input, MEMLEN - fir_sample_start);
		memcpy_int16_to_int16(FsSampleMemCh0, Input + (MEMLEN - fir_sample_start), FrameLen - (MEMLEN - fir_sample_start));

		fir_sample_start = FrameLen - (MEMLEN - fir_sample_start);
	}



#ifdef ARM_BES1000


// 	Fs_Ch0_fir_config_p->fir_sample_num =FrameLen-1;
// 	ANC_enble_p->Apb_anc_sel_fs_ch0 = 1;
// 	Fs_Ch0_fir_control_p->Fir_irq_status = 1;
// 	Fs_Ch0_fir_control_p->fir_start = 1;
//
// 	while (Fs_Ch0_fir_control_p->Fir_irq_status == 0)
// 	{
//
// 	}



	Fs_Ch0_fir_config_p->fir_sample_num = FrameLen - 1;
	ANC_enble_p->Apb_anc_sel_fs_ch0 = 1;
	Fs_Ch0_FinishedFlag=0;

	Fs_Ch0_fir_control_p->fir_start = 1;

// 	while (Fs_Ch0_FinishedFlag == 0)
// 	{
//
// 	}
	fir_sample_start_Fs_ch0 = fir_sample_start;



#else

	for (j = 0; j < FrameLen;j++)
	{
		sum = 0;
		for (i = 0; i <= Fs_fir_order_p->Fs_fir_order_Ch0; i++)
		{
			sum = sum + (((FsCoefOutMemCh0[Fs_fir_order_p->Fs_fir_order_Ch0 - i]) * FsSampleMemCh0[(FirDataReadNum + i) % MEMLEN]));
		}

		sum = sum >> 9;
		sum = sum >> 6;

// 		sum = sum /512;
// 		sum = sum /64;

		if (sum>32767)
		{
			sum = 32767;
		}
		else if (sum<-32768)
		{
			sum = -32768;
		}

		FsCoefOutMemCh0[Fs_Ch0_fir_config_p->result_base_addr + j] =(int16) sum;

		FirDataReadNum++;

		FirDataReadNum = FirDataReadNum % MEMLEN;
	}



	ANC_enble_p->Apb_anc_sel_fs_ch0 = 0;

	memcpy_int16_to_int16(Output, &FsCoefOutMemCh0[Fs_Ch0_fir_config_p->result_base_addr], FrameLen);

	Fs_Ch0_fir_config_p->fir_sample_start = fir_sample_start;

#endif

	return;
}



//calculating
void FirHardwareCal2(const int16 Input[], int16 Output[], uint32 FrameLen)
{
#ifndef ARM_BES1000

	__int64 sum;
	uint32 i, j;
	uint32 FirDataReadNum = (Fs_Ch1_fir_config_p->fir_sample_start + MEMLEN - Fs_fir_order_p->Fs_fir_order_Ch1) % MEMLEN;;
#endif

	uint32 fir_sample_start = Fs_Ch1_fir_config_p->fir_sample_start;
	ANC_enble_p->Apb_anc_sel_fs_ch1 = 0;



	if (MEMLEN - fir_sample_start + 1 >= FrameLen)
	{

		memcpy_int16_to_int16(FsSampleMemCh1 + fir_sample_start, Input, FrameLen);
		fir_sample_start = fir_sample_start + FrameLen;
	}
	else
	{
		memcpy_int16_to_int16(FsSampleMemCh1 + fir_sample_start, Input, MEMLEN - fir_sample_start);
		memcpy_int16_to_int16(FsSampleMemCh1, Input + (MEMLEN - fir_sample_start), FrameLen - (MEMLEN - fir_sample_start));

		fir_sample_start = FrameLen - (MEMLEN - fir_sample_start);
	}

#ifdef ARM_BES1000






// 	Fs_Ch1_fir_config_p->fir_sample_num = FrameLen-1;
//
// 	ANC_enble_p->Apb_anc_sel_fs_ch1 = 1;
// 	Fs_Ch1_fir_control_p->Fir_irq_status = 1;
// 	Fs_Ch1_fir_control_p->fir_start = 1;
//
// 	while (Fs_Ch1_fir_control_p->Fir_irq_status == 0)
// 	{
// 	}

	Fs_Ch1_fir_config_p->fir_sample_num = FrameLen - 1;

	ANC_enble_p->Apb_anc_sel_fs_ch1 = 1;
	Fs_Ch1_FinishedFlag = 0;
	Fs_Ch1_fir_control_p->fir_start = 1;

// 	while (Fs_Ch1_FinishedFlag == 0)
// 	{
// 	}
	fir_sample_start_Fs_ch1 = fir_sample_start;



#else


	for (j = 0; j < FrameLen; j++)
	{
		sum = 0;
		for (i = 0; i <= Fs_fir_order_p->Fs_fir_order_Ch1; i++)
		{
			sum = sum + (((FsCoefOutMemCh1[Fs_fir_order_p->Fs_fir_order_Ch1 - i]) * FsSampleMemCh1[(FirDataReadNum + i) % MEMLEN]));
		}

		sum = sum >> 9;
		sum = sum >> 6;

		// 		sum = sum /512;
		// 		sum = sum /64;

		if (sum > 32767)
		{
			sum = 32767;
		}
		else if (sum < -32768)
		{
			sum = -32768;
		}

		FsCoefOutMemCh1[Fs_Ch1_fir_config_p->result_base_addr + j] = (int16)sum;

		FirDataReadNum++;

		FirDataReadNum = FirDataReadNum % MEMLEN;
	}

	ANC_enble_p->Apb_anc_sel_fs_ch1 = 0;

	memcpy_int16_to_int16(Output, &FsCoefOutMemCh1[Fs_Ch1_fir_config_p->result_base_addr], FrameLen);

	Fs_Ch1_fir_config_p->fir_sample_start = fir_sample_start;

#endif



	return;
}



//calculating
void FirHardwareCal3(const int16 Input[], int16 Output[], uint32 FrameLen)
{
#ifndef ARM_BES1000

	__int64 sum;
	uint32 i, j;
	uint32 FirDataReadNum = (Pdu_Ch0_fir_config_p->fir_sample_start + MEMLEN - Pdu_fir_order_p->Pdu_fir_order_Ch0) % MEMLEN;
#endif

	uint32 fir_sample_start = Pdu_Ch0_fir_config_p->fir_sample_start;

	ANC_enble_p->Apb_anc_sel_pdu_ch0 = 0;


	if (MEMLEN - fir_sample_start + 1 >= FrameLen)
	{

		memcpy_int16_to_int16(PduSampleMemCh0 + fir_sample_start, Input, FrameLen);
		fir_sample_start = fir_sample_start + FrameLen;
	}
	else
	{
		memcpy_int16_to_int16(PduSampleMemCh0 + fir_sample_start, Input, MEMLEN - fir_sample_start);
		memcpy_int16_to_int16(PduSampleMemCh0, Input + (MEMLEN - fir_sample_start), FrameLen - (MEMLEN - fir_sample_start));

		fir_sample_start = FrameLen - (MEMLEN - fir_sample_start);
	}


#ifdef ARM_BES1000



// 	Pdu_Ch0_fir_config_p->fir_sample_num = FrameLen-1;
//
// 	ANC_enble_p->Apb_anc_sel_pdu_ch0 = 1;
// 	Pdu_Ch0_fir_control_p->Fir_irq_status = 1;
// 	Pdu_Ch0_fir_control_p->fir_start = 1;
//
// 	while (Pdu_Ch0_fir_control_p->Fir_irq_status == 0)
// 	{
// 	}


	Pdu_Ch0_fir_config_p->fir_sample_num = FrameLen - 1;

	ANC_enble_p->Apb_anc_sel_pdu_ch0 = 1;
	Pdu_Ch0_FinishedFlag = 0;
	Pdu_Ch0_fir_control_p->fir_start = 1;

// 	while (Pdu_Ch0_FinishedFlag  == 0)
// 	{
// 	}

	fir_sample_start_Pdu_ch0 = fir_sample_start;


#else


	for (j = 0; j < FrameLen; j++)
	{
		sum = 0;
		for (i = 0; i <= Pdu_fir_order_p->Pdu_fir_order_Ch0; i++)
		{
			sum = sum + (((PduCoefOutMemCh0[Pdu_fir_order_p->Pdu_fir_order_Ch0 - i]) * PduSampleMemCh0[(FirDataReadNum + i) % MEMLEN]));
		}

		sum = sum >> 9;
		sum = sum >> 6;

		// 		sum = sum /512;
		// 		sum = sum /64;

		if (sum > 32767)
		{
			sum = 32767;
		}
		else if (sum < -32768)
		{
			sum = -32768;
		}

		PduCoefOutMemCh0[Pdu_Ch0_fir_config_p->result_base_addr + j] = (int16)sum;

		FirDataReadNum++;

		FirDataReadNum = FirDataReadNum % MEMLEN;
	}

	ANC_enble_p->Apb_anc_sel_pdu_ch0 = 0;

	memcpy_int16_to_int16(Output, &PduCoefOutMemCh0[Pdu_Ch0_fir_config_p->result_base_addr], FrameLen);

	Pdu_Ch0_fir_config_p->fir_sample_start = fir_sample_start;

#endif





	return;
}


//calculating
void FirHardwareCal4(const int16 Input[], int16 Output[], uint32 FrameLen)
{
#ifndef ARM_BES1000

	__int64 sum;
	uint32 i, j;
	uint32 FirDataReadNum = (Pdu_Ch1_fir_config_p->fir_sample_start + MEMLEN - Pdu_fir_order_p->Pdu_fir_order_Ch1) % MEMLEN;
#endif

	uint32 fir_sample_start = Pdu_Ch1_fir_config_p->fir_sample_start;
	ANC_enble_p->Apb_anc_sel_pdu_ch1 = 0;


	if (MEMLEN - fir_sample_start + 1 >= FrameLen)
	{

		memcpy_int16_to_int16(PduSampleMemCh1 + fir_sample_start, Input, FrameLen);
		fir_sample_start = fir_sample_start + FrameLen;
	}
	else
	{
		memcpy_int16_to_int16(PduSampleMemCh1 + fir_sample_start, Input, MEMLEN - fir_sample_start);
		memcpy_int16_to_int16(PduSampleMemCh1, Input + (MEMLEN - fir_sample_start), FrameLen - (MEMLEN - fir_sample_start));

		fir_sample_start = FrameLen - (MEMLEN - fir_sample_start);
	}


#ifdef ARM_BES1000

// 	Pdu_Ch1_fir_config_p->fir_sample_num = FrameLen-1;
//
// 	ANC_enble_p->Apb_anc_sel_pdu_ch1 = 1;
// 	Pdu_Ch1_fir_control_p->Fir_irq_status = 1;
// 	Pdu_Ch1_fir_control_p->fir_start = 1;
//
// 	while (Pdu_Ch1_fir_control_p->Fir_irq_status == 0)
// 	{
// 	}

	Pdu_Ch1_fir_config_p->fir_sample_num = FrameLen - 1;

	ANC_enble_p->Apb_anc_sel_pdu_ch1 = 1;
	Pdu_Ch1_FinishedFlag = 0;
	Pdu_Ch1_fir_control_p->fir_start = 1;

// 	while (Pdu_Ch1_FinishedFlag == 0)
// 	{
//
// 	}

	fir_sample_start_Pdu_ch1 = fir_sample_start;


#else


	for (j = 0; j < FrameLen; j++)
	{
		sum = 0;
		for (i = 0; i <= Pdu_fir_order_p->Pdu_fir_order_Ch1; i++)
		{
			sum = sum + (((PduCoefOutMemCh1[Pdu_fir_order_p->Pdu_fir_order_Ch1 - i]) * PduSampleMemCh1[(FirDataReadNum + i) % MEMLEN]));
		}

		sum = sum >> 9;
		sum = sum >> 6;

		// 		sum = sum /512;
		// 		sum = sum /64;

		if (sum > 32767)
		{
			sum = 32767;
		}
		else if (sum < -32768)
		{
			sum = -32768;
		}

		PduCoefOutMemCh1[Pdu_Ch1_fir_config_p->result_base_addr + j] = (int16)sum;

		FirDataReadNum++;

		FirDataReadNum = FirDataReadNum % MEMLEN;
	}


	 	ANC_enble_p->Apb_anc_sel_pdu_ch1 = 0;

	 	memcpy_int16_to_int16(Output, &PduCoefOutMemCh1[Pdu_Ch1_fir_config_p->result_base_addr], FrameLen);

	 	Pdu_Ch1_fir_config_p->fir_sample_start = fir_sample_start;

#endif




	return;
}



const int16 InputBuf[TESTLEN] =
{
   0
};


int16 OutputBuf1[TESTOUTLEN];
int16 OutputBuf2[TESTOUTLEN];
int16 OutputBuf3[TESTOUTLEN];
int16 OutputBuf4[TESTOUTLEN];


/*******************************MAIN functiong*********************************/
void fir_test(void)
{
	int32 i;

	for (i = 0; i < TESTLEN;i++)
	{

		//InputBuf[i] = InputBuf[i] / 2;

	}

	FsHardwarCh0(FirCoef1, COEFLEN1);
	FsHardwarCh1(FirCoef2, COEFLEN2);
	PduHardwarCh0(FirCoef3, COEFLEN3);
	PduHardwarCh1(FirCoef4, COEFLEN4);

	for (i = 0; i < TESTOUTLEN / FRAMELEN; i++)
	{
		FirHardwareCal1(InputBuf + i*FRAMELEN, OutputBuf1 + i*FRAMELEN, FRAMELEN);
 		FirHardwareCal2(InputBuf + i*FRAMELEN, OutputBuf2 + i*FRAMELEN, FRAMELEN);
 		FirHardwareCal3(InputBuf + i*FRAMELEN, OutputBuf3 + i*FRAMELEN, FRAMELEN);
		FirHardwareCal4(InputBuf + i*FRAMELEN, OutputBuf4 + i*FRAMELEN, FRAMELEN);


#ifdef ARM_BES1000

		while (Fs_Ch0_FinishedFlag == 0)
		{

		}

		ANC_enble_p->Apb_anc_sel_fs_ch0 = 0;

		memcpy_int16_to_int16(OutputBuf1 + i*FRAMELEN, &FsCoefOutMemCh0[Fs_Ch0_fir_config_p->result_base_addr], FRAMELEN);

		Fs_Ch0_fir_config_p->fir_sample_start = fir_sample_start_Fs_ch0;



		while (Fs_Ch1_FinishedFlag == 0)
		{

		}

		ANC_enble_p->Apb_anc_sel_fs_ch1 = 0;

		memcpy_int16_to_int16(OutputBuf2 + i*FRAMELEN, &FsCoefOutMemCh1[Fs_Ch1_fir_config_p->result_base_addr], FRAMELEN);

		Fs_Ch1_fir_config_p->fir_sample_start = fir_sample_start_Fs_ch1;

		while (Pdu_Ch0_FinishedFlag  == 0)
		{

		}
		ANC_enble_p->Apb_anc_sel_pdu_ch0 = 0;

		memcpy_int16_to_int16(OutputBuf3 + i*FRAMELEN, &PduCoefOutMemCh0[Pdu_Ch0_fir_config_p->result_base_addr], FRAMELEN);

		Pdu_Ch0_fir_config_p->fir_sample_start = fir_sample_start_Pdu_ch0;

		while (Pdu_Ch1_FinishedFlag == 0)
		{

		}

		ANC_enble_p->Apb_anc_sel_pdu_ch1 = 0;

		memcpy_int16_to_int16(OutputBuf4 + i*FRAMELEN, &PduCoefOutMemCh1[Pdu_Ch1_fir_config_p->result_base_addr], FRAMELEN);

		Pdu_Ch1_fir_config_p->fir_sample_start = fir_sample_start_Pdu_ch1;
#endif


	}




	return;
}





//init
void AncFsHardwarCh0(const int16 Coef[], int16 CoefLen)
{
	ANC_enble_p->Apb_anc_sel_fs_ch0 = 0;

	memcpy_int16_to_int16(FsCoefOutMemCh0, Coef, CoefLen);
	ANC_enble_p->apb_anc_sel_fs_sample_ch0=0;
	memset_int16_to_int16(FsSampleMemCh0, 0x0, 512);
	ANC_enble_p->apb_anc_sel_fs_sample_ch0=1;

	Fs_fir_order_p->Fs_fir_order_Ch0 = CoefLen-1;

	Fs_Ch0_fir_control_p->fir_mode = 0;

	ANC_enble_p->Apb_anc_sel_fs_ch0 = 1;


	Fir_Gain_p->Fs_gain_select_ch0 = 6;


	return;
}

void AncFsHardwarCh1(const int16 Coef[], int16 CoefLen)
{
	ANC_enble_p->Apb_anc_sel_fs_ch1 = 0;

	memcpy_int16_to_int16(FsCoefOutMemCh1, Coef, CoefLen);
	ANC_enble_p->apb_anc_sel_fs_sample_ch1=0;
	memset_int16_to_int16(FsSampleMemCh1, 0x0, 512);
	ANC_enble_p->apb_anc_sel_fs_sample_ch1=1;

	Fs_Ch1_fir_control_p->fir_mode = 0;

	ANC_enble_p->Apb_anc_sel_fs_ch1 = 1;

	Fs_fir_order_p->Fs_fir_order_Ch1 = CoefLen-1;

	Fir_Gain_p->Fs_gain_select_ch1 = 6;

	return;
}
void AncPduHardwarCh0(const int16 Coef[], int16 CoefLen,int16 gain)
{
	ANC_enble_p->Apb_anc_sel_pdu_ch0 = 0;

	//memcpy_int16_to_int16(PduCoefOutMemCh0, Coef, CoefLen);
	memcpy_int16_to_int16_with_gain(PduCoefOutMemCh0, Coef, CoefLen,gain);

	ANC_enble_p->apb_anc_sel_pdu_sample_ch0=0;
	memset_int16_to_int16(PduSampleMemCh0, 0x0, 512);
	ANC_enble_p->apb_anc_sel_pdu_sample_ch0=1;


	Pdu_Ch0_fir_control_p->fir_mode = 0;

	ANC_enble_p->Apb_anc_sel_pdu_ch0 = 1;


	Pdu_fir_order_p->Pdu_fir_order_Ch0 = CoefLen-1;

	Fir_Gain_p->Pdu_gain_select_ch0 = 6;


	return;
}
void AncPduHardwarCh1(const int16 Coef[], int16 CoefLen,int16 gain)
{
	ANC_enble_p->Apb_anc_sel_pdu_ch1 = 0;

//	memcpy_int16_to_int16(PduCoefOutMemCh1, Coef, CoefLen);
	memcpy_int16_to_int16_with_gain(PduCoefOutMemCh1, Coef, CoefLen,gain);

	ANC_enble_p->apb_anc_sel_pdu_sample_ch1=0;
	memset_int16_to_int16(PduSampleMemCh1, 0x0, 512);
	ANC_enble_p->apb_anc_sel_pdu_sample_ch1=1;


	Pdu_Ch1_fir_control_p->fir_mode = 0;
	ANC_enble_p->Apb_anc_sel_pdu_ch1 = 1;


	Pdu_fir_order_p->Pdu_fir_order_Ch1 = CoefLen-1;

	Fir_Gain_p->Pdu_gain_select_ch1 = 6;

	return;
}


//updata coef

void AncFsHardwarCh0_UpdateCoef(const int16 Coef[], int16 CoefLen)
{
	ANC_enble_p->Apb_anc_sel_fs_ch0 = 0;

	memcpy_int16_to_int16(FsCoefOutMemCh0, Coef, CoefLen);

	ANC_enble_p->Apb_anc_sel_fs_ch0 = 1;

	return;
}

void AncFsHardwarCh1_UpdateCoef(const int16 Coef[], int16 CoefLen)
{
	ANC_enble_p->Apb_anc_sel_fs_ch1 = 0;
	memcpy_int16_to_int16(FsCoefOutMemCh1, Coef, CoefLen);
	ANC_enble_p->Apb_anc_sel_fs_ch1 = 1;

	return;
}

void AncPduHardwarCh0_UpdateCoef(const int16 Coef[], int16 CoefLen)
{
	ANC_enble_p->Apb_anc_sel_pdu_ch0 = 0;
	memcpy_int16_to_int16(PduCoefOutMemCh0, Coef, CoefLen);
	ANC_enble_p->Apb_anc_sel_pdu_ch0 = 1;

	return;
}

void AncPduHardwarCh1_UpdateCoef(const int16 Coef[], int16 CoefLen)
{
	ANC_enble_p->Apb_anc_sel_pdu_ch1 = 0;
	memcpy_int16_to_int16(PduCoefOutMemCh1, Coef, CoefLen);
	ANC_enble_p->Apb_anc_sel_pdu_ch1 = 1;

	return;
}

#if 0
static const int16_t POSSIBLY_UNUSED AncFirCoef2[AUD_COEF_LEN] =
{
  0x4000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

static const struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_huawei_384 = {
    .anc_cfg_ff_l = {
        .gain = 0x17,
        .len = AUD_COEF_LEN,
        .coef =
        {
            //414,923,370,948,353,845,337,489,286,219,176,79,386,-123,578,-268,385,57,314,
            //450,-107,290,246,364,175,291,7,309,113,393,481,-142,650,-271,604,69,138,428,
            //136,391,301,399,
            177,507,21,769,-38,405,137,173,516,-62,636,80,487,234,383,178,
            425,260,283,409,146,605,64,462,-46,472,289,6,279,-169,627,-115,851,82,358,26,
            120,205,332,302,-23,467,-157,567,-27,304,127,-121,520,-158,204,258,471,160,145
            ,-135,130,-46,100,524,-347,693,-324,174,-213,-40,408,-104,403,-103,232,-134,
            316,-172,461,-199,153,-13,-227,376,-221,402,-105,79,-33,65,127,-35,80,-226,274
            ,-274,346,-71,172,-328,-157,204,-311,422,-408,425,-347,431,89,-8,71,-338,127,-
            38,157,-236,202,-366,256,-106,-50,249,-321,499,-436,86,-116,-58,39,-36,-149,0,
            177,-190,435,-497,408,-582,180,90,-218,266,-248,185,-234,45,-148,306,-266,311,
            -96,-116,-29,-322,205,-381,246,-149,37,-66,-30,92,-176,244,-184,220,-174,-2,-
            227,-33,-107,-104,56,-125,419,-329,462,-81,-119,65,-360,118,-323,101,18,78,-
            132,151,-309,62,-202,-41,309,-257,306,-279,-15,-68,-110,148,-26,-110,-30,195,-
            291,352,-496,222,-139,-18,283,-210,333,-316,-36,-77,-4,-269,73,-172,133,190,-
            99,31,-223,259,-245,32,72,-46,135,-214,47,-166,382,-304,251,34,-88,-142,-62,91
            ,-267,138,-186,361,-278,131,-172,-118,178,-105,117,-87,56,-69,-148,-75,8,-104,
            178,-31,-92,282,-243,296,-160,-189,121,-261,210,-122,-62,-35,68,-227,229,-182,
            175,105,-222,256,-320,277,-247,-9,84,-109,9,56,112,-120,-25,-135,62,21,106,-46
            ,13,173,-58,16,-178,-63,-180,193,-87,-256,203,9,120,-8,-129,45,104,-275,136,-
            198,197,-44,-156,239,-146,75,70,-246,178,-46,-37,79,-167,261,201,-179,273,-347
            ,3,126,-505,166,-178,191,166,-198,103,-1,-147,282,-236,56,194,-160,123,-173,41
            ,181,-111,95,-180,-54,-93,120,69,11,173,-50,174,-246,-27,-233,71,-181,52,-2,
            104,-94,-12,293,-89,96,43,-106,89,-105,92,43,-76,37,-189,-122,203,-40,179,-8,-
            97,238,-380,210,-248,82,211,-53,-77,-121,-16,109,57,-85,314,-298,150,-114,-62,
            277,-133,129,-36,-75,103,-111,-48,35,77,79,-17,-49,132,-373,167,-70,-77,113,-
            51,120,-114,134,-160,353,-91,-123,120,-121,122,-199,-26,-33,151,-117,295,-184,
            235,151,-311,233,-457,284,82,-143,-165,28,47,59,
        },
    },
     .anc_cfg_ff_r = {
        .gain = 0x17,
        .len = AUD_COEF_LEN,
        .coef =
        {
            //414,923,370,948,353,845,337,489,286,219,176,79,386,-123,578,-268,385,57,314,
            //450,-107,290,246,364,175,291,7,309,113,393,481,-142,650,-271,604,69,138,428,
            //136,391,301,399,
            177,507,21,769,-38,405,137,173,516,-62,636,80,487,234,383,178,
            425,260,283,409,146,605,64,462,-46,472,289,6,279,-169,627,-115,851,82,358,26,
            120,205,332,302,-23,467,-157,567,-27,304,127,-121,520,-158,204,258,471,160,145
            ,-135,130,-46,100,524,-347,693,-324,174,-213,-40,408,-104,403,-103,232,-134,
            316,-172,461,-199,153,-13,-227,376,-221,402,-105,79,-33,65,127,-35,80,-226,274
            ,-274,346,-71,172,-328,-157,204,-311,422,-408,425,-347,431,89,-8,71,-338,127,-
            38,157,-236,202,-366,256,-106,-50,249,-321,499,-436,86,-116,-58,39,-36,-149,0,
            177,-190,435,-497,408,-582,180,90,-218,266,-248,185,-234,45,-148,306,-266,311,
            -96,-116,-29,-322,205,-381,246,-149,37,-66,-30,92,-176,244,-184,220,-174,-2,-
            227,-33,-107,-104,56,-125,419,-329,462,-81,-119,65,-360,118,-323,101,18,78,-
            132,151,-309,62,-202,-41,309,-257,306,-279,-15,-68,-110,148,-26,-110,-30,195,-
            291,352,-496,222,-139,-18,283,-210,333,-316,-36,-77,-4,-269,73,-172,133,190,-
            99,31,-223,259,-245,32,72,-46,135,-214,47,-166,382,-304,251,34,-88,-142,-62,91
            ,-267,138,-186,361,-278,131,-172,-118,178,-105,117,-87,56,-69,-148,-75,8,-104,
            178,-31,-92,282,-243,296,-160,-189,121,-261,210,-122,-62,-35,68,-227,229,-182,
            175,105,-222,256,-320,277,-247,-9,84,-109,9,56,112,-120,-25,-135,62,21,106,-46
            ,13,173,-58,16,-178,-63,-180,193,-87,-256,203,9,120,-8,-129,45,104,-275,136,-
            198,197,-44,-156,239,-146,75,70,-246,178,-46,-37,79,-167,261,201,-179,273,-347
            ,3,126,-505,166,-178,191,166,-198,103,-1,-147,282,-236,56,194,-160,123,-173,41
            ,181,-111,95,-180,-54,-93,120,69,11,173,-50,174,-246,-27,-233,71,-181,52,-2,
            104,-94,-12,293,-89,96,43,-106,89,-105,92,43,-76,37,-189,-122,203,-40,179,-8,-
            97,238,-380,210,-248,82,211,-53,-77,-121,-16,109,57,-85,314,-298,150,-114,-62,
            277,-133,129,-36,-75,103,-111,-48,35,77,79,-17,-49,132,-373,167,-70,-77,113,-
            51,120,-114,134,-160,353,-91,-123,120,-121,122,-199,-26,-33,151,-117,295,-184,
            235,151,-311,233,-457,284,82,-143,-165,28,47,59,
        },
    },
};

static const struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_huawei_384_d = {
    .anc_cfg_ff_l = {
        .gain = 0x17,
        .len = AUD_COEF_LEN,
        .coef =
        {
            //4512,3876,2124,1525,1911,1713,770,74,-221,-682,-1190,-1179,-793,-637,-738,-649,-243,238,636,894,922,766,680,720,662,393,152,104,75,-93,-202,-37,217,233,26,-98,25,295,556,728,772,673,488,274,56,-132,-213,-156,-17,154,334,443,380,199,118,242,436,544,582,587,482,234,28,49,218,279,146,-15,-8,148,288,320,318,338,338,299,288,276,155,8,80,306,286,-62,-268,-57,202,139,12,194,506,539,318,134,43,-48,-50,137,328,272,32,-136,-130,-33,72,120,59,-28,40,270,422,338,166,89,61,-40,-172,-237,-247,-212,-48,221,376,270,60,-33,-3,36,80,189,294,230,-21,-258,-300,-224,-217,-235,-66,228,302,96,10,246,436,235,-112,-234,-207,-259,-276,-87,125,120,16,15,27,-81,-158,-38,130,125,20,-18,-29,-101,-118,47,250,243,12,-269,-436,-384,-102,184,190,-6,-8,261,376,71,-282,-231,59,67,-294,-555,-346,149,448,305,-61,-233,-74,133,78,-137,-140,142,334,118,-298,-440,-188,75,3,-242,-238,88,356,261,-19,-108,38,94,-117,-319,-176,173,264,-22,-276,-195,3,-1,-116,-75,77,105,10,-18,59,110,70,-25,-140,-216,-172,-57,-39,-148,-178,14,272,330,158,-32,-68,-5,12,-53,-127,-184,-260,-303,-176,85,250,214,146,160,117,-80,-248,-206,-63,-9,-6,49,73,-47,-180,-100,148,265,94,-179,-252,-67,136,89,-171,-313,-120,194,255,58,-36,118,196,-67,-406,-406,-75,242,334,209,-53,-273,-240,-29,41,-43,66,375,374,-122,-520,-333,71,95,-168,-225,-7,173,206,233,264,172,-34,-245,-384,-354,-72,264,284,-33,-242,-81,156,117,-84,-140,-43,25,63,180,278,138,-209,-454,-368,-67,160,189,136,106,63,-18,-46,20,61,-11,-96,-83,-47,-101,-164,-52,204,320,129,-159,-220,-54,35,-86,-172,9,282,283,-9,-213,-70,178,127,-178,-302,-93,95,23,-68,35,131,57,42,190,145,-256,-520,-220,278,371,114,-30,35,46,-82,-201,-223,-147,34,207,179,13,10,188,183,-120,-316,-121,136,33,-266,-292,9,227,129,-19,94,328,288,-70,-330,-206,73,130,-34,-153,-132,-91,-70,-4,91,128,106,103,128,99,-20,-165,-218,-118,64,150,44,-129,-136,61,242,184,-60,-236,-180,29,200,218,110,-42,-174,-243,-212,-92,59,185,246,192,17,-153,-152,1,92,-18,-169,-101,166,302,105,-173,-148,100,111,-231,-454,-190,262,382,156,-32,-7,
            //59,24,-36,-29,-3,-44,-129,-156,-90
            5722,5342,3542,2919,3380,3189,2128,1294,884,272,-440,-638,-447,-490,-796,-893,-650,-317,-44,118,75,-130,-243,-205,-245,-484,-682,-676,-644,-755,-804,-579,-267,-199,-358,-436,-268,39,334,539,616,548,390,202,9,-153,-212,-138,17,203,396,514,456,277,198,323,516,622,656,657,546,293,84,104,272,331,196,35,40,194,330,360,354,367,358,312,294,273,141,-17,46,263,235,-118,-332,-128,123,56,-76,101,408,438,214,25,-71,-165,-170,14,202,143,-96,-263,-256,-160,-52,2,-51,-131,-56,182,344,268,102,31,9,-86,-212,-271,-276,-239,-72,201,358,252,42,-49,-15,27,72,183,292,233,-15,-250,-288,-208,-197,-210,-38,259,336,132,48,287,480,281,-66,-189,-163,-214,-230,-41,170,163,56,52,63,-45,-124,-7,158,152,46,5,-9,-83,-100,65,264,252,18,-265,-434,-385,-106,177,180,-20,-24,245,362,56,-300,-251,39,48,-312,-572,-362,133,434,292,-72,-246,-88,119,64,-152,-156,125,318,102,-314,-456,-204,60,-11,-253,-246,82,352,260,-14,-98,52,110,-101,-302,-156,197,290,5,-250,-168,31,28,-86,-45,105,132,38,14,94,143,102,9,-103,-180,-140,-27,-9,-117,-150,38,294,351,178,-15,-54,8,23,-44,-121,-179,-255,-296,-170,90,254,218,150,164,121,-77,-246,-204,-63,-11,-12,40,63,-58,-194,-118,127,244,72,-202,-276,-90,114,68,-191,-331,-136,180,243,49,-40,117,196,-65,-402,-398,-67,249,340,216,-45,-265,-232,-20,49,-38,70,382,384,-112,-510,-320,85,108,-156,-210,11,191,224,253,286,192,-18,-230,-368,-339,-58,278,298,-20,-230,-68,172,132,-70,-125,-27,41,80,197,294,153,-195,-439,-354,-57,166,194,140,106,59,-26,-58,5,45,-26,-112,-104,-70,-125,-186,-75,180,297,109,-178,-238,-69,23,-100,-186,-1,276,277,-15,-218,-74,173,121,-183,-306,-98,91,22,-66,37,131,58,46,195,151,-248,-510,-210,288,383,128,-17,47,59,-68,-187,-209,-132,48,219,189,25,24,200,193,-110,-306,-112,146,46,-253,-279,21,240,141,-12,98,333,294,-69,-334,-210,69,123,-46,-166,-144,-104,-84,-16,81,118,96,96,124,95,-24,-166,-216,-116,64,150,44,-131,-140,57,236,174,-72,-246,-190,16,186,206,100,-55,-190,-257,-222,-102,47,174,238,185,11,-157,-156,-4,86,-23,-173,-105,162,301,105,-174,-150,101,115,-226,-448,
            -180,276,397,172,-12,17,83,48,-10,-3,20,-22,-103,-132,-77,
        },
    },
    .anc_cfg_ff_r = {
        .gain = 0x17,
        .len = AUD_COEF_LEN,
        .coef =
        {
            //4512,3876,2124,1525,1911,1713,770,74,-221,-682,-1190,-1179,-793,-637,-738,-649,-243,238,636,894,922,766,680,720,662,393,152,104,75,-93,-202,-37,217,233,26,-98,25,295,556,728,772,673,488,274,56,-132,-213,-156,-17,154,334,443,380,199,118,242,436,544,582,587,482,234,28,49,218,279,146,-15,-8,148,288,320,318,338,338,299,288,276,155,8,80,306,286,-62,-268,-57,202,139,12,194,506,539,318,134,43,-48,-50,137,328,272,32,-136,-130,-33,72,120,59,-28,40,270,422,338,166,89,61,-40,-172,-237,-247,-212,-48,221,376,270,60,-33,-3,36,80,189,294,230,-21,-258,-300,-224,-217,-235,-66,228,302,96,10,246,436,235,-112,-234,-207,-259,-276,-87,125,120,16,15,27,-81,-158,-38,130,125,20,-18,-29,-101,-118,47,250,243,12,-269,-436,-384,-102,184,190,-6,-8,261,376,71,-282,-231,59,67,-294,-555,-346,149,448,305,-61,-233,-74,133,78,-137,-140,142,334,118,-298,-440,-188,75,3,-242,-238,88,356,261,-19,-108,38,94,-117,-319,-176,173,264,-22,-276,-195,3,-1,-116,-75,77,105,10,-18,59,110,70,-25,-140,-216,-172,-57,-39,-148,-178,14,272,330,158,-32,-68,-5,12,-53,-127,-184,-260,-303,-176,85,250,214,146,160,117,-80,-248,-206,-63,-9,-6,49,73,-47,-180,-100,148,265,94,-179,-252,-67,136,89,-171,-313,-120,194,255,58,-36,118,196,-67,-406,-406,-75,242,334,209,-53,-273,-240,-29,41,-43,66,375,374,-122,-520,-333,71,95,-168,-225,-7,173,206,233,264,172,-34,-245,-384,-354,-72,264,284,-33,-242,-81,156,117,-84,-140,-43,25,63,180,278,138,-209,-454,-368,-67,160,189,136,106,63,-18,-46,20,61,-11,-96,-83,-47,-101,-164,-52,204,320,129,-159,-220,-54,35,-86,-172,9,282,283,-9,-213,-70,178,127,-178,-302,-93,95,23,-68,35,131,57,42,190,145,-256,-520,-220,278,371,114,-30,35,46,-82,-201,-223,-147,34,207,179,13,10,188,183,-120,-316,-121,136,33,-266,-292,9,227,129,-19,94,328,288,-70,-330,-206,73,130,-34,-153,-132,-91,-70,-4,91,128,106,103,128,99,-20,-165,-218,-118,64,150,44,-129,-136,61,242,184,-60,-236,-180,29,200,218,110,-42,-174,-243,-212,-92,59,185,246,192,17,-153,-152,1,92,-18,-169,-101,166,302,105,-173,-148,100,111,-231,-454,-190,262,382,156,-32,-7,
            //59,24,-36,-29,-3,-44,-129,-156,-90
            5722,5342,3542,2919,3380,3189,2128,1294,884,272,-440,-638,-447,-490,-796,-893,-650,-317,-44,118,75,-130,-243,-205,-245,-484,-682,-676,-644,-755,-804,-579,-267,-199,-358,-436,-268,39,334,539,616,548,390,202,9,-153,-212,-138,17,203,396,514,456,277,198,323,516,622,656,657,546,293,84,104,272,331,196,35,40,194,330,360,354,367,358,312,294,273,141,-17,46,263,235,-118,-332,-128,123,56,-76,101,408,438,214,25,-71,-165,-170,14,202,143,-96,-263,-256,-160,-52,2,-51,-131,-56,182,344,268,102,31,9,-86,-212,-271,-276,-239,-72,201,358,252,42,-49,-15,27,72,183,292,233,-15,-250,-288,-208,-197,-210,-38,259,336,132,48,287,480,281,-66,-189,-163,-214,-230,-41,170,163,56,52,63,-45,-124,-7,158,152,46,5,-9,-83,-100,65,264,252,18,-265,-434,-385,-106,177,180,-20,-24,245,362,56,-300,-251,39,48,-312,-572,-362,133,434,292,-72,-246,-88,119,64,-152,-156,125,318,102,-314,-456,-204,60,-11,-253,-246,82,352,260,-14,-98,52,110,-101,-302,-156,197,290,5,-250,-168,31,28,-86,-45,105,132,38,14,94,143,102,9,-103,-180,-140,-27,-9,-117,-150,38,294,351,178,-15,-54,8,23,-44,-121,-179,-255,-296,-170,90,254,218,150,164,121,-77,-246,-204,-63,-11,-12,40,63,-58,-194,-118,127,244,72,-202,-276,-90,114,68,-191,-331,-136,180,243,49,-40,117,196,-65,-402,-398,-67,249,340,216,-45,-265,-232,-20,49,-38,70,382,384,-112,-510,-320,85,108,-156,-210,11,191,224,253,286,192,-18,-230,-368,-339,-58,278,298,-20,-230,-68,172,132,-70,-125,-27,41,80,197,294,153,-195,-439,-354,-57,166,194,140,106,59,-26,-58,5,45,-26,-112,-104,-70,-125,-186,-75,180,297,109,-178,-238,-69,23,-100,-186,-1,276,277,-15,-218,-74,173,121,-183,-306,-98,91,22,-66,37,131,58,46,195,151,-248,-510,-210,288,383,128,-17,47,59,-68,-187,-209,-132,48,219,189,25,24,200,193,-110,-306,-112,146,46,-253,-279,21,240,141,-12,98,333,294,-69,-334,-210,69,123,-46,-166,-144,-104,-84,-16,81,118,96,96,124,95,-24,-166,-216,-116,64,150,44,-131,-140,57,236,174,-72,-246,-190,16,186,206,100,-55,-190,-257,-222,-102,47,174,238,185,11,-157,-156,-4,86,-23,-173,-105,162,301,105,-174,-150,101,115,-226,-448,
            -180,276,397,172,-12,17,83,48,-10,-3,20,-22,-103,-132,-77,
        },
    }
};

static const int16_t POSSIBLY_UNUSED AncFirCoef_384k_north_1more_cooledit_1p3_0db[AUD_COEF_LEN] =
{
7517,1197,521,390,259,216,160,140,108,97,76,70,55,52,40,39,29,29,21,21,14,15,9,11,5,7,2,4,-1,1,-4,-1,-5,-3,-7,-4,-8,-6,-9,-7,-10,-8,-11,-8,-12,-9,-13,-9,-13,-10,-13,-10,-14,-11,-14,-11,-14,-11,-14,-11,-15,-11,-15,-11,-15,-12,-15,-12,-15,-12,-15,-12,-15,-12,-15,-12,-15,-11,-15,-11,-15,-11,-14,-11,-14,-11,-14,-11,-14,-11,-14,-11,-14,-11,-14,-11,-14,-11,-14,-10,-13,-10,-13,-10,-13,-10,-13,-10,-13,-10,-13,-10,-13,-10,-13,-10,-12,-9,-12,-9,-12,-9,-12,-9,-12,-9,-12,-9,-12,-9,-11,-9,-11,-8,-11,-8,-11,-8,-11,-8,-11,-8,-11,-8,-11,-8,-10,-8,-10,-7,-10,-7,-10,-7,-10,-7,-10,-7,-10,-7,-10,-7,-9,-7,-9,-7,-9,-7,-9,-6,-9,-6,-9,-6,-9,-6,-9,-6,-9,-6,-9,-6,-8,-6,-8,-6,-8,-6,-8,-6,-8,-5,-8,-5,-8,-5,-8,-5,-8,-5,-8,-5,-7,-5,-7,-5,-7,-5,-7,-5,-7,-5,-7,-5,-7,-5,-7,-4,-7,-4,-7,-4,-7,-4,-7,-4,-6,-4,-6,-4,-6,-4,-6,-4,-6,-4,-6,-4,-6,-4,-6,-4,-6,-4,-6,-4,-6,-4,-6,-4,-6,-3,-6,-3,-5,-3,-5,-3,-5,-3,-5,-3,-5,-3,-5,-3,-5,-3,-5,-3,-5,-3,-5,-3,-5,-3,-5,-3,-5,-3,-5,-3,-5,-3,-5,-3,-4,-3,-4,-3,-4,-3,-4,-3,-4,-3,-4,-3,-4,-2,-4,-2,-4,-2,-4,-2,-4,-2,-4,-2,-4,-2,-4,-2,-4,-2,-4,-2,-4,-2,-4,-2,-4,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,

//7517,1197,521,390,259,216,160,140,108,97,76,70,55,52,40,39,29,29,21,21,14,15,9,11,5,7,2,4,-1,1,-4,-1,-5,-3,-7,-4,-8,-6,-9,-7,-10,-8,-11,-8,-12,-9,-13,-9,-13,-10,-13,-10,-14,-11,-14,-11,-14,-11,-14,-11,-15,-11,-15,-11,-15,-12,-15,-12,-15,-12,-15,-12,-15,-12,-15,-12,-15,-11,-15,-11,-15,-11,-14,-11,-14,-11,-14,-11,-14,-11,-14,-11,-14,-11,-14,-11,-14,-11,-14,-10,-13,-10,-13,-10,-13,-10,-13,-10,-13,-10,-13,-10,-13,-10,-13,-10,-12,-9,-12,-9,-12,-9,-12,-9,-12,-9,-12,-9,-12,-9,-11,-9,-11,-8,-11,-8,-11,-8,-11,-8,-11,-8,-11,-8,-11,-8,-10,-8,-10,-7,-10,-7,-10,-7,-10,-7,-10,-7,-10,-7,-10,-7,-9,-7,-9,-7,-9,-7,-9,-6,-9,-6,-9,-6,-9,-6,-9,-6,-9,-6,-9,-6,-8,-6,-8,-6,-8,-6,-8,-6,-8,-5,-8,-5,-8,-5,-8,-5,-8,-5,-8,-5,-7,-5,-7,-5,-7,-5,-7,-5,-7,-5,-7,-5,-7,-5,-7,-4,-7,-4,-7,-4,-7,-4,-7,-4,-6,-4,-6,-4,-6,-4,-6,-4,-6,-4,-6,-4,-6,-4,-6,-4,-6,-4,-6,-4,-6,-4,-6,-4,-6,-3,-6,-3,-5,-3,-5,-3,-5,-3,-5,-3,-5,-3,-5,-3,-5,-3,-5,-3,-5,-3,-5,-3,-5,-3,-5,-3,-5,-3,-5,-3,-5,-3,-5,-3,-4,-3,-4,-3,-4,-3,-4,-3,-4,-3,-4,-3,-4,-2,-4,-2,-4,-2,-4,-2,-4,-2,-4,-2,-4,-2,-4,-2,-4,-2,-4,-2,-4,-2,-4,-2,-4,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-3,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,

};

static const int16_t AncFirCoef_96k_north_1more_cooledit_1p3_0db[AUD_COEF_LEN] =
{
10348,-1363,888,561,646,464,429,303,256,170,132,75,50,15,0,-20,-27,-38,-41,-47,-47,-49,-48,-48,-46,-45,-43,-41,-39,-37,-35,-33,-31,-29,-28,-26,-25,-23,-23,-21,-20,-18,-18,-16,-16,-15,-15,-13,-13,-12,-12,-11,-11,-9,-10,-8,-9,-8,-8,-7,-7,-6,-7,-5,-6,-4,-5,-4,-4,-3,-4,-2,-3,-2,-3,-1,-2,-1,-1,0,-1,0,0,1,0,1,1,2,1,2,1,3,2,3,2,3,2,4,3,4,3,4,3,5,4,5,4,5,4,5,4,6,5,6,5,6,5,6,5,6,5,7,6,7,6,7,6,7,6,7,6,7,6,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,8,8,8,9,8,9,8,9,8,9,8,9,8,9,8,9,8,9,8,9,8,9,8,9,9,9,9,9,9,10,9,10,9,10,9,10,9,10,9,10,9,10,9,10,9,10,9,10,9,10,9,10,9,10,9,10,9,10,9,10,9,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,9,10,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,8,9,8,9,8,9,8,9,8,9,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8
};

static const struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_384k_iir = {
    .anc_cfg_ff_l = {
        .gain = -50,
        .len = AUD_COEF_LEN,
        .coef =
        {
            15988,-761,-700,-639,-577,-517,-457,-399,-342,-287,-234,-183,-135,-89,-46,-6,32,66,98,127,153,176,197,215,230,242,252,260,266,269,270,270,268,264,259,253,245,236,227,216,205,194,181,169,157,144,131,118,106,93,81,70,58,47,37,27,17,8,0,-8,-15,-21,-27,-33,-38,-42,-45,-49,-51,-53,-55,-56,-56,-57,-57,-56,-55,-54,-53,-51,-50,-48,-45,-43,-41,-38,-36,-33,-30,-28,-25,-22,-20,-17,-15,-13,-10,-8,-6,-4,-2,0,1,3,4,5,6,7,8,9,10,10,10,11,11,11,11,11,11,11,11,10,10,10,9,9,8,8,7,7,6,6,5,5,4,3,3,2,2,1,1,1,0,0,-1,-1,-1,-1,-2,-2,-2,-2,-2,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-2,-2,-2,-2,-2,-2,-2,-2,-2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        },
    },
    .anc_cfg_ff_r = {
        .gain = -50,
        .len = AUD_COEF_LEN,
        .coef =
        {
            15988,-761,-700,-639,-577,-517,-457,-399,-342,-287,-234,-183,-135,-89,-46,-6,32,66,98,127,153,176,197,215,230,242,252,260,266,269,270,270,268,264,259,253,245,236,227,216,205,194,181,169,157,144,131,118,106,93,81,70,58,47,37,27,17,8,0,-8,-15,-21,-27,-33,-38,-42,-45,-49,-51,-53,-55,-56,-56,-57,-57,-56,-55,-54,-53,-51,-50,-48,-45,-43,-41,-38,-36,-33,-30,-28,-25,-22,-20,-17,-15,-13,-10,-8,-6,-4,-2,0,1,3,4,5,6,7,8,9,10,10,10,11,11,11,11,11,11,11,11,10,10,10,9,9,8,8,7,7,6,6,5,5,4,3,3,2,2,1,1,1,0,0,-1,-1,-1,-1,-2,-2,-2,-2,-2,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-2,-2,-2,-2,-2,-2,-2,-2,-2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        },
    }
};

static const struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_384k_cool_goer0 = {
    .anc_cfg_ff_l = {
        .gain = -79,
        .len = AUD_COEF_LEN,
        .coef =
        {
            8108,-163,-155,-147,-139,-131,-124,-117,-110,-104,-98,-92,-86,-81,-76,-70,-66,-61,-56,-52,-48,-44,-40,-37,-33,-30,-27,-24,-21,-18,-15,-13,-11,-8,-6,-4,-2,0,2,3,5,6,8,9,10,12,13,14,15,16,17,17,18,19,20,20,21,21,22,22,23,23,23,24,24,24,24,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,24,24,24,24,24,24,23,23,23,23,23,22,22,22,22,21,21,21,21,20,20,20,20,19,19,19,18,18,18,18,17,17,17,17,16,16,16,15,15,15,15,14,14,14,14,13,13,13,13,12,12,12,12,11,11,11,11,11,10,10,10,10,9,9,9,9,9,8,8,8,8,8,8,7,7,7,7,7,7,6,6,6,6,6,6,6,5,5,5,5,5,5,5,4,4,4,4,4,4,4,4,4,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
        },
    },
    .anc_cfg_ff_r = {
        .gain = -81,
        .len = AUD_COEF_LEN,
        .coef =
        {
            8108,-163,-155,-147,-139,-131,-124,-117,-110,-104,-98,-92,-86,-81,-76,-70,-66,-61,-56,-52,-48,-44,-40,-37,-33,-30,-27,-24,-21,-18,-15,-13,-11,-8,-6,-4,-2,0,2,3,5,6,8,9,10,12,13,14,15,16,17,17,18,19,20,20,21,21,22,22,23,23,23,24,24,24,24,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,24,24,24,24,24,24,23,23,23,23,23,22,22,22,22,21,21,21,21,20,20,20,20,19,19,19,18,18,18,18,17,17,17,17,16,16,16,15,15,15,15,14,14,14,14,13,13,13,13,12,12,12,12,11,11,11,11,11,10,10,10,10,9,9,9,9,9,8,8,8,8,8,8,7,7,7,7,7,7,6,6,6,6,6,6,6,5,5,5,5,5,5,5,4,4,4,4,4,4,4,4,4,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
        },
    }
};

static const struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_384k_cool_goer1 = {
    .anc_cfg_ff_l = {
        .gain = -79,
        .len = AUD_COEF_LEN,
        .coef =
        {
            4116,21,21,21,21,21,21,20,20,20,20,20,20,20,20,20,20,20,20,20,19,19,19,19,19,19,19,19,19,19,19,19,18,18,18,18,18,18,18,18,18,18,18,18,18,17,17,17,17,17,17,17,17,17,17,17,17,17,16,16,16,16,16,16,16,16,16,16,16,16,16,16,15,15,15,15,15,15,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
        },
    },
    .anc_cfg_ff_r = {
        .gain = -83,
        .len = AUD_COEF_LEN,
        .coef =
        {
            4116,21,21,21,21,21,21,20,20,20,20,20,20,20,20,20,20,20,20,20,19,19,19,19,19,19,19,19,19,19,19,19,18,18,18,18,18,18,18,18,18,18,18,18,18,17,17,17,17,17,17,17,17,17,17,17,17,17,16,16,16,16,16,16,16,16,16,16,16,16,16,16,15,15,15,15,15,15,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
        },
    }
};

static const struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_384k_cool_godlen_typec = {
    .anc_cfg_ff_l = {
        .gain = 32,
        .len = AUD_COEF_LEN,
        .coef =
        {
            7982,-209,-205,-199,-191,-181,-169,-155,-140,-124,-106,-88,-70,-51,-33,-14,3,20,35,50,63,75,85,94,101,106,110,113,114,114,113,111,108,104,100,96,91,87,82,77,73,69,65,61,58,55,53,50,48,47,45,44,43,42,41,40,39,38,37,36,34,33,31,29,27,25,23,21,19,17,15,13,11,9,7,6,4,3,2,0,-1,-2,-3,-4,-5,-5,-6,-7,-8,-9,-10,-11,-11,-12,-13,-14,-15,-16,-17,-17,-18,-19,-19,-20,-20,-21,-21,-21,-21,-21,-21,-21,-20,-20,-20,-20,-19,-19,-19,-19,-19,-19,-19,-19,-19,-19,-19,-20,-20,-20,-21,-21,-21,-21,-22,-22,-22,-22,-22,-22,-21,-21,-21,-20,-20,-20,-19,-18,-18,-17,-17,-16,-16,-15,-15,-14,-14,-13,-13,-13,-12,-12,-12,-12,-12,-11,-11,-11,-11,-11,-11,-11,-11,-10,-10,-10,-10,-10,-9,-9,-9,-8,-8,-7,-7,-7,-6,-6,-5,-5,-5,-4,-4,-3,-3,-3,-3,-2,-2,-2,-2,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,-1,-1,0,-1
        },
    },
    .anc_cfg_ff_r = {
        .gain = 30,
        .len = AUD_COEF_LEN,
        .coef =
        {
            7982,-209,-205,-199,-191,-181,-169,-155,-140,-124,-106,-88,-70,-51,-33,-14,3,20,35,50,63,75,85,94,101,106,110,113,114,114,113,111,108,104,100,96,91,87,82,77,73,69,65,61,58,55,53,50,48,47,45,44,43,42,41,40,39,38,37,36,34,33,31,29,27,25,23,21,19,17,15,13,11,9,7,6,4,3,2,0,-1,-2,-3,-4,-5,-5,-6,-7,-8,-9,-10,-11,-11,-12,-13,-14,-15,-16,-17,-17,-18,-19,-19,-20,-20,-21,-21,-21,-21,-21,-21,-21,-20,-20,-20,-20,-19,-19,-19,-19,-19,-19,-19,-19,-19,-19,-19,-20,-20,-20,-21,-21,-21,-21,-22,-22,-22,-22,-22,-22,-21,-21,-21,-20,-20,-20,-19,-18,-18,-17,-17,-16,-16,-15,-15,-14,-14,-13,-13,-13,-12,-12,-12,-12,-12,-11,-11,-11,-11,-11,-11,-11,-11,-10,-10,-10,-10,-10,-9,-9,-9,-8,-8,-7,-7,-7,-6,-6,-5,-5,-5,-4,-4,-3,-3,-3,-3,-2,-2,-2,-2,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,-1,-1,0,-1
        },
    }
};

static const struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_384k_cool_godlen_lighting = {
    .anc_cfg_ff_l = {
        .gain = 21,
        .len = AUD_COEF_LEN,
        .coef =
        {
            7865,-625,-567,-511,-459,-409,-362,-317,-275,-235,-198,-164,-131,-101,-73,-48,-24,-2,18,36,52,67,80,92,102,111,119,125,131,135,139,142,144,145,145,145,144,143,141,139,136,133,130,126,123,119,115,111,106,102,98,93,89,85,80,76,72,68,63,59,56,52,48,45,41,38,35,32,29,26,23,21,18,16,14,12,10,8,7,5,4,2,1,0,-1,-2,-3,-3,-4,-5,-5,-6,-6,-7,-7,-7,-7,-7,-8,-8,-8,-8,-8,-8,-7,-7,-7,-7,-7,-7,-7,-6,-6,-6,-6,-6,-5,-5,-5,-5,-5,-4,-4,-4,-4,-4,-3,-3,-3,-3,-3,-2,-2,-2,-2,-2,-2,-2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,
        },
    },
    .anc_cfg_ff_r = {
        .gain = 23,
        .len = AUD_COEF_LEN,
        .coef =
        {
            7865,-625,-567,-511,-459,-409,-362,-317,-275,-235,-198,-164,-131,-101,-73,-48,-24,-2,18,36,52,67,80,92,102,111,119,125,131,135,139,142,144,145,145,145,144,143,141,139,136,133,130,126,123,119,115,111,106,102,98,93,89,85,80,76,72,68,63,59,56,52,48,45,41,38,35,32,29,26,23,21,18,16,14,12,10,8,7,5,4,2,1,0,-1,-2,-3,-3,-4,-5,-5,-6,-6,-7,-7,-7,-7,-7,-8,-8,-8,-8,-8,-8,-7,-7,-7,-7,-7,-7,-7,-6,-6,-6,-6,-6,-5,-5,-5,-5,-5,-4,-4,-4,-4,-4,-3,-3,-3,-3,-3,-2,-2,-2,-2,-2,-2,-2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,
        },
    }
};
#endif

static void anc_cfg_lock(void)
{
}

static void anc_cfg_unlock(void)
{
}

#ifdef ARM_BES1000
static void damic_config(void)
{
	ANC_enble_p->Gpadc_unsigned = 1;
	Mute_Gain_coef_p->Cap_L = 0x0;
	Mute_Gain_coef_p->Cap_R = 0x0;
	Mute_Gain_coef_p->cap_gpadc = 0x1;
	Mute_Gain_coef_p->cap_gpadc_48k = 0x0;
}

static void anc_dac_reset_handler(enum HAL_CODEC_DAC_RESET_STAGE_T stage)
{
    int8_t gain[2];

#define DAC_RESET_FADE_STEP             1

    if (stage == HAL_CODEC_DAC_PRE_RESET) {
        chan_enabled[0] = ANC_enble_p->Anc_enable_ch0;
        chan_enabled[1] = ANC_enble_p->Anc_enable_ch1;
        if (chan_enabled[0] || chan_enabled[1]) {
            // Fade out
            cur_gain[0] = Mute_Gain_coef_p->Mute_gain_coef_ch0;
            cur_gain[1] = Mute_Gain_coef_p->Mute_gain_coef_ch1;
            gain[0] = cur_gain[0];
            gain[1] = cur_gain[1];
            while (gain[0] || gain[1]) {
                if (gain[0] > DAC_RESET_FADE_STEP) {
                    gain[0] -= DAC_RESET_FADE_STEP;
                } else if (gain[0] < -DAC_RESET_FADE_STEP) {
                    gain[0] += DAC_RESET_FADE_STEP;
                } else {
                    gain[0] = 0;
                }
                if (gain[1] > DAC_RESET_FADE_STEP) {
                    gain[1] -= DAC_RESET_FADE_STEP;
                } else if (gain[1] < -DAC_RESET_FADE_STEP) {
                    gain[1] += DAC_RESET_FADE_STEP;
                } else {
                    gain[1] = 0;
                }
                Mute_Gain_coef_p->Mute_gain_coef_ch0 = gain[0];
                Mute_Gain_coef_p->Mute_gain_coef_ch1 = gain[1];
                hal_sys_timer_delay(2);
            }
        }
    } else {
        if (chan_enabled[0] || chan_enabled[1]) {
            anc_select_coef(anc_get_current_coef_samplerate(),anc_get_current_coef_index(),ANC_FEEDFORWARD,ANC_GAIN_DELAY);
            anc_ctrl_reg_init();
            Mute_Gain_coef_p->Mute_gain_coef_ch0 = 0;
            Mute_Gain_coef_p->Mute_gain_coef_ch1 = 0;
            ANC_enble_p->Anc_enable_ch0 = chan_enabled[0];
            ANC_enble_p->Anc_enable_ch1 = chan_enabled[1];
            // Fade in
            gain[0] = 0;
            gain[1] = 0;
            while (gain[0] != cur_gain[0] || gain[1] != cur_gain[1]) {
                if (cur_gain[0] < 0 && gain[0] - DAC_RESET_FADE_STEP > cur_gain[0]) {
                    gain[0] -= DAC_RESET_FADE_STEP;
                } else if (cur_gain[0] > 0 && gain[0] + DAC_RESET_FADE_STEP < cur_gain[0]) {
                    gain[0] += DAC_RESET_FADE_STEP;
                } else {
                    gain[0] = cur_gain[0];
                }
                if (cur_gain[1] < 0 && gain[1] - DAC_RESET_FADE_STEP > cur_gain[1]) {
                    gain[1] -= DAC_RESET_FADE_STEP;
                } else if (cur_gain[1] > 0 && gain[1] + DAC_RESET_FADE_STEP < cur_gain[1]) {
                    gain[1] += DAC_RESET_FADE_STEP;
                } else {
                    gain[1] = cur_gain[0];
                }
                Mute_Gain_coef_p->Mute_gain_coef_ch0 = gain[0];
                Mute_Gain_coef_p->Mute_gain_coef_ch1 = gain[1];
                hal_sys_timer_delay(2);
            }
        }
    }
}

static void auxmic_dac_reset_handler(enum HAL_CODEC_DAC_RESET_STAGE_T stage)
{
    if (stage == HAL_CODEC_DAC_POST_RESET) {
        damic_config();
    }
}

static void dac_reset_handler(enum HAL_CODEC_DAC_RESET_STAGE_T stage)
{

    if (stage == HAL_CODEC_DAC_PRE_RESET) {
        anc_cfg_lock();
    }

    if (anc_dac_reset_fp) {
        anc_dac_reset_fp(stage);
    }

    if (auxmic_dac_reset_fp) {
        auxmic_dac_reset_fp(stage);
    }

    if (stage == HAL_CODEC_DAC_POST_RESET) {
        anc_cfg_unlock();
    }
}
#endif

void damic_init(void)
{
	//LOG_I("%s start.", __func__);

#ifdef ARM_BES1000
	hal_cmu_anc_enable(HAL_CMU_ANC_CLK_USER_AUXMIC);

	hal_codec_aux_mic_dma_enable(HAL_CODEC_ID_0);

    anc_cfg_lock();

    damic_config();
	Mute_Gain_coef_p->anc_sr_sel = 0x0;
	Mute_Gain_coef_p->anc_phase_sel = 0x0;
	Mute_Gain_coef_p->Dual_anc = 0x0;

    auxmic_dac_reset_fp = auxmic_dac_reset_handler;
    hal_codec_set_dac_reset_callback(dac_reset_handler);

    anc_cfg_unlock();
#endif


	//LOG_I("%s end: %x, %x.", __func__, *((uint32_t*)(0x4000a050)), *((uint32_t*)(0x40012010)));
}

void damic_deinit (void)
{
    //LOG_I("%s start.", __func__);

#ifdef ARM_BES1000
    anc_cfg_lock();

    auxmic_dac_reset_fp = NULL;
	hal_cmu_anc_disable(HAL_CMU_ANC_CLK_USER_AUXMIC);
	hal_codec_aux_mic_dma_disable(HAL_CODEC_ID_0);

    anc_cfg_unlock();
#endif

    //LOG_I("%s end.", __func__);
}


/**
 * @brief      Update coef and store gain,
 * 				fadein: enable: use anc_set_gain(), disable: anc_apply_max_gain() to set gain.
 *
 * @param[in]  cfg   coef and gain
 */
int anc_set_cfg(const struct_anc_cfg *cfg,enum ANC_TYPE_T anc_type,ANC_GAIN_TIME anc_gain_delay)
{
    const aud_item *item0;
    const aud_item *item1;

    if (cfg == NULL) {
        LOG_I("%s: cfg is null", __func__);
        return ANC_OTHER_ERR;
    }

    item0 = &(cfg->anc_cfg_ff_l);
    item1 = &(cfg->anc_cfg_ff_r);

    LOG_I("ch0:%d,ch1:%d",item0->total_gain,item1->total_gain);
    Mute_Gain_coef_p->Mute_gain_coef_ch0 = 0;
    Mute_Gain_coef_p->Mute_gain_coef_ch1 = 0;

	max_gain0=127;
	max_gain1=127;

	if(anc_gain_delay==ANC_GAIN_NO_DELAY)
	{
		Mute_Gain_coef_p->Mute_gain_coef_ch0 = max_gain0;
    	Mute_Gain_coef_p->Mute_gain_coef_ch1 = max_gain1;
	}

#ifdef ARM_BES1000
    analog_aud_apply_anc_adc_gain_offset(ANC_FEEDFORWARD, item0->adc_gain_offset, item1->adc_gain_offset);
#endif

    AncPduHardwarCh0(item0->fir_coef, AUD_COEF_LEN,item0->total_gain);
    AncPduHardwarCh1(item1->fir_coef, AUD_COEF_LEN,item1->total_gain);

	return ANC_NO_ERR;
}

#ifdef ARM_BES1000
static void anc_ctrl_reg_init(void)
{
	ANC_enble_p->Sel_gpadc_ch0=0;
	ANC_enble_p->Sel_gpadc_ch1=0;

	ANC_enble_p->Anc_mode_ch0=0;
	ANC_enble_p->Anc_mode_ch1=0;

	//ANC_enble_p->Gpadc_unsigned=1;

	// BBG: MainADC -> Memory
	Mute_Gain_coef_p->anc_sr_sel=0x0;
	//Mute_Gain_coef_p->anc_phase_sel=0x0;
	//Mute_Gain_coef_p->Dual_anc=0x0;
	//Mute_Gain_coef_p->Cap_L=0x0;
	//Mute_Gain_coef_p->Cap_R=0x0;
	//Mute_Gain_coef_p->cap_gpadc=0x0;
	//Mute_Gain_coef_p->cap_gpadc_48k=0x0;

	Fir_Gain_p->Pre_pdu_gain_ch0=0;
	Fir_Gain_p->Pre_pdu_gain_ch1=0;

	//AncPduHardwarCh0(AncFirCoef2, ANCCOEFLEN1);
	//AncPduHardwarCh1(AncFirCoef2, ANCCOEFLEN1);

//	AncPduHardwarCh0(AncFirCoef_ram, ANCCOEFLEN1);
//	AncPduHardwarCh1(AncFirCoef_ram, ANCCOEFLEN1);

	ANC_enble_p->Anc_enable_ch0=0;
	ANC_enble_p->Anc_enable_ch1=0;

/*

#if 0

#define MAGTITUED_ANC 16000

	AncFirCoef1[0]=MAGTITUED_ANC;

	for(int i=0;i<50;i++)
	{
		osDelay(1000);
		LOG_I("Delay:%d\n",i);
		AncPduHardwarCh0(AncFirCoef1, ANCCOEFLEN1);
		AncPduHardwarCh1(AncFirCoef1, ANCCOEFLEN1);
		AncFirCoef1[i]=0x0;
		AncFirCoef1[i+1]=MAGTITUED_ANC;
	}



#else

#define DELAY_ANC 0

	AncFirCoef1[DELAY_ANC]=16384;

	for(int i=0;i<100;i++)
	{
		osDelay(1000);
		LOG_I("AncFirCoef1[%d]:%d",i,AncFirCoef1[DELAY_ANC]);
		AncPduHardwarCh0(AncFirCoef1, ANCCOEFLEN1);
		AncPduHardwarCh1(AncFirCoef1, ANCCOEFLEN1);
		AncFirCoef1[DELAY_ANC]=(AncFirCoef1[DELAY_ANC]*(int)0x3ff0)>>14;


	}

#endif

	*/

}
#endif
void anc_disable_gain_updated_when_pass0(uint8_t on)
{

}
int anc_opened(enum ANC_TYPE_T anc_type)
{
    return open_flag;
}

int anc_open(enum ANC_TYPE_T anc_type)
{
#ifdef ARM_BES1000
#if defined(CHIP_BEST1000) && !defined(AUD_PLL_DOUBLE)
    hal_cmu_fir_high_speed_enable(HAL_CMU_FIR_USER_ANC);
#endif

    anc_cfg_lock();

    hal_sysfreq_req(HAL_SYSFREQ_USER_APP_7, HAL_CMU_FREQ_52M);

    anc_ctrl_reg_init();

    anc_dac_reset_fp = anc_dac_reset_handler;
    hal_codec_set_dac_reset_callback(dac_reset_handler);

    anc_cfg_unlock();
#endif

    open_flag = 1;

	return ANC_NO_ERR;
}

void anc_close(enum ANC_TYPE_T anc_type)
{
#ifdef ARM_BES1000
    anc_cfg_lock();

    anc_dac_reset_fp = NULL;

    hal_sysfreq_req(HAL_SYSFREQ_USER_APP_7, HAL_CMU_FREQ_32K);

    anc_cfg_unlock();

#if defined(CHIP_BEST1000) && !defined(AUD_PLL_DOUBLE)
    hal_cmu_fir_high_speed_disable(HAL_CMU_FIR_USER_ANC);
#endif
#endif

    open_flag = 0;
}

int anc_enable(void)
{
    anc_cfg_lock();

	ANC_enble_p->Anc_enable_ch0=1;
	ANC_enble_p->Anc_enable_ch1=1;

    anc_cfg_unlock();

	LOG_I("%s", __func__);

	return ANC_NO_ERR;

}

int anc_disable(void)
{
    anc_cfg_lock();

	ANC_enble_p->Anc_enable_ch0=0;
	ANC_enble_p->Anc_enable_ch1=0;

    anc_cfg_unlock();

	LOG_I("%s", __func__);

	return ANC_NO_ERR;

}


int anc_set_gain(int32_t gain_ch_l,int32_t gain_ch_r,enum ANC_TYPE_T anc_type)
{
    anc_cfg_lock();

    Mute_Gain_coef_p->Mute_gain_coef_ch0=gain_ch_l;
    Mute_Gain_coef_p->Mute_gain_coef_ch1=gain_ch_r;

    anc_cfg_unlock();

	return ANC_NO_ERR;

}

int anc_set_gain_f32(float gain_l, float gain_r, enum ANC_TYPE_T type)
{
    anc_set_gain((int32_t)(max_gain0 * gain_l), (int32_t)(max_gain1 * gain_r), type);

    return 0;
}

int anc_get_gain(int32_t *gain_ch_l,int32_t *gain_ch_r,enum ANC_TYPE_T anc_type)
{
    anc_cfg_lock();

    *gain_ch_l = Mute_Gain_coef_p->Mute_gain_coef_ch0;
    *gain_ch_r = Mute_Gain_coef_p->Mute_gain_coef_ch1;

    anc_cfg_unlock();

	return ANC_NO_ERR;

}

int anc_get_cfg_gain(int32_t *gain_ch_l,int32_t *gain_ch_r,enum ANC_TYPE_T anc_type)
{
    *gain_ch_l = max_gain0;
    *gain_ch_r = max_gain1;

	return ANC_NO_ERR;

}

void  anc_set_ch_map( int32_t ch_map )
{
    return;
}

int anc_set_switching_delay(ANC_SWITCHING_DELAY  anc_switching_delay,enum ANC_TYPE_T anc_type)
{
    return 0;
}

int anc_howling_set(ANC_HOWLING_WINDOW window, ANC_HOWLING_THRESHOLD threshold)
{
    return 0;
}
int anc_adc_data_select(ANC_ADC_DATA data_select)
{
    return 0;
}

