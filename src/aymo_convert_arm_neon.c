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
#ifdef AYMO_CPU_SUPPORT_ARM_NEON

#define AYMO_KEEP_SHORTHANDS
#include "aymo_convert_arm_neon.h"

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
            int16x8_t s16 = vld1q_s16(i16v); i16v += 8;
            int32x4_t s32lo = vmovl_s16(vget_low_s16(s16));
            int32x4_t s32hi = vmovl_s16(vget_high_s16(s16));
            float32x4_t f32lo = vcvtq_f32_s32(s32lo);
            float32x4_t f32hi = vcvtq_f32_s32(s32hi);
            vst1q_f32(f32v, f32lo); f32v += 4;
            vst1q_f32(f32v, f32hi); f32v += 4;
        } while (--nw);
        n %= 8;
        if (n == 0) {
            return;
        }
    }
    if (n >= 4) {
        int16x4_t s16 = vld1_s16(i16v); i16v += 4;
        int32x4_t s32lo = vmovl_s16(s16);
        float32x4_t f32lo = vcvtq_f32_s32(s32lo);
        vst1q_f32(f32v, f32lo); f32v += 4;
        n %= 4;
        if (n == 0) {
            return;
        }
    }
    switch (n) {
        case 3: {
            int32_t i32t[4] = { i16v[0], i16v[1], i16v[2], 0 };
            int32x4_t s32lo = vld1q_s32(i32t);
            float32x4_t f32lo = vcvtq_f32_s32(s32lo);
            f32v[0] = vgetq_lane_f32(f32lo, 0);
            f32v[1] = vgetq_lane_f32(f32lo, 1);
            f32v[2] = vgetq_lane_f32(f32lo, 2);
            break;
        }
        case 2: {
            int32_t i32t[4] = { i16v[0], i16v[1], 0, 0 };
            int32x4_t s32lo = vld1q_s32(i32t);
            float32x4_t f32lo = vcvtq_f32_s32(s32lo);
            f32v[0] = vgetq_lane_f32(f32lo, 0);
            f32v[1] = vgetq_lane_f32(f32lo, 1);
            break;
        }
        case 1: {
            int32_t i32t[4] = { i16v[0], 0, 0, 0 };
            int32x4_t s32lo = vld1q_s32(i32t);
            float32x4_t f32lo = vcvtq_f32_s32(s32lo);
            f32v[0] = vgetq_lane_f32(f32lo, 0);
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
            float32x4_t f32lo = vld1q_f32(f32v); f32v += 4;
            float32x4_t f32hi = vld1q_f32(f32v); f32v += 4;
            int32x4_t s32lo = vcvtq_s32_f32(f32lo);
            int32x4_t s32hi = vcvtq_s32_f32(f32hi);
            int16x4_t s16lo = vqmovn_s32(s32lo);
            int16x4_t s16hi = vqmovn_s32(s32hi);
            vst1q_s16(i16v, vcombine_s16(s16lo, s16hi)); i16v += 8;
        } while (--nw);
        n %= 8;
        if (n == 0) {
            return;
        }
    }
    if (n >= 4) {
        float32x4_t f32lo = vld1q_f32(f32v); f32v += 4;
        int32x4_t s32lo = vcvtq_s32_f32(f32lo);
        int16x4_t s16lo = vqmovn_s32(s32lo);
        vst1_s16(i16v, s16lo); i16v += 4;
        n %= 4;
        if (n == 0) {
            return;
        }
    }
    switch (n) {
        case 3: {
            float f32t[4] = { f32v[0], f32v[1], f32v[2], .0f };
            float32x4_t f32lo = vld1q_f32(f32t);
            int32x4_t s32lo = vcvtq_s32_f32(f32lo);
            int16x4_t s16lo = vqmovn_s32(s32lo);
            i16v[0] = vget_lane_s16(s16lo, 0);
            i16v[1] = vget_lane_s16(s16lo, 1);
            i16v[2] = vget_lane_s16(s16lo, 2);
            break;
        }
        case 2: {
            float f32t[4] = { f32v[0], f32v[1], .0f, .0f };
            float32x4_t f32lo = vld1q_f32(f32t);
            int32x4_t s32lo = vcvtq_s32_f32(f32lo);
            int16x4_t s16lo = vqmovn_s32(s32lo);
            i16v[0] = vget_lane_s16(s16lo, 0);
            i16v[1] = vget_lane_s16(s16lo, 1);
            break;
        }
        case 1: {
            float f32t[4] = { f32v[0], .0f, .0f, .0f };
            float32x4_t f32lo = vld1q_f32(f32t);
            int32x4_t s32lo = vcvtq_s32_f32(f32lo);
            int16x4_t s16lo = vqmovn_s32(s32lo);
            i16v[0] = vget_lane_s16(s16lo, 0);
            break;
        }
        default: break;
    }
}


