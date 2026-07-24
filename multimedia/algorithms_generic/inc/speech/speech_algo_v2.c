#include "speech_algo_cfg.h"
#include "custom_allocator.h"
#include "speech_algo.h"
#include "smf_sco_heap.h"
#include "ae_common.h"
#include "string.h"

#define CODE_VERSION 20260211
bool speech_set_status = false;
extern SpeechAlgoConfig speech_cfg_default;
SpeechAlgoConfig *speech_cfg = NULL;
#if defined(SPEECH_TX_1MIC_PREAF)
static Speech1MicPreafState *speech_tx_1mic_preaf_st = NULL;
static int16_t *af_out_buf = NULL;
#endif
#if defined(SPEECH_TX_1MIC_NS)
static Speech1MicNSState *speech_tx_1mic_ns_st = NULL;
void *model_data = NULL;
#endif
#if defined(SPEECH_TX_2MIC_PREAF)
static Speech2MicPreafState *speech_tx_2mic_preaf_st = NULL;
#endif
#if defined(SPEECH_TX_2MIC_NS8)
static Speech2MicNs8State *speech_tx_2mic_ns8_st = NULL;
static void *model_data_s = NULL;
static int16_t *af_out_buf = NULL;
#endif
#if defined(SPEECH_TX_AEC2FLOAT)
static Ec2FloatState *speech_tx_aec2float_st = NULL;
static int16_t * out_buf = NULL;
#endif
#if defined(SPEECH_TX_NS4)
static SpeechNs4State *speech_tx_ns4_st = NULL;
#endif
#if defined(SPEECH_TX_EQ)
static EqState *speech_tx_eq_st = NULL;
#endif
#if defined(SPEECH_TX_COMPEXP)
static CompexpState *speech_tx_compexp_st = NULL;
#endif
#if defined(SPEECH_RX_NS2FLOAT)
static SpeechNs2FloatState *speech_rx_ns2float_st = NULL;
#endif
#if defined(SPEECH_RX_EQ)
static EqState *speech_rx_eq_st = NULL;
#endif
#if defined(SPEECH_RX_COMPEXP)
static CompexpState *speech_rx_compexp_st = NULL;
#endif

int WEAK speech_store_config(void *cfg, uint16_t cfg_len)
{
    if (cfg_len != sizeof(SpeechAlgoConfig)) {
        TRACE(3, "%s: error cfg_len:%d, %d", __func__, cfg_len, sizeof(SpeechAlgoConfig));
        return 0;
    }

    if (speech_cfg)
    {
        memcpy(speech_cfg, cfg, sizeof(SpeechAlgoConfig));
        speech_set_status = true;
    }

    return 0;
}

