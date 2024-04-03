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

#include "aymo_cpu.h"
#ifdef AYMO_CPU_SUPPORT_X86_AVX2

#include "aymo_tda8425.h"
#define AYMO_KEEP_SHORTHANDS
#include "aymo_tda8425_x86_avx2.h"

#include <assert.h>

AYMO_CXX_EXTERN_C_BEGIN

#undef cos
#undef fabs
#undef log10
#undef pow
#undef sqrt
#undef tan

#define cos     (aymo_tda8425_math->cos)
#define fabs    (aymo_tda8425_math->fabs)
#define log10   (aymo_tda8425_math->log10)
#define pow     (aymo_tda8425_math->pow)
#define sqrt    (aymo_tda8425_math->sqrt)
#define tan     (aymo_tda8425_math->tan)


#undef mm256_alignr_ps
#define mm256_alignr_ps(a, b, imm8)  \
    (_mm256_castsi256_ps(_mm256_alignr_epi8(_mm256_castps_si256(a), _mm256_castps_si256(b), ((imm8) * 4))))


const struct aymo_tda8425_vt aymo_(vt) =
{
    AYMO_STRINGIFY2(aymo_(vt)),
    (aymo_tda8425_get_sizeof_f)&(aymo_(get_sizeof)),
    (aymo_tda8425_ctor_f)&(aymo_(ctor)),
    (aymo_tda8425_dtor_f)&(aymo_(dtor)),
    (aymo_tda8425_read_f)&(aymo_(read)),
    (aymo_tda8425_write_f)&(aymo_(write)),
    (aymo_tda8425_process_f32_f)&(aymo_(process_f32))
};


const struct aymo_tda8425_vt* aymo_(get_vt)(void)
{
    return &aymo_(vt);
}


uint32_t aymo_(get_sizeof)(void)
{
    return sizeof(struct aymo_(chip));
}


void aymo_(ctor)(struct aymo_(chip)* chip, float sample_rate)
{
    assert(chip);
    assert(sample_rate > 0.f);

    // Wipe everything, except VT
    aymo_memset((&chip->parent.vt + 1u), 0, (sizeof(*chip) - sizeof(chip->parent.vt)));

    // Setup default parameters
    chip->sample_rate = sample_rate;
    chip->pseudo_c1 = aymo_tda8425_pseudo_preset_c1[0];
    chip->pseudo_c2 = aymo_tda8425_pseudo_preset_c2[0];

    // Setup default registers
    aymo_(write)(chip, 0x00u, 0xFCu);  // VL: 0 dB
    aymo_(write)(chip, 0x01u, 0xFCu);  // VR: 0 dB
    aymo_(write)(chip, 0x02u, 0xF6u);  // BA: 0 dB
    aymo_(write)(chip, 0x03u, 0xF6u);  // TR: 0 dB
    aymo_(write)(chip, 0x07u, 0xFCu);  // PP: light pseudo
    aymo_(write)(chip, 0x08u, 0xCEu);  // SF: linear stereo, channel 1, unmuted
}


void aymo_(dtor)(struct aymo_(chip)* chip)
{
    AYMO_UNUSED_VAR(chip);
    assert(chip);
}


static void aymo_(apply_vl)(struct aymo_(chip)* chip)
{
    double db = (double)aymo_tda8425_reg_v_to_db[chip->reg_vl & 0x3Fu];

    if (chip->reg_sf & 0x20u) {  // mute
        db = -90.;
    }

    double g = pow(10., (db * .05));
    vf32x4_t kvlo = _mm_set_ps((float)g, .0f, .0f, .0f);
    chip->kv = _mm256_insertf128_ps(chip->kv, kvlo, 0);
}


static void aymo_(apply_vr)(struct aymo_(chip)* chip)
{
    double db = (double)aymo_tda8425_reg_v_to_db[chip->reg_vr & 0x3Fu];

    if (chip->reg_sf & 0x20u) {  // mute
        db = -90.;
    }

    double g = pow(10., (db * .05));
    vf32x4_t kvhi = _mm_set_ps((float)g, .0f, .0f, .0f);
    chip->kv = _mm256_insertf128_ps(chip->kv, kvhi, 1);
}


