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
#if defined(UTILS_ESHELL_EN) && defined(USB_TEST_ENABLE)

#include "cmsis.h"
#include "cmsis_os.h"
#include "eshell.h"
#include "stdlib.h"
#include "string.h"
#include "hal_usb.h"
#include "usb_cdc.h"
#include "usb_host.h"
#include "hal_usbhost.h"
#include "usb_utils.h"
#include "hal_timer.h"
#ifdef MTP_ENABLE
#include "usb_mtp.h"
#include "mtp_storage.h"
#include "fs/fs.h"
#endif // MTP_ENABLE
#ifdef USBHOST_MASS_ENABLE
#include "fs/fs.h"
#include "usb_auto_event.h"
#endif // USBHOST_MASS_ENABLE

#define USB_SERIAL_ID             USB_CDC_ID_0

/**********************usb series start********************************/
static volatile int rx_done = 0;
static volatile int tx_done = 0;
static uint32_t recv_size = 0;
static uint8_t usb_serial_data[256];
static osThreadId_t usb_series_thread_id = NULL;
static void usb_series_thread(const void *arg);

static void usb_serial_break_handler(uint16_t ms);
static const struct USB_SERIAL_CFG_T cdc_cfg = {
    .break_callback = usb_serial_break_handler,
    .mode = USB_SERIAL_API_BLOCKING,
};

static void usb_direct_rx_callback(const void *data, uint32_t size, int error)
{
    rx_done = 1;
    recv_size = size;
    // TRACE(4, "%s: data=%p size=%u error=%d", __func__, data, size, error);
}

static void usb_direct_tx_callback(const void *data, uint32_t size, int error)
{
    tx_done = 1;
    // TRACE(4, "%s: data=%p size=%u error=%d", __func__, data, size, error);
}

void usb_series_thread(const void *arg)
{
    int result;
    while(1) {
        rx_done = 0;
        result = usb_serial_direct_recv(USB_SERIAL_ID, usb_serial_data, ARRAY_SIZE(usb_serial_data),
                                    0, NULL, usb_direct_rx_callback);
        if (result != 0) {
            eshell_putstring("Failed to receive from usb serial: ret=%d\r\n", result);
            break;
        }
        while (rx_done == 0);

        tx_done = 0;
        int tx_size = MIN(ARRAY_SIZE(usb_serial_data), recv_size);
        result = usb_serial_direct_send(USB_SERIAL_ID, usb_serial_data, tx_size, usb_direct_tx_callback);
        if (result != 0) {
            eshell_putstring("Failed to send to usb serial: ret=%d\r\n", result);
            break;
        }
        while (tx_done == 0);
    }
}

static void usb_serial_break_handler(uint16_t ms)
{
    eshell_putstring("Detect break: %u ms\r\n", ms);
}

static void usb_serial_test(uint32_t enable)
{
    eshell_putstring("usb_serial_test\r\n");
    int ret = 0;
    osThreadAttr_t attr;
    if (enable == 1) {
        ret += usb_serial_open(USB_SERIAL_ID, &cdc_cfg);
        ret += usb_serial_flush_recv_buffer(USB_SERIAL_ID);
        if (ret != 0) {
            eshell_putstring("usb_serial enable fail\r\n");
            usb_serial_close(USB_SERIAL_ID);
            return;
        }

        usb_serial_init_xfer(USB_SERIAL_ID, USB_SERIAL_XFER_TYPE_ALL);
        if (usb_series_thread_id == NULL) {
            memset(&attr, 0, sizeof(osThreadAttr_t));
            attr.name = "usb_series_thread";
            attr.attr_bits = osThreadDetached;
            attr.stack_size = 1024;
            attr.priority = osPriorityHigh;
            usb_series_thread_id =
                osThreadNew((osThreadFunc_t)usb_series_thread, NULL, &attr);
            if (usb_series_thread_id == NULL) {
                eshell_putstring("usb_serial enable fail\r\n");
                usb_serial_close(USB_SERIAL_ID);
                return;
            } else {
                eshell_putstring("usb_serial enable success\r\n");
            }
        }
    } else {
        if (usb_series_thread_id != NULL) {
            usb_serial_close(USB_SERIAL_ID);
            osThreadTerminate(usb_series_thread_id);
            usb_series_thread_id = NULL;
            eshell_putstring("usb_serial disable\r\n");
        }
    }
}
/**********************usb series end********************************/

/**********************usb msd start*********************************/
#define CONTACT(x,y)            x y

#define MOUNT_DIR               "/data/massstorage/"
#define TEST_FILE               "test_file.txt"
#define TEST_FILE_CPY           "test_file_copy.txt"

#define USB_TEST_FILE           CONTACT(MOUNT_DIR, TEST_FILE)
#define USB_TEST_FILE_CPY       CONTACT(MOUNT_DIR, TEST_FILE_CPY)

#define TEST_BUF_MAX_SIZE       1024*10
#define TEST_READ
#define TEST_WRITE
#define TEST_COPY

#if defined(TEST_READ) || defined(TEST_WRITE) || defined(TEST_COPY)
static unsigned char file_buf[TEST_BUF_MAX_SIZE];
static unsigned int total_copy_size;

