/***************************************************************************
 *
 * Copyright (c) 2015-2024 BES Technic
 *
 * Authored by BES CD team (Blueelf Prj).
 *
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
#ifndef __CS_SERVICE_H__
#define __CS_SERVICE_H__

#include "bt_common_define.h"
#include "bt_stack_status.h"
#include "gap_service.h"
#ifdef __cplusplus
extern "C" {
#endif

#define CS_BYTE_BIT_NUM         (8)
#define CS_MAX_PRESET_NUM       CS_MAX_CONFIG_NUM

#define CS_TEST_CONN_HANDLE     (0x0FFF)    // connection handle used for cs test
#define CS_INVALID_CONIDX       GAP_INVALID_CONIDX
#define CS_INVALID_CONNHDL      GAP_INVALID_CONN_HANDLE

#define CS_STEP_MODE_MAX_NUM    (4)

#define CS_MAX_CONFIG_NUM       (4)
#define CS_INVALID_CONFIG_ID    CS_MAX_CONFIG_NUM

// TODO only one antenna path supported now
#define CS_MAX_ANTENNA_PATH_NUM (1)

#define CS_CHANNEL_NUM          (72)
#define CS_CHANNEL_MAP_BIT_NUM  (80)
#define CS_CHANNEL_MAP_SIZE     (10)        // CS_CHANNEL_MAP_BIT_NUM / CS_BYTE_BIT_NUM

#define CS_SYNC_PAYLOAD_SIZE    (16)
#define CS_INVALID_TX_POWER     0x7F

#define CS_MAIN_MODE_STEPS_MIN_VALUE    (0x02)
#define CS_STEP_MODE_MASK_BIT_NUM       (16)
#define CS_MAIN_MODE_REPETITION_MAX     (0x03)
#define CS_MODE_0_STEPS_MIN             (0x01)
#define CS_MODE_0_STEPS_MAX             (0x03)

typedef uint8_t cs_proc_enable_t;
enum cs_proc_enable
{
    CS_PROCEDURE_DISABLE,
    CS_PROCEDURE_ENABLE,
};

enum cs_action
{
    CS_CONFIG_REMOVED,
    CS_CONFIG_CREATED,
};

enum cs_aa_qty_type
{
    CS_AA_CHECK_NO_ERROR,
    CS_AA_CHECK_ERROR,
    CS_AA_NOT_FOUND,
};

typedef uint8_t cs_step_mode_t;
enum cs_step_mode
{
    CS_STEP_MODE_0,
    CS_STEP_MODE_1,
    CS_STEP_MODE_2,
    CS_STEP_MODE_3,
    /// @brief only meaningful for sub_mode_type
    CS_STEP_MODE_UNUSED = 0xFF,
};

typedef uint8_t cs_role_t;
enum cs_role
{
    CS_ROLE_INITIATOR,
    CS_ROLE_REFLECTOR,
    /// @brief only meaningful for upper software, not a spec specified value
    CS_ROLE_UNKNOWN     = 0xFF,
};

typedef uint8_t cs_role_mask_t;
#define CS_ROLE_INITIATOR_MASK  CO_BIT_MASK(0)
#define CS_ROLE_REFLECTOR_MASK  CO_BIT_MASK(1)
#define CS_ROLE_BOTH_MASK       (CS_ROLE_INITIATOR_MASK | CS_ROLE_REFLECTOR_MASK)

typedef uint8_t cs_mode_mask_t;
#define CS_MODE_3_MASK          CO_BIT_MASK(0)

typedef uint8_t cs_sync_antenna_selection_type_t;
enum cs_sync_antenna_selection_type
{
    CS_SYNC_ANTENNA_SELECTION_1                 = 0x01,
    CS_SYNC_ANTENNA_SELECTION_2,
    CS_SYNC_ANTENNA_SELECTION_3,
    CS_SYNC_ANTENNA_SELECTION_4,
    /// @brief antennas to be used, in repetive order from 0x01 to 0x04, for CS_SYNC packets by the local controller
    CS_SYNC_ANTENNA_SELECTION_ALL               = 0xFE,
    /// @brief host does not have a CS sync antenna selection recommendation
    CS_SYNC_ANTENNA_SELECTION_NO_RECOMMENDATION = 0xFF,
};

typedef uint8_t cs_rtt_type_t;
enum cs_rtt_type
{
    /// @brief RTT Access Address Only
    CS_RTT_TYPE_AA_ONLY,
    /// @brief RTT with 32-bit Sounding Sequence
    CS_RTT_TYPE_32_SS,
    /// @brief RTT with 96-bit Sounding Sequence
    CS_RTT_TYPE_96_SS,
    /// @brief RTT with 32-bit Random Sequence
    CS_RTT_TYPE_32_RS,
    /// @brief RTT with 64-bit Random Sequence
    CS_RTT_TYPE_64_RS,
    /// @brief RTT with 96-bit Random Sequence
    CS_RTT_TYPE_96_RS,
    /// @brief RTT with 128-bit Random Sequence
    CS_RTT_TYPE_128_RS,
    /// @brief only meaningful for upper software, not a spec specified value
    CS_RTT_TYPE_UNKNOWN = 0xFF,
};

typedef uint8_t cs_sync_phy_t;
enum cs_sync_phy
{
    CS_SYNC_PHY_LE_1M = 0x01,
    CS_SYNC_PHY_LE_2M,
    CS_SYNC_PHY_LE_2M_BT,
};

typedef uint8_t cs_channel_selection_t;
enum cs_channel_selection
{
    CS_CHANNEL_SELECTION_3B,
    CS_CHANNEL_SELECTION_3C,
};

typedef uint8_t cs_ch3c_shape_t;
enum cs_ch3c_shape
{
    /// @brief Use Hat shape for user-specified channel sequence.
    CS_CH3C_HAT_SHAPE,
    /// @brief Use X shape for user-specified channel sequence.
    CS_CH3C_X_SHAPE,
};

typedef uint8_t cs_pkt_nadm_t;
enum cs_pkt_nadm
{
    CS_PKT_NADM_ATTACK_EX_UNLIKELY,
    CS_PKT_NADM_ATTACK_VERY_UNLIKELY,
    CS_PKT_NADM_ATTACK_UNLIKELY,
    CS_PKT_NADM_ATTACK_POSSIBLE,
    CS_PKT_NADM_ATTACK_LIKELY,
    CS_PKT_NADM_ATTACK_VERY_LIKELY,
    CS_PKT_NADM_ATTACK_EX_LIKELY,
    CS_PKT_NADM_ATTACK_UNKNOWN = 0xFF,
};

enum cs_tone_quality
{
    CS_TONE_QUALITY_HIGH,
    CS_TONE_QUALITY_MEDIUM,
    CS_TONE_QUALITY_LOW,
    CS_TONE_QUALITY_UNAVAILABLE,
};

enum cs_tone_extension_slot
{
    /// not tone extension slot
    CS_NOT_TONE_EXTENSION_SLOT,
    /// tone extension slot, but tone not expected to be present
    CS_TONE_EXTENSION_SLOT_NOT_PRESENT,
    /// tone extension slot, and tone expected to present
    CS_TONE_EXTENTION_SLOT_PRESENT,
};

typedef uint8_t cs_create_config_context_t;
enum cs_create_config_context
{
    /// write CS configuration in local controller only
    CS_CREATE_CONFIG_CONTEXT_LOCAL_ONLY,
    /// write CS configuration in both local and remote controller using Channel Sounding Configuration procedure
    CS_CREATE_CONFIG_CONTEXT_BOTH_SIDE,
};

typedef enum
{
    CS_STEP_SPEC_TYPE_MODE_0_ROLE_I,
    CS_STEP_SPEC_TYPE_MODE_0_ROLE_R,
    CS_STEP_SPEC_TYPE_MODE_1_ROLE_I,
    CS_STEP_SPEC_TYPE_MODE_1_ROLE_I_SPBR_SS,
    CS_STEP_SPEC_TYPE_MODE_1_ROLE_R,
    CS_STEP_SPEC_TYPE_MODE_1_ROLE_R_SPBR_SS,
    CS_STEP_SPEC_TYPE_MODE_2_ROLE_E,
    CS_STEP_SPEC_TYPE_MODE_3_ROLE_I,
    CS_STEP_SPEC_TYPE_MODE_3_ROLE_I_SPBR_SS,
    CS_STEP_SPEC_TYPE_MODE_3_ROLE_R,
    CS_STEP_SPEC_TYPE_MODE_3_ROLE_R_SPBR_SS,
    CS_STEP_SPEC_TYPE_MODE_ROLE_UNKNOWN,
} cs_step_spec_type_t;

typedef uint8_t cs_rtt_capability_mask_t;
#define CS_RTT_CAPABILITY_AA_ONLY_PRECISION_10_MASK             CO_BIT_MASK(0)
#define CS_RTT_CAPABILITY_SOUNDING_N_PRECISION_10_MASK          CO_BIT_MASK(1)
#define CS_RTT_CAPABILITY_RANDOM_PAYLOAD_N_PRECISION_10_MASK    CO_BIT_MASK(2)

// for aa_only_n, sounding_n, random_payload_n, 0x00 means not supported
#define CS_RTT_NOT_SUPPORT   0x00
// for nadm_sounding, nadm_random
#define CS_NADM_CAPABILITY_MASK                                 CO_BIT_MASK(0)

typedef uint8_t cs_sync_phys_mask_t;
#define CS_SYNC_PHYS_LE_2M_MASK                                 CO_BIT_MASK(1)
#define CS_SYNC_PHYS_LE_2M_2BT_MASK                             CO_BIT_MASK(2)

typedef uint16_t cs_subfeatures_mask_t;
#define CS_SUBFEATURES_WITH_NO_TRANSMITTER_FAE_MASK             CO_BIT_MASK(1)
#define CS_SUBFEATURES_SELECTION_ALGORITHM_3C_MASK              CO_BIT_MASK(2)
#define CS_SUBFEATURES_PBR_FROM_RTT_SOUNDING_SEQUENCE_MASK      CO_BIT_MASK(3)

typedef uint16_t cs_t_ip_times_mask_t;
#define CS_T_IP_TIMES_10us_MASK                                 CO_BIT_MASK(0)
#define CS_T_IP_TIMES_20us_MASK                                 CO_BIT_MASK(1)
#define CS_T_IP_TIMES_30us_MASK                                 CO_BIT_MASK(2)
#define CS_T_IP_TIMES_40us_MASK                                 CO_BIT_MASK(3)
#define CS_T_IP_TIMES_50us_MASK                                 CO_BIT_MASK(4)
#define CS_T_IP_TIMES_60us_MASK                                 CO_BIT_MASK(5)
#define CS_T_IP_TIMES_80us_MASK                                 CO_BIT_MASK(6)

typedef uint16_t cs_t_fcs_times_mask_t;
#define CS_T_FCS_TIMES_15us_MASK                                CO_BIT_MASK(0)
#define CS_T_FCS_TIMES_20us_MASK                                CO_BIT_MASK(1)
#define CS_T_FCS_TIMES_30us_MASK                                CO_BIT_MASK(2)
#define CS_T_FCS_TIMES_40us_MASK                                CO_BIT_MASK(3)
#define CS_T_FCS_TIMES_50us_MASK                                CO_BIT_MASK(4)
#define CS_T_FCS_TIMES_60us_MASK                                CO_BIT_MASK(5)
#define CS_T_FCS_TIMES_80us_MASK                                CO_BIT_MASK(6)
#define CS_T_FCS_TIMES_100us_MASK                               CO_BIT_MASK(7)
#define CS_T_FCS_TIMES_120us_MASK                               CO_BIT_MASK(8)

typedef uint16_t cs_t_pm_times_mask_t;
#define CS_T_PM_TIMES_10us_MASK                                 CO_BIT_MASK(0)
#define CS_T_PM_TIMES_20us_MASK                                 CO_BIT_MASK(1)

typedef uint8_t cs_snr_capability_mask_t;
#define CS_SNR_CAPABILITY_18dB_MASK                             CO_BIT_MASK(0)
#define CS_SNR_CAPABILITY_21dB_MASK                             CO_BIT_MASK(1)
#define CS_SNR_CAPABILITY_24dB_MASK                             CO_BIT_MASK(2)
#define CS_SNR_CAPABILITY_27dB_MASK                             CO_BIT_MASK(3)
#define CS_SNR_CAPABILITY_30dB_MASK                             CO_BIT_MASK(4)

typedef struct
{
    /// @brief Number of CS configurations supported per connection, range: 0x01 ~ 0x04
    uint8_t                  num_config_supp;
    /// @brief cnsv -> consecutive, 0x0000: support for both a fixed number of consecutive cs procedures,
    ///         and for an indefinite number of CS procedures until termination
    uint16_t                 max_cnsv_proc_supp;
    /// @brief range: 0x01~0x04
    uint8_t                  num_antennas_supp;
    /// @brief range: 0x01~0x04
    uint8_t                  max_antenna_paths_supp;
    /// @brief bit-0: Initiator; bit-1: Reflector; bit-other: RFU
    cs_role_mask_t           roles_supp;
    /// @brief bit-0: Mode-3; bit-other: RFU
    cs_mode_mask_t           modes_supp;
    /// @brief bit-field
    ///
    /// bit-0:
    ///
    /// - If set to 1, then the value reflected in the RTT_AA_Only_N field refers to the
    ///     0 ns time-of-flight precision requirement. Otherwise, the RTT_AA_Only_N
    ///     field refers to the 150 ns time-of-flight precision requirement.
    ///
    /// - If the RTT_AA_Only_N field is set to 0, then the bit shall be ignored.
    ///
    /// bit-1:
    ///
    /// - If set to 1, then the value reflected in the RTT_Sounding_N field refers to the
    ///     10 ns time-of-flight precision requirement. Otherwise, the RTT_Sounding_N
    ///     field refers to the 150 ns time-of-flight precision requirement.
    ///
    /// - If the RTT_Sounding_N field is set to 0, then the bit shall be ignored.
    ///
    /// bit-2:
    ///
    /// - If set to 1, then the value reflected in the RTT_Random_Payload_N field
    ///     refers to the 10 ns time-of-flight precision requirement. Otherwise, the
    ///     RTT_Random_Payload_N field refers to the 150 ns time-of-flight precision requirement.
    ///
    /// - If the RTT_Random_Payload_N field is set to 0, then the bit shall be ignored.
    cs_rtt_capability_mask_t rtt_capability;
    /// @brief 0x00: RTT AA only not supported; 0x01~0xFF: number of CS steps of single packet exchanges needed to satisfy the precision requirements
    uint8_t                  rtt_aa_only_n;
    uint8_t                  rtt_sounding_n;
    uint8_t                  rtt_random_payload_n;
    /// @brief bit-filed, bit-0: support for Phase-based Normalized Attack Detector Metric when a CS_SYNC with sounding sequence is received
    uint16_t                 nadm_sounding_capability;
    /// @brief bit-field, bit-0: support for Phase-based Normalized Attack Detector Metric when a CS_SYNC with random sequence is received
    uint16_t                 nadm_random_capability;
    /// @brief bit-field, bit-1: LE 2M PHY; bit-2: LE 2M 2BT PHY
    cs_sync_phys_mask_t      cs_sync_phys_supp;
    /// @brief bit-field
    ///
    /// bit-1: CS with no transmitter Frequency Actuation Error
    ///
    /// bit-2: CS Channel Selection Algorithm #3c
    ///
    /// bit-3: CS phase-based ranging from RTT sounding sequence
    cs_subfeatures_mask_t    subfeatures_supp;
    /// @brief Interlude time in microseconds between the RTT packets. bit-field
    cs_t_ip_times_mask_t     t_ip1_times_supp;
    /// @brief Interlude time in microseconds between the CS tones. bit-field
    cs_t_ip_times_mask_t     t_ip2_times_supp;
    /// @brief Time in microseconds for frequency changes. bit-field
    cs_t_fcs_times_mask_t    t_fcs_times_supp;
    /// @brief Time in microseconds for the phase measurement period of the CS tones. bit-field
    uint16_t                 t_pm_times_supp;
    /// @brief Time in microseconds for the antenna switch period of the CS tones.
    ///
    /// valid value: 0x00, 0x01, 0x02, 0x04, 0x0A
    uint8_t                  t_sw_time_supp;
    /// @brief bit-field
    ///
    /// bit-0: 18 dB supported
    ///
    /// bit-1: 21 dB supported
    ///
    /// bit-2: 24 dB supported
    ///
    /// bit-3: 27 dB supported
    ///
    /// bit-4: 30 dB supported
    cs_snr_capability_mask_t tx_snr_capability;
} __attribute__((packed)) cs_supp_capas_t;

typedef struct
{
    cs_step_mode_t         main_mode_type;
    cs_step_mode_t         sub_mode_type;
    /// @brief Minimum number of CS main mode steps to be executed before a submode step is executed. Range: 0x02~0xFF.
    uint8_t                min_main_mode_steps;
    /// @brief Maximum number of CS main mode steps to be executed before a submode step is executed. Range: 0x02~0xFF.
    uint8_t                max_main_mode_steps;
    /// @brief Number of main mode steps taken from the end of the last CS subevent to be repeated at the beginning of
    ///         the current CS subevent directly after the last mode-0 step of that event. Range: 0x00~0x03.
    uint8_t                main_mode_repetition;
    /// @brief Number of CS mode-0 steps to be included at the beginning of each CS subevent. Range: 0x01~0x03.
    uint8_t                mode_0_steps;
    cs_role_t              role;
    cs_rtt_type_t          rtt_type;
    cs_sync_phy_t          cs_sync_phy;
    /// @brief bit-field
    ///
    /// This parameter contains 80 1-bit fields.
    ///
    /// The nth such field (in the range 0 to 78) contains the value for the CS channel index n.
    ///
    /// - Channel n is enabled for CS procedure = 1
    ///
    /// - Channel n is disabled for CS procedure = 0
    ///
    /// Channels n = 0, 1, 23, 24, 25, 77, and 78 shall be ignored and `shall` be set to zero. At least 15 channels shall be enabled.
    ///
    /// The most significant bit (bit 79) is reserved for future use.
    uint8_t                channel_map[CS_CHANNEL_MAP_SIZE];

    /// @brief The number of times the Channel_Map field will be cycled through for non-mode-0 steps within a CS procedure. Range: 0x01~0xFF.
    uint8_t                channel_map_repetition;
    /// @brief Channel Selection Algorithm used for non-mode-0 CS steps.
    cs_channel_selection_t channel_selection_type;
    /// @brief shape that will be used for user-specified cahnnel sequence for Channel Selection Algorithm #3c.
    cs_ch3c_shape_t        ch3c_shape;
    /// @brief Number of channels skipped in each rising and falling sequence. Range: 0x02~0x08.
    uint8_t                ch3c_jump;
} cs_basic_config_t;

#define CS_SELECTED_TX_POWER_LEVEL_UNAVALIABLE      0x7F
#define CS_PROC_COUNT_CONTINUE_UNTIL_DISABLED       0x0000
/// @brief the procedure configuration reported by Controller
typedef struct
{
    uint8_t  config_id;
    cs_proc_enable_t state;
    /// @brief Antenna configuration index selection during CS procedure. Range: 0x00~0x07.
    uint8_t  tone_antenna_config_selection;
    /// @brief Transmit power level. Range: -127~20.
    ///
    /// 0xXX: Transmit power level used for CS procedure
    ///
    /// 0x7F: Transmit power level is unavailable
    uint8_t  selected_tx_power;
    /// @brief Duration for each CS subevent in microseconds. Range: 1250us~4s; 0x000004E2~0x003D0900.
    uint32_t subevent_len;
    /// @brief Number of CS subevents anchored off the smae acl connection event. Range: 0x01~0x20.
    uint8_t  subevents_per_evt;
    /// @brief Time between consecutive CS subevents anchored off the same ACL connection event. Units: 0.625ms.
    uint16_t subevent_interval;
    /// @brief Number of ACL connection events between consecutive CS event anchor points.
    uint16_t event_interval;
    /// @brief Number of ACL connection events between consecutive CS procedure anchor points.
    uint16_t proc_interval;
    /// @brief Number of CS procedures to be scheduled.
    ///
    /// 0x0000: CS procedures to continue until disabled
    ///
    /// 0xXXXX: Number of CS procedures to be scheduled. Range: 0x0001~0xFFFF.
    uint16_t proc_count;
    /// @brief Maximum duration for each CS procedure.
    ///
    /// Range: 0x0001~0xFFFF.
    ///
    /// Units: 0.625ms.
    ///
    /// Time range: 0.625ms~40.959375s
    uint16_t max_proc_len;
} cs_proc_params_t;

typedef uint8_t cs_snr_control_t;
enum cs_snr_control
{
    CS_SNR_CONTROL_18dB,
    CS_SNR_CONTROL_21dB,
    CS_SNR_CONTROL_24dB,
    CS_SNR_CONTROL_27dB,
    CS_SNR_CONTROL_30dB,
    CS_SNR_CONTROL_NOT_USE = 0xFF,
};

#define CS_PROC_TX_POWER_DELTA_NO_RECOMMENDATION    0x0080

typedef uint8_t cs_set_proc_phy_t;
enum cs_set_proc_phy
{
    CS_SET_PROC_PHY_LE_1M = 0x01,
    CS_SET_PROC_PHY_LE_2M,
    CS_SET_PROC_PHY_LE_CODED_S8,
    CS_SET_PROC_PHY_LE_CODED_S2,
};

typedef uint8_t cs_prefer_peer_antenna_mask_t;
#define CS_PREFER_PEER_ANTENNA_TYPE_USE_1st_ORDERED_MASK    CO_BIT_MASK(0)
#define CS_PREFER_PEER_ANTENNA_TYPE_USE_2nd_ORDERED_MASK    CO_BIT_MASK(1)
#define CS_PREFER_PEER_ANTENNA_TYPE_USE_3rd_ORDERED_MASK    CO_BIT_MASK(2)
#define CS_PREFER_PEER_ANTENNA_TYPE_USE_4th_ORDERED_MASK    CO_BIT_MASK(3)
typedef struct
{
    /// @brief Maximum duration for each CS procedure. `Ranging`: 0x0001 to 0xFFFF. `Time` = N x 0.625 ms. `Time ranging`: 0.625 ms to 40.959375 s
    uint16_t                      max_proc_len;
    /// @brief  Minimum number of connection events between consecutive CS procedures. `Range`: 0x0001 to 0xFFFF
    uint16_t                      min_proc_interval;
    /// @brief Maximum number of connection events between consecutive CS procedures. `Range`: 0x0001 to 0xFFFF
    uint16_t                      max_proc_interval;
    /// @brief
    /// `0x0000`: CS procedures to continue until disabled. `CS_PROC_COUNT_CONTINUE_UNTIL_DISABLED`
    ///
    /// `0xXXXX`:  Maximum number of CS procedures to be scheduled
    uint16_t                      max_proc_count;
    /// @brief Minimum suggested duration for each CS subevent in microseconds. `Range`: 1250 μs to 4 s, i.e. 0x000004E2 ~ 0x003D0900
    uint32_t                      min_subevent_len;
   /// @brief Maximum suggested duration for each CS subevent in microseconds. `Range`: 1250 μs to 4 s, i.e. 0x000004E2 ~ 0x003D0900
    uint32_t                      max_subevent_len;
    /// @brief Antenna Configuration Index. `Range`: 0x00 ~ 0x07.
    uint8_t                       tone_antenna_config_selection;
    /// @brief `enum cs_proc_phy_type`
    cs_set_proc_phy_t             phy;
    /// @brief
    /// `0xXX`: Transmit power delta, in signed dB, to indicate the recommended difference between the remote device’s power level for
    ///                 the CS tones and RTT packets and the existing power level for the PHY indicated by the PHY parameter.
    ///
    /// `0x80`:  Host does not have a recommendation for transmit power delta. `CS_PROC_TX_POWER_DELTA_NO_RECOMMENDATION`
    uint8_t                       tx_power_delta;
    /// @brief `enum cs_proc_pref_peer_antenna_type`
    cs_prefer_peer_antenna_mask_t preferred_peer_antenna;
    cs_snr_control_t              snr_control_initiator;
    cs_snr_control_t              snr_control_reflector;
} __attribute__((packed)) cs_set_proc_params_t;

// cs tone params -- i.e. `tone_pct` & `tone_qty_indicator`, whose num is subject to `num_antenna_paths`
typedef union
{
    uint32_t tone_params;
    // * note that, this struct is just used to calculate size, the structure do not match the actual case
    struct
    {
        uint32_t tone_pct           : 24;
        uint32_t tone_qty_indicator : 8;
    } ctx;
} __attribute__((packed)) cs_tone_params_t;

typedef union
{
    uint32_t pct;
    struct
    {
        /// @brief actually 12 bits
        uint32_t pkt_pct_i_sample : 12;
        /// @brief actually 12 bits
        uint32_t pkt_pct_q_sample : 12;
        /// @brief always zero now
        uint32_t pkt_pct_reserved : 8;
    } ctx;
} __attribute__((packed)) cs_pkt_pct_t;

typedef union
{
    /// @brief lower 4 bits of `Packet_Quality`
    uint8_t aa_qty    : 4;
    /// @brief higner 4 bits of `Packet_Quality`
    uint8_t rs_ss_qty : 4;
} __attribute__((packed)) cs_pkt_qty_t;

typedef struct
{
    cs_pkt_qty_t     pkt_qty;
    /// @brief range: -127 ~ 20, units: dBm
    uint8_t          pkt_rssi;
    uint8_t          pkt_antenna;
    /// @brief range: -100ppm ~ +100ppm (0x58F0 ~ 0x2710), units: 0.01ppm
    uint16_t         measured_freq_offset;
} __attribute__((packed)) cs_mode_0_role_i_co_t;

typedef struct
{
    cs_pkt_qty_t     pkt_qty;
    /// @brief range: -127 ~ 20, units: dBm
    uint8_t          pkt_rssi;
    uint8_t          pkt_antenna;
} __attribute__((packed)) cs_mode_0_role_r_co_t;

typedef struct
{
    cs_pkt_qty_t     pkt_qty;
    cs_pkt_nadm_t    pkt_nadm;
    /// @brief range: -127 ~ 20, units: dBm
    uint8_t          pkt_rssi;
    /// @brief Time difference in units of 0.5 nanoseconds between the time of arrival and
    ///         the time of departure of the CS packets at the initiator during a CS step
    ///         (16-bit signed integer), where the known nominal offsets are excluded.
    uint16_t         toa_tod_initiator;
    uint8_t          pkt_antenna;
} __attribute__((packed)) cs_mode_1_role_i_co_t;

typedef struct
{
    cs_pkt_qty_t     pkt_qty;
    cs_pkt_nadm_t    pkt_nadm;
    /// @brief range: -127 ~ 20, units: dBm
    uint8_t          pkt_rssi;
    /// @brief Time difference in units of 0.5 nanoseconds between the time of arrival and
    ///         the time of departure of the CS packets at the initiator during a CS step
    ///         (16-bit signed integer), where the known nominal offsets are excluded.
    uint16_t         toa_tod_initiator;
    uint8_t          pkt_antenna;
    cs_pkt_pct_t     pkt_pct_1;
    cs_pkt_pct_t     pkt_pct_2;
} __attribute__((packed)) cs_mode_1_role_i_spbr_ss_t;

typedef struct
{
    cs_pkt_qty_t     pkt_qty;
    cs_pkt_nadm_t    pkt_nadm;
    /// @brief range: -127 ~ 20, units: dBm
    uint8_t          pkt_rssi;
    /// @brief Time difference in units of 0.5 nanoseconds between the time of departure
    ///         and the time of arrival of the CS packets at the reflector during a CS step
    ///         (16-bit signed integer), where the known nominal offsets are excluded.
    /// @details 0x8000 is not available
    uint16_t         tod_toa_reflector;
    uint8_t          pkt_antenna;
} __attribute__((packed)) cs_mode_1_role_r_co_t;

typedef struct
{
    cs_pkt_qty_t     pkt_qty;
    cs_pkt_nadm_t    pkt_nadm;
    /// @brief range: -127 ~ 20, units: dBm
    uint8_t          pkt_rssi;
    /// @brief Time difference in units of 0.5 nanoseconds between the time of departure
    ///         and the time of arrival of the CS packets at the reflector during a CS step
    ///         (16-bit signed integer), where the known nominal offsets are excluded.
    /// @details 0x8000 is not available
    uint16_t         tod_toa_reflector;
    uint8_t          pkt_antenna;
    cs_pkt_pct_t     pkt_pct_1;
    cs_pkt_pct_t     pkt_pct_2;
} __attribute__((packed)) cs_mode_1_role_r_spbr_ss_t;

typedef struct
{
    /// @brief Antenna Permutation Index for the chosen Num_Antenna_Paths parameter
    ///         used during the phase measurement stage of the CS step. Range: 0x00 ~ 0x17
    uint8_t          antenna_permutation_idx;
    /// @brief include `tone_pct` & `tone_qty_indicator`, whose num is subject to `num_antenna_paths`
    cs_tone_params_t tone_params[CS_MAX_ANTENNA_PATH_NUM + 1];
} __attribute__((packed)) cs_mode_2_role_e_co_t;

typedef struct
{
    cs_pkt_qty_t     pkt_qty;
    cs_pkt_nadm_t    pkt_nadm;
    /// @brief range: -127 ~ 20, units: dBm
    uint8_t          pkt_rssi;
    /// @brief Time difference in units of 0.5 nanoseconds between the time of arrival and
    ///         the time of departure of the CS packets at the initiator during a CS step
    ///         (16-bit signed integer), where the known nominal offsets are excluded.
    uint16_t         toa_tod_initiator;
    uint8_t          pkt_antenna;
    /// @brief Antenna Permutation Index for the chosen Num_Antenna_Paths parameter
    ///         used during the phase measurement stage of the CS step. Range: 0x00 ~ 0x17
    uint8_t          antenna_permutation_idx;
    /// @brief include `tone_pct` & `tone_qty_indicator`, whose num is subject to `num_antenna_paths`
    cs_tone_params_t tone_params[CS_MAX_ANTENNA_PATH_NUM + 1];
} __attribute__((packed)) cs_mode_3_role_i_co_t;

typedef struct
{
    cs_pkt_qty_t     pkt_qty;
    cs_pkt_nadm_t    pkt_nadm;
    /// @brief range: -127 ~ 20, units: dBm
    uint8_t          pkt_rssi;
    /// @brief Time difference in units of 0.5 nanoseconds between the time of arrival and
    ///         the time of departure of the CS packets at the initiator during a CS step
    ///         (16-bit signed integer), where the known nominal offsets are excluded.
    uint16_t         toa_tod_initiator;
    uint8_t          pkt_antenna;
    cs_pkt_pct_t     pkt_pct_1;
    cs_pkt_pct_t     pkt_pct_2;
    /// @brief Antenna Permutation Index for the chosen Num_Antenna_Paths parameter
    ///         used during the phase measurement stage of the CS step. Range: 0x00 ~ 0x17
    uint8_t          antenna_permutation_idx;
    /// @brief include `tone_pct` & `tone_qty_indicator`, whose num is subject to `num_antenna_paths`
    cs_tone_params_t tone_params[CS_MAX_ANTENNA_PATH_NUM + 1];
} __attribute__((packed)) cs_mode_3_role_i_spbr_ss_t;

typedef struct
{
    cs_pkt_qty_t     pkt_qty;
    cs_pkt_nadm_t    pkt_nadm;
    /// @brief range: -127 ~ 20, units: dBm
    uint8_t          pkt_rssi;
    /// @brief Time difference in units of 0.5 nanoseconds between the time of departure
    ///         and the time of arrival of the CS packets at the reflector during a CS step
    ///         (16-bit signed integer), where the known nominal offsets are excluded.
    /// @details 0x8000 is not available
    uint16_t         tod_toa_reflector;
    uint8_t          pkt_antenna;
    /// @brief Antenna Permutation Index for the chosen Num_Antenna_Paths parameter
    ///         used during the phase measurement stage of the CS step. Range: 0x00 ~ 0x17
    uint8_t          antenna_permutation_idx;
    /// @brief include `tone_pct` & `tone_qty_indicator`, whose num is subject to `num_antenna_paths`
    cs_tone_params_t tone_params[CS_MAX_ANTENNA_PATH_NUM + 1];
} __attribute__((packed)) cs_mode_3_role_r_co_t;

typedef struct
{
    cs_pkt_qty_t     pkt_qty;
    cs_pkt_nadm_t    pkt_nadm;
    /// @brief range: -127 ~ 20, units: dBm
    uint8_t          pkt_rssi;
    /// @brief Time difference in units of 0.5 nanoseconds between the time of departure
    ///         and the time of arrival of the CS packets at the reflector during a CS step
    ///         (16-bit signed integer), where the known nominal offsets are excluded.
    /// @details 0x8000 is not available
    uint16_t         tod_toa_reflector;
    uint8_t          pkt_antenna;
    cs_pkt_pct_t     pkt_pct_1;
    cs_pkt_pct_t     pkt_pct_2;
    /// @brief Antenna Permutation Index for the chosen Num_Antenna_Paths parameter
    ///         used during the phase measurement stage of the CS step. Range: 0x00 ~ 0x17
    uint8_t          antenna_permutation_idx;
    /// @brief include `tone_pct` & `tone_qty_indicator`, whose num is subject to `num_antenna_paths`
    cs_tone_params_t tone_params[CS_MAX_ANTENNA_PATH_NUM + 1];
} __attribute__((packed)) cs_mode_3_role_r_spbr_ss_t;

typedef union
{
    void *ptr;
    cs_mode_0_role_i_co_t *common;
} cs_mode_0_role_i_t;

typedef union
{
    void *ptr;
    cs_mode_0_role_r_co_t *common;
} cs_mode_0_role_r_t;

typedef union
{
    void *ptr;
    cs_mode_0_role_i_t role_i;
    cs_mode_0_role_r_t role_r;
} cs_mode_0_step_data_t;

typedef union
{
    void *ptr;
    cs_mode_1_role_i_co_t      *common;
    cs_mode_1_role_i_spbr_ss_t *spbr_ss;
} cs_mode_1_role_i_t;

typedef union
{
    void *ptr;
    cs_mode_1_role_r_co_t      *common;
    cs_mode_1_role_r_spbr_ss_t *spbr_ss;
} cs_mode_1_role_r_t;

typedef union
{
    void *ptr;
    cs_mode_1_role_i_t role_i;
    cs_mode_1_role_r_t role_r;
} cs_mode_1_step_data_t;

typedef union
{
    void *ptr;
    cs_mode_2_role_e_co_t *common;
} cs_mode_2_role_e_t;

typedef union
{
    void *ptr;
    cs_mode_2_role_e_t role_e;
} cs_mode_2_step_data_t;

typedef union
{
    void *ptr;
    cs_mode_3_role_i_co_t      *common;
    cs_mode_3_role_i_spbr_ss_t *spbr_ss;
} cs_mode_3_role_i_t;

typedef union
{
    void *ptr;
    cs_mode_3_role_r_co_t      *common;
    cs_mode_3_role_r_spbr_ss_t *spbr_ss;
} cs_mode_3_role_r_t;

typedef union
{
    cs_mode_3_role_i_t role_i;
    cs_mode_3_role_r_t role_r;
} cs_mode_3_step_data_t;

typedef union
{
    void *ptr;
    cs_mode_0_step_data_t mode_0;
    cs_mode_1_step_data_t mode_1;
    cs_mode_2_step_data_t mode_2;
    cs_mode_3_step_data_t mode_3;
} cs_mode_spec_step_data_t;

typedef struct
{
    cs_step_mode_t step_mode;
    uint8_t        step_channel;
    /// @brief Length for mode- and role-specific information being reported
    uint8_t        step_data_len;
    /// @brief see `cs_mode_role_spec_info_t` is subject to `mode_type` & `role`
    uint8_t        step_data[];
} __attribute__((packed)) cs_step_params_t;

typedef uint8_t cs_proc_done_t;
enum cs_proc_done
{
    /// all results complete for the CS procedure
    CS_PROC_DONE_ALL,
    /// partial results with more to follow for the CS procedure
    CS_PROC_DONE_PARTIAL,
    /// all subsequent CS procedure aborted
    CS_PROC_DONE_ABORT = 0x0F,
};

typedef uint8_t cs_subevent_done_t;
enum cs_subevent_done_type
{
    /// all results complete for the CS subevent
    CS_SUBEVENT_DONE_ALL,
    /// partial results with more to follow for the CS subevent
    CS_SUBEVENT_DONE_PARTIAL,
    /// all subsequent CS subevent aborted
    CS_SUBEVENT_DONE_ABORT = 0x0F,
};

typedef uint8_t cs_proc_abort_t;
enum cs_proc_abort
{
    /// report with no abort
    CS_PROC_NO_ABORT,
    /// abort because of Local host or Remote request
    CS_PROC_LR_ABORT,
    /// abort because of Filtered Channel Map (FCM) has less than 15 channels
    CS_PROC_FCM_ABORT,
    /// abort because of the Channel Map Update Instant (CMUI) has passed
    CS_PROC_CMUI_ABORT,
    /// abort because of unspecified reasons
    CS_PROC_UNSPEC_ABORT = 0x0F,
};

typedef uint8_t cs_subevent_abort_t;
enum cs_subevent_abort
{
    /// report with no abort
    CS_SUBEVENT_NO_ABORT,
    /// abort because of Local host or Remote request
    CS_SUBEVENT_LR_ABORT,
    /// abort because of No CS Sync (mode-0) received
    CS_SUBEVENT_NCSS_ABORT,
    /// abort because of Scheduling Conflicts or Limited Resources
    CS_SUBEVENT_SCLR_ABORT,
    /// abort because of unspecified reasons
    CS_SUBEVENT_UNSPEC_ABORT = 0x0F,
};

#define REFERENCE_POWER_LEVEL_NOT_APPLICABLE    0x7F

typedef struct
{
    /// @brief Starting ACL connection event counter for the results reported in the event.
    uint16_t           start_acl_conn_evt_counter;
    /// @brief CS procedure count since completion of the Channel Sounding Security Start procedure.
    uint16_t           proc_counter;
    /// @brief Frequency compensation value in units of 0.01 ppm (15-bit signed integer).
    ///
    /// Range: -100ppm ~ 100ppm, 0x58F0 ~ 0x2710.
    ///
    /// Units: 0.01ppm
    uint16_t           freq_compensation;
    /// @brief range: -127 ~ 20, unit: dBm.
    ///
    /// 0x7F: Reference power level is not applicable.
    uint8_t            reference_power_level;
    /// @brief
    /// 0x0 = All results complete for the CS procedure
    ///
    /// 0x1 = Partial results with more to follow for the CS procedure
    ///
    /// 0xF = All subsequent CS procedures aborted
    cs_proc_done_t     proc_done_status;
    /// @brief
    /// 0x0 = All results complete for the CS subevent
    ///
    /// 0x1 = Partial results with more to follow for the CS subevent
    ///
    /// 0xF = Current CS subevent aborted
    cs_subevent_done_t subevent_done_status;
    /// @brief two-part
    ///
    /// - bit0-3: Indicates the abort reason when Procedure_Done_Status is set to 0xF, otherwise the default value is set to zero.
    ///
    /// - bit4-7: Indicates the abort reason when Subevent_Done_Status is set to 0xF, otherwise the default value is set to zero.
    uint8_t            abort_reason;
    uint8_t            num_antenna_paths;
    uint8_t            num_steps_reported;
} __attribute__((packed)) cs_subevent_header_t;

typedef struct
{
    cs_subevent_header_t sub_header;
    /// `step_params` num is subject to `num_steps_reported`
    /// don't worry about this, the lower layer will free the whole `subevent_result` data, include the step_params at the tail
    /// so just access step_params by the num_steps_reported
    cs_step_params_t     step_params[];
} __attribute__((packed)) cs_subevent_result_t;

typedef struct
{
    /// @see `enum cs_role_enable_type`
    uint8_t                          role_enable;
    cs_sync_antenna_selection_type_t cs_sync_antenna_selection;
    /// range: -127 ~ 20, units: dBm
    uint8_t                          max_tx_power;
} __attribute__((packed)) cs_default_settings_t;

#define CS_TEST_SET_TRANSMIT_POWER_LEVEL_MAX    0x7E
#define CS_TEST_SET_TRANSMIT_POWER_LEVEL_MIN    0x7F

#define CS_TEST_OVERRIDE_CONFIG_BIT_NUM         11   // b0-b10 is meaningful (except bit-1)
#define CS_TEST_OVERRIDE_CONFIG_B0_MASK         CO_BIT_MASK(0)
#define CS_TEST_OVERRIDE_CONFIG_B2_MASK         CO_BIT_MASK(2)
#define CS_TEST_OVERRIDE_CONFIG_B3_MASK         CO_BIT_MASK(3)
#define CS_TEST_OVERRIDE_CONFIG_B4_MASK         CO_BIT_MASK(4)
#define CS_TEST_OVERRIDE_CONFIG_B5_MASK         CO_BIT_MASK(5)
#define CS_TEST_OVERRIDE_CONFIG_B6_MASK         CO_BIT_MASK(6)
#define CS_TEST_OVERRIDE_CONFIG_B7_MASK         CO_BIT_MASK(7)
#define CS_TEST_OVERRIDE_CONFIG_B8_MASK         CO_BIT_MASK(8)
/// Stable Phase test
#define CS_TEST_OVERRIDE_CONFIG_B10_MASK        CO_BIT_MASK(10)

typedef struct
{
    cs_step_mode_t                   main_mode_type;
    cs_step_mode_t                   sub_mode_type;
    /// @brief The number of main mode steps taken from the end of the last CS subevent to be repeated
    ///         at the beginning of the current CS subevent directly after the last mode-0 step of that event.
    /// Range: 0x00~0x03.
    uint8_t                          main_mode_repetition;
    /// @brief Number of CS mode-0 steps to be included at the beginning of the test CS subevent. Range: 0x01~0x03.
    uint8_t                          mode_0_steps;
    cs_role_t                        role;
    cs_rtt_type_t                    rtt_type;
    cs_sync_phy_t                    cs_sync_phy;
    cs_sync_antenna_selection_type_t cs_sync_antenna_selection;
    /// @brief Duration for each CS subevent in microseconds. Range: 1250us~4s; 0x000004E2~0x003D0900.
    uint32_t                         subevent_len;
    /// @brief Time between consecutive CS subevents anchored off the same ACL connection event. Units: 0.625ms.
    uint16_t                         subevent_interval;
    /// @brief Range: 0x00~0x20.
    /// 0x00: The Max_Num_Subevents parameter is ignored when determining the number of subevents in the procedure.
    ///
    /// 0x01~0x20: The maximum allowed number of subevents in the procedure.
    uint8_t                          max_num_subevents;
    /// @brief
    /// 0xXX: Set transmitter to the specified or nearest transmit power level. Range: -127~+20. Units: dBm
    ///
    /// 0x7E: Set transmitter to minimum transmit power level.
    ///
    /// 0x7F: Set transmitter to maximum transmit power level.
    uint8_t                          transmit_power_level;
    /// @brief Interlude time in microseconds between the RTT packet.
    ///
    /// Valid values: 0x0A, 0x14, 0x1E, 0x28, 0x32, 0x3C, 0x50, or 0x91
    uint8_t                          t_ip1_time;
    /// @brief Interlude time in microseconds between the CS tones.
    ///
    /// Valid values: 0x0A, 0x14, 0x1E, 0x28, 0x32, 0x3C, 0x50, or 0x91
    uint8_t                          t_ip2_time;
    /// @brief Time in microseconds for frequency changes.
    ///
    /// Valid values: 0x0F, 0x14, 0x1E, 0x28, 0x32, 0x3C, 0x50, 0x64, 0x78, or 0x96
    uint8_t                          t_fcs_time;
    /// @brief Time in microseconds for the phase measurement period of the CS tones.
    ///
    /// Valid values: 0x0A, 0x14, or 0x28
    uint8_t                          t_pm_time;
    /// @brief Time in microseconds for the antenna switch period of the CS tones.
    ///
    /// Valid values: 0x00, 0x01, 0x02, 0x04, or 0x0A
    uint8_t                          t_sw_time;
    /// @brief Antenna Configuration Index. `Range`: 0x00 ~ 0x07.
    uint8_t                          tone_antenna_config_selection;
    cs_snr_control_t                 snr_control_initiator;
    cs_snr_control_t                 snr_control_reflector;
    /// @brief The DRBG_Nonce value determines octets 14 and 15 of the initial value of the DRBG nonce.
    uint8_t                          drbg_nonce;
    /// @brief The number of times the Channel_Map field will be cycled through for non-mode-0 steps within a CS procedure. Range: 0x01~0xFF.
    uint8_t                          channel_map_repetition;
    uint16_t                         override_config;
    union
    {
        struct
        {
            uint8_t  num_channels;
            /// @brief `num_channels` bytes
            uint8_t *p_channels;
        } set;
        struct
        {
            uint8_t  channel_map[CS_CHANNEL_MAP_SIZE];
            uint8_t  channel_selection_type;
            uint8_t  ch3c_shape;
            uint8_t  ch3c_jump;
        } not_set;
    } opd_b0;
    struct
    {
        uint8_t  main_mode_steps;
    } opd_b2;
    struct
    {
        uint8_t  t_pm_tone_ext;
    } opd_b3;
    struct
    {
        uint8_t  tone_antenna_permutation;
    } opd_b4;
    struct
    {
        uint32_t cs_sync_aa_initiator;
        uint32_t cs_sync_aa_reflector;
    } opd_b5;
    struct
    {
        uint8_t  ss_marker1_position;
        uint8_t  ss_marker2_position;
    } opd_b6;
    struct
    {
        uint8_t  ss_marker_value;
    } opd_b7;
    struct
    {
        uint8_t  cs_sync_payload_pattern;
        uint8_t  cs_sync_user_payload[CS_SYNC_PAYLOAD_SIZE];
    } opd_b8;
} __attribute__((packed)) cs_test_params_t;

typedef struct
{
    uint16_t              connhdl;
    uint8_t               config_id;
    uint16_t              length;
    cs_subevent_result_t *p_sub_res;
} cs_recv_sub_res_param_t;

typedef struct
{
    uint8_t err_code;
} cs_event_err_param_t;

typedef cs_event_err_param_t cs_event_read_local_capas_cmpl_t;
typedef cs_event_err_param_t cs_event_set_channel_classification_cmpl_t;
typedef cs_event_err_param_t cs_event_test_start_cmpl_t;
typedef cs_event_err_param_t cs_event_test_end_cmpl_t;

typedef struct
{
    uint16_t connhdl;
} cs_event_conn_co_param_t;

typedef cs_event_conn_co_param_t cs_event_connected_param_t;
typedef cs_event_conn_co_param_t cs_event_disconnected_param_t;

typedef struct
{
    uint16_t connhdl;
    uint8_t  err_code;
} cs_event_conn_err_param_t;

typedef cs_event_conn_err_param_t cs_event_read_remote_capas_status_t;
typedef cs_event_conn_err_param_t cs_event_security_enable_status_t;
typedef cs_event_conn_err_param_t cs_event_read_remote_fae_table_status_t;
typedef cs_event_conn_err_param_t cs_event_test_end_status_t;

typedef cs_event_conn_err_param_t cs_event_read_remote_capas_cmpl_t;
typedef cs_event_conn_err_param_t cs_event_write_cached_remote_capas_cmpl_t;
typedef cs_event_conn_err_param_t cs_event_set_default_settings_cmpl_t;
typedef cs_event_conn_err_param_t cs_event_read_remote_fae_table_cmpl_t;
typedef cs_event_conn_err_param_t cs_event_write_cached_remote_fae_table_cmpl_t;
typedef cs_event_conn_err_param_t cs_event_security_enable_cmpl_t;

typedef struct
{
    uint16_t connhdl;
    uint8_t  config_id;
    uint8_t  err_code;
} cs_event_conn_cfg_err_param_t;

typedef cs_event_conn_cfg_err_param_t cs_event_create_config_status_t;
typedef cs_event_conn_cfg_err_param_t cs_event_remove_config_status_t;
typedef cs_event_conn_cfg_err_param_t cs_event_proc_enable_status_t;
typedef cs_event_conn_cfg_err_param_t cs_event_proc_disable_status_t;

typedef cs_event_conn_cfg_err_param_t cs_event_create_config_cmpl_t;
typedef cs_event_conn_cfg_err_param_t cs_event_remove_config_cmpl_t;
typedef cs_event_conn_cfg_err_param_t cs_event_set_proc_params_cmpl_t;
typedef cs_event_conn_cfg_err_param_t cs_event_proc_enable_cmpl_t;
typedef cs_event_conn_cfg_err_param_t cs_event_proc_disable_cmpl_t;

typedef union
{
    void *param_ptr;
    cs_event_read_remote_capas_status_t     *read_remote_capas;
    cs_event_security_enable_status_t       *security_enable;
    cs_event_read_remote_fae_table_status_t *read_remote_fae_table;
    cs_event_create_config_status_t         *create_config;
    cs_event_remove_config_status_t         *remove_config;
    cs_event_proc_enable_status_t           *proc_enable;
    cs_event_proc_disable_status_t          *proc_disable;
    cs_event_test_end_status_t              *test_end;
} cs_cmd_status_param_t;

typedef union
{
    void *param_ptr;
    cs_event_read_local_capas_cmpl_t              *read_local_capas;
    cs_event_read_remote_capas_cmpl_t             *read_remote_capas;
    cs_event_write_cached_remote_capas_cmpl_t     *write_remote_cached_capas;
    cs_event_set_default_settings_cmpl_t          *set_default_settings;
    cs_event_read_remote_fae_table_cmpl_t         *read_remote_fae_table;
    cs_event_write_cached_remote_fae_table_cmpl_t *write_cached_fae_table;
    cs_event_security_enable_cmpl_t               *security_enabled;
    cs_event_create_config_cmpl_t                 *create_config;
    cs_event_remove_config_cmpl_t                 *remove_config;
    cs_event_proc_enable_cmpl_t                   *proc_enable;
    cs_event_proc_disable_cmpl_t                  *proc_disable;
    cs_event_set_channel_classification_cmpl_t    *set_channel_classification;
    cs_event_set_proc_params_cmpl_t               *p_set_proc_params;
    cs_event_test_start_cmpl_t                    *start_test;
    cs_event_test_end_cmpl_t                      *end_test;
} cs_cmd_cmpl_param_t;

typedef union
{
    void *param_ptr;
    cs_cmd_status_param_t status;
    cs_cmd_cmpl_param_t   cmpl;
} cs_cmd_param_t;

typedef enum
{
    CS_STATUS_CMD_READ_REMOTE_CAPAS,
    CS_STATUS_CMD_SECURITY_ENABLE,
    CS_STATUS_CMD_READ_REMOTE_FAE_TABLE,
    CS_STATUS_CMD_CREATE_CONFIG,
    CS_STATUS_CMD_REMOVE_CONFIG,
    CS_STATUS_CMD_PROC_ENABLE,
    CS_STATUS_CMD_PROC_DISABLE,
    CS_STATUS_CMD_TEST_END,
} cs_status_cmd_type_t;

typedef enum
{
    CS_CMPL_CMD_READ_LOCAL_CAPAS,
    CS_CMPL_CMD_READ_REMOTE_CAPAS,
    CS_CMPL_CMD_WRITE_REMOTE_CACHED_CAPAS,
    CS_CMPL_CMD_SET_DEFAULT_SETTINGS,
    CS_CMPL_CMD_READ_REMOTE_FAE_TABLE,
    CS_CMPL_CMD_WRITE_CACHED_FAE_TABLE,
    CS_CMPL_CMD_SECURITY_ENABLE,
    CS_CMPL_CMD_CREATE_CONFIG,
    CS_CMPL_CMD_REMOVE_CONFIG,
    CS_CMPL_CMD_PROC_ENABLE,
    CS_CMPL_CMD_PROC_DISABLE,
    CS_CMPL_CMD_SET_CHANNEL_CLASSIFICATION,
    CS_CMPL_CMD_SET_PROC_PARAMS,
    CS_CMPL_CMD_TEST_START,
    CS_CMPL_CMD_TEST_END,
} cs_cmpl_cmd_type_t;

typedef struct
{
    cs_status_cmd_type_t   cmd_type;
    cs_cmd_status_param_t  status_param;
} cs_cmd_status_wrapped_param_t;

typedef struct
{
    cs_cmpl_cmd_type_t   cmd_type;
    cs_cmd_cmpl_param_t  cmpl_param;
} cs_cmd_cmpl_wrapped_param_t;

typedef union
{
    void *param_ptr;
    cs_event_connected_param_t    *connected;
    cs_event_disconnected_param_t *disconnected;
    cs_cmd_status_wrapped_param_t *cmd_status;
    cs_cmd_cmpl_wrapped_param_t   *cmd_cmpl;
    cs_recv_sub_res_param_t       *recv_sub_res;
} cs_event_param_t;

typedef enum
{
    CS_EVENT_CONN_CONNECTED = BT_EVENT_CS_EVENT_START,
    CS_EVENT_CONN_DISCONNECTED,
    CS_EVENT_COMMAND_STATUS,
    CS_EVENT_COMMAND_CMPL,
    CS_EVENT_RECV_SUBEVENT_RESULT,
} cs_event_t;

typedef enum
{
    CS_EVENT_RAP_CALLBACK,
    CS_EVENT_APP_CALLBACK,
    CS_EVENT_TEST_CALLBACK,
    CS_EVENT_CALLBACK_MAX_NUM,
} cs_event_callback_type_t;

extern const uint8_t g_cs_m0_data_len_map[CS_STEP_MODE_MASK_BIT_NUM];
extern const uint8_t g_cs_m1_data_len_map[CS_STEP_MODE_MASK_BIT_NUM];

typedef void (*cs_event_handle_func_t)(const cs_event_param_t param);

bool cs_security_is_enabled(uint16_t connhdl);
bool cs_proc_is_enabled(uint16_t connhdl);
bool cs_proc_is_enabling(uint16_t connhdl);
bool cs_proc_is_disabling(uint16_t connhdl);
bool cs_proc_is_disabled(uint16_t connhdl);

/**
 * @brief get the config id that procedure enable command enables
 *
 * @param connhdl connection handle
 * @return uint8_t : config id that procedure enabled, exact config id if procedure enabled, else `CS_INVALID_CONFIG_ID`
 */
