#ifndef NOSTD
#ifdef TX_IQ_CAL
#include <math.h>
#include <stdio.h>

#include "hal_dma.h"
#include "hal_timer.h"
#include "hal_trace.h"

#include "string.h"
#include "besbt_string.h"
#include "bt_drv.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "hal_btdump.h"
#include "bt_drv_internal.h"
#include "bt_drv_1307ph_internal.h"
#include "bt_1307ph_reg_map.h"
#include CHIP_SPECIFIC_HDR(bt_drv_modem_reg_map)

// configure the bt iq calibration on system boot-up
#define MEM_1_BYBT_FOR_RXMEMDUMP   (EM_BASE_ADDR + 0x8000)
#define BESMDM_INT_TX_IQCOMP_2402   (BESMDM_INT_TX_IQCOMP_BASE + 0x8)

#define BAND_2G 0
#define BAND_5G 1

//DC IQ CALI DEBUG
// #define PWR_CALI_DEBUG 1
// #define CBANK_DEBUG 1
// #define DC_CALI_DEBUG 1
// #define IQ_CALI_DEBUG 1
#define DC_IQ_CALI_DEBUG_FE 1
// #define DPD_CALI_DEBUG 1
// #define RX_MEM_DUMP
// #define IQ_XCORR_DC_CALI

#define TX_IQCOMP_ADDR_2G           0xd0311000
#define TX_DCCOMP_ADDR_2G           0xd0350080
#define TX_DCCOMP_ADDR_2G_IF        0xd0350084
#define TX_GSG_GAIN                 0xd0350308
#define TX_IQ_COMP_EN_CTL           0xd0350080
#define TX_RX_DUMP_SEL              0xd0350218
#define TX_RX_SWITCH_CFG            0xd0220c00
#define RF_TX_DCCOMP_TMIX_EN        0x0010
#define RF_TX_PHASE_SHIFT_2G_0      0x00cd
#define RF_TX_PHASE_SHIFT_2G_DR     0x00cf


#define REG_BIT_ADDR    0
#define REG_BIT_HIGH    1
#define REG_BIT_LOW     2
#define REG_BIT_VAL     3

#define DC_CALI_PWR     0
#define IQ_CALI_PHI     1
#define IQ_CALI_PWR     2

#define DC_CALI_RAD     1
#define DC_CALI_TMX_OFF 0
#define DC_CALI_TMX_ON  1

static uint16_t dc_cali_rf_reg_back[][2] =
{
    {RF_TX_DCCOMP_TMIX_EN,  0},   // rx i2v_pu_and_dr
};

static uint32_t dc_cali_dig_reg_back[][2] =
{
    {0xd0350308, 0},   // gsg_nom_i[9:0]
    {0xd0350308, 0},   // gsg_nom_q[19:10]
};

static uint16_t iq_cali_rf_reg_back[][2] =
{
    {RF_TX_PHASE_SHIFT_2G_0,  0},
    {RF_TX_PHASE_SHIFT_2G_DR, 0},
};

static uint32_t iq_cali_dig_reg_back[][2] =
{
    {0xd0350308, 0},   // gsg_nom_i[9:0]
    {0xd0350308, 0},   // gsg_nom_q[19:10]
};

static uint16_t dc_rf_reg_tmx_off[][4] =
{
    {RF_TX_DCCOMP_TMIX_EN, 9, 8, 0x1},
};

static uint16_t dc_rf_reg_tmx_on[][4] =
{
    {RF_TX_DCCOMP_TMIX_EN, 9, 8, 0x0},
};

static uint16_t dc_rf_reg_ini[][4] ={};

static uint32_t dc_dig_reg_ini[][4] =
{
    {TX_GSG_GAIN, 19, 10, 0x00000000},
    {TX_GSG_GAIN,  9,  0, 0x00000000},   // gsg_nom_i[9:0]
    {TX_DCCOMP_ADDR_2G,14,  0, 0x00000000},
};

static uint16_t dpd_rf_reg_back[][2] =
{
    {0x00BF,  0},   // rx i2v_pu_and_dr
    {0x000F,  0},   // rx_lna_ldo_pu_and_dr
    {0x0030,  0},   // mix_first_en_dr and off
    {0x00A6,  0},   // rxmix_pu_and_dr
    {0x0052,  0},   // rxlna_hg_dr_and_en
    {0x0049,  0},   // rxlna_hg_en_tx
    {0x002B,  0},
    {0x002C,  0},   // rxbt_i2v_gain_dr_and_gain
    {0x100B,  0},   // reg_pa_short_dr_off
    {0x004B,  0},   // reg_bt_BB_txif_bypass
    {0x011D,  0},   // bt_lna_lg_rin_dr
    {0x0122,  0},
    {0x01EF,  0},   // bt_lna_fb_res
    {0x0030,  0},   // bt_lna_mixer_first_en
    {0x0024,  0},   // rxgain_dr
    {0x0209,  0},   // hwagc_fix_gain_index
    {0x0051,  0},   // bt_lna_hg_en_dr
    {0x0052,  0},
    {0x00C1,  0},
    {0x0110,  0},
    {0x00CD,  0},
    {0x00CF,  0},
    {0x00BD,  0},
};

static uint32_t dpd_dig_reg_back[][2] =
{
    {0xd0220C3C, 0},   // i2v_bypass_dr_off
    {0xd0350308, 0},   // gsg_nom_i[9:0]
    {0xd0350308, 0},   // gsg_nom_q[19:10]
    {0xd0350084, 0},
    {TX_RX_SWITCH_CFG, 0},
    {TX_RX_DUMP_SEL, 0},
};

