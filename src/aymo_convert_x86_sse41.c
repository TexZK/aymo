// CPU-specific inline methods for ARM NEON.
// Only #include after "aymo_cpu.h" to have inline methods.
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
#ifdef AYMO_CPU_SUPPORT_X86_SSE41

#define AYMO_KEEP_SHORTHANDS
#include "aymo_convert_x86_sse41.h"

#include <immintrin.h>

AYMO_CXX_EXTERN_C_BEGIN


static inline float reinterpret_f32_i32(int32_t i32)
{
    union { float f; int32_t i; } u;
    u.i = i32;
    return u.f;
}


#undef mm_extract_ps
#define mm_extract_ps(a, imm8)  \
    (reinterpret_f32_i32(_mm_extract_epi32(_mm_castps_si128(a), (imm8))))


void aymo_(i16_f32)(size_t n, const int16_t i16v[], float f32v[])
{
    if (n >= 8) {
        size_t nw = (n / 8);
        do {
            __m128i epi16 = _mm_loadu_si128((const void*)i16v); i16v += 8;
            __m128i epi32lo = _mm_cvtepi16_epi32(epi16);
            epi16 = _mm_shuffle_epi32(epi16, _MM_SHUFFLE(3, 2, 3, 2));
            __m128i epi32hi = _mm_cvtepi16_epi32(epi16);
            __m128 pslo = _mm_cvtepi32_ps(epi32lo);
            __m128 pshi = _mm_cvtepi32_ps(epi32hi);
            _mm_storeu_ps((void*)f32v, pslo); f32v += 4;
            _mm_storeu_ps((void*)f32v, pshi); f32v += 4;
        } while (--nw);
        n %= 8;
        if (n == 0) {
            return;
        }
    }
    if (n >= 4) {
        __m128i epi16lo = _mm_loadl_epi64((const void*)i16v); i16v += 4;
        __m128i epi32lo = _mm_cvtepi16_epi32(epi16lo);
        __m128 pslo = _mm_cvtepi32_ps(epi32lo);
        _mm_storeu_ps((void*)f32v, pslo); f32v += 4;
        n %= 4;
        if (n == 0) {
            return;
        }
    }
    switch (n) {
        case 3: {
            __m128i epi32lo = _mm_setr_epi32(i16v[0], i16v[1], i16v[2], 0);
            __m128 pslo = _mm_cvtepi32_ps(epi32lo);
            f32v[0] = mm_extract_ps(pslo, 0);
            f32v[1] = mm_extract_ps(pslo, 1);
            f32v[2] = mm_extract_ps(pslo, 2);
            break;
        }
        case 2: {
            __m128i epi32lo = _mm_setr_epi32(i16v[0], i16v[1], 0, 0);
            __m128 pslo = _mm_cvtepi32_ps(epi32lo);
            f32v[0] = mm_extract_ps(pslo, 0);
            f32v[1] = mm_extract_ps(pslo, 1);
            break;
        }
        case 1: {
            __m128i epi32lo = _mm_setr_epi32(i16v[0], 0, 0, 0);
            __m128 pslo = _mm_cvtepi32_ps(epi32lo);
            f32v[0] = mm_extract_ps(pslo, 0);
            break;
        }
        default: break;
    }
}


