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
#include "bt_drv_1503p_internal.h"
#include "bt_drv_1503p_config.h"
#include "bt_1503p_reg_map.h"
#include CHIP_SPECIFIC_HDR(bt_drv_modem_reg_map)
#define XTAL_FCAP_MAX_STEP                      (0x40)
#define XTAL_FCAP_NORMAL_SHIFT                  0
#define XTAL_FCAP_NORMAL_MASK                   (0x1FF << XTAL_FCAP_NORMAL_SHIFT)
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
    {15,  0x4204, 0x096F, 0x0000, TX_PWR_1P8_PA},   //15dbm
    {14,  0x4204, 0x08CE, 0x0000, TX_PWR_1P8_PA},   //14dbm
    {13,  0x4204, 0x0907, 0x0000, TX_PWR_1P8_PA},   //13dbm
    {12,  0x4204, 0x0905, 0x0000, TX_PWR_1P8_PA},   //12dbm
    {11,  0x4204, 0x0904, 0x0000, TX_PWR_1P8_PA},   //11dbm
    {10,  0x4204, 0x090F, 0x0001, TX_PWR_1P8_PA},   //10dbm
    {9,   0x4204, 0x0909, 0x0001, TX_PWR_1P8_PA},   //9dbm
    {8,   0x4204, 0x0907, 0x0001, TX_PWR_1P8_PA},   //8dbm
    {7,   0x4204, 0x0905, 0x0001, TX_PWR_1P8_PA},   //7dbm
    {6,   0x4204, 0x0904, 0x0001, TX_PWR_1P8_PA},   //6dbm
    {5,   0x4204, 0x08C4, 0x0001, TX_PWR_1P8_PA},   //5dbm
    {4,   0x4204, 0x08C3, 0x0001, TX_PWR_1P8_PA},   //4dbm
    {3,   0x4204, 0x08C5, 0x0002, TX_PWR_1P8_PA},   //3dbm
    {2,   0x4204, 0x08C4, 0x0002, TX_PWR_1P8_PA},   //2dbm
    {1,   0x4204, 0x08C3, 0x0002, TX_PWR_1P8_PA},   //1dbm
    {0,   0x4204, 0x0893, 0x0002, TX_PWR_1P8_PA},   //0dbm
    {-1,  0x4204, 0x0894, 0x0003, TX_PWR_1P8_PA},  //-1dbm
    {-2,  0x4204, 0x0893, 0x0003, TX_PWR_1P8_PA},  //-2dbm
    {-3,  0x4204, 0x0894, 0x0004, TX_PWR_1P8_PA},  //-3dbm
    {-4,  0x4204, 0x0897, 0x0006, TX_PWR_1P8_PA},  //-4dbm
    {-5,  0x4204, 0x0895, 0x0006, TX_PWR_1P8_PA},  //-5dbm
    {-6,  0x4204, 0x0894, 0x0006, TX_PWR_1P8_PA},  //-6dbm
    {-7,  0x4204, 0x0893, 0x0006, TX_PWR_1P8_PA},  //-7dbm
    {-8,  0x4204, 0x0893, 0x0007, TX_PWR_1P8_PA},  //-8dbm
    {-9,  0x4204, 0x0892, 0x0007, TX_PWR_1P8_PA},  //-9dbm
    {-10, 0x4204, 0x0892, 0x0007, TX_PWR_1P8_PA},  //-10dbm
};

static const struct RF_SYS_INIT_ITEM rf_sys_init_tbl[] =
{
};