static uint16_t dpd_rf_reg_ini[][4] =
{
    {0x00BF,  5,  4, 0x0003},   // rx i2v_pu_and_dr
    {0x000F, 11, 10, 0x0003},   // rx_lna_ldo_pu_and_dr
    // {0x0030, 13, 12, 0x0001},   // mix_first_en_dr and off
    {0x00A6, 13, 12, 0x0003},   // rxmix_pu_and_dr
    {0x0052,  7,  5, 0x0007},   // rxlna_hg_dr_and_en
    {0x0049,  4,  3, 0x0003},   // rxlna_hg_en_tx
    {0x002B, 15, 12, 0x0000},
    {0x002C,  9,  9, 0x0001},   // rxbt_i2v_gain_dr_and_gain
    {0x002C, 13, 10, 0x0002},
    {0x002C,  0,  0, 0x0001},   // rxbt_adc_gain_dr_and_gain
    {0x002C,  4,  1, 0x0003},
    {0x100B,  1,  0, 0x0001},   // pa_short_dr_off
    {0x004B, 13, 13, 0x0001},   // bt_BB_txif_bypass

    {0x011D,  3,  0, 0x0002},   // bt_lna_lg_rin_dr
    {0x0122, 13, 13, 0x0001},
    {0x01EF, 13, 13, 0x0001},   // bt_lna_fb_res
    {0x01EF,  0,  0, 0x0001},
    {0x0030, 13, 13, 0x0001},   // bt_lna_mixer_first_en
    {0x0030, 12, 12, 0x0001},
    {0x0052,  6,  5, 0x0000},   // bt_lna_hg_en_dr
    {0x0051, 15, 15, 0x0001},

    {0x0024,  3,  1, 0x0004},   // rxgain_dr
    {0x0024,  0,  0, 0x0001},
    {0x0024,  5,  7, 0x0003},   // txgain_dr
    {0x0024,  4,  4, 0x0001},
    {0x0209, 12, 10, 0x0004},   // hwagc_fix_gain_index
    {0x0209,  9,  9, 0x0001},

    {0x00C1, 10,  8, 0x0005},   // bt_rfvco_ldo_vres_tx
    {0x0110,  4,  0, 0x0003},   // pa_i_sel
    {0x0110,  5,  5, 0x0001},   // pa_i_sel_dr
};

static uint32_t dpd_dig_reg_ini[][4] =
{
    {0xd0220C3C, 31, 30, 0x00000001},   // i2v_bypass_dr_off
    {0xd0350308,  9,  0, 0x00000009},   // gsg_nom_i[9:0]
    {0xd0350308, 19, 10, 0x00000009},   // gsg_nom_q[19:10]
    {0xd0350084, 30, 30, 0x00000001},   // tx_if_bypass
};

#define N_TXIQ 4
static int16_t iq_comp_val[2] = {0, 0};
static int16_t dc_comp_val[2] = {0, 0};
static float iq_cali_phi[2] = {0, 0};

static const uint32_t prbs9_4ds_phase0[256] =
{
    #include "RPBS9_PACKET_12M_I16.txt"
};

static void reg_restore_pre(uint16_t rf_reg_back[][2], uint32_t dig_reg_back[][2], uint16_t rf_length, uint16_t dig_length)
{
    uint16_t i;
    for (i = 0; i < rf_length; i++) {
        btdrv_read_rf_reg((uint16_t)rf_reg_back[i][0], &rf_reg_back[i][1]);
    }

    for (i = 0; i < dig_length; i++) {
        dig_reg_back[i][1] = BTDIGITAL_REG((uint32_t)dig_reg_back[i][0]);
    }
}

static void reg_restore_post(uint16_t rf_reg_back[][2], uint32_t dig_reg_back[][2], uint16_t rf_length, uint16_t dig_length)
{
    uint16_t i;
    for (i = 0; i < rf_length; i++) {
        btdrv_write_rf_reg((uint16_t)rf_reg_back[i][0], rf_reg_back[i][1]);
    }
    for (i = 0; i < dig_length; i++) {
        BTDIGITAL_REG_WR((uint32_t)dig_reg_back[i][0], dig_reg_back[i][1]);
    }
}

static void reg_ini_by_table(uint16_t rf_reg_val_table[][4], uint32_t dig_reg_val_table[][4], uint16_t rf_length, uint16_t dig_length)
{
    uint16_t i;
    uint16_t val_u16;
    uint32_t val_u32;
    uint16_t mask_u16;
    uint32_t mask_u32;

    for (i = 0; i < rf_length; i++) {
        mask_u16 = (1 << (rf_reg_val_table[i][REG_BIT_HIGH]-rf_reg_val_table[i][REG_BIT_LOW]+1)) - 1;
        btdrv_read_rf_reg(rf_reg_val_table[i][REG_BIT_ADDR], &val_u16);
        val_u16 &= ~(mask_u16 << rf_reg_val_table[i][REG_BIT_LOW]);
        val_u16 |= (rf_reg_val_table[i][REG_BIT_VAL] << rf_reg_val_table[i][REG_BIT_LOW]);
        btdrv_write_rf_reg(rf_reg_val_table[i][REG_BIT_ADDR], val_u16);
    }

    for (i = 0; i < dig_length; i++) {
        mask_u32 = (1 << (dig_reg_val_table[i][REG_BIT_HIGH]-dig_reg_val_table[i][REG_BIT_LOW]+1)) - 1;
        val_u32 = BTDIGITAL_REG(dig_reg_val_table[i][REG_BIT_ADDR]);
        val_u32 &= ~(mask_u32 << dig_reg_val_table[i][REG_BIT_LOW]);
        val_u32 |= (dig_reg_val_table[i][REG_BIT_VAL] << dig_reg_val_table[i][REG_BIT_LOW]);
        BTDIGITAL_REG_WR(dig_reg_val_table[i][REG_BIT_ADDR], val_u32);
    }
}

static void mat_mul(float *mat1_in, float *mat2_in, float *mat_out, int m, int n ,int k)
{
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < k; j++) {
            mat_out[i * k + j] = 0;
        }
    }
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < k; j++) {
            for (int t = 0; t < n; t++) {
                mat_out[i * k + j] += mat1_in[i * n +t ] * mat2_in[t * k + j];//m*k = m*n * n*k
            }
        }
    }
}

