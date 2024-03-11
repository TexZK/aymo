// CPU-specific inline methods for x86 AVX2.
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
#ifndef _include_aymo_cpu_x86_avx2_inline_h
#define _include_aymo_cpu_x86_avx2_inline_h

#include "aymo_cpu.h"
#ifdef AYMO_CPU_SUPPORT_X86_AVX2

#include <stdint.h>

AYMO_CXX_EXTERN_C_BEGIN


#ifndef AYMO_CPU_X86_AVX2_GATHER16_STRATEGY
    #define AYMO_CPU_X86_AVX2_GATHER16_STRATEGY 2
#endif


// Generic CPU shorthands

#define vsfence         _mm_sfence


// SIMD type shorthands
typedef vi16x16_t vi16_t;
typedef vu16x16_t vu16_t;
typedef vi32x8_t vi32_t;


// v*() methods are for vi16_t = int16_t[16]

#define vi2u(x)         x
#define vu2i(x)         x

#define vsetx           _mm256_undefined_si256
#define vset1           _mm256_set1_epi16
#define vseta           _mm256_set_epi16
#define vsetr           _mm256_setr_epi16
#define vsetz           _mm256_setzero_si256
#define vsetf()         (vset1(-1))
#define vsetm            mm256_setm_epi16

#define vnot(x)         (vxor((x), vsetf()))
#define vand            _mm256_and_si256
#define vor             _mm256_or_si256
#define vxor            _mm256_xor_si256
#define vandnot         _mm256_andnot_si256  // ~A & B
#define vblendi         _mm256_blend_epi16
#define vblendv         _mm256_blendv_epi8

#define vcmpeq          _mm256_cmpeq_epi16
#define vcmpgt          _mm256_cmpgt_epi16
#define vcmpz(x)        (vcmpeq((x), vsetz()))
#define vcmpp(x)        (vcmpgt((x), vsetz()))
#define vcmpn(x)        (vcmpgt(vsetz(), (x)))

#define vadd            _mm256_add_epi16
#define vaddsi          _mm256_adds_epi16
#define vaddsu          _mm256_adds_epu16

#define vsub            _mm256_sub_epi16
#define vsubsi          _mm256_subs_epi16
#define vsubsu          _mm256_subs_epu16
#define vneg(x)         (vsub(vsetz(), (x)))

#define vslli           _mm256_slli_epi16
#define vsrli           _mm256_srli_epi16
#define vsrai           _mm256_srai_epi16
#define vsllv            mm256_sllv_epi16
#define vsrlv            mm256_srlv_epi16
#define vsrav            mm256_srav_epi16

#define vmulihi         _mm256_mulhi_epi16
#define vmuluhi         _mm256_mulhi_epu16

#define vmulilo         _mm256_mullo_epi16
#define vmululo         _mm256_mullo_epi16

#define vmini           _mm256_min_epi16
#define vminu           _mm256_min_epu16

#define vmaxi           _mm256_max_epi16
#define vmaxu           _mm256_max_epu16

#define vextract        _mm256_extract_epi16
#define vextractn        mm256_extractn_epi16
#define vextractv(x,i)   (((const int16_t*)(const void*)&(x))[(i)])

#define vinsert         _mm256_insert_epi16
#define vinsertn         mm256_insertn_epi16
#define vinsertv(x,n,i)  {((int16_t*)(void*)&(x))[(i)] = (n);}

#define vgather          mm256_i16gather_epi16lo

#define vhsum            mm256_hsum_epi16
#define vhsums           mm256_hsums_epi16

#define vpow2m1lt4       mm256_pow2m1lt4_epi16
#define vpow2lt4         mm256_pow2lt4_epi16

#define vunpacklo       _mm256_unpacklo_epi16
#define vunpackhi       _mm256_unpackhi_epi16


// vv*() methods are for vi32_t = int32_t[8]

#define vvi2u(x)        x
#define vvu2i(x)        x

#define vvsetx          _mm256_undefined_si256
#define vvset1          _mm256_set1_epi32
#define vvseta          _mm256_set_epi32
#define vvsetr          _mm256_setr_epi32
#define vvsetz          _mm256_setzero_si256
#define vvsetf()        (vvset1(-1))

#define vvand           vand
#define vvor            vor
#define vvxor           vxor
#define vvandnot        vandnot

#define vvadd           _mm256_add_epi32

#define vvsrli          _mm256_srli_epi32

#define vvsllv          _mm256_sllv_epi32

#define vvextract       _mm256_extract_epi32
#define vvextractn       mm256_extractn_epi32

#define vvinsert        _mm256_insert_epi32
#define vvinsertn        mm256_insertn_epi32

#define vvmullo         _mm256_mullo_epi32

