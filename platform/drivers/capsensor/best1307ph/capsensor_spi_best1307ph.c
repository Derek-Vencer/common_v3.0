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
#include "hal_trace.h"
#include "hal_timer.h"
#include "reg_capsensor_best1307ph.h"
#include "capsensor_driver.h"
#include "pmu.h"
#include "capsensor_driver_best1307ph.h"
#include "hal_sleep.h"
#include "cmsis_os.h"

enum CAP_IRQ_MODE_T{
    FP_MODE_INT = 0,
    PRESS_MODE_INT,
};

#define CAP_SAMP_FS_TO_REG_COEF             33
#define SDM_VAL_FULL_SCALE                  1048575.0f
#define SDM_PF_FULL_SCALE                   10.0f

#define cap_reg_read(reg,val)               hal_analogif_reg_read(CAP_REG(reg),val)
#define cap_reg_write(reg,val)              hal_analogif_reg_write(CAP_REG(reg),val)

#ifdef CAPSENSOR_READ_DATA_POLLING
static enum CAPSENSOR_READ_DATA_STATE_T cap_status = CAPSENSOR_READ_DATA_IDLE;

static void capsensor_set_cur_status(enum CAPSENSOR_READ_DATA_STATE_T status)
{
    cap_status = status;
}

static enum CAPSENSOR_READ_DATA_STATE_T capsensor_get_cur_status(void)
{
    return cap_status;
}
#endif

static void capsensor_sw_control_baseline_en(void)
{
    uint16_t value;

    for (int i = 0; i < 8; i++) {
        cap_reg_read(CAP_REG_0C + i, &value);
        value |= CDC_BASELINE_P_BIT_IN_CH0_DR;
        cap_reg_write(CAP_REG_0C + i, value);
    }
}

void capsensor_baseline_reg_read(uint32_t *baseline_value_p)
{
    uint16_t value = 0;
    uint16_t baseline_val[8] = {0};

    for (int i = 0; i < 8; i++) {
        cap_reg_read(CAP_REG_0C + i, &value);
        if (value & (CDC_BASELINE_P_BIT_IN_CH0_DR)) {
            baseline_val[i] = GET_BITFIELD(value, CDC_BASELINE_P_OUT_CH0_REG);
        } else {
            cap_reg_read(CAP_REG_14 + i, &value);
            baseline_val[i] = GET_BITFIELD(value, CDC_BASELINE_P_OUT_CH0_R);
        }
        baseline_value_p[i] = baseline_val[i];
    }
}

void capsensor_baseline_dr(uint32_t *baseline_value_p)
{
    for (int i = 0; i < 8; i++) {
        cap_reg_write(CAP_REG_0C + i, baseline_value_p[i]);
    }

    capsensor_sw_control_baseline_en();
}

void capsensor_fp_mode_int(void)
{
    uint16_t value = 0;

    cap_reg_read(CAP_REG_88, &value);
    value &= ~FP_MODE_RD_INT_MASK;
    cap_reg_write(CAP_REG_88, value);

    cap_reg_read(CAP_REG_87, &value);
    value |= FP_MODE_RD_INT_RAW_EN_REG;
    cap_reg_write(CAP_REG_87, value);
}

static void capsensor_fifo_num_and_capture_config(uint8_t cap_ch_num, uint8_t cap_conversion_num, uint16_t cap_samp_fs)
{
    uint16_t value = 0;

    cap_reg_read(CAP_REG_80, &value);
    value |= FIFO_DATA_SDM_ONLY;
    value &= ~FIFO_TAG_EN;
    cap_reg_write(CAP_REG_80, value);

    cap_reg_read(CAP_REG_02, &value);
    value |= CDC_RSTB;
    value = SET_BITFIELD(value, ACTIVE_CH_NUM, cap_ch_num);
    cap_reg_write(CAP_REG_02, value);

    cap_reg_read(CAP_REG_3E, &value);
    value = SET_BITFIELD(value, FIFO_DATA_NUM_TH, (cap_ch_num + 1) * cap_conversion_num);
    cap_reg_write(CAP_REG_3E, value);

    cap_reg_read(CAP_REG_3C, &value);
    value = SET_BITFIELD(value, LP_SLEEP_TIME, (cap_samp_fs * CAP_SAMP_FS_TO_REG_COEF));
    cap_reg_write(CAP_REG_3C, value);
}

