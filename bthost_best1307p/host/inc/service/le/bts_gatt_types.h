/****************************************************************************
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

#ifndef __BTS_GATT_TYPES_H__
#define __BTS_GATT_TYPES_H__

#include "bt_gatt_types.h"
#include "bts_common_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 ****************************************************************************************
 *     ____    _  _____ _____   _____
 *    / ___|  / \|_   _|_   _| |_   _|   _ _ __   ___  ___
 *   | |  _  / _ \ | |   | |     | || | | | '_ \ / _ \/ __|
 *   | |_| |/ ___ \| |   | |     | || |_| | |_) |  __/\__ \
 *    \____/_/   \_\_|   |_|     |_| \__, | .__/ \___||___/
 *                                   |___/|_|
 *
 ****************************************************************************************
 */

#define BTS_GATT_ARRAY_SIZE_1                                           (1)

#define BTS_ATT_NO_PERM                                                 (0x00)
#define BTS_ATT_SEC_NONE                                                (0x00)
#define BTS_ATT_RD_PERM                                                 (0x01)
#define BTS_ATT_RD_MITM_AUTH                                            (0x02)
#define BTS_ATT_RD_AUTHOR                                               (0x04)
#define BTS_ATT_RD_ENC                                                  (0x08)
#define BTS_ATT_WR_PERM                                                 (0x10)
#define BTS_ATT_WR_MITM_AUTH                                            (0x20)
#define BTS_ATT_WR_AUTHOR                                               (0x40)
#define BTS_ATT_WR_ENC                                                  (0x80)

#define BTS_ATT_FLAG_IS_128_UUID                                        (0x01)
#define BTS_ATT_FLAG_CONST_VALUE                                        (0x02)
#define BTS_ATT_FLAG_OPTION_CHAR                                        (0x04)
#define BTS_ATT_FLAG_ARRAY_VALID                                        (0x08)
#define BTS_ATT_FLAG_IND_AUTH                                           (0x10)
#define BTS_ATT_FLAG_IND_ENC                                            (0x20)
#define BTS_ATT_FLAG_NTF_AUTH                                           (0x40)
#define BTS_ATT_FLAG_NTF_ENC                                            (0x80)

#define BTS_GATT_NO_PROP                                                (0x00)
#define BTS_GATT_BROADCAST                                              (0x01) // permit broadcasts of char value using SCCD, if set, SCCD shall exist
#define BTS_GATT_RD_REQ                                                 (0x02) // permit read req of the char value
#define BTS_GATT_WR_CMD                                                 (0x04) // permit write cmd of the char value
#define BTS_GATT_WR_REQ                                                 (0x08) // permit write req of the char value
#define BTS_GATT_NTF_PROP                                               (0x10) // permit notification of the char value, if set, CCCD shall exist
#define BTS_GATT_IND_PROP                                               (0x20) // permit indication of the char value with ack, if set, CCCD shall exist
#define BTS_GATT_SIGNED_WR                                              (0x40) // permit authenticated signed write of the char value
#define BTS_GATT_EXT_PROP                                               (0x80) // additional char properties are defined in the CEPD, if set, CEPD shall exist
#define BTS_GATT_EXT_PROP_RELIABLE_WRITE                                (0x0001) // permit reliable writes of the char value
#define BTS_GATT_EXT_PROP_WRITABLE_AUX                                  (0x0002) // permit write CUSD descriptor

/* BTS GATT DECLARATIONS */
#define BTS_GATT_UUID_PRI_SERVICE                                       (0x2800) // Primary Service
#define BTS_GATT_UUID_SEC_SERVICE                                       (0x2801) // Secondary Service
#define BTS_GATT_UUID_INCLUDE_SERVICE                                   (0x2802) // Include
#define BTS_GATT_UUID_CHAR_DECLARE                                      (0x2803) // Characteristic

