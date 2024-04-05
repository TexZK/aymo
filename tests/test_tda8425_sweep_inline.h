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
#include "aymo_tda8425.h"
#include "aymo_testing.h"
#include "TDA8425_emu.h"

#ifdef TEST_FILES
#include "aymo_wave.h"
#endif

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AYMO_CXX_EXTERN_C_BEGIN


#ifndef M_PI
#define M_PI (3.14159265358979323846264338327950288)
#endif

#ifndef INPUT_AMPLITUDE
#define INPUT_AMPLITUDE (.4)
#endif

#ifndef STDEV_LIMIT
#define STDEV_LIMIT (.0002)
#endif

#define EMU_AHEAD   4

static AYMO_TDA8425_DEFINE_MATH_DEFAULT(tda8425_math);


struct app_args {
    int argc;
    char** argv;
    double fs;
    double tt;
    uint8_t reg_vl;
    uint8_t reg_vr;
    uint8_t reg_ba;
    uint8_t reg_tr;
    uint8_t reg_pp;
    uint8_t reg_sf;
};


static int app_return;
static struct app_args app_args;

static TDA8425_Chip emu;
static struct aymo_(chip) chip;

#ifdef TEST_FILES
static char* in_name;
static char* emu_out_name;
static char* chip_out_name;
static FILE* in_file;
static FILE* emu_out_file;
static FILE* chip_out_file;
#endif  // TEST_FILES


static int arg2reg(const char* arg, uint8_t* reg) {
    errno = 0;
    unsigned long x = strtoul(arg, NULL, 0);
    if ((x != ULONG_MAX) || (errno != ERANGE)) {
        if (x <= UINT8_MAX) {
            *reg = (uint8_t)x;
        } else {
            errno = ERANGE;
        }
    }
    return errno;
}


static int arg2posf(const char* arg, double* val) {
    errno = 0;
    double x = strtod(arg, NULL);
    if (((x != +HUGE_VAL) && (x != -HUGE_VAL)) || (errno != ERANGE)) {
        if (x > 0.) {
            *val = (float)x;
        } else {
            errno = ERANGE;
        }
    }
    return errno;
}


static void app_boot(void)
{
    aymo_cpu_boot();
    aymo_tda8425_boot(&tda8425_math);

    app_return = TEST_STATUS_PASS;
    memset(&app_args, 0, sizeof(app_args));

    TDA8425_Chip_Ctor(&emu);
}