static void capsensor_ch_map_config(uint8_t chan_num, uint8_t cap_ch_map)
{
    uint16_t value = 0;
    uint8_t ch_map[8]={0};

    for (uint8_t i = 0, j = 0; i < 8; i++) {
        if (cap_ch_map & (1 << i)) {
            ch_map[j++] = i;
            if (j >= (chan_num + 1)) {
                break;
            }
        }
    }

    cap_reg_read(CAP_REG_55, &value);
    value = SET_BITFIELD(value, CH_CONFIG_IDX_0, ch_map[0]);
    value = SET_BITFIELD(value, CH_CONFIG_IDX_1, ch_map[1]);
    cap_reg_write(CAP_REG_55, value);
    cap_reg_read(CAP_REG_56, &value);
    value = SET_BITFIELD(value, CH_CONFIG_IDX_2, ch_map[2]);
    value = SET_BITFIELD(value, CH_CONFIG_IDX_3, ch_map[3]);
    cap_reg_write(CAP_REG_56, value);
    cap_reg_read(CAP_REG_57, &value);
    value = SET_BITFIELD(value, CH_CONFIG_IDX_4, ch_map[4]);
    value = SET_BITFIELD(value, CH_CONFIG_IDX_5, ch_map[5]);
    cap_reg_write(CAP_REG_57, value);
    cap_reg_read(CAP_REG_58, &value);
    value = SET_BITFIELD(value, CH_CONFIG_IDX_6, ch_map[6]);
    value = SET_BITFIELD(value, CH_CONFIG_IDX_7, ch_map[7]);
    cap_reg_write(CAP_REG_58, value);
}

static void capsensor_down_sel_and_cin_config(void)
{
    uint16_t value = 0;

    cap_reg_read(CAP_REG_60, &value);
    value = SET_BITFIELD(value, DOWN_SEL_CH0, 0x2);
    value = SET_BITFIELD(value, DOWN_SEL_CH1, 0x2);
    value = SET_BITFIELD(value, DOWN_SEL_CH2, 0x2);
    value = SET_BITFIELD(value, DOWN_SEL_CH3, 0x2);
    value = SET_BITFIELD(value, DOWN_SEL_CH4, 0x2);
    cap_reg_write(CAP_REG_60, value);
    cap_reg_read(CAP_REG_61, &value);
    value = SET_BITFIELD(value, DOWN_SEL_CH5, 0x2);
    value = SET_BITFIELD(value, DOWN_SEL_CH6, 0x2);
    value = SET_BITFIELD(value, DOWN_SEL_CH7, 0x2);
    cap_reg_write(CAP_REG_61, value);

    cap_reg_read(CAP_REG_66, &value);
    value = SET_BITFIELD(value, CDC_SDM_OP_CIN1_CH0_REG, 0x3);
    value = SET_BITFIELD(value, CDC_SDM_OP_CIN1_CH1_REG, 0x3);
    value = SET_BITFIELD(value, CDC_SDM_OP_CIN1_CH2_REG, 0x3);
    value = SET_BITFIELD(value, CDC_SDM_OP_CIN1_CH3_REG, 0x3);
    cap_reg_write(CAP_REG_66, value);

    cap_reg_read(CAP_REG_67, &value);
    value = SET_BITFIELD(value, CDC_SDM_OP_CIN1_CH4_REG, 0x3);
    value = SET_BITFIELD(value, CDC_SDM_OP_CIN1_CH5_REG, 0x3);
    value = SET_BITFIELD(value, CDC_SDM_OP_CIN1_CH6_REG, 0x3);
    value = SET_BITFIELD(value, CDC_SDM_OP_CIN1_CH7_REG, 0x3);
    cap_reg_write(CAP_REG_67, value);
}

