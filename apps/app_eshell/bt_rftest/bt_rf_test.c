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
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"
#include "hal_location.h"
#include "hal_trace.h"
#include "bt_drv_interface.h"
#include "pmu.h"
#include "hal_analogif.h"
#include "bt_drv.h"

#ifdef UTILS_ESHELL_EN
#include "eshell.h"

#if 0
#define BTRF_TRACE(str, ...) TRACE(0, str, ##__VA_ARGS__)
#define BTRF_DUMP DUMP8
#else
#define BTRF_TRACE(str, ...)
#define BTRF_DUMP
#endif

#define __USE_NV_BT_ADDR_TEST__

__WEAK void nosignal_set_max_power_level(void)
{
}
__WEAK void nosignal_clear_max_power_level(void)
{
}

/*
 * Based on the maximum number of parameters for HCI_LE_Generate_DHKey
 * See BT Core Spec V5.2 Vol. 4, Part E, section 7.8.37
 */
typedef struct {
    const char* string;
    uint8_t idex;
} BT_PACKET_TYPE;

typedef struct {
    const char* string;
    uint8_t idex;
} BT_PAYLOAD_TYPE;

struct bt_test_cmd {
    const char *cmd;
    int (*handler) (int argc, char *argv[]);
};

static const BT_PACKET_TYPE  bt_packet_type_cfg[] = {
    {"ID_NUL",0x0},
    {"POLL",  0x1},
    {"FHS",   0x2},
    {"DM1",   0x3},
    {"DH1",   0x4},
    {"2-DH1", 0x4},
    {"3-DH1", 0x8},
    {"HV1",   0x5},
    {"HV2",   0x6},
    {"2-EV3", 0x6},
    {"HV3",   0x7},
    {"EV3",   0x7},
    {"3-EV3", 0x7},
    {"DV",    0x8},
    {"AUX1",  0x9},
    {"DM3",   0xa},
    {"DH3",   0xb},
    {"2-DH3", 0xa},
    {"3-DH3", 0xb},
    {"EV4",   0xc},
    {"2-EV5", 0xc},
    {"EV5",   0xd},
    {"3-EV5", 0xd},
    {"DM5",   0xe},
    {"DH5",   0xf},
    {"2-DH5", 0xe},
    {"3-DH5", 0xf},
};

static const BT_PAYLOAD_TYPE  bt_payload_type_cfg[] = {
    {"0x00",  0x0},
    {"0xFF",  0x1},
    {"0x55",  0x2},
    {"0x0F",  0x3},
    {"PRBS9", 0x4},
};

static const BT_PAYLOAD_TYPE  ble_payload_type_cfg[] = {
    {"PRBS9", 0x0},
    {"0x0F",  0x1},
    {"0x55",  0x2},
    {"PRBS15",0x3},
    {"0xFF",  0x4},
    {"0x00",  0x5},
    {"0xF0",  0x6},
    {"0xAA",  0x7},
};

static int bt_atoi(char s[])
{
    int i;
    int n = 0;

    for (i = 0; s[i] >= '0' && s[i] <= '9'; ++i)
    {
        n = 10 * n + (s[i] - '0');
    }

    return n;
}

static uint8_t packet2index(char *Packet)
{
    for (int i = 0; i < (sizeof(bt_packet_type_cfg) / sizeof(BT_PACKET_TYPE)); i++)
    {
        if(strncmp(Packet, bt_packet_type_cfg[i].string, strlen(Packet)) == 0)
        {
            return bt_packet_type_cfg[i].idex;
        }
    }

    BTRF_TRACE("Packet2Index packet cmd unknown!");

    return 0xff;
}

static uint8_t payload2index(char *Payload)
{
    for (int i = 0; i < (sizeof(bt_payload_type_cfg) / sizeof(BT_PACKET_TYPE)); i++)
    {
        if(strncmp(Payload, bt_payload_type_cfg[i].string, strlen(Payload)) == 0)
        {
            return bt_payload_type_cfg[i].idex;
        }
    }

    BTRF_TRACE("Payload2Index payload cmd unknown!");

    return 0xff;
}

static uint8_t payload2index_ble(char *Payload)
{
    for (int i = 0; i < (sizeof(ble_payload_type_cfg) / sizeof(BT_PACKET_TYPE)); i++)
    {
        if(strncmp(Payload, ble_payload_type_cfg[i].string, strlen(Payload)) == 0)
        {
            return ble_payload_type_cfg[i].idex;
        }
    }

    BTRF_TRACE("Payload2Index payload cmd unknown!");

    return 0xff;
}

#define BT_GAP_LE_PHY_1M       1
#define BT_GAP_LE_PHY_2M       2
#define BT_GAP_LE_PHY_CODED    3

static uint8_t hexChar2Dec(const char c)
{
  int r = 0;
  if ((c >= '0') && (c <= '9'))
    r = c-'0';
  else if ((c >= 'a') && (c <= 'f'))
    r = c-'a'+10;
  else if ((c >= 'A') && (c <= 'F'))
    r = c-'A'+10;
  else
    r = 16; /* invalid hex character */

  return (uint8_t)r;
}

