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
#ifndef _include_aymo_testing_h
#define _include_aymo_testing_h

#include "aymo_cc.h"


// Test exit code status values
#define TEST_STATUS_PASS        (0)     // Test passed successfully
#define TEST_STATUS_FAIL        (1)     // Tast failed; usually with error text
#define TEST_STATUS_SKIP        (77)    // Test skipped
#define TEST_STATUS_HARD        (99)    // Test failed with hard error


// Macros to build test name lookup tables
typedef void (*aymo_testing_test_f)(void);  // using globals as test status variables

struct aymo_testing_entry {
    const char* name;
    aymo_testing_test_f func;
};

#define AYMO_TEST_ENTRY(name)   { AYMO_STRINGIFY2(name), name }


AYMO_PUBLIC char* aymo_test_args_to_str(
    int first,
    int last,
    char** argv,
    const char *prefix,
    const char *suffix
);
AYMO_PUBLIC void aymo_test_free_args_str(char* line);


#endif  // _include_aymo_testing_h