/* BTS GATT DESCRIPTORS */
#define BTS_GATT_DESC_UUID_CHAR_EXT_PROP                                (0x2900) // Characteristic Extended Properties
#define BTS_GATT_DESC_UUID_CHAR_USER_DESC                               (0x2901) // Characteristic User Description
#define BTS_GATT_DESC_UUID_CHAR_CLIENT_CONFIG                           (0x2902) // Client Characteristic Configuration
#define BTS_GATT_DESC_UUID_CHAR_SERVER_CONFIG                           (0x2903) // Server Characteristic Configuration
#define BTS_GATT_DESC_UUID_CHAR_PRESENT_FORMAT                          (0x2904) // Characteristic Presentation Format
#define BTS_GATT_DESC_UUID_CHAR_AGGREGATE_FORMAT                        (0x2905) // Characteristic Aggregate Format
#define BTS_GATT_DESC_UUID_CHAR_VALID_RANGE                             (0x2906) // Characteristic Valid Range
#define BTS_GATT_DESC_UUID_EXT_REPORT_REFERENCE                         (0x2907) // External Report Reference
#define BTS_GATT_DESC_UUID_REPORT_REFERENCE                             (0x2908) // Report Reference
#define BTS_GATT_DESC_UUID_NUM_DIGITALS                                 (0x2909) // Number of Digitals
#define BTS_GATT_DESC_UUID_VALUE_TRIGGER_SETTING                        (0x290A) // Value Trigger Setting
#define BTS_GATT_DESC_UUID_ES_CONFIGURATION                             (0x290B) // Environmental Sensing Configuration
#define BTS_GATT_DESC_UUID_ES_MEASUREMENT                               (0x290C) // Environmental Sensing Measurement
#define BTS_GATT_DESC_UUID_ES_TRIGGER_SETTING                           (0x290D) // Envrionmental Sensing Trigger Setting
#define BTS_GATT_DESC_UUID_TIME_TRIGGER_SETTING                         (0x290E) // Time Trigger Setting
#define BTS_GATT_DESC_UUID_COMPLETE_BREDR_TBD                           (0x290F) // Copmlete BR-EDR Transport Block Data

#define BTS_ATT_RW_PERM_FROM_PROP(p) \
    ((((p) & (BTS_GATT_RD_REQ)) ? BTS_ATT_RD_PERM : 0) | \
     (((p) & (BTS_GATT_WR_CMD|BTS_GATT_WR_REQ|BTS_GATT_SIGNED_WR)) ? BTS_ATT_WR_PERM : 0))

#define BTS_UINT8_VALUE(a) ((uint8_t)((a)&0xFF))

#define BTS_SPLIT_UINT16_LE(a) \
    BTS_UINT8_VALUE(a), BTS_UINT8_VALUE((a)>>8)

#define BTS_SPLIT_128_UUID_LE(uuid_be) \
    BTS_UINT8_VALUE(((uint8_t [16](uuid_be)))[15]), \
    BTS_UINT8_VALUE(((uint8_t [16](uuid_be)))[14]), \
    BTS_UINT8_VALUE(((uint8_t [16](uuid_be)))[13]), \
    BTS_UINT8_VALUE(((uint8_t [16](uuid_be)))[12]), \
    BTS_UINT8_VALUE(((uint8_t [16](uuid_be)))[11]), \
    BTS_UINT8_VALUE(((uint8_t [16](uuid_be)))[10]), \
    BTS_UINT8_VALUE(((uint8_t [16](uuid_be)))[9]), \
    BTS_UINT8_VALUE(((uint8_t [16](uuid_be)))[8]), \
    BTS_UINT8_VALUE(((uint8_t [16](uuid_be)))[7]), \
    BTS_UINT8_VALUE(((uint8_t [16](uuid_be)))[6]), \
    BTS_UINT8_VALUE(((uint8_t [16](uuid_be)))[5]), \
    BTS_UINT8_VALUE(((uint8_t [16](uuid_be)))[4]), \
    BTS_UINT8_VALUE(((uint8_t [16](uuid_be)))[3]), \
    BTS_UINT8_VALUE(((uint8_t [16](uuid_be)))[2]), \
    BTS_UINT8_VALUE(((uint8_t [16](uuid_be)))[1]), \
    BTS_UINT8_VALUE(((uint8_t [16](uuid_be)))[0])

