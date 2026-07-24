#ifndef _PRESS_DETECTION_HPP
#define _PRESS_DETECTION_HPP

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#define PRESS_USED_TOUCH_NUM   1

#define flag_T                 0//1,2,3
#define KEY_TIME_IDLE          400 / (flag_T + 1)
#define KEY_TIME_CONTINUS      1000 / (flag_T + 1)
#define KEY_TIME_OUT           200000 / (flag_T + 1)
#define KEY_TIME_DISTURBANCE   15

#define EVENT_NONE_CLICK       0x00
#define EVENT_CLICK_LIGHT      0x01
#define EVENT_DOUBLE_CLICK     0x02
#define EVENT_TRIPLE_CLICK     0x03
#define EVENT_LONG_CLICK       0x04
#define EVENT_CLICK_HEAVY      0x05

#define KEY_STATE_IDLE         0x00
#define KEY_STATE_PRESS        0x01
#define KEY_STATE_RELEASE      0x02
#define KEY_STATE_LONG_PRESS   0x03


typedef struct
{
    struct
    {
        uint8_t check      :1; // check flag
        uint8_t key_state  :2; // 0 - idle, 1 - press, 2 - release
        uint8_t once_event :1; // event recognize flag (when once_event == 1)
        uint8_t press_time :1; // 0 - short press, 1 - long press
        bool last_state    :1;
        bool current_state :1;
    }flag;

    uint8_t event_current_type  :5; // 0 - idle, 1 - one press, 2 - double press, 3 - triple press, 4 - long press
    uint8_t event_previous_type :5;

    uint8_t press_cnt; // press number
    uint8_t peak_cnt;

    uint32_t time_idle;
    uint32_t time_continus;
    uint32_t time_disturbance;

    float power_zero;
    float power_threshold;

}KEY_PROCESS_TypeDef;

void key_recognition_entrance(int32_t *key_data, int16_t channel);

#ifdef __cplusplus
}
#endif

#endif