static void inv2(float *A, float *B) // Inverse of second order matrix
{
    float dDiv;
    dDiv = A[0] * A[3] - A[1] * A[2];
    if (!dDiv)
        return;
    B[0] =  A[3] / dDiv;
    B[3] =  A[0] / dDiv;
    B[1] = -A[1] / dDiv;
    B[2] = -A[2] / dDiv;
    return;
}

static int invN(float A[][N_TXIQ], float B[][N_TXIQ], int n) //Inverse of Nth order matrix
{
    int i, j, k;
    double max, temp;
    double t[N_TXIQ][N_TXIQ];                //temp matrix
    //put A[n][n] to t[n][n]
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            t[i][j] = A[i][j];
        }
    }
    //init B to identity matrix
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            B[i][j] = (i == j) ? (double)1 : 0;
        }
    }
    for (i = 0; i < n; i++) {//find the principal element
        max = t[i][i];
        k = i;
        for (j = i + 1; j < n; j++) {
            if (fabs(t[j][i]) > fabs(max)) {
                max = t[j][i];
                k = j;
            }
        }

        //if the row of the primary element is not the i-th row, exchange the rows
        if (k != i) {
            for (j = 0; j < n; j++) {
                temp = t[i][j];
                t[i][j] = t[k][j];
                t[k][j] = temp;
                //B Adjoint exchange
                temp = B[i][j];
                B[i][j] = B[k][j];
                B[k][j] = (float)temp;
            }
        }
        if (t[i][i] == 0) {
            //printf("There is no inverse A!");
            return 0;
        }
        temp = t[i][i];
        for (j = 0; j < n; j++) {
            t[i][j] = t[i][j] / temp;
            B[i][j] = B[i][j] / ((float)temp);
        }
        for (j = 0; j < n; j++) {//row 0->n
            if (j != i) {
                temp = t[j][i];
                for (k = 0; k < n; k++) {
                    t[j][k] = t[j][k] - t[i][k] * temp;
                    B[j][k] = B[j][k] - B[i][k] * ((float)temp);
                }
            }
        }
    }
    return 1;
}

static uint32_t* bt_tx_dpdloop_dma_rxdump(int8_t cali_type, uint16_t dump_length, int8_t print_flag)
{
    uint32_t *dma_dump_data;
    uint8_t channel = 0;
    bt_iq_fft_calculate_init_txcali(dump_length);
    BTDIGITAL_REG_WR(TX_RX_DUMP_SEL, 0x581);//dump config
    if (cali_type == IQ_CALI_PHI) {
        BTDIGITAL_REG_WR(BT_BES_TESTMODE_ADDR, 0x800BAA00 + channel);
    } else {
        BTDIGITAL_REG_WR(BT_BES_TESTMODE_ADDR, 0x800A0000 + channel);
    }

    dma_dump_data = (uint32_t*)bt_iq_fft_calculate_run_txcali(dump_length);
    BTDIGITAL_REG_WR(BT_BES_TESTMODE_ADDR, 0x0);//tx rx off
    if (print_flag == 1) {
        bt_iq_fft_calculate_print_txcali(dump_length);
    }
    return(dma_dump_data);
}

static void bt_rf_dc_cali_dc_pwr(float *rx_dc_re, float *rx_dc_im, int8_t print_flag)
{
    uint32_t *dma_dump_data;
    uint32_t length_dc_cali = 1024;
    dma_dump_data = bt_tx_dpdloop_dma_rxdump(DC_CALI_PWR, length_dc_cali, print_flag);
    int16_t* ptr0 = (int16_t*)dma_dump_data;
    int32_t dump_dc[2] = {0,0};
    for(int ii=0; ii < length_dc_cali; ii++) {
            dump_dc[0] += ptr0[2*ii];
            dump_dc[1] += ptr0[2*ii+1];
    }
    *rx_dc_re = (dump_dc[0]  * 1.0 /256.0);
    *rx_dc_im = (dump_dc[1]  * 1.0 /256.0);
    bt_iq_fft_calculate_clear_txcali();
}

#ifdef IQ_XCORR_DC_CALI
static POSSIBLY_UNUSED void bt_rf_iq_xcorr_dc_cali(uint32_t* dma_dump_data, int32_t* avg_re, int32_t* avg_im)
{
    int32_t dump_re, dump_im;
    int16_t *dump_data_addr = (int16_t*)dma_dump_data;
    int32_t sum_re = 0, sum_im = 0;
    dump_data_addr += 440*2;

    for (int kk = 0; kk <= 255; kk++) {
        dump_re = dump_data_addr[2*kk];
        dump_im = dump_data_addr[2*kk +1];
        sum_re += dump_re;
        sum_im += dump_im;
    }

    *avg_re = sum_re>>8;
    *avg_im = sum_im>>8;

#ifdef IQ_CALI_DEBUG
        btdrv_delay(1);
        BT_DRV_TRACE(1, "iqcali_dc_re_im=%d %d", *avg_re , *avg_im);
#endif
}
#endif

