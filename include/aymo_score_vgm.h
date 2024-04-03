/*
AYMO - Accelerated YaMaha Operator
Copyright (c) 2023 Andrea Zoppi.

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
#ifndef _include_aymo_score_vgm_h
#define _include_aymo_score_vgm_h

#include "aymo_score.h"

AYMO_CXX_EXTERN_C_BEGIN


// See: https://vgmrips.net/wiki/VGM_Specification
enum aymo_score_vgm_offset {
    aymo_score_vgm_offset_vgm_ident             = 0x00,
    aymo_score_vgm_offset_eof_offset            = 0x04,
    aymo_score_vgm_offset_version               = 0x08,
    aymo_score_vgm_offset_sn76489_clock         = 0x0C,

    aymo_score_vgm_offset_ym2413_clock          = 0x10,
    aymo_score_vgm_offset_gd3_offset            = 0x14,
    aymo_score_vgm_offset_total_samples         = 0x18,
    aymo_score_vgm_offset_loop_offset           = 0x1C,

    aymo_score_vgm_offset_loop_samples          = 0x20,
    aymo_score_vgm_offset_rate                  = 0x24,
    aymo_score_vgm_offset_sn76489_feedback      = 0x28,
    aymo_score_vgm_offset_sn76489_width         = 0x2A,
    aymo_score_vgm_offset_sn76489_flags         = 0x2B,
    aymo_score_vgm_offset_ym2612_clock          = 0x2C,

    aymo_score_vgm_offset_ym2151_clock          = 0x30,
    aymo_score_vgm_offset_vgm_data_offset       = 0x34,
    aymo_score_vgm_offset_sega_pcm_clock        = 0x38,
    aymo_score_vgm_offset_spcm_interface        = 0x3C,

    aymo_score_vgm_offset_rf5c68_clock          = 0x40,
    aymo_score_vgm_offset_ym2203_clock          = 0x44,
    aymo_score_vgm_offset_ym2608_clock          = 0x48,
    aymo_score_vgm_offset_ym2610b_clock         = 0x4C,

    aymo_score_vgm_offset_ym3812_clock          = 0x50,
    aymo_score_vgm_offset_ym3526_clock          = 0x54,
    aymo_score_vgm_offset_y8950_clock           = 0x58,
    aymo_score_vgm_offset_ymf262_clock          = 0x5C,

    aymo_score_vgm_offset_ymf278b_clock         = 0x60,
    aymo_score_vgm_offset_ymf271_clock          = 0x64,
    aymo_score_vgm_offset_ymz280b_clock         = 0x68,
    aymo_score_vgm_offset_rf5c164_clock         = 0x6C,

    aymo_score_vgm_offset_pwm_clock             = 0x70,
    aymo_score_vgm_offset_ay8910_clock          = 0x74,
    aymo_score_vgm_offset_ay8910_chip_type      = 0x78,
    aymo_score_vgm_offset_ay8910_flags          = 0x79,
    aymo_score_vgm_offset_volume_modifier       = 0x7C,
    aymo_score_vgm_offset_reserved_0dh          = 0x7D,
    aymo_score_vgm_offset_loop_base             = 0x7E,
    aymo_score_vgm_offset_loop_modifier         = 0x7F,

    aymo_score_vgm_offset_gb_dmg_clock          = 0x80,
    aymo_score_vgm_offset_nes_apu_clock         = 0x84,
    aymo_score_vgm_offset_multipcm_clock        = 0x88,
    aymo_score_vgm_offset_upd7759_clock         = 0x8C,

    aymo_score_vgm_offset_okim6258_clock        = 0x90,
    aymo_score_vgm_offset_okim6258_flags        = 0x94,
    aymo_score_vgm_offset_k054539_flags         = 0x95,
    aymo_score_vgm_offset_c140_chip_type        = 0x96,
    aymo_score_vgm_offset_reserved_97h          = 0x97,
    aymo_score_vgm_offset_okim6295_clock        = 0x98,
    aymo_score_vgm_offset_k051649_clock         = 0x9C,

    aymo_score_vgm_offset_k054539_clock         = 0xA0,
    aymo_score_vgm_offset_huc6280_clock         = 0xA4,
    aymo_score_vgm_offset_c140_clock            = 0xa8,
    aymo_score_vgm_offset_k053260_clock         = 0xAC,

    aymo_score_vgm_offset_pokey_clock           = 0xB0,
    aymo_score_vgm_offset_qsound_clock          = 0xB4,
    aymo_score_vgm_offset_scsp_clock            = 0xB8,
    aymo_score_vgm_offset_extra_header_offset   = 0xBC,

    aymo_score_vgm_offset_wonderswan_clock      = 0xC0,
    aymo_score_vgm_offset_vsu_clock             = 0xC4,
    aymo_score_vgm_offset_saa1099_clock         = 0xC8,
    aymo_score_vgm_offset_es5503_clock          = 0xCC,

    aymo_score_vgm_offset_es5506_clock          = 0xD0,
    aymo_score_vgm_offset_es_chns               = 0xD4,
    aymo_score_vgm_offset_c352_clock_divider    = 0xD6,
    aymo_score_vgm_offset_reserved_d7h          = 0xD7,
    aymo_score_vgm_offset_x1_010_clock          = 0xD8,
    aymo_score_vgm_offset_c352_clock            = 0xDC,

    aymo_score_vgm_offset_ga20_clock            = 0xE0,
    aymo_score_vgm_offset_reserved_e4h          = 0xE4,
    aymo_score_vgm_offset_reserved_e8h          = 0xE8,
    aymo_score_vgm_offset_reserved_ech          = 0xEC,

    aymo_score_vgm_offset_reserved_f0h          = 0xF0,
    aymo_score_vgm_offset_reserved_f4h          = 0xF4,
    aymo_score_vgm_offset_reserved_f8h          = 0xF8,
    aymo_score_vgm_offset_reserved_fch          = 0xFC
};

enum aymo_score_vgm_type {
    aymo_score_vgm_type_opl1,
    aymo_score_vgm_type_opl2,
    aymo_score_vgm_type_opl3
};


struct aymo_score_vgm_instance {
    struct aymo_score_instance parent;
    const uint8_t* events;
    uint32_t opl_rate;
    uint32_t division;
    uint32_t eof_offset;
    uint32_t total_samples;
    uint32_t loop_samples;
    uint32_t loop_offset;
    uint32_t offset;
    uint32_t index;
};


AYMO_PUBLIC const struct aymo_score_vt aymo_score_vgm_vt;


AYMO_PUBLIC int aymo_score_vgm_ctor(
    struct aymo_score_vgm_instance* score
);

AYMO_PUBLIC void aymo_score_vgm_dtor(
    struct aymo_score_vgm_instance* score
);

AYMO_PUBLIC int aymo_score_vgm_load(
    struct aymo_score_vgm_instance* score,
    const void* data,
    uint32_t size
);

AYMO_PUBLIC void aymo_score_vgm_unload(
    struct aymo_score_vgm_instance* score
);

AYMO_PUBLIC struct aymo_score_status* aymo_score_vgm_get_status(
    struct aymo_score_vgm_instance* score
);

AYMO_PUBLIC void aymo_score_vgm_restart(
    struct aymo_score_vgm_instance* score
);

AYMO_PUBLIC uint32_t aymo_score_vgm_tick(
    struct aymo_score_vgm_instance* score,
    uint32_t count
);


AYMO_CXX_EXTERN_C_END

#endif  // _include_aymo_score_vgm_h
