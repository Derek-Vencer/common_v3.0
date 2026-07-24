#include <string.h>
#include <stdlib.h>

#include "cmsis_os.h"
#include "hal_trace.h"
#include "iap2_mfi_i2c.h"
#include "hal_timer.h"
#include "hal_gpio.h"

#define TITLE_STR(x)        #x
#define SimulateI2cLog(num,title,value,...) do{BTAPP_TRACE(0, "[IAP2-Ext]" title ":" value,##__VA_ARGS__);}while(0)
#define SimulateI2cLogI(num,x,...)  SimulateI2cLog(num,TITLE_STR(SimulateI2cLogI),x,##__VA_ARGS__ )
#define SimulateI2cLogD(num,x,...)  SimulateI2cLog(num,TITLE_STR(SimulateI2cLogD),x,##__VA_ARGS__ )
#define SimulateI2cLogW(num,x,...)  SimulateI2cLog(num,TITLE_STR(SimulateI2cLogW),x,##__VA_ARGS__ )
#define SimulateI2cLogE(num,x,...)  SimulateI2cLog(num,TITLE_STR(SimulateI2cLogE),x,##__VA_ARGS__ )

#define CHECK_I2C_INIT_STATUS(status)       if(status) return;

static bool i2c_init_flag = false;
static iap2_mfi_hal_i2c_cfg_t mfi_i2c_cfg = {
    .name = "IAP2_MFi_3.0",
    .port = IAP2_MFI_HAL_I2C_ID,
    .bus_addr = IAP2_MFI_HAL_I2C_BUS_ADDR,
    .i2c_cfg = {
        .speed = IAP2_MFI_HAL_I2C_SPEED,
        .mode = IAP2_MFI_HAL_I2C_MODE,
        .use_dma = 0,
        .use_sync = 1,
        .as_master = 1,
    }
};

static void hal_i2c_delay_ms(int delay_ms)
{
    hal_sys_timer_delay_us(delay_ms * 1000);
}

void hal_i2c_init()
{
    int ret;
    CHECK_I2C_INIT_STATUS(i2c_init_flag); // if true, will early return

    // please check io config is right:
    // make sure i2c_id(like: HAL_I2C_ID_1) and iomux config(like: hal_iomux_set_i2c1)
    // examples:
    //  1502p-maybe add build option: I2C1_IOMUX_INDEX=10
    // hal_iomux_set_i2c1(); // never to call it, only as information

    ret = hal_i2c_open(mfi_i2c_cfg.port, &mfi_i2c_cfg.i2c_cfg);
    if (ret) {
        SimulateI2cLogE(0,"hal_i2c_init:hal_i2c_open failed %d", ret);
        return;
    }

    i2c_init_flag = true;
    SimulateI2cLogI(0,"hal_i2c_init: i2c_id=%d\r\b", mfi_i2c_cfg.port);
}

void hal_i2c_uninit()
{
    if(i2c_init_flag)
    {
        hal_i2c_close(mfi_i2c_cfg.port);
    }

    i2c_init_flag = false;
    SimulateI2cLogI(0,"hal_i2c_uninit: i2c_id=%d\r\b", mfi_i2c_cfg.port);
}

int hal_i2c_tx(iap2_mfi_hal_i2c_cfg_t *dev, const unsigned char reg, const unsigned char *txData, const unsigned int txLength)
{
    int i = 0;
    int ret = 0;
    int max_retry_times = (reg == MFI_CONTROL_STATUS_REG) ? IAP2_MFI_HAL_I2C_MAX_RETRY * 3 : IAP2_MFI_HAL_I2C_MAX_RETRY;
    unsigned int i2cTxLength = txLength + 1;
    unsigned char *i2cTxData = (unsigned char*)malloc(i2cTxLength);
    if(i2cTxData == NULL) {
        SimulateI2cLogE(0,"hal_i2c_tx: malloc error\n");
        return -1;
    }
    i2cTxData[0] = reg;
    if(txLength) {
        memcpy(i2cTxData + 1, txData, txLength);
    }
    for(i = 0; i < max_retry_times; i++) {
        ret = hal_i2c_simple_send(dev->port, dev->bus_addr, i2cTxData, i2cTxLength);
        if(ret != 0) {
            hal_i2c_delay_ms(IAP2_MFI_HAL_I2C_SLEEP_MS);
            if(i == (max_retry_times - 1))
            {
                SimulateI2cLogE(0,"hal_i2c_tx: send failed\n");
            }
            continue;
        } else {
            break;
        }
    }
    free(i2cTxData);
    return ret;
}

