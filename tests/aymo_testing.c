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

#include "aymo_testing.h"

#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

AYMO_CXX_EXTERN_C_BEGIN


char* aymo_test_args_to_str(
    int first,
    int last,
    char** argv,
    const char *prefix,
    const char *suffix
)
{
    assert(argv);

    size_t prefix_len = 0u;
    if (prefix) {
        prefix_len = strlen(prefix);
    }
    size_t suffix_len = 0u;
    if (suffix) {
        suffix_len = strlen(suffix);
    }

    size_t line_len = (prefix_len + (size_t)(last - first) + suffix_len);

    if (first == 0) {
        char* fwd = strrchr(argv[0], '/');
        if (fwd) {
            argv[0] = (fwd + 1);
        }
        char* bwd = strrchr(argv[0], '\\');
        if (bwd) {
            argv[0] = (bwd + 1);
        }
    }

    for (int i = first; i <= last; ++i) {
        assert(argv[i]);
        size_t arg_len = strlen(argv[i]);
        line_len += arg_len;
    }

    char *line = malloc(line_len + 1u);
    assert(line);
    size_t offset = 0u;

    if (prefix) {
        memcpy(&line[offset], prefix, prefix_len);
        offset += prefix_len;
    }
    for (int i = first; i <= last; ++i) {
        size_t arg_len = strlen(argv[i]);
        memcpy(&line[offset], argv[i], arg_len);
        offset += arg_len;
        if (i < last) {
            line[offset++] = '_';
        }
    }
    if (suffix) {
        memcpy(&line[offset], suffix, suffix_len);
        offset += suffix_len;
    }

    line[offset] = '\0';

    for (offset = 0u; offset < line_len; ++offset) {
        char c = line[offset];
        if (!isgraph(c) || (c == '/') || (c == '\\')) {
            line[offset] = '_';
        }
    }

    return line;
}


void aymo_test_free_args_str(char* line)
{
    free(line);
}


AYMO_CXX_EXTERN_C_END
