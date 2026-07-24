
/*******************************************************************************************************************************/
/*********************************************bt controller symbol**************************************************************/

#ifndef  __BT_CONTROLLER_1503P_T0_SYMBOL_H__
#define  __BT_CONTROLLER_1503P_T0_SYMBOL_H__

/*****   Macro Definition of Relative Offset of Variables   *****/

// ld_acl_env_tag/rssi_record
#define OFF_RR_IN_LAET_LD_1503P_T0                                    0XEA
// ld_acl_env_tag/last_sync_bit_off
#define OFF_LSBO_IN_LAET_LD_1503P_T0                                  0XD2
// ld_acl_env_tag/t_poll
#define OFF_TP_IN_LAET_LD_1503P_T0                                    0XF4
// ld_acl_env_tag/last_sync_clk_off
#define OFF_LSCO_IN_LAET_LD_1503P_T0                                  0XBC
// ld_acl_env_tag/rxbit_1us
#define OFF_R1_IN_LAET_LD_1503P_T0                                    0XCE
// bt_util_buf_env_tag/acl_rx_free
#define OFF_ARF_IN_BUBET_BT_1503P_T0                                  0X14
// bt_util_buf_env_tag/acl_tx_free
#define OFF_ATF_IN_BUBET_BT_1503P_T0                                  0X28
// ble_util_buf_env_tag/acl_tx_free
#define OFF_ATF_IN_BUBET_BLE_1503P_T0                                 0X28
// dbg_bt_sche_setting_cmd/music_playing_link
#define OFF_MPL_IN_DBSSC_DBG_1503P_T0                                 0XA
// dbg_bt_sche_setting_cmd/acl_slot_in_ibrt_mode
#define OFF_ASIIM_IN_DBSSC_DBG_1503P_T0                               0XC
// dbg_bt_sche_setting_cmd/acl_interv_in_ibrt_normal_mode
#define OFF_AIIINM_IN_DBSSC_DBG_1503P_T0                              0X2
// dbg_bt_common_setting_cmd/trace_level
#define OFF_TL_IN_DBCSC_DBG_1503P_T0                                  0X8
// ld_bes_bt_env_tag/tws_device_env/link_id
#define OFF_LI_IN_LBBET_LD_1503P_T0                                   0X12
// dbg_bt_ibrt_setting_cmd/accept_remote_enter_sniff
#define OFF_ARES_IN_DBISC_DBG_1503P_T0                                0XF
// dbg_bt_ibrt_setting_cmd/ibrt_second_sco_decision
#define OFF_ISSD_IN_DBISC_DBG_1503P_T0                                0XE
// lc_sco_env_tag/p_link_params/link_id
#define OFF_LI_IN_LSET_LC_1503P_T0                                    0X1E
// lc_sco_nego_params_tag/air_params_received/t_esco
#define OFF_TE_IN_LSNPT_LC_1503P_T0                                   0X45
// lc_sco_nego_params_tag/air_params_received/m2s_pkt_type
#define OFF_MPT_IN_LSNPT_LC_1503P_T0                                  0X47
// lc_sco_nego_params_tag/air_params_received/air_mode
#define OFF_AM_IN_LSNPT_LC_1503P_T0                                   0X4E
// dbg_bt_hw_feat_setting_cmd/bt_sync_swagc_en
#define OFF_BSSE_IN_DBHFSC_DBG_1503P_T0                               0XF
// dbg_bt_hw_feat_setting_cmd/le_sync_swagc_en
#define OFF_LSSE_IN_DBHFSC_DBG_1503P_T0                               0X10
// rwip_env_tag/lp_clk
#define OFF_LC_IN_RET_RWIP_1503P_T0                                   0X530
// rwip_env_tag/last_bt_samp_time/hs
#define OFF_H_IN_RET_RWIP_0_1503P_T0                                  0X28
// rwip_env_tag/last_bt_samp_time/hus
#define OFF_H_IN_RET_RWIP_1_1503P_T0                                  0X2C
// rwip_env_tag/last_us_samp_time
#define OFF_LUST_IN_RET_RWIP_1503P_T0                                 0X34
// lld_cis_env/grp_hdl
#define OFF_GH_IN_LCE_LLD_1503P_T0                                    0X1A
// lld_cig_env/anchor/hs
#define OFF_H_IN_LCE_LLD_0_1503P_T0                                   0X5C
// lld_cig_env/anchor/hus
#define OFF_H_IN_LCE_LLD_1_1503P_T0                                   0X60
// lld_cig_env/iso_interval
#define OFF_II_IN_LCE_LLD_1503P_T0                                    0X9E
// lld_bis_env/grp_hdl
#define OFF_GH_IN_LBE_LLD_1503P_T0                                    0X20
// lld_big_env/anchor/hs
#define OFF_H_IN_LBE_LLD_0_1503P_T0                                   0XC4
// lld_big_env/anchor/hus
#define OFF_H_IN_LBE_LLD_1_1503P_T0                                   0XC8
// lld_big_env/iso_interval
#define OFF_II_IN_LBE_LLD_1503P_T0                                    0XFC
// dbg_bt_common_setting_t2/bt_master_sleep_en
#define OFF_BMSE_IN_DBCST_DBG_1503P_T0                                0X2F
// dbg_bt_common_setting_t2/bt_master_tx_silence_en
#define OFF_BMTSE_IN_DBCST_DBG_1503P_T0                               0X30
// dbg_bt_common_setting_t2/en_sec_con_base_on_le_aud_sup
#define OFF_ESCBOLAS_IN_DBCST_DBG_1503P_T0                            0X2C


