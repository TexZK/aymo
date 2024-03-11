// CPU-specific inline methods for x86 SSE4.1.
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
#ifndef _include_aymo_cpu_x86_sse41_inline_h
#define _include_aymo_cpu_x86_sse41_inline_h

#include "aymo_cpu.h"
#ifdef AYMO_CPU_SUPPORT_X86_SSE41

#include <stdint.h>

AYMO_CXX_EXTERN_C_BEGIN


#ifndef AYMO_CPU_X86_SSE41_GATHER16_STRATEGY
    #define AYMO_CPU_X86_SSE41_GATHER16_STRATEGY 1
#endif


// Generic CPU shorthands

#define vsfence         _mm_sfence


// SIMD type shorthands
typedef vi16x8_t vi16_t;
typedef vu16x8_t vu16_t;
typedef vi32x4_t vi32_t;


// v*() methods are for vi16_t = int16_t[8]

#define vi2u(x)         x
#define vu2i(x)         x

#define vload           _mm_loadu_si128
#define vstore          _mm_storeu_si128
#define vstorelo        _mm_storel_epi64

#define vsetx           _mm_undefined_si128
#define vset1           _mm_set1_epi16
#define vseta           _mm_set_epi16
#define vsetr           _mm_setr_epi16
#define vsetz           _mm_setzero_si128
#define vsetf()         (vset1(-1))
#define vsetm            mm_setm_epi16

#define vnot(x)         (vxor((x), vsetf()))
#define vand            _mm_and_si128
#define vor             _mm_or_si128
#define vxor            _mm_xor_si128
#define vandnot         _mm_andnot_si128  // ~A & B
#define vblendi         _mm_blend_epi16
#define vblendv         _mm_blendv_epi8

#define vcmpeq          _mm_cmpeq_epi16
#define vcmpgt          _mm_cmpgt_epi16
#define vcmpz(x)        (vcmpeq((x), vsetz()))
#define vcmpp(x)        (vcmpgt((x), vsetz()))
#define vcmpn(x)        (vcmpgt(vsetz(), (x)))

#define vadd            _mm_add_epi16
#define vaddsi          _mm_adds_epi16
#define vaddsu          _mm_adds_epu16

#define vsub            _mm_sub_epi16
#define vsubsi          _mm_subs_epi16
#define vsubsu          _mm_subs_epu16
#define vneg(x)         (vsub(vsetz(), (x)))

#define vslli           _mm_slli_epi16
#define vsrli           _mm_srli_epi16
#define vsrai           _mm_srai_epi16
#define vsllv            mm_sllv_epi16
#define vsrlv            mm_srlv_epi16
#define vsrav            mm_srav_epi16

#define vmulihi         _mm_mulhi_epi16
#define vmuluhi         _mm_mulhi_epu16
#define vmulhrs         _mm_mulhrs_epi16

#define vmulilo         _mm_mullo_epi16
#define vmululo         _mm_mullo_epi16

#define vmadd           _mm_madd_epi16

#define vmini           _mm_min_epi16
#define vminu           _mm_min_epu16

#define vmaxi           _mm_max_epi16
#define vmaxu           _mm_max_epu16

#define vextract        _mm_extract_epi16
#define vextractn        mm_extractn_epi16
#define vextractv(x,i)   (((const int16_t*)(const void*)&(x))[(i)])

#define vinsert         _mm_insert_epi16
#define vinsertn         mm_insertn_epi16
#define vinsertv(x,n,i)  {((int16_t*)(void*)&(x))[(i)] = (n);}

#define vgather          mm_i16gather_epi16lo

#define vhsum            mm_hsum_epi16
#define vhsums           mm_hsums_epi16

#define vpow2m1lt4       mm_pow2m1lt4_epi16
#define vpow2lt4         mm_pow2lt4_epi16

#define vshufflelo      _mm_shufflelo_epi16
#define vshufflehi      _mm_shufflehi_epi16
#define valignr         _mm_alignr_epi8

#define vunpacklo       _mm_unpacklo_epi16
#define vunpackhi       _mm_unpackhi_epi16

#define v2vv            _mm_cvtepi16_epi32
#define vlo2vv(x)       (v2vv(x))
#define vhi2vv(x)       (v2vv(vvshuffle((x), KSHUFFLE(3, 2, 3, 2))))


// vv*() methods are for vi32_t = int32_t[4]

#define vvi2u(x)        x
#define vvu2i(x)        x

#define vvsetx          _mm_undefined_si128
#define vvset1          _mm_set1_epi32
#define vvseta          _mm_set_epi32
#define vvsetr          _mm_setr_epi32
#define vvsetz          _mm_setzero_si128
#define vvsetf()        (vvset1(-1))

