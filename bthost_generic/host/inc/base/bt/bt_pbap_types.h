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

#pragma once
#include "bt_base_types.h"

#define BT_PBAP_FOLDER_NAME_MAX_LEN 32

// map to pbap_api.h: PBAP_MAX_VCARD_ENTRY_NAME_SIZE
#define BT_PBAP_MAX_VCARD_ENTRY_NAME_SIZE 42

#define BT_PBAP_MAX_VCARD_CONTACT_NAME_SIZE 80

#define BT_PBAP_MAX_VCARD_FORAMTTED_NAME_SIZE 80

#define BT_PBAP_MAX_VCARD_CONTACT_ADDR_SIZE 128

#define BT_PBAP_MAX_VCARD_CONTACT_TEL_SIZE 32

#define BT_PBAP_MAX_VCARD_CONTACT_EMAIL_SIZE 64

#define BT_PBAP_MAX_VCARD_CONTACT_X_BT_UID_PREFIX_SIZE 9

#define BT_PBAP_MAX_VCARD_CONTACT_X_BT_UID_SIZE 40

#define BT_PBAP_MAX_VCARD_CONTACT_DATE_SIZE 32

#define BT_PBAP_MAX_VCARD_CONTACT_TYPE_SIZE 16

#define BT_PBAP_TELECOM_FOLDER_NAME                "telecom"
#define BT_PBAP_SIM1_FOLDER_NAME                   "SIM1"
#define BT_PBAP_MAIN_PHONEBOOK_FOLDER_NAME         "pb"
#define BT_PBAP_MISSED_CALL_HISTORY_FOLDER_NAME    "mch"
#define BT_PBAP_INCOMING_CALL_HISTORY_FOLDER_NAME  "ich"
#define BT_PBAP_OUTGOING_CALL_HISTORY_FOLDER_NAME  "och"

/**
 ****************************************************************************************
 * @brief  PBAP request search parameter structure
 * @details This structure contains all parameters required for PBAP phonebook search request,
 *          including target path, search telephone number and return count limitation.
 ****************************************************************************************
 */
typedef struct {
    char* child_path_name;          /**< Target child path name for phonebook search (string must end with '\0') */
    uint8_t child_path_name_len;    /**< String length of child_path_name */
    char* search_tel_num;           /**< Telephone number used for vCard search */
    uint8_t search_tel_len;         /**< String length of search_tel_num */
    uint16_t start_pb_index;        /**< Start index of phonebook entries to return */
    uint16_t pb_max_return_count;   /**< Maximum number of phonebook entries to return in one request */

    // if need, can add search params
} bt_pbap_request_search_param_t;

/**
 ****************************************************************************************
 * @brief  PBAP search record structure
 * @details This structure is used to record the status and result of PBAP vCard search by telephone number,
 *          including search keyword and execution status flags.
 ****************************************************************************************
 */
typedef struct {
    uint8_t search_tel_len;                     /**< Length of search_tel_num */
    char search_tel_num[BT_PBAP_MAX_VCARD_CONTACT_TEL_SIZE];  /**< Telephone number for search (buffer includes '\0' terminator) */
    bool list_done;                             /**< Flag indicating whether list search operation is completed */
    bool pull_done;                             /**< Flag indicating whether pull operation is completed */
    bool pull_finished;                         /**< Flag indicating whether pull operation is fully finished (all entries received) */
} bt_pbap_search_recrod_t;

typedef struct {
    uint8_t entry_name_len;
    uint8_t contact_name_len;
    char vcard_entry_name[BT_PBAP_MAX_VCARD_ENTRY_NAME_SIZE+1];
    char vcard_contact_name[BT_PBAP_MAX_VCARD_CONTACT_NAME_SIZE+1];
} bt_pbap_vcard_listing_item_t;