void aymo_(i16_f32_1)(size_t n, const int16_t i16v[], float f32v[])
{
    const float scale = (float)(1. / 32768.);
    float32x4_t psk = vdupq_n_f32(scale);
    if (n >= 8) {
        size_t nw = (n / 8);
        do {
            int16x8_t s16 = vld1q_s16(i16v); i16v += 8;
            int32x4_t s32lo = vmovl_s16(vget_low_s16(s16));
            int32x4_t s32hi = vmovl_s16(vget_high_s16(s16));
            float32x4_t f32lo = vcvtq_f32_s32(s32lo);
            float32x4_t f32hi = vcvtq_f32_s32(s32hi);
            f32lo = vmulq_f32(f32lo, psk);
            f32hi = vmulq_f32(f32hi, psk);
            vst1q_f32(f32v, f32lo); f32v += 4;
            vst1q_f32(f32v, f32hi); f32v += 4;
        } while (--nw);
        n %= 8;
        if (n == 0) {
            return;
        }
    }
    if (n >= 4) {
        int16x4_t s16 = vld1_s16(i16v); i16v += 4;
        int32x4_t s32lo = vmovl_s16(s16);
        float32x4_t f32lo = vcvtq_f32_s32(s32lo);
        f32lo = vmulq_f32(f32lo, psk);
        vst1q_f32(f32v, f32lo); f32v += 4;
        n %= 4;
        if (n == 0) {
            return;
        }
    }
    switch (n) {
    case 3: {
        int32_t i32t[4] = { i16v[0], i16v[1], i16v[2], 0 };
        int32x4_t s32lo = vld1q_s32(i32t);
        float32x4_t f32lo = vcvtq_f32_s32(s32lo);
        f32lo = vmulq_f32(f32lo, psk);
        f32v[0] = vgetq_lane_f32(f32lo, 0);
        f32v[1] = vgetq_lane_f32(f32lo, 1);
        f32v[2] = vgetq_lane_f32(f32lo, 2);
        break;
    }
    case 2: {
        int32_t i32t[4] = { i16v[0], i16v[1], 0, 0 };
        int32x4_t s32lo = vld1q_s32(i32t);
        float32x4_t f32lo = vcvtq_f32_s32(s32lo);
        f32lo = vmulq_f32(f32lo, psk);
        f32v[0] = vgetq_lane_f32(f32lo, 0);
        f32v[1] = vgetq_lane_f32(f32lo, 1);
        break;
    }
    case 1: {
        int32_t i32t[4] = { i16v[0], 0, 0, 0 };
        int32x4_t s32lo = vld1q_s32(i32t);
        float32x4_t f32lo = vcvtq_f32_s32(s32lo);
        f32lo = vmulq_f32(f32lo, psk);
        f32v[0] = vgetq_lane_f32(f32lo, 0);
        break;
    }
    default: break;
    }
}


