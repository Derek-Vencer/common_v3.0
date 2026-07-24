#if defined(UTILS_ESHELL_EN)
#include "eshell.h"
#include "hal_sysfreq.h"
#include "hal_trace.h"
#include "cmsis_os2.h"
#include "hal_timer.h"
#include "string.h"

#ifndef WEAK
#define WEAK				__attribute__((weak))
#endif  /* WEAK */

WEAK bool smf_msg_service_open()
{
    //  TRACE(0, "%s has not been implemented", __func__);
     return true;
}

WEAK void voip_down_test_play_file(const char *fname,uint16_t play_seq_num,uint16_t index)
{
    // TRACE(0, "%s has not been implemented", __func__);
}

static void do_smf_test(int argc, char *argv[])
{
    smf_msg_service_open();
    if (argc <= 1) return;
    if (strcmp(argv[1], "voip_test") == 0) {
        voip_down_test_play_file(0,0,0);
    }
}

WEAK void test_media_service_func(int argc, char *argv[])
{
    // TRACE(0, "%s has not been implemented", __func__);
}

static void do_test_media_service(int argc, char *argv[])
{
    test_media_service_func(argc, argv);
}

ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_SMF, "smf_test", "smf media api test", do_test_media_service);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_SMF, "smf", "smf test", do_smf_test);
#endif /* end UTILS_ESHELL_EN*/