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
#ifndef _include_aymo_cpu_arm_neon_inline_h
#define _include_aymo_cpu_arm_neon_inline_h

#include "aymo_cpu.h"
#ifdef AYMO_CPU_SUPPORT_ARM_NEON

#include <stdint.h>
#ifdef _MSC_VER
    #include <armintr.h>
#endif

AYMO_CXX_EXTERN_C_BEGIN


// Generic CPU shorthands

#if defined(_MSC_VER)
    #define AYMO_ARM_DSB(n)     (__dsb(n))
    #define AYMO_ARM_DSB_ST()   (AYMO_ARM_DSB((unsigned)_ARM_BARRIER_ST))
#elif (defined(__GNUC__) || defined(__clang__))
    #define AYMO_ARM_DSB_ST()   {asm volatile ("dsb st");}
#endif

#define vsfence         AYMO_ARM_DSB_ST


// SIMD type shorthands
typedef vi16x8_t vi16_t;
typedef vu16x8_t vu16_t;
typedef vi32x4_t vi32_t;


// v*() methods are for vi16_t = int16_t[8]

#define vi2u            vreinterpretq_u16_s16
#define vu2i            vreinterpretq_s16_u16

#define vcreate         vcreate_s16
#define vload           vld1q_s16
#define vstore          vst1q_s16

#define vsetx           vsetz
#define vset1           vdupq_n_s16
#define vseta           vseta_s16
#define vsetr           vsetr_s16
#define vsetz()         (vset1(0))
#define vsetf()         (vset1(-1))
#define vsetm           vsetm_s16

#define vnot            vmvnq_s16
#define vand            vandq_s16
#define vor             vorrq_s16
#define vxor            veorq_s16
#define vandnot(a,b)    (vbicq_s16((b), (a)))  // ~A & B
#define vblendv(a,b,m)  (vbslq_s16(vi2u(m), (b), (a)))  // B if M else A

#define vcmpeq(a, b)    (vu2i(vceqq_s16((a), (b))))
#define vcmpgt(a, b)    (vu2i(vcgtq_s16((a), (b))))
#define vcmpz(x)        (vcmpeq((x), vsetz()))
#define vcmpp(x)        (vcmpgt((x), vsetz()))
#define vcmpn(x)        (vcmpgt(vsetz(), (x)))

#define vadd            vaddq_s16
#define vaddsi          vqaddq_s16
#define vaddsu          vqaddq_u16

#define vsub            vsubq_s16
#define vsubsi          vqsubq_s16
#define vsubsu          vqsubq_u16
#define vneg            vqnegq_s16

#define vslli           vshlq_n_s16
#define vsrli(x,n)      (vu2i(vshrq_n_u16(vi2u(x), (n))))
#define vsrai           vshrq_n_s16
#define vsllv           vshlq_s16
#define vsrlv(a,b)      (vu2i(vshlq_u16(vi2u(a), vnegq_s16(b))))
#define vsrav(a,b)      (vshlq_s16((a), vnegq_s16(b)))

#define vmulhrs         vqrdmulhq_s16

#define vmullo          vmulq_s16

#define vmini           vminq_s16
#define vminu           vminq_u16

#define vmaxi           vmaxq_s16
#define vmaxu           vmaxq_u16

#define vextract        vgetq_lane_s16
#define vextractn       vextractn_s16
#define vextractv(x,i)  (((const int16_t*)(const void*)&(x))[(i)])

#define vinsert(x,n,i)  (vsetq_lane_s16((n), (x), (i)))
#define vinsertn        vinsertn_s16
#define vinsertv(x,n,i) {((int16_t*)(void*)&(x))[(i)] = (n);}

#define vgather         vgather_s16

#define vhsum           vhsum_s16
#define vhsums          vhsum

#define vpow2m1lt4      vpow2m1lt4_s16
#define vpow2lt4        vpow2lt4_s16

