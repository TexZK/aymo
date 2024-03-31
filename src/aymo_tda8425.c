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

#include "aymo_cpu.h"
#include "aymo_tda8425.h"
#include "aymo_tda8425_arm_neon.h"
#include "aymo_tda8425_dummy.h"
#include "aymo_tda8425_none.h"
#include "aymo_tda8425_x86_avx2.h"
#include "aymo_tda8425_x86_sse41.h"

#include <assert.h>

AYMO_CXX_EXTERN_C_BEGIN


const struct aymo_tda8425_math* aymo_tda8425_math;

static const struct aymo_tda8425_vt* aymo_tda8425_best_vt;


void aymo_tda8425_boot(const struct aymo_tda8425_math* math)
{
    assert(math);

    aymo_tda8425_math = math;

    #ifdef AYMO_CPU_SUPPORT_X86_AVX2
        if (aymo_cpu_x86_get_extensions() & AYMO_CPU_X86_EXT_AVX2) {
            aymo_tda8425_best_vt = aymo_tda8425_x86_avx2_get_vt();
            return;
        }
    #endif

    #ifdef AYMO_CPU_SUPPORT_X86_SSE41
        if (aymo_cpu_x86_get_extensions() & AYMO_CPU_X86_EXT_SSE41) {
            aymo_tda8425_best_vt = aymo_tda8425_x86_sse41_get_vt();
            return;
        }
    #endif

    #ifdef AYMO_CPU_SUPPORT_ARM_NEON
        if (aymo_cpu_arm_get_extensions() & AYMO_CPU_ARM_EXT_NEON) {
            aymo_tda8425_best_vt = aymo_tda8425_arm_neon_get_vt();
            return;
        }
    #endif

    aymo_tda8425_best_vt = aymo_tda8425_none_get_vt();
}


const struct aymo_tda8425_vt* aymo_tda8425_get_vt(const char* cpu_ext)
{
    if (cpu_ext == NULL) {
        return NULL;
    }

    #ifdef AYMO_CPU_SUPPORT_X86_AVX2
        if (!aymo_strcmp(cpu_ext, "x86_avx2")) {
            if (aymo_cpu_x86_get_extensions() & AYMO_CPU_X86_EXT_AVX2) {
                return aymo_tda8425_x86_avx2_get_vt();
            }
        }
    #endif

    #ifdef AYMO_CPU_SUPPORT_X86_SSE41
        if (!aymo_strcmp(cpu_ext, "x86_sse41")) {
            if (aymo_cpu_x86_get_extensions() & AYMO_CPU_X86_EXT_SSE41) {
                return aymo_tda8425_x86_sse41_get_vt();
            }
        }
    #endif

    #ifdef AYMO_CPU_SUPPORT_ARM_NEON
        if (!aymo_strcmp(cpu_ext, "arm_neon")) {
            if (aymo_cpu_arm_get_extensions() & AYMO_CPU_ARM_EXT_NEON) {
                return aymo_tda8425_arm_neon_get_vt();
            }
        }
    #endif

    if (!aymo_strcmp(cpu_ext, "none")) {
        return aymo_tda8425_none_get_vt();
    }
    if (!aymo_strcmp(cpu_ext, "dummy")) {
        return aymo_tda8425_dummy_get_vt();
    }
    return NULL;
}


const struct aymo_tda8425_vt* aymo_tda8425_get_best_vt(void)
{
    return aymo_tda8425_best_vt;
}


uint32_t aymo_tda8425_get_sizeof(struct aymo_tda8425_chip* chip)
{
    assert(chip);
    assert(chip->vt);
    assert(chip->vt->get_sizeof);

    return chip->vt->get_sizeof();
}


void aymo_tda8425_ctor(struct aymo_tda8425_chip* chip, float sample_rate)
{
    assert(chip);
    assert(chip->vt);
    assert(chip->vt->ctor);

    chip->vt->ctor(chip, sample_rate);
}


void aymo_tda8425_dtor(struct aymo_tda8425_chip* chip)
{
    assert(chip);
    assert(chip->vt);
    assert(chip->vt->dtor);

    chip->vt->dtor(chip);
}


uint8_t aymo_tda8425_read(struct aymo_tda8425_chip* chip, uint16_t address)
{
    assert(chip);
    assert(chip->vt);
    assert(chip->vt->read);

    return chip->vt->read(chip, address);
}


void aymo_tda8425_write(struct aymo_tda8425_chip* chip, uint16_t address, uint8_t value)
{
    assert(chip);
    assert(chip->vt);
    assert(chip->vt->write);

    chip->vt->write(chip, address, value);
}


void aymo_tda8425_process_f32(struct aymo_tda8425_chip* chip, uint32_t count, const float x[], float y[])
{
    assert(chip);
    assert(chip->vt);
    assert(chip->vt->process_f32);

    chip->vt->process_f32(chip, count, x, y);
}


AYMO_CXX_EXTERN_C_END
