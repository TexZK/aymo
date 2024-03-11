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
#ifdef AYMO_CPU_SUPPORT_X86_AVX2

#include "aymo_convert_x86_sse41.h"
#define AYMO_KEEP_SHORTHANDS
#include "aymo_convert_x86_avx2.h"

#include <immintrin.h>

AYMO_CXX_EXTERN_C_BEGIN


void aymo_(i16_f32)(size_t n, const int16_t i16v[], float f32v[])
{
    if (n >= 16) {
        size_t nw = (n / 16);
        n %= 16;
        do {
            __m256i epi16 = _mm256_loadu_si256((const void*)i16v); i16v += 16;
            __m256i epi32lo = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(epi16, 0));
            __m256i epi32hi = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(epi16, 1));
            __m256 pslo = _mm256_cvtepi32_ps(epi32lo);
            __m256 pshi = _mm256_cvtepi32_ps(epi32hi);
            _mm256_storeu_ps((void*)f32v, pslo); f32v += 8;
            _mm256_storeu_ps((void*)f32v, pshi); f32v += 8;
        } while (--nw);
    }
    if (n) {
        aymo_convert_x86_sse41_i16_f32(n, i16v, f32v);
    }
}


void aymo_(f32_i16)(size_t n, const float f32v[], int16_t i16v[])
{
    if (n >= 16) {
        size_t nw = (n / 16);
        n %= 16;
        do {
            __m256 pslo = _mm256_loadu_ps((const void*)f32v); f32v += 8;
            __m256 pshi = _mm256_loadu_ps((const void*)f32v); f32v += 8;
            __m256i epi32lo = _mm256_cvtps_epi32(pslo);
            __m256i epi32hi = _mm256_cvtps_epi32(pshi);
            __m128i epi32lohi = _mm256_extracti128_si256(epi32lo, 1);
            __m128i epi32hilo = _mm256_extracti128_si256(epi32hi, 0);
            epi32lo = _mm256_inserti128_si256(epi32lo, epi32hilo, 1);
            epi32hi = _mm256_inserti128_si256(epi32hi, epi32lohi, 0);
            __m256i epi16 = _mm256_packs_epi32(epi32lo, epi32hi);
            _mm256_storeu_si256((void*)i16v, epi16); i16v += 16;
        } while (--nw);
    }
    if (n) {
        aymo_convert_x86_sse41_f32_i16(n, f32v, i16v);
    }
}


void aymo_(i16_f32_1)(size_t n, const int16_t i16v[], float f32v[])
{
    const float scale = (float)(1. / 32768.);
    __m256 psk = _mm256_set1_ps(scale);
    if (n >= 16) {
        size_t nw = (n / 16);
        n %= 16;
        do {
            __m256i epi16 = _mm256_loadu_si256((const void*)i16v); i16v += 16;
            __m256i epi32lo = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(epi16, 0));
            __m256i epi32hi = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(epi16, 1));
            __m256 pslo = _mm256_cvtepi32_ps(epi32lo);
            __m256 pshi = _mm256_cvtepi32_ps(epi32hi);
            pslo = _mm256_mul_ps(pslo, psk);
            pshi = _mm256_mul_ps(pshi, psk);
            _mm256_storeu_ps((void*)f32v, pslo); f32v += 8;
            _mm256_storeu_ps((void*)f32v, pshi); f32v += 8;
        } while (--nw);
    }
    if (n) {
        aymo_convert_x86_sse41_i16_f32_1(n, i16v, f32v);
    }
}


void aymo_(f32_i16_1)(size_t n, const float f32v[], int16_t i16v[])
{
    const float scale = (float)(32768.);
    __m256 psk = _mm256_set1_ps(scale);
    if (n >= 16) {
        size_t nw = (n / 16);
        n %= 16;
        do {
            __m256 pslo = _mm256_loadu_ps((const void*)f32v); f32v += 8;
            __m256 pshi = _mm256_loadu_ps((const void*)f32v); f32v += 8;
            pslo = _mm256_mul_ps(pslo, psk);
            pshi = _mm256_mul_ps(pshi, psk);
            __m256i epi32lo = _mm256_cvtps_epi32(pslo);
            __m256i epi32hi = _mm256_cvtps_epi32(pshi);
            __m128i epi32lohi = _mm256_extracti128_si256(epi32lo, 1);
            __m128i epi32hilo = _mm256_extracti128_si256(epi32hi, 0);
            epi32lo = _mm256_inserti128_si256(epi32lo, epi32hilo, 1);
            epi32hi = _mm256_inserti128_si256(epi32hi, epi32lohi, 0);
            __m256i epi16 = _mm256_packs_epi32(epi32lo, epi32hi);
            _mm256_storeu_si256((void*)i16v, epi16); i16v += 16;
        } while (--nw);
    }
    if (n) {
        aymo_convert_x86_sse41_f32_i16_1(n, f32v, i16v);
    }
}


