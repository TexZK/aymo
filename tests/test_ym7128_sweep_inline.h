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
#include "aymo_testing.h"
#include "aymo_ym7128.h"
#include "YM7128B_emu.h"

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

#ifndef DUTY_RATIO
#define DUTY_RATIO  (.8)
#endif

#ifndef INPUT_AMPLITUDE
#define INPUT_AMPLITUDE (.25)
#endif

#ifndef STDEV_LIMIT
#define STDEV_LIMIT (.1)
#endif


struct test_args {
    int argc;
    char** argv;
    double tt;
    uint8_t regs[YM7128B_Reg_Count];
};


static int app_return;
static struct test_args test_args;

static YM7128B_ChipFixed emu;
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
    aymo_ym7128_boot();

    app_return = TEST_STATUS_PASS;
    memset(&test_args, 0, sizeof(test_args));

    YM7128B_ChipFixed_Ctor(&emu);
}


static void app_setup(void)
{
    YM7128B_ChipFixed_Reset(&emu);
    for (int i = 0; i < YM7128B_Reg_Count; ++i) {
        YM7128B_ChipFixed_Write(&emu, (YM7128B_Address)i, test_args.regs[i]);
    }

    aymo_(ctor)(&chip);
    for (int i = 0; i < AYMO_YM7128_REG_COUNT; ++i) {
        aymo_(write)(&chip, (uint16_t)i, test_args.regs[i]);
    }

#ifdef TEST_FILES
    in_name = aymo_test_args_to_str(0, (test_args.argc - 1), test_args.argv, "", "_in.wav");
    emu_out_name = aymo_test_args_to_str(0, (test_args.argc - 1), test_args.argv, "", "_emu_out.wav");
    chip_out_name = aymo_test_args_to_str(0, (test_args.argc - 1), test_args.argv, "", "_chip_out.wav");
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

    double fs = (double)YM7128B_Input_Rate;
    double T = (test_args.tt * DUTY_RATIO);
    uint32_t N = (uint32_t)fmax(16., (fs * (T / DUTY_RATIO)));
    uint16_t fmt = AYMO_WAVE_FMT_TYPE_PCM;
    struct aymo_wave_heading wavh;
    aymo_wave_heading_setup(&wavh, fmt, 1u, 16u, (uint32_t)YM7128B_Input_Rate, (1u * N));
    fwrite(&wavh, sizeof(wavh), 1u, in_file);
    aymo_wave_heading_setup(&wavh, fmt, 2u, 16u, (uint32_t)YM7128B_Output_Rate, (2u * N));
    fwrite(&wavh, sizeof(wavh), 1u, emu_out_file);
    aymo_wave_heading_setup(&wavh, fmt, 2u, 16u, (uint32_t)YM7128B_Output_Rate, (2u * N));
    fwrite(&wavh, sizeof(wavh), 1u, chip_out_file);
#endif  // TEST_FILES
}


static void app_teardown(void)
{
    aymo_(dtor)(&chip);

#ifdef TEST_FILES
    fclose(in_file);
    fclose(emu_out_file);
    fclose(chip_out_file);
#endif  // TEST_FILES
}


