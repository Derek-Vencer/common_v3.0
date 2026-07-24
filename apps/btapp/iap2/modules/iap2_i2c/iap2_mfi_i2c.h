#ifndef __SIMULATE_I2C__
#define __SIMULATE_I2C__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_gpio.h"
#include "hal_i2c.h"
#include "reg_gpio_v2.h"
#include "plat_types.h"

// MFI CHIP REGS
#define MFI_CONTROL_STATUS_REG                      0x10
#define MFI_CHALLENGE_RESPONSE_LEN_REG              0x11  // 2 bytes reset value 128
#define MFI_CHALLENGE_RESPONSE_DATA_REG             0x12  //128 bytes
#define MFI_CHALLENGE_DATA_LEN_REG                  0x20  // 2 bytes reset value 20
#define MFI_CHALLENGE_DATA_REG                      0x21  // 128 byte
#define MFI_ACCESSORY_CERTIFICATE_DATA_LEN_REG      0X30 // 2 bytes   <=1280
#define MFI_ACCESSORY_CERTIFICATE_DATA_REG          0x31   // 0x31 --> 0x3A
#define MFI_APPLE_DEVICE_CERTIFICATE_DATA_LEN_REG   0X50  // 2bytes
#define MFI_APPLE_DEVICE_CERTIFICATE_DATA_REG       0X51  // 0X51 --> 0X58
#define MFI_CERTIFICATE_SERIAL_NUMBER               0x4E


// MFI SIMULATE I2C MODULE
#define I2C_SLEEP_TIME 4

#define BOARD_IOMUX_GPIO_I2C_SCL                    HAL_IOMUX_PIN_P1_0
#define BOARD_IOMUX_GPIO_I2C_SDA                    HAL_IOMUX_PIN_P1_1
#define BOARD_GPIO_GPIO_I2C_SCL                     HAL_GPIO_PIN_P1_0
#define BOARD_GPIO_GPIO_I2C_SDA                     HAL_GPIO_PIN_P1_1

#define I2C_IOMUX_SDA                               BOARD_IOMUX_GPIO_I2C_SDA
#define I2C_IOMUX_SCL                               BOARD_IOMUX_GPIO_I2C_SCL
#define I2C_SDA_PIN                                 BOARD_GPIO_GPIO_I2C_SDA
#define I2C_SCK_PIN                                 BOARD_GPIO_GPIO_I2C_SCL

// usage: bta_iap2_service_register_mfi_i2c_callback(i2c_read_reg_data, i2c_write_reg_data)
void i2c_init(void);
int i2c_write_reg_data(u8 device_addr,u8 reg_addr,u8 *buf,int len);
int i2c_read_reg_data(u8 device_addr,u8 reg_addr,u8 *buf,int len);


// MFI HAL I2C MODULE
#define IAP2_MFI_HAL_I2C_ID                         HAL_I2C_ID_1
#define IAP2_MFI_HAL_I2C_SPEED                      (100*1000)
#define IAP2_MFI_HAL_I2C_MODE                       HAL_I2C_API_MODE_SIMPLE
#define IAP2_MFI_HAL_I2C_BUS_ADDR                   (0x10)

#define IAP2_MFI_HAL_I2C_MAX_RETRY                  (100)
#define IAP2_MFI_HAL_I2C_SLEEP_MS                   (2)

typedef struct {
    char* name; // only support use const string: like "IAP2_MFI"
    unsigned char port;
    unsigned char bus_addr;
    struct HAL_I2C_CONFIG_T i2c_cfg;
} iap2_mfi_hal_i2c_cfg_t;

// usage: bta_iap2_service_register_mfi_i2c_callback(hal_i2c_read_reg_data, hal_i2c_write_reg_data)
void hal_i2c_init(void);
int hal_i2c_write_reg_data(u8 device_addr,u8 reg_addr,u8 *buf,int len);
int hal_i2c_read_reg_data(u8 device_addr,u8 reg_addr,u8 *buf,int len);

// other interface be included is up to user
// void i2c_sda(u8 state);
#endif

#ifdef __cplusplus
}
#endif