#define BTS_ATT_16BIT_UUID_WITH_FLAGS(uuid, perm, flags) \
    BTS_UINT8_VALUE(flags), \
    BTS_UINT8_VALUE(perm), \
    BTS_SPLIT_UINT16_LE(uuid)

#define BTS_ATT_128BE_UUID_WITH_FLAGS(uuid_128_be, perm, flags) \
    BTS_UINT8_VALUE(BTS_ATT_FLAG_IS_128_UUID|flags), \
    BTS_UINT8_VALUE(perm), \
    BTS_SPLIT_128_UUID_LE(uuid_128_be)

#define BTS_ATT_16BIT_UUID(uuid, perm) \
    BTS_ATT_16BIT_UUID_WITH_FLAGS(uuid, perm, 0)

#define BTS_ATT_128BE_UUID(uuid_128_be, perm) \
    BTS_ATT_128BE_UUID_WITH_FLAGS(uuid_128_be, perm, 0)

#define BTS_ATT_16BIT_CONST_VALUE(uuid, perm) \
    BTS_ATT_16BIT_UUID_WITH_FLAGS(uuid, perm, BTS_ATT_FLAG_CONST_VALUE)

#define BTS_ATT_128BE_CONST_VALUE(uuid_128_be, perm) \
    BTS_ATT_128BE_UUID_WITH_FLAGS(uuid_128_be, perm, BTS_ATT_FLAG_CONST_VALUE)

#define BTS_GATT_EXTERN_SERVICE(name) \
    extern const uint8_t name[]

#define BTS_GATT_DECL_PRI_SERVICE(name, service_uuid) \
    const uint8_t name[] = { \
        BTS_ATT_16BIT_UUID(BTS_GATT_UUID_PRI_SERVICE, ATT_RD_PERM), \
        BTS_SPLIT_UINT16_LE(service_uuid), \
    }

#define BTS_GATT_DECL_128_BE_PRI_SERVICE(name, service_128_uuid_be) \
    const uint8_t name[] = { \
        BTS_ATT_16BIT_UUID(BTS_GATT_UUID_PRI_SERVICE, ATT_RD_PERM), \
        BTS_SPLIT_128_UUID_LE(service_128_uuid_be), \
    }

#define BTS_GATT_DECL_128_LE_PRI_SERVICE(name, service_128_uuid_le) \
    const uint8_t name[] = { \
        BTS_ATT_16BIT_UUID(BTS_GATT_UUID_PRI_SERVICE, ATT_RD_PERM), \
        service_128_uuid_le, \
    }

#define BTS_GATT_DECL_SEC_SERVICE(name, service_uuid) \
    const uint8_t name[] = { \
        BTS_ATT_16BIT_UUID(BTS_GATT_UUID_SEC_SERVICE, ATT_RD_PERM), \
        BTS_SPLIT_UINT16_LE(service_uuid), \
    }

#define BTS_GATT_DECL_128_BE_SEC_SERVICE(name, service_128_uuid_be) \
    const uint8_t name[] = { \
        BTS_ATT_16BIT_UUID(BTS_GATT_UUID_SEC_SERVICE, ATT_RD_PERM), \
        BTS_SPLIT_128_UUID_LE(service_128_uuid_be), \
    }

#define BTS_GATT_DECL_128_LE_SEC_SERVICE(name, service_128_uuid_le) \
    const uint8_t name[] = { \
        BTS_ATT_16BIT_UUID(BTS_GATT_UUID_SEC_SERVICE, ATT_RD_PERM), \
        service_128_uuid_le, \
    }

#define BTS_GATT_DECL_INC_SERVICE(name, included_service_name) \
    static const bts_gatt_decl_inc_service_t name = { \
        BTS_ATT_16BIT_UUID(BTS_GATT_UUID_INCLUDE_SERVICE, ATT_RD_PERM), \
        (uint32_t)(uintptr_t)(included_service_name), /* included service address */ \
    }