static float bt_rf_iq_cali_xcorr(uint32_t *dma_dump_data, int32_t sum_re, int32_t sum_im)
{
    int32_t acc_i = 0;
    int32_t acc_q = 0;
    int16_t golden_re, golden_im;
    const uint32_t energy_thd = 1e2;
    const int16_t xcorr_length = 256;
    const uint16_t xcorr_window_length = 512;
    int32_t dump_re, dump_im;
    uint32_t *xcorr_result_des = (uint32_t*)bt_drv_malloc(xcorr_window_length*2*sizeof(uint32_t));
    int16_t* txdes = (int16_t*)bt_drv_malloc(xcorr_length * 2 *sizeof(int16_t));
    float ret_phi = 0;
    float cur_pwr;
    float max_pwr = 0.0;
    int xcorr_re, xcorr_im;
    int maxpos = -1;

    int16_t *dump_data_addr = (int16_t*)dma_dump_data;
    dump_data_addr += 320*2;

    memcpy(txdes, prbs9_4ds_phase0, 4 * xcorr_length);

    for (int cnt = 0; cnt <= xcorr_window_length-1; cnt++) {
        //xcorr
        acc_i = 0;
        acc_q = 0;
        for (int kk = 0; kk <= 255; kk++) {
            golden_re = txdes[2*kk];
            golden_im = txdes[2*kk + 1];
            dump_re = dump_data_addr[2*kk + cnt*2];
            dump_im = dump_data_addr[2*kk + cnt*2 +1];
            acc_i = acc_i + golden_re * (dump_re - sum_re) + golden_im * (dump_im - sum_im);
            acc_q = acc_q + golden_re * (dump_im - sum_im) - golden_im * (dump_re - sum_re);
        }
        acc_i = acc_i / 512;
        acc_q = acc_q / 512;
        xcorr_result_des[2*cnt] = acc_i;
        xcorr_result_des[2*cnt+1] = acc_q;
    }

    for (int ii = 0; ii < xcorr_window_length -1; ii++) {
        xcorr_re = xcorr_result_des[2*ii];
        xcorr_im = xcorr_result_des[2*ii+1];
        cur_pwr = 1.0 * xcorr_re*xcorr_re + 1.0 * xcorr_im * xcorr_im;
#ifdef IQ_CALI_DEBUG
        BT_DRV_TRACE(1, "xcorr_re_im=%x %x", xcorr_re, xcorr_im);
        btdrv_delay_us(100);
#endif
        if (max_pwr < cur_pwr) {
            max_pwr = cur_pwr;
            maxpos = ii;
        }
    }
    if (sqrt(max_pwr) > ((double)energy_thd)) {
        xcorr_re = xcorr_result_des[maxpos * 2];
        xcorr_im = xcorr_result_des[maxpos * 2 + 1];
        ret_phi  = -((float)atan2(xcorr_im, xcorr_re));
    }
    #ifdef DC_IQ_CALI_DEBUG_FE
    TRACE(1, "maxpos = %d, %d + j * %d, phi = %d", maxpos, (int)(xcorr_re), (int)(xcorr_im), (int)(ret_phi*100.0));
    #endif
    return ret_phi;
}

static float bt_rf_iq_cali_trx_phi(uint8_t cali_iq_print_flag, uint8_t cali_flag)
{
    float ret_phi;
    int32_t sum_re = 0,sum_im = 0;
    uint32_t *dma_dump_data;

#ifdef IQ_XCORR_DC_CALI
    uint32_t val_u32;
    uint16_t rf_size = sizeof(dc_cali_rf_reg_back) / sizeof(dc_cali_rf_reg_back[0]);
    uint16_t dig_size = sizeof(dc_cali_dig_reg_back) / sizeof(dc_cali_dig_reg_back[0]);

    reg_restore_pre(dc_cali_rf_reg_back, dc_cali_dig_reg_back, rf_size, dig_size);
    val_u32 = BTDIGITAL_REG(TX_GSG_GAIN); // gsg_nom_i[9:0]
    val_u32 &= ~(0x3ff << 0);
    val_u32 &= ~(0x3ff << 10);
    BTDIGITAL_REG_WR(TX_GSG_GAIN, val_u32);
    dma_dump_data = bt_tx_dpdloop_dma_rxdump(DC_CALI_PWR, 1024, cali_iq_print_flag);
    bt_rf_iq_xcorr_dc_cali(dma_dump_data, &sum_re,&sum_im);
    bt_iq_fft_calculate_clear_txcali();
    reg_restore_post(dc_cali_rf_reg_back, dc_cali_dig_reg_back, rf_size, dig_size);
#endif

    dma_dump_data = bt_tx_dpdloop_dma_rxdump(IQ_CALI_PHI, 1024, cali_iq_print_flag);
    ret_phi = bt_rf_iq_cali_xcorr(dma_dump_data, sum_re, sum_im);
    bt_iq_fft_calculate_clear_txcali();
    btdrv_delay_us(100);
    return ret_phi;
}

static void bt_rf_iq_cali_trx_pwr(float Z[], uint8_t cali_flag, int8_t print_flag)
{
    uint32_t *dma_dump_data;

    dma_dump_data = bt_tx_dpdloop_dma_rxdump(IQ_CALI_PWR, 8192, print_flag);

    int16_t* ptr0 = (int16_t*)dma_dump_data;
    int32_t* ptr_sum = (int32_t*)dma_dump_data;
    ptr0 += 1500;
    ptr_sum += (7800-150); // block acc sum at end of MEM1
    memset(ptr_sum, 0, 150*4);
    int16_t* ptr;
    //
    // blockacc by sw, bc hw don't support at bt side
    //
    for(int ii=0; ii<40; ii++) {
        ptr = ptr0 + 150*ii;
        for(int jj=0; jj<75; jj++) {
            ptr_sum[2*jj] += ptr[2*jj];
            ptr_sum[2*jj+1] += ptr[2*jj+1];
        }
    }

    int32_t re, im;
    float re_c, im_c;
    float z0 = 0.0;
    float z1 = 0.0;
    float z2 = 0.0;
    float sum_re = 0.0;
    float sum_im = 0.0;

    int nPtsPerCycle = 75;
    int32_t *p_BlkAccResult = ptr_sum;

    for (int ii = 0; ii < nPtsPerCycle; ii++) {
        re = p_BlkAccResult[2*ii];
        im = p_BlkAccResult[2*ii+1];
        sum_re += 1.0 * re;
        sum_im += 1.0 * im;
    }
    sum_re = sum_re*1.0/nPtsPerCycle;  // calc residual dc
    sum_im = sum_im*1.0/nPtsPerCycle;

    for (int ii = 0; ii < nPtsPerCycle; ii++) {
        re = p_BlkAccResult[2*ii];
        im = p_BlkAccResult[2*ii+1];
        re_c = 1.0*re - sum_re;
        im_c = 1.0*im - sum_im;
        z0 += re_c * re_c;
        z1 += re_c * im_c;
        z2 += im_c * im_c;
        #ifdef IQ_CALI_DEBUG
        TRACE(1, "**blockaccre:%x", re);
        TRACE(1, "**blockaccim:%x", im);
        btdrv_delay(1);
        #endif
    }
    float sf = 1.0 / 1024 / 1024;
    Z[0] = z0 * sf;
    Z[1] = -z1 * sf;
    Z[2] = Z[1];
    Z[3] = z2 * sf;
#ifdef DC_IQ_CALI_DEBUG_FE
    TRACE(1, "Z: <%d, %d, %d>", (int)round(Z[0]), (int)round(Z[1]), (int)round(Z[3]));
#endif
    bt_iq_fft_calculate_clear_txcali();
}

