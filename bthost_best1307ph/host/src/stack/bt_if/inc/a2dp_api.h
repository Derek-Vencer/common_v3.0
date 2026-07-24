/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#ifndef _A2DP_API_H__
#define _A2DP_API_H__
#include "bluetooth.h"
#include "avrcp_api.h"
#include "conmgr_api.h"
#ifdef __cplusplus
extern "C" {
#endif

#define A2DP_SOURCE_NON_TYPE_NON         0
#define A2DP_SOURCE_NON_TYPE_LHDC        1
#define A2DP_SOURCE_NON_TYPE_LHDCV5      2
#define A2DP_SOURCE_NON_TYPE_LDAC        3
#define A2DP_SOURCE_NON_TYPE_MIHC        4


#define MAX_A2DP_VOL   (127)

#define BTIF_CONFIG_A2DP_SBC_ON 1

#if defined(A2DP_AAC_ON) || defined(A2DP_SOURCE_AAC_ON)
#define BTIF_CONFIG_A2DP_AAC_ON 1
#else
#define BTIF_CONFIG_A2DP_AAC_ON 0
#endif

#if defined(A2DP_LHDC_ON) || defined(A2DP_LHDCV5_ON) || defined(A2DP_SOURCE_LHDCV5_ON)
#define BTIF_CONFIG_A2DP_LHDC_ON 2
#else
#define BTIF_CONFIG_A2DP_LHDC_ON 0
#endif

#if defined(A2DP_LDAC_ON)
#define BTIF_CONFIG_A2DP__ON 1
#define BTIF_CONFIG_A2DP_LDAC_ON 1
#else
#define BTIF_CONFIG_A2DP_LDAC_ON 0
#endif

#if defined(A2DP_SCALABLE_ON)
#define BTIF_CONFIG_A2DP_SCALABLE_ON 1
#else
#define BTIF_CONFIG_A2DP_SCALABLE_ON 0
#endif

#if defined(MASTER_USE_OPUS) || defined(ALL_USE_OPUS)
#define BTIF_CONFIG_A2DP_OPUS_ON 1
#else
#define BTIF_CONFIG_A2DP_OPUS_ON 0
#endif

#if defined(A2DP_L2HC_ON)
#define BTIF_CONFIG_A2DP_L2HC_ON 1
#else
#define BTIF_CONFIG_A2DP_L2HC_ON 0
#endif

#if defined(A2DP_MIHC_ON)
#define BTIF_CONFIG_A2DP_MIHC_ON 1
#else
#define BTIF_CONFIG_A2DP_MIHC_ON 0
#endif

#define SYS_MAX_AVRCP_CHNS   BT_DEVICE_NUM

typedef uint8_t btif_a2dp_event_t;

/**  Accept or reject by calling btif_a2dp_open_stream_rsp().
 */
#define BTIF_A2DP_EVENT_STREAM_OPEN_IND           1

/** This event is  the response to a call from   btif_a2dp_open_stream() or
 *  btif_a2dp_open_stream_rsp().
 */
#define BTIF_A2DP_EVENT_STREAM_OPEN               2

/** This event is received during the establishment of an
 *  outgoing stream (btif_a2dp_open_stream()) to indicate the capabilities of the
 *  remote device.  This event may also be received after calling
 *  btif_a2dp_get_stream_capabilities().
 */
#define BTIF_A2DP_EVENT_CODEC_INFO                3

/** This event is received  during the establishment of an
 *  outgoing stream (btif_a2dp_open_stream()) to indicated the capabilities of the
 *  remote device.  This event may also be received after calling
 *  btif_a2dp_get_stream_capabilities().
 *
 */
#define BTIF_A2DP_EVENT_CP_INFO                   4

/**  If this event is received, then the remote device
 *  supports the Delay Reporting feature.
 */
#define BTIF_A2DP_EVENT_DELAY_REPORTING_INFO      5

/** This event is received  when a stream is opening and
 *  all matching capabilities have been found.  The application must call
 *  btif_a2dp_set_stream_config() to configure the stream.  If successful, the
 *  stream will be opened.  btif_a2dp_close_stream() can also be called to close
 *  the stream.  This event is only received on outgoing connections.
 */
#define BTIF_A2DP_EVENT_GET_CONFIG_IND            6

/** This event is received after calling btif_a2dp_get_stream_capabilities().  This
 *  event is received after all the capabilities of the remote device have
 *  been indicated (see BTIF_A2DP_EVENT_CODEC_INFO and BTIF_A2DP_EVENT_CP_INFO).
 */
#define BTIF_A2DP_EVENT_GET_CAP_CNF               7

/** This event is received  when an open stream has been
 *  closed.  This can happen as a result of a call to btif_a2dp_close_stream(), if
 *  the stream has been closed by the remote device, if a link loss has been
 *  detected, or if the remote device rejects a request to open the stream.
 *
 */
#define BTIF_A2DP_EVENT_STREAM_CLOSED             8

/** This event is received  when an open stream has been
 *  set to the idle state.   result of a call to btif_a2dp_idle_stream().
 */
#define BTIF_A2DP_EVENT_STREAM_IDLE               9

/** When the remote device requests streaming to begin, this event will be
 *  received .  The application can accept or reject
 *  this request by calling btif_a2dp_start_stream_rsp(). (Note: this event
 *  will be received only after BTIF_A2DP_EVENT_STREAM_OPEN_IND but it may arrive
 *  before BTIF_A2DP_EVENT_STREAM_OPEN.)
 */
#define BTIF_A2DP_EVENT_STREAM_START_IND         10

/** When streaming has been started, this event will be received.  This can happen as the result to a call to
 *  btif_a2dp_start_stream() or btif_a2dp_start_stream_rsp().
 */
#define BTIF_A2DP_EVENT_STREAM_STARTED           11

/** When streaming has been suspended, this event will be received .  as the result to a call to
 *  btif_a2dp_suspend_stream(), or when the remote device suspends the stream.
 */
#define BTIF_A2DP_EVENT_STREAM_SUSPENDED         12

/** When the remote device wishes to reconfigure an open stream, this event
 *  is received by the application.  The application can accept or reject
 *  the request by calling btif_a2dp_reconfig_stream_rsp().
 *
 */
#define BTIF_A2DP_EVENT_STREAM_RECONFIG_IND      13

/** When an open stream is reconfigured, this event is received by the
 *  application.
 *
 */
#define BTIF_A2DP_EVENT_STREAM_RECONFIG_CNF      14

/** This event is received when the remote device requests the security
 *  process to begin.  The application responds to this request by calling
 *  btif_a2dp_security_control_rsp().
 */
#define BTIF_A2DP_EVENT_STREAM_SECURITY_IND      15

/** This event is received  when the remote device responds
 *  to the security process request.
 */
#define BTIF_A2DP_EVENT_STREAM_SECURITY_CNF      16

/** When the stream is aborted, this event is received by the application.
 *  This can happen in response to a request from the remote device to abort
 *  the stream, or as the result of a call to btif_a2dp_abort_stream().  When a
 *  stream is aborted, the stream is closed.
 */
#define BTIF_A2DP_EVENT_STREAM_ABORTED           17

/** This event is received when stream data has been received from the remote
 *  device.  The data is raw and is not parsed by A2DP.  It should contain
 *  a single media packet.
 */
#define BTIF_A2DP_EVENT_STREAM_DATA_IND          18

/** This event is received when raw data has been sent to the remote device.
 *  This happens as the result of a call to btif_a2dp_stream_send_raw_packet().
 *
 */
#define BTIF_A2DP_EVENT_STREAM_PACKET_SENT       19

/** This event is received when SBC data has been sent to the remote device.
 *  This happens as the result of a call to btif_a2dp_stream_send_sbc_packet().
 */
#define BTIF_A2DP_EVENT_STREAM_SBC_PACKET_SENT   20

/** This event is received by a Source when the Sink reports the value of its
 *  buffer/processing delay.  This may happen when the stream is configured
 *  (or reconfigured), and when the stream is in the streaming state.
 *
 */
#define BTIF_A2DP_EVENT_DELAY_REPORT_IND         21

/** This event is received by a Sink when the Source acknowldeges the
 *  transmitted buffer/processing delay.
 */
#define BTIF_A2DP_EVENT_DELAY_REPORT_CNF         22

/** When the the lower level AVDTP connection is established between the
 *  local and remote devices, this event will be generated.
 *
 *  During this callback, the 'p.device' parameter contains a pointer to the
 *  device that was connected.
 */
#define BTIF_A2DP_EVENT_AVDTP_CONNECT            23

/** When the the lower level AVDTP connection is disconnected, this event
 *  will be generated.
 *
 */
#define BTIF_A2DP_EVENT_AVDTP_DISCONNECT         24

/* Confirms  has received configuration information after an
 * btif_avdtp_get_config() request.
 */
#define BTIF_A2DP_AVDTP_EVENT_GET_CFG_CNF        27

/* Confirms  has sent configuration information after an
 * btif_avdtp_open_stream() request.
 */
#define BTIF_A2DP_AVDTP_EVENT_SET_CFG_CNF        28

#define BTIF_A2DP_EVENT_STREAM_STARTED_MOCK      29

#define BTIF_A2DP_REMOTE_NOT_SUPPORT             30

#define BTIF_A2DP_EVENT_STREAM_OPEN_MOCK         31

#define BTIF_A2DP_EVENT_CODEC_INFO_MOCK          32

#define BTIF_A2DP_RECEVICE_UNKOWN_CMD            33
//user define @biao
#define BTIF_A2DP_EVENT_AVDTP_CLOSE_IND          81
#define BTIF_A2DP_EVENT_AVDTP_DISCOVER_IND       82
#define BTIF_A2DP_EVENT_AVDTP_DISCOVER_RSP       83
//#define AVDTP_EVENT_STREAM_CLOSE_IND

/* event that let app level to see a stream is good to select */
#define BTIF_A2DP_EVENT_STREAM_SELECT            84
/* event that let app level to confirm stream(sep) state */
#define BTIF_A2DP_EVENT_STREAM_STATE_CFM         85
/* event that avdtp discovery and get capability complete */
#define BTIF_A2DP_EVENT_DISCOVERY_COMPLETE       86


typedef uint16_t btif_avdtp_codec_sample_rate_t;

#define BT_A2DP_CODEC_TYPE_INVALID       0xBD

#define BT_A2DP_CODEC_TYPE_SBC           0x00

#define BT_A2DP_CODEC_TYPE_MPEG1_2_AUDIO 0x01

#define BT_A2DP_CODEC_TYPE_MPEG2_4_AAC   0x02

#define BT_A2DP_CODEC_TYPE_ATRAC         0x04

#define BT_A2DP_CODEC_TYPE_OPUS          0x08

#define BT_A2DP_CODEC_TYPE_H263          0x01

#define BT_A2DP_CODEC_TYPE_MPEG4_VSP     0x02

#define BT_A2DP_CODEC_TYPE_H263_PROF3    0x03

#define BT_A2DP_CODEC_TYPE_H263_PROF8    0x04

#define BT_A2DP_CODEC_TYPE_LHDC          0xFF

#define BT_A2DP_CODEC_TYPE_LDAC          0xFF

#define BT_A2DP_CODEC_TYPE_NON_A2DP      0xFF

#define BT_A2DP_CODEC_NONE_TYPE_INVALID  (0x00)

#define BT_A2DP_CODEC_NONE_TYPE_LHDC     (0x01)

#define BT_A2DP_CODEC_NONE_TYPE_LHDCV5   (0x02)

#define BT_A2DP_CODEC_NONE_TYPE_LDAC     (0x03)

#define BT_A2DP_CODEC_NONE_TYPE_SCALABLE (0x04)

#define BT_A2DP_CODEC_NONE_TYPE_LC3      (0x05)

#define BT_A2DP_CODEC_NONE_TYPE_L2HC     (0x06)

#define BT_A2DP_CODEC_NONE_TYPE_MIHC     (0x07)

#define BT_A2DP_CODEC_NONE_TYPE_OPUS     (0x08)

#define BT_A2DP_MAX_CODEC_ELEM_SIZE      10

#define BT_A2DP_MAX_CP_VALUE_SIZE        10

typedef uint8_t btif_avctp_event_t;

#define BTIF_AVCTP_CONNECT_EVENT             1

#define BTIF_AVCTP_CONNECT_IND_EVENT         2

#define BTIF_AVCTP_DISCONNECT_EVENT          3

#define BTIF_AVCTP_COMMAND_EVENT             4

#define BTIF_AVCTP_RESPONSE_EVENT            5

#define BTIF_AVCTP_REJECT_EVENT              6

#define BTIF_AVCTP_TX_DONE_EVENT            8

#define BTIF_AVCTP_OPERANDS_EVENT            9

#define BTIF_AVCTP_CT_SDP_INFO_EVENT        10

#define BTIF_AVCTP_TG_SDP_INFO_EVENT        11

#define BTIF_AVCTP_CONNECT_EVENT_MOCK       12

#define BTIF_AVCTP_EVENT_PLAYBACK_STATUS_CHANGE_EVENT_SUPPORT 13

#define BTIF_AVCTP_EVENT_PLAYBACK_STATUS_CHANGED 14

#define BTIF_AVRCP_BROWSING_EVENT_CONNECT                       15

#define BTIF_AVRCP_BROWSING_EVENT_DISCONNECT                    16

#define BTIF_AVRCP_OBEX_EVENT_OPEN                              17

#define BTIF_AVRCP_OBEX_EVENT_CHANNEL_CONNECTED                 18

#define BTIF_AVRCP_OBEX_EVENT_DISCONNECT                        19

#define BTIF_AVRCP_OBEX_GET_RSP                                 20


#define BTIF_AVCTP_LAST_EVENT               30

#define BTIF_AVCTP_CTYPE_CONTROL               0x00
#define BTIF_AVCTP_CTYPE_STATUS                0x01
#define BTIF_AVCTP_CTYPE_SPECIFIC_INQUIRY      0x02
#define BTIF_AVCTP_CTYPE_NOTIFY                0x03
#define BTIF_AVCTP_CTYPE_GENERAL_INQUIRY       0x04
#define BTIF_AVCTP_CTYPE_BROWSING              0x80
#define BTIF_AVCTP_CTYPE_RESERVED              0x0A

typedef struct {
    U8 version;
    U8 padding;
    U8 marker;
    U8 payloadType;
    U16 sequenceNumber;
    U32 timestamp;
    U32 ssrc;
    U8 csrcCount;
    U32 csrcList[15];
} btif_avdtp_media_header_t;

/* for Codec Specific Information Element */
#if defined(A2DP_SCALABLE_ON) || defined(A2DP_LHDCV5_ON)
#define A2D_SCALABLE_IE_SAMP_FREQ_MSK    0xFF    /* b7-b0 sampling frequency */
#endif

#if defined(A2DP_LHDC_ON)
#define A2D_LHDC__IE_SAMP_FREQ_MSK    0xFF    /* b7-b0 sampling frequency */
#endif

#define A2D_STREAM_SAMP_FREQ_MSK    0xFFFFFFFF    /* b7-b4 sampling frequency */

typedef btif_avdtp_media_header_t btif_media_header_t;

typedef struct a2dp_stream_t { /* empty */ } a2dp_stream_t; /* used to hold the pointer to struct a2dp_control_t */
typedef struct a2dp_callback_parms_t { /* empty */ } a2dp_callback_parms_t; /* used to hold the pointer to btif_a2dp_callback_parms_t */

typedef uint16_t btif_avdtp_content_prot_type_t;
typedef uint8_t btif_avdtp_capability_type_t;
typedef uint8_t btif_avdtp_codec_type_t;

typedef struct btif_avdtp_codec_t {
    btif_avdtp_codec_type_t codecType;
    uint8_t elemLen;
    uint8_t *elements;
    uint8_t *pstreamflags; 
    bool discoverable;
}__attribute__((packed)) btif_avdtp_codec_t ;

typedef struct {
    bool free;
    uint8_t state;
    a2dp_stream_t *a2dp_stream; //stack A2dpStream  object
    btif_avdtp_codec_t *prev_conn_codec;
} btif_a2dp_stream_t;

typedef struct btif_avdtp_content_prot_t {
    btif_avdtp_content_prot_type_t cpType;
    uint8_t dataLen;
    uint8_t *data;
}__attribute__((packed))  btif_avdtp_content_prot_t;

typedef struct btif_avdtp_config_request_t {
    btif_avdtp_codec_t codec;
    btif_avdtp_content_prot_t cp;
    BOOL delayReporting;
} btif_avdtp_config_request_t;

typedef struct btif_a2dp_sbc_packet_t {
    struct list_node node;
    uint16_t reserved_data_size;
    uint16_t reserved_header_size;
    uint8_t *data;
    uint16_t dataLen;
    uint16_t frameSize;
    uint8_t frameNum;  // save the number of frame when the encoder is LHDCV
    btif_bt_packet_t packet;
    uint16_t dataSent;
    uint16_t frmDataSent;
} btif_a2dp_sbc_packet_t;

typedef struct ibrt_a2dp_status_t ibrt_a2dp_status_t;
typedef struct btif_avdtp_content_prot_t btif_avdtp_content_prot_t;
typedef struct btif_avdtp_codec_t btif_avdtp_codec_t;
typedef struct btif_avdtp_capability_t btif_avdtp_capability_t;
typedef struct btif_avdtp_config_request_t btif_avdtp_config_request_t;
typedef struct btif_a2dp_sbc_packet_t btif_a2dp_sbc_packet_t;
typedef struct btif_avdtp_stream_info_t btif_avdtp_stream_info_t;
typedef void btif_av_device_t;
struct btif_get_codec_cap_t;

typedef struct btif_a2dp_callback_parms_t {
    uint8_t event;
    uint8_t role;
    uint16_t len;
    int8_t status;
    uint8_t error;
    uint8_t discReason;
    bool a2dp_closed_due_to_sdp_fail;
    bool start_stream_already_sent;
    union {
        btif_avdtp_content_prot_t *cp;
        btif_avdtp_codec_t *codec;
        btif_avdtp_capability_t *capability;
        btif_avdtp_config_request_t *configReq; /* AVDTP Config request          */
        btif_bt_packet_t *btPacket; /* Raw Transmit packet           */
        btif_a2dp_sbc_packet_t *sbcPacket;  /* SBC Transmit packet           */
        uint8_t *data;          /* Receive data                  */
        uint16_t delayMs;       /* Buffering/processing delay of
                                 * the Sink.
                                 */
        btif_avdtp_stream_info_t *streamInfo;   /* Stream information */
        btif_av_device_t *device;   /* The connecting or disconnectin device. */
        struct btif_get_codec_cap_t *get_cap;

        a2dp_stream_t    *dstStream;  /* Stream to switch */
    } p;
    void   *remDev;
    bt_bdaddr_t remote;
} btif_a2dp_callback_parms_t;

typedef enum {
    BT_A2DP_STREAM_STATE_IDLE = 0,
    BT_A2DP_STREAM_STATE_CONFIGURED,
    BT_A2DP_STREAM_STATE_OPEN,
    BT_A2DP_STREAM_STATE_STREAMING,
    BT_A2DP_STREAM_STATE_CLOSING,
    BT_A2DP_STREAM_STATE_ABORTING,
} bt_a2dp_stream_state_t;

typedef struct bt_a2dp_state_t {
    bt_bdaddr_t remote;
    bt_a2dp_stream_state_t a2dp_stream_state;
    uint8_t a2dp_is_connected;
    uint8_t a2dp_is_streaming;
    uint8_t a2dp_channel_num;
    uint8_t a2dp_channel_mode;
    uint8_t a2dp_codec_type;
    uint8_t a2dp_sample_rate;
    uint8_t a2dp_sample_bit;
    uint8_t delay_report_enabled;
} bt_a2dp_state_t;

typedef struct bt_avrcp_state_t {
    uint8_t avrcp_is_connected;
    uint8_t avrcp_playback_status;
    uint8_t remote_support_playback_status_change_event;
    uint8_t avrcp_play_pause_flag;
    uint8_t curr_abs_volume;
} bt_avrcp_state_t;

typedef enum {
    BT_A2DP_DISC_ON_PROCESS = 1,
    BT_THIS_IS_CLOSED_BG_A2DP,
    BT_DISC_A2DP_PROFILE_ONLY,
    BT_A2DP_SET_CONN_FLAG,
    BT_A2DP_SET_STREAM_STATE,
    BT_A2DP_GET_CONN_FLAG,
    BT_A2DP_GET_STREAM_STATE,
    BT_A2DP_LAST_PAUSED_DEVICE,
} BT_A2DP_FIELD_ENUM_T;

enum A2DP_ROLE
{
    A2DP_ROLE_SRC,
    A2DP_ROLE_SNK,
    A2DP_ROLE_UNKNOWN,
};



#define BTIF_AVTP_MSG_TYPE_COMMAND       0
#define BTIF_AVTP_MSG_TYPE_ACCEPT        2
#define BTIF_AVTP_MSG_TYPE_REJECT        3

#define A2DP_AAC_OCTET_NUMBER                     (6)
#define A2DP_AAC_OCTET0_MPEG2_AAC_LC              0x80
#define A2DP_AAC_OCTET1_SAMPLING_FREQUENCY_44100  0x01
#define A2DP_AAC_OCTET2_CHANNELS_1                0x08
#define A2DP_AAC_OCTET2_CHANNELS_2                0x04
#define A2DP_AAC_OCTET2_SAMPLING_FREQUENCY_48000  0x80
#define A2DP_AAC_OCTET3_VBR_SUPPORTED             0x80

#define BTIF_AVDTP_SIG_DISCOVER             0x01
#define BTIF_AVDTP_SIG_GET_CAPABILITIES     0x02
#define BTIF_AVDTP_SIG_SET_CONFIG           0x03
#define BTIF_AVDTP_SIG_GET_CONFIG           0x04
#define BTIF_AVDTP_SIG_RECONFIG             0x05
#define BTIF_AVDTP_SIG_OPEN                 0x06
#define BTIF_AVDTP_SIG_START                0x07
#define BTIF_AVDTP_SIG_CLOSE                0x08
#define BTIF_AVDTP_SIG_SUSPEND              0x09
#define BTIF_AVDTP_SIG_ABORT                0x0A
#define BTIF_AVDTP_SIG_SECURITY_CTRL        0x0B
#define BTIF_AVDTP_SIG_GET_ALL_CAPABILITIES 0x0C
#define BTIF_AVDTP_SIG_DELAYREPORT          0x0D

#ifndef  avdtp_codec_t
#define   avdtp_codec_t void
#endif
#ifndef avdtp_channel_t
#define   avdtp_channel_t void
#endif

#define BTIF_AVDTP_CP_TYPE_DTCP      0x0001

#define BTIF_AVDTP_CP_TYPE_SCMS_T    0x0002

#define BTIF_AVDTP_SRV_CAT_MEDIA_TRANSPORT      0x01
#define BTIF_AVDTP_SRV_CAT_REPORTING            0x02
#define BTIF_AVDTP_SRV_CAT_RECOVERY             0x03
#define BTIF_AVDTP_SRV_CAT_CONTENT_PROTECTION   0x04
#define BTIF_AVDTP_SRV_CAT_HEADER_COMPRESSION   0x05
#define BTIF_AVDTP_SRV_CAT_MULTIPLEXING         0x06
#define BTIF_AVDTP_SRV_CAT_MEDIA_CODEC          0x07
#define BTIF_AVDTP_SRV_CAT_DELAY_REPORTING      0x08

typedef U8 btif_avdtp_error_t;

#define BTIF_AVDTP_ERR_NO_ERROR                    0x00

#define BTIF_AVDTP_ERR_BAD_HEADER_FORMAT           0x01

#define BTIF_AVDTP_ERR_BAD_LENGTH                  0x11

#define BTIF_AVDTP_ERR_BAD_ACP_SEID                0x12

#define BTIF_AVDTP_ERR_IN_USE                      0x13

#define BTIF_AVDTP_ERR_NOT_IN_USE                  0x14

#define BTIF_AVDTP_ERR_BAD_SERV_CATEGORY           0x17

#define BTIF_AVDTP_ERR_BAD_PAYLOAD_FORMAT          0x18

#define BTIF_AVDTP_ERR_NOT_SUPPORTED_COMMAND       0x19

#define BTIF_AVDTP_ERR_INVALID_CAPABILITIES        0x1A

#define BTIF_AVDTP_ERR_BAD_RECOVERY_TYPE           0x22

#define BTIF_AVDTP_ERR_BAD_MEDIA_TRANSPORT_FORMAT  0x23

#define BTIF_AVDTP_ERR_BAD_RECOVERY_FORMAT         0x25

#define BTIF_AVDTP_ERR_BAD_ROHC_FORMAT             0x26

#define BTIF_AVDTP_ERR_BAD_CP_FORMAT               0x27

#define BTIF_AVDTP_ERR_BAD_MULTIPLEXING_FORMAT     0x28

#define BTIF_AVDTP_ERR_UNSUPPORTED_CONFIGURATION   0x29

#define BTIF_AVDTP_ERR_BAD_STATE                   0x31

#define BTIF_AVDTP_ERR_NOT_SUPPORTED_CODEC_TYPE    0xC2

#define BTIF_AVDTP_ERR_UNKNOWN_ERROR               0xFF

typedef struct btif_avdtp_capability_t {
    btif_avdtp_capability_type_t type;
    union {
        btif_avdtp_codec_t codec;
        btif_avdtp_content_prot_t cp;
    } p;
} btif_avdtp_capability_t;

typedef uint8_t btif_avdtp_streamId_t;
typedef uint8_t btif_avdtp_media_type;
typedef uint8_t btif_avdtp_strm_endpoint_type_t;

typedef struct {
    uint8_t signal_id : 6;
} btif_a2dp_signal_id;
typedef struct btif_avdtp_stream_info_t {
    btif_avdtp_streamId_t id;
    bool inUse;
    btif_avdtp_media_type mediaType;
    btif_avdtp_strm_endpoint_type_t streamType;
    btif_a2dp_signal_id  signal_id;
} btif_avdtp_stream_info_t;

typedef struct {
    U8 bitPool;
    uint8_t sampleFreq;
    uint8_t channelMode;
    uint8_t allocMethod;
    U8 numBlocks;
    U8 numSubBands;
    U8 numChannels;
    U8 mSbcFlag;
} btif_sbc_stream_info_short_t;

typedef enum
{
    CODEC_ACT_MODIFY_SBC_BITPOOL    = 0,
    CODEC_ACT_EN_OR_DISABLE_CODEC   = 1,
    CODEC_ACT_NUM,
} codec_action_e;

typedef struct _custom_act_param {
    uint8_t device_id;      // device to be operated (0xFF: all device will be operated)
    codec_action_e action;
    union
    {
        struct
        {
            // valid value: BT_A2DP_CODEC_TYPE_SBC, BT_A2DP_CODEC_TYPE_MPEG2_4_AAC, BT_A2DP_CODEC_TYPE_NON_A2DP
            uint8_t codec_type;
            // when codec_type is BT_A2DP_CODEC_TYPE_NON_A2DP, this filed is valid,
            // valid value: A2DP_NON_CODEC_TYPE_LHDC, A2DP_NON_CODEC_TYPE_LHDCV5
            uint8_t sub_codec_type;
            // true: discoverable, false: discoverable
            bool enable;
        }enable_codec;
        struct
        {
            uint8_t min_bitpool;    // valid value: 2-250;  0xFF: dont care;  default value:A2D_SBC_IE_MIN_BITPOOL  (2)
            uint8_t max_bitpool;    // valid value: 2-250;  0xFF: dont care;  default value:A2D_SBC_IE_MAX_BITPOOL  (250)
        } sbc_bp;
    } p;
}codec_act_param_t;

typedef void btif_avdtp_stream_t;

typedef uint8_t btif_a2dp_error_t;

#define BTIF_A2DP_ERR_NO_ERROR                         0x00

#define BTIF_A2DP_ERR_BAD_SERVICE                      0x80

#define BTIF_A2DP_ERR_INSUFFICIENT_RESOURCE            0x81

#define BTIF_A2DP_ERR_INVALID_CODEC_TYPE               0xC1

#define BTIF_A2DP_ERR_NOT_SUPPORTED_CODEC_TYPE   AVDTP_ERR_NOT_SUPPORTED_CODEC_TYPE

#define BTIF_A2DP_ERR_INVALID_SAMPLING_FREQUENCY       0xC3

#define BTIF_A2DP_ERR_NOT_SUPPORTED_SAMP_FREQ          0xC4

/** Channel mode not valid
 *
 *  SBC
 *  MPEG-1,2 Audio
 *  ATRAC family
 */
#define BTIF_A2DP_ERR_INVALID_CHANNEL_MODE             0xC5

#define BTIF_A2DP_ERR_NOT_SUPPORTED_CHANNEL_MODE       0xC6

#define BTIF_A2DP_ERR_INVALID_SUBBANDS                 0xC7

#define BTIF_A2DP_ERR_NOT_SUPPORTED_SUBBANDS           0xC8

#define BTIF_A2DP_ERR_INVALID_ALLOCATION_METHOD        0xC9

#define BTIF_A2DP_ERR_NOT_SUPPORTED_ALLOC_METHOD       0xCA

#define BTIF_A2DP_ERR_INVALID_MIN_BITPOOL_VALUE        0xCB

#define BTIF_A2DP_ERR_NOT_SUPPORTED_MIN_BITPOOL_VALUE  0xCC

#define BTIF_A2DP_ERR_INVALID_MAX_BITPOOL_VALUE        0xCD

#define BTIF_A2DP_ERR_NOT_SUPPORTED_MAX_BITPOOL_VALUE  0xCE

#define BTIF_A2DP_ERR_INVALID_LAYER                    0xCF

#define BTIF_A2DP_ERR_NOT_SUPPORTED_LAYER              0xD0

#define BTIF_A2DP_ERR_NOT_SUPPORTED_CRC                0xD1

#define BTIF_A2DP_ERR_NOT_SUPPORTED_MPF                0xD2

#define BTIF_A2DP_ERR_NOT_SUPPORTED_VBR                0xD3

#define BTIF_A2DP_ERR_INVALID_BIT_RATE                 0xD4

#define BTIF_A2DP_ERR_NOT_SUPPORTED_BIT_RATE           0xD5

#define BTIF_A2DP_ERR_INVALID_OBJECT_TYPE              0xD6

#define BTIF_A2DP_ERR_NOT_SUPPORTED_OBJECT_TYPE        0xD7

#define BTIF_A2DP_ERR_INVALID_CHANNELS                 0xD8

#define BTIF_A2DP_ERR_NOT_SUPPORTED_CHANNELS           0xD9

#define A2DP_SCALABLE_OCTET_NUMBER (7)

#define BTIF_A2DP_ERR_INVALID_VERSION                  0xDA

#define BTIF_A2DP_ERR_NOT_SUPPORTED_VERSION            0xDB

#define BTIF_A2DP_ERR_NOT_SUPPORTED_MAXIMUM_SUL        0xDC

#define BTIF_A2DP_ERR_INVALID_BLOCK_LENGTH             0xDD

#define BTIF_A2DP_ERR_INVALID_CP_TYPE                  0xE0

#define BTIF_A2DP_ERR_INVALID_CP_FORMAT                0xE1

#define BTIF_A2DP_ERR_UNKNOWN_ERROR                    AVDTP_ERR_UNKNOWN_ERROR

typedef U16 btif_a22dp_version_t;

typedef U16 btif_a2dp_features_t;

/* Audio Player */
#define BTIF_A2DP_SRC_FEATURE_PLAYER    0x01

/* Microphone */
#define BTIF_A2DP_SRC_FEATURE_MIC       0x02

/* Tuner */
#define BTIF_A2DP_SRC_FEATURE_TUNER     0x04

/* Mixer */
#define BTIF_A2DP_SRC_FEATURE_MIXER     0x08

/* Headphones */
#define BTIF_A2DP_SNK_FEATURE_HEADPHONE 0x01

/* Loudspeaker */
#define BTIF_A2DP_SNK_FEATURE_SPEAKER   0x02

/* Audio Recorder */
#define BTIF_A2DP_SNK_FEATURE_RECORDER  0x04

/* Amplifier */
#define BTIF_A2DP_SNK_FEATURE_AMP       0x08

typedef U8 btif_a2dp_endpoint_type_t;

/* The stream is a source */
#define BTIF_A2DP_STREAM_TYPE_SOURCE  0

/* The stream is a sink */
#define BTIF_A2DP_STREAM_TYPE_SINK    1

typedef void (*btif_a2dp_callback)(uint8_t device_id, a2dp_stream_t * Stream, const a2dp_callback_parms_t * Info);


typedef void btif_av_device_t;

struct btif_get_codec_cap_t
{
    uint8_t ** cap;
    uint16_t * cap_len;
    bool     done;
};

typedef struct {
    struct list_node node;          /* Used internally by A2DP. */
    btif_avdtp_stream_info_t info;  /* Stream information */
} btif_a2dp_streamInfo_t;

/**
 * standard a2dp (sink role) interface
 *
 */

typedef struct {
    uint8_t error_code;
    uint8_t codec_type;
    uint8_t codec_info_len;
    uint8_t *codec_info;
    uint8_t *cp_info;
    uint8_t cp_info_len;
    uint8_t cp_type;
} bt_a2dp_opened_param_t;

typedef struct {
    uint8_t error_code;
} bt_a2dp_closed_param_t;

typedef struct {
    uint8_t error_code;
    uint8_t codec_type;
    uint8_t codec_info_len;
    uint8_t *codec_info;
    uint8_t *cp_info;
    uint8_t cp_info_len;
    uint16_t cp_type;
} bt_a2dp_stream_start_param_t;

typedef struct {
    uint8_t error_code;
    uint8_t codec_type;
    uint8_t codec_info_len;
    uint8_t *codec_info;
    uint8_t *cp_info;
    uint8_t cp_info_len;
    uint16_t cp_type;
} bt_a2dp_stream_reconfig_param_t;

typedef struct {
    uint8_t error_code;
} bt_a2dp_stream_suspend_param_t;

typedef struct {
    uint8_t error_code;
} bt_a2dp_stream_close_param_t;

typedef struct {
    uint8_t *buf;
    uint16_t len;
} bt_a2dp_stream_data_param_t;

typedef struct {
    uint8_t message_type : 2;
    uint8_t packet_type : 2;
    uint8_t transaction : 4;
    uint8_t signal_id : 6;
    uint8_t reserve : 2;
} bt_a2dp_signal_msg_header_t;

typedef struct {
    uint8_t trans_lable;
    uint8_t cmd_id;
    uint16_t data_len;
    uint8_t *cmd_data;
} bt_a2dp_custom_cmd_req_param_t;

typedef struct {
    bool accepted;
    uint8_t cmd_id;
    uint16_t data_len;
    uint8_t *cmd_data;
} bt_a2dp_custom_cmd_rsp_param_t;

typedef union {
    bt_a2dp_opened_param_t *opened;
    bt_a2dp_closed_param_t *closed;
    bt_a2dp_stream_start_param_t *stream_start;
    bt_a2dp_stream_reconfig_param_t *stream_reconfig;
    bt_a2dp_stream_suspend_param_t *stream_suspend;
    bt_a2dp_stream_close_param_t *stream_close;
    bt_a2dp_stream_data_param_t *stream_data;
    bt_a2dp_custom_cmd_req_param_t *custom_cmd_req;
    bt_a2dp_custom_cmd_rsp_param_t *custom_cmd_rsp;
} bt_a2dp_callback_param_t;

void btif_a2dp_init(btif_a2dp_callback cb);

btif_a2dp_stream_t *btif_a2dp_get_stream(uint8_t device_id);

uint16_t btif_avdtp_parse_mediaHeader(btif_media_header_t * header,
                                      btif_a2dp_callback_parms_t * Info, uint8_t avdtp_cp);

uint16_t btif_a2dp_stream_get_media_mtu(a2dp_stream_t *stream);

void a2dp_set_config_codec(btif_avdtp_codec_t * config_codec,
                           const btif_a2dp_callback_parms_t * Info);

void btif_a2dp_stream_init(btif_a2dp_stream_t *Stream);

void btif_a2dp_disable_aac_codec(uint32_t disable);

void btif_a2dp_disable_sbc_codec(uint32_t disable);

void btif_a2dp_set_codec_parameters(codec_act_param_t *codec_act);

void btif_a2dp_disable_vendor_codec(uint32_t disable);

void btif_a2dp_updata_specific_vendor_codec_discoverable(uint32_t allow_discoverable, uint32_t nontype);

bt_status_t btif_a2dp_register(btif_a2dp_stream_t *Stream,
                               btif_a2dp_endpoint_type_t sep_type,
                               btif_avdtp_codec_t *sep_codec,
                               btif_avdtp_content_prot_t *sep_cp,
                               uint8_t sep_priority,
                               btif_a2dp_callback Callback);

bt_status_t btif_a2dp_deregister(btif_a2dp_stream_t *Stream, uint8_t seid_type, btif_avdtp_codec_t *sep_codec);

void btif_a2dp_set_copy_protection_enable(a2dp_stream_t *stream, bool enable);

btif_remote_device_t *btif_a2dp_get_remote_device(a2dp_stream_t * stream);

uint8_t *btif_a2dp_get_stream_devic_cmgrHandler_remdev_bdAddr(a2dp_stream_t * Stream);

void *btif_a2dp_get_stream_device(a2dp_stream_t * Stream);

void *btif_a2dp_get_stream_devic_cmgrHandler_bt_handler(a2dp_stream_t * Stream);

void *btif_a2dp_get_stream_devic_cmgrHandler_remdev(a2dp_stream_t * Stream);

uint8_t btif_a2dp_get_stream_devic_cmgrHandler_remdev_role(a2dp_stream_t * Stream);

void *btif_a2dp_get_stream_devic_cmgrHandler(a2dp_stream_t * Stream);

bt_bdaddr_t *btif_a2dp_stream_conn_remDev_bdAddr(a2dp_stream_t * Stream);

uint8_t *btif_a2dp_get_remote_device_version(btif_remote_device_t * remDev);

btif_a2dp_event_t btif_a2dp_get_cb_event(a2dp_callback_parms_t * info);

bt_status_t btif_a2dp_set_sink_delay(int device_id, U16 delayMs);

bt_status_t btif_a2dp_set_stream_config(a2dp_stream_t * Stream,
                                        btif_avdtp_codec_t * Codec,
                                        btif_avdtp_content_prot_t * Cp);

bt_status_t btif_a2dp_open_stream(btif_avdtp_codec_t *prev_conn_codec, bt_bdaddr_t * Addr, enum A2DP_ROLE role_expected);

bt_status_t btif_a2dp_start_stream(a2dp_stream_t * Stream);

bt_status_t btif_a2dp_idle_stream(a2dp_stream_t * Stream);

bt_status_t btif_a2dp_suspend_stream(a2dp_stream_t * Stream);

bt_status_t btif_a2dp_start_stream_rsp(a2dp_stream_t * Stream, btif_a2dp_error_t error);

bt_status_t btif_a2dp_close_stream(a2dp_stream_t * Stream);

bt_status_t btif_a2dp_reconfig_stream_rsp(a2dp_stream_t * Stream,
                                          btif_a2dp_error_t Error,
                                          btif_avdtp_capability_type_t Type);

bt_status_t btif_a2dp_reconfig_stream(a2dp_stream_t * Stream,
                                      btif_avdtp_codec_t * codec_cfg,
                                      btif_avdtp_content_prot_t * cp);

void btif_a2dp_reconfig_codec_to_vendor_codec(a2dp_stream_t *Stream, uint8_t codec_id, uint8_t a2dp_non_type);

void btif_a2dp_reconfig_codec_to_aac(a2dp_stream_t *Stream);

void btif_a2dp_reconfig_codec_to_sbc(a2dp_stream_t *Stream);

void btif_a2dp_reconfig_codec(a2dp_stream_t *Stream, uint8_t code_type);

uint8_t btif_a2dp_security_control_req(a2dp_stream_t *stream, uint8_t *data, uint16_t len);
uint8_t btif_a2dp_security_control_rsp(a2dp_stream_t *stream,uint8_t* data,uint16_t len, uint8_t error);

bt_status_t btif_a2dp_open_stream_rsp(a2dp_stream_t * Stream,
                                      btif_a2dp_error_t Error,
                                      btif_avdtp_capability_type_t CapType);

bool btif_a2dp_stream_has_remote_device(btif_a2dp_stream_t * stream);

bt_bdaddr_t *btif_a2dp_stream_get_remote_bd_addr(btif_a2dp_stream_t * stream);

btif_a2dp_stream_t *btif_get_a2dp_stream(a2dp_stream_t * stream);

bt_a2dp_stream_state_t btif_a2dp_get_stream_state(a2dp_stream_t * Stream);

uint16_t btif_a2dp_get_stream_chnl_sigchnl_l2ChannelId(a2dp_stream_t * Stream);

void btif_a2dp_set_stream_state(a2dp_stream_t * Stream, bt_a2dp_stream_state_t state);

bool btif_a2dp_is_stream_device_has_delay_reporting(a2dp_stream_t * Stream);

btif_avdtp_codec_t *btif_a2dp_get_stream_codec(a2dp_stream_t * Stream);
btif_avdtp_codec_t *btif_a2dp_get_stream_codec_from_id(uint8_t device_id);

uint16_t btif_a2dp_get_stream_conn_remDev_hciHandle(a2dp_stream_t * Stream);

uint16_t btif_a2dp_get_stream_device_cmgrhandler_remDev_hciHandle(a2dp_stream_t * Stream);

uint8_t *btif_a2dp_get_stream_cp_info(a2dp_stream_t *Stream);

bt_status_t btif_a2dp_get_stream_capabilities(a2dp_stream_t * Stream);

bt_status_t btif_a2dp_stream_send_sbc_packet(a2dp_stream_t * stream,
                                             btif_a2dp_sbc_packet_t * Packet,
                                             btif_sbc_stream_info_short_t * StreamInfo);

bt_status_t btif_a2dp_stream_send_ldac_packet(a2dp_stream_t * stream,
                                             btif_a2dp_sbc_packet_t * Packet,
                                             btif_sbc_stream_info_short_t * StreamInfo);


bt_status_t btif_a2dp_stream_send_aac_packet(a2dp_stream_t *stream,
                                                        btif_a2dp_sbc_packet_t *Packet,
                                                        btif_sbc_stream_info_short_t *StreamInfo);
bt_status_t btif_a2dp_stream_send_lhdc_packet(a2dp_stream_t *stream,
        btif_a2dp_sbc_packet_t *Packet,
        btif_sbc_stream_info_short_t *StreamInfo);

bt_status_t btif_a2dp_stream_send_lhdcv5_packet(a2dp_stream_t *stream,
        btif_a2dp_sbc_packet_t *Packet,
        btif_sbc_stream_info_short_t *StreamInfo);

bt_status_t btif_a2dp_stream_send_mihc_packet(a2dp_stream_t *stream,
                                              btif_a2dp_sbc_packet_t *Packet,
                                              btif_sbc_stream_info_short_t *StreamInfo);

bt_status_t btif_a2dp_stream_send_general_packet(a2dp_stream_t *stream, uint8 *data, uint32 data_len, void* context);

void btif_a2dp_sync_avdtp_streaming_state(bt_bdaddr_t *addr);

void btif_app_a2dp_source_init(void);

uint8_t btif_a2dp_get_cb_error(const btif_a2dp_callback_parms_t * Info);

uint8_t btif_a2dp_set_dst_stream(a2dp_callback_parms_t *Info, a2dp_stream_t *stream);

btif_media_header_t *btif_a2dp_get_stream_media_header(a2dp_stream_t * stream);

int tws_if_get_a2dpbuff_available(void);

bool btif_a2dp_is_disconnected(a2dp_stream_t *Stream);

uint8_t btif_a2dp_confirm_stream_state(a2dp_stream_t *Stream, uint8_t old_state, uint8_t new_state);

void btif_a2dp_accept_stream_request_command(bt_bdaddr_t* remote, uint8_t transaction, uint8_t signal_id);

btif_remote_device_t *btif_a2dp_get_remote_device_from_cbparms(a2dp_stream_t *Stream, const a2dp_callback_parms_t *info);

btif_avdtp_codec_type_t btif_a2dp_get_codec_type(const a2dp_callback_parms_t *info);

void btif_a2dp_register_multi_link_connect_not_allowed_callback(bool (*cb)(uint8_t device_id));

void btif_a2dp_change_codec_freq_capability(uint8_t codec, uint8_t a2dp_non_type, uint8_t support_mask);
void btif_a2dp_set_codec_info_func(void (*func)(uint8_t dev_num, const uint8_t *codec));
void btif_a2dp_get_codec_info_func(void (*func)(uint8_t dev_num, uint8_t *codec));
void btif_a2dp_get_codec_non_type_func(uint8_t (*func)(uint8_t *elements));
bool btif_a2dp_is_profile_initiator(const bt_bdaddr_t* remote);
void btif_a2dp_set_codec_info(uint8_t dev_num, const uint8_t *codec);
void btif_a2dp_get_codec_info(uint8_t dev_num, uint8_t *codec);
uint8_t btif_a2dp_get_codec_non_type(uint8_t *elements);
bt_status_t btif_a2dp_send_signal_message(const bt_bdaddr_t *remote, bt_a2dp_signal_msg_header_t *header, const uint8_t *data, uint16_t len);
bool btif_a2dp_is_remote_support_codec(btif_a2dp_stream_t *Stream, uint8_t codec_type, uint32_t vendor_id, uint16_t codec_id);
void btif_a2dp_update_media_chan_state_to_crtl(const bt_bdaddr_t *remote, bool connected);

/* Callout functions, do not call directly */
uint8_t a2dp_stream_confirm_stream_state(uint8_t index, uint8_t old_state, uint8_t new_state);
uint8_t a2dp_stream_locate_the_connected_dev_id(a2dp_stream_t *Stream);
void btif_a2dp_register_multi_link_close_req_allowed_callback(uint8 (*cb)(uint8_t device_id));

typedef bool (*a2dp_req_cmd_delay_rsp_t)(const bt_bdaddr_t *remote, uint8_t *timeout);
void btif_a2dp_register_req_delay_rsp_cb(a2dp_req_cmd_delay_rsp_t callback);

/**
 ****************************************************************************************
 *       _    ____  ____  ____    ___ ____  ____ _____   _____ _   _ _   _  ____
 *      / \  |___ \|  _ \|  _ \  |_ _| __ )|  _ \_   _| |  ___| | | | \ | |/ ___|
 *     / _ \   __) | | | | |_) |  | ||  _ \| |_) || |   | |_  | | | |  \| | |
 *    / ___ \ / __/| |_| |  __/   | || |_) |  _ < | |   |  _| | |_| | |\  | |___
 *   /_/   \_\_____|____/|_|     |___|____/|_| \_\|_|   |_|    \___/|_| \_|\____|
 *
 ****************************************************************************************
 */

uint32_t btif_a2dp_profile_save_ctx(const bt_bdaddr_t *remote, uint8_t *buf, uint32_t buf_len);
uint32_t btif_a2dp_profile_restore_ctx(const bt_bdaddr_t *bdaddr_p, uint8_t *buf, uint32_t buf_len, void (*close_old_ch_cb)(uint8_t dev_id));
uint8_t btif_a2dp_is_critical_avdtp_cmd_handling(void);
void btif_a2dp_critical_avdtp_cmd_timeout(void);
void btif_a2dp_force_disconnect_a2dp_profile(uint8_t device_id,uint8_t reason);
bool btif_a2dp_signal_media_channel_connected(bt_bdaddr_t * addr);

#ifdef __cplusplus
}
#endif
#endif
