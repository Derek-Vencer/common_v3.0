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
#ifdef GAF_OFFLOAD_ENABLE
#include "lc3_process.h"
#include "heap_api.h"
#include "hal_trace.h"
#include "gaf_offload_dsp_stream.h"
#include "lc3_process.h"
#include "gaf_offload_lc3.h"

#ifdef LC3_IN_ROM_V2
lc3_buf_param offload_lc3_buf_param;
#endif

static LC3_Enc_Info lc3_enc_info_ins[BLE_CONNECTION_MAX];
static LC3_Dec_Info lc3_dec_info_ins[BLE_CONNECTION_MAX];

#define OFFLOAD_STREAM_HEAP_DEBUG            1

heap_handle_t lc3_heap_handle_iso = NULL;
void* bes_heap_init_lc3(uint8_t* app_registered_heap, uint32_t heap_size)
{
    lc3_heap_handle_iso = (heap_handle_t)heap_register(app_registered_heap, heap_size);
    ASSERT(lc3_heap_handle_iso != NULL, "%s: FAILED", __FUNCTION__);
    return (void*)lc3_heap_handle_iso;
}

void* bes_heap_malloc_lc3(uint32_t length)
{
    void* ret_ptr = NULL;
    if (lc3_heap_handle_iso && length)
    {
        ret_ptr = heap_malloc((heap_handle_t)lc3_heap_handle_iso, length);
    }
#if OFFLOAD_STREAM_HEAP_DEBUG
    size_t heap_free_sz = heap_free_size(lc3_heap_handle_iso);
    size_t heap_min_free_sz = heap_minimum_free_size(lc3_heap_handle_iso);
    LOG_I("[%s] size=%u user=%p left/mini left=%u/%u %p",
          __func__, length, __builtin_return_address(0),
          heap_free_sz,
          heap_min_free_sz ,lc3_heap_handle_iso);
#endif
    return ret_ptr;
}

void bes_heap_free_lc3(void* ptr)
{
    if (lc3_heap_handle_iso)
    {
        heap_free((heap_handle_t)lc3_heap_handle_iso, ptr);
    }
}

