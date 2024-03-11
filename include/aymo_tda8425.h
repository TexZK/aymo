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
#ifndef _include_aymo_tda8425_h
#define _include_aymo_tda8425_h

#include "aymo_tda8425_common.h"

AYMO_CXX_EXTERN_C_BEGIN


AYMO_PUBLIC const struct aymo_tda8425_math* aymo_tda8425_math;


AYMO_PUBLIC void aymo_tda8425_boot(const struct aymo_tda8425_math* math);
AYMO_PUBLIC const struct aymo_tda8425_vt* aymo_tda8425_get_vt(const char* cpu_ext);
AYMO_PUBLIC const struct aymo_tda8425_vt* aymo_tda8425_get_best_vt(void);

AYMO_PUBLIC uint32_t aymo_tda8425_get_sizeof(struct aymo_tda8425_chip* chip);
AYMO_PUBLIC void aymo_tda8425_ctor(struct aymo_tda8425_chip* chip, float sample_rate);
AYMO_PUBLIC void aymo_tda8425_dtor(struct aymo_tda8425_chip* chip);
AYMO_PUBLIC uint8_t aymo_tda8425_read(struct aymo_tda8425_chip* chip, uint16_t address);
AYMO_PUBLIC void aymo_tda8425_write(struct aymo_tda8425_chip* chip, uint16_t address, uint8_t value);
AYMO_PUBLIC void aymo_tda8425_process_f32(struct aymo_tda8425_chip* chip, uint32_t count, const float x[], float y[]);


AYMO_CXX_EXTERN_C_END

#endif  // _include_aymo_tda8425_h
