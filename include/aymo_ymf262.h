/*
AYMO - Accelerated YaMaha Operator
Copyright (c) 2023-2024 Andrea Zoppi.

This file is part of AYMO.

AYMO is free software: you can redistribute it and/or modify it under the
terms of the GNU Lesser General Public License as published by the Free
Software Foundation, either version 2.1 of the License, or (at your option)
any later version.

AYMO is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with AYMO. If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef _include_aymo_ymf262_h
#define _include_aymo_ymf262_h

#include "aymo_ymf262_common.h"

AYMO_CXX_EXTERN_C_BEGIN


#define AYMO_YMF262_SLOT_NUM        36
#define AYMO_YMF262_CHANNEL_NUM     18
#define AYMO_YMF262_CONN_NUM_MAX    6

#define AYMO_YMF262_SAMPLE_RATE     49716  // [Hz]


AYMO_PUBLIC void aymo_ymf262_boot(void);
AYMO_PUBLIC const struct aymo_ymf262_vt* aymo_ymf262_get_vt(const char* cpu_ext);
AYMO_PUBLIC const struct aymo_ymf262_vt* aymo_ymf262_get_best_vt(void);

AYMO_PUBLIC uint32_t aymo_ymf262_get_sizeof(struct aymo_ymf262_chip* chip);
AYMO_PUBLIC void aymo_ymf262_ctor(struct aymo_ymf262_chip* chip);
AYMO_PUBLIC void aymo_ymf262_dtor(struct aymo_ymf262_chip* chip);
AYMO_PUBLIC uint8_t aymo_ymf262_read(struct aymo_ymf262_chip* chip, uint16_t address);
AYMO_PUBLIC void aymo_ymf262_write(struct aymo_ymf262_chip* chip, uint16_t address, uint8_t value);
AYMO_PUBLIC int aymo_ymf262_enqueue_write(struct aymo_ymf262_chip* chip, uint16_t address, uint8_t value);
AYMO_PUBLIC int aymo_ymf262_enqueue_delay(struct aymo_ymf262_chip* chip, uint32_t count);
AYMO_PUBLIC int16_t aymo_ymf262_get_output(struct aymo_ymf262_chip* chip, uint8_t channel);
AYMO_PUBLIC void aymo_ymf262_tick(struct aymo_ymf262_chip* chip, uint32_t count);
AYMO_PUBLIC void aymo_ymf262_generate_i16x2(struct aymo_ymf262_chip* chip, uint32_t count, int16_t y[]);
AYMO_PUBLIC void aymo_ymf262_generate_i16x4(struct aymo_ymf262_chip* chip, uint32_t count, int16_t y[]);
AYMO_PUBLIC void aymo_ymf262_generate_f32x2(struct aymo_ymf262_chip* chip, uint32_t count, float y[]);
AYMO_PUBLIC void aymo_ymf262_generate_f32x4(struct aymo_ymf262_chip* chip, uint32_t count, float y[]);


AYMO_CXX_EXTERN_C_END

#endif  // _include_aymo_ymf262_h