void aymo_(f32_i16)(size_t n, const float f32v[], int16_t i16v[])
{
    if (n >= 8) {
        size_t nw = (n / 8);
        do {
            __m128 pslo = _mm_loadu_ps((const void*)f32v); f32v += 4;
            __m128 pshi = _mm_loadu_ps((const void*)f32v); f32v += 4;
            __m128i epi32lo = _mm_cvtps_epi32(pslo);
            __m128i epi32hi = _mm_cvtps_epi32(pshi);
            __m128i epi16 = _mm_packs_epi32(epi32lo, epi32hi);
            _mm_storeu_si128((void*)i16v, epi16); i16v += 8;
        } while (--nw);
        n %= 8;
        if (n == 0) {
            return;
        }
    }
    if (n >= 4) {
        __m128 pslo = _mm_loadu_ps((const void*)f32v); f32v += 4;
        __m128i epi32lo = _mm_cvtps_epi32(pslo);
        __m128i epi16lo = _mm_packs_epi32(epi32lo, epi32lo);
        _mm_storel_epi64((void*)i16v, epi16lo); i16v += 4;
        n %= 4;
        if (n == 0) {
            return;
        }
    }
    switch (n) {
        case 3: {
            __m128 pslo = _mm_setr_ps(f32v[0], f32v[1], f32v[2], .0f);
            __m128i epi32lo = _mm_cvtps_epi32(pslo);
            __m128i epi16lo = _mm_packs_epi32(epi32lo, epi32lo);
            i16v[0] = _mm_extract_epi16(epi16lo, 0);
            i16v[1] = _mm_extract_epi16(epi16lo, 1);
            i16v[2] = _mm_extract_epi16(epi16lo, 2);
            break;
        }
        case 2: {
            __m128 pslo = _mm_setr_ps(f32v[0], f32v[1], .0f, .0f);
            __m128i epi32lo = _mm_cvtps_epi32(pslo);
            __m128i epi16lo = _mm_packs_epi32(epi32lo, epi32lo);
            i16v[0] = _mm_extract_epi16(epi16lo, 0);
            i16v[1] = _mm_extract_epi16(epi16lo, 1);
            break;
        }
        case 1: {
            __m128 pslo = _mm_setr_ps(f32v[0], .0f, .0f, .0f);
            __m128i epi32lo = _mm_cvtps_epi32(pslo);
            __m128i epi16lo = _mm_packs_epi32(epi32lo, epi32lo);
            i16v[0] = _mm_extract_epi16(epi16lo, 0);
            break;
        }
        default: break;
    }
}


void aymo_(i16_f32_1)(size_t n, const int16_t i16v[], float f32v[])
{
    const float scale = (float)(1. / 32768.);
    __m128 psk = _mm_set1_ps(scale);
    if (n >= 8) {
        size_t nw = (n / 8);
        do {
            __m128i epi16 = _mm_loadu_si128((const void*)i16v); i16v += 8;
            __m128i epi32lo = _mm_cvtepi16_epi32(epi16);
            epi16 = _mm_shuffle_epi32(epi16, _MM_SHUFFLE(3, 2, 3, 2));
            __m128i epi32hi = _mm_cvtepi16_epi32(epi16);
            __m128 pslo = _mm_cvtepi32_ps(epi32lo);
            __m128 pshi = _mm_cvtepi32_ps(epi32hi);
            pslo = _mm_mul_ps(pslo, psk);
            pshi = _mm_mul_ps(pshi, psk);
            _mm_storeu_ps((void*)f32v, pslo); f32v += 4;
            _mm_storeu_ps((void*)f32v, pshi); f32v += 4;
        } while (--nw);
        n %= 8;
        if (n == 0) {
            return;
        }
    }
    if (n >= 4) {
        __m128i epi16lo = _mm_loadl_epi64((const void*)i16v); i16v += 4;
        __m128i epi32lo = _mm_cvtepi16_epi32(epi16lo);
        __m128 pslo = _mm_cvtepi32_ps(epi32lo);
        pslo = _mm_mul_ps(pslo, psk);
        _mm_storeu_ps((void*)f32v, pslo); f32v += 4;
        n %= 4;
        if (n == 0) {
            return;
        }
    }
    switch (n) {
        case 3: {
            __m128i epi32lo = _mm_setr_epi32(i16v[0], i16v[1], i16v[2], 0);
            __m128 pslo = _mm_cvtepi32_ps(epi32lo);
            pslo = _mm_mul_ps(pslo, psk);
            f32v[0] = mm_extract_ps(pslo, 0);
            f32v[1] = mm_extract_ps(pslo, 1);
            f32v[2] = mm_extract_ps(pslo, 2);
            break;
        }
        case 2: {
            __m128i epi32lo = _mm_setr_epi32(i16v[0], i16v[1], 0, 0);
            __m128 pslo = _mm_cvtepi32_ps(epi32lo);
            pslo = _mm_mul_ps(pslo, psk);
            f32v[0] = mm_extract_ps(pslo, 0);
            f32v[1] = mm_extract_ps(pslo, 1);
            break;
        }
        case 1: {
            __m128i epi32lo = _mm_setr_epi32(i16v[0], 0, 0, 0);
            __m128 pslo = _mm_cvtepi32_ps(epi32lo);
            pslo = _mm_mul_ps(pslo, psk);
            f32v[0] = mm_extract_ps(pslo, 0);
            break;
        }
        default: break;
    }
}