static void *lc3_alloc(void *pool, unsigned size)
{
#ifdef LC3_IN_ROM_V2
    if(size == 30720 || size == 38912 || size == 34816)
    {
        if(offload_lc3_buf_param.is_encoded)
        {
            if(offload_lc3_buf_param.channels == 1)
            {
                if(offload_lc3_buf_param.sample_rate == 48000)
                {
                    switch(offload_lc3_buf_param.frame_dms)
                    {
                        case 100: size = 18 * 1024; break;
                        case 75:  size = 16 * 1024; break;
                        case 50:  size = 14 * 1024; break;
                        case 25:  size = 12 * 1024; break;
                        default: ASSERT(0, "lc3 frame_dms no support:%d", offload_lc3_buf_param.frame_dms);
                    }
                }else if(offload_lc3_buf_param.sample_rate == 32000)
                {
                    switch(offload_lc3_buf_param.frame_dms)
                    {
                        case 100: size = 16 * 1024; break;
                        case 75:  size = 14 * 1024; break;
                        case 50:  size = 13 * 1024; break;
                        case 25:  size = 11 * 1024; break;
                        default: ASSERT(0, "lc3 frame_dms no support:%d", offload_lc3_buf_param.frame_dms);
                    }
                }else if(offload_lc3_buf_param.sample_rate == 16000)
                {
                    switch(offload_lc3_buf_param.frame_dms)
                    {
                        case 100: size = 12 * 1024; break;
                        case 75:  size = 12 * 1024; break;
                        case 50:  size = 11 * 1024; break;
                        case 25:  size = 10 * 1024; break;
                        default: ASSERT(0, "lc3 frame_dms no support:%d", offload_lc3_buf_param.frame_dms);
                    }
                }
            }else if(offload_lc3_buf_param.channels == 2){
                if(offload_lc3_buf_param.sample_rate == 48000)
                {
                    switch(offload_lc3_buf_param.frame_dms)
                    {
                        case 100: size = 25 * 1024; break;
                        case 75:  size = 23 * 1024; break;
                        case 50:  size = 20 * 1024; break;
                        case 25:  size = 17 * 1024; break;
                        default: ASSERT(0, "lc3 frame_dms no support:%d", offload_lc3_buf_param.frame_dms);
                    }
                }else if(offload_lc3_buf_param.sample_rate == 32000)
                {
                    switch(offload_lc3_buf_param.frame_dms)
                    {
                        case 100: size = 22 * 1024; break;
                        case 75:  size = 20 * 1024; break;
                        case 50:  size = 18 * 1024; break;
                        case 25:  size = 16 * 1024; break;
                        default: ASSERT(0, "lc3 frame_dms no support:%d", offload_lc3_buf_param.frame_dms);
                    }
                }else if(offload_lc3_buf_param.sample_rate == 16000)
                {
                    switch(offload_lc3_buf_param.frame_dms)
                    {
                        case 100: size = 18 * 1024; break;
                        case 75:  size = 17 * 1024; break;
                        case 50:  size = 16 * 1024; break;
                        case 25:  size = 15 * 1024; break;
                        default: ASSERT(0, "lc3 frame_dms no support:%d", offload_lc3_buf_param.frame_dms);
                    }
                }
            }
        }else{
            if(offload_lc3_buf_param.channels == 1)
            {
                if(offload_lc3_buf_param.sample_rate == 48000)
                {
                    switch(offload_lc3_buf_param.frame_dms)
                    {
                        case 100: size = 19 * 1024; break;
                        case 75:  size = 17 * 1024; break;
                        case 50:  size = 13 * 1024; break;
                        case 25:  size = 11 * 1024; break;
                        default: ASSERT(0, "lc3 frame_dms no support:%d", offload_lc3_buf_param.frame_dms);
                    }
                }else if(offload_lc3_buf_param.sample_rate == 32000)
                {
                    switch(offload_lc3_buf_param.frame_dms)
                    {
                        case 100: size = 14 * 1024; break;
                        case 75:  size = 12 * 1024; break;
                        case 50:  size = 10 * 1024; break;
                        case 25:  size = 8 * 1024; break;
                        default: ASSERT(0, "lc3 frame_dms no support:%d", offload_lc3_buf_param.frame_dms);
                    }
                }else if(offload_lc3_buf_param.sample_rate == 16000)
                {
                    switch(offload_lc3_buf_param.frame_dms)
                    {
                        case 100: size = 8 * 1024; break;
                        case 75:  size = 7 * 1024; break;
                        case 50:  size = 6 * 1024; break;
                        case 25:  size = 5 * 1024; break;
                        default: ASSERT(0, "lc3 frame_dms no support:%d", offload_lc3_buf_param.frame_dms);
                    }
                }
            }else{
                if(offload_lc3_buf_param.sample_rate == 48000)
                {
                    switch(offload_lc3_buf_param.frame_dms)
                    {
                        case 100: size = 29 * 1024; break;
                        case 75:  size = 26 * 1024; break;
                        case 50:  size = 21 * 1024; break;
                        case 25:  size = 18 * 1024; break;
                        default: ASSERT(0, "lc3 frame_dms no support:%d", offload_lc3_buf_param.frame_dms);
                    }
                }else if(offload_lc3_buf_param.sample_rate == 32000)
                {
                    switch(offload_lc3_buf_param.frame_dms)
                    {
                        case 100: size = 21 * 1024; break;
                        case 75:  size = 19 * 1024; break;
                        case 50:  size = 16 * 1024; break;
                        case 25:  size = 14 * 1024; break;
                        default: ASSERT(0, "lc3 frame_dms no support:%d", offload_lc3_buf_param.frame_dms);
                    }
                }else if(offload_lc3_buf_param.sample_rate == 16000)
                {
                    switch(offload_lc3_buf_param.frame_dms)
                    {
                        case 100: size = 13 * 1024; break;
                        case 75:  size = 12 * 1024; break;
                        case 50:  size = 10 * 1024; break;
                        case 25:  size = 9 * 1024; break;
                        default: ASSERT(0, "lc3 frame_dms no support:%d", offload_lc3_buf_param.frame_dms);
                    }
                }
            }
        }
    }
#endif
    return bes_heap_malloc_lc3(size);
}

