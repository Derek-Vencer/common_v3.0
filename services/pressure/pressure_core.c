/***************************************************************************
 * Copyright 2024-2024 BES.
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
 ***************************************************************************/
#include "cmsis.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif

#include "hal_trace.h"
#include "pressure_core.h"
#include "pressure_driver.h"
#include CHIP_SPECIFIC_HDR(pressure_driver)
#include "pmu.h"
#include "press_detection.h"
#include "hal_timer.h"
#include "stdint.h"
#include "stdbool.h"

#define WAIT_VCM_STABILIZE      2000
#define PRESSURE_RAW_DATA       10
#define PRESSURE_MIN_INTERVAL   1

enum PRESSURE_MSG {
    PRESSURE_MSG_INIT     = 1,
    PRESSURE_MSG_REINIT   = 2,
    PRESSURE_MSG_RESET    = 3,
    PRESSURE_MSG_READ     = 4,
    PRESSURE_MSG_POLLING  = 5,
};

// Global variables
#define PRESSURE_MAILBOX_MAX (10)
typedef struct {
    uint32_t src_thread;
    uint32_t system_time;
    uint32_t message;
    uint32_t parms1;
    uint32_t parms2;
} PRESSURE_MAIL;

osMailQDef (pressure_mailbox, PRESSURE_MAILBOX_MAX, PRESSURE_MAIL);
static osMailQId pressure_mailbox = NULL;
static uint8_t pressure_mailbox_cnt = 0;

/***************************************************************************
****************************************************************************
***       read_time                                        read_time     ***
***     ____________                                     ____________    ***
***     |          |                                     |          |    ***
***     |          |                                     |          |    ***
***     |          |                                     |          |    ***
***_____|          |_____________________________________|          |____***
***                |<--------------wait_time------------>|               ***
***     |<<<--------------interval_time--------------->>>|               ***
****************************************************************************
***************************************************************************/
static uint8_t pressure_polling_wait_time = 90;     //wait = interval - read (ms)
static uint8_t pressure_interval_freq = 100;        //ms

static void pressure_thread(const void *arg);
osThreadDef(pressure_thread, osPriorityHigh, 1, (1024 * 2), "pressure_thread");
static osThreadId pressure_thread_id = NULL;

static struct pressure_sample_data pressure_sample_buff[PRESSURE_RAW_DATA] = {0};

static int32_t pressure_data[PRESSURE_RAW_DATA] = {0};

static inline int pressure_mailbox_put(PRESSURE_MAIL* msg_src);
static inline int pressure_mailbox_free(PRESSURE_MAIL* mail_p);
static inline int pressure_mailbox_get(PRESSURE_MAIL** mail_p);

/***************************************************************************
 * @brief pressure interrupt handler
 *
 ***************************************************************************/
static void pressure_sensor_irq_handler(void)
{
    // PRESSURE_TRACE(0, "%s", __func__);

    int ret = pressure_sensor_irq_get_active();
    if(!ret){
        pressure_sensor_irq_clr();
        pressure_sensor_irq_enable(false); //disbale irq
        pressure_read_curr_data(pressure_sample_buff, PRESSURE_RAW_DATA);
        osSignalSet(pressure_thread_id, 0x0001);
    }
}

/***************************************************************************
 * @brief pressure interrupt init function
 *
 ***************************************************************************/
void pressure_sensor_irq_init(void)
{
    int ret = -1;
    ret = pmu_pressure_set_irq_handler(pressure_sensor_irq_handler);
    if (ret) {
        PRESSURE_TRACE(1, "pmu_pressure_set_irq_handler failed:%d\n", ret);
    }
    pressure_sensor_irq_enable(true); //enable irq
}

/***************************************************************************
 * @brief pressure wait vcm to stabilize function
 *
 ***************************************************************************/
void pressure_wait_vcm_to_stabilize(void)
{
    osDelay(WAIT_VCM_STABILIZE); // Wait for Vcm to stabilize
}

static void pressure_set_wait_time(uint8_t wait_time)
{
    pressure_polling_wait_time = wait_time;
}

