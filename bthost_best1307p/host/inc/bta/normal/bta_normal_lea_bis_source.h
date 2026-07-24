/**
 * @brief Bluetooth Service Low Energy Audio Unicast Client
 *
 * @copyright Copyright (c) 2015-20223 BES Technic.
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
 */
#ifndef __BTA_NORMAL_LEA_BIS_SOURCE_H__
#define __BTA_NORMAL_LEA_BIS_SOURCE_H__

/****************************** header include ********************************/
#include <stdint.h>


/**
 * @brief Audio type enumeration for BTA LEA BIS source
 */
typedef enum
{
    BTA_AUDIO_TYPE_UNKONW = 0,
    BTA_AUDIO_TYPE_LINEIN,
    BTA_AUDIO_TYPE_USB,
    BTA_AUDIO_TYPE_WIFI,
    BTA_AUDIO_TYPE_A2DP,
    BTA_AUDIO_TYPE_SMF,
    BTA_AUDIO_TYPE_MAX,
} bta_input_audio_type;

/**
 * @brief BTA LEA BIS configuration parameters structure
 *
 * This structure holds the configuration parameters for LEA BIS source operations,
 * including broadcast ID, encryption settings, advertising data, and callback functions.
 */
typedef struct bta_bis_cfg_param_t
{
    uint8_t broadcast_id[3];                    /**< Broadcast identifier */
    uint8_t encrypt_enable;                     /**< Encryption enable flag */
    uint8_t encrypt_key[16];                    /**< Encryption key */
    uint8_t adv_mfrs_data_len;                  /**< Advertising manufacturer data length */
    uint8_t *adv_mfrs_data;                     /**< Advertising manufacturer data */
    uint8_t sync_policy;                        /**< Synchronization policy */
    uint16_t company_id;                        /**< Company identifier */
    const char* bcast_name;                     /**< Broadcast name */
    void (*bt_svc_bis_stream_state_cb)(bool state, uint8_t role); /**< Stream state callback function */
    void (*bt_svc_bis_param_set_cb)(bta_bis_cfg_param_t *param); /**< Parameter set callback function */
} bta_bis_cfg_param_t;

/**
 * @brief Set audio type for LEA BIS source
 *
 * Configures the audio type for the LEA BIS source, determining the input source type.
 *
 * @param audio_type The audio type to be set
 * @return void
 */
void bta_lea_bis_src_set_audio_type(uint8_t audio_type);

/**
 * @brief Open LEA BIS source connection
 *
 * Opens a LEA BIS (Broadcast Isochronous Streams) source connection with the specified configuration.
 *
 * @param cfg_param Pointer to BTA layer BIS configuration parameters
 * @return uint8_t 0 on success, other value on failure
 */
uint8_t bta_lea_bis_src_open(bta_bis_cfg_param_t *cfg_param);

/**
 * @brief Close LEA BIS source connection
 *
 * Closes the LEA BIS source connection and releases associated resources.
 *
 * @return uint8_t 0 on success, other value on failure
 */
uint8_t bta_lea_bis_src_close();


#endif