#define vgetlo          vget_low_s16
#define vgethi          vget_high_s16
#define vswap(x)        (vcombine(vgethi(x), vgetlo(x)))

#define vrev            vrev64q_s16
#define vrevv(x)        (vvcastv(vrev64q_s32(vcastvv(x))))
#define vext            vextq_s16

#define vcombine        vcombine_s16

#define vunpack         vmovl_s16
#define vunpacklo(x)    (vunpack(vgetlo(x)))
#define vunpackhi(x)    (vunpack(vgethi(x)))

#define v2vv            vunpack
#define vlo2vv          vunpacklo
#define vhi2vv          vunpackhi

#define vcastvv         vreinterpretq_s32_s16


// w*() methods are for widening/narrowing vi16_t = int16_t[8] <--> vi32_t = int32_t[4]

#define wmullo          vmull_s16

#define wcombine        vcombine_s16


// vv*() methods are for vi32_t = int32_t[4]

#define vvi2u           vreinterpretq_u32_s32
#define vvu2i           vreinterpretq_s32_u32

#define vvsetx          vvsetz
#define vvset1          vdupq_n_s32
#define vvsetz()        (vvset1(0))
#define vvsetf()        (vvset1(-1))

#define vvand           vandq_s32
#define vvor            vorrq_s32
#define vvxor           veorq_s32
#define vvandnot(a,b)   (vbicq_s32((b), (a)))  // ~A & B

#define vvadd           vaddq_s32
#define vwadd           vaddw_s32

#define vvsrli(x,n)     (vvu2i(vshrq_n_u32(vvi2u(x), (n))))

#define vvsllv          vshlq_s32

#define vvmullo         vmulq_s32

#define vvextract       vgetq_lane_s32
#define vvextractn      vvextractn_s32

#define vvinsert(x,n,i) (vsetq_lane_s32((n), (x), (i)))
#define vvinsertn       vvinsertn_s32

#define vvgetlo         vget_low_s32
#define vvgethi         vget_high_s32
#define vvswap(x)       (vvcombine(vvgethi(x), vvgetlo(x)))

#define vvrev           vrev64q_s32
#define vvext           vextq_s32

#define vvcombine       vcombine_s32

#define vvpack(a,b)     (vcombine_s16(vmovn_s32(a), vmovn_s32(b)))
#define vvpacks(a,b)    (vcombine_s16(vqmovn_s32(a), vqmovn_s32(b)))

#define vvcastv         vreinterpretq_s16_s32


static inline
int16x8_t vseta_s16(
    int16_t i7,
    int16_t i6,
    int16_t i5,
    int16_t i4,
    int16_t i3,
    int16_t i2,
    int16_t i1,
    int16_t i0
)
{
    int16_t r_n128_i16[8] = { i0, i1, i2, i3, i4, i5, i6, i7 };
    return vld1q_s16(r_n128_i16);
}


static inline
int16x8_t vsetr_s16(
    int16_t i7,
    int16_t i6,
    int16_t i5,
    int16_t i4,
    int16_t i3,
    int16_t i2,
    int16_t i1,
    int16_t i0
)
{
    int16_t r_n128_i16[8] = { i7, i6, i5, i4, i3, i2, i1, i0 };
    return vld1q_s16(r_n128_i16);
}


static inline
int16x8_t vsetm_s16(uint8_t m)
{
    static const int16_t kk[8] = { 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80 };
    int16x8_t k = vld1q_s16(kk);
    return vcmpeq(vand(vset1((int16_t)(uint16_t)m), k), k);
}


static inline
short vextractn_s16(int16x8_t x, const int i)
{
    int16_t x_n128_i16[8];
    vst1q_s16(x_n128_i16, x);
    return x_n128_i16[i];
}


static inline
int16x8_t vinsertn_s16(int16x8_t x, short n, const int i)
{
    int16_t x_n128_i16[8];
    vst1q_s16(x_n128_i16, x);
    x_n128_i16[i] = n;
    return vld1q_s16(x_n128_i16);
}


