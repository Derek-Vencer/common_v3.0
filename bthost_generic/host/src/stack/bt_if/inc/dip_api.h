#ifndef __DIP_API__H__
#define __DIP_API__H__

#include "bluetooth.h"
#include "sdp_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SRC_BT (1)
#define SRC_USB (2)
#define SRC_BT_SANSUMG (0x0075)
#define SRC_BT_APPLE   (0X004c)
#define SRC_USB_APPLE   (0x05AC)

enum dip_ctrl_state {
    DIP_CTRL_ST_IDLE = 0,
    DIP_CTRL_ST_SDP_QUERYING,
    DIP_CTRL_ST_SDP_QUERIED,
};

typedef enum {
    MOBILE_CONNECT_IDLE,
    MOBILE_CONNECT_IOS,
    MOBILE_CONNECT_ANDROID,
} MOBILE_CONN_TYPE_E;

typedef struct
{
    uint16_t spec_id;
    uint16_t vend_id;
    uint16_t prod_id;
    uint16_t prod_ver;
    uint8_t  prim_rec;
    uint16_t vend_id_source;
} dip_pnp_info_t;

typedef struct {
    uint8_t error_code;
} bt_dip_queried_state_t;

typedef union {
    void *param_ptr;
    bt_dip_queried_state_t *res;
} bt_dip_callback_param_t;

enum dip_event {
    DIP_EVENT_SDP_REQ_FAIL,
    DIP_EVENT_SDP_QUERYING,
    DIP_EVENT_SDP_REQ_SUCCESS,
};

enum dip_sdp_req_fail_reason {
    DIP_FAIL_REASON_NO_ERROR = 0,
    DIP_FAIL_REASON_SDP_ERROR,
    DIP_FAIL_REASON_SDP_CHANNEL_CLOSE,
    DIP_FAIL_REASON_DIP_INFO_INVALID,
};

struct dip_sdp_req_fail {
    enum dip_sdp_req_fail_reason reason;
};

struct dip_device_info
{
    uint16 spec_id;
    uint16 vend_id;
    uint16 prod_id;
    uint16 prod_ver;
    uint8  prim_rec;
    uint16 vend_id_source;
};

struct dip_callback_param {
    enum dip_event event;
    union {
        struct dip_sdp_req_fail fail;
        struct dip_device_info info;
    }p;
};

typedef void (*DipApiCallBack)(bt_bdaddr_t *_addr, struct dip_callback_param cb_para);

void btif_dip_init(DipApiCallBack callback);
void btif_dip_clear_ctl(const bt_bdaddr_t *remote);
void btif_dip_set_state(const bt_bdaddr_t *remote, enum dip_ctrl_state state);
bool btif_dip_check_is_ios_device(const bt_bdaddr_t *remote);
dip_pnp_info_t* btif_dip_get_device_info(const bt_bdaddr_t *remote);
void btif_dip_query_remote_info(bt_bdaddr_t *remote);
bool btif_dip_check_is_ios_by_vend_id(uint16_t vend_id_source, uint16_t vend_id);

#ifdef __cplusplus
}
#endif

#endif