static int hexString2CharBuf(const char *string, uint8_t *charBuf, uint32_t charBufLength)
{
  uint32_t i, k = 0;
  uint8_t hNibble, lNibble;

  /* sanity checks */
  if (string[0] == '\0') {
    return -1; /* invalid string size */
  }

  if (charBufLength<=0){
    return -2; /* invalid buffer size */
  }

  /* convert to hex characters to corresponding 8bit value */
  for (i=0;(string[i]!='\0')&&((i>>1)<charBufLength);i+=2) {
    k = i>>1;
    hNibble = hexChar2Dec(string[i]);
    lNibble = hexChar2Dec(string[i+1]);
    if ((hNibble == 16) || (lNibble == 16)) {
      return -3; /* invalid character */
    }
    charBuf[k] = ((hNibble<<4)&0xf0) + lNibble;
  }

  /* check if last character was string terminator */
  if ((string[i-2]!=0) && (string[i]!=0)) {
    return -1; /* invalid string size */
  }

  /* fill charBuffer with zeros */
  for (i=k+1;i<charBufLength;i++) {
    charBuf[i] = 0;
  }

  return 0;
}

uint16_t check_paylaod_size(uint8_t edr,uint8_t type)
{
    uint16_t chk_paylaod_size = 0;
    switch(type)
    {
        case 0x00:    //ID_NUL_TYPE
            chk_paylaod_size = 0;
        break;
        case 0x01:   //POLL_TYPE
            chk_paylaod_size = 0;
        break;
        case 0x02:    //FHS_TYPE
            chk_paylaod_size = 18;
        break;
        case 0x03:
            chk_paylaod_size = 17;
        break;
        case 0x04:
            if(edr)
                chk_paylaod_size = 54;
            else
                chk_paylaod_size = 27;
        break;
        case 0x05:
            chk_paylaod_size = 10;
        break;
        case 0x06:
            if(edr)
                chk_paylaod_size = 60;
            else
                chk_paylaod_size = 20;
        break;
        case 0x07:
            if(edr)
                chk_paylaod_size = 90;
            else
                chk_paylaod_size = 30;
        break;
        case 0x08:
            if(edr)
                chk_paylaod_size = 83;
            else
                chk_paylaod_size = 10;
        break;
        case 0x09:
            chk_paylaod_size = 29;
        break;
        case 0x0A:
            if(edr)
                chk_paylaod_size = 367;
            else
                chk_paylaod_size = 121;
        break;
        case 0x0B:
            if(edr)
                chk_paylaod_size = 552;
            else
                chk_paylaod_size = 183;
        break;
        case 0x0C:
            if(edr)
                chk_paylaod_size = 360;
            else
                chk_paylaod_size = 120;
        break;
        case 0x0D:
            if(edr)
                chk_paylaod_size = 540;
            else
                chk_paylaod_size = 180;
        break;
        case 0x0E:
            if(edr)
                chk_paylaod_size = 679;
            else
                chk_paylaod_size = 224;
        break;
        case 0x0F:
            if(edr)
                chk_paylaod_size = 1021;
            else
                chk_paylaod_size = 339;
        break;
        default :
        break;
    }
    return chk_paylaod_size;
}

/* mac addr XX:XX:XX:XX:XX:XX */
static bool str_is_mac_addr(char *str)
{
    char *token, lst[17];

    if (strlen(str) != 17) {
        return false;
    }

    strncpy(lst, str, 17);
    token = strtok(lst, ":");
    while (token != NULL) {
        if (strlen(token) != 2) {
            return false;
        }
        for (int i = 0; i < strlen(token); i++) {
            if ((token[i] < '0' || token[i] > '9') &&
                (token[i] < 'a' || token[i] > 'f') &&
                (token[i] < 'A' || token[i] > 'F')) {
                return false;
            }
        }
        token = strtok(NULL, ":");
    }

    return true;
}

static void ble_daemon(const void *arg);

osThreadDef(ble_daemon, (osPriorityAboveNormal), 1, (1024*4), "ble_nosignal_task");

static bool ble_nosignal_start_flag = 0;
static void ble_daemon(const void *arg)
{
    while(1){
        osDelay(100);

        if(ble_nosignal_start_flag){
            ble_nosignal_uart_bridge_loop();
        }
    }

    return ;
}
#ifdef __WATCHER_DOG_RESET__
extern int app_wdt_close(void);
#endif
#ifdef PWR_CTRL_8DBM
SRAM_DATA_LOC uint8_t HciTxBuff[32] = {0x01,0x87,0xFC,0x1C,0x00,0xe8,0x03,0x00,0x00,0x00,0x00,0x03,0x55,0x55,0x55,0x55,0x00,0x00,
                        0x01,0x00,0x0F,0x03,0x53,0x01,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF};
#else
SRAM_DATA_LOC uint8_t HciTxBuff[32] = {0x01,0x87,0xFC,0x1C,0x00,0xe8,0x03,0x00,0x00,0x00,0x00,0x05,0x55,0x55,0x55,0x55,0x00,0x00,
                        0x01,0x00,0x0F,0x03,0x53,0x01,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF};