void aymo_(f32_i16_1)(size_t n, const float f32v[], int16_t i16v[])
{
    const float scale = (float)(32768.);
    float32x4_t psk = vdupq_n_f32(scale);
    if (n >= 8) {
        size_t nw = (n / 8);
        do {
            float32x4_t f32lo = vld1q_f32(f32v); f32v += 4;
            float32x4_t f32hi = vld1q_f32(f32v); f32v += 4;
            f32lo = vmulq_f32(f32lo, psk);
            f32hi = vmulq_f32(f32hi, psk);
            int32x4_t s32lo = vcvtq_s32_f32(f32lo);
            int32x4_t s32hi = vcvtq_s32_f32(f32hi);
            int16x4_t s16lo = vqmovn_s32(s32lo);
            int16x4_t s16hi = vqmovn_s32(s32hi);
            vst1q_s16(i16v, vcombine_s16(s16lo, s16hi)); i16v += 8;
        } while (--nw);
        n %= 8;
        if (n == 0) {
            return;
        }
    }
    if (n >= 4) {
        float32x4_t f32lo = vld1q_f32(f32v); f32v += 4;
        f32lo = vmulq_f32(f32lo, psk);
        int32x4_t s32lo = vcvtq_s32_f32(f32lo);
        int16x4_t s16lo = vqmovn_s32(s32lo);
        vst1_s16(i16v, s16lo); i16v += 4;
        n %= 4;
        if (n == 0) {
            return;
        }
    }
    switch (n) {
    case 3: {
        float f32t[4] = { f32v[0], f32v[1], f32v[2], .0f };
        float32x4_t f32lo = vld1q_f32(f32t);
        f32lo = vmulq_f32(f32lo, psk);
        int32x4_t s32lo = vcvtq_s32_f32(f32lo);
        int16x4_t s16lo = vqmovn_s32(s32lo);
        i16v[0] = vget_lane_s16(s16lo, 0);
        i16v[1] = vget_lane_s16(s16lo, 1);
        i16v[2] = vget_lane_s16(s16lo, 2);
        break;
    }
    case 2: {
        float f32t[4] = { f32v[0], f32v[1], .0f, .0f };
        float32x4_t f32lo = vld1q_f32(f32t);
        f32lo = vmulq_f32(f32lo, psk);
        int32x4_t s32lo = vcvtq_s32_f32(f32lo);
        int16x4_t s16lo = vqmovn_s32(s32lo);
        i16v[0] = vget_lane_s16(s16lo, 0);
        i16v[1] = vget_lane_s16(s16lo, 1);
        break;
    }
    case 1: {
        float f32t[4] = { f32v[0], .0f, .0f, .0f };
        float32x4_t f32lo = vld1q_f32(f32t);
        f32lo = vmulq_f32(f32lo, psk);
        int32x4_t s32lo = vcvtq_s32_f32(f32lo);
        int16x4_t s16lo = vqmovn_s32(s32lo);
        i16v[0] = vget_lane_s16(s16lo, 0);
        break;
    }
    default: break;
    }
}


void aymo_(i16_f32_k)(size_t n, const int16_t i16v[], float f32v[], float scale)
{
    float32x4_t psk = vdupq_n_f32(scale);
    if (n >= 8) {
        size_t nw = (n / 8);
        do {
            int16x8_t s16 = vld1q_s16(i16v); i16v += 8;
            int32x4_t s32lo = vmovl_s16(vget_low_s16(s16));
            int32x4_t s32hi = vmovl_s16(vget_high_s16(s16));
            float32x4_t f32lo = vcvtq_f32_s32(s32lo);
            float32x4_t f32hi = vcvtq_f32_s32(s32hi);
            f32lo = vmulq_f32(f32lo, psk);
            f32hi = vmulq_f32(f32hi, psk);
            vst1q_f32(f32v, f32lo); f32v += 4;
            vst1q_f32(f32v, f32hi); f32v += 4;
        } while (--nw);
        n %= 8;
        if (n == 0) {
            return;
        }
    }
    if (n >= 4) {
        int16x4_t s16 = vld1_s16(i16v); i16v += 4;
        int32x4_t s32lo = vmovl_s16(s16);
        float32x4_t f32lo = vcvtq_f32_s32(s32lo);
        f32lo = vmulq_f32(f32lo, psk);
        vst1q_f32(f32v, f32lo); f32v += 4;
        n %= 4;
        if (n == 0) {
            return;
        }
    }
    switch (n) {
    case 3: {
        int32_t i32t[4] = { i16v[0], i16v[1], i16v[2], 0 };
        int32x4_t s32lo = vld1q_s32(i32t);
        float32x4_t f32lo = vcvtq_f32_s32(s32lo);
        f32lo = vmulq_f32(f32lo, psk);
        f32v[0] = vgetq_lane_f32(f32lo, 0);
        f32v[1] = vgetq_lane_f32(f32lo, 1);
        f32v[2] = vgetq_lane_f32(f32lo, 2);
        break;
    }
    case 2: {
        int32_t i32t[4] = { i16v[0], i16v[1], 0, 0 };
        int32x4_t s32lo = vld1q_s32(i32t);
        float32x4_t f32lo = vcvtq_f32_s32(s32lo);
        f32lo = vmulq_f32(f32lo, psk);
        f32v[0] = vgetq_lane_f32(f32lo, 0);
        f32v[1] = vgetq_lane_f32(f32lo, 1);
        break;
    }
    case 1: {
        int32_t i32t[4] = { i16v[0], 0, 0, 0 };
        int32x4_t s32lo = vld1q_s32(i32t);
        float32x4_t f32lo = vcvtq_f32_s32(s32lo);
        f32lo = vmulq_f32(f32lo, psk);
        f32v[0] = vgetq_lane_f32(f32lo, 0);
        break;
    }
    default: break;
    }
}