static void SetTxIQ4Polar(uint16_t amp_compen, uint16_t phase_compen, uint8_t band)
{
    uint32_t val_u32;

    val_u32 = BTDIGITAL_REG(TX_IQ_COMP_EN_CTL);
    val_u32 &= ~(0x1 << 29);
    val_u32 |=  (0x1 << 28);
    BTDIGITAL_REG_WR(TX_IQ_COMP_EN_CTL, val_u32);

    BTDIGITAL_REG_WR((TX_IQCOMP_ADDR_2G), amp_compen+(phase_compen<<16));

    val_u32 = BTDIGITAL_REG(TX_IQ_COMP_EN_CTL);
    val_u32 |= (0x1 << 29);
    BTDIGITAL_REG_WR(TX_IQ_COMP_EN_CTL, val_u32);

    val_u32 = BTDIGITAL_REG(TX_RX_SWITCH_CFG);
    val_u32 &= ~(0xf << 16);
    BTDIGITAL_REG_WR(TX_RX_SWITCH_CFG, val_u32);

    btdrv_delay_us(50);

    val_u32 = BTDIGITAL_REG(TX_RX_SWITCH_CFG);
    val_u32 |= (0xa << 16);
    BTDIGITAL_REG_WR(TX_RX_SWITCH_CFG, val_u32);
}

static void iq_cali_by_phaseshift(float Z_1[], float Z_2[], float phi1, float phi2, uint8_t level_sel, uint8_t band)
{
    uint32_t IQ_cali_pre;
    IQ_cali_pre = BTDIGITAL_REG((TX_IQCOMP_ADDR_2G));

    uint16_t ugain_pre = IQ_cali_pre & 0x3fff;
    uint16_t uphase_pre = (IQ_cali_pre >> 16) & 0x3fff;
    int comp_gain_pre = (int)ugain_pre;
    int comp_phase_pre = (int)uphase_pre;
    comp_gain_pre = (comp_gain_pre < 8192) ? comp_gain_pre : comp_gain_pre - 16384;
    comp_phase_pre = (comp_phase_pre < 8192) ? comp_phase_pre : comp_phase_pre - 16384;
    float c_eq1, c_eq2, c_eq3, c_eq4;
    float v_eps[4] = {0, 0, 0, 0};
    float epsilon_t, theta_t;
    // float epsilon_r, theta_r;
    float iq_comp[4] = {0, 0, 0, 0};
    int16_t comp_gain ,comp_phase;
    int total_comp_gain, total_comp_phase;
    c_eq1 = (Z_1[0] - Z_1[3]) / (Z_1[0] + Z_1[3]);
    c_eq2 = (2 * Z_1[1]) / (Z_1[0] + Z_1[3]);
    c_eq3 = (Z_2[0] - Z_2[3]) / (Z_2[0] + Z_2[3]);
    c_eq4 = (2 * Z_2[1]) / (Z_2[0] + Z_2[3]);
    float Mat[][4] = {{(float)cos(2 * phi1), 1,  (float)sin(2  * phi1) ,  0},
                      {(float)sin(2 * phi1), 0, -((float)cos(2 * phi1)), -1},
                      {(float)cos(2 * phi2), 1,  (float)sin(2  * phi2) ,  0},
                      {(float)sin(2 * phi2), 0, -((float)cos(2 * phi2)), -1}};
    float r[4] = {c_eq1, c_eq2, c_eq3, c_eq4};
    invN(Mat,Mat,N_TXIQ);

    for (int i = 0; i < N_TXIQ; i++) {  //N_TXIQ is the matrix order
        v_eps[i] = 0.0;
        for (int j = 0; j < N_TXIQ; j++) {
            v_eps[i] += Mat[i][j] * r[j];
        }
    }
    epsilon_t = v_eps[0];
    theta_t = v_eps[2];
    float numQ_tx[4] = { (1 + epsilon_t / 2)*((float)cos(theta_t / 2)), -((float)sin(theta_t/2)),
                        -((float)sin(theta_t/2)), (1-epsilon_t/2)*((float)cos(theta_t/2)) };
    //inv2_txiq(numQ_tx,iq_comp);
    inv2(numQ_tx, iq_comp);
    comp_gain = (int16_t)round((iq_comp[0] - 1) * 2048 * 4);
    comp_phase = -((int16_t)round((iq_comp[1]) * 2048 * 4));
    iq_comp_val[0] = comp_gain + comp_gain_pre;
    iq_comp_val[1] = comp_phase + comp_phase_pre;

    int tmp_comp_gain = comp_gain;
    int tmp_comp_phase = comp_phase;

    total_comp_gain = tmp_comp_gain + comp_gain_pre;
    total_comp_phase = tmp_comp_phase + comp_phase_pre;
#ifdef DC_IQ_CALI_DEBUG_FE
    TRACE(1, "[TxIQ] comp_gain = %d, comp_ph=%d", tmp_comp_gain, tmp_comp_phase);
    TRACE(1, "[TxIQ] tota_gain = %d, tota_ph=%d", total_comp_gain, total_comp_phase);
#endif
    total_comp_gain = (total_comp_gain < 0) ? (total_comp_gain + 16384) : total_comp_gain;
    total_comp_phase = (total_comp_phase < 0) ? (total_comp_phase + 16384) : total_comp_phase;

    uint16_t uphase = (uint16_t)total_comp_phase;
    uint16_t ugain = (uint16_t)total_comp_gain;
    SetTxIQ4Polar(ugain, uphase, band);
}