int speech_algo_tx_open(int32_t sample_rate, int32_t frame_len, custom_allocator *allocator)
{
    AE_LOG(allocator,"%s: start !!!",__func__);
    AE_LOG(allocator,"CODE_VERSION: %d",CODE_VERSION);
#if defined(SPEECH_TX_1MIC_PREAF)
    AE_LOG(allocator,"SPEECH_TX_1MIC_PREAF");
    speech_tx_1mic_preaf_st = speech_1mic_preaf_create(sample_rate, frame_len, &speech_cfg_default.tx_1mic_preaf_cfg, allocator);
    af_out_buf = (int16_t *)scoup_calloc(frame_len, sizeof(int16_t));
#endif
#if defined(SPEECH_TX_1MIC_NS)
    AE_LOG(allocator,"SPEECH_TX_1MIC_NS");
    model_data = speech_nn_ns_get_model_data_96l();
    speech_tx_1mic_ns_st = speech_1mic_ns_create(model_data, &speech_cfg_default.tx_1mic_ns_cfg, allocator);
#if defined(SPEECH_TX_1MIC_PREAF)
    speech_1mic_ns_set_af_state(speech_tx_1mic_ns_st, speech_tx_1mic_preaf_st, speech_1mic_preaf_reset_coef);
#endif
#endif
#if defined(SPEECH_TX_2MIC_PREAF)
    AE_LOG(allocator,"SPEECH_TX_2MIC_PREAF");
    speech_tx_2mic_preaf_st = speech_2mic_preaf_create(sample_rate, frame_len, &speech_cfg_default.tx_2mic_preaf_cfg, allocator);
#endif
#if defined(SPEECH_TX_2MIC_NS8)
    AE_LOG(allocator,"SPEECH_TX_2MIC_NS8");
    model_data_s = speech_nn_ns_get_model_data_95s();
    speech_tx_2mic_ns8_st = speech_2mic_ns8_create(model_data_s, sample_rate, frame_len, &speech_cfg_default.tx_2mic_ns8_cfg, allocator);
#if defined(SPEECH_TX_2MIC_PREAF)
    speech_2mic_ns8_set_af_state(speech_tx_2mic_ns8_st, speech_tx_2mic_preaf_st, speech_2mic_preaf_reset_coef);
#endif
    af_out_buf = (int16_t *)scoup_calloc(frame_len * 2, sizeof(int16_t));
#endif
#if defined(SPEECH_TX_AEC2FLOAT)
    AE_LOG(allocator,"SPEECH_TX_AEC");
    out_buf = (int16_t *)scoup_calloc(frame_len, sizeof(int16_t));
    speech_tx_aec2float_st = ec2float_create_with_custom_allocator(sample_rate, frame_len, 0, &speech_cfg_default.tx_aec2float_cfg, allocator);
#endif
#if defined(SPEECH_TX_NS4)
    AE_LOG(allocator,"SPEECH_TX_NS4");
    speech_tx_ns4_st = speech_ns4_create_with_custom_allocator(sample_rate, frame_len, &speech_cfg_default.tx_ns4_cfg, allocator);
#endif
#if defined(SPEECH_TX_EQ)
    AE_LOG(allocator,"SPEECH_TX_EQ");
    speech_tx_eq_st = eq_init_with_custom_allocator(sample_rate, frame_len, &speech_cfg_default.tx_eq_cfg, allocator);
#endif
#if defined(SPEECH_TX_COMPEXP)
    AE_LOG(allocator,"SPEECH_TX_COMPEXP");
    speech_tx_compexp_st = compexp_create_with_custom_allocator(sample_rate, frame_len, &speech_cfg_default.tx_compexp_cfg, allocator);
#endif
    AE_LOG(allocator,"%s: end !!!",__func__);
    return 0;
}

int speech_algo_tx_process(int16_t *pcm_buf, int16_t *ref_buf, int16_t pcm_len, int16_t *out)
{

    #if defined(SPEECH_TX_1MIC_PREAF)
        speech_1mic_preaf_process(speech_tx_1mic_preaf_st, pcm_buf, ref_buf, pcm_len, af_out_buf);
    #endif

    #if defined(SPEECH_TX_1MIC_NS)
    #if defined(SPEECH_TX_1MIC_PREAF)
        speech_1mic_ns_process(speech_tx_1mic_ns_st, pcm_buf, ref_buf, pcm_len, af_out_buf);//output is pcm_buf
        memcpy(pcm_buf, af_out_buf, pcm_len*sizeof(int16_t));
    #else
        speech_1mic_ns_process(speech_tx_1mic_ns_st, pcm_buf, ref_buf, pcm_len, pcm_buf);
    #endif
    #endif

    #if defined(SPEECH_TX_2MIC_PREAF)
        speech_2mic_preaf_process(speech_tx_2mic_preaf_st, pcm_buf, ref_buf, pcm_len * 2, af_out_buf);
    #endif

    #if defined(SPEECH_TX_2MIC_NS8)
    #if defined(SPEECH_TX_2MIC_PREAF)
        speech_2mic_ns8_process(speech_tx_2mic_ns8_st, pcm_buf, ref_buf, pcm_len * 2, af_out_buf);
        for (int i=0; i<pcm_len; i++)
            pcm_buf[i] = af_out_buf[i];
    #else
        speech_2mic_ns8_process(speech_tx_2mic_ns8_st, pcm_buf, ref_buf, pcm_len * 2, pcm_buf);
    #endif
    #endif

    #if defined(SPEECH_TX_AEC2FLOAT)
    ec2float_process(speech_tx_aec2float_st, pcm_buf, ref_buf, pcm_len, out_buf);
    for (int i=0; i<pcm_len; i++)
        pcm_buf[i] = out_buf[i];
    #endif

    #if defined(SPEECH_TX_NS4)
    speech_ns4_process(speech_tx_ns4_st, pcm_buf, pcm_len);
    #endif

    #if defined(SPEECH_TX_EQ)
    eq_process(speech_tx_eq_st, pcm_buf, pcm_len);
    #endif

    #if defined(SPEECH_TX_COMPEXP)
    compexp_process(speech_tx_compexp_st, pcm_buf, pcm_len);
    #endif
    for (int i=0; i<pcm_len; i++)
        out[i] = pcm_buf[i];
    return 0;
}