uint8_t cs_get_proc_enabled_config_id(uint16_t connhdl);

uint8_t cs_get_gap_conidx_by_connhdl(uint16_t connhdl);
ble_bdaddr_t cs_get_peer_addr_by_connhdl(uint16_t connhdl);
uint16_t cs_get_connhdl_by_peer_addr(const ble_bdaddr_t *p_ble_bdaddr);

typedef int (*cs_event_cb_t)(cs_event_t event, cs_event_param_t param);

bt_status_t cs_register_callback(cs_event_callback_type_t cb_type, cs_event_cb_t cb);

bt_status_t cs_cmd_read_local_supp_capas(void);
bt_status_t cs_cmd_read_remote_supp_capas(uint16_t connhdl);
bt_status_t cs_cmd_write_cached_remote_supp_capas(uint16_t connhdl, const cs_supp_capas_t *p_supp_capas);
bt_status_t cs_cmd_enable_security(uint16_t connhdl);
bt_status_t cs_cmd_set_default_settings(uint8_t connhdl, const cs_default_settings_t *p_default_settings);
bt_status_t cs_cmd_read_remote_fae_table(uint16_t connhdl);
bt_status_t cs_cmd_write_cached_remote_fae_table(uint8_t connhdl, const uint8_t remote_fae_table[CS_CHANNEL_NUM]);
bt_status_t cs_cmd_create_config(uint16_t connhdl, uint8_t config_id, cs_create_config_context_t create_context, const cs_basic_config_t *p_basic_config);
bt_status_t cs_cmd_remove_config(uint16_t connhdl, uint8_t config_id);
bt_status_t cs_cmd_set_channel_classification(const uint8_t channel_classification[CS_CHANNEL_MAP_SIZE]);
bt_status_t cs_cmd_set_procedure_parameters(uint16_t connhdl, uint8_t config_id, const cs_set_proc_params_t *p_set_proc_params);
bt_status_t cs_cmd_enable_procedure(uint16_t connhdl, uint8_t config_id);
bt_status_t cs_cmd_disable_procedure(uint16_t connhdl, uint8_t config_id);
bt_status_t cs_cmd_start_test(const cs_test_params_t *p_param);
bt_status_t cs_cmd_end_test();