#define BTS_GATT_DECL_CHAR(char_name, char_uuid, char_prop, char_value_sec_perm) \
    static const uint8_t char_name[] = { \
        BTS_ATT_16BIT_UUID(BTS_GATT_UUID_CHAR_DECLARE, ATT_RD_PERM), \
        BTS_UINT8_VALUE(char_prop), \
        BTS_ATT_16BIT_UUID(char_uuid, BTS_ATT_RW_PERM_FROM_PROP(char_prop)|char_value_sec_perm), \
    }

#define BTS_GATT_DECL_CHAR_WITH_FLAG(char_name, char_uuid, char_prop, char_value_sec_perm, flag) \
    static const uint8_t char_name[] = { \
        BTS_ATT_16BIT_UUID(BTS_GATT_UUID_CHAR_DECLARE, ATT_RD_PERM), \
        BTS_UINT8_VALUE(char_prop), (flag) | \
        BTS_ATT_16BIT_UUID(char_uuid, BTS_ATT_RW_PERM_FROM_PROP(char_prop)|char_value_sec_perm), \
    }

// if the value is read-only, but the server can change it, it is not const value
#define BTS_GATT_DECL_CHAR_WITH_CONST_VALUE(char_name, char_uuid, char_prop, char_value_sec_perm, ...) \
    static const uint8_t char_name[] = { \
        BTS_ATT_16BIT_UUID(BTS_GATT_UUID_CHAR_DECLARE, ATT_RD_PERM), \
        BTS_UINT8_VALUE(char_prop), \
        BTS_ATT_16BIT_CONST_VALUE(char_uuid, BTS_ATT_RW_PERM_FROM_PROP(char_prop)|char_value_sec_perm), \
        __VA_ARGS__, \
    }

#define BTS_GATT_DECL_OPTIONAL_CHAR(char_name, char_uuid, char_prop, char_value_sec_perm) \
    static const uint8_t char_name[] = { \
        BTS_ATT_16BIT_UUID(BTS_GATT_UUID_CHAR_DECLARE, ATT_RD_PERM), \
        BTS_UINT8_VALUE(char_prop), BTS_ATT_FLAG_OPTION_CHAR | \
        BTS_ATT_16BIT_UUID(char_uuid, BTS_ATT_RW_PERM_FROM_PROP(char_prop)|char_value_sec_perm), \
    }

#define BTS_GATT_DECL_OPTIONAL_CHAR_WITH_FLAG(char_name, char_uuid, char_prop, char_value_sec_perm, flag) \
    static const uint8_t char_name[] = { \
        BTS_ATT_16BIT_UUID(BTS_GATT_UUID_CHAR_DECLARE, ATT_RD_PERM), \
        BTS_UINT8_VALUE(char_prop), BTS_ATT_FLAG_OPTION_CHAR | (flag) | \
        BTS_ATT_16BIT_UUID(char_uuid, BTS_ATT_RW_PERM_FROM_PROP(char_prop)|char_value_sec_perm), \
    }

// if the value is read-only, but the server can change it, it is not const value
#define BTS_GATT_DECL_OPTIONAL_CHAR_WITH_CONST_VALUE(char_name, char_uuid, char_prop, char_value_sec_perm, ...) \
    static const uint8_t char_name[] = { \
        BTS_ATT_16BIT_UUID(BTS_GATT_UUID_CHAR_DECLARE, ATT_RD_PERM), \
        BTS_UINT8_VALUE(char_prop), BTS_ATT_FLAG_OPTION_CHAR | \
        BTS_ATT_16BIT_CONST_VALUE(char_uuid, BTS_ATT_RW_PERM_FROM_PROP(char_prop)|char_value_sec_perm), \
        __VA_ARGS__, \
    }

#define BTS_GATT_DECL_128_BE_CHAR(char_name, char_128_uuid_be, char_prop, char_value_sec_perm) \
    static const uint8_t char_name[] = { \
        BTS_ATT_16BIT_UUID(BTS_GATT_UUID_CHAR_DECLARE, ATT_RD_PERM), \
        BTS_UINT8_VALUE(char_prop), \
        BTS_UINT8_VALUE(BTS_ATT_FLAG_IS_128_UUID), \
        BTS_UINT8_VALUE(BTS_ATT_RW_PERM_FROM_PROP(char_prop)|char_value_sec_perm), \
        BTS_SPLIT_128_UUID_LE(char_128_uuid_be), \
    }