// Gathers 16x 16-bit words via 16x 8-bit (low) indexes
static inline
int16x8_t vgather_s16(const int16_t* v, int16x8_t i)
{
    // Plain C lookup, smallest cache footprint
    uint8_t i_n128_u8[16];
    vst1q_s16((void*)i_n128_u8, i);
    int16_t* r_n128_i16 = (int16_t*)(void*)i_n128_u8;
    r_n128_i16[0] = v[i_n128_u8[0x0]];
    r_n128_i16[1] = v[i_n128_u8[0x2]];
    r_n128_i16[2] = v[i_n128_u8[0x4]];
    r_n128_i16[3] = v[i_n128_u8[0x6]];
    r_n128_i16[4] = v[i_n128_u8[0x8]];
    r_n128_i16[5] = v[i_n128_u8[0xA]];
    r_n128_i16[6] = v[i_n128_u8[0xC]];
    r_n128_i16[7] = v[i_n128_u8[0xE]];
    return vld1q_s16(r_n128_i16);
}


static inline
int32_t vhsum_s16(int16x8_t x)
{
    int32x4_t sum16 = vpaddlq_s16(x);
    int64x2_t sum32 = vpaddlq_s32(sum16);
    int32x2_t lo32 = vreinterpret_s32_s64(vget_low_s64(sum32));
    int32x2_t hi32 = vreinterpret_s32_s64(vget_high_s64(sum32));
    return (vget_lane_s32(lo32, 0) + vget_lane_s32(hi32, 0));
}


// 0 <= x < 4  -->  (1 << (x - 1))  -->  0, 1, 2, 4
static inline
int16x8_t vpow2m1lt4_s16(int16x8_t x)
{
    return vsub(x, vcmpgt(x, vset1(2)));
}


// 0 <= x < 4  -->  (1 << x)
static inline
int16x8_t vpow2lt4_s16(int16x8_t x)
{
    return vsllv(vset1(1), x);
}


static inline
int32_t vvextractn_s32(int32x4_t x, const int i)
{
    int32_t x_n128_i32[4];
    vst1q_s32(x_n128_i32, x);
    return x_n128_i32[i];
}


static inline
int32x4_t vvinsertn_s32(int32x4_t x, int32_t n, const int i)
{
    int32_t x_n128_i32[4];
    vst1q_s32(x_n128_i32, x);
    x_n128_i32[i] = n;
    return vld1q_s32(x_n128_i32);
}


static inline
int16_t clamp16(int x)
{
    if (x < INT16_MIN) {
        return (int16_t)INT16_MIN;
    }
    if (x >= INT16_MAX) {
        return (int16_t)INT16_MAX;
    }
    return (int16_t)x;
}


// Finds first set bit = Counts trailing zeros
// Emulates the BSD function
static inline
int uffsll(unsigned long long x)
{
#if defined(_MSC_VER)
    unsigned long i = 0;
#if defined(_WIN32)
    if (_BitScanForward(&i, (uint32_t)x)) {
        return (int)(i + 1);
    }
    if (_BitScanForward(&i, (uint32_t)(x >> 32))) {
        return (int)(i + 33);
    }
#else
    if (_BitScanForward64(&i, (unsigned long long)x)) {
        return (int)(i + 1);
    }
#endif
    return 0;

#elif (defined(__GNUC__) || defined(__clang__))
    return __builtin_ffsll((long long)x);

#else
    if (x) {
        int i = 0;
        do {
            ++i;
            x <<= 1;
        } while(x);
        return (64 - i);
    }
    return 0;
#endif
}


AYMO_CXX_EXTERN_C_END

#endif  // AYMO_CPU_SUPPORT_ARM_NEON

#endif  // _include_aymo_cpu_arm_neon_inline_h
