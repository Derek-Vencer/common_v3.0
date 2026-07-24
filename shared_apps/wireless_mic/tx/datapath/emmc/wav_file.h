
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
#ifndef _WAV_FILE_H_
#define _WAV_FILE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

/*
BEXT（Broadcast Extension）字段
BEXT字段包含了广播相关的元数据，如描述、编码日期、编码时间、编码器、编码器版本等。BEXT字段的结构如下：

ChunkID: 4字节的块ID，通常为"bext"
ChunkSize: 4字节的块大小
Description: 256字节的描述字符串
Originator: 32字节的创建者字符串
OriginatorReference: 32字节的创建者参考字符串
OriginationDate: 10字节的创建日期字符串（格式：YYYY-MM-DD）
OriginationTime: 8字节的创建时间字符串（格式：HH:MM:SS）
TimeReference: 8字节的时间参考（样本数）
Version: 2字节的版本号
UMID: 64字节的UMID（Unique Material Identifier）
Reserved: 190字节的保留字段
CodingHistory: 可变长度的编码历史字符串
*/
// struct BEXT{
//     char ChunkID[4]; // "bext"
//     uint32_t ChunkSize; // bext子块的大小
//     char Description[256];
//     char Originator[32];
//     char OriginatorReference[32];
//     char OriginationDate[10];
//     char OriginationTime[8];
//     uint64_t TimeReference;
//     uint16_t Version;
//     char UMID[64];
//     char Reserved[190];
//     char CodingHistory[10];
// };

/*
SPEED（Speed Chunk）字段
SPEED字段用于描述音频文件的播放速度信息。SPEED字段的结构如下：

ChunkID: 4字节的块ID，通常为"SPEED"
ChunkSize: 4字节的块大小
Numerator: 4字节的分子
Denominator: 4字节的分母
Reserved: 4字节的保留字段
*/
// struct SPEED{
//     char ChunkID[4]; // "SPEED"
//     uint32_t ChunkSize;
//     uint32_t Numerator;
//     uint32_t Denominator;
//     uint32_t Reserved;
// };

struct BWF{
    // WAV file header
    char ChunkID[4]; // "RIFF"
    uint32_t ChunkSize;
    char Format[4]; // "WAVE" // fmt sub-chunk

    char Subchunk1ID[4]; // "fmt "
    uint32_t Subchunk1Size;
    uint16_t AudioFormat;
    uint16_t NumChannels;
    uint32_t SampleRate;
    uint32_t ByteRate;
    uint16_t BlockAlign;
    uint16_t BitsPerSample; // data sub-chunk

    // struct BEXT bext; // SPEED chunk
    // struct SPEED speed;

    char Subchunk2ID[4]; // "data"
    uint32_t Subchunk2Size;
};

struct BWF wav_init(struct BWF wav_head, int channels, int sample_rate, int bits);

struct BWF wav_close(struct BWF wav_head ,int data_size);

#ifdef __cplusplus
}
#endif

#endif