void aymo_(f32_i16_1)(size_t n, const float f32v[], int16_t i16v[])
{
    const float scale = (float)(32768.);
    __m128 psk = _mm_set1_ps(scale);
    if (n >= 8) {
        size_t nw = (n / 8);
        do {
            __m128 pslo = _mm_loadu_ps((const void*)f32v); f32v += 4;
            __m128 pshi = _mm_loadu_ps((const void*)f32v); f32v += 4;
            pslo = _mm_mul_ps(pslo, psk);
            pshi = _mm_mul_ps(pshi, psk);
            __m128i epi32lo = _mm_cvtps_epi32(pslo);
            __m128i epi32hi = _mm_cvtps_epi32(pshi);
            __m128i epi16 = _mm_packs_epi32(epi32lo, epi32hi);
            _mm_storeu_si128((void*)i16v, epi16); i16v += 8;
        } while (--nw);
        n %= 8;
        if (n == 0) {
            return;
        }
    }
    if (n >= 4) {
        __m128 pslo = _mm_loadu_ps((const void*)f32v); f32v += 4;
        pslo = _mm_mul_ps(pslo, psk);
        __m128i epi32lo = _mm_cvtps_epi32(pslo);
        __m128i epi16lo = _mm_packs_epi32(epi32lo, epi32lo);
        _mm_storel_epi64((void*)i16v, epi16lo); i16v += 4;
        n %= 4;
        if (n == 0) {
            return;
        }
    }
    switch (n) {
        case 3: {
            __m128 pslo = _mm_setr_ps(f32v[0], f32v[1], f32v[2], .0f);
            pslo = _mm_mul_ps(pslo, psk);
            __m128i epi32lo = _mm_cvtps_epi32(pslo);
            __m128i epi16lo = _mm_packs_epi32(epi32lo, epi32lo);
            i16v[0] = _mm_extract_epi16(epi16lo, 0);
            i16v[1] = _mm_extract_epi16(epi16lo, 1);
            i16v[2] = _mm_extract_epi16(epi16lo, 2);
            break;
        }
        case 2: {
            __m128 pslo = _mm_setr_ps(f32v[0], f32v[1], .0f, .0f);
            pslo = _mm_mul_ps(pslo, psk);
            __m128i epi32lo = _mm_cvtps_epi32(pslo);
            __m128i epi16lo = _mm_packs_epi32(epi32lo, epi32lo);
            i16v[0] = _mm_extract_epi16(epi16lo, 0);
            i16v[1] = _mm_extract_epi16(epi16lo, 1);
            break;
        }
        case 1: {
            __m128 pslo = _mm_setr_ps(f32v[0], .0f, .0f, .0f);
            pslo = _mm_mul_ps(pslo, psk);
            __m128i epi32lo = _mm_cvtps_epi32(pslo);
            __m128i epi16lo = _mm_packs_epi32(epi32lo, epi32lo);
            i16v[0] = _mm_extract_epi16(epi16lo, 0);
            break;
        }
        default: break;
    }
}


void aymo_(i16_f32_k)(size_t n, const int16_t i16v[], float f32v[], float scale)
{
    __m128 psk = _mm_set1_ps(scale);
    if (n >= 8) {
        size_t nw = (n / 8);
        do {
            __m128i epi16 = _mm_loadu_si128((const void*)i16v); i16v += 8;
            __m128i epi32lo = _mm_cvtepi16_epi32(epi16);
            epi16 = _mm_shuffle_epi32(epi16, _MM_SHUFFLE(3, 2, 3, 2));
            __m128i epi32hi = _mm_cvtepi16_epi32(epi16);
            __m128 pslo = _mm_cvtepi32_ps(epi32lo);
            __m128 pshi = _mm_cvtepi32_ps(epi32hi);
            pslo = _mm_mul_ps(pslo, psk);
            pshi = _mm_mul_ps(pshi, psk);
            _mm_storeu_ps((void*)f32v, pslo); f32v += 4;
            _mm_storeu_ps((void*)f32v, pshi); f32v += 4;
        } while (--nw);
        n %= 8;
        if (n == 0) {
            return;
        }
    }
    if (n >= 4) {
        __m128i epi16lo = _mm_loadl_epi64((const void*)i16v); i16v += 4;
        __m128i epi32lo = _mm_cvtepi16_epi32(epi16lo);
        __m128 pslo = _mm_cvtepi32_ps(epi32lo);
        pslo = _mm_mul_ps(pslo, psk);
        _mm_storeu_ps((void*)f32v, pslo); f32v += 4;
        n %= 4;
        if (n == 0) {
            return;
        }
    }
    switch (n) {
        case 3: {
            __m128i epi32lo = _mm_setr_epi32(i16v[0], i16v[1], i16v[2], 0);
            __m128 pslo = _mm_cvtepi32_ps(epi32lo);
            pslo = _mm_mul_ps(pslo, psk);
            f32v[0] = mm_extract_ps(pslo, 0);
            f32v[1] = mm_extract_ps(pslo, 1);
            f32v[2] = mm_extract_ps(pslo, 2);
            break;
        }
        case 2: {
            __m128i epi32lo = _mm_setr_epi32(i16v[0], i16v[1], 0, 0);
            __m128 pslo = _mm_cvtepi32_ps(epi32lo);
            pslo = _mm_mul_ps(pslo, psk);
            f32v[0] = mm_extract_ps(pslo, 0);
            f32v[1] = mm_extract_ps(pslo, 1);
            break;
        }
        case 1: {
            __m128i epi32lo = _mm_setr_epi32(i16v[0], 0, 0, 0);
            __m128 pslo = _mm_cvtepi32_ps(epi32lo);
            pslo = _mm_mul_ps(pslo, psk);
            f32v[0] = mm_extract_ps(pslo, 0);
            break;
        }
        default: break;
    }
}