#endif
#ifdef __EBQ_TEST__
SRAM_DATA_LOC uint8_t feature[] = {0x01,0x81,0xFC,0x08,0xBF, 0xFE, 0xCF,0xFE,0xDB,0xFF,0x5B,0x87};
#else
SRAM_DATA_LOC uint8_t feature[] = {0x01,0x81,0xFC,0x08,0xBF, 0xEE, 0xCD,0xFE,0xC3,0xFF,0x7B,0x87};
#endif
SRAM_DATA_LOC uint8_t hci_cmd_enable_dut[] = {0x01,0x03, 0x18, 0x00};
SRAM_DATA_LOC uint8_t hci_cmd_enable_allscan[] = {0x01, 0x1a, 0x0c, 0x01, 0x03};
SRAM_DATA_LOC uint8_t hci_cmd_autoaccept_connect[] = {0x01,0x05, 0x0c, 0x03, 0x02, 0x00, 0x02};
SRAM_BSS_LOC uint8_t HciRxBuff[64],HciRxBuffLen;
SRAM_BSS_LOC uint16_t paylaod_size;
SRAM_BSS_LOC uint8_t dataBuff[24];
SRAM_BSS_LOC uint8_t HciBuff[32];

#ifdef __USE_NV_BT_ADDR_TEST__
#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif
static void bt_rf_set_dut_addr(uint8_t *addr)
{
    uint8_t length = 0,ret = 0;

    HciBuff[0] = 0x01;
    HciBuff[1] = 0x72;
    HciBuff[2] = 0xFC;
    HciBuff[3] = 0x07;
    HciBuff[4] = 0x00;

    memcpy(&HciBuff[5], addr, 6);
    btdrv_hci_bridge_loop(HciBuff,11,HciRxBuff,&HciRxBuffLen);
    BTRF_TRACE("HciRxBuffLen %d\n",HciRxBuffLen);
    BTRF_DUMP("%02x ",HciRxBuff,HciRxBuffLen);
    BTRF_TRACE("success:%d length:%d\n",ret,length);

    return;
}
#endif

extern void BESHCI_Open(void);
extern void BESHCI_Close(void);
static osThreadId besbt_tid;

static int cmd_bt_mp_start(int argc, char *argv[])
{
    BTRF_TRACE("%s...\n",__func__);
    BTRF_TRACE("init bt stack ");
   // BESHCI_Close();
    bt_drv_config_after_hci_reset();
    btdrv_hci_bridge_start();
#ifdef __WATCHER_DOG_RESET__
    app_wdt_close();
#endif
    besbt_tid = osThreadCreate(osThread(ble_daemon), NULL);
    if (besbt_tid < 0)
    {
        BTRF_TRACE("ble_daemon: ERROR: Failed to start ble_daemon\n");
        return -1;
    }

    return 0;
}

static int cmd_bt_mp_stop(int argc, char *argv[])
{
    BTRF_TRACE("%s...\n",__func__);
    BTRF_TRACE("deinit bt stack ");
    //BESHCI_Open();
    btdrv_hci_bridge_stop();
#ifdef __WATCHER_DOG_RESET__
    app_wdt_open(30);
#endif
    osThreadTerminate(besbt_tid);

    return 0;
}

static int cmd_bt_bdr_tx_signal(int argc, char *argv[])
{
    uint8_t length = 0,ret = 0;
    BTRF_TRACE("%s ...\n",__func__);
    BTRF_TRACE("argc %d\n",argc);

    nosignal_set_max_power_level();

    dataBuff[0] = bt_atoi(argv[1]);
    dataBuff[1] = packet2index(argv[2]);
    dataBuff[2] = payload2index(argv[3]);
    dataBuff[3] = bt_atoi(argv[4]);
    dataBuff[4] = bt_atoi(argv[5]);

    HciTxBuff[4] = 0x00;
    HciTxBuff[7] = dataBuff[4];
    HciTxBuff[19] = 0x00;
    HciTxBuff[9]  = dataBuff[0];
    HciTxBuff[20] = dataBuff[1];
    HciTxBuff[21] = dataBuff[2];
    HciTxBuff[11] = dataBuff[3];
    paylaod_size = check_paylaod_size(HciTxBuff[19],HciTxBuff[20]);
    HciTxBuff[22] = paylaod_size%0x100;
    HciTxBuff[23] = paylaod_size/0x100;
    btdrv_hci_bridge_loop(HciTxBuff,sizeof(HciTxBuff),HciRxBuff,&HciRxBuffLen);

    BTRF_TRACE("HciRxBuffLen %d\n",HciRxBuffLen);
    BTRF_DUMP("%02x ",HciRxBuff,HciRxBuffLen);
    BTRF_TRACE("success:%d length:%d param:\n",ret,length);

    return 0;
}

static int cmd_bt_edr_tx_signal(int argc, char *argv[])
{
    uint8_t length = 0,ret = 0;

    BTRF_TRACE("%s ...\n",__func__);

    nosignal_set_max_power_level();

    dataBuff[0] = bt_atoi(argv[1]);
    dataBuff[1] = packet2index(argv[2]);
    dataBuff[2] = payload2index(argv[3]);
    dataBuff[3] = bt_atoi(argv[4]);
    dataBuff[4] = bt_atoi(argv[5]);
    HciTxBuff[4] = 0x00;
    HciTxBuff[7] = dataBuff[4];
    HciTxBuff[19] = 0x01;
    HciTxBuff[9]  = dataBuff[0];
    HciTxBuff[20] = dataBuff[1];
    HciTxBuff[21] = dataBuff[2];
    HciTxBuff[11] = dataBuff[3];
    paylaod_size = check_paylaod_size(HciTxBuff[19],HciTxBuff[20]);
    HciTxBuff[22] = paylaod_size%0x100;
    HciTxBuff[23] = paylaod_size/0x100;
    btdrv_hci_bridge_loop(HciTxBuff,sizeof(HciTxBuff),HciRxBuff,&HciRxBuffLen);
    BTRF_TRACE("HciRxBuffLen %d\n",HciRxBuffLen);
    BTRF_DUMP("%02x ",HciRxBuff,HciRxBuffLen);
    BTRF_TRACE("success:%d length:%d param:\n",ret,length);

    return 0;
}


