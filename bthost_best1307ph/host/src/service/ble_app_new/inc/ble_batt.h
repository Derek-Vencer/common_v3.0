/***************************************************************************
 *
 * Copyright 2015-2025 BES.
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
#ifndef __BLE_BATT_H__
#define __BLE_BATT_H__
#if defined(BLE_BATT_ENABLE)
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BAS_CHARGE_TYPE_UNKNOWN_OR_NOT_CHARGING = 0,
    BAS_CHARGE_TYPE_CONSTANT_CURRENT,
    BAS_CHARGE_TYPE_CONSTANT_VOLTAGE,
    BAS_CHARGE_TYPE_TRICKLE,
    BAS_CHARGE_TYPE_FLOAT,
} bas_charge_type_t;

typedef struct {
    /// The bits of this field represent the presence of optional fields
    uint8_t flags;
    /// This field represents the battery power status including the presence of the battery, whether it is
    /// connected to an external power source, and whether it is charging or discharging
    uint8_t power_state;
    /// Service identifier; should match Battery Level CPFD Description or "main".
    uint16_t identifier;
    /// Battery level percentage (0..100), same as Battery Level characteristic.
    uint8_t battery_level;
    /// Additional status: service required and battery fault tri-state bits.
    uint8_t     additional_status;
} __attribute__((packed))bas_batt_level_status_t;

typedef struct {
    /// Gregorian year-month_day (e.g., 2025).
    uint8_t date[3];
} bas_estimated_service_date_t;

typedef struct {
    /// The Battery Critical Status
    uint8_t critical_status;
} bas_batt_critical_status_t;

typedef struct {
    /// The bits of this field represent the presence of optional fields
    uint8_t flags;
    /// Total power drawn from external source (aggregation or instance scope).
    int16_t external_source_power;
    /// Present terminal voltage (e.g., in mV).
    uint16_t present_voltage;
    /// Available energy at current state of charge.
    uint8_t available_energy;
    /// Available battery capacity at full charge in current condition.
    uint16_t available_battery_capacity;
    /// Energy flow rate (positive=charging, negative=discharging).
    int16_t  charge_rate;
    /// Available energy observed at last charge completion.
    uint16_t available_energy_at_last_charge;
} __attribute__((packed))bas_batt_energy_status_t;

typedef struct {
    /// The bits of this field represent the presence of optional fields
    uint8_t flags;
    /// Estimated minutes until fully discharged (mandatory).
    uint16_t time_until_discharged_min;
    /// Estimated minutes until fully discharged if in standby (optional).
    uint16_t time_until_discharged_on_standby_min;
    /// Estimated minutes until fully recharged (optional).
    uint16_t time_until_recharged_min;
} __attribute__((packed))bas_batt_time_status_t;

typedef struct {
    /// The bits of this field represent the presence of optional fields
    uint8_t flags;
    /// Overall battery health (100=new; 0=end-of-life). Implementation-defined.
    uint8_t   health_summary_percent;
    /// Charge cycle count (one cycle equals full-charge capacity discharge in current condition).
    uint16_t  cycle_count;
    /// Current battery temperature (unit/resolution per GSS; see spec 3.7.1.4 for behavior).
    int8_t   current_temperature;
    /// Number of deep discharges (fully discharged events).
    uint16_t  deep_discharge_count;
} __attribute__((packed))bas_batt_health_status_t;

typedef struct {
    /// The bits of this field represent the presence of optional fields
    uint8_t flags;
    /// Designed number of charge cycles (lifetime).
    uint16_t cycle_count_designed_lifetime;
    /// Designed minimum operating temperature (unit/resolution per GSS).
    int8_t  min_designed_operating_temperature;
    /// Designed maximum operating temperature (unit/resolution per GSS).
    int8_t  max_designed_operating_temperature;
} __attribute__((packed))bas_batt_health_info_t;

typedef enum {
    BAS_CHEMISTRY_UNKNOWN = 0,
    BAS_CHEMISTRY_LI_ION,
    BAS_CHEMISTRY_LI_POLYMER,
    BAS_CHEMISTRY_NIMH,
    BAS_CHEMISTRY_NICD,
    BAS_CHEMISTRY_LEAD_ACID,
    /// Extend with additional chemistries per Assigned Numbers/GSS.
} bas_battery_chemistry_t;

typedef struct {
    /// Gregorian year (e.g., 2025).
    uint16_t year;
    /// Month (1..12).
    uint8_t  month;
    /// Day (1..31).
    uint8_t  day;
} bas_date_t;

typedef struct {
    /// The bits of this field represent the presence of optional fields
    uint8_t flags;
    /// Mandatory Battery Features bit set (wire form is a bitmask).
    uint8_t features;
    /// Battery manufacture date.
    bas_date_t battery_manufacture_date;
    /// Battery expiration date.
    bas_date_t battery_expiration_date;
    /// Designed capacity when new (unit/width per GSS).
    uint16_t   battery_designed_capacity;
    /// Low energy threshold (used with Available Energy to set Low level).
    uint8_t   battery_low_energy;
    /// Critical energy threshold (used with Available Energy to set Critical level).
    uint8_t   battery_critical_energy;
    /// Battery chemistry type.
    bas_battery_chemistry_t chemistry;
    /// Nominal voltage (unit per GSS, e.g., mV).
    uint16_t   nominal_voltage;
    /// Battery Aggregation Group identifier.
    uint16_t   aggregation_group;
} __attribute__((packed))bas_batt_info_t;

bt_status_t app_ble_report_battery_level(uint8_t instance, uint8_t battery_level);
bt_status_t app_ble_report_batt_level_status(uint8_t instance, bas_batt_level_status_t level_status, uint8_t send_mode);
bt_status_t app_ble_report_estimated_service_date(uint8_t instance, bas_estimated_service_date_t date, uint8_t send_mode);
bt_status_t app_ble_report_batt_critical_status(uint8_t instance, bas_batt_critical_status_t critical);
bt_status_t app_ble_report_batt_energy_status(uint8_t instance, bas_batt_energy_status_t energy, uint8_t send_mode);
bt_status_t app_ble_report_batt_time_status(uint8_t instance, bas_batt_time_status_t time_status, uint8_t send_mode);
bt_status_t app_ble_report_batt_health_status(uint8_t instance, bas_batt_health_status_t health_status, uint8_t send_mode);
bt_status_t app_ble_report_batt_health_info(uint8_t instance, bas_batt_health_info_t health_info);
bt_status_t app_ble_report_batt_info(uint8_t instance, bas_batt_info_t info);
bt_status_t app_ble_report_mfr_name(uint8_t instance, uint8_t *name, uint8_t len);
bt_status_t app_ble_report_model_number(uint8_t instance, uint8_t *model_number, uint8_t len);
bt_status_t app_ble_report_serial_number(uint8_t instance, uint8_t *serial_number, uint8_t len);

#ifdef __cplusplus
    }
#endif
#endif /* BLE_BATT_ENABLE */
#endif /* __BLE_BATT_H__ */