const uint16_t rf_init_tbl_1[][3] =
{
    {0x0001, 0x0101, 0},    //timing_t1
    {0x0002, 0x0c0c, 0},    //timing_t2
    {0x0003, 0x0e0d, 0},    //timing_t3
    {0x0004, 0x131a, 0},    //timing_t4
    {0x0005, 0x0008, 0},    //timing_t5
    {0x0008, 0x0231, 0},    //timing_td
    {0x0009, 0x4456, 0},    //precharge time
    {0x000A, 0x0C01, 0},    //bt_timing_r1=3
    {0x000F, 0x0000, 0},    //pa pwd
    {0x0011, 0x4000, 0},    // lna mixer first en_s dr=0; bit2=0
    {0x0013, 0x0000, 0},    //capbank d
    {0x0016, 0x0951, 0},    ///int_dec_sel
    {0x0018, 0x0160, 0},
    {0x0019, 0xC140, 0},
    {0x001B, 0x0000, 0},    // vco ictrl
    {0x0022, 0x5088, 0},
    {0x0023, 0x00B0, 0},    // enable ble tx gain tbl
    {0x0024, 0x0000, 0},
    {0x0025, 0x8020, 0},
    {0x0026, 0x7090, 0},
    {0x0028, 0x4022, 0},    // i2v cap dr=0; bit 15=0
    {0x0029, 0x8810, 0},    //init1，rx vco current
    {0x002B, 0x0740, 0},    //adc rstb
    {0x002c, 0x240e, 0},    // adc i2v gain dr=0; bit 0 9=0
    {0x002D, 0x8D90, 0},    // adc i2v gain dr=0; bit 0 9=0
    {0x002E, 0x1820, 0},
    {0x0030, 0x0000, 0},    // lna_mixer first en dr=0; bit12=0
    {0x0049, 0x401F, 0},
    {0x004A, 0x0400, 0},
    {0x004C, 0x2D6E, 0},    // mixer bias_s, rc_s, lna ic res_s, ic_t_s, hg_s dr=0; bit 0 4 9 14 15=0
    {0x004D, 0x2c80, 0},    // lna hg en_s, lg rin_s, ldo res_s dr=0; bit 9 5 0 dr=0
    {0x004E, 0x1000, 0},    // adc gain_s,vco div2 dr=0; bit 4 9=0
    {0x004F, 0x3300, 0},
    {0x0051, 0x0200, 0},    // lna hg en dr=0; bit15=0
    {0x0052, 0xB776, 0},    // lna ic res, hg, ic_t dr=0;  bit0 7 11=0
    {0x0081, 0x8002, 0},    // i2v rin dr=0; bit 12=0
    {0x0082, 0x129B, 0},
    {0x008C, 0x0000, 0},    //dac gain
    {0x008D, 0x0020, 0},    //dac_comp_on
    {0x008F, 0x090B, 0},    //dac_vcm_2nd
    {0X0090, 0X3E58, 0},    //bt_dac_ldo_vct
    {0x0091, 0x0A48, 0},    // adc_sel_op1_stb  adc rstb dac tmp0 dr=0; bit 14 15=0
    {0x0092, 0x83C1, 0},    // mixer bias dr=0; bit 11=0
    {0x0095, 0x0838, 0},    // adc rstb dac tmp=0; bit 14=0
    {0x0097, 0x7FF0, 0},    //adc gain
    {0x0098, 0x0000, 0},    // duty 1m
    {0x0099, 0x301B, 0},
    {0x009A, 0X8088, 0},    //rfvco_vres_txbuf
    {0x009B, 0x570F, 0},    // vcorxbuf dr=0 bit15=0
    {0x009C, 0x0004, 0},
    {0x009D, 0x2604, 0},
    {0x009E, 0x3F68, 0},    //iofst
    {0x009F, 0xC911, 0},    //dac tst
    {0X00A0, 0x1108, 0},    //idn
    {0x00A1, 0x7D88, 0},
    {0x00A2, 0x15E8, 0},    //iup
    {0x00A3, 0x0733, 0},
    {0x00A4, 0x8086, 0},
    {0x00A5, 0x0000, 0},
    {0x00A6, 0xC809, 0},
    {0x00BF, 0x0004, 0},
    {0x00C0, 0x03FC, 0},    //DVDDRC
    {0x00C1, 0x2502, 0},
    {0x00C2, 0x4050, 0},    //adc corner
    {0x00CD, 0x0000, 0},    //reg_pa_sel_cap
    {0x00CE, 0x40a2, 0},    // lna ldo res dr=0; bit 6=0
    {0x00CF, 0x0400, 0},    // pa sel cap
    {0x00C2, 0x4050, 0},
    {0x00D3, 0x0121, 0},
    {0x00D4, 0x2000, 0},
    {0x00E0, 0x0012, 0},
    {0x00E5, 0xFF00, 0},
    {0x0101, 0x0099, 0},    // adc_att_sel_dr=0; bit5=0
    {0x0110, 0x0015, 0},    //reg_pa_i_sel
    {0x0122, 0x0000, 0},    // lna lg rin dr=0; bit13=0
    {0x011D, 0x0000, 0},
    {0x0145, 0xFF28, 0},    //adc inres
    {0x0147, 0x4065, 0},
    {0x015C, 0x0600, 0},
    {0x018C, 0x002F, 0},    //tmx gain
    {0x018D, 0x0001, 0},    //pa aux pwr en, pa main pwr en
    {0x01E6, 0x0041, 0},    //cnt_time0
    {0x01E7, 0x0c01, 0},    //cnt_time1
    {0x01E8, 0x0c02, 0},    //cnt_time2
    {0x01E9, 0x0220, 0},    //cnt_time3
    {0x01EA, 0x0208, 0},    //cnt_time4
    {0x01EB, 0x0208, 0},    //cnt_time5
    {0x0160, 0x0000, 0},    // adc_sel_dac_ic_dr=0; bit 7=0
    {0x0308, 0x0460, 0},
    {0x0309, 0x0040, 0},
    {0x030B, 0x0740, 0},
    {0x03E0, 0x0C38, 0},    //adc corner
    {0x03E1, 0x0180, 0},    //adc inres
    {0x03DE, 0xCC86, 0},    //ADC DAC IC
    {0x03DF, 0x3838, 0},    //ADC CORNER
    {0x03E2, 0x8686, 0},    //ADC STB

    // [swagc 0515]
    //rx_gain0
    {0x0041, 0x04A6, 0},
    {0x0042, 0x0490, 0},
    {0x0043, 0x0490, 0},
    {0x0044, 0x1450, 0},
    {0x0045, 0x040A, 0},
    {0x0046, 0x240A, 0},
    {0x0047, 0x5408, 0},
    {0x0048, 0x8408, 0},

    //rx_gain1
    {0x0039, 0x35F3, 0},
    {0x003A, 0x35F3, 0},
    {0x003B, 0x35F1, 0},
    {0x003C, 0x35F1, 0},
    {0x003D, 0x35F0, 0},
    {0x003E, 0x35F0, 0},
    {0x003F, 0x35F0, 0},
    {0x0040, 0x35F0, 0},

    //rx_gain2
    {0x012C, 0x4095, 0},
    {0x012D, 0x409B, 0},
    {0x012E, 0x409B, 0},
    {0x012F, 0x508B, 0},
    {0x0130, 0x508B, 0},
    {0x0131, 0x807B, 0},
    {0x0132, 0xB46B, 0},
    {0x0133, 0xB46B, 0},

    //rx_gain3
    {0x019E, 0x00C3, 0},
    {0x019F, 0xA0E3, 0},
    {0x01A0, 0xA0E3, 0},
    {0x01A1, 0xA0E1, 0},
    {0x01A2, 0x00E0, 0},
    {0x01A3, 0x00E0, 0},
    {0x01A4, 0x00E0, 0},
    {0x01A5, 0x00E0, 0},

    //rx_gain4
    {0x01B6, 0X8688, 0},
    {0x01B7, 0X8688, 0},
    {0x01B8, 0X8688, 0},
    {0x01B9, 0X8688, 0},
    {0x01BA, 0X8688, 0},
    {0x01BB, 0X8688, 0},
    {0x01BC, 0X8688, 0},
    {0x01BD, 0X8688, 0},

    //rx_gain5
    {0x01CE, 0x5C38, 0},
    {0x01CF, 0x5C38, 0},
    {0x01D0, 0x5C38, 0},
    {0x01D1, 0x5C38, 0},
    {0x01D2, 0x5C38, 0},
    {0x01D3, 0x5C38, 0},
    {0x01D4, 0x5C38, 0},
    {0x01D5, 0x5C38, 0},

    //bw2M_i2v_corner
    {0x0124, 0x0040, 0},
    {0x0125, 0x0040, 0},
    {0x0126, 0x0040, 0},
    {0x0127, 0x0050, 0},
    {0x0148, 0x0050, 0},
    {0x0149, 0x0080, 0},
    {0x014A, 0x00B4, 0},
    {0x014B, 0x00B4, 0},
    //bw2M_adc
    {0x0330, 0x0C38, 0},
    {0x0331, 0x0C38, 0},
    {0x0332, 0x0C38, 0},
    {0x0333, 0x0C38, 0},
    {0x0334, 0x0C38, 0},
    {0x0335, 0x0C38, 0},
    {0x0336, 0x0C38, 0},
    {0x0337, 0x0C38, 0},
};

