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


int main(int argc, char** argv)
{
    app_return = TEST_STATUS_PASS;

    if (argc != 2) {
        fprintf(stderr, "USAGE:\t%s TETSNAME\n", (argc ? argv[0] : "test_exe"));
        return TEST_STATUS_HARD;
    }

    for (unsigned i = 0; i < AYMO_VECTOR_LENGTH(unit_tests); ++i) {
        if (!strcmp(unit_tests[i].name, argv[1])) {
            (unit_tests[i].func)();
            break;
        }
    }
    return app_return;
}


AYMO_CXX_EXTERN_C_END
