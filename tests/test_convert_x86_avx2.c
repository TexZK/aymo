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

#include "aymo.h"
#ifdef AYMO_CPU_SUPPORT_X86_AVX2

#include "aymo_file.h"
#include "aymo_testing.h"
#define AYMO_KEEP_SHORTHANDS
#include "aymo_convert_x86_avx2.h"

#include "test_convert_prologue_inline.h"


void test_aymo_convert_x86_avx2_i16_f32(void)
{
    unsigned si, ei; int line = 0;
    for (si = 0; si < ref_n; ++si) {
        for (ei = si; ei < ref_n; ++ei) {
            memset(buf_f32, DIRTY, sizeof(buf_f32));
            aymo_(i16_f32)((ei - si), &src_i16[si], &buf_f32[si]);
            if (compare_dirty(&buf_f32[0], DIRTY, (si * sizeof(buf_f32[0])))) {
                line = __LINE__; goto error_;
            }
            if (compare_f32(&buf_f32[si], &ref_i16_f32[si], (ei - si), 0)) {
                line = __LINE__; goto error_;
            }
            if (compare_dirty(&buf_f32[ei], DIRTY, ((ref_n - ei) * sizeof(buf_f32[0])))) {
                line = __LINE__; goto error_;
            }
        }
    }
    return;
error_:
    app_return = TEST_STATUS_FAIL;
    fprintf(stderr, "%s @ %d:  si=%u, ei=%u\n", __func__, line, si, ei);
    print_f32(stderr, buf_f32, ref_n);
    print_f32(stderr, ref_i16_f32, ref_n);
}


void test_aymo_convert_x86_avx2_f32_i16(void)
{
    unsigned si, ei; int line = 0;
    for (si = 0; si < ref_n; ++si) {
        for (ei = si; ei < ref_n; ++ei) {
            memset(buf_i16, (int)DIRTY, sizeof(buf_i16));
            aymo_(f32_i16)((ei - si), &src_f32[si], &buf_i16[si]);
            if (compare_dirty(&buf_i16[0], DIRTY, (si * sizeof(buf_i16[0])))) {
                line = __LINE__; goto error_;
            }
            if (compare_i16(&buf_i16[si], &ref_f32_i16[si], (ei - si))) {
                line = __LINE__; goto error_;
            }
            if (compare_dirty(&buf_i16[ei], DIRTY, ((ref_n - ei) * sizeof(buf_i16[0])))) {
                line = __LINE__; goto error_;
            }
        }
    }
    return;
error_:
    app_return = TEST_STATUS_FAIL;
    fprintf(stderr, "%s @ %d:  si=%u, ei=%u\n", __func__, line, si, ei);
    print_i16(stderr, buf_i16, ref_n);
    print_i16(stderr, ref_f32_i16, ref_n);
}


void test_aymo_convert_x86_avx2_i16_f32_1(void)
{
    unsigned si, ei; int line = 0;
    for (si = 0; si < ref_n; ++si) {
        for (ei = si; ei < ref_n; ++ei) {
            memset(buf_f32, (int)DIRTY, sizeof(buf_f32));
            aymo_(i16_f32_1)((ei - si), &src_i16[si], &buf_f32[si]);
            if (compare_dirty(&buf_f32[0], DIRTY, (si * sizeof(buf_f32[0])))) {
                line = __LINE__; goto error_;
            }
            if (compare_f32(&buf_f32[si], &ref_i16_f32_1[si], (ei - si), 0)) {
                line = __LINE__; goto error_;
            }
            if (compare_dirty(&buf_f32[ei], DIRTY, ((ref_n - ei) * sizeof(buf_f32[0])))) {
                line = __LINE__; goto error_;
            }
        }
    }
    return;
error_:
    app_return = TEST_STATUS_FAIL;
    fprintf(stderr, "%s @ %d:  si=%u, ei=%u\n", __func__, line, si, ei);
    print_f32(stderr, buf_f32, ref_n);
    print_f32(stderr, ref_i16_f32_1, ref_n);
}