#ifdef __HW_AGC__
const uint16_t rf_init_hw_agc_config[][3] =
{
    //hwagc_ctrl
    {0x0209, 0x8133, 0},
    {0x020B, 0x0180, 0},
    {0x020F, 0x0006, 0},
    {0x0210, 0x0258, 0},
    {0x0211, 0x0108, 0},
    {0x0215, 0x7F7F, 0},
    {0x0216, 0x7F7F, 0},
    {0x0217, 0x4C01, 0},
    {0x021C, 0x3B00, 0},
    {0x027D, 0x0001, 0},
    {0x027E, 0x11D4, 0},
    {0x027F, 0x11D4, 0},
    {0x0280, 0x11D4, 0},
    {0x0281, 0x00C0, 0},
    {0x0283, 0x0084, 0},
    {0x0284, 0x0042, 0},
    {0x0287, 0x013B, 0},
    {0x0288, 0x00AD, 0},
    {0x028D, 0x0042, 0},
    {0x028E, 0x0084, 0},
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
    {0x02DD, 0x5009, 0},

    //rx_gain0
    {0x021D, 0x04A6, 0},
    {0x0220, 0x0490, 0},
    {0x0223, 0x0490, 0},
    {0x0226, 0x1450, 0},
    {0x0229, 0x040A, 0},
    {0x022C, 0x240A, 0},
    {0x022F, 0x5408, 0},
    {0x0232, 0x8408, 0},

    //rx_gain1
    {0x021E, 0x35F3, 0},
    {0x0221, 0x35F3, 0},
    {0x0224, 0x35F1, 0},
    {0x0227, 0x35F1, 0},
    {0x022A, 0x35F0, 0},
    {0x022D, 0x35F0, 0},
    {0x0230, 0x35F0, 0},
    {0x0233, 0x35F0, 0},

    //rx_gain2
    {0x021F, 0x4095, 0},
    {0x0222, 0x409B, 0},
    {0x0225, 0x409B, 0},
    {0x0228, 0x508B, 0},
    {0x022B, 0x508B, 0},
    {0x022E, 0x807B, 0},
    {0x0231, 0xB46B, 0},
    {0x0234, 0xB46B, 0},

    //rx_gain3
    {0x02A9, 0x00C3, 0},
    {0x02AB, 0xA0E3, 0},
    {0x02AD, 0xA0E3, 0},
    {0x02AF, 0xA0E1, 0},
    {0x02B1, 0x00E0, 0},
    {0x02B3, 0x00E0, 0},
    {0x02B5, 0x00E0, 0},
    {0x02B7, 0x00E0, 0},

    //rx_gain4
    {0x02AA, 0x8688, 0},
    {0x02AC, 0x8688, 0},
    {0x02AE, 0x8688, 0},
    {0x02B0, 0x8688, 0},
    {0x02B2, 0x8688, 0},
    {0x02B4, 0x8688, 0},
    {0x02B6, 0x8688, 0},
    {0x02B8, 0x8688, 0},

    //rx_gain5
    {0x02E0, 0x5C38, 0},
    {0x02E1, 0x5C38, 0},
    {0x02E2, 0x5C38, 0},
    {0x02E3, 0x5C38, 0},
    {0x02E4, 0x5C38, 0},
    {0x02E5, 0x5C38, 0},
    {0x02E6, 0x5C38, 0},
    {0x02E7, 0x5C38, 0},

    //jp
    {0x0239, 0xAAA3, 0},
    {0x023A, 0xB4AF, 0},
    {0x023B, 0xBDB8, 0},
    {0x023C, 0x00C4, 0},

    //gain offset
    {0x0235, 0x3236, 0},
    {0x0236, 0x212A, 0},
    {0x0237, 0x151B, 0},
    {0x0238, 0x080F, 0},

    //jp3m
    {0x021A, 0x0009, 0},
};
#endif