static int cmd_bt_bdr_rx_signal(int argc, char *argv[])
{
    uint8_t length = 0,ret = 0;

    BTRF_TRACE("%s ...\n",__func__);

    dataBuff[0] = bt_atoi(argv[1]);
    dataBuff[1] = packet2index(argv[2]);
    dataBuff[2] = payload2index(argv[3]);

    if (argc == 5) {
        if (str_is_mac_addr(argv[4])) {
            for (int i = 0; i < 6; i++, argv[4] += 3) {
                HciTxBuff[12+i] = strtoul(argv[4], NULL, 16);
            }
        } else {
            BTRF_TRACE("%s bt mac format err\n", argv[4]);
            return -1;
        }
    }

    HciTxBuff[4] = 0x01;
    HciTxBuff[19] = 0x00;
    HciTxBuff[10] = dataBuff[0];
    HciTxBuff[20] = dataBuff[1];
    HciTxBuff[21] = dataBuff[2];
    paylaod_size = check_paylaod_size(HciTxBuff[19],HciTxBuff[20]);
    HciTxBuff[22] = paylaod_size%0x100;
    HciTxBuff[23] = paylaod_size/0x100;
    btdrv_hci_bridge_loop(HciTxBuff,sizeof(HciTxBuff),HciRxBuff,&HciRxBuffLen);

    BTRF_TRACE("HciRxBuffLen %d\n",HciRxBuffLen);
    BTRF_DUMP("%02x ",HciRxBuff,HciRxBuffLen);
    BTRF_TRACE("success:%d length:%d param:\n",ret,length);

    return 0;
}

static int cmd_bt_edr_rx_signal(int argc, char *argv[])
{
    uint8_t length = 0,ret = 0;

    BTRF_TRACE("%s ...\n",__func__);

    dataBuff[0] = bt_atoi(argv[1]);
    dataBuff[1] = packet2index(argv[2]);
    dataBuff[2] = payload2index(argv[3]);

    HciTxBuff[4] = 0x01;
    HciTxBuff[19] = 0x01;
    HciTxBuff[10] = dataBuff[0];
    HciTxBuff[20] = dataBuff[1];
    HciTxBuff[21] = dataBuff[2];
    paylaod_size = check_paylaod_size(HciTxBuff[19],HciTxBuff[20]);
    HciTxBuff[22] = paylaod_size%0x100;
    HciTxBuff[23] = paylaod_size/0x100;
    btdrv_hci_bridge_loop(HciTxBuff,sizeof(HciTxBuff),HciRxBuff,&HciRxBuffLen);

    BTRF_TRACE("HciRxBuffLen %d\n",HciRxBuffLen);
    BTRF_DUMP("%02x ",HciRxBuff,HciRxBuffLen);
    BTRF_TRACE("success:%d length:%d param:\n",ret,length);

    return 0;
}

static int cmd_bt_stop_tx_signal(int argc, char *argv[])
{
    uint16_t length = 0,ret = 0;

    BTRF_TRACE("%s ...\n",__func__);

    nosignal_clear_max_power_level();
    HciTxBuff[4] = 0x02;
    btdrv_hci_bridge_loop(HciTxBuff,sizeof(HciTxBuff),HciRxBuff,&HciRxBuffLen);
    BTRF_TRACE("HciRxBuffLen %d\n",HciRxBuffLen);
    BTRF_DUMP("%02x ",HciRxBuff,HciRxBuffLen);
    length = HciRxBuff[8]*0x100+HciRxBuff[7];
    BTRF_TRACE("success:%d send packet num length:%d \n",ret,length);

    return length;
}