static void capsensor_fir_filter_and_smaple_point_config(void)
{
    uint16_t value = 0;

    value = (ADC_HBF1_BYPASS_CH0 |
             ADC_HBF1_BYPASS_CH1 |
             ADC_HBF1_BYPASS_CH2 |
             ADC_HBF1_BYPASS_CH3 |
             ADC_HBF1_BYPASS_CH4 |
             ADC_HBF1_BYPASS_CH5 |
             ADC_HBF1_BYPASS_CH6 |
             ADC_HBF1_BYPASS_CH7 |
             ADC_HBF2_BYPASS_CH0 |
             ADC_HBF2_BYPASS_CH1 |
             ADC_HBF2_BYPASS_CH2 |
             ADC_HBF2_BYPASS_CH3 |
             ADC_HBF2_BYPASS_CH4 |
             ADC_HBF2_BYPASS_CH5 |
             ADC_HBF2_BYPASS_CH6 |
             ADC_HBF2_BYPASS_CH7);
    cap_reg_write(CAP_REG_5F, value);

    cap_reg_read(CAP_REG_04, &value);
    value = SET_BITFIELD(value, CH_IDX_0_SAMPLE_POINT, 0x8);
    value = SET_BITFIELD(value, CH_IDX_1_SAMPLE_POINT, 0x8);
    cap_reg_write(CAP_REG_04, value);
    cap_reg_read(CAP_REG_05, &value);
    value = SET_BITFIELD(value, CH_IDX_2_SAMPLE_POINT, 0x8);
    value = SET_BITFIELD(value, CH_IDX_3_SAMPLE_POINT, 0x8);
    cap_reg_write(CAP_REG_05, value);
    cap_reg_read(CAP_REG_06, &value);
    value = SET_BITFIELD(value, CH_IDX_4_SAMPLE_POINT, 0x8);
    value = SET_BITFIELD(value, CH_IDX_5_SAMPLE_POINT, 0x8);
    cap_reg_write(CAP_REG_06, value);
    cap_reg_read(CAP_REG_07, &value);
    value = SET_BITFIELD(value, CH_IDX_6_SAMPLE_POINT, 0x8);
    value = SET_BITFIELD(value, CH_IDX_7_SAMPLE_POINT, 0x8);
    cap_reg_write(CAP_REG_07, value);

    cap_reg_read(CAP_REG_7F, &value);
    value = SET_BITFIELD(value, CH_SAMPLE_START, 0x7);
    cap_reg_write(CAP_REG_7F, value);
}

void capsensor_setup_cfg(struct CAPSENSOR_CFG_T *cap_cfg)
{
    uint16_t value = 0;

    cap_reg_read(CAP_REG_84, &value);
    value &= ~PU_OSC_CAP_SENSOR_DR;
    cap_reg_write(CAP_REG_84, value);

#ifdef CAPSENSOR_READ_DATA_POLLING
    cap_reg_read(CAP_REG_02, &value);
    value |= POWER_MODE_REG;
    cap_reg_write(CAP_REG_02, value);
#else
    capsensor_fp_mode_int();
#endif

    capsensor_fifo_num_and_capture_config(cap_cfg->ch_num, cap_cfg->conversion_num, cap_cfg->samp_fs);
    capsensor_ch_map_config(cap_cfg->ch_num, cap_cfg->ch_map);
    capsensor_down_sel_and_cin_config();
    capsensor_fir_filter_and_smaple_point_config();

    cap_reg_read(CAP_REG_8C, &value);
    value = SET_BITFIELD(value, CDC_CK_DELAY, 0x2);
    value |= (SAR_OVERFLOW_DR | FIFO_SAR_DATA_SEL);
    cap_reg_write(CAP_REG_8C, value);

    cap_reg_read(CAP_REG_8B, &value);
    uint8_t idle_chan = 0x0;
    for (uint8_t i = 0; i < 8; i++) {
        if (cap_cfg->ch_map & (1 << i)) {
            idle_chan |= (1 << i);
        }
    }
    value = SET_BITFIELD(value, CDC_PD_IDLE_CHAN, idle_chan);
    cap_reg_write(CAP_REG_8B, value);

    cap_reg_read(CAP_REG_8F, &value);
    value |= MEM_PG_PIN_CTRL_DR;
    cap_reg_write(CAP_REG_8F, value);

    cap_reg_read(CAP_REG_01, &value);
    value |= CDC_PU_VREF0P7 | CDC_PU_LDO | CDC_PU_CDC_REG;
    cap_reg_write(CAP_REG_01, value);

#ifdef CAPSENSOR_USE_RC
    analog_aud_codec_enable_capsensor_rc(true);
#endif

    hal_sys_timer_delay(MS_TO_TICKS(50));
    cap_reg_read(CAP_REG_01, &value);
    value |= FSM_EN;
    cap_reg_write(CAP_REG_01, value);
}