void aymo_(f32_i16_k)(size_t n, const float f32v[], int16_t i16v[], float scale)
{
    __m128 psk = _mm_set1_ps(scale);
    if (n >= 8) {
        size_t nw = (n / 8);
        do {
            __m128 pslo = _mm_loadu_ps((const void*)f32v); f32v += 4;
            __m128 pshi = _mm_loadu_ps((const void*)f32v); f32v += 4;
            pslo = _mm_mul_ps(pslo, psk);
            pshi = _mm_mul_ps(pshi, psk);
            __m128i epi32lo = _mm_cvtps_epi32(pslo);
            __m128i epi32hi = _mm_cvtps_epi32(pshi);
            __m128i epi16 = _mm_packs_epi32(epi32lo, epi32hi);
            _mm_storeu_si128((void*)i16v, epi16); i16v += 8;
        } while (--nw);
        n %= 8;
        if (n == 0) {
            return;
        }
    }
    if (n >= 4) {
        __m128 pslo = _mm_loadu_ps((const void*)f32v); f32v += 4;
        pslo = _mm_mul_ps(pslo, psk);
        __m128i epi32lo = _mm_cvtps_epi32(pslo);
        __m128i epi16lo = _mm_packs_epi32(epi32lo, epi32lo);
        _mm_storel_epi64((void*)i16v, epi16lo); i16v += 4;
        n %= 4;
        if (n == 0) {
            return;
        }
    }
    switch (n) {
        case 3: {
            __m128 pslo = _mm_setr_ps(f32v[0], f32v[1], f32v[2], .0f);
            pslo = _mm_mul_ps(pslo, psk);
            __m128i epi32lo = _mm_cvtps_epi32(pslo);
            __m128i epi16lo = _mm_packs_epi32(epi32lo, epi32lo);
            i16v[0] = _mm_extract_epi16(epi16lo, 0);
            i16v[1] = _mm_extract_epi16(epi16lo, 1);
            i16v[2] = _mm_extract_epi16(epi16lo, 2);
            break;
        }
        case 2: {
            __m128 pslo = _mm_setr_ps(f32v[0], f32v[1], .0f, .0f);
            pslo = _mm_mul_ps(pslo, psk);
            __m128i epi32lo = _mm_cvtps_epi32(pslo);
            __m128i epi16lo = _mm_packs_epi32(epi32lo, epi32lo);
            i16v[0] = _mm_extract_epi16(epi16lo, 0);
            i16v[1] = _mm_extract_epi16(epi16lo, 1);
            break;
        }
        case 1: {
            __m128 pslo = _mm_setr_ps(f32v[0], .0f, .0f, .0f);
            pslo = _mm_mul_ps(pslo, psk);
            __m128i epi32lo = _mm_cvtps_epi32(pslo);
            __m128i epi16lo = _mm_packs_epi32(epi32lo, epi32lo);
            i16v[0] = _mm_extract_epi16(epi16lo, 0);
            break;
        }
        default: break;
    }
}