void test_aymo_convert_x86_avx2_f32_i16_1(void)
{
    unsigned si, ei; int line = 0;
    for (si = 0; si < ref_n; ++si) {
        for (ei = si; ei < ref_n; ++ei) {
            memset(buf_i16, (int)DIRTY, sizeof(buf_i16));
            aymo_(f32_i16_1)((ei - si), &src_f32_1[si], &buf_i16[si]);
            if (compare_dirty(&buf_i16[0], DIRTY, (si * sizeof(buf_i16[0])))) {
                line = __LINE__; goto error_;
            }
            if (compare_i16(&buf_i16[si], &ref_f32_i16_1[si], (ei - si))) {
                line = __LINE__; goto error_;
            }
            if (compare_dirty(&buf_i16[ei], DIRTY, ((ref_n - ei) * sizeof(buf_i16[0])))) {
                line = __LINE__; goto error_;
            }
        }
    }
    return;
error_:
    app_return = TEST_STATUS_FAIL;
    fprintf(stderr, "%s @ %d:  si=%u, ei=%u\n", __func__, line, si, ei);
    print_i16(stderr, buf_i16, ref_n);
    print_i16(stderr, ref_f32_i16_1, ref_n);
}


void test_aymo_convert_x86_avx2_i16_f32_k(void)
{
    unsigned si, ei; int line = 0;
    for (si = 0; si < ref_n; ++si) {
        for (ei = si; ei < ref_n; ++ei) {
            memset(buf_f32, (int)DIRTY, sizeof(buf_f32));
            aymo_(i16_f32_k)((ei - si), &src_i16[si], &buf_f32[si], (float)(1. / K));
            if (compare_dirty(&buf_f32[0], DIRTY, (si * sizeof(buf_f32[0])))) {
                line = __LINE__; goto error_;
            }
            if (compare_f32(&buf_f32[si], &ref_i16_f32_1[si], (ei - si), 0)) {
                line = __LINE__; goto error_;
            }
            if (compare_dirty(&buf_f32[ei], DIRTY, ((ref_n - ei) * sizeof(buf_f32[0])))) {
                line = __LINE__; goto error_;
            }
        }
    }
    return;
error_:
    app_return = TEST_STATUS_FAIL;
    fprintf(stderr, "%s @ %d:  si=%u, ei=%u\n", __func__, line, si, ei);
    print_f32(stderr, buf_f32, ref_n);
    print_f32(stderr, ref_i16_f32_1, ref_n);
}


void test_aymo_convert_x86_avx2_f32_i16_k(void)
{
    unsigned si, ei; int line = 0;
    for (si = 0; si < ref_n; ++si) {
        for (ei = si; ei < ref_n; ++ei) {
            memset(buf_i16, (int)DIRTY, sizeof(buf_i16));
            aymo_(f32_i16_k)((ei - si), &src_f32_1[si], &buf_i16[si], (float)(K));
            if (compare_dirty(&buf_i16[0], DIRTY, (si * sizeof(buf_i16[0])))) {
                line = __LINE__; goto error_;
            }
            if (compare_i16(&buf_i16[si], &ref_f32_i16_1[si], (ei - si))) {
                line = __LINE__; goto error_;
            }
            if (compare_dirty(&buf_i16[ei], DIRTY, ((ref_n - ei) * sizeof(buf_i16[0])))) {
                line = __LINE__; goto error_;
            }
        }
    }
    return;
error_:
    app_return = TEST_STATUS_FAIL;
    fprintf(stderr, "%s @ %d:  si=%u, ei=%u\n", __func__, line, si, ei);
    print_i16(stderr, buf_i16, ref_n);
    print_i16(stderr, ref_f32_i16_1, ref_n);
}