static void lc3_free(void *pool, void *data)
{
    bes_heap_free_lc3(data);
    return;
}

void gaf_audio_lc3_direct_decoder_buf_init(uint8_t instance_handle, void *codec_info, CODEC_ALLOC alloc_cb, CODEC_FREE free_cb)
{
    LOG_D("%s", __func__);

    LC3_Dec_Info* info = &lc3_dec_info_ins[instance_handle];
    memset(info, 0,sizeof(LC3_Dec_Info));
    info->cb_alloc = lc3_alloc;
    info->cb_free = lc3_free;
    info->cb_overlay = 0;
}

void gaf_audio_lc3_direct_encoder_buf_init(uint8_t instance_handle, void *codec_info, CODEC_ALLOC alloc_cb, CODEC_FREE free_cb)
{
   TRACE(1, "%s", __func__);
/*
*   Capture should always be less than 1
*/
    LC3_Enc_Info* info = &lc3_enc_info_ins[instance_handle];
    memset(info, 0,sizeof(LC3_Enc_Info));
    info->cb_alloc = lc3_alloc;
    info->cb_free = lc3_free;
    info->cb_overlay = 0;
}

static void aob_stream_lc3_decoder_init(void *_p_lc3_dec_info)
{
    //app_overlay_select(APP_OVERLAY_LC3);//load overlay by callback
    LC3_Dec_Info *p_lc3_dec_info = (LC3_Dec_Info *)_p_lc3_dec_info;
    uint32_t decoder_size = 0;
    uint32_t scratch_size = 0;
    int err = LC3_API_OK;

    LOG_I("%s", __FUNCTION__);

    //decoder_size = lc3_api_dec_get_size(p_lc3_dec_info->sample_rate,
    //    p_lc3_dec_info->channels, p_lc3_dec_info->plcMeth);
    //memset((void*)p_lc3_dec_info->decode, 0, decoder_size);
    err = lc3_api_decoder_init(p_lc3_dec_info);
    
    //scratch_size = lc3_api_dec_get_scratch_size(p_lc3_dec_info->decode);
    //memset((void*)p_lc3_dec_info->scratch, 0, scratch_size);

    ASSERT(LC3_API_OK == err, "%s err %d", __func__, err);

    /* Print info */
    LOG_I("[lc3]");
    LOG_I("Decoder size:     %d", decoder_size);
    LOG_I("Scratch size:     %d", scratch_size);
    LOG_I("Bits:             %d", p_lc3_dec_info->bitwidth);
    LOG_I("Bits align:       %d", p_lc3_dec_info->bitalign);
    LOG_I("Sample rate:      %d", p_lc3_dec_info->sample_rate);
    LOG_I("Channels:         %d", p_lc3_dec_info->channels);
    LOG_I("Frame samples:    %d", p_lc3_dec_info->frame_samples);
    LOG_I("Frame length:     %d", p_lc3_dec_info->frame_size);
    LOG_I("PLC mode:         %d", p_lc3_dec_info->plcMeth);
    LOG_I("Bitrate:          %d", p_lc3_dec_info->bitrate);
}