static void aymo_(apply_ba)(struct aymo_(chip)* chip)
{
    double dbb = (double)aymo_tda8425_reg_ba_to_db[chip->reg_ba & 0x0Fu];
    double gb = pow(10., (dbb * (.05 * .5)));
    double fs = (double)chip->sample_rate;
    double pi = 3.14159265358979323846264338327950288;
    double fcb = 300.;  // [Hz]
    double wb = ((2. * pi) * fcb);
    double kb = (tan(wb * (.5 / fs)) / wb);

    double a0 = ((kb * wb) + gb);
    double a1 = ((kb * wb) - gb);
    double a2 = 0.;

    double b0 = (((kb * wb) * (gb * gb)) + gb);
    double b1 = (((kb * wb) * (gb * gb)) - gb);
    double b2 = 0.;

    double ra0 = (1. / a0);
    chip->kb0 = _mm256_blend_ps(chip->kb0, _mm256_set1_ps((float)(b0 * ra0)), 0x44);
    chip->kb1 = _mm256_blend_ps(chip->kb1, _mm256_set1_ps((float)(b1 * ra0)), 0x44);
    chip->kb2 = _mm256_blend_ps(chip->kb2, _mm256_set1_ps((float)(b2 * ra0)), 0x44);
    ra0 = -ra0;
    chip->ka1 = _mm256_blend_ps(chip->ka1, _mm256_set1_ps((float)(a1 * ra0)), 0x44);
    chip->ka2 = _mm256_blend_ps(chip->ka2, _mm256_set1_ps((float)(a2 * ra0)), 0x44);
}


static void aymo_(apply_tr)(struct aymo_(chip)* chip)
{
    double db = (double)aymo_tda8425_reg_tr_to_db[chip->reg_tr & 0x0Fu];
    double gt = pow(10., (db * (.05 * .5)));
    double fs = (double)chip->sample_rate;
    double pi = 3.14159265358979323846264338327950288;
    double fcd = 10.;  // [Hz]
    double wd = ((2. * pi) * fcd);
    double kd = ((chip->reg_sf & 0x40u) ? 0. : (tan(wd * (.5 / fs)) / wd));
    double fct = 4500.;  // [Hz]
    double wt = ((2. * pi) * fct);
    double kt = (tan(wt * (.5 / fs)) / wt);

    double a0 = (((gt * kt * wt) * (kd * wd)) + ((gt * kt * wt) + (kd * wd)) + 1.);
    double a1 = (((gt * kt * wt) * (kd * wd) * 2.) - 2.);
    double a2 = (((gt * kt * wt) * (kd * wd)) - ((gt * kt * wt) + (kd * wd)) + 1.);

    double b0 = ((gt * gt) + (gt * kt * wt));
    double b1 = ((gt * gt) * -2.);
    double b2 = ((gt * gt) - (gt * kt * wt));

    double ra0 = (1. / a0);
    chip->kb0 = _mm256_blend_ps(chip->kb0, _mm256_set1_ps((float)(b0 * ra0)), 0x22);
    chip->kb1 = _mm256_blend_ps(chip->kb1, _mm256_set1_ps((float)(b1 * ra0)), 0x22);
    chip->kb2 = _mm256_blend_ps(chip->kb2, _mm256_set1_ps((float)(b2 * ra0)), 0x22);
    ra0 = -ra0;
    chip->ka1 = _mm256_blend_ps(chip->ka1, _mm256_set1_ps((float)(a1 * ra0)), 0x22);
    chip->ka2 = _mm256_blend_ps(chip->ka2, _mm256_set1_ps((float)(a2 * ra0)), 0x22);
}