POSSIBLY_UNUSED static void DumpFS(int depth, int count)
{
    DIR *d = opendir(MOUNT_DIR);
    if (!d) {
        TRACE(0, "USB file system borked");
        return;
    }

    TRACE(0, "Dumping root dir");
    struct dirent *p;
    while ((p = readdir(d))) {
        int len = sizeof(struct dirent);
        TRACE(0, "%s %d\n", p->d_name, len);
    }
    closedir(d);
}

POSSIBLY_UNUSED static int test_file_copy(void)
{
    int ret = 0;
    POSSIBLY_UNUSED size_t rcnt, wcnt;
    int tmperr;
    uint32_t stime, etime;
    POSSIBLY_UNUSED uint32_t file_size;

    rcnt = 0;
    wcnt = 0;
    file_size = 0;

    FILE *test_wfp = NULL;
    const char *test_wfile = USB_TEST_FILE;
    for (int8_t i = 0; i < 10; i++) {
        file_buf[i] = i + 'a';
    }
    test_wfp = fopen(test_wfile, "wb");
    fwrite(file_buf, 1, sizeof(file_buf), test_wfp);
    fclose(test_wfp);
    memset(file_buf, 0, TEST_BUF_MAX_SIZE);

    const char *rfile = USB_TEST_FILE;
    const char *wfile = USB_TEST_FILE_CPY;

    FILE *rfp = NULL;
    FILE *wfp = NULL;

    rfp = fopen(rfile, "rb");
    if (rfp == NULL) {
        TRACE(0, "Failed to open file: %s", rfile);
        ret = 1;
        goto _exit;
    }
    wfp = fopen(wfile, "wb");
    if (wfp == NULL) {
        TRACE(0, "Failed to open file: %s", wfile);
        ret = 2;
        goto _exit;
    }

#ifdef TEST_READ
    // Test read
    TRACE(0, "Start to read file ...");

    stime = hal_sys_timer_get();
    total_copy_size = 0;

    while ((rcnt = fread(file_buf, 1, sizeof(file_buf), rfp)) > 0) {
        total_copy_size += rcnt;
        if ((total_copy_size & ((1 << 20) - 1)) == 0) {
            TRACE(0, "Read 0x%x bytes", total_copy_size);
        }
    }
    tmperr = errno;
    if (ret == 0) {
        if (feof(rfp) == 0) {
            TRACE(0, "Failed to read file: %d", tmperr);
            ret = 1;
        }
    }
    etime = hal_sys_timer_get();
    TRACE(0, "Elapsed time: %08d s", TICKS_TO_MS(etime - stime) / 1000);
    TRACE(0, "Read rate: 0x%x byte/s for 0x%x bytes", (total_copy_size / TICKS_TO_MS(etime - stime)) * 1000, total_copy_size);

    if (ret) {
        goto _exit;
    }
#endif

#ifdef TEST_WRITE
    // Test write
    TRACE(0, "Start to write file ...");

    file_size = 100 * TEST_BUF_MAX_SIZE;
    stime = hal_sys_timer_get();
    total_copy_size = 0;
    rcnt = sizeof(file_buf);
    while (total_copy_size < file_size) {
        wcnt = fwrite(file_buf, 1, rcnt, wfp);
        if (wcnt != rcnt) {
            TRACE(0, "Failed to write %d bytes of data at offset %d: %d", rcnt, total_copy_size, errno);
            ret = 1;
            break;
        }
        total_copy_size += rcnt;
    }
    tmperr = errno;
    if (ret) {
        TRACE(0, "Failed to write file: %d", tmperr);
    }
    etime = hal_sys_timer_get();
    TRACE(0, "Elapsed time: %08d s", TICKS_TO_MS(etime - stime) / 1000);
    TRACE(0, "Write rate: 0x%x byte/s for 0x%x bytes", (total_copy_size / TICKS_TO_MS(etime - stime)) * 1000, total_copy_size);

    if (ret) {
        goto _exit;
    }
#endif

#ifdef TEST_COPY
    if (rfp) {
        fclose(rfp);
        rfp = NULL;
    }
    rfp = fopen(rfile, "rb");
    if (rfp == NULL) {
        TRACE(0, "Failed to open file: %s", rfile);
        ret = 1;
        goto _exit;
    }
    // Test copy
    TRACE(0, "Start to copy file ...");

    stime = hal_sys_timer_get();
    total_copy_size = 0;

    while ((rcnt = fread(file_buf, 1, sizeof(file_buf), rfp)) > 0) {
        wcnt = fwrite(file_buf, 1, rcnt, wfp);
        if (wcnt != rcnt) {
            TRACE(0, "Failed to write %d bytes of data at offset %d: %d", rcnt, total_copy_size, errno);
            ret = 1;
            break;
        }
        total_copy_size += rcnt;
        if ((total_copy_size & ((1 << 20) - 1)) == 0) {
            TRACE(0, "Copied %u bytes", total_copy_size);
        }
    }
    tmperr = errno;
    if (ret == 0) {
        if (feof(rfp) == 0) {
            TRACE(0, "Failed to read file: %d", tmperr);
            ret = 1;
        }
    }
    etime = hal_sys_timer_get();
    TRACE(0, "Elapsed time: %08d s", TICKS_TO_MS(etime - stime) / 1000);
    TRACE(0, "Copy rate: 0x%x byte/s for %u bytes", (total_copy_size / TICKS_TO_MS(etime - stime)) * 1000, total_copy_size);
#endif

_exit:
    if (rfp) {
        fclose(rfp);
    }

    return ret;
}
#endif