#define vvand           vand
#define vvor            vor
#define vvxor           vxor
#define vvandnot        vandnot

#define vvadd           _mm_add_epi32

#define vvsrli          _mm_srli_epi32

#define vvsllv           mm_sllv_epi32

#define vvextract       _mm_extract_epi32
#define vvextractn       mm_extractn_epi32

#define vvinsert        _mm_insert_epi32
#define vvinsertn        mm_insertn_epi32

#define vvmullo         _mm_mullo_epi32

#define vvshuffle       _mm_shuffle_epi32
#define KSHUFFLE        _MM_SHUFFLE
#define vvswap(x)       vvshuffle((x), KSHUFFLE(1, 0, 3, 2))

#define vvpacks         _mm_packs_epi32
#define vvpackus        _mm_packus_epi32


static inline
__m128i mm_setm_epi16(uint8_t m)
{
    const __m128i k = vsetr(0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80);
    return vcmpeq(vand(vset1((int16_t)(uint16_t)m), k), k);
}


static inline
__m128i mm_sllv_epi16(__m128i x, __m128i n)
{
    // There's no quick way to perform variable bit shifts; resort to basic x86
    int16_t AYMO_ALIGN_V128 x_m128i_i16[8];
    uint16_t AYMO_ALIGN_V128 n_m128i_u16[8];
    _mm_store_si128((__m128i*)(void*)x_m128i_i16, x);
    _mm_store_si128((__m128i*)(void*)n_m128i_u16, n);
    x_m128i_i16[0] <<= n_m128i_u16[0];
    x_m128i_i16[1] <<= n_m128i_u16[1];
    x_m128i_i16[2] <<= n_m128i_u16[2];
    x_m128i_i16[3] <<= n_m128i_u16[3];
    x_m128i_i16[4] <<= n_m128i_u16[4];
    x_m128i_i16[5] <<= n_m128i_u16[5];
    x_m128i_i16[6] <<= n_m128i_u16[6];
    x_m128i_i16[7] <<= n_m128i_u16[7];
    return _mm_load_si128((__m128i*)(void*)x_m128i_i16);
}


// see: https://stackoverflow.com/questions/51789685/reproduce-mm256-sllv-epi16-and-mm256-sllv-epi8-in-sse41/51805592#51805592
static inline
__m128i mm_srlv_epi16(__m128i x, __m128i n)
{
    // There's no quick way to perform variable bit shifts; resort to basic x86
    uint16_t AYMO_ALIGN_V128 x_m128i_u16[8];
    uint16_t AYMO_ALIGN_V128 n_m128i_u16[8];
    _mm_store_si128((__m128i*)(void*)x_m128i_u16, x);
    _mm_store_si128((__m128i*)(void*)n_m128i_u16, n);
    x_m128i_u16[0] >>= n_m128i_u16[0];
    x_m128i_u16[1] >>= n_m128i_u16[1];
    x_m128i_u16[2] >>= n_m128i_u16[2];
    x_m128i_u16[3] >>= n_m128i_u16[3];
    x_m128i_u16[4] >>= n_m128i_u16[4];
    x_m128i_u16[5] >>= n_m128i_u16[5];
    x_m128i_u16[6] >>= n_m128i_u16[6];
    x_m128i_u16[7] >>= n_m128i_u16[7];
    return _mm_load_si128((__m128i*)(void*)x_m128i_u16);
}


static inline
__m128i mm_srav_epi16(__m128i x, __m128i n)
{
    // There's no quick way to perform variable bit shifts; resort to basic x86
    int16_t AYMO_ALIGN_V128 x_m128i_i16[8];
    uint16_t AYMO_ALIGN_V128 n_m128i_u16[8];
    _mm_store_si128((__m128i*)(void*)x_m128i_i16, x);
    _mm_store_si128((__m128i*)(void*)n_m128i_u16, n);
    x_m128i_i16[0] >>= n_m128i_u16[0];
    x_m128i_i16[1] >>= n_m128i_u16[1];
    x_m128i_i16[2] >>= n_m128i_u16[2];
    x_m128i_i16[3] >>= n_m128i_u16[3];
    x_m128i_i16[4] >>= n_m128i_u16[4];
    x_m128i_i16[5] >>= n_m128i_u16[5];
    x_m128i_i16[6] >>= n_m128i_u16[6];
    x_m128i_i16[7] >>= n_m128i_u16[7];
    return _mm_load_si128((__m128i*)(void*)x_m128i_i16);
}