static int cmd_bt_stop_rx_signal(int argc, char *argv[])
{
    uint16_t length = 0,ret = 0;
    uint16_t head_error,payload_error,adv_estsw,adv_esttpl;
    uint32_t payload_bit_error;
    uint32_t reg_addr = 0;

    BTRF_TRACE("%s ...\n",__func__);
    memset(dataBuff,0,sizeof(dataBuff));
    if(argc > 1 && strlen(argv[1])>=8)
    {
        hexString2CharBuf(argv[1],dataBuff,4);
        reg_addr = dataBuff[0]*0x1000000 + dataBuff[1]*0x10000 + dataBuff[2]*0x100 + dataBuff[3];
    }

    HciTxBuff[4] = 0x02;
    btdrv_hci_bridge_loop(HciTxBuff,sizeof(HciTxBuff),HciRxBuff,&HciRxBuffLen);
    BTRF_TRACE("HciRxBuffLen %d\n",HciRxBuffLen);
    BTRF_DUMP("%02x ",HciRxBuff,HciRxBuffLen);
    length = HciRxBuff[8]*0x100+HciRxBuff[7];
    head_error = HciRxBuff[10]*0x100+HciRxBuff[9];
    payload_error = HciRxBuff[12]*0x100+HciRxBuff[11];
    adv_estsw = HciRxBuff[14]*0x100+HciRxBuff[13];
    adv_esttpl = HciRxBuff[16]*0x100+HciRxBuff[15];
    payload_bit_error = HciRxBuff[20]*0x1000000+HciRxBuff[19]*0x10000+HciRxBuff[18]*0x100+HciRxBuff[17];

    if(reg_addr){
        memcpy((uint8_t *)reg_addr,&HciRxBuff[7],6);
    }

    BTRF_TRACE("success:%d receive packet num length:%d \n",ret,length);
    BTRF_TRACE("head_error:%d \n",head_error);
    BTRF_TRACE("payload_error:%d \n",payload_error);
    BTRF_TRACE("adv_estsw:%d \n",adv_estsw);
    BTRF_TRACE("adv_esttpl:%d \n",adv_esttpl);
    BTRF_TRACE("payload_bit_error:%d \n",payload_bit_error);

    return length;
}

static int cmd_bt_le_transmitter_test(int argc, char *argv[])
{
    uint8_t length = 0,ret = 0;

    BTRF_TRACE("%s ...\n",__func__);

    HciBuff[0] = 0x01;
    HciBuff[1] = 0x34;
    HciBuff[2] = 0x20;
    HciBuff[3] = 0x04;

    nosignal_set_max_power_level();

    dataBuff[0] = bt_atoi(argv[1]);
    dataBuff[1] = bt_atoi(argv[2]);
    dataBuff[2] = payload2index_ble(argv[3]);

    HciBuff[4] = dataBuff[0];
    HciBuff[5] = dataBuff[1];
    HciBuff[6] = dataBuff[2];
    HciBuff[7] = bt_atoi(argv[4]);

    btdrv_hci_bridge_loop(HciBuff,8,HciRxBuff,&HciRxBuffLen);
    BTRF_TRACE("HciRxBuffLen %d\n",HciRxBuffLen);
    BTRF_DUMP("%02x ",HciRxBuff,HciRxBuffLen);
    BTRF_TRACE("success:%d length:%d param:\n",ret,length);

    return 0;
}

static int cmd_bt_le_receiver_test(int argc, char *argv[])
{
    uint8_t length = 0,ret = 0;

    BTRF_TRACE("%s ...\n",__func__);

    HciBuff[0] = 0x01;
    HciBuff[1] = 0x33;
    HciBuff[2] = 0x20;
    HciBuff[3] = 0x03;
    HciBuff[4] = bt_atoi(argv[1]);
    HciBuff[5] = bt_atoi(argv[2]);
    HciBuff[6] = 0x00;

    btdrv_hci_bridge_loop(HciBuff,7,HciRxBuff,&HciRxBuffLen);

    BTRF_TRACE("HciRxBuffLen %d\n",HciRxBuffLen);
    BTRF_DUMP("%02x ",HciRxBuff,HciRxBuffLen);
    BTRF_TRACE("success:%d length:%d param:\n",ret,length);

    return 0;
}

static int cmd_bt_le_test_end(int argc, char *argv[])
{
    uint16_t length = 0,ret = 0;

    BTRF_TRACE("%s ...\n",__func__);

    HciBuff[0] = 0x01;
    HciBuff[1] = 0x1F;
    HciBuff[2] = 0x20;
    HciBuff[3] = 0x00;

    nosignal_clear_max_power_level();

    btdrv_hci_bridge_loop(HciBuff,4,HciRxBuff,&HciRxBuffLen);

    BTRF_TRACE("HciRxBuffLen %d\n",HciRxBuffLen);
    BTRF_DUMP("%02x ",HciRxBuff,HciRxBuffLen);
    length = HciRxBuff[8]*0x100 + HciRxBuff[7];
    BTRF_TRACE("success:%d length:%d \n",ret,length);

    return length;
}

static int cmd_bt_set_bdr_power(int argc, char *argv[])
{
    uint8_t length = 1,ret = 0;

    for(int i=0;i<2;i++)
    {
        hexString2CharBuf(&argv[i+1][2],&dataBuff[i],1);
    }

    btdrv_set_bdr_ble_txpower(dataBuff[0],dataBuff[1]);
    BTRF_TRACE("success:%d length:%d param:\n",ret,length);
    BTRF_DUMP("%02x ",dataBuff,length);

    return 0;
}

static int cmd_bt_set_edr_power(int argc, char *argv[])
{
    uint8_t length = 1,ret = 0;

    for(int i=0;i<2;i++)
    {
        hexString2CharBuf(&argv[i+1][2],&dataBuff[i],1);
    }
    uint32_t power = btdrv_set_edr_txpower(dataBuff[0],dataBuff[1]);
    factory_section_set_bt_tx_power(2, dataBuff[0], power);
    BTRF_TRACE("success:%d length:%d param:\n",ret,length);
    BTRF_DUMP("%02x ",dataBuff,length);

    return 0;
}