static void aymo_(apply_source_mode)(struct aymo_(chip)* chip)
{
    // Default mute
    vf32x8_t klr = _mm256_setzero_ps();
    vf32x8_t krl = _mm256_setzero_ps();

    uint8_t source = (chip->reg_sf & 0x07u);
    uint8_t mode = ((chip->reg_sf >> 3u) & 0x03u);

    // Forced mono
    if (mode == 0x00u) {  // process
        switch (source) {
            // Channel 1
            case 0x02u:
            case 0x04u:
            case 0x06u: {
                klr = _mm256_set_ps(1.f, .0f, .0f, .0f,  1.f, .0f, .0f, .0f);
                krl = _mm256_set_ps(1.f, .0f, .0f, .0f,  1.f, .0f, .0f, .0f);
                break;
            }
        }
    }
    else {  // not forced mono
        switch (source) {
            // Channel 1
            case 0x02u: {  // mono left
                klr = _mm256_set_ps(0.f, .0f, .0f, .0f,  1.f, .0f, .0f, .0f);
                krl = _mm256_set_ps(1.f, .0f, .0f, .0f,  0.f, .0f, .0f, .0f);
                break;
            }
            case 0x04u: {  // mono right
                klr = _mm256_set_ps(1.f, .0f, .0f, .0f,  0.f, .0f, .0f, .0f);
                krl = _mm256_set_ps(0.f, .0f, .0f, .0f,  1.f, .0f, .0f, .0f);
                break;
            }
            case 0x06u: {  // stereo
                klr = _mm256_set_ps(1.f, .0f, .0f, .0f,  1.f, .0f, .0f, .0f);
                krl = _mm256_set_ps(0.f, .0f, .0f, .0f,  0.f, .0f, .0f, .0f);
                break;
            }
            default: {
                if (mode == 0x03u) {  // spatial stereo
                    mode = 0x02u;  // force linear stereo (mute)
                }
                break;
            }
        }

        // Spatial stereo
        if (mode == 0x03u) {  // process
            const float xt = .52f;  // cross-talk
            __m256 kx = _mm256_set_ps(xt, .0f, .0f, .0f,  xt, .0f, .0f, .0f);
            klr = _mm256_add_ps(klr, kx);
            krl = _mm256_sub_ps(krl, kx);
        }
    }  // not forced mono

    chip->klr = klr;
    chip->krl = krl;
}


static void aymo_(apply_pseudo)(struct aymo_(chip)* chip)
{
    uint8_t mode = ((chip->reg_sf >> 3u) & 0x03u);

    // Pseudo stereo
    if (mode == 0x02u) {  // enabled
        double c1 = (double)chip->pseudo_c1;
        double c2 = (double)chip->pseudo_c2;
        double r1 = 15000.;  // [ohm]
        double r2 = 15000.;  // [ohm]
        double t1 = (c1 * r1);
        double t2 = (c2 * r2);

        double fs = (double)chip->sample_rate;
        double k = (.5 / fs);
        double kk = (k * k);
        double t1_t2 = (t1 * t2);
        double t1_t2_k = ((t1 + t2) * k);

        double a0 = (kk + t1_t2 + t1_t2_k);
        double a1 = ((kk - t1_t2) * 2.);
        double a2 = (kk + t1_t2 - t1_t2_k);

        double b0 = a2;
        double b1 = a1;
        double b2 = a0;

        double ra0 = (1. / a0);
        chip->kb0 = _mm256_blend_ps(chip->kb0, _mm256_set1_ps((float)(b0 * ra0)), 0x11);
        chip->kb1 = _mm256_blend_ps(chip->kb1, _mm256_set1_ps((float)(b1 * ra0)), 0x11);
        chip->kb2 = _mm256_blend_ps(chip->kb2, _mm256_set1_ps((float)(b2 * ra0)), 0x11);
        ra0 = -ra0;
        chip->ka1 = _mm256_blend_ps(chip->ka1, _mm256_set1_ps((float)(a1 * ra0)), 0x11);
        chip->ka2 = _mm256_blend_ps(chip->ka2, _mm256_set1_ps((float)(a2 * ra0)), 0x11);
    }
    else {  // pass-through
        chip->kb0 = _mm256_blend_ps(chip->kb0, _mm256_set1_ps(1.f), 0x11);
        chip->kb1 = _mm256_blend_ps(chip->kb1, _mm256_set1_ps(.0f), 0x11);
        chip->kb2 = _mm256_blend_ps(chip->kb2, _mm256_set1_ps(.0f), 0x11);

        chip->ka1 = _mm256_blend_ps(chip->ka1, _mm256_set1_ps(.0f), 0x11);
        chip->ka2 = _mm256_blend_ps(chip->ka2, _mm256_set1_ps(.0f), 0x11);
    }
}