static void pressure_process(PRESSURE_MAIL* mail_p)
{
    PRESSURE_MAIL msg;
    uint32_t active_time1, active_time2;

    memset(&msg, 0, sizeof(PRESSURE_MAIL));
    // PRESSURE_TRACE(0, "%s %d !!!", __func__, mail_p->message);

    switch (mail_p->message) {
        case PRESSURE_MSG_INIT:
            pressure_wait_vcm_to_stabilize();
            pressure_drv_start();
            break;
        case PRESSURE_MSG_REINIT:
            break;
        case PRESSURE_MSG_RESET:
            break;
        case PRESSURE_MSG_READ:
            break;
        case PRESSURE_MSG_POLLING:
            active_time1 = hal_fast_sys_timer_get();
            pressure_read_curr_data(pressure_sample_buff, PRESSURE_RAW_DATA);
            active_time2 = hal_fast_sys_timer_get();
            uint32_t read_time = FAST_TICKS_TO_MS(active_time2 - active_time1);

            if(read_time < pressure_interval_freq) {
                pressure_set_wait_time(pressure_interval_freq - read_time);
            } else {
                pressure_set_wait_time(PRESSURE_MIN_INTERVAL); //min interval
            }

            for (int i = 0; i < PRESSURE_RAW_DATA; i++) {
                pressure_data[i] = pressure_sample_buff[i].value;
            }
            for (int16_t i = 0; i < PRESS_USED_TOUCH_NUM; i++) {
                key_recognition_entrance(pressure_data, i);
            }
            break;
        default:
            break;
    }
}

static inline int pressure_mailbox_put(PRESSURE_MAIL* msg_src)
{
    osStatus status;

    PRESSURE_MAIL *mail_p = NULL;

    if (msg_src == NULL)
    {
        return -1;
    }

    mail_p = (PRESSURE_MAIL*)osMailAlloc(pressure_mailbox, 0);
    if (!mail_p){
        osEvent evt;
        PRESSURE_TRACE(0,"pressure_mailbox");
        for (uint8_t i=0; i<PRESSURE_MAILBOX_MAX; i++){
            evt = osMailGet(pressure_mailbox, 0);
            if (evt.status == osEventMail) {
                PRESSURE_TRACE(4,"msg cnt:%d msg:%d parms:%08x/%08x", i,
                                                              ((PRESSURE_MAIL *)(evt.value.p))->message,
                                                              ((PRESSURE_MAIL *)(evt.value.p))->parms1,
                                                              ((PRESSURE_MAIL *)(evt.value.p))->parms2);
            }else{
                break;
            }
        }
        ASSERT(mail_p, "pressure_mailbox error");
    }
    mail_p->src_thread = (uint32_t)osThreadGetId();
    mail_p->system_time = hal_sys_timer_get();
    mail_p->message = msg_src->message;
    mail_p->parms1 = msg_src->parms1;
    mail_p->parms2 = msg_src->parms2;
    status = osMailPut(pressure_mailbox, mail_p);
    if (osOK == status){
        pressure_mailbox_cnt++;
    }

    return (int)status;
}

static inline int pressure_mailbox_free(PRESSURE_MAIL* mail_p)
{
    osStatus status;

    status = osMailFree(pressure_mailbox, mail_p);
    if (osOK == status){
        pressure_mailbox_cnt--;
    }
    return (int)status;
}

static inline int pressure_mailbox_get(PRESSURE_MAIL** mail_p)
{
    osEvent evt;
    evt = osMailGet(pressure_mailbox, pressure_polling_wait_time); //osWaitForever
    if (evt.status == osEventMail) {
        *mail_p = (PRESSURE_MAIL *)evt.value.p;
    } else if (evt.status == osEventTimeout) {
        *mail_p = (PRESSURE_MAIL*)osMailAlloc(pressure_mailbox, 0);
        (*mail_p)->message = PRESSURE_MSG_POLLING;
    }
    return 0;
}

/***************************************************************************
 * @brief pressure process thread
 *
 * @param arg
 ***************************************************************************/
static void pressure_thread(const void *arg)
{
    PRESSURE_MAIL msg;

    memset(&msg, 0, sizeof(PRESSURE_MAIL));
    msg.message = PRESSURE_MSG_INIT;
    pressure_mailbox_put(&msg);

    while(1) {
        PRESSURE_MAIL *mail_p = NULL;
        if (!pressure_mailbox_get(&mail_p)){
            pressure_process(mail_p);
            pressure_mailbox_free(mail_p);
        }
    }
}

/***************************************************************************
 * @brief init pressure touch key functions.
 *
 ***************************************************************************/
void pressure_core_thread_init(void)
{
    PRESSURE_TRACE(1,"%s", __func__);
    if (pressure_mailbox == NULL){
        pressure_mailbox = osMailCreate(osMailQ(pressure_mailbox), NULL);
    }

    if (pressure_thread_id == NULL) {
        pressure_thread_id = osThreadCreate(osThread(pressure_thread), NULL);
    }
}