static int cmd_bt_set_mac(int argc, char *argv[])
{
    uint8_t length = 0,ret = 0;

    HciBuff[0] = 0x01;
    HciBuff[1] = 0x72;
    HciBuff[2] = 0xFC;
    HciBuff[3] = 0x07;
    HciBuff[4] = 0x00;

    for(int i=0,j=0;j< 6;i+=3,j++)
    {
        hexString2CharBuf(&argv[1][i],&HciBuff[5+j],1);
    }

    btdrv_hci_bridge_loop(HciBuff,11,HciRxBuff,&HciRxBuffLen);
    BTRF_TRACE("HciRxBuffLen %d\n",HciRxBuffLen);
    BTRF_DUMP("%02x ",HciRxBuff,HciRxBuffLen);
    BTRF_TRACE("success:%d length:%d param:\n",ret,length);

    return 0;
}

static int cmd_bt_read_mac(int argc, char *argv[])
{
    uint8_t length = 6,ret = 0;
    uint32_t reg_addr = 0;
    memset(dataBuff,0,sizeof(dataBuff));
    if(argc > 1 && strlen(argv[1])>=8)
    {
        hexString2CharBuf(argv[1],dataBuff,4);
        reg_addr = dataBuff[0]*0x1000000 + dataBuff[1]*0x10000 + dataBuff[2]*0x100 + dataBuff[3];
    }

    HciBuff[0] = 0x01;
    HciBuff[1] = 0x09;
    HciBuff[2] = 0x10;
    HciBuff[3] = 0x00;
    btdrv_hci_bridge_loop(HciBuff,4,HciRxBuff,&HciRxBuffLen);
    BTRF_TRACE("HciRxBuffLen %d\n",HciRxBuffLen);
    BTRF_DUMP("%02x ",HciRxBuff,HciRxBuffLen);
    BTRF_TRACE("success:%d length:%d param:\n",ret,length);
    BTRF_DUMP("%02x ",&HciRxBuff[7],length);

    if(reg_addr){
        memcpy((uint8_t *)reg_addr,&HciRxBuff[7],6);
    }

    return 0;
}

static int cmd_bt_hci_reset(int argc, char *argv[])
{
    uint8_t length = 0,ret = 0;

    HciBuff[0] = 0x01;
    HciBuff[1] = 0x03;
    HciBuff[2] = 0x0C;
    HciBuff[3] = 0x00;
    btdrv_hci_bridge_loop(HciBuff,4,HciRxBuff,&HciRxBuffLen);
    BTRF_TRACE("HciRxBuffLen %d\n",HciRxBuffLen);
    BTRF_DUMP("%02x ",HciRxBuff,HciRxBuffLen);
    BTRF_TRACE("success:%d length:%d param:\n",ret,length);

    return 0;
}

static int cmd_bt_carrier_start(int argc, char *argv[])
{
    uint8_t length = 0,ret = 0;

    BTRF_TRACE("%s ...",__func__);

    cmd_bt_hci_reset(0,NULL);

    HciBuff[0] = bt_atoi(argv[1]);

    btdrv_vco_test_start(HciBuff[0]);
    BTRF_TRACE("success:%d length:%d param:\n",ret,length);

    return 0;
}

static int cmd_bt_carrier_stop(int argc, char *argv[])
{
    uint8_t length = 0,ret = 0;

    BTRF_TRACE("%s ...\n",__func__);

    cmd_bt_hci_reset(0,NULL);
    btdrv_vco_test_stop();
    BTRF_TRACE("success:%d length:%d param:\n",ret,length);

    return 0;
}

static void bt_error_check_timer_handler(void const *param);
osTimerDef(bt_error_check_timer, bt_error_check_timer_handler);
static osTimerId bt_error_check_timer_id = NULL;
static void bt_error_check_timer_handler(void const *param)
{
    //dump rssi
    bt_drv_rssi_dump_handler();
    //adjust parameter for testmode
    bt_drv_testmode_adaptive_adjust_param();
}

static int cmd_bt_enter_signal_mode(int argc, char *argv[])
{
    cmd_bt_hci_reset(0,NULL);
#ifdef __USE_NV_BT_ADDR_TEST__
    uint8_t *btaddr = factory_section_get_bt_address();
     if (btaddr) {
        BTRF_TRACE("%s OK_BES_RF: get btmac from factory: " MACSTR "\n", __func__, MAC2STR(btaddr));
        bt_rf_set_dut_addr(btaddr);
    }
#endif

    btdrv_hci_bridge_loop(feature,sizeof(feature),HciRxBuff,&HciRxBuffLen);
    BTRF_TRACE("HciRxBuffLen %d\n",HciRxBuffLen);
    BTRF_DUMP("%02x ",HciRxBuff,HciRxBuffLen);

    btdrv_hci_bridge_loop(hci_cmd_enable_dut,sizeof(hci_cmd_enable_dut),HciRxBuff,&HciRxBuffLen);
    BTRF_TRACE("HciRxBuffLen %d\n",HciRxBuffLen);
    BTRF_DUMP("%02x ",HciRxBuff,HciRxBuffLen);

    btdrv_hci_bridge_loop(hci_cmd_enable_allscan,sizeof(hci_cmd_enable_allscan),HciRxBuff,&HciRxBuffLen);
    BTRF_TRACE("HciRxBuffLen %d\n",HciRxBuffLen);
    BTRF_DUMP("%02x ",HciRxBuff,HciRxBuffLen);

    btdrv_hci_bridge_loop(hci_cmd_autoaccept_connect,sizeof(hci_cmd_autoaccept_connect),HciRxBuff,&HciRxBuffLen);
    BTRF_TRACE("HciRxBuffLen %d\n",HciRxBuffLen);
    BTRF_DUMP("%02x ",HciRxBuff,HciRxBuffLen);

    BTRF_TRACE("enter signal test mode\n");

    if (!bt_error_check_timer_id){
        bt_error_check_timer_id = osTimerCreate(osTimer(bt_error_check_timer), osTimerPeriodic, NULL);
    }
    if (bt_error_check_timer_id != NULL) {
        osTimerStart(bt_error_check_timer_id, 1000);
    }

    return 0;
}