static void bt_iq_cali_phase_shift(uint16_t val_cap)
{
    uint16_t val_u16;
    btdrv_read_rf_reg(RF_TX_PHASE_SHIFT_2G_DR , &val_u16);
    val_u16 &= ~(0x1 << 14);
    val_u16 |=  (0x1 << 14);
    btdrv_write_rf_reg(RF_TX_PHASE_SHIFT_2G_DR , val_u16);//phase shift dr

    btdrv_read_rf_reg(RF_TX_PHASE_SHIFT_2G_0 , &val_u16);
    val_u16 &= ~(0xf << 12);
    val_u16 |=  (val_cap << 12);
    btdrv_write_rf_reg(RF_TX_PHASE_SHIFT_2G_0 , val_u16);//phase shift
}


static void bt_iq_cali(int iqcomp_rst, uint8_t band)
{
    uint32_t val_u32;
    uint32_t IQ_COMP_ADDR;
    uint8_t phi_cali_cnt = 4;
    uint8_t cnt0 = 0;
    uint8_t cnt1 = 0;
    float Z0[4];
    float Z1[4];
    float phi0;
    float phi1;
    float phi0_tmp[phi_cali_cnt];
    float phi1_tmp[phi_cali_cnt];
    float phi0_com[phi_cali_cnt];
    float phi1_com[phi_cali_cnt];
    float phi0_sum = 0;
    float phi1_sum = 0;
    float phi0_ofs = 6;
    float phi1_ofs = 6;
    phi0 = iq_cali_phi[0];
    phi1 = iq_cali_phi[1];
    uint8_t cali_flag = 0;

    IQ_COMP_ADDR = TX_IQCOMP_ADDR_2G;

    if (iqcomp_rst == 1) {
        val_u32 = BTDIGITAL_REG(TX_IQ_COMP_EN_CTL);
        val_u32 &= ~(0x1 << 29);
        val_u32 |=  (0x1 << 28);
        BTDIGITAL_REG_WR(TX_IQ_COMP_EN_CTL, val_u32);

        BTDIGITAL_REG_WR(IQ_COMP_ADDR, 0x0);

        val_u32 = BTDIGITAL_REG(TX_IQ_COMP_EN_CTL);
        val_u32 &= ~(0x1 << 29);
        val_u32 &= ~(0x1 << 28);
        BTDIGITAL_REG_WR(TX_IQ_COMP_EN_CTL, val_u32);
    }
    uint16_t rf_size = sizeof(iq_cali_rf_reg_back) / sizeof(iq_cali_rf_reg_back[0]);
    uint16_t dig_size = sizeof(iq_cali_dig_reg_back) / sizeof(iq_cali_dig_reg_back[0]);

    reg_restore_pre(iq_cali_rf_reg_back, iq_cali_dig_reg_back, rf_size, dig_size);

    if (iqcomp_rst == 1) {
        for(int i = 0; i < phi_cali_cnt; i++)
        {
            bt_iq_cali_phase_shift(0);
            phi0 = bt_rf_iq_cali_trx_phi(0, cali_flag);//only execute at 1st time
            iq_cali_phi[0] = phi0;
            phi0_tmp[i] = phi0;
            phi0_sum += phi0;
            bt_iq_cali_phase_shift(7);
            phi1 = bt_rf_iq_cali_trx_phi(0, cali_flag);
            iq_cali_phi[1] = phi1;
            phi1_tmp[i] = phi1;
            phi1_sum += phi1;
            if ((phi1-phi0)>0.65 && (phi1-phi0)<1.13) {
                break;
            }
            else{
            TRACE(1, "[TxIQ] phi xcoor failed, repeat cali times=%d", i+1);
                if (i == phi_cali_cnt -1) {
                    phi0_sum = phi0_sum/4;
                    phi1_sum = phi1_sum/4;
                    for(int ii = 0; ii< phi_cali_cnt; ii++) {
                    phi0_com[ii] = (fabsf(phi0_tmp[ii]-phi0_sum) < (float)3.1415926) ? fabsf(phi0_tmp[ii]-phi0_sum) : fabsf(phi0_tmp[ii]-phi0_sum) -(float)3.14152;
                    phi1_com[ii] = (fabsf(phi1_tmp[ii]-phi1_sum) < (float)3.1415926) ? fabsf(phi1_tmp[ii]-phi1_sum) : fabsf(phi1_tmp[ii]-phi1_sum) -(float)3.14152;
                    cnt0 = (phi0_com[ii] < phi0_ofs)? ii : cnt0;
                    cnt1 = (phi1_com[ii] < phi1_ofs)? ii : cnt1;
                    phi0_ofs = (phi0_com[ii] < phi0_ofs)? phi0_tmp[ii]: phi0_ofs;
                    phi1_ofs = (phi1_com[ii] < phi1_ofs)? phi1_tmp[ii]: phi1_ofs;
                    }
                    iq_cali_phi[0] = phi0_tmp[cnt0];
                    iq_cali_phi[1] = phi1_tmp[cnt0];
                TRACE(1, "[TxIQ] phi xcoor failed, final phi0=%d, phi1=%d", (int)iq_cali_phi[0], (int)iq_cali_phi[1]);
                }
            }
        }
    }

    bt_iq_cali_phase_shift(0);
    bt_rf_iq_cali_trx_pwr(Z0, cali_flag, 0);

    bt_iq_cali_phase_shift(7);
    bt_rf_iq_cali_trx_pwr(Z1, cali_flag, 0);

    iq_cali_by_phaseshift(Z0, Z1, phi0, phi1, /*level_sel*/0, band);

    reg_restore_pre(iq_cali_rf_reg_back, iq_cali_dig_reg_back, rf_size, dig_size);
}

