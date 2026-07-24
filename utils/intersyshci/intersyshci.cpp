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
#include <stdio.h>
#include "cmsis.h"
#include "cmsis_os.h"
#include "string.h"
#include "besbt.h"
#include "app_utils.h"
#include "hal_intersys.h"
#include "hal_dma.h"
#include "intersyshci.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_sleep.h"
#include "beslib_info.h"

//#define INTERSYS_RX_DMA
//#define INTERSYS_RX_DMA_TRACE

#define INTERSYS_TX_SIGNAL (0x010000)
#define INTERSYS_CHECK_TIMEOUT 15000
#define HCI_TX_TIMEOUT (5000)
#define HCI_TRC_TYPE   0x06

struct HAL_DMA_CH_CFG_T rx_dma_cfg =
{
    .ch = HAL_DMA_CHAN_NONE,
};

#ifdef INTERSYS_RX_DMA
#define INTERSYS_DMA_BUF_SIZE               1100
#define INTERSYS_DMA_RETRY_CNT              50
static uint8_t rx_dma_buf[INTERSYS_DMA_BUF_SIZE];
STATIC_ASSERT(sizeof(rx_dma_buf) <= HAL_DMA_MAX_DESC_XFER_SIZE, "rx_dma_buf too large");
static const unsigned char *rx_dma_src;
static unsigned int rx_dma_len;
static unsigned char rx_dma_retry;
#ifdef INTERSYS_RX_DMA_TRACE
static uint32_t rx_dma_time;
#endif
#endif

#define HCI_COMMAND_DATA_PACKET 0x01
#define HCI_ACL_DATA_PACKET     0x02
#define HCI_SCO_DATA_PACKET     0x03
#define HCI_EVENT_PACKET        0x04
#define HCI_ISO_DATA_PACKET     0x05
#define HCI_DEBUG_TRACE_PACKET  0x06

#ifndef   FPGA
#define INTERSYSHCI_LOC __attribute__((section(".fast_text_sram")))
#endif

static osThreadId intersys_tx_thread_id;
static osMutexId intersys_tx_mutex_id;
osMutexDef(intersys_tx_mutex);

static uint32_t intersts_tx_systime = 0;
static enum HAL_INTERSYS_ID_T bt_intersys_id = HAL_INTERSYS_ID_0;

static bool isIntersysSleepCheckerEnable = false;

void beshci_enable_sleep_checker(bool isEnable)
{
    isIntersysSleepCheckerEnable = isEnable;
}

void intersys_sleep_checker_ping(void)
{
    if (isIntersysSleepCheckerEnable)
    {
        hal_sys_wake_lock(HAL_SYS_WAKE_LOCK_USER_INTERSYS_HCI);
        intersts_tx_systime = hal_sys_timer_get();
    }
}

void intersys_sleep_checker(void)
{
    if (isIntersysSleepCheckerEnable)
    {
        if(intersts_tx_systime != 0 && ((hal_sys_timer_get() - intersts_tx_systime)> MS_TO_TICKS(INTERSYS_CHECK_TIMEOUT)))
        {
            hal_sys_wake_unlock(HAL_SYS_WAKE_LOCK_USER_INTERSYS_HCI);
            intersts_tx_systime = 0;
        }
    }
}

static void (*g_intersys_hci_rx_data_isr)(const uint8_t *data, uint16_t len);

void bes_hci_bt_rx_isr(void (*cb)(const uint8_t *data, uint16_t len))
{
    g_intersys_hci_rx_data_isr = cb;
}

static void intersys_hci_rx_data_isr(const uint8_t *data, uint16_t len)
{
    if (g_intersys_hci_rx_data_isr)
    {
        g_intersys_hci_rx_data_isr(data, len);
    }
}