void aymo_(f32_i16_k)(size_t n, const float f32v[], int16_t i16v[], float scale)
{
    float32x4_t psk = vdupq_n_f32(scale);
    if (n >= 8) {
        size_t nw = (n / 8);
        do {
            float32x4_t f32lo = vld1q_f32(f32v); f32v += 4;
            float32x4_t f32hi = vld1q_f32(f32v); f32v += 4;
            f32lo = vmulq_f32(f32lo, psk);
            f32hi = vmulq_f32(f32hi, psk);
            int32x4_t s32lo = vcvtq_s32_f32(f32lo);
            int32x4_t s32hi = vcvtq_s32_f32(f32hi);
            int16x4_t s16lo = vqmovn_s32(s32lo);
            int16x4_t s16hi = vqmovn_s32(s32hi);
            vst1q_s16(i16v, vcombine_s16(s16lo, s16hi)); i16v += 8;
        } while (--nw);
        n %= 8;
        if (n == 0) {
            return;
        }
    }
    if (n >= 4) {
        float32x4_t f32lo = vld1q_f32(f32v); f32v += 4;
        f32lo = vmulq_f32(f32lo, psk);
        int32x4_t s32lo = vcvtq_s32_f32(f32lo);
        int16x4_t s16lo = vqmovn_s32(s32lo);
        vst1_s16(i16v, s16lo); i16v += 4;
        n %= 4;
        if (n == 0) {
            return;
        }
    }
    switch (n) {
    case 3: {
        float f32t[4] = { f32v[0], f32v[1], f32v[2], .0f };
        float32x4_t f32lo = vld1q_f32(f32t);
        f32lo = vmulq_f32(f32lo, psk);
        int32x4_t s32lo = vcvtq_s32_f32(f32lo);
        int16x4_t s16lo = vqmovn_s32(s32lo);
        i16v[0] = vget_lane_s16(s16lo, 0);
        i16v[1] = vget_lane_s16(s16lo, 1);
        i16v[2] = vget_lane_s16(s16lo, 2);
        break;
    }
    case 2: {
        float f32t[4] = { f32v[0], f32v[1], .0f, .0f };
        float32x4_t f32lo = vld1q_f32(f32t);
        f32lo = vmulq_f32(f32lo, psk);
        int32x4_t s32lo = vcvtq_s32_f32(f32lo);
        int16x4_t s16lo = vqmovn_s32(s32lo);
        i16v[0] = vget_lane_s16(s16lo, 0);
        i16v[1] = vget_lane_s16(s16lo, 1);
        break;
    }
    case 1: {
        float f32t[4] = { f32v[0], .0f, .0f, .0f };
        float32x4_t f32lo = vld1q_f32(f32t);
        f32lo = vmulq_f32(f32lo, psk);
        int32x4_t s32lo = vcvtq_s32_f32(f32lo);
        int16x4_t s16lo = vqmovn_s32(s32lo);
        i16v[0] = vget_lane_s16(s16lo, 0);
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
            uint16x8_t u16 = vld1q_u16(u16v); u16v += 8;
            uint32x4_t u32lo = vmovl_u16(vget_low_u16(u16));
            uint32x4_t u32hi = vmovl_u16(vget_high_u16(u16));
            float32x4_t f32lo = vcvtq_f32_u32(u32lo);
            float32x4_t f32hi = vcvtq_f32_u32(u32hi);
            vst1q_f32(f32v, f32lo); f32v += 4;
            vst1q_f32(f32v, f32hi); f32v += 4;
        } while (--nw);
        n %= 8;
        if (n == 0) {
            return;
        }
    }
    if (n >= 4) {
        uint16x4_t u16 = vld1_u16(u16v); u16v += 4;
        uint32x4_t u32lo = vmovl_u16(u16);
        float32x4_t f32lo = vcvtq_f32_u32(u32lo);
        vst1q_f32(f32v, f32lo); f32v += 4;
        n %= 4;
        if (n == 0) {
            return;
        }
    }
    switch (n) {
    case 3: {
        uint32_t u32t[4] = { u16v[0], u16v[1], u16v[2], 0 };
        uint32x4_t u32lo = vld1q_u32(u32t);
        float32x4_t f32lo = vcvtq_f32_u32(u32lo);
        f32v[0] = vgetq_lane_f32(f32lo, 0);
        f32v[1] = vgetq_lane_f32(f32lo, 1);
        f32v[2] = vgetq_lane_f32(f32lo, 2);
        break;
    }
    case 2: {
        uint32_t u32t[4] = { u16v[0], u16v[1], 0, 0 };
        uint32x4_t u32lo = vld1q_u32(u32t);
        float32x4_t f32lo = vcvtq_f32_u32(u32lo);
        f32v[0] = vgetq_lane_f32(f32lo, 0);
        f32v[1] = vgetq_lane_f32(f32lo, 1);
        break;
    }
    case 1: {
        uint32_t u32t[4] = { u16v[0], 0, 0, 0 };
        uint32x4_t u32lo = vld1q_u32(u32t);
        float32x4_t f32lo = vcvtq_f32_u32(u32lo);
        f32v[0] = vgetq_lane_f32(f32lo, 0);
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
            float32x4_t f32lo = vld1q_f32(f32v); f32v += 4;
            float32x4_t f32hi = vld1q_f32(f32v); f32v += 4;
            uint32x4_t u32lo = vcvtq_u32_f32(f32lo);
            uint32x4_t u32hi = vcvtq_u32_f32(f32hi);
            uint16x4_t u16lo = vqmovn_u32(u32lo);
            uint16x4_t u16hi = vqmovn_u32(u32hi);
            vst1q_u16(u16v, vcombine_u16(u16lo, u16hi)); u16v += 8;
        } while (--nw);
        n %= 8;
        if (n == 0) {
            return;
        }
    }
    if (n >= 4) {
        float32x4_t f32lo = vld1q_f32(f32v); f32v += 4;
        uint32x4_t u32lo = vcvtq_u32_f32(f32lo);
        uint16x4_t u16lo = vqmovn_u32(u32lo);
        vst1_u16(u16v, u16lo); u16v += 4;
        n %= 4;
        if (n == 0) {
            return;
        }
    }
    switch (n) {
    case 3: {
        float f32t[4] = { f32v[0], f32v[1], f32v[2], .0f };
        float32x4_t f32lo = vld1q_f32(f32t);
        uint32x4_t u32lo = vcvtq_u32_f32(f32lo);
        uint16x4_t u16lo = vqmovn_u32(u32lo);
        u16v[0] = vget_lane_u16(u16lo, 0);
        u16v[1] = vget_lane_u16(u16lo, 1);
        u16v[2] = vget_lane_u16(u16lo, 2);
        break;
    }
    case 2: {
        float f32t[4] = { f32v[0], f32v[1], .0f, .0f };
        float32x4_t f32lo = vld1q_f32(f32t);
        uint32x4_t u32lo = vcvtq_u32_f32(f32lo);
        uint16x4_t u16lo = vqmovn_u32(u32lo);
        u16v[0] = vget_lane_u16(u16lo, 0);
        u16v[1] = vget_lane_u16(u16lo, 1);
        break;
    }
    case 1: {
        float f32t[4] = { f32v[0], .0f, .0f, .0f };
        float32x4_t f32lo = vld1q_f32(f32t);
        uint32x4_t u32lo = vcvtq_u32_f32(f32lo);
        uint16x4_t u16lo = vqmovn_u32(u32lo);
        u16v[0] = vget_lane_u16(u16lo, 0);
        break;
    }
    default: break;
    }
}