void aymo_(u16_f32)(size_t n, const uint16_t u16v[], float f32v[])
{
    if (n >= 8) {
        size_t nw = (n / 8);
        do {
            __m128i epu16 = _mm_loadu_si128((const void*)u16v); u16v += 8;
            __m128i epu32lo = _mm_cvtepu16_epi32(epu16);
            epu16 = _mm_shuffle_epi32(epu16, _MM_SHUFFLE(3, 2, 3, 2));
            __m128i epu32hi = _mm_cvtepu16_epi32(epu16);
            __m128 pslo = _mm_cvtepi32_ps(epu32lo);
            __m128 pshi = _mm_cvtepi32_ps(epu32hi);
            _mm_storeu_ps((void*)f32v, pslo); f32v += 4;
            _mm_storeu_ps((void*)f32v, pshi); f32v += 4;
        } while (--nw);
        n %= 8;
        if (n == 0) {
            return;
        }
    }
    if (n >= 4) {
        __m128i epu16lo = _mm_loadl_epi64((const void*)u16v); u16v += 4;
        __m128i epu32lo = _mm_cvtepu16_epi32(epu16lo);
        __m128 pslo = _mm_cvtepi32_ps(epu32lo);
        _mm_storeu_ps((void*)f32v, pslo); f32v += 4;
        n %= 4;
        if (n == 0) {
            return;
        }
    }
    switch (n) {
        case 3: {
            __m128i epi32lo = _mm_setr_epi32((int32_t)(uint32_t)u16v[0],
                                             (int32_t)(uint32_t)u16v[1],
                                             (int32_t)(uint32_t)u16v[2], 0);
            __m128 pslo = _mm_cvtepi32_ps(epi32lo);
            f32v[0] = mm_extract_ps(pslo, 0);
            f32v[1] = mm_extract_ps(pslo, 1);
            f32v[2] = mm_extract_ps(pslo, 2);
            break;
        }
        case 2: {
            __m128i epi32lo = _mm_setr_epi32((int32_t)(uint32_t)u16v[0],
                                             (int32_t)(uint32_t)u16v[1], 0, 0);
            __m128 pslo = _mm_cvtepi32_ps(epi32lo);
            f32v[0] = mm_extract_ps(pslo, 0);
            f32v[1] = mm_extract_ps(pslo, 1);
            break;
        }
        case 1: {
            __m128i epi32lo = _mm_setr_epi32((int32_t)(uint32_t)u16v[0], 0, 0, 0);
            __m128 pslo = _mm_cvtepi32_ps(epi32lo);
            f32v[0] = mm_extract_ps(pslo, 0);
            break;
        }
        default: break;
    }
}


void aymo_(f32_u16)(size_t n, const float f32v[], uint16_t u16v[])
{
    if (n >= 8) {
        size_t nw = (n / 8);
        do {
            __m128 pslo = _mm_loadu_ps((const void*)f32v); f32v += 4;
            __m128 pshi = _mm_loadu_ps((const void*)f32v); f32v += 4;
            __m128i epu32lo = _mm_cvtps_epi32(pslo);
            __m128i epu32hi = _mm_cvtps_epi32(pshi);
            __m128i epu16 = _mm_packus_epi32(epu32lo, epu32hi);
            _mm_storeu_si128((void*)u16v, epu16); u16v += 8;
        } while (--nw);
        n %= 8;
        if (n == 0) {
            return;
        }
    }
    if (n >= 4) {
        __m128 pslo = _mm_loadu_ps((const void*)f32v); f32v += 4;
        __m128i epu32lo = _mm_cvtps_epi32(pslo);
        __m128i epu16lo = _mm_packus_epi32(epu32lo, epu32lo);
        _mm_storel_epi64((void*)u16v, epu16lo); u16v += 4;
        n %= 4;
        if (n == 0) {
            return;
        }
    }
    switch (n) {
        case 3: {
            __m128 pslo = _mm_setr_ps(f32v[0], f32v[1], f32v[2], .0f);
            __m128i epi32lo = _mm_cvtps_epi32(pslo);
            __m128i epu16lo = _mm_packus_epi32(epi32lo, epi32lo);
            u16v[0] = (uint16_t)_mm_extract_epi16(epu16lo, 0);
            u16v[1] = (uint16_t)_mm_extract_epi16(epu16lo, 1);
            u16v[2] = (uint16_t)_mm_extract_epi16(epu16lo, 2);
            break;
        }
        case 2: {
            __m128 pslo = _mm_setr_ps(f32v[0], f32v[1], .0f, .0f);
            __m128i epi32lo = _mm_cvtps_epi32(pslo);
            __m128i epu16lo = _mm_packus_epi32(epi32lo, epi32lo);
            u16v[0] = (uint16_t)_mm_extract_epi16(epu16lo, 0);
            u16v[1] = (uint16_t)_mm_extract_epi16(epu16lo, 1);
            break;
        }
        case 1: {
            __m128 pslo = _mm_setr_ps(f32v[0], .0f, .0f, .0f);
            __m128i epi32lo = _mm_cvtps_epi32(pslo);
            __m128i epu16lo = _mm_packus_epi32(epi32lo, epi32lo);
            u16v[0] = (uint16_t)_mm_extract_epi16(epu16lo, 0);
            break;
        }
        default: break;
    }
}


