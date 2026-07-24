#ifndef SPEECH_COMMON_H
#define SPEECH_COMMON_H

#define MAX_COMP_NUM (10)
#define MAX_WIND_NUM (5)

enum WIND_STATE_E
{
    NO_WIND = 0,
    SMALL_WIND,
    LARGE_WIND,
    SATURATION_WIND,
};

typedef void(*FILTER_RESET_HANDLER)(void *st);

#endif