cs_rtt_type_t cs_config_get_rtt_type(uint16_t connhdl, uint8_t config_id);
uint8_t cs_get_selected_tx_power(uint16_t connhdl);
cs_step_spec_type_t cs_config_get_local_step_type(uint16_t connhdl, uint8_t config_id, uint8_t mode);
cs_step_spec_type_t cs_config_get_remote_step_type(uint16_t connhdl, uint8_t config_id, uint8_t mode);
cs_role_t cs_get_role_by_step_type(cs_step_spec_type_t step_type);
uint8_t cs_get_step_data_len_by_spec_type(cs_step_spec_type_t step_type);

#if 0
bt_status_t cs_config_refresh_main_mode_type(uint16_t connhdl, uint8_t config_id, uint8_t main_mode_type);
bt_status_t cs_config_refresh_sub_mode_type(uint16_t connhdl, uint8_t config_id, uint8_t sub_mode_type);
bt_status_t cs_config_refresh_min_main_mode_steps(uint16_t connhdl, uint8_t config_id, uint8_t min_main_mode_steps);
bt_status_t cs_config_refresh_max_main_mode_steps(uint16_t connhdl, uint8_t config_id, uint8_t max_main_mode_steps);
bt_status_t cs_config_refresh_main_mode_repetition(uint16_t connhdl, uint8_t config_id, uint8_t main_mode_repetition);
bt_status_t cs_config_refresh_mode_0_steps(uint16_t connhdl, uint8_t config_id, uint8_t mode_0_steps);
bt_status_t cs_config_refresh_role(uint16_t connhdl, uint8_t config_id, uint8_t role);
bt_status_t cs_config_refresh_rtt_type(uint16_t connhdl, uint8_t config_id, uint8_t rtt_type);
bt_status_t cs_config_refresh_sync_phy(uint16_t connhdl, uint8_t config_id, uint8_t cs_sync_phy);
bt_status_t cs_config_refresh_channel_map(uint16_t connhdl, uint8_t config_id, uint8_t channel_map[CS_CHANNEL_MAP_SIZE]);
bt_status_t cs_config_refresh_channel_map_repetition(uint16_t connhdl, uint8_t config_id, uint8_t channel_map_repetition);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* __CS_SERVICE_H__ */