#define BTS_GATT_DECL_128_LE_CHAR(char_name, char_128_uuid_le, char_prop, char_value_sec_perm) \
    static const uint8_t char_name[] = { \
        BTS_ATT_16BIT_UUID(BTS_GATT_UUID_CHAR_DECLARE, ATT_RD_PERM), \
        BTS_UINT8_VALUE(char_prop), \
        BTS_UINT8_VALUE(BTS_ATT_FLAG_IS_128_UUID), \
        BTS_UINT8_VALUE(BTS_ATT_RW_PERM_FROM_PROP(char_prop)|char_value_sec_perm), \
        char_128_uuid_le, \
    }

// if the value is read-only, but the server can change it, it is not const value
#define BTS_GATT_DECL_128_BE_CHAR_WITH_CONST_VALUE(char_name, char_128_uuid_be, char_prop, char_value_sec_perm, ...) \
    static const uint8_t char_name[] = { \
        BTS_ATT_16BIT_UUID(BTS_GATT_UUID_CHAR_DECLARE, ATT_RD_PERM), \
        BTS_UINT8_VALUE(char_prop), \
        BTS_UINT8_VALUE(BTS_ATT_FLAG_IS_128_UUID|BTS_ATT_FLAG_CONST_VALUE), \
        BTS_UINT8_VALUE(BTS_ATT_RW_PERM_FROM_PROP(char_prop)|char_value_sec_perm), \
        BTS_SPLIT_128_UUID_LE(char_128_uuid_be), \
        __VA_ARGS__, \
    }

// if the value is read-only, but the server can change it, it is not const value
#define BTS_GATT_DECL_128_LE_CHAR_WITH_CONST_VALUE(char_name, char_128_uuid_le, char_prop, char_value_sec_perm, ...) \
    static const uint8_t char_name[] = { \
        BTS_ATT_16BIT_UUID(BTS_GATT_UUID_CHAR_DECLARE, ATT_RD_PERM), \
        BTS_UINT8_VALUE(char_prop), \
        BTS_UINT8_VALUE(BTS_ATT_FLAG_IS_128_UUID|BTS_ATT_FLAG_CONST_VALUE), \
        BTS_UINT8_VALUE(BTS_ATT_RW_PERM_FROM_PROP(char_prop)|char_value_sec_perm), \
        char_128_uuid_le, \
        __VA_ARGS__, \
    }

#define BTS_GATT_DECL_OPTIONAL_128_BE_CHAR(char_name, char_128_uuid_be, char_prop, char_value_sec_perm) \
    static const uint8_t char_name[] = { \
        BTS_ATT_16BIT_UUID(BTS_GATT_UUID_CHAR_DECLARE, ATT_RD_PERM), \
        BTS_UINT8_VALUE(char_prop), \
        BTS_UINT8_VALUE(BTS_ATT_FLAG_IS_128_UUID|BTS_ATT_FLAG_OPTION_CHAR), \
        BTS_UINT8_VALUE(BTS_ATT_RW_PERM_FROM_PROP(char_prop)|char_value_sec_perm), \
        BTS_SPLIT_128_UUID_LE(char_128_uuid_be), \
    }

#define BTS_GATT_DECL_OPTIONAL_128_LE_CHAR(char_name, char_128_uuid_le, char_prop, char_value_sec_perm) \
    static const uint8_t char_name[] = { \
        BTS_ATT_16BIT_UUID(BTS_GATT_UUID_CHAR_DECLARE, ATT_RD_PERM), \
        BTS_UINT8_VALUE(char_prop), \
        BTS_UINT8_VALUE(BTS_ATT_FLAG_IS_128_UUID|BTS_ATT_FLAG_OPTION_CHAR), \
        BTS_UINT8_VALUE(BTS_ATT_RW_PERM_FROM_PROP(char_prop)|char_value_sec_perm), \
        char_128_uuid_le, \
    }