void gaf_audio_lc3_direct_decoder_init(uint8_t instance_handle, void * _codec_info)
{
    LOG_I("%s", __func__);
    CODEC_INFO_T *codec_info = (CODEC_INFO_T*)_codec_info;
    /* lc3 deocder init phase starting */
    LC3_Dec_Info *p_lc3_dec_info = (LC3_Dec_Info *)&lc3_dec_info_ins[instance_handle];

    p_lc3_dec_info->sample_rate = codec_info->sample_rate;
#ifdef BLE_AUDIO_STEREO_CHAN_OVER_CIS_CNT
    p_lc3_dec_info->channels   = codec_info->num_channels;
#else
    p_lc3_dec_info->channels   = AUD_CHANNEL_NUM_1;
#endif
    p_lc3_dec_info->bitwidth   = codec_info->bits_depth;
    p_lc3_dec_info->bitalign   = (p_lc3_dec_info->bitwidth == 24) ? 32 : 0;
    p_lc3_dec_info->frame_dms  = (uint8_t)(codec_info->frame_ms * 10);
    p_lc3_dec_info->frame_size = codec_info->frame_size * p_lc3_dec_info->channels;

    p_lc3_dec_info->bitrate    = 0;
    p_lc3_dec_info->plcMeth    = LC3_API_PLC_ADVANCED;
    p_lc3_dec_info->epmode     = LC3_API_EP_OFF;
    p_lc3_dec_info->is_interlaced     = 1;
#ifdef LC3S_ENABLE
    p_lc3_dec_info->is_lc3s       = 1;
#endif
#ifdef LC3_IN_ROM_V2
    offload_lc3_buf_param.is_encoded   = false;
    offload_lc3_buf_param.sample_rate  = p_lc3_dec_info->sample_rate;
    offload_lc3_buf_param.frame_dms    = p_lc3_dec_info->frame_dms;
    offload_lc3_buf_param.channels     = p_lc3_dec_info->channels;
#endif
    aob_stream_lc3_decoder_init(p_lc3_dec_info);

    LOG_I("%s end", __func__);
}

void aob_stream_lc3_encoder_init(void *_p_lc3_enc_info)
{
    LOG_I("%s", __func__);
    //app_overlay_select(APP_OVERLAY_LC3);//load overlay by callback
    LC3_Enc_Info *p_lc3_enc_info = (LC3_Enc_Info *)_p_lc3_enc_info;

    int err = LC3_API_OK;

    err = lc3_api_encoder_init(p_lc3_enc_info);
    ASSERT(LC3_API_OK == err, "%s/%d err %d", __func__,__LINE__, err);
    p_lc3_enc_info->enabled_frame_buff = lc3_alloc(0,p_lc3_enc_info->frame_size);
    ASSERT(p_lc3_enc_info->instance, "%s/%d", __func__, __LINE__);
    ASSERT(p_lc3_enc_info->enabled_frame_buff, "%s/%d", __func__, __LINE__);
    //ASSERT(p_lc3_enc_info->cb_encode, "%s/%d", __func__, __LINE__);
    ASSERT(p_lc3_enc_info->cb_encode_interlaced, "%s/%d", __func__, __LINE__);
    ASSERT(p_lc3_enc_info->frame_samples, "%s/%d", __func__, __LINE__);
    ASSERT(p_lc3_enc_info->frame_size, "%s/%d", __func__, __LINE__);

    /* Print info */
    LOG_I("[lc3]");
    LOG_I("Encoder size:     %d", p_lc3_enc_info->instance_size);
    LOG_I("Scratch size:     %d", p_lc3_enc_info->scratch_size);
    LOG_I("Sample rate:      %d", p_lc3_enc_info->sample_rate);
    LOG_I("Channels:         %d", p_lc3_enc_info->channels);
    LOG_I("Frame samples:    %d", p_lc3_enc_info->frame_samples);
    LOG_I("Frame length:     %d", p_lc3_enc_info->frame_size);
    LOG_I("Output format:    %d bits", p_lc3_enc_info->bitwidth);
    LOG_I("Output format:    %d bits align", p_lc3_enc_info->bitalign);
    LOG_I("Target bitrate:   %d", p_lc3_enc_info->bitrate);
    //LOG_I("Real bitrate:     %d", p_lc3_enc_info->real_bitrate);
    //LOG_I("PLC mode:         %d", p_lc3_enc_info->plcMeth);
    LOG_I("\n");
}