void aymo_(u16_f32_1)(size_t n, const uint16_t u16v[], float f32v[])
{
    const float scale = (float)(1. / 32768.);
    float32x4_t psk = vdupq_n_f32(scale);
    if (n >= 8) {
        size_t nw = (n / 8);
        do {
            uint16x8_t u16 = vld1q_u16(u16v); u16v += 8;
            uint32x4_t u32lo = vmovl_u16(vget_low_u16(u16));
            uint32x4_t u32hi = vmovl_u16(vget_high_u16(u16));
            float32x4_t f32lo = vcvtq_f32_u32(u32lo);
            float32x4_t f32hi = vcvtq_f32_u32(u32hi);
            f32lo = vmulq_f32(f32lo, psk);
            f32hi = vmulq_f32(f32hi, psk);
            vst1q_f32(f32v, f32lo); f32v += 4;
            vst1q_f32(f32v, f32hi); f32v += 4;
        } while (--nw);
        n %= 8;
        if (n == 0) {
            return;
        }
    }
    if (n >= 4) {
        uint16x4_t u16 = vld1_u16(u16v); u16v += 4;
        uint32x4_t u32lo = vmovl_u16(u16);
        float32x4_t f32lo = vcvtq_f32_u32(u32lo);
        f32lo = vmulq_f32(f32lo, psk);
        vst1q_f32(f32v, f32lo); f32v += 4;
        n %= 4;
        if (n == 0) {
            return;
        }
    }
    switch (n) {
    case 3: {
        uint32_t i32t[4] = { u16v[0], u16v[1], u16v[2], 0 };
        uint32x4_t u32lo = vld1q_u32(i32t);
        float32x4_t f32lo = vcvtq_f32_u32(u32lo);
        f32lo = vmulq_f32(f32lo, psk);
        f32v[0] = vgetq_lane_f32(f32lo, 0);
        f32v[1] = vgetq_lane_f32(f32lo, 1);
        f32v[2] = vgetq_lane_f32(f32lo, 2);
        break;
    }
    case 2: {
        uint32_t i32t[4] = { u16v[0], u16v[1], 0, 0 };
        uint32x4_t u32lo = vld1q_u32(i32t);
        float32x4_t f32lo = vcvtq_f32_u32(u32lo);
        f32lo = vmulq_f32(f32lo, psk);
        f32v[0] = vgetq_lane_f32(f32lo, 0);
        f32v[1] = vgetq_lane_f32(f32lo, 1);
        break;
    }
    case 1: {
        uint32_t i32t[4] = { u16v[0], 0, 0, 0 };
        uint32x4_t u32lo = vld1q_u32(i32t);
        float32x4_t f32lo = vcvtq_f32_u32(u32lo);
        f32lo = vmulq_f32(f32lo, psk);
        f32v[0] = vgetq_lane_f32(f32lo, 0);
        break;
    }
    default: break;
    }
}