#if (defined(__RF_APB_RESTORE_SUPPORT__) && defined(BT_LOG_POWEROFF))
void bt_drv_rf_inf_enable_auto_power_down(bool enable)
{
    uint16_t val = 0;

    btdrv_read_rf_reg(RF_REG_PU_RF_INF,&val);
    if(enable)
    {
        val &= ~(REG_PU_RF_INF_SW_DR|REG_ISO_EN_RF_INF_DR|REG_ISO_EN_TO_DIG_DR);
    }
    else
    {
        val |= REG_PU_RF_INF_SW_DR|REG_ISO_EN_RF_INF_DR|REG_ISO_EN_TO_DIG_DR;
    }

    btdrv_write_rf_reg(RF_REG_PU_RF_INF,val);
}
#endif //__RF_APB_RESTORE_SUPPORT__

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
    DRIVERS_TRACE(0,"SDK default rf config");
    btdrv_rf_customer_config.config_xtal_en = false;//get value from NV
    btdrv_rf_customer_config.xtal_cap_val = 0;//get value from NV

    //config TX power in dbm
    btdrv_rf_customer_config.config_tx_pwr_en = true;
    btdrv_rf_customer_config.bt_tx_page_pwr = TX_PWR_11DBM;//in dbm

    btdrv_rf_customer_config.bt_tx_idx3_pwr = TX_PWR_15DBM;//in dbm
    btdrv_rf_customer_config.bt_tx_idx2_pwr = TX_PWR_10DBM;//in dbm
    btdrv_rf_customer_config.bt_tx_idx1_pwr = TX_PWR_5DBM;//in dbm
    btdrv_rf_customer_config.bt_tx_idx0_pwr = TX_PWR_0DBM;//in dbm

    btdrv_rf_customer_config.le_tx_idx3_pwr = TX_PWR_15DBM;//in dbm
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

    DRIVERS_TRACE(2, "BT:TX pwr in:%d dbm,[reg0=0x%x,out:0x%04x][reg1=0x%x,out:0x%04x][reg1=0x%x,out:0x%04x]",
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

    DRIVERS_TRACE(2, "BT:TX pwr in:%d dbm,[reg0=0x%x,out:0x%04x][reg1=0x%x,out:0x%04x][reg1=0x%x,out:0x%04x]",
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
        DRIVERS_TRACE(1,"btdrv customer set xtal fcap=0x%x", xtal_fcap_temp);
    }
    else
    {
#ifdef __RF_INIT_XTAL_CAP_FROM_NV__
        if (!nvrec_dev_get_xtal_fcap(&xtal_fcap_temp))
        {
            DRIVERS_TRACE(2,"%s xtal cap=0x%x", __func__, xtal_fcap_temp);
        }
        else
#endif
        {
            xtal_fcap_temp = DEFAULT_XTAL_FCAP;
            DRIVERS_TRACE(1,"%s failed", __func__);
        }
    }

    btdrv_rf_init_xtal_fcap(xtal_fcap_temp);
}

