#if defined(UTILS_ESHELL_EN)
#include "eshell.h"
#include "hal_sysfreq.h"
#include "hal_trace.h"
#include "cmsis_os2.h"
#include "hal_timer.h"

#if defined(IMS_SUPPORT)
#include "ping.h"
#include "tcpip.h"

extern int tmtc_voip_init(void);
extern int tmtc_voip_reg(void);
extern int tmtc_voip_call(int argc, char *argv[]);
extern int tmtc_volte_init(void);
extern int tmtc_volte_reg(void);
extern int tmtc_volte_answer(int argc, char *argv[]);
extern int tmtc_volte_term(int argc, char *argv[]);
extern int tmtc_volte_setsmsreport(int argc, char *argv[]);
extern int tmtc_volte_sendmsg(int argc, char *argv[]);
extern int tmtc_volte_senddtmf(int argc, char *argv[]);
extern int tmtc_volte_unreg(void);
extern int tmtc_volte_hold(int argc, char *argv[]);
extern int tmtc_volte_unhold(int argc, char *argv[]);

static void do_voip_init(int argc, char *argv[])
{
    tmtc_voip_init();
}

static void do_voip_reg(int argc, char *argv[])
{
    tmtc_voip_reg();
}

static void do_voip_call(int argc, char *argv[])
{
    if (argc < 2) {
        eshell_putstring("%s param err! Usage:call [phone number]", __func__);
        return ;
    }

    tmtc_voip_call(argc, argv);
}

static void do_volte_init(int argc, char *argv[])
{
#ifndef IMS_ON_CP
    hal_sysfreq_req(HAL_SYSFREQ_USER_APP_0, HAL_CMU_FREQ_104M);
#endif
    tcpip_init(NULL, NULL);
    tmtc_volte_init();
}

static void do_volte_reg(int argc, char *argv[])
{
    tmtc_volte_reg();
}

static void do_volte_answer(int argc, char *argv[])
{
    tmtc_volte_answer(argc, argv);
}

static void do_volte_term(int argc, char *argv[])
{
    tmtc_volte_term(argc, argv);
}

static void do_volte_setsmsreport(int argc, char *argv[])
{
    if (argc < 2) {
        eshell_putstring("%s param err! Usage:setsmsreport [0 or 1]", __func__);
        return ;
    }

    tmtc_volte_setsmsreport(argc, argv);
}

static void do_volte_sendmsg(int argc, char *argv[])
{
    if (argc < 3) {
        eshell_putstring("%s param err! Usage:sendmsg [phone number] [content]", __func__);
        return ;
    }

    tmtc_volte_sendmsg(argc, argv);
}

static void do_volte_senddtmf(int argc, char *argv[])
{
    if (argc < 2) {
        eshell_putstring("%s param err! Usage:dtmf [number]", __func__);
        return ;
    }

    tmtc_volte_senddtmf(argc, argv);
}

static void do_volte_hold(int argc, char *argv[])
{
    if (argc < 2) {
        eshell_putstring("%s param err! Usage:hold [1 or 2]", __func__);
        return ;
    }
    tmtc_volte_hold(argc, argv);
}

static void do_volte_unhold(int argc, char *argv[])
{
    if (argc < 2) {
        eshell_putstring("%s param err! Usage:unhold [1 or 2]", __func__);
        return ;
    }
    tmtc_volte_unhold(argc, argv);
}

static void do_volte_unreg(int argc, char *argv[])
{
    tmtc_volte_unreg();
}

static void modem_ping_usage(void)
{
    eshell_putstring("modem ping Usage: \r\n");
    eshell_putstring("  ping specified ip address or domain name;\r\n");
    eshell_putstring("  modem_ping [@1] [@2] [@3] [@4] [@5]\r\n");
    eshell_putstring("  @1:target ip address or domain name\r\n");
    eshell_putstring("  @2:count of ping performance (default 4 times)\r\n");
    eshell_putstring("  @3:time interval of each performance (default 10 ms)\r\n");
    eshell_putstring("  @4:length of sent data packet (default 32byte)\r\n");
    eshell_putstring("  @5:show ping echo (1 - yes, 0 - no, default 1)\r\n");
    eshell_putstring("  eg: modem_ping www.baidu.com\r\n");
}

static void do_modem_ping(int argc, char *argv[])
{
    int ret;
    int count, interval, len, echo_level;
    in_addr_t ping_dst;
    if (argc < 2) {
        eshell_putstring("ERROR_ARG\r\n");
        goto usage;
    }
    if (argc >= 3) {
        count = atoi(argv[2]);
    } else {
        count = 4;
    }

    if (argc >= 4) {
        interval = atoi(argv[3]);
    } else {
        interval = 1000;
    }

    if (argc >= 5) {
        len = atoi(argv[4]);
    } else {
        len = 32;
    }

    if (argc >= 6) {
        echo_level = atoi(argv[5]);
    } else {
        echo_level = 1;
    }

    if (count < 0 || interval < 0 || len < 0) {
        eshell_putstring("ERROR_ARG\r\n");
        goto usage;
    }
    if (interval < 5) {
        interval = 5;
    } else if (interval > 10 * 1000) {
        interval = 10 * 1000;
    }

    if (len > 1460) {
        len = 1460;
    }

    if (echo_level > 0) {
        echo_level = 1;
    } else {
        echo_level = 0;
    }

    // if dest is x.x.x.x
    ret = inet_aton(argv[1], &ping_dst);
    if (!ret) {
        // if dest needs dns resolving
        struct hostent *host = gethostbyname(argv[1]);
        if (!host) {
            eshell_putstring("ERROR_FAILE\r\n");
            return;
        }
        ping_dst = *(in_addr_t *)(host->h_addr_list[0]);
    }

    ping_init(&ping_dst, count, interval, len, echo_level);
    eshell_putstring("%s OK\r\n", __FUNCTION__);
    return;

usage:
    modem_ping_usage();
}

ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_IMS, "tmtc", "voip init, no arg", do_voip_init);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_IMS, "reg", "voip resiter to registar,no arg", do_voip_reg);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_IMS, "call", "call xxx", do_voip_call);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_IMS, "tmtclte", "volte init,no arg", do_volte_init);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_IMS, "ereg", "volte resiter to lte registar", do_volte_reg);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_IMS, "answer", "answer an incoming call", do_volte_answer);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_IMS, "term", "terminate the call", do_volte_term);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_IMS, "setsmsreport", "set sms report", do_volte_setsmsreport);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_IMS, "sendmsg", "send a message", do_volte_sendmsg);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_IMS, "dtmf", "send a dtmf ", do_volte_senddtmf);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_IMS, "hold", "hold a call", do_volte_hold);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_IMS, "unhold", "unhold a held call", do_volte_unhold);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_IMS, "unreg", "de-resiter to registar", do_volte_unreg);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_NET, "modem_ping", "do modem ping", do_modem_ping);
#endif /* end IMS_SUPPORT*/

#if defined(SMF)
extern void testLocalPlayer(void);
extern void testLocalRecord(void);
extern bool smf_msg_service_open();

static void do_local_play(int argc, char *argv[])
{
    smf_msg_service_open();
    testLocalPlayer();
}

static void do_local_record(int argc, char *argv[])
{
    smf_msg_service_open();
    testLocalRecord();
}

ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_IMS, "record", "smf local record test", do_local_record);
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_IMS, "play", "smf local play test", do_local_play);
#endif /* end SMF*/
#endif /* end UTILS_ESHELL_EN*/