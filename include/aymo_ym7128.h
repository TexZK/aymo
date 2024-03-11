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
#ifndef _include_aymo_ym7128_h
#define _include_aymo_ym7128_h

#include "aymo_ym7128_common.h"

AYMO_CXX_EXTERN_C_BEGIN


#define AYMO_YM7128_SAMPLE_RATE_IN      23550  // [Hz]
#define AYMO_YM7128_SAMPLE_RATE_OUT     47100  // [Hz]


AYMO_PUBLIC void aymo_ym7128_boot(void);
AYMO_PUBLIC const struct aymo_ym7128_vt* aymo_ym7128_get_vt(const char* cpu_ext);
AYMO_PUBLIC const struct aymo_ym7128_vt* aymo_ym7128_get_best_vt(void);

AYMO_PUBLIC uint32_t aymo_ym7128_get_sizeof(struct aymo_ym7128_chip* chip);
AYMO_PUBLIC void aymo_ym7128_ctor(struct aymo_ym7128_chip* chip);
AYMO_PUBLIC void aymo_ym7128_dtor(struct aymo_ym7128_chip* chip);
AYMO_PUBLIC uint8_t aymo_ym7128_read(struct aymo_ym7128_chip* chip, uint16_t address);
AYMO_PUBLIC void aymo_ym7128_write(struct aymo_ym7128_chip* chip, uint16_t address, uint8_t value);
AYMO_PUBLIC void aymo_ym7128_process_i16(struct aymo_ym7128_chip* chip, uint32_t count, const int16_t x[], int16_t y[]);


AYMO_CXX_EXTERN_C_END

#endif  // _include_aymo_ym7128_h