static void aymo_(apply_tfilter)(struct aymo_(chip)* chip)
{
    // T-filter
    if (chip->reg_sf & 0x80u) {  // pass-through
        chip->kb0 = _mm256_blend_ps(chip->kb0, _mm256_set1_ps(1.f), 0x88);
        chip->kb1 = _mm256_blend_ps(chip->kb1, _mm256_set1_ps(.0f), 0x88);
        chip->kb2 = _mm256_blend_ps(chip->kb2, _mm256_set1_ps(.0f), 0x88);

        chip->ka1 = _mm256_blend_ps(chip->ka1, _mm256_set1_ps(.0f), 0x88);
        chip->ka2 = _mm256_blend_ps(chip->ka2, _mm256_set1_ps(.0f), 0x88);
    }
    else {  // enabled
        double db = (double)aymo_tda8425_reg_ba_to_db[chip->reg_ba & 0x0Fu];
        double g = pow(10., (db * (.05 * .5)));
        double fs = (double)chip->sample_rate;
        double pi = 3.14159265358979323846264338327950288;
        double fc = 180.;  // [Hz]
        double w = ((2. * pi) * fc);
        double k = (tan(w * (.5 / fs)) / w);

        double log10_g = log10(g);
        double ang = (log10_g * .85);
        double abs_sqrt_log10_g = sqrt(fabs(log10_g));
        double abs2_sqrt_log10_g = abs_sqrt_log10_g * abs_sqrt_log10_g;
        double kw = (k * w);
        double m_k2w2 = ((kw * kw) * -.05);
        double sqrt_5 = 2.23606797749978980505147774238139391;
        double ph = (pi * .75);
        double h_sqrt_5_kw_abs_sqrt_log10_g = ((sqrt_5 * .2) * kw * abs_sqrt_log10_g);
        double cosm = cos(ang - ph);
        double cosp = cos(ang + ph);

        double a0 = (((m_k2w2 - abs2_sqrt_log10_g) + (h_sqrt_5_kw_abs_sqrt_log10_g * cosm)));
        double a1 = (((m_k2w2 + abs2_sqrt_log10_g)) * 2.);
        double a2 = (((m_k2w2 - abs2_sqrt_log10_g) - (h_sqrt_5_kw_abs_sqrt_log10_g * cosm)));

        double b0 = (((m_k2w2 - abs2_sqrt_log10_g) + (h_sqrt_5_kw_abs_sqrt_log10_g * cosp)));
        double b1 = a1;
        double b2 = (((m_k2w2 - abs2_sqrt_log10_g) - (h_sqrt_5_kw_abs_sqrt_log10_g * cosp)));

        double ra0 = (1. / a0);
        chip->kb0 = _mm256_blend_ps(chip->kb0, _mm256_set1_ps((float)(b0 * ra0)), 0x88);
        chip->kb1 = _mm256_blend_ps(chip->kb1, _mm256_set1_ps((float)(b1 * ra0)), 0x88);
        chip->kb2 = _mm256_blend_ps(chip->kb2, _mm256_set1_ps((float)(b2 * ra0)), 0x88);
        ra0 = -ra0;
        chip->ka1 = _mm256_blend_ps(chip->ka1, _mm256_set1_ps((float)(a1 * ra0)), 0x88);
        chip->ka2 = _mm256_blend_ps(chip->ka2, _mm256_set1_ps((float)(a2 * ra0)), 0x88);
    }
}


static void aymo_(apply_pp)(struct aymo_(chip)* chip)
{
    uint8_t pseudo_preset = (chip->reg_pp & 0x03u);
    if (pseudo_preset >= 3u) {
        pseudo_preset = 0u;
    }
    chip->pseudo_c1 = aymo_tda8425_pseudo_preset_c1[pseudo_preset];
    chip->pseudo_c2 = aymo_tda8425_pseudo_preset_c2[pseudo_preset];
}


