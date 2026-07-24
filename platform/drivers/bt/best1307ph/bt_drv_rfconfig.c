/***************************************************************************
 *
 * Copyright 2015-2026 BES.
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
#include <string.h>
#include "pmu.h"
#include "hal_chipid.h"
#ifdef __RF_INIT_XTAL_CAP_FROM_NV__
#include "nvrecord_dev.h"
#endif
#include "bt_drv.h"
#include "bt_drv_interface.h"
#include "bt_drv_internal.h"
#include "bt_drv_reg_op.h"

//CHIP related
#include "bt_drv_1307ph_internal.h"
#include "bt_drv_1307ph_config.h"
#include CHIP_SPECIFIC_HDR(bt_drv_modem_reg_map)
#define XTAL_FCAP_MAX_STEP                      (0x40)
#define XTAL_FCAP_NORMAL_SHIFT                  5
#define XTAL_FCAP_NORMAL_MASK                   (0xFF << XTAL_FCAP_NORMAL_SHIFT)
#define XTAL_FCAP_NORMAL(n)                     BITFIELD_VAL(XTAL_FCAP_NORMAL, n)

static uint16_t xtal_fcap = DEFAULT_XTAL_FCAP;
static uint16_t init_xtal_fcap = DEFAULT_XTAL_FCAP;

struct RF_SYS_INIT_ITEM
{
    uint16_t reg;
    uint16_t set;
    uint16_t mask;
    uint16_t delay;
};

struct TX_PWR_SELECT_ITEM
{
    float pwr_dbm;
    uint16_t tx_gain_tbl0;
    uint16_t tx_gain_tbl1;
    uint16_t tx_gain_tbl2;
    uint8_t pa_sel;
};

#define TX_PWR_1P8_PA           (1 << 0)

static const struct TX_PWR_SELECT_ITEM tx_pwr_select[] =
{
    {14,  0xF07F, 0x42A8, 0x04CC, TX_PWR_1P8_PA},   //14dbm
    {13,  0xF07A, 0x4248, 0x04CC, TX_PWR_1P8_PA},   //13dbm
    {12,  0xF07A, 0x4168, 0x04CC, TX_PWR_1P8_PA},   //12dbm
    {11,  0xF077, 0x4168, 0x04CC, TX_PWR_1P8_PA},   //11dbm
    {10,  0xF077, 0x4109, 0x04CC, TX_PWR_1P8_PA},   //10dbm
    {9,   0xF077, 0x40A9, 0x04CC, TX_PWR_1P8_PA},   //9dbm
    {8,   0xF076, 0x4089, 0x04CC, TX_PWR_1P8_PA},   //8dbm
    {7,   0xF075, 0x4089, 0x04CC, TX_PWR_1P8_PA},   //7dbm
    {6,   0xF074, 0x4089, 0x04CC, TX_PWR_1P8_PA},   //6dbm
    {5,   0xF074, 0x406A, 0x04CC, TX_PWR_1P8_PA},   //5dbm
    {4,   0xF073, 0x406A, 0x04CC, TX_PWR_1P8_PA},   //4dbm
    {3,   0xF073, 0x406D, 0x04CC, TX_PWR_1P8_PA},   //3dbm
    {2,   0xF072, 0x406A, 0x04CC, TX_PWR_1P8_PA},   //2dbm
    {1,   0xF072, 0x406D, 0x04CC, TX_PWR_1P8_PA},   //1dbm
    {0,   0xF072, 0x4070, 0x04CC, TX_PWR_1P8_PA},   //0dbm
    {-1,  0xF072, 0x4073, 0x04CC, TX_PWR_1P8_PA},  //-1dbm
    {-2,  0xF072, 0x4078, 0x04CC, TX_PWR_1P8_PA},  //-2dbm
    {-3,  0xF072, 0x407C, 0x04CC, TX_PWR_1P8_PA},  //-3dbm
    {-4,  0xF071, 0x4073, 0x04CC, TX_PWR_1P8_PA},  //-4dbm
    {-5,  0xF071, 0x4053, 0x04CC, TX_PWR_1P8_PA},  //-5dbm
    {-6,  0xF071, 0x4036, 0x04CC, TX_PWR_1P8_PA},  //-6dbm
    {-7,  0xF074, 0x4065, 0x04CC, TX_PWR_1P8_PA},  //-7dbm
    {-8,  0xF075, 0x4066, 0x04CC, TX_PWR_1P8_PA},  //-8dbm
    {-9,  0xF074, 0x4046, 0x04CC, TX_PWR_1P8_PA},  //-9dbm
    {-10, 0xF074, 0x4047, 0x04CC, TX_PWR_1P8_PA},  //-10dbm
    {-11, 0xF074, 0x4027, 0x04CC, TX_PWR_1P8_PA},  //-11dbm
    {-12, 0xF074, 0x4007, 0x04CC, TX_PWR_1P8_PA},  //-12dbm
};

static const struct RF_SYS_INIT_ITEM rf_sys_init_tbl[] =
{
};

const uint16_t rf_init_tbl_1[][3] =
{
    {0x0001, 0x0032, 0},
    {0x0019, 0xB000, 0},
    {0x0019, 0x3000, 0},
    {0x001A, 0x0F1C, 0},
    {0x001B, 0x4000, 0},
    {0x0023, 0x00B0, 0},
    {0x004B, 0x0600, 0},
    {0x004D, 0x0A00, 0},
    {0x008F, 0x6DFB, 0},
    {0x0091, 0x003C, 0},
    {0x0092, 0x820F, 0},
    {0x0093, 0x0093, 0},
    {0x0099, 0x003A, 0},
    {0x009B, 0x5745, 0},
    {0x009E, 0x0F66, 0},
    {0x00A0, 0x0600, 0},
    {0x00A2, 0x1003, 0},
    {0x00A3, 0x0700, 0},
    {0x00A4, 0x0054, 0},
    {0x00B1, 0x0038, 0},
    {0x00B5, 0x04CC, 0},
    {0x00C1, 0x2E00, 0},
    {0x00CA, 0x0D03, 0},
    {0x0147, 0x2000, 0},
    {0x014F, 0x407F, 0},
    {0x015C, 0x0018, 0},
    {0x018C, 0x0000, 0},

#ifdef BT_RF_I2V_BYPASS
    // ===== gain0 (i2v bypass) =====
    {0x0041, 0x00A7, 0},
    {0x005F, 0x00A7, 0},
    {0x0039, 0xD92E, 0},
    {0x0067, 0xD92E, 0},
    {0x034A, 0xFF00, 0},
    {0x032A, 0x0000, 0},
    {0x033A, 0x0000, 0},

    // ===== gain1 (i2v bypass) =====
    {0x0042, 0x20A3, 0},
    {0x0060, 0x20A3, 0},
    {0x003A, 0xD12E, 0},
    {0x0068, 0xD12E, 0},
    {0x034B, 0xFF00, 0},
    {0x032B, 0x0000, 0},
    {0x033B, 0x0000, 0},

    // ===== gain2 (i2v bypass) =====
    {0x0043, 0x20A3, 0},
    {0x0061, 0x20A3, 0},
    {0x003B, 0x512E, 0},
    {0x0069, 0x512E, 0},
    {0x034C, 0xFF00, 0},
    {0x032C, 0x0000, 0},
    {0x033C, 0x0000, 0},

    // ===== gain3 (i2v bypass) =====
    {0x0044, 0x22A1, 0},
    {0x0062, 0x22A1, 0},
    {0x003C, 0x512E, 0},
    {0x006A, 0x512E, 0},
    {0x034D, 0xFF00, 0},
    {0x032D, 0x0000, 0},
    {0x033D, 0x0000, 0},

    // ===== gain4 (i2v bypass) =====
    {0x0045, 0x00A0, 0},
    {0x0063, 0x00A0, 0},
    {0x003D, 0x112F, 0},
    {0x006B, 0x112F, 0},
    {0x034E, 0xFF00, 0},
    {0x032E, 0x0000, 0},
    {0x033E, 0x0000, 0},

    // ===== gain5 (i2v bypass) =====
    {0x0046, 0x04A0, 0},
    {0x0064, 0x04A0, 0},
    {0x003E, 0x112F, 0},
    {0x006C, 0x112F, 0},
    {0x034F, 0xFF00, 0},
    {0x032F, 0x0000, 0},
    {0x033F, 0x0000, 0},

    // ===== gain6 (i2v bypass) =====
    {0x0047, 0x0AA0, 0},
    {0x0065, 0x0AA0, 0},
    {0x003F, 0x0BEF, 0},
    {0x006D, 0x0BEF, 0},
    {0x0350, 0xFF00, 0},
    {0x0330, 0x0000, 0},
    {0x0340, 0x0000, 0},

    // ===== gain7 (i2v bypass) =====
    {0x0048, 0x10A0, 0},
    {0x0066, 0x10A0, 0},
    {0x0040, 0x05EF, 0},
    {0x006E, 0x05EF, 0},
    {0x0351, 0xFF00, 0},
    {0x0331, 0x0000, 0},
    {0x0341, 0x0000, 0},
#else
    // ===== gain0 =====
    {0x0041, 0x00B7, 0},
    {0x005F, 0x00B7, 0},
    {0x0039, 0xD9C6, 0},
    {0x0067, 0xD9C6, 0},
    {0x034A, 0x5082, 0},
    {0x032A, 0x0041, 0},
    {0x033A, 0x0082, 0},

    // ===== gain1 =====
    {0x0042, 0x20B3, 0},
    {0x0060, 0x20B3, 0},
    {0x003A, 0xD946, 0},
    {0x0068, 0xD946, 0},
    {0x034B, 0x5082, 0},
    {0x032B, 0x0041, 0},
    {0x033B, 0x0082, 0},

    // ===== gain2 =====
    {0x0043, 0x20B3, 0},
    {0x0061, 0x20B3, 0},
    {0x003B, 0x5946, 0},
    {0x0069, 0x5946, 0},
    {0x034C, 0x5082, 0},
    {0x032C, 0x0041, 0},
    {0x033C, 0x0082, 0},

    // ===== gain3 =====
    {0x0044, 0x22B1, 0},
    {0x0062, 0x22B1, 0},
    {0x003C, 0x5846, 0},
    {0x006A, 0x5846, 0},
    {0x034D, 0x50B8, 0},
    {0x032D, 0x005C, 0},
    {0x033D, 0x00B8, 0},

    // ===== gain4 =====
    {0x0045, 0x00B0, 0},
    {0x0063, 0x00B0, 0},
    {0x003D, 0x1847, 0},
    {0x006B, 0x1847, 0},
    {0x034E, 0x50B8, 0},
    {0x032E, 0x005C, 0},
    {0x033E, 0x00B8, 0},

    // ===== gain5 =====
    {0x0046, 0x04B0, 0},
    {0x0064, 0x04B0, 0},
    {0x003E, 0x1747, 0},
    {0x006C, 0x1747, 0},
    {0x034F, 0x50FF, 0},
    {0x032F, 0x0082, 0},
    {0x033F, 0x00FF, 0},

    // ===== gain6 =====
    {0x0047, 0x0AB0, 0},
    {0x0065, 0x0AB0, 0},
    {0x003F, 0x1647, 0},
    {0x006D, 0x1647, 0},
    {0x0350, 0x50FF, 0},
    {0x0330, 0x00B8, 0},
    {0x0340, 0x00FF, 0},

    // ===== gain7 =====
    {0x0048, 0x10B0, 0},
    {0x0066, 0x10B0, 0},
    {0x0040, 0x1647, 0},
    {0x006E, 0x1647, 0},
    {0x0351, 0x50FF, 0},
    {0x0331, 0x00B8, 0},
    {0x0341, 0x00FF, 0},
#endif
};

#ifdef __HW_AGC__
const uint16_t rf_init_hw_agc_config[][3] =
{
#ifdef BT_RF_I2V_BYPASS
    // i2v bypass
    // ===== gain0 (i2v bypass) =====
    {0x021D, 0x00A7, 0},
    {0x021E, 0xD92E, 0},

    // ===== gain1 (i2v bypass) =====
    {0x0220, 0x20A3, 0},
    {0x0221, 0xD12E, 0},

    // ===== gain2 (i2v bypass) =====
    {0x0223, 0x20A3, 0},
    {0x0224, 0x512E, 0},

    // ===== gain3 (i2v bypass) =====
    {0x0226, 0x22A1, 0},
    {0x0227, 0x512E, 0},

    // ===== gain4 (i2v bypass) =====
    {0x0229, 0x00A0, 0},
    {0x022A, 0x112F, 0},

    // ===== gain5 (i2v bypass) =====
    {0x022C, 0x04A0, 0},
    {0x022D, 0x112F, 0},

    // ===== gain6 (i2v bypass) =====
    {0x022F, 0x04A0, 0},
    {0x0230, 0x0DEF, 0},

    // ===== gain7 (i2v bypass) =====
    {0x0232, 0x04A0, 0},
    {0x0233, 0x03EF, 0},

    // gain offset
    {0x0235, 0x2B34, 0},
    {0x0236, 0x2026, 0},
    {0x0237, 0x171A, 0},
    {0x0238, 0x0912, 0},

    // jump
    {0x0239, 0xACA5, 0},
    {0x023A, 0xB6B2, 0},
    {0x023B, 0xBFB9, 0},
    {0x023C, 0x00C8, 0},
#else
    // no i2v bypass
    // ===== gain0 =====
    {0x021D, 0x00B7, 0},
    {0x021E, 0xD9C6, 0},

    // ===== gain1 =====
    {0x0220, 0x20B3, 0},
    {0x0221, 0xD946, 0},

    // ===== gain2 =====
    {0x0223, 0x20B3, 0},
    {0x0224, 0x5946, 0},

    // ===== gain3 =====
    {0x0226, 0x22B1, 0},
    {0x0227, 0x5846, 0},

    // ===== gain4 =====
    {0x0229, 0x00B0, 0},
    {0x022A, 0x1847, 0},

    // ===== gain5 =====
    {0x022C, 0x04B0, 0},
    {0x022D, 0x1747, 0},

    // ===== gain6 =====
    {0x022F, 0x0AB0, 0},
    {0x0230, 0x1647, 0},

    // ===== gain7 =====
    {0x0232, 0x10B0, 0},
    {0x0233, 0x1647, 0},

    // gain offset
    {0x0235, 0x333B, 0},
    {0x0236, 0x232D, 0},
    {0x0237, 0x191F, 0},
    {0x0238, 0x0D14, 0},

    // jump
    {0x0239, 0xACAA, 0},
    {0x023A, 0xB6B2, 0},
    {0x023B, 0xBFB9, 0},
    {0x023C, 0x00C8, 0},
#endif

    // [1307PH hwagc ctrl rf_sync lock]
    // hwagc_ctrl
    {0x0209, 0x8155, 0},
    {0x020B, 0x00A8, 0},
    {0x020C, 0x00A8, 0},
    {0x020D, 0x0060, 0},
    {0x020E, 0x00F0, 0},
    {0x020F, 0x0006, 0},
    {0x0210, 0x0258, 0},
    {0x0211, 0x00A8, 0},
    {0x0212, 0x00A8, 0},
    {0x0213, 0x0060, 0},
    {0x0214, 0x00F0, 0},
    {0x0215, 0x7F7F, 0},
    {0x0216, 0x7F7F, 0},
    {0x0217, 0x4C01, 0},
    {0x021C, 0x3B00, 0},
    {0x027D, 0x0001, 0},
    {0x027E, 0x11D4, 0},
    {0x027F, 0x11D4, 0},
    {0x0280, 0x11D4, 0},
    {0x0281, 0x00C0, 0},
    {0x0282, 0x003C, 0},
    {0x0283, 0x0060, 0},
    {0x0284, 0x003C, 0},
    {0x0287, 0x013B, 0},
    {0x0288, 0x00AD, 0},
    {0x028C, 0x003C, 0},
    {0x028D, 0x003C, 0},
    {0x028E, 0x0060, 0},
    {0x0292, 0x7F04, 0},
    {0x0293, 0x7F7F, 0},
    {0x0294, 0x0000, 0},
    {0x0295, 0x11D4, 0},
    {0x0296, 0x11D4, 0},
    {0x0297, 0x11D4, 0},
    {0x0298, 0x11D4, 0},
    {0x0299, 0x11D4, 0},
    {0x029A, 0x11D4, 0},
    {0x029B, 0x11D4, 0},
    {0x029C, 0x11D4, 0},
    {0x02DD, 0x7000, 0},

    // initial gain ctrl
    {0x019E, 0x2D40, 0},
    {0x019F, 0xBFBA, 0},
    {0x01A0, 0xC9C4, 0},
    {0x01A1, 0xD3CE, 0},
    {0x01A2, 0x0FF0, 0},
    {0x01A5, 0x0400, 0},
    {0x01A6, 0x0008, 0},
    {0x01A9, 0x3601, 0},
    {0x01AA, 0x36DB, 0},
    {0x01AB, 0x781B, 0},
    {0x01AD, 0x0007, 0},
    {0x01CF, 0x002E, 0},
};
#endif

void bt_drv_rf_set_bt_hw_agc_enable(bool enable)
{
#if 0
    uint16_t val_e2 = 0;
    btdrv_read_rf_reg(0xb6,&val_e2);
    if(enable)
    {
        //open rf bt hw agc mode
        val_e2 |= (1<<1);
    }
    else
    {
        //close rf bt hw agc mode
        val_e2 &= ~(1<<1);
    }
    btdrv_write_rf_reg(0xb6,val_e2);
#endif
}

void bt_drv_rf_set_ble_hw_agc_enable(bool enable)
{
#if 0
    uint16_t val_e2 = 0;
    btdrv_read_rf_reg(0xb7,&val_e2);
    if(enable)
    {
        //open rf ble hw agc mode
        val_e2 |= (1<<11);
    }
    else
    {
        //close rf ble hw agc mode
        val_e2 &= ~(1<<11);
    }
    btdrv_write_rf_reg(0xb7,val_e2);
#endif
}

void bt_drv_rf_set_afh_monitor_gain(void)
{
#if 0
    uint16_t val_e2 = 0;
    btdrv_read_rf_reg(0x1e4,&val_e2);//reg_bt_i2v_rin_afh[6:4]
    //i2v_rin_afh = 2 i2v_stb_afh=1
    val_e2 &= ~(7<<4);
    val_e2 |= (2<<4);
    val_e2 &= ~(7<<12); //reg_bt_i2v_stb_afh[14:12]
    val_e2 |= (1<<12); //reg_bt_i2v_stb_afh[14:12]
    btdrv_write_rf_reg(0x1e4,val_e2);


    btdrv_read_rf_reg(0x3cb,&val_e2); //lna_ic = 0 , i2v_gain = 7  lna_ldo_ic = 0xb
    val_e2 &= ~(0xf<<0);//reg_bt_i2v_gain_afh[3:0]
    val_e2 |= (7<<0);

    val_e2 &= ~(3<<4);//reg_bt_adc_vin_sel_afh[5:4]
    val_e2 |= (1<<4);

    val_e2 &= ~(0xF<<8);//reg_bt_lna_ldo_ic_afh[11:8]
    val_e2 |= (0x5<<8);

    val_e2 &= ~(7<<12);// reg_bt_lna_ic_afh[14:12]

    val_e2 &= ~(3<<6);//reg_bt_adc_vin_sel_normal[7:6]
    val_e2 |= (1<<6);

    btdrv_write_rf_reg(0x3cb,val_e2);


    btdrv_read_rf_reg(0x3cc,&val_e2); //flt = 7, hg_en=0, hg=0, rpass=11, rin=00,rfflt=0

    val_e2 = 0x1E0C;

    btdrv_write_rf_reg(0x3cc,val_e2);
#endif
}

static void  bt_drv_rf_set_xtal_fcap_real(int current_val)
{
    uint16_t val;

    btdrv_read_rf_reg(RF_REG_XTAL_FCAP, &val);
    val = (val & ~XTAL_FCAP_NORMAL_MASK) |
          ((current_val << XTAL_FCAP_NORMAL_SHIFT) & XTAL_FCAP_NORMAL_MASK);
    btdrv_write_rf_reg(RF_REG_XTAL_FCAP, val);
}

void btdrv_rf_init_xtal_fcap(uint32_t fcap)
{
    uint16_t val = 0;

    btdrv_read_rf_reg(RF_REG_XTAL_FCAP, &val);
    val &= XTAL_FCAP_NORMAL_MASK;
    bt_drv_rf_set_xtal_fcap(val, fcap, XTAL_FCAP_MAX_STEP, bt_drv_rf_set_xtal_fcap_real);
    xtal_fcap = fcap;
    init_xtal_fcap = xtal_fcap;
}

uint32_t btdrv_rf_get_xtal_fcap(void)
{
    return GET_BITFIELD(xtal_fcap, XTAL_FCAP_NORMAL);
}

struct btdrv_customer_rf_config_t btdrv_rf_customer_config;
void bt_drv_rf_sdk_init(void)
{
    BT_DRV_TRACE(0,"SDK default rf config");
    btdrv_rf_customer_config.config_xtal_en = false;//get value from NV
    btdrv_rf_customer_config.xtal_cap_val = 0;//get value from NV

    //config TX power in dbm
    btdrv_rf_customer_config.config_tx_pwr_en = true;
    btdrv_rf_customer_config.bt_tx_page_pwr = TX_PWR_8DBM;//in dbm

    btdrv_rf_customer_config.bt_tx_idx3_pwr = TX_PWR_14DBM;//in dbm
    btdrv_rf_customer_config.bt_tx_idx2_pwr = TX_PWR_10DBM;//in dbm
    btdrv_rf_customer_config.bt_tx_idx1_pwr = TX_PWR_5DBM;//in dbm
    btdrv_rf_customer_config.bt_tx_idx0_pwr = TX_PWR_0DBM;//in dbm

    btdrv_rf_customer_config.le_tx_idx3_pwr = TX_PWR_14DBM;//in dbm
    btdrv_rf_customer_config.le_tx_idx2_pwr = TX_PWR_10DBM;//in dbm
    btdrv_rf_customer_config.le_tx_idx1_pwr = TX_PWR_5DBM;//in dbm
    btdrv_rf_customer_config.le_tx_idx0_pwr = TX_PWR_0DBM;//in dbm
    //init BT BLE convert table
    btdrv_txpwr_conv_tbl[0] = btdrv_rf_customer_config.bt_tx_idx0_pwr;
    btdrv_txpwr_conv_tbl[1] = btdrv_rf_customer_config.bt_tx_idx1_pwr;
    btdrv_txpwr_conv_tbl[2] = btdrv_rf_customer_config.bt_tx_idx2_pwr;
    btdrv_txpwr_conv_tbl[3] = btdrv_rf_customer_config.bt_tx_idx3_pwr;
    btdrv_txpwr_conv_tbl[4] = btdrv_rf_customer_config.bt_tx_idx3_pwr;
    btdrv_txpwr_conv_tbl[5] = btdrv_rf_customer_config.bt_tx_idx3_pwr;
    btdrv_txpwr_conv_tbl[6] = btdrv_rf_customer_config.bt_tx_idx3_pwr;
    btdrv_txpwr_conv_tbl[7] = btdrv_rf_customer_config.bt_tx_idx3_pwr;

    btdrv_ble_txpwr_conv_tbl[0] = btdrv_rf_customer_config.le_tx_idx0_pwr;
    btdrv_ble_txpwr_conv_tbl[1] = btdrv_rf_customer_config.le_tx_idx1_pwr;
    btdrv_ble_txpwr_conv_tbl[2] = btdrv_rf_customer_config.le_tx_idx2_pwr;
    btdrv_ble_txpwr_conv_tbl[3] = btdrv_rf_customer_config.le_tx_idx3_pwr;
    btdrv_ble_txpwr_conv_tbl[4] = btdrv_rf_customer_config.le_tx_idx3_pwr;
    btdrv_ble_txpwr_conv_tbl[5] = btdrv_rf_customer_config.le_tx_idx3_pwr;
    btdrv_ble_txpwr_conv_tbl[6] = btdrv_rf_customer_config.le_tx_idx3_pwr;
    btdrv_ble_txpwr_conv_tbl[7] = btdrv_rf_customer_config.le_tx_idx3_pwr;
}

void bt_drv_rf_set_customer_config(struct btdrv_customer_rf_config_t* config)
{
    memset(&btdrv_rf_customer_config, 0, sizeof(struct btdrv_customer_rf_config_t));
    if(config !=NULL)
    {
        btdrv_rf_customer_config = *config;
    }
    else
    {
        bt_drv_rf_sdk_init();
    }
}

static void bt_rf_txpwr_reg_get(uint8_t idx, uint16_t *txgain_tbl0, uint16_t *txgain_tbl1, uint16_t *txgain_tbl2)
{
    switch(idx)
    {
        case TX_PWR_IDX_0:
            *txgain_tbl0 = RF_BT_TX_PWR_TBL0_IDX0_REG;
            *txgain_tbl1 = RF_BT_TX_PWR_TBL1_IDX0_REG;
            *txgain_tbl2 = RF_BT_TX_PWR_TBL2_IDX0_REG;
            break;
        case TX_PWR_IDX_1:
            *txgain_tbl0 = RF_BT_TX_PWR_TBL0_IDX1_REG;
            *txgain_tbl1 = RF_BT_TX_PWR_TBL1_IDX1_REG;
            *txgain_tbl2 = RF_BT_TX_PWR_TBL2_IDX1_REG;
            break;
        case TX_PWR_IDX_2:
            *txgain_tbl0 = RF_BT_TX_PWR_TBL0_IDX2_REG;
            *txgain_tbl1 = RF_BT_TX_PWR_TBL1_IDX2_REG;
            *txgain_tbl2 = RF_BT_TX_PWR_TBL2_IDX2_REG;
            break;
        case TX_PWR_IDX_3:
            *txgain_tbl0 = RF_BT_TX_PWR_TBL0_IDX3_REG;
            *txgain_tbl1 = RF_BT_TX_PWR_TBL1_IDX3_REG;
            *txgain_tbl2 = RF_BT_TX_PWR_TBL2_IDX3_REG;
            break;
        case TX_PWR_IDX_4:
            *txgain_tbl0 = RF_BT_TX_PWR_TBL0_IDX4_REG;
            *txgain_tbl1 = RF_BT_TX_PWR_TBL1_IDX4_REG;
            *txgain_tbl2 = RF_BT_TX_PWR_TBL2_IDX4_REG;
            break;
        case TX_PWR_IDX_PAGE:
            *txgain_tbl0 = RF_BT_TX_PWR_TBL0_IDX6_REG;
            *txgain_tbl1 = RF_BT_TX_PWR_TBL1_IDX6_REG;
            *txgain_tbl2 = RF_BT_TX_PWR_TBL2_IDX6_REG;
            break;
        default:
            ASSERT(0, "BT_DRV:ERROR tx pwr idx=%d", idx);
            break;
    }
}

static void ble_rf_txpwr_reg_get(uint8_t idx, uint16_t *txgain_tbl0, uint16_t *txgain_tbl1, uint16_t *txgain_tbl2)
{
    switch(idx)
    {
        case TX_PWR_IDX_0:
            *txgain_tbl0 = RF_LE_TX_PWR_TBL0_IDX0_REG;
            *txgain_tbl1 = RF_LE_TX_PWR_TBL1_IDX0_REG;
            *txgain_tbl2 = RF_LE_TX_PWR_TBL2_IDX0_REG;
            break;
        case TX_PWR_IDX_1:
            *txgain_tbl0 = RF_LE_TX_PWR_TBL0_IDX1_REG;
            *txgain_tbl1 = RF_LE_TX_PWR_TBL1_IDX1_REG;
            *txgain_tbl2 = RF_LE_TX_PWR_TBL2_IDX1_REG;
            break;
        case TX_PWR_IDX_2:
            *txgain_tbl0 = RF_LE_TX_PWR_TBL0_IDX2_REG;
            *txgain_tbl1 = RF_LE_TX_PWR_TBL1_IDX2_REG;
            *txgain_tbl2 = RF_LE_TX_PWR_TBL2_IDX2_REG;
            break;
        case TX_PWR_IDX_3:
            *txgain_tbl0 = RF_LE_TX_PWR_TBL0_IDX3_REG;
            *txgain_tbl1 = RF_LE_TX_PWR_TBL1_IDX3_REG;
            *txgain_tbl2 = RF_LE_TX_PWR_TBL2_IDX3_REG;
            break;
        default:
            ASSERT(0, "BT_DRV:ERROR tx pwr idx=%d", idx);
            break;
    }
}

int rf_reg_txpwr_val_get(int8_t txpwr_dbm, uint16_t *txgain_tbl0, uint16_t *txgain_tbl1, uint16_t *txgain_tbl2, uint8_t *pa_sel)
{
    uint32_t nRet = -1;
    uint16_t i = 0;
    uint16_t tbl_size;
    tbl_size = sizeof(tx_pwr_select)/sizeof(tx_pwr_select[0]);
    for(i=0; i< tbl_size; i++){
        if(tx_pwr_select[i].pwr_dbm == txpwr_dbm){
            *txgain_tbl0 = tx_pwr_select[i].tx_gain_tbl0;
            *txgain_tbl1 = tx_pwr_select[i].tx_gain_tbl1;
            *txgain_tbl2 = tx_pwr_select[i].tx_gain_tbl2;
            *pa_sel = tx_pwr_select[i].pa_sel;
            nRet = 0;
            break;
        }
    }
    return nRet;
}

static void bt_drv_rf_bt_txpwr_set(uint8_t idx, int8_t val_in_dbm)
{
    uint8_t pa_sel = 0;
    uint16_t txgain_tbl0_val = 0;
    uint16_t txgain_tbl1_val = 0;
    uint16_t txgain_tbl2_val = 0;
    uint16_t txgain_tbl0_addr = 0;
    uint16_t txgain_tbl1_addr = 0;
    uint16_t txgain_tbl2_addr = 0;
    int nRet = 0;

    bt_rf_txpwr_reg_get(idx, &txgain_tbl0_addr, &txgain_tbl1_addr, &txgain_tbl2_addr);
    nRet = rf_reg_txpwr_val_get(val_in_dbm, &txgain_tbl0_val, &txgain_tbl1_val, &txgain_tbl2_val, &pa_sel);
    ASSERT(!nRet, "BT_DRV:pls write correct tx pwr");

    btdrv_write_rf_reg(txgain_tbl0_addr, txgain_tbl0_val);
    btdrv_write_rf_reg(txgain_tbl1_addr, txgain_tbl1_val);
    btdrv_write_rf_reg(txgain_tbl2_addr, txgain_tbl2_val);

    BT_DRV_TRACE(2, "BT:TX pwr in:%d dbm,[reg0=0x%x,out:0x%04x][reg1=0x%x,out:0x%04x][reg1=0x%x,out:0x%04x]",
    val_in_dbm, txgain_tbl0_addr, txgain_tbl0_val, txgain_tbl1_addr, txgain_tbl1_val, txgain_tbl2_addr, txgain_tbl2_val);
}

static void bt_drv_rf_ble_txpwr_set(uint8_t idx, int8_t val_in_dbm)
{
    uint8_t pa_sel = 0;
    uint16_t txgain_tbl0_val = 0;
    uint16_t txgain_tbl1_val = 0;
    uint16_t txgain_tbl2_val = 0;
    uint16_t txgain_tbl0_addr = 0;
    uint16_t txgain_tbl1_addr = 0;
    uint16_t txgain_tbl2_addr = 0;
    int nRet = 0;

    ble_rf_txpwr_reg_get(idx, &txgain_tbl0_addr, &txgain_tbl1_addr, &txgain_tbl2_addr);
    nRet = rf_reg_txpwr_val_get(val_in_dbm, &txgain_tbl0_val, &txgain_tbl1_val, &txgain_tbl2_val, &pa_sel);
    ASSERT(!nRet, "BT_DRV:pls write correct tx pwr");

    btdrv_write_rf_reg(txgain_tbl0_addr, txgain_tbl0_val);
    btdrv_write_rf_reg(txgain_tbl1_addr, txgain_tbl1_val);
    btdrv_write_rf_reg(txgain_tbl2_addr, txgain_tbl2_val);

    BT_DRV_TRACE(2, "BT:TX pwr in:%d dbm,[reg0=0x%x,out:0x%04x][reg1=0x%x,out:0x%04x][reg1=0x%x,out:0x%04x]",
    val_in_dbm, txgain_tbl0_addr, txgain_tbl0_val, txgain_tbl1_addr, txgain_tbl1_val, txgain_tbl2_addr, txgain_tbl2_val);
}

void bt_drv_tx_pwr_init(void)
{
    struct btdrv_customer_rf_config_t cfg = btdrv_rf_customer_config;

    if(cfg.config_tx_pwr_en == true)
    {
        //BT
        bt_drv_rf_bt_txpwr_set(TX_PWR_IDX_0, cfg.bt_tx_idx0_pwr);
        bt_drv_rf_bt_txpwr_set(TX_PWR_IDX_1, cfg.bt_tx_idx1_pwr);
        bt_drv_rf_bt_txpwr_set(TX_PWR_IDX_2, cfg.bt_tx_idx2_pwr);
        bt_drv_rf_bt_txpwr_set(TX_PWR_IDX_3, cfg.bt_tx_idx3_pwr);
        bt_drv_rf_bt_txpwr_set(TX_PWR_IDX_PAGE, cfg.bt_tx_page_pwr);
        //BLE
        bt_drv_rf_ble_txpwr_set(TX_PWR_IDX_0, cfg.le_tx_idx0_pwr);
        bt_drv_rf_ble_txpwr_set(TX_PWR_IDX_1, cfg.le_tx_idx1_pwr);
        bt_drv_rf_ble_txpwr_set(TX_PWR_IDX_2, cfg.le_tx_idx2_pwr);
        bt_drv_rf_ble_txpwr_set(TX_PWR_IDX_3, cfg.le_tx_idx3_pwr);
    }
    else
    {
        ASSERT(0, "BT_DRV:pls check TX pwr config");
    }
}

void bt_drv_rf_init_xtal_fcap(void)
{
    unsigned int xtal_fcap_temp = DEFAULT_XTAL_FCAP;

    if(btdrv_rf_customer_config.config_xtal_en == true)
    {
        xtal_fcap_temp = btdrv_rf_customer_config.xtal_cap_val;
        BT_DRV_TRACE(1,"btdrv customer set xtal fcap=0x%x", xtal_fcap_temp);
    }
    else
    {
#ifdef __RF_INIT_XTAL_CAP_FROM_NV__
        if (!nvrec_dev_get_xtal_fcap(&xtal_fcap_temp))
        {
            BT_DRV_TRACE(2,"%s xtal cap=0x%x", __func__, xtal_fcap_temp);
        }
        else
#endif
        {
            xtal_fcap_temp = DEFAULT_XTAL_FCAP;
            BT_DRV_TRACE(1,"%s failed", __func__);
        }
    }

    BT_DRV_TRACE(1,"%s val: 0x%x", __func__, xtal_fcap_temp);

    btdrv_rf_init_xtal_fcap(xtal_fcap_temp);
}

void bt_drv_ble_adv_txpwr_via_advhdl(uint8_t adv_hdl, uint8_t idx, int8_t txpwr_dbm)
{

    BT_DRV_TRACE(2,"set adv_txpwr_via_advhdl hdl 0x%02x idx %d dbm %d", adv_hdl, idx, txpwr_dbm);

    if(idx > 5)
    {
        return;
    }

    if(txpwr_dbm <-3)
    {
        txpwr_dbm += 18;
        bt_drv_reg_op_low_txpwr(true, FACTOR_ATTENUATION_18DBM, BLE_ADV_LOW_TXPWR_MODE, adv_hdl);
    }
    else
    {
        bt_drv_reg_op_low_txpwr(false, FACTOR_ATTENUATION_18DBM, BLE_ADV_LOW_TXPWR_MODE, adv_hdl);
    }

    bt_drv_rf_ble_txpwr_set(idx, txpwr_dbm);
}

#ifdef __RECONN_CONFIG_FOR_TDD_NOISE__
static void bt_drv_decrease_page_pa_current(void)
{
    uint16_t page_idx_reg_addr = 0;

    page_idx_reg_addr = RF_BT_TX_PWR_TBL1_IDX0_REG + TX_PWR_IDX_PAGE;

    BTRF_REG_SET_FIELD(page_idx_reg_addr, 0x1F, 4, 0x0);
}
#endif

void btdrv_rf_register_init(const uint16_t rf_init_tbl_p[][3], uint16_t tbl_size)
{
    uint16_t i;
    uint16_t value = 0;

    for (i=0; i< tbl_size; i++) {
        btdrv_write_rf_reg(rf_init_tbl_p[i][0],rf_init_tbl_p[i][1]);
        if(rf_init_tbl_p[i][2] !=0)
            btdrv_delay(rf_init_tbl_p[i][2]);//delay
        btdrv_read_rf_reg(rf_init_tbl_p[i][0],&value);
        BT_DRV_TRACE(2,"reg=%x,v=%x",rf_init_tbl_p[i][0],value );
    }
}

uint8_t btdrv_rf_init(void)
{
    uint16_t value = 0;
    const uint16_t (*rf_init_tbl_p)[3] = NULL;
    uint32_t tbl_size = 0;
    uint8_t i;

    //system RF register init
    for (i = 0; i < ARRAY_SIZE(rf_sys_init_tbl); i++){
        btdrv_read_rf_reg(rf_sys_init_tbl[i].reg, &value);
        value = (value & ~rf_sys_init_tbl[i].mask) | (rf_sys_init_tbl[i].set & rf_sys_init_tbl[i].mask);
        if (rf_sys_init_tbl[i].delay){
            btdrv_delay(rf_sys_init_tbl[i].delay);
        }
        btdrv_write_rf_reg(rf_sys_init_tbl[i].reg, value);
    }
    //common RF register init
    rf_init_tbl_p = &rf_init_tbl_1[0];
    tbl_size = sizeof(rf_init_tbl_1)/sizeof(rf_init_tbl_1[0]);
    btdrv_rf_register_init(rf_init_tbl_p, tbl_size);

#ifdef __HW_AGC__
    rf_init_tbl_p = &rf_init_hw_agc_config[0];
    tbl_size = sizeof(rf_init_hw_agc_config)/sizeof(rf_init_hw_agc_config[0]);
    btdrv_rf_register_init(rf_init_tbl_p, tbl_size);
#endif
#ifdef __NEW_SWAGC_MODE__
    //open rf BT sync agc mode
    bt_drv_rf_set_bt_sync_agc_enable(true);
#endif
    //customer option
    bt_drv_rf_set_customer_config(NULL);
    //TX power init
    bt_drv_tx_pwr_init();
    //Pa current set 0
#ifdef __RECONN_CONFIG_FOR_TDD_NOISE__
    bt_drv_decrease_page_pa_current();
#endif
    //XTAL cap init
    bt_drv_rf_init_xtal_fcap();

    return 0;
}

#ifdef __NEW_SWAGC_MODE__
void bt_drv_rf_set_bt_sync_agc_enable(bool enable)
{
    uint16_t val = 0;
    btdrv_read_rf_reg(RF_REG_SYNC_AGC, &val);
    if(enable)
    {
        //open rf new sync agc mode
        val |= (1<<3);
    }
    else
    {
        //close rf new sync agc mode
        val &= ~(1<<3);
    }
    btdrv_write_rf_reg(RF_REG_SYNC_AGC, val);
}
#endif

static void btdrv_tx_power_rf_comp(const uint16_t rf_tx_gain)
{
    uint16_t read_val = 0;
    const uint16_t bt_gain_addr_base = RF_BT_TX_PWR_TBL1_IDX0_REG;
    const uint16_t ble_gain_addr_base = RF_LE_TX_PWR_TBL1_IDX0_REG;
    uint16_t tx_gain_val = 0;

    for (uint8_t i = 0; i <= BT_MAX_TX_PWR_IDX; i++) {
        //bt
        btdrv_read_rf_reg((bt_gain_addr_base + i), &read_val);
        tx_gain_val = (read_val >> 4) & 0x1F;
        read_val = (uint16_t)((float)rf_tx_gain * (float)tx_gain_val / 21.0);   // The power calibration is based on 14dbm, and the other idx are linearly mapped
        BTRF_REG_SET_FIELD((bt_gain_addr_base + i), 0x1F, 4, read_val);
        //ble
        btdrv_read_rf_reg((ble_gain_addr_base + i), &read_val);
        tx_gain_val = (read_val >> 4) & 0x1F;
        read_val = (uint16_t)((float)rf_tx_gain * (float)tx_gain_val / 21.0);
        BTRF_REG_SET_FIELD((ble_gain_addr_base + i), 0x1F, 4, read_val);
    }
}

static void btdrv_tx_power_dig_comp(uint32_t dig_gain_offset, uint32_t edr_dig_gain_offset)
{
    uint32_t dig_gain = 0;
    uint32_t edr_dig_gain = 0;

    BTDIGITAL_REG_GET_FIELD(BESMDM_GFSK_DSG_NOM_ADDR, 0x3FF, 0, dig_gain);
    BTDIGITAL_REG_GET_FIELD(BESMDM_EDR_GFSK_DSG_NOM_ADDR, 0x3FF, 0, edr_dig_gain);
    dig_gain = dig_gain - dig_gain_offset;
    edr_dig_gain = edr_dig_gain - edr_dig_gain_offset;
    BTDIGITAL_REG_SET_FIELD(BESMDM_GFSK_DSG_NOM_ADDR, 0x3FF, 0, dig_gain);
    BTDIGITAL_REG_SET_FIELD(BESMDM_GFSK_DSG_NOM_ADDR, 0x3FF, 10, dig_gain);
    BTDIGITAL_REG_SET_FIELD(BESMDM_DPSK_DSG_NOM_ADDR, 0x3FF, 0, dig_gain);
    BTDIGITAL_REG_SET_FIELD(BESMDM_EDR_GFSK_DSG_NOM_ADDR, 0x3FF, 0, edr_dig_gain);
    BTDIGITAL_REG_SET_FIELD(BESMDM_EDR_GFSK_DSG_NOM_ADDR, 0x3FF, 10, edr_dig_gain);
}

void btdrv_txpower_calib(void)
{
    uint16_t read_val = 0;
    int32_t bt_pwr_freql, bt_pwr_freqm, bt_pwr_freqh, bt_pwr_sum;
    int bt_pwr_avg = 0;

    pmu_get_efuse(PMU_EFUSE_PAGE_SW_CFG, &read_val);
    TRACE(2, "%s efuse: 0x%x",__func__, read_val);
    bt_pwr_freql = read_val & 0x7;
    bt_pwr_freqm = (read_val >> 4) & 0x7;
    bt_pwr_freqh = (read_val >> 8) & 0x7;

    bt_pwr_freql = getbit(read_val, 3)  ? bt_pwr_freql : (-bt_pwr_freql);
    bt_pwr_freqm = getbit(read_val, 7)  ? bt_pwr_freqm : (-bt_pwr_freqm);
    bt_pwr_freqh = getbit(read_val, 11) ? bt_pwr_freqh : (-bt_pwr_freqh);

    bt_pwr_sum = bt_pwr_freql + bt_pwr_freqm + bt_pwr_freqh;
    if (bt_pwr_sum >= 0) {
        bt_pwr_avg = (int32_t)((bt_pwr_sum / 3.0f) + 0.5f) ;
    } else {
        bt_pwr_avg = (int32_t)((bt_pwr_sum / 3.0f) - 0.5f) ;
    }

    BT_DRV_TRACE(1, "%s  bt_pwr_avg: %d", __func__, bt_pwr_avg);

    if (bt_pwr_avg < 0) {
        if (bt_pwr_avg == -1) {
            btdrv_tx_power_rf_comp(0x1A);
        } else if (bt_pwr_avg <= -2) {
            btdrv_tx_power_rf_comp(0x1F);
        }
    } else if (bt_pwr_avg > 0) {
        if (bt_pwr_avg == 1) {
            btdrv_tx_power_dig_comp(0x4, 0x3);
        } else if (bt_pwr_avg == 2){
            btdrv_tx_power_dig_comp(0x8, 0x6);
        } else if (bt_pwr_avg == 3){
            btdrv_tx_power_dig_comp(0xC, 0x9);
        } else if (bt_pwr_avg >= 4){
            btdrv_tx_power_dig_comp(0xF, 0xA);
        }
    }
}
