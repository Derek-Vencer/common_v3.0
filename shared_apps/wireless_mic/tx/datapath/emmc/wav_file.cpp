
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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "wav_file.h"


struct BWF wav_init(struct BWF wav_head, int channels, int sample_rate, int bits)
{
    memcpy(wav_head.ChunkID, "RIFF", strlen("RIFF"));
    wav_head.ChunkSize = 0;
    memcpy(wav_head.Format, "WAVE", strlen("WAVE"));

    /* WAVE_FMT */
    memcpy(wav_head.Subchunk1ID, "fmt ", strlen("fmt "));
    wav_head.Subchunk1Size = 16;
    wav_head.AudioFormat = 1;
    wav_head.NumChannels = channels;
    wav_head.SampleRate = sample_rate;
    wav_head.BitsPerSample = bits;
    /* ==SampleRate*NumChannels*BitsPerSample/8 */
    wav_head.ByteRate = wav_head.SampleRate*wav_head.NumChannels*wav_head.BitsPerSample/8;
    //** /* ==NumChannels*BitsPerSample/8 */
    wav_head.BlockAlign = wav_head.NumChannels*wav_head.BitsPerSample/8;

    // /* WAVE_BEXT */
    // memcpy(wav_head.bext.ChunkID, "bext", strlen("bext"));
    // wav_head.bext.ChunkSize = sizeof(struct BEXT) - 8;
    // memcpy(wav_head.bext.Description, "emmc dump data", strlen("emmc dump data"));
    // memcpy(wav_head.bext.Originator, "bestechnic", strlen("bestechnic"));

    // memcpy(wav_head.bext.OriginationDate, "2024-11-27", strlen("2024-11-27"));
    // memcpy(wav_head.bext.OriginationTime, "00:00:00", strlen("00:00:00"));
    // wav_head.bext.Version = 1;

    // /* WAVE_SPEED */
    // memcpy(wav_head.speed.ChunkID, "spd", strlen("spd"));
    // wav_head.speed.ChunkSize = sizeof(struct SPEED) - 8;

    // wav_head.speed.Numerator = 1;
    // wav_head.speed.Denominator = 1;
    // wav_head.speed.Reserved = 1;

    /* WAVE_DATA */
    memcpy(wav_head.Subchunk2ID, "data", strlen("data"));
    wav_head.Subchunk2Size = 0;

    return wav_head;
}


struct BWF wav_close(struct BWF wav_head ,int data_size)
{
    wav_head.ChunkSize = 36 + data_size;
    wav_head.Subchunk2Size = data_size;

    return wav_head;
}