#define vvpackus        _mm256_packus_epi32


static inline
__m256i mm256_setm_epi16(uint16_t m)
{
    const __m256i k = vsetr(
        0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,
        0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, -0x8000
    );
    return vcmpeq(vand(vset1((int16_t)m), k), k);
}


// see: https://stackoverflow.com/questions/51789685/reproduce-mm256-sllv-epi16-and-mm256-sllv-epi8-in-avx2/51805592#51805592
static inline
__m256i mm256_sllv_epi16(__m256i x, __m256i n)
{
    const __m256i m = _mm256_set1_epi32(0xFFFF0000);
    __m256i lo = _mm256_sllv_epi32(x, _mm256_andnot_si256(m, n));
    __m256i hi = _mm256_sllv_epi32(
        _mm256_and_si256(m, x),
        _mm256_srli_epi32(n, 16)
    );
    return _mm256_blend_epi16(lo, hi, 0xAA);
}


// see: https://stackoverflow.com/questions/51789685/reproduce-mm256-sllv-epi16-and-mm256-sllv-epi8-in-avx2/51805592#51805592
static inline
__m256i mm256_srlv_epi16(__m256i x, __m256i n)
{
    const __m256i m = _mm256_set1_epi32(0x0000FFFF);
    __m256i lo = _mm256_srlv_epi32(
        _mm256_and_si256(m, x),
        _mm256_and_si256(m, n)
    );
    __m256i hi = _mm256_srlv_epi32(x, _mm256_srli_epi32(n, 16));
    return _mm256_blend_epi16(lo, hi, 0xAA);
}


// see: https://stackoverflow.com/questions/51789685/reproduce-mm256-sllv-epi16-and-mm256-sllv-epi8-in-avx2/51805592#51805592
static inline
__m256i mm256_srav_epi16(__m256i x, __m256i n)
{
    const __m256i m = _mm256_set1_epi32(0x0000FFFF);
    __m256i lo = _mm256_srav_epi32(
        _mm256_and_si256(m, x),
        _mm256_and_si256(m, n)
    );
    __m256i hi = _mm256_srav_epi32(x, _mm256_srli_epi32(n, 16));
    return _mm256_blend_epi16(lo, hi, 0xAA);
}


static inline
short mm256_extractn_epi16(__m256i x, const int i)
{
    int16_t AYMO_ALIGN_V256 x_m256i_i16[16];
    _mm256_store_si256((__m256i*)(void*)x_m256i_i16, x);
    return x_m256i_i16[i];
}


static inline
__m256i mm256_insertn_epi16(__m256i x, short n, const int i)
{
    int16_t AYMO_ALIGN_V256 x_m256i_i16[16];
    _mm256_store_si256((__m256i*)(void*)x_m256i_i16, x);
    x_m256i_i16[i] = n;
    return _mm256_load_si256((__m256i*)(void*)x_m256i_i16);
}


// Gathers 16x 16-bit words via 16x 8-bit (low) indexes
static inline
__m256i mm256_i16gather_epi16lo(const int16_t* v, __m256i i)
{
#if (AYMO_CPU_X86_AVX2_GATHER16_STRATEGY == 2)
    // 2x 32-bit gatherings, 16-bit words, smallest cache footprint
    const __m256i sl = _mm256_set_epi8(
        -1, -1, -1, 12, -1, -1, -1,  8, -1, -1, -1,  4, -1, -1, -1,  0,
        -1, -1, -1, 12, -1, -1, -1,  8, -1, -1, -1,  4, -1, -1, -1,  0
    );
    const __m256i sh = _mm256_set_epi8(
        -1, -1, -1, 14, -1, -1, -1, 10, -1, -1, -1,  6, -1, -1, -1,  2,
        -1, -1, -1, 14, -1, -1, -1, 10, -1, -1, -1,  6, -1, -1, -1,  2
    );
    __m256i jh = _mm256_shuffle_epi8(i, sh);
    __m256i rh = _mm256_i32gather_epi32((const int32_t*)(const void*)v, jh, 2);
    rh = _mm256_slli_epi32(rh, 16);
    __m256i jl = _mm256_shuffle_epi8(i, sl);
    __m256i rl = _mm256_i32gather_epi32((const int32_t*)(const void*)v, jl, 2);
    return _mm256_blend_epi16(rl, rh, 0xAA);

#elif (CONFIG_AYMO_X86_AVX2_GATHER16_STRATEGY == 1)
    // 1x 32-bit gathering, joint 16-bit words, squared cache footprint
    const __m256i s = _mm256_set_epi8(
        -1, -1, 14, 12, -1, -1, 10, 8, -1, -1, 6, 4, -1, -1, 2, 0,
        -1, -1, 14, 12, -1, -1, 10, 8, -1, -1, 6, 4, -1, -1, 2, 0
    );
    __m256i j = _mm256_shuffle_epi8(i, s);
    return _mm256_i32gather_epi32((const int32_t*)(const void *)v, j, 4);

#else  // CONFIG_AYMO_X86_AVX2_GATHER16_STRATEGY
    // Plain C lookup, smallest cache footprint
    return vsetr(
        v[vextract(i, 0x0)],
        v[vextract(i, 0x1)],
        v[vextract(i, 0x2)],
        v[vextract(i, 0x3)],
        v[vextract(i, 0x4)],
        v[vextract(i, 0x5)],
        v[vextract(i, 0x6)],
        v[vextract(i, 0x7)],
        v[vextract(i, 0x8)],
        v[vextract(i, 0x9)],
        v[vextract(i, 0xA)],
        v[vextract(i, 0xB)],
        v[vextract(i, 0xC)],
        v[vextract(i, 0xD)],
        v[vextract(i, 0xE)],
        v[vextract(i, 0xF)]
    );
#endif  // CONFIG_AYMO_X86_AVX2_GATHER16_STRATEGY
}