void gaf_audio_lc3_direct_encoder_init(uint8_t instance_handle, void *_codec_info)
{
    LOG_I("%s", __func__);
    CODEC_INFO_T *codec_info = (CODEC_INFO_T*)_codec_info;
    //Capture should always be less than 1

    LC3_Enc_Info *p_lc3_enc_info = &lc3_enc_info_ins[instance_handle];

    p_lc3_enc_info->sample_rate = codec_info->sample_rate;
    p_lc3_enc_info->channels   = codec_info->num_channels;
    p_lc3_enc_info->bitwidth   = codec_info->bits_depth;
    p_lc3_enc_info->bitalign   = (p_lc3_enc_info->bitwidth==24)?32:p_lc3_enc_info->bitwidth;
    p_lc3_enc_info->frame_dms   = (uint8_t)(codec_info->frame_ms * 10);
    p_lc3_enc_info->frame_size = codec_info->frame_size * p_lc3_enc_info->channels;

    p_lc3_enc_info->bitrate       = 0;
    p_lc3_enc_info->is_interlaced = 1;
    p_lc3_enc_info->epmode        = LC3_API_EP_OFF;
    p_lc3_enc_info->ltpf_flag     = 1;
    p_lc3_enc_info->quan_prec     = 48;
#ifdef LC3S_ENABLE
    p_lc3_enc_info->is_lc3s       = 1;
#endif
#ifdef LC3_IN_ROM_V2
    offload_lc3_buf_param.is_encoded   = true;
    offload_lc3_buf_param.sample_rate  = p_lc3_enc_info->sample_rate;
    offload_lc3_buf_param.frame_dms    = p_lc3_enc_info->frame_dms;
    offload_lc3_buf_param.channels     = p_lc3_enc_info->channels;
#endif
    aob_stream_lc3_encoder_init(p_lc3_enc_info);
}

int aob_stream_lc3_decode_bits(void *lc3_dec_info, void *input_bytes, int32_t num_bytes,
    void *output_samples, bool isPLC)
{
    int32_t bfi_ext = 0;
    int32_t err = LC3_API_OK;
    LC3_Dec_Info *p_lc3_dec_info = (LC3_Dec_Info *)lc3_dec_info;
    ASSERT(p_lc3_dec_info, "p_lc3_dec_info is empty in %s ", __func__);
    if (0 == num_bytes||isPLC)
    {
        TRACE(0, "PLC num_bytes %d", num_bytes);
        bfi_ext = 1;
    }

    if(p_lc3_dec_info->is_interlaced){
        err = p_lc3_dec_info->cb_decode_interlaced(p_lc3_dec_info, p_lc3_dec_info->scratch, input_bytes,
            num_bytes, output_samples, bfi_ext);
    }
    else{
        uint8_t* output = (uint8_t*)output_samples;
        uint32_t xlen = p_lc3_dec_info->frame_samples*p_lc3_dec_info->bitalign/8;
        void* pcms[4];
        pcms[0] = output;
        pcms[1] = output + xlen;
        pcms[2] = output + xlen*2;
        pcms[3] = output + xlen*3;
        err = p_lc3_dec_info->cb_decode(p_lc3_dec_info, p_lc3_dec_info->scratch, input_bytes,
            num_bytes, pcms , bfi_ext);
    }
    if (err)
    {
        LOG_E("lc3 dec err:%d,num_bytes = %d\n",err,num_bytes);
    }
    LOG_D("[dec]%d->%d\n",p_lc3_dec_info->frame_size,p_lc3_dec_info->frame_samples);
    return (int)err;
}
int gaf_audio_direct_lc3_decode(uint8_t instance_handle, void *codec_info, uint32_t inputDataLength, void *input, void *output, bool isPLC)
{
    LC3_Dec_Info *lc3_dec_info = (LC3_Dec_Info *)&lc3_dec_info_ins[instance_handle];
    int ret = LC3_API_ERROR;
    ret = aob_stream_lc3_decode_bits(lc3_dec_info, input,
        inputDataLength, output, isPLC);

    return ret;
}

