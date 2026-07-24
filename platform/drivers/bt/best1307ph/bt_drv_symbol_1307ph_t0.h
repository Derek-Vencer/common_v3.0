
/*******************************************************************************************************************************/
/*********************************************bt controller symbol**************************************************************/

#ifndef  __BT_CONTROLLER_1307PH_T0_SYMBOL_H__
#define  __BT_CONTROLLER_1307PH_T0_SYMBOL_H__

/*****   Macro Definition of Relative Offset of Variables   *****/

// ld_acl_env_tag/rssi_record
#define OFF_RR_IN_LAET_LD_1307PH_T0                                   0XE6
// ld_acl_env_tag/last_sync_bit_off
#define OFF_LSBO_IN_LAET_LD_1307PH_T0                                 0XCE
// ld_acl_env_tag/t_poll
#define OFF_TP_IN_LAET_LD_1307PH_T0                                   0XF0
// ld_acl_env_tag/last_sync_clk_off
#define OFF_LSCO_IN_LAET_LD_1307PH_T0                                 0XB8
// ld_acl_env_tag/rxbit_1us
#define OFF_R1_IN_LAET_LD_1307PH_T0                                   0XCA
// bt_util_buf_env_tag/acl_rx_free
#define OFF_ARF_IN_BUBET_BT_1307PH_T0                                 0X14
// bt_util_buf_env_tag/acl_tx_free
#define OFF_ATF_IN_BUBET_BT_1307PH_T0                                 0X28
// ble_util_buf_env_tag/acl_tx_free
#define OFF_ATF_IN_BUBET_BLE_1307PH_T0                                0X28
// dbg_bt_sche_setting_cmd/music_playing_link
#define OFF_MPL_IN_DBSSC_DBG_1307PH_T0                                0XA
// dbg_bt_sche_setting_cmd/acl_slot_in_ibrt_mode
#define OFF_ASIIM_IN_DBSSC_DBG_1307PH_T0                              0XC
// dbg_bt_sche_setting_cmd/acl_interv_in_ibrt_normal_mode
#define OFF_AIIINM_IN_DBSSC_DBG_1307PH_T0                             0X2
// dbg_bt_common_setting_cmd/trace_level
#define OFF_TL_IN_DBCSC_DBG_1307PH_T0                                 0X8
// ld_bes_bt_env_tag/tws_device_env/link_id
#define OFF_LI_IN_LBBET_LD_1307PH_T0                                  0X12
// dbg_bt_ibrt_setting_cmd/accept_remote_enter_sniff
#define OFF_ARES_IN_DBISC_DBG_1307PH_T0                               0XF
// dbg_bt_ibrt_setting_cmd/ibrt_second_sco_decision
#define OFF_ISSD_IN_DBISC_DBG_1307PH_T0                               0XE
// lc_sco_env_tag/p_link_params/link_id
#define OFF_LI_IN_LSET_LC_1307PH_T0                                   0X1E
// lc_sco_nego_params_tag/air_params_received/t_esco
#define OFF_TE_IN_LSNPT_LC_1307PH_T0                                  0X45
// lc_sco_nego_params_tag/air_params_received/m2s_pkt_type
#define OFF_MPT_IN_LSNPT_LC_1307PH_T0                                 0X47
// lc_sco_nego_params_tag/air_params_received/air_mode
#define OFF_AM_IN_LSNPT_LC_1307PH_T0                                  0X4E
// dbg_bt_hw_feat_setting_cmd/bt_sync_swagc_en
#define OFF_BSSE_IN_DBHFSC_DBG_1307PH_T0                              0XF
// dbg_bt_hw_feat_setting_cmd/le_sync_swagc_en
#define OFF_LSSE_IN_DBHFSC_DBG_1307PH_T0                              0X10
// rwip_env_tag/lp_clk
#define OFF_LC_IN_RET_RWIP_1307PH_T0                                  0X39C
// rwip_env_tag/last_bt_samp_time/hs
#define OFF_H_IN_RET_RWIP_0_1307PH_T0                                 0X28
// rwip_env_tag/last_bt_samp_time/hus
#define OFF_H_IN_RET_RWIP_1_1307PH_T0                                 0X2C
// rwip_env_tag/last_us_samp_time
#define OFF_LUST_IN_RET_RWIP_1307PH_T0                                0X34
// lld_cis_env/grp_hdl
#define OFF_GH_IN_LCE_LLD_1307PH_T0                                   0X1A
// lld_cig_env/anchor/hs
#define OFF_H_IN_LCE_LLD_0_1307PH_T0                                  0X58
// lld_cig_env/anchor/hus
#define OFF_H_IN_LCE_LLD_1_1307PH_T0                                  0X5C
// lld_cig_env/iso_interval
#define OFF_II_IN_LCE_LLD_1307PH_T0                                   0X9A
// lld_bis_env/grp_hdl
#define OFF_GH_IN_LBE_LLD_1307PH_T0                                   0X1C
// lld_big_env/anchor/hs
#define OFF_H_IN_LBE_LLD_0_1307PH_T0                                  0XBC
// lld_big_env/anchor/hus
#define OFF_H_IN_LBE_LLD_1_1307PH_T0                                  0XC0
// lld_big_env/iso_interval
#define OFF_II_IN_LBE_LLD_1307PH_T0                                   0XF4
// dbg_bt_common_setting_t2/bt_master_sleep_en
#define OFF_BMSE_IN_DBCST_DBG_1307PH_T0                               0X2F
// dbg_bt_common_setting_t2/bt_master_tx_silence_en
#define OFF_BMTSE_IN_DBCST_DBG_1307PH_T0                              0X30
// dbg_bt_common_setting_t2/tws_sleep_dura_dec
#define OFF_TSDD_IN_DBCST_DBG_1307PH_T0                               0X3A
// dbg_bt_common_setting_t2/normal_sleep_dura_inc
#define OFF_NSDI_IN_DBCST_DBG_1307PH_T0                               0X3B


