#include "speech_algo_cfg.h"
#include "custom_allocator.h"
#include "speech_algo.h"
#include "smf_sco_heap.h"
#include "ae_common.h"
#include "string.h"

#define CODE_VERSION 20260211
bool speech_set_status = false;
static int32_t *pcm32_buf = NULL;
static int32_t *ref32_buf = NULL;
static int32_t *pcm32_in = NULL;
extern SpeechAlgoConfig speech_cfg_default;
SpeechAlgoConfig *speech_cfg = NULL;
#if defined(SPEECH_TX_AEC2FLOAT)
static Ec2FloatState *speech_tx_aec2float_st = NULL;
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

int speech_store_config(void *cfg, uint16_t cfg_len)
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

static int speech_set_config(SpeechAlgoConfig *cfg)
{
#if defined(SPEECH_TX_AEC2FLOAT)
    ec2float_set_config(speech_tx_aec2float_st, &cfg->tx_aec2float_cfg, 0);
#endif
#if defined(SPEECH_TX_EQ)
    eq_set_config(speech_tx_eq_st, &cfg->tx_eq_cfg);
#endif
#if defined(SPEECH_TX_COMPEXP)
    compexp_set_config(speech_tx_compexp_st, &cfg->tx_compexp_cfg);
#endif
#if defined(SPEECH_RX_NS2FLOAT)
    speech_ns2float_set_config(speech_rx_ns2float_st, &cfg->rx_ns2float_cfg, 0);
#endif
#if defined(SPEECH_RX_EQ)
    eq_set_config(speech_rx_eq_st, &cfg->rx_eq_cfg);
#endif
#if defined(SPEECH_RX_COMPEXP)
    compexp_set_config(speech_rx_compexp_st, &cfg->rx_compexp_cfg);
#endif
    return 0;
}

int speech_algo_tx_open(int32_t sample_rate, int32_t frame_len, custom_allocator *allocator)
{
    AE_LOG(allocator,"%s: start !!!",__func__);
    AE_LOG(allocator,"CODE_VERSION: %d",CODE_VERSION);
    if(!speech_cfg)
    {
        speech_cfg = (SpeechAlgoConfig *)scoup_calloc(1, sizeof(SpeechAlgoConfig));
        memcpy(speech_cfg, &speech_cfg_default, sizeof(SpeechAlgoConfig));
    }
#ifdef SPEECH_TX_AEC2FLOAT
    AE_LOG(allocator,"SPEECH_TX_AEC");
    speech_tx_aec2float_st = ec2float_create_with_custom_allocator(sample_rate, frame_len, 0, &speech_cfg->tx_aec2float_cfg, allocator);
#endif
#ifdef SPEECH_TX_EQ
    AE_LOG(allocator,"SPEECH_TX_EQ");
    speech_tx_eq_st = eq_init_with_custom_allocator(sample_rate, frame_len, &speech_cfg->tx_eq_cfg, allocator);
#endif
#ifdef SPEECH_TX_COMPEXP
    AE_LOG(allocator,"SPEECH_TX_COMPEXP");
    speech_tx_compexp_st = compexp_create_with_custom_allocator(sample_rate, frame_len, &speech_cfg->tx_compexp_cfg, allocator);
#endif
    pcm32_buf = (int32_t *)scoup_calloc(frame_len, sizeof(int32_t));
    ref32_buf = (int32_t *)scoup_calloc(frame_len, sizeof(int32_t));
    AE_LOG(allocator,"%s: end !!!",__func__);
    return 0;
}

int speech_algo_tx_process(int16_t *pcm_buf, int16_t *ref_buf, int16_t pcm_len, int16_t *out)
{
    if(speech_set_status)
    {
        speech_set_config(speech_cfg);
        speech_set_status = false;
    }
    for(int i=0;i<pcm_len;i++)
    {
        pcm32_buf[i] = (int32_t)pcm_buf[i];
        ref32_buf[i] = (int32_t)ref_buf[i];
    }
#ifdef SPEECH_TX_AEC2FLOAT
    ec2float_process(speech_tx_aec2float_st, pcm32_buf, ref32_buf, pcm_len, pcm32_buf);
#endif

#ifdef SPEECH_TX_EQ
    eq_process_int24(speech_tx_eq_st, pcm32_buf, pcm_len);
#endif

#ifdef SPEECH_TX_COMPEXP
    for(int i=0;i<pcm_len;i++)
        pcm32_buf[i] <<= 8;
    compexp_process_int24_opti(speech_tx_compexp_st, pcm32_buf, pcm_len);
#endif

#ifdef SPEECH_TX_COMPEXP
    for(int i=0;i<pcm_len;i++)
        out[i] = (int16_t)(pcm32_buf[i] >> 8);
#else
    for(int i=0;i<pcm_len;i++)    
        out[i] = (int16_t)pcm32_buf[i];
#endif
    return 0;
}