static inline
short mm_extractn_epi16(__m128i x, const int i)
{
    int16_t AYMO_ALIGN_V128 x_m128i_i16[8];
    _mm_store_si128((__m128i*)(void*)x_m128i_i16, x);
    return x_m128i_i16[i];
}


static inline
__m128i mm_insertn_epi16(__m128i x, short n, const int i)
{
    int16_t AYMO_ALIGN_V128 x_m128i_i16[8];
    _mm_store_si128((__m128i*)(void*)x_m128i_i16, x);
    x_m128i_i16[i] = n;
    return _mm_load_si128((__m128i*)(void*)x_m128i_i16);
}


// Gathers 16x 16-bit words via 16x 8-bit (low) indexes
static inline
__m128i mm_i16gather_epi16lo(const int16_t* v, __m128i i)
{
    // Plain C lookup, smallest cache footprint
    uint8_t AYMO_ALIGN_V128 i_m128i_u8[16];
    _mm_store_si128((__m128i*)(void*)i_m128i_u8, i);
    int16_t* r_m128i_i16 = (int16_t*)(void*)i_m128i_u8;
    r_m128i_i16[0] = v[i_m128i_u8[0x0]];
    r_m128i_i16[1] = v[i_m128i_u8[0x2]];
    r_m128i_i16[2] = v[i_m128i_u8[0x4]];
    r_m128i_i16[3] = v[i_m128i_u8[0x6]];
    r_m128i_i16[4] = v[i_m128i_u8[0x8]];
    r_m128i_i16[5] = v[i_m128i_u8[0xA]];
    r_m128i_i16[6] = v[i_m128i_u8[0xC]];
    r_m128i_i16[7] = v[i_m128i_u8[0xE]];
    return _mm_load_si128((__m128i*)(void*)r_m128i_i16);
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


// see: https://stackoverflow.com/questions/55057933/simd-accumulate-adjacent-pairs
static inline
int mm_hsum_epi16(__m128i x)
{
    __m128i sum16 = _mm_madd_epi16(x, vset1(1));
    return mm_hsum_epi32(sum16);
}


static inline
int mm_hsums_epi16(__m128i x)
{
    __m128i hs16 = _mm_hadds_epi16(x, vsetz());
    __m128i sum16 = _mm_unpacklo_epi16(hs16, vsetz());
    return mm_hsum_epi32(sum16);
}


// 0 <= x < 4  -->  (1 << (x - 1))  -->  0, 1, 2, 4
static inline
__m128i mm_pow2m1lt4_epi16(__m128i x)
{
    return vsub(x, vcmpgt(x, vset1(2)));
}


// 0 <= x < 4  -->  (1 << x)
static inline
__m128i mm_pow2lt4_epi16(__m128i x)
{
    __m128i a = vadd(x, vset1(1));
    __m128i b = vu2i(vsubsu(vi2u(x), vi2u(vset1(2))));
    __m128i c = vmulilo(b, b);
    return vadd(a, c);
}


static inline
__m128i mm_sllv_epi32(__m128i x, __m128i n)
{
    // There's no quick way to perform variable bit shifts; resort to basic x86
    int32_t AYMO_ALIGN_V128 x_m128i_i32[4];
    uint32_t AYMO_ALIGN_V128 n_m128i_u32[4];
    _mm_store_si128((__m128i*)(void*)x_m128i_i32, x);
    _mm_store_si128((__m128i*)(void*)n_m128i_u32, n);
    x_m128i_i32[0] <<= n_m128i_u32[0];
    x_m128i_i32[1] <<= n_m128i_u32[1];
    x_m128i_i32[2] <<= n_m128i_u32[2];
    x_m128i_i32[3] <<= n_m128i_u32[3];
    return _mm_load_si128((__m128i*)(void*)x_m128i_i32);
}


static inline
long mm_extractn_epi32(__m128i x, const int i)
{
    int32_t AYMO_ALIGN_V128 x_m128i_i32[4];
    _mm_store_si128((__m128i*)(void*)x_m128i_i32, x);
    return x_m128i_i32[i];
}


static inline
__m128i mm_insertn_epi32(__m128i x, long n, const int i)
{
    int32_t AYMO_ALIGN_V128 x_m128i_i32[4];
    _mm_store_si128((__m128i*)(void*)x_m128i_i32, x);
    x_m128i_i32[i] = n;
    return _mm_load_si128((__m128i*)(void*)x_m128i_i32);
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

#endif  // AYMO_CPU_SUPPORT_X86_SSE41

#endif  // _include_aymo_cpu_x86_sse41_inline_h