uint8_t aymo_(read)(struct aymo_(chip)* chip, uint16_t address)
{
    assert(chip);

    switch (address) {
        case 0x00u: {
            return chip->reg_vl;
        }
        case 0x01u: {
            return chip->reg_vr;
        }
        case 0x02u: {
            return chip->reg_ba;
        }
        case 0x03u: {
            return chip->reg_tr;
        }
        case 0x07u: {
            return chip->reg_pp;
        }
        case 0x08u: {
            return chip->reg_sf;
        }
        default: {
            return 0xFFu;
        }
    }
}


void aymo_(write)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    assert(chip);

    switch (address) {
        case 0x00u: {  // VL
            value |= 0xC0u;
            chip->reg_vl = value;
            aymo_(apply_vl)(chip);
            break;
        }
        case 0x01u: {  // VR
            value |= 0xC0u;
            chip->reg_vr = value;
            aymo_(apply_vr)(chip);
            break;
        }
        case 0x02u: {  // BA
            value |= 0xF0u;
            chip->reg_ba = value;
            aymo_(apply_ba)(chip);
            break;
        }
        case 0x03u: {  // TR
            value |= 0xF0u;
            chip->reg_tr = value;
            aymo_(apply_tr)(chip);
            break;
        }
        case 0x07u: {  // PP
            value |= 0xFCu;
            chip->reg_pp = value;
            aymo_(apply_pp)(chip);
            aymo_(apply_pseudo)(chip);
            break;
        }
        case 0x08u: {  // SF
            chip->reg_sf = value;
            aymo_(apply_source_mode)(chip);
            aymo_(apply_pseudo)(chip);
            aymo_(apply_tfilter)(chip);
            aymo_(apply_vl)(chip);
            aymo_(apply_vr)(chip);
            aymo_(apply_tr)(chip);
            break;
        }
    }
}


void aymo_(process_f32)(struct aymo_(chip)* chip, uint32_t count, const float x[], float y[])
{
    assert(chip);
    assert(x);
    assert(y);

    float AYMO_ALIGN_V256 xlrv[8];
    float AYMO_ALIGN_V256 xrlv[8];
    float AYMO_ALIGN_V256 yyv[8];

    vf32x8_t b2 = chip->hb1;
    vf32x8_t a2 = chip->ha1;

    const float* xe = &x[count * 2u];

    while AYMO_LIKELY(x != xe) {
        vf32x8_t y2 = _mm256_add_ps(_mm256_mul_ps(b2, chip->kb2), _mm256_mul_ps(a2, chip->ka2));
        chip->hb2 = b2;
        chip->ha2 = a2;

        vf32x8_t b1 = chip->hb0;
        vf32x8_t a1 = chip->ha0;
        vf32x8_t y1 = _mm256_add_ps(_mm256_mul_ps(b1, chip->kb1), _mm256_mul_ps(a1, chip->ka1));
        chip->hb1 = b1;
        chip->ha1 = a1;

        vf32x8_t yy = _mm256_add_ps(y2, y1);

        xrlv[7] = xlrv[3] = *x++;
        xrlv[3] = xlrv[7] = *x++;
        _mm_sfence();
        vf32x8_t xlr = _mm256_load_ps(xlrv);
        vf32x8_t xrl = _mm256_load_ps(xrlv);
        vf32x8_t xx = _mm256_add_ps(_mm256_mul_ps(xlr, chip->klr), _mm256_mul_ps(xrl, chip->krl));

        vf32x8_t b0 = mm256_alignr_ps(chip->ha0, xx, 3);
        yy = _mm256_add_ps(yy, _mm256_mul_ps(b0, chip->kb0));
        chip->hb0 = b0;

        chip->ha0 = yy;

        yy = _mm256_mul_ps(yy, chip->kv);
        _mm256_store_ps(yyv, yy);

        b2 = chip->hb1;
        a2 = chip->ha1;

        _mm_sfence();
        *y++ = yyv[3];
        *y++ = yyv[7];
    }
}


AYMO_CXX_EXTERN_C_END

#endif  // AYMO_CPU_SUPPORT_X86_AVX2