void aymo_(f32_u16_1)(size_t n, const float f32v[], uint16_t u16v[])
{
    const float scale = (float)(32768.);
    float32x4_t psk = vdupq_n_f32(scale);
    if (n >= 8) {
        size_t nw = (n / 8);
        do {
            float32x4_t f32lo = vld1q_f32(f32v); f32v += 4;
            float32x4_t f32hi = vld1q_f32(f32v); f32v += 4;
            f32lo = vmulq_f32(f32lo, psk);
            f32hi = vmulq_f32(f32hi, psk);
            uint32x4_t u32lo = vcvtq_u32_f32(f32lo);
            uint32x4_t u32hi = vcvtq_u32_f32(f32hi);
            uint16x4_t u16lo = vqmovn_u32(u32lo);
            uint16x4_t u16hi = vqmovn_u32(u32hi);
            vst1q_u16(u16v, vcombine_u16(u16lo, u16hi)); u16v += 8;
        } while (--nw);
        n %= 8;
        if (n == 0) {
            return;
        }
    }
    if (n >= 4) {
        float32x4_t f32lo = vld1q_f32(f32v); f32v += 4;
        f32lo = vmulq_f32(f32lo, psk);
        uint32x4_t u32lo = vcvtq_u32_f32(f32lo);
        uint16x4_t u16lo = vqmovn_u32(u32lo);
        vst1_u16(u16v, u16lo); u16v += 4;
        n %= 4;
        if (n == 0) {
            return;
        }
    }
    switch (n) {
    case 3: {
        float f32t[4] = { f32v[0], f32v[1], f32v[2], .0f };
        float32x4_t f32lo = vld1q_f32(f32t);
        f32lo = vmulq_f32(f32lo, psk);
        uint32x4_t u32lo = vcvtq_u32_f32(f32lo);
        uint16x4_t u16lo = vqmovn_u32(u32lo);
        u16v[0] = vget_lane_u16(u16lo, 0);
        u16v[1] = vget_lane_u16(u16lo, 1);
        u16v[2] = vget_lane_u16(u16lo, 2);
        break;
    }
    case 2: {
        float f32t[4] = { f32v[0], f32v[1], .0f, .0f };
        float32x4_t f32lo = vld1q_f32(f32t);
        f32lo = vmulq_f32(f32lo, psk);
        uint32x4_t u32lo = vcvtq_u32_f32(f32lo);
        uint16x4_t u16lo = vqmovn_u32(u32lo);
        u16v[0] = vget_lane_u16(u16lo, 0);
        u16v[1] = vget_lane_u16(u16lo, 1);
        break;
    }
    case 1: {
        float f32t[4] = { f32v[0], .0f, .0f, .0f };
        float32x4_t f32lo = vld1q_f32(f32t);
        f32lo = vmulq_f32(f32lo, psk);
        uint32x4_t u32lo = vcvtq_u32_f32(f32lo);
        uint16x4_t u16lo = vqmovn_u32(u32lo);
        u16v[0] = vget_lane_u16(u16lo, 0);
        break;
    }
    default: break;
    }
}


