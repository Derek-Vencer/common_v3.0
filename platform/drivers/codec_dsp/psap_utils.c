
#include "mm_dbg.h"
#include "psap_utils.h"
#include "hal_trace.h"

#define ROUND(x)        ((int)((x) + ((x) > 0 ? 0.5 : -0.5)))

/* NOTE:
    using psap_utils.c need to add this path in calling makefile:
    -Imultimedia/inc/audio/process/psap/include \
    -Iservices/nv_section/aud_section \
    -Iservices/nv_section/include \ 
*/

/* NOTE: using fastmath.h will have a slight deviation, */
/* math.h is a built-in C function, and the result is correct */ 
#if 0
#include "fastmath_impl_approx.h"
#include "fastmath.h"
#define _EXP(x)         fastexp(x)
#define _POW2(x)        fastpow2(x)
#define _POW10(x)       fastpow10(x)
#define _LOG(x)         fastlog(x)
#else
#include "math.h"
#define _EXP(x)         exp(x)
#define _POW2(x)        pow(2, x)
#define _POW10(x)       pow(10, x)
#define _LOG(x)         log(x)
#endif

/* test code: using this as test input */
#if 0
psap_cpd_user_cfg set_psap_cpd_cfg = {
    .cs=0.9999,
    .ct=-28.8,
    .ws=0.4,
    .wt=-20.0,
    .es=-0.9999,
    .et=-75,
    .at=1,
    .rt=50,
    .ta=0.1
};
#endif

psap_cpd_config psap_cpd_convert_cfg(const psap_cpd_user_cfg *cfg)
{
    float cwidth=16.0, din_width=24.0, din_frac=19.0, z_scale=1052365.0;
    psap_cpd_config psap_cpd_cfg;

    psap_cpd_cfg.psap_cpd_ct = ROUND((1/2.0)*z_scale*(float)_LOG(_POW10(cfg->ct/20.0)*_POW2(din_frac))/(float)_POW2(din_width-cwidth));
    psap_cpd_cfg.psap_cpd_cs = ROUND(cfg->cs*(float)_POW2(15));
    psap_cpd_cfg.psap_cpd_wt = ROUND((1/2.0)*z_scale*(float)_LOG(_POW10(cfg->wt/20.0)*_POW2(din_frac))/(float)_POW2(din_width-cwidth));
    psap_cpd_cfg.psap_cpd_ws = ROUND((cfg->ws*(float)_POW2(15)));
    psap_cpd_cfg.psap_cpd_et = ROUND((1/2.0)*z_scale*(float)_LOG(_POW10(cfg->et/20.0)*_POW2(din_frac))/(float)_POW2(din_width-cwidth));
    psap_cpd_cfg.psap_cpd_es = ROUND((float)_POW2(16))+ROUND(cfg->es*(float)_POW2(15));
    psap_cpd_cfg.psap_cpd_coefa_at = ROUND((float)_EXP(-1/(cfg->at/2/1000*96000))*(float)_POW2(15));
    psap_cpd_cfg.psap_cpd_coefb_at = ROUND((1-(float)_EXP(-1/(cfg->at/2/1000*96000)))*(float)_POW2(15));
    psap_cpd_cfg.psap_cpd_coefa_rt = ROUND((float)_EXP(-1/(cfg->rt/2/1000*96000))*(float)_POW2(15));
    psap_cpd_cfg.psap_cpd_coefb_rt = ROUND((1-(float)_EXP(-1/(cfg->rt/2/1000*96000)))*(float)_POW2(15));
    psap_cpd_cfg.psap_cpd_tava = 32768 - ROUND(cfg->ta*(float)_POW2(15));
    psap_cpd_cfg.psap_cpd_tavb = ROUND(cfg->ta*(float)_POW2(15));

/* debug code: output the result */
#if 0
    LOG_I( "ct = %x\n", psap_cpd_cfg.psap_cpd_ct);
    LOG_I( "cs = %x\n", psap_cpd_cfg.psap_cpd_cs);
    LOG_I( "wt = %x\n", psap_cpd_cfg.psap_cpd_wt);
    LOG_I( "ws = %x\n", psap_cpd_cfg.psap_cpd_ws);
    LOG_I( "et = %x\n", psap_cpd_cfg.psap_cpd_et);
    LOG_I( "es = %x\n", psap_cpd_cfg.psap_cpd_es);
    LOG_I( "at1 = %x\n", psap_cpd_cfg.psap_cpd_coefa_at);
    LOG_I( "at2 = %x\n", psap_cpd_cfg.psap_cpd_coefb_at);
    LOG_I( "rt1 = %x\n", psap_cpd_cfg.psap_cpd_coefa_rt);
    LOG_I( "rt2 = %x\n", psap_cpd_cfg.psap_cpd_coefb_rt);
    LOG_I( "ta1 = %x\n", psap_cpd_cfg.psap_cpd_tava);
    LOG_I( "ta2 = %x\n", psap_cpd_cfg.psap_cpd_tavb);
#endif

    return psap_cpd_cfg;
}