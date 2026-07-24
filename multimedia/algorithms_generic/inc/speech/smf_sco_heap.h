#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "hal_trace.h"
#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif

EXTERNC void* scoup_malloc(uint32_t size);
EXTERNC void* scoup_calloc(uint32_t num, uint32_t size);
EXTERNC void scoup_free(void* ptr);
EXTERNC void* scodown_malloc(uint32_t size);
EXTERNC void* scodown_calloc(uint32_t num, uint32_t size);
EXTERNC void scodown_free(void* ptr);