static void app_setup(void)
{
    TDA8425_Chip_Setup(
        &emu,
        (TDA8425_Float)app_args.fs,
        (TDA8425_Float)TDA8425_Pseudo_C1_Table[TDA8425_Pseudo_Preset_1],
        (TDA8425_Float)TDA8425_Pseudo_C2_Table[TDA8425_Pseudo_Preset_1],
        TDA8425_Tfilter_Mode_Disabled
    );
    TDA8425_Chip_Reset(&emu);
    TDA8425_Chip_Write(&emu, (TDA8425_Address)TDA8425_Reg_VL, app_args.reg_vl);
    TDA8425_Chip_Write(&emu, (TDA8425_Address)TDA8425_Reg_VR, app_args.reg_vr);
    TDA8425_Chip_Write(&emu, (TDA8425_Address)TDA8425_Reg_BA, app_args.reg_ba);
    TDA8425_Chip_Write(&emu, (TDA8425_Address)TDA8425_Reg_TR, app_args.reg_tr);
    TDA8425_Chip_Write(&emu, (TDA8425_Address)TDA8425_Reg_PP, app_args.reg_pp);
    TDA8425_Chip_Write(&emu, (TDA8425_Address)TDA8425_Reg_SF, app_args.reg_sf);
    TDA8425_Chip_Start(&emu);

    aymo_(ctor)(&chip, (float)app_args.fs);
    aymo_(write)(&chip, 0x00u, app_args.reg_vl);
    aymo_(write)(&chip, 0x01u, app_args.reg_vr);
    aymo_(write)(&chip, 0x02u, app_args.reg_ba);
    aymo_(write)(&chip, 0x03u, app_args.reg_tr);
    aymo_(write)(&chip, 0x07u, app_args.reg_pp);
    aymo_(write)(&chip, 0x08u, app_args.reg_sf);

#ifdef TEST_FILES
    in_name = aymo_test_args_to_str(0, (app_args.argc - 1), app_args.argv, "", "_in.wav");
    emu_out_name = aymo_test_args_to_str(0, (app_args.argc - 1), app_args.argv, "", "_emu_out.wav");
    chip_out_name = aymo_test_args_to_str(0, (app_args.argc - 1), app_args.argv, "", "_chip_out.wav");
    assert(in_name);
    assert(emu_out_name);
    assert(chip_out_name);
    fprintf(stderr, "in_name:       \"%s\"\n", in_name);
    fprintf(stderr, "emu_out_name:  \"%s\"\n", emu_out_name);
    fprintf(stderr, "chip_out_name: \"%s\"\n", chip_out_name);

    in_file = fopen(in_name, "wb");
    emu_out_file = fopen(emu_out_name, "wb");
    chip_out_file = fopen(chip_out_name, "wb");
    assert(in_file);
    assert(emu_out_file);
    assert(chip_out_file);

    double fs = app_args.fs;
    double T = app_args.tt;
    uint32_t N = (uint32_t)fmax(16., (fs * T));
    uint16_t fmt = AYMO_WAVE_FMT_TYPE_FLOAT;
    struct aymo_wave_heading wavh;
    aymo_wave_heading_setup(&wavh, fmt, 2u, 32u, (uint32_t)app_args.fs, (2u * N));
    fwrite(&wavh, sizeof(wavh), 1u, in_file);
    aymo_wave_heading_setup(&wavh, fmt, 2u, 32u, (uint32_t)app_args.fs, (2u * N));
    fwrite(&wavh, sizeof(wavh), 1u, emu_out_file);
    aymo_wave_heading_setup(&wavh, fmt, 2u, 32u, (uint32_t)app_args.fs, (2u * N));
    fwrite(&wavh, sizeof(wavh), 1u, chip_out_file);
#endif  // TEST_FILES
}


static void app_teardown(void)
{
    TDA8425_Chip_Stop(&emu);
    TDA8425_Chip_Dtor(&emu);

    aymo_(dtor)(&chip);

#ifdef TEST_FILES
    fclose(in_file);
    fclose(emu_out_file);
    fclose(chip_out_file);
#endif  // TEST_FILES
}