void test_aymo_convert_x86_avx2_u16_f32(void)
{
    unsigned si, ei; int line = 0;
    for (si = 0; si < ref_n; ++si) {
        for (ei = si; ei < ref_n; ++ei) {
            memset(buf_f32, DIRTY, sizeof(buf_f32));
            aymo_(u16_f32)((ei - si), &src_u16[si], &buf_f32[si]);
            if (compare_dirty(&buf_f32[0], DIRTY, (si * sizeof(buf_f32[0])))) {
                line = __LINE__; goto error_;
            }
            if (compare_f32(&buf_f32[si], &ref_u16_f32[si], (ei - si), 0)) {
                line = __LINE__; goto error_;
            }
            if (compare_dirty(&buf_f32[ei], DIRTY, ((ref_n - ei) * sizeof(buf_f32[0])))) {
                line = __LINE__; goto error_;
            }
        }
    }
    return;
error_:
    app_return = TEST_STATUS_FAIL;
    fprintf(stderr, "%s @ %d:  si=%u, ei=%u\n", __func__, line, si, ei);
    print_f32(stderr, buf_f32, ref_n);
    print_f32(stderr, ref_u16_f32, ref_n);
}


void test_aymo_convert_x86_avx2_f32_u16(void)
{
    unsigned si, ei; int line = 0;
    for (si = 0; si < ref_n; ++si) {
        for (ei = si; ei < ref_n; ++ei) {
            memset(buf_u16, (int)DIRTY, sizeof(buf_u16));
            aymo_(f32_u16)((ei - si), &src_f32[si], &buf_u16[si]);
            if (compare_dirty(&buf_u16[0], DIRTY, (si * sizeof(buf_u16[0])))) {
                line = __LINE__; goto error_;
            }
            if (compare_u16(&buf_u16[si], &ref_f32_u16[si], (ei - si))) {
                line = __LINE__; goto error_;
            }
            if (compare_dirty(&buf_u16[ei], DIRTY, ((ref_n - ei) * sizeof(buf_u16[0])))) {
                line = __LINE__; goto error_;
            }
        }
    }
    return;
error_:
    app_return = TEST_STATUS_FAIL;
    fprintf(stderr, "%s @ %d:  si=%u, ei=%u\n", __func__, line, si, ei);
    print_u16(stderr, buf_u16, ref_n);
    print_u16(stderr, ref_f32_u16, ref_n);
}


void test_aymo_convert_x86_avx2_u16_f32_1(void)
{
    unsigned si, ei; int line = 0;
    for (si = 0; si < ref_n; ++si) {
        for (ei = si; ei < ref_n; ++ei) {
            memset(buf_f32, (int)DIRTY, sizeof(buf_f32));
            aymo_(u16_f32_1)((ei - si), &src_u16[si], &buf_f32[si]);
            if (compare_dirty(&buf_f32[0], DIRTY, (si * sizeof(buf_f32[0])))) {
                line = __LINE__; goto error_;
            }
            if (compare_f32(&buf_f32[si], &ref_u16_f32_1[si], (ei - si), 0)) {
                line = __LINE__; goto error_;
            }
            if (compare_dirty(&buf_f32[ei], DIRTY, ((ref_n - ei) * sizeof(buf_f32[0])))) {
                line = __LINE__; goto error_;
            }
        }
    }
    return;
error_:
    app_return = TEST_STATUS_FAIL;
    fprintf(stderr, "%s @ %d:  si=%u, ei=%u\n", __func__, line, si, ei);
    print_f32(stderr, buf_f32, ref_n);
    print_f32(stderr, ref_u16_f32_1, ref_n);
}


void test_aymo_convert_x86_avx2_f32_u16_1(void)
{
    unsigned si, ei; int line = 0;
    for (si = 0; si < ref_n; ++si) {
        for (ei = si; ei < ref_n; ++ei) {
            memset(buf_u16, (int)DIRTY, sizeof(buf_u16));
            aymo_(f32_u16_1)((ei - si), &src_f32_1[si], &buf_u16[si]);
            if (compare_dirty(&buf_u16[0], DIRTY, (si * sizeof(buf_u16[0])))) {
                line = __LINE__; goto error_;
            }
            if (compare_u16(&buf_u16[si], &ref_f32_u16_1[si], (ei - si))) {
                line = __LINE__; goto error_;
            }
            if (compare_dirty(&buf_u16[ei], DIRTY, ((ref_n - ei) * sizeof(buf_u16[0])))) {
                line = __LINE__; goto error_;
            }
        }
    }
    return;
error_:
    app_return = TEST_STATUS_FAIL;
    fprintf(stderr, "%s @ %d:  si=%u, ei=%u\n", __func__, line, si, ei);
    print_u16(stderr, buf_u16, ref_n);
    print_u16(stderr, ref_f32_u16_1, ref_n);
}