static unsigned int INTERSYSHCI_LOC bt_rx(const unsigned char *data, unsigned int len)
{
    if ((data[0] == 0x04) && (data[1] == 0xff) && (data[3] == 0x01))
    {
        if (len > 5)
        {
#if defined(CHIP_BEST2300) || defined(CHIP_BEST2300P) || defined(CHIP_BEST2300A) || defined(CHIP_BEST1400) || defined(CHIP_BEST1402) || defined(CHIP_BEST2001) || defined(CHIP_BEST1305) || defined(CHIP_BEST2002) || defined(CHIP_BEST2003) || defined(CHIP_BEST1501)
            NORM_LOG_RAW_OUTPUT(&data[4], len-4);
#else
            NORM_LOG_RAW_OUTPUT(&data[5], len-5);
#endif
        }

        return len;
    }

    if (data[0] == HCI_TRC_TYPE)
    {
#ifndef IS_BLE_AUDIO_DEBUG_INFO_COLLECTOR_ENABLED
        return len;
#endif
    }

    //reserve for extern vendor event process
    if (data[0] == 0x4 && data[1] == 0xfe)
    {
        len = *(uint16_t *)(data + 2) + 4;
        return len;
    }

    intersys_hci_rx_data_isr(data, len);

    return len;
}

/***************************************************************************
 *
 * using DMA to transfer intersys RX data
 *
 ****************************************************************************/
#ifdef INTERSYS_RX_DMA
static unsigned int INTERSYSHCI_LOC intersys_start_dma_rx(const unsigned char *data, unsigned int len, unsigned char *dst)
{
    enum HAL_DMA_RET_T dma_ret;

    rx_dma_cfg.dst = (uint32_t)dst;
    rx_dma_cfg.src = (uint32_t)data;
    rx_dma_cfg.src_tsize = len;

    dma_ret = hal_dma_start(&rx_dma_cfg);
    ASSERT(dma_ret == HAL_DMA_OK, "%s: Failed to start dma: %d", __func__, dma_ret);

    return len;
}

static void rxdma_irq_handler(uint8_t ch, uint32_t remain_dst_tsize, uint32_t error, struct HAL_DMA_DESC_T *lli)
{
    if (remain_dst_tsize == 0)
    {
#ifdef INTERSYS_RX_DMA_TRACE
        INTERSYSHCI_TRACE(2,"intersys rx_dma consumes %u us len=%u", FAST_TICKS_TO_US(hal_fast_sys_timer_get() - rx_dma_time), rx_dma_len);
#endif

        bt_rx(rx_dma_buf, rx_dma_len);
        rx_dma_src = NULL;
        hal_intersys_rx_done(bt_intersys_id);
    }
    else
    {
        unsigned int pos;

        rx_dma_retry++;
        ASSERT(rx_dma_retry < INTERSYS_DMA_RETRY_CNT, "%s: DMA retry %u times: remain=%u error=%u src=%p total=%u",
               __func__, rx_dma_retry, remain_dst_tsize, error, rx_dma_src, rx_dma_len);

        pos = rx_dma_len - remain_dst_tsize;
        intersys_start_dma_rx(rx_dma_src + pos, remain_dst_tsize, rx_dma_buf + pos);

        INTERSYSHCI_TRACE(5,"\n\nWARNING:%s: Restart dma: remain=%u error=%u src=%p total=%u\n\n", __func__, remain_dst_tsize, error, rx_dma_src, rx_dma_len);
    }
}

static unsigned int INTERSYSHCI_LOC intersys_dma_rx(const unsigned char *data, unsigned int len)
{
#ifdef INTERSYS_RX_DMA_TRACE
    rx_dma_time = hal_fast_sys_timer_get();
#endif

    ASSERT(rx_dma_src == NULL, "%s: DMA is busy", __func__);
    ASSERT(len > 0 && len <= sizeof(rx_dma_buf), "%s: Bad len=%u", __func__, len);

    rx_dma_src = data;
    rx_dma_len = len;
    rx_dma_retry = 0;

    intersys_start_dma_rx(data, len, rx_dma_buf);

    return len;
}
#endif

/***************************************************************************
 *
 * BES intersys TX IRQ callback
 *
 ****************************************************************************/
static void bt_tx(const unsigned char *data, unsigned int len)
{
    osSignalSet(intersys_tx_thread_id, INTERSYS_TX_SIGNAL);
}

/***************************************************************************
 *
 * open BES intersys module
 *
 ****************************************************************************/

static int beshci_close_if = 0;