int get_sample(struct capsensor_sample_data *sample, int i)
{
    uint32_t sdm_low;
    uint32_t sdm_high;
    uint16_t value = 0;

    cap_reg_read(CAP_REG_63, &value);
    cap_reg_read(CAP_REG_43, &value);
    sample[i].ch = value & (0x7);
    sdm_low = (value >> 4) & 0xF;
    cap_reg_read(CAP_REG_44, &value);
    sdm_high = value;
    sample[i].sdm = (sdm_high << 4) | sdm_low;

    return 0;
}

int stop_hw_wr_start_sw_rd(uint8_t cap_num)
{
    uint16_t value = 0;
    uint16_t ptr_reg_value = 0;

    cap_reg_read(CAP_REG_54, &value);
    value |= FIFO_CLK_DR | FIFO_RD_DIRECTION_REG;
    value &= ~FIFO_RD_CLK_DR;
    cap_reg_write(CAP_REG_54, value);

    cap_reg_read(CAP_REG_82, &value);
    ptr_reg_value = GET_BITFIELD(value, WR_ADDR_PTR);

    value = (ptr_reg_value - cap_num) | RD_PTR_DR;
    cap_reg_write(CAP_REG_8A, value);

    cap_reg_write(CAP_REG_8A, value);
    value &= ~RD_PTR_DR;
    cap_reg_write(CAP_REG_8A, value);

    return 0;
}

int stop_sw_rd_start_hw_wr(void)
{
    uint16_t value = 0;

    cap_reg_read(CAP_REG_54, &value);
    value &= ~(FIFO_CLK_DR | FIFO_RD_START_REG);
    cap_reg_write(CAP_REG_54, value);

    return 0;
}

void select_sort_sample_data(struct capsensor_sample_data *sample, int chnum)
{
    for (int i = 0; i < chnum - 1; i++) {
        for (int j = i + 1; j < chnum; j++) {
            if (sample[j].ch < sample[i].ch) {
                struct capsensor_sample_data tmp = sample[i];
                sample[i] = sample[j];
                sample[j] = tmp;
            }
        }
    }
}

void count_sdm_value(struct capsensor_sample_data *sample_value)
{
    float sdm_value = 0;

    sdm_value += ((sample_value->sdm) / SDM_VAL_FULL_SCALE * SDM_PF_FULL_SCALE);
    sample_value->sdm_int = (int)sdm_value;
    sample_value->sdm_float = (int)((sdm_value - (float)(sample_value->sdm_int)) * 1000000);
}

void calculate_sample_data(struct capsensor_sample_data *sample, uint8_t cap_num)
{
    for (int data = 0; data < cap_num; data++) {
        count_sdm_value(&sample[data]);
    }
}