int hal_i2c_rx(iap2_mfi_hal_i2c_cfg_t *dev, unsigned char *rxData, const unsigned int rxLength, int retry_times)
{
    int i = 0;
    int ret = 0;
    int max_retry_times = (retry_times == 0) ? IAP2_MFI_HAL_I2C_MAX_RETRY : retry_times;
    unsigned char* i2cRxData = rxData;
    unsigned int i2cRxLength = rxLength;
    for(i = 0; i < max_retry_times; i++) {
        ret = hal_i2c_simple_recv(dev->port, dev->bus_addr, NULL, 0, i2cRxData, i2cRxLength);
        if(ret != 0) {
            hal_i2c_delay_ms(IAP2_MFI_HAL_I2C_SLEEP_MS);
            if(i == (max_retry_times - 1))
            {
                SimulateI2cLogE(0,"hal_i2c_rx: recv failed\n");
            }
            continue;
        } else {
            break;
        }
    }
    return ret;
}

int hal_i2c_tx_rx(iap2_mfi_hal_i2c_cfg_t *dev, const unsigned char reg, const unsigned char *txData, const unsigned int txLength, unsigned char *rxData, const unsigned int rxLength)
{
    int ret = 0;
    ret = hal_i2c_tx(dev, reg, txData, txLength);
    if(ret == 0) {
        hal_i2c_delay_ms(IAP2_MFI_HAL_I2C_SLEEP_MS);
        ret = hal_i2c_rx(dev, rxData, rxLength, (reg == MFI_CONTROL_STATUS_REG) ? IAP2_MFI_HAL_I2C_MAX_RETRY * 3 : IAP2_MFI_HAL_I2C_MAX_RETRY);
    }
    return ret;
}

int hal_do_i2c(iap2_mfi_hal_i2c_cfg_t *dev, const unsigned char reg, const unsigned char *txData, const unsigned int txLength, unsigned char *rxData, unsigned int rxLength)
{
    int ret = 0;
    if(dev == NULL) {
        SimulateI2cLogE(0,"hal_do_i2c: i2c device is null\n");
        return -1;
    }
    if(rxData != NULL && rxLength > 0) {
        ret = hal_i2c_tx_rx(dev, reg, txData, txLength, rxData, rxLength);
    } else {
        ret = hal_i2c_tx(dev, reg, txData, txLength);
    }
    return ret;
}

int hal_do_i2c_tx(iap2_mfi_hal_i2c_cfg_t *dev, const unsigned char *txData, const unsigned int txLength) {
    int i = 0;
    int ret = 0;

    if(dev == NULL || txData == NULL || txLength == 0) {
        SimulateI2cLogE(0,"hal_do_i2c_tx: param is error\n");
        return -1;
    }
    for(i = 0; i < IAP2_MFI_HAL_I2C_MAX_RETRY; i++) {
        ret = hal_i2c_simple_send(dev->port, dev->bus_addr, txData, txLength);
        if(ret != 0) {
            hal_i2c_delay_ms(IAP2_MFI_HAL_I2C_SLEEP_MS);
            if(i == (IAP2_MFI_HAL_I2C_MAX_RETRY - 1))
            {
                SimulateI2cLogE(0,"hal_do_i2c_tx: send failed\n");
            }
            continue;
        } else {
            break;
        }
    }
    return ret;
}

// device_addr is hal i2c bus address, so don't need to input
int hal_i2c_write_reg_data(u8 device_addr,u8 reg_addr,u8 *buf,int len)
{
    int ret = 0;

    hal_i2c_init();

    ret = hal_do_i2c(&mfi_i2c_cfg, reg_addr, buf, len, NULL, 0); // success return 0
    if(ret != 0)
    {
        ret = -1;
    }
    return ret;
}

// device_addr is hal i2c bus address, so don't need to input
int hal_i2c_read_reg_data(u8 device_addr,u8 reg_addr,u8 *buf,int len)
{
    int ret = 0;

    hal_i2c_init();

    ret = hal_do_i2c(&mfi_i2c_cfg, reg_addr, NULL, 0, buf, len); // success return 0
    if(ret != 0)
    {
        ret = -1;
    }
    return ret;
}