static int cmd_bt_exit_signal_mode(int argc, char *argv[])
{
    cmd_bt_hci_reset(0,NULL);

    if (bt_error_check_timer_id != NULL) {
        osTimerStop(bt_error_check_timer_id);
    }

    BTRF_TRACE("exit signal test mode\n");

    return 0;
}

static int cmd_read_bdr_power(int argc, char *argv[])
{
    return btdrv_rf_get_bdr_power();
}

static int cmd_read_edr_power(int argc, char *argv[])
{
    return btdrv_rf_get_edr_power();
}

static int cmd_read_ble_power(int argc, char *argv[])
{
    return btdrv_rf_get_ble_power();
}

static int cmd_bt_scan_control(int argc, char *argv[])
{
    for(int i=0;i< 1;i++)
    {
        hexString2CharBuf(&argv[i+1][2],&HciBuff[4],1);
    }

    if(HciBuff[4] == 0x03)
        BTRF_TRACE("open bt scan \n");
    else if(HciBuff[4] == 0x00)
        BTRF_TRACE("close bt scan \n");

    HciBuff[0] = 0x01;
    HciBuff[1] = 0x1a;
    HciBuff[2] = 0x0c;
    HciBuff[3] = 0x01;

    btdrv_hci_bridge_loop(HciBuff,5,HciRxBuff,&HciRxBuffLen);

    BTRF_TRACE("HciRxBuffLen %d\n",HciRxBuffLen);
    BTRF_DUMP("%02x ",HciRxBuff,HciRxBuffLen);

    return 0;
}

static int cmd_ble_nosignal_start(int argc, char *argv[])
{
    ble_nosignal_start();
    ble_nosignal_start_flag = 1;

    if (!bt_error_check_timer_id){
        bt_error_check_timer_id = osTimerCreate(osTimer(bt_error_check_timer), osTimerPeriodic, NULL);
    }
    if (bt_error_check_timer_id != NULL) {
        osTimerStart(bt_error_check_timer_id, 1000);
    }

    BTRF_TRACE("enter ble nosignal test mode\n");

    return 0;
}

static int cmd_ble_nosignal_stop(int argc, char *argv[])
{
    ble_nosignal_start_flag = 0;
    ble_nosignal_close();

    if (bt_error_check_timer_id != NULL) {
        osTimerStop(bt_error_check_timer_id);
    }

    BTRF_TRACE("eixt ble nosignal test mode\n");

    return 0;
}

#define BEL_OVERSEAS_POWER_CONFIG   0x12 //010010
#define BLE_DOMESTIC_POWER_CONFIG   0x14 //010100
static int cmd_ble_power_control_overseas(int argc, char *argv[])
{
    int ret = 0;
    ret = factory_section_set_ble_power_control(BEL_OVERSEAS_POWER_CONFIG);
    return ret;
}

static int cmd_ble_power_control_domestic(int argc, char *argv[])
{
    int ret = 0;
    ret = factory_section_set_ble_power_control(BLE_DOMESTIC_POWER_CONFIG);
    return ret;
}

static int cmd_get_ble_power_control_type(int argc, char *argv[])
{
    int ret = 0;
    ret = factory_section_get_ble_power_control();
    TRACE(0, "####ble power control = %04x", ret);
    return ret;
}

static u8 charTodata(const char ch)
{
    switch(ch)
    {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    case 'a':
    case 'A': return 10;
    case 'b':
    case 'B': return 11;
    case 'c':
    case 'C': return 12;
    case 'd':
    case 'D': return 13;
    case 'e':
    case 'E': return 14;
    case 'f':
    case 'F': return 15;
    }
    return 0;
}

static void str2mac(const char * szMac,uint8_t * pMac)
{
    const char * pTemp = szMac;
    for (int i = 0;i < 6;++i)
    {
        pMac[i] = charTodata(*pTemp++) * 16;
        pMac[i] += charTodata(*pTemp++);
        pTemp++;
    }
}

static int cmd_set_ble_addr(int argc, char* argv[])
{
    uint8_t ble_addr[6] = {0};
    if (strlen(argv[1]) == 17) {
        str2mac(argv[1], ble_addr);
        TRACE(0,"success: the mac format parse right");
	for (int i = 0; i < 6; i++) {
            TRACE(0, "buf[%d] = 0x%02X", i, ble_addr[i]);
        }
    } else {
        TRACE(0,"fail:input address format error");
        return -1;
    }
    factory_section_set_ble_address(ble_addr);
    return 0;
}