static void bt_dc_cali_tmx_crtl(uint8_t flag)
{
    uint16_t rf_size;
    uint16_t dig_size = 0;
    uint32_t dc_dig_reg_tmx_off[][4] ={};
    if (flag == DC_CALI_TMX_OFF)
    {
    rf_size = sizeof(dc_rf_reg_tmx_off) / sizeof(dc_rf_reg_tmx_off[0]);
    reg_ini_by_table(dc_rf_reg_tmx_off, dc_dig_reg_tmx_off, rf_size, dig_size);
    }
    if (flag == DC_CALI_TMX_ON)
    {
    rf_size = sizeof(dc_rf_reg_tmx_on) / sizeof(dc_rf_reg_tmx_on[0]);
    reg_ini_by_table(dc_rf_reg_tmx_on, dc_dig_reg_tmx_off, rf_size, dig_size);
    }
}

static void bt_dc_cali_reg_ini(void)
{
    uint16_t rf_size = sizeof(dc_rf_reg_ini) / sizeof(dc_rf_reg_ini[0]);
    uint16_t dig_size = sizeof(dc_dig_reg_ini) / sizeof(dc_dig_reg_ini[0]);;
    reg_ini_by_table(dc_rf_reg_ini, dc_dig_reg_ini, rf_size, dig_size);
}


void bt_dc_cali(int8_t band)
{
    float rx_dc_re = 0, rx_dc_im = 0;
    float rx_dc_re_delta = 0, rx_dc_im_delta = 0;
    float rx_dc_re_default = 0, rx_dc_im_default = 0;
    short reg_dc_re = 0, reg_dc_im = 0;
    unsigned short u_reg_dc_re = 0, u_reg_dc_im = 0;
    short rad_sf = DC_CALI_RAD;
    int radius = 140*rad_sf;
    float diag_v0 = 0.002321995464853*16/(rad_sf*rad_sf); //R=140*4, this value need /65536 further
    float diag_v1 = 7.281777777778422e+02; //R=140*4, this value need /65536 further
    float vec_y[6] = {0, 0, 0, 0, 0, 0};

    int opt_txdc_i = 0;
    int opt_txdc_q = 0;
    int temp_opt_txdc_i = 0;
    int temp_opt_txdc_q = 0;
    uint32_t val_u32 = 0;
    uint32_t DC_COMP_ADDR;

    uint16_t rf_size = sizeof(dc_cali_rf_reg_back) / sizeof(dc_cali_rf_reg_back[0]);
    uint16_t dig_size = sizeof(dc_cali_dig_reg_back) / sizeof(dc_cali_dig_reg_back[0]);
    reg_restore_pre(dc_cali_rf_reg_back, dc_cali_dig_reg_back, rf_size, dig_size);
    DC_COMP_ADDR = TX_DCCOMP_ADDR_2G;   //0xd0350080

    bt_dc_cali_reg_ini();

    bt_dc_cali_tmx_crtl(DC_CALI_TMX_OFF);

#ifdef DC_CALI_DEBUG
    BT_DRV_TRACE(1, "%s L%d Close txmixer", __FUNCTION__, __LINE__);
#endif

    rx_dc_re_default = 0.0;
    rx_dc_im_default = 0.0;

    for(int ii = 0; ii < 16; ii ++) {
        bt_rf_dc_cali_dc_pwr(&rx_dc_re, &rx_dc_im, 0);
        btdrv_delay_us(10);
#ifdef DC_CALI_DEBUG
        BT_DRV_TRACE(1, "txdc_info  %d %d", (int)round(rx_dc_re*100), (int)round(rx_dc_im*100));
#endif
        rx_dc_re_default += rx_dc_re;
        rx_dc_im_default += rx_dc_im;
    }
    rx_dc_re_default /= 16.0;
    rx_dc_im_default /= 16.0;

    bt_dc_cali_tmx_crtl(DC_CALI_TMX_ON);


    for(int t0 = 0; t0 < 360; t0 += 4)
    {
        reg_dc_re = (short)floor(radius * cos(t0 * 3.1415926 / 180.0) + (double)0.5);//128.0
        reg_dc_im = (short)floor(radius * sin(t0 * 3.1415926 / 180.0) + (double)0.5);
        u_reg_dc_re = (reg_dc_re >= 0) ? reg_dc_re : (reg_dc_re + 16384);
        u_reg_dc_im = (reg_dc_im >= 0) ? reg_dc_im : (reg_dc_im + 16384);

        val_u32 = BTDIGITAL_REG(DC_COMP_ADDR);
        val_u32 &= ~(0xfffffff << 0);
        val_u32 |= (u_reg_dc_re + u_reg_dc_im*16384);
        BTDIGITAL_REG_WR(DC_COMP_ADDR, val_u32);

        btdrv_delay_us(10);
        bt_rf_dc_cali_dc_pwr(&rx_dc_re, &rx_dc_im, 0);

        rx_dc_re_delta = rx_dc_re - rx_dc_re_default;
        rx_dc_im_delta = rx_dc_im - rx_dc_im_default;
#ifdef DC_CALI_DEBUG
        float pwr;
        pwr = rx_dc_re_delta*rx_dc_re_delta + rx_dc_im_delta*rx_dc_im_delta;
        BT_DRV_TRACE(1, "pwr_vs_theta: %d %d %d %d %d %d", t0 / 4, (int)pwr, (int)round(rx_dc_re*100), (int)round(rx_dc_im*100), (int)reg_dc_re, (int)reg_dc_im);
        btdrv_delay(1);
#endif
        vec_y[0] += (float)reg_dc_re*rx_dc_re_delta + (float)reg_dc_im*rx_dc_im_delta;
        vec_y[1] += -(float)reg_dc_im*rx_dc_re_delta + (float)reg_dc_re*rx_dc_im_delta;
        vec_y[2] += (float)reg_dc_re*rx_dc_re_delta - (float)reg_dc_im*rx_dc_im_delta;
        vec_y[3] += (float)reg_dc_im*rx_dc_re_delta + (float)reg_dc_re*rx_dc_im_delta;
        vec_y[4] += (rx_dc_re_delta);
        vec_y[5] += (rx_dc_im_delta);
    }

    vec_y[0] = vec_y[0]*diag_v0/65536;
    vec_y[1] = vec_y[1]*diag_v0/65536;
    vec_y[2] = vec_y[2]*diag_v0/65536;
    vec_y[3] = vec_y[3]*diag_v0/65536;
    vec_y[4] = vec_y[4]*diag_v1/65536;
    vec_y[5] = vec_y[5]*diag_v1/65536;

    float mat_B[4] = { 0 };  // 2x2
    float inv_mat_B[4] = { 0 };  // 2x2
    float opt_txdc[2];
    float vec_k[2] = { -vec_y[4], -vec_y[5] };
    mat_B[0] = vec_y[0] + vec_y[2];
    mat_B[1] = vec_y[3] - vec_y[1];
    mat_B[2] = vec_y[3] + vec_y[1];
    mat_B[3] = vec_y[0] - vec_y[2];

    inv2(mat_B, inv_mat_B);
    mat_mul(inv_mat_B, vec_k, opt_txdc, 2, 2, 1);

    temp_opt_txdc_i += (int)floor(opt_txdc[0] + 0.5);
    temp_opt_txdc_q += (int)floor(opt_txdc[1] + 0.5);
    opt_txdc_i = temp_opt_txdc_i;
    opt_txdc_q = temp_opt_txdc_q;

    opt_txdc_i = (opt_txdc_i < 0) ? (opt_txdc_i + 16384) : opt_txdc_i;
    opt_txdc_q = (opt_txdc_q < 0) ? (opt_txdc_q + 16384) : opt_txdc_q;

    uint16_t val_opt_i = (uint16_t)opt_txdc_i;
    uint16_t val_opt_q = (uint16_t)opt_txdc_q;
    BT_DRV_TRACE(1, "opt_txdc_i: %d opt_txdc_q: %d ", (int)temp_opt_txdc_i, (int)temp_opt_txdc_q);
    dc_comp_val[0] = val_opt_i;
    dc_comp_val[1] = val_opt_q;

    val_u32 = BTDIGITAL_REG(DC_COMP_ADDR);
    val_u32 &= ~(0xfffffff << 0);
    val_u32 |= ((val_opt_i & 0x3fff) << 0);
    val_u32 |= ((val_opt_q & 0x3fff) << 14);
    BTDIGITAL_REG_WR(DC_COMP_ADDR, val_u32);

    reg_restore_post(dc_cali_rf_reg_back, dc_cali_dig_reg_back, rf_size, dig_size);
}

