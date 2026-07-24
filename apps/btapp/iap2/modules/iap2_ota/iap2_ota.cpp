#include <string.h>
#include <stdio.h>

#include "iap2_ota.h"
#include "bta_iap2_api.h"
#include "ota_control.h"
#include "ota_basic.h"
#include "hal_trace.h"

static bt_iap2_link_info_t iap2_ota_info;
static uint8_t iap2_ota_protocol_id;

static void iap2_ota_send(uint8_t *data, uint16_t len);

void iap2_ota_set_ea_session_protocol_id(uint8_t protocol_id)
{
    iap2_ota_protocol_id = protocol_id;
}

void iap2_ota_connect_handle(uint8_t *addr)
{
    BTAPP_TRACE(0, "[IAP2]%s", __func__);

    memcpy(&(iap2_ota_info.addr.addr), addr, sizeof(bt_bdaddr_t));
    iap2_ota_info.type = BT_IAP2_LINK_TYPE_BT;

    ota_control_register_transmitter(iap2_ota_send);
    bes_ota_event_param_t otaParam = {
        .pathType = DATA_PATH_SPP,
        .event = BES_OTA_CONN,
    };
    memcpy((uint8_t*)&otaParam.param.address, (uint8_t*)addr, sizeof(otaParam.param.address));
    app_ota_push_rx_data(SPP_RX_DATA_SELF_OTA, &otaParam);
}

void iap2_ota_disconnect_handle(void)
{
    BTAPP_TRACE(0, "[IAP2]%s", __func__);

    memset(&(iap2_ota_info.addr.addr), 0, sizeof(bt_bdaddr_t));
    iap2_ota_info.type = BT_IAP2_LINK_TYPE_NONE;

    bes_ota_event_param_t otaParam = {
        .pathType = DATA_PATH_SPP,
        .event = BES_OTA_DISCONN,
    };
    app_ota_push_rx_data(SPP_RX_DATA_SELF_OTA, &otaParam);
}

static void on_packet_received(uint8_t *data, uint16_t len)
{
    bes_ota_event_param_t otaParam = {
        .pathType = DATA_PATH_SPP,
        .event = BES_OTA_RECEVICE_DATA,
    };
    otaParam.param.receive_data.data = (uint8_t *)data,
    otaParam.param.receive_data.dataLen = (uint16_t)len,
    app_ota_push_rx_data(SPP_RX_DATA_SELF_OTA, &otaParam);
}

void iap2_ota_recv_data_handle(uint8_t *data, uint16_t len)
{
    if (!data || len < 4) {
        return;
    }
    if(data[0] == 0x9F && data[1] == 0x01)
    {
        uint16_t data_len = data[2]|data[3]<<8;
        if(data_len + 4 == len)
        {
            on_packet_received(&data[4], data_len);
        }
    }
}

void iap2_ota_send(uint8_t *data, uint16_t len)
{
    if (len == 0 || data == NULL)
    {
        return;
    }
    uint8_t packet[4 + len];
    packet[0] = 0x9F;
    packet[1] = 0x02;
    packet[2] = (uint8_t)(len >> 8);
    packet[3] = (uint8_t)(len & 0xFF);

    memcpy(&packet[4], data, len);
    bta_iap2_service_send_ea_data(&(iap2_ota_info.addr.addr), iap2_ota_protocol_id, 0, 4 + len, packet);
}