int aob_stream_lc3_encode_bits(void *lc3_enc_info, void *input_samples,
    void *output_bytes, int32_t num_bytes)
{
    // LOG_D("[lc3][enc]\n");
    int err = LC3_API_OK;
    LC3_Enc_Info *p_lc3_enc_info = (LC3_Enc_Info *)lc3_enc_info;
    ASSERT(p_lc3_enc_info, "p_lc3_enc_info is empty in %s ", __func__);

    err = p_lc3_enc_info->cb_encode_interlaced(p_lc3_enc_info, p_lc3_enc_info->scratch,
        input_samples, output_bytes, num_bytes);

    if(err){
        LOG_E("lc3 enc err:%d\n",err);
    }
    // LOG_D("[lc3][enc]%d->%d\n",p_lc3_enc_info->frame_samples,p_lc3_enc_info->frame_size);
    return (int)err;
}

bool check_flag(uint8_t *input_test, size_t inputDataLength) {
    for (size_t i = 0; i <= inputDataLength - 4; i++) {
        if (input_test[i] == 0x89 && input_test[i + 1] == 0x89 &&
            input_test[i + 2] == 0x89)
        {
            TRACE(0, "the check_flag num %d", i);
            return true;
        }
    }
    return false;

}

int gaf_audio_direct_lc3_encode(uint8_t instance_handle, void *codec_info, uint32_t inputDataLength, void *input, uint16_t frame_size, uint8_t *output)
{
    LC3_Enc_Info *lc3_enc_info = &lc3_enc_info_ins[instance_handle];
    uint32_t lc3_encoded_frame_len = lc3_enc_info->frame_size;
#ifdef ULL_AUDIO_GPIO_TEST
        uint8_t * input_test = (uint8_t *)input;
        uint8_t flag = 0;
        if (check_flag(input_test, inputDataLength))
        {
            flag = 1;
        }
#endif

        aob_stream_lc3_encode_bits(lc3_enc_info, input, (void*)output, lc3_encoded_frame_len);
#ifdef ULL_AUDIO_GPIO_TEST
        uint8_t * output_bytes_test = (uint8_t *)output;
        if (flag)
        {
            hal_iomux_tportset(1);
            hal_iomux_tportclr(1);
            output_bytes_test[0] = 0x12;
            output_bytes_test[1] = 0x34;
            output_bytes_test[2] = 0x56;
            output_bytes_test[3] = 0x78;
        }
#endif

    return 0;
}

// static void gaf_audio_direct_lc3_decoder_deinit(void** dec_hd)
// {
//     LOG_I("%s,dec_hdl = %p", __func__,*dec_hd);
//     // void* hd = *dec_hd;
//     // *dec_hd = NULL;
//     // smf_close(hd);
//     // smf_destroy(hd);
//     // //smf_uninit();
// }

static const GAF_DECODER_FUNC_LIST_T gaf_offload_lc3_decoder_func_list =
{
    .decoder_init_buf_func        = gaf_audio_lc3_direct_decoder_buf_init,
    .decoder_init_func            = gaf_audio_lc3_direct_decoder_init,
    // .deinit          = wt_audio_opus_decoder_deinit,
    .decoder_decode_frame_func    = gaf_audio_direct_lc3_decode,
    // .deinit_buf      = wt_audio_opus_decoder_buf_deinit,
};

static const GAF_ENCODER_FUNC_LIST_T gaf_offload_lc3_encoder_func_list =
{
    .encoder_init_buf_func        = gaf_audio_lc3_direct_encoder_buf_init,
    .encoder_init_func            = gaf_audio_lc3_direct_encoder_init,
    // .deinit          = wt_audio_opus_encoder_deinit,
    .encoder_encode_frame_func    = gaf_audio_direct_lc3_encode,
    // .deinit_buf      = wt_audio_opus_encoder_buf_deinit,
};

const GAF_DECODER_FUNC_LIST_T* gaf_offload_lc3_decoder_funcs()
{
    return &gaf_offload_lc3_decoder_func_list;
}

const GAF_ENCODER_FUNC_LIST_T* gaf_offload_lc3_encoder_funcs()
{
    return &gaf_offload_lc3_encoder_func_list;
}
#endif