void dc_iq_calib_1307ph(void)
{
    uint32_t time_start = hal_sys_timer_get();

    BT_DRV_TRACE(1,"################dc_iq_calib_1307ph start");
    uint16_t rf_size = sizeof(dpd_rf_reg_back) / sizeof(dpd_rf_reg_back[0]);
    uint16_t dig_size = sizeof(dpd_dig_reg_back) / sizeof(dpd_dig_reg_back[0]);
    reg_restore_pre(dpd_rf_reg_back, dpd_dig_reg_back, rf_size, dig_size);

    uint16_t rf_size_b = sizeof(dpd_rf_reg_ini) / sizeof(dpd_rf_reg_ini[0]);
    uint16_t dig_size_b = sizeof(dpd_dig_reg_ini) / sizeof(dpd_dig_reg_ini[0]);
    reg_ini_by_table(dpd_rf_reg_ini, dpd_dig_reg_ini, rf_size_b, dig_size_b);

    bt_dc_cali(BAND_2G);
    TRACE(1,"store dc cali comp value = %d, %d", dc_comp_val[0], dc_comp_val[1]);

    bt_iq_cali(1,BAND_2G);
    bt_iq_cali(0,BAND_2G);//iq cali 2nd
    bt_iq_cali(0,BAND_2G);//iq cali 2nd
    bt_iq_cali(0,BAND_2G);//iq cali 2nd
    TRACE(1,"store iq cali comp value = %d, %d", iq_comp_val[0], iq_comp_val[1]);

    BT_DRV_TRACE(1,"-------------------dc_iq_calib_1307ph end");
    BTDIGITAL_REG_WR(TX_RX_DUMP_SEL, 0x0);//dump config
    reg_restore_post(dpd_rf_reg_back, dpd_dig_reg_back,rf_size, dig_size);
    BT_DRV_TRACE(1,"use time: %d ms", __TICKS_TO_MS(hal_sys_timer_get()-time_start));
}

void dc_iq_reg_fullband(void)
{
    uint32_t val_u32;
    uint32_t addr_comp_iq = TX_IQCOMP_ADDR_2G;
    uint32_t comp_iq_u32 = 0;

    // set dc calib val
    val_u32 = BTDIGITAL_REG(TX_DCCOMP_ADDR_2G);
    val_u32 &= ~(0xfffffff << 0);
    val_u32 |=  (dc_comp_val[0] | (dc_comp_val[1] << 14));
    BTDIGITAL_REG_WR(TX_DCCOMP_ADDR_2G, val_u32);

    val_u32 = BTDIGITAL_REG(TX_DCCOMP_ADDR_2G_IF);
    val_u32 &= ~(0xfffffff << 0);
    val_u32 |=  (dc_comp_val[0] | (dc_comp_val[1] << 14));
    BTDIGITAL_REG_WR(TX_DCCOMP_ADDR_2G_IF, val_u32);

    // set iq calib val
    val_u32 = BTDIGITAL_REG(TX_IQ_COMP_EN_CTL);
    val_u32 &= ~(0x1 << 29);
    val_u32 |=  (0x1 << 28);
    BTDIGITAL_REG_WR(TX_IQ_COMP_EN_CTL, val_u32);

    comp_iq_u32 = BTDIGITAL_REG((TX_IQCOMP_ADDR_2G));

    for (int i = 0; i < 128; i++) {
        BTDIGITAL_REG_WR(addr_comp_iq + 4 * i, comp_iq_u32);
    }

    val_u32 = BTDIGITAL_REG(TX_IQ_COMP_EN_CTL);
    val_u32 &= ~(0x1 << 29);
    val_u32 |=  (0x1 << 30);
    val_u32 &= ~(0x1 << 28);
    BTDIGITAL_REG_WR(TX_IQ_COMP_EN_CTL, val_u32);
}

#endif
#endif