#define HCI_FC_ENV_1307PH_T0_ADDR                                     0XC0007088
#define LD_ACL_ENV_1307PH_T0_ADDR                                     0XC000538C
#define BT_UTIL_BUF_ENV_1307PH_T0_ADDR                                0XC0004BEC
#define BLE_UTIL_BUF_ENV_1307PH_T0_ADDR                               0XC0005988
#define LD_BES_BT_ENV_1307PH_T0_ADDR                                  0XC00057A8
#define DBG_STATE_1307PH_T0_ADDR                                      0XC00072A2
#define LC_STATE_1307PH_T0_ADDR                                       0XC00052AC
#define LD_SCO_ENV_1307PH_T0_ADDR                                     0XC0005674
#define RX_MONITOR_1307PH_T0_ADDR                                     0XC0007C65
#define LC_ENV_1307PH_T0_ADDR                                         0XC0005044
#define LM_NB_SYNC_ACTIVE_1307PH_T0_ADDR                              0XC0005030
#define LM_ENV_1307PH_T0_ADDR                                         0XC0004DFC
#define HCI_ENV_1307PH_T0_ADDR                                        0XC0006F98
#define LC_SCO_ENV_1307PH_T0_ADDR                                     0XC0005020
#define LLM_ENV_1307PH_T0_ADDR                                        0XC0005BDC
#define LD_ENV_1307PH_T0_ADDR                                         0XC0005314
#define RWIP_ENV_1307PH_T0_ADDR                                       0XC0007D10
#define BLE_RX_MONITOR_1307PH_T0_ADDR                                 0XC0007BFD
#define LLC_ENV_1307PH_T0_ADDR                                        0XC000647C
#define RWIP_RF_1307PH_T0_ADDR                                        0XC0007C9C
#define LD_ACL_METRICS_1307PH_T0_ADDR                                 0XC000539C
#define RF_RX_HWGAIN_TBL_1307PH_T0_ADDR                               0XC00049B6
#define RF_HWAGC_RSSI_CORRECT_TBL_1307PH_T0_ADDR                      0XC00048D4
#define RF_RX_GAIN_FIXED_TBL_1307PH_T0_ADDR                           0XC0007B70
#define HCI_DBG_EBQ_TEST_MODE_1307PH_T0_ADDR                          0XC00072BC
#define DBG_BT_COMMON_SETTING_1307PH_T0_ADDR                          0XC0007148
#define DBG_BT_SCHE_SETTING_1307PH_T0_ADDR                            0XC0007200
#define DBG_BT_IBRT_SETTING_1307PH_T0_ADDR                            0XC00071E4
#define DBG_BT_HW_FEAT_SETTING_1307PH_T0_ADDR                         0XC00071C0
#define HCI_DBG_SET_SW_RSSI_1307PH_T0_ADDR                            0XC00072EC
#define RWIP_PROG_DELAY_1307PH_T0_ADDR                                0XC0007C98
#define SCH_MULTI_IBRT_ADJUST_ENV_1307PH_T0_ADDR                      0XC0006EF8
#define RF_HWAGC_RSSI_CORRECT_TBL_1307PH_T0_ADDR                      0XC00048D4
#define RF_RX_GAIN_THS_TBL_LE_1307PH_T0_ADDR                          0XC000497A
#define RF_RX_GAIN_THS_TBL_LE_2M_1307PH_T0_ADDR                       0XC0004998
#define RF_RPL_TX_PW_CONV_TBL_1307PH_T0_ADDR                          0XC00048E4
#define REPLACE_MOBILE_ADDR_1307PH_T0_ADDR                            0XC00052B8
#define REPLACE_ADDR_VALID_1307PH_T0_ADDR                             0XC00052B4
#define PCM_NEED_START_FLAG_1307PH_T0_ADDR                            0XC0005690
#define RF_RX_GAIN_THS_TBL_BT_3M_1307PH_T0_ADDR                       0XC0007B7C
#define LD_IBRT_ENV_1307PH_T0_ADDR                                    0XC0005900
#define LLM_LOCAL_LE_FEATS_1307PH_T0_ADDR                             0XC000477E
#define RF_RX_GAIN_THS_TBL_BT_1307PH_T0_ADDR                          0XC0004920
#define DBG_BT_COMMON_SETTING_T2_1307PH_T0_ADDR                       0XC000716E
#define LLD_CON_ENV_1307PH_T0_ADDR                                    0XC0006858
#define LLD_ISO_ENV_1307PH_T0_ADDR                                    0XC0006888
#define ECC_RX_MONITOR_1307PH_T0_ADDR                                 0XC0007C21
#define __STACKLIMIT_1307PH_T0_ADDR                                   0XC00088A8
#define PER_MONITOR_PARAMS_1307PH_T0_ADDR                             0XC0004A6A
#define MASTER_CON_SUPPORT_LE_AUDIO_1307PH_T0_ADDR                    0XC0005054
#define RX_RECORD_1307PH_T0_ADDR                                      0XC000569C
#define SCH_PROG_DBG_ENV_1307PH_T0_ADDR                               0XC0006D40
#define SENS2BT_EN_1307PH_T0_ADDR                                     0XC0004050
#define RF_RX_GAIN_THS_TBL_ECC_1307PH_T0_ADDR                         0XC000495C
#define LLD_BIS_ENV_1307PH_T0_ADDR                                    0XC0006C34
#define LLD_BIG_ENV_1307PH_T0_ADDR                                    0XC0006C24
#define LLD_CIS_ENV_1307PH_T0_ADDR                                    0XC0006BF4
#define LLD_CIG_ENV_1307PH_T0_ADDR                                    0XC0006BE4
#define RX_MONITOR_3M_RXGAIN_1307PH_T0_ADDR                           0XC000477D
#define INTERSYS_MSG_TX_MODE_1307PH_T0_ADDR                           0XC0004030
#define CON_LATENCY_APPLY_1307PH_T0_ADDR                              0XC00047AC
#define LLD_PER_ADV_ENV_1307PH_T0_ADDR                                0XC00067E4
#define REPLACE_MOBILE_LE_LINK_ID_1307PH_T0_ADDR                      0XC000479C
#define REPLACE_MOBILE_LE_LINK_ID_VALID_1307PH_T0_ADDR                0XC00064D0
#define LLI_ENV_1307PH_T0_ADDR                                        0XC00063E4
#define LLI_CIS_ENV_1307PH_T0_ADDR                                    0XC000644C

#endif