void BESHCI_Open(void)
{
    beshci_close_if = 0;
    int ret = 0;
    bool flowctrl;
    HAL_INTERSYS_RX_IRQ_HANDLER rx_hdlr;

#ifndef BT_BUILD_WITH_CUSTOMER_HOST
    Besbt_hook_handler_set(BESBT_HOOK_USER_0, intersys_sleep_checker);
#endif

    if (NULL == intersys_tx_thread_id)
    {
        intersys_tx_thread_id = osThreadGetId();
    }

    if (NULL == intersys_tx_mutex_id)
    {
        intersys_tx_mutex_id = osMutexCreate((osMutex(intersys_tx_mutex)));
    }

    INTERSYSHCI_TRACE(0, "intersys %s", BESLIB_INFO_STR);
    INTERSYSHCI_TRACE(1,"intersys_tx_thread_id %p\n", intersys_tx_thread_id);

    flowctrl = false;

#ifdef INTERSYS_RX_DMA
    rx_hdlr = intersys_dma_rx;
    flowctrl = true;
#else
    rx_hdlr = bt_rx;
#endif
    ret = hal_intersys_open(bt_intersys_id, HAL_INTERSYS_MSG_HCI, rx_hdlr, bt_tx, flowctrl);
    if (ret)
    {
        INTERSYSHCI_TRACE(0,"Failed to open intersys");
        return;
    }

#ifdef INTERSYS_RX_DMA
    memset(&rx_dma_cfg, 0, sizeof(rx_dma_cfg));

    rx_dma_cfg.ch = hal_dma_get_chan(HAL_GPDMA_MEM, HAL_DMA_LOW_PRIO);
    ASSERT(rx_dma_cfg.ch != HAL_DMA_CHAN_NONE, "%s: Failed to get dma chan", __func__);

    rx_dma_cfg.dst_bsize = HAL_DMA_BSIZE_16;
    rx_dma_cfg.dst_periph = HAL_GPDMA_MEM;
    rx_dma_cfg.dst_width = HAL_DMA_WIDTH_BYTE;
    rx_dma_cfg.handler = rxdma_irq_handler;
    rx_dma_cfg.src_bsize = HAL_DMA_BSIZE_16;
    rx_dma_cfg.src_periph = HAL_GPDMA_MEM;
    rx_dma_cfg.src_width = HAL_DMA_WIDTH_BYTE;
    rx_dma_cfg.type = HAL_DMA_FLOW_M2M_DMA;
    rx_dma_cfg.try_burst = false;
#endif

    hal_intersys_start_recv(bt_intersys_id);
}

/***************************************************************************
 *
 * close BES intersys module
 *
 ****************************************************************************/
void BESHCI_Close(void)
{
    beshci_close_if = 1;
    hal_intersys_close(bt_intersys_id,HAL_INTERSYS_MSG_HCI);

#ifdef INTERSYS_RX_DMA
    if (rx_dma_cfg.ch != HAL_DMA_CHAN_NONE)
    {
        hal_dma_cancel(rx_dma_cfg.ch);
        hal_dma_free_chan(rx_dma_cfg.ch);
        rx_dma_cfg.ch = HAL_DMA_CHAN_NONE;
    }
#endif
}

static void BES_INTERSYS_LOCK_TX(void)
{
    osMutexWait(intersys_tx_mutex_id, osWaitForever);
}

static void BES_INTERSYS_UNLOCK_TX(void)
{
    osMutexRelease(intersys_tx_mutex_id);
}

/***************************************************************************
 *
 * BES intersys send data API
 *
 ****************************************************************************/
int bes_hci_send_data(const uint8_t *buf, uint16_t size)
{
    osEvent evt;
    int ret = 0;

    if (beshci_close_if)
    {
        INTERSYSHCI_TRACE(0, "bes_hci_send_data hci closed");
        return ret;
    }

    BES_INTERSYS_LOCK_TX();
    intersys_tx_thread_id = osThreadGetId();

    intersys_sleep_checker_ping();
    osSignalClear(intersys_tx_thread_id, INTERSYS_TX_SIGNAL);
    ret = hal_intersys_send(bt_intersys_id, HAL_INTERSYS_MSG_HCI, buf, size);

    evt = osSignalWait(INTERSYS_TX_SIGNAL, HCI_TX_TIMEOUT);
    if (evt.status == osEventTimeout) {
        ASSERT(0, "%s osEventTimeout", __func__);
    }
    BES_INTERSYS_UNLOCK_TX();

    return ret;
}