static void app_run(void)
{
    double fs = (double)YM7128B_Input_Rate;
    double T = (test_args.tt * DUTY_RATIO);
    long N = (long)fmax(16., (fs * (T / DUTY_RATIO)));
    double f0 = 10.;
    double f1 = fmin((fs / 2.), 21000.);

    YM7128B_ChipFixed_Process_Data emu_data;
    memset(&emu_data, 0, sizeof(emu_data));
    int16_t chip_x[1] = {0};
    int16_t chip_y[4] = {0};
    double sum_e0l = 0.;
    double sum_ee0l = 0.;
    double sum_e0r = 0.;
    double sum_ee0r = 0.;
    double sum_e1l = 0.;
    double sum_ee1l = 0.;
    double sum_e1r = 0.;
    double sum_ee1r = 0.;
    long k;

    for (k = 0; k < N; ++k) {
        double xx = 0.;
        if ((double)k < ((double)N * DUTY_RATIO)) {
            double t = ((double)k / fs);
            double th = ((2. * M_PI * f0 * T) * (pow((f1 / f0), (t / T)) - 1.) / log(f1 / f0));
            th = fmod(th, (2. * M_PI));
            xx = (INPUT_AMPLITUDE * cos(th));
        }
        int16_t x = (int16_t)(xx * (double)YM7128B_Fixed_Max);

        emu_data.inputs[0] = x;
        chip_x[0] = x;

        YM7128B_ChipFixed_Process(&emu, &emu_data);

        aymo_(process_i16)(&chip, 1u, chip_x, chip_y);

        double e0l = ((double)emu_data.outputs[0][0] - (double)chip_y[0]);
        double e0r = ((double)emu_data.outputs[1][0] - (double)chip_y[1]);
        double e1l = ((double)emu_data.outputs[0][1] - (double)chip_y[2]);
        double e1r = ((double)emu_data.outputs[1][1] - (double)chip_y[3]);
        sum_e0l += e0l; sum_ee0l = (e0l * e0l);
        sum_e0r += e0r; sum_ee0r = (e0r * e0r);
        sum_e1l += e1l; sum_ee1l = (e1l * e1l);
        sum_e1r += e1r; sum_ee1r = (e1r * e1r);

#ifdef TEST_FILES
        if (in_file) {
            fwrite(chip_x, sizeof(int16_t), 1, in_file);
        }
        if (emu_out_file) {
            fwrite(&emu_data.outputs[0][0], sizeof(int16_t), 1, emu_out_file);
            fwrite(&emu_data.outputs[1][0], sizeof(int16_t), 1, emu_out_file);
            fwrite(&emu_data.outputs[0][1], sizeof(int16_t), 1, emu_out_file);
            fwrite(&emu_data.outputs[1][1], sizeof(int16_t), 1, emu_out_file);
        }
        if (chip_out_file) {
            fwrite(chip_y, sizeof(int16_t), 4, chip_out_file);
        }
#endif  // TEST_FILES
    }

    double avg_e0l = (sum_e0l / (double)k);
    double avg_ee0l = (sum_ee0l / (double)k);
    double var_e0l = fabs(avg_ee0l - (avg_e0l * avg_e0l));
    double stdev_e0l = sqrt(var_e0l);
    fprintf(stderr, "L0: stdev_e=%g  N=%ld  k=%ld  sum_e=%g  sum_ee=%g\n", stdev_e0l, N, k, sum_e0l, sum_ee0l);

    double avg_e0r = (sum_e0r / (double)k);
    double avg_ee0r = (sum_ee0r / (double)k);
    double var_e0r = fabs(avg_ee0r - (avg_e0r * avg_e0r));
    double stdev_e0r = sqrt(var_e0r);
    fprintf(stderr, "R0: stdev_e=%g  N=%ld  k=%ld  sum_e=%g  sum_ee=%g\n", stdev_e0r, N, k, sum_e0r, sum_ee0r);

    double avg_e1l = (sum_e1l / (double)k);
    double avg_ee1l = (sum_ee1l / (double)k);
    double var_e1l = fabs(avg_ee1l - (avg_e1l * avg_e1l));
    double stdev_e1l = sqrt(var_e1l);
    fprintf(stderr, "L1: stdev_e=%g  N=%ld  k=%ld  sum_e=%g  sum_ee=%g\n", stdev_e1l, N, k, sum_e1l, sum_ee1l);

    double avg_e1r = (sum_e1r / (double)k);
    double avg_ee1r = (sum_ee1r / (double)k);
    double var_e1r = fabs(avg_ee1r - (avg_e1r * avg_e1r));
    double stdev_e1r = sqrt(var_e1r);
    fprintf(stderr, "R1: stdev_e=%g  N=%ld  k=%ld  sum_e=%g  sum_ee=%g\n", stdev_e1r, N, k, sum_e1r, sum_ee1r);

    double stdev_e = sqrt(var_e0l + var_e0r + var_e1l + var_e1r);
    if (stdev_e > STDEV_LIMIT) {
        app_return = TEST_STATUS_FAIL;
    }
}


int main(int argc, char** argv)
{
    const int argo = 2;

    app_boot();

    test_args.argc = argc;
    test_args.argv = argv;

    if ((argc < argo) || (argc > (argo + AYMO_YM7128_REG_COUNT))) {
        fprintf(stderr, "USAGE:\t%s tt REGn...\n", (argc ? argv[0] : "test_exe"));
        app_return = TEST_STATUS_HARD;
        goto catch_;
    }

    if (arg2posf(argv[1], &test_args.tt)) {
        perror("tt"); app_return = TEST_STATUS_HARD; goto catch_;
    }

    int arge = (argo + AYMO_YM7128_REG_COUNT);
    if (arge > argc) {
        arge = argc;
    }
    for (int i = argo; i < arge; ++i) {
        if (arg2reg(argv[i], &test_args.regs[i - argo])) {
            char text[16]; sprintf(text, "%d", i);
            perror(text); app_return = TEST_STATUS_HARD; goto catch_;
        }
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