int capsensor_irq_type_judge(void)
{
    uint16_t value = 0;

    cap_reg_read(CAP_REG_88, &value);
    if (value & FP_MODE_RD_INT) {
        value |= FP_MODE_RD_INT_CLR_REG;
        cap_reg_write(CAP_REG_88, value);
        return FP_MODE_INT;
    } else if (value & PRESS_INT) {
        return PRESS_MODE_INT;
    } else {
        return -1;
    }
}

void capsensor_fifo_reset(void)
{
    uint16_t value = 0;

    cap_reg_read(CAP_REG_91, &value);
    value &= ~RESETN_FIFO_REG;
    cap_reg_write(CAP_REG_91, value);
    value |= RESETN_FIFO_REG;
    cap_reg_write(CAP_REG_91, value);
}

#ifdef CAPSENSOR_READ_DATA_POLLING
void capsensor_module_reset(void)
{
    cap_reg_write(CAP_REG_91, 0x1F);
    cap_reg_write(CAP_REG_91, 0x01);
    cap_reg_write(CAP_REG_91, 0x1E);
}
#endif

void capsensor_baseline_read(uint8_t num)
{
    uint16_t value = 0;
    uint16_t baseline_val[8] = {0};

    for (int i = 0; i < num; i++) {
        cap_reg_read(CAP_REG_0C + i, &value);
        if (value & CDC_BASELINE_P_BIT_IN_CH0_DR) {
            baseline_val[i] = GET_BITFIELD(value, CDC_BASELINE_P_OUT_CH0_REG);
        } else {
            cap_reg_read(CAP_REG_14 + i, &baseline_val[i]);
            baseline_val[i] |= CDC_BASELINE_P_BIT_IN_CH0_DR;
            cap_reg_write(CAP_REG_0C + i, baseline_val[i]);
        }
        DRIVERS_TRACE(0, "baseline[%d]:%x ", i, GET_BITFIELD(baseline_val[i], CDC_BASELINE_P_OUT_CH0_REG));
    }
}

void capsensor_pu_rc_dr(bool en)
{
    uint16_t value = 0;

    cap_reg_read(CAP_REG_84, &value);
    if (en) {
        value |= PU_OSC_CAP_SENSOR_DR | PU_OSC_CAP_SENSOR_REG;
    } else {
        value &= ~PU_OSC_CAP_SENSOR_DR;
    }
    cap_reg_write(CAP_REG_84, value);
}

#ifdef CAPSENSOR_READ_DATA_POLLING
void capsensor_wakeup_start_capture(void)
{
    uint16_t value = 0;

#ifdef CAPSENSOR_USE_RC
    capsensor_pu_rc_dr(true);
#endif
    capsensor_resume();
    cap_reg_read(CAP_REG_01, &value);
    value |= FSM_EN;
    cap_reg_write(CAP_REG_01, value);

    hal_chip_wake_lock(HAL_CHIP_WAKE_LOCK_USER_CAP);
}

void capsensor_read_capture_data(struct capsensor_sample_data *sample, int num)
{
    static bool baseline_dr_flag = false;

    stop_hw_wr_start_sw_rd(num);
    capsensor_set_cur_status(CAPSENSOR_READ_DATA_START);
    for (int i = 0; i < num; i++) {
        get_sample(sample, i);
    }
    stop_sw_rd_start_hw_wr();
    capsensor_set_cur_status(CAPSENSOR_READ_DATA_DONE);

    select_sort_sample_data(sample, num);
    if (baseline_dr_flag == false) {
        capsensor_baseline_read(num);
        baseline_dr_flag = true;
    }
#if 0
    calculate_sample_data(sample, num);

    for (int k = 0; k < num; k++) {
        DRIVERS_TRACE(0 ,"cap_data:%d\t%x\t%d.%06d",
        sample[k].ch, sample[k].sdm, sample[k].sdm_int, sample[k].sdm_float);
    }
    DRIVERS_TRACE(0, "\n");
#endif
}