static void app_run(void)
{
    double fs = app_args.fs;
    double T = app_args.tt;
    long N = (long)fmax(16., (fs * T));
    double f0 = 10.;
    double f1 = fmin((fs / 2.), 21000.);

    TDA8425_Chip_Process_Data emu_data;
    memset(&emu_data, 0, sizeof(emu_data));
    float emu_y[EMU_AHEAD][2] = {{0}};
    float chip_x[2] = {0};
    float chip_y[2] = {0};
    double sum_el = 0.;
    double sum_eel = 0.;
    double sum_er = 0.;
    double sum_eer = 0.;
    long k;

    for (k = 0; k < N; ++k) {
        double t = ((double)k / fs);
        double th = ((2. * M_PI * f0 * T) * (pow((f1 / f0), (t / T)) - 1.) / log(f1 / f0));
        th = fmod(th, (2. * M_PI));
        float xl = (float)(INPUT_AMPLITUDE * sin(th));
        float xr = (float)(INPUT_AMPLITUDE * cos(th));

        emu_data.inputs[0][0] = (TDA8425_Float)xl;
        emu_data.inputs[0][1] = (TDA8425_Float)xr;
        chip_x[0] = xl;
        chip_x[1] = xr;

        TDA8425_Chip_Process(&emu, &emu_data);
        for (int i = (EMU_AHEAD - 1); i > 0; --i) {
            emu_y[i][0] = emu_y[i-1][0];
            emu_y[i][1] = emu_y[i-1][1];
        }
        emu_y[0][0] = (float)emu_data.outputs[0];
        emu_y[0][1] = (float)emu_data.outputs[1];

        aymo_(process_f32)(&chip, 1u, chip_x, chip_y);

        double el = (emu_y[EMU_AHEAD-1][0] - chip_y[0]);
        double er = (emu_y[EMU_AHEAD-1][1] - chip_y[1]);
        sum_el += el;
        sum_er += er;
        sum_eel = (el * el);
        sum_eer = (er * er);

#ifdef TEST_FILES
        if (in_file) {
            fwrite(chip_x, sizeof(float), 2, in_file);
        }
        if (emu_out_file) {
            fwrite(emu_y[EMU_AHEAD-1], sizeof(float), 2, emu_out_file);
        }
        if (chip_out_file) {
            fwrite(chip_y, sizeof(float), 2, chip_out_file);
        }
#endif  // TEST_FILES
    }

    double avg_el = (sum_el / (double)k);
    double avg_er = (sum_er / (double)k);
    double avg_eel = (sum_eel / (double)k);
    double avg_eer = (sum_eer / (double)k);
    double var_el = fabs(avg_eel - (avg_el * avg_el));
    double var_er = fabs(avg_eer - (avg_er * avg_er));
    double stdev_el = sqrt(var_el);
    double stdev_er = sqrt(var_er);

    fprintf(stderr, "L: stdev_e=%g  N=%ld  k=%ld  sum_e=%g  sum_ee=%g\n", stdev_el, N, k, sum_el, sum_eel);
    fprintf(stderr, "R: stdev_e=%g  N=%ld  k=%ld  sum_e=%g  sum_ee=%g\n", stdev_er, N, k, sum_er, sum_eer);

    if ((stdev_el > STDEV_LIMIT) || (stdev_er > STDEV_LIMIT)) {
        app_return = TEST_STATUS_FAIL;
    }
}


int main(int argc, char** argv)
{
    app_boot();

    app_args.argc = argc;
    app_args.argv = argv;

    if (argc != 9) {
        fprintf(stderr, "USAGE:\t%s VL VR BA TR PP SF fs tt\n", (argc ? argv[0] : "test_exe"));
        app_return = TEST_STATUS_HARD;
        goto catch_;
    }

    if (arg2reg(argv[1], &app_args.reg_vl)) {
        perror("VL"); app_return = TEST_STATUS_HARD; goto catch_;
    }
    if (arg2reg(argv[2], &app_args.reg_vr)) {
        perror("VR"); app_return = TEST_STATUS_HARD; goto catch_;
    }
    if (arg2reg(argv[3], &app_args.reg_ba)) {
        perror("BA"); app_return = TEST_STATUS_HARD; goto catch_;
    }
    if (arg2reg(argv[4], &app_args.reg_tr)) {
        perror("TR"); app_return = TEST_STATUS_HARD; goto catch_;
    }
    if (arg2reg(argv[5], &app_args.reg_pp)) {
        perror("PP"); app_return = TEST_STATUS_HARD; goto catch_;
    }
    if (arg2reg(argv[6], &app_args.reg_sf)) {
        perror("SF"); app_return = TEST_STATUS_HARD; goto catch_;
    }
    if (arg2posf(argv[7], &app_args.fs)) {
        perror("fs"); app_return = TEST_STATUS_HARD; goto catch_;
    }
    if (arg2posf(argv[8], &app_args.tt)) {
        perror("tt"); app_return = TEST_STATUS_HARD; goto catch_;
    }

    app_setup();
    app_run();
    goto finally_;

catch_:
finally_:
    app_teardown();
    return app_return;
}


AYMO_CXX_EXTERN_C_END