void aymo_(u16_f32_1)(size_t n, const uint16_t u16v[], float f32v[])
{
    const float scale = (float)(1. / 32768.);
    __m128 psk = _mm_set1_ps(scale);
    if (n >= 8) {
        size_t nw = (n / 8);
        do {
            __m128i epu16 = _mm_loadu_si128((const void*)u16v); u16v += 8;
            __m128i epu32lo = _mm_cvtepu16_epi32(epu16);
            epu16 = _mm_shuffle_epi32(epu16, _MM_SHUFFLE(3, 2, 3, 2));
            __m128i epu32hi = _mm_cvtepu16_epi32(epu16);
            __m128 pslo = _mm_cvtepi32_ps(epu32lo);
            __m128 pshi = _mm_cvtepi32_ps(epu32hi);
            pslo = _mm_mul_ps(pslo, psk);
            pshi = _mm_mul_ps(pshi, psk);
            _mm_storeu_ps((void*)f32v, pslo); f32v += 4;
            _mm_storeu_ps((void*)f32v, pshi); f32v += 4;
        } while (--nw);
        n %= 8;
        if (n == 0) {
            return;
        }
    }
    if (n >= 4) {
        __m128i epu16lo = _mm_loadl_epi64((const void*)u16v); u16v += 4;
        __m128i epu32lo = _mm_cvtepu16_epi32(epu16lo);
        __m128 pslo = _mm_cvtepi32_ps(epu32lo);
        pslo = _mm_mul_ps(pslo, psk);
        _mm_storeu_ps((void*)f32v, pslo); f32v += 4;
        n %= 4;
        if (n == 0) {
            return;
        }
    }
    switch (n) {
        case 3: {
            __m128i epi32lo = _mm_setr_epi32((int32_t)(uint32_t)u16v[0],
                                             (int32_t)(uint32_t)u16v[1],
                                             (int32_t)(uint32_t)u16v[2], 0);
            __m128 pslo = _mm_cvtepi32_ps(epi32lo);
            pslo = _mm_mul_ps(pslo, psk);
            f32v[0] = mm_extract_ps(pslo, 0);
            f32v[1] = mm_extract_ps(pslo, 1);
            f32v[2] = mm_extract_ps(pslo, 2);
            break;
        }
        case 2: {
            __m128i epi32lo = _mm_setr_epi32((int32_t)(uint32_t)u16v[0],
                                             (int32_t)(uint32_t)u16v[1], 0, 0);
            __m128 pslo = _mm_cvtepi32_ps(epi32lo);
            pslo = _mm_mul_ps(pslo, psk);
            f32v[0] = mm_extract_ps(pslo, 0);
            f32v[1] = mm_extract_ps(pslo, 1);
            break;
        }
        case 1: {
            __m128i epi32lo = _mm_setr_epi32((int32_t)(uint32_t)u16v[0], 0, 0, 0);
            __m128 pslo = _mm_cvtepi32_ps(epi32lo);
            pslo = _mm_mul_ps(pslo, psk);
            f32v[0] = mm_extract_ps(pslo, 0);
            break;
        }
        default: break;
    }
}