void capsensor_state_machine_reset(void)
{
    capsensor_suspend();
    capsensor_module_reset();
#ifdef CAPSENSOR_USE_RC
    capsensor_pu_rc_dr(false);
#endif
    hal_chip_wake_unlock(HAL_CHIP_WAKE_LOCK_USER_CAP);
}

void capsensor_write_pointer_position(uint16_t *write_addr)
{
    uint16_t value = 0;

    *write_addr = 0;

    cap_reg_read(CAP_REG_82, &value);
    *write_addr = GET_BITFIELD(value, WR_ADDR_PTR);
}
#endif

int capsensor_get_raw_data(struct capsensor_sample_data *sample, int num)
{
#if defined (CAPSENSOR_READ_DATA_POLLING)
    int count = 0;
    uint16_t value = 0;
    uint16_t write_addr = 0;

    value = capsensor_clk_is_ready();
    if (!value) {
        capsensor_wakeup_start_capture();
        capsensor_set_cur_status(CAPSENSOR_READ_DATA_WAIT_READY);
    } else {
        do {
            capsensor_write_pointer_position(&write_addr);
            if (write_addr < num) {
                osDelay(1);
            }
        } while (write_addr < num && ++count < 15);

        capsensor_write_pointer_position(&write_addr);
        if (write_addr < num) {
            capsensor_set_cur_status(CAPSENSOR_READ_DATA_NOT_ENOUGH);
            DRIVERS_TRACE(0, "%s , capsensor samp num not enough !!!", __func__);
            return capsensor_get_cur_status();
        }

        capsensor_write_pointer_position(&write_addr);
        if (write_addr >= num) {
            capsensor_read_capture_data(sample, num);
            capsensor_set_cur_status(CAPSENSOR_READ_DATA_READY);
        } else {
            capsensor_set_cur_status(CAPSENSOR_READ_DATA_NOT_ENOUGH);
            DRIVERS_TRACE(0, "%s , capsensor samp num not enough !!!", __func__);
            return capsensor_get_cur_status();
        }
    }
    return capsensor_get_cur_status();
#else
    int cap_irq_mode = 0;
    static bool baseline_dr_flag = false;

    capsensor_pu_rc_dr(true);
    cap_irq_mode = capsensor_irq_type_judge();

    switch(cap_irq_mode) {
        case FP_MODE_INT:
            stop_hw_wr_start_sw_rd(num);
            for (int i = 0; i < num; i++) {
                get_sample(sample, i);
            }
            capsensor_fifo_reset();
            stop_sw_rd_start_hw_wr();

            select_sort_sample_data(sample, num);
            if (baseline_dr_flag == false) {
                capsensor_baseline_read(num);
                baseline_dr_flag = true;
            }
#if 0
            calculate_sample_data(sample, num);

            for (int k = 0; k < num; k++) {
                DRIVERS_TRACE(0 ,"cap_data:%d\t%x\t%d.%06d",
                sample[k].ch, sample[k].sdm, sample[k].sdm_int, sample[k].sdm_float);
            }
            DRIVERS_TRACE(0, "\n");
#endif
            break;

        case PRESS_MODE_INT:
            break;

        default:
            break;
    }
    capsensor_pu_rc_dr(false);

    return 0;
#endif
}

void capsensor_fp_mode_set_mask(void)
{
    uint16_t value = 0;

    cap_reg_read(CAP_REG_88, &value);
    value |= FP_MODE_RD_INT_MASK;
    cap_reg_write(CAP_REG_88, value);
}

void capsensor_fp_mode_clr_mask(void)
{
    uint16_t value = 0;

    cap_reg_read(CAP_REG_88, &value);
    value &= ~FP_MODE_RD_INT_MASK;
    cap_reg_write(CAP_REG_88, value);
}

void capsensor_fp_mode_clear_irq(void)
{
    uint16_t value = 0;

    cap_reg_read(CAP_REG_88, &value);
    value |= FP_MODE_RD_INT_CLR_REG;
    cap_reg_write(CAP_REG_88, value);
}