typedef struct {
    uint8_t formatted_name_len;
    uint8_t contact_name_len;
    uint8_t contact_address_len;
    uint8_t contact_tel_len;
    uint8_t contact_email_len;
    uint8_t contact_x_bt_uid_len;
    uint8_t contact_date_len;
    uint8_t contact_type_len;
    char formatted_name[BT_PBAP_MAX_VCARD_FORAMTTED_NAME_SIZE+1];
    char contact_name[BT_PBAP_MAX_VCARD_CONTACT_NAME_SIZE+1];
    char contact_addr[BT_PBAP_MAX_VCARD_CONTACT_ADDR_SIZE+1];
    char contact_tel[BT_PBAP_MAX_VCARD_CONTACT_TEL_SIZE+1];
    char contact_email[BT_PBAP_MAX_VCARD_CONTACT_EMAIL_SIZE+1];
    char context_x_bt_uid_start[BT_PBAP_MAX_VCARD_CONTACT_X_BT_UID_PREFIX_SIZE];
    char contact_x_bt_uid[BT_PBAP_MAX_VCARD_CONTACT_X_BT_UID_SIZE+1];
    char contact_date[BT_PBAP_MAX_VCARD_CONTACT_DATE_SIZE+1];
    char contact_type[BT_PBAP_MAX_VCARD_CONTACT_TYPE_SIZE+1];
} bt_pbap_vcard_pulling_item_t;

/**
 ****************************************************************************************
 * @brief  PBAP report parameter structure
 * @details This structure encapsulates the response data and status of PBAP phonebook operations,
 *          including error code, phonebook size, and pointers to listing/pulling result items.
 ****************************************************************************************
 */
typedef struct {
    uint8_t error_code;                          /**< PBAP operation error code */
    uint16_t phonebook_size;                     /**< Total size/count of entries in the target PBAP phonebook */
    bt_pbap_vcard_listing_item_t *list_rsp_item; /**< Pointer to vCard listing response items */
    bt_pbap_vcard_pulling_item_t *pull_rsp_item; /**< Pointer to vCard pulling response items */
} bt_pbap_report_param_t;

typedef enum {
    BT_PBAP_CONN_STATE_DISCONNECTED = 0,
    BT_PBAP_CONN_STATE_CONNECTED,
    BT_PBAP_CONN_STATE_OBEX_CONNECTED,
} bt_pbap_conn_state_t;

typedef enum {
    BT_PBAP_PHONEBOOK_SIZE_RSP = 0,
    BT_PBAP_LIST_STATE_ITEM_RSP,
    BT_PBAP_LIST_STATE_DONE,
    BT_PBAP_PULL_STATE_ENTRY_RSP,
    BT_PBAP_PULL_STATE_DONE,
    BT_PBAP_PULL_STATE_FINISHED,
} bt_pbap_search_state_t;

/**
 ****************************************************************************************
 * @brief  PBAP connection state callback function type
 * @details This callback is triggered when PBAP connection state changes (connect/disconnect),
 *          and returns connection state and related report parameters.
 * @param  addr: Pointer to the Bluetooth address of the remote device
 * @param  state: Current PBAP connection state (enum bt_pbap_conn_state_t)
 * @param  param: Pointer to PBAP report parameter structure (contains error code if state is failed)
 * @return None
 ****************************************************************************************
 */
typedef void (*bt_pbap_connection_state_cb)(const bt_bdaddr_t *addr, bt_pbap_conn_state_t state, bt_pbap_report_param_t* param);

/**
 ****************************************************************************************
 * @brief  PBAP search result callback function type
 * @details This callback is triggered when PBAP search operation (listing/pulling) has result or completes,
 *          and returns search state and related report parameters (including vCard data).
 * @param  addr: Pointer to the Bluetooth address of the remote device
 * @param  state: Current PBAP search state (enum bt_pbap_search_state_t)
 * @param  param: Pointer to PBAP report parameter structure (contains vCard data)
 * @return None
 ****************************************************************************************
 */
typedef void (*bt_pbap_search_result_cb)(const bt_bdaddr_t* addr, bt_pbap_search_state_t state, bt_pbap_report_param_t* param);

typedef enum {
    BT_PBAP_CB_USER_BTA = 0,
    BT_PBAP_CB_USER_APP,
    BT_PBAP_CB_USER_DEPRECATED,
    BT_PBAP_CB_USER_MODULE_1,
    BT_PBAP_CB_USER_CUSTOMER,
    BT_PBAP_CB_USER_MAX,
} bt_pbap_callback_user_t;

/**
 ****************************************************************************************
 * @brief  PBAP callback function collection structure
 * @details This structure encapsulates all callback functions for PBAP operations,
 *          including connection state change and search result notification callbacks.
 ****************************************************************************************
 */
typedef struct
{
    bt_pbap_connection_state_cb             connection_state_cb;  /**< Callback for PBAP connection state changes (connect/disconnect/fail) */
    bt_pbap_search_result_cb                search_result_cb;     /**< Callback for PBAP search operation results (listing/pulling complete/failed) */
} bt_pbap_callbacks_t;