// if the value is read-only, but the server can change it, it is not const value
#define BTS_GATT_DECL_OPTIONAL_128_BE_CHAR_WITH_CONST_VALUE(char_name, char_128_uuid_be, char_prop, char_value_sec_perm, ...) \
    static const uint8_t char_name[] = { \
        BTS_ATT_16BIT_UUID(BTS_GATT_UUID_CHAR_DECLARE, ATT_RD_PERM), \
        BTS_UINT8_VALUE(char_prop), \
        BTS_UINT8_VALUE(BTS_ATT_FLAG_IS_128_UUID|BTS_ATT_FLAG_OPTION_CHAR|BTS_ATT_FLAG_CONST_VALUE), \
        BTS_UINT8_VALUE(BTS_ATT_RW_PERM_FROM_PROP(char_prop)|char_value_sec_perm), \
        BTS_SPLIT_128_UUID_LE(char_128_uuid_be), \
        __VA_ARGS__, \
    }

// if the value is read-only, but the server can change it, it is not const value
#define BTS_GATT_DECL_OPTIONAL_128_LE_CHAR_WITH_CONST_VALUE(char_name, char_128_uuid_le, char_prop, char_value_sec_perm, ...) \
    static const uint8_t char_name[] = { \
        BTS_ATT_16BIT_UUID(BTS_GATT_UUID_CHAR_DECLARE, ATT_RD_PERM), \
        BTS_UINT8_VALUE(char_prop), \
        BTS_UINT8_VALUE(BTS_ATT_FLAG_IS_128_UUID|BTS_ATT_FLAG_OPTION_CHAR|BTS_ATT_FLAG_CONST_VALUE), \
        BTS_UINT8_VALUE(BTS_ATT_RW_PERM_FROM_PROP(char_prop)|char_value_sec_perm), \
        char_128_uuid_le, \
        __VA_ARGS__, \
    }

// Characteristic Extended Properties Descriptor (CEPD)
#define BTS_GATT_DECL_CEPD_DESCRIPTOR(name, char_ext_prop) \
    static const uint8_t name[] = { \
        BTS_ATT_16BIT_CONST_VALUE(BTS_GATT_DESC_UUID_CHAR_EXT_PROP, ATT_RD_PERM), \
        BTS_SPLIT_UINT16_LE(char_ext_prop), \
    }

// Characteristic User Description Descriptor (CUDD)
#define BTS_GATT_DECL_CUDD_DESCRIPTOR(name, perm) \
    static const uint8_t name[] = { \
        BTS_ATT_16BIT_UUID(BTS_GATT_DESC_UUID_CHAR_USER_DESC, ATT_RD_PERM|perm), \
    }

#define BTS_GATT_DECL_CONST_CUDD_DESCRIPTOR(name, ...) \
    static const uint8_t name[] = { \
        BTS_ATT_16BIT_CONST_VALUE(BTS_GATT_DESC_UUID_CHAR_USER_DESC, ATT_RD_PERM), \
        __VA_ARGS__ \
    }

// Client Characteristic Configuration Descriptor (CCCD)
#define BTS_GATT_DECL_CCCD_DESCRIPTOR(name, write_sec_perm) \
    static const uint8_t name[] = { \
        BTS_ATT_16BIT_UUID(BTS_GATT_DESC_UUID_CHAR_CLIENT_CONFIG, ATT_RD_PERM|ATT_WR_PERM|(write_sec_perm)), \
    }

// Server Characteristic Configuration Descriptor (SCCD)
#define BTS_GATT_DECL_SCCD_DESCRIPTOR(name, write_sec_perm) \
    static const uint8_t name[] = { \
        BTS_ATT_16BIT_UUID(BTS_GATT_DESC_UUID_CHAR_SERVER_CONFIG, ATT_RD_PERM|ATT_WR_PERM|(write_sec_perm)), \
    }

// Characteristic Presentation Format Descriptor (CPFD)
#define BTS_GATT_DECL_CPFD_DESCRIPTOR(name) \
    static const uint8_t name[] = { \
        BTS_ATT_16BIT_UUID(BTS_GATT_DESC_UUID_CHAR_PRESENT_FORMAT, ATT_RD_PERM), \
    }