void aymo_(i16_f32_k)(size_t n, const int16_t i16v[], float f32v[], float scale)
{
    __m256 psk = _mm256_set1_ps(scale);
    if (n >= 16) {
        size_t nw = (n / 16);
        n %= 16;
        do {
            __m256i epi16 = _mm256_loadu_si256((const void*)i16v); i16v += 16;
            __m256i epi32lo = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(epi16, 0));
            __m256i epi32hi = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(epi16, 1));
            __m256 pslo = _mm256_cvtepi32_ps(epi32lo);
            __m256 pshi = _mm256_cvtepi32_ps(epi32hi);
            pslo = _mm256_mul_ps(pslo, psk);
            pshi = _mm256_mul_ps(pshi, psk);
            _mm256_storeu_ps((void*)f32v, pslo); f32v += 8;
            _mm256_storeu_ps((void*)f32v, pshi); f32v += 8;
        } while (--nw);
    }
    if (n) {
        aymo_convert_x86_sse41_i16_f32_k(n, i16v, f32v, scale);
    }
}


void aymo_(f32_i16_k)(size_t n, const float f32v[], int16_t i16v[], float scale)
{
    __m256 psk = _mm256_set1_ps(scale);
    if (n >= 16) {
        size_t nw = (n / 16);
        n %= 16;
        do {
            __m256 pslo = _mm256_loadu_ps((const void*)f32v); f32v += 8;
            __m256 pshi = _mm256_loadu_ps((const void*)f32v); f32v += 8;
            pslo = _mm256_mul_ps(pslo, psk);
            pshi = _mm256_mul_ps(pshi, psk);
            __m256i epi32lo = _mm256_cvtps_epi32(pslo);
            __m256i epi32hi = _mm256_cvtps_epi32(pshi);
            __m128i epi32lohi = _mm256_extracti128_si256(epi32lo, 1);
            __m128i epi32hilo = _mm256_extracti128_si256(epi32hi, 0);
            epi32lo = _mm256_inserti128_si256(epi32lo, epi32hilo, 1);
            epi32hi = _mm256_inserti128_si256(epi32hi, epi32lohi, 0);
            __m256i epi16 = _mm256_packs_epi32(epi32lo, epi32hi);
            _mm256_storeu_si256((void*)i16v, epi16); i16v += 16;
        } while (--nw);
    }
    if (n) {
        aymo_convert_x86_sse41_f32_i16_k(n, f32v, i16v, scale);
    }
}


void aymo_(u16_f32)(size_t n, const uint16_t u16v[], float f32v[])
{
    if (n >= 16) {
        size_t nw = (n / 16);
        n %= 16;
        do {
            __m256i epu16 = _mm256_loadu_si256((const void*)u16v); u16v += 16;
            __m256i epi32lo = _mm256_cvtepu16_epi32(_mm256_extracti128_si256(epu16, 0));
            __m256i epi32hi = _mm256_cvtepu16_epi32(_mm256_extracti128_si256(epu16, 1));
            __m256 pslo = _mm256_cvtepi32_ps(epi32lo);
            __m256 pshi = _mm256_cvtepi32_ps(epi32hi);
            _mm256_storeu_ps((void*)f32v, pslo); f32v += 8;
            _mm256_storeu_ps((void*)f32v, pshi); f32v += 8;
        } while (--nw);
    }
    if (n) {
        aymo_convert_x86_sse41_u16_f32(n, u16v, f32v);
    }
}


void aymo_(f32_u16)(size_t n, const float f32v[], uint16_t u16v[])
{
    if (n >= 16) {
        size_t nw = (n / 16);
        n %= 16;
        do {
            __m256 pslo = _mm256_loadu_ps((const void*)f32v); f32v += 8;
            __m256 pshi = _mm256_loadu_ps((const void*)f32v); f32v += 8;
            __m256i epi32lo = _mm256_cvtps_epi32(pslo);
            __m256i epi32hi = _mm256_cvtps_epi32(pshi);
            __m128i epi32lohi = _mm256_extracti128_si256(epi32lo, 1);
            __m128i epi32hilo = _mm256_extracti128_si256(epi32hi, 0);
            epi32lo = _mm256_inserti128_si256(epi32lo, epi32hilo, 1);
            epi32hi = _mm256_inserti128_si256(epi32hi, epi32lohi, 0);
            __m256i epu16 = _mm256_packus_epi32(epi32lo, epi32hi);
            _mm256_storeu_si256((void*)u16v, epu16); u16v += 16;
        } while (--nw);
    }
    if (n) {
        aymo_convert_x86_sse41_f32_u16(n, f32v, u16v);
    }
}