void aymo_(f32_u16_1)(size_t n, const float f32v[], uint16_t u16v[])
{
    const float scale = (float)(32768.);
    __m128 psk = _mm_set1_ps(scale);
    if (n >= 8) {
        size_t nw = (n / 8);
        do {
            __m128 pslo = _mm_loadu_ps((const void*)f32v); f32v += 4;
            __m128 pshi = _mm_loadu_ps((const void*)f32v); f32v += 4;
            pslo = _mm_mul_ps(pslo, psk);
            pshi = _mm_mul_ps(pshi, psk);
            __m128i epu32lo = _mm_cvtps_epi32(pslo);
            __m128i epu32hi = _mm_cvtps_epi32(pshi);
            __m128i epu16 = _mm_packus_epi32(epu32lo, epu32hi);
            _mm_storeu_si128((void*)u16v, epu16); u16v += 8;
        } while (--nw);
        n %= 8;
        if (n == 0) {
            return;
        }
    }
    if (n >= 4) {
        __m128 pslo = _mm_loadu_ps((const void*)f32v); f32v += 4;
        pslo = _mm_mul_ps(pslo, psk);
        __m128i epu32lo = _mm_cvtps_epi32(pslo);
        __m128i epu16lo = _mm_packus_epi32(epu32lo, epu32lo);
        _mm_storel_epi64((void*)u16v, epu16lo); u16v += 4;
        n %= 4;
        if (n == 0) {
            return;
        }
    }
    switch (n) {
        case 3: {
            __m128 pslo = _mm_setr_ps(f32v[0], f32v[1], f32v[2], .0f);
            pslo = _mm_mul_ps(pslo, psk);
            __m128i epi32lo = _mm_cvtps_epi32(pslo);
            __m128i epu16lo = _mm_packus_epi32(epi32lo, epi32lo);
            u16v[0] = (uint16_t)_mm_extract_epi16(epu16lo, 0);
            u16v[1] = (uint16_t)_mm_extract_epi16(epu16lo, 1);
            u16v[2] = (uint16_t)_mm_extract_epi16(epu16lo, 2);
            break;
        }
        case 2: {
            __m128 pslo = _mm_setr_ps(f32v[0], f32v[1], .0f, .0f);
            pslo = _mm_mul_ps(pslo, psk);
            __m128i epi32lo = _mm_cvtps_epi32(pslo);
            __m128i epu16lo = _mm_packus_epi32(epi32lo, epi32lo);
            u16v[0] = (uint16_t)_mm_extract_epi16(epu16lo, 0);
            u16v[1] = (uint16_t)_mm_extract_epi16(epu16lo, 1);
            break;
        }
        case 1: {
            __m128 pslo = _mm_setr_ps(f32v[0], .0f, .0f, .0f);
            pslo = _mm_mul_ps(pslo, psk);
            __m128i epi32lo = _mm_cvtps_epi32(pslo);
            __m128i epu16lo = _mm_packus_epi32(epi32lo, epi32lo);
            u16v[0] = (uint16_t)_mm_extract_epi16(epu16lo, 0);
            break;
        }
        default: break;
    }
}


void aymo_(u16_f32_k)(size_t n, const uint16_t u16v[], float f32v[], float scale)
{
    __m128 psk = _mm_set1_ps(scale);
    if (n >= 8) {
        size_t nw = (n / 8);
        do {
            __m128i epu16 = _mm_loadu_si128((const void*)u16v); u16v += 8;
            __m128i epu32lo = _mm_cvtepu16_epi32(epu16);
            epu16 = _mm_shuffle_epi32(epu16, _MM_SHUFFLE(3, 2, 3, 2));
            __m128i epu32hi = _mm_cvtepu16_epi32(epu16);
            __m128 pslo = _mm_cvtepi32_ps(epu32lo);
            __m128 pshi = _mm_cvtepi32_ps(epu32hi);
            pslo = _mm_mul_ps(pslo, psk);
            pshi = _mm_mul_ps(pshi, psk);
            _mm_storeu_ps((void*)f32v, pslo); f32v += 4;
            _mm_storeu_ps((void*)f32v, pshi); f32v += 4;
        } while (--nw);
        n %= 8;
        if (n == 0) {
            return;
        }
    }
    if (n >= 4) {
        __m128i epu16lo = _mm_loadl_epi64((const void*)u16v); u16v += 4;
        __m128i epu32lo = _mm_cvtepu16_epi32(epu16lo);
        __m128 pslo = _mm_cvtepi32_ps(epu32lo);
        pslo = _mm_mul_ps(pslo, psk);
        _mm_storeu_ps((void*)f32v, pslo); f32v += 4;
        n %= 4;
        if (n == 0) {
            return;
        }
    }
    switch (n) {
        case 3: {
            __m128i epi32lo = _mm_setr_epi32((int32_t)(uint32_t)u16v[0],
                                             (int32_t)(uint32_t)u16v[1],
                                             (int32_t)(uint32_t)u16v[2], 0);
            __m128 pslo = _mm_cvtepi32_ps(epi32lo);
            pslo = _mm_mul_ps(pslo, psk);
            f32v[0] = mm_extract_ps(pslo, 0);
            f32v[1] = mm_extract_ps(pslo, 1);
            f32v[2] = mm_extract_ps(pslo, 2);
            break;
        }
        case 2: {
            __m128i epi32lo = _mm_setr_epi32((int32_t)(uint32_t)u16v[0],
                                             (int32_t)(uint32_t)u16v[1], 0, 0);
            __m128 pslo = _mm_cvtepi32_ps(epi32lo);
            pslo = _mm_mul_ps(pslo, psk);
            f32v[0] = mm_extract_ps(pslo, 0);
            f32v[1] = mm_extract_ps(pslo, 1);
            break;
        }
        case 1: {
            __m128i epi32lo = _mm_setr_epi32((int32_t)(uint32_t)u16v[0], 0, 0, 0);
            __m128 pslo = _mm_cvtepi32_ps(epi32lo);
            pslo = _mm_mul_ps(pslo, psk);
            f32v[0] = mm_extract_ps(pslo, 0);
            break;
        }
        default: break;
    }
}