#define BTS_GATT_DECL_CONST_CPFD_DESCRIPTOR(name, format, exponent, unit, name_space, description) \
    static const uint8_t name[] = { \
        BTS_ATT_16BIT_CONST_VALUE(BTS_GATT_DESC_UUID_CHAR_PRESENT_FORMAT, ATT_RD_PERM), \
        BTS_UINT8_VALUE(format), \
        BTS_UINT8_VALUE(exponent), \
        BTS_SPLIT_UINT16_LE(unit), \
        BTS_UINT8_VALUE(name_space), \
        BTS_SPLIT_UINT16_LE(description), \
    }

// External Report Reference Characteristic Descriptor (ERRD)
#define BTS_GATT_DECL_ERRD_DESCRIPTOR(name, ext_refer_char_uuid) \
    static const uint8_t name[] = { \
        BTS_ATT_16BIT_CONST_VALUE(BTS_GATT_DESC_UUID_EXT_REPORT_REFERENCE, ATT_RD_PERM), \
        BTS_SPLIT_UINT16_LE(ext_refer_char_uuid), \
    }

// Report Reference Characteristic Descriptor (RRCD)
#define BTS_GATT_DECL_RRCD_DESCRIPTOR(name) \
    static const uint8_t name[] = { \
        BTS_ATT_16BIT_UUID(BTS_GATT_DESC_UUID_REPORT_REFERENCE, ATT_RD_PERM), \
    }

#define BTS_GATT_DECL_COSNT_RRCD_DESCRIPTOR(name, report_id, report_type) \
    static const uint8_t name[] = { \
        BTS_ATT_16BIT_CONST_VALUE(BTS_GATT_DESC_UUID_REPORT_REFERENCE, ATT_RD_PERM), \
        BTS_UINT8_VALUE(report_id), \
        BTS_UINT8_VALUE(report_type), \
    }

#define BTS_GATT_DECL_DESCRIPTOR(name, uuid, att_perm) \
    static const uint8_t name[] = { \
        BTS_ATT_16BIT_UUID(uuid, att_perm), \
    }

#define BTS_GATT_DECL_128_LE_DESCRIPTOR(name, desc_uuid_128_le, att_perm) \
    const uint8_t name[] = { \
        BTS_UINT8_VALUE(BTS_ATT_FLAG_IS_128_UUID), \
        BTS_UINT8_VALUE(att_perm), \
        desc_uuid_128_le, \
    }

#define bts_gatt_include_attribute(a) {(bt_attr_byte_array_t*)(&a), sizeof(bts_gatt_decl_inc_service_t)}

#define bts_gatt_include_instance(a, inst_id) {(bt_attr_byte_array_t*)(&a), sizeof(bts_gatt_decl_inc_service_t), inst_id}

#define bts_gatt_char_attribute(a, inst_id) {(bt_attr_byte_array_t*)(a), sizeof(a), inst_id}

#define bts_gatt_attribute(a) {(bt_attr_byte_array_t*)(a), sizeof(a)}

