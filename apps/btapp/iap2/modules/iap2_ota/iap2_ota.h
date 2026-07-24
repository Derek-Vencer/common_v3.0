#ifndef __IAP2_OTA__
#define __IAP2_OTA__

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <stdio.h>
#include <stdint.h>

// void iap2_ota_init(void);
void iap2_ota_set_ea_session_protocol_id(uint8_t protocol_id);
void iap2_ota_connect_handle(uint8_t *addr);
void iap2_ota_disconnect_handle(void);
void iap2_ota_recv_data_handle(uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif
#endif // __IAP2_OTA__