void aymo_(f32_u16_k)(size_t n, const float f32v[], uint16_t u16v[], float scale)
{
    __m128 psk = _mm_set1_ps(scale);
    if (n >= 8) {
        size_t nw = (n / 8);
        do {
            __m128 pslo = _mm_loadu_ps((const void*)f32v); f32v += 4;
            __m128 pshi = _mm_loadu_ps((const void*)f32v); f32v += 4;
            pslo = _mm_mul_ps(pslo, psk);
            pshi = _mm_mul_ps(pshi, psk);
            __m128i epu32lo = _mm_cvtps_epi32(pslo);
            __m128i epu32hi = _mm_cvtps_epi32(pshi);
            __m128i epu16 = _mm_packus_epi32(epu32lo, epu32hi);
            _mm_storeu_si128((void*)u16v, epu16); u16v += 8;
        } while (--nw);
        n %= 8;
        if (n == 0) {
            return;
        }
    }
    if (n >= 4) {
        __m128 pslo = _mm_loadu_ps((const void*)f32v); f32v += 4;
        pslo = _mm_mul_ps(pslo, psk);
        __m128i epu32lo = _mm_cvtps_epi32(pslo);
        __m128i epu16lo = _mm_packus_epi32(epu32lo, epu32lo);
        _mm_storel_epi64((void*)u16v, epu16lo); u16v += 4;
        n %= 4;
        if (n == 0) {
            return;
        }
    }
    switch (n) {
        case 3: {
            __m128 pslo = _mm_setr_ps(f32v[0], f32v[1], f32v[2], .0f);
            pslo = _mm_mul_ps(pslo, psk);
            __m128i epi32lo = _mm_cvtps_epi32(pslo);
            __m128i epu16lo = _mm_packus_epi32(epi32lo, epi32lo);
            u16v[0] = (uint16_t)_mm_extract_epi16(epu16lo, 0);
            u16v[1] = (uint16_t)_mm_extract_epi16(epu16lo, 1);
            u16v[2] = (uint16_t)_mm_extract_epi16(epu16lo, 2);
            break;
        }
        case 2: {
            __m128 pslo = _mm_setr_ps(f32v[0], f32v[1], .0f, .0f);
            pslo = _mm_mul_ps(pslo, psk);
            __m128i epi32lo = _mm_cvtps_epi32(pslo);
            __m128i epu16lo = _mm_packus_epi32(epi32lo, epi32lo);
            u16v[0] = (uint16_t)_mm_extract_epi16(epu16lo, 0);
            u16v[1] = (uint16_t)_mm_extract_epi16(epu16lo, 1);
            break;
        }
        case 1: {
            __m128 pslo = _mm_setr_ps(f32v[0], .0f, .0f, .0f);
            pslo = _mm_mul_ps(pslo, psk);
            __m128i epi32lo = _mm_cvtps_epi32(pslo);
            __m128i epu16lo = _mm_packus_epi32(epi32lo, epi32lo);
            u16v[0] = (uint16_t)_mm_extract_epi16(epu16lo, 0);
            break;
        }
        default: break;
    }
}


AYMO_CXX_EXTERN_C_END

#endif  // AYMO_CPU_SUPPORT_X86_SSE41