#define HCI_FC_ENV_1503P_T0_ADDR                                      0XC000B224
#define LD_ACL_ENV_1503P_T0_ADDR                                      0XC00053EC
#define BT_UTIL_BUF_ENV_1503P_T0_ADDR                                 0XC0004E20
#define BLE_UTIL_BUF_ENV_1503P_T0_ADDR                                0XC000638C
#define LD_BES_BT_ENV_1503P_T0_ADDR                                   0XC0005EB8
#define DBG_STATE_1503P_T0_ADDR                                       0XC000B6E4
#define LC_STATE_1503P_T0_ADDR                                        0XC0005300
#define LD_SCO_ENV_1503P_T0_ADDR                                      0XC0005D10
#define RX_MONITOR_1503P_T0_ADDR                                      0XC000C2C8
#define LC_ENV_1503P_T0_ADDR                                          0XC0005264
#define LM_NB_SYNC_ACTIVE_1503P_T0_ADDR                               0XC0005250
#define LM_ENV_1503P_T0_ADDR                                          0XC0004FD0
#define HCI_ENV_1503P_T0_ADDR                                         0XC000B0FC
#define LC_SCO_ENV_1503P_T0_ADDR                                      0XC0005240
#define LLM_ENV_1503P_T0_ADDR                                         0XC00066E0
#define LD_ENV_1503P_T0_ADDR                                          0XC0005370
#define RWIP_ENV_1503P_T0_ADDR                                        0XC000C404
#define BLE_RX_MONITOR_1503P_T0_ADDR                                  0XC000C0FD
#define LLC_ENV_1503P_T0_ADDR                                         0XC0009684
#define RWIP_RF_1503P_T0_ADDR                                         0XC000C384
#define LD_ACL_METRICS_1503P_T0_ADDR                                  0XC0005400
#define RF_RX_HWGAIN_TBL_1503P_T0_ADDR                                0XC0004B58
#define RF_HWAGC_RSSI_CORRECT_TBL_1503P_T0_ADDR                       0XC0004A58
#define RF_RX_GAIN_FIXED_TBL_1503P_T0_ADDR                            0XC000C016
#define HCI_DBG_EBQ_TEST_MODE_1503P_T0_ADDR                           0XC000B6FC
#define DBG_BT_COMMON_SETTING_1503P_T0_ADDR                           0XC000B4C2
#define DBG_BT_SCHE_SETTING_1503P_T0_ADDR                             0XC000B588
#define DBG_BT_IBRT_SETTING_1503P_T0_ADDR                             0XC000B56C
#define DBG_BT_HW_FEAT_SETTING_1503P_T0_ADDR                          0XC000B548
#define HCI_DBG_SET_SW_RSSI_1503P_T0_ADDR                             0XC000B72C
#define RWIP_PROG_DELAY_1503P_T0_ADDR                                 0XC000C380
#define SCH_MULTI_IBRT_ADJUST_ENV_1503P_T0_ADDR                       0XC000B038
#define RF_HWAGC_RSSI_CORRECT_TBL_1503P_T0_ADDR                       0XC0004A58
#define RF_RX_GAIN_THS_TBL_LE_1503P_T0_ADDR                           0XC0004AFE
#define RF_RX_GAIN_THS_TBL_LE_2M_1503P_T0_ADDR                        0XC0004B1C
#define RF_RPL_TX_PW_CONV_TBL_1503P_T0_ADDR                           0XC0004A68
#define REPLACE_MOBILE_ADDR_1503P_T0_ADDR                             0XC0005310
#define REPLACE_ADDR_VALID_1503P_T0_ADDR                              0XC000530C
#define PCM_NEED_START_FLAG_1503P_T0_ADDR                             0XC0005D94
#define RF_RX_GAIN_THS_TBL_BT_3M_1503P_T0_ADDR                        0XC000C022
#define LD_IBRT_ENV_1503P_T0_ADDR                                     0XC000613C
#define LLM_LOCAL_LE_FEATS_1503P_T0_ADDR                              0XC00048BA
#define RF_RX_GAIN_THS_TBL_BT_1503P_T0_ADDR                           0XC0004AA4
#define DBG_BT_COMMON_SETTING_T2_1503P_T0_ADDR                        0XC000B4E8
#define LLD_CON_ENV_1503P_T0_ADDR                                     0XC0009C2C
#define LLD_ISO_ENV_1503P_T0_ADDR                                     0XC0009CCC
#define ECC_RX_MONITOR_1503P_T0_ADDR                                  0XC000C131
#define __STACKLIMIT_1503P_T0_ADDR                                    0XC000D408
#define PER_MONITOR_PARAMS_1503P_T0_ADDR                              0XC0004C32
#define SENS2BT_EN_1503P_T0_ADDR                                      0XC0004090
#define RF_RX_GAIN_THS_TBL_ECC_1503P_T0_ADDR                          0XC0004AE0
#define LLD_BIS_ENV_1503P_T0_ADDR                                     0XC000A0D8
#define LLD_BIG_ENV_1503P_T0_ADDR                                     0XC000A0C8
#define LLD_CIS_ENV_1503P_T0_ADDR                                     0XC000A078
#define LLD_CIG_ENV_1503P_T0_ADDR                                     0XC000A068
#define RX_MONITOR_3M_RXGAIN_1503P_T0_ADDR                            0XC00047C1
#define INTERSYS_MSG_TX_MODE_1503P_T0_ADDR                            0XC0004060
#define DBG_BT_SCHE_SETTING_COEX_CIS_1503P_T0_ADDR                    0XC000B5AE
#define DBG_BT_SCHE_SETTING_COEX_BIS_1503P_T0_ADDR                    0XC000B5A0
#define REPLACE_MOBILE_LE_LINK_ID_VALID_1503P_T0_ADDR                 0XC0009710
#define REPLACE_MOBILE_LE_LINK_ID_1503P_T0_ADDR                       0XC00048D8
#define LE_CON_SKIP_3_INTERVAL_1503P_T0_ADDR                          0XC0009C28

#endif