// see: https://stackoverflow.com/questions/60108658/fastest-method-to-calculate-sum-of-all-packed-32-bit-integers-using-avx512-or-av/
static inline
int mm_hsum_epi32(__m128i x)
{
    __m128i hi64 = _mm_unpackhi_epi64(x, x);
    __m128i sum64 = _mm_add_epi32(hi64, x);
    __m128i hi32 = _mm_shuffle_epi32(sum64, _MM_SHUFFLE(2, 3, 0, 1));
    __m128i sum32 = _mm_add_epi32(sum64, hi32);
    return _mm_cvtsi128_si32(sum32);
}


static inline
int mm256_hsum_epi32(__m256i x)
{
    __m128i lo128 = _mm256_castsi256_si128(x);
    __m128i hi128 = _mm256_extracti128_si256(x, 1);
    __m128i sum32 = _mm_add_epi32(lo128, hi128);
    return mm_hsum_epi32(sum32);
}


// see: https://stackoverflow.com/questions/55057933/simd-accumulate-adjacent-pairs
static inline
int mm256_hsum_epi16(__m256i x)
{
    __m256i sum16 = _mm256_madd_epi16(x, vset1(1));
    return mm256_hsum_epi32(sum16);
}


static inline
int mm256_hsums_epi16(__m256i x)
{
    __m256i hs16 = _mm256_hadds_epi16(x, vsetz());
    __m256i sum16 = _mm256_unpacklo_epi16(hs16, vsetz());
    return mm256_hsum_epi32(sum16);
}


// 0 <= x < 4  -->  (1 << (x - 1))  -->  0, 1, 2, 4
static inline
__m256i mm256_pow2m1lt4_epi16(__m256i x)
{
    return vsub(x, vcmpgt(x, vset1(2)));
}


// 0 <= x < 4  -->  (1 << x)
static inline
__m256i mm256_pow2lt4_epi16(__m256i x)
{
    __m256i a = vadd(x, vset1(1));
    __m256i b = vu2i(vsubsu(vi2u(x), vi2u(vset1(2))));
    __m256i c = vmululo(b, b);
    return vadd(a, c);
}


static inline
long mm256_extractn_epi32(__m256i x, const int i)
{
    int32_t AYMO_ALIGN_V256 x_m256i_i32[8];
    _mm256_store_si256((__m256i*)(void*)x_m256i_i32, x);
    return x_m256i_i32[i];
}


static inline
__m256i mm256_insertn_epi32(__m256i x, long n, const int i)
{
    int32_t AYMO_ALIGN_V256 x_m256i_i32[8];
    _mm256_store_si256((__m256i*)(void*)x_m256i_i32, x);
    x_m256i_i32[i] = n;
    return _mm256_load_si256((__m256i*)(void*)x_m256i_i32);
}


static inline
float mm256_extractn_ps(__m256 x, const int i)
{
    float AYMO_ALIGN_V256 x_m256_f32[8];
    _mm256_store_ps(x_m256_f32, x);
    return x_m256_f32[i];
}


static inline
__m256 mm256_insertn_ps(__m256 x, float f, const int i)
{
    float AYMO_ALIGN_V256 x_m256_f32[8];
    _mm256_store_ps(x_m256_f32, x);
    x_m256_f32[i] = f;
    return _mm256_load_ps(x_m256_f32);
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
    if (_BitScanForward64(&i, x)) {
        return (int)(i + 1);
    }
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

#endif  // AYMO_CPU_SUPPORT_X86_AVX2

#endif  // _include_aymo_cpu_x86_avx2_inline_h