void aymo_(u16_f32_k)(size_t n, const uint16_t u16v[], float f32v[], float scale)
{
    float32x4_t psk = vdupq_n_f32(scale);
    if (n >= 8) {
        size_t nw = (n / 8);
        do {
            uint16x8_t u16 = vld1q_u16(u16v); u16v += 8;
            uint32x4_t u32lo = vmovl_u16(vget_low_u16(u16));
            uint32x4_t u32hi = vmovl_u16(vget_high_u16(u16));
            float32x4_t f32lo = vcvtq_f32_u32(u32lo);
            float32x4_t f32hi = vcvtq_f32_u32(u32hi);
            f32lo = vmulq_f32(f32lo, psk);
            f32hi = vmulq_f32(f32hi, psk);
            vst1q_f32(f32v, f32lo); f32v += 4;
            vst1q_f32(f32v, f32hi); f32v += 4;
        } while (--nw);
        n %= 8;
        if (n == 0) {
            return;
        }
    }
    if (n >= 4) {
        uint16x4_t u16 = vld1_u16(u16v); u16v += 4;
        uint32x4_t u32lo = vmovl_u16(u16);
        float32x4_t f32lo = vcvtq_f32_u32(u32lo);
        f32lo = vmulq_f32(f32lo, psk);
        vst1q_f32(f32v, f32lo); f32v += 4;
        n %= 4;
        if (n == 0) {
            return;
        }
    }
    switch (n) {
    case 3: {
        uint32_t i32t[4] = { u16v[0], u16v[1], u16v[2], 0 };
        uint32x4_t u32lo = vld1q_u32(i32t);
        float32x4_t f32lo = vcvtq_f32_u32(u32lo);
        f32lo = vmulq_f32(f32lo, psk);
        f32v[0] = vgetq_lane_f32(f32lo, 0);
        f32v[1] = vgetq_lane_f32(f32lo, 1);
        f32v[2] = vgetq_lane_f32(f32lo, 2);
        break;
    }
    case 2: {
        uint32_t i32t[4] = { u16v[0], u16v[1], 0, 0 };
        uint32x4_t u32lo = vld1q_u32(i32t);
        float32x4_t f32lo = vcvtq_f32_u32(u32lo);
        f32lo = vmulq_f32(f32lo, psk);
        f32v[0] = vgetq_lane_f32(f32lo, 0);
        f32v[1] = vgetq_lane_f32(f32lo, 1);
        break;
    }
    case 1: {
        uint32_t i32t[4] = { u16v[0], 0, 0, 0 };
        uint32x4_t u32lo = vld1q_u32(i32t);
        float32x4_t f32lo = vcvtq_f32_u32(u32lo);
        f32lo = vmulq_f32(f32lo, psk);
        f32v[0] = vgetq_lane_f32(f32lo, 0);
        break;
    }
    default: break;
    }
}