static void insert_notify(void)
{
#if defined(TEST_READ) || defined(TEST_WRITE) || defined(TEST_COPY)
    DumpFS(0, 0);
    test_file_copy();
#endif
}

static void remove_notify(void)
{
    TRACE(0, "disk remove");
}

static void usb_msd_test(uint32_t enable)
{
    eshell_putstring("usb_msd_test\r\n");
    if (enable == 1) {
        DiskEventRegister(insert_notify, remove_notify);
        USBInit();
        eshell_putstring("usb_msd enable success\r\n");
        while(1) {
            USBLoop();
            osDelay(1000);
        }
    } else {
        eshell_putstring("usb_msd_disable not adapter\r\n");
        //todo
    }
}
/**********************usb msd end***********************************/

/**********************usb mtp start*********************************/
#ifdef MTP_ENABLE
static void usb_mtp_test(uint32_t enable)
{
    eshell_putstring("usb_mtp_test\r\n");
    static uint8_t first_en = 1;

    if (enable == 1) {
        int ret;
        char *fs_mount_point_mem = NULL;
        const uint32_t fs_max_file_size = 1024 * 1024 * 1024;
        int32_t storageid[STORAGE_MAX_NUM];
        int id_index = 0;
        POSSIBLY_UNUSED struct MTP_STORAGE_NAME storagename[STORAGE_MAX_NUM];

        fs_mount_point_mem = "data/emmc0";
        if (first_en) {
            ret = mount("/dev/emmc0", fs_mount_point_mem, "fatfs", 0, "autoformat");
            if (ret != 0) {
                eshell_putstring("/dev/emmc0 mount failed\r\n");
                return;
            }
            first_en = 0;
        }

        ret = mtp_callback_register();
        if (ret != 0) {
            eshell_putstring("Failed to register mtp callback\r\n");
            return;
        }
        if (fs_mount_point_mem != NULL) {
            storageid[id_index] = usb_mtp_fs_bind(fs_mount_point_mem, fs_max_file_size);
            eshell_putstring("fs_bind return storage id[%d]:%x", id_index, storageid[id_index]);
            if(storageid[id_index] <= 0) {
                eshell_putstring("Failed to bind mtp fs: %d\r\n", storageid[id_index]);
                return;
            }
            storagename[id_index].namelen = strlen(fs_mount_point_mem);
            storagename[id_index].name = (uint8_t *)fs_mount_point_mem;
            id_index++;
        }

        usb_mtp_set_storage(id_index, storageid, NULL);
        ret = usb_mtp_open(USB_MTP_API_NONBLOCKING, NULL);
        if (ret != 0) {
            eshell_putstring("Failed to open usb mtp: %d\r\n", ret);
            return;
        }
    } else {
        eshell_putstring("usb_mtp disable\r\n");
        usb_mtp_close();
    }
}
#endif
/**********************usb mtp end***********************************/

static void unitest_usb(int argc, char *argv[])
{
    if (argc < 4) {
        eshell_putstring("ERROR_ARG\r\n");
        eshell_putstring("   utest_usb master msd enable/disable\r\n");
        eshell_putstring("   utest_usb slave serial/mtp enable/disable\r\n");
        return;
    }

    static uint32_t enable;
    if (strncmp(argv[3], "enable", 6) == 0) {
        enable = 1;
    } else if (strncmp(argv[3], "disable", 7) == 0) {
        enable = 0;
    } else {
        eshell_putstring("ERROR_ARG\r\n");
        eshell_putstring("   utest_usb master msd enable/disable\r\n");
        eshell_putstring("   utest_usb slave serial/mtp enable/disable\r\n");
        return;
    }

    if (strncmp(argv[1], "master", 6) == 0) {
        if (strncmp(argv[2], "msd", 3) == 0) {
            usb_msd_test(enable);
        } else {
            eshell_putstring("usb enum_type err, valid value: msd\r\n");
            return;
        }
    } else if (strncmp(argv[1], "slave", 5) == 0) {
        if (strncmp(argv[2], "serial", 6) == 0) {
            usb_serial_test(enable);
#ifdef MTP_ENABLE
        } else if (strncmp(argv[2], "mtp", 3) == 0) {
            usb_mtp_test(enable);
#endif
        } else {
            eshell_putstring("usb enum_type err, valid value: serial,mtp\r\n");
            return;
        }
    } else {
        eshell_putstring("usb mode err, valid value: master/slave\r\n");
    }
}
ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_UNIT_TEST, "utest_usb", "utest_usb mode(master/slave) enum_type(serial/msd)",
                   unitest_usb);

#endif