int speech_algo_tx_close(void)
{
    if(pcm32_buf) {
        scoup_free(pcm32_buf);
        pcm32_buf = NULL;
    }
    if(ref32_buf) {
        scoup_free(ref32_buf);
        pcm32_buf = NULL;
    }
    if(speech_cfg) {
        scoup_free(speech_cfg);
        speech_cfg = NULL;
    }
#if defined(SPEECH_TX_AEC2FLOAT)
    if(speech_tx_aec2float_st) {
        ec2float_destroy(speech_tx_aec2float_st);
        speech_tx_aec2float_st = NULL;
    }
#endif
#if defined(SPEECH_TX_EQ)
    if(speech_tx_eq_st) {
        eq_destroy(speech_tx_eq_st);
        speech_tx_eq_st = NULL;
    }
#endif
#ifdef SPEECH_TX_COMPEXP
    if(speech_tx_compexp_st) {
        compexp_destroy(speech_tx_compexp_st);
        speech_tx_compexp_st = NULL;
    }
#endif
    return 0;
}

int speech_algo_rx_open(int32_t sample_rate, int32_t frame_len, custom_allocator *allocator)
{
    AE_LOG(allocator,"%s: start !!!",__func__);
    if(!speech_cfg)
    {
        speech_cfg = (SpeechAlgoConfig *)scodown_calloc(1, sizeof(SpeechAlgoConfig));
        memcpy(speech_cfg, &speech_cfg_default, sizeof(SpeechAlgoConfig));
    }
#ifdef SPEECH_RX_NS2FLOAT
    AE_LOG(allocator,"SPEECH_RX_NS");
    speech_rx_ns2float_st = speech_ns2float_create_with_custom_allocator(sample_rate, frame_len, 0, &speech_cfg->rx_ns2float_cfg, allocator);
#endif
#ifdef SPEECH_RX_EQ
    AE_LOG(allocator,"SPEECH_RX_EQ");
    speech_rx_eq_st = eq_init_with_custom_allocator(sample_rate, frame_len, &speech_cfg->rx_eq_cfg, allocator);
#endif
#ifdef SPEECH_RX_COMPEXP
    AE_LOG(allocator,"SPEECH_RX_COMPEXP");
    speech_rx_compexp_st = compexp_create_with_custom_allocator(sample_rate, frame_len, &speech_cfg->rx_compexp_cfg, allocator);
#endif
    pcm32_in = (int32_t *)scodown_calloc(frame_len, sizeof(int32_t));
    if(!pcm32_in) {
        AE_LOG(allocator,"malloc fail");
    }
    AE_LOG(allocator,"%s: end !!!",__func__);
    return 0;
}

int speech_algo_rx_process(int16_t *pcm_buf, int16_t pcm_len, int16_t *out)
{
    for(int i=0;i<pcm_len;i++)
    {
        pcm32_in[i] = (int32_t)pcm_buf[i];
    }
#ifdef SPEECH_RX_NS2FLOAT
    speech_ns2float_process_int24(speech_rx_ns2float_st, pcm32_in, pcm_len);
#endif
#ifdef SPEECH_RX_EQ
    eq_process_int24(speech_rx_eq_st, pcm32_in, pcm_len);
#endif
#ifdef SPEECH_RX_COMPEXP
    for (uint32_t i = 0; i < pcm_len; i++) {
        pcm32_in[i] = (pcm32_in[i] << 8);
    }
    compexp_process_int24_opti(speech_rx_compexp_st, pcm32_in, pcm_len);
#endif
#ifdef SPEECH_RX_COMPEXP
    for (uint32_t i = 0; i < pcm_len; i++)
        out[i] = (int16_t)(pcm32_in[i] >> 8);
#else
    for (uint32_t i = 0; i < pcm_len; i++)    
        out[i] = (int16_t)pcm32_in[i];
#endif
    return 0;
}

int speech_algo_rx_close(void)
{
    if(pcm32_in) {
        scodown_free(pcm32_in);
        pcm32_in = NULL;
    }
    if(speech_cfg) {
        scodown_free(speech_cfg);
        speech_cfg = NULL;
    }
#if defined(SPEECH_RX_NS2FLOAT)
    if(speech_rx_ns2float_st) {
        speech_ns2float_destroy(speech_rx_ns2float_st);
        speech_rx_ns2float_st = NULL;
    }
#endif
#if defined(SPEECH_RX_EQ)
    if(speech_rx_eq_st) {
        eq_destroy(speech_rx_eq_st);
        speech_rx_eq_st = NULL;
    }
#endif
#ifdef SPEECH_RX_COMPEXP
    if(speech_rx_compexp_st) {
        compexp_destroy(speech_rx_compexp_st);
        speech_rx_compexp_st = NULL;
    }
#endif
    return 0;
}