void aymo_(f32_u16_k)(size_t n, const float f32v[], uint16_t u16v[], float scale)
{
    float32x4_t psk = vdupq_n_f32(scale);
    if (n >= 8) {
        size_t nw = (n / 8);
        do {
            float32x4_t f32lo = vld1q_f32(f32v); f32v += 4;
            float32x4_t f32hi = vld1q_f32(f32v); f32v += 4;
            f32lo = vmulq_f32(f32lo, psk);
            f32hi = vmulq_f32(f32hi, psk);
            uint32x4_t u32lo = vcvtq_u32_f32(f32lo);
            uint32x4_t u32hi = vcvtq_u32_f32(f32hi);
            uint16x4_t u16lo = vqmovn_u32(u32lo);
            uint16x4_t u16hi = vqmovn_u32(u32hi);
            vst1q_u16(u16v, vcombine_u16(u16lo, u16hi)); u16v += 8;
        } while (--nw);
        n %= 8;
        if (n == 0) {
            return;
        }
    }
    if (n >= 4) {
        float32x4_t f32lo = vld1q_f32(f32v); f32v += 4;
        f32lo = vmulq_f32(f32lo, psk);
        uint32x4_t u32lo = vcvtq_u32_f32(f32lo);
        uint16x4_t u16lo = vqmovn_u32(u32lo);
        vst1_u16(u16v, u16lo); u16v += 4;
        n %= 4;
        if (n == 0) {
            return;
        }
    }
    switch (n) {
    case 3: {
        float f32t[4] = { f32v[0], f32v[1], f32v[2], .0f };
        float32x4_t f32lo = vld1q_f32(f32t);
        f32lo = vmulq_f32(f32lo, psk);
        uint32x4_t u32lo = vcvtq_u32_f32(f32lo);
        uint16x4_t u16lo = vqmovn_u32(u32lo);
        u16v[0] = vget_lane_u16(u16lo, 0);
        u16v[1] = vget_lane_u16(u16lo, 1);
        u16v[2] = vget_lane_u16(u16lo, 2);
        break;
    }
    case 2: {
        float f32t[4] = { f32v[0], f32v[1], .0f, .0f };
        float32x4_t f32lo = vld1q_f32(f32t);
        f32lo = vmulq_f32(f32lo, psk);
        uint32x4_t u32lo = vcvtq_u32_f32(f32lo);
        uint16x4_t u16lo = vqmovn_u32(u32lo);
        u16v[0] = vget_lane_u16(u16lo, 0);
        u16v[1] = vget_lane_u16(u16lo, 1);
        break;
    }
    case 1: {
        float f32t[4] = { f32v[0], .0f, .0f, .0f };
        float32x4_t f32lo = vld1q_f32(f32t);
        f32lo = vmulq_f32(f32lo, psk);
        uint32x4_t u32lo = vcvtq_u32_f32(f32lo);
        uint16x4_t u16lo = vqmovn_u32(u32lo);
        u16v[0] = vget_lane_u16(u16lo, 0);
        break;
    }
    default: break;
    }
}


AYMO_CXX_EXTERN_C_END

#endif  // AYMO_CPU_SUPPORT_ARM_NEON
