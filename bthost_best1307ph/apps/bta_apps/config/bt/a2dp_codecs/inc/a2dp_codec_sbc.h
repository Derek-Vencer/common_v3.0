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
#ifndef __A2DP_CODEC_SBC_H__
#define __A2DP_CODEC_SBC_H__

#if defined(__cplusplus)
extern "C" {
#endif


#define A2D_SBC_IE_SAMP_FREQ_MSK    0xF0    /* b7-b4 sampling frequency */

// bit31-8: Reserved for Future Use
#define A2D_SBC_IE_SAMP_FREQ_24         (0x00000100)    /* b8:24  kHz */
#define A2D_SBC_IE_SAMP_FREQ_16         (0x00000080)    /* b7:16  kHz */
#define A2D_SBC_IE_SAMP_FREQ_32         (0x00000040)    /* b6:32  kHz */
#define A2D_SBC_IE_SAMP_FREQ_44         (0x00000020)    /* b5:44.1kHz */
#define A2D_SBC_IE_SAMP_FREQ_48         (0x00000010)    /* b4:48  kHz */
#define A2D_SBC_IE_SAMP_FREQ_96         (0x00000008)    /* b3:96  kHz */
#define A2D_SBC_IE_SAMP_FREQ_192        (0x00000004)    /* b2:192  kHz */
#define A2D_SBC_IE_SAMP_FREQ_88D2       (0x00000002)    /* b1:88.2  kHz */
#define A2D_SBC_IE_SAMP_FREQ_176D4      (0x00000001)    /* b0:176.4 kHz */


#define A2D_SBC_IE_BIT_NUM_16       0x16
#define A2D_SBC_IE_BIT_NUM_24       0x24

#define A2D_SBC_IE_CH_MD_MSK        0x0F    /* b3-b0 channel mode */
#define A2D_SBC_IE_CH_MD_MONO       0x08    /* b3: mono */
#define A2D_SBC_IE_CH_MD_DUAL       0x04    /* b2: dual */
#define A2D_SBC_IE_CH_MD_STEREO     0x02    /* b1: stereo */
#define A2D_SBC_IE_CH_MD_JOINT      0x01    /* b0: joint stereo */

#define A2D_SBC_IE_BLOCKS_MSK       0xF0    /* b7-b4 number of blocks */
#define A2D_SBC_IE_BLOCKS_4         0x80    /* 4 blocks */
#define A2D_SBC_IE_BLOCKS_8         0x40    /* 8 blocks */
#define A2D_SBC_IE_BLOCKS_12        0x20    /* 12blocks */
#define A2D_SBC_IE_BLOCKS_16        0x10    /* 16blocks */

#define A2D_SBC_IE_SUBBAND_MSK      0x0C    /* b3-b2 number of subbands */
#define A2D_SBC_IE_SUBBAND_4        0x08    /* b3: 4 */
#define A2D_SBC_IE_SUBBAND_8        0x04    /* b2: 8 */

#define A2D_SBC_IE_ALLOC_MD_MSK     0x03    /* b1-b0 allocation mode */
#define A2D_SBC_IE_ALLOC_MD_S       0x02    /* b1: SNR */
#define A2D_SBC_IE_ALLOC_MD_L       0x01    /* b0: loundess */

#define A2D_SBC_IE_MIN_BITPOOL      2
#define A2D_SBC_IE_MAX_BITPOOL      250

extern const unsigned char a2dp_codec_elements[];

void a2dp_codec_sbc_init(void);

#if defined(CUSTOM_BITRATE) || defined(BTIF_DIP_DEVICE)
uint8_t a2dp_avdtpcodec_sbc_user_bitpool_get();
void a2dp_avdtpcodec_sbc_user_configure_set(uint32_t bitpool,uint8_t user_configure);
#endif

#if defined(__cplusplus)
}
#endif

#endif /* __A2DP_CODEC_SBC_H__ */