void bt_drv_ble_adv_txpwr_via_advhdl(uint8_t adv_hdl, uint8_t idx, int8_t txpwr_dbm)
{

    DRIVERS_TRACE(2,"set adv_txpwr_via_advhdl hdl 0x%02x idx %d dbm %d", adv_hdl, idx, txpwr_dbm);

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
        DRIVERS_TRACE(2,"reg=%x,v=%x",rf_init_tbl_p[i][0],value);
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

void bt_drv_rf_set_ble_sync_agc_enable(bool enable)
{
    uint16_t val = 0;
    btdrv_read_rf_reg(RF_REG_SYNC_AGC,&val);
    if(enable)
    {
        //open rf new sync agc mode
        val |= (1<<2);
    }
    else
    {
        //close rf new sync agc mode
        val &= ~(1<<2);
    }
    btdrv_write_rf_reg(RF_REG_SYNC_AGC,val);
}

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
        read_val = (uint16_t)((float)rf_tx_gain * (float)tx_gain_val / 22.0);   // The power calibration is based on 14dbm, and the other idx are linearly mapped
        BTRF_REG_SET_FIELD((bt_gain_addr_base + i), 0x1F, 4, read_val);
        //ble
        btdrv_read_rf_reg((ble_gain_addr_base + i), &read_val);
        tx_gain_val = (read_val >> 4) & 0x1F;
        read_val = (uint16_t)((float)rf_tx_gain * (float)tx_gain_val / 22.0);
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

    pmu_get_efuse(PMU_EFUSE_PAGE_F, &read_val);
    DRIVERS_TRACE(2, "%s efuse: 0x%x",__func__, read_val);
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

    DRIVERS_TRACE(1, "%s  bt_pwr_avg: %d", __func__, bt_pwr_avg);

    if (bt_pwr_avg < 0) {
        if (bt_pwr_avg == -1) {
            btdrv_tx_power_rf_comp(0x16);
        } else if (bt_pwr_avg == -2) {
            btdrv_tx_power_rf_comp(0x1A);
        } else if (bt_pwr_avg <= -3) {
            btdrv_tx_power_rf_comp(0x1F);
        }
    } else if (bt_pwr_avg > 0) {
        if (bt_pwr_avg == 1) {
            btdrv_tx_power_dig_comp(0x5, 0x4);
        } else if (bt_pwr_avg == 2) {
            btdrv_tx_power_dig_comp(0xA, 0x7);
        } else if (bt_pwr_avg == 3) {
            btdrv_tx_power_dig_comp(0xF, 0xA);
        } else if (bt_pwr_avg >= 4) {
            btdrv_tx_power_dig_comp(0x14, 0xD);
        } else if (bt_pwr_avg >= 5) {
            btdrv_tx_power_dig_comp(0x18, 0x11);
        }
    }
}
