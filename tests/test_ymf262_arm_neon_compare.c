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

#define AYMO_KEEP_SHORTHANDS
#include "aymo_ymf262_arm_neon.h"

#include "test_ymf262_compare_prologue_inline.h"


static int compare_slots(int slot_)
{
    if (slot_ >= 36) {
        return 0;  // ignore
    }

    // TODO:

    return 0;
catch_:
    return 1;
}


static int compare_ch2xs(int ch2x)
{
    if (ch2x >= 18) {
        return 0;  // ignore
    }

    // TODO:

    return 0;
catch_:
    return 1;
}


static int compare_chips(void)
{
    // TODO:

    for (int ch2x = 0; ch2x < 18; ++ch2x) {
        if (compare_ch2xs(ch2x)) {
            assert(0);
        }
    }

    for (int slot = 0; slot < 36; ++slot) {
        if (compare_slots(slot)) {
            assert(0);
        }
    }

    return 0;
catch_:
    return 1;
}


#include "test_ymf262_compare_epilogue_inline.h"
