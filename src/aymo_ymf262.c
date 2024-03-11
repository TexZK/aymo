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

#include <assert.h>
#include "aymo_cpu.h"
#include "aymo_ymf262.h"
#include "aymo_ymf262_arm_neon.h"
#include "aymo_ymf262_none.h"
#include "aymo_ymf262_x86_sse41.h"
#include "aymo_ymf262_x86_avx.h"
#include "aymo_ymf262_x86_avx2.h"

AYMO_CXX_EXTERN_C_BEGIN


static const struct aymo_ymf262_vt* aymo_ymf262_best_vt;


void aymo_ymf262_boot(void)
{
    #ifdef AYMO_CPU_SUPPORT_X86_AVX2
        if (aymo_cpu_x86_get_extensions() & AYMO_CPU_X86_EXT_AVX2) {
            aymo_ymf262_best_vt = aymo_ymf262_x86_avx2_get_vt();
            return;
        }
    #endif

    #ifdef AYMO_CPU_SUPPORT_X86_AVX
        if (aymo_cpu_x86_get_extensions() & AYMO_CPU_X86_EXT_AVX) {
            aymo_ymf262_best_vt = aymo_ymf262_x86_avx_get_vt();
            return;
        }
    #endif

    #ifdef AYMO_CPU_SUPPORT_X86_SSE41
        if (aymo_cpu_x86_get_extensions() & AYMO_CPU_X86_EXT_SSE41) {
            aymo_ymf262_best_vt = aymo_ymf262_x86_sse41_get_vt();
            return;
        }
    #endif

    #ifdef AYMO_CPU_SUPPORT_ARM_NEON
        if (aymo_cpu_arm_get_extensions() & AYMO_CPU_ARM_EXT_NEON) {
            aymo_ymf262_best_vt = aymo_ymf262_arm_neon_get_vt();
        }
    #endif

    aymo_ymf262_best_vt = aymo_ymf262_none_get_vt();
}


const struct aymo_ymf262_vt* aymo_ymf262_get_vt(const char* cpu_ext)
{
    if (cpu_ext == NULL) {
        return NULL;
    }

    #ifdef AYMO_CPU_SUPPORT_X86_AVX2
        if (!aymo_strcmp(cpu_ext, "x86_avx2")) {
            if (aymo_cpu_x86_get_extensions() & AYMO_CPU_X86_EXT_AVX2) {
                return aymo_ymf262_x86_avx2_get_vt();
            }
        }
    #endif

    #ifdef AYMO_CPU_SUPPORT_X86_AVX
        if (!aymo_strcmp(cpu_ext, "x86_avx")) {
            if (aymo_cpu_x86_get_extensions() & AYMO_CPU_X86_EXT_AVX) {
                return aymo_ymf262_x86_avx_get_vt();
            }
        }
    #endif

    #ifdef AYMO_CPU_SUPPORT_X86_SSE41
        if (!aymo_strcmp(cpu_ext, "x86_sse41")) {
            if (aymo_cpu_x86_get_extensions() & AYMO_CPU_X86_EXT_SSE41) {
                return aymo_ymf262_x86_sse41_get_vt();
            }
        }
    #endif

    #ifdef AYMO_CPU_SUPPORT_ARM_NEON
        if (!aymo_strcmp(cpu_ext, "arm_neon")) {
            if (aymo_cpu_arm_get_extensions() & AYMO_CPU_ARM_EXT_NEON) {
                return aymo_ymf262_arm_neon_get_vt();
            }
        }
    #endif

    if (!aymo_strcmp(cpu_ext, "none")) {
        return aymo_ymf262_none_get_vt();
    }
    return NULL;
}


const struct aymo_ymf262_vt* aymo_ymf262_get_best_vt(void)
{
    return aymo_ymf262_best_vt;
}


uint32_t aymo_ymf262_get_sizeof(struct aymo_ymf262_chip* chip)
{
    assert(chip);
    assert(chip->vt);
    assert(chip->vt->get_sizeof);

    return chip->vt->get_sizeof();
}


void aymo_ymf262_ctor(struct aymo_ymf262_chip* chip)
{
    assert(chip);
    assert(chip->vt);
    assert(chip->vt->ctor);

    chip->vt->ctor(chip);
}


void aymo_ymf262_dtor(struct aymo_ymf262_chip* chip)
{
    assert(chip);
    assert(chip->vt);
    assert(chip->vt->dtor);

    chip->vt->dtor(chip);
}


uint8_t aymo_ymf262_read(struct aymo_ymf262_chip* chip, uint16_t address)
{
    assert(chip);
    assert(chip->vt);
    assert(chip->vt->read);

    return chip->vt->read(chip, address);
}


void aymo_ymf262_write(struct aymo_ymf262_chip* chip, uint16_t address, uint8_t value)
{
    assert(chip);
    assert(chip->vt);
    assert(chip->vt->write);

    chip->vt->write(chip, address, value);
}


int aymo_ymf262_enqueue_write(struct aymo_ymf262_chip* chip, uint16_t address, uint8_t value)
{
    assert(chip);
    assert(chip->vt);
    assert(chip->vt->enqueue_write);

    return chip->vt->enqueue_write(chip, address, value);
}


int aymo_ymf262_enqueue_delay(struct aymo_ymf262_chip* chip, uint32_t count)
{
    assert(chip);
    assert(chip->vt);
    assert(chip->vt->enqueue_delay);

    return chip->vt->enqueue_delay(chip, count);
}


int16_t aymo_ymf262_get_output(struct aymo_ymf262_chip* chip, uint8_t channel)
{
    assert(chip);
    assert(chip->vt);
    assert(chip->vt->get_output);

    return chip->vt->get_output(chip, channel);
}


void aymo_ymf262_tick(struct aymo_ymf262_chip* chip, uint32_t count)
{
    assert(chip);
    assert(chip->vt);
    assert(chip->vt->tick);

    chip->vt->tick(chip, count);
}


void aymo_ymf262_generate_i16x2(struct aymo_ymf262_chip* chip, uint32_t count, int16_t y[])
{
    assert(chip);
    assert(chip->vt);
    assert(chip->vt->generate_i16x2);

    chip->vt->generate_i16x2(chip, count, y);
}


void aymo_ymf262_generate_i16x4(struct aymo_ymf262_chip* chip, uint32_t count, int16_t y[])
{
    assert(chip);
    assert(chip->vt);
    assert(chip->vt->generate_i16x4);

    chip->vt->generate_i16x4(chip, count, y);
}


void aymo_ymf262_generate_f32x2(struct aymo_ymf262_chip* chip, uint32_t count, float y[])
{
    assert(chip);
    assert(chip->vt);
    assert(chip->vt->generate_f32x2);

    chip->vt->generate_f32x2(chip, count, y);
}


void aymo_ymf262_generate_f32x4(struct aymo_ymf262_chip* chip, uint32_t count, float y[])
{
    assert(chip);
    assert(chip->vt);
    assert(chip->vt->generate_f32x4);

    chip->vt->generate_f32x4(chip, count, y);
}


AYMO_CXX_EXTERN_C_END