static int cmd_get_ble_addr(int argc, char* argv[])
{
    uint8_t* addr = factory_section_get_ble_address();
    if (addr) {
        TRACE(0, "success:get address %02x:%02x:%02x:%02x:%02x:%02x, success", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
    } else {
        TRACE(0, "fail: get address failed");
    }
    return 0;
}

static int cmd_set_bt_addr(int argc, char* argv[])
{
    uint8_t bt_addr[6] = {0};
    if (strlen(argv[1]) == 17) {
        str2mac(argv[1], bt_addr);
        TRACE(0,"success: the mac format parse right");
	for (int i = 0; i < 6; i++) {
            TRACE(0, "buf[%d] = 0x%02X\n", i, bt_addr[i]);
        }
    } else {
        TRACE(0,"fail:input address format error");
        return -1;
    }
    factory_section_set_bt_address(bt_addr);
    return 0;
}

static int cmd_get_bt_addr(int argc, char* argv[])
{
    uint8_t* addr = factory_section_get_bt_address();
    if (addr) {
        TRACE(0, "success:get address %02x:%02x:%02x:%02x:%02x:%02x, success", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
    } else {
        TRACE(0, "fail: get address failed");
    }
    return 0;
}

static int cmd_set_wifi_addr(int argc, char* argv[])
{
    uint8_t wifi_addr[6] = {0};
    if (strlen(argv[1]) == 17) {
        str2mac(argv[1], wifi_addr);
        TRACE(0,"success: the mac format parse right");
	for (int i = 0; i < 6; i++) {
            TRACE(0, "buf[%d] = 0x%02X\n", i, wifi_addr[i]);
        }
    } else {
        TRACE(0,"fail:input address format error");
        return -1;
    }
    factory_section_set_wifi_address(wifi_addr);
    return 0;
}

static int cmd_get_wifi_addr(int argc, char* argv[])
{
    uint8_t* addr = factory_section_get_bt_address();
    if (addr) {
        TRACE(0, "success:get address %02x:%02x:%02x:%02x:%02x:%02x, success", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
    } else {
        TRACE(0, "fail: get address failed");
    }
    return 0;
}

static const struct bt_test_cmd bt_test_commands[] = {
    {"bt_mp_start", cmd_bt_mp_start},
    {"bt_mp_stop", cmd_bt_mp_stop},
    {"bt_hci_reset", cmd_bt_hci_reset},
    {"bt_bdr_tx_signal", cmd_bt_bdr_tx_signal},
    {"bt_set_bdr_power", cmd_bt_set_bdr_power},
    {"bt_edr_tx_signal", cmd_bt_edr_tx_signal},
    {"bt_set_edr_power", cmd_bt_set_edr_power},
    {"bt_bdr_rx_signal", cmd_bt_bdr_rx_signal},
    {"bt_stop_rx_signal", cmd_bt_stop_rx_signal},
    {"bt_edr_rx_signal", cmd_bt_edr_rx_signal},
    {"bt_stop_tx_signal", cmd_bt_stop_tx_signal},
    {"bt_set_mac", cmd_bt_set_mac},
    {"bt_read_mac", cmd_bt_read_mac},
    {"bt_carrier_start", cmd_bt_carrier_start},
    {"bt_carrier_stop", cmd_bt_carrier_stop},
    {"read_bdr_power", cmd_read_bdr_power},
    {"read_edr_power", cmd_read_edr_power},
    {"bt_scan", cmd_bt_scan_control},
    {"bt_enter_signal_mode", cmd_bt_enter_signal_mode},
    {"bt_exit_signal_mode", cmd_bt_exit_signal_mode},

    {"ble_nosignal_start", cmd_ble_nosignal_start},
    {"ble_nosignal_stop", cmd_ble_nosignal_stop},
    {"read_ble_power", cmd_read_ble_power},
    {"ble_overseas", cmd_ble_power_control_overseas},
    {"ble_domestic", cmd_ble_power_control_domestic},
    {"get_ble_ctry_type", cmd_get_ble_power_control_type},
    {"bt_le_transmitter_test", cmd_bt_le_transmitter_test},
    {"bt_le_receiver_test", cmd_bt_le_receiver_test},
    {"bt_le_test_end", cmd_bt_le_test_end},
    {"bt_set_addr", cmd_set_bt_addr},
    {"bt_get_addr", cmd_get_bt_addr},
    {"ble_set_addr", cmd_set_ble_addr},
    {"ble_get_addr", cmd_get_ble_addr},
    {"wifi_set_addr", cmd_set_wifi_addr},
    {"wifi_get_addr", cmd_get_wifi_addr},
};

static void do_bt_rf_test(int argc, char *argv[])
{
    eshell_putstring("enter %s cmd \r\n", __func__);
    if(argc < 2) {
        eshell_putstring("too few argument!!!\r\n");
        return;
    }

    for (int i = 0;i < sizeof(bt_test_commands) / sizeof(struct bt_test_cmd); i++) {
        if (!memcmp(argv[1], bt_test_commands[i].cmd, strlen(bt_test_commands[i].cmd))) {
            eshell_putstring("enter %s cmd argc:%d\r\n", bt_test_commands[i].cmd, argc);
            bt_test_commands[i].handler(argc - 1, &argv[1]);
            break;
        }
    }
}

ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_BT_COMMON, "btrf", "start bt rf test", do_bt_rf_test);
#endif // UTILS_ESHELL_EN