int speech_algo_tx_close(void)
{
#if defined(SPEECH_TX_1MIC_PREAF)
    speech_1mic_preaf_destroy(speech_tx_1mic_preaf_st);
    if(af_out_buf)scoup_free(af_out_buf);
    af_out_buf = NULL;
#endif
#if defined(SPEECH_TX_1MIC_NS)
    speech_1mic_ns_destroy(speech_tx_1mic_ns_st);
#endif
#if defined(SPEECH_TX_2MIC_PREAF)
    speech_2mic_preaf_destroy(speech_tx_2mic_preaf_st);
#endif
#if defined(SPEECH_TX_2MIC_NS8)
    speech_2mic_ns8_destroy(speech_tx_2mic_ns8_st);
    if(af_out_buf)scoup_free(af_out_buf);
    af_out_buf = NULL;
#endif
#if defined(SPEECH_TX_AEC2FLOAT)
	if(out_buf)scoup_free(out_buf);
	ec2float_destroy(speech_tx_aec2float_st);
    out_buf = NULL;
#endif
#if defined(SPEECH_TX_NS4)
	speech_ns4_destroy(speech_tx_ns4_st);
#endif
#if defined(SPEECH_TX_EQ)
	eq_destroy(speech_tx_eq_st);
#endif
#ifdef SPEECH_TX_COMPEXP
	compexp_destroy(speech_tx_compexp_st);
#endif
    return 0;
}

int speech_algo_rx_open(int32_t sample_rate, int32_t frame_len, custom_allocator *allocator)
{
    AE_LOG(allocator,"%s: start !!!",__func__);
#if defined(SPEECH_RX_NS2FLOAT)
    AE_LOG(allocator,"SPEECH_RX_NS");
    speech_rx_ns2float_st = speech_ns2float_create_with_custom_allocator(sample_rate, frame_len, 0, &speech_cfg_default.rx_ns2float_cfg, allocator);
#endif
#if defined(SPEECH_RX_EQ)
    AE_LOG(allocator,"SPEECH_RX_EQ");
    speech_rx_eq_st = eq_init_with_custom_allocator(sample_rate, frame_len, &speech_cfg_default.rx_eq_cfg, allocator);
#endif
#if defined(SPEECH_RX_COMPEXP)
    AE_LOG(allocator,"SPEECH_RX_COMPEXP");
    speech_rx_compexp_st = compexp_create_with_custom_allocator(sample_rate, frame_len, &speech_cfg_default.rx_compexp_cfg, allocator);
#endif
    AE_LOG(allocator,"%s: end !!!",__func__);
    return 0;
}

int speech_algo_rx_process(int16_t *pcm_buf, int16_t pcm_len, int16_t *out)
{
#if defined(SPEECH_RX_NS2FLOAT)
    speech_ns2float_process(speech_rx_ns2float_st, pcm_buf, pcm_len);
#endif
#if defined(SPEECH_RX_EQ)
    eq_process(speech_rx_eq_st, pcm_buf, pcm_len);
#endif
#if defined(SPEECH_RX_COMPEXP)
    compexp_process(speech_rx_compexp_st, pcm_buf, pcm_len);
#endif
    for (int i=0; i<pcm_len; i++)
        out[i] = pcm_buf[i];
    return 0;
}

int speech_algo_rx_close(void)
{
#if defined(SPEECH_RX_NS2FLOAT)
    speech_ns2float_destroy(speech_rx_ns2float_st);
#endif
#if defined(SPEECH_RX_EQ)
    eq_destroy(speech_rx_eq_st);
#endif
#if defined(SPEECH_RX_COMPEXP)
    compexp_destroy(speech_rx_compexp_st);
#endif
    return 0;
}
