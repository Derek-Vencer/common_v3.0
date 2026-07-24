/***************************************************************************
 *
 * Copyright 2015-2024 BES.
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
#ifndef __REG_CODEC_BEST1503P_H__
#define __REG_CODEC_BEST1503P_H__

#include "plat_types.h"

struct CODEC_T {
    __IO uint32_t REG_000;
    __IO uint32_t REG_004;
    __IO uint32_t REG_008;
    __IO uint32_t REG_00C;
    __IO uint32_t REG_010;
    __IO uint32_t REG_014;
    __IO uint32_t REG_018;
    __IO uint32_t REG_01C;
    __IO uint32_t REG_020;
    __IO uint32_t REG_024;
    __IO uint32_t REG_028;
    __IO uint32_t REG_02C;
    __IO uint32_t REG_030;
    __IO uint32_t REG_034;
    __IO uint32_t REG_038;
    __IO uint32_t REG_03C;
    __IO uint32_t REG_040;
    __IO uint32_t REG_044;
    __IO uint32_t REG_048;
    __IO uint32_t REG_04C;
    __IO uint32_t REG_050;
    __IO uint32_t REG_054;
    __IO uint32_t REG_058;
    __IO uint32_t REG_05C;
    __IO uint32_t REG_060;
    __IO uint32_t REG_064;
    __IO uint32_t REG_068;
    __IO uint32_t REG_06C;
    __IO uint32_t REG_070;
    __IO uint32_t REG_074;
    __IO uint32_t REG_078;
    __IO uint32_t REG_07C;
    __IO uint32_t REG_080;
    __IO uint32_t REG_084;
    __IO uint32_t REG_088;
    __IO uint32_t REG_08C;
    __IO uint32_t REG_090;
    __IO uint32_t REG_094;
    __IO uint32_t REG_098;
    __IO uint32_t REG_09C;
    __IO uint32_t REG_0A0;
    __IO uint32_t REG_0A4;
    __IO uint32_t REG_0A8;
    __IO uint32_t REG_0AC;
    __IO uint32_t REG_0B0;
    __IO uint32_t REG_0B4;
    __IO uint32_t REG_0B8;
    __IO uint32_t REG_0BC;
    __IO uint32_t REG_0C0;
    __IO uint32_t REG_0C4;
    __IO uint32_t REG_0C8;
    __IO uint32_t REG_0CC;
    __IO uint32_t REG_0D0;
    __IO uint32_t REG_0D4;
    __IO uint32_t REG_0D8;
    __IO uint32_t REG_0DC;
    __IO uint32_t REG_0E0;
    __IO uint32_t REG_0E4;
    __IO uint32_t REG_0E8;
    __IO uint32_t REG_0EC;
    __IO uint32_t REG_0F0;
    __IO uint32_t REG_0F4;
    __IO uint32_t REG_0F8;
    __IO uint32_t REG_0FC;
    __IO uint32_t REG_100;
    __IO uint32_t REG_104;
    __IO uint32_t REG_108;
    __IO uint32_t REG_10C;
    __IO uint32_t REG_110;
    __IO uint32_t REG_114;
    __IO uint32_t REG_118;
    __IO uint32_t REG_11C;
    __IO uint32_t REG_120;
    __IO uint32_t REG_124;
    __IO uint32_t REG_128;
    __IO uint32_t REG_12C;
    __IO uint32_t REG_130;
    __IO uint32_t REG_134;
    __IO uint32_t REG_138;
    __IO uint32_t REG_13C;
    __IO uint32_t REG_140;
    __IO uint32_t REG_144;
    __IO uint32_t REG_148;
    __IO uint32_t REG_14C;
    __IO uint32_t REG_150;
    __IO uint32_t REG_154;
    __IO uint32_t REG_158;
    __IO uint32_t REG_15C;
    __IO uint32_t REG_160;
    __IO uint32_t REG_164;
    __IO uint32_t REG_168;
    __IO uint32_t REG_16C;
    __IO uint32_t REG_170;
    __IO uint32_t REG_174;
    __IO uint32_t REG_178;
    __IO uint32_t REG_17C;
    __IO uint32_t REG_180;
    __IO uint32_t REG_184;
    __IO uint32_t REG_188;
    __IO uint32_t REG_18C;
    __IO uint32_t REG_190;
    __IO uint32_t REG_194;
    __IO uint32_t REG_198;
    __IO uint32_t REG_19C;
    __IO uint32_t REG_1A0;
    __IO uint32_t REG_1A4;
    __IO uint32_t REG_1A8;
    __IO uint32_t REG_1AC;
    __IO uint32_t REG_1B0;
    __IO uint32_t REG_1B4;
    __IO uint32_t REG_1B8;
    __IO uint32_t REG_1BC;
    __IO uint32_t REG_1C0;
    __IO uint32_t REG_1C4;
    __IO uint32_t REG_1C8;
    __IO uint32_t REG_1CC;
    __IO uint32_t REG_1D0;
    __IO uint32_t REG_1D4;
    __IO uint32_t REG_1D8;
    __IO uint32_t REG_1DC;
    __IO uint32_t REG_1E0;
    __IO uint32_t REG_1E4;
    __IO uint32_t REG_1E8;
    __IO uint32_t REG_1EC;
    __IO uint32_t REG_1F0;
    __IO uint32_t REG_1F4;
    __IO uint32_t REG_1F8;
    __IO uint32_t REG_1FC;
    __IO uint32_t REG_200;
    __IO uint32_t REG_204;
    __IO uint32_t REG_208;
    __IO uint32_t REG_20C;
    __IO uint32_t REG_210;
    __IO uint32_t REG_214;
    __IO uint32_t REG_218;
    __IO uint32_t REG_21C;
    __IO uint32_t REG_220;
    __IO uint32_t REG_224;
    __IO uint32_t REG_228;
    __IO uint32_t REG_22C;
    __IO uint32_t REG_230;
    __IO uint32_t REG_234;
    __IO uint32_t REG_238;
    __IO uint32_t REG_23C;
    __IO uint32_t REG_240;
    __IO uint32_t REG_244;
    __IO uint32_t REG_248;
    __IO uint32_t REG_24C;
    __IO uint32_t REG_250;
    __IO uint32_t REG_254;
    __IO uint32_t REG_258;
    __IO uint32_t REG_25C;
    __IO uint32_t REG_260;
    __IO uint32_t REG_264;
    __IO uint32_t REG_268;
    __IO uint32_t REG_26C;
    __IO uint32_t REG_270;
    __IO uint32_t REG_274;
    __IO uint32_t REG_278;
    __IO uint32_t REG_27C;
    __IO uint32_t REG_280;
    __IO uint32_t REG_284;
    __IO uint32_t REG_288;
    __IO uint32_t REG_28C;
    __IO uint32_t REG_290;
    __IO uint32_t REG_294;
    __IO uint32_t REG_298;
    __IO uint32_t REG_29C;
    __IO uint32_t REG_2A0;
    __IO uint32_t REG_2A4;
    __IO uint32_t REG_2A8;
    __IO uint32_t REG_2AC;
    __IO uint32_t REG_2B0;
    __IO uint32_t REG_2B4;
    __IO uint32_t REG_2B8;
    __IO uint32_t REG_2BC;
    __IO uint32_t REG_2C0;
    __IO uint32_t REG_2C4;
    __IO uint32_t REG_2C8;
    __IO uint32_t REG_2CC;
    __IO uint32_t REG_2D0;
    __IO uint32_t REG_2D4;
    __IO uint32_t REG_2D8;
    __IO uint32_t REG_2DC;
    __IO uint32_t REG_2E0;
    __IO uint32_t REG_2E4;
    __IO uint32_t REG_2E8;
    __IO uint32_t REG_2EC;
    __IO uint32_t REG_2F0;
    __IO uint32_t REG_2F4;
    __IO uint32_t REG_2F8;
    __IO uint32_t REG_2FC;
    __IO uint32_t REG_300;
    __IO uint32_t REG_304;
    __IO uint32_t REG_308;
    __IO uint32_t REG_30C;
    __IO uint32_t REG_310;
    __IO uint32_t REG_314;
    __IO uint32_t REG_318;
    __IO uint32_t REG_31C;
    __IO uint32_t REG_320;
    __IO uint32_t REG_324;
    __IO uint32_t REG_328;
    __IO uint32_t REG_32C;
    __IO uint32_t REG_330;
    __IO uint32_t REG_334;
    __IO uint32_t REG_338;
    __IO uint32_t REG_33C;
    __IO uint32_t REG_340;
    __IO uint32_t REG_344;
    __IO uint32_t REG_348;
    __IO uint32_t REG_34C;
    __IO uint32_t REG_350;
    __IO uint32_t REG_354;
    __IO uint32_t REG_358;
    __IO uint32_t REG_35C;
    __IO uint32_t REG_360;
    __IO uint32_t REG_364;
    __IO uint32_t REG_368;
    __IO uint32_t REG_36C;
    __IO uint32_t REG_370;
    __IO uint32_t REG_374;
    __IO uint32_t REG_378;
    __IO uint32_t REG_37C;
    __IO uint32_t REG_380;
    __IO uint32_t REG_384;
    __IO uint32_t REG_388;
    __IO uint32_t REG_38C;
    __IO uint32_t REG_390;
    __IO uint32_t REG_394;
    __IO uint32_t REG_398;
    __IO uint32_t REG_39C;
    __IO uint32_t REG_3A0;
    __IO uint32_t REG_3A4;
    __IO uint32_t REG_3A8;
    __IO uint32_t REG_3AC;
    __IO uint32_t REG_3B0;
    __IO uint32_t REG_3B4;
    __IO uint32_t REG_3B8;
    __IO uint32_t REG_3BC;
    __IO uint32_t REG_3C0;
    __IO uint32_t REG_3C4;
    __IO uint32_t REG_3C8;
    __IO uint32_t REG_3CC;
    __IO uint32_t REG_3D0;
    __IO uint32_t REG_3D4;
    __IO uint32_t REG_3D8;
    __IO uint32_t REG_3DC;
    __IO uint32_t REG_3E0;
    __IO uint32_t REG_3E4;
    __IO uint32_t REG_3E8;
    __IO uint32_t REG_3EC;
    __IO uint32_t REG_3F0;
    __IO uint32_t REG_3F4;
    __IO uint32_t REG_3F8;
    __IO uint32_t REG_3FC;
    __IO uint32_t REG_400;
    __IO uint32_t REG_404;
    __IO uint32_t REG_408;
    __IO uint32_t REG_40C;
    __IO uint32_t REG_410;
    __IO uint32_t REG_414;
    __IO uint32_t REG_418;
    __IO uint32_t REG_41C;
    __IO uint32_t REG_420;
    __IO uint32_t REG_424;
    __IO uint32_t REG_428;
    __IO uint32_t REG_42C;
    __IO uint32_t REG_430;
    __IO uint32_t REG_434;
    __IO uint32_t REG_438;
    __IO uint32_t REG_43C;
    __IO uint32_t REG_440;
    __IO uint32_t REG_444;
    __IO uint32_t REG_448;
    __IO uint32_t REG_44C;
    __IO uint32_t REG_450;
    __IO uint32_t REG_454;
    __IO uint32_t REG_458;
    __IO uint32_t REG_45C;
    __IO uint32_t REG_460;
    __IO uint32_t REG_464;
    __IO uint32_t REG_468;
    __IO uint32_t REG_46C;
    __IO uint32_t REG_470;
    __IO uint32_t REG_474;
    __IO uint32_t REG_478;
    __IO uint32_t REG_47C;
    __IO uint32_t REG_480;
    __IO uint32_t REG_484;
    __IO uint32_t REG_488;
    __IO uint32_t REG_48C;
    __IO uint32_t REG_490;
    __IO uint32_t REG_494;
    __IO uint32_t REG_498;
    __IO uint32_t REG_49C;
    __IO uint32_t REG_4A0;
    __IO uint32_t REG_4A4;
    __IO uint32_t REG_4A8;
    __IO uint32_t REG_4AC;
    __IO uint32_t REG_4B0;
    __IO uint32_t REG_4B4;
    __IO uint32_t REG_4B8;
    __IO uint32_t REG_4BC;
    __IO uint32_t REG_4C0;
    __IO uint32_t REG_4C4;
    __IO uint32_t REG_4C8;
    __IO uint32_t REG_4CC;
    __IO uint32_t REG_4D0;
    __IO uint32_t REG_4D4;
    __IO uint32_t REG_4D8;
    __IO uint32_t REG_4DC;
    __IO uint32_t REG_4E0;
    __IO uint32_t REG_4E4;
    __IO uint32_t REG_4E8;
    __IO uint32_t REG_4EC;
    __IO uint32_t REG_4F0;
    __IO uint32_t REG_4F4;
    __IO uint32_t REG_4F8;
    __IO uint32_t REG_4FC;
    __IO uint32_t REG_500;
    __IO uint32_t REG_504;
    __IO uint32_t REG_508;
    __IO uint32_t REG_50C;
    __IO uint32_t REG_510;
    __IO uint32_t REG_514;
    __IO uint32_t REG_518;
    __IO uint32_t REG_51C;
    __IO uint32_t REG_520;
    __IO uint32_t REG_524;
    __IO uint32_t REG_528;
    __IO uint32_t REG_52C;
    __IO uint32_t REG_530;
    __IO uint32_t REG_534;
    __IO uint32_t REG_538;
    __IO uint32_t REG_53C;
    __IO uint32_t REG_540;
    __IO uint32_t REG_544;
    __IO uint32_t REG_548;
    __IO uint32_t REG_54C;
    __IO uint32_t REG_550;
    __IO uint32_t REG_554;
    __IO uint32_t REG_558;
    __IO uint32_t REG_55C;
    __IO uint32_t REG_560;
    __IO uint32_t REG_564;
    __IO uint32_t REG_568;
    __IO uint32_t REG_56C;
    __IO uint32_t REG_570;
    __IO uint32_t REG_574;
    __IO uint32_t REG_578;
    __IO uint32_t REG_57C;
    __IO uint32_t REG_580;
    __IO uint32_t REG_584;
    __IO uint32_t REG_588;
    __IO uint32_t REG_58C;
    __IO uint32_t REG_590;
    __IO uint32_t REG_594;
    __IO uint32_t REG_598;
    __IO uint32_t REG_59C;
    __IO uint32_t REG_5A0;
    __IO uint32_t REG_5A4;
    __IO uint32_t REG_5A8;
    __IO uint32_t REG_5AC;
    __IO uint32_t REG_5B0;
    __IO uint32_t REG_5B4;
    __IO uint32_t REG_5B8;
    __IO uint32_t REG_5BC;
    __IO uint32_t REG_5C0;
    __IO uint32_t REG_5C4;
    __IO uint32_t REG_5C8;
    __IO uint32_t REG_5CC;
    __IO uint32_t REG_5D0;
    __IO uint32_t REG_5D4;
    __IO uint32_t REG_5D8;
    __IO uint32_t REG_5DC;
    __IO uint32_t REG_5E0;
    __IO uint32_t REG_5E4;
    __IO uint32_t REG_5E8;
    __IO uint32_t REG_5EC;
    __IO uint32_t REG_5F0;
    __IO uint32_t REG_5F4;
    __IO uint32_t REG_5F8;
    __IO uint32_t REG_5FC;
    __IO uint32_t REG_600;
    __IO uint32_t REG_604;
    __IO uint32_t REG_608;
    __IO uint32_t REG_60C;
    __IO uint32_t REG_610;
    __IO uint32_t REG_614;
    __IO uint32_t REG_618;
    __IO uint32_t REG_61C;
    __IO uint32_t REG_620;
    __IO uint32_t REG_624;
    __IO uint32_t REG_628;
    __IO uint32_t REG_62C;
    __IO uint32_t REG_630;
    __IO uint32_t REG_634;
    __IO uint32_t REG_638;
    __IO uint32_t REG_63C;
    __IO uint32_t REG_640;
    __IO uint32_t REG_644;
    __IO uint32_t REG_648;
    __IO uint32_t REG_64C;
    __IO uint32_t REG_650;
    __IO uint32_t REG_654;
    __IO uint32_t REG_658;
    __IO uint32_t REG_65C;
    __IO uint32_t REG_660;
    __IO uint32_t REG_664;
    __IO uint32_t REG_668;
    __IO uint32_t REG_66C;
    __IO uint32_t REG_670;
    __IO uint32_t REG_674;
    __IO uint32_t REG_678;
    __IO uint32_t REG_67C;
    __IO uint32_t REG_680;
    __IO uint32_t REG_684;
    __IO uint32_t REG_688;
    __IO uint32_t REG_68C;
    __IO uint32_t REG_690;
    __IO uint32_t REG_694;
    __IO uint32_t REG_698;
    __IO uint32_t REG_69C;
    __IO uint32_t REG_6A0;
    __IO uint32_t REG_6A4;
    __IO uint32_t REG_6A8;
    __IO uint32_t REG_6AC;
    __IO uint32_t REG_6B0;
    __IO uint32_t REG_6B4;
    __IO uint32_t REG_6B8;
    __IO uint32_t REG_6BC;
    __IO uint32_t REG_6C0;
    __IO uint32_t REG_6C4;
    __IO uint32_t REG_6C8;
    __IO uint32_t REG_6CC;
    __IO uint32_t REG_6D0;
    __IO uint32_t REG_6D4;
    __IO uint32_t REG_6D8;
    __IO uint32_t REG_6DC;
    __IO uint32_t REG_6E0;
    __IO uint32_t REG_6E4;
    __IO uint32_t REG_6E8;
    __IO uint32_t REG_6EC;
    __IO uint32_t REG_6F0;
    __IO uint32_t REG_6F4;
    __IO uint32_t REG_6F8;
    __IO uint32_t REG_6FC;
    __IO uint32_t REG_700;
    __IO uint32_t REG_704;
    __IO uint32_t REG_708;
    __IO uint32_t REG_70C;
    __IO uint32_t REG_710;
    __IO uint32_t REG_714;
    __IO uint32_t REG_718;
    __IO uint32_t REG_71C;
    __IO uint32_t REG_720;
    __IO uint32_t REG_724;
    __IO uint32_t REG_728;
    __IO uint32_t REG_72C;
    __IO uint32_t REG_730;
    __IO uint32_t REG_734;
    __IO uint32_t REG_738;
    __IO uint32_t REG_73C;
    __IO uint32_t REG_740;
    __IO uint32_t REG_744;
    __IO uint32_t REG_748;
    __IO uint32_t REG_74C;
    __IO uint32_t REG_750;
    __IO uint32_t REG_754;
    __IO uint32_t REG_758;
    __IO uint32_t REG_75C;
    __IO uint32_t REG_760;
    __IO uint32_t REG_764;
    __IO uint32_t REG_768;
    __IO uint32_t REG_76C;
    __IO uint32_t REG_770;
    __IO uint32_t REG_774;
    __IO uint32_t REG_778;
    __IO uint32_t REG_77C;
    __IO uint32_t REG_780;
    __IO uint32_t REG_784;
    __IO uint32_t REG_788;
    __IO uint32_t REG_78C;
    __IO uint32_t REG_790;
    __IO uint32_t REG_794;
    __IO uint32_t REG_798;
    __IO uint32_t REG_79C;
    __IO uint32_t REG_7A0;
    __IO uint32_t REG_7A4;
};

// reg_00
#define CODEC_CODEC_IF_EN                                   (1 << 0)
#define CODEC_ADC_ENABLE                                    (1 << 1)
#define CODEC_ADC_ENABLE_CH0                                (1 << 2)
#define CODEC_ADC_ENABLE_CH1                                (1 << 3)
#define CODEC_ADC_ENABLE_CH2                                (1 << 4)
#define CODEC_ADC_ENABLE_CH3                                (1 << 5)
#define CODEC_DAC_ENABLE                                    (1 << 6)
#define CODEC_DAC_ENABLE_SND                                (1 << 7)
#define CODEC_DMACTRL_RX                                    (1 << 8)
#define CODEC_DMACTRL_TX                                    (1 << 9)
#define CODEC_DMACTRL_TX_SND                                (1 << 10)

// reg_04
#define CODEC_RX_FIFO_FLUSH_CH0                             (1 << 0)
#define CODEC_RX_FIFO_FLUSH_CH1                             (1 << 1)
#define CODEC_RX_FIFO_FLUSH_CH2                             (1 << 2)
#define CODEC_RX_FIFO_FLUSH_CH3                             (1 << 3)
#define CODEC_TX_FIFO_FLUSH                                 (1 << 4)
#define CODEC_TX_FIFO_FLUSH_SND                             (1 << 5)
#define CODEC_MC_FIFO_FLUSH                                 (1 << 6)

// reg_08
#define CODEC_CODEC_RX_THRESHOLD(n)                         (((n) & 0xF) << 0)
#define CODEC_CODEC_RX_THRESHOLD_MASK                       (0xF << 0)
#define CODEC_CODEC_RX_THRESHOLD_SHIFT                      (0)
#define CODEC_CODEC_TX_THRESHOLD(n)                         (((n) & 0xF) << 4)
#define CODEC_CODEC_TX_THRESHOLD_MASK                       (0xF << 4)
#define CODEC_CODEC_TX_THRESHOLD_SHIFT                      (4)
#define CODEC_CODEC_TX_THRESHOLD_SND(n)                     (((n) & 0xF) << 8)
#define CODEC_CODEC_TX_THRESHOLD_SND_MASK                   (0xF << 8)
#define CODEC_CODEC_TX_THRESHOLD_SND_SHIFT                  (8)
#define CODEC_MC_THRESHOLD(n)                               (((n) & 0xF) << 12)
#define CODEC_MC_THRESHOLD_MASK                             (0xF << 12)
#define CODEC_MC_THRESHOLD_SHIFT                            (12)

// reg_0c
#define CODEC_CODEC_RX_OVERFLOW(n)                          (((n) & 0xF) << 0)
#define CODEC_CODEC_RX_OVERFLOW_MASK                        (0xF << 0)
#define CODEC_CODEC_RX_OVERFLOW_SHIFT                       (0)
#define CODEC_CODEC_RX_UNDERFLOW(n)                         (((n) & 0xF) << 4)
#define CODEC_CODEC_RX_UNDERFLOW_MASK                       (0xF << 4)
#define CODEC_CODEC_RX_UNDERFLOW_SHIFT                      (4)
#define CODEC_CODEC_TX_OVERFLOW                             (1 << 8)
#define CODEC_CODEC_TX_UNDERFLOW                            (1 << 9)
#define CODEC_CODEC_TX_OVERFLOW_SND                         (1 << 10)
#define CODEC_CODEC_TX_UNDERFLOW_SND                        (1 << 11)
#define CODEC_MC_OVERFLOW                                   (1 << 12)
#define CODEC_MC_UNDERFLOW                                  (1 << 13)
#define CODEC_EVENT_TRIGGER                                 (1 << 14)
#define CODEC_FB_CHECK_ERROR_TRIG_CH0                       (1 << 15)
#define CODEC_ADC_MAX_OVERFLOW                              (1 << 16)
#define CODEC_TIME_TRIGGER                                  (1 << 17)
#define CODEC_ADC_FIFO_OVERFLOW(n)                          (((n) & 0x7) << 18)
#define CODEC_ADC_FIFO_OVERFLOW_MASK                        (0x7 << 18)
#define CODEC_ADC_FIFO_OVERFLOW_SHIFT                       (18)
#define CODEC_ADC_FIFO_UNDERFLOW(n)                         (((n) & 0x7) << 21)
#define CODEC_ADC_FIFO_UNDERFLOW_MASK                       (0x7 << 21)
#define CODEC_ADC_FIFO_UNDERFLOW_SHIFT                      (21)

// reg_10
#define CODEC_CODEC_RX_OVERFLOW_MSK(n)                      (((n) & 0xF) << 0)
#define CODEC_CODEC_RX_OVERFLOW_MSK_MASK                    (0xF << 0)
#define CODEC_CODEC_RX_OVERFLOW_MSK_SHIFT                   (0)
#define CODEC_CODEC_RX_UNDERFLOW_MSK(n)                     (((n) & 0xF) << 4)
#define CODEC_CODEC_RX_UNDERFLOW_MSK_MASK                   (0xF << 4)
#define CODEC_CODEC_RX_UNDERFLOW_MSK_SHIFT                  (4)
#define CODEC_CODEC_TX_OVERFLOW_MSK                         (1 << 8)
#define CODEC_CODEC_TX_UNDERFLOW_MSK                        (1 << 9)
#define CODEC_CODEC_TX_OVERFLOW_SND_MSK                     (1 << 10)
#define CODEC_CODEC_TX_UNDERFLOW_SND_MSK                    (1 << 11)
#define CODEC_MC_OVERFLOW_MSK                               (1 << 12)
#define CODEC_MC_UNDERFLOW_MSK                              (1 << 13)
#define CODEC_EVENT_TRIGGER_MSK                             (1 << 14)
#define CODEC_FB_CHECK_ERROR_TRIG_CH0_MSK                   (1 << 15)
#define CODEC_ADC_MAX_OVERFLOW_MSK                          (1 << 16)
#define CODEC_TIME_TRIGGER_MSK                              (1 << 17)
#define CODEC_ADC_FIFO_OVERFLOW_MSK(n)                      (((n) & 0x7) << 18)
#define CODEC_ADC_FIFO_OVERFLOW_MSK_MASK                    (0x7 << 18)
#define CODEC_ADC_FIFO_OVERFLOW_MSK_SHIFT                   (18)
#define CODEC_ADC_FIFO_UNDERFLOW_MSK(n)                     (((n) & 0x7) << 21)
#define CODEC_ADC_FIFO_UNDERFLOW_MSK_MASK                   (0x7 << 21)
#define CODEC_ADC_FIFO_UNDERFLOW_MSK_SHIFT                  (21)

// reg_14
#define CODEC_FIFO_COUNT_CH0(n)                             (((n) & 0xF) << 0)
#define CODEC_FIFO_COUNT_CH0_MASK                           (0xF << 0)
#define CODEC_FIFO_COUNT_CH0_SHIFT                          (0)
#define CODEC_FIFO_COUNT_CH1(n)                             (((n) & 0xF) << 4)
#define CODEC_FIFO_COUNT_CH1_MASK                           (0xF << 4)
#define CODEC_FIFO_COUNT_CH1_SHIFT                          (4)
#define CODEC_FIFO_COUNT_CH2(n)                             (((n) & 0xF) << 8)
#define CODEC_FIFO_COUNT_CH2_MASK                           (0xF << 8)
#define CODEC_FIFO_COUNT_CH2_SHIFT                          (8)
#define CODEC_FIFO_COUNT_CH3(n)                             (((n) & 0xF) << 12)
#define CODEC_FIFO_COUNT_CH3_MASK                           (0xF << 12)
#define CODEC_FIFO_COUNT_CH3_SHIFT                          (12)

// reg_18
#define CODEC_FIFO_COUNT_TX(n)                              (((n) & 0xF) << 0)
#define CODEC_FIFO_COUNT_TX_MASK                            (0xF << 0)
#define CODEC_FIFO_COUNT_TX_SHIFT                           (0)
#define CODEC_FIFO_COUNT_TX_SND(n)                          (((n) & 0xF) << 4)
#define CODEC_FIFO_COUNT_TX_SND_MASK                        (0xF << 4)
#define CODEC_FIFO_COUNT_TX_SND_SHIFT                       (4)
#define CODEC_STATE_RX_CH(n)                                (((n) & 0x1FF) << 8)
#define CODEC_STATE_RX_CH_MASK                              (0x1FF << 8)
#define CODEC_STATE_RX_CH_SHIFT                             (8)
#define CODEC_MC_FIFO_COUNT(n)                              (((n) & 0xF) << 17)
#define CODEC_MC_FIFO_COUNT_MASK                            (0xF << 17)
#define CODEC_MC_FIFO_COUNT_SHIFT                           (17)

// reg_1c
#define CODEC_RX_FIFO_DATA(n)                               (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_MASK                             (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SHIFT                            (0)

// reg_20
#define CODEC_RX_FIFO_DATA(n)                               (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_MASK                             (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SHIFT                            (0)

// reg_24
#define CODEC_RX_FIFO_DATA(n)                               (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_MASK                             (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SHIFT                            (0)

// reg_28
#define CODEC_RX_FIFO_DATA(n)                               (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_MASK                             (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SHIFT                            (0)

// reg_2c
#define CODEC_RX_FIFO_DATA(n)                               (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_MASK                             (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SHIFT                            (0)

// reg_38
#define CODEC_MC_FIFO_DATA(n)                               (((n) & 0xFFFFFFFF) << 0)
#define CODEC_MC_FIFO_DATA_MASK                             (0xFFFFFFFF << 0)
#define CODEC_MC_FIFO_DATA_SHIFT                            (0)

// reg_3c
#define CODEC_TX_FIFO_DATA_SND(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_TX_FIFO_DATA_SND_MASK                         (0xFFFFFFFF << 0)
#define CODEC_TX_FIFO_DATA_SND_SHIFT                        (0)

// reg_40
#define CODEC_MODE_16BIT_ADC                                (1 << 0)
#define CODEC_MODE_24BIT_ADC                                (1 << 1)
#define CODEC_MODE_32BIT_ADC                                (1 << 2)

// reg_44
#define CODEC_DUAL_CHANNEL_DAC                              (1 << 0)
#define CODEC_DAC_EXCHANGE_L_R                              (1 << 1)
#define CODEC_MODE_16BIT_DAC                                (1 << 2)
#define CODEC_MODE_32BIT_DAC                                (1 << 3)
#define CODEC_DUAL_CHANNEL_DAC_SND                          (1 << 4)
#define CODEC_DAC_EXCHANGE_L_R_SND                          (1 << 5)
#define CODEC_MODE_16BIT_DAC_SND                            (1 << 6)
#define CODEC_MODE_32BIT_DAC_SND                            (1 << 7)

// reg_4c
#define CODEC_MC_ENABLE                                     (1 << 0)
#define CODEC_DUAL_CHANNEL_MC                               (1 << 1)
#define CODEC_MODE_16BIT_MC                                 (1 << 2)
#define CODEC_DMACTRL_MC                                    (1 << 3)
#define CODEC_MC_DELAY(n)                                   (((n) & 0xFF) << 4)
#define CODEC_MC_DELAY_MASK                                 (0xFF << 4)
#define CODEC_MC_DELAY_SHIFT                                (4)
#define CODEC_MC_RATE_SEL                                   (1 << 12)
#define CODEC_MODE_32BIT_MC                                 (1 << 13)
#define CODEC_MC_EN_SEL                                     (1 << 14)
#define CODEC_MC_RATE_SRC_SEL                               (1 << 15)
#define CODEC_CODEC_DAC_ENABLE_SND_SEL(n)                   (((n) & 0x3) << 16)
#define CODEC_CODEC_DAC_ENABLE_SND_SEL_MASK                 (0x3 << 16)
#define CODEC_CODEC_DAC_ENABLE_SND_SEL_SHIFT                (16)

// reg_50
#define CODEC_CODEC_COUNT_KEEP(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_COUNT_KEEP_MASK                         (0xFFFFFFFF << 0)
#define CODEC_CODEC_COUNT_KEEP_SHIFT                        (0)

// reg_54
#define CODEC_DAC_ENABLE_SEL(n)                             (((n) & 0x3) << 0)
#define CODEC_DAC_ENABLE_SEL_MASK                           (0x3 << 0)
#define CODEC_DAC_ENABLE_SEL_SHIFT                          (0)
#define CODEC_ADC_ENABLE_SEL(n)                             (((n) & 0x3) << 2)
#define CODEC_ADC_ENABLE_SEL_MASK                           (0x3 << 2)
#define CODEC_ADC_ENABLE_SEL_SHIFT                          (2)
#define CODEC_DAC_ENABLE_SEL_SND(n)                         (((n) & 0x3) << 4)
#define CODEC_DAC_ENABLE_SEL_SND_MASK                       (0x3 << 4)
#define CODEC_DAC_ENABLE_SEL_SND_SHIFT                      (4)
#define CODEC_CODEC_DAC_ENABLE_SEL(n)                       (((n) & 0x3) << 6)
#define CODEC_CODEC_DAC_ENABLE_SEL_MASK                     (0x3 << 6)
#define CODEC_CODEC_DAC_ENABLE_SEL_SHIFT                    (6)
#define CODEC_CODEC_ADC_ENABLE_SEL(n)                       (((n) & 0x3) << 8)
#define CODEC_CODEC_ADC_ENABLE_SEL_MASK                     (0x3 << 8)
#define CODEC_CODEC_ADC_ENABLE_SEL_SHIFT                    (8)
#define CODEC_CODEC_ADC_UH_ENABLE_SEL(n)                    (((n) & 0x3) << 10)
#define CODEC_CODEC_ADC_UH_ENABLE_SEL_MASK                  (0x3 << 10)
#define CODEC_CODEC_ADC_UH_ENABLE_SEL_SHIFT                 (10)
#define CODEC_CODEC_ADC_LH_ENABLE_SEL(n)                    (((n) & 0x3) << 12)
#define CODEC_CODEC_ADC_LH_ENABLE_SEL_MASK                  (0x3 << 12)
#define CODEC_CODEC_ADC_LH_ENABLE_SEL_SHIFT                 (12)
#define CODEC_CODEC_DAC_UH_ENABLE_SEL(n)                    (((n) & 0x3) << 14)
#define CODEC_CODEC_DAC_UH_ENABLE_SEL_MASK                  (0x3 << 14)
#define CODEC_CODEC_DAC_UH_ENABLE_SEL_SHIFT                 (14)
#define CODEC_CODEC_DAC_LH_ENABLE_SEL(n)                    (((n) & 0x3) << 16)
#define CODEC_CODEC_DAC_LH_ENABLE_SEL_MASK                  (0x3 << 16)
#define CODEC_CODEC_DAC_LH_ENABLE_SEL_SHIFT                 (16)
#define CODEC_GPIO_TRIGGER_DB_ENABLE                        (1 << 18)
#define CODEC_STAMP_CLR_USED                                (1 << 19)
#define CODEC_EVENT_SEL                                     (1 << 20)
#define CODEC_EVENT_FOR_CAPTURE                             (1 << 21)
#define CODEC_TEST_PORT_SEL(n)                              (((n) & 0x7) << 22)
#define CODEC_TEST_PORT_SEL_MASK                            (0x7 << 22)
#define CODEC_TEST_PORT_SEL_SHIFT                           (22)
#define CODEC_PLL_OSC_TRIGGER_SEL(n)                        (((n) & 0x3) << 25)
#define CODEC_PLL_OSC_TRIGGER_SEL_MASK                      (0x3 << 25)
#define CODEC_PLL_OSC_TRIGGER_SEL_SHIFT                     (25)
#define CODEC_FAULT_MUTE_DAC_ENABLE                         (1 << 27)
#define CODEC_FAULT_MUTE_DAC_ENABLE_SND                     (1 << 28)
#define CODEC_MODE_HCLK_ACCESS_REG                          (1 << 29)

// reg_60
#define CODEC_EN_CLK_ADC_ANA(n)                             (((n) & 0x7) << 0)
#define CODEC_EN_CLK_ADC_ANA_MASK                           (0x7 << 0)
#define CODEC_EN_CLK_ADC_ANA_SHIFT                          (0)
#define CODEC_EN_CLK_ADC(n)                                 (((n) & 0x1F) << 3)
#define CODEC_EN_CLK_ADC_MASK                               (0x1F << 3)
#define CODEC_EN_CLK_ADC_SHIFT                              (3)
#define CODEC_EN_CLK_DAC                                    (1 << 8)
#define CODEC_EN_CLK_CLASSAB                                (1 << 9)
#define CODEC_EN_CLK_CLASSD                                 (1 << 10)
#define CODEC_EN_CLK_RSADC                                  (1 << 11)
#define CODEC_EN_CLK_RSDAC(n)                               (((n) & 0x3) << 12)
#define CODEC_EN_CLK_RSDAC_MASK                             (0x3 << 12)
#define CODEC_EN_CLK_RSDAC_SHIFT                            (12)
#define CODEC_POL_ADC_ANA(n)                                (((n) & 0x7) << 14)
#define CODEC_POL_ADC_ANA_MASK                              (0x7 << 14)
#define CODEC_POL_ADC_ANA_SHIFT                             (14)
#define CODEC_POL_DAC_OUT                                   (1 << 17)
#define CODEC_CFG_CLK_OUT(n)                                (((n) & 0x7) << 18)
#define CODEC_CFG_CLK_OUT_MASK                              (0x7 << 18)
#define CODEC_CFG_CLK_OUT_SHIFT                             (18)
#define CODEC_CODEC_CLKX2_EN                                (1 << 21)
#define CODEC_SEL_OSC_4_EQIIR                               (1 << 22)
#define CODEC_SEL_OSC_2_EQIIR                               (1 << 23)
#define CODEC_POL_CLK_CODEC                                 (1 << 24)

// reg_64
#define CODEC_SOFT_RSTN_ADC_ANA(n)                          (((n) & 0x7) << 0)
#define CODEC_SOFT_RSTN_ADC_ANA_MASK                        (0x7 << 0)
#define CODEC_SOFT_RSTN_ADC_ANA_SHIFT                       (0)
#define CODEC_SOFT_RSTN_ADC(n)                              (((n) & 0x1F) << 3)
#define CODEC_SOFT_RSTN_ADC_MASK                            (0x1F << 3)
#define CODEC_SOFT_RSTN_ADC_SHIFT                           (3)
#define CODEC_SOFT_RSTN_DAC                                 (1 << 8)
#define CODEC_SOFT_RSTN_RS_ADC                              (1 << 9)
#define CODEC_SOFT_RSTN_RS_DAC(n)                           (((n) & 0x3) << 10)
#define CODEC_SOFT_RSTN_RS_DAC_MASK                         (0x3 << 10)
#define CODEC_SOFT_RSTN_RS_DAC_SHIFT                        (10)
#define CODEC_SOFT_RSTN_IIR_ANC(n)                          (((n) & 0xF) << 12)
#define CODEC_SOFT_RSTN_IIR_ANC_MASK                        (0xF << 12)
#define CODEC_SOFT_RSTN_IIR_ANC_SHIFT                       (12)
#define CODEC_SOFT_RSTN_FIR(n)                              (((n) & 0x1F) << 16)
#define CODEC_SOFT_RSTN_FIR_MASK                            (0x1F << 16)
#define CODEC_SOFT_RSTN_FIR_SHIFT                           (16)
#define CODEC_SOFT_RSTN_IIR_EQ                              (1 << 21)

// reg_68
#define CODEC_RET1N_RF                                      (1 << 0)
#define CODEC_RET2N_RF                                      (1 << 1)
#define CODEC_PGEN_RF                                       (1 << 2)
#define CODEC_EMA_RF(n)                                     (((n) & 0x7) << 3)
#define CODEC_EMA_RF_MASK                                   (0x7 << 3)
#define CODEC_EMA_RF_SHIFT                                  (3)
#define CODEC_EMAW_RF(n)                                    (((n) & 0x3) << 6)
#define CODEC_EMAW_RF_MASK                                  (0x3 << 6)
#define CODEC_EMAW_RF_SHIFT                                 (6)
#define CODEC_EMAS_RF                                       (1 << 8)
#define CODEC_WABL_RF                                       (1 << 9)
#define CODEC_WABLM_RF(n)                                   (((n) & 0x3) << 10)
#define CODEC_WABLM_RF_MASK                                 (0x3 << 10)
#define CODEC_WABLM_RF_SHIFT                                (10)
#define CODEC_RET1N_SRAM                                    (1 << 12)
#define CODEC_RET2N_SRAM                                    (1 << 13)
#define CODEC_PGEN_SRAM                                     (1 << 14)
#define CODEC_EMA_SRAM(n)                                   (((n) & 0x7) << 15)
#define CODEC_EMA_SRAM_MASK                                 (0x7 << 15)
#define CODEC_EMA_SRAM_SHIFT                                (15)
#define CODEC_EMAW_SRAM(n)                                  (((n) & 0x3) << 18)
#define CODEC_EMAW_SRAM_MASK                                (0x3 << 18)
#define CODEC_EMAW_SRAM_SHIFT                               (18)
#define CODEC_EMAS_SRAM                                     (1 << 20)
#define CODEC_WABL_SRAM                                     (1 << 21)
#define CODEC_WABLM_SRAM(n)                                 (((n) & 0x7) << 22)
#define CODEC_WABLM_SRAM_MASK                               (0x7 << 22)
#define CODEC_WABLM_SRAM_SHIFT                              (22)
#define CODEC_EMA_ROM(n)                                    (((n) & 0x7) << 25)
#define CODEC_EMA_ROM_MASK                                  (0x7 << 25)
#define CODEC_EMA_ROM_SHIFT                                 (25)
#define CODEC_KEN_ROM                                       (1 << 28)
#define CODEC_PGEN_ROM_0                                    (1 << 29)
#define CODEC_PGEN_ROM_1                                    (1 << 30)

// reg_6c
#define CODEC_RTSEL_ROM(n)                                  (((n) & 0x3) << 0)
#define CODEC_RTSEL_ROM_MASK                                (0x3 << 0)
#define CODEC_RTSEL_ROM_SHIFT                               (0)
#define CODEC_PTSEL_ROM(n)                                  (((n) & 0x3) << 2)
#define CODEC_PTSEL_ROM_MASK                                (0x3 << 2)
#define CODEC_PTSEL_ROM_SHIFT                               (2)
#define CODEC_TRB_ROM(n)                                    (((n) & 0x3) << 4)
#define CODEC_TRB_ROM_MASK                                  (0x3 << 4)
#define CODEC_TRB_ROM_SHIFT                                 (4)
#define CODEC_EN_CLK_IIR_ANC(n)                             (((n) & 0xF) << 6)
#define CODEC_EN_CLK_IIR_ANC_MASK                           (0xF << 6)
#define CODEC_EN_CLK_IIR_ANC_SHIFT                          (6)
#define CODEC_EN_CLK_IIR_EQ                                 (1 << 10)
#define CODEC_EN_CLK_FIR(n)                                 (((n) & 0x1F) << 11)
#define CODEC_EN_CLK_FIR_MASK                               (0x1F << 11)
#define CODEC_EN_CLK_FIR_SHIFT                              (11)
#define CODEC_EN_CLK_VAD                                    (1 << 16)
#define CODEC_SEL_I2S_MCLK(n)                               (((n) & 0x7) << 17)
#define CODEC_SEL_I2S_MCLK_MASK                             (0x7 << 17)
#define CODEC_SEL_I2S_MCLK_SHIFT                            (17)
#define CODEC_EN_I2S_MCLK                                   (1 << 20)
#define CODEC_CFG_DIV_CODEC_EQIIR(n)                        (((n) & 0x3) << 21)
#define CODEC_CFG_DIV_CODEC_EQIIR_MASK                      (0x3 << 21)
#define CODEC_CFG_DIV_CODEC_EQIIR_SHIFT                     (21)
#define CODEC_BYPASS_DIV_CODEC_EQIIR                        (1 << 23)
#define CODEC_SEL_OSCX8_EQIIR                               (1 << 24)
#define CODEC_SEL_OSCX4_EQIIR                               (1 << 25)
#define CODEC_SEL_OSCX2_EQIIR                               (1 << 26)
#define CODEC_SEL_OSC_EQIIR                                 (1 << 27)

// reg_70
#define CODEC_TRIG_TIME(n)                                  (((n) & 0x1FFFF) << 0)
#define CODEC_TRIG_TIME_MASK                                (0x1FFFF << 0)
#define CODEC_TRIG_TIME_SHIFT                               (0)
#define CODEC_TRIG_TIME_ENABLE                              (1 << 17)
#define CODEC_TRIG_MODE                                     (1 << 18)
#define CODEC_GET_CNT_TRIG                                  (1 << 19)
#define CODEC_CFG_DIV_CODEC(n)                              (((n) & 0xF) << 20)
#define CODEC_CFG_DIV_CODEC_MASK                            (0xF << 20)
#define CODEC_CFG_DIV_CODEC_SHIFT                           (20)
#define CODEC_BYPASS_DIV_CODEC                              (1 << 24)
#define CODEC_SEL_PLL_CODEC                                 (1 << 25)
#define CODEC_BYPASS_DIV_CODEC_FIR                          (1 << 26)
#define CODEC_SEL_OSCX8_FIR                                 (1 << 27)
#define CODEC_SEL_OSCX4_FIR                                 (1 << 28)
#define CODEC_SEL_OSCX2_FIR                                 (1 << 29)
#define CODEC_SEL_OSC_FIR                                   (1 << 30)
#define CODEC_SEL_HCLK_FIR                                  (1 << 31)

// reg_74
#define CODEC_SEL_PLL_AUD(n)                                (((n) & 0x3) << 0)
#define CODEC_SEL_PLL_AUD_MASK                              (0x3 << 0)
#define CODEC_SEL_PLL_AUD_SHIFT                             (0)
#define CODEC_CFG_DIV_CODEC_RSADC(n)                        (((n) & 0x3) << 2)
#define CODEC_CFG_DIV_CODEC_RSADC_MASK                      (0x3 << 2)
#define CODEC_CFG_DIV_CODEC_RSADC_SHIFT                     (2)
#define CODEC_CFG_DIV_CODEC_RSDAC(n)                        (((n) & 0x3) << 4)
#define CODEC_CFG_DIV_CODEC_RSDAC_MASK                      (0x3 << 4)
#define CODEC_CFG_DIV_CODEC_RSDAC_SHIFT                     (4)
#define CODEC_CFG_DIV_CODEC_ANCIIR(n)                       (((n) & 0x3) << 6)
#define CODEC_CFG_DIV_CODEC_ANCIIR_MASK                     (0x3 << 6)
#define CODEC_CFG_DIV_CODEC_ANCIIR_SHIFT                    (6)
#define CODEC_CFG_DIV_CODEC_FIR(n)                          (((n) & 0x3) << 8)
#define CODEC_CFG_DIV_CODEC_FIR_MASK                        (0x3 << 8)
#define CODEC_CFG_DIV_CODEC_FIR_SHIFT                       (8)
#define CODEC_SEL_OSC_PRE_FIR                               (1 << 10)
#define CODEC_BYPASS_DIV_CODEC_RSADC                        (1 << 11)
#define CODEC_SEL_OSCX8_RSADC                               (1 << 12)
#define CODEC_SEL_OSCX4_RSADC                               (1 << 13)
#define CODEC_SEL_OSCX2_RSADC                               (1 << 14)
#define CODEC_SEL_OSC_RSADC                                 (1 << 15)
#define CODEC_BYPASS_DIV_CODEC_RSDAC                        (1 << 16)
#define CODEC_SEL_OSCX8_RSDAC                               (1 << 17)
#define CODEC_SEL_OSCX4_RSDAC                               (1 << 18)
#define CODEC_SEL_OSCX2_RSDAC                               (1 << 19)
#define CODEC_SEL_OSC_RSDAC                                 (1 << 20)
#define CODEC_BYPASS_DIV_CODEC_ANCIIR                       (1 << 21)
#define CODEC_SEL_OSCX8_ANCIIR                              (1 << 22)
#define CODEC_SEL_OSCX4_ANCIIR                              (1 << 23)
#define CODEC_SEL_OSCX2_ANCIIR                              (1 << 24)
#define CODEC_SEL_OSC_ANCIIR                                (1 << 25)
#define CODEC_SEL_OSC_CODEC                                 (1 << 26)
#define CODEC_SEL_CLK_PLL_CODEC                             (1 << 27)
#define CODEC_SEL_PLL_CODECHCLK                             (1 << 28)
#define CODEC_SEL_HCLK_OSC                                  (1 << 29)
#define CODEC_SEL_CODEC_HCLK_SENS                           (1 << 30)

// reg_78
#define CODEC_ADC_ENABLE_PLAYTIME_STAMP_SEL(n)              (((n) & 0x3) << 0)
#define CODEC_ADC_ENABLE_PLAYTIME_STAMP_SEL_MASK            (0x3 << 0)
#define CODEC_ADC_ENABLE_PLAYTIME_STAMP_SEL_SHIFT           (0)
#define CODEC_DAC_ENABLE_PLAYTIME_STAMP_SEL(n)              (((n) & 0x3) << 2)
#define CODEC_DAC_ENABLE_PLAYTIME_STAMP_SEL_MASK            (0x3 << 2)
#define CODEC_DAC_ENABLE_PLAYTIME_STAMP_SEL_SHIFT           (2)
#define CODEC_CODEC_ADC_ENABLE_PLAYTIME_STAMP_SEL(n)        (((n) & 0x3) << 4)
#define CODEC_CODEC_ADC_ENABLE_PLAYTIME_STAMP_SEL_MASK      (0x3 << 4)
#define CODEC_CODEC_ADC_ENABLE_PLAYTIME_STAMP_SEL_SHIFT     (4)
#define CODEC_CODEC_DAC_ENABLE_PLAYTIME_STAMP_SEL(n)        (((n) & 0x3) << 6)
#define CODEC_CODEC_DAC_ENABLE_PLAYTIME_STAMP_SEL_MASK      (0x3 << 6)
#define CODEC_CODEC_DAC_ENABLE_PLAYTIME_STAMP_SEL_SHIFT     (6)
#define CODEC_CODEC_ADC_UH_ENABLE_PLAYTIME_STAMP_SEL(n)     (((n) & 0x3) << 8)
#define CODEC_CODEC_ADC_UH_ENABLE_PLAYTIME_STAMP_SEL_MASK   (0x3 << 8)
#define CODEC_CODEC_ADC_UH_ENABLE_PLAYTIME_STAMP_SEL_SHIFT  (8)
#define CODEC_CODEC_ADC_LH_ENABLE_PLAYTIME_STAMP_SEL(n)     (((n) & 0x3) << 10)
#define CODEC_CODEC_ADC_LH_ENABLE_PLAYTIME_STAMP_SEL_MASK   (0x3 << 10)
#define CODEC_CODEC_ADC_LH_ENABLE_PLAYTIME_STAMP_SEL_SHIFT  (10)
#define CODEC_CODEC_DAC_UH_ENABLE_PLAYTIME_STAMP_SEL(n)     (((n) & 0x3) << 12)
#define CODEC_CODEC_DAC_UH_ENABLE_PLAYTIME_STAMP_SEL_MASK   (0x3 << 12)
#define CODEC_CODEC_DAC_UH_ENABLE_PLAYTIME_STAMP_SEL_SHIFT  (12)
#define CODEC_CODEC_DAC_LH_ENABLE_PLAYTIME_STAMP_SEL(n)     (((n) & 0x3) << 14)
#define CODEC_CODEC_DAC_LH_ENABLE_PLAYTIME_STAMP_SEL_MASK   (0x3 << 14)
#define CODEC_CODEC_DAC_LH_ENABLE_PLAYTIME_STAMP_SEL_SHIFT  (14)
#define CODEC_DAC_ENABLE_SND_PLAYTIME_STAMP_SEL(n)          (((n) & 0x3) << 16)
#define CODEC_DAC_ENABLE_SND_PLAYTIME_STAMP_SEL_MASK        (0x3 << 16)
#define CODEC_DAC_ENABLE_SND_PLAYTIME_STAMP_SEL_SHIFT       (16)
#define CODEC_CODEC_DAC_ENABLE_SND_PLAYTIME_STAMP_SEL(n)    (((n) & 0x3) << 18)
#define CODEC_CODEC_DAC_ENABLE_SND_PLAYTIME_STAMP_SEL_MASK  (0x3 << 18)
#define CODEC_CODEC_DAC_ENABLE_SND_PLAYTIME_STAMP_SEL_SHIFT (18)
#define CODEC_PLL_OSC_TRIGGER_PLAYTIME_STAMP_SEL(n)         (((n) & 0x3) << 20)
#define CODEC_PLL_OSC_TRIGGER_PLAYTIME_STAMP_SEL_MASK       (0x3 << 20)
#define CODEC_PLL_OSC_TRIGGER_PLAYTIME_STAMP_SEL_SHIFT      (20)
#define CODEC_RESAMPLE_DAC_ENABLE_PLAYTIME_STAMP_SEL(n)     (((n) & 0x3) << 22)
#define CODEC_RESAMPLE_DAC_ENABLE_PLAYTIME_STAMP_SEL_MASK   (0x3 << 22)
#define CODEC_RESAMPLE_DAC_ENABLE_PLAYTIME_STAMP_SEL_SHIFT  (22)
#define CODEC_RESAMPLE_ADC_ENABLE_PLAYTIME_STAMP_SEL(n)     (((n) & 0x3) << 24)
#define CODEC_RESAMPLE_ADC_ENABLE_PLAYTIME_STAMP_SEL_MASK   (0x3 << 24)
#define CODEC_RESAMPLE_ADC_ENABLE_PLAYTIME_STAMP_SEL_SHIFT  (24)
#define CODEC_RESAMPLE_DAC_PHASE_PLAYTIME_STAMP_SEL(n)      (((n) & 0x3) << 26)
#define CODEC_RESAMPLE_DAC_PHASE_PLAYTIME_STAMP_SEL_MASK    (0x3 << 26)
#define CODEC_RESAMPLE_DAC_PHASE_PLAYTIME_STAMP_SEL_SHIFT   (26)
#define CODEC_RESAMPLE_ADC_PHASE_PLAYTIME_STAMP_SEL(n)      (((n) & 0x3) << 28)
#define CODEC_RESAMPLE_ADC_PHASE_PLAYTIME_STAMP_SEL_MASK    (0x3 << 28)
#define CODEC_RESAMPLE_ADC_PHASE_PLAYTIME_STAMP_SEL_SHIFT   (28)
#define CODEC_RESAMPLE_DAC_ENABLE_SND_PLAYTIME_STAMP_SEL(n) (((n) & 0x3) << 30)
#define CODEC_RESAMPLE_DAC_ENABLE_SND_PLAYTIME_STAMP_SEL_MASK (0x3 << 30)
#define CODEC_RESAMPLE_DAC_ENABLE_SND_PLAYTIME_STAMP_SEL_SHIFT (30)

// reg_7c
#define CODEC_RESAMPLE_DAC_PHASE_SND_PLAYTIME_STAMP_SEL(n)  (((n) & 0x3) << 0)
#define CODEC_RESAMPLE_DAC_PHASE_SND_PLAYTIME_STAMP_SEL_MASK (0x3 << 0)
#define CODEC_RESAMPLE_DAC_PHASE_SND_PLAYTIME_STAMP_SEL_SHIFT (0)
#define CODEC_CODEC_DAC_GAIN_PLAYTIME_STAMP_SEL(n)          (((n) & 0x3) << 2)
#define CODEC_CODEC_DAC_GAIN_PLAYTIME_STAMP_SEL_MASK        (0x3 << 2)
#define CODEC_CODEC_DAC_GAIN_PLAYTIME_STAMP_SEL_SHIFT       (2)
#define CODEC_CODEC_DAC_GAIN_SND_PLAYTIME_STAMP_SEL(n)      (((n) & 0x3) << 4)
#define CODEC_CODEC_DAC_GAIN_SND_PLAYTIME_STAMP_SEL_MASK    (0x3 << 4)
#define CODEC_CODEC_DAC_GAIN_SND_PLAYTIME_STAMP_SEL_SHIFT   (4)
#define CODEC_TRIGGER_EVENT_PLAYTIME_STAMP_SEL(n)           (((n) & 0x3) << 6)
#define CODEC_TRIGGER_EVENT_PLAYTIME_STAMP_SEL_MASK         (0x3 << 6)
#define CODEC_TRIGGER_EVENT_PLAYTIME_STAMP_SEL_SHIFT        (6)
#define CODEC_PLAYTIME_STAMP_MSK                            (1 << 8)
#define CODEC_PLAYTIME_STAMP1_MSK                           (1 << 9)
#define CODEC_PLAYTIME_STAMP2_MSK                           (1 << 10)
#define CODEC_PLAYTIME_STAMP3_MSK                           (1 << 11)

// reg_80
#define CODEC_CODEC_ADC_EN                                  (1 << 0)
#define CODEC_CODEC_ADC_EN_CH0                              (1 << 1)
#define CODEC_CODEC_ADC_EN_CH1                              (1 << 2)
#define CODEC_CODEC_ADC_EN_CH2                              (1 << 3)
#define CODEC_CODEC_ADC_EN_CH3                              (1 << 4)
#define CODEC_CODEC_SIDE_TONE_GAIN(n)                       (((n) & 0x1F) << 5)
#define CODEC_CODEC_SIDE_TONE_GAIN_MASK                     (0x1F << 5)
#define CODEC_CODEC_SIDE_TONE_GAIN_SHIFT                    (5)
#define CODEC_CODEC_SIDE_TONE_MIC_SEL(n)                    (((n) & 0x7) << 10)
#define CODEC_CODEC_SIDE_TONE_MIC_SEL_MASK                  (0x7 << 10)
#define CODEC_CODEC_SIDE_TONE_MIC_SEL_SHIFT                 (10)
#define CODEC_CODEC_SIDE_TONE_IIR_ENABLE                    (1 << 13)
#define CODEC_CODEC_ADC_LOOP                                (1 << 14)
#define CODEC_CODEC_LOOP_SEL_L(n)                           (((n) & 0x7) << 15)
#define CODEC_CODEC_LOOP_SEL_L_MASK                         (0x7 << 15)
#define CODEC_CODEC_LOOP_SEL_L_SHIFT                        (15)
#define CODEC_CODEC_ADC_UH_EN                               (1 << 18)
#define CODEC_CODEC_ADC_LH_EN                               (1 << 19)
#define CODEC_CODEC_TEST_PORT_SEL(n)                        (((n) & 0x1F) << 20)
#define CODEC_CODEC_TEST_PORT_SEL_MASK                      (0x1F << 20)
#define CODEC_CODEC_TEST_PORT_SEL_SHIFT                     (20)

// reg_84
#define CODEC_CODEC_ADC_SIGNED_CH0                          (1 << 0)
#define CODEC_CODEC_ADC_IN_SEL_CH0(n)                       (((n) & 0x7) << 1)
#define CODEC_CODEC_ADC_IN_SEL_CH0_MASK                     (0x7 << 1)
#define CODEC_CODEC_ADC_IN_SEL_CH0_SHIFT                    (1)
#define CODEC_CODEC_ADC_DOWN_SEL_CH0(n)                     (((n) & 0x3) << 4)
#define CODEC_CODEC_ADC_DOWN_SEL_CH0_MASK                   (0x3 << 4)
#define CODEC_CODEC_ADC_DOWN_SEL_CH0_SHIFT                  (4)
#define CODEC_CODEC_ADC_HBF3_BYPASS_CH0                     (1 << 6)
#define CODEC_CODEC_ADC_HBF2_BYPASS_CH0                     (1 << 7)
#define CODEC_CODEC_ADC_HBF1_BYPASS_CH0                     (1 << 8)
#define CODEC_CODEC_ADC_GAIN_SEL_CH0                        (1 << 9)
#define CODEC_CODEC_ADC_GAIN_CH0(n)                         (((n) & 0xFFFFF) << 10)
#define CODEC_CODEC_ADC_GAIN_CH0_MASK                       (0xFFFFF << 10)
#define CODEC_CODEC_ADC_GAIN_CH0_SHIFT                      (10)
#define CODEC_CODEC_ADC_HBF3_SEL_CH0(n)                     (((n) & 0x3) << 30)
#define CODEC_CODEC_ADC_HBF3_SEL_CH0_MASK                   (0x3 << 30)
#define CODEC_CODEC_ADC_HBF3_SEL_CH0_SHIFT                  (30)

// reg_88
#define CODEC_CODEC_ADC_SIGNED_CH1                          (1 << 0)
#define CODEC_CODEC_ADC_IN_SEL_CH1(n)                       (((n) & 0x7) << 1)
#define CODEC_CODEC_ADC_IN_SEL_CH1_MASK                     (0x7 << 1)
#define CODEC_CODEC_ADC_IN_SEL_CH1_SHIFT                    (1)
#define CODEC_CODEC_ADC_DOWN_SEL_CH1(n)                     (((n) & 0x3) << 4)
#define CODEC_CODEC_ADC_DOWN_SEL_CH1_MASK                   (0x3 << 4)
#define CODEC_CODEC_ADC_DOWN_SEL_CH1_SHIFT                  (4)
#define CODEC_CODEC_ADC_HBF3_BYPASS_CH1                     (1 << 6)
#define CODEC_CODEC_ADC_HBF2_BYPASS_CH1                     (1 << 7)
#define CODEC_CODEC_ADC_HBF1_BYPASS_CH1                     (1 << 8)
#define CODEC_CODEC_ADC_GAIN_SEL_CH1                        (1 << 9)
#define CODEC_CODEC_ADC_GAIN_CH1(n)                         (((n) & 0xFFFFF) << 10)
#define CODEC_CODEC_ADC_GAIN_CH1_MASK                       (0xFFFFF << 10)
#define CODEC_CODEC_ADC_GAIN_CH1_SHIFT                      (10)
#define CODEC_CODEC_ADC_HBF3_SEL_CH1(n)                     (((n) & 0x3) << 30)
#define CODEC_CODEC_ADC_HBF3_SEL_CH1_MASK                   (0x3 << 30)
#define CODEC_CODEC_ADC_HBF3_SEL_CH1_SHIFT                  (30)

// reg_8c
#define CODEC_CODEC_ADC_SIGNED_CH2                          (1 << 0)
#define CODEC_CODEC_ADC_IN_SEL_CH2(n)                       (((n) & 0x7) << 1)
#define CODEC_CODEC_ADC_IN_SEL_CH2_MASK                     (0x7 << 1)
#define CODEC_CODEC_ADC_IN_SEL_CH2_SHIFT                    (1)
#define CODEC_CODEC_ADC_DOWN_SEL_CH2(n)                     (((n) & 0x3) << 4)
#define CODEC_CODEC_ADC_DOWN_SEL_CH2_MASK                   (0x3 << 4)
#define CODEC_CODEC_ADC_DOWN_SEL_CH2_SHIFT                  (4)
#define CODEC_CODEC_ADC_HBF3_BYPASS_CH2                     (1 << 6)
#define CODEC_CODEC_ADC_HBF2_BYPASS_CH2                     (1 << 7)
#define CODEC_CODEC_ADC_HBF1_BYPASS_CH2                     (1 << 8)
#define CODEC_CODEC_ADC_GAIN_SEL_CH2                        (1 << 9)
#define CODEC_CODEC_ADC_GAIN_CH2(n)                         (((n) & 0xFFFFF) << 10)
#define CODEC_CODEC_ADC_GAIN_CH2_MASK                       (0xFFFFF << 10)
#define CODEC_CODEC_ADC_GAIN_CH2_SHIFT                      (10)
#define CODEC_CODEC_ADC_HBF3_SEL_CH2(n)                     (((n) & 0x3) << 30)
#define CODEC_CODEC_ADC_HBF3_SEL_CH2_MASK                   (0x3 << 30)
#define CODEC_CODEC_ADC_HBF3_SEL_CH2_SHIFT                  (30)

// reg_98
#define CODEC_CODEC_ECHO_ENABLE_CH0                         (1 << 0)
#define CODEC_CODEC_ADC_DOWN_SEL_CH3(n)                     (((n) & 0x3) << 1)
#define CODEC_CODEC_ADC_DOWN_SEL_CH3_MASK                   (0x3 << 1)
#define CODEC_CODEC_ADC_DOWN_SEL_CH3_SHIFT                  (1)
#define CODEC_CODEC_ADC_HBF3_BYPASS_CH3                     (1 << 3)
#define CODEC_CODEC_ADC_HBF2_BYPASS_CH3                     (1 << 4)
#define CODEC_CODEC_ADC_HBF1_BYPASS_CH3                     (1 << 5)
#define CODEC_CODEC_ADC_GAIN_SEL_CH3                        (1 << 6)
#define CODEC_CODEC_ADC_GAIN_CH3(n)                         (((n) & 0xFFFFF) << 7)
#define CODEC_CODEC_ADC_GAIN_CH3_MASK                       (0xFFFFF << 7)
#define CODEC_CODEC_ADC_GAIN_CH3_SHIFT                      (7)
#define CODEC_CODEC_DAC_ECHO_DATA_SEL_CH0(n)                (((n) & 0x3) << 27)
#define CODEC_CODEC_DAC_ECHO_DATA_SEL_CH0_MASK              (0x3 << 27)
#define CODEC_CODEC_DAC_ECHO_DATA_SEL_CH0_SHIFT             (27)

// reg_ac
#define CODEC_CODEC_ADC_FIFO_THRESHOLD(n)                   (((n) & 0xF) << 0)
#define CODEC_CODEC_ADC_FIFO_THRESHOLD_MASK                 (0xF << 0)
#define CODEC_CODEC_ADC_FIFO_THRESHOLD_SHIFT                (0)
#define CODEC_CODEC_ADC_FIFO_FLUSH_CH0                      (1 << 4)
#define CODEC_CODEC_ADC_FIFO_FLUSH_CH1                      (1 << 5)
#define CODEC_CODEC_ADC_FIFO_FLUSH_CH2                      (1 << 6)
#define CODEC_CODEC_ADC_FIFO_POP_MODE                       (1 << 7)

// reg_b0
#define CODEC_CODEC_DAC_EN                                  (1 << 0)
#define CODEC_CODEC_DAC_EN_CH0                              (1 << 1)
#define CODEC_CODEC_DAC_DITHER_GAIN(n)                      (((n) & 0x1F) << 2)
#define CODEC_CODEC_DAC_DITHER_GAIN_MASK                    (0x1F << 2)
#define CODEC_CODEC_DAC_DITHER_GAIN_SHIFT                   (2)
#define CODEC_CODEC_DAC_SDM_GAIN(n)                         (((n) & 0x7) << 7)
#define CODEC_CODEC_DAC_SDM_GAIN_MASK                       (0x7 << 7)
#define CODEC_CODEC_DAC_SDM_GAIN_SHIFT                      (7)
#define CODEC_CODEC_DITHER_BYPASS                           (1 << 10)
#define CODEC_CODEC_DAC_HBF3_BYPASS                         (1 << 11)
#define CODEC_CODEC_DAC_HBF2_BYPASS                         (1 << 12)
#define CODEC_CODEC_DAC_HBF1_BYPASS                         (1 << 13)
#define CODEC_CODEC_DAC_UP_SEL(n)                           (((n) & 0x7) << 14)
#define CODEC_CODEC_DAC_UP_SEL_MASK                         (0x7 << 14)
#define CODEC_CODEC_DAC_UP_SEL_SHIFT                        (14)
#define CODEC_CODEC_DAC_TONE_TEST                           (1 << 17)
#define CODEC_CODEC_DAC_SIN1K_STEP(n)                       (((n) & 0xF) << 18)
#define CODEC_CODEC_DAC_SIN1K_STEP_MASK                     (0xF << 18)
#define CODEC_CODEC_DAC_SIN1K_STEP_SHIFT                    (18)
#define CODEC_CODEC_DAC_OSR_SEL(n)                          (((n) & 0x3) << 22)
#define CODEC_CODEC_DAC_OSR_SEL_MASK                        (0x3 << 22)
#define CODEC_CODEC_DAC_OSR_SEL_SHIFT                       (22)
#define CODEC_CODEC_DAC_SDM_H4_6M_CH0                       (1 << 24)
#define CODEC_CODEC_DAC_L_FIR_UPSAMPLE                      (1 << 25)
#define CODEC_CODEC_DAC_USE_HBF4                            (1 << 26)
#define CODEC_CODEC_DAC_USE_HBF5                            (1 << 27)

// reg_b4
#define CODEC_CODEC_DAC_GAIN_CH0(n)                         (((n) & 0xFFFFF) << 0)
#define CODEC_CODEC_DAC_GAIN_CH0_MASK                       (0xFFFFF << 0)
#define CODEC_CODEC_DAC_GAIN_CH0_SHIFT                      (0)
#define CODEC_CODEC_DAC_GAIN_SEL_CH0                        (1 << 20)
#define CODEC_CODEC_DAC_GAIN_UPDATE                         (1 << 21)
#define CODEC_CODEC_ADC_GAIN_UPDATE_CH0                     (1 << 22)
#define CODEC_CODEC_ADC_GAIN_UPDATE_CH1                     (1 << 23)
#define CODEC_CODEC_ADC_GAIN_UPDATE_CH2                     (1 << 24)
#define CODEC_CODEC_ADC_GAIN_UPDATE_CH3                     (1 << 25)
#define CODEC_CODEC_DAC_GAIN_TRIGGER_SEL(n)                 (((n) & 0x3) << 26)
#define CODEC_CODEC_DAC_GAIN_TRIGGER_SEL_MASK               (0x3 << 26)
#define CODEC_CODEC_DAC_GAIN_TRIGGER_SEL_SHIFT              (26)

// reg_b8
#define CODEC_CODEC_DAC_H4_DELAY_CH0(n)                     (((n) & 0x3) << 0)
#define CODEC_CODEC_DAC_H4_DELAY_CH0_MASK                   (0x3 << 0)
#define CODEC_CODEC_DAC_H4_DELAY_CH0_SHIFT                  (0)
#define CODEC_CODEC_DAC_L4_DELAY_CH0(n)                     (((n) & 0x3) << 2)
#define CODEC_CODEC_DAC_L4_DELAY_CH0_MASK                   (0x3 << 2)
#define CODEC_CODEC_DAC_L4_DELAY_CH0_SHIFT                  (2)
#define CODEC_CODEC_DAC_HBF4_DELAY_SEL                      (1 << 4)

// reg_bc
#define CODEC_CODEC_DAC_UH_EN                               (1 << 0)
#define CODEC_CODEC_DAC_LH_EN                               (1 << 1)
#define CODEC_CODEC_DAC_SDM_3RD_EN_CH0                      (1 << 2)
#define CODEC_CODEC_DAC_SDM_CLOSE                           (1 << 3)
#define CODEC_CODEC_DITHERF_BYPASS                          (1 << 4)
#define CODEC_CODEC_DAC_DITHERF_GAIN(n)                     (((n) & 0x3) << 5)
#define CODEC_CODEC_DAC_DITHERF_GAIN_MASK                   (0x3 << 5)
#define CODEC_CODEC_DAC_DITHERF_GAIN_SHIFT                  (5)
#define CODEC_CODEC_DAC_MIX_MODE1                           (1 << 7)
#define CODEC_CODEC_DAC_MIX_MODE2                           (1 << 8)
#define CODEC_EN_48KX64_MODE(n)                             (((n) & 0x3) << 9)
#define CODEC_EN_48KX64_MODE_MASK                           (0x3 << 9)
#define CODEC_EN_48KX64_MODE_SHIFT                          (9)
#define CODEC_ADC_DCF_BYPASS_CH0                            (1 << 11)
#define CODEC_ADC_DCF_BYPASS_CH1                            (1 << 12)
#define CODEC_ADC_DCF_BYPASS_CH2                            (1 << 13)
#define CODEC_ADC_DC_SEL                                    (1 << 14)

// reg_c0
#define CODEC_ADC_UDC_CH0(n)                                (((n) & 0xF) << 0)
#define CODEC_ADC_UDC_CH0_MASK                              (0xF << 0)
#define CODEC_ADC_UDC_CH0_SHIFT                             (0)
#define CODEC_ADC_UDC_CH1(n)                                (((n) & 0xF) << 4)
#define CODEC_ADC_UDC_CH1_MASK                              (0xF << 4)
#define CODEC_ADC_UDC_CH1_SHIFT                             (4)
#define CODEC_ADC_UDC_CH2(n)                                (((n) & 0xF) << 8)
#define CODEC_ADC_UDC_CH2_MASK                              (0xF << 8)
#define CODEC_ADC_UDC_CH2_SHIFT                             (8)

// reg_c4
#define CODEC_CODEC_PDM_ENABLE                              (1 << 0)
#define CODEC_CODEC_PDM_DATA_INV                            (1 << 1)
#define CODEC_CODEC_PDM_RATE_SEL_01(n)                      (((n) & 0x3) << 2)
#define CODEC_CODEC_PDM_RATE_SEL_01_MASK                    (0x3 << 2)
#define CODEC_CODEC_PDM_RATE_SEL_01_SHIFT                   (2)
#define CODEC_CODEC_PDM_RATE_SEL_23(n)                      (((n) & 0x3) << 4)
#define CODEC_CODEC_PDM_RATE_SEL_23_MASK                    (0x3 << 4)
#define CODEC_CODEC_PDM_RATE_SEL_23_SHIFT                   (4)
#define CODEC_CODEC_PDM_RATE_SEL_45(n)                      (((n) & 0x3) << 6)
#define CODEC_CODEC_PDM_RATE_SEL_45_MASK                    (0x3 << 6)
#define CODEC_CODEC_PDM_RATE_SEL_45_SHIFT                   (6)
#define CODEC_CODEC_PDM_ADC_SEL_CH0                         (1 << 8)
#define CODEC_CODEC_PDM_ADC_SEL_CH1                         (1 << 9)
#define CODEC_CODEC_PDM_ADC_SEL_CH2                         (1 << 10)
#define CODEC_CODEC_PDM_VAD_SHARE_SEL(n)                    (((n) & 0x3) << 11)
#define CODEC_CODEC_PDM_VAD_SHARE_SEL_MASK                  (0x3 << 11)
#define CODEC_CODEC_PDM_VAD_SHARE_SEL_SHIFT                 (11)

// reg_c8
#define CODEC_CODEC_PDM_MUX_CH0(n)                          (((n) & 0x7) << 0)
#define CODEC_CODEC_PDM_MUX_CH0_MASK                        (0x7 << 0)
#define CODEC_CODEC_PDM_MUX_CH0_SHIFT                       (0)
#define CODEC_CODEC_PDM_MUX_CH1(n)                          (((n) & 0x7) << 3)
#define CODEC_CODEC_PDM_MUX_CH1_MASK                        (0x7 << 3)
#define CODEC_CODEC_PDM_MUX_CH1_SHIFT                       (3)
#define CODEC_CODEC_PDM_MUX_CH2(n)                          (((n) & 0x7) << 6)
#define CODEC_CODEC_PDM_MUX_CH2_MASK                        (0x7 << 6)
#define CODEC_CODEC_PDM_MUX_CH2_SHIFT                       (6)
#define CODEC_CODEC_PDM_CAP_PHASE_CH0(n)                    (((n) & 0x3) << 9)
#define CODEC_CODEC_PDM_CAP_PHASE_CH0_MASK                  (0x3 << 9)
#define CODEC_CODEC_PDM_CAP_PHASE_CH0_SHIFT                 (9)
#define CODEC_CODEC_PDM_CAP_PHASE_CH1(n)                    (((n) & 0x3) << 11)
#define CODEC_CODEC_PDM_CAP_PHASE_CH1_MASK                  (0x3 << 11)
#define CODEC_CODEC_PDM_CAP_PHASE_CH1_SHIFT                 (11)
#define CODEC_CODEC_PDM_CAP_PHASE_CH2(n)                    (((n) & 0x3) << 13)
#define CODEC_CODEC_PDM_CAP_PHASE_CH2_MASK                  (0x3 << 13)
#define CODEC_CODEC_PDM_CAP_PHASE_CH2_SHIFT                 (13)
#define CODEC_CODEC_PDM_CAP_PHASE_CH3(n)                    (((n) & 0x3) << 15)
#define CODEC_CODEC_PDM_CAP_PHASE_CH3_MASK                  (0x3 << 15)
#define CODEC_CODEC_PDM_CAP_PHASE_CH3_SHIFT                 (15)
#define CODEC_CODEC_PDM_CAP_PHASE_CH4(n)                    (((n) & 0x3) << 17)
#define CODEC_CODEC_PDM_CAP_PHASE_CH4_MASK                  (0x3 << 17)
#define CODEC_CODEC_PDM_CAP_PHASE_CH4_SHIFT                 (17)
#define CODEC_CODEC_PDM_CAP_PHASE_CH5(n)                    (((n) & 0x3) << 19)
#define CODEC_CODEC_PDM_CAP_PHASE_CH5_MASK                  (0x3 << 19)
#define CODEC_CODEC_PDM_CAP_PHASE_CH5_SHIFT                 (19)

// reg_d0
#define CODEC_CODEC_ANC_ENABLE_CH0                          (1 << 0)
#define CODEC_CODEC_DUAL_ANC_CH0                            (1 << 1)
#define CODEC_CODEC_ANC_MUTE_CH0                            (1 << 2)
#define CODEC_CODEC_FF_CH0_FIR_EN                           (1 << 3)
#define CODEC_CODEC_FB_CH0_FIR_EN                           (1 << 4)
#define CODEC_CODEC_ANC_RATE_SEL                            (1 << 5)
#define CODEC_CODEC_ANC_FF_SR_SEL(n)                        (((n) & 0x3) << 6)
#define CODEC_CODEC_ANC_FF_SR_SEL_MASK                      (0x3 << 6)
#define CODEC_CODEC_ANC_FF_SR_SEL_SHIFT                     (6)
#define CODEC_CODEC_ANC_FF_IN_PHASE_SEL(n)                  (((n) & 0x7) << 8)
#define CODEC_CODEC_ANC_FF_IN_PHASE_SEL_MASK                (0x7 << 8)
#define CODEC_CODEC_ANC_FF_IN_PHASE_SEL_SHIFT               (8)
#define CODEC_CODEC_ANC_FB_SR_SEL(n)                        (((n) & 0x3) << 11)
#define CODEC_CODEC_ANC_FB_SR_SEL_MASK                      (0x3 << 11)
#define CODEC_CODEC_ANC_FB_SR_SEL_SHIFT                     (11)
#define CODEC_CODEC_ANC_FB_IN_PHASE_SEL(n)                  (((n) & 0x7) << 13)
#define CODEC_CODEC_ANC_FB_IN_PHASE_SEL_MASK                (0x7 << 13)
#define CODEC_CODEC_ANC_FB_IN_PHASE_SEL_SHIFT               (13)
#define CODEC_CODEC_FEEDBACK_CH0                            (1 << 16)
#define CODEC_CODEC_ADC_FIR_DS_EN_CH0                       (1 << 17)
#define CODEC_CODEC_ADC_FIR_DS_SEL_CH0                      (1 << 18)
#define CODEC_CODEC_ADC_FIR_DS_EN_CH1                       (1 << 19)
#define CODEC_CODEC_ADC_FIR_DS_SEL_CH1                      (1 << 20)
#define CODEC_CODEC_ADC_FIR_DS_EN_CH2                       (1 << 21)
#define CODEC_CODEC_ADC_FIR_DS_SEL_CH2                      (1 << 22)
#define CODEC_CODEC_TT0_FIR_EN                              (1 << 23)
#define CODEC_CODEC_MM0_FIR_EN                              (1 << 24)
#define CODEC_CODEC_DEQ_FIR_EN_CH0                          (1 << 25)
#define CODEC_CODEC_DEQ_FIR_EN_CH1                          (1 << 26)

// reg_d4
#define CODEC_CODEC_ANC_MUTE_GAIN_FF_CH0(n)                 (((n) & 0xFFF) << 0)
#define CODEC_CODEC_ANC_MUTE_GAIN_FF_CH0_MASK               (0xFFF << 0)
#define CODEC_CODEC_ANC_MUTE_GAIN_FF_CH0_SHIFT              (0)
#define CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FF_CH0              (1 << 12)
#define CODEC_CODEC_ANC_MUTE_GAIN_UPDATE_FF_CH0             (1 << 13)

// reg_d8
#define CODEC_CODEC_ANC_MUTE_GAIN_FB_CH0(n)                 (((n) & 0xFFF) << 0)
#define CODEC_CODEC_ANC_MUTE_GAIN_FB_CH0_MASK               (0xFFF << 0)
#define CODEC_CODEC_ANC_MUTE_GAIN_FB_CH0_SHIFT              (0)
#define CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FB_CH0              (1 << 12)
#define CODEC_CODEC_ANC_MUTE_GAIN_UPDATE_FB_CH0             (1 << 13)

// reg_dc
#define CODEC_CODEC_ADC_MC_EN_CH0                           (1 << 0)
#define CODEC_CODEC_FEEDBACK_MC_EN_CH0                      (1 << 1)
#define CODEC_CODEC_DAC_L_IIR_ENABLE                        (1 << 2)
#define CODEC_CODEC_ADC_CH0_IIR_ENABLE                      (1 << 3)
#define CODEC_CODEC_ADC_CH1_IIR_ENABLE                      (1 << 4)
#define CODEC_CODEC_ADC_CH4_IIR_ENABLE                      (1 << 5)
#define CODEC_CODEC_ADC_CH5_IIR_ENABLE                      (1 << 6)
#define CODEC_CODEC_FB_CHECK_UDC_CH0(n)                     (((n) & 0xF) << 7)
#define CODEC_CODEC_FB_CHECK_UDC_CH0_MASK                   (0xF << 7)
#define CODEC_CODEC_FB_CHECK_UDC_CH0_SHIFT                  (7)
#define CODEC_CODEC_FF0_FIR_SEL                             (1 << 11)
#define CODEC_CODEC_TT0_FIR_SEL                             (1 << 12)
#define CODEC_CODEC_FB0_FIR_SEL                             (1 << 13)
#define CODEC_CODEC_MM0_FIR_SEL                             (1 << 14)

// reg_e0
#define CODEC_CODEC_ADC_IIR_CH0_SEL(n)                      (((n) & 0xF) << 0)
#define CODEC_CODEC_ADC_IIR_CH0_SEL_MASK                    (0xF << 0)
#define CODEC_CODEC_ADC_IIR_CH0_SEL_SHIFT                   (0)
#define CODEC_CODEC_ADC_IIR_CH1_SEL(n)                      (((n) & 0xF) << 4)
#define CODEC_CODEC_ADC_IIR_CH1_SEL_MASK                    (0xF << 4)
#define CODEC_CODEC_ADC_IIR_CH1_SEL_SHIFT                   (4)
#define CODEC_CODEC_ADC_IIR_CH4_SEL(n)                      (((n) & 0xF) << 8)
#define CODEC_CODEC_ADC_IIR_CH4_SEL_MASK                    (0xF << 8)
#define CODEC_CODEC_ADC_IIR_CH4_SEL_SHIFT                   (8)
#define CODEC_CODEC_ADC_IIR_CH5_SEL(n)                      (((n) & 0xF) << 12)
#define CODEC_CODEC_ADC_IIR_CH5_SEL_MASK                    (0xF << 12)
#define CODEC_CODEC_ADC_IIR_CH5_SEL_SHIFT                   (12)

// reg_e4
#define CODEC_CODEC_RESAMPLE_DAC_ENABLE                     (1 << 0)
#define CODEC_CODEC_RESAMPLE_DAC_L_ENABLE                   (1 << 1)
#define CODEC_CODEC_RESAMPLE_DAC_FIFO_ENABLE                (1 << 2)
#define CODEC_CODEC_RESAMPLE_ADC_ENABLE                     (1 << 3)
#define CODEC_CODEC_RESAMPLE_ADC_CH_CNT(n)                  (((n) & 0x7) << 4)
#define CODEC_CODEC_RESAMPLE_ADC_CH_CNT_MASK                (0x7 << 4)
#define CODEC_CODEC_RESAMPLE_ADC_CH_CNT_SHIFT               (4)
#define CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE               (1 << 7)
#define CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL(n)      (((n) & 0x3) << 8)
#define CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL_MASK    (0x3 << 8)
#define CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL_SHIFT   (8)
#define CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE               (1 << 10)
#define CODEC_CODEC_RESAMPLE_ADC_UPDATE_TRIGGER_SEL(n)      (((n) & 0x3) << 11)
#define CODEC_CODEC_RESAMPLE_ADC_UPDATE_TRIGGER_SEL_MASK    (0x3 << 11)
#define CODEC_CODEC_RESAMPLE_ADC_UPDATE_TRIGGER_SEL_SHIFT   (11)
#define CODEC_CODEC_RESAMPLE_DAC_ENABLE_TRIGGER_SEL(n)      (((n) & 0x3) << 13)
#define CODEC_CODEC_RESAMPLE_DAC_ENABLE_TRIGGER_SEL_MASK    (0x3 << 13)
#define CODEC_CODEC_RESAMPLE_DAC_ENABLE_TRIGGER_SEL_SHIFT   (13)
#define CODEC_CODEC_RESAMPLE_ADC_ENABLE_TRIGGER_SEL(n)      (((n) & 0x3) << 15)
#define CODEC_CODEC_RESAMPLE_ADC_ENABLE_TRIGGER_SEL_MASK    (0x3 << 15)
#define CODEC_CODEC_RESAMPLE_ADC_ENABLE_TRIGGER_SEL_SHIFT   (15)
#define CODEC_CODEC_RESAMPLE_DAC_ENABLE_SND                 (1 << 17)
#define CODEC_CODEC_RESAMPLE_DAC_L_ENABLE_SND               (1 << 18)
#define CODEC_CODEC_RESAMPLE_DAC_FIFO_ENABLE_SND            (1 << 19)
#define CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_SND           (1 << 20)
#define CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL_SND(n)  (((n) & 0x3) << 21)
#define CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL_SND_MASK (0x3 << 21)
#define CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL_SND_SHIFT (21)
#define CODEC_CODEC_RESAMPLE_DAC_ENABLE_TRIGGER_SEL_SND(n)  (((n) & 0x3) << 23)
#define CODEC_CODEC_RESAMPLE_DAC_ENABLE_TRIGGER_SEL_SND_MASK (0x3 << 23)
#define CODEC_CODEC_RESAMPLE_DAC_ENABLE_TRIGGER_SEL_SND_SHIFT (23)
#define CODEC_CODEC_ADC_REMAP_ENABLE                        (1 << 25)
#define CODEC_CODEC_RESAMPLE_DAC_FIFO_DELAY(n)              (((n) & 0x7) << 26)
#define CODEC_CODEC_RESAMPLE_DAC_FIFO_DELAY_MASK            (0x7 << 26)
#define CODEC_CODEC_RESAMPLE_DAC_FIFO_DELAY_SHIFT           (26)
#define CODEC_CODEC_RESAMPLE_DAC_FIFO_DELAY_SND(n)          (((n) & 0x7) << 29)
#define CODEC_CODEC_RESAMPLE_DAC_FIFO_DELAY_SND_MASK        (0x7 << 29)
#define CODEC_CODEC_RESAMPLE_DAC_FIFO_DELAY_SND_SHIFT       (29)

// reg_e8
#define CODEC_CODEC_RESAMPLE_ADC_CH0_SEL(n)                 (((n) & 0xF) << 0)
#define CODEC_CODEC_RESAMPLE_ADC_CH0_SEL_MASK               (0xF << 0)
#define CODEC_CODEC_RESAMPLE_ADC_CH0_SEL_SHIFT              (0)
#define CODEC_CODEC_RESAMPLE_ADC_CH1_SEL(n)                 (((n) & 0xF) << 4)
#define CODEC_CODEC_RESAMPLE_ADC_CH1_SEL_MASK               (0xF << 4)
#define CODEC_CODEC_RESAMPLE_ADC_CH1_SEL_SHIFT              (4)
#define CODEC_CODEC_RESAMPLE_ADC_CH2_SEL(n)                 (((n) & 0xF) << 8)
#define CODEC_CODEC_RESAMPLE_ADC_CH2_SEL_MASK               (0xF << 8)
#define CODEC_CODEC_RESAMPLE_ADC_CH2_SEL_SHIFT              (8)
#define CODEC_CODEC_RESAMPLE_ADC_CH3_SEL(n)                 (((n) & 0xF) << 12)
#define CODEC_CODEC_RESAMPLE_ADC_CH3_SEL_MASK               (0xF << 12)
#define CODEC_CODEC_RESAMPLE_ADC_CH3_SEL_SHIFT              (12)
#define CODEC_CODEC_RESAMPLE_ADC_FIFO_ENABLE                (1 << 16)
#define CODEC_CODEC_RESAMPLE_ADC_FIFO_DELAY(n)              (((n) & 0x7) << 17)
#define CODEC_CODEC_RESAMPLE_ADC_FIFO_DELAY_MASK            (0x7 << 17)
#define CODEC_CODEC_RESAMPLE_ADC_FIFO_DELAY_SHIFT           (17)

// reg_ec
#define CODEC_CODEC_RESAMPLE_DAC_PHASE_INC(n)               (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_RESAMPLE_DAC_PHASE_INC_MASK             (0xFFFFFFFF << 0)
#define CODEC_CODEC_RESAMPLE_DAC_PHASE_INC_SHIFT            (0)

// reg_f0
#define CODEC_CODEC_RESAMPLE_ADC_PHASE_INC(n)               (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_RESAMPLE_ADC_PHASE_INC_MASK             (0xFFFFFFFF << 0)
#define CODEC_CODEC_RESAMPLE_ADC_PHASE_INC_SHIFT            (0)

// reg_f4
#define CODEC_CODEC_RESAMPLE_DAC_PHASE_INC_SND(n)           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_RESAMPLE_DAC_PHASE_INC_SND_MASK         (0xFFFFFFFF << 0)
#define CODEC_CODEC_RESAMPLE_DAC_PHASE_INC_SND_SHIFT        (0)

// reg_100
#define CODEC_FIR_STREAM_ENABLE_CH0                         (1 << 0)
#define CODEC_FIR_STREAM_ENABLE_CH1                         (1 << 1)
#define CODEC_FIR_STREAM_ENABLE_CH2                         (1 << 2)
#define CODEC_FIR_STREAM_ENABLE_CH3                         (1 << 3)
#define CODEC_FIR_ENABLE_CH0                                (1 << 4)
#define CODEC_FIR_ENABLE_CH1                                (1 << 5)
#define CODEC_FIR_ENABLE_CH2                                (1 << 6)
#define CODEC_FIR_ENABLE_CH3                                (1 << 7)
#define CODEC_DMACTRL_RX_FIR                                (1 << 8)
#define CODEC_DMACTRL_TX_FIR                                (1 << 9)
#define CODEC_FIR_UPSAMPLE_CH0                              (1 << 10)
#define CODEC_FIR_UPSAMPLE_CH1                              (1 << 11)
#define CODEC_FIR_UPSAMPLE_CH2                              (1 << 12)
#define CODEC_FIR_UPSAMPLE_CH3                              (1 << 13)
#define CODEC_MODE_32BIT_FIR                                (1 << 14)
#define CODEC_FIR_RESERVED_REG0                             (1 << 15)
#define CODEC_MODE_16BIT_FIR_TX_CH0                         (1 << 16)
#define CODEC_MODE_16BIT_FIR_RX_CH0                         (1 << 17)
#define CODEC_MODE_16BIT_FIR_TX_CH1                         (1 << 18)
#define CODEC_MODE_16BIT_FIR_RX_CH1                         (1 << 19)
#define CODEC_MODE_16BIT_FIR_TX_CH2                         (1 << 20)
#define CODEC_MODE_16BIT_FIR_RX_CH2                         (1 << 21)
#define CODEC_MODE_16BIT_FIR_TX_CH3                         (1 << 22)
#define CODEC_MODE_16BIT_FIR_RX_CH3                         (1 << 23)
#define CODEC_FIR_FULL_OUT_CH0                              (1 << 24)
#define CODEC_FIR_FULL_OUT_CH1                              (1 << 25)
#define CODEC_FIR_FULL_OUT_CH2                              (1 << 26)
#define CODEC_FIR_FULL_OUT_CH3                              (1 << 27)

// reg_104
#define CODEC_FIR_ACCESS_OFFSET_CH0(n)                      (((n) & 0x7) << 0)
#define CODEC_FIR_ACCESS_OFFSET_CH0_MASK                    (0x7 << 0)
#define CODEC_FIR_ACCESS_OFFSET_CH0_SHIFT                   (0)
#define CODEC_FIR_ACCESS_OFFSET_CH1(n)                      (((n) & 0x7) << 3)
#define CODEC_FIR_ACCESS_OFFSET_CH1_MASK                    (0x7 << 3)
#define CODEC_FIR_ACCESS_OFFSET_CH1_SHIFT                   (3)
#define CODEC_FIR_ACCESS_OFFSET_CH2(n)                      (((n) & 0x7) << 6)
#define CODEC_FIR_ACCESS_OFFSET_CH2_MASK                    (0x7 << 6)
#define CODEC_FIR_ACCESS_OFFSET_CH2_SHIFT                   (6)
#define CODEC_FIR_ACCESS_OFFSET_CH3(n)                      (((n) & 0x7) << 9)
#define CODEC_FIR_ACCESS_OFFSET_CH3_MASK                    (0x7 << 9)
#define CODEC_FIR_ACCESS_OFFSET_CH3_SHIFT                   (9)
#define CODEC_PDU_FS_SWAP                                   (1 << 12)
#define CODEC_ANC_COEF_SEL_FIR_CH0                          (1 << 13)
#define CODEC_ANC_COEF_SEL_FIR_CH1                          (1 << 14)
#define CODEC_ANC_COEF_SEL_FIR_CH2                          (1 << 15)
#define CODEC_ANC_COEF_SEL_FIR_CH3                          (1 << 16)
#define CODEC_ANC_COEF_SWITCH_DIN0_SEL                      (1 << 17)
#define CODEC_ANC_COEF_SWITCH_MODE_CH0                      (1 << 18)
#define CODEC_ANC_COEF_SWITCH_MODE_CH1                      (1 << 19)
#define CODEC_ANC_COEF_SWITCH_MODE_CH2                      (1 << 20)
#define CODEC_ANC_COEF_SWITCH_MODE_CH3                      (1 << 21)
#define CODEC_ANC_COEF_SEL_FIR_CH0_NEW_SYNC_1               (1 << 22)
#define CODEC_ANC_COEF_SEL_FIR_CH1_NEW_SYNC_1               (1 << 23)
#define CODEC_ANC_COEF_SEL_FIR_CH2_NEW_SYNC_1               (1 << 24)
#define CODEC_ANC_COEF_SEL_FIR_CH3_NEW_SYNC_1               (1 << 25)

// reg_108
#define CODEC_STREAM0_FIR1_CH0                              (1 << 0)
#define CODEC_FIR_MODE_CH0(n)                               (((n) & 0x3) << 1)
#define CODEC_FIR_MODE_CH0_MASK                             (0x3 << 1)
#define CODEC_FIR_MODE_CH0_SHIFT                            (1)
#define CODEC_FIR_ORDER_CH0(n)                              (((n) & 0x3FF) << 3)
#define CODEC_FIR_ORDER_CH0_MASK                            (0x3FF << 3)
#define CODEC_FIR_ORDER_CH0_SHIFT                           (3)
#define CODEC_FIR_SAMPLE_START_CH0(n)                       (((n) & 0x1FF) << 13)
#define CODEC_FIR_SAMPLE_START_CH0_MASK                     (0x1FF << 13)
#define CODEC_FIR_SAMPLE_START_CH0_SHIFT                    (13)
#define CODEC_FIR_SAMPLE_NUM_CH0(n)                         (((n) & 0x1FF) << 22)
#define CODEC_FIR_SAMPLE_NUM_CH0_MASK                       (0x1FF << 22)
#define CODEC_FIR_SAMPLE_NUM_CH0_SHIFT                      (22)
#define CODEC_FIR_DO_REMAP_CH0                              (1 << 31)

// reg_10c
#define CODEC_FIR_RESULT_BASE_ADDR_CH0(n)                   (((n) & 0x1FF) << 0)
#define CODEC_FIR_RESULT_BASE_ADDR_CH0_MASK                 (0x1FF << 0)
#define CODEC_FIR_RESULT_BASE_ADDR_CH0_SHIFT                (0)
#define CODEC_FIR_SLIDE_OFFSET_CH0(n)                       (((n) & 0x3F) << 9)
#define CODEC_FIR_SLIDE_OFFSET_CH0_MASK                     (0x3F << 9)
#define CODEC_FIR_SLIDE_OFFSET_CH0_SHIFT                    (9)
#define CODEC_FIR_BURST_LENGTH_CH0(n)                       (((n) & 0x3F) << 15)
#define CODEC_FIR_BURST_LENGTH_CH0_MASK                     (0x3F << 15)
#define CODEC_FIR_BURST_LENGTH_CH0_SHIFT                    (15)
#define CODEC_FIR_GAIN_SEL_CH0(n)                           (((n) & 0xF) << 21)
#define CODEC_FIR_GAIN_SEL_CH0_MASK                         (0xF << 21)
#define CODEC_FIR_GAIN_SEL_CH0_SHIFT                        (21)
#define CODEC_FIR_LOOP_NUM_CH0(n)                           (((n) & 0x7F) << 25)
#define CODEC_FIR_LOOP_NUM_CH0_MASK                         (0x7F << 25)
#define CODEC_FIR_LOOP_NUM_CH0_SHIFT                        (25)

// reg_110
#define CODEC_STREAM0_FIR1_CH1                              (1 << 0)
#define CODEC_FIR_MODE_CH1(n)                               (((n) & 0x3) << 1)
#define CODEC_FIR_MODE_CH1_MASK                             (0x3 << 1)
#define CODEC_FIR_MODE_CH1_SHIFT                            (1)
#define CODEC_FIR_ORDER_CH1(n)                              (((n) & 0x3FF) << 3)
#define CODEC_FIR_ORDER_CH1_MASK                            (0x3FF << 3)
#define CODEC_FIR_ORDER_CH1_SHIFT                           (3)
#define CODEC_FIR_SAMPLE_START_CH1(n)                       (((n) & 0x1FF) << 13)
#define CODEC_FIR_SAMPLE_START_CH1_MASK                     (0x1FF << 13)
#define CODEC_FIR_SAMPLE_START_CH1_SHIFT                    (13)
#define CODEC_FIR_SAMPLE_NUM_CH1(n)                         (((n) & 0x1FF) << 22)
#define CODEC_FIR_SAMPLE_NUM_CH1_MASK                       (0x1FF << 22)
#define CODEC_FIR_SAMPLE_NUM_CH1_SHIFT                      (22)
#define CODEC_FIR_DO_REMAP_CH1                              (1 << 31)

// reg_114
#define CODEC_FIR_RESULT_BASE_ADDR_CH1(n)                   (((n) & 0x1FF) << 0)
#define CODEC_FIR_RESULT_BASE_ADDR_CH1_MASK                 (0x1FF << 0)
#define CODEC_FIR_RESULT_BASE_ADDR_CH1_SHIFT                (0)
#define CODEC_FIR_SLIDE_OFFSET_CH1(n)                       (((n) & 0x3F) << 9)
#define CODEC_FIR_SLIDE_OFFSET_CH1_MASK                     (0x3F << 9)
#define CODEC_FIR_SLIDE_OFFSET_CH1_SHIFT                    (9)
#define CODEC_FIR_BURST_LENGTH_CH1(n)                       (((n) & 0x3F) << 15)
#define CODEC_FIR_BURST_LENGTH_CH1_MASK                     (0x3F << 15)
#define CODEC_FIR_BURST_LENGTH_CH1_SHIFT                    (15)
#define CODEC_FIR_GAIN_SEL_CH1(n)                           (((n) & 0xF) << 21)
#define CODEC_FIR_GAIN_SEL_CH1_MASK                         (0xF << 21)
#define CODEC_FIR_GAIN_SEL_CH1_SHIFT                        (21)
#define CODEC_FIR_LOOP_NUM_CH1(n)                           (((n) & 0x7F) << 25)
#define CODEC_FIR_LOOP_NUM_CH1_MASK                         (0x7F << 25)
#define CODEC_FIR_LOOP_NUM_CH1_SHIFT                        (25)

// reg_118
#define CODEC_STREAM0_FIR1_CH2                              (1 << 0)
#define CODEC_FIR_MODE_CH2(n)                               (((n) & 0x3) << 1)
#define CODEC_FIR_MODE_CH2_MASK                             (0x3 << 1)
#define CODEC_FIR_MODE_CH2_SHIFT                            (1)
#define CODEC_FIR_ORDER_CH2(n)                              (((n) & 0x3FF) << 3)
#define CODEC_FIR_ORDER_CH2_MASK                            (0x3FF << 3)
#define CODEC_FIR_ORDER_CH2_SHIFT                           (3)
#define CODEC_FIR_SAMPLE_START_CH2(n)                       (((n) & 0x1FF) << 13)
#define CODEC_FIR_SAMPLE_START_CH2_MASK                     (0x1FF << 13)
#define CODEC_FIR_SAMPLE_START_CH2_SHIFT                    (13)
#define CODEC_FIR_SAMPLE_NUM_CH2(n)                         (((n) & 0x1FF) << 22)
#define CODEC_FIR_SAMPLE_NUM_CH2_MASK                       (0x1FF << 22)
#define CODEC_FIR_SAMPLE_NUM_CH2_SHIFT                      (22)
#define CODEC_FIR_DO_REMAP_CH2                              (1 << 31)

// reg_11c
#define CODEC_FIR_RESULT_BASE_ADDR_CH2(n)                   (((n) & 0x1FF) << 0)
#define CODEC_FIR_RESULT_BASE_ADDR_CH2_MASK                 (0x1FF << 0)
#define CODEC_FIR_RESULT_BASE_ADDR_CH2_SHIFT                (0)
#define CODEC_FIR_SLIDE_OFFSET_CH2(n)                       (((n) & 0x3F) << 9)
#define CODEC_FIR_SLIDE_OFFSET_CH2_MASK                     (0x3F << 9)
#define CODEC_FIR_SLIDE_OFFSET_CH2_SHIFT                    (9)
#define CODEC_FIR_BURST_LENGTH_CH2(n)                       (((n) & 0x3F) << 15)
#define CODEC_FIR_BURST_LENGTH_CH2_MASK                     (0x3F << 15)
#define CODEC_FIR_BURST_LENGTH_CH2_SHIFT                    (15)
#define CODEC_FIR_GAIN_SEL_CH2(n)                           (((n) & 0xF) << 21)
#define CODEC_FIR_GAIN_SEL_CH2_MASK                         (0xF << 21)
#define CODEC_FIR_GAIN_SEL_CH2_SHIFT                        (21)
#define CODEC_FIR_LOOP_NUM_CH2(n)                           (((n) & 0x7F) << 25)
#define CODEC_FIR_LOOP_NUM_CH2_MASK                         (0x7F << 25)
#define CODEC_FIR_LOOP_NUM_CH2_SHIFT                        (25)

// reg_120
#define CODEC_STREAM0_FIR1_CH3                              (1 << 0)
#define CODEC_FIR_MODE_CH3(n)                               (((n) & 0x3) << 1)
#define CODEC_FIR_MODE_CH3_MASK                             (0x3 << 1)
#define CODEC_FIR_MODE_CH3_SHIFT                            (1)
#define CODEC_FIR_ORDER_CH3(n)                              (((n) & 0x3FF) << 3)
#define CODEC_FIR_ORDER_CH3_MASK                            (0x3FF << 3)
#define CODEC_FIR_ORDER_CH3_SHIFT                           (3)
#define CODEC_FIR_SAMPLE_START_CH3(n)                       (((n) & 0x1FF) << 13)
#define CODEC_FIR_SAMPLE_START_CH3_MASK                     (0x1FF << 13)
#define CODEC_FIR_SAMPLE_START_CH3_SHIFT                    (13)
#define CODEC_FIR_SAMPLE_NUM_CH3(n)                         (((n) & 0x1FF) << 22)
#define CODEC_FIR_SAMPLE_NUM_CH3_MASK                       (0x1FF << 22)
#define CODEC_FIR_SAMPLE_NUM_CH3_SHIFT                      (22)
#define CODEC_FIR_DO_REMAP_CH3                              (1 << 31)

// reg_124
#define CODEC_FIR_RESULT_BASE_ADDR_CH3(n)                   (((n) & 0x1FF) << 0)
#define CODEC_FIR_RESULT_BASE_ADDR_CH3_MASK                 (0x1FF << 0)
#define CODEC_FIR_RESULT_BASE_ADDR_CH3_SHIFT                (0)
#define CODEC_FIR_SLIDE_OFFSET_CH3(n)                       (((n) & 0x3F) << 9)
#define CODEC_FIR_SLIDE_OFFSET_CH3_MASK                     (0x3F << 9)
#define CODEC_FIR_SLIDE_OFFSET_CH3_SHIFT                    (9)
#define CODEC_FIR_BURST_LENGTH_CH3(n)                       (((n) & 0x3F) << 15)
#define CODEC_FIR_BURST_LENGTH_CH3_MASK                     (0x3F << 15)
#define CODEC_FIR_BURST_LENGTH_CH3_SHIFT                    (15)
#define CODEC_FIR_GAIN_SEL_CH3(n)                           (((n) & 0xF) << 21)
#define CODEC_FIR_GAIN_SEL_CH3_MASK                         (0xF << 21)
#define CODEC_FIR_GAIN_SEL_CH3_SHIFT                        (21)
#define CODEC_FIR_LOOP_NUM_CH3(n)                           (((n) & 0x7F) << 25)
#define CODEC_FIR_LOOP_NUM_CH3_MASK                         (0x7F << 25)
#define CODEC_FIR_LOOP_NUM_CH3_SHIFT                        (25)

// reg_128
#define CODEC_FIR_CH0_STATE(n)                              (((n) & 0xFF) << 0)
#define CODEC_FIR_CH0_STATE_MASK                            (0xFF << 0)
#define CODEC_FIR_CH0_STATE_SHIFT                           (0)
#define CODEC_FIR_CH1_STATE(n)                              (((n) & 0xFF) << 8)
#define CODEC_FIR_CH1_STATE_MASK                            (0xFF << 8)
#define CODEC_FIR_CH1_STATE_SHIFT                           (8)
#define CODEC_FIR_CH2_STATE(n)                              (((n) & 0xFF) << 16)
#define CODEC_FIR_CH2_STATE_MASK                            (0xFF << 16)
#define CODEC_FIR_CH2_STATE_SHIFT                           (16)
#define CODEC_FIR_CH3_STATE(n)                              (((n) & 0xFF) << 24)
#define CODEC_FIR_CH3_STATE_MASK                            (0xFF << 24)
#define CODEC_FIR_CH3_STATE_SHIFT                           (24)

// reg_12c
#define CODEC_FIR_COMB_MST_EN_CH0                           (1 << 0)
#define CODEC_FIR_COMB_MST_EN_CH1                           (1 << 1)
#define CODEC_FIR_COMB_MST_EN_CH2                           (1 << 2)
#define CODEC_FIR_COMB_MST_EN_CH3                           (1 << 3)
#define CODEC_FIR_COMB_CNT_CH0(n)                           (((n) & 0x3) << 4)
#define CODEC_FIR_COMB_CNT_CH0_MASK                         (0x3 << 4)
#define CODEC_FIR_COMB_CNT_CH0_SHIFT                        (4)
#define CODEC_FIR_COMB_CNT_CH2(n)                           (((n) & 0x3) << 6)
#define CODEC_FIR_COMB_CNT_CH2_MASK                         (0x3 << 6)
#define CODEC_FIR_COMB_CNT_CH2_SHIFT                        (6)
#define CODEC_FIR_SAMPLE_WRAP_MODE_CH0                      (1 << 8)
#define CODEC_FIR_SAMPLE_WRAP_MODE_CH1                      (1 << 9)
#define CODEC_FIR_SAMPLE_WRAP_MODE_CH2                      (1 << 10)
#define CODEC_FIR_SAMPLE_WRAP_MODE_CH3                      (1 << 11)
#define CODEC_FIR_DMA_IN_END_MODE                           (1 << 12)
#define CODEC_FIR_MEM_LP_MODE                               (1 << 13)

// reg_130
#define CODEC_CODEC_FB_CHECK_ENABLE_CH0                     (1 << 0)
#define CODEC_CODEC_FB_CHECK_ACC_SAMPLE_RATE_CH0(n)         (((n) & 0x3) << 1)
#define CODEC_CODEC_FB_CHECK_ACC_SAMPLE_RATE_CH0_MASK       (0x3 << 1)
#define CODEC_CODEC_FB_CHECK_ACC_SAMPLE_RATE_CH0_SHIFT      (1)
#define CODEC_CODEC_FB_CHECK_SRC_SEL_CH0(n)                 (((n) & 0x3) << 3)
#define CODEC_CODEC_FB_CHECK_SRC_SEL_CH0_MASK               (0x3 << 3)
#define CODEC_CODEC_FB_CHECK_SRC_SEL_CH0_SHIFT              (3)
#define CODEC_CODEC_FB_CHECK_KEEP_SEL_CH0                   (1 << 5)
#define CODEC_CODEC_FB_CHECK_ACC_WINDOW_CH0(n)              (((n) & 0xFFF) << 6)
#define CODEC_CODEC_FB_CHECK_ACC_WINDOW_CH0_MASK            (0xFFF << 6)
#define CODEC_CODEC_FB_CHECK_ACC_WINDOW_CH0_SHIFT           (6)
#define CODEC_CODEC_FB_CHECK_TRIG_WINDOW_CH0(n)             (((n) & 0x3FF) << 18)
#define CODEC_CODEC_FB_CHECK_TRIG_WINDOW_CH0_MASK           (0x3FF << 18)
#define CODEC_CODEC_FB_CHECK_TRIG_WINDOW_CH0_SHIFT          (18)
#define CODEC_CODEC_FB_CHECK_KEEP_CH0                       (1 << 28)
#define CODEC_CODEC_FB_CHECK_DCF_BYPASS_CH0                 (1 << 29)

// reg_138
#define CODEC_CODEC_FB_CHECK_THRESHOLD_CH0(n)               (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_FB_CHECK_THRESHOLD_CH0_MASK             (0xFFFFFFFF << 0)
#define CODEC_CODEC_FB_CHECK_THRESHOLD_CH0_SHIFT            (0)

// reg_140
#define CODEC_CODEC_FB_CHECK_DATA_AVG_KEEP_CH0(n)           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_FB_CHECK_DATA_AVG_KEEP_CH0_MASK         (0xFFFFFFFF << 0)
#define CODEC_CODEC_FB_CHECK_DATA_AVG_KEEP_CH0_SHIFT        (0)

// reg_148
#define CODEC_CODEC_CLASSG_EN                               (1 << 0)
#define CODEC_CODEC_CLASSG_QUICK_DOWN                       (1 << 1)
#define CODEC_CODEC_CLASSG_STEP_4_3N                        (1 << 2)
#define CODEC_CODEC_CLASSG_LR                               (1 << 3)
#define CODEC_CODEC_CLASSG_WINDOW(n)                        (((n) & 0xFFF) << 4)
#define CODEC_CODEC_CLASSG_WINDOW_MASK                      (0xFFF << 4)
#define CODEC_CODEC_CLASSG_WINDOW_SHIFT                     (4)

// reg_14c
#define CODEC_CODEC_CLASSG_THD0(n)                          (((n) & 0xFF) << 0)
#define CODEC_CODEC_CLASSG_THD0_MASK                        (0xFF << 0)
#define CODEC_CODEC_CLASSG_THD0_SHIFT                       (0)
#define CODEC_CODEC_CLASSG_THD1(n)                          (((n) & 0xFF) << 8)
#define CODEC_CODEC_CLASSG_THD1_MASK                        (0xFF << 8)
#define CODEC_CODEC_CLASSG_THD1_SHIFT                       (8)
#define CODEC_CODEC_CLASSG_THD2(n)                          (((n) & 0xFF) << 16)
#define CODEC_CODEC_CLASSG_THD2_MASK                        (0xFF << 16)
#define CODEC_CODEC_CLASSG_THD2_SHIFT                       (16)

// reg_154
#define CODEC_CODEC_RAMP_STEP_CH0(n)                        (((n) & 0xFFF) << 0)
#define CODEC_CODEC_RAMP_STEP_CH0_MASK                      (0xFFF << 0)
#define CODEC_CODEC_RAMP_STEP_CH0_SHIFT                     (0)
#define CODEC_CODEC_RAMP_EN_CH0                             (1 << 12)
#define CODEC_CODEC_RAMP_INTERVAL_CH0(n)                    (((n) & 0x7) << 13)
#define CODEC_CODEC_RAMP_INTERVAL_CH0_MASK                  (0x7 << 13)
#define CODEC_CODEC_RAMP_INTERVAL_CH0_SHIFT                 (13)

// reg_15c
#define CODEC_CODEC_DAC_EN_SND                              (1 << 0)
#define CODEC_CODEC_DAC_EN_SND_CH0                          (1 << 1)
#define CODEC_CODEC_DAC_HBF3_BYPASS_SND                     (1 << 2)
#define CODEC_CODEC_DAC_HBF2_BYPASS_SND                     (1 << 3)
#define CODEC_CODEC_DAC_HBF1_BYPASS_SND                     (1 << 4)
#define CODEC_CODEC_DAC_UP_SEL_SND(n)                       (((n) & 0x7) << 5)
#define CODEC_CODEC_DAC_UP_SEL_SND_MASK                     (0x7 << 5)
#define CODEC_CODEC_DAC_UP_SEL_SND_SHIFT                    (5)
#define CODEC_CODEC_DAC_TONE_TEST_SND                       (1 << 8)
#define CODEC_CODEC_DAC_SIN1K_SEL                           (1 << 9)

// reg_160
#define CODEC_CODEC_DAC_GAIN_SND_CH0(n)                     (((n) & 0xFFFFF) << 0)
#define CODEC_CODEC_DAC_GAIN_SND_CH0_MASK                   (0xFFFFF << 0)
#define CODEC_CODEC_DAC_GAIN_SND_CH0_SHIFT                  (0)
#define CODEC_CODEC_DAC_GAIN_SEL_SND_CH0                    (1 << 20)
#define CODEC_CODEC_DAC_GAIN_UPDATE_SND                     (1 << 21)
#define CODEC_CODEC_DAC_GAIN_TRIGGER_SEL_SND(n)             (((n) & 0x3) << 22)
#define CODEC_CODEC_DAC_GAIN_TRIGGER_SEL_SND_MASK           (0x3 << 22)
#define CODEC_CODEC_DAC_GAIN_TRIGGER_SEL_SND_SHIFT          (22)

// reg_168
#define CODEC_CODEC_RAMP_STEP_SND_CH0(n)                    (((n) & 0xFFF) << 0)
#define CODEC_CODEC_RAMP_STEP_SND_CH0_MASK                  (0xFFF << 0)
#define CODEC_CODEC_RAMP_STEP_SND_CH0_SHIFT                 (0)
#define CODEC_CODEC_RAMP_EN_SND_CH0                         (1 << 12)
#define CODEC_CODEC_RAMP_INTERVAL_SND_CH0(n)                (((n) & 0x7) << 13)
#define CODEC_CODEC_RAMP_INTERVAL_SND_CH0_MASK              (0x7 << 13)
#define CODEC_CODEC_RAMP_INTERVAL_SND_CH0_SHIFT             (13)

// reg_16c
#define CODEC_CODEC_IIR_GC_DBG_OUT_SYNC(n)                  (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR_GC_DBG_OUT_SYNC_MASK                (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR_GC_DBG_OUT_SYNC_SHIFT               (0)

// reg_170
#define CODEC_CODEC_IIR_GS_DBG_OUT_SYNC(n)                  (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR_GS_DBG_OUT_SYNC_MASK                (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR_GS_DBG_OUT_SYNC_SHIFT               (0)

// reg_174
#define CODEC_CODEC_ADC_DC_DOUT_CH0_SYNC(n)                 (((n) & 0x1FFFFF) << 0)
#define CODEC_CODEC_ADC_DC_DOUT_CH0_SYNC_MASK               (0x1FFFFF << 0)
#define CODEC_CODEC_ADC_DC_DOUT_CH0_SYNC_SHIFT              (0)

// reg_178
#define CODEC_CODEC_ADC_DC_DOUT_CH1_SYNC(n)                 (((n) & 0x1FFFFF) << 0)
#define CODEC_CODEC_ADC_DC_DOUT_CH1_SYNC_MASK               (0x1FFFFF << 0)
#define CODEC_CODEC_ADC_DC_DOUT_CH1_SYNC_SHIFT              (0)

// reg_17c
#define CODEC_CODEC_ADC_DC_DOUT_CH2_SYNC(n)                 (((n) & 0x1FFFFF) << 0)
#define CODEC_CODEC_ADC_DC_DOUT_CH2_SYNC_MASK               (0x1FFFFF << 0)
#define CODEC_CODEC_ADC_DC_DOUT_CH2_SYNC_SHIFT              (0)

// reg_188
#define CODEC_CODEC_ADC_DC_DIN_CH0(n)                       (((n) & 0xFFFFF) << 0)
#define CODEC_CODEC_ADC_DC_DIN_CH0_MASK                     (0xFFFFF << 0)
#define CODEC_CODEC_ADC_DC_DIN_CH0_SHIFT                    (0)
#define CODEC_CODEC_ADC_DC_UPDATE_CH0                       (1 << 20)

// reg_18c
#define CODEC_CODEC_ADC_DC_DIN_CH1(n)                       (((n) & 0xFFFFF) << 0)
#define CODEC_CODEC_ADC_DC_DIN_CH1_MASK                     (0xFFFFF << 0)
#define CODEC_CODEC_ADC_DC_DIN_CH1_SHIFT                    (0)
#define CODEC_CODEC_ADC_DC_UPDATE_CH1                       (1 << 20)

// reg_190
#define CODEC_CODEC_ADC_DC_DIN_CH2(n)                       (((n) & 0xFFFFF) << 0)
#define CODEC_CODEC_ADC_DC_DIN_CH2_MASK                     (0xFFFFF << 0)
#define CODEC_CODEC_ADC_DC_DIN_CH2_SHIFT                    (0)
#define CODEC_CODEC_ADC_DC_UPDATE_CH2                       (1 << 20)

// reg_1a4
#define CODEC_ANC_COEF_SWITCH_DELAY_CH0(n)                  (((n) & 0x3FF) << 0)
#define CODEC_ANC_COEF_SWITCH_DELAY_CH0_MASK                (0x3FF << 0)
#define CODEC_ANC_COEF_SWITCH_DELAY_CH0_SHIFT               (0)
#define CODEC_ANC_COEF_SWITCH_DELAY_CH1(n)                  (((n) & 0x3FF) << 10)
#define CODEC_ANC_COEF_SWITCH_DELAY_CH1_MASK                (0x3FF << 10)
#define CODEC_ANC_COEF_SWITCH_DELAY_CH1_SHIFT               (10)
#define CODEC_ANC_COEF_SWITCH_REQ_CH0                       (1 << 20)
#define CODEC_ANC_COEF_SWITCH_REQ_CH1                       (1 << 21)
#define CODEC_ANC_COEF_SWITCH_REQ_CH2                       (1 << 22)
#define CODEC_ANC_COEF_SWITCH_REQ_CH3                       (1 << 23)
#define CODEC_ANC_COEF_SWITCH_ACK_CH0_SYNC_1                (1 << 24)
#define CODEC_ANC_COEF_SWITCH_ACK_CH1_SYNC_1                (1 << 25)
#define CODEC_ANC_COEF_SWITCH_ACK_CH2_SYNC_1                (1 << 26)
#define CODEC_ANC_COEF_SWITCH_ACK_CH3_SYNC_1                (1 << 27)

// reg_1a8
#define CODEC_ANC_COEF_SWITCH_DELAY_CH2(n)                  (((n) & 0x3FF) << 0)
#define CODEC_ANC_COEF_SWITCH_DELAY_CH2_MASK                (0x3FF << 0)
#define CODEC_ANC_COEF_SWITCH_DELAY_CH2_SHIFT               (0)
#define CODEC_ANC_COEF_SWITCH_DELAY_CH3(n)                  (((n) & 0x3FF) << 10)
#define CODEC_ANC_COEF_SWITCH_DELAY_CH3_MASK                (0x3FF << 10)
#define CODEC_ANC_COEF_SWITCH_DELAY_CH3_SHIFT               (10)

// reg_1b8
#define CODEC_CODEC_DAC_DC_CH0(n)                           (((n) & 0x7FFFF) << 0)
#define CODEC_CODEC_DAC_DC_CH0_MASK                         (0x7FFFF << 0)
#define CODEC_CODEC_DAC_DC_CH0_SHIFT                        (0)
#define CODEC_CODEC_DAC_DC_UPDATE_CH0                       (1 << 19)
#define CODEC_CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH0(n)        (((n) & 0xFF) << 20)
#define CODEC_CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH0_MASK      (0xFF << 20)
#define CODEC_CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH0_SHIFT     (20)
#define CODEC_CODEC_DAC_DC_UPDATE_PASS0_CH0                 (1 << 28)
#define CODEC_CODEC_DAC_DC_UPDATE_STATUS_CH0                (1 << 29)

// reg_1c0
#define CODEC_CODEC_ADC_DRE_ENABLE_CH0                      (1 << 0)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH0(n)                (((n) & 0x3) << 1)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH0_MASK              (0x3 << 1)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH0_SHIFT             (1)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH0(n)            (((n) & 0xF) << 3)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH0_MASK          (0xF << 3)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH0_SHIFT         (3)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH0(n)             (((n) & 0xF) << 7)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH0_MASK           (0xF << 7)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH0_SHIFT          (7)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH0(n)                (((n) & 0x7) << 11)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH0_MASK              (0x7 << 11)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH0_SHIFT             (11)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH0(n)                (((n) & 0x1F) << 14)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH0_MASK              (0x1F << 14)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH0_SHIFT             (14)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_SIGN_CH0          (1 << 19)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH0(n)                  (((n) & 0x3) << 20)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH0_MASK                (0x3 << 20)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH0_SHIFT               (20)
#define CODEC_CODEC_ADC_DRE_OVERFLOW_MUTE_EN_CH0            (1 << 22)
#define CODEC_CODEC_ADC_DRE_MUTE_MODE_CH0                   (1 << 23)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH0(n)           (((n) & 0x3) << 24)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH0_MASK         (0x3 << 24)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH0_SHIFT        (24)
#define CODEC_CODEC_ADC_DRE_MUTE_STATUS_CH0                 (1 << 26)

// reg_1c4
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH0(n)                 (((n) & 0x7FF) << 0)
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH0_MASK               (0x7FF << 0)
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH0_SHIFT              (0)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH0(n)                   (((n) & 0xFFFFF) << 11)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH0_MASK                 (0xFFFFF << 11)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH0_SHIFT                (11)

// reg_1c8
#define CODEC_CODEC_ADC_DRE_ENABLE_CH1                      (1 << 0)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH1(n)                (((n) & 0x3) << 1)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH1_MASK              (0x3 << 1)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH1_SHIFT             (1)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH1(n)            (((n) & 0xF) << 3)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH1_MASK          (0xF << 3)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH1_SHIFT         (3)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH1(n)             (((n) & 0xF) << 7)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH1_MASK           (0xF << 7)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH1_SHIFT          (7)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH1(n)                (((n) & 0x7) << 11)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH1_MASK              (0x7 << 11)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH1_SHIFT             (11)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH1(n)                (((n) & 0x1F) << 14)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH1_MASK              (0x1F << 14)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH1_SHIFT             (14)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_SIGN_CH1          (1 << 19)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH1(n)                  (((n) & 0x3) << 20)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH1_MASK                (0x3 << 20)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH1_SHIFT               (20)
#define CODEC_CODEC_ADC_DRE_OVERFLOW_MUTE_EN_CH1            (1 << 22)
#define CODEC_CODEC_ADC_DRE_MUTE_MODE_CH1                   (1 << 23)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH1(n)           (((n) & 0x3) << 24)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH1_MASK         (0x3 << 24)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH1_SHIFT        (24)
#define CODEC_CODEC_ADC_DRE_MUTE_STATUS_CH1                 (1 << 26)

// reg_1cc
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH1(n)                 (((n) & 0x7FF) << 0)
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH1_MASK               (0x7FF << 0)
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH1_SHIFT              (0)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH1(n)                   (((n) & 0xFFFFF) << 11)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH1_MASK                 (0xFFFFF << 11)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH1_SHIFT                (11)

// reg_1d0
#define CODEC_CODEC_ADC_DRE_ENABLE_CH2                      (1 << 0)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH2(n)                (((n) & 0x3) << 1)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH2_MASK              (0x3 << 1)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH2_SHIFT             (1)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH2(n)            (((n) & 0xF) << 3)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH2_MASK          (0xF << 3)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH2_SHIFT         (3)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH2(n)             (((n) & 0xF) << 7)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH2_MASK           (0xF << 7)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH2_SHIFT          (7)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH2(n)                (((n) & 0x7) << 11)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH2_MASK              (0x7 << 11)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH2_SHIFT             (11)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH2(n)                (((n) & 0x1F) << 14)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH2_MASK              (0x1F << 14)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH2_SHIFT             (14)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_SIGN_CH2          (1 << 19)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH2(n)                  (((n) & 0x3) << 20)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH2_MASK                (0x3 << 20)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH2_SHIFT               (20)
#define CODEC_CODEC_ADC_DRE_OVERFLOW_MUTE_EN_CH2            (1 << 22)
#define CODEC_CODEC_ADC_DRE_MUTE_MODE_CH2                   (1 << 23)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH2(n)           (((n) & 0x3) << 24)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH2_MASK         (0x3 << 24)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH2_SHIFT        (24)
#define CODEC_CODEC_ADC_DRE_MUTE_STATUS_CH2                 (1 << 26)

// reg_1d4
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH2(n)                 (((n) & 0x7FF) << 0)
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH2_MASK               (0x7FF << 0)
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH2_SHIFT              (0)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH2(n)                   (((n) & 0xFFFFF) << 11)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH2_MASK                 (0xFFFFF << 11)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH2_SHIFT                (11)

// reg_1e0
#define CODEC_CODEC_ADC_DRE_GAIN_STEP0_CH0(n)               (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP0_CH0_MASK             (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP0_CH0_SHIFT            (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP1_CH0(n)               (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP1_CH0_MASK             (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP1_CH0_SHIFT            (14)

// reg_1e4
#define CODEC_CODEC_ADC_DRE_GAIN_STEP2_CH0(n)               (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP2_CH0_MASK             (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP2_CH0_SHIFT            (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP3_CH0(n)               (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP3_CH0_MASK             (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP3_CH0_SHIFT            (14)

// reg_1e8
#define CODEC_CODEC_ADC_DRE_GAIN_STEP4_CH0(n)               (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP4_CH0_MASK             (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP4_CH0_SHIFT            (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP5_CH0(n)               (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP5_CH0_MASK             (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP5_CH0_SHIFT            (14)

// reg_1ec
#define CODEC_CODEC_ADC_DRE_GAIN_STEP6_CH0(n)               (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP6_CH0_MASK             (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP6_CH0_SHIFT            (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP7_CH0(n)               (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP7_CH0_MASK             (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP7_CH0_SHIFT            (14)

// reg_1f0
#define CODEC_CODEC_ADC_DRE_GAIN_STEP8_CH0(n)               (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP8_CH0_MASK             (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP8_CH0_SHIFT            (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP9_CH0(n)               (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP9_CH0_MASK             (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP9_CH0_SHIFT            (14)

// reg_1f4
#define CODEC_CODEC_ADC_DRE_GAIN_STEP10_CH0(n)              (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP10_CH0_MASK            (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP10_CH0_SHIFT           (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP11_CH0(n)              (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP11_CH0_MASK            (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP11_CH0_SHIFT           (14)

// reg_1f8
#define CODEC_CODEC_ADC_DRE_GAIN_STEP12_CH0(n)              (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP12_CH0_MASK            (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP12_CH0_SHIFT           (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP13_CH0(n)              (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP13_CH0_MASK            (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP13_CH0_SHIFT           (14)

// reg_1fc
#define CODEC_CODEC_ADC_DRE_GAIN_STEP14_CH0(n)              (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP14_CH0_MASK            (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP14_CH0_SHIFT           (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP15_CH0(n)              (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP15_CH0_MASK            (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP15_CH0_SHIFT           (14)

// reg_200
#define CODEC_CODEC_ADC_DRE_GAIN_STEP0_CH1(n)               (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP0_CH1_MASK             (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP0_CH1_SHIFT            (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP1_CH1(n)               (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP1_CH1_MASK             (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP1_CH1_SHIFT            (14)

// reg_204
#define CODEC_CODEC_ADC_DRE_GAIN_STEP2_CH1(n)               (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP2_CH1_MASK             (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP2_CH1_SHIFT            (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP3_CH1(n)               (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP3_CH1_MASK             (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP3_CH1_SHIFT            (14)

// reg_208
#define CODEC_CODEC_ADC_DRE_GAIN_STEP4_CH1(n)               (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP4_CH1_MASK             (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP4_CH1_SHIFT            (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP5_CH1(n)               (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP5_CH1_MASK             (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP5_CH1_SHIFT            (14)

// reg_20c
#define CODEC_CODEC_ADC_DRE_GAIN_STEP6_CH1(n)               (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP6_CH1_MASK             (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP6_CH1_SHIFT            (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP7_CH1(n)               (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP7_CH1_MASK             (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP7_CH1_SHIFT            (14)

// reg_210
#define CODEC_CODEC_ADC_DRE_GAIN_STEP8_CH1(n)               (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP8_CH1_MASK             (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP8_CH1_SHIFT            (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP9_CH1(n)               (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP9_CH1_MASK             (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP9_CH1_SHIFT            (14)

// reg_214
#define CODEC_CODEC_ADC_DRE_GAIN_STEP10_CH1(n)              (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP10_CH1_MASK            (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP10_CH1_SHIFT           (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP11_CH1(n)              (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP11_CH1_MASK            (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP11_CH1_SHIFT           (14)

// reg_218
#define CODEC_CODEC_ADC_DRE_GAIN_STEP12_CH1(n)              (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP12_CH1_MASK            (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP12_CH1_SHIFT           (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP13_CH1(n)              (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP13_CH1_MASK            (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP13_CH1_SHIFT           (14)

// reg_21c
#define CODEC_CODEC_ADC_DRE_GAIN_STEP14_CH1(n)              (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP14_CH1_MASK            (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP14_CH1_SHIFT           (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP15_CH1(n)              (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP15_CH1_MASK            (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP15_CH1_SHIFT           (14)

// reg_220
#define CODEC_CODEC_IIR_RAMP_STEP(n)                        (((n) & 0x7FFFFFF) << 0)
#define CODEC_CODEC_IIR_RAMP_STEP_MASK                      (0x7FFFFFF << 0)
#define CODEC_CODEC_IIR_RAMP_STEP_SHIFT                     (0)
#define CODEC_CODEC_IIR_RAMP_INTERVAL(n)                    (((n) & 0x7) << 27)
#define CODEC_CODEC_IIR_RAMP_INTERVAL_MASK                  (0x7 << 27)
#define CODEC_CODEC_IIR_RAMP_INTERVAL_SHIFT                 (27)

// reg_224
#define CODEC_CODEC_IIR0_RAMP_EN                            (1 << 0)
#define CODEC_CODEC_IIR1_RAMP_EN                            (1 << 1)
#define CODEC_CODEC_IIR4_RAMP_EN                            (1 << 2)
#define CODEC_CODEC_IIR5_RAMP_EN                            (1 << 3)
#define CODEC_CODEC_IIR0_RAMP_CFG_UPDATE                    (1 << 4)
#define CODEC_CODEC_IIR1_RAMP_CFG_UPDATE                    (1 << 5)
#define CODEC_CODEC_IIR4_RAMP_CFG_UPDATE                    (1 << 6)
#define CODEC_CODEC_IIR5_RAMP_CFG_UPDATE                    (1 << 7)
#define CODEC_CODEC_IIR_LMT_DBG_SEL(n)                      (((n) & 0xF) << 8)
#define CODEC_CODEC_IIR_LMT_DBG_SEL_MASK                    (0xF << 8)
#define CODEC_CODEC_IIR_LMT_DBG_SEL_SHIFT                   (8)

// reg_228
#define CODEC_CODEC_DEQ_IIR_RAMP_STEP(n)                    (((n) & 0x7FFFFFF) << 0)
#define CODEC_CODEC_DEQ_IIR_RAMP_STEP_MASK                  (0x7FFFFFF << 0)
#define CODEC_CODEC_DEQ_IIR_RAMP_STEP_SHIFT                 (0)
#define CODEC_CODEC_DEQ_IIR_RAMP_EN_CH0                     (1 << 27)
#define CODEC_CODEC_DEQ_IIR_RAMP_EN_CH1                     (1 << 28)
#define CODEC_CODEC_DEQ_IIR_RAMP_INTERVAL(n)                (((n) & 0x7) << 29)
#define CODEC_CODEC_DEQ_IIR_RAMP_INTERVAL_MASK              (0x7 << 29)
#define CODEC_CODEC_DEQ_IIR_RAMP_INTERVAL_SHIFT             (29)

// reg_22c
#define CODEC_CODEC_TT_ENABLE_CH0                           (1 << 0)
#define CODEC_CODEC_TT_ADC_SEL_CH0(n)                       (((n) & 0x7) << 1)
#define CODEC_CODEC_TT_ADC_SEL_CH0_MASK                     (0x7 << 1)
#define CODEC_CODEC_TT_ADC_SEL_CH0_SHIFT                    (1)
#define CODEC_CODEC_MM_ENABLE_CH0                           (1 << 4)
#define CODEC_CODEC_MM_FIFO_EN_CH0                          (1 << 5)
#define CODEC_CODEC_MM_FIFO_BYPASS_CH0                      (1 << 6)
#define CODEC_CODEC_MM_DELAY_CH0(n)                         (((n) & 0x1F) << 7)
#define CODEC_CODEC_MM_DELAY_CH0_MASK                       (0x1F << 7)
#define CODEC_CODEC_MM_DELAY_CH0_SHIFT                      (7)
#define CODEC_CODEC_TT_IIR_SEL_CH0                          (1 << 12)
#define CODEC_CODEC_MM_IIR_SEL_CH0                          (1 << 13)

// reg_230
#define CODEC_CODEC_MUTE_GAIN_COEF_TT_CH0(n)                (((n) & 0xFFF) << 0)
#define CODEC_CODEC_MUTE_GAIN_COEF_TT_CH0_MASK              (0xFFF << 0)
#define CODEC_CODEC_MUTE_GAIN_COEF_TT_CH0_SHIFT             (0)
#define CODEC_CODEC_MUTE_GAIN_PASS0_TT_CH0                  (1 << 12)
#define CODEC_CODEC_MUTE_GAIN_UPDATE_TT_CH0                 (1 << 13)

// reg_234
#define CODEC_CODEC_MUTE_GAIN_COEF_MM_CH0(n)                (((n) & 0xFFF) << 0)
#define CODEC_CODEC_MUTE_GAIN_COEF_MM_CH0_MASK              (0xFFF << 0)
#define CODEC_CODEC_MUTE_GAIN_COEF_MM_CH0_SHIFT             (0)
#define CODEC_CODEC_MUTE_GAIN_PASS0_MM_CH0                  (1 << 12)
#define CODEC_CODEC_MUTE_GAIN_UPDATE_MM_CH0                 (1 << 13)

// reg_238
#define CODEC_CODEC_ANC_CALIB_GAIN_COEF_FF_CH0(n)           (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_ANC_CALIB_GAIN_COEF_FF_CH0_MASK         (0xFFFF << 0)
#define CODEC_CODEC_ANC_CALIB_GAIN_COEF_FF_CH0_SHIFT        (0)

// reg_23c
#define CODEC_CODEC_ANC_CALIB_GAIN_COEF_FB_CH0(n)           (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_ANC_CALIB_GAIN_COEF_FB_CH0_MASK         (0xFFFF << 0)
#define CODEC_CODEC_ANC_CALIB_GAIN_COEF_FB_CH0_SHIFT        (0)

// reg_240
#define CODEC_CODEC_CALIB_GAIN_COEF_TT_CH0(n)               (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_CALIB_GAIN_COEF_TT_CH0_MASK             (0xFFFF << 0)
#define CODEC_CODEC_CALIB_GAIN_COEF_TT_CH0_SHIFT            (0)

// reg_244
#define CODEC_CODEC_ANC_CALIB_GAIN_PASS0_FF_CH0             (1 << 0)
#define CODEC_CODEC_ANC_CALIB_GAIN_UPDATE_FF_CH0            (1 << 1)
#define CODEC_CODEC_ANC_CALIB_GAIN_PASS0_FB_CH0             (1 << 2)
#define CODEC_CODEC_ANC_CALIB_GAIN_UPDATE_FB_CH0            (1 << 3)
#define CODEC_CODEC_CALIB_GAIN_PASS0_TT_CH0                 (1 << 4)
#define CODEC_CODEC_CALIB_GAIN_UPDATE_TT_CH0                (1 << 5)

// reg_248
#define CODEC_CODEC_IIR0_ENABLE                             (1 << 0)
#define CODEC_CODEC_IIR0_IIRA_ENABLE                        (1 << 1)
#define CODEC_CODEC_IIR0_IIRB_ENABLE                        (1 << 2)
#define CODEC_CODEC_IIR0_BYPASS                             (1 << 3)
#define CODEC_CODEC_IIR0_GAINCAL_EXT_BYPASS                 (1 << 4)
#define CODEC_CODEC_IIR0_GAINUSE_EXT_BYPASS                 (1 << 5)
#define CODEC_CODEC_IIR0_LMT_BYPASS                         (1 << 6)
#define CODEC_CODEC_IIR0_COUNT(n)                           (((n) & 0x1F) << 7)
#define CODEC_CODEC_IIR0_COUNT_MASK                         (0x1F << 7)
#define CODEC_CODEC_IIR0_COUNT_SHIFT                        (7)
#define CODEC_CODEC_IIR0_COEF_SWAP                          (1 << 12)
#define CODEC_CODEC_IIR0_AUTO_STOP                          (1 << 13)
#define CODEC_CODEC_IIR0_GC_DBG_SEL                         (1 << 14)
#define CODEC_CODEC_IIR0_COEF_SWAP_STATUS_SYNC_1            (1 << 15)
#define CODEC_CODEC_IIR0_IIRA_STOP_STATUS_SYNC_1            (1 << 16)
#define CODEC_CODEC_IIR0_IIRB_STOP_STATUS_SYNC_1            (1 << 17)

// reg_24c
#define CODEC_CODEC_IIR1_ENABLE                             (1 << 0)
#define CODEC_CODEC_IIR1_IIRA_ENABLE                        (1 << 1)
#define CODEC_CODEC_IIR1_IIRB_ENABLE                        (1 << 2)
#define CODEC_CODEC_IIR1_BYPASS                             (1 << 3)
#define CODEC_CODEC_IIR1_GAINCAL_EXT_BYPASS                 (1 << 4)
#define CODEC_CODEC_IIR1_GAINUSE_EXT_BYPASS                 (1 << 5)
#define CODEC_CODEC_IIR1_LMT_BYPASS                         (1 << 6)
#define CODEC_CODEC_IIR1_COUNT(n)                           (((n) & 0x1F) << 7)
#define CODEC_CODEC_IIR1_COUNT_MASK                         (0x1F << 7)
#define CODEC_CODEC_IIR1_COUNT_SHIFT                        (7)
#define CODEC_CODEC_IIR1_COEF_SWAP                          (1 << 12)
#define CODEC_CODEC_IIR1_AUTO_STOP                          (1 << 13)
#define CODEC_CODEC_IIR1_GC_DBG_SEL                         (1 << 14)
#define CODEC_CODEC_IIR1_COEF_SWAP_STATUS_SYNC_1            (1 << 15)
#define CODEC_CODEC_IIR1_IIRA_STOP_STATUS_SYNC_1            (1 << 16)
#define CODEC_CODEC_IIR1_IIRB_STOP_STATUS_SYNC_1            (1 << 17)

// reg_258
#define CODEC_CODEC_DEQ_IIR_ENABLE                          (1 << 0)
#define CODEC_CODEC_DEQ_IIR_IIRA_ENABLE                     (1 << 1)
#define CODEC_CODEC_DEQ_IIR_IIRB_ENABLE                     (1 << 2)
#define CODEC_CODEC_DEQ_IIR_CH0_BYPASS                      (1 << 3)
#define CODEC_CODEC_DEQ_IIR_CH1_BYPASS                      (1 << 4)
#define CODEC_CODEC_DEQ_IIR_GAINCAL_EXT_CH0_BYPASS          (1 << 5)
#define CODEC_CODEC_DEQ_IIR_GAINCAL_EXT_CH1_BYPASS          (1 << 6)
#define CODEC_CODEC_DEQ_IIR_GAINUSE_EXT_CH0_BYPASS          (1 << 7)
#define CODEC_CODEC_DEQ_IIR_GAINUSE_EXT_CH1_BYPASS          (1 << 8)
#define CODEC_CODEC_DEQ_IIR_LMT_CH0_BYPASS                  (1 << 9)
#define CODEC_CODEC_DEQ_IIR_LMT_CH1_BYPASS                  (1 << 10)
#define CODEC_CODEC_DEQ_IIR_COUNT_CH0(n)                    (((n) & 0x3F) << 11)
#define CODEC_CODEC_DEQ_IIR_COUNT_CH0_MASK                  (0x3F << 11)
#define CODEC_CODEC_DEQ_IIR_COUNT_CH0_SHIFT                 (11)
#define CODEC_CODEC_DEQ_IIR_COUNT_CH1(n)                    (((n) & 0x3F) << 17)
#define CODEC_CODEC_DEQ_IIR_COUNT_CH1_MASK                  (0x3F << 17)
#define CODEC_CODEC_DEQ_IIR_COUNT_CH1_SHIFT                 (17)
#define CODEC_CODEC_DEQ_IIR_COEF_SWAP                       (1 << 23)
#define CODEC_CODEC_DEQ_IIR_AUTO_STOP                       (1 << 24)
#define CODEC_CODEC_DEQ_IIR_GC_DBG_SEL                      (1 << 25)
#define CODEC_CODEC_DEQ_IIR_COEF_SWAP_STATUS_SYNC_1         (1 << 26)
#define CODEC_CODEC_DEQ_IIR_IIRA_STOP_STATUS_SYNC_1         (1 << 27)
#define CODEC_CODEC_DEQ_IIR_IIRB_STOP_STATUS_SYNC_1         (1 << 28)

// reg_25c
#define CODEC_CODEC_IIR0_GAIN_EXT_UPDATE                    (1 << 0)
#define CODEC_CODEC_IIR0_GAIN_EXT_SEL                       (1 << 1)
#define CODEC_CODEC_IIR1_GAIN_EXT_UPDATE                    (1 << 2)
#define CODEC_CODEC_IIR1_GAIN_EXT_SEL                       (1 << 3)
#define CODEC_CODEC_IIR4_GAIN_EXT_UPDATE                    (1 << 4)
#define CODEC_CODEC_IIR4_GAIN_EXT_SEL                       (1 << 5)
#define CODEC_CODEC_IIR5_GAIN_EXT_UPDATE                    (1 << 6)
#define CODEC_CODEC_IIR5_GAIN_EXT_SEL                       (1 << 7)
#define CODEC_CODEC_DEQ_IIR_GAIN_EXT_UPDATE_CH0             (1 << 8)
#define CODEC_CODEC_DEQ_IIR_GAIN_EXT_UPDATE_CH1             (1 << 9)
#define CODEC_CODEC_DEQ_IIR_GAIN_EXT_SEL_CH0                (1 << 10)
#define CODEC_CODEC_DEQ_IIR_GAIN_EXT_SEL_CH1                (1 << 11)
#define CODEC_CODEC_IIR0_LMT_TH_UPDATE                      (1 << 12)
#define CODEC_CODEC_IIR1_LMT_TH_UPDATE                      (1 << 13)
#define CODEC_CODEC_IIR4_LMT_TH_UPDATE                      (1 << 14)
#define CODEC_CODEC_IIR5_LMT_TH_UPDATE                      (1 << 15)
#define CODEC_CODEC_DEQ_IIR_LMT_TH_UPDATE_CH0               (1 << 16)
#define CODEC_CODEC_DEQ_IIR_LMT_TH_UPDATE_CH1               (1 << 17)

// reg_260
#define CODEC_CODEC_IIR0_GAINA_EXT(n)                       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR0_GAINA_EXT_MASK                     (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR0_GAINA_EXT_SHIFT                    (0)

// reg_264
#define CODEC_CODEC_IIR0_GAINB_EXT(n)                       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR0_GAINB_EXT_MASK                     (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR0_GAINB_EXT_SHIFT                    (0)

// reg_268
#define CODEC_CODEC_IIR1_GAINA_EXT(n)                       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR1_GAINA_EXT_MASK                     (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR1_GAINA_EXT_SHIFT                    (0)

// reg_26c
#define CODEC_CODEC_IIR1_GAINB_EXT(n)                       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR1_GAINB_EXT_MASK                     (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR1_GAINB_EXT_SHIFT                    (0)

// reg_280
#define CODEC_CODEC_IIR4_GAINA_EXT(n)                       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR4_GAINA_EXT_MASK                     (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR4_GAINA_EXT_SHIFT                    (0)

// reg_284
#define CODEC_CODEC_IIR4_GAINB_EXT(n)                       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR4_GAINB_EXT_MASK                     (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR4_GAINB_EXT_SHIFT                    (0)

// reg_288
#define CODEC_CODEC_IIR5_GAINA_EXT(n)                       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR5_GAINA_EXT_MASK                     (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR5_GAINA_EXT_SHIFT                    (0)

// reg_28c
#define CODEC_CODEC_IIR5_GAINB_EXT(n)                       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR5_GAINB_EXT_MASK                     (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR5_GAINB_EXT_SHIFT                    (0)

// reg_2a0
#define CODEC_CODEC_IIR0_GAINA_EXT_OUT_SYNC(n)              (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR0_GAINA_EXT_OUT_SYNC_MASK            (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR0_GAINA_EXT_OUT_SYNC_SHIFT           (0)

// reg_2a4
#define CODEC_CODEC_IIR0_GAINB_EXT_OUT_SYNC(n)              (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR0_GAINB_EXT_OUT_SYNC_MASK            (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR0_GAINB_EXT_OUT_SYNC_SHIFT           (0)

// reg_2a8
#define CODEC_CODEC_IIR1_GAINA_EXT_OUT_SYNC(n)              (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR1_GAINA_EXT_OUT_SYNC_MASK            (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR1_GAINA_EXT_OUT_SYNC_SHIFT           (0)

// reg_2ac
#define CODEC_CODEC_IIR1_GAINB_EXT_OUT_SYNC(n)              (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR1_GAINB_EXT_OUT_SYNC_MASK            (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR1_GAINB_EXT_OUT_SYNC_SHIFT           (0)

// reg_2c0
#define CODEC_CODEC_IIR4_GAINA_EXT_OUT_SYNC(n)              (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR4_GAINA_EXT_OUT_SYNC_MASK            (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR4_GAINA_EXT_OUT_SYNC_SHIFT           (0)

// reg_2c4
#define CODEC_CODEC_IIR4_GAINB_EXT_OUT_SYNC(n)              (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR4_GAINB_EXT_OUT_SYNC_MASK            (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR4_GAINB_EXT_OUT_SYNC_SHIFT           (0)

// reg_2c8
#define CODEC_CODEC_IIR5_GAINA_EXT_OUT_SYNC(n)              (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR5_GAINA_EXT_OUT_SYNC_MASK            (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR5_GAINA_EXT_OUT_SYNC_SHIFT           (0)

// reg_2cc
#define CODEC_CODEC_IIR5_GAINB_EXT_OUT_SYNC(n)              (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR5_GAINB_EXT_OUT_SYNC_MASK            (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR5_GAINB_EXT_OUT_SYNC_SHIFT           (0)

// reg_2e0
#define CODEC_CODEC_DEQ_IIR_GAINA_EXT_CH0(n)                (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_DEQ_IIR_GAINA_EXT_CH0_MASK              (0xFFFFFFFF << 0)
#define CODEC_CODEC_DEQ_IIR_GAINA_EXT_CH0_SHIFT             (0)

// reg_2e4
#define CODEC_CODEC_DEQ_IIR_GAINA_EXT_CH1(n)                (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_DEQ_IIR_GAINA_EXT_CH1_MASK              (0xFFFFFFFF << 0)
#define CODEC_CODEC_DEQ_IIR_GAINA_EXT_CH1_SHIFT             (0)

// reg_2e8
#define CODEC_CODEC_DEQ_IIR_GAINB_EXT_CH0(n)                (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_DEQ_IIR_GAINB_EXT_CH0_MASK              (0xFFFFFFFF << 0)
#define CODEC_CODEC_DEQ_IIR_GAINB_EXT_CH0_SHIFT             (0)

// reg_2ec
#define CODEC_CODEC_DEQ_IIR_GAINB_EXT_CH1(n)                (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_DEQ_IIR_GAINB_EXT_CH1_MASK              (0xFFFFFFFF << 0)
#define CODEC_CODEC_DEQ_IIR_GAINB_EXT_CH1_SHIFT             (0)

// reg_2f0
#define CODEC_CODEC_DEQ_IIR_GAINA_EXT_OUT_CH0_SYNC(n)       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_DEQ_IIR_GAINA_EXT_OUT_CH0_SYNC_MASK     (0xFFFFFFFF << 0)
#define CODEC_CODEC_DEQ_IIR_GAINA_EXT_OUT_CH0_SYNC_SHIFT    (0)

// reg_2f4
#define CODEC_CODEC_DEQ_IIR_GAINA_EXT_OUT_CH1_SYNC(n)       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_DEQ_IIR_GAINA_EXT_OUT_CH1_SYNC_MASK     (0xFFFFFFFF << 0)
#define CODEC_CODEC_DEQ_IIR_GAINA_EXT_OUT_CH1_SYNC_SHIFT    (0)

// reg_2f8
#define CODEC_CODEC_DEQ_IIR_GAINB_EXT_OUT_CH0_SYNC(n)       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_DEQ_IIR_GAINB_EXT_OUT_CH0_SYNC_MASK     (0xFFFFFFFF << 0)
#define CODEC_CODEC_DEQ_IIR_GAINB_EXT_OUT_CH0_SYNC_SHIFT    (0)

// reg_2fc
#define CODEC_CODEC_DEQ_IIR_GAINB_EXT_OUT_CH1_SYNC(n)       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_DEQ_IIR_GAINB_EXT_OUT_CH1_SYNC_MASK     (0xFFFFFFFF << 0)
#define CODEC_CODEC_DEQ_IIR_GAINB_EXT_OUT_CH1_SYNC_SHIFT    (0)

// reg_300
#define CODEC_CODEC_IIR0_LMT_DELAY(n)                       (((n) & 0x7F) << 0)
#define CODEC_CODEC_IIR0_LMT_DELAY_MASK                     (0x7F << 0)
#define CODEC_CODEC_IIR0_LMT_DELAY_SHIFT                    (0)
#define CODEC_CODEC_IIR1_LMT_DELAY(n)                       (((n) & 0x7F) << 7)
#define CODEC_CODEC_IIR1_LMT_DELAY_MASK                     (0x7F << 7)
#define CODEC_CODEC_IIR1_LMT_DELAY_SHIFT                    (7)
#define CODEC_CODEC_IIR4_LMT_DELAY(n)                       (((n) & 0x7F) << 14)
#define CODEC_CODEC_IIR4_LMT_DELAY_MASK                     (0x7F << 14)
#define CODEC_CODEC_IIR4_LMT_DELAY_SHIFT                    (14)
#define CODEC_CODEC_IIR5_LMT_DELAY(n)                       (((n) & 0x7F) << 21)
#define CODEC_CODEC_IIR5_LMT_DELAY_MASK                     (0x7F << 21)
#define CODEC_CODEC_IIR5_LMT_DELAY_SHIFT                    (21)

// reg_308
#define CODEC_CODEC_IIR0_GAIN_EXT_TH(n)                     (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR0_GAIN_EXT_TH_MASK                   (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR0_GAIN_EXT_TH_SHIFT                  (0)

// reg_30c
#define CODEC_CODEC_IIR1_GAIN_EXT_TH(n)                     (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR1_GAIN_EXT_TH_MASK                   (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR1_GAIN_EXT_TH_SHIFT                  (0)

// reg_318
#define CODEC_CODEC_DEQ_IIR_GAIN_EXT_TH(n)                  (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_DEQ_IIR_GAIN_EXT_TH_MASK                (0xFFFFFFFF << 0)
#define CODEC_CODEC_DEQ_IIR_GAIN_EXT_TH_SHIFT               (0)

// reg_31c
#define CODEC_CODEC_IIR0_LMT_TH(n)                          (((n) & 0x7FFFFF) << 0)
#define CODEC_CODEC_IIR0_LMT_TH_MASK                        (0x7FFFFF << 0)
#define CODEC_CODEC_IIR0_LMT_TH_SHIFT                       (0)

// reg_320
#define CODEC_CODEC_IIR1_LMT_TH(n)                          (((n) & 0x7FFFFF) << 0)
#define CODEC_CODEC_IIR1_LMT_TH_MASK                        (0x7FFFFF << 0)
#define CODEC_CODEC_IIR1_LMT_TH_SHIFT                       (0)

// reg_32c
#define CODEC_CODEC_IIR4_LMT_TH(n)                          (((n) & 0x7FFFFF) << 0)
#define CODEC_CODEC_IIR4_LMT_TH_MASK                        (0x7FFFFF << 0)
#define CODEC_CODEC_IIR4_LMT_TH_SHIFT                       (0)

// reg_330
#define CODEC_CODEC_IIR5_LMT_TH(n)                          (((n) & 0x7FFFFF) << 0)
#define CODEC_CODEC_IIR5_LMT_TH_MASK                        (0x7FFFFF << 0)
#define CODEC_CODEC_IIR5_LMT_TH_SHIFT                       (0)

// reg_338
#define CODEC_CODEC_PDU_FILTER_MODE                         (1 << 0)
#define CODEC_CODEC_TT_FILTER_MODE                          (1 << 1)
#define CODEC_CODEC_FS_FILTER_MODE                          (1 << 2)
#define CODEC_CODEC_MM_FILTER_MODE                          (1 << 3)

// reg_33c
#define CODEC_CODEC_MUSIC_MIX_OFF_CH0                       (1 << 0)
#define CODEC_CODEC_PDU_OFF_CH0                             (1 << 1)
#define CODEC_CODEC_PDU_MIX_EN_CH0                          (1 << 2)
#define CODEC_CODEC_DEHOWL_EN_CH0                           (1 << 3)
#define CODEC_CODEC_TT_OUT_OFF_CH0                          (1 << 4)
#define CODEC_CODEC_TT_PDU_OFF_CH0                          (1 << 5)
#define CODEC_CODEC_MM_EX_TT_CH0                            (1 << 6)
#define CODEC_CODEC_DEHOWL_KEEP_CH0                         (1 << 7)
#define CODEC_CODEC_DEHOWL_KEEP_SEL_CH0(n)                  (((n) & 0x7) << 8)
#define CODEC_CODEC_DEHOWL_KEEP_SEL_CH0_MASK                (0x7 << 8)
#define CODEC_CODEC_DEHOWL_KEEP_SEL_CH0_SHIFT               (8)
#define CODEC_CODEC_TWS_ANC_EQ                              (1 << 11)

// reg_340
#define CODEC_CODEC_DEQ_IIR_LMT_DELAY_CH0(n)                (((n) & 0x7F) << 0)
#define CODEC_CODEC_DEQ_IIR_LMT_DELAY_CH0_MASK              (0x7F << 0)
#define CODEC_CODEC_DEQ_IIR_LMT_DELAY_CH0_SHIFT             (0)
#define CODEC_CODEC_DEQ_IIR_LMT_TH_CH0(n)                   (((n) & 0x7FFFFF) << 7)
#define CODEC_CODEC_DEQ_IIR_LMT_TH_CH0_MASK                 (0x7FFFFF << 7)
#define CODEC_CODEC_DEQ_IIR_LMT_TH_CH0_SHIFT                (7)

// reg_344
#define CODEC_CODEC_DEQ_IIR_LMT_DELAY_CH1(n)                (((n) & 0x7F) << 0)
#define CODEC_CODEC_DEQ_IIR_LMT_DELAY_CH1_MASK              (0x7F << 0)
#define CODEC_CODEC_DEQ_IIR_LMT_DELAY_CH1_SHIFT             (0)
#define CODEC_CODEC_DEQ_IIR_LMT_TH_CH1(n)                   (((n) & 0x7FFFFF) << 7)
#define CODEC_CODEC_DEQ_IIR_LMT_TH_CH1_MASK                 (0x7FFFFF << 7)
#define CODEC_CODEC_DEQ_IIR_LMT_TH_CH1_SHIFT                (7)

// reg_34c
#define CODEC_CODEC_DRE_DELAY_MODE_CH0                      (1 << 0)
#define CODEC_CODEC_DRE_DELAY_HL_CH0(n)                     (((n) & 0xFF) << 1)
#define CODEC_CODEC_DRE_DELAY_HL_CH0_MASK                   (0xFF << 1)
#define CODEC_CODEC_DRE_DELAY_HL_CH0_SHIFT                  (1)

// reg_350
#define CODEC_CODEC_DRE_ENABLE_CH0                          (1 << 0)
#define CODEC_CODEC_DRE_STEP_MODE_CH0(n)                    (((n) & 0x7) << 1)
#define CODEC_CODEC_DRE_STEP_MODE_CH0_MASK                  (0x7 << 1)
#define CODEC_CODEC_DRE_STEP_MODE_CH0_SHIFT                 (1)
#define CODEC_CODEC_DRE_INI_ANA_GAIN_CH0(n)                 (((n) & 0xF) << 4)
#define CODEC_CODEC_DRE_INI_ANA_GAIN_CH0_MASK               (0xF << 4)
#define CODEC_CODEC_DRE_INI_ANA_GAIN_CH0_SHIFT              (4)
#define CODEC_CODEC_DRE_DELAY_LH_CH0(n)                     (((n) & 0xFF) << 8)
#define CODEC_CODEC_DRE_DELAY_LH_CH0_MASK                   (0xFF << 8)
#define CODEC_CODEC_DRE_DELAY_LH_CH0_SHIFT                  (8)
#define CODEC_CODEC_DRE_AMP_HIGH_CH0(n)                     (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DRE_AMP_HIGH_CH0_MASK                   (0xFFFF << 16)
#define CODEC_CODEC_DRE_AMP_HIGH_CH0_SHIFT                  (16)

// reg_354
#define CODEC_CODEC_DRE_WINDOW_CH0(n)                       (((n) & 0x1FFFFF) << 0)
#define CODEC_CODEC_DRE_WINDOW_CH0_MASK                     (0x1FFFFF << 0)
#define CODEC_CODEC_DRE_WINDOW_CH0_SHIFT                    (0)
#define CODEC_CODEC_DRE_THD_DB_OFFSET_CH0(n)                (((n) & 0xF) << 21)
#define CODEC_CODEC_DRE_THD_DB_OFFSET_CH0_MASK              (0xF << 21)
#define CODEC_CODEC_DRE_THD_DB_OFFSET_CH0_SHIFT             (21)
#define CODEC_CODEC_DRE_THD_DB_OFFSET_SIGN_CH0              (1 << 25)
#define CODEC_CODEC_DRE_GAIN_OFFSET_CH0(n)                  (((n) & 0x1F) << 26)
#define CODEC_CODEC_DRE_GAIN_OFFSET_CH0_MASK                (0x1F << 26)
#define CODEC_CODEC_DRE_GAIN_OFFSET_CH0_SHIFT               (26)

// reg_358
#define CODEC_CODEC_DRE_DB_HIGH_CH0(n)                      (((n) & 0x3F) << 0)
#define CODEC_CODEC_DRE_DB_HIGH_CH0_MASK                    (0x3F << 0)
#define CODEC_CODEC_DRE_DB_HIGH_CH0_SHIFT                   (0)
#define CODEC_CODEC_DRE_DB_LOW_CH0(n)                       (((n) & 0x3F) << 6)
#define CODEC_CODEC_DRE_DB_LOW_CH0_MASK                     (0x3F << 6)
#define CODEC_CODEC_DRE_DB_LOW_CH0_SHIFT                    (6)
#define CODEC_CODEC_DRE_GAIN_TOP_CH0(n)                     (((n) & 0xF) << 12)
#define CODEC_CODEC_DRE_GAIN_TOP_CH0_MASK                   (0xF << 12)
#define CODEC_CODEC_DRE_GAIN_TOP_CH0_SHIFT                  (12)
#define CODEC_CODEC_DRE_DELAY_DC_LH_CH0(n)                  (((n) & 0xFF) << 16)
#define CODEC_CODEC_DRE_DELAY_DC_LH_CH0_MASK                (0xFF << 16)
#define CODEC_CODEC_DRE_DELAY_DC_LH_CH0_SHIFT               (16)
#define CODEC_CODEC_DRE_DELAY_DC_HL_CH0(n)                  (((n) & 0xFF) << 24)
#define CODEC_CODEC_DRE_DELAY_DC_HL_CH0_MASK                (0xFF << 24)
#define CODEC_CODEC_DRE_DELAY_DC_HL_CH0_SHIFT               (24)

// reg_368
#define CODEC_CODEC_DAC_DRE_GAIN_STEP0_CH0(n)               (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP0_CH0_MASK             (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP0_CH0_SHIFT            (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP1_CH0(n)               (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP1_CH0_MASK             (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP1_CH0_SHIFT            (16)

// reg_36c
#define CODEC_CODEC_DAC_DRE_GAIN_STEP2_CH0(n)               (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP2_CH0_MASK             (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP2_CH0_SHIFT            (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP3_CH0(n)               (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP3_CH0_MASK             (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP3_CH0_SHIFT            (16)

// reg_370
#define CODEC_CODEC_DAC_DRE_GAIN_STEP4_CH0(n)               (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP4_CH0_MASK             (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP4_CH0_SHIFT            (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP5_CH0(n)               (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP5_CH0_MASK             (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP5_CH0_SHIFT            (16)

// reg_374
#define CODEC_CODEC_DAC_DRE_GAIN_STEP6_CH0(n)               (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP6_CH0_MASK             (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP6_CH0_SHIFT            (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP7_CH0(n)               (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP7_CH0_MASK             (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP7_CH0_SHIFT            (16)

// reg_378
#define CODEC_CODEC_DAC_DRE_GAIN_STEP8_CH0(n)               (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP8_CH0_MASK             (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP8_CH0_SHIFT            (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP9_CH0(n)               (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP9_CH0_MASK             (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP9_CH0_SHIFT            (16)

// reg_37c
#define CODEC_CODEC_DAC_DRE_GAIN_STEP10_CH0(n)              (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP10_CH0_MASK            (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP10_CH0_SHIFT           (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP11_CH0(n)              (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP11_CH0_MASK            (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP11_CH0_SHIFT           (16)

// reg_380
#define CODEC_CODEC_DAC_DRE_GAIN_STEP12_CH0(n)              (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP12_CH0_MASK            (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP12_CH0_SHIFT           (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP13_CH0(n)              (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP13_CH0_MASK            (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP13_CH0_SHIFT           (16)

// reg_384
#define CODEC_CODEC_DAC_DRE_GAIN_STEP14_CH0(n)              (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP14_CH0_MASK            (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP14_CH0_SHIFT           (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP15_CH0(n)              (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP15_CH0_MASK            (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP15_CH0_SHIFT           (16)

// reg_3a8
#define CODEC_CODEC_DAC_DRE_DC0_CH0(n)                      (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_DC0_CH0_MASK                    (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_DC0_CH0_SHIFT                   (0)
#define CODEC_CODEC_DAC_DRE_DC1_CH0(n)                      (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_DC1_CH0_MASK                    (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_DC1_CH0_SHIFT                   (16)

// reg_3ac
#define CODEC_CODEC_DAC_DRE_DC2_CH0(n)                      (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_DC2_CH0_MASK                    (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_DC2_CH0_SHIFT                   (0)
#define CODEC_CODEC_DAC_DRE_DC3_CH0(n)                      (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_DC3_CH0_MASK                    (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_DC3_CH0_SHIFT                   (16)

// reg_3b0
#define CODEC_CODEC_DAC_DRE_DC4_CH0(n)                      (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_DC4_CH0_MASK                    (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_DC4_CH0_SHIFT                   (0)
#define CODEC_CODEC_DAC_DRE_DC5_CH0(n)                      (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_DC5_CH0_MASK                    (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_DC5_CH0_SHIFT                   (16)

// reg_3b4
#define CODEC_CODEC_DAC_DRE_DC6_CH0(n)                      (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_DC6_CH0_MASK                    (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_DC6_CH0_SHIFT                   (0)
#define CODEC_CODEC_DAC_DRE_DC7_CH0(n)                      (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_DC7_CH0_MASK                    (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_DC7_CH0_SHIFT                   (16)

// reg_3b8
#define CODEC_CODEC_DAC_DRE_DC8_CH0(n)                      (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_DC8_CH0_MASK                    (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_DC8_CH0_SHIFT                   (0)
#define CODEC_CODEC_DAC_DRE_DC9_CH0(n)                      (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_DC9_CH0_MASK                    (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_DC9_CH0_SHIFT                   (16)

// reg_3bc
#define CODEC_CODEC_DAC_DRE_DC10_CH0(n)                     (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_DC10_CH0_MASK                   (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_DC10_CH0_SHIFT                  (0)
#define CODEC_CODEC_DAC_DRE_DC11_CH0(n)                     (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_DC11_CH0_MASK                   (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_DC11_CH0_SHIFT                  (16)

// reg_3c0
#define CODEC_CODEC_DAC_DRE_DC12_CH0(n)                     (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_DC12_CH0_MASK                   (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_DC12_CH0_SHIFT                  (0)
#define CODEC_CODEC_DAC_DRE_DC13_CH0(n)                     (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_DC13_CH0_MASK                   (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_DC13_CH0_SHIFT                  (16)

// reg_3c4
#define CODEC_CODEC_DAC_DRE_DC14_CH0(n)                     (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_DC14_CH0_MASK                   (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_DC14_CH0_SHIFT                  (0)
#define CODEC_CODEC_DAC_DRE_DC15_CH0(n)                     (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_DC15_CH0_MASK                   (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_DC15_CH0_SHIFT                  (16)

// reg_3e8
#define CODEC_CODEC_DRE_ANA_GAIN_CH0_SYNC(n)                (((n) & 0x1F) << 0)
#define CODEC_CODEC_DRE_ANA_GAIN_CH0_SYNC_MASK              (0x1F << 0)
#define CODEC_CODEC_DRE_ANA_GAIN_CH0_SYNC_SHIFT             (0)
#define CODEC_CODEC_DRE_COUNT_CH0_SYNC(n)                   (((n) & 0x1FFFFF) << 5)
#define CODEC_CODEC_DRE_COUNT_CH0_SYNC_MASK                 (0x1FFFFF << 5)
#define CODEC_CODEC_DRE_COUNT_CH0_SYNC_SHIFT                (5)

// reg_488
#define CODEC_RESERVED_REG0(n)                              (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RESERVED_REG0_MASK                            (0xFFFFFFFF << 0)
#define CODEC_RESERVED_REG0_SHIFT                           (0)

// reg_48c
#define CODEC_RESERVED_REG1(n)                              (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RESERVED_REG1_MASK                            (0xFFFFFFFF << 0)
#define CODEC_RESERVED_REG1_SHIFT                           (0)

// reg_490
#define CODEC_WS_FLUSH                                      (1 << 0)
#define CODEC_WS_TRIG_MODE                                  (1 << 1)
#define CODEC_WS_TRIGGER_INTERVAL(n)                        (((n) & 0xF) << 2)
#define CODEC_WS_TRIGGER_INTERVAL_MASK                      (0xF << 2)
#define CODEC_WS_TRIGGER_INTERVAL_SHIFT                     (2)
#define CODEC_TRIG_BT0_I2S1                                 (1 << 6)
#define CODEC_CALIB_INTERVAL(n)                             (((n) & 0xF) << 7)
#define CODEC_CALIB_INTERVAL_MASK                           (0xF << 7)
#define CODEC_CALIB_INTERVAL_SHIFT                          (7)
#define CODEC_WS_CNT_INI(n)                                 (((n) & 0xFFFFF) << 11)
#define CODEC_WS_CNT_INI_MASK                               (0xFFFFF << 11)
#define CODEC_WS_CNT_INI_SHIFT                              (11)

// reg_494
#define CODEC_WS_CNT(n)                                     (((n) & 0xFFFFF) << 0)
#define CODEC_WS_CNT_MASK                                   (0xFFFFF << 0)
#define CODEC_WS_CNT_SHIFT                                  (0)

// reg_498
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_8(n)               (((n) & 0xFF) << 0)
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_8_MASK             (0xFF << 0)
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_8_SHIFT            (0)
#define CODEC_CFG_CLASSD_DRE_THRESHOLD_N22(n)               (((n) & 0xFFFF) << 8)
#define CODEC_CFG_CLASSD_DRE_THRESHOLD_N22_MASK             (0xFFFF << 8)
#define CODEC_CFG_CLASSD_DRE_THRESHOLD_N22_SHIFT            (8)

// reg_49c
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DIGGAIN_1(n)           (((n) & 0xFFFF) << 0)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DIGGAIN_1_MASK         (0xFFFF << 0)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DIGGAIN_1_SHIFT        (0)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DIGGAIN_2(n)           (((n) & 0xFFFF) << 16)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DIGGAIN_2_MASK         (0xFFFF << 16)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DIGGAIN_2_SHIFT        (16)

// reg_4a0
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DIGGAIN_3(n)           (((n) & 0xFFFF) << 0)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DIGGAIN_3_MASK         (0xFFFF << 0)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DIGGAIN_3_SHIFT        (0)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DIGGAIN_4(n)           (((n) & 0xFFFF) << 16)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DIGGAIN_4_MASK         (0xFFFF << 16)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DIGGAIN_4_SHIFT        (16)

// reg_4a4
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DIGGAIN_5(n)           (((n) & 0xFFFF) << 0)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DIGGAIN_5_MASK         (0xFFFF << 0)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DIGGAIN_5_SHIFT        (0)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DIGGAIN_6(n)           (((n) & 0xFFFF) << 16)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DIGGAIN_6_MASK         (0xFFFF << 16)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DIGGAIN_6_SHIFT        (16)

// reg_4a8
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DIGGAIN_7(n)           (((n) & 0xFFFF) << 0)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DIGGAIN_7_MASK         (0xFFFF << 0)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DIGGAIN_7_SHIFT        (0)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DIGGAIN_8(n)           (((n) & 0xFFFF) << 16)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DIGGAIN_8_MASK         (0xFFFF << 16)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DIGGAIN_8_SHIFT        (16)

// reg_4ac
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DC_1(n)                (((n) & 0xFFFF) << 0)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DC_1_MASK              (0xFFFF << 0)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DC_1_SHIFT             (0)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DC_2(n)                (((n) & 0xFFFF) << 16)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DC_2_MASK              (0xFFFF << 16)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DC_2_SHIFT             (16)

// reg_4b0
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DC_3(n)                (((n) & 0xFFFF) << 0)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DC_3_MASK              (0xFFFF << 0)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DC_3_SHIFT             (0)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DC_4(n)                (((n) & 0xFFFF) << 16)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DC_4_MASK              (0xFFFF << 16)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DC_4_SHIFT             (16)

// reg_4b4
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DC_5(n)                (((n) & 0xFFFF) << 0)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DC_5_MASK              (0xFFFF << 0)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DC_5_SHIFT             (0)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DC_6(n)                (((n) & 0xFFFF) << 16)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DC_6_MASK              (0xFFFF << 16)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DC_6_SHIFT             (16)

// reg_4b8
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DC_7(n)                (((n) & 0xFFFF) << 0)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DC_7_MASK              (0xFFFF << 0)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DC_7_SHIFT             (0)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DC_8(n)                (((n) & 0xFFFF) << 16)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DC_8_MASK              (0xFFFF << 16)
#define CODEC_CFG_DRE_CLASSD_GAIN_LN_DC_8_SHIFT             (16)

// reg_4bc
#define CODEC_CFG_CLASSD_MASKING_MODE(n)                    (((n) & 0x3) << 0)
#define CODEC_CFG_CLASSD_MASKING_MODE_MASK                  (0x3 << 0)
#define CODEC_CFG_CLASSD_MASKING_MODE_SHIFT                 (0)
#define CODEC_CFG_CLASSD_MASKING_ENABLE                     (1 << 2)
#define CODEC_CFG_CLASSD_0P6_MASKING_CF_MODE                (1 << 3)
#define CODEC_CFG_CLASSD_0P6_LPF_F9_ALPHA(n)                (((n) & 0xF) << 4)
#define CODEC_CFG_CLASSD_0P6_LPF_F9_ALPHA_MASK              (0xF << 4)
#define CODEC_CFG_CLASSD_0P6_LPF_F9_ALPHA_SHIFT             (4)
#define CODEC_CFG_CLASSD_0P6_LPF_F11_ALPHA(n)               (((n) & 0xF) << 8)
#define CODEC_CFG_CLASSD_0P6_LPF_F11_ALPHA_MASK             (0xF << 8)
#define CODEC_CFG_CLASSD_0P6_LPF_F11_ALPHA_SHIFT            (8)
#define CODEC_CFG_CLASSD_FIND_OK_DELAY(n)                   (((n) & 0x1FF) << 12)
#define CODEC_CFG_CLASSD_FIND_OK_DELAY_MASK                 (0x1FF << 12)
#define CODEC_CFG_CLASSD_FIND_OK_DELAY_SHIFT                (12)

// reg_4c0
#define CODEC_CFG_CLASSD_0P6_MASKING_CF(n)                  (((n) & 0xFFFFFF) << 0)
#define CODEC_CFG_CLASSD_0P6_MASKING_CF_MASK                (0xFFFFFF << 0)
#define CODEC_CFG_CLASSD_0P6_MASKING_CF_SHIFT               (0)

// reg_4c4
#define CODEC_CFG_CLASSD_0P6_HPF_F9_COEF_A1(n)              (((n) & 0xFFFFF) << 0)
#define CODEC_CFG_CLASSD_0P6_HPF_F9_COEF_A1_MASK            (0xFFFFF << 0)
#define CODEC_CFG_CLASSD_0P6_HPF_F9_COEF_A1_SHIFT           (0)

// reg_4c8
#define CODEC_CFG_CLASSD_0P6_HPF_F9_COEF_A2(n)              (((n) & 0xFFFFF) << 0)
#define CODEC_CFG_CLASSD_0P6_HPF_F9_COEF_A2_MASK            (0xFFFFF << 0)
#define CODEC_CFG_CLASSD_0P6_HPF_F9_COEF_A2_SHIFT           (0)

// reg_4cc
#define CODEC_CFG_CLASSD_0P6_HPF_F9_COEF_B0(n)              (((n) & 0xFFFFF) << 0)
#define CODEC_CFG_CLASSD_0P6_HPF_F9_COEF_B0_MASK            (0xFFFFF << 0)
#define CODEC_CFG_CLASSD_0P6_HPF_F9_COEF_B0_SHIFT           (0)

// reg_4d0
#define CODEC_CFG_CLASSD_0P6_HPF_F9_COEF_B1(n)              (((n) & 0xFFFFF) << 0)
#define CODEC_CFG_CLASSD_0P6_HPF_F9_COEF_B1_MASK            (0xFFFFF << 0)
#define CODEC_CFG_CLASSD_0P6_HPF_F9_COEF_B1_SHIFT           (0)

// reg_4d4
#define CODEC_CFG_CLASSD_0P6_HPF_F9_COEF_B2(n)              (((n) & 0xFFFFF) << 0)
#define CODEC_CFG_CLASSD_0P6_HPF_F9_COEF_B2_MASK            (0xFFFFF << 0)
#define CODEC_CFG_CLASSD_0P6_HPF_F9_COEF_B2_SHIFT           (0)

// reg_4d8
#define CODEC_CFG_CLASSD_0P6_HPF_F11_COEF_A1(n)             (((n) & 0xFFFFF) << 0)
#define CODEC_CFG_CLASSD_0P6_HPF_F11_COEF_A1_MASK           (0xFFFFF << 0)
#define CODEC_CFG_CLASSD_0P6_HPF_F11_COEF_A1_SHIFT          (0)

// reg_4dc
#define CODEC_CFG_CLASSD_0P6_HPF_F11_COEF_A2(n)             (((n) & 0xFFFFF) << 0)
#define CODEC_CFG_CLASSD_0P6_HPF_F11_COEF_A2_MASK           (0xFFFFF << 0)
#define CODEC_CFG_CLASSD_0P6_HPF_F11_COEF_A2_SHIFT          (0)

// reg_4e0
#define CODEC_CFG_CLASSD_0P6_HPF_F11_COEF_B0(n)             (((n) & 0xFFFFF) << 0)
#define CODEC_CFG_CLASSD_0P6_HPF_F11_COEF_B0_MASK           (0xFFFFF << 0)
#define CODEC_CFG_CLASSD_0P6_HPF_F11_COEF_B0_SHIFT          (0)

// reg_4e4
#define CODEC_CFG_CLASSD_0P6_HPF_F11_COEF_B1(n)             (((n) & 0xFFFFF) << 0)
#define CODEC_CFG_CLASSD_0P6_HPF_F11_COEF_B1_MASK           (0xFFFFF << 0)
#define CODEC_CFG_CLASSD_0P6_HPF_F11_COEF_B1_SHIFT          (0)

// reg_4ec
#define CODEC_CFG_CLASSD_0P6_HPF_F11_COEF_B2(n)             (((n) & 0xFFFFF) << 0)
#define CODEC_CFG_CLASSD_0P6_HPF_F11_COEF_B2_MASK           (0xFFFFF << 0)
#define CODEC_CFG_CLASSD_0P6_HPF_F11_COEF_B2_SHIFT          (0)

// reg_4f0
#define CODEC_CFG_DRE_GAIN_4_EXT                            (1 << 0)
#define CODEC_CFG_CLASSD_MASKING_EVENT_MSK                  (1 << 1)
#define CODEC_CFG_CLASSD_GAIN_LN_UPDATE_MODE(n)             (((n) & 0x3) << 2)
#define CODEC_CFG_CLASSD_GAIN_LN_UPDATE_MODE_MASK           (0x3 << 2)
#define CODEC_CFG_CLASSD_GAIN_LN_UPDATE_MODE_SHIFT          (2)
#define CODEC_CFG_CLASSD_WAIT_DIG0_CNT_MSK                  (1 << 4)
#define CODEC_CFG_CLASSD_VLG_OUT_MODE                       (1 << 5)
#define CODEC_CFG_CLASSD_OUT2ANA_INV                        (1 << 6)
#define CODEC_CFG_CLASSD_WAIT_ANA_CNT_MSK                   (1 << 7)
#define CODEC_CFG_CLASSD_GAIN_LN_SWITCH_MSK                 (1 << 8)
#define CODEC_CFG_CLASSD_WAIT_ANA_TIME(n)                   (((n) & 0x3FFF) << 9)
#define CODEC_CFG_CLASSD_WAIT_ANA_TIME_MASK                 (0x3FFF << 9)
#define CODEC_CFG_CLASSD_WAIT_ANA_TIME_SHIFT                (9)

// reg_4f4
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0(n)            (((n) & 0xFF) << 0)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0_MASK          (0xFF << 0)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0_SHIFT         (0)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_1(n)            (((n) & 0xFF) << 8)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_1_MASK          (0xFF << 8)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_1_SHIFT         (8)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_2(n)            (((n) & 0xFF) << 16)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_2_MASK          (0xFF << 16)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_2_SHIFT         (16)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_3(n)            (((n) & 0xFF) << 24)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_3_MASK          (0xFF << 24)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_3_SHIFT         (24)

// reg_4f8
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_4(n)            (((n) & 0xFF) << 0)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_4_MASK          (0xFF << 0)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_4_SHIFT         (0)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0_1(n)          (((n) & 0xFF) << 8)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0_1_MASK        (0xFF << 8)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0_1_SHIFT       (8)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0_2(n)          (((n) & 0xFF) << 16)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0_2_MASK        (0xFF << 16)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0_2_SHIFT       (16)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0_3(n)          (((n) & 0xFF) << 24)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0_3_MASK        (0xFF << 24)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0_3_SHIFT       (24)

// reg_4fc
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0_4(n)          (((n) & 0xFF) << 0)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0_4_MASK        (0xFF << 0)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0_4_SHIFT       (0)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0_5(n)          (((n) & 0xFF) << 8)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0_5_MASK        (0xFF << 8)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0_5_SHIFT       (8)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0_6(n)          (((n) & 0xFF) << 16)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0_6_MASK        (0xFF << 16)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0_6_SHIFT       (16)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0_7(n)          (((n) & 0xFF) << 24)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0_7_MASK        (0xFF << 24)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0_7_SHIFT       (24)

// reg_500
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0_8(n)          (((n) & 0xFF) << 0)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0_8_MASK        (0xFF << 0)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_0_8_SHIFT       (0)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_OVFW(n)         (((n) & 0xFF) << 8)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_OVFW_MASK       (0xFF << 8)
#define CODEC_CFG_CLASSD_DIG_ANA_GAIN_DELAY_OVFW_SHIFT      (8)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0(n)                  (((n) & 0xFFFF) << 16)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0_MASK                (0xFFFF << 16)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0_SHIFT               (16)

// reg_504
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_1(n)                  (((n) & 0xFFFF) << 0)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_1_MASK                (0xFFFF << 0)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_1_SHIFT               (0)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_2(n)                  (((n) & 0xFFFF) << 16)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_2_MASK                (0xFFFF << 16)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_2_SHIFT               (16)

// reg_508
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_3(n)                  (((n) & 0xFFFF) << 0)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_3_MASK                (0xFFFF << 0)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_3_SHIFT               (0)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_4(n)                  (((n) & 0xFFFF) << 16)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_4_MASK                (0xFFFF << 16)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_4_SHIFT               (16)

// reg_50c
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0_1(n)                (((n) & 0xFFFF) << 0)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0_1_MASK              (0xFFFF << 0)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0_1_SHIFT             (0)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0_2(n)                (((n) & 0xFFFF) << 16)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0_2_MASK              (0xFFFF << 16)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0_2_SHIFT             (16)

// reg_510
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0_3(n)                (((n) & 0xFFFF) << 0)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0_3_MASK              (0xFFFF << 0)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0_3_SHIFT             (0)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0_4(n)                (((n) & 0xFFFF) << 16)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0_4_MASK              (0xFFFF << 16)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0_4_SHIFT             (16)

// reg_514
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0_5(n)                (((n) & 0xFFFF) << 0)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0_5_MASK              (0xFFFF << 0)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0_5_SHIFT             (0)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0_6(n)                (((n) & 0xFFFF) << 16)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0_6_MASK              (0xFFFF << 16)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0_6_SHIFT             (16)

// reg_518
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0_7(n)                (((n) & 0xFFFF) << 0)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0_7_MASK              (0xFFFF << 0)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0_7_SHIFT             (0)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0_8(n)                (((n) & 0xFFFF) << 16)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0_8_MASK              (0xFFFF << 16)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_0_8_SHIFT             (16)

// reg_51c
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_DEFAULT(n)            (((n) & 0xFFFF) << 0)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_DEFAULT_MASK          (0xFFFF << 0)
#define CODEC_CFG_CLASSD_DRE_AMP_HIGH_DEFAULT_SHIFT         (0)
#define CODEC_CFG_CLASSD_WAIT_DIG0_TIME(n)                  (((n) & 0x3FFF) << 16)
#define CODEC_CFG_CLASSD_WAIT_DIG0_TIME_MASK                (0x3FFF << 16)
#define CODEC_CFG_CLASSD_WAIT_DIG0_TIME_SHIFT               (16)

// reg_520
#define CODEC_CFG_CLASSD_0P6_LPF_F9_ALPHA_STABLE(n)         (((n) & 0xF) << 0)
#define CODEC_CFG_CLASSD_0P6_LPF_F9_ALPHA_STABLE_MASK       (0xF << 0)
#define CODEC_CFG_CLASSD_0P6_LPF_F9_ALPHA_STABLE_SHIFT      (0)
#define CODEC_CFG_CLASSD_0P6_LPF_F11_ALPHA_STABLE(n)        (((n) & 0xF) << 4)
#define CODEC_CFG_CLASSD_0P6_LPF_F11_ALPHA_STABLE_MASK      (0xF << 4)
#define CODEC_CFG_CLASSD_0P6_LPF_F11_ALPHA_STABLE_SHIFT     (4)
#define CODEC_CFG_CLASSD_0P6_FIND_OK_DELAY_48K(n)           (((n) & 0x7F) << 8)
#define CODEC_CFG_CLASSD_0P6_FIND_OK_DELAY_48K_MASK         (0x7F << 8)
#define CODEC_CFG_CLASSD_0P6_FIND_OK_DELAY_48K_SHIFT        (8)
#define CODEC_CFG_CLASSD_0P6_FIND_OK_DELAY_3M(n)            (((n) & 0x7F) << 15)
#define CODEC_CFG_CLASSD_0P6_FIND_OK_DELAY_3M_MASK          (0x7F << 15)
#define CODEC_CFG_CLASSD_0P6_FIND_OK_DELAY_3M_SHIFT         (15)
#define CODEC_CFG_CLASSD_0P6_LPF_STABLE0_THR(n)             (((n) & 0x3FF) << 22)
#define CODEC_CFG_CLASSD_0P6_LPF_STABLE0_THR_MASK           (0x3FF << 22)
#define CODEC_CFG_CLASSD_0P6_LPF_STABLE0_THR_SHIFT          (22)

// reg_524
#define CODEC_CFG_CLASSD_0P6_LPF_STABLE1_THR(n)             (((n) & 0x3FF) << 0)
#define CODEC_CFG_CLASSD_0P6_LPF_STABLE1_THR_MASK           (0x3FF << 0)
#define CODEC_CFG_CLASSD_0P6_LPF_STABLE1_THR_SHIFT          (0)
#define CODEC_CFG_CLASSD_1P2_MASKING_CF_MODE                (1 << 10)
#define CODEC_CFG_CLASSD_1P2_LPF_F9_ALPHA(n)                (((n) & 0xF) << 11)
#define CODEC_CFG_CLASSD_1P2_LPF_F9_ALPHA_MASK              (0xF << 11)
#define CODEC_CFG_CLASSD_1P2_LPF_F9_ALPHA_SHIFT             (11)
#define CODEC_CFG_CLASSD_1P2_LPF_F11_ALPHA(n)               (((n) & 0xF) << 15)
#define CODEC_CFG_CLASSD_1P2_LPF_F11_ALPHA_MASK             (0xF << 15)
#define CODEC_CFG_CLASSD_1P2_LPF_F11_ALPHA_SHIFT            (15)
#define CODEC_CFG_CLASSD_1P2_LPF_F9_ALPHA_STABLE(n)         (((n) & 0xF) << 19)
#define CODEC_CFG_CLASSD_1P2_LPF_F9_ALPHA_STABLE_MASK       (0xF << 19)
#define CODEC_CFG_CLASSD_1P2_LPF_F9_ALPHA_STABLE_SHIFT      (19)
#define CODEC_CFG_CLASSD_1P2_LPF_F11_ALPHA_STABLE(n)        (((n) & 0xF) << 23)
#define CODEC_CFG_CLASSD_1P2_LPF_F11_ALPHA_STABLE_MASK      (0xF << 23)
#define CODEC_CFG_CLASSD_1P2_LPF_F11_ALPHA_STABLE_SHIFT     (23)

// reg_528
#define CODEC_CFG_CLASSD_1P2_MASKING_CF(n)                  (((n) & 0xFFFFFF) << 0)
#define CODEC_CFG_CLASSD_1P2_MASKING_CF_MASK                (0xFFFFFF << 0)
#define CODEC_CFG_CLASSD_1P2_MASKING_CF_SHIFT               (0)
#define CODEC_CFG_CLASSD_1P2_FIND_OK_DELAY_48K(n)           (((n) & 0x7F) << 24)
#define CODEC_CFG_CLASSD_1P2_FIND_OK_DELAY_48K_MASK         (0x7F << 24)
#define CODEC_CFG_CLASSD_1P2_FIND_OK_DELAY_48K_SHIFT        (24)

// reg_52c
#define CODEC_CFG_CLASSD_1P2_FIND_OK_DELAY_3M(n)            (((n) & 0x7F) << 0)
#define CODEC_CFG_CLASSD_1P2_FIND_OK_DELAY_3M_MASK          (0x7F << 0)
#define CODEC_CFG_CLASSD_1P2_FIND_OK_DELAY_3M_SHIFT         (0)
#define CODEC_CFG_CLASSD_1P2_LPF_STABLE0_THR(n)             (((n) & 0x3FF) << 7)
#define CODEC_CFG_CLASSD_1P2_LPF_STABLE0_THR_MASK           (0x3FF << 7)
#define CODEC_CFG_CLASSD_1P2_LPF_STABLE0_THR_SHIFT          (7)
#define CODEC_CFG_CLASSD_1P2_LPF_STABLE1_THR(n)             (((n) & 0x3FF) << 17)
#define CODEC_CFG_CLASSD_1P2_LPF_STABLE1_THR_MASK           (0x3FF << 17)
#define CODEC_CFG_CLASSD_1P2_LPF_STABLE1_THR_SHIFT          (17)

// reg_530
#define CODEC_CFG_CLASSD_1P2_HPF_F9_COEF_A1(n)              (((n) & 0xFFFFF) << 0)
#define CODEC_CFG_CLASSD_1P2_HPF_F9_COEF_A1_MASK            (0xFFFFF << 0)
#define CODEC_CFG_CLASSD_1P2_HPF_F9_COEF_A1_SHIFT           (0)

// reg_534
#define CODEC_CFG_CLASSD_1P2_HPF_F9_COEF_A2(n)              (((n) & 0xFFFFF) << 0)
#define CODEC_CFG_CLASSD_1P2_HPF_F9_COEF_A2_MASK            (0xFFFFF << 0)
#define CODEC_CFG_CLASSD_1P2_HPF_F9_COEF_A2_SHIFT           (0)

// reg_538
#define CODEC_CFG_CLASSD_1P2_HPF_F9_COEF_B0(n)              (((n) & 0xFFFFF) << 0)
#define CODEC_CFG_CLASSD_1P2_HPF_F9_COEF_B0_MASK            (0xFFFFF << 0)
#define CODEC_CFG_CLASSD_1P2_HPF_F9_COEF_B0_SHIFT           (0)

// reg_53c
#define CODEC_CFG_CLASSD_1P2_HPF_F9_COEF_B1(n)              (((n) & 0xFFFFF) << 0)
#define CODEC_CFG_CLASSD_1P2_HPF_F9_COEF_B1_MASK            (0xFFFFF << 0)
#define CODEC_CFG_CLASSD_1P2_HPF_F9_COEF_B1_SHIFT           (0)

// reg_540
#define CODEC_CFG_CLASSD_1P2_HPF_F9_COEF_B2(n)              (((n) & 0xFFFFF) << 0)
#define CODEC_CFG_CLASSD_1P2_HPF_F9_COEF_B2_MASK            (0xFFFFF << 0)
#define CODEC_CFG_CLASSD_1P2_HPF_F9_COEF_B2_SHIFT           (0)

// reg_544
#define CODEC_CFG_CLASSD_1P2_HPF_F11_COEF_A1(n)             (((n) & 0xFFFFF) << 0)
#define CODEC_CFG_CLASSD_1P2_HPF_F11_COEF_A1_MASK           (0xFFFFF << 0)
#define CODEC_CFG_CLASSD_1P2_HPF_F11_COEF_A1_SHIFT          (0)

// reg_548
#define CODEC_CFG_CLASSD_1P2_HPF_F11_COEF_A2(n)             (((n) & 0xFFFFF) << 0)
#define CODEC_CFG_CLASSD_1P2_HPF_F11_COEF_A2_MASK           (0xFFFFF << 0)
#define CODEC_CFG_CLASSD_1P2_HPF_F11_COEF_A2_SHIFT          (0)

// reg_54c
#define CODEC_CFG_CLASSD_1P2_HPF_F11_COEF_B0(n)             (((n) & 0xFFFFF) << 0)
#define CODEC_CFG_CLASSD_1P2_HPF_F11_COEF_B0_MASK           (0xFFFFF << 0)
#define CODEC_CFG_CLASSD_1P2_HPF_F11_COEF_B0_SHIFT          (0)

// reg_550
#define CODEC_CFG_CLASSD_1P2_HPF_F11_COEF_B1(n)             (((n) & 0xFFFFF) << 0)
#define CODEC_CFG_CLASSD_1P2_HPF_F11_COEF_B1_MASK           (0xFFFFF << 0)
#define CODEC_CFG_CLASSD_1P2_HPF_F11_COEF_B1_SHIFT          (0)

// reg_554
#define CODEC_CFG_CLASSD_1P2_HPF_F11_COEF_B2(n)             (((n) & 0xFFFFF) << 0)
#define CODEC_CFG_CLASSD_1P2_HPF_F11_COEF_B2_MASK           (0xFFFFF << 0)
#define CODEC_CFG_CLASSD_1P2_HPF_F11_COEF_B2_SHIFT          (0)

// reg_608
#define CODEC_CODEC_DOWN_SEL_FREE(n)                        (((n) & 0x3) << 0)
#define CODEC_CODEC_DOWN_SEL_FREE_MASK                      (0x3 << 0)
#define CODEC_CODEC_DOWN_SEL_FREE_SHIFT                     (0)

// reg_60c
#define CODEC_CODEC_ADC_HBF3_BYPASS_FREE                    (1 << 0)
#define CODEC_CODEC_ADC_HBF2_BYPASS_FREE                    (1 << 1)
#define CODEC_CODEC_ADC_HBF1_BYPASS_FREE                    (1 << 2)

// reg_610
#define CODEC_PVT_RO_EN                                     (1 << 0)
#define CODEC_PVT_COUNT_START_EN                            (1 << 1)
#define CODEC_PVT_OSC_CLK_EN                                (1 << 2)
#define CODEC_PVT_RSTN                                      (1 << 3)
#define CODEC_PVT_REG_TIMESET(n)                            (((n) & 0x3FF) << 4)
#define CODEC_PVT_REG_TIMESET_MASK                          (0x3FF << 4)
#define CODEC_PVT_REG_TIMESET_SHIFT                         (4)

// reg_614
#define CODEC_PVT_COUNT_VALUE_SYNC(n)                       (((n) & 0x3FFFF) << 0)
#define CODEC_PVT_COUNT_VALUE_SYNC_MASK                     (0x3FFFF << 0)
#define CODEC_PVT_COUNT_VALUE_SYNC_SHIFT                    (0)
#define CODEC_PVT_COUNT_DONE_SYNC_2                         (1 << 18)

// reg_618
#define CODEC_DAC_EN_CLASS_SEL                              (1 << 0)
#define CODEC_CLASD_DITHER_BYPASS                           (1 << 1)
#define CODEC_CLASD_DITHER_GAIN(n)                          (((n) & 0x1F) << 2)
#define CODEC_CLASD_DITHER_GAIN_MASK                        (0x1F << 2)
#define CODEC_CLASD_DITHER_GAIN_SHIFT                       (2)
#define CODEC_CLASD_SDM_GAIN(n)                             (((n) & 0x7) << 7)
#define CODEC_CLASD_SDM_GAIN_MASK                           (0x7 << 7)
#define CODEC_CLASD_SDM_GAIN_SHIFT                          (7)
#define CODEC_CLASD_DAC_SDM_CLOSE                           (1 << 10)
#define CODEC_CLASD_DAC_H7_DELAY_CH0(n)                     (((n) & 0x3) << 11)
#define CODEC_CLASD_DAC_H7_DELAY_CH0_MASK                   (0x3 << 11)
#define CODEC_CLASD_DAC_H7_DELAY_CH0_SHIFT                  (11)
#define CODEC_CLASD_DAC_L5_DELAY_CH0(n)                     (((n) & 0x3) << 13)
#define CODEC_CLASD_DAC_L5_DELAY_CH0_MASK                   (0x3 << 13)
#define CODEC_CLASD_DAC_L5_DELAY_CH0_SHIFT                  (13)
#define CODEC_CLK_CLASD_DAC_OUT_PHASE(n)                    (((n) & 0x3) << 15)
#define CODEC_CLK_CLASD_DAC_OUT_PHASE_MASK                  (0x3 << 15)
#define CODEC_CLK_CLASD_DAC_OUT_PHASE_SHIFT                 (15)

// reg_61c
#define CODEC_CFG_CLASSD_L_ENABLE                           (1 << 0)
#define CODEC_CFG_WINDOW_WIDTH_OFF(n)                       (((n) & 0xFFFF) << 1)
#define CODEC_CFG_WINDOW_WIDTH_OFF_MASK                     (0xFFFF << 1)
#define CODEC_CFG_WINDOW_WIDTH_OFF_SHIFT                    (1)
#define CODEC_CFG_STEP_UP_DELAY(n)                          (((n) & 0x7FF) << 17)
#define CODEC_CFG_STEP_UP_DELAY_MASK                        (0x7FF << 17)
#define CODEC_CFG_STEP_UP_DELAY_SHIFT                       (17)
#define CODEC_CFG_QUICK_DOWN                                (1 << 28)

// reg_620
#define CODEC_CFG_WINDOW_WIDTH(n)                           (((n) & 0x7FFFFF) << 0)
#define CODEC_CFG_WINDOW_WIDTH_MASK                         (0x7FFFFF << 0)
#define CODEC_CFG_WINDOW_WIDTH_SHIFT                        (0)
#define CODEC_CFG_CLASSD_STEP0_PATRN_RSTN0_DELAY2_L(n)      (((n) & 0x7F) << 23)
#define CODEC_CFG_CLASSD_STEP0_PATRN_RSTN0_DELAY2_L_MASK    (0x7F << 23)
#define CODEC_CFG_CLASSD_STEP0_PATRN_RSTN0_DELAY2_L_SHIFT   (23)

// reg_624
#define CODEC_CFG_VTOI_EN_CLASSD_EN_DELAY_L(n)              (((n) & 0x1F) << 0)
#define CODEC_CFG_VTOI_EN_CLASSD_EN_DELAY_L_MASK            (0x1F << 0)
#define CODEC_CFG_VTOI_EN_CLASSD_EN_DELAY_L_SHIFT           (0)
#define CODEC_CFG_CLASSD_EN_CLASSD_RSTN_DELAY_L(n)          (((n) & 0x7FF) << 5)
#define CODEC_CFG_CLASSD_EN_CLASSD_RSTN_DELAY_L_MASK        (0x7FF << 5)
#define CODEC_CFG_CLASSD_EN_CLASSD_RSTN_DELAY_L_SHIFT       (5)
#define CODEC_CFG_CLASSD_RSTN_DAC_ENABLE_DELAY_L(n)         (((n) & 0x1F) << 16)
#define CODEC_CFG_CLASSD_RSTN_DAC_ENABLE_DELAY_L_MASK       (0x1F << 16)
#define CODEC_CFG_CLASSD_RSTN_DAC_ENABLE_DELAY_L_SHIFT      (16)
#define CODEC_CFG_CLASSD_RSTN0_PATRN_RSTN_DELAY_L(n)        (((n) & 0x1F) << 21)
#define CODEC_CFG_CLASSD_RSTN0_PATRN_RSTN_DELAY_L_MASK      (0x1F << 21)
#define CODEC_CFG_CLASSD_RSTN0_PATRN_RSTN_DELAY_L_SHIFT     (21)
#define CODEC_CFG_PATRN_RSTN_CLASSD_EN0_DELAY_L(n)          (((n) & 0x1F) << 26)
#define CODEC_CFG_PATRN_RSTN_CLASSD_EN0_DELAY_L_MASK        (0x1F << 26)
#define CODEC_CFG_PATRN_RSTN_CLASSD_EN0_DELAY_L_SHIFT       (26)

// reg_628
#define CODEC_CFG_CLASSD_STEP0_PATRN_RSTN0_DELAY_L(n)       (((n) & 0x7F) << 0)
#define CODEC_CFG_CLASSD_STEP0_PATRN_RSTN0_DELAY_L_MASK     (0x7F << 0)
#define CODEC_CFG_CLASSD_STEP0_PATRN_RSTN0_DELAY_L_SHIFT    (0)
#define CODEC_CFG_VLG_THRESHOLD_1_L(n)                      (((n) & 0xFF) << 7)
#define CODEC_CFG_VLG_THRESHOLD_1_L_MASK                    (0xFF << 7)
#define CODEC_CFG_VLG_THRESHOLD_1_L_SHIFT                   (7)
#define CODEC_CFG_VLG_THRESHOLD_0_L(n)                      (((n) & 0xFF) << 15)
#define CODEC_CFG_VLG_THRESHOLD_0_L_MASK                    (0xFF << 15)
#define CODEC_CFG_VLG_THRESHOLD_0_L_SHIFT                   (15)
#define CODEC_CFG_VLG_START_COND_L                          (1 << 23)
#define CODEC_CFG_GAIN_SW_DLY_BYPASS_L                      (1 << 24)
#define CODEC_CFG_VLG_TH0_DLY_BYPASS_L                      (1 << 25)
#define CODEC_CFG_VLG_TH1_DLY_BYPASS_L                      (1 << 26)
#define CODEC_CFG_CLASSD_OCP_MODE_L                         (1 << 27)
#define CODEC_CFG_CLASSD_OVERFLOW_MODE_L                    (1 << 28)

// reg_62c
#define CODEC_CLASSD_OVERFLOW_INTR_CLEAR_L                  (1 << 0)
#define CODEC_CLASSD_OVERFLOW_INTR_MSK_L                    (1 << 1)
#define CODEC_CLASSD_OVERFLOW_MUSIC_REOPEN_L                (1 << 2)
#define CODEC_CLASSD_OCP_INTR_CLEAR_L                       (1 << 3)
#define CODEC_CLASSD_OCP_INTR_MSK_L                         (1 << 4)
#define CODEC_CLASSD_OCP_MUSIC_REOPEN_L                     (1 << 5)
#define CODEC_CLASSD_OVFW_INTR_STATUS_L_SYNC_1              (1 << 6)
#define CODEC_CLASSD_OCP_INTR_STATUS_L_SYNC_1               (1 << 7)

// reg_630
#define CODEC_CLASSD_DRE_ENABLE_L                           (1 << 0)
#define CODEC_CFG_DRE_WAIT_ANA_MODE_L                       (1 << 1)
#define CODEC_CFG_GAIN_PASS0_UPT_MODE_L                     (1 << 2)
#define CODEC_CFG_PASS0_AMP_SEL_L(n)                        (((n) & 0x3) << 3)
#define CODEC_CFG_PASS0_AMP_SEL_L_MASK                      (0x3 << 3)
#define CODEC_CFG_PASS0_AMP_SEL_L_SHIFT                     (3)
#define CODEC_CFG_ANA_PASS0_BYPASS_MODE_L                   (1 << 5)
#define CODEC_CFG_OVFW_PASS0_UPT_BYPASS_L                   (1 << 6)
#define CODEC_CFG_DRE_FLOOR_INI_L(n)                        (((n) & 0x7) << 7)
#define CODEC_CFG_DRE_FLOOR_INI_L_MASK                      (0x7 << 7)
#define CODEC_CFG_DRE_FLOOR_INI_L_SHIFT                     (7)
#define CODEC_CFG_CLASSD_DIG_GAIN_INI_L(n)                  (((n) & 0x7) << 10)
#define CODEC_CFG_CLASSD_DIG_GAIN_INI_L_MASK                (0x7 << 10)
#define CODEC_CFG_CLASSD_DIG_GAIN_INI_L_SHIFT               (10)
#define CODEC_CLASSD_MODE_INI_L                             (1 << 13)

// reg_634
#define CODEC_CFG_DRE_GAIN_0_L                              (1 << 0)
#define CODEC_CFG_DRE_GAIN_1_L(n)                           (((n) & 0x3) << 1)
#define CODEC_CFG_DRE_GAIN_1_L_MASK                         (0x3 << 1)
#define CODEC_CFG_DRE_GAIN_1_L_SHIFT                        (1)
#define CODEC_CFG_DRE_GAIN_2_L(n)                           (((n) & 0x3) << 3)
#define CODEC_CFG_DRE_GAIN_2_L_MASK                         (0x3 << 3)
#define CODEC_CFG_DRE_GAIN_2_L_SHIFT                        (3)
#define CODEC_CFG_DRE_GAIN_3_L                              (1 << 5)
#define CODEC_CFG_DRE_GAIN_4_L                              (1 << 6)

// reg_638
#define CODEC_CFG_DRE_WINDOW_WIDTH_L(n)                     (((n) & 0x7FFFFF) << 0)
#define CODEC_CFG_DRE_WINDOW_WIDTH_L_MASK                   (0x7FFFFF << 0)
#define CODEC_CFG_DRE_WINDOW_WIDTH_L_SHIFT                  (0)

// reg_63c
#define CODEC_CFG_DRE_PASS0_UPDATE_DELAY_L(n)               (((n) & 0x1F) << 0)
#define CODEC_CFG_DRE_PASS0_UPDATE_DELAY_L_MASK             (0x1F << 0)
#define CODEC_CFG_DRE_PASS0_UPDATE_DELAY_L_SHIFT            (0)
#define CODEC_CFG_DRE_DC_DELAY_L(n)                         (((n) & 0xFF) << 5)
#define CODEC_CFG_DRE_DC_DELAY_L_MASK                       (0xFF << 5)
#define CODEC_CFG_DRE_DC_DELAY_L_SHIFT                      (5)
#define CODEC_CFG_DRE_DIG_ANA_GAIN_DELAY_L(n)               (((n) & 0xFF) << 13)
#define CODEC_CFG_DRE_DIG_ANA_GAIN_DELAY_L_MASK             (0xFF << 13)
#define CODEC_CFG_DRE_DIG_ANA_GAIN_DELAY_L_SHIFT            (13)
#define CODEC_CFG_DRE_CLASSD_GAIN_OFFSET_L(n)               (((n) & 0x7) << 21)
#define CODEC_CFG_DRE_CLASSD_GAIN_OFFSET_L_MASK             (0x7 << 21)
#define CODEC_CFG_DRE_CLASSD_GAIN_OFFSET_L_SHIFT            (21)

// reg_640
#define CODEC_CFG_DRE_CLASSD_STEP0_L(n)                     (((n) & 0xFFFF) << 0)
#define CODEC_CFG_DRE_CLASSD_STEP0_L_MASK                   (0xFFFF << 0)
#define CODEC_CFG_DRE_CLASSD_STEP0_L_SHIFT                  (0)
#define CODEC_CFG_DRE_CLASSD_STEP1_L(n)                     (((n) & 0xFFFF) << 16)
#define CODEC_CFG_DRE_CLASSD_STEP1_L_MASK                   (0xFFFF << 16)
#define CODEC_CFG_DRE_CLASSD_STEP1_L_SHIFT                  (16)

// reg_644
#define CODEC_CFG_DRE_CLASSD_STEP2_L(n)                     (((n) & 0xFFFF) << 0)
#define CODEC_CFG_DRE_CLASSD_STEP2_L_MASK                   (0xFFFF << 0)
#define CODEC_CFG_DRE_CLASSD_STEP2_L_SHIFT                  (0)
#define CODEC_CFG_DRE_CLASSD_STEP3_L(n)                     (((n) & 0xFFFF) << 16)
#define CODEC_CFG_DRE_CLASSD_STEP3_L_MASK                   (0xFFFF << 16)
#define CODEC_CFG_DRE_CLASSD_STEP3_L_SHIFT                  (16)

// reg_648
#define CODEC_CFG_DRE_CLASSD_STEP4_L(n)                     (((n) & 0xFFFF) << 0)
#define CODEC_CFG_DRE_CLASSD_STEP4_L_MASK                   (0xFFFF << 0)
#define CODEC_CFG_DRE_CLASSD_STEP4_L_SHIFT                  (0)
#define CODEC_CFG_DRE_CLASSD_DC4_L(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_CFG_DRE_CLASSD_DC4_L_MASK                     (0xFFFF << 16)
#define CODEC_CFG_DRE_CLASSD_DC4_L_SHIFT                    (16)

// reg_64c
#define CODEC_CFG_DRE_CLASSD_DC0_L(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_CFG_DRE_CLASSD_DC0_L_MASK                     (0xFFFF << 0)
#define CODEC_CFG_DRE_CLASSD_DC0_L_SHIFT                    (0)
#define CODEC_CFG_DRE_CLASSD_DC1_L(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_CFG_DRE_CLASSD_DC1_L_MASK                     (0xFFFF << 16)
#define CODEC_CFG_DRE_CLASSD_DC1_L_SHIFT                    (16)

// reg_650
#define CODEC_CFG_DRE_CLASSD_DC2_L(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_CFG_DRE_CLASSD_DC2_L_MASK                     (0xFFFF << 0)
#define CODEC_CFG_DRE_CLASSD_DC2_L_SHIFT                    (0)
#define CODEC_CFG_DRE_CLASSD_DC3_L(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_CFG_DRE_CLASSD_DC3_L_MASK                     (0xFFFF << 16)
#define CODEC_CFG_DRE_CLASSD_DC3_L_SHIFT                    (16)

// reg_654
#define CODEC_CFG_DRE_THRESHOLD_0_L(n)                      (((n) & 0xFFFF) << 0)
#define CODEC_CFG_DRE_THRESHOLD_0_L_MASK                    (0xFFFF << 0)
#define CODEC_CFG_DRE_THRESHOLD_0_L_SHIFT                   (0)
#define CODEC_CFG_DRE_THRESHOLD_1_L(n)                      (((n) & 0xFFFF) << 16)
#define CODEC_CFG_DRE_THRESHOLD_1_L_MASK                    (0xFFFF << 16)
#define CODEC_CFG_DRE_THRESHOLD_1_L_SHIFT                   (16)

// reg_658
#define CODEC_CFG_DRE_THRESHOLD_2_L(n)                      (((n) & 0xFFFF) << 0)
#define CODEC_CFG_DRE_THRESHOLD_2_L_MASK                    (0xFFFF << 0)
#define CODEC_CFG_DRE_THRESHOLD_2_L_SHIFT                   (0)
#define CODEC_CFG_DRE_THRESHOLD_3_L(n)                      (((n) & 0xFFFF) << 16)
#define CODEC_CFG_DRE_THRESHOLD_3_L_MASK                    (0xFFFF << 16)
#define CODEC_CFG_DRE_THRESHOLD_3_L_SHIFT                   (16)

// reg_65c
#define CODEC_CFG_DRE_AMP_HIGH_L(n)                         (((n) & 0xFFFF) << 0)
#define CODEC_CFG_DRE_AMP_HIGH_L_MASK                       (0xFFFF << 0)
#define CODEC_CFG_DRE_AMP_HIGH_L_SHIFT                      (0)
#define CODEC_CFG_EIG_MODE_D_EXIT_BYPASS_L                  (1 << 16)
#define CODEC_CFG_QUAR_MODE_D_EXIT_BYPASS_L                 (1 << 17)
#define CODEC_CFG_HALF_MODE_D_EXIT_BYPASS_L                 (1 << 18)
#define CODEC_CFG_GAIN_D_EXIT_BYPASS_L                      (1 << 19)
#define CODEC_CFG_EIG_MODE_D_ENTER_BYPASS_L                 (1 << 20)
#define CODEC_CFG_QUAR_MODE_D_ENTER_BYPASS_L                (1 << 21)
#define CODEC_CFG_HALF_MODE_D_ENTER_BYPASS_L                (1 << 22)
#define CODEC_CFG_GAIN_D_ENTER_BYPASS_L                     (1 << 23)
#define CODEC_CFG_CLASSD_0P3MODE_BYPASS_L                   (1 << 24)

// reg_660
#define CODEC_CFG_CLASSD_RSTN_L_DR                          (1 << 0)
#define CODEC_CFG_CLASSD_OFFSET_EN_L_DR                     (1 << 1)
#define CODEC_CFG_CLASSD_VTOI_EN_L_DR                       (1 << 2)
#define CODEC_CFG_CLASSD_EN_L_DR                            (1 << 3)
#define CODEC_CFG_CLASSD_PATTERN_RSTN_L_DR                  (1 << 4)
#define CODEC_CFG_CLASSD_DAC_ENABLE_L_DR                    (1 << 5)
#define CODEC_CFG_CLASSD_0P3_MODE_L_DR                      (1 << 6)
#define CODEC_CFG_CLASSD_0P6_MODE_L_DR                      (1 << 7)
#define CODEC_CFG_CLASSD_1P2_MODE_L_DR                      (1 << 8)
#define CODEC_CFG_CLASSD_1P8_MODE_L_DR                      (1 << 9)
#define CODEC_CFG_CLASSD_EIG_MODE_L_DR                      (1 << 10)
#define CODEC_CFG_CLASSD_QUAR_MODE_L_DR                     (1 << 11)
#define CODEC_CFG_CLASSD_HALF_MODE_L_DR                     (1 << 12)
#define CODEC_CFG_CLASSD_GAIN_L_DR                          (1 << 13)
#define CODEC_CFG_CLASSD_GAIN_N6DB_L_DR                     (1 << 14)
#define CODEC_CFG_CLASSD_GAIN_N12DB_L_DR                    (1 << 15)
#define CODEC_CFG_CLASSD_GAIN_N18DB_L_DR                    (1 << 16)
#define CODEC_CFG_CLASSD_GAIN_LN_L_DR                       (1 << 17)
#define CODEC_CFG_CLASSD_GAIN_N12DB_N22DB_DR                (1 << 18)
#define CODEC_CFG_CLASSD_EIG_PATTERN_BYPASS_L_DR            (1 << 19)
#define CODEC_CFG_CLASSD_QUARTER_PATTERN_BYPASS_L_DR        (1 << 20)
#define CODEC_CFG_CLASSD_HALF_PATTERN_BYPASS_L_DR           (1 << 21)
#define CODEC_CFG_CLASSD_GAIN_PATTERN_BYPASS_L_DR           (1 << 22)

// reg_664
#define CODEC_REG_CLASSD_RSTN_L                             (1 << 0)
#define CODEC_REG_CLASSD_OFFSET_EN_L                        (1 << 1)
#define CODEC_REG_CLASSD_PATTERN_RSTN_L                     (1 << 2)
#define CODEC_REG_CLASSD_EN_L                               (1 << 3)
#define CODEC_REG_CLASSD_VTOI_EN_L                          (1 << 4)
#define CODEC_REG_CLASSD_DAC_ENABLE_L                       (1 << 5)
#define CODEC_REG_CLASSD_0P3_MODE_L                         (1 << 6)
#define CODEC_REG_CLASSD_0P6_MODE_L                         (1 << 7)
#define CODEC_REG_CLASSD_1P2_MODE_L                         (1 << 8)
#define CODEC_REG_CLASSD_1P8_MODE_L                         (1 << 9)
#define CODEC_REG_CLASSD_EIG_MODE_L                         (1 << 10)
#define CODEC_REG_CLASSD_QUAR_MODE_L                        (1 << 11)
#define CODEC_REG_CLASSD_HALF_MODE_L                        (1 << 12)
#define CODEC_REG_CLASSD_GAIN_L(n)                          (((n) & 0x3) << 13)
#define CODEC_REG_CLASSD_GAIN_L_MASK                        (0x3 << 13)
#define CODEC_REG_CLASSD_GAIN_L_SHIFT                       (13)
#define CODEC_REG_CLASSD_GAIN_N6DB_L                        (1 << 15)
#define CODEC_REG_CLASSD_GAIN_N12DB_L                       (1 << 16)
#define CODEC_REG_CLASSD_GAIN_N18DB_L                       (1 << 17)
#define CODEC_REG_CLASSD_GAIN_LN_L(n)                       (((n) & 0xFF) << 18)
#define CODEC_REG_CLASSD_GAIN_LN_L_MASK                     (0xFF << 18)
#define CODEC_REG_CLASSD_GAIN_LN_L_SHIFT                    (18)
#define CODEC_REG_CLASSD_GAIN_N12DB_N22DB                   (1 << 26)
#define CODEC_REG_CLASSD_EIG_PATTERN_BYPASS_L               (1 << 27)
#define CODEC_REG_CLASSD_QUARTER_PATTERN_BYPASS_L           (1 << 28)
#define CODEC_REG_CLASSD_HALF_PATTERN_BYPASS_L              (1 << 29)
#define CODEC_REG_CLASSD_GAIN_PATTERN_BYPASS_L              (1 << 30)

// reg_668
#define CODEC_CFG_CLASSD_CLK_SW_EN                          (1 << 0)
#define CODEC_SOFT_RSTN_CLK_SW_IN                           (1 << 1)
#define CODEC_CFG_CLASSD_CLK_SW_PRD(n)                      (((n) & 0xFF) << 2)
#define CODEC_CFG_CLASSD_CLK_SW_PRD_MASK                    (0xFF << 2)
#define CODEC_CFG_CLASSD_CLK_SW_PRD_SHIFT                   (2)
#define CODEC_CFG_CLASSD_CLK_SW_START(n)                    (((n) & 0x3FF) << 10)
#define CODEC_CFG_CLASSD_CLK_SW_START_MASK                  (0x3FF << 10)
#define CODEC_CFG_CLASSD_CLK_SW_START_SHIFT                 (10)
#define CODEC_CFG_CLASSD_CLK_SW_TIME(n)                     (((n) & 0x3FF) << 20)
#define CODEC_CFG_CLASSD_CLK_SW_TIME_MASK                   (0x3FF << 20)
#define CODEC_CFG_CLASSD_CLK_SW_TIME_SHIFT                  (20)
#define CODEC_REG_CLASSD_CLK_SW_IN                          (1 << 30)
#define CODEC_REG_CLASSD_CLK_SW_IN_DR                       (1 << 31)

// reg_66c
#define CODEC_DEBUG_DRE_MST_STATE_L(n)                      (((n) & 0x3F) << 0)
#define CODEC_DEBUG_DRE_MST_STATE_L_MASK                    (0x3F << 0)
#define CODEC_DEBUG_DRE_MST_STATE_L_SHIFT                   (0)
#define CODEC_DEBUG_DRE_SLV_STATE_L(n)                      (((n) & 0xF) << 6)
#define CODEC_DEBUG_DRE_SLV_STATE_L_MASK                    (0xF << 6)
#define CODEC_DEBUG_DRE_SLV_STATE_L_SHIFT                   (6)
#define CODEC_DEBUG_VLG_STATE_L(n)                          (((n) & 0xF) << 10)
#define CODEC_DEBUG_VLG_STATE_L_MASK                        (0xF << 10)
#define CODEC_DEBUG_VLG_STATE_L_SHIFT                       (10)
#define CODEC_DEBUG_CLASSD_STATE_L(n)                       (((n) & 0x7) << 14)
#define CODEC_DEBUG_CLASSD_STATE_L_MASK                     (0x7 << 14)
#define CODEC_DEBUG_CLASSD_STATE_L_SHIFT                    (14)
#define CODEC_DEBUG_CLASSD_DAC_ENABLE_L                     (1 << 17)
#define CODEC_DEBUG_CLASSD_RSTN_L                           (1 << 18)
#define CODEC_DEBUG_GAIN_L(n)                               (((n) & 0x3) << 19)
#define CODEC_DEBUG_GAIN_L_MASK                             (0x3 << 19)
#define CODEC_DEBUG_GAIN_L_SHIFT                            (19)
#define CODEC_DEBUG_GAIN_N6DB_L                             (1 << 21)
#define CODEC_DEBUG_GAIN_N12DB_L                            (1 << 22)
#define CODEC_DEBUG_QUARTER_MODE_L                          (1 << 23)
#define CODEC_DEBUG_HALF_MODE_L                             (1 << 24)
#define CODEC_DEBUG_0P6_MODE_L                              (1 << 25)
#define CODEC_DEBUG_1P2_MODE_L                              (1 << 26)
#define CODEC_DEBUG_1P8_MODE_L                              (1 << 27)
#define CODEC_DEBUG_QUARTER_PATTERN_BYPASS_L                (1 << 28)
#define CODEC_DEBUG_HALF_PATTERN_BYPASS_L                   (1 << 29)
#define CODEC_DEBUG_GAIN_PATTERN_BYPASS_L                   (1 << 30)

// reg_670
#define CODEC_DEBUG_CLASSD_GAIN_D_L                         (1 << 0)
#define CODEC_DEBUG_CLASSD_HALF_MODE_D_L                    (1 << 1)
#define CODEC_DEBUG_CLASSD_QUAR_MODE_D_L                    (1 << 2)
#define CODEC_DEBUG_CLASSD_PATTERN_RESET_D_L                (1 << 3)
#define CODEC_DEBUG_CLASSD_OVERFLOW_L                       (1 << 4)
#define CODEC_DEBUG_CLASSD_OCP_L                            (1 << 5)
#define CODEC_DEBUG_DRE_WHICH_OVFW_L(n)                     (((n) & 0x3F) << 6)
#define CODEC_DEBUG_DRE_WHICH_OVFW_L_MASK                   (0x3F << 6)
#define CODEC_DEBUG_DRE_WHICH_OVFW_L_SHIFT                  (6)
#define CODEC_DEBUG_GAIN_LN_L(n)                            (((n) & 0xFF) << 12)
#define CODEC_DEBUG_GAIN_LN_L_MASK                          (0xFF << 12)
#define CODEC_DEBUG_GAIN_LN_L_SHIFT                         (12)
#define CODEC_DEBUG_0P3_MODE_L                              (1 << 20)

// reg_674
#define CODEC_CFG_CLASSD_N12_N22_ENABLE                     (1 << 0)
#define CODEC_CFG_CLASSD_N12_N22_STEP(n)                    (((n) & 0x3) << 1)
#define CODEC_CFG_CLASSD_N12_N22_STEP_MASK                  (0x3 << 1)
#define CODEC_CFG_CLASSD_N12_N22_STEP_SHIFT                 (1)
#define CODEC_CFG_CLASSD_ANA_DIG_GAIN_LN_INDEX_INI(n)       (((n) & 0xF) << 3)
#define CODEC_CFG_CLASSD_ANA_DIG_GAIN_LN_INDEX_INI_MASK     (0xF << 3)
#define CODEC_CFG_CLASSD_ANA_DIG_GAIN_LN_INDEX_INI_SHIFT    (3)
#define CODEC_CFG_CLASSD_GAIN_LN_INTERVAL(n)                (((n) & 0x3FF) << 7)
#define CODEC_CFG_CLASSD_GAIN_LN_INTERVAL_MASK              (0x3FF << 7)
#define CODEC_CFG_CLASSD_GAIN_LN_INTERVAL_SHIFT             (7)
#define CODEC_CFG_CLASSD_LN_GAIN_INI(n)                     (((n) & 0xFF) << 17)
#define CODEC_CFG_CLASSD_LN_GAIN_INI_MASK                   (0xFF << 17)
#define CODEC_CFG_CLASSD_LN_GAIN_INI_SHIFT                  (17)

// reg_678
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_0(n)               (((n) & 0xFF) << 0)
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_0_MASK             (0xFF << 0)
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_0_SHIFT            (0)
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_1(n)               (((n) & 0xFF) << 8)
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_1_MASK             (0xFF << 8)
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_1_SHIFT            (8)
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_2(n)               (((n) & 0xFF) << 16)
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_2_MASK             (0xFF << 16)
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_2_SHIFT            (16)
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_3(n)               (((n) & 0xFF) << 24)
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_3_MASK             (0xFF << 24)
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_3_SHIFT            (24)

// reg_67c
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_4(n)               (((n) & 0xFF) << 0)
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_4_MASK             (0xFF << 0)
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_4_SHIFT            (0)
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_5(n)               (((n) & 0xFF) << 8)
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_5_MASK             (0xFF << 8)
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_5_SHIFT            (8)
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_6(n)               (((n) & 0xFF) << 16)
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_6_MASK             (0xFF << 16)
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_6_SHIFT            (16)
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_7(n)               (((n) & 0xFF) << 24)
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_7_MASK             (0xFF << 24)
#define CODEC_CFG_CLASSD_GAIN_LN_ANAGAIN_7_SHIFT            (24)

// reg_680
#define CODEC_CODEC_IIR4_ENABLE                             (1 << 0)
#define CODEC_CODEC_IIR4_IIRA_ENABLE                        (1 << 1)
#define CODEC_CODEC_IIR4_IIRB_ENABLE                        (1 << 2)
#define CODEC_CODEC_IIR4_BYPASS                             (1 << 3)
#define CODEC_CODEC_IIR4_GAINCAL_EXT_BYPASS                 (1 << 4)
#define CODEC_CODEC_IIR4_GAINUSE_EXT_BYPASS                 (1 << 5)
#define CODEC_CODEC_IIR4_LMT_BYPASS                         (1 << 6)
#define CODEC_CODEC_IIR4_COUNT(n)                           (((n) & 0x1F) << 7)
#define CODEC_CODEC_IIR4_COUNT_MASK                         (0x1F << 7)
#define CODEC_CODEC_IIR4_COUNT_SHIFT                        (7)
#define CODEC_CODEC_IIR4_COEF_SWAP                          (1 << 12)
#define CODEC_CODEC_IIR4_AUTO_STOP                          (1 << 13)
#define CODEC_CODEC_IIR4_GC_DBG_SEL                         (1 << 14)
#define CODEC_CODEC_IIR4_COEF_SWAP_STATUS_SYNC_1            (1 << 15)
#define CODEC_CODEC_IIR4_IIRA_STOP_STATUS_SYNC_1            (1 << 16)
#define CODEC_CODEC_IIR4_IIRB_STOP_STATUS_SYNC_1            (1 << 17)

// reg_684
#define CODEC_CODEC_IIR5_ENABLE                             (1 << 0)
#define CODEC_CODEC_IIR5_IIRA_ENABLE                        (1 << 1)
#define CODEC_CODEC_IIR5_IIRB_ENABLE                        (1 << 2)
#define CODEC_CODEC_IIR5_BYPASS                             (1 << 3)
#define CODEC_CODEC_IIR5_GAINCAL_EXT_BYPASS                 (1 << 4)
#define CODEC_CODEC_IIR5_GAINUSE_EXT_BYPASS                 (1 << 5)
#define CODEC_CODEC_IIR5_LMT_BYPASS                         (1 << 6)
#define CODEC_CODEC_IIR5_COUNT(n)                           (((n) & 0x1F) << 7)
#define CODEC_CODEC_IIR5_COUNT_MASK                         (0x1F << 7)
#define CODEC_CODEC_IIR5_COUNT_SHIFT                        (7)
#define CODEC_CODEC_IIR5_COEF_SWAP                          (1 << 12)
#define CODEC_CODEC_IIR5_AUTO_STOP                          (1 << 13)
#define CODEC_CODEC_IIR5_GC_DBG_SEL                         (1 << 14)
#define CODEC_CODEC_IIR5_COEF_SWAP_STATUS_SYNC_1            (1 << 15)
#define CODEC_CODEC_IIR5_IIRA_STOP_STATUS_SYNC_1            (1 << 16)
#define CODEC_CODEC_IIR5_IIRB_STOP_STATUS_SYNC_1            (1 << 17)

// reg_690
#define CODEC_CODEC_IIR4_GAIN_EXT_TH(n)                     (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR4_GAIN_EXT_TH_MASK                   (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR4_GAIN_EXT_TH_SHIFT                  (0)

// reg_694
#define CODEC_CODEC_IIR5_GAIN_EXT_TH(n)                     (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR5_GAIN_EXT_TH_MASK                   (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR5_GAIN_EXT_TH_SHIFT                  (0)

// reg_774
#define CODEC_BT_TRIGGER                                    (1 << 0)
#define CODEC_BT_TRIGGER1                                   (1 << 1)
#define CODEC_BT_TRIGGER2                                   (1 << 2)
#define CODEC_BT_TRIGGER3                                   (1 << 3)

// reg_778
#define CODEC_BT_TRIGGER_MSK                                (1 << 0)
#define CODEC_BT_TRIGGER1_MSK                               (1 << 1)
#define CODEC_BT_TRIGGER2_MSK                               (1 << 2)
#define CODEC_BT_TRIGGER3_MSK                               (1 << 3)

// reg_77c
#define CODEC_FIR_COMB_SLV_EN_CH0                           (1 << 0)
#define CODEC_FIR_COMB_SLV_EN_CH1                           (1 << 1)
#define CODEC_FIR_COMB_SLV_EN_CH2                           (1 << 2)
#define CODEC_FIR_COMB_SLV_EN_CH3                           (1 << 3)
#define CODEC_FIR_COMB_MODE_CH0                             (1 << 4)
#define CODEC_FIR_COMB_MODE_CH1                             (1 << 5)
#define CODEC_FIR_COMB_MODE_CH2                             (1 << 6)
#define CODEC_FIR_COMB_MODE_CH3                             (1 << 7)
#define CODEC_FIR_SAMPLE_STEP_CH0(n)                        (((n) & 0x3F) << 8)
#define CODEC_FIR_SAMPLE_STEP_CH0_MASK                      (0x3F << 8)
#define CODEC_FIR_SAMPLE_STEP_CH0_SHIFT                     (8)
#define CODEC_FIR_SAMPLE_STEP_CH1(n)                        (((n) & 0x3F) << 14)
#define CODEC_FIR_SAMPLE_STEP_CH1_MASK                      (0x3F << 14)
#define CODEC_FIR_SAMPLE_STEP_CH1_SHIFT                     (14)
#define CODEC_FIR_SAMPLE_STEP_CH2(n)                        (((n) & 0x3F) << 20)
#define CODEC_FIR_SAMPLE_STEP_CH2_MASK                      (0x3F << 20)
#define CODEC_FIR_SAMPLE_STEP_CH2_SHIFT                     (20)
#define CODEC_FIR_SAMPLE_STEP_CH3(n)                        (((n) & 0x3F) << 26)
#define CODEC_FIR_SAMPLE_STEP_CH3_MASK                      (0x3F << 26)
#define CODEC_FIR_SAMPLE_STEP_CH3_SHIFT                     (26)

// reg_780
#define CODEC_SIN30K_ENABLE                                 (1 << 0)
#define CODEC_SIN30K_STEP(n)                                (((n) & 0x3) << 1)
#define CODEC_SIN30K_STEP_MASK                              (0x3 << 1)
#define CODEC_SIN30K_STEP_SHIFT                             (1)
#define CODEC_SIN30K_MODE                                   (1 << 3)
#define CODEC_SIN30K_GAIN(n)                                (((n) & 0x3FFF) << 4)
#define CODEC_SIN30K_GAIN_MASK                              (0x3FFF << 4)
#define CODEC_SIN30K_GAIN_SHIFT                             (4)

// reg_784
#define CODEC_CODEC_ADC_DITHER_DC_UPDATE_CH0                (1 << 0)
#define CODEC_CODEC_ADC_DITHER_DC_UPDATE_CH1                (1 << 1)
#define CODEC_CODEC_ADC_DITHER_DC_UPDATE_CH2                (1 << 2)
#define CODEC_CODEC_ADC_DITHER_DC_DIN_CH0(n)                (((n) & 0x1FF) << 3)
#define CODEC_CODEC_ADC_DITHER_DC_DIN_CH0_MASK              (0x1FF << 3)
#define CODEC_CODEC_ADC_DITHER_DC_DIN_CH0_SHIFT             (3)
#define CODEC_CODEC_ADC_DITHER_DC_DIN_CH1(n)                (((n) & 0x1FF) << 12)
#define CODEC_CODEC_ADC_DITHER_DC_DIN_CH1_MASK              (0x1FF << 12)
#define CODEC_CODEC_ADC_DITHER_DC_DIN_CH1_SHIFT             (12)
#define CODEC_CODEC_ADC_DITHER_DC_DIN_CH2(n)                (((n) & 0x1FF) << 21)
#define CODEC_CODEC_ADC_DITHER_DC_DIN_CH2_MASK              (0x1FF << 21)
#define CODEC_CODEC_ADC_DITHER_DC_DIN_CH2_SHIFT             (21)

// reg_788
#define CODEC_CODEC_ADC_CH0_DITHER_BYPASS_MODE              (1 << 0)
#define CODEC_CODEC_ADC_CH1_DITHER_BYPASS_MODE              (1 << 1)
#define CODEC_CODEC_ADC_CH2_DITHER_BYPASS_MODE              (1 << 2)
#define CODEC_CODEC_ADCA_SAT_DET_GAIN_PATCH_EN              (1 << 3)
#define CODEC_CODEC_ADCB_SAT_DET_GAIN_PATCH_EN              (1 << 4)
#define CODEC_CODEC_ADCC_SAT_DET_GAIN_PATCH_EN              (1 << 5)
#define CODEC_ADCA_DEBUG_SAT_DET_STATE_SYNC(n)              (((n) & 0x7) << 6)
#define CODEC_ADCA_DEBUG_SAT_DET_STATE_SYNC_MASK            (0x7 << 6)
#define CODEC_ADCA_DEBUG_SAT_DET_STATE_SYNC_SHIFT           (6)
#define CODEC_ADCB_DEBUG_SAT_DET_STATE_SYNC(n)              (((n) & 0x7) << 9)
#define CODEC_ADCB_DEBUG_SAT_DET_STATE_SYNC_MASK            (0x7 << 9)
#define CODEC_ADCB_DEBUG_SAT_DET_STATE_SYNC_SHIFT           (9)
#define CODEC_ADCC_DEBUG_SAT_DET_STATE_SYNC(n)              (((n) & 0x7) << 12)
#define CODEC_ADCC_DEBUG_SAT_DET_STATE_SYNC_MASK            (0x7 << 12)
#define CODEC_ADCC_DEBUG_SAT_DET_STATE_SYNC_SHIFT           (12)

// reg_78c
#define CODEC_CODEC_ADCA_SAT_DET_EN                         (1 << 0)
#define CODEC_CODEC_ADCA_SAT_DET_POL                        (1 << 1)
#define CODEC_CODEC_ADCA_NML_ZERO_DET_BYPASS                (1 << 2)
#define CODEC_CODEC_ADCA_SNR_ZERO_DET_BYPASS                (1 << 3)
#define CODEC_CODEC_ADCA_WAIT0_TIMER_MSK                    (1 << 4)
#define CODEC_CODEC_ADCA_ANA_DIG_GAIN_DELAY(n)              (((n) & 0x1F) << 5)
#define CODEC_CODEC_ADCA_ANA_DIG_GAIN_DELAY_MASK            (0x1F << 5)
#define CODEC_CODEC_ADCA_ANA_DIG_GAIN_DELAY_SHIFT           (5)
#define CODEC_CODEC_ADCA_GAIN_DELAY_BYPASS                  (1 << 10)
#define CODEC_CODEC_ADCA_ANA_NML_GAIN(n)                    (((n) & 0x7) << 11)
#define CODEC_CODEC_ADCA_ANA_NML_GAIN_MASK                  (0x7 << 11)
#define CODEC_CODEC_ADCA_ANA_NML_GAIN_SHIFT                 (11)
#define CODEC_CODEC_ADCA_ANA_SAT_GAIN(n)                    (((n) & 0x7) << 14)
#define CODEC_CODEC_ADCA_ANA_SAT_GAIN_MASK                  (0x7 << 14)
#define CODEC_CODEC_ADCA_ANA_SAT_GAIN_SHIFT                 (14)
#define CODEC_CODEC_ADCA_SAT_DET_DC(n)                      (((n) & 0x1FF) << 17)
#define CODEC_CODEC_ADCA_SAT_DET_DC_MASK                    (0x1FF << 17)
#define CODEC_CODEC_ADCA_SAT_DET_DC_SHIFT                   (17)

// reg_790
#define CODEC_CODEC_ADCA_WAIT0_MAXTIME(n)                   (((n) & 0x3FF) << 0)
#define CODEC_CODEC_ADCA_WAIT0_MAXTIME_MASK                 (0x3FF << 0)
#define CODEC_CODEC_ADCA_WAIT0_MAXTIME_SHIFT                (0)
#define CODEC_CODEC_ADCA_SAT_EXIT_WINDOW(n)                 (((n) & 0x1FFF) << 10)
#define CODEC_CODEC_ADCA_SAT_EXIT_WINDOW_MASK               (0x1FFF << 10)
#define CODEC_CODEC_ADCA_SAT_EXIT_WINDOW_SHIFT              (10)
#define CODEC_CODEC_ADC_SAT_DET_GAIN_DLY(n)                 (((n) & 0xFF) << 23)
#define CODEC_CODEC_ADC_SAT_DET_GAIN_DLY_MASK               (0xFF << 23)
#define CODEC_CODEC_ADC_SAT_DET_GAIN_DLY_SHIFT              (23)
#define CODEC_CODEC_ADC_SAT_DET_GAIN_DLY_MODE               (1 << 31)

// reg_794
#define CODEC_CODEC_ADCB_SAT_DET_EN                         (1 << 0)
#define CODEC_CODEC_ADCB_SAT_DET_POL                        (1 << 1)
#define CODEC_CODEC_ADCB_NML_ZERO_DET_BYPASS                (1 << 2)
#define CODEC_CODEC_ADCB_SNR_ZERO_DET_BYPASS                (1 << 3)
#define CODEC_CODEC_ADCB_WAIT0_TIMER_MSK                    (1 << 4)
#define CODEC_CODEC_ADCB_ANA_DIG_GAIN_DELAY(n)              (((n) & 0x1F) << 5)
#define CODEC_CODEC_ADCB_ANA_DIG_GAIN_DELAY_MASK            (0x1F << 5)
#define CODEC_CODEC_ADCB_ANA_DIG_GAIN_DELAY_SHIFT           (5)
#define CODEC_CODEC_ADCB_GAIN_DELAY_BYPASS                  (1 << 10)
#define CODEC_CODEC_ADCB_ANA_NML_GAIN(n)                    (((n) & 0x7) << 11)
#define CODEC_CODEC_ADCB_ANA_NML_GAIN_MASK                  (0x7 << 11)
#define CODEC_CODEC_ADCB_ANA_NML_GAIN_SHIFT                 (11)
#define CODEC_CODEC_ADCB_ANA_SAT_GAIN(n)                    (((n) & 0x7) << 14)
#define CODEC_CODEC_ADCB_ANA_SAT_GAIN_MASK                  (0x7 << 14)
#define CODEC_CODEC_ADCB_ANA_SAT_GAIN_SHIFT                 (14)
#define CODEC_CODEC_ADCB_SAT_DET_DC(n)                      (((n) & 0x1FF) << 17)
#define CODEC_CODEC_ADCB_SAT_DET_DC_MASK                    (0x1FF << 17)
#define CODEC_CODEC_ADCB_SAT_DET_DC_SHIFT                   (17)

// reg_798
#define CODEC_CODEC_ADCB_WAIT0_MAXTIME(n)                   (((n) & 0x3FF) << 0)
#define CODEC_CODEC_ADCB_WAIT0_MAXTIME_MASK                 (0x3FF << 0)
#define CODEC_CODEC_ADCB_WAIT0_MAXTIME_SHIFT                (0)
#define CODEC_CODEC_ADCB_SAT_EXIT_WINDOW(n)                 (((n) & 0x1FFF) << 10)
#define CODEC_CODEC_ADCB_SAT_EXIT_WINDOW_MASK               (0x1FFF << 10)
#define CODEC_CODEC_ADCB_SAT_EXIT_WINDOW_SHIFT              (10)

// reg_79c
#define CODEC_CODEC_ADCC_SAT_DET_EN                         (1 << 0)
#define CODEC_CODEC_ADCC_SAT_DET_POL                        (1 << 1)
#define CODEC_CODEC_ADCC_NML_ZERO_DET_BYPASS                (1 << 2)
#define CODEC_CODEC_ADCC_SNR_ZERO_DET_BYPASS                (1 << 3)
#define CODEC_CODEC_ADCC_WAIT0_TIMER_MSK                    (1 << 4)
#define CODEC_CODEC_ADCC_ANA_DIG_GAIN_DELAY(n)              (((n) & 0x1F) << 5)
#define CODEC_CODEC_ADCC_ANA_DIG_GAIN_DELAY_MASK            (0x1F << 5)
#define CODEC_CODEC_ADCC_ANA_DIG_GAIN_DELAY_SHIFT           (5)
#define CODEC_CODEC_ADCC_GAIN_DELAY_BYPASS                  (1 << 10)
#define CODEC_CODEC_ADCC_ANA_NML_GAIN(n)                    (((n) & 0x7) << 11)
#define CODEC_CODEC_ADCC_ANA_NML_GAIN_MASK                  (0x7 << 11)
#define CODEC_CODEC_ADCC_ANA_NML_GAIN_SHIFT                 (11)
#define CODEC_CODEC_ADCC_ANA_SAT_GAIN(n)                    (((n) & 0x7) << 14)
#define CODEC_CODEC_ADCC_ANA_SAT_GAIN_MASK                  (0x7 << 14)
#define CODEC_CODEC_ADCC_ANA_SAT_GAIN_SHIFT                 (14)
#define CODEC_CODEC_ADCC_SAT_DET_DC(n)                      (((n) & 0x1FF) << 17)
#define CODEC_CODEC_ADCC_SAT_DET_DC_MASK                    (0x1FF << 17)
#define CODEC_CODEC_ADCC_SAT_DET_DC_SHIFT                   (17)

// reg_7a0
#define CODEC_CODEC_ADCC_WAIT0_MAXTIME(n)                   (((n) & 0x3FF) << 0)
#define CODEC_CODEC_ADCC_WAIT0_MAXTIME_MASK                 (0x3FF << 0)
#define CODEC_CODEC_ADCC_WAIT0_MAXTIME_SHIFT                (0)
#define CODEC_CODEC_ADCC_SAT_EXIT_WINDOW(n)                 (((n) & 0x1FFF) << 10)
#define CODEC_CODEC_ADCC_SAT_EXIT_WINDOW_MASK               (0x1FFF << 10)
#define CODEC_CODEC_ADCC_SAT_EXIT_WINDOW_SHIFT              (10)

// reg_7a4
#define CODEC_CODEC_ADCA_CFG_GAIN(n)                        (((n) & 0x3FF) << 0)
#define CODEC_CODEC_ADCA_CFG_GAIN_MASK                      (0x3FF << 0)
#define CODEC_CODEC_ADCA_CFG_GAIN_SHIFT                     (0)
#define CODEC_CODEC_ADCB_CFG_GAIN(n)                        (((n) & 0x3FF) << 10)
#define CODEC_CODEC_ADCB_CFG_GAIN_MASK                      (0x3FF << 10)
#define CODEC_CODEC_ADCB_CFG_GAIN_SHIFT                     (10)
#define CODEC_CODEC_ADCC_CFG_GAIN(n)                        (((n) & 0x3FF) << 20)
#define CODEC_CODEC_ADCC_CFG_GAIN_MASK                      (0x3FF << 20)
#define CODEC_CODEC_ADCC_CFG_GAIN_SHIFT                     (20)

#endif