void test_aymo_convert_x86_avx2_u16_f32_k(void)
{
    unsigned si, ei; int line = 0;
    for (si = 0; si < ref_n; ++si) {
        for (ei = si; ei < ref_n; ++ei) {
            memset(buf_f32, (int)DIRTY, sizeof(buf_f32));
            aymo_(u16_f32_k)((ei - si), &src_u16[si], &buf_f32[si], (float)(1. / K));
            if (compare_dirty(&buf_f32[0], DIRTY, (si * sizeof(buf_f32[0])))) {
                line = __LINE__; goto error_;
            }
            if (compare_f32(&buf_f32[si], &ref_u16_f32_1[si], (ei - si), 0)) {
                line = __LINE__; goto error_;
            }
            if (compare_dirty(&buf_f32[ei], DIRTY, ((ref_n - ei) * sizeof(buf_f32[0])))) {
                line = __LINE__; goto error_;
            }
        }
    }
    return;
error_:
    app_return = TEST_STATUS_FAIL;
    fprintf(stderr, "%s @ %d:  si=%u, ei=%u\n", __func__, line, si, ei);
    print_f32(stderr, buf_f32, ref_n);
    print_f32(stderr, ref_u16_f32_1, ref_n);
}


void test_aymo_convert_x86_avx2_f32_u16_k(void)
{
    unsigned si, ei; int line = 0;
    for (si = 0; si < ref_n; ++si) {
        for (ei = si; ei < ref_n; ++ei) {
            memset(buf_u16, (int)DIRTY, sizeof(buf_u16));
            aymo_(f32_u16_k)((ei - si), &src_f32_1[si], &buf_u16[si], (float)(K));
            if (compare_dirty(&buf_u16[0], DIRTY, (si * sizeof(buf_u16[0])))) {
                line = __LINE__; goto error_;
            }
            if (compare_u16(&buf_u16[si], &ref_f32_u16_1[si], (ei - si))) {
                line = __LINE__; goto error_;
            }
            if (compare_dirty(&buf_u16[ei], DIRTY, ((ref_n - ei) * sizeof(buf_u16[0])))) {
                line = __LINE__; goto error_;
            }
        }
    }
    return;
error_:
    app_return = TEST_STATUS_FAIL;
    fprintf(stderr, "%s @ %d:  si=%u, ei=%u\n", __func__, line, si, ei);
    print_u16(stderr, buf_u16, ref_n);
    print_u16(stderr, ref_f32_u16_1, ref_n);
}


struct aymo_testing_entry unit_tests[] =
{
    AYMO_TEST_ENTRY(test_aymo_convert_x86_avx2_i16_f32),
    AYMO_TEST_ENTRY(test_aymo_convert_x86_avx2_f32_i16),
    AYMO_TEST_ENTRY(test_aymo_convert_x86_avx2_i16_f32_1),
    AYMO_TEST_ENTRY(test_aymo_convert_x86_avx2_f32_i16_1),
    AYMO_TEST_ENTRY(test_aymo_convert_x86_avx2_i16_f32_k),
    AYMO_TEST_ENTRY(test_aymo_convert_x86_avx2_f32_i16_k),
    AYMO_TEST_ENTRY(test_aymo_convert_x86_avx2_u16_f32),
    AYMO_TEST_ENTRY(test_aymo_convert_x86_avx2_f32_u16),
    AYMO_TEST_ENTRY(test_aymo_convert_x86_avx2_u16_f32_1),
    AYMO_TEST_ENTRY(test_aymo_convert_x86_avx2_f32_u16_1),
    AYMO_TEST_ENTRY(test_aymo_convert_x86_avx2_u16_f32_k),
    AYMO_TEST_ENTRY(test_aymo_convert_x86_avx2_f32_u16_k)
};


#include "aymo_testing_epilogue_inline.h"


#endif  // AYMO_CPU_SUPPORT_X86_AVX2