/*
 * ENUMERATIONS
*/
typedef enum
{
    BTS_ATT_ERROR_RSP                           = 0x01,
    BTS_ATT_EXCHANGE_MTU_REQ                    = 0x02,
    BTS_ATT_EXCHANGE_MTU_RSP                    = 0x03,
    BTS_ATT_FIND_INFO_REQ                       = 0x04,
    BTS_ATT_FIND_INFO_RSP                       = 0x05,
    BTS_ATT_FIND_BY_TYPE_VALUE_REQ              = 0x06,
    BTS_ATT_FIND_BY_TYPE_VALUE_RSP              = 0x07,
    BTS_ATT_READ_BY_TYPE_REQ                    = 0x08,
    BTS_ATT_READ_BY_TYPE_RSP                    = 0x09,
    BTS_ATT_READ_REQ                            = 0x0A,
    BTS_ATT_READ_RSP                            = 0x0B,
    BTS_ATT_READ_BLOB_REQ                       = 0x0C,
    BTS_ATT_READ_BLOB_RSP                       = 0x0D,
    BTS_ATT_READ_MULTIPLE_REQ                   = 0x0E,
    BTS_ATT_READ_MULTIPLE_RSP                   = 0x0F,
    BTS_ATT_READ_BY_GROUP_TYPE_REQ              = 0x10,
    BTS_ATT_READ_BY_GROUP_TYPE_RSP              = 0x11,
    BTS_ATT_WRITE_REQ                           = 0x12,
    BTS_ATT_WRITE_RSP                           = 0x13,
    BTS_ATT_PREPARE_WRITE_REQ                   = 0x16,
    BTS_ATT_PREPARE_WRITE_RSP                   = 0x17,
    BTS_ATT_EXECUTE_WRITE_REQ                   = 0x18,
    BTS_ATT_EXECUTE_WRITE_RSP                   = 0x19,
    BTS_ATT_READ_MULTI_VAR_REQ                  = 0x20,
    BTS_ATT_READ_MULTI_VAR_RSP                  = 0x21,
    BTS_ATT_MULTI_HANDLE_VALUE_NTF              = 0x23,
    BTS_ATT_HANDLE_VALUE_NTF                    = 0x1B,
    BTS_ATT_HANDLE_VALUE_IND                    = 0x1D,
    BTS_ATT_HANDLE_VALUE_CFM                    = 0x1E,
    BTS_ATT_WRITE_CMD                           = 0x52,
    BTS_ATT_SIGNED_WRITE_CMD                    = 0xD2,
} bts_att_opcode_e;

typedef enum
{
    /// Stop notification/indication
    BTS_GATT_CLI_STOP_NTFIND = 0x0000,
    /// Start notification
    BTS_GATT_CLI_START_NTF   = 0x0001,
    /// Start indication
    BTS_GATT_CLI_START_IND   = 0x0002,
} bts_gatt_cli_cfg_e;

typedef enum
{
    BTS_GATT_NO_ERR = 0,
    /// Problem with ATTC protocol response
    BTS_GATT_ERR_INVALID_ATT_LEN,
    /// Error in service search
    BTS_GATT_ERR_INVALID_TYPE_IN_SVC_SEARCH,
    /// Invalid write data
    BTS_GATT_ERR_WRITE,
    /// Signed write error
    BTS_GATT_ERR_SIGNED_WRITE,
    /// No attribute client defined
    BTS_GATT_ERR_ATTRIBUTE_CLIENT_MISSING,
    /// No attribute server defined
    BTS_GATT_ERR_ATTRIBUTE_SERVER_MISSING,
    /// Permission set in service/attribute are invalid
    BTS_GATT_ERR_INVALID_PERM,
    /// The Attribute bearer is closed
    BTS_GATT_ERR_ATT_BEARER_CLOSE,
    /// No more Attribute bearer available
    BTS_GATT_ERR_NO_MORE_BEARER,

    /// parameter Invalid
    BTS_GATT_ERR_INVALID,
    /// parameter null pointer
    BTS_GATT_ERR_POINTER_NULL,

} bts_gatt_err_code_e;

/*
 * TYPEDEF
*/
typedef struct
{
    uint8_t attr_flags;
    uint8_t attr_perm;
    uint8_t uuid_le_l;
    uint8_t uuid_le_h;
    uint32_t inc_srvc_addr;
} bts_gatt_decl_inc_service_t;

typedef struct
{
    uint8_t uuid_le[BT_GATT_UUID_128_LEN];
    uint8_t uuid_128_bit;
    uint8_t uuid_len;
} bts_gatt_char_uuid_t;

struct bt_gatt_peer_char_node_t;

typedef struct bt_gatt_peer_char_node_t
{
    struct bt_gatt_peer_char_node_t *next;
    bt_gatt_peer_serv_t *service;
    uint8_t count; // same uuid char count
    uint16_t char_uuid; // set when 16-bit uuid
    bt_gatt_peer_char_t character[BTS_GATT_ARRAY_SIZE_1];
} bt_gatt_peer_char_node_t;

#ifdef __cplusplus
}
#endif

#endif /* __BTS_GATT_TYPES_H__ */
