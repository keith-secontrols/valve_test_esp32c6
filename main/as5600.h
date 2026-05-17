#pragma once
#include <stdint.h>

void     as5600_init(void);
uint16_t as5600_read_raw(void);      /* 0–4095 (12-bit, one full revolution) */
float    as5600_read_degrees(void);  /* 0.0–360.0 */