void aymo_(u16_f32_1)(size_t n, const uint16_t u16v[], float f32v[])
{
    const float scale = (float)(1. / 32768.);
    __m256 psk = _mm256_set1_ps(scale);
    if (n >= 16) {
        size_t nw = (n / 16);
        n %= 16;
        do {
            __m256i epu16 = _mm256_loadu_si256((const void*)u16v); u16v += 16;
            __m256i epi32lo = _mm256_cvtepu16_epi32(_mm256_extracti128_si256(epu16, 0));
            __m256i epi32hi = _mm256_cvtepu16_epi32(_mm256_extracti128_si256(epu16, 1));
            __m256 pslo = _mm256_cvtepi32_ps(epi32lo);
            __m256 pshi = _mm256_cvtepi32_ps(epi32hi);
            pslo = _mm256_mul_ps(pslo, psk);
            pshi = _mm256_mul_ps(pshi, psk);
            _mm256_storeu_ps((void*)f32v, pslo); f32v += 8;
            _mm256_storeu_ps((void*)f32v, pshi); f32v += 8;
        } while (--nw);
    }
    if (n) {
        aymo_convert_x86_sse41_u16_f32_1(n, u16v, f32v);
    }
}


void aymo_(f32_u16_1)(size_t n, const float f32v[], uint16_t u16v[])
{
    const float scale = (float)(32768.);
    __m256 psk = _mm256_set1_ps(scale);
    if (n >= 16) {
        size_t nw = (n / 16);
        n %= 16;
        do {
            __m256 pslo = _mm256_loadu_ps((const void*)f32v); f32v += 8;
            __m256 pshi = _mm256_loadu_ps((const void*)f32v); f32v += 8;
            pslo = _mm256_mul_ps(pslo, psk);
            pshi = _mm256_mul_ps(pshi, psk);
            __m256i epi32lo = _mm256_cvtps_epi32(pslo);
            __m256i epi32hi = _mm256_cvtps_epi32(pshi);
            __m128i epi32lohi = _mm256_extracti128_si256(epi32lo, 1);
            __m128i epi32hilo = _mm256_extracti128_si256(epi32hi, 0);
            epi32lo = _mm256_inserti128_si256(epi32lo, epi32hilo, 1);
            epi32hi = _mm256_inserti128_si256(epi32hi, epi32lohi, 0);
            __m256i epu16 = _mm256_packus_epi32(epi32lo, epi32hi);
            _mm256_storeu_si256((void*)u16v, epu16); u16v += 16;
        } while (--nw);
    }
    if (n) {
        aymo_convert_x86_sse41_f32_u16_1(n, f32v, u16v);
    }
}


void aymo_(u16_f32_k)(size_t n, const uint16_t u16v[], float f32v[], float scale)
{
    __m256 psk = _mm256_set1_ps(scale);
    if (n >= 16) {
        size_t nw = (n / 16);
        n %= 16;
        do {
            __m256i epu16 = _mm256_loadu_si256((const void*)u16v); u16v += 16;
            __m256i epi32lo = _mm256_cvtepu16_epi32(_mm256_extracti128_si256(epu16, 0));
            __m256i epi32hi = _mm256_cvtepu16_epi32(_mm256_extracti128_si256(epu16, 1));
            __m256 pslo = _mm256_cvtepi32_ps(epi32lo);
            __m256 pshi = _mm256_cvtepi32_ps(epi32hi);
            pslo = _mm256_mul_ps(pslo, psk);
            pshi = _mm256_mul_ps(pshi, psk);
            _mm256_storeu_ps((void*)f32v, pslo); f32v += 8;
            _mm256_storeu_ps((void*)f32v, pshi); f32v += 8;
        } while (--nw);
    }
    if (n) {
        aymo_convert_x86_sse41_u16_f32_k(n, u16v, f32v, scale);
    }
}


void aymo_(f32_u16_k)(size_t n, const float f32v[], uint16_t u16v[], float scale)
{
    __m256 psk = _mm256_set1_ps(scale);
    if (n >= 16) {
        size_t nw = (n / 16);
        n %= 16;
        do {
            __m256 pslo = _mm256_loadu_ps((const void*)f32v); f32v += 8;
            __m256 pshi = _mm256_loadu_ps((const void*)f32v); f32v += 8;
            pslo = _mm256_mul_ps(pslo, psk);
            pshi = _mm256_mul_ps(pshi, psk);
            __m256i epi32lo = _mm256_cvtps_epi32(pslo);
            __m256i epi32hi = _mm256_cvtps_epi32(pshi);
            __m128i epi32lohi = _mm256_extracti128_si256(epi32lo, 1);
            __m128i epi32hilo = _mm256_extracti128_si256(epi32hi, 0);
            epi32lo = _mm256_inserti128_si256(epi32lo, epi32hilo, 1);
            epi32hi = _mm256_inserti128_si256(epi32hi, epi32lohi, 0);
            __m256i epu16 = _mm256_packus_epi32(epi32lo, epi32hi);
            _mm256_storeu_si256((void*)u16v, epu16); u16v += 16;
        } while (--nw);
    }
    if (n) {
        aymo_convert_x86_sse41_f32_u16_k(n, f32v, u16v, scale);
    }
}


AYMO_CXX_EXTERN_C_END

#endif  // AYMO_CPU_SUPPORT_X86_AVX2
