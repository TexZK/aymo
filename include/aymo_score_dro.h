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
#ifndef _include_aymo_score_dro_h
#define _include_aymo_score_dro_h

#include "aymo_score.h"

AYMO_CXX_EXTERN_C_BEGIN


// See: https://moddingwiki.shikadi.net/wiki/DRO_Format

AYMO_PRAGMA_SCALAR_STORAGE_ORDER_LITTLE_ENDIAN
AYMO_PRAGMA_PACK_PUSH_1

#define AYMO_DRO_SIGNATURE  "DBRAWOPL"

// Common DRO header, at the very beginning of the file
// NOTE: v0.1 == v1.0
struct aymo_score_dro_header {
    char signature[8u];
    uint16_t version_major;
    uint16_t version_minor;
//  struct aymo_score_dro_v?_header versioned_header;
};


// DRO v1.0 hardware type
enum aymo_score_dro_v1_hardware_type {
    aymo_score_dro_v1_hardware_type_opl2 = 0,
    aymo_score_dro_v1_hardware_type_opl2x2,
    aymo_score_dro_v1_hardware_type_opl3,
};

// DRO v1.0 sub-header
struct aymo_score_dro_v1_header {
    uint32_t length_ms;
    uint32_t length_bytes;
    uint8_t hardware_type;
    uint8_t hardware_extra[3];
};

// DRO v1.0 special codes
enum aymo_score_dro_v1_code {
    aymo_score_dro_v1_code_delay_byte = 0,
    aymo_score_dro_v1_code_delay_word,
    aymo_score_dro_v1_code_switch_low,
    aymo_score_dro_v1_code_switch_high,
    aymo_score_dro_v1_code_escape,
    aymo_score_dro_v1_code_invalid = 0xFF
};


// DRO v2.0 hardware type
enum aymo_score_dro_v2_hardware_type {
    aymo_score_dro_v2_hardware_type_opl2 = 0,
    aymo_score_dro_v2_hardware_type_opl2x2,
    aymo_score_dro_v2_hardware_type_opl3,
};

// DRO v2.0 format
enum aymo_score_dro_v2_format {
    aymo_score_dro_v2_format_interleaved = 0
};

// DRO v2.0 sub-header
struct aymo_score_dro_v2_header {
    uint32_t length_pairs;
    uint32_t length_ms;
    uint8_t hardware_type;
    uint8_t format;
    uint8_t compression;
    uint8_t short_delay_code;
    uint8_t long_delay_code;
    uint8_t codemap_length;
//  uint8_t codemap_table[codemap_length];
};


// <Code, Value> score event pair
struct aymo_score_dro_pair {
    uint8_t code;
    uint8_t value;
};

AYMO_PRAGMA_PACK_POP
AYMO_PRAGMA_SCALAR_STORAGE_ORDER_DEFAULT


// Score player score
struct aymo_score_dro_instance {
    const struct aymo_score_vt* vt;
    struct aymo_score_status status;
    const struct aymo_score_dro_header *header;
    const struct aymo_score_dro_v1_header *v1_header;
    const struct aymo_score_dro_v2_header *v2_header;
    const uint8_t* codemap;
    const uint8_t* events;
    uint32_t opl_rate;
    uint32_t division;
    uint32_t length;
    uint32_t offset;
    uint8_t address_hi;
};


AYMO_PUBLIC const struct aymo_score_vt aymo_score_dro_vt;


AYMO_PUBLIC int aymo_score_dro_init_specific(
    struct aymo_score_dro_instance* score,
    uint32_t opl_rate
);

AYMO_PUBLIC int aymo_score_dro_ctor(
    struct aymo_score_dro_instance* score
);

AYMO_PUBLIC void aymo_score_dro_dtor(
    struct aymo_score_dro_instance* score
);

AYMO_PUBLIC int aymo_score_dro_load(
    struct aymo_score_dro_instance* score,
    const void* data,
    uint32_t size
);

AYMO_PUBLIC void aymo_score_dro_unload(
    struct aymo_score_dro_instance* score
);

AYMO_PUBLIC struct aymo_score_status* aymo_score_dro_get_status(
    struct aymo_score_dro_instance* score
);

AYMO_PUBLIC void aymo_score_dro_restart(
    struct aymo_score_dro_instance* score
);

AYMO_PUBLIC uint32_t aymo_score_dro_tick(
    struct aymo_score_dro_instance* score,
    uint32_t count
);


AYMO_CXX_EXTERN_C_END

#endif  // _include_aymo_score_dro_h
